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

#ifndef INCLUDED_NTCR_LISTENERSOCKET
#define INCLUDED_NTCR_LISTENERSOCKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_listenersocketoptions.h>
#include <ntccfg_platform.h>
#include <ntci_datapool.h>
#include <ntci_listenersocket.h>
#include <ntci_listenersocketmanager.h>
#include <ntci_listenersocketsession.h>
#include <ntci_reactor.h>
#include <ntci_reactorpool.h>
#include <ntci_reactorsocket.h>
#include <ntci_resolver.h>
#include <ntci_strand.h>
#include <ntci_timer.h>
#include <ntcq_accept.h>
#include <ntcs_detachstate.h>
#include <ntcs_flowcontrolcontext.h>
#include <ntcs_flowcontrolstate.h>
#include <ntcs_metrics.h>
#include <ntcs_observer.h>
#include <ntcs_shutdowncontext.h>
#include <ntcs_shutdownstate.h>
#include <ntcscm_version.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_shutdownmode.h>
#include <ntsa_shutdowntype.h>
#include <ntsi_descriptor.h>
#include <bsls_atomic.h>
#include <bsl_list.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcr {

/// @internal @brief
/// Provide an asynchronous, reactively-driven listener socket.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcr
class ListenerSocket : public ntci::ListenerSocket,
                       public ntci::ReactorSocket,
                       public ntccfg::Shared<ListenerSocket>
{
    /// Define a type alias for a shared pointer to a blob
    /// buffer factory.
    typedef bsl::shared_ptr<bdlbb::BlobBufferFactory> BlobBufferFactoryPtr;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    ntccfg::Object                               d_object;
    mutable Mutex                                d_mutex;
    ntsa::Handle                                 d_systemHandle;
    ntsa::Handle                                 d_publicHandle;
    ntsa::Transport::Value                       d_transport;
    ntsa::Endpoint                               d_sourceEndpoint;
    bsl::shared_ptr<ntsi::ListenerSocket>        d_socket_sp;
    ntcs::Observer<ntci::Resolver>               d_resolver;
    ntcs::Observer<ntci::Reactor>                d_reactor;
    ntcs::Observer<ntci::ReactorPool>            d_reactorPool;
    bsl::shared_ptr<ntci::Strand>                d_reactorStrand_sp;
    bsl::shared_ptr<ntci::ListenerSocketManager> d_manager_sp;
    bsl::shared_ptr<ntci::Strand>                d_managerStrand_sp;
    bsl::shared_ptr<ntci::ListenerSocketSession> d_session_sp;
    bsl::shared_ptr<ntci::Strand>                d_sessionStrand_sp;
    bsl::shared_ptr<ntci::DataPool>              d_dataPool_sp;
    BlobBufferFactoryPtr                         d_incomingBufferFactory_sp;
    BlobBufferFactoryPtr                         d_outgoingBufferFactory_sp;
    bsl::shared_ptr<ntcs::Metrics>               d_metrics_sp;
    ntcs::FlowControlState                       d_flowControlState;
    ntcs::ShutdownState                          d_shutdownState;
    ntcq::AcceptQueue                            d_acceptQueue;
    bsl::shared_ptr<ntci::RateLimiter>           d_acceptRateLimiter_sp;
    bsl::shared_ptr<ntci::Timer>                 d_acceptRateTimer_sp;
    bsl::shared_ptr<ntci::Timer>                 d_acceptBackoffTimer_sp;
    bool                                         d_acceptGreedily;
    const bool                                   d_oneShot;
    ntca::ListenerSocketOptions                  d_options;
    ntcs::DetachState                            d_detachState;
    ntci::CloseCallback                          d_closeCallback;
    ntci::Executor::FunctorSequence              d_deferredCalls;
    bslma::Allocator*                            d_allocator_p;

  private:
    ListenerSocket(const ListenerSocket&) BSLS_KEYWORD_DELETED;
    ListenerSocket& operator=(const ListenerSocket&) BSLS_KEYWORD_DELETED;

  private:
    /// Process the readability of the descriptor.
    void processSocketReadable(const ntca::ReactorEvent& event)
        BSLS_KEYWORD_OVERRIDE;

    /// Process the writability of the descriptor.
    void processSocketWritable(const ntca::ReactorEvent& event)
        BSLS_KEYWORD_OVERRIDE;

    /// Process an descriptor that has occurred on the socket.
    void processSocketError(const ntca::ReactorEvent& event)
        BSLS_KEYWORD_OVERRIDE;

    /// Process the specified 'notifications' of the socket.
    void processNotifications(const ntsa::NotificationQueue& notifications)
        BSLS_KEYWORD_OVERRIDE;

    /// Attempt to dequeue from the backlog after the accept rate limiter
    /// estimates more connections might be able to be accepted.
    void processAcceptRateTimer(const bsl::shared_ptr<ntci::Timer>& timer,
                                const ntca::TimerEvent&             event);

    /// Attempt to dequeue from the backlog after the accept backoff
    /// timer estimates more connections might be able to be accepted.
    void processAcceptBackoffTimer(const bsl::shared_ptr<ntci::Timer>& timer,
                                   const ntca::TimerEvent&             event);

    /// Fail the specified 'entry' because no connected was dequeued from
    /// the backlog within the deadline.
    void processAcceptDeadlineTimer(
        const bsl::shared_ptr<ntci::Timer>&                    timer,
        const ntca::TimerEvent&                                event,
        const bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>& entry);

    /// Process the readability of the socket by performing one accept
    /// iteration.
    ntsa::Error privateSocketReadableIteration(
        const bsl::shared_ptr<ListenerSocket>& self);

    /// Indicate a failure has occurred and detach the socket from its
    /// monitor.
    void privateFail(const bsl::shared_ptr<ListenerSocket>& self,
                     const ntsa::Error&                     error);

    /// Indicate a failure has occurred and detach the socket from its
    /// monitor.
    void privateFail(const bsl::shared_ptr<ListenerSocket>& self,
                     const ntca::ErrorEvent&                event);

    /// Shutdown the stream socket in the specified 'direction' according
    /// to the specified 'mode' of shutdown. Return the error.
    ntsa::Error privateShutdown(const bsl::shared_ptr<ListenerSocket>& self,
                                ntsa::ShutdownType::Value direction,
                                ntsa::ShutdownMode::Value mode,
                                bool                      defer);

    /// Process the advancement through the shutdown sequence by trying to
    /// shut down the socket for sending.
    void privateShutdownSend(const bsl::shared_ptr<ListenerSocket>& self,
                             bool                                   defer);

    /// Process the advancement through the shutdown sequence by trying to
    /// shut down the socket for sending.
    void privateShutdownReceive(const bsl::shared_ptr<ListenerSocket>& self,
                                ntsa::ShutdownOrigin::Value            origin,
                                bool                                   defer);

    /// Perform the shutdown sequence: if the specified 'context' indicates,
    /// has been initiated, announce the initiation of the shutdown sequence
    /// from the specified 'origin'; if the 'context' indicates the socket
    /// should be shut down for sending, shut down the socket for sending
    /// and announce the corresponding event; if the 'context' indicates
    /// the socket should be shut down for receiving, shut down the socket
    /// for receiving and announce the corresponding event; if the 'context'
    /// indicates the shutdown sequence has completed, announce the
    /// completion of the shutdown sequence.
    /// If it is required to detach the socket from the proactor then part of
    /// described functionality will be executed asynchronously using the next
    /// method.
    void privateShutdownSequence(const bsl::shared_ptr<ListenerSocket>& self,
                                 ntsa::ShutdownOrigin::Value            origin,
                                 const ntcs::ShutdownContext& context,
                                 bool                         defer);

    /// Execute the second part of shutdown sequence when the socket is
    /// detached. See also "privateShutdownSequence".
    void privateShutdownSequenceComplete(
        const bsl::shared_ptr<ListenerSocket>& self,
        const ntcs::ShutdownContext&           context,
        bool                                   defer,
        bool                                   lock);

    /// Enable copying from the socket buffers in the specified 'direction'.
    /// The behavior is undefined unless 'd_mutex' is locked.
    ntsa::Error privateRelaxFlowControl(
        const bsl::shared_ptr<ListenerSocket>& self,
        ntca::FlowControlType::Value           direction,
        bool                                   defer,
        bool                                   unlock);

    /// Disable copying from socket buffers in the specified 'direction'
    /// according to the specified 'mode'. The behavior is undefined unless
    /// 'd_mutex' is locked.
    ntsa::Error privateApplyFlowControl(
        const bsl::shared_ptr<ListenerSocket>& self,
        ntca::FlowControlType::Value           direction,
        ntca::FlowControlMode::Value           mode,
        bool                                   defer,
        bool                                   lock);

    /// Disable copying from socket buffers in both directions and detach
    /// the socket from the reactor. Return true if asynchronous socket
    /// detachment started, otherwise return false.
    bool privateCloseFlowControl(
        const bsl::shared_ptr<ListenerSocket>& self,
        bool                                   defer,
        const ntci::SocketDetachedCallback&    detachCallback);

    /// Test if rate limiting is applied to accepting from the backlog,
    /// and if so, determine whether more connections are allowed to be
    /// dequeued from the backlog at this time. If not, apply flow control
    /// in the receive direction and schedule a timer to relax flow control
    /// in the receive direction at the estimated time that more data might
    /// be able to be received.
    ntsa::Error privateThrottleBacklog(
        const bsl::shared_ptr<ListenerSocket>& self);

    /// Accept a connection into the specified 'result'. Return the
    /// error. The behavior is undefined unless the read mutex is acquired.
    ntsa::Error privateDequeueBacklog(
        const bsl::shared_ptr<ListenerSocket>& self,
        bsl::shared_ptr<ntci::StreamSocket>*   result);

    /// Rearm the interest in the readability of the socket in the reactor,
    /// if necessary.
    void privateRearmAfterAccept(const bsl::shared_ptr<ListenerSocket>& self);

    /// Open the listener socket. Return the error.
    ntsa::Error privateOpen(const bsl::shared_ptr<ListenerSocket>& self);

    /// Open the listener socket using the specified 'transport'. Return the
    /// error.
    ntsa::Error privateOpen(const bsl::shared_ptr<ListenerSocket>& self,
                            ntsa::Transport::Value                 transport);

    /// Open the listener socket using the transport associated with the
    /// specified 'endpoint'. Return the error.
    ntsa::Error privateOpen(const bsl::shared_ptr<ListenerSocket>& self,
                            const ntsa::Endpoint&                  endpoint);

    /// Open the listener socket using the specified 'transport' imported
    /// from the specified 'handle'. Return the error.
    ntsa::Error privateOpen(const bsl::shared_ptr<ListenerSocket>& self,
                            ntsa::Transport::Value                 transport,
                            ntsa::Handle                           handle);

    /// Open the listener socket using the specified 'transport' imported
    /// from the specified 'listenerSocket'. Return the error.
    ntsa::Error privateOpen(
        const bsl::shared_ptr<ListenerSocket>&       self,
        ntsa::Transport::Value                       transport,
        const bsl::shared_ptr<ntsi::ListenerSocket>& listenerSocket);

    /// Process the resolution of the specified source 'endpoint' by the
    /// specified 'resolver' according to the specified 'getEndpointEvent'.
    /// Open the handle, if necessary, and bind to the resolved endpoint,
    /// if resolution was successful. Invoke the specified 'bindCallback'
    /// according to the specified 'bindOptions'.
    void processSourceEndpointResolution(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const ntsa::Endpoint&                  endpoint,
        const ntca::GetEndpointEvent&          getEndpointEvent,
        const ntca::BindOptions&               bindOptions,
        const ntci::BindCallback&              bindCallback);

  public:
    /// Create a new, initially uninitilialized listener socket. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Note that the 'create' function must be subsequently called
    /// before using this object.
    ListenerSocket(const ntca::ListenerSocketOptions&        options,
                   const bsl::shared_ptr<ntci::Resolver>&    resolver,
                   const bsl::shared_ptr<ntci::Reactor>&     reactor,
                   const bsl::shared_ptr<ntci::ReactorPool>& reactorPool,
                   const bsl::shared_ptr<ntcs::Metrics>&     metrics,
                   bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ListenerSocket() BSLS_KEYWORD_OVERRIDE;

    /// Open the listener socket. Return the error.
    ntsa::Error open() BSLS_KEYWORD_OVERRIDE;

    /// Open the listener socket using the specified 'transport'. Return the
    /// error.
    ntsa::Error open(ntsa::Transport::Value transport) BSLS_KEYWORD_OVERRIDE;

    /// Open the listener socket using the specified 'transport' imported
    /// from the specified 'handle'. Return the error.
    ntsa::Error open(ntsa::Transport::Value transport,
                     ntsa::Handle           handle) BSLS_KEYWORD_OVERRIDE;

    /// Open the stream socket using the specified 'transport' imported from
    /// the specified 'listenerSocket'. Return the error.
    ntsa::Error open(ntsa::Transport::Value transport,
                     const bsl::shared_ptr<ntsi::ListenerSocket>&
                         listenerSocket) BSLS_KEYWORD_OVERRIDE;

    /// Bind to the specified source 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on the callback's strand,
    /// if any, when the socket has been bound or any error occurs. Return
    /// the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created.
    ntsa::Error bind(const ntsa::Endpoint&     endpoint,
                     const ntca::BindOptions&  options,
                     const ntci::BindFunction& callback) BSLS_KEYWORD_OVERRIDE;

    /// Bind to the specified source 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on the callback's strand,
    /// if any, when the socket has been bound or any error occurs. Return
    /// the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created.
    ntsa::Error bind(const ntsa::Endpoint&     endpoint,
                     const ntca::BindOptions&  options,
                     const ntci::BindCallback& callback) BSLS_KEYWORD_OVERRIDE;

    /// Bind to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    ntsa::Error bind(const bsl::string&        name,
                     const ntca::BindOptions&  options,
                     const ntci::BindFunction& callback) BSLS_KEYWORD_OVERRIDE;

    /// Bind to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    ntsa::Error bind(const bsl::string&        name,
                     const ntca::BindOptions&  options,
                     const ntci::BindCallback& callback) BSLS_KEYWORD_OVERRIDE;

    /// Listen for connections. Return the error.
    ntsa::Error listen() BSLS_KEYWORD_OVERRIDE;

    /// Listen for connections using the specified 'backlog'. Return the
    /// error.
    ntsa::Error listen(bsl::size_t backlog) BSLS_KEYWORD_OVERRIDE;

    /// Dequeue a connection from the backlog according to the specified
    /// 'options'. If the accept queue is not empty, synchronously pop the
    /// front of the accept queue into the specified 'streamSocket'.
    /// Otherwise, asynchronously accept a connection from the backlog onto
    /// the accept queue as a connection in the backlog becomes accepted, at
    /// the configured accept rate limit, if any, up to the accept queue
    /// high watermark. After satisfying any queued accept operations, when
    /// the accept queue is asynchronously filled up to the accept queue low
    /// watermark, announce an accept queue low watermark event. When
    /// asynchronously enqueing connections onto the accept queue causes the
    /// accept queue high watermark to become breached, stop asynchronously
    /// accepting connections from the backlog onto the accept queue and
    /// announce an accept queue high watermark event. Return the error,
    /// notably 'ntsa::Error::e_WOULD_BLOCK' if neither the accept queue nor
    /// the backlog has any accepted connections. All other errors indicate
    /// no more connections have been accepted at this time or will become
    /// accepted in the future.
    ntsa::Error accept(ntca::AcceptContext*                 context,
                       bsl::shared_ptr<ntci::StreamSocket>* streamSocket,
                       const ntca::AcceptOptions&           options)
        BSLS_KEYWORD_OVERRIDE;

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
    /// event. When asynchronously enqueing connections onto the accept
    /// queue causes the accept queue high watermark to become breached,
    /// stop asynchronously accepting connections from the backlog onto the
    /// accept queue and announce an accept queue high watermark event.
    /// Return the error, notably 'ntsa::Error::e_WOULD_BLOCK' if neither
    /// the accept queue nor the backlog is non-empty. All other errors
    /// indicate no more connections have been accepted at this time or will
    /// become accepted in the future. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    ntsa::Error accept(const ntca::AcceptOptions&  options,
                       const ntci::AcceptFunction& callback)
        BSLS_KEYWORD_OVERRIDE;

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
    /// event. When asynchronously enqueing connections onto the accept
    /// queue causes the accept queue high watermark to become breached,
    /// stop asynchronously accepting connections from the backlog onto the
    /// accept queue and announce an accept queue high watermark event.
    /// Return the error, notably 'ntsa::Error::e_WOULD_BLOCK' if neither
    /// the accept queue nor the backlog is non-empty. All other errors
    /// indicate no more connections have been accepted at this time or will
    /// become accepted in the future. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    ntsa::Error accept(const ntca::AcceptOptions&  options,
                       const ntci::AcceptCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Register the specified 'resolver' for this socket. Return the error.
    ntsa::Error registerResolver(
        const bsl::shared_ptr<ntci::Resolver>& resolver) BSLS_KEYWORD_OVERRIDE;

    /// Deregister the current resolver for this socket. Return the error.
    ntsa::Error deregisterResolver() BSLS_KEYWORD_OVERRIDE;

    /// Register the specified 'manager' for this socket. Return the error.
    ntsa::Error registerManager(
        const bsl::shared_ptr<ntci::ListenerSocketManager>& manager)
        BSLS_KEYWORD_OVERRIDE;

    /// Deregister the current manager or manager callback for this socket.
    /// Return the error.
    ntsa::Error deregisterManager() BSLS_KEYWORD_OVERRIDE;

    /// Register the specified 'session' for this socket. Return the
    /// error.
    ntsa::Error registerSession(
        const bsl::shared_ptr<ntci::ListenerSocketSession>& session)
        BSLS_KEYWORD_OVERRIDE;

    /// Register the specified session 'callback' for this socket to be
    /// invoked on this socket's strand. Return the error.
    ntsa::Error registerSessionCallback(
        const ntci::ListenerSocket::SessionCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Register the specified session 'callback' for this socket to be
    /// invoked on the specified 'strand'. Return the error.
    ntsa::Error registerSessionCallback(
        const ntci::ListenerSocket::SessionCallback& callback,
        const bsl::shared_ptr<ntci::Strand>& strand) BSLS_KEYWORD_OVERRIDE;

    /// Deregister the current session or session callback for this socket.
    /// Return the error.
    ntsa::Error deregisterSession() BSLS_KEYWORD_OVERRIDE;

    /// Set the accept rate limiter to the specified 'rateLimiter'. Return
    /// the error.
    ntsa::Error setAcceptRateLimiter(const bsl::shared_ptr<ntci::RateLimiter>&
                                         rateLimiter) BSLS_KEYWORD_OVERRIDE;

    /// Set the accept queue low watermark to the specified 'lowWatermark'.
    /// Return the error.
    ntsa::Error setAcceptQueueLowWatermark(bsl::size_t lowWatermark)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the accept queue high watermark to the specified 'highWatermark'.
    /// Return the error.
    ntsa::Error setAcceptQueueHighWatermark(bsl::size_t highWatermark)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the accept queue limits to the specified 'lowWatermark' and
    /// 'highWatermark'. Return the error.
    ntsa::Error setAcceptQueueWatermarks(bsl::size_t lowWatermark,
                                         bsl::size_t highWatermark)
        BSLS_KEYWORD_OVERRIDE;

    /// Enable copying from the socket buffers in the specified 'direction'.
    ntsa::Error relaxFlowControl(ntca::FlowControlType::Value direction)
        BSLS_KEYWORD_OVERRIDE;

    /// Disable copying from socket buffers in the specified 'direction'
    /// according to the specified 'mode'.
    ntsa::Error applyFlowControl(ntca::FlowControlType::Value direction,
                                 ntca::FlowControlMode::Value mode)
        BSLS_KEYWORD_OVERRIDE;

    /// Cancel the bind operation identified by the specified 'token'.
    /// Return the error.
    ntsa::Error cancel(const ntca::BindToken& token) BSLS_KEYWORD_OVERRIDE;

    /// Cancel the accept operation identified by the specified 'token'.
    /// Return the error.
    ntsa::Error cancel(const ntca::AcceptToken& token) BSLS_KEYWORD_OVERRIDE;

    /// Shutdown the listener socket. Return the error.
    ntsa::Error shutdown() BSLS_KEYWORD_OVERRIDE;

    /// Close the listener socket.
    void close() BSLS_KEYWORD_OVERRIDE;

    /// Close the listener socket and invoke the specified 'callback' on
    /// the callback's strand, if any, when the listener socket is closed.
    /// Note that callbacks created by this object will automatically be
    /// invoked on this object's strand unless an explicit strand is
    /// specified at the time the callback is created.
    void close(const ntci::CloseFunction& callback) BSLS_KEYWORD_OVERRIDE;

    /// Close the listener socket and invoke the specified 'callback' on
    /// the callback's strand, if any, when the listener socket is closed.
    /// Note that callbacks created by this object will automatically be
    /// invoked on this object's strand unless an explicit strand is
    /// specified at the time the callback is created.
    void close(const ntci::CloseCallback& callback) BSLS_KEYWORD_OVERRIDE;

    /// Defer the execution of the specified 'functor'.
    void execute(const Functor& functor) BSLS_KEYWORD_OVERRIDE;

    /// Atomically defer the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE;

    // MANIPULATROS (ntci::StrandFactory)

    /// Create a new strand to serialize execution of functors. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::Strand> createStrand(
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'session' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&                  options,
        const bsl::shared_ptr<ntci::TimerSession>& session,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'callback' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&  options,
        const ntci::TimerCallback& callback,
        bslma::Allocator*          basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Return a shared pointer to a data container suitable for storing
    /// incoming data. The resulting data container is is automatically
    /// returned to this pool when its reference count reaches zero.
    bsl::shared_ptr<ntsa::Data> createIncomingData() BSLS_KEYWORD_OVERRIDE;

    /// Return a shared pointer to a data container suitable for storing
    /// outgoing data. The resulting data container is is automatically
    /// returned to this pool when its reference count reaches zero.
    bsl::shared_ptr<ntsa::Data> createOutgoingData() BSLS_KEYWORD_OVERRIDE;

    /// Return a shared pointer to a blob suitable for storing incoming
    /// data. The resulting blob is is automatically returned to this pool
    /// when its reference count reaches zero.
    bsl::shared_ptr<bdlbb::Blob> createIncomingBlob() BSLS_KEYWORD_OVERRIDE;

    /// Return a shared pointer to a blob suitable for storing incoming
    /// data. The resulting blob is is automatically returned to this pool
    /// when its reference count reaches zero.
    bsl::shared_ptr<bdlbb::Blob> createOutgoingBlob() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'blobBuffer' the data and size of a new
    /// buffer allocated from the incoming blob buffer factory.
    void createIncomingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'blobBuffer' the data and size of a new
    /// buffer allocated from the outgoing blob buffer factory.
    void createOutgoingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
        BSLS_KEYWORD_OVERRIDE;

    /// Return the descriptor handle.
    ntsa::Handle handle() const BSLS_KEYWORD_OVERRIDE;

    /// Return the transport of the listener socket.
    ntsa::Transport::Value transport() const BSLS_KEYWORD_OVERRIDE;

    /// Return the source endpoint.
    ntsa::Endpoint sourceEndpoint() const BSLS_KEYWORD_OVERRIDE;

    /// Return the strand that guarantees sequential, non-current execution
    /// of arbitrary functors on the unspecified threads processing events
    /// for this object.
    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;

    /// Return the handle of the thread that manages this socket, or
    /// the default value if no such thread has been set.
    bslmt::ThreadUtil::Handle threadHandle() const BSLS_KEYWORD_OVERRIDE;

    /// Return the index in the thread pool of the thread that manages this
    /// socket, or 0 if no such thread has been set.
    bsl::size_t threadIndex() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current number of connections queued after being accepted
    /// from the backlog. These connections have been accepted by the
    /// operating system buffers but not yet accepted by the user.
    bsl::size_t acceptQueueSize() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current accept queue low watermark.
    bsl::size_t acceptQueueLowWatermark() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current accept queue high watermark.
    bsl::size_t acceptQueueHighWatermark() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current elapsed time since the Unix epoch.
    bsls::TimeInterval currentTime() const BSLS_KEYWORD_OVERRIDE;

    /// Return the incoming blob buffer factory.
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& incomingBlobBufferFactory()
        const BSLS_KEYWORD_OVERRIDE;

    /// Return the outgoing blob buffer factory.
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& outgoingBlobBufferFactory()
        const BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace
#endif
