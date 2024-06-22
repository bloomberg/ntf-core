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

#include <ntcq_receive.h>

#include <ntccfg_bind.h>
#include <ntccfg_test.h>
#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------

// [ 1]
//-----------------------------------------------------------------------------
// [ 1]
//-----------------------------------------------------------------------------

namespace test {

/// Provide an interface to guarantee sequential, non-concurrent
/// execution.
class Strand : public ntci::Strand, public ntccfg::Shared<Strand>
{
    /// Define a type alias for a queue of callbacks to
    /// execute on this thread.
    typedef ntci::Executor::FunctorSequence FunctorQueue;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    ntccfg::Object    d_object;
    mutable Mutex     d_functorQueueMutex;
    FunctorQueue      d_functorQueue;
    bslma::Allocator* d_allocator_p;

  private:
    Strand(const Strand&) BSLS_KEYWORD_DELETED;
    Strand& operator=(const Strand&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new strand with characteristics suitable for this
    /// test driver. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit Strand(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Strand() BSLS_KEYWORD_OVERRIDE;

    /// Defer the specified 'function' to execute sequentially, and
    /// non-concurrently, after all previously deferred functions. Note that
    /// the 'function' is not necessarily guaranteed to execute on the same
    /// thread as previously deferred functions were executed, or is it
    /// necessarily guaranteed to execute on a different thread than
    /// previously deferred functions were executed.
    void execute(const Functor& function) BSLS_KEYWORD_OVERRIDE;

    /// Atomically defer the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE;

    /// Execute all pending operations on the calling thread. The behavior
    /// is undefined unless no other thread is processing pending
    /// operations.
    void drain() BSLS_KEYWORD_OVERRIDE;

    /// Cancel all pending functions.
    void clear() BSLS_KEYWORD_OVERRIDE;
};

Strand::Strand(bslma::Allocator* basicAllocator)
: d_object("test::Strand")
, d_functorQueueMutex()
, d_functorQueue(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Strand::~Strand()
{
}

void Strand::execute(const Functor& function)
{
    LockGuard lock(&d_functorQueueMutex);

    d_functorQueue.push_back(function);
}

void Strand::moveAndExecute(FunctorSequence* functorSequence,
                            const Functor&   functor)
{
    LockGuard lock(&d_functorQueueMutex);

    d_functorQueue.splice(d_functorQueue.end(), *functorSequence);
    if (functor) {
        d_functorQueue.push_back(functor);
    }
}

void Strand::drain()
{
    FunctorQueue functorQueue;
    {
        LockGuard lock(&d_functorQueueMutex);
        functorQueue.swap(d_functorQueue);
    }

    for (FunctorQueue::iterator it = functorQueue.begin();
         it != functorQueue.end();
         ++it)
    {
        (*it)();
    }
}

void Strand::clear()
{
    LockGuard lock(&d_functorQueueMutex);

    d_functorQueue.clear();
}

void processData(bsls::AtomicUint*                   numInvoked,
                 const ntsa::Error&                  error,
                 const bsl::shared_ptr<bdlbb::Blob>& data)
{
    NTCCFG_TEST_FALSE(error);

    ++(*numInvoked);
}

void processCancelled(bsls::AtomicUint*                   numInvoked,
                      const ntsa::Error&                  error,
                      const bsl::shared_ptr<bdlbb::Blob>& data)
{
    NTCCFG_TEST_EQ(error, ntsa::Error::e_CANCELLED);

    ++(*numInvoked);
}

#if 0
bool complete(const bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>& entry)
{
    bsl::shared_ptr<ntci::Strand>   strand;
    bsl::shared_ptr<ntci::Executor> executor;

    ntccfg::Mutex     mutex;
    ntccfg::LockGuard guard(&mutex);

    bsl::shared_ptr<bdlbb::Blob> data;

    return ntcq::ReceiveCallbackQueueEntry::complete(
        entry, data, strand, executor, false, &mutex);
}

bool fail(const bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>& entry)
{
    bsl::shared_ptr<ntci::Strand>   strand;
    bsl::shared_ptr<ntci::Executor> executor;

    ntccfg::Mutex     mutex;
    ntccfg::LockGuard guard(&mutex);

    return ntcq::ReceiveCallbackQueueEntry::fail(
        entry,
        ntsa::Error(ntsa::Error::e_CANCELLED),
        strand,
        executor,
        false,
        &mutex);
}

bool abort(const bsl::shared_ptr<ntcq::ReceiveCallbackQueueEntry>& entry)
{
    return entry->abort();
}
#endif

}  // close namespace test

NTCCFG_TEST_CASE(1)
{
#if 0
    // Concern: The callback entry invokes the callback with the correct
    // parameters, the correct number of times, when completed, canceled, and
    // aborted, from within a context that allows the callback entry to be
    // immediately executed.

    ntccfg::TestAllocator ta;
    {
        typedef ntcq::ReceiveCallbackQueueEntry Entry;

        // Test clear, assign, complete, cancel.

        {
            bsl::shared_ptr<Entry> entry;
            entry.createInplace(&ta, &ta);

            bool result;

            for (bsl::size_t i = 0; i < 2; ++i) {
                entry->clear();

                bsls::AtomicUint numInvoked;

                entry->assign(NTCCFG_BIND(&test::processData,
                                          &numInvoked,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              ntci::Strand::unspecified(),
                              ntca::ReceiveOptions());

                result = test::complete(entry);
                NTCCFG_TEST_TRUE(result);
                NTCCFG_TEST_EQ(numInvoked, 1);

                result = test::fail(entry);
                NTCCFG_TEST_FALSE(result);
                NTCCFG_TEST_EQ(numInvoked, 1);
            }
        }

        // Test clear, assign, complete, abort.

        {
            bsl::shared_ptr<Entry> entry;
            entry.createInplace(&ta, &ta);

            bool result;

            for (bsl::size_t i = 0; i < 2; ++i) {
                entry->clear();

                bsls::AtomicUint numInvoked;

                entry->assign(NTCCFG_BIND(&test::processData,
                                          &numInvoked,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              ntci::Strand::unspecified(),
                              ntca::ReceiveOptions());

                result = test::complete(entry);
                NTCCFG_TEST_TRUE(result);
                NTCCFG_TEST_EQ(numInvoked, 1);

                result = test::abort(entry);
                NTCCFG_TEST_FALSE(result);
                NTCCFG_TEST_EQ(numInvoked, 1);
            }
        }

        // Test clear, assign, cancel, complete.

        {
            bsl::shared_ptr<Entry> entry;
            entry.createInplace(&ta, &ta);

            bool result;

            for (bsl::size_t i = 0; i < 2; ++i) {
                entry->clear();

                bsls::AtomicUint numInvoked;

                entry->assign(NTCCFG_BIND(&test::processCancelled,
                                          &numInvoked,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              ntci::Strand::unspecified(),
                              ntca::ReceiveOptions());

                result = test::fail(entry);
                NTCCFG_TEST_TRUE(result);
                NTCCFG_TEST_EQ(numInvoked, 1);

                result = test::complete(entry);
                NTCCFG_TEST_FALSE(result);
                NTCCFG_TEST_EQ(numInvoked, 1);
            }
        }

        // Test clear, assign, cancel, abort.

        {
            bsl::shared_ptr<Entry> entry;
            entry.createInplace(&ta, &ta);

            bool result;

            for (bsl::size_t i = 0; i < 2; ++i) {
                entry->clear();

                bsls::AtomicUint numInvoked;

                entry->assign(NTCCFG_BIND(&test::processCancelled,
                                          &numInvoked,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              ntci::Strand::unspecified(),
                              ntca::ReceiveOptions());

                result = test::fail(entry);
                NTCCFG_TEST_TRUE(result);
                NTCCFG_TEST_EQ(numInvoked, 1);

                result = test::abort(entry);
                NTCCFG_TEST_FALSE(result);
                NTCCFG_TEST_EQ(numInvoked, 1);
            }
        }

        // Test clear, assign, abort, complete.

        {
            bsl::shared_ptr<Entry> entry;
            entry.createInplace(&ta, &ta);

            bool result;

            for (bsl::size_t i = 0; i < 2; ++i) {
                entry->clear();

                bsls::AtomicUint numInvoked;

                entry->assign(NTCCFG_BIND(&test::processCancelled,
                                          &numInvoked,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              ntci::Strand::unspecified(),
                              ntca::ReceiveOptions());

                result = test::abort(entry);
                NTCCFG_TEST_TRUE(result);
                NTCCFG_TEST_EQ(numInvoked, 0);

                result = test::complete(entry);
                NTCCFG_TEST_FALSE(result);
                NTCCFG_TEST_EQ(numInvoked, 0);
            }
        }

        // Test clear, assign, abort, cancel.

        {
            bsl::shared_ptr<Entry> entry;
            entry.createInplace(&ta, &ta);

            bool result;

            for (bsl::size_t i = 0; i < 2; ++i) {
                entry->clear();

                bsls::AtomicUint numInvoked;

                entry->assign(NTCCFG_BIND(&test::processCancelled,
                                          &numInvoked,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              ntci::Strand::unspecified(),
                              ntca::ReceiveOptions());

                result = test::abort(entry);
                NTCCFG_TEST_TRUE(result);
                NTCCFG_TEST_EQ(numInvoked, 0);

                result = test::fail(entry);
                NTCCFG_TEST_FALSE(result);
                NTCCFG_TEST_EQ(numInvoked, 0);
            }
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
#endif
}

NTCCFG_TEST_CASE(2)
{
#if 0
    // Concern: The callback entry invokes the callback with the correct
    // parameters, the correct number of times, when completed, canceled, and
    // aborted, from within a context that requires the callback entry's
    // execution to be deferred (i.e. simulating a strand driven by another
    // thread.)

    ntccfg::TestAllocator ta;
    {
        typedef ntcq::ReceiveCallbackQueueEntry Entry;

        bsl::shared_ptr<test::Strand> strand;
        strand.createInplace(&ta, &ta);

        // Test clear, assign, complete, cancel.

        {
            bsl::shared_ptr<Entry> entry;
            entry.createInplace(&ta, &ta);

            bool result;

            for (bsl::size_t i = 0; i < 2; ++i) {
                entry->clear();

                bsls::AtomicUint numInvoked;

                entry->assign(NTCCFG_BIND(&test::processData,
                                          &numInvoked,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              strand,
                              ntca::ReceiveOptions());

                result = test::complete(entry);
                NTCCFG_TEST_TRUE(result);
                NTCCFG_TEST_EQ(numInvoked, 0);

                result = test::fail(entry);
                NTCCFG_TEST_FALSE(result);
                NTCCFG_TEST_EQ(numInvoked, 0);

                strand->drain();
                NTCCFG_TEST_EQ(numInvoked, 1);
            }
        }

        // Test clear, assign, complete, abort.

        {
            bsl::shared_ptr<Entry> entry;
            entry.createInplace(&ta, &ta);

            bool result;

            for (bsl::size_t i = 0; i < 2; ++i) {
                entry->clear();

                bsls::AtomicUint numInvoked;

                entry->assign(NTCCFG_BIND(&test::processData,
                                          &numInvoked,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              strand,
                              ntca::ReceiveOptions());

                result = test::complete(entry);
                NTCCFG_TEST_TRUE(result);
                NTCCFG_TEST_EQ(numInvoked, 0);

                result = test::abort(entry);
                NTCCFG_TEST_TRUE(result);
                NTCCFG_TEST_EQ(numInvoked, 0);

                strand->drain();
                NTCCFG_TEST_EQ(numInvoked, 0);
            }
        }

        // Test clear, assign, cancel, complete.

        {
            bsl::shared_ptr<Entry> entry;
            entry.createInplace(&ta, &ta);

            bool result;

            for (bsl::size_t i = 0; i < 2; ++i) {
                entry->clear();

                bsls::AtomicUint numInvoked;

                entry->assign(NTCCFG_BIND(&test::processCancelled,
                                          &numInvoked,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              strand,
                              ntca::ReceiveOptions());

                result = test::fail(entry);
                NTCCFG_TEST_TRUE(result);
                NTCCFG_TEST_EQ(numInvoked, 0);

                result = test::complete(entry);
                NTCCFG_TEST_FALSE(result);
                NTCCFG_TEST_EQ(numInvoked, 0);

                strand->drain();
                NTCCFG_TEST_EQ(numInvoked, 1);
            }
        }

        // Test clear, assign, cancel, abort.

        {
            bsl::shared_ptr<Entry> entry;
            entry.createInplace(&ta, &ta);

            bool result;

            for (bsl::size_t i = 0; i < 2; ++i) {
                entry->clear();

                bsls::AtomicUint numInvoked;

                entry->assign(NTCCFG_BIND(&test::processCancelled,
                                          &numInvoked,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              strand,
                              ntca::ReceiveOptions());

                result = test::fail(entry);
                NTCCFG_TEST_TRUE(result);
                NTCCFG_TEST_EQ(numInvoked, 0);

                result = test::abort(entry);
                NTCCFG_TEST_TRUE(result);
                NTCCFG_TEST_EQ(numInvoked, 0);

                strand->drain();
                NTCCFG_TEST_EQ(numInvoked, 0);
            }
        }

        // Test clear, assign, abort, complete.

        {
            bsl::shared_ptr<Entry> entry;
            entry.createInplace(&ta, &ta);

            bool result;

            for (bsl::size_t i = 0; i < 2; ++i) {
                entry->clear();

                bsls::AtomicUint numInvoked;

                entry->assign(NTCCFG_BIND(&test::processCancelled,
                                          &numInvoked,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              strand,
                              ntca::ReceiveOptions());

                result = test::abort(entry);
                NTCCFG_TEST_TRUE(result);
                NTCCFG_TEST_EQ(numInvoked, 0);

                result = test::complete(entry);
                NTCCFG_TEST_FALSE(result);
                NTCCFG_TEST_EQ(numInvoked, 0);

                strand->drain();
                NTCCFG_TEST_EQ(numInvoked, 0);
            }
        }

        // Test clear, assign, abort, cancel.

        {
            bsl::shared_ptr<Entry> entry;
            entry.createInplace(&ta, &ta);

            bool result;

            for (bsl::size_t i = 0; i < 2; ++i) {
                entry->clear();

                bsls::AtomicUint numInvoked;

                entry->assign(NTCCFG_BIND(&test::processCancelled,
                                          &numInvoked,
                                          NTCCFG_BIND_PLACEHOLDER_1,
                                          NTCCFG_BIND_PLACEHOLDER_2),
                              strand,
                              ntca::ReceiveOptions());

                result = test::abort(entry);
                NTCCFG_TEST_TRUE(result);
                NTCCFG_TEST_EQ(numInvoked, 0);

                result = test::fail(entry);
                NTCCFG_TEST_FALSE(result);
                NTCCFG_TEST_EQ(numInvoked, 0);

                strand->drain();
                NTCCFG_TEST_EQ(numInvoked, 0);
            }
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
#endif
}

NTCCFG_TEST_CASE(3)
{
#if 0
    // Concern: The pool of shared objects correctly creates a shared pointer
    // to an entry, which is released back to the pool when its reference
    // count reaches zero. The pool grows by one each time a new object must
    // be created but no objects are available.
    //
    // Plan:

    ntccfg::TestAllocator ta;
    {
        typedef ntcq::ReceiveCallbackQueueEntry     Entry;
        typedef ntcq::ReceiveCallbackQueueEntryPool Pool;

        // Create a pool an ensure it is initially empty.

        Pool pool(&ta);

        NTCCFG_TEST_EQ(pool.numObjects(), 0);
        NTCCFG_TEST_EQ(pool.numObjectsAvailable(), 0);

        // Allocate a shared object from the pool, ensure the pool is now
        // managing one object, with zero objects free, then reset the
        // shared pointer to the object, and ensure the shared object is
        // released back to the pool.

        {
            bsl::shared_ptr<Entry> entry = pool.create();

            NTCCFG_TEST_EQ(pool.numObjects(), 1);
            NTCCFG_TEST_EQ(pool.numObjectsAvailable(), 0);

            entry.reset();

            NTCCFG_TEST_EQ(pool.numObjects(), 1);
            NTCCFG_TEST_EQ(pool.numObjectsAvailable(), 1);
        }

        // Allocate another shared object from the pool and ensure the
        // returned object is from the pool, the reset the shared pointer to
        // return the object back to the pool.

        {
            bsl::shared_ptr<Entry> entry = pool.create();

            NTCCFG_TEST_EQ(pool.numObjects(), 1);
            NTCCFG_TEST_EQ(pool.numObjectsAvailable(), 0);

            entry.reset();

            NTCCFG_TEST_EQ(pool.numObjects(), 1);
            NTCCFG_TEST_EQ(pool.numObjectsAvailable(), 1);
        }

        // Allocate two shared objects from the pool, and ensure the pool
        // grows by one.

        {
            bsl::shared_ptr<Entry> entry1 = pool.create();

            NTCCFG_TEST_EQ(pool.numObjects(), 1);
            NTCCFG_TEST_EQ(pool.numObjectsAvailable(), 0);

            bsl::shared_ptr<Entry> entry2 = pool.create();

            NTCCFG_TEST_EQ(pool.numObjects(), 2);
            NTCCFG_TEST_EQ(pool.numObjectsAvailable(), 0);

            entry1.reset();

            NTCCFG_TEST_EQ(pool.numObjects(), 2);
            NTCCFG_TEST_EQ(pool.numObjectsAvailable(), 1);

            entry2.reset();

            NTCCFG_TEST_EQ(pool.numObjects(), 2);
            NTCCFG_TEST_EQ(pool.numObjectsAvailable(), 2);
        }

        // Allocate three shared objects from the pool, and ensure the pool
        // grows by one.

        {
            bsl::shared_ptr<Entry> entry1 = pool.create();

            NTCCFG_TEST_EQ(pool.numObjects(), 2);
            NTCCFG_TEST_EQ(pool.numObjectsAvailable(), 1);

            bsl::shared_ptr<Entry> entry2 = pool.create();

            NTCCFG_TEST_EQ(pool.numObjects(), 2);
            NTCCFG_TEST_EQ(pool.numObjectsAvailable(), 0);

            bsl::shared_ptr<Entry> entry3 = pool.create();

            NTCCFG_TEST_EQ(pool.numObjects(), 3);
            NTCCFG_TEST_EQ(pool.numObjectsAvailable(), 0);

            entry1.reset();

            NTCCFG_TEST_EQ(pool.numObjects(), 3);
            NTCCFG_TEST_EQ(pool.numObjectsAvailable(), 1);

            entry2.reset();

            NTCCFG_TEST_EQ(pool.numObjects(), 3);
            NTCCFG_TEST_EQ(pool.numObjectsAvailable(), 2);

            entry3.reset();

            NTCCFG_TEST_EQ(pool.numObjects(), 3);
            NTCCFG_TEST_EQ(pool.numObjectsAvailable(), 3);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
#endif
}

NTCCFG_TEST_CASE(4)
{
#if 0
    // Concern: The entries are correctly ordered in the queue.

    ntccfg::TestAllocator ta;
    {
        typedef ntcq::ReceiveCallbackQueueEntry Entry;
        typedef ntcq::ReceiveCallbackQueue      Queue;

        Queue queue(&ta);

        bsl::shared_ptr<Entry> entryOnePushed = queue.create();
        bsl::shared_ptr<Entry> entryTwoPushed = queue.create();

        bsls::AtomicUint numOneInvoked;
        bsls::AtomicUint numTwoInvoked;

        ntsa::Error error;
        bool        result;

        entryOnePushed->assign(NTCCFG_BIND(&test::processData,
                                           &numOneInvoked,
                                           NTCCFG_BIND_PLACEHOLDER_1,
                                           NTCCFG_BIND_PLACEHOLDER_2),
                               ntci::Strand::unspecified(),
                               ntca::ReceiveOptions());

        entryTwoPushed->assign(NTCCFG_BIND(&test::processCancelled,
                                           &numTwoInvoked,
                                           NTCCFG_BIND_PLACEHOLDER_1,
                                           NTCCFG_BIND_PLACEHOLDER_2),
                               ntci::Strand::unspecified(),
                               ntca::ReceiveOptions());

        queue.push(entryOnePushed);
        queue.push(entryTwoPushed);

        bsl::shared_ptr<Entry> entryOnePopped;
        error = queue.pop(&entryOnePopped, 1);
        NTCCFG_TEST_FALSE(error);
        NTCCFG_TEST_EQ(entryOnePopped, entryOnePushed);

        result = test::complete(entryOnePopped);
        NTCCFG_TEST_TRUE(result);
        NTCCFG_TEST_EQ(numOneInvoked, 1);

        result = test::fail(entryOnePopped);
        NTCCFG_TEST_FALSE(result);
        NTCCFG_TEST_EQ(numOneInvoked, 1);

        bsl::shared_ptr<Entry> entryTwoPopped;
        error = queue.pop(&entryTwoPopped, 1);
        NTCCFG_TEST_FALSE(error);
        NTCCFG_TEST_EQ(entryTwoPopped, entryTwoPushed);

        result = test::fail(entryTwoPopped);
        NTCCFG_TEST_TRUE(result);
        NTCCFG_TEST_EQ(numTwoInvoked, 1);

        result = test::complete(entryTwoPopped);
        NTCCFG_TEST_FALSE(result);
        NTCCFG_TEST_EQ(numTwoInvoked, 1);

        bsl::shared_ptr<Entry> entryUndefined;
        error = queue.pop(&entryUndefined, 0);
        NTCCFG_TEST_TRUE(error);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
#endif
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
}
NTCCFG_TEST_DRIVER_END;
