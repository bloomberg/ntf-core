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

#ifndef INCLUDED_NTCI_MONITORABLEUTIL
#define INCLUDED_NTCI_MONITORABLEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_monitorablecollectorconfig.h>
#include <ntca_monitorableregistryconfig.h>
#include <ntccfg_platform.h>
#include <ntci_identifiable.h>
#include <ntci_monitorable.h>
#include <ntcscm_version.h>
#include <bdld_manageddatum.h>
#include <bsls_spinlock.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcm {

/// @internal @brief
/// Provide utilities to manage a default monitorable registry.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcm
struct MonitorableUtil {
    /// This typedef defines a function to load the set of currently active
    /// monitorable objects.
    typedef bsl::function<void(
        bsl::vector<bsl::shared_ptr<ntci::Monitorable> >* result)>
        LoadCallback;

    /// Initialize this component.
    static void initialize();

    /// Set the default monitorable object registry to an object with the
    /// specified 'configuration' and enable the registration of monitorable
    /// objects with that default registry. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator is 0, the
    /// global allocator is used.
    static void enableMonitorableRegistry(
        const ntca::MonitorableRegistryConfig& configuration,
        bslma::Allocator*                      basicAllocator = 0);

    /// Set a default monitorable object registry to the specified
    /// 'monitorableRegistry' and enable the registration of monitorable
    /// object with that default registry. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator is 0, the
    /// global allocator is used.
    static void enableMonitorableRegistry(
        const bsl::shared_ptr<ntci::MonitorableRegistry>& monitorableRegistry);

    /// Disable the registration of monitorable objects with the default
    /// registry and unset the default monitorable object registry, if any.
    static void disableMonitorableRegistry();

    /// Set the default monitorable object collector to an object with the
    /// specified 'configuration' and enable the periodic collection of
    /// monitorable objects by that default collector. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator is 0, the
    /// global allocator is used.
    static void enableMonitorableCollector(
        const ntca::MonitorableCollectorConfig& configuration,
        bslma::Allocator*                       basicAllocator = 0);

    /// Set the default monitorable object collector to the specified
    /// 'monitorableCollector' and enable the periodic collection of
    /// monitorable objects by that default collector. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator is 0, the
    /// global allocator is used.
    static void enableMonitorableCollector(
        const bsl::shared_ptr<ntci::MonitorableCollector>&
            monitorableCollector);

    /// Disable the periodic collection of monitorable objects by the
    /// default monitorable object collector and unset the default
    /// monitorable object collector, if any.
    static void disableMonitorableCollector();

    /// Add the specified 'monitorable' to the default monitorable object
    /// registry, if a default monitorable object registry has been enabled.
    static void registerMonitorable(
        const bsl::shared_ptr<ntci::Monitorable>& monitorable);

    /// Remove the specified 'monitorable' from the default monitorable
    /// object registry, if a default monitorable object registry has been
    /// enabled.
    static void deregisterMonitorable(
        const bsl::shared_ptr<ntci::Monitorable>& monitorable);

    /// Add the specified 'monitorable' of the entire process to the default
    /// monitorable object registry, if a default monitorable object
    /// registry has been enabled, and no other 'monitorable' for the entire
    /// process has already been registered.
    static void registerMonitorableProcess(
        const bsl::shared_ptr<ntci::Monitorable>& monitorable);

    /// Remove any monitorable of the entire process from the default
    /// monitorable object registry, if a default monitorable object
    /// registry has been enabled and any monitorable of the entire process
    /// had been previously registered.
    static void deregisterMonitorableProcess();

    /// Add the specified 'monitorablePublisher' to the default monitorable
    /// object collector, if a default monitorable object collector has been
    /// set.
    static void registerMonitorablePublisher(
        const bsl::shared_ptr<ntci::MonitorablePublisher>&
            monitorablePublisher);

    /// Remove the specified 'monitorablePublisher' from the default
    /// monitorable object collector, if a default monitorable object
    /// collector has been set.
    static void deregisterMonitorablePublisher(
        const bsl::shared_ptr<ntci::MonitorablePublisher>&
            monitorablePublisher);

    /// Add a monitorable publisher to the BSLS log at the specified
    /// 'severityLevel'.
    static void registerMonitorablePublisher(
        bsls::LogSeverity::Enum severityLevel);

    /// Remove a monitorable publisher to the BSLS log at the specified
    /// 'severityLevel'.
    static void deregisterMonitorablePublisher(
        bsls::LogSeverity::Enum severityLevel);

    /// Force the collection of statistics from each monitorable object
    /// registered with the default monitorable object registry and publish
    /// their statistics through each registered publisher.
    static void collectMetrics();

    /// Append to the specified 'result' each currently registered
    /// monitorable object with the default monitorable object registry, if
    /// a default monitorable object registry has been set.
    static void loadRegisteredObjects(
        bsl::vector<bsl::shared_ptr<ntci::Monitorable> >* result);

    /// Return the function to load the set of currently active monitorable
    /// objects.
    static LoadCallback loadCallback();

    /// Cleanup the resources used by this component.
    static void exit();
};

}  // close package namespace
}  // close enterprise namespace

#endif
