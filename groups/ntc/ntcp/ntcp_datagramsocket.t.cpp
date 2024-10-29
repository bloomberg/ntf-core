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
BSLS_IDENT_RCSID(ntcp_datagramsocket_t_cpp, "$Id$ $CSID$")

#include <ntcp_datagramsocket.h>

#include <ntccfg_bind.h>
#include <ntccfg_limits.h>
#include <ntcd_datautil.h>
#include <ntcd_encryption.h>
#include <ntcd_proactor.h>
#include <ntcd_simulation.h>
#include <ntci_log.h>
#include <ntcs_monitorable.h>
#include <ntcs_datapool.h>
#include <ntcs_ratelimiter.h>
#include <ntcs_user.h>
#include <ntsa_error.h>
#include <ntsa_transport.h>

using namespace BloombergLP;

// Uncomment to test a particular style of socket-to-thread load balancing,
// instead of both static and dynamic load balancing.
// #define NTCP_DATAGRAM_SOCKET_TEST_DYNAMIC_LOAD_BALANCING true

namespace BloombergLP {
namespace ntcp {

// Provide tests for 'ntcp::DatagramSocket'.
class DatagramSocketTest
{
    /// Provide a test case execution framework.
    class Framework;

    /// Describe test parameters.
    class Parameters;

    /// Provide a datagram socket session for this test driver.
    class DatagramSocketSession;

    /// Provide a datagram socket manager for this test driver.
    class DatagramSocketManager;

    // Execute the concern with the specified 'parameters' for the specified
    // 'transport' using the specified 'proactor'.
    static void verifyGenericVariation(
        ntsa::Transport::Value                 transport,
        const bsl::shared_ptr<ntci::Proactor>& proactor,
        const DatagramSocketTest::Parameters&  parameters,
        bslma::Allocator*                      allocator);

    // Execute the concern with the specified 'parameters' for the specified
    // 'transport' using the specified 'proactor'.
    static void verifyReceiveDeadlineVariation(
        ntsa::Transport::Value                 transport,
        const bsl::shared_ptr<ntci::Proactor>& proactor,
        const DatagramSocketTest::Parameters&  parameters,
        bslma::Allocator*                      allocator);

    // Execute the concern with the specified 'parameters' for the specified
    // 'transport' using the specified 'proactor'.
    static void verifyReceiveCancellationVariation(
        ntsa::Transport::Value                 transport,
        const bsl::shared_ptr<ntci::Proactor>& proactor,
        const DatagramSocketTest::Parameters&  parameters,
        bslma::Allocator*                      allocator);

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

    /// Return an endpoint representing a suitable address to which to
    /// bind a socket of the specified 'transport' type for use by this
    /// test driver.
    static ntsa::Endpoint any(ntsa::Transport::Value transport);

    // Cancel the receive operation on the specified 'datagramSocket'
    // identified by the specified 'token'.
    static void cancelReceive(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ReceiveToken                     token);

    // Process the expected timeout of a receive operation.
    static void processReceiveTimeout(
        const bsl::shared_ptr<ntci::Receiver>& receiver,
        const bsl::shared_ptr<bdlbb::Blob>&    data,
        const ntca::ReceiveEvent&              event,
        bslmt::Semaphore*                      semaphore);

    // Process the expected cancellation of a receive operation.
    static void processReceiveCancellation(
        const bsl::shared_ptr<ntci::Receiver>& receiver,
        const bsl::shared_ptr<bdlbb::Blob>&    data,
        const ntca::ReceiveEvent&              event,
        bslmt::Semaphore*                      semaphore);

  public:
    // TODO
    static void verifyBreathing();

    // TODO
    static void verifyBreathingAsync();

    // TODO
    static void verifyStress();

    // TODO
    static void verifyStressAsync();

    // TODO
    static void verifyIncomingTimestamps();

    // TODO
    static void verifyOutgoingTimestamps();

    // TODO
    static void verifyReceiveDeadline();

    // TODO
    static void verifyReceiveCancellation();
};

/// Provide a test case execution framework.
class DatagramSocketTest::Framework
{
  private:
    /// Run a thread identified by the specified 'threadIndex' that waits
    /// on the specified 'barrier' then drives the specified 'proactor' until
    /// it is stopped.
    static void runProactor(const bsl::shared_ptr<ntci::Proactor>& proactor,
                            bslmt::Barrier*                        barrier,
                            bsl::size_t threadIndex);

  public:
    /// Define a type alias for the function implementing a
    /// test case driven by this test framework.
    typedef NTCCFG_FUNCTION(ntsa::Transport::Value                 transport,
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
class DatagramSocketTest::Parameters
{
  public:
    ntsa::Transport::Value             d_transport;
    bsl::size_t                        d_numSocketPairs;
    bsl::size_t                        d_numTimers;
    bsl::size_t                        d_numMessages;
    bsl::size_t                        d_messageSize;
    bsl::shared_ptr<bdlbb::Blob>       d_message_sp;
    bsl::shared_ptr<ntci::RateLimiter> d_readRateLimiter_sp;
    bsl::size_t                        d_readQueueHighWatermark;
    bsl::shared_ptr<ntci::RateLimiter> d_writeRateLimiter_sp;
    bsl::size_t                        d_writeQueueHighWatermark;
    bdlb::NullableValue<bsl::size_t>   d_sendBufferSize;
    bdlb::NullableValue<bsl::size_t>   d_receiveBufferSize;
    bool                               d_useAsyncCallbacks;
    bool                               d_tolerateDataLoss;
    bool                               d_timestampIncomingData;
    bool                               d_timestampOutgoingData;
    bool                               d_collectMetrics;

    Parameters()
    : d_transport(ntsa::Transport::e_UDP_IPV4_DATAGRAM)
    , d_numSocketPairs(1)
    , d_numTimers(0)
    , d_numMessages(1)
    , d_messageSize(32)
    , d_message_sp()
    , d_readRateLimiter_sp()
    , d_readQueueHighWatermark(static_cast<bsl::size_t>(-1))
    , d_writeRateLimiter_sp()
    , d_writeQueueHighWatermark(static_cast<bsl::size_t>(-1))
    , d_sendBufferSize()
    , d_receiveBufferSize()
    , d_useAsyncCallbacks(false)
    , d_tolerateDataLoss(true)
    , d_timestampIncomingData(false)
    , d_timestampOutgoingData(false)
    , d_collectMetrics(false)
    {
    }
};

/// This test provides a datagram socket protocol for this test driver.
class DatagramSocketTest::DatagramSocketSession
: public ntci::DatagramSocketSession
{
    ntccfg::Object                        d_object;
    bsl::shared_ptr<ntci::DatagramSocket> d_datagramSocket_sp;
    ntsa::Endpoint                        d_receiverEndpoint;
    bslmt::Latch                          d_numTimerEvents;
    bsls::AtomicUint                      d_numMessagesLeftToSend;
    bslmt::Latch                          d_numMessagesSent;
    bslmt::Latch                          d_numMessagesReceived;
    DatagramSocketTest::Parameters        d_parameters;
    bslma::Allocator*                     d_allocator_p;

  private:
    DatagramSocketSession(const DatagramSocketSession&) BSLS_KEYWORD_DELETED;
    DatagramSocketSession& operator=(const DatagramSocketSession&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Process the condition that the size of the read queue is greater
    /// than or equal to the read queue low watermark.
    void processReadQueueLowWatermark(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue is less
    /// than or equal to the write queue low watermark.
    void processWriteQueueLowWatermark(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process a receive completion with the specified 'endpoint' and
    /// 'data' or failure according to the specified 'error'.
    void processRead(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const bsl::shared_ptr<ntci::Receiver>&       receiver,
        const bsl::shared_ptr<bdlbb::Blob>&          data,
        const ntca::ReceiveEvent&                    event);

    /// Process a send completion or failure according to the specified
    /// 'error'.
    void processWrite(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const bsl::shared_ptr<ntci::Sender>&         sender,
        const ntca::SendEvent&                       event);

    /// Process a timer event.
    void processTimer(const bsl::shared_ptr<ntci::Timer>& timer,
                      const ntca::TimerEvent&             event);

  public:
    /// Create a new datagram socket application implemented using the
    /// specified 'datagramSocket' that operates according to the specfied
    /// test 'parameters'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    DatagramSocketSession(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const DatagramSocketTest::Parameters&        parameters,
        bslma::Allocator*                            basicAllocator = 0);

    /// Destroy this object.
    ~DatagramSocketSession() BSLS_KEYWORD_OVERRIDE;

    /// Schedule the timers for which this socket is responsible.
    void schedule();

    /// Send data to the specified 'endpoint'.
    void send(const ntsa::Endpoint& endpoint);

    /// Begin receiving data.
    void receive();

    /// Wait until all expected timers have fired and messages have been
    /// received.
    void wait();

    /// Close the socket.
    void close();

    /// Return the source endpoint of the datagram socket.
    ntsa::Endpoint sourceEndpoint() const;
};

/// Provide a datagram socket manager for this test driver.
class DatagramSocketTest::DatagramSocketManager
: public ntci::DatagramSocketManager,
  public ntccfg::Shared<DatagramSocketManager>
{
    /// Define a type alias for a set of managed datagram
    /// sockets.
    typedef bsl::unordered_map<bsl::shared_ptr<ntci::DatagramSocket>,
                               bsl::shared_ptr<DatagramSocketSession> >
        DatagramSocketApplicationMap;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    ntccfg::Object                  d_object;
    bsl::shared_ptr<ntci::Proactor> d_proactor_sp;
    bsl::shared_ptr<ntcs::Metrics>  d_metrics_sp;
    Mutex                           d_socketMapMutex;
    DatagramSocketApplicationMap    d_socketMap;
    bslmt::Latch                    d_socketsEstablished;
    bslmt::Latch                    d_socketsClosed;
    DatagramSocketTest::Parameters  d_parameters;
    bslma::Allocator*               d_allocator_p;

  private:
    DatagramSocketManager(const DatagramSocketManager&) BSLS_KEYWORD_DELETED;
    DatagramSocketManager& operator=(const DatagramSocketManager&)
        BSLS_KEYWORD_DELETED;

  private:
    void processDatagramSocketEstablished(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
        BSLS_KEYWORD_OVERRIDE;
    // Process the establishment of the specified 'datagramSocket'.

    void processDatagramSocketClosed(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
        BSLS_KEYWORD_OVERRIDE;
    // Process the closure of the specified 'datagramSocket'.

  public:
    /// Create a new datagram socket manager operating according to the
    /// specified test 'parameters' whose sockets are driven by the
    /// specified 'proactor'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    DatagramSocketManager(const bsl::shared_ptr<ntci::Proactor>& proactor,
                          const DatagramSocketTest::Parameters&  parameters,
                          bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~DatagramSocketManager() BSLS_KEYWORD_OVERRIDE;

    /// Create two datagram sockets, have them send data to each, and wait
    /// for each to receive the data.
    void run();
};

void DatagramSocketTest::Framework::runProactor(
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    bslmt::Barrier*                        barrier,
    bsl::size_t                            threadIndex)
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

void DatagramSocketTest::Framework::execute(
    const ExecuteCallback& executeCallback)
{
    Framework::execute(ntsa::Transport::e_UDP_IPV4_DATAGRAM, executeCallback);
}

void DatagramSocketTest::Framework::execute(
    ntsa::Transport::Value transport,
    const ExecuteCallback& executeCallback)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    const bsl::size_t MIN_THREADS = 1;
    const bsl::size_t MAX_THREADS = 1;

#else

    const bsl::size_t MIN_THREADS = 1;
    const bsl::size_t MAX_THREADS = 1;

#endif

    for (bsl::size_t numThreads = MIN_THREADS; numThreads <= MAX_THREADS;
         ++numThreads)
    {
        const bool dynamicLoadBalancing = numThreads > 1;

#if defined(NTCP_DATAGRAM_SOCKET_TEST_DYNAMIC_LOAD_BALANCING)
        if (dynamicLoadBalancing !=
            NTCP_DATAGRAM_SOCKET_TEST_DYNAMIC_LOAD_BALANCING)
        {
            continue;
        }
#endif

        Framework::execute(transport, numThreads, executeCallback);
    }
}

void DatagramSocketTest::Framework::execute(
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

    const bsl::size_t BLOB_BUFFER_SIZE = 4096;

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(NTSCFG_TEST_ALLOCATOR,
                           BLOB_BUFFER_SIZE,
                           BLOB_BUFFER_SIZE,
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

void DatagramSocketTest::DatagramSocketSession::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (d_parameters.d_useAsyncCallbacks) {
        return;
    }

    while (true) {
        ntca::ReceiveContext receiveContext;
        bdlbb::Blob          data;
        ntca::ReceiveOptions receiveOptions;

        error = d_datagramSocket_sp->receive(&receiveContext,
                                             &data,
                                             receiveOptions);
        if (error) {
            if (error == ntsa::Error::e_WOULD_BLOCK) {
                break;
            }
            else if (error == ntsa::Error::e_EOF) {
                NTCI_LOG_DEBUG(
                    "Datagram socket %d at %s received EOF",
                    (int)(d_datagramSocket_sp->handle()),
                    d_datagramSocket_sp->sourceEndpoint().text().c_str());
                break;
            }
            else {
                NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
            }
        }

        NTSCFG_TEST_EQ(d_datagramSocket_sp->transport(),
                       d_parameters.d_transport);

        NTSCFG_TEST_EQ(receiveContext.transport(),
                       d_datagramSocket_sp->transport());

        NTSCFG_TEST_FALSE(receiveContext.endpoint().isNull());
        NTSCFG_TEST_FALSE(receiveContext.endpoint().value().isUndefined());

        NTSCFG_TEST_EQ(data.length(), d_parameters.d_messageSize);

        NTCI_LOG_DEBUG(
            "Datagram socket %d at %s received message %d/%d from %s",
            (int)(d_datagramSocket_sp->handle()),
            d_datagramSocket_sp->sourceEndpoint().text().c_str(),
            (int)((d_parameters.d_numMessages -
                   d_numMessagesReceived.currentCount()) +
                  1),
            (int)(d_parameters.d_numMessages),
            receiveContext.endpoint().value().text().c_str());

        d_numMessagesReceived.arrive();
    }
}

void DatagramSocketTest::DatagramSocketSession::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    while (d_numMessagesLeftToSend > 0) {
        bdlbb::Blob data(
            d_datagramSocket_sp->outgoingBlobBufferFactory().get());
        ntcd::DataUtil::generateData(&data, d_parameters.d_messageSize);

        NTCI_LOG_DEBUG(
            "Datagram socket %d at %s sending message %d/%d to %s",
            (int)(d_datagramSocket_sp->handle()),
            d_datagramSocket_sp->sourceEndpoint().text().c_str(),
            (int)((d_parameters.d_numMessages - d_numMessagesLeftToSend) + 1),
            (int)(d_parameters.d_numMessages),
            d_receiverEndpoint.text().c_str());

        if (d_parameters.d_useAsyncCallbacks) {
            ntci::SendCallback sendCallback =
                d_datagramSocket_sp->createSendCallback(
                    NTCCFG_BIND(&DatagramSocketSession::processWrite,
                                this,
                                d_datagramSocket_sp,
                                NTCCFG_BIND_PLACEHOLDER_1,
                                NTCCFG_BIND_PLACEHOLDER_2),
                    d_allocator_p);

            NTSCFG_TEST_EQ(sendCallback.strand(),
                           d_datagramSocket_sp->strand());

            ntca::SendOptions sendOptions;
            sendOptions.setEndpoint(d_receiverEndpoint);

            error = d_datagramSocket_sp->send(data, sendOptions, sendCallback);
            if (error) {
                NTCI_LOG_ERROR(
                    "Datagram socket %d at %s unable to send message "
                    "%d/%d to %s: %s",
                    (int)(d_datagramSocket_sp->handle()),
                    d_datagramSocket_sp->sourceEndpoint().text().c_str(),
                    (int)((d_parameters.d_numMessages -
                           d_numMessagesLeftToSend) +
                          1),
                    (int)(d_parameters.d_numMessages),
                    d_receiverEndpoint.text().c_str(),
                    error.text().c_str());

                // Linux sometimes returns EPERM from 'sendto'.
                if (error == ntsa::Error::e_NOT_AUTHORIZED) {
                    continue;
                }

                NTSCFG_TEST_EQ(error, ntsa::Error::e_WOULD_BLOCK);
                break;
            }

            --d_numMessagesLeftToSend;
        }
        else {
            ntca::SendOptions sendOptions;
            sendOptions.setEndpoint(d_receiverEndpoint);

            error = d_datagramSocket_sp->send(data, sendOptions);
            if (error) {
                NTCI_LOG_ERROR(
                    "Datagram socket %d at %s unable to send message "
                    "%d/%d to %s: %s",
                    (int)(d_datagramSocket_sp->handle()),
                    d_datagramSocket_sp->sourceEndpoint().text().c_str(),
                    (int)((d_parameters.d_numMessages -
                           d_numMessagesLeftToSend) +
                          1),
                    (int)(d_parameters.d_numMessages),
                    d_receiverEndpoint.text().c_str(),
                    error.text().c_str());

                // Linux sometimes returns EPERM from 'sendto'.
                if (error == ntsa::Error::e_NOT_AUTHORIZED) {
                    continue;
                }

                NTSCFG_TEST_EQ(error, ntsa::Error::e_WOULD_BLOCK);
                break;
            }

            --d_numMessagesLeftToSend;
        }
    }
}

void DatagramSocketTest::DatagramSocketSession::processRead(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const bsl::shared_ptr<ntci::Receiver>&       receiver,
    const bsl::shared_ptr<bdlbb::Blob>&          data,
    const ntca::ReceiveEvent&                    event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::ReceiveEventType::e_ERROR) {
        NTSCFG_TEST_EQ(event.context().error(), ntsa::Error::e_EOF);

        NTCI_LOG_DEBUG("Datagram socket %d at %s asynchronously received EOF",
                       (int)(d_datagramSocket_sp->handle()),
                       d_datagramSocket_sp->sourceEndpoint().text().c_str());
    }
    else {
        NTSCFG_TEST_FALSE(event.context().error());

        NTSCFG_TEST_EQ(datagramSocket->transport(), d_parameters.d_transport);

        NTSCFG_TEST_EQ(event.context().transport(),
                       datagramSocket->transport());

        NTSCFG_TEST_FALSE(event.context().endpoint().isNull());
        NTSCFG_TEST_FALSE(event.context().endpoint().value().isUndefined());

        NTSCFG_TEST_EQ(data->length(), d_parameters.d_messageSize);

        NTCI_LOG_DEBUG("Datagram socket %d at %s asynchronously received "
                       "message %d/%d from %s",
                       (int)(d_datagramSocket_sp->handle()),
                       d_datagramSocket_sp->sourceEndpoint().text().c_str(),
                       (int)((d_parameters.d_numMessages -
                              d_numMessagesReceived.currentCount()) +
                             1),
                       (int)(d_parameters.d_numMessages),
                       event.context().endpoint().value().text().c_str());

        ntci::ReceiveCallback receiveCallback =
            d_datagramSocket_sp->createReceiveCallback(
                NTCCFG_BIND(&DatagramSocketSession::processRead,
                            this,
                            d_datagramSocket_sp,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3),
                d_allocator_p);

        NTSCFG_TEST_EQ(receiveCallback.strand(),
                       d_datagramSocket_sp->strand());

        ntsa::Error receiveError =
            d_datagramSocket_sp->receive(ntca::ReceiveOptions(),
                                         receiveCallback);
        NTSCFG_TEST_OK(receiveError);

        d_numMessagesReceived.arrive();
    }
}

void DatagramSocketTest::DatagramSocketSession::processWrite(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const bsl::shared_ptr<ntci::Sender>&         sender,
    const ntca::SendEvent&                       event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::SendEventType::e_ERROR) {
        if (event.context().error() == ntsa::Error::e_CANCELLED) {
            NTCI_LOG_DEBUG(
                "Datagram socket %d at %s asynchronous write cancelled",
                (int)(d_datagramSocket_sp->handle()),
                d_datagramSocket_sp->sourceEndpoint().text().c_str());
            return;
        }
        else {
            // Linux sometimes returns EPERM from 'sendto'.
            NTCI_LOG_ERROR(
                "Datagram socket %d at %s asynchronous write failed: %s",
                (int)(d_datagramSocket_sp->handle()),
                d_datagramSocket_sp->sourceEndpoint().text().c_str(),
                event.context().error().text().c_str());

            // MRM
#if 0
            ++d_numMessagesLeftToSend;

            ntca::WriteQueueEvent event;
            event.setType(ntca::WriteQueueEventType::e_LOW_WATERMARK);

            d_datagramSocket_sp->execute(
                NTCCFG_BIND(
                    &DatagramSocketSession::processWriteQueueLowWatermark,
                    this,
                    d_datagramSocket_sp,
                    event));
#endif
        }
    }
    else {
        NTSCFG_TEST_FALSE(event.context().error());

        NTCI_LOG_DEBUG("Datagram socket %d at %s asynchronously sent "
                       "message %d/%d",
                       (int)(d_datagramSocket_sp->handle()),
                       d_datagramSocket_sp->sourceEndpoint().text().c_str(),
                       (int)((d_parameters.d_numMessages -
                              d_numMessagesSent.currentCount()) +
                             1),
                       (int)(d_parameters.d_numMessages));
    }

    // Intentionally update the latch event on failure, since 'sendto' might
    // fail with EPERM on Linux and the test driver tolerates data loss.
    d_numMessagesSent.arrive();
}

void DatagramSocketTest::DatagramSocketSession::processTimer(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        NTCI_LOG_DEBUG("Datagram socket %d at %s timer has fired",
                       (int)(d_datagramSocket_sp->handle()),
                       d_datagramSocket_sp->sourceEndpoint().text().c_str());

        d_numTimerEvents.arrive();
    }
    else if (event.type() == ntca::TimerEventType::e_CANCELED) {
        BSLS_ASSERT(event.context().error() == ntsa::Error::e_CANCELLED);

        NTCI_LOG_DEBUG("Datagram socket %d at %s timer has been canceled",
                       (int)(d_datagramSocket_sp->handle()),
                       d_datagramSocket_sp->sourceEndpoint().text().c_str());

        d_numTimerEvents.arrive();
    }
}

DatagramSocketTest::DatagramSocketSession::DatagramSocketSession(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const DatagramSocketTest::Parameters&        parameters,
    bslma::Allocator*                            basicAllocator)
: d_object("DatagramSocketTest::DatagramSocketSession")
, d_datagramSocket_sp(datagramSocket)
, d_receiverEndpoint()
, d_numTimerEvents(parameters.d_numTimers)
, d_numMessagesLeftToSend(parameters.d_numMessages)
, d_numMessagesSent(parameters.d_numMessages)
, d_numMessagesReceived(parameters.d_numMessages)
, d_parameters(parameters)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

DatagramSocketTest::DatagramSocketSession::~DatagramSocketSession()
{
}

void DatagramSocketTest::DatagramSocketSession::schedule()
{
    NTCI_LOG_CONTEXT();

    bsls::TimeInterval now = d_datagramSocket_sp->currentTime();

    NTCCFG_FOREACH(bsl::size_t, timerIndex, 0, d_parameters.d_numTimers)
    {
        ntca::TimerOptions timerOptions;
        timerOptions.setOneShot(true);

        ntci::TimerCallback timerCallback =
            d_datagramSocket_sp->createTimerCallback(
                bdlf::MemFnUtil::memFn(&DatagramSocketSession::processTimer,
                                       this),
                d_allocator_p);

        NTSCFG_TEST_EQ(timerCallback.strand(), d_datagramSocket_sp->strand());

        bsl::shared_ptr<ntci::Timer> timer =
            d_datagramSocket_sp->createTimer(timerOptions,
                                             timerCallback,
                                             d_allocator_p);

        bsls::TimeInterval timerDeadline = now;
        timerDeadline.addMilliseconds(timerIndex);

        NTCI_LOG_DEBUG("Datagram socket %d at %s starting timer %d/%d",
                       (int)(d_datagramSocket_sp->handle()),
                       d_datagramSocket_sp->sourceEndpoint().text().c_str(),
                       (int)(timerIndex + 1),
                       (int)(d_parameters.d_numTimers));

        timer->schedule(timerDeadline);
    }
}

void DatagramSocketTest::DatagramSocketSession::send(
    const ntsa::Endpoint& endpoint)
{
    ntsa::Error error;

    NTSCFG_TEST_TRUE(d_receiverEndpoint.isUndefined());
    d_receiverEndpoint = endpoint;

    ntca::WriteQueueEvent event;
    event.setType(ntca::WriteQueueEventType::e_LOW_WATERMARK);

    d_datagramSocket_sp->execute(
        NTCCFG_BIND(&DatagramSocketSession::processWriteQueueLowWatermark,
                    this,
                    d_datagramSocket_sp,
                    event));
}

void DatagramSocketTest::DatagramSocketSession::receive()
{
    ntsa::Error error;

    if (d_parameters.d_useAsyncCallbacks) {
        ntci::ReceiveCallback receiveCallback =
            d_datagramSocket_sp->createReceiveCallback(
                NTCCFG_BIND(&DatagramSocketSession::processRead,
                            this,
                            d_datagramSocket_sp,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3),
                d_allocator_p);

        NTSCFG_TEST_EQ(receiveCallback.strand(),
                       d_datagramSocket_sp->strand());

        error = d_datagramSocket_sp->receive(ntca::ReceiveOptions(),
                                             receiveCallback);
        NTSCFG_TEST_OK(error);
    }

    d_datagramSocket_sp->relaxFlowControl(ntca::FlowControlType::e_RECEIVE);
}

void DatagramSocketTest::DatagramSocketSession::wait()
{
    NTCI_LOG_CONTEXT();

    d_numTimerEvents.wait();

    if (d_parameters.d_useAsyncCallbacks) {
        d_numMessagesSent.wait();
    }

    if (d_parameters.d_tolerateDataLoss) {
        // Not all datagrams might be received, so wait up to 5 seconds to try
        // to recieve those that can.
        for (bsl::size_t i = 0; i < 50; ++i) {
            if (d_numMessagesReceived.tryWait()) {
                break;
            }

            NTCI_LOG_DEBUG(
                "Datagram socket %d at %s waiting to receive messages",
                (int)(d_datagramSocket_sp->handle()),
                d_datagramSocket_sp->sourceEndpoint().text().c_str());

            bslmt::ThreadUtil::microSleep(100 * 1000);
        }
    }
    else {
        d_numMessagesReceived.wait();
    }

    bsl::size_t numMessagesReceived =
        (d_parameters.d_numMessages - d_numMessagesReceived.currentCount());

    bsl::size_t numMessagesExpectedToReceive = d_parameters.d_numMessages;

    if (numMessagesReceived < numMessagesExpectedToReceive) {
        NTCI_LOG_WARN(
            "Datagram socket %d at %s received %d/%d messages (%.2f%%)",
            (int)(d_datagramSocket_sp->handle()),
            d_datagramSocket_sp->sourceEndpoint().text().c_str(),
            (int)(numMessagesReceived),
            (int)(numMessagesExpectedToReceive),

            (double)(((double)(numMessagesReceived) /
                      (double)(numMessagesExpectedToReceive)) *
                     100)

        );
    }
}

void DatagramSocketTest::DatagramSocketSession::close()
{
    ntsa::Error error;

    error = d_datagramSocket_sp->shutdown(ntsa::ShutdownType::e_BOTH,
                                          ntsa::ShutdownMode::e_IMMEDIATE);
    NTSCFG_TEST_FALSE(error);

    if (d_parameters.d_useAsyncCallbacks) {
        ntci::DatagramSocketCloseGuard guard(d_datagramSocket_sp);
    }
    else {
        d_datagramSocket_sp->close();
    }
}

ntsa::Endpoint DatagramSocketTest::DatagramSocketSession::sourceEndpoint()
    const
{
    return d_datagramSocket_sp->sourceEndpoint();
}

void DatagramSocketTest::DatagramSocketManager::
    processDatagramSocketEstablished(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Datagram socket %d established",
                   (int)(datagramSocket->handle()));

    bsl::shared_ptr<DatagramSocketSession> datagramSocketSession;
    datagramSocketSession.createInplace(d_allocator_p,
                                        datagramSocket,
                                        d_parameters,
                                        d_allocator_p);

    datagramSocket->registerSession(datagramSocketSession);

    if (d_parameters.d_readRateLimiter_sp) {
        datagramSocket->setReadRateLimiter(d_parameters.d_readRateLimiter_sp);
    }

    if (d_parameters.d_writeRateLimiter_sp) {
        datagramSocket->setWriteRateLimiter(
            d_parameters.d_writeRateLimiter_sp);
    }

    {
        LockGuard guard(&d_socketMapMutex);
        d_socketMap.insert(
            DatagramSocketApplicationMap::value_type(datagramSocket,
                                                     datagramSocketSession));
    }

    datagramSocketSession->receive();

    d_socketsEstablished.arrive();
}

void DatagramSocketTest::DatagramSocketManager::processDatagramSocketClosed(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Datagram socket %d closed",
                   (int)(datagramSocket->handle()));

    {
        LockGuard   guard(&d_socketMapMutex);
        bsl::size_t n = d_socketMap.erase(datagramSocket);
        NTSCFG_TEST_EQ(n, 1);
    }

    d_socketsClosed.arrive();
}

DatagramSocketTest::DatagramSocketManager::DatagramSocketManager(
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    const DatagramSocketTest::Parameters&  parameters,
    bslma::Allocator*                      basicAllocator)
: d_object("DatagramSocketTest::DatagramSocketManager")
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

DatagramSocketTest::DatagramSocketManager::~DatagramSocketManager()
{
    NTSCFG_TEST_TRUE(d_socketMap.empty());
}

void DatagramSocketTest::DatagramSocketManager::run()
{
    ntsa::Error error;

    ntca::MonitorableRegistryConfig monitorableRegistryConfig;
    ntcs::MonitorableUtil::enableMonitorableRegistry(
        monitorableRegistryConfig);

    // Create all the datagram socket pairs.

    for (bsl::size_t i = 0; i < d_parameters.d_numSocketPairs; ++i) {
        for (bsl::size_t j = 0; j < 2; ++j) {
            ntca::DatagramSocketOptions options;
            options.setTransport(d_parameters.d_transport);
            options.setSourceEndpoint(
                DatagramSocketTest::any(d_parameters.d_transport));
            options.setReadQueueLowWatermark(1);
            options.setReadQueueHighWatermark(
                d_parameters.d_readQueueHighWatermark);
            options.setWriteQueueLowWatermark(0);
            options.setWriteQueueHighWatermark(
                d_parameters.d_writeQueueHighWatermark);
            options.setSendGreedily(false);
            options.setReceiveGreedily(false);
            options.setKeepHalfOpen(false);
            options.setTimestampIncomingData(
                d_parameters.d_timestampIncomingData);
            options.setTimestampOutgoingData(
                d_parameters.d_timestampOutgoingData);
            options.setMetrics(d_parameters.d_collectMetrics);
            if (d_parameters.d_timestampIncomingData ||
                d_parameters.d_timestampOutgoingData)
            {
                // Metrics is used to validate timestamps
                NTSCFG_TEST_TRUE(d_parameters.d_collectMetrics);
            }

            if (!d_parameters.d_sendBufferSize.isNull()) {
                options.setSendBufferSize(
                    d_parameters.d_sendBufferSize.value());
            }

            if (!d_parameters.d_receiveBufferSize.isNull()) {
                options.setReceiveBufferSize(
                    d_parameters.d_receiveBufferSize.value());
            }

            bsl::shared_ptr<ntci::Resolver> resolver;

            bsl::shared_ptr<ntcp::DatagramSocket> datagramSocket;
            datagramSocket.createInplace(d_allocator_p,
                                         options,
                                         resolver,
                                         d_proactor_sp,
                                         d_proactor_sp,
                                         d_metrics_sp,
                                         d_allocator_p);

            error = datagramSocket->registerManager(this->getSelf(this));
            NTSCFG_TEST_FALSE(error);

            bsl::shared_ptr<ntcd::DatagramSocket> datagramSocketBase;
            datagramSocketBase.createInplace(d_allocator_p, d_allocator_p);

            error = datagramSocket->open(d_parameters.d_transport,
                                         datagramSocketBase);
            NTSCFG_TEST_FALSE(error);
        }
    }

    // Wait for all the datagram sockets to become established.

    d_socketsEstablished.wait();

    // Start the timers for each datagram socket.

    {
        LockGuard guard(&d_socketMapMutex);

        for (DatagramSocketApplicationMap::const_iterator it =
                 d_socketMap.begin();
             it != d_socketMap.end();
             ++it)
        {
            const bsl::shared_ptr<DatagramSocketSession>& socket = it->second;
            socket->schedule();
        }
    }

    // Send data between each datagram socket pair.

    {
        LockGuard guard(&d_socketMapMutex);

        BSLS_ASSERT(d_socketMap.size() % 2 == 0);

        typedef bsl::vector<bsl::shared_ptr<DatagramSocketSession> >
            DatagramSocketApplicationVector;

        DatagramSocketApplicationVector socketVector;
        socketVector.reserve(d_socketMap.size());

        for (DatagramSocketApplicationMap::const_iterator it =
                 d_socketMap.begin();
             it != d_socketMap.end();
             ++it)
        {
            const bsl::shared_ptr<DatagramSocketSession>& socket = it->second;

            socketVector.push_back(socket);
        }

        BSLS_ASSERT(socketVector.size() % 2 == 0);

        for (bsl::size_t i = 0; i < socketVector.size(); i += 2) {
            const bsl::shared_ptr<DatagramSocketSession>& socketOne =
                socketVector[i + 0];

            const bsl::shared_ptr<DatagramSocketSession>& socketTwo =
                socketVector[i + 1];

            socketOne->send(socketTwo->sourceEndpoint());
            socketTwo->send(socketOne->sourceEndpoint());
        }
    }

    // Wait for all timers to fire and all messages to be received for each
    // datagram socket.

    {
        LockGuard guard(&d_socketMapMutex);

        for (DatagramSocketApplicationMap::const_iterator it =
                 d_socketMap.begin();
             it != d_socketMap.end();
             ++it)
        {
            const bsl::shared_ptr<DatagramSocketSession>& socket = it->second;
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
            bslmt::ThreadUtil::microSleep(500 * 1000);
        }

        bsl::vector<bsl::shared_ptr<ntci::Monitorable> > monitorables;
        ntcs::MonitorableUtil::loadRegisteredObjects(&monitorables);
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
            const double txTimestampsPercentage = 0.90;

            if (!d_parameters.d_timestampOutgoingData) {
                validateNoMetricsAvailable(statsArray,
                                           baseTxDelayBeforeSchedIndex,
                                           total);
                validateNoMetricsAvailable(statsArray,
                                           baseTxDelayInSoftwareIndex,
                                           total);
                validateNoMetricsAvailable(statsArray,
                                           baseTxDelayIndex,
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
                validateMetricsAvailable(statsArray, baseTxDelayIndex, total);
                validateNoMetricsAvailable(statsArray,
                                           baseTxDelayBeforeAckIndex,
                                           total);

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
                    statsArray[baseTxDelayIndex + countOffset].theDouble(),
                    d_parameters.d_numMessages * txTimestampsPercentage);
                NTSCFG_TEST_GT(
                    statsArray[baseTxDelayIndex + totalOffset].theDouble(),
                    0);
                NTSCFG_TEST_GT(
                    statsArray[baseTxDelayIndex + minOffset].theDouble(),
                    0);
                NTSCFG_TEST_GT(
                    statsArray[baseTxDelayIndex + avgOffset].theDouble(),
                    0);
                NTSCFG_TEST_GT(
                    statsArray[baseTxDelayIndex + maxOffset].theDouble(),
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
                validateMetricsAvailable(statsArray, baseRxDelayIndex, total);
                validateNoMetricsAvailable(statsArray,
                                           baseRxDelayInHardwareIndex,
                                           total);

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

    // Close all the datagram sockets.

    {
        typedef bsl::vector<bsl::shared_ptr<DatagramSocketSession> >
            DatagramSocketApplicationVector;

        DatagramSocketApplicationVector socketVector;
        socketVector.reserve(d_socketMap.size());

        {
            LockGuard guard(&d_socketMapMutex);

            for (DatagramSocketApplicationMap::const_iterator it =
                     d_socketMap.begin();
                 it != d_socketMap.end();
                 ++it)
            {
                const bsl::shared_ptr<DatagramSocketSession>& socket =
                    it->second;

                socketVector.push_back(socket);
            }
        }

        for (DatagramSocketApplicationVector::const_iterator it =
                 socketVector.begin();
             it != socketVector.end();
             ++it)
        {
            const bsl::shared_ptr<DatagramSocketSession>& socket = *it;
            socket->close();
        }
    }

    // Wait for all datagram sockets to close.

    d_socketsClosed.wait();
}

void DatagramSocketTest::verifyGenericVariation(
    ntsa::Transport::Value                 transport,
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    const DatagramSocketTest::Parameters&  parameters,
    bslma::Allocator*                      allocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Datagram socket test starting");

    DatagramSocketTest::Parameters effectiveParameters = parameters;
    effectiveParameters.d_transport                    = transport;

    bsl::shared_ptr<DatagramSocketTest::DatagramSocketManager>
        datagramSocketManager;
    datagramSocketManager.createInplace(allocator,
                                        proactor,
                                        effectiveParameters,
                                        allocator);

    datagramSocketManager->run();
    datagramSocketManager.reset();

    NTCI_LOG_DEBUG("Datagram socket test complete");

    proactor->stop();
}

void DatagramSocketTest::verifyReceiveDeadlineVariation(
    ntsa::Transport::Value                 transport,
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    const DatagramSocketTest::Parameters&  parameters,
    bslma::Allocator*                      allocator)
{
    // Concern: Receive deadlines.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Datagram socket receive deadline test starting");

    const int k_RECEIVE_TIMEOUT_IN_MILLISECONDS = 200;

    ntsa::Error                    error;
    bslmt::Semaphore               semaphore;
    bsl::shared_ptr<ntcs::Metrics> metrics;

    ntca::DatagramSocketOptions options;
    options.setTransport(transport);
    options.setSourceEndpoint(DatagramSocketTest::any(transport));

    bsl::shared_ptr<ntci::Resolver> resolver;

    bsl::shared_ptr<ntcp::DatagramSocket> datagramSocket;
    datagramSocket.createInplace(allocator,
                                 options,
                                 resolver,
                                 proactor,
                                 proactor,
                                 metrics,
                                 allocator);

    bsl::shared_ptr<ntcd::DatagramSocket> datagramSocketBase;
    datagramSocketBase.createInplace(allocator, allocator);

    error = datagramSocket->open(transport, datagramSocketBase);
    NTSCFG_TEST_FALSE(error);

    bsls::TimeInterval receiveTimeout;
    receiveTimeout.setTotalMilliseconds(k_RECEIVE_TIMEOUT_IN_MILLISECONDS);

    bsls::TimeInterval receiveDeadline =
        datagramSocket->currentTime() + receiveTimeout;

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setDeadline(receiveDeadline);

    ntci::ReceiveCallback receiveCallback =
        datagramSocket->createReceiveCallback(
            NTCCFG_BIND(&DatagramSocketTest::processReceiveTimeout,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        &semaphore),
            allocator);

    error = datagramSocket->receive(receiveOptions, receiveCallback);
    NTSCFG_TEST_OK(error);

    semaphore.wait();

    {
        ntci::DatagramSocketCloseGuard datagramSocketCloseGuard(
            datagramSocket);
    }

    NTCI_LOG_DEBUG("Datagram socket receive deadline test complete");

    proactor->stop();
}

void DatagramSocketTest::verifyReceiveCancellationVariation(
    ntsa::Transport::Value                 transport,
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    const DatagramSocketTest::Parameters&  parameters,
    bslma::Allocator*                      allocator)
{
    // Concern: Receive cancellation.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Datagram socket receive cancellation test starting");

    const int k_RECEIVE_TIMEOUT_IN_MILLISECONDS = 200;

    ntsa::Error                    error;
    bslmt::Semaphore               semaphore;
    bsl::shared_ptr<ntcs::Metrics> metrics;

    ntca::DatagramSocketOptions options;
    options.setTransport(transport);
    options.setSourceEndpoint(DatagramSocketTest::any(transport));

    bsl::shared_ptr<ntci::Resolver> resolver;

    bsl::shared_ptr<ntcp::DatagramSocket> datagramSocket;
    datagramSocket.createInplace(allocator,
                                 options,
                                 resolver,
                                 proactor,
                                 proactor,
                                 metrics,
                                 allocator);

    bsl::shared_ptr<ntcd::DatagramSocket> datagramSocketBase;
    datagramSocketBase.createInplace(allocator, allocator);

    error = datagramSocket->open(transport, datagramSocketBase);
    NTSCFG_TEST_FALSE(error);

    bsls::TimeInterval receiveTimeout;
    receiveTimeout.setTotalMilliseconds(k_RECEIVE_TIMEOUT_IN_MILLISECONDS);

    bsls::TimeInterval receiveDeadline =
        datagramSocket->currentTime() + receiveTimeout;

    ntca::ReceiveToken receiveToken;
    receiveToken.setValue(1);

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setToken(receiveToken);

    ntci::ReceiveCallback receiveCallback =
        datagramSocket->createReceiveCallback(
            NTCCFG_BIND(&DatagramSocketTest::processReceiveCancellation,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        &semaphore),
            allocator);

    error = datagramSocket->receive(receiveOptions, receiveCallback);
    NTSCFG_TEST_OK(error);

    ntca::TimerOptions timerOptions;

    timerOptions.setOneShot(true);
    timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    ntci::TimerCallback timerCallback = datagramSocket->createTimerCallback(
        bdlf::BindUtil::bind(&cancelReceive, datagramSocket, receiveToken),
        allocator);

    bsl::shared_ptr<ntci::Timer> timer =
        datagramSocket->createTimer(timerOptions, timerCallback, allocator);

    error = timer->schedule(receiveDeadline);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    {
        ntci::DatagramSocketCloseGuard datagramSocketCloseGuard(
            datagramSocket);
    }

    NTCI_LOG_DEBUG("Datagram socket receive cancellation test complete");

    proactor->stop();
}

void DatagramSocketTest::validateNoMetricsAvailable(
    const bdld::DatumArrayRef& metrics,
    int                        base,
    int                        number)
{
    NTSCFG_TEST_GE(metrics.length(), base + number);
    for (int i = base; i < base + number; ++i) {
        NTSCFG_TEST_EQ(metrics[i].type(), bdld::Datum::e_NIL);
    }
}

void DatagramSocketTest::validateMetricsAvailable(
    const bdld::DatumArrayRef& metrics,
    int                        base,
    int                        number)
{
    NTSCFG_TEST_GE(metrics.length(), base + number);
    for (int i = base; i < base + number; ++i) {
        NTSCFG_TEST_EQ(metrics[i].type(), bdld::Datum::e_DOUBLE);
    }
}

ntsa::Endpoint DatagramSocketTest::any(ntsa::Transport::Value transport)
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

void DatagramSocketTest::cancelReceive(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReceiveToken                     token)
{
    ntsa::Error error = datagramSocket->cancel(token);
    NTSCFG_TEST_FALSE(error);
}

void DatagramSocketTest::processReceiveTimeout(
    const bsl::shared_ptr<ntci::Receiver>& receiver,
    const bsl::shared_ptr<bdlbb::Blob>&    data,
    const ntca::ReceiveEvent&              event,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing receive from event type %s: %s",
                   ntca::ReceiveEventType::toString(event.type()),
                   event.context().error().text().c_str());

    NTSCFG_TEST_EQ(event.type(), ntca::ReceiveEventType::e_ERROR);
    NTSCFG_TEST_EQ(event.context().error(), ntsa::Error::e_WOULD_BLOCK);

    semaphore->post();
}

void DatagramSocketTest::processReceiveCancellation(
    const bsl::shared_ptr<ntci::Receiver>& receiver,
    const bsl::shared_ptr<bdlbb::Blob>&    data,
    const ntca::ReceiveEvent&              event,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing receive from event type %s: %s",
                   ntca::ReceiveEventType::toString(event.type()),
                   event.context().error().text().c_str());

    NTSCFG_TEST_EQ(event.type(), ntca::ReceiveEventType::e_ERROR);
    NTSCFG_TEST_EQ(event.context().error(), ntsa::Error::e_CANCELLED);

    semaphore->post();
}

NTSCFG_TEST_FUNCTION(ntcp::DatagramSocketTest::verifyBreathing)
{
    // Concern: Breathing test.

    DatagramSocketTest::Parameters parameters;
    parameters.d_numTimers         = 0;
    parameters.d_numSocketPairs    = 1;
    parameters.d_numMessages       = 1;
    parameters.d_messageSize       = 32;
    parameters.d_useAsyncCallbacks = false;

    DatagramSocketTest::Framework::execute(
        NTCCFG_BIND(&DatagramSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::DatagramSocketTest::verifyBreathingAsync)
{
    // Concern: Breathing test using asynchronous callbacks.

    DatagramSocketTest::Parameters parameters;
    parameters.d_numTimers         = 0;
    parameters.d_numSocketPairs    = 1;
    parameters.d_numMessages       = 1;
    parameters.d_messageSize       = 32;
    parameters.d_useAsyncCallbacks = true;

    DatagramSocketTest::Framework::execute(
        NTCCFG_BIND(&DatagramSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::DatagramSocketTest::verifyStress)
{
    // Concern: Stress test.

    // The test currently fails sporadically on Linux on CI build machines
    // with "Assertion failed: !d_chronology_sp->hasAnyDeferred()".
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0
    DatagramSocketTest::Parameters parameters;
    parameters.d_numTimers         = 100;
    parameters.d_numSocketPairs    = 100;
    parameters.d_numMessages       = 32;
    parameters.d_messageSize       = 1024;
    parameters.d_useAsyncCallbacks = false;

    DatagramSocketTest::Framework::execute(
        NTCCFG_BIND(&DatagramSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
#endif
}

NTSCFG_TEST_FUNCTION(ntcp::DatagramSocketTest::verifyStressAsync)
{
    // Concern: Stress test using asynchronous callbacks.

    // The test currently fails sporadically on Linux on CI build machines
    // with "Assertion failed: !d_chronology_sp->hasAnyDeferred()".
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0
    DatagramSocketTest::Parameters parameters;
    parameters.d_numTimers         = 100;
    parameters.d_numSocketPairs    = 100;
    parameters.d_numMessages       = 32;
    parameters.d_messageSize       = 1024;
    parameters.d_useAsyncCallbacks = true;

    DatagramSocketTest::Framework::execute(
        NTCCFG_BIND(&DatagramSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
#endif
}

NTSCFG_TEST_FUNCTION(ntcp::DatagramSocketTest::verifyIncomingTimestamps)
{
    // Concern: Incoming timestamps test
#if 0
    // The test is disabled due to its flaky nature
    DatagramSocketTest::Parameters parameters;
    parameters.d_numTimers             = 0;
    parameters.d_numSocketPairs        = 1;
    parameters.d_numMessages           = 10;
    parameters.d_messageSize           = 32;
    parameters.d_useAsyncCallbacks     = false;
    parameters.d_timestampIncomingData = true;
    parameters.d_collectMetrics        = true;

    DatagramSocketTest::Framework::execute(
        NTCCFG_BIND(&DatagramSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
#endif
}

NTSCFG_TEST_FUNCTION(ntcp::DatagramSocketTest::verifyOutgoingTimestamps)
{
    // Concern: Outgoing timestamps test
#if 0
    // The test is disabled due to its flaky nature
    DatagramSocketTest::Parameters parameters;
    parameters.d_numTimers             = 0;
    parameters.d_numSocketPairs        = 1;
    parameters.d_numMessages           = 100;
    parameters.d_messageSize           = 512;
    parameters.d_sendBufferSize        = 512;
    parameters.d_useAsyncCallbacks     = false;
    parameters.d_timestampOutgoingData = true;
    parameters.d_collectMetrics        = true;
    parameters.d_tolerateDataLoss      = false;

    DatagramSocketTest::Framework::execute(
        NTCCFG_BIND(&DatagramSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
#endif
}

NTSCFG_TEST_FUNCTION(ntcp::DatagramSocketTest::verifyReceiveDeadline)
{
    DatagramSocketTest::Parameters parameters;

    DatagramSocketTest::Framework::execute(
        NTCCFG_BIND(&DatagramSocketTest::verifyReceiveDeadlineVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::DatagramSocketTest::verifyReceiveCancellation)
{
    DatagramSocketTest::Parameters parameters;

    DatagramSocketTest::Framework::execute(
        NTCCFG_BIND(&DatagramSocketTest::verifyReceiveCancellationVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

}  // close namespace ntcp
}  // close namespace BloombergLP
