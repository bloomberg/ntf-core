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

#ifndef INCLUDED_NTCA_WRITEQUEUEEVENTTYPE
#define INCLUDED_NTCA_WRITEQUEUEEVENTTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntca {

/// Enumerate the write queue event types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntci_operation_send
struct WriteQueueEventType {
  public:
    /// Enumerate the write queue event types.
    enum Value {
        /// Flow control has been relaxed: the write queue is being
        /// automatically copied to the socket send buffer.
        e_FLOW_CONTROL_RELAXED = 0,

        /// Flow control has been applied: the write queue is not being
        /// automatically copied to the socket send buffer.
        e_FLOW_CONTROL_APPLIED = 1,

        /// The write queue size is greater than or equal to the write queue
        /// low watermark.
        e_LOW_WATERMARK = 2,

        /// The write queue size is greater than the write queue high
        /// watermark.
        e_HIGH_WATERMARK = 3,

        /// The contents of the write queue have been discarded without being
        /// processed.
        e_DISCARDED = 4,

        /// The send rate limit has been reached and the send rate limit timer
        /// has been set.
        e_RATE_LIMIT_APPLIED = 5,

        /// The send rate limit timer has fired and the send rate limit has
        /// been relaxed.
        e_RATE_LIMIT_RELAXED = 6
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
/// @related ntca::WriteQueueEventType
bsl::ostream& operator<<(bsl::ostream& stream, WriteQueueEventType::Value rhs);

}  // end namespace ntca
}  // end namespace BloombergLP
#endif
