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
BSLS_IDENT_RCSID(ntcr_thread_t_cpp, "$Id$ $CSID$")

#include <ntcr_thread.h>

#include <ntcd_reactor.h>
#include <ntcd_simulation.h>
#include <ntci_log.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcr {

// Provide tests for 'ntcr::Thread'.
class ThreadTest
{
  public:
    // Concern: Threads may be restarted.
    static void verifyCase1();

    // Concern: Threads with injected reactors may be restarted.
    static void verifyCase2();
};

NTSCFG_TEST_FUNCTION(ntcr::ThreadTest::verifyCase1)
{
    // Concern: Threads may be restarted.

#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    const bsl::size_t NUM_RESTARTS = 100;

#else

    const bsl::size_t NUM_RESTARTS = 10;

#endif

    ntsa::Error error;

    // Create the simulation.

    bsl::shared_ptr<ntcd::Simulation> simulation;
    simulation.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    error = simulation->run();
    NTSCFG_TEST_OK(error);

    // Create the reactor factory.

    bsl::shared_ptr<ntci::ReactorFactory> reactorFactory;
    {
        bsl::shared_ptr<ntcd::ReactorFactory> concreteReactorFactory;
        concreteReactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR,
                                             NTSCFG_TEST_ALLOCATOR);

        reactorFactory = concreteReactorFactory;
    }

    // Create the thread.

    ntca::ThreadConfig threadConfig;
    threadConfig.setMetricName("test");

    bsl::shared_ptr<ntcr::Thread> thread;
    thread.createInplace(NTSCFG_TEST_ALLOCATOR,
                         threadConfig,
                         reactorFactory,
                         NTSCFG_TEST_ALLOCATOR);

    // Start, stop, and restart the thread.

    for (bsl::size_t restartIteration = 0; restartIteration < NUM_RESTARTS;
         ++restartIteration)
    {
        BSLS_LOG_INFO("Testing restart iteration %d", (int)(restartIteration));

        error = thread->start();
        NTSCFG_TEST_FALSE(error);

        NTSCFG_TEST_FALSE(
            bslmt::ThreadUtil::areEqual(thread->threadHandle(),
                                        bslmt::ThreadUtil::Handle()));

        NTSCFG_TEST_FALSE(
            bslmt::ThreadUtil::areEqual(thread->threadHandle(),
                                        bslmt::ThreadUtil::invalidHandle()));

        thread->shutdown();
        thread->linger();
    }

    // Stop the simulation.

    simulation->stop();
}

NTSCFG_TEST_FUNCTION(ntcr::ThreadTest::verifyCase2)
{
    // Concern: Threads with injected reactors may be restarted.

#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    const bsl::size_t NUM_RESTARTS = 100;

#else

    const bsl::size_t NUM_RESTARTS = 10;

#endif

    ntsa::Error error;

    // Create the simulation.

    bsl::shared_ptr<ntcd::Simulation> simulation;
    simulation.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    error = simulation->run();
    NTSCFG_TEST_OK(error);

    // Create the reactor factory.

    bsl::shared_ptr<ntcd::ReactorFactory> reactorFactory;
    reactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Create the reactor.

    ntca::ReactorConfig reactorConfig;
    reactorConfig.setMetricName("test");
    reactorConfig.setMinThreads(1);
    reactorConfig.setMaxThreads(1);

    bsl::shared_ptr<ntci::Reactor> reactor =
        reactorFactory->createReactor(reactorConfig,
                                      bsl::shared_ptr<ntci::User>(),
                                      NTSCFG_TEST_ALLOCATOR);

    // Create the thread.

    ntca::ThreadConfig threadConfig;
    threadConfig.setMetricName("test");

    bsl::shared_ptr<ntcr::Thread> thread;
    thread.createInplace(NTSCFG_TEST_ALLOCATOR,
                         threadConfig,
                         reactor,
                         NTSCFG_TEST_ALLOCATOR);

    // Start, stop, and restart the thread.

    for (bsl::size_t restartIteration = 0; restartIteration < NUM_RESTARTS;
         ++restartIteration)
    {
        BSLS_LOG_INFO("Testing restart iteration %d", (int)(restartIteration));

        error = thread->start();
        NTSCFG_TEST_FALSE(error);

        NTSCFG_TEST_FALSE(
            bslmt::ThreadUtil::areEqual(thread->threadHandle(),
                                        bslmt::ThreadUtil::Handle()));

        NTSCFG_TEST_FALSE(
            bslmt::ThreadUtil::areEqual(thread->threadHandle(),
                                        bslmt::ThreadUtil::invalidHandle()));

        thread->shutdown();
        thread->linger();
    }

    // Stop the simulation.

    simulation->stop();
}

}  // close namespace ntcr
}  // close namespace BloombergLP
