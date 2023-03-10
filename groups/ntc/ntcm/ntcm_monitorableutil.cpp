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

#include <ntcm_monitorableutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcm_monitorableutil_cpp, "$Id$ $CSID$")

#include <ntcm_collector.h>
#include <ntcm_logpublisher.h>
#include <ntcm_monitorableregistry.h>
#include <ntcm_periodiccollector.h>

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_unordered_map.h>

namespace BloombergLP {
namespace ntcm {

namespace {

bsls::SpinLock s_monitorableRegistryLock = BSLS_SPINLOCK_UNLOCKED;
bsl::shared_ptr<ntci::MonitorableRegistry> s_monitorableRegistry_sp;

bsls::SpinLock s_monitorableCollectorLock = BSLS_SPINLOCK_UNLOCKED;
bsl::shared_ptr<ntci::MonitorableCollector> s_monitorableCollector_sp;

bsls::SpinLock s_monitorableSystemLock = BSLS_SPINLOCK_UNLOCKED;
bsl::shared_ptr<ntci::Monitorable> s_monitorableSystem_sp;

bsls::SpinLock s_logPublisherLock = BSLS_SPINLOCK_UNLOCKED;
bsl::shared_ptr<ntcm::LogPublisher> s_logPublisher_sp;

}  // close unnamed namespace

void MonitorableUtil::initialize()
{
}

void MonitorableUtil::enableMonitorableRegistry(
    const ntca::MonitorableRegistryConfig& configuration,
    bslma::Allocator*                      basicAllocator)
{
    bslma::Allocator* allocator =
        basicAllocator ? basicAllocator : bslma::Default::globalAllocator();

    bsl::shared_ptr<ntcm::MonitorableRegistry> monitorableRegistry;
    monitorableRegistry.createInplace(allocator, configuration, allocator);

    bsls::SpinLockGuard guard(&s_monitorableRegistryLock);
    s_monitorableRegistry_sp = monitorableRegistry;
}

void MonitorableUtil::enableMonitorableRegistry(
    const bsl::shared_ptr<ntci::MonitorableRegistry>& monitorableRegistry)
{
    bsls::SpinLockGuard guard(&s_monitorableRegistryLock);
    s_monitorableRegistry_sp = monitorableRegistry;
}

void MonitorableUtil::disableMonitorableRegistry()
{
    {
        bsls::SpinLockGuard guard(&s_monitorableSystemLock);
        s_monitorableSystem_sp.reset();
    }

    {
        bsls::SpinLockGuard guard(&s_monitorableRegistryLock);
        s_monitorableRegistry_sp.reset();
    }
}

void MonitorableUtil::enableMonitorableCollector(
    const ntca::MonitorableCollectorConfig& configuration,
    bslma::Allocator*                       basicAllocator)
{
    bslma::Allocator* allocator =
        basicAllocator ? basicAllocator : bslma::Default::globalAllocator();

    bsl::shared_ptr<ntci::MonitorableCollector> monitorableCollector;

    if (!configuration.period().isNull() && configuration.period().value() > 0)
    {
        bsl::shared_ptr<ntcm::PeriodicCollector> concreteMonitorableCollector;
        concreteMonitorableCollector.createInplace(
            allocator,
            configuration,
            ntcm::MonitorableUtil::loadCallback(),
            allocator);
        monitorableCollector = concreteMonitorableCollector;
    }
    else {
        bsl::shared_ptr<ntcm::Collector> concreteMonitorableCollector;
        concreteMonitorableCollector.createInplace(
            allocator,
            configuration,
            ntcm::MonitorableUtil::loadCallback(),
            allocator);
        monitorableCollector = concreteMonitorableCollector;
    }

    monitorableCollector->start();

    bsl::shared_ptr<ntci::MonitorableCollector> previousMonitorableCollector;
    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        previousMonitorableCollector = s_monitorableCollector_sp;
        s_monitorableCollector_sp    = monitorableCollector;
    }

    if (previousMonitorableCollector) {
        previousMonitorableCollector->stop();
    }
}

void MonitorableUtil::enableMonitorableCollector(
    const bsl::shared_ptr<ntci::MonitorableCollector>& monitorableCollector)
{
    bsl::shared_ptr<ntci::MonitorableCollector> previousMonitorableCollector;
    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        previousMonitorableCollector = s_monitorableCollector_sp;
        s_monitorableCollector_sp    = monitorableCollector;
    }

    if (previousMonitorableCollector &&
        previousMonitorableCollector != monitorableCollector)
    {
        previousMonitorableCollector->stop();
    }
}

void MonitorableUtil::disableMonitorableCollector()
{
    bsl::shared_ptr<ntci::MonitorableCollector> previousMonitorableCollector;
    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        previousMonitorableCollector = s_monitorableCollector_sp;
        s_monitorableCollector_sp.reset();
    }

    if (previousMonitorableCollector) {
        previousMonitorableCollector->stop();
    }
}

void MonitorableUtil::registerMonitorable(
    const bsl::shared_ptr<ntci::Monitorable>& monitorable)
{
    bsl::shared_ptr<ntci::MonitorableRegistry> monitorableRegistry;
    {
        bsls::SpinLockGuard guard(&s_monitorableRegistryLock);
        monitorableRegistry = s_monitorableRegistry_sp;
    }

    if (monitorableRegistry) {
        monitorableRegistry->registerMonitorable(monitorable);
    }
}

void MonitorableUtil::deregisterMonitorable(
    const bsl::shared_ptr<ntci::Monitorable>& monitorable)
{
    bsl::shared_ptr<ntci::MonitorableRegistry> monitorableRegistry;
    {
        bsls::SpinLockGuard guard(&s_monitorableRegistryLock);
        monitorableRegistry = s_monitorableRegistry_sp;
    }

    if (monitorableRegistry) {
        monitorableRegistry->deregisterMonitorable(monitorable);
    }
}

void MonitorableUtil::registerMonitorableProcess(
    const bsl::shared_ptr<ntci::Monitorable>& monitorable)
{
    {
        bsls::SpinLockGuard guard(&s_monitorableSystemLock);
        if (s_monitorableSystem_sp) {
            return;
        }

        s_monitorableSystem_sp = monitorable;
    }

    MonitorableUtil::registerMonitorable(monitorable);
}

void MonitorableUtil::deregisterMonitorableProcess()
{
    bsl::shared_ptr<ntci::Monitorable> monitorable;

    {
        bsls::SpinLockGuard guard(&s_monitorableSystemLock);
        if (!s_monitorableSystem_sp) {
            return;
        }

        s_monitorableSystem_sp.swap(monitorable);
    }

    MonitorableUtil::deregisterMonitorable(monitorable);
}

void MonitorableUtil::registerMonitorablePublisher(
    const bsl::shared_ptr<ntci::MonitorablePublisher>& monitorablePublisher)
{
    bsl::shared_ptr<ntci::MonitorableCollector> monitorableCollector;
    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        monitorableCollector = s_monitorableCollector_sp;
    }

    if (monitorableCollector) {
        monitorableCollector->registerPublisher(monitorablePublisher);
    }
}

void MonitorableUtil::deregisterMonitorablePublisher(
    const bsl::shared_ptr<ntci::MonitorablePublisher>& monitorablePublisher)
{
    bsl::shared_ptr<ntci::MonitorableCollector> monitorableCollector;
    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        monitorableCollector = s_monitorableCollector_sp;
    }

    if (monitorableCollector) {
        monitorableCollector->deregisterPublisher(monitorablePublisher);
    }
}

void MonitorableUtil::registerMonitorablePublisher(
    bsls::LogSeverity::Enum severityLevel)
{
    bsl::shared_ptr<ntci::MonitorableCollector> monitorableCollector;
    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        monitorableCollector = s_monitorableCollector_sp;
    }

    bsl::shared_ptr<ntcm::LogPublisher> logPublisher;
    {
        bsls::SpinLockGuard guard(&s_logPublisherLock);

        if (s_logPublisher_sp) {
            s_logPublisher_sp->setSeverityLevel(severityLevel);
            return;
        }
        else {
            s_logPublisher_sp.createInplace(bslma::Default::globalAllocator(),
                                            severityLevel,
                                            bslma::Default::globalAllocator());
            logPublisher = s_logPublisher_sp;
        }
    }

    if (monitorableCollector) {
        monitorableCollector->registerPublisher(logPublisher);
    }
}

void MonitorableUtil::deregisterMonitorablePublisher(
    bsls::LogSeverity::Enum severityLevel)
{
    NTCCFG_WARNING_UNUSED(severityLevel);

    bsl::shared_ptr<ntci::MonitorableCollector> monitorableCollector;
    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        monitorableCollector = s_monitorableCollector_sp;
    }

    bsl::shared_ptr<ntcm::LogPublisher> logPublisher;
    {
        bsls::SpinLockGuard guard(&s_logPublisherLock);
        logPublisher.swap(s_logPublisher_sp);
    }

    if (monitorableCollector) {
        monitorableCollector->deregisterPublisher(logPublisher);
    }
}

void MonitorableUtil::collectMetrics()
{
    bsl::shared_ptr<ntci::MonitorableCollector> monitorableCollector;
    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        monitorableCollector = s_monitorableCollector_sp;
    }

    if (monitorableCollector) {
        monitorableCollector->collect();
    }
}

void MonitorableUtil::loadRegisteredObjects(
    bsl::vector<bsl::shared_ptr<ntci::Monitorable> >* result)
{
    bsl::shared_ptr<ntci::MonitorableRegistry> monitorableRegistry;
    {
        bsls::SpinLockGuard guard(&s_monitorableRegistryLock);
        monitorableRegistry = s_monitorableRegistry_sp;
    }

    if (monitorableRegistry) {
        monitorableRegistry->loadRegisteredObjects(result);
    }
}

MonitorableUtil::LoadCallback MonitorableUtil::loadCallback()
{
    return &MonitorableUtil::loadRegisteredObjects;
}

void MonitorableUtil::exit()
{
    {
        bsls::SpinLockGuard guard(&s_logPublisherLock);
        s_logPublisher_sp.reset();
    }

    {
        bsls::SpinLockGuard guard(&s_monitorableSystemLock);
        s_monitorableSystem_sp.reset();
    }

    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        s_monitorableCollector_sp.reset();
    }

    {
        bsls::SpinLockGuard guard(&s_monitorableRegistryLock);
        s_monitorableRegistry_sp.reset();
    }
}

}  // close package namespace
}  // close enterprise namespace
