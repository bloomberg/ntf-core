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

#include <ntcr_streamsocket.h>

#include <ntccfg_bind.h>
#include <ntccfg_test.h>
#include <ntcd_datautil.h>
#include <ntcd_encryption.h>
#include <ntcd_reactor.h>
#include <ntcd_simulation.h>
#include <ntci_log.h>
#include <ntcm_monitorableutil.h>
#include <ntcs_datapool.h>
#include <ntcs_ratelimiter.h>
#include <ntcs_user.h>
#include <ntcu_streamsocketeventqueue.h>
#include <ntsa_error.h>
#include <ntsa_transport.h>
#include <ntsi_streamsocket.h>
#include <bdlb_bigendian.h>
#include <bdlb_string.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobstreambuf.h>
#include <bdlbb_blobutil.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bdlma_countingallocator.h>
#include <bdlt_currenttime.h>
#include <bslmt_barrier.h>
#include <bslmt_latch.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>
#include <bsl_unordered_map.h>

#include <pdh.h>

using namespace BloombergLP;

// Uncomment to test a particular style of socket-to-thread load balancing,
// instead of both static and dynamic load balancing.
// #define NTCR_STREAM_SOCKET_TEST_DYNAMIC_LOAD_BALANCING false

namespace test {

namespace {

/// Validate that the specified 'metrics' does not contain data for
/// elements starting from the specified 'base' up to 'base' + the
/// 'specified 'num' (exclusive) in total.
void validateNoMetricsAvailable(const bdld::DatumArrayRef& metrics,
                                int                        base,
                                int                        num)
{
    NTCCFG_TEST_GE(metrics.length(), base + num);
    for (int i = base; i < base + num; ++i) {
        NTCCFG_TEST_EQ(metrics[i].type(), bdld::Datum::e_NIL);
    }
}

/// Validate that the specified 'metrics' contains data for elements
/// starting from the specified 'base' up to 'base' + the specified 'num'
/// (exclusive) in total.
void validateMetricsAvailable(const bdld::DatumArrayRef& metrics,
                              int                        base,
                              int                        num)
{
    NTCCFG_TEST_GE(metrics.length(), base + num);
    for (int i = base; i < base + num; ++i) {
        NTCCFG_TEST_EQ(metrics[i].type(), bdld::Datum::e_DOUBLE);
    }
}

}  // close unnamed namespace

/// Provide a test case execution framework.
class Framework
{
  private:
    /// Run a thread identified by the specified 'threadIndex' that waits
    /// on the specified 'barrier' then drives the specified 'reactor' until
    /// it is stopped.
    static void runReactor(const bsl::shared_ptr<ntci::Reactor>& reactor,
                           bslmt::Barrier*                       barrier,
                           bsl::size_t                           threadIndex);

  public:
    /// Define a type alias for the function implementing a
    /// test case driven by this test framework.
    typedef NTCCFG_FUNCTION(ntsa::Transport::Value                transport,
                            const bsl::shared_ptr<ntci::Reactor>& reactor,
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

void Framework::runReactor(const bsl::shared_ptr<ntci::Reactor>& reactor,
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

    // Register this thread as the thread that will wait on the reactor.

    ntci::Waiter waiter = reactor->registerWaiter(ntca::WaiterOptions());

    // Wait until all threads have reached the rendezvous point.

    barrier->wait();

    // Process deferred functions.

    reactor->run(waiter);

    // Deregister the waiter.

    reactor->deregisterWaiter(waiter);
}

void Framework::execute(const ExecuteCallback& executeCallback)
{
    Framework::execute(ntsa::Transport::e_TCP_IPV4_STREAM, executeCallback);
}

void Framework::execute(ntsa::Transport::Value transport,
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

#if defined(NTCR_STREAM_SOCKET_TEST_DYNAMIC_LOAD_BALANCING)
        if (dynamicLoadBalancing !=
            NTCR_STREAM_SOCKET_TEST_DYNAMIC_LOAD_BALANCING)
        {
            continue;
        }
#endif

        Framework::execute(transport, numThreads, executeCallback);
    }
}

void Framework::execute(ntsa::Transport::Value transport,
                        bsl::size_t            numThreads,
                        const ExecuteCallback& executeCallback)
{
    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;

        BSLS_LOG_INFO("Testing transport %s numThreads %d",
                      ntsa::Transport::toString(transport),
                      (int)(numThreads));

        bsl::shared_ptr<ntcd::Simulation> simulation;
        simulation.createInplace(&ta, &ta);

        error = simulation->run();
        NTCCFG_TEST_OK(error);

        const bsl::size_t BLOB_BUFFER_SIZE = 4096;

        bsl::shared_ptr<ntcs::DataPool> dataPool;
        dataPool.createInplace(&ta, BLOB_BUFFER_SIZE, BLOB_BUFFER_SIZE, &ta);

        bsl::shared_ptr<ntcs::User> user;
        user.createInplace(&ta, &ta);

        user->setDataPool(dataPool);

        ntca::ReactorConfig reactorConfig;
        reactorConfig.setMetricName("test");
        reactorConfig.setMinThreads(numThreads);
        reactorConfig.setMaxThreads(numThreads);
        reactorConfig.setAutoAttach(false);
        reactorConfig.setAutoDetach(false);
        reactorConfig.setOneShot(numThreads > 1);

        bsl::shared_ptr<ntcd::Reactor> reactor;
        reactor.createInplace(&ta, reactorConfig, user, &ta);

        bslmt::Barrier threadGroupBarrier(numThreads + 1);

        bslmt::ThreadGroup threadGroup(&ta);

        for (bsl::size_t threadIndex = 0; threadIndex < numThreads;
             ++threadIndex)
        {
            threadGroup.addThread(NTCCFG_BIND(&Framework::runReactor,
                                              reactor,
                                              &threadGroupBarrier,
                                              threadIndex));
        }

        threadGroupBarrier.wait();

        executeCallback(transport, reactor, &ta);

        threadGroup.joinAll();

        simulation->stop();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

/// Provide functions for returning endpoints used by this
/// test driver.
struct EndpointUtil {
    /// Return an endpoint representing a suitable address to which to
    /// bind a socket of the specified 'transport' type for use by this
    /// test driver.
    static ntsa::Endpoint any(ntsa::Transport::Value transport);
};

ntsa::Endpoint EndpointUtil::any(ntsa::Transport::Value transport)
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

/// This struct defines the parameters of a test.
struct Parameters {
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
class StreamSocketSession : public ntci::StreamSocketSession
{
    ntccfg::Object                      d_object;
    bsl::shared_ptr<ntci::StreamSocket> d_streamSocket_sp;
    bdlbb::Blob                         d_dataReceived;
    bslmt::Latch                        d_numTimerEvents;
    bsls::AtomicUint                    d_numMessagesLeftToSend;
    bslmt::Latch                        d_numMessagesSent;
    bslmt::Latch                        d_numMessagesReceived;
    test::Parameters                    d_parameters;
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
        const test::Parameters&                    parameters,
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
class StreamSocketManager : public ntci::StreamSocketManager,
                            public ntccfg::Shared<StreamSocketManager>
{
    /// Define a type alias for a set of managed stream
    /// sockets.
    typedef bsl::unordered_map<bsl::shared_ptr<ntci::StreamSocket>,
                               bsl::shared_ptr<StreamSocketSession> >
        StreamSocketApplicationMap;

    ntccfg::Object                 d_object;
    bsl::shared_ptr<ntci::Reactor> d_reactor_sp;
    bsl::shared_ptr<ntcs::Metrics> d_metrics_sp;
    bslmt::Mutex                   d_socketMapMutex;
    StreamSocketApplicationMap     d_socketMap;
    bslmt::Latch                   d_socketsEstablished;
    bslmt::Latch                   d_socketsClosed;
    test::Parameters               d_parameters;
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
    /// specified 'reactor'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    StreamSocketManager(const bsl::shared_ptr<ntci::Reactor>& reactor,
                        const test::Parameters&               parameters,
                        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~StreamSocketManager() BSLS_KEYWORD_OVERRIDE;

    /// Create two stream sockets, have them send data to each, and wait
    /// for each to receive the data.
    void run();
};

void StreamSocketSession::processReadQueueLowWatermark(
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
                NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            }
        }

        NTCI_LOG_DEBUG("Stream socket %d at %s to %s received %d bytes",
                       (int)(d_streamSocket_sp->handle()),
                       d_streamSocket_sp->sourceEndpoint().text().c_str(),
                       d_streamSocket_sp->remoteEndpoint().text().c_str(),
                       d_dataReceived.length());

        NTCCFG_TEST_EQ(d_streamSocket_sp->transport(),
                       d_parameters.d_transport);

        NTCCFG_TEST_EQ(receiveContext.transport(),
                       d_streamSocket_sp->transport());

        NTCCFG_TEST_FALSE(receiveContext.endpoint().isNull());
        NTCCFG_TEST_FALSE(receiveContext.endpoint().value().isUndefined());

        NTCCFG_TEST_EQ(d_dataReceived.length(), d_parameters.d_messageSize);

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

void StreamSocketSession::processWriteQueueLowWatermark(
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

            NTCCFG_TEST_EQ(sendCallback.strand(), d_streamSocket_sp->strand());

            error = d_streamSocket_sp->send(data,
                                            ntca::SendOptions(),
                                            sendCallback);
            if (error) {
                NTCCFG_TEST_EQ(error, ntsa::Error::e_WOULD_BLOCK);
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
                NTCCFG_TEST_EQ(error, ntsa::Error::e_WOULD_BLOCK);
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

void StreamSocketSession::processRead(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Receiver>&     receiver,
    const bsl::shared_ptr<bdlbb::Blob>&        data,
    const ntca::ReceiveEvent&                  event)
{
    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::ReceiveEventType::e_ERROR) {
        NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_EOF);

        NTCI_LOG_DEBUG(
            "Stream socket %d at %s to %s asynchronously received EOF",
            (int)(d_streamSocket_sp->handle()),
            d_streamSocket_sp->sourceEndpoint().text().c_str(),
            d_streamSocket_sp->remoteEndpoint().text().c_str());
    }
    else {
        NTCCFG_TEST_FALSE(event.context().error());

        NTCCFG_TEST_EQ(streamSocket->transport(), d_parameters.d_transport);

        NTCCFG_TEST_EQ(event.context().transport(), streamSocket->transport());

        NTCCFG_TEST_FALSE(event.context().endpoint().isNull());
        NTCCFG_TEST_FALSE(event.context().endpoint().value().isUndefined());

        NTCCFG_TEST_EQ(data->length(), d_parameters.d_messageSize);

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

        NTCCFG_TEST_EQ(receiveCallback.strand(), d_streamSocket_sp->strand());

        ntsa::Error receiveError =
            d_streamSocket_sp->receive(options, receiveCallback);
        NTCCFG_TEST_OK(receiveError);

        d_numMessagesReceived.arrive();
    }
}

void StreamSocketSession::processWrite(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Sender>&       sender,
    const ntca::SendEvent&                     event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::SendEventType::e_ERROR) {
        NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_CANCELLED);

        NTCI_LOG_DEBUG(
            "Stream socket %d at %s to %s asynchronous write cancelled",
            (int)(d_streamSocket_sp->handle()),
            d_streamSocket_sp->sourceEndpoint().text().c_str(),
            d_streamSocket_sp->remoteEndpoint().text().c_str());
    }
    else {
        NTCCFG_TEST_FALSE(event.context().error());

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

void StreamSocketSession::processTimer(
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

StreamSocketSession::StreamSocketSession(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const test::Parameters&                    parameters,
    bslma::Allocator*                          basicAllocator)
: d_object("test::StreamSocketSession")
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

StreamSocketSession::~StreamSocketSession()
{
}

void StreamSocketSession::schedule()
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

        NTCCFG_TEST_EQ(timerCallback.strand(), d_streamSocket_sp->strand());

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

void StreamSocketSession::send()
{
    ntca::WriteQueueEvent event;
    event.setType(ntca::WriteQueueEventType::e_LOW_WATERMARK);

    d_streamSocket_sp->execute(
        NTCCFG_BIND(&StreamSocketSession::processWriteQueueLowWatermark,
                    this,
                    d_streamSocket_sp,
                    event));
}

void StreamSocketSession::receive()
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

        NTCCFG_TEST_EQ(receiveCallback.strand(), d_streamSocket_sp->strand());

        error = d_streamSocket_sp->receive(options, receiveCallback);
        NTCCFG_TEST_OK(error);
    }

    d_streamSocket_sp->relaxFlowControl(ntca::FlowControlType::e_RECEIVE);
}

void StreamSocketSession::wait()
{
    d_numTimerEvents.wait();
    if (d_parameters.d_useAsyncCallbacks) {
        d_numMessagesSent.wait();
    }
    d_numMessagesReceived.wait();
}

void StreamSocketSession::close()
{
    ntsa::Error error;

    error = d_streamSocket_sp->shutdown(ntsa::ShutdownType::e_BOTH,
                                        ntsa::ShutdownMode::e_IMMEDIATE);
    NTCCFG_TEST_FALSE(error);

    if (d_parameters.d_useAsyncCallbacks) {
        ntci::StreamSocketCloseGuard guard(d_streamSocket_sp);
    }
    else {
        d_streamSocket_sp->close();
    }
}

ntsa::Endpoint StreamSocketSession::sourceEndpoint() const
{
    return d_streamSocket_sp->sourceEndpoint();
}

ntsa::Endpoint StreamSocketSession::remoteEndpoint() const
{
    return d_streamSocket_sp->remoteEndpoint();
}

void StreamSocketManager::processStreamSocketEstablished(
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
        bslmt::LockGuard<bslmt::Mutex> guard(&d_socketMapMutex);
        d_socketMap.insert(
            StreamSocketApplicationMap::value_type(streamSocket,
                                                   streamSocketSession));
    }

    streamSocketSession->receive();

    d_socketsEstablished.arrive();
}

void StreamSocketManager::processStreamSocketClosed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket %d closed", (int)(streamSocket->handle()));

    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_socketMapMutex);
        bsl::size_t                    n = d_socketMap.erase(streamSocket);
        NTCCFG_TEST_EQ(n, 1);
    }

    d_socketsClosed.arrive();
}

StreamSocketManager::StreamSocketManager(
    const bsl::shared_ptr<ntci::Reactor>& reactor,
    const test::Parameters&               parameters,
    bslma::Allocator*                     basicAllocator)
: d_object("test::StreamSocketManager")
, d_reactor_sp(reactor)
, d_metrics_sp()
, d_socketMapMutex()
, d_socketMap(basicAllocator)
, d_socketsEstablished(parameters.d_numSocketPairs * 2)
, d_socketsClosed(parameters.d_numSocketPairs * 2)
, d_parameters(parameters)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

StreamSocketManager::~StreamSocketManager()
{
    NTCCFG_TEST_TRUE(d_socketMap.empty());
}

void StreamSocketManager::run()
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
            NTCCFG_TEST_TRUE(d_parameters.d_collectMetrics);
        }

        bsl::shared_ptr<ntci::Resolver> resolver;

        bsl::shared_ptr<ntcd::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntcd::StreamSocket> basicServerSocket;

        error =
            ntcd::Simulation::createStreamSocketPair(&basicClientSocket,
                                                     &basicServerSocket,
                                                     d_parameters.d_transport);
        NTCCFG_TEST_FALSE(error);

        bsl::shared_ptr<ntcr::StreamSocket> clientStreamSocket;
        clientStreamSocket.createInplace(d_allocator_p,
                                         options,
                                         resolver,
                                         d_reactor_sp,
                                         d_reactor_sp,
                                         d_metrics_sp,
                                         d_allocator_p);

        error = clientStreamSocket->registerManager(this->getSelf(this));
        NTCCFG_TEST_FALSE(error);

        error = clientStreamSocket->open(d_parameters.d_transport,
                                         basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        bsl::shared_ptr<ntcr::StreamSocket> serverStreamSocket;
        serverStreamSocket.createInplace(d_allocator_p,
                                         options,
                                         resolver,
                                         d_reactor_sp,
                                         d_reactor_sp,
                                         d_metrics_sp,
                                         d_allocator_p);

        error = serverStreamSocket->registerManager(this->getSelf(this));
        NTCCFG_TEST_FALSE(error);

        error = serverStreamSocket->open(d_parameters.d_transport,
                                         basicServerSocket);
        NTCCFG_TEST_FALSE(error);
    }

    // Wait for all the stream sockets to become established.

    d_socketsEstablished.wait();

    // Start the timers for each stream socket.

    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_socketMapMutex);

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
        bslmt::LockGuard<bslmt::Mutex> guard(&d_socketMapMutex);

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
        bslmt::LockGuard<bslmt::Mutex> guard(&d_socketMapMutex);

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
    // reactor implementation may supports timestamping the socket API
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
            NTCCFG_TEST_EQ(d.type(), bdld::Datum::e_ARRAY);
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
            // of ntcr_datagramsocket does not timestamp any outgoing packet
            /// until the first TX timestamp is received from the reactor
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

                NTCCFG_TEST_GE(
                    statsArray[baseTxDelayBeforeSchedIndex + countOffset]
                        .theDouble(),
                    d_parameters.d_numMessages * txTimestampsPercentage);
                NTCCFG_TEST_GT(
                    statsArray[baseTxDelayBeforeSchedIndex + totalOffset]
                        .theDouble(),
                    0);
                NTCCFG_TEST_GT(
                    statsArray[baseTxDelayBeforeSchedIndex + minOffset]
                        .theDouble(),
                    0);
                NTCCFG_TEST_GT(
                    statsArray[baseTxDelayBeforeSchedIndex + avgOffset]
                        .theDouble(),
                    0);
                NTCCFG_TEST_GT(
                    statsArray[baseTxDelayBeforeSchedIndex + maxOffset]
                        .theDouble(),
                    0);

                NTCCFG_TEST_GE(
                    statsArray[baseTxDelayInSoftwareIndex + countOffset]
                        .theDouble(),
                    d_parameters.d_numMessages * txTimestampsPercentage);
                NTCCFG_TEST_GT(
                    statsArray[baseTxDelayInSoftwareIndex + totalOffset]
                        .theDouble(),
                    0);
                NTCCFG_TEST_GT(
                    statsArray[baseTxDelayInSoftwareIndex + minOffset]
                        .theDouble(),
                    0);
                NTCCFG_TEST_GT(
                    statsArray[baseTxDelayInSoftwareIndex + avgOffset]
                        .theDouble(),
                    0);
                NTCCFG_TEST_GT(
                    statsArray[baseTxDelayInSoftwareIndex + maxOffset]
                        .theDouble(),
                    0);

                NTCCFG_TEST_GE(
                    statsArray[baseTxDelayBeforeAckIndex + countOffset]
                        .theDouble(),
                    d_parameters.d_numMessages * txTimestampsPercentage);
                NTCCFG_TEST_GT(
                    statsArray[baseTxDelayBeforeAckIndex + totalOffset]
                        .theDouble(),
                    0);
                NTCCFG_TEST_GT(
                    statsArray[baseTxDelayBeforeAckIndex + minOffset]
                        .theDouble(),
                    0);
                NTCCFG_TEST_GT(
                    statsArray[baseTxDelayBeforeAckIndex + avgOffset]
                        .theDouble(),
                    0);
                NTCCFG_TEST_GT(
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

                NTCCFG_TEST_EQ(
                    statsArray[baseRxDelayIndex + countOffset].theDouble(),
                    d_parameters.d_numMessages);
                NTCCFG_TEST_GT(
                    statsArray[baseRxDelayIndex + totalOffset].theDouble(),
                    0);
                NTCCFG_TEST_GT(
                    statsArray[baseRxDelayIndex + minOffset].theDouble(),
                    0);
                NTCCFG_TEST_GT(
                    statsArray[baseRxDelayIndex + avgOffset].theDouble(),
                    0);
                NTCCFG_TEST_GT(
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
            bslmt::LockGuard<bslmt::Mutex> guard(&d_socketMapMutex);

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

void concern(ntsa::Transport::Value                transport,
             const bsl::shared_ptr<ntci::Reactor>& reactor,
             const test::Parameters&               parameters,
             bslma::Allocator*                     allocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket test starting");

    test::Parameters effectiveParameters = parameters;
    effectiveParameters.d_transport      = transport;

    bsl::shared_ptr<test::StreamSocketManager> streamSocketManager;
    streamSocketManager.createInplace(allocator,
                                      reactor,
                                      effectiveParameters,
                                      allocator);

    streamSocketManager->run();
    streamSocketManager.reset();

    NTCI_LOG_DEBUG("Stream socket test complete");

    reactor->stop();
}

void variation(const test::Parameters& parameters)
{
    test::Framework::execute(NTCCFG_BIND(&test::concern,
                                         NTCCFG_BIND_PLACEHOLDER_1,
                                         NTCCFG_BIND_PLACEHOLDER_2,
                                         parameters,
                                         NTCCFG_BIND_PLACEHOLDER_3));
}

namespace mock {

#define UNEXPECTED_CALL(unused) NTCCFG_TEST_TRUE(false && "unexpected call")
#if 1
class ResolverMock : public ntci::Resolver
{
  public:
    NTF_MOCK_METHOD_NEW(void, execute, const Functor&)
    NTF_MOCK_METHOD_NEW(void, moveAndExecute, FunctorSequence*, const Functor&)
  public:
    const bsl::shared_ptr<ntci::Strand>& strand() const override
    {
        UNEXPECTED_CALL();
        return dummyStrand;
    }
    NTF_MOCK_METHOD_NEW(ntsa::Error, start)
    NTF_MOCK_METHOD_NEW(void, shutdown)
    NTF_MOCK_METHOD_NEW(void, linger)
    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        setIpAddress,
                        const bslstl::StringRef&,
                        const bsl::vector<ntsa::IpAddress>&)
    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        addIpAddress,
                        const bslstl::StringRef&,
                        const bsl::vector<ntsa::IpAddress>&)
    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        addIpAddress,
                        const bslstl::StringRef&,
                        const ntsa::IpAddress&)

    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        setPort,
                        const bslstl::StringRef&,
                        const bsl::vector<ntsa::Port>&,
                        ntsa::Transport::Value)

    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        addPort,
                        const bslstl::StringRef&,
                        const bsl::vector<ntsa::Port>&,
                        ntsa::Transport::Value)

    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        addPort,
                        const bslstl::StringRef&,
                        ntsa::Port,
                        ntsa::Transport::Value)

    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        setLocalIpAddress,
                        const bsl::vector<ntsa::IpAddress>&)
    NTF_MOCK_METHOD_NEW(ntsa::Error, setHostname, const bsl::string&)
    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        setHostnameFullyQualified,
                        const bsl::string&)

    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        getIpAddress,
                        const bslstl::StringRef&,
                        const ntca::GetIpAddressOptions&,
                        const ntci::GetIpAddressCallback&)

    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        getDomainName,
                        const ntsa::IpAddress&,
                        const ntca::GetDomainNameOptions&,
                        const ntci::GetDomainNameCallback&)

    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        getPort,
                        const bslstl::StringRef&,
                        const ntca::GetPortOptions&,
                        const ntci::GetPortCallback&)

    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        getServiceName,
                        ntsa::Port,
                        const ntca::GetServiceNameOptions&,
                        const ntci::GetServiceNameCallback&)

    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        getEndpoint,
                        const bslstl::StringRef&,
                        const ntca::GetEndpointOptions&,
                        const ntci::GetEndpointCallback&)

    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        getLocalIpAddress,
                        bsl::vector<ntsa::IpAddress>*,
                        const ntsa::IpAddressOptions&)
    NTF_MOCK_METHOD_NEW(ntsa::Error, getHostname, bsl::string*)
    NTF_MOCK_METHOD_NEW(ntsa::Error, getHostnameFullyQualified, bsl::string*)
    NTF_MOCK_METHOD_NEW(bsl::shared_ptr<ntci::Strand>,
                        createStrand,
                        bslma::Allocator*)

    NTF_MOCK_METHOD_NEW(bsl::shared_ptr<ntci::Timer>,
                        createTimer,
                        const ntca::TimerOptions&,
                        const bsl::shared_ptr<ntci::TimerSession>&,
                        bslma::Allocator*)

    NTF_MOCK_METHOD_NEW(bsl::shared_ptr<ntci::Timer>,
                        createTimer,
                        const ntca::TimerOptions&,
                        const ntci::TimerCallback&,
                        bslma::Allocator*)
    NTF_MOCK_METHOD_CONST_NEW(bsls::TimeInterval, currentTime)

  private:
    bsl::shared_ptr<ntci::Strand> dummyStrand;
};

class BufferFactoryMock : public bdlbb::BlobBufferFactory
{
    NTF_MOCK_METHOD(void, allocate, bdlbb::BlobBuffer*)
};
#endif

struct MyClass {
    virtual void doSmth(int){};
    virtual void doSmth2(int*){};
    virtual void doSmth3(int&){};
    virtual void doSmth4(const int){};
    virtual void doSmth5(const int*){};
    virtual void doSmth6(const int&){};
};

class MyClassMock : public MyClass
{
  public:
    NTF_MOCK_METHOD(void, doSmth, int);
    NTF_MOCK_METHOD(void, doSmth2, int*);
    NTF_MOCK_METHOD(void, doSmth3, int&);
    NTF_MOCK_METHOD(void, doSmth4, const int);
    NTF_MOCK_METHOD(void, doSmth5, const int*);
    NTF_MOCK_METHOD(void, doSmth6, const int&);
};

void func()
{
    int  k   = 5;
    int* k_p = &k;

    int  tmp1   = 0;
    int* tmp1_p = &tmp1;

    int& k_r = k;

    const int* k_cp = &k;

    MyClassMock m;

    {
        m.expect_doSmth(5).willOnce().saveArg1(&tmp1);
    }

    {
        m.expect_doSmth2(k_p).willOnce().setArg1To(6).saveArg1(&tmp1_p);
    }

    {
        m.expect_doSmth3(k_r).willOnce().setArg1To(6).saveArg1(tmp1_p);
    }

    {
        m.expect_doSmth4(55).willOnce().saveArg1(&tmp1);
    }

    {
        // m.expect_doSmth5(k_p).willOnce().saveArg1(&k_cp);
        // m.expect_doSmth5(k_cp).willOnce();
    }
}

namespace {

struct My {
    virtual void f()            = 0;
    virtual int  f2()           = 0;
    virtual void f3(int)        = 0;
    virtual void f4(int*)       = 0;
    virtual void f5(const int*) = 0;
    virtual void f6(int&)       = 0;
    virtual void f7(int const&) = 0;
};

struct MyMock : public My {
    NTF_MOCK_METHOD_NEW_0(void, f)
    NTF_MOCK_METHOD_NEW_0(int, f2)
    NTF_MOCK_METHOD_NEW_1(void, f3, int)
    NTF_MOCK_METHOD_NEW_1(void, f4, int*)
    NTF_MOCK_METHOD_NEW_1(void, f5, const int*)
    NTF_MOCK_METHOD_NEW_1(void, f6, int&)
    NTF_MOCK_METHOD_NEW_1(void, f7, const int&)
};

void wegweg()
{
    // const bsl::nullopt_t doNotCare = bsl::nullopt;

    {  // void 0 args
        MyMock m;
        NTF_EXPECT_0(m, f).ONCE();
        NTF_EXPECT_0(m, f2).ONCE().RETURN(5);
    }
    {
        // void 1 arg int
        int    to;
        MyMock m;
        NTF_EXPECT_1(m, f3, NTF_EQ(2)).SAVE_ARG_1(TO(&to));
        // NTF_EXPECT_1(m, f3, NTF_EQ(2)).SAVE_ARG_1(TO(to)); //SHALL FAIL
        NTF_EXPECT_1(m, f3, NTF_EQ(2)).SET_ARG_1(FROM(5));  //TODO: SHALL FAIL

        NTF_EXPECT_1(m, f3, IGNORE_ARG);
    }

    {  // void 1 arg int*
        int    k  = 0;
        int*   to = 0;
        MyMock m;
        NTF_EXPECT_1(m, f4, NTF_EQ(&k));
        NTF_EXPECT_1(m, f4, NTF_EQ_DEREF(k));

        NTF_EXPECT_1(m, f4, NTF_EQ_DEREF(k)).SAVE_ARG_1(TO(&to));
        NTF_EXPECT_1(m, f4, NTF_EQ_DEREF(k)).SAVE_ARG_1(TO_DEREF(to));
        // NTF_EXPECT_1(m, f4, NTF_EQ_DEREF(k)).SAVE_ARG_1(TO(to)); //SHALL FAIL

        // NTF_EXPECT_1(m, f4, NTF_EQ(&k)).SET_ARG_1(FROM(5)); //SHALL FAIL
        NTF_EXPECT_1(m, f4, NTF_EQ(&k)).SET_ARG_1(FROM_DEREF(5));

        NTF_EXPECT_1(m, f4, NTF_EQ(&k))
            .SET_ARG_1(FROM(to));  // TODO: SHALL FAIL as  it is useless
    }

    {  // void 1 arg int const *
        int    k  = 0;
        int*   to = 0;
        MyMock m;
        NTF_EXPECT_1(m, f5, NTF_EQ(&k));
        NTF_EXPECT_1(m, f5, NTF_EQ_DEREF(k));
        // NTF_EXPECT_1(m, f5, NTF_EQ_DEREF(k)).SAVE_ARG_1(TO(&to)); // SHALL FAIL
        int const* to_const = 0;
        NTF_EXPECT_1(m, f5, NTF_EQ_DEREF(k)).SAVE_ARG_1(TO(&to_const));
        NTF_EXPECT_1(m, f5, NTF_EQ_DEREF(k)).SAVE_ARG_1(TO_DEREF(to));

        // NTF_EXPECT_1(m, f5, NTF_EQ(&k)).SET_ARG_1(FROM(5));// SHALL FAIL
        // NTF_EXPECT_1(m, f5, NTF_EQ(&k)).SET_ARG_1(FROM_DEREF(5)); //SHALL_FAIL

        const int* tmp;
        NTF_EXPECT_1(m, f5, NTF_EQ(&k))
            .SET_ARG_1(FROM(tmp));  //TODO: SHALL also fail as it is useless
    }

    {  // void 1 arg int &
        int    k  = 0;
        int*   to = 0;
        MyMock m;
        NTF_EXPECT_1(m, f6, NTF_EQ(k));
        // NTF_EXPECT_1(m, f6, NTF_EQ_DEREF(k)); //SHALL FAIL

        NTF_EXPECT_1(m, f6, NTF_EQ(k)).SAVE_ARG_1(TO(to));
        // NTF_EXPECT_1(m, f6, NTF_EQ(k)).SAVE_ARG_1(TO_DEREF(to)); // SHALL FAIL

        NTF_EXPECT_1(m, f6, NTF_EQ(k)).SET_ARG_1(FROM(5));
        // NTF_EXPECT_1(m, f6, NTF_EQ(k)).SET_ARG_1(FROM_DEREF(to)); // SHALL FAIL
    }

    {  // void 1 arg int const &
        int    k  = 0;
        int*   to = 0;
        MyMock m;
        NTF_EXPECT_1(m, f7, NTF_EQ(k));
        // NTF_EXPECT_1(m, f7, NTF_EQ_DEREF(k)); //SHALL FAIL
        NTF_EXPECT_1(m, f7, NTF_EQ(k)).SAVE_ARG_1(TO(to));
        // NTF_EXPECT_1(m, f7, NTF_EQ(k)).SAVE_ARG_1(TO_DEREF(&to)); // SHALL FAIL

        NTF_EXPECT_1(m, f7, NTF_EQ(k)).SET_ARG_1(FROM(5));  // TODO: SHALL FAIL

        int& k_r = k;
        NTF_EXPECT_1(m, f7, NTF_EQ(k))
            .SET_ARG_1(FROM(k_r));  // TODO: SHALL FAIL
    }
}

}

#if 1
class StreamSocketMock : public ntsi::StreamSocket
{
  public:
    NTF_MOCK_METHOD_CONST_NEW(ntsa::Handle, handle)
    NTF_MOCK_METHOD_NEW(ntsa::Error, open, ntsa::Transport::Value)
    NTF_MOCK_METHOD_NEW(ntsa::Error, acquire, ntsa::Handle)
    NTF_MOCK_METHOD_NEW(ntsa::Handle, release)

    NTF_MOCK_METHOD_NEW(ntsa::Error, bind, const ntsa::Endpoint&, bool)
    NTF_MOCK_METHOD_NEW(ntsa::Error, bindAny, ntsa::Transport::Value, bool)
    NTF_MOCK_METHOD_NEW(ntsa::Error, connect, const ntsa::Endpoint&)
  public:
    ntsa::Error send(ntsa::SendContext*       context,
                     const bdlbb::Blob&       data,
                     const ntsa::SendOptions& options) override
    {
        UNEXPECTED_CALL();
        return ntsa::Error();
    }
    ntsa::Error send(ntsa::SendContext*       context,
                     const ntsa::Data&        data,
                     const ntsa::SendOptions& options) override
    {
        UNEXPECTED_CALL();
        return ntsa::Error();
    }
    ntsa::Error receive(ntsa::ReceiveContext*       context,
                        bdlbb::Blob*                data,
                        const ntsa::ReceiveOptions& options) override
    {
        UNEXPECTED_CALL();
        return ntsa::Error();
    }
    ntsa::Error receive(ntsa::ReceiveContext*       context,
                        ntsa::Data*                 data,
                        const ntsa::ReceiveOptions& options) override
    {
        UNEXPECTED_CALL();
        return ntsa::Error();
    }

    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        receiveNotifications,
                        ntsa::NotificationQueue*)
    NTF_MOCK_METHOD_NEW(ntsa::Error, shutdown, ntsa::ShutdownType::Value)
    NTF_MOCK_METHOD_NEW(ntsa::Error, unlink)
    NTF_MOCK_METHOD_NEW(ntsa::Error, close)
    NTF_MOCK_METHOD_CONST_NEW(ntsa::Error, sourceEndpoint, ntsa::Endpoint*)
    NTF_MOCK_METHOD_CONST_NEW(ntsa::Error, remoteEndpoint, ntsa::Endpoint*)
    NTF_MOCK_METHOD_NEW(ntsa::Error, setBlocking, bool)
    NTF_MOCK_METHOD_NEW(ntsa::Error, setOption, const ntsa::SocketOption&)

    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        getOption,
                        ntsa::SocketOption*,
                        ntsa::SocketOptionType::Value)
    NTF_MOCK_METHOD_NEW(ntsa::Error, getLastError, ntsa::Error*)
    NTF_MOCK_METHOD_CONST_NEW(bsl::size_t, maxBuffersPerSend)
    NTF_MOCK_METHOD_CONST_NEW(bsl::size_t, maxBuffersPerReceive)
};

class DataPoolMock : public ntci::DataPool
{
  public:
    NTF_MOCK_METHOD_NEW(bsl::shared_ptr<ntsa::Data>, createIncomingData)
    NTF_MOCK_METHOD_NEW(bsl::shared_ptr<ntsa::Data>, createOutgoingData);
    NTF_MOCK_METHOD_NEW(bsl::shared_ptr<bdlbb::Blob>, createIncomingBlob);
    NTF_MOCK_METHOD_NEW(bsl::shared_ptr<bdlbb::Blob>, createOutgoingBlob);
    NTF_MOCK_METHOD_NEW(void, createIncomingBlobBuffer, bdlbb::BlobBuffer*)
    NTF_MOCK_METHOD_NEW(void, createOutgoingBlobBuffer, bdlbb::BlobBuffer*)

  public:
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& incomingBlobBufferFactory()
        const override
    {
        UNEXPECTED_CALL();
        return dummyBlobBufferFactory;
    }
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& outgoingBlobBufferFactory()
        const override
    {
        UNEXPECTED_CALL();
        return dummyBlobBufferFactory;
    }

  private:
    bsl::shared_ptr<bdlbb::BlobBufferFactory> dummyBlobBufferFactory;
};

class ReactorMock : public ntci::Reactor
{
  public:
    NTF_MOCK_METHOD_NEW(bsl::shared_ptr<ntci::DatagramSocket>,
                        createDatagramSocket,
                        const ntca::DatagramSocketOptions&,
                        bslma::Allocator*)
    NTF_MOCK_METHOD_NEW(bsl::shared_ptr<ntsa::Data>, createIncomingData)
    NTF_MOCK_METHOD_NEW(bsl::shared_ptr<ntsa::Data>, createOutgoingData)
    NTF_MOCK_METHOD_NEW(bsl::shared_ptr<bdlbb::Blob>, createIncomingBlob)
    NTF_MOCK_METHOD_NEW(bsl::shared_ptr<bdlbb::Blob>, createOutgoingBlob)
    NTF_MOCK_METHOD_NEW(void, createIncomingBlobBuffer, bdlbb::BlobBuffer*)
    NTF_MOCK_METHOD_NEW(void, createOutgoingBlobBuffer, bdlbb::BlobBuffer*)

  public:
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& incomingBlobBufferFactory()
        const override
    {
        if (d_incomingBlobBufferFactory_result.isNull()) {
            UNEXPECTED_CALL();
        }
        return d_incomingBlobBufferFactory_result.value();
    }
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& outgoingBlobBufferFactory()
        const override
    {
        if (d_outgoingBlobBufferFactory_result.isNull()) {
            UNEXPECTED_CALL();
        }
        return d_outgoingBlobBufferFactory_result.value();
    }

    NTF_MOCK_METHOD_NEW(ntci::Waiter,
                        registerWaiter,
                        const ntca::WaiterOptions&)
    NTF_MOCK_METHOD_NEW(void, deregisterWaiter, ntci::Waiter)
    NTF_MOCK_METHOD_NEW(void, run, ntci::Waiter)
    NTF_MOCK_METHOD_NEW(void, poll, ntci::Waiter)
    NTF_MOCK_METHOD_NEW(void, interruptOne)
    NTF_MOCK_METHOD_NEW(void, interruptAll)
    NTF_MOCK_METHOD_NEW(void, stop)
    NTF_MOCK_METHOD_NEW(void, restart)
    NTF_MOCK_METHOD_NEW(void, execute, const Functor&)
    NTF_MOCK_METHOD_NEW(void, moveAndExecute, FunctorSequence*, const Functor&)
    NTF_MOCK_METHOD_NEW(bsl::shared_ptr<ntci::ListenerSocket>,
                        createListenerSocket,
                        const ntca::ListenerSocketOptions&,
                        bslma::Allocator*)

    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        attachSocket,
                        const bsl::shared_ptr<ntci::ReactorSocket>&)
    NTF_MOCK_METHOD_NEW(ntsa::Error, attachSocket, ntsa::Handle)
    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        showReadable,
                        const bsl::shared_ptr<ntci::ReactorSocket>&,
                        const ntca::ReactorEventOptions&)

  public:
    ntsa::Error showReadable(
        ntsa::Handle                      handle,
        const ntca::ReactorEventOptions&  options,
        const ntci::ReactorEventCallback& callback) override
    {
        UNEXPECTED_CALL();
        return ntsa::Error();
    }

    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        showWritable,
                        const bsl::shared_ptr<ntci::ReactorSocket>&,
                        const ntca::ReactorEventOptions&)

  public:
    ntsa::Error showWritable(
        ntsa::Handle                      handle,
        const ntca::ReactorEventOptions&  options,
        const ntci::ReactorEventCallback& callback) override
    {
        UNEXPECTED_CALL();
        return ntsa::Error();
    }

    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        showError,
                        const bsl::shared_ptr<ntci::ReactorSocket>&,
                        const ntca::ReactorEventOptions&)
  public:
    ntsa::Error showError(ntsa::Handle                      handle,
                          const ntca::ReactorEventOptions&  options,
                          const ntci::ReactorEventCallback& callback) override
    {
        UNEXPECTED_CALL();
        return ntsa::Error();
    }
    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        hideReadable,
                        const bsl::shared_ptr<ntci::ReactorSocket>&)
    NTF_MOCK_METHOD_NEW(ntsa::Error, hideReadable, ntsa::Handle)
    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        hideWritable,
                        const bsl::shared_ptr<ntci::ReactorSocket>&)
    NTF_MOCK_METHOD_NEW(ntsa::Error, hideWritable, ntsa::Handle)
    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        hideError,
                        const bsl::shared_ptr<ntci::ReactorSocket>&)
    NTF_MOCK_METHOD_NEW(ntsa::Error, hideError, ntsa::Handle)
    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        detachSocket,
                        const bsl::shared_ptr<ntci::ReactorSocket>&)
    NTF_MOCK_METHOD_NEW(ntsa::Error, detachSocket, ntsa::Handle)

    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        detachSocket,
                        const bsl::shared_ptr<ntci::ReactorSocket>&,
                        const ntci::SocketDetachedCallback&)
    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        detachSocket,
                        ntsa::Handle,
                        const ntci::SocketDetachedCallback&)

    NTF_MOCK_METHOD_NEW(ntsa::Error, closeAll)
    NTF_MOCK_METHOD_NEW(void, incrementLoad, const ntca::LoadBalancingOptions&)
    NTF_MOCK_METHOD_NEW(void, decrementLoad, const ntca::LoadBalancingOptions&)

    NTF_MOCK_METHOD_NEW(void, drainFunctions)
    NTF_MOCK_METHOD_NEW(void, clearFunctions)
    NTF_MOCK_METHOD_NEW(void, clearTimers)
    NTF_MOCK_METHOD_NEW(void, clearSockets)
    NTF_MOCK_METHOD_NEW(void, clear)
    NTF_MOCK_METHOD_CONST_NEW(size_t, numSockets)
    NTF_MOCK_METHOD_CONST_NEW(size_t, maxSockets)
    NTF_MOCK_METHOD_CONST_NEW(size_t, numTimers)
    NTF_MOCK_METHOD_CONST_NEW(size_t, maxTimers)
    NTF_MOCK_METHOD_CONST_NEW(bool, autoAttach)
    NTF_MOCK_METHOD_CONST_NEW(bool, autoDetach)
    NTF_MOCK_METHOD_CONST_NEW(bool, oneShot)
    NTF_MOCK_METHOD_CONST_NEW(ntca::ReactorEventTrigger::Value, trigger)
    NTF_MOCK_METHOD_CONST_NEW(size_t, load)
    NTF_MOCK_METHOD_CONST_NEW(bslmt::ThreadUtil::Handle, threadHandle)
    NTF_MOCK_METHOD_CONST_NEW(size_t, threadIndex)
    NTF_MOCK_METHOD_CONST_NEW(bool, empty)

  public:
    const bsl::shared_ptr<ntci::DataPool>& dataPool() const override
    {
        if (d_dataPool_result.isNull()) {
            UNEXPECTED_CALL();
        }
        return d_dataPool_result.value();
    }

    NTF_MOCK_METHOD_CONST_NEW(bool, supportsOneShot, bool)
    NTF_MOCK_METHOD_CONST_NEW(bool,
                              supportsTrigger,
                              ntca::ReactorEventTrigger::Value)

    NTF_MOCK_METHOD_NEW(bsl::shared_ptr<ntci::Reactor>,
                        acquireReactor,
                        const ntca::LoadBalancingOptions&)
    NTF_MOCK_METHOD_NEW(void,
                        releaseReactor,
                        const bsl::shared_ptr<ntci::Reactor>&,
                        const ntca::LoadBalancingOptions&)
    NTF_MOCK_METHOD_NEW(bool, acquireHandleReservation)
    NTF_MOCK_METHOD_NEW(void, releaseHandleReservation)

    NTF_MOCK_METHOD_CONST_NEW(size_t, numReactors)
    NTF_MOCK_METHOD_CONST_NEW(size_t, numThreads)
    NTF_MOCK_METHOD_CONST_NEW(size_t, minThreads)
    NTF_MOCK_METHOD_CONST_NEW(size_t, maxThreads)

    NTF_MOCK_METHOD_NEW(bsl::shared_ptr<ntci::Strand>,
                        createStrand,
                        bslma::Allocator*)

    NTF_MOCK_METHOD_NEW(bsl::shared_ptr<ntci::StreamSocket>,
                        createStreamSocket,
                        const ntca::StreamSocketOptions&,
                        bslma::Allocator*)
  public:
    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&                  options,
        const bsl::shared_ptr<ntci::TimerSession>& session,
        bslma::Allocator*                          basicAllocator) override
    {
        UNEXPECTED_CALL();

        return bsl::shared_ptr<ntci::Timer>();
    }
    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&  options,
        const ntci::TimerCallback& callback,
        bslma::Allocator*          basicAllocator) override
    {
        return d_invocation_createTimer.invoke(options,
                                               callback,
                                               basicAllocator);
    }
    const bsl::shared_ptr<ntci::Strand>& strand() const override
    {
        UNEXPECTED_CALL();
        return dummyStrand;
    }
    NTF_MOCK_METHOD_CONST_NEW(bsls::TimeInterval, currentTime)

  public:
    // Helper data structures

    struct Invocation_createTimer {
      private:
        struct InvocationData {
            int                                                d_expectedCalls;
            bdlb::NullableValue<ntca::TimerOptions>            d_arg1;
            bdlb::NullableValue<ntci::TimerCallback>           d_arg2;
            bdlb::NullableValue<bslma::Allocator*>             d_arg3;
            bdlb::NullableValue<bsl::shared_ptr<ntci::Timer> > d_result;
            ntca::TimerOptions*                                d_arg1_out;
            ntci::TimerCallback*                               d_arg2_out;
            bslma::Allocator**                                 d_arg3_out;

            InvocationData()
            : d_expectedCalls(0)
            , d_arg1()
            , d_arg2()
            , d_arg3()
            , d_result()
            , d_arg1_out(0)
            , d_arg2_out(0)
            , d_arg3_out(0)
            {
            }
        };

      public:
        Invocation_createTimer& expect(
            const bdlb::NullableValue<ntca::TimerOptions>&  arg1,
            const bdlb::NullableValue<ntci::TimerCallback>& arg2,
            const bdlb::NullableValue<bslma::Allocator*>&   arg3)
        {
            d_invocations.emplace_back();
            InvocationData& invocation = d_invocations.back();
            invocation.d_arg1          = arg1;
            invocation.d_arg2          = arg2;
            invocation.d_arg3          = arg3;
            return *this;
        }
        Invocation_createTimer& willOnce()
        {
            NTCCFG_TEST_FALSE(d_invocations.empty());

            InvocationData& invocation = d_invocations.back();
            NTCCFG_TEST_EQ(invocation.d_expectedCalls, 0);

            invocation.d_expectedCalls = 1;
            return *this;
        }
        Invocation_createTimer& willAlways()
        {
            NTCCFG_TEST_FALSE(d_invocations.empty());

            InvocationData& invocation = d_invocations.back();
            NTCCFG_TEST_EQ(invocation.d_expectedCalls, 0);

            invocation.d_expectedCalls = -1;
            return *this;
        }
        //        Invocation_createTimer& times(int val){} //TODO: multiple calls

        Invocation_createTimer& willReturn(
            const bsl::shared_ptr<ntci::Timer>& result)
        {
            NTCCFG_TEST_FALSE(d_invocations.empty());
            InvocationData& invocation = d_invocations.back();
            invocation.d_result        = result;
            return *this;
        }

        Invocation_createTimer& saveArg1(ntca::TimerOptions& arg)
        {
            NTCCFG_TEST_FALSE(d_invocations.empty());
            InvocationData& invocation = d_invocations.back();
            NTCCFG_TEST_EQ(invocation.d_arg1_out, 0);
            invocation.d_arg1_out = &arg;
            return *this;
        }
        Invocation_createTimer& saveArg2(ntci::TimerCallback& arg)
        {
            NTCCFG_TEST_FALSE(d_invocations.empty());
            InvocationData& invocation = d_invocations.back();
            NTCCFG_TEST_EQ(invocation.d_arg2_out, 0);
            invocation.d_arg2_out = &arg;
            return *this;
        }
        Invocation_createTimer& saveArg3(bslma::Allocator*& arg)
        {
            NTCCFG_TEST_FALSE(d_invocations.empty());
            InvocationData& invocation = d_invocations.back();
            NTCCFG_TEST_EQ(invocation.d_arg3_out, 0);
            invocation.d_arg3_out = &arg;
            return *this;
        }

        bsl::shared_ptr<ntci::Timer> invoke(const ntca::TimerOptions&  arg1,
                                            const ntci::TimerCallback& arg2,
                                            bslma::Allocator*          arg3)
        {
            NTCCFG_TEST_FALSE(d_invocations.empty());
            InvocationData& invocation = d_invocations.front();

            if (invocation.d_expectedCalls != -1) {
                NTCCFG_TEST_GE(invocation.d_expectedCalls, 1);
            }

            if (invocation.d_arg1.has_value()) {
                NTCCFG_TEST_EQ(arg1, invocation.d_arg1.value());
            }

            if (invocation.d_arg2.has_value()) {
                NTCCFG_TEST_EQ(arg2, invocation.d_arg2.value());
            }

            if (invocation.d_arg3.has_value()) {
                NTCCFG_TEST_EQ(arg3, invocation.d_arg3.value());
            }

            if (invocation.d_arg1_out) {
                *invocation.d_arg1_out = arg1;
            }

            if (invocation.d_arg2_out) {
                *invocation.d_arg2_out = arg2;
            }

            if (invocation.d_arg3_out) {
                *invocation.d_arg3_out = arg3;
            }

            NTCCFG_TEST_TRUE(invocation.d_result.has_value());
            const auto result = invocation.d_result.value();

            if (invocation.d_expectedCalls != -1) {
                --invocation.d_expectedCalls;
                if (invocation.d_expectedCalls == 0) {
                    d_invocations.pop_front();
                }
            }

            return result;
        }

      private:
        bsl::list<InvocationData> d_invocations;
    };

  public:
    // auxiliary methods
    void expect_dataPool_WillAlwaysReturn(
        const bsl::shared_ptr<ntci::DataPool>& dataPool)
    {
        d_dataPool_result = dataPool;
    }

    void expect_outgoingBlobBufferFactory_WillAlwaysReturn(
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& bufferFactory)
    {
        d_outgoingBlobBufferFactory_result = bufferFactory;
    }

    void expect_incomingBlobBufferFactory_WillAlwaysReturn(
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& bufferFactory)
    {
        d_incomingBlobBufferFactory_result = bufferFactory;
    }

    Invocation_createTimer& expect_createTimer(
        const bdlb::NullableValue<ntca::TimerOptions>&  arg1,
        const bdlb::NullableValue<ntci::TimerCallback>& arg2,
        const bdlb::NullableValue<bslma::Allocator*>&   arg3)
    {
        return d_invocation_createTimer.expect(arg1, arg2, arg3);
    }

  private:
    bdlb::NullableValue<bsl::shared_ptr<bdlbb::BlobBufferFactory> >
        d_incomingBlobBufferFactory_result;
    bdlb::NullableValue<bsl::shared_ptr<bdlbb::BlobBufferFactory> >
                                  d_outgoingBlobBufferFactory_result;
    bsl::shared_ptr<ntci::Strand> dummyStrand;
    bdlb::NullableValue<bsl::shared_ptr<ntci::DataPool> > d_dataPool_result;

    Invocation_createTimer d_invocation_createTimer;
};

class TimerMock : public ntci::Timer
{
    NTF_MOCK_METHOD_NEW(ntsa::Error,
                        schedule,
                        const bsls::TimeInterval&,
                        const bsls::TimeInterval&)
    NTF_MOCK_METHOD_NEW(ntsa::Error, cancel)
    NTF_MOCK_METHOD_NEW(ntsa::Error, close)
  public:
    void arrive(const bsl::shared_ptr<ntci::Timer>& self,
                const bsls::TimeInterval&           now,
                const bsls::TimeInterval&           deadline) override
    {
        UNEXPECTED_CALL();
    }
    NTF_MOCK_METHOD_CONST_NEW(void*, handle)
    NTF_MOCK_METHOD_CONST_NEW(int, id)
    NTF_MOCK_METHOD_CONST_NEW(bool, oneShot)
    NTF_MOCK_METHOD_CONST_NEW(bslmt::ThreadUtil::Handle, threadHandle)
    NTF_MOCK_METHOD_CONST_NEW(size_t, threadIndex)

  public:
    const bsl::shared_ptr<ntci::Strand>& strand() const override
    {
        UNEXPECTED_CALL();
        return dummyStrand;
    }
    NTF_MOCK_METHOD_CONST_NEW(bsls::TimeInterval, currentTime)

  private:
    bsl::shared_ptr<ntci::Strand> dummyStrand;
};
#endif
}  // close namespace mock

}  // close namespace test

NTCCFG_TEST_CASE(1)
{
    // Concern: Breathing test.

    test::Parameters parameters;
    parameters.d_numTimers         = 0;
    parameters.d_numSocketPairs    = 1;
    parameters.d_numMessages       = 1;
    parameters.d_messageSize       = 32;
    parameters.d_useAsyncCallbacks = false;

    test::variation(parameters);
}

NTCCFG_TEST_CASE(2)
{
    // Concern: Breathing test using asynchronous callbacks.

    test::Parameters parameters;
    parameters.d_numTimers         = 0;
    parameters.d_numSocketPairs    = 1;
    parameters.d_numMessages       = 1;
    parameters.d_messageSize       = 32;
    parameters.d_useAsyncCallbacks = true;

    test::variation(parameters);
}

NTCCFG_TEST_CASE(3)
{
    // Concern: Minimal read queue high watermark.

    test::Parameters parameters;
    parameters.d_numTimers              = 0;
    parameters.d_numSocketPairs         = 1;
    parameters.d_numMessages            = 100;
    parameters.d_messageSize            = 1024 * 32;
    parameters.d_useAsyncCallbacks      = false;
    parameters.d_readQueueHighWatermark = 1;

    test::variation(parameters);
}

NTCCFG_TEST_CASE(4)
{
    // Concern: Minimal read queue high watermark using asynchronous callbacks.

    test::Parameters parameters;
    parameters.d_numTimers              = 0;
    parameters.d_numSocketPairs         = 1;
    parameters.d_numMessages            = 100;
    parameters.d_messageSize            = 1024 * 32;
    parameters.d_useAsyncCallbacks      = true;
    parameters.d_readQueueHighWatermark = 1;

    test::variation(parameters);
}

NTCCFG_TEST_CASE(5)
{
    // Concern: Minimal write queue high watermark.

    test::Parameters parameters;
    parameters.d_numTimers               = 0;
    parameters.d_numSocketPairs          = 1;
    parameters.d_numMessages             = 100;
    parameters.d_messageSize             = 1024 * 32;
    parameters.d_useAsyncCallbacks       = false;
    parameters.d_writeQueueHighWatermark = 1;
    parameters.d_sendBufferSize          = 32 * 1024;

    test::variation(parameters);
}

NTCCFG_TEST_CASE(6)
{
    // Concern: Minimal write queue high watermark using asynchronous
    // callbacks.

    test::Parameters parameters;
    parameters.d_numTimers               = 0;
    parameters.d_numSocketPairs          = 1;
    parameters.d_numMessages             = 100;
    parameters.d_messageSize             = 1024 * 32;
    parameters.d_useAsyncCallbacks       = true;
    parameters.d_writeQueueHighWatermark = 1;
    parameters.d_sendBufferSize          = 32 * 1024;

    test::variation(parameters);
}

NTCCFG_TEST_CASE(7)
{
#if !defined(BSLS_PLATFORM_OS_AIX)

    // Concern: Rate limit copying from the receive buffer.

    test::Parameters parameters;
    parameters.d_numTimers         = 0;
    parameters.d_numSocketPairs    = 1;
    parameters.d_numMessages       = 1;
    parameters.d_messageSize       = 32 * 1024 * 4;
    parameters.d_useAsyncCallbacks = false;
    parameters.d_readRate          = 32 * 1024;
    parameters.d_receiveBufferSize = 32 * 1024;

    test::variation(parameters);

#endif
}

NTCCFG_TEST_CASE(8)
{
    // Concern: Rate limit copying from the receive buffer using
    // asynchronous callbacks.

    test::Parameters parameters;
    parameters.d_numTimers         = 0;
    parameters.d_numSocketPairs    = 1;
    parameters.d_numMessages       = 1;
    parameters.d_messageSize       = 32 * 1024 * 4;
    parameters.d_useAsyncCallbacks = true;
    parameters.d_readRate          = 32 * 1024;
    parameters.d_receiveBufferSize = 32 * 1024;

    test::variation(parameters);
}

NTCCFG_TEST_CASE(9)
{
    // Concern: Rate limit copying to the send buffer.

    test::Parameters parameters;
    parameters.d_numTimers         = 0;
    parameters.d_numSocketPairs    = 1;
    parameters.d_numMessages       = 1;
    parameters.d_messageSize       = 32 * 1024 * 4;
    parameters.d_useAsyncCallbacks = false;
    parameters.d_writeRate         = 32 * 1024;
    parameters.d_sendBufferSize    = 32 * 1024;

    test::variation(parameters);
}

NTCCFG_TEST_CASE(10)
{
    // Concern: Rate limit copying to the send buffer using asynchronous
    // callbacks.

    test::Parameters parameters;
    parameters.d_numTimers         = 0;
    parameters.d_numSocketPairs    = 1;
    parameters.d_numMessages       = 1;
    parameters.d_messageSize       = 32 * 1024 * 4;
    parameters.d_useAsyncCallbacks = true;
    parameters.d_writeRate         = 32 * 1024;
    parameters.d_sendBufferSize    = 32 * 1024;

    test::variation(parameters);
}

NTCCFG_TEST_CASE(11)
{
    // Concern: Stress test using the read queue low watermark.

    // The test currently fails sporadically on Linux on CI build machines
    // with "Assertion failed: !d_chronology_sp->hasAnyDeferred()".
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0
    test::Parameters parameters;
    parameters.d_numTimers         = 100;
    parameters.d_numSocketPairs    = 100;
    parameters.d_numMessages       = 32;
    parameters.d_messageSize       = 1024;
    parameters.d_useAsyncCallbacks = false;

    test::variation(parameters);
#endif
}

NTCCFG_TEST_CASE(12)
{
    // Concern: Stress test using asynchronous callbacks.

    // The test currently fails sporadically on Linux on CI build machines
    // with "Assertion failed: !d_chronology_sp->hasAnyDeferred()".
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0
    test::Parameters parameters;
    parameters.d_numTimers         = 100;
    parameters.d_numSocketPairs    = 100;
    parameters.d_numMessages       = 32;
    parameters.d_messageSize       = 1024;
    parameters.d_useAsyncCallbacks = true;

    test::variation(parameters);
#endif
}

namespace test {
namespace concern13 {

void processReceive(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                    const bsl::shared_ptr<ntci::Receiver>&     receiver,
                    const bsl::shared_ptr<bdlbb::Blob>&        data,
                    const ntca::ReceiveEvent&                  event,
                    bslmt::Semaphore*                          semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing receive event type %s: %s",
                   ntca::ReceiveEventType::toString(event.type()),
                   event.context().error().text().c_str());

    NTCCFG_TEST_EQ(event.type(), ntca::ReceiveEventType::e_ERROR);
    NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_WOULD_BLOCK);

    semaphore->post();
}

void execute(ntsa::Transport::Value                transport,
             const bsl::shared_ptr<ntci::Reactor>& reactor,
             const test::Parameters&               parameters,
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

    bsl::shared_ptr<ntcr::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntcr::StreamSocket> serverStreamSocket;
    {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntcd::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntcd::StreamSocket> basicServerSocket;

        error = ntcd::Simulation::createStreamSocketPair(&basicClientSocket,
                                                         &basicServerSocket,
                                                         transport);
        NTCCFG_TEST_FALSE(error);

        clientStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         reactor,
                                         reactor,
                                         metrics,
                                         allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         reactor,
                                         reactor,
                                         metrics,
                                         allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
    }

    bsls::TimeInterval receiveTimeout;
    receiveTimeout.setTotalMilliseconds(k_RECEIVE_TIMEOUT_IN_MILLISECONDS);

    bsls::TimeInterval receiveDeadline =
        serverStreamSocket->currentTime() + receiveTimeout;

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setDeadline(receiveDeadline);

    ntci::ReceiveCallback receiveCallback =
        serverStreamSocket->createReceiveCallback(
            NTCCFG_BIND(&processReceive,
                        serverStreamSocket,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        &semaphore),
            allocator);

    error = serverStreamSocket->receive(receiveOptions, receiveCallback);
    NTCCFG_TEST_OK(error);

    semaphore.wait();

    {
        ntci::StreamSocketCloseGuard clientStreamSocketCloseGuard(
            clientStreamSocket);

        ntci::StreamSocketCloseGuard serverStreamSocketCloseGuard(
            serverStreamSocket);
    }

    NTCI_LOG_DEBUG("Stream socket receive deadline test complete");

    reactor->stop();
}

}  // close namespace concern13
}  // close namespace test

NTCCFG_TEST_CASE(13)
{
    // Concern: Receive deadlines.

    test::Parameters parameters;

    test::Framework::execute(NTCCFG_BIND(&test::concern13::execute,
                                         NTCCFG_BIND_PLACEHOLDER_1,
                                         NTCCFG_BIND_PLACEHOLDER_2,
                                         parameters,
                                         NTCCFG_BIND_PLACEHOLDER_3));
}

namespace test {
namespace concern14 {

void processSend(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
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
        NTCCFG_TEST_EQ(event.type(), ntca::SendEventType::e_ERROR);
        NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_WOULD_BLOCK);
    }
    else {
        NTCI_LOG_INFO("Message %s has been sent", name.c_str());
    }

    semaphore->post();
}

void processReceive(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
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
        NTCCFG_TEST_EQ(event.type(), ntca::ReceiveEventType::e_ERROR);
        NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_WOULD_BLOCK);
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

                NTCCFG_TEST_EQ(f, e);
                ++position;
            }
        }

        NTCI_LOG_DEBUG("Comparing message %s: OK", name.c_str());
    }

    semaphore->post();
}

void execute(ntsa::Transport::Value                transport,
             const bsl::shared_ptr<ntci::Reactor>& reactor,
             const test::Parameters&               parameters,
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

    bsl::shared_ptr<ntcr::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntcr::StreamSocket> serverStreamSocket;
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
        NTCCFG_TEST_FALSE(error);

        clientStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         reactor,
                                         reactor,
                                         metrics,
                                         allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         reactor,
                                         reactor,
                                         metrics,
                                         allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
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
        NTCCFG_TEST_TRUE(!error);
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
                NTCCFG_BIND(&processSend,
                            clientStreamSocket,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            bsl::string("B"),
                            ntsa::Error(ntsa::Error::e_CANCELLED),
                            &sendSemaphore),
                allocator);

        error = clientStreamSocket->send(*dataB, sendOptions, sendCallback);
        NTCCFG_TEST_TRUE(!error);
    }

    NTCI_LOG_DEBUG("Sending message C");
    {
        ntca::SendOptions sendOptions;

        error = clientStreamSocket->send(*dataC, sendOptions);
        NTCCFG_TEST_TRUE(!error);
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
                NTCCFG_BIND(&processReceive,
                            serverStreamSocket,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            bsl::string("A"),
                            ntsa::Error(ntsa::Error::e_OK),
                            &receiveSemaphore),
                allocator);

        error = serverStreamSocket->receive(receiveOptions, receiveCallback);
        NTCCFG_TEST_OK(error);
    }

    {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setSize(k_MESSAGE_C_SIZE);

        ntci::ReceiveCallback receiveCallback =
            serverStreamSocket->createReceiveCallback(
                NTCCFG_BIND(&processReceive,
                            serverStreamSocket,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            bsl::string("C"),
                            ntsa::Error(ntsa::Error::e_OK),
                            &receiveSemaphore),
                allocator);

        error = serverStreamSocket->receive(receiveOptions, receiveCallback);
        NTCCFG_TEST_OK(error);
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

    reactor->stop();
}

}  // close namespace concern14
}  // close namespace test

NTCCFG_TEST_CASE(14)
{
    // Concern: Send deadlines.

    test::Parameters parameters;

    test::Framework::execute(NTCCFG_BIND(&test::concern14::execute,
                                         NTCCFG_BIND_PLACEHOLDER_1,
                                         NTCCFG_BIND_PLACEHOLDER_2,
                                         parameters,
                                         NTCCFG_BIND_PLACEHOLDER_3));
}

namespace test {
namespace concern15 {

void processReceive(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                    const bsl::shared_ptr<ntci::Receiver>&     receiver,
                    const bsl::shared_ptr<bdlbb::Blob>&        data,
                    const ntca::ReceiveEvent&                  event,
                    bslmt::Semaphore*                          semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing receive event type %s: %s",
                   ntca::ReceiveEventType::toString(event.type()),
                   event.context().error().text().c_str());

    NTCCFG_TEST_EQ(event.type(), ntca::ReceiveEventType::e_ERROR);
    NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_CANCELLED);

    semaphore->post();
}

void cancelReceive(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                   const ntca::ReceiveToken                   token)
{
    ntsa::Error error = streamSocket->cancel(token);
    NTCCFG_TEST_FALSE(error);
}

void execute(ntsa::Transport::Value                transport,
             const bsl::shared_ptr<ntci::Reactor>& reactor,
             const test::Parameters&               parameters,
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

    bsl::shared_ptr<ntcr::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntcr::StreamSocket> serverStreamSocket;
    {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntcd::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntcd::StreamSocket> basicServerSocket;

        error = ntcd::Simulation::createStreamSocketPair(&basicClientSocket,
                                                         &basicServerSocket,
                                                         transport);
        NTCCFG_TEST_FALSE(error);

        clientStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         reactor,
                                         reactor,
                                         metrics,
                                         allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         reactor,
                                         reactor,
                                         metrics,
                                         allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
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
            NTCCFG_BIND(&processReceive,
                        serverStreamSocket,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        &semaphore),
            allocator);

    error = serverStreamSocket->receive(receiveOptions, receiveCallback);
    NTCCFG_TEST_OK(error);

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
    NTCCFG_TEST_FALSE(error);

    semaphore.wait();

    {
        ntci::StreamSocketCloseGuard clientStreamSocketCloseGuard(
            clientStreamSocket);

        ntci::StreamSocketCloseGuard serverStreamSocketCloseGuard(
            serverStreamSocket);
    }

    NTCI_LOG_DEBUG("Stream socket receive cancellation test complete");

    reactor->stop();
}

}  // close namespace concern15
}  // close namespace test

NTCCFG_TEST_CASE(15)
{
    // Concern: Receive cancellation.

    test::Parameters parameters;

    test::Framework::execute(NTCCFG_BIND(&test::concern15::execute,
                                         NTCCFG_BIND_PLACEHOLDER_1,
                                         NTCCFG_BIND_PLACEHOLDER_2,
                                         parameters,
                                         NTCCFG_BIND_PLACEHOLDER_3));
}

namespace test {
namespace concern16 {

void processSend(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
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
        NTCCFG_TEST_EQ(event.type(), ntca::SendEventType::e_ERROR);
        NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_CANCELLED);
    }
    else {
        NTCI_LOG_INFO("Message %s has been sent", name.c_str());
    }

    semaphore->post();
}

void processReceive(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
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
        NTCCFG_TEST_EQ(event.type(), ntca::ReceiveEventType::e_ERROR);
        NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_CANCELLED);
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

                NTCCFG_TEST_EQ(f, e);
                ++position;
            }
        }

        NTCI_LOG_DEBUG("Comparing message %s: OK", name.c_str());
    }

    semaphore->post();
}

void cancelSend(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                const ntca::SendToken                      token)
{
    ntsa::Error error = streamSocket->cancel(token);
    NTCCFG_TEST_FALSE(error);
}

void execute(ntsa::Transport::Value                transport,
             const bsl::shared_ptr<ntci::Reactor>& reactor,
             const test::Parameters&               parameters,
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

    bsl::shared_ptr<ntcr::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntcr::StreamSocket> serverStreamSocket;
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
        NTCCFG_TEST_FALSE(error);

        clientStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         reactor,
                                         reactor,
                                         metrics,
                                         allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         reactor,
                                         reactor,
                                         metrics,
                                         allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
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
        NTCCFG_TEST_TRUE(!error);
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
                NTCCFG_BIND(&processSend,
                            clientStreamSocket,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            bsl::string("B"),
                            ntsa::Error(ntsa::Error::e_CANCELLED),
                            &sendSemaphore),
                allocator);

        error = clientStreamSocket->send(*dataB, sendOptions, sendCallback);
        NTCCFG_TEST_TRUE(!error);

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
        NTCCFG_TEST_FALSE(error);
    }

    NTCI_LOG_DEBUG("Sending message C");
    {
        ntca::SendOptions sendOptions;

        error = clientStreamSocket->send(*dataC, sendOptions);
        NTCCFG_TEST_TRUE(!error);
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
                NTCCFG_BIND(&processReceive,
                            serverStreamSocket,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            bsl::string("A"),
                            ntsa::Error(ntsa::Error::e_OK),
                            &receiveSemaphore),
                allocator);

        error = serverStreamSocket->receive(receiveOptions, receiveCallback);
        NTCCFG_TEST_OK(error);
    }

    {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setSize(k_MESSAGE_C_SIZE);

        ntci::ReceiveCallback receiveCallback =
            serverStreamSocket->createReceiveCallback(
                NTCCFG_BIND(&processReceive,
                            serverStreamSocket,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            bsl::string("C"),
                            ntsa::Error(ntsa::Error::e_OK),
                            &receiveSemaphore),
                allocator);

        error = serverStreamSocket->receive(receiveOptions, receiveCallback);
        NTCCFG_TEST_OK(error);
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

    reactor->stop();
}

}  // close namespace concern16
}  // close namespace test

NTCCFG_TEST_CASE(16)
{
    // Concern: Send cancellation.

    test::Parameters parameters;

    test::Framework::execute(NTCCFG_BIND(&test::concern16::execute,
                                         NTCCFG_BIND_PLACEHOLDER_1,
                                         NTCCFG_BIND_PLACEHOLDER_2,
                                         parameters,
                                         NTCCFG_BIND_PLACEHOLDER_3));
}

namespace test {
namespace concern17 {

void processSend(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                 const bsl::shared_ptr<ntci::Sender>&       sender,
                 const ntca::SendEvent&                     event,
                 bslmt::Semaphore*                          semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing send event type %s: %s",
                   ntca::SendEventType::toString(event.type()),
                   event.context().error().text().c_str());

    semaphore->post();
}

void processReceive(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                    const bsl::shared_ptr<ntci::Receiver>&     receiver,
                    const bsl::shared_ptr<bdlbb::Blob>&        data,
                    const ntca::ReceiveEvent&                  event,
                    bslmt::Semaphore*                          semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing receive event type %s: %s",
                   ntca::ReceiveEventType::toString(event.type()),
                   event.context().error().text().c_str());

    semaphore->post();
}

void execute(ntsa::Transport::Value                transport,
             const bsl::shared_ptr<ntci::Reactor>& reactor,
             const test::Parameters&               parameters,
             bslma::Allocator*                     allocator)
{
// Disable this test until we can determine how to correctly scope
// the couting allocator supplied to the socket: the allocator must
// outlive the strand internally created by the socket, and the scope of
// the strand may be extended past the lifetime of this function when
// the 'reactor' is dynamically load balanced.
#if 0
    // Concern: Socket memory allocation.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket memory allocation test starting");

    const int k_MESSAGE_SIZE = 1024 * 1024 * 32;

    ntsa::Error                     error;
    bslmt::Semaphore                sendSemaphore;
    bslmt::Semaphore                receiveSemaphore;
    bsl::shared_ptr<ntcs::Metrics>  metrics;
    bsl::shared_ptr<ntci::Resolver> resolver;

    bdlma::CountingAllocator clientStreamSocketAllocator(allocator);
    bdlma::CountingAllocator serverStreamSocketAllocator(allocator);

    bsl::shared_ptr<ntcr::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntcr::StreamSocket> serverStreamSocket;
    {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntcd::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntcd::StreamSocket> basicServerSocket;

        error = ntcd::Simulation::createStreamSocketPair(
            &basicClientSocket, &basicServerSocket, transport);
        NTCCFG_TEST_FALSE(error);

        clientStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         reactor,
                                         reactor,
                                         metrics,
                                         &clientStreamSocketAllocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverStreamSocket.createInplace(allocator,
                                         options,
                                         resolver,
                                         reactor,
                                         reactor,
                                         metrics,
                                         &serverStreamSocketAllocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
    }

    NTCI_LOG_DEBUG("Client stream socket number of bytes in use = %zu",
                   (bsl::size_t)(clientStreamSocketAllocator.numBytesInUse()));

    NTCI_LOG_DEBUG("Server stream socket number of bytes in use = %zu",
                   (bsl::size_t)(serverStreamSocketAllocator.numBytesInUse()));

    NTCI_LOG_DEBUG("Generating message");

    bsl::shared_ptr<bdlbb::Blob> data =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(data.get(), k_MESSAGE_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Sending message");

    {
        ntca::SendOptions sendOptions;

        ntci::SendCallback sendCallback =
            clientStreamSocket->createSendCallback(
                NTCCFG_BIND(&processSend,
                            clientStreamSocket,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            &sendSemaphore),
                allocator);

        error = clientStreamSocket->send(*data, sendOptions, sendCallback);
        NTCCFG_TEST_TRUE(!error);
    }

    NTCI_LOG_INFO("Receiving message");

    {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setSize(k_MESSAGE_SIZE);

        ntci::ReceiveCallback receiveCallback =
            serverStreamSocket->createReceiveCallback(
                NTCCFG_BIND(&processReceive,
                            serverStreamSocket,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            &receiveSemaphore),
                allocator);

        error = serverStreamSocket->receive(receiveOptions, receiveCallback);
        NTCCFG_TEST_OK(error);
    }

    sendSemaphore.wait();

    NTCI_LOG_DEBUG("Message has been sent");

    receiveSemaphore.wait();

    NTCI_LOG_DEBUG("Message has been received");

    NTCI_LOG_DEBUG("Client stream socket number of bytes in use = %zu",
                   (bsl::size_t)(clientStreamSocketAllocator.numBytesInUse()));

    NTCI_LOG_DEBUG("Server stream socket number of bytes in use = %zu",
                   (bsl::size_t)(serverStreamSocketAllocator.numBytesInUse()));

    NTCI_LOG_DEBUG("Closing sockets");

    {
        ntci::StreamSocketCloseGuard clientStreamSocketCloseGuard(
                                                           clientStreamSocket);

        ntci::StreamSocketCloseGuard serverStreamSocketCloseGuard(
                                                           serverStreamSocket);
    }

    NTCI_LOG_DEBUG("Sockets have been closed");

    NTCI_LOG_DEBUG("Client stream socket number of bytes in use = %zu",
                   (bsl::size_t)(clientStreamSocketAllocator.numBytesInUse()));

    NTCI_LOG_DEBUG("Server stream socket number of bytes in use = %zu",
                   (bsl::size_t)(serverStreamSocketAllocator.numBytesInUse()));

    NTCI_LOG_DEBUG("Destroying sockets");

    clientStreamSocket.reset();
    serverStreamSocket.reset();

    NTCI_LOG_DEBUG("Sockets have been destroyed");

    NTCI_LOG_DEBUG("Client stream socket number of bytes in use = %zu",
                   (bsl::size_t)(clientStreamSocketAllocator.numBytesInUse()));

    NTCI_LOG_DEBUG("Server stream socket number of bytes in use = %zu",
                   (bsl::size_t)(serverStreamSocketAllocator.numBytesInUse()));

    NTCI_LOG_DEBUG("Stream socket memory allocation test complete");
#endif

    reactor->stop();
}

}  // close namespace concern17
}  // close namespace test

NTCCFG_TEST_CASE(17)
{
    // Concern: Memory allocation.

    test::Parameters parameters;

    test::Framework::execute(NTCCFG_BIND(&test::concern17::execute,
                                         NTCCFG_BIND_PLACEHOLDER_1,
                                         NTCCFG_BIND_PLACEHOLDER_2,
                                         parameters,
                                         NTCCFG_BIND_PLACEHOLDER_3));
}

namespace test {
namespace case18 {

/// This struct describes the context of the data sent and received.
class StreamSocketContext
{
  public:
    bsl::shared_ptr<bdlbb::Blob>               d_message_sp;
    bsl::vector<bsl::shared_ptr<bdlbb::Blob> > d_fragment;

  private:
    StreamSocketContext(const StreamSocketContext&) BSLS_KEYWORD_DELETED;
    StreamSocketContext& operator=(const StreamSocketContext&)
        BSLS_KEYWORD_DELETED;

  public:
    explicit StreamSocketContext(bslma::Allocator* basicAllocator = 0)
    : d_message_sp()
    , d_fragment(basicAllocator)
    {
    }

    ~StreamSocketContext()
    {
    }
};

/// Provide an implementation of the 'ntci::StreamSocketSession'
/// interface to test concerns related to the read queue low watermark.
/// This class is thread safe.
class StreamSocketSession : public ntci::StreamSocketSession
{
    bsl::shared_ptr<ntci::StreamSocket>  d_streamSocket_sp;
    bsl::shared_ptr<StreamSocketContext> d_context_sp;
    bsl::size_t                          d_index;
    bslma::Allocator*                    d_allocator_p;

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

  public:
    /// Create a new stream socket session for the specified 'streamSocket'.
    explicit StreamSocketSession(
        const bsl::shared_ptr<ntci::StreamSocket>&  streamSocket,
        const bsl::shared_ptr<StreamSocketContext>& context,
        bslma::Allocator*                           basicAllocator = 0);

    /// Destroy this object.
    ~StreamSocketSession() BSLS_KEYWORD_OVERRIDE;

    /// Return true if all fragments have been received.
    bool done() const;
};

void StreamSocketSession::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    ntsa::Error error;

    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Stream socket at "
                          << streamSocket->sourceEndpoint() << " to "
                          << streamSocket->remoteEndpoint()
                          << " processing read queue low watermark event "
                          << event << NTCI_LOG_STREAM_END;

    if (d_index < d_context_sp->d_fragment.size()) {
        ntca::ReceiveContext receiveContext;
        ntca::ReceiveOptions receiveOptions;

        receiveOptions.setSize(d_context_sp->d_fragment[d_index]->length());

        NTCI_LOG_STREAM_DEBUG
            << "Stream socket at " << streamSocket->sourceEndpoint() << " to "
            << streamSocket->remoteEndpoint() << " attempting to receive "
            << d_context_sp->d_fragment[d_index]->length()
            << " bytes for fragment index " << d_index << NTCI_LOG_STREAM_END;

        bdlbb::Blob fragment;
        error =
            streamSocket->receive(&receiveContext, &fragment, receiveOptions);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_EQ(
            bdlbb::BlobUtil::compare(fragment,
                                     *d_context_sp->d_fragment[d_index]),
            0);

        ++d_index;

        if (d_index < d_context_sp->d_fragment.size()) {
            d_streamSocket_sp->setReadQueueLowWatermark(
                d_context_sp->d_fragment[d_index]->length());
        }
    }
    else {
        ntca::ReceiveContext receiveContext;
        ntca::ReceiveOptions receiveOptions;

        bdlbb::Blob fragment;
        error =
            streamSocket->receive(&receiveContext, &fragment, receiveOptions);
        NTCCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_EOF));

        NTCI_LOG_STREAM_DEBUG
            << "Stream socket at " << streamSocket->sourceEndpoint() << " to "
            << streamSocket->remoteEndpoint() << " received EOF"
            << NTCI_LOG_STREAM_END;
    }
}

StreamSocketSession::StreamSocketSession(
    const bsl::shared_ptr<ntci::StreamSocket>&  streamSocket,
    const bsl::shared_ptr<StreamSocketContext>& context,
    bslma::Allocator*                           basicAllocator)
: d_streamSocket_sp(streamSocket)
, d_context_sp(context)
, d_index(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

StreamSocketSession::~StreamSocketSession()
{
}

bool StreamSocketSession::done() const
{
    return d_index == d_context_sp->d_fragment.size();
}

}  // close namespace case18
}  // close namespace test

NTCCFG_TEST_CASE(18)
{
    ntccfg::TestAllocator ta;
    {
        NTCI_LOG_CONTEXT();
        NTCI_LOG_CONTEXT_GUARD_OWNER("main");

        ntsa::Error error;

        // Create and start the simulation.

        bsl::shared_ptr<ntcd::Simulation> simulation;
        simulation.createInplace(&ta, &ta);

        error = simulation->run();
        NTCCFG_TEST_OK(error);

        {
            // Create a reactor.

            const bsl::size_t BLOB_BUFFER_SIZE = 4096;

            bsl::shared_ptr<ntcs::DataPool> dataPool;
            dataPool.createInplace(&ta,
                                   BLOB_BUFFER_SIZE,
                                   BLOB_BUFFER_SIZE,
                                   &ta);

            bsl::shared_ptr<ntcs::User> user;
            user.createInplace(&ta, &ta);

            user->setDataPool(dataPool);

            ntca::ReactorConfig reactorConfig;
            reactorConfig.setMetricName("test");
            reactorConfig.setMinThreads(1);
            reactorConfig.setMaxThreads(1);
            reactorConfig.setAutoAttach(false);
            reactorConfig.setAutoDetach(false);
            reactorConfig.setOneShot(false);

            bsl::shared_ptr<ntcd::Reactor> reactor;
            reactor.createInplace(&ta, reactorConfig, user, &ta);

            // Register this thread as the thread that will wait on the reactor.

            ntci::Waiter waiter =
                reactor->registerWaiter(ntca::WaiterOptions());

            {
                // Create the server stream socket context.

                bsl::shared_ptr<test::case18::StreamSocketContext>
                    serverStreamSocketContext;
                serverStreamSocketContext.createInplace(&ta, &ta);

                serverStreamSocketContext->d_fragment.resize(4);

                serverStreamSocketContext->d_fragment[0] =
                    dataPool->createOutgoingBlob();
                ntcd::DataUtil::generateData(
                    serverStreamSocketContext->d_fragment[0].get(),
                    10);

                serverStreamSocketContext->d_fragment[1] =
                    dataPool->createOutgoingBlob();
                ntcd::DataUtil::generateData(
                    serverStreamSocketContext->d_fragment[1].get(),
                    200);

                serverStreamSocketContext->d_fragment[2] =
                    dataPool->createOutgoingBlob();
                ntcd::DataUtil::generateData(
                    serverStreamSocketContext->d_fragment[2].get(),
                    10);

                serverStreamSocketContext->d_fragment[3] =
                    dataPool->createOutgoingBlob();
                ntcd::DataUtil::generateData(
                    serverStreamSocketContext->d_fragment[3].get(),
                    400);

                // Create the client stream socket context.

                bsl::shared_ptr<test::case18::StreamSocketContext>
                    clientStreamSocketContext;

                clientStreamSocketContext.createInplace(&ta, &ta);

                clientStreamSocketContext->d_message_sp =
                    dataPool->createOutgoingBlob();
                for (bsl::size_t i = 0;
                     i < serverStreamSocketContext->d_fragment.size();
                     ++i)
                {
                    bdlbb::BlobUtil::append(
                        clientStreamSocketContext->d_message_sp.get(),
                        *serverStreamSocketContext->d_fragment[i]);
                }

                // Create a basic stream socket pair using the simulation.

                bsl::shared_ptr<ntci::Resolver> resolver;
                bsl::shared_ptr<ntcs::Metrics>  metrics;

                bsl::shared_ptr<ntcd::StreamSocket> basicClientSocket;
                bsl::shared_ptr<ntcd::StreamSocket> basicServerSocket;

                error = ntcd::Simulation::createStreamSocketPair(
                    &basicClientSocket,
                    &basicServerSocket,
                    ntsa::Transport::e_TCP_IPV4_STREAM);
                NTCCFG_TEST_FALSE(error);

                // Create a stream socket for the client.

                ntca::StreamSocketOptions clientStreamSocketOptions;
                clientStreamSocketOptions.setTransport(
                    ntsa::Transport::e_TCP_IPV4_STREAM);

                bsl::shared_ptr<ntcr::StreamSocket> clientStreamSocket;
                clientStreamSocket.createInplace(&ta,
                                                 clientStreamSocketOptions,
                                                 resolver,
                                                 reactor,
                                                 reactor,
                                                 metrics,
                                                 &ta);

                bsl::shared_ptr<test::case18::StreamSocketSession>
                    clientStreamSocketSession;

                clientStreamSocketSession.createInplace(
                    &ta,
                    clientStreamSocket,
                    clientStreamSocketContext,
                    &ta);

                error = clientStreamSocket->registerSession(
                    clientStreamSocketSession);
                NTCCFG_TEST_FALSE(error);

                error = clientStreamSocket->open(
                    ntsa::Transport::e_TCP_IPV4_STREAM,
                    basicClientSocket);
                NTCCFG_TEST_FALSE(error);

                // Create a stream socket for the server.

                ntca::StreamSocketOptions serverStreamSocketOptions;
                serverStreamSocketOptions.setTransport(
                    ntsa::Transport::e_TCP_IPV4_STREAM);

                bsl::shared_ptr<ntcr::StreamSocket> serverStreamSocket;
                serverStreamSocket.createInplace(&ta,
                                                 serverStreamSocketOptions,
                                                 resolver,
                                                 reactor,
                                                 reactor,
                                                 metrics,
                                                 &ta);

                bsl::shared_ptr<test::case18::StreamSocketSession>
                    serverStreamSocketSession;

                serverStreamSocketSession.createInplace(
                    &ta,
                    serverStreamSocket,
                    serverStreamSocketContext,
                    &ta);

                error = serverStreamSocket->registerSession(
                    serverStreamSocketSession);
                NTCCFG_TEST_FALSE(error);

                error = serverStreamSocket->open(
                    ntsa::Transport::e_TCP_IPV4_STREAM,
                    basicServerSocket);
                NTCCFG_TEST_FALSE(error);

                // Set the initial read queue low watermark of the server.

                error = serverStreamSocket->setReadQueueLowWatermark(
                    serverStreamSocketContext->d_fragment[0]->length());
                NTCCFG_TEST_FALSE(error);

                // Send all fragments from the client to the server as a single
                // message.

                error = clientStreamSocket->send(
                    *clientStreamSocketContext->d_message_sp,
                    ntca::SendOptions());
                NTCCFG_TEST_FALSE(error);

                // Poll.

                while (!serverStreamSocketSession->done()) {
                    reactor->poll(waiter);
                }

                // Close the client and server.

                clientStreamSocket->close();
                serverStreamSocket->close();

                // Poll.

                reactor->poll(waiter);
            }

            // Deregister the waiter.

            reactor->deregisterWaiter(waiter);
        }

        // Stop the simulation.

        simulation->stop();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(19)
{
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

    ntccfg::TestAllocator ta;
    {
        NTCI_LOG_CONTEXT();
        NTCI_LOG_CONTEXT_GUARD_OWNER("main");

        const bsl::size_t k_BLOB_BUFFER_SIZE           = 4096;
        const bsl::size_t k_SEND_BUFFER_SIZE           = 32;
        const bsl::size_t k_WRITE_QUEUE_HIGH_WATERMARK = 64;

        ntsa::Error error;

        // Create and start the simulation.

        bsl::shared_ptr<ntcd::Simulation> simulation;
        simulation.createInplace(&ta, &ta);

        // Create a reactor.

        bsl::shared_ptr<ntcs::DataPool> dataPool;
        dataPool.createInplace(&ta,
                               k_BLOB_BUFFER_SIZE,
                               k_BLOB_BUFFER_SIZE,
                               &ta);

        bsl::shared_ptr<ntcs::User> user;
        user.createInplace(&ta, &ta);
        user->setDataPool(dataPool);

        ntca::ReactorConfig reactorConfig;
        reactorConfig.setMetricName("test");
        reactorConfig.setMinThreads(1);
        reactorConfig.setMaxThreads(1);
        reactorConfig.setAutoAttach(false);
        reactorConfig.setAutoDetach(false);
        reactorConfig.setOneShot(false);

        bsl::shared_ptr<ntcd::Reactor> reactor;
        reactor.createInplace(&ta, reactorConfig, user, &ta);

        // Register this thread as the thread that will wait on the reactor.

        ntci::Waiter waiter = reactor->registerWaiter(ntca::WaiterOptions());

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
        NTCCFG_TEST_FALSE(error);

        // Create a stream socket for the client with a specific send buffer
        // size and write queue high watermark.

        ntca::StreamSocketOptions clientStreamSocketOptions;
        clientStreamSocketOptions.setTransport(
            ntsa::Transport::e_TCP_IPV4_STREAM);
        clientStreamSocketOptions.setSendBufferSize(k_SEND_BUFFER_SIZE);
        clientStreamSocketOptions.setWriteQueueHighWatermark(
            k_WRITE_QUEUE_HIGH_WATERMARK);

        bsl::shared_ptr<ntcr::StreamSocket> clientStreamSocket;
        clientStreamSocket.createInplace(&ta,
                                         clientStreamSocketOptions,
                                         resolver,
                                         reactor,
                                         reactor,
                                         metrics,
                                         &ta);

        // Register a session to process the events that passively
        // occur during the operation of the client stream socket.

        bsl::shared_ptr<ntcu::StreamSocketEventQueue>
            clientStreamSocketEventQueue;

        clientStreamSocketEventQueue.createInplace(&ta, &ta);
        clientStreamSocketEventQueue->show(
            ntca::WriteQueueEventType::e_HIGH_WATERMARK);

        error =
            clientStreamSocket->registerSession(clientStreamSocketEventQueue);
        NTCCFG_TEST_FALSE(error);

        error = clientStreamSocket->open(ntsa::Transport::e_TCP_IPV4_STREAM,
                                         basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        // Create a stream socket for the server.

        ntca::StreamSocketOptions serverStreamSocketOptions;
        serverStreamSocketOptions.setTransport(
            ntsa::Transport::e_TCP_IPV4_STREAM);

        bsl::shared_ptr<ntcr::StreamSocket> serverStreamSocket;
        serverStreamSocket.createInplace(&ta,
                                         serverStreamSocketOptions,
                                         resolver,
                                         reactor,
                                         reactor,
                                         metrics,
                                         &ta);

        // Register a session to process the events that passively
        // occur during the operation of the client stream socket.

        bsl::shared_ptr<ntcu::StreamSocketEventQueue>
            serverStreamSocketEventQueue;

        serverStreamSocketEventQueue.createInplace(&ta, &ta);

        error =
            serverStreamSocket->registerSession(serverStreamSocketEventQueue);
        NTCCFG_TEST_FALSE(error);

        // Open the the server stream socket.

        error = serverStreamSocket->open(ntsa::Transport::e_TCP_IPV4_STREAM,
                                         basicServerSocket);
        NTCCFG_TEST_FALSE(error);

        // Send data from the client stream socket to the server
        // stream socket to fill up the client stream socket send
        // buffer.

        {
            bsl::shared_ptr<bdlbb::Blob> blob =
                clientStreamSocket->createOutgoingBlob();

            ntcd::DataUtil::generateData(blob.get(), k_SEND_BUFFER_SIZE);

            error = clientStreamSocket->send(*blob, ntca::SendOptions());
            NTCCFG_TEST_FALSE(error);
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
            NTCCFG_TEST_FALSE(error);
        }

        // Now the client stream socket send buffer should be entirely
        // full and the write queue should be half full.

        NTCCFG_TEST_EQ(clientStreamSocket->writeQueueSize(),
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
            NTCCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_WOULD_BLOCK));
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
                reactor->poll(waiter);
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
        reactor->poll(waiter);

        // Deregister the waiter.

        reactor->deregisterWaiter(waiter);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(20)
{
    // Concern: RX timestamping test.

    test::Parameters parameters;
    parameters.d_numTimers             = 0;
    parameters.d_numSocketPairs        = 1;
    parameters.d_numMessages           = 100;
    parameters.d_messageSize           = 32;
    parameters.d_useAsyncCallbacks     = false;
    parameters.d_timestampIncomingData = true;
    parameters.d_collectMetrics        = true;

    test::variation(parameters);
}

NTCCFG_TEST_CASE(21)
{
    // Concern: TX timestamping test.
#if 0
    // The test is disabled due to its flaky nature
    test::Parameters parameters;
    parameters.d_numTimers             = 0;
    parameters.d_numSocketPairs        = 1;
    parameters.d_numMessages           = 100;
    parameters.d_messageSize           = 32;
    parameters.d_useAsyncCallbacks     = false;
    parameters.d_timestampOutgoingData = true;
    parameters.d_collectMetrics        = true;

    test::variation(parameters);
#endif
}
#if 1
NTCCFG_TEST_CASE(22)
{
    NTCI_LOG_CONTEXT();

    ntccfg::TestAllocator ta;
    {
        NTCI_LOG_DEBUG("Fixture setup, socket creation...");

        const bsl::nullopt_t doNotCare = bsl::nullopt;
        const ntsa::Handle   handle    = 22;

        bdlb::NullableValue<ntca::ConnectEvent> connectResult;

        bsl::shared_ptr<ntci::ReactorPool> nullPool;
        bsl::shared_ptr<ntcs::Metrics>     nullMetrics;
        bsl::shared_ptr<bdlbb::Blob>       nullBlob;
        bsl::shared_ptr<ntci::Strand>      nullStrand;

        bsl::shared_ptr<ntsa::Data> dummyData;
        dummyData.createInplace(&ta);

        bsl::shared_ptr<test::mock::ResolverMock> resolverMock;
        resolverMock.createInplace(&ta);

        bsl::shared_ptr<test::mock::ReactorMock> reactorMock;
        reactorMock.createInplace(&ta);

        bsl::shared_ptr<test::mock::StreamSocketMock> socketMock;
        socketMock.createInplace(&ta);

        bsl::shared_ptr<test::mock::DataPoolMock> dataPoolMock;
        dataPoolMock.createInplace(&ta);
        reactorMock->expect_dataPool_WillAlwaysReturn(dataPoolMock);

        bsl::shared_ptr<test::mock::BufferFactoryMock> bufferFactoryMock;
        bufferFactoryMock.createInplace(&ta);
        reactorMock->expect_outgoingBlobBufferFactory_WillAlwaysReturn(
            bufferFactoryMock);
        reactorMock->expect_incomingBlobBufferFactory_WillAlwaysReturn(
            bufferFactoryMock);

        NTF_EXPECT_0(*reactorMock, oneShot).ALWAYS().RETURN(false);
        NTF_EXPECT_0(*reactorMock, maxThreads).ALWAYS().RETURN(1);

        NTF_EXPECT_0(*dataPoolMock, createIncomingBlob)
            .ALWAYS()
            .RETURN(nullBlob);
        NTF_EXPECT_0(*dataPoolMock, createOutgoingBlob)
            .ALWAYS()
            .RETURN(nullBlob);
        NTF_EXPECT_0(*dataPoolMock, createOutgoingData)
            .ALWAYS()
            .RETURN(dummyData);

        const ntca::StreamSocketOptions options;

        bsl::shared_ptr<ntcr::StreamSocket> socket;
        socket.createInplace(&ta,
                             options,
                             resolverMock,
                             reactorMock,
                             nullPool,
                             nullMetrics,
                             &ta);

        NTCI_LOG_DEBUG("Inject mocked ntsi::StreamSocket");

        socketMock->expect_handle().willAlways().willReturn(handle);

        NTF_EXPECT_1(*socketMock, setBlocking, NTF_EQ(false))
            .TIMES(2)
            .RETURN(ntsa::Error());  //TODO: for some reason it is called twice

        NTF_EXPECT_1(*socketMock, setOption, IGNORE_ARG)
            .ALWAYS()
            .RETURN(ntsa::Error());

        NTF_EXPECT_1(*socketMock, sourceEndpoint, IGNORE_ARG)
            .ONCE()
            .RETURN(ntsa::Error::invalid());

        NTF_EXPECT_1(*socketMock, remoteEndpoint, IGNORE_ARG)
            .ONCE()
            .RETURN(ntsa::Error::invalid());

        ntsa::SocketOption sendBufferSizeOption;
        sendBufferSizeOption.makeSendBufferSize(100500);
        ntsa::SocketOption rcvBufferSizeOption;
        rcvBufferSizeOption.makeReceiveBufferSize(100500);

        NTF_EXPECT_2(*socketMock,
                     getOption,
                     IGNORE_ARG,
                     NTF_EQ(ntsa::SocketOptionType::e_SEND_BUFFER_SIZE))
            .ONCE()
            .RETURN(ntsa::Error())
            .SET_ARG_1(FROM_DEREF(sendBufferSizeOption));

        NTF_EXPECT_2(*socketMock,
                     getOption,
                     IGNORE_ARG,
                     NTF_EQ(ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE))
            .ONCE()
            .RETURN(ntsa::Error())
            .SET_ARG_1(FROM_DEREF(rcvBufferSizeOption));

        socketMock->expect_maxBuffersPerSend().willOnce().willReturn(22);
        socketMock->expect_maxBuffersPerReceive().willOnce().willReturn(22);

        NTF_EXPECT_0(*reactorMock, acquireHandleReservation)
            .ALWAYS()
            .RETURN(true);
        NTF_EXPECT_0(*reactorMock, releaseHandleReservation).ALWAYS();

        socket->open(ntsa::Transport::e_TCP_IPV4_STREAM, socketMock);

        NTCI_LOG_DEBUG("Connection initiation...");

        bsl::shared_ptr<test::mock::TimerMock> connectRetryTimerMock;
        connectRetryTimerMock.createInplace(&ta);

        ntci::TimerCallback retryTimerCallback;
        reactorMock->expect_createTimer(doNotCare, doNotCare, doNotCare)
            .willOnce()
            .willReturn(connectRetryTimerMock)
            .saveArg2(retryTimerCallback);

        NTF_EXPECT_2(*connectRetryTimerMock, schedule, IGNORE_ARG, IGNORE_ARG)
            .ONCE()
            .RETURN(ntsa::Error());

        const ntci::ConnectFunction connectCallback =
            [&connectResult](const bsl::shared_ptr<ntci::Connector>& connector,
                             const ntca::ConnectEvent&               event) {
                NTCCFG_TEST_FALSE(connectResult.has_value());
                connectResult = event;
            };

        const ntca::ConnectOptions connectOptions;

        const bsl::string epName = "unreachable.bbg.com";

        socket->connect(epName, connectOptions, connectCallback);

        NTCI_LOG_DEBUG("Trigger internal timer to initiate connection...");

        NTF_EXPECT_3(*resolverMock,
                     getEndpoint,
                     NTF_EQ(epName),
                     IGNORE_ARG,
                     IGNORE_ARG)
            .ONCE()
            .RETURN(ntsa::Error());

        ntca::TimerEvent timerEvent;
        timerEvent.setType(ntca::TimerEventType::e_DEADLINE);
        retryTimerCallback(connectRetryTimerMock, timerEvent, nullStrand);

        NTCI_LOG_DEBUG("Shutdown socket while it is waiting for remote "
                       "endpoint resolution");

        connectRetryTimerMock->expect_close().willOnce().willReturn(
            ntsa::Error());

        ntci::Reactor::Functor callback;
        NTF_EXPECT_1(*reactorMock, execute, IGNORE_ARG)
            .ONCE()
            .SAVE_ARG_1(TO(&callback));

        NTF_EXPECT_2(
            *reactorMock,
            detachSocket,
            NTF_EQ_SPEC(socket, const bsl::shared_ptr<ntci::ReactorSocket>&),
            IGNORE_ARG)
            .ONCE()
            .RETURN(ntsa::Error::invalid());
        //TODO: is that ok to detach socket that has not been attached?

        socketMock->expect_close().willOnce().willReturn(ntsa::Error());

        socket->shutdown(ntsa::ShutdownType::e_BOTH,
                         ntsa::ShutdownMode::e_GRACEFUL);

        callback();
        NTCCFG_TEST_TRUE(connectResult.has_value());
        NTCCFG_TEST_EQ(connectResult.value().type(),
                       ntca::ConnectEventType::e_ERROR);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(23)
{
    NTCI_LOG_CONTEXT();

    ntccfg::TestAllocator ta;
    {
        NTCI_LOG_DEBUG("Fixture setup, socket creation...");

        const bsl::nullopt_t doNotCare = bsl::nullopt;
        const ntsa::Handle   handle    = 22;

        bdlb::NullableValue<ntca::ConnectEvent> connectResult;

        bsl::shared_ptr<ntci::ReactorPool> nullPool;
        bsl::shared_ptr<ntcs::Metrics>     nullMetrics;
        bsl::shared_ptr<bdlbb::Blob>       nullBlob;
        bsl::shared_ptr<ntci::Strand>      nullStrand;

        bsl::shared_ptr<ntsa::Data> dummyData;
        dummyData.createInplace(&ta);

        bsl::shared_ptr<test::mock::ResolverMock> resolverMock;
        resolverMock.createInplace(&ta);

        bsl::shared_ptr<test::mock::ReactorMock> reactorMock;
        reactorMock.createInplace(&ta);

        bsl::shared_ptr<test::mock::StreamSocketMock> socketMock;
        socketMock.createInplace(&ta);

        bsl::shared_ptr<test::mock::DataPoolMock> dataPoolMock;
        dataPoolMock.createInplace(&ta);
        reactorMock->expect_dataPool_WillAlwaysReturn(dataPoolMock);

        bsl::shared_ptr<test::mock::BufferFactoryMock> bufferFactoryMock;
        bufferFactoryMock.createInplace(&ta);
        reactorMock->expect_outgoingBlobBufferFactory_WillAlwaysReturn(
            bufferFactoryMock);
        reactorMock->expect_incomingBlobBufferFactory_WillAlwaysReturn(
            bufferFactoryMock);

        NTF_EXPECT_0(*reactorMock, oneShot).ALWAYS().RETURN(false);
        NTF_EXPECT_0(*reactorMock, maxThreads).ALWAYS().RETURN(1);

        NTF_EXPECT_0(*dataPoolMock, createIncomingBlob)
            .ALWAYS()
            .RETURN(nullBlob);
        NTF_EXPECT_0(*dataPoolMock, createOutgoingBlob)
            .ALWAYS()
            .RETURN(nullBlob);
        NTF_EXPECT_0(*dataPoolMock, createOutgoingData)
            .ALWAYS()
            .RETURN(dummyData);

        const ntca::StreamSocketOptions options;

        bsl::shared_ptr<ntcr::StreamSocket> socket;
        socket.createInplace(&ta,
                             options,
                             resolverMock,
                             reactorMock,
                             nullPool,
                             nullMetrics,
                             &ta);

        NTCI_LOG_DEBUG("Inject mocked ntsi::StreamSocket");

        socketMock->expect_handle().willAlways().willReturn(handle);

        NTF_EXPECT_1(*socketMock, setBlocking, NTF_EQ(false))
            .TIMES(2)
            .RETURN(ntsa::Error());  //TODO: for some reason it is called twice

        NTF_EXPECT_1(*socketMock, setOption, IGNORE_ARG)
            .ALWAYS()
            .RETURN(ntsa::Error());

        NTF_EXPECT_1(*socketMock, sourceEndpoint, IGNORE_ARG)
            .ONCE()
            .RETURN(ntsa::Error::invalid());

        NTF_EXPECT_1(*socketMock, remoteEndpoint, IGNORE_ARG)
            .ONCE()
            .RETURN(ntsa::Error::invalid());

        ntsa::SocketOption sendBufferSizeOption;
        sendBufferSizeOption.makeSendBufferSize(100500);
        ntsa::SocketOption rcvBufferSizeOption;
        rcvBufferSizeOption.makeReceiveBufferSize(100500);

        NTF_EXPECT_2(*socketMock,
                     getOption,
                     IGNORE_ARG,
                     NTF_EQ(ntsa::SocketOptionType::e_SEND_BUFFER_SIZE))
            .ONCE()
            .RETURN(ntsa::Error())
            .SET_ARG_1(FROM_DEREF(sendBufferSizeOption));

        NTF_EXPECT_2(*socketMock,
                     getOption,
                     IGNORE_ARG,
                     NTF_EQ(ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE))
            .ONCE()
            .RETURN(ntsa::Error())
            .SET_ARG_1(FROM_DEREF(rcvBufferSizeOption));

        socketMock->expect_maxBuffersPerSend().willOnce().willReturn(22);
        socketMock->expect_maxBuffersPerReceive().willOnce().willReturn(22);

        NTF_EXPECT_0(*reactorMock, acquireHandleReservation)
            .ALWAYS()
            .RETURN(true);
        NTF_EXPECT_0(*reactorMock, releaseHandleReservation).ALWAYS();

        socket->open(ntsa::Transport::e_TCP_IPV4_STREAM, socketMock);

        NTCI_LOG_DEBUG("Connection initiation...");

        bsl::shared_ptr<test::mock::TimerMock> connectRetryTimerMock;
        connectRetryTimerMock.createInplace(&ta);

        ntci::TimerCallback retryTimerCallback;
        reactorMock->expect_createTimer(doNotCare, doNotCare, doNotCare)
            .willOnce()
            .willReturn(connectRetryTimerMock)
            .saveArg2(retryTimerCallback);

        NTF_EXPECT_2(*connectRetryTimerMock, schedule, IGNORE_ARG, IGNORE_ARG)
            .ONCE()
            .RETURN(ntsa::Error());

        const ntci::ConnectFunction connectCallback =
            [&connectResult](const bsl::shared_ptr<ntci::Connector>& connector,
                             const ntca::ConnectEvent&               event) {
                NTCCFG_TEST_FALSE(connectResult.has_value());
                connectResult = event;
            };

        const ntca::ConnectOptions connectOptions;

        const ntsa::Endpoint targetEp{"127.0.0.1:1234"};
        const ntsa::Endpoint sourceEp{"127.0.0.1:22"};

        socket->connect(targetEp, connectOptions, connectCallback);

        NTCI_LOG_DEBUG("Trigger internal timer to initiate connection...");

        NTF_EXPECT_1(
            *reactorMock,
            attachSocket,
            NTF_EQ_SPEC(socket, const bsl::shared_ptr<ntci::ReactorSocket>&))
            .ONCE()
            .RETURN(ntsa::Error());

        NTF_EXPECT_2(*reactorMock, showWritable, NTF_EQ(socket), IGNORE_ARG)
            .ONCE()
            .RETURN(ntsa::Error());

        NTF_EXPECT_1(*socketMock, connect, NTF_EQ(targetEp))
            .ONCE()
            .RETURN(ntsa::Error());

        NTF_EXPECT_1(*socketMock, sourceEndpoint, IGNORE_ARG)
            .ONCE()
            .RETURN(ntsa::Error())
            .SET_ARG_1(FROM_DEREF(sourceEp));

        ntca::TimerEvent timerEvent;
        timerEvent.setType(ntca::TimerEventType::e_DEADLINE);
        retryTimerCallback(connectRetryTimerMock, timerEvent, nullStrand);

        NTCI_LOG_DEBUG(
            "Shutdown socket while it is waiting for connection result");

        connectRetryTimerMock->expect_close().willOnce().willReturn(
            ntsa::Error());

        ntci::SocketDetachedCallback detachCallback;

        NTF_EXPECT_2(
            *reactorMock,
            detachSocket,
            NTF_EQ_SPEC(socket, const bsl::shared_ptr<ntci::ReactorSocket>&),
            IGNORE_ARG)
            .ONCE()
            .SAVE_ARG_2(TO(&detachCallback))
            .RETURN(ntsa::Error());

        socketMock->expect_close().willOnce().willReturn(ntsa::Error());

        socket->shutdown(ntsa::ShutdownType::e_BOTH,
                         ntsa::ShutdownMode::e_GRACEFUL);

        NTCCFG_TEST_TRUE(detachCallback);

        ntci::Reactor::Functor callback;
        NTF_EXPECT_1(*reactorMock, execute, IGNORE_ARG)
            .ONCE()
            .SAVE_ARG_1(TO(&callback));
        detachCallback(nullStrand);

        callback();

        NTCCFG_TEST_TRUE(connectResult.has_value());
        NTCCFG_TEST_EQ(connectResult.value().type(),
                       ntca::ConnectEventType::e_ERROR);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(24)
{
    NTCI_LOG_CONTEXT();

    ntccfg::TestAllocator ta;
    {
        NTCI_LOG_DEBUG("Fixture setup, socket creation...");

        const bsl::nullopt_t doNotCare = bsl::nullopt;

        bdlb::NullableValue<ntca::ConnectEvent> connectResult;

        bsl::shared_ptr<ntci::ReactorPool> nullPool;
        bsl::shared_ptr<ntcs::Metrics>     nullMetrics;
        bsl::shared_ptr<bdlbb::Blob>       nullBlob;
        bsl::shared_ptr<ntci::Strand>      nullStrand;

        bsl::shared_ptr<ntsa::Data> dummyData;
        dummyData.createInplace(&ta);

        bsl::shared_ptr<test::mock::ResolverMock> resolverMock;
        resolverMock.createInplace(&ta);

        bsl::shared_ptr<test::mock::ReactorMock> reactorMock;
        reactorMock.createInplace(&ta);

        bsl::shared_ptr<test::mock::StreamSocketMock> socketMock;
        socketMock.createInplace(&ta);

        bsl::shared_ptr<test::mock::DataPoolMock> dataPoolMock;
        dataPoolMock.createInplace(&ta);
        reactorMock->expect_dataPool_WillAlwaysReturn(dataPoolMock);

        bsl::shared_ptr<test::mock::BufferFactoryMock> bufferFactoryMock;
        bufferFactoryMock.createInplace(&ta);
        reactorMock->expect_outgoingBlobBufferFactory_WillAlwaysReturn(
            bufferFactoryMock);
        reactorMock->expect_incomingBlobBufferFactory_WillAlwaysReturn(
            bufferFactoryMock);

        NTF_EXPECT_0(*reactorMock, oneShot).ALWAYS().RETURN(false);
        NTF_EXPECT_0(*reactorMock, maxThreads).ALWAYS().RETURN(1);

        NTF_EXPECT_0(*dataPoolMock, createIncomingBlob)
            .ALWAYS()
            .RETURN(nullBlob);
        NTF_EXPECT_0(*dataPoolMock, createOutgoingBlob)
            .ALWAYS()
            .RETURN(nullBlob);
        NTF_EXPECT_0(*dataPoolMock, createOutgoingData)
            .ALWAYS()
            .RETURN(dummyData);

        const ntca::StreamSocketOptions options;

        bsl::shared_ptr<ntcr::StreamSocket> socket;
        socket.createInplace(&ta,
                             options,
                             resolverMock,
                             reactorMock,
                             nullPool,
                             nullMetrics,
                             &ta);

        NTCI_LOG_DEBUG("Connection initiation...");

        bsl::shared_ptr<test::mock::TimerMock> connectDeadlineTimerMock;
        bsls::TimeInterval                     deadlineTime;
        ntci::TimerCallback                    deadlineTimerCallback;
        {
            deadlineTime.setTotalHours(1);

            connectDeadlineTimerMock.createInplace(&ta);
            reactorMock->expect_createTimer(doNotCare, doNotCare, doNotCare)
                .willOnce()
                .willReturn(connectDeadlineTimerMock)
                .saveArg2(deadlineTimerCallback);

            NTF_EXPECT_2(*connectDeadlineTimerMock,
                         schedule,
                         NTF_EQ(deadlineTime),
                         NTF_EQ(bsls::TimeInterval()))
                .ONCE()
                .RETURN(ntsa::Error());
        }

        bsl::shared_ptr<test::mock::TimerMock> connectRetryTimerMock;
        ntci::TimerCallback                    retryTimerCallback;
        {
            connectRetryTimerMock.createInplace(&ta);

            reactorMock->expect_createTimer(doNotCare, doNotCare, doNotCare)
                .willOnce()
                .willReturn(connectRetryTimerMock)
                .saveArg2(retryTimerCallback);

            NTF_EXPECT_2(*connectRetryTimerMock,
                         schedule,
                         IGNORE_ARG,
                         IGNORE_ARG)
                .ONCE()
                .RETURN(ntsa::Error());
        }

        const ntci::ConnectFunction connectCallback =
            [&connectResult](const bsl::shared_ptr<ntci::Connector>& connector,
                             const ntca::ConnectEvent&               event) {
                NTCCFG_TEST_FALSE(connectResult.has_value());
                connectResult = event;
            };

        ntca::ConnectOptions connectOptions;
        connectOptions.setDeadline(deadlineTime);

        const ntsa::Endpoint ep{"127.0.0.1:1234"};

        socket->connect(ep, connectOptions, connectCallback);

        NTCI_LOG_DEBUG("Trigger internal timer to initiate connection...");

        NTF_EXPECT_0(*reactorMock, acquireHandleReservation)
            .ALWAYS()
            .RETURN(true);
        NTF_EXPECT_0(*reactorMock, releaseHandleReservation).ALWAYS();

        NTF_EXPECT_1(
            *reactorMock,
            attachSocket,
            NTF_EQ_SPEC(socket, const bsl::shared_ptr<ntci::ReactorSocket>&))
            .ONCE()
            .RETURN(ntsa::Error());

        NTF_EXPECT_2(*reactorMock, showWritable, NTF_EQ(socket), IGNORE_ARG)
            .ONCE()
            .RETURN(ntsa::Error());

        ntca::TimerEvent timerEvent;
        timerEvent.setType(ntca::TimerEventType::e_DEADLINE);
        retryTimerCallback(connectRetryTimerMock, timerEvent, nullStrand);

        NTCI_LOG_DEBUG(
            "Shutdown socket while it is waiting for connection result");

        ntci::SocketDetachedCallback detachCallback;
        {
            connectRetryTimerMock->expect_close().willOnce().willReturn(
                ntsa::Error());
            connectDeadlineTimerMock->expect_close().willOnce().willReturn(
                ntsa::Error());

            NTF_EXPECT_2(
                *reactorMock,
                detachSocket,
                NTF_EQ_SPEC(socket,
                            const bsl::shared_ptr<ntci::ReactorSocket>&),
                IGNORE_ARG)
                .ONCE()
                .SAVE_ARG_2(TO(&detachCallback))
                .RETURN(ntsa::Error());
        }

        socket->shutdown(ntsa::ShutdownType::e_BOTH,
                         ntsa::ShutdownMode::e_GRACEFUL);

        NTCCFG_TEST_TRUE(detachCallback);

        ntci::Reactor::Functor callback;
        NTF_EXPECT_1(*reactorMock, execute, IGNORE_ARG)
            .ONCE()
            .SAVE_ARG_1(TO(&callback));

        detachCallback(nullStrand);

        callback();

        NTCCFG_TEST_TRUE(connectResult.has_value());
        NTCCFG_TEST_EQ(connectResult.value().type(),
                       ntca::ConnectEventType::e_ERROR);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}
#endif
NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
    NTCCFG_TEST_REGISTER(5);
    NTCCFG_TEST_REGISTER(6);
    NTCCFG_TEST_REGISTER(7);
    NTCCFG_TEST_REGISTER(8);
    NTCCFG_TEST_REGISTER(9);
    NTCCFG_TEST_REGISTER(10);
    NTCCFG_TEST_REGISTER(11);
    NTCCFG_TEST_REGISTER(12);

    NTCCFG_TEST_REGISTER(13);
    NTCCFG_TEST_REGISTER(14);

    NTCCFG_TEST_REGISTER(15);
    NTCCFG_TEST_REGISTER(16);

    NTCCFG_TEST_REGISTER(17);

    NTCCFG_TEST_REGISTER(18);

    NTCCFG_TEST_REGISTER(19);

    NTCCFG_TEST_REGISTER(20);
    NTCCFG_TEST_REGISTER(21);

    NTCCFG_TEST_REGISTER(22);
    NTCCFG_TEST_REGISTER(23);
    NTCCFG_TEST_REGISTER(24);
}
NTCCFG_TEST_DRIVER_END;
