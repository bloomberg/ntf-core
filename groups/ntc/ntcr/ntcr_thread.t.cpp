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

#include <ntcr_thread.h>

#include <ntccfg_test.h>
#include <ntcd_reactor.h>
#include <ntcd_simulation.h>
#include <ntci_log.h>

#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------

// [ 1]
//-----------------------------------------------------------------------------
// [ 1]
//-----------------------------------------------------------------------------

NTCCFG_TEST_CASE(1)
{
    // Concern: Threads may be restarted.
    // Plan:

#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    const bsl::size_t NUM_RESTARTS = 100;

#else

    const bsl::size_t NUM_RESTARTS = 10;

#endif

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;

        // Create the simulation.

        bsl::shared_ptr<ntcd::Simulation> simulation;
        simulation.createInplace(&ta, &ta);

        error = simulation->run();
        NTCCFG_TEST_OK(error);

        // Create the reactor factory.

        bsl::shared_ptr<ntci::ReactorFactory> reactorFactory;
        {
            bsl::shared_ptr<ntcd::ReactorFactory> concreteReactorFactory;
            concreteReactorFactory.createInplace(&ta, &ta);

            reactorFactory = concreteReactorFactory;
        }

        // Create the thread.

        ntca::ThreadConfig threadConfig;
        threadConfig.setMetricName("test");

        bsl::shared_ptr<ntcr::Thread> thread;
        thread.createInplace(&ta, threadConfig, reactorFactory, &ta);

        // Start, stop, and restart the thread.

        for (bsl::size_t restartIteration = 0; restartIteration < NUM_RESTARTS;
             ++restartIteration)
        {
            BSLS_LOG_INFO("Testing restart iteration %d",
                          (int)(restartIteration));

            error = thread->start();
            NTCCFG_TEST_FALSE(error);

            NTCCFG_TEST_FALSE(
                bslmt::ThreadUtil::areEqual(thread->threadHandle(),
                                            bslmt::ThreadUtil::Handle()));

            NTCCFG_TEST_FALSE(bslmt::ThreadUtil::areEqual(
                thread->threadHandle(),
                bslmt::ThreadUtil::invalidHandle()));

            thread->shutdown();
            thread->linger();
        }

        // Stop the simulation.

        simulation->stop();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern: Threads with injected reactors may be restarted.
    // Plan:

#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    const bsl::size_t NUM_RESTARTS = 100;

#else

    const bsl::size_t NUM_RESTARTS = 10;

#endif

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;

        // Create the simulation.

        bsl::shared_ptr<ntcd::Simulation> simulation;
        simulation.createInplace(&ta, &ta);

        error = simulation->run();
        NTCCFG_TEST_OK(error);

        // Create the reactor factory.

        bsl::shared_ptr<ntcd::ReactorFactory> reactorFactory;
        reactorFactory.createInplace(&ta, &ta);

        // Create the reactor.

        ntca::ReactorConfig reactorConfig;
        reactorConfig.setMetricName("test");
        reactorConfig.setMinThreads(1);
        reactorConfig.setMaxThreads(1);

        bsl::shared_ptr<ntci::Reactor> reactor =
            reactorFactory->createReactor(reactorConfig,
                                          bsl::shared_ptr<ntci::User>(),
                                          &ta);

        // Create the thread.

        ntca::ThreadConfig threadConfig;
        threadConfig.setMetricName("test");

        bsl::shared_ptr<ntcr::Thread> thread;
        thread.createInplace(&ta, threadConfig, reactor, &ta);

        // Start, stop, and restart the thread.

        for (bsl::size_t restartIteration = 0; restartIteration < NUM_RESTARTS;
             ++restartIteration)
        {
            BSLS_LOG_INFO("Testing restart iteration %d",
                          (int)(restartIteration));

            error = thread->start();
            NTCCFG_TEST_FALSE(error);

            NTCCFG_TEST_FALSE(
                bslmt::ThreadUtil::areEqual(thread->threadHandle(),
                                            bslmt::ThreadUtil::Handle()));

            NTCCFG_TEST_FALSE(bslmt::ThreadUtil::areEqual(
                thread->threadHandle(),
                bslmt::ThreadUtil::invalidHandle()));

            thread->shutdown();
            thread->linger();
        }

        // Stop the simulation.

        simulation->stop();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
}
NTCCFG_TEST_DRIVER_END;
