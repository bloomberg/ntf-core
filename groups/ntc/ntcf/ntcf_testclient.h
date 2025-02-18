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

#ifndef INCLUDED_NTCF_TESTCLIENT
#define INCLUDED_NTCF_TESTCLIENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntcf_testframework.h>
#include <ntcf_testvocabulary.h>

namespace BloombergLP {
namespace ntcf {

/// Provide a test client transaction.
///
/// @par Thread Safety
/// This class is thread safe.
class TestClientTransaction
{

};

/// Provide a test client.
///
/// @par Thread Safety
/// This class is thread safe.
class TestClient : public ntci::DatagramSocketSession, 
                   public ntci::DatagramSocketManager,
                   public ntci::StreamSocketSession,
                   public ntci::StreamSocketManager, 
                   public ntcf::TestTradeCallbackFactory,
                   public ntcf::TestAcknowledgmentCallbackFactory,
                   public ntccfg::Shared<TestClient>
{
    /// This type defines a type alias for this type.
    typedef TestClient Self;

    /// Defines a type alias for a mutex.
    typedef ntci::Mutex Mutex;

    /// Defines a type alias for a mutex lock guard.
    typedef ntci::LockGuard LockGuard;

    Mutex                                    d_mutex;
    bsl::shared_ptr<ntci::DataPool>          d_dataPool_sp;
    bsl::shared_ptr<ntcf::TestMessagePool>   d_messagePool_sp;
    bsl::shared_ptr<ntci::Serialization>     d_serialization_sp;
    bsl::shared_ptr<ntci::Compression>       d_compression_sp;
    bsl::shared_ptr<ntci::Scheduler>         d_scheduler_sp;
    bsl::shared_ptr<ntci::Strand>            d_strand_sp;
    bsl::shared_ptr<ntci::DatagramSocket>    d_datagramSocket_sp;
    bsl::shared_ptr<ntcf::TestMessageParser> d_datagramParser_sp;
    bsl::shared_ptr<ntci::StreamSocket>      d_streamSocket_sp;
    bsl::shared_ptr<ntcf::TestMessageParser> d_streamParser_sp;
    ntsa::Endpoint                           d_tcpEndpoint;
    ntsa::Endpoint                           d_udpEndpoint;
    bsls::AtomicBool                         d_closed;
    ntcf::TestClientConfig                   d_config;
    bslma::Allocator*                        d_allocator_p;

    BALL_LOG_SET_CLASS_CATEGORY("NTCF.TEST.CLIENT");

private:
    TestClient(const TestClient&) BSLS_KEYWORD_DELETED;
    TestClient& operator=(const TestClient&) BSLS_KEYWORD_DELETED;

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




    /// Process the connection of the specified 'datagramSocket' according to
    /// the specified 'event'.
    void processDatagramSocketConnectEvent(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ConnectEvent&                    event);
    
    /// Process the connection of the specified 'streamSocket' according to the
    /// specified 'event'.
    void processStreamSocketConnectEvent(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ConnectEvent&                  event);


    /// Process the upgrade of the specified 'streamSocket' that is the
    /// specified 'upgradable' object used by the specified 'clientSocket'
    /// according to the specified 'event'.
    void processStreamSocketUpgradeEvent(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::UpgradeEvent&                  event);
    



    /// Wait for the TLS handshake to begin upgrading the connection to the
    /// authority to be authenticated and secured.
    void privateStreamSocketInitiateUpgrade(
        const bsl::shared_ptr<Self>&                self,
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

    /// Return the strand on which this object's functions should be called.
    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;

public:
    /// Create a new client with the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit TestClient(const ntcf::TestClientConfig& configuration,
                        const ntsa::Endpoint&         tcpEndpoint,
                        const ntsa::Endpoint&         udpEndpoint,
                        bslma::Allocator*             basicAllocator = 0);

    /// Destroy this object.
    ~TestClient();

    /// Send the specified 'bid' to the server and block until a response is
    /// received or an error occurs. On success, load the completed trade into
    /// the specified 'trade', otherwise load the error information into the
    /// specified 'fault'. Return the error.
    ntsa::Error bid(
        ntcf::TestFault*     fault,
        ntcf::TestTrade*     trade, 
        const ntcf::TestBid& bid);

    /// Send the specified 'bid' to the server. Invoke the specified 'callback'
    /// when a response is received or an error occurs. Return the error.
    ntsa::Error bid(
        const ntcf::TestBid&           bid,
        const ntcf::TestTradeCallback& callback);

    /// Send the specified 'ask' to the server and block until a response is
    /// received or an error occurs. On success, load the completed trade into
    /// the specified 'trade', otherwise load the error information into the
    /// specified 'fault'. Return the error.
    ntsa::Error ask(
        ntcf::TestFault*     fault,
        ntcf::TestTrade*     trade, 
        const ntcf::TestAsk& ask);

    /// Send the specified 'ask' to the server. Invoke the specified 'callback'
    /// when a response is received or an error occurs. Return the error.
    ntsa::Error ask(
        const ntcf::TestAsk&           ask,
        const ntcf::TestTradeCallback& callback);

    /// Send the specified 'encryption' control message to the server and 
    /// block until the operation completes. On success, load the response
    /// into the specified 'acknowledgement', otherwise, load the error 
    /// information into the specified 'fault'. Return the error. 
    ntsa::Error encrypt(
        ntcf::TestFault*                   fault,
        ntcf::TestAcknowledgment*          acknowledgement, 
        const ntcf::TestControlEncryption& encryption);

    /// Send the specified 'encryption' control message to the server. Invoke
    /// the specified 'callback' when a response is received or an error
    /// occurs. Return the error.
    ntsa::Error encrypt(
        const ntcf::TestControlEncryption&      encryption,
        const ntcf::TestAcknowledgmentCallback& callback);

    /// Send the specified 'compression' control message to the server and 
    /// block until the operation completes. On success, load the response
    /// into the specified 'acknowledgement', otherwise, load the error 
    /// information into the specified 'fault'. Return the error. 
    ntsa::Error compress(
        ntcf::TestFault*                    fault,
        ntcf::TestAcknowledgment*           acknowledgement, 
        const ntcf::TestControlCompression& compression);

    /// Send the specified 'compression' control message to the server. Invoke
    /// the specified 'callback' when a response is received or an error
    /// occurs. Return the error.
    ntsa::Error compress(
        const ntcf::TestControlCompression&     compression,
        const ntcf::TestAcknowledgmentCallback& callback);

    /// Send the specified 'heartbeat' control message to the server and 
    /// block until the operation completes. On success, load the response
    /// into the specified 'acknowledgement', otherwise, load the error 
    /// information into the specified 'fault'. Return the error. 
    ntsa::Error heartbeat(
        ntcf::TestFault*                  fault,
        ntcf::TestAcknowledgment*         acknowledgement, 
        const ntcf::TestControlHeartbeat& heartbeat);

    /// Send the specified 'heartbeat' control message to the server. Invoke
    /// the specified 'callback' when a response is received or an error
    /// occurs. Return the error.
    ntsa::Error heartbeat(
        const ntcf::TestControlHeartbeat&       heartbeat,
        const ntcf::TestAcknowledgmentCallback& callback);
};

}  // end namespace ntcf
}  // end namespace BloombergLP
#endif
