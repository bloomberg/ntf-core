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

#include <bslmt_latch.h>
#include <bslmt_threadutil.h>

using namespace BloombergLP;

namespace Test {
void increment(ntcs::ProactorDetachContext* dc, bslmt::Latch* latch, int* steps)
{
    for (int i = 0; i < 100; ++i) {
        NTCCFG_TEST_TRUE(dc->incrementAndCheckNoDetach());
        ++steps;
    }
    latch->arriveAndWait();
    dc->trySetDetachRequired();
    for (int i = 0; i < 10000; ++i) {
        bool noDetach = dc->incrementAndCheckNoDetach();
        ++steps;
        if (!noDetach) {
            bool detachPossible =
                dc->decrementProcessCounterAndCheckDetachPossible();
            if (detachPossible) {
                break;
            }
        }
    }
}

void decrement(ntcs::ProactorDetachContext* dc, bslmt::Latch* latch, int* steps)
{
    latch->arriveAndWait();
    for (int i = 0; i < 10100; ++i) {
        bool detachPossible =
            dc->decrementProcessCounterAndCheckDetachPossible();
        ++steps;
        if (detachPossible) {
            break;
        }
    }
}

}

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

NTCCFG_TEST_CASE(4)
{
    ntcs::ProactorDetachContext dc;

    bslmt::Latch              latch(2);
    int incSteps = 0;
    int decSteps = 0;

    bslmt::ThreadUtil::Handle inc = bslmt::ThreadUtil::invalidHandle();
    bslmt::ThreadUtil::create(&inc, NTCCFG_BIND(Test::increment, &dc, &latch, &incSteps));
    NTCCFG_TEST_ASSERT(inc != bslmt::ThreadUtil::invalidHandle());

    bslmt::ThreadUtil::Handle dec = bslmt::ThreadUtil::invalidHandle();
    bslmt::ThreadUtil::create(&dec, NTCCFG_BIND(Test::decrement, &dc, &latch, &decSteps));
    NTCCFG_TEST_ASSERT(dec != bslmt::ThreadUtil::invalidHandle());

    bslmt::ThreadUtil::join(inc);
    bslmt::ThreadUtil::join(dec);

    NTCCFG_TEST_TRUE(incSteps == decSteps);
}

NTCCFG_TEST_CASE(5)
{

}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);


    NTCCFG_TEST_REGISTER(5);
}
NTCCFG_TEST_DRIVER_END;
