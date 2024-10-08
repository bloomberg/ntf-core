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
#include <ntso_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntso_test_t_cpp, "$Id$ $CSID$")

#include <ntso_eventport.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntso {

#if NTSO_EVENTPORT_ENABLED

// Provide tests for 'ntso::EventPort'.
class EventPortTest
{
  public:
    // Concern: Test the usage example.
    static void verifyCase1();

    // Concern: Polling after a socket has been shutdown for both reading and
    // writing after both sides have shutdown writing does not block.
    static void verifyCase2();

    // Concern: Close socket while it still remains added to the reactor.
    // Polling the reactor times out.
    static void verifyCase3();
};

NTSCFG_TEST_FUNCTION(ntso::EventPortTest::verifyCase1)
{
    // Concern: Test the usage example.

    ntsa::ReactorConfig reactorConfig(NTSCFG_TEST_ALLOCATOR);
    reactorConfig.setDriverName("eventport");

    bsl::shared_ptr<ntsi::Reactor> reactor =
        ntso::EventPortUtil::createReactor(reactorConfig, NTSCFG_TEST_ALLOCATOR);

    ntso::Test::ReactorVector reactorVector(NTSCFG_TEST_ALLOCATOR);
    reactorVector.push_back(reactor);

    ntso::Test::usage(reactorVector, NTSCFG_TEST_ALLOCATOR);
}

NTSCFG_TEST_FUNCTION(ntso::EventPortTest::verifyCase2)
{
    // Concern: Polling after a socket has been shutdown for both reading and
    // writing after both sides have shutdown writing does not block.

    ntsa::ReactorConfig reactorConfig(NTSCFG_TEST_ALLOCATOR);
    reactorConfig.setDriverName("eventport");

    bsl::shared_ptr<ntsi::Reactor> reactor =
        ntso::EventPortUtil::createReactor(reactorConfig, NTSCFG_TEST_ALLOCATOR);

    ntso::Test::ReactorVector reactorVector(NTSCFG_TEST_ALLOCATOR);
    reactorVector.push_back(reactor);

    ntso::Test::pollingAfterFullShutdown(reactorVector, NTSCFG_TEST_ALLOCATOR);
}

NTSCFG_TEST_FUNCTION(ntso::EventPortTest::verifyCase3)
{
    // Concern: Close socket while it still remains added to the reactor.
    // Polling the reactor times out.

    ntsa::ReactorConfig reactorConfig(NTSCFG_TEST_ALLOCATOR);
    reactorConfig.setDriverName("eventport");

    bsl::shared_ptr<ntsi::Reactor> reactor =
        ntso::EventPortUtil::createReactor(reactorConfig, NTSCFG_TEST_ALLOCATOR);

    ntso::Test::ReactorVector reactorVector(NTSCFG_TEST_ALLOCATOR);
    reactorVector.push_back(reactor);

    ntso::Test::pollingAfterClose(reactorVector, NTSCFG_TEST_ALLOCATOR);
}

#endif

}  // close namespace ntso
}  // close namespace BloombergLP
