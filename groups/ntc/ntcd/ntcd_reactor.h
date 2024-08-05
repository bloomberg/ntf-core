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

#ifndef INCLUDED_NTCD_REACTOR
#define INCLUDED_NTCD_REACTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_reactorconfig.h>
#include <ntccfg_platform.h>
#include <ntccfg_test.h>
#include <ntcd_datagramsocket.h>
#include <ntcd_listenersocket.h>
#include <ntcd_streamsocket.h>
#include <ntci_datagramsocket.h>
#include <ntci_datagramsocketfactory.h>
#include <ntci_listenersocket.h>
#include <ntci_listenersocketfactory.h>
#include <ntci_reactor.h>
#include <ntci_reactorfactory.h>
#include <ntci_reservation.h>
#include <ntci_strand.h>
#include <ntci_streamsocket.h>
#include <ntci_streamsocketfactory.h>
#include <ntci_timer.h>
#include <ntci_user.h>
#include <ntcs_chronology.h>
#include <ntcs_driver.h>
#include <ntcs_registry.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcd {

/// @internal @brief
/// Provide a reactor for simulated sockets.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class Reactor : public ntci::Reactor,
                public ntcs::Driver,
                public ntccfg::Shared<Reactor>
{
    /// This typedef defines a set of waiters.
    typedef bsl::unordered_set<ntci::Waiter> WaiterSet;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

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

    bsl::shared_ptr<ntcd::Machine>               d_machine_sp;
    bsl::shared_ptr<ntcd::Monitor>               d_monitor_sp;
    bsl::shared_ptr<ntci::User>                  d_user_sp;
    bsl::shared_ptr<ntci::DataPool>              d_dataPool_sp;
    bsl::shared_ptr<ntci::Resolver>              d_resolver_sp;
    bsl::shared_ptr<ntcs::Chronology>            d_chronology_sp;
    bsl::shared_ptr<ntci::Reservation>           d_connectionLimiter_sp;
    bsl::shared_ptr<ntci::ReactorMetrics>        d_metrics_sp;
    bsl::shared_ptr<ntci::DatagramSocketFactory> d_datagramSocketFactory_sp;
    bsl::shared_ptr<ntci::ListenerSocketFactory> d_listenerSocketFactory_sp;
    bsl::shared_ptr<ntci::StreamSocketFactory>   d_streamSocketFactory_sp;
    bsl::shared_ptr<ntcs::RegistryEntryCatalog::EntryFunctor>
                                                d_detachFunctor_sp;
    bsl::shared_ptr<ntcs::RegistryEntryCatalog> d_registry_sp;
    mutable Mutex                               d_waiterSetMutex;
    WaiterSet                                   d_waiterSet;
    bslmt::ThreadUtil::Handle                   d_threadHandle;
    bsl::size_t                                 d_threadIndex;
    bool                                        d_dynamic;
    bsls::AtomicUint64                          d_load;
    bsls::AtomicBool                            d_run;
    ntca::ReactorConfig                         d_config;
    bslma::Allocator*                           d_allocator_p;

  private:
             Reactor(const Reactor&) BSLS_KEYWORD_DELETED;
    Reactor& operator=(const Reactor&) BSLS_KEYWORD_DELETED;

  private:
    /// Initialize this object.
    void initialize();

    /// Execute all pending jobs.
    void flush();

    /// Add the specified 'handle' with the specified 'interest' to the
    /// device. Return the error.
    ntsa::Error add(ntsa::Handle handle, ntcs::Interest interest);

    /// Update the specified 'handle' with the specified 'interest' in the
    /// device. The specified 'type' indicates whether events have been
    /// included or excluded as a result of the update. Return the error.
    ntsa::Error update(ntsa::Handle   handle,
                       ntcs::Interest interest,
                       UpdateType     type);

    /// Remove the specified 'handle' from the device.
    ntsa::Error remove(ntsa::Handle handle);

    /// Remove the specified 'entry' from the device and announce its
    /// detachment if possible. Return the error.
    ntsa::Error removeDetached(
        const bsl::shared_ptr<ntcs::RegistryEntry>& entry);

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
    /// Defines a type alias for a function to create a new datagram socket
    /// with the specified 'options'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    typedef ntcd::DatagramSocketFactory::Function
        DatagramSocketFactoryFunction;

    /// Defines a type alias for a function to create a new listener socket
    /// with the specified 'options'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    typedef ntcd::ListenerSocketFactory::Function
        ListenerSocketFactoryFunction;

    /// Defines a type alias for a function to create a new stream socket
    /// with the specified 'options'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    typedef ntcd::StreamSocketFactory::Function StreamSocketFactoryFunction;

    /// Create a new test reactor having the specified 'configuration'
    /// operating in the environment of the specified 'user' implemented
    /// using sessions on the default machine . Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Reactor(const ntca::ReactorConfig&         configuration,
                     const bsl::shared_ptr<ntci::User>& user,
                     bslma::Allocator*                  basicAllocator = 0);

    /// Create a new test reactor having the specified 'configuration'
    /// operating in the environment of the specified 'user' implemented
    /// using sessions on the specified 'machine' . Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Reactor(const ntca::ReactorConfig&            configuration,
                     const bsl::shared_ptr<ntci::User>&    user,
                     const bsl::shared_ptr<ntcd::Machine>& machine,
                     bslma::Allocator*                     basicAllocator = 0);

    /// Destroy this object.
    ~Reactor() BSLS_KEYWORD_OVERRIDE;

    /// Set the datagram socket factory used to create datagram sockets to
    /// the specified 'datagramSocketFactory'.
    void setDatagramSocketFactory(
        const bsl::shared_ptr<ntci::DatagramSocketFactory>&
            datagramSocketFactory);

    /// Set the datagram socket factory used to create datagram sockets to
    /// the specified 'datagramSocketFactory'.
    void setDatagramSocketFactory(
        const DatagramSocketFactoryFunction& datagramSocketFactory);

    /// Set the listener socket factory used to create listener sockets to
    /// the specified 'listenerSocketFactory'.
    void setListenerSocketFactory(
        const bsl::shared_ptr<ntci::ListenerSocketFactory>&
            listenerSocketFactory);

    /// Set the listener socket factory used to create listener sockets to
    /// the specified 'listenerSocketFactory'.
    void setListenerSocketFactory(
        const ListenerSocketFactoryFunction& listenerSocketFactory);

    /// Set the stream socket factory used to create stream sockets to
    /// the specified 'streamSocketFactory'.
    void setStreamSocketFactory(
        const bsl::shared_ptr<ntci::StreamSocketFactory>& streamSocketFactory);

    /// Set the stream socket factory used to create stream sockets to
    /// the specified 'streamSocketFactory'.
    void setStreamSocketFactory(
        const StreamSocketFactoryFunction& streamSocketFactory);

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

    /// Stop monitoring the specified socket 'handle'. Invoke the specified
    /// 'callback' when the socket is detached. Return the error.
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

    /// Return the data pool.
    const bsl::shared_ptr<ntci::DataPool>& dataPool() const
        BSLS_KEYWORD_OVERRIDE;

    /// Return true if the reactor supports registering events in the
    /// specified 'oneShot' mode, otherwise return false.
    bool supportsOneShot(bool oneShot) const BSLS_KEYWORD_OVERRIDE;

    /// Return true if the reactor supports registering events having the
    /// specified 'trigger', otherwise return false.
    bool supportsTrigger(ntca::ReactorEventTrigger::Value trigger) const
        BSLS_KEYWORD_OVERRIDE;

    /// Always return true indicating that the reactor supports notifications
    /// of the socket.
    bool supportsNotifications() const BSLS_KEYWORD_OVERRIDE;

    /// Return the strand that guarantees sequential, non-current execution
    /// of arbitrary functors on the unspecified threads processing events
    /// for this object.
    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current elapsed time since the Unix epoch.
    bsls::TimeInterval currentTime() const BSLS_KEYWORD_OVERRIDE;

    /// Return the incoming blob buffer factory.
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& incomingBlobBufferFactory()
        const BSLS_KEYWORD_OVERRIDE;

    /// Return the outgoing blob buffer factory.
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& outgoingBlobBufferFactory()
        const BSLS_KEYWORD_OVERRIDE;

    /// Return the name of the driver.
    const char* name() const BSLS_KEYWORD_OVERRIDE;
};

/// @internal @brief
/// Provide a factory to produce reactors for simulated sockets.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class ReactorFactory : public ntci::ReactorFactory
{
    bslma::Allocator* d_allocator_p;

  private:
                    ReactorFactory(const ReactorFactory&) BSLS_KEYWORD_DELETED;
    ReactorFactory& operator=(const ReactorFactory&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new reactor factory that produces reactors for simulated
    /// sockets. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit ReactorFactory(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ReactorFactory() BSLS_KEYWORD_OVERRIDE;

    /// Create a new reactor with the specified 'configuration' operating
    /// in the environment of the specified 'user'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    bsl::shared_ptr<ntci::Reactor> createReactor(
        const ntca::ReactorConfig&         configuration,
        const bsl::shared_ptr<ntci::User>& user,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;
};

#if NTCCFG_TEST_MOCK_ENABLED

NTF_MOCK_CLASS(ReactorMock, ntci::Reactor)

NTF_MOCK_METHOD(bsl::shared_ptr<ntci::DatagramSocket>,
                createDatagramSocket,
                const ntca::DatagramSocketOptions&,
                bslma::Allocator*)
NTF_MOCK_METHOD(bsl::shared_ptr<ntsa::Data>, createIncomingData)
NTF_MOCK_METHOD(bsl::shared_ptr<ntsa::Data>, createOutgoingData)
NTF_MOCK_METHOD(bsl::shared_ptr<bdlbb::Blob>, createIncomingBlob)
NTF_MOCK_METHOD(bsl::shared_ptr<bdlbb::Blob>, createOutgoingBlob)
NTF_MOCK_METHOD(void, createIncomingBlobBuffer, bdlbb::BlobBuffer*)
NTF_MOCK_METHOD(void, createOutgoingBlobBuffer, bdlbb::BlobBuffer*)

NTF_MOCK_METHOD_CONST(const bsl::shared_ptr<bdlbb::BlobBufferFactory>&,
                      incomingBlobBufferFactory)
NTF_MOCK_METHOD_CONST(const bsl::shared_ptr<bdlbb::BlobBufferFactory>&,
                      outgoingBlobBufferFactory)

NTF_MOCK_METHOD(ntci::Waiter, registerWaiter, const ntca::WaiterOptions&)
NTF_MOCK_METHOD(void, deregisterWaiter, ntci::Waiter)
NTF_MOCK_METHOD(void, run, ntci::Waiter)
NTF_MOCK_METHOD(void, poll, ntci::Waiter)
NTF_MOCK_METHOD(void, interruptOne)
NTF_MOCK_METHOD(void, interruptAll)
NTF_MOCK_METHOD(void, stop)
NTF_MOCK_METHOD(void, restart)
NTF_MOCK_METHOD(void, execute, const Functor&)
NTF_MOCK_METHOD(void, moveAndExecute, FunctorSequence*, const Functor&)
NTF_MOCK_METHOD(bsl::shared_ptr<ntci::ListenerSocket>,
                createListenerSocket,
                const ntca::ListenerSocketOptions&,
                bslma::Allocator*)

NTF_MOCK_METHOD(ntsa::Error,
                attachSocket,
                const bsl::shared_ptr<ntci::ReactorSocket>&)
NTF_MOCK_METHOD(ntsa::Error, attachSocket, ntsa::Handle)
NTF_MOCK_METHOD(ntsa::Error,
                showReadable,
                const bsl::shared_ptr<ntci::ReactorSocket>&,
                const ntca::ReactorEventOptions&)
NTF_MOCK_METHOD(ntsa::Error,
                showReadable,
                ntsa::Handle,
                const ntca::ReactorEventOptions&,
                const ntci::ReactorEventCallback&)

NTF_MOCK_METHOD(ntsa::Error,
                showWritable,
                const bsl::shared_ptr<ntci::ReactorSocket>&,
                const ntca::ReactorEventOptions&)
NTF_MOCK_METHOD(ntsa::Error,
                showWritable,
                ntsa::Handle,
                const ntca::ReactorEventOptions&,
                const ntci::ReactorEventCallback&)

NTF_MOCK_METHOD(ntsa::Error,
                showError,
                const bsl::shared_ptr<ntci::ReactorSocket>&,
                const ntca::ReactorEventOptions&)
NTF_MOCK_METHOD(ntsa::Error,
                showError,
                ntsa::Handle,
                const ntca::ReactorEventOptions&,
                const ntci::ReactorEventCallback&)

NTF_MOCK_METHOD(ntsa::Error,
                hideReadable,
                const bsl::shared_ptr<ntci::ReactorSocket>&)
NTF_MOCK_METHOD(ntsa::Error, hideReadable, ntsa::Handle)
NTF_MOCK_METHOD(ntsa::Error,
                hideWritable,
                const bsl::shared_ptr<ntci::ReactorSocket>&)
NTF_MOCK_METHOD(ntsa::Error, hideWritable, ntsa::Handle)
NTF_MOCK_METHOD(ntsa::Error,
                hideError,
                const bsl::shared_ptr<ntci::ReactorSocket>&)
NTF_MOCK_METHOD(ntsa::Error, hideError, ntsa::Handle)
NTF_MOCK_METHOD(ntsa::Error,
                detachSocket,
                const bsl::shared_ptr<ntci::ReactorSocket>&)
NTF_MOCK_METHOD(ntsa::Error, detachSocket, ntsa::Handle)

NTF_MOCK_METHOD(ntsa::Error,
                detachSocket,
                const bsl::shared_ptr<ntci::ReactorSocket>&,
                const ntci::SocketDetachedCallback&)
NTF_MOCK_METHOD(ntsa::Error,
                detachSocket,
                ntsa::Handle,
                const ntci::SocketDetachedCallback&)

NTF_MOCK_METHOD(ntsa::Error, closeAll)
NTF_MOCK_METHOD(void, incrementLoad, const ntca::LoadBalancingOptions&)
NTF_MOCK_METHOD(void, decrementLoad, const ntca::LoadBalancingOptions&)

NTF_MOCK_METHOD(void, drainFunctions)
NTF_MOCK_METHOD(void, clearFunctions)
NTF_MOCK_METHOD(void, clearTimers)
NTF_MOCK_METHOD(void, clearSockets)
NTF_MOCK_METHOD(void, clear)
NTF_MOCK_METHOD_CONST(size_t, numSockets)
NTF_MOCK_METHOD_CONST(size_t, maxSockets)
NTF_MOCK_METHOD_CONST(size_t, numTimers)
NTF_MOCK_METHOD_CONST(size_t, maxTimers)
NTF_MOCK_METHOD_CONST(bool, autoAttach)
NTF_MOCK_METHOD_CONST(bool, autoDetach)
NTF_MOCK_METHOD_CONST(bool, oneShot)
NTF_MOCK_METHOD_CONST(ntca::ReactorEventTrigger::Value, trigger)
NTF_MOCK_METHOD_CONST(size_t, load)
NTF_MOCK_METHOD_CONST(bslmt::ThreadUtil::Handle, threadHandle)
NTF_MOCK_METHOD_CONST(size_t, threadIndex)
NTF_MOCK_METHOD_CONST(bool, empty)
NTF_MOCK_METHOD_CONST(const bsl::shared_ptr<ntci::DataPool>&, dataPool)

NTF_MOCK_METHOD_CONST(bool, supportsOneShot, bool)
NTF_MOCK_METHOD_CONST(bool, supportsTrigger, ntca::ReactorEventTrigger::Value)

NTF_MOCK_METHOD(bsl::shared_ptr<ntci::Reactor>,
                acquireReactor,
                const ntca::LoadBalancingOptions&)
NTF_MOCK_METHOD(void,
                releaseReactor,
                const bsl::shared_ptr<ntci::Reactor>&,
                const ntca::LoadBalancingOptions&)
NTF_MOCK_METHOD(bool, acquireHandleReservation)
NTF_MOCK_METHOD(void, releaseHandleReservation)

NTF_MOCK_METHOD_CONST(size_t, numReactors)
NTF_MOCK_METHOD_CONST(size_t, numThreads)
NTF_MOCK_METHOD_CONST(size_t, minThreads)
NTF_MOCK_METHOD_CONST(size_t, maxThreads)

NTF_MOCK_METHOD(bsl::shared_ptr<ntci::Strand>, createStrand, bslma::Allocator*)

NTF_MOCK_METHOD(bsl::shared_ptr<ntci::StreamSocket>,
                createStreamSocket,
                const ntca::StreamSocketOptions&,
                bslma::Allocator*)

NTF_MOCK_METHOD(bsl::shared_ptr<ntci::Timer>,
                createTimer,
                const ntca::TimerOptions&,
                const bsl::shared_ptr<ntci::TimerSession>&,
                bslma::Allocator*)
NTF_MOCK_METHOD(bsl::shared_ptr<ntci::Timer>,
                createTimer,
                const ntca::TimerOptions&,
                const ntci::TimerCallback&,
                bslma::Allocator*)
NTF_MOCK_METHOD_CONST(const bsl::shared_ptr<ntci::Strand>&, strand)
NTF_MOCK_METHOD_CONST(bsls::TimeInterval, currentTime)

NTF_MOCK_CLASS_END;

#endif

}  // close package namespace
}  // close enterprise namespace
#endif
