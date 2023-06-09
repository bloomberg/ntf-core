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

#include <ntcs_compat.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_compat_cpp, "$Id$ $CSID$")

#include <ntccfg_tune.h>
#include <ntci_log.h>
#include <ntcs_nomenclature.h>
#include <ntsu_socketoptionutil.h>
#include <bsls_atomic.h>

// Define to 1 to configure send timeouts, or define to 0 to ignore the
// requested send timeout configuration.
#define NTCS_COMPAT_CONFIGURE_SEND_TIMEOUT 0

// Define to 1 to configure receive timeouts, or define to 0 to ignore the
// requested receive timeout configuration.
#define NTCS_COMPAT_CONFIGURE_RECEIVE_TIMEOUT 0

namespace BloombergLP {
namespace ntcs {

void Compat::sanitize(ntca::InterfaceConfig* config)
{
    NTCI_LOG_CONTEXT();

    if (config->metricName().empty()) {
        config->setMetricName(config->threadName());
    }

    if (config->threadName().empty()) {
        config->setThreadName(config->metricName());
    }

    if (config->metricName().empty()) {
        config->setMetricName(ntcs::Nomenclature::createInterfaceName());
    }

    if (config->threadName().empty()) {
        config->setThreadName(config->metricName());
    }

    BSLS_ASSERT(!config->metricName().empty());
    BSLS_ASSERT(!config->threadName().empty());

    if (config->minThreads() == 0) {
        config->setMinThreads(1);
    }

    if (config->maxThreads() < config->minThreads()) {
        config->setMaxThreads(config->minThreads());
    }

#if NTC_BUILD_WITH_THREAD_SCALING == 0
    if (config->minThreads() != config->maxThreads()) {
        BSLS_LOG_WARN("Thread scaling is not supported");
        config->setMinThreads(config->maxThreads());
    }
#endif

    if (config->driverName().empty()) {
        bsl::string driverOverride;
        if (ntccfg::Tune::configure(&driverOverride, "NTC_DRIVER")) {
            config->setDriverName(driverOverride);
            NTCI_LOG_WARN("Using driver override '%s'",
                          driverOverride.c_str());
        }
    }

    if (config->dynamicLoadBalancing().isNull()) {
        bool dynamicLoadBalancingOverride;
        if (ntccfg::Tune::configure(&dynamicLoadBalancingOverride,
                                    "NTC_DYNAMIC_LOAD_BALANCING"))
        {
            config->setDynamicLoadBalancing(dynamicLoadBalancingOverride);
            NTCI_LOG_WARN("Using dynamic load balancing override '%d'",
                          (int)(dynamicLoadBalancingOverride));
        }
        else {
            config->setDynamicLoadBalancing(
                NTCCFG_DEFAULT_DYNAMIC_LOAD_BALANCING);
        }
    }

#if NTC_BUILD_WITH_DYNAMIC_LOAD_BALANCING == 0
    if (!config->dynamicLoadBalancing().isNull() &&
        config->dynamicLoadBalancing().value())
    {
        BSLS_LOG_WARN("Dynamic load balancing is not supported");
        config->setDynamicLoadBalancing(false);
    }
#endif

    if (config->driverMetrics().isNull()) {
        bool driverMetrics;
        if (ntccfg::Tune::configure(&driverMetrics, "NTC_DRIVER_METRICS")) {
            config->setDriverMetrics(driverMetrics);
            NTCI_LOG_WARN("Collecting driver metrics '%d'",
                          (int)(driverMetrics));
        }
        else {
            config->setDriverMetrics(NTCCFG_DEFAULT_DRIVER_METRICS);
        }
    }

    if (config->driverMetricsPerWaiter().isNull()) {
        bool driverMetricsPerWaiter;
        if (ntccfg::Tune::configure(&driverMetricsPerWaiter,
                                    "NTC_DRIVER_METRICS_PER_WAITER"))
        {
            config->setDriverMetricsPerWaiter(driverMetricsPerWaiter);
            NTCI_LOG_WARN("Collecting driver metrics per waiter '%d'",
                          (int)(driverMetricsPerWaiter));
        }
        else {
            config->setDriverMetricsPerWaiter(
                NTCCFG_DEFAULT_DRIVER_METRICS_PER_WAITER);
        }
    }

    if (config->socketMetrics().isNull()) {
        bool socketMetrics;
        if (ntccfg::Tune::configure(&socketMetrics, "NTC_SOCKET_METRICS")) {
            config->setSocketMetrics(socketMetrics);
            NTCI_LOG_WARN("Collecting socket metrics '%d'",
                          (int)(socketMetrics));
        }
        else {
            config->setSocketMetrics(NTCCFG_DEFAULT_SOCKET_METRICS);
        }
    }

    if (config->socketMetricsPerHandle().isNull()) {
        bool socketMetricsPerHandle;
        if (ntccfg::Tune::configure(&socketMetricsPerHandle,
                                    "NTC_SOCKET_METRICS_PER_HANDLE"))
        {
            config->setSocketMetricsPerHandle(socketMetricsPerHandle);
            NTCI_LOG_WARN("Collecting socket metrics per handle '%d'",
                          (int)(socketMetricsPerHandle));
        }
        else {
            config->setSocketMetricsPerHandle(
                NTCCFG_DEFAULT_SOCKET_METRICS_PER_HANDLE);
        }
    }

    if (!config->driverMetricsPerWaiter().isNull() &&
        config->driverMetricsPerWaiter().value())
    {
        config->setDriverMetrics(true);
    }

    if (!config->socketMetricsPerHandle().isNull() &&
        config->socketMetricsPerHandle().value())
    {
        config->setSocketMetrics(true);
    }

    if (config->resolverEnabled().isNull()) {
        config->setResolverEnabled(NTCCFG_DEFAULT_RESOLVER_ENABLED);
    }

    if (config->resolverEnabled().value()) {
        if (config->resolverConfig().isNull()) {
            ntca::ResolverConfig resolverConfig;
            config->setResolverConfig(resolverConfig);
        }
    }

    if (config->threadLoadFactor() == 0) {
        config->setThreadLoadFactor(
            NTCCFG_DEFAULT_MAX_DESIRED_SOCKETS_PER_THREAD);
    }

    if (config->threadStackSize() == 0) {
        config->setThreadStackSize(NTCCFG_DEFAULT_STACK_SIZE);
    }
}

void Compat::convert(ntca::StreamSocketOptions*         result,
                     const ntca::ListenerSocketOptions& options)
{
    result->setReuseAddress(options.reuseAddress());

    if (!options.readQueueLowWatermark().isNull()) {
        result->setReadQueueLowWatermark(
            options.readQueueLowWatermark().value());
    }

    if (!options.readQueueHighWatermark().isNull()) {
        result->setReadQueueHighWatermark(
            options.readQueueHighWatermark().value());
    }

    if (!options.writeQueueLowWatermark().isNull()) {
        result->setWriteQueueLowWatermark(
            options.writeQueueLowWatermark().value());
    }

    if (!options.writeQueueHighWatermark().isNull()) {
        result->setWriteQueueHighWatermark(
            options.writeQueueHighWatermark().value());
    }

    if (!options.minIncomingStreamTransferSize().isNull()) {
        result->setMinIncomingStreamTransferSize(
            options.minIncomingStreamTransferSize().value());
    }

    if (!options.maxIncomingStreamTransferSize().isNull()) {
        result->setMaxIncomingStreamTransferSize(
            options.maxIncomingStreamTransferSize().value());
    }

    if (!options.sendGreedily().isNull()) {
        result->setSendGreedily(options.sendGreedily().value());
    }

    if (!options.receiveGreedily().isNull()) {
        result->setReceiveGreedily(options.receiveGreedily().value());
    }

    if (!options.sendBufferSize().isNull()) {
        result->setSendBufferSize(options.sendBufferSize().value());
    }

    if (!options.receiveBufferSize().isNull()) {
        result->setReceiveBufferSize(options.receiveBufferSize().value());
    }

    if (!options.sendBufferLowWatermark().isNull()) {
        result->setSendBufferLowWatermark(
            options.sendBufferLowWatermark().value());
    }

    if (!options.receiveBufferLowWatermark().isNull()) {
        result->setReceiveBufferLowWatermark(
            options.receiveBufferLowWatermark().value());
    }

    if (!options.sendTimeout().isNull()) {
        result->setSendTimeout(options.sendTimeout().value());
    }

    if (!options.receiveTimeout().isNull()) {
        result->setReceiveTimeout(options.receiveTimeout().value());
    }

    if (!options.keepAlive().isNull()) {
        result->setKeepAlive(options.keepAlive().value());
    }

    if (!options.noDelay().isNull()) {
        result->setNoDelay(options.noDelay().value());
    }
    else {
        result->setNoDelay(true);
    }

    if (!options.debugFlag().isNull()) {
        result->setDebugFlag(options.debugFlag().value());
    }

    if (!options.allowBroadcasting().isNull()) {
        result->setAllowBroadcasting(options.allowBroadcasting().value());
    }

    if (!options.bypassNormalRouting().isNull()) {
        result->setBypassNormalRouting(options.bypassNormalRouting().value());
    }

    if (!options.leaveOutofBandDataInline().isNull()) {
        result->setLeaveOutofBandDataInline(
            options.leaveOutofBandDataInline().value());
    }

    if (!options.lingerFlag().isNull()) {
        result->setLingerFlag(options.lingerFlag().value());
    }

    if (!options.lingerTimeout().isNull()) {
        result->setLingerTimeout(options.lingerTimeout().value());
    }

    if (!options.keepHalfOpen().isNull()) {
        result->setKeepHalfOpen(options.keepHalfOpen().value());
    }

    if (!options.metrics().isNull()) {
        result->setMetrics(options.metrics().value());
    }

    if (!options.timestampOutgoingData().isNull()) {
        result->setTimestampOutgoingData(
            options.timestampOutgoingData().value());
    }

    if (!options.timestampIncomingData().isNull()) {
        result->setTimestampIncomingData(
            options.timestampIncomingData().value());
    }

    result->setLoadBalancingOptions(options.loadBalancingOptions());
}

void Compat::convert(ntca::ListenerSocketOptions*     result,
                     const ntca::StreamSocketOptions& options)
{
    result->setReuseAddress(options.reuseAddress());

    if (!options.readQueueLowWatermark().isNull()) {
        result->setReadQueueLowWatermark(
            options.readQueueLowWatermark().value());
    }

    if (!options.readQueueHighWatermark().isNull()) {
        result->setReadQueueHighWatermark(
            options.readQueueHighWatermark().value());
    }

    if (!options.writeQueueLowWatermark().isNull()) {
        result->setWriteQueueLowWatermark(
            options.writeQueueLowWatermark().value());
    }

    if (!options.writeQueueHighWatermark().isNull()) {
        result->setWriteQueueHighWatermark(
            options.writeQueueHighWatermark().value());
    }

    if (!options.minIncomingStreamTransferSize().isNull()) {
        result->setMinIncomingStreamTransferSize(
            options.minIncomingStreamTransferSize().value());
    }

    if (!options.maxIncomingStreamTransferSize().isNull()) {
        result->setMaxIncomingStreamTransferSize(
            options.maxIncomingStreamTransferSize().value());
    }

    if (!options.sendGreedily().isNull()) {
        result->setSendGreedily(options.sendGreedily().value());
    }

    if (!options.receiveGreedily().isNull()) {
        result->setReceiveGreedily(options.receiveGreedily().value());
    }

    if (!options.sendBufferSize().isNull()) {
        result->setSendBufferSize(options.sendBufferSize().value());
    }

    if (!options.receiveBufferSize().isNull()) {
        result->setReceiveBufferSize(options.receiveBufferSize().value());
    }

    if (!options.sendBufferLowWatermark().isNull()) {
        result->setSendBufferLowWatermark(
            options.sendBufferLowWatermark().value());
    }

    if (!options.receiveBufferLowWatermark().isNull()) {
        result->setReceiveBufferLowWatermark(
            options.receiveBufferLowWatermark().value());
    }

    if (!options.sendTimeout().isNull()) {
        result->setSendTimeout(options.sendTimeout().value());
    }

    if (!options.receiveTimeout().isNull()) {
        result->setReceiveTimeout(options.receiveTimeout().value());
    }

    if (!options.keepAlive().isNull()) {
        result->setKeepAlive(options.keepAlive().value());
    }

    if (!options.noDelay().isNull()) {
        result->setNoDelay(options.noDelay().value());
    }
    else {
        result->setNoDelay(true);
    }

    if (!options.debugFlag().isNull()) {
        result->setDebugFlag(options.debugFlag().value());
    }

    if (!options.allowBroadcasting().isNull()) {
        result->setAllowBroadcasting(options.allowBroadcasting().value());
    }

    if (!options.bypassNormalRouting().isNull()) {
        result->setBypassNormalRouting(options.bypassNormalRouting().value());
    }

    if (!options.leaveOutofBandDataInline().isNull()) {
        result->setLeaveOutofBandDataInline(
            options.leaveOutofBandDataInline().value());
    }

    if (!options.lingerFlag().isNull()) {
        result->setLingerFlag(options.lingerFlag().value());
    }

    if (!options.lingerTimeout().isNull()) {
        result->setLingerTimeout(options.lingerTimeout().value());
    }

    if (!options.keepHalfOpen().isNull()) {
        result->setKeepHalfOpen(options.keepHalfOpen().value());
    }

    if (!options.metrics().isNull()) {
        result->setMetrics(options.metrics().value());
    }

    if (!options.timestampOutgoingData().isNull()) {
        result->setTimestampOutgoingData(
            options.timestampOutgoingData().value());
    }

    if (!options.timestampIncomingData().isNull()) {
        result->setTimestampIncomingData(
            options.timestampIncomingData().value());
    }

    result->setLoadBalancingOptions(options.loadBalancingOptions());
}

void Compat::convert(ntca::DatagramSocketOptions*       result,
                     const ntca::DatagramSocketOptions& options,
                     const ntca::InterfaceConfig&       config)
{
    *result = options;

    if (result->maxDatagramSize().isNull()) {
        if (!config.maxDatagramSize().isNull()) {
            result->setMaxDatagramSize(config.maxDatagramSize().value());
        }
    }

    if (result->multicastLoopback().isNull()) {
        if (!config.multicastLoopback().isNull()) {
            result->setMulticastLoopback(config.multicastLoopback().value());
        }
    }

    if (result->multicastTimeToLive().isNull()) {
        if (!config.multicastTimeToLive().isNull()) {
            result->setMulticastTimeToLive(
                config.multicastTimeToLive().value());
        }
    }

    if (result->multicastInterface().isNull()) {
        if (!config.multicastInterface().isNull()) {
            result->setMulticastInterface(config.multicastInterface().value());
        }
    }

    if (result->sendGreedily().isNull()) {
        if (!config.sendGreedily().isNull()) {
            result->setSendGreedily(config.sendGreedily().value());
        }
    }

    if (result->receiveGreedily().isNull()) {
        if (!config.receiveGreedily().isNull()) {
            result->setReceiveGreedily(config.receiveGreedily().value());
        }
    }

    if (result->sendBufferSize().isNull()) {
        if (!config.sendBufferSize().isNull()) {
            result->setSendBufferSize(config.sendBufferSize().value());
        }
    }

    if (result->receiveBufferSize().isNull()) {
        if (!config.receiveBufferSize().isNull()) {
            result->setReceiveBufferSize(config.receiveBufferSize().value());
        }
    }

    if (result->sendBufferLowWatermark().isNull()) {
        if (!config.sendBufferLowWatermark().isNull()) {
            result->setSendBufferLowWatermark(
                config.sendBufferLowWatermark().value());
        }
    }

    if (result->receiveBufferLowWatermark().isNull()) {
        if (!config.receiveBufferLowWatermark().isNull()) {
            result->setReceiveBufferLowWatermark(
                config.receiveBufferLowWatermark().value());
        }
    }

    if (result->sendTimeout().isNull()) {
        if (!config.sendTimeout().isNull()) {
            result->setSendTimeout(config.sendTimeout().value());
        }
    }

    if (result->receiveTimeout().isNull()) {
        if (!config.receiveTimeout().isNull()) {
            result->setReceiveTimeout(config.receiveTimeout().value());
        }
    }

    if (result->keepAlive().isNull()) {
        if (!config.keepAlive().isNull()) {
            result->setKeepAlive(config.keepAlive().value());
        }
    }

    if (result->noDelay().isNull()) {
        if (!config.noDelay().isNull()) {
            result->setNoDelay(config.noDelay().value());
        }
        else {
            // result->setNoDelay(true);
        }
    }

    if (result->debugFlag().isNull()) {
        if (!config.debugFlag().isNull()) {
            result->setDebugFlag(config.debugFlag().value());
        }
    }

    if (result->allowBroadcasting().isNull()) {
        if (!config.allowBroadcasting().isNull()) {
            result->setAllowBroadcasting(config.allowBroadcasting().value());
        }
    }

    if (result->bypassNormalRouting().isNull()) {
        if (!config.bypassNormalRouting().isNull()) {
            result->setBypassNormalRouting(
                config.bypassNormalRouting().value());
        }
    }

    if (result->leaveOutofBandDataInline().isNull()) {
        if (!config.leaveOutofBandDataInline().isNull()) {
            result->setLeaveOutofBandDataInline(
                config.leaveOutofBandDataInline().value());
        }
    }

    if (result->lingerFlag().isNull()) {
        if (!config.lingerFlag().isNull()) {
            result->setLingerFlag(config.lingerFlag().value());
        }
    }

    if (result->lingerTimeout().isNull()) {
        if (!config.lingerTimeout().isNull()) {
            result->setLingerTimeout(config.lingerTimeout().value());
        }
    }

    if (result->keepHalfOpen().isNull()) {
        if (!config.keepHalfOpen().isNull()) {
            result->setKeepHalfOpen(config.keepHalfOpen().value());
        }
    }

    if (result->metrics().isNull()) {
        if (!config.socketMetricsPerHandle().isNull()) {
            result->setMetrics(config.socketMetricsPerHandle().value());
        }
    }
}

void Compat::convert(ntca::ListenerSocketOptions*       result,
                     const ntca::ListenerSocketOptions& options,
                     const ntca::InterfaceConfig&       config)
{
    *result = options;

    if (result->backlog().isNull()) {
        if (!config.backlog().isNull()) {
            result->setBacklog(config.backlog().value());
        }
    }

    if (result->acceptQueueLowWatermark().isNull()) {
        if (!config.acceptQueueLowWatermark().isNull()) {
            result->setAcceptQueueLowWatermark(
                config.acceptQueueLowWatermark().value());
        }
    }

    if (result->acceptQueueHighWatermark().isNull()) {
        if (!config.acceptQueueHighWatermark().isNull()) {
            result->setAcceptQueueHighWatermark(
                config.acceptQueueHighWatermark().value());
        }
    }

    if (result->readQueueLowWatermark().isNull()) {
        if (!config.readQueueLowWatermark().isNull()) {
            result->setReadQueueLowWatermark(
                config.readQueueLowWatermark().value());
        }
    }

    if (result->readQueueHighWatermark().isNull()) {
        if (!config.readQueueHighWatermark().isNull()) {
            result->setReadQueueHighWatermark(
                config.readQueueHighWatermark().value());
        }
    }

    if (result->writeQueueLowWatermark().isNull()) {
        if (!config.writeQueueLowWatermark().isNull()) {
            result->setWriteQueueLowWatermark(
                config.writeQueueLowWatermark().value());
        }
    }

    if (result->writeQueueHighWatermark().isNull()) {
        if (!config.writeQueueHighWatermark().isNull()) {
            result->setWriteQueueHighWatermark(
                config.writeQueueHighWatermark().value());
        }
    }

    if (result->minIncomingStreamTransferSize().isNull()) {
        if (!config.minIncomingStreamTransferSize().isNull()) {
            result->setMinIncomingStreamTransferSize(
                config.minIncomingStreamTransferSize().value());
        }
    }

    if (result->maxIncomingStreamTransferSize().isNull()) {
        if (!config.maxIncomingStreamTransferSize().isNull()) {
            result->setMaxIncomingStreamTransferSize(
                config.maxIncomingStreamTransferSize().value());
        }
    }

    if (result->acceptGreedily().isNull()) {
        if (!config.acceptGreedily().isNull()) {
            result->setAcceptGreedily(config.acceptGreedily().value());
        }
    }

    if (result->sendGreedily().isNull()) {
        if (!config.sendGreedily().isNull()) {
            result->setSendGreedily(config.sendGreedily().value());
        }
    }

    if (result->receiveGreedily().isNull()) {
        if (!config.receiveGreedily().isNull()) {
            result->setReceiveGreedily(config.receiveGreedily().value());
        }
    }

    if (result->sendBufferSize().isNull()) {
        if (!config.sendBufferSize().isNull()) {
            result->setSendBufferSize(config.sendBufferSize().value());
        }
    }

    if (result->receiveBufferSize().isNull()) {
        if (!config.receiveBufferSize().isNull()) {
            result->setReceiveBufferSize(config.receiveBufferSize().value());
        }
    }

    if (result->sendBufferLowWatermark().isNull()) {
        if (!config.sendBufferLowWatermark().isNull()) {
            result->setSendBufferLowWatermark(
                config.sendBufferLowWatermark().value());
        }
    }

    if (result->receiveBufferLowWatermark().isNull()) {
        if (!config.receiveBufferLowWatermark().isNull()) {
            result->setReceiveBufferLowWatermark(
                config.receiveBufferLowWatermark().value());
        }
    }

    if (result->sendTimeout().isNull()) {
        if (!config.sendTimeout().isNull()) {
            result->setSendTimeout(config.sendTimeout().value());
        }
    }

    if (result->receiveTimeout().isNull()) {
        if (!config.receiveTimeout().isNull()) {
            result->setReceiveTimeout(config.receiveTimeout().value());
        }
    }

    if (result->keepAlive().isNull()) {
        if (!config.keepAlive().isNull()) {
            result->setKeepAlive(config.keepAlive().value());
        }
    }

    if (result->noDelay().isNull()) {
        if (!config.noDelay().isNull()) {
            result->setNoDelay(config.noDelay().value());
        }
        else {
            result->setNoDelay(true);
        }
    }

    if (result->debugFlag().isNull()) {
        if (!config.debugFlag().isNull()) {
            result->setDebugFlag(config.debugFlag().value());
        }
    }

    if (result->allowBroadcasting().isNull()) {
        if (!config.allowBroadcasting().isNull()) {
            result->setAllowBroadcasting(config.allowBroadcasting().value());
        }
    }

    if (result->bypassNormalRouting().isNull()) {
        if (!config.bypassNormalRouting().isNull()) {
            result->setBypassNormalRouting(
                config.bypassNormalRouting().value());
        }
    }

    if (result->leaveOutofBandDataInline().isNull()) {
        if (!config.leaveOutofBandDataInline().isNull()) {
            result->setLeaveOutofBandDataInline(
                config.leaveOutofBandDataInline().value());
        }
    }

    if (result->lingerFlag().isNull()) {
        if (!config.lingerFlag().isNull()) {
            result->setLingerFlag(config.lingerFlag().value());
        }
    }

    if (result->lingerTimeout().isNull()) {
        if (!config.lingerTimeout().isNull()) {
            result->setLingerTimeout(config.lingerTimeout().value());
        }
    }

    if (result->keepHalfOpen().isNull()) {
        if (!config.keepHalfOpen().isNull()) {
            result->setKeepHalfOpen(config.keepHalfOpen().value());
        }
    }

    if (result->metrics().isNull()) {
        if (!config.socketMetricsPerHandle().isNull()) {
            result->setMetrics(config.socketMetricsPerHandle().value());
        }
    }
}

void Compat::convert(ntca::StreamSocketOptions*       result,
                     const ntca::StreamSocketOptions& options,
                     const ntca::InterfaceConfig&     config)
{
    *result = options;

    if (result->readQueueLowWatermark().isNull()) {
        if (!config.readQueueLowWatermark().isNull()) {
            result->setReadQueueLowWatermark(
                config.readQueueLowWatermark().value());
        }
    }

    if (result->readQueueHighWatermark().isNull()) {
        if (!config.readQueueHighWatermark().isNull()) {
            result->setReadQueueHighWatermark(
                config.readQueueHighWatermark().value());
        }
    }

    if (result->writeQueueLowWatermark().isNull()) {
        if (!config.writeQueueLowWatermark().isNull()) {
            result->setWriteQueueLowWatermark(
                config.writeQueueLowWatermark().value());
        }
    }

    if (result->writeQueueHighWatermark().isNull()) {
        if (!config.writeQueueHighWatermark().isNull()) {
            result->setWriteQueueHighWatermark(
                config.writeQueueHighWatermark().value());
        }
    }

    if (result->minIncomingStreamTransferSize().isNull()) {
        if (!config.minIncomingStreamTransferSize().isNull()) {
            result->setMinIncomingStreamTransferSize(
                config.minIncomingStreamTransferSize().value());
        }
    }

    if (result->maxIncomingStreamTransferSize().isNull()) {
        if (!config.maxIncomingStreamTransferSize().isNull()) {
            result->setMaxIncomingStreamTransferSize(
                config.maxIncomingStreamTransferSize().value());
        }
    }

    if (result->sendGreedily().isNull()) {
        if (!config.sendGreedily().isNull()) {
            result->setSendGreedily(config.sendGreedily().value());
        }
    }

    if (result->receiveGreedily().isNull()) {
        if (!config.receiveGreedily().isNull()) {
            result->setReceiveGreedily(config.receiveGreedily().value());
        }
    }

    if (result->sendBufferSize().isNull()) {
        if (!config.sendBufferSize().isNull()) {
            result->setSendBufferSize(config.sendBufferSize().value());
        }
    }

    if (result->receiveBufferSize().isNull()) {
        if (!config.receiveBufferSize().isNull()) {
            result->setReceiveBufferSize(config.receiveBufferSize().value());
        }
    }

    if (result->sendBufferLowWatermark().isNull()) {
        if (!config.sendBufferLowWatermark().isNull()) {
            result->setSendBufferLowWatermark(
                config.sendBufferLowWatermark().value());
        }
    }

    if (result->receiveBufferLowWatermark().isNull()) {
        if (!config.receiveBufferLowWatermark().isNull()) {
            result->setReceiveBufferLowWatermark(
                config.receiveBufferLowWatermark().value());
        }
    }

    if (result->sendTimeout().isNull()) {
        if (!config.sendTimeout().isNull()) {
            result->setSendTimeout(config.sendTimeout().value());
        }
    }

    if (result->receiveTimeout().isNull()) {
        if (!config.receiveTimeout().isNull()) {
            result->setReceiveTimeout(config.receiveTimeout().value());
        }
    }

    if (result->keepAlive().isNull()) {
        if (!config.keepAlive().isNull()) {
            result->setKeepAlive(config.keepAlive().value());
        }
    }

    if (result->noDelay().isNull()) {
        if (!config.noDelay().isNull()) {
            result->setNoDelay(config.noDelay().value());
        }
        else {
            result->setNoDelay(true);
        }
    }

    if (result->debugFlag().isNull()) {
        if (!config.debugFlag().isNull()) {
            result->setDebugFlag(config.debugFlag().value());
        }
    }

    if (result->allowBroadcasting().isNull()) {
        if (!config.allowBroadcasting().isNull()) {
            result->setAllowBroadcasting(config.allowBroadcasting().value());
        }
    }

    if (result->bypassNormalRouting().isNull()) {
        if (!config.bypassNormalRouting().isNull()) {
            result->setBypassNormalRouting(
                config.bypassNormalRouting().value());
        }
    }

    if (result->leaveOutofBandDataInline().isNull()) {
        if (!config.leaveOutofBandDataInline().isNull()) {
            result->setLeaveOutofBandDataInline(
                config.leaveOutofBandDataInline().value());
        }
    }

    if (result->lingerFlag().isNull()) {
        if (!config.lingerFlag().isNull()) {
            result->setLingerFlag(config.lingerFlag().value());
        }
    }

    if (result->lingerTimeout().isNull()) {
        if (!config.lingerTimeout().isNull()) {
            result->setLingerTimeout(config.lingerTimeout().value());
        }
    }

    if (result->keepHalfOpen().isNull()) {
        if (!config.keepHalfOpen().isNull()) {
            result->setKeepHalfOpen(config.keepHalfOpen().value());
        }
    }

    if (result->metrics().isNull()) {
        if (!config.socketMetricsPerHandle().isNull()) {
            result->setMetrics(config.socketMetricsPerHandle().value());
        }
    }
}

void Compat::convert(ntca::GetEndpointOptions* result,
                     const ntca::BindOptions&  options)
{
    if (!options.ipAddressFallback().isNull()) {
        result->setIpAddressFallback(options.ipAddressFallback().value());
    }

    if (!options.ipAddressType().isNull()) {
        result->setIpAddressType(options.ipAddressType().value());
    }

    if (!options.ipAddressSelector().isNull()) {
        result->setIpAddressSelector(options.ipAddressSelector().value());
    }

    if (!options.portFallback().isNull()) {
        result->setPortFallback(options.portFallback().value());
    }

    if (!options.portSelector().isNull()) {
        result->setPortSelector(options.portSelector().value());
    }

    if (!options.transport().isNull()) {
        result->setTransport(options.transport().value());
    }

    if (!options.deadline().isNull()) {
        result->setDeadline(options.deadline().value());
    }
}

void Compat::convert(ntca::GetEndpointOptions*   result,
                     const ntca::ConnectOptions& options)
{
    if (!options.ipAddressFallback().isNull()) {
        result->setIpAddressFallback(options.ipAddressFallback().value());
    }

    if (!options.ipAddressType().isNull()) {
        result->setIpAddressType(options.ipAddressType().value());
    }

    if (!options.ipAddressSelector().isNull()) {
        result->setIpAddressSelector(options.ipAddressSelector().value());
    }

    if (!options.portFallback().isNull()) {
        result->setPortFallback(options.portFallback().value());
    }

    if (!options.portSelector().isNull()) {
        result->setPortSelector(options.portSelector().value());
    }

    if (!options.transport().isNull()) {
        result->setTransport(options.transport().value());
    }

    if (!options.deadline().isNull()) {
        result->setDeadline(options.deadline().value());
    }
}

ntsa::Error Compat::configure(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket,
    const ntca::DatagramSocketOptions&           options)
{
    ntsa::Error error;

    error = socket->setBlocking(false);
    if (error) {
        BSLS_LOG_DEBUG("Failed to set non-blocking mode: %s",
                       error.text().c_str());
        return error;
    }

    {
        ntsa::SocketOption option;
        option.makeReuseAddress(options.reuseAddress());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "reuse address: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.multicastLoopback().isNull()) {
        error =
            socket->setMulticastLoopback(options.multicastLoopback().value());
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "multicast loopback: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.multicastTimeToLive().isNull()) {
        error = socket->setMulticastTimeToLive(
            options.multicastTimeToLive().value());
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "multicast time to live: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.multicastInterface().isNull()) {
        error = socket->setMulticastInterface(
            options.multicastInterface().value());
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "multicast interface: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.sendBufferSize().isNull()) {
        ntsa::SocketOption option;
        option.makeSendBufferSize(options.sendBufferSize().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "send buffer size: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.receiveBufferSize().isNull()) {
        ntsa::SocketOption option;
        option.makeReceiveBufferSize(options.receiveBufferSize().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "receive buffer size: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.sendBufferLowWatermark().isNull()) {
        ntsa::SocketOption option;
        option.makeSendBufferLowWatermark(
            options.sendBufferLowWatermark().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "send buffer low watermark: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.receiveBufferLowWatermark().isNull()) {
        ntsa::SocketOption option;
        option.makeReceiveBufferLowWatermark(
            options.receiveBufferLowWatermark().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "receive buffer low watermark: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

#if NTCS_COMPAT_CONFIGURE_SEND_TIMEOUT
    if (!options.sendTimeout().isNull()) {
        ntsa::SocketOption option;
        option.makeSendTimeout(options.sendTimeout().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "send timeout: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }
#endif

#if NTCS_COMPAT_CONFIGURE_RECEIVE_TIMEOUT
    if (!options.receiveTimeout().isNull()) {
        ntsa::SocketOption option;
        option.makeReceiveTimeout(options.receiveTimeout().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "receive timeout: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }
#endif

    if (!options.keepAlive().isNull()) {
        ntsa::SocketOption option;
        option.makeKeepAlive(options.keepAlive().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "keep alive: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.noDelay().isNull()) {
        ntsa::SocketOption option;
        option.makeDelayTransmission(!options.noDelay().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "no delay: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.debugFlag().isNull()) {
        ntsa::SocketOption option;
        option.makeDebug(options.debugFlag().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "debug flag: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.allowBroadcasting().isNull()) {
        ntsa::SocketOption option;
        option.makeBroadcast(options.allowBroadcasting().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "broadcast: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.bypassNormalRouting().isNull()) {
        ntsa::SocketOption option;
        option.makeBypassRouting(options.bypassNormalRouting().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "bypass routing: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.leaveOutofBandDataInline().isNull()) {
        ntsa::SocketOption option;
        option.makeInlineOutOfBandData(
            options.leaveOutofBandDataInline().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "inline out-of-band data: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.lingerFlag().isNull()) {
        ntsa::Linger linger;
        linger.setEnabled(options.lingerFlag().value());

        if (!options.lingerTimeout().isNull()) {
            bsls::TimeInterval lingerTimeout;
            lingerTimeout.setTotalSeconds(options.lingerTimeout().value());
            linger.setDuration(lingerTimeout);
        }

        ntsa::SocketOption option;
        option.makeLinger(linger);

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "linger: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.timestampOutgoingData().isNull()) {
        ntsa::SocketOption option;
        option.makeTimestampOutgoingData(
            options.timestampOutgoingData().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "timestamp outcoming data: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.timestampIncomingData().isNull()) {
        ntsa::SocketOption option;
        option.makeTimestampIncomingData(
            options.timestampIncomingData().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "timestamp incoming data: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error Compat::configure(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket,
    const ntca::ListenerSocketOptions&           options)
{
    ntsa::Error error;

    error = socket->setBlocking(false);
    if (error) {
        BSLS_LOG_DEBUG("Failed to set non-blocking mode: %s",
                       error.text().c_str());
        return error;
    }

    {
        ntsa::SocketOption option;
        option.makeReuseAddress(options.reuseAddress());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "reuse address: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.sendBufferSize().isNull()) {
        ntsa::SocketOption option;
        option.makeSendBufferSize(options.sendBufferSize().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "send buffer size: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.receiveBufferSize().isNull()) {
        ntsa::SocketOption option;
        option.makeReceiveBufferSize(options.receiveBufferSize().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "receive buffer size: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.sendBufferLowWatermark().isNull()) {
        ntsa::SocketOption option;
        option.makeSendBufferLowWatermark(
            options.sendBufferLowWatermark().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "send buffer low watermark: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.receiveBufferLowWatermark().isNull()) {
        ntsa::SocketOption option;
        option.makeReceiveBufferLowWatermark(
            options.receiveBufferLowWatermark().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "receive buffer low watermark: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

#if NTCS_COMPAT_CONFIGURE_SEND_TIMEOUT
    if (!options.sendTimeout().isNull()) {
        ntsa::SocketOption option;
        option.makeSendTimeout(options.sendTimeout().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "send timeout: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }
#endif

#if NTCS_COMPAT_CONFIGURE_RECEIVE_TIMEOUT
    if (!options.receiveTimeout().isNull()) {
        ntsa::SocketOption option;
        option.makeReceiveTimeout(options.receiveTimeout().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "receive timeout: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }
#endif

    if (!options.keepAlive().isNull()) {
        ntsa::SocketOption option;
        option.makeKeepAlive(options.keepAlive().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "keep alive: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.noDelay().isNull()) {
        ntsa::SocketOption option;
        option.makeDelayTransmission(!options.noDelay().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "no delay: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.debugFlag().isNull()) {
        ntsa::SocketOption option;
        option.makeDebug(options.debugFlag().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "debug flag: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.allowBroadcasting().isNull()) {
        ntsa::SocketOption option;
        option.makeBroadcast(options.allowBroadcasting().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "broadcast: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.bypassNormalRouting().isNull()) {
        ntsa::SocketOption option;
        option.makeBypassRouting(options.bypassNormalRouting().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "bypass routing: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.leaveOutofBandDataInline().isNull()) {
        ntsa::SocketOption option;
        option.makeInlineOutOfBandData(
            options.leaveOutofBandDataInline().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "inline out-of-band data: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.lingerFlag().isNull()) {
        ntsa::Linger linger;
        linger.setEnabled(options.lingerFlag().value());

        if (!options.lingerTimeout().isNull()) {
            bsls::TimeInterval lingerTimeout;
            lingerTimeout.setTotalSeconds(options.lingerTimeout().value());
            linger.setDuration(lingerTimeout);
        }

        ntsa::SocketOption option;
        option.makeLinger(linger);

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "linger: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error Compat::configure(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket,
    const ntca::StreamSocketOptions&           options)
{
    ntsa::Error error;

    error = socket->setBlocking(false);
    if (error) {
        BSLS_LOG_DEBUG("Failed to set non-blocking mode: %s",
                       error.text().c_str());
        return error;
    }

    {
        ntsa::SocketOption option;
        option.makeReuseAddress(options.reuseAddress());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "reuse address: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.sendBufferSize().isNull()) {
        ntsa::SocketOption option;
        option.makeSendBufferSize(options.sendBufferSize().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "send buffer size: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.receiveBufferSize().isNull()) {
        ntsa::SocketOption option;
        option.makeReceiveBufferSize(options.receiveBufferSize().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "receive buffer size: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.sendBufferLowWatermark().isNull()) {
        ntsa::SocketOption option;
        option.makeSendBufferLowWatermark(
            options.sendBufferLowWatermark().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "send buffer low watermark: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.receiveBufferLowWatermark().isNull()) {
        ntsa::SocketOption option;
        option.makeReceiveBufferLowWatermark(
            options.receiveBufferLowWatermark().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "receive buffer low watermark: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

#if NTCS_COMPAT_CONFIGURE_SEND_TIMEOUT
    if (!options.sendTimeout().isNull()) {
        ntsa::SocketOption option;
        option.makeSendTimeout(options.sendTimeout().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "send timeout: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }
#endif

#if NTCS_COMPAT_CONFIGURE_RECEIVE_TIMEOUT
    if (!options.receiveTimeout().isNull()) {
        ntsa::SocketOption option;
        option.makeReceiveTimeout(options.receiveTimeout().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "receive timeout: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }
#endif

    if (!options.keepAlive().isNull()) {
        ntsa::SocketOption option;
        option.makeKeepAlive(options.keepAlive().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "keep alive: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.noDelay().isNull()) {
        ntsa::SocketOption option;
        option.makeDelayTransmission(!options.noDelay().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "no delay: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.debugFlag().isNull()) {
        ntsa::SocketOption option;
        option.makeDebug(options.debugFlag().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "debug flag: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.allowBroadcasting().isNull()) {
        ntsa::SocketOption option;
        option.makeBroadcast(options.allowBroadcasting().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "broadcast: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.bypassNormalRouting().isNull()) {
        ntsa::SocketOption option;
        option.makeBypassRouting(options.bypassNormalRouting().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "bypass routing: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.leaveOutofBandDataInline().isNull()) {
        ntsa::SocketOption option;
        option.makeInlineOutOfBandData(
            options.leaveOutofBandDataInline().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "inline out-of-band data: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.lingerFlag().isNull()) {
        ntsa::Linger linger;
        linger.setEnabled(options.lingerFlag().value());

        if (!options.lingerTimeout().isNull()) {
            bsls::TimeInterval lingerTimeout;
            lingerTimeout.setTotalSeconds(options.lingerTimeout().value());
            linger.setDuration(lingerTimeout);
        }

        ntsa::SocketOption option;
        option.makeLinger(linger);

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "linger: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    if (!options.timestampIncomingData().isNull()) {
        ntsa::SocketOption option;
        option.makeTimestampIncomingData(
            options.timestampIncomingData().value());

        error = socket->setOption(option);
        if (error) {
            BSLS_LOG_DEBUG("Failed to set socket option: "
                           "timestamp incoming data: %s",
                           error.text().c_str());
            if (error != ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED)) {
                return error;
            }
        }
    }

    return ntsa::Error();
}

}  // close package namespace
}  // close enterprise namespace
