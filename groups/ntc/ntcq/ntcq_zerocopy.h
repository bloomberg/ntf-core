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

#ifndef INCLUDED_NTCQ_ZEROCOPY
#define INCLUDED_NTCQ_ZEROCOPY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_data.h>
#include <ntsa_error.h>
#include <ntsa_transport.h>
#include <ntsa_zerocopy.h>
#include <ntci_datapool.h>
#include <ntci_sendcallback.h>
#include <ntcq_send.h>
#include <bdlbb_blob.h>
#include <bslma_allocator.h>
#include <bsls_keyword.h>
#include <bsl_algorithm.h>
#include <bsl_limits.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcq {

/// @internal @brief
/// Describe the 64-bit unsigned integer incremented after each successfully
/// zero-copy 'sendmsg' system call.
///
/// @details
/// Some operating system implementations may internally use 32-bit unsigned
/// integers to identify a zero-copy 'sendmsg' system call. This library's
/// implementation detects 32-bit wraparound and converts to 64-bit unsigned
/// integers automatically when interpreting 'ntsa::ZeroCopy'.
///
/// @ingroup module_ntcq
typedef bsl::uint64_t ZeroCopyCounter;

/// @internal @brief
/// Describe a half-open range of zero-copy counters.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcq
class ZeroCopyRange
{
    ntcq::ZeroCopyCounter d_minCounter;
    ntcq::ZeroCopyCounter d_maxCounter;

public:
    /// Create a new zero-copy range.
    ZeroCopyRange();

    /// Create a new zero-copy range from the specified 'minCounter',
    /// inclusive, to the specified 'maxCounter', exclusive.
    ZeroCopyRange(ntcq::ZeroCopyCounter minCounter,
                  ntcq::ZeroCopyCounter maxCounter);

    /// Create a new zero-copy range having the same value as the specified
    /// 'original' object.
    ZeroCopyRange(const ZeroCopyRange& original);

    /// Destroy this object.
    ~ZeroCopyRange();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    ZeroCopyRange& operator=(const ZeroCopyRange& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the minimum zero-copy counter of the range, inclusive, to the
    /// specified 'counter'.
    void setMinCounter(ntcq::ZeroCopyCounter counter);

    /// Set the maximum zero-copy counter of range, exclusive, to the specified
    /// 'counter'.
    void setMaxCounter(ntcq::ZeroCopyCounter counter);

    /// Return the minimum zero-copy counter of the range, inclusive.
    ntcq::ZeroCopyCounter minCounter() const;

    /// Return the maximum zero-copy counter of the range, exclusive.
    ntcq::ZeroCopyCounter maxCounter() const;

    /// Return the number of contiguous counter represented by this range.
    bsl::size_t size() const;

    /// Return true if the range is empty, otherwise return false.
    bool empty() const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Return the range that is the intersection of the specified 'lhs' and
    /// 'rhs' ranges.
    static ZeroCopyRange intersect(const ZeroCopyRange& lhs,
                                   const ZeroCopyRange& rhs);

    /// Calculate the range that is the difference between the specified 'lhs'
    /// and 'rhs' ranges. If the difference is contiguous, load it into the
    /// specified 'result' and load the empty range into the specified
    /// 'overflow'. Otherwise, load the lesser difference into 'result' and the
    /// greater difference into the specified 'overflow'.
    static void difference(ZeroCopyRange*       result,
                           ZeroCopyRange*       overflow,
                           const ZeroCopyRange& lhs,
                           const ZeroCopyRange& rhs);

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(ZeroCopyRange);
};

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
///
/// @related ntcq::ZeroCopyRange
bsl::ostream& operator<<(bsl::ostream& stream, const ZeroCopyRange& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntcq::ZeroCopyRange
bool operator==(const ZeroCopyRange& lhs, const ZeroCopyRange& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntcq::ZeroCopyRange
bool operator!=(const ZeroCopyRange& lhs, const ZeroCopyRange& rhs);

/// @internal @brief
/// Describe an entry in a zero-copy queue.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcq
class ZeroCopyEntry
{
    typedef bsl::vector<ntcq::ZeroCopyRange> RangeSet;

    ntcq::SendCounter           d_group;
    ntcq::ZeroCopyRange         d_range;
    RangeSet                    d_rangeSet;
    bool                        d_framed;
    bsl::shared_ptr<ntsa::Data> d_data_sp;
    ntsa::Error                 d_error;
    ntci::SendCallback          d_callback;
    bslma::Allocator*           d_allocator_p;

  public:
    /// Create a new zero-copy entry. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit ZeroCopyEntry(bslma::Allocator* basicAllocator = 0);

    /// Create a new zero-copy entry having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    ZeroCopyEntry(const ZeroCopyEntry& original,
                  bslma::Allocator*    basicAllocator = 0);

    /// Destroy this object.
    ~ZeroCopyEntry();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    ZeroCopyEntry& operator=(const ZeroCopyEntry& other);

    /// Set the identifier of the data to the specified 'group'.
    void setGroup(ntcq::SendCounter group);

    /// Set the minimum zero-copy counter needed, inclusive, to the specified
    /// 'counter'.
    void setMinCounter(ntcq::ZeroCopyCounter counter);

    /// Set the maximum zero-copy counter needed, exclusive, to the specified
    /// 'counter'.
    void setMaxCounter(ntcq::ZeroCopyCounter counter);

    /// Set the flag that indicates all portions of the data have been
    /// sent (zero-copied or not), so that no further zero-copy counters are
    /// expected, to the specified 'framed' value.
    void setFramed(bool framed);

    /// Set the data transmitted for the group to the specified 'data'.
    void setData(const bsl::shared_ptr<ntsa::Data>& data);

    /// Set the error encountered during transmission to the specified 'error'.
    void setError(const ntsa::Error& error);

    /// Set the callback invoked when the data has been completely transmitted
    /// to the specified 'callback'.
    void setCallback(const ntci::SendCallback& callback);

    /// Match the specified 'complete' zero-copy counters against the zero-copy
    /// counters required to complete this entry, remove the intersection from
    /// those needed, and if the result is the empty set indicate the entry is
    /// complete.
    void match(const ntcq::ZeroCopyRange& complete);

    /// Return the identifier of the data.
    ntcq::SendCounter group() const;

    /// Return the minimum zero-copy counter needed, inclusive.
    ntcq::ZeroCopyCounter minCounter() const;

    /// Return the maximum zero-copy counter needed, inclusive.
    ntcq::ZeroCopyCounter maxCounter() const;

    /// Return the flag that indicates all portions of the data have been
    /// sent (zero-copied or not), so that no further zero-copy counters are
    /// expected.
    bool framed() const;

    /// Return the flag that indicates all portions of the data that have been
    /// (or will be) zero-copied are complete.
    bool complete() const;

    /// Return the error encountered during transmission, if any.
    ntsa::Error error() const;

    /// Return the callback invoked when the data has been completely
    /// transmitted.
    const ntci::SendCallback& callback() const;

    /// Return the allocator used to supply memory.
    bslma::Allocator* allocator() const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(ZeroCopyEntry);
};

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
///
/// @related ntcq::ZeroCopyEntry
bsl::ostream& operator<<(bsl::ostream& stream, const ZeroCopyEntry& object);

/// @internal @brief
/// Provide a zero-copy counter generator.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcq
class ZeroCopyCounterGenerator
{
    ntcq::ZeroCopyCounter d_next;
    ntcq::ZeroCopyCounter d_bias;
    ntcq::ZeroCopyCounter d_generation;

private:
    ZeroCopyCounterGenerator(
        const ZeroCopyCounterGenerator&) BSLS_KEYWORD_DELETED;
    ZeroCopyCounterGenerator& operator=(
        const ZeroCopyCounterGenerator&) BSLS_KEYWORD_DELETED;

public:
    /// Create a new zero copy counter generator from the default epoch of
    /// zero.
    ZeroCopyCounterGenerator();

    /// Destroy this object.
    ~ZeroCopyCounterGenerator();

    /// Configure the generator to return the specified 'next' counter in the
    /// specified 32-bit wraparound 'generation'.
    void configure(ntcq::ZeroCopyCounter next,
                   bsl::size_t           generation);

    // Return the next zero-copy counter.
    ntcq::ZeroCopyCounter next();

    /// Update the 32-bit wraparound generation and return the 64-bit half-open
    /// range that is equivalent to the specified 'zeroCopy' update.
    ntcq::ZeroCopyRange update(const ntsa::ZeroCopy& zeroCopy);
};

/// @internal @brief
/// Provide a queue of operations requested to be zero-copied, and a
/// correlation mechanism to learn when they are complete.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcq
class ZeroCopyQueue
{
    typedef bsl::list<ntcq::ZeroCopyEntry> EntryList;

    ntcq::ZeroCopyCounterGenerator  d_generator;
    EntryList                       d_waitList;
    EntryList                       d_doneList;
    bsl::shared_ptr<ntci::DataPool> d_dataPool_sp;
    bslma::Allocator*               d_allocator_p;

  private:
    ZeroCopyQueue(const ZeroCopyQueue&) BSLS_KEYWORD_DELETED;
    ZeroCopyQueue& operator=(const ZeroCopyQueue&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new zero-copy queue. Allocate data containers using the
    /// specified 'dataPool'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit ZeroCopyQueue(
        const bsl::shared_ptr<ntci::DataPool>& dataPool,
        bslma::Allocator*                      basicAllocator = 0);

    /// Destroy this object.
    ~ZeroCopyQueue();

    /// Append a new zero-copy entry for the specified 'data' sent as part of
    /// the specified 'group'. Return the first zero-copy counter associated
    /// with this entry.
    ntcq::ZeroCopyCounter push(ntcq::SendCounter  group,
                               const bdlbb::Blob& data);

    /// Append a new zero-copy entry for the specified 'data' sent as part of
    /// the specified 'group'. When sending the 'data' is complete, the
    /// specified 'callback' should be invoked. Return the first zero-copy
    /// counter associated with this entry.
    ntcq::ZeroCopyCounter push(ntcq::SendCounter         group,
                               const bdlbb::Blob&        data,
                               const ntci::SendCallback& callback);

    /// Append a new zero-copy entry for the specified 'data' sent as part of
    /// the specified 'group'. Return the first zero-copy counter associated
    /// with this entry.
    ntcq::ZeroCopyCounter push(ntcq::SendCounter group,
                               const ntsa::Data& data);

    /// Append a new zero-copy entry for the specified 'data' sent as part of
    /// the specified 'group'. When sending the 'data' is complete, the
    /// specified 'callback' should be invoked. Return the first zero-copy
    /// counter associated with this entry.
    ntcq::ZeroCopyCounter push(ntcq::SendCounter         group,
                               const ntsa::Data&         data,
                               const ntci::SendCallback& callback);

    /// Append a new zero-copy entry for the specified 'data' sent as part of
    /// the specified 'group'. Return the first zero-copy counter associated
    /// with this entry.
    ntcq::ZeroCopyCounter push(ntcq::SendCounter                  group,
                               const bsl::shared_ptr<ntsa::Data>& data);

    /// Append a new zero-copy entry the specified 'data' sent as part of the
    /// specified 'group'. When sending the 'data' is complete, the specified
    /// 'callback' should be invoked. Return the first zero-copy counter
    /// associated with this entry.
    ntcq::ZeroCopyCounter push(ntcq::SendCounter                  group,
                               const bsl::shared_ptr<ntsa::Data>& data,
                               const ntci::SendCallback&          callback);

    /// Extend the last zero-copy entry sent as part of the specified 'group'.
    /// Return the next zero-copy counter associated with this entry.
    ntcq::ZeroCopyCounter push(ntcq::SendCounter group);

    /// Indicate zero copy entry sent as part of the specified 'group' will
    /// have no subsequent system calls performed to send its data.
    void frame(ntcq::SendCounter group);

    /// Update the queue that the specified 'zeroCopy' range is complete.
    /// Return the error.
    ntsa::Error update(const ntsa::ZeroCopy& zeroCopy);

    /// Pop the oldest, completed entry and load its callback, if any, into the
    /// specified 'result'. Return true if such and entry and callback exists,
    /// otherwise return false.
    bool pop(ntci::SendCallback* result);

    /// Pop each completed entry and append its callback, if any, into the
    /// specified 'result'. Return true if such and entry and callback exists,
    /// otherwise return false.
    bool pop(bsl::vector<ntci::SendCallback>* result);

    /// Remove all entries from the queue.
    void clear();

    /// Remove all entries from the queue and load the callback, if any, for
    /// each entry into the specified 'result'.
    void clear(bsl::vector<ntci::SendCallback>* result);

    /// Return the allocator used to supply memory.
    bslma::Allocator* allocator() const;

    /// Load each entry into the specified 'result'.
    void load(bsl::vector<ntcq::ZeroCopyEntry>* result) const;

    /// Return true if the queue a completed entry with a callback, otherwise
    /// return false.
    bool ready() const;

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(ZeroCopyQueue);
};

NTCCFG_INLINE
ZeroCopyRange::ZeroCopyRange()
: d_minCounter(0)
, d_maxCounter(0)
{
}

NTCCFG_INLINE
ZeroCopyRange::ZeroCopyRange(ntcq::ZeroCopyCounter minCounter,
                             ntcq::ZeroCopyCounter maxCounter)
: d_minCounter(minCounter)
, d_maxCounter(maxCounter)
{
}

NTCCFG_INLINE
ZeroCopyRange::ZeroCopyRange(const ZeroCopyRange& original)
: d_minCounter(original.d_minCounter)
, d_maxCounter(original.d_maxCounter)
{
}

NTCCFG_INLINE
ZeroCopyRange::~ZeroCopyRange()
{
}

NTCCFG_INLINE
ZeroCopyRange& ZeroCopyRange::operator=(const ZeroCopyRange& other)
{
    d_minCounter = other.d_minCounter;
    d_maxCounter = other.d_maxCounter;
    return *this;
}

NTCCFG_INLINE
void ZeroCopyRange::reset()
{
    d_minCounter = 0;
    d_maxCounter = 0;
}

NTCCFG_INLINE
void ZeroCopyRange::setMinCounter(ntcq::ZeroCopyCounter counter)
{
    d_minCounter = counter;
}

NTCCFG_INLINE
void ZeroCopyRange::setMaxCounter(ntcq::ZeroCopyCounter counter)
{
    d_maxCounter = counter;
}

NTCCFG_INLINE
ntcq::ZeroCopyCounter ZeroCopyRange::minCounter() const
{
    return d_minCounter;
}

NTCCFG_INLINE
ntcq::ZeroCopyCounter ZeroCopyRange::maxCounter() const
{
    return d_maxCounter;
}

NTCCFG_INLINE
bsl::size_t ZeroCopyRange::size() const
{
    return static_cast<bsl::size_t>(d_maxCounter - d_minCounter);
}

NTCCFG_INLINE
bool ZeroCopyRange::empty() const
{
    return d_minCounter == d_maxCounter;
}

NTCCFG_INLINE
ZeroCopyRange ZeroCopyRange::intersect(const ZeroCopyRange& lhs,
                                       const ZeroCopyRange& rhs)
{
    ZeroCopyRange result;

    const ntcq::ZeroCopyCounter lhsMin = lhs.minCounter();
    const ntcq::ZeroCopyCounter lhsMax = lhs.maxCounter();

    const ntcq::ZeroCopyCounter rhsMin = rhs.minCounter();
    const ntcq::ZeroCopyCounter rhsMax = rhs.maxCounter();

    const ntcq::ZeroCopyCounter resultMin = bsl::max(lhsMin, rhsMin);
    const ntcq::ZeroCopyCounter resultMax = bsl::min(lhsMax, rhsMax);

    if (resultMax >= resultMin) {
        result.setMinCounter(resultMin);
        result.setMaxCounter(resultMax);
    }
    else {
        result.setMinCounter(0);
        result.setMaxCounter(0);
    }

    return result;
}

NTCCFG_INLINE
void ZeroCopyRange::difference(ZeroCopyRange*       result,
                               ZeroCopyRange*       overflow,
                               const ZeroCopyRange& lhs,
                               const ZeroCopyRange& rhs)
{
    result->reset();
    overflow->reset();

    if (rhs.minCounter() <= lhs.minCounter() &&
        rhs.maxCounter() >= lhs.maxCounter())
    {
        return;
    }

    if (lhs.minCounter() < rhs.minCounter()) {
        result->setMinCounter(lhs.minCounter());
        result->setMaxCounter(bsl::min(lhs.maxCounter(), rhs.minCounter()));
    }

    if (lhs.maxCounter() > rhs.maxCounter()) {
        overflow->setMinCounter(bsl::max(lhs.minCounter(), rhs.maxCounter()));
        overflow->setMaxCounter(lhs.maxCounter());
    }

    if (result->empty()) {
        result->setMinCounter(overflow->minCounter());
        result->setMaxCounter(overflow->maxCounter());
        overflow->reset();
    }
    else if (overflow->minCounter() == result->maxCounter()) {
        result->setMaxCounter(overflow->maxCounter());
        overflow->reset();
    }
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ZeroCopyRange& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ZeroCopyRange& lhs, const ZeroCopyRange& rhs)
{
    return lhs.minCounter() == rhs.minCounter() &&
           lhs.maxCounter() == rhs.maxCounter();
}

NTCCFG_INLINE
bool operator!=(const ZeroCopyRange& lhs, const ZeroCopyRange& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
ZeroCopyEntry::ZeroCopyEntry(bslma::Allocator* basicAllocator)
: d_group(0)
, d_range()
, d_rangeSet(basicAllocator)
, d_framed(false)
, d_data_sp()
, d_error()
, d_callback(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

NTCCFG_INLINE
ZeroCopyEntry::ZeroCopyEntry(const ZeroCopyEntry& original,
                             bslma::Allocator*    basicAllocator)
: d_group(original.d_group)
, d_range(original.d_range)
, d_rangeSet(original.d_rangeSet, basicAllocator)
, d_framed(original.d_framed)
, d_data_sp(original.d_data_sp)
, d_error(original.d_error)
, d_callback(original.d_callback, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

NTCCFG_INLINE
ZeroCopyEntry::~ZeroCopyEntry()
{
}

NTCCFG_INLINE
ZeroCopyEntry& ZeroCopyEntry::operator=(const ZeroCopyEntry& other)
{
    if (this != &other) {
        d_group      = other.d_group;
        d_range      = other.d_range;
        d_rangeSet   = other.d_rangeSet;
        d_framed     = other.d_framed;
        d_data_sp    = other.d_data_sp;
        d_error      = other.d_error;
        d_callback   = other.d_callback;
    }

    return *this;
}

NTCCFG_INLINE
void ZeroCopyEntry::setGroup(ntcq::SendCounter group)
{
    d_group = group;
}

NTCCFG_INLINE
void ZeroCopyEntry::setMinCounter(ntcq::ZeroCopyCounter counter)
{
    if (d_rangeSet.empty()) {
        d_range.setMinCounter(counter);
    }
    else {
        d_rangeSet.front().setMinCounter(counter);
    }
}

NTCCFG_INLINE
void ZeroCopyEntry::setMaxCounter(ntcq::ZeroCopyCounter counter)
{
    if (d_rangeSet.empty()) {
        d_range.setMaxCounter(counter);
    }
    else {
        d_rangeSet.back().setMaxCounter(counter);
    }
}

NTCCFG_INLINE
void ZeroCopyEntry::setFramed(bool framed)
{
    d_framed = framed;
}

NTCCFG_INLINE
void ZeroCopyEntry::setData(const bsl::shared_ptr<ntsa::Data>& data)
{
    d_data_sp = data;
}

NTCCFG_INLINE
void ZeroCopyEntry::setError(const ntsa::Error& error)
{
    d_error = error;
}

NTCCFG_INLINE
void ZeroCopyEntry::setCallback(const ntci::SendCallback& callback)
{
    d_callback = callback;
}

NTCCFG_INLINE
ntcq::SendCounter ZeroCopyEntry::group() const
{
    return d_group;
}

NTCCFG_INLINE
ntcq::ZeroCopyCounter ZeroCopyEntry::minCounter() const
{
    if (d_rangeSet.empty()) {
        return d_range.minCounter();
    }
    else {
        return d_rangeSet.front().minCounter();
    }
}

NTCCFG_INLINE
ntcq::ZeroCopyCounter ZeroCopyEntry::maxCounter() const
{
    if (d_rangeSet.empty()) {
        return d_range.maxCounter();
    }
    else {
        return d_rangeSet.back().maxCounter();
    }
}

NTCCFG_INLINE
bool ZeroCopyEntry::framed() const
{
    return d_framed;
}

NTCCFG_INLINE
ntsa::Error ZeroCopyEntry::error() const
{
    return d_error;
}

NTCCFG_INLINE
const ntci::SendCallback& ZeroCopyEntry::callback() const
{
    return d_callback;
}

NTCCFG_INLINE
bslma::Allocator* ZeroCopyEntry::allocator() const
{
    return d_allocator_p;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ZeroCopyEntry& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
ZeroCopyCounterGenerator::ZeroCopyCounterGenerator()
: d_next(0)
, d_bias(0)
, d_generation(0)
{
}

NTCCFG_INLINE
ZeroCopyCounterGenerator::~ZeroCopyCounterGenerator()
{
}

NTCCFG_INLINE
void ZeroCopyCounterGenerator::configure(ntcq::ZeroCopyCounter next,
                                         bsl::size_t           generation)
{
    const ntcq::ZeroCopyCounter k_UINT32_MAX =
        static_cast<ntcq::ZeroCopyCounter>(
            bsl::numeric_limits<bsl::uint32_t>::max());

    d_next = next;
    d_bias = static_cast<ntcq::ZeroCopyCounter>(generation * k_UINT32_MAX);
    d_generation = generation;
}

NTCCFG_INLINE
ntcq::ZeroCopyCounter ZeroCopyCounterGenerator::next()
{
    return d_next++;
}

NTCCFG_INLINE
ntcq::ZeroCopyRange ZeroCopyCounterGenerator::update(
    const ntsa::ZeroCopy& zeroCopy)
{
    const ntcq::ZeroCopyCounter k_UINT32_MAX =
        static_cast<ntcq::ZeroCopyCounter>(
            bsl::numeric_limits<bsl::uint32_t>::max());

    const ntcq::ZeroCopyCounter zeroCopyFrom =
        static_cast<ntcq::ZeroCopyCounter>(zeroCopy.from());

    const ntcq::ZeroCopyCounter zeroCopyThru =
        static_cast<ntcq::ZeroCopyCounter>(zeroCopy.thru());

    const ntcq::ZeroCopyCounter offset = d_bias + d_generation;

    ntcq::ZeroCopyRange zeroCopyRange;

    if (zeroCopyFrom > zeroCopyThru) {
        const ntcq::ZeroCopyCounter size =
            static_cast<ntcq::ZeroCopyCounter>(
                (k_UINT32_MAX - zeroCopyFrom) + zeroCopyThru + 2);

        zeroCopyRange.setMinCounter(offset + zeroCopyFrom);
        zeroCopyRange.setMaxCounter(zeroCopyRange.minCounter() + size);

        d_bias       += k_UINT32_MAX;
        d_generation += 1;
    }
    else {
        zeroCopyRange.setMinCounter(offset + zeroCopyFrom);
        zeroCopyRange.setMaxCounter(offset + zeroCopyThru + 1);

        if (zeroCopyThru == k_UINT32_MAX) {
            d_bias       += k_UINT32_MAX;
            d_generation += 1;
        }
    }

    return zeroCopyRange;
}

}  // close package namespace
}  // close enterprise namespace

#endif  //INCLUDED_NTCQ_ZEROCOPY
