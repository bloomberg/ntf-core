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

#ifndef INCLUDED_NTCI_COMPRESSIONDRIVER
#define INCLUDED_NTCI_COMPRESSIONDRIVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_compression.h>
#include <ntci_datapool.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// Provide a factory for a mechanism to deflate and inflate a data stream.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_compression
class CompressionDriver
{
  public:
    /// Destroy this object.
    virtual ~CompressionDriver();

    /// Load into the specified 'result' a new compression mechanism with the
    /// specified 'configuration'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    virtual ntsa::Error createCompression(
        bsl::shared_ptr<ntci::Compression>* result,
        const ntca::CompressionConfig&      configuration,
        bslma::Allocator*                   basicAllocator = 0) = 0;

    /// Load into the specified 'result' a new compression mechanism with the
    /// specified 'options'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    virtual ntsa::Error createCompression(
        bsl::shared_ptr<ntci::Compression>*              result,
        const ntca::CompressionConfig&                   configuration,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator* basicAllocator = 0) = 0;

    /// Load into the specified 'result' a new compression mechanism with the
    /// specified 'configuration'. Allocate data containers using the specified
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used. Return the error.
    virtual ntsa::Error createCompression(
        bsl::shared_ptr<ntci::Compression>*    result,
        const ntca::CompressionConfig&         configuration,
        const bsl::shared_ptr<ntci::DataPool>& dataPool,
        bslma::Allocator*                      basicAllocator = 0) = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
