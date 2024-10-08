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
BSLS_IDENT_RCSID(ntsf_system_t_cpp, "$Id$ $CSID$")

#include <ntsf_system.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsf {

// Provide tests for 'ntsf::System'.
class SystemTest
{
  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifyCase3();

    // TODO
    static void verifyCase4();

    // TODO
    static void verifyCase5();

    // TODO
    static void verifyCase6();

    // TODO
    static void verifyCase7();
};

NTSCFG_TEST_FUNCTION(ntsf::SystemTest::verifyCase1)
{
    // Concern: A TCP/IPv4 socket may bind to the same port number assigned to
    // another TCP/IPv4 socket as long as the first socket binds to INADDR_ANY
    // and both sockets specify SO_REUSEADDR.
    //
    // Note that some operating system allow two sockets to bind to both the
    // same *address* and port, but this is not portable, e.g. on Solaris.
    // The technique is only portable if the first socket binds to INADDR_ANY
    // and the second socket binds to some address that is *not* INADDR_ANY.
    //
    // This case specifically tests binding the second socket to INADDR_ANY,
    // and verifies the assumption that the binding only succeeds on certain
    // operating systems but fails on others.

    ntsa::Error error;

    const bool k_REUSE_ADDRESS = true;

    // Create an "outer" TCP/IPv4 listener socket.

    bsl::shared_ptr<ntsi::ListenerSocket> listenerSocketOne =
        ntsf::System::createListenerSocket(NTSCFG_TEST_ALLOCATOR);

    error = listenerSocketOne->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    // Bind the "outer" listener socket to INADDR_ANY:0.

    error = listenerSocketOne->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::any(), 0)),
        k_REUSE_ADDRESS);
    NTSCFG_TEST_OK(error);

    // Learn the port assigned to the "outer" socket.

    ntsa::Endpoint listenerSocketOneSourceEndpoint;
    error = listenerSocketOne->sourceEndpoint(
        &listenerSocketOneSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Create an "inner" TCP/IPv4 listener socket.

    bsl::shared_ptr<ntsi::ListenerSocket> listenerSocketTwo =
        ntsf::System::createListenerSocket(NTSCFG_TEST_ALLOCATOR);

    error = listenerSocketTwo->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    // Bind the "inner" listener socket to INADDR_ANY and the port of the
    // "outer" listener socket.

    error = listenerSocketTwo->bind(
        ntsa::Endpoint(
            ntsa::IpEndpoint(ntsa::Ipv4Address::any(),
                                listenerSocketOneSourceEndpoint.ip().port())),
        k_REUSE_ADDRESS);

    // Assert the binding succeeds on Linux and AIX, and fails on all
    // other operating systems.

#if defined(BSLS_PLATFORM_OS_AIX) || defined(BSLS_PLATFORM_OS_LINUX)

    NTSCFG_TEST_OK(error);

    // Learn the port assigned to the "inner" socket.

    ntsa::Endpoint listenerSocketTwoSourceEndpoint;
    error = listenerSocketTwo->sourceEndpoint(
        &listenerSocketTwoSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Assert the endpoints to which the two sockets are bound are the
    // same.

    NTSCFG_TEST_EQ(listenerSocketOneSourceEndpoint.ip().port(),
                    listenerSocketTwoSourceEndpoint.ip().port());

    // Begin listening on the "inner" listener socket.

    error = listenerSocketTwo->listen(1);
    NTSCFG_TEST_OK(error);

    // Create a "client" TCP/IPv4 stream socket.

    bsl::shared_ptr<ntsi::StreamSocket> clientSocket =
        ntsf::System::createStreamSocket(NTSCFG_TEST_ALLOCATOR);

    error = clientSocket->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    // Connect the "client" stream socket to the "inner" listening socket.

    error = clientSocket->connect(listenerSocketTwoSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Accept a "server" stream socket from the "inner" listening socket.

    bsl::shared_ptr<ntsi::StreamSocket> serverSocket;
    error = listenerSocketTwo->accept(&serverSocket, NTSCFG_TEST_ALLOCATOR);
    NTSCFG_TEST_OK(error);

    // Send data from the "client" socket to the "server" socket.

    ntsa::SendContext sendContext;
    ntsa::SendOptions sendOptions;

    char sendData[13];
    bsl::memcpy(sendData, "Hello, world!", 13);

    error = clientSocket->send(&sendContext, sendData, 13, sendOptions);
    NTSCFG_TEST_OK(error);
    NTSCFG_TEST_EQ(sendContext.bytesSent(), 13);

    // Receive data at the "server" socket send by the "client" socket.

    ntsa::ReceiveContext receiveContext;
    ntsa::ReceiveOptions receiveOptions;

    char receiveData[13];

    error = serverSocket->receive(&receiveContext,
                                    receiveData,
                                    sizeof receiveData,
                                    receiveOptions);
    NTSCFG_TEST_OK(error);
    NTSCFG_TEST_EQ(receiveContext.bytesReceived(), 13);

    // Assert the data received by the "server" socket matches the data
    // send by the "client" socket.

    NTSCFG_TEST_EQ(bsl::memcmp(receiveData, sendData, 13), 0);

    // Close the "client" socket.

    error = clientSocket->close();
    NTSCFG_TEST_OK(error);

    // Close the "server" socket.

    error = serverSocket->close();
    NTSCFG_TEST_OK(error);

#elif defined(BSLS_PLATFORM_OS_SOLARIS)

    // Assert that Solaris refuses to allow two sockets to bind to the
    // same port, despite both sockets having SO_REUSEADDR set, unless
    // the second sockets binds to an IPv4 address that is different
    // than the IPv4 address to which the first socket is bound. Note that
    // other operating systems may also exhibit the same behavior, but
    // this test case is simply verifying that this combination of
    // parameters is not portable.

    NTSCFG_TEST_ERROR(error, ntsa::Error::e_ADDRESS_IN_USE);

#endif

    // Close the "inner" listener socket.

    error = listenerSocketTwo->close();
    NTSCFG_TEST_OK(error);

    // Close the "outer" listener socket.

    error = listenerSocketOne->close();
    NTSCFG_TEST_OK(error);
}

NTSCFG_TEST_FUNCTION(ntsf::SystemTest::verifyCase2)
{
    // Concern: A TCP/IPv4 socket may bind to the same port number assigned to
    // another TCP/IPv4 socket as long as the first socket binds to INADDR_ANY
    // and both sockets specify SO_REUSEADDR.
    //
    // Note that some operating system allow two sockets to bind to both the
    // same *address* and port, but this is not portable, e.g. on Solaris.
    // The technique is only portable if the first socket binds to INADDR_ANY
    // and the second socket binds to some address that is *not* INADDR_ANY.
    //
    // This case specifically tests binding the second socket to the loopback
    // address.

    ntsa::Error error;

    const bool k_REUSE_ADDRESS = true;

    // Create an "outer" TCP/IPv4 listener socket.

    bsl::shared_ptr<ntsi::ListenerSocket> listenerSocketOne =
        ntsf::System::createListenerSocket(NTSCFG_TEST_ALLOCATOR);

    error = listenerSocketOne->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    // Bind the "outer" listener socket to INADDRY_ANY:0.

    error = listenerSocketOne->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::any(), 0)),
        k_REUSE_ADDRESS);
    NTSCFG_TEST_OK(error);

    // Learn the port assigned to the "outer" socket.

    ntsa::Endpoint listenerSocketOneSourceEndpoint;
    error = listenerSocketOne->sourceEndpoint(
        &listenerSocketOneSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Create an "inner" TCP/IPv4 listener socket.

    bsl::shared_ptr<ntsi::ListenerSocket> listenerSocketTwo =
        ntsf::System::createListenerSocket(NTSCFG_TEST_ALLOCATOR);

    error = listenerSocketTwo->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    // Bind the "inner" listener socket to 127.0.0.1 and the port of the
    // "outer" listener socket.

    error = listenerSocketTwo->bind(
        ntsa::Endpoint(
            ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(),
                                listenerSocketOneSourceEndpoint.ip().port())),
        k_REUSE_ADDRESS);
    NTSCFG_TEST_OK(error);

    // Learn the port assigned to the "inner" socket.

    ntsa::Endpoint listenerSocketTwoSourceEndpoint;
    error = listenerSocketTwo->sourceEndpoint(
        &listenerSocketTwoSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Assert the endpoints to which the two sockets are bound are the
    // same.

    NTSCFG_TEST_EQ(listenerSocketOneSourceEndpoint.ip().port(),
                    listenerSocketTwoSourceEndpoint.ip().port());

    // Begin listening on the "inner" listener socket.

    error = listenerSocketTwo->listen(1);
    NTSCFG_TEST_OK(error);

    // Create a "client" TCP/IPv4 stream socket.

    bsl::shared_ptr<ntsi::StreamSocket> clientSocket =
        ntsf::System::createStreamSocket(NTSCFG_TEST_ALLOCATOR);

    error = clientSocket->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    // Connect the "client" stream socket to the "inner" listening socket.

    error = clientSocket->connect(listenerSocketTwoSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Accept a "server" stream socket from the "inner" listening socket.

    bsl::shared_ptr<ntsi::StreamSocket> serverSocket;
    error = listenerSocketTwo->accept(&serverSocket, NTSCFG_TEST_ALLOCATOR);
    NTSCFG_TEST_OK(error);

    // Send data from the "client" socket to the "server" socket.

    ntsa::SendContext sendContext;
    ntsa::SendOptions sendOptions;

    char sendData[13];
    bsl::memcpy(sendData, "Hello, world!", 13);

    error = clientSocket->send(&sendContext, sendData, 13, sendOptions);
    NTSCFG_TEST_OK(error);
    NTSCFG_TEST_EQ(sendContext.bytesSent(), 13);

    // Receive data at the "server" socket send by the "client" socket.

    ntsa::ReceiveContext receiveContext;
    ntsa::ReceiveOptions receiveOptions;

    char receiveData[13];

    error = serverSocket->receive(&receiveContext,
                                    receiveData,
                                    sizeof receiveData,
                                    receiveOptions);
    NTSCFG_TEST_OK(error);
    NTSCFG_TEST_EQ(receiveContext.bytesReceived(), 13);

    // Assert the data received by the "server" socket matches the data
    // send by the "client" socket.

    NTSCFG_TEST_EQ(bsl::memcmp(receiveData, sendData, 13), 0);

    // Close the "client" socket.

    error = clientSocket->close();
    NTSCFG_TEST_OK(error);

    // Close the "server" socket.

    error = serverSocket->close();
    NTSCFG_TEST_OK(error);

    // Close the "inner" listener socket.

    error = listenerSocketTwo->close();
    NTSCFG_TEST_OK(error);

    // Close the "outer" listener socket.

    error = listenerSocketOne->close();
    NTSCFG_TEST_OK(error);
}

NTSCFG_TEST_FUNCTION(ntsf::SystemTest::verifyCase3)
{
        // Concern: A TCP/IPv4 socket may bind to the same port number assigned to
    // another TCP/IPv4 socket as long as the first socket binds to INADDR_ANY
    // and both sockets specify SO_REUSEADDR.
    //
    // Note that some operating system allow two sockets to bind to both the
    // same *address* and port, but this is not portable, e.g. on Solaris.
    // The technique is only portable if the first socket binds to INADDR_ANY
    // and the second socket binds to some address that is *not* INADDR_ANY.
    //
    // This case specifically tests binding the second socket to the IPv4
    // address assigned to a network interface.

    ntsa::Error error;

    const bool k_REUSE_ADDRESS = true;

    // Discover the principle network interface assigned an IPv4 address.

    ntsa::Adapter adapter;
    bool          adapterFound =
        ntsf::System::discoverAdapter(&adapter,
                                        ntsa::IpAddressType::e_V4,
                                        false);
    NTSCFG_TEST_TRUE(adapterFound);

    NTSCFG_TEST_FALSE(adapter.ipv4Address().isNull());

    ntsa::Ipv4Address adapterAddress = adapter.ipv4Address().value();

    // Create an "outer" TCP/IPv4 listener socket.

    bsl::shared_ptr<ntsi::ListenerSocket> listenerSocketOne =
        ntsf::System::createListenerSocket(NTSCFG_TEST_ALLOCATOR);

    error = listenerSocketOne->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    // Bind the "outer" listener socket to the adapter address.

    error = listenerSocketOne->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::any(), 0)),
        k_REUSE_ADDRESS);
    NTSCFG_TEST_OK(error);

    // Learn the port assigned to the "outer" socket.

    ntsa::Endpoint listenerSocketOneSourceEndpoint;
    error = listenerSocketOne->sourceEndpoint(
        &listenerSocketOneSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Create an "inner" TCP/IPv4 listener socket.

    bsl::shared_ptr<ntsi::ListenerSocket> listenerSocketTwo =
        ntsf::System::createListenerSocket(NTSCFG_TEST_ALLOCATOR);

    error = listenerSocketTwo->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    // Bind the "inner" listener socket to the adapter address and the port
    // of the "outer" listener socket.

    error = listenerSocketTwo->bind(
        ntsa::Endpoint(
            ntsa::IpEndpoint(adapterAddress,
                                listenerSocketOneSourceEndpoint.ip().port())),
        k_REUSE_ADDRESS);
    NTSCFG_TEST_OK(error);

    // Learn the port assigned to the "inner" socket.

    ntsa::Endpoint listenerSocketTwoSourceEndpoint;
    error = listenerSocketTwo->sourceEndpoint(
        &listenerSocketTwoSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Assert the endpoints to which the two sockets are bound are the
    // same.

    NTSCFG_TEST_EQ(listenerSocketOneSourceEndpoint.ip().port(),
                    listenerSocketTwoSourceEndpoint.ip().port());

    // Begin listening on the "inner" listener socket.

    error = listenerSocketTwo->listen(1);
    NTSCFG_TEST_OK(error);

    // Create a "client" TCP/IPv4 stream socket.

    bsl::shared_ptr<ntsi::StreamSocket> clientSocket =
        ntsf::System::createStreamSocket(NTSCFG_TEST_ALLOCATOR);

    error = clientSocket->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    // Connect the "client" stream socket to the "inner" listening socket.

    error = clientSocket->connect(listenerSocketTwoSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Accept a "server" stream socket from the "inner" listening socket.

    bsl::shared_ptr<ntsi::StreamSocket> serverSocket;
    error = listenerSocketTwo->accept(&serverSocket, NTSCFG_TEST_ALLOCATOR);
    NTSCFG_TEST_OK(error);

    // Send data from the "client" socket to the "server" socket.

    ntsa::SendContext sendContext;
    ntsa::SendOptions sendOptions;

    char sendData[13];
    bsl::memcpy(sendData, "Hello, world!", 13);

    error = clientSocket->send(&sendContext, sendData, 13, sendOptions);
    NTSCFG_TEST_OK(error);
    NTSCFG_TEST_EQ(sendContext.bytesSent(), 13);

    // Receive data at the "server" socket send by the "client" socket.

    ntsa::ReceiveContext receiveContext;
    ntsa::ReceiveOptions receiveOptions;

    char receiveData[13];

    error = serverSocket->receive(&receiveContext,
                                    receiveData,
                                    sizeof receiveData,
                                    receiveOptions);
    NTSCFG_TEST_OK(error);
    NTSCFG_TEST_EQ(receiveContext.bytesReceived(), 13);

    // Assert the data received by the "server" socket matches the data
    // send by the "client" socket.

    NTSCFG_TEST_EQ(bsl::memcmp(receiveData, sendData, 13), 0);

    // Close the "client" socket.

    error = clientSocket->close();
    NTSCFG_TEST_OK(error);

    // Close the "server" socket.

    error = serverSocket->close();
    NTSCFG_TEST_OK(error);

    // Close the "inner" listener socket.

    error = listenerSocketTwo->close();
    NTSCFG_TEST_OK(error);

    // Close the "outer" listener socket.

    error = listenerSocketOne->close();
    NTSCFG_TEST_OK(error);
}

NTSCFG_TEST_FUNCTION(ntsf::SystemTest::verifyCase4)
{
    // Concern: Resolution from default resolver automatically initialized

    ntsa::Error error;

    bsl::vector<ntsa::IpAddress> ipAddressListOverride;
    ipAddressListOverride.push_back(ntsa::IpAddress("4.4.4.4"));
    ipAddressListOverride.push_back(ntsa::IpAddress("8.8.8.8"));

    error =
        ntsf::System::setIpAddress("example.com", ipAddressListOverride);
    NTSCFG_TEST_OK(error);

    bsl::set<ntsa::IpAddress> ipAddressSet(ipAddressListOverride.begin(),
                                            ipAddressListOverride.end());

    NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

    bsl::vector<ntsa::IpAddress> ipAddressList;
    ntsa::IpAddressOptions       ipAddressOptions;

    error = ntsf::System::getIpAddress(&ipAddressList,
                                        "example.com",
                                        ipAddressOptions);
    NTSCFG_TEST_FALSE(error);

    for (bsl::vector<ntsa::IpAddress>::const_iterator it =
                ipAddressList.begin();
            it != ipAddressList.end();
            ++it)
    {
        NTSCFG_TEST_LOG_DEBUG << "Address: " << it->text()
                                << NTSCFG_TEST_LOG_END;

        bsl::size_t n = ipAddressSet.erase(*it);
        NTSCFG_TEST_EQ(n, 1);
    }

    NTSCFG_TEST_TRUE(ipAddressSet.empty());
}

NTSCFG_TEST_FUNCTION(ntsf::SystemTest::verifyCase5)
{
    // Concern: Resolution from default resolver explicitly installed

    ntsa::Error error;

    {
        ntsa::ResolverConfig resolverConfig;
        resolverConfig.setOverridesEnabled(true);
        resolverConfig.setSystemEnabled(false);

        bsl::shared_ptr<ntsi::Resolver> resolver =
            ntsf::System::createResolver(
                resolverConfig,
                bslma::Default::globalAllocator());

        NTSCFG_TEST_EQ(resolver.use_count(), 1);

        ntsf::System::setDefault(resolver);

        NTSCFG_TEST_EQ(resolver.use_count(), 2);

        bsl::shared_ptr<ntsi::Resolver> resolverDefault;
        ntsf::System::getDefault(&resolverDefault);

        NTSCFG_TEST_EQ(resolver.use_count(), 3);

        NTSCFG_TEST_EQ(resolverDefault, resolver);
    }

    bsl::vector<ntsa::IpAddress> ipAddressListOverride;
    ipAddressListOverride.push_back(ntsa::IpAddress("4.4.4.4"));
    ipAddressListOverride.push_back(ntsa::IpAddress("8.8.8.8"));

    error =
        ntsf::System::setIpAddress("example.com", ipAddressListOverride);
    NTSCFG_TEST_OK(error);

    bsl::set<ntsa::IpAddress> ipAddressSet(ipAddressListOverride.begin(),
                                            ipAddressListOverride.end());

    NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

    bsl::vector<ntsa::IpAddress> ipAddressList;
    ntsa::IpAddressOptions       ipAddressOptions;

    error = ntsf::System::getIpAddress(&ipAddressList,
                                        "example.com",
                                        ipAddressOptions);
    NTSCFG_TEST_FALSE(error);

    for (bsl::vector<ntsa::IpAddress>::const_iterator it =
                ipAddressList.begin();
            it != ipAddressList.end();
            ++it)
    {
        NTSCFG_TEST_LOG_DEBUG << "Address: " << it->text()
                                << NTSCFG_TEST_LOG_END;

        bsl::size_t n = ipAddressSet.erase(*it);
        NTSCFG_TEST_EQ(n, 1);
    }

    NTSCFG_TEST_TRUE(ipAddressSet.empty());
}

NTSCFG_TEST_FUNCTION(ntsf::SystemTest::verifyCase6)
{
    // Concern: test that loadTcpCongestionControlAlgorithmSupport does not
    // contradict with testTcpCongestionControlAlgorithmSupport.

    ntsa::Error error;

    bsl::vector<bsl::string> supportedAlgorithms(NTSCFG_TEST_ALLOCATOR);
    error =
        ntsf::System::loadTcpCongestionControlAlgorithmSupport(
            &supportedAlgorithms);

#if defined(BSLS_PLATFORM_OS_LINUX)
    NTSCFG_TEST_OK(error);

    for (bsl::vector<bsl::string>::const_iterator it =
                supportedAlgorithms.cbegin();
            it != supportedAlgorithms.cend();
            ++it)
    {
        NTSCFG_TEST_TRUE(
            ntsf::System::testTcpCongestionControlAlgorithmSupport(*it));
    }
#else
    NTSCFG_TEST_ERROR(error, ntsa::Error::e_NOT_IMPLEMENTED);
#endif

    NTSCFG_TEST_FALSE(
        ntsf::System::testTcpCongestionControlAlgorithmSupport(
            "random_name"));
}

NTSCFG_TEST_FUNCTION(ntsf::SystemTest::verifyCase7)
{
    // Concern: setBlocking/getBlocking.

    ntsa::Error error;

    bsl::shared_ptr<ntsi::DatagramSocket> socket =
        ntsf::System::createDatagramSocket(NTSCFG_TEST_ALLOCATOR);

    error = socket->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

#if defined(BSLS_PLATFORM_OS_UNIX)
    error = ntsf::System::setBlocking(socket->handle(), true);
    NTSCFG_TEST_OK(error);

    bool blocking = false;
    error = ntsf::System::getBlocking(socket->handle(), &blocking);
    NTSCFG_TEST_OK(error);
    NTSCFG_TEST_TRUE(blocking);

    error = ntsf::System::setBlocking(socket->handle(), false);
    NTSCFG_TEST_OK(error);

    blocking = true;
    error = ntsf::System::getBlocking(socket->handle(), &blocking);
    NTSCFG_TEST_OK(error);
    NTSCFG_TEST_FALSE(blocking);

#else

    bool blocking = false;
    error = ntsf::System::getBlocking(socket->handle(), &blocking);
    NTSCFG_TEST_TRUE(error);

#endif

    error = socket->close();
    NTSCFG_TEST_OK(error);
}

}  // close namespace ntsf
}  // close namespace BloombergLP
