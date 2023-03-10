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

#include <ntcd_reactor.h>

#include <ntccfg_bind.h>
#include <ntccfg_test.h>
#include <ntcd_datautil.h>
#include <ntcd_simulation.h>
#include <ntci_log.h>
#include <ntcs_strand.h>
#include <ntsf_system.h>
#include <ntsi_datagramsocket.h>
#include <ntsi_listenersocket.h>
#include <ntsi_streamsocket.h>
#include <bdlb_random.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bdlma_concurrentmultipoolallocator.h>
#include <bdlmt_eventscheduler.h>
#include <bslma_testallocator.h>
#include <bslmt_barrier.h>
#include <bslmt_latch.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>
#include <bslmt_turnstile.h>
#include <bsls_stopwatch.h>
#include <bsls_timeinterval.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_unordered_map.h>

using namespace BloombergLP;

// Uncomment to enable testing of shutting down writes.
#define NTCR_REACTOR_TEST_SHUTDOWN_WRITE 1

// Uncomment to enable testing of shutting down reads.
#define NTCR_REACTOR_TEST_SHUTDOWN_READ 0

namespace test {

namespace case1 {

ntsa::Error processDescriptorEvent(bslmt::Latch*             latch,
                                   const ntca::ReactorEvent& event)
{
    latch->arrive();
    return ntsa::Error();
}

void execute(bool autoManage, bslma::Allocator* allocator)
{
    NTCCFG_TEST_LOG_WARN << "==================" << NTCCFG_TEST_LOG_END;
    NTCCFG_TEST_LOG_WARN << "AM: " << autoManage << NTCCFG_TEST_LOG_END;
    NTCCFG_TEST_LOG_WARN << "------------------" << NTCCFG_TEST_LOG_END;

    ntsa::Error error;

    // Create a number of latches that track the events that should
    // occur.

    bslmt::Latch listenerAcceptable(1);

    bslmt::Latch clientConnected(1);
    bslmt::Latch clientReadableAfterServerShutdownSend(1);
    bslmt::Latch clientReadableAfterClientShutdownReceive(1);
    bslmt::Latch clientWritableAfterClientShutdownSend(1);

    bslmt::Latch serverReadable(1);
    bslmt::Latch serverReadableAfterClientShutdownSend(1);
    bslmt::Latch serverReadableAfterServerShutdownReceive(1);
    bslmt::Latch serverWritableAfterServerShutdownSend(1);

    // Define the user.

    bsl::shared_ptr<ntci::User> user;

    // Create the simulation.

    bsl::shared_ptr<ntcd::Simulation> simulation;
    simulation.createInplace(allocator, allocator);

    error = simulation->run();
    NTCCFG_TEST_OK(error);

    // Create the reactor.

    ntca::ReactorConfig reactorConfig;

    reactorConfig.setMetricName("test");
    reactorConfig.setMinThreads(1);
    reactorConfig.setMaxThreads(1);

    if (autoManage) {
        reactorConfig.setAutoAttach(true);
        reactorConfig.setAutoDetach(true);
    }

    bsl::shared_ptr<ntcd::Reactor> reactor =
        simulation->createReactor(reactorConfig, user, allocator);

    // The reactor is initialize not managing any sockets.

    NTCCFG_TEST_EQ(reactor->numSockets(), 0);

    // Register this thread as the thread that will wait on the
    // reactor.

    ntci::Waiter waiter = reactor->registerWaiter(ntca::WaiterOptions());

    // Create a TCP/IPv4 non-blocking socket bound to any ephemeral
    // port on the IPv4 loopback address, and begin listening for
    // connections.

    bsl::shared_ptr<ntsi::ListenerSocket> listener =
        simulation->createListenerSocket(allocator);

    error = listener->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTCCFG_TEST_FALSE(error);

    error = listener->setBlocking(false);
    NTCCFG_TEST_FALSE(error);

    error = listener->bind(ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                           false);
    NTCCFG_TEST_FALSE(error);

    error = listener->listen(0);
    NTCCFG_TEST_FALSE(error);

    // Create a TCP/IPv4 non-blocking socket and connect that socket to
    // the listener's local endpoint.

    bsl::shared_ptr<ntsi::StreamSocket> client =
        simulation->createStreamSocket(allocator);

    error = client->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTCCFG_TEST_FALSE(error);

    error = client->setBlocking(false);
    NTCCFG_TEST_FALSE(error);

    ntsa::Endpoint listenerEndpoint;
    error = listener->sourceEndpoint(&listenerEndpoint);
    NTCCFG_TEST_FALSE(error);

    error = client->connect(listenerEndpoint);
    NTCCFG_TEST_TRUE(error == ntsa::Error::e_OK ||
                     error == ntsa::Error::e_PENDING ||
                     error == ntsa::Error::e_WOULD_BLOCK);

    if (!autoManage) {
        // Attach the listener socket to the reactor.

        reactor->attachSocket(listener->handle());

        NTCCFG_TEST_EQ(reactor->numSockets(), 1);

        // Attach the client socket to the reactor.

        reactor->attachSocket(client->handle());

        NTCCFG_TEST_EQ(reactor->numSockets(), 2);
    }

    // Become interested when the client is writable, that is, it has
    // connected to its peer.

    reactor->showWritable(client->handle(),
                          ntca::ReactorEventOptions(),
                          ntci::ReactorEventCallback(
                              NTCCFG_BIND(&test::case1::processDescriptorEvent,
                                          &clientConnected,
                                          NTCCFG_BIND_PLACEHOLDER_1)));

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 1);
    }

    // Wait until the client is writable.

    while (!clientConnected.tryWait()) {
        reactor->poll(waiter);
    }

    // The client is now connected. Lose interest in the writability of
    // the client.

    reactor->hideWritable(client->handle());

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 0);
    }

    // Become interested when the listener is readable, that is, it has
    // a connection available to be accepted.

    reactor->showReadable(listener->handle(),
                          ntca::ReactorEventOptions(),
                          ntci::ReactorEventCallback(
                              NTCCFG_BIND(&test::case1::processDescriptorEvent,
                                          &listenerAcceptable,
                                          NTCCFG_BIND_PLACEHOLDER_1)));

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 1);
    }

    // Wait until the listener is readable.

    while (!listenerAcceptable.tryWait()) {
        reactor->poll(waiter);
    }

    // Accept the connection made to the listener.

    bsl::shared_ptr<ntsi::StreamSocket> server;
    error = listener->accept(&server, allocator);
    NTCCFG_TEST_FALSE(error);

    error = server->setBlocking(false);
    NTCCFG_TEST_FALSE(error);

    // All connections have been accepted. Lose interest in the
    // readability of the listener.

    reactor->hideReadable(listener->handle());

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 0);
    }

    // The server socket has been accepted.

    if (!autoManage) {
        // Attach the server socket to the reactor.

        reactor->attachSocket(server->handle());

        NTCCFG_TEST_EQ(reactor->numSockets(), 3);
    }

    // Send a single byte to the server.

    {
        char buffer = 'X';

        ntsa::SendContext context;
        ntsa::SendOptions options;

        ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

        error = client->send(&context, data, options);
        NTCCFG_TEST_FALSE(error);

        NTCCFG_TEST_EQ(context.bytesSendable(), 1);
        NTCCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Become interested in the readability of the server.

    reactor->showReadable(server->handle(),
                          ntca::ReactorEventOptions(),
                          ntci::ReactorEventCallback(
                              NTCCFG_BIND(&test::case1::processDescriptorEvent,
                                          &serverReadable,
                                          NTCCFG_BIND_PLACEHOLDER_1)));

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 1);
    }

    // Wait until the server socket is readable.

    while (!serverReadable.tryWait()) {
        reactor->poll(waiter);
    }

    // Receive a single byte from the client.

    {
        char buffer;

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

        error = server->receive(&context, &data, options);
        NTCCFG_TEST_FALSE(error);

        NTCCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTCCFG_TEST_EQ(context.bytesReceived(), 1);
        NTCCFG_TEST_EQ(buffer, 'X');
    }

    // The server has now read data from the client. Lose interest in
    // the readability of the server.

    reactor->hideReadable(server->handle());

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 0);
    }

#if NTCR_REACTOR_TEST_SHUTDOWN_WRITE

    // Shutdown writing from the server.

    error = server->shutdown(ntsa::ShutdownType::e_SEND);
    NTCCFG_TEST_FALSE(error);

    // Become interested in the writability of the server.

    reactor->showWritable(server->handle(),
                          ntca::ReactorEventOptions(),
                          ntci::ReactorEventCallback(NTCCFG_BIND(
                              &test::case1::processDescriptorEvent,
                              &serverWritableAfterServerShutdownSend,
                              NTCCFG_BIND_PLACEHOLDER_1)));

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 1);
    }

    // Wait until the server socket is writable, even though the
    // server has shut down writing from its side of the connection.

    while (!serverWritableAfterServerShutdownSend.tryWait()) {
        reactor->poll(waiter);
    }

    // Try to send a single byte to the client, but observe the send
    // fails because the server has shutdown writing from its side of
    // the connection.

    {
        char buffer = 'X';

        ntsa::SendContext context;
        ntsa::SendOptions options;

        ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

        error = server->send(&context, data, options);
        NTCCFG_TEST_TRUE(error);
        NTCCFG_TEST_EQ(error.code(), ntsa::Error::e_CONNECTION_DEAD);

        NTCCFG_TEST_EQ(context.bytesSendable(), 1);
        NTCCFG_TEST_EQ(context.bytesSent(), 0);
    }

    // Lose interest in the writability of the server.

    reactor->hideWritable(server->handle());

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 0);
    }

    // Become interested in the readability of the client.

    reactor->showReadable(client->handle(),
                          ntca::ReactorEventOptions(),
                          ntci::ReactorEventCallback(NTCCFG_BIND(
                              &test::case1::processDescriptorEvent,
                              &clientReadableAfterServerShutdownSend,
                              NTCCFG_BIND_PLACEHOLDER_1)));

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 1);
    }

    // Wait until the client socket is readable.

    while (!clientReadableAfterServerShutdownSend.tryWait()) {
        reactor->poll(waiter);
    }

    // Receive a single byte from the server, but observe that zero
    // bytes have been successfully read, indicating the server has
    // shutdown writing from its side of the connection.

    {
        char buffer;

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

        error = client->receive(&context, &data, options);
        if (error) {
            NTCCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_EOF));
        }
        else {
            NTCCFG_TEST_EQ(context.bytesReceivable(), 1);
            NTCCFG_TEST_EQ(context.bytesReceived(), 0);
        }
    }

    // The client has now learned the server has shutdown writing from
    // its side of the connection. Lose interest in the readability of
    // the client.

    reactor->hideReadable(client->handle());

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 0);
    }

    // Shutdown writing from the client.

    error = client->shutdown(ntsa::ShutdownType::e_SEND);
    NTCCFG_TEST_FALSE(error);

    // Become interested in the writability of the client.

    reactor->showWritable(client->handle(),
                          ntca::ReactorEventOptions(),
                          ntci::ReactorEventCallback(NTCCFG_BIND(
                              &test::case1::processDescriptorEvent,
                              &clientWritableAfterClientShutdownSend,
                              NTCCFG_BIND_PLACEHOLDER_1)));

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 1);
    }

    // Wait until the client socket is writable, even though the
    // client has shut down writing from its side of the connection.

    while (!clientWritableAfterClientShutdownSend.tryWait()) {
        reactor->poll(waiter);
    }

    // Try to send a single byte to the server, but observe the send
    // fails because the client has shutdown writing from its side of
    // the connection.

    {
        char buffer = 'X';

        ntsa::SendContext context;
        ntsa::SendOptions options;

        ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

        error = client->send(&context, data, options);
        NTCCFG_TEST_TRUE(error);
        NTCCFG_TEST_EQ(error.code(), ntsa::Error::e_CONNECTION_DEAD);

        NTCCFG_TEST_EQ(context.bytesSendable(), 1);
        NTCCFG_TEST_EQ(context.bytesSent(), 0);
    }

    // Lose interest in the writability of the client.

    reactor->hideWritable(client->handle());

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 0);
    }

    // Become interested in the readability of the server.

    reactor->showReadable(server->handle(),
                          ntca::ReactorEventOptions(),
                          ntci::ReactorEventCallback(NTCCFG_BIND(
                              &test::case1::processDescriptorEvent,
                              &serverReadableAfterClientShutdownSend,
                              NTCCFG_BIND_PLACEHOLDER_1)));

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 1);
    }

    // Wait until the server socket is readable.

    while (!serverReadableAfterClientShutdownSend.tryWait()) {
        reactor->poll(waiter);
    }

    // Receive a single byte from the client, but observe that zero
    // bytes have been successfully read, indicating the client has
    // shutdown writing from its side of the connection.

    {
        char buffer;

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

        error = server->receive(&context, &data, options);
        if (error) {
            NTCCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_EOF));
        }
        else {
            NTCCFG_TEST_EQ(context.bytesReceivable(), 1);
            NTCCFG_TEST_EQ(context.bytesReceived(), 0);
        }
    }

    // The server has now learned the client has shutdown writing from
    // its side of the connection. Lose interest in the readability of
    // the server.

    reactor->hideReadable(server->handle());

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 0);
    }

#endif

#if NTCR_REACTOR_TEST_SHUTDOWN_READ

    // Shutdown reading from the server.

    error = server->shutdown(ntsa::ShutdownType::e_RECEIVE);
    if (error) {
        BSLS_LOG_ERROR("Error: %s", error.text().c_str());
    }
    NTCCFG_TEST_FALSE(error);

    // Become interested in the readability of the server.

    reactor->showReadable(server->handle(),
                          ntca::ReactorEventOptions(),
                          ntci::ReactorEventCallback(NTCCFG_BIND(
                              &test::case1::processDescriptorEvent,
                              &serverReadableAfterServerShutdownReceive,
                              NTCCFG_BIND_PLACEHOLDER_1)));

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 1);
    }

    // Wait until the server socket is readable, even though the
    // server has shutdown reading from its side of the connection.

    while (!serverReadableAfterServerShutdownReceive.tryWait()) {
        reactor->poll(waiter);
    }

    // Try to receive a single byte from the client, and observe the
    // receive either fails indicating the connection is dead, or
    // succeeds even though the server has shutdown reading
    // from its side of the connection, but that zero bytes are read.

    {
        char buffer;

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

        error = server->receive(&context, &data, options);
        if (error) {
            NTCCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
        }
        else {
            NTCCFG_TEST_FALSE(error);
        }

        NTCCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTCCFG_TEST_EQ(context.bytesReceived(), 0);
    }

    // Lose interest in the readability of the server.

    reactor->hideReadable(server->handle());

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 0);
    }

    // Shutdown reading from the client.

    error = client->shutdown(ntsa::ShutdownType::e_RECEIVE);
    NTCCFG_TEST_FALSE(error);

    // Become interested in the readability of the client.

    reactor->showReadable(client->handle(),
                          ntca::ReactorEventOptions(),
                          ntci::ReactorEventCallback(NTCCFG_BIND(
                              &test::case1::processDescriptorEvent,
                              &clientReadableAfterClientShutdownReceive,
                              NTCCFG_BIND_PLACEHOLDER_1)));

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 1);
    }

    // Wait until the client socket is readable, even though the
    // client has shutdown reading from its side of the connection.

    while (!clientReadableAfterClientShutdownReceive.tryWait()) {
        reactor->poll(waiter);
    }

    // Try to receive a single byte from the server, and observe the
    // receive either fails indicating the connection is dead, or
    // succeeds even though the client has shutdown reading
    // from its side of the connection, but that zero bytes are read.

    {
        char buffer;

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

        error = client->receive(&context, &data, options);
        if (error) {
            NTCCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
        }
        else {
            NTCCFG_TEST_FALSE(error);
        }

        NTCCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTCCFG_TEST_EQ(context.bytesReceived(), 0);
    }

    // Lose interest in the readability of the server.

    reactor->hideReadable(client->handle());

    if (autoManage) {
        NTCCFG_TEST_EQ(reactor->numSockets(), 0);
    }

#endif

    if (!autoManage) {
        // Detach the server from the reactor.

        reactor->detachSocket(server->handle());

        // Detach the client from the reactor.

        reactor->detachSocket(client->handle());

        // Detach the listener from the reactor.

        reactor->detachSocket(listener->handle());
    }

    NTCCFG_TEST_EQ(reactor->numSockets(), 0);

    // Deregister the waiter.

    reactor->deregisterWaiter(waiter);

    // Now that the sockets have been detached from the reactor, they
    // can be closed.

    listener->close();
    client->close();
    server->close();

    // Stop the simulation.

    simulation->stop();
}

}  // close namespace case1
}  // close namespace test

NTCCFG_TEST_CASE(1)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    const bool AUTO_MANAGE[] = {false, true};

    for (size_t autoManageIndex = 0; autoManageIndex < 2; ++autoManageIndex) {
        bool autoManage = AUTO_MANAGE[autoManageIndex];

        ntccfg::TestAllocator ta;
        {
            test::case1::execute(autoManage, &ta);
        }
        NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
    }
}

namespace test {
namespace case2 {

/// Provide an implementation of the 'ntci::TimerSession'
/// protocol for use by this test driver.
class TimerSession : public ntci::TimerSession
{
    bsl::string  d_name;
    bslmt::Latch d_deadline;
    bslmt::Latch d_cancelled;
    bslmt::Latch d_closed;

  private:
    /// Process the arrival of the timer's deadline.
    void processTimerDeadline(const bsl::shared_ptr<ntci::Timer>& timer,
                              const ntca::TimerEvent&             event)
        BSLS_KEYWORD_OVERRIDE;

    /// Process the cancellation of the timer.
    void processTimerCancelled(const bsl::shared_ptr<ntci::Timer>& timer,
                               const ntca::TimerEvent&             event)
        BSLS_KEYWORD_OVERRIDE;

    /// Process the removal of the timer.
    void processTimerClosed(const bsl::shared_ptr<ntci::Timer>& timer,
                            const ntca::TimerEvent&             event)
        BSLS_KEYWORD_OVERRIDE;

    /// Return the strand on which this object's functions should be called.
    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;

  private:
    TimerSession(const TimerSession&) BSLS_KEYWORD_DELETED;
    TimerSession& operator=(const TimerSession&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new timer session having the specified 'name'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit TimerSession(const bsl::string& name,
                          bslma::Allocator*  basicAllocator = 0);

    /// Destroy this object.
    ~TimerSession() BSLS_KEYWORD_OVERRIDE;

    /// Wait until the occurrence of the specified 'timerEventType'.
    void wait(ntca::TimerEventType::Value timerEventType);

    /// Try to wait until the occurrence of the specified 'timerEventType'.
    /// Return true if the wait is successful and the 'timerEventType' has
    /// occurred, otherwise return false.
    bool tryWait(ntca::TimerEventType::Value timerEventType);

    /// Return true if the specified 'timerEventType' has occurred,
    /// otherwise return false.
    bool has(ntca::TimerEventType::Value timerEventType);

    /// Return the number of occurrences of the specified 'timerEventType'.
    bsl::size_t count(ntca::TimerEventType::Value timerEventType);
};

void TimerSession::processTimerDeadline(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_TEST_EQ(event.type(), ntca::TimerEventType::e_DEADLINE);

    NTCCFG_TEST_LOG_INFO << "Timer '" << d_name << "' deadline: " << event
                         << " drifted "
                         << ntcd::DataUtil::formatMicroseconds(
                                event.context().drift().totalMicroseconds())
                         << NTCCFG_TEST_LOG_END;

    d_deadline.arrive();
}

void TimerSession::processTimerCancelled(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_TEST_EQ(event.type(), ntca::TimerEventType::e_CANCELED);

    NTCCFG_TEST_LOG_DEBUG << "Timer '" << d_name << "' cancelled"
                          << NTCCFG_TEST_LOG_END;

    d_cancelled.arrive();
}

void TimerSession::processTimerClosed(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_TEST_EQ(event.type(), ntca::TimerEventType::e_CLOSED);

    NTCCFG_TEST_LOG_DEBUG << "Timer '" << d_name << "' closed"
                          << NTCCFG_TEST_LOG_END;

    d_closed.arrive();
}

const bsl::shared_ptr<ntci::Strand>& TimerSession::strand() const
{
    return ntci::Strand::unspecified();
}

TimerSession::TimerSession(const bsl::string& name,
                           bslma::Allocator*  basicAllocator)
: d_name(name, basicAllocator)
, d_deadline(1)
, d_cancelled(1)
, d_closed(1)
{
}

TimerSession::~TimerSession()
{
}

void TimerSession::wait(ntca::TimerEventType::Value timerEventType)
{
    if (timerEventType == ntca::TimerEventType::e_DEADLINE) {
        d_deadline.wait();
    }
    else if (timerEventType == ntca::TimerEventType::e_CANCELED) {
        d_cancelled.wait();
    }
    else if (timerEventType == ntca::TimerEventType::e_CLOSED) {
        d_closed.wait();
    }
    else {
        NTCCFG_TEST_TRUE(false);
    }
}

bool TimerSession::tryWait(ntca::TimerEventType::Value timerEventType)
{
    if (timerEventType == ntca::TimerEventType::e_DEADLINE) {
        return d_deadline.tryWait();
    }
    else if (timerEventType == ntca::TimerEventType::e_CANCELED) {
        return d_cancelled.tryWait();
    }
    else if (timerEventType == ntca::TimerEventType::e_CLOSED) {
        return d_closed.tryWait();
    }
    else {
        NTCCFG_TEST_TRUE(false);
        return false;
    }
}

bool TimerSession::has(ntca::TimerEventType::Value timerEventType)
{
    if (timerEventType == ntca::TimerEventType::e_DEADLINE) {
        return d_deadline.currentCount() != 1;
    }
    else if (timerEventType == ntca::TimerEventType::e_CANCELED) {
        return d_cancelled.currentCount() != 1;
    }
    else if (timerEventType == ntca::TimerEventType::e_CLOSED) {
        return d_closed.currentCount() != 1;
    }
    else {
        NTCCFG_TEST_TRUE(false);
        return 0;
    }
}

bsl::size_t TimerSession::count(ntca::TimerEventType::Value timerEventType)
{
    if (timerEventType == ntca::TimerEventType::e_DEADLINE) {
        return 1 - d_deadline.currentCount();
    }
    else if (timerEventType == ntca::TimerEventType::e_CANCELED) {
        return 1 - d_cancelled.currentCount();
    }
    else if (timerEventType == ntca::TimerEventType::e_CLOSED) {
        return 1 - d_closed.currentCount();
    }
    else {
        NTCCFG_TEST_TRUE(false);
        return 0;
    }
}

void execute(bsl::size_t       maskInterestCase,
             bsl::size_t       oneShotCase,
             bslma::Allocator* allocator)
{
    ntca::TimerOptions timerOptions;

    switch (maskInterestCase) {
    case 0:
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
        timerOptions.showEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);
        break;
    case 1:
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
        timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);
        break;
    case 2:
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
        timerOptions.showEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
        break;
    case 3:
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
        timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
        break;
    }

    if (oneShotCase == 0) {
        timerOptions.setOneShot(false);
    }
    else {
        timerOptions.setOneShot(true);
    }

    NTCCFG_TEST_LOG_INFO << "Testing timer options = " << timerOptions
                         << NTCCFG_TEST_LOG_END;

    ntsa::Error error;

    // Create the simulation.

    bsl::shared_ptr<ntcd::Simulation> simulation;
    simulation.createInplace(allocator, allocator);

    error = simulation->run();
    NTCCFG_TEST_OK(error);

    // Create the user.

    bsl::shared_ptr<ntci::User> user;

    // Create the reactor.

    ntca::ReactorConfig reactorConfig;

    reactorConfig.setMetricName("test");
    reactorConfig.setMinThreads(1);
    reactorConfig.setMaxThreads(1);

    bsl::shared_ptr<ntcd::Reactor> reactor =
        simulation->createReactor(reactorConfig, user, allocator);

    // Register this thread as a thread that will wait on the reactor.

    ntci::Waiter waiter = reactor->registerWaiter(ntca::WaiterOptions());

    // Register three timers to fire at t1, t2, and t3. Implement the
    // first timer so that when it fires at t1, it cancels the timer
    // at t2.

    bsl::shared_ptr<test::case2::TimerSession> timerSession1;
    timerSession1.createInplace(allocator, "timer1", allocator);

    bsl::shared_ptr<test::case2::TimerSession> timerSession2;
    timerSession2.createInplace(allocator, "timer2", allocator);

    bsl::shared_ptr<test::case2::TimerSession> timerSession3;
    timerSession3.createInplace(allocator, "timer3", allocator);

    bsl::shared_ptr<ntci::Timer> timer1 = reactor->createTimer(
        timerOptions,
        static_cast<bsl::shared_ptr<ntci::TimerSession> >(timerSession1),
        allocator);

    bsl::shared_ptr<ntci::Timer> timer2 = reactor->createTimer(
        timerOptions,
        static_cast<bsl::shared_ptr<ntci::TimerSession> >(timerSession2),
        allocator);

    bsl::shared_ptr<ntci::Timer> timer3 = reactor->createTimer(
        timerOptions,
        static_cast<bsl::shared_ptr<ntci::TimerSession> >(timerSession3),
        allocator);

    bsls::TimeInterval now = bdlt::CurrentTime::now();

    timer1->schedule(now + bsls::TimeInterval(1));
    timer2->schedule(now + bsls::TimeInterval(2));
    timer3->schedule(now + bsls::TimeInterval(3));

    // Wait for the first timer at t1 to fire.

    NTCCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));

    while (!timerSession1->tryWait(ntca::TimerEventType::e_DEADLINE)) {
        reactor->poll(waiter);
    }

    if (!timerOptions.oneShot()) {
        timer1->close();
    }

    // Cancel the timer at t2.

    timer2->close();

    // Wait for the timer at t1 to be closed.

    if (timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED)) {
        while (!timerSession1->tryWait(ntca::TimerEventType::e_CLOSED)) {
            reactor->poll(waiter);
        }
    }

    // Wait for the timer at t2 to be cancelled.

    if (timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED)) {
        while (!timerSession2->tryWait(ntca::TimerEventType::e_CANCELED)) {
            reactor->poll(waiter);
        }
    }

    // Wait for the timer at t2 to be closed.

    if (timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED)) {
        while (!timerSession2->tryWait(ntca::TimerEventType::e_CLOSED)) {
            reactor->poll(waiter);
        }
    }

    // Wait for the timer at t3 to fire.

    NTCCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));

    while (!timerSession3->tryWait(ntca::TimerEventType::e_DEADLINE)) {
        reactor->poll(waiter);
    }

    if (!timerOptions.oneShot()) {
        timer3->close();
    }

    // Wait for the timer at t3 to be closed.

    if (timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED)) {
        while (!timerSession3->tryWait(ntca::TimerEventType::e_CLOSED)) {
            reactor->poll(waiter);
        }
    }

    // Ensure the timer at t1 fired and was not cancelled.

    NTCCFG_TEST_EQ(timerSession1->count(ntca::TimerEventType::e_DEADLINE), 1);
    if (timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED)) {
        NTCCFG_TEST_EQ(timerSession1->count(ntca::TimerEventType::e_CANCELED),
                       0);
    }

    // Ensure the timer at t2 did not fire, because it was cancelled
    // by the timer at t1.

    NTCCFG_TEST_EQ(timerSession2->count(ntca::TimerEventType::e_DEADLINE), 0);
    if (timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED)) {
        NTCCFG_TEST_EQ(timerSession2->count(ntca::TimerEventType::e_CANCELED),
                       1);
    }

    // Ensure the timer at t3 fired and was not canceled.

    NTCCFG_TEST_EQ(timerSession3->count(ntca::TimerEventType::e_DEADLINE), 1);
    if (timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED)) {
        NTCCFG_TEST_EQ(timerSession3->count(ntca::TimerEventType::e_CANCELED),
                       0);
    }

    // Ensure all three timers are closed.

    if (timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED)) {
        NTCCFG_TEST_EQ(timerSession1->count(ntca::TimerEventType::e_CLOSED),
                       1);
        NTCCFG_TEST_EQ(timerSession2->count(ntca::TimerEventType::e_CLOSED),
                       1);
        NTCCFG_TEST_EQ(timerSession3->count(ntca::TimerEventType::e_CLOSED),
                       1);
    }

    // Deregister the waiter.

    reactor->deregisterWaiter(waiter);

    // Stop the simulation.

    simulation->stop();
}

}  // close namespace case2
}  // close namespace test

NTCCFG_TEST_CASE(2)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    for (bsl::size_t maskInterestCase = 0; maskInterestCase < 4;
         ++maskInterestCase)
    {
        for (bsl::size_t oneShotCase = 0; oneShotCase < 2; ++oneShotCase) {
            ntccfg::TestAllocator ta;
            {
                test::case2::execute(maskInterestCase, oneShotCase, &ta);
            }
            NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
        }
    }
}

namespace test {
namespace case3 {

void processFunction(bslmt::Latch* latch)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Executed function");

    latch->arrive();
}

void execute(bslma::Allocator* allocator)
{
    ntsa::Error error;

    // Create the simulation.

    bsl::shared_ptr<ntcd::Simulation> simulation;
    simulation.createInplace(allocator, allocator);

    error = simulation->run();
    NTCCFG_TEST_OK(error);

    // Create the user.

    bsl::shared_ptr<ntci::User> user;

    // Create the reactor.

    ntca::ReactorConfig reactorConfig;

    reactorConfig.setMetricName("test");
    reactorConfig.setMinThreads(1);
    reactorConfig.setMaxThreads(1);

    bsl::shared_ptr<ntcd::Reactor> reactor =
        simulation->createReactor(reactorConfig, user, allocator);

    // Register this thread as a thread that will wait on the reactor.

    ntci::Waiter waiter = reactor->registerWaiter(ntca::WaiterOptions());

    // Defer a function to execute.

    bslmt::Latch latch(1);
    reactor->execute(NTCCFG_BIND(&processFunction, &latch));

    while (!latch.tryWait()) {
        reactor->poll(waiter);
    }

    // Deregister the waiter.

    reactor->deregisterWaiter(waiter);

    // Stop the simulation.

    simulation->stop();
}

}  // close namespace case3
}  // close namespace test

NTCCFG_TEST_CASE(3)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    ntccfg::TestAllocator ta;
    {
        test::case3::execute(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
}
NTCCFG_TEST_DRIVER_END;
