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

#ifndef INCLUDED_NTCI_STREAMSOCKETSESSION
#define INCLUDED_NTCI_STREAMSOCKETSESSION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_connectevent.h>
#include <ntca_downgradeevent.h>
#include <ntca_errorevent.h>
#include <ntca_readqueueevent.h>
#include <ntca_shutdownevent.h>
#include <ntca_upgradeevent.h>
#include <ntca_writequeueevent.h>
#include <ntccfg_platform.h>
#include <ntci_encryptioncertificate.h>
#include <ntci_strand.h>
#include <ntcscm_version.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_shutdownorigin.h>
#include <ntsa_shutdowntype.h>
#include <bdlbb_blob.h>
#include <bsl_memory.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntci {
class StreamSocket;
}
namespace ntci {

/// Provide an interface to detect events that passively occur during the
/// asynchronous operation of a stream socket.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_socket
class StreamSocketSession
{
  public:
    /// Destroy this object.
    virtual ~StreamSocketSession();

    /// Process the condition that read queue flow control has been relaxed:
    /// the socket receive buffer is being automatically copied to the read
    /// queue.
    virtual void processReadQueueFlowControlRelaxed(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent&                event);

    /// Process the condition that read queue flow control has been applied:
    /// the socket receive buffer is not being automatically copied to the
    /// read queue.
    virtual void processReadQueueFlowControlApplied(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent&                event);

    /// Process the condition that the size of the read queue is greater
    /// than or equal to the read queue low watermark.
    virtual void processReadQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent&                event);

    /// Process the condition that the size of the read queue is greater
    /// than the read queue high watermark.
    virtual void processReadQueueHighWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent&                event);

    /// Process the condition that the read queue has been discarded
    /// because a non-transient read error asynchronously occured.
    virtual void processReadQueueDiscarded(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent&                event);

    virtual void processReadQueueRateLimitApplied(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent&                event);

    virtual void processReadQueueRateLimitRelaxed(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent&                event);

    /// Process the condition that write queue flow control has been
    /// relaxed: the write queue is being automatically copied to the socket
    /// send buffer.
    virtual void processWriteQueueFlowControlRelaxed(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent&               event);

    /// Process the condition that write queue flow control has been
    /// applied: the write queue is not being automatically copied to the
    /// socket send buffer.
    virtual void processWriteQueueFlowControlApplied(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent&               event);

    /// Process the condition that the size of the write queue has been
    /// drained down to less than or equal to the write queue low watermark.
    /// This condition will not occur until the write queue high watermark
    /// condition occurs. The write queue low watermark conditions and the
    /// high watermark conditions are guaranteed to occur serially.
    virtual void processWriteQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent&               event);

    /// Process the condition that the size of the write queue is greater
    /// than the write queue high watermark. This condition will occur the
    /// first time the write queue high watermark has been reached but
    /// then will not subsequently ooccur until the write queue low
    /// watermark. The write queue low watermark conditions and the
    /// high watermark conditions are guaranteed to occur serially.
    virtual void processWriteQueueHighWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent&               event);

    /// Process the condition that the write queue has been discarded
    /// because a non-transient write error asynchronously occured.
    virtual void processWriteQueueDiscarded(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent&               event);

    virtual void processWriteQueueRateLimitApplied(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent&               event);

    virtual void processWriteQueueRateLimitRelaxed(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent&               event);

    /// Process the initiation of a downgrade from encrypted to unencrypted
    /// communication.
    virtual void processDowngradeInitiated(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::DowngradeEvent&                event);

    /// Process the completion of a downgrade from encrypted to unencrypted
    /// communication.
    virtual void processDowngradeComplete(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::DowngradeEvent&                event);

    /// Process the initiation of the shutdown sequence from the specified
    /// 'origin'.
    virtual void processShutdownInitiated(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent&                 event);

    /// Process the socket being shut down for reading.
    virtual void processShutdownReceive(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent&                 event);

    /// Process the socket being shut down for writing.
    virtual void processShutdownSend(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent&                 event);

    /// Process the completion of the shutdown sequence.
    virtual void processShutdownComplete(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent&                 event);

    /// Process an error.
    virtual void processError(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ErrorEvent&                    event);

    /// Return the strand on which this object's functions should be called.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const;
};

}  // close package namespace
}  // close enterprise namespace
#endif
