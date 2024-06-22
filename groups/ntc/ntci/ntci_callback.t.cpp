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

#include <ntci_callback.h>

#include <ntca_reactorevent.h>
#include <ntccfg_test.h>

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

/// Provide a mechanism to authorize the invocation of an
/// operation. This class is thread safe.
class Authorization : public ntci::Authorization
{
  public:
    /// Define a type alias for an atomic signed integer.
    typedef bsls::AtomicInt64 AtomicCountType;

  private:
    AtomicCountType d_count;
    CountType       d_limit;

  private:
    Authorization(const Authorization&) BSLS_KEYWORD_DELETED;
    Authorization& operator=(const Authorization&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new operation authorizaiton that permits the maximum
    /// number of simulataneous authorizations.
    Authorization();

    /// Create a new operation authorization that limits the maximum
    /// number of simultaneous authorizations to the specified 'limit'.
    explicit Authorization(CountType limit);

    /// Destroy this object.
    ~Authorization() BSLS_KEYWORD_OVERRIDE;

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset() BSLS_KEYWORD_OVERRIDE;

    /// Acquire an authorization to perform an operation. Return the error.
    ntsa::Error acquire() BSLS_KEYWORD_OVERRIDE;

    /// Release an authorization to perform an operation. Return the error.
    ntsa::Error release() BSLS_KEYWORD_OVERRIDE;

    /// Prevent the authorization of subsequent operations. Return true
    /// if authorization was successfully prevented, otherwise return false,
    /// indicating an operation is currently in progress.
    bool abort() BSLS_KEYWORD_OVERRIDE;

    /// Return a snapshot of the current number of acquired authorizations
    /// or -1 if authorization has been canceled.
    CountType count() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if authorization has been canceled, otherwise return
    /// false.
    bool canceled() const BSLS_KEYWORD_OVERRIDE;

    /// Create a new authorization. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    static bsl::shared_ptr<ntci::Authorization> create(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new operation authorization that limits the maximum number
    /// of simultaneous authorizations to the specified 'limit'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static bsl::shared_ptr<ntci::Authorization> create(
        bsl::size_t       limit,
        bslma::Allocator* basicAllocator = 0);
};

/// Provide an interface to guarantee sequential, non-concurrent
/// execution.
class Executor : public ntci::Executor, public ntccfg::Shared<Executor>
{
    /// Define a type alias for a queue of callbacks to execute on this thread.
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
    Executor(const Executor&) BSLS_KEYWORD_DELETED;
    Executor& operator=(const Executor&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new executor with characteristics suitable for this
    /// test driver. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit Executor(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Executor() BSLS_KEYWORD_OVERRIDE;

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
    void drain();

    /// Cancel all pending functions.
    void clear();

    /// Create a new executor. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static bsl::shared_ptr<test::Executor> create(
        bslma::Allocator* basicAllocator = 0);
};

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

    /// Return true if operations in this strand are currently being invoked
    /// by the current thread, otherwise return false.
    bool isRunningInCurrentThread() const BSLS_KEYWORD_OVERRIDE;

    /// Create a new strand. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static bsl::shared_ptr<ntci::Strand> create(
        bslma::Allocator* basicAllocator = 0);
};

Authorization::Authorization()
: d_count(0)
, d_limit(bsl::numeric_limits<CountType>::max())
{
}

Authorization::Authorization(CountType limit)
: d_count(0)
, d_limit(limit)
{
}

Authorization::~Authorization()
{
}

void Authorization::reset()
{
    d_count = 0;
}

ntsa::Error Authorization::acquire()
{
    while (true) {
        CountType c = d_count.load();

        if (c < 0) {
            return ntsa::Error(ntsa::Error::e_CANCELLED);
        }

        if (c == d_limit) {
            return ntsa::Error(ntsa::Error::e_LIMIT);
        }

        CountType n = c + 1;

        CountType p = d_count.testAndSwap(c, n);
        if (p != c) {
            continue;
        }

        break;
    }

    return ntsa::Error();
}

ntsa::Error Authorization::release()
{
    while (true) {
        CountType c = d_count.load();

        if (c < 0) {
            return ntsa::Error(ntsa::Error::e_CANCELLED);
        }

        if (c == 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        CountType n = c - 1;

        CountType p = d_count.testAndSwap(c, n);
        if (p != c) {
            continue;
        }

        break;
    }

    return ntsa::Error();
}

bool Authorization::abort()
{
    while (true) {
        CountType c = d_count.load();

        if (c < 0) {
            return true;
        }

        if (c > 0) {
            return false;
        }

        CountType n = -1;

        CountType p = d_count.testAndSwap(c, n);
        if (p != c) {
            continue;
        }

        break;
    }

    return true;
}

Authorization::CountType Authorization::count() const
{
    return d_count.load();
}

bool Authorization::canceled() const
{
    CountType c = d_count.load();
    return c < 0;
}

bsl::shared_ptr<ntci::Authorization> Authorization::create(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<test::Authorization> result;
    result.createInplace(allocator);

    return result;
}

bsl::shared_ptr<ntci::Authorization> Authorization::create(
    bsl::size_t       limit,
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<test::Authorization> result;
    result.createInplace(allocator,
                         static_cast<test::Authorization::CountType>(limit));

    return result;
}

Executor::Executor(bslma::Allocator* basicAllocator)
: d_object("test::Executor")
, d_functorQueueMutex()
, d_functorQueue(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Executor::~Executor()
{
    NTCCFG_TEST_TRUE(d_functorQueue.empty());
}

void Executor::execute(const Functor& function)
{
    LockGuard lock(&d_functorQueueMutex);

    d_functorQueue.push_back(function);
}

void Executor::moveAndExecute(FunctorSequence* functorSequence,
                              const Functor&   functor)
{
    LockGuard lock(&d_functorQueueMutex);

    d_functorQueue.splice(d_functorQueue.end(), *functorSequence);
    if (functor) {
        d_functorQueue.push_back(functor);
    }
}

void Executor::drain()
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

void Executor::clear()
{
    LockGuard lock(&d_functorQueueMutex);

    d_functorQueue.clear();
}

bsl::shared_ptr<test::Executor> Executor::create(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<test::Executor> strand;
    strand.createInplace(allocator, allocator);

    return strand;
}

Strand::Strand(bslma::Allocator* basicAllocator)
: d_object("test::Strand")
, d_functorQueueMutex()
, d_functorQueue(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Strand::~Strand()
{
    NTCCFG_TEST_TRUE(d_functorQueue.empty());
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

    {
        ntci::StrandGuard strandGuard(this);

        for (FunctorQueue::iterator it = functorQueue.begin();
             it != functorQueue.end();
             ++it)
        {
            (*it)();
        }
    }
}

void Strand::clear()
{
    LockGuard lock(&d_functorQueueMutex);

    d_functorQueue.clear();
}

bool Strand::isRunningInCurrentThread() const
{
    const ntci::Strand* current = ntci::Strand::getThreadLocal();
    return (current == this);
}

bsl::shared_ptr<ntci::Strand> Strand::create(bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<test::Strand> strand;
    strand.createInplace(allocator, allocator);

    return strand;
}

const int ARG1 = 1;
const int ARG2 = 2;
const int ARG3 = 3;

bsl::size_t callsToTargetFunctionArg0 = 0;
bsl::size_t callsToTargetFunctionArg1 = 0;
bsl::size_t callsToTargetFunctionArg2 = 0;
bsl::size_t callsToTargetFunctionArg3 = 0;

void targetFunctionArg0()
{
    NTCCFG_TEST_LOG_DEBUG << "Executed f0" << NTCCFG_TEST_LOG_END;
    ++callsToTargetFunctionArg0;
}

void targetFunctionArg1(int arg1)
{
    NTCCFG_TEST_LOG_DEBUG << "Executed f1" << NTCCFG_TEST_LOG_END;
    NTCCFG_TEST_EQ(arg1, test::ARG1);
    ++callsToTargetFunctionArg1;
}

void targetFunctionArg2(int arg1, int arg2)
{
    NTCCFG_TEST_LOG_DEBUG << "Executed f2" << NTCCFG_TEST_LOG_END;
    NTCCFG_TEST_EQ(arg1, test::ARG1);
    NTCCFG_TEST_EQ(arg2, test::ARG2);
    ++callsToTargetFunctionArg2;
}

void targetFunctionArg3(int arg1, int arg2, int arg3)
{
    NTCCFG_TEST_LOG_DEBUG << "Executed f3" << NTCCFG_TEST_LOG_END;
    NTCCFG_TEST_EQ(arg1, test::ARG1);
    NTCCFG_TEST_EQ(arg2, test::ARG2);
    NTCCFG_TEST_EQ(arg3, test::ARG3);
    ++callsToTargetFunctionArg3;
}

}  // close namespace test

NTCCFG_TEST_CASE(1)
{
    // Concern: 0-arg callbacks.
    // Plan:

    typedef ntci::Callback<void()> CallbackArg0;

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;

        {
            CallbackArg0 callback(&ta);
            NTCCFG_TEST_FALSE(callback);
        }

        // Test *:*

        {
            test::callsToTargetFunctionArg0 = 0;

            CallbackArg0 callback(&test::targetFunctionArg0, &ta);

            error = callback.execute(ntci::Strand::unknown());

            NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 1);
        }

        // Test *:strand

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0, &ta);

            error = callback.execute(strand);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 1);
        }

        // Test strand:*

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0, strand, &ta);

            error = callback.execute(ntci::Strand::unknown());

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);

            callback.reset();
            strand->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 1);
        }

        // Test strand:strand

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0, strand, &ta);

            error = callback.execute(strand);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 1);
        }

        // Test strand1:strand2

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<ntci::Strand> strand1 = test::Strand::create(&ta);

            bsl::shared_ptr<ntci::Strand> strand2 = test::Strand::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0, strand1, &ta);

            error = callback.execute(strand2);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);

            callback.reset();
            strand1->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 1);
        }

        // Test cancellation.

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<ntci::Authorization> authorization =
                test::Authorization::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0,
                                  authorization,
                                  &ta);

            bool canceled = callback.abort();
            NTCCFG_TEST_TRUE(canceled);

            error = callback.execute(ntci::Strand::unknown());

            NTCCFG_TEST_EQ(error, ntsa::Error::e_CANCELLED);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);
        }

        // Test asynchronous cancellation.

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<ntci::Authorization> authorization =
                test::Authorization::create(&ta);

            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0,
                                  authorization,
                                  strand,
                                  &ta);

            error = callback.execute(ntci::Strand::unknown());

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);

            bool canceled = callback.abort();
            NTCCFG_TEST_TRUE(canceled);

            strand->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern: 1-arg callbacks.
    // Plan:

    typedef ntci::Callback<void(int arg1)> CallbackArg1;

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;

        {
            CallbackArg1 callback(&ta);
            NTCCFG_TEST_FALSE(callback);
        }

        // Test *:*

        {
            test::callsToTargetFunctionArg1 = 0;

            CallbackArg1 callback(&test::targetFunctionArg1, &ta);

            error = callback.execute(test::ARG1, ntci::Strand::unknown());

            NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg1, 1);
        }

        // Test *:strand

        {
            test::callsToTargetFunctionArg1 = 0;

            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            CallbackArg1 callback(&test::targetFunctionArg1, &ta);

            error = callback.execute(test::ARG1, strand);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg1, 1);
        }

        // Test strand:*

        {
            test::callsToTargetFunctionArg1 = 0;

            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            CallbackArg1 callback(&test::targetFunctionArg1, strand, &ta);

            error = callback.execute(test::ARG1, ntci::Strand::unknown());

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg1, 0);

            callback.reset();
            strand->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg1, 1);
        }

        // Test strand:strand

        {
            test::callsToTargetFunctionArg1 = 0;

            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            CallbackArg1 callback(&test::targetFunctionArg1, strand, &ta);

            error = callback.execute(test::ARG1, strand);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg1, 1);
        }

        // Test strand1:strand2

        {
            test::callsToTargetFunctionArg1 = 0;

            bsl::shared_ptr<ntci::Strand> strand1 = test::Strand::create(&ta);

            bsl::shared_ptr<ntci::Strand> strand2 = test::Strand::create(&ta);

            CallbackArg1 callback(&test::targetFunctionArg1, strand1, &ta);

            error = callback.execute(test::ARG1, strand2);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg1, 0);

            callback.reset();
            strand1->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg1, 1);
        }

        // Test cancellation.

        {
            test::callsToTargetFunctionArg1 = 0;

            bsl::shared_ptr<ntci::Authorization> authorization =
                test::Authorization::create(&ta);

            CallbackArg1 callback(&test::targetFunctionArg1,
                                  authorization,
                                  &ta);

            bool canceled = callback.abort();
            NTCCFG_TEST_TRUE(canceled);

            error = callback.execute(test::ARG1, ntci::Strand::unknown());

            NTCCFG_TEST_EQ(error, ntsa::Error::e_CANCELLED);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg1, 0);
        }

        // Test asynchronous cancellation.

        {
            test::callsToTargetFunctionArg1 = 0;

            bsl::shared_ptr<ntci::Authorization> authorization =
                test::Authorization::create(&ta);

            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            CallbackArg1 callback(&test::targetFunctionArg1,
                                  authorization,
                                  strand,
                                  &ta);

            error = callback.execute(test::ARG1, ntci::Strand::unknown());

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg1, 0);

            bool canceled = callback.abort();
            NTCCFG_TEST_TRUE(canceled);

            strand->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg1, 0);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Concern: 2-arg callbacks.
    // Plan:

    typedef ntci::Callback<void(int arg1, int arg2)> CallbackArg2;

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;

        {
            CallbackArg2 callback(&ta);
            NTCCFG_TEST_FALSE(callback);
        }

        // Test *:*

        {
            test::callsToTargetFunctionArg2 = 0;

            CallbackArg2 callback(&test::targetFunctionArg2, &ta);

            error = callback.execute(test::ARG1,
                                     test::ARG2,
                                     ntci::Strand::unknown());

            NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg2, 1);
        }

        // Test *:strand

        {
            test::callsToTargetFunctionArg2 = 0;

            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            CallbackArg2 callback(&test::targetFunctionArg2, &ta);

            error = callback.execute(test::ARG1, test::ARG2, strand);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg2, 1);
        }

        // Test strand:*

        {
            test::callsToTargetFunctionArg2 = 0;

            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            CallbackArg2 callback(&test::targetFunctionArg2, strand, &ta);

            error = callback.execute(test::ARG1,
                                     test::ARG2,
                                     ntci::Strand::unknown());

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg2, 0);

            callback.reset();
            strand->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg2, 1);
        }

        // Test strand:strand

        {
            test::callsToTargetFunctionArg2 = 0;

            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            CallbackArg2 callback(&test::targetFunctionArg2, strand, &ta);

            error = callback.execute(test::ARG1, test::ARG2, strand);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg2, 1);
        }

        // Test strand1:strand2

        {
            test::callsToTargetFunctionArg2 = 0;

            bsl::shared_ptr<ntci::Strand> strand1 = test::Strand::create(&ta);

            bsl::shared_ptr<ntci::Strand> strand2 = test::Strand::create(&ta);

            CallbackArg2 callback(&test::targetFunctionArg2, strand1, &ta);

            error = callback.execute(test::ARG1, test::ARG2, strand2);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg2, 0);

            callback.reset();
            strand1->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg2, 1);
        }

        // Test cancellation.

        {
            test::callsToTargetFunctionArg2 = 0;

            bsl::shared_ptr<ntci::Authorization> authorization =
                test::Authorization::create(&ta);

            CallbackArg2 callback(&test::targetFunctionArg2,
                                  authorization,
                                  &ta);

            bool canceled = callback.abort();
            NTCCFG_TEST_TRUE(canceled);

            error = callback.execute(test::ARG1,
                                     test::ARG2,
                                     ntci::Strand::unknown());

            NTCCFG_TEST_EQ(error, ntsa::Error::e_CANCELLED);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg2, 0);
        }

        // Test asynchronous cancellation.

        {
            test::callsToTargetFunctionArg2 = 0;

            bsl::shared_ptr<ntci::Authorization> authorization =
                test::Authorization::create(&ta);

            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            CallbackArg2 callback(&test::targetFunctionArg2,
                                  authorization,
                                  strand,
                                  &ta);

            error = callback.execute(test::ARG1,
                                     test::ARG2,
                                     ntci::Strand::unknown());

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg2, 0);

            bool canceled = callback.abort();
            NTCCFG_TEST_TRUE(canceled);

            strand->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg2, 0);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    // Concern: 3-arg callbacks.
    // Plan:

    typedef ntci::Callback<void(int arg1, int arg2, int arg3)> CallbackArg3;

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;

        {
            CallbackArg3 callback(&ta);
            NTCCFG_TEST_FALSE(callback);
        }

        // Test *:*

        {
            test::callsToTargetFunctionArg3 = 0;

            CallbackArg3 callback(&test::targetFunctionArg3, &ta);

            error = callback.execute(test::ARG1,
                                     test::ARG2,
                                     test::ARG3,
                                     ntci::Strand::unknown());

            NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg3, 1);
        }

        // Test *:strand

        {
            test::callsToTargetFunctionArg3 = 0;

            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            CallbackArg3 callback(&test::targetFunctionArg3, &ta);

            error =
                callback.execute(test::ARG1, test::ARG2, test::ARG3, strand);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg3, 1);
        }

        // Test strand:*

        {
            test::callsToTargetFunctionArg3 = 0;

            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            CallbackArg3 callback(&test::targetFunctionArg3, strand, &ta);

            error = callback.execute(test::ARG1,
                                     test::ARG2,
                                     test::ARG3,
                                     ntci::Strand::unknown());

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg3, 0);

            callback.reset();
            strand->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg3, 1);
        }

        // Test strand:strand

        {
            test::callsToTargetFunctionArg3 = 0;

            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            ntci::Callback<void(int arg1, int arg2, int arg3)> callback(
                &test::targetFunctionArg3,
                strand,
                &ta);

            error =
                callback.execute(test::ARG1, test::ARG2, test::ARG3, strand);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg3, 1);
        }

        // Test strand1:strand2

        {
            test::callsToTargetFunctionArg3 = 0;

            bsl::shared_ptr<ntci::Strand> strand1 = test::Strand::create(&ta);

            bsl::shared_ptr<ntci::Strand> strand2 = test::Strand::create(&ta);

            CallbackArg3 callback(&test::targetFunctionArg3, strand1, &ta);

            error =
                callback.execute(test::ARG1, test::ARG2, test::ARG3, strand2);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg3, 0);

            callback.reset();
            strand1->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg3, 1);
        }

        // Test cancellation.

        {
            test::callsToTargetFunctionArg3 = 0;

            bsl::shared_ptr<ntci::Authorization> authorization =
                test::Authorization::create(&ta);

            CallbackArg3 callback(&test::targetFunctionArg3,
                                  authorization,
                                  &ta);

            bool canceled = callback.abort();
            NTCCFG_TEST_TRUE(canceled);

            error = callback.execute(test::ARG1,
                                     test::ARG2,
                                     test::ARG3,
                                     ntci::Strand::unknown());

            NTCCFG_TEST_EQ(error, ntsa::Error::e_CANCELLED);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg3, 0);
        }

        // Test asynchronous cancellation.

        {
            test::callsToTargetFunctionArg3 = 0;

            bsl::shared_ptr<ntci::Authorization> authorization =
                test::Authorization::create(&ta);

            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            CallbackArg3 callback(&test::targetFunctionArg3,
                                  authorization,
                                  strand,
                                  &ta);

            error = callback.execute(test::ARG1,
                                     test::ARG2,
                                     test::ARG3,
                                     ntci::Strand::unknown());

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg3, 0);

            bool canceled = callback.abort();
            NTCCFG_TEST_TRUE(canceled);

            strand->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg3, 0);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

#define NTCI_CALLBACK_TEST_API_SUPPORTS_CALLBACK 1
#define NTCI_CALLBACK_TEST_API_SUPPORTS_FUNCTION 1
#define NTCI_CALLBACK_TEST_API_SUPPORTS_INTERFACE 1

namespace api {

typedef bsl::function<void(const ntca::ReactorEvent& event)>
    ReactorEventFunction;

typedef ntci::Callback<void(const ntca::ReactorEvent& event)>
    ReactorEventCallback;

/// Provide a sample pure interface to test API design.
class ReactorEventInterface
{
  public:
    /// Destroy this object.
    virtual ~ReactorEventInterface();

    // MANIPULAOTRS

    /// Process the specified reactor 'event'.
    virtual void processReactorEvent(const ntca::ReactorEvent& event) = 0;
};

/// This class implements a sample pure interface to test API design.
class ReactorEventMechanism : public ReactorEventInterface,
                              public ntccfg::Shared<ReactorEventMechanism>
{
  private:
    ReactorEventMechanism(const ReactorEventMechanism&) BSLS_KEYWORD_DELETED;
    ReactorEventMechanism& operator=(const ReactorEventMechanism&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new reactor event mechanism.
    ReactorEventMechanism();

    /// Destroy this object.
    ~ReactorEventMechanism() BSLS_KEYWORD_OVERRIDE;

    /// Process the specified reactor 'event'.
    void processReactorEvent(const ntca::ReactorEvent& event)
        BSLS_KEYWORD_OVERRIDE;
};

/// Provide a utility to test API design.
struct ReactorUtil {
    /// Invoke the specified 'function' with the specified 'event'.
    static void invokeFunction(const api::ReactorEventFunction& function,
                               const ntca::ReactorEvent&        event);

  public:
    /// Create a new reactor event callback invoking the specified
    /// 'function' on any strand. Optionally specify an 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default basicAllocator is used.
    static api::ReactorEventCallback createReactorEventCallback(
        const api::ReactorEventFunction& function,
        bslma::Allocator*                basicAllocator = 0);

    /// Create a new reactor event callback invoking the specified
    /// 'function' on the specified 'strand'. Optionally specify an
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    static api::ReactorEventCallback createReactorEventCallback(
        const api::ReactorEventFunction&     function,
        const bsl::shared_ptr<ntci::Strand>& strand,
        bslma::Allocator*                    basicAllocator = 0);

#if NTCI_CALLBACK_TEST_API_SUPPORTS_INTERFACE

    /// Register the specified 'interface' to be invoked when a reactor
    /// event occurs.
    static void registerEventInterface(
        const bsl::shared_ptr<api::ReactorEventInterface>& interface);

#endif

#if NTCI_CALLBACK_TEST_API_SUPPORTS_CALLBACK

    /// Register the specified 'callback' to be invoked on the callback's
    /// strand when a reactor event occurs.
    static void registerEventCallback(
        const api::ReactorEventCallback& callback);

#endif

#if NTCI_CALLBACK_TEST_API_SUPPORTS_FUNCTION

    /// Register the specified 'function' to be invoked on any strand when a
    /// reactor event occurs.
    static void registerEventCallback(
        const api::ReactorEventFunction& function);

    /// Register the specified 'function' to be invoked on the specified
    /// 'strand' when a reactor event occurs.
    static void registerEventCallback(
        const api::ReactorEventFunction&     function,
        const bsl::shared_ptr<ntci::Strand>& strand);

#endif

    /// Process the specified reactor 'event'.
    static void processReactorEvent(const ntca::ReactorEvent& event);

    /// Process the specified reactor 'event'.
    static void processReactorEventEx(const ntca::ReactorEvent& event, int id);
};

ReactorEventInterface::~ReactorEventInterface()
{
}

ReactorEventMechanism::ReactorEventMechanism()
{
}

ReactorEventMechanism::~ReactorEventMechanism()
{
}

void ReactorEventMechanism::processReactorEvent(
    const ntca::ReactorEvent& event)
{
    BSLS_LOG_DEBUG("Processing reactor event %s",
                   ntca::ReactorEventType::toString(event.type()));
}

void ReactorUtil::invokeFunction(const api::ReactorEventFunction& function,
                                 const ntca::ReactorEvent&        event)
{
    function(event);
}

api::ReactorEventCallback ReactorUtil::createReactorEventCallback(
    const api::ReactorEventFunction& function,
    bslma::Allocator*                basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    api::ReactorEventCallback result(function, allocator);

    return result;
}

api::ReactorEventCallback ReactorUtil::createReactorEventCallback(
    const api::ReactorEventFunction&     function,
    const bsl::shared_ptr<ntci::Strand>& strand,
    bslma::Allocator*                    basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    api::ReactorEventCallback result(function, strand, allocator);

    return result;
}

#if NTCI_CALLBACK_TEST_API_SUPPORTS_INTERFACE

void ReactorUtil::registerEventInterface(
    const bsl::shared_ptr<api::ReactorEventInterface>& interface)
{
    ntca::ReactorEvent event;
    event.setType(ntca::ReactorEventType::e_READABLE);

    interface->processReactorEvent(event);
}

#endif

#if NTCI_CALLBACK_TEST_API_SUPPORTS_CALLBACK

void ReactorUtil::registerEventCallback(
    const api::ReactorEventCallback& callback)
{
    ntca::ReactorEvent event;
    event.setType(ntca::ReactorEventType::e_READABLE);

    callback(event, ntci::Strand::unknown());
}

#endif

#if NTCI_CALLBACK_TEST_API_SUPPORTS_FUNCTION

void ReactorUtil::registerEventCallback(
    const api::ReactorEventFunction& function)
{
    ntca::ReactorEvent event;
    event.setType(ntca::ReactorEventType::e_READABLE);

    function(event);
}

void ReactorUtil::registerEventCallback(
    const api::ReactorEventFunction&     function,
    const bsl::shared_ptr<ntci::Strand>& strand)
{
    ntca::ReactorEvent event;
    event.setType(ntca::ReactorEventType::e_READABLE);

    if (ntci::Strand::passthrough(strand, ntci::Strand::unknown())) {
        function(event);
    }
    else {
        strand->execute(bdlf::BindUtil::bind(&ReactorUtil::invokeFunction,
                                             function,
                                             event));
    }
}

#endif

void ReactorUtil::processReactorEvent(const ntca::ReactorEvent& event)
{
    BSLS_LOG_DEBUG("Processing reactor event %s",
                   ntca::ReactorEventType::toString(event.type()));
}

void ReactorUtil::processReactorEventEx(const ntca::ReactorEvent& event,
                                        int                       id)
{
    BSLS_LOG_DEBUG("Processing reactor event %s by ID %d",
                   ntca::ReactorEventType::toString(event.type()),
                   id);
}

}  // close namespace api

NTCCFG_TEST_CASE(5)
{
    // Concern: API test binding to overloaded functions.

    ntccfg::TestAllocator ta;
    {
        bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

#if NTCI_CALLBACK_TEST_API_SUPPORTS_FUNCTION

        //
        // FUNCTIONS
        //

        // Function pointer bound to function, any strand.

        api::ReactorUtil::registerEventCallback(
            &api::ReactorUtil::processReactorEvent);

        // Function pointer bound to function, specific strand.

        api::ReactorUtil::registerEventCallback(
            &api::ReactorUtil::processReactorEvent,
            strand);

        // Function binding bound to function, any strand.

        api::ReactorUtil::registerEventCallback(
            bdlf::BindUtil::bind(&api::ReactorUtil::processReactorEventEx,
                                 bdlf::PlaceHolders::_1,
                                 123));

        // Function binding bound to function, specific strand.

        api::ReactorUtil::registerEventCallback(
            bdlf::BindUtil::bind(&api::ReactorUtil::processReactorEventEx,
                                 bdlf::PlaceHolders::_1,
                                 123),
            strand);

#if NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMDAS

        // Lamda bound to function, any strand.

        api::ReactorUtil::registerEventCallback([&](auto event) {
            BSLS_LOG_DEBUG("Processing reactor event %s in lamda 1",
                           ntca::ReactorEventType::toString(event.type()));
        });

        // Lamda bound to function, specific strand.

        api::ReactorUtil::registerEventCallback(
            [&](auto event) {
                BSLS_LOG_DEBUG("Processing reactor event %s in lamda 2",
                               ntca::ReactorEventType::toString(event.type()));
            },
            strand);

#endif
#endif

#if NTCI_CALLBACK_TEST_API_SUPPORTS_CALLBACK

        //
        // CALLBACKS
        //

        // Function pointer bound to callback, any strand.

        api::ReactorUtil::registerEventCallback(
            api::ReactorEventCallback(&api::ReactorUtil::processReactorEvent));

        // Function pointer bound to callback, specific strand.

        api::ReactorUtil::registerEventCallback(
            api::ReactorEventCallback(&api::ReactorUtil::processReactorEvent,
                                      strand));

        // Function binding bound to callback, any strand.

        api::ReactorUtil::registerEventCallback(api::ReactorEventCallback(
            bdlf::BindUtil::bind(&api::ReactorUtil::processReactorEventEx,
                                 bdlf::PlaceHolders::_1,
                                 123)));

        // Function binding bound to callback, specific strand.

        api::ReactorUtil::registerEventCallback(api::ReactorEventCallback(
            bdlf::BindUtil::bind(&api::ReactorUtil::processReactorEventEx,
                                 bdlf::PlaceHolders::_1,
                                 123),
            strand));

#if NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMDAS

        // Lamda bound to callback, any strand.

        api::ReactorUtil::registerEventCallback(
            api::ReactorEventCallback([&](auto event) {
                BSLS_LOG_DEBUG("Processing reactor event %s in lamda 3",
                               ntca::ReactorEventType::toString(event.type()));
            }));

        // Lamda bound to callback, specific strand.

        api::ReactorUtil::registerEventCallback(api::ReactorEventCallback(
            [&](auto event) {
                BSLS_LOG_DEBUG("Processing reactor event %s in lamda 4",
                               ntca::ReactorEventType::toString(event.type()));
            },
            strand));

        // Lamda bound to callback using factory function, any strand.

        {
            api::ReactorEventCallback callback =
                api::ReactorUtil::createReactorEventCallback([&](auto event) {
                    BSLS_LOG_DEBUG(
                        "Processing reactor event %s in lamda 5",
                        ntca::ReactorEventType::toString(event.type()));
                });

            api::ReactorUtil::registerEventCallback(callback);
        }

        // Lamda bound to callback using factory function, specific strand.

        {
            api::ReactorEventCallback callback =
                api::ReactorUtil::createReactorEventCallback(
                    [&](auto event) {
                        BSLS_LOG_DEBUG(
                            "Processing reactor event %s in lamda 6",
                            ntca::ReactorEventType::toString(event.type()));
                    },
                    strand);

            api::ReactorUtil::registerEventCallback(callback);
        }
#endif
#endif

#if NTCI_CALLBACK_TEST_API_SUPPORTS_INTERFACE

        //
        // Interfaces
        //

        bsl::shared_ptr<api::ReactorEventMechanism> mechanism;
        mechanism.createInplace(&ta);

        api::ReactorUtil::registerEventInterface(mechanism);

#endif

        strand->drain();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(6)
{
    // Concern: 0-arg callbacks deferred to executor.
    // Plan:

    typedef ntci::Callback<void()> CallbackArg0;

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;

        ntccfg::Mutex     mutex;
        ntccfg::LockGuard mutexGuard(&mutex);

        {
            CallbackArg0 callback(&ta);
            NTCCFG_TEST_FALSE(callback);
        }

        // Test *:*

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<ntci::Executor> executor =
                test::Executor::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0, &ta);

            error = callback.dispatch(ntci::Strand::unknown(),
                                      executor,
                                      false,
                                      &mutex);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 1);
        }

        // Test *:* (defer)

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<test::Executor> executor =
                test::Executor::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0, &ta);

            error = callback.dispatch(ntci::Strand::unknown(),
                                      executor,
                                      true,
                                      &mutex);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);

            callback.reset();
            executor->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 1);
        }

        // Test *:strand

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<ntci::Strand>   strand = test::Strand::create(&ta);
            bsl::shared_ptr<test::Executor> executor =
                test::Executor::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0, &ta);

            error = callback.dispatch(strand, executor, false, &mutex);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 1);
        }

        // Test *:strand (defer)

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<ntci::Strand>   strand = test::Strand::create(&ta);
            bsl::shared_ptr<test::Executor> executor =
                test::Executor::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0, &ta);

            error = callback.dispatch(strand, executor, true, &mutex);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);

            callback.reset();
            strand->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);

            executor->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 1);
        }

        // Test strand:*

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<ntci::Strand>   strand = test::Strand::create(&ta);
            bsl::shared_ptr<test::Executor> executor =
                test::Executor::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0, strand, &ta);

            error = callback.dispatch(ntci::Strand::unknown(),
                                      executor,
                                      false,
                                      &mutex);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);

            callback.reset();
            strand->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 1);
        }

        // Test strand:* (defer)

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<ntci::Strand>   strand = test::Strand::create(&ta);
            bsl::shared_ptr<test::Executor> executor =
                test::Executor::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0, strand, &ta);

            error = callback.dispatch(ntci::Strand::unknown(),
                                      executor,
                                      true,
                                      &mutex);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);

            callback.reset();
            executor->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);

            strand->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 1);
        }

        // Test strand:strand

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<ntci::Strand>   strand = test::Strand::create(&ta);
            bsl::shared_ptr<test::Executor> executor =
                test::Executor::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0, strand, &ta);

            error = callback.dispatch(strand, executor, false, &mutex);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 1);
        }

        // Test strand:strand (defer)

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<ntci::Strand>   strand = test::Strand::create(&ta);
            bsl::shared_ptr<test::Executor> executor =
                test::Executor::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0, strand, &ta);

            error = callback.dispatch(strand, executor, true, &mutex);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);

            callback.reset();
            executor->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);

            strand->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 1);
        }

        // Test strand1:strand2

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<ntci::Strand> strand1 = test::Strand::create(&ta);
            bsl::shared_ptr<ntci::Strand> strand2 = test::Strand::create(&ta);

            bsl::shared_ptr<test::Executor> executor =
                test::Executor::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0, strand1, &ta);

            error = callback.dispatch(strand2, executor, false, &mutex);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);

            callback.reset();
            strand1->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 1);
        }

        // Test strand1:strand2 (defer)

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<ntci::Strand> strand1 = test::Strand::create(&ta);
            bsl::shared_ptr<ntci::Strand> strand2 = test::Strand::create(&ta);

            bsl::shared_ptr<test::Executor> executor =
                test::Executor::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0, strand1, &ta);

            error = callback.dispatch(strand2, executor, true, &mutex);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);

            callback.reset();

            strand2->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);

            executor->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);

            strand1->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 1);
        }

        // Test cancellation.

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<test::Executor> executor =
                test::Executor::create(&ta);

            bsl::shared_ptr<ntci::Authorization> authorization =
                test::Authorization::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0,
                                  authorization,
                                  &ta);

            bool canceled = callback.abort();
            NTCCFG_TEST_TRUE(canceled);

            error = callback.dispatch(ntci::Strand::unknown(),
                                      executor,
                                      false,
                                      &mutex);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_CANCELLED);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);
        }

        // Test asynchronous cancellation.

        {
            test::callsToTargetFunctionArg0 = 0;

            bsl::shared_ptr<ntci::Authorization> authorization =
                test::Authorization::create(&ta);

            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            bsl::shared_ptr<test::Executor> executor =
                test::Executor::create(&ta);

            CallbackArg0 callback(&test::targetFunctionArg0,
                                  authorization,
                                  strand,
                                  &ta);

            error = callback.dispatch(ntci::Strand::unknown(),
                                      executor,
                                      false,
                                      &mutex);

            NTCCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);

            bool canceled = callback.abort();
            NTCCFG_TEST_TRUE(canceled);

            strand->drain();

            NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 0);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(7)
{
    // Concern: Callbacks may be invoked multiple times.

    ntccfg::TestAllocator ta;
    {
        typedef ntci::Callback<void()> CallbackArg0;

        ntsa::Error error;

        test::callsToTargetFunctionArg0 = 0;

        CallbackArg0 callback(&test::targetFunctionArg0, &ta);

        error = callback.execute(ntci::Strand::unknown());

        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 1);

        error = callback.execute(ntci::Strand::unknown());

        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 2);

        error = callback.execute(ntci::Strand::unknown());

        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTCCFG_TEST_EQ(test::callsToTargetFunctionArg0, 3);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

namespace example {

typedef NTCI_CALLBACK(const ntca::ReactorEvent& event) ReactorEventCallback;

void processReactorEvent(const ntca::ReactorEvent& event)
{
    NTCCFG_TEST_LOG_DEBUG << "Reactor event = " << event
                          << NTCCFG_TEST_LOG_END;
}

}  // close namespace example

NTCCFG_TEST_CASE(8)
{
    // Concern: Usage example.

    ntccfg::TestAllocator ta;
    {
        {
            example::ReactorEventCallback callback(
                &example::processReactorEvent,
                &ta);

            ntca::ReactorEvent reactorEvent;

            callback(reactorEvent, ntci::Strand::unknown());
        }

        {
            bsl::shared_ptr<ntci::Strand> strand = test::Strand::create(&ta);

            example::ReactorEventCallback callback(
                &example::processReactorEvent,
                strand,
                &ta);

            ntca::ReactorEvent reactorEvent;

            callback(reactorEvent, ntci::Strand::unknown());

            strand->drain();
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
    NTCCFG_TEST_REGISTER(5);
    NTCCFG_TEST_REGISTER(6);
    NTCCFG_TEST_REGISTER(7);
    NTCCFG_TEST_REGISTER(8);
}
NTCCFG_TEST_DRIVER_END;
