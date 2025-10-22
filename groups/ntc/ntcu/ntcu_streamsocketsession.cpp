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

#include <ntcu_streamsocketsession.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcu_streamsocketsession_cpp, "$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcu {

StreamSocketSession::StreamSocketSession(
    const ntci::StreamSocket::SessionCallback& callback,
    const bsl::shared_ptr<ntci::Strand>&       strand,
    bslma::Allocator*                          basicAllocator)
: d_callback(NTCCFG_FUNCTION_COPY(callback, basicAllocator))
, d_strand_sp(strand)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

StreamSocketSession::~StreamSocketSession()
{
}

void StreamSocketSession::processConnectInitiated(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ConnectEvent&                  event)
{
    BSLS_ASSERT(event.type() == ntca::ConnectEventType::e_INITIATED);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processConnectComplete(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ConnectEvent&                  event)
{
    BSLS_ASSERT(event.type() == ntca::ConnectEventType::e_COMPLETE);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processReadQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    BSLS_ASSERT(event.type() ==
                ntca::ReadQueueEventType::e_FLOW_CONTROL_RELAXED);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processReadQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    BSLS_ASSERT(event.type() ==
                ntca::ReadQueueEventType::e_FLOW_CONTROL_APPLIED);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    BSLS_ASSERT(event.type() == ntca::ReadQueueEventType::e_LOW_WATERMARK);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processReadQueueHighWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    BSLS_ASSERT(event.type() == ntca::ReadQueueEventType::e_HIGH_WATERMARK);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processReadQueueDiscarded(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    BSLS_ASSERT(event.type() == ntca::ReadQueueEventType::e_DISCARDED);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processWriteQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    BSLS_ASSERT(event.type() ==
                ntca::WriteQueueEventType::e_FLOW_CONTROL_RELAXED);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processWriteQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    BSLS_ASSERT(event.type() ==
                ntca::WriteQueueEventType::e_FLOW_CONTROL_APPLIED);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    BSLS_ASSERT(event.type() == ntca::WriteQueueEventType::e_LOW_WATERMARK);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processWriteQueueHighWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    BSLS_ASSERT(event.type() == ntca::WriteQueueEventType::e_HIGH_WATERMARK);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processWriteQueueDiscarded(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    BSLS_ASSERT(event.type() == ntca::WriteQueueEventType::e_DISCARDED);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processDowngradeInitiated(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::DowngradeEvent&                event)
{
    BSLS_ASSERT(event.type() == ntca::DowngradeEventType::e_INITIATED);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processDowngradeComplete(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::DowngradeEvent&                event)
{
    BSLS_ASSERT(event.type() == ntca::DowngradeEventType::e_COMPLETE);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processShutdownInitiated(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    BSLS_ASSERT(event.type() == ntca::ShutdownEventType::e_INITIATED);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processShutdownReceive(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    BSLS_ASSERT(event.type() == ntca::ShutdownEventType::e_RECEIVE);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processShutdownSend(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    BSLS_ASSERT(event.type() == ntca::ShutdownEventType::e_SEND);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processShutdownComplete(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    BSLS_ASSERT(event.type() == ntca::ShutdownEventType::e_COMPLETE);
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

void StreamSocketSession::processError(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ErrorEvent&                    event)
{
    d_callback(streamSocket, ntca::StreamSocketEvent(event));
}

const bsl::shared_ptr<ntci::Strand>& StreamSocketSession::strand() const
{
    return d_strand_sp;
}

}  // close package namespace
}  // close enterprise namespace
