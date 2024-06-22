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

#ifndef INCLUDED_NTCQ_ACCEPT
#define INCLUDED_NTCQ_ACCEPT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_acceptoptions.h>
#include <ntca_acceptqueuecontext.h>
#include <ntccfg_platform.h>
#include <ntci_acceptcallback.h>
#include <ntci_acceptor.h>
#include <ntci_strand.h>
#include <ntci_streamsocket.h>
#include <ntci_timer.h>
#include <ntcs_callbackstate.h>
#include <ntcs_watermarkutil.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
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
/// Describe an entry in an accept callback queue.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcq
class AcceptCallbackQueueEntry
{
    ntccfg::Object               d_object;
    ntcs::CallbackState          d_state;
    ntci::AcceptCallback         d_callback;
    ntca::AcceptOptions          d_options;
    bsl::shared_ptr<ntci::Timer> d_timer_sp;

  private:
    AcceptCallbackQueueEntry(const AcceptCallbackQueueEntry&)
        BSLS_KEYWORD_DELETED;
    AcceptCallbackQueueEntry& operator=(const AcceptCallbackQueueEntry&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new accept callback queue entry. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit AcceptCallbackQueueEntry(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~AcceptCallbackQueueEntry();

    /// Clear the state of this entry.
    void clear();

    /// Assign the specified 'callback' to be invoked according to the
    /// specified 'options'.
    void assign(const ntci::AcceptCallback& callback,
                const ntca::AcceptOptions&  options);

    /// Set the timer to the specified 'timer'.
    void setTimer(const bsl::shared_ptr<ntci::Timer>& timer);

    /// Close the timer, if any.
    void closeTimer();

    /// Return the criteria to invoke the callback.
    const ntca::AcceptOptions& options() const;

    /// Invoke the callback of the specified 'entry' for the specified
    /// 'acceptor', 'streamSocket', and 'event'. If the specified
    /// 'defer' flag is false and the requirements of the strand of the
    /// specified 'entry' permits the callback to be invoked immediately by
    /// the 'strand', unlock the specified 'mutex', invoke the callback,
    /// then relock the 'mutex'. Otherwise, enqueue the invocation of the
    /// callback to be executed on the strand of the 'entry', if defined, or
    /// by the specified 'executor' otherwise.
    static void dispatch(
        const bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>& entry,
        const bsl::shared_ptr<ntci::Acceptor>&                 acceptor,
        const bsl::shared_ptr<ntci::StreamSocket>&             streamSocket,
        const ntca::AcceptEvent&                               event,
        const bsl::shared_ptr<ntci::Strand>&                   strand,
        const bsl::shared_ptr<ntci::Executor>&                 executor,
        bool                                                   defer,
        ntccfg::Mutex*                                         mutex);

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(AcceptCallbackQueueEntry);
};

/// @internal @brief
/// Provide a pool of shared pointers to accept callback queue entries.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcq
class AcceptCallbackQueueEntryPool
{
    typedef bdlcc::SharedObjectPool<
        ntcq::AcceptCallbackQueueEntry,
        bdlcc::ObjectPoolFunctors::DefaultCreator,
        bdlcc::ObjectPoolFunctors::Clear<ntcq::AcceptCallbackQueueEntry> >
        Pool;

    Pool d_pool;

  private:
    AcceptCallbackQueueEntryPool(const AcceptCallbackQueueEntryPool&)
        BSLS_KEYWORD_DELETED;
    AcceptCallbackQueueEntryPool& operator=(
        const AcceptCallbackQueueEntryPool&) BSLS_KEYWORD_DELETED;

  private:
    /// Construct a new accept callback queue entry at the specified
    /// 'address' using the specified 'allocator' to supply memory.
    static void construct(void* address, bslma::Allocator* allocator);

  public:
    /// Create a new object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit AcceptCallbackQueueEntryPool(
        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~AcceptCallbackQueueEntryPool();

    /// Return a shared pointer to an accept callback queue entry in the
    /// pool having a default value. The resulting accept callback queue
    /// entry is automatically returned to this pool when its reference
    /// count reaches zero.
    bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry> create();
};

/// @internal @brief
/// Provide an accept callback queue.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcq
class AcceptCallbackQueue
{
    /// Define a type alias for a linked list of callback
    /// entries.
    typedef bsl::list<bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry> >
        EntryList;

    /// Define a type alias for a pool of shared pointers to
    /// callback entries.
    typedef ntcq::AcceptCallbackQueueEntryPool EntryPool;

    EntryList         d_entryList;
    EntryPool         d_entryPool;
    bslma::Allocator* d_allocator_p;

  private:
    AcceptCallbackQueue(const AcceptCallbackQueue&) BSLS_KEYWORD_DELETED;
    AcceptCallbackQueue& operator=(const AcceptCallbackQueue&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new accept callback queue having an unlimited size.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit AcceptCallbackQueue(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~AcceptCallbackQueue();

    /// Return a shared pointer to a new accept callback queue entry.
    bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry> create();

    /// Push the specified 'entry' onto the callback queue. Return the
    /// error.
    ntsa::Error push(
        const bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>& entry);

    /// Pop the entry at the front of the queue and load it into the
    /// specified 'result'. Return the error.
    ntsa::Error pop(bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>* result);

    /// Remove the specified 'entry' from the queue, if found. Return the
    /// error.
    ntsa::Error remove(
        const bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>& entry);

    /// Remove the entry having the specified 'token' from the queue, if
    /// found, and load it into the specified 'result'. Return the error.
    ntsa::Error remove(bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>* result,
                       const ntca::AcceptToken&                         token);

    /// Remove all entries and load them into the specified 'result'.
    void removeAll(
        bsl::vector<bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry> >* result);

    /// Return the number of callbacks in the queue.
    bsl::size_t size() const;

    /// Return true if the number of callbacks in the queue is zero,
    /// otherwise return false.
    bool empty() const;
};

/// @internal @brief
/// Describe an entry in an accept queue.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcq
class AcceptQueueEntry
{
    bsl::shared_ptr<ntci::StreamSocket> d_streamSocket_sp;
    bsl::int64_t                        d_timestamp;

  public:
    /// Create a new receive from message queue entry.
    AcceptQueueEntry();

    /// Set the stream socket to the specified 'streamSocket'.
    void setStreamSocket(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket);

    /// Set the timestamp to the specified 'timestamp'.
    void setTimestamp(bsl::int64_t timestamp);

    /// Return the endpoint.
    const ntsa::Endpoint& endpoint() const;

    /// Return the data.
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket() const;

    /// Return the timestamp, in nanoseconds since an arbitrary but
    /// consistent epoch.
    bsl::int64_t timestamp() const;

    /// Return the duration from the timestamp until now.
    bsls::TimeInterval delay() const;
};

/// @internal @brief
/// Provide an accept queue.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcq
class AcceptQueue
{
    /// This typedef defines a linked list of structures that describe
    /// the read queue.
    typedef bsl::list<AcceptQueueEntry> EntryList;

    EntryList           d_entryList;
    bsl::size_t         d_size;
    bsl::size_t         d_watermarkLow;
    bool                d_watermarkLowWanted;
    bsl::size_t         d_watermarkHigh;
    bool                d_watermarkHighWanted;
    AcceptCallbackQueue d_callbackQueue;
    bslma::Allocator*   d_allocator_p;

  private:
    AcceptQueue(const AcceptQueue&) BSLS_KEYWORD_DELETED;
    AcceptQueue& operator=(const AcceptQueue&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new receive from message queue. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit AcceptQueue(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~AcceptQueue();

    /// Push the specified 'entry' onto the queue. Return true if queue
    /// becomes non-empty as a result of this operation, otherwise return
    /// false.
    bool pushEntry(const AcceptQueueEntry& entry);

    /// Return a reference to the modifiable entry at the front of the
    /// queue.
    AcceptQueueEntry& frontEntry();

    /// Pop the entry off the front of the queue. Return true if the
    /// queue becomes empty as a result of the operation, otherwise return
    /// false.
    bool popEntry();

    /// Return a shared pointer to a new receive callback queue entry.
    bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry> createCallbackEntry();

    /// Push the specified 'callbackEntry' onto the callback queue. Return
    /// the error.
    ntsa::Error pushCallbackEntry(
        const bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>& callbackEntry);

    /// Pop the callback entry at the front of the queue if its criteria is
    /// satisfied and load the callback entry into the specified 'result'.
    /// Return the error.
    ntsa::Error popCallbackEntry(
        bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>* result);

    /// Pop all callback entries into the specified 'result'.
    void popAllCallbackEntries(
        bsl::vector<bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry> >* result);

    /// Remove the specified 'callbackEntry' from the queue, if found.
    /// Return the error.
    ntsa::Error removeCallbackEntry(
        const bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>& callbackEntry);

    /// Remove the entry having the specified 'token' from the queue, if
    /// found, and load it into the specified 'result'. Return the error.
    ntsa::Error removeCallbackEntry(
        bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>* result,
        const ntca::AcceptToken&                         token);

    /// Set the low watermark to the specified 'lowWatermark'.
    void setLowWatermark(bsl::size_t lowWatermark);

    /// Set the high watermark to the specified 'highWatermark'.
    void setHighWatermark(bsl::size_t highWatermark);

    /// Return true if the queue has been filled to greater than the high
    /// watermark, otherwise return false.
    bool authorizeLowWatermarkEvent();

    /// Return true if the queue has been filled to greater than the high
    /// watermark, otherwise return false.
    bool authorizeHighWatermarkEvent();

    /// Return the low watermark.
    bsl::size_t lowWatermark() const;

    /// Return the high watermark.
    bsl::size_t highWatermark() const;

    /// Return the number of bytes on the queue.
    bsl::size_t size() const;

    /// Return true if there are entries on the queue, and false otherwise.
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

    /// Return the accept queue context.
    ntca::AcceptQueueContext context() const;
};

NTCCFG_INLINE
void AcceptCallbackQueueEntry::assign(const ntci::AcceptCallback& callback,
                                      const ntca::AcceptOptions&  options)
{
    d_callback = callback;
    d_options  = options;
}

NTCCFG_INLINE
void AcceptCallbackQueueEntry::setTimer(
    const bsl::shared_ptr<ntci::Timer>& timer)
{
    d_timer_sp = timer;
}

NTCCFG_INLINE
void AcceptCallbackQueueEntry::closeTimer()
{
    if (d_timer_sp) {
        d_timer_sp->close();
        d_timer_sp.reset();
    }
}

NTCCFG_INLINE
const ntca::AcceptOptions& AcceptCallbackQueueEntry::options() const
{
    return d_options;
}

NTCCFG_INLINE
bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry> AcceptCallbackQueueEntryPool::
    create()
{
    return d_pool.getObject();
}

NTCCFG_INLINE
bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry> AcceptCallbackQueue::create()
{
    return d_entryPool.create();
}

NTCCFG_INLINE
ntsa::Error AcceptCallbackQueue::push(
    const bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>& entry)
{
    d_entryList.push_back(entry);
    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error AcceptCallbackQueue::pop(
    bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>* result)
{
    if (!d_entryList.empty()) {
        *result = d_entryList.front();
        d_entryList.pop_front();
        return ntsa::Error();
    }
    else {
        return ntsa::Error::invalid();
    }
}

NTCCFG_INLINE
ntsa::Error AcceptCallbackQueue::remove(
    const bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>& entry)
{
    for (EntryList::iterator it = d_entryList.begin(); it != d_entryList.end();
         ++it)
    {
        const bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>& target = *it;
        if (entry == target) {
            d_entryList.erase(it);
            return ntsa::Error();
        }
    }

    return ntsa::Error(ntsa::Error::e_EOF);
}

NTCCFG_INLINE
ntsa::Error AcceptCallbackQueue::remove(
    bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>* result,
    const ntca::AcceptToken&                         token)
{
    result->reset();

    for (EntryList::iterator it = d_entryList.begin(); it != d_entryList.end();
         ++it)
    {
        const bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>& entry = *it;

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
void AcceptCallbackQueue::removeAll(
    bsl::vector<bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry> >* result)
{
    result->insert(result->end(), d_entryList.begin(), d_entryList.end());
    d_entryList.clear();
}

NTCCFG_INLINE
bsl::size_t AcceptCallbackQueue::size() const
{
    return d_entryList.size();
}

NTCCFG_INLINE
bool AcceptCallbackQueue::empty() const
{
    return d_entryList.empty();
}

NTCCFG_INLINE
AcceptQueueEntry::AcceptQueueEntry()
: d_streamSocket_sp()
, d_timestamp(0)
{
}

NTCCFG_INLINE
void AcceptQueueEntry::setStreamSocket(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    d_streamSocket_sp = streamSocket;
}

NTCCFG_INLINE
void AcceptQueueEntry::setTimestamp(bsl::int64_t timestamp)
{
    d_timestamp = timestamp;
}

NTCCFG_INLINE
const bsl::shared_ptr<ntci::StreamSocket>& AcceptQueueEntry::streamSocket()
    const
{
    return d_streamSocket_sp;
}

NTCCFG_INLINE
bsl::int64_t AcceptQueueEntry::timestamp() const
{
    return d_timestamp;
}

NTCCFG_INLINE
bsls::TimeInterval AcceptQueueEntry::delay() const
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
bool AcceptQueue::pushEntry(const AcceptQueueEntry& entry)
{
    d_entryList.push_back(entry);
    d_size += 1;
    return d_entryList.size() == 1;
}

NTCCFG_INLINE
AcceptQueueEntry& AcceptQueue::frontEntry()
{
    return d_entryList.front();
}

NTCCFG_INLINE
bool AcceptQueue::popEntry()
{
    {
        BSLS_ASSERT(d_size >= 1);
        d_size -= 1;
    }

    if (d_size < d_watermarkLow) {
        d_watermarkLowWanted  = true;
        d_watermarkHighWanted = true;
    }

    d_entryList.pop_front();
    return d_entryList.empty();
}

NTCCFG_INLINE
bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry> AcceptQueue::
    createCallbackEntry()
{
    return d_callbackQueue.create();
}

NTCCFG_INLINE
ntsa::Error AcceptQueue::pushCallbackEntry(
    const bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>& callbackEntry)
{
    return d_callbackQueue.push(callbackEntry);
}

NTCCFG_INLINE
ntsa::Error AcceptQueue::popCallbackEntry(
    bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>* result)
{
    if (d_entryList.empty()) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    return d_callbackQueue.pop(result);
}

NTCCFG_INLINE
void AcceptQueue::popAllCallbackEntries(
    bsl::vector<bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry> >* result)
{
    d_callbackQueue.removeAll(result);
}

NTCCFG_INLINE
ntsa::Error AcceptQueue::removeCallbackEntry(
    const bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>& callbackEntry)
{
    return d_callbackQueue.remove(callbackEntry);
}

NTCCFG_INLINE
ntsa::Error AcceptQueue::removeCallbackEntry(
    bsl::shared_ptr<ntcq::AcceptCallbackQueueEntry>* result,
    const ntca::AcceptToken&                         token)
{
    return d_callbackQueue.remove(result, token);
}

NTCCFG_INLINE
void AcceptQueue::setLowWatermark(bsl::size_t lowWatermark)
{
    d_watermarkLow       = lowWatermark;
    d_watermarkLowWanted = true;

    ntcs::WatermarkUtil::sanitizeIncomingQueueWatermarks(&d_watermarkLow,
                                                         &d_watermarkHigh);
}

NTCCFG_INLINE
void AcceptQueue::setHighWatermark(bsl::size_t highWatermark)
{
    d_watermarkHigh = highWatermark;

    ntcs::WatermarkUtil::sanitizeIncomingQueueWatermarks(&d_watermarkLow,
                                                         &d_watermarkHigh);
}

NTCCFG_INLINE
bool AcceptQueue::authorizeLowWatermarkEvent()
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
bool AcceptQueue::authorizeHighWatermarkEvent()
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
bsl::size_t AcceptQueue::lowWatermark() const
{
    return d_watermarkLow;
}

NTCCFG_INLINE
bsl::size_t AcceptQueue::highWatermark() const
{
    return d_watermarkHigh;
}

NTCCFG_INLINE
bsl::size_t AcceptQueue::size() const
{
    return d_size;
}

NTCCFG_INLINE
bool AcceptQueue::hasEntry() const
{
    return !d_entryList.empty();
}

NTCCFG_INLINE
bool AcceptQueue::hasCallbackEntry() const
{
    return !d_callbackQueue.empty();
}

NTCCFG_INLINE
bool AcceptQueue::isLowWatermarkSatisfied() const
{
    return ntcs::WatermarkUtil::isIncomingQueueLowWatermarkSatisfied(
        d_size,
        d_watermarkLow);
}

NTCCFG_INLINE
bool AcceptQueue::isHighWatermarkViolated() const
{
    return ntcs::WatermarkUtil::isIncomingQueueHighWatermarkViolated(
        d_size,
        d_watermarkHigh);
}

NTCCFG_INLINE
ntca::AcceptQueueContext AcceptQueue::context() const
{
    ntca::AcceptQueueContext context;

    context.setSize(d_size);
    context.setLowWatermark(d_watermarkLow);
    context.setHighWatermark(d_watermarkHigh);

    return context;
}

}  // close package namespace
}  // close enterprise namespace
#endif
