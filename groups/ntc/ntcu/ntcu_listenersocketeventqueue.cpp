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

#include <ntcu_listenersocketeventqueue.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcu_listenersocketeventqueue_cpp, "$Id$ $CSID$")

#include <ntci_log.h>

#include <bslmt_lockguard.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#if BSL_VERSION >= BSL_MAKE_VERSION(3, 80)
#define NTCU_LISTENERSOCKETEVENTQUEUE_TIMEOUT bslmt::Condition::e_TIMED_OUT
#else
#define NTCU_LISTENERSOCKETEVENTQUEUE_TIMEOUT -1
#endif

#define NTCU_LISTENERSOCKETEVENTQUEUE_LOG_EVENT(listenerSocket, event)        \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG                                                 \
            << "Listener socket at " << listenerSocket->sourceEndpoint()      \
            << " announced event " << event << NTCI_LOG_STREAM_END;           \
    } while (false)

namespace BloombergLP {
namespace ntcu {

void ListenerSocketEventQueue::processAcceptQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);

    NTCI_LOG_CONTEXT();

    NTCU_LISTENERSOCKETEVENTQUEUE_LOG_EVENT(listenerSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() ==
                ntca::AcceptQueueEventType::e_FLOW_CONTROL_RELAXED);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::ListenerSocketEvent(event));
        d_condition.signal();
    }
}

void ListenerSocketEventQueue::processAcceptQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);

    NTCI_LOG_CONTEXT();

    NTCU_LISTENERSOCKETEVENTQUEUE_LOG_EVENT(listenerSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() ==
                ntca::AcceptQueueEventType::e_FLOW_CONTROL_APPLIED);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::ListenerSocketEvent(event));
        d_condition.signal();
    }
}

void ListenerSocketEventQueue::processAcceptQueueLowWatermark(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);

    NTCI_LOG_CONTEXT();

    NTCU_LISTENERSOCKETEVENTQUEUE_LOG_EVENT(listenerSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() == ntca::AcceptQueueEventType::e_LOW_WATERMARK);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::ListenerSocketEvent(event));
        d_condition.signal();
    }
}

void ListenerSocketEventQueue::processAcceptQueueHighWatermark(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);

    NTCI_LOG_CONTEXT();

    NTCU_LISTENERSOCKETEVENTQUEUE_LOG_EVENT(listenerSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() == ntca::AcceptQueueEventType::e_HIGH_WATERMARK);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::ListenerSocketEvent(event));
        d_condition.signal();
    }
}

void ListenerSocketEventQueue::processAcceptQueueDiscarded(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);

    NTCI_LOG_CONTEXT();

    NTCU_LISTENERSOCKETEVENTQUEUE_LOG_EVENT(listenerSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() == ntca::AcceptQueueEventType::e_DISCARDED);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::ListenerSocketEvent(event));
        d_condition.signal();
    }
}

void ListenerSocketEventQueue::processShutdownInitiated(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);

    NTCI_LOG_CONTEXT();

    NTCU_LISTENERSOCKETEVENTQUEUE_LOG_EVENT(listenerSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() == ntca::ShutdownEventType::e_INITIATED);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::ListenerSocketEvent(event));
        d_condition.signal();
    }
}

void ListenerSocketEventQueue::processShutdownReceive(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);

    NTCI_LOG_CONTEXT();

    NTCU_LISTENERSOCKETEVENTQUEUE_LOG_EVENT(listenerSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() == ntca::ShutdownEventType::e_RECEIVE);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::ListenerSocketEvent(event));
        d_condition.signal();
    }
}

void ListenerSocketEventQueue::processShutdownSend(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);

    NTCI_LOG_CONTEXT();

    NTCU_LISTENERSOCKETEVENTQUEUE_LOG_EVENT(listenerSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() == ntca::ShutdownEventType::e_SEND);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::ListenerSocketEvent(event));
        d_condition.signal();
    }
}

void ListenerSocketEventQueue::processShutdownComplete(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);

    NTCI_LOG_CONTEXT();

    NTCU_LISTENERSOCKETEVENTQUEUE_LOG_EVENT(listenerSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() == ntca::ShutdownEventType::e_COMPLETE);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::ListenerSocketEvent(event));
        d_condition.signal();
    }
}

void ListenerSocketEventQueue::processError(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ErrorEvent&                      event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);

    NTCI_LOG_CONTEXT();

    NTCU_LISTENERSOCKETEVENTQUEUE_LOG_EVENT(listenerSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::ListenerSocketEvent(event));
        d_condition.signal();
    }
}

const bsl::shared_ptr<ntci::Strand>& ListenerSocketEventQueue::strand() const
{
    return d_strand_sp;
}

bool ListenerSocketEventQueue::want(
    ntca::AcceptQueueEventType::Value type) const
{
    BSLS_ASSERT_OPT(
        static_cast<int>(ntca::ListenerSocketEventType::e_ACCEPT_QUEUE) <
        k_NUM_EVENT_TYPES);

    const bsl::uint32_t interest = d_interest[static_cast<int>(
        ntca::ListenerSocketEventType::e_ACCEPT_QUEUE)];

    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    if ((interest & mask) != 0) {
        return true;
    }

    return false;
}

bool ListenerSocketEventQueue::want(ntca::ShutdownEventType::Value type) const
{
    BSLS_ASSERT_OPT(
        static_cast<int>(ntca::ListenerSocketEventType::e_SHUTDOWN) <
        k_NUM_EVENT_TYPES);

    const bsl::uint32_t interest = d_interest[static_cast<int>(
        ntca::ListenerSocketEventType::e_SHUTDOWN)];

    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    if ((interest & mask) != 0) {
        return true;
    }

    return false;
}

bool ListenerSocketEventQueue::want(ntca::ErrorEventType::Value type) const
{
    BSLS_ASSERT_OPT(static_cast<int>(ntca::ListenerSocketEventType::e_ERROR) <
                    k_NUM_EVENT_TYPES);

    const bsl::uint32_t interest =
        d_interest[static_cast<int>(ntca::ListenerSocketEventType::e_ERROR)];

    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    if ((interest & mask) != 0) {
        return true;
    }

    return false;
}

ListenerSocketEventQueue::ListenerSocketEventQueue(
    bslma::Allocator* basicAllocator)
: d_mutex()
, d_condition()
, d_queue(basicAllocator)
, d_closed(false)
, d_strand_sp(ntci::Strand::unspecified())
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    bsl::memset(&d_interest, 0, sizeof d_interest);
}

ListenerSocketEventQueue::~ListenerSocketEventQueue()
{
}

void ListenerSocketEventQueue::showAll()
{
    const bsl::uint32_t mask = static_cast<bsl::uint32_t>(-1);

    for (bsl::size_t i = 0; i < k_NUM_EVENT_TYPES; ++i) {
        d_interest[i] = mask;
    }
}

void ListenerSocketEventQueue::show(ntca::ListenerSocketEventType::Value type)
{
    BSLS_ASSERT_OPT(static_cast<int>(type) < k_NUM_EVENT_TYPES);

    const bsl::uint32_t mask = static_cast<bsl::uint32_t>(-1);

    d_interest[type] = mask;
}

void ListenerSocketEventQueue::show(ntca::AcceptQueueEventType::Value type)
{
    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    d_interest[ntca::ListenerSocketEventType::e_ACCEPT_QUEUE] |= mask;
}

void ListenerSocketEventQueue::show(ntca::ShutdownEventType::Value type)
{
    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    d_interest[ntca::ListenerSocketEventType::e_SHUTDOWN] |= mask;
}

void ListenerSocketEventQueue::show(ntca::ErrorEventType::Value type)
{
    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    d_interest[ntca::ListenerSocketEventType::e_ERROR] |= mask;
}

void ListenerSocketEventQueue::hideAll()
{
    for (bsl::size_t i = 0; i < k_NUM_EVENT_TYPES; ++i) {
        d_interest[i] = 0;
    }
}

void ListenerSocketEventQueue::hide(ntca::ListenerSocketEventType::Value type)
{
    BSLS_ASSERT_OPT(static_cast<int>(type) < k_NUM_EVENT_TYPES);

    d_interest[type] = 0;
}

void ListenerSocketEventQueue::hide(ntca::AcceptQueueEventType::Value type)
{
    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    d_interest[ntca::ListenerSocketEventType::e_ACCEPT_QUEUE] &= ~mask;
}

void ListenerSocketEventQueue::hide(ntca::ShutdownEventType::Value type)
{
    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    d_interest[ntca::ListenerSocketEventType::e_SHUTDOWN] &= ~mask;
}

void ListenerSocketEventQueue::hide(ntca::ErrorEventType::Value type)
{
    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    d_interest[ntca::ListenerSocketEventType::e_ERROR] &= ~mask;
}

ntsa::Error ListenerSocketEventQueue::wait(ntca::ListenerSocketEvent* result)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    while (!d_closed && d_queue.empty()) {
        d_condition.wait(&d_mutex);
    }

    if (d_closed) {
        return ntsa::Error(ntsa::Error::e_CANCELLED);
    }

    *result = d_queue.front();
    d_queue.pop_front();

    return ntsa::Error();
}

ntsa::Error ListenerSocketEventQueue::wait(ntca::ListenerSocketEvent* result,
                                           const bsls::TimeInterval&  timeout)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    while (!d_closed && d_queue.empty()) {
        int rc = d_condition.timedWait(&d_mutex, timeout);
        if (rc == 0) {
            break;
        }
        else if (rc == NTCU_LISTENERSOCKETEVENTQUEUE_TIMEOUT) {
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (d_closed) {
        return ntsa::Error(ntsa::Error::e_CANCELLED);
    }

    *result = d_queue.front();
    d_queue.pop_front();

    return ntsa::Error();
}

ntsa::Error ListenerSocketEventQueue::wait(ntca::AcceptQueueEvent* result)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            d_condition.wait(&d_mutex);
        }

        if (d_closed) {
            return ntsa::Error(ntsa::Error::e_CANCELLED);
        }

        for (Queue::iterator it = d_queue.begin(); it != d_queue.end(); ++it) {
            const ntca::ListenerSocketEvent& event = *it;
            if (event.isAcceptQueueEvent()) {
                *result = event.acceptQueueEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error ListenerSocketEventQueue::wait(ntca::AcceptQueueEvent*   result,
                                           const bsls::TimeInterval& timeout)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            int rc = d_condition.timedWait(&d_mutex, timeout);
            if (rc == 0) {
                break;
            }
            else if (rc == NTCU_LISTENERSOCKETEVENTQUEUE_TIMEOUT) {
                return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }

        if (d_closed) {
            return ntsa::Error(ntsa::Error::e_CANCELLED);
        }

        for (Queue::iterator it = d_queue.begin(); it != d_queue.end(); ++it) {
            const ntca::ListenerSocketEvent& event = *it;
            if (event.isAcceptQueueEvent()) {
                *result = event.acceptQueueEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error ListenerSocketEventQueue::wait(
    ntca::AcceptQueueEvent*           result,
    ntca::AcceptQueueEventType::Value type)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            d_condition.wait(&d_mutex);
        }

        if (d_closed) {
            return ntsa::Error(ntsa::Error::e_CANCELLED);
        }

        for (Queue::iterator it = d_queue.begin(); it != d_queue.end(); ++it) {
            const ntca::ListenerSocketEvent& event = *it;
            if (event.isAcceptQueueEvent() &&
                event.acceptQueueEvent().type() == type)
            {
                *result = event.acceptQueueEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error ListenerSocketEventQueue::wait(
    ntca::AcceptQueueEvent*           result,
    ntca::AcceptQueueEventType::Value type,
    const bsls::TimeInterval&         timeout)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            int rc = d_condition.timedWait(&d_mutex, timeout);
            if (rc == 0) {
                break;
            }
            else if (rc == NTCU_LISTENERSOCKETEVENTQUEUE_TIMEOUT) {
                return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }

        if (d_closed) {
            return ntsa::Error(ntsa::Error::e_CANCELLED);
        }

        for (Queue::iterator it = d_queue.begin(); it != d_queue.end(); ++it) {
            const ntca::ListenerSocketEvent& event = *it;
            if (event.isAcceptQueueEvent() &&
                event.acceptQueueEvent().type() == type)
            {
                *result = event.acceptQueueEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error ListenerSocketEventQueue::wait(ntca::ShutdownEvent* result)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            d_condition.wait(&d_mutex);
        }

        if (d_closed) {
            return ntsa::Error(ntsa::Error::e_CANCELLED);
        }

        for (Queue::iterator it = d_queue.begin(); it != d_queue.end(); ++it) {
            const ntca::ListenerSocketEvent& event = *it;
            if (event.isShutdownEvent()) {
                *result = event.shutdownEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error ListenerSocketEventQueue::wait(ntca::ShutdownEvent*      result,
                                           const bsls::TimeInterval& timeout)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            int rc = d_condition.timedWait(&d_mutex, timeout);
            if (rc == 0) {
                break;
            }
            else if (rc == NTCU_LISTENERSOCKETEVENTQUEUE_TIMEOUT) {
                return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }

        if (d_closed) {
            return ntsa::Error(ntsa::Error::e_CANCELLED);
        }

        for (Queue::iterator it = d_queue.begin(); it != d_queue.end(); ++it) {
            const ntca::ListenerSocketEvent& event = *it;
            if (event.isShutdownEvent()) {
                *result = event.shutdownEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error ListenerSocketEventQueue::wait(ntca::ShutdownEvent* result,
                                           ntca::ShutdownEventType::Value type)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            d_condition.wait(&d_mutex);
        }

        if (d_closed) {
            return ntsa::Error(ntsa::Error::e_CANCELLED);
        }

        for (Queue::iterator it = d_queue.begin(); it != d_queue.end(); ++it) {
            const ntca::ListenerSocketEvent& event = *it;
            if (event.isShutdownEvent() &&
                event.shutdownEvent().type() == type)
            {
                *result = event.shutdownEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error ListenerSocketEventQueue::wait(ntca::ShutdownEvent* result,
                                           ntca::ShutdownEventType::Value type,
                                           const bsls::TimeInterval& timeout)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            int rc = d_condition.timedWait(&d_mutex, timeout);
            if (rc == 0) {
                break;
            }
            else if (rc == NTCU_LISTENERSOCKETEVENTQUEUE_TIMEOUT) {
                return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }

        if (d_closed) {
            return ntsa::Error(ntsa::Error::e_CANCELLED);
        }

        for (Queue::iterator it = d_queue.begin(); it != d_queue.end(); ++it) {
            const ntca::ListenerSocketEvent& event = *it;
            if (event.isShutdownEvent() &&
                event.shutdownEvent().type() == type)
            {
                *result = event.shutdownEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error ListenerSocketEventQueue::wait(ntca::ErrorEvent* result)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            d_condition.wait(&d_mutex);
        }

        if (d_closed) {
            return ntsa::Error(ntsa::Error::e_CANCELLED);
        }

        for (Queue::iterator it = d_queue.begin(); it != d_queue.end(); ++it) {
            const ntca::ListenerSocketEvent& event = *it;
            if (event.isErrorEvent()) {
                *result = event.errorEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error ListenerSocketEventQueue::wait(ntca::ErrorEvent*         result,
                                           const bsls::TimeInterval& timeout)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            int rc = d_condition.timedWait(&d_mutex, timeout);
            if (rc == 0) {
                break;
            }
            else if (rc == NTCU_LISTENERSOCKETEVENTQUEUE_TIMEOUT) {
                return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }

        if (d_closed) {
            return ntsa::Error(ntsa::Error::e_CANCELLED);
        }

        for (Queue::iterator it = d_queue.begin(); it != d_queue.end(); ++it) {
            const ntca::ListenerSocketEvent& event = *it;
            if (event.isErrorEvent()) {
                *result = event.errorEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error ListenerSocketEventQueue::wait(ntca::ErrorEvent*           result,
                                           ntca::ErrorEventType::Value type)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            d_condition.wait(&d_mutex);
        }

        if (d_closed) {
            return ntsa::Error(ntsa::Error::e_CANCELLED);
        }

        for (Queue::iterator it = d_queue.begin(); it != d_queue.end(); ++it) {
            const ntca::ListenerSocketEvent& event = *it;
            if (event.isErrorEvent() && event.errorEvent().type() == type) {
                *result = event.errorEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error ListenerSocketEventQueue::wait(ntca::ErrorEvent*           result,
                                           ntca::ErrorEventType::Value type,
                                           const bsls::TimeInterval&   timeout)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            int rc = d_condition.timedWait(&d_mutex, timeout);
            if (rc == 0) {
                break;
            }
            else if (rc == NTCU_LISTENERSOCKETEVENTQUEUE_TIMEOUT) {
                return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }

        if (d_closed) {
            return ntsa::Error(ntsa::Error::e_CANCELLED);
        }

        for (Queue::iterator it = d_queue.begin(); it != d_queue.end(); ++it) {
            const ntca::ListenerSocketEvent& event = *it;
            if (event.isErrorEvent() && event.errorEvent().type() == type) {
                *result = event.errorEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

void ListenerSocketEventQueue::close()
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    if (!d_closed) {
        d_closed = true;
        d_condition.broadcast();
    }
}

}  // close package namespace
}  // close enterprise namespace
