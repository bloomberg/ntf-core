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

#ifndef INCLUDED_NTCD_LISTENERSOCKET
#define INCLUDED_NTCD_LISTENERSOCKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcd_machine.h>
#include <ntci_listenersocketfactory.h>
#include <ntcscm_version.h>
#include <ntsi_listenersocket.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcd {

/// @internal @brief
/// Provide a simulated, non-blocking listener socket for testing.
///
/// @details
/// Provide an implementation of the 'ntsi::ListenerSocket'
/// interface for testing.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class ListenerSocket : public ntsi::ListenerSocket
{
    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex                  d_mutex;
    bsl::shared_ptr<ntcd::Machine> d_machine_sp;
    bsl::shared_ptr<ntcd::Session> d_session_sp;
    bslma::Allocator*              d_allocator_p;

  private:
    ListenerSocket(const ListenerSocket&) BSLS_KEYWORD_DELETED;
    ListenerSocket& operator=(const ListenerSocket&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new listener socket implemented using sessions on the
    /// default machine. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit ListenerSocket(bslma::Allocator* basicAllocator = 0);

    /// Create a new listener socket implemented using sessions on the
    /// specified 'machine'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit ListenerSocket(const bsl::shared_ptr<ntcd::Machine>& machine,
                            bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ListenerSocket();

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

/// @internal @brief
/// Provide a factory of simulated, asynchronous listener sockets for testing.
///
/// @details
/// Provide an implementation of the 'ntci::ListenerSocketFactory'
/// interface to create asynchronous listener sockets through a proxy function.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class ListenerSocketFactory : public ntci::ListenerSocketFactory
{
  public:
    /// Defines a type alias for a function to create a new listener socket
    /// with the specified 'options'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    typedef bsl::function<bsl::shared_ptr<ntci::ListenerSocket>(
        const ntca::ListenerSocketOptions& options,
        bslma::Allocator*                  basicAllocator)>
        Function;

  private:
    Function          d_function;
    bslma::Allocator* d_allocator_p;

  private:
    ListenerSocketFactory(const ListenerSocketFactory&) BSLS_KEYWORD_DELETED;
    ListenerSocketFactory& operator=(const ListenerSocketFactory&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new listener socket factory that creates listener sockets
    /// using the specified 'function'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit ListenerSocketFactory(const Function&   function,
                                   bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ListenerSocketFactory();

    /// Create a new listener socket with the specified 'options'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::ListenerSocket> createListenerSocket(
        const ntca::ListenerSocketOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace
#endif
