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

#include <ntcf_testmessage.h>
#include <ntcf_testvocabulary.h>

namespace BloombergLP {
namespace ntcf {

/// Provide a test client transaction.
///
/// @par Thread Safety
/// This class is thread safe.
class TestClientTransaction
{
    mutable bsls::SpinLock             d_lock;
    bsl::uint64_t                      d_id;
    bsl::shared_ptr<ntcf::TestMessage> d_request_sp;
    bsl::shared_ptr<ntci::Timer>       d_timer_sp;
    ntcf::TestOptions                  d_options;
    ntcf::TestMessageCallback          d_callback;
    bslma::Allocator*                  d_allocator_p;

  private:
    TestClientTransaction(const TestClientTransaction&) BSLS_KEYWORD_DELETED;
    TestClientTransaction& operator=(const TestClientTransaction&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new client transaction identified by the specified 'id'
    /// initiated by the specified 'request' according to the specified
    /// 'options'. Invoke the specified 'callback' when the response is
    /// received or an error occurs. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    TestClientTransaction(bsl::uint64_t                             id,
                          const bsl::shared_ptr<ntcf::TestMessage>& request,
                          const ntcf::TestOptions&                  options,
                          const ntcf::TestMessageCallback&          callback,
                          bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~TestClientTransaction();

    /// Set the timer for the this entry to the specified 'timer'.
    void setTimer(const bsl::shared_ptr<ntci::Timer>& timer);

    /// Invoke the callback with the specified 'socket', 'response', and
    /// 'error' from the specified 'strand'. Return true if the callback was
    /// invoked, or false if the callback is undefined, already invoked, or
    /// has timed out.
    bool invoke(const ntcf::TestContext&                  context,
                const ntcf::TestFault&                    fault,
                const bsl::shared_ptr<ntcf::TestMessage>& response,
                const bsl::shared_ptr<ntci::Strand>&      strand);

    /// Invoke the callback with the specified 'socket', 'response', and
    /// 'error' from the specified 'strand'. Return true if the callback was
    /// invoked, or false if the callback is undefined, already invoked, or
    /// has timed out.
    bool dispatch(const ntcf::TestContext&                  context,
                  const ntcf::TestFault&                    fault,
                  const bsl::shared_ptr<ntcf::TestMessage>& response,
                  const bsl::shared_ptr<ntci::Strand>&      strand,
                  const bsl::shared_ptr<ntci::Executor>&    executor,
                  bool                                      defer,
                  ntci::Mutex*                              mutex);

    /// Timeout the entry if it has not already completed and invoke the
    /// callback on the specified 'strand'.
    void timeout(const bsl::shared_ptr<ntci::Strand>& strand);

    /// Return the unique identifier of this transaction.
    bsl::uint64_t id() const;

    /// Return the request.
    const bsl::shared_ptr<ntcf::TestMessage>& request() const;

    /// Return the options.
    const ntcf::TestOptions& options() const;

    /// Return true if the transmission has an associated timer set, otherwise
    /// return false.
    bool hasTimer() const;

    /// Return true if the transmission has an associated callback set,
    /// otherwise return false.
    bool hasCallback() const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(TestClientTransaction);
};

/// Provide a pool of client transactions.
///
/// @par Thread Safety
/// This class is thread safe.
class TestClientTransactionPool
{
    bdlma::ConcurrentPoolAllocator d_pool;
    bslma::Allocator*              d_allocator_p;

  private:
    TestClientTransactionPool(const TestClientTransactionPool&)
        BSLS_KEYWORD_DELETED;
    TestClientTransactionPool& operator=(const TestClientTransactionPool&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new client transaction pool. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit TestClientTransactionPool(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~TestClientTransactionPool();

    /// Create a new client transaction for the specified 'request' with the
    /// specified 'options'. Invoke the specified 'callback' when the response
    /// to the request has been received or an error occurs. The resulting
    /// object is automatically returned to this pool when its reference count
    /// reaches zero.
    bsl::shared_ptr<ntcf::TestClientTransaction> create(
        bsl::uint64_t                             id,
        const bsl::shared_ptr<ntcf::TestMessage>& request,
        const ntcf::TestOptions&                  options,
        const ntcf::TestMessageCallback&          callback);
};

NTCCFG_INLINE
bsl::shared_ptr<ntcf::TestClientTransaction> TestClientTransactionPool::create(
    bsl::uint64_t                             id,
    const bsl::shared_ptr<ntcf::TestMessage>& request,
    const ntcf::TestOptions&                  options,
    const ntcf::TestMessageCallback&          callback)
{
    bsl::shared_ptr<ntcf::TestClientTransaction> result;
    result
        .createInplace(&d_pool, id, request, options, callback, d_allocator_p);
    return result;
}

/// Provide a test client transaction catalog.
///
/// @par Thread Safety
/// This class is thread safe.
class TestClientTransactionCatalog
{
    typedef bsl::unordered_map<bsl::uint64_t,
                               bsl::shared_ptr<ntcf::TestClientTransaction> >
        Map;

    /// Defines a type alias for a mutex.
    typedef ntci::Mutex Mutex;

    /// Defines a type alias for a mutex lock guard.
    typedef ntci::LockGuard LockGuard;

    Mutex                           d_mutex;
    ntcf::TestClientTransactionPool d_pool;
    Map                             d_map;
    bsl::uint64_t                   d_generation;
    bslma::Allocator*               d_allocator_p;

  private:
    TestClientTransactionCatalog(const TestClientTransactionCatalog&)
        BSLS_KEYWORD_DELETED;
    TestClientTransactionCatalog& operator=(
        const TestClientTransactionCatalog&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new test client transaction catalog. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit TestClientTransactionCatalog(
        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~TestClientTransactionCatalog();

    /// Create and register a new client transaction for the specified
    /// 'request' with the specified 'options' that invokes the specified
    /// 'callback' when the response to the request has been received or an
    /// error occurs. Return the new transaction.
    bsl::shared_ptr<ntcf::TestClientTransaction> create(
        const bsl::shared_ptr<ntcf::TestMessage>& request,
        const ntcf::TestOptions&                  options,
        const ntcf::TestMessageCallback&          callback);

    /// Remove the transaction identified by the specified 'message' and load
    /// it into the specified 'result'. Return true if a such a transaction
    /// exists, and false otherwise.
    bool remove(const bsl::shared_ptr<ntcf::TestMessage>& message);

    /// Remove the transaction identified by the specified 'message' and load
    /// it into the specified 'result'. Return true if a such a transaction
    /// exists, and false otherwise.
    bool remove(bsl::shared_ptr<ntcf::TestClientTransaction>* result,
                const bsl::shared_ptr<ntcf::TestMessage>&     message);
};

/// Provide a test client.
///
/// @par Thread Safety
/// This class is thread safe.
class TestClient : public ntci::DatagramSocketSession,
                   public ntci::DatagramSocketManager,
                   public ntci::StreamSocketSession,
                   public ntci::StreamSocketManager,
                   public ntci::ConnectCallbackFactory,
                   public ntcf::TestMessageCallbackFactory,
                   public ntcf::TestTradeCallbackFactory,
                   public ntcf::TestEchoCallbackFactory,
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
    bsl::shared_ptr<ntcf::TestMessageEncryption> d_encryption_sp;
    ntcf::TestClientTransactionCatalog       d_transactionCatalog;
    bslmt::Semaphore                         d_downgradeSemaphore;
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

    /// Analyze the specified incoming 'message'. Load the context into the
    /// specified 'context', the fault, if any, into the specified 'fault',
    /// and the response, if any, into the specified 'response'.
    void analyzeIncomingMessage(
        ntcf::TestContext*                        context,
        ntcf::TestFault*                          fault,
        bsl::shared_ptr<ntcf::TestMessage>*       response,
        const bsl::shared_ptr<ntcf::TestMessage>& message);

    /// Load into the specified 'fault' the error information describing a
    /// failure to initiate an operation.
    void describeInititationFailure(ntcf::TestFault* fault);

    /// Load into the specified 'fault' the error information describing a
    /// failure to wait for an operation to complete.
    void describeWaitFailure(ntcf::TestFault* fault);

    /// Load into the specified 'fault' the error information describing a
    /// an unexpected result type.
    void describeResultTypeFailure(ntcf::TestFault* fault);

    /// Invoke the specified 'callback' with the specified 'connector' and
    /// 'event'.
    void dispatchConnect(const bsl::shared_ptr<ntci::Connector>& connector,
                         const ntca::ConnectEvent&               event,
                         const ntci::ConnectCallback&            callback);

    /// Invoke the specified 'callback' with the arguments deduced from the
    /// specified 'context', 'fault', and 'message'.
    void dispatchTrade(const ntcf::TestContext&                  context,
                       const ntcf::TestFault&                    fault,
                       const bsl::shared_ptr<ntcf::TestMessage>& message,
                       const ntcf::TestTradeCallback&            callback);

    /// Invoke the specified 'callback' with the arguments deduced from the
    /// specified 'context', 'fault', and 'message'.
    void dispatchEcho(const ntcf::TestContext&                  context,
                      const ntcf::TestFault&                    fault,
                      const bsl::shared_ptr<ntcf::TestMessage>& message,
                      const ntcf::TestEchoCallback&             callback);

    /// Invoke the specified 'callback' with the arguments deduced from the
    /// specified 'context', 'fault', and 'message'.
    void dispatchAcknowledgment(
        const ntcf::TestContext&                  context,
        const ntcf::TestFault&                    fault,
        const bsl::shared_ptr<ntcf::TestMessage>& message,
        const ntcf::TestAcknowledgmentCallback    callback);

    /// Return the strand on which this object's functions should be called.
    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;

  public:
    /// Create a new client with the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    TestClient(const ntcf::TestClientConfig&           configuration,
               const bsl::shared_ptr<ntci::Scheduler>& scheduler,
               const bsl::shared_ptr<ntci::DataPool>&  dataPool,
               const bsl::shared_ptr<ntcf::TestMessageEncryption>& encryption,
               const ntsa::Endpoint&                   tcpEndpoint,
               const ntsa::Endpoint&                   udpEndpoint,
               bslma::Allocator*                       basicAllocator = 0);

    /// Destroy this object.
    ~TestClient();

    /// Connect to the server and block until the connection is established.
    /// Return the error.
    ntsa::Error connect();

    /// Connect to the server and invoke the specified 'callback' when the
    /// connection is established or an error occurs. Return the error.
    ntsa::Error connect(const ntci::ConnectCallback& callback);

    /// Return a new messsage.
    bsl::shared_ptr<ntcf::TestMessage> createMessage();

    /// Return a new messsage initially primed for the specified 'options'.
    bsl::shared_ptr<ntcf::TestMessage> createMessage(
        const ntcf::TestOptions& options);

    /// Send the specified 'message' to the server and block until a response
    /// is received or an error occurs. On success, load the completed trade
    /// into the specified 'trade', otherwise load the error information into
    /// the specified 'fault'. Return the error.
    ntsa::Error message(ntcf::TestContext*                        context,
                        ntcf::TestFault*                          fault,
                        bsl::shared_ptr<ntcf::TestMessage>*       response,
                        const bsl::shared_ptr<ntcf::TestMessage>& request,
                        const ntcf::TestOptions&                  options);

    /// Send the specified 'message' to the server. Invoke the specified
    /// 'callback' when a response is received or an error occurs. Return the
    /// error.
    ntsa::Error message(const bsl::shared_ptr<ntcf::TestMessage>& request,
                        const ntcf::TestOptions&                  options,
                        const ntcf::TestMessageCallback&          callback);

    /// Send the specified 'bid' to the server and block until a response is
    /// received or an error occurs. On success, load the completed trade into
    /// the specified 'trade', otherwise load the error information into the
    /// specified 'fault'. Return the error.
    ntsa::Error bid(ntcf::TestContext*       context,
                    ntcf::TestFault*         fault,
                    ntcf::TestTrade*         trade,
                    const ntcf::TestBid&     bid,
                    const ntcf::TestOptions& options);

    /// Send the specified 'bid' to the server and block until a response is
    /// received or an error occurs. On success, load the result into the
    /// specified 'result'. Return the error.
    ntsa::Error bid(ntcf::TestTradeResult*   result,
                    const ntcf::TestBid&     bid,
                    const ntcf::TestOptions& options);

    /// Send the specified 'bid' to the server. Invoke the specified 'callback'
    /// when a response is received or an error occurs. Return the error.
    ntsa::Error bid(const ntcf::TestBid&           bid,
                    const ntcf::TestOptions&       options,
                    const ntcf::TestTradeCallback& callback);

    /// Send the specified 'ask' to the server and block until a response is
    /// received or an error occurs. On success, load the completed trade into
    /// the specified 'trade', otherwise load the error information into the
    /// specified 'fault'. Return the error.
    ntsa::Error ask(ntcf::TestContext*       context,
                    ntcf::TestFault*         fault,
                    ntcf::TestTrade*         trade,
                    const ntcf::TestAsk&     ask,
                    const ntcf::TestOptions& options);

    /// Send the specified 'ask' to the server and block until a response is
    /// received or an error occurs. On success, load the result into the
    /// specified 'result'. Return the error.
    ntsa::Error ask(ntcf::TestTradeResult*   result,
                    const ntcf::TestAsk&     ask,
                    const ntcf::TestOptions& options);

    /// Send the specified 'ask' to the server. Invoke the specified 'callback'
    /// when a response is received or an error occurs. Return the error.
    ntsa::Error ask(const ntcf::TestAsk&           ask,
                    const ntcf::TestOptions&       options,
                    const ntcf::TestTradeCallback& callback);

    /// Send the specified 'signal' control message to the server and block
    /// until the operation completes. On success, load the response into the
    /// specified 'echo', otherwise, load the error information into the
    /// specified 'fault'. Return the error.
    ntsa::Error signal(ntcf::TestContext*       context,
                       ntcf::TestFault*         fault,
                       ntcf::TestEcho*          echo,
                       const ntcf::TestSignal&  signal,
                       const ntcf::TestOptions& options);

    /// Send the specified 'signal' control message to the server and block
    /// until a response is received or an error occurs. On success, load the
    /// result into the specified 'result'. Return the error.
    ntsa::Error signal(ntcf::TestEchoResult*    result,
                       const ntcf::TestSignal&  signal,
                       const ntcf::TestOptions& options);

    /// Send the specified 'signal' control message to the server. Invoke the
    /// specified 'callback' when a response is received or an error occurs.
    /// Return the error.
    ntsa::Error signal(const ntcf::TestSignal&       signal,
                       const ntcf::TestOptions&      options,
                       const ntcf::TestEchoCallback& callback);

    /// Send the specified 'encryption' control message to the server and
    /// block until the operation completes. On success, load the response
    /// into the specified 'acknowledgment', otherwise, load the error
    /// information into the specified 'fault'. Return the error.
    ntsa::Error encrypt(ntcf::TestContext*                 context,
                        ntcf::TestFault*                   fault,
                        ntcf::TestAcknowledgment*          acknowledgment,
                        const ntcf::TestControlEncryption& encryption,
                        const ntcf::TestOptions&           options);

    /// Send the specified 'encryption' control message to the server and
    /// block until a response is received or an error occurs. On success, load
    /// the result into the specified 'result'. Return the error.
    ntsa::Error encrypt(ntcf::TestAcknowledgmentResult*     result,
                        const ntcf::TestControlEncryption&  encryption,
                        const ntcf::TestOptions&            options);

    /// Send the specified 'encryption' control message to the server. Return
    /// the error.
    ntsa::Error encrypt(const ntcf::TestControlEncryption& encryption,
                        const ntcf::TestOptions&           options);

    /// Send the specified 'encryption' control message to the server. Invoke
    /// the specified 'callback' when a response is received or an error
    /// occurs. Return the error.
    ntsa::Error encrypt(const ntcf::TestControlEncryption&      encryption,
                        const ntcf::TestOptions&                options,
                        const ntcf::TestAcknowledgmentCallback& callback);

    /// Send the specified 'compression' control message to the server and
    /// block until the operation completes. On success, load the response
    /// into the specified 'acknowledgment', otherwise, load the error
    /// information into the specified 'fault'. Return the error.
    ntsa::Error compress(ntcf::TestContext*                  context,
                         ntcf::TestFault*                    fault,
                         ntcf::TestAcknowledgment*           acknowledgment,
                         const ntcf::TestControlCompression& compression,
                         const ntcf::TestOptions&            options);

    /// Send the specified 'compression' control message to the server and
    /// block until a response is received or an error occurs. On success, load
    /// the result into the specified 'result'. Return the error.
    ntsa::Error compress(ntcf::TestAcknowledgmentResult*      result,
                         const ntcf::TestControlCompression&  compression,
                         const ntcf::TestOptions&             options);

    /// Send the specified 'compression' control message to the server. Invoke
    /// the specified 'callback' when a response is received or an error
    /// occurs. Return the error.
    ntsa::Error compress(const ntcf::TestControlCompression&     compression,
                         const ntcf::TestOptions&                options,
                         const ntcf::TestAcknowledgmentCallback& callback);

    /// Send the specified 'compression' control message to the server. Return
    /// the error.
    ntsa::Error compress(const ntcf::TestControlCompression& compression,
                         const ntcf::TestOptions&            options);

    /// Send the specified 'heartbeat' control message to the server and
    /// block until the operation completes. On success, load the response
    /// into the specified 'acknowledgment', otherwise, load the error
    /// information into the specified 'fault'. Return the error.
    ntsa::Error heartbeat(ntcf::TestContext*                context,
                          ntcf::TestFault*                  fault,
                          ntcf::TestAcknowledgment*         acknowledgment,
                          const ntcf::TestControlHeartbeat& heartbeat,
                          const ntcf::TestOptions&          options);

    /// Send the specified 'heartbeat' control message to the server and
    /// block until a response is received or an error occurs. On success, load
    /// the result into the specified 'result'. Return the error.
    ntsa::Error heartbeat(ntcf::TestAcknowledgmentResult*    result,
                          const ntcf::TestControlHeartbeat&  heartbeat,
                          const ntcf::TestOptions&           options);

    /// Send the specified 'heartbeat' control message to the server. Invoke
    /// the specified 'callback' when a response is received or an error
    /// occurs. Return the error.
    ntsa::Error heartbeat(const ntcf::TestControlHeartbeat&       heartbeat,
                          const ntcf::TestOptions&                options,
                          const ntcf::TestAcknowledgmentCallback& callback);

    /// Send the specified 'heartbeat' control message to the server. Return
    /// the error.
    ntsa::Error heartbeat(const ntcf::TestControlHeartbeat& heartbeat,
                          const ntcf::TestOptions&          options);

    /// Enable compression on all communication with the server.
    void enableCompression();

    /// Enable encryption on all communication with the server.
    void enableEncryption();

    /// Disable compression on all communication with the server.
    void disableCompression();

    /// Disable encryption on all communication with the server.
    void disableEncryption();

    /// Close all sockets.
    void close();

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(TestClient);
};

/// Defines a type alias for shared pointer to a test client.
typedef bsl::shared_ptr<ntcf::TestClient> TestClientPtr;

/// Defines a type alias for vector of shared pointers to test clients.
typedef bsl::vector<ntcf::TestClientPtr> TestClientVector;

}  // end namespace ntcf
}  // end namespace BloombergLP
#endif
