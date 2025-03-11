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

#ifndef INCLUDED_NTCS_DETACHSTATE
#define INCLUDED_NTCS_DETACHSTATE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_inline.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide a enumeration that indicates the detachment status of a socket from
/// its reactor or proactor.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class DetachMode
{
public:
    /// Provide a enumeration that indicates the detachment status of a socket
    /// from its reactor or proactor.
    enum Value {
        /// The socket is attached.
        e_IDLE, 

        /// The socket detachment has been initiated.
        e_INITIATED
    };
};

/// @internal @brief
/// Provide a enumeration that indicates why a socket is being detached from
/// its reactor or proactor.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class DetachGoal
{
public:
    /// Provide a enumeration that indicates why a socket is being detached
    /// from its reactor or proactor.
    enum Value {
        /// The socket is being detached to be shutdown and closed.
        e_CLOSE = 0,

        /// The socket is being detached to be exported.
        e_EXPORT = 1
    };
};

/// @internal @brief
/// Provide a enumeration that indicates state of a socket detachment process.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class DetachState
{
    ntcs::DetachMode::Value d_mode;
    ntcs::DetachGoal::Value d_goal;

  private:
    DetachState(const DetachState&) BSLS_KEYWORD_DELETED;
    DetachState& operator=(const DetachState&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new detachment state initially in idle state with the goal
    /// to close the socket.
    DetachState();

    /// Destroy this object.
    ~DetachState();

    /// Set the mode to the specified 'mode'.
    void setMode(ntcs::DetachMode::Value mode);

    /// Set the goal to the specified 'goal'.
    void setGoal(ntcs::DetachGoal::Value goal);

    /// Return the mode.
    ntcs::DetachMode::Value mode() const;

    /// Return the goal.
    ntcs::DetachGoal::Value goal() const;
};

NTCCFG_INLINE
DetachState::DetachState()
: d_mode(ntcs::DetachMode::e_IDLE)
, d_goal(ntcs::DetachGoal::e_CLOSE)
{
}

NTCCFG_INLINE
DetachState::~DetachState()
{
}

NTCCFG_INLINE
void DetachState::setMode(ntcs::DetachMode::Value mode)
{
    d_mode = mode;
}

NTCCFG_INLINE
void DetachState::setGoal(ntcs::DetachGoal::Value goal)
{
    d_goal = goal;
}

NTCCFG_INLINE
ntcs::DetachMode::Value DetachState::mode() const
{
    return d_mode;
}

NTCCFG_INLINE
ntcs::DetachGoal::Value DetachState::goal() const
{
    return d_goal;
}

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
