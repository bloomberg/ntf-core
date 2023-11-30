// Copyright 2023 Bloomberg Finance L.P.
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

#include <ntcq_zerocopy.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcq_zerocopy_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntcq {

ZeroCopyEntry::ZeroCopyEntry()
: d_id(0)
{
}

ZeroCopyEntry::~ZeroCopyEntry()
{
}

void ZeroCopyEntry::setCallback(const ntci::SendCallback& callback)
{
    d_callback = callback;
}
void ZeroCopyEntry::setId(const bsl::uint32_t id)
{
    d_id = id;
}

const ntci::SendCallback& ZeroCopyEntry::callback() const
{
    return d_callback;
}

bsl::uint32_t ZeroCopyEntry::id() const
{
    return d_id;
}

void ZeroCopyEntry::dispatch(const bsl::shared_ptr<ntci::Sender>&   sender,
                             const bsl::shared_ptr<ntci::Strand>&   strand,
                             const bsl::shared_ptr<ntci::Executor>& executor,
                             bool                                   defer,
                             bslmt::Mutex*                          mutex)
{
    ntci::SendCallback callback = d_callback;
    d_callback.reset();

    if (callback) {
        ntca::SendEvent sendEvent;
        sendEvent.setType(ntca::SendEventType::e_COMPLETE);
        sendEvent.setContext(d_sendContext);

        callback.dispatch(sender, sendEvent, strand, executor, defer, mutex);
    }
}

ZeroCopyWaitList::ZeroCopyWaitList(
    const bsl::shared_ptr<ntci::Sender>&   sender,
    const bsl::shared_ptr<ntci::Executor>& executor,
    bslma::Allocator*                      basicAllocator)
: d_entryList(basicAllocator)
, d_sender(sender)
, d_executor(executor)
, d_strand()
, d_nextId(0)
{
}

ZeroCopyWaitList::~ZeroCopyWaitList()
{
    BSLS_ASSERT(d_entryList.empty());
}

void ZeroCopyWaitList::setStrand(const bsl::shared_ptr<ntci::Strand>& strand)
{
    d_strand = strand;
}

void ZeroCopyWaitList::addEntry(ZeroCopyEntry& entry)
{
    entry.setId(d_nextId++);
    d_entryList.push_back(entry);
}

void ZeroCopyWaitList::zeroCopyAcknowledge(const ntsa::ZeroCopy& zc)
{
    const bsl::uint32_t from = zc.from();
    const bsl::uint32_t to   = zc.to();

    const bool overflow = to > from;

    const bsl::uint32_t acknowledged =
        (!overflow ? (to - from)
                   : (bsl::numeric_limits<bsl::uint32_t>::max() - from + to)) +
        1;

    bsl::uint32_t                   matched = 0;
    const EntryList::const_iterator end     = d_entryList.cend();
    EntryList::iterator             it      = d_entryList.begin();
    while ((matched < acknowledged) && (it != end)) {
        ntcq::ZeroCopyEntry& entry = *it;

        bool match = false;
        if (!overflow) {
            if (entry.id() >= from && entry.id() <= to) {
                match = true;
            }
        }
        else {
            if (entry.id() >= from || entry.id() <= to) {
                match = true;
            }
        }

        matched += static_cast<bsl::uint32_t>(match);

        if (match) {
            entry.dispatch(d_sender, d_strand, d_executor, true, 0);
            it = d_entryList.erase(it);
        }
        else {
            ++it;
        }
    }
}

}  // close package namespace
}  // close enterprise namespace
