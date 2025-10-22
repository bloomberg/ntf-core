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

#include <ntcu_datagramsocketsession.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcu_datagramsocketsession_cpp, "$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcu {

DatagramSocketSession::DatagramSocketSession(
    const ntci::DatagramSocket::SessionCallback& callback,
    const bsl::shared_ptr<ntci::Strand>&         strand,
    bslma::Allocator*                            basicAllocator)
: d_callback(NTCCFG_FUNCTION_COPY(callback, basicAllocator))
, d_strand_sp(strand)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

DatagramSocketSession::~DatagramSocketSession()
{
}

void DatagramSocketSession::processConnectInitiated(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ConnectEvent&                    event)
{
    BSLS_ASSERT(event.type() == ntca::ConnectEventType::e_INITIATED);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processConnectComplete(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ConnectEvent&                    event)
{
    BSLS_ASSERT(event.type() == ntca::ConnectEventType::e_COMPLETE);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processReadQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    BSLS_ASSERT(event.type() ==
                ntca::ReadQueueEventType::e_FLOW_CONTROL_RELAXED);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processReadQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    BSLS_ASSERT(event.type() ==
                ntca::ReadQueueEventType::e_FLOW_CONTROL_APPLIED);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    BSLS_ASSERT(event.type() == ntca::ReadQueueEventType::e_LOW_WATERMARK);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processReadQueueHighWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    BSLS_ASSERT(event.type() == ntca::ReadQueueEventType::e_HIGH_WATERMARK);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processReadQueueDiscarded(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    BSLS_ASSERT(event.type() == ntca::ReadQueueEventType::e_DISCARDED);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processWriteQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    BSLS_ASSERT(event.type() ==
                ntca::WriteQueueEventType::e_FLOW_CONTROL_RELAXED);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processWriteQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    BSLS_ASSERT(event.type() ==
                ntca::WriteQueueEventType::e_FLOW_CONTROL_APPLIED);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    BSLS_ASSERT(event.type() == ntca::WriteQueueEventType::e_LOW_WATERMARK);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processWriteQueueHighWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    BSLS_ASSERT(event.type() == ntca::WriteQueueEventType::e_HIGH_WATERMARK);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processWriteQueueDiscarded(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    BSLS_ASSERT(event.type() == ntca::WriteQueueEventType::e_DISCARDED);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processDowngradeInitiated(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::DowngradeEvent&                  event)
{
    BSLS_ASSERT(event.type() == ntca::DowngradeEventType::e_INITIATED);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processDowngradeComplete(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::DowngradeEvent&                  event)
{
    BSLS_ASSERT(event.type() == ntca::DowngradeEventType::e_COMPLETE);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processShutdownInitiated(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    BSLS_ASSERT(event.type() == ntca::ShutdownEventType::e_INITIATED);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processShutdownReceive(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    BSLS_ASSERT(event.type() == ntca::ShutdownEventType::e_RECEIVE);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processShutdownSend(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    BSLS_ASSERT(event.type() == ntca::ShutdownEventType::e_SEND);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processShutdownComplete(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    BSLS_ASSERT(event.type() == ntca::ShutdownEventType::e_COMPLETE);
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

void DatagramSocketSession::processError(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ErrorEvent&                      event)
{
    d_callback(datagramSocket, ntca::DatagramSocketEvent(event));
}

const bsl::shared_ptr<ntci::Strand>& DatagramSocketSession::strand() const
{
    return d_strand_sp;
}

}  // close package namespace
}  // close enterprise namespace
