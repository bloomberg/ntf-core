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

#ifndef INCLUDED_NTCU_STREAMSOCKETEVENTQUEUE
#define INCLUDED_NTCU_STREAMSOCKETEVENTQUEUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_streamsocketevent.h>
#include <ntccfg_platform.h>
#include <ntci_strand.h>
#include <ntci_streamsocket.h>
#include <ntci_streamsocketmanager.h>
#include <ntci_streamsocketsession.h>
#include <ntcscm_version.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bsls_timeinterval.h>
#include <bsl_list.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcu {

/// @internal @brief Provide a queue of events passively detected for a
/// stream socket.
///
/// @details
/// Provide a mechanism that implements the
/// 'ntci::StreamSocketSession' interface to operate as a blocking or
/// non-blocking queue of events passively detected for a stream socket.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcu
class StreamSocketEventQueue : public ntci::StreamSocketSession,
                               public ntci::StreamSocketManager
{
    /// Define a type alias for a queue of events.
    typedef bsl::list<ntca::StreamSocketEvent> Queue;

    enum Constant {
        // This enumeration defines the constants used by this class.

        /// The total number of stream socket event types.
        k_NUM_EVENT_TYPES = 6
    };

    ntccfg::ConditionMutex        d_mutex;
    ntccfg::Condition             d_condition;
    Queue                         d_queue;
    bool                          d_established;
    bsl::uint32_t                 d_interest[k_NUM_EVENT_TYPES];
    bool                          d_closed;
    bsl::shared_ptr<ntci::Strand> d_strand_sp;
    bslma::Allocator*             d_allocator_p;

  private:
    StreamSocketEventQueue(const StreamSocketEventQueue&) BSLS_KEYWORD_DELETED;
    StreamSocketEventQueue& operator=(const StreamSocketEventQueue&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Process the establishment of the specified 'streamSocket'. Return
    /// the application protocol of the 'streamSocket'.
    void processStreamSocketEstablished(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
        BSLS_KEYWORD_OVERRIDE;

    /// Process the closure of the specified 'streamSocket'.
    void processStreamSocketClosed(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
        BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that read queue flow control has been relaxed:
    /// the socket receive buffer is being automatically copied to the read
    /// queue.
    void processReadQueueFlowControlRelaxed(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that read queue flow control has been applied:
    /// the socket receive buffer is not being automatically copied to the
    /// read queue.
    void processReadQueueFlowControlApplied(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the read queue is greater
    /// than or equal to the read queue low watermark.
    void processReadQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the read queue is greater
    /// than the read queue high watermark.
    void processReadQueueHighWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the read queue has been discarded
    /// because a non-transient read error asynchronously occurred.
    void processReadQueueDiscarded(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that write queue flow control has been
    /// relaxed: the write queue is being automatically copied to the socket
    /// send buffer.
    void processWriteQueueFlowControlRelaxed(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that write queue flow control has been
    /// applied: the write queue is not being automatically copied to the
    /// socket send buffer.
    void processWriteQueueFlowControlApplied(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue has been
    /// drained down to less than or equal to the write queue low watermark.
    /// This condition will not occur until the write queue high watermark
    /// condition occurs. The write queue low watermark conditions and the
    /// high watermark conditions are guaranteed to occur serially.
    void processWriteQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue is greater
    /// than the write queue high watermark. This condition will occur the
    /// first time the write queue high watermark has been reached but
    /// then will not subsequently occur until the write queue low
    /// watermark. The write queue low watermark conditions and the
    /// high watermark conditions are guaranteed to occur serially.
    void processWriteQueueHighWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the write queue has been discarded
    /// because a non-transient write error asynchronously occurred.
    void processWriteQueueDiscarded(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the initiation of a downgrade from encrypted to unencrypted
    /// communication.
    void processDowngradeInitiated(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::DowngradeEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the completion of a downgrade from encrypted to unencrypted
    /// communication.
    void processDowngradeComplete(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::DowngradeEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the initiation of the shutdown sequence from the specified
    /// 'origin'.
    void processShutdownInitiated(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for reading.
    void processShutdownReceive(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for writing.
    void processShutdownSend(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the completion of the shutdown sequence.
    void processShutdownComplete(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process an error.
    void processError(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                      const ntca::ErrorEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Return the strand on which this object's functions should be called.
    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if the user is interested in events of the specified
    /// 'type', otherwise return false.
    bool want(ntca::ReadQueueEventType::Value type) const;

    /// Return true if the user is interested in events of the specified
    /// 'type', otherwise return false.
    bool want(ntca::WriteQueueEventType::Value type) const;

    /// Return true if the user is interested in events of the specified
    /// 'type', otherwise return false.
    bool want(ntca::DowngradeEventType::Value type) const;

    /// Return true if the user is interested in events of the specified
    /// 'type', otherwise return false.
    bool want(ntca::ShutdownEventType::Value type) const;

    /// Return true if the user is interested in events of the specified
    /// 'type', otherwise return false.
    bool want(ntca::ErrorEventType::Value type) const;

  public:
    /// Create a new stream socket event queue. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit StreamSocketEventQueue(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~StreamSocketEventQueue() BSLS_KEYWORD_OVERRIDE;

    /// Gain interest in all events.
    void showAll();

    /// Gain interest in all events of the specified 'type'.
    void show(ntca::StreamSocketEventType::Value type);

    /// Gain interest in events of the specified 'type'.
    void show(ntca::ReadQueueEventType::Value type);

    /// Gain interest in events of the specified 'type'.
    void show(ntca::WriteQueueEventType::Value type);

    /// Gain interest in events of the specified 'type'.
    void show(ntca::DowngradeEventType::Value type);

    /// Gain interest in events of the specified 'type'.
    void show(ntca::ShutdownEventType::Value type);

    /// Gain interest in events of the specified 'type'.
    void show(ntca::ErrorEventType::Value type);

    /// Lose interest in all events.
    void hideAll();

    /// Lose interest in all events of the specified 'type'.
    void hide(ntca::StreamSocketEventType::Value type);

    /// Lose interest in events of the specified 'type'.
    void hide(ntca::ReadQueueEventType::Value type);

    /// Lose interest in events of the specified 'type'.
    void hide(ntca::WriteQueueEventType::Value type);

    /// Lose interest in events of the specified 'type'.
    void hide(ntca::DowngradeEventType::Value type);

    /// Lose interest in events of the specified 'type'.
    void hide(ntca::ShutdownEventType::Value type);

    /// Lose interest in events of the specified 'type'.
    void hide(ntca::ErrorEventType::Value type);

    /// Wait for any stream socket event to occur and load the result into
    /// the specified 'result'. Return the error.
    ntsa::Error wait(ntca::StreamSocketEvent* result);

    /// Wait for any stream socket event to occur or until the specified
    /// 'timeout', in absolute time since the Unix epoch, elapses. Return
    /// the error.
    ntsa::Error wait(ntca::StreamSocketEvent*  result,
                     const bsls::TimeInterval& timeout);

    /// Wait for any read queue event to occur and load the result into
    /// the specified 'result'. Return the error.
    ntsa::Error wait(ntca::ReadQueueEvent* result);

    /// Wait for any read queue event to occur or until the specified
    /// 'timeout', in absolute time since the Unix epoch, elapses. Return
    /// the error.
    ntsa::Error wait(ntca::ReadQueueEvent*     result,
                     const bsls::TimeInterval& timeout);

    /// Wait for a read queue event of the specified 'type' to occur and
    /// load the result into the specified 'result'. Return the error.
    ntsa::Error wait(ntca::ReadQueueEvent*           result,
                     ntca::ReadQueueEventType::Value type);

    /// Wait for a read queue event of the specified 'type' to occur or
    /// until the specified 'timeout', in absolute time since the Unix
    /// epoch, elapses. Return the error.
    ntsa::Error wait(ntca::ReadQueueEvent*           result,
                     ntca::ReadQueueEventType::Value type,
                     const bsls::TimeInterval&       timeout);

    /// Wait for any write queue event to occur and load the result into
    /// the specified 'result'. Return the error.
    ntsa::Error wait(ntca::WriteQueueEvent* result);

    /// Wait for any write queue event to occur or until the specified
    /// 'timeout', in absolute time since the Unix epoch, elapses. Return
    /// the error.
    ntsa::Error wait(ntca::WriteQueueEvent*    result,
                     const bsls::TimeInterval& timeout);

    /// Wait for a write queue event of the specified 'type' to occur and
    /// load the result into the specified 'result'. Return the error.
    ntsa::Error wait(ntca::WriteQueueEvent*           result,
                     ntca::WriteQueueEventType::Value type);

    /// Wait for a write queue event of the specified 'type' to occur or
    /// until the specified 'timeout', in absolute time since the Unix
    /// epoch, elapses. Return the error.
    ntsa::Error wait(ntca::WriteQueueEvent*           result,
                     ntca::WriteQueueEventType::Value type,
                     const bsls::TimeInterval&        timeout);

    /// Wait for any downgrade event to occur and load the result into
    /// the specified 'result'. Return the error.
    ntsa::Error wait(ntca::DowngradeEvent* result);

    /// Wait for any downgrade event to occur or until the specified
    /// 'timeout', in absolute time since the Unix epoch, elapses. Return
    /// the error.
    ntsa::Error wait(ntca::DowngradeEvent*     result,
                     const bsls::TimeInterval& timeout);

    /// Wait for a downgrade event of the specified 'type' to occur and
    /// load the result into the specified 'result'. Return the error.
    ntsa::Error wait(ntca::DowngradeEvent*           result,
                     ntca::DowngradeEventType::Value type);

    /// Wait for a downgrade event of the specified 'type' to occur or
    /// until the specified 'timeout', in absolute time since the Unix
    /// epoch, elapses. Return the error.
    ntsa::Error wait(ntca::DowngradeEvent*           result,
                     ntca::DowngradeEventType::Value type,
                     const bsls::TimeInterval&       timeout);

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
