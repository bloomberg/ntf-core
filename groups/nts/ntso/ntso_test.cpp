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

#include <ntso_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntso_test_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntso {

void Test::usage(
        const ReactorVector& reactors, bslma::Allocator* allocator)
{
    // Concern:
    // Plan:

    ntsa::Error error;

    for (ReactorVector::const_iterator reactorIterator  = reactors.begin(); 
                                       reactorIterator != reactors.end(); 
                                     ++reactorIterator)
    {
        const bsl::shared_ptr<ntsi::Reactor>& reactor = *reactorIterator;

        bsl::vector<ntsa::Event> events(allocator);

        bdlb::NullableValue<bsls::TimeInterval> deadline;

        // Create a socket pair.

        bsl::shared_ptr<ntsb::StreamSocket> client;
        bsl::shared_ptr<ntsb::StreamSocket> server;

        error = ntsb::StreamSocket::pair(&client,
                                         &server,
                                         ntsa::Transport::e_TCP_IPV4_STREAM,
                                         allocator);
        NTSCFG_TEST_OK(error);

        error = client->setBlocking(false);
        NTSCFG_TEST_OK(error);

        error = server->setBlocking(false);
        NTSCFG_TEST_OK(error);

        // Gain interest in the writability of the client.

        error = reactor->add(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showWritable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is writable.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), client->handle());
        NTSCFG_TEST_TRUE(events[0].isWritable());

        // Send a single byte to the server.

        {
            char buffer = 'X';

            ntsa::SendContext context;
            ntsa::SendOptions options;

            ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

            error = client->send(&context, data, options);
            NTSCFG_TEST_OK(error);

            NTSCFG_TEST_EQ(context.bytesSendable(), 1);
            NTSCFG_TEST_EQ(context.bytesSent(), 1);
        }

        // Lose interest in the writability of the client.

        error = reactor->hideWritable(client);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(client);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the server.

        error = reactor->add(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is readable.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), server->handle());
        NTSCFG_TEST_TRUE(events[0].isReadable());

        // Receive a single byte at the server.

        {
            char buffer;

            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error = server->receive(&context, &data, options);
            NTSCFG_TEST_FALSE(error);

            NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
            NTSCFG_TEST_EQ(context.bytesReceived(), 1);
            NTSCFG_TEST_EQ(buffer, 'X');
        }

        // Lose interest in the readability of the server.

        error = reactor->hideReadable(server);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(server);
        NTSCFG_TEST_OK(error);

        // Gain interest in the writability of the server.

        error = reactor->add(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showWritable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is writable.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), server->handle());
        NTSCFG_TEST_TRUE(events[0].isWritable());

        // Send a single byte to the client.

        {
            char buffer = 'X';

            ntsa::SendContext context;
            ntsa::SendOptions options;

            ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

            error = server->send(&context, data, options);
            NTSCFG_TEST_OK(error);

            NTSCFG_TEST_EQ(context.bytesSendable(), 1);
            NTSCFG_TEST_EQ(context.bytesSent(), 1);
        }

        // Lose interest in the writability of the server.

        error = reactor->hideWritable(server);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(server);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the client.

        error = reactor->add(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is readable.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), client->handle());
        NTSCFG_TEST_TRUE(events[0].isReadable());

        // Receive a single byte at the client.

        {
            char buffer;

            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error = client->receive(&context, &data, options);
            NTSCFG_TEST_FALSE(error);

            NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
            NTSCFG_TEST_EQ(context.bytesReceived(), 1);
            NTSCFG_TEST_EQ(buffer, 'X');
        }

        // Lose interest in the readability of the client.

        error = reactor->hideReadable(client);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(client);
        NTSCFG_TEST_OK(error);

        // Shutdown writing by the client.

        BSLS_LOG_DEBUG("Descriptor %d shutting down writing",
                       (int)(client->handle()));

        error = client->shutdown(ntsa::ShutdownType::e_SEND);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the server.

        error = reactor->add(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is readable.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), server->handle());
        NTSCFG_TEST_TRUE(events[0].isReadable());

        // Receive a single byte at the server.

        {
            char buffer;

            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error = server->receive(&context, &data, options);
            if (error) {
                NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_EOF));
            }
            else {
                NTSCFG_TEST_FALSE(error);

                NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
                NTSCFG_TEST_EQ(context.bytesReceived(), 0);
            }
        }

        // Lose interest in the readability of the server.

        error = reactor->hideReadable(server);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(server);
        NTSCFG_TEST_OK(error);

        // Shutdown writing by the server.

        BSLS_LOG_DEBUG("Descriptor %d shutting down writing",
                       (int)(server->handle()));

        error = server->shutdown(ntsa::ShutdownType::e_SEND);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the client.

        error = reactor->add(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is readable.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), client->handle());
        NTSCFG_TEST_TRUE(events[0].isReadable());

        // Receive a single byte at the client.

        {
            char buffer;

            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error = client->receive(&context, &data, options);
            if (error) {
                NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_EOF));
            }
            else {
                NTSCFG_TEST_FALSE(error);

                NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
                NTSCFG_TEST_EQ(context.bytesReceived(), 0);
            }
        }

        // Lose interest in the readability of the client.

        error = reactor->hideReadable(client);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(client);
        NTSCFG_TEST_OK(error);

        // Close the sockets.

        client->close();
        server->close();
    }
}

void Test::pollingAfterFullShutdown(
        const ReactorVector& reactors, bslma::Allocator* allocator)
{
    // Concern: Polling after a socket has been shutdown for both reading and
    // writing after both sides have shutdown writing does not block.

    ntsa::Error error;

    for (ReactorVector::const_iterator reactorIterator  = reactors.begin(); 
                                       reactorIterator != reactors.end(); 
                                     ++reactorIterator)
    {
        const bsl::shared_ptr<ntsi::Reactor>& reactor = *reactorIterator;

        bsl::vector<ntsa::Event> events(allocator);

        bdlb::NullableValue<bsls::TimeInterval> deadline;

        // Create a socket pair.

        bsl::shared_ptr<ntsb::StreamSocket> client;
        bsl::shared_ptr<ntsb::StreamSocket> server;

        error = ntsb::StreamSocket::pair(&client,
                                         &server,
                                         ntsa::Transport::e_TCP_IPV4_STREAM,
                                         allocator);
        NTSCFG_TEST_OK(error);

        error = client->setBlocking(false);
        NTSCFG_TEST_OK(error);

        error = server->setBlocking(false);
        NTSCFG_TEST_OK(error);

        // Gain interest in the writability of the client.

        error = reactor->add(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showWritable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is writable.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), client->handle());
        NTSCFG_TEST_TRUE(events[0].isWritable());

        // Send a single byte to the server.

        {
            char buffer = 'X';

            ntsa::SendContext context;
            ntsa::SendOptions options;

            ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

            error = client->send(&context, data, options);
            NTSCFG_TEST_OK(error);

            NTSCFG_TEST_EQ(context.bytesSendable(), 1);
            NTSCFG_TEST_EQ(context.bytesSent(), 1);
        }

        // Lose interest in the writability of the client.

        error = reactor->hideWritable(client);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(client);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the server.

        error = reactor->add(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is readable.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), server->handle());
        NTSCFG_TEST_TRUE(events[0].isReadable());

        // Receive a single byte at the server.

        {
            char buffer;

            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error = server->receive(&context, &data, options);
            NTSCFG_TEST_FALSE(error);

            NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
            NTSCFG_TEST_EQ(context.bytesReceived(), 1);
            NTSCFG_TEST_EQ(buffer, 'X');
        }

        // Lose interest in the readability of the server.

        error = reactor->hideReadable(server);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(server);
        NTSCFG_TEST_OK(error);

        // Gain interest in the writability of the server.

        error = reactor->add(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showWritable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is writable.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), server->handle());
        NTSCFG_TEST_TRUE(events[0].isWritable());

        // Send a single byte to the client.

        {
            char buffer = 'X';

            ntsa::SendContext context;
            ntsa::SendOptions options;

            ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

            error = server->send(&context, data, options);
            NTSCFG_TEST_OK(error);

            NTSCFG_TEST_EQ(context.bytesSendable(), 1);
            NTSCFG_TEST_EQ(context.bytesSent(), 1);
        }

        // Lose interest in the writability of the server.

        error = reactor->hideWritable(server);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(server);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the client.

        error = reactor->add(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is readable.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), client->handle());
        NTSCFG_TEST_TRUE(events[0].isReadable());

        // Receive a single byte at the client.

        {
            char buffer;

            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error = client->receive(&context, &data, options);
            NTSCFG_TEST_FALSE(error);

            NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
            NTSCFG_TEST_EQ(context.bytesReceived(), 1);
            NTSCFG_TEST_EQ(buffer, 'X');
        }

        // Lose interest in the readability of the client.

        error = reactor->hideReadable(client);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(client);
        NTSCFG_TEST_OK(error);

        // Shutdown writing by the client.

        BSLS_LOG_DEBUG("Descriptor %d shutting down writing",
                       (int)(client->handle()));

        error = client->shutdown(ntsa::ShutdownType::e_SEND);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the server.

        error = reactor->add(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is readable.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), server->handle());
        NTSCFG_TEST_TRUE(events[0].isReadable());

        // Receive a single byte at the server.

        {
            char buffer;

            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error = server->receive(&context, &data, options);
            if (error) {
                NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_EOF));
            }
            else {
                NTSCFG_TEST_FALSE(error);

                NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
                NTSCFG_TEST_EQ(context.bytesReceived(), 0);
            }
        }

        // Lose interest in the readability of the server.

        error = reactor->hideReadable(server);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(server);
        NTSCFG_TEST_OK(error);

        // Shutdown writing by the server.

        BSLS_LOG_DEBUG("Descriptor %d shutting down writing",
                       (int)(server->handle()));

        error = server->shutdown(ntsa::ShutdownType::e_SEND);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the client.

        error = reactor->add(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is readable.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), client->handle());
        NTSCFG_TEST_TRUE(events[0].isReadable());

        // Receive a single byte at the client.

        {
            char buffer;

            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error = client->receive(&context, &data, options);
            if (error) {
                NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_EOF));
            }
            else {
                NTSCFG_TEST_FALSE(error);

                NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
                NTSCFG_TEST_EQ(context.bytesReceived(), 0);
            }
        }

        // Lose interest in the readability of the client.

        error = reactor->hideReadable(client);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(client);
        NTSCFG_TEST_OK(error);

        BSLS_LOG_INFO("Polling server after shutdown complete");

        // Shutdown reading by the server.

        server->shutdown(ntsa::ShutdownType::e_RECEIVE);

        // Gain interest in the readability of the server.

        error = reactor->add(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is readable or has an error.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), server->handle());
        NTSCFG_TEST_TRUE(events[0].isReadable() || 
                         events[0].isError());

        // Lose interest in the readability of the server.

        error = reactor->hideReadable(server);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(server);
        NTSCFG_TEST_OK(error);

        BSLS_LOG_INFO("Polling client after shutdown complete");

        // Shutdown reading by the client.

        client->shutdown(ntsa::ShutdownType::e_RECEIVE);

        // Gain interest in the readability of the client.

        error = reactor->add(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is readable or has an error.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), client->handle());
        NTSCFG_TEST_TRUE(events[0].isReadable() || events[0].isError());

        // Lose interest in the readability of the client.

        error = reactor->hideReadable(client);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(client);
        NTSCFG_TEST_OK(error);

        // Close the sockets.

        client->close();
        server->close();
    }
}

void Test::pollingDuringClose(
        const ReactorVector& reactors, bslma::Allocator* allocator)
{
    // Concern: Close socket while it still remains added to the reactor.
    // Polling the reactor times out.

    ntsa::Error error;

    for (ReactorVector::const_iterator reactorIterator  = reactors.begin(); 
                                       reactorIterator != reactors.end(); 
                                     ++reactorIterator)
    {
        const bsl::shared_ptr<ntsi::Reactor>& reactor = *reactorIterator;

        bsl::vector<ntsa::Event> events(allocator);

        bdlb::NullableValue<bsls::TimeInterval> deadline;

        // Create a socket pair.

        bsl::shared_ptr<ntsb::StreamSocket> client;
        bsl::shared_ptr<ntsb::StreamSocket> server;

        error = ntsb::StreamSocket::pair(&client,
                                         &server,
                                         ntsa::Transport::e_TCP_IPV4_STREAM,
                                         allocator);
        NTSCFG_TEST_OK(error);

        error = client->setBlocking(false);
        NTSCFG_TEST_OK(error);

        error = server->setBlocking(false);
        NTSCFG_TEST_OK(error);

        // Gain interest in the writability of the client.

        error = reactor->add(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showWritable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is writable.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), client->handle());
        NTSCFG_TEST_TRUE(events[0].isWritable());

        // Send a single byte to the server.

        {
            char buffer = 'X';

            ntsa::SendContext context;
            ntsa::SendOptions options;

            ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

            error = client->send(&context, data, options);
            NTSCFG_TEST_OK(error);

            NTSCFG_TEST_EQ(context.bytesSendable(), 1);
            NTSCFG_TEST_EQ(context.bytesSent(), 1);
        }

        // Lose interest in the writability of the client.

        error = reactor->hideWritable(client);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(client);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the server.

        error = reactor->add(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is readable.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), server->handle());
        NTSCFG_TEST_TRUE(events[0].isReadable());

        // Receive a single byte at the server.

        {
            char buffer;

            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error = server->receive(&context, &data, options);
            NTSCFG_TEST_FALSE(error);

            NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
            NTSCFG_TEST_EQ(context.bytesReceived(), 1);
            NTSCFG_TEST_EQ(buffer, 'X');
        }

        // Lose interest in the readability of the server.

        error = reactor->hideReadable(server);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(server);
        NTSCFG_TEST_OK(error);

        // Gain interest in the writability of the server.

        error = reactor->add(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showWritable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is writable.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), server->handle());
        NTSCFG_TEST_TRUE(events[0].isWritable());

        // Send a single byte to the client.

        {
            char buffer = 'X';

            ntsa::SendContext context;
            ntsa::SendOptions options;

            ntsa::Data data(ntsa::ConstBuffer(&buffer, 1));

            error = server->send(&context, data, options);
            NTSCFG_TEST_OK(error);

            NTSCFG_TEST_EQ(context.bytesSendable(), 1);
            NTSCFG_TEST_EQ(context.bytesSent(), 1);
        }

        // Lose interest in the writability of the server.

        error = reactor->hideWritable(server);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(server);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the client.

        error = reactor->add(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is readable.

        error = reactor->wait(&events, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(events);

        NTSCFG_TEST_EQ(events.size(), 1);
        NTSCFG_TEST_EQ(events[0].handle(), client->handle());
        NTSCFG_TEST_TRUE(events[0].isReadable());

        // Receive a single byte at the client.

        {
            char buffer;

            ntsa::ReceiveContext context;
            ntsa::ReceiveOptions options;

            ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

            error = client->receive(&context, &data, options);
            NTSCFG_TEST_FALSE(error);

            NTSCFG_TEST_EQ(context.bytesReceivable(), 1);
            NTSCFG_TEST_EQ(context.bytesReceived(), 1);
            NTSCFG_TEST_EQ(buffer, 'X');
        }

        // Lose interest in the readability of the client.

        error = reactor->hideReadable(client);
        NTSCFG_TEST_OK(error);

        error = reactor->remove(client);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the server.

        error = reactor->add(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(server);
        NTSCFG_TEST_OK(error);

        BSLS_LOG_DEBUG("Descriptor %d closing", (int)(server->handle()));
        ntsa::Handle serverHandle = server->handle();
        server->close();

        // Wait until the server is readable.

        error =
            reactor->wait(&events,
                          bdlt::CurrentTime::now() + bsls::TimeInterval(1));
        if (error) {
            NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_WOULD_BLOCK));
            ntso::Test::log(events);
            NTSCFG_TEST_EQ(events.size(), 0);
        }
        else {
            ntso::Test::log(events);
            NTSCFG_TEST_EQ(events.size(), 1);
            NTSCFG_TEST_EQ(events[0].handle(), serverHandle);
            NTSCFG_TEST_TRUE(events[0].isError());
            NTSCFG_TEST_EQ(events[0].error(), 
                           ntsa::Error(ntsa::Error::e_CLOSED));
        }
    }
}

void Test::log(const bsl::vector<ntsa::Event>& events)
{
    for (bsl::size_t i = 0; i < events.size(); ++i) {
        bsl::stringstream ss;
        ss << events[i];

        BSLS_LOG_DEBUG("Polled event %s", ss.str().c_str());
    }
}

}  // close package namespace
}  // close enterprise namespace
