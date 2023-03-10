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
#include <ntsi_listenersocket.h>
#include <ntsi_streamsocket.h>
#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_latch.h>
#include <bslmt_semaphore.h>
#include <bsls_assert.h>
#include <bsls_timeinterval.h>
#include <bsl_iostream.h>

using namespace BloombergLP;

namespace example {

namespace usage1 {

/// Describes a set of semaphores to coordinate the occurrence of events
/// for a socket.
struct SemaphoreSet {
    bslmt::Semaphore d_readable;
    bslmt::Semaphore d_writable;
};

ntsa::Error processDescriptorEvent(SemaphoreSet              *semaphoreSet,
                                   const ntca::ReactorEvent&  event)
{
    BSLS_ASSERT_OPT(!event.isError());

    if (event.isReadable()) {
        BSLS_ASSERT_OPT(semaphoreSet->d_readable.getValue() == 0);
        semaphoreSet->d_readable.post();
    }

    if (event.isWritable()) {
        BSLS_ASSERT_OPT(semaphoreSet->d_writable.getValue() == 0);
        semaphoreSet->d_writable.post();
    }

    return ntsa::Error();
}

void execute()
{
    // Only run this example on machines that support Unix domain sockets.

#if defined(BSLS_PLATFORM_OS_UNIX)

    ntsa::Error error;

    // Create a number of semaphores that track the events that should
    // occur for each socket.

    example::usage1::SemaphoreSet listenerSemaphoreSet;
    example::usage1::SemaphoreSet clientSemaphoreSet;
    example::usage1::SemaphoreSet serverSemaphoreSet;

    // Create callbacks to be invoked to react to socket events.

    ntci::ReactorEventCallback listenerEventCallback(NTCCFG_BIND(
                                      &example::usage1::processDescriptorEvent,
                                      &listenerSemaphoreSet,
                                      NTCCFG_BIND_PLACEHOLDER_1));

    ntci::ReactorEventCallback clientEventCallback(NTCCFG_BIND(
                                      &example::usage1::processDescriptorEvent,
                                      &clientSemaphoreSet,
                                      NTCCFG_BIND_PLACEHOLDER_1));

    ntci::ReactorEventCallback serverEventCallback(NTCCFG_BIND(
                                      &example::usage1::processDescriptorEvent,
                                      &serverSemaphoreSet,
                                      NTCCFG_BIND_PLACEHOLDER_1));

    // Create the reactor.

    ntca::ReactorConfig reactorConfig;
    reactorConfig.setMetricName("example");
    reactorConfig.setMinThreads(1);
    reactorConfig.setMaxThreads(1);

    bsl::shared_ptr<ntci::Reactor> reactor =
                                    ntcf::System::createReactor(reactorConfig);

    // Register this thread as a thread that will wait on the reactor.

    ntci::Waiter waiter = reactor->registerWaiter(ntca::WaiterOptions());

    //
    // PART 1: BEGIN LISTENING
    //

    // Create a non-blocking socket, bind it to any port on the loopback
    // address, then begin listening for connections.

    bsl::shared_ptr<ntsi::ListenerSocket> listener =
                                          ntsf::System::createListenerSocket();

    error = listener->open(ntsa::Transport::e_LOCAL_STREAM);
    BSLS_ASSERT_OPT(!error);

    error = listener->setBlocking(false);
    BSLS_ASSERT_OPT(!error);

    error = listener->bind(ntsa::Endpoint(ntsa::LocalName::generateUnique()),
                           false);
    BSLS_ASSERT_OPT(!error);

    error = listener->listen(1);
    BSLS_ASSERT_OPT(!error);

    // Attach the listener socket to the reactor.

    reactor->attachSocket(listener->handle());

    //
    // PART 2: BEGIN CONNECTING
    //

    // Create a non-blocking socket for the client, then begin connecting that
    // socket to the listener socket's local endpoint.

    bsl::shared_ptr<ntsi::StreamSocket> client =
                                            ntsf::System::createStreamSocket();

    error = client->open(ntsa::Transport::e_LOCAL_STREAM);
    BSLS_ASSERT_OPT(!error);

    error = client->setBlocking(false);
    BSLS_ASSERT_OPT(!error);

    ntsa::Endpoint listenerEndpoint;
    error = listener->sourceEndpoint(&listenerEndpoint);
    BSLS_ASSERT_OPT(!error);

    error = client->connect(listenerEndpoint);
    BSLS_ASSERT_OPT(error == ntsa::Error::e_OK ||
                    error == ntsa::Error::e_PENDING ||
                    error == ntsa::Error::e_WOULD_BLOCK);

    // Attach the client socket to the reactor.

    reactor->attachSocket(client->handle());

    //
    // PART 3: WAIT FOR THE LISTENER TO ACCEPT THE CONNECTION
    //

    // Gain interest in the readability of the listener socket, that is, it has
    // a connection available to be accepted.

    reactor->showReadable(listener->handle(),
                          ntca::ReactorEventOptions(),
                          listenerEventCallback);

    // Wait until the listener socket is readable.

    while (listenerSemaphoreSet.d_readable.tryWait() != 0) {
        reactor->poll(waiter);
    }

    // Create a non-blocking socket for the server by accepting the connection
    // made to the listener socket.

    bsl::shared_ptr<ntsi::StreamSocket> server;
    error = listener->accept(&server);
    BSLS_ASSERT_OPT(!error);

    error = server->setBlocking(false);
    BSLS_ASSERT_OPT(!error);

    // Lose interest in the readability of the listener socket.

    reactor->hideReadable(listener->handle());

    // Attach the server socket to the reactor.

    reactor->attachSocket(server->handle());

    //
    // PART 4: WAIT FOR THE CLIENT TO BECOME CONNECTED
    //

    // Gain interest in the writability of the client socket, that is, it has
    // connected to its peer.

    reactor->showWritable(client->handle(),
                          ntca::ReactorEventOptions(),
                          clientEventCallback);

    // Wait until the client socket is writable.

    while (clientSemaphoreSet.d_writable.tryWait() != 0) {
        reactor->poll(waiter);
    }

    // Lose interest in the writability of the client socket.

    reactor->hideWritable(client->handle());

    //
    // PART 5: SEND DATA FROM THE CLIENT TO THE SERVER
    //

    // Enqueue outgoing data to transmit by the client socket.

    {
        char storage = 'C';

        ntsa::Data data(ntsa::ConstBuffer(&storage, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = client->send(&context, data, options);
        BSLS_ASSERT_OPT(!error);

        BSLS_ASSERT_OPT(context.bytesSent() == 1);
    }

    // Gain interest in the readability of the server socket.

    reactor->showReadable(server->handle(),
                          ntca::ReactorEventOptions(),
                          serverEventCallback);

    // Wait until the server socket is readable.

    while (serverSemaphoreSet.d_readable.tryWait() != 0) {
        reactor->poll(waiter);
    }

    // Dequeue incoming data received by the server socket.

    {
        char storage;

        ntsa::Data data(ntsa::MutableBuffer(&storage, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &data, options);
        BSLS_ASSERT_OPT(!error);

        BSLS_ASSERT_OPT(context.bytesReceived() == 1);
        BSLS_ASSERT_OPT(storage == 'C');
    }

    // Lose interest in the readability of the server socket.

    reactor->hideReadable(server->handle());

    //
    // PART 6: SEND DATA FROM THE SERVER TO THE CLIENT
    //

    // Enqueue outgoing data to transmit by the server socket.

    {
        char storage = 'S';

        ntsa::Data data(ntsa::ConstBuffer(&storage, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = server->send(&context, data, options);
        BSLS_ASSERT_OPT(!error);

        BSLS_ASSERT_OPT(context.bytesSent() == 1);
    }

    // Gain interest in the readability of the client socket.

    reactor->showReadable(client->handle(),
                          ntca::ReactorEventOptions(),
                          clientEventCallback);

    // Wait until the client socket is readable.

    while (clientSemaphoreSet.d_readable.tryWait() != 0) {
        reactor->poll(waiter);
    }

    // Dequeue incoming data received by the client socket.

    {
        char storage;

        ntsa::Data data(ntsa::MutableBuffer(&storage, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = client->receive(&context, &data, options);
        BSLS_ASSERT_OPT(!error);

        BSLS_ASSERT_OPT(context.bytesReceived() == 1);
        BSLS_ASSERT_OPT(storage == 'S');
    }

    // Lose interest in the readability of the client socket.

    reactor->hideReadable(client->handle());

    //
    // PART 7: SHUTDOWN WRITING BY THE CLIENT
    //

    // Shutdown writing by the client socket.

    error = client->shutdown(ntsa::ShutdownType::e_SEND);
    BSLS_ASSERT_OPT(!error);

    // Gain interest in the writability of the client socket.

    reactor->showWritable(client->handle(),
                          ntca::ReactorEventOptions(),
                          clientEventCallback);

    // Wait until the client socket is writable.

    while (clientSemaphoreSet.d_writable.tryWait() != 0) {
        reactor->poll(waiter);
    }

    // Enqueue outgoing data to transmit by the client socket, and observe the
    // operation fails because the client socket has been shut down for
    // writing.

    {
        char storage = 'C';

        ntsa::Data data(ntsa::ConstBuffer(&storage, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = client->send(&context, data, options);
        BSLS_ASSERT_OPT(error);
        BSLS_ASSERT_OPT(error.code() == ntsa::Error::e_CONNECTION_DEAD);

        BSLS_ASSERT_OPT(context.bytesSent() == 0);
    }

    // Lose interest in the writability of the client socket.

    reactor->hideWritable(client->handle());

    // Gain interest in the readability of the server socket.

    reactor->showReadable(server->handle(),
                          ntca::ReactorEventOptions(),
                          serverEventCallback);

    // Wait until the server socket is readable.

    while (serverSemaphoreSet.d_readable.tryWait() != 0) {
        reactor->poll(waiter);
    }

    // Dequeue incoming data received by the server socket, and observe that
    // zero bytes are successfully dequeued, indicating the client socket has
    // shut down writing from its side of the connection.

    {
        char storage;

        ntsa::Data data(ntsa::MutableBuffer(&storage, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &data, options);
        BSLS_ASSERT_OPT(!error);

        BSLS_ASSERT_OPT(context.bytesReceived() == 0);
    }

    // Lose interest in the readability of the server socket.

    reactor->hideReadable(server->handle());

    //
    // PART 8: SHUTDOWN WRITING BY THE SERVER
    //

    // Shutdown writing by the server socket.

    error = server->shutdown(ntsa::ShutdownType::e_SEND);
    BSLS_ASSERT_OPT(!error);

    // Gain interest in the writability of the server socket.

    reactor->showWritable(server->handle(),
                          ntca::ReactorEventOptions(),
                          serverEventCallback);

    // Wait until the server socket is writable.

    while (serverSemaphoreSet.d_writable.tryWait() != 0) {
        reactor->poll(waiter);
    }

    // Enqueue outgoing data to transmit by the server socket, and observe the
    // operation fails because the server socket has been shut down for
    // writing.

    {
        char storage = 'S';

        ntsa::Data data(ntsa::ConstBuffer(&storage, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;

        error = server->send(&context, data, options);
        BSLS_ASSERT_OPT(error);
        BSLS_ASSERT_OPT(error.code() == ntsa::Error::e_CONNECTION_DEAD);

        BSLS_ASSERT_OPT(context.bytesSent() == 0);
    }

    // Lose interest in the writability of the server socket.

    reactor->hideWritable(server->handle());

    // Gain interest in the readability of the client socket.

    reactor->showReadable(client->handle(),
                          ntca::ReactorEventOptions(),
                          clientEventCallback);

    // Wait until the client socket is readable.

    while (clientSemaphoreSet.d_readable.tryWait() != 0) {
        reactor->poll(waiter);
    }

    // Dequeue incoming data received by the client socket, and observe that
    // zero bytes are successfully dequeued, indicating the server socket has
    // shut down writing from its side of the connection.

    {
        char storage;

        ntsa::Data data(ntsa::MutableBuffer(&storage, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = client->receive(&context, &data, options);
        BSLS_ASSERT_OPT(!error);

        BSLS_ASSERT_OPT(context.bytesReceived() == 0);
    }

    // Lose interest in the readability of the client socket.

    reactor->hideReadable(client->handle());

    //
    // PART 9: DETACH SOCKETS FROM THE REACTOR
    //

    // Detach the server socket from the reactor.

    reactor->detachSocket(server->handle());

    // Detach the client socket from the reactor.

    reactor->detachSocket(client->handle());

    // Detach the listener socket from the reactor.

    reactor->detachSocket(listener->handle());

    // Deregister the waiter.

    reactor->deregisterWaiter(waiter);

#endif
}

}  // close namespace usage1
}  // close namespace example

void help()
{
    bsl::cout << "usage: <program>> [-v <level>]" << bsl::endl;
}

int main(int argc, char **argv)
{
    ntcf::System::initialize();
    ntcf::System::ignore(ntscfg::Signal::e_PIPE);

    int verbosity = 0;
    {
        int i = 1;
        while (i < argc) {
            if ((0 == std::strcmp(argv[i], "-?")) ||
                (0 == std::strcmp(argv[i], "--help"))) {
                help();
                return 0;
            }

            if (0 == std::strcmp(argv[i], "-v") ||
                0 == std::strcmp(argv[i], "--verbosity")) {
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

    example::usage1::execute();

    return 0;
}

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
