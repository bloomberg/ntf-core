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

#ifndef INCLUDED_NTCA_ERROREVENTTYPE
#define INCLUDED_NTCA_ERROREVENTTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntca {

/// Enumerate the error event types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntci_operation_failure
struct ErrorEventType {
  public:
    /// Enumerate the error event types.
    enum Value {
        /// The error type is unknown.
        e_UNKNOWN = 0,

        /// An error was detected on a listening socket.
        e_LISTEN = 1,

        /// An error was detected during an attempt to accept a connection
        /// from a listening socket's backlog.
        e_ACCEPT = 2,

        /// An error was detected attempting to establish a connection to a
        /// peer.
        e_CONNECT = 3,

        /// An error was detected attempting to upgrade a connection from a
        /// raw transport to an encrypted session.
        e_UPGRADE = 4,

        /// An error was detected attempting to encrypt data to send to a peer.
        e_ENCRYPT = 5,

        /// An error was detected on the socket transport or network protocol.
        e_TRANSPORT = 6,

        /// An error was deteced attempting to decrypt data received from a
        /// peer.
        e_DECRYPT = 7,

        /// An error was detected attempting to downgrade an encrypted session
        /// to a raw transport.
        e_DOWNGRADE = 8,

        /// An error was detected attempting to shut down a connection.
        e_SHUTDOWN = 9
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

// FREE OPERATORS

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntca::ErrorEventType
bsl::ostream& operator<<(bsl::ostream& stream, ErrorEventType::Value rhs);

}  // end namespace ntca
}  // end namespace BloombergLP
#endif
