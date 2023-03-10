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

#ifndef INCLUDED_NTCS_SHUTDOWNSTATE
#define INCLUDED_NTCS_SHUTDOWNSTATE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcs_shutdowncontext.h>
#include <ntcscm_version.h>
#include <ntsa_shutdownorigin.h>
#include <ntsa_shutdowntype.h>
#include <bsls_atomic.h>
#include <bsls_spinlock.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide a mechanism to sequence the shutdown of a stream socket
///
/// @details
/// Provide a mechanism to assist in the implementation of a socket
/// when sequencing the shutdown of reads and writes for that socket.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class ShutdownState
{
    ntcs::ShutdownContext d_context;

  private:
    ShutdownState(const ShutdownState&) BSLS_KEYWORD_DELETED;
    ShutdownState& operator=(const ShutdownState&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new object in the default state.
    ShutdownState();

    /// Destroy this object.
    ~ShutdownState();

    /// Shut down sending by the stream socket being modeled and proceed
    /// through the shutdown sequence according to the specified
    /// 'keepHalfOpen' mode. On success, set in the specified
    /// 'context' the flag indicating whether this operation should cause
    /// the announcement of the initiation of the shutdown sequence, set in
    /// the specified 'context' the flag indicating whether the socket
    /// should be subsequently shut down for sending, set in the specified
    /// 'context' the flag indicating whether the socket should be
    /// subsequently shut down for receiving, set in the specified 'context'
    /// the flag indicating whether this operation should cause the
    /// announcement of the completion of the shutdown sequence, i.e., this
    /// operation has resulted in both sending and receiving being shut down
    /// for the stream socket being modeled (thus the socket should be
    /// detached from its reactor or proactor and optionally closed) and
    /// return true. Otherwise, return false to indicate the stream socket
    /// has already been shut down for sending.
    bool tryShutdownSend(ntcs::ShutdownContext* context, bool keepHalfOpen);

    /// Shut down receiving by the stream socket being modeled, instigated
    /// by an event at the specified 'origin', and proceed through the
    /// shutdown sequence according to the specified 'keepHalfOpen' mode. If
    /// 'origin' equals 'e_REMOTE', this operation represents (and should be
    /// called when) the socket has detected the peer has shut down sending
    /// from its side of the connection (thus no more data will be received
    /// locally.) If 'origin' equals 'e_SOURCE', this operation represents
    /// (and should be called when) the user of the socket has instigated
    /// shutting down receiving independently. On success, set in the
    /// specified 'context' the flag indicating whether this operation
    /// should cause the announcement of the initiation of the shutdown
    /// sequence, set in the specified 'context' the flag indicating whether
    /// the socket should be subsequently shut down for sending, set in the
    /// specified 'context' the flag indicating whether the socket should be
    /// subsequently shut down for receiving, set in the specified 'context'
    /// the flag indicating whether this operation should cause the
    /// announcement of the completion of the shutdown sequence, i.e., this
    /// operation has resulted in both sending and receiving being shut down
    /// for the stream socket being modeled (thus the socket should be
    /// detached from its reactor or proactor and optionally closed) and
    /// return true. Otherwise, return false to indicate the stream socket
    /// has already been shut down for receiving.
    bool tryShutdownReceive(ntcs::ShutdownContext*      context,
                            bool                        keepHalfOpen,
                            ntsa::ShutdownOrigin::Value origin);

    /// Forcibly close both sending and receiving.
    void close();

    /// Reset the value of this object to its value upon construction.
    void reset();

    /// Return true if shutdown has been initiated, otherwise return false.
    bool initiated() const;

    /// Return true if the stream socket being modeled has not been shutdown
    /// for sending, otherwise return false.
    bool canSend() const;

    /// Return true if the stream socket being modeled has not been shutdown
    /// for receiving, otherwise return false.
    bool canReceive() const;

    /// Return true if shutdown has been completed, otherwise return false.
    bool completed() const;
};

NTCCFG_INLINE
bool ShutdownState::initiated() const
{
    return d_context.shutdownInitiated();
}

NTCCFG_INLINE
bool ShutdownState::canSend() const
{
    return !d_context.shutdownSend();
}

NTCCFG_INLINE
bool ShutdownState::canReceive() const
{
    return !d_context.shutdownReceive();
}

NTCCFG_INLINE
bool ShutdownState::completed() const
{
    return d_context.shutdownCompleted();
}

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
