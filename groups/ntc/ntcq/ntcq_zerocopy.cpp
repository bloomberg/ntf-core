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

#include <ntci_log.h>
#include <bslim_printer.h>

namespace BloombergLP {
namespace ntcq {

bsl::ostream& ZeroCopyRange::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (d_minCounter == d_maxCounter) {
        printer.printValue("EMPTY");
    }
    else {
        printer.printAttribute("min", d_minCounter);
        printer.printAttribute("max", d_maxCounter);
    }

    printer.end();
    return stream;
}

void ZeroCopyEntry::match(const ntcq::ZeroCopyRange& complete)
{
    if (d_rangeSet.empty()) {
        ntcq::ZeroCopyRange required = d_range;

        ntcq::ZeroCopyRange intersection =
            ntcq::ZeroCopyRange::intersect(required, complete);

        if (intersection.empty()) {
            return;
        }

        ntcq::ZeroCopyRange pending;
        ntcq::ZeroCopyRange overflow;

        ntcq::ZeroCopyRange::difference(
            &pending, &overflow, required, intersection);

        if (overflow.empty()) {
            d_range = pending;
        }
        else {
            d_rangeSet.push_back(pending);
            d_rangeSet.push_back(overflow);
            d_range.reset();
        }

        return;
    }

    RangeSet rangeSet(d_allocator_p);
    rangeSet.swap(d_rangeSet);

    RangeSet::const_iterator it = rangeSet.begin();
    RangeSet::const_iterator et = rangeSet.end();

    for (; it != et; ++it) {
        ntcq::ZeroCopyRange required = *it;

        ntcq::ZeroCopyRange intersection =
            ntcq::ZeroCopyRange::intersect(required, complete);

        if (intersection.empty()) {
            d_rangeSet.push_back(required);
            continue;
        }

        ntcq::ZeroCopyRange pending;
        ntcq::ZeroCopyRange overflow;

        ntcq::ZeroCopyRange::difference(
            &pending, &overflow, required, intersection);

        if (!pending.empty()) {
            d_rangeSet.push_back(pending);
        }

        if (!overflow.empty()) {
            d_rangeSet.push_back(overflow);
        }
    }
}

bool ZeroCopyEntry::complete() const
{
    if (d_framed) {
        if (d_rangeSet.empty()) {
            return d_range.empty();
        }
        else {
            RangeSet::const_iterator it = d_rangeSet.begin();
            RangeSet::const_iterator et = d_rangeSet.end();

            for (; it != et; ++it) {
                const ntcq::ZeroCopyRange& range = *it;
                if (!range.empty()) {
                    return false;
                }
            }

            return true;
        }
    }

    return false;
}

bsl::ostream& ZeroCopyEntry::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("group", d_group);

    if (d_rangeSet.empty()) {
        printer.printAttribute("range", d_range);
    }
    else {
        printer.printAttribute("range", d_rangeSet);
    }

    if (this->complete()) {
        printer.printAttribute("state", "COMPLETE");
    }
    else {
        printer.printAttribute("state", "PENDING");
    }

    printer.end();
    return stream;
}

ZeroCopyQueue::ZeroCopyQueue(
    const bsl::shared_ptr<ntci::DataPool>& dataPool,
    bslma::Allocator*                      basicAllocator)
: d_generator()
, d_waitList(basicAllocator)
, d_doneList(basicAllocator)
, d_dataPool_sp(dataPool)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ZeroCopyQueue::~ZeroCopyQueue()
{
}

ntcq::ZeroCopyCounter ZeroCopyQueue::push(ntcq::SendCounter  group,
                                          const bdlbb::Blob& data)
{
    BSLS_ASSERT(d_waitList.empty() || d_waitList.front().group() < group);

    ntcq::ZeroCopyCounter counter = d_generator.next();

    bsl::shared_ptr<ntsa::Data> dataContainer =
        d_dataPool_sp->createOutgoingData();

    dataContainer->makeBlob(data);

    d_waitList.resize(d_waitList.size() + 1);
    ZeroCopyEntry& entry = d_waitList.back();

    entry.setGroup(group);
    entry.setMinCounter(counter);
    entry.setMaxCounter(counter + 1);
    entry.setData(dataContainer);

    return counter;
}

ntcq::ZeroCopyCounter ZeroCopyQueue::push(ntcq::SendCounter         group,
                                          const bdlbb::Blob&        data,
                                          const ntci::SendCallback& callback)
{
    BSLS_ASSERT(d_waitList.empty() || d_waitList.front().group() < group);

    ntcq::ZeroCopyCounter counter = d_generator.next();

    bsl::shared_ptr<ntsa::Data> dataContainer =
        d_dataPool_sp->createOutgoingData();

    dataContainer->makeBlob(data);

    d_waitList.resize(d_waitList.size() + 1);
    ZeroCopyEntry& entry = d_waitList.back();

    entry.setGroup(group);
    entry.setMinCounter(counter);
    entry.setMaxCounter(counter + 1);
    entry.setData(dataContainer);

    if (callback) {
        entry.setCallback(callback);
    }

    return counter;
}

ntcq::ZeroCopyCounter ZeroCopyQueue::push(ntcq::SendCounter group,
                                          const ntsa::Data& data)
{
    BSLS_ASSERT(d_waitList.empty() || d_waitList.front().group() < group);

    ntcq::ZeroCopyCounter counter = d_generator.next();

    bsl::shared_ptr<ntsa::Data> dataContainer =
        d_dataPool_sp->createOutgoingData();

    *dataContainer = data;

    d_waitList.resize(d_waitList.size() + 1);
    ZeroCopyEntry& entry = d_waitList.back();

    entry.setGroup(group);
    entry.setMinCounter(counter);
    entry.setMaxCounter(counter + 1);
    entry.setData(dataContainer);

    return counter;
}

ntcq::ZeroCopyCounter ZeroCopyQueue::push(ntcq::SendCounter         group,
                                          const ntsa::Data&         data,
                                          const ntci::SendCallback& callback)
{
    BSLS_ASSERT(d_waitList.empty() || d_waitList.front().group() < group);

    ntcq::ZeroCopyCounter counter = d_generator.next();

    bsl::shared_ptr<ntsa::Data> dataContainer =
        d_dataPool_sp->createOutgoingData();

    *dataContainer = data;

    d_waitList.resize(d_waitList.size() + 1);
    ZeroCopyEntry& entry = d_waitList.back();

    entry.setGroup(group);
    entry.setMinCounter(counter);
    entry.setMaxCounter(counter + 1);
    entry.setData(dataContainer);

    if (callback) {
        entry.setCallback(callback);
    }

    return counter;
}

ntcq::ZeroCopyCounter ZeroCopyQueue::push(
    ntcq::SendCounter                  group,
    const bsl::shared_ptr<ntsa::Data>& data)
{
    BSLS_ASSERT(d_waitList.empty() || d_waitList.front().group() < group);

    ntcq::ZeroCopyCounter counter = d_generator.next();

    d_waitList.resize(d_waitList.size() + 1);
    ZeroCopyEntry& entry = d_waitList.back();

    entry.setGroup(group);
    entry.setMinCounter(counter);
    entry.setMaxCounter(counter + 1);
    entry.setData(data);

    return counter;
}

ntcq::ZeroCopyCounter ZeroCopyQueue::push(
    ntcq::SendCounter                  group,
    const bsl::shared_ptr<ntsa::Data>& data,
    const ntci::SendCallback&          callback)
{
    BSLS_ASSERT(d_waitList.empty() || d_waitList.front().group() < group);

    ntcq::ZeroCopyCounter counter = d_generator.next();

    d_waitList.resize(d_waitList.size() + 1);
    ZeroCopyEntry& entry = d_waitList.back();

    entry.setGroup(group);
    entry.setMinCounter(counter);
    entry.setMaxCounter(counter + 1);
    entry.setData(data);

    if (callback) {
        entry.setCallback(callback);
    }

    return counter;
}

ntcq::ZeroCopyCounter ZeroCopyQueue::push(ntcq::SendCounter group)
{
    NTCCFG_WARNING_UNUSED(group);

    BSLS_ASSERT(!d_waitList.empty());

    ntcq::ZeroCopyCounter counter = d_generator.next();

    ZeroCopyEntry& entry = d_waitList.back();
    BSLS_ASSERT(entry.group() == group);

    entry.setMaxCounter(counter + 1);

    return counter;
}

void ZeroCopyQueue::frame(ntcq::SendCounter group)
{
    NTCCFG_WARNING_UNUSED(group);

    BSLS_ASSERT(!d_waitList.empty());

    ZeroCopyEntry& entry = d_waitList.back();
    BSLS_ASSERT(entry.group() == group);

    entry.setFramed(true);

    if (entry.complete()) {
        d_doneList.push_back(entry);
        d_waitList.pop_back();
    }
}

ntsa::Error ZeroCopyQueue::update(const ntsa::ZeroCopy& zeroCopy)
{
    ntcq::ZeroCopyRange zeroCopyRange = d_generator.update(zeroCopy);

    EntryList::iterator current = d_waitList.begin();

    while (true) {
        if (current == d_waitList.end()) {
            break;
        }

        ZeroCopyEntry& entry = *current;

        if (zeroCopyRange.maxCounter() < entry.minCounter()) {
            break;
        }

        entry.match(zeroCopyRange);

        if (entry.complete()) {
            if (entry.callback()) {
                d_doneList.push_back(entry);
            }

            current = d_waitList.erase(current);
        }
        else {
            ++current;
        }
    }

    return ntsa::Error();
}

bool ZeroCopyQueue::pop(ntci::SendCallback* result)
{
    while (!d_doneList.empty()) {
        if (d_doneList.front().callback()) {
            *result = d_doneList.front().callback();
            d_doneList.pop_front();
            return true;
        }
        else {
            d_doneList.pop_front();
        }
    }

    return false;
}

bool ZeroCopyQueue::pop(bsl::vector<ntci::SendCallback>* result)
{
    bool found = false;

    while (!d_doneList.empty()) {
        if (d_doneList.front().callback()) {
            result->push_back(d_doneList.front().callback());
            d_doneList.pop_front();
            found = true;
        }
        else {
            d_doneList.pop_front();
        }
    }

    return found;
}

void ZeroCopyQueue::clear()
{
    d_waitList.clear();
    d_doneList.clear();
}

void ZeroCopyQueue::clear(bsl::vector<ntci::SendCallback>* result)
{
    if (!d_doneList.empty()) {
        EntryList::iterator it = d_doneList.begin();
        EntryList::iterator et = d_doneList.end();

        for (; it != et; ++it) {
            const ZeroCopyEntry& entry = *it;
            if (entry.callback()) {
                result->push_back(entry.callback());
            }
        }
    }

    if (!d_waitList.empty()) {
        EntryList::iterator it = d_waitList.begin();
        EntryList::iterator et = d_waitList.end();

        for (; it != et; ++it) {
            const ZeroCopyEntry& entry = *it;
            if (entry.callback()) {
                result->push_back(entry.callback());
            }
        }
    }

    d_doneList.clear();
    d_waitList.clear();
}

bslma::Allocator* ZeroCopyQueue::allocator() const
{
    return d_allocator_p;
}

void ZeroCopyQueue::load(bsl::vector<ntcq::ZeroCopyEntry>* result) const
{
    result->insert(result->end(), d_doneList.begin(), d_doneList.end());
    result->insert(result->end(), d_waitList.begin(), d_waitList.end());
}

bool ZeroCopyQueue::ready() const
{
    return !d_doneList.empty();
}

}  // close package namespace
}  // close enterprise namespace
