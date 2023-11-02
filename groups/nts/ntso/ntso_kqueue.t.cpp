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

#include <ntso_kqueue.h>

#include <ntso_test.h>

using namespace BloombergLP;

#if NTSO_KQUEUE_ENABLED

namespace test {

}  // close namespace test

NTSCFG_TEST_CASE(1)
{
    // Concern: Test the usage example.

    ntscfg::TestAllocator ta;
    {
        bsl::shared_ptr<ntsi::Reactor> reactor =
            ntso::KqueueUtil::createReactor(&ta);

        ntso::Test::ReactorVector reactorVector(&ta);
        reactorVector.push_back(reactor);

        ntso::Test::usage(reactorVector, &ta);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(2)
{
    // Concern: Polling after a socket has been shutdown for both reading and
    // writing after both sides have shutdown writing does not block.

    ntscfg::TestAllocator ta;
    {
        bsl::shared_ptr<ntsi::Reactor> reactor =
            ntso::KqueueUtil::createReactor(&ta);

        ntso::Test::ReactorVector reactorVector(&ta);
        reactorVector.push_back(reactor);

        ntso::Test::pollingAfterFullShutdown(reactorVector, &ta);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(3)
{
    // Concern: Close socket while it still remains added to the reactor.
    // Polling the reactor times out.

    ntscfg::TestAllocator ta;
    {
        bsl::shared_ptr<ntsi::Reactor> reactor =
            ntso::KqueueUtil::createReactor(&ta);

        ntso::Test::ReactorVector reactorVector(&ta);
        reactorVector.push_back(reactor);

        ntso::Test::pollingAfterClose(reactorVector, &ta);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
}
NTSCFG_TEST_DRIVER_END;

#else

NTSCFG_TEST_CASE(1)
{
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
}
NTSCFG_TEST_DRIVER_END;

#endif
