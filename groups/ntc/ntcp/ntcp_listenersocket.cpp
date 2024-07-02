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

#include <ntcp_listenersocket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcp_listenersocket_cpp, "$Id$ $CSID$")

#include <ntcp_streamsocket.h>

#include <ntccfg_limits.h>
#include <ntci_log.h>
#include <ntci_monitorable.h>
#include <ntcm_monitorableutil.h>
#include <ntcs_async.h>
#include <ntcs_compat.h>
#include <ntcs_dispatch.h>
#include <ntcu_listenersocketsession.h>
#include <ntcu_listenersocketutil.h>
#include <ntsf_system.h>
#include <ntsi_streamsocket.h>
#include <bdlf_bind.h>
#include <bdls_pathutil.h>
#include <bdlt_currenttime.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsls_timeutil.h>

// Define to 1 to observe object using weak pointers, otherwise objects are
// observed using raw pointers.
#define NTCP_LISTENERSOCKET_OBSERVE_BY_WEAK_PTR 0

#define NTCP_LISTENERSOCKET_LOG_BIND_ATTEMPT(sourceEndpoint, reuseAddress)    \
    NTCI_LOG_DEBUG("Listener socket binding to source endpoint '%s'"          \
                   ": reuseAddress = %d",                                     \
                   (sourceEndpoint).text().c_str(),                           \
                   (int)(reuseAddress))

#define NTCP_LISTENERSOCKET_LOG_BIND_FAILURE(sourceEndpoint, error)           \
    NTCI_LOG_DEBUG("Listener socket failed to bind to source endpoint '%s'"   \
                   ": %s",                                                    \
                   (sourceEndpoint).text().c_str(),                           \
                   error.text().c_str())

#define NTCP_LISTENERSOCKET_LOG_BACKLOG_THROTTLE_APPLIED(timeToSubmit)        \
    NTCI_LOG_TRACE("Listener socket backlog throttle applied for %d "         \
                   "milliseconds",                                            \
                   (int)((timeToSubmit).totalMilliseconds()))

#define NTCP_LISTENERSOCKET_LOG_BACKLOG_THROTTLE_RELAXED()                    \
    NTCI_LOG_TRACE("Listener socket backlog throttle relaxed")

#define NTCP_LISTENERSOCKET_LOG_BACKLOG_UNDERFLOW()                           \
    NTCI_LOG_TRACE("Listener socket has emptied the backlog")

#define NTCP_LISTENERSOCKET_LOG_ACCEPT_RESULT(remoteEndpoint)                 \
    NTCI_LOG_TRACE("Listener socket has accepted a connection from %s",       \
                   (remoteEndpoint).text().c_str())

#define NTCP_LISTENERSOCKET_LOG_ACCEPT_FAILURE(error)                         \
    NTCI_LOG_DEBUG("Listener socket failed to accept socket: %s",             \
                   error.text().c_str())

#define NTCP_LISTENERSOCKET_LOG_ACCEPTED_SOURCE_ENDPOINT_FAILED(handle,       \
                                                                error)        \
    NTCI_LOG_DEBUG("Listener socket failed to get source endpoint for "       \
                   "accepted socket %d: %s",                                  \
                   handle,                                                    \
                   error.text().c_str())

#define NTCP_LISTENERSOCKET_LOG_ACCEPTED_REMOTE_ENDPOINT_FAILED(handle,       \
                                                                error)        \
    NTCI_LOG_DEBUG("Listener socket failed to get remote endpoint for "       \
                   "accepted socket %d: %s",                                  \
                   handle,                                                    \
                   error.text().c_str())

#define NTCP_LISTENERSOCKET_LOG_ACCEPTED_SOCKET_OPTIONS_FAILED(handle, error) \
    NTCI_LOG_DEBUG("Listener socket failed to set socket options for "        \
                   "accepted socket %d: %s",                                  \
                   handle,                                                    \
                   error.text().c_str())

#define NTCP_LISTENERSOCKET_LOG_ACCEPTED_SOCKET_IMPORT_FAILED(handle, error)  \
    NTCI_LOG_DEBUG("Listener socket failed to import accepted socket %d: %s", \
                   handle,                                                    \
                   error.text().c_str())

#define NTCP_LISTENERSOCKET_LOG_ACCEPT_QUEUE_FILLED(size)                     \
    NTCI_LOG_TRACE("Listener socket "                                         \
                   "has filled the accept queue up to %zu connections",       \
                   size)

#define NTCP_LISTENERSOCKET_LOG_ACCEPT_QUEUE_DRAINED(size)                    \
    NTCI_LOG_TRACE("Listener socket "                                         \
                   "has drained the accept queue down to %zu connections",    \
                   size)

#define NTCP_LISTENERSOCKET_LOG_ACCEPT_QUEUE_LOW_WATERMARK(lowWatermark,      \
                                                           size)              \
    NTCI_LOG_TRACE("Listener socket "                                         \
                   "has satisfied the accept queue low watermark of %zu "     \
                   "connections with an accept queue of %zu connections",     \
                   lowWatermark,                                              \
                   size)

#define NTCP_LISTENERSOCKET_LOG_ACCEPT_QUEUE_HIGH_WATERMARK(highWatermark,    \
                                                            size)             \
    NTCI_LOG_TRACE("Listener socket "                                         \
                   "has breached the accept queue high watermark of %zu "     \
                   "connections with an accept queue of %zu connections",     \
                   highWatermark,                                             \
                   size)

#define NTCP_LISTENERSOCKET_LOG_SHUTDOWN_RECEIVE()                            \
    NTCI_LOG_TRACE("Listener socket "                                         \
                   "is shutting down acceptance")

// Some versions of GCC erroneously warn ntcs::ObserverRef::d_shared may be
// uninitialized.
#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

namespace BloombergLP {
namespace ntcp {

void ListenerSocket::processSocketAccepted(
    const ntsa::Error&                         error,
    const bsl::shared_ptr<ntsi::StreamSocket>& streamSocket)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    if (NTCCFG_UNLIKELY(d_detachState.get() ==
                        ntcs::DetachState::e_DETACH_INITIATED))
    {
        return;
    }

    d_acceptPending = false;

    if (error) {
        if (error != ntsa::Error::e_CANCELLED) {
            NTCP_LISTENERSOCKET_LOG_ACCEPT_FAILURE(error);
            this->privateFailAccept(self, error);
        }
    }
    else {
        this->privateCompleteAccept(self, streamSocket);
    }

    this->privateInitiateAccept(self);
}

void ListenerSocket::processSocketError(const ntsa::Error& error)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (NTCCFG_UNLIKELY(d_detachState.get() ==
                        ntcs::DetachState::e_DETACH_INITIATED))
    {
        return;
    }

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    this->privateFail(self, error);
}

void ListenerSocket::processSocketDetached()
{
    NTCCFG_OBJECT_GUARD(&d_object);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    BSLS_ASSERT(d_detachState.get() == ntcs::DetachState::e_DETACH_INITIATED);
    d_detachState.set(ntcs::DetachState::e_DETACH_IDLE);
    BSLS_ASSERT(d_deferredCall);
    if (NTCCFG_LIKELY(d_deferredCall)) {
        NTCCFG_FUNCTION() deferredCall;
        deferredCall.swap(d_deferredCall);
        deferredCall();
    }
}

void ListenerSocket::processAcceptRateTimer(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_WARNING_UNUSED(timer);

    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        NTCP_LISTENERSOCKET_LOG_BACKLOG_THROTTLE_RELAXED();

        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      false,
                                      true);

        if (d_session_sp) {
            ntca::AcceptQueueEvent event;
            event.setType(ntca::AcceptQueueEventType::e_RATE_LIMIT_RELAXED);
            event.setContext(d_acceptQueue.context());

            ntcs::Dispatch::announceAcceptQueueRateLimitRelaxed(
                d_session_sp,
                self,
                event,
                d_sessionStrand_sp,
                ntci::Strand::unknown(),
                self,
                false,
                &d_mutex);
        }
    }
}

void ListenerSocket::processAcceptBackoffTimer(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_WARNING_UNUSED(timer);

    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      false,
                                      false);

        d_acceptBackoffTimer_sp->close();
        d_acceptBackoffTimer_sp.reset();
    }
}

void ListenerSocket::processAcceptDeadlineTimer(
    const bsl::shared_ptr<ntci::Timer>&                    timer,
    const ntca::TimerEvent&                                event,
    const bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>& entry)
{
    NTCCFG_WARNING_UNUSED(timer);

    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        ntsa::Error error = d_acceptQueue.removeCallbackEntry(entry);
        if (!error) {
            ntca::AcceptContext acceptContext;
            acceptContext.setError(ntsa::Error(ntsa::Error::e_WOULD_BLOCK));

            ntca::AcceptEvent acceptEvent;
            acceptEvent.setType(ntca::AcceptEventType::e_ERROR);
            acceptEvent.setContext(acceptContext);

            ntcq::AcceptCallbackQueueEntry::dispatch(
                entry,
                self,
                bsl::shared_ptr<ntci::StreamSocket>(),
                acceptEvent,
                d_proactorStrand_sp,
                self,
                false,
                &d_mutex);
        }
    }
}

void ListenerSocket::privateInitiateAccept(
    const bsl::shared_ptr<ListenerSocket>& self)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (d_acceptPending) {
        return;
    }

    if (!d_flowControlState.wantReceive()) {
        return;
    }

    if (!d_shutdownState.canReceive()) {
        return;
    }

    ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
    if (!proactorRef) {
        this->privateFailAccept(self, ntsa::Error(ntsa::Error::e_INVALID));
        return;
    }

    error = this->privateThrottleBacklog(self);
    if (error) {
        return;
    }

    error = proactorRef->accept(self);
    if (error) {
        NTCP_LISTENERSOCKET_LOG_ACCEPT_FAILURE(error);
        this->privateFailAccept(self, error);
        return;
    }

    d_acceptPending = true;
}

void ListenerSocket::privateCompleteAccept(
    const bsl::shared_ptr<ListenerSocket>&     self,
    const bsl::shared_ptr<ntsi::StreamSocket>& streamSocketBase)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (NTCCFG_UNLIKELY(d_acceptRateLimiter_sp)) {
        d_acceptRateLimiter_sp->submit(1);
    }

    ntca::StreamSocketOptions streamSocketOptions;
    ntcs::Compat::convert(&streamSocketOptions, d_options);

    // If the peer closes the connection immediately after connecting, on
    // some platforms this situation is detected by 'getpeername()' failing
    // for the accepted socket. Do not log or treat this error as an accept
    // error, just try accepting another connection. Note that this can
    // be detected in this function, or when calling StreamSocket::initialize,
    // since that function also tries to get the peer name.

    ntsa::Endpoint sourceEndpoint;
    error = streamSocketBase->sourceEndpoint(&sourceEndpoint);
    if (error) {
        // NTCP_LISTENERSOCKET_LOG_ACCEPTED_SOURCE_ENDPOINT_FAILED(
        //     streamSocketBase->handle(),
        //     error);
        // NTCS_METRICS_UPDATE_ACCEPT_FAILURE();
        return;
    }

    ntsa::Endpoint remoteEndpoint;
    error = streamSocketBase->remoteEndpoint(&remoteEndpoint);
    if (error) {
        // NTCP_LISTENERSOCKET_LOG_ACCEPTED_REMOTE_ENDPOINT_FAILED(
        //     streamSocketBase->handle(),
        //     error);
        // NTCS_METRICS_UPDATE_ACCEPT_FAILURE();
        return;
    }

    error = ntcs::Compat::configure(streamSocketBase, streamSocketOptions);
    if (error) {
        NTCP_LISTENERSOCKET_LOG_ACCEPTED_SOCKET_OPTIONS_FAILED(
            streamSocketBase->handle(),
            error);
        NTCS_METRICS_UPDATE_ACCEPT_FAILURE();
        return;
    }

    ntcs::ObserverRef<ntci::ProactorPool> proactorPoolRef(&d_proactorPool);
    if (!proactorPoolRef) {
        NTCS_METRICS_UPDATE_ACCEPT_FAILURE();
        return;
    }

    NTCP_LISTENERSOCKET_LOG_ACCEPT_RESULT(remoteEndpoint);

    bsl::shared_ptr<ntci::Proactor> proactor =
        proactorPoolRef->acquireProactor(d_options.loadBalancingOptions());

    bsl::shared_ptr<ntcs::Metrics> metrics;
    if (!d_options.metrics().isNull() && d_options.metrics().value()) {
        metrics = d_metrics_sp->parent();
    }
    else {
        metrics = d_metrics_sp;
    }

    bsl::shared_ptr<ntci::Resolver> resolver;
    {
        ntcs::ObserverRef<ntci::Resolver> resolverRef(&d_resolver);
        if (resolverRef) {
            resolver = resolverRef.getShared();
        }
    }

    bsl::shared_ptr<ntcp::StreamSocket> streamSocket;
    streamSocket.createInplace(d_allocator_p,
                               streamSocketOptions,
                               resolver,
                               proactor,
                               proactorPoolRef.getShared(),
                               metrics,
                               d_allocator_p);

    error = streamSocket->registerManager(d_manager_sp);
    if (error) {
        NTCP_LISTENERSOCKET_LOG_ACCEPTED_SOCKET_IMPORT_FAILED(
            streamSocketBase->handle(),
            error);
        NTCS_METRICS_UPDATE_ACCEPT_FAILURE();
        streamSocket->close();
        return;
    }

    error = streamSocket->open(d_transport, streamSocketBase, self);
    if (error) {
        NTCP_LISTENERSOCKET_LOG_ACCEPTED_SOCKET_IMPORT_FAILED(
            streamSocketBase->handle(),
            error);
        NTCS_METRICS_UPDATE_ACCEPT_FAILURE();
        streamSocket->close();

        if (error == ntsa::Error::e_LIMIT) {
            if (d_manager_sp) {
                ntcs::Dispatch::announceConnectionLimit(
                    d_manager_sp,
                    self,
                    d_managerStrand_sp,
                    ntci::Strand::unknown(),
                    self,
                    true,
                    &d_mutex);
            }
        }

        return;
    }

    NTCS_METRICS_UPDATE_ACCEPT_COMPLETE();

    {
        ntcq::AcceptQueueEntry entry;
        entry.setStreamSocket(streamSocket);
        entry.setTimestamp(bsls::TimeUtil::getTimer());

        d_acceptQueue.pushEntry(entry);
    }

    NTCP_LISTENERSOCKET_LOG_ACCEPT_QUEUE_FILLED(d_acceptQueue.size());

    NTCS_METRICS_UPDATE_ACCEPT_QUEUE_SIZE(d_acceptQueue.size());

    while (true) {
        bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry> callbackEntry;
        error = d_acceptQueue.popCallbackEntry(&callbackEntry);
        if (error) {
            break;
        }

        BSLS_ASSERT(d_acceptQueue.hasEntry());

        ntcq::AcceptQueueEntry& entry = d_acceptQueue.frontEntry();

        bsl::shared_ptr<ntci::StreamSocket> streamSocket =
            entry.streamSocket();

        NTCS_METRICS_UPDATE_ACCEPT_QUEUE_DELAY(entry.delay());

        d_acceptQueue.popEntry();

        NTCP_LISTENERSOCKET_LOG_ACCEPT_QUEUE_DRAINED(d_acceptQueue.size());

        NTCS_METRICS_UPDATE_ACCEPT_QUEUE_SIZE(d_acceptQueue.size());

        ntca::AcceptContext acceptContext;

        ntca::AcceptEvent acceptEvent;
        acceptEvent.setType(ntca::AcceptEventType::e_COMPLETE);
        acceptEvent.setContext(acceptContext);

        ntcq::AcceptCallbackQueueEntry::dispatch(callbackEntry,
                                                 self,
                                                 streamSocket,
                                                 acceptEvent,
                                                 d_proactorStrand_sp,
                                                 self,
                                                 false,
                                                 &d_mutex);
    }

    if (d_acceptQueue.authorizeLowWatermarkEvent()) {
        NTCP_LISTENERSOCKET_LOG_ACCEPT_QUEUE_LOW_WATERMARK(
            d_acceptQueue.lowWatermark(),
            d_acceptQueue.size());

        if (d_session_sp) {
            ntca::AcceptQueueEvent event;
            event.setType(ntca::AcceptQueueEventType::e_LOW_WATERMARK);
            event.setContext(d_acceptQueue.context());

            ntcs::Dispatch::announceAcceptQueueLowWatermark(
                d_session_sp,
                self,
                event,
                d_sessionStrand_sp,
                d_proactorStrand_sp,
                self,
                false,
                &d_mutex);
        }
    }

    if (d_acceptQueue.authorizeHighWatermarkEvent()) {
        NTCP_LISTENERSOCKET_LOG_ACCEPT_QUEUE_HIGH_WATERMARK(
            d_acceptQueue.highWatermark(),
            d_acceptQueue.size());

        this->privateApplyFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      ntca::FlowControlMode::e_IMMEDIATE,
                                      false,
                                      false);

        if (d_session_sp) {
            ntca::AcceptQueueEvent event;
            event.setType(ntca::AcceptQueueEventType::e_HIGH_WATERMARK);
            event.setContext(d_acceptQueue.context());

            ntcs::Dispatch::announceAcceptQueueHighWatermark(
                d_session_sp,
                self,
                event,
                d_sessionStrand_sp,
                d_proactorStrand_sp,
                self,
                false,
                &d_mutex);
        }
    }
}

void ListenerSocket::privateFailAccept(
    const bsl::shared_ptr<ListenerSocket>& self,
    const ntsa::Error&                     error)
{
    if (error == ntsa::Error::e_LIMIT) {
        this->privateApplyFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      ntca::FlowControlMode::e_IMMEDIATE,
                                      false,
                                      false);

        ntca::TimerOptions timerOptions;
        timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
        timerOptions.setOneShot(true);

        ntci::TimerCallback timerCallback = this->createTimerCallback(
            bdlf::MemFnUtil::memFn(&ListenerSocket::processAcceptBackoffTimer,
                                   self),
            d_allocator_p);

        d_acceptBackoffTimer_sp =
            this->createTimer(timerOptions, timerCallback, d_allocator_p);

        d_acceptBackoffTimer_sp->schedule(this->currentTime() +
                                          bsls::TimeInterval(1));

        ntca::ErrorContext context;
        context.setError(error);

        ntca::ErrorEvent event;
        event.setType(ntca::ErrorEventType::e_TRANSPORT);
        event.setContext(context);

        if (d_session_sp) {
            ntcs::Dispatch::announceError(d_session_sp,
                                          self,
                                          event,
                                          d_sessionStrand_sp,
                                          d_proactorStrand_sp,
                                          self,
                                          false,
                                          &d_mutex);
        }
    }
    else {
        this->privateFail(self, error);
    }
}

void ListenerSocket::privateFail(const bsl::shared_ptr<ListenerSocket>& self,
                                 const ntsa::Error&                     error)
{
    ntca::ErrorContext context;
    context.setError(error);

    ntca::ErrorEvent event;
    event.setType(ntca::ErrorEventType::e_TRANSPORT);
    event.setContext(context);

    this->privateFail(self, event);
}

void ListenerSocket::privateFail(const bsl::shared_ptr<ListenerSocket>& self,
                                 const ntca::ErrorEvent&                event)
{
    this->privateApplyFlowControl(self,
                                  ntca::FlowControlType::e_BOTH,
                                  ntca::FlowControlMode::e_IMMEDIATE,
                                  false,
                                  true);

    d_flowControlState.close();

    if (d_session_sp) {
        ntcs::Dispatch::announceError(d_session_sp,
                                      self,
                                      event,
                                      d_sessionStrand_sp,
                                      d_proactorStrand_sp,
                                      self,
                                      false,
                                      &d_mutex);
    }

    this->privateShutdown(self,
                          ntsa::ShutdownType::e_BOTH,
                          ntsa::ShutdownMode::e_IMMEDIATE,
                          false);
}

ntsa::Error ListenerSocket::privateShutdown(
    const bsl::shared_ptr<ListenerSocket>& self,
    ntsa::ShutdownType::Value              direction,
    ntsa::ShutdownMode::Value              mode,
    bool                                   defer)
{
    NTCCFG_WARNING_UNUSED(mode);

    bool shutdownReceive = false;
    bool shutdownSend    = false;

    if (direction == ntsa::ShutdownType::e_RECEIVE ||
        direction == ntsa::ShutdownType::e_BOTH)
    {
        shutdownReceive = true;
    }

    if (direction == ntsa::ShutdownType::e_SEND ||
        direction == ntsa::ShutdownType::e_BOTH)
    {
        shutdownSend = true;
    }

    const bool closeAnnouncementRequired =
        d_closeCallback && d_shutdownState.completed();

    if (shutdownReceive) {
        if (d_shutdownState.canReceive()) {
            this->privateShutdownReceive(self,
                                         ntsa::ShutdownOrigin::e_SOURCE,
                                         defer);
        }
    }

    if (shutdownSend) {
        if (d_shutdownState.canSend()) {
            this->privateShutdownSend(self, defer);
        }
    }

    if (closeAnnouncementRequired) {
        d_closeCallback.dispatch(ntci::Strand::unknown(),
                                 self,
                                 true,
                                 &d_mutex);
        d_closeCallback.reset();
    }

    return ntsa::Error();
}

void ListenerSocket::privateShutdownSend(
    const bsl::shared_ptr<ListenerSocket>& self,
    bool                                   defer)
{
    ntcs::ShutdownContext context;
    if (d_shutdownState.tryShutdownSend(&context, false)) {
        this->privateShutdownSequence(self,
                                      ntsa::ShutdownOrigin::e_SOURCE,
                                      context,
                                      defer);
    }
}

void ListenerSocket::privateShutdownReceive(
    const bsl::shared_ptr<ListenerSocket>& self,
    ntsa::ShutdownOrigin::Value            origin,
    bool                                   defer)
{
    ntcs::ShutdownContext context;
    if (d_shutdownState.tryShutdownReceive(&context, false, origin)) {
        this->privateShutdownSequence(self, origin, context, defer);
    }
}

void ListenerSocket::privateShutdownSequence(
    const bsl::shared_ptr<ListenerSocket>& self,
    ntsa::ShutdownOrigin::Value            origin,
    const ntcs::ShutdownContext&           context,
    bool                                   defer)
{
    NTCCFG_WARNING_UNUSED(origin);

    // MRM: Always defer to properly clean up pending operations?
    defer = true;

    // First handle flow control and detachment from the reactor, if necessary.

    bool asyncDetachmentStarted = false;
    if (context.shutdownCompleted()) {
        asyncDetachmentStarted = this->privateCloseFlowControl(self, defer);
    }
    else {
        if (context.shutdownSend()) {
            this->privateApplyFlowControl(self,
                                          ntca::FlowControlType::e_SEND,
                                          ntca::FlowControlMode::e_IMMEDIATE,
                                          defer,
                                          true);
        }

        if (context.shutdownReceive()) {
            this->privateApplyFlowControl(self,
                                          ntca::FlowControlType::e_RECEIVE,
                                          ntca::FlowControlMode::e_IMMEDIATE,
                                          defer,
                                          true);
        }
    }

    if (!asyncDetachmentStarted) {
        privateShutdownSequencePart2(self, context, defer);
    }
    else {
        BSLS_ASSERT(!d_deferredCall);
        d_deferredCall =
            NTCCFG_BIND(&ListenerSocket::privateShutdownSequencePart2,
                        this,
                        self,
                        context,
                        defer);
    }
}

void ListenerSocket::privateShutdownSequencePart2(
    const bsl::shared_ptr<ListenerSocket>& self,
    const ntcs::ShutdownContext&           context,
    bool                                   defer)
{
    NTCI_LOG_CONTEXT();
    // Second handle socket shutdown.

    if (context.shutdownSend()) {
        if (d_socket_sp) {
            d_socket_sp->shutdown(ntsa::ShutdownType::e_SEND);
        }
    }

    if (context.shutdownReceive()) {
        if (d_socket_sp) {
            d_socket_sp->shutdown(ntsa::ShutdownType::e_RECEIVE);
        }
    }

    // Third handle internal data structures and announce events.

    if (context.shutdownInitiated()) {
        if (d_session_sp) {
            ntca::ShutdownEvent event;
            event.setType(ntca::ShutdownEventType::e_INITIATED);
            event.setContext(context.base());

            ntcs::Dispatch::announceShutdownInitiated(d_session_sp,
                                                      self,
                                                      event,
                                                      d_sessionStrand_sp,
                                                      d_proactorStrand_sp,
                                                      self,
                                                      defer,
                                                      &d_mutex);
        }
    }

    if (context.shutdownSend()) {
        // Note that the application of flow control and shutting down
        // of the socket in the send direction is handled earlier in this
        // function.

        // NTCP_LISTENERSOCKET_LOG_SHUTDOWN_SEND();

        if (d_session_sp) {
            ntca::ShutdownEvent event;
            event.setType(ntca::ShutdownEventType::e_SEND);
            event.setContext(context.base());

            ntcs::Dispatch::announceShutdownSend(d_session_sp,
                                                 self,
                                                 event,
                                                 d_sessionStrand_sp,
                                                 d_proactorStrand_sp,
                                                 self,
                                                 defer,
                                                 &d_mutex);
        }
    }

    if (context.shutdownReceive()) {
        // Note that the application of flow control and shutting down
        // of the socket in the receive direction is handled earlier in this
        // function.

        NTCP_LISTENERSOCKET_LOG_SHUTDOWN_RECEIVE();

        if (d_acceptRateTimer_sp) {
            d_acceptRateTimer_sp->close();
            d_acceptRateTimer_sp.reset();
        }

        if (d_acceptBackoffTimer_sp) {
            d_acceptBackoffTimer_sp->close();
            d_acceptBackoffTimer_sp.reset();
        }

        this->privateApplyFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      ntca::FlowControlMode::e_IMMEDIATE,
                                      defer,
                                      true);

        bsl::vector<bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry> >
            callbackEntryVector;

        d_acceptQueue.popAllCallbackEntries(&callbackEntryVector);

        ntca::AcceptContext acceptContext;
        acceptContext.setError(ntsa::Error(ntsa::Error::e_EOF));

        ntca::AcceptEvent acceptEvent;
        acceptEvent.setType(ntca::AcceptEventType::e_ERROR);
        acceptEvent.setContext(acceptContext);

        for (bsl::size_t i = 0; i < callbackEntryVector.size(); ++i) {
            ntcq::AcceptCallbackQueueEntry::dispatch(
                callbackEntryVector[i],
                self,
                bsl::shared_ptr<ntci::StreamSocket>(),
                acceptEvent,
                d_proactorStrand_sp,
                self,
                defer,
                &d_mutex);
        }

        callbackEntryVector.clear();

        // Force the announcement of an accept queue low watermark event
        // to give the user the opportunity to accept EOF.

        if (d_session_sp) {
            ntca::AcceptQueueEvent event;
            event.setType(ntca::AcceptQueueEventType::e_LOW_WATERMARK);
            event.setContext(d_acceptQueue.context());

            ntcs::Dispatch::announceAcceptQueueLowWatermark(
                d_session_sp,
                self,
                event,
                d_sessionStrand_sp,
                d_proactorStrand_sp,
                self,
                defer,
                &d_mutex);
        }

        if (d_session_sp) {
            ntca::ShutdownEvent event;
            event.setType(ntca::ShutdownEventType::e_RECEIVE);
            event.setContext(context.base());

            ntcs::Dispatch::announceShutdownReceive(d_session_sp,
                                                    self,
                                                    event,
                                                    d_sessionStrand_sp,
                                                    d_proactorStrand_sp,
                                                    self,
                                                    defer,
                                                    &d_mutex);
        }
    }

    if (context.shutdownCompleted()) {
        if (d_session_sp) {
            ntca::ShutdownEvent event;
            event.setType(ntca::ShutdownEventType::e_COMPLETE);
            event.setContext(context.base());

            ntcs::Dispatch::announceShutdownComplete(d_session_sp,
                                                     self,
                                                     event,
                                                     d_sessionStrand_sp,
                                                     d_proactorStrand_sp,
                                                     self,
                                                     defer,
                                                     &d_mutex);
        }

        // Note that detachment from the proactor is handled earlier in this
        // function.

        ntcs::ObserverRef<ntci::ProactorPool> proactorPoolRef(&d_proactorPool);
        if (proactorPoolRef) {
            ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
            if (proactorRef) {
                proactorPoolRef->releaseProactor(
                    proactorRef.getShared(),
                    d_options.loadBalancingOptions());
            }
        }

        d_socket_sp.reset();
        d_systemHandle = ntsa::k_INVALID_HANDLE;

        NTCI_LOG_TRACE("Listener socket closed descriptor %d",
                       (int)(d_publicHandle));

        ntcs::Dispatch::announceClosed(d_manager_sp,
                                       self,
                                       d_managerStrand_sp,
                                       d_proactorStrand_sp,
                                       self,
                                       defer,
                                       &d_mutex);

        if (d_closeCallback) {
            d_closeCallback.dispatch(ntci::Strand::unknown(),
                                     self,
                                     true,
                                     &d_mutex);
            d_closeCallback.reset();
        }

        d_resolver.reset();

        d_sessionStrand_sp.reset();
        d_session_sp.reset();

        d_managerStrand_sp.reset();
        d_manager_sp.reset();
    }
    this->moveAndExecute(&d_deferredCalls, ntci::Executor::Functor());
    d_deferredCalls.clear();
}

ntsa::Error ListenerSocket::privateRelaxFlowControl(
    const bsl::shared_ptr<ListenerSocket>& self,
    ntca::FlowControlType::Value           direction,
    bool                                   defer,
    bool                                   unlock)
{
    bool relaxSend    = false;
    bool relaxReceive = false;

    switch (direction) {
    case ntca::FlowControlType::e_SEND:
        relaxSend = true;
        break;
    case ntca::FlowControlType::e_RECEIVE:
        relaxReceive = true;
        break;
    case ntca::FlowControlType::e_BOTH:
        relaxSend    = true;
        relaxReceive = true;
        break;
    }

    ntcs::FlowControlContext context;
    if (d_flowControlState.relax(&context, direction, unlock)) {
        NTCCFG_WARNING_UNUSED(relaxSend);

        if (relaxReceive) {
            if (context.enableReceive()) {
                if (d_shutdownState.canReceive()) {
                    if (d_session_sp) {
                        ntca::AcceptQueueEvent event;
                        event.setType(ntca::AcceptQueueEventType::
                                          e_FLOW_CONTROL_RELAXED);
                        event.setContext(d_acceptQueue.context());

                        ntcs::Dispatch::announceAcceptQueueFlowControlRelaxed(
                            d_session_sp,
                            self,
                            event,
                            d_sessionStrand_sp,
                            ntci::Strand::unknown(),
                            self,
                            defer,
                            &d_mutex);
                    }

                    this->privateInitiateAccept(self);
                }
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error ListenerSocket::privateApplyFlowControl(
    const bsl::shared_ptr<ListenerSocket>& self,
    ntca::FlowControlType::Value           direction,
    ntca::FlowControlMode::Value           mode,
    bool                                   defer,
    bool                                   lock)
{
    NTCCFG_WARNING_UNUSED(mode);

    bool applySend    = false;
    bool applyReceive = false;

    switch (direction) {
    case ntca::FlowControlType::e_SEND:
        applySend = true;
        break;
    case ntca::FlowControlType::e_RECEIVE:
        applyReceive = true;
        break;
    case ntca::FlowControlType::e_BOTH:
        applySend    = true;
        applyReceive = true;
        break;
    }

    ntcs::FlowControlContext context;
    if (d_flowControlState.apply(&context, direction, lock)) {
        NTCCFG_WARNING_UNUSED(applySend);

        if (applyReceive) {
            if (!context.enableReceive()) {
                if (d_session_sp) {
                    ntca::AcceptQueueEvent event;
                    event.setType(
                        ntca::AcceptQueueEventType::e_FLOW_CONTROL_APPLIED);
                    event.setContext(d_acceptQueue.context());

                    ntcs::Dispatch::announceAcceptQueueFlowControlApplied(
                        d_session_sp,
                        self,
                        event,
                        d_sessionStrand_sp,
                        ntci::Strand::unknown(),
                        self,
                        defer,
                        &d_mutex);
                }

                // Handled at the completion of the last accept.
            }
        }
    }

    return ntsa::Error();
}

bool ListenerSocket::privateCloseFlowControl(
    const bsl::shared_ptr<ListenerSocket>& self,
    bool                                   defer)
{
    bool applyReceive = true;

    ntcs::FlowControlContext context;
    if (d_flowControlState.apply(&context,
                                 ntca::FlowControlType::e_BOTH,
                                 true))
    {
        if (applyReceive) {
            if (!context.enableReceive()) {
                if (d_session_sp) {
                    ntca::AcceptQueueEvent event;
                    event.setType(
                        ntca::AcceptQueueEventType::e_FLOW_CONTROL_APPLIED);
                    event.setContext(d_acceptQueue.context());

                    ntcs::Dispatch::announceAcceptQueueFlowControlApplied(
                        d_session_sp,
                        self,
                        event,
                        d_sessionStrand_sp,
                        ntci::Strand::unknown(),
                        self,
                        defer,
                        &d_mutex);
                }
            }
        }
    }

    if (d_systemHandle != ntsa::k_INVALID_HANDLE) {
        ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
        if (proactorRef) {
            BSLS_ASSERT(d_detachState.get() !=
                        ntcs::DetachState::e_DETACH_INITIATED);
            proactorRef->cancel(self);
            const ntsa::Error error = proactorRef->detachSocket(self);
            if (NTCCFG_UNLIKELY(error)) {
                return false;
            }
            else {
                d_detachState.set(ntcs::DetachState::e_DETACH_INITIATED);
                return true;
            }
        }
    }

    return false;
}

ntsa::Error ListenerSocket::privateThrottleBacklog(
    const bsl::shared_ptr<ListenerSocket>& self)
{
    NTCI_LOG_CONTEXT();

    if (NTCCFG_UNLIKELY(d_acceptRateLimiter_sp)) {
        bsls::TimeInterval now = this->currentTime();
        if (NTCCFG_UNLIKELY(d_acceptRateLimiter_sp->wouldExceedBandwidth(now)))
        {
            bsls::TimeInterval timeToSubmit =
                d_acceptRateLimiter_sp->calculateTimeToSubmit(now);

            bsls::TimeInterval nextAcceptAttemptTime = now + timeToSubmit;

            if (NTCCFG_UNLIKELY(!d_acceptRateTimer_sp)) {
                ntca::TimerOptions timerOptions;
                timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
                timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

                ntci::TimerCallback timerCallback = this->createTimerCallback(
                    bdlf::MemFnUtil::memFn(
                        &ListenerSocket::processAcceptRateTimer,
                        self),
                    d_allocator_p);

                d_acceptRateTimer_sp = this->createTimer(timerOptions,
                                                         timerCallback,
                                                         d_allocator_p);
            }

            NTCP_LISTENERSOCKET_LOG_BACKLOG_THROTTLE_APPLIED(timeToSubmit);

            this->privateApplyFlowControl(self,
                                          ntca::FlowControlType::e_RECEIVE,
                                          ntca::FlowControlMode::e_IMMEDIATE,
                                          false,
                                          true);

            d_acceptRateTimer_sp->schedule(nextAcceptAttemptTime);

            if (d_session_sp) {
                ntca::AcceptQueueEvent event;
                event.setType(
                    ntca::AcceptQueueEventType::e_RATE_LIMIT_APPLIED);
                event.setContext(d_acceptQueue.context());

                ntcs::Dispatch::announceAcceptQueueRateLimitApplied(
                    d_session_sp,
                    self,
                    event,
                    d_sessionStrand_sp,
                    ntci::Strand::unknown(),
                    self,
                    true,
                    &d_mutex);
            }

            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }
    }

    return ntsa::Error();
}

bool ListenerSocket::isStream() const
{
    return true;
}

bool ListenerSocket::isListener() const
{
    return true;
}

ntsa::Error ListenerSocket::privateOpen(
    const bsl::shared_ptr<ListenerSocket>& self)
{
    if (d_systemHandle != ntsa::k_INVALID_HANDLE) {
        return ntsa::Error();
    }

    return this->privateOpen(self, d_options.transport());
}

ntsa::Error ListenerSocket::privateOpen(
    const bsl::shared_ptr<ListenerSocket>& self,
    ntsa::Transport::Value                 transport)
{
    ntsa::Error error;

    if (d_systemHandle != ntsa::k_INVALID_HANDLE) {
        if (d_transport == transport) {
            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        if (d_options.transport() != ntsa::Transport::e_UNDEFINED &&
            transport != d_options.transport())
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    error = ntcu::ListenerSocketUtil::validateTransport(transport);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntsi::ListenerSocket> listenerSocket;
    if (d_socket_sp) {
        listenerSocket = d_socket_sp;
    }
    else {
        listenerSocket = ntsf::System::createListenerSocket(d_allocator_p);
    }

    error = this->privateOpen(self, transport, listenerSocket);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error ListenerSocket::privateOpen(
    const bsl::shared_ptr<ListenerSocket>& self,
    const ntsa::Endpoint&                  endpoint)
{
    return this->privateOpen(
        self,
        endpoint.transport(ntsa::TransportMode::e_STREAM));
}

ntsa::Error ListenerSocket::privateOpen(
    const bsl::shared_ptr<ListenerSocket>& self,
    ntsa::Transport::Value                 transport,
    ntsa::Handle                           handle)
{
    ntsa::Error error;

    bsl::shared_ptr<ntsi::ListenerSocket> listenerSocket;
    if (d_socket_sp) {
        listenerSocket = d_socket_sp;
        error          = listenerSocket->acquire(handle);
        if (error) {
            return error;
        }
    }
    else {
        ntsf::System::createListenerSocket(handle, d_allocator_p);
    }

    error = this->privateOpen(self, transport, listenerSocket);
    if (error) {
        listenerSocket->release();
        return error;
    }

    return ntsa::Error();
}

ntsa::Error ListenerSocket::privateOpen(
    const bsl::shared_ptr<ListenerSocket>&       self,
    ntsa::Transport::Value                       transport,
    const bsl::shared_ptr<ntsi::ListenerSocket>& listenerSocket)
{
    NTCCFG_WARNING_UNUSED(self);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (d_systemHandle != ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = ntcu::ListenerSocketUtil::validateTransport(transport);
    if (error) {
        return error;
    }

    if (listenerSocket->handle() == ntsa::k_INVALID_HANDLE) {
        error = listenerSocket->open(transport);
        if (error) {
            return error;
        }
    }

    ntsa::Handle handle = listenerSocket->handle();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(handle);

    ntsa::Endpoint sourceEndpoint;

    error = ntcs::Compat::configure(listenerSocket, d_options);
    if (error) {
        return error;
    }

    error = listenerSocket->setBlocking(false);
    if (error) {
        return error;
    }

    if (!d_options.sourceEndpoint().isNull()) {
        NTCP_LISTENERSOCKET_LOG_BIND_ATTEMPT(
            d_options.sourceEndpoint().value(),
            d_options.reuseAddress());

        error = listenerSocket->bind(d_options.sourceEndpoint().value(),
                                     d_options.reuseAddress());
        if (error) {
            NTCP_LISTENERSOCKET_LOG_BIND_FAILURE(
                d_options.sourceEndpoint().value(),
                error);
            return error;
        }
    }

    error = listenerSocket->sourceEndpoint(&sourceEndpoint);
    if (error) {
        sourceEndpoint.reset();
    }

    d_systemHandle   = handle;
    d_publicHandle   = handle;
    d_transport      = transport;
    d_sourceEndpoint = sourceEndpoint;
    d_socket_sp      = listenerSocket;

    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    NTCI_LOG_TRACE("Listener socket opened descriptor %d",
                   (int)(d_publicHandle));

    return ntsa::Error();
}

void ListenerSocket::processSourceEndpointResolution(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const ntsa::Endpoint&                  endpoint,
    const ntca::GetEndpointEvent&          getEndpointEvent,
    const ntca::BindOptions&               bindOptions,
    const ntci::BindCallback&              bindCallback)
{
    NTCCFG_WARNING_UNUSED(resolver);

    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    ntsa::Error       error;
    ntca::BindContext bindContext;

    if (getEndpointEvent.type() == ntca::GetEndpointEventType::e_ERROR) {
        error = getEndpointEvent.context().error();
    }
    else {
        bindContext.setName(getEndpointEvent.context().authority());

        if (getEndpointEvent.context().latency() != bsls::TimeInterval()) {
            bindContext.setLatency(getEndpointEvent.context().latency());
        }

        if (!getEndpointEvent.context().nameServer().isNull()) {
            bindContext.setNameServer(
                getEndpointEvent.context().nameServer().value());
        }

        if (getEndpointEvent.context().source() !=
            ntca::ResolverSource::e_UNKNOWN)
        {
            bindContext.setSource(getEndpointEvent.context().source());
        }
    }

    if (!error) {
        error = this->privateOpen(self, endpoint);
    }

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);

    if (!error) {
        NTCP_LISTENERSOCKET_LOG_BIND_ATTEMPT(endpoint,
                                             d_options.reuseAddress());

        error = d_socket_sp->bind(endpoint, d_options.reuseAddress());
        if (error) {
            NTCP_LISTENERSOCKET_LOG_BIND_FAILURE(endpoint, error);
        }
    }

    if (!error) {
        error = d_socket_sp->sourceEndpoint(&d_sourceEndpoint);
    }

    ntca::BindEvent bindEvent;
    if (!error) {
        bindEvent.setType(ntca::BindEventType::e_COMPLETE);
        bindContext.setEndpoint(d_sourceEndpoint);
    }
    else {
        bindEvent.setType(ntca::BindEventType::e_ERROR);
        bindContext.setError(error);
    }

    bindEvent.setContext(bindContext);

    if (bindCallback) {
        const bool defer = !bindOptions.recurse();

        bindCallback.dispatch(self,
                              bindEvent,
                              ntci::Strand::unknown(),
                              self,
                              defer,
                              &d_mutex);
    }
}

ListenerSocket::ListenerSocket(
    const ntca::ListenerSocketOptions&         options,
    const bsl::shared_ptr<ntci::Resolver>&     resolver,
    const bsl::shared_ptr<ntci::Proactor>&     proactor,
    const bsl::shared_ptr<ntci::ProactorPool>& proactorPool,
    const bsl::shared_ptr<ntcs::Metrics>&      metrics,
    bslma::Allocator*                          basicAllocator)
: d_object("ntcp::ListenerSocket")
, d_mutex()
, d_systemHandle(ntsa::k_INVALID_HANDLE)
, d_publicHandle(ntsa::k_INVALID_HANDLE)
, d_transport(ntsa::Transport::e_UNDEFINED)
, d_sourceEndpoint()
#if NTCP_LISTENERSOCKET_OBSERVE_BY_WEAK_PTR
, d_resolver(bsl::weak_ptr<ntci::Resolver>(resolver))
, d_proactor(bsl::weak_ptr<ntci::Proactor>(proactor))
, d_proactorPool(bsl::weak_ptr<ntci::ProactorPool>(proactorPool))
#else
, d_resolver(resolver.get())
, d_proactor(proactor.get())
, d_proactorPool(proactorPool.get())
#endif
, d_proactorStrand_sp()
, d_manager_sp()
, d_managerStrand_sp()
, d_session_sp()
, d_sessionStrand_sp()
, d_dataPool_sp(proactor->dataPool())
, d_incomingBufferFactory_sp(proactor->incomingBlobBufferFactory())
, d_outgoingBufferFactory_sp(proactor->outgoingBlobBufferFactory())
, d_metrics_sp()
, d_flowControlState()
, d_shutdownState()
, d_acceptQueue(basicAllocator)
, d_acceptRateLimiter_sp()
, d_acceptRateTimer_sp()
, d_acceptBackoffTimer_sp()
, d_acceptPending(false)
, d_acceptGreedily(NTCCFG_DEFAULT_LISTENER_SOCKET_ACCEPT_GREEDILY)
, d_options(options)
, d_detachState(ntcs::DetachState::e_DETACH_IDLE)
, d_deferredCall()
, d_closeCallback(bslma::Default::allocator(basicAllocator))
, d_deferredCalls(bslma::Default::allocator(basicAllocator))
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (!d_options.acceptQueueLowWatermark().isNull()) {
        d_acceptQueue.setLowWatermark(
            d_options.acceptQueueLowWatermark().value());
    }

    if (!d_options.acceptQueueHighWatermark().isNull()) {
        d_acceptQueue.setHighWatermark(
            d_options.acceptQueueHighWatermark().value());
    }

    if (!d_options.acceptGreedily().isNull()) {
        d_acceptGreedily = d_options.acceptGreedily().value();
    }

    if (proactor->maxThreads() > 1) {
        d_proactorStrand_sp = proactor->createStrand(d_allocator_p);
    }

    if (!d_managerStrand_sp) {
        d_managerStrand_sp = d_proactorStrand_sp;
    }

    if (!d_options.metrics().isNull() && d_options.metrics().value()) {
        ntsa::Guid guid = ntsa::Guid::generate();
        char       guidText[ntsa::Guid::SIZE_TEXT];
        guid.writeText(guidText);

        bsl::string monitorableObjectName;
        {
            bsl::stringstream ss;
            ss << "socket-";
            ss.write(guidText, ntsa::Guid::SIZE_TEXT);
            monitorableObjectName = ss.str();
        }

        d_metrics_sp.createInplace(d_allocator_p,
                                   "socket",
                                   monitorableObjectName,
                                   metrics,
                                   d_allocator_p);

        ntcm::MonitorableUtil::registerMonitorable(d_metrics_sp);
    }
    else {
        d_metrics_sp = metrics;
    }
}

ListenerSocket::~ListenerSocket()
{
    if (!d_options.metrics().isNull() && d_options.metrics().value()) {
        if (d_metrics_sp) {
            ntcm::MonitorableUtil::deregisterMonitorable(d_metrics_sp);
        }
    }
}

ntsa::Error ListenerSocket::open()
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    return this->privateOpen(self);
}

ntsa::Error ListenerSocket::open(ntsa::Transport::Value transport)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    return this->privateOpen(self, transport);
}

ntsa::Error ListenerSocket::open(ntsa::Transport::Value transport,
                                 ntsa::Handle           handle)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    return this->privateOpen(self, transport, handle);
}

ntsa::Error ListenerSocket::open(
    ntsa::Transport::Value                       transport,
    const bsl::shared_ptr<ntsi::ListenerSocket>& listenerSocket)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    return this->privateOpen(self, transport, listenerSocket);
}

ntsa::Error ListenerSocket::bind(const ntsa::Endpoint&     endpoint,
                                 const ntca::BindOptions&  options,
                                 const ntci::BindFunction& callback)
{
    return this->bind(endpoint,
                      options,
                      this->createBindCallback(callback, d_allocator_p));
}

ntsa::Error ListenerSocket::bind(const ntsa::Endpoint&     endpoint,
                                 const ntca::BindOptions&  options,
                                 const ntci::BindCallback& callback)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    ntsa::Error error;

    error = this->privateOpen(self, endpoint);
    if (error) {
        return error;
    }

    error = d_socket_sp->bind(endpoint, d_options.reuseAddress());
    if (error) {
        return error;
    }

    error = d_socket_sp->sourceEndpoint(&d_sourceEndpoint);
    if (error) {
        return error;
    }

    if (callback) {
        ntca::BindContext bindContext;
        bindContext.setEndpoint(d_sourceEndpoint);

        ntca::BindEvent bindEvent;
        bindEvent.setType(ntca::BindEventType::e_COMPLETE);
        bindEvent.setContext(bindContext);

        const bool defer = !options.recurse();

        callback.dispatch(self,
                          bindEvent,
                          ntci::Strand::unknown(),
                          self,
                          defer,
                          &d_mutex);
    }

    return ntsa::Error();
}

ntsa::Error ListenerSocket::bind(const bsl::string&        name,
                                 const ntca::BindOptions&  options,
                                 const ntci::BindFunction& callback)
{
    return this->bind(name,
                      options,
                      this->createBindCallback(callback, d_allocator_p));
}

ntsa::Error ListenerSocket::bind(const bsl::string&        name,
                                 const ntca::BindOptions&  options,
                                 const ntci::BindCallback& callback)
{
    if (d_options.transport() == ntsa::Transport::e_LOCAL_STREAM ||
        bdls::PathUtil::isAbsolute(name))
    {
        ntsa::LocalName localName;
        localName.setValue(name);
        return this->bind(ntsa::Endpoint(localName), options, callback);
    }

    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    ntsa::Error error;

    ntcs::ObserverRef<ntci::Resolver> resolverRef(&d_resolver);
    if (!resolverRef) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntca::GetEndpointOptions getEndpointOptions;
    ntcs::Compat::convert(&getEndpointOptions, options);

    ntci::GetEndpointCallback getEndpointCallback =
        resolverRef->createGetEndpointCallback(
            NTCCFG_BIND(&ListenerSocket::processSourceEndpointResolution,
                        self,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        options,
                        callback),
            d_allocator_p);

    return resolverRef->getEndpoint(name,
                                    getEndpointOptions,
                                    getEndpointCallback);
}

ntsa::Error ListenerSocket::listen()
{
    bsl::size_t backlog = 0;
    if (!d_options.backlog().isNull()) {
        backlog = d_options.backlog().value();
    }

    return this->listen(backlog);
}

ntsa::Error ListenerSocket::listen(bsl::size_t backlog)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    ntsa::Error error;

    error = this->privateOpen(self);
    if (error) {
        return error;
    }

    error = d_socket_sp->listen(backlog);
    if (error) {
        return error;
    }

    error = d_socket_sp->sourceEndpoint(&d_sourceEndpoint);
    if (error) {
        return error;
    }

    if (!this->getProactorContext()) {
        ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
        if (!proactorRef) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        proactorRef->attachSocket(self);

        ntcs::Dispatch::announceEstablished(d_manager_sp,
                                            self,
                                            d_managerStrand_sp,
                                            ntci::Strand::unknown(),
                                            self,
                                            true,
                                            &d_mutex);
    }

    this->privateRelaxFlowControl(self,
                                  ntca::FlowControlType::e_RECEIVE,
                                  true,
                                  true);

    return ntsa::Error();
}

ntsa::Error ListenerSocket::accept(
    ntca::AcceptContext*                 context,
    bsl::shared_ptr<ntci::StreamSocket>* streamSocket,
    const ntca::AcceptOptions&           options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(options);

    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    ntsa::Error error;

    if (NTCCFG_UNLIKELY(!d_acceptQueue.hasEntry() &&
                        !d_shutdownState.canReceive()))
    {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    if (NTCCFG_LIKELY(d_acceptQueue.hasEntry())) {
        bool acceptQueueHighWatermarkViolatedBefore =
            d_acceptQueue.isHighWatermarkViolated();

        ntcq::AcceptQueueEntry& entry = d_acceptQueue.frontEntry();

        *streamSocket = entry.streamSocket();

        NTCS_METRICS_UPDATE_ACCEPT_QUEUE_DELAY(entry.delay());

        d_acceptQueue.popEntry();

        NTCP_LISTENERSOCKET_LOG_ACCEPT_QUEUE_DRAINED(d_acceptQueue.size());

        NTCS_METRICS_UPDATE_ACCEPT_QUEUE_SIZE(d_acceptQueue.size());

        bool acceptQueueHighWatermarkViolatedAfter =
            d_acceptQueue.isHighWatermarkViolated();

        if (acceptQueueHighWatermarkViolatedBefore &&
            !acceptQueueHighWatermarkViolatedAfter)
        {
            this->privateRelaxFlowControl(self,
                                          ntca::FlowControlType::e_RECEIVE,
                                          true,
                                          false);
        }

        error = ntsa::Error::e_OK;
    }
    else {
        error = ntsa::Error::e_WOULD_BLOCK;
    }

    BSLS_ASSERT(error == ntsa::Error::e_OK ||
                error == ntsa::Error::e_WOULD_BLOCK);

    if (error == ntsa::Error::e_WOULD_BLOCK) {
        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      true,
                                      false);
    }

    return error;
}

ntsa::Error ListenerSocket::accept(const ntca::AcceptOptions&  options,
                                   const ntci::AcceptFunction& callback)
{
    return this->accept(options,
                        this->createAcceptCallback(callback, d_allocator_p));
}

ntsa::Error ListenerSocket::accept(const ntca::AcceptOptions&  options,
                                   const ntci::AcceptCallback& callback)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    ntsa::Error error;

    if (NTCCFG_UNLIKELY(!d_acceptQueue.hasEntry() &&
                        !d_shutdownState.canReceive()))
    {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry> callbackEntry =
        d_acceptQueue.createCallbackEntry();
    callbackEntry->assign(callback, options);

    if (NTCCFG_LIKELY(!d_acceptQueue.hasCallbackEntry() &&
                      d_acceptQueue.hasEntry()))
    {
        bool acceptQueueHighWatermarkViolatedBefore =
            d_acceptQueue.isHighWatermarkViolated();

        ntcq::AcceptQueueEntry& entry = d_acceptQueue.frontEntry();

        bsl::shared_ptr<ntci::StreamSocket> streamSocket =
            entry.streamSocket();

        NTCS_METRICS_UPDATE_ACCEPT_QUEUE_DELAY(entry.delay());

        d_acceptQueue.popEntry();

        NTCP_LISTENERSOCKET_LOG_ACCEPT_QUEUE_DRAINED(d_acceptQueue.size());

        NTCS_METRICS_UPDATE_ACCEPT_QUEUE_SIZE(d_acceptQueue.size());

        ntca::AcceptContext acceptContext;

        ntca::AcceptEvent acceptEvent;
        acceptEvent.setType(ntca::AcceptEventType::e_COMPLETE);
        acceptEvent.setContext(acceptContext);

        const bool defer = !options.recurse();

        ntcq::AcceptCallbackQueueEntry::dispatch(callbackEntry,
                                                 self,
                                                 streamSocket,
                                                 acceptEvent,
                                                 ntci::Strand::unknown(),
                                                 self,
                                                 defer,
                                                 &d_mutex);

        bool acceptQueueHighWatermarkViolatedAfter =
            d_acceptQueue.isHighWatermarkViolated();

        if (acceptQueueHighWatermarkViolatedBefore &&
            !acceptQueueHighWatermarkViolatedAfter)
        {
            this->privateRelaxFlowControl(self,
                                          ntca::FlowControlType::e_RECEIVE,
                                          true,
                                          false);
        }

        error = ntsa::Error::e_OK;
    }
    else {
        if (!options.deadline().isNull()) {
            ntca::TimerOptions timerOptions;
            timerOptions.setOneShot(true);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
            timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
            timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

            ntci::TimerCallback timerCallback = this->createTimerCallback(
                bdlf::BindUtil::bind(
                    &ListenerSocket::processAcceptDeadlineTimer,
                    self,
                    bdlf::PlaceHolders::_1,
                    bdlf::PlaceHolders::_2,
                    callbackEntry),
                d_allocator_p);

            bsl::shared_ptr<ntci::Timer> timer =
                this->createTimer(timerOptions, timerCallback, d_allocator_p);

            callbackEntry->setTimer(timer);

            timer->schedule(options.deadline().value());
        }

        d_acceptQueue.pushCallbackEntry(callbackEntry);
        error = ntsa::Error::e_WOULD_BLOCK;
    }

    BSLS_ASSERT(error == ntsa::Error::e_OK ||
                error == ntsa::Error::e_WOULD_BLOCK);

    if (error == ntsa::Error::e_WOULD_BLOCK) {
        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      true,
                                      false);
        error = ntsa::Error::e_OK;
    }

    return error;
}

ntsa::Error ListenerSocket::registerResolver(
    const bsl::shared_ptr<ntci::Resolver>& resolver)
{
    LockGuard lock(&d_mutex);
    d_resolver = resolver;
    return ntsa::Error();
}

ntsa::Error ListenerSocket::deregisterResolver()
{
    LockGuard lock(&d_mutex);
    d_resolver.reset();
    return ntsa::Error();
}

ntsa::Error ListenerSocket::registerManager(
    const bsl::shared_ptr<ntci::ListenerSocketManager>& manager)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (manager) {
        d_manager_sp       = manager;
        d_managerStrand_sp = d_manager_sp->strand();

        if (!d_managerStrand_sp) {
            d_managerStrand_sp = d_proactorStrand_sp;
        }
    }
    else {
        d_manager_sp.reset();
        d_managerStrand_sp.reset();
    }

    return ntsa::Error();
}

ntsa::Error ListenerSocket::deregisterManager()
{
    LockGuard lock(&d_mutex);

    d_manager_sp.reset();
    d_managerStrand_sp.reset();

    return ntsa::Error();
}

ntsa::Error ListenerSocket::registerSession(
    const bsl::shared_ptr<ntci::ListenerSocketSession>& session)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (session) {
        d_session_sp       = session;
        d_sessionStrand_sp = d_session_sp->strand();

        if (!d_sessionStrand_sp) {
            d_sessionStrand_sp = d_proactorStrand_sp;
        }
    }
    else {
        d_session_sp.reset();
        d_sessionStrand_sp.reset();
    }

    return ntsa::Error();
}

ntsa::Error ListenerSocket::registerSessionCallback(
    const ntci::ListenerSocket::SessionCallback& callback)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (callback) {
        bsl::shared_ptr<ntcu::ListenerSocketSession> session;
        session.createInplace(d_allocator_p,
                              callback,
                              d_proactorStrand_sp,
                              d_allocator_p);

        d_session_sp       = session;
        d_sessionStrand_sp = d_session_sp->strand();

        if (!d_sessionStrand_sp) {
            d_sessionStrand_sp = d_proactorStrand_sp;
        }
    }
    else {
        d_session_sp.reset();
        d_sessionStrand_sp.reset();
    }

    return ntsa::Error();
}

ntsa::Error ListenerSocket::registerSessionCallback(
    const ntci::ListenerSocket::SessionCallback& callback,
    const bsl::shared_ptr<ntci::Strand>&         strand)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (callback) {
        bsl::shared_ptr<ntcu::ListenerSocketSession> session;
        session.createInplace(d_allocator_p, callback, strand, d_allocator_p);

        d_session_sp       = session;
        d_sessionStrand_sp = d_session_sp->strand();

        if (!d_sessionStrand_sp) {
            d_sessionStrand_sp = d_proactorStrand_sp;
        }
    }
    else {
        d_session_sp.reset();
        d_sessionStrand_sp.reset();
    }

    return ntsa::Error();
}

ntsa::Error ListenerSocket::deregisterSession()
{
    LockGuard lock(&d_mutex);

    d_session_sp.reset();
    d_sessionStrand_sp.reset();

    return ntsa::Error();
}

ntsa::Error ListenerSocket::setAcceptRateLimiter(
    const bsl::shared_ptr<ntci::RateLimiter>& rateLimiter)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    d_acceptRateLimiter_sp = rateLimiter;

    if (!d_acceptRateLimiter_sp) {
        if (d_acceptRateTimer_sp) {
            d_acceptRateTimer_sp->close();
            d_acceptRateTimer_sp.reset();
        }

        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      true,
                                      true);
    }

    return ntsa::Error();
}

ntsa::Error ListenerSocket::setAcceptQueueLowWatermark(
    bsl::size_t lowWatermark)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    d_acceptQueue.setLowWatermark(lowWatermark);

    if (!d_acceptQueue.isLowWatermarkSatisfied()) {
        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      true,
                                      false);
    }
    else {
        if (d_session_sp) {
            ntca::AcceptQueueEvent event;
            event.setType(ntca::AcceptQueueEventType::e_LOW_WATERMARK);
            event.setContext(d_acceptQueue.context());

            ntcs::Dispatch::announceAcceptQueueLowWatermark(
                d_session_sp,
                self,
                event,
                d_sessionStrand_sp,
                ntci::Strand::unknown(),
                self,
                true,
                &d_mutex);
        }
    }

    return ntsa::Error();
}

ntsa::Error ListenerSocket::setAcceptQueueHighWatermark(
    bsl::size_t highWatermark)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    d_acceptQueue.setHighWatermark(highWatermark);

    if (d_acceptQueue.isHighWatermarkViolated()) {
        this->privateApplyFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      ntca::FlowControlMode::e_IMMEDIATE,
                                      true,
                                      false);
    }

    return ntsa::Error();
}

ntsa::Error ListenerSocket::setAcceptQueueWatermarks(bsl::size_t lowWatermark,
                                                     bsl::size_t highWatermark)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    d_acceptQueue.setLowWatermark(lowWatermark);
    d_acceptQueue.setHighWatermark(highWatermark);

    if (!d_acceptQueue.isLowWatermarkSatisfied()) {
        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      true,
                                      false);
    }

    if (d_acceptQueue.isHighWatermarkViolated()) {
        this->privateApplyFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      ntca::FlowControlMode::e_IMMEDIATE,
                                      true,
                                      false);
    }

    return ntsa::Error();
}

ntsa::Error ListenerSocket::relaxFlowControl(
    ntca::FlowControlType::Value direction)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    return this->privateRelaxFlowControl(self, direction, true, true);
}

ntsa::Error ListenerSocket::applyFlowControl(
    ntca::FlowControlType::Value direction,
    ntca::FlowControlMode::Value mode)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    if (direction == ntca::FlowControlType::e_RECEIVE ||
        direction == ntca::FlowControlType::e_BOTH)
    {
        if (d_acceptRateTimer_sp) {
            d_acceptRateTimer_sp->close();
            d_acceptRateTimer_sp.reset();
        }
    }

    return this->privateApplyFlowControl(self, direction, mode, true, true);
}

ntsa::Error ListenerSocket::cancel(const ntca::BindToken& token)
{
    NTCCFG_WARNING_UNUSED(token);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error ListenerSocket::cancel(const ntca::AcceptToken& token)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry> callbackEntry;
    ntsa::Error                                     error =
        d_acceptQueue.removeCallbackEntry(&callbackEntry, token);
    if (!error) {
        ntca::AcceptContext acceptContext;
        acceptContext.setError(ntsa::Error(ntsa::Error::e_CANCELLED));

        ntca::AcceptEvent acceptEvent;
        acceptEvent.setType(ntca::AcceptEventType::e_ERROR);
        acceptEvent.setContext(acceptContext);

        ntcq::AcceptCallbackQueueEntry::dispatch(
            callbackEntry,
            self,
            bsl::shared_ptr<ntci::StreamSocket>(),
            acceptEvent,
            d_proactorStrand_sp,
            self,
            true,
            &d_mutex);

        return ntsa::Error();
    }

    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error ListenerSocket::shutdown()
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    this->privateShutdown(self,
                          ntsa::ShutdownType::e_BOTH,
                          ntsa::ShutdownMode::e_IMMEDIATE,
                          true);
    return ntsa::Error();
}

void ListenerSocket::close()
{
    this->close(ntci::CloseCallback());
}

void ListenerSocket::close(const ntci::CloseFunction& callback)
{
    this->close(this->createCloseCallback(callback, d_allocator_p));
}

void ListenerSocket::close(const ntci::CloseCallback& callback)
{
    bsl::shared_ptr<ListenerSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);

    if (d_detachState.get() == ntcs::DetachState::e_DETACH_INITIATED) {
        d_deferredCalls.push_back(NTCCFG_BIND(
            static_cast<void (ListenerSocket::*)(
                const ntci::CloseCallback& callback)>(&ListenerSocket::close),
            self,
            callback));
        return;
    }

    if (NTCCFG_UNLIKELY(d_acceptQueue.hasEntry())) {
        ntca::AcceptQueueEvent event;
        event.setType(ntca::AcceptQueueEventType::e_DISCARDED);
        event.setContext(d_acceptQueue.context());

        while (NTCCFG_LIKELY(d_acceptQueue.hasEntry())) {
            bsl::shared_ptr<ntci::StreamSocket> streamSocket;
            {
                ntcq::AcceptQueueEntry& entry = d_acceptQueue.frontEntry();
                streamSocket                  = entry.streamSocket();
                d_acceptQueue.popEntry();
            }

            streamSocket->close();
        }

        if (d_session_sp) {
            ntcs::Dispatch::announceAcceptQueueDiscarded(d_session_sp,
                                                         self,
                                                         event,
                                                         d_sessionStrand_sp,
                                                         d_proactorStrand_sp,
                                                         self,
                                                         true,
                                                         &d_mutex);
        }
    }

    BSLS_ASSERT(!d_closeCallback);
    d_closeCallback = callback;

    this->privateShutdown(self,
                          ntsa::ShutdownType::e_BOTH,
                          ntsa::ShutdownMode::e_IMMEDIATE,
                          true);
}

void ListenerSocket::execute(const Functor& functor)
{
    if (d_proactorStrand_sp) {
        d_proactorStrand_sp->execute(functor);
    }
    else {
        ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
        if (proactorRef) {
            proactorRef->execute(functor);
        }
        else {
            ntcs::Async::execute(functor);
        }
    }
}

void ListenerSocket::moveAndExecute(FunctorSequence* functorSequence,
                                    const Functor&   functor)
{
    if (d_proactorStrand_sp) {
        d_proactorStrand_sp->moveAndExecute(functorSequence, functor);
    }
    else {
        ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
        if (proactorRef) {
            proactorRef->moveAndExecute(functorSequence, functor);
        }
        else {
            ntcs::Async::moveAndExecute(functorSequence, functor);
        }
    }
}

// MANIPULATROS (ntci::StrandFactory)
bsl::shared_ptr<ntci::Strand> ListenerSocket::createStrand(
    bslma::Allocator* basicAllocator)
{
    ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
    if (proactorRef) {
        return proactorRef->createStrand(basicAllocator);
    }
    else {
        return ntcs::Async::createStrand(basicAllocator);
    }
}

bsl::shared_ptr<ntci::Timer> ListenerSocket::createTimer(
    const ntca::TimerOptions&                  options,
    const bsl::shared_ptr<ntci::TimerSession>& session,
    bslma::Allocator*                          basicAllocator)
{
    ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
    if (proactorRef) {
        return proactorRef->createTimer(options, session, basicAllocator);
    }
    else {
        return ntcs::Async::createTimer(options, session, basicAllocator);
    }
}

bsl::shared_ptr<ntci::Timer> ListenerSocket::createTimer(
    const ntca::TimerOptions&  options,
    const ntci::TimerCallback& callback,
    bslma::Allocator*          basicAllocator)
{
    ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
    if (proactorRef) {
        return proactorRef->createTimer(options, callback, basicAllocator);
    }
    else {
        return ntcs::Async::createTimer(options, callback, basicAllocator);
    }
}

bsl::shared_ptr<ntsa::Data> ListenerSocket::createIncomingData()
{
    return d_dataPool_sp->createIncomingData();
}

bsl::shared_ptr<ntsa::Data> ListenerSocket::createOutgoingData()
{
    return d_dataPool_sp->createOutgoingData();
}

bsl::shared_ptr<bdlbb::Blob> ListenerSocket::createIncomingBlob()
{
    return d_dataPool_sp->createIncomingBlob();
}

bsl::shared_ptr<bdlbb::Blob> ListenerSocket::createOutgoingBlob()
{
    return d_dataPool_sp->createOutgoingBlob();
}

void ListenerSocket::createIncomingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_incomingBufferFactory_sp->allocate(blobBuffer);
}

void ListenerSocket::createOutgoingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_outgoingBufferFactory_sp->allocate(blobBuffer);
}

ntsa::Handle ListenerSocket::handle() const
{
    return d_publicHandle;
}

ntsa::Transport::Value ListenerSocket::transport() const
{
    return d_transport;
}

ntsa::Endpoint ListenerSocket::sourceEndpoint() const
{
    LockGuard lock(&d_mutex);
    return d_sourceEndpoint;
}

const bsl::shared_ptr<ntci::Strand>& ListenerSocket::strand() const
{
    return d_proactorStrand_sp;
}

bslmt::ThreadUtil::Handle ListenerSocket::threadHandle() const
{
    ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
    if (proactorRef) {
        return proactorRef->threadHandle();
    }
    else {
        return bslmt::ThreadUtil::invalidHandle();
    }
}

bsl::size_t ListenerSocket::threadIndex() const
{
    ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
    if (proactorRef) {
        return proactorRef->threadIndex();
    }
    else {
        return 0;
    }
}

bsl::size_t ListenerSocket::acceptQueueSize() const
{
    LockGuard lock(&d_mutex);
    return d_acceptQueue.size();
}

bsl::size_t ListenerSocket::acceptQueueLowWatermark() const
{
    LockGuard lock(&d_mutex);
    return d_acceptQueue.lowWatermark();
}

bsl::size_t ListenerSocket::acceptQueueHighWatermark() const
{
    LockGuard lock(&d_mutex);
    return d_acceptQueue.highWatermark();
}

bsls::TimeInterval ListenerSocket::currentTime() const
{
    return bdlt::CurrentTime::now();
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& ListenerSocket::
    incomingBlobBufferFactory() const
{
    return d_incomingBufferFactory_sp;
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& ListenerSocket::
    outgoingBlobBufferFactory() const
{
    return d_outgoingBufferFactory_sp;
}

}  // close package namespace
}  // close enterprise namespace
