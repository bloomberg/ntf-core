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

#ifndef INCLUDED_NTCU_LISTENERSOCKETEVENTQUEUE
#define INCLUDED_NTCU_LISTENERSOCKETEVENTQUEUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_listenersocketevent.h>
#include <ntccfg_platform.h>
#include <ntci_listenersocket.h>
#include <ntci_listenersocketsession.h>
#include <ntci_strand.h>
#include <ntcscm_version.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bsls_timeinterval.h>
#include <bsl_list.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcu {

/// @internal @brief Provide a queue of events passively detected for a
/// listener socket.
///
/// @details
/// Provide a mechanism that implements the
/// 'ntci::ListenerSocketSession' interface to operate as a blocking or
/// non-blocking queue of events passively detected for a listener socket.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcu
class ListenerSocketEventQueue : public ntci::ListenerSocketSession
{
    /// Define a type alias for a queue of events.
    typedef bsl::list<ntca::ListenerSocketEvent> Queue;

    enum Constant {
        // This enumeration defines the constants used by this class.

        /// The total number of listener socket event types.
        k_NUM_EVENT_TYPES = 4
    };

    ntccfg::ConditionMutex        d_mutex;
    ntccfg::Condition             d_condition;
    Queue                         d_queue;
    bsl::uint32_t                 d_interest[k_NUM_EVENT_TYPES];
    bool                          d_closed;
    bsl::shared_ptr<ntci::Strand> d_strand_sp;
    bslma::Allocator*             d_allocator_p;

  private:
    ListenerSocketEventQueue(const ListenerSocketEventQueue&)
        BSLS_KEYWORD_DELETED;
    ListenerSocketEventQueue& operator=(const ListenerSocketEventQueue&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Process the condition that accept queue flow control has been
    /// relaxed: the connections in the backlog are now being automatically
    /// accepted from the operating system and enqueued to the accept queue.
    void processAcceptQueueFlowControlRelaxed(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that accept queue flow control has been
    /// applied: the connections in the backlog are not being automatically
    /// accepted from the operating system and enqueued to the accept queue.
    void processAcceptQueueFlowControlApplied(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the accept queue is greater
    /// than or equal to the accept queue low watermark.
    void processAcceptQueueLowWatermark(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the accept queue is greater
    /// than the accept queue high watermark.
    void processAcceptQueueHighWatermark(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the accept queue has been discarded
    /// because a non-transient accept error asynchronously occured.
    void processAcceptQueueDiscarded(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the initiation of the shutdown sequence from the specified
    /// 'origin'.
    void processShutdownInitiated(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for reading.
    void processShutdownReceive(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for writing.
    void processShutdownSend(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the completion of the shutdown sequence.
    void processShutdownComplete(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process an error.
    void processError(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ErrorEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Return the strand on which this object's functions should be called.
    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if the user is interested in events of the specified
    /// 'type', otherwise return false.
    bool want(ntca::AcceptQueueEventType::Value type) const;

    /// Return true if the user is interested in events of the specified
    /// 'type', otherwise return false.
    bool want(ntca::ShutdownEventType::Value type) const;

    /// Return true if the user is interested in events of the specified
    /// 'type', otherwise return false.
    bool want(ntca::ErrorEventType::Value type) const;

  public:
    /// Create a new listener socket event queue. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit ListenerSocketEventQueue(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ListenerSocketEventQueue() BSLS_KEYWORD_OVERRIDE;

    void showAll();
    // Gain interest in all events.

    /// Gain interest in all events of the specified 'type'.
    void show(ntca::ListenerSocketEventType::Value type);

    /// Gain interest in events of the specified 'type'.
    void show(ntca::AcceptQueueEventType::Value type);

    /// Gain interest in events of the specified 'type'.
    void show(ntca::ShutdownEventType::Value type);

    /// Gain interest in events of the specified 'type'.
    void show(ntca::ErrorEventType::Value type);

    /// Lose interest in all events.
    void hideAll();

    /// Lose interest in all events of the specified 'type'.
    void hide(ntca::ListenerSocketEventType::Value type);

    /// Lose interest in events of the specified 'type'.
    void hide(ntca::AcceptQueueEventType::Value type);

    /// Lose interest in events of the specified 'type'.
    void hide(ntca::ShutdownEventType::Value type);

    /// Lose interest in events of the specified 'type'.
    void hide(ntca::ErrorEventType::Value type);

    /// Wait for any listener socket event to occur and load the result into
    /// the specified 'result'. Return the error.
    ntsa::Error wait(ntca::ListenerSocketEvent* result);

    /// Wait for any listener socket event to occur or until the specified
    /// 'timeout', in absolute time since the Unix epoch, elapses. Return
    /// the error.
    ntsa::Error wait(ntca::ListenerSocketEvent* result,
                     const bsls::TimeInterval&  timeout);

    /// Wait for any accept queue event to occur and load the result into
    /// the specified 'result'. Return the error.
    ntsa::Error wait(ntca::AcceptQueueEvent* result);

    /// Wait for any accept queue event to occur or until the specified
    /// 'timeout', in absolute time since the Unix epoch, elapses. Return
    /// the error.
    ntsa::Error wait(ntca::AcceptQueueEvent*   result,
                     const bsls::TimeInterval& timeout);

    /// Wait for a accept queue event of the specified 'type' to occur and
    /// load the result into the specified 'result'. Return the error.
    ntsa::Error wait(ntca::AcceptQueueEvent*           result,
                     ntca::AcceptQueueEventType::Value type);

    /// Wait for a accept queue event of the specified 'type' to occur or
    /// until the specified 'timeout', in absolute time since the Unix
    /// epoch, elapses. Return the error.
    ntsa::Error wait(ntca::AcceptQueueEvent*           result,
                     ntca::AcceptQueueEventType::Value type,
                     const bsls::TimeInterval&         timeout);

    /// Wait for any shutdown event to occur and load the result into
    /// the specified 'result'. Return the error.
    ntsa::Error wait(ntca::ShutdownEvent* result);

    /// Wait for any shutdown event to occur or until the specified
    /// 'timeout', in absolute time since the Unix epoch, elapses. Return
    /// the error.
    ntsa::Error wait(ntca::ShutdownEvent*      result,
                     const bsls::TimeInterval& timeout);

    /// Wait for a shutdown event of the specified 'type' to occur and
    /// load the result into the specified 'result'. Return the error.
    ntsa::Error wait(ntca::ShutdownEvent*           result,
                     ntca::ShutdownEventType::Value type);

    /// Wait for a shutdown event of the specified 'type' to occur or
    /// until the specified 'timeout', in absolute time since the Unix
    /// epoch, elapses. Return the error.
    ntsa::Error wait(ntca::ShutdownEvent*           result,
                     ntca::ShutdownEventType::Value type,
                     const bsls::TimeInterval&      timeout);

    /// Wait for any error event to occur and load the result into
    /// the specified 'result'. Return the error.
    ntsa::Error wait(ntca::ErrorEvent* result);

    /// Wait for any error event to occur or until the specified
    /// 'timeout', in absolute time since the Unix epoch, elapses. Return
    /// the error.
    ntsa::Error wait(ntca::ErrorEvent*         result,
                     const bsls::TimeInterval& timeout);

    /// Wait for a error event of the specified 'type' to occur and
    /// load the result into the specified 'result'. Return the error.
    ntsa::Error wait(ntca::ErrorEvent*           result,
                     ntca::ErrorEventType::Value type);

    /// Wait for a error event of the specified 'type' to occur or
    /// until the specified 'timeout', in absolute time since the Unix
    /// epoch, elapses. Return the error.
    ntsa::Error wait(ntca::ErrorEvent*           result,
                     ntca::ErrorEventType::Value type,
                     const bsls::TimeInterval&   timeout);

    /// Close the event queue and unblock any waiters.
    void close();
};

}  // close package namespace
}  // close enterprise namespace
#endif
