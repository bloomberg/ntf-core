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

#ifndef INCLUDED_NTCA_STREAMSOCKETEVENTTYPE
#define INCLUDED_NTCA_STREAMSOCKETEVENTTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntca {

/// Enumerate the stream socket event types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntci_socket
struct StreamSocketEventType {
  public:
    /// Enumerate the stream socket event types.
    enum Value {
        /// The stream socket event type is undefined.
        e_UNDEFINED = 0,

        /// The stream socket event type is a read queue event.
        e_READ_QUEUE = 1,

        /// The stream socket event type is a write queue event.
        e_WRITE_QUEUE = 2,

        /// The stream socket event type is a downgrade event.
        e_DOWNGRADE = 3,

        /// The stream socket event type is a shutdown event.
        e_SHUTDOWN = 4,

        /// The stream socket event type is an error event.
        e_ERROR = 5
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
/// @related ntca::StreamSocketEventType
bsl::ostream& operator<<(bsl::ostream&                stream,
                         StreamSocketEventType::Value rhs);

}  // end namespace ntca
}  // end namespace BloombergLP
#endif
