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
BSLS_IDENT_RCSID(ntcd_machine_t_cpp, "$Id$ $CSID$")

#include <ntcd_machine.h>

#include <ntcd_datautil.h>
#include <ntci_log.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcd {

// Provide tests for 'ntcd::Machine'.
class MachineTest
{
  public:
    // Concern: Opening and closing handles.
    static void verifyOpen();

    // Concern: Binding to ports.
    static void verifyBind();

    // Concern: Simulate blocking IPv4 datagram sockets, manually stepping the
    // simulated machine.
    static void verifyForegroundBlockingDatagramSockets();

    // Concern: Simulate blocking one-way connected IPv4 datagram sockets,
    // manually stepping the simulated machine.
    static void verifyForegroundBlockingDatagramSocketsConnectedOneWay();

    // Concern: Simulate blocking two-way connected IPv4 datagram sockets,
    // manually stepping the simulated machine.
    static void verifyForegroundBlockingDatagramSocketsConnectedTwoWay();

    // Concern: Simulate blocking IPv4 stream sockets, manually stepping the
    // simulated machine.
    static void verifyForegroundBlockingStreamSockets();

    // Concern: Simulate blocking IPv4 datagram sockets, automatically stepping
    // the simulated machine in a background thread.
    static void verifyBackgroundBlockingDatagramSockets();

    // Concern: Simulate blocking one-way connected IPv4 datagram sockets,
    // automatically stepping the simulated machine in a background
    // thread.
    static void verifyBackgroundBlockingDatagramSocketsConnectedOneWay();

    // Concern: Simulate blocking two-way connected IPv4 datagram sockets,
    // automatically stepping the simulated machine in a background
    // thread.
    static void verifyBackgroundBlockingDatagramSocketsConnectedTwoWay();

    // Concern: Simulate blocking IPv4 stream sockets, automatically stepping
    // the simulated machine in a background thread.
    static void verifyBackgroundBlockingStreamSockets();

    // Concern: Simulate non-blocking IPv4 datagram sockets, automatically
    // stepping the simulated machine in a background thread.
    static void verifyBackgroundNonblockingDatagramSockets();

    // Concern: Simulate non-blocking one-way connected IPv4 datagram sockets,
    // automatically stepping the simulated machine in a background
    // thread.
    static void verifyBackgroundNonblockingDatagramSocketsConnectedOneWay();

    // Concern: Simulate non-blocking two-way connected IPv4 datagram sockets,
    // automatically stepping the simulated machine in a background
    // thread.
    static void verifyBackgroundNonblockingDatagramSocketsConnectedTwoWay();

    // Concern: Simulate non-blocking IPv4 stream sockets, automatically
    // stepping the simulated machine in a background thread.
    static void verifyBackgroundNonblockingStreamSockets();

    // Concern: Sending and receiving data larger than socket buffer sizes.
    static void verifySendBufferOverflow();
};

NTSCFG_TEST_FUNCTION(ntcd::MachineTest::verifyOpen)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    ntsa::Error error;

    // Create a machine.

    bsl::shared_ptr<ntcd::Machine> machine;
    machine.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntcd::Session> sessionA =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);
    bsl::shared_ptr<ntcd::Session> sessionB =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);
    bsl::shared_ptr<ntcd::Session> sessionC =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_UDP_IPV4_DATAGRAM;

    // Open a session and ensure the session is assigned handle 3.

    error = sessionA->open(transport);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_EQ(sessionA->handle(), 3);

    // Close the session.

    error = sessionA->close();
    NTSCFG_TEST_OK(error);

    // Open the session and ensure the session is again assigned handle 3,
    // the handle is immediately reused.

    error = sessionA->open(transport);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_EQ(sessionA->handle(), 3);

    // Open another session and ensure the session is assigned handle 4.

    error = sessionB->open(transport);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_EQ(sessionB->handle(), 4);

    // Open another session and ensure the session is assigned handle 5.

    error = sessionC->open(transport);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_EQ(sessionC->handle(), 5);

    // Close the session that has been assigned handle 4.

    error = sessionB->close();
    NTSCFG_TEST_OK(error);

    // Open another session and ensure the session is again assigned handle
    // 4, the handle is immediately reused despite being in the "gap"
    // between the still-open handles 3 and 5.

    error = sessionB->open(transport);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_EQ(sessionB->handle(), 4);

    // Close all sessions.

    error = sessionA->close();
    NTSCFG_TEST_OK(error);

    error = sessionB->close();
    NTSCFG_TEST_OK(error);

    error = sessionC->close();
    NTSCFG_TEST_OK(error);
}

NTSCFG_TEST_FUNCTION(ntcd::MachineTest::verifyBind)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    ntsa::Error error;

    // Create a machine.

    bsl::shared_ptr<ntcd::Machine> machine;
    machine.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntcd::Session> sessionA =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);
    bsl::shared_ptr<ntcd::Session> sessionB =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);
    bsl::shared_ptr<ntcd::Session> sessionC =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    ntsa::Endpoint sourceEndpointA;
    ntsa::Endpoint sourceEndpointB;
    ntsa::Endpoint sourceEndpointC;

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_UDP_IPV4_DATAGRAM;

    // Open a session.

    error = sessionA->open(transport);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint and ensure the operation fails.

    error = sessionA->sourceEndpoint(&sourceEndpointA);
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));

    // Bind the session to the loopback address and request that the
    // assigned port is any available ephemeral port.

    error = sessionA->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint and ensure the session was bound to the
    // loopback address and the first available port in the ephemeral port
    // range.

    error = sessionA->sourceEndpoint(&sourceEndpointA);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_TRUE(sourceEndpointA.isIp());
    NTSCFG_TEST_EQ(sourceEndpointA.ip().host(),
                   ntsa::IpAddress::loopbackIpv4());
    NTSCFG_TEST_EQ(sourceEndpointA.ip().port(), 49152);

    // Close the session.

    error = sessionA->close();
    NTSCFG_TEST_OK(error);

    // Open the session again.

    error = sessionA->open(transport);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint and ensure the operation fails.

    error = sessionA->sourceEndpoint(&sourceEndpointA);
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));

    // Bind the session to the loopback address and request that the
    // assigned port is any available ephemeral port.

    error = sessionA->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint and ensure the session was again bound to
    // the loopback address and the first available port in the ephemeral
    // port range; the port is immediately reused.

    error = sessionA->sourceEndpoint(&sourceEndpointA);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_TRUE(sourceEndpointA.isIp());
    NTSCFG_TEST_EQ(sourceEndpointA.ip().host(),
                   ntsa::IpAddress::loopbackIpv4());
    NTSCFG_TEST_EQ(sourceEndpointA.ip().port(), 49152);

    // Open another session.

    error = sessionB->open(transport);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint and ensure the operation fails.

    error = sessionB->sourceEndpoint(&sourceEndpointB);
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));

    // Bind the session to the loopback address and request that the
    // assigned port is any available ephemeral port.

    error = sessionB->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint and ensure the session was bound to
    // the loopback address and the second available port in the ephemeral
    // port range.

    error = sessionB->sourceEndpoint(&sourceEndpointB);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_TRUE(sourceEndpointB.isIp());
    NTSCFG_TEST_EQ(sourceEndpointB.ip().host(),
                   ntsa::IpAddress::loopbackIpv4());
    NTSCFG_TEST_EQ(sourceEndpointB.ip().port(), 49152 + 1);

    // Open yet another session.

    error = sessionC->open(transport);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint and ensure the operation fails.

    error = sessionC->sourceEndpoint(&sourceEndpointC);
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));

    // Bind the session to the loopback address and request that the
    // assigned port is any available ephemeral port.

    error = sessionC->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint and ensure the session was bound to
    // the loopback address and the second available port in the ephemeral
    // port range.

    error = sessionC->sourceEndpoint(&sourceEndpointC);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_TRUE(sourceEndpointC.isIp());
    NTSCFG_TEST_EQ(sourceEndpointC.ip().host(),
                   ntsa::IpAddress::loopbackIpv4());
    NTSCFG_TEST_EQ(sourceEndpointC.ip().port(), 49152 + 2);

    // Close the second session.

    error = sessionB->close();
    NTSCFG_TEST_OK(error);

    // Open the second session again.

    error = sessionB->open(transport);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint and ensure the operation fails.

    error = sessionB->sourceEndpoint(&sourceEndpointB);
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));

    // Bind the session to the loopback address and request that the
    // assigned port is any available ephemeral port.

    error = sessionB->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint and ensure the session was bound to
    // the loopback address and the second available port in the ephemeral
    // port range, the port is reused from the "gap".

    error = sessionB->sourceEndpoint(&sourceEndpointB);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_TRUE(sourceEndpointB.isIp());
    NTSCFG_TEST_EQ(sourceEndpointB.ip().host(),
                   ntsa::IpAddress::loopbackIpv4());
    NTSCFG_TEST_EQ(sourceEndpointB.ip().port(), 49152 + 1);

    // Close all sessions.

    error = sessionA->close();
    NTSCFG_TEST_OK(error);

    error = sessionB->close();
    NTSCFG_TEST_OK(error);

    error = sessionC->close();
    NTSCFG_TEST_OK(error);
}

NTSCFG_TEST_FUNCTION(
    ntcd::MachineTest::verifyForegroundBlockingDatagramSockets)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    ntsa::Error error;

    // Create a machine.

    bsl::shared_ptr<ntcd::Machine> machine;
    machine.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Create a client.

    bsl::shared_ptr<ntcd::Session> client =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the client as an IPv4 datagram socket.

    error = client->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    // Bind the client to any port on the IPv4 loopback address.

    error = client->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the client.

    ntsa::Endpoint clientSourceEndpoint;
    error = client->sourceEndpoint(&clientSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Client source endpoint = "
                          << clientSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the client is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(clientSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(clientSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(clientSourceEndpoint.ip().port(), 0);

    // Create a server.

    bsl::shared_ptr<ntcd::Session> server =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the server as an IPv4 datagram socket.

    error = server->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    // Bind the server to any port on the IPv4 loopback address.

    error = server->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the server.

    ntsa::Endpoint serverSourceEndpoint;
    error = server->sourceEndpoint(&serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Server source endpoint = "
                          << serverSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the server is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(serverSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(serverSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(serverSourceEndpoint.ip().port(), 0);

    // Ensure the source endpoint of the client is different than the
    // source endpoint of the server.

    NTSCFG_TEST_NE(clientSourceEndpoint, serverSourceEndpoint);

    // Send data from the client to the server.

    const char CLIENT_DATA = 'C';

    {
        ntsa::Data data(ntsa::ConstBuffer(&CLIENT_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setEndpoint(serverSourceEndpoint);

        error = client->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Advance the simulation.

    error = machine->step(false);
    NTSCFG_TEST_OK(error);

    // Receive data at the server.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, CLIENT_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), clientSourceEndpoint);
    }

    // Send data from the server to the client.

    const char SERVER_DATA = 'S';

    {
        ntsa::Data data(ntsa::ConstBuffer(&SERVER_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setEndpoint(clientSourceEndpoint);

        error = server->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Advance the simulation.

    error = machine->step(false);
    NTSCFG_TEST_OK(error);

    // Receive data at the client.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = client->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, SERVER_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), serverSourceEndpoint);
    }

    // Close the client.

    error = client->close();
    NTSCFG_TEST_OK(error);

    // Close the server.

    error = server->close();
    NTSCFG_TEST_OK(error);
}

NTSCFG_TEST_FUNCTION(
    ntcd::MachineTest::verifyForegroundBlockingDatagramSocketsConnectedOneWay)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    ntsa::Error error;

    // Create a machine.

    bsl::shared_ptr<ntcd::Machine> machine;
    machine.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Create a client.

    bsl::shared_ptr<ntcd::Session> client =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the client as an IPv4 datagram socket.

    error = client->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    // Bind the client to any port on the IPv4 loopback address.

    error = client->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the client.

    ntsa::Endpoint clientSourceEndpoint;
    error = client->sourceEndpoint(&clientSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Client source endpoint = "
                          << clientSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the client is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(clientSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(clientSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(clientSourceEndpoint.ip().port(), 0);

    // Create a server.

    bsl::shared_ptr<ntcd::Session> server =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the server as an IPv4 datagram socket.

    error = server->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    // Bind the server to any port on the IPv4 loopback address.

    error = server->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the server.

    ntsa::Endpoint serverSourceEndpoint;
    error = server->sourceEndpoint(&serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Server source endpoint = "
                          << serverSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the server is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(serverSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(serverSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(serverSourceEndpoint.ip().port(), 0);

    // Ensure the source endpoint of the client is different than the
    // source endpoint of the server.

    NTSCFG_TEST_NE(clientSourceEndpoint, serverSourceEndpoint);

    // Connect the client to the server.

    error = client->connect(serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Advance the simulation.

    error = machine->step(false);
    NTSCFG_TEST_OK(error);

    // Send data from the client to the server.

    const char CLIENT_DATA = 'C';

    {
        ntsa::Data data(ntsa::ConstBuffer(&CLIENT_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = client->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Advance the simulation.

    error = machine->step(false);
    NTSCFG_TEST_OK(error);

    // Receive data at the server.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, CLIENT_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), clientSourceEndpoint);
    }

    // Send data from the server to the client.

    const char SERVER_DATA = 'S';

    {
        ntsa::Data data(ntsa::ConstBuffer(&SERVER_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setEndpoint(clientSourceEndpoint);

        error = server->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Advance the simulation.

    error = machine->step(false);
    NTSCFG_TEST_OK(error);

    // Receive data at the client.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = client->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, SERVER_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), serverSourceEndpoint);
    }

    // Close the client.

    error = client->close();
    NTSCFG_TEST_OK(error);

    // Close the server.

    error = server->close();
    NTSCFG_TEST_OK(error);
}

NTSCFG_TEST_FUNCTION(
    ntcd::MachineTest::verifyForegroundBlockingDatagramSocketsConnectedTwoWay)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    ntsa::Error error;

    // Create a machine.

    bsl::shared_ptr<ntcd::Machine> machine;
    machine.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Create a client.

    bsl::shared_ptr<ntcd::Session> client =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the client as an IPv4 datagram socket.

    error = client->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    // Bind the client to any port on the IPv4 loopback address.

    error = client->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the client.

    ntsa::Endpoint clientSourceEndpoint;
    error = client->sourceEndpoint(&clientSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Client source endpoint = "
                          << clientSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the client is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(clientSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(clientSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(clientSourceEndpoint.ip().port(), 0);

    // Create a server.

    bsl::shared_ptr<ntcd::Session> server =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the server as an IPv4 datagram socket.

    error = server->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    // Bind the server to any port on the IPv4 loopback address.

    error = server->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the server.

    ntsa::Endpoint serverSourceEndpoint;
    error = server->sourceEndpoint(&serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Server source endpoint = "
                          << serverSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the server is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(serverSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(serverSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(serverSourceEndpoint.ip().port(), 0);

    // Ensure the source endpoint of the client is different than the
    // source endpoint of the server.

    NTSCFG_TEST_NE(clientSourceEndpoint, serverSourceEndpoint);

    // Connect the client to the server.

    error = client->connect(serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Connect the server to the client.

    error = server->connect(clientSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Advance the simulation.

    error = machine->step(false);
    NTSCFG_TEST_OK(error);

    // Send data from the client to the server.

    const char CLIENT_DATA = 'C';

    {
        ntsa::Data data(ntsa::ConstBuffer(&CLIENT_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = client->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Advance the simulation.

    error = machine->step(false);
    NTSCFG_TEST_OK(error);

    // Receive data at the server.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, CLIENT_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), clientSourceEndpoint);
    }

    // Send data from the server to the client.

    const char SERVER_DATA = 'S';

    {
        ntsa::Data data(ntsa::ConstBuffer(&SERVER_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = server->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Advance the simulation.

    error = machine->step(false);
    NTSCFG_TEST_OK(error);

    // Receive data at the client.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = client->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, SERVER_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), serverSourceEndpoint);
    }

    // Close the client.

    error = client->close();
    NTSCFG_TEST_OK(error);

    // Close the server.

    error = server->close();
    NTSCFG_TEST_OK(error);
}

NTSCFG_TEST_FUNCTION(ntcd::MachineTest::verifyForegroundBlockingStreamSockets)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    ntsa::Error error;

    // Create a machine.

    bsl::shared_ptr<ntcd::Machine> machine;
    machine.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Create a listener.

    bsl::shared_ptr<ntcd::Session> listener =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the listener for IPv4 stream sockets.

    error = error = listener->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    // Bind the listener to any port on the IPv4 loopback address.

    error = listener->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the listener.

    ntsa::Endpoint listenerSourceEndpoint;
    error = listener->sourceEndpoint(&listenerSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Listener source endpoint = "
                          << listenerSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the listener is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(listenerSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(listenerSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(listenerSourceEndpoint.ip().port(), 0);

    // Begin listening for connections.

    error = listener->listen(0);
    NTSCFG_TEST_OK(error);

    // Create a client.

    bsl::shared_ptr<ntcd::Session> client =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the client as an IPv4 stream socket.

    error = client->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    // Connect the client to the listener.

    error = client->connect(listenerSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Advance the simulation.

    error = machine->step(false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the client.

    ntsa::Endpoint clientSourceEndpoint;
    error = client->sourceEndpoint(&clientSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Client source endpoint = "
                          << clientSourceEndpoint << NTCI_LOG_STREAM_END;

    // Get the remote endpoint of the client.

    ntsa::Endpoint clientRemoteEndpoint;
    error = client->remoteEndpoint(&clientRemoteEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Client remote endpoint = "
                          << clientRemoteEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the client is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(clientSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(clientSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(clientSourceEndpoint.ip().port(), 0);

    // Accept a server from the listener.

    bsl::shared_ptr<ntcd::Session> server;
    error = listener->accept(&server);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the server.

    ntsa::Endpoint serverSourceEndpoint;
    error = server->sourceEndpoint(&serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Server source endpoint = "
                          << serverSourceEndpoint << NTCI_LOG_STREAM_END;

    // Get the remote endpoint of the server.

    ntsa::Endpoint serverRemoteEndpoint;
    error = server->remoteEndpoint(&serverRemoteEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Server remote endpoint = "
                          << serverRemoteEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the server is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(serverSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(serverSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(serverSourceEndpoint.ip().port(), 0);

    // Ensure the remote endpoint of the client is the source endpoint of
    // the server.

    NTSCFG_TEST_EQ(clientRemoteEndpoint, serverSourceEndpoint);

    // Ensure the remote endpoint of the server is the source endpoint of
    // the client.

    NTSCFG_TEST_EQ(serverRemoteEndpoint, clientSourceEndpoint);

    // Send data from the client to the server.

    const char CLIENT_DATA = 'C';

    {
        ntsa::Data data(ntsa::ConstBuffer(&CLIENT_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = client->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Advance the simulation.

    error = machine->step(false);
    NTSCFG_TEST_OK(error);

    // Receive data at the server.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, CLIENT_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), clientSourceEndpoint);
    }

    // Send data from the server to the client.

    const char SERVER_DATA = 'S';

    {
        ntsa::Data data(ntsa::ConstBuffer(&SERVER_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = server->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Advance the simulation.

    error = machine->step(false);
    NTSCFG_TEST_OK(error);

    // Receive data at the client.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = client->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, SERVER_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), serverSourceEndpoint);
    }

    // Shutdown the client.

    error = client->shutdown(ntsa::ShutdownType::e_SEND);
    NTSCFG_TEST_OK(error);

    // Advance the simulation.

    error = machine->step(false);
    NTSCFG_TEST_OK(error);

    // Receive data at the server.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &data, options);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_EOF));
        NTSCFG_TEST_EQ(context.bytesReceived(), 0);
    }

    // Shutdown the server.

    error = server->shutdown(ntsa::ShutdownType::e_SEND);
    NTSCFG_TEST_OK(error);

    // Advance the simulation.

    error = machine->step(false);
    NTSCFG_TEST_OK(error);

    // Receive data at the client.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = client->receive(&context, &data, options);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_EOF));
        NTSCFG_TEST_EQ(context.bytesReceived(), 0);
    }

    // Try to send data from the client to the server and ensure the
    // operation fails because the client has been shutdown for sending.

    {
        ntsa::Data data(ntsa::ConstBuffer(&CLIENT_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = client->send(&context, data, options);
        NTSCFG_TEST_TRUE(error);

        NTSCFG_TEST_EQ(context.bytesSent(), 0);
    }

    // Try to send data from the server to the client and ensure the
    // operation fails because the server has been shutdown for sending.

    {
        ntsa::Data data(ntsa::ConstBuffer(&SERVER_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = server->send(&context, data, options);
        NTSCFG_TEST_TRUE(error);

        NTSCFG_TEST_EQ(context.bytesSent(), 0);
    }

    // Close the client.

    error = client->close();
    NTSCFG_TEST_OK(error);

    // Close the server.

    error = server->close();
    NTSCFG_TEST_OK(error);

    // Close the listener.

    error = listener->close();
    NTSCFG_TEST_OK(error);
}

NTSCFG_TEST_FUNCTION(
    ntcd::MachineTest::verifyBackgroundBlockingDatagramSockets)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    ntsa::Error error;

    // Create a machine.

    bsl::shared_ptr<ntcd::Machine> machine;
    machine.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Run the machine.

    error = machine->run();
    NTSCFG_TEST_OK(error);

    // Create a client.

    bsl::shared_ptr<ntcd::Session> client =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the client as an IPv4 datagram socket.

    error = client->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    // Bind the client to any port on the IPv4 loopback address.

    error = client->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the client.

    ntsa::Endpoint clientSourceEndpoint;
    error = client->sourceEndpoint(&clientSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Client source endpoint = "
                          << clientSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the client is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(clientSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(clientSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(clientSourceEndpoint.ip().port(), 0);

    // Create a server.

    bsl::shared_ptr<ntcd::Session> server =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the server as an IPv4 datagram socket.

    error = server->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    // Bind the server to any port on the IPv4 loopback address.

    error = server->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the server.

    ntsa::Endpoint serverSourceEndpoint;
    error = server->sourceEndpoint(&serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Server source endpoint = "
                          << serverSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the server is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(serverSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(serverSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(serverSourceEndpoint.ip().port(), 0);

    // Ensure the source endpoint of the client is different than the
    // source endpoint of the server.

    NTSCFG_TEST_NE(clientSourceEndpoint, serverSourceEndpoint);

    // Send data from the client to the server.

    const char CLIENT_DATA = 'C';

    {
        ntsa::Data data(ntsa::ConstBuffer(&CLIENT_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setEndpoint(serverSourceEndpoint);

        error = client->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Receive data at the server.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, CLIENT_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), clientSourceEndpoint);
    }

    // Send data from the server to the client.

    const char SERVER_DATA = 'S';

    {
        ntsa::Data data(ntsa::ConstBuffer(&SERVER_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setEndpoint(clientSourceEndpoint);

        error = server->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Receive data at the client.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = client->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, SERVER_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), serverSourceEndpoint);
    }

    // Close the client.

    error = client->close();
    NTSCFG_TEST_OK(error);

    // Close the server.

    error = server->close();
    NTSCFG_TEST_OK(error);

    // Stop the machine.

    machine->stop();
}

NTSCFG_TEST_FUNCTION(
    ntcd::MachineTest::verifyBackgroundBlockingDatagramSocketsConnectedOneWay)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    ntsa::Error error;

    // Create a machine.

    bsl::shared_ptr<ntcd::Machine> machine;
    machine.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Run the machine.

    error = machine->run();
    NTSCFG_TEST_OK(error);

    // Create a client.

    bsl::shared_ptr<ntcd::Session> client =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the client as an IPv4 datagram socket.

    error = client->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    // Bind the client to any port on the IPv4 loopback address.

    error = client->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the client.

    ntsa::Endpoint clientSourceEndpoint;
    error = client->sourceEndpoint(&clientSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Client source endpoint = "
                          << clientSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the client is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(clientSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(clientSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(clientSourceEndpoint.ip().port(), 0);

    // Create a server.

    bsl::shared_ptr<ntcd::Session> server =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the server as an IPv4 datagram socket.

    error = server->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    // Bind the server to any port on the IPv4 loopback address.

    error = server->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the server.

    ntsa::Endpoint serverSourceEndpoint;
    error = server->sourceEndpoint(&serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Server source endpoint = "
                          << serverSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the server is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(serverSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(serverSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(serverSourceEndpoint.ip().port(), 0);

    // Ensure the source endpoint of the client is different than the
    // source endpoint of the server.

    NTSCFG_TEST_NE(clientSourceEndpoint, serverSourceEndpoint);

    // Connect the client to the server.

    error = client->connect(serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Send data from the client to the server.

    const char CLIENT_DATA = 'C';

    {
        ntsa::Data data(ntsa::ConstBuffer(&CLIENT_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = client->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Receive data at the server.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, CLIENT_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), clientSourceEndpoint);
    }

    // Send data from the server to the client.

    const char SERVER_DATA = 'S';

    {
        ntsa::Data data(ntsa::ConstBuffer(&SERVER_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setEndpoint(clientSourceEndpoint);

        error = server->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Receive data at the client.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = client->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, SERVER_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), serverSourceEndpoint);
    }

    // Close the client.

    error = client->close();
    NTSCFG_TEST_OK(error);

    // Close the server.

    error = server->close();
    NTSCFG_TEST_OK(error);

    // Stop the machine.

    machine->stop();
}

NTSCFG_TEST_FUNCTION(
    ntcd::MachineTest::verifyBackgroundBlockingDatagramSocketsConnectedTwoWay)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    ntsa::Error error;

    // Create a machine.

    bsl::shared_ptr<ntcd::Machine> machine;
    machine.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Run the machine.

    error = machine->run();
    NTSCFG_TEST_OK(error);

    // Create a client.

    bsl::shared_ptr<ntcd::Session> client =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the client as an IPv4 datagram socket.

    error = client->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    // Bind the client to any port on the IPv4 loopback address.

    error = client->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the client.

    ntsa::Endpoint clientSourceEndpoint;
    error = client->sourceEndpoint(&clientSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Client source endpoint = "
                          << clientSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the client is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(clientSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(clientSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(clientSourceEndpoint.ip().port(), 0);

    // Create a server.

    bsl::shared_ptr<ntcd::Session> server =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the server as an IPv4 datagram socket.

    error = server->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    // Bind the server to any port on the IPv4 loopback address.

    error = server->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the server.

    ntsa::Endpoint serverSourceEndpoint;
    error = server->sourceEndpoint(&serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Server source endpoint = "
                          << serverSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the server is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(serverSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(serverSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(serverSourceEndpoint.ip().port(), 0);

    // Ensure the source endpoint of the client is different than the
    // source endpoint of the server.

    NTSCFG_TEST_NE(clientSourceEndpoint, serverSourceEndpoint);

    // Connect the client to the server.

    error = client->connect(serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Connect the server to the client.

    error = server->connect(clientSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Send data from the client to the server.

    const char CLIENT_DATA = 'C';

    {
        ntsa::Data data(ntsa::ConstBuffer(&CLIENT_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = client->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Receive data at the server.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, CLIENT_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), clientSourceEndpoint);
    }

    // Send data from the server to the client.

    const char SERVER_DATA = 'S';

    {
        ntsa::Data data(ntsa::ConstBuffer(&SERVER_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = server->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Receive data at the client.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = client->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, SERVER_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), serverSourceEndpoint);
    }

    // Close the client.

    error = client->close();
    NTSCFG_TEST_OK(error);

    // Close the server.

    error = server->close();
    NTSCFG_TEST_OK(error);

    // Stop the machine.

    machine->stop();
}

NTSCFG_TEST_FUNCTION(ntcd::MachineTest::verifyBackgroundBlockingStreamSockets)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    ntsa::Error error;

    // Create a machine.

    bsl::shared_ptr<ntcd::Machine> machine;
    machine.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Run the machine.

    error = machine->run();
    NTSCFG_TEST_OK(error);

    // Create a listener.

    bsl::shared_ptr<ntcd::Session> listener =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the listener for IPv4 stream sockets.

    error = error = listener->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    // Bind the listener to any port on the IPv4 loopback address.

    error = listener->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the listener.

    ntsa::Endpoint listenerSourceEndpoint;
    error = listener->sourceEndpoint(&listenerSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Listener source endpoint = "
                          << listenerSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the listener is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(listenerSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(listenerSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(listenerSourceEndpoint.ip().port(), 0);

    // Begin listening for connections.

    error = listener->listen(0);
    NTSCFG_TEST_OK(error);

    // Create a client.

    bsl::shared_ptr<ntcd::Session> client =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the client as an IPv4 stream socket.

    error = client->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    // Connect the client to the listener.

    error = client->connect(listenerSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the client.

    ntsa::Endpoint clientSourceEndpoint;
    error = client->sourceEndpoint(&clientSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Client source endpoint = "
                          << clientSourceEndpoint << NTCI_LOG_STREAM_END;

    // Get the remote endpoint of the client.

    ntsa::Endpoint clientRemoteEndpoint;
    error = client->remoteEndpoint(&clientRemoteEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Client remote endpoint = "
                          << clientRemoteEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the client is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(clientSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(clientSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(clientSourceEndpoint.ip().port(), 0);

    // Accept a server from the listener.

    bsl::shared_ptr<ntcd::Session> server;
    error = listener->accept(&server);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the server.

    ntsa::Endpoint serverSourceEndpoint;
    error = server->sourceEndpoint(&serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Server source endpoint = "
                          << serverSourceEndpoint << NTCI_LOG_STREAM_END;

    // Get the remote endpoint of the server.

    ntsa::Endpoint serverRemoteEndpoint;
    error = server->remoteEndpoint(&serverRemoteEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Server remote endpoint = "
                          << serverRemoteEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the server is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(serverSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(serverSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(serverSourceEndpoint.ip().port(), 0);

    // Ensure the remote endpoint of the client is the source endpoint of
    // the server.

    NTSCFG_TEST_EQ(clientRemoteEndpoint, serverSourceEndpoint);

    // Ensure the remote endpoint of the server is the source endpoint of
    // the client.

    NTSCFG_TEST_EQ(serverRemoteEndpoint, clientSourceEndpoint);

    // Send data from the client to the server.

    const char CLIENT_DATA = 'C';

    {
        ntsa::Data data(ntsa::ConstBuffer(&CLIENT_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = client->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Receive data at the server.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, CLIENT_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), clientSourceEndpoint);
    }

    // Send data from the server to the client.

    const char SERVER_DATA = 'S';

    {
        ntsa::Data data(ntsa::ConstBuffer(&SERVER_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = server->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Receive data at the client.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = client->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, SERVER_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), serverSourceEndpoint);
    }

    // Shutdown the client.

    error = client->shutdown(ntsa::ShutdownType::e_SEND);
    NTSCFG_TEST_OK(error);

    // Receive data at the server.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &data, options);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_EOF));
        NTSCFG_TEST_EQ(context.bytesReceived(), 0);
    }

    // Shutdown the server.

    error = server->shutdown(ntsa::ShutdownType::e_SEND);
    NTSCFG_TEST_OK(error);

    // Receive data at the client.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = client->receive(&context, &data, options);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_EOF));
        NTSCFG_TEST_EQ(context.bytesReceived(), 0);
    }

    // Try to send data from the client to the server and ensure the
    // operation fails because the client has been shutdown for sending.

    {
        ntsa::Data data(ntsa::ConstBuffer(&CLIENT_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = client->send(&context, data, options);
        NTSCFG_TEST_TRUE(error);

        NTSCFG_TEST_EQ(context.bytesSent(), 0);
    }

    // Try to send data from the server to the client and ensure the
    // operation fails because the server has been shutdown for sending.

    {
        ntsa::Data data(ntsa::ConstBuffer(&SERVER_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = server->send(&context, data, options);
        NTSCFG_TEST_TRUE(error);

        NTSCFG_TEST_EQ(context.bytesSent(), 0);
    }

    // Close the client.

    error = client->close();
    NTSCFG_TEST_OK(error);

    // Close the server.

    error = server->close();
    NTSCFG_TEST_OK(error);

    // Close the listener.

    error = listener->close();
    NTSCFG_TEST_OK(error);

    // Stop the machine.

    machine->stop();
}

NTSCFG_TEST_FUNCTION(
    ntcd::MachineTest::verifyBackgroundNonblockingDatagramSockets)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    ntsa::Error error;

    // Create a machine.

    bsl::shared_ptr<ntcd::Machine> machine;
    machine.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Run the machine.

    error = machine->run();
    NTSCFG_TEST_OK(error);

    // Create a monitor.

    bsl::shared_ptr<ntcd::Monitor> monitor =
        machine->createMonitor(NTSCFG_TEST_ALLOCATOR);

    // Create a client.

    bsl::shared_ptr<ntcd::Session> client =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the client as an IPv4 datagram socket.

    error = client->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    error = client->setBlocking(false);
    NTSCFG_TEST_OK(error);

    // Bind the client to any port on the IPv4 loopback address.

    error = client->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the client.

    ntsa::Endpoint clientSourceEndpoint;
    error = client->sourceEndpoint(&clientSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Client source endpoint = "
                          << clientSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the client is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(clientSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(clientSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(clientSourceEndpoint.ip().port(), 0);

    // Create a server.

    bsl::shared_ptr<ntcd::Session> server =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the server as an IPv4 datagram socket.

    error = server->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    error = server->setBlocking(false);
    NTSCFG_TEST_OK(error);

    // Bind the server to any port on the IPv4 loopback address.

    error = server->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the server.

    ntsa::Endpoint serverSourceEndpoint;
    error = server->sourceEndpoint(&serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Server source endpoint = "
                          << serverSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the server is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(serverSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(serverSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(serverSourceEndpoint.ip().port(), 0);

    // Ensure the source endpoint of the client is different than the
    // source endpoint of the server.

    NTSCFG_TEST_NE(clientSourceEndpoint, serverSourceEndpoint);

    // Add the client to the monitor.

    error = monitor->add(client);
    NTSCFG_TEST_OK(error);

    // Add the server to the monitor.

    error = monitor->add(server);
    NTSCFG_TEST_OK(error);

    // Gain interest in writability for the client.

    error = monitor->show(client, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Block until the client is writable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == client->handle() && event.isWritable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Send data from the client to the server.

    const char CLIENT_DATA = 'C';

    {
        ntsa::Data data(ntsa::ConstBuffer(&CLIENT_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setEndpoint(serverSourceEndpoint);

        error = client->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Lose interest in writability for the client.

    error = monitor->hide(client, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Gain interest in readability for the server.

    error = monitor->show(server, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Block until the server is readable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == server->handle() && event.isReadable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Receive data at the server.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, CLIENT_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), clientSourceEndpoint);
    }

    // Lose interest in readability for the server.

    error = monitor->hide(server, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Gain interest in writability for the server.

    error = monitor->show(server, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Block until the server is writable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == server->handle() && event.isWritable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Send data from the server to the client.

    const char SERVER_DATA = 'S';

    {
        ntsa::Data data(ntsa::ConstBuffer(&SERVER_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setEndpoint(clientSourceEndpoint);

        error = server->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Lose interest in writability for the server.

    error = monitor->hide(server, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Gain interest in readability for the client.

    error = monitor->show(client, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Block until the client is readable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == client->handle() && event.isReadable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Receive data at the client.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = client->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, SERVER_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), serverSourceEndpoint);
    }

    // Lose interest in readabilty for the client.

    error = monitor->hide(client, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Remove the client from the monitor.

    error = monitor->remove(client);
    NTSCFG_TEST_OK(error);

    // Remove the server from the monitor.

    error = monitor->remove(server);
    NTSCFG_TEST_OK(error);

    // Close the client.

    error = client->close();
    NTSCFG_TEST_OK(error);

    // Close the server.

    error = server->close();
    NTSCFG_TEST_OK(error);

    // Stop the machine.

    machine->stop();
}

NTSCFG_TEST_FUNCTION(
    ntcd::MachineTest::
        verifyBackgroundNonblockingDatagramSocketsConnectedOneWay)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    ntsa::Error error;

    // Create a machine.

    bsl::shared_ptr<ntcd::Machine> machine;
    machine.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Run the machine.

    error = machine->run();
    NTSCFG_TEST_OK(error);

    // Create a monitor.

    bsl::shared_ptr<ntcd::Monitor> monitor =
        machine->createMonitor(NTSCFG_TEST_ALLOCATOR);

    // Create a client.

    bsl::shared_ptr<ntcd::Session> client =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the client as an IPv4 datagram socket.

    error = client->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    error = client->setBlocking(false);
    NTSCFG_TEST_OK(error);

    // Bind the client to any port on the IPv4 loopback address.

    error = client->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the client.

    ntsa::Endpoint clientSourceEndpoint;
    error = client->sourceEndpoint(&clientSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Client source endpoint = "
                          << clientSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the client is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(clientSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(clientSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(clientSourceEndpoint.ip().port(), 0);

    // Create a server.

    bsl::shared_ptr<ntcd::Session> server =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the server as an IPv4 datagram socket.

    error = server->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    error = server->setBlocking(false);
    NTSCFG_TEST_OK(error);

    // Bind the server to any port on the IPv4 loopback address.

    error = server->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the server.

    ntsa::Endpoint serverSourceEndpoint;
    error = server->sourceEndpoint(&serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Server source endpoint = "
                          << serverSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the server is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(serverSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(serverSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(serverSourceEndpoint.ip().port(), 0);

    // Ensure the source endpoint of the client is different than the
    // source endpoint of the server.

    NTSCFG_TEST_NE(clientSourceEndpoint, serverSourceEndpoint);

    // Connect the client to the server.

    error = client->connect(serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Add the client to the monitor.

    error = monitor->add(client);
    NTSCFG_TEST_OK(error);

    // Add the server to the monitor.

    error = monitor->add(server);
    NTSCFG_TEST_OK(error);

    // Gain interest in writability for the client.

    error = monitor->show(client, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Block until the client is writable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == client->handle() && event.isWritable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Send data from the client to the server.

    const char CLIENT_DATA = 'C';

    {
        ntsa::Data data(ntsa::ConstBuffer(&CLIENT_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = client->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Lose interest in writability for the client.

    error = monitor->hide(client, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Gain interest in readability for the server.

    error = monitor->show(server, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Block until the server is readable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == server->handle() && event.isReadable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Receive data at the server.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, CLIENT_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), clientSourceEndpoint);
    }

    // Lose interest in readability for the server.

    error = monitor->hide(server, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Gain interest in writability for the server.

    error = monitor->show(server, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Block until the server is writable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == server->handle() && event.isWritable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Send data from the server to the client.

    const char SERVER_DATA = 'S';

    {
        ntsa::Data data(ntsa::ConstBuffer(&SERVER_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setEndpoint(clientSourceEndpoint);

        error = server->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Lose interest in writability for the server.

    error = monitor->hide(server, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Gain interest in readability for the client.

    error = monitor->show(client, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Block until the client is readable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == client->handle() && event.isReadable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Receive data at the client.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = client->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, SERVER_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), serverSourceEndpoint);
    }

    // Lose interest in readabilty for the client.

    error = monitor->hide(client, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Remove the client from the monitor.

    error = monitor->remove(client);
    NTSCFG_TEST_OK(error);

    // Remove the server from the monitor.

    error = monitor->remove(server);
    NTSCFG_TEST_OK(error);

    // Close the client.

    error = client->close();
    NTSCFG_TEST_OK(error);

    // Close the server.

    error = server->close();
    NTSCFG_TEST_OK(error);

    // Stop the machine.

    machine->stop();
}

NTSCFG_TEST_FUNCTION(
    ntcd::MachineTest::
        verifyBackgroundNonblockingDatagramSocketsConnectedTwoWay)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    ntsa::Error error;

    // Create a machine.

    bsl::shared_ptr<ntcd::Machine> machine;
    machine.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Run the machine.

    error = machine->run();
    NTSCFG_TEST_OK(error);

    // Create a monitor.

    bsl::shared_ptr<ntcd::Monitor> monitor =
        machine->createMonitor(NTSCFG_TEST_ALLOCATOR);

    // Create a client.

    bsl::shared_ptr<ntcd::Session> client =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the client as an IPv4 datagram socket.

    error = client->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    error = client->setBlocking(false);
    NTSCFG_TEST_OK(error);

    // Bind the client to any port on the IPv4 loopback address.

    error = client->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the client.

    ntsa::Endpoint clientSourceEndpoint;
    error = client->sourceEndpoint(&clientSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Client source endpoint = "
                          << clientSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the client is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(clientSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(clientSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(clientSourceEndpoint.ip().port(), 0);

    // Create a server.

    bsl::shared_ptr<ntcd::Session> server =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the server as an IPv4 datagram socket.

    error = server->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    NTSCFG_TEST_OK(error);

    error = server->setBlocking(false);
    NTSCFG_TEST_OK(error);

    // Bind the server to any port on the IPv4 loopback address.

    error = server->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the server.

    ntsa::Endpoint serverSourceEndpoint;
    error = server->sourceEndpoint(&serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Server source endpoint = "
                          << serverSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the server is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(serverSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(serverSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(serverSourceEndpoint.ip().port(), 0);

    // Ensure the source endpoint of the client is different than the
    // source endpoint of the server.

    NTSCFG_TEST_NE(clientSourceEndpoint, serverSourceEndpoint);

    // Connect the client to the server.

    error = client->connect(serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Connect the server to the client.

    error = server->connect(clientSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Add the client to the monitor.

    error = monitor->add(client);
    NTSCFG_TEST_OK(error);

    // Add the server to the monitor.

    error = monitor->add(server);
    NTSCFG_TEST_OK(error);

    // Gain interest in writability for the client.

    error = monitor->show(client, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Block until the client is writable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == client->handle() && event.isWritable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Send data from the client to the server.

    const char CLIENT_DATA = 'C';

    {
        ntsa::Data data(ntsa::ConstBuffer(&CLIENT_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = client->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Lose interest in writability for the client.

    error = monitor->hide(client, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Gain interest in readability for the server.

    error = monitor->show(server, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Block until the server is readable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == server->handle() && event.isReadable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Receive data at the server.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, CLIENT_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), clientSourceEndpoint);
    }

    // Lose interest in readability for the server.

    error = monitor->hide(server, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Gain interest in writability for the server.

    error = monitor->show(server, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Block until the server is writable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == server->handle() && event.isWritable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Send data from the server to the client.

    const char SERVER_DATA = 'S';

    {
        ntsa::Data data(ntsa::ConstBuffer(&SERVER_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = server->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Lose interest in writability for the server.

    error = monitor->hide(server, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Gain interest in readability for the client.

    error = monitor->show(client, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Block until the client is readable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == client->handle() && event.isReadable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Receive data at the client.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = client->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, SERVER_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), serverSourceEndpoint);
    }

    // Lose interest in readabilty for the client.

    error = monitor->hide(client, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Remove the client from the monitor.

    error = monitor->remove(client);
    NTSCFG_TEST_OK(error);

    // Remove the server from the monitor.

    error = monitor->remove(server);
    NTSCFG_TEST_OK(error);

    // Close the client.

    error = client->close();
    NTSCFG_TEST_OK(error);

    // Close the server.

    error = server->close();
    NTSCFG_TEST_OK(error);

    // Stop the machine.

    machine->stop();
}

NTSCFG_TEST_FUNCTION(
    ntcd::MachineTest::verifyBackgroundNonblockingStreamSockets)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    ntsa::Error error;

    // Create a machine.

    bsl::shared_ptr<ntcd::Machine> machine;
    machine.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Run the machine.

    error = machine->run();
    NTSCFG_TEST_OK(error);

    // Create a monitor.

    bsl::shared_ptr<ntcd::Monitor> monitor =
        machine->createMonitor(NTSCFG_TEST_ALLOCATOR);

    // Create a listener.

    bsl::shared_ptr<ntcd::Session> listener =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the listener for IPv4 stream sockets.

    error = error = listener->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    error = listener->setBlocking(false);
    NTSCFG_TEST_OK(error);

    // Bind the listener to any port on the IPv4 loopback address.

    error = listener->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the listener.

    ntsa::Endpoint listenerSourceEndpoint;
    error = listener->sourceEndpoint(&listenerSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Listener source endpoint = "
                          << listenerSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the listener is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(listenerSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(listenerSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(listenerSourceEndpoint.ip().port(), 0);

    // Begin listening for connections.

    error = listener->listen(0);
    NTSCFG_TEST_OK(error);

    // Add the listener to the monitor.

    error = monitor->add(listener);
    NTSCFG_TEST_OK(error);

    // Create a client.

    bsl::shared_ptr<ntcd::Session> client =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the client as an IPv4 stream socket.

    error = client->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    error = client->setBlocking(false);
    NTSCFG_TEST_OK(error);

    // Connect the client to the listener.

    error = client->connect(listenerSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Add the client to the monitor.

    error = monitor->add(client);
    NTSCFG_TEST_OK(error);

    // Gain interest in writability for the client.

    error = monitor->show(client, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Block until the client is writable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == client->handle() && event.isWritable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Lose interest in writability for the client.

    error = monitor->hide(client, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the client.

    ntsa::Endpoint clientSourceEndpoint;
    error = client->sourceEndpoint(&clientSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Client source endpoint = "
                          << clientSourceEndpoint << NTCI_LOG_STREAM_END;

    // Get the remote endpoint of the client.

    ntsa::Endpoint clientRemoteEndpoint;
    error = client->remoteEndpoint(&clientRemoteEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Client remote endpoint = "
                          << clientRemoteEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the client is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(clientSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(clientSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(clientSourceEndpoint.ip().port(), 0);

    // Gain interest in readability for the listener.

    error = monitor->show(listener, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Block until the listener is readable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == listener->handle() && event.isReadable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Accept a server from the listener.

    bsl::shared_ptr<ntcd::Session> server;
    error = listener->accept(&server);
    NTSCFG_TEST_OK(error);

    error = server->setBlocking(false);
    NTSCFG_TEST_OK(error);

    // Lose interest in readability for the listener.

    error = monitor->hide(listener, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the server.

    ntsa::Endpoint serverSourceEndpoint;
    error = server->sourceEndpoint(&serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Server source endpoint = "
                          << serverSourceEndpoint << NTCI_LOG_STREAM_END;

    // Get the remote endpoint of the server.

    ntsa::Endpoint serverRemoteEndpoint;
    error = server->remoteEndpoint(&serverRemoteEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Server remote endpoint = "
                          << serverRemoteEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the server is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(serverSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(serverSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(serverSourceEndpoint.ip().port(), 0);

    // Ensure the remote endpoint of the client is the source endpoint of
    // the server.

    NTSCFG_TEST_EQ(clientRemoteEndpoint, serverSourceEndpoint);

    // Ensure the remote endpoint of the server is the source endpoint of
    // the client.

    NTSCFG_TEST_EQ(serverRemoteEndpoint, clientSourceEndpoint);

    // Add the server to the monitor.

    error = monitor->add(server);
    NTSCFG_TEST_OK(error);

    // Gain interest in writability for the client.

    error = monitor->show(client, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Block until the client is writable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == client->handle() && event.isWritable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Send data from the client to the server.

    const char CLIENT_DATA = 'C';

    {
        ntsa::Data data(ntsa::ConstBuffer(&CLIENT_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = client->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Lose interest in writability for the client.

    error = monitor->hide(client, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Gain interest in readability for the server.

    error = monitor->show(server, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Block until the server is readable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == server->handle() && event.isReadable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Receive data at the server.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, CLIENT_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), clientSourceEndpoint);
    }

    // Lose interest in readability for the server.

    error = monitor->hide(server, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Gain interest in writability for the server.

    error = monitor->show(server, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Block until the server is writable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == server->handle() && event.isWritable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Send data from the server to the client.

    const char SERVER_DATA = 'S';

    {
        ntsa::Data data(ntsa::ConstBuffer(&SERVER_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = server->send(&context, data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesSendable(), 1);
        NTSCFG_TEST_EQ(context.bytesSent(), 1);
    }

    // Lose interest in writability for the server.

    error = monitor->hide(server, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Gain interest in readability for the client.

    error = monitor->show(client, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Block until the client is readable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == client->handle() && event.isReadable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Receive data at the client.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = client->receive(&context, &data, options);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
        NTSCFG_TEST_EQ(context.bytesReceived(), 1);

        NTSCFG_TEST_EQ(remoteData, SERVER_DATA);

        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), serverSourceEndpoint);
    }

    // Lose interest in readabilty for the client.

    error = monitor->hide(client, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Shutdown the client.

    error = client->shutdown(ntsa::ShutdownType::e_SEND);
    NTSCFG_TEST_OK(error);

    // Gain interest in readability for the server.

    error = monitor->show(server, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Block until the server is readable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == server->handle() && event.isReadable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Receive data at the server.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &data, options);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_EOF));
        NTSCFG_TEST_EQ(context.bytesReceived(), 0);
    }

    // Lose interest in readability for the server.

    error = monitor->hide(server, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Shutdown the server.

    error = server->shutdown(ntsa::ShutdownType::e_SEND);
    NTSCFG_TEST_OK(error);

    // Gain interest in readability for the client.

    error = monitor->show(client, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Block until the client is readable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == client->handle() && event.isReadable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Receive data at the client.

    {
        char remoteData = 0;

        ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = client->receive(&context, &data, options);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_EOF));
        NTSCFG_TEST_EQ(context.bytesReceived(), 0);
    }

    // Lose interest in readabilty for the client.

    error = monitor->hide(client, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Try to send data from the client to the server and ensure the
    // operation fails because the client has been shutdown for sending.

    {
        ntsa::Data data(ntsa::ConstBuffer(&CLIENT_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = client->send(&context, data, options);
        NTSCFG_TEST_TRUE(error);

        NTSCFG_TEST_EQ(context.bytesSent(), 0);
    }

    // Try to send data from the server to the client and ensure the
    // operation fails because the server has been shutdown for sending.

    {
        ntsa::Data data(ntsa::ConstBuffer(&SERVER_DATA, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = server->send(&context, data, options);
        NTSCFG_TEST_TRUE(error);

        NTSCFG_TEST_EQ(context.bytesSent(), 0);
    }

    // Remove the client from the monitor.

    error = monitor->remove(client);
    NTSCFG_TEST_OK(error);

    // Remove the server from the monitor.

    error = monitor->remove(server);
    NTSCFG_TEST_OK(error);

    // Remove the listener from the monitor.

    error = monitor->remove(listener);
    NTSCFG_TEST_OK(error);

    // Close the client.

    error = client->close();
    NTSCFG_TEST_OK(error);

    // Close the server.

    error = server->close();
    NTSCFG_TEST_OK(error);

    // Close the listener.

    error = listener->close();
    NTSCFG_TEST_OK(error);

    // Stop the machine.

    machine->stop();
}

NTSCFG_TEST_FUNCTION(ntcd::MachineTest::verifySendBufferOverflow)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    const bsl::size_t DATA_SIZE           = 1024 * 1024;
    const bsl::size_t SEND_BUFFER_SIZE    = 1024 * 256;
    const bsl::size_t RECEIVE_BUFFER_SIZE = 1024 * 64;

    ntsa::Error error;

    // Create a blob buffer factory.

    bdlbb::PooledBlobBufferFactory blobBufferFactory(1024,
                                                     NTSCFG_TEST_ALLOCATOR);

    // Create a machine.

    bsl::shared_ptr<ntcd::Machine> machine;
    machine.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Run the machine.

    error = machine->run();
    NTSCFG_TEST_OK(error);

    // Create a monitor.

    bsl::shared_ptr<ntcd::Monitor> monitor =
        machine->createMonitor(NTSCFG_TEST_ALLOCATOR);

    // Create a listener.

    bsl::shared_ptr<ntcd::Session> listener =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the listener for IPv4 stream sockets.

    error = error = listener->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    error = listener->setBlocking(false);
    NTSCFG_TEST_OK(error);

    // Bind the listener to any port on the IPv4 loopback address.

    error = listener->bind(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
        false);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the listener.

    ntsa::Endpoint listenerSourceEndpoint;
    error = listener->sourceEndpoint(&listenerSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Listener source endpoint = "
                          << listenerSourceEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the listener is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(listenerSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(listenerSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(listenerSourceEndpoint.ip().port(), 0);

    // Begin listening for connections.

    error = listener->listen(0);
    NTSCFG_TEST_OK(error);

    // Add the listener to the monitor.

    error = monitor->add(listener);
    NTSCFG_TEST_OK(error);

    // Create a client.

    bsl::shared_ptr<ntcd::Session> client =
        machine->createSession(NTSCFG_TEST_ALLOCATOR);

    // Open the client as an IPv4 stream socket.

    error = client->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    error = client->setBlocking(false);
    NTSCFG_TEST_OK(error);

    // Connect the client to the listener.

    error = client->connect(listenerSourceEndpoint);
    NTSCFG_TEST_OK(error);

    // Add the client to the monitor.

    error = monitor->add(client);
    NTSCFG_TEST_OK(error);

    // Gain interest in writability for the client.

    error = monitor->show(client, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Block until the client is writable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == client->handle() && event.isWritable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Lose interest in writability for the client.

    error = monitor->hide(client, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the client.

    ntsa::Endpoint clientSourceEndpoint;
    error = client->sourceEndpoint(&clientSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Client source endpoint = "
                          << clientSourceEndpoint << NTCI_LOG_STREAM_END;

    // Get the remote endpoint of the client.

    ntsa::Endpoint clientRemoteEndpoint;
    error = client->remoteEndpoint(&clientRemoteEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Client remote endpoint = "
                          << clientRemoteEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the client is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(clientSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(clientSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(clientSourceEndpoint.ip().port(), 0);

    // Gain interest in readability for the listener.

    error = monitor->show(listener, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Block until the listener is readable.

    while (true) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        bool satisfied = false;
        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == listener->handle() && event.isReadable()) {
                satisfied = true;
                break;
            }
        }

        if (satisfied) {
            break;
        }
    }

    // Accept a server from the listener.

    bsl::shared_ptr<ntcd::Session> server;
    error = listener->accept(&server);
    NTSCFG_TEST_OK(error);

    error = server->setBlocking(false);
    NTSCFG_TEST_OK(error);

    // Lose interest in readability for the listener.

    error = monitor->hide(listener, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Get the source endpoint of the server.

    ntsa::Endpoint serverSourceEndpoint;
    error = server->sourceEndpoint(&serverSourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Server source endpoint = "
                          << serverSourceEndpoint << NTCI_LOG_STREAM_END;

    // Get the remote endpoint of the server.

    ntsa::Endpoint serverRemoteEndpoint;
    error = server->remoteEndpoint(&serverRemoteEndpoint);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Server remote endpoint = "
                          << serverRemoteEndpoint << NTCI_LOG_STREAM_END;

    // Ensure the source endpoint of the server is the IPv4 loopback
    // address and a non-zero port.

    NTSCFG_TEST_TRUE(serverSourceEndpoint.isIp());
    NTSCFG_TEST_EQ(serverSourceEndpoint.ip().host(),
                   ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
    NTSCFG_TEST_NE(serverSourceEndpoint.ip().port(), 0);

    // Ensure the remote endpoint of the client is the source endpoint of
    // the server.

    NTSCFG_TEST_EQ(clientRemoteEndpoint, serverSourceEndpoint);

    // Ensure the remote endpoint of the server is the source endpoint of
    // the client.

    NTSCFG_TEST_EQ(serverRemoteEndpoint, clientSourceEndpoint);

    // Add the server to the monitor.

    error = monitor->add(server);
    NTSCFG_TEST_OK(error);

    // Gain interest in writability for the client.

    error = monitor->show(client, ntca::ReactorEventType::e_WRITABLE);
    NTSCFG_TEST_OK(error);

    // Gain interest in readability for the server.

    error = monitor->show(server, ntca::ReactorEventType::e_READABLE);
    NTSCFG_TEST_OK(error);

    // Define the data to be sent by the client and received by the server.

    bdlbb::Blob clientData(&blobBufferFactory);
    bdlbb::Blob serverData(&blobBufferFactory);

    bdlbb::Blob clientDataRemaining(&blobBufferFactory);

    ntcd::DataUtil::generateData(&clientData, DATA_SIZE);
    clientDataRemaining = clientData;

    // Set the send buffer size for the client.

    {
        ntsa::SocketOption option;
        option.makeSendBufferSize(SEND_BUFFER_SIZE);

        error = client->setOption(option);
        NTSCFG_TEST_OK(error);
    }

    // Set the receive buffer size for the server.

    {
        ntsa::SocketOption option;
        option.makeReceiveBufferSize(RECEIVE_BUFFER_SIZE);

        error = server->setOption(option);
        NTSCFG_TEST_OK(error);
    }

    // Send data from the client when the client is writable and receive
    // data at the server when the server is readable, until the client
    // has sent all data and the server has received all data.

    bool clientDone = false;
    bool serverDone = false;

    while (!clientDone || !serverDone) {
        bsl::vector<ntca::ReactorEvent> events;
        error = monitor->dequeue(&events);
        NTSCFG_TEST_OK(error);

        for (bsl::size_t i = 0; i < events.size(); ++i) {
            const ntca::ReactorEvent& event = events[i];

            if (event.handle() == client->handle()) {
                NTSCFG_TEST_TRUE(event.isWritable());
                NTSCFG_TEST_FALSE(clientDone);

                ntsa::SendContext context;
                ntsa::SendOptions options;

                error = client->send(&context, clientDataRemaining, options);

                if (error) {
                    NTSCFG_TEST_EQ(error,
                                   ntsa::Error(ntsa::Error::e_WOULD_BLOCK));

                    NTCI_LOG_STREAM_DEBUG << "Client would block"
                                         << NTCI_LOG_STREAM_END;
                }
                else {
                    NTSCFG_TEST_OK(error);

                    NTSCFG_TEST_EQ(context.bytesSendable(),
                                   clientDataRemaining.length());

                    NTSCFG_TEST_GT(context.bytesSent(), 0);

                    NTCI_LOG_STREAM_DEBUG << "Client sent "
                                         << context.bytesSent() << " bytes"
                                         << NTCI_LOG_STREAM_END;

                    bdlbb::BlobUtil::erase(
                        &clientDataRemaining,
                        0,
                        NTCCFG_WARNING_NARROW(int, context.bytesSent()));

                    if (clientDataRemaining.length() == 0) {
                        NTCI_LOG_STREAM_DEBUG << "Client is done"
                                             << NTCI_LOG_STREAM_END;

                        error =
                            monitor->hide(client,
                                          ntca::ReactorEventType::e_WRITABLE);
                        NTSCFG_TEST_OK(error);

                        clientDone = true;
                    }
                }
            }
            else if (event.handle() == server->handle()) {
                NTSCFG_TEST_TRUE(event.isReadable());
                NTSCFG_TEST_FALSE(serverDone);

                int size     = serverData.length();
                int capacity = serverData.totalSize();

                if (capacity == size) {
                    serverData.setLength(size + RECEIVE_BUFFER_SIZE);
                    serverData.setLength(size);

                    capacity = serverData.totalSize();
                }

                ntsa::ReceiveContext context;
                ntsa::ReceiveOptions options;

                error = server->receive(&context, &serverData, options);
                if (error) {
                    NTSCFG_TEST_EQ(error,
                                   ntsa::Error(ntsa::Error::e_WOULD_BLOCK));

                    NTCI_LOG_STREAM_DEBUG << "Server would block"
                                         << NTCI_LOG_STREAM_END;
                }
                else {
                    NTSCFG_TEST_OK(error);
                    NTSCFG_TEST_GT(context.bytesReceivable(), 0);
                    NTSCFG_TEST_GT(context.bytesReceived(), 0);

                    NTCI_LOG_STREAM_DEBUG << "Server received "
                                         << context.bytesReceived() << " bytes"
                                         << NTCI_LOG_STREAM_END;

                    if (serverData.length() == DATA_SIZE) {
                        NTCI_LOG_STREAM_DEBUG << "Server is done"
                                             << NTCI_LOG_STREAM_END;

                        error =
                            monitor->hide(server,
                                          ntca::ReactorEventType::e_READABLE);
                        NTSCFG_TEST_OK(error);

                        serverDone = true;
                    }
                }
            }
            else {
                NTSCFG_TEST_FALSE(true);
            }
        }
    }

    NTSCFG_TEST_TRUE(clientDone);
    NTSCFG_TEST_TRUE(serverDone);

    NTSCFG_TEST_EQ(clientDataRemaining.length(), 0);
    NTSCFG_TEST_EQ(clientData.length(), DATA_SIZE);
    NTSCFG_TEST_EQ(serverData.length(), DATA_SIZE);

    int compareResult = bdlbb::BlobUtil::compare(clientData, serverData);
    NTSCFG_TEST_EQ(compareResult, 0);

    // Remove the client from the monitor.

    error = monitor->remove(client);
    NTSCFG_TEST_OK(error);

    // Remove the server from the monitor.

    error = monitor->remove(server);
    NTSCFG_TEST_OK(error);

    // Remove the listener from the monitor.

    error = monitor->remove(listener);
    NTSCFG_TEST_OK(error);

    // Close the client.

    error = client->close();
    NTSCFG_TEST_OK(error);

    // Close the server.

    error = server->close();
    NTSCFG_TEST_OK(error);

    // Close the listener.

    error = listener->close();
    NTSCFG_TEST_OK(error);

    // Stop the machine.

    machine->stop();
}

}  // close namespace ntcd
}  // close namespace BloombergLP
