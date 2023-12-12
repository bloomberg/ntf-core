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

#ifndef INCLUDED_NTSI_LISTENERSOCKET
#define INCLUDED_NTSI_LISTENERSOCKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_notificationqueue.h>
#include <ntsa_shutdowntype.h>
#include <ntsa_socketoption.h>
#include <ntsa_transport.h>
#include <ntscfg_platform.h>
#include <ntsi_descriptor.h>
#include <ntsi_streamsocket.h>
#include <ntsscm_version.h>
#include <bslma_managedptr.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntsi {

/// Provide a blocking or non-blocking listener socket.
///
/// @details
/// Provide an abstract mechanism to listen for and accept
/// connections initiated by stream sockets. The public member functions of
/// this class roughly correspond to the stream-type socket in listening mode
/// functionality of the Berkeley Sockets API in the POSIX specification.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @par Usage Example: Listen and Accept Connections
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
///     error = listener->bind(
///         ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0), false);
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
///
/// @ingroup module_ntsi
class ListenerSocket : public ntsi::Descriptor
{
  public:
    /// Destroy this object.
    ~ListenerSocket() BSLS_KEYWORD_OVERRIDE;

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

    /// Listen for connections made to this socket's source endpoint. Return
    /// the error.
    virtual ntsa::Error listen(bsl::size_t backlog);

    /// Load into the specified 'result' a handle to a connection to this
    /// socket's source endpoint. Return the error.
    virtual ntsa::Error accept(ntsa::Handle* result);

    /// Load into the specified 'result' a handle to a connection to this
    /// socket's source endpoint. Return the error. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    virtual ntsa::Error accept(bslma::ManagedPtr<ntsi::StreamSocket>* result,
                               bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' a handle to a connection to this
    /// socket's source endpoint. Return the error. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    virtual ntsa::Error accept(bsl::shared_ptr<ntsi::StreamSocket>* result,
                               bslma::Allocator* basicAllocator = 0);

    /// Read data from the socket error queue. Then if the specified
    /// 'notifications' is not null parse fetched data to extract control
    /// messages into the specified 'notifications'. Return the error.
    virtual ntsa::Error receiveNotifications(
        ntsa::NotificationQueue* notifications);

    /// Shutdown the stream socket in the specified 'direction'. Return the
    /// error.
    virtual ntsa::Error shutdown(ntsa::ShutdownType::Value direction);

    /// Unlink the file corresponding to the socket, if the socket is a
    /// local (a.k.a. Unix domain) socket bound to a non-abstract path.
    /// Return the error.
    virtual ntsa::Error unlink();

    /// Close the socket. Return the error.
    virtual ntsa::Error close();

    /// Load into the specified 'result' the source endpoint of this socket.
    /// Return the error.
    virtual ntsa::Error sourceEndpoint(ntsa::Endpoint* result) const;

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
};

}  // close package namespace
}  // close enterprise namespace
#endif
