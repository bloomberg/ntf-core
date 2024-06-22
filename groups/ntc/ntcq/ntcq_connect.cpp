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

#include <ntcq_connect.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcq_connect_cpp, "$Id$ $CSID$")

#include <ntccfg_limits.h>

#include <ntccfg_bind.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>

namespace BloombergLP {
namespace ntcq {

ConnectCallbackEntry::ConnectCallbackEntry(bslma::Allocator* basicAllocator)
: d_object("ntcq::ConnectCallbackEntry")
, d_state()
, d_callback(basicAllocator)
, d_options()
{
}

ConnectCallbackEntry::~ConnectCallbackEntry()
{
    BSLS_ASSERT(!d_callback);
}

void ConnectCallbackEntry::clear()
{
    d_state.reset();
    d_callback.reset();
    d_options.reset();
}

void ConnectCallbackEntry::assign(const ntci::ConnectCallback& callback,
                                  const ntca::ConnectOptions&  options)
{
    d_callback = callback;
    d_options  = options;
}

void ConnectCallbackEntry::dispatch(
    const bsl::shared_ptr<ntcq::ConnectCallbackEntry>& entry,
    const bsl::shared_ptr<ntci::Connector>&            connector,
    const ntca::ConnectEvent&                          event,
    const bsl::shared_ptr<ntci::Strand>&               strand,
    const bsl::shared_ptr<ntci::Executor>&             executor,
    bool                                               defer,
    ntccfg::Mutex*                                     mutex)
{
    if (NTCCFG_LIKELY(entry->d_state.finish())) {
        ntci::ConnectCallback callback = entry->d_callback;
        entry->d_callback.reset();

        if (callback) {
            callback
                .dispatch(connector, event, strand, executor, defer, mutex);
        }
    }
}

}  // close package namespace
}  // close enterprise namespace
