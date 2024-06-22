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

#include <ntcp_streamsocket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcp_streamsocket_cpp, "$Id$ $CSID$")

#include <ntccfg_bind.h>
#include <ntccfg_limits.h>
#include <ntci_encryption.h>
#include <ntci_encryptioncertificate.h>
#include <ntci_log.h>
#include <ntci_monitorable.h>
#include <ntcm_monitorableutil.h>
#include <ntcs_async.h>
#include <ntcs_blobbufferutil.h>
#include <ntcs_blobutil.h>
#include <ntcs_compat.h>
#include <ntcs_dispatch.h>
#include <ntcu_streamsocketsession.h>
#include <ntcu_streamsocketutil.h>
#include <ntsa_distinguishedname.h>
#include <ntsa_error.h>
#include <ntsf_system.h>
#include <ntsi_streamsocket.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobutil.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdls_pathutil.h>
#include <bdls_processutil.h>
#include <bdlt_currenttime.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bsls_assert.h>
#include <bsls_timeutil.h>
#include <bsl_algorithm.h>

// Define to 1 to observe object using weak pointers, otherwise objects are
// observed using raw pointers.
#define NTCP_STREAMSOCKET_OBSERVE_BY_WEAK_PTR 0

// The number of calls to 'sendmsg' before the size of the socket send buffer
// is refreshed.
#define NTCP_STREAMSOCKET_SEND_BUFFER_REFRESH_INTERVAL 100

// The payload size threshold that triggers whether the size of the socket
// send buffer should be evaluated to be refreshed.
#define NTCP_STREAMSOCKET_SEND_BUFFER_REFRESH_SIZE_THRESHOLD (1024 * 16)

#define NTCP_STREAMSOCKET_LOG_ENCRYPTION_UPGRADE_STARTING()                   \
    NTCI_LOG_DEBUG("Encryption upgrade starting")

#define NTCP_STREAMSOCKET_LOG_ENCRYPTION_UPGRADE_COMPLETE(certificate)        \
    do {                                                                      \
        if (certificate) {                                                    \
            bsl::string subject;                                              \
            certificate->subject().generate(&subject);                        \
            bsl::string issuer;                                               \
            certificate->issuer().generate(&issuer);                          \
            NTCI_LOG_DEBUG("Encryption upgrade complete to "                  \
                           "peer '%s' issued by '%s'",                        \
                           subject.c_str(),                                   \
                           issuer.c_str());                                   \
        }                                                                     \
        else {                                                                \
            NTCI_LOG_DEBUG("Encryption upgrade complete");                    \
        }                                                                     \
    } while (false)

#define NTCP_STREAMSOCKET_LOG_ENCRYPTION_UPGRADE_FAILED(details)              \
    NTCI_LOG_DEBUG("Encryption upgrade failed: %s", details.c_str())

#define NTCP_STREAMSOCKET_LOG_RECEIVE_BUFFER_THROTTLE_APPLIED(timeToSubmit)   \
    NTCI_LOG_TRACE("Stream socket receive buffer throttle applied for %d "    \
                   "milliseconds",                                            \
                   (int)((timeToSubmit).totalMilliseconds()))

#define NTCP_STREAMSOCKET_LOG_RECEIVE_BUFFER_THROTTLE_RELAXED()               \
    NTCI_LOG_TRACE("Stream socket receive buffer throttle relaxed")

#define NTCP_STREAMSOCKET_LOG_RECEIVE_BUFFER_UNDERFLOW()                      \
    NTCI_LOG_TRACE("Stream socket "                                           \
                   "has emptied the socket receive buffer")

#define NTCP_STREAMSOCKET_LOG_RECEIVE_RESULT(context)                         \
    NTCI_LOG_TRACE("Stream socket "                                           \
                   "has copied %zu bytes out of %zu bytes attempted from "    \
                   "the socket receive buffer",                               \
                   (context).bytesReceived(),                                 \
                   (context).bytesReceivable())

#define NTCP_STREAMSOCKET_LOG_RECEIVE_FAILURE(error)                          \
    NTCI_LOG_TRACE("Stream socket "                                           \
                   "failed to receive: %s",                                   \
                   (error).text().c_str())

#define NTCP_STREAMSOCKET_LOG_READ_QUEUE_FILLED(size)                         \
    NTCI_LOG_TRACE("Stream socket "                                           \
                   "has filled the read queue up to %zu bytes",               \
                   size)

#define NTCP_STREAMSOCKET_LOG_READ_QUEUE_DRAINED(size)                        \
    NTCI_LOG_TRACE("Stream socket "                                           \
                   "has drained the read queue down to %zu bytes",            \
                   size)

#define NTCP_STREAMSOCKET_LOG_END_OF_ENCRYPTION()                             \
    NTCI_LOG_TRACE("Stream socket "                                           \
                   "has read all encrypted data from its peer")

#define NTCP_STREAMSOCKET_LOG_END_OF_FILE()                                   \
    NTCI_LOG_TRACE("Stream socket "                                           \
                   "has read all data from its peer")

#define NTCP_STREAMSOCKET_LOG_READ_QUEUE_LOW_WATERMARK(lowWatermark, size)    \
    NTCI_LOG_TRACE("Stream socket "                                           \
                   "has satisfied the read queue low watermark of %zu "       \
                   "bytes with a read queue of %zu bytes",                    \
                   lowWatermark,                                              \
                   size)

#define NTCP_STREAMSOCKET_LOG_READ_QUEUE_HIGH_WATERMARK(highWatermark, size)  \
    NTCI_LOG_TRACE("Stream socket "                                           \
                   "has breached the read queue high watermark of %zu "       \
                   "bytes with a read queue of %zu bytes",                    \
                   highWatermark,                                             \
                   size)

#define NTCP_STREAMSOCKET_LOG_SHUTDOWN_RECEIVE()                              \
    NTCI_LOG_TRACE("Stream socket "                                           \
                   "is shutting down reception")

#define NTCP_STREAMSOCKET_LOG_SEND_BUFFER_THROTTLE_APPLIED(timeToSubmit)      \
    NTCI_LOG_TRACE("Stream socket send buffer throttle applied for %d "       \
                   "milliseconds",                                            \
                   (int)((timeToSubmit).totalMilliseconds()))

#define NTCP_STREAMSOCKET_LOG_SEND_BUFFER_THROTTLE_RELAXED()                  \
    NTCI_LOG_TRACE("Stream socket send buffer throttle relaxed")

#define NTCP_STREAMSOCKET_LOG_SEND_BUFFER_OVERFLOW()                          \
    NTCI_LOG_TRACE("Stream socket "                                           \
                   "has saturated the socket send buffer")

#define NTCP_STREAMSOCKET_LOG_SEND_RESULT(context)                            \
    NTCI_LOG_TRACE("Stream socket "                                           \
                   "has copied %zu bytes out of %zu bytes attempted to "      \
                   "the socket send buffer",                                  \
                   (context).bytesSent(),                                     \
                   (context).bytesSendable())

#define NTCP_STREAMSOCKET_LOG_SEND_FAILURE(error)                             \
    NTCI_LOG_TRACE("Stream socket "                                           \
                   "failed to send: %s",                                      \
                   (error).text().c_str())

#define NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_FILLED(size)                        \
    NTCI_LOG_TRACE("Stream socket "                                           \
                   "has filled the write queue up to %zu bytes",              \
                   size)

#define NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_DRAINED(size)                       \
    NTCI_LOG_DEBUG("Stream socket "                                           \
                   "has drained the write queue down to %zu bytes",           \
                   size)

#define NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_LOW_WATERMARK(lowWatermark, size)   \
    NTCI_LOG_TRACE("Stream socket "                                           \
                   "has satisfied the write queue low watermark of %zu "      \
                   "bytes with a write queue of %zu bytes",                   \
                   lowWatermark,                                              \
                   size)

#define NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_HIGH_WATERMARK(highWatermark, size) \
    NTCI_LOG_TRACE("Stream socket "                                           \
                   "has breached the write queue high watermark of %d "       \
                   "bytes with a write queue of %d bytes",                    \
                   highWatermark,                                             \
                   size)

#define NTCP_STREAMSOCKET_LOG_SHUTDOWN_SEND()                                 \
    NTCI_LOG_TRACE("Stream socket "                                           \
                   "is shutting down transmission")

// Some versions of GCC erroneously warn ntcs::ObserverRef::d_shared may be
// uninitialized.
#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

namespace BloombergLP {
namespace ntcp {

void StreamSocket::processSocketConnected(const ntsa::Error& error)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (NTCCFG_UNLIKELY(d_detachState.get() ==
                        ntcs::DetachState::e_DETACH_INITIATED))
    {
        return;
    }

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    if (error) {
        if (error != ntsa::Error::e_CANCELLED) {
            this->privateFailConnect(self, error, false, false);
        }
    }
    else {
        this->privateCompleteConnect(self);
    }
}

void StreamSocket::processSocketReceived(const ntsa::Error&          error,
                                         const ntsa::ReceiveContext& context)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (NTCCFG_UNLIKELY(d_detachState.get() ==
                        ntcs::DetachState::e_DETACH_INITIATED))
    {
        return;
    }

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    d_receivePending = false;

    if (error) {
        if (error != ntsa::Error::e_CANCELLED) {
            NTCP_STREAMSOCKET_LOG_RECEIVE_FAILURE(error);
            this->privateFailReceive(self, error);
        }
    }
    else {
        NTCP_STREAMSOCKET_LOG_RECEIVE_RESULT(context);
        this->privateCompleteReceive(self,
                                     context.bytesReceivable(),
                                     context.bytesReceived());
    }

    this->privateInitiateReceive(self);
}

void StreamSocket::processSocketSent(const ntsa::Error&       error,
                                     const ntsa::SendContext& context)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (NTCCFG_UNLIKELY(d_detachState.get() ==
                        ntcs::DetachState::e_DETACH_INITIATED))
    {
        return;
    }

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    d_sendPending = false;

    if (error) {
        if (error != ntsa::Error::e_CANCELLED) {
            NTCP_STREAMSOCKET_LOG_SEND_FAILURE(error);
            this->privateFailSend(self, error);
        }
    }
    else {
        NTCP_STREAMSOCKET_LOG_SEND_RESULT(context);
        this->privateCompleteSend(self, context.bytesSent());
    }

    this->privateInitiateSend(self);
}

void StreamSocket::processSocketError(const ntsa::Error& error)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (NTCCFG_UNLIKELY(d_detachState.get() ==
                        ntcs::DetachState::e_DETACH_INITIATED))
    {
        return;
    }  // TODO: or it's better to defer it?

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    this->privateFail(self, error);
}

void StreamSocket::processSocketDetached()
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

void StreamSocket::processConnectDeadlineTimer(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_WARNING_UNUSED(timer);

    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        if (NTCCFG_UNLIKELY(d_detachState.get() ==
                            ntcs::DetachState::e_DETACH_INITIATED))
        {
            d_retryConnect = false;

            d_deferredCalls.push_back(
                NTCCFG_BIND(&StreamSocket::processConnectDeadlineTimer,
                            self,
                            timer,
                            event));
            return;
        }

        if (d_connectInProgress) {
            this->privateFailConnect(
                self,
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT),
                false,
                true);
        }
    }
}

void StreamSocket::processConnectRetryTimer(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_WARNING_UNUSED(timer);

    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        if (d_connectInProgress) {
            if (d_connectAttempts > 0) {
                d_retryConnect =
                    true;  // privateRetryConnect will be called in privateFailConnectPart2
                if (d_detachState.get() !=
                    ntcs::DetachState::e_DETACH_INITIATED)
                {
                    this->privateFailConnect(
                        self,
                        ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT),
                        false,
                        false);
                }
            }
            else {
                this->privateRetryConnect(self);
            }
        }
    }
}

void StreamSocket::processUpgradeTimer(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_WARNING_UNUSED(timer);

    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        // MRM: Log upgrade timeout

        if (d_upgradeInProgress) {
            this->privateFailUpgrade(self,
                                     ntsa::Error(ntsa::Error::e_WOULD_BLOCK),
                                     "The operation has timed out");
        }
    }
}

void StreamSocket::processSendRateTimer(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_WARNING_UNUSED(timer);

    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        NTCP_STREAMSOCKET_LOG_SEND_BUFFER_THROTTLE_RELAXED();

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

void StreamSocket::processSendDeadlineTimer(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event,
    bsl::uint64_t                       entryId)
{
    NTCCFG_WARNING_UNUSED(timer);

    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        ntci::SendCallback callback;
        bool becameEmpty = d_sendQueue.removeEntryId(&callback, entryId);
        if (becameEmpty) {
            this->privateApplyFlowControl(self,
                                          ntca::FlowControlType::e_SEND,
                                          ntca::FlowControlMode::e_IMMEDIATE,
                                          false,
                                          false);
        }

        if (callback) {
            ntca::SendContext sendContext;
            sendContext.setError(ntsa::Error(ntsa::Error::e_WOULD_BLOCK));

            ntca::SendEvent sendEvent;
            sendEvent.setType(ntca::SendEventType::e_ERROR);
            sendEvent.setContext(sendContext);

            callback.dispatch(self,
                              sendEvent,
                              d_proactorStrand_sp,
                              self,
                              false,
                              &d_mutex);
        }
    }
}

void StreamSocket::processReceiveRateTimer(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_WARNING_UNUSED(timer);

    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        NTCP_STREAMSOCKET_LOG_RECEIVE_BUFFER_THROTTLE_RELAXED();

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

void StreamSocket::processReceiveDeadlineTimer(
    const bsl::shared_ptr<ntci::Timer>&                     timer,
    const ntca::TimerEvent&                                 event,
    const bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>& entry)
{
    NTCCFG_WARNING_UNUSED(timer);

    NTCCFG_OBJECT_GUARD(&d_object);

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        ntsa::Error error = d_receiveQueue.removeCallbackEntry(entry);
        if (!error) {
            ntca::ReceiveContext receiveContext;
            receiveContext.setError(ntsa::Error(ntsa::Error::e_WOULD_BLOCK));
            receiveContext.setTransport(d_transport);
            receiveContext.setEndpoint(d_remoteEndpoint);

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

void StreamSocket::privateEncryptionHandshake(
    const ntsa::Error&                                  error,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const bsl::string&                                  details)
{
    // IMPLEMENTATION NOTE: This function is always called under a lock
    // during the exection of 'd_encryption->popIncomingCipherText()'.

    NTCI_LOG_CONTEXT();

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    if (!error) {
        NTCP_STREAMSOCKET_LOG_ENCRYPTION_UPGRADE_COMPLETE(certificate);

        d_upgradeInProgress = false;

        ntci::UpgradeCallback upgradeCallback = d_upgradeCallback;
        d_upgradeCallback.reset();

        ntca::UpgradeContext context;

        ntca::UpgradeEvent event;
        event.setType(ntca::UpgradeEventType::e_COMPLETE);
        event.setContext(context);

        if (d_upgradeTimer_sp) {
            d_upgradeTimer_sp->close();
            d_upgradeTimer_sp.reset();
        }

        if (upgradeCallback) {
            upgradeCallback.dispatch(self,
                                     event,
                                     d_proactorStrand_sp,
                                     self,
                                     false,
                                     &d_mutex);
        }
    }
    else {
        NTCP_STREAMSOCKET_LOG_ENCRYPTION_UPGRADE_FAILED(details);

        this->privateFailUpgrade(self, error, details);
    }
}

void StreamSocket::privateCompleteConnect(
    const bsl::shared_ptr<StreamSocket>& self)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (!d_connectInProgress) {
        return;
    }

    ntsa::Error lastError;
    d_socket_sp->getLastError(&lastError);
    if (!lastError) {
        lastError = ntsa::Error::invalid();
    }

    error = d_socket_sp->remoteEndpoint(&d_remoteEndpoint);
    if (NTCCFG_UNLIKELY(error)) {
        NTCS_METRICS_UPDATE_CONNECT_FAILURE();

        if (lastError == ntsa::Error::invalid()) {
            lastError = error;
        }

        this->privateFailConnect(self, lastError, false, false);
        return;
    }

    error = d_socket_sp->sourceEndpoint(&d_sourceEndpoint);
    if (NTCCFG_UNLIKELY(error)) {
        NTCS_METRICS_UPDATE_CONNECT_FAILURE();

        if (lastError == ntsa::Error::invalid()) {
            lastError = error;
        }

        this->privateFailConnect(self, lastError, false, false);
        return;
    }

    {
        bsl::size_t sendBufferSize;

        ntsa::SocketOption option(d_allocator_p);
        error =
            d_socket_sp->getOption(&option,
                                   ntsa::SocketOptionType::e_SEND_BUFFER_SIZE);
        if (!error) {
            sendBufferSize = option.sendBufferSize();
        }
        else {
            sendBufferSize = 0;
        }

        if (sendBufferSize > 0) {
            d_sendOptions.setMaxBytes(sendBufferSize * 2);
        }
    }

    {
        bsl::size_t receiveBufferSize;

        ntsa::SocketOption option(d_allocator_p);
        error = d_socket_sp->getOption(
            &option,
            ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE);
        if (!error) {
            receiveBufferSize = option.receiveBufferSize();
        }
        else {
            receiveBufferSize = 0;
        }

        if (receiveBufferSize > 0) {
            d_receiveOptions.setMaxBytes(receiveBufferSize);
        }
    }

    d_sendOptions.setMaxBuffers(d_socket_sp->maxBuffersPerSend());
    d_receiveOptions.setMaxBuffers(d_socket_sp->maxBuffersPerReceive());

    NTCS_METRICS_UPDATE_CONNECT_COMPLETE();

    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    bsls::TimeInterval now = this->currentTime();

    bsls::TimeInterval connectLatency;
    if (now > d_connectStartTime) {
        connectLatency = now - d_connectStartTime;
    }

    d_connectContext.setError(ntsa::Error());
    d_connectContext.setAttemptsRemaining(0);
    d_connectContext.setLatency(connectLatency);

    if (d_connectContext.name().isNull()) {
        if (!d_connectName.empty()) {
            d_connectContext.setName(d_connectName);
        }
    }

    if (d_connectContext.endpoint().isUndefined()) {
        if (!d_connectEndpoint.isUndefined()) {
            d_connectContext.setEndpoint(d_connectEndpoint);
        }
    }

    d_connectOptions.setRetryCount(0);
    d_connectInProgress = false;

    d_openState.set(ntcs::OpenState::e_CONNECTED);

    ntci::ConnectCallback connectCallback = d_connectCallback;
    d_connectCallback.reset();

    ntca::ConnectContext connectContext = d_connectContext;
    d_connectContext.reset();

    ntca::ConnectEvent connectEvent;
    connectEvent.setType(ntca::ConnectEventType::e_COMPLETE);
    connectEvent.setContext(connectContext);

    if (d_connectDeadlineTimer_sp) {
        d_connectDeadlineTimer_sp->close();
        d_connectDeadlineTimer_sp.reset();
    }

    if (d_connectRetryTimer_sp) {
        d_connectRetryTimer_sp->close();
        d_connectRetryTimer_sp.reset();
    }

    NTCI_LOG_DEBUG("Connection attempt succeeded");

    if (connectCallback) {
        connectCallback.dispatch(self,
                                 connectEvent,
                                 d_proactorStrand_sp,
                                 self,
                                 false,
                                 &d_mutex);
    }

    if (d_openState.value() != ntcs::OpenState::e_CONNECTED) {
        return;
    }

    ntcs::Dispatch::announceEstablished(d_manager_sp,
                                        self,
                                        d_managerStrand_sp,
                                        d_proactorStrand_sp,
                                        self,
                                        false,
                                        &d_mutex);
}

void StreamSocket::privateFailConnect(
    const bsl::shared_ptr<StreamSocket>& self,
    const ntsa::Error&                   error,
    bool                                 defer,
    bool                                 close)
{
    NTCI_LOG_CONTEXT();

    if (!d_connectInProgress) {
        return;
    }

    BSLS_ASSERT(d_detachState.get() != ntcs::DetachState::e_DETACH_INITIATED);

    if (close) {
        d_connectOptions.setRetryCount(0);
    }

    if (!d_connectContext.error() || close) {
        NTCI_LOG_DEBUG("Connection attempt has failed: %s",
                       error.text().c_str());

        NTCS_METRICS_UPDATE_CONNECT_FAILURE();

        d_connectContext.setError(error);
        d_connectContext.setAttemptsRemaining(
            d_connectOptions.retryCount().valueOr(bsl::size_t(0)));

        if (d_connectContext.name().isNull()) {
            if (!d_connectName.empty()) {
                d_connectContext.setName(d_connectName);
            }
        }

        if (d_connectContext.endpoint().isUndefined()) {
            if (!d_connectEndpoint.isUndefined()) {
                d_connectContext.setEndpoint(d_connectEndpoint);
            }
        }

        ntci::ConnectCallback connectCallback = d_connectCallback;
        ntca::ConnectContext  connectContext  = d_connectContext;

        ntca::ConnectEvent connectEvent;
        connectEvent.setType(ntca::ConnectEventType::e_ERROR);
        connectEvent.setContext(connectContext);

        if (d_connectOptions.retryCount().valueOr(bsl::size_t(0)) == 0) {
            d_openState.set(ntcs::OpenState::e_CLOSED);
            d_connectInProgress = false;

            d_connectCallback.reset();
            d_connectContext.reset();

            if (d_connectDeadlineTimer_sp) {
                d_connectDeadlineTimer_sp->close();
                d_connectDeadlineTimer_sp.reset();
            }

            if (d_connectRetryTimer_sp) {
                d_connectRetryTimer_sp->close();
                d_connectRetryTimer_sp.reset();
            }

            d_flowControlState.close();
            d_shutdownState.close();

            if (d_systemHandle != ntsa::k_INVALID_HANDLE) {
                ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
                if (proactorRef) {
                    proactorRef->cancel(self);
                    const ntsa::Error error = proactorRef->detachSocket(self);
                    if (NTCCFG_LIKELY(!error)) {
                        d_detachState.set(
                            ntcs::DetachState::e_DETACH_INITIATED);
                        BSLS_ASSERT(!d_deferredCall);
                        d_deferredCall =
                            NTCCFG_BIND(&StreamSocket::privateFailConnectPart2,
                                        this,
                                        self,
                                        connectCallback,
                                        connectEvent,
                                        defer);
                    }
                }
            }

            ntcs::ObserverRef<ntci::ProactorPool> proactorPoolRef(
                &d_proactorPool);
            if (proactorPoolRef) {
                ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
                if (proactorRef) {
                    proactorPoolRef->releaseProactor(
                        proactorRef.getShared(),
                        d_options.loadBalancingOptions());
                }
            }
        }
        else {
            if (d_systemHandle != ntsa::k_INVALID_HANDLE) {
                ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
                if (proactorRef) {
                    proactorRef->cancel(self);
                    const ntsa::Error error = proactorRef->detachSocket(self);
                    if (NTCCFG_LIKELY(!error)) {
                        d_detachState.set(
                            ntcs::DetachState::e_DETACH_INITIATED);
                        d_deferredCall =
                            NTCCFG_BIND(&StreamSocket::privateFailConnectPart2,
                                        this,
                                        self,
                                        connectCallback,
                                        connectEvent,
                                        defer);
                    }
                }
            }

            d_openState.set(ntcs::OpenState::e_WAITING);
        }

        if (NTCCFG_UNLIKELY(d_detachState.get() !=
                            ntcs::DetachState::e_DETACH_INITIATED))
        {
            privateFailConnectPart2(self,
                                    connectCallback,
                                    connectEvent,
                                    defer);
        }
    }
    else {
        if (d_retryConnect) {
            d_retryConnect = false;
            this->privateRetryConnect(self);
        }
    }
}

void StreamSocket::privateFailConnectPart2(
    const bsl::shared_ptr<StreamSocket>& self,
    const ntci::ConnectCallback&         connectCallback,
    const ntca::ConnectEvent&            connectEvent,
    bool                                 defer)
{
    NTCI_LOG_CONTEXT();
    if (d_systemHandle != ntsa::k_INVALID_HANDLE) {
        if (d_socket_sp) {
            ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
            if (proactorRef) {
                proactorRef->releaseHandleReservation();
            }

            BSLS_ASSERT(d_socket_sp->handle() == d_publicHandle);
            BSLS_ASSERT(d_socket_sp->handle() == d_systemHandle);

            d_socket_sp->close();

            NTCI_LOG_TRACE("Stream socket closed descriptor %d",
                           (int)(d_publicHandle));

            d_publicHandle = ntsa::k_INVALID_HANDLE;
            d_systemHandle = ntsa::k_INVALID_HANDLE;
        }
    }

    if (connectCallback) {
        connectCallback.dispatch(self,
                                 connectEvent,
                                 d_proactorStrand_sp,
                                 self,
                                 defer,
                                 &d_mutex);
    }

    if (d_connectOptions.retryCount().valueOr(bsl::size_t(0)) == 0) {
        d_resolver.reset();

        d_sessionStrand_sp.reset();
        d_session_sp.reset();

        d_managerStrand_sp.reset();
        d_manager_sp.reset();
    }

    if (d_closeCallback) {
        d_closeCallback.dispatch(ntci::Strand::unknown(),
                                 self,
                                 true,
                                 &d_mutex);
        d_closeCallback.reset();
    }

    if (d_retryConnect) {
        d_retryConnect = false;
        this->privateRetryConnect(self);
    }

    this->moveAndExecute(&d_deferredCalls, ntci::Executor::Functor());
    d_deferredCalls.clear();
}

void StreamSocket::privateFailUpgrade(
    const bsl::shared_ptr<StreamSocket>& self,
    const ntsa::Error&                   error,
    const bsl::string&                   errorDescription)
{
    if (!d_upgradeInProgress) {
        return;
    }

    ntca::UpgradeContext upgradeContext;
    upgradeContext.setError(error);
    upgradeContext.setErrorDescription(errorDescription);

    d_encryption_sp.reset();
    d_upgradeInProgress = false;

    ntci::UpgradeCallback upgradeCallback = d_upgradeCallback;
    d_upgradeCallback.reset();

    ntca::UpgradeEvent upgradeEvent;
    upgradeEvent.setType(ntca::UpgradeEventType::e_ERROR);
    upgradeEvent.setContext(upgradeContext);

    if (d_upgradeTimer_sp) {
        d_upgradeTimer_sp->close();
        d_upgradeTimer_sp.reset();
    }

    this->privateApplyFlowControl(self,
                                  ntca::FlowControlType::e_BOTH,
                                  ntca::FlowControlMode::e_IMMEDIATE,
                                  false,
                                  true);

    d_flowControlState.close();

    if (upgradeCallback) {
        upgradeCallback.dispatch(self,
                                 upgradeEvent,
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

void StreamSocket::privateInitiateReceive(
    const bsl::shared_ptr<StreamSocket>& self)
{
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

    if (NTCCFG_UNLIKELY(d_receiveRateLimiter_sp)) {
        error = this->privateThrottleReceiveBuffer(self);
        if (error) {
            return;
        }
    }

    ntcs::BlobBufferUtil::reserveCapacity(d_receiveBlob_sp.get(),
                                          d_incomingBufferFactory_sp.get(),
                                          d_metrics_sp.get(),
                                          d_receiveQueue.lowWatermark(),
                                          d_receiveFeedback.current(),
                                          d_receiveFeedback.maximum());

    error =
        proactorRef->receive(self, d_receiveBlob_sp.get(), d_receiveOptions);
    if (error) {
        this->privateFailReceive(self, error);
        return;
    }

    d_receivePending = true;
}

void StreamSocket::privateCompleteReceive(
    const bsl::shared_ptr<StreamSocket>& self,
    bsl::size_t                          numBytesReceivable,
    bsl::size_t                          numBytesReceived)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (numBytesReceived == 0) {
        // Execute the shutdown of the receive buffer asynchronously to allow
        // pending writes to first complete, otherwise pending writes which
        // have had their data copied to the socket send buffer but not
        // announced to be complete by the proactor will be mistakenly
        // considered by be discarded.
        //
        // However, there is no current API to call something like
        // 'privateShutdownReceive' indicating the peer has initiated the
        // shutdown while not expected that function to be holding the lock.
        //
        // TODO: Resolve this issue.
        //
        // this->privateApplyFlowControl(self,
        //                               ntca::FlowControlType::e_RECEIVE,
        //                               ntca::FlowControlMode::e_IMMEDIATE,
        //                               true);
        //
        // this->execute(NTCCFG_BIND(
        //     &StreamSocket::privateShutdownReceive,
        //     self,
        //     self,
        //     ntsa::ShutdownOrigin::e_REMOTE,
        //     false));

        this->privateShutdownReceive(self,
                                     ntsa::ShutdownOrigin::e_REMOTE,
                                     false);
        return;
    }

    d_receiveFeedback.setFeedback(numBytesReceivable, numBytesReceived);

    if (NTCCFG_UNLIKELY(d_receiveRateLimiter_sp)) {
        d_receiveRateLimiter_sp->submit(numBytesReceived);
    }

    BSLS_ASSERT(
        NTCCFG_WARNING_PROMOTE(bsl::size_t, d_receiveBlob_sp->length()) ==
        numBytesReceived);

    if (NTCCFG_LIKELY(!d_encryption_sp)) {
        bdlbb::BlobUtil::append(d_receiveQueue.data().get(),
                                *d_receiveBlob_sp);

        bdlbb::BlobUtil::erase(d_receiveBlob_sp.get(),
                               0,
                               d_receiveBlob_sp->length());
    }
    else {
        const int receiveQueueLengthBefore = d_receiveQueue.data()->length();

        error = d_encryption_sp->pushIncomingCipherText(*d_receiveBlob_sp);
        if (NTCCFG_UNLIKELY(error)) {
            this->privateFailReceive(self, error);
            return;
        }

        bdlbb::BlobUtil::erase(d_receiveBlob_sp.get(),
                               0,
                               d_receiveBlob_sp->length());

        while (NTCCFG_LIKELY(d_encryption_sp->hasIncomingPlainText())) {
            error = d_encryption_sp->popIncomingPlainText(
                d_receiveQueue.data().get());
            if (NTCCFG_UNLIKELY(error)) {
                this->privateFailReceive(self, error);
                return;
            }
        }

        const int receiveQueueLengthAfter = d_receiveQueue.data()->length();

        numBytesReceived = receiveQueueLengthAfter - receiveQueueLengthBefore;
    }

    if (numBytesReceived > 0) {
        ntcq::ReceiveQueueEntry entry;
        entry.setLength(numBytesReceived);
        entry.setTimestamp(bsls::TimeUtil::getTimer());

        d_receiveQueue.pushEntry(entry);
    }

    NTCP_STREAMSOCKET_LOG_READ_QUEUE_FILLED(d_receiveQueue.size());

    NTCS_METRICS_UPDATE_READ_QUEUE_SIZE(d_receiveQueue.size());

    while (true) {
        bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> callbackEntry;
        error = d_receiveQueue.popCallbackEntry(&callbackEntry);
        if (error) {
            break;
        }

        BSLS_ASSERT(d_receiveQueue.hasEntry());
        BSLS_ASSERT(d_receiveQueue.size() ==
                    NTCCFG_WARNING_PROMOTE(bsl::size_t,
                                           d_receiveQueue.data()->length()));
        BSLS_ASSERT(d_receiveQueue.size() >=
                    callbackEntry->options().minSize());

        bsl::size_t numBytesRemaining = callbackEntry->options().maxSize();
        bsl::size_t numBytesDequeued  = 0;

        while (true) {
            ntcq::ReceiveQueueEntry& entry = d_receiveQueue.frontEntry();

            bsl::size_t numBytesToDequeue =
                bsl::min(numBytesRemaining, entry.length());

            numBytesDequeued += numBytesToDequeue;
            BSLS_ASSERT(numBytesDequeued <=
                        callbackEntry->options().maxSize());

            BSLS_ASSERT(numBytesRemaining >= numBytesToDequeue);
            numBytesRemaining -= numBytesToDequeue;

            if (numBytesToDequeue == entry.length()) {
                NTCS_METRICS_UPDATE_READ_QUEUE_DELAY(entry.delay());

                if (d_receiveQueue.popEntry()) {
                    break;
                }
            }
            else {
                d_receiveQueue.popSize(numBytesToDequeue);
                break;
            }

            if (numBytesRemaining == 0) {
                break;
            }
        }

        BSLS_ASSERT(numBytesDequeued >= callbackEntry->options().minSize());
        BSLS_ASSERT(numBytesDequeued <= callbackEntry->options().maxSize());

        bsl::shared_ptr<bdlbb::Blob> data =
            d_dataPool_sp->createIncomingBlob();

        ntcs::BlobUtil::append(data, d_receiveQueue.data(), numBytesDequeued);

        ntcs::BlobUtil::pop(d_receiveQueue.data(), numBytesDequeued);

        BSLS_ASSERT(d_receiveQueue.size() ==
                    NTCCFG_WARNING_PROMOTE(bsl::size_t,
                                           d_receiveQueue.data()->length()));

        NTCP_STREAMSOCKET_LOG_READ_QUEUE_DRAINED(d_receiveQueue.size());

        NTCS_METRICS_UPDATE_READ_QUEUE_SIZE(d_receiveQueue.size());

        ntca::ReceiveContext receiveContext;
        receiveContext.setTransport(d_transport);
        receiveContext.setEndpoint(d_remoteEndpoint);

        ntca::ReceiveEvent receiveEvent;
        receiveEvent.setType(ntca::ReceiveEventType::e_COMPLETE);
        receiveEvent.setContext(receiveContext);

        ntcq::ReceiveCallbackQueueEntry::dispatch(callbackEntry,
                                                  self,
                                                  data,
                                                  receiveEvent,
                                                  d_proactorStrand_sp,
                                                  self,
                                                  false,
                                                  &d_mutex);
    }

    if (d_receiveQueue.authorizeLowWatermarkEvent()) {
        NTCP_STREAMSOCKET_LOG_READ_QUEUE_LOW_WATERMARK(
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
        NTCP_STREAMSOCKET_LOG_READ_QUEUE_HIGH_WATERMARK(
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

    if (NTCCFG_UNLIKELY(d_encryption_sp)) {
        if (d_encryption_sp->isShutdownReceived()) {
            if (!d_encryption_sp->isShutdownSent()) {
                error = d_encryption_sp->shutdown();
                if (error) {
                    this->privateFailReceive(self, error);
                    return;
                }

                if (d_session_sp) {
                    ntca::DowngradeContext context;

                    ntca::DowngradeEvent event;
                    event.setType(ntca::DowngradeEventType::e_INITIATED);
                    event.setContext(context);

                    ntcs::Dispatch::announceDowngradeInitiated(
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

        if (NTCCFG_UNLIKELY(d_encryption_sp->hasOutgoingCipherText())) {
            bdlbb::Blob cipherData(d_outgoingBufferFactory_sp.get());

            while (NTCCFG_UNLIKELY(d_encryption_sp->hasOutgoingCipherText())) {
                error = d_encryption_sp->popOutgoingCipherText(&cipherData);
                if (error) {
                    this->privateFailReceive(self, error);
                    return;
                }
            }

            if (NTCCFG_UNLIKELY(cipherData.length() > 0)) {
                error = this->privateSendRaw(self,
                                             cipherData,
                                             ntca::SendOptions());
                if (error) {
                    this->privateFailReceive(self, error);
                    return;
                }
            }
        }

        if (d_encryption_sp->isShutdownFinished()) {
            d_encryption_sp.reset();

            if (d_session_sp) {
                ntca::DowngradeContext context;

                ntca::DowngradeEvent event;
                event.setType(ntca::DowngradeEventType::e_COMPLETE);
                event.setContext(context);

                ntcs::Dispatch::announceDowngradeComplete(d_session_sp,
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
}

void StreamSocket::privateFailReceive(
    const bsl::shared_ptr<StreamSocket>& self,
    const ntsa::Error&                   error)
{
    this->privateFail(self, error);
}

void StreamSocket::privateInitiateSend(
    const bsl::shared_ptr<StreamSocket>& self)
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

    if (NTCCFG_UNLIKELY(d_sendRateLimiter_sp)) {
        error = this->privateThrottleSendBuffer(self);
        if (error) {
            return;
        }
    }

    while (d_sendQueue.hasEntry()) {
        ntcq::SendQueueEntry& entry = d_sendQueue.frontEntry();

        if (NTCCFG_LIKELY(entry.data())) {
            const bool hasDeadline = !entry.deadline().isNull();

            ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
            if (!proactorRef) {
                this->privateFailSend(self,
                                      ntsa::Error(ntsa::Error::e_INVALID));
                break;
            }

#if defined(BSLS_PLATFORM_OS_LINUX)
            if (d_sendCount++ ==
                NTCP_STREAMSOCKET_SEND_BUFFER_REFRESH_INTERVAL)
            {
                if (entry.length() >=
                    NTCP_STREAMSOCKET_SEND_BUFFER_REFRESH_SIZE_THRESHOLD)
                {
                    bsl::size_t sendBufferSize;

                    ntsa::SocketOption option(d_allocator_p);
                    error = d_socket_sp->getOption(
                        &option,
                        ntsa::SocketOptionType::e_SEND_BUFFER_SIZE);
                    if (!error) {
                        sendBufferSize = option.sendBufferSize();
                    }
                    else {
                        sendBufferSize = 0;
                    }

                    if (sendBufferSize > 0) {
                        d_sendOptions.setMaxBytes(sendBufferSize * 2);
                    }
                }

                d_sendCount = 0;
            }
#endif

            error = proactorRef->send(self, *entry.data(), d_sendOptions);
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

void StreamSocket::privateCompleteSend(
    const bsl::shared_ptr<StreamSocket>& self,
    bsl::size_t                          numBytesSent)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (NTCCFG_UNLIKELY(d_sendRateLimiter_sp)) {
        d_sendRateLimiter_sp->submit(numBytesSent);
    }

    if (!d_sendQueue.hasEntry()) {
        return;
    }

    ntcq::SendQueueEntry& entry = d_sendQueue.frontEntry();

    ntci::SendCallback callback;

    if (numBytesSent == entry.length()) {
        NTCS_METRICS_UPDATE_WRITE_QUEUE_DELAY(entry.delay());
        callback = entry.callback();
        d_sendQueue.popEntry();
    }
    else {
        d_sendQueue.popSize(numBytesSent);
    }

    NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_DRAINED(d_sendQueue.size());
    NTCS_METRICS_UPDATE_WRITE_QUEUE_SIZE(d_sendQueue.size());

    if (callback) {
        ntca::SendContext sendContext;

        ntca::SendEvent sendEvent;
        sendEvent.setType(ntca::SendEventType::e_COMPLETE);
        sendEvent.setContext(sendContext);

        callback.dispatch(self,
                          sendEvent,
                          d_proactorStrand_sp,
                          self,
                          false,
                          &d_mutex);
    }

    if (d_sendQueue.authorizeLowWatermarkEvent()) {
        NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_LOW_WATERMARK(
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
                                                           true,
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

void StreamSocket::privateFailSend(const bsl::shared_ptr<StreamSocket>& self,
                                   const ntsa::Error&                   error)
{
    if (!d_sendQueue.hasEntry()) {
        return;
    }

    ntci::SendCallback callback = d_sendQueue.frontEntry().callback();

    d_sendQueue.popEntry();

    if (callback) {
        ntca::SendContext sendContext;
        sendContext.setError(error);

        ntca::SendEvent sendEvent;
        sendEvent.setType(ntca::SendEventType::e_ERROR);
        sendEvent.setContext(sendContext);

        callback.dispatch(self,
                          sendEvent,
                          d_proactorStrand_sp,
                          self,
                          false,
                          &d_mutex);
    }
}

void StreamSocket::privateFail(const bsl::shared_ptr<StreamSocket>& self,
                               const ntsa::Error&                   error)
{
    if (d_connectInProgress) {
        this->privateFailConnect(self, error, false, false);
        return;
    }

    if (d_upgradeInProgress) {
        this->privateFailUpgrade(self, error, "");
        return;
    }

    this->privateApplyFlowControl(self,
                                  ntca::FlowControlType::e_BOTH,
                                  ntca::FlowControlMode::e_IMMEDIATE,
                                  false,
                                  true);

    d_flowControlState.close();

    // An error can be detected on the socket when keep half open is false
    // and the peer initiates the downgrade of the TLS session immediately
    // followed by the shutdown of the TCP connection. In this case, locally,
    // the socket detects the socket is readable, reads the TLS shutdown, then
    // writes a TLS shutdown in response. Sometimes, that write can succeed,
    // but its transmission races with the reception of the TCP shutdown in
    // the operating system. When the operating system processes the TCP
    // shutdown first, some polling mechanisms, e.g. poll and epoll, will
    // detect POLLERR/EPOLLERR and the error on the socket error queue will
    // be EPIPE. The implementation automatically shuts down the socket and
    // announced it is closed in this case, so do not announce EPIPE as an
    // actual error, but instread simply treat it as an asynchronous detection
    // that the connection has been completely shut down.

    if (error == ntsa::Error::e_CONNECTION_DEAD ||
        error == ntsa::Error::e_CONNECTION_RESET)
    {
        this->privateShutdownReceive(self,
                                     ntsa::ShutdownOrigin::e_REMOTE,
                                     false);
        this->privateShutdownSend(self, false);
    }
    else {
        if (d_session_sp) {
            ntca::ErrorContext context;
            context.setError(error);

            ntca::ErrorEvent event;
            event.setType(ntca::ErrorEventType::e_TRANSPORT);
            event.setContext(context);

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
}

ntsa::Error StreamSocket::privateShutdown(
    const bsl::shared_ptr<StreamSocket>& self,
    ntsa::ShutdownType::Value            direction,
    ntsa::ShutdownMode::Value            mode,
    bool                                 defer)
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

void StreamSocket::privateShutdownSend(
    const bsl::shared_ptr<StreamSocket>& self,
    bool                                 defer)
{
    bool keepHalfOpen = NTCCFG_DEFAULT_STREAM_SOCKET_KEEP_HALF_OPEN;
    if (!d_options.keepHalfOpen().isNull()) {
        keepHalfOpen = d_options.keepHalfOpen().value();
    }

    if (d_options.abortiveClose()) {
        keepHalfOpen = false;
    }

    ntcs::ShutdownContext context;
    if (d_shutdownState.tryShutdownSend(&context, keepHalfOpen)) {
        this->privateShutdownSequence(self,
                                      ntsa::ShutdownOrigin::e_SOURCE,
                                      context,
                                      defer);
    }
}

void StreamSocket::privateShutdownReceive(
    const bsl::shared_ptr<StreamSocket>& self,
    ntsa::ShutdownOrigin::Value          origin,
    bool                                 defer)
{
    bool keepHalfOpen = NTCCFG_DEFAULT_STREAM_SOCKET_KEEP_HALF_OPEN;
    if (!d_options.keepHalfOpen().isNull()) {
        keepHalfOpen = d_options.keepHalfOpen().value();
    }

    if (d_options.abortiveClose()) {
        keepHalfOpen = false;
    }

    ntcs::ShutdownContext context;
    if (d_shutdownState.tryShutdownReceive(&context, keepHalfOpen, origin)) {
        this->privateShutdownSequence(self, origin, context, defer);
    }
}

void StreamSocket::privateShutdownSequence(
    const bsl::shared_ptr<StreamSocket>& self,
    ntsa::ShutdownOrigin::Value          origin,
    const ntcs::ShutdownContext&         context,
    bool                                 defer)
{
    NTCCFG_WARNING_UNUSED(origin);

    // Forcibly override the indication that the announcements should be
    // deferred on execute on the strand or asynchonrously on the reactor.
    // The announcements must always be deferred, otherwise, the user may
    // process the announcements out-of-order, say, when the shutdown is
    // initiated by the calling thread but completed by the proactor thread,
    // or when a write queue high watermark event has been deferred but the
    // reactor next learns that the socket has been shut down by the peer and
    // the socket is not kept half-open.
    //
    // TODO: Remove the 'defer' parameter and always defer the announcements.

    // MRM
#if 0
    bool keepHalfOpen = NTCCFG_DEFAULT_STREAM_SOCKET_KEEP_HALF_OPEN;
    if (!d_options.keepHalfOpen().isNull()) {
        keepHalfOpen = d_options.keepHalfOpen().value();
    }

    if (keepHalfOpen) {
        defer = true;
    }
#endif

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
            NTCCFG_BIND(&StreamSocket::privateShutdownSequencePart2,
                        this,
                        self,
                        context,
                        defer);
    }
}

void StreamSocket::privateShutdownSequencePart2(
    const bsl::shared_ptr<StreamSocket>& self,
    const ntcs::ShutdownContext&         context,
    bool                                 defer)
{
    NTCI_LOG_CONTEXT();
    // Second handle socket shutdown.

    if (context.shutdownSend()) {
        if (!d_options.abortiveClose()) {
            if (d_socket_sp) {
                d_socket_sp->shutdown(ntsa::ShutdownType::e_SEND);
            }
        }
    }

    if (context.shutdownReceive()) {
        if (!d_options.abortiveClose()) {
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

        NTCP_STREAMSOCKET_LOG_SHUTDOWN_SEND();

        bsl::vector<ntci::SendCallback> callbackVector;

        bool announceWriteQueueDiscarded = false;
        {
            if (d_sendRateTimer_sp) {
                d_sendRateTimer_sp->close();
                d_sendRateTimer_sp.reset();
            }

            announceWriteQueueDiscarded =
                d_sendQueue.removeAll(&callbackVector);
        }

        if (d_upgradeInProgress) {
            ntca::UpgradeContext upgradeContext;

            if (context.shutdownOrigin() == ntsa::ShutdownOrigin::e_SOURCE) {
                upgradeContext.setError(ntsa::Error(ntsa::Error::e_CANCELLED));
            }
            else {
                upgradeContext.setError(
                    ntsa::Error(ntsa::Error::e_CONNECTION_DEAD));
            }

            d_upgradeInProgress = false;
            d_encryption_sp.reset();

            ntci::UpgradeCallback upgradeCallback = d_upgradeCallback;
            d_upgradeCallback.reset();

            ntca::UpgradeEvent upgradeEvent;
            upgradeEvent.setType(ntca::UpgradeEventType::e_ERROR);
            upgradeEvent.setContext(upgradeContext);

            if (d_upgradeTimer_sp) {
                d_upgradeTimer_sp->close();
                d_upgradeTimer_sp.reset();
            }

            if (upgradeCallback) {
                upgradeCallback.dispatch(self,
                                         upgradeEvent,
                                         d_proactorStrand_sp,
                                         self,
                                         defer,
                                         &d_mutex);
            }
        }

        for (bsl::size_t i = 0; i < callbackVector.size(); ++i) {
            ntca::SendContext sendContext;
            sendContext.setError(ntsa::Error(ntsa::Error::e_CANCELLED));

            ntca::SendEvent sendEvent;
            sendEvent.setType(ntca::SendEventType::e_ERROR);
            sendEvent.setContext(sendContext);

            callbackVector[i].dispatch(self,
                                       sendEvent,
                                       d_proactorStrand_sp,
                                       self,
                                       defer,
                                       &d_mutex);
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

        NTCP_STREAMSOCKET_LOG_SHUTDOWN_RECEIVE();

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
            receiveContext.setEndpoint(d_remoteEndpoint);

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
            ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
            if (proactorRef) {
                proactorRef->releaseHandleReservation();
            }
            d_socket_sp->close();
        }

        d_systemHandle = ntsa::k_INVALID_HANDLE;

        NTCI_LOG_TRACE("Stream socket closed descriptor %d",
                       (int)(d_publicHandle));

        d_openState.set(ntcs::OpenState::e_CLOSED);

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

ntsa::Error StreamSocket::privateRelaxFlowControl(
    const bsl::shared_ptr<StreamSocket>& self,
    ntca::FlowControlType::Value         direction,
    bool                                 defer,
    bool                                 unlock)
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

ntsa::Error StreamSocket::privateApplyFlowControl(
    const bsl::shared_ptr<StreamSocket>& self,
    ntca::FlowControlType::Value         direction,
    ntca::FlowControlMode::Value         mode,
    bool                                 defer,
    bool                                 lock)
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

bool StreamSocket::privateCloseFlowControl(
    const bsl::shared_ptr<StreamSocket>& self,
    bool                                 defer)
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

ntsa::Error StreamSocket::privateThrottleSendBuffer(
    const bsl::shared_ptr<StreamSocket>& self)
{
    NTCI_LOG_CONTEXT();

    if (NTCCFG_UNLIKELY(d_sendRateLimiter_sp)) {
        bsls::TimeInterval now = this->currentTime();
        if (NTCCFG_UNLIKELY(d_sendRateLimiter_sp->wouldExceedBandwidth(now))) {
            bsls::TimeInterval timeToSubmit =
                d_sendRateLimiter_sp->calculateTimeToSubmit(now);

            NTCP_STREAMSOCKET_LOG_SEND_BUFFER_THROTTLE_APPLIED(timeToSubmit);

            this->privateApplyFlowControl(self,
                                          ntca::FlowControlType::e_SEND,
                                          ntca::FlowControlMode::e_IMMEDIATE,
                                          false,
                                          true);

            if (!d_shutdownState.canSend()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            if (NTCCFG_UNLIKELY(!d_sendRateTimer_sp)) {
                ntca::TimerOptions timerOptions;
                timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
                timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

                ntci::TimerCallback timerCallback = this->createTimerCallback(
                    bdlf::MemFnUtil::memFn(&StreamSocket::processSendRateTimer,
                                           self),
                    d_allocator_p);

                d_sendRateTimer_sp = this->createTimer(timerOptions,
                                                       timerCallback,
                                                       d_allocator_p);
            }

            bsls::TimeInterval nextSendAttemptTime = now + timeToSubmit;

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

ntsa::Error StreamSocket::privateThrottleReceiveBuffer(
    const bsl::shared_ptr<StreamSocket>& self)
{
    NTCI_LOG_CONTEXT();

    if (NTCCFG_UNLIKELY(d_receiveRateLimiter_sp)) {
        bsls::TimeInterval now = this->currentTime();
        if (NTCCFG_UNLIKELY(
                d_receiveRateLimiter_sp->wouldExceedBandwidth(now)))
        {
            bsls::TimeInterval timeToSubmit =
                d_receiveRateLimiter_sp->calculateTimeToSubmit(now);

            NTCP_STREAMSOCKET_LOG_RECEIVE_BUFFER_THROTTLE_APPLIED(
                timeToSubmit);

            this->privateApplyFlowControl(self,
                                          ntca::FlowControlType::e_RECEIVE,
                                          ntca::FlowControlMode::e_IMMEDIATE,
                                          false,
                                          true);

            if (!d_shutdownState.canReceive()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            if (NTCCFG_UNLIKELY(!d_receiveRateTimer_sp)) {
                ntca::TimerOptions timerOptions;
                timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
                timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

                ntci::TimerCallback timerCallback = this->createTimerCallback(
                    bdlf::MemFnUtil::memFn(
                        &StreamSocket::processReceiveRateTimer,
                        self),
                    d_allocator_p);

                d_receiveRateTimer_sp = this->createTimer(timerOptions,
                                                          timerCallback,
                                                          d_allocator_p);
            }

            bsls::TimeInterval nextReceiveAttemptTime = now + timeToSubmit;

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

ntsa::Error StreamSocket::privateSendRaw(
    const bsl::shared_ptr<StreamSocket>& self,
    const bdlbb::Blob&                   data,
    const ntca::SendOptions&             options)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<ntsa::Data> dataContainer =
        d_dataPool_sp->createOutgoingData();

    dataContainer->makeBlob(data);

    ntcq::SendQueueEntry entry;
    entry.setId(d_sendQueue.generateEntryId());
    entry.setToken(options.token());
    entry.setData(dataContainer);
    entry.setLength(dataContainer->blob().length());
    entry.setTimestamp(bsls::TimeUtil::getTimer());

    if (NTCCFG_UNLIKELY(!options.deadline().isNull())) {
        ntca::TimerOptions timerOptions;
        timerOptions.setOneShot(true);
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
        timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

        ntci::TimerCallback timerCallback = this->createTimerCallback(
            bdlf::BindUtil::bind(&StreamSocket::processSendDeadlineTimer,
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

    NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_FILLED(d_sendQueue.size());

    NTCS_METRICS_UPDATE_WRITE_QUEUE_SIZE(d_sendQueue.size());

    if (becameNonEmpty) {
        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_SEND,
                                      true,
                                      false);
    }

    return ntsa::Error();
}

ntsa::Error StreamSocket::privateSendRaw(
    const bsl::shared_ptr<StreamSocket>& self,
    const ntsa::Data&                    data,
    const ntca::SendOptions&             options)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<ntsa::Data> dataContainer =
        d_dataPool_sp->createOutgoingData();

    *dataContainer = data;

    BSLS_ASSERT(dataContainer->size() != 0);

    ntcq::SendQueueEntry entry;
    entry.setId(d_sendQueue.generateEntryId());
    entry.setToken(options.token());
    entry.setData(dataContainer);
    entry.setLength(dataContainer->size());
    entry.setTimestamp(bsls::TimeUtil::getTimer());

    if (NTCCFG_UNLIKELY(!options.deadline().isNull())) {
        ntca::TimerOptions timerOptions;
        timerOptions.setOneShot(true);
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
        timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

        ntci::TimerCallback timerCallback = this->createTimerCallback(
            bdlf::BindUtil::bind(&StreamSocket::processSendDeadlineTimer,
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

    NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_FILLED(d_sendQueue.size());

    NTCS_METRICS_UPDATE_WRITE_QUEUE_SIZE(d_sendQueue.size());

    if (becameNonEmpty) {
        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_SEND,
                                      true,
                                      false);
    }

    return ntsa::Error();
}

ntsa::Error StreamSocket::privateSendRaw(
    const bsl::shared_ptr<StreamSocket>& self,
    const bdlbb::Blob&                   data,
    const ntca::SendOptions&             options,
    const ntci::SendCallback&            callback)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<ntsa::Data> dataContainer =
        d_dataPool_sp->createOutgoingData();

    dataContainer->makeBlob(data);

    ntcq::SendQueueEntry entry;
    entry.setId(d_sendQueue.generateEntryId());
    entry.setToken(options.token());
    entry.setData(dataContainer);
    entry.setLength(dataContainer->blob().length());
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
            bdlf::BindUtil::bind(&StreamSocket::processSendDeadlineTimer,
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

    NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_FILLED(d_sendQueue.size());

    NTCS_METRICS_UPDATE_WRITE_QUEUE_SIZE(d_sendQueue.size());

    if (becameNonEmpty) {
        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_SEND,
                                      true,
                                      false);
    }

    return ntsa::Error();
}

ntsa::Error StreamSocket::privateSendRaw(
    const bsl::shared_ptr<StreamSocket>& self,
    const ntsa::Data&                    data,
    const ntca::SendOptions&             options,
    const ntci::SendCallback&            callback)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<ntsa::Data> dataContainer =
        d_dataPool_sp->createOutgoingData();

    *dataContainer = data;

    ntcq::SendQueueEntry entry;
    entry.setId(d_sendQueue.generateEntryId());
    entry.setToken(options.token());
    entry.setData(dataContainer);
    entry.setLength(dataContainer->size());
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
            bdlf::BindUtil::bind(&StreamSocket::processSendDeadlineTimer,
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

    NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_FILLED(d_sendQueue.size());

    NTCS_METRICS_UPDATE_WRITE_QUEUE_SIZE(d_sendQueue.size());

    if (becameNonEmpty) {
        this->privateRelaxFlowControl(self,
                                      ntca::FlowControlType::e_SEND,
                                      true,
                                      false);
    }

    return ntsa::Error();
}

bool StreamSocket::isStream() const
{
    return true;
}

ntsa::Error StreamSocket::privateOpen(
    const bsl::shared_ptr<StreamSocket>& self)
{
    if (d_systemHandle != ntsa::k_INVALID_HANDLE) {
        return ntsa::Error();
    }

    return this->privateOpen(self, d_options.transport());
}

ntsa::Error StreamSocket::privateOpen(
    const bsl::shared_ptr<StreamSocket>& self,
    ntsa::Transport::Value               transport)
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

    error = ntcu::StreamSocketUtil::validateTransport(transport);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntsi::StreamSocket> streamSocket;
    if (d_socket_sp) {
        streamSocket = d_socket_sp;
    }
    else {
        streamSocket = ntsf::System::createStreamSocket(d_allocator_p);
    }

    error = this->privateOpen(self, transport, streamSocket);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error StreamSocket::privateOpen(
    const bsl::shared_ptr<StreamSocket>& self,
    const ntsa::Endpoint&                endpoint)
{
    return this->privateOpen(
        self,
        endpoint.transport(ntsa::TransportMode::e_STREAM));
}

ntsa::Error StreamSocket::privateOpen(
    const bsl::shared_ptr<StreamSocket>& self,
    ntsa::Transport::Value               transport,
    ntsa::Handle                         handle)
{
    return this->privateOpen(self,
                             transport,
                             handle,
                             bsl::shared_ptr<ntci::ListenerSocket>());
}

ntsa::Error StreamSocket::privateOpen(
    const bsl::shared_ptr<StreamSocket>&       self,
    ntsa::Transport::Value                     transport,
    const bsl::shared_ptr<ntsi::StreamSocket>& streamSocket)
{
    return this->privateOpen(self,
                             transport,
                             streamSocket,
                             bsl::shared_ptr<ntci::ListenerSocket>());
}

ntsa::Error StreamSocket::privateOpen(
    const bsl::shared_ptr<StreamSocket>&         self,
    ntsa::Transport::Value                       transport,
    ntsa::Handle                                 handle,
    const bsl::shared_ptr<ntci::ListenerSocket>& acceptor)
{
    ntsa::Error error;

    bsl::shared_ptr<ntsi::StreamSocket> streamSocket;
    if (d_socket_sp) {
        streamSocket = d_socket_sp;
        error        = streamSocket->acquire(handle);
        if (error) {
            return error;
        }
    }
    else {
        streamSocket = ntsf::System::createStreamSocket(handle, d_allocator_p);
    }

    error = this->privateOpen(self, transport, streamSocket, acceptor);
    if (error) {
        streamSocket->release();
        return error;
    }

    return ntsa::Error();
}

ntsa::Error StreamSocket::privateOpen(
    const bsl::shared_ptr<StreamSocket>&         self,
    ntsa::Transport::Value                       transport,
    const bsl::shared_ptr<ntsi::StreamSocket>&   streamSocket,
    const bsl::shared_ptr<ntci::ListenerSocket>& acceptor)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (d_systemHandle != ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = ntcu::StreamSocketUtil::validateTransport(transport);
    if (error) {
        return error;
    }

    if (streamSocket->handle() == ntsa::k_INVALID_HANDLE) {
        error = streamSocket->open(transport);
        if (error) {
            return error;
        }
    }

    ntsa::Handle handle = streamSocket->handle();

    error = ntcs::Compat::configure(streamSocket, d_options);
    if (error) {
        return error;
    }

    error = streamSocket->setBlocking(false);
    if (error) {
        return error;
    }

    if (!d_options.sourceEndpoint().isNull()) {
        error = streamSocket->bind(d_options.sourceEndpoint().value(),
                                   d_options.reuseAddress());
        if (error) {
            return error;
        }
    }

    ntsa::Endpoint sourceEndpoint;
    error = streamSocket->sourceEndpoint(&sourceEndpoint);
    if (error) {
        sourceEndpoint.reset();
    }

    ntsa::Endpoint remoteEndpoint;
    error = streamSocket->remoteEndpoint(&remoteEndpoint);
    if (error) {
        remoteEndpoint.reset();
    }

    {
        bsl::size_t sendBufferSize;

        ntsa::SocketOption option(d_allocator_p);
        error = streamSocket->getOption(
            &option,
            ntsa::SocketOptionType::e_SEND_BUFFER_SIZE);
        if (!error) {
            sendBufferSize = option.sendBufferSize();
        }
        else {
            sendBufferSize = 0;
        }

        if (sendBufferSize > 0) {
            d_sendOptions.setMaxBytes(sendBufferSize * 2);
        }
    }

    {
        bsl::size_t receiveBufferSize;

        ntsa::SocketOption option(d_allocator_p);
        error = streamSocket->getOption(
            &option,
            ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE);
        if (!error) {
            receiveBufferSize = option.receiveBufferSize();
        }
        else {
            receiveBufferSize = 0;
        }

        if (receiveBufferSize > 0) {
            d_receiveOptions.setMaxBytes(receiveBufferSize);
        }
    }

    d_sendOptions.setMaxBuffers(streamSocket->maxBuffersPerSend());
    d_receiveOptions.setMaxBuffers(streamSocket->maxBuffersPerReceive());

    ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
    if (!proactorRef) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!proactorRef->acquireHandleReservation()) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    d_systemHandle   = handle;
    d_publicHandle   = handle;
    d_transport      = transport;
    d_sourceEndpoint = sourceEndpoint;
    d_remoteEndpoint = remoteEndpoint;
    d_socket_sp      = streamSocket;
    d_acceptor_sp    = acceptor;

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    NTCI_LOG_TRACE("Stream socket opened descriptor %d",
                   (int)(d_publicHandle));

    proactorRef->attachSocket(self);

    if (!d_remoteEndpoint.isUndefined()) {
        d_openState.set(ntcs::OpenState::e_CONNECTED);

        ntcs::Dispatch::announceEstablished(d_manager_sp,
                                            self,
                                            d_managerStrand_sp,
                                            d_proactorStrand_sp,
                                            self,
                                            true,
                                            &d_mutex);
    }

    return ntsa::Error();
}

void StreamSocket::processSourceEndpointResolution(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const ntsa::Endpoint&                  endpoint,
    const ntca::GetEndpointEvent&          getEndpointEvent,
    const ntca::BindOptions&               bindOptions,
    const ntci::BindCallback&              bindCallback)
{
    NTCCFG_WARNING_UNUSED(resolver);

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

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

void StreamSocket::processRemoteEndpointResolution(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const ntsa::Endpoint&                  endpoint,
    const ntca::GetEndpointEvent&          getEndpointEvent,
    bsl::size_t                            connectAttempts)
{
    NTCCFG_WARNING_UNUSED(resolver);

    NTCI_LOG_CONTEXT();

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (NTCCFG_UNLIKELY(d_detachState.get() ==
                        ntcs::DetachState::e_DETACH_INITIATED))
    {
        return;
    }

    ntsa::Error error;

    if (!d_connectInProgress) {
        NTCI_LOG_STREAM_TRACE
            << "Stream socket socket ignored remote endpoint resolution "
            << getEndpointEvent << " for connection attempt "
            << connectAttempts
            << " because a connection is no longer in progress"
            << NTCI_LOG_STREAM_END;
        return;
    }

    if (connectAttempts != d_connectAttempts) {
        NTCI_LOG_STREAM_TRACE
            << "Stream socket socket ignored remote endpoint resolution "
            << getEndpointEvent << " for connection attempt "
            << connectAttempts << " because connection attempt "
            << d_connectAttempts << " is now active" << NTCI_LOG_STREAM_END;
        return;
    }

    if (getEndpointEvent.type() == ntca::GetEndpointEventType::e_ERROR) {
        error = getEndpointEvent.context().error();
    }
    else {
        d_connectContext.setName(getEndpointEvent.context().authority());
        d_connectContext.setEndpoint(endpoint);

        if (getEndpointEvent.context().latency() != bsls::TimeInterval()) {
            d_connectContext.setLatency(getEndpointEvent.context().latency());
        }

        if (!getEndpointEvent.context().nameServer().isNull()) {
            d_connectContext.setNameServer(
                getEndpointEvent.context().nameServer().value());
        }

        if (getEndpointEvent.context().source() !=
            ntca::ResolverSource::e_UNKNOWN)
        {
            d_connectContext.setSource(getEndpointEvent.context().source());
        }
    }

    if (!error) {
        error = this->privateOpen(self, endpoint);
    }

    if (!error) {
        if (d_transport == ntsa::Transport::e_LOCAL_STREAM) {
            if (d_sourceEndpoint.isImplicit()) {
                error = d_socket_sp->bindAny(d_transport,
                                             d_options.reuseAddress());

                if (!error) {
                    error = d_socket_sp->sourceEndpoint(&d_sourceEndpoint);
                }
            }
        }
    }

    if (!error) {
        ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
        if (!proactorRef) {
            error = ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (!error) {
            error = proactorRef->connect(self, endpoint);
        }
    }

    if (!error) {
        error = d_socket_sp->sourceEndpoint(&d_sourceEndpoint);
    }

    if (error) {
        this->privateFailConnect(self, error, false, false);
    }
}

ntsa::Error StreamSocket::privateUpgrade(
    const bsl::shared_ptr<StreamSocket>& self,
    const ntca::UpgradeOptions&          upgradeOptions)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    // Announce an upgrade to encrypted communication is starting.

    NTCP_STREAMSOCKET_LOG_ENCRYPTION_UPGRADE_STARTING();

    // Initiate the encryption handshake.

    ntci::Encryption::HandshakeCallback handshakeCallback =
        bdlf::MemFnUtil::memFn(&StreamSocket::privateEncryptionHandshake,
                               this);

    error =
        d_encryption_sp->initiateHandshake(upgradeOptions, handshakeCallback);
    if (error) {
        return error;
    }

    // Push any unconsumed receive queue data that must be now considered
    // cipher text into the encryption session.

    if (d_receiveQueue.hasEntry()) {
        // Push the receive queue data into the encryption session as incoming
        // cipher text.

        error =
            d_encryption_sp->pushIncomingCipherText(*d_receiveQueue.data());
        if (error) {
            return error;
        }

        bdlbb::BlobUtil::erase(d_receiveQueue.data().get(),
                               0,
                               d_receiveQueue.data()->length());

        // Pop incoming plain text back into the receive queue data.

        while (d_encryption_sp->hasIncomingPlainText()) {
            error = d_encryption_sp->popIncomingPlainText(
                d_receiveQueue.data().get());
            if (error) {
                return error;
            }
        }

        // Remember the timestamp of the earliest entry on the receive queue.

        bsl::int64_t timestamp = d_receiveQueue.frontEntry().timestamp();

        // Pop all unconsumed entries from the receive queue.

        while (true) {
            if (d_receiveQueue.popEntry()) {
                break;
            }
        }

        // Reform an artificial receive queue entry from the incoming plain
        // text popped from the encryption session.

        ntcq::ReceiveQueueEntry entry;
        entry.setLength(d_receiveQueue.data()->length());
        entry.setTimestamp(timestamp);

        d_receiveQueue.pushEntry(entry);

        BSLS_ASSERT(d_receiveQueue.size() ==
                    NTCCFG_WARNING_PROMOTE(bsl::size_t,
                                           d_receiveQueue.data()->length()));

        // MRM: Announce watermarks if neccessary?
    }

    // Pop any outgoing cipher text generated as a result of initiating the
    // handshake.

    bdlbb::Blob cipherData(d_outgoingBufferFactory_sp.get());

    while (d_encryption_sp->hasOutgoingCipherText()) {
        error = d_encryption_sp->popOutgoingCipherText(&cipherData);
        if (error) {
            return error;
        }
    }

    // Send the outgoing cipher text, if any.

    if (cipherData.length() > 0) {
        error = this->privateSendRaw(self, cipherData, ntca::SendOptions());
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

void StreamSocket::privateRetryConnect(
    const bsl::shared_ptr<StreamSocket>& self)
{
    ntsa::Error error;

    if (d_openState.value() != ntcs::OpenState::e_WAITING) {
        return;
    }

    if (!d_connectInProgress) {
        return;
    }

    if (d_connectOptions.retryCount().isNull() ||
        d_connectOptions.retryCount().value() == 0)
    {
        return;
    }

    if (d_openState.isNotEither(ntcs::OpenState::e_DEFAULT,
                                ntcs::OpenState::e_WAITING))
    {
        return;
    }

    d_sourceEndpoint.reset();
    d_remoteEndpoint.reset();

    d_flowControlState.reset();
    d_shutdownState.reset();

    d_connectContext.reset();

    d_openState.set(ntcs::OpenState::e_CONNECTING);
    ++d_connectAttempts;

    d_connectOptions.setRetryCount(d_connectOptions.retryCount().value() - 1);

    if (!d_connectEndpoint.isUndefined()) {
        error = this->privateRetryConnectToEndpoint(self);
    }
    else {
        error = this->privateRetryConnectToName();
    }

    if (error) {
        this->privateFailConnect(self, error, false, false);
    }
}

ntsa::Error StreamSocket::privateRetryConnectToName()
{
    struct WeakBinder {
        static void invoke(const bsl::weak_ptr<StreamSocket>&     socket,
                           const bsl::shared_ptr<ntci::Resolver>& resolver,
                           const ntsa::Endpoint&                  endpoint,
                           const ntca::GetEndpointEvent& getEndpointEvent,
                           bsl::size_t                   connectAttempts)
        {
            const bsl::shared_ptr<StreamSocket> strongRef = socket.lock();
            if (strongRef) {
                strongRef->processRemoteEndpointResolution(resolver,
                                                           endpoint,
                                                           getEndpointEvent,
                                                           connectAttempts);
            }
        }
    };

    ntsa::Error error;

    ntcs::ObserverRef<ntci::Resolver> resolverRef(&d_resolver);
    if (!resolverRef) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntca::GetEndpointOptions getEndpointOptions;
    ntcs::Compat::convert(&getEndpointOptions, d_connectOptions);

    ntci::GetEndpointCallback getEndpointCallback =
        resolverRef->createGetEndpointCallback(
            NTCCFG_BIND(&WeakBinder::invoke,
                        this->weak_from_this(),
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        d_connectAttempts),
            d_proactorStrand_sp,
            d_allocator_p);

    error = resolverRef->getEndpoint(d_connectName,
                                     getEndpointOptions,
                                     getEndpointCallback);

    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error StreamSocket::privateRetryConnectToEndpoint(
    const bsl::shared_ptr<StreamSocket>& self)
{
    ntsa::Error error;

    error = this->privateOpen(self, d_connectEndpoint);
    if (error) {
        return error;
    }

    if (d_transport == ntsa::Transport::e_LOCAL_STREAM) {
        if (d_sourceEndpoint.isImplicit()) {
            error =
                d_socket_sp->bindAny(d_transport, d_options.reuseAddress());
            if (error) {
                return error;
            }

            error = d_socket_sp->sourceEndpoint(&d_sourceEndpoint);
            if (error) {
                return error;
            }
        }
    }

    ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
    if (!proactorRef) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = proactorRef->connect(self, d_connectEndpoint);
    if (error) {
        return error;
    }

    error = d_socket_sp->sourceEndpoint(&d_sourceEndpoint);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

StreamSocket::StreamSocket(
    const ntca::StreamSocketOptions&           options,
    const bsl::shared_ptr<ntci::Resolver>&     resolver,
    const bsl::shared_ptr<ntci::Proactor>&     proactor,
    const bsl::shared_ptr<ntci::ProactorPool>& proactorPool,
    const bsl::shared_ptr<ntcs::Metrics>&      metrics,
    bslma::Allocator*                          basicAllocator)
: d_object("ntcp::StreamSocket")
, d_mutex()
, d_systemHandle(ntsa::k_INVALID_HANDLE)
, d_publicHandle(ntsa::k_INVALID_HANDLE)
, d_transport(ntsa::Transport::e_UNDEFINED)
, d_sourceEndpoint()
, d_remoteEndpoint()
, d_socket_sp()
, d_acceptor_sp()
, d_encryption_sp()
#if NTCP_STREAMSOCKET_OBSERVE_BY_WEAK_PTR
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
, d_openState()
, d_flowControlState()
, d_shutdownState()
, d_sendOptions()
, d_sendQueue(basicAllocator)
, d_sendRateLimiter_sp()
, d_sendRateTimer_sp()
, d_sendPending(false)
, d_sendGreedily(NTCCFG_DEFAULT_STREAM_SOCKET_WRITE_GREEDILY)
, d_sendCount(0)
, d_receiveOptions()
, d_receiveQueue(basicAllocator)
, d_receiveFeedback()
, d_receiveRateLimiter_sp()
, d_receiveRateTimer_sp()
, d_receivePending(false)
, d_receiveGreedily(NTCCFG_DEFAULT_STREAM_SOCKET_READ_GREEDILY)
, d_receiveCount(0)
, d_receiveBlob_sp()
, d_connectEndpoint()
, d_connectName(basicAllocator)
, d_connectStartTime()
, d_connectAttempts(0)
, d_connectOptions()
, d_connectContext(basicAllocator)
, d_connectCallback(basicAllocator)
, d_connectDeadlineTimer_sp()
, d_connectRetryTimer_sp()
, d_connectInProgress(false)
, d_upgradeCallback(basicAllocator)
, d_upgradeTimer_sp()
, d_upgradeInProgress(false)
, d_options(options)
, d_retryConnect(false)
, d_detachState(ntcs::DetachState::e_DETACH_IDLE)
, d_deferredCall()
, d_closeCallback(bslma::Default::allocator(basicAllocator))
, d_deferredCalls(bslma::Default::allocator(basicAllocator))
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_sendQueue.setData(d_dataPool_sp->createOutgoingBlob());
    d_receiveQueue.setData(d_dataPool_sp->createIncomingBlob());
    d_receiveBlob_sp = d_dataPool_sp->createIncomingBlob();

    d_receiveOptions.hideEndpoint();

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

    if (!d_options.readQueueLowWatermark().isNull()) {
        d_receiveQueue.setLowWatermark(
            d_options.readQueueLowWatermark().value());
    }

    if (!d_options.readQueueHighWatermark().isNull()) {
        d_receiveQueue.setHighWatermark(
            d_options.readQueueHighWatermark().value());
    }

    if (!d_options.minIncomingStreamTransferSize().isNull()) {
        d_receiveFeedback.setMinimum(
            d_options.minIncomingStreamTransferSize().value());
    }

    if (!d_options.maxIncomingStreamTransferSize().isNull()) {
        d_receiveFeedback.setMaximum(
            d_options.maxIncomingStreamTransferSize().value());
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

        ntcm::MonitorableUtil::registerMonitorable(d_metrics_sp);
    }
    else {
        d_metrics_sp = metrics;
    }
}

StreamSocket::~StreamSocket()
{
    if (!d_options.metrics().isNull() && d_options.metrics().value()) {
        if (d_metrics_sp) {
            ntcm::MonitorableUtil::deregisterMonitorable(d_metrics_sp);
        }
    }
}

ntsa::Error StreamSocket::open()
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    return this->privateOpen(self);
}

ntsa::Error StreamSocket::open(ntsa::Transport::Value transport)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    return this->privateOpen(self, transport);
}

ntsa::Error StreamSocket::open(ntsa::Transport::Value transport,
                               ntsa::Handle           handle)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    return this->privateOpen(self, transport, handle);
}

ntsa::Error StreamSocket::open(
    ntsa::Transport::Value                     transport,
    const bsl::shared_ptr<ntsi::StreamSocket>& streamSocket)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    return this->privateOpen(self, transport, streamSocket);
}

ntsa::Error StreamSocket::open(
    ntsa::Transport::Value                       transport,
    ntsa::Handle                                 handle,
    const bsl::shared_ptr<ntci::ListenerSocket>& acceptor)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    return this->privateOpen(self, transport, handle, acceptor);
}

ntsa::Error StreamSocket::open(
    ntsa::Transport::Value                       transport,
    const bsl::shared_ptr<ntsi::StreamSocket>&   streamSocket,
    const bsl::shared_ptr<ntci::ListenerSocket>& acceptor)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    return this->privateOpen(self, transport, streamSocket, acceptor);
}

ntsa::Error StreamSocket::bind(const ntsa::Endpoint&     endpoint,
                               const ntca::BindOptions&  options,
                               const ntci::BindFunction& callback)
{
    return this->bind(endpoint,
                      options,
                      this->createBindCallback(callback, d_allocator_p));
}

ntsa::Error StreamSocket::bind(const ntsa::Endpoint&     endpoint,
                               const ntca::BindOptions&  options,
                               const ntci::BindCallback& callback)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_openState.canBind()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

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

ntsa::Error StreamSocket::bind(const bsl::string&        name,
                               const ntca::BindOptions&  options,
                               const ntci::BindFunction& callback)
{
    return this->bind(name,
                      options,
                      this->createBindCallback(callback, d_allocator_p));
}

ntsa::Error StreamSocket::bind(const bsl::string&        name,
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

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_openState.canBind()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntcs::ObserverRef<ntci::Resolver> resolverRef(&d_resolver);
    if (!resolverRef) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntca::GetEndpointOptions getEndpointOptions;
    ntcs::Compat::convert(&getEndpointOptions, options);

    ntci::GetEndpointCallback getEndpointCallback =
        resolverRef->createGetEndpointCallback(
            NTCCFG_BIND(&StreamSocket::processSourceEndpointResolution,
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

ntsa::Error StreamSocket::connect(const ntsa::Endpoint&        endpoint,
                                  const ntca::ConnectOptions&  options,
                                  const ntci::ConnectFunction& callback)
{
    return this->connect(endpoint,
                         options,
                         this->createConnectCallback(callback, d_allocator_p));
}

ntsa::Error StreamSocket::connect(const ntsa::Endpoint&        endpoint,
                                  const ntca::ConnectOptions&  options,
                                  const ntci::ConnectCallback& callback)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_openState.canConnect()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_connectInProgress) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!d_remoteEndpoint.isUndefined()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

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

    d_connectEndpoint   = endpoint;
    d_connectOptions    = options;
    d_connectCallback   = callback;
    d_connectInProgress = true;

    d_openState.set(ntcs::OpenState::e_WAITING);

    if (d_connectOptions.retryCount().isNull()) {
        d_connectOptions.setRetryCount(1);
    }
    else {
        d_connectOptions.setRetryCount(d_connectOptions.retryCount().value() +
                                       1);
    }

    if (d_connectOptions.retryCount().value() > 1) {
        if (d_connectOptions.retryInterval().isNull()) {
            d_connectOptions.setRetryInterval(bsls::TimeInterval(0));
        }
    }

    d_connectStartTime = this->currentTime();

    if (!d_connectOptions.deadline().isNull()) {
        ntca::TimerOptions timerOptions;
        timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
        timerOptions.setOneShot(true);

        ntci::TimerCallback timerCallback = this->createTimerCallback(
            bdlf::MemFnUtil::memFn(&StreamSocket::processConnectDeadlineTimer,
                                   self),
            d_allocator_p);

        d_connectDeadlineTimer_sp =
            this->createTimer(timerOptions, timerCallback, d_allocator_p);

        d_connectDeadlineTimer_sp->schedule(
            d_connectOptions.deadline().value());
    }

    if (d_connectOptions.retryCount().value() == 1) {
        ntca::TimerOptions timerOptions;
        timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
        timerOptions.setOneShot(true);

        ntci::TimerCallback timerCallback = this->createTimerCallback(
            bdlf::MemFnUtil::memFn(&StreamSocket::processConnectRetryTimer,
                                   self),
            d_allocator_p);

        d_connectRetryTimer_sp =
            this->createTimer(timerOptions, timerCallback, d_allocator_p);

        d_connectRetryTimer_sp->schedule(this->currentTime());
    }
    else {
        ntca::TimerOptions timerOptions;
        timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
        timerOptions.setOneShot(false);

        ntci::TimerCallback timerCallback = this->createTimerCallback(
            bdlf::MemFnUtil::memFn(&StreamSocket::processConnectRetryTimer,
                                   self),
            d_allocator_p);

        d_connectRetryTimer_sp =
            this->createTimer(timerOptions, timerCallback, d_allocator_p);

        d_connectRetryTimer_sp->schedule(
            this->currentTime(),
            d_connectOptions.retryInterval().value());
    }

    return ntsa::Error();
}

ntsa::Error StreamSocket::connect(const bsl::string&           name,
                                  const ntca::ConnectOptions&  options,
                                  const ntci::ConnectFunction& callback)
{
    return this->connect(name,
                         options,
                         this->createConnectCallback(callback, d_allocator_p));
}

ntsa::Error StreamSocket::connect(const bsl::string&           name,
                                  const ntca::ConnectOptions&  options,
                                  const ntci::ConnectCallback& callback)
{
    if (d_options.transport() == ntsa::Transport::e_LOCAL_STREAM ||
        bdls::PathUtil::isAbsolute(name))
    {
        ntsa::LocalName localName;
        localName.setValue(name);
        return this->connect(ntsa::Endpoint(localName), options, callback);
    }

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_openState.canConnect()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_connectInProgress) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!d_remoteEndpoint.isUndefined()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntcs::ObserverRef<ntci::Resolver> resolverRef(&d_resolver);
    if (!resolverRef) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_connectName       = name;
    d_connectOptions    = options;
    d_connectCallback   = callback;
    d_connectInProgress = true;

    d_openState.set(ntcs::OpenState::e_WAITING);

    if (d_connectOptions.retryCount().isNull()) {
        d_connectOptions.setRetryCount(1);
    }
    else {
        d_connectOptions.setRetryCount(d_connectOptions.retryCount().value() +
                                       1);
    }

    if (d_connectOptions.retryCount().value() > 1) {
        if (d_connectOptions.retryInterval().isNull()) {
            d_connectOptions.setRetryInterval(bsls::TimeInterval(0));
        }
    }

    d_connectStartTime = this->currentTime();

    if (!d_connectOptions.deadline().isNull()) {
        ntca::TimerOptions timerOptions;
        timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
        timerOptions.setOneShot(true);

        ntci::TimerCallback timerCallback = this->createTimerCallback(
            bdlf::MemFnUtil::memFn(&StreamSocket::processConnectDeadlineTimer,
                                   self),
            d_allocator_p);

        d_connectDeadlineTimer_sp =
            this->createTimer(timerOptions, timerCallback, d_allocator_p);

        d_connectDeadlineTimer_sp->schedule(
            d_connectOptions.deadline().value());
    }

    if (d_connectOptions.retryCount().value() == 1) {
        ntca::TimerOptions timerOptions;
        timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
        timerOptions.setOneShot(true);

        ntci::TimerCallback timerCallback = this->createTimerCallback(
            bdlf::MemFnUtil::memFn(&StreamSocket::processConnectRetryTimer,
                                   self),
            d_allocator_p);

        d_connectRetryTimer_sp =
            this->createTimer(timerOptions, timerCallback, d_allocator_p);

        d_connectRetryTimer_sp->schedule(this->currentTime());
    }
    else {
        ntca::TimerOptions timerOptions;
        timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
        timerOptions.setOneShot(false);

        ntci::TimerCallback timerCallback = this->createTimerCallback(
            bdlf::MemFnUtil::memFn(&StreamSocket::processConnectRetryTimer,
                                   self),
            d_allocator_p);

        d_connectRetryTimer_sp =
            this->createTimer(timerOptions, timerCallback, d_allocator_p);

        d_connectRetryTimer_sp->schedule(
            this->currentTime(),
            d_connectOptions.retryInterval().value());
    }

    return ntsa::Error();
}

ntsa::Error StreamSocket::upgrade(
    const bsl::shared_ptr<ntci::Encryption>& encryption,
    const ntca::UpgradeOptions&              options,
    const ntci::UpgradeFunction&             callback)
{
    return this->upgrade(encryption,
                         options,
                         this->createUpgradeCallback(callback, d_allocator_p));
}

ntsa::Error StreamSocket::upgrade(
    const bsl::shared_ptr<ntci::Encryption>& encryption,
    const ntca::UpgradeOptions&              options,
    const ntci::UpgradeCallback&             callback)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    ntsa::Error error;

    if (NTCCFG_UNLIKELY(!d_openState.canSend())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_UNLIKELY(!d_openState.canReceive())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    // It is invalid to upgrade unless no encryption session is currently
    // active.

    if (d_upgradeInProgress) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_encryption_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    // Set the encryption session used to encrypt and decrypt data.

    d_encryption_sp = encryption;

    d_upgradeCallback   = callback;
    d_upgradeInProgress = true;

    // Initiate the upgrade.

    error = this->privateUpgrade(self, options);
    if (error) {
        d_encryption_sp.reset();
        d_upgradeCallback.reset();
        d_upgradeInProgress = false;
        this->privateShutdown(self,
                              ntsa::ShutdownType::e_BOTH,
                              ntsa::ShutdownMode::e_IMMEDIATE,
                              true);
        return error;
    }

    if (!options.deadline().isNull()) {
        ntca::TimerOptions timerOptions;
        timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
        timerOptions.setOneShot(true);

        ntci::TimerCallback timerCallback = this->createTimerCallback(
            bdlf::MemFnUtil::memFn(&StreamSocket::processUpgradeTimer, self),
            d_allocator_p);

        d_upgradeTimer_sp =
            this->createTimer(timerOptions, timerCallback, d_allocator_p);

        d_upgradeTimer_sp->schedule(options.deadline().value());
    }

    this->privateRelaxFlowControl(self,
                                  ntca::FlowControlType::e_RECEIVE,
                                  true,
                                  false);

    return ntsa::Error();
}

ntsa::Error StreamSocket::upgrade(
    const bsl::shared_ptr<ntci::EncryptionClient>& encryptionClient,
    const ntca::UpgradeOptions&                    options,
    const ntci::UpgradeFunction&                   callback)
{
    return this->upgrade(encryptionClient,
                         options,
                         this->createUpgradeCallback(callback, d_allocator_p));
}

ntsa::Error StreamSocket::upgrade(
    const bsl::shared_ptr<ntci::EncryptionClient>& encryptionClient,
    const ntca::UpgradeOptions&                    options,
    const ntci::UpgradeCallback&                   callback)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::Encryption> encryption;
    error = encryptionClient->createEncryption(&encryption, d_allocator_p);
    if (error) {
        return error;
    }

    return this->upgrade(encryption, options, callback);
}

ntsa::Error StreamSocket::upgrade(
    const bsl::shared_ptr<ntci::EncryptionServer>& encryptionServer,
    const ntca::UpgradeOptions&                    options,
    const ntci::UpgradeFunction&                   callback)
{
    return this->upgrade(encryptionServer,
                         options,
                         this->createUpgradeCallback(callback, d_allocator_p));
}

ntsa::Error StreamSocket::upgrade(
    const bsl::shared_ptr<ntci::EncryptionServer>& encryptionServer,
    const ntca::UpgradeOptions&                    options,
    const ntci::UpgradeCallback&                   callback)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::Encryption> encryption;
    error = encryptionServer->createEncryption(&encryption, d_allocator_p);
    if (error) {
        return error;
    }

    return this->upgrade(encryption, options, callback);
}

ntsa::Error StreamSocket::send(const bdlbb::Blob&       data,
                               const ntca::SendOptions& options)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    ntsa::Error error;

    if (NTCCFG_UNLIKELY(!d_openState.canSend())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::size_t effectiveHighWatermark = d_sendQueue.highWatermark();
    if (!options.highWatermark().isNull()) {
        effectiveHighWatermark = options.highWatermark().value();
    }

    if (NTCCFG_UNLIKELY(
            d_sendQueue.isHighWatermarkViolated(effectiveHighWatermark)))
    {
        if (d_sendQueue.authorizeHighWatermarkEvent(effectiveHighWatermark)) {
            NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_HIGH_WATERMARK(
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

    if (NTCCFG_LIKELY(!d_encryption_sp)) {
        return this->privateSendRaw(self, data, options);
    }
    else {
        error = d_encryption_sp->pushOutgoingPlainText(data);
        if (error) {
            return error;
        }

        bdlbb::Blob cipherData(d_outgoingBufferFactory_sp.get());

        while (d_encryption_sp->hasOutgoingCipherText()) {
            error = d_encryption_sp->popOutgoingCipherText(&cipherData);
            if (error) {
                return error;
            }
        }

        if (cipherData.length() > 0) {
            error = this->privateSendRaw(self, cipherData, options);
            if (error) {
                return error;
            }
        }

        return ntsa::Error();
    }
}

ntsa::Error StreamSocket::send(const ntsa::Data&        data,
                               const ntca::SendOptions& options)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    ntsa::Error error;

    if (NTCCFG_UNLIKELY(!d_openState.canSend())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::size_t effectiveHighWatermark = d_sendQueue.highWatermark();
    if (!options.highWatermark().isNull()) {
        effectiveHighWatermark = options.highWatermark().value();
    }

    if (NTCCFG_UNLIKELY(
            d_sendQueue.isHighWatermarkViolated(effectiveHighWatermark)))
    {
        if (d_sendQueue.authorizeHighWatermarkEvent(effectiveHighWatermark)) {
            NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_HIGH_WATERMARK(
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

    if (NTCCFG_LIKELY(!d_encryption_sp)) {
        return this->privateSendRaw(self, data, options);
    }
    else {
        error = d_encryption_sp->pushOutgoingPlainText(data);
        if (error) {
            return error;
        }

        bdlbb::Blob cipherData(d_outgoingBufferFactory_sp.get());

        while (d_encryption_sp->hasOutgoingCipherText()) {
            error = d_encryption_sp->popOutgoingCipherText(&cipherData);
            if (error) {
                return error;
            }
        }

        if (cipherData.length() > 0) {
            error = this->privateSendRaw(self, cipherData, options);
            if (error) {
                return error;
            }
        }

        return ntsa::Error();
    }
}

ntsa::Error StreamSocket::send(const bdlbb::Blob&        data,
                               const ntca::SendOptions&  options,
                               const ntci::SendFunction& callback)
{
    return this->send(data,
                      options,
                      this->createSendCallback(callback, d_allocator_p));
}

ntsa::Error StreamSocket::send(const bdlbb::Blob&        data,
                               const ntca::SendOptions&  options,
                               const ntci::SendCallback& callback)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    ntsa::Error error;

    if (NTCCFG_UNLIKELY(!d_openState.canSend())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::size_t effectiveHighWatermark = d_sendQueue.highWatermark();
    if (!options.highWatermark().isNull()) {
        effectiveHighWatermark = options.highWatermark().value();
    }

    if (NTCCFG_UNLIKELY(
            d_sendQueue.isHighWatermarkViolated(effectiveHighWatermark)))
    {
        if (d_sendQueue.authorizeHighWatermarkEvent(effectiveHighWatermark)) {
            NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_HIGH_WATERMARK(
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

    if (NTCCFG_LIKELY(!d_encryption_sp)) {
        return this->privateSendRaw(self, data, options, callback);
    }
    else {
        bdlbb::Blob cipherData(d_outgoingBufferFactory_sp.get());

        error = d_encryption_sp->pushOutgoingPlainText(data);
        if (error) {
            return error;
        }

        while (d_encryption_sp->hasOutgoingCipherText()) {
            error = d_encryption_sp->popOutgoingCipherText(&cipherData);
            if (error) {
                return error;
            }
        }

        if (cipherData.length() > 0) {
            error = this->privateSendRaw(self, cipherData, options, callback);
            if (error) {
                return error;
            }
        }
        else {
            ntca::SendContext sendContext;

            ntca::SendEvent sendEvent;
            sendEvent.setType(ntca::SendEventType::e_COMPLETE);
            sendEvent.setContext(sendContext);

            const bool defer = !options.recurse();

            callback.dispatch(self,
                              sendEvent,
                              ntci::Strand::unknown(),
                              self,
                              defer,
                              &d_mutex);
        }

        return ntsa::Error();
    }
}

ntsa::Error StreamSocket::send(const ntsa::Data&         data,
                               const ntca::SendOptions&  options,
                               const ntci::SendFunction& callback)
{
    return this->send(data,
                      options,
                      this->createSendCallback(callback, d_allocator_p));
}

ntsa::Error StreamSocket::send(const ntsa::Data&         data,
                               const ntca::SendOptions&  options,
                               const ntci::SendCallback& callback)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    ntsa::Error error;

    if (NTCCFG_UNLIKELY(!d_openState.canSend())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::size_t effectiveHighWatermark = d_sendQueue.highWatermark();
    if (!options.highWatermark().isNull()) {
        effectiveHighWatermark = options.highWatermark().value();
    }

    if (NTCCFG_UNLIKELY(
            d_sendQueue.isHighWatermarkViolated(effectiveHighWatermark)))
    {
        if (d_sendQueue.authorizeHighWatermarkEvent(effectiveHighWatermark)) {
            NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_HIGH_WATERMARK(
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

    if (NTCCFG_LIKELY(!d_encryption_sp)) {
        return this->privateSendRaw(self, data, options, callback);
    }
    else {
        error = d_encryption_sp->pushOutgoingPlainText(data);
        if (error) {
            return error;
        }

        bdlbb::Blob cipherData(d_outgoingBufferFactory_sp.get());

        while (d_encryption_sp->hasOutgoingCipherText()) {
            error = d_encryption_sp->popOutgoingCipherText(&cipherData);
            if (error) {
                return error;
            }
        }

        if (cipherData.length() > 0) {
            error = this->privateSendRaw(self, cipherData, options, callback);
            if (error) {
                return error;
            }
        }
        else {
            ntca::SendContext sendContext;

            ntca::SendEvent sendEvent;
            sendEvent.setType(ntca::SendEventType::e_COMPLETE);
            sendEvent.setContext(sendContext);

            const bool defer = !options.recurse();

            callback.dispatch(self,
                              sendEvent,
                              ntci::Strand::unknown(),
                              self,
                              defer,
                              &d_mutex);
        }

        return ntsa::Error();
    }
}

ntsa::Error StreamSocket::receive(ntca::ReceiveContext*       context,
                                  bdlbb::Blob*                data,
                                  const ntca::ReceiveOptions& options)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    ntsa::Error error;

    if (NTCCFG_UNLIKELY(!d_openState.canReceive())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_UNLIKELY(d_receiveQueue.size() == 0 &&
                        !d_shutdownState.canReceive()))
    {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    if (NTCCFG_LIKELY(d_receiveQueue.size() >= options.minSize())) {
        BSLS_ASSERT(d_receiveQueue.hasEntry());
        BSLS_ASSERT(d_receiveQueue.size() ==
                    NTCCFG_WARNING_PROMOTE(bsl::size_t,
                                           d_receiveQueue.data()->length()));

        bool receiveQueueHighWatermarkViolatedBefore =
            d_receiveQueue.isHighWatermarkViolated();

        bsl::size_t numBytesRemaining = options.maxSize();
        bsl::size_t numBytesDequeued  = 0;

        while (NTCCFG_LIKELY(d_receiveQueue.hasEntry())) {
            ntcq::ReceiveQueueEntry& entry = d_receiveQueue.frontEntry();

            bsl::size_t numBytesToDequeue =
                bsl::min(numBytesRemaining, entry.length());

            numBytesDequeued += numBytesToDequeue;
            BSLS_ASSERT(numBytesDequeued <= options.maxSize());

            BSLS_ASSERT(numBytesRemaining >= numBytesToDequeue);
            numBytesRemaining -= numBytesToDequeue;

            if (numBytesToDequeue == entry.length()) {
                NTCS_METRICS_UPDATE_READ_QUEUE_DELAY(entry.delay());

                if (d_receiveQueue.popEntry()) {
                    break;
                }
            }
            else {
                d_receiveQueue.popSize(numBytesToDequeue);
                break;
            }

            if (numBytesRemaining == 0) {
                break;
            }
        }

        BSLS_ASSERT(numBytesDequeued >= options.minSize());
        BSLS_ASSERT(numBytesDequeued <= options.maxSize());

        context->setTransport(d_transport);
        context->setEndpoint(d_remoteEndpoint);

        ntcs::BlobUtil::append(data, d_receiveQueue.data(), numBytesDequeued);

        ntcs::BlobUtil::pop(d_receiveQueue.data(), numBytesDequeued);

        BSLS_ASSERT(d_receiveQueue.size() ==
                    NTCCFG_WARNING_PROMOTE(bsl::size_t,
                                           d_receiveQueue.data()->length()));

        NTCP_STREAMSOCKET_LOG_READ_QUEUE_DRAINED(d_receiveQueue.size());

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

ntsa::Error StreamSocket::receive(const ntca::ReceiveOptions&  options,
                                  const ntci::ReceiveFunction& callback)
{
    return this->receive(options,
                         this->createReceiveCallback(callback, d_allocator_p));
}

ntsa::Error StreamSocket::receive(const ntca::ReceiveOptions&  options,
                                  const ntci::ReceiveCallback& callback)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    ntsa::Error error;

    if (NTCCFG_UNLIKELY(!d_openState.canReceive())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_UNLIKELY(d_receiveQueue.size() == 0 &&
                        !d_shutdownState.canReceive()))
    {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> callbackEntry =
        d_receiveQueue.createCallbackEntry();
    callbackEntry->assign(callback, options);

    if (NTCCFG_LIKELY(!d_receiveQueue.hasCallbackEntry() &&
                      d_receiveQueue.size() >= options.minSize()))
    {
        BSLS_ASSERT(d_receiveQueue.hasEntry());
        BSLS_ASSERT(d_receiveQueue.size() ==
                    NTCCFG_WARNING_PROMOTE(bsl::size_t,
                                           d_receiveQueue.data()->length()));

        bool receiveQueueHighWatermarkViolatedBefore =
            d_receiveQueue.isHighWatermarkViolated();

        bsl::size_t numBytesRemaining = options.maxSize();
        bsl::size_t numBytesDequeued  = 0;

        while (NTCCFG_LIKELY(d_receiveQueue.hasEntry())) {
            ntcq::ReceiveQueueEntry& entry = d_receiveQueue.frontEntry();

            bsl::size_t numBytesToDequeue =
                bsl::min(numBytesRemaining, entry.length());

            numBytesDequeued += numBytesToDequeue;
            BSLS_ASSERT(numBytesDequeued <= options.maxSize());

            BSLS_ASSERT(numBytesRemaining >= numBytesToDequeue);
            numBytesRemaining -= numBytesToDequeue;

            if (numBytesToDequeue == entry.length()) {
                NTCS_METRICS_UPDATE_READ_QUEUE_DELAY(entry.delay());

                if (d_receiveQueue.popEntry()) {
                    break;
                }
            }
            else {
                d_receiveQueue.popSize(numBytesToDequeue);
                break;
            }

            if (numBytesRemaining == 0) {
                break;
            }
        }

        BSLS_ASSERT(numBytesDequeued >= options.minSize());
        BSLS_ASSERT(numBytesDequeued <= options.maxSize());

        bsl::shared_ptr<bdlbb::Blob> data =
            d_dataPool_sp->createIncomingBlob();

        ntcs::BlobUtil::append(data, d_receiveQueue.data(), numBytesDequeued);

        ntcs::BlobUtil::pop(d_receiveQueue.data(), numBytesDequeued);

        BSLS_ASSERT(d_receiveQueue.size() ==
                    NTCCFG_WARNING_PROMOTE(bsl::size_t,
                                           d_receiveQueue.data()->length()));

        NTCP_STREAMSOCKET_LOG_READ_QUEUE_DRAINED(d_receiveQueue.size());

        NTCS_METRICS_UPDATE_READ_QUEUE_SIZE(d_receiveQueue.size());

        ntca::ReceiveContext receiveContext;
        receiveContext.setTransport(d_transport);
        receiveContext.setEndpoint(d_remoteEndpoint);

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
                    &StreamSocket::processReceiveDeadlineTimer,
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

ntsa::Error StreamSocket::registerResolver(
    const bsl::shared_ptr<ntci::Resolver>& resolver)
{
    LockGuard lock(&d_mutex);
    d_resolver = resolver;
    return ntsa::Error();
}

ntsa::Error StreamSocket::deregisterResolver()
{
    LockGuard lock(&d_mutex);
    d_resolver.reset();
    return ntsa::Error();
}

ntsa::Error StreamSocket::registerManager(
    const bsl::shared_ptr<ntci::StreamSocketManager>& manager)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

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

ntsa::Error StreamSocket::deregisterManager()
{
    LockGuard lock(&d_mutex);

    d_manager_sp.reset();
    d_managerStrand_sp.reset();

    return ntsa::Error();
}

ntsa::Error StreamSocket::registerSession(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

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

ntsa::Error StreamSocket::registerSessionCallback(
    const ntci::StreamSocket::SessionCallback& callback)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (callback) {
        bsl::shared_ptr<ntcu::StreamSocketSession> session;
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

ntsa::Error StreamSocket::registerSessionCallback(
    const ntci::StreamSocket::SessionCallback& callback,
    const bsl::shared_ptr<ntci::Strand>&       strand)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    if (callback) {
        bsl::shared_ptr<ntcu::StreamSocketSession> session;
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

ntsa::Error StreamSocket::deregisterSession()
{
    LockGuard lock(&d_mutex);

    d_session_sp.reset();
    d_sessionStrand_sp.reset();

    return ntsa::Error();
}

ntsa::Error StreamSocket::setWriteRateLimiter(
    const bsl::shared_ptr<ntci::RateLimiter>& rateLimiter)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

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

ntsa::Error StreamSocket::setWriteQueueLowWatermark(bsl::size_t lowWatermark)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    d_sendQueue.setLowWatermark(lowWatermark);

    if (d_sendQueue.authorizeLowWatermarkEvent()) {
        NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_LOW_WATERMARK(
            d_sendQueue.lowWatermark(),
            d_sendQueue.size());

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

ntsa::Error StreamSocket::setWriteQueueHighWatermark(bsl::size_t highWatermark)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    d_sendQueue.setHighWatermark(highWatermark);

    if (d_sendQueue.authorizeHighWatermarkEvent()) {
        NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_HIGH_WATERMARK(
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

    return ntsa::Error();
}

ntsa::Error StreamSocket::setWriteQueueWatermarks(bsl::size_t lowWatermark,
                                                  bsl::size_t highWatermark)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    d_sendQueue.setLowWatermark(lowWatermark);
    d_sendQueue.setHighWatermark(highWatermark);

    if (d_sendQueue.authorizeLowWatermarkEvent()) {
        NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_LOW_WATERMARK(
            d_sendQueue.lowWatermark(),
            d_sendQueue.size());

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
        NTCP_STREAMSOCKET_LOG_WRITE_QUEUE_HIGH_WATERMARK(
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

    return ntsa::Error();
}

ntsa::Error StreamSocket::setReadRateLimiter(
    const bsl::shared_ptr<ntci::RateLimiter>& rateLimiter)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

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

ntsa::Error StreamSocket::setReadQueueLowWatermark(bsl::size_t lowWatermark)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

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

ntsa::Error StreamSocket::setReadQueueHighWatermark(bsl::size_t highWatermark)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

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

ntsa::Error StreamSocket::setReadQueueWatermarks(bsl::size_t lowWatermark,
                                                 bsl::size_t highWatermark)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

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

ntsa::Error StreamSocket::relaxFlowControl(
    ntca::FlowControlType::Value direction)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    return this->privateRelaxFlowControl(self, direction, true, true);
}

ntsa::Error StreamSocket::applyFlowControl(
    ntca::FlowControlType::Value direction,
    ntca::FlowControlMode::Value mode)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

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

ntsa::Error StreamSocket::cancel(const ntca::BindToken& token)
{
    NTCCFG_WARNING_UNUSED(token);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::cancel(const ntca::ConnectToken& token)
{
    NTCCFG_WARNING_UNUSED(token);

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    if (d_connectInProgress) {
        this->privateFailConnect(self,
                                 ntsa::Error(ntsa::Error::e_CANCELLED),
                                 false,
                                 true);

        return ntsa::Error();
    }

    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error StreamSocket::cancel(const ntca::UpgradeToken& token)
{
    NTCCFG_WARNING_UNUSED(token);

    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    if (d_upgradeInProgress) {
        ntca::UpgradeContext upgradeContext;
        upgradeContext.setError(ntsa::Error(ntsa::Error::e_CANCELLED));
        upgradeContext.setErrorDescription("");

        d_upgradeInProgress = false;
        d_encryption_sp.reset();

        ntci::UpgradeCallback upgradeCallback = d_upgradeCallback;
        d_upgradeCallback.reset();

        ntca::UpgradeEvent upgradeEvent;
        upgradeEvent.setType(ntca::UpgradeEventType::e_ERROR);
        upgradeEvent.setContext(upgradeContext);

        if (d_upgradeTimer_sp) {
            d_upgradeTimer_sp->close();
            d_upgradeTimer_sp.reset();
        }

        if (upgradeCallback) {
            upgradeCallback.dispatch(self,
                                     upgradeEvent,
                                     ntci::Strand::unknown(),
                                     self,
                                     true,
                                     &d_mutex);
        }

        this->privateFail(self, ntsa::Error(ntsa::Error::e_CANCELLED));

        return ntsa::Error();
    }

    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error StreamSocket::cancel(const ntca::SendToken& token)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    ntci::SendCallback callback;
    bool becameEmpty = d_sendQueue.removeEntryToken(&callback, token);

    if (becameEmpty) {
        this->privateApplyFlowControl(self,
                                      ntca::FlowControlType::e_SEND,
                                      ntca::FlowControlMode::e_IMMEDIATE,
                                      true,
                                      false);
    }

    if (callback) {
        ntca::SendContext sendContext;
        sendContext.setError(ntsa::Error(ntsa::Error::e_CANCELLED));

        ntca::SendEvent sendEvent;
        sendEvent.setType(ntca::SendEventType::e_ERROR);
        sendEvent.setContext(sendContext);

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

ntsa::Error StreamSocket::cancel(const ntca::ReceiveToken& token)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> callbackEntry;
    ntsa::Error                                      error =
        d_receiveQueue.removeCallbackEntry(&callbackEntry, token);
    if (!error) {
        ntca::ReceiveContext receiveContext;
        receiveContext.setError(ntsa::Error(ntsa::Error::e_CANCELLED));
        receiveContext.setTransport(d_transport);
        receiveContext.setEndpoint(d_remoteEndpoint);

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

ntsa::Error StreamSocket::downgrade()
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    if (!d_encryption_sp) {
        return ntsa::Error::invalid();
    }

    if (d_encryption_sp->isShutdownSent()) {
        return ntsa::Error();
    }

    ntsa::Error error;

    error = d_encryption_sp->shutdown();
    if (error) {
        return error;
    }

    if (!d_encryption_sp->isShutdownReceived()) {
        if (d_session_sp) {
            ntca::DowngradeContext context;

            ntca::DowngradeEvent event;
            event.setType(ntca::DowngradeEventType::e_INITIATED);
            event.setContext(context);

            ntcs::Dispatch::announceDowngradeInitiated(d_session_sp,
                                                       self,
                                                       event,
                                                       d_sessionStrand_sp,
                                                       ntci::Strand::unknown(),
                                                       self,
                                                       true,
                                                       &d_mutex);
        }
    }

    bdlbb::Blob cipherData(d_outgoingBufferFactory_sp.get());

    while (d_encryption_sp->hasOutgoingCipherText()) {
        error = d_encryption_sp->popOutgoingCipherText(&cipherData);
        if (error) {
            return error;
        }
    }

    if (cipherData.length() > 0) {
        error = this->privateSendRaw(self, cipherData, ntca::SendOptions());
        if (error) {
            return error;
        }
    }

    if (d_encryption_sp->isShutdownFinished()) {
        d_encryption_sp.reset();

        if (d_session_sp) {
            ntca::DowngradeContext context;

            ntca::DowngradeEvent event;
            event.setType(ntca::DowngradeEventType::e_COMPLETE);
            event.setContext(context);

            ntcs::Dispatch::announceDowngradeComplete(d_session_sp,
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

ntsa::Error StreamSocket::shutdown(ntsa::ShutdownType::Value direction,
                                   ntsa::ShutdownMode::Value mode)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    if (d_detachState.get() == ntcs::DetachState::e_DETACH_INITIATED) {
        d_deferredCalls.push_back(
            NTCCFG_BIND(&StreamSocket::shutdown, self, direction, mode));
        return ntsa::Error();
        ;
    }

    if (d_connectInProgress) {
        if (direction == ntsa::ShutdownType::e_SEND ||
            direction == ntsa::ShutdownType::e_BOTH)
        {
            this->privateFailConnect(self,
                                     ntsa::Error(ntsa::Error::e_CANCELLED),
                                     true,
                                     true);
        }
    }
    else {
        this->privateShutdown(self, direction, mode, true);
    }

    return ntsa::Error();
}

void StreamSocket::close()
{
    this->close(ntci::CloseCallback());
}

void StreamSocket::close(const ntci::CloseFunction& callback)
{
    return this->close(this->createCloseCallback(callback, d_allocator_p));
}

void StreamSocket::close(const ntci::CloseCallback& callback)
{
    bsl::shared_ptr<StreamSocket> self = this->getSelf(this);

    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_publicHandle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    if (d_detachState.get() == ntcs::DetachState::e_DETACH_INITIATED) {
        d_deferredCalls.push_back(NTCCFG_BIND(
            static_cast<void (StreamSocket::*)(
                const ntci::CloseCallback& callback)>(&StreamSocket::close),
            self,
            callback));
        return;
    }

    BSLS_ASSERT(!d_closeCallback);
    d_closeCallback = callback;

    if (d_connectInProgress) {
        this->privateFailConnect(self,
                                 ntsa::Error(ntsa::Error::e_CANCELLED),
                                 true,
                                 true);
    }
    else {
        // MRM: Announce discarded.

        this->privateShutdown(self,
                              ntsa::ShutdownType::e_BOTH,
                              ntsa::ShutdownMode::e_IMMEDIATE,
                              true);
    }
}

void StreamSocket::execute(const Functor& functor)
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

void StreamSocket::moveAndExecute(FunctorSequence* functorSequence,
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
bsl::shared_ptr<ntci::Strand> StreamSocket::createStrand(
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

bsl::shared_ptr<ntci::Timer> StreamSocket::createTimer(
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

bsl::shared_ptr<ntci::Timer> StreamSocket::createTimer(
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

bsl::shared_ptr<ntsa::Data> StreamSocket::createIncomingData()
{
    return d_dataPool_sp->createIncomingData();
}

bsl::shared_ptr<ntsa::Data> StreamSocket::createOutgoingData()
{
    return d_dataPool_sp->createOutgoingData();
}

bsl::shared_ptr<bdlbb::Blob> StreamSocket::createIncomingBlob()
{
    return d_dataPool_sp->createIncomingBlob();
}

bsl::shared_ptr<bdlbb::Blob> StreamSocket::createOutgoingBlob()
{
    return d_dataPool_sp->createOutgoingBlob();
}

void StreamSocket::createIncomingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_incomingBufferFactory_sp->allocate(blobBuffer);
}

void StreamSocket::createOutgoingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_outgoingBufferFactory_sp->allocate(blobBuffer);
}

ntsa::Handle StreamSocket::handle() const
{
    return d_publicHandle;
}

ntsa::Transport::Value StreamSocket::transport() const
{
    return d_transport;
}

ntsa::Endpoint StreamSocket::sourceEndpoint() const
{
    LockGuard lock(&d_mutex);
    return d_sourceEndpoint;
}

ntsa::Endpoint StreamSocket::remoteEndpoint() const
{
    LockGuard lock(&d_mutex);
    return d_remoteEndpoint;
}

bsl::shared_ptr<ntci::EncryptionCertificate> StreamSocket::sourceCertificate()
    const
{
    LockGuard lock(&d_mutex);

    bsl::shared_ptr<ntci::EncryptionCertificate> result;
    if (d_encryption_sp) {
        result = d_encryption_sp->sourceCertificate();
    }

    return result;
}

bsl::shared_ptr<ntci::EncryptionCertificate> StreamSocket::remoteCertificate()
    const
{
    LockGuard lock(&d_mutex);

    bsl::shared_ptr<ntci::EncryptionCertificate> result;
    if (d_encryption_sp) {
        result = d_encryption_sp->remoteCertificate();
    }

    return result;
}

bsl::shared_ptr<ntci::EncryptionKey> StreamSocket::privateKey() const
{
    LockGuard lock(&d_mutex);

    bsl::shared_ptr<ntci::EncryptionKey> result;
    if (d_encryption_sp) {
        result = d_encryption_sp->privateKey();
    }

    return result;
}

bsl::shared_ptr<ntci::ListenerSocket> StreamSocket::acceptor() const
{
    return d_acceptor_sp;
}

const bsl::shared_ptr<ntci::Strand>& StreamSocket::strand() const
{
    return d_proactorStrand_sp;
}

bslmt::ThreadUtil::Handle StreamSocket::threadHandle() const
{
    ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
    if (proactorRef) {
        return proactorRef->threadHandle();
    }
    else {
        return bslmt::ThreadUtil::invalidHandle();
    }
}

bsl::size_t StreamSocket::threadIndex() const
{
    ntcs::ObserverRef<ntci::Proactor> proactorRef(&d_proactor);
    if (proactorRef) {
        return proactorRef->threadIndex();
    }
    else {
        return 0;
    }
}

bsl::size_t StreamSocket::readQueueSize() const
{
    LockGuard lock(&d_mutex);
    return d_receiveQueue.size();
}

bsl::size_t StreamSocket::readQueueLowWatermark() const
{
    LockGuard lock(&d_mutex);
    return d_receiveQueue.lowWatermark();
}

bsl::size_t StreamSocket::readQueueHighWatermark() const
{
    LockGuard lock(&d_mutex);
    return d_receiveQueue.highWatermark();
}

bsl::size_t StreamSocket::writeQueueSize() const
{
    LockGuard lock(&d_mutex);
    return d_sendQueue.size();
}

bsl::size_t StreamSocket::writeQueueLowWatermark() const
{
    LockGuard lock(&d_mutex);
    return d_sendQueue.lowWatermark();
}

bsl::size_t StreamSocket::writeQueueHighWatermark() const
{
    LockGuard lock(&d_mutex);
    return d_sendQueue.highWatermark();
}

bsl::size_t StreamSocket::totalBytesSent() const
{
    // TODO
    return 0;
}

bsl::size_t StreamSocket::totalBytesReceived() const
{
    // TODO
    return 0;
}

bsls::TimeInterval StreamSocket::currentTime() const
{
    return bdlt::CurrentTime::now();
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& StreamSocket::
    incomingBlobBufferFactory() const
{
    return d_incomingBufferFactory_sp;
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& StreamSocket::
    outgoingBlobBufferFactory() const
{
    return d_outgoingBufferFactory_sp;
}

}  // close package namespace
}  // close enterprise namespace
