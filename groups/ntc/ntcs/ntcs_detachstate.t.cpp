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

#include <ntcs_detachstate.h>

#include <ntccfg_test.h>

using namespace BloombergLP;

NTCCFG_TEST_CASE(1)
{
    ntcs::DetachState state;
    NTCCFG_TEST_EQ(state.get(), ntcs::DetachState::e_DETACH_IDLE);

    state.set(ntcs::DetachState::e_DETACH_INITIATED);
    NTCCFG_TEST_EQ(state.get(), ntcs::DetachState::e_DETACH_INITIATED);
}

NTCCFG_TEST_CASE(2)
{
    ntcs::DetachState state(ntcs::DetachState::e_DETACH_INITIATED);
    NTCCFG_TEST_EQ(state.get(), ntcs::DetachState::e_DETACH_INITIATED);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
}
NTCCFG_TEST_DRIVER_END;
