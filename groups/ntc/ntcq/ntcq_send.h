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

#ifndef INCLUDED_NTCQ_SEND
#define INCLUDED_NTCQ_SEND

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_sendoptions.h>
#include <ntca_writequeuecontext.h>
#include <ntccfg_platform.h>
#include <ntci_log.h>
#include <ntci_sendcallback.h>
#include <ntci_sender.h>
#include <ntci_strand.h>
#include <ntci_timer.h>
#include <ntcs_callbackstate.h>
#include <ntcs_watermarkutil.h>
#include <ntcscm_version.h>
#include <ntsa_data.h>
#include <ntsa_error.h>
#include <ntsa_sendoptions.h>
#include <bdlb_nullablevalue.h>
#include <bdlcc_sharedobjectpool.h>
#include <bsls_timeinterval.h>
#include <bsls_timeutil.h>
#include <bsl_functional.h>
#include <bsl_list.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcq {

/// @internal @brief
/// Describe an entry in a send callback queue.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcq
class SendCallbackQueueEntry
{
    ntccfg::Object               d_object;
    ntcs::CallbackState          d_state;
    ntci::SendCallback           d_callback;
    ntca::SendOptions            d_options;
    bsl::shared_ptr<ntci::Timer> d_timer_sp;

  private:
    SendCallbackQueueEntry(const SendCallbackQueueEntry&) BSLS_KEYWORD_DELETED;
    SendCallbackQueueEntry& operator=(const SendCallbackQueueEntry&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new send callback queue entry. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit SendCallbackQueueEntry(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~SendCallbackQueueEntry();

    /// Clear the state of this entry.
    void clear();

    /// Assign the specified 'callback' to be invoked on the specified
    /// 'strand'.
    void assign(const ntci::SendCallback& callback,
                const ntca::SendOptions&  options);

    /// Set the timer to the specified 'timer'.
    void setTimer(const bsl::shared_ptr<ntci::Timer>& timer);

    /// Clear the timer, if any.
    void clearTimer();

    /// Return the criteria to invoke the callback.
    const ntca::SendOptions& options() const;

    /// Invoke the callback of the specified 'entry' for the specified
    /// 'sender' and 'event'. If the specified 'defer' flag is false
    /// and the requirements of the strand of the specified 'entry' permits
    /// the callback to be invoked immediately by the 'strand', unlock the
    /// specified 'mutex', invoke the callback, then relock the 'mutex'.
    /// Otherwise, enqueue the invocation of the callback to be executed on
    /// the strand of the 'entry', if defined, or by the specified
    /// 'executor' otherwise.
    static void dispatch(
        const bsl::shared_ptr<ntcq::SendCallbackQueueEntry>& entry,
        const bsl::shared_ptr<ntci::Sender>&                 sender,
        const ntca::SendEvent&                               event,
        const bsl::shared_ptr<ntci::Strand>&                 strand,
        const bsl::shared_ptr<ntci::Executor>&               executor,
        bool                                                 defer,
        bslmt::Mutex*                                        mutex);

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(SendCallbackQueueEntry);
};

/// @internal @brief
/// Provide a pool of shared pointers to send callback queue entries.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcq
class SendCallbackQueueEntryPool
{
    typedef bdlcc::SharedObjectPool<
        ntcq::SendCallbackQueueEntry,
        bdlcc::ObjectPoolFunctors::DefaultCreator,
        bdlcc::ObjectPoolFunctors::Clear<ntcq::SendCallbackQueueEntry> >
        Pool;

    Pool d_pool;

  private:
    SendCallbackQueueEntryPool(const SendCallbackQueueEntryPool&)
        BSLS_KEYWORD_DELETED;
    SendCallbackQueueEntryPool& operator=(const SendCallbackQueueEntryPool&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Construct a new send callback queue entry at the specified
    /// 'address' using the specified 'allocator' to supply memory.
    static void construct(void* address, bslma::Allocator* allocator);

  public:
    /// Create a new object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit SendCallbackQueueEntryPool(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~SendCallbackQueueEntryPool();

    /// Return a shared pointer to an send callback queue entry in the
    /// pool having a default value. The resulting send callback queue
    /// entry is automatically returned to this pool when its reference
    /// count reaches zero.
    bsl::shared_ptr<ntcq::SendCallbackQueueEntry> create();

    /// Return the total number of objects in the pool.
    bsl::size_t numObjects() const;

    /// Return the number of un-allocated objects available in the pool.
    bsl::size_t numObjectsAvailable() const;
};

/// @internal @brief
/// Describe an entry on a send queue.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcq
class SendQueueEntry
{
    bsl::uint64_t                           d_id;
    bdlb::NullableValue<ntca::SendToken>    d_token;
    bdlb::NullableValue<ntsa::Endpoint>     d_endpoint;
    bsl::shared_ptr<ntsa::Data>             d_data_sp;
    bsl::size_t                             d_length;
    bsl::int64_t                            d_timestamp;
    bdlb::NullableValue<bsls::TimeInterval> d_deadline;
    bsl::shared_ptr<ntci::Timer>            d_timer_sp;
    bsl::shared_ptr<SendCallbackQueueEntry> d_callbackEntry_sp;
    bsl::uint32_t                           d_zeroCopyId;
    bool                                    d_inProgress;

  private:
    /// If this entry is batchable, append a reference to this data of this
    /// entry represented as the specified 'blob' to the specified 'result'
    /// according to the specified 'options'. Return true if more entries
    /// should be attempted to be batched, and false otherwise.
    bool batchNext(ntsa::ConstBufferArray*  result,
                   const bdlbb::Blob&       blob,
                   const ntsa::SendOptions& options) const;

    /// If this entry is batchable, append a reference to this data of this
    /// entry represented as the specified 'blobBuffer' to the specified
    /// 'result' according to the specified 'options'. Return true if more
    /// entries should be attempted to be batched, and false otherwise.
    bool batchNext(ntsa::ConstBufferArray*  result,
                   const bdlbb::BlobBuffer& blobBuffer,
                   const ntsa::SendOptions& options) const;

    /// If this entry is batchable, append a reference to this data of this
    /// entry represented as the specified 'constBuffer' to the specified
    /// 'result' according to the specified 'options'. Return true if more
    /// entries should be attempted to be batched, and false otherwise.
    bool batchNext(ntsa::ConstBufferArray*  result,
                   const ntsa::ConstBuffer& constBuffer,
                   const ntsa::SendOptions& options) const;

    /// If this entry is batchable, append a reference to this data of this
    /// entry represented as the specified 'constBufferArray' to the specified
    /// 'result' according to the specified 'options'. Return true if more
    /// entries should be attempted to be batched, and false otherwise.
    bool batchNext(ntsa::ConstBufferArray*       result,
                   const ntsa::ConstBufferArray& constBufferArray,
                   const ntsa::SendOptions&      options) const;

    /// If this entry is batchable, append a reference to this data of this
    /// entry represented as the specified 'constBufferPtrArray' to the
    /// specified 'result' according to the specified 'options'. Return true if
    /// more entries should be attempted to be batched, and false otherwise.
    bool batchNext(ntsa::ConstBufferArray*          result,
                   const ntsa::ConstBufferPtrArray& constBufferPtrArray,
                   const ntsa::SendOptions&         options) const;

    /// If this entry is batchable, append a reference to this data of this
    /// entry represented as the specified 'mutableBuffer' to the specified
    /// 'result' according to the specified 'options'. Return true if more
    /// entries should be attempted to be batched, and false otherwise.
    bool batchNext(ntsa::ConstBufferArray*    result,
                   const ntsa::MutableBuffer& mutableBuffer,
                   const ntsa::SendOptions&   options) const;

    /// If this entry is batchable, append a reference to this data of this
    /// entry represented as the specified 'mutableBufferArray' to the
    /// specified 'result' according to the specified 'options'. Return true if
    /// more entries should be attempted to be batched, and false otherwise.
    bool batchNext(ntsa::ConstBufferArray*         result,
                   const ntsa::MutableBufferArray& mutableBufferArray,
                   const ntsa::SendOptions&        options) const;

    /// If this entry is batchable, append a reference to this data of this
    /// entry represented as the specified 'mutableBufferPtrArray' to the
    /// specified 'result' according to the specified 'options'. Return true if
    /// more entries should be attempted to be batched, and false otherwise.
    bool batchNext(ntsa::ConstBufferArray*            result,
                   const ntsa::MutableBufferPtrArray& mutableBufferPtrArray,
                   const ntsa::SendOptions&           options) const;

    /// If this entry is batchable, append a reference to this data of this
    /// entry represented as the specified 'string' to the specified 'result'
    /// according to the specified 'options'. Return true if more entries
    /// should be attempted to be batched, and false otherwise.
    bool batchNext(ntsa::ConstBufferArray*  result,
                   const bsl::string&       string,
                   const ntsa::SendOptions& options) const;

  public:
    /// Create a new send queue entry.
    SendQueueEntry();

    /// Set the identifier used to internally time-out the queue entry to
    /// the specified 'id'.
    void setId(bsl::uint64_t id);

    /// Set the token used to cancel the queue entry to the specified
    /// 'token'.
    void setToken(const ntca::SendToken& token);

    /// Set the token used to cancel the queue entry to the specified
    /// 'token'.
    void setToken(const bdlb::NullableValue<ntca::SendToken>& token);

    /// Set the endpoint to the specified 'endpoint'.
    void setEndpoint(const ntsa::Endpoint& endpoint);

    /// Set the endpoint to the specified 'endpoint'.
    void setEndpoint(const bdlb::NullableValue<ntsa::Endpoint>& endpoint);

    /// Set the data to the specified 'data'.
    void setData(const bsl::shared_ptr<ntsa::Data>& data);

    /// Set the length of the data to the specified 'length'.
    void setLength(bsl::size_t length);

    /// Set the timestamp to the specified 'timestamp'.
    void setTimestamp(bsl::int64_t timestamp);

    /// Set the deadline within which the data must be sent to the
    /// specified 'value'.
    void setDeadline(const bsls::TimeInterval& value);

    /// Set the deadline within which the data must be sent to the
    /// specified 'value'.
    void setDeadline(const bdlb::NullableValue<bsls::TimeInterval>& value);

    /// Set the timer to the specified 'timer'.
    void setTimer(const bsl::shared_ptr<ntci::Timer>& timer);

    /// Set the callback entry to the specified 'callbackEntry'.
    void setCallbackEntry(
        const bsl::shared_ptr<SendCallbackQueueEntry>& callbackEntry);

    // Set entry's zeroCopyId to the secified 'value'.
    void setZeroCopyId(bsl::uint32_t value);

    /// Set the flag to indicate that the entry is now in-progress, i.e. its
    /// data has been at least partially copied to the send buffer.
    void setInProgress();

    /// Close the timer, if any.
    void closeTimer();

    /// If this entry is batchable, append a reference to this data of this
    /// entry to the specified 'result' according to the specified 'options'.
    /// Return true if more entries should be attempted to be batched, and
    /// false otherwise.
    bool batchNext(ntsa::ConstBufferArray*  result,
                   const ntsa::SendOptions& options) const;

    /// Return the identifier used to internally time-out the queue entry.
    bsl::uint64_t id() const;

    /// Return the token used to cancel the queue entry.
    const bdlb::NullableValue<ntca::SendToken>& token() const;

    /// Return the endpoint.
    const bdlb::NullableValue<ntsa::Endpoint>& endpoint() const;

    /// Return the data.
    const bsl::shared_ptr<ntsa::Data>& data() const;

    /// Return the length of the data.
    bsl::size_t length() const;

    /// Return the timestamp, in nanoseconds since an arbitrary but
    /// consistent epoch.
    bsl::int64_t timestamp() const;

    /// Return the deadline within which the data must be sent.
    const bdlb::NullableValue<bsls::TimeInterval>& deadline() const;

    /// Return the duration from the timestamp until now.
    bsls::TimeInterval delay() const;

    /// Return the callback entry.
    const bsl::shared_ptr<SendCallbackQueueEntry>& callbackEntry() const;

    /// Return the identifier used to correlate the entry with MSG_ZEROCOPY
    /// notification from the socket error queue.
    bsl::uint32_t zeroCopyId() const;

    /// Return the flag that indicates whether the entry is now in-progress,
    /// i.e. its data has been at least partially copied to the send buffer.
    bool inProgress() const;

    /// Return the flag that indicates the data representation of this entry
    /// is batchable with other similar representations.
    bool isBatchable() const;
};

/// @internal @brief
/// Provide a send queue.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcq
class SendQueue
{
    /// This typedef defines a linked list of structures that describe
    /// the write queue.
    typedef bsl::list<SendQueueEntry> EntryList;

    EntryList                        d_entryList;
    EntryList                        d_zeroCopyWaitList;
    bsl::shared_ptr<bdlbb::Blob>     d_data_sp;
    bsl::size_t                      d_size;
    bsl::size_t                      d_watermarkLow;
    bool                             d_watermarkLowWanted;
    bsl::size_t                      d_watermarkHigh;
    bool                             d_watermarkHighWanted;
    bsl::uint64_t                    d_nextEntryId;
    bsl::uint32_t                    d_zeroCopyId;
    ntcq::SendCallbackQueueEntryPool d_callbackEntryPool;
    bslma::Allocator*                d_allocator_p;

  private:
    SendQueue(const SendQueue&) BSLS_KEYWORD_DELETED;
    SendQueue& operator=(const SendQueue&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new send to message queue. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit SendQueue(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~SendQueue();

    /// Return a shared pointer to a new send callback queue entry.
    bsl::shared_ptr<ntcq::SendCallbackQueueEntry> createCallbackEntry();

    /// Return the next entry identifier.
    bsl::uint64_t generateEntryId();

    /// Push the specified 'entry' onto the queue. Return true if queue
    /// becomes non-empty as a result of this operation, otherwise return
    /// false.
    bool pushEntry(const SendQueueEntry& entry);

    /// Return a reference to the modifiable entry at the front of the
    /// queue.
    SendQueueEntry& frontEntry();

    /// Pop the entry off the front of the queue. Return true if the
    /// queue becomes empty as a result of the operation, otherwise return
    /// false.
    bool popEntry();

    /// Pop the specified 'numBytes' from the entry at the front of the
    /// queue.
    void popSize(bsl::size_t numBytes);

    /// Remove the entry having the specified 'id' and load its callback
    /// entry into the specified 'result', if an entry with such an 'id' and
    /// defined callback and defined deadline exists and has not already
    /// had any portion of its data copied to the socket send buffer. Return
    /// true if queue becomes empty as a result of this operation, otherwise
    /// return false.
    bool removeEntryId(bsl::shared_ptr<ntcq::SendCallbackQueueEntry>* result,
                       bsl::uint64_t                                  id);

    /// Remove the entry having the specified 'token' and load its callback
    /// entry into the specified 'result', if an entry with such a 'token'
    /// and defined callback exists and has not already had any portion of
    /// its data copied to the socket send buffer. Return true if queue
    /// becomes empty as a result of this operation, otherwise return false.
    bool removeEntryToken(
        bsl::shared_ptr<ntcq::SendCallbackQueueEntry>* result,
        const ntca::SendToken&                         token);

    /// Load into the specified 'result' any pending callback entries and
    /// clear the queue. Return true if the queue was non-empty, and false
    /// otherwise.
    bool removeAll(
        bsl::vector<bsl::shared_ptr<ntcq::SendCallbackQueueEntry> >* result);

    /// Set the data stored in the queue to the specified 'data'.
    void setData(const bsl::shared_ptr<bdlbb::Blob>& data);

    /// Set the low watermark to the specified 'lowWatermark'.
    void setLowWatermark(bsl::size_t lowWatermark);

    /// Set the high watermark to the specified 'highWatermark'.
    void setHighWatermark(bsl::size_t highWatermark);

    /// Return true if the queue has been drained down to the low watermark
    /// after first breaching the high watermark. otherwise return false.
    bool authorizeLowWatermarkEvent();

    /// Return true if the queue has either initially breached the high
    /// watermark or breached the high watermark again after being drained
    /// down to the low watermark.
    bool authorizeHighWatermarkEvent();

    /// Return true if the queue has either initially breached the high
    /// watermark or breached the high watermark again after being drained
    /// down to the low watermark, according to the
    /// 'effectiveHighWatermark'.
    bool authorizeHighWatermarkEvent(bsl::size_t effectiveHighWatermark);

    /// Batch together the next range of contiguous entries whose data may be
    /// attempted to be copied to the socket send buffer all at once. Limit
    /// the number of bytes and buffers according to the specified 'options'.
    /// Load into the specified 'result' the representation of each batched
    /// entry. Return true if batching is possible, and false otherwise.
    bool batchNext(ntsa::ConstBufferArray*  result,
                   const ntsa::SendOptions& options) const;

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

    /// Return true if the low watermark is satisfied, otherwise return
    /// false.
    bool isLowWatermarkSatisfied() const;

    /// Return true if the high watermark is violated, otherwise return
    /// false.
    bool isHighWatermarkViolated() const;

    /// Return true if the high watermark is violated according to the
    /// specified 'effectiveHighWatermark', otherwise return false.
    bool isHighWatermarkViolated(bsl::size_t effectiveHighWatermark) const;

    /// Return the write queue context.
    ntca::WriteQueueContext context() const;
};

NTCCFG_INLINE
void SendCallbackQueueEntry::assign(const ntci::SendCallback& callback,
                                    const ntca::SendOptions&  options)
{
    d_callback = callback;
    d_options  = options;
}

NTCCFG_INLINE
void SendCallbackQueueEntry::setTimer(
    const bsl::shared_ptr<ntci::Timer>& timer)
{
    d_timer_sp = timer;
}

NTCCFG_INLINE
void SendCallbackQueueEntry::clearTimer()
{
    if (d_timer_sp) {
        d_timer_sp.reset();
    }
}

NTCCFG_INLINE
const ntca::SendOptions& SendCallbackQueueEntry::options() const
{
    return d_options;
}

NTCCFG_INLINE
bsl::shared_ptr<ntcq::SendCallbackQueueEntry> SendCallbackQueueEntryPool::
    create()
{
    return d_pool.getObject();
}

NTCCFG_INLINE
bsl::size_t SendCallbackQueueEntryPool::numObjects() const
{
    return d_pool.numObjects();
}

NTCCFG_INLINE
bsl::size_t SendCallbackQueueEntryPool::numObjectsAvailable() const
{
    return d_pool.numAvailableObjects();
}

NTCCFG_INLINE
SendQueueEntry::SendQueueEntry()
: d_id(0)
, d_token()
, d_endpoint()
, d_data_sp()
, d_length(0)
, d_timestamp(0)
, d_deadline()
, d_timer_sp()
, d_callbackEntry_sp()
, d_zeroCopyId(0)
, d_inProgress(false)
{
}

NTCCFG_INLINE
void SendQueueEntry::setId(bsl::uint64_t id)
{
    d_id = id;
}

NTCCFG_INLINE
void SendQueueEntry::setToken(const ntca::SendToken& token)
{
    d_token = token;
}

NTCCFG_INLINE
void SendQueueEntry::setToken(
    const bdlb::NullableValue<ntca::SendToken>& token)
{
    d_token = token;
}

NTCCFG_INLINE
void SendQueueEntry::setEndpoint(const ntsa::Endpoint& endpoint)
{
    d_endpoint = endpoint;
}

NTCCFG_INLINE
void SendQueueEntry::setEndpoint(
    const bdlb::NullableValue<ntsa::Endpoint>& endpoint)
{
    d_endpoint = endpoint;
}

NTCCFG_INLINE
void SendQueueEntry::setData(const bsl::shared_ptr<ntsa::Data>& data)
{
    d_data_sp = data;
}

NTCCFG_INLINE
void SendQueueEntry::setLength(bsl::size_t length)
{
    d_length = length;
}

NTCCFG_INLINE
void SendQueueEntry::setTimestamp(bsl::int64_t timestamp)
{
    d_timestamp = timestamp;
}

NTCCFG_INLINE
void SendQueueEntry::setDeadline(const bsls::TimeInterval& value)
{
    d_deadline = value;
}

NTCCFG_INLINE
void SendQueueEntry::setDeadline(
    const bdlb::NullableValue<bsls::TimeInterval>& value)
{
    d_deadline = value;
}

NTCCFG_INLINE
void SendQueueEntry::setTimer(const bsl::shared_ptr<ntci::Timer>& timer)
{
    d_timer_sp = timer;

    if (d_callbackEntry_sp) {
        d_callbackEntry_sp->setTimer(timer);
    }
}

NTCCFG_INLINE
void SendQueueEntry::setCallbackEntry(
    const bsl::shared_ptr<SendCallbackQueueEntry>& callbackEntry)
{
    d_callbackEntry_sp = callbackEntry;
}

NTCCFG_INLINE
void SendQueueEntry::setZeroCopyId(bsl::uint32_t value)
{
    d_zeroCopyId = value;
}

NTCCFG_INLINE
void SendQueueEntry::setInProgress()
{
    d_inProgress = true;
}

NTCCFG_INLINE
void SendQueueEntry::closeTimer()
{
    if (d_timer_sp) {
        d_timer_sp->close();
        d_timer_sp.reset();
    }

    if (d_callbackEntry_sp) {
        d_callbackEntry_sp->clearTimer();
    }
}

NTCCFG_INLINE
bsl::uint64_t SendQueueEntry::id() const
{
    return d_id;
}

NTCCFG_INLINE
const bdlb::NullableValue<ntca::SendToken>& SendQueueEntry::token() const
{
    return d_token;
}

NTCCFG_INLINE
const bdlb::NullableValue<ntsa::Endpoint>& SendQueueEntry::endpoint() const
{
    return d_endpoint;
}

NTCCFG_INLINE
const bsl::shared_ptr<ntsa::Data>& SendQueueEntry::data() const
{
    return d_data_sp;
}

NTCCFG_INLINE
bsl::size_t SendQueueEntry::length() const
{
    return d_length;
}

NTCCFG_INLINE
bsl::int64_t SendQueueEntry::timestamp() const
{
    return d_timestamp;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsls::TimeInterval>& SendQueueEntry::deadline() const
{
    return d_deadline;
}

NTCCFG_INLINE
bsls::TimeInterval SendQueueEntry::delay() const
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
const bsl::shared_ptr<SendCallbackQueueEntry>& SendQueueEntry::callbackEntry()
    const
{
    return d_callbackEntry_sp;
}

NTCCFG_INLINE
bsl::uint32_t SendQueueEntry::zeroCopyId() const
{
    return d_zeroCopyId;
}

NTCCFG_INLINE
bool SendQueueEntry::inProgress() const
{
    return d_inProgress;
}

NTCCFG_INLINE
bool SendQueueEntry::isBatchable() const
{
    if (NTCCFG_UNLIKELY(!d_data_sp)) {
        return false;
    }

    if (NTCCFG_UNLIKELY(d_data_sp->isFile())) {
        return false;
    }

    return true;
}

NTCCFG_INLINE
bsl::shared_ptr<ntcq::SendCallbackQueueEntry> SendQueue::createCallbackEntry()
{
    return d_callbackEntryPool.create();
}

NTCCFG_INLINE
bsl::uint64_t SendQueue::generateEntryId()
{
    return ++d_nextEntryId;
}

NTCCFG_INLINE
bool SendQueue::pushEntry(const SendQueueEntry& entry)
{
    d_entryList.push_back(entry);

    if (entry.data()) {
        BSLS_ASSERT(entry.length() > 0);
        BSLS_ASSERT(entry.length() == entry.data()->size());

        d_size += entry.length();
    }

    return d_entryList.size() == 1;
}

NTCCFG_INLINE
SendQueueEntry& SendQueue::frontEntry()
{
    return d_entryList.front();
}

NTCCFG_INLINE
bool SendQueue::popEntry()
{
    {
        SendQueueEntry& entry = d_entryList.front();

        if (entry.data()) {
            BSLS_ASSERT(entry.length() > 0);
            BSLS_ASSERT(entry.length() == entry.data()->size());
            BSLS_ASSERT(d_size >= entry.length());
            d_size -= entry.length();
        }
    }

    d_entryList.pop_front();
    return d_entryList.empty();
}

NTCCFG_INLINE
void SendQueue::popSize(bsl::size_t numBytes)
{
    BSLS_ASSERT(!d_entryList.empty());

    SendQueueEntry& entry = d_entryList.front();

    BSLS_ASSERT(entry.data());
    BSLS_ASSERT(entry.data()->size() == entry.length());

    ntsa::DataUtil::pop(entry.data().get(), numBytes);
    entry.setInProgress();

    BSLS_ASSERT(entry.length() >= numBytes);
    entry.setLength(entry.length() - numBytes);

    BSLS_ASSERT(entry.data()->size() == entry.length());

    BSLS_ASSERT(d_size >= numBytes);
    d_size -= numBytes;
}

NTCCFG_INLINE
bool SendQueue::removeEntryId(
    bsl::shared_ptr<ntcq::SendCallbackQueueEntry>* result,
    bsl::uint64_t                                  id)
{
    result->reset();

    for (EntryList::iterator it = d_entryList.begin(); it != d_entryList.end();
         ++it)
    {
        const ntcq::SendQueueEntry& entry = *it;

        if (entry.id() == id) {
            if (!entry.deadline().isNull()) {
                if (!entry.inProgress()) {
                    if (entry.data()) {
                        BSLS_ASSERT(entry.length() > 0);
                        BSLS_ASSERT(entry.length() == entry.data()->size());
                        BSLS_ASSERT(d_size >= entry.length());
                        d_size -= entry.length();
                    }

                    if (entry.callbackEntry()) {
                        *result = entry.callbackEntry();
                    }

                    d_entryList.erase(it);
                }
            }
            break;
        }
    }

    return d_entryList.empty();
}

NTCCFG_INLINE
bool SendQueue::removeEntryToken(
    bsl::shared_ptr<ntcq::SendCallbackQueueEntry>* result,
    const ntca::SendToken&                         token)
{
    result->reset();

    for (EntryList::iterator it = d_entryList.begin(); it != d_entryList.end();
         ++it)
    {
        const ntcq::SendQueueEntry& entry = *it;

        if (!entry.token().isNull()) {
            if (entry.token().value() == token) {
                if (!entry.inProgress()) {
                    if (entry.data()) {
                        BSLS_ASSERT(entry.length() > 0);
                        BSLS_ASSERT(entry.length() == entry.data()->size());
                        BSLS_ASSERT(d_size >= entry.length());
                        d_size -= entry.length();
                    }

                    if (entry.callbackEntry()) {
                        *result = entry.callbackEntry();
                    }

                    d_entryList.erase(it);
                }
                break;
            }
        }
    }

    return d_entryList.empty();
}

NTCCFG_INLINE
bool SendQueue::removeAll(
    bsl::vector<bsl::shared_ptr<ntcq::SendCallbackQueueEntry> >* result)
{
    bool nonEmpty = !d_entryList.empty();

    for (EntryList::iterator it = d_entryList.begin(); it != d_entryList.end();
         ++it)
    {
        const ntcq::SendQueueEntry& entry = *it;
        if (entry.callbackEntry()) {
            result->push_back(entry.callbackEntry());
        }
    }

    d_entryList.clear();
    d_size = 0;

    return nonEmpty;
}

NTCCFG_INLINE
void SendQueue::setData(const bsl::shared_ptr<bdlbb::Blob>& data)
{
    d_data_sp = data;
}

NTCCFG_INLINE
void SendQueue::setLowWatermark(bsl::size_t lowWatermark)
{
    d_watermarkLow = lowWatermark;

    ntcs::WatermarkUtil::sanitizeOutgoingQueueWatermarks(&d_watermarkLow,
                                                         &d_watermarkHigh);
}

NTCCFG_INLINE
void SendQueue::setHighWatermark(bsl::size_t highWatermark)
{
    d_watermarkHigh = highWatermark;

    ntcs::WatermarkUtil::sanitizeOutgoingQueueWatermarks(&d_watermarkLow,
                                                         &d_watermarkHigh);
}

NTCCFG_INLINE
bool SendQueue::authorizeLowWatermarkEvent()
{
    if (ntcs::WatermarkUtil::isOutgoingQueueLowWatermarkSatisfied(
            d_size,
            d_watermarkLow))
    {
        if (d_watermarkLowWanted) {
            d_watermarkLowWanted  = false;
            d_watermarkHighWanted = true;
            return true;
        }
    }

    return false;
}

NTCCFG_INLINE
bool SendQueue::authorizeHighWatermarkEvent()
{
    if (ntcs::WatermarkUtil::isOutgoingQueueHighWatermarkViolated(
            d_size,
            d_watermarkHigh))
    {
        if (d_watermarkHighWanted) {
            d_watermarkHighWanted = false;
            d_watermarkLowWanted  = true;
            return true;
        }
    }

    return false;
}

NTCCFG_INLINE
bool SendQueue::authorizeHighWatermarkEvent(bsl::size_t effectiveHighWatermark)
{
    if (ntcs::WatermarkUtil::isOutgoingQueueHighWatermarkViolated(
            d_size,
            effectiveHighWatermark))
    {
        if (d_watermarkHighWanted) {
            d_watermarkHighWanted = false;
            d_watermarkLowWanted  = true;
            return true;
        }
    }

    return false;
}

NTCCFG_INLINE
const bsl::shared_ptr<bdlbb::Blob>& SendQueue::data() const
{
    return d_data_sp;
}

NTCCFG_INLINE
bsl::size_t SendQueue::lowWatermark() const
{
    return d_watermarkLow;
}

NTCCFG_INLINE
bsl::size_t SendQueue::highWatermark() const
{
    return d_watermarkHigh;
}

NTCCFG_INLINE
bsl::size_t SendQueue::size() const
{
    return d_size;
}

NTCCFG_INLINE
bool SendQueue::hasEntry() const
{
    return !d_entryList.empty();
}

NTCCFG_INLINE
bool SendQueue::isLowWatermarkSatisfied() const
{
    return ntcs::WatermarkUtil::isOutgoingQueueLowWatermarkSatisfied(
        d_size,
        d_watermarkLow);
}

NTCCFG_INLINE
bool SendQueue::isHighWatermarkViolated() const
{
    return ntcs::WatermarkUtil::isOutgoingQueueHighWatermarkViolated(
        d_size,
        d_watermarkHigh);
}

NTCCFG_INLINE
bool SendQueue::isHighWatermarkViolated(
    bsl::size_t effectiveHighWatermark) const
{
    return ntcs::WatermarkUtil::isOutgoingQueueHighWatermarkViolated(
        d_size,
        effectiveHighWatermark);
}

NTCCFG_INLINE
ntca::WriteQueueContext SendQueue::context() const
{
    ntca::WriteQueueContext context;

    context.setSize(d_size);
    context.setLowWatermark(d_watermarkLow);
    context.setHighWatermark(d_watermarkHigh);

    return context;
}

}  // close package namespace
}  // close enterprise namespace
#endif
