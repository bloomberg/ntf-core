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

#ifndef INCLUDED_NTCS_CALLBACKSTATE
#define INCLUDED_NTCS_CALLBACKSTATE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bsls_atomic.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide a mechanism that guards the state transitions of an asynchronous
/// callback.
///
/// @details
/// Provide a mechanism to manage the state transistions of an
/// asynchronous callback by preventing multiple execution and allowing for
/// cancellation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class CallbackState
{
    enum { e_PENDING, e_SCHEDULED, e_FINISHED };

    bsls::AtomicInt d_value;

  private:
    CallbackState(const CallbackState&) BSLS_KEYWORD_DELETED;
    CallbackState& operator=(const CallbackState&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new callback state initially in the pending state.
    CallbackState();

    /// Destroy this object.
    ~CallbackState();

    /// Reset the callback state to pending, its value upon default
    /// construction.
    void reset();

    /// Attempt to transition the callback state from pending to scheduled.
    /// If the callback state is pending, transition the state to scheduled
    /// and return true. Otherwise, return false.
    bool schedule();

    /// Attempt to transition the callback state from pending or scheduled
    /// to finished. If the callback state is pending or scheduled,
    /// transition the state to finished and return true. Otherwise, return
    /// false.
    bool finish();
};

NTCCFG_INLINE
CallbackState::CallbackState()
: d_value(e_PENDING)
{
}

NTCCFG_INLINE
CallbackState::~CallbackState()
{
}

NTCCFG_INLINE
void CallbackState::reset()
{
    d_value = e_PENDING;
}

NTCCFG_INLINE
bool CallbackState::schedule()
{
    while (true) {
        int prevValue = d_value.load();
        if (prevValue == e_PENDING) {
            if (prevValue == d_value.testAndSwap(prevValue, e_SCHEDULED)) {
                break;
            }
        }
        else {
            return false;
        }
    }

    return true;
}

NTCCFG_INLINE
bool CallbackState::finish()
{
    while (true) {
        int prevValue = d_value.load();
        if (prevValue != e_FINISHED) {
            if (prevValue == d_value.testAndSwap(prevValue, e_FINISHED)) {
                break;
            }
        }
        else {
            return false;
        }
    }

    return true;
}

}  // close package namespace
}  // close enterprise namespace
#endif
