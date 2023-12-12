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

#ifndef INCLUDED_NTSB_LISTENERSOCKET
#define INCLUDED_NTSB_LISTENERSOCKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_shutdowntype.h>
#include <ntsa_transport.h>
#include <ntsb_streamsocket.h>
#include <ntscfg_platform.h>
#include <ntsi_listenersocket.h>
#include <ntsi_streamsocket.h>
#include <ntsscm_version.h>
#include <bslma_managedptr.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntsb {

/// @internal @brief
/// Provide a blocking or non-blocking listener socket implemented by the
/// system.
///
/// @par Thread Safey
/// This class is thread safe.
///
/// @ingroup module_ntsb
class ListenerSocket : public ntsi::ListenerSocket
{
    ntsa::Handle d_handle;

  private:
    ListenerSocket(const ListenerSocket&) BSLS_KEYWORD_DELETED;
    ListenerSocket& operator=(const ListenerSocket&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new, uninitialized stream socket.
    ListenerSocket();

    /// Create a new stream socket implemented using the specified
    /// 'handle'.
    explicit ListenerSocket(ntsa::Handle handle);

    /// Destroy this object.
    ~ListenerSocket() BSLS_KEYWORD_OVERRIDE;

    /// Create a new socket of the specified 'transport'. Return the
    /// error.
    ntsa::Error open(ntsa::Transport::Value transport) BSLS_KEYWORD_OVERRIDE;

    /// Acquire ownership of the specified 'handle' to implement this
    /// socket. Return the error.
    ntsa::Error acquire(ntsa::Handle handle) BSLS_KEYWORD_OVERRIDE;

    /// Release ownership of the handle that implements this socket.
    ntsa::Handle release() BSLS_KEYWORD_OVERRIDE;

    /// Bind this socket to the specified source 'endpoint'. If the
    /// specified 'reuseAddress' flag is set, allow this socket to bind to
    /// an address already in use by the operating system. Return the error.
    ntsa::Error bind(const ntsa::Endpoint& endpoint,
                     bool                  reuseAddress) BSLS_KEYWORD_OVERRIDE;

    /// Bind this to any suitable source endpoint appropriate for a socket
    /// of the specified 'transport'. If the specified 'reuseAddress' flag
    /// is set, allow this socket to bind to an address already in use by
    /// the operating system. Return the error.
    ntsa::Error bindAny(ntsa::Transport::Value transport,
                        bool reuseAddress) BSLS_KEYWORD_OVERRIDE;

    /// Listen for connections made to this socket's source endpoint. Return
    /// the error.
    ntsa::Error listen(bsl::size_t backlog) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a handle to a connection to this
    /// socket's source endpoint. Return the error.
    ntsa::Error accept(ntsa::Handle* result) BSLS_KEYWORD_OVERRIDE;

    /// Acquire by the specified 'result' a handle to a connection to this
    /// socket's source endpoint. Return the error.
    ntsa::Error accept(ntsb::StreamSocket* result);

    /// Load into the specified 'result' a handle to a connection to this
    /// socket's source endpoint. Return the error. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    ntsa::Error accept(bslma::ManagedPtr<ntsi::StreamSocket>* result,
                       bslma::Allocator* basicAllocator = 0)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a handle to a connection to this
    /// socket's source endpoint. Return the error. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    ntsa::Error accept(bsl::shared_ptr<ntsi::StreamSocket>* result,
                       bslma::Allocator*                    basicAllocator = 0)
        BSLS_KEYWORD_OVERRIDE;

    /// Read data from the socket error queue. Then if the specified
    /// 'notifications' is not null parse fetched data to extract control
    /// messages into the specified 'notifications'. Return the error.
    ntsa::Error receiveNotifications(ntsa::NotificationQueue* notifications)
        BSLS_KEYWORD_OVERRIDE;

    /// Shutdown the stream socket in the specified 'direction'. Return the
    /// error.
    ntsa::Error shutdown(ntsa::ShutdownType::Value direction)
        BSLS_KEYWORD_OVERRIDE;

    /// Unlink the file corresponding to the socket, if the socket is a
    /// local (a.k.a. Unix domain) socket bound to a non-abstract path.
    /// Return the error.
    ntsa::Error unlink() BSLS_KEYWORD_OVERRIDE;

    /// Close the socket. Return the error.
    ntsa::Error close() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the source endpoint of this socket.
    /// Return the error.
    ntsa::Error sourceEndpoint(ntsa::Endpoint* result) const
        BSLS_KEYWORD_OVERRIDE;

    /// Return the descriptor handle.
    ntsa::Handle handle() const BSLS_KEYWORD_OVERRIDE;

    // *** Socket Options ***

    /// Set the option for the 'socket' that controls its blocking mode
    /// according to the specified 'blocking' flag. Return the error.
    ntsa::Error setBlocking(bool blocking) BSLS_KEYWORD_OVERRIDE;

    /// Set the specified 'option' for this socket. Return the error.
    ntsa::Error setOption(const ntsa::SocketOption& option)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'option' the socket option of the specified
    /// 'type' set for this socket. Return the error.
    ntsa::Error getOption(ntsa::SocketOption*           option,
                          ntsa::SocketOptionType::Value type)
        BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace
#endif
