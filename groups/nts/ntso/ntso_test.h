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

#ifndef INCLUDED_NTSO_TEST
#define INCLUDED_NTSO_TEST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsb_datagramsocket.h>
#include <ntsb_listenersocket.h>
#include <ntsb_streamsocket.h>
#include <ntscfg_platform.h>
#include <ntscfg_test.h>
#include <ntsi_reactor.h>
#include <ntsscm_version.h>

namespace BloombergLP {
namespace ntso {

/// @internal @brief
/// Provide utilities for testing reactors.
///
/// @par Thread Safety
// This struct is thread safe.
///
/// @ingroup module_ntso
struct Test {
    /// Define a type alias for a vector of reactors.
    typedef bsl::vector<bsl::shared_ptr<ntsi::Reactor> > ReactorVector;

    /// Test the usage example.
    static void usage(const ReactorVector& reactors,
                      bslma::Allocator*    allocator);

    /// Test the behavior of the reactor when a socket is closed while it is
    /// being polled.
    static void pollingAfterFullShutdown(const ReactorVector& reactors,
                                         bslma::Allocator*    allocator);

    /// Test the behavior of the reactor when a socket is closed while it is
    /// being polled.
    ///
    /// select  The polling function fails, but does not indicate which file
    ///         descriptor was invalid. The reactor implementation scans each
    ///         attached file descriptor and checks if the file descriptor is
    ///         invalid. For each invalid (i.e. file descriptor), it
    ///         synthesizes an error event and immediately detaches that file
    ///         descriptor from the reactor on behalf of the user.
    ///
    /// poll    The polling function succeeds and indicates POLLINVAL for the
    ///         offending file descriptor. The reactor implementation returns
    ///         an error event but immediately detaches the file descriptor
    ///         from the reactor on behalf of the user.
    ///
    /// kqueue  When the file descriptor is closed, it is automatically removed
    ///         from the kqueue interest set maintained internally in the
    ///         kernel. The polling function thus does not return, unless it
    ///         is given a timeout in which case it times out.
    static void pollingAfterClose(const ReactorVector& reactors,
                                  bslma::Allocator*    allocator);

    /// Load into the specified 'client' and 'server' a connected pair of
    /// stream sockets of the specified 'type'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    static ntsa::Error pair(bsl::shared_ptr<ntsi::StreamSocket>* client,
                            bsl::shared_ptr<ntsi::StreamSocket>* server,
                            ntsa::Transport::Value               type,
                            bslma::Allocator* basicAllocator = 0);

    /// Log the specified 'events'.
    static void log(const bsl::vector<ntsa::Event>& events);

    /// Log the specified 'eventSet'.
    static void log(const ntsa::EventSet& eventSet);
};

inline void Test::usage(const ReactorVector& reactors,
                        bslma::Allocator*    allocator)
{
    // Concern:
    // Plan:

    ntsa::Error error;

    for (ReactorVector::const_iterator reactorIterator = reactors.begin();
         reactorIterator != reactors.end();
         ++reactorIterator)
    {
        const bsl::shared_ptr<ntsi::Reactor>& reactor = *reactorIterator;

        ntsa::EventSet eventSet(allocator);

        bdlb::NullableValue<bsls::TimeInterval> deadline;

        // Create a socket pair.

        bsl::shared_ptr<ntsi::StreamSocket> client;
        bsl::shared_ptr<ntsi::StreamSocket> server;

        error = Test::pair(&client,
                           &server,
                           ntsa::Transport::e_TCP_IPV4_STREAM,
                           allocator);
        NTSCFG_TEST_OK(error);

        error = client->setBlocking(false);
        NTSCFG_TEST_OK(error);

        error = server->setBlocking(false);
        NTSCFG_TEST_OK(error);

        // Gain interest in the writability of the client.

        error = reactor->attachSocket(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showWritable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is writable.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isWritable(client->handle()));

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

        error = reactor->detachSocket(client);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the server.

        error = reactor->attachSocket(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is readable.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isReadable(server->handle()));

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

        error = reactor->detachSocket(server);
        NTSCFG_TEST_OK(error);

        // Gain interest in the writability of the server.

        error = reactor->attachSocket(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showWritable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is writable.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isWritable(server->handle()));

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

        error = reactor->detachSocket(server);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the client.

        error = reactor->attachSocket(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is readable.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isReadable(client->handle()));

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

        error = reactor->detachSocket(client);
        NTSCFG_TEST_OK(error);

        // Shutdown writing by the client.

        BSLS_LOG_DEBUG("Descriptor %d shutting down writing",
                       (int)(client->handle()));

        error = client->shutdown(ntsa::ShutdownType::e_SEND);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the server.

        error = reactor->attachSocket(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is readable.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isReadable(server->handle()) ||
                         eventSet.isHangup(server->handle()));

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

        error = reactor->detachSocket(server);
        NTSCFG_TEST_OK(error);

        // Shutdown writing by the server.

        BSLS_LOG_DEBUG("Descriptor %d shutting down writing",
                       (int)(server->handle()));

        error = server->shutdown(ntsa::ShutdownType::e_SEND);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the client.

        error = reactor->attachSocket(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is readable.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isReadable(client->handle()) ||
                         eventSet.isHangup(client->handle()));

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

        error = reactor->detachSocket(client);
        NTSCFG_TEST_OK(error);

        // Close the sockets.

        client->close();
        server->close();
    }
}

inline void Test::pollingAfterFullShutdown(const ReactorVector& reactors,
                                           bslma::Allocator*    allocator)
{
    // Concern: Polling after a socket has been shutdown for both reading and
    // writing after both sides have shutdown writing does not block.

    ntsa::Error error;

    for (ReactorVector::const_iterator reactorIterator = reactors.begin();
         reactorIterator != reactors.end();
         ++reactorIterator)
    {
        const bsl::shared_ptr<ntsi::Reactor>& reactor = *reactorIterator;

        ntsa::EventSet eventSet(allocator);

        bdlb::NullableValue<bsls::TimeInterval> deadline;

        // Create a socket pair.

        bsl::shared_ptr<ntsi::StreamSocket> client;
        bsl::shared_ptr<ntsi::StreamSocket> server;

        error = Test::pair(&client,
                           &server,
                           ntsa::Transport::e_TCP_IPV4_STREAM,
                           allocator);
        NTSCFG_TEST_OK(error);

        error = client->setBlocking(false);
        NTSCFG_TEST_OK(error);

        error = server->setBlocking(false);
        NTSCFG_TEST_OK(error);

        // Gain interest in the writability of the client.

        error = reactor->attachSocket(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showWritable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is writable.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isWritable(client->handle()));

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

        error = reactor->detachSocket(client);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the server.

        error = reactor->attachSocket(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is readable.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isReadable(server->handle()));

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

        error = reactor->detachSocket(server);
        NTSCFG_TEST_OK(error);

        // Gain interest in the writability of the server.

        error = reactor->attachSocket(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showWritable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is writable.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isWritable(server->handle()));

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

        error = reactor->detachSocket(server);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the client.

        error = reactor->attachSocket(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is readable.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isReadable(client->handle()));

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

        error = reactor->detachSocket(client);
        NTSCFG_TEST_OK(error);

        // Shutdown writing by the client.

        BSLS_LOG_DEBUG("Descriptor %d shutting down writing",
                       (int)(client->handle()));

        error = client->shutdown(ntsa::ShutdownType::e_SEND);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the server.

        error = reactor->attachSocket(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is readable.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isReadable(server->handle()) ||
                         eventSet.isHangup(server->handle()));

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

        error = reactor->detachSocket(server);
        NTSCFG_TEST_OK(error);

        // Shutdown writing by the server.

        BSLS_LOG_DEBUG("Descriptor %d shutting down writing",
                       (int)(server->handle()));

        error = server->shutdown(ntsa::ShutdownType::e_SEND);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the client.

        error = reactor->attachSocket(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is readable.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isReadable(client->handle()) ||
                         eventSet.isHangup(client->handle()));

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

        error = reactor->detachSocket(client);
        NTSCFG_TEST_OK(error);

        BSLS_LOG_INFO("Polling server after shutdown complete");

        // Shutdown reading by the server.

        server->shutdown(ntsa::ShutdownType::e_RECEIVE);

        // Gain interest in the readability of the server.

        error = reactor->attachSocket(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is readable or has an error.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isReadable(server->handle()) ||
                         eventSet.isHangup(server->handle()) ||
                         eventSet.isError(server->handle()));

        // Lose interest in the readability of the server.

        error = reactor->hideReadable(server);
        NTSCFG_TEST_OK(error);

        error = reactor->detachSocket(server);
        NTSCFG_TEST_OK(error);

        BSLS_LOG_INFO("Polling client after shutdown complete");

        // Shutdown reading by the client.

        client->shutdown(ntsa::ShutdownType::e_RECEIVE);

        // Gain interest in the readability of the client.

        error = reactor->attachSocket(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is readable or has an error.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isReadable(client->handle()) ||
                         eventSet.isHangup(client->handle()) ||
                         eventSet.isError(client->handle()));

        // Lose interest in the readability of the client.

        error = reactor->hideReadable(client);
        NTSCFG_TEST_OK(error);

        error = reactor->detachSocket(client);
        NTSCFG_TEST_OK(error);

        // Close the sockets.

        client->close();
        server->close();
    }
}

inline void Test::pollingAfterClose(const ReactorVector& reactors,
                                    bslma::Allocator*    allocator)
{
    // Concern: Close socket while it still remains added to the reactor.
    // Polling the reactor times out.

    ntsa::Error error;

    for (ReactorVector::const_iterator reactorIterator = reactors.begin();
         reactorIterator != reactors.end();
         ++reactorIterator)
    {
        const bsl::shared_ptr<ntsi::Reactor>& reactor = *reactorIterator;

        ntsa::EventSet eventSet(allocator);

        bdlb::NullableValue<bsls::TimeInterval> deadline;

        // Create a socket pair.

        bsl::shared_ptr<ntsi::StreamSocket> client;
        bsl::shared_ptr<ntsi::StreamSocket> server;

        error = Test::pair(&client,
                           &server,
                           ntsa::Transport::e_TCP_IPV4_STREAM,
                           allocator);
        NTSCFG_TEST_OK(error);

        error = client->setBlocking(false);
        NTSCFG_TEST_OK(error);

        error = server->setBlocking(false);
        NTSCFG_TEST_OK(error);

        // Gain interest in the writability of the client.

        error = reactor->attachSocket(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showWritable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is writable.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isWritable(client->handle()));

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

        error = reactor->detachSocket(client);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the server.

        error = reactor->attachSocket(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is readable.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isReadable(server->handle()));

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

        error = reactor->detachSocket(server);
        NTSCFG_TEST_OK(error);

        // Gain interest in the writability of the server.

        error = reactor->attachSocket(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showWritable(server);
        NTSCFG_TEST_OK(error);

        // Wait until the server is writable.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isWritable(server->handle()));

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

        error = reactor->detachSocket(server);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the client.

        error = reactor->attachSocket(client);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(client);
        NTSCFG_TEST_OK(error);

        // Wait until the client is readable.

        error = reactor->wait(&eventSet, deadline);
        NTSCFG_TEST_OK(error);
        ntso::Test::log(eventSet);

        NTSCFG_TEST_EQ(eventSet.size(), 1);
        NTSCFG_TEST_TRUE(eventSet.isReadable(client->handle()));

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

        error = reactor->detachSocket(client);
        NTSCFG_TEST_OK(error);

        // Gain interest in the readability of the server.

        error = reactor->attachSocket(server);
        NTSCFG_TEST_OK(error);

        error = reactor->showReadable(server);
        NTSCFG_TEST_OK(error);

        BSLS_LOG_DEBUG("Descriptor %d closing", (int)(server->handle()));
        ntsa::Handle serverHandle = server->handle();
        server->close();

        // Wait until the server is readable.

        error =
            reactor->wait(&eventSet,
                          bdlt::CurrentTime::now() + bsls::TimeInterval(1));
        if (error) {
            NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_WOULD_BLOCK));
            ntso::Test::log(eventSet);
            NTSCFG_TEST_EQ(eventSet.size(), 0);
        }
        else {
            ntso::Test::log(eventSet);
            NTSCFG_TEST_EQ(eventSet.size(), 1);
            NTSCFG_TEST_TRUE(eventSet.isError(serverHandle));

            ntsa::Event event;
            const bool  found = eventSet.find(&event, serverHandle);
            NTSCFG_TEST_TRUE(found);
            NTSCFG_TEST_TRUE(
                event.error() == ntsa::Error(ntsa::Error::e_NOT_OPEN) ||
                event.error() == ntsa::Error(ntsa::Error::e_NOT_SOCKET));
        }
    }
}

inline void Test::log(const bsl::vector<ntsa::Event>& eventSet)
{
    for (bsl::size_t i = 0; i < eventSet.size(); ++i) {
        bsl::stringstream ss;
        ss << eventSet[i];

        BSLS_LOG_DEBUG("Polled event %s", ss.str().c_str());
    }
}

inline ntsa::Error Test::pair(bsl::shared_ptr<ntsi::StreamSocket>* client,
                              bsl::shared_ptr<ntsi::StreamSocket>* server,
                              ntsa::Transport::Value               type,
                              bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    client->reset();
    server->reset();

    bsl::shared_ptr<ntsb::StreamSocket> concreteClient;
    bsl::shared_ptr<ntsb::StreamSocket> concreteServer;

    error = ntsb::StreamSocket::pair(&concreteClient,
                                     &concreteServer,
                                     ntsa::Transport::e_TCP_IPV4_STREAM,
                                     allocator);
    if (error) {
        return error;
    }

    *client = concreteClient;
    *server = concreteServer;

    return ntsa::Error();
}

inline void Test::log(const ntsa::EventSet& eventSet)
{
    for (ntsa::EventSet::const_iterator it = eventSet.cbegin();
         it != eventSet.cend();
         ++it)
    {
        const ntsa::Event& event = *it;

        bsl::stringstream ss;
        ss << event;

        BSLS_LOG_DEBUG("Polled event %s", ss.str().c_str());
    }
}

}  // close package namespace
}  // close enterprise namespace

#endif
