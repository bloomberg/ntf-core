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
BSLS_IDENT_RCSID(ntco_poll_t_cpp, "$Id$ $CSID$")

#include <ntco_test.h>
#include <ntco_poll.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntco {

#if NTC_BUILD_WITH_POLL

// Provide tests for 'ntco::Poll'.
class PollTest
{
  public:
    // Verify the reactor implements sockets.
    static void verifySockets();

    // Verify the reactor implements timers.
    static void verifyTimers();

    // Verify the reactor implements deferred functions.
    static void verifyFunctions();
};

NTSCFG_TEST_FUNCTION(ntco::PollTest::verifySockets)
{
    bsl::shared_ptr<ntco::PollFactory> reactorFactory;
    reactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    Test::verifyReactorSockets(reactorFactory);
}

NTSCFG_TEST_FUNCTION(ntco::PollTest::verifyTimers)
{
    bsl::shared_ptr<ntco::PollFactory> reactorFactory;
    reactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    Test::verifyReactorTimers(reactorFactory);
}

NTSCFG_TEST_FUNCTION(ntco::PollTest::verifyFunctions)
{
    bsl::shared_ptr<ntco::PollFactory> reactorFactory;
    reactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    Test::verifyReactorFunctions(reactorFactory);
}

#endif

}  // close namespace ntco
}  // close namespace BloombergLP

