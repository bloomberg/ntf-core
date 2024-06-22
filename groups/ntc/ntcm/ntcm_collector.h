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

#ifndef INCLUDED_NTCM_COLLECTOR
#define INCLUDED_NTCM_COLLECTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_monitorablecollectorconfig.h>
#include <ntccfg_platform.h>
#include <ntci_monitorable.h>
#include <ntcscm_version.h>
#include <bdlma_concurrentmultipoolallocator.h>
#include <bslmt_mutex.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_unordered_set.h>

namespace BloombergLP {
namespace ntcm {
class Publisher;
}
namespace ntcm {

/// @internal @brief
/// Provide a collector of statistics from monitorable objects.
///
/// @details
/// Provide a mechanism that provides a collector of statistics
/// from monitorable objects. Each set of collected statistics are published,
/// along with the monitorable object that measured them, through various
/// registered publishers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcm
class Collector : public ntci::MonitorableCollector
{
  public:
    /// This typedef defines a function to load the set of currently active
    /// monitorable objects.
    typedef bsl::function<void(
        bsl::vector<bsl::shared_ptr<ntci::Monitorable> >* result)>
        LoadCallback;

  private:
    /// Define a type alias for a set of registered
    /// publishers.
    typedef bsl::unordered_set<bsl::shared_ptr<ntci::MonitorablePublisher> >
        PublisherSet;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    Mutex                               d_mutex;
    PublisherSet                        d_publishers;
    bdlma::ConcurrentMultipoolAllocator d_memoryPools;
    LoadCallback                        d_loader;
    ntca::MonitorableCollectorConfig    d_config;
    bslma::Allocator*                   d_allocator_p;

  private:
    Collector(const Collector&) BSLS_KEYWORD_DELETED;
    Collector& operator=(const Collector&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new collector having a default configuration that collects
    /// statistics on-demand from all monitorable objects loaded from the
    /// specified 'loadCallback'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Collector(const LoadCallback& loadCallback,
                       bslma::Allocator*   basicAllocator = 0);

    /// Create a new collector having the specified 'configuration' that
    /// collects statistics on-demand from all monitorable objects loaded
    /// from the specified 'loadCallback'.  Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Collector(const ntca::MonitorableCollectorConfig& configuration,
                       const LoadCallback&                     loadCallback,
                       bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Collector() BSLS_KEYWORD_OVERRIDE;

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

    /// Collect statistics from each monitorable object registered with
    /// the default monitorable object registry and publish their statistics
    /// through each registered publisher.
    void collect() BSLS_KEYWORD_OVERRIDE;

    /// Return the configuration of this object.
    const ntca::MonitorableCollectorConfig& configuration() const;
};

}  // close package namespace
}  // close enterprise namespace
#endif
