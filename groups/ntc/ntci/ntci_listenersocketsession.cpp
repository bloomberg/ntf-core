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

#include <ntci_listenersocketsession.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_listenersocketsession_cpp, "$Id$ $CSID$")

#include <ntci_log.h>

namespace BloombergLP {
namespace ntci {

// CREATOR
ListenerSocketSession::~ListenerSocketSession()
{
}

void ListenerSocketSession::processAcceptQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ListenerSocketSession::processAcceptQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ListenerSocketSession::processAcceptQueueLowWatermark(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ListenerSocketSession::processAcceptQueueHighWatermark(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ListenerSocketSession::processAcceptQueueDiscarded(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ListenerSocketSession::processAcceptQueueRateLimitApplied(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ListenerSocketSession::processAcceptQueueRateLimitRelaxed(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ListenerSocketSession::processShutdownInitiated(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ListenerSocketSession::processShutdownReceive(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ListenerSocketSession::processShutdownSend(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ListenerSocketSession::processShutdownComplete(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ListenerSocketSession::processError(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ErrorEvent&                      event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ListenerSocketSession::processConnectionRejectedLimitReached(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ConnectEvent& event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);
}

const bsl::shared_ptr<ntci::Strand>& ListenerSocketSession::strand() const
{
    return ntci::Strand::unspecified();
}

}  // close package namespace
}  // close enterprise namespace
