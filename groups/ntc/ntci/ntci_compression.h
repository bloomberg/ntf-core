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

#ifndef INCLUDED_NTCI_COMPRESSION
#define INCLUDED_NTCI_COMPRESSION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_deflatecontext.h>
#include <ntca_deflateoptions.h>
#include <ntca_inflatecontext.h>
#include <ntca_inflateoptions.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_buffer.h>
#include <ntsa_data.h>
#include <ntsa_error.h>
#include <bdlbb_blob.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to a deflation/inflation session.
///
/// @details
/// This class provides a mechanism to reduce the size of a data stream
/// according to a compression technique.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_encryption
class Compression
{
  public:

    /// Destroy this object.
    virtual ~Compression();

    /// Deflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    virtual ntsa::Error deflate(ntca::DeflateContext*       context,
                                bdlbb::Blob*                result,
                                const bdlbb::Blob&          data,
                                const ntca::DeflateOptions& options);

    /// Deflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    virtual ntsa::Error deflate(ntca::DeflateContext*       context,
                                bdlbb::Blob*                result,
                                const ntsa::Data&           data,
                                const ntca::DeflateOptions& options);

    /// Inflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    virtual ntsa::Error inflate(ntca::InflateContext*       context,
                                bdlbb::Blob*                result,
                                const bdlbb::Blob&          data,
                                const ntca::InflateOptions& options);

    /// Inflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    virtual ntsa::Error inflate(ntca::InflateContext*       context,
                                bdlbb::Blob*                result,
                                const ntsa::Data&           data,
                                const ntca::InflateOptions& options);
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
