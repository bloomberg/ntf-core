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

#include <ntcq_bind.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcq_bind_cpp, "$Id$ $CSID$")

#include <ntccfg_limits.h>

#include <ntccfg_bind.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>

namespace BloombergLP {
namespace ntcq {

BindCallbackEntry::BindCallbackEntry(bslma::Allocator* basicAllocator)
: d_object("ntcq::BindCallbackEntry")
, d_state()
, d_callback(basicAllocator)
, d_options()
{
}

BindCallbackEntry::~BindCallbackEntry()
{
    BSLS_ASSERT(!d_callback);
}

void BindCallbackEntry::clear()
{
    d_state.reset();
    d_callback.reset();
    d_options.reset();
}

void BindCallbackEntry::assign(const ntci::BindCallback& callback,
                               const ntca::BindOptions&  options)
{
    d_callback = callback;
    d_options  = options;
}

void BindCallbackEntry::dispatch(
    const bsl::shared_ptr<ntcq::BindCallbackEntry>& entry,
    const bsl::shared_ptr<ntci::Bindable>&          bindable,
    const ntca::BindEvent&                          event,
    const bsl::shared_ptr<ntci::Strand>&            strand,
    const bsl::shared_ptr<ntci::Executor>&          executor,
    bool                                            defer,
    ntccfg::Mutex*                                  mutex)
{
    if (NTCCFG_LIKELY(entry->d_state.finish())) {
        ntci::BindCallback callback = entry->d_callback;
        entry->d_callback.reset();

        if (callback) {
            callback.dispatch(bindable, event, strand, executor, defer, mutex);
        }
    }
}

}  // close package namespace
}  // close enterprise namespace
