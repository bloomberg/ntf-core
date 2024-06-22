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

#ifndef INCLUDED_NTCQ_CONNECT
#define INCLUDED_NTCQ_CONNECT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_connectoptions.h>
#include <ntccfg_platform.h>
#include <ntci_connectcallback.h>
#include <ntci_connector.h>
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
/// Describe an entry in a connect callback queue.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcq
class ConnectCallbackEntry
{
    ntccfg::Object        d_object;
    ntcs::CallbackState   d_state;
    ntci::ConnectCallback d_callback;
    ntca::ConnectOptions  d_options;

  private:
    ConnectCallbackEntry(const ConnectCallbackEntry&) BSLS_KEYWORD_DELETED;
    ConnectCallbackEntry& operator=(const ConnectCallbackEntry&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new connect callback queue entry. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit ConnectCallbackEntry(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ConnectCallbackEntry();

    /// Clear the state of this entry.
    void clear();

    /// Assign the specified 'callback' to be invoked on the specified
    /// 'strand'.
    void assign(const ntci::ConnectCallback& callback,
                const ntca::ConnectOptions&  options);

    /// Return the criteria to invoke the callback.
    const ntca::ConnectOptions& options() const;

    /// Invoke the callback of the specified 'entry' for the specified
    /// connector' and 'event'. If the specified 'defer' flag is false
    /// and the requirements of the strand of the specified 'entry' permits
    /// the callback to be invoked immediately by the 'strand', unlock the
    /// specified 'mutex', invoke the callback, then relock the 'mutex'.
    /// Otherwise, enqueue the invocation of the callback to be executed on
    /// the strand of the 'entry', if defined, or by the specified
    /// 'executor' otherwise.
    static void dispatch(
        const bsl::shared_ptr<ntcq::ConnectCallbackEntry>& entry,
        const bsl::shared_ptr<ntci::Connector>&            connector,
        const ntca::ConnectEvent&                          event,
        const bsl::shared_ptr<ntci::Strand>&               strand,
        const bsl::shared_ptr<ntci::Executor>&             executor,
        bool                                               defer,
        ntccfg::Mutex*                                     mutex);

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(ConnectCallbackEntry);
};

NTCCFG_INLINE
const ntca::ConnectOptions& ConnectCallbackEntry::options() const
{
    return d_options;
}

}  // close package namespace
}  // close enterprise namespace
#endif
