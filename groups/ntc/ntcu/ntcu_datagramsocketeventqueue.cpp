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

#include <ntcu_datagramsocketeventqueue.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcu_datagramsocketeventqueue_cpp, "$Id$ $CSID$")

#include <ntci_log.h>

#include <bslmt_lockguard.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#if BSL_VERSION >= BSL_MAKE_VERSION(3, 80)
#define NTCU_DATAGRAMSOCKETEVENTQUEUE_TIMEOUT bslmt::Condition::e_TIMED_OUT
#else
#define NTCU_DATAGRAMSOCKETEVENTQUEUE_TIMEOUT -1
#endif

#define NTCU_DATAGRAMSOCKETEVENTQUEUE_LOG_EVENT(datagramSocket, event)        \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG                                                 \
            << "Datagram socket at " << datagramSocket->sourceEndpoint()      \
            << " to " << datagramSocket->remoteEndpoint()                     \
            << " announced event " << event << NTCI_LOG_STREAM_END;           \
    } while (false)

namespace BloombergLP {
namespace ntcu {

void DatagramSocketEventQueue::processReadQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);

    NTCI_LOG_CONTEXT();

    NTCU_DATAGRAMSOCKETEVENTQUEUE_LOG_EVENT(datagramSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() ==
                ntca::ReadQueueEventType::e_FLOW_CONTROL_RELAXED);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::DatagramSocketEvent(event));
        d_condition.signal();
    }
}

void DatagramSocketEventQueue::processReadQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);

    NTCI_LOG_CONTEXT();

    NTCU_DATAGRAMSOCKETEVENTQUEUE_LOG_EVENT(datagramSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() ==
                ntca::ReadQueueEventType::e_FLOW_CONTROL_APPLIED);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::DatagramSocketEvent(event));
        d_condition.signal();
    }
}

void DatagramSocketEventQueue::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);

    NTCI_LOG_CONTEXT();

    NTCU_DATAGRAMSOCKETEVENTQUEUE_LOG_EVENT(datagramSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() == ntca::ReadQueueEventType::e_LOW_WATERMARK);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::DatagramSocketEvent(event));
        d_condition.signal();
    }
}

void DatagramSocketEventQueue::processReadQueueHighWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);

    NTCI_LOG_CONTEXT();

    NTCU_DATAGRAMSOCKETEVENTQUEUE_LOG_EVENT(datagramSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() == ntca::ReadQueueEventType::e_HIGH_WATERMARK);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::DatagramSocketEvent(event));
        d_condition.signal();
    }
}

void DatagramSocketEventQueue::processReadQueueDiscarded(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);

    NTCI_LOG_CONTEXT();

    NTCU_DATAGRAMSOCKETEVENTQUEUE_LOG_EVENT(datagramSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() == ntca::ReadQueueEventType::e_DISCARDED);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::DatagramSocketEvent(event));
        d_condition.signal();
    }
}

void DatagramSocketEventQueue::processWriteQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);

    NTCI_LOG_CONTEXT();

    NTCU_DATAGRAMSOCKETEVENTQUEUE_LOG_EVENT(datagramSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() ==
                ntca::WriteQueueEventType::e_FLOW_CONTROL_RELAXED);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::DatagramSocketEvent(event));
        d_condition.signal();
    }
}

void DatagramSocketEventQueue::processWriteQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);

    NTCI_LOG_CONTEXT();

    NTCU_DATAGRAMSOCKETEVENTQUEUE_LOG_EVENT(datagramSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() ==
                ntca::WriteQueueEventType::e_FLOW_CONTROL_APPLIED);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::DatagramSocketEvent(event));
        d_condition.signal();
    }
}

void DatagramSocketEventQueue::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);

    NTCI_LOG_CONTEXT();

    NTCU_DATAGRAMSOCKETEVENTQUEUE_LOG_EVENT(datagramSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() == ntca::WriteQueueEventType::e_LOW_WATERMARK);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::DatagramSocketEvent(event));
        d_condition.signal();
    }
}

void DatagramSocketEventQueue::processWriteQueueHighWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);

    NTCI_LOG_CONTEXT();

    NTCU_DATAGRAMSOCKETEVENTQUEUE_LOG_EVENT(datagramSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() == ntca::WriteQueueEventType::e_HIGH_WATERMARK);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::DatagramSocketEvent(event));
        d_condition.signal();
    }
}

void DatagramSocketEventQueue::processWriteQueueDiscarded(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);

    NTCI_LOG_CONTEXT();

    NTCU_DATAGRAMSOCKETEVENTQUEUE_LOG_EVENT(datagramSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() == ntca::WriteQueueEventType::e_DISCARDED);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::DatagramSocketEvent(event));
        d_condition.signal();
    }
}

void DatagramSocketEventQueue::processShutdownInitiated(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);

    NTCI_LOG_CONTEXT();

    NTCU_DATAGRAMSOCKETEVENTQUEUE_LOG_EVENT(datagramSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() == ntca::ShutdownEventType::e_INITIATED);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::DatagramSocketEvent(event));
        d_condition.signal();
    }
}

void DatagramSocketEventQueue::processShutdownReceive(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);

    NTCI_LOG_CONTEXT();

    NTCU_DATAGRAMSOCKETEVENTQUEUE_LOG_EVENT(datagramSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() == ntca::ShutdownEventType::e_RECEIVE);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::DatagramSocketEvent(event));
        d_condition.signal();
    }
}

void DatagramSocketEventQueue::processShutdownSend(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);

    NTCI_LOG_CONTEXT();

    NTCU_DATAGRAMSOCKETEVENTQUEUE_LOG_EVENT(datagramSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() == ntca::ShutdownEventType::e_SEND);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::DatagramSocketEvent(event));
        d_condition.signal();
    }
}

void DatagramSocketEventQueue::processShutdownComplete(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);

    NTCI_LOG_CONTEXT();

    NTCU_DATAGRAMSOCKETEVENTQUEUE_LOG_EVENT(datagramSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    BSLS_ASSERT(event.type() == ntca::ShutdownEventType::e_COMPLETE);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::DatagramSocketEvent(event));
        d_condition.signal();
    }
}

void DatagramSocketEventQueue::processError(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ErrorEvent&                      event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);

    NTCI_LOG_CONTEXT();

    NTCU_DATAGRAMSOCKETEVENTQUEUE_LOG_EVENT(datagramSocket, event);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    if (!d_closed && this->want(event.type())) {
        d_queue.push_back(ntca::DatagramSocketEvent(event));
        d_condition.signal();
    }
}

const bsl::shared_ptr<ntci::Strand>& DatagramSocketEventQueue::strand() const
{
    return d_strand_sp;
}

bool DatagramSocketEventQueue::want(ntca::ReadQueueEventType::Value type) const
{
    BSLS_ASSERT_OPT(
        static_cast<int>(ntca::DatagramSocketEventType::e_READ_QUEUE) <
        k_NUM_EVENT_TYPES);

    const bsl::uint32_t interest = d_interest[static_cast<int>(
        ntca::DatagramSocketEventType::e_READ_QUEUE)];

    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    if ((interest & mask) != 0) {
        return true;
    }

    return false;
}

bool DatagramSocketEventQueue::want(
    ntca::WriteQueueEventType::Value type) const
{
    BSLS_ASSERT_OPT(
        static_cast<int>(ntca::DatagramSocketEventType::e_WRITE_QUEUE) <
        k_NUM_EVENT_TYPES);

    const bsl::uint32_t interest = d_interest[static_cast<int>(
        ntca::DatagramSocketEventType::e_WRITE_QUEUE)];

    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    if ((interest & mask) != 0) {
        return true;
    }

    return false;
}

bool DatagramSocketEventQueue::want(ntca::ShutdownEventType::Value type) const
{
    BSLS_ASSERT_OPT(
        static_cast<int>(ntca::DatagramSocketEventType::e_SHUTDOWN) <
        k_NUM_EVENT_TYPES);

    const bsl::uint32_t interest = d_interest[static_cast<int>(
        ntca::DatagramSocketEventType::e_SHUTDOWN)];

    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    if ((interest & mask) != 0) {
        return true;
    }

    return false;
}

bool DatagramSocketEventQueue::want(ntca::ErrorEventType::Value type) const
{
    BSLS_ASSERT_OPT(static_cast<int>(ntca::DatagramSocketEventType::e_ERROR) <
                    k_NUM_EVENT_TYPES);

    const bsl::uint32_t interest =
        d_interest[static_cast<int>(ntca::DatagramSocketEventType::e_ERROR)];

    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    if ((interest & mask) != 0) {
        return true;
    }

    return false;
}

DatagramSocketEventQueue::DatagramSocketEventQueue(
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

DatagramSocketEventQueue::~DatagramSocketEventQueue()
{
}

void DatagramSocketEventQueue::showAll()
{
    const bsl::uint32_t mask = static_cast<bsl::uint32_t>(-1);

    for (bsl::size_t i = 0; i < k_NUM_EVENT_TYPES; ++i) {
        d_interest[i] = mask;
    }
}

void DatagramSocketEventQueue::show(ntca::DatagramSocketEventType::Value type)
{
    BSLS_ASSERT_OPT(static_cast<int>(type) < k_NUM_EVENT_TYPES);

    const bsl::uint32_t mask = static_cast<bsl::uint32_t>(-1);

    d_interest[type] = mask;
}

void DatagramSocketEventQueue::show(ntca::ReadQueueEventType::Value type)
{
    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    d_interest[ntca::DatagramSocketEventType::e_READ_QUEUE] |= mask;
}

void DatagramSocketEventQueue::show(ntca::WriteQueueEventType::Value type)
{
    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    d_interest[ntca::DatagramSocketEventType::e_WRITE_QUEUE] |= mask;
}

void DatagramSocketEventQueue::show(ntca::ShutdownEventType::Value type)
{
    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    d_interest[ntca::DatagramSocketEventType::e_SHUTDOWN] |= mask;
}

void DatagramSocketEventQueue::show(ntca::ErrorEventType::Value type)
{
    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    d_interest[ntca::DatagramSocketEventType::e_ERROR] |= mask;
}

void DatagramSocketEventQueue::hideAll()
{
    for (bsl::size_t i = 0; i < k_NUM_EVENT_TYPES; ++i) {
        d_interest[i] = 0;
    }
}

void DatagramSocketEventQueue::hide(ntca::DatagramSocketEventType::Value type)
{
    BSLS_ASSERT_OPT(static_cast<int>(type) < k_NUM_EVENT_TYPES);

    d_interest[type] = 0;
}

void DatagramSocketEventQueue::hide(ntca::ReadQueueEventType::Value type)
{
    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    d_interest[ntca::DatagramSocketEventType::e_READ_QUEUE] &= ~mask;
}

void DatagramSocketEventQueue::hide(ntca::WriteQueueEventType::Value type)
{
    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    d_interest[ntca::DatagramSocketEventType::e_WRITE_QUEUE] &= ~mask;
}

void DatagramSocketEventQueue::hide(ntca::ShutdownEventType::Value type)
{
    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    d_interest[ntca::DatagramSocketEventType::e_SHUTDOWN] &= ~mask;
}

void DatagramSocketEventQueue::hide(ntca::ErrorEventType::Value type)
{
    const bsl::uint32_t mask =
        static_cast<bsl::uint32_t>(1 << static_cast<int>(type));

    d_interest[ntca::DatagramSocketEventType::e_ERROR] &= ~mask;
}

ntsa::Error DatagramSocketEventQueue::wait(ntca::DatagramSocketEvent* result)
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

ntsa::Error DatagramSocketEventQueue::wait(ntca::DatagramSocketEvent* result,
                                           const bsls::TimeInterval&  timeout)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    while (!d_closed && d_queue.empty()) {
        int rc = d_condition.timedWait(&d_mutex, timeout);
        if (rc == 0) {
            break;
        }
        else if (rc == NTCU_DATAGRAMSOCKETEVENTQUEUE_TIMEOUT) {
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

ntsa::Error DatagramSocketEventQueue::wait(ntca::ReadQueueEvent* result)
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
            const ntca::DatagramSocketEvent& event = *it;
            if (event.isReadQueueEvent()) {
                *result = event.readQueueEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error DatagramSocketEventQueue::wait(ntca::ReadQueueEvent*     result,
                                           const bsls::TimeInterval& timeout)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            int rc = d_condition.timedWait(&d_mutex, timeout);
            if (rc == 0) {
                break;
            }
            else if (rc == NTCU_DATAGRAMSOCKETEVENTQUEUE_TIMEOUT) {
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
            const ntca::DatagramSocketEvent& event = *it;
            if (event.isReadQueueEvent()) {
                *result = event.readQueueEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error DatagramSocketEventQueue::wait(
    ntca::ReadQueueEvent*           result,
    ntca::ReadQueueEventType::Value type)
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
            const ntca::DatagramSocketEvent& event = *it;
            if (event.isReadQueueEvent() &&
                event.readQueueEvent().type() == type)
            {
                *result = event.readQueueEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error DatagramSocketEventQueue::wait(
    ntca::ReadQueueEvent*           result,
    ntca::ReadQueueEventType::Value type,
    const bsls::TimeInterval&       timeout)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            int rc = d_condition.timedWait(&d_mutex, timeout);
            if (rc == 0) {
                break;
            }
            else if (rc == NTCU_DATAGRAMSOCKETEVENTQUEUE_TIMEOUT) {
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
            const ntca::DatagramSocketEvent& event = *it;
            if (event.isReadQueueEvent() &&
                event.readQueueEvent().type() == type)
            {
                *result = event.readQueueEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error DatagramSocketEventQueue::wait(ntca::WriteQueueEvent* result)
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
            const ntca::DatagramSocketEvent& event = *it;
            if (event.isWriteQueueEvent()) {
                *result = event.writeQueueEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error DatagramSocketEventQueue::wait(ntca::WriteQueueEvent*    result,
                                           const bsls::TimeInterval& timeout)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            int rc = d_condition.timedWait(&d_mutex, timeout);
            if (rc == 0) {
                break;
            }
            else if (rc == NTCU_DATAGRAMSOCKETEVENTQUEUE_TIMEOUT) {
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
            const ntca::DatagramSocketEvent& event = *it;
            if (event.isWriteQueueEvent()) {
                *result = event.writeQueueEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error DatagramSocketEventQueue::wait(
    ntca::WriteQueueEvent*           result,
    ntca::WriteQueueEventType::Value type)
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
            const ntca::DatagramSocketEvent& event = *it;
            if (event.isWriteQueueEvent() &&
                event.writeQueueEvent().type() == type)
            {
                *result = event.writeQueueEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error DatagramSocketEventQueue::wait(
    ntca::WriteQueueEvent*           result,
    ntca::WriteQueueEventType::Value type,
    const bsls::TimeInterval&        timeout)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            int rc = d_condition.timedWait(&d_mutex, timeout);
            if (rc == 0) {
                break;
            }
            else if (rc == NTCU_DATAGRAMSOCKETEVENTQUEUE_TIMEOUT) {
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
            const ntca::DatagramSocketEvent& event = *it;
            if (event.isWriteQueueEvent() &&
                event.writeQueueEvent().type() == type)
            {
                *result = event.writeQueueEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error DatagramSocketEventQueue::wait(ntca::ShutdownEvent* result)
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
            const ntca::DatagramSocketEvent& event = *it;
            if (event.isShutdownEvent()) {
                *result = event.shutdownEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error DatagramSocketEventQueue::wait(ntca::ShutdownEvent*      result,
                                           const bsls::TimeInterval& timeout)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            int rc = d_condition.timedWait(&d_mutex, timeout);
            if (rc == 0) {
                break;
            }
            else if (rc == NTCU_DATAGRAMSOCKETEVENTQUEUE_TIMEOUT) {
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
            const ntca::DatagramSocketEvent& event = *it;
            if (event.isShutdownEvent()) {
                *result = event.shutdownEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error DatagramSocketEventQueue::wait(ntca::ShutdownEvent* result,
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
            const ntca::DatagramSocketEvent& event = *it;
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

ntsa::Error DatagramSocketEventQueue::wait(ntca::ShutdownEvent* result,
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
            else if (rc == NTCU_DATAGRAMSOCKETEVENTQUEUE_TIMEOUT) {
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
            const ntca::DatagramSocketEvent& event = *it;
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

ntsa::Error DatagramSocketEventQueue::wait(ntca::ErrorEvent* result)
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
            const ntca::DatagramSocketEvent& event = *it;
            if (event.isErrorEvent()) {
                *result = event.errorEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error DatagramSocketEventQueue::wait(ntca::ErrorEvent*         result,
                                           const bsls::TimeInterval& timeout)
{
    while (true) {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (!d_closed && d_queue.empty()) {
            int rc = d_condition.timedWait(&d_mutex, timeout);
            if (rc == 0) {
                break;
            }
            else if (rc == NTCU_DATAGRAMSOCKETEVENTQUEUE_TIMEOUT) {
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
            const ntca::DatagramSocketEvent& event = *it;
            if (event.isErrorEvent()) {
                *result = event.errorEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error DatagramSocketEventQueue::wait(ntca::ErrorEvent*           result,
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
            const ntca::DatagramSocketEvent& event = *it;
            if (event.isErrorEvent() && event.errorEvent().type() == type) {
                *result = event.errorEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

ntsa::Error DatagramSocketEventQueue::wait(ntca::ErrorEvent*           result,
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
            else if (rc == NTCU_DATAGRAMSOCKETEVENTQUEUE_TIMEOUT) {
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
            const ntca::DatagramSocketEvent& event = *it;
            if (event.isErrorEvent() && event.errorEvent().type() == type) {
                *result = event.errorEvent();
                d_queue.erase(it);
                return ntsa::Error();
            }
        }
    }
}

void DatagramSocketEventQueue::close()
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    if (!d_closed) {
        d_closed = true;
        d_condition.broadcast();
    }
}

}  // close package namespace
}  // close enterprise namespace
