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

#ifndef INCLUDED_NTCS_INTERACTABLE
#define INCLUDED_NTCS_INTERACTABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_mutex.h>
#include <ntccfg_platform.h>
#include <ntci_identifiable.h>
#include <ntci_interactable.h>
#include <ntcscm_version.h>
#include <bsls_atomic.h>
#include <bsls_keyword.h>
#include <bsls_spinlock.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide a concrete registry of interactable objects.
///
/// @details
/// Provide a mechanism that implements the
/// 'ntci::InteractableRegistry' protocol to register interactable objects by
/// their locally-unique object IDs.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class InteractableRegistry : public ntci::InteractableRegistry
{
    /// Define a type alias for a map of locally-unique object
    /// identifiers to the shared pointers to registered interactable objects
    /// so identified.
    typedef bsl::unordered_map<int, bsl::shared_ptr<ntci::Interactable> >
        ObjectMap;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex     d_mutex;
    ObjectMap         d_objects;
    bslma::Allocator* d_allocator_p;

  private:
    InteractableRegistry(const InteractableRegistry&) BSLS_KEYWORD_DELETED;
    InteractableRegistry& operator=(const InteractableRegistry&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new interactable registry having a default configuration.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit InteractableRegistry(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~InteractableRegistry() BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'object' to this interactable object registry.
    void registerInteractable(const bsl::shared_ptr<ntci::Interactable>&
                                  object) BSLS_KEYWORD_OVERRIDE;

    /// Remove the specified 'object' from this interactable object
    /// registry.
    void deregisterInteractable(const bsl::shared_ptr<ntci::Interactable>&
                                    object) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' each currently registered
    /// interactable object.
    void getInteractableObjects(
        bsl::vector<bsl::shared_ptr<ntci::Interactable> >* result) const
        BSLS_KEYWORD_OVERRIDE;
};

/// @internal @brief
/// Provide utilities to manage a default interactable registry.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcs
struct InteractableUtil {
    /// This typedef defines a function to load the set of currently active
    /// interactable objects.
    typedef bsl::function<void(
        bsl::vector<bsl::shared_ptr<ntci::Interactable> >* result)>
        LoadCallback;

    /// Initialize this component.
    static void initialize();

    /// Set the default interactable object registry to a default
    /// implementation and enable the registration of interactable objects with
    /// that default registry. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator is 0, the global allocator is used.
    static void enableInteractableRegistry(
        bslma::Allocator* basicAllocator = 0);

    /// Set a default interactable object registry to the specified
    /// 'interactableRegistry' and enable the registration of interactable
    /// object with that default registry. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator is 0, the
    /// global allocator is used.
    static void enableInteractableRegistry(
        const bsl::shared_ptr<ntci::InteractableRegistry>&
            interactableRegistry);

    /// Disable the registration of interactable objects with the default
    /// registry and unset the default interactable object registry, if any.
    static void disableInteractableRegistry();

    /// Add the specified 'interactable' object to the default interactable
    /// object registry, if a default interactable object registry has been
    /// enabled.
    static void registerInteractable(
        const bsl::shared_ptr<ntci::Interactable>& interactable);

    /// Remove the specified 'interactable' object from the default
    /// interactable object registry, if a default interactable object registry
    /// has been enabled.
    static void deregisterInteractable(
        const bsl::shared_ptr<ntci::Interactable>& interactable);

    /// Add the specified 'interactable' of the entire process to the default
    /// interactable object registry, if a default interactable object
    /// registry has been enabled, and no other 'interactable' for the entire
    /// process has already been registered.
    static void registerInteractableProcess(
        const bsl::shared_ptr<ntci::Interactable>& interactable);

    /// Remove any interactable of the entire process from the default
    /// interactable object registry, if a default interactable object
    /// registry has been enabled and any interactable of the entire process
    /// had been previously registered.
    static void deregisterInteractableProcess();

    /// Append to the specified 'result' each currently registered
    /// interactable object with the default interactable object registry, if
    /// a default interactable object registry has been set.
    static void getInteractableObjects(
        bsl::vector<bsl::shared_ptr<ntci::Interactable> >* result);

    /// Return the function to load the set of currently active interactable
    /// objects.
    static LoadCallback loadCallback();

    /// Cleanup the resources used by this component.
    static void exit();

  private:
    /// Provide global interactable state.
    class State;
};

}  // close namespace ntcs
}  // close namespace BloombergLP
#endif
