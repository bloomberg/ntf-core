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
BSLS_IDENT_RCSID(ntco_kqueue_t_cpp, "$Id$ $CSID$")

#include <ntco_test.h>
#include <ntco_kqueue.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntco {

#if NTC_BUILD_WITH_KQUEUE

// Provide tests for 'ntco::Kqueue'.
class KqueueTest
{
  public:
    // Verify the reactor implements sockets.
    static void verifySockets();

    // Verify the reactor implements timers.
    static void verifyTimers();

    // Verify the reactor implements deferred functions.
    static void verifyFunctions();
};

NTSCFG_TEST_FUNCTION(ntco::KqueueTest::verifySockets)
{
    bsl::shared_ptr<ntco::KqueueFactory> reactorFactory;
    reactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    Test::verifyReactorSockets(reactorFactory);
}

NTSCFG_TEST_FUNCTION(ntco::KqueueTest::verifyTimers)
{
    bsl::shared_ptr<ntco::KqueueFactory> reactorFactory;
    reactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    Test::verifyReactorTimers(reactorFactory);
}

NTSCFG_TEST_FUNCTION(ntco::KqueueTest::verifyFunctions)
{
    bsl::shared_ptr<ntco::KqueueFactory> reactorFactory;
    reactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    Test::verifyReactorFunctions(reactorFactory);
}

#endif

}  // close namespace ntco
}  // close namespace BloombergLP

