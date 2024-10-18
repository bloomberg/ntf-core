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

#include <ntscfg_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_callback_t_cpp, "$Id$ $CSID$")

#include <ntci_callback.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntci {

// Provide tests for 'ntci::Callback'.
class CallbackTest
{
  public:
    // Concern: 0-arg callbacks.
    static void verifyCase1();

    // Concern: 1-arg callbacks.
    static void verifyCase2();

    // Concern: 2-arg callbacks.
    static void verifyCase3();

    // Concern: 3-arg callbacks.
    static void verifyCase4();

    // Concern: 0-arg callbacks deferred to executor.
    static void verifyCase5();

    // Concern: Callbacks may be invoked multiple times.
    static void verifyCase6();

  private:
    /// Provide a mechanism to authorize the invocation of an operation. This
    /// class is thread safe.
    class Authorization;

    /// Provide an interface to guarantee sequential, non-concurrent execution.
    class Executor;

    /// Provide an interface to guarantee sequential, non-concurrent execution.
    class Strand;

    static void targetFunctionArg0();
    static void targetFunctionArg1(int arg1);
    static void targetFunctionArg2(int arg1, int arg2);
    static void targetFunctionArg3(int arg1, int arg2, int arg3);

    static const int ARG1;
    static const int ARG2;
    static const int ARG3;

    static bsl::size_t callsToTargetFunctionArg0;
    static bsl::size_t callsToTargetFunctionArg1;
    static bsl::size_t callsToTargetFunctionArg2;
    static bsl::size_t callsToTargetFunctionArg3;
};

const int CallbackTest::ARG1 = 1;
const int CallbackTest::ARG2 = 2;
const int CallbackTest::ARG3 = 3;

bsl::size_t CallbackTest::callsToTargetFunctionArg0 = 0;
bsl::size_t CallbackTest::callsToTargetFunctionArg1 = 0;
bsl::size_t CallbackTest::callsToTargetFunctionArg2 = 0;
bsl::size_t CallbackTest::callsToTargetFunctionArg3 = 0;

void CallbackTest::targetFunctionArg0()
{
    NTSCFG_TEST_LOG_DEBUG << "Executed f0" << NTSCFG_TEST_LOG_END;

    ++callsToTargetFunctionArg0;
}

void CallbackTest::targetFunctionArg1(int arg1)
{
    NTSCFG_TEST_LOG_DEBUG << "Executed f1" << NTSCFG_TEST_LOG_END;

    NTSCFG_TEST_EQ(arg1, CallbackTest::ARG1);

    ++callsToTargetFunctionArg1;
}

void CallbackTest::targetFunctionArg2(int arg1, int arg2)
{
    NTSCFG_TEST_LOG_DEBUG << "Executed f2" << NTSCFG_TEST_LOG_END;

    NTSCFG_TEST_EQ(arg1, CallbackTest::ARG1);
    NTSCFG_TEST_EQ(arg2, CallbackTest::ARG2);

    ++callsToTargetFunctionArg2;
}

void CallbackTest::targetFunctionArg3(int arg1, int arg2, int arg3)
{
    NTSCFG_TEST_LOG_DEBUG << "Executed f3" << NTSCFG_TEST_LOG_END;

    NTSCFG_TEST_EQ(arg1, CallbackTest::ARG1);
    NTSCFG_TEST_EQ(arg2, CallbackTest::ARG2);
    NTSCFG_TEST_EQ(arg3, CallbackTest::ARG3);

    ++callsToTargetFunctionArg3;
}

/// Provide a mechanism to authorize the invocation of an operation. This class
/// is thread safe.
class CallbackTest::Authorization : public ntci::Authorization
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
class CallbackTest::Executor : public ntci::Executor,
                               public ntccfg::Shared<Executor>
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
    static bsl::shared_ptr<CallbackTest::Executor> create(
        bslma::Allocator* basicAllocator = 0);
};

/// Provide an interface to guarantee sequential, non-concurrent
/// execution.
class CallbackTest::Strand : public ntci::Strand, public ntccfg::Shared<Strand>
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

CallbackTest::Authorization::Authorization()
: d_count(0)
, d_limit(bsl::numeric_limits<CountType>::max())
{
}

CallbackTest::Authorization::Authorization(CountType limit)
: d_count(0)
, d_limit(limit)
{
}

CallbackTest::Authorization::~Authorization()
{
}

void CallbackTest::Authorization::reset()
{
    d_count = 0;
}

ntsa::Error CallbackTest::Authorization::acquire()
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

ntsa::Error CallbackTest::Authorization::release()
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

bool CallbackTest::Authorization::abort()
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

CallbackTest::Authorization::CountType CallbackTest::Authorization::count()
    const
{
    return d_count.load();
}

bool CallbackTest::Authorization::canceled() const
{
    CountType c = d_count.load();
    return c < 0;
}

bsl::shared_ptr<ntci::Authorization> CallbackTest::Authorization::create(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<CallbackTest::Authorization> result;
    result.createInplace(allocator);

    return result;
}

bsl::shared_ptr<ntci::Authorization> CallbackTest::Authorization::create(
    bsl::size_t       limit,
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<CallbackTest::Authorization> result;
    result.createInplace(
        allocator,
        static_cast<CallbackTest::Authorization::CountType>(limit));

    return result;
}

CallbackTest::Executor::Executor(bslma::Allocator* basicAllocator)
: d_object("CallbackTest::Executor")
, d_functorQueueMutex()
, d_functorQueue(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

CallbackTest::Executor::~Executor()
{
    NTSCFG_TEST_TRUE(d_functorQueue.empty());
}

void CallbackTest::Executor::execute(const Functor& function)
{
    LockGuard lock(&d_functorQueueMutex);

    d_functorQueue.push_back(function);
}

void CallbackTest::Executor::moveAndExecute(FunctorSequence* functorSequence,
                                            const Functor&   functor)
{
    LockGuard lock(&d_functorQueueMutex);

    d_functorQueue.splice(d_functorQueue.end(), *functorSequence);
    if (functor) {
        d_functorQueue.push_back(functor);
    }
}

void CallbackTest::Executor::drain()
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

void CallbackTest::Executor::clear()
{
    LockGuard lock(&d_functorQueueMutex);

    d_functorQueue.clear();
}

bsl::shared_ptr<CallbackTest::Executor> CallbackTest::Executor::create(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<CallbackTest::Executor> strand;
    strand.createInplace(allocator, allocator);

    return strand;
}

CallbackTest::Strand::Strand(bslma::Allocator* basicAllocator)
: d_object("CallbackTest::Strand")
, d_functorQueueMutex()
, d_functorQueue(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

CallbackTest::Strand::~Strand()
{
    NTSCFG_TEST_TRUE(d_functorQueue.empty());
}

void CallbackTest::Strand::execute(const Functor& function)
{
    LockGuard lock(&d_functorQueueMutex);

    d_functorQueue.push_back(function);
}

void CallbackTest::Strand::moveAndExecute(FunctorSequence* functorSequence,
                                          const Functor&   functor)
{
    LockGuard lock(&d_functorQueueMutex);

    d_functorQueue.splice(d_functorQueue.end(), *functorSequence);
    if (functor) {
        d_functorQueue.push_back(functor);
    }
}

void CallbackTest::Strand::drain()
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

void CallbackTest::Strand::clear()
{
    LockGuard lock(&d_functorQueueMutex);

    d_functorQueue.clear();
}

bool CallbackTest::Strand::isRunningInCurrentThread() const
{
    const ntci::Strand* current = ntci::Strand::getThreadLocal();
    return (current == this);
}

bsl::shared_ptr<ntci::Strand> CallbackTest::Strand::create(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<CallbackTest::Strand> strand;
    strand.createInplace(allocator, allocator);

    return strand;
}

NTSCFG_TEST_FUNCTION(ntci::CallbackTest::verifyCase1)
{
    typedef ntci::Callback<void()> CallbackArg0;

    ntsa::Error error;

    {
        CallbackArg0 callback(NTSCFG_TEST_ALLOCATOR);
        NTSCFG_TEST_FALSE(callback);
    }

    // Test *:*

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(ntci::Strand::unknown());

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 1);
    }

    // Test *:strand

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(strand);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 1);
    }

    // Test strand:*

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              strand,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(ntci::Strand::unknown());

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);

        callback.reset();
        strand->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 1);
    }

    // Test strand:strand

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              strand,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(strand);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 1);
    }

    // Test strand1:strand2

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<ntci::Strand> strand1 =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<ntci::Strand> strand2 =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              strand1,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(strand2);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);

        callback.reset();
        strand1->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 1);
    }

    // Test cancellation.

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<ntci::Authorization> authorization =
            CallbackTest::Authorization::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              authorization,
                              NTSCFG_TEST_ALLOCATOR);

        bool canceled = callback.abort();
        NTSCFG_TEST_TRUE(canceled);

        error = callback.execute(ntci::Strand::unknown());

        NTSCFG_TEST_EQ(error, ntsa::Error::e_CANCELLED);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);
    }

    // Test asynchronous cancellation.

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<ntci::Authorization> authorization =
            CallbackTest::Authorization::create(NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              authorization,
                              strand,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(ntci::Strand::unknown());

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);

        bool canceled = callback.abort();
        NTSCFG_TEST_TRUE(canceled);

        strand->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);
    }
}

NTSCFG_TEST_FUNCTION(ntci::CallbackTest::verifyCase2)
{
    typedef ntci::Callback<void(int arg1)> CallbackArg1;

    ntsa::Error error;

    {
        CallbackArg1 callback(NTSCFG_TEST_ALLOCATOR);
        NTSCFG_TEST_FALSE(callback);
    }

    // Test *:*

    {
        CallbackTest::callsToTargetFunctionArg1 = 0;

        CallbackArg1 callback(&CallbackTest::targetFunctionArg1,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(CallbackTest::ARG1, ntci::Strand::unknown());

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg1, 1);
    }

    // Test *:strand

    {
        CallbackTest::callsToTargetFunctionArg1 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg1 callback(&CallbackTest::targetFunctionArg1,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(CallbackTest::ARG1, strand);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg1, 1);
    }

    // Test strand:*

    {
        CallbackTest::callsToTargetFunctionArg1 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg1 callback(&CallbackTest::targetFunctionArg1,
                              strand,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(CallbackTest::ARG1, ntci::Strand::unknown());

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg1, 0);

        callback.reset();
        strand->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg1, 1);
    }

    // Test strand:strand

    {
        CallbackTest::callsToTargetFunctionArg1 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg1 callback(&CallbackTest::targetFunctionArg1,
                              strand,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(CallbackTest::ARG1, strand);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg1, 1);
    }

    // Test strand1:strand2

    {
        CallbackTest::callsToTargetFunctionArg1 = 0;

        bsl::shared_ptr<ntci::Strand> strand1 =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<ntci::Strand> strand2 =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg1 callback(&CallbackTest::targetFunctionArg1,
                              strand1,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(CallbackTest::ARG1, strand2);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg1, 0);

        callback.reset();
        strand1->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg1, 1);
    }

    // Test cancellation.

    {
        CallbackTest::callsToTargetFunctionArg1 = 0;

        bsl::shared_ptr<ntci::Authorization> authorization =
            CallbackTest::Authorization::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg1 callback(&CallbackTest::targetFunctionArg1,
                              authorization,
                              NTSCFG_TEST_ALLOCATOR);

        bool canceled = callback.abort();
        NTSCFG_TEST_TRUE(canceled);

        error = callback.execute(CallbackTest::ARG1, ntci::Strand::unknown());

        NTSCFG_TEST_EQ(error, ntsa::Error::e_CANCELLED);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg1, 0);
    }

    // Test asynchronous cancellation.

    {
        CallbackTest::callsToTargetFunctionArg1 = 0;

        bsl::shared_ptr<ntci::Authorization> authorization =
            CallbackTest::Authorization::create(NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg1 callback(&CallbackTest::targetFunctionArg1,
                              authorization,
                              strand,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(CallbackTest::ARG1, ntci::Strand::unknown());

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg1, 0);

        bool canceled = callback.abort();
        NTSCFG_TEST_TRUE(canceled);

        strand->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg1, 0);
    }
}

NTSCFG_TEST_FUNCTION(ntci::CallbackTest::verifyCase3)
{
    typedef ntci::Callback<void(int arg1, int arg2)> CallbackArg2;

    ntsa::Error error;

    {
        CallbackArg2 callback(NTSCFG_TEST_ALLOCATOR);
        NTSCFG_TEST_FALSE(callback);
    }

    // Test *:*

    {
        CallbackTest::callsToTargetFunctionArg2 = 0;

        CallbackArg2 callback(&CallbackTest::targetFunctionArg2,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(CallbackTest::ARG1,
                                 CallbackTest::ARG2,
                                 ntci::Strand::unknown());

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg2, 1);
    }

    // Test *:strand

    {
        CallbackTest::callsToTargetFunctionArg2 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg2 callback(&CallbackTest::targetFunctionArg2,
                              NTSCFG_TEST_ALLOCATOR);

        error =
            callback.execute(CallbackTest::ARG1, CallbackTest::ARG2, strand);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg2, 1);
    }

    // Test strand:*

    {
        CallbackTest::callsToTargetFunctionArg2 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg2 callback(&CallbackTest::targetFunctionArg2,
                              strand,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(CallbackTest::ARG1,
                                 CallbackTest::ARG2,
                                 ntci::Strand::unknown());

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg2, 0);

        callback.reset();
        strand->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg2, 1);
    }

    // Test strand:strand

    {
        CallbackTest::callsToTargetFunctionArg2 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg2 callback(&CallbackTest::targetFunctionArg2,
                              strand,
                              NTSCFG_TEST_ALLOCATOR);

        error =
            callback.execute(CallbackTest::ARG1, CallbackTest::ARG2, strand);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg2, 1);
    }

    // Test strand1:strand2

    {
        CallbackTest::callsToTargetFunctionArg2 = 0;

        bsl::shared_ptr<ntci::Strand> strand1 =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<ntci::Strand> strand2 =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg2 callback(&CallbackTest::targetFunctionArg2,
                              strand1,
                              NTSCFG_TEST_ALLOCATOR);

        error =
            callback.execute(CallbackTest::ARG1, CallbackTest::ARG2, strand2);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg2, 0);

        callback.reset();
        strand1->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg2, 1);
    }

    // Test cancellation.

    {
        CallbackTest::callsToTargetFunctionArg2 = 0;

        bsl::shared_ptr<ntci::Authorization> authorization =
            CallbackTest::Authorization::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg2 callback(&CallbackTest::targetFunctionArg2,
                              authorization,
                              NTSCFG_TEST_ALLOCATOR);

        bool canceled = callback.abort();
        NTSCFG_TEST_TRUE(canceled);

        error = callback.execute(CallbackTest::ARG1,
                                 CallbackTest::ARG2,
                                 ntci::Strand::unknown());

        NTSCFG_TEST_EQ(error, ntsa::Error::e_CANCELLED);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg2, 0);
    }

    // Test asynchronous cancellation.

    {
        CallbackTest::callsToTargetFunctionArg2 = 0;

        bsl::shared_ptr<ntci::Authorization> authorization =
            CallbackTest::Authorization::create(NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg2 callback(&CallbackTest::targetFunctionArg2,
                              authorization,
                              strand,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(CallbackTest::ARG1,
                                 CallbackTest::ARG2,
                                 ntci::Strand::unknown());

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg2, 0);

        bool canceled = callback.abort();
        NTSCFG_TEST_TRUE(canceled);

        strand->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg2, 0);
    }
}

NTSCFG_TEST_FUNCTION(ntci::CallbackTest::verifyCase4)
{
    typedef ntci::Callback<void(int arg1, int arg2, int arg3)> CallbackArg3;

    ntsa::Error error;

    {
        CallbackArg3 callback(NTSCFG_TEST_ALLOCATOR);
        NTSCFG_TEST_FALSE(callback);
    }

    // Test *:*

    {
        CallbackTest::callsToTargetFunctionArg3 = 0;

        CallbackArg3 callback(&CallbackTest::targetFunctionArg3,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(CallbackTest::ARG1,
                                 CallbackTest::ARG2,
                                 CallbackTest::ARG3,
                                 ntci::Strand::unknown());

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg3, 1);
    }

    // Test *:strand

    {
        CallbackTest::callsToTargetFunctionArg3 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg3 callback(&CallbackTest::targetFunctionArg3,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(CallbackTest::ARG1,
                                 CallbackTest::ARG2,
                                 CallbackTest::ARG3,
                                 strand);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg3, 1);
    }

    // Test strand:*

    {
        CallbackTest::callsToTargetFunctionArg3 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg3 callback(&CallbackTest::targetFunctionArg3,
                              strand,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(CallbackTest::ARG1,
                                 CallbackTest::ARG2,
                                 CallbackTest::ARG3,
                                 ntci::Strand::unknown());

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg3, 0);

        callback.reset();
        strand->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg3, 1);
    }

    // Test strand:strand

    {
        CallbackTest::callsToTargetFunctionArg3 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        ntci::Callback<void(int arg1, int arg2, int arg3)> callback(
            &CallbackTest::targetFunctionArg3,
            strand,
            NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(CallbackTest::ARG1,
                                 CallbackTest::ARG2,
                                 CallbackTest::ARG3,
                                 strand);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg3, 1);
    }

    // Test strand1:strand2

    {
        CallbackTest::callsToTargetFunctionArg3 = 0;

        bsl::shared_ptr<ntci::Strand> strand1 =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<ntci::Strand> strand2 =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg3 callback(&CallbackTest::targetFunctionArg3,
                              strand1,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(CallbackTest::ARG1,
                                 CallbackTest::ARG2,
                                 CallbackTest::ARG3,
                                 strand2);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg3, 0);

        callback.reset();
        strand1->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg3, 1);
    }

    // Test cancellation.

    {
        CallbackTest::callsToTargetFunctionArg3 = 0;

        bsl::shared_ptr<ntci::Authorization> authorization =
            CallbackTest::Authorization::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg3 callback(&CallbackTest::targetFunctionArg3,
                              authorization,
                              NTSCFG_TEST_ALLOCATOR);

        bool canceled = callback.abort();
        NTSCFG_TEST_TRUE(canceled);

        error = callback.execute(CallbackTest::ARG1,
                                 CallbackTest::ARG2,
                                 CallbackTest::ARG3,
                                 ntci::Strand::unknown());

        NTSCFG_TEST_EQ(error, ntsa::Error::e_CANCELLED);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg3, 0);
    }

    // Test asynchronous cancellation.

    {
        CallbackTest::callsToTargetFunctionArg3 = 0;

        bsl::shared_ptr<ntci::Authorization> authorization =
            CallbackTest::Authorization::create(NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg3 callback(&CallbackTest::targetFunctionArg3,
                              authorization,
                              strand,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.execute(CallbackTest::ARG1,
                                 CallbackTest::ARG2,
                                 CallbackTest::ARG3,
                                 ntci::Strand::unknown());

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg3, 0);

        bool canceled = callback.abort();
        NTSCFG_TEST_TRUE(canceled);

        strand->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg3, 0);
    }
}

NTSCFG_TEST_FUNCTION(ntci::CallbackTest::verifyCase5)
{
    typedef ntci::Callback<void()> CallbackArg0;

    ntsa::Error error;

    ntccfg::Mutex     mutex;
    ntccfg::LockGuard mutexGuard(&mutex);

    {
        CallbackArg0 callback(NTSCFG_TEST_ALLOCATOR);
        NTSCFG_TEST_FALSE(callback);
    }

    // Test *:*

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<ntci::Executor> executor =
            CallbackTest::Executor::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.dispatch(ntci::Strand::unknown(),
                                  executor,
                                  false,
                                  &mutex);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 1);
    }

    // Test *:* (defer)

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<CallbackTest::Executor> executor =
            CallbackTest::Executor::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              NTSCFG_TEST_ALLOCATOR);

        error =
            callback.dispatch(ntci::Strand::unknown(), executor, true, &mutex);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);

        callback.reset();
        executor->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 1);
    }

    // Test *:strand

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);
        bsl::shared_ptr<CallbackTest::Executor> executor =
            CallbackTest::Executor::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.dispatch(strand, executor, false, &mutex);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 1);
    }

    // Test *:strand (defer)

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);
        bsl::shared_ptr<CallbackTest::Executor> executor =
            CallbackTest::Executor::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.dispatch(strand, executor, true, &mutex);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);

        callback.reset();
        strand->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);

        executor->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 1);
    }

    // Test strand:*

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);
        bsl::shared_ptr<CallbackTest::Executor> executor =
            CallbackTest::Executor::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              strand,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.dispatch(ntci::Strand::unknown(),
                                  executor,
                                  false,
                                  &mutex);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);

        callback.reset();
        strand->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 1);
    }

    // Test strand:* (defer)

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);
        bsl::shared_ptr<CallbackTest::Executor> executor =
            CallbackTest::Executor::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              strand,
                              NTSCFG_TEST_ALLOCATOR);

        error =
            callback.dispatch(ntci::Strand::unknown(), executor, true, &mutex);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);

        callback.reset();
        executor->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);

        strand->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 1);
    }

    // Test strand:strand

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);
        bsl::shared_ptr<CallbackTest::Executor> executor =
            CallbackTest::Executor::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              strand,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.dispatch(strand, executor, false, &mutex);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 1);
    }

    // Test strand:strand (defer)

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);
        bsl::shared_ptr<CallbackTest::Executor> executor =
            CallbackTest::Executor::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              strand,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.dispatch(strand, executor, true, &mutex);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);

        callback.reset();
        executor->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);

        strand->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 1);
    }

    // Test strand1:strand2

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<ntci::Strand> strand1 =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);
        bsl::shared_ptr<ntci::Strand> strand2 =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<CallbackTest::Executor> executor =
            CallbackTest::Executor::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              strand1,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.dispatch(strand2, executor, false, &mutex);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);

        callback.reset();
        strand1->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 1);
    }

    // Test strand1:strand2 (defer)

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<ntci::Strand> strand1 =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);
        bsl::shared_ptr<ntci::Strand> strand2 =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<CallbackTest::Executor> executor =
            CallbackTest::Executor::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              strand1,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.dispatch(strand2, executor, true, &mutex);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);

        callback.reset();

        strand2->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);

        executor->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);

        strand1->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 1);
    }

    // Test cancellation.

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<CallbackTest::Executor> executor =
            CallbackTest::Executor::create(NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<ntci::Authorization> authorization =
            CallbackTest::Authorization::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              authorization,
                              NTSCFG_TEST_ALLOCATOR);

        bool canceled = callback.abort();
        NTSCFG_TEST_TRUE(canceled);

        error = callback.dispatch(ntci::Strand::unknown(),
                                  executor,
                                  false,
                                  &mutex);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_CANCELLED);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);
    }

    // Test asynchronous cancellation.

    {
        CallbackTest::callsToTargetFunctionArg0 = 0;

        bsl::shared_ptr<ntci::Authorization> authorization =
            CallbackTest::Authorization::create(NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<ntci::Strand> strand =
            CallbackTest::Strand::create(NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<CallbackTest::Executor> executor =
            CallbackTest::Executor::create(NTSCFG_TEST_ALLOCATOR);

        CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                              authorization,
                              strand,
                              NTSCFG_TEST_ALLOCATOR);

        error = callback.dispatch(ntci::Strand::unknown(),
                                  executor,
                                  false,
                                  &mutex);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_PENDING);
        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);

        bool canceled = callback.abort();
        NTSCFG_TEST_TRUE(canceled);

        strand->drain();

        NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 0);
    }
}

NTSCFG_TEST_FUNCTION(ntci::CallbackTest::verifyCase6)
{
    typedef ntci::Callback<void()> CallbackArg0;

    ntsa::Error error;

    CallbackTest::callsToTargetFunctionArg0 = 0;

    CallbackArg0 callback(&CallbackTest::targetFunctionArg0,
                          NTSCFG_TEST_ALLOCATOR);

    error = callback.execute(ntci::Strand::unknown());

    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
    NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 1);

    error = callback.execute(ntci::Strand::unknown());

    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
    NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 2);

    error = callback.execute(ntci::Strand::unknown());

    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
    NTSCFG_TEST_EQ(CallbackTest::callsToTargetFunctionArg0, 3);
}

}  // close namespace ntci
}  // close namespace BloombergLP
