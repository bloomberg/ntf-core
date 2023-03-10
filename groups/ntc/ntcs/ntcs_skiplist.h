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

#ifndef INCLUDED_NTCS_SKIPLIST
#define INCLUDED_NTCS_SKIPLIST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntcscm_version.h>

#include <bdlb_print.h>
#include <bdlb_printmethods.h>

#include <bslalg_scalarprimitives.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_types.h>
#include <bsls_util.h>

#include <bsl_algorithm.h>
#include <bsl_functional.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntcs {

template <class KEY, class DATA, class RANDGEN>
class SkipList;

// local class SkipList_Node

/// @internal @brief
/// Describe a node in a skip list.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
template <class KEY, class DATA>
struct SkipList_Node {
    typedef SkipList_Node<KEY, DATA> Node;

    struct Ptrs {
        Node* d_next_p;
        Node* d_prev_p;
    };

    int  d_level;  // values in range '[ 0 .. 31 ]'
    DATA d_data;
    KEY  d_key;
    Ptrs d_ptrs[1];  // Must be last; each node has space for extra 'Ptrs'
                     // allocated based on its level
};

/// @internal @brief
/// Defines constants used in the implementation of skip lists.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
struct SkipListConsts {
    enum { k_MAX_LEVEL = 31, k_MAX_NUM_LEVELS = k_MAX_LEVEL + 1 };
};

// local class SkipList_RandomLevelGenerator

/// @internal @brief
/// Provide a mechanism to generate a random level in a skip list.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class SkipList_RandomLevelGenerator
{
    enum {
        k_SEED = 0x12b9b0a1  // arbitrary
    };

    // DATA
    int d_seed;  // current random seed

    int d_randomBits;  // 14 random bits and a sentinel bit at the 15th
                       // position

  public:
    /// Construct a random-level generator.
    SkipList_RandomLevelGenerator();

    /// Return a random integer between 0 and k_MAX_LEVEL.
    int randomLevel();
};

// local class bdlcc::SkipList_PoolUtil

class SkipList_PoolManager;

/// @internal @brief
/// Provide utilities for allocating skip list nodes.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
struct SkipList_PoolUtil {
    typedef SkipList_PoolManager PoolManager;

    /// Reserve sufficient space for a node at the specified 'level' from
    /// the specified 'poolManager', and return the address of the reserved
    /// memory.
    static void* allocate(PoolManager* poolManager, int level);

    /// Create a new pooled node allocator that manages nodes up to the
    /// specified 'numLevels' as described by the specified 'objectSizes'.
    /// For 'i' in '[0, numLevels)', a node at level 'i' will have size
    /// 'objectSizes[i]' bytes.  Use the specified 'basicAllocator' to
    /// supply memory.  Return the address of the new allocator.  Note that
    /// the behavior is undefined if 'basicAllocator' is 0.
    static PoolManager* createPoolManager(int*              objectSizes,
                                          int               numLevels,
                                          bslma::Allocator* basicAllocator);

    /// Return the node having specified 'level' at the specified 'address'
    /// to the specified 'poolManager'.  The behavior is undefined if
    /// 'address' was not allocated from 'poolManager' or 'level' does not
    /// match level of the node
    static void deallocate(PoolManager* poolManager, void* address, int level);

    /// Destroy the specified 'poolManager' which was allocated from the
    /// specified 'basicAllocator'.  The behavior is undefined if
    /// 'poolManager' was not allocated from 'basicAllocator'.
    static void deletePoolManager(bslma::Allocator* basicAllocator,
                                  PoolManager*      poolManager);
};

// local class SkipList_NodeCreationHelper

/// @internal @brief
/// Provide a guard to initialize new nodes in a skip list and automatically
/// release them in the case of an exception.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
template <class KEY, class DATA>
class SkipList_NodeCreationHelper
{
    typedef SkipList_PoolManager PoolManager;
    typedef SkipList_PoolUtil    PoolUtil;

    typedef SkipList_Node<KEY, DATA> Node;

    // DATA
    Node*             d_node_p;         // the node, or 0 if no managed node
    PoolManager*      d_poolManager_p;  // pool from which node was allocated
    bool              d_keyFlag;        // 'true' if the key was constructed
    bslma::Allocator* d_allocator_p;    // held

    BSLMF_NESTED_TRAIT_DECLARATION(SkipList_NodeCreationHelper,
                                   bslma::UsesBslmaAllocator);

  public:
    /// Create a new scoped guard object to assist in exception-safe
    /// initialization of the specified 'node', which was allocated from the
    /// specified 'poolManager'.  Optionally specify a 'basicAllocator' used
    /// to supply memory.  If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    SkipList_NodeCreationHelper(PoolManager*      poolManager,
                                Node*             node,
                                bslma::Allocator* basicAllocator = 0);

    /// Destroy this scoped guard.  If the guard currently manages a node,
    /// destroy its data as necessary and return it to the pool.
    ~SkipList_NodeCreationHelper();

    /// Return the allocator used by this object to supply memory.
    bslma::Allocator* allocator() const;

    /// Attempt to copy-construct the specified 'key' and 'data' into the
    /// node specified at construction; then release the node from
    /// management.  Note that if an exception is thrown during the
    /// invocation of either constructor, the node will remain under
    /// management and thus the destructor of this object will do the
    /// appropriate cleanup.  The behavior is undefined if 'construct' has
    /// already been invoked on this scoped guard object.
    void construct(const KEY& key, const DATA& data);
};

/// @internal @brief
/// Describe a node in a skip list.
///
/// @details
/// Pointers to objects of this class are used in the API of 'ntcs::SkipList',
/// however, objects of the class are never constructed as the class serves
/// only to provide type-safe pointers.
///
/// In addition, this class defines 'key' and 'data' member functions that
/// pass 'this' to static methods of 'SkipList'.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
template <class KEY, class DATA>
class SkipListPair
{
    // Note these data elements are never accessed.  A pointer to this type
    // will be cast to a pointer to 'SkipList_Node' so make sure we are
    // adequately aligned to avoid compiler warnings.

    // DATA
    SkipList_Node<KEY, DATA> d_node;  // never directly accessed

  private:
    /// Constructor. No way to construct a pair
    SkipListPair() BSLS_KEYWORD_DELETED;

    /// Forbidden copy-constructor
    SkipListPair(const SkipListPair&) BSLS_KEYWORD_DELETED;

    /// Forbidden assignment operator
    SkipListPair& operator=(const SkipListPair&) BSLS_KEYWORD_DELETED;

  public:
    /// Return a reference to the modifiable "data" of this pair.
    DATA& data() const;

    /// Return a reference to the non-modifiable "key" value of this pair.
    const KEY& key() const;
};

/// @internal @brief
/// Provide an associative container implemented by a skip list.
///
/// @details
/// Provide an implementation of a skip list data structure.  This
/// implementation is based on (almost borrowed) bdlcc_skiplist and it is
/// written to be used for ntcs::Chronology to store ordered sequence of timer
/// deadlines.  Because of that the API is restricted to only necessary
/// minimum.  Also, in comparison with bdlcc::Skiplist, this component is not
/// thread safe at all (and thus it's faster).
///
/// SkipList is a randomized data structure. SkipList has two main properties:
/// length and level (listLevel in this description).  length of SkipList is a
/// number of its nodes and listLevel is a maximum node level which has ever
/// been allocated.  So it can be increased with each new node allocated.
/// listLevel can become 0 again only after removal of all nodes via removeAll
/// method.  Each new node is assigned randomly with a level (nodeLevel in this
/// description).  Maximum nodeLevel number is restricted with
/// k_SKIPLIST_MAX_LEVEL constant.  It is assumed that
/// SkipList_RandomLevelGenerator generates a value (level candidate) in [0;
/// k_SKIPLIST_MAX_LEVEL].  If this candidate value is bigger than listLevel
/// then nodeLevel is assigned with candidateValue + 1 and listLevel is
/// incremented by 1. The list guarantees that if there are items with equal
/// keys then their order in the list is the same as the order in which they
/// were added to the list
///
/// Differences from bdlcc_skiplist (except the lack of thread safety and
/// restricted API are: 1) addL method of ntcs_skiplist uses
/// lookupImpUpperBoundL while bdlcc_skiplist uses lookupImpLowerBound. It
/// means that given an existing list of key-value pairs [-1 0] [2 0] [2 1] [2
/// 2] [3 0] addition of an new pair [2 3] for ntcs_skiplist will make it [-1
/// 0] [2 0] [2 1] [2 2] _[2 3]_ [3 0] while for bdlcc_skipist it will become
/// [-1 0] _[2 3]_ [2 0] [2 1] [2 2] [3 0]. 2) removeAll() method of
/// ntcs_skiplist resets list level (while for bdlcc_skiplist it does not)
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
template <class KEY, class DATA, class RANDGEN = SkipList_RandomLevelGenerator>
class SkipList
{
#ifndef SKIPLIST_UNIT_TEST_BUILD
    BSLMF_ASSERT(
        (bsl::is_same<RANDGEN, SkipList_RandomLevelGenerator>::value));
#endif

  public:
    enum { e_SUCCESS = 0, e_NOT_FOUND = 1, e_INVALID = 2 };

    typedef SkipListPair<KEY, DATA> Pair;

  private:
    typedef SkipList_PoolManager PoolManager;
    typedef SkipList_PoolUtil    PoolUtil;

    typedef SkipList_Node<KEY, DATA>               Node;
    typedef SkipList_NodeCreationHelper<KEY, DATA> NodeGuard;

    // DATA
    RANDGEN d_rand;

    int d_listLevel;
    int d_length;

#ifdef SKIPLIST_UNIT_TEST_BUILD
    mutable int
        d_numberOfSteps;  // this value is used only in tests to validate
                          // number of lookup steps
#endif

    Node* d_head_p;
    Node* d_tail_p;

    PoolManager* d_poolManager_p;  // owned

    bslma::Allocator* d_allocator_p;  // held

    // FRIENDS
    friend class SkipListPair<KEY, DATA>;

    /// Return a non-'const' reference to the "data" value of the pair
    /// identified by the specified 'reference'.
    static DATA& data(const Pair* reference);

    /// Return a 'const' reference to the "key" value of the pair identified
    /// by the specified 'reference'.
    static const KEY& key(const Pair* reference);

    /// Return the offset in bytes of 'd_ptrs' from the start of the
    /// 'SkipList_Node' struct.  (similar to
    /// 'offsetof(SkipList_Node, d_ptrs)' but with no requirement that
    /// 'DATA' or 'KEY' be PODs)
    static inline BSLS_KEYWORD_CONSTEXPR bsls::Types::IntPtr offsetOfPtrs();

    /// Const-cast the specified 'reference' to a 'Node *'.
    static Node* pairToNode(const Pair* reference);

    /// Add the specified 'newNode' to the list.  Search for the correct
    /// position for 'newNode' from the start of the list (in acsending
    /// order by key value).  If specified 'newFrontFlag' is not 0, load
    /// into it a 'true' value if the node is at the front of the list, and
    /// a 'false' value otherwise.
    void addNodeL(bool* newFrontFlag, Node* newNode);

    /// Add the specified 'newNode' to the list.  Search for the correct
    /// position for 'newNode' from the back of the list (in descending
    /// order by key value).  If specified 'newFrontFlag' is not 0, load
    /// into it a 'true' value if the node is at the front of the list, and
    /// a 'false' value otherwise.
    void addNodeR(bool* newFrontFlag, Node* newNode);

    /// Allocate a node from the node pool of this list, and set its key
    /// value to the specified 'key' and data value to the specified 'data'.
    /// Set the node's level to the specified 'level' if 'level' is less
    /// than or equal to the highest level of any node previously in the
    /// list, or to one greater than that value otherwise.  Return the
    /// allocated node
    Node* allocateNode(int level, const KEY& key, const DATA& data);

    /// Populate the members of a new Skip List.  This private manipulator
    /// must be called only once, by the constructor.
    void initialize();

    /// Insert the specified 'node' into this list immediately before the
    /// specified 'location' (which is populated by 'lookupImpLowerBoundR')
    /// Load into the specified 'newFrontFlag' a 'true' value if the node is
    /// inserted at the front of the list, and 'false' otherwise
    void insertImp(bool* newFrontFlag, Node* location[], Node* node);

    /// Insert the specified 'node' into this list immediately before the
    /// specified 'location' (which is populated by 'lookupImpUpperBoundR')
    /// Load into the specified 'newFrontFlag' a 'true' value if the node is
    /// inserted at the front of the list, and 'false' otherwise
    ///
    /// Like 'insertImp', but 'node' must already be present in the list
    void moveImp(bool* newFrontFlag, Node* location[], Node* node);

    /// Destroy specified 'node' and return it to the pool
    void releaseNode(Node* node);

    /// Remove the specified 'node' from the list.  Return 0 on success, and
    /// 'e_NOT_FOUND' if the 'node' is no longer in the list.
    int removeNode(Node* node);

    /// Move the specified 'node' to the correct position for the specified
    /// 'newKey'.  The search for the correct location for 'newKey' proceeds
    /// from the back of the list in descending order by by key value.
    /// Update the key value of 'node' to the 'newKey' value.  If the
    /// specified 'newFrontFlag' is not 0, load into it a 'true' value if
    /// the new location of the node is the front of the list, and a 'false'
    /// value otherwise.  If there are multiple instances of 'newKey' in
    /// the list after the update, the updated node will be the *last* node
    /// with the key 'newKey'.  Return 0 on success, 'e_NOT_FOUND' if the
    /// node is no longer in the list.
    int updateNodeR(bool* newFrontFlag, Node* node, const KEY& newKey);

    /// Return the node at the front of the list, or 0 if the list is empty.
    Node* frontNode() const;

    /// Populate the specified 'location' array with the first node whose
    /// key is greater than the specified 'key' at each level in the list,
    /// found by searching the list from the back (in descending order of
    /// key value); if no such node exists at a given level, the
    /// tail-of-list sentinel is populated for that level
    void lookupImpUpperBoundL(Node* location[], const KEY& key) const;

    /// Populate the specified 'location' array with the first node whose
    /// key is greater than the specified 'key' at each level in the list,
    /// found by searching the list from the back (in descending order of
    /// key value); if no such node exists at a given level, the
    /// tail-of-list sentinel is populated for that level
    void lookupImpUpperBoundR(Node* location[], const KEY& key) const;

  private:
    SkipList& operator=(const SkipList& rhs) BSLS_KEYWORD_DELETED;
    SkipList(const SkipList&) BSLS_KEYWORD_DELETED;

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(SkipList, bslma::UsesBslmaAllocator);

    /// Create a new Skip List.  Optionally specify a 'basicAllocator' used
    /// to supply memory.  If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit SkipList(bslma::Allocator* basicAllocator = 0);

    /// Destroy this Skip List.  The behavior is undefined if references are
    /// outstanding to any pairs in the list.
    ~SkipList();

    /// Add the specified 'key' / 'data' pair to this list, and return a
    /// reference to the pair in the list.  Search for the correct position
    /// for 'key' from the start of the list (in ascending order by key
    /// value).  Load into the optionally specified 'newFrontFlag' a 'true'
    /// value if the pair is at the front of the list, and a 'false' value
    /// otherwise.
    Pair* addL(const KEY& key, const DATA& data, bool* newFrontFlag = 0);

    /// Add the specified 'key' / 'data' pair to this list, and return a
    /// reference to the pair in the list.  Search for the correct position
    /// for 'key' from the back of the list (in descending order by key
    /// value).  Load into the optionally specified 'newFrontFlag' a 'true'
    /// value if the pair is at the front of the list, and a 'false' value
    /// otherwise.
    Pair* addR(const KEY& key, const DATA& data, bool* newFrontFlag = 0);

    /// Remove the item identified by the specified 'reference' from the
    /// list.  Return 0 on success, and a non-zero value if the pair has
    /// already been removed from the list.
    int remove(const Pair* reference);

    /// Remove all nodes from the list.  After this method the list has
    /// level and length both equal 0
    int removeAll();

    /// Assign the specified 'newKey' value to the pair identified by the
    /// specified 'reference', moving the pair within the list as necessary.
    /// Search for the new position from the back of the list (in descending
    /// order by key value).  Load into the optionally specified
    /// 'newFrontFlag' a 'true' value if the new location of the pair is the
    /// front of the list.  Return 0 on success, 'e_NOT_FOUND' if the pair
    /// referred to by 'reference' is no longer in the list.
    int updateR(const Pair* reference,
                const KEY&  newKey,
                bool*       newFrontFlag = 0);

    /// This function is normally never called -- it is useful in debugging
    void checkInvariants() const;

    /// Return a reference to the first item in the list.  If the list is
    /// empty then 0 is returned
    Pair* front() const;

    /// Return current list level
    int getListLevel() const;

#ifdef SKIPLIST_UNIT_TEST_BUILD
    int getNumberOfSteps() const;
    // Return number of steps done using lookupImpUpperBoundR method
    // This method is assumed to exist only for testing purposes
#endif

    /// Return 'true' if this list is empty, and 'false' otherwise.
    bool isEmpty() const;

    /// Return the number of items in this list.
    int length() const;

    /// Format this list object to the specified output 'stream' at the
    /// (absolute value of) the optionally specified indentation 'level' and
    /// return a reference to 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  If 'level' is negative,
    /// suppress indentation of the first line.  If 'spacesPerLevel' is
    /// negative, suppress all indentation AND format the entire output on
    /// one line.  If 'stream' is not valid on entry, this operation has no
    /// effect.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// If the item identified by the specified 'item' is not at the end of
    /// the list, load a reference to the next item in the list into 'item';
    /// otherwise reset the value of 'item'.  Return 0 on success, and
    /// 'e_NOT_FOUND' (with no effect on the value of 'item') if 'item' is
    /// no longer in the list.
    int skipForward(Pair** item) const;

    // Aspects

    /// Return the allocator used by this object to supply memory.
    bslma::Allocator* allocator() const;
};

// FREE OPERATORS

/// Write the specified 'list' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
template <class KEY, class DATA, class RANDGEN>
bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const SkipList<KEY, DATA, RANDGEN>& list);

//                            INLINE DEFINITIONS

template <class KEY, class DATA>
inline DATA& SkipListPair<KEY, DATA>::data() const
{
    return SkipList<KEY, DATA>::data(this);
}

template <class KEY, class DATA>
inline const KEY& SkipListPair<KEY, DATA>::key() const
{
    return SkipList<KEY, DATA>::key(this);
}

}  // close package namespace

namespace ntcs {

template <class KEY, class DATA>
inline SkipList_NodeCreationHelper<KEY, DATA>::SkipList_NodeCreationHelper(
    PoolManager*      poolManager,
    Node*             node,
    bslma::Allocator* basicAllocator)
: d_node_p(node)
, d_poolManager_p(poolManager)
, d_keyFlag(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class KEY, class DATA>
inline SkipList_NodeCreationHelper<KEY, DATA>::~SkipList_NodeCreationHelper()
{
    if (d_node_p) {
        if (d_keyFlag) {
            d_node_p->d_key.~KEY();
        }
        PoolUtil::deallocate(d_poolManager_p, d_node_p, d_node_p->d_level);
    }
}

template <class KEY, class DATA>
inline bslma::Allocator* SkipList_NodeCreationHelper<KEY, DATA>::allocator()
    const
{
    return d_allocator_p;
}

template <class KEY, class DATA>
inline void SkipList_NodeCreationHelper<KEY, DATA>::construct(const KEY&  key,
                                                              const DATA& data)
{
    BSLS_ASSERT(d_node_p);

    bslalg::ScalarPrimitives::copyConstruct(
        BSLS_UTIL_ADDRESSOF(d_node_p->d_key),
        key,
        d_allocator_p);
    d_keyFlag = true;

    bslalg::ScalarPrimitives::copyConstruct(
        BSLS_UTIL_ADDRESSOF(d_node_p->d_data),
        data,
        d_allocator_p);

    d_node_p = 0;
}

template <class KEY, class DATA, class RANDGEN>
inline DATA& SkipList<KEY, DATA, RANDGEN>::data(const Pair* reference)
{
    BSLS_ASSERT(reference);

    Node* node =
        static_cast<Node*>(static_cast<void*>(const_cast<Pair*>(reference)));
    return node->d_data;
}

template <class KEY, class DATA, class RANDGEN>
inline const KEY& SkipList<KEY, DATA, RANDGEN>::key(const Pair* reference)
{
    BSLS_ASSERT(reference);

    const Node* node =
        static_cast<const Node*>(static_cast<const void*>(reference));
    return node->d_key;
}

template <class KEY, class DATA, class RANDGEN>
inline BSLS_KEYWORD_CONSTEXPR bsls::Types::IntPtr SkipList<
    KEY,
    DATA,
    RANDGEN>::offsetOfPtrs()
{
    typedef bsls::Types::IntPtr IntPtr;

    // The null pointer dereference is just used for taking offsets and sizes
    // in the 'Node' struct.  Note that we don't want to just create a default
    // constructed 'Node', because if 'KEY' or 'DATA' lack default constructors
    // then 'Node' has no default constructor.

    return reinterpret_cast<IntPtr>(&reinterpret_cast<Node*>(0)->d_ptrs);
}

template <class KEY, class DATA, class RANDGEN>
inline typename SkipList<KEY, DATA, RANDGEN>::Node* SkipList<
    KEY,
    DATA,
    RANDGEN>::pairToNode(const Pair* reference)
{
    return static_cast<Node*>(
        static_cast<void*>(const_cast<Pair*>(reference)));
}

template <class KEY, class DATA, class RANDGEN>
inline void SkipList<KEY, DATA, RANDGEN>::addNodeL(bool* newFrontFlag,
                                                   Node* newNode)
{
    BSLS_ASSERT(newNode);
    BSLS_ASSERT(0 == newNode->d_ptrs[0].d_next_p);

    Node* location[SkipListConsts::k_MAX_NUM_LEVELS];
    lookupImpUpperBoundL(location, newNode->d_key);

    insertImp(newFrontFlag, location, newNode);
}

template <class KEY, class DATA, class RANDGEN>
inline void SkipList<KEY, DATA, RANDGEN>::addNodeR(bool* newFrontFlag,
                                                   Node* newNode)
{
    BSLS_ASSERT(newNode);
    BSLS_ASSERT(0 == newNode->d_ptrs[0].d_next_p);

    Node* location[SkipListConsts::k_MAX_NUM_LEVELS];
    lookupImpUpperBoundR(location, newNode->d_key);

    insertImp(newFrontFlag, location, newNode);
}

template <class KEY, class DATA, class RANDGEN>
SkipList_Node<KEY, DATA>* SkipList<KEY, DATA, RANDGEN>::allocateNode(
    int         level,
    const KEY&  key,
    const DATA& data)
{
    int listLevel = d_listLevel;
    if (level > listLevel) {
        level = listLevel + 1;
    }

    Node* node =
        reinterpret_cast<Node*>(PoolUtil::allocate(d_poolManager_p, level));
    node->d_level = level;

    NodeGuard nodeGuard(d_poolManager_p, node, d_allocator_p);

    nodeGuard.construct(key, data);

    node->d_ptrs[0].d_next_p = 0;

    return node;
}

template <class KEY, class DATA, class RANDGEN>
void SkipList<KEY, DATA, RANDGEN>::initialize()
{
    typedef bsls::Types::IntPtr IntPtr;

    static const int alignMask = bsls::AlignmentFromType<Node>::VALUE - 1;

    // Assert that this method has not been invoked.

    BSLS_ASSERT(0 == d_poolManager_p);

    int nodeSizes[SkipListConsts::k_MAX_NUM_LEVELS];

    const IntPtr offset = offsetOfPtrs();
    for (int i = 0; i < SkipListConsts::k_MAX_NUM_LEVELS; ++i) {
        IntPtr nodeSize = offset + (i + 1) * sizeof(typename Node::Ptrs);
        nodeSize        = (nodeSize + alignMask) & ~alignMask;
        nodeSizes[i]    = static_cast<int>(nodeSize);
    }

    d_poolManager_p =
        PoolUtil::createPoolManager(nodeSizes,
                                    SkipListConsts::k_MAX_NUM_LEVELS,
                                    d_allocator_p);

    d_head_p = reinterpret_cast<Node*>(
        PoolUtil::allocate(d_poolManager_p, SkipListConsts::k_MAX_LEVEL));
    d_head_p->d_level = SkipListConsts::k_MAX_LEVEL;
    d_tail_p          = reinterpret_cast<Node*>(
        PoolUtil::allocate(d_poolManager_p, SkipListConsts::k_MAX_LEVEL));
    d_tail_p->d_level = SkipListConsts::k_MAX_LEVEL;

    for (int i = 0; i < SkipListConsts::k_MAX_NUM_LEVELS; ++i) {
        d_head_p->d_ptrs[i].d_prev_p = 0;
        d_head_p->d_ptrs[i].d_next_p = d_tail_p;

        d_tail_p->d_ptrs[i].d_prev_p = d_head_p;
        d_tail_p->d_ptrs[i].d_next_p = 0;
    }
}

template <class KEY, class DATA, class RANDGEN>
void SkipList<KEY, DATA, RANDGEN>::insertImp(bool* newFrontFlag,
                                             Node* location[],
                                             Node* node)
{
    BSLS_ASSERT(location);
    BSLS_ASSERT(node);

    int level = node->d_level;
    if (level > d_listLevel) {
        BSLS_ASSERT(level == d_listLevel + 1);

        d_listLevel = level;

        node->d_ptrs[level].d_prev_p = d_head_p;
        node->d_ptrs[level].d_next_p = d_tail_p;

        d_head_p->d_ptrs[level].d_next_p = node;
        d_tail_p->d_ptrs[level].d_prev_p = node;

        level--;
    }

    for (int k = level; k >= 0; --k) {
        Node* p = location[k]->d_ptrs[k].d_prev_p;
        Node* q = location[k];

        node->d_ptrs[k].d_prev_p = p;
        node->d_ptrs[k].d_next_p = q;

        p->d_ptrs[k].d_next_p = node;
        q->d_ptrs[k].d_prev_p = node;
    }

    if (newFrontFlag) {
        *newFrontFlag = (node->d_ptrs[0].d_prev_p == d_head_p);
    }

    ++d_length;
}

template <class KEY, class DATA, class RANDGEN>
void SkipList<KEY, DATA, RANDGEN>::moveImp(bool* newFrontFlag,
                                           Node* location[],
                                           Node* node)
{
    BSLS_ASSERT(location);
    BSLS_ASSERT(node);

    int level = node->d_level;
    BSLS_ASSERT(level <= d_listLevel);

    for (int k = 0; k <= level; ++k) {
        Node* newP = location[k]->d_ptrs[k].d_prev_p;
        Node* newQ = location[k];

        if (newP == node || newQ == node) {
            // The node's already in the right place.  Since we started at
            // level 0, there's no more work to do.

            break;
        }

        Node* oldP = node->d_ptrs[k].d_prev_p;
        Node* oldQ = node->d_ptrs[k].d_next_p;

        oldQ->d_ptrs[k].d_prev_p = oldP;
        oldP->d_ptrs[k].d_next_p = oldQ;

        node->d_ptrs[k].d_prev_p = newP;
        node->d_ptrs[k].d_next_p = newQ;

        newP->d_ptrs[k].d_next_p = node;
        newQ->d_ptrs[k].d_prev_p = node;
    }

    if (newFrontFlag) {
        *newFrontFlag = (node->d_ptrs[0].d_prev_p == d_head_p);
    }
}

template <class KEY, class DATA, class RANDGEN>
inline void SkipList<KEY, DATA, RANDGEN>::releaseNode(Node* node)
{
    BSLS_ASSERT(node);

    node->d_key.~KEY();
    node->d_data.~DATA();

    BSLS_ASSERT(0 == node->d_ptrs[0].d_next_p);

    PoolUtil::deallocate(d_poolManager_p, node, node->d_level);
}

template <class KEY, class DATA, class RANDGEN>
int SkipList<KEY, DATA, RANDGEN>::removeNode(Node* node)
{
    BSLS_ASSERT(node);

    if (0 == node->d_ptrs[0].d_next_p) {
        return e_NOT_FOUND;  // RETURN
    }

    int level = node->d_level;

    for (int k = level; k >= 0; --k) {
        Node* p = node->d_ptrs[k].d_prev_p;
        Node* q = node->d_ptrs[k].d_next_p;

        q->d_ptrs[k].d_prev_p = p;
        p->d_ptrs[k].d_next_p = q;
    }

    node->d_ptrs[0].d_next_p = 0;
    --d_length;
    return 0;
}

template <class KEY, class DATA, class RANDGEN>
int SkipList<KEY, DATA, RANDGEN>::updateNodeR(bool*      newFrontFlag,
                                              Node*      node,
                                              const KEY& newKey)
{
    BSLS_ASSERT(node);

    if (0 == node->d_ptrs[0].d_next_p) {
        return e_NOT_FOUND;  // RETURN
    }

    Node* location[SkipListConsts::k_MAX_NUM_LEVELS];

    lookupImpUpperBoundR(location, newKey);

    node->d_key = newKey;  // may throw

    // now we are committed: change the list!

    moveImp(newFrontFlag, location, node);

    return 0;
}

template <class KEY, class DATA, class RANDGEN>
void SkipList<KEY, DATA, RANDGEN>::checkInvariants() const
{
    for (int ii = 0; ii <= d_listLevel; ++ii) {
        int   numNodes = 0;
        Node* prev     = d_head_p;
        for (Node* q = d_head_p->d_ptrs[ii].d_next_p; d_tail_p != q;
             prev = q, q = q->d_ptrs[ii].d_next_p)
        {
            ++numNodes;

            BSLS_ASSERT_OPT(q->d_ptrs[ii].d_prev_p == prev);

            BSLS_ASSERT_OPT(q->d_level >= ii);
            BSLS_ASSERT_OPT(q->d_level <= d_listLevel);

            for (int jj = ii - 1; 0 <= jj; --jj) {
                BSLS_ASSERT_OPT(q->d_ptrs[jj].d_next_p);
                BSLS_ASSERT_OPT(q->d_ptrs[jj].d_prev_p);
            }
        }

        BSLS_ASSERT_OPT(numNodes <= d_length);
        BSLS_ASSERT_OPT(0 != ii || numNodes == d_length);

        BSLS_ASSERT_OPT(0 == d_head_p->d_ptrs[ii].d_prev_p);
        BSLS_ASSERT_OPT(0 == d_tail_p->d_ptrs[ii].d_next_p);
    }
}

template <class KEY, class DATA, class RANDGEN>
SkipList_Node<KEY, DATA>* SkipList<KEY, DATA, RANDGEN>::frontNode() const
{
    Node* node = d_head_p->d_ptrs[0].d_next_p;
    if (node == d_tail_p) {
        return 0;  // RETURN
    }

    return node;
}

template <class KEY, class DATA, class RANDGEN>
void SkipList<KEY, DATA, RANDGEN>::lookupImpUpperBoundL(Node*      location[],
                                                        const KEY& key) const
{
    Node* p = d_head_p;
    for (int k = d_listLevel; k >= 0; --k) {
        Node* q = p->d_ptrs[k].d_next_p;
        while (q != d_tail_p && !(key < q->d_key)) {
            p = q;
            q = p->d_ptrs[k].d_next_p;
        }

        location[k] = q;
    }

    BSLS_ASSERT_SAFE(d_head_p != location[0]);
}

template <class KEY, class DATA, class RANDGEN>
void SkipList<KEY, DATA, RANDGEN>::lookupImpUpperBoundR(Node*      location[],
                                                        const KEY& key) const
{
    Node* q = d_tail_p;
    for (int k = d_listLevel; k >= 0; --k) {
        Node* p = q->d_ptrs[k].d_prev_p;
        while (p != d_head_p && key < p->d_key) {
            q = p;
            p = q->d_ptrs[k].d_prev_p;
#ifdef SKIPLIST_UNIT_TEST_BUILD
            ++d_numberOfSteps;
#endif
        }
        location[k] = q;
    }

    BSLS_ASSERT_SAFE(d_head_p != location[0]);
}

template <class KEY, class DATA, class RANDGEN>
SkipList<KEY, DATA, RANDGEN>::SkipList(bslma::Allocator* basicAllocator)
: d_listLevel(0)
, d_length(0)
#ifdef SKIPLIST_UNIT_TEST_BUILD
, d_numberOfSteps(0)
#endif
, d_poolManager_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    initialize();
}

template <class KEY, class DATA, class RANDGEN>
SkipList<KEY, DATA, RANDGEN>::~SkipList()
{
#ifdef SKIPLIST_UNIT_TEST_BUILD
    checkInvariants();
#endif

    for (Node* p = d_tail_p->d_ptrs[0].d_prev_p; d_head_p != p;) {
        Node* condemned               = p;
        p                             = p->d_ptrs[0].d_prev_p;
        condemned->d_ptrs[0].d_next_p = 0;

        releaseNode(condemned);
    }

    PoolUtil::deallocate(d_poolManager_p, d_head_p, d_head_p->d_level);
    PoolUtil::deallocate(d_poolManager_p, d_tail_p, d_tail_p->d_level);

    PoolUtil::deletePoolManager(d_allocator_p, d_poolManager_p);
}

template <class KEY, class DATA, class RANDGEN>
inline SkipListPair<KEY, DATA>* SkipList<KEY, DATA, RANDGEN>::addL(
    const KEY&  key,
    const DATA& data,
    bool*       newFrontFlag)
{
    const int level = d_rand.randomLevel();
    Node*     node  = allocateNode(level, key, data);
    addNodeL(newFrontFlag, node);
    return reinterpret_cast<Pair*>(node);
}

template <class KEY, class DATA, class RANDGEN>
inline SkipListPair<KEY, DATA>* SkipList<KEY, DATA, RANDGEN>::addR(
    const KEY&  key,
    const DATA& data,
    bool*       newFrontFlag)
{
    const int level = d_rand.randomLevel();
    Node*     node  = allocateNode(level, key, data);
    addNodeR(newFrontFlag, node);
    return reinterpret_cast<Pair*>(node);
}

// Removal Methods

template <class KEY, class DATA, class RANDGEN>
inline int SkipList<KEY, DATA, RANDGEN>::remove(const Pair* reference)
{
    if (0 == reference) {
        return e_INVALID;  // RETURN
    }

    Node* node = pairToNode(reference);

    int ret = removeNode(node);
    if (ret) {
        return ret;  // RETURN
    }

    releaseNode(node);
    return 0;
}

template <class KEY, class DATA, class RANDGEN>
inline int SkipList<KEY, DATA, RANDGEN>::removeAll()
{
    Node* const begin      = d_head_p;
    Node* const end        = d_tail_p->d_ptrs[0].d_prev_p;
    int         numRemoved = d_length;

    for (int ii = 0; ii <= d_listLevel; ++ii) {
        d_head_p->d_ptrs[ii].d_next_p = d_tail_p;
        d_tail_p->d_ptrs[ii].d_prev_p = d_head_p;
    }
    d_length = 0;

    for (Node* q = end; begin != q; q = q->d_ptrs[0].d_prev_p) {
        q->d_ptrs[0].d_next_p = 0;  // Marks node as removed from list
    }

    for (Node* q = end; begin != q;) {
        Node* condemned = q;
        q               = q->d_ptrs[0].d_prev_p;

        releaseNode(condemned);
    }

    d_listLevel =
        0;  //this is one of the differences from bdlcc_skiplist implementation

    return numRemoved;
}

// Update Methods

template <class KEY, class DATA, class RANDGEN>
inline int SkipList<KEY, DATA, RANDGEN>::updateR(const Pair* reference,
                                                 const KEY&  newKey,
                                                 bool*       newFrontFlag)
{
    if (0 == reference) {
        return e_INVALID;  // RETURN
    }

    Node* node = pairToNode(reference);
    return updateNodeR(newFrontFlag, node, newKey);
}

template <class KEY, class DATA, class RANDGEN>
inline SkipListPair<KEY, DATA>* SkipList<KEY, DATA, RANDGEN>::front() const
{
    return reinterpret_cast<Pair*>(frontNode());
}

template <class KEY, class DATA, class RANDGEN>
inline int SkipList<KEY, DATA, RANDGEN>::getListLevel() const
{
    return d_listLevel;
}

#ifdef SKIPLIST_UNIT_TEST_BUILD
template <class KEY, class DATA, class RANDGEN>
inline int SkipList<KEY, DATA, RANDGEN>::getNumberOfSteps() const
{
    return d_numberOfSteps;
}
#endif

template <class KEY, class DATA, class RANDGEN>
inline bool SkipList<KEY, DATA, RANDGEN>::isEmpty() const
{
    return d_tail_p == d_head_p->d_ptrs[0].d_next_p;
}

template <class KEY, class DATA, class RANDGEN>
inline int SkipList<KEY, DATA, RANDGEN>::length() const
{
    return d_length;
}

template <class KEY, class DATA, class RANDGEN>
bsl::ostream& SkipList<KEY, DATA, RANDGEN>::print(bsl::ostream& stream,
                                                  int           level,
                                                  int spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;  // RETURN
    }

    bdlb::Print::indent(stream, level, spacesPerLevel);

    if (0 <= spacesPerLevel) {
        // Multi-line output.

        if (level < 0) {
            level = -level;
        }

        stream << "[\n";

        const int levelPlus1 = level + 1;

        Node* node = frontNode();
        while (node) {
            bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
            stream << "[\n";

            const int levelPlus2 = level + 2;
            bdlb::Print::indent(stream, levelPlus2, spacesPerLevel);
            stream << "level = " << node->d_level << "\n";

            bdlb::PrintMethods::print(stream,
                                      node->d_key,
                                      levelPlus2,
                                      spacesPerLevel);

            bdlb::Print::indent(stream, levelPlus2, spacesPerLevel);
            stream << "=>\n";

            bdlb::PrintMethods::print(stream,
                                      node->d_data,
                                      levelPlus2,
                                      spacesPerLevel);
            bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
            stream << "]\n";
            skipForward(&node);
        }

        bdlb::Print::indent(stream, level, spacesPerLevel);

        stream << "]\n";
    }
    else {
        // Output on a single line and suppress any further indentation.

        stream << "[";

        Node* node = frontNode();
        while (node) {
            stream << "[ (level = " << node->d_level << ") ";

            bdlb::PrintMethods::print(stream, node->d_key, 0, -1);
            stream << " => ";
            bdlb::PrintMethods::print(stream, node->d_data, 0, -1);

            stream << " ]";
            skipForward(&node);
        }

        stream << "]";
    }

    return stream << bsl::flush;
}

template <class KEY, class DATA, class RANDGEN>
inline int SkipList<KEY, DATA, RANDGEN>::skipForward(Pair** item) const
{
    BSLS_ASSERT(item);

    Node** node = reinterpret_cast<Node**>(item);

    Node* current = *node;
    BSLS_ASSERT(current);
    BSLS_ASSERT(current != d_head_p);
    BSLS_ASSERT(current != d_tail_p);

    if (0 == current->d_ptrs[0].d_next_p) {
        // The node is no longer on the list.
        return e_NOT_FOUND;  // RETURN
    }

    Node* next = current->d_ptrs[0].d_next_p;
    if (d_tail_p == next) {
        *node = 0;
        return 0;  // RETURN
    }

    *node = next;
    return 0;
}

// Aspects

template <class KEY, class DATA, class RANDGEN>
inline bslma::Allocator* SkipList<KEY, DATA, RANDGEN>::allocator() const
{
    return d_allocator_p;
}

}  // close package namespace

// FREE OPERATORS
template <class KEY, class DATA, class RANDGEN>
inline bsl::ostream& ntcs::operator<<(bsl::ostream& stream,
                                      const SkipList<KEY, DATA, RANDGEN>& list)
{
    return list.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif
