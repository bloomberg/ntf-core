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

#ifndef INCLUDED_NTCI_LISTENERSOCKETSESSION
#define INCLUDED_NTCI_LISTENERSOCKETSESSION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_acceptqueueevent.h>
#include <ntca_connectevent.h>
#include <ntca_errorevent.h>
#include <ntca_shutdownevent.h>
#include <ntccfg_platform.h>
#include <ntci_strand.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {
class ListenerSocket;
}
namespace ntci {

/// Provide an interface to detect events that passively occur during the
/// asynchronous operation of a listener socket.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_socket
class ListenerSocketSession
{
  public:
    /// Destroy this object.
    virtual ~ListenerSocketSession();

    /// Process the condition that accept queue flow control has been
    /// relaxed: the connections in the backlog are now being automatically
    /// accepted from the operating system and enqueued to the accept queue.
    virtual void processAcceptQueueFlowControlRelaxed(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent&                event);

    /// Process the condition that accept queue flow control has been
    /// applied: the connections in the backlog are not being automatically
    /// accepted from the operating system and enqueued to the accept queue.
    virtual void processAcceptQueueFlowControlApplied(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent&                event);

    /// Process the condition that the size of the accept queue is greater
    /// than or equal to the accept queue low watermark.
    virtual void processAcceptQueueLowWatermark(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent&                event);

    /// Process the condition that the size of the accept queue is greater
    /// than the accept queue high watermark.
    virtual void processAcceptQueueHighWatermark(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent&                event);

    /// Process the condition that the accept queue has been discarded
    /// because a non-transient accept error asynchronously occured.
    virtual void processAcceptQueueDiscarded(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent&                event);

    /// Process the condition that the accept queue rate limit has been reached
    /// and the connections in the backlog will not be automatically accepted.
    virtual void processAcceptQueueRateLimitApplied(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent&                event);

    /// Process the condition that the accept queue rate limit timer has fired
    /// and the connections in the backlog will be automatically accepted.
    virtual void processAcceptQueueRateLimitRelaxed(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent&                event);

    /// Process the initiation of the shutdown sequence from the specified
    /// 'origin'.
    virtual void processShutdownInitiated(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ShutdownEvent&                   event);

    /// Process the socket being shut down for reading.
    virtual void processShutdownReceive(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ShutdownEvent&                   event);

    /// Process the socket being shut down for writing.
    virtual void processShutdownSend(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ShutdownEvent&                   event);

    /// Process the completion of the shutdown sequence.
    virtual void processShutdownComplete(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ShutdownEvent&                   event);

    /// Process an error.
    virtual void processError(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ErrorEvent&                      event);

    /// Process indication of connection rejection due to established limit.
    virtual void processListenerSocketLimit(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ConnectEvent& event);

    /// Return the strand on which this object's functions should be called.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const;
};

}  // close package namespace
}  // close enterprise namespace
#endif
