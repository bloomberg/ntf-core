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

#include <ntcq_send.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcq_send_cpp, "$Id$ $CSID$")

#include <ntccfg_bind.h>
#include <ntccfg_limits.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace ntcq {

SendCallbackQueueEntry::SendCallbackQueueEntry(
    bslma::Allocator* basicAllocator)
: d_object("ntcq::SendCallbackQueueEntry")
, d_state()
, d_callback(basicAllocator)
, d_options()
, d_timer_sp()
{
}

SendCallbackQueueEntry::~SendCallbackQueueEntry()
{
    BSLS_ASSERT(!d_callback);
    BSLS_ASSERT(!d_timer_sp);
}

void SendCallbackQueueEntry::clear()
{
    d_state.reset();
    d_callback.reset();
    d_options.reset();
    if (d_timer_sp) {
        d_timer_sp->close();
        d_timer_sp.reset();
    }
}

void SendCallbackQueueEntry::dispatch(
    const bsl::shared_ptr<ntcq::SendCallbackQueueEntry>& entry,
    const bsl::shared_ptr<ntci::Sender>&                 sender,
    const ntca::SendEvent&                               event,
    const bsl::shared_ptr<ntci::Strand>&                 strand,
    const bsl::shared_ptr<ntci::Executor>&               executor,
    bool                                                 defer,
    bslmt::Mutex*                                        mutex)
{
    if (NTCCFG_LIKELY(entry->d_state.finish())) {
        if (entry->d_timer_sp) {
            entry->d_timer_sp->close();
            entry->d_timer_sp.reset();
        }

        ntci::SendCallback callback = entry->d_callback;
        entry->d_callback.reset();

        if (callback) {
            callback.dispatch(sender, event, strand, executor, defer, mutex);
        }
    }
}

void SendCallbackQueueEntryPool::construct(void*             address,
                                           bslma::Allocator* allocator)
{
    new (address) ntcq::SendCallbackQueueEntry(allocator);
}

SendCallbackQueueEntryPool::SendCallbackQueueEntryPool(
    bslma::Allocator* basicAllocator)
: d_pool(NTCCFG_BIND(&SendCallbackQueueEntryPool::construct,
                     NTCCFG_BIND_PLACEHOLDER_1,
                     NTCCFG_BIND_PLACEHOLDER_2),
         1,
         basicAllocator)
{
}

SendCallbackQueueEntryPool::~SendCallbackQueueEntryPool()
{
    BSLS_ASSERT(d_pool.numObjects() == d_pool.numAvailableObjects());
}

SendQueue::SendQueue(bslma::Allocator* basicAllocator)
: d_entryList(basicAllocator)
, d_data_sp()
, d_size(0)
, d_watermarkLow(NTCCFG_DEFAULT_STREAM_SOCKET_WRITE_QUEUE_LOW_WATERMARK)
, d_watermarkLowWanted(false)
, d_watermarkHigh(NTCCFG_DEFAULT_STREAM_SOCKET_WRITE_QUEUE_HIGH_WATERMARK)
, d_watermarkHighWanted(true)
, d_nextEntryId(1)
, d_callbackEntryPool(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntcs::WatermarkUtil::sanitizeOutgoingQueueWatermarks(&d_watermarkLow,
                                                         &d_watermarkHigh);
}

SendQueue::~SendQueue()
{
}

}  // close package namespace
}  // close enterprise namespace
