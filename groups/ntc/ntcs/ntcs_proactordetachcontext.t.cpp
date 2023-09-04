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

#include <ntccfg_test.h>

using namespace BloombergLP;

NTCCFG_TEST_CASE(1)
{
    ntcs::ProactorDetachContext dc;
    NTCCFG_TEST_FALSE(dc.trySetDetachScheduled());
    NTCCFG_TEST_TRUE(dc.trySetDetachRequired());
    NTCCFG_TEST_FALSE(dc.trySetDetachRequired());
    NTCCFG_TEST_TRUE(dc.trySetDetachScheduled());
}

NTCCFG_TEST_CASE(2)
{
    ntcs::ProactorDetachContext dc;
    NTCCFG_TEST_TRUE(dc.incrementAndCheckNoDetach());
    NTCCFG_TEST_TRUE(dc.trySetDetachRequired());
    NTCCFG_TEST_FALSE(dc.incrementAndCheckNoDetach());
}

NTCCFG_TEST_CASE(3)
{
    ntcs::ProactorDetachContext dc;
    NTCCFG_TEST_TRUE(dc.incrementAndCheckNoDetach());
    NTCCFG_TEST_TRUE(dc.incrementAndCheckNoDetach());
    NTCCFG_TEST_TRUE(dc.incrementAndCheckNoDetach());
    NTCCFG_TEST_FALSE(dc.decrementProcessCounterAndCheckDetachPossible());
    NTCCFG_TEST_TRUE(dc.trySetDetachRequired());
    NTCCFG_TEST_FALSE(dc.decrementProcessCounterAndCheckDetachPossible());
    NTCCFG_TEST_TRUE(dc.decrementProcessCounterAndCheckDetachPossible());
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
}
NTCCFG_TEST_DRIVER_END;
