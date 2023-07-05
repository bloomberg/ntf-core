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

class DetachState
{
  public:
    enum Value { e_NOT_DETACHED, e_DETACH_INITIATED, e_DETACHED };

    DetachState();
    explicit DetachState(Value state);

    Value get() const {
        return d_state;
    }

    void set(Value state) {
        d_state = state;
    }

  private:
    Value d_state;
};

NTCCFG_INLINE
DetachState::DetachState()
: d_state(e_NOT_DETACHED)
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
