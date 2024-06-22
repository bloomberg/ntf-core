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

#ifndef INCLUDED_NTCR_DATAGRAMSOCKET
#define INCLUDED_NTCR_DATAGRAMSOCKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_datagramsocketoptions.h>
#include <ntccfg_platform.h>
#include <ntci_datagramsocket.h>
#include <ntci_datagramsocketmanager.h>
#include <ntci_datagramsocketsession.h>
#include <ntci_datapool.h>
#include <ntci_reactor.h>
#include <ntci_reactorpool.h>
#include <ntci_reactorsocket.h>
#include <ntci_resolver.h>
#include <ntci_strand.h>
#include <ntci_timer.h>
#include <ntcq_receive.h>
#include <ntcq_send.h>
#include <ntcq_zerocopy.h>
#include <ntcs_detachstate.h>
#include <ntcs_flowcontrolcontext.h>
#include <ntcs_flowcontrolstate.h>
#include <ntcs_metrics.h>
#include <ntcs_observer.h>
#include <ntcs_shutdowncontext.h>
#include <ntcs_shutdownstate.h>
#include <ntcscm_version.h>
#include <ntcu_timestampcorrelator.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_receivecontext.h>
#include <ntsa_receiveoptions.h>
#include <ntsa_sendcontext.h>
#include <ntsa_sendoptions.h>
#include <ntsi_descriptor.h>
#include <bdlb_nullablevalue.h>
#include <bsls_atomic.h>
#include <bsl_list.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcr {

/// @internal @brief
/// Provide an asynchronous, reactively-driven datagram socket.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcr
class DatagramSocket : public ntci::DatagramSocket,
                       public ntci::ReactorSocket,
                       public ntccfg::Shared<DatagramSocket>
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
    ntsa::Endpoint                               d_remoteEndpoint;
    bsl::shared_ptr<ntsi::DatagramSocket>        d_socket_sp;
    ntcs::Observer<ntci::Resolver>               d_resolver;
    ntcs::Observer<ntci::Reactor>                d_reactor;
    ntcs::Observer<ntci::ReactorPool>            d_reactorPool;
    bsl::shared_ptr<ntci::Strand>                d_reactorStrand_sp;
    bsl::shared_ptr<ntci::DatagramSocketManager> d_manager_sp;
    bsl::shared_ptr<ntci::Strand>                d_managerStrand_sp;
    bsl::shared_ptr<ntci::DatagramSocketSession> d_session_sp;
    bsl::shared_ptr<ntci::Strand>                d_sessionStrand_sp;
    bsl::shared_ptr<ntci::DataPool>              d_dataPool_sp;
    BlobBufferFactoryPtr                         d_incomingBufferFactory_sp;
    BlobBufferFactoryPtr                         d_outgoingBufferFactory_sp;
    bsl::shared_ptr<ntcs::Metrics>               d_metrics_sp;
    ntcs::FlowControlState                       d_flowControlState;
    ntcs::ShutdownState                          d_shutdownState;
    ntcq::ZeroCopyQueue                          d_zeroCopyQueue;
    bsl::size_t                                  d_zeroCopyThreshold;
    ntcq::SendQueue                              d_sendQueue;
    bsl::shared_ptr<ntci::RateLimiter>           d_sendRateLimiter_sp;
    bsl::shared_ptr<ntci::Timer>                 d_sendRateTimer_sp;
    bool                                         d_sendGreedily;
    ntci::SendCallback                           d_sendComplete;
    ntcq::SendCounter                            d_sendCounter;
    ntsa::ReceiveOptions                         d_receiveOptions;
    ntcq::ReceiveQueue                           d_receiveQueue;
    bsl::shared_ptr<ntci::RateLimiter>           d_receiveRateLimiter_sp;
    bsl::shared_ptr<ntci::Timer>                 d_receiveRateTimer_sp;
    bool                                         d_receiveGreedily;
    bsl::shared_ptr<bdlbb::Blob>                 d_receiveBlob_sp;
    bool                                         d_timestampOutgoingData;
    bool                                         d_timestampIncomingData;
    ntcu::TimestampCorrelator                    d_timestampCorrelator;
    bsl::uint32_t                                d_timestampCounter;
    bsl::size_t                                  d_maxDatagramSize;
    const bool                                   d_oneShot;
    ntcs::DetachState                            d_detachState;
    ntci::CloseCallback                          d_closeCallback;
    ntci::Executor::FunctorSequence              d_deferredCalls;
    bsl::size_t                                  d_totalBytesSent;
    bsl::size_t                                  d_totalBytesReceived;
    ntca::DatagramSocketOptions                  d_options;
    bslma::Allocator*                            d_allocator_p;

  private:
    DatagramSocket(const DatagramSocket&) BSLS_KEYWORD_DELETED;
    DatagramSocket& operator=(const DatagramSocket&) BSLS_KEYWORD_DELETED;

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

    /// Process the readability of the socket by performing one read
    /// iteration. The behavior is undefined unless 'd_mutex' is locked.
    ntsa::Error privateSocketReadableIteration(
        const bsl::shared_ptr<DatagramSocket>& self);

    /// Process the writability of the socket by performing one write
    /// iteration. The behavior is undefined unless 'd_mutex' is locked.
    ntsa::Error privateSocketWritableIteration(
        const bsl::shared_ptr<DatagramSocket>& self);

    /// Indicate a failure has occurred and detach the socket from its
    /// monitor. The behavior is undefined unless 'd_mutex' is locked.
    void privateFail(const bsl::shared_ptr<DatagramSocket>& self,
                     const ntsa::Error&                     error);

    /// Indicate a failure has occurred and detach the socket from its
    /// monitor.
    void privateFail(const bsl::shared_ptr<DatagramSocket>& self,
                     const ntca::ErrorEvent&                event);

    /// Shutdown the stream socket in the specified 'direction' according
    /// to the specified 'mode' of shutdown. Return the error. The behavior
    /// is undefined unless 'd_mutex' is locked.
    ntsa::Error privateShutdown(const bsl::shared_ptr<DatagramSocket>& self,
                                ntsa::ShutdownType::Value direction,
                                ntsa::ShutdownMode::Value mode,
                                bool                      defer);

    /// Process the advancement through the shutdown sequence by trying to
    /// shut down the socket for sending. The behavior is undefined unless
    /// 'd_mutex' is locked.
    void privateShutdownSend(const bsl::shared_ptr<DatagramSocket>& self,
                             bool                                   defer);

    /// Process the advancement through the shutdown sequence by trying to
    /// shut down the socket for sending. The behavior is undefined unless
    /// 'd_mutex' is locked.
    void privateShutdownReceive(const bsl::shared_ptr<DatagramSocket>& self,
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
    /// completion of the shutdown sequence. The behavior is undefined
    /// unless 'd_mutex' is locked.
    /// If it is required to detach the socket from the proactor then part of
    /// described functionality will be executed asynchronously using the next
    /// method.
    void privateShutdownSequence(const bsl::shared_ptr<DatagramSocket>& self,
                                 ntsa::ShutdownOrigin::Value            origin,
                                 const ntcs::ShutdownContext& context,
                                 bool                         defer);

    /// Execute the second part of shutdown sequence when the socket is
    /// detached. See also "privateShutdownSequence".
    void privateShutdownSequenceComplete(
        const bsl::shared_ptr<DatagramSocket>& self,
        const ntcs::ShutdownContext&           context,
        bool                                   defer,
        bool                                   lock);

    /// Enable copying from the socket buffers in the specified 'direction'.
    /// The behavior is undefined unless 'd_mutex' is locked.
    ntsa::Error privateRelaxFlowControl(
        const bsl::shared_ptr<DatagramSocket>& self,
        ntca::FlowControlType::Value           direction,
        bool                                   defer,
        bool                                   unlock);

    /// Disable copying from socket buffers in the specified 'direction'
    /// according to the specified 'mode'. The behavior is undefined unless
    /// 'd_mutex' is locked.
    ntsa::Error privateApplyFlowControl(
        const bsl::shared_ptr<DatagramSocket>& self,
        ntca::FlowControlType::Value           direction,
        ntca::FlowControlMode::Value           mode,
        bool                                   defer,
        bool                                   lock);

    /// Disable copying from socket buffers in both directions and detach
    /// the socket from the reactor. Return true if asynchronous detachment
    /// started, otherwise return false
    bool privateCloseFlowControl(
        const bsl::shared_ptr<DatagramSocket>& self,
        bool                                   defer,
        const ntci::SocketDetachedCallback&    detachCallback);

    /// Test if rate limiting is applied to copying to the send buffer, and
    /// if so, determine whether more data is allowed to be copied to the
    /// send buffer at this time. If not, apply flow control in the send
    /// direction and schedule a timer to relax flow control in the send
    /// direction at the estimated time that more data might be able to be
    /// sent. The behavior is undefined unless 'd_mutex' is locked.
    ntsa::Error privateThrottleSendBuffer(
        const bsl::shared_ptr<DatagramSocket>& self);

    /// Test if rate limiting is applied to copying from the receive buffer,
    /// and if so, determine whether more data is allowed to be copied from
    /// the receive buffer at this time. If not, apply flow control in the
    /// receive direction and schedule a timer to relax flow control in
    /// the receive direction at the estimated time that more data might be
    /// able to be received. The behavior is undefined unless 'd_mutex' is
    /// locked.
    ntsa::Error privateThrottleReceiveBuffer(
        const bsl::shared_ptr<DatagramSocket>& self);

    /// Enqueue a message to the specified 'endpoint' having the specified
    /// 'data' to the socket send buffer. Return the error. The behavior is
    /// undefined unless 'd_mutex' is locked.
    ntsa::Error privateEnqueueSendBuffer(
        const bsl::shared_ptr<DatagramSocket>&     self,
        ntsa::SendContext*                         context,
        const bdlb::NullableValue<ntsa::Endpoint>& endpoint,
        const bdlbb::Blob&                         data);

    /// Enqueue a message to the specified 'endpoint' having the specified
    /// 'data' to the socket send buffer. Return the error. The behavior is
    /// undefined unless 'd_mutex' is locked.
    ntsa::Error privateEnqueueSendBuffer(
        const bsl::shared_ptr<DatagramSocket>&     self,
        ntsa::SendContext*                         context,
        const bdlb::NullableValue<ntsa::Endpoint>& endpoint,
        const ntsa::Data&                          data);

    /// Dequeue a message from the socket receive buffer. Append to the
    /// specified 'data' the data dequeued and load into the specified
    /// 'endpoint' the endpoint of the sender of the data. Return the error.
    /// The behavior is undefined unless 'd_mutex' is locked.
    ntsa::Error privateDequeueReceiveBuffer(
        const bsl::shared_ptr<DatagramSocket>& self,
        bdlb::NullableValue<ntsa::Endpoint>*   endpoint,
        bdlbb::Blob*                           data);

    /// Allocate a new blob assigned to 'd_receiveBlob_sp', if necessary
    /// and allocate sufficient capacity buffers to store the maximum
    /// datagram size. The behavior is undefined unless 'd_mutex' is locked.
    void privateAllocateReceiveBlob();

    /// Rearm the interest in the writability of the socket in the reactor,
    /// if necessary. The behavior is undefined unless 'd_mutex' is locked.
    void privateRearmAfterSend(const bsl::shared_ptr<DatagramSocket>& self);

    /// Rearm the interest in the readability of the socket in the reactor,
    /// if necessary. The behavior is undefined unless 'd_mutex' is locked.
    void privateRearmAfterReceive(const bsl::shared_ptr<DatagramSocket>& self);

    void privateRearmAfterNotification(
        const bsl::shared_ptr<DatagramSocket>& self);

    /// Open the datagram socket. Return the error.
    ntsa::Error privateOpen(const bsl::shared_ptr<DatagramSocket>& self);

    /// Open the datagram socket using the specified 'transport'. Return the
    /// error.
    ntsa::Error privateOpen(const bsl::shared_ptr<DatagramSocket>& self,
                            ntsa::Transport::Value                 transport);

    /// Open the datagram socket using the transport associated with the
    /// specified 'endpoint'. Return the error.
    ntsa::Error privateOpen(const bsl::shared_ptr<DatagramSocket>& self,
                            const ntsa::Endpoint&                  endpoint);

    /// Open the datagram socket using the specified 'transport' imported
    /// from the specified 'handle'. Return the error.
    ntsa::Error privateOpen(const bsl::shared_ptr<DatagramSocket>& self,
                            ntsa::Transport::Value                 transport,
                            ntsa::Handle                           handle);

    /// Open the datagram socket using the specified 'transport' imported
    /// from the specified 'handle'. Return the error.
    ntsa::Error privateOpen(
        const bsl::shared_ptr<DatagramSocket>&       self,
        ntsa::Transport::Value                       transport,
        const bsl::shared_ptr<ntsi::DatagramSocket>& datagramSocket);

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
    /// if resolution was successful. Invoke the specified 'connectCallback'
    /// according to the specified 'connectOptions'.
    void processRemoteEndpointResolution(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const ntsa::Endpoint&                  endpoint,
        const ntca::GetEndpointEvent&          getEndpointEvent,
        const ntca::ConnectOptions&            connectOptions,
        const ntci::ConnectCallback&           connectCallback);

    /// Enable or disable timestamping of outgoing data according to the
    /// specified 'enable' flag. Return the error.
    ntsa::Error privateTimestampOutgoingData(
        const bsl::shared_ptr<DatagramSocket>& self,
        bool                                   enable);

    /// Enable or disable timestamping of incoming data according to the
    /// specified 'enable' flag. Return the error.
    ntsa::Error privateTimestampIncomingData(
        const bsl::shared_ptr<DatagramSocket>& self,
        bool                                   enable);

    /// Process the detection of the specified outgoing data 'timestamp'.
    void privateTimestampUpdate(const bsl::shared_ptr<DatagramSocket>& self,
                                const ntsa::Timestamp& timestamp);

    // Engage zero-copy transmissions for data whose size is greater than or
    // equal to the specified 'threshold', in bytes. Return the error.
    ntsa::Error privateZeroCopyEngage(
        const bsl::shared_ptr<DatagramSocket>& self,
        bsl::size_t                            threshold);

    // Process the completion of one or more zero-copy transmissions described
    // by the specified 'zeroCopy' notification.
    void privateZeroCopyUpdate(const bsl::shared_ptr<DatagramSocket>& self,
                               const ntsa::ZeroCopy& zeroCopy);

  public:
    /// Create a new, initially uninitilialized datagram socket. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Note that the 'create' function must be subsequently called
    /// before using this object.
    DatagramSocket(const ntca::DatagramSocketOptions&        options,
                   const bsl::shared_ptr<ntci::Resolver>&    resolver,
                   const bsl::shared_ptr<ntci::Reactor>&     reactor,
                   const bsl::shared_ptr<ntci::ReactorPool>& reactorPool,
                   const bsl::shared_ptr<ntcs::Metrics>&     metrics,
                   bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~DatagramSocket() BSLS_KEYWORD_OVERRIDE;

    /// Open the datagram socket. Return the error.
    ntsa::Error open() BSLS_KEYWORD_OVERRIDE;

    /// Open the datagram socket using the specified 'transport'. Return the
    /// error.
    ntsa::Error open(ntsa::Transport::Value transport) BSLS_KEYWORD_OVERRIDE;

    /// Open the datagram socket using the specified 'transport' imported
    /// from the specified 'handle'. Return the error.
    ntsa::Error open(ntsa::Transport::Value transport,
                     ntsa::Handle           handle) BSLS_KEYWORD_OVERRIDE;

    /// Open the stream socket using the specified 'transport' imported from
    /// the specified 'datagramSocket'. Return the error.
    ntsa::Error open(ntsa::Transport::Value transport,
                     const bsl::shared_ptr<ntsi::DatagramSocket>&
                         datagramSocket) BSLS_KEYWORD_OVERRIDE;

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
    /// queue high watermark event. Return the error, notably
    /// 'ntsa::Error::e_WOULD_BLOCK' if neither the read queue nor the
    /// socket receive buffer has sufficient size to synchronously satisfy
    /// the read operation, or 'ntsa::Error::e_EOF' if the read queue is
    /// empty and the socket receive buffer has been shut down. All other
    /// errors indicate no more received data is available at this time or
    /// will become available in the future. Note that callbacks created by
    /// this object will automatically be invoked on this object's strand
    /// unless an explicit strand is specified at the time the callback is
    /// created.
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
    /// queue high watermark event. Return the error, notably
    /// 'ntsa::Error::e_WOULD_BLOCK' if neither the read queue nor the
    /// socket receive buffer has sufficient size to synchronously satisfy
    /// the read operation, or 'ntsa::Error::e_EOF' if the read queue is
    /// empty and the socket receive buffer has been shut down. All other
    /// errors indicate no more received data is available at this time or
    /// will become available in the future. Note that callbacks created by
    /// this object will automatically be invoked on this object's strand
    /// unless an explicit strand is specified at the time the callback is
    /// created.
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
        const bsl::shared_ptr<ntci::DatagramSocketManager>& manager)
        BSLS_KEYWORD_OVERRIDE;

    /// Deregister the current manager or manager callback for this socket.
    /// Return the error.
    ntsa::Error deregisterManager() BSLS_KEYWORD_OVERRIDE;

    /// Register the specified 'session' for this socket. Return the
    /// error.
    ntsa::Error registerSession(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session)
        BSLS_KEYWORD_OVERRIDE;

    /// Register the specified session 'callback' for this socket to be
    /// invoked on this socket's strand. Return the error.
    ntsa::Error registerSessionCallback(
        const ntci::DatagramSocket::SessionCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Register the specified session 'callback' for this socket to be
    /// invoked on the specified 'strand'. Return the error.
    ntsa::Error registerSessionCallback(
        const ntci::DatagramSocket::SessionCallback& callback,
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

    /// Set the flag that indicates multicast datagrams should be looped
    /// back to the local host to the specified 'value'. Return the error.
    ntsa::Error setMulticastLoopback(bool value) BSLS_KEYWORD_OVERRIDE;

    /// Set the maximum number of hops over which multicast datagrams
    /// should be forwarded to the specified 'value'. Return the error.
    ntsa::Error setMulticastTimeToLive(bsl::size_t value)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the address of the network interface on which multicast
    /// datagrams should be sent to the specified 'value'. Return the error.
    ntsa::Error setMulticastInterface(const ntsa::IpAddress& value)
        BSLS_KEYWORD_OVERRIDE;

    /// Issue an IGMP message to add this datagram socket to the
    /// specified multicast 'group' on the adapter identified by the
    /// specified 'interface'. Return the error.
    ntsa::Error joinMulticastGroup(const ntsa::IpAddress& interface,
                                   const ntsa::IpAddress& group)
        BSLS_KEYWORD_OVERRIDE;

    /// Issue an IGMP message to remove this datagram socket from the
    /// specified multicast 'group' on the adapter identified by the
    /// specified 'interface'. Return the error.
    ntsa::Error leaveMulticastGroup(const ntsa::IpAddress& interface,
                                    const ntsa::IpAddress& group)
        BSLS_KEYWORD_OVERRIDE;

    /// Request the implementation to start timestamping outgoing data if the
    /// specified 'enable' flag is true. Otherwise, request the implementation
    /// to stop timestamping outgoing data. Return true if operation was
    /// successful (though it does not guarantee that transmit timestamps would
    /// be generated). Otherwise return false.
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

    /// Cancel the connect operation identified by the specified 'token'.
    /// Return the error.
    ntsa::Error cancel(const ntca::ConnectToken& token) BSLS_KEYWORD_OVERRIDE;

    /// Cancel the send operation identified by the specified 'token'.
    /// Return the error.
    ntsa::Error cancel(const ntca::SendToken& token) BSLS_KEYWORD_OVERRIDE;

    /// Cancel the receive operation identified by the specified 'token'.
    /// Return the error.
    ntsa::Error cancel(const ntca::ReceiveToken& token) BSLS_KEYWORD_OVERRIDE;

    /// Shutdown the datagram socket in the specified 'direction' according
    /// to the specified 'mode' of shutdown. Return the error.
    ntsa::Error shutdown(ntsa::ShutdownType::Value direction,
                         ntsa::ShutdownMode::Value mode) BSLS_KEYWORD_OVERRIDE;

    /// Close the datagram socket.
    void close() BSLS_KEYWORD_OVERRIDE;

    /// Close the datagram socket and invoke the specified 'callback' on
    /// the callback's strand, if any, when the datagram socket is closed.
    /// Note that callbacks created by this object will automatically be
    /// invoked on this object's strand unless an explicit strand is
    /// specified at the time the callback is created.
    void close(const ntci::CloseFunction& callback) BSLS_KEYWORD_OVERRIDE;

    /// Close the datagram socket and invoke the specified 'callback' on
    /// the callback's strand, if any, when the datagram socket is closed.
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

    /// Return the transport of the datagram socket.
    ntsa::Transport::Value transport() const BSLS_KEYWORD_OVERRIDE;

    /// Return the source endpoint.
    ntsa::Endpoint sourceEndpoint() const BSLS_KEYWORD_OVERRIDE;

    /// Return the remote endpoint.
    ntsa::Endpoint remoteEndpoint() const BSLS_KEYWORD_OVERRIDE;

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
