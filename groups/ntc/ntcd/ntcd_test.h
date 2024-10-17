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

#ifndef INCLUDED_NTCD_TEST
#define INCLUDED_NTCD_TEST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntcd_encryption.h>
#include <ntcd_datautil.h>
#include <ntcd_simulation.h>
#include <ntcd_reactor.h>
#include <ntcd_proactor.h>
#include <ntcm_monitorable.h>
#include <ntca_reactorconfig.h>
#include <ntca_proactorconfig.h>
#include <ntccfg_platform.h>
#include <ntci_log.h>
#include <ntci_reactor.h>
#include <ntci_reactorfactory.h>
#include <ntci_proactor.h>
#include <ntci_proactorfactory.h>
#include <ntci_user.h>
#include <ntcs_datapool.h>
#include <ntcs_strand.h>
#include <ntcs_user.h>
#include <ntcscm_version.h>
#include <ntsf_system.h>
#include <ntscfg_test.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcd {

// Uncomment to restrict testing static vs. dynamic load balancing.
// #define NTCD_TEST_DYNAMIC_LOAD_BALANCING false

/// Provide a test case execution framework.
class TestFramework
{
 public:
    /// Define a type alias for the function implementing a test case driven by
    /// this test framework.
    typedef NTCCFG_FUNCTION(const bsl::shared_ptr<ntci::Reactor>& reactor) 
                            ReactorTestCallback;

    /// Define a type alias for the function implementing a test case driven by
    /// this test framework.
    typedef NTCCFG_FUNCTION(const bsl::shared_ptr<ntci::Proactor>& proactor) 
                            ProactorTestCallback;

  private:
    /// Run a thread identified by the specified 'threadIndex' that waits
    /// on the specified 'barrier' then drives the specified 'reactor' until
    /// it is stopped.
    static void runReactor(const bsl::shared_ptr<ntci::Reactor>& reactor,
                           bslmt::Barrier*                       barrier,
                           bsl::size_t                           threadIndex);

    /// Run a thread identified by the specified 'threadIndex' that waits
    /// on the specified 'barrier' then drives the specified 'proactor' until
    /// it is stopped.
    static void runProactor(const bsl::shared_ptr<ntci::Proactor>& proactor,
                            bslmt::Barrier*                        barrier,
                            bsl::size_t threadIndex);

public:
    /// Execute the specified 'callback' implementing a test case, varying the
    /// test configuration and machinery by driver type, and number
    /// of threads.
    static void verifyReactor(const ReactorTestCallback& callback);

    /// Execute the specified 'callback' implementing a test case, varying the
    /// test configuration and machinery by driver type, and number
    /// of threads.
    static void verifyProactor(const ProactorTestCallback& callback);


    /// Return an endpoint representing a suitable address to which to
    /// bind a socket of the specified 'transport' type for use by this
    /// test driver.
    static ntsa::Endpoint any(ntsa::Transport::Value transport);
};

void TestFramework::runReactor(const bsl::shared_ptr<ntci::Reactor>& reactor,
                               bslmt::Barrier*                       barrier,
                               bsl::size_t                           threadIndex)
{
    const char* threadNamePrefix = "test";

    bsl::string threadName;
    {
        bsl::stringstream ss;
        ss << threadNamePrefix << "-" << threadIndex;
        threadName = ss.str();
    }

    bslmt::ThreadUtil::setThreadName(threadName);

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER(threadNamePrefix);
    NTCI_LOG_CONTEXT_GUARD_THREAD(threadIndex);

    // Register this thread as the thread that will wait on the reactor.

    ntci::Waiter waiter = reactor->registerWaiter(ntca::WaiterOptions());

    // Wait until all threads have reached the rendezvous point.

    barrier->wait();

    // Process deferred functions.

    reactor->run(waiter);

    // Deregister the waiter.

    reactor->deregisterWaiter(waiter);
}

void TestFramework::runProactor(const bsl::shared_ptr<ntci::Proactor>& proactor,
                            bslmt::Barrier*                        barrier,
                            bsl::size_t                            threadIndex)
{
    const char* threadNamePrefix = "test";

    bsl::string threadName;
    {
        bsl::stringstream ss;
        ss << threadNamePrefix << "-" << threadIndex;
        threadName = ss.str();
    }

    bslmt::ThreadUtil::setThreadName(threadName);

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER(threadNamePrefix);
    NTCI_LOG_CONTEXT_GUARD_THREAD(threadIndex);

    // Register this thread as the thread that will wait on the proactor.

    ntci::Waiter waiter = proactor->registerWaiter(ntca::WaiterOptions());

    // Wait until all threads have reached the rendezvous point.

    barrier->wait();

    // Process deferred functions.

    proactor->run(waiter);

    // Deregister the waiter.

    proactor->deregisterWaiter(waiter);
}

void TestFramework::verifyReactor(const ReactorTestCallback& callback)
{
    ntsa::Error error;

#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    const bsl::size_t k_MIN_THREADS = 1;
    const bsl::size_t k_MAX_THREADS = 1;

#else

    const bsl::size_t k_MIN_THREADS = 1;
    const bsl::size_t k_MAX_THREADS = 1;

#endif

    for (bsl::size_t numThreads = k_MIN_THREADS; numThreads <= k_MAX_THREADS;
         ++numThreads)
    {
        const bool dynamicLoadBalancing = numThreads > 1;

#if defined(NTCD_TEST_DYNAMIC_LOAD_BALANCING)
        if (dynamicLoadBalancing != NTCD_TEST_DYNAMIC_LOAD_BALANCING) {
            continue;
        }
#endif

        BSLS_LOG_INFO("Testing numThreads %d", (int)(numThreads));

        bsl::shared_ptr<ntcd::Simulation> simulation;
        simulation.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

        error = simulation->run();
        NTCCFG_TEST_OK(error);

        const bsl::size_t k_BLOB_BUFFER_SIZE = 4096;

        bsl::shared_ptr<ntcs::DataPool> dataPool;
        dataPool.createInplace(
            NTSCFG_TEST_ALLOCATOR, 
            k_BLOB_BUFFER_SIZE, 
            k_BLOB_BUFFER_SIZE, 
            NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<ntcs::User> user;
        user.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

        user->setDataPool(dataPool);

        ntca::ReactorConfig reactorConfig;
        reactorConfig.setMetricName("test");
        reactorConfig.setMinThreads(numThreads);
        reactorConfig.setMaxThreads(numThreads);
        reactorConfig.setAutoAttach(false);
        reactorConfig.setAutoDetach(false);
        reactorConfig.setOneShot(numThreads > 1);

        bsl::shared_ptr<ntcd::Reactor> reactor;
        reactor.createInplace(
            NTSCFG_TEST_ALLOCATOR, reactorConfig, user, NTSCFG_TEST_ALLOCATOR);

        bslmt::Barrier threadGroupBarrier(numThreads + 1);

        bslmt::ThreadGroup threadGroup(NTSCFG_TEST_ALLOCATOR);

        for (bsl::size_t threadIndex = 0; threadIndex < numThreads;
                ++threadIndex)
        {
            threadGroup.addThread(NTCCFG_BIND(&TestFramework::runReactor,
                                                reactor,
                                                &threadGroupBarrier,
                                                threadIndex));
        }

        threadGroupBarrier.wait();

        callback(reactor);

        reactor->stop();
        threadGroup.joinAll();

        simulation->stop();
    }
}

void TestFramework::verifyProactor(const ProactorTestCallback& callback)
{
    ntsa::Error error;

#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    const bsl::size_t k_MIN_THREADS = 1;
    const bsl::size_t k_MAX_THREADS = 1;

#else

    const bsl::size_t k_MIN_THREADS = 1;
    const bsl::size_t k_MAX_THREADS = 1;

#endif

    for (bsl::size_t numThreads = k_MIN_THREADS; numThreads <= k_MAX_THREADS;
         ++numThreads)
    {
        const bool dynamicLoadBalancing = numThreads > 1;

#if defined(NTCP_DATAGRAM_SOCKET_TEST_DYNAMIC_LOAD_BALANCING)
        if (dynamicLoadBalancing !=
            NTCP_DATAGRAM_SOCKET_TEST_DYNAMIC_LOAD_BALANCING)
        {
            continue;
        }
#endif

        BSLS_LOG_INFO("Testing numThreads %d", (int)(numThreads));

        bsl::shared_ptr<ntcd::Simulation> simulation;
        simulation.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

        error = simulation->run();
        NTCCFG_TEST_OK(error);

        const bsl::size_t k_BLOB_BUFFER_SIZE = 4096;

        bsl::shared_ptr<ntcs::DataPool> dataPool;
        dataPool.createInplace(
            NTSCFG_TEST_ALLOCATOR, 
            k_BLOB_BUFFER_SIZE, 
            k_BLOB_BUFFER_SIZE, 
            NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<ntcs::User> user;
        user.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

        user->setDataPool(dataPool);

        ntca::ProactorConfig proactorConfig;
        proactorConfig.setMetricName("test");
        proactorConfig.setMinThreads(numThreads);
        proactorConfig.setMaxThreads(numThreads);

        bsl::shared_ptr<ntcd::Proactor> proactor;
        proactor.createInplace(
            NTSCFG_TEST_ALLOCATOR, 
            proactorConfig, 
            user, 
            NTSCFG_TEST_ALLOCATOR);

        bslmt::Barrier threadGroupBarrier(numThreads + 1);

        bslmt::ThreadGroup threadGroup(NTSCFG_TEST_ALLOCATOR);

        for (bsl::size_t threadIndex = 0; threadIndex < numThreads;
             ++threadIndex)
        {
            threadGroup.addThread(NTCCFG_BIND(&TestFramework::runProactor,
                                              proactor,
                                              &threadGroupBarrier,
                                              threadIndex));
        }

        threadGroupBarrier.wait();

        callback(proactor);

        proactor->stop();
        threadGroup.joinAll();

        simulation->stop();
    }
}

ntsa::Endpoint TestFramework::any(ntsa::Transport::Value transport)
{
    ntsa::Endpoint endpoint;

    switch (transport) {
    case ntsa::Transport::e_TCP_IPV4_STREAM:
    case ntsa::Transport::e_UDP_IPV4_DATAGRAM:
        endpoint.makeIp(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0));
        break;
    case ntsa::Transport::e_TCP_IPV6_STREAM:
    case ntsa::Transport::e_UDP_IPV6_DATAGRAM:
        endpoint.makeIp(ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), 0));
        break;
    case ntsa::Transport::e_LOCAL_STREAM:
    case ntsa::Transport::e_LOCAL_DATAGRAM: {
        ntsa::LocalName   localName;
        const ntsa::Error error = ntsa::LocalName::generateUnique(&localName);
        BSLS_ASSERT_OPT(!error);

        endpoint.makeLocal(localName);
        break;
    }
    default:
        NTCCFG_UNREACHABLE();
    }

    return endpoint;
}

} // close namespace ntcd
} // close namespace BloombergLP
#endif
