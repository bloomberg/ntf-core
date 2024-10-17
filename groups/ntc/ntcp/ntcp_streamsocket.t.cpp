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

#include <ntscfg_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcp_streamsocket_t_cpp, "$Id$ $CSID$")

#include <ntcp_streamsocket.h>

#include <ntcd_blobbufferfactory.h>
#include <ntcd_datapool.h>
#include <ntcd_datautil.h>
#include <ntcd_encryption.h>
#include <ntcd_proactor.h>
#include <ntcd_resolver.h>
#include <ntcd_simulation.h>
#include <ntcd_timer.h>
#include <ntci_log.h>
#include <ntcm_monitorable.h>
#include <ntcs_datapool.h>
#include <ntcs_ratelimiter.h>
#include <ntcs_user.h>
#include <ntcu_streamsocketeventqueue.h>
#include <ntsa_error.h>
#include <ntsa_transport.h>
#include <ntscfg_mock.h>
#include <ntsi_streamsocket.h>

using namespace BloombergLP;

// Uncomment to test a particular style of socket-to-thread load balancing,
// instead of both static and dynamic load balancing.
// #define NTCP_STREAM_SOCKET_TEST_DYNAMIC_LOAD_BALANCING false

namespace BloombergLP {
namespace ntcp {

// Provide tests for 'ntcp::StreamSocket'.
class StreamSocketTest
{
    /// Provide a test case execution framework.
    class Framework;

    /// This struct defines the parameters of a test.
    class Parameters;

    /// Provide a stream socket protocol for this test driver.
    class StreamSocketSession;

    /// Provide a stream socket manager for this test driver.
    class StreamSocketManager;

    // Execute the concern with the specified 'parameters' for the specified
    // 'transport' using the specified 'proactor'.
    static void verifyGenericVariation(
        ntsa::Transport::Value                transport,
        const bsl::shared_ptr<ntci::Proactor>& proactor,
        const StreamSocketTest::Parameters&   parameters,
        bslma::Allocator*                     allocator);

    // Execute the concern with the specified 'parameters' for the specified
    // 'transport' using the specified 'proactor'.
    static void verifySendDeadlineVariation(
        ntsa::Transport::Value                transport,
        const bsl::shared_ptr<ntci::Proactor>& proactor,
        const StreamSocketTest::Parameters&   parameters,
        bslma::Allocator*                     allocator);

    // Execute the concern with the specified 'parameters' for the specified
    // 'transport' using the specified 'proactor'.
    static void verifySendCancellationVariation(
        ntsa::Transport::Value                transport,
        const bsl::shared_ptr<ntci::Proactor>& proactor,
        const StreamSocketTest::Parameters&   parameters,
        bslma::Allocator*                     allocator);

    // Execute the concern with the specified 'parameters' for the specified
    // 'transport' using the specified 'proactor'.
    static void verifyReceiveDeadlineVariation(
        ntsa::Transport::Value                transport,
        const bsl::shared_ptr<ntci::Proactor>& proactor,
        const StreamSocketTest::Parameters&   parameters,
        bslma::Allocator*                     allocator);

    // Execute the concern with the specified 'parameters' for the specified
    // 'transport' using the specified 'proactor'.
    static void verifyReceiveCancellationVariation(
        ntsa::Transport::Value                transport,
        const bsl::shared_ptr<ntci::Proactor>& proactor,
        const StreamSocketTest::Parameters&   parameters,
        bslma::Allocator*                     allocator);

    // Process the expected send timeout.
    static void processSendTimeout(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Sender>&       sender,
        const ntca::SendEvent&                     event,
        const bsl::string&                         name,
        const ntsa::Error&                         error,
        bslmt::Semaphore*                          semaphore);

    // Process the expected send cancellation.
    static void processSendCancellation(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Sender>&       sender,
        const ntca::SendEvent&                     event,
        const bsl::string&                         name,
        const ntsa::Error&                         error,
        bslmt::Semaphore*                          semaphore);

    // Process the expected receive timeout.
    static void processReceiveTimeout(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Receiver>&     receiver,
        const bsl::shared_ptr<bdlbb::Blob>&        data,
        const ntca::ReceiveEvent&                  event,
        bslmt::Semaphore*                          semaphore);

    // Process the expected receive timeout or success.
    static void processReceiveTimeoutOrSuccess(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Receiver>&     receiver,
        const bsl::shared_ptr<bdlbb::Blob>&        data,
        const ntca::ReceiveEvent&                  event,
        const bsl::string&                         name,
        const ntsa::Error&                         error,
        bslmt::Semaphore*                          semaphore);

    // Process the expected receive cancellation.
    static void processReceiveCancellation(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Receiver>&     receiver,
        const bsl::shared_ptr<bdlbb::Blob>&        data,
        const ntca::ReceiveEvent&                  event,
        bslmt::Semaphore*                          semaphore);

    // Process the expected receive cancellation or success.
    static void processReceiveCancellationOrSuccess(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Receiver>&     receiver,
        const bsl::shared_ptr<bdlbb::Blob>&        data,
        const ntca::ReceiveEvent&                  event,
        const bsl::string&                         name,
        const ntsa::Error&                         error,
        bslmt::Semaphore*                          semaphore);

    /// Validate that the specified 'metrics' does not contain data for
    /// elements starting from the specified 'base' up to 'base' + the
    /// 'specified 'number' (exclusive) in total.
    static void validateNoMetricsAvailable(const bdld::DatumArrayRef& metrics,
                                           int                        base,
                                           int                        number);

    /// Validate that the specified 'metrics' contains data for elements
    /// starting from the specified 'base' up to 'base' + the specified
    /// 'number' (exclusive) in total.
    static void validateMetricsAvailable(const bdld::DatumArrayRef& metrics,
                                         int                        base,
                                         int                        number);

    // Cancel the send operation of the specified 'streamSocket' identified
    // by the specified 'token'.
    static void cancelSend(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::SendToken                      token);

    // Cancel the receive operation on the specified 'streamSocket' identified
    // by the specified 'token'.
    static void cancelReceive(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReceiveToken                   token);

    /// Return an endpoint representing a suitable address to which to
    /// bind a socket of the specified 'transport' type for use by this
    /// test driver.
    static ntsa::Endpoint any(ntsa::Transport::Value transport);

  public:
    // Concern: Breathing test.
    static void verifyBreathing();

    // Concern: Breathing test using asynchronous callbacks.
    static void verifyBreathingAsync();

    // Concern: Stress test using the read queue low watermark.
    static void verifyStress();

    // Concern: Stress test using asynchronous callbacks.
    static void verifyStressAsync();

    // Concern: Minimal read queue high watermark.
    static void verifyMinimalReadQueueHighWatermark();

    // Concern: Minimal read queue high watermark using asynchronous callbacks.
    static void verifyMinimalReadQueueHighWatermarkAsync();

    // Concern: Minimal write queue high watermark.
    static void verifyMinimalWriteQueueHighWatermark();

    // Concern: Minimal write queue high watermark using asynchronous
    // callbacks.
    static void verifyMinimalWriteQueueHighWatermarkAsync();

    // Concern: Rate limit copying to the send buffer.
    static void verifySendRateLimiting();

    // Concern: Rate limit copying to the send buffer using asynchronous
    // callbacks.
    static void verifySendRateLimitingAsync();

    // Concern: Rate limit copying from the receive buffer.
    static void verifyReceiveRateLimiting();

    // Concern: Rate limit copying from the receive buffer using
    // asynchronous callbacks.
    static void verifyReceiveRateLimitingAsync();

    // Concern: Send deadlines.
    static void verifySendDeadline();

    // Concern: Receive deadlines.
    static void verifyReceiveDeadline();

    // Concern: Send cancellation.
    static void verifySendCancellation();

    // Concern: Receive cancellation.
    static void verifyReceiveCancellation();

    // Concern: Write queue high watermark event can be overriden on a
    // per-send basis.
    static void verifyWriteQueueHighWatermarkOverride();

    // Concern: RX timestamping test.
    static void verifyIncomingTimestamps();

    // Concern: RX timestamping test.
    static void verifyOutgoingTimestamps();
};

/// Provide a test case execution framework.
class StreamSocketTest::Framework
{
  private:
    /// Run a thread identified by the specified 'threadIndex' that waits
    /// on the specified 'barrier' then drives the specified 'proactor' until
    /// it is stopped.
    static void runProactor(const bsl::shared_ptr<ntci::Proactor>& proactor,
                           bslmt::Barrier*                       barrier,
                           bsl::size_t                           threadIndex);

  public:
    /// Define a type alias for the function implementing a
    /// test case driven by this test framework.
    typedef NTCCFG_FUNCTION(ntsa::Transport::Value                transport,
                            const bsl::shared_ptr<ntci::Proactor>& proactor,
                            bslma::Allocator* allocator) ExecuteCallback;

    /// Execute the specified 'executeCallback' implementing a test case,
    /// varying the test configuration and machinery by transport, driver
    /// type, and number of threads.
    static void execute(const ExecuteCallback& executeCallback);

    /// Execute the specified 'executeCallback' implementing a test case
    /// for the specified 'transport', varying the test configuration and
    /// machinery by driver type and number of threads.
    static void execute(ntsa::Transport::Value transport,
                        const ExecuteCallback& executeCallback);

    /// Execute the specified 'executeCallback' implementing a test case
    /// for the specified 'transport' and 'numThreads'.
    static void execute(ntsa::Transport::Value transport,
                        bsl::size_t            numThreads,
                        const ExecuteCallback& executeCallback);
};

/// This struct defines the parameters of a test.
class StreamSocketTest::Parameters
{
  public:
    ntsa::Transport::Value             d_transport;
    bsl::size_t                        d_numSocketPairs;
    bsl::size_t                        d_numTimers;
    bsl::size_t                        d_numMessages;
    bsl::size_t                        d_messageSize;
    bsl::shared_ptr<bdlbb::Blob>       d_message_sp;
    bdlb::NullableValue<bsl::size_t>   d_readRate;
    bsl::shared_ptr<ntci::RateLimiter> d_readRateLimiter_sp;
    bsl::size_t                        d_readQueueHighWatermark;
    bdlb::NullableValue<bsl::size_t>   d_writeRate;
    bsl::shared_ptr<ntci::RateLimiter> d_writeRateLimiter_sp;
    bsl::size_t                        d_writeQueueHighWatermark;
    bdlb::NullableValue<bsl::size_t>   d_sendBufferSize;
    bdlb::NullableValue<bsl::size_t>   d_receiveBufferSize;
    bool                               d_useAsyncCallbacks;
    bool                               d_timestampIncomingData;
    bool                               d_timestampOutgoingData;
    bool                               d_collectMetrics;

    Parameters()
    : d_transport(ntsa::Transport::e_TCP_IPV4_STREAM)
    , d_numSocketPairs(1)
    , d_numTimers(0)
    , d_numMessages(1)
    , d_messageSize(32)
    , d_message_sp()
    , d_readRate()
    , d_readRateLimiter_sp()
    , d_readQueueHighWatermark(static_cast<bsl::size_t>(-1))
    , d_writeRate()
    , d_writeRateLimiter_sp()
    , d_writeQueueHighWatermark(static_cast<bsl::size_t>(-1))
    , d_sendBufferSize()
    , d_receiveBufferSize()
    , d_useAsyncCallbacks(false)
    , d_timestampIncomingData(false)
    , d_timestampOutgoingData(false)
    , d_collectMetrics(false)
    {
    }
};

/// This test provides a stream socket protocol for this test driver.
class StreamSocketTest::StreamSocketSession : public ntci::StreamSocketSession
{
    ntccfg::Object                      d_object;
    bsl::shared_ptr<ntci::StreamSocket> d_streamSocket_sp;
    bdlbb::Blob                         d_dataReceived;
    bslmt::Latch                        d_numTimerEvents;
    bsls::AtomicUint                    d_numMessagesLeftToSend;
    bslmt::Latch                        d_numMessagesSent;
    bslmt::Latch                        d_numMessagesReceived;
    StreamSocketTest::Parameters        d_parameters;
    bslma::Allocator*                   d_allocator_p;

  private:
    StreamSocketSession(const StreamSocketSession&) BSLS_KEYWORD_DELETED;
    StreamSocketSession& operator=(const StreamSocketSession&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Process the condition that the size of the read queue is greater
    /// than or equal to the read queue low watermark.
    void processReadQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue is less
    /// than or equal to the write queue low watermark.
    void processWriteQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process a read completion with the specified 'data' or failure
    /// according to the specified 'error'.
    void processRead(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                     const bsl::shared_ptr<ntci::Receiver>&     receiver,
                     const bsl::shared_ptr<bdlbb::Blob>&        data,
                     const ntca::ReceiveEvent&                  event);

    /// Process a send completion or failure according to the specified
    /// 'error'.
    void processWrite(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                      const bsl::shared_ptr<ntci::Sender>&       sender,
                      const ntca::SendEvent&                     event);

    /// Process a timer event.
    void processTimer(const bsl::shared_ptr<ntci::Timer>& timer,
                      const ntca::TimerEvent&             event);

  public:
    /// Create a new stream socket application implemented using the
    /// specified 'streamSocket' that operates according to the specfied
    /// test 'parameters'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    StreamSocketSession(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const StreamSocketTest::Parameters&        parameters,
        bslma::Allocator*                          basicAllocator = 0);

    /// Destroy this object.
    ~StreamSocketSession() BSLS_KEYWORD_OVERRIDE;

    /// Start the timers for which this socket is responsible.
    void schedule();

    /// Send data to the peer.
    void send();

    /// Begin receiving data.
    void receive();

    /// Wait until all expected timers have fired and messages have been
    /// received.
    void wait();

    /// Close the socket.
    void close();

    /// Return the source endpoint of the stream socket.
    ntsa::Endpoint sourceEndpoint() const;

    /// Return the remote endpoint of the stream socket.
    ntsa::Endpoint remoteEndpoint() const;
};

/// Provide a stream socket manager for this test driver.
class StreamSocketTest::StreamSocketManager
: public ntci::StreamSocketManager,
  public ntccfg::Shared<StreamSocketManager>
{
    /// Define a type alias for a set of managed stream
    /// sockets.
    typedef bsl::unordered_map<bsl::shared_ptr<ntci::StreamSocket>,
                               bsl::shared_ptr<StreamSocketSession> >
        StreamSocketApplicationMap;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    ntccfg::Object                 d_object;
    bsl::shared_ptr<ntci::Proactor> d_proactor_sp;
    bsl::shared_ptr<ntcs::Metrics> d_metrics_sp;
    Mutex                          d_socketMapMutex;
    StreamSocketApplicationMap     d_socketMap;
    bslmt::Latch                   d_socketsEstablished;
    bslmt::Latch                   d_socketsClosed;
    StreamSocketTest::Parameters   d_parameters;
    bslma::Allocator*              d_allocator_p;

  private:
    StreamSocketManager(const StreamSocketManager&) BSLS_KEYWORD_DELETED;
    StreamSocketManager& operator=(const StreamSocketManager&)
        BSLS_KEYWORD_DELETED;

  private:
    void processStreamSocketEstablished(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
        BSLS_KEYWORD_OVERRIDE;
    // Process the establishment of the specified 'streamSocket'.

    /// Process the closure of the specified 'streamSocket'.
    void processStreamSocketClosed(const bsl::shared_ptr<ntci::StreamSocket>&
                                       streamSocket) BSLS_KEYWORD_OVERRIDE;

  public:
    /// Create a new stream socket manager operating according to the
    /// specified test 'parameters' whose sockets are driven by the
    /// specified 'proactor'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    StreamSocketManager(const bsl::shared_ptr<ntci::Proactor>& proactor,
                        const StreamSocketTest::Parameters&   parameters,
                        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~StreamSocketManager() BSLS_KEYWORD_OVERRIDE;

    /// Create two stream sockets, have them send data to each, and wait
    /// for each to receive the data.
    void run();
};

void StreamSocketTest::Framework::runProactor(
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    bslmt::Barrier*                       barrier,
    bsl::size_t                           threadIndex)
{
    const char* threadNamePrefix = "test";

    bsl::string threadName;
    {
        bsl::stringstream ss;
        ss << threadNamePrefix << "-" << threadIndex;
        threadName = ss.str();
    }

    bslmt::ThreadUtil::setThreadName(threadName);

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER(threadNamePrefix);
    NTCI_LOG_CONTEXT_GUARD_THREAD(threadIndex);

    // Register this thread as the thread that will wait on the proactor.

    ntci::Waiter waiter = proactor->registerWaiter(ntca::WaiterOptions());

    // Wait until all threads have reached the rendezvous point.

    barrier->wait();

    // Process deferred functions.

    proactor->run(waiter);

    // Deregister the waiter.

    proactor->deregisterWaiter(waiter);
}

void StreamSocketTest::Framework::execute(
    const ExecuteCallback& executeCallback)
{
    Framework::execute(ntsa::Transport::e_TCP_IPV4_STREAM, executeCallback);
}

void StreamSocketTest::Framework::execute(
    ntsa::Transport::Value transport,
    const ExecuteCallback& executeCallback)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    const bsl::size_t k_MIN_THREADS = 1;
    const bsl::size_t k_MAX_THREADS = 1;

#else

    const bsl::size_t k_MIN_THREADS = 1;
    const bsl::size_t k_MAX_THREADS = 1;

#endif

    for (bsl::size_t numThreads = k_MIN_THREADS; numThreads <= k_MAX_THREADS;
         ++numThreads)
    {
        const bool dynamicLoadBalancing = numThreads > 1;

#if defined(NTCP_STREAM_SOCKET_TEST_DYNAMIC_LOAD_BALANCING)
        if (dynamicLoadBalancing !=
            NTCP_STREAM_SOCKET_TEST_DYNAMIC_LOAD_BALANCING)
        {
            continue;
        }
#endif

        Framework::execute(transport, numThreads, executeCallback);
    }
}

void StreamSocketTest::Framework::execute(
    ntsa::Transport::Value transport,
    bsl::size_t            numThreads,
    const ExecuteCallback& executeCallback)
{
    ntsa::Error error;

    BSLS_LOG_INFO("Testing transport %s numThreads %d",
                  ntsa::Transport::toString(transport),
                  (int)(numThreads));

    bsl::shared_ptr<ntcd::Simulation> simulation;
    simulation.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    error = simulation->run();
    NTSCFG_TEST_OK(error);

    const bsl::size_t k_BLOB_BUFFER_SIZE = 4096;

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(NTSCFG_TEST_ALLOCATOR,
                           k_BLOB_BUFFER_SIZE,
                           k_BLOB_BUFFER_SIZE,
                           NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntcs::User> user;
    user.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    user->setDataPool(dataPool);

    ntca::ProactorConfig proactorConfig;
    proactorConfig.setMetricName("test");
    proactorConfig.setMinThreads(numThreads);
    proactorConfig.setMaxThreads(numThreads);

    bsl::shared_ptr<ntcd::Proactor> proactor;
    proactor.createInplace(NTSCFG_TEST_ALLOCATOR,
                          proactorConfig,
                          user,
                          NTSCFG_TEST_ALLOCATOR);

    bslmt::Barrier threadGroupBarrier(numThreads + 1);

    bslmt::ThreadGroup threadGroup(NTSCFG_TEST_ALLOCATOR);

    for (bsl::size_t threadIndex = 0; threadIndex < numThreads; ++threadIndex)
    {
        threadGroup.addThread(NTCCFG_BIND(&Framework::runProactor,
                                          proactor,
                                          &threadGroupBarrier,
                                          threadIndex));
    }

    threadGroupBarrier.wait();

    executeCallback(transport, proactor, NTSCFG_TEST_ALLOCATOR);

    threadGroup.joinAll();

    simulation->stop();
}

void StreamSocketTest::StreamSocketSession::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (d_parameters.d_useAsyncCallbacks) {
        return;
    }

    while (true) {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setMinSize(d_parameters.d_messageSize);
        receiveOptions.setMaxSize(d_parameters.d_messageSize);

        ntca::ReceiveContext receiveContext;

        error = d_streamSocket_sp->receive(&receiveContext,
                                           &d_dataReceived,
                                           receiveOptions);
        if (error) {
            if (error == ntsa::Error::e_WOULD_BLOCK) {
                break;
            }
            else if (error == ntsa::Error::e_EOF) {
                NTCI_LOG_DEBUG(
                    "Stream socket %d at %s to %s received EOF",
                    (int)(d_streamSocket_sp->handle()),
                    d_streamSocket_sp->sourceEndpoint().text().c_str(),
                    d_streamSocket_sp->remoteEndpoint().text().c_str());
                break;
            }
            else {
                NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
            }
        }

        NTCI_LOG_DEBUG("Stream socket %d at %s to %s received %d bytes",
                       (int)(d_streamSocket_sp->handle()),
                       d_streamSocket_sp->sourceEndpoint().text().c_str(),
                       d_streamSocket_sp->remoteEndpoint().text().c_str(),
                       d_dataReceived.length());

        NTSCFG_TEST_EQ(d_streamSocket_sp->transport(),
                       d_parameters.d_transport);

        NTSCFG_TEST_EQ(receiveContext.transport(),
                       d_streamSocket_sp->transport());

        NTSCFG_TEST_FALSE(receiveContext.endpoint().isNull());
        NTSCFG_TEST_FALSE(receiveContext.endpoint().value().isUndefined());

        NTSCFG_TEST_EQ(d_dataReceived.length(), d_parameters.d_messageSize);

        NTCI_LOG_DEBUG("Stream socket %d at %s to %s received message %d/%d",
                       (int)(d_streamSocket_sp->handle()),
                       d_streamSocket_sp->sourceEndpoint().text().c_str(),
                       d_streamSocket_sp->remoteEndpoint().text().c_str(),
                       (int)((d_parameters.d_numMessages -
                              d_numMessagesReceived.currentCount()) +
                             1),
                       (int)(d_parameters.d_numMessages));

        bdlbb::BlobUtil::erase(&d_dataReceived, 0, d_parameters.d_messageSize);

        d_numMessagesReceived.arrive();
    }
}

void StreamSocketTest::StreamSocketSession::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    while (d_numMessagesLeftToSend > 0) {
        bdlbb::Blob data(d_streamSocket_sp->outgoingBlobBufferFactory().get());
        ntcd::DataUtil::generateData(&data, d_parameters.d_messageSize);

        NTCI_LOG_DEBUG(
            "Stream socket %d at %s to %s sending message %d/%d",
            (int)(d_streamSocket_sp->handle()),
            d_streamSocket_sp->sourceEndpoint().text().c_str(),
            d_streamSocket_sp->remoteEndpoint().text().c_str(),
            (int)((d_parameters.d_numMessages - d_numMessagesLeftToSend) + 1),
            (int)(d_parameters.d_numMessages));

        if (d_parameters.d_useAsyncCallbacks) {
            ntci::SendCallback sendCallback =
                d_streamSocket_sp->createSendCallback(
                    NTCCFG_BIND(&StreamSocketSession::processWrite,
                                this,
                                d_streamSocket_sp,
                                NTCCFG_BIND_PLACEHOLDER_1,
                                NTCCFG_BIND_PLACEHOLDER_2),
                    d_allocator_p);

            NTSCFG_TEST_EQ(sendCallback.strand(), d_streamSocket_sp->strand());

            error = d_streamSocket_sp->send(data,
                                            ntca::SendOptions(),
                                            sendCallback);
            if (error) {
                NTSCFG_TEST_EQ(error, ntsa::Error::e_WOULD_BLOCK);
                NTCI_LOG_DEBUG(
                    "Stream socket %d at %s to %s unable to send message "
                    "%d/%d: %s",
                    (int)(d_streamSocket_sp->handle()),
                    d_streamSocket_sp->sourceEndpoint().text().c_str(),
                    d_streamSocket_sp->remoteEndpoint().text().c_str(),
                    (int)((d_parameters.d_numMessages -
                           d_numMessagesLeftToSend) +
                          1),
                    (int)(d_parameters.d_numMessages),
                    error.text().c_str());
                break;
            }

            --d_numMessagesLeftToSend;
        }
        else {
            error = d_streamSocket_sp->send(data, ntca::SendOptions());
            if (error) {
                NTSCFG_TEST_EQ(error, ntsa::Error::e_WOULD_BLOCK);
                NTCI_LOG_DEBUG(
                    "Stream socket %d at %s to %s unable to send message "
                    "%d/%d: %s",
                    (int)(d_streamSocket_sp->handle()),
                    d_streamSocket_sp->sourceEndpoint().text().c_str(),
                    d_streamSocket_sp->remoteEndpoint().text().c_str(),
                    (int)((d_parameters.d_numMessages -
                           d_numMessagesLeftToSend) +
                          1),
                    (int)(d_parameters.d_numMessages),
                    error.text().c_str());
                break;
            }

            --d_numMessagesLeftToSend;
        }
    }
}

void StreamSocketTest::StreamSocketSession::processRead(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Receiver>&     receiver,
    const bsl::shared_ptr<bdlbb::Blob>&        data,
    const ntca::ReceiveEvent&                  event)
{
    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::ReceiveEventType::e_ERROR) {
        NTSCFG_TEST_EQ(event.context().error(), ntsa::Error::e_EOF);

        NTCI_LOG_DEBUG(
            "Stream socket %d at %s to %s asynchronously received EOF",
            (int)(d_streamSocket_sp->handle()),
            d_streamSocket_sp->sourceEndpoint().text().c_str(),
            d_streamSocket_sp->remoteEndpoint().text().c_str());
    }
    else {
        NTSCFG_TEST_FALSE(event.context().error());

        NTSCFG_TEST_EQ(streamSocket->transport(), d_parameters.d_transport);

        NTSCFG_TEST_EQ(event.context().transport(), streamSocket->transport());

        NTSCFG_TEST_FALSE(event.context().endpoint().isNull());
        NTSCFG_TEST_FALSE(event.context().endpoint().value().isUndefined());

        NTSCFG_TEST_EQ(data->length(), d_parameters.d_messageSize);

        NTCI_LOG_DEBUG("Stream socket %d at %s to %s asynchronously received "
                       "message %d/%d",
                       (int)(d_streamSocket_sp->handle()),
                       d_streamSocket_sp->sourceEndpoint().text().c_str(),
                       d_streamSocket_sp->remoteEndpoint().text().c_str(),
                       (int)((d_parameters.d_numMessages -
                              d_numMessagesReceived.currentCount()) +
                             1),
                       (int)(d_parameters.d_numMessages));

        ntca::ReceiveOptions options;
        options.setMinSize(d_parameters.d_messageSize);
        options.setMaxSize(d_parameters.d_messageSize);

        ntci::ReceiveCallback receiveCallback =
            d_streamSocket_sp->createReceiveCallback(
                NTCCFG_BIND(&StreamSocketSession::processRead,
                            this,
                            d_streamSocket_sp,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3),
                d_allocator_p);

        NTSCFG_TEST_EQ(receiveCallback.strand(), d_streamSocket_sp->strand());

        ntsa::Error receiveError =
            d_streamSocket_sp->receive(options, receiveCallback);
        NTSCFG_TEST_OK(receiveError);

        d_numMessagesReceived.arrive();
    }
}

void StreamSocketTest::StreamSocketSession::processWrite(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Sender>&       sender,
    const ntca::SendEvent&                     event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::SendEventType::e_ERROR) {
        NTSCFG_TEST_EQ(event.context().error(), ntsa::Error::e_CANCELLED);

        NTCI_LOG_DEBUG(
            "Stream socket %d at %s to %s asynchronous write cancelled",
            (int)(d_streamSocket_sp->handle()),
            d_streamSocket_sp->sourceEndpoint().text().c_str(),
            d_streamSocket_sp->remoteEndpoint().text().c_str());
    }
    else {
        NTSCFG_TEST_FALSE(event.context().error());

        NTCI_LOG_DEBUG("Stream socket %d at %s to %s asynchronously sent "
                       "message %d/%d",
                       (int)(d_streamSocket_sp->handle()),
                       d_streamSocket_sp->sourceEndpoint().text().c_str(),
                       d_streamSocket_sp->remoteEndpoint().text().c_str(),
                       (int)((d_parameters.d_numMessages -
                              d_numMessagesSent.currentCount()) +
                             1),
                       (int)(d_parameters.d_numMessages));

        d_numMessagesSent.arrive();
    }
}

void StreamSocketTest::StreamSocketSession::processTimer(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        NTCI_LOG_DEBUG("Stream socket %d at %s to %s timer %d/%d has fired",
                       (int)(d_streamSocket_sp->handle()),
                       d_streamSocket_sp->sourceEndpoint().text().c_str(),
                       d_streamSocket_sp->remoteEndpoint().text().c_str(),
                       (int)((d_parameters.d_numTimers -
                              d_numTimerEvents.currentCount()) +
                             1),
                       (int)(d_parameters.d_numTimers));

        d_numTimerEvents.arrive();
    }
    else if (event.type() == ntca::TimerEventType::e_CANCELED) {
        BSLS_ASSERT(event.context().error() == ntsa::Error::e_CANCELLED);

        NTCI_LOG_DEBUG("Stream socket %d at %s to %s timer has been canceled",
                       (int)(d_streamSocket_sp->handle()),
                       d_streamSocket_sp->sourceEndpoint().text().c_str(),
                       d_streamSocket_sp->remoteEndpoint().text().c_str());

        d_numTimerEvents.arrive();
    }
}

StreamSocketTest::StreamSocketSession::StreamSocketSession(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const StreamSocketTest::Parameters&        parameters,
    bslma::Allocator*                          basicAllocator)
: d_object("StreamSocketTest::StreamSocketSession")
, d_streamSocket_sp(streamSocket)
, d_dataReceived(streamSocket->incomingBlobBufferFactory().get(),
                 basicAllocator)
, d_numTimerEvents(parameters.d_numTimers)
, d_numMessagesLeftToSend(parameters.d_numMessages)
, d_numMessagesSent(parameters.d_numMessages)
, d_numMessagesReceived(parameters.d_numMessages)
, d_parameters(parameters)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

StreamSocketTest::StreamSocketSession::~StreamSocketSession()
{
}

void StreamSocketTest::StreamSocketSession::schedule()
{
    NTCI_LOG_CONTEXT();

    bsls::TimeInterval now = d_streamSocket_sp->currentTime();

    NTCCFG_FOREACH(bsl::size_t, timerIndex, 0, d_parameters.d_numTimers)
    {
        ntca::TimerOptions timerOptions;
        timerOptions.setOneShot(true);

        ntci::TimerCallback timerCallback =
            d_streamSocket_sp->createTimerCallback(
                bdlf::MemFnUtil::memFn(&StreamSocketSession::processTimer,
                                       this),
                d_allocator_p);

        NTSCFG_TEST_EQ(timerCallback.strand(), d_streamSocket_sp->strand());

        bsl::shared_ptr<ntci::Timer> timer =
            d_streamSocket_sp->createTimer(timerOptions,
                                           timerCallback,
                                           d_allocator_p);

        bsls::TimeInterval timerDeadline = now;
        timerDeadline.addMilliseconds(timerIndex);

        NTCI_LOG_DEBUG("Stream socket %d at %s to %s starting timer %d/%d",
                       (int)(d_streamSocket_sp->handle()),
                       d_streamSocket_sp->sourceEndpoint().text().c_str(),
                       d_streamSocket_sp->remoteEndpoint().text().c_str(),
                       (int)(timerIndex + 1),
                       (int)(d_parameters.d_numTimers));

        timer->schedule(timerDeadline);
    }
}

void StreamSocketTest::StreamSocketSession::send()
{
    ntca::WriteQueueEvent event;
    event.setType(ntca::WriteQueueEventType::e_LOW_WATERMARK);

    d_streamSocket_sp->execute(
        NTCCFG_BIND(&StreamSocketSession::processWriteQueueLowWatermark,
                    this,
                    d_streamSocket_sp,
                    event));
}

void StreamSocketTest::StreamSocketSession::receive()
{
    ntsa::Error error;

    if (d_parameters.d_useAsyncCallbacks) {
        ntca::ReceiveOptions options;
        options.setMinSize(d_parameters.d_messageSize);
        options.setMaxSize(d_parameters.d_messageSize);

        ntci::ReceiveCallback receiveCallback =
            d_streamSocket_sp->createReceiveCallback(
                NTCCFG_BIND(&StreamSocketSession::processRead,
                            this,
                            d_streamSocket_sp,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3),
                d_allocator_p);

        NTSCFG_TEST_EQ(receiveCallback.strand(), d_streamSocket_sp->strand());

        error = d_streamSocket_sp->receive(options, receiveCallback);
        NTSCFG_TEST_OK(error);
    }

    d_streamSocket_sp->relaxFlowControl(ntca::FlowControlType::e_RECEIVE);
}

void StreamSocketTest::StreamSocketSession::wait()
{
    d_numTimerEvents.wait();
    if (d_parameters.d_useAsyncCallbacks) {
        d_numMessagesSent.wait();
    }
    d_numMessagesReceived.wait();
}

void StreamSocketTest::StreamSocketSession::close()
{
    ntsa::Error error;

    error = d_streamSocket_sp->shutdown(ntsa::ShutdownType::e_BOTH,
                                        ntsa::ShutdownMode::e_IMMEDIATE);
    NTSCFG_TEST_FALSE(error);

    if (d_parameters.d_useAsyncCallbacks) {
        ntci::StreamSocketCloseGuard guard(d_streamSocket_sp);
    }
    else {
        d_streamSocket_sp->close();
    }
}

ntsa::Endpoint StreamSocketTest::StreamSocketSession::sourceEndpoint() const
{
    return d_streamSocket_sp->sourceEndpoint();
}

ntsa::Endpoint StreamSocketTest::StreamSocketSession::remoteEndpoint() const
{
    return d_streamSocket_sp->remoteEndpoint();
}

void StreamSocketTest::StreamSocketManager::processStreamSocketEstablished(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket %d established",
                   (int)(streamSocket->handle()));

    bsl::shared_ptr<StreamSocketSession> streamSocketSession;
    streamSocketSession.createInplace(d_allocator_p,
                                      streamSocket,
                                      d_parameters,
                                      d_allocator_p);

    streamSocket->registerSession(streamSocketSession);

    if (d_parameters.d_readRateLimiter_sp) {
        streamSocket->setReadRateLimiter(d_parameters.d_readRateLimiter_sp);
    }
    else if (!d_parameters.d_readRate.isNull()) {
        bsl::shared_ptr<ntcs::RateLimiter> rateLimiter;
        rateLimiter.createInplace(bslma::Default::defaultAllocator(),
                                  d_parameters.d_readRate.value(),
                                  bsls::TimeInterval(1.0),
                                  d_parameters.d_readRate.value(),
                                  bsls::TimeInterval(1.0),
                                  streamSocket->currentTime());

        streamSocket->setReadRateLimiter(rateLimiter);
    }

    if (d_parameters.d_writeRateLimiter_sp) {
        streamSocket->setWriteRateLimiter(d_parameters.d_writeRateLimiter_sp);
    }
    else if (!d_parameters.d_writeRate.isNull()) {
        bsl::shared_ptr<ntcs::RateLimiter> rateLimiter;
        rateLimiter.createInplace(bslma::Default::defaultAllocator(),
                                  d_parameters.d_writeRate.value(),
                                  bsls::TimeInterval(1.0),
                                  d_parameters.d_writeRate.value(),
                                  bsls::TimeInterval(1.0),
                                  streamSocket->currentTime());

        streamSocket->setWriteRateLimiter(rateLimiter);
    }

    {
        LockGuard guard(&d_socketMapMutex);
        d_socketMap.insert(
            StreamSocketApplicationMap::value_type(streamSocket,
                                                   streamSocketSession));
    }

    streamSocketSession->receive();

    d_socketsEstablished.arrive();
}

void StreamSocketTest::StreamSocketManager::processStreamSocketClosed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket %d closed", (int)(streamSocket->handle()));

    {
        LockGuard   guard(&d_socketMapMutex);
        bsl::size_t n = d_socketMap.erase(streamSocket);
        NTSCFG_TEST_EQ(n, 1);
    }

    d_socketsClosed.arrive();
}

StreamSocketTest::StreamSocketManager::StreamSocketManager(
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    const StreamSocketTest::Parameters&   parameters,
    bslma::Allocator*                     basicAllocator)
: d_object("StreamSocketTest::StreamSocketManager")
, d_proactor_sp(proactor)
, d_metrics_sp()
, d_socketMapMutex()
, d_socketMap(basicAllocator)
, d_socketsEstablished(parameters.d_numSocketPairs * 2)
, d_socketsClosed(parameters.d_numSocketPairs * 2)
, d_parameters(parameters)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

StreamSocketTest::StreamSocketManager::~StreamSocketManager()
{
    NTSCFG_TEST_TRUE(d_socketMap.empty());
}

void StreamSocketTest::StreamSocketManager::run()
{
    ntsa::Error error;

    ntca::MonitorableRegistryConfig monitorableRegistryConfig;
    ntcm::MonitorableUtil::enableMonitorableRegistry(
        monitorableRegistryConfig);

    // Create all the stream socket pairs.

    for (bsl::size_t i = 0; i < d_parameters.d_numSocketPairs; ++i) {
        ntca::StreamSocketOptions options;
        options.setTransport(d_parameters.d_transport);
        options.setReadQueueLowWatermark(d_parameters.d_messageSize);
        options.setReadQueueHighWatermark(
            d_parameters.d_readQueueHighWatermark);
        options.setWriteQueueLowWatermark(0);
        options.setWriteQueueHighWatermark(
            d_parameters.d_writeQueueHighWatermark);
        options.setSendGreedily(false);
        options.setReceiveGreedily(false);
        options.setKeepHalfOpen(false);

        if (!d_parameters.d_sendBufferSize.isNull()) {
            options.setSendBufferSize(d_parameters.d_sendBufferSize.value());
        }

        if (!d_parameters.d_receiveBufferSize.isNull()) {
            options.setReceiveBufferSize(
                d_parameters.d_receiveBufferSize.value());
        }

        options.setTimestampIncomingData(d_parameters.d_timestampIncomingData);
        options.setTimestampOutgoingData(d_parameters.d_timestampOutgoingData);
        options.setMetrics(d_parameters.d_collectMetrics);

        if (d_parameters.d_timestampIncomingData ||
            d_parameters.d_timestampOutgoingData)
        {
            // metrics must be enabled to verify timestamping feature
            NTSCFG_TEST_TRUE(d_parameters.d_collectMetrics);
        }

        bsl::shared_ptr<ntci::Resolver> resolver;

        bsl::shared_ptr<ntcd::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntcd::StreamSocket> basicServerSocket;

        error =
            ntcd::Simulation::createStreamSocketPair(&basicClientSocket,
                                                     &basicServerSocket,
                                                     d_parameters.d_transport);
        NTSCFG_TEST_FALSE(error);

        bsl::shared_ptr<ntcp::StreamSocket> clientStreamSocket;
        clientStreamSocket.createInplace(d_allocator_p,
                                         options,
                                         resolver,
                                         d_proactor_sp,
                                         d_proactor_sp,
                                         d_metrics_sp,
                                         d_allocator_p);

        error = clientStreamSocket->registerManager(this->getSelf(this));
        NTSCFG_TEST_FALSE(error);

        error = clientStreamSocket->open(d_parameters.d_transport,
                                         basicClientSocket);
        NTSCFG_TEST_FALSE(error);

        bsl::shared_ptr<ntcp::StreamSocket> serverStreamSocket;
        serverStreamSocket.createInplace(d_allocator_p,
                                         options,
                                         resolver,
                                         d_proactor_sp,
                                         d_proactor_sp,
                                         d_metrics_sp,
                                         d_allocator_p);

        error = serverStreamSocket->registerManager(this->getSelf(this));
        NTSCFG_TEST_FALSE(error);

        error = serverStreamSocket->open(d_parameters.d_transport,
                                         basicServerSocket);
        NTSCFG_TEST_FALSE(error);
    }

    // Wait for all the stream sockets to become established.

    d_socketsEstablished.wait();

    // Start the timers for each stream socket.

    {
        LockGuard guard(&d_socketMapMutex);

        for (StreamSocketApplicationMap::const_iterator it =
                 d_socketMap.begin();
             it != d_socketMap.end();
             ++it)
        {
            const bsl::shared_ptr<StreamSocketSession>& socket = it->second;
            socket->schedule();
        }
    }

    // Send data between each stream socket pair.

    {
        LockGuard guard(&d_socketMapMutex);

        for (StreamSocketApplicationMap::const_iterator it =
                 d_socketMap.begin();
             it != d_socketMap.end();
             ++it)
        {
            const bsl::shared_ptr<StreamSocketSession>& socket = it->second;
            socket->send();
        }
    }

    // Wait for all timers to fire and all messages to be received for each
    // stream socket.

    {
        LockGuard guard(&d_socketMapMutex);

        for (StreamSocketApplicationMap::const_iterator it =
                 d_socketMap.begin();
             it != d_socketMap.end();
             ++it)
        {
            const bsl::shared_ptr<StreamSocketSession>& socket = it->second;
            socket->wait();
        }
    }

    // Validate RX and TX timestamps using metrics. Note that such validation
    // is currently only performed on Linux, because while the underlying
    // proactor implementation may supports timestamping the socket API
    // functions disallow enabling timestamping except on those platforms
    // known to natively support timestamping.

#if defined(BSLS_PLATFORM_OS_LINUX)
    {
        // If it is required to validate outgoing timestamps mechanism then it
        // is not enough to wait for all packets to be transferred. It is also
        // needed to ensure that all notifications with timestamps have been
        // delivered. At this point there is no good enough mechanism to
        // provide such synchronization.
        if (d_parameters.d_timestampOutgoingData) {
            bslmt::ThreadUtil::microSleep(0, 1);
        }

        bsl::vector<bsl::shared_ptr<ntci::Monitorable> > monitorables;
        ntcm::MonitorableUtil::loadRegisteredObjects(&monitorables);
        for (bsl::vector<bsl::shared_ptr<ntci::Monitorable> >::iterator it =
                 monitorables.begin();
             it != monitorables.end();
             ++it)
        {
            bdld::ManagedDatum stats;
            (*it)->getStats(&stats);
            const bdld::Datum& d = stats.datum();
            NTSCFG_TEST_EQ(d.type(), bdld::Datum::e_ARRAY);
            bdld::DatumArrayRef statsArray = d.theArray();

            const int baseTxDelayBeforeSchedIndex = 90;
            const int baseTxDelayInSoftwareIndex  = 95;
            const int baseTxDelayIndex            = 100;
            const int baseTxDelayBeforeAckIndex   = 105;
            const int baseRxDelayInHardwareIndex  = 110;
            const int baseRxDelayIndex            = 115;

            const int countOffset = 0;
            const int totalOffset = 1;
            const int minOffset   = 2;
            const int avgOffset   = 3;
            const int maxOffset   = 4;
            const int total       = maxOffset + 1;

            /// due to multithreaded nature of the tests it's hard to predict
            /// the exact amount of TX timestamps received. The implementation
            // of ntcp_datagramsocket does not timestamp any outgoing packet
            /// until the first TX timestamp is received from the proactor
            const double txTimestampsPercentage = 0.45;

            if (!d_parameters.d_timestampOutgoingData) {
                validateNoMetricsAvailable(statsArray,
                                           baseTxDelayBeforeSchedIndex,
                                           total);
                validateNoMetricsAvailable(statsArray,
                                           baseTxDelayInSoftwareIndex,
                                           total);
                validateNoMetricsAvailable(statsArray,
                                           baseTxDelayBeforeAckIndex,
                                           total);
            }
            else {
                validateMetricsAvailable(statsArray,
                                         baseTxDelayBeforeSchedIndex,
                                         total);
                validateMetricsAvailable(statsArray,
                                         baseTxDelayInSoftwareIndex,
                                         total);
                validateMetricsAvailable(statsArray,
                                         baseTxDelayBeforeAckIndex,
                                         total);

                NTSCFG_TEST_GE(
                    statsArray[baseTxDelayBeforeSchedIndex + countOffset]
                        .theDouble(),
                    d_parameters.d_numMessages * txTimestampsPercentage);
                NTSCFG_TEST_GT(
                    statsArray[baseTxDelayBeforeSchedIndex + totalOffset]
                        .theDouble(),
                    0);
                NTSCFG_TEST_GT(
                    statsArray[baseTxDelayBeforeSchedIndex + minOffset]
                        .theDouble(),
                    0);
                NTSCFG_TEST_GT(
                    statsArray[baseTxDelayBeforeSchedIndex + avgOffset]
                        .theDouble(),
                    0);
                NTSCFG_TEST_GT(
                    statsArray[baseTxDelayBeforeSchedIndex + maxOffset]
                        .theDouble(),
                    0);

                NTSCFG_TEST_GE(
                    statsArray[baseTxDelayInSoftwareIndex + countOffset]
                        .theDouble(),
                    d_parameters.d_numMessages * txTimestampsPercentage);
                NTSCFG_TEST_GT(
                    statsArray[baseTxDelayInSoftwareIndex + totalOffset]
                        .theDouble(),
                    0);
                NTSCFG_TEST_GT(
                    statsArray[baseTxDelayInSoftwareIndex + minOffset]
                        .theDouble(),
                    0);
                NTSCFG_TEST_GT(
                    statsArray[baseTxDelayInSoftwareIndex + avgOffset]
                        .theDouble(),
                    0);
                NTSCFG_TEST_GT(
                    statsArray[baseTxDelayInSoftwareIndex + maxOffset]
                        .theDouble(),
                    0);

                NTSCFG_TEST_GE(
                    statsArray[baseTxDelayBeforeAckIndex + countOffset]
                        .theDouble(),
                    d_parameters.d_numMessages * txTimestampsPercentage);
                NTSCFG_TEST_GT(
                    statsArray[baseTxDelayBeforeAckIndex + totalOffset]
                        .theDouble(),
                    0);
                NTSCFG_TEST_GT(
                    statsArray[baseTxDelayBeforeAckIndex + minOffset]
                        .theDouble(),
                    0);
                NTSCFG_TEST_GT(
                    statsArray[baseTxDelayBeforeAckIndex + avgOffset]
                        .theDouble(),
                    0);
                NTSCFG_TEST_GT(
                    statsArray[baseTxDelayBeforeAckIndex + maxOffset]
                        .theDouble(),
                    0);
            }
            if (!d_parameters.d_timestampIncomingData) {
                validateNoMetricsAvailable(statsArray,
                                           baseRxDelayIndex,
                                           total);
                validateNoMetricsAvailable(statsArray,
                                           baseRxDelayInHardwareIndex,
                                           total);
            }
            else {
                validateNoMetricsAvailable(statsArray,
                                           baseRxDelayInHardwareIndex,
                                           total);
                validateMetricsAvailable(statsArray, baseRxDelayIndex, total);

                NTSCFG_TEST_EQ(
                    statsArray[baseRxDelayIndex + countOffset].theDouble(),
                    d_parameters.d_numMessages);
                NTSCFG_TEST_GT(
                    statsArray[baseRxDelayIndex + totalOffset].theDouble(),
                    0);
                NTSCFG_TEST_GT(
                    statsArray[baseRxDelayIndex + minOffset].theDouble(),
                    0);
                NTSCFG_TEST_GT(
                    statsArray[baseRxDelayIndex + avgOffset].theDouble(),
                    0);
                NTSCFG_TEST_GT(
                    statsArray[baseRxDelayIndex + maxOffset].theDouble(),
                    0);
            }
        }
    }
#endif

    // Close all the stream sockets.

    {
        typedef bsl::vector<bsl::shared_ptr<StreamSocketSession> >
            StreamSocketApplicationVector;

        StreamSocketApplicationVector socketVector;
        socketVector.reserve(d_socketMap.size());

        {
            LockGuard guard(&d_socketMapMutex);

            for (StreamSocketApplicationMap::const_iterator it =
                     d_socketMap.begin();
                 it != d_socketMap.end();
                 ++it)
            {
                const bsl::shared_ptr<StreamSocketSession>& socket =
                    it->second;

                socketVector.push_back(socket);
            }
        }

        for (StreamSocketApplicationVector::const_iterator it =
                 socketVector.begin();
             it != socketVector.end();
             ++it)
        {
            const bsl::shared_ptr<StreamSocketSession>& socket = *it;
            socket->close();
        }
    }

    // Wait for all stream sockets to close.

    d_socketsClosed.wait();
}

void StreamSocketTest::verifyGenericVariation(
    ntsa::Transport::Value                transport,
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    const StreamSocketTest::Parameters&   parameters,
    bslma::Allocator*                     allocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket test starting");

    StreamSocketTest::Parameters effectiveParameters = parameters;
    effectiveParameters.d_transport                  = transport;

    bsl::shared_ptr<StreamSocketTest::StreamSocketManager> streamSocketManager;
    streamSocketManager.createInplace(allocator,
                                      proactor,
                                      effectiveParameters,
                                      allocator);

    streamSocketManager->run();
    streamSocketManager.reset();

    NTCI_LOG_DEBUG("Stream socket test complete");

    proactor->stop();
}

void StreamSocketTest::verifySendDeadlineVariation(
    ntsa::Transport::Value                transport,
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    const StreamSocketTest::Parameters&   parameters,
    bslma::Allocator*                     allocator)
{
    // Concern: Send deadlines.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket send deadline test starting");

    const int k_SEND_TIMEOUT_IN_MILLISECONDS = 1000;
    const int k_MESSAGE_A_SIZE               = 1024 * 1024 * 16;
    const int k_MESSAGE_B_SIZE               = 1024;
    const int k_MESSAGE_C_SIZE               = 1024 * 32;

    ntsa::Error                     error;
    bslmt::Semaphore                sendSemaphore;
    bslmt::Semaphore                receiveSemaphore;
    bsl::shared_ptr<ntcs::Metrics>  metrics;
    bsl::shared_ptr<ntci::Resolver> resolver;

    bsl::shared_ptr<ntcp::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntcp::StreamSocket> serverStreamSocket;
    {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);
        options.setWriteQueueHighWatermark(
            k_MESSAGE_A_SIZE + k_MESSAGE_B_SIZE + k_MESSAGE_C_SIZE);
        options.setReadQueueHighWatermark(k_MESSAGE_A_SIZE + k_MESSAGE_B_SIZE +
                                          k_MESSAGE_C_SIZE);

        options.setSendBufferSize(1024 * 32);
        options.setReceiveBufferSize(1024 * 32);

        bsl::shared_ptr<ntcd::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntcd::StreamSocket> basicServerSocket;

        error = ntcd::Simulation::createStreamSocketPair(&basicClientSocket,
                                                         &basicServerSocket,
                                                         transport);
        NTSCFG_TEST_FALSE(error);

        clientStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         proactor,
                                         proactor,
                                         metrics,
                                         allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTSCFG_TEST_FALSE(error);

        serverStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         proactor,
                                         proactor,
                                         metrics,
                                         allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTSCFG_TEST_FALSE(error);
    }

    NTCI_LOG_DEBUG("Generating message A");

    bsl::shared_ptr<bdlbb::Blob> dataA =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(dataA.get(), k_MESSAGE_A_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Generating message A: OK");

    NTCI_LOG_DEBUG("Generating message B");

    bsl::shared_ptr<bdlbb::Blob> dataB =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(dataB.get(), k_MESSAGE_B_SIZE, 0, 1);

    NTCI_LOG_DEBUG("Generating message B: OK");

    NTCI_LOG_DEBUG("Generating message C");

    bsl::shared_ptr<bdlbb::Blob> dataC =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(dataC.get(), k_MESSAGE_C_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Generating message C: OK");

    NTCI_LOG_DEBUG("Sending message A");
    {
        ntca::SendOptions sendOptions;

        error = clientStreamSocket->send(*dataA, sendOptions);
        NTSCFG_TEST_TRUE(!error);
    }

    NTCI_LOG_DEBUG("Sending message B");
    {
        bsls::TimeInterval sendTimeout;
        sendTimeout.setTotalMilliseconds(k_SEND_TIMEOUT_IN_MILLISECONDS);

        bsls::TimeInterval sendDeadline =
            clientStreamSocket->currentTime() + sendTimeout;

        ntca::SendOptions sendOptions;
        sendOptions.setDeadline(sendDeadline);

        ntci::SendCallback sendCallback =
            clientStreamSocket->createSendCallback(
                NTCCFG_BIND(&StreamSocketTest::processSendTimeout,
                            clientStreamSocket,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            bsl::string("B"),
                            ntsa::Error(ntsa::Error::e_CANCELLED),
                            &sendSemaphore),
                allocator);

        error = clientStreamSocket->send(*dataB, sendOptions, sendCallback);
        NTSCFG_TEST_TRUE(!error);
    }

    NTCI_LOG_DEBUG("Sending message C");
    {
        ntca::SendOptions sendOptions;

        error = clientStreamSocket->send(*dataC, sendOptions);
        NTSCFG_TEST_TRUE(!error);
    }

    NTCI_LOG_INFO("Waiting for message B to time out");

    sendSemaphore.wait();

    NTCI_LOG_INFO("Message B has timed out");
    NTCI_LOG_INFO("Receiving message A and C");

    {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setSize(k_MESSAGE_A_SIZE);

        ntci::ReceiveCallback receiveCallback =
            serverStreamSocket->createReceiveCallback(
                NTCCFG_BIND(&StreamSocketTest::processReceiveTimeoutOrSuccess,
                            serverStreamSocket,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            bsl::string("A"),
                            ntsa::Error(ntsa::Error::e_OK),
                            &receiveSemaphore),
                allocator);

        error = serverStreamSocket->receive(receiveOptions, receiveCallback);
        NTSCFG_TEST_OK(error);
    }

    {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setSize(k_MESSAGE_C_SIZE);

        ntci::ReceiveCallback receiveCallback =
            serverStreamSocket->createReceiveCallback(
                NTCCFG_BIND(&StreamSocketTest::processReceiveTimeoutOrSuccess,
                            serverStreamSocket,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            bsl::string("C"),
                            ntsa::Error(ntsa::Error::e_OK),
                            &receiveSemaphore),
                allocator);

        error = serverStreamSocket->receive(receiveOptions, receiveCallback);
        NTSCFG_TEST_OK(error);
    }

    receiveSemaphore.wait();
    receiveSemaphore.wait();

    {
        ntci::StreamSocketCloseGuard clientStreamSocketCloseGuard(
            clientStreamSocket);

        ntci::StreamSocketCloseGuard serverStreamSocketCloseGuard(
            serverStreamSocket);
    }

    NTCI_LOG_DEBUG("Stream socket send deadline test complete");

    proactor->stop();
}

void StreamSocketTest::verifySendCancellationVariation(
    ntsa::Transport::Value                transport,
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    const StreamSocketTest::Parameters&   parameters,
    bslma::Allocator*                     allocator)
{
    // Concern: Send cancellation.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket send cancellation test starting");

    const int k_SEND_TIMEOUT_IN_MILLISECONDS = 1000;
    const int k_MESSAGE_A_SIZE               = 1024 * 1024 * 16;
    const int k_MESSAGE_B_SIZE               = 1024;
    const int k_MESSAGE_C_SIZE               = 1024 * 32;

    ntsa::Error                     error;
    bslmt::Semaphore                sendSemaphore;
    bslmt::Semaphore                receiveSemaphore;
    bsl::shared_ptr<ntcs::Metrics>  metrics;
    bsl::shared_ptr<ntci::Resolver> resolver;

    bsl::shared_ptr<ntcp::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntcp::StreamSocket> serverStreamSocket;
    {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);
        options.setWriteQueueHighWatermark(
            k_MESSAGE_A_SIZE + k_MESSAGE_B_SIZE + k_MESSAGE_C_SIZE);
        options.setReadQueueHighWatermark(k_MESSAGE_A_SIZE + k_MESSAGE_B_SIZE +
                                          k_MESSAGE_C_SIZE);

        options.setSendBufferSize(1024 * 32);
        options.setReceiveBufferSize(1024 * 32);

        bsl::shared_ptr<ntcd::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntcd::StreamSocket> basicServerSocket;

        error = ntcd::Simulation::createStreamSocketPair(&basicClientSocket,
                                                         &basicServerSocket,
                                                         transport);
        NTSCFG_TEST_FALSE(error);

        clientStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         proactor,
                                         proactor,
                                         metrics,
                                         allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTSCFG_TEST_FALSE(error);

        serverStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         proactor,
                                         proactor,
                                         metrics,
                                         allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTSCFG_TEST_FALSE(error);
    }

    NTCI_LOG_DEBUG("Generating message A");

    bsl::shared_ptr<bdlbb::Blob> dataA =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(dataA.get(), k_MESSAGE_A_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Generating message A: OK");

    NTCI_LOG_DEBUG("Generating message B");

    bsl::shared_ptr<bdlbb::Blob> dataB =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(dataB.get(), k_MESSAGE_B_SIZE, 0, 1);

    NTCI_LOG_DEBUG("Generating message B: OK");

    NTCI_LOG_DEBUG("Generating message C");

    bsl::shared_ptr<bdlbb::Blob> dataC =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(dataC.get(), k_MESSAGE_C_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Generating message C: OK");

    NTCI_LOG_DEBUG("Sending message A");
    {
        ntca::SendOptions sendOptions;

        error = clientStreamSocket->send(*dataA, sendOptions);
        NTSCFG_TEST_TRUE(!error);
    }

    NTCI_LOG_DEBUG("Sending message B");
    {
        bsls::TimeInterval sendTimeout;
        sendTimeout.setTotalMilliseconds(k_SEND_TIMEOUT_IN_MILLISECONDS);

        bsls::TimeInterval sendDeadline =
            clientStreamSocket->currentTime() + sendTimeout;

        ntca::SendToken sendToken;
        sendToken.setValue(1);

        ntca::SendOptions sendOptions;
        sendOptions.setToken(sendToken);

        ntci::SendCallback sendCallback =
            clientStreamSocket->createSendCallback(
                NTCCFG_BIND(&StreamSocketTest::processSendCancellation,
                            clientStreamSocket,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            bsl::string("B"),
                            ntsa::Error(ntsa::Error::e_CANCELLED),
                            &sendSemaphore),
                allocator);

        error = clientStreamSocket->send(*dataB, sendOptions, sendCallback);
        NTSCFG_TEST_TRUE(!error);

        ntca::TimerOptions timerOptions;

        timerOptions.setOneShot(true);
        timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

        ntci::TimerCallback timerCallback =
            clientStreamSocket->createTimerCallback(
                bdlf::BindUtil::bind(&cancelSend,
                                     clientStreamSocket,
                                     sendToken),
                allocator);

        bsl::shared_ptr<ntci::Timer> timer =
            clientStreamSocket->createTimer(timerOptions,
                                            timerCallback,
                                            allocator);

        error = timer->schedule(sendDeadline);
        NTSCFG_TEST_FALSE(error);
    }

    NTCI_LOG_DEBUG("Sending message C");
    {
        ntca::SendOptions sendOptions;

        error = clientStreamSocket->send(*dataC, sendOptions);
        NTSCFG_TEST_TRUE(!error);
    }

    NTCI_LOG_INFO("Waiting for message B to be canceled");

    sendSemaphore.wait();

    NTCI_LOG_INFO("Message B has been canceled");
    NTCI_LOG_INFO("Receiving message A and C");

    {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setSize(k_MESSAGE_A_SIZE);

        ntci::ReceiveCallback receiveCallback =
            serverStreamSocket->createReceiveCallback(
                NTCCFG_BIND(
                    &StreamSocketTest::processReceiveCancellationOrSuccess,
                    serverStreamSocket,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    bsl::string("A"),
                    ntsa::Error(ntsa::Error::e_OK),
                    &receiveSemaphore),
                allocator);

        error = serverStreamSocket->receive(receiveOptions, receiveCallback);
        NTSCFG_TEST_OK(error);
    }

    {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setSize(k_MESSAGE_C_SIZE);

        ntci::ReceiveCallback receiveCallback =
            serverStreamSocket->createReceiveCallback(
                NTCCFG_BIND(
                    &StreamSocketTest::processReceiveCancellationOrSuccess,
                    serverStreamSocket,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    bsl::string("C"),
                    ntsa::Error(ntsa::Error::e_OK),
                    &receiveSemaphore),
                allocator);

        error = serverStreamSocket->receive(receiveOptions, receiveCallback);
        NTSCFG_TEST_OK(error);
    }

    receiveSemaphore.wait();
    receiveSemaphore.wait();

    {
        ntci::StreamSocketCloseGuard clientStreamSocketCloseGuard(
            clientStreamSocket);

        ntci::StreamSocketCloseGuard serverStreamSocketCloseGuard(
            serverStreamSocket);
    }

    NTCI_LOG_DEBUG("Stream socket send cancellation test complete");

    proactor->stop();
}

void StreamSocketTest::verifyReceiveDeadlineVariation(
    ntsa::Transport::Value                transport,
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    const StreamSocketTest::Parameters&   parameters,
    bslma::Allocator*                     allocator)
{
    // Concern: Receive deadlines.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket receive deadline test starting");

    const int k_RECEIVE_TIMEOUT_IN_MILLISECONDS = 200;

    ntsa::Error                     error;
    bslmt::Semaphore                semaphore;
    bsl::shared_ptr<ntcs::Metrics>  metrics;
    bsl::shared_ptr<ntci::Resolver> resolver;

    bsl::shared_ptr<ntcp::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntcp::StreamSocket> serverStreamSocket;
    {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntcd::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntcd::StreamSocket> basicServerSocket;

        error = ntcd::Simulation::createStreamSocketPair(&basicClientSocket,
                                                         &basicServerSocket,
                                                         transport);
        NTSCFG_TEST_FALSE(error);

        clientStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         proactor,
                                         proactor,
                                         metrics,
                                         allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTSCFG_TEST_FALSE(error);

        serverStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         proactor,
                                         proactor,
                                         metrics,
                                         allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTSCFG_TEST_FALSE(error);
    }

    bsls::TimeInterval receiveTimeout;
    receiveTimeout.setTotalMilliseconds(k_RECEIVE_TIMEOUT_IN_MILLISECONDS);

    bsls::TimeInterval receiveDeadline =
        serverStreamSocket->currentTime() + receiveTimeout;

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setDeadline(receiveDeadline);

    ntci::ReceiveCallback receiveCallback =
        serverStreamSocket->createReceiveCallback(
            NTCCFG_BIND(&StreamSocketTest::processReceiveTimeout,
                        serverStreamSocket,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        &semaphore),
            allocator);

    error = serverStreamSocket->receive(receiveOptions, receiveCallback);
    NTSCFG_TEST_OK(error);

    semaphore.wait();

    {
        ntci::StreamSocketCloseGuard clientStreamSocketCloseGuard(
            clientStreamSocket);

        ntci::StreamSocketCloseGuard serverStreamSocketCloseGuard(
            serverStreamSocket);
    }

    NTCI_LOG_DEBUG("Stream socket receive deadline test complete");

    proactor->stop();
}

void StreamSocketTest::verifyReceiveCancellationVariation(
    ntsa::Transport::Value                transport,
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    const StreamSocketTest::Parameters&   parameters,
    bslma::Allocator*                     allocator)
{
    // Concern: Receive cancellation.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket receive cancellation test starting");

    const int k_RECEIVE_TIMEOUT_IN_MILLISECONDS = 200;

    ntsa::Error                     error;
    bslmt::Semaphore                semaphore;
    bsl::shared_ptr<ntcs::Metrics>  metrics;
    bsl::shared_ptr<ntci::Resolver> resolver;

    bsl::shared_ptr<ntcp::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntcp::StreamSocket> serverStreamSocket;
    {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntcd::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntcd::StreamSocket> basicServerSocket;

        error = ntcd::Simulation::createStreamSocketPair(&basicClientSocket,
                                                         &basicServerSocket,
                                                         transport);
        NTSCFG_TEST_FALSE(error);

        clientStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         proactor,
                                         proactor,
                                         metrics,
                                         allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTSCFG_TEST_FALSE(error);

        serverStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         proactor,
                                         proactor,
                                         metrics,
                                         allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTSCFG_TEST_FALSE(error);
    }

    bsls::TimeInterval receiveTimeout;
    receiveTimeout.setTotalMilliseconds(k_RECEIVE_TIMEOUT_IN_MILLISECONDS);

    bsls::TimeInterval receiveDeadline =
        serverStreamSocket->currentTime() + receiveTimeout;

    ntca::ReceiveToken receiveToken;
    receiveToken.setValue(1);

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setToken(receiveToken);

    ntci::ReceiveCallback receiveCallback =
        serverStreamSocket->createReceiveCallback(
            NTCCFG_BIND(&StreamSocketTest::processReceiveCancellation,
                        serverStreamSocket,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        &semaphore),
            allocator);

    error = serverStreamSocket->receive(receiveOptions, receiveCallback);
    NTSCFG_TEST_OK(error);

    ntca::TimerOptions timerOptions;

    timerOptions.setOneShot(true);
    timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    ntci::TimerCallback timerCallback =
        serverStreamSocket->createTimerCallback(
            bdlf::BindUtil::bind(&cancelReceive,
                                 serverStreamSocket,
                                 receiveToken),
            allocator);

    bsl::shared_ptr<ntci::Timer> timer =
        serverStreamSocket->createTimer(timerOptions,
                                        timerCallback,
                                        allocator);

    error = timer->schedule(receiveDeadline);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    {
        ntci::StreamSocketCloseGuard clientStreamSocketCloseGuard(
            clientStreamSocket);

        ntci::StreamSocketCloseGuard serverStreamSocketCloseGuard(
            serverStreamSocket);
    }

    NTCI_LOG_DEBUG("Stream socket receive cancellation test complete");

    proactor->stop();
}

void StreamSocketTest::processSendTimeout(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Sender>&       sender,
    const ntca::SendEvent&                     event,
    const bsl::string&                         name,
    const ntsa::Error&                         error,
    bslmt::Semaphore*                          semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing send event type %s: %s",
                   ntca::SendEventType::toString(event.type()),
                   event.context().error().text().c_str());

    if (error) {
        NTCI_LOG_INFO("Message %s has timed out", name.c_str());
        NTSCFG_TEST_EQ(event.type(), ntca::SendEventType::e_ERROR);
        NTSCFG_TEST_EQ(event.context().error(), ntsa::Error::e_WOULD_BLOCK);
    }
    else {
        NTCI_LOG_INFO("Message %s has been sent", name.c_str());
    }

    semaphore->post();
}

void StreamSocketTest::processSendCancellation(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Sender>&       sender,
    const ntca::SendEvent&                     event,
    const bsl::string&                         name,
    const ntsa::Error&                         error,
    bslmt::Semaphore*                          semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing send event type %s: %s",
                   ntca::SendEventType::toString(event.type()),
                   event.context().error().text().c_str());

    if (error) {
        NTCI_LOG_INFO("Message %s has been canceled", name.c_str());
        NTSCFG_TEST_EQ(event.type(), ntca::SendEventType::e_ERROR);
        NTSCFG_TEST_EQ(event.context().error(), ntsa::Error::e_CANCELLED);
    }
    else {
        NTCI_LOG_INFO("Message %s has been sent", name.c_str());
    }

    semaphore->post();
}

void StreamSocketTest::processReceiveTimeout(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Receiver>&     receiver,
    const bsl::shared_ptr<bdlbb::Blob>&        data,
    const ntca::ReceiveEvent&                  event,
    bslmt::Semaphore*                          semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing receive event type %s: %s",
                   ntca::ReceiveEventType::toString(event.type()),
                   event.context().error().text().c_str());

    NTSCFG_TEST_EQ(event.type(), ntca::ReceiveEventType::e_ERROR);
    NTSCFG_TEST_EQ(event.context().error(), ntsa::Error::e_WOULD_BLOCK);

    semaphore->post();
}

void StreamSocketTest::processReceiveTimeoutOrSuccess(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Receiver>&     receiver,
    const bsl::shared_ptr<bdlbb::Blob>&        data,
    const ntca::ReceiveEvent&                  event,
    const bsl::string&                         name,
    const ntsa::Error&                         error,
    bslmt::Semaphore*                          semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing receive event type %s: %s",
                   ntca::ReceiveEventType::toString(event.type()),
                   event.context().error().text().c_str());

    if (error) {
        NTCI_LOG_INFO("Message %s has timed out", name.c_str());
        NTSCFG_TEST_EQ(event.type(), ntca::ReceiveEventType::e_ERROR);
        NTSCFG_TEST_EQ(event.context().error(), ntsa::Error::e_WOULD_BLOCK);
    }
    else {
        NTCI_LOG_INFO("Message %s has been received", name.c_str());

        NTCI_LOG_DEBUG("Comparing message %s", name.c_str());

        bsl::size_t position = 0;
        for (int dataBufferIndex = 0; dataBufferIndex < data->numDataBuffers();
             ++dataBufferIndex)
        {
            const bdlbb::BlobBuffer& dataBuffer =
                data->buffer(dataBufferIndex);

            const char* dataPtr = dataBuffer.data();
            int dataSize        = dataBufferIndex == data->numDataBuffers() - 1
                                      ? data->lastDataBufferLength()
                                      : dataBuffer.size();

            for (int dataByteIndex = 0; dataByteIndex < dataSize;
                 ++dataByteIndex)
            {
                char e = ntcd::DataUtil::generateByte(position, 0);
                char f = dataPtr[dataByteIndex];

                if (e != f) {
                    // bsl::cout << "Message:\n"
                    //           << bdlbb::BlobUtilHexDumper(
                    //                                data.get(), 0, 256)
                    //           << bsl::endl;

                    NTCI_LOG_ERROR(
                        "Unexpected byte found at position %zu relative "
                        "offset %d: expected '%c', found '%c'",
                        position,
                        dataByteIndex,
                        e,
                        f);
                }

                NTSCFG_TEST_EQ(f, e);
                ++position;
            }
        }

        NTCI_LOG_DEBUG("Comparing message %s: OK", name.c_str());
    }

    semaphore->post();
}

void StreamSocketTest::processReceiveCancellation(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Receiver>&     receiver,
    const bsl::shared_ptr<bdlbb::Blob>&        data,
    const ntca::ReceiveEvent&                  event,
    bslmt::Semaphore*                          semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing receive event type %s: %s",
                   ntca::ReceiveEventType::toString(event.type()),
                   event.context().error().text().c_str());

    NTSCFG_TEST_EQ(event.type(), ntca::ReceiveEventType::e_ERROR);
    NTSCFG_TEST_EQ(event.context().error(), ntsa::Error::e_CANCELLED);

    semaphore->post();
}

void StreamSocketTest::processReceiveCancellationOrSuccess(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Receiver>&     receiver,
    const bsl::shared_ptr<bdlbb::Blob>&        data,
    const ntca::ReceiveEvent&                  event,
    const bsl::string&                         name,
    const ntsa::Error&                         error,
    bslmt::Semaphore*                          semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing receive event type %s: %s",
                   ntca::ReceiveEventType::toString(event.type()),
                   event.context().error().text().c_str());

    if (error) {
        NTCI_LOG_INFO("Message %s has been canceled", name.c_str());
        NTSCFG_TEST_EQ(event.type(), ntca::ReceiveEventType::e_ERROR);
        NTSCFG_TEST_EQ(event.context().error(), ntsa::Error::e_CANCELLED);
    }
    else {
        NTCI_LOG_INFO("Message %s has been received", name.c_str());

        NTCI_LOG_DEBUG("Comparing message %s", name.c_str());

        bsl::size_t position = 0;
        for (int dataBufferIndex = 0; dataBufferIndex < data->numDataBuffers();
             ++dataBufferIndex)
        {
            const bdlbb::BlobBuffer& dataBuffer =
                data->buffer(dataBufferIndex);

            const char* dataPtr = dataBuffer.data();
            int dataSize        = dataBufferIndex == data->numDataBuffers() - 1
                                      ? data->lastDataBufferLength()
                                      : dataBuffer.size();

            for (int dataByteIndex = 0; dataByteIndex < dataSize;
                 ++dataByteIndex)
            {
                char e = ntcd::DataUtil::generateByte(position, 0);
                char f = dataPtr[dataByteIndex];

                if (e != f) {
                    // bsl::cout << "Message:\n"
                    //           << bdlbb::BlobUtilHexDumper(
                    //                                data.get(), 0, 256)
                    //           << bsl::endl;

                    NTCI_LOG_ERROR(
                        "Unexpected byte found at position %zu relative "
                        "offset %d: expected '%c', found '%c'",
                        position,
                        dataByteIndex,
                        e,
                        f);
                }

                NTSCFG_TEST_EQ(f, e);
                ++position;
            }
        }

        NTCI_LOG_DEBUG("Comparing message %s: OK", name.c_str());
    }

    semaphore->post();
}

void StreamSocketTest::validateNoMetricsAvailable(
    const bdld::DatumArrayRef& metrics,
    int                        base,
    int                        number)
{
    NTSCFG_TEST_GE(metrics.length(), base + number);
    for (int i = base; i < base + number; ++i) {
        NTSCFG_TEST_EQ(metrics[i].type(), bdld::Datum::e_NIL);
    }
}

void StreamSocketTest::validateMetricsAvailable(
    const bdld::DatumArrayRef& metrics,
    int                        base,
    int                        number)
{
    NTSCFG_TEST_GE(metrics.length(), base + number);
    for (int i = base; i < base + number; ++i) {
        NTSCFG_TEST_EQ(metrics[i].type(), bdld::Datum::e_DOUBLE);
    }
}

void StreamSocketTest::cancelSend(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::SendToken                      token)
{
    ntsa::Error error = streamSocket->cancel(token);
    NTSCFG_TEST_FALSE(error);
}

void StreamSocketTest::cancelReceive(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReceiveToken                   token)
{
    ntsa::Error error = streamSocket->cancel(token);
    NTSCFG_TEST_FALSE(error);
}

ntsa::Endpoint StreamSocketTest::any(ntsa::Transport::Value transport)
{
    ntsa::Endpoint endpoint;

    switch (transport) {
    case ntsa::Transport::e_TCP_IPV4_STREAM:
    case ntsa::Transport::e_UDP_IPV4_DATAGRAM:
        endpoint.makeIp(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0));
        break;
    case ntsa::Transport::e_TCP_IPV6_STREAM:
    case ntsa::Transport::e_UDP_IPV6_DATAGRAM:
        endpoint.makeIp(ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), 0));
        break;
    case ntsa::Transport::e_LOCAL_STREAM:
    case ntsa::Transport::e_LOCAL_DATAGRAM: {
        ntsa::LocalName   localName;
        const ntsa::Error error = ntsa::LocalName::generateUnique(&localName);
        BSLS_ASSERT_OPT(!error);

        endpoint.makeLocal(localName);
        break;
    }
    default:
        NTCCFG_UNREACHABLE();
    }

    return endpoint;
}

NTSCFG_TEST_FUNCTION(ntcp::StreamSocketTest::verifyBreathing)
{
    // Concern: Breathing test.

    StreamSocketTest::Parameters parameters;
    parameters.d_numTimers         = 0;
    parameters.d_numSocketPairs    = 1;
    parameters.d_numMessages       = 1;
    parameters.d_messageSize       = 32;
    parameters.d_useAsyncCallbacks = false;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::StreamSocketTest::verifyBreathingAsync)
{
    // Concern: Breathing test using asynchronous callbacks.

    StreamSocketTest::Parameters parameters;
    parameters.d_numTimers         = 0;
    parameters.d_numSocketPairs    = 1;
    parameters.d_numMessages       = 1;
    parameters.d_messageSize       = 32;
    parameters.d_useAsyncCallbacks = true;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::StreamSocketTest::verifyStress)
{
    // Concern: Stress test using the read queue low watermark.

    // The test currently fails sporadically on Linux on CI build machines
    // with "Assertion failed: !d_chronology_sp->hasAnyDeferred()".
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0
    StreamSocketTest::Parameters parameters;
    parameters.d_numTimers         = 100;
    parameters.d_numSocketPairs    = 100;
    parameters.d_numMessages       = 32;
    parameters.d_messageSize       = 1024;
    parameters.d_useAsyncCallbacks = false;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
#endif
}

NTSCFG_TEST_FUNCTION(ntcp::StreamSocketTest::verifyStressAsync)
{
    // Concern: Stress test using asynchronous callbacks.

    // The test currently fails sporadically on Linux on CI build machines
    // with "Assertion failed: !d_chronology_sp->hasAnyDeferred()".
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0
    StreamSocketTest::Parameters parameters;
    parameters.d_numTimers         = 100;
    parameters.d_numSocketPairs    = 100;
    parameters.d_numMessages       = 32;
    parameters.d_messageSize       = 1024;
    parameters.d_useAsyncCallbacks = true;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
#endif
}

NTSCFG_TEST_FUNCTION(
    ntcp::StreamSocketTest::verifyMinimalReadQueueHighWatermark)
{
    // Concern: Minimal read queue high watermark.

    StreamSocketTest::Parameters parameters;
    parameters.d_numTimers              = 0;
    parameters.d_numSocketPairs         = 1;
    parameters.d_numMessages            = 100;
    parameters.d_messageSize            = 1024 * 32;
    parameters.d_useAsyncCallbacks      = false;
    parameters.d_readQueueHighWatermark = 1;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(
    ntcp::StreamSocketTest::verifyMinimalReadQueueHighWatermarkAsync)
{
    // Concern: Minimal read queue high watermark using asynchronous callbacks.

    StreamSocketTest::Parameters parameters;
    parameters.d_numTimers              = 0;
    parameters.d_numSocketPairs         = 1;
    parameters.d_numMessages            = 100;
    parameters.d_messageSize            = 1024 * 32;
    parameters.d_useAsyncCallbacks      = true;
    parameters.d_readQueueHighWatermark = 1;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(
    ntcp::StreamSocketTest::verifyMinimalWriteQueueHighWatermark)
{
    // Concern: Minimal write queue high watermark.

    StreamSocketTest::Parameters parameters;
    parameters.d_numTimers               = 0;
    parameters.d_numSocketPairs          = 1;
    parameters.d_numMessages             = 100;
    parameters.d_messageSize             = 1024 * 32;
    parameters.d_useAsyncCallbacks       = false;
    parameters.d_writeQueueHighWatermark = 1;
    parameters.d_sendBufferSize          = 32 * 1024;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(
    ntcp::StreamSocketTest::verifyMinimalWriteQueueHighWatermarkAsync)
{
    // Concern: Minimal write queue high watermark using asynchronous
    // callbacks.

    StreamSocketTest::Parameters parameters;
    parameters.d_numTimers               = 0;
    parameters.d_numSocketPairs          = 1;
    parameters.d_numMessages             = 100;
    parameters.d_messageSize             = 1024 * 32;
    parameters.d_useAsyncCallbacks       = true;
    parameters.d_writeQueueHighWatermark = 1;
    parameters.d_sendBufferSize          = 32 * 1024;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::StreamSocketTest::verifySendRateLimiting)
{
    // Concern: Rate limit copying to the send buffer.

    StreamSocketTest::Parameters parameters;
    parameters.d_numTimers         = 0;
    parameters.d_numSocketPairs    = 1;
    parameters.d_numMessages       = 1;
    parameters.d_messageSize       = 32 * 1024 * 4;
    parameters.d_useAsyncCallbacks = false;
    parameters.d_writeRate         = 32 * 1024;
    parameters.d_sendBufferSize    = 32 * 1024;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::StreamSocketTest::verifySendRateLimitingAsync)
{
    // Concern: Rate limit copying to the send buffer using asynchronous
    // callbacks.

    StreamSocketTest::Parameters parameters;
    parameters.d_numTimers         = 0;
    parameters.d_numSocketPairs    = 1;
    parameters.d_numMessages       = 1;
    parameters.d_messageSize       = 32 * 1024 * 4;
    parameters.d_useAsyncCallbacks = true;
    parameters.d_writeRate         = 32 * 1024;
    parameters.d_sendBufferSize    = 32 * 1024;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::StreamSocketTest::verifyReceiveRateLimiting)
{
#if !defined(BSLS_PLATFORM_OS_AIX)

    // Concern: Rate limit copying from the receive buffer.

    StreamSocketTest::Parameters parameters;
    parameters.d_numTimers         = 0;
    parameters.d_numSocketPairs    = 1;
    parameters.d_numMessages       = 1;
    parameters.d_messageSize       = 32 * 1024 * 4;
    parameters.d_useAsyncCallbacks = false;
    parameters.d_readRate          = 32 * 1024;
    parameters.d_receiveBufferSize = 32 * 1024;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));

#endif
}

NTSCFG_TEST_FUNCTION(ntcp::StreamSocketTest::verifyReceiveRateLimitingAsync)
{
    // Concern: Rate limit copying from the receive buffer using
    // asynchronous callbacks.

    StreamSocketTest::Parameters parameters;
    parameters.d_numTimers         = 0;
    parameters.d_numSocketPairs    = 1;
    parameters.d_numMessages       = 1;
    parameters.d_messageSize       = 32 * 1024 * 4;
    parameters.d_useAsyncCallbacks = true;
    parameters.d_readRate          = 32 * 1024;
    parameters.d_receiveBufferSize = 32 * 1024;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::StreamSocketTest::verifySendDeadline)
{
    StreamSocketTest::Parameters parameters;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifySendDeadlineVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::StreamSocketTest::verifyReceiveDeadline)
{
    StreamSocketTest::Parameters parameters;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifyReceiveDeadlineVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::StreamSocketTest::verifySendCancellation)
{
    StreamSocketTest::Parameters parameters;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifySendCancellationVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::StreamSocketTest::verifyReceiveCancellation)
{
    StreamSocketTest::Parameters parameters;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifyReceiveCancellationVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(
    ntcp::StreamSocketTest::verifyWriteQueueHighWatermarkOverride)
{
#if 0
    // The test is disabled due to the implementation being cloned from
    // the equivalent test for ntcr::StreamSocket, but with white-box
    // assumptions that are not true for how ntcp::StreamSocket is implemented,
    // with respect to how data is not necessarily synchronously copied to
    // the send buffer of the simulated socket.

    // Concern: Write queue high watermark event can be overriden on a
    //          per-send basis.
    //
    // Plan: Run a simulation to be able to control when data is transferred
    //       through two sockets. Create a client socket and a server socket.
    //       Configure the client socket send buffer and write queue high
    //       watermark to specific, known sizes. Send data from the client
    //       socket to the server socket so that the client socket send buffer
    //       is exactly and completely filled. Send data from the client
    //       socket to the server socket so that half the write queue is
    //       filled. Send data from the client socket to the server socket
    //       with a per-send write queue high watermark override set to less
    //       than the current size of the write queue, and ensure the write
    //       fails and a high watermark event is announced (without the
    //       per-send write queue high watermark, the send would succeed and
    //       breach the high watermark but no high watermark event would be
    //       announced.)

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    const bsl::size_t k_BLOB_BUFFER_SIZE           = 4096;
    const bsl::size_t k_SEND_BUFFER_SIZE           = 32;
    const bsl::size_t k_WRITE_QUEUE_HIGH_WATERMARK = 64;

    ntsa::Error error;

    // Create and start the simulation.

    bsl::shared_ptr<ntcd::Simulation> simulation;
    simulation.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Create a proactor.

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(NTSCFG_TEST_ALLOCATOR,
                           k_BLOB_BUFFER_SIZE,
                           k_BLOB_BUFFER_SIZE,
                           NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntcs::User> user;
    user.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);
    user->setDataPool(dataPool);

    ntca::ProactorConfig proactorConfig;
    proactorConfig.setMetricName("test");
    proactorConfig.setMinThreads(1);
    proactorConfig.setMaxThreads(1);

    bsl::shared_ptr<ntcd::Proactor> proactor;
    proactor.createInplace(NTSCFG_TEST_ALLOCATOR,
                          proactorConfig,
                          user,
                          NTSCFG_TEST_ALLOCATOR);

    // Register this thread as the thread that will wait on the proactor.

    ntci::Waiter waiter = proactor->registerWaiter(ntca::WaiterOptions());

    bsl::shared_ptr<ntci::Resolver> resolver;
    bsl::shared_ptr<ntcs::Metrics>  metrics;

    // Create a pair of connected, non-blocking stream sockets using the
    // simulation.

    bsl::shared_ptr<ntcd::StreamSocket> basicClientSocket;
    bsl::shared_ptr<ntcd::StreamSocket> basicServerSocket;

    error = ntcd::Simulation::createStreamSocketPair(
        &basicClientSocket,
        &basicServerSocket,
        ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_FALSE(error);

    // Create a stream socket for the client with a specific send buffer
    // size and write queue high watermark.

    ntca::StreamSocketOptions clientStreamSocketOptions;
    clientStreamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
    clientStreamSocketOptions.setSendBufferSize(k_SEND_BUFFER_SIZE);
    clientStreamSocketOptions.setWriteQueueHighWatermark(
        k_WRITE_QUEUE_HIGH_WATERMARK);

    bsl::shared_ptr<ntcp::StreamSocket> clientStreamSocket;
    clientStreamSocket.createInplace(NTSCFG_TEST_ALLOCATOR,
                                     clientStreamSocketOptions,
                                     resolver,
                                     proactor,
                                     proactor,
                                     metrics,
                                     NTSCFG_TEST_ALLOCATOR);

    // Register a session to process the events that passively
    // occur during the operation of the client stream socket.

    bsl::shared_ptr<ntcu::StreamSocketEventQueue> clientStreamSocketEventQueue;

    clientStreamSocketEventQueue.createInplace(NTSCFG_TEST_ALLOCATOR,
                                               NTSCFG_TEST_ALLOCATOR);
    clientStreamSocketEventQueue->show(
        ntca::WriteQueueEventType::e_HIGH_WATERMARK);

    error = clientStreamSocket->registerSession(clientStreamSocketEventQueue);
    NTSCFG_TEST_FALSE(error);

    error = clientStreamSocket->open(ntsa::Transport::e_TCP_IPV4_STREAM,
                                     basicClientSocket);
    NTSCFG_TEST_FALSE(error);

    // Create a stream socket for the server.

    ntca::StreamSocketOptions serverStreamSocketOptions;
    serverStreamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntcp::StreamSocket> serverStreamSocket;
    serverStreamSocket.createInplace(NTSCFG_TEST_ALLOCATOR,
                                     serverStreamSocketOptions,
                                     resolver,
                                     proactor,
                                     proactor,
                                     metrics,
                                     NTSCFG_TEST_ALLOCATOR);

    // Register a session to process the events that passively
    // occur during the operation of the client stream socket.

    bsl::shared_ptr<ntcu::StreamSocketEventQueue> serverStreamSocketEventQueue;

    serverStreamSocketEventQueue.createInplace(NTSCFG_TEST_ALLOCATOR,
                                               NTSCFG_TEST_ALLOCATOR);

    error = serverStreamSocket->registerSession(serverStreamSocketEventQueue);
    NTSCFG_TEST_FALSE(error);

    // Open the the server stream socket.

    error = serverStreamSocket->open(ntsa::Transport::e_TCP_IPV4_STREAM,
                                     basicServerSocket);
    NTSCFG_TEST_FALSE(error);

    // Send data from the client stream socket to the server
    // stream socket to fill up the client stream socket send
    // buffer.

    {
        bsl::shared_ptr<bdlbb::Blob> blob =
            clientStreamSocket->createOutgoingBlob();

        ntcd::DataUtil::generateData(blob.get(), k_SEND_BUFFER_SIZE);

        error = clientStreamSocket->send(*blob, ntca::SendOptions());
        NTSCFG_TEST_FALSE(error);
    }

    // Send data from the client stream socket to the server
    // stream socket to fill up half the client stream socket
    // write queue.

    {
        bsl::shared_ptr<bdlbb::Blob> blob =
            clientStreamSocket->createOutgoingBlob();

        ntcd::DataUtil::generateData(blob.get(),
                                     k_WRITE_QUEUE_HIGH_WATERMARK / 2);

        error = clientStreamSocket->send(*blob, ntca::SendOptions());
        NTSCFG_TEST_FALSE(error);
    }

    // Now the client stream socket send buffer should be entirely
    // full and the write queue should be half full.

    NTSCFG_TEST_EQ(clientStreamSocket->writeQueueSize(),
                   k_WRITE_QUEUE_HIGH_WATERMARK / 2);

    // If we were to write anything now, that write would succeed
    // and be enqueued to the write queue, since the write queue
    // high watermark is not yet breached. Send a single byte
    // but apply a send operation-specific write queue high
    // watermark to something known to be less than the current
    // write queue size, to force a high watermark event.

    {
        bsl::shared_ptr<bdlbb::Blob> blob =
            clientStreamSocket->createOutgoingBlob();

        ntcd::DataUtil::generateData(blob.get(), 1);

        ntca::SendOptions sendOptions;
        sendOptions.setHighWatermark(k_WRITE_QUEUE_HIGH_WATERMARK / 4);

        error = clientStreamSocket->send(*blob, sendOptions);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_WOULD_BLOCK));
    }

    // Wait for the announcement of the write queue high watermark event.

    while (true) {
        ntca::WriteQueueEvent writeQueueEvent;
        error = clientStreamSocketEventQueue->wait(
            &writeQueueEvent,
            ntca::WriteQueueEventType::e_HIGH_WATERMARK,
            bsls::TimeInterval());
        if (error) {
            simulation->step(true);
            proactor->poll(waiter);
            continue;
        }

        break;
    }

    // Close the client and server.

    clientStreamSocket->close();
    serverStreamSocket->close();

    // Step through the simulation to process the asynchronous closure
    // of each socket.

    simulation->step(true);
    proactor->poll(waiter);

    // Deregister the waiter.

    proactor->deregisterWaiter(waiter);
#endif
}

NTSCFG_TEST_FUNCTION(ntcp::StreamSocketTest::verifyIncomingTimestamps)
{
    // Concern: RX timestamping test.

    StreamSocketTest::Parameters parameters;
    parameters.d_numTimers             = 0;
    parameters.d_numSocketPairs        = 1;
    parameters.d_numMessages           = 100;
    parameters.d_messageSize           = 32;
    parameters.d_useAsyncCallbacks     = false;
    parameters.d_timestampIncomingData = true;
    parameters.d_collectMetrics        = true;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::StreamSocketTest::verifyOutgoingTimestamps)
{
    // Concern: TX timestamping test.
#if 0
    // The test is disabled due to its flaky nature
    StreamSocketTest::Parameters parameters;
    parameters.d_numTimers             = 0;
    parameters.d_numSocketPairs        = 1;
    parameters.d_numMessages           = 100;
    parameters.d_messageSize           = 32;
    parameters.d_useAsyncCallbacks     = false;
    parameters.d_timestampOutgoingData = true;
    parameters.d_collectMetrics        = true;

    StreamSocketTest::Framework::execute(
        NTCCFG_BIND(&StreamSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
#endif
}

}  // close namespace ntcp
}  // close namespace BloombergLP
