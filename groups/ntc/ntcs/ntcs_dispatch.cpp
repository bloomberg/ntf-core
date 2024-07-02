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

#include <ntcs_dispatch.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_dispatch_cpp, "$Id$ $CSID$")

#include <ntccfg_bind.h>
#include <ntci_log.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bslmt_lockguard.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcs {

// *** Datagram Socket ***

void Dispatch::announceEstablished(
    const bsl::shared_ptr<ntci::DatagramSocketManager>& manager,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!manager) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketManager> managerGuard = manager;
        ntccfg::UnLockGuard                          guard(mutex);
        managerGuard->processDatagramSocketEstablished(socket);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::DatagramSocketManager::processDatagramSocketEstablished,
            manager,
            socket));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::DatagramSocketManager::processDatagramSocketEstablished,
            manager,
            socket));
    }
}

void Dispatch::announceClosed(
    const bsl::shared_ptr<ntci::DatagramSocketManager>& manager,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!manager) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketManager> managerGuard = manager;
        ntccfg::UnLockGuard                          guard(mutex);
        managerGuard->processDatagramSocketClosed(socket);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::DatagramSocketManager::processDatagramSocketClosed,
            manager,
            socket));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::DatagramSocketManager::processDatagramSocketClosed,
            manager,
            socket));
    }
}

void Dispatch::announceReadQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::ReadQueueEvent&                         event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processReadQueueFlowControlRelaxed(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processReadQueueFlowControlRelaxed,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processReadQueueFlowControlRelaxed,
            session,
            socket,
            event));
    }
}

void Dispatch::announceReadQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::ReadQueueEvent&                         event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processReadQueueFlowControlApplied(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processReadQueueFlowControlApplied,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processReadQueueFlowControlApplied,
            session,
            socket,
            event));
    }
}

void Dispatch::announceReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::ReadQueueEvent&                         event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processReadQueueLowWatermark(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processReadQueueLowWatermark,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processReadQueueLowWatermark,
            session,
            socket,
            event));
    }
}

void Dispatch::announceReadQueueHighWatermark(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::ReadQueueEvent&                         event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processReadQueueHighWatermark(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processReadQueueHighWatermark,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processReadQueueHighWatermark,
            session,
            socket,
            event));
    }
}

void Dispatch::announceReadQueueDiscarded(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::ReadQueueEvent&                         event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processReadQueueDiscarded(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processReadQueueDiscarded,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processReadQueueDiscarded,
            session,
            socket,
            event));
    }
}

void Dispatch::announceReadQueueRateLimitApplied(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::ReadQueueEvent&                         event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processReadQueueRateLimitApplied(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processReadQueueRateLimitApplied,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processReadQueueRateLimitApplied,
            session,
            socket,
            event));
    }
}

void Dispatch::announceReadQueueRateLimitRelaxed(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::ReadQueueEvent&                         event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processReadQueueRateLimitRelaxed(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processReadQueueRateLimitRelaxed,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processReadQueueRateLimitRelaxed,
            session,
            socket,
            event));
    }
}

void Dispatch::announceWriteQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::WriteQueueEvent&                        event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processWriteQueueFlowControlRelaxed(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processWriteQueueFlowControlRelaxed,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processWriteQueueFlowControlRelaxed,
            session,
            socket,
            event));
    }
}

void Dispatch::announceWriteQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::WriteQueueEvent&                        event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processWriteQueueFlowControlApplied(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processWriteQueueFlowControlApplied,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processWriteQueueFlowControlApplied,
            session,
            socket,
            event));
    }
}

void Dispatch::announceWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::WriteQueueEvent&                        event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processWriteQueueLowWatermark(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processWriteQueueLowWatermark,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processWriteQueueLowWatermark,
            session,
            socket,
            event));
    }
}

void Dispatch::announceWriteQueueHighWatermark(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::WriteQueueEvent&                        event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processWriteQueueHighWatermark(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processWriteQueueHighWatermark,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processWriteQueueHighWatermark,
            session,
            socket,
            event));
    }
}

void Dispatch::announceWriteQueueDiscarded(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::WriteQueueEvent&                        event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processWriteQueueDiscarded(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processWriteQueueDiscarded,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processWriteQueueDiscarded,
            session,
            socket,
            event));
    }
}

void Dispatch::announceWriteQueueRateLimitApplied(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::WriteQueueEvent&                        event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processWriteQueueRateLimitApplied(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processWriteQueueRateLimitApplied,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processWriteQueueRateLimitApplied,
            session,
            socket,
            event));
    }
}

void Dispatch::announceWriteQueueRateLimitRelaxed(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::WriteQueueEvent&                        event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processWriteQueueRateLimitRelaxed(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processWriteQueueRateLimitRelaxed,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::DatagramSocketSession::processWriteQueueRateLimitRelaxed,
            session,
            socket,
            event));
    }
}

void Dispatch::announceShutdownInitiated(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::ShutdownEvent&                          event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processShutdownInitiated(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::DatagramSocketSession::processShutdownInitiated,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::DatagramSocketSession::processShutdownInitiated,
                        session,
                        socket,
                        event));
    }
}

void Dispatch::announceShutdownReceive(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::ShutdownEvent&                          event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processShutdownReceive(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::DatagramSocketSession::processShutdownReceive,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::DatagramSocketSession::processShutdownReceive,
                        session,
                        socket,
                        event));
    }
}

void Dispatch::announceShutdownSend(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::ShutdownEvent&                          event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processShutdownSend(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::DatagramSocketSession::processShutdownSend,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::DatagramSocketSession::processShutdownSend,
                        session,
                        socket,
                        event));
    }
}

void Dispatch::announceShutdownComplete(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::ShutdownEvent&                          event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processShutdownComplete(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::DatagramSocketSession::processShutdownComplete,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::DatagramSocketSession::processShutdownComplete,
                        session,
                        socket,
                        event));
    }
}

void Dispatch::announceError(
    const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
    const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
    const ntca::ErrorEvent&                             event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::DatagramSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processError(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::DatagramSocketSession::processError,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::DatagramSocketSession::processError,
                        session,
                        socket,
                        event));
    }
}

// *** Listener Socket ***

void Dispatch::announceEstablished(
    const bsl::shared_ptr<ntci::ListenerSocketManager>& manager,
    const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!manager) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::ListenerSocketManager> managerGuard = manager;
        ntccfg::UnLockGuard                          guard(mutex);
        managerGuard->processListenerSocketEstablished(socket);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::ListenerSocketManager::processListenerSocketEstablished,
            manager,
            socket));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::ListenerSocketManager::processListenerSocketEstablished,
            manager,
            socket));
    }
}

void Dispatch::announceClosed(
    const bsl::shared_ptr<ntci::ListenerSocketManager>& manager,
    const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!manager) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::ListenerSocketManager> managerGuard = manager;
        ntccfg::UnLockGuard                          guard(mutex);
        managerGuard->processListenerSocketClosed(socket);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::ListenerSocketManager::processListenerSocketClosed,
            manager,
            socket));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::ListenerSocketManager::processListenerSocketClosed,
            manager,
            socket));
    }
}

void Dispatch::announceAcceptQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
    const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
    const ntca::AcceptQueueEvent&                       event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::ListenerSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processAcceptQueueFlowControlRelaxed(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::ListenerSocketSession::processAcceptQueueFlowControlRelaxed,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::ListenerSocketSession::processAcceptQueueFlowControlRelaxed,
            session,
            socket,
            event));
    }
}

void Dispatch::announceAcceptQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
    const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
    const ntca::AcceptQueueEvent&                       event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::ListenerSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processAcceptQueueFlowControlApplied(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::ListenerSocketSession::processAcceptQueueFlowControlApplied,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::ListenerSocketSession::processAcceptQueueFlowControlApplied,
            session,
            socket,
            event));
    }
}

void Dispatch::announceAcceptQueueLowWatermark(
    const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
    const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
    const ntca::AcceptQueueEvent&                       event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::ListenerSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processAcceptQueueLowWatermark(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::ListenerSocketSession::processAcceptQueueLowWatermark,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::ListenerSocketSession::processAcceptQueueLowWatermark,
            session,
            socket,
            event));
    }
}

void Dispatch::announceAcceptQueueHighWatermark(
    const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
    const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
    const ntca::AcceptQueueEvent&                       event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::ListenerSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processAcceptQueueHighWatermark(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::ListenerSocketSession::processAcceptQueueHighWatermark,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::ListenerSocketSession::processAcceptQueueHighWatermark,
            session,
            socket,
            event));
    }
}

void Dispatch::announceAcceptQueueDiscarded(
    const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
    const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
    const ntca::AcceptQueueEvent&                       event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::ListenerSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processAcceptQueueDiscarded(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::ListenerSocketSession::processAcceptQueueDiscarded,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::ListenerSocketSession::processAcceptQueueDiscarded,
            session,
            socket,
            event));
    }
}

void Dispatch::announceAcceptQueueRateLimitApplied(
    const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
    const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
    const ntca::AcceptQueueEvent&                       event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::ListenerSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processAcceptQueueRateLimitApplied(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::ListenerSocketSession::processAcceptQueueRateLimitApplied,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::ListenerSocketSession::processAcceptQueueRateLimitApplied,
            session,
            socket,
            event));
    }
}

void Dispatch::announceAcceptQueueRateLimitRelaxed(
    const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
    const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
    const ntca::AcceptQueueEvent&                       event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::ListenerSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processAcceptQueueRateLimitRelaxed(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::ListenerSocketSession::processAcceptQueueRateLimitRelaxed,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::ListenerSocketSession::processAcceptQueueRateLimitRelaxed,
            session,
            socket,
            event));
    }
}

void Dispatch::announceConnectionLimit(
    const bsl::shared_ptr<ntci::ListenerSocketManager>& manager,
    const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!manager) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::ListenerSocketManager> managerGuard = manager;
        ntccfg::UnLockGuard                          guard(mutex);
        managerGuard->processListenerSocketLimit(socket);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::ListenerSocketManager::processListenerSocketLimit,
            manager,
            socket));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::ListenerSocketManager::processListenerSocketLimit,
            manager,
            socket));
    }
}

void Dispatch::announceShutdownInitiated(
    const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
    const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
    const ntca::ShutdownEvent&                          event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::ListenerSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processShutdownInitiated(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::ListenerSocketSession::processShutdownInitiated,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::ListenerSocketSession::processShutdownInitiated,
                        session,
                        socket,
                        event));
    }
}

void Dispatch::announceShutdownReceive(
    const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
    const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
    const ntca::ShutdownEvent&                          event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::ListenerSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processShutdownReceive(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::ListenerSocketSession::processShutdownReceive,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::ListenerSocketSession::processShutdownReceive,
                        session,
                        socket,
                        event));
    }
}

void Dispatch::announceShutdownSend(
    const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
    const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
    const ntca::ShutdownEvent&                          event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::ListenerSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processShutdownSend(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::ListenerSocketSession::processShutdownSend,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::ListenerSocketSession::processShutdownSend,
                        session,
                        socket,
                        event));
    }
}

void Dispatch::announceShutdownComplete(
    const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
    const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
    const ntca::ShutdownEvent&                          event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::ListenerSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processShutdownComplete(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::ListenerSocketSession::processShutdownComplete,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::ListenerSocketSession::processShutdownComplete,
                        session,
                        socket,
                        event));
    }
}

void Dispatch::announceError(
    const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
    const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
    const ntca::ErrorEvent&                             event,
    const bsl::shared_ptr<ntci::Strand>&                destination,
    const bsl::shared_ptr<ntci::Strand>&                source,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bool                                                defer,
    ntccfg::Mutex*                                      mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::ListenerSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                          guard(mutex);
        sessionGuard->processError(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::ListenerSocketSession::processError,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::ListenerSocketSession::processError,
                        session,
                        socket,
                        event));
    }
}

// *** Stream Socket ***

void Dispatch::announceEstablished(
    const bsl::shared_ptr<ntci::StreamSocketManager>& manager,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!manager) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketManager> managerGuard = manager;
        ntccfg::UnLockGuard                        guard(mutex);
        managerGuard->processStreamSocketEstablished(socket);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::StreamSocketManager::processStreamSocketEstablished,
            manager,
            socket));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::StreamSocketManager::processStreamSocketEstablished,
            manager,
            socket));
    }
}

void Dispatch::announceClosed(
    const bsl::shared_ptr<ntci::StreamSocketManager>& manager,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!manager) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketManager> managerGuard = manager;
        ntccfg::UnLockGuard                        guard(mutex);
        managerGuard->processStreamSocketClosed(socket);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::StreamSocketManager::processStreamSocketClosed,
                        manager,
                        socket));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::StreamSocketManager::processStreamSocketClosed,
                        manager,
                        socket));
    }
}

void Dispatch::announceReadQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::ReadQueueEvent&                       event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processReadQueueFlowControlRelaxed(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processReadQueueFlowControlRelaxed,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processReadQueueFlowControlRelaxed,
            session,
            socket,
            event));
    }
}

void Dispatch::announceReadQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::ReadQueueEvent&                       event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processReadQueueFlowControlApplied(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processReadQueueFlowControlApplied,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processReadQueueFlowControlApplied,
            session,
            socket,
            event));
    }
}

void Dispatch::announceReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::ReadQueueEvent&                       event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processReadQueueLowWatermark(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processReadQueueLowWatermark,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processReadQueueLowWatermark,
            session,
            socket,
            event));
    }
}

void Dispatch::announceReadQueueHighWatermark(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::ReadQueueEvent&                       event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processReadQueueHighWatermark(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processReadQueueHighWatermark,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processReadQueueHighWatermark,
            session,
            socket,
            event));
    }
}

void Dispatch::announceReadQueueDiscarded(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::ReadQueueEvent&                       event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processReadQueueDiscarded(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::StreamSocketSession::processReadQueueDiscarded,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::StreamSocketSession::processReadQueueDiscarded,
                        session,
                        socket,
                        event));
    }
}

void Dispatch::announceReadQueueRateLimitApplied(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::ReadQueueEvent&                       event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processReadQueueRateLimitApplied(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processReadQueueRateLimitApplied,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processReadQueueRateLimitApplied,
            session,
            socket,
            event));
    }
}

void Dispatch::announceReadQueueRateLimitRelaxed(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::ReadQueueEvent&                       event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processReadQueueRateLimitRelaxed(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processReadQueueRateLimitRelaxed,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processReadQueueRateLimitRelaxed,
            session,
            socket,
            event));
    }
}

void Dispatch::announceWriteQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::WriteQueueEvent&                      event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processWriteQueueFlowControlRelaxed(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processWriteQueueFlowControlRelaxed,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processWriteQueueFlowControlRelaxed,
            session,
            socket,
            event));
    }
}

void Dispatch::announceWriteQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::WriteQueueEvent&                      event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processWriteQueueFlowControlApplied(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processWriteQueueFlowControlApplied,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processWriteQueueFlowControlApplied,
            session,
            socket,
            event));
    }
}

void Dispatch::announceWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::WriteQueueEvent&                      event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processWriteQueueLowWatermark(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processWriteQueueLowWatermark,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processWriteQueueLowWatermark,
            session,
            socket,
            event));
    }
}

void Dispatch::announceWriteQueueHighWatermark(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::WriteQueueEvent&                      event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processWriteQueueHighWatermark(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processWriteQueueHighWatermark,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processWriteQueueHighWatermark,
            session,
            socket,
            event));
    }
}

void Dispatch::announceWriteQueueDiscarded(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::WriteQueueEvent&                      event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processWriteQueueDiscarded(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::StreamSocketSession::processWriteQueueDiscarded,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::StreamSocketSession::processWriteQueueDiscarded,
                        session,
                        socket,
                        event));
    }
}

void Dispatch::announceWriteQueueRateLimitApplied(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::WriteQueueEvent&                      event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processWriteQueueRateLimitApplied(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processWriteQueueRateLimitApplied,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processWriteQueueRateLimitApplied,
            session,
            socket,
            event));
    }
}

void Dispatch::announceWriteQueueRateLimitRelaxed(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::WriteQueueEvent&                      event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processWriteQueueRateLimitRelaxed(socket, event);
    }
    else if (destination) {
        destination->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processWriteQueueRateLimitRelaxed,
            session,
            socket,
            event));
    }
    else {
        executor->execute(NTCCFG_BIND(
            &ntci::StreamSocketSession::processWriteQueueRateLimitRelaxed,
            session,
            socket,
            event));
    }
}

void Dispatch::announceDowngradeInitiated(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::DowngradeEvent&                       event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processDowngradeInitiated(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::StreamSocketSession::processDowngradeInitiated,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::StreamSocketSession::processDowngradeInitiated,
                        session,
                        socket,
                        event));
    }
}

void Dispatch::announceDowngradeComplete(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::DowngradeEvent&                       event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processDowngradeComplete(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::StreamSocketSession::processDowngradeComplete,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::StreamSocketSession::processDowngradeComplete,
                        session,
                        socket,
                        event));
    }
}

void Dispatch::announceShutdownInitiated(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::ShutdownEvent&                        event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processShutdownInitiated(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::StreamSocketSession::processShutdownInitiated,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::StreamSocketSession::processShutdownInitiated,
                        session,
                        socket,
                        event));
    }
}

void Dispatch::announceShutdownReceive(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::ShutdownEvent&                        event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processShutdownReceive(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::StreamSocketSession::processShutdownReceive,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::StreamSocketSession::processShutdownReceive,
                        session,
                        socket,
                        event));
    }
}

void Dispatch::announceShutdownSend(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::ShutdownEvent&                        event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processShutdownSend(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::StreamSocketSession::processShutdownSend,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::StreamSocketSession::processShutdownSend,
                        session,
                        socket,
                        event));
    }
}

void Dispatch::announceShutdownComplete(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::ShutdownEvent&                        event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processShutdownComplete(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::StreamSocketSession::processShutdownComplete,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::StreamSocketSession::processShutdownComplete,
                        session,
                        socket,
                        event));
    }
}

void Dispatch::announceError(
    const bsl::shared_ptr<ntci::StreamSocketSession>& session,
    const bsl::shared_ptr<ntci::StreamSocket>&        socket,
    const ntca::ErrorEvent&                           event,
    const bsl::shared_ptr<ntci::Strand>&              destination,
    const bsl::shared_ptr<ntci::Strand>&              source,
    const bsl::shared_ptr<ntci::Executor>&            executor,
    bool                                              defer,
    ntccfg::Mutex*                                    mutex)
{
    if (!session) {
        return;
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        bsl::shared_ptr<ntci::StreamSocketSession> sessionGuard = session;
        ntccfg::UnLockGuard                        guard(mutex);
        sessionGuard->processError(socket, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::StreamSocketSession::processError,
                        session,
                        socket,
                        event));
    }
    else {
        executor->execute(NTCCFG_BIND(&ntci::StreamSocketSession::processError,
                                      session,
                                      socket,
                                      event));
    }
}

// *** Proactor Socket ***

void Dispatch::announceAccepted(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    const ntsa::Error&                           error,
    const bsl::shared_ptr<ntsi::StreamSocket>&   streamSocket,
    const bsl::shared_ptr<ntci::Strand>&         destination)
{
    if (NTCCFG_LIKELY(!destination)) {
        socket->processSocketAccepted(error, streamSocket);
    }
    else {
        destination->execute(
            NTCCFG_BIND(&ntci::ProactorSocket::processSocketAccepted,
                        socket,
                        error,
                        streamSocket));
    }
}

void Dispatch::announceConnected(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    const ntsa::Error&                           error,
    const bsl::shared_ptr<ntci::Strand>&         destination)
{
    if (NTCCFG_LIKELY(!destination)) {
        socket->processSocketConnected(error);
    }
    else {
        destination->execute(
            NTCCFG_BIND(&ntci::ProactorSocket::processSocketConnected,
                        socket,
                        error));
    }
}

void Dispatch::announceReceived(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    const ntsa::Error&                           error,
    const ntsa::ReceiveContext&                  context,
    const bsl::shared_ptr<ntci::Strand>&         destination)
{
    if (NTCCFG_LIKELY(!destination)) {
        socket->processSocketReceived(error, context);
    }
    else {
        destination->execute(
            NTCCFG_BIND(&ntci::ProactorSocket::processSocketReceived,
                        socket,
                        error,
                        context));
    }
}

void Dispatch::announceSent(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    const ntsa::Error&                           error,
    const ntsa::SendContext&                     context,
    const bsl::shared_ptr<ntci::Strand>&         destination)
{
    if (NTCCFG_LIKELY(!destination)) {
        socket->processSocketSent(error, context);
    }
    else {
        destination->execute(
            NTCCFG_BIND(&ntci::ProactorSocket::processSocketSent,
                        socket,
                        error,
                        context));
    }
}

void Dispatch::announceError(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    const ntsa::Error&                           error,
    const bsl::shared_ptr<ntci::Strand>&         destination)
{
    if (NTCCFG_LIKELY(!destination)) {
        socket->processSocketError(error);
    }
    else {
        destination->execute(
            NTCCFG_BIND(&ntci::ProactorSocket::processSocketError,
                        socket,
                        error));
    }
}

void Dispatch::announceDetached(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    const bsl::shared_ptr<ntci::Strand>&         destination)
{
    if (NTCCFG_LIKELY(!destination)) {
        socket->processSocketDetached();
    }
    else {
        destination->execute(
            NTCCFG_BIND(&ntci::ProactorSocket::processSocketDetached, socket));
    }
}

// *** Timer ***

void Dispatch::announceDeadline(
    const bsl::shared_ptr<ntci::TimerSession>& session,
    const bsl::shared_ptr<ntci::Timer>&        timer,
    const ntca::TimerEvent&                    event,
    const bsl::shared_ptr<ntci::Strand>&       destination,
    const bsl::shared_ptr<ntci::Strand>&       source,
    const bsl::shared_ptr<ntci::Executor>&     executor,
    bool                                       defer)
{
    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        session->processTimerDeadline(timer, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::TimerSession::processTimerDeadline,
                        session,
                        timer,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::TimerSession::processTimerDeadline,
                        session,
                        timer,
                        event));
    }
}

void Dispatch::announceCancelled(
    const bsl::shared_ptr<ntci::TimerSession>& session,
    const bsl::shared_ptr<ntci::Timer>&        timer,
    const ntca::TimerEvent&                    event,
    const bsl::shared_ptr<ntci::Strand>&       destination,
    const bsl::shared_ptr<ntci::Strand>&       source,
    const bsl::shared_ptr<ntci::Executor>&     executor,
    bool                                       defer)
{
    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        session->processTimerCancelled(timer, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::TimerSession::processTimerCancelled,
                        session,
                        timer,
                        event));
    }
    else {
        executor->execute(
            NTCCFG_BIND(&ntci::TimerSession::processTimerCancelled,
                        session,
                        timer,
                        event));
    }
}

void Dispatch::announceRemoval(
    const bsl::shared_ptr<ntci::TimerSession>& session,
    const bsl::shared_ptr<ntci::Timer>&        timer,
    const ntca::TimerEvent&                    event,
    const bsl::shared_ptr<ntci::Strand>&       destination,
    const bsl::shared_ptr<ntci::Strand>&       source,
    const bsl::shared_ptr<ntci::Executor>&     executor,
    bool                                       defer)
{
    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(destination, source)))
    {
        session->processTimerClosed(timer, event);
    }
    else if (destination) {
        destination->execute(
            NTCCFG_BIND(&ntci::TimerSession::processTimerClosed,
                        session,
                        timer,
                        event));
    }
    else {
        executor->execute(NTCCFG_BIND(&ntci::TimerSession::processTimerClosed,
                                      session,
                                      timer,
                                      event));
    }
}

}  // close package namespace
}  // close enterprise namespace
