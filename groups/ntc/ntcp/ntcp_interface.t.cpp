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

#include <ntcp_interface.h>

#include <ntcd_simulation.h>
#include <ntcs_datapool.h>

#include <ntccfg_test.h>

#include <bslmt_threadutil.h>
#include <bsls_timeinterval.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

using namespace BloombergLP;

// Uncomment to test a particular style of socket-to-thread load balancing,
// instead of both static and dynamic load balancing.
// #define NTCP_INTERFACE_TEST_DYNAMIC_LOAD_BALANCING false

// Uncomment to test a particular style of thread pool resizing, instead of
// both static and dynamic sizing.
// #define NTCP_INTERFACE_TEST_DYNAMIC_THREAD_COUNT true

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

namespace test {

namespace case1 {

void execute(bslma::Allocator* allocator)
{
    ntsa::Error error;

    // Create the simulation.

    bsl::shared_ptr<ntcd::Simulation> simulation;
    simulation.createInplace(allocator, allocator);

    error = simulation->run();
    NTCCFG_TEST_OK(error);

    // Create the data pool.

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(allocator, allocator);

    // Create the proactor factory.

    bsl::shared_ptr<ntcd::ProactorFactory> proactorFactory;
    proactorFactory.createInplace(allocator, allocator);

    // Create the interface.

    ntca::InterfaceConfig interfaceConfig;
    interfaceConfig.setMetricName("test");
    interfaceConfig.setMinThreads(1);
    interfaceConfig.setMaxThreads(1);

    bsl::shared_ptr<ntcp::Interface> interface;
    interface.createInplace(allocator,
                            interfaceConfig,
                            dataPool,
                            proactorFactory,
                            allocator);

    error = interface->start();
    NTCCFG_TEST_OK(error);

    // Ensure the interface has created one proactor run by one thread.

    NTCCFG_TEST_EQ(interface->numProactors(), 1);
    NTCCFG_TEST_EQ(interface->numThreads(), 1);

    // Stop the interface.

    interface->shutdown();
    interface->linger();

    // Ensure the interface is no longer running any threads but the
    // original proactor still exists.

    NTCCFG_TEST_EQ(interface->numProactors(), 1);
    NTCCFG_TEST_EQ(interface->numThreads(), 0);

    // Stop the simulation.

    simulation->stop();
}

}  // close namespace case1

namespace case2 {

void run(ntcp::Interface* interface,
         bsl::size_t      numThreads,
         bsl::size_t      restartIteration,
         bool             dynamicLoadBalancing,
         bool             dynamicThreadCount)
{
    BSLS_LOG_INFO("Testing restart iteration %d", (int)(restartIteration));

    if (restartIteration == 0) {
        NTCCFG_TEST_EQ(interface->numProactors(), 0);
    }
    else {
        if (dynamicLoadBalancing) {
            NTCCFG_TEST_EQ(interface->numProactors(), 1);
        }
        else {
            NTCCFG_TEST_EQ(interface->numProactors(), numThreads);
        }
    }

    NTCCFG_TEST_EQ(interface->numThreads(), 0);

    if (dynamicThreadCount) {
        NTCCFG_TEST_EQ(interface->minThreads(), 1);
    }
    else {
        NTCCFG_TEST_EQ(interface->minThreads(), numThreads);
    }

    NTCCFG_TEST_EQ(interface->maxThreads(), numThreads);

    interface->start();

    if (dynamicThreadCount) {
        if (restartIteration == 0) {
            NTCCFG_TEST_EQ(interface->numProactors(), 1);
        }
        else {
            if (dynamicLoadBalancing) {
                NTCCFG_TEST_EQ(interface->numProactors(), 1);
            }
            else {
                NTCCFG_TEST_EQ(interface->numProactors(), numThreads);
            }
        }

        if (restartIteration == 0) {
            NTCCFG_TEST_EQ(interface->numThreads(), 1);
        }
        else {
            NTCCFG_TEST_EQ(interface->numThreads(), numThreads);
        }

        NTCCFG_TEST_EQ(interface->minThreads(), 1);
        NTCCFG_TEST_EQ(interface->maxThreads(), numThreads);

        if (restartIteration == 0) {
            for (bsl::size_t newNumThreads = 2;
                 newNumThreads <= interface->maxThreads();
                 ++newNumThreads)
            {
                bool grew = interface->expand();
                NTCCFG_TEST_TRUE(grew);

                if (dynamicLoadBalancing) {
                    NTCCFG_TEST_EQ(interface->numProactors(), 1);
                }
                else {
                    NTCCFG_TEST_EQ(interface->numProactors(), newNumThreads);
                }

                NTCCFG_TEST_EQ(interface->numThreads(), newNumThreads);
                NTCCFG_TEST_EQ(interface->minThreads(), 1);
                NTCCFG_TEST_EQ(interface->maxThreads(), numThreads);
            }

            {
                bool grew = interface->expand();
                NTCCFG_TEST_FALSE(grew);
            }
        }
        else {
            bool grew = interface->expand();
            NTCCFG_TEST_FALSE(grew);
        }

        if (dynamicLoadBalancing) {
            NTCCFG_TEST_EQ(interface->numProactors(), 1);
        }
        else {
            NTCCFG_TEST_EQ(interface->numProactors(), numThreads);
        }

        NTCCFG_TEST_EQ(interface->numThreads(), numThreads);
        NTCCFG_TEST_EQ(interface->minThreads(), 1);
        NTCCFG_TEST_EQ(interface->maxThreads(), numThreads);
    }
    else {
        if (dynamicLoadBalancing) {
            NTCCFG_TEST_EQ(interface->numProactors(), 1);
        }
        else {
            NTCCFG_TEST_EQ(interface->numProactors(), numThreads);
        }

        NTCCFG_TEST_EQ(interface->numThreads(), numThreads);
        NTCCFG_TEST_EQ(interface->minThreads(), numThreads);
        NTCCFG_TEST_EQ(interface->maxThreads(), numThreads);
    }

    interface->shutdown();
    interface->linger();

    if (dynamicLoadBalancing) {
        NTCCFG_TEST_EQ(interface->numProactors(), 1);
    }
    else {
        NTCCFG_TEST_EQ(interface->numProactors(), numThreads);
    }

    NTCCFG_TEST_EQ(interface->numThreads(), 0);

    if (dynamicThreadCount) {
        NTCCFG_TEST_EQ(interface->minThreads(), 1);
    }
    else {
        NTCCFG_TEST_EQ(interface->minThreads(), numThreads);
    }

    NTCCFG_TEST_EQ(interface->maxThreads(), numThreads);
}

void execute(bslma::Allocator* allocator)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    const bsl::size_t NUM_RESTARTS = 10;
    const bsl::size_t MIN_THREADS  = 1;
    const bsl::size_t MAX_THREADS  = 10;

#else

    const bsl::size_t NUM_RESTARTS = 2;
    const bsl::size_t MIN_THREADS  = 1;
    const bsl::size_t MAX_THREADS  = 2;

#endif

    const bool DYNAMIC_LOAD_BALANCING[] = {false, true};
    const bool DYNAMIC_THREAD_COUNT[]   = {false, true};

    ntsa::Error error;

    // Create the simulation.

    bsl::shared_ptr<ntcd::Simulation> simulation;
    simulation.createInplace(allocator, allocator);

    error = simulation->run();
    NTCCFG_TEST_OK(error);

    // Create the data pool.

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(allocator, allocator);

    // Create the proactor factory.

    bsl::shared_ptr<ntcd::ProactorFactory> proactorFactory;
    proactorFactory.createInplace(allocator, allocator);

    for (bsl::size_t dynamicLoadBalancingIndex = 0;
         dynamicLoadBalancingIndex < 2;
         ++dynamicLoadBalancingIndex)
    {
        bool dynamicLoadBalancing =
            DYNAMIC_LOAD_BALANCING[dynamicLoadBalancingIndex];

#if defined(NTCP_INTERFACE_TEST_DYNAMIC_LOAD_BALANCING)
        if (dynamicLoadBalancing != NTCP_INTERFACE_TEST_DYNAMIC_LOAD_BALANCING)
        {
            continue;
        }
#endif

#if NTC_BUILD_WITH_DYNAMIC_LOAD_BALANCING == 0
        if (dynamicLoadBalancing) {
            continue;
        }
#endif

        for (bsl::size_t dynamicThreadCountIndex = 0;
             dynamicThreadCountIndex < 2;
             ++dynamicThreadCountIndex)
        {
            bool dynamicThreadCount =
                DYNAMIC_THREAD_COUNT[dynamicThreadCountIndex];

#if defined(NTCP_INTERFACE_TEST_DYNAMIC_THREAD_COUNT)
            if (dynamicThreadCount != NTCP_INTERFACE_TEST_DYNAMIC_THREAD_COUNT)
            {
                continue;
            }
#endif

#if NTC_BUILD_WITH_THREAD_SCALING == 0
            if (dynamicThreadCount) {
                continue;
            }
#endif

            BSLS_LOG_WARN("Testing driver (%s) (%s)",
                          (dynamicLoadBalancing ? "dynamic" : "static"),
                          (dynamicThreadCount ? "resizable" : "fixed"));

            for (bsl::size_t numThreads = MIN_THREADS;
                 numThreads <= MAX_THREADS;
                 ++numThreads)
            {
                BSLS_LOG_INFO("Testing %d threads", (int)(numThreads));

                ntccfg::TestAllocator ta;
                {
                    ntca::InterfaceConfig interfaceConfig;
                    interfaceConfig.setMetricName("test");

                    if (dynamicThreadCount) {
                        interfaceConfig.setMinThreads(1);
                    }
                    else {
                        interfaceConfig.setMinThreads(numThreads);
                    }

                    interfaceConfig.setMaxThreads(numThreads);
                    interfaceConfig.setDynamicLoadBalancing(
                        dynamicLoadBalancing);

                    bsl::shared_ptr<ntcp::Interface> interface;
                    interface.createInplace(allocator,
                                            interfaceConfig,
                                            dataPool,
                                            proactorFactory,
                                            allocator);

                    for (bsl::size_t restartIteration = 0;
                         restartIteration < NUM_RESTARTS;
                         ++restartIteration)
                    {
                        test::case2::run(interface.get(),
                                         numThreads,
                                         restartIteration,
                                         dynamicLoadBalancing,
                                         dynamicThreadCount);
                    }
                }
                NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
            }
        }
    }

    // Stop the simulation.

    simulation->stop();
}

}  // close namespace case2

}  // close namespace test

NTCCFG_TEST_CASE(1)
{
    // Concern: Interface can be started and stopped.
    // Plan:

    ntccfg::TestAllocator ta;
    {
        test::case1::execute(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern: Interface can be started, stopped, restarted, and expanded.
    // Plan:

    ntccfg::TestAllocator ta;
    {
        test::case2::execute(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
}
NTCCFG_TEST_DRIVER_END;
