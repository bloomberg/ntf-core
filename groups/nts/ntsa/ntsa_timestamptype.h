// Copyright 2023 Bloomberg Finance L.P.
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

#ifndef INCLUDED_NTSA_TIMESTAMPTYPE
#define INCLUDED_NTSA_TIMESTAMPTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an enumeration of the outgoing timestamp types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_identity
struct TimestampType {
  public:
    /// Provide an enumeration of the timestamp types.
    enum Value {
        /// The timestamp type is undefined.
        e_UNDEFINED = 0,

        /// The timestamp measured at the time when the data enters the 
        /// packet scheduler. The delta between such a timestamp and the time
        /// immediately before the data is enqueued to the send buffer is the
        /// time spent processing the data required by transport protocol.
        e_SCHEDULED    = 1,

        /// The timestamp measured at the time when the data leaves the
        /// operating system and is enqueue in the network device for 
        /// transmission. The delta between such a timestamp and the scheduled
        /// timestamp is the time spending processing the data independant of
        /// the transport protocol.
        e_SENT = 2,

        /// The timestamp measured at the time when the ackowledgement of the
        /// outgoing data has been received from the peer, for positive 
        /// acknowledgement transport protocols such as TCP.
        e_ACKNOWLEDGED = 3
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
/// @related ntsa::TimestampType
bsl::ostream& operator<<(bsl::ostream& stream, TimestampType::Value rhs);

}  // close package namespace
}  // close enterprise namespace
#endif
