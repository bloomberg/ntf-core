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

#ifndef INCLUDED_NTSA_SOCKETOPTIONTYPE
#define INCLUDED_NTSA_SOCKETOPTIONTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an enumeration of the socket option types.
//
/// @par Thread Safety
// This class is thread safe.
///
/// @ingroup module_ntsa_system
struct SocketOptionType {
  public:
    /// Provide an enumeration of the socket option types.
    enum Value {
        /// The socket option type is undefined.
        e_UNDEFINED = 0,

        /// The operating system should allow the user to bind a socket to
        /// reuse local addresses.
        e_REUSE_ADDRESS = 1,

        /// The operating system should periodically emit transport-level
        /// "keep-alive" packets.
        e_KEEP_ALIVE = 2,

        /// Queue partial frames until the option is cleared, or an operating
        /// system timeout is reached.
        e_CORK = 3,

        /// Enable or disable packet coalescing, otherwise known as Nagle's
        /// algorith.
        e_DELAY_TRANSMISSION = 4,

        /// Enable or disable delayed acknowledgement.
        e_DELAY_ACKNOWLEDGEMENT = 5,

        /// The size of the send buffer.
        e_SEND_BUFFER_SIZE = 6,

        /// The minimum capacity that must be available in the socket send
        /// buffer for the socket to be considered writable.
        e_SEND_BUFFER_LOW_WATERMARK = 7,

        /// The size of the receive buffer.
        e_RECEIVE_BUFFER_SIZE = 8,

        /// The minimum size of the socket receive buffer for the socket to
        /// be considered readable.
        e_RECEIVE_BUFFER_LOW_WATERMARK = 9,

        /// Enable socket debugging and diagnostics.
        e_DEBUG = 10,

        /// Set the linger behavior.
        e_LINGER = 11,

        /// Allow datagram sockets to send to a broadcast address.
        e_BROADCAST = 12,

        /// Do not send packets throught a gateway.
        e_BYPASS_ROUTING = 13,

        /// Place out-of-band data into the normal incoming data stream.
        e_INLINE_OUT_OF_BAND_DATA = 14,

        /// Generate timestamps for incoming data.
        e_RX_TIMESTAMPING = 15,

        /// Generate timestamps for outgoing data.
        e_TX_TIMESTAMPING = 16,

        /// Allow each send operation to request copy avoidance when enqueing
        /// data to the socket send buffer.
        e_ZERO_COPY = 17
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
/// @related ntsa::SocketOptionType
bsl::ostream& operator<<(bsl::ostream& stream, SocketOptionType::Value rhs);

}  // close package namespace
}  // close enterprise namespace
#endif
