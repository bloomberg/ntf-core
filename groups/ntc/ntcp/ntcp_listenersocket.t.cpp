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
BSLS_IDENT_RCSID(ntcp_listenersocket_t_cpp, "$Id$ $CSID$")

#include <ntcp_listenersocket.h>

#include <ntcd_datautil.h>
#include <ntcd_encryption.h>
#include <ntcd_proactor.h>
#include <ntcd_simulation.h>
#include <ntci_log.h>
#include <ntcp_streamsocket.h>
#include <ntcs_datapool.h>
#include <ntcs_ratelimiter.h>
#include <ntcs_user.h>
#include <ntsa_error.h>
#include <ntsa_transport.h>
#include <ntsi_streamsocket.h>

using namespace BloombergLP;

// Uncomment to test a particular style of socket-to-thread load balancing,
// instead of both static and dynamic load balancing.
// #define NTCP_LISTENER_SOCKET_TEST_DYNAMIC_LOAD_BALANCING false

namespace BloombergLP {
namespace ntcp {

// Provide tests for 'ntcp::ListenerSocket'.
class ListenerSocketTest
{
    /// Provide a test case execution framework.
    class Framework;

    /// Describe the parameters of a test.
    class Parameters;

    /// Provides a listener socket protocol for this test driver.
    class ListenerSocketApplication;

    /// This test provides a stream socket protocol for this test driver.
    class StreamSocketSession;

    /// Provide a stream socket manager for this test driver.
    class StreamSocketManager;

    // Execute the concern with the specified 'parameters' for the specified
    // 'transport' using the specified 'proactor'.
    static void verifyGenericVariation(
        ntsa::Transport::Value                 transport,
        const bsl::shared_ptr<ntci::Proactor>& proactor,
        const ListenerSocketTest::Parameters&  parameters,
        bslma::Allocator*                      allocator);

    // Execute the concern with the specified 'parameters' for the specified
    // 'transport' using the specified 'proactor'.
    static void verifyAcceptDeadlineVariation(
        ntsa::Transport::Value                 transport,
        const bsl::shared_ptr<ntci::Proactor>& proactor,
        const ListenerSocketTest::Parameters&  parameters,
        bslma::Allocator*                      allocator);

    // Execute the concern with the specified 'parameters' for the specified
    // 'transport' using the specified 'proactor'.
    static void verifyAcceptCancellationVariation(
        ntsa::Transport::Value                 transport,
        const bsl::shared_ptr<ntci::Proactor>& proactor,
        const ListenerSocketTest::Parameters&  parameters,
        bslma::Allocator*                      allocator);

    // Cancel the accept operation on the specified 'listenerSocket' identified
    // by the specified 'token'.
    static void cancelAccept(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptToken                      token);

    // Process the expected accept timeout.
    static void processAcceptTimeout(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const bsl::shared_ptr<ntci::Acceptor>&       acceptor,
        const bsl::shared_ptr<ntci::StreamSocket>&   streamSocket,
        const ntca::AcceptEvent&                     event,
        bslmt::Semaphore*                            semaphore);

    // Process the expected accept cancellation.
    static void processAcceptCancellation(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const bsl::shared_ptr<ntci::Acceptor>&       acceptor,
        const bsl::shared_ptr<ntci::StreamSocket>&   streamSocket,
        const ntca::AcceptEvent&                     event,
        bslmt::Semaphore*                            semaphore);

    /// Return an endpoint representing a suitable address to which to
    /// bind a socket of the specified 'transport' type for use by this
    /// test driver.
    static ntsa::Endpoint any(ntsa::Transport::Value transport);

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
    static void verifyAcceptDeadline();

    // TODO
    static void verifyAcceptCancellation();
};

/// Provide a test case execution framework.
class ListenerSocketTest::Framework
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
class ListenerSocketTest::Parameters
{
  public:
    ntsa::Transport::Value             d_transport;
    bsl::size_t                        d_numListeners;
    bsl::size_t                        d_numConnectionsPerListener;
    bsl::size_t                        d_numTimers;
    bsl::size_t                        d_numMessages;
    bsl::size_t                        d_messageSize;
    bsl::shared_ptr<bdlbb::Blob>       d_message_sp;
    bsl::shared_ptr<ntci::RateLimiter> d_acceptRateLimiter_sp;
    bsl::size_t                        d_acceptQueueHighWatermark;
    bsl::shared_ptr<ntci::RateLimiter> d_readRateLimiter_sp;
    bsl::size_t                        d_readQueueHighWatermark;
    bsl::shared_ptr<ntci::RateLimiter> d_writeRateLimiter_sp;
    bsl::size_t                        d_writeQueueHighWatermark;
    bdlb::NullableValue<bsl::size_t>   d_sendBufferSize;
    bdlb::NullableValue<bsl::size_t>   d_receiveBufferSize;
    bool                               d_useAsyncCallbacks;

    Parameters()
    : d_transport(ntsa::Transport::e_TCP_IPV4_STREAM)
    , d_numListeners(1)
    , d_numConnectionsPerListener(1)
    , d_numTimers(0)
    , d_numMessages(1)
    , d_messageSize(32)
    , d_message_sp()
    , d_acceptRateLimiter_sp()
    , d_acceptQueueHighWatermark(static_cast<bsl::size_t>(-1))
    , d_readRateLimiter_sp()
    , d_readQueueHighWatermark(static_cast<bsl::size_t>(-1))
    , d_writeRateLimiter_sp()
    , d_writeQueueHighWatermark(static_cast<bsl::size_t>(-1))
    , d_sendBufferSize()
    , d_receiveBufferSize()
    , d_useAsyncCallbacks(false)
    {
    }
};

/// Provides a listener socket protocol for this test driver.
class ListenerSocketTest::ListenerSocketApplication
: public ntci::ListenerSocketSession
{
    ntccfg::Object                        d_object;
    bsl::shared_ptr<ntci::ListenerSocket> d_listenerSocket_sp;
    bslmt::Latch                          d_numTimerEvents;
    bslmt::Latch                          d_numSocketsAccepted;
    ListenerSocketTest::Parameters        d_parameters;
    bslma::Allocator*                     d_allocator_p;

  private:
    ListenerSocketApplication(const ListenerSocketApplication&)
        BSLS_KEYWORD_DELETED;
    ListenerSocketApplication& operator=(const ListenerSocketApplication&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Process the condition that the size of the accept queue is greater
    /// than or equal to the accept queue low watermark.
    void processAcceptQueueLowWatermark(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the completion or failure of an asynchronous accept by the
    /// specified 'listenerSocket' of the specified 'streamSocket', if any,
    /// according to the specified 'event'.
    void processAccept(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const bsl::shared_ptr<ntci::Acceptor>&       acceptor,
        const bsl::shared_ptr<ntci::StreamSocket>&   streamSocket,
        const ntca::AcceptEvent&                     event);

    /// Process a timer event.
    void processTimer(const bsl::shared_ptr<ntci::Timer>& timer,
                      const ntca::TimerEvent&             event);

  public:
    /// Create a new listener socket application implemented using the
    /// specified 'listenerSocket' that operates according to the specified
    /// test 'parameters'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    ListenerSocketApplication(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ListenerSocketTest::Parameters&        parameters,
        bslma::Allocator*                            basicAllocator = 0);

    /// Destroy this object.
    ~ListenerSocketApplication() BSLS_KEYWORD_OVERRIDE;

    /// Schedule the timers for which this socket is responsible.
    void schedule();

    /// Begin accepting connections.
    void accept();

    /// Wait until all expected timers have fired and sockets have been
    /// accepted.
    void wait();

    /// Close the socket.
    void close();

    /// Return the source endpoint of the listener socket.
    ntsa::Endpoint sourceEndpoint() const;
};

/// This test provides a stream socket protocol for this test driver.
class ListenerSocketTest::StreamSocketSession
: public ntci::StreamSocketSession
{
    ntccfg::Object                      d_object;
    bsl::shared_ptr<ntci::StreamSocket> d_streamSocket_sp;
    bdlbb::Blob                         d_dataReceived;
    bslmt::Latch                        d_numTimerEvents;
    bsls::AtomicUint                    d_numMessagesLeftToSend;
    bslmt::Latch                        d_numMessagesSent;
    bslmt::Latch                        d_numMessagesReceived;
    ListenerSocketTest::Parameters      d_parameters;
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
    /// specified 'streamSocket' that operates according to the specified
    /// test 'parameters'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    StreamSocketSession(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ListenerSocketTest::Parameters&      parameters,
        bslma::Allocator*                          basicAllocator = 0);

    /// Destroy this object.
    ~StreamSocketSession() BSLS_KEYWORD_OVERRIDE;

    /// Schedule the timers for which this socket is responsible.
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
class ListenerSocketTest::StreamSocketManager
: public ntci::ListenerSocketManager,
  public ntccfg::Shared<StreamSocketManager>
{
    typedef bsl::unordered_map<bsl::shared_ptr<ntci::ListenerSocket>,
                               bsl::shared_ptr<ListenerSocketApplication> >
        ListenerSocketApplicationMap;

    /// Define a type alias for a set of managed stream
    /// sockets.
    typedef bsl::unordered_map<bsl::shared_ptr<ntci::StreamSocket>,
                               bsl::shared_ptr<StreamSocketSession> >
        StreamSocketApplicationMap;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    ntccfg::Object                  d_object;
    bsl::shared_ptr<ntci::Proactor> d_proactor_sp;
    bsl::shared_ptr<ntcs::Metrics>  d_metrics_sp;
    Mutex                           d_listenerSocketMapMutex;
    ListenerSocketApplicationMap    d_listenerSocketMap;
    bslmt::Latch                    d_listenerSocketsEstablished;
    bslmt::Latch                    d_listenerSocketsClosed;
    Mutex                           d_streamSocketMapMutex;
    StreamSocketApplicationMap      d_streamSocketMap;
    bslmt::Latch                    d_streamSocketsConnected;
    bslmt::Latch                    d_streamSocketsEstablished;
    bslmt::Latch                    d_streamSocketsClosed;
    ListenerSocketTest::Parameters  d_parameters;
    bslma::Allocator*               d_allocator_p;

  private:
    StreamSocketManager(const StreamSocketManager&) BSLS_KEYWORD_DELETED;
    StreamSocketManager& operator=(const StreamSocketManager&)
        BSLS_KEYWORD_DELETED;

  private:
    void processListenerSocketEstablished(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
        BSLS_KEYWORD_OVERRIDE;
    // Process the establishment of the specified 'listenerSocket'.

    void processListenerSocketClosed(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
        BSLS_KEYWORD_OVERRIDE;
    // Process the closure of the specified 'listenerSocket'.

    void processStreamSocketEstablished(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
        BSLS_KEYWORD_OVERRIDE;
    // Process the establishment of the specified 'streamSocket'.

    /// Process the closure of the specified 'streamSocket'.
    void processStreamSocketClosed(const bsl::shared_ptr<ntci::StreamSocket>&
                                       streamSocket) BSLS_KEYWORD_OVERRIDE;

    /// Process the specified 'connectEvent' for the specified 'connector'
    /// that is the specified 'streamSocket'.
    void processConnect(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Connector>&    connector,
        const ntca::ConnectEvent&                  connectEvent);

  public:
    /// Create a new stream socket manager operating according to the
    /// specified test 'parameters' whose sockets are driven by the
    /// specified 'proactor'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    StreamSocketManager(const bsl::shared_ptr<ntci::Proactor>& proactor,
                        const ListenerSocketTest::Parameters&  parameters,
                        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~StreamSocketManager() BSLS_KEYWORD_OVERRIDE;

    /// Create two stream sockets, have them send data to each, and wait
    /// for each to receive the data.
    void run();
};

void ListenerSocketTest::Framework::runProactor(
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

void ListenerSocketTest::Framework::execute(
    const ExecuteCallback& executeCallback)
{
    Framework::execute(ntsa::Transport::e_TCP_IPV4_STREAM, executeCallback);
}

void ListenerSocketTest::Framework::execute(
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

#if defined(NTCP_LISTENER_SOCKET_TEST_DYNAMIC_LOAD_BALANCING)
        if (dynamicLoadBalancing !=
            NTCP_LISTENER_SOCKET_TEST_DYNAMIC_LOAD_BALANCING)
        {
            continue;
        }
#endif

        Framework::execute(transport, numThreads, executeCallback);
    }
}

void ListenerSocketTest::Framework::execute(
    ntsa::Transport::Value transport,
    bsl::size_t            numThreads,
    const ExecuteCallback& executeCallback)
{
    ntsa::Error error;

    BSLS_LOG_TRACE("Testing transport %s numThreads %d",
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

void ListenerSocketTest::ListenerSocketApplication::
    processAcceptQueueLowWatermark(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (d_parameters.d_useAsyncCallbacks) {
        return;
    }

    while (true) {
        ntca::AcceptContext                 acceptContext;
        bsl::shared_ptr<ntci::StreamSocket> streamSocket;
        error = d_listenerSocket_sp->accept(&acceptContext,
                                            &streamSocket,
                                            ntca::AcceptOptions());
        if (error) {
            if (error == ntsa::Error::e_WOULD_BLOCK) {
                break;
            }
            else if (error == ntsa::Error::e_EOF) {
                NTCI_LOG_DEBUG(
                    "Listener socket %d at %s accepted EOF",
                    (int)(d_listenerSocket_sp->handle()),
                    d_listenerSocket_sp->sourceEndpoint().text().c_str());
                break;
            }
            else {
                NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
            }
        }

        error = streamSocket->open();
        NTSCFG_TEST_FALSE(error);

        NTCI_LOG_DEBUG(
            "Listener socket %d at %s accepted stream socket %d at %s to %s "
            "(%d/%d)",
            (int)(d_listenerSocket_sp->handle()),
            d_listenerSocket_sp->sourceEndpoint().text().c_str(),
            (int)(streamSocket->handle()),
            streamSocket->sourceEndpoint().text().c_str(),
            streamSocket->remoteEndpoint().text().c_str(),
            (int)((d_parameters.d_numConnectionsPerListener -
                   d_numSocketsAccepted.currentCount()) +
                  1),
            (int)(d_parameters.d_numConnectionsPerListener));

        d_numSocketsAccepted.arrive();
    }
}

void ListenerSocketTest::ListenerSocketApplication::processAccept(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const bsl::shared_ptr<ntci::Acceptor>&       acceptor,
    const bsl::shared_ptr<ntci::StreamSocket>&   streamSocket,
    const ntca::AcceptEvent&                     event)
{
    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::AcceptEventType::e_ERROR) {
        NTSCFG_TEST_EQ(event.context().error(), ntsa::Error::e_EOF);

        NTCI_LOG_DEBUG("Listener socket %d at %s asynchronously accepted EOF",
                       (int)(d_listenerSocket_sp->handle()),
                       d_listenerSocket_sp->sourceEndpoint().text().c_str());
    }
    else {
        NTSCFG_TEST_FALSE(event.context().error());

        ntsa::Error openError = streamSocket->open();
        NTSCFG_TEST_FALSE(openError);

        NTCI_LOG_DEBUG("Listener socket %d at %s asynchronously accepted "
                       "stream socket %d at %s to %s",
                       (int)(d_listenerSocket_sp->handle()),
                       d_listenerSocket_sp->sourceEndpoint().text().c_str(),
                       (int)(streamSocket->handle()),
                       streamSocket->sourceEndpoint().text().c_str(),
                       streamSocket->remoteEndpoint().text().c_str());

        ntci::AcceptCallback acceptCallback =
            d_listenerSocket_sp->createAcceptCallback(
                NTCCFG_BIND(&ListenerSocketApplication::processAccept,
                            this,
                            d_listenerSocket_sp,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3),
                d_allocator_p);

        NTSCFG_TEST_EQ(acceptCallback.strand(), d_listenerSocket_sp->strand());

        ntsa::Error acceptError =
            d_listenerSocket_sp->accept(ntca::AcceptOptions(), acceptCallback);
        NTSCFG_TEST_OK(acceptError);

        d_numSocketsAccepted.arrive();
    }
}

void ListenerSocketTest::ListenerSocketApplication::processTimer(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        NTCI_LOG_DEBUG("Listener socket %d at %s timer %d/%d has fired",
                       (int)(d_listenerSocket_sp->handle()),
                       d_listenerSocket_sp->sourceEndpoint().text().c_str(),
                       (int)((d_parameters.d_numTimers -
                              d_numTimerEvents.currentCount()) +
                             1),
                       (int)(d_parameters.d_numTimers));

        d_numTimerEvents.arrive();
    }
    else if (event.type() == ntca::TimerEventType::e_CANCELED) {
        BSLS_ASSERT(event.context().error() == ntsa::Error::e_CANCELLED);

        NTCI_LOG_DEBUG("Listener socket %d at %s timer has been canceled",
                       (int)(d_listenerSocket_sp->handle()),
                       d_listenerSocket_sp->sourceEndpoint().text().c_str());

        d_numTimerEvents.arrive();
    }
}

ListenerSocketTest::ListenerSocketApplication::ListenerSocketApplication(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ListenerSocketTest::Parameters&        parameters,
    bslma::Allocator*                            basicAllocator)
: d_object("ListenerSocketTest::ListenerSocketApplication")
, d_listenerSocket_sp(listenerSocket)
, d_numTimerEvents(parameters.d_numTimers)
, d_numSocketsAccepted(parameters.d_numConnectionsPerListener)
, d_parameters(parameters)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ListenerSocketTest::ListenerSocketApplication::~ListenerSocketApplication()
{
}

void ListenerSocketTest::ListenerSocketApplication::schedule()
{
    NTCI_LOG_CONTEXT();

    bsls::TimeInterval now = d_listenerSocket_sp->currentTime();

    NTCCFG_FOREACH(bsl::size_t, timerIndex, 0, d_parameters.d_numTimers)
    {
        ntca::TimerOptions timerOptions;
        timerOptions.setOneShot(true);

        ntci::TimerCallback timerCallback =
            d_listenerSocket_sp->createTimerCallback(
                bdlf::MemFnUtil::memFn(
                    &ListenerSocketApplication::processTimer,
                    this),
                d_allocator_p);

        NTSCFG_TEST_EQ(timerCallback.strand(), d_listenerSocket_sp->strand());

        bsl::shared_ptr<ntci::Timer> timer =
            d_listenerSocket_sp->createTimer(timerOptions,
                                             timerCallback,
                                             d_allocator_p);

        bsls::TimeInterval timerDeadline = now;
        timerDeadline.addMilliseconds(timerIndex);

        NTCI_LOG_DEBUG("Listener socket %d at %s starting timer %d/%d",
                       (int)(d_listenerSocket_sp->handle()),
                       d_listenerSocket_sp->sourceEndpoint().text().c_str(),
                       (int)(timerIndex + 1),
                       (int)(d_parameters.d_numTimers));

        timer->schedule(timerDeadline);
    }
}

void ListenerSocketTest::ListenerSocketApplication::accept()
{
    ntsa::Error error;

    if (d_parameters.d_useAsyncCallbacks) {
        ntci::AcceptCallback acceptCallback =
            d_listenerSocket_sp->createAcceptCallback(
                NTCCFG_BIND(&ListenerSocketApplication::processAccept,
                            this,
                            d_listenerSocket_sp,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3),
                d_allocator_p);

        NTSCFG_TEST_EQ(acceptCallback.strand(), d_listenerSocket_sp->strand());

        error =
            d_listenerSocket_sp->accept(ntca::AcceptOptions(), acceptCallback);
        NTSCFG_TEST_OK(error);
    }

    d_listenerSocket_sp->relaxFlowControl(ntca::FlowControlType::e_RECEIVE);
}

void ListenerSocketTest::ListenerSocketApplication::wait()
{
    d_numTimerEvents.wait();
    d_numSocketsAccepted.wait();
}

void ListenerSocketTest::ListenerSocketApplication::close()
{
    ntsa::Error error;

    error = d_listenerSocket_sp->shutdown();
    NTSCFG_TEST_FALSE(error);

    if (d_parameters.d_useAsyncCallbacks) {
        ntci::ListenerSocketCloseGuard guard(d_listenerSocket_sp);
    }
    else {
        d_listenerSocket_sp->close();
    }
}

ntsa::Endpoint ListenerSocketTest::ListenerSocketApplication::sourceEndpoint()
    const
{
    return d_listenerSocket_sp->sourceEndpoint();
}

void ListenerSocketTest::StreamSocketSession::processReadQueueLowWatermark(
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

void ListenerSocketTest::StreamSocketSession::processWriteQueueLowWatermark(
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

void ListenerSocketTest::StreamSocketSession::processRead(
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

void ListenerSocketTest::StreamSocketSession::processWrite(
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

void ListenerSocketTest::StreamSocketSession::processTimer(
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

ListenerSocketTest::StreamSocketSession::StreamSocketSession(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ListenerSocketTest::Parameters&      parameters,
    bslma::Allocator*                          basicAllocator)
: d_object("ListenerSocketTest::StreamSocketSession")
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

ListenerSocketTest::StreamSocketSession::~StreamSocketSession()
{
}

void ListenerSocketTest::StreamSocketSession::schedule()
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

void ListenerSocketTest::StreamSocketSession::send()
{
    ntca::WriteQueueEvent event;
    event.setType(ntca::WriteQueueEventType::e_LOW_WATERMARK);

    d_streamSocket_sp->execute(
        NTCCFG_BIND(&StreamSocketSession::processWriteQueueLowWatermark,
                    this,
                    d_streamSocket_sp,
                    event));
}

void ListenerSocketTest::StreamSocketSession::receive()
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

void ListenerSocketTest::StreamSocketSession::wait()
{
    d_numTimerEvents.wait();
    if (d_parameters.d_useAsyncCallbacks) {
        d_numMessagesSent.wait();
    }
    d_numMessagesReceived.wait();
}

void ListenerSocketTest::StreamSocketSession::close()
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

ntsa::Endpoint ListenerSocketTest::StreamSocketSession::sourceEndpoint() const
{
    return d_streamSocket_sp->sourceEndpoint();
}

ntsa::Endpoint ListenerSocketTest::StreamSocketSession::remoteEndpoint() const
{
    return d_streamSocket_sp->remoteEndpoint();
}

void ListenerSocketTest::StreamSocketManager::processListenerSocketEstablished(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Listener socket %d established",
                   (int)(listenerSocket->handle()));

    bsl::shared_ptr<ListenerSocketApplication> listenerSocketApplication;
    listenerSocketApplication.createInplace(d_allocator_p,
                                            listenerSocket,
                                            d_parameters,
                                            d_allocator_p);

    listenerSocket->registerSession(listenerSocketApplication);

    if (d_parameters.d_acceptRateLimiter_sp) {
        listenerSocket->setAcceptRateLimiter(
            d_parameters.d_acceptRateLimiter_sp);
    }

    {
        LockGuard guard(&d_listenerSocketMapMutex);
        d_listenerSocketMap.insert(ListenerSocketApplicationMap::value_type(
            listenerSocket,
            listenerSocketApplication));
    }

    listenerSocketApplication->accept();

    d_listenerSocketsEstablished.arrive();
}

void ListenerSocketTest::StreamSocketManager::processListenerSocketClosed(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Listener socket %d closed",
                   (int)(listenerSocket->handle()));

    {
        LockGuard   guard(&d_listenerSocketMapMutex);
        bsl::size_t n = d_listenerSocketMap.erase(listenerSocket);
        NTSCFG_TEST_EQ(n, 1);
    }

    d_listenerSocketsClosed.arrive();
}

void ListenerSocketTest::StreamSocketManager::processStreamSocketEstablished(
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

    if (d_parameters.d_writeRateLimiter_sp) {
        streamSocket->setWriteRateLimiter(d_parameters.d_writeRateLimiter_sp);
    }

    {
        LockGuard guard(&d_streamSocketMapMutex);
        d_streamSocketMap.insert(
            StreamSocketApplicationMap::value_type(streamSocket,
                                                   streamSocketSession));
    }

    streamSocketSession->receive();

    d_streamSocketsEstablished.arrive();
}

void ListenerSocketTest::StreamSocketManager::processStreamSocketClosed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket %d closed", (int)(streamSocket->handle()));

    {
        LockGuard   guard(&d_streamSocketMapMutex);
        bsl::size_t n = d_streamSocketMap.erase(streamSocket);
        NTSCFG_TEST_EQ(n, 1);
    }

    d_streamSocketsClosed.arrive();
}

void ListenerSocketTest::StreamSocketManager::processConnect(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Connector>&    connector,
    const ntca::ConnectEvent&                  connectEvent)
{
    NTSCFG_TEST_EQ(connectEvent.type(), ntca::ConnectEventType::e_COMPLETE);
    d_streamSocketsConnected.arrive();
}

ListenerSocketTest::StreamSocketManager::StreamSocketManager(
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    const ListenerSocketTest::Parameters&  parameters,
    bslma::Allocator*                      basicAllocator)
: d_object("ListenerSocketTest::StreamSocketManager")
, d_proactor_sp(proactor)
, d_metrics_sp()
, d_listenerSocketMapMutex()
, d_listenerSocketMap(basicAllocator)
, d_listenerSocketsEstablished(parameters.d_numListeners)
, d_listenerSocketsClosed(parameters.d_numListeners)
, d_streamSocketMapMutex()
, d_streamSocketMap(basicAllocator)
, d_streamSocketsConnected(parameters.d_numListeners *
                           parameters.d_numConnectionsPerListener)
, d_streamSocketsEstablished(
      2 * (parameters.d_numListeners * parameters.d_numConnectionsPerListener))
, d_streamSocketsClosed(
      2 * (parameters.d_numListeners * parameters.d_numConnectionsPerListener))
, d_parameters(parameters)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ListenerSocketTest::StreamSocketManager::~StreamSocketManager()
{
    NTSCFG_TEST_TRUE(d_listenerSocketMap.empty());
    NTSCFG_TEST_TRUE(d_streamSocketMap.empty());
}

void ListenerSocketTest::StreamSocketManager::run()
{
    ntsa::Error error;

    // Create all the listener sockets.

    for (bsl::size_t i = 0; i < d_parameters.d_numListeners; ++i) {
        ntca::ListenerSocketOptions options;
        options.setTransport(d_parameters.d_transport);
        options.setSourceEndpoint(
            ListenerSocketTest::any(d_parameters.d_transport));
        options.setAcceptQueueLowWatermark(1);
        options.setAcceptQueueHighWatermark(
            d_parameters.d_acceptQueueHighWatermark);
        options.setReadQueueLowWatermark(d_parameters.d_messageSize);
        options.setReadQueueHighWatermark(
            d_parameters.d_readQueueHighWatermark);
        options.setWriteQueueLowWatermark(0);
        options.setWriteQueueHighWatermark(
            d_parameters.d_writeQueueHighWatermark);
        options.setAcceptGreedily(false);
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

        bsl::shared_ptr<ntci::Resolver> resolver;

        bsl::shared_ptr<ntcp::ListenerSocket> listenerSocket;
        listenerSocket.createInplace(d_allocator_p,
                                     options,
                                     resolver,
                                     d_proactor_sp,
                                     d_proactor_sp,
                                     d_metrics_sp,
                                     d_allocator_p);

        error = listenerSocket->registerManager(this->getSelf(this));
        NTSCFG_TEST_FALSE(error);

        bsl::shared_ptr<ntcd::ListenerSocket> listenerSocketBase;
        listenerSocketBase.createInplace(d_allocator_p, d_allocator_p);

        error =
            listenerSocket->open(d_parameters.d_transport, listenerSocketBase);
        NTSCFG_TEST_FALSE(error);

        error = listenerSocket->listen();
        NTSCFG_TEST_FALSE(error);
    }

    // Wait for all the listener sockets to become established.

    d_listenerSocketsEstablished.wait();

    // Connect the configured number of sockets to each listener.

    {
        LockGuard guard(&d_listenerSocketMapMutex);

        for (ListenerSocketApplicationMap::const_iterator it =
                 d_listenerSocketMap.begin();
             it != d_listenerSocketMap.end();
             ++it)
        {
            const bsl::shared_ptr<ListenerSocketApplication>& listenerSocket =
                it->second;

            for (bsl::size_t i = 0;
                 i < d_parameters.d_numConnectionsPerListener;
                 ++i)
            {
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
                    options.setSendBufferSize(
                        d_parameters.d_sendBufferSize.value());
                }

                if (!d_parameters.d_receiveBufferSize.isNull()) {
                    options.setReceiveBufferSize(
                        d_parameters.d_receiveBufferSize.value());
                }

                bsl::shared_ptr<ntci::Resolver> resolver;

                bsl::shared_ptr<ntcp::StreamSocket> streamSocket;
                streamSocket.createInplace(d_allocator_p,
                                           options,
                                           resolver,
                                           d_proactor_sp,
                                           d_proactor_sp,
                                           d_metrics_sp,
                                           d_allocator_p);

                error = streamSocket->registerManager(this->getSelf(this));
                NTSCFG_TEST_FALSE(error);

                bsl::shared_ptr<ntcd::StreamSocket> streamSocketBase;
                streamSocketBase.createInplace(d_allocator_p, d_allocator_p);

                error = streamSocket->open(d_parameters.d_transport,
                                           streamSocketBase);
                NTSCFG_TEST_FALSE(error);

                ntci::ConnectCallback connectCallback =
                    streamSocket->createConnectCallback(
                        NTCCFG_BIND(&StreamSocketManager::processConnect,
                                    this,
                                    streamSocket,
                                    NTCCFG_BIND_PLACEHOLDER_1,
                                    NTCCFG_BIND_PLACEHOLDER_2),
                        d_allocator_p);

                error = streamSocket->connect(listenerSocket->sourceEndpoint(),
                                              ntca::ConnectOptions(),
                                              connectCallback);
                NTSCFG_TEST_OK(error);
            }
        }
    }

    // Wait for all the stream sockets to become connected.

    d_streamSocketsConnected.wait();

    // Wait for all the stream sockets to become established.

    d_streamSocketsEstablished.wait();

    // Start the timers for each listener socket.

    {
        LockGuard guard(&d_listenerSocketMapMutex);

        for (ListenerSocketApplicationMap::const_iterator it =
                 d_listenerSocketMap.begin();
             it != d_listenerSocketMap.end();
             ++it)
        {
            const bsl::shared_ptr<ListenerSocketApplication>& listenerSocket =
                it->second;
            listenerSocket->schedule();
        }
    }

    // Start the timers for each stream socket.

    {
        LockGuard guard(&d_streamSocketMapMutex);

        for (StreamSocketApplicationMap::const_iterator it =
                 d_streamSocketMap.begin();
             it != d_streamSocketMap.end();
             ++it)
        {
            const bsl::shared_ptr<StreamSocketSession>& streamSocket =
                it->second;
            streamSocket->schedule();
        }
    }

    // Send data from each connected socket pair.

    {
        LockGuard guard(&d_streamSocketMapMutex);

        for (StreamSocketApplicationMap::const_iterator it =
                 d_streamSocketMap.begin();
             it != d_streamSocketMap.end();
             ++it)
        {
            const bsl::shared_ptr<StreamSocketSession>& streamSocket =
                it->second;
            streamSocket->send();
        }
    }

    // Wait for all timers to fire and all messages to be received for each
    // stream socket.

    {
        LockGuard guard(&d_streamSocketMapMutex);

        for (StreamSocketApplicationMap::const_iterator it =
                 d_streamSocketMap.begin();
             it != d_streamSocketMap.end();
             ++it)
        {
            const bsl::shared_ptr<StreamSocketSession>& streamSocket =
                it->second;
            streamSocket->wait();
        }
    }

    // Wait for all timers to fire and all sockets to be accepted for each
    // listener socket.

    {
        LockGuard guard(&d_listenerSocketMapMutex);

        for (ListenerSocketApplicationMap::const_iterator it =
                 d_listenerSocketMap.begin();
             it != d_listenerSocketMap.end();
             ++it)
        {
            const bsl::shared_ptr<ListenerSocketApplication>& listenerSocket =
                it->second;
            listenerSocket->wait();
        }
    }

    // Close all the stream sockets.

    {
        typedef bsl::vector<bsl::shared_ptr<StreamSocketSession> >
            StreamSocketApplicationVector;

        StreamSocketApplicationVector streamSocketVector;
        streamSocketVector.reserve(d_streamSocketMap.size());

        {
            LockGuard guard(&d_streamSocketMapMutex);

            for (StreamSocketApplicationMap::const_iterator it =
                     d_streamSocketMap.begin();
                 it != d_streamSocketMap.end();
                 ++it)
            {
                const bsl::shared_ptr<StreamSocketSession>& socket =
                    it->second;

                streamSocketVector.push_back(socket);
            }
        }

        for (StreamSocketApplicationVector::const_iterator it =
                 streamSocketVector.begin();
             it != streamSocketVector.end();
             ++it)
        {
            const bsl::shared_ptr<StreamSocketSession>& socket = *it;
            socket->close();
        }
    }

    // Wait for all stream sockets to close.

    d_streamSocketsClosed.wait();

    // Close all the listener sockets.

    {
        typedef bsl::vector<bsl::shared_ptr<ListenerSocketApplication> >
            ListenerSocketApplicationVector;

        ListenerSocketApplicationVector listenerSocketVector;
        listenerSocketVector.reserve(d_listenerSocketMap.size());

        {
            LockGuard guard(&d_listenerSocketMapMutex);

            for (ListenerSocketApplicationMap::const_iterator it =
                     d_listenerSocketMap.begin();
                 it != d_listenerSocketMap.end();
                 ++it)
            {
                const bsl::shared_ptr<ListenerSocketApplication>& socket =
                    it->second;

                listenerSocketVector.push_back(socket);
            }
        }

        for (ListenerSocketApplicationVector::const_iterator it =
                 listenerSocketVector.begin();
             it != listenerSocketVector.end();
             ++it)
        {
            const bsl::shared_ptr<ListenerSocketApplication>& socket = *it;
            socket->close();
        }
    }

    // Wait for all listener sockets to close.

    d_listenerSocketsClosed.wait();
}

void ListenerSocketTest::verifyGenericVariation(
    ntsa::Transport::Value                 transport,
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    const ListenerSocketTest::Parameters&  parameters,
    bslma::Allocator*                      allocator)
{
    // Test accepting using asynchronous accept callbacks and reading data
    // using asynchronous read callbacks.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Listener socket test starting");

    ListenerSocketTest::Parameters effectiveParameters = parameters;
    effectiveParameters.d_transport                    = transport;

    bsl::shared_ptr<ListenerSocketTest::StreamSocketManager>
        streamSocketManager;
    streamSocketManager.createInplace(allocator,
                                      proactor,
                                      effectiveParameters,
                                      allocator);

    streamSocketManager->run();
    streamSocketManager.reset();

    NTCI_LOG_DEBUG("Listener socket test complete");

    proactor->stop();
}

void ListenerSocketTest::verifyAcceptDeadlineVariation(
    ntsa::Transport::Value                 transport,
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    const ListenerSocketTest::Parameters&  parameters,
    bslma::Allocator*                      allocator)
{
    // Concern: Accept deadlines.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Listener socket accept deadline test starting");

    const int k_ACCEPT_TIMEOUT_IN_MILLISECONDS = 200;

    ntsa::Error                    error;
    bslmt::Semaphore               semaphore;
    bsl::shared_ptr<ntcs::Metrics> metrics;

    ntca::ListenerSocketOptions options;
    options.setTransport(transport);
    options.setSourceEndpoint(ListenerSocketTest::any(transport));

    bsl::shared_ptr<ntci::Resolver> resolver;

    bsl::shared_ptr<ntcp::ListenerSocket> listenerSocket;
    listenerSocket.createInplace(allocator,
                                 options,
                                 resolver,
                                 proactor,
                                 proactor,
                                 metrics,
                                 allocator);

    bsl::shared_ptr<ntcd::ListenerSocket> listenerSocketBase;
    listenerSocketBase.createInplace(allocator, allocator);

    error = listenerSocket->open(transport, listenerSocketBase);
    NTSCFG_TEST_FALSE(error);

    error = listenerSocket->listen();
    NTSCFG_TEST_FALSE(error);

    bsls::TimeInterval acceptTimeout;
    acceptTimeout.setTotalMilliseconds(k_ACCEPT_TIMEOUT_IN_MILLISECONDS);

    bsls::TimeInterval acceptDeadline =
        listenerSocket->currentTime() + acceptTimeout;

    ntca::AcceptOptions acceptOptions;
    acceptOptions.setDeadline(acceptDeadline);

    ntci::AcceptCallback acceptCallback = listenerSocket->createAcceptCallback(
        NTCCFG_BIND(&ListenerSocketTest::processAcceptTimeout,
                    listenerSocket,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    &semaphore),
        allocator);

    error = listenerSocket->accept(acceptOptions, acceptCallback);
    NTSCFG_TEST_OK(error);

    semaphore.wait();

    {
        ntci::ListenerSocketCloseGuard listenerSocketCloseGuard(
            listenerSocket);
    }

    NTCI_LOG_DEBUG("Listener socket accept deadline test complete");

    proactor->stop();
}

void ListenerSocketTest::verifyAcceptCancellationVariation(
    ntsa::Transport::Value                 transport,
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    const ListenerSocketTest::Parameters&  parameters,
    bslma::Allocator*                      allocator)
{
    // Concern: Accept cancellation.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Listener socket accept cancellation test starting");

    const int k_ACCEPT_TIMEOUT_IN_MILLISECONDS = 200;

    ntsa::Error                    error;
    bslmt::Semaphore               semaphore;
    bsl::shared_ptr<ntcs::Metrics> metrics;

    ntca::ListenerSocketOptions options;
    options.setTransport(transport);
    options.setSourceEndpoint(ListenerSocketTest::any(transport));

    bsl::shared_ptr<ntci::Resolver> resolver;

    bsl::shared_ptr<ntcp::ListenerSocket> listenerSocket;
    listenerSocket.createInplace(allocator,
                                 options,
                                 resolver,
                                 proactor,
                                 proactor,
                                 metrics,
                                 allocator);

    bsl::shared_ptr<ntcd::ListenerSocket> listenerSocketBase;
    listenerSocketBase.createInplace(allocator, allocator);

    error = listenerSocket->open(transport, listenerSocketBase);
    NTSCFG_TEST_FALSE(error);

    error = listenerSocket->listen();
    NTSCFG_TEST_FALSE(error);

    bsls::TimeInterval acceptTimeout;
    acceptTimeout.setTotalMilliseconds(k_ACCEPT_TIMEOUT_IN_MILLISECONDS);

    bsls::TimeInterval acceptDeadline =
        listenerSocket->currentTime() + acceptTimeout;

    ntca::AcceptToken acceptToken;
    acceptToken.setValue(1);

    ntca::AcceptOptions acceptOptions;
    acceptOptions.setToken(acceptToken);

    ntci::AcceptCallback acceptCallback = listenerSocket->createAcceptCallback(
        NTCCFG_BIND(&ListenerSocketTest::processAcceptCancellation,
                    listenerSocket,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    &semaphore),
        allocator);

    error = listenerSocket->accept(acceptOptions, acceptCallback);
    NTSCFG_TEST_OK(error);

    ntca::TimerOptions timerOptions;

    timerOptions.setOneShot(true);
    timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    ntci::TimerCallback timerCallback = listenerSocket->createTimerCallback(
        bdlf::BindUtil::bind(&ListenerSocketTest::cancelAccept,
                             listenerSocket,
                             acceptToken),
        allocator);

    bsl::shared_ptr<ntci::Timer> timer =
        listenerSocket->createTimer(timerOptions, timerCallback, allocator);

    error = timer->schedule(acceptDeadline);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    {
        ntci::ListenerSocketCloseGuard listenerSocketCloseGuard(
            listenerSocket);
    }

    NTCI_LOG_DEBUG("Listener socket accept cancellation test complete");

    proactor->stop();
}

void ListenerSocketTest::cancelAccept(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptToken                      token)
{
    ntsa::Error error = listenerSocket->cancel(token);
    NTSCFG_TEST_FALSE(error);
}

void ListenerSocketTest::processAcceptTimeout(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const bsl::shared_ptr<ntci::Acceptor>&       acceptor,
    const bsl::shared_ptr<ntci::StreamSocket>&   streamSocket,
    const ntca::AcceptEvent&                     event,
    bslmt::Semaphore*                            semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing accept event type %s: %s",
                   ntca::AcceptEventType::toString(event.type()),
                   event.context().error().text().c_str());

    NTSCFG_TEST_EQ(event.type(), ntca::AcceptEventType::e_ERROR);
    NTSCFG_TEST_EQ(event.context().error(), ntsa::Error::e_WOULD_BLOCK);

    semaphore->post();
}

void ListenerSocketTest::processAcceptCancellation(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const bsl::shared_ptr<ntci::Acceptor>&       acceptor,
    const bsl::shared_ptr<ntci::StreamSocket>&   streamSocket,
    const ntca::AcceptEvent&                     event,
    bslmt::Semaphore*                            semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing accept event type %s: %s",
                   ntca::AcceptEventType::toString(event.type()),
                   event.context().error().text().c_str());

    NTSCFG_TEST_EQ(event.type(), ntca::AcceptEventType::e_ERROR);
    NTSCFG_TEST_EQ(event.context().error(), ntsa::Error::e_CANCELLED);

    semaphore->post();
}

ntsa::Endpoint ListenerSocketTest::any(ntsa::Transport::Value transport)
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

NTSCFG_TEST_FUNCTION(ntcp::ListenerSocketTest::verifyBreathing)
{
    // Concern: Breathing test.

    ListenerSocketTest::Parameters parameters;
    parameters.d_numTimers                 = 0;
    parameters.d_numListeners              = 1;
    parameters.d_numConnectionsPerListener = 1;
    parameters.d_numMessages               = 1;
    parameters.d_messageSize               = 32;
    parameters.d_useAsyncCallbacks         = false;

    ListenerSocketTest::Framework::execute(
        NTCCFG_BIND(&ListenerSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::ListenerSocketTest::verifyBreathingAsync)
{
    // Concern: Breathing test using asynchronous callbacks.

    ListenerSocketTest::Parameters parameters;
    parameters.d_numTimers                 = 0;
    parameters.d_numListeners              = 1;
    parameters.d_numConnectionsPerListener = 1;
    parameters.d_numMessages               = 1;
    parameters.d_messageSize               = 32;
    parameters.d_useAsyncCallbacks         = true;

    ListenerSocketTest::Framework::execute(
        NTCCFG_BIND(&ListenerSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::ListenerSocketTest::verifyStress)
{
    // Concern: Stress test.

    ListenerSocketTest::Parameters parameters;
    parameters.d_numTimers                 = 0;
    parameters.d_numListeners              = 10;
    parameters.d_numConnectionsPerListener = 10;
    parameters.d_numMessages               = 100;
    parameters.d_messageSize               = 32;
    parameters.d_useAsyncCallbacks         = false;

    ListenerSocketTest::Framework::execute(
        NTCCFG_BIND(&ListenerSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::ListenerSocketTest::verifyStressAsync)
{
    // Concern: Stress test using asynchronous callbacks.

    ListenerSocketTest::Parameters parameters;
    parameters.d_numTimers                 = 0;
    parameters.d_numListeners              = 10;
    parameters.d_numConnectionsPerListener = 10;
    parameters.d_numMessages               = 100;
    parameters.d_messageSize               = 32;
    parameters.d_useAsyncCallbacks         = true;

    ListenerSocketTest::Framework::execute(
        NTCCFG_BIND(&ListenerSocketTest::verifyGenericVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::ListenerSocketTest::verifyAcceptDeadline)
{
    ListenerSocketTest::Parameters parameters;

    ListenerSocketTest::Framework::execute(
        NTCCFG_BIND(&ListenerSocketTest::verifyAcceptDeadlineVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

NTSCFG_TEST_FUNCTION(ntcp::ListenerSocketTest::verifyAcceptCancellation)
{
    ListenerSocketTest::Parameters parameters;

    ListenerSocketTest::Framework::execute(
        NTCCFG_BIND(&ListenerSocketTest::verifyAcceptCancellationVariation,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    parameters,
                    NTCCFG_BIND_PLACEHOLDER_3));
}

}  // close namespace ntcp
}  // close namespace BloombergLP
