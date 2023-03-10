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

#include <ntcm_periodiccollector.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcm_periodiccollector_cpp, "$Id$ $CSID$")

#include <ntci_log.h>
#include <bdlf_memfn.h>

namespace BloombergLP {
namespace ntcm {

namespace {

const bsl::size_t k_DEFAULT_INTERVAL = 30;

}  // close unnamed namespace

PeriodicCollector::PeriodicCollector(const bsls::TimeInterval& interval,
                                     const LoadCallback&       loadCallback,
                                     bslma::Allocator*         basicAllocator)
: d_scheduler(basicAllocator)
, d_event()
, d_interval(interval)
, d_collector(loadCallback, basicAllocator)
{
}

PeriodicCollector::PeriodicCollector(
    const ntca::MonitorableCollectorConfig& configuration,
    const LoadCallback&                     loadCallback,
    bslma::Allocator*                       basicAllocator)
: d_scheduler(basicAllocator)
, d_event()
, d_interval(0)
, d_collector(configuration, loadCallback, basicAllocator)
{
    if (!configuration.period().isNull()) {
        d_interval = bsls::TimeInterval(configuration.period().value(), 0);
    }
    else {
        d_interval = bsls::TimeInterval(k_DEFAULT_INTERVAL, 0);
    }
}

PeriodicCollector::~PeriodicCollector()
{
}

void PeriodicCollector::registerPublisher(
    const bsl::shared_ptr<ntci::MonitorablePublisher>& publisher)
{
    d_collector.registerPublisher(publisher);
}

void PeriodicCollector::deregisterPublisher(
    const bsl::shared_ptr<ntci::MonitorablePublisher>& publisher)
{
    d_collector.deregisterPublisher(publisher);
}

void PeriodicCollector::start()
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_TRACE("Starting metrics collector");

    d_collector.start();

    if (d_interval > bsls::TimeInterval(0, 0)) {
        d_scheduler.scheduleRecurringEvent(
            &d_event,
            d_interval,
            bdlf::MemFnUtil::memFn(&ntcm::Collector::collect, &d_collector));

        bslmt::ThreadAttributes threadAttributes;
        if (!d_collector.configuration().threadName().isNull()) {
            threadAttributes.setThreadName(
                d_collector.configuration().threadName().value());
        }

        int rc = d_scheduler.start(threadAttributes);
        if (rc != 0) {
            NTCI_LOG_ERROR(
                "Failed to start metrics collector scheduler, rc = %d",
                rc);
            return;
        }
    }
}

void PeriodicCollector::stop()
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_TRACE("Stopping metrics collector");

    if (d_interval > bsls::TimeInterval(0, 0)) {
        d_scheduler.cancelEventAndWait(&d_event);
        d_scheduler.stop();
    }

    d_collector.stop();
}

void PeriodicCollector::collect()
{
    d_collector.collect();
}

}  // close package namespace
}  // close enterprise namespace
