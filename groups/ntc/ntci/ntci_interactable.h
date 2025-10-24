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

#ifndef INCLUDED_NTCI_INTERACTABLE
#define INCLUDED_NTCI_INTERACTABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_identifiable.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface for an object that may interact with administrative
/// commands at run-time.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @par Usage Example
/// Let's assume the existence of a class 'example::Object', that implements
/// the 'ntci::Interactable' interface, and an 'object' instantiated of that
/// class type.
///
/// @ingroup module_ntci_metrics
class Interactable : public virtual Identifiable
{
  public:
    /// Destroy this object.
    virtual ~Interactable();

    /// Interact with the specified object according to the specified
    /// 'command', and load into the specified 'output' the output of the
    /// 'command'. Return the error.
    virtual ntsa::Error interact(bsl::string*       output,
                                 const bsl::string& command);
};

/// Provide an interface for a registry of interactable objects.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_metrics
class InteractableRegistry
{
  public:
    /// Destroy this object.
    virtual ~InteractableRegistry();

    /// Add the specified 'object' to this interactable object registry.
    virtual void registerInteractable(
        const bsl::shared_ptr<ntci::Interactable>& object) = 0;

    /// Remove the specified 'object' from this interactable object registry.
    virtual void deregisterInteractable(
        const bsl::shared_ptr<ntci::Interactable>& object) = 0;

    /// Append to the specified 'result' each currently registered
    /// interactable object.
    virtual void getInteractableObjects(
        bsl::vector<bsl::shared_ptr<ntci::Interactable> >* result) const = 0;
};

}  // close package namespace
}  // close enterprise namespace

#endif
