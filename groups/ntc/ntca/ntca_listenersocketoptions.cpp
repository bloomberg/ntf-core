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

#include <ntca_listenersocketoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_listenersocketoptions_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

namespace {

const bool DEFAULT_REUSE_ADDRESS = true;

}  // close unnamed namespace

ListenerSocketOptions::ListenerSocketOptions()
: d_transport(ntsa::Transport::e_UNDEFINED)
, d_sourceEndpoint()
, d_reuseAddress(DEFAULT_REUSE_ADDRESS)
, d_backlog()
, d_acceptQueueLowWatermark()
, d_acceptQueueHighWatermark()
, d_readQueueLowWatermark()
, d_readQueueHighWatermark()
, d_writeQueueLowWatermark()
, d_writeQueueHighWatermark()
, d_minIncomingStreamTransferSize()
, d_maxIncomingStreamTransferSize()
, d_acceptGreedily()
, d_sendGreedily()
, d_receiveGreedily()
, d_sendBufferSize()
, d_receiveBufferSize()
, d_sendBufferLowWatermark()
, d_receiveBufferLowWatermark()
, d_sendTimeout()
, d_receiveTimeout()
, d_keepAlive()
, d_noDelay()
, d_debugFlag()
, d_allowBroadcasting()
, d_bypassNormalRouting()
, d_leaveOutOfBandDataInline()
, d_lingerFlag()
, d_lingerTimeout()
, d_keepHalfOpen()
, d_metrics()
, d_timestampOutgoingData()
, d_timestampIncomingData()
, d_zeroCopyThreshold()
, d_loadBalancingOptions()
{
}

ListenerSocketOptions::ListenerSocketOptions(
    const ListenerSocketOptions& other)
: d_transport(other.d_transport)
, d_sourceEndpoint(other.d_sourceEndpoint)
, d_reuseAddress(other.d_reuseAddress)
, d_backlog(other.d_backlog)
, d_acceptQueueLowWatermark(other.d_acceptQueueLowWatermark)
, d_acceptQueueHighWatermark(other.d_acceptQueueHighWatermark)
, d_readQueueLowWatermark(other.d_readQueueLowWatermark)
, d_readQueueHighWatermark(other.d_readQueueHighWatermark)
, d_writeQueueLowWatermark(other.d_writeQueueLowWatermark)
, d_writeQueueHighWatermark(other.d_writeQueueHighWatermark)
, d_minIncomingStreamTransferSize(other.d_minIncomingStreamTransferSize)
, d_maxIncomingStreamTransferSize(other.d_maxIncomingStreamTransferSize)
, d_acceptGreedily(other.d_acceptGreedily)
, d_sendGreedily(other.d_sendGreedily)
, d_receiveGreedily(other.d_receiveGreedily)
, d_sendBufferSize(other.d_sendBufferSize)
, d_receiveBufferSize(other.d_receiveBufferSize)
, d_sendBufferLowWatermark(other.d_sendBufferLowWatermark)
, d_receiveBufferLowWatermark(other.d_receiveBufferLowWatermark)
, d_sendTimeout(other.d_sendTimeout)
, d_receiveTimeout(other.d_receiveTimeout)
, d_keepAlive(other.d_keepAlive)
, d_noDelay(other.d_noDelay)
, d_debugFlag(other.d_debugFlag)
, d_allowBroadcasting(other.d_allowBroadcasting)
, d_bypassNormalRouting(other.d_bypassNormalRouting)
, d_leaveOutOfBandDataInline(other.d_leaveOutOfBandDataInline)
, d_lingerFlag(other.d_lingerFlag)
, d_lingerTimeout(other.d_lingerTimeout)
, d_keepHalfOpen(other.d_keepHalfOpen)
, d_metrics(other.d_metrics)
, d_timestampOutgoingData(other.d_timestampOutgoingData)
, d_timestampIncomingData(other.d_timestampIncomingData)
, d_zeroCopyThreshold(other.d_zeroCopyThreshold)
, d_loadBalancingOptions(other.d_loadBalancingOptions)
{
}

ListenerSocketOptions::~ListenerSocketOptions()
{
}

ListenerSocketOptions& ListenerSocketOptions::operator=(
    const ListenerSocketOptions& other)
{
    if (this != &other) {
        d_transport                = other.d_transport;
        d_sourceEndpoint           = other.d_sourceEndpoint;
        d_reuseAddress             = other.d_reuseAddress;
        d_backlog                  = other.d_backlog;
        d_acceptQueueLowWatermark  = other.d_acceptQueueLowWatermark;
        d_acceptQueueHighWatermark = other.d_acceptQueueHighWatermark;
        d_readQueueLowWatermark    = other.d_readQueueLowWatermark;
        d_readQueueHighWatermark   = other.d_readQueueHighWatermark;
        d_writeQueueLowWatermark   = other.d_writeQueueLowWatermark;
        d_writeQueueHighWatermark  = other.d_writeQueueHighWatermark;
        d_minIncomingStreamTransferSize =
            other.d_minIncomingStreamTransferSize;
        d_maxIncomingStreamTransferSize =
            other.d_maxIncomingStreamTransferSize;
        d_acceptGreedily            = other.d_acceptGreedily;
        d_sendGreedily              = other.d_sendGreedily;
        d_receiveGreedily           = other.d_receiveGreedily;
        d_sendBufferSize            = other.d_sendBufferSize;
        d_receiveBufferSize         = other.d_receiveBufferSize;
        d_sendBufferLowWatermark    = other.d_sendBufferLowWatermark;
        d_receiveBufferLowWatermark = other.d_receiveBufferLowWatermark;
        d_sendTimeout               = other.d_sendTimeout;
        d_receiveTimeout            = other.d_receiveTimeout;
        d_keepAlive                 = other.d_keepAlive;
        d_noDelay                   = other.d_noDelay;
        d_debugFlag                 = other.d_debugFlag;
        d_allowBroadcasting         = other.d_allowBroadcasting;
        d_bypassNormalRouting       = other.d_bypassNormalRouting;
        d_leaveOutOfBandDataInline  = other.d_leaveOutOfBandDataInline;
        d_lingerFlag                = other.d_lingerFlag;
        d_lingerTimeout             = other.d_lingerTimeout;
        d_keepHalfOpen              = other.d_keepHalfOpen;
        d_metrics                   = other.d_metrics;
        d_timestampOutgoingData     = other.d_timestampOutgoingData;
        d_timestampIncomingData     = other.d_timestampIncomingData;
        d_zeroCopyThreshold         = other.d_zeroCopyThreshold;
        d_loadBalancingOptions      = other.d_loadBalancingOptions;
    }

    return *this;
}

void ListenerSocketOptions::setTransport(ntsa::Transport::Value value)
{
    d_transport = value;
}

void ListenerSocketOptions::setSourceEndpoint(const ntsa::Endpoint& value)
{
    d_sourceEndpoint = value;
    if (d_transport == ntsa::Transport::e_UNDEFINED) {
        d_transport = value.transport(ntsa::TransportMode::e_STREAM);
    }
}

void ListenerSocketOptions::setReuseAddress(bool value)
{
    d_reuseAddress = value;
}

void ListenerSocketOptions::setBacklog(bsl::size_t value)
{
    d_backlog = value;
}

void ListenerSocketOptions::setAcceptQueueLowWatermark(bsl::size_t value)
{
    d_acceptQueueLowWatermark = value;
}

void ListenerSocketOptions::setAcceptQueueHighWatermark(bsl::size_t value)
{
    d_acceptQueueHighWatermark = value;
}

void ListenerSocketOptions::setReadQueueLowWatermark(bsl::size_t value)
{
    d_readQueueLowWatermark = value;
}

void ListenerSocketOptions::setReadQueueHighWatermark(bsl::size_t value)
{
    d_readQueueHighWatermark = value;
}

void ListenerSocketOptions::setWriteQueueLowWatermark(bsl::size_t value)
{
    d_writeQueueLowWatermark = value;
}

void ListenerSocketOptions::setWriteQueueHighWatermark(bsl::size_t value)
{
    d_writeQueueHighWatermark = value;
}

void ListenerSocketOptions::setMinIncomingStreamTransferSize(bsl::size_t value)
{
    d_minIncomingStreamTransferSize = value;
}

void ListenerSocketOptions::setMaxIncomingStreamTransferSize(bsl::size_t value)
{
    d_maxIncomingStreamTransferSize = value;
}

void ListenerSocketOptions::setAcceptGreedily(bool value)
{
    d_acceptGreedily = value;
}

void ListenerSocketOptions::setSendGreedily(bool value)
{
    d_sendGreedily = value;
}

void ListenerSocketOptions::setReceiveGreedily(bool value)
{
    d_receiveGreedily = value;
}

void ListenerSocketOptions::setSendBufferSize(bsl::size_t value)
{
    d_sendBufferSize = value;
}

void ListenerSocketOptions::setReceiveBufferSize(bsl::size_t value)
{
    d_receiveBufferSize = value;
}

void ListenerSocketOptions::setSendBufferLowWatermark(bsl::size_t value)
{
    d_sendBufferLowWatermark = value;
}

void ListenerSocketOptions::setReceiveBufferLowWatermark(bsl::size_t value)
{
    d_receiveBufferLowWatermark = value;
}

void ListenerSocketOptions::setSendTimeout(bsl::size_t value)
{
    d_sendTimeout = value;
}

void ListenerSocketOptions::setReceiveTimeout(bsl::size_t value)
{
    d_receiveTimeout = value;
}

void ListenerSocketOptions::setKeepAlive(bool value)
{
    d_keepAlive = value;
}

void ListenerSocketOptions::setNoDelay(bool value)
{
    d_noDelay = value;
}

void ListenerSocketOptions::setDebugFlag(bool value)
{
    d_debugFlag = value;
}

void ListenerSocketOptions::setAllowBroadcasting(bool value)
{
    d_allowBroadcasting = value;
}

void ListenerSocketOptions::setBypassNormalRouting(bool value)
{
    d_bypassNormalRouting = value;
}

void ListenerSocketOptions::setLeaveOutofBandDataInline(bool value)
{
    d_leaveOutOfBandDataInline = value;
}

void ListenerSocketOptions::setLingerFlag(bool value)
{
    d_lingerFlag = value;
}

void ListenerSocketOptions::setLingerTimeout(bsl::size_t value)
{
    d_lingerTimeout = value;
}

void ListenerSocketOptions::setKeepHalfOpen(bool value)
{
    d_keepHalfOpen = value;
}

void ListenerSocketOptions::setMetrics(bool value)
{
    d_metrics = value;
}

void ListenerSocketOptions::setTimestampOutgoingData(bool value)
{
    d_timestampOutgoingData = value;
}

void ListenerSocketOptions::setTimestampIncomingData(bool value)
{
    d_timestampIncomingData = value;
}

void ListenerSocketOptions::setZeroCopyThreshold(bsl::size_t value)
{
    d_zeroCopyThreshold = value;
}

void ListenerSocketOptions::setLoadBalancingOptions(
    const ntca::LoadBalancingOptions& value)
{
    d_loadBalancingOptions = value;
}

ntsa::Transport::Value ListenerSocketOptions::transport() const
{
    return d_transport;
}

const bdlb::NullableValue<ntsa::Endpoint>& ListenerSocketOptions::
    sourceEndpoint() const
{
    return d_sourceEndpoint;
}

bool ListenerSocketOptions::reuseAddress() const
{
    return d_reuseAddress;
}

const bdlb::NullableValue<bsl::size_t>& ListenerSocketOptions::backlog() const
{
    return d_backlog;
}

const bdlb::NullableValue<bsl::size_t>& ListenerSocketOptions::
    acceptQueueLowWatermark() const
{
    return d_acceptQueueLowWatermark;
}

const bdlb::NullableValue<bsl::size_t>& ListenerSocketOptions::
    acceptQueueHighWatermark() const
{
    return d_acceptQueueHighWatermark;
}

const bdlb::NullableValue<bsl::size_t>& ListenerSocketOptions::
    readQueueLowWatermark() const
{
    return d_readQueueLowWatermark;
}

const bdlb::NullableValue<bsl::size_t>& ListenerSocketOptions::
    readQueueHighWatermark() const
{
    return d_readQueueHighWatermark;
}

const bdlb::NullableValue<bsl::size_t>& ListenerSocketOptions::
    writeQueueLowWatermark() const
{
    return d_writeQueueLowWatermark;
}

const bdlb::NullableValue<bsl::size_t>& ListenerSocketOptions::
    writeQueueHighWatermark() const
{
    return d_writeQueueHighWatermark;
}

const bdlb::NullableValue<bsl::size_t>& ListenerSocketOptions::
    minIncomingStreamTransferSize() const
{
    return d_minIncomingStreamTransferSize;
}

const bdlb::NullableValue<bsl::size_t>& ListenerSocketOptions::
    maxIncomingStreamTransferSize() const
{
    return d_maxIncomingStreamTransferSize;
}

const bdlb::NullableValue<bool>& ListenerSocketOptions::acceptGreedily() const
{
    return d_acceptGreedily;
}

const bdlb::NullableValue<bool>& ListenerSocketOptions::sendGreedily() const
{
    return d_sendGreedily;
}

const bdlb::NullableValue<bool>& ListenerSocketOptions::receiveGreedily() const
{
    return d_receiveGreedily;
}

const bdlb::NullableValue<bsl::size_t>& ListenerSocketOptions::sendBufferSize()
    const
{
    return d_sendBufferSize;
}

const bdlb::NullableValue<bsl::size_t>& ListenerSocketOptions::
    receiveBufferSize() const
{
    return d_receiveBufferSize;
}

const bdlb::NullableValue<bsl::size_t>& ListenerSocketOptions::
    sendBufferLowWatermark() const
{
    return d_sendBufferLowWatermark;
}

const bdlb::NullableValue<bsl::size_t>& ListenerSocketOptions::
    receiveBufferLowWatermark() const
{
    return d_receiveBufferLowWatermark;
}

const bdlb::NullableValue<bsl::size_t>& ListenerSocketOptions::sendTimeout()
    const
{
    return d_sendTimeout;
}

const bdlb::NullableValue<bsl::size_t>& ListenerSocketOptions::receiveTimeout()
    const
{
    return d_receiveTimeout;
}

const bdlb::NullableValue<bool>& ListenerSocketOptions::keepAlive() const
{
    return d_keepAlive;
}

const bdlb::NullableValue<bool>& ListenerSocketOptions::noDelay() const
{
    return d_noDelay;
}

const bdlb::NullableValue<bool>& ListenerSocketOptions::debugFlag() const
{
    return d_debugFlag;
}

const bdlb::NullableValue<bool>& ListenerSocketOptions::allowBroadcasting()
    const
{
    return d_allowBroadcasting;
}

const bdlb::NullableValue<bool>& ListenerSocketOptions::bypassNormalRouting()
    const
{
    return d_bypassNormalRouting;
}

const bdlb::NullableValue<bool>& ListenerSocketOptions::
    leaveOutofBandDataInline() const
{
    return d_leaveOutOfBandDataInline;
}

const bdlb::NullableValue<bool>& ListenerSocketOptions::lingerFlag() const
{
    return d_lingerFlag;
}

const bdlb::NullableValue<bsl::size_t>& ListenerSocketOptions::lingerTimeout()
    const
{
    return d_lingerTimeout;
}

const bdlb::NullableValue<bool>& ListenerSocketOptions::keepHalfOpen() const
{
    return d_keepHalfOpen;
}

const bdlb::NullableValue<bool>& ListenerSocketOptions::metrics() const
{
    return d_metrics;
}

const bdlb::NullableValue<bool>& ListenerSocketOptions::timestampOutgoingData()
    const
{
    return d_timestampOutgoingData;
}

const bdlb::NullableValue<bool>& ListenerSocketOptions::timestampIncomingData()
    const
{
    return d_timestampIncomingData;
}

const bdlb::NullableValue<bsl::size_t>& ListenerSocketOptions::
    zeroCopyThreshold() const
{
    return d_zeroCopyThreshold;
}

const ntca::LoadBalancingOptions& ListenerSocketOptions::loadBalancingOptions()
    const
{
    return d_loadBalancingOptions;
}

bsl::ostream& ListenerSocketOptions::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("transport", d_transport);
    printer.printAttribute("sourceEndpoint", d_sourceEndpoint);
    printer.printAttribute("reuseAddress", d_reuseAddress);
    printer.printAttribute("backlog", d_backlog);
    printer.printAttribute("acceptQueueLowWatermark",
                           d_acceptQueueLowWatermark);
    printer.printAttribute("acceptQueueHighWatermark",
                           d_acceptQueueHighWatermark);
    printer.printAttribute("readQueueLowWatermark", d_readQueueLowWatermark);
    printer.printAttribute("readQueueHighWatermark", d_readQueueHighWatermark);
    printer.printAttribute("writeQueueLowWatermark", d_writeQueueLowWatermark);
    printer.printAttribute("writeQueueHighWatermark",
                           d_writeQueueHighWatermark);
    printer.printAttribute("acceptGreedily", d_acceptGreedily);
    printer.printAttribute("sendGreedily", d_sendGreedily);
    printer.printAttribute("receiveGreedily", d_receiveGreedily);
    printer.printAttribute("sendBufferSize", d_sendBufferSize);
    printer.printAttribute("receiveBufferSize", d_receiveBufferSize);
    printer.printAttribute("sendBufferLowWatermark", d_sendBufferLowWatermark);
    printer.printAttribute("receiveBufferLowWatermark",
                           d_receiveBufferLowWatermark);
    printer.printAttribute("sendTimeout", d_sendTimeout);
    printer.printAttribute("receiveTimeout", d_receiveTimeout);
    printer.printAttribute("keepAlive", d_keepAlive);
    printer.printAttribute("noDelay", d_noDelay);
    printer.printAttribute("debugFlag", d_debugFlag);
    printer.printAttribute("allowBroadcasting", d_allowBroadcasting);
    printer.printAttribute("bypassNormalRouting", d_bypassNormalRouting);
    printer.printAttribute("leaveOutOfBandDataInline",
                           d_leaveOutOfBandDataInline);
    printer.printAttribute("lingerFlag", d_lingerFlag);
    printer.printAttribute("lingerTimeout", d_lingerTimeout);
    printer.printAttribute("keepHalfOpen", d_keepHalfOpen);
    printer.printAttribute("metrics", d_metrics);
    printer.printAttribute("timestampOutgoingData", d_timestampOutgoingData);
    printer.printAttribute("timestampIncomingData", d_timestampIncomingData);
    printer.printAttribute("zeroCopyThreshold", d_zeroCopyThreshold);
    printer.printAttribute("loadBalancingOptions", d_loadBalancingOptions);
    printer.end();
    return stream;
}

bool operator==(const ListenerSocketOptions& lhs,
                const ListenerSocketOptions& rhs)
{
    return lhs.transport() == rhs.transport() &&
           lhs.sourceEndpoint() == rhs.sourceEndpoint() &&
           lhs.reuseAddress() == rhs.reuseAddress() &&
           lhs.backlog() == rhs.backlog() &&
           lhs.acceptQueueLowWatermark() == rhs.acceptQueueLowWatermark() &&
           lhs.acceptQueueHighWatermark() == rhs.acceptQueueHighWatermark() &&
           lhs.readQueueLowWatermark() == rhs.readQueueLowWatermark() &&
           lhs.readQueueHighWatermark() == rhs.readQueueHighWatermark() &&
           lhs.writeQueueLowWatermark() == rhs.writeQueueLowWatermark() &&
           lhs.writeQueueHighWatermark() == rhs.writeQueueHighWatermark() &&
           lhs.acceptGreedily() == rhs.acceptGreedily() &&
           lhs.sendGreedily() == rhs.sendGreedily() &&
           lhs.receiveGreedily() == rhs.receiveGreedily() &&
           lhs.sendBufferSize() == rhs.sendBufferSize() &&
           lhs.receiveBufferSize() == rhs.receiveBufferSize() &&
           lhs.sendBufferLowWatermark() == rhs.sendBufferLowWatermark() &&
           lhs.receiveBufferLowWatermark() ==
               rhs.receiveBufferLowWatermark() &&
           lhs.sendTimeout() == rhs.sendTimeout() &&
           lhs.receiveTimeout() == rhs.receiveTimeout() &&
           lhs.keepAlive() == rhs.keepAlive() &&
           lhs.noDelay() == rhs.noDelay() &&
           lhs.debugFlag() == rhs.debugFlag() &&
           lhs.allowBroadcasting() == rhs.allowBroadcasting() &&
           lhs.bypassNormalRouting() == rhs.bypassNormalRouting() &&
           lhs.leaveOutofBandDataInline() == rhs.leaveOutofBandDataInline() &&
           lhs.lingerFlag() == rhs.lingerFlag() &&
           lhs.lingerTimeout() == rhs.lingerTimeout() &&
           lhs.keepHalfOpen() == rhs.keepHalfOpen() &&
           lhs.metrics() == rhs.metrics() &&
           lhs.timestampOutgoingData() == rhs.timestampOutgoingData() &&
           lhs.timestampIncomingData() == rhs.timestampIncomingData() &&
           lhs.zeroCopyThreshold() == rhs.zeroCopyThreshold() &&
           lhs.loadBalancingOptions() == rhs.loadBalancingOptions();
}

bool operator!=(const ListenerSocketOptions& lhs,
                const ListenerSocketOptions& rhs)
{
    return !operator==(lhs, rhs);
}

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const ListenerSocketOptions& object)
{
    return object.print(stream, 0, -1);
}

}  // close package namespace
}  // close enterprise namespace
