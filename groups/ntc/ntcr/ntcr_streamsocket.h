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

#ifndef INCLUDED_NTCR_STREAMSOCKET
#define INCLUDED_NTCR_STREAMSOCKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_streamsocketoptions.h>
#include <ntccfg_platform.h>
#include <ntci_datapool.h>
#include <ntci_listenersocket.h>
#include <ntci_reactor.h>
#include <ntci_reactorpool.h>
#include <ntci_reactorsocket.h>
#include <ntci_resolver.h>
#include <ntci_strand.h>
#include <ntci_streamsocket.h>
#include <ntci_streamsocketmanager.h>
#include <ntci_streamsocketsession.h>
#include <ntci_timer.h>
#include <ntcq_connect.h>
#include <ntcq_receive.h>
#include <ntcq_send.h>
#include <ntcq_zerocopy.h>
#include <ntcs_detachstate.h>
#include <ntcs_flowcontrolcontext.h>
#include <ntcs_flowcontrolstate.h>
#include <ntcs_metrics.h>
#include <ntcs_observer.h>
#include <ntcs_openstate.h>
#include <ntcs_shutdowncontext.h>
#include <ntcs_shutdownstate.h>
#include <ntcscm_version.h>
#include <ntcu_timestampcorrelator.h>
#include <ntsa_buffer.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_shutdownmode.h>
#include <ntsa_shutdowntype.h>
#include <ntsi_channel.h>
#include <ntsi_descriptor.h>
#include <bdlbb_blob.h>
#include <bdlma_aligningallocator.h>
#include <bdls_filesystemutil.h>
#include <bslmt_mutex.h>
#include <bsls_atomic.h>
#include <bsls_timeinterval.h>
#include <bsl_deque.h>
#include <bsl_list.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcr {

/// @internal @brief
/// Provide an asynchronous, reactively-driven stream socket.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcr
class StreamSocket : public ntci::StreamSocket,
                     public ntci::ReactorSocket,
                     public ntccfg::Shared<StreamSocket>
{
    /// Define a type alias for a shared pointer to a blob
    /// buffer factory.
    typedef bsl::shared_ptr<bdlbb::BlobBufferFactory> BlobBufferFactoryPtr;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    ntccfg::Object                             d_object;
    mutable Mutex                              d_mutex;
    ntsa::Handle                               d_systemHandle;
    ntsa::Handle                               d_publicHandle;
    ntsa::Transport::Value                     d_transport;
    ntsa::Endpoint                             d_sourceEndpoint;
    ntsa::Endpoint                             d_remoteEndpoint;
    bsl::shared_ptr<ntsi::StreamSocket>        d_socket_sp;
    bsl::shared_ptr<ntci::ListenerSocket>      d_acceptor_sp;
    bsl::shared_ptr<ntci::Encryption>          d_encryption_sp;
    ntcs::Observer<ntci::Resolver>             d_resolver;
    ntcs::Observer<ntci::Reactor>              d_reactor;
    ntcs::Observer<ntci::ReactorPool>          d_reactorPool;
    bsl::shared_ptr<ntci::Strand>              d_reactorStrand_sp;
    bsl::shared_ptr<ntci::StreamSocketManager> d_manager_sp;
    bsl::shared_ptr<ntci::Strand>              d_managerStrand_sp;
    bsl::shared_ptr<ntci::StreamSocketSession> d_session_sp;
    bsl::shared_ptr<ntci::Strand>              d_sessionStrand_sp;
    bsl::shared_ptr<ntci::DataPool>            d_dataPool_sp;
    BlobBufferFactoryPtr                       d_incomingBufferFactory_sp;
    BlobBufferFactoryPtr                       d_outgoingBufferFactory_sp;
    bsl::shared_ptr<ntcs::Metrics>             d_metrics_sp;
    ntcs::OpenState                            d_openState;
    ntcs::FlowControlState                     d_flowControlState;
    ntcs::ShutdownState                        d_shutdownState;
    ntcq::ZeroCopyQueue                        d_zeroCopyQueue;
    bsl::size_t                                d_zeroCopyThreshold;
    ntsa::SendOptions                          d_sendOptions;
    ntcq::SendQueue                            d_sendQueue;
    bsl::shared_ptr<ntci::RateLimiter>         d_sendRateLimiter_sp;
    bsl::shared_ptr<ntci::Timer>               d_sendRateTimer_sp;
    bool                                       d_sendGreedily;
    ntci::SendCallback                         d_sendComplete;
    ntcq::SendCounter                          d_sendCounter;
    bsl::shared_ptr<ntsa::Data>                d_sendData_sp;
    ntsa::ReceiveOptions                       d_receiveOptions;
    ntcq::ReceiveQueue                         d_receiveQueue;
    ntcq::ReceiveFeedback                      d_receiveFeedback;
    bsl::shared_ptr<ntci::RateLimiter>         d_receiveRateLimiter_sp;
    bsl::shared_ptr<ntci::Timer>               d_receiveRateTimer_sp;
    bool                                       d_receiveGreedily;
    bsl::shared_ptr<bdlbb::Blob>               d_receiveBlob_sp;
    ntsa::Endpoint                             d_connectEndpoint;
    bsl::string                                d_connectName;
    bsls::TimeInterval                         d_connectStartTime;
    bsl::size_t                                d_connectAttempts;
    ntca::ConnectOptions                       d_connectOptions;
    ntca::ConnectContext                       d_connectContext;
    ntci::ConnectCallback                      d_connectCallback;
    bsl::shared_ptr<ntci::Timer>               d_connectDeadlineTimer_sp;
    bsl::shared_ptr<ntci::Timer>               d_connectRetryTimer_sp;
    bool                                       d_connectInProgress;
    ntci::UpgradeCallback                      d_upgradeCallback;
    bsl::shared_ptr<ntci::Timer>               d_upgradeTimer_sp;
    bool                                       d_upgradeInProgress;
    bool                                       d_timestampOutgoingData;
    bool                                       d_timestampIncomingData;
    ntcu::TimestampCorrelator                  d_timestampCorrelator;
    bsl::uint32_t                              d_timestampCounter;
    const bool                                 d_oneShot;
    bool                                       d_retryConnect;
    ntcs::DetachState                          d_detachState;
    ntci::CloseCallback                        d_closeCallback;
    ntci::Executor::FunctorSequence            d_deferredCalls;
    bsl::size_t                                d_totalBytesSent;
    bsl::size_t                                d_totalBytesReceived;
    ntca::StreamSocketOptions                  d_options;
    bslma::Allocator*                          d_allocator_p;

  private:
    StreamSocket(const StreamSocket&) BSLS_KEYWORD_DELETED;
    StreamSocket& operator=(const StreamSocket&) BSLS_KEYWORD_DELETED;

  private:
    /// Process the readability of the socket.
    void processSocketReadable(const ntca::ReactorEvent& event)
        BSLS_KEYWORD_OVERRIDE;

    /// Process the writability of the socket.
    void processSocketWritable(const ntca::ReactorEvent& event)
        BSLS_KEYWORD_OVERRIDE;

    /// Process the specified 'error' for the socket.
    void processSocketError(const ntca::ReactorEvent& event)
        BSLS_KEYWORD_OVERRIDE;

    /// Process the specified 'notifications' of the socket.
    void processNotifications(const ntsa::NotificationQueue& notifications)
        BSLS_KEYWORD_OVERRIDE;

    /// Fail the connection operation.
    void processConnectDeadlineTimer(const bsl::shared_ptr<ntci::Timer>& timer,
                                     const ntca::TimerEvent& event);

    /// Fail the current connection attempt restart another one.
    void processConnectRetryTimer(const bsl::shared_ptr<ntci::Timer>& timer,
                                  const ntca::TimerEvent&             event);

    /// Fail the current upgrade operation.
    void processUpgradeTimer(const bsl::shared_ptr<ntci::Timer>& timer,
                             const ntca::TimerEvent&             event);

    /// Attempt to copy from the write queue to the send buffer after the
    /// write rate limiter estimates more data might be able to be sent.
    void processSendRateTimer(const bsl::shared_ptr<ntci::Timer>& timer,
                              const ntca::TimerEvent&             event);

    /// Fail the specified 'entryId' because the none of the entry's data
    /// had begun to be copied to the socket send buffer within the
    /// deadline.
    void processSendDeadlineTimer(const bsl::shared_ptr<ntci::Timer>& timer,
                                  const ntca::TimerEvent&             event,
                                  bsl::uint64_t                       entryId);

    /// Attempt to copy from the read queue to the receive buffer after the
    /// read rate limiter estimates more data might be able to be received.
    void processReceiveRateTimer(const bsl::shared_ptr<ntci::Timer>& timer,
                                 const ntca::TimerEvent&             event);

    /// Fail the specified 'entry' because the operation did not complete
    /// within the deadline.
    void processReceiveDeadlineTimer(
        const bsl::shared_ptr<ntci::Timer>&                     timer,
        const ntca::TimerEvent&                                 event,
        const bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>& entry);

    /// Process the completion or failure according to the specified 'error'
    /// of the TLS handshake to the peer identified by the specified
    /// 'certificate', if any. If an 'error' is indicated, the cause of the
    /// handshake failure is specified by 'details'.
    void privateEncryptionHandshake(
        const ntsa::Error&                                  error,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
        const bsl::string&                                  details);

    /// Process the readability of the socket by performing one read
    /// iteration.
    ntsa::Error privateSocketReadableIteration(
        const bsl::shared_ptr<StreamSocket>& self);

    /// Process the writability of the socket indicating the connection
    /// is established.
    ntsa::Error privateSocketWritableConnection(
        const bsl::shared_ptr<StreamSocket>& self);

    /// Process the writability of the socket by performing one write
    /// iteration.
    ntsa::Error privateSocketWritableIteration(
        const bsl::shared_ptr<StreamSocket>& self);

    /// Process the writability of the socket by performing one write
    /// iteration from the contiguous range of suitable entries at the front
    /// of the write queue.
    ntsa::Error privateSocketWritableIterationBatch(
        const bsl::shared_ptr<StreamSocket>& self);

    /// Process the writability of the socket by performing one write
    /// iteration from the entry at the front of the write queue.
    ntsa::Error privateSocketWritableIterationFront(
        const bsl::shared_ptr<StreamSocket>& self);

    /// Indicate a connection failure has occurred. If the specified 'defer'
    /// flag is true, ensure the announcement is deferred. If the specified
    /// 'close' flag is true, announce the connection failure regardless if
    /// the connection failure for this attempt was already announce (e.g.,
    /// in cases when the socket is closed after a previous connection
    /// attempt had failed, but the socket is waiting to retry another
    /// connection attempt) and detach the socket.
    /// If it is required to detach the socket from the proactor then part of
    /// described functionality will be executed asynchronously using the next
    /// method.
    void privateFailConnect(const bsl::shared_ptr<StreamSocket>& self,
                            const ntsa::Error&                   error,
                            bool                                 defer,
                            bool                                 close);

    /// Execute the second part of connection failure processing when the
    /// socket is detached. See also "privateFailConnect"
    void privateFailConnectPart2(const bsl::shared_ptr<StreamSocket>& self,
                                 bool                                 defer,
                                 const ntci::ConnectCallback& connectCallback,
                                 const ntca::ConnectEvent&    connectEvent,
                                 bool                         lock);

    /// Indicate a upgrade failure has occurred and detach the socket
    /// from its monitor.
    void privateFailUpgrade(const bsl::shared_ptr<StreamSocket>& self,
                            const ntsa::Error&                   error,
                            const bsl::string& errorDescription);

    /// Indicate a failure has occurred and detach the socket from its
    /// monitor.
    void privateFail(const bsl::shared_ptr<StreamSocket>& self,
                     const ntsa::Error&                   error);

    /// Shutdown the stream socket in the specified 'direction' according
    /// to the specified 'mode' of shutdown. Return the error.
    ntsa::Error privateShutdown(const bsl::shared_ptr<StreamSocket>& self,
                                ntsa::ShutdownType::Value            direction,
                                ntsa::ShutdownMode::Value            mode,
                                bool                                 defer);

    /// Process the advancement through the shutdown sequence by trying to
    /// shut down the socket for sending.
    void privateShutdownSend(const bsl::shared_ptr<StreamSocket>& self,
                             bool                                 defer);

    /// Process the advancement through the shutdown sequence by trying to
    /// shut down the socket for sending.
    void privateShutdownReceive(const bsl::shared_ptr<StreamSocket>& self,
                                ntsa::ShutdownOrigin::Value          origin,
                                bool                                 defer);

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
    void privateShutdownSequence(const bsl::shared_ptr<StreamSocket>& self,
                                 ntsa::ShutdownOrigin::Value          origin,
                                 const ntcs::ShutdownContext&         context,
                                 bool                                 defer);

    /// Execute the second part of shutdown sequence when the socket is
    /// detached. See also "privateShutdownSequence"
    void privateShutdownSequenceComplete(
        const bsl::shared_ptr<StreamSocket>& self,
        const ntcs::ShutdownContext&         context,
        bool                                 defer,
        bool                                 lock);

    /// Enable copying from the socket buffers in the specified 'direction'.
    /// The behavior is undefined unless 'd_mutex' is locked.
    ntsa::Error privateRelaxFlowControl(
        const bsl::shared_ptr<StreamSocket>& self,
        ntca::FlowControlType::Value         direction,
        bool                                 defer,
        bool                                 unlock);

    /// Disable copying from socket buffers in the specified 'direction'
    /// according to the specified 'mode'. The behavior is undefined unless
    /// 'd_mutex' is locked.
    ntsa::Error privateApplyFlowControl(
        const bsl::shared_ptr<StreamSocket>& self,
        ntca::FlowControlType::Value         direction,
        ntca::FlowControlMode::Value         mode,
        bool                                 defer,
        bool                                 lock);

    /// Disable copying from socket buffers in both directions and detach
    /// the socket from the reactor. Return true if asynchronous socket
    /// detachment has started. Otherwise return false.
    bool privateCloseFlowControl(
        const bsl::shared_ptr<StreamSocket>& self,
        bool                                 defer,
        const ntci::SocketDetachedCallback&  detachCallback);

    /// Test if rate limiting is applied to copying to the send buffer, and
    /// if so, determine whether more data is allowed to be copied to the
    /// send buffer at this time. If not, apply flow control in the send
    /// direction and schedule a timer to relax flow control in the send
    /// direction at the estimated time that more data might be able to be
    /// sent.
    ntsa::Error privateThrottleSendBuffer(
        const bsl::shared_ptr<StreamSocket>& self);

    /// Test if rate limiting is applied to copying from the receive buffer,
    /// and if so, determine whether more data is allowed to be copied from
    /// the receive buffer at this time. If not, apply flow control in the
    /// receive direction and schedule a timer to relax flow control in
    /// the receive direction at the estimated time that more data might be
    /// able to be received.
    ntsa::Error privateThrottleReceiveBuffer(
        const bsl::shared_ptr<StreamSocket>& self);

    /// Enqueue the specified 'data' to the socket send buffer. Return the
    /// error.
    ntsa::Error privateEnqueueSendBuffer(
        const bsl::shared_ptr<StreamSocket>& self,
        ntsa::SendContext*                   context,
        const bdlbb::Blob&                   data);

    /// Enqueue the specified 'data' to the socket send buffer. Return the
    /// error.
    ntsa::Error privateEnqueueSendBuffer(
        const bsl::shared_ptr<StreamSocket>& self,
        ntsa::SendContext*                   context,
        const ntsa::Data&                    data);

    /// Dequeue data from the socket receive buffer. Append to the
    /// specified 'data' the data dequeued. Return the error.
    ntsa::Error privateDequeueReceiveBuffer(
        const bsl::shared_ptr<StreamSocket>& self,
        ntsa::ReceiveContext*                context,
        bdlbb::Blob*                         data);

    /// Dequeue raw or encrypted data from the socket receive buffer. Append
    /// to the specified 'data' the data dequeued . Return the error.
    ntsa::Error privateDequeueReceiveBufferRaw(
        const bsl::shared_ptr<StreamSocket>& self,
        ntsa::ReceiveContext*                context,
        bdlbb::Blob*                         data);

    /// Rearm the interest in the writability of the socket in the reactor,
    /// if necessary.
    void privateRearmAfterSend(const bsl::shared_ptr<StreamSocket>& self);

    /// Rearm the interest in the readability of the socket in the reactor,
    /// if necessary.
    void privateRearmAfterReceive(const bsl::shared_ptr<StreamSocket>& self);

    /// Rearm the interest in notifications from the socket in the reactor.
    void privateRearmAfterNotification(
        const bsl::shared_ptr<StreamSocket>& self);

    /// Send the specified raw or already encrypted 'data' according to the
    /// specified 'options'. When the 'data' is entirely copied to the
    /// send buffer, invoke the specified 'callback' on the callback's
    /// strand, if any.  Return the error.
    ntsa::Error privateSendRaw(const bsl::shared_ptr<StreamSocket>& self,
                               const bdlbb::Blob&                   data,
                               const ntcq::SendState&               state,
                               const ntca::SendOptions&             options,
                               const ntci::SendCallback&            callback);

    /// Send the specified raw or already encrypted 'data' according to the
    /// specified 'options'. When the 'data' is entirely copied to the
    /// send buffer, invoke the specified 'callback' on the callback's
    /// strand, if any. Return the error.
    ntsa::Error privateSendRaw(const bsl::shared_ptr<StreamSocket>& self,
                               const ntsa::Data&                    data,
                               const ntcq::SendState&               state,
                               const ntca::SendOptions&             options,
                               const ntci::SendCallback&            callback);

    /// Send the specified encrypted 'data' according to the specified
    /// 'options'. When the 'data' is entirely copied to the send buffer,
    /// invoke the specified 'callback' on the callback's strand, if any.
    /// Return the error.
    ntsa::Error privateSendEncrypted(const bsl::shared_ptr<StreamSocket>& self,
                                     const bdlbb::Blob&                   data,
                                     const ntcq::SendState&    state,
                                     const ntca::SendOptions&  options,
                                     const ntci::SendCallback& callback);

    /// Send the specified encrypted 'data' according to the specified
    /// 'options'. When the 'data' is entirely copied to the send buffer,
    /// invoke the specified 'callback' on the callback's strand, if any.
    /// Return the error.
    ntsa::Error privateSendEncrypted(const bsl::shared_ptr<StreamSocket>& self,
                                     const ntsa::Data&                    data,
                                     const ntcq::SendState&    state,
                                     const ntca::SendOptions&  options,
                                     const ntci::SendCallback& callback);

    /// Open the stream socket. Return the error.
    ntsa::Error privateOpen(const bsl::shared_ptr<StreamSocket>& self);

    /// Open the stream socket using the specified 'transport'. Return the
    /// error.
    ntsa::Error privateOpen(const bsl::shared_ptr<StreamSocket>& self,
                            ntsa::Transport::Value               transport);

    /// Open the stream socket using the transport associated with the
    /// specified 'endpoint'. Return the error.
    ntsa::Error privateOpen(const bsl::shared_ptr<StreamSocket>& self,
                            const ntsa::Endpoint&                endpoint);

    /// Open the stream socket using the specified 'transport' imported
    /// from the specified 'handle'. Return the error.
    ntsa::Error privateOpen(const bsl::shared_ptr<StreamSocket>& self,
                            ntsa::Transport::Value               transport,
                            ntsa::Handle                         handle);

    /// Open the stream socket using the specified 'transport' imported
    /// from the specified 'streamSocket'. Return the error.
    ntsa::Error privateOpen(
        const bsl::shared_ptr<StreamSocket>&       self,
        ntsa::Transport::Value                     transport,
        const bsl::shared_ptr<ntsi::StreamSocket>& streamSocket);

    /// Open the stream socket using the specified 'transport' imported
    /// from the specified 'handle' accepted by the specified 'acceptor'.
    /// Return the error.
    ntsa::Error privateOpen(
        const bsl::shared_ptr<StreamSocket>&         self,
        ntsa::Transport::Value                       transport,
        ntsa::Handle                                 handle,
        const bsl::shared_ptr<ntci::ListenerSocket>& acceptor);

    /// Open the stream socket using the specified 'transport' imported
    /// from the specified 'streamSocket' accepted by the specified
    /// 'acceptor'. Return the error.
    ntsa::Error privateOpen(
        const bsl::shared_ptr<StreamSocket>&         self,
        ntsa::Transport::Value                       transport,
        const bsl::shared_ptr<ntsi::StreamSocket>&   streamSocket,
        const bsl::shared_ptr<ntci::ListenerSocket>& acceptor);

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

    /// Process the resolution of the specified source 'endpoint' by the
    /// specified 'resolver' according to the specified 'getEndpointEvent'.
    /// Open the handle, if necessary, and connect to the resolved endpoint,
    /// if resolution was successful. Invoke 'd_connectCallback'
    /// according to 'd_connectOptions' when the connection completes or
    /// fails.
    void processRemoteEndpointResolution(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const ntsa::Endpoint&                  endpoint,
        const ntca::GetEndpointEvent&          getEndpointEvent,
        bsl::size_t                            connectAttempts);

    /// Initiate the upgrade. Return the error.
    ntsa::Error privateUpgrade(
        const bsl::shared_ptr<StreamSocket>& self,
        const ntca::UpgradeOptions&          upgradeOptions);

    /// Retry connecting to the remote peer.
    void privateRetryConnect(const bsl::shared_ptr<StreamSocket>& self);

    /// Retry connecting to the remote name. Return the error.
    ntsa::Error privateRetryConnectToName();

    /// Retry connecting to the remote endpoint. Return the error.
    ntsa::Error privateRetryConnectToEndpoint(
        const bsl::shared_ptr<StreamSocket>& self);

    /// Enable or disable timestamping of outgoing data according to the
    /// specified 'enable' flag. Return the error.
    ntsa::Error privateTimestampOutgoingData(
        const bsl::shared_ptr<StreamSocket>& self,
        bool                                 enable);

    /// Enable or disable timestamping of incoming data according to the
    /// specified 'enable' flag. Return the error.
    ntsa::Error privateTimestampIncomingData(
        const bsl::shared_ptr<StreamSocket>& self,
        bool                                 enable);

    /// Process the detection of the specified outgoing data 'timestamp'.
    void privateTimestampUpdate(const bsl::shared_ptr<StreamSocket>& self,
                                const ntsa::Timestamp& timestamp);

    // Engage zero-copy transmissions for data whose size is greater than or
    // equal to the specified 'threshold', in bytes. Return the error.
    ntsa::Error privateZeroCopyEngage(
        const bsl::shared_ptr<StreamSocket>& self,
        bsl::size_t                          threshold);

    // Process the completion of one or more zero-copy transmissions described
    // by the specified 'zeroCopy' notification.
    void privateZeroCopyUpdate(const bsl::shared_ptr<StreamSocket>& self,
                               const ntsa::ZeroCopy&                zeroCopy);

  public:
    /// Create a new, initially uninitilialized stream socket. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Note that the 'create' function must be subsequently called
    /// before using this object.
    StreamSocket(const ntca::StreamSocketOptions&          options,
                 const bsl::shared_ptr<ntci::Resolver>&    resolver,
                 const bsl::shared_ptr<ntci::Reactor>&     reactor,
                 const bsl::shared_ptr<ntci::ReactorPool>& reactorPool,
                 const bsl::shared_ptr<ntcs::Metrics>&     metrics,
                 bslma::Allocator*                         basicAllocator = 0);

    /// Destroy this object.
    ~StreamSocket() BSLS_KEYWORD_OVERRIDE;

    /// Open the stream socket. Return the error.
    ntsa::Error open() BSLS_KEYWORD_OVERRIDE;

    /// Open the stream socket using the specified 'transport'. Return the
    /// error.
    ntsa::Error open(ntsa::Transport::Value transport) BSLS_KEYWORD_OVERRIDE;

    /// Open the stream socket using the specified 'transport' imported from
    /// the specified 'handle'. Return the error.
    ntsa::Error open(ntsa::Transport::Value transport,
                     ntsa::Handle           handle) BSLS_KEYWORD_OVERRIDE;

    /// Open the stream socket using the specified 'transport' imported from
    /// the specified 'streamSocket'. Return the error.
    ntsa::Error open(ntsa::Transport::Value                     transport,
                     const bsl::shared_ptr<ntsi::StreamSocket>& streamSocket)
        BSLS_KEYWORD_OVERRIDE;

    /// Open the stream socket using the specified 'transport' imported from
    /// the specified 'handle' accepted by the specified 'acceptor'. Return
    /// the error.
    ntsa::Error open(ntsa::Transport::Value                       transport,
                     ntsa::Handle                                 handle,
                     const bsl::shared_ptr<ntci::ListenerSocket>& acceptor)
        BSLS_KEYWORD_OVERRIDE;

    /// Open the stream socket using the specified 'transport' imported from
    /// the specified 'streamSocket' accepted by the specified 'acceptor'.
    /// Return the error.
    ntsa::Error open(ntsa::Transport::Value                       transport,
                     const bsl::shared_ptr<ntsi::StreamSocket>&   streamSocket,
                     const bsl::shared_ptr<ntci::ListenerSocket>& acceptor)
        BSLS_KEYWORD_OVERRIDE;

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

    /// Connect to the specified 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on the callback's strand,
    /// if any, when the connection is established or an error occurs.
    /// Return the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created.
    ntsa::Error connect(const ntsa::Endpoint&        endpoint,
                        const ntca::ConnectOptions&  options,
                        const ntci::ConnectFunction& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Connect to the specified 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on the callback's strand,
    /// if any, when the connection is established or an error occurs.
    /// Return the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created.
    ntsa::Error connect(const ntsa::Endpoint&        endpoint,
                        const ntca::ConnectOptions&  options,
                        const ntci::ConnectCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Connect to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the connection is established or an
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    ntsa::Error connect(const bsl::string&           name,
                        const ntca::ConnectOptions&  options,
                        const ntci::ConnectFunction& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Connect to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the connection is established or an
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    ntsa::Error connect(const bsl::string&           name,
                        const ntca::ConnectOptions&  options,
                        const ntci::ConnectCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryption'. If the 'encryption' was created from an encryption
    /// client, the upgrade process will proceed in the client (i.e.
    /// connector) role. If the 'encryption' was created from an encryption
    /// server, the upgrade process will proceed in the server (i.e.
    /// acceptor) role. The upgrade process will re-use state cached from
    /// previous uses the 'encryption', if any. Invoke the specified
    /// 'callback' on the callback's strand, if any, when the upgrade is
    /// complete or any error occurs. Return the error. Note that callbacks
    /// created by this object will automatically be invoked on this
    /// object's strand unless an explicit strand is specified at the time
    /// the callback is created.
    ntsa::Error upgrade(const bsl::shared_ptr<ntci::Encryption>& encryption,
                        const ntca::UpgradeOptions&              options,
                        const ntci::UpgradeFunction&             callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryption'. If the 'encryption' was created from an encryption
    /// client, the upgrade process will proceed in the client (i.e.
    /// connector) role. If the 'encryption' was created from an encryption
    /// server, the upgrade process will proceed in the server (i.e.
    /// acceptor) role. The upgrade process will re-use state cached from
    /// previous uses the 'encryption', if any. Invoke the specified
    /// 'callback' on the callback's strand, if any, when the upgrade is
    /// complete or any error occurs. Return the error. Note that callbacks
    /// created by this object will automatically be invoked on this
    /// object's strand unless an explicit strand is specified at the time
    /// the callback is created.
    ntsa::Error upgrade(const bsl::shared_ptr<ntci::Encryption>& encryption,
                        const ntca::UpgradeOptions&              options,
                        const ntci::UpgradeCallback&             callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryptionClient'. The upgrade process will proceed in the client
    /// (i.e. connector) role. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    ntsa::Error upgrade(
        const bsl::shared_ptr<ntci::EncryptionClient>& encryptionClient,
        const ntca::UpgradeOptions&                    options,
        const ntci::UpgradeFunction& callback) BSLS_KEYWORD_OVERRIDE;

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryptionClient'. The upgrade process will proceed in the client
    /// (i.e. connector) role. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    ntsa::Error upgrade(
        const bsl::shared_ptr<ntci::EncryptionClient>& encryptionClient,
        const ntca::UpgradeOptions&                    options,
        const ntci::UpgradeCallback& callback) BSLS_KEYWORD_OVERRIDE;

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryptionServer'. The upgrade process will proceed in the server
    /// (i.e. acceptor) role. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    ntsa::Error upgrade(
        const bsl::shared_ptr<ntci::EncryptionServer>& encryptionServer,
        const ntca::UpgradeOptions&                    options,
        const ntci::UpgradeFunction& callback) BSLS_KEYWORD_OVERRIDE;

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryptionServer'. The upgrade process will proceed in the server
    /// (i.e. acceptor) role. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    ntsa::Error upgrade(
        const bsl::shared_ptr<ntci::EncryptionServer>& encryptionServer,
        const ntca::UpgradeOptions&                    options,
        const ntci::UpgradeCallback& callback) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'data' for transmission according to the
    /// specified 'options'. If the write queue is empty and the write rate
    /// limit, if any, is not exceeded, synchronously copy the 'data' to the
    /// socket send buffer. Otherwise, if the write queue is not empty, or
    /// the write rate limit, if any, is exceeded, or the socket send buffer
    /// has insufficient capacity to store the entirety of the 'data',
    /// enqueue the remainder of the 'data' not copied to the socket send
    /// buffer onto the write queue and asynchronously copy the write queue
    /// to the socket send buffer as capacity in the socket send buffer
    /// becomes available, at the configured write rate limit, if any,
    /// according to the priorities of the individual write operations on
    /// the write queue. The integrity of the entire sequence of the 'data'
    /// is always preserved when transmitting the data stream, even when
    /// other data is sent concurrently by different threads, although the
    /// order of transmission of the entirety of the 'data' in relation to
    /// other transmitted data is unspecified. If enqueuing the 'data' onto
    /// the write queue causes the write queue high watermark to become
    /// breached, announce a write queue high watermark event but continue
    /// to queue the 'data' for transmission. After a write queue high
    /// watermark event is announced, announce a write queue low watermark
    /// event when the write queue is subsequently and asynchronously
    /// drained down to the write queue low watermark. A write queue high
    /// watermark event must be first announced before a write queue low
    /// watermark event will be announced, and thereafter a write queue
    /// low watermark event must be announced before a subsequent write
    /// queue high watermark event will be announced. Return the error,
    /// notably 'ntsa::Error::e_WOULD_BLOCK' if the size of the write queue
    /// has already breached the write queue high watermark. All other
    /// errors indicate the socket is incapable of transmitting data at this
    /// time or any time in the future.
    ntsa::Error send(const bdlbb::Blob&       data,
                     const ntca::SendOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'data' for transmission according to the
    /// specified 'options'. If the write queue is empty and the write rate
    /// limit, if any, is not exceeded, synchronously copy the 'data' to the
    /// socket send buffer. Otherwise, if the write queue is not empty, or
    /// the write rate limit, if any, is exceeded, or the socket send buffer
    /// has insufficient capacity to store the entirety of the 'data',
    /// enqueue the remainder of the 'data' not copied to the socket send
    /// buffer onto the write queue and asynchronously copy the write queue
    /// to the socket send buffer as capacity in the socket send buffer
    /// becomes available, at the configured write rate limit, if any,
    /// according to the priorities of the individual write operations on
    /// the write queue. The integrity of the entire sequence of the 'data'
    /// is always preserved when transmitting the data stream, even when
    /// other data is sent concurrently by different threads, although the
    /// order of transmission of the entirety of the 'data' in relation to
    /// other transmitted data is unspecified. If enqueuing the 'data' onto
    /// the write queue causes the write queue high watermark to become
    /// breached, announce a write queue high watermark event but continue
    /// to queue the 'data' for transmission. After a write queue high
    /// watermark event is announced, announce a write queue low watermark
    /// event when the write queue is subsequently and asynchronously
    /// drained down to the write queue low watermark. A write queue high
    /// watermark event must be first announced before a write queue low
    /// watermark event will be announced, and thereafter a write queue
    /// low watermark event must be announced before a subsequent write
    /// queue high watermark event will be announced. Return the error,
    /// notably 'ntsa::Error::e_WOULD_BLOCK' if the size of the write queue
    /// has already breached the write queue high watermark. All other
    /// errors indicate the socket is incapable of transmitting data at this
    /// time or any time in the future. Note that 'ntsa::Data' is a
    /// container of data with many possibly representations and ownership
    /// semantics, and the 'data' will be queued, copied, shared, or held
    /// according to the user's choice of representation.
    ntsa::Error send(const ntsa::Data&        data,
                     const ntca::SendOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'data' for transmission according to the
    /// specified 'options'. If the write queue is empty and the write rate
    /// limit, if any, is not exceeded, synchronously copy the 'data' to the
    /// socket send buffer. Otherwise, if the write queue is not empty, or
    /// the write rate limit, if any, is exceeded, or the socket send buffer
    /// has insufficient capacity to store the entirety of the 'data',
    /// enqueue the remainder of the 'data' not copied to the socket send
    /// buffer onto the write queue and asynchronously copy the write queue
    /// to the socket send buffer as capacity in the socket send buffer
    /// becomes available, at the configured write rate limit, if any,
    /// according to the priorities of the individual write operations on
    /// the write queue. The integrity of the entire sequence of the 'data'
    /// is always preserved when transmitting the data stream, even when
    /// other data is sent concurrently by different threads, although the
    /// order of transmission of the entirety of the 'data' in relation to
    /// other transmitted data is unspecified. If enqueuing the 'data' onto
    /// the write queue causes the write queue high watermark to become
    /// breached, announce a write queue high watermark event but continue
    /// to queue the 'data' for transmission. After a write queue high
    /// watermark event is announced, announce a write queue low watermark
    /// event when the write queue is subsequently and asynchronously
    /// drained down to the write queue low watermark. A write queue high
    /// watermark event must be first announced before a write queue low
    /// watermark event will be announced, and thereafter a write queue
    /// low watermark event must be announced before a subsequent write
    /// queue high watermark event will be announced. When the 'data' has
    /// been completly copied to the send buffer, invoke the specified
    /// 'callback' on the callbacks's strand, if any. Return the error,
    /// notably 'ntsa::Error::e_WOULD_BLOCK' if the size of the write queue
    /// has already breached the write queue high watermark. All other
    /// errors indicate the socket is incapable of transmitting data at this
    /// time or any time in the future. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    ntsa::Error send(const bdlbb::Blob&        data,
                     const ntca::SendOptions&  options,
                     const ntci::SendFunction& callback) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'data' for transmission according to the
    /// specified 'options'. If the write queue is empty and the write rate
    /// limit, if any, is not exceeded, synchronously copy the 'data' to the
    /// socket send buffer. Otherwise, if the write queue is not empty, or
    /// the write rate limit, if any, is exceeded, or the socket send buffer
    /// has insufficient capacity to store the entirety of the 'data',
    /// enqueue the remainder of the 'data' not copied to the socket send
    /// buffer onto the write queue and asynchronously copy the write queue
    /// to the socket send buffer as capacity in the socket send buffer
    /// becomes available, at the configured write rate limit, if any,
    /// according to the priorities of the individual write operations on
    /// the write queue. The integrity of the entire sequence of the 'data'
    /// is always preserved when transmitting the data stream, even when
    /// other data is sent concurrently by different threads, although the
    /// order of transmission of the entirety of the 'data' in relation to
    /// other transmitted data is unspecified. If enqueuing the 'data' onto
    /// the write queue causes the write queue high watermark to become
    /// breached, announce a write queue high watermark event but continue
    /// to queue the 'data' for transmission. After a write queue high
    /// watermark event is announced, announce a write queue low watermark
    /// event when the write queue is subsequently and asynchronously
    /// drained down to the write queue low watermark. A write queue high
    /// watermark event must be first announced before a write queue low
    /// watermark event will be announced, and thereafter a write queue
    /// low watermark event must be announced before a subsequent write
    /// queue high watermark event will be announced. When the 'data' has
    /// been completly copied to the send buffer, invoke the specified
    /// 'callback' on the callbacks's strand, if any. Return the error,
    /// notably 'ntsa::Error::e_WOULD_BLOCK' if the size of the write queue
    /// has already breached the write queue high watermark. All other
    /// errors indicate the socket is incapable of transmitting data at this
    /// time or any time in the future. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    ntsa::Error send(const bdlbb::Blob&        data,
                     const ntca::SendOptions&  options,
                     const ntci::SendCallback& callback) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'data' for transmission according to the
    /// specified 'options'. If the write queue is empty and the write rate
    /// limit, if any, is not exceeded, synchronously copy the 'data' to the
    /// socket send buffer. Otherwise, if the write queue is not empty, or
    /// the write rate limit, if any, is exceeded, or the socket send buffer
    /// has insufficient capacity to store the entirety of the 'data',
    /// enqueue the remainder of the 'data' not copied to the socket send
    /// buffer onto the write queue and asynchronously copy the write queue
    /// to the socket send buffer as capacity in the socket send buffer
    /// becomes available, at the configured write rate limit, if any,
    /// according to the priorities of the individual write operations on
    /// the write queue. The integrity of the entire sequence of the 'data'
    /// is always preserved when transmitting the data stream, even when
    /// other data is sent concurrently by different threads, although the
    /// order of transmission of the entirety of the 'data' in relation to
    /// other transmitted data is unspecified. If enqueuing the 'data' onto
    /// the write queue causes the write queue high watermark to become
    /// breached, announce a write queue high watermark event but continue
    /// to queue the 'data' for transmission. After a write queue high
    /// watermark event is announced, announce a write queue low watermark
    /// event when the write queue is subsequently and asynchronously
    /// drained down to the write queue low watermark. A write queue high
    /// watermark event must be first announced before a write queue low
    /// watermark event will be announced, and thereafter a write queue
    /// low watermark event must be announced before a subsequent write
    /// queue high watermark event will be announced. When the 'data' has
    /// been completly copied to the send buffer, invoke the specified
    /// 'callback' on the callbacks's strand, if any. Return the error,
    /// notably 'ntsa::Error::e_WOULD_BLOCK' if the size of the write queue
    /// has already breached the write queue high watermark. All other
    /// errors indicate the socket is incapable of transmitting data at this
    /// time or any time in the future. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    /// Also note that 'ntsa::Data' is a container of data with many
    /// possibly representations and ownership semantics, and the 'data'
    /// will be queued, copied, shared, or held according to the user's
    /// choice of representation.
    ntsa::Error send(const ntsa::Data&         data,
                     const ntca::SendOptions&  options,
                     const ntci::SendFunction& callback) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'data' for transmission according to the
    /// specified 'options'. If the write queue is empty and the write rate
    /// limit, if any, is not exceeded, synchronously copy the 'data' to the
    /// socket send buffer. Otherwise, if the write queue is not empty, or
    /// the write rate limit, if any, is exceeded, or the socket send buffer
    /// has insufficient capacity to store the entirety of the 'data',
    /// enqueue the remainder of the 'data' not copied to the socket send
    /// buffer onto the write queue and asynchronously copy the write queue
    /// to the socket send buffer as capacity in the socket send buffer
    /// becomes available, at the configured write rate limit, if any,
    /// according to the priorities of the individual write operations on
    /// the write queue. The integrity of the entire sequence of the 'data'
    /// is always preserved when transmitting the data stream, even when
    /// other data is sent concurrently by different threads, although the
    /// order of transmission of the entirety of the 'data' in relation to
    /// other transmitted data is unspecified. If enqueuing the 'data' onto
    /// the write queue causes the write queue high watermark to become
    /// breached, announce a write queue high watermark event but continue
    /// to queue the 'data' for transmission. After a write queue high
    /// watermark event is announced, announce a write queue low watermark
    /// event when the write queue is subsequently and asynchronously
    /// drained down to the write queue low watermark. A write queue high
    /// watermark event must be first announced before a write queue low
    /// watermark event will be announced, and thereafter a write queue
    /// low watermark event must be announced before a subsequent write
    /// queue high watermark event will be announced. When the 'data' has
    /// been completly copied to the send buffer, invoke the specified
    /// 'callback' on the callbacks's strand, if any. Return the error,
    /// notably 'ntsa::Error::e_WOULD_BLOCK' if the size of the write queue
    /// has already breached the write queue high watermark. All other
    /// errors indicate the socket is incapable of transmitting data at this
    /// time or any time in the future. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    /// Also note that 'ntsa::Data' is a container of data with many
    /// possibly representations and ownership semantics, and the 'data'
    /// will be queued, copied, shared, or held according to the user's
    /// choice of representation.
    ntsa::Error send(const ntsa::Data&         data,
                     const ntca::SendOptions&  options,
                     const ntci::SendCallback& callback) BSLS_KEYWORD_OVERRIDE;

    /// Dequeue received data according to the specified 'options'. If the
    /// read queue has sufficient size to fill the 'data', synchronously
    /// copy the read queue into the specified 'data'. Otherwise,
    /// asynchronously copy the socket receive buffer onto the read queue
    /// as data in the socket receive buffer becomes available, at the
    /// configured read rate limit, if any, up to the read queue high
    /// watermark. After satisfying any queued read operations, when the
    /// read queue is asynchronously filled up to the read queue low
    /// watermark, announce a read queue low watermark event. When
    /// asynchronously enqueing data onto the read queue causes the read
    /// queue high watermark to become breached, stop asynchronously copying
    /// the socket receive buffer to the read queue and announce a read
    /// queue high watermark event. Return the error, notably
    /// 'ntsa::Error::e_WOULD_BLOCK' if neither the read queue nor the
    /// socket receive buffer has sufficient size to fill the 'data', or
    /// 'ntsa::Error::e_EOF' if the read queue is empty and the socket
    /// receive buffer has been shut down. All other errors indicate no more
    /// received data is available at this time or will become available in
    /// the future.
    ntsa::Error receive(ntca::ReceiveContext*       context,
                        bdlbb::Blob*                data,
                        const ntca::ReceiveOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Dequeue received data according to the specified 'options'. If the
    /// read queue has sufficient size to satisfy the read operation,
    /// synchronously copy the read queue into an internally allocated data
    /// structure and invoke the specified 'callback' on the callback's
    /// strand, if any, with that data structure. Otherwise, queue the read
    /// operation and asynchronously copy the socket receive buffer onto the
    /// read queue as data in the socket receive buffer becomes available,
    /// at the configured read rate limit, if any, up to the read queue high
    /// watermark. When the read queue is asynchronously filled to a
    /// sufficient size to satisfy the read operation, synchronously copy
    /// the read queue into an internally allocated data structure and
    /// invoke the 'callback' on the callback's strand, if any, with that
    /// data structure. After satisfying any queued read operations, when
    /// the read queue is asynchronously filled up to the read queue low
    /// watermark, announce a read queue low watermark event. When
    /// asynchronously enqueing data onto the read queue causes the read
    /// queue high watermark to become breached, stop asynchronously copying
    /// the socket receive buffer to the read queue and announce a read
    /// queue high watermark event. Load into the specified 'cancellation'
    /// the mechanism to cancel or abort the read operation. Return the
    /// error, notably 'ntsa::Error::e_WOULD_BLOCK' if neither the read
    /// queue nor the socket receive buffer has sufficient size to
    /// synchronously satisfy the read operation, or 'ntsa::Error::e_EOF' if
    /// the read queue is empty and the socket receive buffer has been shut
    /// down. All other errors indicate no more received data is available
    /// at this time or will become available in the future. Note that
    /// callbacks created by this object will automatically be invoked on
    /// this object's strand unless an explicit strand is specified at the
    /// time the callback is created.
    ntsa::Error receive(const ntca::ReceiveOptions&  options,
                        const ntci::ReceiveFunction& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Dequeue received data according to the specified 'options'. If the
    /// read queue has sufficient size to satisfy the read operation,
    /// synchronously copy the read queue into an internally allocated data
    /// structure and invoke the specified 'callback' on the callback's
    /// strand, if any, with that data structure. Otherwise, queue the read
    /// operation and asynchronously copy the socket receive buffer onto the
    /// read queue as data in the socket receive buffer becomes available,
    /// at the configured read rate limit, if any, up to the read queue high
    /// watermark. When the read queue is asynchronously filled to a
    /// sufficient size to satisfy the read operation, synchronously copy
    /// the read queue into an internally allocated data structure and
    /// invoke the 'callback' on the callback's strand, if any, with that
    /// data structure. After satisfying any queued read operations, when
    /// the read queue is asynchronously filled up to the read queue low
    /// watermark, announce a read queue low watermark event. When
    /// asynchronously enqueing data onto the read queue causes the read
    /// queue high watermark to become breached, stop asynchronously copying
    /// the socket receive buffer to the read queue and announce a read
    /// queue high watermark event. Load into the specified 'cancellation'
    /// the mechanism to cancel or abort the read operation. Return the
    /// error, notably 'ntsa::Error::e_WOULD_BLOCK' if neither the read
    /// queue nor the socket receive buffer has sufficient size to
    /// synchronously satisfy the read operation, or 'ntsa::Error::e_EOF' if
    /// the read queue is empty and the socket receive buffer has been shut
    /// down. All other errors indicate no more received data is available
    /// at this time or will become available in the future. Note that
    /// callbacks created by this object will automatically be invoked on
    /// this object's strand unless an explicit strand is specified at the
    /// time the callback is created.
    ntsa::Error receive(const ntca::ReceiveOptions&  options,
                        const ntci::ReceiveCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Register the specified 'resolver' for this socket. Return the error.
    ntsa::Error registerResolver(
        const bsl::shared_ptr<ntci::Resolver>& resolver) BSLS_KEYWORD_OVERRIDE;

    /// Deregister the current resolver for this socket. Return the error.
    ntsa::Error deregisterResolver() BSLS_KEYWORD_OVERRIDE;

    /// Register the specified 'manager' for this socket. Return the error.
    ntsa::Error registerManager(
        const bsl::shared_ptr<ntci::StreamSocketManager>& manager)
        BSLS_KEYWORD_OVERRIDE;

    /// Deregister the current manager or manager callback for this socket.
    /// Return the error.
    ntsa::Error deregisterManager() BSLS_KEYWORD_OVERRIDE;

    /// Register the specified 'session' for this socket. Return the
    /// error.
    ntsa::Error registerSession(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session)
        BSLS_KEYWORD_OVERRIDE;

    /// Register the specified session 'callback' for this socket to be
    /// invoked on this socket's strand. Return the error.
    ntsa::Error registerSessionCallback(
        const ntci::StreamSocket::SessionCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Register the specified session 'callback' for this socket to be
    /// invoked on the specified 'strand'. Return the error.
    ntsa::Error registerSessionCallback(
        const ntci::StreamSocket::SessionCallback& callback,
        const bsl::shared_ptr<ntci::Strand>& strand) BSLS_KEYWORD_OVERRIDE;

    /// Deregister the current session or session callback for this socket.
    /// Return the error.
    ntsa::Error deregisterSession() BSLS_KEYWORD_OVERRIDE;

    /// Set the minimum number of bytes that must be available to send in order
    /// to attempt a zero-copy send to the specified 'value'. Return the error.
    ntsa::Error setZeroCopyThreshold(bsl::size_t value) BSLS_KEYWORD_OVERRIDE;

    /// Set the write rate limiter to the specified 'rateLimiter'. Return
    /// the error.
    ntsa::Error setWriteRateLimiter(const bsl::shared_ptr<ntci::RateLimiter>&
                                        rateLimiter) BSLS_KEYWORD_OVERRIDE;

    /// Set the write queue low watermark to the specified 'lowWatermark'.
    /// Return the error.
    ntsa::Error setWriteQueueLowWatermark(bsl::size_t lowWatermark)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the write queue high watermark to the specified 'highWatermark'.
    /// Return the error.
    ntsa::Error setWriteQueueHighWatermark(bsl::size_t highWatermark)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the write queue limits to the specified 'lowWatermark' and
    /// 'highWatermark'. Return the error.
    ntsa::Error setWriteQueueWatermarks(bsl::size_t lowWatermark,
                                        bsl::size_t highWatermark)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the read rate limiter to the specified 'rateLimiter'. Return
    /// the error.
    ntsa::Error setReadRateLimiter(const bsl::shared_ptr<ntci::RateLimiter>&
                                       rateLimiter) BSLS_KEYWORD_OVERRIDE;

    /// Set the read queue low watermark to the specified 'lowWatermark'.
    /// Return the error.
    ntsa::Error setReadQueueLowWatermark(bsl::size_t lowWatermark)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the read queue high watermark to the specified 'highWatermark'.
    /// Return the error.
    ntsa::Error setReadQueueHighWatermark(bsl::size_t highWatermark)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the read queue limits to the specified 'lowWatermark' and
    /// 'highWatermark'. Return the error.
    ntsa::Error setReadQueueWatermarks(bsl::size_t lowWatermark,
                                       bsl::size_t highWatermark)
        BSLS_KEYWORD_OVERRIDE;

    /// Request the implementation to start timestamping outgoing data if the
    /// specified 'enable' flag is true. Otherwise, request the implementation
    /// to stop timestamping outgoing data. Return the error.
    ntsa::Error timestampOutgoingData(bool enable) BSLS_KEYWORD_OVERRIDE;

    /// Request the implementation to start timestamping incoming data if the
    /// specified 'enable' flag is true. Otherwise, request the implementation
    /// to stop timestamping outgoing data. Return the error.
    ntsa::Error timestampIncomingData(bool enable) BSLS_KEYWORD_OVERRIDE;

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

    /// Cancel the upgrade operation identified by the specified 'token'.
    /// Return the error.
    ntsa::Error cancel(const ntca::UpgradeToken& token) BSLS_KEYWORD_OVERRIDE;

    /// Cancel the connect operation identified by the specified 'token'.
    /// Return the error.
    ntsa::Error cancel(const ntca::ConnectToken& token) BSLS_KEYWORD_OVERRIDE;

    /// Cancel the send operation identified by the specified 'token'.
    /// Return the error.
    ntsa::Error cancel(const ntca::SendToken& token) BSLS_KEYWORD_OVERRIDE;

    /// Cancel the receive operation identified by the specified 'token'.
    /// Return the error.
    ntsa::Error cancel(const ntca::ReceiveToken& token) BSLS_KEYWORD_OVERRIDE;

    /// Downgrade the stream socket from encrypted to unencrypted. Return
    /// the error.
    ntsa::Error downgrade() BSLS_KEYWORD_OVERRIDE;

    /// Shutdown the stream socket in the specified 'direction' according
    /// to the specified 'mode' of shutdown. Return the error.
    ntsa::Error shutdown(ntsa::ShutdownType::Value direction,
                         ntsa::ShutdownMode::Value mode) BSLS_KEYWORD_OVERRIDE;

    /// Close the stream socket.
    void close() BSLS_KEYWORD_OVERRIDE;

    /// Close the stream socket and invoke the specified 'callback' on
    /// the callback's strand, if any, when the stream socket is closed.
    /// Note that callbacks created by this object will automatically be
    /// invoked on this object's strand unless an explicit strand is
    /// specified at the time the callback is created.
    void close(const ntci::CloseFunction& callback) BSLS_KEYWORD_OVERRIDE;

    /// Close the stream socket and invoke the specified 'callback' on
    /// the callback's strand, if any, when the stream socket is closed.
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

    /// Return the transport of the datagram socket.
    ntsa::Transport::Value transport() const BSLS_KEYWORD_OVERRIDE;

    /// Return the source endpoint.
    ntsa::Endpoint sourceEndpoint() const BSLS_KEYWORD_OVERRIDE;

    /// Return the remote endpoint.
    ntsa::Endpoint remoteEndpoint() const BSLS_KEYWORD_OVERRIDE;

    /// Return the source certificate used by the encryption session, if
    /// any.
    bsl::shared_ptr<ntci::EncryptionCertificate> sourceCertificate() const
        BSLS_KEYWORD_OVERRIDE;

    /// Return the remote certificate used by the encryption session, if
    /// any.
    bsl::shared_ptr<ntci::EncryptionCertificate> remoteCertificate() const
        BSLS_KEYWORD_OVERRIDE;

    /// Return the private key used by the encryption session, if any.
    bsl::shared_ptr<ntci::EncryptionKey> privateKey() const
        BSLS_KEYWORD_OVERRIDE;

    /// Return the listener socket that accepted this socket, or null if
    /// the socket acted in the connecting role or if the socket was
    /// imported.
    bsl::shared_ptr<ntci::ListenerSocket> acceptor() const
        BSLS_KEYWORD_OVERRIDE;

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

    /// Return the current number of bytes queued after being copied from
    /// the socket receive buffer. These bytes have been received by
    /// the operating system buffers but not yet received by the user.
    bsl::size_t readQueueSize() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current read queue low watermark.
    bsl::size_t readQueueLowWatermark() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current read queue high watermark.
    bsl::size_t readQueueHighWatermark() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current number of bytes queued to be copied to the
    /// socket send buffer. These bytes have by sent by the user but not
    /// yet sent to the operating system.
    bsl::size_t writeQueueSize() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current write queue low watermark.
    bsl::size_t writeQueueLowWatermark() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current write queue high watermark.
    bsl::size_t writeQueueHighWatermark() const BSLS_KEYWORD_OVERRIDE;

    /// Return the total number of bytes copied to the socket send buffer.
    bsl::size_t totalBytesSent() const BSLS_KEYWORD_OVERRIDE;

    /// Return the total number of bytes copied from the socket receive
    /// buffer.
    bsl::size_t totalBytesReceived() const BSLS_KEYWORD_OVERRIDE;

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
