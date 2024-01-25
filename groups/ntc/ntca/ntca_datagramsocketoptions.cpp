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

#include <ntca_datagramsocketoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_datagramsocketoptions_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

DatagramSocketOptions::DatagramSocketOptions()
: d_transport(ntsa::Transport::e_UNDEFINED)
, d_sourceEndpoint()
, d_reuseAddress(false)
, d_maxDatagramSize()
, d_readQueueLowWatermark()
, d_readQueueHighWatermark()
, d_writeQueueLowWatermark()
, d_writeQueueHighWatermark()
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
, d_multicastLoopback()
, d_multicastTimeToLive()
, d_multicastInterface()
, d_keepHalfOpen()
, d_metrics()
, d_timestampOutgoingData()
, d_timestampIncomingData()
, d_zeroCopyThreshold()
, d_loadBalancingOptions()
{
}

DatagramSocketOptions::DatagramSocketOptions(
    const DatagramSocketOptions& other)
: d_transport(other.d_transport)
, d_sourceEndpoint(other.d_sourceEndpoint)
, d_reuseAddress(other.d_reuseAddress)
, d_maxDatagramSize(other.d_maxDatagramSize)
, d_readQueueLowWatermark(other.d_readQueueLowWatermark)
, d_readQueueHighWatermark(other.d_readQueueHighWatermark)
, d_writeQueueLowWatermark(other.d_writeQueueLowWatermark)
, d_writeQueueHighWatermark(other.d_writeQueueHighWatermark)
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
, d_multicastLoopback(other.d_multicastLoopback)
, d_multicastTimeToLive(other.d_multicastTimeToLive)
, d_multicastInterface(other.d_multicastInterface)
, d_keepHalfOpen(other.d_keepHalfOpen)
, d_metrics(other.d_metrics)
, d_timestampOutgoingData(other.d_timestampOutgoingData)
, d_timestampIncomingData(other.d_timestampIncomingData)
, d_zeroCopyThreshold(other.d_zeroCopyThreshold)
, d_loadBalancingOptions(other.d_loadBalancingOptions)
{
}

DatagramSocketOptions::~DatagramSocketOptions()
{
}

DatagramSocketOptions& DatagramSocketOptions::operator=(
    const DatagramSocketOptions& other)
{
    if (this != &other) {
        d_transport                 = other.d_transport;
        d_sourceEndpoint            = other.d_sourceEndpoint;
        d_reuseAddress              = other.d_reuseAddress;
        d_maxDatagramSize           = other.d_maxDatagramSize;
        d_readQueueLowWatermark     = other.d_readQueueLowWatermark;
        d_readQueueHighWatermark    = other.d_readQueueHighWatermark;
        d_writeQueueLowWatermark    = other.d_writeQueueLowWatermark;
        d_writeQueueHighWatermark   = other.d_writeQueueHighWatermark;
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
        d_multicastLoopback         = other.d_multicastLoopback;
        d_multicastTimeToLive       = other.d_multicastTimeToLive;
        d_multicastInterface        = other.d_multicastInterface;
        d_keepHalfOpen              = other.d_keepHalfOpen;
        d_metrics                   = other.d_metrics;
        d_timestampOutgoingData     = other.d_timestampOutgoingData;
        d_timestampIncomingData     = other.d_timestampIncomingData;
        d_zeroCopyThreshold         = other.d_zeroCopyThreshold;
        d_loadBalancingOptions      = other.d_loadBalancingOptions;
    }

    return *this;
}

void DatagramSocketOptions::setTransport(ntsa::Transport::Value value)
{
    d_transport = value;
}

void DatagramSocketOptions::setSourceEndpoint(const ntsa::Endpoint& value)
{
    d_sourceEndpoint = value;
    if (d_transport == ntsa::Transport::e_UNDEFINED) {
        d_transport = value.transport(ntsa::TransportMode::e_DATAGRAM);
    }
}

void DatagramSocketOptions::setReuseAddress(bool value)
{
    d_reuseAddress = value;
}

void DatagramSocketOptions::setMaxDatagramSize(bsl::size_t value)
{
    d_maxDatagramSize = value;
}

void DatagramSocketOptions::setReadQueueLowWatermark(bsl::size_t value)
{
    d_readQueueLowWatermark = value;
}

void DatagramSocketOptions::setReadQueueHighWatermark(bsl::size_t value)
{
    d_readQueueHighWatermark = value;
}

void DatagramSocketOptions::setWriteQueueLowWatermark(bsl::size_t value)
{
    d_writeQueueLowWatermark = value;
}

void DatagramSocketOptions::setWriteQueueHighWatermark(bsl::size_t value)
{
    d_writeQueueHighWatermark = value;
}

void DatagramSocketOptions::setSendGreedily(bool value)
{
    d_sendGreedily = value;
}

void DatagramSocketOptions::setReceiveGreedily(bool value)
{
    d_receiveGreedily = value;
}

void DatagramSocketOptions::setSendBufferSize(bsl::size_t value)
{
    d_sendBufferSize = value;
}

void DatagramSocketOptions::setReceiveBufferSize(bsl::size_t value)
{
    d_receiveBufferSize = value;
}

void DatagramSocketOptions::setSendBufferLowWatermark(bsl::size_t value)
{
    d_sendBufferLowWatermark = value;
}

void DatagramSocketOptions::setReceiveBufferLowWatermark(bsl::size_t value)
{
    d_receiveBufferLowWatermark = value;
}

void DatagramSocketOptions::setSendTimeout(bsl::size_t value)
{
    d_sendTimeout = value;
}

void DatagramSocketOptions::setReceiveTimeout(bsl::size_t value)
{
    d_receiveTimeout = value;
}

void DatagramSocketOptions::setKeepAlive(bool value)
{
    d_keepAlive = value;
}

void DatagramSocketOptions::setNoDelay(bool value)
{
    d_noDelay = value;
}

void DatagramSocketOptions::setDebugFlag(bool value)
{
    d_debugFlag = value;
}

void DatagramSocketOptions::setAllowBroadcasting(bool value)
{
    d_allowBroadcasting = value;
}

void DatagramSocketOptions::setBypassNormalRouting(bool value)
{
    d_bypassNormalRouting = value;
}

void DatagramSocketOptions::setLeaveOutofBandDataInline(bool value)
{
    d_leaveOutOfBandDataInline = value;
}

void DatagramSocketOptions::setLingerFlag(bool value)
{
    d_lingerFlag = value;
}

void DatagramSocketOptions::setLingerTimeout(bsl::size_t value)
{
    d_lingerTimeout = value;
}

void DatagramSocketOptions::setMulticastLoopback(bool value)
{
    d_multicastLoopback = value;
}

void DatagramSocketOptions::setMulticastTimeToLive(bsl::size_t value)
{
    d_multicastTimeToLive = value;
}

void DatagramSocketOptions::setMulticastInterface(const ntsa::IpAddress& value)
{
    d_multicastInterface = value;
}

void DatagramSocketOptions::setKeepHalfOpen(bool value)
{
    d_keepHalfOpen = value;
}

void DatagramSocketOptions::setMetrics(bool value)
{
    d_metrics = value;
}

void DatagramSocketOptions::setTimestampOutgoingData(bool value)
{
    d_timestampOutgoingData = value;
}

void DatagramSocketOptions::setTimestampIncomingData(bool value)
{
    d_timestampIncomingData = value;
}

void DatagramSocketOptions::setZeroCopyThreshold(bsl::size_t value)
{
    d_zeroCopyThreshold = value;
}

void DatagramSocketOptions::setLoadBalancingOptions(
    const ntca::LoadBalancingOptions& value)
{
    d_loadBalancingOptions = value;
}

ntsa::Transport::Value DatagramSocketOptions::transport() const
{
    return d_transport;
}

const bdlb::NullableValue<ntsa::Endpoint>& DatagramSocketOptions::
    sourceEndpoint() const
{
    return d_sourceEndpoint;
}

bool DatagramSocketOptions::reuseAddress() const
{
    return d_reuseAddress;
}

const bdlb::NullableValue<bsl::size_t>& DatagramSocketOptions::
    maxDatagramSize() const
{
    return d_maxDatagramSize;
}

const bdlb::NullableValue<bsl::size_t>& DatagramSocketOptions::
    readQueueLowWatermark() const
{
    return d_readQueueLowWatermark;
}

const bdlb::NullableValue<bsl::size_t>& DatagramSocketOptions::
    readQueueHighWatermark() const
{
    return d_readQueueHighWatermark;
}

const bdlb::NullableValue<bsl::size_t>& DatagramSocketOptions::
    writeQueueLowWatermark() const
{
    return d_writeQueueLowWatermark;
}

const bdlb::NullableValue<bsl::size_t>& DatagramSocketOptions::
    writeQueueHighWatermark() const
{
    return d_writeQueueHighWatermark;
}

const bdlb::NullableValue<bool>& DatagramSocketOptions::sendGreedily() const
{
    return d_sendGreedily;
}

const bdlb::NullableValue<bool>& DatagramSocketOptions::receiveGreedily() const
{
    return d_receiveGreedily;
}

const bdlb::NullableValue<bsl::size_t>& DatagramSocketOptions::sendBufferSize()
    const
{
    return d_sendBufferSize;
}

const bdlb::NullableValue<bsl::size_t>& DatagramSocketOptions::
    receiveBufferSize() const
{
    return d_receiveBufferSize;
}

const bdlb::NullableValue<bsl::size_t>& DatagramSocketOptions::
    sendBufferLowWatermark() const
{
    return d_sendBufferLowWatermark;
}

const bdlb::NullableValue<bsl::size_t>& DatagramSocketOptions::
    receiveBufferLowWatermark() const
{
    return d_receiveBufferLowWatermark;
}

const bdlb::NullableValue<bsl::size_t>& DatagramSocketOptions::sendTimeout()
    const
{
    return d_sendTimeout;
}

const bdlb::NullableValue<bsl::size_t>& DatagramSocketOptions::receiveTimeout()
    const
{
    return d_receiveTimeout;
}

const bdlb::NullableValue<bool>& DatagramSocketOptions::keepAlive() const
{
    return d_keepAlive;
}

const bdlb::NullableValue<bool>& DatagramSocketOptions::noDelay() const
{
    return d_noDelay;
}

const bdlb::NullableValue<bool>& DatagramSocketOptions::debugFlag() const
{
    return d_debugFlag;
}

const bdlb::NullableValue<bool>& DatagramSocketOptions::allowBroadcasting()
    const
{
    return d_allowBroadcasting;
}

const bdlb::NullableValue<bool>& DatagramSocketOptions::bypassNormalRouting()
    const
{
    return d_bypassNormalRouting;
}

const bdlb::NullableValue<bool>& DatagramSocketOptions::
    leaveOutofBandDataInline() const
{
    return d_leaveOutOfBandDataInline;
}

const bdlb::NullableValue<bool>& DatagramSocketOptions::lingerFlag() const
{
    return d_lingerFlag;
}

const bdlb::NullableValue<bsl::size_t>& DatagramSocketOptions::lingerTimeout()
    const
{
    return d_lingerTimeout;
}

const bdlb::NullableValue<bool>& DatagramSocketOptions::multicastLoopback()
    const
{
    return d_multicastLoopback;
}

const bdlb::NullableValue<bsl::size_t>& DatagramSocketOptions::
    multicastTimeToLive() const
{
    return d_multicastTimeToLive;
}

const bdlb::NullableValue<ntsa::IpAddress>& DatagramSocketOptions::
    multicastInterface() const
{
    return d_multicastInterface;
}

const bdlb::NullableValue<bool>& DatagramSocketOptions::keepHalfOpen() const
{
    return d_keepHalfOpen;
}

const bdlb::NullableValue<bool>& DatagramSocketOptions::metrics() const
{
    return d_metrics;
}

const ntca::LoadBalancingOptions& DatagramSocketOptions::loadBalancingOptions()
    const
{
    return d_loadBalancingOptions;
}

const bdlb::NullableValue<bool>& DatagramSocketOptions::timestampOutgoingData()
    const
{
    return d_timestampOutgoingData;
}

const bdlb::NullableValue<bool>& DatagramSocketOptions::timestampIncomingData()
    const
{
    return d_timestampIncomingData;
}

const bdlb::NullableValue<bsl::size_t>& DatagramSocketOptions::
    zeroCopyThreshold() const
{
    return d_zeroCopyThreshold;
}

bsl::ostream& DatagramSocketOptions::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("transport", d_transport);
    printer.printAttribute("sourceEndpoint", d_sourceEndpoint);
    printer.printAttribute("reuseAddress", d_reuseAddress);
    printer.printAttribute("maxDatagramSize", d_maxDatagramSize);
    printer.printAttribute("readQueueLowWatermark", d_readQueueLowWatermark);
    printer.printAttribute("readQueueHighWatermark", d_readQueueHighWatermark);
    printer.printAttribute("writeQueueLowWatermark", d_writeQueueLowWatermark);
    printer.printAttribute("writeQueueHighWatermark",
                           d_writeQueueHighWatermark);
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
    printer.printAttribute("multicastLoopback", d_multicastLoopback);
    printer.printAttribute("multicastTimeToLive", d_multicastTimeToLive);
    printer.printAttribute("multicastInterface", d_multicastInterface);
    printer.printAttribute("keepHalfOpen", d_keepHalfOpen);
    printer.printAttribute("metrics", d_metrics);
    printer.printAttribute("timestampOutgoingData", d_timestampOutgoingData);
    printer.printAttribute("timestampIncomingData", d_timestampIncomingData);
    printer.printAttribute("zeroCopyThreshold", d_zeroCopyThreshold);
    printer.printAttribute("loadBalancingOptions", d_loadBalancingOptions);
    printer.end();
    return stream;
}

bool operator==(const DatagramSocketOptions& lhs,
                const DatagramSocketOptions& rhs)
{
    return lhs.transport() == rhs.transport() &&
           lhs.sourceEndpoint() == rhs.sourceEndpoint() &&
           lhs.reuseAddress() == rhs.reuseAddress() &&
           lhs.maxDatagramSize() == rhs.maxDatagramSize() &&
           lhs.readQueueLowWatermark() == rhs.readQueueLowWatermark() &&
           lhs.readQueueHighWatermark() == rhs.readQueueHighWatermark() &&
           lhs.writeQueueLowWatermark() == rhs.writeQueueLowWatermark() &&
           lhs.writeQueueHighWatermark() == rhs.writeQueueHighWatermark() &&
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
           lhs.multicastLoopback() == rhs.multicastLoopback() &&
           lhs.multicastTimeToLive() == rhs.multicastTimeToLive() &&
           lhs.multicastInterface() == rhs.multicastInterface() &&
           lhs.keepHalfOpen() == rhs.keepHalfOpen() &&
           lhs.metrics() == rhs.metrics() &&
           lhs.timestampOutgoingData() == rhs.timestampOutgoingData() &&
           lhs.timestampIncomingData() == rhs.timestampIncomingData() &&
           lhs.zeroCopyThreshold() == rhs.zeroCopyThreshold() &&
           lhs.loadBalancingOptions() == rhs.loadBalancingOptions();
}

bool operator!=(const DatagramSocketOptions& lhs,
                const DatagramSocketOptions& rhs)
{
    return !operator==(lhs, rhs);
}

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const DatagramSocketOptions& object)
{
    return object.print(stream, 0, -1);
}

}  // close package namespace
}  // close enterprise namespace
