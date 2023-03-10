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

#include <ntcs_datapool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_datapool_cpp, "$Id$ $CSID$")

#include <ntccfg_bind.h>
#include <ntccfg_limits.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcs {

bsl::shared_ptr<bdlbb::BlobBufferFactory> DataPool::createBlobBufferFactory(
    bsl::size_t       blobBufferSize,
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<bdlbb::PooledBlobBufferFactory> blobBufferFactory;
    blobBufferFactory.createInplace(allocator,
                                    NTCCFG_WARNING_NARROW(int, blobBufferSize),
                                    allocator);

    return blobBufferFactory;
}

void DataPool::constructIncomingBlob(
    void*                                            address,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                allocator)
{
    BSLS_ASSERT(allocator);
    BSLS_ASSERT(blobBufferFactory);

    new (address) bdlbb::Blob(blobBufferFactory.get(), allocator);
}

void DataPool::constructOutgoingBlob(
    void*                                            address,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                allocator)
{
    BSLS_ASSERT(allocator);
    BSLS_ASSERT(blobBufferFactory);

    new (address) bdlbb::Blob(blobBufferFactory.get(), allocator);
}

void DataPool::constructIncomingData(
    void*                                            address,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                allocator)
{
    BSLS_ASSERT(allocator);
    BSLS_ASSERT(blobBufferFactory);

    new (address) ntsa::Data(blobBufferFactory.get(), allocator);
}

void DataPool::constructOutgoingData(
    void*                                            address,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                allocator)
{
    BSLS_ASSERT(allocator);
    BSLS_ASSERT(blobBufferFactory);

    new (address) ntsa::Data(blobBufferFactory.get(), allocator);
}

DataPool::DataPool(bslma::Allocator* basicAllocator)
: d_incomingBlobBufferFactory_sp(DataPool::createBlobBufferFactory(
      NTCCFG_DEFAULT_INCOMING_BLOB_BUFFER_SIZE,
      basicAllocator))
, d_outgoingBlobBufferFactory_sp(DataPool::createBlobBufferFactory(
      NTCCFG_DEFAULT_OUTGOING_BLOB_BUFFER_SIZE,
      basicAllocator))
, d_incomingBlobPool(NTCCFG_BIND(&DataPool::constructIncomingBlob,
                                 NTCCFG_BIND_PLACEHOLDER_1,
                                 d_incomingBlobBufferFactory_sp,
                                 NTCCFG_BIND_PLACEHOLDER_2),
                     1,
                     basicAllocator)
, d_outgoingBlobPool(NTCCFG_BIND(&DataPool::constructOutgoingBlob,
                                 NTCCFG_BIND_PLACEHOLDER_1,
                                 d_outgoingBlobBufferFactory_sp,
                                 NTCCFG_BIND_PLACEHOLDER_2),
                     1,
                     basicAllocator)

, d_incomingDataContainerPool(NTCCFG_BIND(&DataPool::constructIncomingData,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          d_incomingBlobBufferFactory_sp,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              1,
                              basicAllocator)
, d_outgoingDataContainerPool(NTCCFG_BIND(&DataPool::constructOutgoingData,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          d_outgoingBlobBufferFactory_sp,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              1,
                              basicAllocator)

, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

DataPool::DataPool(bsl::size_t       incomingBlobBufferSize,
                   bsl::size_t       outgoingBlobBufferSize,
                   bslma::Allocator* basicAllocator)
: d_incomingBlobBufferFactory_sp(
      DataPool::createBlobBufferFactory(incomingBlobBufferSize,
                                        basicAllocator))
, d_outgoingBlobBufferFactory_sp(
      DataPool::createBlobBufferFactory(outgoingBlobBufferSize,
                                        basicAllocator))
, d_incomingBlobPool(NTCCFG_BIND(&DataPool::constructIncomingBlob,
                                 NTCCFG_BIND_PLACEHOLDER_1,
                                 d_incomingBlobBufferFactory_sp,
                                 NTCCFG_BIND_PLACEHOLDER_2),
                     1,
                     basicAllocator)
, d_outgoingBlobPool(NTCCFG_BIND(&DataPool::constructOutgoingBlob,
                                 NTCCFG_BIND_PLACEHOLDER_1,
                                 d_outgoingBlobBufferFactory_sp,
                                 NTCCFG_BIND_PLACEHOLDER_2),
                     1,
                     basicAllocator)

, d_incomingDataContainerPool(NTCCFG_BIND(&DataPool::constructIncomingData,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          d_incomingBlobBufferFactory_sp,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              1,
                              basicAllocator)
, d_outgoingDataContainerPool(NTCCFG_BIND(&DataPool::constructOutgoingData,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          d_outgoingBlobBufferFactory_sp,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              1,
                              basicAllocator)

, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

DataPool::DataPool(
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& incomingBlobBufferFactory,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& outgoingBlobBufferFactory,
    bslma::Allocator*                                basicAllocator)
: d_incomingBlobBufferFactory_sp(incomingBlobBufferFactory)
, d_outgoingBlobBufferFactory_sp(outgoingBlobBufferFactory)
, d_incomingBlobPool(NTCCFG_BIND(&DataPool::constructIncomingBlob,
                                 NTCCFG_BIND_PLACEHOLDER_1,
                                 d_incomingBlobBufferFactory_sp,
                                 NTCCFG_BIND_PLACEHOLDER_2),
                     1,
                     basicAllocator)
, d_outgoingBlobPool(NTCCFG_BIND(&DataPool::constructOutgoingBlob,
                                 NTCCFG_BIND_PLACEHOLDER_1,
                                 d_outgoingBlobBufferFactory_sp,
                                 NTCCFG_BIND_PLACEHOLDER_2),
                     1,
                     basicAllocator)

, d_incomingDataContainerPool(NTCCFG_BIND(&DataPool::constructIncomingData,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          d_incomingBlobBufferFactory_sp,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              1,
                              basicAllocator)
, d_outgoingDataContainerPool(NTCCFG_BIND(&DataPool::constructOutgoingData,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          d_outgoingBlobBufferFactory_sp,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              1,
                              basicAllocator)

, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

DataPool::~DataPool()
{
}

}  // close package namespace
}  // close enterprise namespace
