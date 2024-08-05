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

#ifndef INCLUDED_NTCD_PROACTOR
#define INCLUDED_NTCD_PROACTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_proactorconfig.h>
#include <ntccfg_platform.h>
#include <ntcd_datagramsocket.h>
#include <ntcd_listenersocket.h>
#include <ntcd_streamsocket.h>
#include <ntci_datagramsocket.h>
#include <ntci_datagramsocketfactory.h>
#include <ntci_listenersocket.h>
#include <ntci_listenersocketfactory.h>
#include <ntci_proactor.h>
#include <ntci_proactorfactory.h>
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
/// Provide a proactor for simulated sockets.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class Proactor : public ntci::Proactor,
                 public ntcs::Driver,
                 public ntccfg::Shared<Proactor>
{
    /// This typedef defines a set of waiters.
    typedef bsl::unordered_set<ntci::Waiter> WaiterSet;

    struct Result;
    // This struct describes the context of a waiter.

    struct Work;
    // This struct describes the work pending for a socket.

    enum UpdateType {
        // Enumerates the types of update.

        e_INCLUDE = 1,
        // The device is being modified to gain interest in certain
        // events.

        e_EXCLUDE = 2
        // The device is being modified to lose interest in certain
        // events.
    };

    /// This typedef defines a map of work pending for the socket.
    typedef bsl::unordered_map<ntsa::Handle, bsl::shared_ptr<Work> > WorkMap;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    bsl::shared_ptr<ntcd::Machine>               d_machine_sp;
    bsl::shared_ptr<ntcd::Monitor>               d_monitor_sp;
    bsl::shared_ptr<ntci::User>                  d_user_sp;
    bsl::shared_ptr<ntci::DataPool>              d_dataPool_sp;
    bsl::shared_ptr<ntci::Resolver>              d_resolver_sp;
    bsl::shared_ptr<ntcs::Chronology>            d_chronology_sp;
    bsl::shared_ptr<ntci::Reservation>           d_connectionLimiter_sp;
    bsl::shared_ptr<ntci::ProactorMetrics>       d_metrics_sp;
    bsl::shared_ptr<ntci::DatagramSocketFactory> d_datagramSocketFactory_sp;
    bsl::shared_ptr<ntci::ListenerSocketFactory> d_listenerSocketFactory_sp;
    bsl::shared_ptr<ntci::StreamSocketFactory>   d_streamSocketFactory_sp;
    bsl::shared_ptr<ntcs::RegistryEntryCatalog::EntryFunctor>
                                                d_detachFunctor_sp;
    bsl::shared_ptr<ntcs::RegistryEntryCatalog> d_registry_sp;
    mutable Mutex                               d_waiterSetMutex;
    WaiterSet                                   d_waiterSet;
    mutable Mutex                               d_workMapMutex;
    WorkMap                                     d_workMap;
    bslmt::ThreadUtil::Handle                   d_threadHandle;
    bsl::size_t                                 d_threadIndex;
    bool                                        d_dynamic;
    bsls::AtomicUint64                          d_load;
    bsls::AtomicBool                            d_run;
    ntca::ProactorConfig                        d_config;
    bslma::Allocator*                           d_allocator_p;

  private:
    Proactor(const Proactor&) BSLS_KEYWORD_DELETED;
    Proactor& operator=(const Proactor&) BSLS_KEYWORD_DELETED;

  private:
    /// Initialize this object.
    void initialize();

    /// Execute all pending jobs.
    void flush();

    /// Acquire usage of the most suitable proactor selected according to
    /// the specified load balancing 'options'.
    bsl::shared_ptr<ntci::Proactor> acquireProactor(
        const ntca::LoadBalancingOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Release usage of the specified 'proactor' selected according to the
    /// specified load balancing 'options'.
    void releaseProactor(const bsl::shared_ptr<ntci::Proactor>& proactor,
                         const ntca::LoadBalancingOptions&      options)
        BSLS_KEYWORD_OVERRIDE;

    /// Increment the current number of handle reservations, if permitted.
    /// Return true if the resulting number of handle reservations is
    /// permitted, and false otherwise.
    bool acquireHandleReservation() BSLS_KEYWORD_OVERRIDE;

    /// Remove the specified 'entry' from the device and announce its
    /// detachment if possible. Return the error.
    ntsa::Error removeDetached(
        const bsl::shared_ptr<ntcs::RegistryEntry>& entry);

    /// Decrement the current number of handle reservations.
    void releaseHandleReservation() BSLS_KEYWORD_OVERRIDE;

    /// Return the number of proactors in the thread pool.
    bsl::size_t numProactors() const BSLS_KEYWORD_OVERRIDE;

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
    explicit Proactor(const ntca::ProactorConfig&        configuration,
                      const bsl::shared_ptr<ntci::User>& user,
                      bslma::Allocator*                  basicAllocator = 0);

    /// Create a new test reactor having the specified 'configuration'
    /// operating in the environment of the specified 'user' implemented
    /// using sessions on the specified 'machine' . Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Proactor(const ntca::ProactorConfig&           configuration,
                      const bsl::shared_ptr<ntci::User>&    user,
                      const bsl::shared_ptr<ntcd::Machine>& machine,
                      bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Proactor();

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
    /// threads driving this proactor. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    bsl::shared_ptr<ntci::Strand> createStrand(
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Attach the specified 'socket' to the proactor. Return the
    /// error.
    ntsa::Error attachSocket(const bsl::shared_ptr<ntci::ProactorSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;

    /// Accept the next connection made to the specified 'socket' bound to
    /// the specified 'endpoint'. Return the error.
    ntsa::Error accept(const bsl::shared_ptr<ntci::ProactorSocket>& socket)
        BSLS_KEYWORD_OVERRIDE;

    /// Connect the specified 'socket' to the specified 'endpoint'. Return
    /// the error.
    ntsa::Error connect(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                        const ntsa::Endpoint& endpoint) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'data' to the send buffer of the specified
    /// 'socket' according to the specified 'options'. Return the error.
    /// Note that 'data' must not be modified or destroyed until the
    /// operation completes or fails.
    ntsa::Error send(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                     const bdlbb::Blob&                           data,
                     const ntsa::SendOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'data' to the send buffer of the specified
    /// 'socket' according to the specified 'options'. Return the error.
    /// Note that 'data' must not be modified or destroyed until the
    /// operation completes or fails.
    ntsa::Error send(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                     const ntsa::Data&                            data,
                     const ntsa::SendOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Dequeue from the receive buffer of the specified 'socket' into the
    /// specified 'data' according to the specified 'options'. Return the
    /// error. Note that 'data' must not be modified or destroyed until the
    /// operation completes or fails.
    ntsa::Error receive(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                        bdlbb::Blob*                                 data,
                        const ntsa::ReceiveOptions&                  options)
        BSLS_KEYWORD_OVERRIDE;

    /// Shutdown the stream socket in the specified 'direction'. Return the
    /// error.
    ntsa::Error shutdown(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                         ntsa::ShutdownType::Value direction)
        BSLS_KEYWORD_OVERRIDE;

    /// Cancel all outstanding operations initiated for the specified
    /// 'socket'. Return the error.
    ntsa::Error cancel(const bsl::shared_ptr<ntci::ProactorSocket>& socket)
        BSLS_KEYWORD_OVERRIDE;

    /// Detach the specified 'socket' from the proactor. Return the error.
    ntsa::Error detachSocket(const bsl::shared_ptr<ntci::ProactorSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;

    /// Close all monitored sockets and timers.
    ntsa::Error closeAll() BSLS_KEYWORD_OVERRIDE;

    /// Increment the estimation of the load on the proactor according to
    /// the specified load balancing 'options'.
    void incrementLoad(const ntca::LoadBalancingOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Decrement the estimation of the load on the proactor according to
    /// the specified load balancing 'options'.
    void decrementLoad(const ntca::LoadBalancingOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Block the calling thread until stopped. As each previously initiated
    /// operation completes, or each timer fires, invoke the corresponding
    /// processing function on the associated descriptor or timer. The
    /// behavior is undefined unless the calling thread has previously
    /// registered the 'waiter'. Note that after this function returns, the
    /// 'restart' function must be called before this or the 'run' function
    /// can be called again.
    void run(ntci::Waiter waiter) BSLS_KEYWORD_OVERRIDE;

    /// Block the calling thread identified by the specified 'waiter', until
    /// at least one socket enters the state in which interest has been
    /// registered, or timer fires. For each socket that has entered the
    /// state in which interest has been registered, or each timer that has
    /// fired, invoke the corresponding processing function on the
    /// associated descriptor or timer. The behavior is undefined unless the
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

    /// Return the number of sockets currently being monitored.
    bsl::size_t numSockets() const BSLS_KEYWORD_OVERRIDE;

    /// Return the maximum number of sockets capable of being monitored
    /// at one time.
    bsl::size_t maxSockets() const BSLS_KEYWORD_OVERRIDE;

    /// Return the number of timers currently being monitored.
    bsl::size_t numTimers() const BSLS_KEYWORD_OVERRIDE;

    /// Return the maximum number of timers capable of being monitored
    /// at one time.
    bsl::size_t maxTimers() const BSLS_KEYWORD_OVERRIDE;

    /// Return the estimation of the load on the proactor.
    bsl::size_t load() const BSLS_KEYWORD_OVERRIDE;

    /// Return the handle of the thread that drives this proactor, or
    /// the default value if no such thread has been set.
    bslmt::ThreadUtil::Handle threadHandle() const BSLS_KEYWORD_OVERRIDE;

    /// Return the index in the thread pool of the thread that drives this
    /// proactor, or 0 if no such thread has been set.
    bsl::size_t threadIndex() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current number of registered waiters.
    bsl::size_t numWaiters() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if the reactor has no pending deferred functors no
    /// pending timers, and no registered sockets, otherwise return false.
    bool empty() const BSLS_KEYWORD_OVERRIDE;

    /// Return the data pool.
    const bsl::shared_ptr<ntci::DataPool>& dataPool() const
        BSLS_KEYWORD_OVERRIDE;

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
/// Provide a factory to produce proactors for simulated sockets.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class ProactorFactory : public ntci::ProactorFactory
{
    bslma::Allocator* d_allocator_p;

  private:
    ProactorFactory(const ProactorFactory&) BSLS_KEYWORD_DELETED;
    ProactorFactory& operator=(const ProactorFactory&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new proactor factory that produces proactors for simulated
    /// sockets. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit ProactorFactory(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ProactorFactory() BSLS_KEYWORD_OVERRIDE;

    /// Create a new proactor with the specified 'configuration' operating
    /// in the environment of the specified 'user'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    bsl::shared_ptr<ntci::Proactor> createProactor(
        const ntca::ProactorConfig&        configuration,
        const bsl::shared_ptr<ntci::User>& user,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace
#endif
