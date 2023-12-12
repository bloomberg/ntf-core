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

#ifndef INCLUDED_NTSI_STREAMSOCKET
#define INCLUDED_NTSI_STREAMSOCKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_buffer.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_notificationqueue.h>
#include <ntsa_shutdowntype.h>
#include <ntsa_socketoption.h>
#include <ntsa_transport.h>
#include <ntscfg_platform.h>
#include <ntsi_channel.h>
#include <ntsi_descriptor.h>
#include <ntsscm_version.h>
#include <bdls_filesystemutil.h>
#include <bslma_managedptr.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntsi {

/// Provide a blocking or non-blocking socket whose transport has stream
/// semantics.
///
/// @details
/// Provide an abstract mechanism to perform unicast communication
/// with a single remote endpoint using a transport that has stream semantics.
/// Communication with stream semantics is characterized as stream-oriented,
/// connected, reliable, and ordered. The public member functions of this class
/// roughly correspond to the stream-type socket functionality of the
/// Berkeley Sockets API in the POSIX specification.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @par Usage Example: Sending and Receiving Data Streams
/// This example shows how to use stream sockets to send and receive data
/// streams.
///
/// To start, initialize the library.
///
///     ntsa::Error error = ntsf::System::initialize();
///     BSLS_ASSERT(!error);
///
/// Now, let's create a listener socket, bind it to any port on the loopback
/// address, then begin listening for connections.
///
///     bsl::shared_ptr<ntsi::ListenerSocket> listener =
///                                   ntsf::System::createListenerSocket();
///
///     error = listener->open(ntsa::Transport::e_TCP_IPV4_STREAM);
///     BSLS_ASSERT(!error);
///
///     error = listener->bind(
///         ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0), false);
///     BSLS_ASSERT(!error);
///
///     error = listener->listen(1);
///     BSLS_ASSERT(!error);
///
/// Now, let's create a blocking socket for the client, then connect that
/// socket to the listener socket's local endpoint.
///
///     bsl::shared_ptr<ntsi::StreamSocket> client =
///                                 ntsf::System::createStreamSocket();
///
///     error = client->open(ntsa::Transport::e_TCP_IPV4_STREAM);
///     BSLS_ASSERT(!error);
///
///     ntsa::Endpoint listenerEndpoint;
///     error = listener->sourceEndpoint(&listenerEndpoint);
///     BSLS_ASSERT(!error);
///
///     error = client->connect(listenerEndpoint);
///     BSLS_ASSERT(!error);
///
/// Now, let's create a blocking socket for the server by accepting the
/// connection made to the listener socket.
///
///     bsl::shared_ptr<ntsi::StreamSocket> server;
///     error = listener->accept(&server);
///     BSLS_ASSERT(!error);
///
/// Now, lets' send data from the client to the server. First, enqueue outgoing
/// data to transmit by the client socket.
///
///     {
///         char storage = 'C';
///
///         ntsa::Data data(ntsa::ConstBuffer(&storage, 1));
///
///         ntsa::SendContext context;
///         ntsa::SendOptions options;
///
///         error = client->send(&context, data, options);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(context.bytesSent() == 1);
///     }
///
/// Next, dequeue incoming data received by the server socket.
///
///     {
///         char storage;
///
///         ntsa::Data data(ntsa::MutableBuffer(&storage, 1));
///
///         ntsa::ReceiveContext context;
///         ntsa::ReceiveOptions options;
///
///         error = server->receive(&context, &data, options);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(context.bytesReceived() == 1);
///         BSLS_ASSERT(storage == 'C');
///     }
///
/// Now, let's send data from the server to the client. First, enqueue outgoing
/// data to transmit by the server socket.
///
///     {
///         char storage = 'S';
///
///         ntsa::Data data(ntsa::ConstBuffer(&storage, 1));
///
///         ntsa::SendContext context;
///         ntsa::SendOptions options;
///
///         error = server->send(&context, data, options);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(context.bytesSent() == 1);
///     }
///
/// Next, dequeue incoming data received by the client socket.
///
///     {
///         char storage;
///
///         ntsa::Data data(ntsa::MutableBuffer(&storage, 1));
///
///         ntsa::ReceiveContext context;
///         ntsa::ReceiveOptions options;
///
///         error = client->receive(&context, &data, options);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(context.bytesReceived() == 1);
///         BSLS_ASSERT(storage == 'S');
///     }
///
/// Now, let's shutdown writing by the client socket.
///
///     error = client->shutdown(ntsa::ShutdownType::e_SEND);
///     BSLS_ASSERT(!error);
///
/// Next, dequeue incoming data received by the server socket, and observe that
/// either 'ntsa::Error::e_EOF' is returned or zero bytes are successfully
/// dequeued, indicating the client socket has shut down writing from its side
/// of the connection.
///
///     {
///         char storage;
///
///         ntsa::Data data(ntsa::MutableBuffer(&storage, 1));
///
///         ntsa::ReceiveContext context;
///         ntsa::ReceiveOptions options;
///
///         error = server->receive(&context, &data, options);
///         BSLS_ASSERT(!error || error == ntsa::Error::e_EOF);
///
///         BSLS_ASSERT(context.bytesReceived() == 0);
///     }
///
/// Now, let's shutdown writing by the server socket.
///
///     error = server->shutdown(ntsa::ShutdownType::e_SEND);
///     BSLS_ASSERT(!error);
///
/// Next, dequeue incoming data received by the client socket, and observe that
/// either 'ntsa::Error::e_EOF' is returned or zero bytes are successfully
/// dequeued, indicating the server socket has shut down writing from its side
/// of the connection.
///
///     {
///         char storage;
///
///         ntsa::Data data(ntsa::MutableBuffer(&storage, 1));
///
///         ntsa::ReceiveContext context;
///         ntsa::ReceiveOptions options;
///
///         error = client->receive(&context, &data, options);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(context.bytesReceived() == 0);
///     }
///
///
/// @ingroup module_ntsi
class StreamSocket : public ntsi::Channel
{
  public:
    /// Destroy this object.
    ~StreamSocket() BSLS_KEYWORD_OVERRIDE;

    /// Create a new socket of the specified 'transport'. Return the
    /// error.
    virtual ntsa::Error open(ntsa::Transport::Value transport) = 0;

    /// Acquire ownership of the specified 'handle' to implement this
    /// socket. Return the error.
    virtual ntsa::Error acquire(ntsa::Handle handle) = 0;

    /// Release ownership of the handle that implements this socket.
    virtual ntsa::Handle release() = 0;

    /// Bind this socket to the specified source 'endpoint'. If the
    /// specified 'reuseAddress' flag is set, allow this socket to bind to
    /// an address already in use by the operating system. Return the error.
    virtual ntsa::Error bind(const ntsa::Endpoint& endpoint,
                             bool                  reuseAddress);

    /// Bind this to any suitable source endpoint appropriate for a socket
    /// of the specified 'transport'. If the specified 'reuseAddress' flag
    /// is set, allow this socket to bind to an address already in use by
    /// the operating system. Return the error.
    virtual ntsa::Error bindAny(ntsa::Transport::Value transport,
                                bool                   reuseAddress);

    /// Connect to the specified remote 'endpoint'. Return the error.
    virtual ntsa::Error connect(const ntsa::Endpoint& endpoint);

    /// Enqueue the specified 'data' to the socket send buffer according to
    /// the specified 'options'. Load into the specified 'context' the
    /// result of the operation. Return the error.
    ntsa::Error send(ntsa::SendContext*       context,
                     const bdlbb::Blob&       data,
                     const ntsa::SendOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'data' to the socket send buffer according to
    /// the specified 'options'. Load into the specified 'context' the
    /// result of the operation. Return the error.
    ntsa::Error send(ntsa::SendContext*       context,
                     const ntsa::Data&        data,
                     const ntsa::SendOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'data' having the specified 'size' to the
    /// socket send buffer according to the specified 'options'. Load into
    /// the specified 'context' the result of the operation. Return the
    /// error.
    ntsa::Error send(ntsa::SendContext*       context,
                     const void*              data,
                     bsl::size_t              size,
                     const ntsa::SendOptions& options);

    /// Dequeue from the socket receive buffer into the specified 'data'
    /// according to the specified 'options'. Load into the specified
    /// 'context' the result of the operation. Return the error.
    ntsa::Error receive(ntsa::ReceiveContext*       context,
                        bdlbb::Blob*                data,
                        const ntsa::ReceiveOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Dequeue from the socket receive buffer into the specified 'data'
    /// according to the specified 'options'. Load into the specified
    /// 'context' the result of the operation. Return the error.
    ntsa::Error receive(ntsa::ReceiveContext*       context,
                        ntsa::Data*                 data,
                        const ntsa::ReceiveOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Dequeue from the socket receive buffer into the specified 'data'
    /// having the specified 'capacity' according to the specified
    /// 'options'. Load into the specified 'context' the result of the
    /// operation. Return the error.
    ntsa::Error receive(ntsa::ReceiveContext*       context,
                        void*                       data,
                        bsl::size_t                 capacity,
                        const ntsa::ReceiveOptions& options);

    /// Read data from the socket error queue. Then if the specified
    /// 'notifications' is not null parse fetched data to extract control
    /// messages into the specified 'notifications'. Return the error.
    virtual ntsa::Error receiveNotifications(
        ntsa::NotificationQueue* notifications);

    /// Shutdown the stream socket in the specified 'direction'. Return the
    /// error.
    ntsa::Error shutdown(ntsa::ShutdownType::Value direction)
        BSLS_KEYWORD_OVERRIDE;

    /// Unlink the file corresponding to the socket, if the socket is a
    /// local (a.k.a. Unix domain) socket bound to a non-abstract path.
    /// Return the error. Note that this function should only be called
    /// for sockets in the connect role.
    virtual ntsa::Error unlink();

    /// Close the socket. Return the error.
    virtual ntsa::Error close();

    /// Load into the specified 'result' the source endpoint of this socket.
    /// Return the error.
    virtual ntsa::Error sourceEndpoint(ntsa::Endpoint* result) const;

    /// Load into the specified 'result' the remote endpoint to which this
    /// socket is connected. Return the error.
    virtual ntsa::Error remoteEndpoint(ntsa::Endpoint* result) const;

    // *** Socket Options ***

    /// Set the option for the 'socket' that controls its blocking mode
    /// according to the specified 'blocking' flag. Return the error.
    virtual ntsa::Error setBlocking(bool blocking);

    /// Set the specified 'option' for this socket. Return the error.
    virtual ntsa::Error setOption(const ntsa::SocketOption& option);

    /// Load into the specified 'option' the socket option of the specified
    /// 'type' set for this socket. Return the error.
    virtual ntsa::Error getOption(ntsa::SocketOption*           option,
                                  ntsa::SocketOptionType::Value type);

    /// Load into the specified 'result' the last known error encountered
    /// when connecting the socket. Return the error (retrieving the error).
    virtual ntsa::Error getLastError(ntsa::Error* result);

    // *** Limits ***

    /// Return the maximum number of buffers that can be the source of a
    /// gathered write. Additional buffers beyond this limit are silently
    /// ignored.
    virtual bsl::size_t maxBuffersPerSend() const;

    /// Return the maximum number of buffers that can be the destination
    /// of a scattered read. Additional buffers beyond this limit are
    /// silently ignored.
    virtual bsl::size_t maxBuffersPerReceive() const;
};

NTSCFG_INLINE
ntsa::Error StreamSocket::send(ntsa::SendContext*       context,
                               const void*              data,
                               bsl::size_t              size,
                               const ntsa::SendOptions& options)
{
    return this->send(context,
                      ntsa::Data(ntsa::ConstBuffer(data, size)),
                      options);
}

NTSCFG_INLINE
ntsa::Error StreamSocket::receive(ntsa::ReceiveContext*       context,
                                  void*                       data,
                                  bsl::size_t                 capacity,
                                  const ntsa::ReceiveOptions& options)
{
    ntsa::Data container(ntsa::MutableBuffer(data, capacity));
    return this->receive(context, &container, options);
}

}  // close package namespace
}  // close enterprise namespace
#endif
