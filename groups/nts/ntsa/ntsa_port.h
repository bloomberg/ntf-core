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

#ifndef INCLUDED_NTSA_PORT
#define INCLUDED_NTSA_PORT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>

namespace BloombergLP {
namespace ntsa {

/// Define a type alias for a port number.
///
/// @ingroup module_ntsa_identity
typedef bsl::uint16_t Port;

/// Provide utilities for encoding and decoding port numbers.
///
/// @par Thread Safey
/// This struct is thread safe.
///
/// @ingroup module_ntsa_identity
struct PortUtil {
    enum {
        /// The maximum length of the string representation of a port number,
        /// not including the null terminator.
        MAX_LENGTH = 5
    };

    /// Encode the specified 'port' to the specified 'destination' having
    /// the specified 'capacity'. If 'destination' has sufficient capacity,
    /// null-terminate 'destination'. Return the number of bytes written,
    /// not including the null terminator.
    static bsl::size_t format(char*       destination,
                              bsl::size_t capacity,
                              ntsa::Port  port);

    /// Load into the specified 'result' the port decoded from the specified
    /// 'source' having the specified 'size'. Return true if the
    /// 'source' contains a valid port, and false otherwise.
    static bool parse(ntsa::Port* result,
                      const char* source,
                      bsl::size_t size);
};

}  // close package namespace
}  // close enterprise namespace
#endif
