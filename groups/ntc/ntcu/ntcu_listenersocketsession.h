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

#ifndef INCLUDED_NTCU_LISTENERSOCKETSESSION
#define INCLUDED_NTCU_LISTENERSOCKETSESSION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_listenersocketevent.h>
#include <ntccfg_platform.h>
#include <ntci_listenersocket.h>
#include <ntci_listenersocketsession.h>
#include <ntci_strand.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcu {

/// @internal @brief
/// Adapt a listener socket session to a callback.
///
/// @details
/// Provide a mechanism that implements the
/// 'ntci::ListenerSocketSession' interface to invoke a callback with
/// potentially bound state.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcu
class ListenerSocketSession : public ntci::ListenerSocketSession
{
    ntci::ListenerSocket::SessionCallback d_callback;
    bsl::shared_ptr<ntci::Strand>         d_strand_sp;
    bslma::Allocator*                     d_allocator_p;

  private:
    ListenerSocketSession(const ListenerSocketSession&) BSLS_KEYWORD_DELETED;
    ListenerSocketSession& operator=(const ListenerSocketSession&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new listener socket session that proxies the announcement of
    /// listener socket events to the specified 'callback' on the specified
    /// 'strand'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    ListenerSocketSession(
        const ntci::ListenerSocket::SessionCallback& callback,
        const bsl::shared_ptr<ntci::Strand>&         strand,
        bslma::Allocator*                            basicAllocator = 0);

    /// Destroy this object.
    ~ListenerSocketSession() BSLS_KEYWORD_OVERRIDE;

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
};

}  // close package namespace
}  // close enterprise namespace
#endif
