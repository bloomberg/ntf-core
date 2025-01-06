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

#ifndef INCLUDED_NTCI_LISTENERSOCKET
#define INCLUDED_NTCI_LISTENERSOCKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_acceptoptions.h>
#include <ntca_accepttoken.h>
#include <ntca_flowcontrolmode.h>
#include <ntca_flowcontroltype.h>
#include <ntca_listenersocketevent.h>
#include <ntccfg_platform.h>
#include <ntci_acceptcallback.h>
#include <ntci_acceptcallbackfactory.h>
#include <ntci_acceptor.h>
#include <ntci_bindable.h>
#include <ntci_closable.h>
#include <ntci_closecallback.h>
#include <ntci_closecallbackfactory.h>
#include <ntci_datapool.h>
#include <ntci_executor.h>
#include <ntci_ratelimiter.h>
#include <ntci_resolver.h>
#include <ntci_strand.h>
#include <ntci_strandfactory.h>
#include <ntci_timer.h>
#include <ntci_timerfactory.h>
#include <ntcscm_version.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsi_descriptor.h>
#include <ntsi_listenersocket.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadutil.h>
#include <bsl_functional.h>
#include <bsl_memory.h>

namespace BloombergLP {

namespace ntci {
class ListenerSocketSession;
}
namespace ntci {
class ListenerSocketManager;
}
namespace ntci {
class StreamSocket;
}
namespace ntci {

/// Provide an interface to an asynchronous listener socket.
///
/// @details
/// Listener sockets accept connection requests made to the address to which
/// the listener socket is bound, resulting in 'ntci::StreamSocket' objects.
///
/// A listener socket may listen for connection requests made by peers using
/// the Transmission Control Protocol (TCP) transport over an Internet Protocol
/// (IP) network, or, on Unix-like platforms, using a local (a.k.a. Unix)
/// domain protocol.
///
/// For a comprehensive explanation of networking address families, protocols,
/// sockets, and network programming in general, see the definitive reference
/// "UNIX Network Programming, Volume 1: The Sockets Networking API", by W.
/// Richard Stevens.
///
/// @par Closing
/// Each 'ntci::ListenerSocket' is shared between the user and this library's
/// asynchronous machinery. It is not sufficient for users to simply release
/// their reference counts on a listener socket object to close and destroy it.
/// Users *must* explicitly close each 'ntci::ListenerSocket'. Closing a socket
/// is asynchronous, users must wait until the close callback is invoked before
/// assuming the socket is completely closed. After a socket's close callback
/// is invoked, the socket remains in a valid state but all member functions
/// with failure modes will return an error. The socket object will be
/// destroyed only after it has been closed and all references are released.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_socket
class ListenerSocket : public ntsi::Descriptor,
                       public ntci::Executor,
                       public ntci::StrandFactory,
                       public ntci::TimerFactory,
                       public ntci::Closable,
                       public ntci::Bindable,
                       public ntci::Acceptor,
                       public ntci::DataPool
{
  public:
    /// Define a type alias for a callback function invoked
    /// when an event occurs on a stream socket.
    typedef NTCCFG_FUNCTION(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ListenerSocketEvent&             event) SessionCallback;

    /// Destroy this object.
    virtual ~ListenerSocket();

    /// Open the listener socket. Return the error.
    virtual ntsa::Error open() = 0;

    /// Open the listener socket using the specified 'transport'. Return the
    /// error.
    virtual ntsa::Error open(ntsa::Transport::Value transport) = 0;

    /// Open the listener socket using the specified 'transport' imported
    /// from the specified 'handle'. Return the error.
    virtual ntsa::Error open(ntsa::Transport::Value transport,
                             ntsa::Handle           handle) = 0;

    /// Open the stream socket using the specified 'transport' imported from
    /// the specified 'listenerSocket'. Return the error.
    virtual ntsa::Error open(
        ntsa::Transport::Value                       transport,
        const bsl::shared_ptr<ntsi::ListenerSocket>& listenerSocket) = 0;

    /// Bind to the specified source 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on the callback's strand,
    /// if any, when the socket has been bound or any error occurs. Return
    /// the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created.
    virtual ntsa::Error bind(const ntsa::Endpoint&     endpoint,
                             const ntca::BindOptions&  options,
                             const ntci::BindFunction& callback) = 0;

    /// Bind to the specified source 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on the callback's strand,
    /// if any, when the socket has been bound or any error occurs. Return
    /// the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created.
    virtual ntsa::Error bind(const ntsa::Endpoint&     endpoint,
                             const ntca::BindOptions&  options,
                             const ntci::BindCallback& callback) = 0;

    /// Bind to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error bind(const bsl::string&        name,
                             const ntca::BindOptions&  options,
                             const ntci::BindFunction& callback) = 0;

    /// Bind to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error bind(const bsl::string&        name,
                             const ntca::BindOptions&  options,
                             const ntci::BindCallback& callback) = 0;

    /// Listen for connections. Return the error.
    virtual ntsa::Error listen() = 0;

    /// Listen for connections using the specified 'backlog'. Return the
    /// error.
    virtual ntsa::Error listen(bsl::size_t backlog) = 0;

    /// Dequeue a connection from the backlog according to the specified
    /// 'options'. If the accept queue is not empty, synchronously pop the
    /// front of the accept queue into the specified 'streamSocket'.
    /// Otherwise, asynchronously accept a connection from the backlog onto
    /// the accept queue as a connection in the backlog becomes accepted, at
    /// the configured accept rate limit, if any, up to the accept queue
    /// high watermark. After satisfying any queued accept operations, when
    /// the accept queue is asynchronously filled up to the accept queue low
    /// watermark, announce an accept queue low watermark event. When
    /// asynchronously enqueuing connections onto the accept queue causes the
    /// accept queue high watermark to become breached, stop asynchronously
    /// accepting connections from the backlog onto the accept queue and
    /// announce an accept queue high watermark event. Return the error,
    /// notably 'ntsa::Error::e_WOULD_BLOCK' if neither the accept queue nor
    /// the backlog is non-empty. All other errors indicate no more
    /// connections have been accepted at this time or will become accepted
    /// in the future.
    virtual ntsa::Error accept(
        ntca::AcceptContext*                 context,
        bsl::shared_ptr<ntci::StreamSocket>* streamSocket,
        const ntca::AcceptOptions&           options) = 0;

    /// Dequeue a connection from the backlog according to the specified
    /// 'options'. If the accept queue is not empty, synchronously pop the
    /// front of the accept queue into an internally allocated stream socket
    /// and invoke the specified 'callback' on the callback's strand, if
    /// any, with that stream socket. Otherwise, queue the accept operation
    /// and asynchronously accept connections from the backlog onto the
    /// accept queue as connections in the backlog become accepted, at the
    /// configured accept rate limit, if any, up to the accept queue high
    /// watermark. When the accept queue becomes non-empty, synchronously
    /// pop the front of the accept queue into an internally allocated
    /// stream socket and invoke the 'callback' on the callbacks strand, if
    /// any, with that stream socket. After satisfying any queued accept
    /// operations, when the accept queue is asynchronously filled up to the
    /// accept queue low watermark, announce an accept queue low watermark
    /// event. When asynchronously enqueuing connections onto the accept
    /// queue causes the accept queue high watermark to become breached,
    /// stop asynchronously accepting connections from the backlog onto the
    /// accept queue and announce an accept queue high watermark event.
    /// Return the error, notably 'ntsa::Error::e_WOULD_BLOCK' if neither
    /// the accept queue nor the backlog is non-empty. All other errors
    /// indicate no more connections have been accepted at this time or will
    /// become accepted in the future. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error accept(const ntca::AcceptOptions&  options,
                               const ntci::AcceptFunction& callback) = 0;

    /// Dequeue a connection from the backlog according to the specified
    /// 'options'. If the accept queue is not empty, synchronously pop the
    /// front of the accept queue into an internally allocated stream socket
    /// and invoke the specified 'callback' on the callback's strand, if
    /// any, with that stream socket. Otherwise, queue the accept operation
    /// and asynchronously accept connections from the backlog onto the
    /// accept queue as connections in the backlog become accepted, at the
    /// configured accept rate limit, if any, up to the accept queue high
    /// watermark. When the accept queue becomes non-empty, synchronously
    /// pop the front of the accept queue into an internally allocated
    /// stream socket and invoke the 'callback' on the callbacks strand, if
    /// any, with that stream socket. After satisfying any queued accept
    /// operations, when the accept queue is asynchronously filled up to the
    /// accept queue low watermark, announce an accept queue low watermark
    /// event. When asynchronously enqueuing connections onto the accept
    /// queue causes the accept queue high watermark to become breached,
    /// stop asynchronously accepting connections from the backlog onto the
    /// accept queue and announce an accept queue high watermark event.
    /// Return the error, notably 'ntsa::Error::e_WOULD_BLOCK' if neither
    /// the accept queue nor the backlog is non-empty. All other errors
    /// indicate no more connections have been accepted at this time or will
    /// become accepted in the future. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error accept(const ntca::AcceptOptions&  options,
                               const ntci::AcceptCallback& callback) = 0;

    /// Register the specified 'resolver' for this socket. Return the error.
    virtual ntsa::Error registerResolver(
        const bsl::shared_ptr<ntci::Resolver>& resolver) = 0;

    /// Deregister the current resolver for this socket. Return the error.
    virtual ntsa::Error deregisterResolver() = 0;

    /// Register the specified 'manager' for this socket. Return the error.
    virtual ntsa::Error registerManager(
        const bsl::shared_ptr<ntci::ListenerSocketManager>& manager) = 0;

    /// Deregister the current manager or manager callback for this socket.
    /// Return the error.
    virtual ntsa::Error deregisterManager() = 0;

    /// Register the specified 'session' for this socket. Return the
    /// error.
    virtual ntsa::Error registerSession(
        const bsl::shared_ptr<ntci::ListenerSocketSession>& session) = 0;

    /// Register the specified session 'callback' for this socket to be
    /// invoked on this socket's strand. Return the error.
    virtual ntsa::Error registerSessionCallback(
        const ntci::ListenerSocket::SessionCallback& callback) = 0;

    /// Register the specified session 'callback' for this socket to be
    /// invoked on the specified 'strand'. Return the error.
    virtual ntsa::Error registerSessionCallback(
        const ntci::ListenerSocket::SessionCallback& callback,
        const bsl::shared_ptr<ntci::Strand>&         strand) = 0;

    /// Deregister the current session or session callback for this socket.
    /// Return the error.
    virtual ntsa::Error deregisterSession() = 0;

    /// Set the accept rate limiter to the specified 'rateLimiter'. Return
    /// the error.
    virtual ntsa::Error setAcceptRateLimiter(
        const bsl::shared_ptr<ntci::RateLimiter>& rateLimiter) = 0;

    /// Set the accept queue low watermark to the specified 'lowWatermark'.
    /// Return the error.
    virtual ntsa::Error setAcceptQueueLowWatermark(
        bsl::size_t lowWatermark) = 0;

    /// Set the accept queue high watermark to the specified 'highWatermark'.
    /// Return the error.
    virtual ntsa::Error setAcceptQueueHighWatermark(
        bsl::size_t highWatermark) = 0;

    /// Set the accept queue limits to the specified 'lowWatermark' and
    /// 'highWatermark'. Return the error.
    virtual ntsa::Error setAcceptQueueWatermarks(
        bsl::size_t lowWatermark,
        bsl::size_t highWatermark) = 0;

    /// Enable copying from the socket buffers in the specified 'direction'.
    virtual ntsa::Error relaxFlowControl(
        ntca::FlowControlType::Value direction) = 0;

    /// Disable copying from socket buffers in the specified 'direction'
    /// according to the specified 'mode'.
    virtual ntsa::Error applyFlowControl(
        ntca::FlowControlType::Value direction,
        ntca::FlowControlMode::Value mode) = 0;

    /// Cancel the bind operation identified by the specified 'token'.
    /// Return the error.
    virtual ntsa::Error cancel(const ntca::BindToken& token) = 0;

    /// Cancel the accept operation identified by the specified 'token'.
    /// Return the error.
    virtual ntsa::Error cancel(const ntca::AcceptToken& token) = 0;

    /// Shutdown the listener socket. Return the error.
    virtual ntsa::Error shutdown() = 0;

    /// Close the listener socket.
    virtual void close() = 0;

    /// Close the listener socket and invoke the specified 'callback' on
    /// the callback's strand, if any, when the listener socket is closed.
    /// Note that callbacks created by this object will automatically be
    /// invoked on this object's strand unless an explicit strand is
    /// specified at the time the callback is created.
    virtual void close(const ntci::CloseFunction& callback) = 0;

    /// Close the listener socket and invoke the specified 'callback' on
    /// the callback's strand, if any, when the listener socket is closed.
    /// Note that callbacks created by this object will automatically be
    /// invoked on this object's strand unless an explicit strand is
    /// specified at the time the callback is created.
    virtual void close(const ntci::CloseCallback& callback) = 0;

    /// Return the handle to the descriptor.
    virtual ntsa::Handle handle() const = 0;

    /// Return the transport of the listener socket.
    virtual ntsa::Transport::Value transport() const = 0;

    /// Return the source address.
    virtual ntsa::Endpoint sourceEndpoint() const = 0;

    /// Return the strand that guarantees sequential, non-current execution
    /// of arbitrary functors on the unspecified threads processing events
    /// for this object.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const = 0;

    /// Return the handle of the thread that manages this socket, or
    /// the default value if no such thread has been set.
    virtual bslmt::ThreadUtil::Handle threadHandle() const = 0;

    /// Return the index in the thread pool of the thread that manages this
    /// socket, or 0 if no such thread has been set.
    virtual bsl::size_t threadIndex() const = 0;

    /// Return the current number of connections queued after being accepted
    /// from the backlog. These connections have been accepted by the
    /// operating system buffers but not yet accepted by the user.
    virtual bsl::size_t acceptQueueSize() const = 0;

    /// Return the current accept queue low watermark.
    virtual bsl::size_t acceptQueueLowWatermark() const = 0;

    /// Return the current accept queue high watermark.
    virtual bsl::size_t acceptQueueHighWatermark() const = 0;
};

/// Provide a guard to automatically close a listener socket
/// and block until the listener socket is asynchronously closed.
class ListenerSocketCloseGuard
{
    bsl::shared_ptr<ntci::ListenerSocket> d_listenerSocket_sp;

  private:
    ListenerSocketCloseGuard(const ListenerSocketCloseGuard&)
        BSLS_KEYWORD_DELETED;
    ListenerSocketCloseGuard& operator=(const ListenerSocketCloseGuard&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Post to the specified 'semaphore'.
    static void complete(bslmt::Semaphore* semaphore);

  public:
    /// Create a new listener socket guard for the specified
    /// 'listenerSocket', if any.
    explicit ListenerSocketCloseGuard(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket);

    /// Destroy this object: close the managed listener socket, if any, and
    /// block until the listener socket is asynchronously closed.
    ~ListenerSocketCloseGuard();

    /// Release the guard and return the guarded object.
    bsl::shared_ptr<ntci::ListenerSocket> release();
};

}  // close package namespace
}  // close enterprise namespace
#endif
