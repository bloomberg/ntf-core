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

#ifndef INCLUDED_NTCI_CLOSERESULT
#define INCLUDED_NTCI_CLOSERESULT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {
class Closable;
}
namespace ntci {

/// Describe the result of a close operation.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_close
class CloseResult
{
    bsl::shared_ptr<ntci::Closable> d_closable_sp;

  public:
    /// Create a new close result. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default allocator is used.
    explicit CloseResult(bslma::Allocator* basicAllocator = 0);

    /// Create a a new close result having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is null, the currently installed
    /// default allocator is used.
    CloseResult(const CloseResult& original,
                bslma::Allocator*  basicAllocator = 0);

    /// Destroy this object.
    ~CloseResult();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    CloseResult& operator=(const CloseResult& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the closable to the specified 'value'.
    void setClosable(const bsl::shared_ptr<ntci::Closable>& value);

    /// Return the closable.
    const bsl::shared_ptr<ntci::Closable>& closable() const;
};

NTCCFG_INLINE
CloseResult::CloseResult(bslma::Allocator* basicAllocator)
: d_closable_sp()
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

NTCCFG_INLINE
CloseResult::CloseResult(const CloseResult& original,
                         bslma::Allocator*  basicAllocator)
: d_closable_sp(original.d_closable_sp)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

NTCCFG_INLINE
CloseResult::~CloseResult()
{
}

NTCCFG_INLINE
CloseResult& CloseResult::operator=(const CloseResult& other)
{
    if (this != &other) {
        d_closable_sp = other.d_closable_sp;
    }
    return *this;
}

NTCCFG_INLINE
void CloseResult::reset()
{
    d_closable_sp.reset();
}

NTCCFG_INLINE
void CloseResult::setClosable(const bsl::shared_ptr<ntci::Closable>& value)
{
    d_closable_sp = value;
}

NTCCFG_INLINE
const bsl::shared_ptr<ntci::Closable>& CloseResult::closable() const
{
    return d_closable_sp;
}

}  // close package namespace
}  // close enterprise namespace
#endif
