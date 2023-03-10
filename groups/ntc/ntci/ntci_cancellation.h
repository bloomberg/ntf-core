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

#ifndef INCLUDED_NTCI_CANCELLATION
#define INCLUDED_NTCI_CANCELLATION

#include <ntccfg_platform.h>
#include <ntci_strand.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to cancel the invocation of an asynchronous function.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_utility
class Cancellation
{
  public:
    /// Destroy this object.
    virtual ~Cancellation();

    /// Prevent subsequent execution of this object's functions.  Return
    /// true if this function is successfully aborted and false to indicate
    /// its (sole) function is already executing.
    virtual bool abort() = 0;

    /// Return a pointer that indicates an operation is uncancellable by
    /// the initiator.
    static bsl::shared_ptr<ntci::Cancellation>* none();
};

// The uncancelable mechanism.
extern bsl::shared_ptr<ntci::Cancellation>* const k_UNCANCELLABLE;

NTCCFG_INLINE
bsl::shared_ptr<ntci::Cancellation>* Cancellation::none()
{
    return 0;
}

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
