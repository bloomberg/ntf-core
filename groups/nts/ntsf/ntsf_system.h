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

#ifndef INCLUDED_NTSF_SYSTEM
#define INCLUDED_NTSF_SYSTEM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_adapter.h>
#include <ntsa_error.h>
#include <ntsa_reactorconfig.h>
#include <ntsa_resolverconfig.h>
#include <ntsa_socketinfo.h>
#include <ntsa_socketinfofilter.h>
#include <ntsa_socketstate.h>
#include <ntsa_transport.h>
#include <ntscfg_platform.h>
#include <ntsi_datagramsocket.h>
#include <ntsi_listenersocket.h>
#include <ntsi_reactor.h>
#include <ntsi_resolver.h>
#include <ntsi_streamsocket.h>
#include <ntsscm_version.h>
#include <bslma_managedptr.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntsf {

/// Provide a factory for objects implemented by the system.
///
/// @details
/// This component provides utilities to create objects that implement the
/// abstract mechanisms and interfaces in the 'ntsi' package using the system.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @par Usage Example 1: Creating Datagram Sockets for Unicasting
/// This examples shows how to use datagram sockets to send and receive unicast
/// messages. Unicast messaging transmits a single message to a single
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
///         BSLS_ASSERT(!context.endpoint().isNull();
///         BSLS_ASSERT(context.endpoint().value() == serverEndpoint);
///         BSLS_ASSERT(context.bytesReceived() == 1);
///         BSLS_ASSERT(storage == 'S');
///     }
///
///
/// @par Usage Example 2: Creating Datagram Sockets for Multicasting
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
/// note that multicasting may only be performed by network interfaces that are
/// explicitly enabled for it.
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
///         error = serverTwo->receive(&context,&data, options);
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
/// @par Usage Example 3: Creating Listener Sockets to Accept Connections
/// This example shows how to use listener sockets to accept connections
/// initiated by stream sockets.
///
/// To start, initialize the library.
///
///     ntsa::Error error = ntsf::System::initialize();
///     BSLS_ASSERT(!error);
///
/// Next, create the listener socket, bind it to any port on the loopback
/// address, then begin listening for connections.
///
///     bsl::shared_ptr<ntsi::ListenerSocket> listener =
///                                   ntsf::System::createListenerSocket();
///
///     error = listener->open(ntsa::Transport::e_TCP_IPV4_STREAM);
///     BSLS_ASSERT(!error);
///
///     error = listener->bind(ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
///                            false);
///     BSLS_ASSERT(!error);
///
///     error = listener->listen(1);
///     BSLS_ASSERT(!error);
///
/// Next, create a blocking socket for the client, then connect that socket to
/// the listener socket's local endpoint.
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
/// Finally, create a blocking socket for the server by accepting the connection
/// made to the listener socket.
///
///     bsl::shared_ptr<ntsi::StreamSocket> server;
///     error = listener->accept(&server);
///     BSLS_ASSERT(!error);
///
/// @par Usage Example 4: Creating Stream Sockets for Unicasting
/// This examples shows how to use stream sockets to send and receive data
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
///     error = listener->bind(ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
///                            false);
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
/// Now, let's send data from the server to the client. First, enqueue
/// outgoing data to transmit by the server socket.
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
///     return 0;
///
///
/// @par Usage Example 5: Resolving the Local Hostname
/// This example shows how to use a resolver to get the local hostname.
///
/// To start, initialize the library.
///
///     ntsa::Error error = ntsf::System::initialize();
///     BSLS_ASSERT(!error);
///
/// Next, create a resolver.
///
///     bsl::shared_ptr<ntsi::Resolver> resolver =
///                                     ntsf::System::createResolver();
///
/// Next, get the unqualified lost hostname.
///
///     bsl::string hostname;
///     error = resolver->getHostname(&hostname);
///     BSLS_ASSERT(!error);
///
/// Next, get the fully-qualified domain name of the local host.
///
///     bsl::string hostnameFullyQualified;
///     resolver->getHostnameFullyQualified(&hostnameFullyQualified);
///     BSLS_ASSERT(!error);
///
///
/// @par Usage Example 6: Resolving a Domain Name to an IP Address
/// This example shows how to use a resolver to resolve a domain name to an
/// IP address.
///
/// To start, initialize the library.
///
///     ntsa::Error error = ntsf::System::initialize();
///     BSLS_ASSERT(!error);
///
/// Next, create a resolver.
///
///     bsl::shared_ptr<ntsi::Resolver> resolver =
///                                     ntsf::System::createResolver();
///
/// Next, resolve "dns.google.com" to the IP addresses it is assigned.
///
///     bsl::vector<ntsa::IpAddress> addressList;
///     error = resolver->getIpAddress(&addressList, "dns.google.com");
///     BSLS_ASSERT(!error);
///
/// Next, verify that the resolver has resolved "dns.google.com" into at least
/// some of the expected IP addresses (assigned as of January 1, 2020.)
///
///     bsl::set<ntsa::IpAddress> addressSet;
///     addressSet.insert(ntsa::IpAddress("8.8.8.8"));
///     addressSet.insert(ntsa::IpAddress("8.8.4.4"));
///
///     for (bsl::vector<ntsa::IpAddress>::const_iterator
///             it  = addressList.begin();
///             it != addressList.end();
///           ++it)
///     {
///         bsl::size_t n = addressSet.erase(*it);
///         BSLS_ASSERT(n == 1);
///     }
///
///     BSLS_ASSERT(addressSet.empty());
///
///
/// @par Usage Example 7: Resolving an IP Address to a Domain Name
/// This example shows how to use a resolver to resolve an IP address to a
/// domain name.
///
/// To start, initialize the library.
///
///     ntsa::Error error = ntsf::System::initialize();
///     BSLS_ASSERT(!error);
///
/// Next, create a resolver.
///
///     bsl::shared_ptr<ntsi::Resolver> resolver =
///                                     ntsf::System::createResolver();
///
/// Next, resolve "8.8.8.8", one of the IP addresses of Google's Public DNS
/// servers, to the domain name assigned that IP address (assigned as of
/// January 1, 2020.)
///
///     bsl::string domainName;
///     error = resolver->getDomainName(&domainName,
///                                     ntsa::IpAddress("8.8.8.8"));
///     BSLS_ASSERT(!error);
///
///     BSLS_ASSERT(domainName == "dns.google");
///
/// @par Usage Example 8: Enumerating the Network Devices on the Local Host
/// This example shows how to use enumerate the network interfaces available
/// on the local host. A network interface is a device managed by the operating
/// system to transmit and receive data over a network. Note that enumerating
/// the network interfaces is potentially an expensive operation. Users should
/// prefer to cache the results when possible and appropriate.
///
/// To start, initialize the library.
///
///     ntsa::Error error = ntsf::System::initialize();
///     BSLS_ASSERT(!error);
///
/// Next, enumerate the network interfaces available on the local host, and
/// print their properties to the log.
///
///     bsl::vector<ntsa::Adapter> adapterList;
///     ntsf::System::discoverAdapterList(&adapterList);
///
///     for (bsl::vector<ntsa::Adapter>::const_iterator
///             it  = adapterList.begin();
///             it != adapterList.end();
///           ++it)
///     {
///         bsl::string name            = it->name();
///         bsl::size_t index           = it->index();
///         bool        multicast       = it->multicast();
///         bsl::string ethernetAddress = it->ethernetAddress();
///         bsl::string ipv4Address;
///         bsl::string ipv6Address;
///
///
///         if (!it->ipv4Address().isNull()) {
///             ipv4Address = it->ipv4Address().value().text();
///         }
///
///         if (!it->ipv6Address().isNull()) {
///             ipv6Address = it->ipv6Address().value().text();
///         }
///
///         BSLS_LOG_INFO("Discovered adapter"
///                       "\nName:     %s"
///                       "\nIndex:    %d"
///                       "\nEthernet: %s"
///                       "\nIPv4:     %s"
///                       "\nIPv6:     %s"
///                       "\nMulticast %s",
///                       name.c_str(),
///                       index,
///                       ethernetAddress.c_str(),
///                       ipv4Address.c_str(),
///                       ipv6Address.c_str(),
///                       (multicast ? "YES" : "NO"));
///     }
///
/// @ingroup module_ntsi
struct System {
  public:
    /// Initialize the current process to meet the requirements of this
    /// library and acquire the resources necessary for its implementation.
    /// Return the error.
    static ntsa::Error initialize();

    /// Ignore the specified 'signal'. Return the error.
    static ntsa::Error ignore(ntscfg::Signal::Value signal);

    /// Load into the specified 'result' a handle to a new datagram socket
    /// of the specified 'type'. Return the error.
    static ntsa::Error createDatagramSocket(ntsa::Handle*          result,
                                            ntsa::Transport::Value type);

    /// Create a new, uninitialized datagram socket. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    static bsl::shared_ptr<ntsi::DatagramSocket> createDatagramSocket(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new datagram socket implemented using the specified
    /// 'handle'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    static bsl::shared_ptr<ntsi::DatagramSocket> createDatagramSocket(
        ntsa::Handle      handle,
        bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'client' and 'server' a connected pair of
    /// datagram sockets of the specified 'type'. Return the error.
    static ntsa::Error createDatagramSocketPair(ntsa::Handle*          client,
                                                ntsa::Handle*          server,
                                                ntsa::Transport::Value type);

    /// Load into the specified 'client' and 'server' a connected pair of
    /// datagram sockets of the specified 'type'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    static ntsa::Error createDatagramSocketPair(
        bslma::ManagedPtr<ntsi::DatagramSocket>* client,
        bslma::ManagedPtr<ntsi::DatagramSocket>* server,
        ntsa::Transport::Value                   type,
        bslma::Allocator*                        basicAllocator = 0);

    /// Load into the specified 'client' and 'server' a connected pair of
    /// datagram sockets of the specified 'type'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    static ntsa::Error createDatagramSocketPair(
        bsl::shared_ptr<ntsi::DatagramSocket>* client,
        bsl::shared_ptr<ntsi::DatagramSocket>* server,
        ntsa::Transport::Value                 type,
        bslma::Allocator*                      basicAllocator = 0);

    /// Load into the specified 'result' a handle to a new listener socket
    /// of the specified 'type'. Return the error.
    static ntsa::Error createListenerSocket(ntsa::Handle*          result,
                                            ntsa::Transport::Value type);

    /// Create a new, uninitialized listener socket. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    static bsl::shared_ptr<ntsi::ListenerSocket> createListenerSocket(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new listener socket implemented using the specified
    /// 'handle'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    static bsl::shared_ptr<ntsi::ListenerSocket> createListenerSocket(
        ntsa::Handle      handle,
        bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' a handle to a new stream socket
    /// of the specified 'type'. Return the error.
    static ntsa::Error createStreamSocket(ntsa::Handle*          result,
                                          ntsa::Transport::Value type);

    /// Create a new, uninitialized stream socket. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    static bsl::shared_ptr<ntsi::StreamSocket> createStreamSocket(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new stream socket implemented using the specified 'handle'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static bsl::shared_ptr<ntsi::StreamSocket> createStreamSocket(
        ntsa::Handle      handle,
        bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'client' and 'server' a connected pair of
    /// stream sockets of the specified 'type'. Return the error.
    static ntsa::Error createStreamSocketPair(ntsa::Handle*          client,
                                              ntsa::Handle*          server,
                                              ntsa::Transport::Value type);

    /// Load into the specified 'client' and 'server' a connected pair of
    /// stream sockets of the specified 'type'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    static ntsa::Error createStreamSocketPair(
        bslma::ManagedPtr<ntsi::StreamSocket>* client,
        bslma::ManagedPtr<ntsi::StreamSocket>* server,
        ntsa::Transport::Value                 type,
        bslma::Allocator*                      basicAllocator = 0);

    /// Load into the specified 'client' and 'server' a connected pair of
    /// stream sockets of the specified 'type'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    static ntsa::Error createStreamSocketPair(
        bsl::shared_ptr<ntsi::StreamSocket>* client,
        bsl::shared_ptr<ntsi::StreamSocket>* server,
        ntsa::Transport::Value               type,
        bslma::Allocator*                    basicAllocator = 0);

    /// Create a new reactor. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static bsl::shared_ptr<ntsi::Reactor> createReactor(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new reactor having the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    static bsl::shared_ptr<ntsi::Reactor> createReactor(
        const ntsa::ReactorConfig& configuration,
        bslma::Allocator*          basicAllocator = 0);

    /// Create a new resolver. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static bsl::shared_ptr<ntsi::Resolver> createResolver(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new resolver having the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    static bsl::shared_ptr<ntsi::Resolver> createResolver(
        const ntsa::ResolverConfig& configuration,
        bslma::Allocator*           basicAllocator = 0);

    /// Bind the specified 'socket' to the specified source 'endpoint'. If
    /// the specified 'reuseAddress' flag is set, allow this socket to bind
    /// to an address already in use by the operating system. Return the
    /// error.
    static ntsa::Error bind(ntsa::Handle          socket,
                            const ntsa::Endpoint& endpoint,
                            bool                  reuseAddress);

    /// Bind the specified 'socket' to any suitable source endpoint
    /// appropriate for a socket of the specified 'type'. If the specified
    /// 'reuseAddress' flag is set, allow this socket to bind to an address
    /// already in use by the operating system. Return the error.
    static ntsa::Error bindAny(ntsa::Handle           socket,
                               ntsa::Transport::Value type,
                               bool                   reuseAddress);

    /// Listen for connections made to the source endpoint of the specified
    /// 'socket'. Return the error.
    static ntsa::Error listen(ntsa::Handle socket, bsl::size_t backlog);

    /// Connect the specified 'socket' to the specified remote 'endpoint'.
    /// Return the error.
    static ntsa::Error connect(ntsa::Handle          socket,
                               const ntsa::Endpoint& endpoint);

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

    /// Enqueue the specified 'data' to the send buffer of the specified
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

    /// Dequeue from the receive buffer of the specified 'socket' into the
    /// specified 'data' having the specified 'capacity'. Load into the
    /// specified 'numBytesReceived' the result of the operation. Return the
    /// error.
    static ntsa::Error receive(bsl::size_t* numBytesReceived,
                               void*        data,
                               bsl::size_t  capacity,
                               ntsa::Handle socket);

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
    /// specified 'blob' according to the specified 'options'. Load into the
    /// specified 'context' the result of the operation. Return the error.
    static ntsa::Error receive(ntsa::ReceiveContext*       context,
                               bdlbb::Blob*                blob,
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

    /// Issue an IGMP message to add the specified 'socket' to the
    /// specified multicast 'group' on the adapter identified by the
    /// specified 'interface'. Return the error.
    static ntsa::Error joinMulticastGroup(ntsa::Handle           socket,
                                          const ntsa::IpAddress& interface,
                                          const ntsa::IpAddress& group);

    /// Issue an IGMP message to remove the specified 'socket' from the
    /// specified multicast 'group' on the adapter identified by the
    /// specified 'interface'. Return the error.
    static ntsa::Error leaveMulticastGroup(ntsa::Handle           socket,
                                           const ntsa::IpAddress& interface,
                                           const ntsa::IpAddress& group);

    /// Issue an IGMP message to add the specified 'socket' to the specified
    /// source-specific multicast 'group' on the adapter identified by the
    /// specified 'interface' and allow receiving datagrams only from the
    /// specified 'source'. Return the error. Note that this function may be
    /// called multiple times to allow receiving multicast datagrams only from
    /// a particular set of source addresses.
    static ntsa::Error joinMulticastGroupSource(
        ntsa::Handle           socket,
        const ntsa::IpAddress& interface,
        const ntsa::IpAddress& group,
        const ntsa::IpAddress& source);

    /// Issue an IGMP message to remove the specified 'socket' from the
    /// specified source-specific multicast 'group' on the adapter identified
    /// by the specified 'interface', disallowing datagrams sent by the
    /// specified 'source'. Return the error. If the 'socket' has subscribed to
    /// multiple sources within the same group, data from the remaining sources
    /// will still be delivered.  To stop receiving data from all sources at
    /// once, use 'leaveMulticastGroup'.
    static ntsa::Error leaveMulticastGroupSource(
        ntsa::Handle           socket,
        const ntsa::IpAddress& interface,
        const ntsa::IpAddress& group,
        const ntsa::IpAddress& source);

    /// Set the option for the 'socket' that controls its blocking mode
    /// according to the specified 'blocking' flag. Return the error.
    static ntsa::Error setBlocking(ntsa::Handle socket, bool blocking);

    /// Set the specified 'option' for the specified 'socket'. Return the
    /// error.
    static ntsa::Error setOption(ntsa::Handle              socket,
                                 const ntsa::SocketOption& option);

    /// Set the flag that indicates multicast datagrams should be looped
    /// back to the local host to the specified 'value'. Return the error.
    static ntsa::Error setMulticastLoopback(ntsa::Handle socket, bool enabled);

    /// Set the network interface on which multicast datagrams will be
    /// sent for the specified 'socket' to the network interface assigned
    /// the specified 'interface' address. Return the error.
    static ntsa::Error setMulticastInterface(ntsa::Handle           socket,
                                             const ntsa::IpAddress& interface);

    /// Set multicast on the specified 'socket' limited to the specified
    /// 'numHops'. Return the error.
    static ntsa::Error setMulticastTimeToLive(ntsa::Handle socket,
                                              bsl::size_t  maxHops);

    /// Load into the specified 'option' the socket option of the specified
    /// 'type' for the specified 'socket'. Return the error.
    static ntsa::Error getOption(ntsa::SocketOption*           option,
                                 ntsa::Handle                  socket,
                                 ntsa::SocketOptionType::Value type);

    /// Load into the specified 'blocking' flag the blocking mode of the
    /// specified 'socket'. Return the error. Note that this function always
    /// returns an error on Windows, as determination of the blocking mode is
    /// not supported on that platform.
    static ntsa::Error getBlocking(ntsa::Handle socket, bool* blocking);

    /// Load into the specified 'result' the source (i.e. local) endpoint of
    /// the specified 'socket'. Return the error.
    static ntsa::Error getSourceEndpoint(ntsa::Endpoint* result,
                                         ntsa::Handle    socket);

    /// Load into the specified 'result' the remote (i.e. peer) endpoint to
    /// which this socket is connected. Return the error.
    static ntsa::Error getRemoteEndpoint(ntsa::Endpoint* result,
                                         ntsa::Handle    socket);

    /// Load into the specified 'result' an estimation of the number of
    /// bytes that may be copied from the socket receive buffer of the
    /// specified 'socket'. Return the error.
    static ntsa::Error getBytesReadable(bsl::size_t* result,
                                        ntsa::Handle socket);

    /// Load into the specified 'result' an estimation of the number of
    /// bytes that may be copied to the socket send buffer of the
    /// specified 'socket'. Return the error.
    static ntsa::Error getBytesWritable(bsl::size_t* result,
                                        ntsa::Handle socket);

    /// Load into the specified 'error' the last known error encountered
    /// by the specified 'socket'. Return the error (retrieving the error).
    static ntsa::Error getLastError(ntsa::Error* error, ntsa::Handle socket);

    /// Shutdown the socket in the specified 'direction'. Return the error.
    static ntsa::Error shutdown(ntsa::Handle              socket,
                                ntsa::ShutdownType::Value direction);

    /// Load into the specified 'result' a new handle that refers to the
    /// same socket as the specified 'socket'. Return the error.
    static ntsa::Error duplicate(ntsa::Handle* result, ntsa::Handle socket);

    /// Unlink the file corresponding to the specified 'socket', if the
    /// 'socket' is a local (a.k.a. Unix domain) socket bound to a
    /// non-abstract path. Return the error. Note that this function should
    /// only be called for listening sockets and sockets acting in the
    /// connect role.
    static ntsa::Error unlink(ntsa::Handle socket);

    /// Close the specified 'socket'. Return the error.
    static ntsa::Error close(ntsa::Handle socket);

    /// Set the specified 'domainName' to resolve to the specified
    /// 'ipAddressList', and vice-versa. Return the error. Note that calling
    /// this function affects the future behavior of the default resolver only:
    /// 'ipAddressList' will be subsequently returned from calling
    /// 'getIpAddress()' of 'domainName', and 'domainName' will be returned
    /// from calling 'getDomainName()' of any of the addresses in
    /// 'ipAddressList', but this function does not affect any name resolution
    /// caches maintained elsewhere by the system. Also note that calling this
    /// function is synonymous with calling the equivalent function on the
    /// the default resolver.
    static ntsa::Error setIpAddress(
        const bslstl::StringRef&            domainName,
        const bsl::vector<ntsa::IpAddress>& ipAddressList);

    /// Add the specified 'domainName' to resolve to the specified
    /// 'ipAddressList', and vice-versa, in addition to any previously,
    /// explicitly defined associations. Return the error. Note that calling
    /// this function affects the future behavior of the default resolver only:
    /// 'ipAddressList' will be subsequently returned from calling
    /// 'getIpAddress()' of 'domainName', and 'domainName' will be returned
    /// from calling 'getDomainName()' of any of the addresses in
    /// 'ipAddressList', but this function does not affect any name resolution
    /// caches maintained elsewhere by the system. Also note that calling this
    /// function is synonymous with calling the equivalent function on the
    /// the default resolver.
    static ntsa::Error addIpAddress(
        const bslstl::StringRef&            domainName,
        const bsl::vector<ntsa::IpAddress>& ipAddressList);

    /// Add the specified 'domainName' to resolve to the specified 'ipAddress',
    /// and vice-versa, in addition to any previously, explicitly defined
    /// associations. Return the error. Note that calling this function affects
    /// the future behavior of the default resolver only: 'ipAddress' will be
    /// subsequently returned from calling 'getIpAddress()' of 'domainName',
    /// and 'domainName' will be returned from calling 'getDomainName()' of any
    /// of the addresses in 'ipAddressList', but this function does not affect
    /// any name resolution caches maintained elsewhere by the system. Also
    /// note that calling this function is synonymous with calling the
    /// equivalent function on the the default resolver.
    static ntsa::Error addIpAddress(const bslstl::StringRef& domainName,
                                    const ntsa::IpAddress&   ipAddress);

    /// Set the specified 'serviceName' to resolve to the specified 'portList'
    /// for use by the specified 'transport', and vice-versa. Return the error.
    /// Note that calling this function affects the future behavior of the
    /// default resolver only: 'port' will be subsequently returned from
    /// calling 'getPort()' of 'serviceName', and 'serviceName' will be
    /// returned from calling 'getServiceName()' of any of the ports in
    /// 'portList', but this function does not affect any name resolution
    /// caches maintained elsewhere by the system. Also note that calling this
    /// function is synonymous with calling the equivalent function on the
    /// the default resolver.
    static ntsa::Error setPort(const bslstl::StringRef&       serviceName,
                               const bsl::vector<ntsa::Port>& portList,
                               ntsa::Transport::Value         transport);

    /// Add the specified 'serviceName' to resolve to the specified 'portList'
    /// for use by the specified 'transport', and vice-versa, in addition to
    /// any previously, explicitly defined associations. Return the error. Note
    /// that calling this function affects the future behavior of the default
    /// resolver only: 'port' will be subsequently returned from calling
    /// 'getPort()' of 'serviceName', and 'serviceName' will be returned from
    /// calling 'getServiceName()' of any of the ports in 'portList', but this
    /// function does not affect any name resolution caches maintained
    /// elsewhere by the system. Also note that calling this function is
    /// synonymous with calling the equivalent function on the the default
    /// resolver.
    static ntsa::Error addPort(const bslstl::StringRef&       serviceName,
                               const bsl::vector<ntsa::Port>& portList,
                               ntsa::Transport::Value         transport);

    /// Add the specified 'serviceName' to resolve to the specified 'port' for
    /// use by the specified 'transport', and vice-versa, in addition to any
    /// previously, explicitly defined associations. Return the error. Note
    /// that calling this function affects the future behavior of the default
    /// resolver only: 'port' will be subsequently returned from calling
    /// 'getPort()' of 'serviceName', and 'serviceName' will be returned from
    /// calling 'getServiceName()' of any of the ports in 'portList', but this
    /// function does not affect any name resolution caches maintained
    /// elsewhere by the system. Also note that calling this function is
    /// synonymous with calling the equivalent function on the the default
    /// resolver.
    static ntsa::Error addPort(const bslstl::StringRef& serviceName,
                               ntsa::Port               port,
                               ntsa::Transport::Value   transport);

    /// Set the local IP addresses assigned to the local machine to the
    /// specified 'ipAddressList'. Return the error. Note that calling this
    /// function affects the future behavior of the default resolver only:
    /// 'ipAddressList' will be subsequently returned from calling
    /// 'getLocalIpAddress()' but this function does not set the local IP
    /// addresses of the system or have any wider effect on other objects or
    /// name resolution functionality in this process. Also note that calling
    /// this function is synonymous with calling the equivalent function on the
    /// the default resolver.
    static ntsa::Error setLocalIpAddress(
        const bsl::vector<ntsa::IpAddress>& ipAddressList);

    /// Set the hostname of the local machine to the specified 'name'. Return
    /// the error. Note that calling this function affects the future behavior
    /// of the default resolver only: 'name' will be subsequently returned from
    /// calling 'getHostname()' but this function does not set the hostname of
    /// the system or have any wider effect on other objects or name resolution
    /// functionality in this process. Also note that calling this function is
    /// synonymous with calling the equivalent function on the the default
    /// resolver.
    static ntsa::Error setHostname(const bsl::string& name);

    /// Set the canonical, fully-qualified hostname of the local machine to the
    /// specified 'name'. Return the error. Note that calling this function
    /// affects the future behavior of the default resolver only: 'name' will
    /// be subsequently returned from calling 'getHostnameFullyQualified()' but
    /// this function does not set the hostname of the system or have any wider
    /// effect on other objects or name resolution functionality in this
    /// process. Also note that calling this function is synonymous with
    /// calling the equivalent function on the the default resolver.
    static ntsa::Error setHostnameFullyQualified(const bsl::string& name);

    /// Load into the specified 'result' the IP addresses assigned to the
    /// specified 'domainName'. Perform all resolution and validation of the
    /// characteristics of the desired 'result' according to the specified
    /// 'options'. Return the error. Note that calling this function is
    /// synonymous with calling the equivalent function on the the default
    /// resolver.
    static ntsa::Error getIpAddress(bsl::vector<ntsa::IpAddress>* result,
                                    const bslstl::StringRef&      domainName,
                                    const ntsa::IpAddressOptions& options);

    /// Load into the specified 'result' the domain name to which the specified
    /// 'ipAddress' is assigned. Return the error. Note that calling this
    /// function is synonymous with calling the equivalent function on the the
    /// default resolver.
    static ntsa::Error getDomainName(bsl::string*           result,
                                     const ntsa::IpAddress& ipAddress);

    /// Load into the specified 'result' the port numbers assigned to the the
    /// specified 'serviceName'. Perform all resolution and validation of the
    /// characteristics of the desired 'result' according to the specified
    /// 'options'. Return the error. Note that calling this function is
    /// synonymous with calling the equivalent function on the the default
    /// resolver.
    static ntsa::Error getPort(bsl::vector<ntsa::Port>* result,
                               const bslstl::StringRef& serviceName,
                               const ntsa::PortOptions& options);

    /// Load into the specified 'result' the service name to which the
    /// specified 'port' is assigned for use by the specified 'transport'.
    /// Return the error. Note that calling this function is synonymous with
    /// calling the equivalent function on the the default resolver.
    static ntsa::Error getServiceName(bsl::string*           result,
                                      ntsa::Port             port,
                                      ntsa::Transport::Value transport);

    /// Load into the specified 'result' the endpoint parsed and potentially
    /// resolved from the components of the specified 'text', in the format of
    /// '<port>' or '[<host>][:<port>]'. If the optionally specified '<host>'
    /// component is not an IP address, interpret the '<host>' as a domain name
    /// and resolve it into an IP address. If the optionally specified '<port>'
    /// is a name and not a number, interpret the '<port>' as a service name
    /// and resolve it into a port. Perform all resolution and validation of
    /// the characteristics of the desired 'result' according to the specified
    /// 'options'. Return the error. Note that calling this function is
    /// synonymous with calling the equivalent function on the the default
    /// resolver.
    static ntsa::Error getEndpoint(ntsa::Endpoint*              result,
                                   const bslstl::StringRef&     text,
                                   const ntsa::EndpointOptions& options);

    /// Load into the specified 'result' the IP addresses assigned to the local
    /// machine. Perform all resolution and validation of the characteristics
    /// of the desired 'result' according to the specified 'options'. Return
    /// the error. Note that calling this function is synonymous with calling
    /// the equivalent function on the the default resolver.
    static ntsa::Error getLocalIpAddress(
        bsl::vector<ntsa::IpAddress>* result,
        const ntsa::IpAddressOptions& options);

    /// Return the hostname of the local machine. Note that calling this
    /// function is synonymous with calling the equivalent function on the the
    /// default resolver.
    static ntsa::Error getHostname(bsl::string* result);

    /// Return the canonical, fully-qualified hostname of the local machine.
    /// Note that calling this function is synonymous with calling the
    /// equivalent function on the the default resolver.
    static ntsa::Error getHostnameFullyQualified(bsl::string* result);

    /// Load into the specified 'result' the list of all the network
    /// adapters of the local machine. Note that this function loads
    /// descriptions of all network adapters currently available on the
    /// system, which may be expensive; prefer to cache the results of this
    /// function when possible.
    static void discoverAdapterList(bsl::vector<ntsa::Adapter>* result);

    /// Load into the specified 'adapter' the first adapter found assigned
    /// an IP address of the specified 'addressType'. Require that the
    /// resulting adapter support multicast according to the specified
    /// 'multicast' flag. Return true if such an adapter is found, and false
    /// otherwise. Note that this function loads descriptions of all network
    /// adapters currently available on the system, which may be expensive;
    /// prefer to cache the results of this function when possible.
    static bool discoverAdapter(ntsa::Adapter*             result,
                                ntsa::IpAddressType::Value addressType,
                                bool                       multicast);

    /// Load into the specified 'bufferArray' references to the leading
    /// non-data buffers contained in the specified 'data' blob that are
    /// intended for reception by the specified 'socket' according to the
    /// specified 'options', and load into the specified 'numBytes' the
    /// total number of bytes referenced by all buffers in the
    /// 'bufferArray'. Return the error. Note that 'bufferArray' must be
    /// empty before calling this function.
    static ntsa::Error scatter(ntsa::MutableBufferArray*   bufferArray,
                               bsl::size_t*                numBytes,
                               bdlbb::Blob*                data,
                               ntsa::Handle                socket,
                               const ntsa::ReceiveOptions& options);

    /// Load into the specified 'bufferArray' references to the leading
    /// data buffers contained in the specified 'data' blob that are
    /// intended for transmission by the specified 'socket' according to the
    /// specified 'options', and load into the specified 'numBytes' the
    /// total number of bytes referenced by all buffers in the
    /// 'bufferArray'. Return the error.  Note that 'bufferArray' must be
    /// empty before calling this function.
    static ntsa::Error gather(ntsa::ConstBufferArray*  bufferArray,
                              bsl::size_t*             numBytes,
                              const bdlbb::Blob&       data,
                              ntsa::Handle             socket,
                              const ntsa::SendOptions& options);

    /// Load into the specified 'result' the information of all sockets in
    /// the operating system. Filter the sockets reported according to the
    /// specified 'filter'. Return the error.
    static ntsa::Error reportInfo(bsl::vector<ntsa::SocketInfo>* result,
                                  const ntsa::SocketInfoFilter&  filter);

    // Install the specified 'resolver' as the default resolver.
    static void setDefault(const bsl::shared_ptr<ntsi::Resolver>& resolver);

    // Load into the specified 'result' the default resolver. If no default
    // resolver is explicitly installed, automatically install a default
    // resolver as if by internally calling 'ntsf::System::createResolver()'
    // with a default configuration followed by 'ntsf::System::setDefault()'.
    static void getDefault(bsl::shared_ptr<ntsi::Resolver>* result);

    /// Return true if the current machine has any adapter assigned an
    /// IPv4 address, otherwise return false. Note that this function loads
    /// descriptions of all network adapters currently available on the
    /// system, which may be expensive; prefer to cache the results of this
    /// function when possible.
    static bool supportsIpv4();

    /// Return true if the current machine has any adapter assigned an
    /// IPv4 address that supports multicast, otherwise return false. Note
    /// that this function loads descriptions of all network adapters
    /// currently available on the system, which may be expensive; prefer to
    ///  cache the results of this function when possible.
    static bool supportsIpv4Multicast();

    /// Return true if the current machine has any adapter assigned an
    /// IPv6 address, otherwise return false. Note that this function loads
    /// descriptions of all network adapters currently available on the
    /// system, which may be expensive; prefer to cache the results of this
    /// function when possible.
    static bool supportsIpv6();

    /// Return true if the current machine has any adapter assigned an
    /// IPv6 address that supports multicast, otherwise return false. Note
    /// that this function loads descriptions of all network adapters
    /// currently available on the system, which may be expensive; prefer to
    /// cache the results of this function when possible.
    static bool supportsIpv6Multicast();

    /// Return true if the current machine has any adapter assigned to
    /// either an IPv4 or IPv6 address that supports the Transport Control
    /// Protocol (TCP), otherwise return false. Note that this function
    /// loads descriptions of all network adapters currently available on
    /// the system, which may be expensive; prefer to cache the results of
    /// this function when possible.
    static bool supportsTcp();

    /// Return true if the current machine has any adapter assigned to
    /// either an IPv4 or IPv6 address that supports the User Datagram
    /// Protocol (UDP), otherwise return false. Note that this function
    /// loads descriptions of all network adapters currently available on
    /// the system, which may be expensive; prefer to cache the results of
    /// this function when possible.
    static bool supportsUdp();

    /// Return true if the current machine supports local (a.k.a Unix)
    /// domain stream sockets, otherwise return false.
    static bool supportsLocalStream();

    /// Return true if the current machine supports local (a.k.a Unix)
    /// domain datagram sockets, otherwise return false.
    static bool supportsLocalDatagram();

    /// Return true if the current machine supports the specified
    /// 'transport', otherwise return false. Note that this function loads
    /// descriptions of all network adapters currently available on the
    /// system, which may be expensive; prefer to cache the results of this
    /// function when possible.
    static bool supportsTransport(ntsa::Transport::Value transport);

    /// Load into the specified `result` names of tcp congestion control
    /// algorithms available for the process. Return the error.
    static ntsa::Error loadTcpCongestionControlAlgorithmSupport(
        bsl::vector<bsl::string>* result);

    /// Return true if the specified 'algorithmName' is supported on the
    /// current platform, otherwise return false.
    static bool testTcpCongestionControlAlgorithmSupport(
        const bsl::string& algorithmName);

    /// Release the resources necessary for this library's implementation.
    static void exit();
};

/// Provide a guard to automatically initialize the resources required by this
/// library upon construction and release all resources used by this library
/// upon destruction.
class SystemGuard
{
  private:
                 SystemGuard(const SystemGuard&) BSLS_KEYWORD_DELETED;
    SystemGuard& operator=(const SystemGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Initialize the current process to meet the requirements of this
    /// library and acquire the resources necessary for its implementation.
    /// The behavior is undefined if any resource required by this library
    /// cannot be acquired.  Users are encouraged to create an instance of
    /// this system guard in 'main' before any other networking functions
    /// are called.
    SystemGuard();

    /// Initialize the current process to meet the requirements of this
    /// library and acquire the resources necessary for its implementation.
    /// Ignore the specified 'signal' in this process.  The behavior is
    /// undefined if any resource required by this library cannot be
    /// acquired.  Users are encouraged to create an instance of this system
    /// guard in 'main' before any other networking functions are called.
    explicit SystemGuard(ntscfg::Signal::Value signal);

    /// Release the resources necessary for this library's implementation.
    ~SystemGuard();
};

/// Provide a guard to automatically close a socket unless otherwise released.
class HandleGuard
{
    ntsa::Handle d_handle;

  private:
                 HandleGuard(const HandleGuard&) BSLS_KEYWORD_DELETED;
    HandleGuard& operator=(const HandleGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Guard the specified 'handle'.
    explicit HandleGuard(ntsa::Handle handle);

    /// Close the guarded socket, if any, and destroy this object.
    ~HandleGuard();

    /// Stop guarding the underyling socket, and return the guarded socket.
    ntsa::Handle release();
};

}  // close package namespace
}  // close enterprise namespace
#endif
