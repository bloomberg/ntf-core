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

#ifndef INCLUDED_NTCDNS_COMPAT
#define INCLUDED_NTCDNS_COMPAT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_getdomainnameoptions.h>
#include <ntca_getendpointoptions.h>
#include <ntca_getipaddressoptions.h>
#include <ntca_getportoptions.h>
#include <ntca_getservicenameoptions.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_endpointoptions.h>
#include <ntsa_error.h>
#include <ntsa_ipaddressoptions.h>
#include <ntsa_portoptions.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcdns {

/// @internal @brief
/// Provide conversion functions for DNS vocabulary types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcdns
struct Compat {
    /// Load into the specified 'result' the conversion of the specified
    /// 'options'.
    static void convert(ntca::GetIpAddressOptions*      result,
                        const ntca::GetEndpointOptions& options);

    /// Load into the specified 'result' the conversion of the specified
    /// 'options'.
    static void convert(ntca::GetPortOptions*           result,
                        const ntca::GetEndpointOptions& options);

    /// Load into the specified 'result' the conversion of the specified
    /// 'options'.
    static void convert(ntsa::IpAddressOptions*          result,
                        const ntca::GetIpAddressOptions& options);

    /// Load into the specified 'result' the conversion of the specified
    /// 'options'.
    static void convert(ntsa::PortOptions*          result,
                        const ntca::GetPortOptions& options);

    /// Load into the specified 'result' the effective IP address type, if
    /// any, from the specified validated and evaluated 'options'. Return
    /// the error.
    static ntsa::Error convert(
        bdlb::NullableValue<ntsa::IpAddressType::Value>* result,
        const ntca::GetIpAddressOptions&                 options);
};

}  // end namespace ntcdns
}  // end namespace BloombergLP
#endif
