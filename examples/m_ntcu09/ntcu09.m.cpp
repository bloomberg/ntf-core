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

#include <ntccfg_bind.h>
#include <ntcf_system.h>
#include <ntsf_system.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobutil.h>
#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadgroup.h>

using namespace BloombergLP;

namespace example {

void processConnect(bslmt::Semaphore*                       semaphore,
                    const bsl::shared_ptr<ntci::Connector>& connector,
                    const ntca::ConnectEvent&               event)
{
    NTCCFG_WARNING_UNUSED(connector);

    BSLS_ASSERT_OPT(event.type() == ntca::ConnectEventType::e_COMPLETE);
    semaphore->post();
}

void processAccept(bslmt::Semaphore*                          semaphore,
                   bsl::shared_ptr<ntci::StreamSocket>*       result,
                   const bsl::shared_ptr<ntci::Acceptor>&     acceptor,
                   const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                   const ntca::AcceptEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(acceptor);

    BSLS_ASSERT_OPT(event.type() == ntca::AcceptEventType::e_COMPLETE);
    *result = streamSocket;
    semaphore->post();
}

void processSend(bslmt::Semaphore*                    semaphore,
                 const bsl::shared_ptr<ntci::Sender>& sender,
                 const ntca::SendEvent&               event)
{
    NTCCFG_WARNING_UNUSED(sender);

    BSLS_ASSERT_OPT(event.type() == ntca::SendEventType::e_COMPLETE);
    semaphore->post();
}

void processReceive(bslmt::Semaphore*                      semaphore,
                    bdlbb::Blob*                           result,
                    const bsl::shared_ptr<ntci::Receiver>& receiver,
                    const bsl::shared_ptr<bdlbb::Blob>&    data,
                    const ntca::ReceiveEvent&              event)
{
    NTCCFG_WARNING_UNUSED(receiver);

    BSLS_ASSERT_OPT(event.type() == ntca::ReceiveEventType::e_COMPLETE);
    *result = *data;
    semaphore->post();
}

void processClose(bslmt::Semaphore* semaphore)
{
    semaphore->post();
}

namespace usage_tcp_ipv4_cpp03 {

void execute(bslma::Allocator* allocator)
{
    NTCCFG_WARNING_UNUSED(allocator);

    // Initialize the library.

    ntcf::System::initialize();
    ntcf::System::ignore(ntscfg::Signal::e_PIPE);

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    // Create and start a pool of I/O threads.

    ntca::InterfaceConfig interfaceConfig;
    interfaceConfig.setThreadName("example");

    bsl::shared_ptr<ntci::Interface> interface =
        ntcf::System::createInterface(interfaceConfig);

    error = interface->start();
    BSLS_ASSERT_OPT(!error);

    // Create a listener socket and begin listening.

    ntca::ListenerSocketOptions listenerSocketOptions;
    listenerSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
    listenerSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0));

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        interface->createListenerSocket(listenerSocketOptions);

    error = listenerSocket->open();
    BSLS_ASSERT_OPT(!error);

    error = listenerSocket->listen();
    BSLS_ASSERT_OPT(!error);

    // Connect a socket to the listener.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> clientSocket =
        interface->createStreamSocket(streamSocketOptions);

    ntci::ConnectCallback connectCallback =
        clientSocket->createConnectCallback(
            NTCCFG_BIND(&processConnect,
                        &semaphore,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2));

    error = clientSocket->connect(listenerSocket->sourceEndpoint(),
                                  ntca::ConnectOptions(),
                                  connectCallback);
    BSLS_ASSERT_OPT(!error);

    semaphore.wait();

    // Accept a connection from the listener socket's backlog.

    bsl::shared_ptr<ntci::StreamSocket> serverSocket;

    ntci::AcceptCallback acceptCallback = listenerSocket->createAcceptCallback(
        NTCCFG_BIND(&processAccept,
                    &semaphore,
                    &serverSocket,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3));

    error = listenerSocket->accept(ntca::AcceptOptions(), acceptCallback);
    BSLS_ASSERT_OPT(!error || error == ntsa::Error::e_WOULD_BLOCK);

    semaphore.wait();

    // Send some data from the client to the server.

    bdlbb::Blob clientData(clientSocket->outgoingBlobBufferFactory().get());
    bdlbb::BlobUtil::append(&clientData, "Hello, world!", 13);

    ntci::SendCallback sendCallback = clientSocket->createSendCallback(
        NTCCFG_BIND(&processSend,
                    &semaphore,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2));

    error = clientSocket->send(clientData, ntca::SendOptions(), sendCallback);
    BSLS_ASSERT_OPT(!error);

    semaphore.wait();

    // Receive the expected amount of data from the client.

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setSize(13);

    bdlbb::Blob serverData;

    ntci::ReceiveCallback receiveCallback =
        serverSocket->createReceiveCallback(
            NTCCFG_BIND(&processReceive,
                        &semaphore,
                        &serverData,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3));

    error = serverSocket->receive(receiveOptions, receiveCallback);
    BSLS_ASSERT_OPT(!error || error == ntsa::Error::e_WOULD_BLOCK);

    semaphore.wait();

    // Ensure the data received matches the data sent.

    BSLS_ASSERT_OPT(bdlbb::BlobUtil::compare(clientData, serverData) == 0);

    // Close the client socket.

    clientSocket->close(clientSocket->createCloseCallback(
        NTCCFG_BIND(&processClose, &semaphore)));

    semaphore.wait();

    // Close the server socket.

    serverSocket->close(serverSocket->createCloseCallback(
        NTCCFG_BIND(&processClose, &semaphore)));

    semaphore.wait();

    // Close the listener socket.

    listenerSocket->close(listenerSocket->createCloseCallback(
        NTCCFG_BIND(&processClose, &semaphore)));

    semaphore.wait();

    // Join the I/O thread pool.

    interface->shutdown();
    interface->linger();
}

}  // close namespace usage_tcp_ipv4_cpp03

namespace usage_tcp_ipv4_cpp11 {

void execute(bslma::Allocator* allocator)
{
#if NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMDAS

    NTCCFG_WARNING_UNUSED(allocator);

    // Initialize the library.

    ntcf::System::initialize();
    ntcf::System::ignore(ntscfg::Signal::e_PIPE);

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    // Create and start a pool of I/O threads.

    ntca::InterfaceConfig interfaceConfig;
    interfaceConfig.setThreadName("example");

    bsl::shared_ptr<ntci::Interface> interface =
        ntcf::System::createInterface(interfaceConfig);

    error = interface->start();
    BSLS_ASSERT_OPT(!error);

    // Create a listener socket and begin listening.

    ntca::ListenerSocketOptions listenerSocketOptions;
    listenerSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
    listenerSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0));

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        interface->createListenerSocket(listenerSocketOptions);

    error = listenerSocket->open();
    BSLS_ASSERT_OPT(!error);

    error = listenerSocket->listen();
    BSLS_ASSERT_OPT(!error);

    // Connect a socket to the listener.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> clientSocket =
        interface->createStreamSocket(streamSocketOptions);

    ntci::ConnectCallback connectCallback =
        clientSocket->createConnectCallback([&](auto connector, auto event) {
            NTCCFG_WARNING_UNUSED(connector);
            BSLS_ASSERT_OPT(event.type() ==
                            ntca::ConnectEventType::e_COMPLETE);
            semaphore.post();
        });

    error = clientSocket->connect(listenerSocket->sourceEndpoint(),
                                  ntca::ConnectOptions(),
                                  connectCallback);
    BSLS_ASSERT_OPT(!error);

    semaphore.wait();

    // Accept a connection from the listener socket's backlog.

    bsl::shared_ptr<ntci::StreamSocket> serverSocket;

    ntci::AcceptCallback acceptCallback = listenerSocket->createAcceptCallback(
        [&](auto acceptor, auto streamSocket, auto event) {
            NTCCFG_WARNING_UNUSED(acceptor);
            BSLS_ASSERT_OPT(event.type() == ntca::AcceptEventType::e_COMPLETE);
            serverSocket = streamSocket;
            semaphore.post();
        });

    error = listenerSocket->accept(ntca::AcceptOptions(), acceptCallback);
    BSLS_ASSERT_OPT(!error || error == ntsa::Error::e_WOULD_BLOCK);

    semaphore.wait();

    // Send some data from the client to the server.

    bdlbb::Blob clientData(clientSocket->outgoingBlobBufferFactory().get());
    bdlbb::BlobUtil::append(&clientData, "Hello, world!", 13);

    ntci::SendCallback sendCallback =
        clientSocket->createSendCallback([&](auto sender, auto event) {
            NTCCFG_WARNING_UNUSED(sender);
            BSLS_ASSERT_OPT(event.type() == ntca::SendEventType::e_COMPLETE);
            semaphore.post();
        });

    error = clientSocket->send(clientData, ntca::SendOptions(), sendCallback);
    BSLS_ASSERT_OPT(!error);

    semaphore.wait();

    // Receive the expected amount of data from the client.

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setSize(13);

    bdlbb::Blob serverData;

    ntci::ReceiveCallback receiveCallback =
        serverSocket->createReceiveCallback(
            [&](auto receiver, auto data, auto event) {
                NTCCFG_WARNING_UNUSED(receiver);
                BSLS_ASSERT_OPT(event.type() ==
                                ntca::ReceiveEventType::e_COMPLETE);
                serverData = *data;
                semaphore.post();
            });

    error = serverSocket->receive(receiveOptions, receiveCallback);
    BSLS_ASSERT_OPT(!error || error == ntsa::Error::e_WOULD_BLOCK);

    semaphore.wait();

    // Ensure the data received matches the data sent.

    BSLS_ASSERT_OPT(bdlbb::BlobUtil::compare(clientData, serverData) == 0);

    // Close the client socket.

    clientSocket->close(clientSocket->createCloseCallback([&]() {
        semaphore.post();
    }));

    semaphore.wait();

    // Close the server socket.

    serverSocket->close(serverSocket->createCloseCallback([&]() {
        semaphore.post();
    }));

    semaphore.wait();

    // Close the listener socket.

    listenerSocket->close(listenerSocket->createCloseCallback([&]() {
        semaphore.post();
    }));

    semaphore.wait();

    // Join the I/O thread pool.

    interface->shutdown();
    interface->linger();

#endif
}

}  // close namespace usage_tcp_ipv4_cpp11

namespace usage_local_cpp03 {

void execute(bslma::Allocator* allocator)
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    NTCCFG_WARNING_UNUSED(allocator);

    // Initialize the library.

    ntcf::System::initialize();
    ntcf::System::ignore(ntscfg::Signal::e_PIPE);

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    // Create and start a pool of I/O threads.

    ntca::InterfaceConfig interfaceConfig;
    interfaceConfig.setThreadName("example");

    bsl::shared_ptr<ntci::Interface> interface =
        ntcf::System::createInterface(interfaceConfig);

    error = interface->start();
    BSLS_ASSERT_OPT(!error);

    // Create a listener socket and begin listening.

    ntca::ListenerSocketOptions listenerSocketOptions;
    listenerSocketOptions.setTransport(ntsa::Transport::e_LOCAL_STREAM);

    ntsa::LocalName localName;
    error = ntsa::LocalName::generateUnique(&localName);
    BSLS_ASSERT_OPT(!error);

    listenerSocketOptions.setSourceEndpoint(ntsa::Endpoint(localName));

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        interface->createListenerSocket(listenerSocketOptions);

    error = listenerSocket->open();
    BSLS_ASSERT_OPT(!error);

    error = listenerSocket->listen();
    BSLS_ASSERT_OPT(!error);

    // Connect a socket to the listener.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_LOCAL_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> clientSocket =
        interface->createStreamSocket(streamSocketOptions);

    ntci::ConnectCallback connectCallback =
        clientSocket->createConnectCallback(
            NTCCFG_BIND(&processConnect,
                        &semaphore,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2));

    error = clientSocket->connect(listenerSocket->sourceEndpoint(),
                                  ntca::ConnectOptions(),
                                  connectCallback);
    BSLS_ASSERT_OPT(!error);

    semaphore.wait();

    // Accept a connection from the listener socket's backlog.

    bsl::shared_ptr<ntci::StreamSocket> serverSocket;

    ntci::AcceptCallback acceptCallback = listenerSocket->createAcceptCallback(
        NTCCFG_BIND(&processAccept,
                    &semaphore,
                    &serverSocket,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3));

    error = listenerSocket->accept(ntca::AcceptOptions(), acceptCallback);
    BSLS_ASSERT_OPT(!error || error == ntsa::Error::e_WOULD_BLOCK);

    semaphore.wait();

    // Send some data from the client to the server.

    bdlbb::Blob clientData(clientSocket->outgoingBlobBufferFactory().get());
    bdlbb::BlobUtil::append(&clientData, "Hello, world!", 13);

    ntci::SendCallback sendCallback = clientSocket->createSendCallback(
        NTCCFG_BIND(&processSend,
                    &semaphore,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2));

    error = clientSocket->send(clientData, ntca::SendOptions(), sendCallback);
    BSLS_ASSERT_OPT(!error);

    semaphore.wait();

    // Receive the expected amount of data from the client.

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setSize(13);

    bdlbb::Blob serverData;

    ntci::ReceiveCallback receiveCallback =
        serverSocket->createReceiveCallback(
            NTCCFG_BIND(&processReceive,
                        &semaphore,
                        &serverData,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3));

    error = serverSocket->receive(receiveOptions, receiveCallback);
    BSLS_ASSERT_OPT(!error || error == ntsa::Error::e_WOULD_BLOCK);

    semaphore.wait();

    // Ensure the data received matches the data sent.

    BSLS_ASSERT_OPT(bdlbb::BlobUtil::compare(clientData, serverData) == 0);

    // Close the client socket.

    clientSocket->close(clientSocket->createCloseCallback(
        NTCCFG_BIND(&processClose, &semaphore)));

    semaphore.wait();

    // Close the server socket.

    serverSocket->close(serverSocket->createCloseCallback(
        NTCCFG_BIND(&processClose, &semaphore)));

    semaphore.wait();

    // Close the listener socket.

    listenerSocket->close(listenerSocket->createCloseCallback(
        NTCCFG_BIND(&processClose, &semaphore)));

    semaphore.wait();

    // Join the I/O thread pool.

    interface->shutdown();
    interface->linger();

#endif
}

}  // close namespace usage_local_cpp03

namespace usage_local_cpp11 {

void execute(bslma::Allocator* allocator)
{
#if defined(BSLS_PLATFORM_OS_UNIX)
#if NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMDAS

    NTCCFG_WARNING_UNUSED(allocator);

    // Initialize the library.

    ntcf::System::initialize();
    ntcf::System::ignore(ntscfg::Signal::e_PIPE);

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    // Create and start a pool of I/O threads.

    ntca::InterfaceConfig interfaceConfig;
    interfaceConfig.setThreadName("example");

    bsl::shared_ptr<ntci::Interface> interface =
        ntcf::System::createInterface(interfaceConfig);

    error = interface->start();
    BSLS_ASSERT_OPT(!error);

    // Create a listener socket and begin listening.

    ntca::ListenerSocketOptions listenerSocketOptions;
    listenerSocketOptions.setTransport(ntsa::Transport::e_LOCAL_STREAM);

    ntsa::LocalName localName;
    error = ntsa::LocalName::generateUnique(&localName);
    BSLS_ASSERT_OPT(!error);

    listenerSocketOptions.setSourceEndpoint(ntsa::Endpoint(localName));

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        interface->createListenerSocket(listenerSocketOptions);

    error = listenerSocket->open();
    BSLS_ASSERT_OPT(!error);

    error = listenerSocket->listen();
    BSLS_ASSERT_OPT(!error);

    // Connect a socket to the listener.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_LOCAL_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> clientSocket =
        interface->createStreamSocket(streamSocketOptions);

    ntci::ConnectCallback connectCallback =
        clientSocket->createConnectCallback([&](auto connector, auto event) {
            NTCCFG_WARNING_UNUSED(connector);
            BSLS_ASSERT_OPT(event.type() ==
                            ntca::ConnectEventType::e_COMPLETE);
            semaphore.post();
        });

    error = clientSocket->connect(listenerSocket->sourceEndpoint(),
                                  ntca::ConnectOptions(),
                                  connectCallback);
    BSLS_ASSERT_OPT(!error);

    semaphore.wait();

    // Accept a connection from the listener socket's backlog.

    bsl::shared_ptr<ntci::StreamSocket> serverSocket;

    ntci::AcceptCallback acceptCallback = listenerSocket->createAcceptCallback(
        [&](auto acceptor, auto streamSocket, auto event) {
            NTCCFG_WARNING_UNUSED(acceptor);
            BSLS_ASSERT_OPT(event.type() == ntca::AcceptEventType::e_COMPLETE);
            serverSocket = streamSocket;
            semaphore.post();
        });

    error = listenerSocket->accept(ntca::AcceptOptions(), acceptCallback);
    BSLS_ASSERT_OPT(!error || error == ntsa::Error::e_WOULD_BLOCK);

    semaphore.wait();

    // Send some data from the client to the server.

    bdlbb::Blob clientData(clientSocket->outgoingBlobBufferFactory().get());
    bdlbb::BlobUtil::append(&clientData, "Hello, world!", 13);

    ntci::SendCallback sendCallback =
        clientSocket->createSendCallback([&](auto sender, auto event) {
            NTCCFG_WARNING_UNUSED(sender);
            BSLS_ASSERT_OPT(event.type() == ntca::SendEventType::e_COMPLETE);
            semaphore.post();
        });

    error = clientSocket->send(clientData, ntca::SendOptions(), sendCallback);
    BSLS_ASSERT_OPT(!error);

    semaphore.wait();

    // Receive the expected amount of data from the client.

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setSize(13);

    bdlbb::Blob serverData;

    ntci::ReceiveCallback receiveCallback =
        serverSocket->createReceiveCallback(
            [&](auto receiver, auto data, auto event) {
                NTCCFG_WARNING_UNUSED(receiver);
                BSLS_ASSERT_OPT(event.type() ==
                                ntca::ReceiveEventType::e_COMPLETE);
                serverData = *data;
                semaphore.post();
            });

    error = serverSocket->receive(receiveOptions, receiveCallback);
    BSLS_ASSERT_OPT(!error || error == ntsa::Error::e_WOULD_BLOCK);

    semaphore.wait();

    // Ensure the data received matches the data sent.

    BSLS_ASSERT_OPT(bdlbb::BlobUtil::compare(clientData, serverData) == 0);

    // Close the client socket.

    clientSocket->close(clientSocket->createCloseCallback([&]() {
        semaphore.post();
    }));

    semaphore.wait();

    // Close the server socket.

    serverSocket->close(serverSocket->createCloseCallback([&]() {
        semaphore.post();
    }));

    semaphore.wait();

    // Close the listener socket.

    listenerSocket->close(listenerSocket->createCloseCallback([&]() {
        semaphore.post();
    }));

    semaphore.wait();

    // Join the I/O thread pool.

    interface->shutdown();
    interface->linger();

#endif
#endif
}

}  // close namespace usage_local_cpp11
}  // close namespace example

void help()
{
    bsl::cout << "usage: ntcu90.tsk [-v <level>]" << bsl::endl;
}

int main(int argc, char** argv)
{
    int verbosity = 0;
    {
        int i = 1;
        while (i < argc) {
            if ((0 == std::strcmp(argv[i], "-?")) ||
                (0 == std::strcmp(argv[i], "--help")))
            {
                help();
                return 0;
            }

            if (0 == std::strcmp(argv[i], "-v") ||
                0 == std::strcmp(argv[i], "--verbosity"))
            {
                ++i;
                if (i >= argc) {
                    help();
                    return 1;
                }
                verbosity = std::atoi(argv[i]);
                ++i;
                continue;
            }

            bsl::cerr << "Invalid option: " << argv[i] << bsl::endl;
            return 1;
        }
    }

    switch (verbosity) {
    case 0:
        break;
    case 1:
        bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_ERROR);
        break;
    case 2:
        bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_WARN);
        break;
    case 3:
        bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_INFO);
        break;
    case 4:
        bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_DEBUG);
        break;
    default:
        bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_TRACE);
        break;
    }

    bslma::TestAllocator         defaultAllocator;
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);
    {
        example::usage_tcp_ipv4_cpp03::execute(&defaultAllocator);
        example::usage_tcp_ipv4_cpp11::execute(&defaultAllocator);

        example::usage_local_cpp03::execute(&defaultAllocator);
        example::usage_local_cpp11::execute(&defaultAllocator);
    }
    BSLS_ASSERT_OPT(defaultAllocator.numBlocksInUse() == 0);

    return 0;
}
