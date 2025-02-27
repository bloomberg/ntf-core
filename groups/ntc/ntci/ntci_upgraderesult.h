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

#ifndef INCLUDED_NTCI_UPGRADERESULT
#define INCLUDED_NTCI_UPGRADERESULT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_upgradeevent.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {
class Upgradable;
}
namespace ntci {

/// Describe the result of a upgrade operation.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_upgrade
class UpgradeResult
{
    bsl::shared_ptr<ntci::Upgradable> d_upgradable_sp;
    ntca::UpgradeEvent                d_event;

  public:
    /// Create a new upgrade result. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default allocator is used.
    explicit UpgradeResult(bslma::Allocator* basicAllocator = 0);

    /// Create a a new upgrade result having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is null, the currently installed
    /// default allocator is used.
    UpgradeResult(const UpgradeResult& original,
                  bslma::Allocator*    basicAllocator = 0);

    /// Destroy this object.
    ~UpgradeResult();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    UpgradeResult& operator=(const UpgradeResult& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the upgradable socket to the specified 'value'.
    void setUpgradable(const bsl::shared_ptr<ntci::Upgradable>& value);

    /// Set the event to the specified 'value'.
    void setEvent(const ntca::UpgradeEvent& value);

    /// Return the upgradable socket.
    const bsl::shared_ptr<ntci::Upgradable>& upgradable() const;

    /// Return the event.
    const ntca::UpgradeEvent& event() const;
};

NTCCFG_INLINE
UpgradeResult::UpgradeResult(bslma::Allocator* basicAllocator)
: d_upgradable_sp()
, d_event()
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

NTCCFG_INLINE
UpgradeResult::UpgradeResult(const UpgradeResult& original,
                             bslma::Allocator*    basicAllocator)
: d_upgradable_sp(original.d_upgradable_sp)
, d_event(original.d_event)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

NTCCFG_INLINE
UpgradeResult::~UpgradeResult()
{
}

NTCCFG_INLINE
UpgradeResult& UpgradeResult::operator=(const UpgradeResult& other)
{
    if (this != &other) {
        d_upgradable_sp = other.d_upgradable_sp;
        d_event         = other.d_event;
    }
    return *this;
}

NTCCFG_INLINE
void UpgradeResult::reset()
{
    d_upgradable_sp.reset();
    d_event.reset();
}

NTCCFG_INLINE
void UpgradeResult::setUpgradable(const bsl::shared_ptr<ntci::Upgradable>& value)
{
    d_upgradable_sp = value;
}

NTCCFG_INLINE
void UpgradeResult::setEvent(const ntca::UpgradeEvent& value)
{
    d_event = value;
}

NTCCFG_INLINE
const bsl::shared_ptr<ntci::Upgradable>& UpgradeResult::upgradable() const
{
    return d_upgradable_sp;
}

NTCCFG_INLINE
const ntca::UpgradeEvent& UpgradeResult::event() const
{
    return d_event;
}

}  // close package namespace
}  // close enterprise namespace
#endif
