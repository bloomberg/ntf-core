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

#ifndef INCLUDED_NTCM_PERIODICCOLLECTOR
#define INCLUDED_NTCM_PERIODICCOLLECTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_monitorablecollectorconfig.h>
#include <ntci_monitorable.h>
#include <ntcm_collector.h>
#include <ntcscm_version.h>
#include <bdlmt_eventscheduler.h>
#include <bsls_keyword.h>
#include <bsls_timeinterval.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcm {

/// @internal @brief
/// Provide a periodic collector of statistics in a separate thread.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcm
class PeriodicCollector : public ntci::MonitorableCollector
{
    bdlmt::EventScheduler                       d_scheduler;
    bdlmt::EventScheduler::RecurringEventHandle d_event;
    bsls::TimeInterval                          d_interval;
    ntcm::Collector                             d_collector;

  private:
    PeriodicCollector(const PeriodicCollector&) BSLS_KEYWORD_DELETED;
    PeriodicCollector& operator=(const PeriodicCollector&)
        BSLS_KEYWORD_DELETED;

  public:
    /// This typedef defines a function to load the set of currently active
    /// monitorable objects.
    typedef ntcm::Collector::LoadCallback LoadCallback;

    /// Create a new collector that periodically collects statistcs from all
    /// monitorable objects loaded from the specified 'loadCallback' at the
    /// specified 'interval'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit PeriodicCollector(const bsls::TimeInterval& interval,
                               const LoadCallback&       loadCallback,
                               bslma::Allocator*         basicAllocator = 0);

    /// Create a new collector having the specified 'configuration' that
    /// periodically collects statistics from all monitorable objects loaded
    /// from the specified 'loadCallback'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit PeriodicCollector(
        const ntca::MonitorableCollectorConfig& configuration,
        const LoadCallback&                     loadCallback,
        bslma::Allocator*                       basicAllocator = 0);

    /// Destroy this object.
    ~PeriodicCollector() BSLS_KEYWORD_OVERRIDE;

    /// Register the specified 'publisher' to publish statistics measured
    /// by each monitorable object.
    void registerPublisher(const bsl::shared_ptr<ntci::MonitorablePublisher>&
                               publisher) BSLS_KEYWORD_OVERRIDE;

    /// Deregister the specified 'publisher' to no longer publish statistics
    /// measured by each monitorable object.
    void deregisterPublisher(const bsl::shared_ptr<ntci::MonitorablePublisher>&
                                 publisher) BSLS_KEYWORD_OVERRIDE;

    /// Start the background thread periodically collecting statistics.
    void start() BSLS_KEYWORD_OVERRIDE;

    /// Stop the background thread periodically collecting statistics.
    void stop() BSLS_KEYWORD_OVERRIDE;

    /// Force the collection of statistics from each monitorable object
    /// registered with the default monitorable object registry and publish
    /// their statistics through each registered publisher.
    void collect() BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace
#endif
