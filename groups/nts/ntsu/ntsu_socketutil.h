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

#ifndef INCLUDED_NTSU_SOCKETUTIL
#define INCLUDED_NTSU_SOCKETUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_buffer.h>
#include <ntsa_data.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_handle.h>
#include <ntsa_message.h>
#include <ntsa_notificationqueue.h>
#include <ntsa_receivecontext.h>
#include <ntsa_receiveoptions.h>
#include <ntsa_sendcontext.h>
#include <ntsa_sendoptions.h>
#include <ntsa_shutdowntype.h>
#include <ntsa_socketinfo.h>
#include <ntsa_socketinfofilter.h>
#include <ntsa_transport.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlbb_blob.h>
#include <bdls_filesystemutil.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsu {

/// @internal @brief
/// Provide utilities for operating system sockets.
///
/// @details
/// Provide a portable interface to operating system sockets by
/// wrapping the relevant operating system interfaces, hiding their various
/// differences and homogenizing their various inconsistences.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @par Usage Example 1: Stream sockets
/// This examples shows how to create a stream socket listener, connect a
/// client to that listener, accept the server socket from the listener,
/// exchange data between the client and server, then shutdown and close each
/// socket. For brevity, all error checking is omitted.
///
/// To start, initialize the library.
///
///     ntsa::Error error = ntscfg::Platform::initialize();
///     BSLS_ASSERT(!error);
///
/// Next, create a blocking socket for the listener, bind it to any port on the
/// loopback address, then begin listening for connections.
///
///     ntsa::Handle listener;
///     error = ntsu::SocketUtil::create(&listener,
///                                      ntsa::Transport::e_TCP_IPV4_STREAM);
///     BSLS_ASSERT(!error);
///
///     error = ntsu::SocketUtil::bind(
///         ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
///         false,
///         listener);
///     BSLS_ASSERT(!error);
///
///     error = ntsu::SocketUtil::listen(1, listener);
///     BSLS_ASSERT(!error);
///
/// Next, create a blocking socket for the client, then connect that socket to
/// the listener socket's local endpoint.
///
///     ntsa::Handle client;
///     error = ntsu::SocketUtil::create(&client,
///                                      ntsa::Transport::e_TCP_IPV4_STREAM);
///     BSLS_ASSERT(!error);
///
///     ntsa::Endpoint listenerEndpoint;
///     error = ntsu::SocketUtil::sourceEndpoint(&listenerEndpoint, listener);
///     BSLS_ASSERT(!error);
///
///     error = ntsu::SocketUtil::connect(listenerEndpoint, client);
///     BSLS_ASSERT(!error);
///
/// Next, create a blocking socket for the server by accepting the connection
/// made to the listener socket.
///
///     ntsa::Handle server;
///     error = ntsu::SocketUtil::accept(&server, listener);
///     BSLS_ASSERT(!error);
///
/// Now, enqueue outgoing data to transmit by the client socket.
///
///     {
///         char storage = 'C';
///
///         ntsa::Data data(ntsa::ConstBuffer(&storage, 1));
///
///         ntsa::SendContext context;
///         ntsa::SendOptions options;
///
///         error = ntsu::SocketUtil::send(&context, data, options, client);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(context.bytesSent() == 1);
///     }
///
/// Then, dequeue incoming data received by the server socket.
///
///     {
///         char storage;
///
///         ntsa::Data data(ntsa::MutableBuffer(&storage, 1));
///
///         ntsa::ReceiveContext context;
///         ntsa::ReceiveOptions options;
///
///         error = ntsu::SocketUtil::receive(
///                                   &context, &data, options, server);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(context.bytesReceived() == 1);
///         BSLS_ASSERT(storage == 'C');
///     }
///
/// Now, enqueue outgoing data to transmit by the server socket.
///
///     {
///         char storage = 'S';
///
///         ntsa::Data data(ntsa::ConstBuffer(&storage, 1));
///
///         ntsa::SendContext context;
///         ntsa::SendOptions options;
///
///         error = ntsu::SocketUtil::send(&context, data, options, server);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(context.bytesSent() == 1);
///     }
///
/// Then, dequeue incoming data received by the client socket.
///
///     {
///         char storage;
///
///         ntsa::Data data(ntsa::MutableBuffer(&storage, 1));
///
///         ntsa::ReceiveContext context;
///         ntsa::ReceiveOptions options;
///
///         error = ntsu::SocketUtil::receive(
///                                   &context, &data, options, client);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(context.bytesReceived() == 1);
///         BSLS_ASSERT(storage == 'S');
///     }
///
/// Next, shutdown writing by the client socket.
///
///     ntsu::SocketUtil::shutdown(ntsa::ShutdownType::e_SEND, client);
///
/// Now, dequeue incoming data received by the server socket, and observe that
/// zero bytes are successfully dequeued, indicating the client socket has shut
/// down writing from its side of the connection.
///
///     {
///         char storage;
///
///         ntsa::Data data(ntsa::MutableBuffer(&storage, 1));
///
///         ntsa::ReceiveContext context;
///         ntsa::ReceiveOptions options;
///
///         error = ntsu::SocketUtil::receive(
///                                   &context, &data, options, server);
///         BSLS_ASSERT(!error || error == ntsa::Error(ntsa::Error::e_EOF));
///         BSLS_ASSERT(numBytesReceivable == 1);
///         BSLS_ASSERT(numBytesReceived == 0);
///     }
///
/// Next, shutdown writing by the server socket.
///
///     ntsu::SocketUtil::shutdown(ntsa::ShutdownType::e_SEND, server);
///
/// Now, dequeue incoming data received by the client socket, and observe that
/// zero bytes are successfully dequeued, indicating the server socket has shut
/// down writing from its side of the connection.
///
///     {
///         char storage;
///
///         ntsa::Data data(ntsa::MutableBuffer(&storage, 1));
///
///         ntsa::ReceiveContext context;
///         ntsa::ReceiveOptions options;
///
///         error = ntsu::SocketUtil::receive(
///                                   &context, &data, options, client);
///         BSLS_ASSERT(!error || error == ntsa::Error(ntsa::Error::e_EOF));
///         BSLS_ASSERT(numBytesReceivable == 1);
///         BSLS_ASSERT(numBytesReceived == 0);
///     }
///
/// Finally, close each socket.
///
///     ntsu::SocketUtil::close(listener);
///     ntsu::SocketUtil::close(client);
///     ntsu::SocketUtil::close(server);
///
///
/// @par Usage Example 2: Datagram sockets
/// This examples shows how to exchange data between two datagram sockets. For
/// brevity, all error checking is omitted.
///
/// To start, initialize the library.
///
///     ntsa::Error error = ntscfg::Platform::initialize();
///     BSLS_ASSERT(!error);
///
/// Next, create a blocking socket for the server and bind it to any port on
/// the loopback address.
///
///     ntsa::Handle server;
///     error = ntsu::SocketUtil::create(&server,
///                                      ntsa::Transport::e_UDP_IPV4_DATAGRAM);
///     BSLS_ASSERT(!error);
///
///     error = ntsu::SocketUtil::bind(
///         ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
///         false,
///         server);
///     BSLS_ASSERT(!error);
///
///     ntsa::Endpoint serverEndpoint;
///     error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint, server);
///     BSLS_ASSERT(!error);
///
/// Next, create a blocking socket for the client and bind it to any port on
/// the loopback address.
///
///     ntsa::Handle client;
///     error = ntsu::SocketUtil::create(&client,
///                                      ntsa::Transport::e_UDP_IPV4_DATAGRAM);
///     BSLS_ASSERT(!error);
///
///     error = ntsu::SocketUtil::bind(
///         ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
///         false,
///         client);
///     BSLS_ASSERT(!error);
///
///     ntsa::Endpoint clientEndpoint;
///     error = ntsu::SocketUtil::sourceEndpoint(&clientEndpoint, client);
///     BSLS_ASSERT(!error);
///
/// Next, enqueue outgoing data to transmit by the client socket.
///
///     {
///         char storage = 'C';
///
///         ntsa::Data data(ntsa::ConstBuffer(&storage, 1));
///
///         ntsa::SendContext context;
///         ntsa::SendOptions options;
///
///         options.setEndpoint(serverEndpoint);
///
///         error = ntsu::SocketUtil::send(&context, data, options, client);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(context.bytesSent() == 1);
///     }
///
/// Next, dequeue incoming data received by the server socket.
///
///    {
///         char storage;
///
///         ntsa::Data data(ntsa::MutableBuffer(&storage, 1));
///
///         ntsa::ReceiveContext context;
///         ntsa::ReceiveOptions options;
///
///         error = ntsu::SocketUtil::receive(
///                                   &context, &data, options, server);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(!context.endpoint().isNull());
///         BSLS_ASSERT(context.endpoint().value() == clientEndpoint);
///         BSLS_ASSERT(context.bytesReceived() == 1);
///         BSLS_ASSERT(storage == 'C');
///     }
///
/// Now, enqueue outgoing data to transmit by the server socket.
///
///     {
///         char storage = 'S';
///
///         ntsa::Data data(ntsa::ConstBuffer(&storage, 1));
///
///         ntsa::SendContext context;
///         ntsa::SendOptions options;
///
///         options.setEndpoint(clientEndpoint);
///
///         error = ntsu::SocketUtil::send(&context, data, options, server);
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
///         error = ntsu::SocketUtil::receive(
///                                   &context, &data, options, client);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(!context.endpoint().isNull());
///         BSLS_ASSERT(context.endpoint().value() == serverEndpoint);
///         BSLS_ASSERT(context.bytesReceived() == 1);
///         BSLS_ASSERT(storage == 'S');
///     }
///
/// Finally, close each socket.
///
///     ntsu::SocketUtil::close(client);
///     ntsu::SocketUtil::close(server);
///
/// @ingroup module_ntsu
struct SocketUtil {
    /// Provide a guard mechanism to automatically close a socket unless
    /// otherwise released.
    class Guard;

    /// Load into the specified 'result' a handle to a new socket of the
    /// specified 'type'. Return the error.
    static ntsa::Error create(ntsa::Handle*          result,
                              ntsa::Transport::Value type);

    /// Load into the specified 'result' a new handle that refers to the
    /// same socket as the specified 'socket'. Return the error.
    static ntsa::Error duplicate(ntsa::Handle* result, ntsa::Handle socket);

    /// Bind the specified 'socket' to the specified source 'endpoint'. If
    /// the specified 'reuseAddress' flag is set, allow this socket to bind
    /// to an address already in use by the operating system. Return the
    /// error.
    static ntsa::Error bind(const ntsa::Endpoint& endpoint,
                            bool                  reuseAddress,
                            ntsa::Handle          socket);

    /// Bind the specified 'socket' to any suitable source endpoint
    /// appropriate for a socket of the specified 'type'. If the specified
    /// 'reuseAddress' flag is set, allow this socket to bind to an address
    /// already in use by the operating system. Return the error.
    static ntsa::Error bindAny(ntsa::Transport::Value type,
                               bool                   reuseAddress,
                               ntsa::Handle           socket);

    /// Listen for connections made to the source endpoint of the specified
    /// 'socket'. Return the error.
    static ntsa::Error listen(bsl::size_t backlog, ntsa::Handle socket);

    /// Connect the specified 'socket' to the specified remote 'endpoint'.
    /// Return the error.
    static ntsa::Error connect(const ntsa::Endpoint& endpoint,
                               ntsa::Handle          socket);

    /// Load into the specified 'result' a handle to an accepted connection
    /// to the source endpoint of the specified 'socket'. Return the error.
    static ntsa::Error accept(ntsa::Handle* result, ntsa::Handle socket);

    /// Enqueue the specified 'data' having the specified 'size' to the
    /// send buffer of the specified 'socket' according to the specified
    /// 'options'. Load into the specified 'context' the result of the
    /// operation. Return the error.
    static ntsa::Error send(ntsa::SendContext*       context,
                            const void*              data,
                            bsl::size_t              size,
                            const ntsa::SendOptions& options,
                            ntsa::Handle             socket);

    /// Enqueue the specified 'buffer' to the send buffer of the specified
    /// 'socket' according to the specified 'options'. Load into the
    /// specified 'context' the result of the operation. Return the error.
    static ntsa::Error send(ntsa::SendContext*       context,
                            const ntsa::ConstBuffer& buffer,
                            const ntsa::SendOptions& options,
                            ntsa::Handle             socket);

    /// Enqueue the specified 'bufferArray' having the specified
    /// 'bufferCount' to the send buffer of the specified 'socket' according
    /// to the specified 'options'. Load into the specified 'context' the
    /// result of the operation. Return the error.
    static ntsa::Error send(ntsa::SendContext*       context,
                            const ntsa::ConstBuffer* bufferArray,
                            bsl::size_t              bufferCount,
                            const ntsa::SendOptions& options,
                            ntsa::Handle             socket);

    /// Enqueue the specified 'bufferArray' to the send buffer of the
    /// specified 'socket' according to the specified 'options'. Load into
    /// the specified 'context' the result of the operation. Return the
    /// error.
    static ntsa::Error send(ntsa::SendContext*            context,
                            const ntsa::ConstBufferArray& bufferArray,
                            const ntsa::SendOptions&      options,
                            ntsa::Handle                  socket);

    /// Enqueue the specified 'bufferArray' to the send buffer of the
    /// specified 'socket' according to the specified 'options'. Load into
    /// the specified 'context' the result of the operation. Return the
    /// error.
    static ntsa::Error send(ntsa::SendContext*               context,
                            const ntsa::ConstBufferPtrArray& bufferArray,
                            const ntsa::SendOptions&         options,
                            ntsa::Handle                     socket);

    /// Enqueue the specified 'buffer' to the send buffer of the specified
    /// 'socket' according to the specified 'options'. Load into the
    /// specified 'context' the result of the operation. Return the error.
    static ntsa::Error send(ntsa::SendContext*         context,
                            const ntsa::MutableBuffer& buffer,
                            const ntsa::SendOptions&   options,
                            ntsa::Handle               socket);

    /// Enqueue the specified 'bufferArray' having the specified
    /// 'bufferCount' to the send buffer of the specified 'socket' according
    /// to the specified 'options'. Load into the specified 'context' the
    /// result of the operation. Return the error.
    static ntsa::Error send(ntsa::SendContext*         context,
                            const ntsa::MutableBuffer* bufferArray,
                            bsl::size_t                bufferCount,
                            const ntsa::SendOptions&   options,
                            ntsa::Handle               socket);

    /// Enqueue the specified 'bufferArray' to the send buffer of the
    /// specified 'socket' according to the specified 'options'. Load into
    /// the specified 'context' the result of the operation. Return the
    /// error.
    static ntsa::Error send(ntsa::SendContext*              context,
                            const ntsa::MutableBufferArray& bufferArray,
                            const ntsa::SendOptions&        options,
                            ntsa::Handle                    socket);

    /// Enqueue the specified 'bufferArray' to the send buffer of the
    /// specified 'socket' according to the specified 'options'. Load into
    /// the specified 'context' the result of the operation. Return the
    /// error.
    static ntsa::Error send(ntsa::SendContext*                 context,
                            const ntsa::MutableBufferPtrArray& bufferArray,
                            const ntsa::SendOptions&           options,
                            ntsa::Handle                       socket);

    /// Enqueue the specified 'string' to the send buffer of the
    /// specified 'socket' according to the specified 'options'. Load into
    /// the specified 'context' the result of the operation. Return the
    /// error.
    static ntsa::Error send(ntsa::SendContext*       context,
                            const bslstl::StringRef& string,
                            const ntsa::SendOptions& options,
                            ntsa::Handle             socket);

    /// Enqueue the specified 'blob' to the send buffer of the specified
    /// 'socket' according to the specified 'options'. Load into the
    /// specified 'context' the result of the operation. Return the error.
    static ntsa::Error send(ntsa::SendContext*       context,
                            const bdlbb::Blob&       blob,
                            const ntsa::SendOptions& options,
                            ntsa::Handle             socket);

    /// Enqueue the specified 'blobBuffer' to the send buffer of the
    /// specified 'socket' according to the specified 'options'. Load into
    /// the specified 'context' the result of the operation. Return the
    /// error.
    static ntsa::Error send(ntsa::SendContext*       context,
                            const bdlbb::BlobBuffer& blobBuffer,
                            const ntsa::SendOptions& options,
                            ntsa::Handle             socket);

    /// Enqueue the the specified 'file' to the send buffer of the specified
    /// 'socket' according to the specified 'options'. Load into the
    /// specified 'context' the result of the operation. Return the error.
    static ntsa::Error send(ntsa::SendContext*       context,
                            const ntsa::File&        file,
                            const ntsa::SendOptions& options,
                            ntsa::Handle             socket);

    /// Enqueue the specified 'data' to the send buffer of the specified
    /// 'socket' according to the specified 'options'. Load into the
    /// specified 'context' the result of the operation. Return the error.
    static ntsa::Error send(ntsa::SendContext*       context,
                            const ntsa::Data&        data,
                            const ntsa::SendOptions& options,
                            ntsa::Handle             socket);

    /// Send from the specified 'socket' the specified 'messages', each
    /// message describing the buffers to send and the remote endpoint to
    /// which the data referenced by those buffers should be sent. Load into
    /// the specified 'numBytesSendable' the number of bytes that the
    /// implementation tried to send, and load into the specified
    /// 'numBytesSent' the actual number of bytes sent. Load into the
    /// specified 'numMessagesSendable' the number of messages that the
    /// implementation tried to send, and load into the specified
    /// 'numMessagesSent' the actual number of messages sent. Return the
    /// error. Note that this function is only supported on Linux when
    /// the both the compile-time and run-time GNU libc version is >= 2.17;
    /// this function is not supported on any other platform.
    static ntsa::Error sendToMultiple(bsl::size_t* numBytesSendable,
                                      bsl::size_t* numBytesSent,
                                      bsl::size_t* numMessagesSendable,
                                      bsl::size_t* numMessagesSent,
                                      const ntsa::ConstMessage* messages,
                                      bsl::size_t               numMessages,
                                      ntsa::Handle              socket);

    /// Dequeue from the receive buffer of the specified 'socket' into the
    /// specified 'data' having the specified 'capacity' according to the
    /// specified 'options'. Load into the specified 'context' the result of
    /// the operation. Return the error.
    static ntsa::Error receive(ntsa::ReceiveContext*       context,
                               void*                       data,
                               bsl::size_t                 capacity,
                               const ntsa::ReceiveOptions& options,
                               ntsa::Handle                socket);

    /// Dequeue from the receive buffer of the specified 'socket' into the
    /// specified 'buffer' according to the specified 'options'. Load into
    /// the specified 'context' the result of the operation. Return the
    /// error.
    static ntsa::Error receive(ntsa::ReceiveContext*       context,
                               ntsa::MutableBuffer*        buffer,
                               const ntsa::ReceiveOptions& options,
                               ntsa::Handle                socket);

    /// Dequeue from the receive buffer of the specified 'socket' into the
    /// specified 'bufferArray' having the specified 'bufferCount' according
    /// to the specified 'options'. Load into the specified 'context' the
    /// result of the operation. Return the error.
    static ntsa::Error receive(ntsa::ReceiveContext*       context,
                               ntsa::MutableBuffer*        bufferArray,
                               bsl::size_t                 bufferCount,
                               const ntsa::ReceiveOptions& options,
                               ntsa::Handle                socket);

    /// Dequeue from the receive buffer of the specified 'socket' into the
    /// specified 'bufferArray' according to the specified 'options'. Load
    /// into the specified 'context' the result of the operation. Return the
    /// error.
    static ntsa::Error receive(ntsa::ReceiveContext*       context,
                               ntsa::MutableBufferArray*   bufferArray,
                               const ntsa::ReceiveOptions& options,
                               ntsa::Handle                socket);

    /// Dequeue from the receive buffer of the specified 'socket' into the
    /// specified 'bufferArray' according to the specified 'options'. Load
    /// into the specified 'context' the result of the operation. Return the
    /// error.
    static ntsa::Error receive(ntsa::ReceiveContext*        context,
                               ntsa::MutableBufferPtrArray* bufferArray,
                               const ntsa::ReceiveOptions&  options,
                               ntsa::Handle                 socket);

    /// Dequeue from the receive buffer of the specified 'socket' into the
    /// specified 'string' according to the specified 'options'. Load into
    /// the specified 'context' the result of the operation. Return the
    /// error.
    static ntsa::Error receive(ntsa::ReceiveContext*       context,
                               bsl::string*                string,
                               const ntsa::ReceiveOptions& options,
                               ntsa::Handle                socket);

    /// Dequeue from the receive buffer of the specified 'socket' into the
    /// specified 'data' according to the specified 'options'. Load into the
    /// specified 'context' the result of the operation. Return the error.
    static ntsa::Error receive(ntsa::ReceiveContext*       context,
                               bdlbb::Blob*                data,
                               const ntsa::ReceiveOptions& options,
                               ntsa::Handle                socket);

    /// Dequeue from the receive buffer of the specified 'socket' into the
    /// specified 'blobBuffer' according to the specified 'options'. Load
    /// into the specified 'context' the result of the operation. Return the
    /// error.
    static ntsa::Error receive(ntsa::ReceiveContext*       context,
                               bdlbb::BlobBuffer*          blob,
                               const ntsa::ReceiveOptions& options,
                               ntsa::Handle                socket);

    /// Dequeue from the receive buffer of the specified 'socket' into the
    /// specified 'data' according to the specified 'options'. Load into the
    /// specified 'context' the result of the operation. Return the error.
    static ntsa::Error receive(ntsa::ReceiveContext*       context,
                               ntsa::Data*                 data,
                               const ntsa::ReceiveOptions& options,
                               ntsa::Handle                socket);

    /// Receive the specifed 'numMessages' of 'messages' from the specified
    /// 'socket'. Load into the specified 'numBytesReceivable' the number of
    /// bytes attempted to be dequeued, and load into the specified
    /// 'numBytesReceived' the actual number of bytes dequeued. Load into
    /// the specified 'numMessagesReceivable' the number of messages
    /// attempted to be dequeued, and load into the specified
    /// 'numMessagesReceived' the actual number of messages dequeued. Return
    /// the error. Note that at most 'maxBuffersToReceive()' number of
    /// buffers in each message will be attempted to be written; additional
    /// data present in the corresponding message will be silently
    /// truncated. Note that at most 'maxMessagesToReceive()' number of
    /// messages will be attempted to be dequeued. Note that this function
    /// is only supported on Linux when the both the compile-time and
    /// run-time GNU libc version is >= 2.17; this function is not supported
    /// on any other platform.
    static ntsa::Error receiveFromMultiple(bsl::size_t* numBytesReceivable,
                                           bsl::size_t* numBytesReceived,
                                           bsl::size_t* numMessagesReceivable,
                                           bsl::size_t* numMessagesReceived,
                                           ntsa::MutableMessage* messages,
                                           bsl::size_t           numMessages,
                                           ntsa::Handle          socket);

    /// Read data from the specified 'socket' error queue. Then if the
    /// specified 'notifications' is not null parse fetched data to extract
    /// control messages into the specified 'notifications'. Return the error.
    static ntsa::Error receiveNotifications(
        ntsa::NotificationQueue* notifications,
        ntsa::Handle             socket);

    /// Shutdown the socket in the specified 'direction'. Return the error.
    static ntsa::Error shutdown(ntsa::ShutdownType::Value direction,
                                ntsa::Handle              socket);

    /// Unlink the file corresponding to the specified 'socket', if the
    /// 'socket' is a local (a.k.a. Unix domain) socket bound to a
    /// non-abstract path. Return the error. Note that this function should
    /// only be called for listening sockets and sockets acting in the
    /// connect role.
    static ntsa::Error unlink(ntsa::Handle socket);

    /// Close the specified 'socket'. Return the error.
    static ntsa::Error close(ntsa::Handle socket);

    /// Load into the specified 'result' the source endpoint of this socket.
    /// Return the error.
    static ntsa::Error sourceEndpoint(ntsa::Endpoint* result,
                                      ntsa::Handle    socket);

    /// Load into the specified 'result' the remote endpoint to which this
    /// socket is connected. Return the error.
    static ntsa::Error remoteEndpoint(ntsa::Endpoint* result,
                                      ntsa::Handle    socket);

    /// Wait indefinitely until the specified 'socket' is readable. Return
    /// the error.
    static ntsa::Error waitUntilReadable(ntsa::Handle socket);

    /// Wait until the specified 'socket' is readable or the specified
    /// absolute 'timeout' elapses. Return the error.
    static ntsa::Error waitUntilReadable(ntsa::Handle              socket,
                                         const bsls::TimeInterval& timeout);

    /// Wait indefinitely until the specified 'socket' is readable. Return
    /// the error.
    static ntsa::Error waitUntilWritable(ntsa::Handle socket);

    /// Wait until the specified 'socket' is readable or the specified
    /// absolute 'timeout' elapses. Return the error.
    static ntsa::Error waitUntilWritable(ntsa::Handle              socket,
                                         const bsls::TimeInterval& timeout);

    /// Load into the specified 'client' and 'server' a connected pair of
    /// sockets of the specified 'type'. Return the error.
    static ntsa::Error pair(ntsa::Handle*          client,
                            ntsa::Handle*          server,
                            ntsa::Transport::Value type);

    /// Load into the specified 'endpoint' the conversion of the specified
    /// 'socketAddress' having the specified 'socketAddressSize'. Return the
    /// error.
    static ntsa::Error decodeEndpoint(ntsa::Endpoint* endpoint,
                                      const void*     socketAddress,
                                      bsl::size_t     socketAddressSize);

    /// Load into the specified 'socketAddress' and 'socketAddressSize' the
    /// conversion of the specified 'endpoint'. Return the error.
    static ntsa::Error encodeEndpoint(void*                 socketAddress,
                                      bsl::size_t*          socketAddressSize,
                                      const ntsa::Endpoint& endpoint);

    /// Return the maximum number of bytes beyond which it is estimated to
    /// not be advantageous to reference for the next call to perform
    /// gathered write from multiple buffers to the specified 'socket', or
    /// 0 if this number is either not known, or the application of such
    /// limit is inappropriate for the type of the 'socket' (e.g, when the
    /// 'socket' is a datagram socket.)
    static bsl::size_t maxBytesPerSend(ntsa::Handle socket);

    /// Return the maximum number of bytes beyond which it is estimated to
    /// not be advantageous to reference for the next call to perform
    /// a scattered read to multiple buffers from the specified 'socket', or
    /// 0 if this number is either not known, or the application of such
    /// limit is inappropriate for the type of the 'socket' (e.g, when the
    /// 'socket' is a datagram socket.)
    static bsl::size_t maxBytesPerReceive(ntsa::Handle socket);

    /// Return the maximum number of buffers that can be the source of a
    /// gathered write. Additional buffers beyond this limit are silently
    /// ignored.
    static bsl::size_t maxBuffersPerSend();

    /// Return the maximum number of buffers that can be the destination
    /// of a scattered read. Additional buffers beyond this limit are
    /// silently ignored.
    static bsl::size_t maxBuffersPerReceive();

#if defined(BSLS_PLATFORM_OS_LINUX)
    /// Return the maximum number of messages that can be the source of
    /// the simultaneous transmission of multiple messages. Additional
    /// messages beyond this limit are silently ignored.
    static bsl::size_t maxMessagesPerSend();

    /// Return the maximum number of messages that can be the destination of
    /// the simultaneous reception of multiple messages. Additional
    /// messages beyond this limit are silently ignored.
    static bsl::size_t maxMessagesPerReceive();
#endif

    /// Return the maximum size of a listening socket's backlog, as
    /// specified by SOMAXCONN.
    static bsl::size_t maxBacklog();

    /// Return true if the specified 'socket' is an open, valid handle to a
    /// socket, otherwise return false.
    static bool isSocket(ntsa::Handle socket);

    /// Write a formatted, human-readable table summarizing the information
    /// of all sockets in the operating system to the specified 'stream'.
    /// Filter the sockets reported according to the specified 'filter'.
    /// Return the error.
    static ntsa::Error reportInfo(bsl::ostream&                 stream,
                                  const ntsa::SocketInfoFilter& filter);

    /// Load into the specified 'result' the information of all sockets in
    /// the operating system. Filter the sockets reported according to the specified 'filter'.
    /// Return the error.
    static ntsa::Error reportInfo(bsl::vector<ntsa::SocketInfo>* result,
                                  const ntsa::SocketInfoFilter&  filter);
};

/// @internal @brief
/// Provide a guard mechanism to automatically close a socket unless otherwise
/// released.
class SocketUtil::Guard
{
    ntsa::Handle d_handle;

  public:
    /// Create a new socket handle guard that automatically closes the
    /// specified 'handle' unless dismissed.
    explicit Guard(ntsa::Handle handle)
    : d_handle(handle)
    {
    }

    /// Close the underlying socket handle unless dismissed and destroy this
    /// object.
    ~Guard()
    {
        if (d_handle != ntsa::k_INVALID_HANDLE) {
            ntsu::SocketUtil::close(d_handle);
        }
    }

    /// Dismiss the guard from managing the underlying socket handle.
    ntsa::Handle release()
    {
        ntsa::Handle result = d_handle;
        d_handle            = ntsa::k_INVALID_HANDLE;
        return result;
    }
};

}  // close package namespace
}  // close enterprise namespace
#endif
