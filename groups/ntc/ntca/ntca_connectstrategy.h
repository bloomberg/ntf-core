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

#ifndef INCLUDED_NTCA_CONNECTSTRATEGY
#define INCLUDED_NTCA_CONNECTSTRATEGY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntca {

/// Enumerate the connect strategies that determine how to interpret the
/// results of name resolution.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntci_operation_connect
class ConnectStrategy
{
  public:
    /// Enumerate the connect strategies that determine how to interpret the
    /// results of name resolution.
    enum Value {
        /// Resolve the name upon each attempt and pick one endpoint given by
        /// the resolver, according to the IP address selector, IP address
        /// filter, port selector, and port filer, depending on which, if any,
        /// are defined.
        e_RESOLVE_INTO_SINGLE,

        /// Resolve the name once before all attempts are first begun,
        /// save the IP address list, retry each IP address in order, only
        /// re-resolving after a connection attempt has been tried to each
        /// address and failed.
        e_RESOLVE_INTO_LIST
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntca::ConnectEventType
bsl::ostream& operator<<(bsl::ostream& stream, ConnectStrategy::Value rhs);

}  // end namespace ntca
}  // end namespace BloombergLP
#endif
