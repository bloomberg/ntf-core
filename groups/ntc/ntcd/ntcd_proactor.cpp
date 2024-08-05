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

#include <ntcd_proactor.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcd_proactor_cpp, "$Id$ $CSID$")

#include <ntccfg_limits.h>
#include <ntci_log.h>
#include <ntcm_monitorableutil.h>
#include <ntcs_datapool.h>
#include <ntcs_dispatch.h>
#include <ntcs_nomenclature.h>
#include <ntcs_proactormetrics.h>
#include <ntcs_strand.h>
#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>
#include <bdlt_localtimeoffset.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_timeutil.h>

#define NTCD_PROACTOR_LOG_WAIT_INDEFINITE()                                   \
    NTCI_LOG_TRACE("Polling for socket events indefinitely")

#define NTCD_PROACTOR_LOG_WAIT_TIMED(timeout)                                 \
    NTCI_LOG_TRACE(                                                           \
        "Polling for sockets events or until %d milliseconds have elapsed",   \
        (int)(timeout))

#define NTCD_PROACTOR_LOG_WAIT_TIMED_HIGH_PRECISION(timeInterval)             \
    do {                                                                      \
        bdlt::Datetime dateTimeDue =                                          \
            bdlt::EpochUtil::convertFromTimeInterval(timeInterval);           \
        dateTimeDue.addSeconds(                                               \
            bdlt::LocalTimeOffset::localTimeOffset(bdlt::CurrentTime::utc())  \
                .totalSeconds());                                             \
        char buffer[128];                                                     \
        dateTimeDue.printToBuffer(buffer, sizeof buffer);                     \
        NTCI_LOG_TRACE("Polling for sockets events or until %s", buffer);     \
    } while (false)

#define NTCD_PROACTOR_LOG_WAIT_FAILURE(error)                                 \
    NTCI_LOG_ERROR("Failed to poll for socket events: %s",                    \
                   error.text().c_str())

#define NTCD_PROACTOR_LOG_WAIT_TIMEOUT()                                      \
    NTCI_LOG_TRACE("Timed out polling for socket events")

#define NTCD_PROACTOR_LOG_WAIT_RESULT(numEvents)                              \
    NTCI_LOG_TRACE("Polled %zu socket events", (bsl::size_t)(numEvents))

#define NTCD_PROACTOR_LOG_EVENT(event)                                        \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Polled event " << event                     \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

namespace BloombergLP {
namespace ntcd {

/// This struct describes the context of a waiter.
struct Proactor::Result {
  public:
    ntca::WaiterOptions                    d_options;
    bsl::shared_ptr<ntci::ProactorMetrics> d_metrics_sp;

  private:
    Result(const Result&) BSLS_KEYWORD_DELETED;
    Result& operator=(const Result&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new proactor result. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit Result(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Result();
};

/// This struct describes the work pending for a socket.
struct Proactor::Work : public ntccfg::Shared<Proactor::Work> {
  public:
    class UpdateGuard;
    // Provide a guard to automatically update a work's
    // enabled events.

    /// This struct describes a pending accept.
    struct Accept {
    };

    /// This struct describes a pending connect.
    struct Connect {
        Connect()
        : d_endpoint()
        {
        }

        explicit Connect(const ntsa::Endpoint& endpoint)
        : d_endpoint(endpoint)
        {
        }

        ntsa::Endpoint d_endpoint;
    };

    /// This struct describes a pending send.
    struct Send {
        enum Type { e_NONE, e_BLOB, e_DATA };

        explicit Send(bslma::Allocator* basicAllocator = 0)
        : d_blob(basicAllocator)
        , d_data(basicAllocator)
        , d_type(e_NONE)
        , d_options()
        {
        }

        Send(const bdlbb::Blob&       blob,
             const ntsa::SendOptions& options,
             bslma::Allocator*        basicAllocator = 0)
        : d_blob(blob, basicAllocator)
        , d_data(basicAllocator)
        , d_type(e_BLOB)
        , d_options(options)
        {
        }

        Send(const ntsa::Data&        data,
             const ntsa::SendOptions& options,
             bslma::Allocator*        basicAllocator = 0)
        : d_blob(basicAllocator)
        , d_data(data, basicAllocator)
        , d_type(e_DATA)
        , d_options(options)
        {
        }

        Send(const Send& original, bslma::Allocator* basicAllocator = 0)
        : d_blob(original.d_blob, basicAllocator)
        , d_data(original.d_data, basicAllocator)
        , d_type(original.d_type)
        , d_options(original.d_options)
        {
        }

        bdlbb::Blob       d_blob;
        ntsa::Data        d_data;
        Type              d_type;
        ntsa::SendOptions d_options;
    };

    /// This struct describes a pending receive.
    struct Receive {
        Receive()
        : d_blob_p(0)
        , d_options()
        {
        }

        Receive(bdlbb::Blob* blob, const ntsa::ReceiveOptions& options)
        : d_blob_p(blob)
        , d_options(options)
        {
        }

        bdlbb::Blob*         d_blob_p;
        ntsa::ReceiveOptions d_options;
    };

    /// This typedef defines a queue of pending accepts.
    typedef bsl::list<Accept> AcceptQueue;

    /// This typedef defines a queue of pending connects.
    typedef bsl::list<Connect> ConnectQueue;

    /// This typedef defines a queue of pending sends.
    typedef bsl::list<Send> SendQueue;

    /// This typedef defines a queue of pending receives.
    typedef bsl::list<Receive> ReceiveQueue;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    Mutex                                 d_mutex;
    ntsa::Handle                          d_handle;
    bsl::shared_ptr<ntcd::Machine>        d_machine_sp;
    bsl::shared_ptr<ntcd::Monitor>        d_monitor_sp;
    bsl::shared_ptr<ntci::ProactorSocket> d_socket_sp;
    bsl::shared_ptr<ntcs::RegistryEntry>  d_entry_sp;
    AcceptQueue                           d_acceptQueue;
    ConnectQueue                          d_connectQueue;
    SendQueue                             d_sendQueue;
    ReceiveQueue                          d_receiveQueue;
    bslma::Allocator*                     d_allocator_p;

  private:
    Work(const Work&) BSLS_KEYWORD_DELETED;
    Work& operator=(const Work&) BSLS_KEYWORD_DELETED;

  private:
    /// Process readability of the socket according to the specified
    /// 'event'.
    void processReadable(const ntca::ReactorEvent& event);

    /// Process writability of the socket according to the specified
    /// 'event'.
    void processWritable(const ntca::ReactorEvent& event);

    /// Process the failure of the socket according to the specified
    /// 'event'.
    void processError(const ntca::ReactorEvent& event);

    void update();

  public:
    /// Create new work for the specified 'socket' identified by the
    /// specified 'handle' on the specified 'machine' monitored by the
    /// specified 'monitor' using the specified 'entry'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    Work(const bsl::shared_ptr<ntcd::Machine>&        machine,
         const bsl::shared_ptr<ntcd::Monitor>&        monitor,
         const bsl::shared_ptr<ntci::ProactorSocket>& socket,
         const bsl::shared_ptr<ntcs::RegistryEntry>&  entry,
         bslma::Allocator*                            basicAllocator = 0);

    /// Destroy this object.
    ~Work();

    /// Show errors for the socket.
    ntsa::Error showError();

    /// Hide errors for the socket.
    ntsa::Error hideError();

    /// Initiate an asynchronous accept operation. Return the error.
    ntsa::Error initiateAccept();

    /// Initiate an asynchronous connect operation to the specified
    /// 'endpoint'. Return the error.
    ntsa::Error initiateConnect(const ntsa::Endpoint& endpoint);

    /// Initiate an asynchronous send operation of the specified 'data'
    /// according to the specified 'options'. Return the error.
    ntsa::Error initiateSend(const bdlbb::Blob&       data,
                             const ntsa::SendOptions& options);

    /// Initiate an asynchronous send operation of the specified 'data'
    /// according to the specified 'options'. Return the error.
    ntsa::Error initiateSend(const ntsa::Data&        data,
                             const ntsa::SendOptions& options);

    /// Initiate an asynchronous receive operation of the specified 'data'
    /// according to the specified 'options'. Return the error. Note that
    /// 'data' must not be modified or destroyed until the operation
    /// completes or fails.
    ntsa::Error initiateReceive(bdlbb::Blob*                data,
                                const ntsa::ReceiveOptions& options);

    /// Shut down the socket in the specified 'direction'. Return the error.
    ntsa::Error shutdown(ntsa::ShutdownType::Value direction);

    /// Cancel all pending operations.
    ntsa::Error cancel();
};

/// Provide a guard to automatically update a session's
/// enabled events.
class Proactor::Work::UpdateGuard
{
    Work* d_work_p;

  private:
    UpdateGuard(const UpdateGuard&) BSLS_KEYWORD_DELETED;
    UpdateGuard& operator=(const UpdateGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new guard that automatically updates the specified
    /// 'work' upon destruction.
    explicit UpdateGuard(Work* work);

    /// Destroy this object.
    ~UpdateGuard();
};

Proactor::Result::Result(bslma::Allocator* basicAllocator)
: d_options(basicAllocator)
, d_metrics_sp()
{
}

Proactor::Result::~Result()
{
}

Proactor::Work::UpdateGuard::UpdateGuard(Work* work)
: d_work_p(work)
{
}

Proactor::Work::UpdateGuard::~UpdateGuard()
{
    d_work_p->update();
}

void Proactor::Work::processReadable(const ntca::ReactorEvent& event)
{
    NTCCFG_WARNING_UNUSED(event);

    bsl::shared_ptr<Work> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    UpdateGuard update(this);

    ntsa::Error error;

    if (!d_acceptQueue.empty()) {
        if (!event.context().error()) {
            bsl::shared_ptr<ntcd::Session> listenerSession;
            {
                bsl::weak_ptr<ntcd::Session> listenerSession_wp;
                error =
                    d_machine_sp->lookupSession(&listenerSession_wp, d_handle);
                if (error) {
                    d_acceptQueue.clear();
                    return;
                }

                listenerSession = listenerSession_wp.lock();
                if (!listenerSession) {
                    d_acceptQueue.clear();
                    return;
                }
            }

            bsl::shared_ptr<ntcd::Session> serverSession;
            error = listenerSession->accept(&serverSession);
            if (error) {
                Accept accept = d_acceptQueue.front();
                NTCCFG_WARNING_UNUSED(accept);
                d_acceptQueue.pop_front();

                ntccfg::UnLockGuard unlock(&d_mutex);
                ntcs::Dispatch::announceAccepted(
                    d_socket_sp,
                    error,
                    bsl::shared_ptr<ntsi::StreamSocket>(),
                    d_socket_sp->strand());

                return;
            }

            bsl::shared_ptr<ntcd::StreamSocket> streamSocket;
            streamSocket.createInplace(d_allocator_p,
                                       d_machine_sp,
                                       serverSession,
                                       d_allocator_p);

            Accept accept = d_acceptQueue.front();
            NTCCFG_WARNING_UNUSED(accept);
            d_acceptQueue.pop_front();

            ntccfg::UnLockGuard unlock(&d_mutex);
            ntcs::Dispatch::announceAccepted(d_socket_sp,
                                             ntsa::Error(),
                                             streamSocket,
                                             d_socket_sp->strand());
        }
        else {
            while (!d_acceptQueue.empty()) {
                Accept accept = d_acceptQueue.front();
                NTCCFG_WARNING_UNUSED(accept);
                d_acceptQueue.pop_front();

                ntccfg::UnLockGuard unlock(&d_mutex);
                ntcs::Dispatch::announceAccepted(
                    d_socket_sp,
                    event.context().error(),
                    bsl::shared_ptr<ntsi::StreamSocket>(),
                    d_socket_sp->strand());
            }
        }
    }
    else if (!d_receiveQueue.empty()) {
        if (!event.context().error()) {
            bsl::shared_ptr<ntcd::Session> session;
            {
                bsl::weak_ptr<ntcd::Session> session_wp;
                error = d_machine_sp->lookupSession(&session_wp, d_handle);
                if (error) {
                    d_receiveQueue.clear();
                    return;
                }

                session = session_wp.lock();
                if (!session) {
                    d_receiveQueue.clear();
                    return;
                }
            }

            Receive receive = d_receiveQueue.front();

            ntsa::ReceiveContext context;

            error = session->receive(&context,
                                     receive.d_blob_p,
                                     receive.d_options);
            if (error) {
                d_receiveQueue.pop_front();

                ntccfg::UnLockGuard unlock(&d_mutex);
                ntcs::Dispatch::announceReceived(d_socket_sp,
                                                 error,
                                                 context,
                                                 d_socket_sp->strand());

                return;
            }

            d_receiveQueue.pop_front();

            ntccfg::UnLockGuard unlock(&d_mutex);
            ntcs::Dispatch::announceReceived(d_socket_sp,
                                             error,
                                             context,
                                             d_socket_sp->strand());
        }
        else {
            while (!d_receiveQueue.empty()) {
                Receive receive = d_receiveQueue.front();
                NTCCFG_WARNING_UNUSED(receive);
                d_receiveQueue.pop_front();

                ntccfg::UnLockGuard unlock(&d_mutex);
                ntcs::Dispatch::announceReceived(d_socket_sp,
                                                 event.context().error(),
                                                 ntsa::ReceiveContext(),
                                                 d_socket_sp->strand());
            }
        }
    }
}

void Proactor::Work::processWritable(const ntca::ReactorEvent& event)
{
    NTCCFG_WARNING_UNUSED(event);

    bsl::shared_ptr<Work> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    UpdateGuard update(this);

    ntsa::Error error;

    if (!d_connectQueue.empty()) {
        if (!event.context().error()) {
            Connect connect = d_connectQueue.front();
            NTCCFG_WARNING_UNUSED(connect);
            d_connectQueue.pop_front();

            ntccfg::UnLockGuard unlock(&d_mutex);
            ntcs::Dispatch::announceConnected(d_socket_sp,
                                              ntsa::Error(),
                                              d_socket_sp->strand());
        }
        else {
            while (!d_connectQueue.empty()) {
                Connect connect = d_connectQueue.front();
                NTCCFG_WARNING_UNUSED(connect);
                d_connectQueue.pop_front();

                ntccfg::UnLockGuard unlock(&d_mutex);
                ntcs::Dispatch::announceConnected(d_socket_sp,
                                                  event.context().error(),
                                                  d_socket_sp->strand());
            }
        }
    }
    else if (!d_sendQueue.empty()) {
        if (!event.context().error()) {
            bsl::shared_ptr<ntcd::Session> session;
            {
                bsl::weak_ptr<ntcd::Session> session_wp;
                error = d_machine_sp->lookupSession(&session_wp, d_handle);
                if (error) {
                    d_sendQueue.clear();
                    return;
                }

                session = session_wp.lock();
                if (!session) {
                    d_sendQueue.clear();
                    return;
                }
            }

            Send send = d_sendQueue.front();

            ntsa::SendContext context;

            if (send.d_type == Send::e_BLOB) {
                error = session->send(&context, send.d_blob, send.d_options);
            }
            else if (send.d_type == Send::e_DATA) {
                error = session->send(&context, send.d_data, send.d_options);
            }
            else {
                error = ntsa::Error(ntsa::Error::e_INVALID);
            }

            if (error) {
                d_sendQueue.pop_front();

                ntccfg::UnLockGuard unlock(&d_mutex);
                ntcs::Dispatch::announceSent(d_socket_sp,
                                             error,
                                             context,
                                             d_socket_sp->strand());

                return;
            }

            d_sendQueue.pop_front();

            ntccfg::UnLockGuard unlock(&d_mutex);
            ntcs::Dispatch::announceSent(d_socket_sp,
                                         error,
                                         context,
                                         d_socket_sp->strand());
        }
        else {
            while (!d_sendQueue.empty()) {
                Send send = d_sendQueue.front();
                NTCCFG_WARNING_UNUSED(send);
                d_sendQueue.pop_front();

                ntccfg::UnLockGuard unlock(&d_mutex);
                ntcs::Dispatch::announceSent(d_socket_sp,
                                             event.context().error(),
                                             ntsa::SendContext(),
                                             d_socket_sp->strand());
            }
        }
    }
}

void Proactor::Work::processError(const ntca::ReactorEvent& event)
{
    bsl::shared_ptr<Work> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    UpdateGuard update(this);

    if (event.context().error() == ntsa::Error::e_CANCELLED) {
        return;
    }

    while (!d_acceptQueue.empty()) {
        Accept accept = d_acceptQueue.front();
        NTCCFG_WARNING_UNUSED(accept);
        d_acceptQueue.pop_front();

        ntccfg::UnLockGuard unlock(&d_mutex);
        ntcs::Dispatch::announceAccepted(d_socket_sp,
                                         event.context().error(),
                                         bsl::shared_ptr<ntsi::StreamSocket>(),
                                         d_socket_sp->strand());
    }

    while (!d_connectQueue.empty()) {
        Connect connect = d_connectQueue.front();
        NTCCFG_WARNING_UNUSED(connect);
        d_connectQueue.pop_front();

        ntccfg::UnLockGuard unlock(&d_mutex);
        ntcs::Dispatch::announceConnected(d_socket_sp,
                                          event.context().error(),
                                          d_socket_sp->strand());
    }

    while (!d_sendQueue.empty()) {
        Send send = d_sendQueue.front();
        NTCCFG_WARNING_UNUSED(send);
        d_sendQueue.pop_front();

        ntccfg::UnLockGuard unlock(&d_mutex);
        ntcs::Dispatch::announceSent(d_socket_sp,
                                     event.context().error(),
                                     ntsa::SendContext(),
                                     d_socket_sp->strand());
    }

    while (!d_receiveQueue.empty()) {
        Receive receive = d_receiveQueue.front();
        NTCCFG_WARNING_UNUSED(receive);
        d_receiveQueue.pop_front();

        ntccfg::UnLockGuard unlock(&d_mutex);
        ntcs::Dispatch::announceReceived(d_socket_sp,
                                         event.context().error(),
                                         ntsa::ReceiveContext(),
                                         d_socket_sp->strand());
    }
}

void Proactor::Work::update()
{
    bool updateInterest = false;

    if (d_acceptQueue.empty() && d_receiveQueue.empty()) {
        d_entry_sp->hideReadableCallback(ntca::ReactorEventOptions());
        updateInterest = true;
    }

    if (d_connectQueue.empty() && d_sendQueue.empty()) {
        d_entry_sp->hideWritableCallback(ntca::ReactorEventOptions());
        updateInterest = true;
    }

    if (updateInterest) {
        d_monitor_sp->update(d_handle, d_entry_sp->interest());
    }
}

Proactor::Work::Work(const bsl::shared_ptr<ntcd::Machine>&        machine,
                     const bsl::shared_ptr<ntcd::Monitor>&        monitor,
                     const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                     const bsl::shared_ptr<ntcs::RegistryEntry>&  entry,
                     bslma::Allocator* basicAllocator)
: d_mutex()
, d_handle(socket->handle())
, d_machine_sp(machine)
, d_monitor_sp(monitor)
, d_socket_sp(socket)
, d_entry_sp(entry)
, d_acceptQueue(basicAllocator)
, d_connectQueue(basicAllocator)
, d_sendQueue(basicAllocator)
, d_receiveQueue(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Proactor::Work::~Work()
{
}

ntsa::Error Proactor::Work::showError()
{
    LockGuard lock(&d_mutex);

    ntcs::Interest interest = d_entry_sp->showErrorCallback(
        ntca::ReactorEventOptions(),
        ntci::ReactorEventCallback(NTCCFG_BIND(&Proactor::Work::processError,
                                               this,
                                               NTCCFG_BIND_PLACEHOLDER_1)));

    return d_monitor_sp->update(d_handle, interest);
}

ntsa::Error Proactor::Work::hideError()
{
    LockGuard lock(&d_mutex);

    ntcs::Interest interest =
        d_entry_sp->hideErrorCallback(ntca::ReactorEventOptions());

    return d_monitor_sp->update(d_handle, interest);
}

ntsa::Error Proactor::Work::initiateAccept()
{
    LockGuard lock(&d_mutex);

    if (!d_receiveQueue.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool wasEmpty = d_acceptQueue.empty();

    d_acceptQueue.push_back(Accept());

    if (wasEmpty) {
        ntcs::Interest interest = d_entry_sp->showReadableCallback(
            ntca::ReactorEventOptions(),
            ntci::ReactorEventCallback(
                NTCCFG_BIND(&Proactor::Work::processReadable,
                            this,
                            NTCCFG_BIND_PLACEHOLDER_1)));

        return d_monitor_sp->update(d_handle, interest);
    }
    else {
        return ntsa::Error();
    }
}

ntsa::Error Proactor::Work::initiateConnect(const ntsa::Endpoint& endpoint)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_sendQueue.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!d_connectQueue.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<ntcd::Session> session;
    {
        bsl::weak_ptr<ntcd::Session> session_wp;
        error = d_machine_sp->lookupSession(&session_wp, d_handle);
        if (error) {
            return error;
        }

        session = session_wp.lock();
        if (!session) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    error = session->connect(endpoint);
    if (error) {
        if (error != ntsa::Error(ntsa::Error::e_WOULD_BLOCK) &&
            error != ntsa::Error(ntsa::Error::e_PENDING))
        {
            return error;
        }
    }

    bool wasEmpty = d_connectQueue.empty();

    d_connectQueue.push_back(Connect(endpoint));

    if (wasEmpty) {
        ntcs::Interest interest = d_entry_sp->showWritableCallback(
            ntca::ReactorEventOptions(),
            ntci::ReactorEventCallback(
                NTCCFG_BIND(&Proactor::Work::processWritable,
                            this,
                            NTCCFG_BIND_PLACEHOLDER_1)));

        return d_monitor_sp->update(d_handle, interest);
    }
    else {
        return ntsa::Error();
    }
}

ntsa::Error Proactor::Work::initiateSend(const bdlbb::Blob&       data,
                                         const ntsa::SendOptions& options)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_connectQueue.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool wasEmpty = d_sendQueue.empty();

    d_sendQueue.push_back(Send(data, options, d_allocator_p));

    if (wasEmpty) {
        ntcs::Interest interest = d_entry_sp->showWritableCallback(
            ntca::ReactorEventOptions(),
            ntci::ReactorEventCallback(
                NTCCFG_BIND(&Proactor::Work::processWritable,
                            this,
                            NTCCFG_BIND_PLACEHOLDER_1)));

        return d_monitor_sp->update(d_handle, interest);
    }
    else {
        return ntsa::Error();
    }
}

ntsa::Error Proactor::Work::initiateSend(const ntsa::Data&        data,
                                         const ntsa::SendOptions& options)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_connectQueue.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool wasEmpty = d_sendQueue.empty();

    d_sendQueue.push_back(Send(data, options, d_allocator_p));

    if (wasEmpty) {
        ntcs::Interest interest = d_entry_sp->showWritableCallback(
            ntca::ReactorEventOptions(),
            ntci::ReactorEventCallback(
                NTCCFG_BIND(&Proactor::Work::processWritable,
                            this,
                            NTCCFG_BIND_PLACEHOLDER_1)));

        return d_monitor_sp->update(d_handle, interest);
    }
    else {
        return ntsa::Error();
    }
}

ntsa::Error Proactor::Work::initiateReceive(
    bdlbb::Blob*                data,
    const ntsa::ReceiveOptions& options)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_acceptQueue.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool wasEmpty = d_receiveQueue.empty();

    d_receiveQueue.push_back(Receive(data, options));

    if (wasEmpty) {
        ntcs::Interest interest = d_entry_sp->showReadableCallback(
            ntca::ReactorEventOptions(),
            ntci::ReactorEventCallback(
                NTCCFG_BIND(&Proactor::Work::processReadable,
                            this,
                            NTCCFG_BIND_PLACEHOLDER_1)));

        return d_monitor_sp->update(d_handle, interest);
    }
    else {
        return ntsa::Error();
    }
}

ntsa::Error Proactor::Work::shutdown(ntsa::ShutdownType::Value direction)
{
    ntsa::Error error;

    bsl::shared_ptr<ntcd::Session> session;
    {
        bsl::weak_ptr<ntcd::Session> session_wp;
        error = d_machine_sp->lookupSession(&session_wp, d_handle);
        if (error) {
            return error;
        }

        session = session_wp.lock();
        if (!session) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return session->shutdown(direction);
}

ntsa::Error Proactor::Work::cancel()
{
    LockGuard lock(&d_mutex);

    d_acceptQueue.clear();
    d_connectQueue.clear();
    d_sendQueue.clear();
    d_receiveQueue.clear();

    d_entry_sp->hideReadableCallback(ntca::ReactorEventOptions());
    d_entry_sp->hideWritableCallback(ntca::ReactorEventOptions());

    return d_monitor_sp->update(d_handle, d_entry_sp->interest());
}

void Proactor::initialize()
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

    d_chronology_sp.createInplace(
        d_allocator_p,
        bsl::shared_ptr<ntcs::Driver>(this,
                                      bslstl::SharedPtrNilDeleter(),
                                      d_allocator_p),
        d_allocator_p);

    if (d_user_sp) {
        d_resolver_sp = d_user_sp->resolver();
    }

    if (!d_resolver_sp) {
        d_resolver_sp = d_machine_sp->resolver();
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
            d_chronology_sp->setParent(chronology);
        }
    }

    d_monitor_sp = d_machine_sp->createMonitor(d_allocator_p);

    const ntca::ReactorEventTrigger::Value trigger =
        ntca::ReactorEventTrigger::e_LEVEL;

    const bool oneShot = d_config.maxThreads().value() > 1;

    d_monitor_sp->setTrigger(trigger);
    d_monitor_sp->setOneShot(oneShot);

#if NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMDAS
    d_detachFunctor_sp.createInplace(d_allocator_p, [this](const auto& entry) {
        return this->removeDetached(entry);
    });
#else
    d_detachFunctor_sp.createInplace(d_allocator_p,
                                     NTCCFG_BIND(&Proactor::removeDetached,
                                                 this,
                                                 NTCCFG_BIND_PLACEHOLDER_1));
#endif
    d_registry_sp.createInplace(d_allocator_p,
                                trigger,
                                oneShot,
                                d_allocator_p);
}

void Proactor::flush()
{
    while (d_chronology_sp->hasAnyScheduledOrDeferred()) {
        d_chronology_sp->announce(d_dynamic);
    }
}

bsl::shared_ptr<ntci::Proactor> Proactor::acquireProactor(
    const ntca::LoadBalancingOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    return this->getSelf(this);
}

void Proactor::releaseProactor(const bsl::shared_ptr<ntci::Proactor>& proactor,
                               const ntca::LoadBalancingOptions&      options)
{
    BSLS_ASSERT(proactor == this->getSelf(this));
    proactor->decrementLoad(options);
}

bool Proactor::acquireHandleReservation()
{
    if (d_connectionLimiter_sp) {
        return d_connectionLimiter_sp->acquire();
    }
    else {
        return true;
    }
}

void Proactor::releaseHandleReservation()
{
    if (d_connectionLimiter_sp) {
        d_connectionLimiter_sp->release();
    }
}

bsl::size_t Proactor::numProactors() const
{
    return 1;
}

bsl::size_t Proactor::numThreads() const
{
    return this->numWaiters();
}

bsl::size_t Proactor::minThreads() const
{
    return d_config.minThreads().value();
}

bsl::size_t Proactor::maxThreads() const
{
    return d_config.maxThreads().value();
}

Proactor::Proactor(const ntca::ProactorConfig&        configuration,
                   const bsl::shared_ptr<ntci::User>& user,
                   bslma::Allocator*                  basicAllocator)
: d_machine_sp(ntcd::Machine::getDefault())
, d_monitor_sp()
, d_user_sp(user)
, d_dataPool_sp()
, d_resolver_sp()
, d_chronology_sp()
, d_connectionLimiter_sp()
, d_metrics_sp()
, d_datagramSocketFactory_sp()
, d_listenerSocketFactory_sp()
, d_streamSocketFactory_sp()
, d_detachFunctor_sp()
, d_registry_sp()
, d_waiterSetMutex()
, d_waiterSet(basicAllocator)
, d_workMapMutex()
, d_workMap(basicAllocator)
, d_threadHandle(bslmt::ThreadUtil::invalidHandle())
, d_threadIndex(0)
, d_dynamic(false)
, d_load(0)
, d_run(true)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(d_machine_sp);

    this->initialize();
}

Proactor::Proactor(const ntca::ProactorConfig&           configuration,
                   const bsl::shared_ptr<ntci::User>&    user,
                   const bsl::shared_ptr<ntcd::Machine>& machine,
                   bslma::Allocator*                     basicAllocator)
: d_machine_sp(machine)
, d_monitor_sp()
, d_user_sp(user)
, d_dataPool_sp()
, d_resolver_sp()
, d_chronology_sp()
, d_connectionLimiter_sp()
, d_metrics_sp()
, d_datagramSocketFactory_sp()
, d_listenerSocketFactory_sp()
, d_streamSocketFactory_sp()
, d_detachFunctor_sp()
, d_registry_sp()
, d_waiterSetMutex()
, d_waiterSet(basicAllocator)
, d_threadHandle(bslmt::ThreadUtil::invalidHandle())
, d_threadIndex(0)
, d_dynamic(false)
, d_load(0)
, d_run(true)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(d_machine_sp);

    this->initialize();
}

Proactor::~Proactor()
{
    // Assert all timers and functions are executed.

    BSLS_ASSERT_OPT(!d_chronology_sp->hasAnyDeferred());
    BSLS_ASSERT_OPT(!d_chronology_sp->hasAnyScheduled());
    BSLS_ASSERT_OPT(!d_chronology_sp->hasAnyRegistered());

    // Assert all waiters are deregistered.

    BSLS_ASSERT_OPT(d_waiterSet.empty());

    // Reset internal mechanisms.

    d_registry_sp.reset();
    d_streamSocketFactory_sp.reset();
    d_listenerSocketFactory_sp.reset();
    d_datagramSocketFactory_sp.reset();
    d_metrics_sp.reset();
    d_connectionLimiter_sp.reset();
    d_chronology_sp.reset();
    d_resolver_sp.reset();
    d_dataPool_sp.reset();
    d_user_sp.reset();
    d_monitor_sp.reset();
    d_machine_sp.reset();
}

void Proactor::setDatagramSocketFactory(
    const bsl::shared_ptr<ntci::DatagramSocketFactory>& datagramSocketFactory)
{
    d_datagramSocketFactory_sp = datagramSocketFactory;
}

void Proactor::setDatagramSocketFactory(
    const DatagramSocketFactoryFunction& datagramSocketFactory)
{
    bsl::shared_ptr<ntcd::DatagramSocketFactory> object;
    object.createInplace(d_allocator_p, datagramSocketFactory, d_allocator_p);

    d_datagramSocketFactory_sp = object;
}

void Proactor::setListenerSocketFactory(
    const bsl::shared_ptr<ntci::ListenerSocketFactory>& listenerSocketFactory)
{
    d_listenerSocketFactory_sp = listenerSocketFactory;
}

void Proactor::setListenerSocketFactory(
    const ListenerSocketFactoryFunction& listenerSocketFactory)
{
    bsl::shared_ptr<ntcd::ListenerSocketFactory> object;
    object.createInplace(d_allocator_p, listenerSocketFactory, d_allocator_p);

    d_listenerSocketFactory_sp = object;
}

void Proactor::setStreamSocketFactory(
    const bsl::shared_ptr<ntci::StreamSocketFactory>& streamSocketFactory)
{
    d_streamSocketFactory_sp = streamSocketFactory;
}

void Proactor::setStreamSocketFactory(
    const StreamSocketFactoryFunction& streamSocketFactory)
{
    bsl::shared_ptr<ntcd::StreamSocketFactory> object;
    object.createInplace(d_allocator_p, streamSocketFactory, d_allocator_p);

    d_streamSocketFactory_sp = object;
}

ntci::Waiter Proactor::registerWaiter(const ntca::WaiterOptions& waiterOptions)
{
    Proactor::Result* result =
        new (*d_allocator_p) Proactor::Result(d_allocator_p);

    result->d_options = waiterOptions;

    {
        LockGuard lockGuard(&d_waiterSetMutex);

        if (result->d_options.threadHandle() == bslmt::ThreadUtil::Handle()) {
            result->d_options.setThreadHandle(bslmt::ThreadUtil::self());
        }

        if (d_waiterSet.empty()) {
            d_threadHandle = result->d_options.threadHandle();
            if (!result->d_options.threadIndex().isNull()) {
                d_threadIndex = result->d_options.threadIndex().value();
            }
        }

        if (!d_config.metricCollection().isNull() &&
            d_config.metricCollection().value())
        {
            if (!d_config.metricCollectionPerWaiter().isNull() &&
                d_config.metricCollectionPerWaiter().value())
            {
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

                ntcm::MonitorableUtil::registerMonitorable(
                    result->d_metrics_sp);
            }
            else {
                result->d_metrics_sp = d_metrics_sp;
            }
        }

        d_waiterSet.insert(result);
    }

    d_monitor_sp->registerWaiter();

    return result;
}

void Proactor::deregisterWaiter(ntci::Waiter waiter)
{
    Proactor::Result* result = static_cast<Proactor::Result*>(waiter);

    bool flush = false;

    {
        LockGuard lockGuard(&d_waiterSetMutex);

        bsl::size_t n = d_waiterSet.erase(result);
        BSLS_ASSERT_OPT(n == 1);

        if (d_waiterSet.empty()) {
            d_threadHandle = bslmt::ThreadUtil::Handle();
            flush          = true;
        }
    }

    if (flush) {
        this->flush();
    }

    if (!d_config.metricCollection().isNull() &&
        d_config.metricCollection().value())
    {
        if (!d_config.metricCollectionPerWaiter().isNull() &&
            d_config.metricCollectionPerWaiter().value())
        {
            ntcm::MonitorableUtil::deregisterMonitorable(result->d_metrics_sp);
        }
    }

    d_monitor_sp->deregisterWaiter();

    d_allocator_p->deleteObject(result);
}

bsl::shared_ptr<ntci::Strand> Proactor::createStrand(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<Proactor> self = this->getSelf(this);

    bsl::shared_ptr<ntcs::Strand> strand;
    strand.createInplace(allocator, self, allocator);

    return strand;
}

ntsa::Error Proactor::attachSocket(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket)
{
    ntsa::Error error;

    ntsa::Handle handle = socket->handle();

    bsl::shared_ptr<ntcs::RegistryEntry> entry = d_registry_sp->add(handle);
    if (!entry) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_monitor_sp->add(entry->handle());
    if (error) {
        d_registry_sp->remove(handle);
        return error;
    }

    error = d_monitor_sp->update(entry->handle(), entry->interest());
    if (error) {
        d_registry_sp->remove(handle);
        return error;
    }

    bsl::shared_ptr<Work> work;
    {
        LockGuard lock(&d_workMapMutex);

        WorkMap::iterator it = d_workMap.find(handle);
        if (it != d_workMap.end()) {
            d_registry_sp->remove(handle);
            d_monitor_sp->remove(handle);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        work.createInplace(d_allocator_p,
                           d_machine_sp,
                           d_monitor_sp,
                           socket,
                           entry,
                           d_allocator_p);

        error = work->showError();
        if (error) {
            d_registry_sp->remove(handle);
            d_monitor_sp->remove(handle);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        d_workMap.insert(WorkMap::value_type(handle, work));
    }

    socket->setProactorContext(work);

    return ntsa::Error();
}

ntsa::Error Proactor::accept(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket)
{
    bsl::shared_ptr<Work> work =
        bslstl::SharedPtrUtil::staticCast<Work>(socket->getProactorContext());
    if (!work) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return work->initiateAccept();
}

ntsa::Error Proactor::connect(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    const ntsa::Endpoint&                        endpoint)
{
    bsl::shared_ptr<Work> work =
        bslstl::SharedPtrUtil::staticCast<Work>(socket->getProactorContext());
    if (!work) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return work->initiateConnect(endpoint);
}

ntsa::Error Proactor::send(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                           const bdlbb::Blob&                           data,
                           const ntsa::SendOptions& options)
{
    bsl::shared_ptr<Work> work =
        bslstl::SharedPtrUtil::staticCast<Work>(socket->getProactorContext());
    if (!work) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return work->initiateSend(data, options);
}

ntsa::Error Proactor::send(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                           const ntsa::Data&                            data,
                           const ntsa::SendOptions& options)
{
    bsl::shared_ptr<Work> work =
        bslstl::SharedPtrUtil::staticCast<Work>(socket->getProactorContext());
    if (!work) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return work->initiateSend(data, options);
}

ntsa::Error Proactor::receive(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    bdlbb::Blob*                                 data,
    const ntsa::ReceiveOptions&                  options)
{
    bsl::shared_ptr<Work> work =
        bslstl::SharedPtrUtil::staticCast<Work>(socket->getProactorContext());
    if (!work) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return work->initiateReceive(data, options);
}

ntsa::Error Proactor::shutdown(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    ntsa::ShutdownType::Value                    direction)
{
    bsl::shared_ptr<Work> work =
        bslstl::SharedPtrUtil::staticCast<Work>(socket->getProactorContext());
    if (!work) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return work->shutdown(direction);
}

ntsa::Error Proactor::cancel(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket)
{
    bsl::shared_ptr<Work> work =
        bslstl::SharedPtrUtil::staticCast<Work>(socket->getProactorContext());
    if (!work) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return work->cancel();
}

ntsa::Error Proactor::detachSocket(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket)
{
    ntsa::Error error;

    ntsa::Handle handle = socket->handle();

    {
        LockGuard lock(&d_workMapMutex);

        WorkMap::iterator it = d_workMap.find(handle);
        if (it == d_workMap.end()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        d_workMap.erase(it);
    }

    socket->setProactorContext(bsl::shared_ptr<void>());

#if NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMDAS
    ntci::SocketDetachedCallback detachCallback(
        [socket]() {
            socket->processSocketDetached();
        },
        socket->strand(),
        d_allocator_p);
#else
    ntci::SocketDetachedCallback detachCallback(
        NTCCFG_BIND(&ntci::ProactorSocket::processSocketDetached, socket),
        socket->strand(),
        d_allocator_p);
#endif

    d_registry_sp->removeAndGetReadyToDetach(handle,
                                             detachCallback,
                                             *d_detachFunctor_sp);

    return ntsa::Error();
}

ntsa::Error Proactor::removeDetached(
    const bsl::shared_ptr<ntcs::RegistryEntry>& entry)
{
    d_monitor_sp->remove(entry->handle());
    if (!entry->isProcessing() && entry->announceDetached(this->getSelf(this)))
    {
        entry->clear();
        Proactor::interruptOne();
    }
    return ntsa::Error();
}

ntsa::Error Proactor::closeAll()
{
    {
        LockGuard lock(&d_workMapMutex);
        d_workMap.clear();
    }

    d_chronology_sp->closeAll();
    d_registry_sp->closeAll(ntsa::k_INVALID_HANDLE);

    return ntsa::Error();
}

void Proactor::incrementLoad(const ntca::LoadBalancingOptions& options)
{
    bsl::size_t weight =
        options.weight().isNull() ? 1 : options.weight().value();

    d_load += weight;
}

void Proactor::decrementLoad(const ntca::LoadBalancingOptions& options)
{
    bsl::size_t weight =
        options.weight().isNull() ? 1 : options.weight().value();

    d_load -= weight;
}

void Proactor::run(ntci::Waiter waiter)
{
    while (d_run) {
        this->poll(waiter);
    }
}

void Proactor::poll(ntci::Waiter waiter)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    Proactor::Result* result = static_cast<Proactor::Result*>(waiter);
    NTCCFG_WARNING_UNUSED(result);

    BSLS_ASSERT(result->d_options.threadHandle() == bslmt::ThreadUtil::self());

    NTCS_PROACTORMETRICS_GET();

    // Block until a socket is readable, writable, has an error, or any timer
    // expires.

    bsl::vector<ntca::ReactorEvent> events;

    bdlb::NullableValue<bsls::TimeInterval> earliestTimerDue =
        d_chronology_sp->earliest();

    if (!earliestTimerDue.isNull()) {
        NTCD_PROACTOR_LOG_WAIT_TIMED_HIGH_PRECISION(earliestTimerDue.value());

        error = d_monitor_sp->dequeue(&events, earliestTimerDue.value());
    }
    else {
        NTCD_PROACTOR_LOG_WAIT_INDEFINITE();

        error = d_monitor_sp->dequeue(&events);
    }

    bsl::size_t numResults = 0;

    if (error) {
        if (error == ntsa::Error::e_WOULD_BLOCK) {
            NTCD_PROACTOR_LOG_WAIT_TIMEOUT();
            NTCS_PROACTORMETRICS_UPDATE_POLL(0, 0, 0);
        }
        else {
            NTCD_PROACTOR_LOG_WAIT_FAILURE(error);
        }
    }
    else {
        numResults = events.size();
        NTCD_PROACTOR_LOG_WAIT_RESULT(numResults);
    }

    if (numResults > 0) {
        bsl::size_t numReadable    = 0;
        bsl::size_t numWritable    = 0;
        bsl::size_t numErrors      = 0;
        bsl::size_t numTimers      = 0;
        bsl::size_t numDetachments = 0;

        for (bsl::size_t i = 0; i < numResults; ++i) {
            const ntca::ReactorEvent& event = events[i];

            ntsa::Handle descriptorHandle = event.handle();

            bsl::shared_ptr<ntcs::RegistryEntry> entry;
            if (!d_registry_sp->lookupAndMarkProcessingOngoing(
                    &entry,
                    descriptorHandle))
            {
                continue;
            }

            NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(descriptorHandle);

            NTCD_PROACTOR_LOG_EVENT(event);

            if (event.isError()) {
                NTCS_PROACTORMETRICS_UPDATE_ERROR_CALLBACK_TIME_BEGIN();
                if (entry->announceError(event)) {
                    ++numErrors;
                }
                NTCS_PROACTORMETRICS_UPDATE_ERROR_CALLBACK_TIME_END();
            }
            else if (event.isWritable()) {
                NTCS_PROACTORMETRICS_UPDATE_WRITE_CALLBACK_TIME_BEGIN();
                if (entry->announceWritable(event)) {
                    ++numWritable;
                }
                NTCS_PROACTORMETRICS_UPDATE_WRITE_CALLBACK_TIME_END();
            }
            else if (event.isReadable()) {
                NTCS_PROACTORMETRICS_UPDATE_READ_CALLBACK_TIME_BEGIN();
                if (entry->announceReadable(event)) {
                    ++numReadable;
                }
                NTCS_PROACTORMETRICS_UPDATE_READ_CALLBACK_TIME_END();
            }
            if (entry->decrementProcessCounter() == 0 &&
                entry->announceDetached(this->getSelf(this)))
            {
                entry->clear();
                ++numDetachments;
            }
        }

        const bsl::size_t numTotal =
            numReadable + numWritable + numErrors + numTimers + numDetachments;

        if (NTCCFG_UNLIKELY(numTotal == 0)) {
            NTCS_PROACTORMETRICS_UPDATE_SPURIOUS_WAKEUP();
            bslmt::ThreadUtil::yield();
        }
        else {
            NTCS_PROACTORMETRICS_UPDATE_POLL(numReadable,
                                             numWritable,
                                             numErrors);
        }
    }

    // Invoke functions deferred while processing each polled event and process
    // all expired timers.

    bsl::size_t numCycles = d_config.maxCyclesPerWait().value();
    while (numCycles != 0) {
        if (d_chronology_sp->hasAnyScheduledOrDeferred()) {
            d_chronology_sp->announce(d_dynamic);
            --numCycles;
        }
        else {
            break;
        }
    }
}

void Proactor::interruptOne()
{
    d_monitor_sp->interruptOne();
}

void Proactor::interruptAll()
{
    d_monitor_sp->interruptAll();
}

void Proactor::stop()
{
    d_run = false;
    d_monitor_sp->stop();
}

void Proactor::restart()
{
    d_run = true;
    d_monitor_sp->restart();
}

void Proactor::drainFunctions()
{
    d_chronology_sp->drain();
}

void Proactor::clearFunctions()
{
    d_chronology_sp->clearFunctions();
}

void Proactor::clearTimers()
{
    d_chronology_sp->clearTimers();
}

void Proactor::clearSockets()
{
    bsl::vector<bsl::shared_ptr<ntcs::RegistryEntry> > entryList;
    d_registry_sp->clear(&entryList, ntsa::k_INVALID_HANDLE);

    for (bsl::vector<bsl::shared_ptr<ntcs::RegistryEntry> >::iterator it =
             entryList.begin();
         it != entryList.end();
         ++it)
    {
        const bsl::shared_ptr<ntcs::RegistryEntry>& entry = *it;
        d_monitor_sp->remove(entry->handle());
    }

    entryList.clear();

    LockGuard lock(&d_workMapMutex);

    for (WorkMap::iterator it = d_workMap.begin(); it != d_workMap.end(); ++it)
    {
        const bsl::shared_ptr<Work>& work = it->second;
        work->d_socket_sp->setProactorContext(bsl::shared_ptr<void>());
    }

    d_workMap.clear();
}

void Proactor::clear()
{
    d_chronology_sp->clear();

    bsl::vector<bsl::shared_ptr<ntcs::RegistryEntry> > entryList;
    d_registry_sp->clear(&entryList, ntsa::k_INVALID_HANDLE);

    for (bsl::vector<bsl::shared_ptr<ntcs::RegistryEntry> >::iterator it =
             entryList.begin();
         it != entryList.end();
         ++it)
    {
        const bsl::shared_ptr<ntcs::RegistryEntry>& entry = *it;
        d_monitor_sp->remove(entry->handle());
    }

    entryList.clear();

    LockGuard lock(&d_workMapMutex);

    for (WorkMap::iterator it = d_workMap.begin(); it != d_workMap.end(); ++it)
    {
        const bsl::shared_ptr<Work>& work = it->second;
        work->d_socket_sp->setProactorContext(bsl::shared_ptr<void>());
    }

    d_workMap.clear();
}

void Proactor::execute(const Functor& functor)
{
    d_chronology_sp->execute(functor);
}

void Proactor::moveAndExecute(FunctorSequence* functorSequence,
                              const Functor&   functor)
{
    d_chronology_sp->moveAndExecute(functorSequence, functor);
}

bsl::shared_ptr<ntci::Timer> Proactor::createTimer(
    const ntca::TimerOptions&                  options,
    const bsl::shared_ptr<ntci::TimerSession>& session,
    bslma::Allocator*                          basicAllocator)
{
    return d_chronology_sp->createTimer(options, session, basicAllocator);
}

bsl::shared_ptr<ntci::Timer> Proactor::createTimer(
    const ntca::TimerOptions&  options,
    const ntci::TimerCallback& callback,
    bslma::Allocator*          basicAllocator)
{
    return d_chronology_sp->createTimer(options, callback, basicAllocator);
}

bsl::shared_ptr<ntci::DatagramSocket> Proactor::createDatagramSocket(
    const ntca::DatagramSocketOptions& options,
    bslma::Allocator*                  basicAllocator)
{
    return d_datagramSocketFactory_sp->createDatagramSocket(options,
                                                            basicAllocator);
}

bsl::shared_ptr<ntci::ListenerSocket> Proactor::createListenerSocket(
    const ntca::ListenerSocketOptions& options,
    bslma::Allocator*                  basicAllocator)
{
    return d_listenerSocketFactory_sp->createListenerSocket(options,
                                                            basicAllocator);
}

bsl::shared_ptr<ntci::StreamSocket> Proactor::createStreamSocket(
    const ntca::StreamSocketOptions& options,
    bslma::Allocator*                basicAllocator)
{
    return d_streamSocketFactory_sp->createStreamSocket(options,
                                                        basicAllocator);
}

bsl::shared_ptr<ntsa::Data> Proactor::createIncomingData()
{
    return d_dataPool_sp->createIncomingData();
}

bsl::shared_ptr<ntsa::Data> Proactor::createOutgoingData()
{
    return d_dataPool_sp->createOutgoingData();
}

bsl::shared_ptr<bdlbb::Blob> Proactor::createIncomingBlob()
{
    return d_dataPool_sp->createIncomingBlob();
}

bsl::shared_ptr<bdlbb::Blob> Proactor::createOutgoingBlob()
{
    return d_dataPool_sp->createOutgoingBlob();
}

void Proactor::createIncomingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_dataPool_sp->createIncomingBlobBuffer(blobBuffer);
}

void Proactor::createOutgoingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_dataPool_sp->createOutgoingBlobBuffer(blobBuffer);
}

bsl::size_t Proactor::numSockets() const
{
    return d_registry_sp->size();
}

bsl::size_t Proactor::maxSockets() const
{
    return static_cast<bsl::size_t>(-1);
}

bsl::size_t Proactor::numTimers() const
{
    return d_chronology_sp->numScheduled();
}

bsl::size_t Proactor::maxTimers() const
{
    return static_cast<bsl::size_t>(-1);
}

bsl::size_t Proactor::load() const
{
    return static_cast<bsl::size_t>(d_load);
}

bslmt::ThreadUtil::Handle Proactor::threadHandle() const
{
    LockGuard lock(&d_waiterSetMutex);
    return d_threadHandle;
}

bsl::size_t Proactor::threadIndex() const
{
    LockGuard lock(&d_waiterSetMutex);
    return d_threadIndex;
}

bsl::size_t Proactor::numWaiters() const
{
    LockGuard lock(&d_waiterSetMutex);
    return d_waiterSet.size();
}

bool Proactor::empty() const
{
    if (d_chronology_sp->hasAnyScheduledOrDeferred()) {
        return false;
    }

    if (d_chronology_sp->hasAnyRegistered()) {
        return false;
    }

    if (this->numSockets() != 0) {
        return false;
    }

    return true;
}

const bsl::shared_ptr<ntci::DataPool>& Proactor::dataPool() const
{
    return d_dataPool_sp;
}

const bsl::shared_ptr<ntci::Strand>& Proactor::strand() const
{
    return ntci::Strand::unspecified();
}

bsls::TimeInterval Proactor::currentTime() const
{
    return bdlt::CurrentTime::now();
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& Proactor::
    incomingBlobBufferFactory() const
{
    return d_dataPool_sp->incomingBlobBufferFactory();
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& Proactor::
    outgoingBlobBufferFactory() const
{
    return d_dataPool_sp->outgoingBlobBufferFactory();
}

const char* Proactor::name() const
{
    return "SIMULATION";
}

ProactorFactory::ProactorFactory(bslma::Allocator* basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ProactorFactory::~ProactorFactory()
{
}

bsl::shared_ptr<ntci::Proactor> ProactorFactory::createProactor(
    const ntca::ProactorConfig&        configuration,
    const bsl::shared_ptr<ntci::User>& user,
    bslma::Allocator*                  basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::Proactor> proactor;
    proactor.createInplace(allocator, configuration, user, allocator);

    return proactor;
}

}  // close package namespace
}  // close enterprise namespace
