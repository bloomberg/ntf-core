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

#ifndef INCLUDED_NTSA_IPV6SCOPEID
#define INCLUDED_NTSA_IPV6SCOPEID

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>

namespace BloombergLP {
namespace ntsa {

/// Define a type alias for an Ipv6 scope ID.
///
/// @ingroup module_ntsa_identity
typedef bsl::uint32_t Ipv6ScopeId;

/// Provide utilities for encoding and decoding IPv6 scope IDs.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_identity
struct Ipv6ScopeIdUtil {
    enum {
        /// The maximum length of the string representation of an IPv6 scope
        /// ID, not including the null terminator.
        MAX_LENGTH = 5
    };

    /// Encode the specified 'scopeId' to the specified 'destination' having
    /// the specified 'capacity'. Return the number of bytes written.
    static bsl::size_t format(char*             destination,
                              bsl::size_t       capacity,
                              ntsa::Ipv6ScopeId scopeId);

    /// Load into the specified 'result' the scope ID decoded from the
    /// specified 'source' having the specified 'size'. Return true if the
    /// 'source' contains a valid port, and false otherwise.
    static bool parse(ntsa::Ipv6ScopeId* result,
                      const char*        source,
                      bsl::size_t        size);
};

}  // close package namespace
}  // close enterprise namespace
#endif
