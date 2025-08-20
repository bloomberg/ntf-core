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
#include <ntcf_test.h>

using namespace BloombergLP;

#if NTC_BUILD_WITH_COROUTINES
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
        bsl::size_t numConnections;
    };

    // TODO
    static ntsa::CoroutineTask<void> coVerifyDatagramSocket(
        const bsl::shared_ptr<ntci::Scheduler>& scheduler,
        ntsa::Allocator                         allocator);

    // TODO
    static ntsa::CoroutineTask<void> coVerifyStreamSocket(
        const bsl::shared_ptr<ntci::Scheduler>& scheduler,
        ntsa::Allocator                         allocator);

    // TODO
    static ntsa::CoroutineTask<void> coVerifyApplication(
        const bsl::shared_ptr<ntci::Scheduler>& scheduler,
        ntsa::Allocator                         allocator);

    // TODO
    static ntsa::CoroutineTask<void> coVerifyApplicationListener(
        const Configuration&                         configuration,
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        ntsa::Allocator                              allocator);

    // TODO
    static ntsa::CoroutineTask<void> coVerifyApplicationClient(
        const Configuration&                         configuration,
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const bsl::shared_ptr<ntci::StreamSocket>&   streamSocket,
        ntsa::Allocator                              allocator);

    // TODO
    static ntsa::CoroutineTask<void> coVerifyApplicationServer(
        const Configuration&                         configuration,
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const bsl::shared_ptr<ntci::StreamSocket>&   streamSocket,
        ntsa::Allocator                              allocator);

    // TODO
    static ntsa::CoroutineTask<void> coVerifySandbox(
        const bsl::shared_ptr<ntci::Scheduler>& scheduler,
        ntsa::Allocator                         allocator);

  public:
    // TODO
    static void verifyDatagramSocket();

    // TODO
    static void verifyStreamSocket();

    // TODO
    static void verifyApplication();

    // TODO
    static void verifySandbox();
};

ntsa::CoroutineTask<void> ConcurrentTest::coVerifyDatagramSocket(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    ntsa::Allocator                         allocator)
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

    ntca::ConnectContext clientConnectContext;
    ntca::ConnectOptions clientConnectOptions;

    error = co_await ntcf::Concurrent::connect(
        clientDatagramSocket,
        &clientConnectContext,
        serverDatagramSocket->sourceEndpoint(),
        clientConnectOptions);
    NTSCFG_TEST_OK(error);

    BALL_LOG_INFO << "Client socket connect complete: " << clientConnectContext
                  << BALL_LOG_END;

    // Connect the server datagram socket to the client datagram socket.

    ntca::ConnectContext serverConnectContext;
    ntca::ConnectOptions serverConnectOptions;

    error = co_await ntcf::Concurrent::connect(
        serverDatagramSocket,
        &serverConnectContext,
        clientDatagramSocket->sourceEndpoint(),
        ntca::ConnectOptions());
    NTSCFG_TEST_OK(error);

    BALL_LOG_INFO << "Server socket connect complete: " << clientConnectContext
                  << BALL_LOG_END;

    // Send data from the client datagram socket to the server datagram socket.

    bsl::shared_ptr<bdlbb::Blob> clientSendData =
        clientDatagramSocket->createOutgoingBlob();

    bdlbb::BlobUtil::append(clientSendData.get(), "Hello, world!", 13);

    ntca::SendContext clientSendContext;
    ntca::SendOptions clientSendOptions;

    error = co_await ntcf::Concurrent::send(clientDatagramSocket,
                                            &clientSendContext,
                                            clientSendData,
                                            clientSendOptions);
    NTSCFG_TEST_OK(error);

    BALL_LOG_INFO << "Client socket send complete: " << clientSendContext
                  << "\n"
                  << bdlbb::BlobUtilHexDumper(clientSendData.get())
                  << BALL_LOG_END;

    // Receive data at the server datagram socket from the client datagram
    // socket.

    bsl::shared_ptr<bdlbb::Blob> serverReceiveData =
        serverDatagramSocket->createIncomingBlob();

    ntca::ReceiveContext serverReceiveContext;
    ntca::ReceiveOptions serverReceiveOptions;

    error = co_await ntcf::Concurrent::receive(serverDatagramSocket,
                                               &serverReceiveContext,
                                               &serverReceiveData,
                                               serverReceiveOptions);
    NTSCFG_TEST_OK(error);

    BALL_LOG_INFO << "Server socket receive complete: " << serverReceiveContext
                  << "\n"
                  << bdlbb::BlobUtilHexDumper(serverReceiveData.get())
                  << BALL_LOG_END;

    // Close the client datagram socket.

    co_await ntcf::Concurrent::close(clientDatagramSocket);

    BALL_LOG_INFO << "Client socket closed" << BALL_LOG_END;

    // Close the server datagram socket.

    co_await ntcf::Concurrent::close(serverDatagramSocket);

    BALL_LOG_INFO << "Server socket closed" << BALL_LOG_END;

    co_return;
}

ntsa::CoroutineTask<void> ConcurrentTest::coVerifyStreamSocket(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    ntsa::Allocator                         allocator)
{
    ntccfg::Object scope("coVerifyStreamSocket");

    ntsa::Error error;

    // Create the listener socket and begin listening.

    ntca::ListenerSocketOptions listenerSocketOptions;

    listenerSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    listenerSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)));

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        scheduler->createListenerSocket(listenerSocketOptions,
                                        allocator.mechanism());

    error = listenerSocket->open();
    NTSCFG_TEST_OK(error);

    error = listenerSocket->listen();
    NTSCFG_TEST_OK(error);

    // Create a client stream socket.

    ntca::StreamSocketOptions clientStreamSocketOptions;

    clientStreamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket =
        scheduler->createStreamSocket(clientStreamSocketOptions,
                                      allocator.mechanism());

    // Connect the client stream socket to the listener socket.

    ntca::ConnectContext clientConnectContext;
    ntca::ConnectOptions clientConnectOptions;

    error =
        co_await ntcf::Concurrent::connect(clientStreamSocket,
                                           &clientConnectContext,
                                           listenerSocket->sourceEndpoint(),
                                           clientConnectOptions);
    NTSCFG_TEST_OK(error);

    BALL_LOG_INFO << "Client socket connect complete: " << clientConnectContext
                  << BALL_LOG_END;

    // Accept the server stream socket from the listener socket.

    bsl::shared_ptr<ntci::StreamSocket> serverStreamSocket;

    ntca::AcceptContext serverAcceptContext;
    ntca::AcceptOptions serverAcceptOptions;

    error = co_await ntcf::Concurrent::accept(listenerSocket,
                                              &serverAcceptContext,
                                              &serverStreamSocket,
                                              serverAcceptOptions);
    NTSCFG_TEST_OK(error);

    BALL_LOG_INFO << "Server socket accept complete: " << serverAcceptContext
                  << BALL_LOG_END;

    // Send data from the client stream socket to the server stream socket.

    bsl::shared_ptr<bdlbb::Blob> clientSendData =
        clientStreamSocket->createOutgoingBlob();

    bdlbb::BlobUtil::append(clientSendData.get(), "Hello, world!", 13);

    ntca::SendContext clientSendContext;
    ntca::SendOptions clientSendOptions;

    error = co_await ntcf::Concurrent::send(clientStreamSocket,
                                            &clientSendContext,
                                            clientSendData,
                                            clientSendOptions);
    NTSCFG_TEST_OK(error);

    BALL_LOG_INFO << "Client socket send complete: " << clientSendContext
                  << "\n"
                  << bdlbb::BlobUtilHexDumper(clientSendData.get())
                  << BALL_LOG_END;

    // Receive data at the server datagram socket from the client datagram
    // socket.

    bsl::shared_ptr<bdlbb::Blob> serverReceiveData =
        serverStreamSocket->createIncomingBlob();

    ntca::ReceiveContext serverReceiveContext;
    ntca::ReceiveOptions serverReceiveOptions;

    serverReceiveOptions.setSize(13);

    error = co_await ntcf::Concurrent::receive(serverStreamSocket,
                                               &serverReceiveContext,
                                               &serverReceiveData,
                                               serverReceiveOptions);
    NTSCFG_TEST_OK(error);

    BALL_LOG_INFO << "Server socket receive complete: " << serverReceiveContext
                  << "\n"
                  << bdlbb::BlobUtilHexDumper(serverReceiveData.get())
                  << BALL_LOG_END;

    // Close the client stream socket.

    co_await ntcf::Concurrent::close(clientStreamSocket);

    BALL_LOG_INFO << "Client socket closed" << BALL_LOG_END;

    // Close the server stream socket.

    co_await ntcf::Concurrent::close(serverStreamSocket);

    BALL_LOG_INFO << "Server socket closed" << BALL_LOG_END;

    // Close the listener socket.

    co_await ntcf::Concurrent::close(listenerSocket);

    BALL_LOG_INFO << "Listener socket closed" << BALL_LOG_END;

    co_return;
}

ntsa::CoroutineTask<void> ConcurrentTest::coVerifyApplication(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    ntsa::Allocator                         allocator)
{
    ntsa::Error error;

    Configuration configuration;
    configuration.numConnections = 3;

    // Create the listener socket and begin listening.

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

    BALL_LOG_DEBUG << "Listening at " << listenerSocket->sourceEndpoint()
                   << BALL_LOG_END;

    // Enter a coroutine dedicated to the listener socket.

    co_await coVerifyApplicationListener(configuration,
                                         listenerSocket,
                                         allocator);

    // Create the client sockets.

    for (bsl::size_t i = 0; i < configuration.numConnections; ++i) {
        // Create a client stream socket.

        ntca::StreamSocketOptions streamSocketOptions;

        streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
        streamSocketOptions.setKeepHalfOpen(true);

        bsl::shared_ptr<ntci::StreamSocket> streamSocket =
            scheduler->createStreamSocket(streamSocketOptions,
                                          allocator.mechanism());

        // Enter a coroutine dedicated to the client stream socket.

        co_await coVerifyApplicationClient(configuration,
                                           listenerSocket,
                                           streamSocket,
                                           allocator);
    }

    co_return;
}

ntsa::CoroutineTask<void> ConcurrentTest::coVerifyApplicationListener(
    const Configuration&                         configuration,
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    ntsa::Allocator                              allocator)
{
    ntsa::Error error;

    for (bsl::size_t i = 0; i < configuration.numConnections; ++i) {
        // Accept a server stream socket from the listener socket.

        bsl::shared_ptr<ntci::StreamSocket> streamSocket;

        ntca::AcceptContext acceptContext;
        ntca::AcceptOptions acceptOptions;

        error = co_await ntcf::Concurrent::accept(listenerSocket,
                                                  &acceptContext,
                                                  &streamSocket,
                                                  acceptOptions);
        NTSCFG_TEST_OK(error);

        BALL_LOG_INFO << "Server socket accept complete: " << acceptContext
                      << BALL_LOG_END;

        // Enter a coroutine dedicated to the server stream socket.

        co_await coVerifyApplicationServer(configuration,
                                           listenerSocket,
                                           streamSocket,
                                           allocator);
    }

    // Close the listener socket.

    co_await ntcf::Concurrent::close(listenerSocket);

    BALL_LOG_INFO << "Listener socket closed" << BALL_LOG_END;

    co_return;
}

ntsa::CoroutineTask<void> ConcurrentTest::coVerifyApplicationClient(
    const Configuration&                         configuration,
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const bsl::shared_ptr<ntci::StreamSocket>&   streamSocket,
    ntsa::Allocator                              allocator)
{
    ntsa::Error error;

    // Connect the client stream socket to the listener socket.

    ntca::ConnectContext connectContext;
    ntca::ConnectOptions connectOptions;

    error =
        co_await ntcf::Concurrent::connect(streamSocket,
                                           &connectContext,
                                           listenerSocket->sourceEndpoint(),
                                           connectOptions);
    NTSCFG_TEST_OK(error);

    BALL_LOG_INFO << "Client socket connect complete: " << connectContext
                  << BALL_LOG_END;

    // Send data from the client stream socket to the server stream socket.

    bsl::shared_ptr<bdlbb::Blob> sendData = streamSocket->createOutgoingBlob();

    bdlbb::BlobUtil::append(sendData.get(), "Hello, world!", 13);

    ntca::SendContext sendContext;
    ntca::SendOptions sendOptions;

    error = co_await ntcf::Concurrent::send(streamSocket,
                                            &sendContext,
                                            sendData,
                                            sendOptions);
    NTSCFG_TEST_OK(error);

    BALL_LOG_INFO << "Client socket send complete: " << sendContext << "\n"
                  << bdlbb::BlobUtilHexDumper(sendData.get()) << BALL_LOG_END;

    // Close the client stream socket.

    co_await ntcf::Concurrent::close(streamSocket);

    BALL_LOG_INFO << "Client socket closed" << BALL_LOG_END;

    co_return;
}

ntsa::CoroutineTask<void> ConcurrentTest::coVerifyApplicationServer(
    const Configuration&                         configuration,
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const bsl::shared_ptr<ntci::StreamSocket>&   streamSocket,
    ntsa::Allocator                              allocator)
{
    ntsa::Error error;

    // Receive data at the server datagram socket from the client datagram
    // socket.

    bsl::shared_ptr<bdlbb::Blob> receiveData =
        streamSocket->createIncomingBlob();

    ntca::ReceiveContext receiveContext;
    ntca::ReceiveOptions receiveOptions;

    receiveOptions.setSize(13);

    error = co_await ntcf::Concurrent::receive(streamSocket,
                                               &receiveContext,
                                               &receiveData,
                                               receiveOptions);
    NTSCFG_TEST_OK(error);

    BALL_LOG_INFO << "Server socket receive complete: " << receiveContext
                  << "\n"
                  << bdlbb::BlobUtilHexDumper(receiveData.get())
                  << BALL_LOG_END;

    // Close the server stream socket.

    co_await ntcf::Concurrent::close(streamSocket);

    BALL_LOG_INFO << "Server socket closed" << BALL_LOG_END;

    co_return;
}

ntsa::CoroutineTask<void> ConcurrentTest::coVerifySandbox(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    ntsa::Allocator                         allocator)
{
    NTCCFG_WARNING_UNUSED(scheduler);
    NTCCFG_WARNING_UNUSED(allocator);

    co_return;
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifyDatagramSocket)
{
    ntccfg::Object scope("verifyDatagramSocket");

    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Scheduler> scheduler;
    ntcf::System::getDefault(&scheduler);

    ntsa::CoroutineTask<void> task =
        coVerifyDatagramSocket(scheduler, allocator);
    ntsa::CoroutineTaskUtil::synchronize(bsl::move(task));
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifyStreamSocket)
{
    ntccfg::Object scope("verifyStreamSocket");

    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Scheduler> scheduler;
    ntcf::System::getDefault(&scheduler);

    ntsa::CoroutineTask<void> task =
        coVerifyStreamSocket(scheduler, allocator);
    ntsa::CoroutineTaskUtil::synchronize(bsl::move(task));
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifyApplication)
{
    ntccfg::Object scope("verifyApplication");

    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Scheduler> scheduler;
    ntcf::System::getDefault(&scheduler);

    ntsa::CoroutineTask<void> task = coVerifyApplication(scheduler, allocator);
    ntsa::CoroutineTaskUtil::synchronize(bsl::move(task));
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifySandbox)
{
    ntccfg::Object scope("verifySandbox");

    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Scheduler> scheduler;
    ntcf::System::getDefault(&scheduler);

    ntsa::CoroutineTask<void> task = coVerifySandbox(scheduler, allocator);
    ntsa::CoroutineTaskUtil::synchronize(bsl::move(task));
}

}  // close namespace ntcf
}  // close namespace BloombergLP
#endif
