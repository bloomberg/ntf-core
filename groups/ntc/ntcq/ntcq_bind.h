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

#ifndef INCLUDED_NTCQ_BIND
#define INCLUDED_NTCQ_BIND

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_bindoptions.h>
#include <ntccfg_platform.h>
#include <ntci_bindable.h>
#include <ntci_bindcallback.h>
#include <ntci_strand.h>
#include <ntcs_callbackstate.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bdlb_nullablevalue.h>
#include <bsls_timeinterval.h>
#include <bsls_timeutil.h>
#include <bsl_functional.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcq {

/// @internal @brief
/// Describe an entry in a bind callback queue.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcq
class BindCallbackEntry
{
    ntccfg::Object      d_object;
    ntcs::CallbackState d_state;
    ntci::BindCallback  d_callback;
    ntca::BindOptions   d_options;

  private:
    BindCallbackEntry(const BindCallbackEntry&) BSLS_KEYWORD_DELETED;
    BindCallbackEntry& operator=(const BindCallbackEntry&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new bind callback queue entry. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit BindCallbackEntry(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~BindCallbackEntry();

    /// Clear the state of this entry.
    void clear();

    /// Assign the specified 'callback' to be invoked on the specified
    /// 'strand'.
    void assign(const ntci::BindCallback& callback,
                const ntca::BindOptions&  options);

    /// Return the criteria to invoke the callback.
    const ntca::BindOptions& options() const;

    /// Invoke the callback of the specified 'entry' for the specified
    /// 'bindable' and 'event'. If the specified 'defer' flag is false
    /// and the requirements of the strand of the specified 'entry' permits
    /// the callback to be invoked immediately by the 'strand', unlock the
    /// specified 'mutex', invoke the callback, then relock the 'mutex'.
    /// Otherwise, enqueue the invocation of the callback to be executed on
    /// the strand of the 'entry', if defined, or by the specified
    /// 'executor' otherwise.
    static void dispatch(const bsl::shared_ptr<ntcq::BindCallbackEntry>& entry,
                         const bsl::shared_ptr<ntci::Bindable>& bindable,
                         const ntca::BindEvent&                 event,
                         const bsl::shared_ptr<ntci::Strand>&   strand,
                         const bsl::shared_ptr<ntci::Executor>& executor,
                         bool                                   defer,
                         ntccfg::Mutex*                         mutex);

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(BindCallbackEntry);
};

NTCCFG_INLINE
const ntca::BindOptions& BindCallbackEntry::options() const
{
    return d_options;
}

}  // close package namespace
}  // close enterprise namespace
#endif
