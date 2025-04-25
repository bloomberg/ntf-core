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
BSLS_IDENT_RCSID(ntcp_interface_t_cpp, "$Id$ $CSID$")

#include <ntcp_interface.h>

#include <ntcd_simulation.h>
#include <ntcs_datapool.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcp {

// Uncomment to test a particular style of socket-to-thread load balancing,
// instead of both static and dynamic load balancing.
// #define NTCP_INTERFACE_TEST_DYNAMIC_LOAD_BALANCING false

// Uncomment to test a particular style of thread pool resizing, instead of
// both static and dynamic sizing.
// #define NTCP_INTERFACE_TEST_DYNAMIC_THREAD_COUNT true

// Provide tests for 'ntcp::Interface'.
class InterfaceTest
{
    static void run(const bsl::shared_ptr<ntcp::Interface>& interface,
                    bsl::size_t                             numThreads,
                    bsl::size_t                             restartIteration,
                    bool dynamicLoadBalancing,
                    bool dynamicThreadCount);

  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();
};

void InterfaceTest::run(const bsl::shared_ptr<ntcp::Interface>& interface,
                        bsl::size_t                             numThreads,
                        bsl::size_t restartIteration,
                        bool        dynamicLoadBalancing,
                        bool        dynamicThreadCount)
{
    BSLS_LOG_TRACE("Testing restart iteration %d", (int)(restartIteration));

    if (restartIteration == 0) {
        NTSCFG_TEST_EQ(interface->numProactors(), 0);
    }
    else {
        if (dynamicLoadBalancing) {
            NTSCFG_TEST_EQ(interface->numProactors(), 1);
        }
        else {
            NTSCFG_TEST_EQ(interface->numProactors(), numThreads);
        }
    }

    NTSCFG_TEST_EQ(interface->numThreads(), 0);

    if (dynamicThreadCount) {
        NTSCFG_TEST_EQ(interface->minThreads(), 1);
    }
    else {
        NTSCFG_TEST_EQ(interface->minThreads(), numThreads);
    }

    NTSCFG_TEST_EQ(interface->maxThreads(), numThreads);

    interface->start();

    if (dynamicThreadCount) {
        if (restartIteration == 0) {
            NTSCFG_TEST_EQ(interface->numProactors(), 1);
        }
        else {
            if (dynamicLoadBalancing) {
                NTSCFG_TEST_EQ(interface->numProactors(), 1);
            }
            else {
                NTSCFG_TEST_EQ(interface->numProactors(), numThreads);
            }
        }

        if (restartIteration == 0) {
            NTSCFG_TEST_EQ(interface->numThreads(), 1);
        }
        else {
            NTSCFG_TEST_EQ(interface->numThreads(), numThreads);
        }

        NTSCFG_TEST_EQ(interface->minThreads(), 1);
        NTSCFG_TEST_EQ(interface->maxThreads(), numThreads);

        if (restartIteration == 0) {
            for (bsl::size_t newNumThreads = 2;
                 newNumThreads <= interface->maxThreads();
                 ++newNumThreads)
            {
                bool grew = interface->expand();
                NTSCFG_TEST_TRUE(grew);

                if (dynamicLoadBalancing) {
                    NTSCFG_TEST_EQ(interface->numProactors(), 1);
                }
                else {
                    NTSCFG_TEST_EQ(interface->numProactors(), newNumThreads);
                }

                NTSCFG_TEST_EQ(interface->numThreads(), newNumThreads);
                NTSCFG_TEST_EQ(interface->minThreads(), 1);
                NTSCFG_TEST_EQ(interface->maxThreads(), numThreads);
            }

            {
                bool grew = interface->expand();
                NTSCFG_TEST_FALSE(grew);
            }
        }
        else {
            bool grew = interface->expand();
            NTSCFG_TEST_FALSE(grew);
        }

        if (dynamicLoadBalancing) {
            NTSCFG_TEST_EQ(interface->numProactors(), 1);
        }
        else {
            NTSCFG_TEST_EQ(interface->numProactors(), numThreads);
        }

        NTSCFG_TEST_EQ(interface->numThreads(), numThreads);
        NTSCFG_TEST_EQ(interface->minThreads(), 1);
        NTSCFG_TEST_EQ(interface->maxThreads(), numThreads);
    }
    else {
        if (dynamicLoadBalancing) {
            NTSCFG_TEST_EQ(interface->numProactors(), 1);
        }
        else {
            NTSCFG_TEST_EQ(interface->numProactors(), numThreads);
        }

        NTSCFG_TEST_EQ(interface->numThreads(), numThreads);
        NTSCFG_TEST_EQ(interface->minThreads(), numThreads);
        NTSCFG_TEST_EQ(interface->maxThreads(), numThreads);
    }

    interface->shutdown();
    interface->linger();

    if (dynamicLoadBalancing) {
        NTSCFG_TEST_EQ(interface->numProactors(), 1);
    }
    else {
        NTSCFG_TEST_EQ(interface->numProactors(), numThreads);
    }

    NTSCFG_TEST_EQ(interface->numThreads(), 0);

    if (dynamicThreadCount) {
        NTSCFG_TEST_EQ(interface->minThreads(), 1);
    }
    else {
        NTSCFG_TEST_EQ(interface->minThreads(), numThreads);
    }

    NTSCFG_TEST_EQ(interface->maxThreads(), numThreads);
}

NTSCFG_TEST_FUNCTION(ntcp::InterfaceTest::verifyCase1)
{
    ntsa::Error error;

    // Create the simulation.

    bsl::shared_ptr<ntcd::Simulation> simulation;
    simulation.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    error = simulation->run();
    NTSCFG_TEST_OK(error);

    // Create the data pool.

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Create the proactor factory.

    bsl::shared_ptr<ntcd::ProactorFactory> proactorFactory;
    proactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR,
                                  NTSCFG_TEST_ALLOCATOR);

    // Create the interface.

    ntca::InterfaceConfig interfaceConfig;
    interfaceConfig.setMetricName("test");
    interfaceConfig.setMinThreads(1);
    interfaceConfig.setMaxThreads(1);

    bsl::shared_ptr<ntcp::Interface> interface;
    interface.createInplace(NTSCFG_TEST_ALLOCATOR,
                            interfaceConfig,
                            dataPool,
                            proactorFactory,
                            NTSCFG_TEST_ALLOCATOR);

    error = interface->start();
    NTSCFG_TEST_OK(error);

    // Ensure the interface has created one proactor run by one thread.

    NTSCFG_TEST_EQ(interface->numProactors(), 1);
    NTSCFG_TEST_EQ(interface->numThreads(), 1);

    // Stop the interface.

    interface->shutdown();
    interface->linger();

    // Ensure the interface is no longer running any threads but the
    // original proactor still exists.

    NTSCFG_TEST_EQ(interface->numProactors(), 1);
    NTSCFG_TEST_EQ(interface->numThreads(), 0);

    // Stop the simulation.

    simulation->stop();
}

NTSCFG_TEST_FUNCTION(ntcp::InterfaceTest::verifyCase2)
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
    simulation.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    error = simulation->run();
    NTSCFG_TEST_OK(error);

    // Create the data pool.

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Create the proactor factory.

    bsl::shared_ptr<ntcd::ProactorFactory> proactorFactory;
    proactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR,
                                  NTSCFG_TEST_ALLOCATOR);

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

            BSLS_LOG_DEBUG("Testing driver (%s) (%s)",
                          (dynamicLoadBalancing ? "dynamic" : "static"),
                          (dynamicThreadCount ? "resizable" : "fixed"));

            for (bsl::size_t numThreads = MIN_THREADS;
                 numThreads <= MAX_THREADS;
                 ++numThreads)
            {
                BSLS_LOG_TRACE("Testing %d threads", (int)(numThreads));

                ntca::InterfaceConfig interfaceConfig;
                interfaceConfig.setMetricName("test");

                if (dynamicThreadCount) {
                    interfaceConfig.setMinThreads(1);
                }
                else {
                    interfaceConfig.setMinThreads(numThreads);
                }

                interfaceConfig.setMaxThreads(numThreads);
                interfaceConfig.setDynamicLoadBalancing(dynamicLoadBalancing);

                bsl::shared_ptr<ntcp::Interface> interface;
                interface.createInplace(NTSCFG_TEST_ALLOCATOR,
                                        interfaceConfig,
                                        dataPool,
                                        proactorFactory,
                                        NTSCFG_TEST_ALLOCATOR);

                for (bsl::size_t restartIteration = 0;
                     restartIteration < NUM_RESTARTS;
                     ++restartIteration)
                {
                    InterfaceTest::run(interface,
                                       numThreads,
                                       restartIteration,
                                       dynamicLoadBalancing,
                                       dynamicThreadCount);
                }
            }
        }
    }

    // Stop the simulation.

    simulation->stop();
}

}  // close namespace ntcp
}  // close namespace BloombergLP
