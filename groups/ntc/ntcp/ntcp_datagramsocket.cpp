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

#include <ntcp_datagramsocket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcp_datagramsocket_cpp, "$Id$ $CSID$")

#include <ntccfg_limits.h>
#include <ntci_log.h>
#include <ntci_monitorable.h>
#include <ntcs_monitorable.h>
#include <ntcs_async.h>
#include <ntcs_blobbufferutil.h>
#include <ntcs_blobutil.h>
#include <ntcs_compat.h>
#include <ntcs_dispatch.h>
#include <ntcs_plugin.h>
#include <ntcu_datagramsocketsession.h>
#include <ntcu_datagramsocketutil.h>
#include <ntsa_receivecontext.h>
#include <ntsa_receiveoptions.h>
#include <ntsa_sendcontext.h>
#include <ntsa_sendoptions.h>
#include <ntsf_system.h>
#include <bdlbb_blobutil.h>
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
#define NTCP_DATAGRAMSOCKET_OBSERVE_BY_WEAK_PTR 0

#define NTCP_DATAGRAMSOCKET_LOG_RECEIVE_BUFFER_THROTTLE_APPLIED(timeToSubmit) \
    NTCI_LOG_TRACE("Datagram socket receive buffer throttle applied for %d "  \
                   "milliseconds",                                            \
                   (int)((timeToSubmit).totalMilliseconds()))

#define NTCP_DATAGRAMSOCKET_LOG_RECEIVE_BUFFER_THROTTLE_RELAXED()             \
    NTCI_LOG_TRACE("Datagram socket receive buffer throttle relaxed")

#define NTCP_DATAGRAMSOCKET_LOG_RECEIVE_BUFFER_UNDERFLOW()                    \
    NTCI_LOG_TRACE("Datagram socket "                                         \
                   "has emptied the socket receive buffer")

#define NTCP_DATAGRAMSOCKET_LOG_RECEIVE_RESULT(context)                       \
    NTCI_LOG_TRACE("Datagram socket "                                         \
                   "has copied %zu bytes out of %zu bytes attempted from "    \
                   "the socket receive buffer",                               \
                   (context).bytesReceived(),                                 \
                   (context).bytesReceivable())

#define NTCP_DATAGRAMSOCKET_LOG_RECEIVE_FAILURE(error)                        \
    NTCI_LOG_TRACE("Datagram socket "                                         \
                   "failed to receive: %s",                                   \
                   (error).text().c_str())

#define NTCP_DATAGRAMSOCKET_LOG_READ_QUEUE_FILLED(size)                       \
    NTCI_LOG_TRACE("Datagram socket "                                         \
                   "has filled the read queue up to %zu bytes",               \
                   size)

#define NTCP_DATAGRAMSOCKET_LOG_READ_QUEUE_DRAINED(size)                      \
    NTCI_LOG_TRACE("Datagram socket "                                         \
                   "has drained the read queue down to %zu bytes",            \
                   size)

#define NTCP_DATAGRAMSOCKET_LOG_READ_QUEUE_LOW_WATERMARK(lowWatermark, size)  \
    NTCI_LOG_TRACE("Datagram socket "                                         \
                   "has satisfied the read queue low watermark of %zu "       \
                   "bytes with a read queue of %zu bytes",                    \
                   lowWatermark,                                              \
                   size)

#define NTCP_DATAGRAMSOCKET_LOG_READ_QUEUE_HIGH_WATERMARK(highWatermark,      \
                                                          size)               \
    NTCI_LOG_TRACE("Datagram socket "                                         \
                   "has breached the read queue high watermark of %zu "       \
                   "bytes with a read queue of %zu bytes",                    \
                   highWatermark,                                             \
                   size)

#define NTCP_DATAGRAMSOCKET_LOG_SHUTDOWN_RECEIVE()                            \
    NTCI_LOG_TRACE("Datagram socket "                                         \
                   "is shutting down reception")

#define NTCP_DATAGRAMSOCKET_LOG_SEND_BUFFER_THROTTLE_APPLIED(timeToSubmit)    \
    NTCI_LOG_TRACE("Datagram socket send buffer throttle applied for %d "     \
                   "milliseconds",                                            \
                   (int)((timeToSubmit).totalMilliseconds()))

#define NTCP_DATAGRAMSOCKET_LOG_SEND_BUFFER_THROTTLE_RELAXED()                \
    NTCI_LOG_TRACE("Datagram socket send buffer throttle relaxed")

#define NTCP_DATAGRAMSOCKET_LOG_SEND_BUFFER_OVERFLOW()                        \
    NTCI_LOG_TRACE("Datagram socket "                                         \
                   "has saturated the socket send buffer")

#define NTCP_DATAGRAMSOCKET_LOG_SEND_RESULT(context)                          \
    NTCI_LOG_TRACE("Datagram socket "                                         \
                   "has copied %zu bytes out of %zu bytes attempted to "      \
                   "the socket send buffer",                                  \
                   (context).bytesSent(),                                     \
                   (context).bytesSendable())

#define NTCP_DATAGRAMSOCKET_LOG_SEND_FAILURE(error)                           \
    NTCI_LOG_TRACE("Datagram socket "                                         \
                   "failed to send: %s",                                      \
                   (error).text().c_str())

#define NTCP_DATAGRAMSOCKET_LOG_WRITE_QUEUE_FILLED(size)                      \
    NTCI_LOG_TRACE("Datagram socket "                                         \
                   "has filled the write queue up to %zu bytes",              \
                   size)

#define NTCP_DATAGRAMSOCKET_LOG_WRITE_QUEUE_DRAINED(size)                     \
    NTCI_LOG_TRACE("Datagram socket "                                         \
                   "has drained the write queue down to %zu bytes",           \
                   size)

#define NTCP_DATAGRAMSOCKET_LOG_WRITE_QUEUE_LOW_WATERMARK(lowWatermark, size) \
    NTCI_LOG_TRACE("Datagram socket "                                         \
                   "has satisfied the write queue low watermark of %zu "      \
                   "bytes with a write queue of %zu bytes",                   \
                   lowWatermark,                                              \
                   size)

#define NTCP_DATAGRAMSOCKET_LOG_WRITE_QUEUE_HIGH_WATERMARK(highWatermark,     \
                                                           size)              \
    NTCI_LOG_TRACE("Datagram socket "                                         \
                   "has breached the write queue high watermark of %d "       \
                   "bytes with a write queue of %d bytes",                    \
                   highWatermark,                                             \
                   size)

#define NTCP_DATAGRAMSOCKET_LOG_SHUTDOWN_SEND()                               \
    NTCI_LOG_TRACE("Datagram socket "                                         \
                   "is shutting down transmission")

// Some versions of GCC erroneously warn ntcs::ObserverRef::d_shared may be
// uninitialized.
#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

namespace BloombergLP {
namespace ntcp {

void DatagramSocket::processSocketReceived(const ntsa::Error&          error,
                                           const ntsa::ReceiveContext& context)
{
    NTCCFG_WARNING_UNUSED(context);

    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (NTCCFG_UNLIKELY(d_detachState.mode() ==
                        ntcs::DetachMode::e_INITIATED))
    {
        return;
    }

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    d_receivePending = false;

    if (error) {
        if (error != ntsa::Error::e_CANCELLED) {
            NTCP_DATAGRAMSOCKET_LOG_RECEIVE_FAILURE(error);
            this->privateFailReceive(self, error);
            return;
        }
    }
    else {
        NTCP_DATAGRAMSOCKET_LOG_RECEIVE_RESULT(context);

        BSLS_ASSERT(
            NTCCFG_WARNING_PROMOTE(bsl::size_t, d_receiveBlob_sp->length()) ==
            context.bytesReceived());

        bsl::shared_ptr<bdlbb::Blob> data;
        data.swap(d_receiveBlob_sp);

        if (!context.endpoint().isNull()) {
            this->privateCompleteReceive(self,
                                         context.endpoint().value(),
                                         data);
        }
        else {
            this->privateCompleteReceive(self, d_systemRemoteEndpoint, data);
        }
    }

    this->privateInitiateReceive(self);
}

void DatagramSocket::processSocketSent(const ntsa::Error&       error,
                                       const ntsa::SendContext& context)
{
    NTCCFG_WARNING_UNUSED(context);

    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (NTCCFG_UNLIKELY(d_detachState.mode() ==
                        ntcs::DetachMode::e_INITIATED))
    {
        return;
    }

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    d_sendPending = false;

    if (error) {
        if (error != ntsa::Error::e_CANCELLED) {
            NTCP_DATAGRAMSOCKET_LOG_SEND_FAILURE(error);
            this->privateFailSend(self, error);
            return;
        }
    }
    else {
        NTCP_DATAGRAMSOCKET_LOG_SEND_RESULT(context);
        this->privateCompleteSend(self);
    }

    this->privateInitiateSend(self);
}

void DatagramSocket::processSocketError(const ntsa::Error& error)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (NTCCFG_UNLIKELY(d_detachState.mode() ==
                        ntcs::DetachMode::e_INITIATED))
    {
        return;
    }

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    this->privateFail(self, error);
}

void DatagramSocket::processSocketDetached()
{
    NTCCFG_OBJECT_GUARD(&d_object);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    BSLS_ASSERT(d_detachState.mode() == ntcs::DetachMode::e_INITIATED);
    d_detachState.setMode(ntcs::DetachMode::e_IDLE);
    BSLS_ASSERT(d_deferredCall);
    if (NTCCFG_LIKELY(d_deferredCall)) {
        NTCCFG_FUNCTION() deferredCall;
        deferredCall.swap(d_deferredCall);
        deferredCall();
    }
}

void DatagramSocket::processSendRateTimer(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_WARNING_UNUSED(timer);

    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        NTCP_DATAGRAMSOCKET_LOG_SEND_BUFFER_THROTTLE_RELAXED();

        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_SEND,
                                      false,
                                      true);

        if (d_session_sp) {
            ntca::WriteQueueEvent event;
            event.setType(ntca::WriteQueueEventType::e_RATE_LIMIT_RELAXED);
            event.setContext(d_sendQueue.context());

            ntcs::Dispatch::announceWriteQueueRateLimitRelaxed(
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

void DatagramSocket::processSendDeadlineTimer(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event,
    bsl::uint64_t                       entryId)
{
    NTCCFG_WARNING_UNUSED(timer);

    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_systemRemoteEndpoint);

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        ntci::SendCallback callback;
        ntca::SendContext  context;

        bool becameEmpty = d_sendQueue.removeEntryId(&callback, 
                                                     &context,
                                                     entryId);
        if (becameEmpty) {
            this->privateApplyFlowControl(self,
                                          ntca::FlowControlType::e_SEND,
                                          ntca::FlowControlMode::e_IMMEDIATE,
                                          false,
                                          false);
        }

        if (callback) {
            context.setError(ntsa::Error(ntsa::Error::e_WOULD_BLOCK));

            ntca::SendEvent sendEvent;
            sendEvent.setType(ntca::SendEventType::e_ERROR);
            sendEvent.setContext(context);

            callback.dispatch(self,
                              sendEvent,
                              d_proactorStrand_sp,
                              self,
                              false,
                              &d_mutex);
        }
    }
}

void DatagramSocket::processReceiveRateTimer(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_WARNING_UNUSED(timer);

    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        NTCP_DATAGRAMSOCKET_LOG_RECEIVE_BUFFER_THROTTLE_RELAXED();

        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      false,
                                      true);

        if (d_session_sp) {
            ntca::ReadQueueEvent event;
            event.setType(ntca::ReadQueueEventType::e_RATE_LIMIT_RELAXED);
            event.setContext(d_receiveQueue.context());

            ntcs::Dispatch::announceReadQueueRateLimitRelaxed(
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

void DatagramSocket::processReceiveDeadlineTimer(
    const bsl::shared_ptr<ntci::Timer>&                     timer,
    const ntca::TimerEvent&                                 event,
    const bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>& entry)
{
    NTCCFG_WARNING_UNUSED(timer);

    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        ntsa::Error error = d_receiveQueue.removeCallbackEntry(entry);
        if (!error) {
            ntca::ReceiveContext receiveContext;
            receiveContext.setError(ntsa::Error(ntsa::Error::e_WOULD_BLOCK));
            receiveContext.setTransport(d_transport);

            ntca::ReceiveEvent receiveEvent;
            receiveEvent.setType(ntca::ReceiveEventType::e_ERROR);
            receiveEvent.setContext(receiveContext);

            ntcq::ReceiveCallbackQueueEntry::dispatch(
                entry,
                self,
                bsl::shared_ptr<bdlbb::Blob>(),
                receiveEvent,
                d_proactorStrand_sp,
                self,
                false,
                &d_mutex);
        }
    }
}

void DatagramSocket::privateInitiateReceive(
    const bsl::shared_ptr<DatagramSocket>& self)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (d_receivePending) {
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
        this->privateFailReceive(self, ntsa::Error(ntsa::Error::e_INVALID));
        return;
    }

    error = this->privateThrottleReceiveBuffer(self);
    if (error) {
        return;
    }

    this->privateAllocateReceiveBlob();

    BSLS_ASSERT(d_receiveBlob_sp->length() == 0);
    BSLS_ASSERT(
        NTCCFG_WARNING_PROMOTE(bsl::size_t, d_receiveBlob_sp->totalSize()) ==
        d_maxDatagramSize);

    error = proactorRef->receive(self,
                                 d_receiveBlob_sp.get(),
                                 ntsa::ReceiveOptions());

    if (error) {
        NTCP_DATAGRAMSOCKET_LOG_RECEIVE_FAILURE(error);
        this->privateFailReceive(self, error);
        return;
    }

    d_receivePending = true;
}

void DatagramSocket::privateCompleteReceive(
    const bsl::shared_ptr<DatagramSocket>& self,
    const ntsa::Endpoint&                  endpoint,
    const bsl::shared_ptr<bdlbb::Blob>&    data)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (NTCCFG_UNLIKELY(d_receiveRateLimiter_sp)) {
        d_receiveRateLimiter_sp->submit(data->length());
    }

    {
        ntcq::ReceiveQueueEntry entry;
        entry.setTimestamp(bsls::TimeUtil::getTimer());

        if (!endpoint.isUndefined()) {
            entry.setEndpoint(endpoint);
        }
        else {
            entry.setEndpoint(d_systemRemoteEndpoint);
        }

        if (!d_receiveInflater_sp) {
            entry.setData(data);
            entry.setLength(data->length());
        }
        else {
            bsl::shared_ptr<bdlbb::Blob> inflatedData = 
                d_dataPool_sp->createIncomingBlob();

            ntca::InflateOptions inflateOptions;
            ntca::InflateContext inflateContext;

            error = d_receiveInflater_sp->inflate(&inflateContext,
                                                  inflatedData.get(),
                                                  *data,
                                                  inflateOptions);
            if (NTCCFG_UNLIKELY(error)) {
                this->privateFailReceive(self, error);
                return;
            }

            entry.setData(inflatedData);
            entry.setLength(inflatedData->length());
        }

        d_receiveQueue.pushEntry(entry);

        d_receiveBlob_sp.reset();
    }

    NTCP_DATAGRAMSOCKET_LOG_READ_QUEUE_FILLED(d_receiveQueue.size());

    NTCS_METRICS_UPDATE_READ_QUEUE_SIZE(d_receiveQueue.size());

    while (true) {
        bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> callbackEntry;
        error = d_receiveQueue.popCallbackEntry(&callbackEntry);
        if (error) {
            break;
        }

        BSLS_ASSERT(d_receiveQueue.hasEntry());

        ntcq::ReceiveQueueEntry& entry = d_receiveQueue.frontEntry();

        bdlb::NullableValue<ntsa::Endpoint> queuedEndpoint = entry.endpoint();
        bsl::shared_ptr<bdlbb::Blob>        queuedData     = entry.data();

        NTCS_METRICS_UPDATE_READ_QUEUE_DELAY(entry.delay());

        d_receiveQueue.popEntry();

        NTCP_DATAGRAMSOCKET_LOG_READ_QUEUE_DRAINED(d_receiveQueue.size());

        NTCS_METRICS_UPDATE_READ_QUEUE_SIZE(d_receiveQueue.size());

        ntca::ReceiveContext receiveContext;
        receiveContext.setTransport(d_transport);
        if (!queuedEndpoint.isNull()) {
            receiveContext.setEndpoint(queuedEndpoint.value());
        }

        ntca::ReceiveEvent receiveEvent;
        receiveEvent.setType(ntca::ReceiveEventType::e_COMPLETE);
        receiveEvent.setContext(receiveContext);

        ntcq::ReceiveCallbackQueueEntry::dispatch(callbackEntry,
                                                  self,
                                                  queuedData,
                                                  receiveEvent,
                                                  d_proactorStrand_sp,
                                                  self,
                                                  false,
                                                  &d_mutex);
    }

    if (d_receiveQueue.authorizeLowWatermarkEvent()) {
        NTCP_DATAGRAMSOCKET_LOG_READ_QUEUE_LOW_WATERMARK(
            d_receiveQueue.lowWatermark(),
            d_receiveQueue.size());

        if (d_session_sp) {
            ntca::ReadQueueEvent event;
            event.setType(ntca::ReadQueueEventType::e_LOW_WATERMARK);
            event.setContext(d_receiveQueue.context());

            ntcs::Dispatch::announceReadQueueLowWatermark(d_session_sp,
                                                          self,
                                                          event,
                                                          d_sessionStrand_sp,
                                                          d_proactorStrand_sp,
                                                          self,
                                                          false,
                                                          &d_mutex);
        }
    }

    if (d_receiveQueue.authorizeHighWatermarkEvent()) {
        NTCP_DATAGRAMSOCKET_LOG_READ_QUEUE_HIGH_WATERMARK(
            d_receiveQueue.highWatermark(),
            d_receiveQueue.size());

        this->privateApplyFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      ntca::FlowControlMode::e_IMMEDIATE,
                                      false,
                                      false);

        if (d_session_sp) {
            ntca::ReadQueueEvent event;
            event.setType(ntca::ReadQueueEventType::e_HIGH_WATERMARK);
            event.setContext(d_receiveQueue.context());

            ntcs::Dispatch::announceReadQueueHighWatermark(d_session_sp,
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

void DatagramSocket::privateFailReceive(
    const bsl::shared_ptr<DatagramSocket>& self,
    const ntsa::Error&                     error)
{
    this->privateFail(self, error);
}

void DatagramSocket::privateInitiateSend(
    const bsl::shared_ptr<DatagramSocket>& self)
{
    ntsa::Error error;

    if (d_sendPending) {
        return;
    }

    if (!d_flowControlState.wantSend()) {
        return;
    }

    if (!d_shutdownState.canSend()) {
        return;
    }

    ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
    if (!proactorRef) {
        this->privateFailSend(self, ntsa::Error(ntsa::Error::e_INVALID));
        return;
    }

    error = this->privateThrottleSendBuffer(self);
    if (error) {
        return;
    }

    while (d_sendQueue.hasEntry()) {
        ntcq::SendQueueEntry& entry = d_sendQueue.frontEntry();

        if (NTCCFG_LIKELY(entry.data())) {
            const bool hasDeadline = !entry.deadline().isNull();

            if (NTCCFG_LIKELY(d_systemRemoteEndpoint.isUndefined())) {
                if (entry.endpoint().isNull()) {
                    this->privateFailSend(self, ntsa::Error::invalid());
                    continue;
                }

                ntsa::SendOptions options;
                options.setEndpoint(entry.endpoint().value());

                error = proactorRef->send(self, *entry.data(), options);
            }
            else {
                if (!entry.endpoint().isNull() &&
                    entry.endpoint() != d_systemRemoteEndpoint)
                {
                    this->privateFailSend(self, ntsa::Error::invalid());
                    continue;
                }

                error = proactorRef->send(self,
                                          *entry.data(),
                                          ntsa::SendOptions());
            }

            if (error) {
                this->privateFailSend(self, error);
                continue;
            }

            if (hasDeadline) {
                entry.setDeadline(bdlb::NullableValue<bsls::TimeInterval>());
                entry.closeTimer();
            }

            d_sendPending = true;
            break;
        }
        else {
            d_sendQueue.popEntry();
            this->privateShutdownSend(self, false);
        }
    }
}

void DatagramSocket::privateCompleteSend(
    const bsl::shared_ptr<DatagramSocket>& self)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (!d_sendQueue.hasEntry()) {
        return;
    }

    ntcq::SendQueueEntry& entry = d_sendQueue.frontEntry();

    if (NTCCFG_UNLIKELY(d_sendRateLimiter_sp)) {
        d_sendRateLimiter_sp->submit(entry.length());
    }

    NTCS_METRICS_UPDATE_WRITE_QUEUE_DELAY(entry.delay());

    ntca::SendContext  context  = entry.context();
    ntci::SendCallback callback = entry.callback();

    d_sendQueue.popEntry();

    NTCP_DATAGRAMSOCKET_LOG_WRITE_QUEUE_DRAINED(d_sendQueue.size());

    NTCS_METRICS_UPDATE_WRITE_QUEUE_SIZE(d_sendQueue.size());

    if (callback) {
        ntca::SendEvent sendEvent;
        sendEvent.setType(ntca::SendEventType::e_COMPLETE);
        sendEvent.setContext(context);

        callback.dispatch(self,
                          sendEvent,
                          d_proactorStrand_sp,
                          self,
                          false,
                          &d_mutex);
    }

    if (d_sendQueue.authorizeLowWatermarkEvent()) {
        NTCP_DATAGRAMSOCKET_LOG_WRITE_QUEUE_LOW_WATERMARK(
            d_sendQueue.lowWatermark(),
            d_sendQueue.size());

        if (d_session_sp) {
            ntca::WriteQueueEvent event;
            event.setType(ntca::WriteQueueEventType::e_LOW_WATERMARK);
            event.setContext(d_sendQueue.context());

            ntcs::Dispatch::announceWriteQueueLowWatermark(d_session_sp,
                                                           self,
                                                           event,
                                                           d_sessionStrand_sp,
                                                           d_proactorStrand_sp,
                                                           self,
                                                           false,
                                                           &d_mutex);
        }
    }

    if (!d_sendQueue.hasEntry()) {
        this->privateApplyFlowControl(self,
                                      ntca::FlowControlType::e_SEND,
                                      ntca::FlowControlMode::e_IMMEDIATE,
                                      false,
                                      false);
    }
}

void DatagramSocket::privateFailSend(
    const bsl::shared_ptr<DatagramSocket>& self,
    const ntsa::Error&                     error)
{
    if (!d_sendQueue.hasEntry()) {
        return;
    }

    ntca::SendContext  context  = d_sendQueue.frontEntry().context();
    ntci::SendCallback callback = d_sendQueue.frontEntry().callback();

    d_sendQueue.popEntry();

    if (callback) {
        context.setError(error);

        ntca::SendEvent sendEvent;
        sendEvent.setType(ntca::SendEventType::e_ERROR);
        sendEvent.setContext(context);

        callback.dispatch(self,
                          sendEvent,
                          d_proactorStrand_sp,
                          self,
                          false,
                          &d_mutex);
    }
}

void DatagramSocket::privateFail(const bsl::shared_ptr<DatagramSocket>& self,
                                 const ntsa::Error&                     error)
{
    ntca::ErrorContext context;
    context.setError(error);

    ntca::ErrorEvent event;
    event.setType(ntca::ErrorEventType::e_TRANSPORT);
    event.setContext(context);

    this->privateFail(self, event);
}

void DatagramSocket::privateFail(const bsl::shared_ptr<DatagramSocket>& self,
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

ntsa::Error DatagramSocket::privateShutdown(
    const bsl::shared_ptr<DatagramSocket>& self,
    ntsa::ShutdownType::Value              direction,
    ntsa::ShutdownMode::Value              mode,
    bool                                   defer)
{
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
            if (mode == ntsa::ShutdownMode::e_GRACEFUL &&
                d_sendQueue.hasEntry())
            {
                ntcq::SendQueueEntry entry;
                entry.setId(d_sendQueue.generateEntryId());
                d_sendQueue.pushEntry(entry);
                this->privateRelaxFlowControl(self,
                                              ntca::FlowControlType::e_SEND,
                                              defer,
                                              true);
            }
            else {
                this->privateShutdownSend(self, defer);
            }
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

void DatagramSocket::privateShutdownSend(
    const bsl::shared_ptr<DatagramSocket>& self,
    bool                                   defer)
{
    bool keepHalfOpen = NTCCFG_DEFAULT_DATAGRAM_SOCKET_KEEP_HALF_OPEN;
    if (!d_options.keepHalfOpen().isNull()) {
        keepHalfOpen = d_options.keepHalfOpen().value();
    }

    ntcs::ShutdownContext context;
    if (d_shutdownState.tryShutdownSend(&context, keepHalfOpen)) {
        this->privateShutdownSequence(self,
                                      ntsa::ShutdownOrigin::e_SOURCE,
                                      context,
                                      defer);
    }
}

void DatagramSocket::privateShutdownReceive(
    const bsl::shared_ptr<DatagramSocket>& self,
    ntsa::ShutdownOrigin::Value            origin,
    bool                                   defer)
{
    bool keepHalfOpen = NTCCFG_DEFAULT_DATAGRAM_SOCKET_KEEP_HALF_OPEN;
    if (!d_options.keepHalfOpen().isNull()) {
        keepHalfOpen = d_options.keepHalfOpen().value();
    }

    ntcs::ShutdownContext context;
    if (d_shutdownState.tryShutdownReceive(&context, keepHalfOpen, origin)) {
        this->privateShutdownSequence(self, origin, context, defer);
    }
}

void DatagramSocket::privateShutdownSequence(
    const bsl::shared_ptr<DatagramSocket>& self,
    ntsa::ShutdownOrigin::Value            origin,
    const ntcs::ShutdownContext&           context,
    bool                                   defer)
{
    NTCCFG_WARNING_UNUSED(origin);

    // Forcibly override the indication that the announcements should be
    // deferred on execute on the strand or asynchonrously on the reactor.
    // The announcements must always be deferred, otherwise, the user may
    // process the announcements out-of-order, say, when the shutdown is
    // initiated by the calling thread but completed by the proactor thread.
    //
    // This only needs to be done when supported half-open connections.
    // Otherwise, the announcements are always deferred or always processed
    // immediately by the proactor thread.
    //
    // TODO: Remove the 'defer' parameter and always defer the announcements.
    //
    // bool keepHalfOpen = NTCCFG_DEFAULT_DATAGRAM_SOCKET_KEEP_HALF_OPEN;
    // if (!d_options.keepHalfOpen().isNull()) {
    //     keepHalfOpen = d_options.keepHalfOpen().value();
    // }
    // 
    // if (keepHalfOpen) {
    //     defer = true;
    // }

    defer = true;

    // First handle flow control and detachment from the proactor, if 
    // necessary.

    bool asyncDetachInitiated = false;

    if (context.shutdownCompleted()) {
        asyncDetachInitiated = this->privateCloseFlowControl(self, defer);
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

    if (!asyncDetachInitiated) {
        privateShutdownSequenceComplete(self, context, defer);
    }
    else {
        BSLS_ASSERT(!d_deferredCall);
        d_deferredCall =
            NTCCFG_BIND(&DatagramSocket::privateShutdownSequenceComplete,
                        this,
                        self,
                        context,
                        defer);
    }
}

void DatagramSocket::privateShutdownSequenceComplete(
    const bsl::shared_ptr<DatagramSocket>& self,
    const ntcs::ShutdownContext&           context,
    bool                                   defer)
{
    NTCI_LOG_CONTEXT();
    // Second handle socket shutdown.

    if (context.shutdownSend()) {
        if (d_detachState.goal() == ntcs::DetachGoal::e_CLOSE) {
            if (d_socket_sp) {
                d_socket_sp->shutdown(ntsa::ShutdownType::e_SEND);
            }
        }
    }

    if (context.shutdownReceive()) {
        if (d_detachState.goal() == ntcs::DetachGoal::e_CLOSE) {
            if (d_socket_sp) {
                d_socket_sp->shutdown(ntsa::ShutdownType::e_RECEIVE);
            }
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

        NTCP_DATAGRAMSOCKET_LOG_SHUTDOWN_SEND();

        typedef bsl::pair<ntca::SendContext, ntci::SendCallback> 
        SendContextCallback;

        bsl::vector<SendContextCallback> callbackVector;

        bool announceWriteQueueDiscarded = false;
        {
            if (d_sendRateTimer_sp) {
                d_sendRateTimer_sp->close();
                d_sendRateTimer_sp.reset();
            }

            {
                bsl::vector<ntcq::SendQueueEntry> sendQueueEntryVector;
                announceWriteQueueDiscarded =
                    d_sendQueue.removeAll(&sendQueueEntryVector);
                for (bsl::size_t i = 0; i < sendQueueEntryVector.size(); ++i) {
                    const ntcq::SendQueueEntry& entry = 
                        sendQueueEntryVector[i];
                    if (entry.callback()) {
                        callbackVector.push_back(
                            SendContextCallback(
                                entry.context(), entry.callback()));
                    }
                }
            }
        }

        for (bsl::size_t i = 0; i < callbackVector.size(); ++i) {
            ntca::SendContext  sendContext  = callbackVector[i].first;
            ntci::SendCallback sendCallback = callbackVector[i].second;

            sendContext.setError(ntsa::Error(ntsa::Error::e_CANCELLED));

            ntca::SendEvent sendEvent;
            sendEvent.setType(ntca::SendEventType::e_ERROR);
            sendEvent.setContext(sendContext);

            if (sendCallback) {
                sendCallback.dispatch(self,
                                      sendEvent,
                                      d_proactorStrand_sp,
                                      self,
                                      defer,
                                      &d_mutex);
            }
        }

        callbackVector.clear();

        if (announceWriteQueueDiscarded) {
            if (d_session_sp) {
                ntca::WriteQueueEvent event;
                event.setType(ntca::WriteQueueEventType::e_DISCARDED);
                event.setContext(d_sendQueue.context());

                ntcs::Dispatch::announceWriteQueueDiscarded(
                    d_session_sp,
                    self,
                    event,
                    d_sessionStrand_sp,
                    d_proactorStrand_sp,
                    self,
                    defer,
                    &d_mutex);
            }
        }

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

        NTCP_DATAGRAMSOCKET_LOG_SHUTDOWN_RECEIVE();

        if (d_receiveRateTimer_sp) {
            d_receiveRateTimer_sp->close();
            d_receiveRateTimer_sp.reset();
        }

        bsl::vector<bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> >
            callbackEntryVector;

        d_receiveQueue.popAllCallbackEntries(&callbackEntryVector);

        for (bsl::size_t i = 0; i < callbackEntryVector.size(); ++i) {
            ntca::ReceiveContext receiveContext;
            receiveContext.setError(ntsa::Error(ntsa::Error::e_EOF));
            receiveContext.setTransport(d_transport);

            ntca::ReceiveEvent receiveEvent;
            receiveEvent.setType(ntca::ReceiveEventType::e_ERROR);
            receiveEvent.setContext(receiveContext);

            ntcq::ReceiveCallbackQueueEntry::dispatch(
                callbackEntryVector[i],
                self,
                bsl::shared_ptr<bdlbb::Blob>(),
                receiveEvent,
                d_proactorStrand_sp,
                self,
                defer,
                &d_mutex);
        }

        callbackEntryVector.clear();

        // Force the announcement of a read queue low watermark event
        // to give the user the opportunity to receive EOF.

        if (d_session_sp) {
            ntca::ReadQueueEvent event;
            event.setType(ntca::ReadQueueEventType::e_LOW_WATERMARK);
            event.setContext(d_receiveQueue.context());

            ntcs::Dispatch::announceReadQueueLowWatermark(d_session_sp,
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

        if (d_socket_sp) {
            if (d_detachState.goal() == ntcs::DetachGoal::e_CLOSE) {
                d_socket_sp->close();
            }
            else {
                d_socket_sp->release();
            }
        }

        d_systemHandle = ntsa::k_INVALID_HANDLE;

        NTCI_LOG_TRACE("Datagram socket closed descriptor %d",
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

        d_sendDeflater_sp.reset();
        d_receiveInflater_sp.reset();

        d_sessionStrand_sp.reset();
        d_session_sp.reset();

        d_managerStrand_sp.reset();
        d_manager_sp.reset();
    }

    this->moveAndExecute(&d_deferredCalls, ntci::Executor::Functor());
    d_deferredCalls.clear();
}

ntsa::Error DatagramSocket::privateRelaxFlowControl(
    const bsl::shared_ptr<DatagramSocket>& self,
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
        if (relaxSend) {
            if (context.enableSend()) {
                if (d_shutdownState.canSend()) {
                    if (d_session_sp) {
                        ntca::WriteQueueEvent event;
                        event.setType(
                            ntca::WriteQueueEventType::e_FLOW_CONTROL_RELAXED);
                        event.setContext(d_sendQueue.context());

                        ntcs::Dispatch::announceWriteQueueFlowControlRelaxed(
                            d_session_sp,
                            self,
                            event,
                            d_sessionStrand_sp,
                            ntci::Strand::unknown(),
                            self,
                            defer,
                            &d_mutex);
                    }

                    this->privateInitiateSend(self);
                }
            }
        }

        if (relaxReceive) {
            if (context.enableReceive()) {
                if (d_shutdownState.canReceive()) {
                    if (d_session_sp) {
                        ntca::ReadQueueEvent event;
                        event.setType(
                            ntca::ReadQueueEventType::e_FLOW_CONTROL_RELAXED);
                        event.setContext(d_receiveQueue.context());

                        ntcs::Dispatch::announceReadQueueFlowControlRelaxed(
                            d_session_sp,
                            self,
                            event,
                            d_sessionStrand_sp,
                            ntci::Strand::unknown(),
                            self,
                            defer,
                            &d_mutex);
                    }

                    this->privateInitiateReceive(self);
                }
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error DatagramSocket::privateApplyFlowControl(
    const bsl::shared_ptr<DatagramSocket>& self,
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
        if (applySend) {
            if (!context.enableSend()) {
                if (d_session_sp) {
                    ntca::WriteQueueEvent event;
                    event.setType(
                        ntca::WriteQueueEventType::e_FLOW_CONTROL_APPLIED);
                    event.setContext(d_sendQueue.context());

                    ntcs::Dispatch::announceWriteQueueFlowControlApplied(
                        d_session_sp,
                        self,
                        event,
                        d_sessionStrand_sp,
                        ntci::Strand::unknown(),
                        self,
                        defer,
                        &d_mutex);
                }

                // Handled at the completion of the last send.
            }
        }

        if (applyReceive) {
            if (!context.enableReceive()) {
                if (d_session_sp) {
                    ntca::ReadQueueEvent event;
                    event.setType(
                        ntca::ReadQueueEventType::e_FLOW_CONTROL_APPLIED);
                    event.setContext(d_receiveQueue.context());

                    ntcs::Dispatch::announceReadQueueFlowControlApplied(
                        d_session_sp,
                        self,
                        event,
                        d_sessionStrand_sp,
                        ntci::Strand::unknown(),
                        self,
                        defer,
                        &d_mutex);
                }

                // Handled at the completion of the last receive.
            }
        }
    }

    return ntsa::Error();
}

bool DatagramSocket::privateCloseFlowControl(
    const bsl::shared_ptr<DatagramSocket>& self,
    bool                                   defer)
{
    bool applySend    = true;
    bool applyReceive = true;

    ntcs::FlowControlContext context;
    if (d_flowControlState.apply(&context,
                                 ntca::FlowControlType::e_BOTH,
                                 true))
    {
        if (applySend) {
            if (!context.enableSend()) {
                if (d_session_sp) {
                    ntca::WriteQueueEvent event;
                    event.setType(
                        ntca::WriteQueueEventType::e_FLOW_CONTROL_APPLIED);
                    event.setContext(d_sendQueue.context());

                    ntcs::Dispatch::announceWriteQueueFlowControlApplied(
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

        if (applyReceive) {
            if (!context.enableReceive()) {
                if (d_session_sp) {
                    ntca::ReadQueueEvent event;
                    event.setType(
                        ntca::ReadQueueEventType::e_FLOW_CONTROL_APPLIED);
                    event.setContext(d_receiveQueue.context());

                    ntcs::Dispatch::announceReadQueueFlowControlApplied(
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
            BSLS_ASSERT(d_detachState.mode() !=
                        ntcs::DetachMode::e_INITIATED);
            proactorRef->cancel(self);
            const ntsa::Error error = proactorRef->detachSocket(self);
            if (NTCCFG_UNLIKELY(error)) {
                return false;
            }
            else {
                d_detachState.setMode(ntcs::DetachMode::e_INITIATED);
                return true;
            }
        }
    }

    return false;
}

ntsa::Error DatagramSocket::privateThrottleSendBuffer(
    const bsl::shared_ptr<DatagramSocket>& self)
{
    NTCI_LOG_CONTEXT();

    if (NTCCFG_UNLIKELY(d_sendRateLimiter_sp)) {
        bsls::TimeInterval now = this->currentTime();
        if (NTCCFG_UNLIKELY(d_sendRateLimiter_sp->wouldExceedBandwidth(now))) {
            bsls::TimeInterval timeToSubmit =
                d_sendRateLimiter_sp->calculateTimeToSubmit(now);

            bsls::TimeInterval nextSendAttemptTime = now + timeToSubmit;

            if (NTCCFG_UNLIKELY(!d_sendRateTimer_sp)) {
                ntca::TimerOptions timerOptions;
                timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
                timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

                ntci::TimerCallback timerCallback = this->createTimerCallback(
                    bdlf::MemFnUtil::memFn(
                        &DatagramSocket::processSendRateTimer,
                        self),
                    d_allocator_p);

                d_sendRateTimer_sp = this->createTimer(timerOptions,
                                                       timerCallback,
                                                       d_allocator_p);
            }

            NTCP_DATAGRAMSOCKET_LOG_SEND_BUFFER_THROTTLE_APPLIED(timeToSubmit);

            this->privateApplyFlowControl(self,
                                          ntca::FlowControlType::e_SEND,
                                          ntca::FlowControlMode::e_IMMEDIATE,
                                          false,
                                          true);

            d_sendRateTimer_sp->schedule(nextSendAttemptTime);

            if (d_session_sp) {
                ntca::WriteQueueEvent event;
                event.setType(ntca::WriteQueueEventType::e_RATE_LIMIT_APPLIED);
                event.setContext(d_sendQueue.context());

                ntcs::Dispatch::announceWriteQueueRateLimitApplied(
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

ntsa::Error DatagramSocket::privateThrottleReceiveBuffer(
    const bsl::shared_ptr<DatagramSocket>& self)
{
    NTCI_LOG_CONTEXT();

    if (NTCCFG_UNLIKELY(d_receiveRateLimiter_sp)) {
        bsls::TimeInterval now = this->currentTime();
        if (NTCCFG_UNLIKELY(
                d_receiveRateLimiter_sp->wouldExceedBandwidth(now)))
        {
            bsls::TimeInterval timeToSubmit =
                d_receiveRateLimiter_sp->calculateTimeToSubmit(now);

            bsls::TimeInterval nextReceiveAttemptTime = now + timeToSubmit;

            if (NTCCFG_UNLIKELY(!d_receiveRateTimer_sp)) {
                ntca::TimerOptions timerOptions;
                timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
                timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

                ntci::TimerCallback timerCallback = this->createTimerCallback(
                    bdlf::MemFnUtil::memFn(
                        &DatagramSocket::processReceiveRateTimer,
                        self),
                    d_allocator_p);

                d_receiveRateTimer_sp = this->createTimer(timerOptions,
                                                          timerCallback,
                                                          d_allocator_p);
            }

            NTCP_DATAGRAMSOCKET_LOG_RECEIVE_BUFFER_THROTTLE_APPLIED(
                timeToSubmit);

            this->privateApplyFlowControl(self,
                                          ntca::FlowControlType::e_RECEIVE,
                                          ntca::FlowControlMode::e_IMMEDIATE,
                                          false,
                                          true);

            d_receiveRateTimer_sp->schedule(nextReceiveAttemptTime);

            if (d_session_sp) {
                ntca::ReadQueueEvent event;
                event.setType(ntca::ReadQueueEventType::e_RATE_LIMIT_APPLIED);
                event.setContext(d_receiveQueue.context());

                ntcs::Dispatch::announceReadQueueRateLimitApplied(
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

void DatagramSocket::privateAllocateReceiveBlob()
{
    if (!d_receiveBlob_sp) {
        d_receiveBlob_sp = d_dataPool_sp->createIncomingBlob();
    }

    BSLS_ASSERT(ntcs::BlobUtil::size(d_receiveBlob_sp) == 0);

    if (ntcs::BlobUtil::capacity(d_receiveBlob_sp) < d_maxDatagramSize) {
        BSLS_ASSERT(ntcs::BlobUtil::capacity(d_receiveBlob_sp) == 0);
        ntcs::BlobUtil::resize(d_receiveBlob_sp, d_maxDatagramSize);
        ntcs::BlobUtil::trim(d_receiveBlob_sp);
        ntcs::BlobUtil::resize(d_receiveBlob_sp, 0);

        NTCS_METRICS_UPDATE_BLOB_BUFFER_ALLOCATIONS(
            ntcs::BlobUtil::capacity(d_receiveBlob_sp));
    }

    BSLS_ASSERT(ntcs::BlobUtil::size(d_receiveBlob_sp) == 0);
    BSLS_ASSERT(ntcs::BlobUtil::capacity(d_receiveBlob_sp) ==
                d_maxDatagramSize);
}

bool DatagramSocket::isDatagram() const
{
    return true;
}

ntsa::Error DatagramSocket::privateOpen(
    const bsl::shared_ptr<DatagramSocket>& self)
{
    if (d_systemHandle != ntsa::k_INVALID_HANDLE) {
        return ntsa::Error();
    }

    return this->privateOpen(self, d_options.transport());
}

ntsa::Error DatagramSocket::privateOpen(
    const bsl::shared_ptr<DatagramSocket>& self,
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

    error = ntcu::DatagramSocketUtil::validateTransport(transport);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntsi::DatagramSocket> datagramSocket;
    if (d_socket_sp) {
        datagramSocket = d_socket_sp;
    }
    else {
        datagramSocket = ntsf::System::createDatagramSocket(d_allocator_p);
    }

    error = this->privateOpen(self, transport, datagramSocket);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error DatagramSocket::privateOpen(
    const bsl::shared_ptr<DatagramSocket>& self,
    const ntsa::Endpoint&                  endpoint)
{
    return this->privateOpen(
        self,
        endpoint.transport(ntsa::TransportMode::e_DATAGRAM));
}

ntsa::Error DatagramSocket::privateOpen(
    const bsl::shared_ptr<DatagramSocket>& self,
    ntsa::Transport::Value                 transport,
    ntsa::Handle                           handle)
{
    ntsa::Error error;

    bsl::shared_ptr<ntsi::DatagramSocket> datagramSocket;
    if (d_socket_sp) {
        datagramSocket = d_socket_sp;
        error          = datagramSocket->acquire(handle);
        if (error) {
            return error;
        }
    }
    else {
        datagramSocket =
            ntsf::System::createDatagramSocket(handle, d_allocator_p);
    }

    error = this->privateOpen(self, transport, datagramSocket);
    if (error) {
        datagramSocket->release();
        return error;
    }

    return ntsa::Error();
}

ntsa::Error DatagramSocket::privateOpen(
    const bsl::shared_ptr<DatagramSocket>&       self,
    ntsa::Transport::Value                       transport,
    const bsl::shared_ptr<ntsi::DatagramSocket>& datagramSocket)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (d_systemHandle != ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = ntcu::DatagramSocketUtil::validateTransport(transport);
    if (error) {
        return error;
    }

    if (datagramSocket->handle() == ntsa::k_INVALID_HANDLE) {
        error = datagramSocket->open(transport);
        if (error) {
            return error;
        }
    }

    ntsa::Handle handle = datagramSocket->handle();

    ntsa::Endpoint sourceEndpoint;
    ntsa::Endpoint remoteEndpoint;

    error = ntcs::Compat::configure(datagramSocket, d_options);
    if (error) {
        return error;
    }

    if (d_options.compressionConfig().has_value()) {
        if (d_options.compressionConfig().value().type() != 
            ntca::CompressionType::e_UNDEFINED &&
            d_options.compressionConfig().value().type() != 
            ntca::CompressionType::e_NONE)
        {
            bsl::shared_ptr<ntci::CompressionDriver> compressionDriver;
            error = ntcs::Plugin::lookupCompressionDriver(&compressionDriver);
            if (error) {
                return error;
            }

            bsl::shared_ptr<ntci::Compression> compression;
            error = compressionDriver->createCompression(
                &compression,
                d_options.compressionConfig().value(),
                d_dataPool_sp,
                d_allocator_p);
            if (error) {
                return error;
            }

            d_sendDeflater_sp = compression;
            d_receiveInflater_sp = compression;
        }
    }

    error = datagramSocket->setBlocking(false);
    if (error) {
        return error;
    }

    if (!d_options.sourceEndpoint().isNull()) {
        error = datagramSocket->bind(d_options.sourceEndpoint().value(),
                                     d_options.reuseAddress());
        if (error) {
            return error;
        }
    }

    error = datagramSocket->sourceEndpoint(&sourceEndpoint);
    if (error) {
        sourceEndpoint.reset();
    }

    error = datagramSocket->remoteEndpoint(&remoteEndpoint);
    if (error) {
        remoteEndpoint.reset();
    }

    d_transport            = transport;
    d_systemHandle         = handle;
    d_systemSourceEndpoint = sourceEndpoint;
    d_systemRemoteEndpoint = remoteEndpoint;
    d_publicHandle         = handle;
    d_publicSourceEndpoint = sourceEndpoint;
    d_publicRemoteEndpoint = remoteEndpoint;
    d_socket_sp            = datagramSocket;

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    NTCI_LOG_TRACE("Datagram socket opened descriptor %d",
                   (int)(d_publicHandle));

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

    return ntsa::Error();
}

void DatagramSocket::processSourceEndpointResolution(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const ntsa::Endpoint&                  endpoint,
    const ntca::GetEndpointEvent&          getEndpointEvent,
    const ntca::BindOptions&               bindOptions,
    const ntci::BindCallback&              bindCallback)
{
    NTCCFG_WARNING_UNUSED(resolver);

    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

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

    if (!error) {
        error = d_socket_sp->bind(endpoint, d_options.reuseAddress());
    }

    if (!error) {
        error = d_socket_sp->sourceEndpoint(&d_systemSourceEndpoint);
        d_publicSourceEndpoint = d_systemSourceEndpoint;
    }

    ntca::BindEvent bindEvent;
    if (!error) {
        bindEvent.setType(ntca::BindEventType::e_COMPLETE);
        bindContext.setEndpoint(d_systemSourceEndpoint);
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

void DatagramSocket::processRemoteEndpointResolution(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const ntsa::Endpoint&                  endpoint,
    const ntca::GetEndpointEvent&          getEndpointEvent,
    const ntca::ConnectOptions&            connectOptions,
    const ntci::ConnectCallback&           connectCallback)
{
    NTCCFG_WARNING_UNUSED(resolver);

    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    ntsa::Error          error;
    ntca::ConnectContext connectContext;

    if (getEndpointEvent.type() == ntca::GetEndpointEventType::e_ERROR) {
        error = getEndpointEvent.context().error();
    }
    else {
        connectContext.setName(getEndpointEvent.context().authority());

        if (getEndpointEvent.context().latency() != bsls::TimeInterval()) {
            connectContext.setLatency(getEndpointEvent.context().latency());
        }

        if (!getEndpointEvent.context().nameServer().isNull()) {
            connectContext.setNameServer(
                getEndpointEvent.context().nameServer().value());
        }

        if (getEndpointEvent.context().source() !=
            ntca::ResolverSource::e_UNKNOWN)
        {
            connectContext.setSource(getEndpointEvent.context().source());
        }
    }

    if (!error) {
        error = this->privateOpen(self, endpoint);
    }

    if (!error) {
        if (d_transport == ntsa::Transport::e_LOCAL_DATAGRAM) {
            if (d_systemSourceEndpoint.isImplicit()) {
                error = d_socket_sp->bindAny(d_transport,
                                             d_options.reuseAddress());

                if (!error) {
                    error = d_socket_sp->sourceEndpoint(
                        &d_systemSourceEndpoint);
                    d_publicSourceEndpoint = d_systemSourceEndpoint;
                }
            }
        }
    }

    if (!error) {
        error = d_socket_sp->connect(endpoint);
    }

    if (!error) {
        error = d_socket_sp->sourceEndpoint(&d_systemSourceEndpoint);
        d_publicSourceEndpoint = d_systemSourceEndpoint;
    }

    if (!error) {
        error = d_socket_sp->remoteEndpoint(&d_systemRemoteEndpoint);
    }

    d_publicRemoteEndpoint = d_systemRemoteEndpoint;

    ntca::ConnectEvent connectEvent;
    if (!error) {
        connectEvent.setType(ntca::ConnectEventType::e_COMPLETE);
        connectContext.setEndpoint(d_systemSourceEndpoint);
    }
    else {
        connectEvent.setType(ntca::ConnectEventType::e_ERROR);
        connectContext.setError(error);
    }

    connectEvent.setContext(connectContext);

    if (connectCallback) {
        const bool defer = !connectOptions.recurse();

        connectCallback.dispatch(self,
                                 connectEvent,
                                 ntci::Strand::unknown(),
                                 self,
                                 defer,
                                 &d_mutex);
    }
}

void DatagramSocket::privateClose(
    const bsl::shared_ptr<DatagramSocket>& self,
    const ntci::CloseCallback&             callback)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    if (d_detachState.mode() == ntcs::DetachMode::e_INITIATED) {
        d_deferredCalls.push_back(NTCCFG_BIND(
            static_cast<void (DatagramSocket::*)(
                const ntci::CloseCallback& callback)>(&DatagramSocket::close),
            self,
            callback));
        return;
    }

    BSLS_ASSERT(!d_closeCallback);
    d_closeCallback = callback;

    this->privateShutdown(self,
                          ntsa::ShutdownType::e_BOTH,
                          ntsa::ShutdownMode::e_IMMEDIATE,
                          true);
}

DatagramSocket::DatagramSocket(
    const ntca::DatagramSocketOptions&         options,
    const bsl::shared_ptr<ntci::Resolver>&     resolver,
    const bsl::shared_ptr<ntci::Proactor>&     proactor,
    const bsl::shared_ptr<ntci::ProactorPool>& proactorPool,
    const bsl::shared_ptr<ntcs::Metrics>&      metrics,
    bslma::Allocator*                          basicAllocator)
: d_object("ntcp::DatagramSocket")
, d_mutex()
, d_transport(ntsa::Transport::e_UNDEFINED)
, d_systemHandle(ntsa::k_INVALID_HANDLE)
, d_systemSourceEndpoint()
, d_systemRemoteEndpoint()
, d_publicHandle(ntsa::k_INVALID_HANDLE)
, d_publicSourceEndpoint()
, d_publicRemoteEndpoint()
, d_socket_sp()
#if NTCP_DATAGRAMSOCKET_OBSERVE_BY_WEAK_PTR
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
, d_sendQueue(basicAllocator)
, d_sendDeflater_sp()
, d_sendRateLimiter_sp()
, d_sendRateTimer_sp()
, d_sendPending(false)
, d_sendGreedily(NTCCFG_DEFAULT_DATAGRAM_SOCKET_WRITE_GREEDILY)
, d_sendComplete(basicAllocator)
, d_receiveQueue(basicAllocator)
, d_receiveInflater_sp()
, d_receiveRateLimiter_sp()
, d_receiveRateTimer_sp()
, d_receivePending(false)
, d_receiveGreedily(NTCCFG_DEFAULT_DATAGRAM_SOCKET_READ_GREEDILY)
, d_receiveBlob_sp()
, d_maxDatagramSize(NTCCFG_DEFAULT_DATAGRAM_SOCKET_MAX_MESSAGE_SIZE)
, d_options(options)
, d_detachState()
, d_deferredCall()
, d_closeCallback(bslma::Default::allocator(basicAllocator))
, d_deferredCalls(bslma::Default::allocator(basicAllocator))
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (!d_options.maxDatagramSize().isNull()) {
        d_maxDatagramSize = d_options.maxDatagramSize().value();
    }

    if (!d_options.writeQueueLowWatermark().isNull()) {
        d_sendQueue.setLowWatermark(
            d_options.writeQueueLowWatermark().value());
    }

    if (!d_options.writeQueueHighWatermark().isNull()) {
        d_sendQueue.setHighWatermark(
            d_options.writeQueueHighWatermark().value());
    }

    if (!d_options.sendGreedily().isNull()) {
        d_sendGreedily = d_options.sendGreedily().value();
    }

    if (proactor->maxThreads() > 1) {
        d_receiveQueue.setTrigger(ntca::ReactorEventTrigger::e_EDGE);
    }

    if (!d_options.readQueueLowWatermark().isNull()) {
        d_receiveQueue.setLowWatermark(
            d_options.readQueueLowWatermark().value());
    }

    if (!d_options.readQueueHighWatermark().isNull()) {
        d_receiveQueue.setHighWatermark(
            d_options.readQueueHighWatermark().value());
    }

    if (!d_options.receiveGreedily().isNull()) {
        d_receiveGreedily = d_options.receiveGreedily().value();
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

        ntcs::MonitorableUtil::registerMonitorable(d_metrics_sp);
    }
    else {
        d_metrics_sp = metrics;
    }
}

DatagramSocket::~DatagramSocket()
{
    if (!d_options.metrics().isNull() && d_options.metrics().value()) {
        if (d_metrics_sp) {
            ntcs::MonitorableUtil::deregisterMonitorable(d_metrics_sp);
        }
    }
}

ntsa::Error DatagramSocket::open()
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    return this->privateOpen(self);
}

ntsa::Error DatagramSocket::open(ntsa::Transport::Value transport)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    return this->privateOpen(self, transport);
}

ntsa::Error DatagramSocket::open(ntsa::Transport::Value transport,
                                 ntsa::Handle           handle)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    return this->privateOpen(self, transport, handle);
}

ntsa::Error DatagramSocket::open(
    ntsa::Transport::Value                       transport,
    const bsl::shared_ptr<ntsi::DatagramSocket>& datagramSocket)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    return this->privateOpen(self, transport, datagramSocket);
}

ntsa::Error DatagramSocket::bind(const ntsa::Endpoint&     endpoint,
                                 const ntca::BindOptions&  options,
                                 const ntci::BindFunction& callback)
{
    return this->bind(endpoint,
                      options,
                      this->createBindCallback(callback, d_allocator_p));
}

ntsa::Error DatagramSocket::bind(const ntsa::Endpoint&     endpoint,
                                 const ntca::BindOptions&  options,
                                 const ntci::BindCallback& callback)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

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

    error = d_socket_sp->sourceEndpoint(&d_systemSourceEndpoint);
    if (error) {
        return error;
    }

    d_publicSourceEndpoint = d_systemSourceEndpoint;

    if (callback) {
        ntca::BindContext bindContext;
        bindContext.setEndpoint(d_systemSourceEndpoint);

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

ntsa::Error DatagramSocket::bind(const bsl::string&        name,
                                 const ntca::BindOptions&  options,
                                 const ntci::BindFunction& callback)
{
    return this->bind(name,
                      options,
                      this->createBindCallback(callback, d_allocator_p));
}

ntsa::Error DatagramSocket::bind(const bsl::string&        name,
                                 const ntca::BindOptions&  options,
                                 const ntci::BindCallback& callback)
{
    if (d_options.transport() == ntsa::Transport::e_LOCAL_DATAGRAM ||
        bdls::PathUtil::isAbsolute(name))
    {
        ntsa::LocalName localName;
        localName.setValue(name);
        return this->bind(ntsa::Endpoint(localName), options, callback);
    }

    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

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
            NTCCFG_BIND(&DatagramSocket::processSourceEndpointResolution,
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

ntsa::Error DatagramSocket::connect(const ntsa::Endpoint&        endpoint,
                                    const ntca::ConnectOptions&  options,
                                    const ntci::ConnectFunction& callback)
{
    return this->connect(endpoint,
                         options,
                         this->createConnectCallback(callback, d_allocator_p));
}

ntsa::Error DatagramSocket::connect(const ntsa::Endpoint&        endpoint,
                                    const ntca::ConnectOptions&  options,
                                    const ntci::ConnectCallback& callback)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (endpoint.isIp()) {
        if (endpoint.ip().host().isV4()) {
            if (endpoint.ip().host().v4().isAny()) {
                return ntsa::Error::invalid();
            }
        }
        else if (endpoint.ip().host().isV6()) {
            if (endpoint.ip().host().v6().isAny()) {
                return ntsa::Error::invalid();
            }
        }

        if (endpoint.ip().port() == 0) {
            return ntsa::Error::invalid();
        }
    }

    error = this->privateOpen(self, endpoint);
    if (error) {
        return error;
    }

    if (d_transport == ntsa::Transport::e_LOCAL_DATAGRAM) {
        if (d_systemSourceEndpoint.isImplicit()) {
            error =
                d_socket_sp->bindAny(d_transport, d_options.reuseAddress());
            if (error) {
                return error;
            }

            error = d_socket_sp->sourceEndpoint(&d_systemSourceEndpoint);
            if (error) {
                return error;
            }

            d_publicSourceEndpoint = d_systemSourceEndpoint;
        }
    }

    error = d_socket_sp->connect(endpoint);
    if (error) {
        return error;
    }

    error = d_socket_sp->sourceEndpoint(&d_systemSourceEndpoint);
    if (error) {
        return error;
    }

    d_publicSourceEndpoint = d_systemSourceEndpoint;

    error = d_socket_sp->remoteEndpoint(&d_systemRemoteEndpoint);
    if (error) {
        return error;
    }

    d_publicRemoteEndpoint = d_systemRemoteEndpoint;

    if (callback) {
        ntca::ConnectContext connectContext;
        connectContext.setEndpoint(d_systemRemoteEndpoint);

        ntca::ConnectEvent connectEvent;
        connectEvent.setType(ntca::ConnectEventType::e_COMPLETE);
        connectEvent.setContext(connectContext);

        const bool defer = !options.recurse();

        callback.dispatch(self,
                          connectEvent,
                          ntci::Strand::unknown(),
                          self,
                          defer,
                          &d_mutex);
    }

    return ntsa::Error();
}

ntsa::Error DatagramSocket::connect(const bsl::string&           name,
                                    const ntca::ConnectOptions&  options,
                                    const ntci::ConnectFunction& callback)
{
    return this->connect(name,
                         options,
                         this->createConnectCallback(callback, d_allocator_p));
}

ntsa::Error DatagramSocket::connect(const bsl::string&           name,
                                    const ntca::ConnectOptions&  options,
                                    const ntci::ConnectCallback& callback)
{
    if (d_options.transport() == ntsa::Transport::e_LOCAL_DATAGRAM ||
        bdls::PathUtil::isAbsolute(name))
    {
        ntsa::LocalName localName;
        localName.setValue(name);
        return this->connect(ntsa::Endpoint(localName), options, callback);
    }

    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

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
            NTCCFG_BIND(&DatagramSocket::processRemoteEndpointResolution,
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

ntsa::Error DatagramSocket::send(const bdlbb::Blob&       data,
                                 const ntca::SendOptions& options)
{
    return this->send(data, options, d_sendComplete);
}

ntsa::Error DatagramSocket::send(const ntsa::Data&        data,
                                 const ntca::SendOptions& options)
{
    return this->send(data, options, d_sendComplete);
}

ntsa::Error DatagramSocket::send(const bdlbb::Blob&        data,
                                 const ntca::SendOptions&  options,
                                 const ntci::SendFunction& callback)
{
    return this->send(data,
                      options,
                      this->createSendCallback(callback, d_allocator_p));
}

ntsa::Error DatagramSocket::send(const bdlbb::Blob&        data,
                                 const ntca::SendOptions&  options,
                                 const ntci::SendCallback& callback)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    if (NTCCFG_UNLIKELY(NTCCFG_WARNING_PROMOTE(bsl::size_t, data.length()) >
                        d_maxDatagramSize))
    {
        return ntsa::Error::invalid();
    }

    bsl::size_t effectiveHighWatermark = d_sendQueue.highWatermark();
    if (!options.highWatermark().isNull()) {
        effectiveHighWatermark = options.highWatermark().value();
    }

    if (NTCCFG_UNLIKELY(
            d_sendQueue.isHighWatermarkViolated(effectiveHighWatermark)))
    {
        if (d_sendQueue.authorizeHighWatermarkEvent(effectiveHighWatermark)) {
            NTCP_DATAGRAMSOCKET_LOG_WRITE_QUEUE_HIGH_WATERMARK(
                effectiveHighWatermark,
                d_sendQueue.size());

            if (d_session_sp) {
                ntca::WriteQueueEvent event;
                event.setType(ntca::WriteQueueEventType::e_HIGH_WATERMARK);
                event.setContext(d_sendQueue.context());

                ntcs::Dispatch::announceWriteQueueHighWatermark(
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

        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    ntsa::Error error;

    ntca::SendContext context;

    if (options.token().has_value()) {
        context.setToken(options.token().value());
    }

    bsl::shared_ptr<ntsa::Data> dataContainer =
        d_dataPool_sp->createOutgoingData();

    bsl::size_t dataLength = 0;

    if (NTCCFG_LIKELY(!d_sendDeflater_sp)) {
        dataContainer->makeBlob(data);
        dataLength = static_cast<bsl::size_t>(data.length());
    }
    else {
        bdlbb::Blob& deflatedData = dataContainer->makeBlob();

        ntca::DeflateOptions deflateOptions;
        ntca::DeflateContext deflateContext;

        error = d_sendDeflater_sp->deflate(&deflateContext, 
                                           &deflatedData, 
                                           data, 
                                           deflateOptions);
        if (error) {
            return error;
        }

        context.setCompressionType(deflateContext.compressionType());
        context.setCompressionRatio(
            static_cast<double>(deflateContext.bytesWritten()) / 
            deflateContext.bytesRead());

        dataLength = static_cast<bsl::size_t>(deflatedData.length());
    }

    ntcq::SendQueueEntry entry;
    entry.setId(d_sendQueue.generateEntryId());
    entry.setContext(context);
    entry.setEndpoint(options.endpoint());
    entry.setData(dataContainer);
    entry.setLength(dataLength);
    entry.setTimestamp(bsls::TimeUtil::getTimer());

    if (callback) {
        entry.setCallback(callback);
    }

    if (NTCCFG_UNLIKELY(!options.deadline().isNull())) {
        ntca::TimerOptions timerOptions;
        timerOptions.setOneShot(true);
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
        timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

        ntci::TimerCallback timerCallback = this->createTimerCallback(
            bdlf::BindUtil::bind(&DatagramSocket::processSendDeadlineTimer,
                                 self,
                                 bdlf::PlaceHolders::_1,
                                 bdlf::PlaceHolders::_2,
                                 entry.id()),
            d_allocator_p);

        bsl::shared_ptr<ntci::Timer> timer =
            this->createTimer(timerOptions, timerCallback, d_allocator_p);

        entry.setDeadline(options.deadline().value());
        entry.setTimer(timer);

        timer->schedule(options.deadline().value());
    }

    bool becameNonEmpty = d_sendQueue.pushEntry(entry);

    NTCP_DATAGRAMSOCKET_LOG_WRITE_QUEUE_FILLED(d_sendQueue.size());

    NTCS_METRICS_UPDATE_WRITE_QUEUE_SIZE(d_sendQueue.size());

    if (d_sendQueue.authorizeHighWatermarkEvent()) {
        NTCP_DATAGRAMSOCKET_LOG_WRITE_QUEUE_HIGH_WATERMARK(
            d_sendQueue.highWatermark(),
            d_sendQueue.size());

        if (d_session_sp) {
            ntca::WriteQueueEvent event;
            event.setType(ntca::WriteQueueEventType::e_HIGH_WATERMARK);
            event.setContext(d_sendQueue.context());

            ntcs::Dispatch::announceWriteQueueHighWatermark(
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

    if (becameNonEmpty) {
        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_SEND,
                                      true,
                                      false);
    }

    return ntsa::Error();
}

ntsa::Error DatagramSocket::send(const ntsa::Data&         data,
                                 const ntca::SendOptions&  options,
                                 const ntci::SendFunction& callback)
{
    return this->send(data,
                      options,
                      this->createSendCallback(callback, d_allocator_p));
}

ntsa::Error DatagramSocket::send(const ntsa::Data&         data,
                                 const ntca::SendOptions&  options,
                                 const ntci::SendCallback& callback)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    if (NTCCFG_UNLIKELY(data.size() > d_maxDatagramSize)) {
        return ntsa::Error::invalid();
    }

    bsl::size_t effectiveHighWatermark = d_sendQueue.highWatermark();
    if (!options.highWatermark().isNull()) {
        effectiveHighWatermark = options.highWatermark().value();
    }

    if (NTCCFG_UNLIKELY(
            d_sendQueue.isHighWatermarkViolated(effectiveHighWatermark)))
    {
        if (d_sendQueue.authorizeHighWatermarkEvent(effectiveHighWatermark)) {
            NTCP_DATAGRAMSOCKET_LOG_WRITE_QUEUE_HIGH_WATERMARK(
                effectiveHighWatermark,
                d_sendQueue.size());

            if (d_session_sp) {
                ntca::WriteQueueEvent event;
                event.setType(ntca::WriteQueueEventType::e_HIGH_WATERMARK);
                event.setContext(d_sendQueue.context());

                ntcs::Dispatch::announceWriteQueueHighWatermark(
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

        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    ntsa::Error error;

    ntca::SendContext context;

    if (options.token().has_value()) {
        context.setToken(options.token().value());
    }

    bsl::shared_ptr<ntsa::Data> dataContainer =
        d_dataPool_sp->createOutgoingData();

    bsl::size_t dataLength = 0;

    if (NTCCFG_LIKELY(!d_sendDeflater_sp)) {

        *dataContainer = data;
        dataLength = dataContainer->size();
    }
    else {
        bdlbb::Blob& deflatedData = dataContainer->makeBlob();

        ntca::DeflateOptions deflateOptions;
        ntca::DeflateContext deflateContext;

        error = d_sendDeflater_sp->deflate(&deflateContext, 
                                           &deflatedData, 
                                           data, 
                                           deflateOptions);
        if (error) {
            return error;
        }

        context.setCompressionType(deflateContext.compressionType());
        context.setCompressionRatio(
            static_cast<double>(deflateContext.bytesWritten()) / 
            deflateContext.bytesRead());

        dataLength = static_cast<bsl::size_t>(deflatedData.length());
    }

    ntcq::SendQueueEntry entry;
    entry.setId(d_sendQueue.generateEntryId());
    entry.setContext(context);
    entry.setEndpoint(options.endpoint());
    entry.setData(dataContainer);
    entry.setLength(dataLength);
    entry.setTimestamp(bsls::TimeUtil::getTimer());

    if (callback) {
        entry.setCallback(callback);
    }

    if (NTCCFG_UNLIKELY(!options.deadline().isNull())) {
        ntca::TimerOptions timerOptions;
        timerOptions.setOneShot(true);
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
        timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

        ntci::TimerCallback timerCallback = this->createTimerCallback(
            bdlf::BindUtil::bind(&DatagramSocket::processSendDeadlineTimer,
                                 self,
                                 bdlf::PlaceHolders::_1,
                                 bdlf::PlaceHolders::_2,
                                 entry.id()),
            d_allocator_p);

        bsl::shared_ptr<ntci::Timer> timer =
            this->createTimer(timerOptions, timerCallback, d_allocator_p);

        entry.setDeadline(options.deadline().value());
        entry.setTimer(timer);

        timer->schedule(options.deadline().value());
    }

    bool becameNonEmpty = d_sendQueue.pushEntry(entry);

    NTCP_DATAGRAMSOCKET_LOG_WRITE_QUEUE_FILLED(d_sendQueue.size());

    NTCS_METRICS_UPDATE_WRITE_QUEUE_SIZE(d_sendQueue.size());

    if (d_sendQueue.authorizeHighWatermarkEvent()) {
        NTCP_DATAGRAMSOCKET_LOG_WRITE_QUEUE_HIGH_WATERMARK(
            d_sendQueue.highWatermark(),
            d_sendQueue.size());

        if (d_session_sp) {
            ntca::WriteQueueEvent event;
            event.setType(ntca::WriteQueueEventType::e_HIGH_WATERMARK);
            event.setContext(d_sendQueue.context());

            ntcs::Dispatch::announceWriteQueueHighWatermark(
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

    if (becameNonEmpty) {
        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_SEND,
                                      true,
                                      false);
    }

    return ntsa::Error();
}

ntsa::Error DatagramSocket::receive(ntca::ReceiveContext*       context,
                                    bdlbb::Blob*                data,
                                    const ntca::ReceiveOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    ntsa::Error error;

    if (NTCCFG_UNLIKELY(!d_receiveQueue.hasEntry() &&
                        !d_shutdownState.canReceive()))
    {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    if (NTCCFG_LIKELY(d_receiveQueue.hasEntry())) {
        bool receiveQueueHighWatermarkViolatedBefore =
            d_receiveQueue.isHighWatermarkViolated();

        ntcq::ReceiveQueueEntry& entry = d_receiveQueue.frontEntry();

        context->setTransport(d_transport);
        if (!entry.endpoint().isNull()) {
            context->setEndpoint(entry.endpoint().value());
        }
        *data = *entry.data();

        NTCS_METRICS_UPDATE_READ_QUEUE_DELAY(entry.delay());

        d_receiveQueue.popEntry();

        NTCP_DATAGRAMSOCKET_LOG_READ_QUEUE_DRAINED(d_receiveQueue.size());

        NTCS_METRICS_UPDATE_READ_QUEUE_SIZE(d_receiveQueue.size());

        bool receiveQueueHighWatermarkViolatedAfter =
            d_receiveQueue.isHighWatermarkViolated();

        if (receiveQueueHighWatermarkViolatedBefore &&
            !receiveQueueHighWatermarkViolatedAfter)
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

ntsa::Error DatagramSocket::receive(const ntca::ReceiveOptions&  options,
                                    const ntci::ReceiveFunction& callback)
{
    return this->receive(options,
                         this->createReceiveCallback(callback, d_allocator_p));
}

ntsa::Error DatagramSocket::receive(const ntca::ReceiveOptions&  options,
                                    const ntci::ReceiveCallback& callback)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    ntsa::Error error;

    if (NTCCFG_UNLIKELY(!d_receiveQueue.hasEntry() &&
                        !d_shutdownState.canReceive()))
    {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> callbackEntry =
        d_receiveQueue.createCallbackEntry();
    callbackEntry->assign(callback, options);

    if (NTCCFG_LIKELY(!d_receiveQueue.hasCallbackEntry() &&
                      d_receiveQueue.hasEntry()))
    {
        bool receiveQueueHighWatermarkViolatedBefore =
            d_receiveQueue.isHighWatermarkViolated();

        ntcq::ReceiveQueueEntry& entry = d_receiveQueue.frontEntry();

        bdlb::NullableValue<ntsa::Endpoint> endpoint = entry.endpoint();
        bsl::shared_ptr<bdlbb::Blob>        data     = entry.data();

        NTCS_METRICS_UPDATE_READ_QUEUE_DELAY(entry.delay());

        d_receiveQueue.popEntry();

        NTCP_DATAGRAMSOCKET_LOG_READ_QUEUE_DRAINED(d_receiveQueue.size());

        NTCS_METRICS_UPDATE_READ_QUEUE_SIZE(d_receiveQueue.size());

        ntca::ReceiveContext receiveContext;
        receiveContext.setTransport(d_transport);
        if (!endpoint.isNull()) {
            receiveContext.setEndpoint(endpoint.value());
        }

        ntca::ReceiveEvent receiveEvent;
        receiveEvent.setType(ntca::ReceiveEventType::e_COMPLETE);
        receiveEvent.setContext(receiveContext);

        const bool defer = !options.recurse();

        ntcq::ReceiveCallbackQueueEntry::dispatch(callbackEntry,
                                                  self,
                                                  data,
                                                  receiveEvent,
                                                  ntci::Strand::unknown(),
                                                  self,
                                                  defer,
                                                  &d_mutex);

        bool receiveQueueHighWatermarkViolatedAfter =
            d_receiveQueue.isHighWatermarkViolated();

        if (receiveQueueHighWatermarkViolatedBefore &&
            !receiveQueueHighWatermarkViolatedAfter)
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
                    &DatagramSocket::processReceiveDeadlineTimer,
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

        d_receiveQueue.pushCallbackEntry(callbackEntry);
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

ntsa::Error DatagramSocket::registerResolver(
    const bsl::shared_ptr<ntci::Resolver>& resolver)
{
    LockGuard lock(&d_mutex);
    d_resolver = resolver;
    return ntsa::Error();
}

ntsa::Error DatagramSocket::deregisterResolver()
{
    LockGuard lock(&d_mutex);
    d_resolver.reset();
    return ntsa::Error();
}

ntsa::Error DatagramSocket::registerManager(
    const bsl::shared_ptr<ntci::DatagramSocketManager>& manager)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

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

ntsa::Error DatagramSocket::deregisterManager()
{
    LockGuard lock(&d_mutex);

    d_manager_sp.reset();
    d_managerStrand_sp.reset();

    return ntsa::Error();
}

ntsa::Error DatagramSocket::registerSession(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (session) {
        d_session_sp       = session;
        d_sessionStrand_sp = d_session_sp->strand();

        if (!d_sessionStrand_sp) {
            d_sessionStrand_sp = d_proactorStrand_sp;
        }

        if (d_sessionStrand_sp) {
            d_receiveQueue.setTrigger(ntca::ReactorEventTrigger::e_EDGE);
        }
        else {
            d_receiveQueue.setTrigger(ntca::ReactorEventTrigger::e_LEVEL);
        }
    }
    else {
        d_session_sp.reset();
        d_sessionStrand_sp.reset();
    }

    return ntsa::Error();
}

ntsa::Error DatagramSocket::registerSessionCallback(
    const ntci::DatagramSocket::SessionCallback& callback)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (callback) {
        bsl::shared_ptr<ntcu::DatagramSocketSession> session;
        session.createInplace(d_allocator_p,
                              callback,
                              d_proactorStrand_sp,
                              d_allocator_p);

        d_session_sp       = session;
        d_sessionStrand_sp = d_session_sp->strand();

        if (!d_sessionStrand_sp) {
            d_sessionStrand_sp = d_proactorStrand_sp;
        }

        if (d_sessionStrand_sp) {
            d_receiveQueue.setTrigger(ntca::ReactorEventTrigger::e_EDGE);
        }
        else {
            d_receiveQueue.setTrigger(ntca::ReactorEventTrigger::e_LEVEL);
        }
    }
    else {
        d_session_sp.reset();
        d_sessionStrand_sp.reset();
    }

    return ntsa::Error();
}

ntsa::Error DatagramSocket::registerSessionCallback(
    const ntci::DatagramSocket::SessionCallback& callback,
    const bsl::shared_ptr<ntci::Strand>&         strand)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (callback) {
        bsl::shared_ptr<ntcu::DatagramSocketSession> session;
        session.createInplace(d_allocator_p, callback, strand, d_allocator_p);

        d_session_sp       = session;
        d_sessionStrand_sp = d_session_sp->strand();

        if (!d_sessionStrand_sp) {
            d_sessionStrand_sp = d_proactorStrand_sp;
        }

        if (d_sessionStrand_sp) {
            d_receiveQueue.setTrigger(ntca::ReactorEventTrigger::e_EDGE);
        }
        else {
            d_receiveQueue.setTrigger(ntca::ReactorEventTrigger::e_LEVEL);
        }
    }
    else {
        d_session_sp.reset();
        d_sessionStrand_sp.reset();
    }

    return ntsa::Error();
}

ntsa::Error DatagramSocket::deregisterSession()
{
    LockGuard lock(&d_mutex);

    d_session_sp.reset();
    d_sessionStrand_sp.reset();

    return ntsa::Error();
}

ntsa::Error DatagramSocket::setWriteDeflater(
        const bsl::shared_ptr<ntci::Compression>& compression)
{
    LockGuard lock(&d_mutex);

    d_sendDeflater_sp = compression;

    return ntsa::Error();
}

ntsa::Error DatagramSocket::setWriteRateLimiter(
    const bsl::shared_ptr<ntci::RateLimiter>& rateLimiter)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    d_sendRateLimiter_sp = rateLimiter;

    if (!d_sendRateLimiter_sp) {
        if (d_sendRateTimer_sp) {
            d_sendRateTimer_sp->close();
            d_sendRateTimer_sp.reset();
        }

        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_SEND,
                                      true,
                                      true);
    }

    return ntsa::Error();
}

ntsa::Error DatagramSocket::setWriteQueueLowWatermark(bsl::size_t lowWatermark)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    d_sendQueue.setLowWatermark(lowWatermark);

    if (d_sendQueue.authorizeLowWatermarkEvent()) {
        if (d_session_sp) {
            ntca::WriteQueueEvent event;
            event.setType(ntca::WriteQueueEventType::e_LOW_WATERMARK);
            event.setContext(d_sendQueue.context());

            ntcs::Dispatch::announceWriteQueueLowWatermark(
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

ntsa::Error DatagramSocket::setWriteQueueHighWatermark(
    bsl::size_t highWatermark)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    d_sendQueue.setHighWatermark(highWatermark);

    if (d_sendQueue.authorizeHighWatermarkEvent()) {
        if (d_session_sp) {
            ntca::WriteQueueEvent event;
            event.setType(ntca::WriteQueueEventType::e_HIGH_WATERMARK);
            event.setContext(d_sendQueue.context());

            ntcs::Dispatch::announceWriteQueueHighWatermark(
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

ntsa::Error DatagramSocket::setWriteQueueWatermarks(bsl::size_t lowWatermark,
                                                    bsl::size_t highWatermark)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    d_sendQueue.setLowWatermark(lowWatermark);
    d_sendQueue.setHighWatermark(highWatermark);

    if (d_sendQueue.authorizeLowWatermarkEvent()) {
        if (d_session_sp) {
            ntca::WriteQueueEvent event;
            event.setType(ntca::WriteQueueEventType::e_LOW_WATERMARK);
            event.setContext(d_sendQueue.context());

            ntcs::Dispatch::announceWriteQueueLowWatermark(
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

    if (d_sendQueue.authorizeHighWatermarkEvent()) {
        if (d_session_sp) {
            ntca::WriteQueueEvent event;
            event.setType(ntca::WriteQueueEventType::e_HIGH_WATERMARK);
            event.setContext(d_sendQueue.context());

            ntcs::Dispatch::announceWriteQueueHighWatermark(
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

ntsa::Error DatagramSocket::setReadInflater(
        const bsl::shared_ptr<ntci::Compression>& compression)
{
    LockGuard lock(&d_mutex);

    d_receiveInflater_sp = compression;

    return ntsa::Error();
}

ntsa::Error DatagramSocket::setReadRateLimiter(
    const bsl::shared_ptr<ntci::RateLimiter>& rateLimiter)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    d_receiveRateLimiter_sp = rateLimiter;

    if (!d_receiveRateLimiter_sp) {
        if (d_receiveRateTimer_sp) {
            d_receiveRateTimer_sp->close();
            d_receiveRateTimer_sp.reset();
        }

        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      true,
                                      true);
    }

    return ntsa::Error();
}

ntsa::Error DatagramSocket::setReadQueueLowWatermark(bsl::size_t lowWatermark)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    d_receiveQueue.setLowWatermark(lowWatermark);

    if (!d_receiveQueue.isLowWatermarkSatisfied()) {
        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      true,
                                      false);
    }
    else {
        if (d_session_sp) {
            ntca::ReadQueueEvent event;
            event.setType(ntca::ReadQueueEventType::e_LOW_WATERMARK);
            event.setContext(d_receiveQueue.context());

            ntcs::Dispatch::announceReadQueueLowWatermark(
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

ntsa::Error DatagramSocket::setReadQueueHighWatermark(
    bsl::size_t highWatermark)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    d_receiveQueue.setHighWatermark(highWatermark);

    if (d_receiveQueue.isHighWatermarkViolated()) {
        this->privateApplyFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      ntca::FlowControlMode::e_IMMEDIATE,
                                      true,
                                      false);
    }

    return ntsa::Error();
}

ntsa::Error DatagramSocket::setReadQueueWatermarks(bsl::size_t lowWatermark,
                                                   bsl::size_t highWatermark)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    d_receiveQueue.setLowWatermark(lowWatermark);
    d_receiveQueue.setHighWatermark(highWatermark);

    if (!d_receiveQueue.isLowWatermarkSatisfied()) {
        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      true,
                                      false);
    }

    if (d_receiveQueue.isHighWatermarkViolated()) {
        this->privateApplyFlowControl(self,
                                      ntca::FlowControlType::e_RECEIVE,
                                      ntca::FlowControlMode::e_IMMEDIATE,
                                      true,
                                      false);
    }

    return ntsa::Error();
}

ntsa::Error DatagramSocket::setMulticastLoopback(bool value)
{
    LockGuard lock(&d_mutex);

    if (d_socket_sp) {
        return d_socket_sp->setMulticastLoopback(value);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error DatagramSocket::setMulticastTimeToLive(bsl::size_t value)
{
    LockGuard lock(&d_mutex);

    if (d_socket_sp) {
        return d_socket_sp->setMulticastTimeToLive(value);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error DatagramSocket::setMulticastInterface(const ntsa::IpAddress& value)
{
    LockGuard lock(&d_mutex);

    if (d_socket_sp) {
        return d_socket_sp->setMulticastInterface(value);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error DatagramSocket::joinMulticastGroup(
    const ntsa::IpAddress& interface,
    const ntsa::IpAddress& group)
{
    LockGuard lock(&d_mutex);

    if (d_socket_sp) {
        return d_socket_sp->joinMulticastGroup(interface, group);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error DatagramSocket::leaveMulticastGroup(
    const ntsa::IpAddress& interface,
    const ntsa::IpAddress& group)
{
    LockGuard lock(&d_mutex);

    if (d_socket_sp) {
        return d_socket_sp->leaveMulticastGroup(interface, group);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error DatagramSocket::relaxFlowControl(
    ntca::FlowControlType::Value direction)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    return this->privateRelaxFlowControl(self, direction, true, true);
}

ntsa::Error DatagramSocket::applyFlowControl(
    ntca::FlowControlType::Value direction,
    ntca::FlowControlMode::Value mode)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    if (direction == ntca::FlowControlType::e_SEND ||
        direction == ntca::FlowControlType::e_BOTH)
    {
        if (d_sendRateTimer_sp) {
            d_sendRateTimer_sp->close();
            d_sendRateTimer_sp.reset();
        }
    }

    if (direction == ntca::FlowControlType::e_RECEIVE ||
        direction == ntca::FlowControlType::e_BOTH)
    {
        if (d_receiveRateTimer_sp) {
            d_receiveRateTimer_sp->close();
            d_receiveRateTimer_sp.reset();
        }
    }

    return this->privateApplyFlowControl(self, direction, mode, true, true);
}

ntsa::Error DatagramSocket::cancel(const ntca::BindToken& token)
{
    NTCCFG_WARNING_UNUSED(token);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::cancel(const ntca::ConnectToken& token)
{
    NTCCFG_WARNING_UNUSED(token);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::cancel(const ntca::SendToken& token)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_systemRemoteEndpoint);

    ntci::SendCallback callback;
    ntca::SendContext  context;

    bool becameEmpty = d_sendQueue.removeEntryToken(&callback, 
                                                    &context,
                                                    token);

    if (becameEmpty) {
        this->privateApplyFlowControl(self,
                                      ntca::FlowControlType::e_SEND,
                                      ntca::FlowControlMode::e_IMMEDIATE,
                                      true,
                                      false);
    }

    if (callback) {
        context.setError(ntsa::Error(ntsa::Error::e_CANCELLED));

        ntca::SendEvent sendEvent;
        sendEvent.setType(ntca::SendEventType::e_ERROR);
        sendEvent.setContext(context);

        callback.dispatch(self,
                          sendEvent,
                          d_proactorStrand_sp,
                          self,
                          true,
                          &d_mutex);

        return ntsa::Error();
    }

    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error DatagramSocket::cancel(const ntca::ReceiveToken& token)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_systemRemoteEndpoint);

    bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> callbackEntry;
    ntsa::Error                                      error =
        d_receiveQueue.removeCallbackEntry(&callbackEntry, token);
    if (!error) {
        ntca::ReceiveContext receiveContext;
        receiveContext.setError(ntsa::Error(ntsa::Error::e_CANCELLED));
        receiveContext.setTransport(d_transport);

        ntca::ReceiveEvent receiveEvent;
        receiveEvent.setType(ntca::ReceiveEventType::e_ERROR);
        receiveEvent.setContext(receiveContext);

        ntcq::ReceiveCallbackQueueEntry::dispatch(
            callbackEntry,
            self,
            bsl::shared_ptr<bdlbb::Blob>(),
            receiveEvent,
            d_proactorStrand_sp,
            self,
            true,
            &d_mutex);

        return ntsa::Error();
    }

    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error DatagramSocket::shutdown(ntsa::ShutdownType::Value direction,
                                     ntsa::ShutdownMode::Value mode)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_systemSourceEndpoint);

    this->privateShutdown(self, direction, mode, true);
    return ntsa::Error();
}

ntsa::Error DatagramSocket::release(ntsa::Handle* result)
{
    return this->release(result, ntci::CloseCallback());
}

ntsa::Error DatagramSocket::release(ntsa::Handle*              result, 
                                    const ntci::CloseFunction& callback)
{
    return this->release(
        result, this->createCloseCallback(callback, d_allocator_p));
}

ntsa::Error DatagramSocket::release(ntsa::Handle*              result,
                                    const ntci::CloseCallback& callback)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);
    
    LockGuard lock(&d_mutex);

    *result = ntsa::k_INVALID_HANDLE;

    if (d_socket_sp) {
        *result = d_socket_sp->handle();
    }

    if (*result == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_detachState.setGoal(ntcs::DetachGoal::e_EXPORT);

    d_manager_sp.reset();
    d_session_sp.reset();
    
    this->privateClose(self, callback);

    return ntsa::Error();
}

void DatagramSocket::close()
{
    this->close(ntci::CloseCallback());
}

void DatagramSocket::close(const ntci::CloseFunction& callback)
{
    this->close(this->createCloseCallback(callback, d_allocator_p));
}

void DatagramSocket::close(const ntci::CloseCallback& callback)
{
    bsl::shared_ptr<DatagramSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    this->privateClose(self, callback);
}

void DatagramSocket::execute(const Functor& functor)
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

void DatagramSocket::moveAndExecute(FunctorSequence* functorSequence,
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
bsl::shared_ptr<ntci::Strand> DatagramSocket::createStrand(
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

bsl::shared_ptr<ntci::Timer> DatagramSocket::createTimer(
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

bsl::shared_ptr<ntci::Timer> DatagramSocket::createTimer(
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

bsl::shared_ptr<ntsa::Data> DatagramSocket::createIncomingData()
{
    return d_dataPool_sp->createIncomingData();
}

bsl::shared_ptr<ntsa::Data> DatagramSocket::createOutgoingData()
{
    return d_dataPool_sp->createOutgoingData();
}

bsl::shared_ptr<bdlbb::Blob> DatagramSocket::createIncomingBlob()
{
    return d_dataPool_sp->createIncomingBlob();
}

bsl::shared_ptr<bdlbb::Blob> DatagramSocket::createOutgoingBlob()
{
    return d_dataPool_sp->createOutgoingBlob();
}

void DatagramSocket::createIncomingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_incomingBufferFactory_sp->allocate(blobBuffer);
}

void DatagramSocket::createOutgoingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_outgoingBufferFactory_sp->allocate(blobBuffer);
}

ntsa::Handle DatagramSocket::handle() const
{
    return d_publicHandle;
}

ntsa::Transport::Value DatagramSocket::transport() const
{
    return d_transport;
}

ntsa::Endpoint DatagramSocket::sourceEndpoint() const
{
    ntsa::Endpoint result;
    d_publicSourceEndpoint.load(&result);
    return result;
}

ntsa::Endpoint DatagramSocket::remoteEndpoint() const
{
    ntsa::Endpoint result;
    d_publicRemoteEndpoint.load(&result);
    return result;
}

const bsl::shared_ptr<ntci::Strand>& DatagramSocket::strand() const
{
    return d_proactorStrand_sp;
}

bslmt::ThreadUtil::Handle DatagramSocket::threadHandle() const
{
    ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
    if (proactorRef) {
        return proactorRef->threadHandle();
    }
    else {
        return bslmt::ThreadUtil::invalidHandle();
    }
}

bsl::size_t DatagramSocket::threadIndex() const
{
    ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
    if (proactorRef) {
        return proactorRef->threadIndex();
    }
    else {
        return 0;
    }
}

bsl::size_t DatagramSocket::readQueueSize() const
{
    LockGuard lock(&d_mutex);
    return d_receiveQueue.size();
}

bsl::size_t DatagramSocket::readQueueLowWatermark() const
{
    LockGuard lock(&d_mutex);
    return d_receiveQueue.lowWatermark();
}

bsl::size_t DatagramSocket::readQueueHighWatermark() const
{
    LockGuard lock(&d_mutex);
    return d_receiveQueue.highWatermark();
}

bsl::size_t DatagramSocket::writeQueueSize() const
{
    LockGuard lock(&d_mutex);
    return d_sendQueue.size();
}

bsl::size_t DatagramSocket::writeQueueLowWatermark() const
{
    LockGuard lock(&d_mutex);
    return d_sendQueue.lowWatermark();
}

bsl::size_t DatagramSocket::writeQueueHighWatermark() const
{
    LockGuard lock(&d_mutex);
    return d_sendQueue.highWatermark();
}

bsl::size_t DatagramSocket::totalBytesSent() const
{
    // TODO
    return 0;
}

bsl::size_t DatagramSocket::totalBytesReceived() const
{
    // TODO
    return 0;
}

bsls::TimeInterval DatagramSocket::currentTime() const
{
    return bdlt::CurrentTime::now();
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& DatagramSocket::
    incomingBlobBufferFactory() const
{
    return d_incomingBufferFactory_sp;
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& DatagramSocket::
    outgoingBlobBufferFactory() const
{
    return d_outgoingBufferFactory_sp;
}

}  // close package namespace
}  // close enterprise namespace
