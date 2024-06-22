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

#include <ntcq_accept.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcq_accept_cpp, "$Id$ $CSID$")

#include <ntccfg_limits.h>

#include <ntccfg_bind.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace ntcq {

AcceptCallbackQueueEntry::AcceptCallbackQueueEntry(
    bslma::Allocator* basicAllocator)
: d_object("ntcq::AcceptCallbackQueueEntry")
, d_state()
, d_callback(basicAllocator)
, d_options()
, d_timer_sp()
{
}

AcceptCallbackQueueEntry::~AcceptCallbackQueueEntry()
{
    BSLS_ASSERT(!d_callback);
    BSLS_ASSERT(!d_timer_sp);
}

void AcceptCallbackQueueEntry::clear()
{
    d_state.reset();
    d_callback.reset();
    d_options.reset();
    if (d_timer_sp) {
        d_timer_sp->close();
        d_timer_sp.reset();
    }
}

void AcceptCallbackQueueEntry::dispatch(
    const bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>& entry,
    const bsl::shared_ptr<ntci::Acceptor>&                 acceptor,
    const bsl::shared_ptr<ntci::StreamSocket>&             streamSocket,
    const ntca::AcceptEvent&                               event,
    const bsl::shared_ptr<ntci::Strand>&                   strand,
    const bsl::shared_ptr<ntci::Executor>&                 executor,
    bool                                                   defer,
    ntccfg::Mutex*                                         mutex)
{
    if (NTCCFG_LIKELY(entry->d_state.finish())) {
        if (entry->d_timer_sp) {
            entry->d_timer_sp->close();
            entry->d_timer_sp.reset();
        }

        ntci::AcceptCallback callback = entry->d_callback;
        entry->d_callback.reset();

        if (callback) {
            callback.dispatch(acceptor,
                              streamSocket,
                              event,
                              strand,
                              executor,
                              defer,
                              mutex);
        }
    }
}

void AcceptCallbackQueueEntryPool::construct(void*             address,
                                             bslma::Allocator* allocator)
{
    new (address) ntcq::AcceptCallbackQueueEntry(allocator);
}

AcceptCallbackQueueEntryPool::AcceptCallbackQueueEntryPool(
    bslma::Allocator* basicAllocator)
: d_pool(NTCCFG_BIND(&AcceptCallbackQueueEntryPool::construct,
                     NTCCFG_BIND_PLACEHOLDER_1,
                     NTCCFG_BIND_PLACEHOLDER_2),
         1,
         basicAllocator)
{
}

AcceptCallbackQueueEntryPool::~AcceptCallbackQueueEntryPool()
{
    BSLS_ASSERT(d_pool.numObjects() == d_pool.numAvailableObjects());
}

AcceptCallbackQueue::AcceptCallbackQueue(bslma::Allocator* basicAllocator)
: d_entryList(basicAllocator)
, d_entryPool(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

AcceptCallbackQueue::~AcceptCallbackQueue()
{
    BSLS_ASSERT(d_entryList.empty());
}

AcceptQueue::AcceptQueue(bslma::Allocator* basicAllocator)
: d_entryList(basicAllocator)
, d_size(0)
, d_watermarkLow(NTCCFG_DEFAULT_LISTENER_SOCKET_ACCEPT_QUEUE_LOW_WATERMARK)
, d_watermarkLowWanted(true)
, d_watermarkHigh(NTCCFG_DEFAULT_LISTENER_SOCKET_ACCEPT_QUEUE_HIGH_WATERMARK)
, d_watermarkHighWanted(true)
, d_callbackQueue(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntcs::WatermarkUtil::sanitizeIncomingQueueWatermarks(&d_watermarkLow,
                                                         &d_watermarkHigh);
}

AcceptQueue::~AcceptQueue()
{
}

}  // close package namespace
}  // close enterprise namespace
