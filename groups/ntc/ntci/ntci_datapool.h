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

#ifndef INCLUDED_NTCI_DATAPOOL
#define INCLUDED_NTCI_DATAPOOL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_data.h>
#include <bdlbb_blob.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to create blobs, blob buffers, and data containers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_utility
class DataPool
{
  public:
    /// Destroy this object.
    virtual ~DataPool();

    /// Return a shared pointer to a data container suitable for storing
    /// incoming data. The resulting data container is is automatically
    /// returned to this pool when its reference count reaches zero.
    virtual bsl::shared_ptr<ntsa::Data> createIncomingData() = 0;

    /// Return a shared pointer to a data container suitable for storing
    /// outgoing data. The resulting data container is is automatically
    /// returned to this pool when its reference count reaches zero.
    virtual bsl::shared_ptr<ntsa::Data> createOutgoingData() = 0;

    /// Return a shared pointer to a blob suitable for storing incoming
    /// data. The resulting blob is is automatically returned to this pool
    /// when its reference count reaches zero.
    virtual bsl::shared_ptr<bdlbb::Blob> createIncomingBlob() = 0;

    /// Return a shared pointer to a blob suitable for storing incoming
    /// data. The resulting blob is is automatically returned to this pool
    /// when its reference count reaches zero.
    virtual bsl::shared_ptr<bdlbb::Blob> createOutgoingBlob() = 0;

    /// Load into the specified 'blobBuffer' the data and size of a new
    /// buffer allocated from the incoming blob buffer factory.
    virtual void createIncomingBlobBuffer(bdlbb::BlobBuffer* blobBuffer) = 0;

    /// Load into the specified 'blobBuffer' the data and size of a new
    /// buffer allocated from the outgoing blob buffer factory.
    virtual void createOutgoingBlobBuffer(bdlbb::BlobBuffer* blobBuffer) = 0;

    /// Return the incoming blob buffer factory.
    virtual const bsl::shared_ptr<bdlbb::BlobBufferFactory>&
    incomingBlobBufferFactory() const = 0;

    /// Return the outgoing blob buffer factory.
    virtual const bsl::shared_ptr<bdlbb::BlobBufferFactory>&
    outgoingBlobBufferFactory() const = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
