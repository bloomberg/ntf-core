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

#include <ntca_interfaceconfig.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_interfaceconfig_cpp, "$Id$ $CSID$")

#include <ntccfg_limits.h>

namespace BloombergLP {
namespace ntca {

InterfaceConfig::InterfaceConfig(bslma::Allocator* basicAllocator)
: d_driverName(basicAllocator)
, d_metricName(basicAllocator)
, d_threadName(basicAllocator)
, d_minThreads(1)
, d_maxThreads(NTCCFG_DEFAULT_MAX_THREADS)
, d_threadStackSize(NTCCFG_DEFAULT_STACK_SIZE)
, d_threadLoadFactor(NTCCFG_DEFAULT_MAX_DESIRED_SOCKETS_PER_THREAD)
, d_maxEventsPerWait()
, d_maxTimersPerWait()
, d_maxCyclesPerWait()
, d_maxConnections()
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
, d_maxDatagramSize()
, d_multicastLoopback()
, d_multicastTimeToLive()
, d_multicastInterface()
, d_dynamicLoadBalancing()
, d_driverMetrics()
, d_driverMetricsPerWaiter()
, d_socketMetrics()
, d_socketMetricsPerHandle()
, d_resolverEnabled()
, d_resolverConfig(basicAllocator)
{
}

InterfaceConfig::InterfaceConfig(const InterfaceConfig& other,
                                 bslma::Allocator*      basicAllocator)
: d_driverName(other.d_driverName, basicAllocator)
, d_metricName(other.d_metricName, basicAllocator)
, d_threadName(other.d_threadName, basicAllocator)
, d_minThreads(other.d_minThreads)
, d_maxThreads(other.d_maxThreads)
, d_threadStackSize(other.d_threadStackSize)
, d_threadLoadFactor(other.d_threadLoadFactor)
, d_maxEventsPerWait(other.d_maxEventsPerWait)
, d_maxTimersPerWait(other.d_maxTimersPerWait)
, d_maxCyclesPerWait(other.d_maxCyclesPerWait)
, d_maxConnections(other.d_maxConnections)
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
, d_maxDatagramSize(other.d_maxDatagramSize)
, d_multicastLoopback(other.d_multicastLoopback)
, d_multicastTimeToLive(other.d_multicastTimeToLive)
, d_multicastInterface(other.d_multicastInterface)
, d_dynamicLoadBalancing(other.d_dynamicLoadBalancing)
, d_driverMetrics(other.d_driverMetrics)
, d_driverMetricsPerWaiter(other.d_driverMetricsPerWaiter)
, d_socketMetrics(other.d_socketMetrics)
, d_socketMetricsPerHandle(other.d_socketMetricsPerHandle)
, d_resolverEnabled(other.d_resolverEnabled)
, d_resolverConfig(other.d_resolverConfig, basicAllocator)
{
}

InterfaceConfig::~InterfaceConfig()
{
}

InterfaceConfig& InterfaceConfig::operator=(const InterfaceConfig& other)
{
    if (this != &other) {
        d_driverName               = other.d_driverName;
        d_metricName               = other.d_metricName;
        d_threadName               = other.d_threadName;
        d_minThreads               = other.d_minThreads;
        d_maxThreads               = other.d_maxThreads;
        d_threadStackSize          = other.d_threadStackSize;
        d_threadLoadFactor         = other.d_threadLoadFactor;
        d_maxEventsPerWait         = other.d_maxEventsPerWait;
        d_maxTimersPerWait         = other.d_maxTimersPerWait;
        d_maxCyclesPerWait         = other.d_maxCyclesPerWait;
        d_maxConnections           = other.d_maxConnections;
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
        d_maxDatagramSize           = other.d_maxDatagramSize;
        d_multicastLoopback         = other.d_multicastLoopback;
        d_multicastTimeToLive       = other.d_multicastTimeToLive;
        d_multicastInterface        = other.d_multicastInterface;
        d_dynamicLoadBalancing      = other.d_dynamicLoadBalancing;
        d_driverMetrics             = other.d_driverMetrics;
        d_driverMetricsPerWaiter    = other.d_driverMetricsPerWaiter;
        d_socketMetrics             = other.d_socketMetrics;
        d_socketMetricsPerHandle    = other.d_socketMetricsPerHandle;
        d_resolverEnabled           = other.d_resolverEnabled;
        d_resolverConfig            = other.d_resolverConfig;
    }

    return *this;
}

void InterfaceConfig::setDriverName(const bslstl::StringRef& driverName)
{
    d_driverName = driverName;
}

void InterfaceConfig::setMetricName(const bslstl::StringRef& metricName)
{
    d_metricName = metricName;
}

void InterfaceConfig::setThreadName(const bslstl::StringRef& threadName)
{
    d_threadName = threadName;
}

void InterfaceConfig::setMinThreads(bsl::size_t minThreads)
{
    d_minThreads = minThreads;
}

void InterfaceConfig::setMaxThreads(bsl::size_t maxThreads)
{
    d_maxThreads = maxThreads;
}

void InterfaceConfig::setThreadStackSize(bsl::size_t threadStackSize)
{
    d_threadStackSize = threadStackSize;
}

void InterfaceConfig::setThreadLoadFactor(bsl::size_t threadLoadFactor)
{
    d_threadLoadFactor = threadLoadFactor;
}

void InterfaceConfig::setMaxEventsPerWait(bsl::size_t value)
{
    d_maxEventsPerWait = value;
}

void InterfaceConfig::setMaxTimersPerWait(bsl::size_t value)
{
    d_maxTimersPerWait = value;
}

void InterfaceConfig::setMaxCyclesPerWait(bsl::size_t value)
{
    d_maxCyclesPerWait = value;
}

void InterfaceConfig::setMaxConnections(bsl::size_t value)
{
    d_maxConnections = value;
}

void InterfaceConfig::setBacklog(bsl::size_t value)
{
    d_backlog = value;
}

void InterfaceConfig::setAcceptQueueLowWatermark(bsl::size_t value)
{
    d_acceptQueueLowWatermark = value;
}

void InterfaceConfig::setAcceptQueueHighWatermark(bsl::size_t value)
{
    d_acceptQueueHighWatermark = value;
}

void InterfaceConfig::setReadQueueLowWatermark(bsl::size_t value)
{
    d_readQueueLowWatermark = value;
}

void InterfaceConfig::setReadQueueHighWatermark(bsl::size_t value)
{
    d_readQueueHighWatermark = value;
}

void InterfaceConfig::setWriteQueueLowWatermark(bsl::size_t value)
{
    d_writeQueueLowWatermark = value;
}

void InterfaceConfig::setWriteQueueHighWatermark(bsl::size_t value)
{
    d_writeQueueHighWatermark = value;
}

void InterfaceConfig::setMinIncomingStreamTransferSize(bsl::size_t value)
{
    d_minIncomingStreamTransferSize = value;
}

void InterfaceConfig::setMaxIncomingStreamTransferSize(bsl::size_t value)
{
    d_maxIncomingStreamTransferSize = value;
}

void InterfaceConfig::setAcceptGreedily(bool value)
{
    d_acceptGreedily = value;
}

void InterfaceConfig::setSendGreedily(bool value)
{
    d_sendGreedily = value;
}

void InterfaceConfig::setReceiveGreedily(bool value)
{
    d_receiveGreedily = value;
}

void InterfaceConfig::setSendBufferSize(bsl::size_t value)
{
    d_sendBufferSize = value;
}

void InterfaceConfig::setReceiveBufferSize(bsl::size_t value)
{
    d_receiveBufferSize = value;
}

void InterfaceConfig::setSendBufferLowWatermark(bsl::size_t value)
{
    d_sendBufferLowWatermark = value;
}

void InterfaceConfig::setReceiveBufferLowWatermark(bsl::size_t value)
{
    d_receiveBufferLowWatermark = value;
}

void InterfaceConfig::setSendTimeout(bsl::size_t value)
{
    d_sendTimeout = value;
}

void InterfaceConfig::setReceiveTimeout(bsl::size_t value)
{
    d_receiveTimeout = value;
}

void InterfaceConfig::setKeepAlive(bool value)
{
    d_keepAlive = value;
}

void InterfaceConfig::setNoDelay(bool value)
{
    d_noDelay = value;
}

void InterfaceConfig::setDebugFlag(bool value)
{
    d_debugFlag = value;
}

void InterfaceConfig::setAllowBroadcasting(bool value)
{
    d_allowBroadcasting = value;
}

void InterfaceConfig::setBypassNormalRouting(bool value)
{
    d_bypassNormalRouting = value;
}

void InterfaceConfig::setLeaveOutofBandDataInline(bool value)
{
    d_leaveOutOfBandDataInline = value;
}

void InterfaceConfig::setLingerFlag(bool value)
{
    d_lingerFlag = value;
}

void InterfaceConfig::setLingerTimeout(bsl::size_t value)
{
    d_lingerTimeout = value;
}

void InterfaceConfig::setKeepHalfOpen(bool value)
{
    d_keepHalfOpen = value;
}

void InterfaceConfig::setMaxDatagramSize(bsl::size_t value)
{
    d_maxDatagramSize = value;
}

void InterfaceConfig::setMulticastLoopback(bool value)
{
    d_multicastLoopback = value;
}

void InterfaceConfig::setMulticastTimeToLive(bsl::size_t value)
{
    d_multicastTimeToLive = value;
}

void InterfaceConfig::setMulticastInterface(const ntsa::IpAddress& value)
{
    d_multicastInterface = value;
}

void InterfaceConfig::setDynamicLoadBalancing(bool value)
{
    d_dynamicLoadBalancing = value;
}

void InterfaceConfig::setDriverMetrics(bool value)
{
    d_driverMetrics = value;
}

void InterfaceConfig::setDriverMetricsPerWaiter(bool value)
{
    d_driverMetricsPerWaiter = value;
}

void InterfaceConfig::setSocketMetrics(bool value)
{
    d_socketMetrics = value;
}

void InterfaceConfig::setSocketMetricsPerHandle(bool value)
{
    d_socketMetricsPerHandle = value;
}

void InterfaceConfig::setResolverEnabled(bool value)
{
    d_resolverEnabled = value;
}

void InterfaceConfig::setResolverConfig(const ntca::ResolverConfig& value)
{
    d_resolverConfig = value;
}

const bsl::string& InterfaceConfig::driverName() const
{
    return d_driverName;
}

const bsl::string& InterfaceConfig::metricName() const
{
    return d_metricName;
}

const bsl::string& InterfaceConfig::threadName() const
{
    return d_threadName;
}

bsl::size_t InterfaceConfig::minThreads() const
{
    return d_minThreads;
}

bsl::size_t InterfaceConfig::maxThreads() const
{
    return d_maxThreads;
}

bsl::size_t InterfaceConfig::threadStackSize() const
{
    return d_threadStackSize;
}

bsl::size_t InterfaceConfig::threadLoadFactor() const
{
    return d_threadLoadFactor;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::maxEventsPerWait()
    const
{
    return d_maxEventsPerWait;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::maxTimersPerWait()
    const
{
    return d_maxTimersPerWait;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::maxCyclesPerWait()
    const
{
    return d_maxCyclesPerWait;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::maxConnections() const
{
    return d_maxConnections;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::backlog() const
{
    return d_backlog;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::
    acceptQueueLowWatermark() const
{
    return d_acceptQueueLowWatermark;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::
    acceptQueueHighWatermark() const
{
    return d_acceptQueueHighWatermark;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::
    readQueueLowWatermark() const
{
    return d_readQueueLowWatermark;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::
    readQueueHighWatermark() const
{
    return d_readQueueHighWatermark;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::
    writeQueueLowWatermark() const
{
    return d_writeQueueLowWatermark;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::
    writeQueueHighWatermark() const
{
    return d_writeQueueHighWatermark;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::
    minIncomingStreamTransferSize() const
{
    return d_minIncomingStreamTransferSize;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::
    maxIncomingStreamTransferSize() const
{
    return d_maxIncomingStreamTransferSize;
}

const bdlb::NullableValue<bool>& InterfaceConfig::acceptGreedily() const
{
    return d_acceptGreedily;
}

const bdlb::NullableValue<bool>& InterfaceConfig::sendGreedily() const
{
    return d_sendGreedily;
}

const bdlb::NullableValue<bool>& InterfaceConfig::receiveGreedily() const
{
    return d_receiveGreedily;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::sendBufferSize() const
{
    return d_sendBufferSize;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::receiveBufferSize()
    const
{
    return d_receiveBufferSize;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::
    sendBufferLowWatermark() const
{
    return d_sendBufferLowWatermark;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::
    receiveBufferLowWatermark() const
{
    return d_receiveBufferLowWatermark;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::sendTimeout() const
{
    return d_sendTimeout;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::receiveTimeout() const
{
    return d_receiveTimeout;
}

const bdlb::NullableValue<bool>& InterfaceConfig::keepAlive() const
{
    return d_keepAlive;
}

const bdlb::NullableValue<bool>& InterfaceConfig::noDelay() const
{
    return d_noDelay;
}

const bdlb::NullableValue<bool>& InterfaceConfig::debugFlag() const
{
    return d_debugFlag;
}

const bdlb::NullableValue<bool>& InterfaceConfig::allowBroadcasting() const
{
    return d_allowBroadcasting;
}

const bdlb::NullableValue<bool>& InterfaceConfig::bypassNormalRouting() const
{
    return d_bypassNormalRouting;
}

const bdlb::NullableValue<bool>& InterfaceConfig::leaveOutofBandDataInline()
    const
{
    return d_leaveOutOfBandDataInline;
}

const bdlb::NullableValue<bool>& InterfaceConfig::lingerFlag() const
{
    return d_lingerFlag;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::lingerTimeout() const
{
    return d_lingerTimeout;
}

const bdlb::NullableValue<bool>& InterfaceConfig::keepHalfOpen() const
{
    return d_keepHalfOpen;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::maxDatagramSize()
    const
{
    return d_maxDatagramSize;
}

const bdlb::NullableValue<bool>& InterfaceConfig::multicastLoopback() const
{
    return d_multicastLoopback;
}

const bdlb::NullableValue<bsl::size_t>& InterfaceConfig::multicastTimeToLive()
    const
{
    return d_multicastTimeToLive;
}

const bdlb::NullableValue<ntsa::IpAddress>& InterfaceConfig::
    multicastInterface() const
{
    return d_multicastInterface;
}

const bdlb::NullableValue<bool>& InterfaceConfig::dynamicLoadBalancing() const
{
    return d_dynamicLoadBalancing;
}

const bdlb::NullableValue<bool>& InterfaceConfig::driverMetrics() const
{
    return d_driverMetrics;
}

const bdlb::NullableValue<bool>& InterfaceConfig::driverMetricsPerWaiter()
    const
{
    return d_driverMetricsPerWaiter;
}

const bdlb::NullableValue<bool>& InterfaceConfig::socketMetrics() const
{
    return d_socketMetrics;
}

const bdlb::NullableValue<bool>& InterfaceConfig::socketMetricsPerHandle()
    const
{
    return d_socketMetricsPerHandle;
}

const bdlb::NullableValue<bool>& InterfaceConfig::resolverEnabled() const
{
    return d_resolverEnabled;
}

const bdlb::NullableValue<ntca::ResolverConfig>& InterfaceConfig::
    resolverConfig() const
{
    return d_resolverConfig;
}

}  // close package namespace
}  // close enterprise namespace
