// Copyright 2020-2023 Bloomberg Finance L.P.
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <ntcs_skiplist.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_skiplist_cpp, "$Id$ $CSID$")

#include <bdlb_random.h>
#include <bdlma_infrequentdeleteblocklist.h>

#include <bslma_allocator.h>
#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsls_log.h>

#include <bsl_algorithm.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace ntcs {

SkipList_RandomLevelGenerator::SkipList_RandomLevelGenerator()
: d_seed(k_SEED)
, d_randomBits(1)
{
}

int SkipList_RandomLevelGenerator::randomLevel()
{
    // This routine is "thread-safe enough".

    int level = 0;
    int b;

    do {
        if (1 == d_randomBits) {
            // Only the sentinel bit left.  Regenerate.

            int seed     = d_seed;
            d_randomBits = bdlb::Random::generate15(&seed);
            d_seed       = seed;
            BSLS_ASSERT((d_randomBits >> 15) == 0);

            d_randomBits |= (1 << 14);  // Set the sentinel bit.
        }

        b            = d_randomBits & 3;
        level        += !b;
        d_randomBits >>= 2;

    } while (!b);

    return level > SkipListConsts::k_MAX_LEVEL ? SkipListConsts::k_MAX_LEVEL
                                               : level;
}
}  // close package namespace

namespace ntcs {

/// Operate a set of 'k_MAX_POOLS' memory pools, each of which allocates
/// memory chunks of a certain size.
class SkipList_PoolManager
{
    enum {
        k_MAX_POOLS = SkipListConsts::k_MAX_NUM_LEVELS,

        k_INITIAL_NUM_OBJECTS_TO_ALLOCATE = 1,

        k_GROWTH_FACTOR = 2
    };

    struct Node {
        Node* d_next_p;
    };

    struct Pool {
        Node* d_freeList_p;
        int   d_objectSize;
        int   d_numObjectsToAllocate;
        int   d_level;
    };

    // DATA
    bdlma::InfrequentDeleteBlockList d_blockList;  // supplies free memory

    Pool d_pools[k_MAX_POOLS];

  private:
    SkipList_PoolManager(const SkipList_PoolManager&) BSLS_KEYWORD_DELETED;
    SkipList_PoolManager& operator=(const SkipList_PoolManager&)
        BSLS_KEYWORD_DELETED;

  public:
    // CLASS METHOD

    /// Cast the specified pointer 'p' to 'Node *'.
    template <class TYPE>
    static Node* toNode(TYPE* p);

    /// Create a pool manager having the specified 'numPools' pools, with
    /// the specified 'objectSizes' being an array of 'numPools' object
    /// sizes for the respective pools.  Use the specified 'basicAllocator'
    /// for memory allocation.  The behavior is undefined if
    /// 'numPools > k_MAX_POOLS'.
    explicit SkipList_PoolManager(int*              objectSizes,
                                  int               numPools,
                                  bslma::Allocator* basicAllocator);

    /// d'tor
    ~SkipList_PoolManager();

  public:
    // pool manipulators

    /// Allocate a standard chunk of memory from the specified 'pool'.
    /// Return a pointer to the allocated node.
    void* allocate(Pool* pool);

    /// Free the specified 'node' and return it to the list of the specified
    /// 'pool'.
    void deallocate(Pool* pool, void* node);

    /// Initialize the specified 'pool' with the specified 'level' and
    /// 'objectSize'.  Note that we don't want to make this a constructor
    /// because the 'pool' objects are created in an array and we want to
    /// pass 'level' and 'objectSize' at initialization.
    void initPool(Pool* pool, int level, int objectSize);

    /// Allocate a new block for the specified 'pool'.
    void replenish(Pool* pool);

    // pool manager manipulators

    /// Allocate and return a node of the size appropriate for the specified
    /// 'level'.
    void* allocate(int level);

    /// Free the specified 'node' which has specified 'level' and return it
    /// to its appropriate pool.  Behaviour is undefined if 'node' does not
    /// have the level 'level'
    void deallocate(void* node, int level);
};

// SkipList_PoolManager

// CLASS METHOD
template <class TYPE>
SkipList_PoolManager::Node* SkipList_PoolManager::toNode(TYPE* p)
{
    return static_cast<Node*>(static_cast<void*>(p));
}

SkipList_PoolManager::SkipList_PoolManager(int*              objectSizes,
                                           int               numPools,
                                           bslma::Allocator* basicAllocator)
: d_blockList(basicAllocator)
{
    BSLS_ASSERT(numPools > 0);
    BSLS_ASSERT(numPools <= k_MAX_POOLS);

    // sanity-check

    for (int i = 0; i < numPools; ++i) {
        initPool(&d_pools[i], i, objectSizes[i]);
    }
}

inline SkipList_PoolManager::~SkipList_PoolManager()
{
}

// pool manipulators

void* SkipList_PoolManager::allocate(Pool* pool)
{
    if (!pool->d_freeList_p) {
        replenish(pool);
    }

    Node* p = pool->d_freeList_p;
    BSLS_ASSERT(p);

    pool->d_freeList_p = p->d_next_p;

    return p;
}

void SkipList_PoolManager::deallocate(Pool* pool, void* node)
{
    Node* p            = static_cast<Node*>(node);
    p->d_next_p        = pool->d_freeList_p;
    pool->d_freeList_p = p;
}

inline void SkipList_PoolManager::initPool(Pool* pool,
                                           int   level,
                                           int   objectSize)
{
    pool->d_freeList_p           = 0;
    pool->d_objectSize           = objectSize;
    pool->d_numObjectsToAllocate = k_INITIAL_NUM_OBJECTS_TO_ALLOCATE;
    pool->d_level                = level;
}

void SkipList_PoolManager::replenish(Pool* pool)
{
    BSLS_ASSERT(0 == pool->d_freeList_p);

    int objectSize = pool->d_objectSize;
    int numObjects = pool->d_numObjectsToAllocate;

    BSLS_ASSERT(0 < objectSize);
    BSLS_ASSERT(0 < numObjects);

    char* start =
        static_cast<char*>(d_blockList.allocate(numObjects * objectSize));

    char* end  = start + (numObjects - 1) * objectSize;
    Node* last = toNode(end);
    for (char* p = start; p < end; p += objectSize) {
        Node* n     = toNode(p);
        n->d_next_p = toNode(p + objectSize);
    }
    last->d_next_p = pool->d_freeList_p;

    pool->d_freeList_p = toNode(start);

    pool->d_numObjectsToAllocate *= k_GROWTH_FACTOR;
}

// pool manager manipulators

inline void* SkipList_PoolManager::allocate(int level)
{
    return allocate(&d_pools[level]);
}

inline void SkipList_PoolManager::deallocate(void* node, int level)
{
    deallocate(&d_pools[level], node);
}

void* SkipList_PoolUtil::allocate(PoolManager* poolManager, int level)
{
    return poolManager->allocate(level);
}

SkipList_PoolManager* SkipList_PoolUtil::createPoolManager(
    int*              objectSizes,
    int               numLevels,
    bslma::Allocator* basicAllocator)
{
    return new (*basicAllocator)
        PoolManager(objectSizes, numLevels, basicAllocator);
}

void SkipList_PoolUtil::deallocate(PoolManager* poolManager,
                                   void*        address,
                                   int          level)
{
    poolManager->deallocate(address, level);
}

void SkipList_PoolUtil::deletePoolManager(bslma::Allocator* basicAllocator,
                                          PoolManager*      poolManager)
{
    basicAllocator->deleteObject(poolManager);
}

}  // close package namespace
}  // close enterprise namespace
