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

#include <ntscfg_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_detachstate_t_cpp, "$Id$ $CSID$")

#include <ntcs_detachstate.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::DetachState'.
class DetachStateTest
{
  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();
};

NTSCFG_TEST_FUNCTION(ntcs::DetachStateTest::verifyCase1)
{
    ntcs::DetachState state;
    NTSCFG_TEST_EQ(state.get(), ntcs::DetachState::e_DETACH_IDLE);

    state.set(ntcs::DetachState::e_DETACH_INITIATED);
    NTSCFG_TEST_EQ(state.get(), ntcs::DetachState::e_DETACH_INITIATED);
}

NTSCFG_TEST_FUNCTION(ntcs::DetachStateTest::verifyCase2)
{
    ntcs::DetachState state(ntcs::DetachState::e_DETACH_INITIATED);
    NTSCFG_TEST_EQ(state.get(), ntcs::DetachState::e_DETACH_INITIATED);
}

}  // close namespace ntcs
}  // close namespace BloombergLP
