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
BSLS_IDENT_RCSID(ntcf_concurrent_t_cpp, "$Id$ $CSID$")

#include <ntcf_concurrent.h>

using namespace BloombergLP;

#if NTC_BUILD_WITH_COROUTINES

#define NTCF_CONCURRENT_TEST_LOG_CLIENT_COROUTINE(socket, milestone)          \
    do {                                                                      \
        BALL_LOG_INFO << "Client socket at " << (socket)->sourceEndpoint()    \
                      << " to " << (socket)->remoteEndpoint()                 \
                      << " coroutine " << (milestone) << BALL_LOG_END;        \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_CLIENT_CONNECT_COMPLETE(socket, result)      \
    do {                                                                      \
        BALL_LOG_INFO << "Client socket at " << (socket)->sourceEndpoint()    \
                      << " to " << (socket)->remoteEndpoint()                 \
                      << " connect complete: " << (result).event()            \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_CLIENT_SEND_COMPLETE(socket, result, data)   \
    do {                                                                      \
        BALL_LOG_INFO << "Client socket at " << (socket)->sourceEndpoint()    \
                      << " to " << (socket)->remoteEndpoint()                 \
                      << " TX complete: "                                     \
                      << ConcurrentTest::Dumper((data).get())                 \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_CLIENT_RECEIVE_COMPLETE(socket, result)      \
    do {                                                                      \
        if (!(result).event().context().error()) {                            \
            BALL_LOG_INFO << "Client socket at "                              \
                          << (socket)->sourceEndpoint() << " to "             \
                          << (socket)->remoteEndpoint() << " RX complete: "   \
                          << ConcurrentTest::Dumper((result).data().get())    \
                          << BALL_LOG_END;                                    \
        }                                                                     \
        else {                                                                \
            BALL_LOG_INFO << "Client socket at "                              \
                          << (socket)->sourceEndpoint() << " to "             \
                          << (socket)->remoteEndpoint()                       \
                          << " RX complete: " << (result).event().context()   \
                          << BALL_LOG_END;                                    \
        }                                                                     \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_CLIENT_CLOSED(socket)                        \
    do {                                                                      \
        BALL_LOG_INFO << "Client socket at " << (socket)->sourceEndpoint()    \
                      << " to " << (socket)->remoteEndpoint() << " closed"    \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_SERVER_COROUTINE(socket, milestone)          \
    do {                                                                      \
        BALL_LOG_INFO << "Server socket at " << (socket)->sourceEndpoint()    \
                      << " to " << (socket)->remoteEndpoint()                 \
                      << " coroutine " << (milestone) << BALL_LOG_END;        \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_SERVER_CONNECT_COMPLETE(socket, result)      \
    do {                                                                      \
        BALL_LOG_INFO << "Server socket at " << (socket)->sourceEndpoint()    \
                      << " to " << (socket)->remoteEndpoint()                 \
                      << " connect complete: " << (result).event()            \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_SERVER_ACCEPT_COMPLETE(socket, result)       \
    do {                                                                      \
        BALL_LOG_INFO << "Server socket at " << (socket)->sourceEndpoint()    \
                      << " to " << (socket)->remoteEndpoint()                 \
                      << " accept complete: " << (result).event()             \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_SERVER_SEND_COMPLETE(socket, result, data)   \
    do {                                                                      \
        BALL_LOG_INFO << "Server socket at " << (socket)->sourceEndpoint()    \
                      << " to " << (socket)->remoteEndpoint()                 \
                      << " TX complete: "                                     \
                      << ConcurrentTest::Dumper((data).get())                 \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_SERVER_RECEIVE_COMPLETE(socket, result)      \
    do {                                                                      \
        if (!(result).event().context().error()) {                            \
            BALL_LOG_INFO << "Server socket at "                              \
                          << (socket)->sourceEndpoint() << " to "             \
                          << (socket)->remoteEndpoint() << " RX complete: "   \
                          << ConcurrentTest::Dumper((result).data().get())    \
                          << BALL_LOG_END;                                    \
        }                                                                     \
        else {                                                                \
            BALL_LOG_INFO << "Server socket at "                              \
                          << (socket)->sourceEndpoint() << " to "             \
                          << (socket)->remoteEndpoint()                       \
                          << " RX complete: " << (result).event().context()   \
                          << BALL_LOG_END;                                    \
        }                                                                     \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_SERVER_CLOSED(socket)                        \
    do {                                                                      \
        BALL_LOG_INFO << "Server socket at " << (socket)->sourceEndpoint()    \
                      << " to " << (socket)->remoteEndpoint() << " closed"    \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_LISTENER_BIND_COMPLETE(socket, result)       \
    do {                                                                      \
        BALL_LOG_INFO << "Listener socket at " << (socket)->sourceEndpoint()  \
                      << " bind complete: " << (result).event()               \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_LISTENER_READY(endpoint)                     \
    do {                                                                      \
        BALL_LOG_DEBUG << "Listening at " << (endpoint) << BALL_LOG_END;      \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_LISTENER_CLOSED()                            \
    do {                                                                      \
        BALL_LOG_INFO << "Listener socket closed" << BALL_LOG_END;            \
    } while (false)

namespace BloombergLP {
namespace ntcf {

// Provide tests for 'ntcf::Concurrent'.
class ConcurrentTest
{
    BALL_LOG_SET_CLASS_CATEGORY("NTCF.CONCURRENT");

    // Describe the configurable parameters of the application simulation.
    class Configuration
    {
      public:
        Configuration()
        : numConnections(1)
        , numMessages(1)
        , endpoint()
        {
        }

        bsl::size_t    numConnections;
        bsl::size_t    numMessages;
        ntsa::Endpoint endpoint;
    };

    class Dumper
    {
        bdlbb::Blob* d_blob;

      public:
        explicit Dumper(bdlbb::Blob* blob)
        : d_blob(blob)
        {
        }

        friend bsl::ostream& operator<<(bsl::ostream& stream,
                                        const Dumper& dumper)
        {
            NTSCFG_TEST_EQ(dumper.d_blob->length(), 1);
            stream << static_cast<char>(
                *dumper.d_blob->buffer(0).buffer().get());
            return stream;
        }
    };

    // Create a new scheduler. Allocate memory using the specified 'allocator'.
    static bsl::shared_ptr<ntci::Scheduler> createScheduler(
        ntsa::Allocator allocator);

    // Create a new scheduler with the specified 'configuration'. Allocate
    // memory using the specified 'allocator'.
    static bsl::shared_ptr<ntci::Scheduler> createScheduler(
        const ntca::SchedulerConfig& configuration,
        ntsa::Allocator              allocator);

    // Create a new listener socket bound to any available ephemeral port at
    // the loopback address and begin listening. Allocate memory using the
    // specified 'allocator'.
    static bsl::shared_ptr<ntci::ListenerSocket> createListenerSocket(
        const bsl::shared_ptr<ntci::Scheduler>& scheduler,
        ntsa::Allocator                         allocator);

    // Create a new stream socket. Allocate memory using the specified
    // 'allocator'.
    static bsl::shared_ptr<ntci::StreamSocket> createStreamSocket(
        const bsl::shared_ptr<ntci::Scheduler>& scheduler,
        ntsa::Allocator                         allocator);

    // TODO
    static ntsa::Task<void> coVerifyExecute(
        bsl::shared_ptr<ntci::Scheduler> scheduler,
        ntsa::AllocatorArg               allocatorTag,
        ntsa::Allocator                  allocator);

    // TODO
    static ntsa::Task<void> coVerifyDatagramSocket(
        bsl::shared_ptr<ntci::Scheduler> scheduler,
        ntsa::AllocatorArg               allocatorTag,
        ntsa::Allocator                  allocator);

    // TODO
    static ntsa::Task<void> coVerifyStreamSocket(
        bsl::shared_ptr<ntci::Scheduler> scheduler,
        ntsa::AllocatorArg               allocatorTag,
        ntsa::Allocator                  allocator);

    // TODO
    static ntsa::Task<void> coVerifyApplication(
        bsl::shared_ptr<ntci::Scheduler> scheduler,
        ntsa::AllocatorArg               allocatorTag,
        ntsa::Allocator                  allocator);

    // TODO
    static ntsa::Task<void> coVerifyApplicationListener(
        Configuration                         configuration,
        bsl::shared_ptr<ntci::ListenerSocket> listenerSocket,
        ntsa::AllocatorArg                    allocatorTag,
        ntsa::Allocator                       allocator);

    // TODO
    static ntsa::Task<void> coVerifyApplicationClient(
        Configuration                       configuration,
        bsl::shared_ptr<ntci::StreamSocket> streamSocket,
        bsl::size_t                         index,
        ntsa::AllocatorArg                  allocatorTag,
        ntsa::Allocator                     allocator);

    // TODO
    static ntsa::Task<void> coVerifyApplicationServer(
        Configuration                       configuration,
        bsl::shared_ptr<ntci::StreamSocket> streamSocket,
        ntsa::AllocatorArg                  allocatorTag,
        ntsa::Allocator                     allocator);

  public:
    // Verify a coroutine can be resume on an scheduler's I/O thread.
    static void verifyExecute();

    // Verify datagram socket awaitables.
    static void verifyDatagramSocket();

    // Verify stream socket awaitables.
    static void verifyStreamSocket();

    // Verify a sample application.
    static void verifyApplication();
};

bsl::shared_ptr<ntci::Scheduler> ConcurrentTest::createScheduler(
    ntsa::Allocator allocator)
{
    ntsa::Error error;

    ntca::SchedulerConfig schedulerConfig;

    schedulerConfig.setThreadName("test");
    schedulerConfig.setMinThreads(4);
    schedulerConfig.setMaxThreads(4);

    bsl::shared_ptr<ntci::Scheduler> scheduler =
        ntcf::System::createScheduler(schedulerConfig, allocator.mechanism());

    error = scheduler->start();
    NTSCFG_TEST_OK(error);

    return scheduler;
}

bsl::shared_ptr<ntci::Scheduler> ConcurrentTest::createScheduler(
    const ntca::SchedulerConfig& configuration,
    ntsa::Allocator              allocator)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::Scheduler> scheduler =
        ntcf::System::createScheduler(configuration, allocator.mechanism());

    error = scheduler->start();
    NTSCFG_TEST_OK(error);

    return scheduler;
}

bsl::shared_ptr<ntci::ListenerSocket> ConcurrentTest::createListenerSocket(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    ntsa::Allocator                         allocator)
{
    ntsa::Error error;

    ntca::ListenerSocketOptions listenerSocketOptions;

    listenerSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
    listenerSocketOptions.setKeepHalfOpen(true);

    listenerSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)));

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        scheduler->createListenerSocket(listenerSocketOptions,
                                        allocator.mechanism());

    error = listenerSocket->open();
    NTSCFG_TEST_OK(error);

    error = listenerSocket->listen();
    NTSCFG_TEST_OK(error);

    return listenerSocket;
}

bsl::shared_ptr<ntci::StreamSocket> ConcurrentTest::createStreamSocket(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    ntsa::Allocator                         allocator)
{
    ntsa::Error error;

    ntca::StreamSocketOptions streamSocketOptions;

    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
    streamSocketOptions.setKeepHalfOpen(true);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions,
                                      allocator.mechanism());

    return streamSocket;
}

ntsa::Task<void> ConcurrentTest::coVerifyExecute(
    bsl::shared_ptr<ntci::Scheduler> scheduler,
    ntsa::AllocatorArg               allocatorTag,
    ntsa::Allocator                  allocator)
{
    ntccfg::Object scope("coVerifyExecute");

    BALL_LOG_DEBUG << "Starting on thread "
                   << bslmt::ThreadUtil::selfIdAsUint64() << BALL_LOG_END;

    for (bsl::size_t i = 0; i < 8; ++i) {
        co_await ntcf::Concurrent::resume(scheduler);

        BALL_LOG_DEBUG << "Resuming on thread "
                       << bslmt::ThreadUtil::selfIdAsUint64() << BALL_LOG_END;
    }

    co_return;
}

ntsa::Task<void> ConcurrentTest::coVerifyDatagramSocket(
    bsl::shared_ptr<ntci::Scheduler> scheduler,
    ntsa::AllocatorArg               allocatorTag,
    ntsa::Allocator                  allocator)
{
    ntccfg::Object scope("coVerifyDatagramSocket");

    ntsa::Error error;

    // Create a client datagram socket.

    ntca::DatagramSocketOptions clientDatagramSocketOptions;

    clientDatagramSocketOptions.setTransport(
        ntsa::Transport::e_UDP_IPV4_DATAGRAM);

    clientDatagramSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)));

    bsl::shared_ptr<ntci::DatagramSocket> clientDatagramSocket =
        scheduler->createDatagramSocket(clientDatagramSocketOptions,
                                        allocator.mechanism());

    error = clientDatagramSocket->open();
    NTSCFG_TEST_OK(error);

    // Create a server datagram socket.

    ntca::DatagramSocketOptions serverDatagramSocketOptions;

    serverDatagramSocketOptions.setTransport(
        ntsa::Transport::e_UDP_IPV4_DATAGRAM);

    serverDatagramSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)));

    bsl::shared_ptr<ntci::DatagramSocket> serverDatagramSocket =
        scheduler->createDatagramSocket(serverDatagramSocketOptions,
                                        allocator.mechanism());

    error = serverDatagramSocket->open();
    NTSCFG_TEST_OK(error);

    // Connect the client datagram socket to the server datagram socket.

    ntca::ConnectOptions clientConnectOptions;
    ntci::ConnectResult  clientConnectResult;

    clientConnectResult = co_await ntcf::Concurrent::connect(
        clientDatagramSocket,
        serverDatagramSocket->sourceEndpoint(),
        clientConnectOptions);

    NTSCFG_TEST_OK(clientConnectResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_CLIENT_CONNECT_COMPLETE(clientDatagramSocket,
                                                     clientConnectResult);

    // Connect the server datagram socket to the client datagram socket.

    ntca::ConnectOptions serverConnectOptions;
    ntci::ConnectResult  serverConnectResult;

    serverConnectResult = co_await ntcf::Concurrent::connect(
        serverDatagramSocket,
        clientDatagramSocket->sourceEndpoint(),
        ntca::ConnectOptions());
    NTSCFG_TEST_OK(serverConnectResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_SERVER_CONNECT_COMPLETE(serverDatagramSocket,
                                                     serverConnectResult);

    // Send data from the client datagram socket to the server datagram socket.

    bsl::shared_ptr<bdlbb::Blob> clientSendData =
        clientDatagramSocket->createOutgoingBlob();

    bdlbb::BlobUtil::append(clientSendData.get(), "X", 1);

    ntca::SendOptions clientSendOptions;
    ntci::SendResult  clientSendResult;

    clientSendResult = co_await ntcf::Concurrent::send(clientDatagramSocket,
                                                       clientSendData,
                                                       clientSendOptions);
    NTSCFG_TEST_OK(clientSendResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_CLIENT_SEND_COMPLETE(clientDatagramSocket,
                                                  clientSendResult,
                                                  clientSendData);

    // Receive data at the server datagram socket from the client datagram
    // socket.

    ntca::ReceiveOptions serverReceiveOptions;
    ntci::ReceiveResult  serverReceiveResult;

    serverReceiveResult =
        co_await ntcf::Concurrent::receive(serverDatagramSocket,
                                           serverReceiveOptions);
    NTSCFG_TEST_OK(serverReceiveResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_SERVER_RECEIVE_COMPLETE(serverDatagramSocket,
                                                     serverReceiveResult);

    // Close the client datagram socket.

    co_await ntcf::Concurrent::close(clientDatagramSocket);
    NTCF_CONCURRENT_TEST_LOG_CLIENT_CLOSED(clientDatagramSocket);

    // Close the server datagram socket.

    co_await ntcf::Concurrent::close(serverDatagramSocket);
    NTCF_CONCURRENT_TEST_LOG_SERVER_CLOSED(serverDatagramSocket);

    co_return;
}

ntsa::Task<void> ConcurrentTest::coVerifyStreamSocket(
    bsl::shared_ptr<ntci::Scheduler> scheduler,
    ntsa::AllocatorArg               allocatorTag,
    ntsa::Allocator                  allocator)
{
    ntccfg::Object scope("coVerifyStreamSocket");

    ntsa::Error error;

    // Create a listener socket.

    ntca::ListenerSocketOptions listenerSocketOptions;

    listenerSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
    listenerSocketOptions.setKeepHalfOpen(true);

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        scheduler->createListenerSocket(listenerSocketOptions,
                                        allocator.mechanism());

    error = listenerSocket->open();
    NTSCFG_TEST_OK(error);

    // Bind the listener socket to any available ephemeral port on the
    // loopback device.

    ntca::BindOptions bindOptions;
    ntci::BindResult  bindResult;

    bindResult = co_await ntcf::Concurrent::bind(
        listenerSocket,
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        bindOptions);

    NTCF_CONCURRENT_TEST_LOG_LISTENER_BIND_COMPLETE(listenerSocket,
                                                    bindResult);

    NTSCFG_TEST_OK(bindResult.event().context().error());

    // Begin listening.

    error = listenerSocket->listen();
    NTSCFG_TEST_OK(error);

    // Create a client stream socket.

    ntca::StreamSocketOptions clientStreamSocketOptions;

    clientStreamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
    clientStreamSocketOptions.setKeepHalfOpen(true);

    bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket =
        scheduler->createStreamSocket(clientStreamSocketOptions,
                                      allocator.mechanism());

    // Connect the client stream socket to the listener socket.

    ntca::ConnectOptions clientConnectOptions;
    ntci::ConnectResult  clientConnectResult;

    clientConnectResult =
        co_await ntcf::Concurrent::connect(clientStreamSocket,
                                           listenerSocket->sourceEndpoint(),
                                           clientConnectOptions);

    NTCF_CONCURRENT_TEST_LOG_CLIENT_CONNECT_COMPLETE(clientStreamSocket,
                                                     clientConnectResult);

    NTSCFG_TEST_OK(clientConnectResult.event().context().error());

    // Accept the server stream socket from the listener socket.

    ntca::AcceptOptions serverAcceptOptions;
    ntci::AcceptResult  serverAcceptResult;

    serverAcceptResult =
        co_await ntcf::Concurrent::accept(listenerSocket, serverAcceptOptions);

    bsl::shared_ptr<ntci::StreamSocket> serverStreamSocket =
        serverAcceptResult.streamSocket();

    NTCF_CONCURRENT_TEST_LOG_SERVER_ACCEPT_COMPLETE(serverStreamSocket,
                                                    serverAcceptResult);

    NTSCFG_TEST_OK(serverAcceptResult.event().context().error());

    // Send data from the client stream socket to the server stream socket.

    bsl::shared_ptr<bdlbb::Blob> clientSendData =
        clientStreamSocket->createOutgoingBlob();

    bdlbb::BlobUtil::append(clientSendData.get(), "X", 1);

    ntca::SendOptions clientSendOptions;
    ntci::SendResult  clientSendResult;

    clientSendResult = co_await ntcf::Concurrent::send(clientStreamSocket,
                                                       clientSendData,
                                                       clientSendOptions);

    NTCF_CONCURRENT_TEST_LOG_CLIENT_SEND_COMPLETE(clientStreamSocket,
                                                  clientSendResult,
                                                  clientSendData);

    NTSCFG_TEST_OK(clientSendResult.event().context().error());

    // Receive data at the server stream socket from the client stream
    // socket.

    ntca::ReceiveOptions serverReceiveOptions;
    ntci::ReceiveResult  serverReceiveResult;

    serverReceiveOptions.setSize(1);

    serverReceiveResult =
        co_await ntcf::Concurrent::receive(serverStreamSocket,
                                           serverReceiveOptions);

    NTCF_CONCURRENT_TEST_LOG_SERVER_RECEIVE_COMPLETE(serverStreamSocket,
                                                     serverReceiveResult);

    NTSCFG_TEST_OK(serverReceiveResult.event().context().error());

    // Send data from the server stream socket to the client stream socket.

    bsl::shared_ptr<bdlbb::Blob> serverSendData = serverReceiveResult.data();

    ntca::SendOptions serverSendOptions;
    ntci::SendResult  serverSendResult;

    serverSendResult = co_await ntcf::Concurrent::send(serverStreamSocket,
                                                       serverSendData,
                                                       serverSendOptions);

    NTCF_CONCURRENT_TEST_LOG_SERVER_SEND_COMPLETE(serverStreamSocket,
                                                  servertSendResult,
                                                  serverSendData);

    NTSCFG_TEST_OK(serverSendResult.event().context().error());

    // Receive data at the client stream socket from the server stream socket.

    ntca::ReceiveOptions clientReceiveOptions;
    ntci::ReceiveResult  clientReceiveResult;

    clientReceiveOptions.setSize(1);

    clientReceiveResult =
        co_await ntcf::Concurrent::receive(clientStreamSocket,
                                           clientReceiveOptions);

    NTCF_CONCURRENT_TEST_LOG_CLIENT_RECEIVE_COMPLETE(clientStreamSocket,
                                                     clientReceiveResult);

    NTSCFG_TEST_OK(clientReceiveResult.event().context().error());

    // Shutdown transmission from the client socket to initiate the graceful
    // shutdown of the connection.

    error = clientStreamSocket->shutdown(ntsa::ShutdownType::e_SEND,
                                         ntsa::ShutdownMode::e_GRACEFUL);
    NTSCFG_TEST_OK(error);

    // Receive data at the server stream socket and notice the client stream
    // socket has shut down the connection.

    serverReceiveResult =
        co_await ntcf::Concurrent::receive(serverStreamSocket,
                                           serverReceiveOptions);

    NTCF_CONCURRENT_TEST_LOG_SERVER_RECEIVE_COMPLETE(serverStreamSocket,
                                                     serverReceiveResult);

    NTSCFG_TEST_EQ(serverReceiveResult.event().context().error(),
                   ntsa::Error(ntsa::Error::e_EOF));

    // Shutdown transmission from the server socket to complete the
    // graceful shutdown of the connection.

    error = serverStreamSocket->shutdown(ntsa::ShutdownType::e_SEND,
                                         ntsa::ShutdownMode::e_GRACEFUL);
    NTSCFG_TEST_OK(error);

    // Receive data at the client stream socket and notice the server stream
    // socket has shut down the connection.

    clientReceiveResult =
        co_await ntcf::Concurrent::receive(clientStreamSocket,
                                           clientReceiveOptions);

    NTCF_CONCURRENT_TEST_LOG_CLIENT_RECEIVE_COMPLETE(clientStreamSocket,
                                                     clientReceiveResult);

    NTSCFG_TEST_EQ(clientReceiveResult.event().context().error(),
                   ntsa::Error(ntsa::Error::e_EOF));

    // Close the client stream socket.

    co_await ntcf::Concurrent::close(clientStreamSocket);
    NTCF_CONCURRENT_TEST_LOG_CLIENT_CLOSED(clientStreamSocket);

    // Close the server stream socket.

    co_await ntcf::Concurrent::close(serverStreamSocket);
    NTCF_CONCURRENT_TEST_LOG_SERVER_CLOSED(serverStreamSocket);

    // Close the listener socket.

    co_await ntcf::Concurrent::close(listenerSocket);
    NTCF_CONCURRENT_TEST_LOG_LISTENER_CLOSED();

    co_return;
}

ntsa::Task<void> ConcurrentTest::coVerifyApplication(
    bsl::shared_ptr<ntci::Scheduler> scheduler,
    ntsa::AllocatorArg               allocatorTag,
    ntsa::Allocator                  allocator)
{
    ntccfg::Object scope("coVerifyApplication");

    ntsa::Error error;

    bsl::vector<ntsa::Task<void> > taskList;

    // Create the listener socket and begin listening.

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        ntcf::ConcurrentTest::createListenerSocket(scheduler, allocator);

    Configuration configuration;

    configuration.numConnections = 9;
    configuration.numMessages    = 100;
    configuration.endpoint       = listenerSocket->sourceEndpoint();

    NTCF_CONCURRENT_TEST_LOG_LISTENER_READY(configuration.endpoint);

    // Create a coroutine dedicated to the listener socket.

    ntsa::Task<void> listenerTask = coVerifyApplicationListener(configuration,
                                                                listenerSocket,
                                                                allocatorTag,
                                                                allocator);

    taskList.emplace_back(bsl::move(listenerTask));

    for (bsl::size_t i = 0; i < configuration.numConnections; ++i) {
        // Create a client stream socket.

        bsl::shared_ptr<ntci::StreamSocket> streamSocket =
            ntcf::ConcurrentTest::createStreamSocket(scheduler, allocator);

        // Create a coroutine dedicated to the client stream socket.

        ntsa::Task<void> clientTask = coVerifyApplicationClient(configuration,
                                                                streamSocket,
                                                                i + 1,
                                                                allocatorTag,
                                                                allocator);

        taskList.emplace_back(bsl::move(clientTask));
    }

    // Run all coroutines until complete.

    co_await ntsa::Coroutine::join(bsl::move(taskList));

    co_return;
}

ntsa::Task<void> ConcurrentTest::coVerifyApplicationListener(
    Configuration                         configuration,
    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket,
    ntsa::AllocatorArg                    allocatorTag,
    ntsa::Allocator                       allocator)
{
    ntccfg::Object scope("coVerifyApplicationListener");

    ntsa::Error error;

    for (bsl::size_t i = 0; i < configuration.numConnections; ++i) {
        // Accept a server stream socket from the listener socket.

        ntca::AcceptOptions acceptOptions;
        ntci::AcceptResult  acceptResult;

        acceptResult =
            co_await ntcf::Concurrent::accept(listenerSocket, acceptOptions);
        NTSCFG_TEST_OK(acceptResult.event().context().error());

        NTCF_CONCURRENT_TEST_LOG_SERVER_ACCEPT_COMPLETE(
            acceptResult.streamSocket(),
            acceptResult);

        // Create a coroutine dedicated to the server stream socket.

        ntsa::Task<void> serverTask =
            coVerifyApplicationServer(configuration,
                                      acceptResult.streamSocket(),
                                      allocatorTag,
                                      allocator);

        ntsa::Coroutine::spawn(bsl::move(serverTask), allocator);
    }

    // Close the listener socket.

    co_await ntcf::Concurrent::close(listenerSocket);
    NTCF_CONCURRENT_TEST_LOG_LISTENER_CLOSED();

    co_return;
}

ntsa::Task<void> ConcurrentTest::coVerifyApplicationClient(
    Configuration                       configuration,
    bsl::shared_ptr<ntci::StreamSocket> streamSocket,
    bsl::size_t                         index,
    ntsa::AllocatorArg                  allocatorTag,
    ntsa::Allocator                     allocator)
{
    ntccfg::Object scope("coVerifyApplicationClient");

    ntsa::Error error;

    NTCF_CONCURRENT_TEST_LOG_CLIENT_COROUTINE(streamSocket, "starting");

    // Connect the client stream socket to the listener socket.

    ntca::ConnectOptions connectOptions;
    ntci::ConnectResult  connectResult;

    connectResult = co_await ntcf::Concurrent::connect(streamSocket,
                                                       configuration.endpoint,
                                                       connectOptions);
    NTSCFG_TEST_OK(connectResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_CLIENT_CONNECT_COMPLETE(streamSocket,
                                                     connectResult);

    for (bsl::size_t tx = 0; tx < configuration.numMessages; ++tx) {
        // Send data to the peer.

        bsl::string sendText = bsl::to_string(index);
        BSLS_ASSERT_OPT(index <= 9);

        bsl::shared_ptr<bdlbb::Blob> sendData =
            streamSocket->createOutgoingBlob();

        bdlbb::BlobUtil::append(sendData.get(),
                                sendText.c_str(),
                                sendText.size());

        ntca::SendOptions sendOptions;
        ntci::SendResult  sendResult;

        sendResult = co_await ntcf::Concurrent::send(streamSocket,
                                                     sendData,
                                                     sendOptions);

        NTSCFG_TEST_OK(sendResult.event().context().error());

        NTCF_CONCURRENT_TEST_LOG_CLIENT_SEND_COMPLETE(streamSocket,
                                                      sendResult,
                                                      sendData);

        // Receive data from the peer.

        ntca::ReceiveOptions receiveOptions;
        ntci::ReceiveResult  receiveResult;

        receiveOptions.setSize(1);

        receiveResult =
            co_await ntcf::Concurrent::receive(streamSocket, receiveOptions);
        NTSCFG_TEST_OK(receiveResult.event().context().error());

        NTCF_CONCURRENT_TEST_LOG_CLIENT_RECEIVE_COMPLETE(streamSocket,
                                                         receiveResult);
    }

    // Close the socket.

    co_await ntcf::Concurrent::close(streamSocket);
    NTCF_CONCURRENT_TEST_LOG_CLIENT_CLOSED(streamSocket);

    NTCF_CONCURRENT_TEST_LOG_CLIENT_COROUTINE(streamSocket, "complete");

    co_return;
}

ntsa::Task<void> ConcurrentTest::coVerifyApplicationServer(
    Configuration                       configuration,
    bsl::shared_ptr<ntci::StreamSocket> streamSocket,
    ntsa::AllocatorArg                  allocatorTag,
    ntsa::Allocator                     allocator)
{
    ntccfg::Object scope("coVerifyApplicationServer");

    ntsa::Error error;

    NTCF_CONCURRENT_TEST_LOG_SERVER_COROUTINE(streamSocket, "starting");

    for (bsl::size_t tx = 0; tx < configuration.numMessages; ++tx) {
        // Receive data from the peer.

        ntca::ReceiveOptions receiveOptions;
        ntci::ReceiveResult  receiveResult;

        receiveOptions.setSize(1);

        receiveResult =
            co_await ntcf::Concurrent::receive(streamSocket, receiveOptions);
        NTSCFG_TEST_OK(receiveResult.event().context().error());

        NTCF_CONCURRENT_TEST_LOG_SERVER_RECEIVE_COMPLETE(streamSocket,
                                                         receiveResult);

        // Send data to the peer.

        bsl::shared_ptr<bdlbb::Blob> sendData = receiveResult.data();

        ntca::SendOptions sendOptions;
        ntci::SendResult  sendResult;

        sendResult = co_await ntcf::Concurrent::send(streamSocket,
                                                     sendData,
                                                     sendOptions);

        NTSCFG_TEST_OK(sendResult.event().context().error());

        NTCF_CONCURRENT_TEST_LOG_SERVER_SEND_COMPLETE(streamSocket,
                                                      sendResult,
                                                      sendData);
    }

    // Close the socket.

    co_await ntcf::Concurrent::close(streamSocket);
    NTCF_CONCURRENT_TEST_LOG_SERVER_CLOSED(streamSocket);

    NTCF_CONCURRENT_TEST_LOG_SERVER_COROUTINE(streamSocket, "complete");

    co_return;
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifyExecute)
{
    ntccfg::Object scope("verifyExecute");

    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Scheduler> scheduler =
        ntcf::ConcurrentTest::createScheduler(allocator);

    ntci::SchedulerStopGuard schedulerGuard(scheduler);

    ntsa::Task<void> task =
        coVerifyExecute(scheduler, bsl::allocator_arg, allocator);

    ntsa::Coroutine::synchronize(bsl::move(task));
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifyDatagramSocket)
{
    ntccfg::Object scope("verifyDatagramSocket");

    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Scheduler> scheduler =
        ntcf::ConcurrentTest::createScheduler(allocator);

    ntci::SchedulerStopGuard schedulerGuard(scheduler);

    ntsa::Task<void> task =
        coVerifyDatagramSocket(scheduler, bsl::allocator_arg, allocator);

    ntsa::Coroutine::synchronize(bsl::move(task));
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifyStreamSocket)
{
    ntccfg::Object scope("verifyStreamSocket");

    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Scheduler> scheduler =
        ntcf::ConcurrentTest::createScheduler(allocator);

    ntci::SchedulerStopGuard schedulerGuard(scheduler);

    ntsa::Task<void> task =
        coVerifyStreamSocket(scheduler, bsl::allocator_arg, allocator);
    ntsa::Coroutine::synchronize(bsl::move(task));
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifyApplication)
{
    ntccfg::Object scope("verifyApplication");

    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    ntca::SchedulerConfig schedulerConfig;
    schedulerConfig.setThreadName("test");
    schedulerConfig.setMinThreads(4);
    schedulerConfig.setMaxThreads(4);

    bsl::shared_ptr<ntci::Scheduler> scheduler =
        ntcf::ConcurrentTest::createScheduler(schedulerConfig, allocator);

    ntci::SchedulerStopGuard schedulerGuard(scheduler);

    ntsa::Task<void> task =
        coVerifyApplication(scheduler, bsl::allocator_arg, allocator);

    ntsa::Coroutine::synchronize(bsl::move(task));
}

}  // close namespace ntcf
}  // close namespace BloombergLP
#endif
