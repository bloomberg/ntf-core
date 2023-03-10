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

#ifndef INCLUDED_NTCS_FLOWCONTROLSTATE
#define INCLUDED_NTCS_FLOWCONTROLSTATE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_flowcontroltype.h>
#include <ntccfg_platform.h>
#include <ntcs_flowcontrolcontext.h>
#include <ntcscm_version.h>
#include <bsls_atomic.h>
#include <bsls_spinlock.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide a mechanism to to manage flow control.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class FlowControlState
{
    bool d_enableSend;
    bool d_enableReceive;
    bool d_lockSend;
    bool d_lockReceive;
    bool d_closed;

  private:
    FlowControlState(const FlowControlState&) BSLS_KEYWORD_DELETED;
    FlowControlState& operator=(const FlowControlState&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new object in the default state.
    FlowControlState();

    /// Destroy this object.
    ~FlowControlState();

    /// Apply flow control of the specified 'type'. Lock the flow control
    /// state of the 'type' to be unable to be subsequently relaxed or
    /// rearmed according to the specified 'lock' flag. If the flow control
    /// state for the 'type' is already locked, prevent any associated
    /// changes to the flow control state of that 'type'. If any change in
    /// the state of flow control occurs, load into the specified 'context'
    /// the result of the operation and return true. Otherwise, return
    /// false.
    bool apply(ntcs::FlowControlContext*    context,
               ntca::FlowControlType::Value type,
               bool                         lock);

    /// Relax flow control of the specified 'type'. Unlock the flow control
    /// state of the 'type' to be subsequently applied or rearmed according
    /// to the specified 'unlock' flag. If the flow control state for the
    /// 'type' is already locked, and the 'unlock' flag is false, prevent
    /// any associated changes to the flow control state of that 'type'. If
    /// any change in the state of flow control occurs, load into the
    /// specified 'context' the result of the operation and return true.
    /// Otherwise, return false.
    bool relax(ntcs::FlowControlContext*    context,
               ntca::FlowControlType::Value type,
               bool                         unlock);

    /// Close the flow control state for any subsequent application,
    /// regardless of the current lock state, and unlock each state.
    void close();

    /// Reset the value of this object to its value upon construction.
    void reset();

    /// Rearm flow control of the specified 'type' reacting to events in the
    /// specified 'oneShot' mode. If the flow control state for the 'type'
    /// is locked, prevent the associated event from being indicated it
    /// should be rearmed. If any events should be rearmed, load the result
    /// of the operation into the specified 'context' and return true.
    /// Otherwise, return false.
    bool rearm(ntcs::FlowControlContext*    context,
               ntca::FlowControlType::Value type,
               bool                         oneShot) const;

    /// Return true if the socket being modeled can and wants to send
    /// more data, otherwise return false.
    bool wantSend() const;

    /// Return true if the socket being modeled can and wants to receive
    /// more data, otherwise return false.
    bool wantReceive() const;

    /// Return true if changes to the flow control state in the send
    /// direction have been locked, otherwise return false.
    bool lockSend() const;

    /// Return true if changes to the flow control state in the receive
    /// direction have been locked, otherwise return false.
    bool lockReceive() const;

    /// Return true if flow control has been closed, otherwise return false.
    bool closed() const;
};

NTCCFG_INLINE
bool FlowControlState::wantSend() const
{
    return d_enableSend;
}

NTCCFG_INLINE
bool FlowControlState::wantReceive() const
{
    return d_enableReceive;
}

NTCCFG_INLINE
bool FlowControlState::lockSend() const
{
    return d_lockSend;
}

NTCCFG_INLINE
bool FlowControlState::lockReceive() const
{
    return d_lockReceive;
}

NTCCFG_INLINE
bool FlowControlState::closed() const
{
    return d_closed;
}

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
