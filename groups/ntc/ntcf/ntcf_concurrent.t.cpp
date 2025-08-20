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

    // TODO
    static ntsa::CoroutineTask<void> coVerifyDatagramSocket(
        const bsl::shared_ptr<ntci::Scheduler>& scheduler,
        ntsa::Allocator                         memory);

    // TODO
    static ntsa::CoroutineTask<void> coVerifyStreamSocket(
        const bsl::shared_ptr<ntci::Scheduler>& scheduler,
        ntsa::Allocator                         memory);

    // TODO
    static ntsa::CoroutineTask<void> coVerifySandbox(
        const bsl::shared_ptr<ntci::Scheduler>& scheduler,
        ntsa::Allocator                         memory);

  public:
    // TODO
    static void verifyDatagramSocket();

    // TODO
    static void verifyStreamSocket();

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

    // Close the client datagram socket.

    co_await ntcf::Concurrent::close(clientStreamSocket);

    BALL_LOG_INFO << "Client socket closed" << BALL_LOG_END;

    // Close the server datagram socket.

    co_await ntcf::Concurrent::close(serverStreamSocket);

    BALL_LOG_INFO << "Server socket closed" << BALL_LOG_END;

    // Close the listener socket.

    co_await ntcf::Concurrent::close(listenerSocket);

    BALL_LOG_INFO << "Listener socket closed" << BALL_LOG_END;

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
    ntccfg::Object scope("coVerifyDatagramSocket");

    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Scheduler> scheduler;
    ntcf::System::getDefault(&scheduler);

    ntsa::CoroutineTask<void> task =
        coVerifyDatagramSocket(scheduler, allocator);
    ntsa::CoroutineTaskUtil::synchronize(bsl::move(task));
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifyStreamSocket)
{
    ntccfg::Object scope("coVerifyStreamSocket");

    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Scheduler> scheduler;
    ntcf::System::getDefault(&scheduler);

    ntsa::CoroutineTask<void> task =
        coVerifyStreamSocket(scheduler, allocator);
    ntsa::CoroutineTaskUtil::synchronize(bsl::move(task));
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifySandbox)
{
    ntccfg::Object scope("coVerifySandbox");

    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Scheduler> scheduler;
    ntcf::System::getDefault(&scheduler);

    ntsa::CoroutineTask<void> task = coVerifySandbox(scheduler, allocator);
    ntsa::CoroutineTaskUtil::synchronize(bsl::move(task));
}

}  // close namespace ntcf
}  // close namespace BloombergLP
#endif
