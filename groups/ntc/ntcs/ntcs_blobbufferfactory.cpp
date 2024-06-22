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

#include <ntcs_blobbufferfactory.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_blobbufferfactory_cpp, "$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmf_assert.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_log.h>

// Uncomment to enable pooling.
#define NTCS_BLOBBUFFERFACTORY_POOL 1

// Uncomment to enable logging from this component.
// #define NTCS_BLOBBUFFERFACTORY_LOG 1

#if NTCS_BLOBBUFFERFACTORY_LOG

#define NTCS_BLOBBUFFERFACTORY_LOG_SYSTEM_ACQUIRE(address, blockSize)         \
    BSLS_LOG_TRACE("Acquired system memory: address = %p, blockSize = %d",    \
                   address,                                                   \
                   (int)(blockSize))

#define NTCS_BLOBBUFFERFACTORY_LOG_SYSTEM_RELEASE(address, blockSize)         \
    BSLS_LOG_TRACE("Released system memory: address = %p, blockSize = %d",    \
                   address,                                                   \
                   (int)(blockSize))

#define NTCS_BLOBBUFFERFACTORY_LOG_POOL_ACQUIRE(address,                      \
                                                blockSize,                    \
                                                numBytesInUse,                \
                                                numAllocated,                 \
                                                numPooled)                    \
    BSLS_LOG_TRACE("Acquired buffer memory: address = %p, blockSize = %d, "   \
                   "numBytesInUse = %d, numAllocated = %d, numPooled = %d",   \
                   address,                                                   \
                   (int)(blockSize),                                          \
                   (int)(numBytesInUse),                                      \
                   (int)(numAllocated),                                       \
                   (int)(numPooled))

#define NTCS_BLOBBUFFERFACTORY_LOG_POOL_RELEASE(address,                      \
                                                blockSize,                    \
                                                numBytesInUse,                \
                                                numAllocated,                 \
                                                numPooled)                    \
    BSLS_LOG_TRACE("Released buffer memory: address = %p, blockSize = %d, "   \
                   "numBytesInUse = %d, numAllocated = %d, numPooled = %d",   \
                   address,                                                   \
                   (int)(d_blockSize),                                        \
                   (int)(numBytesInUse),                                      \
                   (int)(numAllocated),                                       \
                   (int)(numPooled))

#else

#define NTCS_BLOBBUFFERFACTORY_LOG_SYSTEM_ACQUIRE(address, blockSize)
#define NTCS_BLOBBUFFERFACTORY_LOG_SYSTEM_RELEASE(address, blockSize)
#define NTCS_BLOBBUFFERFACTORY_LOG_POOL_ACQUIRE(address,                      \
                                                blockSize,                    \
                                                numBytesInUse,                \
                                                numAllocated,                 \
                                                numPooled)
#define NTCS_BLOBBUFFERFACTORY_LOG_POOL_RELEASE(address,                      \
                                                blockSize,                    \
                                                numBytesInUse,                \
                                                numAllocated,                 \
                                                numPooled)

#endif

namespace BloombergLP {
namespace ntcs {

namespace {

enum { MAX_BLOCKS_PER_CHUNK = 1 };

}  // close unnamed namespace

const ntci::MetricMetadata BlobBufferFactoryMetrics::STATISTICS[] = {
#if NTCI_METRIC_PREFIX

    NTCI_METRIC_METADATA_GAUGE(BuffersInUse),
    NTCI_METRIC_METADATA_GAUGE(BuffersPooled),

    NTCI_METRIC_METADATA_GAUGE(BytesInUse),
    NTCI_METRIC_METADATA_GAUGE(BytesPooled),

#else
    NTCI_METRIC_METADATA_GAUGE(buffersInUse),
    NTCI_METRIC_METADATA_GAUGE(buffersPooled),

    NTCI_METRIC_METADATA_GAUGE(bytesInUse),
    NTCI_METRIC_METADATA_GAUGE(bytesPooled),
#endif
};

BlobBufferFactoryMetrics::BlobBufferFactoryMetrics(
    const bslstl::StringRef& prefix,
    const bslstl::StringRef& objectName,
    bslma::Allocator*        basicAllocator)
: d_mutex()
, d_numAllocated(0)
, d_numAvailable(0)
, d_numPooled(0)
, d_numBytesInUse(0)
, d_prefix(prefix, basicAllocator)
, d_objectName(objectName, basicAllocator)
, d_parent_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

BlobBufferFactoryMetrics::BlobBufferFactoryMetrics(
    const bslstl::StringRef&                               prefix,
    const bslstl::StringRef&                               objectName,
    const bsl::shared_ptr<ntcs::BlobBufferFactoryMetrics>& parent,
    bslma::Allocator*                                      basicAllocator)
: d_mutex()
, d_numAllocated(0)
, d_numAvailable(0)
, d_numPooled(0)
, d_numBytesInUse(0)
, d_prefix(basicAllocator)
, d_objectName(basicAllocator)
, d_parent_sp(parent)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_parent_sp) {
        d_prefix.append(d_parent_sp->getFieldPrefix(0));
        d_prefix.append(1, '.');

        d_objectName.append(d_parent_sp->objectName());
        d_objectName.append(1, '-');
    }

    d_prefix.append(prefix);
    d_objectName.append(objectName);
}

BlobBufferFactoryMetrics::~BlobBufferFactoryMetrics()
{
}

void BlobBufferFactoryMetrics::getStats(bdld::ManagedDatum* result)
{
    LockGuard guard(&d_mutex);

    bdld::DatumMutableArrayRef array;
    bdld::Datum::createUninitializedArray(&array,
                                          numOrdinals(),
                                          result->allocator());

    bsl::size_t buffersInUse  = 0;
    bsl::size_t buffersPooled = 0;
    bsl::size_t bytesInUse    = 0;
    bsl::size_t bytesPooled   = 0;

    // MRM: Calculate values.

    array.data()[0] = bdld::Datum::createDouble(double(buffersInUse));
    array.data()[1] = bdld::Datum::createDouble(double(buffersPooled));
    array.data()[2] = bdld::Datum::createDouble(double(bytesInUse));
    array.data()[3] = bdld::Datum::createDouble(double(bytesPooled));

    *array.length() = numOrdinals();

    result->adopt(bdld::Datum::adoptArray(array));
}

const char* BlobBufferFactoryMetrics::getFieldPrefix(int ordinal) const
{
    NTCCFG_WARNING_UNUSED(ordinal);
    return d_prefix.c_str();
}

const char* BlobBufferFactoryMetrics::getFieldName(int ordinal) const
{
    if (ordinal < numOrdinals()) {
        return BlobBufferFactoryMetrics::STATISTICS[ordinal].d_name;
    }
    else {
        return 0;
    }
}

const char* BlobBufferFactoryMetrics::getFieldDescription(int ordinal) const
{
    NTCCFG_WARNING_UNUSED(ordinal);
    return "";
}

ntci::Monitorable::StatisticType BlobBufferFactoryMetrics::getFieldType(
    int ordinal) const
{
    if (ordinal < numOrdinals()) {
        return BlobBufferFactoryMetrics::STATISTICS[ordinal].d_type;
    }
    else {
        return ntci::Monitorable::e_AVERAGE;
    }
}

int BlobBufferFactoryMetrics::getFieldTags(int ordinal) const
{
    NTCCFG_WARNING_UNUSED(ordinal);
    return ntci::Monitorable::e_ANONYMOUS;
}

int BlobBufferFactoryMetrics::getFieldOrdinal(const char* fieldName) const
{
    int result = 0;

    for (int ordinal = 0; ordinal < numOrdinals(); ++ordinal) {
        if (bsl::strcmp(BlobBufferFactoryMetrics::STATISTICS[ordinal].d_name,
                        fieldName) == 0)
        {
            result = ordinal;
        }
    }

    return result;
}

int BlobBufferFactoryMetrics::numOrdinals() const
{
    return sizeof BlobBufferFactoryMetrics::STATISTICS /
           sizeof BlobBufferFactoryMetrics::STATISTICS[0];
}

const char* BlobBufferFactoryMetrics::objectName() const
{
    return d_objectName.c_str();
}

const bsl::shared_ptr<ntcs::BlobBufferFactoryMetrics>& BlobBufferFactoryMetrics::
    parent() const
{
    return d_parent_sp;
}

bsl::size_t BlobBufferFactoryMetrics::numBuffersAllocated() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numAllocated.loadRelaxed());
}

bsl::size_t BlobBufferFactoryMetrics::numBuffersAvailable() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numAvailable.loadRelaxed());
}

bsl::size_t BlobBufferFactoryMetrics::numBuffersPooled() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numPooled.loadRelaxed());
}

bsl::size_t BlobBufferFactoryMetrics::numBytesInUse() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numBytesInUse.loadRelaxed());
}

BlobBufferFactoryAllocator::BlobBufferFactoryAllocator(
    bslma::Allocator* basicAllocator)
: d_blockSize(0)
, d_numBytesInUse(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

BlobBufferFactoryAllocator::~BlobBufferFactoryAllocator()
{
    BSLS_ASSERT_OPT(d_numBytesInUse == 0);
}

void* BlobBufferFactoryAllocator::allocate(size_type size)
{
    bsl::uint64_t previous = d_blockSize.testAndSwap(0, size);
    if (previous != 0) {
        BSLS_ASSERT_OPT(previous == size);
    }

    d_numBytesInUse.addRelaxed(d_blockSize.loadRelaxed());

    void* address = d_allocator_p->allocate(size);

    NTCS_BLOBBUFFERFACTORY_LOG_SYSTEM_ACQUIRE(address, size);

    return address;
}

void BlobBufferFactoryAllocator::deallocate(void* address)
{
    bsl::uint64_t blockSize = d_blockSize.loadRelaxed();
    BSLS_ASSERT_OPT(blockSize != 0);

    d_numBytesInUse.subtractRelaxed(blockSize);

    d_allocator_p->deallocate(address);

    NTCS_BLOBBUFFERFACTORY_LOG_SYSTEM_RELEASE(address, blockSize);
}

bsl::size_t BlobBufferFactoryAllocator::numBytesInUse() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numBytesInUse.loadRelaxed());
}

bsl::size_t BlobBufferFactoryAllocator::blockSize() const
{
    bsl::uint64_t blockSize = d_blockSize.loadRelaxed();
    BSLS_ASSERT_OPT(blockSize != 0);

    return NTCCFG_WARNING_NARROW(bsl::size_t, blockSize);
}

void* BlobBufferFactory::allocate(size_type size)
{
#if NTCS_BLOBBUFFERFACTORY_POOL

    void* address = d_memoryPool.allocate(size);

    bsl::uint64_t numAllocated = d_numAllocated.addRelaxed(1);

    bsl::size_t numBytesInUse = d_memoryPoolAllocator.numBytesInUse();
    bsl::size_t numPooled     = numBytesInUse / d_blockSize;

    d_numPooled     = numPooled;
    d_numAvailable  = numPooled - numAllocated;
    d_numBytesInUse = numBytesInUse;

    NTCS_BLOBBUFFERFACTORY_LOG_POOL_ACQUIRE(address,
                                            d_blockSize,
                                            numBytesInUse,
                                            numAllocated,
                                            numPooled);

    return address;

#else

    void* address = d_allocator_p->allocate(size);

    d_numAllocated.addRelaxed(1);
    d_numPooled.addRelaxed(1);
    d_numBytesInUse.addRelaxed(d_blobBufferSize);

    return address;

#endif
}

void BlobBufferFactory::deallocate(void* address)
{
#if NTCS_BLOBBUFFERFACTORY_POOL

    d_memoryPool.deallocate(address);

    bsl::uint64_t numAllocated = d_numAllocated.subtractRelaxed(1);

    bsl::size_t numBytesInUse = d_memoryPoolAllocator.numBytesInUse();
    bsl::size_t numPooled     = numBytesInUse / d_blockSize;

    d_numPooled     = numPooled;
    d_numAvailable  = numPooled - numAllocated;
    d_numBytesInUse = numBytesInUse;

    NTCS_BLOBBUFFERFACTORY_LOG_POOL_RELEASE(address,
                                            d_blockSize,
                                            numBytesInUse,
                                            numAllocated,
                                            numPooled);

#else

    d_allocator_p->deallocate(address);

    d_numAllocated.subtractRelaxed(1);
    d_numPooled.subtractRelaxed(1);
    d_numBytesInUse.subtractRelaxed(d_blobBufferSize);

#endif
}

BlobBufferFactory::BlobBufferFactory(bsl::size_t       blobBufferSize,
                                     bslma::Allocator* basicAllocator)
: d_memoryPoolAllocator(basicAllocator)
, d_memoryPool(bsls::BlockGrowth::BSLS_CONSTANT,
               MAX_BLOCKS_PER_CHUNK,
               &d_memoryPoolAllocator)
, d_blobBufferSize(blobBufferSize)
, d_blockSize(0)
, d_numAllocated(0)
, d_numAvailable(0)
, d_numPooled(0)
, d_numBytesInUse(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
#if NTCS_BLOBBUFFERFACTORY_POOL

    // Allocate a pseudo blob buffer to determine the block size used by the
    // concurrent memory pool.

    {
        bsl::shared_ptr<char> probe =
            bslstl::SharedPtrUtil::createInplaceUninitializedBuffer(
                d_blobBufferSize,
                &d_memoryPool);

        BSLS_ASSERT_OPT(probe.get() != 0);
    }

    // Remember the block size.

    d_blockSize = d_memoryPoolAllocator.blockSize();
    BSLS_ASSERT_OPT(d_blockSize != 0);

#endif
}

BlobBufferFactory::~BlobBufferFactory()
{
    BSLS_ASSERT_OPT(d_numAllocated == 0);
}

void BlobBufferFactory::allocate(bdlbb::BlobBuffer* buffer)
{
    buffer->reset(bslstl::SharedPtrUtil::createInplaceUninitializedBuffer(
                      d_blobBufferSize,
                      this),
                  NTCCFG_WARNING_NARROW(int, d_blobBufferSize));
}

bsl::size_t BlobBufferFactory::numBuffersAllocated() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numAllocated.loadRelaxed());
}

bsl::size_t BlobBufferFactory::numBuffersAvailable() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numAvailable.loadRelaxed());
}

bsl::size_t BlobBufferFactory::numBuffersPooled() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numPooled.loadRelaxed());
}

bsl::size_t BlobBufferFactory::numBytesInUse() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numBytesInUse.loadRelaxed());
}

// 32-bits                    64-bits:
// 4 = vtable                 8 = vtable
// 4 = strong                 4 = strong
// 4 = weak                   4 = weak
// 4 = data                   8 = data
// 4 = data                   8 = pool
// 4 = next                   8 = next
// 24                         40
// 64 - 24 = 40 bytes padding 64 - 40 = 24 bytes padding
BSLMF_ASSERT(sizeof(BlobBufferPoolObject) == 64);

BlobBufferPoolObject::BlobBufferPoolObject(BlobBufferPool* pool)
: bslma::SharedPtrRep()
, d_data_p(0)
, d_pool_p(pool)
, d_next_p(0)
, d_generation(0)
, d_padding()
{
    d_data_p = (char*)(this) + sizeof(BlobBufferPoolObject);
    BSLS_ASSERT((bsl::size_t)(bsl::uintptr_t)(d_data_p) % 16 == 0);
}

BlobBufferPoolObject* BlobBufferPool::replenish()
{
    const bsl::size_t allocationSize =
        sizeof(BlobBufferPoolObject) + d_blobBufferSize;

    void* arena = d_aligningAllocator.allocate(allocationSize);
    BSLS_ASSERT((bsl::size_t)(bsl::uintptr_t)(arena) % k_ALIGNMENT == 0);

    new (arena) BlobBufferPoolObject(this);

    BlobBufferPoolObject* object = (BlobBufferPoolObject*)(arena);
    object->resetCountsRaw(0, 0);

    ++d_numPooled;
    d_numBytesInUse += allocationSize;

    return object;
}

BlobBufferPool::BlobBufferPool(bsl::size_t       blobBufferSize,
                               bslma::Allocator* basicAllocator)
: d_head()
, d_blobBufferSize(blobBufferSize)
, d_numAllocated(0)
, d_numPooled(0)
, d_numBytesInUse(0)
, d_aligningAllocator(k_ALIGNMENT, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
#if NTCS_BLOBBUFFERPOOL_DEBUG
    bsl::memset(d_objectArray, 0, sizeof d_objectArray);
    d_objectCount = 0;
#endif
}

BlobBufferPool::~BlobBufferPool()
{
    bsl::uint64_t numAllocated = d_numAllocated.loadAcquire();

    if (numAllocated != 0) {
        bsl::cout << "numAllocated = " << numAllocated << bsl::endl;
    }

    bsl::uint64_t numFreed = 0;

    BlobBufferPoolObject* currentObject;
    Handle::TagType       currentTag;

    d_head.loadAcquire(&currentObject, &currentTag);

    while (currentObject) {
        BlobBufferPoolObject* targetObject = currentObject;
        currentObject                      = currentObject->next();
        d_aligningAllocator.deallocate(targetObject);
        ++numFreed;
    }

    if (numAllocated != 0) {
        bsl::cout << "numFreed = " << numFreed << bsl::endl;
    }

    // BSLS_ASSERT_OPT(d_numAllocated.loadAcquire() == 0);
}

void BlobBufferPool::allocate(bdlbb::BlobBuffer* buffer)
{
    BlobBufferPoolObject* object = 0;

    BlobBufferPoolObject* oldHead;
    Handle::TagType       oldTag;

    d_head.loadAcquire(&oldHead, &oldTag);

    while (true) {
        // MRM: for (unsigned int attempt = 0; true; ++attempt) {
        if (oldHead == 0) {
            break;
        }

        BlobBufferPoolObject* newHead = oldHead->next();
        Handle::TagType       newTag  = (oldTag + 1) % Handle::maxTag();

        BlobBufferPoolObject* nowHead;
        Handle::TagType       nowTag;

        const bool unchanged = d_head.testAndSwapAcqRel(&nowHead,
                                                        &nowTag,
                                                        oldHead,
                                                        oldTag,
                                                        newHead,
                                                        newTag);

        if (unchanged) {
// MRM: Remove after debugging.
#if NTCS_BLOBBUFFERPOOL_DEBUG
            BSLS_ASSERT(object != d_objectArray[d_objectCount - 1]);
#endif

            // MRM: Remove after debugging. This condition is only true
            // when the total number of objects is pre-reserved.
            // BSLS_ASSERT(newHead != 0);

            object = oldHead;
            break;
        }
        else {
            oldHead = nowHead;
            oldTag  = nowTag;

// MRM
#if 0
            if (attempt % 2 == 0) {
                for (unsigned i = 0; i < 3; ++i) {
                    BlobBufferPoolObject* currentHead;
                    Handle::TagType       currentTag;

                    d_head.loadRelaxed(&currentHead, &currentTag);

                    if ((currentHead == oldHead) && (currentTag == nowTag)) {
                        break;
                    }
                }
            }
#endif

            // MRM: Remove after debugging.
            // bslmt::ThreadUtil::yield();
        }
    }

    if (NTCCFG_UNLIKELY(object == 0)) {
        object = this->replenish();
    }
    else {
        object->setNext(0);
    }

    ++d_numAllocated;

    BSLS_ASSERT(object->data() != 0);
    BSLS_ASSERT(object->next() == 0);
    BSLS_ASSERT(object->numReferences() == 0);
    BSLS_ASSERT(object->numWeakReferences() == 0);

    object->resetCountsRaw(1, 0);

    buffer->buffer().reset(object->data(), object);
    buffer->setSize(NTCCFG_WARNING_NARROW(int, d_blobBufferSize));
}

void BlobBufferPool::release(BlobBufferPoolObject* object)
{
    BSLS_ASSERT(object);
    BSLS_ASSERT(object->data() != 0);
    BSLS_ASSERT(object->next() == 0);
    BSLS_ASSERT(object->numReferences() == 0);
    BSLS_ASSERT(object->numWeakReferences() == 0);

// MRM: Remove after debugging.
#if NTCS_BLOBBUFFERPOOL_DEBUG
    BSLS_ASSERT(object != d_objectArray[d_objectCount - 1]);
#endif

    BlobBufferPoolObject* oldHead;
    Handle::TagType       oldTag;

    d_head.loadAcquire(&oldHead, &oldTag);

    while (true) {
        // MRM: for (unsigned int attempt = 0; true; ++attempt) {
        // MRM: Remove after debugging.
        BSLS_ASSERT(oldHead != object);

        object->setNext(oldHead);

        BlobBufferPoolObject* newHead = object;
        Handle::TagType       newTag  = (oldTag + 1) % Handle::maxTag();

        BlobBufferPoolObject* nowHead;
        Handle::TagType       nowTag;

        const bool unchanged = d_head.testAndSwapAcqRel(&nowHead,
                                                        &nowTag,
                                                        oldHead,
                                                        oldTag,
                                                        newHead,
                                                        newTag);

        if (unchanged) {
            // MRM: Remove after debugging. This condition is only true
            // when the total number of objects is pre-reserved.
            // BSLS_ASSERT(oldHead != 0);

            break;
        }
        else {
            oldHead = nowHead;
            oldTag  = nowTag;

// MRM
#if 0
            if (attempt % 2 == 0) {
                for (unsigned i = 0; i < 3; ++i) {
                    BlobBufferPoolObject* currentHead;
                    Handle::TagType       currentTag;

                    d_head.loadRelaxed(&currentHead, &currentTag);

                    if ((currentHead == oldHead) && (currentTag == nowTag)) {
                        break;
                    }
                }
            }
#endif

            // MRM: Remove after debugging.
            // MRM: bslmt::ThreadUtil::yield();
        }
    }

    --d_numAllocated;
}

void BlobBufferPool::reserve(bsl::size_t numObjects)
{
    for (bsl::size_t i = 0; i < numObjects; ++i) {
        BlobBufferPoolObject* object = this->replenish();

#if NTCS_BLOBBUFFERPOOL_DEBUG
        d_objectArray[(k_OBJECT_ARRAY_CAPACITY - 1) - d_objectCount++] =
            object;
#endif

#if 0

        ++d_numAllocated;
        this->release(object);

#else

        BlobBufferPoolObject* currentObject;
        Handle::TagType       currentTag;

        d_head.load(&currentObject, &currentTag);

        if (currentObject == 0) {
            d_head.store(object, 0);
        }
        else {
            object->setNext(currentObject);
            d_head.store(object, 0);
        }

#endif
    }
}

bsl::size_t BlobBufferPool::numBuffersAllocated() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numAllocated.loadRelaxed());
}

bsl::size_t BlobBufferPool::numBuffersAvailable() const
{
    bsl::uint64_t numAllocated = d_numAllocated.loadRelaxed();
    bsl::uint64_t numPooled    = d_numPooled.loadRelaxed();

    if (numPooled > numAllocated) {
        return NTCCFG_WARNING_NARROW(bsl::size_t, numPooled - numAllocated);
    }
    else {
        return 0;
    }
}

bsl::size_t BlobBufferPool::numBuffersPooled() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numPooled.loadRelaxed());
}

bsl::size_t BlobBufferPool::numBytesInUse() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_numBytesInUse.loadRelaxed());
}

}  // close package namespace
}  // close enterprise namespace
