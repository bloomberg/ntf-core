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

#include <bsls_assert.h>
#include <ntccfg_inline.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide a enumeration that indicates state of a socket detachment process.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs

class DetachState
{
  public:
    enum Value { e_DETACH_IDLE, e_DETACH_INITIATED };

    /// Create a new detachment state initially in idle state.
    DetachState();

    /// Create a new detachment state initially in the specified 'state'.
    explicit DetachState(Value state);

    /// Returns current value of socket detachment state
    Value get() const {
        return d_state;
    }

    /// Sets socket detachment state to the specified 'state'
    void set(Value state) {
        d_state = state;
    }

  private:
    Value d_state;
};

NTCCFG_INLINE
DetachState::DetachState()
: d_state(e_DETACH_IDLE)
{
}

NTCCFG_INLINE
DetachState::DetachState(DetachState::Value state)
: d_state(state)
{
}

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
