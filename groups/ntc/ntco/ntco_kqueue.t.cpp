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

#include <ntco_kqueue.h>
#include <ntco_test.h>
#include <ntci_timerfuture.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntco {

// Provide tests for 'ntco::Kqueue'.
class KqueueTest
{
#if NTC_BUILD_WITH_KQUEUE

    // Process the specified reactor 'event' by incrementing the specified
    // 'eventCount'.
    static void processReactorEvent(
        bsls::AtomicUint64*                    eventCount,
        const BloombergLP::ntca::ReactorEvent& event);

#endif

  public:
    // Verify the reactor implements sockets.
    static void verifySockets();

    // Verify the reactor implements timers.
    static void verifyTimers();

    // Verify the reactor implements deferred functions.
    static void verifyFunctions();

    // Verify maximum relative timeouts.
    static void verifyTimerLimit();
};

#if NTC_BUILD_WITH_KQUEUE
void KqueueTest::processReactorEvent(
    bsls::AtomicUint64*                    eventCount,
    const BloombergLP::ntca::ReactorEvent& event)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Reactor event " << event << NTCI_LOG_STREAM_END;

    eventCount->add(1);
}
#endif

NTSCFG_TEST_FUNCTION(ntco::KqueueTest::verifySockets)
{
#if NTC_BUILD_WITH_KQUEUE

    bsl::shared_ptr<ntco::KqueueFactory> reactorFactory;
    reactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    Test::verifyReactorSockets(reactorFactory);

#endif
}

NTSCFG_TEST_FUNCTION(ntco::KqueueTest::verifyTimers)
{
#if NTC_BUILD_WITH_KQUEUE

    bsl::shared_ptr<ntco::KqueueFactory> reactorFactory;
    reactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    Test::verifyReactorTimers(reactorFactory);

#endif
}

NTSCFG_TEST_FUNCTION(ntco::KqueueTest::verifyFunctions)
{
#if NTC_BUILD_WITH_KQUEUE

    bsl::shared_ptr<ntco::KqueueFactory> reactorFactory;
    reactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    Test::verifyReactorFunctions(reactorFactory);

#endif
}

NTSCFG_TEST_FUNCTION(ntco::KqueueTest::verifyTimerLimit)
{
#if NTC_BUILD_WITH_KQUEUE

    ntsa::Error error;

    bsl::shared_ptr<ntci::User> user;

    ntca::ReactorConfig reactorConfig;
    reactorConfig.setDriverName("kqueue");
    reactorConfig.setMaxThreads(1);
    reactorConfig.setAutoAttach(true);
    reactorConfig.setAutoDetach(true);

    bsl::shared_ptr<ntco::KqueueFactory> reactorFactory;
    reactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Reactor> reactor = reactorFactory->createReactor(
        reactorConfig, 
        user, 
        NTSCFG_TEST_ALLOCATOR);

    ntca::WaiterOptions waiterOptions;
    waiterOptions.setThreadHandle(bslmt::ThreadUtil::self());
    waiterOptions.setThreadIndex(0);

    ntci::Waiter waiter = reactor->registerWaiter(waiterOptions);

    ntsa::Handle client = ntsa::k_INVALID_HANDLE;
    ntsa::Handle server = ntsa::k_INVALID_HANDLE;

    error = ntsf::System::createStreamSocketPair(
        &client, &server, ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_NE(client, ntsa::k_INVALID_HANDLE);
    NTSCFG_TEST_NE(server, ntsa::k_INVALID_HANDLE);

    ntsa::SendContext sendContext;
    ntsa::SendOptions sendOptions;

    error = ntsf::System::send(
        &sendContext, "X", sendOptions, client);
    NTSCFG_TEST_OK(error);

    error = ntsf::System::waitUntilReadable(server);
    NTSCFG_TEST_OK(error);

    ntca::ReactorEventOptions reactorEventOptions;

    bsls::AtomicUint64 reactorEventCount(0);

    ntci::ReactorEventCallback reactorEventCallback(
        NTCCFG_BIND(&ntco::KqueueTest::processReactorEvent, 
                    &reactorEventCount,
                    NTCCFG_BIND_PLACEHOLDER_1));

    error = reactor->showReadable(
        server, reactorEventOptions, reactorEventCallback);
    NTSCFG_TEST_OK(error);

    ntca::TimerOptions timerOptions;
    timerOptions.setOneShot(true);

    ntci::TimerFuture timerFuture;

    bsl::shared_ptr<ntci::Timer> timer = reactor->createTimer(
        timerOptions, 
        timerFuture, 
        NTSCFG_TEST_ALLOCATOR);

    bsls::TimeInterval timerDeadline(
        bsl::numeric_limits<bsls::Types::Int64>::max(), 
        0);
    
    error = timer->schedule(timerDeadline);
    NTSCFG_TEST_OK(error);

    bsls::Types::Uint64 c0 = reactorEventCount.load();
    reactor->poll(waiter);
    bsls::Types::Uint64 c1 = reactorEventCount.load();

    NTSCFG_TEST_EQ(c1, c0 + 1);

    error = reactor->hideReadable(server);
    NTSCFG_TEST_OK(error);

    error = ntsf::System::close(client);
    NTSCFG_TEST_OK(error);

    error = ntsf::System::close(server);
    NTSCFG_TEST_OK(error);

    timer->close();
    
    reactor->deregisterWaiter(waiter);

#endif
}

}  // close namespace ntco
}  // close namespace BloombergLP
