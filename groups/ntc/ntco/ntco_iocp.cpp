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

#include <ntco_iocp.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntco_iocp_cpp, "$Id$ $CSID$")

#if NTC_BUILD_WITH_IOCP
#if defined(BSLS_PLATFORM_OS_WINDOWS)

#include <ntcp_datagramsocket.h>
#include <ntcp_listenersocket.h>
#include <ntcp_streamsocket.h>

#include <ntcm_monitorableregistry.h>
#include <ntcm_monitorableutil.h>

#include <ntci_log.h>
#include <ntcs_async.h>
#include <ntcs_authorization.h>
#include <ntcs_chronology.h>
#include <ntcs_datapool.h>
#include <ntcs_driver.h>
#include <ntcs_event.h>
#include <ntcs_nomenclature.h>
#include <ntcs_proactordetachcontext.h>
#include <ntcs_proactormetrics.h>
#include <ntcs_registry.h>
#include <ntcs_reservation.h>
#include <ntcs_strand.h>
#include <ntcs_user.h>
#include <ntsf_system.h>
#include <ntsu_socketoptionutil.h>
#include <ntsu_socketutil.h>

#include <bdlbb_blob.h>
#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bdlt_datetime.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_keyword.h>

#include <bsl_functional.h>
#include <bsl_iosfwd.h>
#include <bsl_list.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_utility.h>

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
#include <ws2def.h>
// Unix Domain Socket requirements on Windows:
//
// 1) Support for the AF_UNIX socket driver can be detected by running
// 'sc query afunix' from the command line with Administrator privileges.
//
// 2) The sockaddr_un structure is declared in <afunix.h>. It is not known
// if this header is present in all platform SDK versions on the supported
// operating systems. For now, do not include the <afunix.h> header to allow
// compilation on machines not having the platform SDK version that contains
// that header. The sole necessary structure, sockaddr_un, is "manually"
// redeclared, below.
//
// 2) When an AF_UNIX socket is created, C:\Windows\System32\wshunix.dll is
// dynamically loaded. If this DLL is missing, it is not currently known if
// the either the WSASocket function call simply fails or if some other, more
// drastic negative consequence to the process occurs.
// clang-format on
#ifdef interface
#undef interface
#endif
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "mswsock")
#pragma warning(disable : 4018)

// Declare the sockaddr_un structure "manually" to allow compilation on
// machines not having the platform SDK version that contains the <afunix.h>
// header.
#define SOCKADDR_UN_WIN32_PATH_MAX 108
struct sockaddr_un_win32 {
    ADDRESS_FAMILY sun_family;
    char           sun_path[SOCKADDR_UN_WIN32_PATH_MAX];
};
#define sockaddr_un sockaddr_un_win32
#endif

// Uncomment to announce the completion of operations that have been
// cancelled using 'ntcs::Driver::cancel()'. The contract for this function
// currently states that pending operations are not explicitly announced to
// have been cancelled as a result of this function.
// #define NTCP_IOCP_ANNOUNCE_CANCELLATIONS 1

#define NTCP_IOCP_LOG_WAIT_INDEFINITE()                                       \
    NTCI_LOG_TRACE("Polling for socket events indefinitely")

#define NTCP_IOCP_LOG_WAIT_TIMED(timeout)                                     \
    NTCI_LOG_TRACE(                                                           \
        "Polling for sockets events or until %d milliseconds have elapsed",   \
        timeout)

#define NTCP_IOCP_LOG_WAIT_FAILURE(error)                                     \
    NTCI_LOG_ERROR("Failed to poll for socket events: %s",                    \
                   error.text().c_str())

#define NTCP_IOCP_LOG_WAIT_TIMEOUT()                                          \
    NTCI_LOG_TRACE("Timed out polling for socket events")

#define NTCP_IOCP_LOG_EVENT_STATUS(event, status)                             \
    do {                                                                      \
        if (event->d_type == ntcs::EventType::e_CALLBACK) {                   \
            NTCI_LOG_TRACE("Event %p type %s %s",                             \
                           event.get(),                                       \
                           ntcs::EventType::toString(event->d_type),          \
                           status);                                           \
        }                                                                     \
        else {                                                                \
            NTCI_LOG_TRACE(                                                   \
                "Event %p type %s for socket %d %s",                          \
                event.get(),                                                  \
                ntcs::EventType::toString(event->d_type),                     \
                (int)(event->d_socket ? event->d_socket->handle() : -1),      \
                status);                                                      \
        }                                                                     \
    } while (false)

#define NTCP_IOCP_LOG_EVENT_STARTING(event)                                   \
    NTCP_IOCP_LOG_EVENT_STATUS(event, "starting")

#define NTCP_IOCP_LOG_EVENT_COMPLETE(event)                                   \
    NTCP_IOCP_LOG_EVENT_STATUS(event, "complete")

#define NTCP_IOCP_LOG_EVENT_CANCELLED(event)                                  \
    NTCP_IOCP_LOG_EVENT_STATUS(event, "cancelled")

#define NTCP_IOCP_LOG_EVENT_ABANDONED(event)                                  \
    NTCP_IOCP_LOG_EVENT_STATUS(event, "abandoned")

#define NTCP_IOCP_LOG_EVENT_IGNORED(event)                                    \
    NTCP_IOCP_LOG_EVENT_STATUS(event, "ignored")

namespace BloombergLP {
namespace ntco {

namespace {

// The maximum number of iovec buffers to submit to a call to 'WSASend' or
// 'WSARecv'.
#define NTCP_COMPLETIONPORT_WSABUF_MAX 64

void noop()
{
}

}  // close unnamed namespace

// Provide a proactor context for an implementation of the 'ntci::Proactor'
// interface implemented using the I/O completion port API.
class IocpContext : public ntcs::ProactorDetachContext
{
  private:
    IocpContext(const IocpContext&) BSLS_KEYWORD_DELETED;
    IocpContext& operator=(const IocpContext&) BSLS_KEYWORD_DELETED;

  public:
    // Create a new I/O completion port context.
    IocpContext();

    // Destroy this object.
    ~IocpContext();
};

IocpContext::IocpContext()
: ntcs::ProactorDetachContext()
{
}

IocpContext::~IocpContext()
{
}

class Iocp : public ntci::Proactor,
             public ntcs::Driver,
             public ntccfg::Shared<Iocp>
{
    // Provide an implementation of the 'ntci::Proactor' interface
    // implemented using the I/O completion port API. This class is thread
    // safe.

    typedef bsl::unordered_set<ntci::Waiter> WaiterSet;
    // This typedef defines a set of waiters.

    typedef bsl::unordered_map<ntsa::Handle,
                               bsl::shared_ptr<ntci::ProactorSocket> >
        ProactorSocketMap;
    // Define a type alias for a map of proactive handles
    // to descriptors.

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

    ntccfg::Object                         d_object;
    HANDLE                                 d_completionPort;
    ntcs::EventPool                        d_eventPool;
    ntcs::Chronology                       d_chronology;
    bsl::shared_ptr<ntci::User>            d_user_sp;
    bsl::shared_ptr<ntci::DataPool>        d_dataPool_sp;
    bsl::shared_ptr<ntci::Resolver>        d_resolver_sp;
    bsl::shared_ptr<ntci::Reservation>     d_connectionLimiter_sp;
    bsl::shared_ptr<ntci::ProactorMetrics> d_metrics_sp;
    mutable Mutex                          d_proactorSocketMapMutex;
    ProactorSocketMap                      d_proactorSocketMap;
    mutable Mutex                          d_waiterSetMutex;
    WaiterSet                              d_waiterSet;
    bslmt::ThreadUtil::Handle              d_threadHandle;
    bsl::size_t                            d_threadIndex;
    bsls::AtomicUint64                     d_threadId;
    bool                                   d_dynamic;
    bsls::AtomicUint64                     d_load;
    bsls::AtomicBool                       d_run;
    ntca::ProactorConfig                   d_config;
    bslma::Allocator*                      d_allocator_p;

  private:
    Iocp(const Iocp&) BSLS_KEYWORD_DELETED;
    Iocp& operator=(const Iocp&) BSLS_KEYWORD_DELETED;

  private:
    ntsa::Error submit(bslma::ManagedPtr<ntcs::Event>& event);
    // Push the specified 'event' to the completion port, blocking
    // if the queue is full until an event has been popped by another
    // thread. Return the error.

    void flush();
    // Execute all pending jobs.

    void wait(ntci::Waiter waiter);
    // Block the calling thread, identified by the specified 'waiter',
    // until any registered events for any descriptor in the polling set
    // occurs, or the earliest due timer in the specified 'chronology'
    // elapses, if any.  For each event that has occurred, invoke the
    // correspond processing function on the associated descriptor. Note
    // that implementations are permitted to wakeup from 'timeout'
    // prematurely. The behavior is undefined unless the calling thread
    // has previously registered the 'waiter'.

    bsl::shared_ptr<ntci::Proactor> acquireProactor(
        const ntca::LoadBalancingOptions& options) BSLS_KEYWORD_OVERRIDE;
    // Acquire usage of the most suitable proactor selected according to
    // the specified load balancing 'options'.

    void releaseProactor(const bsl::shared_ptr<ntci::Proactor>& proactor,
                         const ntca::LoadBalancingOptions&      options)
        BSLS_KEYWORD_OVERRIDE;
    // Release usage of the specified 'proactor' selected according to the
    // specified load balancing 'options'.

    bool acquireHandleReservation() BSLS_KEYWORD_OVERRIDE;
    // Increment the current number of handle reservations, if permitted.
    // Return true if the resulting number of handle reservations is
    // permitted, and false otherwise.

    void releaseHandleReservation() BSLS_KEYWORD_OVERRIDE;
    // Decrement the current number of handle reservations.

    bool isWaiter() const;
    // Return true if the current thread is the principle waiter, i.e.,
    // the principle I/O thread in a statically load-balanced
    // configuration, otherwise return false.

    bsl::size_t numProactors() const BSLS_KEYWORD_OVERRIDE;
    // Return the number of proactors in the thread pool.

    bsl::size_t numThreads() const BSLS_KEYWORD_OVERRIDE;
    // Return the current number of threads in the thread pool.

    bsl::size_t minThreads() const BSLS_KEYWORD_OVERRIDE;
    // Return the minimum number of threads in the thread pool.

    bsl::size_t maxThreads() const BSLS_KEYWORD_OVERRIDE;
    // Return the maximum number of threads in the thread pool.

  public:
    typedef NTCCFG_FUNCTION() Functor;
    // Define a type alias for a deferred function.

    explicit Iocp(const ntca::ProactorConfig&        configuration,
                  const bsl::shared_ptr<ntci::User>& user,
                  bslma::Allocator*                  basicAllocator = 0);
    // Create a new proactor having the specified 'configuration' operating
    // in the environment of the specified 'user'. Optionally specify a
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    // the currently installed default allocator is used.

    ~Iocp() BSLS_KEYWORD_OVERRIDE;
    // Destroy this object.

    ntci::Waiter registerWaiter(const ntca::WaiterOptions& waiterOptions)
        BSLS_KEYWORD_OVERRIDE;
    // Register a thread described by the specified 'waiterOptions' that
    // will drive this object. Return the handle to the waiter.

    void deregisterWaiter(ntci::Waiter waiter) BSLS_KEYWORD_OVERRIDE;
    // Deregister the specified 'waiter'.

    bsl::shared_ptr<ntci::Strand> createStrand(
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;
    // Create a new strand to serialize execution of functors by the
    // threads driving this proactor. Optionally specify a 'basicAllocator'
    // used to supply memory. If 'basicAllocator' is 0, the currently
    // installed default allocator is used.

    ntsa::Error attachSocket(const bsl::shared_ptr<ntci::ProactorSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;
    // Attach the specified 'socket' to the proactor. Return the
    // error.

    ntsa::Error accept(const bsl::shared_ptr<ntci::ProactorSocket>& socket)
        BSLS_KEYWORD_OVERRIDE;
    // Accept the next connection made to the specified 'socket' bound to
    // the specified 'endpoint'. Return the error.

    ntsa::Error connect(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                        const ntsa::Endpoint& endpoint) BSLS_KEYWORD_OVERRIDE;
    // Connect the specified 'socket' to the specified 'endpoint'. Return
    // the error.

    ntsa::Error send(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                     const bdlbb::Blob&                           data,
                     const ntsa::SendOptions& options) BSLS_KEYWORD_OVERRIDE;
    // Enqueue the specified 'data' to the send buffer of the specified
    // 'socket' according to the specified 'options'. Return the error.
    // Note that 'data' must not be modified or destroyed until the
    // operation completes or fails.

    ntsa::Error send(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                     const ntsa::Data&                            data,
                     const ntsa::SendOptions& options) BSLS_KEYWORD_OVERRIDE;
    // Enqueue the specified 'data' to the send buffer of the specified
    // 'socket' according to the specified 'options'. Return the error.
    // Note that 'data' must not be modified or destroyed until the
    // operation completes or fails.

    ntsa::Error receive(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                        bdlbb::Blob*                                 data,
                        const ntsa::ReceiveOptions&                  options)
        BSLS_KEYWORD_OVERRIDE;
    // Dequeue from the receive buffer of the specified 'socket' into the
    // specified 'data' according to the specified 'options'. Return the
    // error. Note that 'data' must not be modified or destroyed until the
    // operation completes or fails.

    ntsa::Error shutdown(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                         ntsa::ShutdownType::Value direction)
        BSLS_KEYWORD_OVERRIDE;
    // Shutdown the stream socket in the specified 'direction'. Return the
    // error.

    ntsa::Error cancel(const bsl::shared_ptr<ntci::ProactorSocket>& socket)
        BSLS_KEYWORD_OVERRIDE;
    // Cancel all outstanding operations initiated for the specified
    // 'socket'. Return the error.

    ntsa::Error detachSocket(const bsl::shared_ptr<ntci::ProactorSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;
    // Detach the specified 'socket' from the proactor. Return the error.

    ntsa::Error closeAll() BSLS_KEYWORD_OVERRIDE;
    // Close all monitored sockets and timers.

    void incrementLoad(const ntca::LoadBalancingOptions& options)
        BSLS_KEYWORD_OVERRIDE;
    // Increment the estimation of the load on the proactor according to
    // the specified load balancing 'options'.

    void decrementLoad(const ntca::LoadBalancingOptions& options)
        BSLS_KEYWORD_OVERRIDE;
    // Decrement the estimation of the load on the proactor according to
    // the specified load balancing 'options'.

    void run(ntci::Waiter waiter) BSLS_KEYWORD_OVERRIDE;
    // Block the calling thread until stopped. As each previously initiated
    // operation completes, or each timer fires, invoke the corresponding
    // processing function on the associated descriptor or timer. The
    // behavior is undefined unless the calling thread has previously
    // registered the 'waiter'. Note that after this function returns, the
    // 'restart' function must be called before this or the 'run' function
    // can be called again.

    void poll(ntci::Waiter waiter) BSLS_KEYWORD_OVERRIDE;
    // Block the calling thread identified by the specified 'waiter', until
    // at least one socket enters the state in which interest has been
    // registered, or timer fires. For each socket that has entered the
    // state in which interest has been registered, or each timer that has
    // fired, invoke the corresponding processing function on the
    // associated descriptor or timer. The behavior is undefined unless the
    // calling thread has previously registered the 'waiter'. Note that
    // if this function returns because 'stop' was called, the 'restart'
    // function must be called before this or the 'run' function can be
    // called again.

    void interruptOne() BSLS_KEYWORD_OVERRIDE;
    // Unblock and return one caller blocked on either 'poll' or 'run'.

    void interruptAll() BSLS_KEYWORD_OVERRIDE;
    // Unblock and return any caller blocked on either 'poll' or 'run'.

    void stop() BSLS_KEYWORD_OVERRIDE;
    // Unblock and return any caller blocked on either 'poll' or 'run'.

    void restart() BSLS_KEYWORD_OVERRIDE;
    // Prepare the reactor for 'run' to be called again after previously
    // being stopped.

    void drainFunctions() BSLS_KEYWORD_OVERRIDE;
    // Execute all deferred functions managed by this object.

    void clearFunctions() BSLS_KEYWORD_OVERRIDE;
    // Clear all deferred functions managed by this object.

    void clearTimers() BSLS_KEYWORD_OVERRIDE;
    // Clear all timers managed by this object.

    void clearSockets() BSLS_KEYWORD_OVERRIDE;
    // Clear all sockets managed by this object.

    void clear() BSLS_KEYWORD_OVERRIDE;
    // Clear all resources managed by this object.

    void execute(const Functor& functor) BSLS_KEYWORD_OVERRIDE;
    // Defer the execution of the specified 'functor'.

    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE;
    // Atomically defer the execution of the specified 'functorSequence'
    // immediately followed by the specified 'functor', then clear the
    // 'functorSequence'.

    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&                  options,
        const bsl::shared_ptr<ntci::TimerSession>& session,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;
    // Create a new timer according to the specified 'options' that invokes
    // the specified 'session' for each timer event on this object's
    // 'strand()', if defined, or on an unspecified thread otherwise.
    // Optionally specify a 'basicAllocator' used to supply memory. If
    // 'basicAllocator' is 0, the currently installed default allocator is
    // used.

    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&  options,
        const ntci::TimerCallback& callback,
        bslma::Allocator*          basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;
    // Create a new timer according to the specified 'options' that invokes
    // the specified 'callback' for each timer event on this object's
    // 'strand()', if defined, or on an unspecified thread otherwise.
    // Optionally specify a 'basicAllocator' used to supply memory. If
    // 'basicAllocator' is 0, the currently installed default allocator is
    // used.

    bsl::shared_ptr<ntci::DatagramSocket> createDatagramSocket(
        const ntca::DatagramSocketOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;
    // Create a new datagram socket with the specified 'options'.
    // Optionally specify a 'basicAllocator' used to supply memory. If
    // 'basicAllocator' is 0, the currently installed default allocator is
    // used. Return the error.

    bsl::shared_ptr<ntci::ListenerSocket> createListenerSocket(
        const ntca::ListenerSocketOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;
    // Create a new listener socket with the specified 'options'.
    // Optionally specify a 'basicAllocator' used to supply memory. If
    // 'basicAllocator' is 0, the currently installed default allocator is
    // used. Return the error.

    bsl::shared_ptr<ntci::StreamSocket> createStreamSocket(
        const ntca::StreamSocketOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;
    // Create a new stream socket with the specified 'options'.
    // Optionally specify a 'basicAllocator' used to supply memory. If
    // 'basicAllocator' is 0, the currently installed default allocator is
    // used. Return the error.

    bsl::shared_ptr<ntsa::Data> createIncomingData() BSLS_KEYWORD_OVERRIDE;
    // Return a shared pointer to a data container suitable for storing
    // incoming data. The resulting data container is automatically
    // returned to this pool when its reference count reaches zero.

    bsl::shared_ptr<ntsa::Data> createOutgoingData() BSLS_KEYWORD_OVERRIDE;
    // Return a shared pointer to a data container suitable for storing
    // outgoing data. The resulting data container is automatically
    // returned to this pool when its reference count reaches zero.

    bsl::shared_ptr<bdlbb::Blob> createIncomingBlob() BSLS_KEYWORD_OVERRIDE;
    // Return a shared pointer to a blob suitable for storing incoming
    // data. The resulting blob is automatically returned to this pool
    // when its reference count reaches zero.

    bsl::shared_ptr<bdlbb::Blob> createOutgoingBlob() BSLS_KEYWORD_OVERRIDE;
    // Return a shared pointer to a blob suitable for storing incoming
    // data. The resulting blob is automatically returned to this pool
    // when its reference count reaches zero.

    void createIncomingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
        BSLS_KEYWORD_OVERRIDE;
    // Load into the specified 'blobBuffer' the data and size of a new
    // buffer allocated from the incoming blob buffer factory.

    void createOutgoingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
        BSLS_KEYWORD_OVERRIDE;
    // Load into the specified 'blobBuffer' the data and size of a new
    // buffer allocated from the outgoing blob buffer factory.

    bsl::size_t numSockets() const BSLS_KEYWORD_OVERRIDE;
    // Return the number of sockets currently being monitored.

    bsl::size_t maxSockets() const BSLS_KEYWORD_OVERRIDE;
    // Return the maximum number of sockets capable of being monitored
    // at one time.

    bsl::size_t numTimers() const BSLS_KEYWORD_OVERRIDE;
    // Return the number of timers currently being monitored.

    bsl::size_t maxTimers() const BSLS_KEYWORD_OVERRIDE;
    // Return the maximum number of timers capable of being monitored
    // at one time.

    bsl::size_t load() const BSLS_KEYWORD_OVERRIDE;
    // Return the estimation of the load on the proactor.

    bslmt::ThreadUtil::Handle threadHandle() const BSLS_KEYWORD_OVERRIDE;
    // Return the handle of the thread that drives this proactor, or
    // the default value if no such thread has been set.

    bsl::size_t threadIndex() const BSLS_KEYWORD_OVERRIDE;
    // Return the index in the thread pool of the thread that drives this
    // proactor, or 0 if no such thread has been set.

    bsl::size_t numWaiters() const BSLS_KEYWORD_OVERRIDE;
    // Return the current number of registered waiters.

    bool empty() const BSLS_KEYWORD_OVERRIDE;
    // Return true if the reactor has no pending deferred functors no
    // pending timers, and no registered sockets, otherwise return false.

    const bsl::shared_ptr<ntci::DataPool>& dataPool() const
        BSLS_KEYWORD_OVERRIDE;
    // Return the data pool.

    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;
    // Return the strand that guarantees sequential, non-current execution
    // of arbitrary functors on the unspecified threads processing events
    // for this object.

    bsls::TimeInterval currentTime() const BSLS_KEYWORD_OVERRIDE;
    // Return the current elapsed time since the Unix epoch.

    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& incomingBlobBufferFactory()
        const BSLS_KEYWORD_OVERRIDE;
    // Return the incoming blob buffer factory.

    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& outgoingBlobBufferFactory()
        const BSLS_KEYWORD_OVERRIDE;
    // Return the outgoing blob buffer factory.

    const char* name() const BSLS_KEYWORD_OVERRIDE;
    // Return the name of the driver.
};

struct Iocp::Result {
    // This struct describes the context of a waiter.

  public:
    ntca::WaiterOptions                    d_options;
    bsl::shared_ptr<ntci::ProactorMetrics> d_metrics_sp;

  private:
    Result(const Result&) BSLS_KEYWORD_DELETED;
    Result& operator=(const Result&) BSLS_KEYWORD_DELETED;

  public:
    explicit Result(bslma::Allocator* basicAllocator = 0);
    // Create a new proactor result. Optionally specify a 'basicAllocator'
    // used to supply memory. If 'basicAllocator' is 0, the currently
    // installed default allocator is used.

    ~Result();
    // Destroy this object.
};

Iocp::Result::Result(bslma::Allocator* basicAllocator)
: d_options(basicAllocator)
, d_metrics_sp()
{
}

Iocp::Result::~Result()
{
}

ntsa::Error Iocp::submit(bslma::ManagedPtr<ntcs::Event>& event)
{
    BOOL success =
        PostQueuedCompletionStatus(d_completionPort,
                                   0,
                                   0,
                                   reinterpret_cast<OVERLAPPED*>(event.get()));

    if (success == FALSE) {
        return ntsa::Error(GetLastError());
    }

    event.release();

    return ntsa::Error();
}

void Iocp::flush()
{
    // Ensure that all pending jobs are executed. This prevents sockets from
    // being still bound to functors in the pending operation queue of the
    // I/O completion port, and their reference counts never decremented,
    // resulting in those sockets never being destroyed, causing a memory leak.

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (d_chronology.hasAnyScheduledOrDeferred()) {
        d_chronology.announce(d_dynamic);
    }

    while (true) {
        bslma::ManagedPtr<ntcs::Event> event;

        DWORD       lastError  = 0;
        DWORD       numBytes   = 0;
        ULONG_PTR   key        = 0;
        OVERLAPPED* overlapped = 0;

        SetLastError(0);

        BOOL getQueueCompletionStatusResult =
            GetQueuedCompletionStatus(d_completionPort,
                                      &numBytes,
                                      &key,
                                      &overlapped,
                                      0);

        lastError = GetLastError();

        if (!getQueueCompletionStatusResult) {
            if (overlapped) {
                error = ntsa::Error(lastError);
            }
            else if (lastError == WAIT_TIMEOUT) {
                break;
            }
            else {
                error = ntsa::Error(lastError);
                NTCP_IOCP_LOG_WAIT_FAILURE(error);
                break;
            }
        }
        else {
            error = ntsa::Error();
        }

        BSLS_ASSERT(overlapped);
        BSLS_ASSERT(numBytes >= 0);

        ntcs::Event* eventRaw = reinterpret_cast<ntcs::Event*>(overlapped);
        event.load(eventRaw, &d_eventPool);

        if (event->d_type == ntcs::EventType::e_ACCEPT) {
            BSLS_ASSERT(event->d_socket.get() != 0);
            BSLS_ASSERT(event->d_target != ntsa::k_INVALID_HANDLE);

            ntsf::System::close(event->d_target);
        }

        if (error && error == ntsa::Error::e_CANCELLED) {
            BSLS_ASSERT(lastError == ERROR_OPERATION_ABORTED);
            NTCP_IOCP_LOG_EVENT_CANCELLED(event);
            continue;
        }

        NTCP_IOCP_LOG_EVENT_ABANDONED(event);
    }

    if (d_chronology.hasAnyScheduledOrDeferred()) {
        do {
            d_chronology.announce(d_dynamic);
        } while (d_chronology.hasAnyDeferred());
    }
}

void Iocp::wait(ntci::Waiter waiter)
{
    NTCCFG_WARNING_UNUSED(waiter);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bslma::ManagedPtr<ntcs::Event> event;

    DWORD       lastError  = 0;
    DWORD       numBytes   = 0;
    ULONG_PTR   key        = 0;
    OVERLAPPED* overlapped = 0;

    DWORD milliseconds;

    int timeout = d_chronology.timeoutInMilliseconds();

    if (timeout >= 0) {
        milliseconds = timeout;
        NTCP_IOCP_LOG_WAIT_TIMED(milliseconds);
    }
    else {
        milliseconds = INFINITE;
        NTCP_IOCP_LOG_WAIT_INDEFINITE();
    }

    SetLastError(0);

    BOOL getQueueCompletionStatusResult =
        GetQueuedCompletionStatus(d_completionPort,
                                  &numBytes,
                                  &key,
                                  &overlapped,
                                  milliseconds);

    lastError = GetLastError();

    if (!getQueueCompletionStatusResult) {
        if (overlapped) {
            error = ntsa::Error(lastError);
        }
        else if (lastError == WAIT_TIMEOUT) {
            NTCP_IOCP_LOG_WAIT_TIMEOUT();
            return;
        }
        else {
            error = ntsa::Error(lastError);
            NTCP_IOCP_LOG_WAIT_FAILURE(error);
            return;
        }
    }

    BSLS_ASSERT(overlapped);
    BSLS_ASSERT(numBytes >= 0);

    ntcs::Event* eventRaw = reinterpret_cast<ntcs::Event*>(overlapped);
    event.load(eventRaw, &d_eventPool);

    if (error && error == ntsa::Error::e_CANCELLED) {
        BSLS_ASSERT(lastError == ERROR_OPERATION_ABORTED);
        NTCP_IOCP_LOG_EVENT_CANCELLED(event);
        return;
    }

    NTCP_IOCP_LOG_EVENT_COMPLETE(event);

    if (event->d_type == ntcs::EventType::e_CALLBACK) {
        event->d_function();
        return;
    }

    if (event->d_type == ntcs::EventType::e_ACCEPT) {
        BSLS_ASSERT(event->d_socket.get() != 0);
        BSLS_ASSERT(event->d_target != ntsa::k_INVALID_HANDLE);

        if (error) {
            ntsf::System::close(event->d_target);

            ntcs::Dispatch::announceAccepted(
                event->d_socket,
                error,
                bsl::shared_ptr<ntsi::StreamSocket>(),
                event->d_socket->strand());
            return;
        }

        SOCKET listenerHandle = event->d_socket->handle();

        SetLastError(0);

        int updateAcceptContextResult =
            setsockopt(event->d_target,
                       SOL_SOCKET,
                       SO_UPDATE_ACCEPT_CONTEXT,
                       reinterpret_cast<char*>(&listenerHandle),
                       sizeof(SOCKET));

        lastError = GetLastError();

        if (updateAcceptContextResult != 0) {
            BSLS_ASSERT(updateAcceptContextResult == SOCKET_ERROR);
            error = ntsa::Error(lastError);

            BSLS_LOG_ERROR(
                "Accepted socket %d failed to update accept context: %s",
                event->d_target,
                error.text().c_str());

            ntsf::System::close(event->d_target);

            ntcs::Dispatch::announceAccepted(
                event->d_socket,
                error,
                bsl::shared_ptr<ntsi::StreamSocket>(),
                event->d_socket->strand());
            return;
        }

        bsl::shared_ptr<ntsi::StreamSocket> streamSocket =
            ntsf::System::createStreamSocket(event->d_target, d_allocator_p);

        ntcs::Dispatch::announceAccepted(event->d_socket,
                                         ntsa::Error(),
                                         streamSocket,
                                         event->d_socket->strand());
        return;
    }
    else if (event->d_type == ntcs::EventType::e_CONNECT) {
        BSLS_ASSERT(event->d_socket.get() != 0);

        if (error) {
            ntcs::Dispatch::announceConnected(event->d_socket,
                                              error,
                                              event->d_socket->strand());
            return;
        }

        SetLastError(0);

        int updateConnectContextResult = setsockopt(event->d_socket->handle(),
                                                    SOL_SOCKET,
                                                    SO_UPDATE_CONNECT_CONTEXT,
                                                    0,
                                                    0);

        lastError = GetLastError();

        if (updateConnectContextResult != 0) {
            BSLS_ASSERT(updateConnectContextResult == SOCKET_ERROR);
            error = ntsa::Error(lastError);

            BSLS_LOG_ERROR(
                "Connecting socket %d failed to update connect context: %s",
                event->d_target,
                error.text().c_str());

            ntcs::Dispatch::announceConnected(event->d_socket,
                                              error,
                                              event->d_socket->strand());
            return;
        }

        ntcs::Dispatch::announceConnected(event->d_socket,
                                          ntsa::Error(),
                                          event->d_socket->strand());
        return;
    }
    else if (event->d_type == ntcs::EventType::e_SEND) {
        BSLS_ASSERT(event->d_socket.get() != 0);

        ntsa::SendContext context;
        context.setBytesSendable(event->d_numBytesAttempted);

        if (error) {
            ntcs::Dispatch::announceSent(event->d_socket,
                                         error,
                                         context,
                                         event->d_socket->strand());
            return;
        }

        context.setBytesSent(numBytes);

        ntcs::Dispatch::announceSent(event->d_socket,
                                     error,
                                     context,
                                     event->d_socket->strand());
    }
    else if (event->d_type == ntcs::EventType::e_RECEIVE) {
        BSLS_ASSERT(event->d_socket.get() != 0);

        ntsa::ReceiveContext context;
        context.setBytesReceivable(event->d_numBytesAttempted);

        if (error) {
            ntcs::Dispatch::announceReceived(event->d_socket,
                                             error,
                                             context,
                                             event->d_socket->strand());
            return;
        }

        const bool wantedEndpoint = event->d_numBytesIndicated > 0;

        if (wantedEndpoint) {
            const bool haveEndpoint =
                event->d_numBytesIndicated <= sizeof(sockaddr_storage);

            if (haveEndpoint) {
                ntsa::Endpoint endpoint;
                error = ntsu::SocketUtil::decodeEndpoint(
                    &endpoint,
                    reinterpret_cast<const sockaddr_storage*>(
                        event->d_address),
                    static_cast<bsl::size_t>(event->d_numBytesIndicated));
                if (error) {
                    ntcs::Dispatch::announceReceived(
                        event->d_socket,
                        error,
                        context,
                        event->d_socket->strand());
                    return;
                }

                context.setEndpoint(endpoint);
            }
            else {
                ntsa::Endpoint endpoint;
                error =
                    ntsf::System::getRemoteEndpoint(&endpoint,
                                                    event->d_socket->handle());
                if (error) {
                    ntcs::Dispatch::announceReceived(
                        event->d_socket,
                        error,
                        context,
                        event->d_socket->strand());
                    return;
                }

                context.setEndpoint(endpoint);
            }
        }

        BSLS_ASSERT(event->d_receiveData_p);
        BSLS_ASSERT(event->d_receiveData_p->length() + numBytes <=
                    event->d_receiveData_p->totalSize());

        event->d_receiveData_p->setLength(event->d_receiveData_p->length() +
                                          numBytes);

        context.setBytesReceived(numBytes);

        ntcs::Dispatch::announceReceived(event->d_socket,
                                         error,
                                         context,
                                         event->d_socket->strand());
    }
    else {
        NTCP_IOCP_LOG_EVENT_IGNORED(event);
        return;
    }
}

bsl::shared_ptr<ntci::Proactor> Iocp::acquireProactor(
    const ntca::LoadBalancingOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    return this->getSelf(this);
}

void Iocp::releaseProactor(const bsl::shared_ptr<ntci::Proactor>& proactor,
                           const ntca::LoadBalancingOptions&      options)
{
    BSLS_ASSERT(proactor == this->getSelf(this));
    proactor->decrementLoad(options);
}

bool Iocp::acquireHandleReservation()
{
    if (d_connectionLimiter_sp) {
        return d_connectionLimiter_sp->acquire();
    }
    else {
        return true;
    }
}

void Iocp::releaseHandleReservation()
{
    if (d_connectionLimiter_sp) {
        d_connectionLimiter_sp->release();
    }
}

NTCCFG_INLINE
bool Iocp::isWaiter() const
{
    return bslmt::ThreadUtil::selfIdAsUint64() == d_threadId.load();
}

bsl::size_t Iocp::numProactors() const
{
    return 1;
}

bsl::size_t Iocp::numThreads() const
{
    return this->numWaiters();
}

bsl::size_t Iocp::minThreads() const
{
    return d_config.minThreads().value();
}

bsl::size_t Iocp::maxThreads() const
{
    return d_config.maxThreads().value();
}

Iocp::Iocp(const ntca::ProactorConfig&        configuration,
           const bsl::shared_ptr<ntci::User>& user,
           bslma::Allocator*                  basicAllocator)
: d_object("ntco::Proactor")
, d_completionPort(0)
, d_eventPool(basicAllocator)
, d_chronology(this, basicAllocator)
, d_user_sp(user)
, d_dataPool_sp()
, d_resolver_sp()
, d_connectionLimiter_sp()
, d_metrics_sp()
, d_proactorSocketMapMutex()
, d_proactorSocketMap(basicAllocator)
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
        d_config.setMetricName(ntcs::Nomenclature::createProactorName());
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
        d_metrics_sp = d_user_sp->proactorMetrics();
    }

    if (d_user_sp) {
        bsl::shared_ptr<ntci::Chronology> chronology = d_user_sp->chronology();
        if (chronology) {
            d_chronology.setParent(chronology);
        }
    }

    d_completionPort = CreateIoCompletionPort(
        INVALID_HANDLE_VALUE,
        0,
        0,
        static_cast<DWORD>(d_config.maxThreads().value()));
    BSLS_ASSERT(d_completionPort != 0);
}

Iocp::~Iocp()
{
    // Assert all timers and functions are executed.

    BSLS_ASSERT_OPT(!d_chronology.hasAnyDeferred());
    BSLS_ASSERT_OPT(!d_chronology.hasAnyScheduled());
    BSLS_ASSERT_OPT(!d_chronology.hasAnyRegistered());

    // Assert all waiters are deregistered.

    BSLS_ASSERT_OPT(d_waiterSet.empty());

    BOOL rc = CloseHandle(d_completionPort);
    BSLS_ASSERT(rc != 0);
}

ntci::Waiter Iocp::registerWaiter(const ntca::WaiterOptions& waiterOptions)
{
    Iocp::Result* result = new (*d_allocator_p) Iocp::Result(d_allocator_p);

    result->d_options = waiterOptions;

    bdlb::NullableValue<bslmt::ThreadUtil::Handle> principleThreadHandle;

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

                bsl::shared_ptr<ntcs::ProactorMetrics> metrics;
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

void Iocp::deregisterWaiter(ntci::Waiter waiter)
{
    Iocp::Result* result = static_cast<Iocp::Result*>(waiter);

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

bsl::shared_ptr<ntci::Strand> Iocp::createStrand(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<Proactor> self = this->getSelf(this);

    bsl::shared_ptr<ntcs::Strand> strand;
    strand.createInplace(allocator, self, allocator);

    return strand;
}

ntsa::Error Iocp::attachSocket(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket)
{
    if (socket->handle() == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (0 == CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket->handle()),
                                    d_completionPort,
                                    0,
                                    0))
    {
        return ntsa::Error(GetLastError());
    }

    {
        LockGuard lockGuard(&d_proactorSocketMapMutex);

        bsl::pair<ProactorSocketMap::iterator, bool> insertResult =
            d_proactorSocketMap.insert(
                bsl::make_pair(socket->handle(), socket));
        if (!insertResult.second) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    bsl::shared_ptr<IocpContext> proactorContext;
    proactorContext.createInplace(d_allocator_p);

    BSLS_ASSERT(!socket->getProactorContext());
    socket->setProactorContext(proactorContext);

    return ntsa::Error();
}

ntsa::Error Iocp::accept(const bsl::shared_ptr<ntci::ProactorSocket>& socket)
{
    NTCI_LOG_CONTEXT();

    bslma::ManagedPtr<ntcs::Event> event =
        d_eventPool.getManagedObject(socket);
    if (NTCCFG_UNLIKELY(!event)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    event->d_type = ntcs::EventType::e_ACCEPT;

    NTCP_IOCP_LOG_EVENT_STARTING(event);

    ntsa::Error error;

    ntsa::Endpoint sourceEndpoint;
    error = ntsf::System::getSourceEndpoint(&sourceEndpoint, socket->handle());
    if (error) {
        return error;
    }

    if (sourceEndpoint.isIp()) {
        if (sourceEndpoint.ip().host().isV4()) {
            error = ntsf::System::createStreamSocket(
                &event->d_target,
                ntsa::Transport::e_TCP_IPV4_STREAM);
            if (error) {
                return error;
            }
        }
        else if (sourceEndpoint.ip().host().isV6()) {
            error = ntsf::System::createStreamSocket(
                &event->d_target,
                ntsa::Transport::e_TCP_IPV6_STREAM);
            if (error) {
                return error;
            }
        }
        else {
            return ntsa::Error::invalid();
        }
    }
    else if (sourceEndpoint.isLocal()) {
        error =
            ntsf::System::createStreamSocket(&event->d_target,
                                             ntsa::Transport::e_LOCAL_STREAM);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error::invalid();
    }

    BSLS_ASSERT(event->d_target != ntsa::k_INVALID_HANDLE);

    LPFN_ACCEPTEX acceptEx = 0;
    {
        DWORD unused = 0;
        GUID  guid   = WSAID_ACCEPTEX;

        int rc = WSAIoctl(socket->handle(),
                          SIO_GET_EXTENSION_FUNCTION_POINTER,
                          &guid,
                          sizeof(GUID),
                          reinterpret_cast<void**>(&acceptEx),
                          sizeof(void*),
                          &unused,
                          0,
                          0);

        if (rc != 0) {
            return ntsa::Error(GetLastError());
        }

        BSLS_ASSERT(acceptEx);
    }

    DWORD rc            = 0;
    DWORD bytesReceived = 0;

    SetLastError(0);

    if (sourceEndpoint.isIp()) {
        if (sourceEndpoint.ip().host().isV4()) {
            rc = acceptEx(event->d_socket->handle(),
                          event->d_target,
                          event->d_address,
                          0,
                          sizeof(sockaddr_in) + 16,
                          sizeof(sockaddr_in) + 16,
                          &bytesReceived,
                          reinterpret_cast<OVERLAPPED*>(event.get()));
        }
        else if (sourceEndpoint.ip().host().isV6()) {
            rc = acceptEx(event->d_socket->handle(),
                          event->d_target,
                          event->d_address,
                          0,
                          sizeof(sockaddr_in6) + 16,
                          sizeof(sockaddr_in6) + 16,
                          &bytesReceived,
                          reinterpret_cast<OVERLAPPED*>(event.get()));
        }
        else {
            return ntsa::Error::invalid();
        }
    }
    else if (sourceEndpoint.isLocal()) {
        rc = acceptEx(event->d_socket->handle(),
                      event->d_target,
                      event->d_address,
                      0,
                      sizeof(sockaddr_un) + 16,
                      sizeof(sockaddr_un) + 16,
                      &bytesReceived,
                      reinterpret_cast<OVERLAPPED*>(event.get()));
    }
    else {
        return ntsa::Error::invalid();
    }

    DWORD lastError = GetLastError();

    if (rc != 0) {
        this->submit(event);
        return ntsa::Error();
    }
    else if (lastError == ERROR_IO_PENDING) {
        event.release();
        return ntsa::Error();
    }
    else {
        return ntsa::Error(lastError);
    }
}

ntsa::Error Iocp::connect(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                          const ntsa::Endpoint& endpoint)
{
    NTCI_LOG_CONTEXT();

    if (endpoint.isUndefined()) {
        return ntsa::Error::invalid();
    }

    bslma::ManagedPtr<ntcs::Event> event =
        d_eventPool.getManagedObject(socket);
    if (NTCCFG_UNLIKELY(!event)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    event->d_type = ntcs::EventType::e_CONNECT;

    NTCP_IOCP_LOG_EVENT_STARTING(event);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    {
        bsl::size_t socketAddressSizeT = 0;
        ntsa::Error error =
            ntsu::SocketUtil::encodeEndpoint(&socketAddress,
                                             &socketAddressSizeT,
                                             endpoint);

        socketAddressSize = static_cast<socklen_t>(socketAddressSizeT);
    }

    LPFN_CONNECTEX connectEx = 0;
    {
        DWORD unused = 0;
        GUID  guid   = WSAID_CONNECTEX;

        int rc = WSAIoctl(socket->handle(),
                          SIO_GET_EXTENSION_FUNCTION_POINTER,
                          &guid,
                          sizeof(GUID),
                          reinterpret_cast<void**>(&connectEx),
                          sizeof(void*),
                          &unused,
                          0,
                          0);

        if (rc != 0) {
            return ntsa::Error(GetLastError());
        }

        BSLS_ASSERT(connectEx);
    }

    // The LPFN_CONNECTEX function requires the socket to be explicitly
    // bound. Check if the socket has been previously bound, and if not,
    // bind it to "any" address.

    {
        ntsa::Endpoint sourceEndpoint;
        ntsa::Error    error =
            ntsf::System::getSourceEndpoint(&sourceEndpoint, socket->handle());
        if (error || sourceEndpoint.isUndefined()) {
            error = ntsf::System::bindAny(
                socket->handle(),
                endpoint.transport(ntsa::TransportMode::e_STREAM),
                false);
            if (error) {
                return error;
            }
        }
    }

    SetLastError(0);

    DWORD rc = connectEx(socket->handle(),
                         reinterpret_cast<sockaddr*>(&socketAddress),
                         socketAddressSize,
                         0,
                         0,
                         0,
                         reinterpret_cast<OVERLAPPED*>(event.get()));

    DWORD lastError = GetLastError();

    if (rc != 0) {
        this->submit(event);
        return ntsa::Error();
    }
    else if (lastError == ERROR_IO_PENDING) {
        event.release();
        return ntsa::Error();
    }
    else {
        return ntsa::Error(lastError);
    }
}

ntsa::Error Iocp::send(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                       const bdlbb::Blob&                           data,
                       const ntsa::SendOptions&                     options)
{
    NTCI_LOG_CONTEXT();

    bslma::ManagedPtr<ntcs::Event> event =
        d_eventPool.getManagedObject(socket);
    if (NTCCFG_UNLIKELY(!event)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    event->d_type = ntcs::EventType::e_SEND;

    NTCP_IOCP_LOG_EVENT_STARTING(event);

    ntsa::Handle descriptorHandle = socket->handle();

    bsl::size_t numBytesMax = options.maxBytes();
    if (numBytesMax == 0) {
        numBytesMax = ntsu::SocketUtil::maxBytesPerSend(descriptorHandle);
    }

    bsl::size_t numBuffersMax = options.maxBuffers();
    if (numBuffersMax == 0) {
        numBuffersMax = NTCP_COMPLETIONPORT_WSABUF_MAX;
    }
    else if (numBuffersMax > NTCP_COMPLETIONPORT_WSABUF_MAX) {
        numBuffersMax = NTCP_COMPLETIONPORT_WSABUF_MAX;
    }

    WSABUF wsaBufArray[NTCP_COMPLETIONPORT_WSABUF_MAX];

    bsl::size_t numBuffersTotal;
    bsl::size_t numBytesTotal;

    ntsa::ConstBuffer::gather(
        &numBuffersTotal,
        &numBytesTotal,
        reinterpret_cast<ntsa::ConstBuffer*>(wsaBufArray),
        numBuffersMax,
        data,
        numBytesMax);

    event->d_numBytesAttempted = numBytesTotal;

    const bool specifyEndpoint = !options.endpoint().isNull();

    if (specifyEndpoint) {
        socklen_t socketAddressSize;
        {
            bsl::size_t socketAddressSizeT;
            ntsa::Error error = ntsu::SocketUtil::encodeEndpoint(
                reinterpret_cast<sockaddr_storage*>(event->d_address),
                &socketAddressSizeT,
                options.endpoint().value());
            if (error) {
                return error;
            }

            socketAddressSize = static_cast<socklen_t>(socketAddressSizeT);
        }

        SetLastError(0);

        int wsaSendToResult =
            WSASendTo(descriptorHandle,
                      wsaBufArray,
                      static_cast<DWORD>(numBuffersTotal),
                      0,
                      0,
                      reinterpret_cast<sockaddr*>(event->d_address),
                      socketAddressSize,
                      reinterpret_cast<OVERLAPPED*>(event.get()),
                      0);

        DWORD lastError = GetLastError();

        if (wsaSendToResult == 0) {
            event.release();
            return ntsa::Error();
        }
        else if (lastError == WSA_IO_PENDING) {
            event.release();
            return ntsa::Error();
        }
        else {
            return ntsa::Error(lastError);
        }
    }
    else {
        SetLastError(0);

        int wsaSendResult = WSASend(descriptorHandle,
                                    wsaBufArray,
                                    static_cast<DWORD>(numBuffersTotal),
                                    0,
                                    0,
                                    reinterpret_cast<OVERLAPPED*>(event.get()),
                                    0);

        DWORD lastError = GetLastError();

        if (wsaSendResult == 0) {
            event.release();
            return ntsa::Error();
        }
        else if (lastError == WSA_IO_PENDING) {
            event.release();
            return ntsa::Error();
        }
        else {
            return ntsa::Error(lastError);
        }
    }
}

ntsa::Error Iocp::send(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                       const ntsa::Data&                            data,
                       const ntsa::SendOptions&                     options)
{
    NTCI_LOG_CONTEXT();

    const bool specifyEndpoint = !options.endpoint().isNull();

    if (specifyEndpoint && data.isFile()) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    bslma::ManagedPtr<ntcs::Event> event =
        d_eventPool.getManagedObject(socket);
    if (NTCCFG_UNLIKELY(!event)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    event->d_type = ntcs::EventType::e_SEND;

    NTCP_IOCP_LOG_EVENT_STARTING(event);

    ntsa::Handle descriptorHandle  = socket->handle();
    socklen_t    socketAddressSize = 0;
    DWORD        lastError         = 0;

    if (specifyEndpoint) {
        bsl::size_t socketAddressSizeT;
        ntsa::Error error = ntsu::SocketUtil::encodeEndpoint(
            reinterpret_cast<sockaddr_storage*>(event->d_address),
            &socketAddressSizeT,
            options.endpoint().value());
        if (error) {
            return error;
        }

        socketAddressSize = static_cast<socklen_t>(socketAddressSizeT);
    }

    if (NTCCFG_LIKELY(data.isBlob())) {
        const bdlbb::Blob& blob = data.blob();

        bsl::size_t numBytesMax = options.maxBytes();
        if (numBytesMax == 0) {
            numBytesMax = ntsu::SocketUtil::maxBytesPerSend(descriptorHandle);
        }

        bsl::size_t numBuffersMax = options.maxBuffers();
        if (numBuffersMax == 0) {
            numBuffersMax = NTCP_COMPLETIONPORT_WSABUF_MAX;
        }
        else if (numBuffersMax > NTCP_COMPLETIONPORT_WSABUF_MAX) {
            numBuffersMax = NTCP_COMPLETIONPORT_WSABUF_MAX;
        }

        WSABUF wsaBufArray[NTCP_COMPLETIONPORT_WSABUF_MAX];

        bsl::size_t numBuffersTotal;
        bsl::size_t numBytesTotal;

        ntsa::ConstBuffer::gather(
            &numBuffersTotal,
            &numBytesTotal,
            reinterpret_cast<ntsa::ConstBuffer*>(wsaBufArray),
            numBuffersMax,
            blob,
            numBytesMax);

        event->d_numBytesAttempted = numBytesTotal;

        if (specifyEndpoint) {
            SetLastError(0);

            int wsaSendToResult =
                WSASendTo(descriptorHandle,
                          wsaBufArray,
                          NTCCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                          0,
                          0,
                          reinterpret_cast<sockaddr*>(event->d_address),
                          socketAddressSize,
                          reinterpret_cast<OVERLAPPED*>(event.get()),
                          0);

            if (wsaSendToResult != 0) {
                lastError = GetLastError();
            }
        }
        else {
            SetLastError(0);

            int wsaSendResult =
                WSASend(descriptorHandle,
                        wsaBufArray,
                        NTCCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                        0,
                        0,
                        reinterpret_cast<OVERLAPPED*>(event.get()),
                        0);

            if (wsaSendResult != 0) {
                lastError = GetLastError();
            }
        }
    }
    else if (data.isSharedBlob()) {
        const bsl::shared_ptr<bdlbb::Blob>& blob = data.sharedBlob();

        bsl::size_t numBytesMax = options.maxBytes();
        if (numBytesMax == 0) {
            numBytesMax = ntsu::SocketUtil::maxBytesPerSend(descriptorHandle);
        }

        bsl::size_t numBuffersMax = options.maxBuffers();
        if (numBuffersMax == 0) {
            numBuffersMax = NTCP_COMPLETIONPORT_WSABUF_MAX;
        }
        else if (numBuffersMax > NTCP_COMPLETIONPORT_WSABUF_MAX) {
            numBuffersMax = NTCP_COMPLETIONPORT_WSABUF_MAX;
        }

        WSABUF wsaBufArray[NTCP_COMPLETIONPORT_WSABUF_MAX];

        bsl::size_t numBuffersTotal;
        bsl::size_t numBytesTotal;

        ntsa::ConstBuffer::gather(
            &numBuffersTotal,
            &numBytesTotal,
            reinterpret_cast<ntsa::ConstBuffer*>(wsaBufArray),
            numBuffersMax,
            *blob,
            numBytesMax);

        event->d_numBytesAttempted = numBytesTotal;

        if (specifyEndpoint) {
            SetLastError(0);

            int wsaSendToResult =
                WSASendTo(descriptorHandle,
                          wsaBufArray,
                          NTCCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                          0,
                          0,
                          reinterpret_cast<sockaddr*>(event->d_address),
                          socketAddressSize,
                          reinterpret_cast<OVERLAPPED*>(event.get()),
                          0);

            if (wsaSendToResult != 0) {
                lastError = GetLastError();
            }
        }
        else {
            SetLastError(0);

            int wsaSendResult =
                WSASend(descriptorHandle,
                        wsaBufArray,
                        NTCCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                        0,
                        0,
                        reinterpret_cast<OVERLAPPED*>(event.get()),
                        0);

            if (wsaSendResult != 0) {
                lastError = GetLastError();
            }
        }
    }
    else if (data.isBlobBuffer()) {
        const bdlbb::BlobBuffer& blobBuffer = data.blobBuffer();

        event->d_numBytesAttempted = blobBuffer.size();

        ntsa::ConstBuffer constBuffer(blobBuffer.data(), blobBuffer.size());

        if (specifyEndpoint) {
            SetLastError(0);

            int wsaSendToResult =
                WSASendTo(descriptorHandle,
                          (WSABUF*)(&constBuffer),
                          1,
                          0,
                          0,
                          reinterpret_cast<sockaddr*>(event->d_address),
                          socketAddressSize,
                          reinterpret_cast<OVERLAPPED*>(event.get()),
                          0);

            if (wsaSendToResult != 0) {
                lastError = GetLastError();
            }
        }
        else {
            SetLastError(0);

            int wsaSendResult =
                WSASend(descriptorHandle,
                        (WSABUF*)(&constBuffer),
                        1,
                        0,
                        0,
                        reinterpret_cast<OVERLAPPED*>(event.get()),
                        0);

            if (wsaSendResult != 0) {
                lastError = GetLastError();
            }
        }
    }
    else if (data.isConstBuffer()) {
        const ntsa::ConstBuffer& constBuffer = data.constBuffer();

        event->d_numBytesAttempted = constBuffer.size();

        if (specifyEndpoint) {
            SetLastError(0);

            int wsaSendToResult =
                WSASendTo(descriptorHandle,
                          (WSABUF*)(&constBuffer),
                          1,
                          0,
                          0,
                          reinterpret_cast<sockaddr*>(event->d_address),
                          socketAddressSize,
                          reinterpret_cast<OVERLAPPED*>(event.get()),
                          0);

            if (wsaSendToResult != 0) {
                lastError = GetLastError();
            }
        }
        else {
            SetLastError(0);

            int wsaSendResult =
                WSASend(descriptorHandle,
                        (WSABUF*)(&constBuffer),
                        1,
                        0,
                        0,
                        reinterpret_cast<OVERLAPPED*>(event.get()),
                        0);

            if (wsaSendResult != 0) {
                lastError = GetLastError();
            }
        }
    }
    else if (data.isConstBufferArray()) {
        const ntsa::ConstBufferArray& constBufferArray =
            data.constBufferArray();

        bsl::size_t numBuffersTotal = constBufferArray.numBuffers();
        bsl::size_t numBytesTotal   = constBufferArray.numBytes();

        event->d_numBytesAttempted = numBytesTotal;

        const ntsa::ConstBuffer* bufferList = &constBufferArray.buffer(0);

        if (specifyEndpoint) {
            SetLastError(0);

            int wsaSendToResult =
                WSASendTo(descriptorHandle,
                          (WSABUF*)(bufferList),
                          NTCCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                          0,
                          0,
                          reinterpret_cast<sockaddr*>(event->d_address),
                          socketAddressSize,
                          reinterpret_cast<OVERLAPPED*>(event.get()),
                          0);

            if (wsaSendToResult != 0) {
                lastError = GetLastError();
            }
        }
        else {
            SetLastError(0);

            int wsaSendResult =
                WSASend(descriptorHandle,
                        (WSABUF*)(bufferList),
                        NTCCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                        0,
                        0,
                        reinterpret_cast<OVERLAPPED*>(event.get()),
                        0);

            if (wsaSendResult != 0) {
                lastError = GetLastError();
            }
        }
    }
    else if (data.isConstBufferPtrArray()) {
        const ntsa::ConstBufferPtrArray& constBufferPtrArray =
            data.constBufferPtrArray();

        bsl::size_t numBuffersTotal = constBufferPtrArray.numBuffers();
        bsl::size_t numBytesTotal   = constBufferPtrArray.numBytes();

        event->d_numBytesAttempted = numBytesTotal;

        const ntsa::ConstBuffer* bufferList = &constBufferPtrArray.buffer(0);

        if (specifyEndpoint) {
            SetLastError(0);

            int wsaSendToResult =
                WSASendTo(descriptorHandle,
                          (WSABUF*)(bufferList),
                          NTCCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                          0,
                          0,
                          reinterpret_cast<sockaddr*>(event->d_address),
                          socketAddressSize,
                          reinterpret_cast<OVERLAPPED*>(event.get()),
                          0);

            if (wsaSendToResult != 0) {
                lastError = GetLastError();
            }
        }
        else {
            SetLastError(0);

            int wsaSendResult =
                WSASend(descriptorHandle,
                        (WSABUF*)(bufferList),
                        NTCCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                        0,
                        0,
                        reinterpret_cast<OVERLAPPED*>(event.get()),
                        0);

            if (wsaSendResult != 0) {
                lastError = GetLastError();
            }
        }
    }
    else if (data.isMutableBuffer()) {
        const ntsa::MutableBuffer& mutableBuffer = data.mutableBuffer();

        event->d_numBytesAttempted = mutableBuffer.size();

        if (specifyEndpoint) {
            SetLastError(0);

            int wsaSendToResult =
                WSASendTo(descriptorHandle,
                          (WSABUF*)(&mutableBuffer),
                          1,
                          0,
                          0,
                          reinterpret_cast<sockaddr*>(event->d_address),
                          socketAddressSize,
                          reinterpret_cast<OVERLAPPED*>(event.get()),
                          0);

            if (wsaSendToResult != 0) {
                lastError = GetLastError();
            }
        }
        else {
            SetLastError(0);

            int wsaSendResult =
                WSASend(descriptorHandle,
                        (WSABUF*)(&mutableBuffer),
                        1,
                        0,
                        0,
                        reinterpret_cast<OVERLAPPED*>(event.get()),
                        0);

            if (wsaSendResult != 0) {
                lastError = GetLastError();
            }
        }
    }
    else if (data.isMutableBufferArray()) {
        const ntsa::MutableBufferArray& mutableBufferArray =
            data.mutableBufferArray();

        bsl::size_t numBuffersTotal = mutableBufferArray.numBuffers();
        bsl::size_t numBytesTotal   = mutableBufferArray.numBytes();

        event->d_numBytesAttempted = numBytesTotal;

        const ntsa::MutableBuffer* bufferList = &mutableBufferArray.buffer(0);

        if (specifyEndpoint) {
            SetLastError(0);

            int wsaSendToResult =
                WSASendTo(descriptorHandle,
                          (WSABUF*)(bufferList),
                          NTCCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                          0,
                          0,
                          reinterpret_cast<sockaddr*>(event->d_address),
                          socketAddressSize,
                          reinterpret_cast<OVERLAPPED*>(event.get()),
                          0);

            if (wsaSendToResult != 0) {
                lastError = GetLastError();
            }
        }
        else {
            SetLastError(0);

            int wsaSendResult =
                WSASend(descriptorHandle,
                        (WSABUF*)(bufferList),
                        NTCCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                        0,
                        0,
                        reinterpret_cast<OVERLAPPED*>(event.get()),
                        0);

            if (wsaSendResult != 0) {
                lastError = GetLastError();
            }
        }
    }
    else if (data.isMutableBufferPtrArray()) {
        const ntsa::MutableBufferPtrArray& mutableBufferPtrArray =
            data.mutableBufferPtrArray();

        bsl::size_t numBuffersTotal = mutableBufferPtrArray.numBuffers();
        bsl::size_t numBytesTotal   = mutableBufferPtrArray.numBytes();

        event->d_numBytesAttempted = numBytesTotal;

        const ntsa::MutableBuffer* bufferList =
            &mutableBufferPtrArray.buffer(0);

        if (specifyEndpoint) {
            SetLastError(0);

            int wsaSendToResult =
                WSASendTo(descriptorHandle,
                          (WSABUF*)(bufferList),
                          NTCCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                          0,
                          0,
                          reinterpret_cast<sockaddr*>(event->d_address),
                          socketAddressSize,
                          reinterpret_cast<OVERLAPPED*>(event.get()),
                          0);

            if (wsaSendToResult != 0) {
                lastError = GetLastError();
            }
        }
        else {
            SetLastError(0);

            int wsaSendResult =
                WSASend(descriptorHandle,
                        (WSABUF*)(bufferList),
                        NTCCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                        0,
                        0,
                        reinterpret_cast<OVERLAPPED*>(event.get()),
                        0);

            if (wsaSendResult != 0) {
                lastError = GetLastError();
            }
        }
    }
    else if (data.isString()) {
        const bsl::string& string = data.string();

        event->d_numBytesAttempted = string.size();

        ntsa::ConstBuffer constBuffer(string.data(), string.size());

        if (specifyEndpoint) {
            SetLastError(0);

            int wsaSendToResult =
                WSASendTo(descriptorHandle,
                          (WSABUF*)(&constBuffer),
                          1,
                          0,
                          0,
                          reinterpret_cast<sockaddr*>(event->d_address),
                          socketAddressSize,
                          reinterpret_cast<OVERLAPPED*>(event.get()),
                          0);

            if (wsaSendToResult != 0) {
                lastError = GetLastError();
            }
        }
        else {
            SetLastError(0);

            int wsaSendResult =
                WSASend(descriptorHandle,
                        (WSABUF*)(&constBuffer),
                        1,
                        0,
                        0,
                        reinterpret_cast<OVERLAPPED*>(event.get()),
                        0);

            if (wsaSendResult != 0) {
                lastError = GetLastError();
            }
        }
    }
    else if (data.isFile()) {
        const ntsa::File& file = data.file();

        DWORD size;
        if (file.size() <= bsl::numeric_limits<DWORD>::max()) {
            size = NTCCFG_WARNING_NARROW(DWORD, file.size());
        }
        else {
            size = bsl::numeric_limits<DWORD>::max();
        }

        bdls::FilesystemUtil::Offset offset = bdls::FilesystemUtil::seek(
            file.descriptor(),
            file.position(),
            bdls::FilesystemUtil::e_SEEK_FROM_BEGINNING);

        if (offset != file.position()) {
            return ntsa::Error(GetLastError());
        }

        event->d_numBytesAttempted = size;

        SetLastError(0);

        BOOL transmitFileResult =
            TransmitFile(descriptorHandle,
                         file.descriptor(),
                         size,
                         0,
                         reinterpret_cast<OVERLAPPED*>(event.get()),
                         0,
                         0);

        if (!transmitFileResult) {
            lastError = GetLastError();
        }
    }
    else {
        return ntsa::Error::invalid();
    }

    if (lastError == 0) {
        event.release();
        return ntsa::Error();
    }
    else if (lastError == WSA_IO_PENDING) {
        event.release();
        return ntsa::Error();
    }
    else {
        return ntsa::Error(lastError);
    }
}

ntsa::Error Iocp::receive(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                          bdlbb::Blob*                                 data,
                          const ntsa::ReceiveOptions&                  options)
{
    NTCI_LOG_CONTEXT();

    const bool wantEndpoint = options.wantEndpoint();

    bslma::ManagedPtr<ntcs::Event> event =
        d_eventPool.getManagedObject(socket);
    if (NTCCFG_UNLIKELY(!event)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    event->d_type          = ntcs::EventType::e_RECEIVE;
    event->d_receiveData_p = data;

    if (wantEndpoint) {
        BSLMF_ASSERT(sizeof(event->d_address) >= sizeof(sockaddr_storage));
        event->d_numBytesIndicated = sizeof(sockaddr_storage) + 1;
    }

    NTCP_IOCP_LOG_EVENT_STARTING(event);

    ntsa::Handle descriptorHandle = socket->handle();

    bsl::size_t numBytesMax = options.maxBytes();
    if (numBytesMax == 0) {
        numBytesMax = ntsu::SocketUtil::maxBytesPerReceive(descriptorHandle);
    }

    bsl::size_t numBuffersMax = options.maxBuffers();
    if (numBuffersMax == 0) {
        numBuffersMax = NTCP_COMPLETIONPORT_WSABUF_MAX;
    }
    else if (numBuffersMax > NTCP_COMPLETIONPORT_WSABUF_MAX) {
        numBuffersMax = NTCP_COMPLETIONPORT_WSABUF_MAX;
    }

    WSABUF wsaBufArray[NTCP_COMPLETIONPORT_WSABUF_MAX];

    bsl::size_t numBuffersTotal;
    bsl::size_t numBytesTotal;

    ntsa::MutableBuffer::scatter(
        &numBuffersTotal,
        &numBytesTotal,
        reinterpret_cast<ntsa::MutableBuffer*>(wsaBufArray),
        numBuffersMax,
        data,
        numBytesMax);

    event->d_numBytesAttempted = numBytesTotal;

    SetLastError(0);

    if (wantEndpoint) {
        DWORD wsaFlags = 0;
        int   wsaRecvfromResult =
            WSARecvFrom(descriptorHandle,
                        wsaBufArray,
                        NTCCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                        0,
                        &wsaFlags,
                        reinterpret_cast<sockaddr*>(event->d_address),
                        &event->d_numBytesIndicated,
                        reinterpret_cast<OVERLAPPED*>(event.get()),
                        0);

        DWORD lastError = GetLastError();

        if (wsaRecvfromResult == 0) {
            event.release();
            return ntsa::Error();
        }
        else if (lastError == WSA_IO_PENDING) {
            event.release();
            return ntsa::Error();
        }
        else {
            return ntsa::Error(lastError);
        }
    }
    else {
        DWORD wsaFlags = 0;
        int   wsaRecvResult =
            WSARecv(descriptorHandle,
                    wsaBufArray,
                    NTCCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                    0,
                    &wsaFlags,
                    reinterpret_cast<OVERLAPPED*>(event.get()),
                    0);

        DWORD lastError = GetLastError();

        if (wsaRecvResult == 0) {
            event.release();
            return ntsa::Error();
        }
        else if (lastError == WSA_IO_PENDING) {
            event.release();
            return ntsa::Error();
        }
        else {
            return ntsa::Error(lastError);
        }
    }
}

ntsa::Error Iocp::shutdown(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                           ntsa::ShutdownType::Value direction)
{
    ntsa::Error error = ntsf::System::shutdown(socket->handle(), direction);

    if (error && error != ntsa::Error::e_INVALID) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Iocp::cancel(const bsl::shared_ptr<ntci::ProactorSocket>& socket)
{
    NTCI_LOG_CONTEXT();

    BOOL rc = CancelIoEx(reinterpret_cast<HANDLE>(socket->handle()), 0);

    DWORD lastError = GetLastError();

    if (FALSE == rc && ERROR_NOT_FOUND != lastError &&
        ERROR_INVALID_HANDLE != lastError)
    {
        return ntsa::Error(lastError);
    }

    return ntsa::Error();
}

ntsa::Error Iocp::detachSocket(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket)
{
    ntsa::Error error;

    if (socket->handle() == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error::invalid();
    }

    bsl::shared_ptr<ntco::IocpContext> context =
        bslstl::SharedPtrUtil::staticCast<ntco::IocpContext>(
            socket->getProactorContext());
    if (!context) {
        return ntsa::Error::invalid();
    }

    this->cancel(socket);

    {
        LockGuard lockGuard(&d_proactorSocketMapMutex);

        bsl::size_t n = d_proactorSocketMap.erase(socket->handle());
        if (n == 0) {
            return ntsa::Error::invalid();
        }
    }

    error = context->detach();
    if (error) {
        if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK)) {
            return ntsa::Error();
        }
        return error;
    }

    socket->setProactorContext(bsl::shared_ptr<void>());

    this->execute(NTCCFG_BIND(&ntcs::Dispatch::announceDetached,
                              socket,
                              socket->strand()));

    return ntsa::Error();
}

ntsa::Error Iocp::closeAll()
{
    ProactorSocketMap proactorSocketMap;
    {
        LockGuard lockGuard(&d_proactorSocketMapMutex);
        proactorSocketMap = d_proactorSocketMap;
    }

    for (ProactorSocketMap::iterator it = proactorSocketMap.begin();
         it != proactorSocketMap.end();
         ++it)
    {
        const bsl::shared_ptr<ntci::ProactorSocket>& proactorSocket =
            it->second;
        proactorSocket->close();
    }

    proactorSocketMap.clear();

    return ntsa::Error();
}

void Iocp::incrementLoad(const ntca::LoadBalancingOptions& options)
{
    bsl::size_t weight =
        options.weight().isNull() ? 1 : options.weight().value();

    d_load += weight;
}

void Iocp::decrementLoad(const ntca::LoadBalancingOptions& options)
{
    bsl::size_t weight =
        options.weight().isNull() ? 1 : options.weight().value();

    d_load -= weight;
}

void Iocp::run(ntci::Waiter waiter)
{
    while (d_run) {
        // Wait for an operation to complete or a timeout.

        this->wait(waiter);

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

void Iocp::poll(ntci::Waiter waiter)
{
    // Wait for an operation to complete or a timeout.

    this->wait(waiter);

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

void Iocp::interruptOne()
{
    if (NTCCFG_LIKELY(isWaiter())) {
        return;
    }

    bslma::ManagedPtr<ntcs::Event> event = d_eventPool.getManagedObject();

    event->d_type     = ntcs::EventType::e_CALLBACK;
    event->d_function = &noop;

    this->submit(event);
}

void Iocp::interruptAll()
{
    if (NTCCFG_LIKELY(d_config.maxThreads().value() == 1)) {
        if (NTCCFG_LIKELY(isWaiter())) {
            return;
        }

        bslma::ManagedPtr<ntcs::Event> event = d_eventPool.getManagedObject();

        event->d_type     = ntcs::EventType::e_CALLBACK;
        event->d_function = &noop;

        this->submit(event);
    }
    else {
        bsl::size_t numWaiters;
        {
            LockGuard guard(&d_waiterSetMutex);
            numWaiters = d_waiterSet.size();
        }

        for (bsl::size_t i = 0; i < numWaiters; ++i) {
            bslma::ManagedPtr<ntcs::Event> event =
                d_eventPool.getManagedObject();

            event->d_type     = ntcs::EventType::e_CALLBACK;
            event->d_function = &noop;

            this->submit(event);
        }
    }
}

void Iocp::stop()
{
    d_run = false;
    this->interruptAll();
}

void Iocp::restart()
{
    d_run = true;
}

void Iocp::drainFunctions()
{
    d_chronology.drain();
}

void Iocp::clearFunctions()
{
    d_chronology.clearFunctions();
}

void Iocp::clearTimers()
{
    d_chronology.clearTimers();
}

void Iocp::clearSockets()
{
    LockGuard lockGuard(&d_proactorSocketMapMutex);
    d_proactorSocketMap.clear();
}

void Iocp::clear()
{
    d_chronology.clear();

    LockGuard lockGuard(&d_proactorSocketMapMutex);
    d_proactorSocketMap.clear();
}

void Iocp::execute(const Functor& functor)
{
    d_chronology.execute(functor);
}

void Iocp::moveAndExecute(FunctorSequence* functorSequence,
                          const Functor&   functor)
{
    d_chronology.moveAndExecute(functorSequence, functor);
}

bsl::shared_ptr<ntci::Timer> Iocp::createTimer(
    const ntca::TimerOptions&                  options,
    const bsl::shared_ptr<ntci::TimerSession>& session,
    bslma::Allocator*                          basicAllocator)
{
    return d_chronology.createTimer(options, session, basicAllocator);
}

bsl::shared_ptr<ntci::Timer> Iocp::createTimer(
    const ntca::TimerOptions&  options,
    const ntci::TimerCallback& callback,
    bslma::Allocator*          basicAllocator)
{
    return d_chronology.createTimer(options, callback, basicAllocator);
}

bsl::shared_ptr<ntci::DatagramSocket> Iocp::createDatagramSocket(
    const ntca::DatagramSocketOptions& options,
    bslma::Allocator*                  basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    // MRM: Define how metrics are injected into sockets.
    bsl::shared_ptr<ntcs::Metrics> metrics;

    bsl::shared_ptr<ntcp::DatagramSocket> datagramSocket;
    datagramSocket.createInplace(allocator,
                                 options,
                                 d_resolver_sp,
                                 this->getSelf(this),
                                 this->getSelf(this),
                                 metrics,
                                 allocator);

    return datagramSocket;
}

bsl::shared_ptr<ntci::ListenerSocket> Iocp::createListenerSocket(
    const ntca::ListenerSocketOptions& options,
    bslma::Allocator*                  basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    // MRM: Define how metrics are injected into sockets.
    bsl::shared_ptr<ntcs::Metrics> metrics;

    bsl::shared_ptr<ntcp::ListenerSocket> listenerSocket;
    listenerSocket.createInplace(allocator,
                                 options,
                                 d_resolver_sp,
                                 this->getSelf(this),
                                 this->getSelf(this),
                                 metrics,
                                 allocator);

    return listenerSocket;
}

bsl::shared_ptr<ntci::StreamSocket> Iocp::createStreamSocket(
    const ntca::StreamSocketOptions& options,
    bslma::Allocator*                basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    // MRM: Define how metrics are injected into sockets.
    bsl::shared_ptr<ntcs::Metrics> metrics;

    bsl::shared_ptr<ntcp::StreamSocket> streamSocket;
    streamSocket.createInplace(allocator,
                               options,
                               d_resolver_sp,
                               this->getSelf(this),
                               this->getSelf(this),
                               metrics,
                               allocator);

    return streamSocket;
}

bsl::shared_ptr<ntsa::Data> Iocp::createIncomingData()
{
    return d_dataPool_sp->createIncomingData();
}

bsl::shared_ptr<ntsa::Data> Iocp::createOutgoingData()
{
    return d_dataPool_sp->createOutgoingData();
}

bsl::shared_ptr<bdlbb::Blob> Iocp::createIncomingBlob()
{
    return d_dataPool_sp->createIncomingBlob();
}

bsl::shared_ptr<bdlbb::Blob> Iocp::createOutgoingBlob()
{
    return d_dataPool_sp->createOutgoingBlob();
}

void Iocp::createIncomingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_dataPool_sp->createIncomingBlobBuffer(blobBuffer);
}

void Iocp::createOutgoingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_dataPool_sp->createOutgoingBlobBuffer(blobBuffer);
}

bsl::size_t Iocp::numSockets() const
{
    LockGuard lockGuard(&d_proactorSocketMapMutex);
    return d_proactorSocketMap.size();
}

bsl::size_t Iocp::maxSockets() const
{
    return static_cast<bsl::size_t>(-1);
}

bsl::size_t Iocp::numTimers() const
{
    return d_chronology.numScheduled();
}

bsl::size_t Iocp::maxTimers() const
{
    return static_cast<bsl::size_t>(-1);
}

bsl::size_t Iocp::load() const
{
    return static_cast<bsl::size_t>(d_load);
}

bslmt::ThreadUtil::Handle Iocp::threadHandle() const
{
    LockGuard lock(&d_waiterSetMutex);
    return d_threadHandle;
}

bsl::size_t Iocp::threadIndex() const
{
    LockGuard lock(&d_waiterSetMutex);
    return d_threadIndex;
}

bsl::size_t Iocp::numWaiters() const
{
    LockGuard lock(&d_waiterSetMutex);
    return d_waiterSet.size();
}

bool Iocp::empty() const
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

const bsl::shared_ptr<ntci::DataPool>& Iocp::dataPool() const
{
    return d_dataPool_sp;
}

const bsl::shared_ptr<ntci::Strand>& Iocp::strand() const
{
    return ntci::Strand::unspecified();
}

bsls::TimeInterval Iocp::currentTime() const
{
    return bdlt::CurrentTime::now();
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& Iocp::
    incomingBlobBufferFactory() const
{
    return d_dataPool_sp->incomingBlobBufferFactory();
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& Iocp::
    outgoingBlobBufferFactory() const
{
    return d_dataPool_sp->outgoingBlobBufferFactory();
}

const char* Iocp::name() const
{
    return "EXAMPLE";
}

IocpFactory::IocpFactory(bslma::Allocator* basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

IocpFactory::~IocpFactory()
{
}

bsl::shared_ptr<ntci::Proactor> IocpFactory::createProactor(
    const ntca::ProactorConfig&        configuration,
    const bsl::shared_ptr<ntci::User>& user,
    bslma::Allocator*                  basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntco::Iocp> proactor;
    proactor.createInplace(allocator, configuration, user, allocator);

    return proactor;
}

}  // close package namespace
}  // close enterprise namespace

#endif
#endif
