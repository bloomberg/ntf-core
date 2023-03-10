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

#include <ntcd_simulation.h>

#include <ntccfg_test.h>
#include <ntci_log.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------

// [ 1]
//-----------------------------------------------------------------------------
// [ 1]
//-----------------------------------------------------------------------------

NTCCFG_TEST_CASE(1)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        NTCI_LOG_CONTEXT();

        ntsa::Error error;

        // Create and run the simulation.

        bsl::shared_ptr<ntcd::Simulation> simulation;
        simulation.createInplace(&ta, &ta);

        error = simulation->run();
        NTCCFG_TEST_OK(error);

        // Create a stream socket pair.

        bsl::shared_ptr<ntcd::StreamSocket> client;
        bsl::shared_ptr<ntcd::StreamSocket> server;

        error = ntcd::Simulation::createStreamSocketPair(
            &client,
            &server,
            ntsa::Transport::e_TCP_IPV4_STREAM);
        NTCCFG_TEST_OK(error);

        // Get the source endpoint of the client.

        ntsa::Endpoint clientSourceEndpoint;
        error = client->sourceEndpoint(&clientSourceEndpoint);
        NTCCFG_TEST_OK(error);

        NTCI_LOG_STREAM_DEBUG
            << "Client source endpoint = " << clientSourceEndpoint
            << NTCI_LOG_STREAM_END;

        // Get the remote endpoint of the client.

        ntsa::Endpoint clientRemoteEndpoint;
        error = client->remoteEndpoint(&clientRemoteEndpoint);
        NTCCFG_TEST_OK(error);

        NTCI_LOG_STREAM_DEBUG
            << "Client remote endpoint = " << clientRemoteEndpoint
            << NTCI_LOG_STREAM_END;

        // Ensure the source endpoint of the client is the IPv4 loopback
        // address and a non-zero port.

        NTCCFG_TEST_TRUE(clientSourceEndpoint.isIp());
        NTCCFG_TEST_EQ(clientSourceEndpoint.ip().host(),
                       ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
        NTCCFG_TEST_NE(clientSourceEndpoint.ip().port(), 0);

        // Get the source endpoint of the server.

        ntsa::Endpoint serverSourceEndpoint;
        error = server->sourceEndpoint(&serverSourceEndpoint);
        NTCCFG_TEST_OK(error);

        NTCI_LOG_STREAM_DEBUG
            << "Server source endpoint = " << serverSourceEndpoint
            << NTCI_LOG_STREAM_END;

        // Get the remote endpoint of the server.

        ntsa::Endpoint serverRemoteEndpoint;
        error = server->remoteEndpoint(&serverRemoteEndpoint);
        NTCCFG_TEST_OK(error);

        NTCI_LOG_STREAM_DEBUG
            << "Server remote endpoint = " << serverRemoteEndpoint
            << NTCI_LOG_STREAM_END;

        // Ensure the source endpoint of the server is the IPv4 loopback
        // address and a non-zero port.

        NTCCFG_TEST_TRUE(serverSourceEndpoint.isIp());
        NTCCFG_TEST_EQ(serverSourceEndpoint.ip().host(),
                       ntsa::IpAddress(ntsa::Ipv4Address::loopback()));
        NTCCFG_TEST_NE(serverSourceEndpoint.ip().port(), 0);

        // Ensure the remote endpoint of the client is the source endpoint of
        // the server.

        NTCCFG_TEST_EQ(clientRemoteEndpoint, serverSourceEndpoint);

        // Ensure the remote endpoint of the server is the source endpoint of
        // the client.

        NTCCFG_TEST_EQ(serverRemoteEndpoint, clientSourceEndpoint);

        // Send data from the client to the server.

        const char CLIENT_DATA = 'C';

        {
            ntsa::Data data(ntsa::ConstBuffer(&CLIENT_DATA, 1));

            ntsa::SendContext context;
            ntsa::SendOptions options;

            error = client->send(&context, data, options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(context.bytesSendable(), 1);
            NTCCFG_TEST_EQ(context.bytesSent(), 1);
        }

        // Receive data at the server.

        {
            char remoteData = 0;

            ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            error = server->receive(&context, &data, options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(context.bytesReceivable(), 1);
            NTCCFG_TEST_EQ(context.bytesReceived(), 1);

            NTCCFG_TEST_EQ(remoteData, CLIENT_DATA);

            NTCCFG_TEST_FALSE(context.endpoint().isNull());
            NTCCFG_TEST_EQ(context.endpoint().value(), clientSourceEndpoint);
        }

        // Send data from the server to the client.

        const char SERVER_DATA = 'S';

        {
            ntsa::Data data(ntsa::ConstBuffer(&SERVER_DATA, 1));

            ntsa::SendContext context;
            ntsa::SendOptions options;

            error = server->send(&context, data, options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(context.bytesSendable(), 1);
            NTCCFG_TEST_EQ(context.bytesSent(), 1);
        }

        // Receive data at the client.

        {
            char remoteData = 0;

            ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            error = client->receive(&context, &data, options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(context.bytesReceivable(), 1);
            NTCCFG_TEST_EQ(context.bytesReceived(), 1);

            NTCCFG_TEST_EQ(remoteData, SERVER_DATA);

            NTCCFG_TEST_FALSE(context.endpoint().isNull());
            NTCCFG_TEST_EQ(context.endpoint().value(), serverSourceEndpoint);
        }

        // Shutdown the client.

        error = client->shutdown(ntsa::ShutdownType::e_SEND);
        NTCCFG_TEST_OK(error);

        // Receive data at the server.

        {
            char remoteData = 0;

            ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            error = server->receive(&context, &data, options);
            NTCCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_EOF));
            NTCCFG_TEST_EQ(context.bytesReceived(), 0);
        }

        // Shutdown the server.

        error = server->shutdown(ntsa::ShutdownType::e_SEND);
        NTCCFG_TEST_OK(error);

        // Receive data at the client.

        {
            char remoteData = 0;

            ntsa::Data data(ntsa::MutableBuffer(&remoteData, 1));

            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            error = client->receive(&context, &data, options);
            NTCCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_EOF));
            NTCCFG_TEST_EQ(context.bytesReceived(), 0);
        }

        // Try to send data from the client to the server and ensure the
        // operation fails because the client has been shutdown for sending.

        {
            ntsa::Data data(ntsa::ConstBuffer(&CLIENT_DATA, 1));

            ntsa::SendContext context;
            ntsa::SendOptions options;

            error = client->send(&context, data, options);
            NTCCFG_TEST_TRUE(error);

            NTCCFG_TEST_EQ(context.bytesSent(), 0);
        }

        // Try to send data from the server to the client and ensure the
        // operation fails because the server has been shutdown for sending.

        {
            ntsa::Data data(ntsa::ConstBuffer(&SERVER_DATA, 1));

            ntsa::SendContext context;
            ntsa::SendOptions options;

            error = server->send(&context, data, options);
            NTCCFG_TEST_TRUE(error);

            NTCCFG_TEST_EQ(context.bytesSent(), 0);
        }

        // Close the client.

        error = client->close();
        NTCCFG_TEST_OK(error);

        // Close the server.

        error = server->close();
        NTCCFG_TEST_OK(error);

        // Stop the simulation.

        simulation->stop();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
}
NTCCFG_TEST_DRIVER_END;
