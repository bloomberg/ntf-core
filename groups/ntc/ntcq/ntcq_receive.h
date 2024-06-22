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

#ifndef INCLUDED_NTCQ_RECEIVE
#define INCLUDED_NTCQ_RECEIVE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_readqueuecontext.h>
#include <ntca_receiveoptions.h>
#include <ntccfg_limits.h>
#include <ntccfg_platform.h>
#include <ntci_log.h>
#include <ntci_receivecallback.h>
#include <ntci_receiver.h>
#include <ntci_strand.h>
#include <ntci_timer.h>
#include <ntcs_callbackstate.h>
#include <ntcs_watermarkutil.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bdlb_nullablevalue.h>
#include <bdlbb_blob.h>
#include <bdlcc_sharedobjectpool.h>
#include <bsls_timeinterval.h>
#include <bsls_timeutil.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_list.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcq {

/// @internal @brief
/// Describe an entry in a receive callback queue.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcq
class ReceiveCallbackQueueEntry
{
    ntccfg::Object               d_object;
    ntcs::CallbackState          d_state;
    ntci::ReceiveCallback        d_callback;
    ntca::ReceiveOptions         d_options;
    bsl::shared_ptr<ntci::Timer> d_timer_sp;

  private:
    ReceiveCallbackQueueEntry(const ReceiveCallbackQueueEntry&)
        BSLS_KEYWORD_DELETED;
    ReceiveCallbackQueueEntry& operator=(const ReceiveCallbackQueueEntry&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new receive callback queue entry. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit ReceiveCallbackQueueEntry(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ReceiveCallbackQueueEntry();

    /// Clear the state of this entry.
    void clear();

    /// Assign the specified 'callback' to be invoked on the specified
    /// 'strand' with the specified 'data' once the specified 'options'
    /// are met.
    void assign(const ntci::ReceiveCallback& callback,
                const ntca::ReceiveOptions&  options);

    /// Set the timer to the specified 'timer'.
    void setTimer(const bsl::shared_ptr<ntci::Timer>& timer);

    /// Close the timer, if any.
    void closeTimer();

    /// Return the criteria to invoke the callback.
    const ntca::ReceiveOptions& options() const;

    /// Invoke the callback of the specified 'entry' for the specified
    /// 'receiver', 'data', and 'event'. If the specified 'defer' flag
    /// is false and the requirements of the strand of the specified 'entry'
    /// permits the callback to be invoked immediately by the 'strand',
    /// unlock the specified 'mutex', invoke the callback, then relock the
    /// 'mutex'. Otherwise, enqueue the invocation of the callback to be
    /// executed on the strand of the 'entry', if defined, or by the
    /// specified 'executor' otherwise.
    static void dispatch(
        const bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>& entry,
        const bsl::shared_ptr<ntci::Receiver>&                  receiver,
        const bsl::shared_ptr<bdlbb::Blob>&                     data,
        const ntca::ReceiveEvent&                               event,
        const bsl::shared_ptr<ntci::Strand>&                    strand,
        const bsl::shared_ptr<ntci::Executor>&                  executor,
        bool                                                    defer,
        ntccfg::Mutex*                                          mutex);

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(ReceiveCallbackQueueEntry);
};

/// @internal @brief
/// Provide a pool of shared pointers to receive callback queue entries.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcq
class ReceiveCallbackQueueEntryPool
{
    typedef bdlcc::SharedObjectPool<
        ntcq::ReceiveCallbackQueueEntry,
        bdlcc::ObjectPoolFunctors::DefaultCreator,
        bdlcc::ObjectPoolFunctors::Clear<ntcq::ReceiveCallbackQueueEntry> >
        Pool;

    Pool d_pool;

  private:
    ReceiveCallbackQueueEntryPool(const ReceiveCallbackQueueEntryPool&)
        BSLS_KEYWORD_DELETED;
    ReceiveCallbackQueueEntryPool& operator=(
        const ReceiveCallbackQueueEntryPool&) BSLS_KEYWORD_DELETED;

  private:
    /// Construct a new receive callback queue entry at the specified
    /// 'address' using the specified 'allocator' to supply memory.
    static void construct(void* address, bslma::Allocator* allocator);

  public:
    /// Create a new object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit ReceiveCallbackQueueEntryPool(
        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ReceiveCallbackQueueEntryPool();

    /// Return a shared pointer to an receive callback queue entry in the
    /// pool having a default value. The resulting receive callback queue
    /// entry is automatically returned to this pool when its reference
    /// count reaches zero.
    bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> create();

    /// Return the total number of objects in the pool.
    bsl::size_t numObjects() const;

    /// Return the number of un-allocated objects available in the pool.
    bsl::size_t numObjectsAvailable() const;
};

/// @internal @brief
/// Provide a receive callback queue.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcq
class ReceiveCallbackQueue
{
    /// Define a type alias for a linked list of callback
    /// entries.
    typedef bsl::list<bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> >
        EntryList;

    /// Define a type alias for a pool of shared pointers to
    /// callback entries.
    typedef ntcq::ReceiveCallbackQueueEntryPool EntryPool;

    EntryList         d_entryList;
    EntryPool         d_entryPool;
    bslma::Allocator* d_allocator_p;

  private:
    ReceiveCallbackQueue(const ReceiveCallbackQueue&) BSLS_KEYWORD_DELETED;
    ReceiveCallbackQueue& operator=(const ReceiveCallbackQueue&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new receive callback queue having an unlimited size.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit ReceiveCallbackQueue(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ReceiveCallbackQueue();

    /// Return a shared pointer to a new receive callback queue entry.
    bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> create();

    /// Push the specified 'entry' onto the callback queue. Return the
    /// error.
    ntsa::Error push(
        const bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>& entry);

    /// Pop the entry at the front of the queue if its criteria is
    /// satisfied by the specified 'numBytesAvailable' and load it into the
    /// specified 'result'. Return the error.
    ntsa::Error pop(bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>* result,
                    bsl::size_t numBytesAvailable);

    /// Remove the specified 'entry' from the queue, if found. Return the
    /// error.
    ntsa::Error remove(
        const bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>& entry);

    /// Remove the entry having the specified 'token' from the queue, if
    /// found, and load it into the specified 'result'. Return the error.
    ntsa::Error remove(
        bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>* result,
        const ntca::ReceiveToken&                         token);

    /// Remove all entries and load them into the specified 'result'.
    void removeAll(
        bsl::vector<bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> >*
            result);

    /// Return the number of callbacks in the queue.
    bsl::size_t size() const;

    /// Return true if the number of callbacks in the queue is zero,
    /// otherwise return false.
    bool empty() const;
};

/// @internal @brief
/// Describe an entry in a receive queue.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcq
class ReceiveQueueEntry
{
    bdlb::NullableValue<ntsa::Endpoint> d_endpoint;
    bsl::shared_ptr<bdlbb::Blob>        d_data_sp;
    bsl::size_t                         d_length;
    bsl::int64_t                        d_timestamp;

  public:
    /// Create a new receive from message queue entry.
    ReceiveQueueEntry();

    /// Set the endpoint to the specified 'endpoint'.
    void setEndpoint(const ntsa::Endpoint& endpoint);

    /// Set the endpoint to the specified 'endpoint'.
    void setEndpoint(const bdlb::NullableValue<ntsa::Endpoint>& endpoint);

    /// Set the data to the specified 'data'.
    void setData(const bsl::shared_ptr<bdlbb::Blob>& data);

    /// Set the length of the data to the specified 'length'.
    void setLength(bsl::size_t length);

    /// Set the timestamp to the specified 'timestamp'.
    void setTimestamp(bsl::int64_t timestamp);

    /// Return the endpoint.
    const bdlb::NullableValue<ntsa::Endpoint>& endpoint() const;

    /// Return the data.
    const bsl::shared_ptr<bdlbb::Blob>& data() const;

    /// Return the length of the data.
    bsl::size_t length() const;

    /// Return the timestamp, in nanoseconds since an arbitrary but
    /// consistent epoch.
    bsl::int64_t timestamp() const;

    /// Return the duration from the timestamp until now.
    bsls::TimeInterval delay() const;
};

/// @internal @brief
/// Provide a mechanism to advise how much data should be attempted to be
/// copied from a receive buffer.
///
/// @details
/// Provide a mechanism to gauge the amount of data that should
/// be copied from a socket receive buffer. The amount is automatically
/// adjusted according to an additive-increase/multiplicative-decrease
/// (AIMD) feedback control algorithm.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntdcq
class ReceiveFeedback
{
    enum {
        k_INCREASE_FACTOR = 8192,
        // The additive increase factor when "congestion" is detected,
        // e.g., when the amount of data received is approximately equal to
        // the amount of data receivable, within some threshold.

        k_DECREASE_FACTOR = 2,
        // The multiplicative decrease factor when "congestion" is *not*
        // detected, e.g., when the amount of data received is less than
        // the amount of data receivable, beyond some threshold.

        k_THRESHOLD = 90
        // The percentage of the amount of data receivable, when compared
        // with amount of data received, above which the receiver is
        // considered to be "congesting" the network.
    };

    bsl::size_t d_minimum;
    bsl::size_t d_current;
    bsl::size_t d_maximum;
    bsl::size_t d_increaseFactor;
    bsl::size_t d_decreaseFactor;
    bsl::size_t d_count;

  private:
    ReceiveFeedback(const ReceiveFeedback&) BSLS_KEYWORD_DELETED;
    ReceiveFeedback& operator=(const ReceiveFeedback&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new receive buffer feedback control mechanism that adjusts
    /// the amount of data that should be copied from a socket receive
    /// buffer from the default minimum and maximum.
    ReceiveFeedback();

    /// Create a new receive buffer feedback control mechanism that adjusts
    /// the amount of data that should be copied from a socket receive
    /// buffer from the specified 'minimum' to the specified 'maximum',
    /// inclusive.
    ReceiveFeedback(bsl::size_t minimum, bsl::size_t maximum);

    /// Destroy this object.
    ~ReceiveFeedback();

    /// Set the minimum amount of data that should be attempted to be
    /// copied from the receive buffer to the specified 'minimum'.
    void setMinimum(bsl::size_t minimum);

    /// Set the maximum amount of data that should be attempted to be
    /// copied from the receive buffer to the specified 'minimum'.
    void setMaximum(bsl::size_t maximum);

    /// Set the additive increase factor when "congestion" is detected,
    /// e.g., when the amount of data received is approximately equal to
    /// the amount of data receivable, within some threshold, to the
    /// specified 'increaseFactor'.
    void setIncreaseFactor(bsl::size_t increaseFactor);

    /// Set the multiplicative decrease factor when "congestion" is *not*
    /// detected, e.g., when the amount of data received is less than the
    /// amount of data receivable, beyond some threshold, to the specified
    /// 'decreaseFactor'.
    void setDecreaseFactor(bsl::size_t decreaseFactor);

    /// Set whether "congestion" has been detected according to the
    /// specified 'congestion'. When congestion is detected (e.g., when the
    /// amount of data received is approximately equal to the amount of data
    /// receivable, within some threshold), the amount of data to receive is
    /// increased. Otherwise, the amount of data to receive is decreased.
    void setCongestion(bool congestion);

    /// Set the feedback into the control mechanism according to the
    /// specified 'numBytesReceivable' and 'numBytesReceived' from the last
    ///  receive operation. If 'numBytesReceived' approximately equals
    /// 'numBytesReceivable', within some threshold, "congestion" is
    /// detected. When "congestion" is detected, the amount of data to
    /// receive is increased. Otherwise, the amount of data to receive is
    /// decreased.
    void setFeedback(bsl::size_t numBytesReceivable,
                     bsl::size_t numBytesReceived);

    /// Return the minimum number of bytes suggested to receive.
    bsl::size_t minimum() const;

    /// Return the current number of bytes suggested to receive.
    bsl::size_t current() const;

    /// Return the maximum number of bytes suggested to receive.
    bsl::size_t maximum() const;

    /// Return the additive increase factor when "congestion" is detected,
    /// e.g., when the amount of data received is approximately equal to
    /// the amount of data receivable, within some threshold.
    bsl::size_t increaseFactor() const;

    /// Return the multiplicative decrease factor when "congestion" is *not*
    /// detected, e.g., when the amount of data received is less than the
    /// amount of data receivable, beyond some threshold.
    bsl::size_t decreaseFactor() const;
};

/// @internal @brief
/// Provide a receive queue.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcq
class ReceiveQueue
{
    /// This typedef defines a linked list of structures that describe
    /// the read queue.
    typedef bsl::list<ReceiveQueueEntry> EntryList;

    EntryList                    d_entryList;
    bsl::shared_ptr<bdlbb::Blob> d_data_sp;
    bsl::size_t                  d_size;
    bsl::size_t                  d_watermarkLow;
    bool                         d_watermarkLowWanted;
    bsl::size_t                  d_watermarkHigh;
    bool                         d_watermarkHighWanted;
    ReceiveCallbackQueue         d_callbackQueue;
    bslma::Allocator*            d_allocator_p;

  private:
    ReceiveQueue(const ReceiveQueue&) BSLS_KEYWORD_DELETED;
    ReceiveQueue& operator=(const ReceiveQueue&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new receive from message queue. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit ReceiveQueue(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ReceiveQueue();

    /// Push the specified 'entry' onto the queue. Return true if queue
    /// becomes non-empty as a result of this operation, otherwise return
    /// false.
    bool pushEntry(const ReceiveQueueEntry& entry);

    /// Return a reference to the modifiable entry at the front of the
    /// queue.
    ReceiveQueueEntry& frontEntry();

    /// Pop the entry off the front of the queue. Return true if the
    /// queue becomes empty as a result of the operation, otherwise return
    /// false.
    bool popEntry();

    /// Pop the specified 'numBytes' from the entry at the front of the
    /// queue.
    void popSize(bsl::size_t numBytes);

    /// Return a shared pointer to a new receive callback queue entry.
    bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> createCallbackEntry();

    /// Push the specified 'callbackEntry' onto the callback queue. Return
    /// the error.
    ntsa::Error pushCallbackEntry(
        const bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>& callbackEntry);

    /// Pop the callback entry at the front of the queue if its criteria is
    /// satisfied by the current number of bytes available in the queue and
    /// load it into the specified 'result'. Return the error.
    ntsa::Error popCallbackEntry(
        bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>* result);

    /// Pop all callback entries into the specified 'result'.
    void popAllCallbackEntries(
        bsl::vector<bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> >*
            result);

    /// Remove the specified 'callbackEntry' from the queue, if found.
    /// Return the error.
    ntsa::Error removeCallbackEntry(
        const bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>& callbackEntry);

    /// Remove the entry having the specified 'token' from the queue, if
    /// found, and load it into the specified 'result'. Return the error.
    ntsa::Error removeCallbackEntry(
        bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>* result,
        const ntca::ReceiveToken&                         token);

    /// Set the data stored in the queue to the specified 'data'.
    void setData(const bsl::shared_ptr<bdlbb::Blob>& data);

    /// Set the low watermark to the specified 'lowWatermark'.
    void setLowWatermark(bsl::size_t lowWatermark);

    /// Set the high watermark to the specified 'highWatermark'.
    void setHighWatermark(bsl::size_t highWatermark);

    /// Return true if the queue has been filled to greater than or equal
    /// to the low watermark, otherwise return false.
    bool authorizeLowWatermarkEvent();

    /// Return true if the queue has been filled to greater than the high
    /// watermark, otherwise return false.
    bool authorizeHighWatermarkEvent();

    /// Return the data stored in the queue.
    const bsl::shared_ptr<bdlbb::Blob>& data() const;

    /// Return the low watermark.
    bsl::size_t lowWatermark() const;

    /// Return the high watermark.
    bsl::size_t highWatermark() const;

    /// Return the number of bytes on the queue.
    bsl::size_t size() const;

    /// Return true if there are entries on the queue, and false otherwise.
    /// Note that the queue may have entries but still have a zero size
    /// when the sole remaining entry is a shutdown entry.
    bool hasEntry() const;

    /// Return true if there are callback entries on the queue, and
    /// false otherwise.
    bool hasCallbackEntry() const;

    /// Return true if the low watermark is satisfied, otherwise return
    /// false.
    bool isLowWatermarkSatisfied() const;

    /// Return true if the high watermark is violated, otherwise return
    /// false.
    bool isHighWatermarkViolated() const;

    /// Return the read queue context.
    ntca::ReadQueueContext context() const;
};

NTCCFG_INLINE
void ReceiveCallbackQueueEntry::assign(const ntci::ReceiveCallback& callback,
                                       const ntca::ReceiveOptions&  options)
{
    d_callback = callback;
    d_options  = options;
}

NTCCFG_INLINE
void ReceiveCallbackQueueEntry::setTimer(
    const bsl::shared_ptr<ntci::Timer>& timer)
{
    d_timer_sp = timer;
}

NTCCFG_INLINE
void ReceiveCallbackQueueEntry::closeTimer()
{
    if (d_timer_sp) {
        d_timer_sp->close();
        d_timer_sp.reset();
    }
}

NTCCFG_INLINE
const ntca::ReceiveOptions& ReceiveCallbackQueueEntry::options() const
{
    return d_options;
}

NTCCFG_INLINE
bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> ReceiveCallbackQueueEntryPool::
    create()
{
    return d_pool.getObject();
}

NTCCFG_INLINE
bsl::size_t ReceiveCallbackQueueEntryPool::numObjects() const
{
    return d_pool.numObjects();
}

NTCCFG_INLINE
bsl::size_t ReceiveCallbackQueueEntryPool::numObjectsAvailable() const
{
    return d_pool.numAvailableObjects();
}

NTCCFG_INLINE
bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> ReceiveCallbackQueue::create()
{
    return d_entryPool.create();
}

NTCCFG_INLINE
ntsa::Error ReceiveCallbackQueue::push(
    const bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>& entry)
{
    d_entryList.push_back(entry);
    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error ReceiveCallbackQueue::pop(
    bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>* result,
    bsl::size_t                                       numBytesAvailable)
{
    if (!d_entryList.empty()) {
        if (numBytesAvailable >= d_entryList.front()->options().minSize()) {
            *result = d_entryList.front();
            d_entryList.pop_front();
            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }
    }
    else {
        return ntsa::Error::invalid();
    }
}

NTCCFG_INLINE
ntsa::Error ReceiveCallbackQueue::remove(
    const bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>& entry)
{
    for (EntryList::iterator it = d_entryList.begin(); it != d_entryList.end();
         ++it)
    {
        const bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>& target = *it;
        if (entry == target) {
            d_entryList.erase(it);
            return ntsa::Error();
        }
    }

    return ntsa::Error(ntsa::Error::e_EOF);
}

NTCCFG_INLINE
ntsa::Error ReceiveCallbackQueue::remove(
    bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>* result,
    const ntca::ReceiveToken&                         token)
{
    result->reset();

    for (EntryList::iterator it = d_entryList.begin(); it != d_entryList.end();
         ++it)
    {
        const bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>& entry = *it;

        if (!entry->options().token().isNull()) {
            if (entry->options().token().value() == token) {
                *result = entry;
                d_entryList.erase(it);
                return ntsa::Error();
            }
        }
    }

    return ntsa::Error(ntsa::Error::e_EOF);
}

NTCCFG_INLINE
void ReceiveCallbackQueue::removeAll(
    bsl::vector<bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> >* result)
{
    result->insert(result->end(), d_entryList.begin(), d_entryList.end());
    d_entryList.clear();
}

NTCCFG_INLINE
bsl::size_t ReceiveCallbackQueue::size() const
{
    return d_entryList.size();
}

NTCCFG_INLINE
bool ReceiveCallbackQueue::empty() const
{
    return d_entryList.empty();
}

NTCCFG_INLINE
ReceiveQueueEntry::ReceiveQueueEntry()
: d_endpoint()
, d_data_sp()
, d_length(0)
, d_timestamp(0)
{
}

NTCCFG_INLINE
void ReceiveQueueEntry::setEndpoint(const ntsa::Endpoint& endpoint)
{
    d_endpoint = endpoint;
}

NTCCFG_INLINE
void ReceiveQueueEntry::setEndpoint(
    const bdlb::NullableValue<ntsa::Endpoint>& endpoint)
{
    d_endpoint = endpoint;
}

NTCCFG_INLINE
void ReceiveQueueEntry::setData(const bsl::shared_ptr<bdlbb::Blob>& data)
{
    d_data_sp = data;
}

NTCCFG_INLINE
void ReceiveQueueEntry::setLength(bsl::size_t length)
{
    d_length = length;
}

NTCCFG_INLINE
void ReceiveQueueEntry::setTimestamp(bsl::int64_t timestamp)
{
    d_timestamp = timestamp;
}

NTCCFG_INLINE
const bdlb::NullableValue<ntsa::Endpoint>& ReceiveQueueEntry::endpoint() const
{
    return d_endpoint;
}

NTCCFG_INLINE
const bsl::shared_ptr<bdlbb::Blob>& ReceiveQueueEntry::data() const
{
    return d_data_sp;
}

NTCCFG_INLINE
bsl::size_t ReceiveQueueEntry::length() const
{
    return d_length;
}

NTCCFG_INLINE
bsl::int64_t ReceiveQueueEntry::timestamp() const
{
    return d_timestamp;
}

NTCCFG_INLINE
bsls::TimeInterval ReceiveQueueEntry::delay() const
{
    bsl::int64_t delayInNanoseconds = bsls::TimeUtil::getTimer() - d_timestamp;

    if (delayInNanoseconds < 0) {
        delayInNanoseconds = 0;
    }

    bsls::TimeInterval delay;
    delay.setTotalNanoseconds(delayInNanoseconds);

    return delay;
}

NTCCFG_INLINE
ReceiveFeedback::ReceiveFeedback()
: d_minimum(NTCCFG_DEFAULT_STREAM_SOCKET_MIN_INCOMING_TRANSFER_SIZE)
, d_current(NTCCFG_DEFAULT_STREAM_SOCKET_MIN_INCOMING_TRANSFER_SIZE)
, d_maximum(NTCCFG_DEFAULT_STREAM_SOCKET_MAX_INCOMING_TRANSFER_SIZE)
, d_increaseFactor(k_INCREASE_FACTOR)
, d_decreaseFactor(k_DECREASE_FACTOR)
, d_count(0)
{
}

NTCCFG_INLINE
ReceiveFeedback::ReceiveFeedback(bsl::size_t minimum, bsl::size_t maximum)
: d_minimum(minimum)
, d_current(minimum)
, d_maximum(maximum)
, d_increaseFactor(k_INCREASE_FACTOR)
, d_decreaseFactor(k_DECREASE_FACTOR)
, d_count(0)
{
}

NTCCFG_INLINE
ReceiveFeedback::~ReceiveFeedback()
{
}

NTCCFG_INLINE
void ReceiveFeedback::setMinimum(bsl::size_t minimum)
{
    d_minimum = minimum;
}

NTCCFG_INLINE
void ReceiveFeedback::setMaximum(bsl::size_t maximum)
{
    d_maximum = maximum;
}

NTCCFG_INLINE
void ReceiveFeedback::setIncreaseFactor(bsl::size_t increaseFactor)
{
    d_increaseFactor = increaseFactor;
}

NTCCFG_INLINE
void ReceiveFeedback::setDecreaseFactor(bsl::size_t decreaseFactor)
{
    d_decreaseFactor = decreaseFactor;
}

NTCCFG_INLINE
void ReceiveFeedback::setCongestion(bool congestion)
{
    if (congestion) {
        if (d_current < d_maximum) {
            d_current = d_current + d_increaseFactor;
            if (d_current > d_maximum) {
                d_current = d_maximum;
            }
        }
    }
    else {
        if (++d_count < 10) {
            return;
        }
        d_count = 0;

        if (d_current > d_minimum) {
            d_current = d_current / d_decreaseFactor;
            if (d_current < d_minimum) {
                d_current = d_minimum;
            }
        }
    }
}

NTCCFG_INLINE
void ReceiveFeedback::setFeedback(bsl::size_t numBytesReceivable,
                                  bsl::size_t numBytesReceived)
{
    if (numBytesReceived > numBytesReceivable) {
        return;
    }

    const bsl::size_t threshold = (numBytesReceivable * k_THRESHOLD) / 100;

    if (numBytesReceived >= threshold) {
        this->setCongestion(true);
    }
    else {
        this->setCongestion(false);
    }
}

NTCCFG_INLINE
bsl::size_t ReceiveFeedback::minimum() const
{
    return d_minimum;
}

NTCCFG_INLINE
bsl::size_t ReceiveFeedback::current() const
{
    return d_current;
}

NTCCFG_INLINE
bsl::size_t ReceiveFeedback::maximum() const
{
    return d_maximum;
}

NTCCFG_INLINE
bsl::size_t ReceiveFeedback::increaseFactor() const
{
    return d_increaseFactor;
}

NTCCFG_INLINE
bsl::size_t ReceiveFeedback::decreaseFactor() const
{
    return d_decreaseFactor;
}

NTCCFG_INLINE
bool ReceiveQueue::pushEntry(const ReceiveQueueEntry& entry)
{
    d_entryList.push_back(entry);

    BSLS_ASSERT(entry.length() > 0);
    d_size += entry.length();

    return d_entryList.size() == 1;
}

NTCCFG_INLINE
ReceiveQueueEntry& ReceiveQueue::frontEntry()
{
    return d_entryList.front();
}

NTCCFG_INLINE
bool ReceiveQueue::popEntry()
{
    {
        ReceiveQueueEntry& entry = d_entryList.front();

        BSLS_ASSERT(entry.length() > 0);
        BSLS_ASSERT(d_size >= entry.length());
        d_size -= entry.length();
    }

    if (d_size < d_watermarkLow) {
        d_watermarkLowWanted  = true;
        d_watermarkHighWanted = true;
    }

    d_entryList.pop_front();
    return d_entryList.empty();
}

NTCCFG_INLINE
void ReceiveQueue::popSize(bsl::size_t numBytes)
{
    BSLS_ASSERT(!d_entryList.empty());

    ReceiveQueueEntry& entry = d_entryList.front();

    BSLS_ASSERT(entry.length() >= numBytes);
    entry.setLength(entry.length() - numBytes);

    BSLS_ASSERT(d_size >= numBytes);
    d_size -= numBytes;

    if (d_size < d_watermarkLow) {
        d_watermarkLowWanted  = true;
        d_watermarkHighWanted = true;
    }
}

NTCCFG_INLINE
bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> ReceiveQueue::
    createCallbackEntry()
{
    return d_callbackQueue.create();
}

NTCCFG_INLINE
ntsa::Error ReceiveQueue::pushCallbackEntry(
    const bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>& callbackEntry)
{
    return d_callbackQueue.push(callbackEntry);
}

NTCCFG_INLINE
ntsa::Error ReceiveQueue::popCallbackEntry(
    bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>* result)
{
    if (d_entryList.empty()) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    return d_callbackQueue.pop(result, d_size);
}

NTCCFG_INLINE
void ReceiveQueue::popAllCallbackEntries(
    bsl::vector<bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry> >* result)
{
    d_callbackQueue.removeAll(result);
}

NTCCFG_INLINE
ntsa::Error ReceiveQueue::removeCallbackEntry(
    const bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>& callbackEntry)
{
    return d_callbackQueue.remove(callbackEntry);
}

NTCCFG_INLINE
ntsa::Error ReceiveQueue::removeCallbackEntry(
    bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>* result,
    const ntca::ReceiveToken&                         token)
{
    return d_callbackQueue.remove(result, token);
}

NTCCFG_INLINE
void ReceiveQueue::setData(const bsl::shared_ptr<bdlbb::Blob>& data)
{
    d_data_sp = data;
}

NTCCFG_INLINE
void ReceiveQueue::setLowWatermark(bsl::size_t lowWatermark)
{
    d_watermarkLow       = lowWatermark;
    d_watermarkLowWanted = true;

    ntcs::WatermarkUtil::sanitizeIncomingQueueWatermarks(&d_watermarkLow,
                                                         &d_watermarkHigh);
}

NTCCFG_INLINE
void ReceiveQueue::setHighWatermark(bsl::size_t highWatermark)
{
    d_watermarkHigh = highWatermark;

    ntcs::WatermarkUtil::sanitizeIncomingQueueWatermarks(&d_watermarkLow,
                                                         &d_watermarkHigh);
}

NTCCFG_INLINE
bool ReceiveQueue::authorizeLowWatermarkEvent()
{
    if (ntcs::WatermarkUtil::isIncomingQueueLowWatermarkSatisfied(
            d_size,
            d_watermarkLow))
    {
        if (d_watermarkLowWanted) {
            d_watermarkLowWanted = false;
            return true;
        }
    }

    return false;
}

NTCCFG_INLINE
bool ReceiveQueue::authorizeHighWatermarkEvent()
{
    if (ntcs::WatermarkUtil::isIncomingQueueHighWatermarkViolated(
            d_size,
            d_watermarkHigh))
    {
        if (d_watermarkHighWanted) {
            d_watermarkHighWanted = false;
            return true;
        }
    }

    return false;
}

NTCCFG_INLINE
const bsl::shared_ptr<bdlbb::Blob>& ReceiveQueue::data() const
{
    return d_data_sp;
}

NTCCFG_INLINE
bsl::size_t ReceiveQueue::lowWatermark() const
{
    return d_watermarkLow;
}

NTCCFG_INLINE
bsl::size_t ReceiveQueue::highWatermark() const
{
    return d_watermarkHigh;
}

NTCCFG_INLINE
bsl::size_t ReceiveQueue::size() const
{
    return d_size;
}

NTCCFG_INLINE
bool ReceiveQueue::hasEntry() const
{
    return !d_entryList.empty();
}

NTCCFG_INLINE
bool ReceiveQueue::hasCallbackEntry() const
{
    return !d_callbackQueue.empty();
}

NTCCFG_INLINE
bool ReceiveQueue::isLowWatermarkSatisfied() const
{
    return ntcs::WatermarkUtil::isIncomingQueueLowWatermarkSatisfied(
        d_size,
        d_watermarkLow);
}

NTCCFG_INLINE
bool ReceiveQueue::isHighWatermarkViolated() const
{
    return ntcs::WatermarkUtil::isIncomingQueueHighWatermarkViolated(
        d_size,
        d_watermarkHigh);
}

NTCCFG_INLINE
ntca::ReadQueueContext ReceiveQueue::context() const
{
    ntca::ReadQueueContext context;

    context.setSize(d_size);
    context.setLowWatermark(d_watermarkLow);
    context.setHighWatermark(d_watermarkHigh);

    return context;
}

}  // close package namespace
}  // close enterprise namespace
#endif
