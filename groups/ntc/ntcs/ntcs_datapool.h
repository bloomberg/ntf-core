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

#ifndef INCLUDED_NTCS_DATAPOOL
#define INCLUDED_NTCS_DATAPOOL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_datapool.h>
#include <ntcscm_version.h>
#include <bdlbb_blob.h>
#include <bdlcc_sharedobjectpool.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide a pool of blobs, blob buffers, and data containers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class DataPool : public ntci::DataPool
{
    /// Define a type alias for a pool of shared pointers to
    /// blobs.
    typedef bdlcc::SharedObjectPool<
        bdlbb::Blob,
        bdlcc::ObjectPoolFunctors::DefaultCreator,
        bdlcc::ObjectPoolFunctors::RemoveAll<bdlbb::Blob> >
        BlobPool;

    /// Define a type alias for a pool of shared pointers to
    /// blobs.
    typedef bdlcc::SharedObjectPool<
        ntsa::Data,
        bdlcc::ObjectPoolFunctors::DefaultCreator,
        bdlcc::ObjectPoolFunctors::Reset<ntsa::Data> >
        DataContainerPool;

    bsl::shared_ptr<bdlbb::BlobBufferFactory> d_incomingBlobBufferFactory_sp;
    bsl::shared_ptr<bdlbb::BlobBufferFactory> d_outgoingBlobBufferFactory_sp;
    BlobPool                                  d_incomingBlobPool;
    BlobPool                                  d_outgoingBlobPool;
    DataContainerPool                         d_incomingDataContainerPool;
    DataContainerPool                         d_outgoingDataContainerPool;
    bslma::Allocator*                         d_allocator_p;

  private:
    DataPool(const DataPool&) BSLS_KEYWORD_DELETED;
    DataPool& operator=(const DataPool&) BSLS_KEYWORD_DELETED;

  private:
    /// Return a new blob buffer factory that allocates blob buffers each
    /// having the specified 'blobBufferSize'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    static bsl::shared_ptr<bdlbb::BlobBufferFactory> createBlobBufferFactory(
        bsl::size_t       blobBufferSize,
        bslma::Allocator* basicAllocator = 0);

    /// Construct a new blob, suitable to store incoming data, at the
    /// specified 'address' using the specified 'allocator' to supply
    /// memory.
    static void constructIncomingBlob(
        void*                                            address,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator*                                allocator);

    /// Construct a new blob, suitable to store outgoing data, at the
    /// specified 'address' using the specified 'allocator' to supply
    /// memory.
    static void constructOutgoingBlob(
        void*                                            address,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator*                                allocator);

    /// Construct a new data container, suitable to store incoming data, at
    /// the specified 'address' using the specified 'allocator' to supply
    /// memory.
    static void constructIncomingData(
        void*                                            address,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator*                                allocator);

    /// Construct a new data container, suitable to store outgoing data, at
    /// the specified 'address' using the specified 'allocator' to supply
    /// memory.
    static void constructOutgoingData(
        void*                                            address,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator*                                allocator);

  public:
    /// Create a new data pool using the default sizes for incoming and
    /// outgoing blob buffers. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit DataPool(bslma::Allocator* basicAllocator = 0);

    /// Create a new data pool using the specified 'incomingBlobBufferSize'
    /// and 'outgoingBlobBufferSize'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    DataPool(bsl::size_t       incomingBlobBufferSize,
             bsl::size_t       outgoingBlobBufferSize,
             bslma::Allocator* basicAllocator = 0);

    /// Create a new data pool using the specified
    /// 'incomingBlobBufferFactory' and 'outgoingBlobBufferFactory'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    ///  used.
    DataPool(const bsl::shared_ptr<bdlbb::BlobBufferFactory>&
                 incomingBlobBufferFactory,
             const bsl::shared_ptr<bdlbb::BlobBufferFactory>&
                               outgoingBlobBufferFactory,
             bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~DataPool() BSLS_KEYWORD_OVERRIDE;

    /// Return a shared pointer to a data container suitable for storing
    /// incoming data. The resulting data container is is automatically
    /// returned to this pool when its reference count reaches zero.
    bsl::shared_ptr<ntsa::Data> createIncomingData() BSLS_KEYWORD_OVERRIDE;

    /// Return a shared pointer to a data container suitable for storing
    /// outgoing data. The resulting data container is is automatically
    /// returned to this pool when its reference count reaches zero.
    bsl::shared_ptr<ntsa::Data> createOutgoingData() BSLS_KEYWORD_OVERRIDE;

    /// Return a shared pointer to a blob suitable for storing incoming
    /// data. The resulting blob is is automatically returned to this pool
    /// when its reference count reaches zero.
    bsl::shared_ptr<bdlbb::Blob> createIncomingBlob() BSLS_KEYWORD_OVERRIDE;

    /// Return a shared pointer to a blob suitable for storing incoming
    /// data. The resulting blob is is automatically returned to this pool
    /// when its reference count reaches zero.
    bsl::shared_ptr<bdlbb::Blob> createOutgoingBlob() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'blobBuffer' the data and size of a new
    /// buffer allocated from the incoming blob buffer factory.
    void createIncomingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'blobBuffer' the data and size of a new
    /// buffer allocated from the outgoing blob buffer factory.
    void createOutgoingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
        BSLS_KEYWORD_OVERRIDE;

    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& incomingBlobBufferFactory()
        const BSLS_KEYWORD_OVERRIDE;
    // Return the incoming blob buffer factory.

    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& outgoingBlobBufferFactory()
        const BSLS_KEYWORD_OVERRIDE;
    // Return the outgoing blob buffer factory.
};

NTCCFG_INLINE
bsl::shared_ptr<ntsa::Data> DataPool::createIncomingData()
{
    return d_incomingDataContainerPool.getObject();
}

NTCCFG_INLINE
bsl::shared_ptr<ntsa::Data> DataPool::createOutgoingData()
{
    return d_outgoingDataContainerPool.getObject();
}

NTCCFG_INLINE
bsl::shared_ptr<bdlbb::Blob> DataPool::createIncomingBlob()
{
    return d_incomingBlobPool.getObject();
}

NTCCFG_INLINE
bsl::shared_ptr<bdlbb::Blob> DataPool::createOutgoingBlob()
{
    return d_outgoingBlobPool.getObject();
}

NTCCFG_INLINE
void DataPool::createIncomingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_incomingBlobBufferFactory_sp->allocate(blobBuffer);
}

NTCCFG_INLINE
void DataPool::createOutgoingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_outgoingBlobBufferFactory_sp->allocate(blobBuffer);
}

NTCCFG_INLINE
const bsl::shared_ptr<bdlbb::BlobBufferFactory>& DataPool::
    incomingBlobBufferFactory() const
{
    return d_incomingBlobBufferFactory_sp;
}

NTCCFG_INLINE
const bsl::shared_ptr<bdlbb::BlobBufferFactory>& DataPool::
    outgoingBlobBufferFactory() const
{
    return d_outgoingBlobBufferFactory_sp;
}

}  // close package namespace
}  // close enterprise namespace
#endif
