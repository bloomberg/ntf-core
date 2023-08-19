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

#include <ntcs_proactordetachcontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_roactordetachcontext_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntcs {

ProactorDetachContext::ProactorDetachContext()
: d_stateAndCounter()
{
}

bool ProactorDetachContext::incrementAndCheckNoDetach()
{
    unsigned int val = ++d_stateAndCounter;
    if ((val & k_STATE_MASK) != DetachState::e_DETACH_NOT_REQUIRED) {
        return false;
    }
    return true;
}

bool ProactorDetachContext::decrementProcessCounterAndCheckDetachPossible()
{
    unsigned int val = --d_stateAndCounter;
    if ((val & k_COUNTER_MASK) == 0 &&
        (val & k_STATE_MASK) == DetachState::e_DETACH_REQUIRED)
    {
        unsigned int prev =
            d_stateAndCounter.testAndSwap(val,
                                          DetachState::e_DETACH_SCHEDULED);
        return prev == val;
    }
    return false;
}

bool ProactorDetachContext::trySetDetachScheduled()
{
    unsigned int val = d_stateAndCounter.load();
    if ((val & k_COUNTER_MASK) == 0 &&
        (val & k_STATE_MASK) == DetachState::e_DETACH_REQUIRED)
    {
        unsigned int prev =
            d_stateAndCounter.testAndSwap(val,
                                          DetachState::e_DETACH_SCHEDULED);
        return prev == val;
    }
    return false;
}

bool ProactorDetachContext::trySetDetachRequired()
{
    while (true) {
        unsigned int current = d_stateAndCounter.load();
        if ((current & k_STATE_MASK) != e_DETACH_NOT_REQUIRED) {
            return false;
        }
        unsigned int prev =
            d_stateAndCounter.testAndSwap(current,
                                          current | e_DETACH_REQUIRED);
        if (prev == current) {
            break;
        }
    }
    return true;
}

}  // close package namespace
}  // close enterprise namespace
