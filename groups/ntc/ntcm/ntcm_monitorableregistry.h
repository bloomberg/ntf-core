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

#ifndef INCLUDED_NTCM_MONITORABLEREGISTRY
#define INCLUDED_NTCM_MONITORABLEREGISTRY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_monitorableregistryconfig.h>
#include <ntci_monitorable.h>
#include <ntcscm_version.h>
#include <bslmt_mutex.h>
#include <bsls_keyword.h>
#include <bsl_memory.h>
#include <bsl_unordered_map.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcm {

/// @internal @brief
/// Provide a concrete registry of monitorable objects.
///
/// @details
/// Provide a mechanism that implements the
/// 'ntci::MonitorableRegistry' protocol to register monitorable objects by
/// their locally-unique object IDs.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcm
class MonitorableRegistry : public ntci::MonitorableRegistry
{
    /// Define a type alias for a map of locally-unique object
    /// identifiers to the shared pointers to registered monitorable objects
    /// so identified.
    typedef bsl::unordered_map<int, bsl::shared_ptr<ntci::Monitorable> >
        ObjectMap;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex                   d_mutex;
    ObjectMap                       d_objects;
    ntca::MonitorableRegistryConfig d_config;
    bslma::Allocator*               d_allocator_p;

  private:
    MonitorableRegistry(const MonitorableRegistry&) BSLS_KEYWORD_DELETED;
    MonitorableRegistry& operator=(const MonitorableRegistry&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new monitorable registry having a default configuration.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit MonitorableRegistry(bslma::Allocator* basicAllocator = 0);

    /// Create a new monitorable registry having the specified
    /// 'configuration'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit MonitorableRegistry(
        const ntca::MonitorableRegistryConfig& configuration,
        bslma::Allocator*                      basicAllocator = 0);

    /// Destroy this object.
    ~MonitorableRegistry() BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'object' to this monitorable object registry.
    void registerMonitorable(const bsl::shared_ptr<ntci::Monitorable>& object)
        BSLS_KEYWORD_OVERRIDE;

    /// Remove the specified 'object' from this monitorable object
    /// registry.
    void deregisterMonitorable(const bsl::shared_ptr<ntci::Monitorable>&
                                   object) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' each currently registered
    /// monitorable object.
    void loadRegisteredObjects(
        bsl::vector<bsl::shared_ptr<ntci::Monitorable> >* result) const
        BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace
#endif
