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

#ifndef INCLUDED_NTSI_DATAGRAMSOCKET
#define INCLUDED_NTSI_DATAGRAMSOCKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_buffer.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_message.h>
#include <ntsa_notificationqueue.h>
#include <ntsa_receivecontext.h>
#include <ntsa_receiveoptions.h>
#include <ntsa_sendcontext.h>
#include <ntsa_sendoptions.h>
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

/// Provide a blocking or non-blocking socket whose transport has datagram
/// semantics.
///
/// @details
/// Provide an abstract mechanism to perform unicast or multicast
/// communication between endpoints using a transport that has datagram
/// semantics. Communication with datagram semantics is characterized as
/// message-oriented, connectionless, unreliable, and unordered. The public
/// member functions of this class roughly correspond to the datagram-type
/// socket functionality of the Berkeley Sockets API in the POSIX
/// specification.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @par Usage Example: Sending and Receiving Unicast Messages
/// This examples shows how to use datagram sockets to send and receive
/// unicast messages. Unicast messaging transmits a single message to a single
/// recipient.
///
/// To start, initialize the library.
///
///     ntsa::Error error = ntsf::System::initialize();
///     BSLS_ASSERT(!error);
///
/// Now, let's create the datagram sockets and bind them to their local
/// addresses. First, create a blocking socket for the server and bind it to
/// any port on the loopback address.
///
///     bsl::shared_ptr<ntsi::DatagramSocket> server =
///                                   ntsf::System::createDatagramSocket();
///
///     error = server->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
///     BSLS_ASSERT(!error);
///
///     error = server->bind(ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
///                          false);
///     BSLS_ASSERT(!error);
///
///     ntsa::Endpoint serverEndpoint;
///     error = server->sourceEndpoint(&serverEndpoint);
///     BSLS_ASSERT(!error);
///
/// Next, create a blocking socket for the client and bind it to any port on
/// the loopback address.
///
///     bsl::shared_ptr<ntsi::DatagramSocket> client =
///                                   ntsf::System::createDatagramSocket();
///
///     error = client->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
///     BSLS_ASSERT(!error);
///
///     error = client->bind(ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
///                          false);
///     BSLS_ASSERT(!error);
///
///     ntsa::Endpoint clientEndpoint;
///     error = client->sourceEndpoint(&clientEndpoint);
///     BSLS_ASSERT(!error);
///
/// Now, let's send data from the client to the server. First, enqueue outgoing
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
///         options.setEndpoint(serverEndpoint);
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
///         BSLS_ASSERT(!context.endpoint().isNull());
///         BSLS_ASSERT(context.endpoint().value() == clientEndpoint);
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
///         options.setEndpoint(clientEndpoint);
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
///         BSLS_ASSERT(!context.endpoint().isNull());
///         BSLS_ASSERT(context.endpoint().value() == serverEndpoint);
///         BSLS_ASSERT(context.bytesReceived() == 1);
///         BSLS_ASSERT(storage == 'S');
///     }
///
///
/// @par Usage Example: Sending and Receiving Multicast Messages
/// This examples shows how to use datagram sockets to send and receive
/// multicast messages. Multicast messaging transmits a single message to
/// multiple recipients.
///
/// To start, initialize the library.
///
///     ntsa::Error error = ntsf::System::initialize();
///     BSLS_ASSERT(!error);
///
/// Next, find the first network interface that supports IPv4 multicasting, but
/// note that multicasting may only be performed by network interfaces that
/// are explicitly enabled for it.
///
///     ntsa::Adapter adapter;
///     if (!ntsf::System::discoverAdapter(&adapter,
///                                        ntsa::IpAddressType::e_V4,
///                                        true))
///     {
///         bsl::abort();
///     }
///
/// Next, choose a multicast group and UDP port number used in this example.
///
///     const ntsa::IpAddress MULTICAST_GROUP = ntsa::IpAddress("234.5.6.7");
///     const unsigned short  MULTICAST_PORT  = 42800;
///
/// Now, let's create the datagram sockets and bind them to their local
/// addresses. First, create a blocking socket for the client and bind it to
/// any port on the multicasting network interface.
///
///     bsl::shared_ptr<ntsi::DatagramSocket> client =
///                                   ntsf::System::createDatagramSocket();
///
///     error = client->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
///     BSLS_ASSERT(!error);
///
///     error = client->bind(ntsa::Endpoint(adapter.ipv4Address().value(), 0),
///                                         false);
///     BSLS_ASSERT(!error);
///
///     ntsa::Endpoint clientEndpoint;
///     error = client->sourceEndpoint(&clientEndpoint);
///     BSLS_ASSERT(!error);
///
/// Next, create a blocking socket for the first server and bind it to the
/// chosen port on the multicasting network interface.
///
///     bsl::shared_ptr<ntsi::DatagramSocket> serverOne =
///                                   ntsf::System::createDatagramSocket();
///
///     error = serverOne->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
///     BSLS_ASSERT(!error);
///
///     error = serverOne->bind(
///         ntsa::Endpoint(ntsa::Ipv4Address::any(), MULTICAST_PORT), true);
///     BSLS_ASSERT(!error);
///
///     ntsa::Endpoint serverOneEndpoint;
///     error = serverOne->sourceEndpoint(&serverOneEndpoint);
///     BSLS_ASSERT(!error);
///
/// Next, create a blocking socket for the second server and bind it to the
/// chosen port on the multicasting network interface.
///
///     bsl::shared_ptr<ntsi::DatagramSocket> serverTwo =
///                                   ntsf::System::createDatagramSocket();
///
///     error = serverTwo->open(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
///     BSLS_ASSERT(!error);
///
///     error = serverTwo->bind(
///         ntsa::Endpoint(ntsa::Ipv4Address::any(), MULTICAST_PORT), true);
///     BSLS_ASSERT(!error);
///
///     ntsa::Endpoint serverTwoEndpoint;
///     error = serverTwo->sourceEndpoint(&serverTwoEndpoint);
///     BSLS_ASSERT(!error);
///
/// Now, let's configure multicast publication. Configure the client socket to
/// send multicast data on specific multicasting network interface limited to
/// the host machine.
///
///     error = client->setMulticastInterface(
///                        ntsa::IpAddress(adapter.ipv4Address().value()));
///     BSLS_ASSERT(!error);
///
///     error = client->setMulticastTimeToLive(0);
///     BSLS_ASSERT(!error);
///
/// Now, let's configure multicast subscription. First, join the first server
/// socket to the multicast group.
///
///     error = serverOne->joinMulticastGroup(
///         ntsa::IpAddress(adapter.ipv4Address().value()), MULTICAST_GROUP);
///     BSLS_ASSERT(!error);
///
/// Next, join the second server socket to the multicast group.
///
///     error = serverTwo->joinMulticastGroup(
///         ntsa::IpAddress(adapter.ipv4Address().value()), MULTICAST_GROUP);
///     BSLS_ASSERT(!error);
///
/// Now, let's send multicast data from the client to the server. First,
/// enqueue outgoing data to transmit by the client socket.
///
///     {
///         ntsa::Endpoint multicastEndpoint(MULTICAST_GROUP, MULTICAST_PORT);
///
///         char storage = 'C';
///
///         ntsa::Data data(ntsa::ConstBuffer(&storage, 1));
///
///         ntsa::SendContext context;
///         ntsa::SendOptions options;
///
///         options.setEndpoint(multicastEndpoint);
///
///         error = client->send(&context, data, options);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(context.bytesSent() == 1);
///     }
///
/// Next, dequeue incoming data received by the first server socket.
///
///     {
///         char storage;
///
///         ntsa::Data data(ntsa::MutableBuffer(&storage, 1));
///
///         ntsa::ReceiveContext context;
///         ntsa::ReceiveOptions options;
///
///         error = serverOne->receive(&context, &data, options);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(!context.endpoint().isNull());
///         BSLS_ASSERT(context.endpoint().value() == clientEndpoint);
///         BSLS_ASSERT(context.bytesReceived() == 1);
///         BSLS_ASSERT(storage == 'C');
///     }
///
/// Next, dequeue incoming data received by the second server socket.
///
///     {
///         char storage;
///
///         ntsa::Data data(ntsa::MutableBuffer(&storage, 1));
///
///         ntsa::ReceiveContext context;
///         ntsa::ReceiveOptions options;
///
///         error = serverTwo->receive(&context, &data, options);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(!context.endpoint().isNull());
///         BSLS_ASSERT(context.endpoint().value() == clientEndpoint);
///         BSLS_ASSERT(context.bytesReceived() == 1);
///         BSLS_ASSERT(storage == 'C');
///     }
///
/// Now, let's send unicast data from the servers to the client. First, enqueue
/// outgoing data to transmit by the first server socket.
///
///     {
///         char storage = '1';
///
///         ntsa::Data data(ntsa::ConstBuffer(&storage, 1));
///
///         ntsa::SendContext context;
///         ntsa::SendOptions options;
///
///         options.setEndpoint(clientEndpoint);
///
///         error = serverOne->send(&context, data, options);
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
///         BSLS_ASSERT(!context.endpoint().isNull());
///         BSLS_ASSERT(
///             context.endpoint().value() ==
///             ntsa::Endpoint(adapter.ipv4Address().value(), MULTICAST_PORT));
///         BSLS_ASSERT(context.bytesReceived() == 1);
///         BSLS_ASSERT(storage == '1');
///     }
///
/// Next, enqueue outgoing data to transmit by the second server socket.
///
///     {
///         char storage = '2';
///
///         ntsa::Data data(ntsa::ConstBuffer(&storage, 1));
///
///         ntsa::SendContext context;
///         ntsa::SendOptions options;
///
///         options.setEndpoint(clientEndpoint);
///
///         error = serverTwo->send(&context, data, options);
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
///         BSLS_ASSERT(!context.endpoint().isNull());
///         BSLS_ASSERT(
///             context.endpoint().value() ==
///             ntsa::Endpoint(adapter.ipv4Address().value(), MULTICAST_PORT));
///         BSLS_ASSERT(context.bytesReceived() == 1);
///         BSLS_ASSERT(storage == '2');
///     }
///
///
/// @ingroup module_ntsi
class DatagramSocket : public ntsi::Channel
{
  public:
    /// Destroy this object.
    ~DatagramSocket() BSLS_KEYWORD_OVERRIDE;

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
    virtual ntsa::Error send(ntsa::SendContext*       context,
                             const ntsa::ConstBuffer *data,
                             bsl::size_t              size,
                             const ntsa::SendOptions& options);

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
    /// Return the error.
    virtual ntsa::Error unlink();

    /// Close the socket. Return the error.
    virtual ntsa::Error close();

    /// Load into the specified 'result' the source endpoint of this socket.
    /// Return the error.
    virtual ntsa::Error sourceEndpoint(ntsa::Endpoint* result) const;

    /// Load into the specified 'result' the remote endpoint to which this
    /// socket is connected. Return the error.
    virtual ntsa::Error remoteEndpoint(ntsa::Endpoint* result) const;

    // *** Multicasting ***

    /// Set the flag that indicates multicast datagrams should be looped
    /// back to the local host to the specified 'value'. Return the error.
    virtual ntsa::Error setMulticastLoopback(bool enabled);

    /// Set the network interface on which multicast datagrams will be
    /// sent to the network interface assigned the specified 'interface'
    /// address. Return the error.
    virtual ntsa::Error setMulticastInterface(
        const ntsa::IpAddress& interface);

    /// Set the multicast time-to-live to the specified 'numHops'. Return
    /// the error.
    virtual ntsa::Error setMulticastTimeToLive(bsl::size_t maxHops);

    /// Join the specified multicast 'group' on the adapter identified by
    /// the specified 'interface'. Return the error.
    virtual ntsa::Error joinMulticastGroup(const ntsa::IpAddress& interface,
                                           const ntsa::IpAddress& group);

    /// Leave the specified multicast 'group' on the adapter identified by
    /// the specified 'interface'. Return the error.
    virtual ntsa::Error leaveMulticastGroup(const ntsa::IpAddress& interface,
                                            const ntsa::IpAddress& group);

    /// Join the specified source-specific multicast 'group' on the adapter
    /// identified by the specified 'interface' and allow receiving datagrams
    /// only from the specified 'source'. Return the error. Note that this
    /// function may be called multiple times to allow receiving multicast
    /// datagrams only from a particular set of source addresses.
    virtual ntsa::Error joinMulticastGroupSource(
        const ntsa::IpAddress& interface,
        const ntsa::IpAddress& group,
        const ntsa::IpAddress& source);

    /// Leave the specified source-specific multicast 'group' on the adapter
    /// identified by the specified 'interface', disallowing datagrams sent by
    /// the specified 'source'. Return the error. If the 'socket' has
    /// subscribed to multiple sources within the same group, data from the
    /// remaining sources will still be delivered. To stop receiving data from
    /// all sources at once, use 'leaveMulticastGroup'.
    virtual ntsa::Error leaveMulticastGroupSource(
        const ntsa::IpAddress& interface,
        const ntsa::IpAddress& group,
        const ntsa::IpAddress& source);

    // *** Socket Options ***

    /// Set the option for the 'socket' that controls its blocking mode
    /// according to the specified 'blocking' flag. Return the error.
    virtual ntsa::Error setBlocking(bool blocking);

    /// Set the specified 'option' for this socket. Return the error.
    virtual ntsa::Error setOption(const ntsa::SocketOption& option);

    /// Load into the specified 'blocking' flag the blocking mode of the
    /// specified 'socket'. Return the error.
    virtual ntsa::Error getBlocking(bool* blocking) const;

    /// Load into the specified 'option' the socket option of the specified
    /// 'type' set for this socket. Return the error.
    virtual ntsa::Error getOption(ntsa::SocketOption*           option,
                                  ntsa::SocketOptionType::Value type);

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
ntsa::Error DatagramSocket::send(ntsa::SendContext*       context,
                                 const void*              data,
                                 bsl::size_t              size,
                                 const ntsa::SendOptions& options)
{
    return this->send(context,
                      ntsa::Data(ntsa::ConstBuffer(data, size)),
                      options);
}

NTSCFG_INLINE
ntsa::Error DatagramSocket::receive(ntsa::ReceiveContext*       context,
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
