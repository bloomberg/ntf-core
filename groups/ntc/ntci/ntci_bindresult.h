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

#ifndef INCLUDED_NTCI_BINDRESULT
#define INCLUDED_NTCI_BINDRESULT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_bindevent.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {
class Bindable;
}
namespace ntci {

/// Describe the result of a bind operation.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_bind
class BindResult
{
    bsl::shared_ptr<ntci::Bindable> d_bindable_sp;
    ntca::BindEvent                 d_event;

  public:
    /// Create a new bind result. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default allocator is used.
    explicit BindResult(bslma::Allocator* basicAllocator = 0);

    /// Create a a new bind result having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is null, the currently installed
    /// default allocator is used.
    BindResult(const BindResult& original,
               bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~BindResult();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    BindResult& operator=(const BindResult& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the bindable to the specified 'value'.
    void setBindable(const bsl::shared_ptr<ntci::Bindable>& value);

    /// Set the event to the specified 'value'.
    void setEvent(const ntca::BindEvent& value);

    /// Return the bindable.
    const bsl::shared_ptr<ntci::Bindable>& bindable() const;

    /// Return the event.
    const ntca::BindEvent& event() const;
};

NTCCFG_INLINE
BindResult::BindResult(bslma::Allocator* basicAllocator)
: d_bindable_sp()
, d_event()
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

NTCCFG_INLINE
BindResult::BindResult(const BindResult& original,
                       bslma::Allocator* basicAllocator)
: d_bindable_sp(original.d_bindable_sp)
, d_event(original.d_event)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

NTCCFG_INLINE
BindResult::~BindResult()
{
}

NTCCFG_INLINE
BindResult& BindResult::operator=(const BindResult& other)
{
    if (this != &other) {
        d_bindable_sp = other.d_bindable_sp;
        d_event       = other.d_event;
    }
    return *this;
}

NTCCFG_INLINE
void BindResult::reset()
{
    d_bindable_sp.reset();
    d_event.reset();
}

NTCCFG_INLINE
void BindResult::setBindable(const bsl::shared_ptr<ntci::Bindable>& value)
{
    d_bindable_sp = value;
}

NTCCFG_INLINE
void BindResult::setEvent(const ntca::BindEvent& value)
{
    d_event = value;
}

NTCCFG_INLINE
const bsl::shared_ptr<ntci::Bindable>& BindResult::bindable() const
{
    return d_bindable_sp;
}

NTCCFG_INLINE
const ntca::BindEvent& BindResult::event() const
{
    return d_event;
}

}  // close package namespace
}  // close enterprise namespace
#endif
