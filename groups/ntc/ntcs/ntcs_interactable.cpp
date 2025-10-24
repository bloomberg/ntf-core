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

#include <ntcs_interactable.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_interactable_cpp, "$Id$ $CSID$")

#include <ntci_log.h>
#include <bdlf_memfn.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsls_timeinterval.h>
#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_iomanip.h>
#include <bsl_sstream.h>
#include <bsl_unordered_map.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcs {

InteractableRegistry::InteractableRegistry(bslma::Allocator* basicAllocator)
: d_mutex()
, d_objects(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

InteractableRegistry::~InteractableRegistry()
{
}

void InteractableRegistry::registerInteractable(
    const bsl::shared_ptr<ntci::Interactable>& object)
{
    LockGuard guard(&d_mutex);

    d_objects[static_cast<int>(object->objectId())] = object;
}

void InteractableRegistry::deregisterInteractable(
    const bsl::shared_ptr<ntci::Interactable>& object)
{
    LockGuard guard(&d_mutex);

    d_objects.erase(static_cast<int>(object->objectId()));
}

void InteractableRegistry::getInteractableObjects(
    bsl::vector<bsl::shared_ptr<ntci::Interactable> >* result) const
{
    LockGuard guard(&d_mutex);

    result->reserve(result->size() + d_objects.size());

    for (ObjectMap::const_iterator it = d_objects.begin();
         it != d_objects.end();
         ++it)
    {
        result->push_back(it->second);
    }
}

/// Provide global interactable state.
class InteractableUtil::State
{
  public:
    /// Create new global state.
    State();

    /// Destroy this object.
    ~State();

    /// The installed registry lock.
    bsls::SpinLock d_registryLock;

    /// The installed registry.
    bsl::shared_ptr<ntci::InteractableRegistry> d_registry_sp;

    /// The installed system interactable object lock.
    bsls::SpinLock d_systemLock;

    /// The installed system interactable object.
    bsl::shared_ptr<ntci::Interactable> d_system_sp;

    /// The instance of the global interactable state.
    static State s_global;

  private:
    State(const State&) BSLS_KEYWORD_DELETED;
    State& operator=(const State&) BSLS_KEYWORD_DELETED;
};

InteractableUtil::State InteractableUtil::State::s_global;

InteractableUtil::State::State()
: d_registryLock(bsls::SpinLock::s_unlocked)
, d_registry_sp()
, d_systemLock(bsls::SpinLock::s_unlocked)
, d_system_sp()
{
}

InteractableUtil::State::~State()
{
}

void InteractableUtil::initialize()
{
}

void InteractableUtil::enableInteractableRegistry(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator =
        basicAllocator ? basicAllocator : bslma::Default::globalAllocator();

    bsl::shared_ptr<ntcs::InteractableRegistry> interactableRegistry;
    interactableRegistry.createInplace(allocator, allocator);

    bsls::SpinLockGuard guard(
        &InteractableUtil::State::s_global.d_registryLock);
    InteractableUtil::State::s_global.d_registry_sp = interactableRegistry;
}

void InteractableUtil::enableInteractableRegistry(
    const bsl::shared_ptr<ntci::InteractableRegistry>& interactableRegistry)
{
    bsls::SpinLockGuard guard(
        &InteractableUtil::State::s_global.d_registryLock);
    InteractableUtil::State::s_global.d_registry_sp = interactableRegistry;
}

void InteractableUtil::disableInteractableRegistry()
{
    {
        bsls::SpinLockGuard guard(
            &InteractableUtil::State::s_global.d_systemLock);
        InteractableUtil::State::s_global.d_system_sp.reset();
    }

    {
        bsls::SpinLockGuard guard(
            &InteractableUtil::State::s_global.d_registryLock);
        InteractableUtil::State::s_global.d_registry_sp.reset();
    }
}

void InteractableUtil::registerInteractable(
    const bsl::shared_ptr<ntci::Interactable>& interactable)
{
    bsl::shared_ptr<ntci::InteractableRegistry> interactableRegistry;
    {
        bsls::SpinLockGuard guard(
            &InteractableUtil::State::s_global.d_registryLock);
        interactableRegistry = InteractableUtil::State::s_global.d_registry_sp;
    }

    if (interactableRegistry) {
        interactableRegistry->registerInteractable(interactable);
    }
}

void InteractableUtil::deregisterInteractable(
    const bsl::shared_ptr<ntci::Interactable>& interactable)
{
    bsl::shared_ptr<ntci::InteractableRegistry> interactableRegistry;
    {
        bsls::SpinLockGuard guard(
            &InteractableUtil::State::s_global.d_registryLock);
        interactableRegistry = InteractableUtil::State::s_global.d_registry_sp;
    }

    if (interactableRegistry) {
        interactableRegistry->deregisterInteractable(interactable);
    }
}

void InteractableUtil::registerInteractableProcess(
    const bsl::shared_ptr<ntci::Interactable>& interactable)
{
    {
        bsls::SpinLockGuard guard(
            &InteractableUtil::State::s_global.d_systemLock);
        if (InteractableUtil::State::s_global.d_system_sp) {
            return;
        }

        InteractableUtil::State::s_global.d_system_sp = interactable;
    }

    InteractableUtil::registerInteractable(interactable);
}

void InteractableUtil::deregisterInteractableProcess()
{
    bsl::shared_ptr<ntci::Interactable> interactable;

    {
        bsls::SpinLockGuard guard(
            &InteractableUtil::State::s_global.d_systemLock);
        if (!InteractableUtil::State::s_global.d_system_sp) {
            return;
        }

        InteractableUtil::State::s_global.d_system_sp.swap(interactable);
    }

    InteractableUtil::deregisterInteractable(interactable);
}

void InteractableUtil::getInteractableObjects(
    bsl::vector<bsl::shared_ptr<ntci::Interactable> >* result)
{
    bsl::shared_ptr<ntci::InteractableRegistry> interactableRegistry;
    {
        bsls::SpinLockGuard guard(
            &InteractableUtil::State::s_global.d_registryLock);
        interactableRegistry = InteractableUtil::State::s_global.d_registry_sp;
    }

    if (interactableRegistry) {
        interactableRegistry->getInteractableObjects(result);
    }
}

InteractableUtil::LoadCallback InteractableUtil::loadCallback()
{
    return &InteractableUtil::getInteractableObjects;
}

void InteractableUtil::exit()
{
    {
        bsls::SpinLockGuard guard(
            &InteractableUtil::State::s_global.d_systemLock);
        InteractableUtil::State::s_global.d_system_sp.reset();
    }

    {
        bsls::SpinLockGuard guard(
            &InteractableUtil::State::s_global.d_registryLock);
        InteractableUtil::State::s_global.d_registry_sp.reset();
    }
}

}  // close namespace ntcs
}  // close namespace BloombergLP
