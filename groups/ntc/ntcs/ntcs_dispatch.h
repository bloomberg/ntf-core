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

#ifndef INCLUDED_NTCS_DISPATCH
#define INCLUDED_NTCS_DISPATCH

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_datagramsocketmanager.h>
#include <ntci_datagramsocketsession.h>
#include <ntci_executor.h>
#include <ntci_listenersocketmanager.h>
#include <ntci_listenersocketsession.h>
#include <ntci_proactor.h>
#include <ntci_proactorsocket.h>
#include <ntci_reactor.h>
#include <ntci_reactorsocket.h>
#include <ntci_strand.h>
#include <ntci_streamsocketmanager.h>
#include <ntci_streamsocketsession.h>
#include <ntci_timer.h>
#include <ntci_timersession.h>
#include <ntcscm_version.h>
#include <bslmt_mutex.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide a utility to dispatch the announcement of socket events.
///
/// @details
/// Provide utilities that internally correctly decide how
/// a socket event must be announced. Socket events may be announced either
/// immediately, or must be deferred to execute on a particular strand, or,
/// in some cases, must be forced to be announced asynchonously.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcs
struct Dispatch {
    // *** Datagram Socket ***

    /// Announce to the specified 'manager' the establishment of the
    /// specified 'socket'. If the specified 'defer' flag is false and the
    /// requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceEstablished(
        const bsl::shared_ptr<ntci::DatagramSocketManager>& manager,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'manager' the closure of the specified
    /// 'socket'. If the specified 'defer' flag is false and the
    /// requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceClosed(
        const bsl::shared_ptr<ntci::DatagramSocketManager>& manager,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition that read queue
    /// flow control has been relaxed. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceReadQueueFlowControlRelaxed(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::ReadQueueEvent&                         event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition that read queue
    /// flow control has been applied. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceReadQueueFlowControlApplied(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::ReadQueueEvent&                         event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition of the specified
    /// 'socket' that the size of the read queue is greater than or equal to
    /// the read queue low watermark. If the specified 'defer' flag is false
    /// and the requirements of the specified 'destination' strand permits
    /// the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceReadQueueLowWatermark(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::ReadQueueEvent&                         event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition of the specified
    /// 'socket' that the size of the read queue is greater than the read
    /// queue high watermark. If the specified 'defer' flag is false and the
    /// requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceReadQueueHighWatermark(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::ReadQueueEvent&                         event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition of the specified
    /// 'socket' that the read queue has been discarded because
    /// a non-transient read error asynchronously occured. If the specified
    /// 'defer' flag is false and the requirements of the specified
    /// 'destination' strand permits the announcement to be executed
    /// immediately by the specified 'source' strand, unlock the specified
    /// 'mutex', execute the announcement, then relock the 'mutex'.
    /// Otherwise, enqueue the announcement to be executed on the
    /// 'destination' strand, if not null, or by the specified 'executor'
    /// otherwise. The behavior is undefined if 'mutex' is null or not
    /// locked. The behavior is *not* undefined if either the 'destination'
    /// strand is null or the 'source' strand is null; a null 'destination'
    /// strand indicates the announcement may be invoked on any strand by
    /// any thread; a null 'source' strand indicates the source strand is
    /// unknown.
    static void announceReadQueueDiscarded(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::ReadQueueEvent&                         event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition that read queue
    /// rate limit has been reached. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceReadQueueRateLimitApplied(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::ReadQueueEvent&                         event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition that read queue
    /// rate limit timer has fired. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceReadQueueRateLimitRelaxed(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::ReadQueueEvent&                         event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition that write queue
    /// flow control has been relaxed. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceWriteQueueFlowControlRelaxed(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::WriteQueueEvent&                        event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition that write queue
    /// flow control has been applied. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceWriteQueueFlowControlApplied(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::WriteQueueEvent&                        event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition of the specified
    /// 'socket' that the size of the write queue is less than or equal to
    /// the write queue low watermark. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceWriteQueueLowWatermark(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::WriteQueueEvent&                        event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition of the specified
    /// 'socket' that the size of the write queue is greater than the write
    /// queue high watermark. If the specified 'defer' flag is false and the
    /// requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceWriteQueueHighWatermark(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::WriteQueueEvent&                        event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the  condition of the specified
    /// 'socket' that the write queue has been discarded because
    /// a non-transient write error asynchronously occured. If the specified
    /// 'defer' flag is false and the requirements of the specified
    /// 'destination' strand permits the announcement to be executed
    /// immediately by the specified 'source' strand, unlock the specified
    /// 'mutex', execute the announcement, then relock the 'mutex'.
    /// Otherwise, enqueue the announcement to be executed on the
    /// 'destination' strand, if not null, or by the specified 'executor'
    /// otherwise. The behavior is undefined if 'mutex' is null or not
    /// locked. The behavior is *not* undefined if either the 'destination'
    /// strand is null or the 'source' strand is null; a null 'destination'
    /// strand indicates the announcement may be invoked on any strand by
    /// any thread; a null 'source' strand indicates the source strand is
    /// unknown.
    static void announceWriteQueueDiscarded(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::WriteQueueEvent&                        event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition that write queue
    /// rate limit has been reached. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceWriteQueueRateLimitApplied(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::WriteQueueEvent&                        event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition that write queue
    /// rate limit timer has fired. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceWriteQueueRateLimitRelaxed(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::WriteQueueEvent&                        event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the initiation of the shutdown
    /// sequence of the specified 'socket' from the specified 'origin'. If
    /// the specified 'defer' flag is false and the requirements of the
    /// specified 'destination' strand permits the announcement to be
    /// executed immediately by the specified 'source' strand, unlock the
    /// specified 'mutex', execute the announcement, then relock the
    /// 'mutex'. Otherwise, enqueue the announcement to be executed on the
    /// 'destination' strand, if not null, or by the specified 'executor'
    /// otherwise. The behavior is undefined if 'mutex' is null or not
    /// locked. The behavior is *not* undefined if either the 'destination'
    /// strand is null or the 'source' strand is null; a null 'destination'
    /// strand indicates the announcement may be invoked on any strand by
    /// any thread; a null 'source' strand indicates the source strand is
    /// unknown.
    static void announceShutdownInitiated(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::ShutdownEvent&                          event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' that the specified 'socket' is
    /// shut down for reading. If the specified 'defer' flag is false and
    /// the requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceShutdownReceive(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::ShutdownEvent&                          event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' that the specified 'socket' is
    /// shut down for writing. If the specified 'defer' flag is false and
    /// the requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceShutdownSend(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::ShutdownEvent&                          event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the shutdown sequence of the
    /// specified 'socket' has completed. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceShutdownComplete(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::ShutdownEvent&                          event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the detection of an error for
    /// the specified 'socket'. If the specified 'defer' flag is false and
    /// the requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceError(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session,
        const bsl::shared_ptr<ntci::DatagramSocket>&        socket,
        const ntca::ErrorEvent&                             event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    // *** Listener Socket ***

    /// Announce to the specified 'manager' the establishment of the
    /// specified 'socket'. If the specified 'defer' flag is false and the
    /// requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceEstablished(
        const bsl::shared_ptr<ntci::ListenerSocketManager>& manager,
        const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'manager' the closure of the specified
    /// 'socket'. If the specified 'defer' flag is false and the
    /// requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceClosed(
        const bsl::shared_ptr<ntci::ListenerSocketManager>& manager,
        const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition that accept queue
    /// flow control has been relaxed. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceAcceptQueueFlowControlRelaxed(
        const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
        const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
        const ntca::AcceptQueueEvent&                       event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition that accept queue
    /// flow control has been applied. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceAcceptQueueFlowControlApplied(
        const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
        const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
        const ntca::AcceptQueueEvent&                       event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition of the specified
    /// 'socket' that the size of the accept queue is greater than or equal
    /// to the read queue low watermark. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceAcceptQueueLowWatermark(
        const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
        const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
        const ntca::AcceptQueueEvent&                       event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition of the specified
    /// 'socket' that the size of the accept queue is greater than the read
    /// queue high watermark. If the specified 'defer' flag is false and the
    /// requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceAcceptQueueHighWatermark(
        const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
        const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
        const ntca::AcceptQueueEvent&                       event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition of the specified
    /// 'socket' that the accept queue has been discarded because
    /// a non-transient read error asynchronously occured. If the specified
    /// 'defer' flag is false and the requirements of the specified
    /// 'destination' strand permits the announcement to be executed
    /// immediately by the specified 'source' strand, unlock the specified
    /// 'mutex', execute the announcement, then relock the 'mutex'.
    /// Otherwise, enqueue the announcement to be executed on the
    /// 'destination' strand, if not null, or by the specified 'executor'
    /// otherwise. The behavior is undefined if 'mutex' is null or not
    /// locked. The behavior is *not* undefined if either the 'destination'
    /// strand is null or the 'source' strand is null; a null 'destination'
    /// strand indicates the announcement may be invoked on any strand by
    /// any thread; a null 'source' strand indicates the source strand is
    /// unknown.
    static void announceAcceptQueueDiscarded(
        const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
        const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
        const ntca::AcceptQueueEvent&                       event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition that acccept queue
    /// rate limit has been reached. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceAcceptQueueRateLimitApplied(
        const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
        const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
        const ntca::AcceptQueueEvent&                       event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the condition that accept queue
    /// rate limit timer has fired. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceAcceptQueueRateLimitRelaxed(
        const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
        const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
        const ntca::AcceptQueueEvent&                       event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'manager' the specified 'socket' rejected
    /// the connection. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceConnectionLimit(
        const bsl::shared_ptr<ntci::ListenerSocketManager>& manager,
        const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the initiation of the shutdown
    /// sequence of the specified 'socket' from the specified 'origin'. If
    /// the specified 'defer' flag is false and the requirements of the
    /// specified 'destination' strand permits the announcement to be
    /// executed immediately by the specified 'source' strand, unlock the
    /// specified 'mutex', execute the announcement, then relock the
    /// 'mutex'. Otherwise, enqueue the announcement to be executed on the
    /// 'destination' strand, if not null, or by the specified 'executor'
    /// otherwise. The behavior is undefined if 'mutex' is null or not
    /// locked. The behavior is *not* undefined if either the 'destination'
    /// strand is null or the 'source' strand is null; a null 'destination'
    /// strand indicates the announcement may be invoked on any strand by
    /// any thread; a null 'source' strand indicates the source strand is
    /// unknown.
    static void announceShutdownInitiated(
        const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
        const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
        const ntca::ShutdownEvent&                          event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' that the specified 'socket' is
    /// shut down for reading. If the specified 'defer' flag is false and
    /// the requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceShutdownReceive(
        const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
        const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
        const ntca::ShutdownEvent&                          event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' that the specified 'socket' is
    /// shut down for writing. If the specified 'defer' flag is false and
    /// the requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceShutdownSend(
        const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
        const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
        const ntca::ShutdownEvent&                          event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the shutdown sequence of the
    /// specified 'socket' has completed. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceShutdownComplete(
        const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
        const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
        const ntca::ShutdownEvent&                          event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    /// Announce to the specified 'session' the detection of an error for
    /// the specified 'socket'. If the specified 'defer' flag is false and
    /// the requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceError(
        const bsl::shared_ptr<ntci::ListenerSocketSession>& session,
        const bsl::shared_ptr<ntci::ListenerSocket>&        socket,
        const ntca::ErrorEvent&                             event,
        const bsl::shared_ptr<ntci::Strand>&                destination,
        const bsl::shared_ptr<ntci::Strand>&                source,
        const bsl::shared_ptr<ntci::Executor>&              executor,
        bool                                                defer,
        ntccfg::Mutex*                                      mutex);

    // *** Stream Socket ***

    /// Announce to the specified 'manager' the establishment of the
    /// specified 'socket'. If the specified 'defer' flag is false and the
    /// requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceEstablished(
        const bsl::shared_ptr<ntci::StreamSocketManager>& manager,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'manager' the closure of the specified
    /// 'socket'. If the specified 'defer' flag is false and the
    /// requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceClosed(
        const bsl::shared_ptr<ntci::StreamSocketManager>& manager,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the condition that read queue
    /// flow control has been relaxed. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceReadQueueFlowControlRelaxed(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::ReadQueueEvent&                       event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the condition that read queue
    /// flow control has been applied. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceReadQueueFlowControlApplied(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::ReadQueueEvent&                       event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the condition of the specified
    /// 'socket' that the size of the read queue is greater than or equal to
    /// the read queue low watermark. If the specified 'defer' flag is false
    /// and the requirements of the specified 'destination' strand permits
    /// the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceReadQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::ReadQueueEvent&                       event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the condition of the specified
    /// 'socket' that the size of the read queue is greater than the read
    /// queue high watermark. If the specified 'defer' flag is false and the
    /// requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceReadQueueHighWatermark(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::ReadQueueEvent&                       event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the condition of the specified
    /// 'socket' that the read queue has been discarded because
    /// a non-transient read error asynchronously occured. If the specified
    /// 'defer' flag is false and the requirements of the specified
    /// 'destination' strand permits the announcement to be executed
    /// immediately by the specified 'source' strand, unlock the specified
    /// 'mutex', execute the announcement, then relock the 'mutex'.
    /// Otherwise, enqueue the announcement to be executed on the
    /// 'destination' strand, if not null, or by the specified 'executor'
    /// otherwise. The behavior is undefined if 'mutex' is null or not
    /// locked. The behavior is *not* undefined if either the 'destination'
    /// strand is null or the 'source' strand is null; a null 'destination'
    /// strand indicates the announcement may be invoked on any strand by
    /// any thread; a null 'source' strand indicates the source strand is
    /// unknown.
    static void announceReadQueueDiscarded(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::ReadQueueEvent&                       event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the condition that read queue
    /// rate limit has been reached. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceReadQueueRateLimitApplied(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::ReadQueueEvent&                       event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the condition that read queue
    /// rate limit timer has fired. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceReadQueueRateLimitRelaxed(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::ReadQueueEvent&                       event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the condition that write queue
    /// flow control has been relaxed. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceWriteQueueFlowControlRelaxed(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::WriteQueueEvent&                      event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the condition that write queue
    /// flow control has been applied. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceWriteQueueFlowControlApplied(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::WriteQueueEvent&                      event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the condition of the specified
    /// 'socket' that the size of the write queue is less than or equal to
    /// the write queue low watermark. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceWriteQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::WriteQueueEvent&                      event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the condition of the specified
    /// 'socket' that the size of the write queue is greater than the write
    /// queue high watermark. If the specified 'defer' flag is false and the
    /// requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceWriteQueueHighWatermark(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::WriteQueueEvent&                      event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the  condition of the specified
    /// 'socket' that the write queue has been discarded because
    /// a non-transient write error asynchronously occured. If the specified
    /// 'defer' flag is false and the requirements of the specified
    /// 'destination' strand permits the announcement to be executed
    /// immediately by the specified 'source' strand, unlock the specified
    /// 'mutex', execute the announcement, then relock the 'mutex'.
    /// Otherwise, enqueue the announcement to be executed on the
    /// 'destination' strand, if not null, or by the specified 'executor'
    /// otherwise. The behavior is undefined if 'mutex' is null or not
    /// locked. The behavior is *not* undefined if either the 'destination'
    /// strand is null or the 'source' strand is null; a null 'destination'
    /// strand indicates the announcement may be invoked on any strand by
    /// any thread; a null 'source' strand indicates the source strand is
    /// unknown.
    static void announceWriteQueueDiscarded(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::WriteQueueEvent&                      event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the condition that write queue
    /// rate limit has been reached. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceWriteQueueRateLimitApplied(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::WriteQueueEvent&                      event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the condition that write queue
    /// rate limit timer has fired. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceWriteQueueRateLimitRelaxed(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::WriteQueueEvent&                      event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the initiation of a downgrade of
    /// the specified 'socket' from encrypted to unencrypted communication.
    /// If the specified 'defer' flag is false and the requirements of the
    /// specified 'destination' strand permits the announcement to be
    /// executed immediately by the specified 'source' strand, unlock the
    /// specified 'mutex', execute the announcement, then relock the
    /// 'mutex'. Otherwise, enqueue the announcement to be executed on the
    /// 'destination' strand, if not null, or by the specified 'executor'
    /// otherwise. The behavior is undefined if 'mutex' is null or not
    /// locked. The behavior is *not* undefined if either the 'destination'
    /// strand is null or the 'source' strand is null; a null 'destination'
    /// strand indicates the announcement may be invoked on any strand by
    /// any thread; a null 'source' strand indicates the source strand is
    /// unknown.
    static void announceDowngradeInitiated(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::DowngradeEvent&                       event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the initiation of a downgrade of
    /// the specified 'socket' from encrypted to unencrypted communication.
    /// If the specified 'defer' flag is false and the requirements of the
    /// specified 'destination' strand permits the announcement to be
    /// executed immediately by the specified 'source' strand, unlock the
    /// specified 'mutex', execute the announcement, then relock the
    /// 'mutex'. Otherwise, enqueue the announcement to be executed on the
    /// 'destination' strand, if not null, or by the specified 'executor'
    /// otherwise. The behavior is undefined if 'mutex' is null or not
    /// locked. The behavior is *not* undefined if either the 'destination'
    /// strand is null or the 'source' strand is null; a null 'destination'
    /// strand indicates the announcement may be invoked on any strand by
    /// any thread; a null 'source' strand indicates the source strand is
    /// unknown.
    static void announceDowngradeComplete(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::DowngradeEvent&                       event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the initiation of the shutdown
    /// sequence of the specified 'socket' from the specified 'origin'. If
    /// the specified 'defer' flag is false and the requirements of the
    /// specified 'destination' strand permits the announcement to be
    /// executed immediately by the specified 'source' strand, unlock the
    /// specified 'mutex', execute the announcement, then relock the
    /// 'mutex'. Otherwise, enqueue the announcement to be executed on the
    /// 'destination' strand, if not null, or by the specified 'executor'
    /// otherwise. The behavior is undefined if 'mutex' is null or not
    /// locked. The behavior is *not* undefined if either the 'destination'
    /// strand is null or the 'source' strand is null; a null 'destination'
    /// strand indicates the announcement may be invoked on any strand by
    /// any thread; a null 'source' strand indicates the source strand is
    /// unknown.
    static void announceShutdownInitiated(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::ShutdownEvent&                        event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' that the specified 'socket' is
    /// shut down for reading. If the specified 'defer' flag is false and
    /// the requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceShutdownReceive(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::ShutdownEvent&                        event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' that the specified 'socket' is
    /// shut down for writing. If the specified 'defer' flag is false and
    /// the requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceShutdownSend(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::ShutdownEvent&                        event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the shutdown sequence of the
    /// specified 'socket' has completed. If the specified 'defer' flag is
    /// false and the requirements of the specified 'destination' strand
    /// permits the announcement to be executed immediately by the specified
    /// 'source' strand, unlock the specified 'mutex', execute the
    /// announcement, then relock the 'mutex'. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand, if not
    /// null, or by the specified 'executor' otherwise. The behavior is
    /// undefined if 'mutex' is null or not locked. The behavior is *not*
    /// undefined if either the 'destination' strand is null or the 'source'
    /// strand is null; a null 'destination' strand indicates the
    /// announcement may be invoked on any strand by any thread; a null
    /// 'source' strand indicates the source strand is unknown.
    static void announceShutdownComplete(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::ShutdownEvent&                        event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    /// Announce to the specified 'session' the detection of an error for
    /// the specified 'socket'. If the specified 'defer' flag is false and
    /// the requirements of the specified 'destination' strand permits the
    /// announcement to be executed immediately by the specified 'source'
    /// strand, unlock the specified 'mutex', execute the announcement, then
    /// relock the 'mutex'. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand, if not null, or by the
    /// specified 'executor' otherwise. The behavior is undefined if 'mutex'
    /// is null or not locked. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceError(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session,
        const bsl::shared_ptr<ntci::StreamSocket>&        socket,
        const ntca::ErrorEvent&                           event,
        const bsl::shared_ptr<ntci::Strand>&              destination,
        const bsl::shared_ptr<ntci::Strand>&              source,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bool                                              defer,
        ntccfg::Mutex*                                    mutex);

    // *** Reactor Socket ***

    /// Announce to the specified 'socket' that it is readable. If the
    /// specified 'destination' strand is null, execute the announcement
    /// immediately. Otherwise, enqueue the announcement to be executed on
    /// the 'destination' strand.
    static void announceReadable(
        const bsl::shared_ptr<ntci::ReactorSocket>& socket,
        const ntca::ReactorEvent&                   event,
        const bsl::shared_ptr<ntci::Strand>&        destination);

    /// Announce to the specified 'socket' that it is writable. If the
    /// specified 'destination' strand is null, execute the announcement
    /// immediately. Otherwise, enqueue the announcement to be executed on
    /// the 'destination' strand.
    static void announceWritable(
        const bsl::shared_ptr<ntci::ReactorSocket>& socket,
        const ntca::ReactorEvent&                   event,
        const bsl::shared_ptr<ntci::Strand>&        destination);

    /// Announce to the specified 'socket' that the specified 'error' has
    /// occured. If the specified 'destination' strand is null, execute the
    /// announcement immediately. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand.
    static void announceError(
        const bsl::shared_ptr<ntci::ReactorSocket>& socket,
        const ntca::ReactorEvent&                   event,
        const bsl::shared_ptr<ntci::Strand>&        destination);

    /// Announce to the specified 'socket' that the specified 'notifications'
    /// have occured. If the specified 'destination' strand is null, execute
    /// the announcement immediately. Otherwise, enqueue the announcement to
    /// be executed on the 'destination' strand.
    static void announceNotifications(
        const bsl::shared_ptr<ntci::ReactorSocket>& socket,
        const ntsa::NotificationQueue&              notifications,
        const bsl::shared_ptr<ntci::Strand>&        destination);

    // *** Proactor Socket ***

    /// Announce to the specified 'socket' the completion of the acceptance
    /// of the specified 'streamSocket' or the specified 'error'. If the
    /// specified 'destination' strand is null, execute the announcement
    /// immediately. Otherwise, enqueue the announcement to be executed on
    /// the 'destination' strand.
    static void announceAccepted(
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        const ntsa::Error&                           error,
        const bsl::shared_ptr<ntsi::StreamSocket>&   streamSocket,
        const bsl::shared_ptr<ntci::Strand>&         destination);

    /// Announce to the specified 'socket' the completion of the
    /// establishment of the connection or the specified 'error'. If the
    /// specified 'destination' strand is null, execute the announcement
    /// immediately. Otherwise, enqueue the announcement to be executed on
    /// the 'destination' strand.
    static void announceConnected(
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        const ntsa::Error&                           error,
        const bsl::shared_ptr<ntci::Strand>&         destination);

    /// Announce to the specified 'socket' the completion of the reception
    /// of data described by the specified 'context' or the specified
    /// 'error'. If the specified 'destination' strand is null, execute the
    /// announcement immediately. Otherwise, enqueue the announcement to be
    ///  executed on the 'destination' strand.
    static void announceReceived(
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        const ntsa::Error&                           error,
        const ntsa::ReceiveContext&                  context,
        const bsl::shared_ptr<ntci::Strand>&         destination);

    /// Announce to the specified 'socket' the completion of the
    /// transmission of data described by the specified 'context' or the
    /// specified 'error'. If the specified 'destination' strand' is null,
    /// execute the announcement immediately. Otherwise, enqueue the
    /// announcement to be executed on the 'destination' strand.
    static void announceSent(
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        const ntsa::Error&                           error,
        const ntsa::SendContext&                     context,
        const bsl::shared_ptr<ntci::Strand>&         destination);

    /// Announce to the specified 'socket' that the specified 'error' has
    /// occured. If the specified 'destination' strand is null, execute the
    /// announcement immediately. Otherwise, enqueue the announcement to be
    /// executed on the 'destination' strand.
    static void announceError(
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        const ntsa::Error&                           error,
        const bsl::shared_ptr<ntci::Strand>&         destination);

    /// Announce to the specified socket that it has been detached.
    /// If the specified 'destination' strand is available then announce it
    /// on this strand
    static void announceDetached(
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        const bsl::shared_ptr<ntci::Strand>&         destination);

    // *** Timer ***

    /// Announce to the specified 'timer' that its deadline has arrived. If
    /// the specified 'defer' flag is false and the requirements of the
    /// specified 'destination' strand permits the announcement to be
    /// executed immediately by the specified 'source' strand, execute the
    /// announcement Otherwise, enqueue the announcement to be executed on
    /// the 'destination' strand, if not null, or by the specified
    /// 'executor' otherwise. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceDeadline(
        const bsl::shared_ptr<ntci::TimerSession>& session,
        const bsl::shared_ptr<ntci::Timer>&        timer,
        const ntca::TimerEvent&                    event,
        const bsl::shared_ptr<ntci::Strand>&       destination,
        const bsl::shared_ptr<ntci::Strand>&       source,
        const bsl::shared_ptr<ntci::Executor>&     executor,
        bool                                       defer);

    /// Announce to the specified 'timer' that it has been cancelled. If
    /// the specified 'defer' flag is false and the requirements of the
    /// specified 'destination' strand permits the announcement to be
    /// executed immediately by the specified 'source' strand, execute the
    /// announcement Otherwise, enqueue the announcement to be executed on
    /// the 'destination' strand, if not null, or by the specified
    /// 'executor' otherwise. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    /// unknown.
    static void announceCancelled(
        const bsl::shared_ptr<ntci::TimerSession>& session,
        const bsl::shared_ptr<ntci::Timer>&        timer,
        const ntca::TimerEvent&                    event,
        const bsl::shared_ptr<ntci::Strand>&       destination,
        const bsl::shared_ptr<ntci::Strand>&       source,
        const bsl::shared_ptr<ntci::Executor>&     executor,
        bool                                       defer);

    /// Announce to the specified 'timer' that it has been removed. If
    /// the specified 'defer' flag is false and the requirements of the
    /// specified 'destination' strand permits the announcement to be
    /// executed immediately by the specified 'source' strand, execute the
    /// announcement Otherwise, enqueue the announcement to be executed on
    /// the 'destination' strand, if not null, or by the specified
    /// 'executor' otherwise. The behavior is *not* undefined if either the
    /// 'destination' strand is null or the 'source' strand is null; a null
    /// 'destination' strand indicates the announcement may be invoked on
    /// any strand by any thread; a null 'source' strand indicates the
    /// source strand is unknown.
    static void announceRemoval(
        const bsl::shared_ptr<ntci::TimerSession>& session,
        const bsl::shared_ptr<ntci::Timer>&        timer,
        const ntca::TimerEvent&                    event,
        const bsl::shared_ptr<ntci::Strand>&       destination,
        const bsl::shared_ptr<ntci::Strand>&       source,
        const bsl::shared_ptr<ntci::Executor>&     executor,
        bool                                       defer);
};

// *** Reactor Socket ***

NTCCFG_INLINE
void Dispatch::announceReadable(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket,
    const ntca::ReactorEvent&                   event,
    const bsl::shared_ptr<ntci::Strand>&        destination)
{
    if (NTCCFG_LIKELY(!destination)) {
        socket->processSocketReadable(event);
    }
    else {
        destination->execute(
            NTCCFG_BIND(&ntci::ReactorSocket::processSocketReadable,
                        socket,
                        event));
    }
}

NTCCFG_INLINE
void Dispatch::announceWritable(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket,
    const ntca::ReactorEvent&                   event,
    const bsl::shared_ptr<ntci::Strand>&        destination)
{
    if (NTCCFG_LIKELY(!destination)) {
        socket->processSocketWritable(event);
    }
    else {
        destination->execute(
            NTCCFG_BIND(&ntci::ReactorSocket::processSocketWritable,
                        socket,
                        event));
    }
}

NTCCFG_INLINE
void Dispatch::announceError(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket,
    const ntca::ReactorEvent&                   event,
    const bsl::shared_ptr<ntci::Strand>&        destination)
{
    if (NTCCFG_LIKELY(!destination)) {
        socket->processSocketError(event);
    }
    else {
        destination->execute(
            NTCCFG_BIND(&ntci::ReactorSocket::processSocketError,
                        socket,
                        event));
    }
}

NTCCFG_INLINE
void Dispatch::announceNotifications(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket,
    const ntsa::NotificationQueue&              notifications,
    const bsl::shared_ptr<ntci::Strand>&        destination)
{
    if (NTCCFG_LIKELY(!destination)) {
        socket->processNotifications(notifications);
    }
    else {
        destination->execute(
            NTCCFG_BIND(&ntci::ReactorSocket::processNotifications,
                        socket,
                        notifications));
    }
}

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
