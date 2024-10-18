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
BSLS_IDENT_RCSID(ntcp_thread_t_cpp, "$Id$ $CSID$")

#include <ntcp_thread.h>

#include <ntcd_proactor.h>
#include <ntcd_simulation.h>
#include <ntci_log.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcp {

// Provide tests for 'ntcp::Thread'.
class ThreadTest
{
  public:
    // Concern: Threads may be restarted.
    static void verifyCase1();

    // Concern: Threads with injected proactors may be restarted.
    static void verifyCase2();
};

NTSCFG_TEST_FUNCTION(ntcp::ThreadTest::verifyCase1)
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

    // Create the proactor factory.

    bsl::shared_ptr<ntci::ProactorFactory> proactorFactory;
    {
        bsl::shared_ptr<ntcd::ProactorFactory> concreteProactorFactory;
        concreteProactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR,
                                              NTSCFG_TEST_ALLOCATOR);

        proactorFactory = concreteProactorFactory;
    }

    // Create the thread.

    ntca::ThreadConfig threadConfig;
    threadConfig.setMetricName("test");

    bsl::shared_ptr<ntcp::Thread> thread;
    thread.createInplace(NTSCFG_TEST_ALLOCATOR,
                         threadConfig,
                         proactorFactory,
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

NTSCFG_TEST_FUNCTION(ntcp::ThreadTest::verifyCase2)
{
    // Concern: Threads with injected proactors may be restarted.

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

    // Create the proactor factory.

    bsl::shared_ptr<ntcd::ProactorFactory> proactorFactory;
    proactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR,
                                  NTSCFG_TEST_ALLOCATOR);

    // Create the proactor.

    ntca::ProactorConfig proactorConfig;
    proactorConfig.setMetricName("test");
    proactorConfig.setMinThreads(1);
    proactorConfig.setMaxThreads(1);

    bsl::shared_ptr<ntci::Proactor> proactor =
        proactorFactory->createProactor(proactorConfig,
                                        bsl::shared_ptr<ntci::User>(),
                                        NTSCFG_TEST_ALLOCATOR);

    // Create the thread.

    ntca::ThreadConfig threadConfig;
    threadConfig.setMetricName("test");

    bsl::shared_ptr<ntcp::Thread> thread;
    thread.createInplace(NTSCFG_TEST_ALLOCATOR,
                         threadConfig,
                         proactor,
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

}  // close namespace ntcp
}  // close namespace BloombergLP
