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

#include <ntco_poll.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntco_poll_cpp, "$Id$ $CSID$")

#if NTC_BUILD_WITH_POLL

#include <ntcr_datagramsocket.h>
#include <ntcr_listenersocket.h>
#include <ntcr_streamsocket.h>

#include <ntcm_monitorableregistry.h>
#include <ntcm_monitorableutil.h>

#include <ntci_log.h>
#include <ntci_mutex.h>
#include <ntcs_async.h>
#include <ntcs_authorization.h>
#include <ntcs_chronology.h>
#include <ntcs_controller.h>
#include <ntcs_datapool.h>
#include <ntcs_driver.h>
#include <ntcs_nomenclature.h>
#include <ntcs_reactormetrics.h>
#include <ntcs_registry.h>
#include <ntcs_reservation.h>
#include <ntcs_strand.h>
#include <ntcs_user.h>

#include <ntsf_system.h>

#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>
#include <bdlt_localtimeoffset.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_timeutil.h>

#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <errno.h>
#include <poll.h>
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif
#ifdef WINVER
#undef WINVER
#endif
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define NTDDI_VERSION 0x06000100
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// clang-format off
#include <windows.h>
#include <winerror.h>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
// clang-format on
#ifdef interface
#undef interface
#endif
#pragma comment(lib, "ws2_32")
#endif

// The flag that defines whether all waiters are interrupted when the polling
// device gains or loses interest in socket events.
#define NTCRO_POLL_INTERRUPT_ALL true

#define NTCO_POLL_LOG_WAIT_INDEFINITE()                                       \
    NTCI_LOG_TRACE("Polling for socket events indefinitely", timeout)

#define NTCO_POLL_LOG_WAIT_TIMED(timeout)                                     \
    NTCI_LOG_TRACE(                                                           \
        "Polling for sockets events or until %d milliseconds have elapsed",   \
        timeout)

#define NTCO_POLL_LOG_WAIT_FAILURE(error)                                     \
    NTCI_LOG_ERROR("Failed to poll for socket events: %s",                    \
                   error.text().c_str())

#define NTCO_POLL_LOG_WAIT_TIMEOUT()                                          \
    NTCI_LOG_TRACE("Timed out polling for socket events")

#define NTCO_POLL_LOG_WAIT_RESULT(numEvents)                                  \
    NTCI_LOG_TRACE("Polled %d socket events", numEvents)

#define NTCO_POLL_LOG_EVENTS(handle, event)                                   \
    NTCI_LOG_TRACE("Descriptor %d polled%s%s%s%s%s",                          \
                   handle,                                                    \
                   (((event.revents & POLLIN) != 0) ? " POLLIN" : ""),        \
                   (((event.revents & POLLOUT) != 0) ? " POLLOUT" : ""),      \
                   (((event.revents & POLLERR) != 0) ? " POLLERR" : ""),      \
                   (((event.revents & POLLHUP) != 0) ? " POLLHUP" : ""),      \
                   (((event.revents & POLLNVAL) != 0) ? " POLLNVAL" : ""))

#define NTCO_POLL_LOG_ADD(handle, interest)                                   \
    NTCI_LOG_TRACE("Descriptor %d added%s%s",                                 \
                   handle,                                                    \
                   ((interest.wantReadable()) ? " POLLIN" : ""),              \
                   ((interest.wantWritable()) ? " POLLOUT" : ""))

#define NTCO_POLL_LOG_UPDATE(handle, eventMask)                               \
    NTCI_LOG_TRACE("Descriptor %d updated%s%s",                               \
                   handle,                                                    \
                   ((interest.wantReadable()) ? " POLLIN" : ""),              \
                   ((interest.wantWritable()) ? " POLLOUT" : ""))

#define NTCO_POLL_LOG_REMOVE(handle)                                          \
    NTCI_LOG_TRACE("Descriptor %d removed", handle)

namespace BloombergLP {
namespace ntco {

/// Provide an implementation of the 'ntci::Reactor' interface
/// implemented using the 'poll' API. This class is thread safe.
class Poll : public ntci::Reactor,
             public ntcs::Driver,
             public ntccfg::Shared<Poll>
{
    /// This typedef defines a set of waiters.
    typedef bsl::unordered_set<ntci::Waiter> WaiterSet;

    /// This typedef defines a vector of poll structures.
    typedef bsl::vector<struct ::pollfd> DescriptorList;

    struct Result;
    // This struct describes the context of a waiter.

    enum UpdateType {
        // Enumerates the types of update.

        e_INCLUDE = 1,
        // The device is being modified to gain interest in certain
        // events.

        e_EXCLUDE = 2
        // The device is being modified to lose interest in certain
        // events.
    };

    /// This typedef defines a mutex.
    typedef ntci::Mutex Mutex;

    /// This typedef defines a mutex lock guard.
    typedef ntci::LockGuard LockGuard;

    typedef bsl::list<bsl::shared_ptr<ntcs::RegistryEntry> > DetachList;

    ntccfg::Object                           d_object;
    mutable Mutex                            d_generationMutex;
    bslmt::Semaphore                         d_generationSemaphore;
    bsls::AtomicUint64                       d_generation;
    Mutex                                    d_detachMutex;
    DetachList                               d_detachList;
    ntcs::RegistryEntryCatalog::EntryFunctor d_detachFunctor;
    ntcs::RegistryEntryCatalog               d_registry;
    ntcs::Chronology                         d_chronology;
    bsl::shared_ptr<ntci::User>              d_user_sp;
    bsl::shared_ptr<ntci::DataPool>          d_dataPool_sp;
    bsl::shared_ptr<ntci::Resolver>          d_resolver_sp;
    bsl::shared_ptr<ntci::Reservation>       d_connectionLimiter_sp;
    bsl::shared_ptr<ntci::ReactorMetrics>    d_metrics_sp;
    bsl::shared_ptr<ntcs::Controller>        d_controller_sp;
    ntsa::Handle                             d_controllerDescriptorHandle;
    mutable Mutex                            d_waiterSetMutex;
    WaiterSet                                d_waiterSet;
    bslmt::ThreadUtil::Handle                d_threadHandle;
    bsl::size_t                              d_threadIndex;
    bsls::AtomicUint64                       d_threadId;
    bool                                     d_dynamic;
    bsls::AtomicUint64                       d_load;
    bsls::AtomicBool                         d_run;
    ntca::ReactorConfig                      d_config;
    bslma::Allocator*                        d_allocator_p;

  private:
    Poll(const Poll&) BSLS_KEYWORD_DELETED;
    Poll& operator=(const Poll&) BSLS_KEYWORD_DELETED;

  private:
    /// Load into the specified 'result->fd' and 'result->events' the
    /// fields to monitor the specified 'handle' according to the specified
    /// event 'interest'.
    static void specify(struct ::pollfd* result,
                        ntsa::Handle     handle,
                        ntcs::Interest   interest);

    /// Execute all pending jobs.
    void flush();

    /// Add the specified 'handle' identified by the with the specified
    /// 'interest' to the device. Return the error.
    ntsa::Error add(ntsa::Handle handle, ntcs::Interest interest);

    /// Update the specified 'handle' with the specified 'interest' in the
    /// device. The specified 'type' indicates whether events have been
    /// included or excluded as a result of the update. Return the error.
    ntsa::Error update(ntsa::Handle   handle,
                       ntcs::Interest interest,
                       UpdateType     type);

    /// Remove the specified 'handle' from the device.
    ntsa::Error remove(ntsa::Handle handle);

    /// Remove the specified 'entry' from the device. Return the error.
    ntsa::Error removeDetached(
        const bsl::shared_ptr<ntcs::RegistryEntry>& entry);

    /// Link the specified 'entry' into the specified 'descriptorList'. Use the
    /// specified 'result' for intermediate storage of linkage results
    void link(const bsl::shared_ptr<ntcs::RegistryEntry>& entry,
              Poll::Result*                               result);

    /// Reinitialize the control mechanism and add it to the polled set.
    void reinitializeControl();

    /// Deinitialize the control mechanism and remove it from the polled
    /// set.
    void deinitializeControl();

    /// Return true if the current thread is the principle waiter, i.e.,
    /// the principle I/O thread in a statically load-balanced
    /// configuration, otherwise return false.
    bool isWaiter();

    /// Acquire usage of the most suitable reactor selected according to the
    /// specified load balancing 'options'.
    bsl::shared_ptr<ntci::Reactor> acquireReactor(
        const ntca::LoadBalancingOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Release usage of the specified 'reactor' selected according to the
    /// specified load balancing 'options'.
    void releaseReactor(const bsl::shared_ptr<ntci::Reactor>& reactor,
                        const ntca::LoadBalancingOptions&     options)
        BSLS_KEYWORD_OVERRIDE;

    /// Increment the current number of handle reservations, if permitted.
    /// Return true if the resulting number of handle reservations is
    /// permitted, and false otherwise.
    bool acquireHandleReservation() BSLS_KEYWORD_OVERRIDE;

    /// Decrement the current number of handle reservations.
    void releaseHandleReservation() BSLS_KEYWORD_OVERRIDE;

    /// Return the number of reactors in the thread pool.
    bsl::size_t numReactors() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current number of threads in the thread pool.
    bsl::size_t numThreads() const BSLS_KEYWORD_OVERRIDE;

    /// Return the minimum number of threads in the thread pool.
    bsl::size_t minThreads() const BSLS_KEYWORD_OVERRIDE;

    /// Return the maximum number of threads in the thread pool.
    bsl::size_t maxThreads() const BSLS_KEYWORD_OVERRIDE;

  public:
    /// Create a new reactor having the specified 'configuration' operating
    /// in the environment of the specified 'user'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Poll(const ntca::ReactorConfig&         configuration,
                  const bsl::shared_ptr<ntci::User>& user,
                  bslma::Allocator*                  basicAllocator = 0);

    /// Destroy this object.
    ~Poll() BSLS_KEYWORD_OVERRIDE;

    /// Register a thread described by the specified 'waiterOptions' that
    /// will drive this object. Return the handle to the waiter.
    ntci::Waiter registerWaiter(const ntca::WaiterOptions& waiterOptions)
        BSLS_KEYWORD_OVERRIDE;

    /// Deregister the specified 'waiter'.
    void deregisterWaiter(ntci::Waiter waiter) BSLS_KEYWORD_OVERRIDE;

    /// Create a new strand to serialize execution of functors by the
    /// threads driving this reactor. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    bsl::shared_ptr<ntci::Strand> createStrand(
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Start monitoring the specified 'socket'. Return the error.
    ntsa::Error attachSocket(const bsl::shared_ptr<ntci::ReactorSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;

    /// Start monitoring the specified socket 'handle'. Return the error.
    ntsa::Error attachSocket(ntsa::Handle handle) BSLS_KEYWORD_OVERRIDE;

    /// Start monitoring for readability of the specified 'socket' according
    /// to the specified 'options'. Return the error.
    ntsa::Error showReadable(
        const bsl::shared_ptr<ntci::ReactorSocket>& socket,
        const ntca::ReactorEventOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Start monitoring for readability of the specified socket 'handle'
    /// according to the specified 'options'. Invoke the specified
    /// 'callback' when the socket becomes readable. Return the error.
    ntsa::Error showReadable(ntsa::Handle                      handle,
                             const ntca::ReactorEventOptions&  options,
                             const ntci::ReactorEventCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Start monitoring for writability of the specified 'socket' according
    /// to the specified 'options'. Return the error.
    ntsa::Error showWritable(
        const bsl::shared_ptr<ntci::ReactorSocket>& socket,
        const ntca::ReactorEventOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Start monitoring for writability of the specified socket 'handle'
    /// according to the specified 'options'. Invoke the specified
    /// 'callback' when the socket becomes writable. Return the error.
    ntsa::Error showWritable(ntsa::Handle                      handle,
                             const ntca::ReactorEventOptions&  options,
                             const ntci::ReactorEventCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Start monitoring for errors of the specified 'socket' according to
    /// the specified 'options'. Return the error.
    ntsa::Error showError(const bsl::shared_ptr<ntci::ReactorSocket>& socket,
                          const ntca::ReactorEventOptions&            options)
        BSLS_KEYWORD_OVERRIDE;

    /// Start monitoring for errors of the specified socket 'handle'
    /// according to the specified 'options'. Invoke the specified
    /// 'callback' when the socket has an error. Return the error.
    ntsa::Error showError(ntsa::Handle                      handle,
                          const ntca::ReactorEventOptions&  options,
                          const ntci::ReactorEventCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Start monitoring for notifications of the specified 'socket'. Return
    /// the error.
    ntsa::Error showNotifications(const bsl::shared_ptr<ntci::ReactorSocket>&
                                      socket) BSLS_KEYWORD_OVERRIDE;

    /// Start monitoring for notifications of the specified socket 'handle'.
    /// Invoke the specified 'callback' when there are notifications to be
    /// processed. Return the error.
    ntsa::Error showNotifications(ntsa::Handle handle,
                                  const ntci::ReactorNotificationCallback&
                                      callback) BSLS_KEYWORD_OVERRIDE;

    /// Stop monitoring for readability of the specified 'socket'. Return
    /// the error.
    ntsa::Error hideReadable(const bsl::shared_ptr<ntci::ReactorSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;

    /// Stop monitoring for readability of the specified socket 'handle'.
    /// Return the error.
    ntsa::Error hideReadable(ntsa::Handle handle) BSLS_KEYWORD_OVERRIDE;

    /// Stop monitoring for writability of the specified 'socket'. Return
    /// the error.
    ntsa::Error hideWritable(const bsl::shared_ptr<ntci::ReactorSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;

    /// Stop monitoring for writability of the specified socket
    /// 'handle'. Return the error.
    ntsa::Error hideWritable(ntsa::Handle handle) BSLS_KEYWORD_OVERRIDE;

    /// Stop monitoring for errors of the specified 'socket'. Return the
    /// error.
    ntsa::Error hideError(const bsl::shared_ptr<ntci::ReactorSocket>& socket)
        BSLS_KEYWORD_OVERRIDE;

    /// Stop monitoring for errors of the specified socket 'handle'. Return
    /// the error.
    ntsa::Error hideError(ntsa::Handle handle) BSLS_KEYWORD_OVERRIDE;

    /// Stop monitoring for notifications of the specified 'socket'. Return
    /// the error.
    ntsa::Error hideNotifications(const bsl::shared_ptr<ntci::ReactorSocket>&
                                      socket) BSLS_KEYWORD_OVERRIDE;

    /// Stop monitoring for notifications of the specified socket 'handle'.
    /// Return the error.
    ntsa::Error hideNotifications(ntsa::Handle handle) BSLS_KEYWORD_OVERRIDE;

    /// Stop monitoring the specified 'socket' and close the
    /// 'socket' if it is not already closed. Return the error.
    ntsa::Error detachSocket(const bsl::shared_ptr<ntci::ReactorSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;

    /// Stop monitoring the specified 'socket'. Invoke the specified 'callback'
    /// when the socket is detached. Return the error.
    ntsa::Error detachSocket(
        const bsl::shared_ptr<ntci::ReactorSocket>& socket,
        const ntci::SocketDetachedCallback& callback) BSLS_KEYWORD_OVERRIDE;

    /// Stop monitoring the specified socket 'handle'. Return the error.
    ntsa::Error detachSocket(ntsa::Handle handle) BSLS_KEYWORD_OVERRIDE;

    /// Stop monitoring the specified 'handle'. Invoke the specified 'callback'
    /// when the socket is detached. Return the error.
    ntsa::Error detachSocket(ntsa::Handle                        handle,
                             const ntci::SocketDetachedCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Close all monitored sockets and timers.
    ntsa::Error closeAll() BSLS_KEYWORD_OVERRIDE;

    /// Increment the estimation of the load on the reactor according to
    /// the specified load balancing 'options'.
    void incrementLoad(const ntca::LoadBalancingOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Decrement the estimation of the load on the reactor according to
    /// the specified load balancing 'options'.
    void decrementLoad(const ntca::LoadBalancingOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Block the calling thread until stopped. As each operation completes,
    /// invoke the corresponding processing function on the associated
    /// socket. The behavior is undefined unless the calling thread has
    /// previously registered the 'waiter'. Note that after this function
    /// returns, the 'restart' function must be called before this or the
    /// 'run' function can be called again.
    void run(ntci::Waiter waiter) BSLS_KEYWORD_OVERRIDE;

    /// Block the calling thread identified by the specified 'waiter', until
    /// at least one socket enters the state in which interest has been
    /// registered, or timer fires. For each socket that has entered the
    /// state in which interest has been registered, or each timer that has
    /// fired, invoke the corresponding processing function on the
    /// associated socket or timer. The behavior is undefined unless the
    /// calling thread has previously registered the 'waiter'. Note that
    /// if this function returns because 'stop' was called, the 'restart'
    /// function must be called before this or the 'run' function can be
    /// called again.
    void poll(ntci::Waiter waiter) BSLS_KEYWORD_OVERRIDE;

    /// Unblock and return one caller blocked on either 'poll' or 'run'.
    void interruptOne() BSLS_KEYWORD_OVERRIDE;

    /// Unblock and return any caller blocked on either 'poll' or 'run'.
    void interruptAll() BSLS_KEYWORD_OVERRIDE;

    /// Unblock and return any caller blocked on either 'poll' or 'run'.
    void stop() BSLS_KEYWORD_OVERRIDE;

    /// Prepare the reactor for 'run' to be called again after previously
    /// being stopped.
    void restart() BSLS_KEYWORD_OVERRIDE;

    /// Execute all deferred functions managed by this object.
    void drainFunctions() BSLS_KEYWORD_OVERRIDE;

    /// Clear all deferred functions managed by this object.
    void clearFunctions() BSLS_KEYWORD_OVERRIDE;

    /// Clear all timers managed by this object.
    void clearTimers() BSLS_KEYWORD_OVERRIDE;

    /// Clear all sockets managed by this object.
    void clearSockets() BSLS_KEYWORD_OVERRIDE;

    /// Clear all resources managed by this object.
    void clear() BSLS_KEYWORD_OVERRIDE;

    /// Defer the execution of the specified 'functor'.
    void execute(const Functor& functor) BSLS_KEYWORD_OVERRIDE;

    /// Atomically defer the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE;

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

    /// Create a new datagram socket with the specified 'options'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    bsl::shared_ptr<ntci::DatagramSocket> createDatagramSocket(
        const ntca::DatagramSocketOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new listener socket with the specified 'options'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    bsl::shared_ptr<ntci::ListenerSocket> createListenerSocket(
        const ntca::ListenerSocketOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new stream socket with the specified 'options'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    bsl::shared_ptr<ntci::StreamSocket> createStreamSocket(
        const ntca::StreamSocketOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

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

    /// Return the number of descriptors currently being monitored.
    bsl::size_t numSockets() const BSLS_KEYWORD_OVERRIDE;

    /// Return the maximum number of descriptors capable of being monitored
    /// at one time.
    bsl::size_t maxSockets() const BSLS_KEYWORD_OVERRIDE;

    /// Return the number of timers currently being monitored.
    bsl::size_t numTimers() const BSLS_KEYWORD_OVERRIDE;

    /// Return the maximum number of timers capable of being monitored at
    /// one time.
    bsl::size_t maxTimers() const BSLS_KEYWORD_OVERRIDE;

    /// Return the flag that indicates a socket should be automatically
    /// attached to the reactor when interest in any event for a sodcket is
    /// gained.
    bool autoAttach() const BSLS_KEYWORD_OVERRIDE;

    /// Return the flag that indicates a socket should be automatically
    /// detached from the reactor when interest in all events for the socket
    /// is lost.
    bool autoDetach() const BSLS_KEYWORD_OVERRIDE;

    /// Return the one-shot mode of the delivery of events. When one-shot
    /// mode is enabled, after a reactor detects the socket is readable or
    /// writable, interest in readability or writability must be explicitly
    /// registered before the reactor will again detect the socket is
    /// readable or writable.
    bool oneShot() const BSLS_KEYWORD_OVERRIDE;

    /// Return the default trigger mode of the detection of events. When
    /// events are level-triggered, the event will occur as long as the
    /// conditions for the event continue to be satisfied. When events are
    /// edge-triggered, the event is raised when conditions for the event
    /// change are first satisfied, but the event is not subsequently raised
    /// until the conditions are "reset".
    ntca::ReactorEventTrigger::Value trigger() const BSLS_KEYWORD_OVERRIDE;

    /// Return the estimation of the load on the reactor.
    bsl::size_t load() const BSLS_KEYWORD_OVERRIDE;

    /// Return the handle of the thread that drives this reactor, or
    /// the default value if no such thread has been set.
    bslmt::ThreadUtil::Handle threadHandle() const BSLS_KEYWORD_OVERRIDE;

    /// Return the index in the thread pool of the thread that drives this
    /// reactor, or 0 if no such thread has been set.
    bsl::size_t threadIndex() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current number of registered waiters.
    bsl::size_t numWaiters() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if the reactor has no pending deferred functors no
    /// pending timers, and no registered sockets, otherwise return false.
    bool empty() const BSLS_KEYWORD_OVERRIDE;

    const bsl::shared_ptr<ntci::DataPool>& dataPool() const
        BSLS_KEYWORD_OVERRIDE;
    // Return the data pool.

    /// Return true if the reactor supports registering events in the
    /// specified 'oneShot' mode, otherwise return false.
    bool supportsOneShot(bool oneShot) const BSLS_KEYWORD_OVERRIDE;

    /// Return true if the reactor supports registering events having the
    /// specified 'trigger', otherwise return false.
    bool supportsTrigger(ntca::ReactorEventTrigger::Value trigger) const
        BSLS_KEYWORD_OVERRIDE;

    /// Always return true indicating that the reactor supports notification
    /// mechanism (polling of a socket error queue).
    bool supportsNotifications() const BSLS_KEYWORD_OVERRIDE;

    /// Return the strand that guarantees sequential, non-current execution
    /// of arbitrary functors on the unspecified threads processing events
    /// for this object.
    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current elapsed time since the Unix epoch.
    bsls::TimeInterval currentTime() const BSLS_KEYWORD_OVERRIDE;

    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& incomingBlobBufferFactory()
        const BSLS_KEYWORD_OVERRIDE;
    // Return the incoming blob buffer factory.

    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& outgoingBlobBufferFactory()
        const BSLS_KEYWORD_OVERRIDE;
    // Return the outgoing blob buffer factory.

    /// Return the name of the driver.
    const char* name() const BSLS_KEYWORD_OVERRIDE;
};

/// This struct describes the context of a waiter.
struct Poll::Result {
  public:
    /// This typedef defines vector of poll descriptions.
    typedef bsl::vector<struct ::pollfd> DescriptorList;

    ntca::WaiterOptions                         d_options;
    bsl::shared_ptr<ntci::ReactorMetrics>       d_metrics_sp;
    bsls::AtomicUint64                          d_generation;
    DescriptorList                              d_descriptorList;
    ntcs::RegistryEntryCatalog::ForEachCallback d_forEachCallback;
    bool                                        d_controllerHandleFound;
    bsl::size_t                                 d_controllerHandleIdx;

  private:
    Result(const Result&) BSLS_KEYWORD_DELETED;
    Result& operator=(const Result&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new reactor result. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit Result(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Result();
};

Poll::Result::Result(bslma::Allocator* basicAllocator)
: d_options(basicAllocator)
, d_metrics_sp()
, d_generation(0)
, d_descriptorList(basicAllocator)
, d_forEachCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_controllerHandleFound(0)
, d_controllerHandleIdx(0)
{
}

Poll::Result::~Result()
{
}

NTCCFG_INLINE
void Poll::specify(struct ::pollfd* result,
                   ntsa::Handle     handle,
                   ntcs::Interest   interest)
{
    short events = 0;

    if (interest.wantReadable()) {
        events |= POLLIN;
    }

    if (interest.wantWritable()) {
        events |= POLLOUT;
    }

    result->fd     = handle;
    result->events = events;
}

void Poll::flush()
{
    while (true) {
        {
            LockGuard detachGuard(&d_detachMutex);
            for (DetachList::const_iterator it = d_detachList.cbegin();
                 it != d_detachList.cend();
                 ++it)
            {
                ntcs::RegistryEntry& entry = **it;
                entry.announceDetached(this->getSelf(this));
                entry.clear();
            }
            d_detachList.clear();
        }

        if (d_chronology.hasAnyScheduledOrDeferred()) {
            d_chronology.announce(d_dynamic);
        }

        {
            LockGuard detachGuard(&d_detachMutex);
            if (!d_chronology.hasAnyDeferred() && d_detachList.empty()) {
                break;
            }
        }
    }
}

NTCCFG_INLINE
ntsa::Error Poll::add(ntsa::Handle handle, ntcs::Interest interest)
{
    NTCCFG_WARNING_UNUSED(handle);
    NTCCFG_WARNING_UNUSED(interest);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(handle);

    NTCO_POLL_LOG_ADD(handle, interest);

    ++d_generation;

    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error Poll::update(ntsa::Handle   handle,
                         ntcs::Interest interest,
                         UpdateType     type)
{
    NTCCFG_WARNING_UNUSED(handle);
    NTCCFG_WARNING_UNUSED(interest);
    NTCCFG_WARNING_UNUSED(type);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(handle);

    NTCO_POLL_LOG_UPDATE(handle, interest);

    ++d_generation;

    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error Poll::remove(ntsa::Handle handle)
{
    NTCCFG_WARNING_UNUSED(handle);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(handle);

    NTCO_POLL_LOG_REMOVE(handle);

    ++d_generation;

    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error Poll::removeDetached(
    const bsl::shared_ptr<ntcs::RegistryEntry>& entry)
{
    ntsa::Handle handle = entry->handle();
    NTCCFG_WARNING_UNUSED(handle);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(handle);

    NTCO_POLL_LOG_REMOVE(handle);

    ++d_generation;

    {
        LockGuard lock(&d_detachMutex);
        d_detachList.push_back(entry);
    }
    Poll::interruptOne();

    return ntsa::Error();
}

void Poll::link(const bsl::shared_ptr<ntcs::RegistryEntry>& entry,
                Poll::Result*                               result)
{
    ntsa::Handle   handle   = entry->handle();
    ntcs::Interest interest = entry->interest();

    struct ::pollfd pfd;
    Poll::specify(&pfd, handle, interest);
    result->d_descriptorList.push_back(pfd);

    if (!result->d_controllerHandleFound) {
        if (handle == d_controllerDescriptorHandle) {
            result->d_controllerHandleFound = true;
        }
        else {
            result->d_controllerHandleIdx++;
        }
    }
}

void Poll::reinitializeControl()
{
    if (d_controller_sp) {
        bsl::shared_ptr<ntcs::RegistryEntry> entry =
            d_registry.remove(d_controller_sp);
        if (entry) {
            this->remove(entry->handle());
        }
        d_controller_sp.reset();
    }

    d_controller_sp.createInplace(d_allocator_p);

    bsl::shared_ptr<ntcs::RegistryEntry> entry =
        d_registry.add(d_controller_sp);

    d_controllerDescriptorHandle = entry->handle();

    ntca::ReactorEventOptions options;

    entry->showReadable(options);
    this->add(entry->handle(), entry->interest());
}

void Poll::deinitializeControl()
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry =
        d_registry.remove(d_controller_sp);
    if (entry) {
        this->remove(entry->handle());
    }

    d_controller_sp.reset();
}

NTCCFG_INLINE
bool Poll::isWaiter()
{
    return bslmt::ThreadUtil::selfIdAsUint64() == d_threadId.load();
}

bsl::shared_ptr<ntci::Reactor> Poll::acquireReactor(
    const ntca::LoadBalancingOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);
    return this->getSelf(this);
}

void Poll::releaseReactor(const bsl::shared_ptr<ntci::Reactor>& reactor,
                          const ntca::LoadBalancingOptions&     options)
{
    BSLS_ASSERT(reactor == this->getSelf(this));
    reactor->decrementLoad(options);
}

bool Poll::acquireHandleReservation()
{
    if (d_connectionLimiter_sp) {
        return d_connectionLimiter_sp->acquire();
    }
    else {
        return true;
    }
}

void Poll::releaseHandleReservation()
{
    if (d_connectionLimiter_sp) {
        d_connectionLimiter_sp->release();
    }
}

bsl::size_t Poll::numReactors() const
{
    return 1;
}

bsl::size_t Poll::numThreads() const
{
    return this->numWaiters();
}

bsl::size_t Poll::minThreads() const
{
    return d_config.minThreads().value();
}

bsl::size_t Poll::maxThreads() const
{
    return d_config.maxThreads().value();
}

Poll::Poll(const ntca::ReactorConfig&         configuration,
           const bsl::shared_ptr<ntci::User>& user,
           bslma::Allocator*                  basicAllocator)
: d_object("ntco::Poll")
, d_generationMutex()
, d_generationSemaphore()
, d_generation(1)
, d_detachMutex()
, d_detachList(basicAllocator)
#if NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMDAS
, d_detachFunctor([this](const auto& entry) {
    return this->removeDetached(entry);
})
#else
, d_detachFunctor(
      NTCCFG_BIND(&Poll::removeDetached, this, NTCCFG_BIND_PLACEHOLDER_1))
#endif
, d_registry(basicAllocator)
, d_chronology(this, basicAllocator)
, d_user_sp(user)
, d_dataPool_sp()
, d_resolver_sp()
, d_connectionLimiter_sp()
, d_metrics_sp()
, d_controller_sp()
, d_controllerDescriptorHandle(ntsa::k_INVALID_HANDLE)
, d_waiterSetMutex()
, d_waiterSet(basicAllocator)
, d_threadHandle(bslmt::ThreadUtil::invalidHandle())
, d_threadIndex(0)
, d_threadId(0)
, d_dynamic(false)
, d_load(0)
, d_run(true)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_config.metricName().isNull() ||
        d_config.metricName().value().empty())
    {
        d_config.setMetricName(ntcs::Nomenclature::createReactorName());
    }

    BSLS_ASSERT(!d_config.metricName().isNull());
    BSLS_ASSERT(!d_config.metricName().value().empty());

    if (d_config.minThreads().isNull() || d_config.minThreads().value() == 0) {
        d_config.setMinThreads(1);
    }

    BSLS_ASSERT(!d_config.minThreads().isNull());
    BSLS_ASSERT(d_config.minThreads().value() > 0);

    if (d_config.maxThreads().isNull() || d_config.maxThreads().value() == 0) {
        d_config.setMaxThreads(NTCCFG_DEFAULT_MAX_THREADS);
    }

    BSLS_ASSERT(!d_config.maxThreads().isNull());
    BSLS_ASSERT(d_config.maxThreads().value() > 0);

    if (d_config.maxThreads().value() > NTCCFG_DEFAULT_MAX_THREADS) {
        d_config.setMaxThreads(NTCCFG_DEFAULT_MAX_THREADS);
    }

    if (d_config.minThreads().value() > d_config.maxThreads().value()) {
        d_config.setMinThreads(d_config.maxThreads().value());
    }

    if (d_config.maxThreads().value() > 1) {
        d_dynamic = true;
    }

    BSLS_ASSERT(d_config.minThreads().value() <=
                d_config.maxThreads().value());
    BSLS_ASSERT(d_config.maxThreads().value() <= NTCCFG_DEFAULT_MAX_THREADS);

    if (d_config.maxEventsPerWait().isNull()) {
        d_config.setMaxEventsPerWait(NTCCFG_DEFAULT_MAX_EVENTS_PER_WAIT);
    }

    if (d_config.maxTimersPerWait().isNull()) {
        d_config.setMaxTimersPerWait(NTCCFG_DEFAULT_MAX_TIMERS_PER_WAIT);
    }

    if (d_config.maxCyclesPerWait().isNull()) {
        d_config.setMaxCyclesPerWait(NTCCFG_DEFAULT_MAX_CYCLES_PER_WAIT);
    }

    if (d_config.metricCollection().isNull()) {
        d_config.setMetricCollection(NTCCFG_DEFAULT_DRIVER_METRICS);
    }

    if (d_config.metricCollectionPerWaiter().isNull()) {
        d_config.setMetricCollectionPerWaiter(
            NTCCFG_DEFAULT_DRIVER_METRICS_PER_WAITER);
    }

    if (d_config.metricCollectionPerSocket().isNull()) {
        d_config.setMetricCollectionPerSocket(false);
    }

    if (d_config.autoAttach().isNull()) {
        d_config.setAutoAttach(false);
    }

    if (d_config.autoDetach().isNull()) {
        d_config.setAutoDetach(false);
    }

    if (d_config.oneShot().isNull()) {
        if (d_config.maxThreads().value() == 1) {
            d_config.setOneShot(false);
        }
        else {
            d_config.setOneShot(true);
        }
    }

    if (d_config.trigger().isNull()) {
        d_config.setTrigger(ntca::ReactorEventTrigger::e_LEVEL);
    }

    if (d_user_sp) {
        d_dataPool_sp = d_user_sp->dataPool();
    }

    if (!d_dataPool_sp) {
        bsl::shared_ptr<ntcs::DataPool> dataPool;
        dataPool.createInplace(d_allocator_p, d_allocator_p);

        d_dataPool_sp = dataPool;
    }

    if (d_user_sp) {
        d_resolver_sp = d_user_sp->resolver();
    }

    if (!d_resolver_sp) {
        // MRM: Consider implementing a resolver compatible with this object's
        // interface, namely, that it does not support a 'close' or 'shutdown'
        // and 'linger' idiom.
        //
        // bsl::shared_ptr<ntcdns::Resolver> resolver;
        // resolver.createInplace(
        //     d_allocator_p, ntca::ResolverConfig(), d_allocator_p);
        //
        // resolver->start();
        //
        // d_resolver_sp = resolver;
    }

    if (d_user_sp) {
        d_connectionLimiter_sp = d_user_sp->connectionLimiter();
    }

    if (d_user_sp) {
        d_metrics_sp = d_user_sp->reactorMetrics();
    }

    if (d_user_sp) {
        bsl::shared_ptr<ntci::Chronology> chronology = d_user_sp->chronology();
        if (chronology) {
            d_chronology.setParent(chronology);
        }
    }

    d_registry.setDefaultTrigger(d_config.trigger().value());
    d_registry.setDefaultOneShot(d_config.oneShot().value());

    if (d_config.maxThreads().value() > 1) {
        d_generationSemaphore.post();
    }

    this->reinitializeControl();
}

Poll::~Poll()
{
    // Assert all timers and functions are executed.

    BSLS_ASSERT_OPT(!d_chronology.hasAnyDeferred());
    BSLS_ASSERT_OPT(!d_chronology.hasAnyScheduled());
    BSLS_ASSERT_OPT(!d_chronology.hasAnyRegistered());

    // Assert all waiters are deregistered.

    BSLS_ASSERT_OPT(d_waiterSet.empty());

    this->deinitializeControl();
}

ntci::Waiter Poll::registerWaiter(const ntca::WaiterOptions& waiterOptions)
{
    Poll::Result* result = new (*d_allocator_p) Poll::Result(d_allocator_p);

    result->d_options = waiterOptions;

    bdlb::NullableValue<bslmt::ThreadUtil::Handle> principleThreadHandle;

    result->d_forEachCallback =
        NTCCFG_BIND(&Poll::link, this, NTCCFG_BIND_PLACEHOLDER_1, result);

    {
        LockGuard lockGuard(&d_waiterSetMutex);

        if (result->d_options.threadHandle() == bslmt::ThreadUtil::Handle()) {
            result->d_options.setThreadHandle(bslmt::ThreadUtil::self());
        }

        if (d_waiterSet.empty()) {
            d_threadHandle = result->d_options.threadHandle();
            principleThreadHandle.makeValue(d_threadHandle);

            if (!result->d_options.threadIndex().isNull()) {
                d_threadIndex = result->d_options.threadIndex().value();
            }
        }

        if (d_config.metricCollection().value()) {
            if (d_config.metricCollectionPerWaiter().value()) {
                if (result->d_options.metricName().empty()) {
                    bsl::stringstream ss;
                    ss << d_config.metricName().value() << "-"
                       << d_waiterSet.size();
                    result->d_options.setMetricName(ss.str());
                }

                bsl::shared_ptr<ntcs::ReactorMetrics> metrics;
                metrics.createInplace(d_allocator_p,
                                      "thread",
                                      result->d_options.metricName(),
                                      d_metrics_sp,
                                      d_allocator_p);

                result->d_metrics_sp = metrics;

                ntcm::MonitorableUtil::registerMonitorable(
                    result->d_metrics_sp);
            }
            else {
                result->d_metrics_sp = d_metrics_sp;
            }
        }

        d_waiterSet.insert(result);
    }

    if (!principleThreadHandle.isNull()) {
        d_threadId.store(bslmt::ThreadUtil::idAsUint64(
            bslmt::ThreadUtil::handleToId(principleThreadHandle.value())));
    }

    return result;
}

void Poll::deregisterWaiter(ntci::Waiter waiter)
{
    Poll::Result* result = static_cast<Poll::Result*>(waiter);

    bool nowEmpty = false;

    {
        LockGuard lockGuard(&d_waiterSetMutex);

        bsl::size_t n = d_waiterSet.erase(result);
        BSLS_ASSERT_OPT(n == 1);

        if (d_waiterSet.empty()) {
            d_threadHandle = bslmt::ThreadUtil::invalidHandle();
            nowEmpty       = true;
        }
    }

    if (nowEmpty) {
        this->flush();
        d_threadId.store(0);
    }

    if (d_config.metricCollection().value()) {
        if (d_config.metricCollectionPerWaiter().value()) {
            ntcm::MonitorableUtil::deregisterMonitorable(result->d_metrics_sp);
        }
    }

    d_allocator_p->deleteObject(result);
}

bsl::shared_ptr<ntci::Strand> Poll::createStrand(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<Reactor> self = this->getSelf(this);

    bsl::shared_ptr<ntcs::Strand> strand;
    strand.createInplace(allocator, self, allocator);

    return strand;
}

ntsa::Error Poll::attachSocket(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry = d_registry.add(socket);
    return this->add(entry->handle(), entry->interest());
}

ntsa::Error Poll::attachSocket(ntsa::Handle handle)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry = d_registry.add(handle);
    return this->add(handle, entry->interest());
}

ntsa::Error Poll::showReadable(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket,
    const ntca::ReactorEventOptions&            options)
{
    ntsa::Error error;

    if (!options.trigger().isNull()) {
        if (options.trigger().value() == ntca::ReactorEventTrigger::e_EDGE) {
            if (!this->supportsTrigger(options.trigger().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    if (!options.oneShot().isNull()) {
        if (options.oneShot().value()) {
            if (!this->supportsOneShot(options.oneShot().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    bsl::shared_ptr<ntcs::RegistryEntry> entry =
        bslstl::SharedPtrUtil::staticCast<ntcs::RegistryEntry>(
            socket->getReactorContext());

    if (NTCCFG_LIKELY(entry)) {
        ntcs::Interest interest = entry->showReadable(options);

        error = this->update(entry->handle(), interest, e_INCLUDE);
        if (error) {
            return error;
        }
        if (NTCRO_POLL_INTERRUPT_ALL) {
            Poll::interruptAll();
        }
        return ntsa::Error();
    }
    else {
        if (d_config.autoAttach().value()) {
            entry = d_registry.add(socket);

            ntcs::Interest interest = entry->showReadable(options);

            error = this->add(entry->handle(), interest);
            if (error) {
                return error;
            }
            if (NTCRO_POLL_INTERRUPT_ALL) {
                Poll::interruptAll();
            }
            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

ntsa::Error Poll::showReadable(ntsa::Handle                      handle,
                               const ntca::ReactorEventOptions&  options,
                               const ntci::ReactorEventCallback& callback)
{
    ntsa::Error error;

    if (!options.trigger().isNull()) {
        if (options.trigger().value() == ntca::ReactorEventTrigger::e_EDGE) {
            if (!this->supportsTrigger(options.trigger().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    if (!options.oneShot().isNull()) {
        if (options.oneShot().value()) {
            if (!this->supportsOneShot(options.oneShot().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    bsl::shared_ptr<ntcs::RegistryEntry> entry;
    bool found = d_registry.lookup(&entry, handle);
    if (NTCCFG_LIKELY(found)) {
        ntcs::Interest interest =
            entry->showReadableCallback(options, callback);

        error = this->update(handle, interest, e_INCLUDE);
        if (error) {
            return error;
        }

        if (NTCRO_POLL_INTERRUPT_ALL) {
            Poll::interruptAll();
        }

        return ntsa::Error();
    }
    else {
        if (d_config.autoAttach().value()) {
            entry = d_registry.add(handle);

            ntcs::Interest interest =
                entry->showReadableCallback(options, callback);

            error = this->add(handle, interest);
            if (error) {
                return error;
            }

            if (NTCRO_POLL_INTERRUPT_ALL) {
                Poll::interruptAll();
            }

            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

ntsa::Error Poll::showWritable(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket,
    const ntca::ReactorEventOptions&            options)
{
    ntsa::Error error;

    if (!options.trigger().isNull()) {
        if (options.trigger().value() == ntca::ReactorEventTrigger::e_EDGE) {
            if (!this->supportsTrigger(options.trigger().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    if (!options.oneShot().isNull()) {
        if (options.oneShot().value()) {
            if (!this->supportsOneShot(options.oneShot().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    bsl::shared_ptr<ntcs::RegistryEntry> entry =
        bslstl::SharedPtrUtil::staticCast<ntcs::RegistryEntry>(
            socket->getReactorContext());

    if (NTCCFG_LIKELY(entry)) {
        ntcs::Interest interest = entry->showWritable(options);

        error = this->update(entry->handle(), interest, e_INCLUDE);
        if (error) {
            return error;
        }

        if (NTCRO_POLL_INTERRUPT_ALL) {
            Poll::interruptAll();
        }
        return ntsa::Error();
    }
    else {
        if (d_config.autoAttach().value()) {
            entry = d_registry.add(socket);

            ntcs::Interest interest = entry->showWritable(options);

            error = this->add(entry->handle(), interest);
            if (error) {
                return error;
            }

            if (NTCRO_POLL_INTERRUPT_ALL) {
                Poll::interruptAll();
            }
            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

ntsa::Error Poll::showWritable(ntsa::Handle                      handle,
                               const ntca::ReactorEventOptions&  options,
                               const ntci::ReactorEventCallback& callback)
{
    ntsa::Error error;

    if (!options.trigger().isNull()) {
        if (options.trigger().value() == ntca::ReactorEventTrigger::e_EDGE) {
            if (!this->supportsTrigger(options.trigger().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    if (!options.oneShot().isNull()) {
        if (options.oneShot().value()) {
            if (!this->supportsOneShot(options.oneShot().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    bsl::shared_ptr<ntcs::RegistryEntry> entry;
    bool found = d_registry.lookup(&entry, handle);
    if (NTCCFG_LIKELY(found)) {
        ntcs::Interest interest =
            entry->showWritableCallback(options, callback);

        error = this->update(handle, interest, e_INCLUDE);
        if (error) {
            return error;
        }

        if (NTCRO_POLL_INTERRUPT_ALL) {
            Poll::interruptAll();
        }
        return ntsa::Error();
    }
    else {
        if (d_config.autoAttach().value()) {
            entry = d_registry.add(handle);

            ntcs::Interest interest =
                entry->showWritableCallback(options, callback);

            error = this->add(handle, interest);
            if (error) {
                return error;
            }

            if (NTCRO_POLL_INTERRUPT_ALL) {
                Poll::interruptAll();
            }

            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

ntsa::Error Poll::showError(const bsl::shared_ptr<ntci::ReactorSocket>& socket,
                            const ntca::ReactorEventOptions& options)
{
    ntsa::Error error;

    if (!options.trigger().isNull()) {
        if (options.trigger().value() == ntca::ReactorEventTrigger::e_EDGE) {
            if (!this->supportsTrigger(options.trigger().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    if (!options.oneShot().isNull()) {
        if (options.oneShot().value()) {
            if (!this->supportsOneShot(options.oneShot().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    bsl::shared_ptr<ntcs::RegistryEntry> entry =
        bslstl::SharedPtrUtil::staticCast<ntcs::RegistryEntry>(
            socket->getReactorContext());

    if (NTCCFG_LIKELY(entry)) {
        ntcs::Interest interest = entry->showError(options);

        error = this->update(entry->handle(), interest, e_INCLUDE);
        if (error) {
            return error;
        }

        if (NTCRO_POLL_INTERRUPT_ALL) {
            Poll::interruptAll();
        }
        return ntsa::Error();
    }
    else {
        if (d_config.autoAttach().value()) {
            entry = d_registry.add(socket);

            ntcs::Interest interest = entry->showError(options);

            error = this->add(entry->handle(), interest);
            if (error) {
                return error;
            }

            if (NTCRO_POLL_INTERRUPT_ALL) {
                Poll::interruptAll();
            }

            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

ntsa::Error Poll::showError(ntsa::Handle                      handle,
                            const ntca::ReactorEventOptions&  options,
                            const ntci::ReactorEventCallback& callback)
{
    ntsa::Error error;

    if (!options.trigger().isNull()) {
        if (options.trigger().value() == ntca::ReactorEventTrigger::e_EDGE) {
            if (!this->supportsTrigger(options.trigger().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    if (!options.oneShot().isNull()) {
        if (options.oneShot().value()) {
            if (!this->supportsOneShot(options.oneShot().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    bsl::shared_ptr<ntcs::RegistryEntry> entry;
    bool found = d_registry.lookup(&entry, handle);
    if (NTCCFG_LIKELY(found)) {
        ntcs::Interest interest = entry->showErrorCallback(options, callback);

        error = this->update(handle, interest, e_INCLUDE);
        if (error) {
            return error;
        }

        if (NTCRO_POLL_INTERRUPT_ALL) {
            Poll::interruptAll();
        }

        return ntsa::Error();
    }
    else {
        if (d_config.autoAttach().value()) {
            entry = d_registry.add(handle);
            ntcs::Interest interest =
                entry->showErrorCallback(options, callback);

            error = this->add(handle, interest);
            if (error) {
                return error;
            }

            if (NTCRO_POLL_INTERRUPT_ALL) {
                Poll::interruptAll();
            }

            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

ntsa::Error Poll::showNotifications(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket)
{
    ntsa::Error error;

    bsl::shared_ptr<ntcs::RegistryEntry> entry =
        bslstl::SharedPtrUtil::staticCast<ntcs::RegistryEntry>(
            socket->getReactorContext());

    if (NTCCFG_LIKELY(entry)) {
        ntcs::Interest interest = entry->showNotifications();

        error = this->update(entry->handle(), interest, e_INCLUDE);
        if (error) {
            return error;
        }

        if (NTCRO_POLL_INTERRUPT_ALL) {
            Poll::interruptAll();
        }
        return ntsa::Error();
    }
    else {
        if (d_config.autoAttach().value()) {
            entry = d_registry.add(socket);

            ntcs::Interest interest = entry->showNotifications();

            error = this->add(entry->handle(), interest);
            if (error) {
                return error;
            }

            if (NTCRO_POLL_INTERRUPT_ALL) {
                Poll::interruptAll();
            }

            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

ntsa::Error Poll::showNotifications(
    ntsa::Handle                             handle,
    const ntci::ReactorNotificationCallback& callback)
{
    ntsa::Error error;

    bsl::shared_ptr<ntcs::RegistryEntry> entry;
    bool found = d_registry.lookup(&entry, handle);
    if (NTCCFG_LIKELY(found)) {
        ntcs::Interest interest = entry->showNotificationsCallback(callback);

        error = this->update(handle, interest, e_INCLUDE);
        if (error) {
            return error;
        }

        if (NTCRO_POLL_INTERRUPT_ALL) {
            Poll::interruptAll();
        }
        return ntsa::Error();
    }
    else {
        if (d_config.autoAttach().value()) {
            entry = d_registry.add(handle);

            ntcs::Interest interest =
                entry->showNotificationsCallback(callback);

            error = this->add(handle, interest);
            if (error) {
                return error;
            }

            if (NTCRO_POLL_INTERRUPT_ALL) {
                Poll::interruptAll();
            }

            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

ntsa::Error Poll::hideReadable(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry =
        bslstl::SharedPtrUtil::staticCast<ntcs::RegistryEntry>(
            socket->getReactorContext());

    if (NTCCFG_LIKELY(entry)) {
        ntca::ReactorEventOptions options;
        ntcs::Interest            interest = entry->hideReadable(options);
        if (!d_config.autoDetach().value()) {
            return this->update(entry->handle(), interest, e_EXCLUDE);
        }
        else {
            if (interest.wantReadableOrWritable()) {
                return this->update(entry->handle(), interest, e_EXCLUDE);
            }
            else {
                d_registry.remove(socket);
                return this->remove(entry->handle());
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error Poll::hideReadable(ntsa::Handle handle)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry;
    bool found = d_registry.lookup(&entry, handle);
    if (NTCCFG_LIKELY(found)) {
        ntca::ReactorEventOptions options;
        ntcs::Interest interest = entry->hideReadableCallback(options);
        if (!d_config.autoDetach().value()) {
            return this->update(handle, interest, e_EXCLUDE);
        }
        else {
            if (interest.wantReadableOrWritable()) {
                return this->update(handle, interest, e_EXCLUDE);
            }
            else {
                d_registry.remove(handle);
                return this->remove(handle);
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error Poll::hideWritable(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry =
        bslstl::SharedPtrUtil::staticCast<ntcs::RegistryEntry>(
            socket->getReactorContext());

    if (NTCCFG_LIKELY(entry)) {
        ntca::ReactorEventOptions options;
        ntcs::Interest            interest = entry->hideWritable(options);
        if (!d_config.autoDetach().value()) {
            return this->update(entry->handle(), interest, e_EXCLUDE);
        }
        else {
            if (interest.wantReadableOrWritable()) {
                return this->update(entry->handle(), interest, e_EXCLUDE);
            }
            else {
                d_registry.remove(socket);
                return this->remove(entry->handle());
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error Poll::hideWritable(ntsa::Handle handle)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry;
    bool found = d_registry.lookup(&entry, handle);
    if (NTCCFG_LIKELY(found)) {
        ntca::ReactorEventOptions options;
        ntcs::Interest interest = entry->hideWritableCallback(options);
        if (!d_config.autoDetach().value()) {
            return this->update(handle, interest, e_EXCLUDE);
        }
        else {
            if (interest.wantReadableOrWritable()) {
                return this->update(handle, interest, e_EXCLUDE);
            }
            else {
                d_registry.remove(handle);
                return this->remove(handle);
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error Poll::hideError(const bsl::shared_ptr<ntci::ReactorSocket>& socket)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry =
        bslstl::SharedPtrUtil::staticCast<ntcs::RegistryEntry>(
            socket->getReactorContext());

    if (NTCCFG_LIKELY(entry)) {
        ntca::ReactorEventOptions options;
        ntcs::Interest            interest = entry->hideError(options);
        if (!d_config.autoDetach().value()) {
            return this->update(entry->handle(), interest, e_EXCLUDE);
        }
        else {
            if (interest.wantReadableOrWritable()) {
                return this->update(entry->handle(), interest, e_EXCLUDE);
            }
            else {
                d_registry.remove(socket);
                return this->remove(entry->handle());
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error Poll::hideError(ntsa::Handle handle)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry;
    bool found = d_registry.lookup(&entry, handle);
    if (NTCCFG_LIKELY(found)) {
        ntca::ReactorEventOptions options;
        ntcs::Interest            interest = entry->hideErrorCallback(options);
        if (!d_config.autoDetach().value()) {
            return this->update(handle, interest, e_EXCLUDE);
        }
        else {
            if (interest.wantReadableOrWritable()) {
                return this->update(handle, interest, e_EXCLUDE);
            }
            else {
                d_registry.remove(handle);
                return this->remove(handle);
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error Poll::hideNotifications(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry =
        bslstl::SharedPtrUtil::staticCast<ntcs::RegistryEntry>(
            socket->getReactorContext());

    if (NTCCFG_LIKELY(entry)) {
        ntcs::Interest interest = entry->hideNotifications();
        if (!d_config.autoDetach().value()) {
            return this->update(entry->handle(), interest, e_EXCLUDE);
        }
        else {
            if (interest.wantReadableOrWritable()) {
                return this->update(entry->handle(), interest, e_EXCLUDE);
            }
            else {
                d_registry.remove(socket);
                return this->remove(entry->handle());
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error Poll::hideNotifications(ntsa::Handle handle)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry;
    bool found = d_registry.lookup(&entry, handle);
    if (NTCCFG_LIKELY(found)) {
        ntcs::Interest interest = entry->hideNotifications();
        if (!d_config.autoDetach().value()) {
            return this->update(handle, interest, e_EXCLUDE);
        }
        else {
            if (interest.wantReadableOrWritable()) {
                return this->update(handle, interest, e_EXCLUDE);
            }
            else {
                d_registry.remove(handle);
                return this->remove(handle);
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error Poll::detachSocket(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket)
{
    return detachSocket(socket, ntci::SocketDetachedCallback());
}

ntsa::Error Poll::detachSocket(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket,
    const ntci::SocketDetachedCallback&         callback)
{
    const ntsa::Error error =
        d_registry.removeAndGetReadyToDetach(socket,
                                             callback,
                                             d_detachFunctor);
    return error;
}

ntsa::Error Poll::detachSocket(ntsa::Handle handle)
{
    return detachSocket(handle, ntci::SocketDetachedCallback());
}

ntsa::Error Poll::detachSocket(ntsa::Handle                        handle,
                               const ntci::SocketDetachedCallback& callback)
{
    const ntsa::Error error =
        d_registry.removeAndGetReadyToDetach(handle,
                                             callback,
                                             d_detachFunctor);
    return error;
}

ntsa::Error Poll::closeAll()
{
    d_chronology.closeAll();
    d_registry.closeAll(d_controllerDescriptorHandle);
    return ntsa::Error();
}

void Poll::incrementLoad(const ntca::LoadBalancingOptions& options)
{
    bsl::size_t weight =
        options.weight().isNull() ? 1 : options.weight().value();

    d_load += weight;
}

void Poll::decrementLoad(const ntca::LoadBalancingOptions& options)
{
    bsl::size_t weight =
        options.weight().isNull() ? 1 : options.weight().value();

    d_load -= weight;
}

void Poll::run(ntci::Waiter waiter)
{
    NTCI_LOG_CONTEXT();

    int rc;

    Poll::Result* result = static_cast<Poll::Result*>(waiter);
    BSLS_ASSERT(result->d_options.threadHandle() == bslmt::ThreadUtil::self());

    NTCCFG_WARNING_UNUSED(result);

    NTCS_METRICS_GET();

    while (d_run) {
        if (d_config.maxThreads().value() > 1) {
            d_generationSemaphore.wait();
        }

        int timeout = d_chronology.timeoutInMilliseconds();

        bsl::uint64_t generation = d_generation;

        {
            LockGuard lock(&d_generationMutex);

            if (d_config.oneShot().value() ||
                result->d_generation != generation)
            {
                result->d_generation = generation;

                result->d_descriptorList.clear();
                result->d_descriptorList.reserve(d_registry.size() + 1);

                result->d_controllerHandleFound = false;
                result->d_controllerHandleIdx   = 0;
                d_registry.forEach(result->d_forEachCallback);
                BSLS_ASSERT(result->d_controllerHandleFound);
                BSLS_ASSERT(result->d_controllerHandleIdx <
                            result->d_descriptorList.size());
            }
        }

        bsl::size_t numDetachments = 0;
        {
            LockGuard lock(&d_detachMutex);

            for (DetachList::const_iterator it = d_detachList.cbegin();
                 it != d_detachList.cend();)
            {
                ntcs::RegistryEntry& entry = **it;
                bool                 erase = false;
                if (!entry.isProcessing() &&
                    entry.announceDetached(this->getSelf(this)))
                {
                    entry.clear();
                    ++numDetachments;
                    erase = true;
                }
                it = erase ? d_detachList.erase(it) : ++it;
            }
        }

        if (numDetachments > 0) {
            timeout = 0;
        }

        int wait;
        if (timeout >= 0) {
            NTCO_POLL_LOG_WAIT_TIMED(timeout);
            wait = timeout;
        }
        else {
            NTCO_POLL_LOG_WAIT_INDEFINITE();
            wait = -1;
        }

#if defined(BSLS_PLATFORM_OS_UNIX)

        if (timeout == 0 && this->numSockets() == 0) {
            rc = 0;
        }
        else {
            rc = ::poll(&result->d_descriptorList[0],
                        static_cast<nfds_t>(result->d_descriptorList.size()),
                        wait);
        }

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

        if (timeout == 0 && this->numSockets() == 0) {
            rc = 0;
        }
        else {
            rc = WSAPoll(&result->d_descriptorList[0],
                         static_cast<ULONG>(result->d_descriptorList.size()),
                         wait);
        }

#else
#error Not implemented
#endif

        if (rc > 0 && d_config.oneShot().value()) {
            const int numResults          = rc;
            int       numResultsRemaining = numResults;
            for (DescriptorList::const_iterator it =
                     result->d_descriptorList.begin();
                 it != result->d_descriptorList.end();
                 ++it)
            {
                if (numResultsRemaining == 0) {
                    break;
                }

                struct ::pollfd e = *it;

                if (e.revents == 0) {
                    continue;
                }

                BSLS_ASSERT(numResultsRemaining > 0);
                --numResultsRemaining;

                BSLS_ASSERT(e.fd >= 0);
                BSLS_ASSERT(e.revents != 0);

                bsl::shared_ptr<ntcs::RegistryEntry> entry;
                if (!d_registry.lookup(&entry, e.fd)) {
                    continue;
                }

                if (NTCCFG_LIKELY(e.fd != d_controllerDescriptorHandle)) {
                    ntcs::Interest interest = entry->interest();
                    bool           disarm   = false;

                    if ((e.revents & POLLOUT) != 0) {
                        interest.hideWritable();
                        disarm = true;
                    }

                    if (((e.revents & POLLIN) != 0) ||
                        ((e.revents & POLLHUP) != 0))
                    {
                        interest.hideReadable();
                        disarm = true;
                    }

                    if (disarm) {
                        this->update(entry->handle(), interest, e_EXCLUDE);
                    }
                }
            }
        }

        bsl::size_t numReadable = 0;

        if (rc > 0) {
            const struct ::pollfd& e =
                result->d_descriptorList[result->d_controllerHandleIdx];
            if (NTCCFG_UNLIKELY((e.revents & POLLERR) != 0)) {
                this->reinitializeControl();
            }
            else {
                if (((e.revents & POLLIN) != 0) ||
                    ((e.revents & POLLHUP) != 0))
                {
                    ++numReadable;
                    ntsa::Error error = d_controller_sp->acknowledge();
                    if (NTCCFG_UNLIKELY(error)) {
                        this->reinitializeControl();
                    }
                    else {
                        bsl::shared_ptr<ntcs::RegistryEntry> entry;
                        //TODO: can be optimized (store controller entry in registry)
                        if (d_registry.lookup(&entry, e.fd)) {
                            if (entry->oneShot()) {
                                ntca::ReactorEventOptions options;
                                ntcs::Interest            interest =
                                    entry->showReadable(options);
                                this->update(entry->handle(),
                                             interest,
                                             e_INCLUDE);
                            }
                        }
                    }
                }
            }
        }

        if (d_config.maxThreads().value() > 1) {
            d_generationSemaphore.post();
        }

        if (NTCCFG_LIKELY(rc > 0)) {
            NTCO_POLL_LOG_WAIT_RESULT(rc);

            int numResults          = rc;
            int numResultsRemaining = numResults;

            bsl::size_t numWritable = 0;
            bsl::size_t numErrors   = 0;

            for (DescriptorList::const_iterator it =
                     result->d_descriptorList.begin();
                 it != result->d_descriptorList.end();
                 ++it)
            {
                if (numResultsRemaining == 0) {
                    break;
                }

                struct ::pollfd e = *it;

                if (e.revents == 0) {
                    continue;
                }

                BSLS_ASSERT(numResultsRemaining > 0);
                --numResultsRemaining;

                NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(e.fd);

                NTCO_POLL_LOG_EVENTS(e.fd, e);

                if (NTCCFG_UNLIKELY(e.fd == d_controllerDescriptorHandle)) {
                    continue;
                }

                bsl::shared_ptr<ntcs::RegistryEntry> entry;
                if (!d_registry.lookupAndMarkProcessingOngoing(&entry, e.fd)) {
                    continue;
                }

                ntsa::Handle descriptorHandle = entry->handle();

                bool fatalSocketError = false;

                if (NTCCFG_UNLIKELY((e.revents & POLLNVAL) != 0)) {
                    continue;
                }
                else if (NTCCFG_UNLIKELY((e.revents & POLLERR) != 0)) {
                    ntsa::Error lastError;
                    ntsa::Error error =
                        ntsf::System::getLastError(&lastError,
                                                   descriptorHandle);
                    if (error) {
                        if (!lastError) {
                            lastError =
                                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD);
                        }
                    }

                    if (NTCCFG_LIKELY(!lastError)) {
                        bdlma::LocalSequentialAllocator<
                            ntsa::NotificationQueue::k_NUM_BYTES_TO_ALLOCATE>
                                                lsa(d_allocator_p);
                        ntsa::NotificationQueue queue(descriptorHandle, &lsa);

                        lastError = ntsu::SocketUtil::receiveNotifications(
                            &queue,
                            descriptorHandle);

                        if (NTCCFG_LIKELY(!lastError)) {
                            entry->announceNotifications(queue);
                        }
                    }
                    if (NTCCFG_UNLIKELY(lastError)) {
                        fatalSocketError = true;

                        ntca::ReactorEvent event;
                        event.setHandle(descriptorHandle);
                        event.setType(ntca::ReactorEventType::e_ERROR);
                        event.setError(lastError);

                        NTCS_METRICS_UPDATE_ERROR_CALLBACK_TIME_BEGIN();
                        if (entry->announceError(event)) {
                            ++numErrors;
                        }
                        NTCS_METRICS_UPDATE_ERROR_CALLBACK_TIME_END();
                    }
                }
                if (NTCCFG_LIKELY(!fatalSocketError)) {
                    if (((e.revents & POLLOUT) != 0) ||
                        ((e.revents & POLLHUP) != 0))
                    {
                        if (entry->wantWritable()) {
                            ntca::ReactorEvent event;
                            event.setHandle(descriptorHandle);
                            event.setType(ntca::ReactorEventType::e_WRITABLE);

                            NTCS_METRICS_UPDATE_WRITE_CALLBACK_TIME_BEGIN();
                            if (entry->announceWritable(event)) {
                                ++numWritable;
                            }
                            NTCS_METRICS_UPDATE_WRITE_CALLBACK_TIME_END();
                        }
                    }

                    if (((e.revents & POLLIN) != 0) ||
                        ((e.revents & POLLHUP) != 0))
                    {
                        if (entry->wantReadable()) {
                            ntca::ReactorEvent event;
                            event.setHandle(descriptorHandle);
                            event.setType(ntca::ReactorEventType::e_READABLE);

                            NTCS_METRICS_UPDATE_READ_CALLBACK_TIME_BEGIN();
                            if (entry->announceReadable(event)) {
                                ++numReadable;
                            }
                            NTCS_METRICS_UPDATE_READ_CALLBACK_TIME_END();
                        }
                    }
                }

                entry->decrementProcessCounter();
            }

            {
                bool interrupt = false;
                {
                    LockGuard detachGuard(&d_detachMutex);
                    interrupt = !d_detachList.empty();
                }
                if (interrupt) {
                    this->interruptOne();
                }
            }

            BSLS_ASSERT(numResultsRemaining == 0);

            if (NTCCFG_UNLIKELY(numReadable == 0 && numWritable == 0 &&
                                numErrors == 0 && numDetachments == 0))
            {
                NTCS_METRICS_UPDATE_SPURIOUS_WAKEUP();
                bslmt::ThreadUtil::yield();
            }
            else {
                NTCS_METRICS_UPDATE_POLL(numReadable, numWritable, numErrors);
            }
        }
        else if (rc == 0) {
            NTCO_POLL_LOG_WAIT_TIMEOUT();
            NTCS_METRICS_UPDATE_POLL(0, 0, 0);
        }
        else if (rc < 0) {
#if defined(BSLS_PLATFORM_OS_UNIX)

            if (errno == EINTR) {
                // MRM: Handle this errno.
            }
            else if (errno == EBADF) {
                // MRM: Handle this errno.
            }
            else if (errno == ENOTSOCK) {
                // MRM: Handle this errno.
            }
            else {
                ntsa::Error error(errno);
                NTCO_POLL_LOG_WAIT_FAILURE(error);
            }

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

            DWORD lastError = WSAGetLastError();
            if (lastError == WSAEINTR) {
                // MRM: Handle this errno.
            }
            else if (lastError == WSAEBADF) {
                // MRM: Handle this errno.
            }
            else if (lastError == WSAENOTSOCK) {
                // MRM: Handle this errno.
            }
            else if (lastError == WSAENETDOWN) {
                // MRM: Handle this errno.
            }
            else {
                ntsa::Error error(errno);
                NTCO_POLL_LOG_WAIT_FAILURE(error);
            }

#else
#error Not implemented
#endif
        }

        // Invoke functions deferred while processing each polled event and
        // process all expired timers.

        bsl::size_t numCycles = d_config.maxCyclesPerWait().value();
        while (numCycles != 0) {
            if (d_chronology.hasAnyScheduledOrDeferred()) {
                d_chronology.announce(d_dynamic);
                --numCycles;
            }
            else {
                break;
            }
        }
    }
}

void Poll::poll(ntci::Waiter waiter)
{
    NTCI_LOG_CONTEXT();

    int rc;

    Poll::Result* result = static_cast<Poll::Result*>(waiter);
    BSLS_ASSERT(result->d_options.threadHandle() == bslmt::ThreadUtil::self());

    NTCCFG_WARNING_UNUSED(result);

    NTCS_METRICS_GET();

    if (d_config.maxThreads().value() > 1) {
        d_generationSemaphore.wait();
    }

    int timeout = d_chronology.timeoutInMilliseconds();

    bsl::uint64_t generation = d_generation;

    {
        LockGuard lock(&d_generationMutex);

        if (d_config.oneShot().value() || result->d_generation != generation) {
            result->d_generation = generation;

            result->d_descriptorList.clear();
            result->d_descriptorList.reserve(d_registry.size() + 1);

            d_registry.forEach(result->d_forEachCallback);
        }
    }

    bsl::size_t numDetachments = 0;
    {
        LockGuard lock(&d_detachMutex);

        for (DetachList::const_iterator it = d_detachList.cbegin();
             it != d_detachList.cend();)
        {
            ntcs::RegistryEntry& entry = **it;
            bool                 erase = false;
            if (!entry.isProcessing() &&
                entry.announceDetached(this->getSelf(this)))
            {
                entry.clear();
                ++numDetachments;
                erase = true;
            }
            it = erase ? d_detachList.erase(it) : ++it;
        }
    }

    if (numDetachments > 0) {
        timeout = 0;
    }

    int wait;
    if (timeout >= 0) {
        NTCO_POLL_LOG_WAIT_TIMED(timeout);
        wait = timeout;
    }
    else {
        NTCO_POLL_LOG_WAIT_INDEFINITE();
        wait = -1;
    }

#if defined(BSLS_PLATFORM_OS_UNIX)

    if (timeout == 0 && this->numSockets() == 0) {
        rc = 0;
    }
    else {
        rc = ::poll(&result->d_descriptorList[0],
                    static_cast<nfds_t>(result->d_descriptorList.size()),
                    wait);
    }

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    if (timeout == 0 && this->numSockets() == 0) {
        rc = 0;
    }
    else {
        rc = WSAPoll(&result->d_descriptorList[0],
                     static_cast<ULONG>(result->d_descriptorList.size()),
                     wait);
    }

#else
#error Not implemented
#endif

    if (rc > 0 && d_config.oneShot().value()) {
        const int numResults          = rc;
        int       numResultsRemaining = numResults;
        for (DescriptorList::const_iterator it =
                 result->d_descriptorList.begin();
             it != result->d_descriptorList.end();
             ++it)
        {
            if (numResultsRemaining == 0) {
                break;
            }

            struct ::pollfd e = *it;

            if (e.revents == 0) {
                continue;
            }

            BSLS_ASSERT(numResultsRemaining > 0);
            --numResultsRemaining;

            BSLS_ASSERT(e.fd >= 0);
            BSLS_ASSERT(e.revents != 0);

            bsl::shared_ptr<ntcs::RegistryEntry> entry;
            if (!d_registry.lookup(&entry, e.fd)) {
                continue;
            }

            if (NTCCFG_LIKELY(e.fd != d_controllerDescriptorHandle)) {
                ntcs::Interest interest = entry->interest();
                bool           disarm   = false;

                if ((e.revents & POLLOUT) != 0) {
                    interest.hideWritable();
                    disarm = true;
                }

                if (((e.revents & POLLIN) != 0) ||
                    ((e.revents & POLLHUP) != 0))
                {
                    interest.hideReadable();
                    disarm = true;
                }

                if (disarm) {
                    this->update(entry->handle(), interest, e_EXCLUDE);
                }
            }
        }
    }

    bsl::size_t numReadable = 0;

    if (rc > 0) {
        const struct ::pollfd& e =
            result->d_descriptorList[result->d_controllerHandleIdx];
        if (NTCCFG_UNLIKELY((e.revents & POLLERR) != 0)) {
            this->reinitializeControl();
        }
        else {
            if (((e.revents & POLLIN) != 0) || ((e.revents & POLLHUP) != 0)) {
                ++numReadable;
                ntsa::Error error = d_controller_sp->acknowledge();
                if (NTCCFG_UNLIKELY(error)) {
                    this->reinitializeControl();
                }
                else {
                    bsl::shared_ptr<ntcs::RegistryEntry> entry;
                    //TODO: can be optimized (store controller entry in registry)
                    if (d_registry.lookup(&entry, e.fd)) {
                        if (entry->oneShot()) {
                            ntca::ReactorEventOptions options;
                            ntcs::Interest            interest =
                                entry->showReadable(options);
                            this->update(entry->handle(), interest, e_INCLUDE);
                        }
                    }
                }
            }
        }
    }

    if (d_config.maxThreads().value() > 1) {
        d_generationSemaphore.post();
    }

    if (NTCCFG_LIKELY(rc > 0)) {
        NTCO_POLL_LOG_WAIT_RESULT(rc);

        int numResults          = rc;
        int numResultsRemaining = numResults;

        bsl::size_t numWritable = 0;
        bsl::size_t numErrors   = 0;

        for (DescriptorList::const_iterator it =
                 result->d_descriptorList.begin();
             it != result->d_descriptorList.end();
             ++it)
        {
            if (numResultsRemaining == 0) {
                break;
            }

            struct ::pollfd e = *it;

            if (e.revents == 0) {
                continue;
            }

            BSLS_ASSERT(numResultsRemaining > 0);
            --numResultsRemaining;

            NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(e.fd);

            NTCO_POLL_LOG_EVENTS(e.fd, e);

            if (NTCCFG_UNLIKELY(e.fd == d_controllerDescriptorHandle)) {
                continue;
            }

            bsl::shared_ptr<ntcs::RegistryEntry> entry;
            if (!d_registry.lookupAndMarkProcessingOngoing(&entry, e.fd)) {
                continue;
            }

            ntsa::Handle descriptorHandle = entry->handle();

            bool fatalSocketError = false;

            if (NTCCFG_UNLIKELY((e.revents & POLLNVAL) != 0)) {
                continue;
            }
            else if (NTCCFG_UNLIKELY((e.revents & POLLERR) != 0)) {
                ntsa::Error lastError;
                ntsa::Error error =
                    ntsf::System::getLastError(&lastError, descriptorHandle);
                if (error) {
                    if (!lastError) {
                        lastError =
                            ntsa::Error(ntsa::Error::e_CONNECTION_DEAD);
                    }
                }

                if (NTCCFG_LIKELY(!lastError)) {
                    bdlma::LocalSequentialAllocator<
                        ntsa::NotificationQueue::k_NUM_BYTES_TO_ALLOCATE>
                                            lsa(d_allocator_p);
                    ntsa::NotificationQueue queue(descriptorHandle, &lsa);

                    lastError = ntsu::SocketUtil::receiveNotifications(
                        &queue,
                        descriptorHandle);

                    if (NTCCFG_LIKELY(!lastError)) {
                        entry->announceNotifications(queue);
                    }
                }
                if (NTCCFG_UNLIKELY(lastError)) {
                    fatalSocketError = true;

                    ntca::ReactorEvent event;
                    event.setHandle(descriptorHandle);
                    event.setType(ntca::ReactorEventType::e_ERROR);
                    event.setError(lastError);

                    NTCS_METRICS_UPDATE_ERROR_CALLBACK_TIME_BEGIN();
                    if (entry->announceError(event)) {
                        ++numErrors;
                    }
                    NTCS_METRICS_UPDATE_ERROR_CALLBACK_TIME_END();
                }
            }
            if (NTCCFG_LIKELY(!fatalSocketError)) {
                if (((e.revents & POLLOUT) != 0) ||
                    ((e.revents & POLLHUP) != 0))
                {
                    if (entry->wantWritable()) {
                        ntca::ReactorEvent event;
                        event.setHandle(descriptorHandle);
                        event.setType(ntca::ReactorEventType::e_WRITABLE);

                        NTCS_METRICS_UPDATE_WRITE_CALLBACK_TIME_BEGIN();
                        if (entry->announceWritable(event)) {
                            ++numWritable;
                        }
                        NTCS_METRICS_UPDATE_WRITE_CALLBACK_TIME_END();
                    }
                }

                if (((e.revents & POLLIN) != 0) ||
                    ((e.revents & POLLHUP) != 0))
                {
                    if (entry->wantReadable()) {
                        ntca::ReactorEvent event;
                        event.setHandle(descriptorHandle);
                        event.setType(ntca::ReactorEventType::e_READABLE);

                        NTCS_METRICS_UPDATE_READ_CALLBACK_TIME_BEGIN();
                        if (entry->announceReadable(event)) {
                            ++numReadable;
                        }
                        NTCS_METRICS_UPDATE_READ_CALLBACK_TIME_END();
                    }
                }
            }

            entry->decrementProcessCounter();
        }

        {
            bool interrupt = false;
            {
                LockGuard detachGuard(&d_detachMutex);
                interrupt = !d_detachList.empty();
            }
            if (interrupt) {
                this->interruptOne();
            }
        }

        BSLS_ASSERT(numResultsRemaining == 0);

        if (NTCCFG_UNLIKELY(numReadable == 0 && numWritable == 0 &&
                            numErrors == 0 && numDetachments == 0))
        {
            NTCS_METRICS_UPDATE_SPURIOUS_WAKEUP();
            bslmt::ThreadUtil::yield();
        }
        else {
            NTCS_METRICS_UPDATE_POLL(numReadable, numWritable, numErrors);
        }
    }
    else if (rc == 0) {
        NTCO_POLL_LOG_WAIT_TIMEOUT();
        NTCS_METRICS_UPDATE_POLL(0, 0, 0);
    }
    else if (rc < 0) {
#if defined(BSLS_PLATFORM_OS_UNIX)

        if (errno == EINTR) {
            // MRM: Handle this errno.
        }
        else if (errno == EBADF) {
            // MRM: Handle this errno.
        }
        else if (errno == ENOTSOCK) {
            // MRM: Handle this errno.
        }
        else {
            ntsa::Error error(errno);
            NTCO_POLL_LOG_WAIT_FAILURE(error);
        }

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

        DWORD lastError = WSAGetLastError();
        if (lastError == WSAEINTR) {
            // MRM: Handle this errno.
        }
        else if (lastError == WSAEBADF) {
            // MRM: Handle this errno.
        }
        else if (lastError == WSAENOTSOCK) {
            // MRM: Handle this errno.
        }
        else if (lastError == WSAENETDOWN) {
            // MRM: Handle this errno.
        }
        else {
            ntsa::Error error(errno);
            NTCO_POLL_LOG_WAIT_FAILURE(error);
        }

#else
#error Not implemented
#endif
    }

    // Invoke functions deferred while processing each polled event and process
    // all expired timers.

    bsl::size_t numCycles = d_config.maxCyclesPerWait().value();
    while (numCycles != 0) {
        if (d_chronology.hasAnyScheduledOrDeferred()) {
            d_chronology.announce(d_dynamic);
            --numCycles;
        }
        else {
            break;
        }
    }
}

void Poll::interruptOne()
{
    const ntsa::Error error = d_controller_sp->interrupt(1);
    if (NTCCFG_UNLIKELY(error)) {
        reinitializeControl();
    }
}

void Poll::interruptAll()
{
    if (NTCCFG_LIKELY(d_config.maxThreads().value() == 1)) {
        if (NTCCFG_LIKELY(isWaiter())) {
            return;
        }

        ntsa::Error error = d_controller_sp->interrupt(1);
        if (NTCCFG_UNLIKELY(error)) {
            reinitializeControl();
        }
    }
    else {
        unsigned int numWaiters;
        {
            LockGuard lock(&d_waiterSetMutex);
            numWaiters =
                NTCCFG_WARNING_NARROW(unsigned int, d_waiterSet.size());
        }

        if (NTCCFG_LIKELY(numWaiters > 0)) {
            ntsa::Error error = d_controller_sp->interrupt(numWaiters);
            if (NTCCFG_UNLIKELY(error)) {
                reinitializeControl();
            }
        }
    }
}

void Poll::stop()
{
    d_run = false;
    Poll::interruptAll();
}

void Poll::restart()
{
    d_run = true;
}

void Poll::drainFunctions()
{
    d_chronology.drain();
}

void Poll::clearFunctions()
{
    d_chronology.clearFunctions();
}

void Poll::clearTimers()
{
    d_chronology.clearTimers();
}

void Poll::clearSockets()
{
    bsl::vector<bsl::shared_ptr<ntcs::RegistryEntry> > entryList;
    d_registry.clear(&entryList, d_controllerDescriptorHandle);

    for (bsl::vector<bsl::shared_ptr<ntcs::RegistryEntry> >::iterator it =
             entryList.begin();
         it != entryList.end();
         ++it)
    {
        const bsl::shared_ptr<ntcs::RegistryEntry>& entry = *it;
        this->remove(entry->handle());
    }

    entryList.clear();
}

void Poll::clear()
{
    d_chronology.clear();

    bsl::vector<bsl::shared_ptr<ntcs::RegistryEntry> > entryList;
    d_registry.clear(&entryList, d_controllerDescriptorHandle);

    for (bsl::vector<bsl::shared_ptr<ntcs::RegistryEntry> >::iterator it =
             entryList.begin();
         it != entryList.end();
         ++it)
    {
        const bsl::shared_ptr<ntcs::RegistryEntry>& entry = *it;
        this->remove(entry->handle());
    }

    entryList.clear();
}

void Poll::execute(const Functor& functor)
{
    d_chronology.execute(functor);
}

void Poll::moveAndExecute(FunctorSequence* functorSequence,
                          const Functor&   functor)
{
    d_chronology.moveAndExecute(functorSequence, functor);
}

bsl::shared_ptr<ntci::Timer> Poll::createTimer(
    const ntca::TimerOptions&                  options,
    const bsl::shared_ptr<ntci::TimerSession>& session,
    bslma::Allocator*                          basicAllocator)
{
    return d_chronology.createTimer(options, session, basicAllocator);
}

bsl::shared_ptr<ntci::Timer> Poll::createTimer(
    const ntca::TimerOptions&  options,
    const ntci::TimerCallback& callback,
    bslma::Allocator*          basicAllocator)
{
    return d_chronology.createTimer(options, callback, basicAllocator);
}

bsl::shared_ptr<ntci::DatagramSocket> Poll::createDatagramSocket(
    const ntca::DatagramSocketOptions& options,
    bslma::Allocator*                  basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    // MRM: Define how metrics are injected into sockets.
    bsl::shared_ptr<ntcs::Metrics> metrics;

    bsl::shared_ptr<ntcr::DatagramSocket> datagramSocket;
    datagramSocket.createInplace(allocator,
                                 options,
                                 d_resolver_sp,
                                 this->getSelf(this),
                                 this->getSelf(this),
                                 metrics,
                                 allocator);

    return datagramSocket;
}

bsl::shared_ptr<ntci::ListenerSocket> Poll::createListenerSocket(
    const ntca::ListenerSocketOptions& options,
    bslma::Allocator*                  basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    // MRM: Define how metrics are injected into sockets.
    bsl::shared_ptr<ntcs::Metrics> metrics;

    bsl::shared_ptr<ntcr::ListenerSocket> listenerSocket;
    listenerSocket.createInplace(allocator,
                                 options,
                                 d_resolver_sp,
                                 this->getSelf(this),
                                 this->getSelf(this),
                                 metrics,
                                 allocator);

    return listenerSocket;
}

bsl::shared_ptr<ntci::StreamSocket> Poll::createStreamSocket(
    const ntca::StreamSocketOptions& options,
    bslma::Allocator*                basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    // MRM: Define how metrics are injected into sockets.
    bsl::shared_ptr<ntcs::Metrics> metrics;

    bsl::shared_ptr<ntcr::StreamSocket> streamSocket;
    streamSocket.createInplace(allocator,
                               options,
                               d_resolver_sp,
                               this->getSelf(this),
                               this->getSelf(this),
                               metrics,
                               allocator);

    return streamSocket;
}

bsl::shared_ptr<ntsa::Data> Poll::createIncomingData()
{
    return d_dataPool_sp->createIncomingData();
}

bsl::shared_ptr<ntsa::Data> Poll::createOutgoingData()
{
    return d_dataPool_sp->createOutgoingData();
}

bsl::shared_ptr<bdlbb::Blob> Poll::createIncomingBlob()
{
    return d_dataPool_sp->createIncomingBlob();
}

bsl::shared_ptr<bdlbb::Blob> Poll::createOutgoingBlob()
{
    return d_dataPool_sp->createOutgoingBlob();
}

void Poll::createIncomingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_dataPool_sp->createIncomingBlobBuffer(blobBuffer);
}

void Poll::createOutgoingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_dataPool_sp->createOutgoingBlobBuffer(blobBuffer);
}

bsl::size_t Poll::numSockets() const
{
    bsl::size_t result = d_registry.size();
    if (result > 0) {
        return result - 1;
    }
    else {
        return result;
    }
}

bsl::size_t Poll::maxSockets() const
{
    return static_cast<bsl::size_t>(-1);
}

bsl::size_t Poll::numTimers() const
{
    return d_chronology.numScheduled();
}

bsl::size_t Poll::maxTimers() const
{
    return static_cast<bsl::size_t>(-1);
}

bool Poll::autoAttach() const
{
    return d_config.autoAttach().value();
}

bool Poll::autoDetach() const
{
    return d_config.autoDetach().value();
}

bool Poll::oneShot() const
{
    return d_config.oneShot().value();
}

ntca::ReactorEventTrigger::Value Poll::trigger() const
{
    return d_config.trigger().value();
}

bsl::size_t Poll::load() const
{
    return static_cast<bsl::size_t>(d_load);
}

bslmt::ThreadUtil::Handle Poll::threadHandle() const
{
    LockGuard lock(&d_waiterSetMutex);
    return d_threadHandle;
}

bsl::size_t Poll::threadIndex() const
{
    LockGuard lock(&d_waiterSetMutex);
    return d_threadIndex;
}

bsl::size_t Poll::numWaiters() const
{
    LockGuard lock(&d_waiterSetMutex);
    return d_waiterSet.size();
}

bool Poll::empty() const
{
    if (d_chronology.hasAnyScheduledOrDeferred()) {
        return false;
    }

    if (d_chronology.hasAnyRegistered()) {
        return false;
    }

    if (this->numSockets() != 0) {
        return false;
    }

    return true;
}

const bsl::shared_ptr<ntci::DataPool>& Poll::dataPool() const
{
    return d_dataPool_sp;
}

bool Poll::supportsOneShot(bool oneShot) const
{
    return oneShot == d_config.oneShot().value();
}

bool Poll::supportsTrigger(ntca::ReactorEventTrigger::Value trigger) const
{
    if (trigger == ntca::ReactorEventTrigger::e_EDGE) {
        return false;
    }
    else {
        return true;
    }
}

bool Poll::supportsNotifications() const
{
    return true;
}

const bsl::shared_ptr<ntci::Strand>& Poll::strand() const
{
    return ntci::Strand::unspecified();
}

bsls::TimeInterval Poll::currentTime() const
{
    return bdlt::CurrentTime::now();
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& Poll::
    incomingBlobBufferFactory() const
{
    return d_dataPool_sp->incomingBlobBufferFactory();
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& Poll::
    outgoingBlobBufferFactory() const
{
    return d_dataPool_sp->outgoingBlobBufferFactory();
}

const char* Poll::name() const
{
    return "POLL";
}

PollFactory::PollFactory(bslma::Allocator* basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

PollFactory::~PollFactory()
{
}

bsl::shared_ptr<ntci::Reactor> PollFactory::createReactor(
    const ntca::ReactorConfig&         configuration,
    const bsl::shared_ptr<ntci::User>& user,
    bslma::Allocator*                  basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntco::Poll> reactor;
    reactor.createInplace(allocator, configuration, user, allocator);

    return reactor;
}

}  // close package namespace
}  // close enterprise namespace

#endif
