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

#ifndef INCLUDED_NTCS_BLOBBUFFERFACTORY
#define INCLUDED_NTCS_BLOBBUFFERFACTORY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_metric.h>
#include <ntci_monitorable.h>
#include <ntcscm_version.h>
#include <bdlbb_blob.h>
#include <bdlma_aligningallocator.h>
#include <bdlma_concurrentpoolallocator.h>
#include <bdlma_countingallocator.h>
#include <bsls_atomic.h>
#include <bsls_platform.h>
#include <bsl_memory.h>
#include <bsl_typeinfo.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide statistics for the runtime behavior of a blob buffer pool.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class BlobBufferFactoryMetrics
: public ntci::Monitorable,
  public ntccfg::Shared<BlobBufferFactoryMetrics>
{
    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex                                   d_mutex;
    bsls::AtomicUint64                              d_numAllocated;
    bsls::AtomicUint64                              d_numAvailable;
    bsls::AtomicUint64                              d_numPooled;
    bsls::AtomicUint64                              d_numBytesInUse;
    bsl::string                                     d_prefix;
    bsl::string                                     d_objectName;
    bsl::shared_ptr<ntcs::BlobBufferFactoryMetrics> d_parent_sp;
    bslma::Allocator*                               d_allocator_p;

    static const struct ntci::MetricMetadata STATISTICS[];

  private:
    BlobBufferFactoryMetrics(const BlobBufferFactoryMetrics&)
        BSLS_KEYWORD_DELETED;
    BlobBufferFactoryMetrics& operator=(const BlobBufferFactoryMetrics&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create new metrics for the specified 'objectName whose field names
    /// have the specified 'prefix'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    BlobBufferFactoryMetrics(const bslstl::StringRef& prefix,
                             const bslstl::StringRef& objectName,
                             bslma::Allocator*        basicAllocator = 0);

    /// Create new metrics for the specified 'objectName whose field names
    /// have the specified 'prefix'. Aggregate updates into the specified
    /// 'parent'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    BlobBufferFactoryMetrics(
        const bslstl::StringRef&                               prefix,
        const bslstl::StringRef&                               objectName,
        const bsl::shared_ptr<ntcs::BlobBufferFactoryMetrics>& parent,
        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~BlobBufferFactoryMetrics() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the array of statistics from the
    /// specified 'snapshot' for this object based on the specified
    /// 'operation': if 'operation' is e_CUMULATIVE then the statistics are
    /// for the entire life of this object;  otherwise the statistics are
    /// for the period since the last call to this function. If 'operation'
    /// is e_INTERVAL_WITH_RESET then reset all internal measurements.  Note
    /// that 'result->theArray().length()' is expected to have the same
    /// value each time this function returns.
    void getStats(bdld::ManagedDatum* result) BSLS_KEYWORD_OVERRIDE;

    /// Return the prefix corresponding to the field at the specified
    /// 'ordinal' position, or 0 if no field at the 'ordinal' position
    /// exists.
    const char* getFieldPrefix(int ordinal) const BSLS_KEYWORD_OVERRIDE;

    /// Return the field name corresponding to the field at the specified
    /// 'ordinal' position, or 0 if no field at the 'ordinal' position
    /// exists.
    const char* getFieldName(int ordinal) const BSLS_KEYWORD_OVERRIDE;

    /// Return the field description corresponding to the field at the
    /// specified 'ordinal' position, or 0 if no field at the 'ordinal'
    /// position exists.
    const char* getFieldDescription(int ordinal) const BSLS_KEYWORD_OVERRIDE;

    /// Return the type of the statistic at the specified 'ordinal'
    /// position, or e_AVERAGE if no field at the 'ordinal' position exists
    /// or the type is unknown.
    ntci::Monitorable::StatisticType getFieldType(int ordinal) const
        BSLS_KEYWORD_OVERRIDE;

    /// Return the flags that indicate which indexes to apply to the
    /// statistics measured by this monitorable object.
    int getFieldTags(int ordinal) const BSLS_KEYWORD_OVERRIDE;

    /// Return the ordinal of the specified 'fieldName', or a negative value
    /// if no field identified by 'fieldName' exists.
    int getFieldOrdinal(const char* fieldName) const BSLS_KEYWORD_OVERRIDE;

    /// Return the maximum number of elements in a datum resulting from
    /// a call to 'getStats()'.
    int numOrdinals() const BSLS_KEYWORD_OVERRIDE;

    /// Return the human-readable name of the monitorable object, or 0 or
    /// the empty string if no such human-readable name has been assigned to
    /// the monitorable object.
    const char* objectName() const BSLS_KEYWORD_OVERRIDE;

    /// Return the parent metrics object into which these metrics are
    /// aggregated, or null if no such parent object is defined.
    const bsl::shared_ptr<ntcs::BlobBufferFactoryMetrics>& parent() const;

    /// Return the number of blob buffers that have been allocated and not
    /// returned to the pool.
    bsl::size_t numBuffersAllocated() const;

    /// Return the number of blob buffers that are pooled but not allocated.
    bsl::size_t numBuffersAvailable() const;

    /// Return the total number of blob buffers that have been pooled, that
    /// is, the sum of the number of blob buffers that have been allocated
    /// and the number of blob buffers available.
    bsl::size_t numBuffersPooled() const;

    /// Return the number of bytes allocated from the allocator supplied
    /// to this object at the time of its construction and not yet freed.
    bsl::size_t numBytesInUse() const;
};

/// @internal @brief
/// Provide an instrumented allocator to allocate memory for the blob buffer
/// factory concurrent pool allocator.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class BlobBufferFactoryAllocator : public bslma::Allocator
{
    bsls::AtomicUint64 d_blockSize;
    bsls::AtomicUint64 d_numBytesInUse;
    bslma::Allocator*  d_allocator_p;

  private:
    BlobBufferFactoryAllocator(const BlobBufferFactoryAllocator&)
        BSLS_KEYWORD_DELETED;
    BlobBufferFactoryAllocator& operator=(const BlobBufferFactoryAllocator&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new blob buffer factory allocator. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit BlobBufferFactoryAllocator(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~BlobBufferFactoryAllocator() BSLS_KEYWORD_OVERRIDE;

    /// Return a newly allocated block of memory of (at least) the specified
    /// positive 'size' (in bytes).  If 'size' is 0, a null pointer is
    /// returned with no other effect.  If this allocator cannot return the
    /// requested number of bytes, then it will throw a 'bsl::bad_alloc'
    /// exception in an exception-enabled build, or else will abort the
    /// program in a non-exception build.  The behavior is undefined unless
    /// '0 <= size'.  Note that the alignment of the address returned
    /// conforms to the platform requirement for any object of the specified
    /// 'size'.
    void* allocate(size_type size) BSLS_KEYWORD_OVERRIDE;

    /// Return the memory block at the specified 'address' back to this
    /// allocator.  If 'address' is 0, this function has no effect.  The
    /// behavior is undefined unless 'address' was allocated using this
    /// allocator object and has not already been deallocated.
    void deallocate(void* address) BSLS_KEYWORD_OVERRIDE;

    /// Return the number of bytes allocated from the allocator supplied
    /// to this object at the time of its construction and not yet freed.
    bsl::size_t numBytesInUse() const;

    /// Return the block size.
    bsl::size_t blockSize() const;
};

/// @internal @brief
/// Provide a pool of blob buffers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class BlobBufferFactory : public bdlbb::BlobBufferFactory,
                          private bslma::Allocator
{
    ntcs::BlobBufferFactoryAllocator d_memoryPoolAllocator;
    bdlma::ConcurrentPoolAllocator   d_memoryPool;
    bsl::size_t                      d_blobBufferSize;
    bsl::size_t                      d_blockSize;
    bsls::AtomicUint64               d_numAllocated;
    bsls::AtomicUint64               d_numAvailable;
    bsls::AtomicUint64               d_numPooled;
    bsls::AtomicUint64               d_numBytesInUse;
    bslma::Allocator*                d_allocator_p;

  private:
    BlobBufferFactory(const BlobBufferFactory&) BSLS_KEYWORD_DELETED;
    BlobBufferFactory& operator=(const BlobBufferFactory&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Return a newly allocated block of memory of (at least) the specified
    /// positive 'size' (in bytes).  If 'size' is 0, a null pointer is
    /// returned with no other effect.  If this allocator cannot return the
    /// requested number of bytes, then it will throw a 'bsl::bad_alloc'
    /// exception in an exception-enabled build, or else will abort the
    /// program in a non-exception build.  The behavior is undefined unless
    /// '0 <= size'.  Note that the alignment of the address returned
    /// conforms to the platform requirement for any object of the specified
    /// 'size'.
    void* allocate(size_type size) BSLS_KEYWORD_OVERRIDE;

    /// Return the memory block at the specified 'address' back to this
    /// allocator.  If 'address' is 0, this function has no effect.  The
    /// behavior is undefined unless 'address' was allocated using this
    /// allocator object and has not already been deallocated.
    void deallocate(void* address) BSLS_KEYWORD_OVERRIDE;

  public:
    /// Create a new blob buffer factory that allocates blob buffers each
    /// having the specified 'blobBufferSize'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit BlobBufferFactory(bsl::size_t       blobBufferSize,
                               bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~BlobBufferFactory() BSLS_KEYWORD_OVERRIDE;

    /// Allocate a blob buffer from this blob buffer factory, and load it
    /// into the specified 'buffer'.
    void allocate(bdlbb::BlobBuffer* buffer) BSLS_KEYWORD_OVERRIDE;

    /// Return the number of blob buffers that have been allocated and not
    /// returned to the pool.
    bsl::size_t numBuffersAllocated() const;

    /// Return the number of blob buffers that are pooled but not allocated.
    bsl::size_t numBuffersAvailable() const;

    /// Return the total number of blob buffers that have been pooled, that
    /// is, the sum of the number of blob buffers that have been allocated
    /// and the number of blob buffers available.
    bsl::size_t numBuffersPooled() const;

    /// Return the number of bytes allocated from the allocator supplied
    /// to this object at the time of its construction and not yet freed.
    bsl::size_t numBytesInUse() const;
};

class BlobBufferPool;
class BlobBufferPoolObject;

/// @internal @brief
/// Provide utilities for manipulating a tagged pointer to a pooled blob
/// buffer.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcs
template <typename TYPE, unsigned ALIGNMENT>
struct BlobBufferPoolHandleUtil {
    /// Define a type alias for the raw pointer type.
    typedef void* RawType;

    /// Define a type alias for the object pointer type.
    typedef TYPE* PtrType;

    /// Define a type alias for the tag type.
    typedef bsl::size_t TagType;

    /// Return the address that respresents the specified object 'ptr' and
    /// 'tag'.
    static RawType initialize(PtrType ptr, TagType tag);

    /// Set the value of the specified 'address' to represent the specified
    /// object 'ptr' and 'tag'.
    static void set(RawType* address, PtrType ptr, TagType tag);

    /// Set the value of the specified 'address' to represent the specified
    /// object 'ptr' while leaving the tag unchanged.
    static void setPtr(RawType* address, PtrType ptr);

    /// Set the value of the specified 'address' to represent the specified
    /// 'tag' while leaving the object pointer unchanged.
    static void setTag(RawType* address, TagType tag);

    /// Load into the specified object 'ptr' and 'tag' the object pointer
    /// and tag represented by the specified 'address'.
    static void get(PtrType* ptr, TagType* tag, RawType address);

    /// Return the object pointer represented by the specified 'address'.
    static PtrType getPtr(RawType address);

    /// Return the tag represented by the specfied 'address'.
    static TagType getTag(RawType address);

    /// Return the maximum tag value for the alignment.
    static TagType maxTag();

    /// Return true if the specified 'ptr' has a valid alignment, otherwise
    /// return false.
    static bool isAligned(PtrType ptr);

    /// Return true if the specified 'tag' is valid for the alignment,
    /// otherwise return false.
    static bool isValid(TagType tag);
};

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE typename BlobBufferPoolHandleUtil<TYPE, ALIGNMENT>::RawType
BlobBufferPoolHandleUtil<TYPE, ALIGNMENT>::initialize(PtrType ptr, TagType tag)
{
    RawType result;
    BlobBufferPoolHandleUtil<TYPE, ALIGNMENT>::set(&result, ptr, tag);
    return result;
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE void BlobBufferPoolHandleUtil<TYPE, ALIGNMENT>::set(
    RawType* address,
    PtrType  ptr,
    TagType  tag)
{
    const bsl::size_t TAG_MASK = ALIGNMENT - 1;
    const bsl::size_t PTR_MASK = ~TAG_MASK;

    BSLS_ASSERT((reinterpret_cast<bsl::size_t>(ptr) & TAG_MASK) == 0);
    BSLS_ASSERT((tag & PTR_MASK) == 0);

    bsl::size_t number  = reinterpret_cast<bsl::size_t>(ptr) & PTR_MASK;
    number             |= tag & TAG_MASK;

    *address = reinterpret_cast<TYPE*>(number);
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE void BlobBufferPoolHandleUtil<TYPE, ALIGNMENT>::setPtr(
    RawType* address,
    PtrType  ptr)
{
    const bsl::size_t TAG_MASK = ALIGNMENT - 1;
    const bsl::size_t PTR_MASK = ~TAG_MASK;

    bsl::size_t number = reinterpret_cast<bsl::size_t>(*address);

    BSLS_ASSERT((reinterpret_cast<bsl::size_t>(ptr) & TAG_MASK) == 0);

    number =
        (reinterpret_cast<bsl::size_t>(ptr) & PTR_MASK) | (number & TAG_MASK);

    *address = reinterpret_cast<TYPE*>(number);
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE void BlobBufferPoolHandleUtil<TYPE, ALIGNMENT>::setTag(
    RawType* address,
    TagType  tag)
{
    const bsl::size_t TAG_MASK = ALIGNMENT - 1;
    const bsl::size_t PTR_MASK = ~TAG_MASK;

    bsl::size_t number = reinterpret_cast<bsl::size_t>(*address);

    BSLS_ASSERT((tag & PTR_MASK) == 0);

    number = (number & PTR_MASK) | (tag & TAG_MASK);

    *address = reinterpret_cast<TYPE*>(number);
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE void BlobBufferPoolHandleUtil<TYPE, ALIGNMENT>::get(
    PtrType* ptr,
    TagType* tag,
    RawType  address)
{
    const bsl::size_t TAG_MASK = ALIGNMENT - 1;
    const bsl::size_t PTR_MASK = ~TAG_MASK;

    bsl::size_t number = reinterpret_cast<bsl::size_t>(address);

    *ptr = reinterpret_cast<TYPE*>(number & PTR_MASK);
    *tag = number & TAG_MASK;
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE typename BlobBufferPoolHandleUtil<TYPE, ALIGNMENT>::PtrType
BlobBufferPoolHandleUtil<TYPE, ALIGNMENT>::getPtr(RawType address)
{
    const bsl::size_t TAG_MASK = ALIGNMENT - 1;
    const bsl::size_t PTR_MASK = ~TAG_MASK;

    bsl::size_t number = reinterpret_cast<bsl::size_t>(address);

    bsl::size_t result = number & PTR_MASK;

    return reinterpret_cast<TYPE*>(result);
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE typename BlobBufferPoolHandleUtil<TYPE, ALIGNMENT>::TagType
BlobBufferPoolHandleUtil<TYPE, ALIGNMENT>::getTag(RawType address)
{
    const bsl::size_t TAG_MASK = ALIGNMENT - 1;

    bsl::size_t number = reinterpret_cast<bsl::size_t>(address);

    bsl::size_t result = number & TAG_MASK;

    return result;
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE typename BlobBufferPoolHandleUtil<TYPE, ALIGNMENT>::TagType
BlobBufferPoolHandleUtil<TYPE, ALIGNMENT>::maxTag()
{
    return ALIGNMENT - 1;
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE bool BlobBufferPoolHandleUtil<TYPE, ALIGNMENT>::isAligned(
    PtrType ptr)
{
    const bsl::size_t TAG_MASK = ALIGNMENT - 1;

    return (reinterpret_cast<bsl::size_t>(ptr) & TAG_MASK) == 0;
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE bool BlobBufferPoolHandleUtil<TYPE, ALIGNMENT>::isValid(
    TagType tag)
{
    const bsl::size_t TAG_MASK = ALIGNMENT - 1;
    const bsl::size_t PTR_MASK = ~TAG_MASK;

    return ((tag & PTR_MASK) == 0);
}

/// @internal @brief
/// Provide an atomic tagged pointer to a parameterized type.
///
/// @details
/// This class implements an atomic tagged pointer to a parameterized
/// 'TYPE' allocated at an address having the parameterized power-of-two
/// 'ALIGNMENT'. This class supports common pointer operations in a way that
/// is guaranteed to be atomic.  Operations on objects of this class provide
/// the sequential consistency memory ordering guarantee unless explicitly
/// qualified with a less strict consistency guarantee suffix (i.e.,
/// Acquire, Release, AcqRel or Relaxed).
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
template <typename TYPE, unsigned ALIGNMENT>
class BlobBufferPoolHandle
{
  public:
    /// Define a type alias for the utility.
    typedef ntcs::BlobBufferPoolHandleUtil<TYPE, ALIGNMENT> Util;

    /// Define a type alias for the raw pointer type.
    typedef typename Util::RawType RawType;

    /// Define a type alias for the object pointer type.
    typedef typename Util::PtrType PtrType;

    /// Define a type alias for the tag type.
    typedef typename Util::TagType TagType;

  private:
    // DATA
    bsls::AtomicOperations::AtomicTypes::Pointer d_value;

  private:
    BlobBufferPoolHandle(const BlobBufferPoolHandle&) BSLS_KEYWORD_DELETED;
    BlobBufferPoolHandle& operator=(const BlobBufferPoolHandle&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create an atomic tagged pointer object having the default value of
    /// NULL with a tag set to zero.
    BlobBufferPoolHandle();

    /// Create an atomic pointer object having the specified 'value'.
    BlobBufferPoolHandle(PtrType object, TagType tag);

    /// Destroy this atomic pointer.
    ~BlobBufferPoolHandle();

    /// Atomically assign the specified 'value' to this object, providing
    /// the sequential consistency memory ordering guarantee.
    void store(PtrType newObject, TagType newTag);

    /// Atomically assign the specified 'value' to this object, providing
    /// the relaxed memory ordering guarantee.
    void storeRelaxed(PtrType newObject, TagType newTag);

    /// Atomically assign the specified 'value' to this object, providing
    /// the release memory ordering guarantee.
    void storeRelease(PtrType newObject, TagType newTag);

    /// Atomically set the value of this handle to point to the specified
    /// 'newObject' and 'newTag' and load into the specified 'oldObject' and
    /// 'oldTag' the previous object pointed to by this handle and the tag,
    /// respectively. Perform the operation with the sequential consistency
    /// memory ordering guarantee.
    void swap(PtrType* previousObject,
              TagType* previousTag,
              PtrType  newObject,
              TagType  newTag);

    /// Atomically set the value of this handle to point to the specified
    /// 'newObject' and 'newTag' and load into the specified 'oldObject' and
    /// 'oldTag' the previous object pointed to by this handle and the tag,
    /// respectively. Perform the operation with the acquire/release memory
    /// ordering guarantee.
    void swapAcqRel(PtrType* previousObject,
                    TagType* previousTag,
                    PtrType  newObject,
                    TagType  newTag);

    /// Compare the value of this object to the specified 'compareValue'.
    /// If they are equal, set the value of this atomic pointer to the
    /// specified 'swapValue', otherwise leave this value unchanged.  Return
    /// the previous value of this atomic pointer, whether or not the swap
    /// occurred.  Note that the entire test-and-swap operation is performed
    /// atomically.
    bool testAndSwap(PtrType* previousObject,
                     TagType* previousTag,
                     PtrType  expectedObject,
                     TagType  expectedTag,
                     PtrType  newObject,
                     TagType  newTag);

    /// Compare the value of this object to the specified 'compareValue'.
    /// If they are equal, set the value of this atomic pointer to the
    /// specified 'swapValue', otherwise leave this value unchanged.  Return
    /// the previous value of this atomic pointer, whether or not the swap
    /// occurred.  Note that the entire test-and-swap operation is performed
    /// atomically and it provides the acquire/release memory ordering
    /// guarantee.
    bool testAndSwapAcqRel(PtrType* previousObject,
                           TagType* previousTag,
                           PtrType  expectedObject,
                           TagType  expectedTag,
                           PtrType  newObject,
                           TagType  newTag);

    /// Return the current value of this object.
    void load(PtrType* currentObject, TagType* currentTag) const;

    /// Return the current value of this object, providing the relaxed
    /// memory ordering guarantee.
    void loadRelaxed(PtrType* currentObject, TagType* currentTag) const;

    /// Return the current value of this object, providing the acquire
    /// memory ordering guarantee.
    void loadAcquire(PtrType* currentObject, TagType* currentTag) const;

    /// Return the maximum tag value for the alignment.
    static TagType maxTag();
};

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE BlobBufferPoolHandle<TYPE, ALIGNMENT>::BlobBufferPoolHandle()
{
    bsls::AtomicOperations_Imp::initPointer(&d_value, 0);
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE BlobBufferPoolHandle<TYPE, ALIGNMENT>::BlobBufferPoolHandle(
    PtrType object,
    TagType tag)
{
    bsls::AtomicOperations_Imp::initPointer(&d_value,
                                            Util::initialize(object, tag));
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE BlobBufferPoolHandle<TYPE, ALIGNMENT>::~BlobBufferPoolHandle()
{
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE void BlobBufferPoolHandle<TYPE, ALIGNMENT>::store(
    PtrType newObject,
    TagType newTag)
{
    RawType newRaw;
    Util::set(&newRaw, newObject, newTag);

    bsls::AtomicOperations_Imp::setPtr(&d_value, newRaw);
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE void BlobBufferPoolHandle<TYPE, ALIGNMENT>::storeRelaxed(
    PtrType newObject,
    TagType newTag)
{
    RawType newRaw;
    Util::set(&newRaw, newObject, newTag);

    bsls::AtomicOperations_Imp::setPtrRelaxed(&d_value, newRaw);
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE void BlobBufferPoolHandle<TYPE, ALIGNMENT>::storeRelease(
    PtrType newObject,
    TagType newTag)
{
    RawType newRaw;
    Util::set(&newRaw, newObject, newTag);

    bsls::AtomicOperations_Imp::setPtrRelease(&d_value, newRaw);
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE void BlobBufferPoolHandle<TYPE, ALIGNMENT>::swap(
    PtrType* previousObject,
    TagType* previousTag,
    PtrType  newObject,
    TagType  newTag)
{
    RawType newRaw;
    Util::set(&newRaw, newObject, newTag);

    RawType previousRaw =
        bsls::AtomicOperations_Imp::swapPtr(&d_value, newRaw);

    Util::get(previousObject, previousTag, previousRaw);
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE void BlobBufferPoolHandle<TYPE, ALIGNMENT>::swapAcqRel(
    PtrType* previousObject,
    TagType* previousTag,
    PtrType  newObject,
    TagType  newTag)
{
    RawType newRaw;
    Util::set(&newRaw, newObject, newTag);

    RawType previousRaw =
        bsls::AtomicOperations_Imp::swapPtrAcqRel(&d_value, newRaw);

    Util::get(previousObject, previousTag, previousRaw);
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE bool BlobBufferPoolHandle<TYPE, ALIGNMENT>::testAndSwap(
    PtrType* previousObject,
    TagType* previousTag,
    PtrType  expectedObject,
    TagType  expectedTag,
    PtrType  newObject,
    TagType  newTag)
{
    RawType expectedRaw;
    Util::set(&expectedRaw, expectedObject, expectedTag);

    RawType newRaw;
    Util::set(&newRaw, newObject, newTag);

    RawType previousRaw =
        bsls::AtomicOperations_Imp::testAndSwapPtr(&d_value,
                                                   expectedRaw,
                                                   newRaw);

    Util::get(previousObject, previousTag, previousRaw);

    return (*previousObject == expectedObject) &&
           (*previousTag == expectedTag);
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE bool BlobBufferPoolHandle<TYPE, ALIGNMENT>::testAndSwapAcqRel(
    PtrType* previousObject,
    TagType* previousTag,
    PtrType  expectedObject,
    TagType  expectedTag,
    PtrType  newObject,
    TagType  newTag)
{
    RawType expectedRaw;
    Util::set(&expectedRaw, expectedObject, expectedTag);

    RawType newRaw;
    Util::set(&newRaw, newObject, newTag);

    RawType previousRaw =
        bsls::AtomicOperations_Imp::testAndSwapPtrAcqRel(&d_value,
                                                         expectedRaw,
                                                         newRaw);

    Util::get(previousObject, previousTag, previousRaw);

    return (*previousObject == expectedObject) &&
           (*previousTag == expectedTag);
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE void BlobBufferPoolHandle<TYPE, ALIGNMENT>::load(
    PtrType* currentObject,
    TagType* currentTag) const
{
    RawType currentRaw = bsls::AtomicOperations_Imp::getPtr(&d_value);
    Util::get(currentObject, currentTag, currentRaw);
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE void BlobBufferPoolHandle<TYPE, ALIGNMENT>::loadRelaxed(
    PtrType* currentObject,
    TagType* currentTag) const
{
    RawType currentRaw = bsls::AtomicOperations_Imp::getPtrRelaxed(&d_value);
    Util::get(currentObject, currentTag, currentRaw);
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE void BlobBufferPoolHandle<TYPE, ALIGNMENT>::loadAcquire(
    PtrType* currentObject,
    TagType* currentTag) const
{
    RawType currentRaw = bsls::AtomicOperations_Imp::getPtrAcquire(&d_value);
    Util::get(currentObject, currentTag, currentRaw);
}

template <typename TYPE, unsigned ALIGNMENT>
NTCCFG_INLINE typename BlobBufferPoolHandle<TYPE, ALIGNMENT>::TagType
BlobBufferPoolHandle<TYPE, ALIGNMENT>::maxTag()
{
    return Util::maxTag();
}

/// @internal @brief Provide padding to ensure the correct size of a blob
/// buffer pool object.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
struct BlobBufferPoolObjectPadding {
    BlobBufferPoolObjectPadding()
    {
        bsl::memset(this->zero, 0, sizeof this->zero);
    }

#if defined(BSLS_PLATFORM_CPU_64_BIT)
    bsl::uint8_t zero[16];
#elif defined(BSLS_PLATFORM_CPU_32_BIT)
    bsl::uint8_t zero[32];
#else
#error Not implemented
#endif
};

/// @internal @brief
/// Provide an intrusively linked-list of blob buffers implementing a shared
/// pointer representation.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class BlobBufferPoolObject : public bslma::SharedPtrRep
{
    char*                       d_data_p;
    BlobBufferPool*             d_pool_p;
    BlobBufferPoolObject*       d_next_p;
    bsls::AtomicUint64          d_generation;
    BlobBufferPoolObjectPadding d_padding;

  private:
    BlobBufferPoolObject(const BlobBufferPoolObject&) BSLS_KEYWORD_DELETED;
    BlobBufferPoolObject& operator=(const BlobBufferPoolObject&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new blob buffer pool object that belongs to the specified
    /// 'pool'.
    explicit BlobBufferPoolObject(BlobBufferPool* pool);

    /// Destroy this object.
    ~BlobBufferPoolObject() BSLS_KEYWORD_OVERRIDE;

    /// Set the blob buffer data to which this object manages to the
    /// specified 'data'.
    void setData(char* data);

    /// Set the next object in linked list to the specified 'next' object.
    void setNext(BlobBufferPoolObject* next);

    /// Increment the generation.
    void incrementGeneration();

// MRM
#if 0
    BlobBufferPoolObject* compareAndSwapNext(BlobBufferPoolObject* expected,
                                             BlobBufferPoolObject* next);
        // Set the next object in linked list to the specified 'next' object.
#endif

    /// Destroy the object referred to by this representation.  This method
    /// is invoked by 'releaseRef' when the number of shared references
    /// reaches zero and should not be explicitly invoked otherwise.
    void disposeObject() BSLS_KEYWORD_OVERRIDE;

    /// Destroy this representation object and deallocate the associated
    /// memory.  This method is invoked by 'releaseRef' and 'releaseWeakRef'
    /// when the number of weak references and the number of shared
    /// references both reach zero and should not be explicitly invoked
    /// otherwise.  The behavior is undefined unless 'disposeObject' has
    /// already been called for this representation.  Note that this method
    /// effectively serves as the representation object's destructor.
    void disposeRep() BSLS_KEYWORD_OVERRIDE;

    /// Return a pointer to the deleter stored by the derived representation
    /// if the deleter has the same type as that described by the specified
    /// 'type', and a null pointer otherwise.
    void* getDeleter(const bsl::type_info& type) BSLS_KEYWORD_OVERRIDE;

    /// Return the blob buffer data to which this object manages.
    char* data() const;

    /// Return the next object in linked list.
    BlobBufferPoolObject* next() const;

    /// Return the generation.
    bsl::uint64_t generation() const;

    /// Return the (untyped) address of the modifiable shared object to
    /// which this object refers.
    void* originalPtr() const BSLS_KEYWORD_OVERRIDE;
};

#define NTCS_BLOBBUFFERPOOL_DEBUG 0

/// @internal @brief
/// Provide a pool of blob buffers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class BlobBufferPool : public bdlbb::BlobBufferFactory
{
    enum {
        k_ALIGNMENT = 256  // 4096
    };

    typedef ntcs::BlobBufferPoolHandle<ntcs::BlobBufferPoolObject, k_ALIGNMENT>
        Handle;

#if NTCS_BLOBBUFFERPOOL_DEBUG
    enum {k_OBJECT_ARRAY_CAPACITY = 11};
#endif

    Handle      d_head;
    bsl::size_t d_blobBufferSize;

#if NTCS_BLOBBUFFERPOOL_DEBUG
    BlobBufferPoolObject* d_objectArray[k_OBJECT_ARRAY_CAPACITY];
    bsl::size_t           d_objectCount;
#endif

    bsls::AtomicUint64       d_numAllocated;
    bsls::AtomicUint64       d_numPooled;
    bsls::AtomicUint64       d_numBytesInUse;
    bdlma::AligningAllocator d_aligningAllocator;
    bslma::Allocator*        d_allocator_p;

  private:
    BlobBufferPool(const BlobBufferPool&) BSLS_KEYWORD_DELETED;
    BlobBufferPool& operator=(const BlobBufferPool&) BSLS_KEYWORD_DELETED;

  private:
    /// Replenish the pool with one more object.
    BlobBufferPoolObject* replenish();

  public:
    /// Create a new blob buffer pool that allocates blob buffers each
    /// having the specified 'blobBufferSize'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit BlobBufferPool(bsl::size_t       blobBufferSize,
                            bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~BlobBufferPool() BSLS_KEYWORD_OVERRIDE;

    /// Allocate a blob buffer from this blob buffer factory, and load it
    /// into the specified 'buffer'.
    void allocate(bdlbb::BlobBuffer* buffer) BSLS_KEYWORD_OVERRIDE;

    /// Return the specified 'object' to the pool.
    void release(BlobBufferPoolObject* object);

    /// Reserve the specified 'numObjects' to be available in the pool.
    void reserve(bsl::size_t numObjects);

    /// Return the number of blob buffers that have been allocated and not
    /// returned to the pool.
    bsl::size_t numBuffersAllocated() const;

    /// Return the number of blob buffers that are pooled but not allocated.
    bsl::size_t numBuffersAvailable() const;

    /// Return the total number of blob buffers that have been pooled, that
    /// is, the sum of the number of blob buffers that have been allocated
    /// and the number of blob buffers available.
    bsl::size_t numBuffersPooled() const;

    /// Return the number of bytes allocated from the allocator supplied
    /// to this object at the time of its construction and not yet freed.
    bsl::size_t numBytesInUse() const;
};

NTCCFG_INLINE
BlobBufferPoolObject::~BlobBufferPoolObject()
{
}

NTCCFG_INLINE
void BlobBufferPoolObject::setData(char* data)
{
    d_data_p = data;
}

NTCCFG_INLINE
void BlobBufferPoolObject::setNext(BlobBufferPoolObject* next)
{
    d_next_p = next;
}

NTCCFG_INLINE
void BlobBufferPoolObject::incrementGeneration()
{
    d_generation.add(1);
}

NTCCFG_INLINE
void BlobBufferPoolObject::disposeObject()
{
}

NTCCFG_INLINE
void BlobBufferPoolObject::disposeRep()
{
    d_pool_p->release(this);
}

NTCCFG_INLINE
void* BlobBufferPoolObject::getDeleter(const bsl::type_info& type)
{
    NTCCFG_WARNING_UNUSED(type);
    return 0;
}

NTCCFG_INLINE
char* BlobBufferPoolObject::data() const
{
    return d_data_p;
}

NTCCFG_INLINE
BlobBufferPoolObject* BlobBufferPoolObject::next() const
{
    return d_next_p;
}

NTCCFG_INLINE
bsl::uint64_t BlobBufferPoolObject::generation() const
{
    return d_generation.load();
}

NTCCFG_INLINE
void* BlobBufferPoolObject::originalPtr() const
{
    return (void*)(d_data_p);
}

}  // close package namespace
}  // close enterprise namespace
#endif
