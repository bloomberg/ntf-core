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

#ifndef INCLUDED_NTCF_TESTSERVER
#define INCLUDED_NTCF_TESTSERVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntcf_testmessage.h>
#include <ntcf_testvocabulary.h>

namespace BloombergLP {
namespace ntcf {

/// Provide a test server transaction.
///
/// @par Thread Safety
/// This class is thread safe.
class TestServerTransaction : public ntccfg::Shared<TestServerTransaction>
{
    /// Defines a type alias for a mutex.
    typedef ntci::Mutex Mutex;

    /// Defines a type alias for a mutex lock guard.
    typedef ntci::LockGuard LockGuard;

    Mutex                                        d_mutex;
    bsl::shared_ptr<ntcf::TestMessage>           d_request_sp;
    bsl::shared_ptr<ntcf::TestMessagePool>       d_responsePool_sp;
    bsl::shared_ptr<ntci::DataPool>              d_dataPool_sp;
    bsl::shared_ptr<ntci::DatagramSocket>        d_datagramSocket_sp;
    bsl::shared_ptr<ntci::StreamSocket>          d_streamSocket_sp;
    bsl::shared_ptr<ntci::Serialization>         d_serialization_sp;
    bsl::shared_ptr<ntci::Compression>           d_compression_sp;
    bsl::shared_ptr<ntcf::TestMessageEncryption> d_encryption_sp;
    bdlb::NullableValue<ntsa::Endpoint>          d_endpoint;
    bsls::TimeInterval                           d_timestamp;
    bslma::Allocator*                            d_allocator_p;

    /// Return a new response.
    bsl::shared_ptr<ntcf::TestMessage> createResponse();

    /// Deliver the specified 'response' to the sender.
    void deliverResponse(const bsl::shared_ptr<ntcf::TestMessage>& response);

    /// Process the specified upgrade 'event' from the specified 'upgradable'
    /// socket.
    void processUpgrade(const bsl::shared_ptr<ntci::Upgradable>& upgradable,
                        const ntca::UpgradeEvent&                event,
                        bool                                     acknowledge,
                        ntcf::TestControlTransition::Value       transition);

    BALL_LOG_SET_CLASS_CATEGORY("NTCF.TEST.SERVER");

  public:
    /// Create a new server transaction. Create responses from the specified
    /// 'responsePool'. Create buffers from the specified 'dataPool'. Serialize
    /// structures using the specified 'serialization'. Compress payloads using
    /// the specified 'compression'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    TestServerTransaction(
        const bsl::shared_ptr<ntcf::TestMessagePool>&       responsePool,
        const bsl::shared_ptr<ntci::DataPool>&              dataPool,
        const bsl::shared_ptr<ntci::Serialization>&         serialization,
        const bsl::shared_ptr<ntci::Compression>&           compression,
        const bsl::shared_ptr<ntcf::TestMessageEncryption>& encryption,
        bslma::Allocator* basicAllocator = 0);

    /// Reset the transaction.
    void reset();

    /// Start the transaction to process the specified 'request' received at
    /// the specified 'timestamp'. Send any responses through the specified
    /// 'sender'.
    void start(const bsl::shared_ptr<ntcf::TestMessage>&  request,
               const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
               const bsls::TimeInterval&                  timestamp);

    /// Start the transaction to process the specified 'request' received at
    /// the specified 'timestamp'. Send any responses to the specified
    /// 'endpoint' through the specified 'sender'.
    void start(const bsl::shared_ptr<ntcf::TestMessage>&    request,
               const bsl::shared_ptr<ntci::DatagramSocket>& sender,
               const bsls::TimeInterval&                    timestamp,
               const ntsa::Endpoint&                        endpoint);

    /// Complete the transaction with an acknowledgement.
    void acknowledge();

    /// Complete the transaction with the specified 'acknowledgement'.
    void acknowledge(const ntcf::TestAcknowledgment& acknowledgment);

    /// Complete the transaction with the specified 'trade'.
    void complete(const ntcf::TestTrade& trade);

    /// Complete the transaction with the specified 'echo'.
    void complete(const ntcf::TestEcho& echo);

    /// Complete the transaction with the specified 'fault'.
    void fail(const ntcf::TestFault& fault);

    /// Enable compression on the underlying socket using the specified
    /// 'compression' mechanism.
    void enableCompression(bool                               acknowledge,
                           ntcf::TestControlTransition::Value transition);

    /// Enable compression on the underlying socket using the specified
    /// 'encryption' mechanism.
    void enableEncryption(bool                               acknowledge,
                          ntcf::TestControlTransition::Value transition);

    /// Disable compression on the underlying socket.
    void disableCompression(bool                               acknowledge,
                            ntcf::TestControlTransition::Value transition);

    /// Disable encryption on the underlying socket.
    void disableEncryption(bool                               acknowledge,
                           ntcf::TestControlTransition::Value transition);

    /// Close the transaction.
    void close();
};

/// Provide a test server.
///
/// @par Thread Safety
/// This class is thread safe.
class TestServer : public ntci::DatagramSocketSession,
                   public ntci::DatagramSocketManager,
                   public ntci::StreamSocketSession,
                   public ntci::ListenerSocketSession,
                   public ntci::ListenerSocketManager,
                   public ntccfg::Shared<TestServer>
{
    /// This type defines a map of stream sockets to their message parsers.
    typedef bsl::map<bsl::shared_ptr<ntci::StreamSocket>,
                     bsl::shared_ptr<ntcf::TestMessageParser> >
        StreamSocketMap;

    /// This type defines a type alias for this type.
    typedef TestServer Self;

    /// Defines a type alias for a mutex.
    typedef ntci::Mutex Mutex;

    /// Defines a type alias for a mutex lock guard.
    typedef ntci::LockGuard LockGuard;

    Mutex                                        d_mutex;
    bsl::shared_ptr<ntci::DataPool>              d_dataPool_sp;
    bsl::shared_ptr<ntcf::TestMessagePool>       d_messagePool_sp;
    bsl::shared_ptr<ntci::Serialization>         d_serialization_sp;
    bsl::shared_ptr<ntci::Compression>           d_compression_sp;
    bsl::shared_ptr<ntci::Scheduler>             d_scheduler_sp;
    bsl::shared_ptr<ntci::DatagramSocket>        d_datagramSocket_sp;
    bsl::shared_ptr<ntcf::TestMessageParser>     d_datagramParser_sp;
    ntsa::Endpoint                               d_datagramEndpoint;
    bsl::shared_ptr<ntci::ListenerSocket>        d_listenerSocket_sp;
    ntsa::Endpoint                               d_listenerEndpoint;
    StreamSocketMap                              d_streamSocketMap;
    bsl::shared_ptr<ntcf::TestMessageEncryption> d_encryption_sp;
    bsls::AtomicBool                             d_closed;
    ntcf::TestServerConfig                       d_config;
    bslma::Allocator*                            d_allocator_p;

    BALL_LOG_SET_CLASS_CATEGORY("NTCF.TEST.SERVER");

  private:
    TestServer(const TestServer&) BSLS_KEYWORD_DELETED;
    TestServer& operator=(const TestServer&) BSLS_KEYWORD_DELETED;

  private:
    /// Process the establishment of the specified 'datagramSocket'. Return
    /// the application protocol of the 'datagramSocket'.
    void processDatagramSocketEstablished(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
        BSLS_KEYWORD_OVERRIDE;

    /// Process the closure of the specified 'datagramSocket'.
    void processDatagramSocketClosed(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
        BSLS_KEYWORD_OVERRIDE;

    /// Process the establishment of the specified 'streamSocket'. Return
    /// the application protocol of the 'streamSocket'.
    void processStreamSocketEstablished(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
        BSLS_KEYWORD_OVERRIDE;

    /// Process the closure of the specified 'streamSocket'.
    void processStreamSocketClosed(const bsl::shared_ptr<ntci::StreamSocket>&
                                       streamSocket) BSLS_KEYWORD_OVERRIDE;

    /// Process the establishment of the specified 'listenerSocket'. Return
    /// the application protocol of the 'listenerSocket'.
    void processListenerSocketEstablished(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
        BSLS_KEYWORD_OVERRIDE;

    /// Process the closure of the specified 'listenerSocket'.
    void processListenerSocketClosed(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
        BSLS_KEYWORD_OVERRIDE;

    // MRM: ntci::DatagramSocketSession

    /// Process the condition that read queue flow control has been relaxed:
    /// the socket receive buffer is being automatically copied to the read
    /// queue.
    void processReadQueueFlowControlRelaxed(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that read queue flow control has been applied:
    /// the socket receive buffer is not being automatically copied to the
    /// read queue.
    void processReadQueueFlowControlApplied(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the read queue is greater
    /// than or equal to the read queue low watermark.
    void processReadQueueLowWatermark(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the read queue is greater
    /// than the read queue high watermark.
    void processReadQueueHighWatermark(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the read queue has been discarded
    /// because a non-transient read error asynchronously occured.
    void processReadQueueDiscarded(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that write queue flow control has been
    /// relaxed: the write queue is being automatically copied to the socket
    /// send buffer.
    void processWriteQueueFlowControlRelaxed(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that write queue flow control has been
    /// applied: the write queue is not being automatically copied to the
    /// socket send buffer.
    void processWriteQueueFlowControlApplied(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue has been
    /// drained down to less than or equal to the write queue low watermark.
    /// This condition will not occur until the write queue high watermark
    /// condition occurs. The write queue low watermark conditions and the
    /// high watermark conditions are guaranteed to occur serially.
    void processWriteQueueLowWatermark(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue is greater
    /// than the write queue high watermark. This condition will occur the
    /// first time the write queue high watermark has been reached but
    /// then will not subsequently ooccur until the write queue low
    /// watermark. The write queue low watermark conditions and the
    /// high watermark conditions are guaranteed to occur serially.
    void processWriteQueueHighWatermark(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the write queue has been discarded
    /// because a non-transient write error asynchronously occured.
    void processWriteQueueDiscarded(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the initiation of the shutdown sequence from the specified
    /// 'origin'.
    void processShutdownInitiated(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for reading.
    void processShutdownReceive(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for writing.
    void processShutdownSend(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the completion of the shutdown sequence.
    void processShutdownComplete(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process an error.
    void processError(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ErrorEvent& event) BSLS_KEYWORD_OVERRIDE;

    // MRM: ntci::StreamSocketSession

    /// Process the condition that read queue flow control has been relaxed:
    /// the socket receive buffer is being automatically copied to the read
    /// queue.
    void processReadQueueFlowControlRelaxed(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that read queue flow control has been applied:
    /// the socket receive buffer is not being automatically copied to the
    /// read queue.
    void processReadQueueFlowControlApplied(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the read queue is greater
    /// than or equal to the read queue low watermark.
    void processReadQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the read queue is greater
    /// than the read queue high watermark.
    void processReadQueueHighWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the read queue has been discarded
    /// because a non-transient read error asynchronously occured.
    void processReadQueueDiscarded(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that write queue flow control has been
    /// relaxed: the write queue is being automatically copied to the socket
    /// send buffer.
    void processWriteQueueFlowControlRelaxed(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that write queue flow control has been
    /// applied: the write queue is not being automatically copied to the
    /// socket send buffer.
    void processWriteQueueFlowControlApplied(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue has been
    /// drained down to less than or equal to the write queue low watermark.
    /// This condition will not occur until the write queue high watermark
    /// condition occurs. The write queue low watermark conditions and the
    /// high watermark conditions are guaranteed to occur serially.
    void processWriteQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue is greater
    /// than the write queue high watermark. This condition will occur the
    /// first time the write queue high watermark has been reached but
    /// then will not subsequently ooccur until the write queue low
    /// watermark. The write queue low watermark conditions and the
    /// high watermark conditions are guaranteed to occur serially.
    void processWriteQueueHighWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the write queue has been discarded
    /// because a non-transient write error asynchronously occured.
    void processWriteQueueDiscarded(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the initiation of a downgrade from encrypted to unencrypted
    /// communication.
    void processDowngradeInitiated(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::DowngradeEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the completion of a downgrade from encrypted to unencrypted
    /// communication.
    void processDowngradeComplete(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::DowngradeEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the initiation of the shutdown sequence from the specified
    /// 'origin'.
    void processShutdownInitiated(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for reading.
    void processShutdownReceive(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for writing.
    void processShutdownSend(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the completion of the shutdown sequence.
    void processShutdownComplete(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process an error.
    void processError(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                      const ntca::ErrorEvent& event) BSLS_KEYWORD_OVERRIDE;

    // MRM: ntci::ListenerSocketSession

    /// Process the condition that accept queue flow control has been
    /// relaxed: the connections in the backlog are now being automatically
    /// accepted from the operating system and enqueued to the accept queue.
    void processAcceptQueueFlowControlRelaxed(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that accept queue flow control has been
    /// applied: the connections in the backlog are not being automatically
    /// accepted from the operating system and enqueued to the accept queue.
    void processAcceptQueueFlowControlApplied(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the accept queue is greater
    /// than or equal to the accept queue low watermark.
    void processAcceptQueueLowWatermark(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the accept queue is greater
    /// than the accept queue high watermark.
    void processAcceptQueueHighWatermark(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the accept queue has been discarded
    /// because a non-transient accept error asynchronously occured.
    void processAcceptQueueDiscarded(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the initiation of the shutdown sequence from the specified
    /// 'origin'.
    void processShutdownInitiated(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for reading.
    void processShutdownReceive(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for writing.
    void processShutdownSend(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the completion of the shutdown sequence.
    void processShutdownComplete(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process an error.
    void processError(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ErrorEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the upgrade of the specified 'streamSocket' that is the
    /// specified 'upgradable' object used by the specified 'clientSocket'
    /// according to the specified 'event'.
    void processStreamSocketUpgradeEvent(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::UpgradeEvent&                  event);

    /// Wait for the TLS handshake to begin upgrading the connection to the
    /// authority to be authenticated and secured.
    void privateStreamSocketInitiateUpgrade(
        const bsl::shared_ptr<Self>&               self,
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket);

    /// Process completion or failure of the upgrade of the connection to the
    /// authority to be authenticated and secured. according to the specified
    /// 'event'.
    void privateStreamSocketCompleteUpgrade(
        const bsl::shared_ptr<Self>&               self,
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::UpgradeEvent&                  event);

    /// Initiate the TLS shutdown sequence to begin downgrading the
    /// communication so it is no longer authenticated nor encrypted.
    void privateStreamSocketInitiateDowngrade(
        const bsl::shared_ptr<Self>&               self,
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket);

    /// Process completion or failure of the downgrade of the communication so
    /// it is no longer authenticated nor encrypted according to the specified
    /// 'event'.
    void privateStreamSocketCompleteDowngrade(
        const bsl::shared_ptr<Self>&               self,
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::DowngradeEvent&                event);

    /// Process the conclusion of phases required by the protocol after the
    /// transport-level connection for the specified 'datagramSocket' is
    /// established.
    void privateDatagramSocketUp(
        const bsl::shared_ptr<Self>&                 self,
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket);

    /// Process the termination of the trensport-level connection for the
    /// specified 'datagramSocket'.
    void privateDatagramSocketDown(
        const bsl::shared_ptr<Self>&                 self,
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket);

    /// Process the failure of the specified 'datagramSocket'.
    void privateDatagramSocketError(
        const bsl::shared_ptr<Self>&                 self,
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntsa::Error&                           error);

    /// Process the conclusion of phases required by the protocol after the
    /// transport-level connection for the specified 'socket' is established.
    void privateStreamSocketUp(
        const bsl::shared_ptr<Self>&               self,
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket);

    /// Process the termination of the trensport-level connection for the
    /// specified 'socket'.
    void privateStreamSocketDown(
        const bsl::shared_ptr<Self>&               self,
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket);

    /// Process the failure of the specified 'streamSocket'.
    void privateStreamSocketError(
        const bsl::shared_ptr<Self>&               self,
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntsa::Error&                         error);

    /// Process the establishment of the specified 'listenerSocket'.
    void privateListenerSocketUp(
        const bsl::shared_ptr<Self>&                 self,
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket);

    /// Process the termination of the specified 'listeningSocket'.
    void privateListenerSocketDown(
        const bsl::shared_ptr<Self>&                 self,
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket);

    /// Process the failure of the specified 'listeningSocket'.
    void privateListenerSocketError(
        const bsl::shared_ptr<Self>&                 self,
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntsa::Error&                           error);

    /// Dispatch the specified 'transaction' to an appropriate processor.
    void dispatchMessage(
        const bsl::shared_ptr<ntcf::TestServerTransaction>& transaction,
        const bsl::shared_ptr<ntcf::TestMessage>&           request);

    /// Process the specified 'bid' in the specified 'transaction'.
    void processBid(
        const bsl::shared_ptr<ntcf::TestServerTransaction>& transaction,
        const ntcf::TestBid&                                bid);

    /// Process the specified 'ask' in the specified 'transaction'.
    void processAsk(
        const bsl::shared_ptr<ntcf::TestServerTransaction>& transaction,
        const ntcf::TestAsk&                                ask);

    /// Process the specified 'subscription' in the specified 'transaction'.
    void processSubscription(
        const bsl::shared_ptr<ntcf::TestServerTransaction>& transaction,
        const ntcf::TestSubscription&                       subscription);

    /// Process the specified 'signal' control request in the specified
    /// 'transaction'.
    void processSignal(
        const bsl::shared_ptr<ntcf::TestServerTransaction>& transaction,
        const ntcf::TestSignal&                             signal);

    /// Process the specified 'encryption' control request in the specified
    /// 'transaction'.
    void processEncryption(
        const bsl::shared_ptr<ntcf::TestServerTransaction>& transaction,
        const ntcf::TestControlEncryption&                  encryption);

    /// Process the specified 'compression' control request in the specified
    /// 'transaction'.
    void processCompression(
        const bsl::shared_ptr<ntcf::TestServerTransaction>& transaction,
        const ntcf::TestControlCompression&                 compression);

    /// Process the specified 'heartbeat' control request in the specified
    /// 'transaction'.
    void processHeartbeat(
        const bsl::shared_ptr<ntcf::TestServerTransaction>& transaction,
        const ntcf::TestControlHeartbeat&                   heartbeat);

  public:
    /// Create a new server with the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    TestServer(const ntcf::TestServerConfig&           configuration,
               const bsl::shared_ptr<ntci::Scheduler>& scheduler,
               const bsl::shared_ptr<ntci::DataPool>&  dataPool,
               const bsl::shared_ptr<ntcf::TestMessageEncryption>& encryption,
               bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~TestServer() BSLS_KEYWORD_OVERRIDE;

    /// Set the server accept queue low watermark to the specified 'value'.
    void setAcceptQueueLowWatermark(bsl::size_t value);

    /// Set the server accept queue high watermark to the specified 'value'.
    void setAcceptQueueHighWatermark(bsl::size_t value);

    /// Start monitoring the listener socket backlog.
    void relaxFlowControl();

    /// Stop monitoring the listener socket backlog.
    void applyFlowControl();

    /// Close the server.
    void close();

    /// Return the TCP endpoint.
    const ntsa::Endpoint& tcpEndpoint() const;

    /// Return the UDP endpoint.
    const ntsa::Endpoint& udpEndpoint() const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(TestServer);
};

/// Defines a type alias for shared pointer to a test server.
typedef bsl::shared_ptr<ntcf::TestServer> TestServerPtr;

/// Defines a type alias for vector of shared pointers to test servers.
typedef bsl::vector<ntcf::TestServerPtr> TestServerVector;

}  // end namespace ntcf
}  // end namespace BloombergLP
#endif
