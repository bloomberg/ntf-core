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

#ifndef INCLUDED_NTSA_SOCKETSTATE
#define INCLUDED_NTSA_SOCKETSTATE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an enumeration of the socket states.
///
/// @par Thread Safety
// This class is thread safe.
///
/// @ingroup module_ntsa_system
struct SocketState {
  public:
    /// Provide an enumeration of the socket states.
    enum Value {
        /// The state is undefined.
        e_UNDEFINED = 0,

        /// The socket is established.
        e_ESTABLISHED = 1,

        /// The socket has sent a SYN packet.
        e_SYN_SENT = 2,

        /// The socket has received a SYN packet.
        e_SYN_RECV = 3,

        /// The socket has sent a FIN packet without first having received a
        /// FIN packet (i.e. an active close has been performed.)
        e_FIN_WAIT1 = 4,

        /// The socket has received an ACK for its FIN packet, but has not yet
        /// received a FIN packet from the peer.
        e_FIN_WAIT2 = 5,

        /// The socket is waiting for twice the maximum segment lifetime
        /// to elapse to ensure that the remote peer received the
        /// acknowledgement before automatically transitioning to the CLOSED
        /// state.
        e_TIME_WAIT = 6,

        /// The socket is closed.
        e_CLOSED = 7,

        /// The socket has received a FIN packet and acknowledged it without
        /// first having sent a FIN packet itself (i.e. a passive close has
        /// been detected.)
        e_CLOSE_WAIT = 8,

        /// The socket has detected a passive close and has initiated an active
        /// close by sending a FIN packet to the peer.
        e_LAST_ACK = 9,

        /// The socket is listening for connections.
        e_LISTEN = 10,

        /// The socket has sent a FIN packet and received a FIN packet without
        /// first receiving an acknowledgement of its FIN packet.
        e_CLOSING = 11
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
/// @related ntsa::SocketState
bsl::ostream& operator<<(bsl::ostream& stream, SocketState::Value rhs);

}  // close package namespace
}  // close enterprise namespace
#endif
