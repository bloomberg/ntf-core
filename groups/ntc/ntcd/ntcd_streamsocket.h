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

#ifndef INCLUDED_NTCD_STREAMSOCKET
#define INCLUDED_NTCD_STREAMSOCKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcd_machine.h>
#include <ntci_streamsocketfactory.h>
#include <ntcscm_version.h>
#include <ntsi_streamsocket.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <ntccfg_test.h>

namespace BloombergLP {
namespace ntcd {

/// @internal @brief
/// Provide a simulated, non-blocking stream socket for testing.
///
/// @details
/// Provide an implementation of the 'ntsi::StreamSocket'
/// interface for testing.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class StreamSocket : public ntsi::StreamSocket
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
    StreamSocket(const StreamSocket&) BSLS_KEYWORD_DELETED;
    StreamSocket& operator=(const StreamSocket&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new stream socket implemented using sessions on the
    /// default machine. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit StreamSocket(bslma::Allocator* basicAllocator = 0);

    /// Create a new stream socket implemented using sessions on the
    /// specified 'machine'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit StreamSocket(const bsl::shared_ptr<ntcd::Machine>& machine,
                          bslma::Allocator* basicAllocator = 0);

    /// Create a new stream socket implemented using the specified 'session'
    /// on the specified 'machine'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit StreamSocket(const bsl::shared_ptr<ntcd::Machine>& machine,
                          const bsl::shared_ptr<ntcd::Session>& session,
                          bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~StreamSocket();

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

    /// Connect to the specified remote 'endpoint'. Return the error.
    ntsa::Error connect(const ntsa::Endpoint& endpoint) BSLS_KEYWORD_OVERRIDE;

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
                     const ntsa::ConstBuffer* data,
                     bsl::size_t              size,
                     const ntsa::SendOptions& options) BSLS_KEYWORD_OVERRIDE;

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

    /// Shutdown the stream socket in the specified 'direction'. Return the
    /// error.
    ntsa::Error shutdown(ntsa::ShutdownType::Value direction)
        BSLS_KEYWORD_OVERRIDE;

    /// Unlink the file corresponding to the socket, if the socket is a
    /// local (a.k.a. Unix domain) socket bound to a non-abstract path.
    /// Return the error. Note that this function should only be called
    /// for sockets in the connect role.
    ntsa::Error unlink() BSLS_KEYWORD_OVERRIDE;

    /// Close the socket. Return the error.
    ntsa::Error close() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the source endpoint of this socket.
    /// Return the error.
    ntsa::Error sourceEndpoint(ntsa::Endpoint* result) const
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the remote endpoint to which this
    /// socket is connected. Return the error.
    ntsa::Error remoteEndpoint(ntsa::Endpoint* result) const
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

    /// Load into the specified 'result' the last known error encountered
    /// when connecting the socket. Return the error (retrieving the error).
    ntsa::Error getLastError(ntsa::Error* result) BSLS_KEYWORD_OVERRIDE;

    // *** Limits ***

    /// Return the maximum number of buffers that can be the source of a
    /// gathered write. Additional buffers beyond this limit are silently
    /// ignored.
    bsl::size_t maxBuffersPerSend() const BSLS_KEYWORD_OVERRIDE;

    /// Return the maximum number of buffers that can be the destination
    /// of a scattered read. Additional buffers beyond this limit are
    /// silently ignored.
    bsl::size_t maxBuffersPerReceive() const BSLS_KEYWORD_OVERRIDE;
};

/// @internal @brief
/// Provide a factory of simulated, asynchronous stream sockets for testing.
///
/// @details
/// Provide an implementation of the 'ntci::StreamSocketFactory'
/// interface to create asynchronous stream sockets through a proxy function.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class StreamSocketFactory : public ntci::StreamSocketFactory
{
  public:
    /// Defines a type alias for a function to create a new stream socket
    /// with the specified 'options'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    typedef bsl::function<bsl::shared_ptr<ntci::StreamSocket>(
        const ntca::StreamSocketOptions& options,
        bslma::Allocator*                basicAllocator)>
        Function;

  private:
    Function          d_function;
    bslma::Allocator* d_allocator_p;

  private:
    StreamSocketFactory(const StreamSocketFactory&) BSLS_KEYWORD_DELETED;
    StreamSocketFactory& operator=(const StreamSocketFactory&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new stream socket factory that creates stream sockets
    /// using the specified 'function'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit StreamSocketFactory(const Function&   function,
                                 bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~StreamSocketFactory();

    /// Create a new stream socket with the specified 'options'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::StreamSocket> createStreamSocket(
        const ntca::StreamSocketOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;
};

#if NTCCFG_TEST_MOCK_ENABLED

NTF_MOCK_CLASS(StreamSocketMock, ntsi::StreamSocket)

NTF_MOCK_METHOD_CONST(ntsa::Handle, handle)
NTF_MOCK_METHOD(ntsa::Error, open, ntsa::Transport::Value)
NTF_MOCK_METHOD(ntsa::Error, acquire, ntsa::Handle)
NTF_MOCK_METHOD(ntsa::Handle, release)

NTF_MOCK_METHOD(ntsa::Error, bind, const ntsa::Endpoint&, bool)
NTF_MOCK_METHOD(ntsa::Error, bindAny, ntsa::Transport::Value, bool)
NTF_MOCK_METHOD(ntsa::Error, connect, const ntsa::Endpoint&)

NTF_MOCK_METHOD(ntsa::Error,
                send,
                ntsa::SendContext*,
                const bdlbb::Blob&,
                const ntsa::SendOptions&)
NTF_MOCK_METHOD(ntsa::Error,
                send,
                ntsa::SendContext*,
                const ntsa::Data&,
                const ntsa::SendOptions&)
NTF_MOCK_METHOD(ntsa::Error,
                send,
                ntsa::SendContext*,
                const ntsa::ConstBuffer*,
                bsl::size_t,
                const ntsa::SendOptions&)

NTF_MOCK_METHOD(ntsa::Error,
                receive,
                ntsa::ReceiveContext*,
                bdlbb::Blob*,
                const ntsa::ReceiveOptions&)
NTF_MOCK_METHOD(ntsa::Error,
                receive,
                ntsa::ReceiveContext*,
                ntsa::Data*,
                const ntsa::ReceiveOptions&)

NTF_MOCK_METHOD(ntsa::Error, receiveNotifications, ntsa::NotificationQueue*)
NTF_MOCK_METHOD(ntsa::Error, shutdown, ntsa::ShutdownType::Value)
NTF_MOCK_METHOD(ntsa::Error, unlink)
NTF_MOCK_METHOD(ntsa::Error, close)
NTF_MOCK_METHOD_CONST(ntsa::Error, sourceEndpoint, ntsa::Endpoint*)
NTF_MOCK_METHOD_CONST(ntsa::Error, remoteEndpoint, ntsa::Endpoint*)
NTF_MOCK_METHOD(ntsa::Error, setBlocking, bool)
NTF_MOCK_METHOD(ntsa::Error, setOption, const ntsa::SocketOption&)

NTF_MOCK_METHOD(ntsa::Error,
                getOption,
                ntsa::SocketOption*,
                ntsa::SocketOptionType::Value)
NTF_MOCK_METHOD(ntsa::Error, getLastError, ntsa::Error*)
NTF_MOCK_METHOD_CONST(bsl::size_t, maxBuffersPerSend)
NTF_MOCK_METHOD_CONST(bsl::size_t, maxBuffersPerReceive)
NTF_MOCK_CLASS_END;

#endif

}  // close package namespace
}  // close enterprise namespace
#endif
