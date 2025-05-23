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
BSLS_IDENT_RCSID(ntcs_callbackstate_t_cpp, "$Id$ $CSID$")

#include <ntcs_callbackstate.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::CallbackState'.
class CallbackStateTest
{
  public:
    // TODO
    static void verify();
};

NTSCFG_TEST_FUNCTION(ntcs::CallbackStateTest::verify)
{
    ntcs::CallbackState callbackState;

    bool result;

    result = callbackState.schedule();
    NTSCFG_TEST_TRUE(result);

    result = callbackState.schedule();
    NTSCFG_TEST_FALSE(result);

    result = callbackState.finish();
    NTSCFG_TEST_TRUE(result);

    result = callbackState.finish();
    NTSCFG_TEST_FALSE(result);

    result = callbackState.schedule();
    NTSCFG_TEST_FALSE(result);

    result = callbackState.schedule();
    NTSCFG_TEST_FALSE(result);
}

}  // close namespace ntcs
}  // close namespace BloombergLP
