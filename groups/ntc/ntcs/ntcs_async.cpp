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

#include <ntcs_async.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_async_cpp, "$Id$ $CSID$")

#include <bdlmt_threadpool.h>
#include <bdlt_currenttime.h>
#include <bslmt_qlock.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsl_cstdlib.h>

namespace BloombergLP {
namespace ntcs {

namespace {

const int k_MIN_THREADS   = 0;
const int k_MAX_THREADS   = 1;
const int k_MAX_IDLE_TIME = 10;

bslmt::QLock       s_threadPoolLock        = BSLMT_QLOCK_INITIALIZER;
bdlmt::ThreadPool* s_threadPool_p          = 0;
bslma::Allocator*  s_threadPoolAllocator_p = 0;
bool               s_threadPoolDestroyed   = false;
bool               s_threadPoolExit        = false;

}  // close unnamed namespace

ntsa::Error Async::privateInitialize()
{
    if (s_threadPool_p == 0) {
        BSLS_ASSERT_OPT(!s_threadPoolDestroyed);

        BSLS_ASSERT_OPT(s_threadPoolAllocator_p == 0);
        s_threadPoolAllocator_p = bslma::Default::globalAllocator();

        void* arena =
            s_threadPoolAllocator_p->allocate(sizeof(bdlmt::ThreadPool));

        bslmt::ThreadAttributes threadAttributes;
        threadAttributes.setThreadName("async");

        s_threadPool_p =
            new (arena) bdlmt::ThreadPool(threadAttributes,
                                          k_MIN_THREADS,
                                          k_MAX_THREADS,
                                          k_MAX_IDLE_TIME,
                                          s_threadPoolAllocator_p);

        int rc = s_threadPool_p->start();
        if (rc != 0) {
            typedef bdlmt::ThreadPool Type;
            s_threadPool_p->~Type();

            s_threadPoolAllocator_p->deallocate(s_threadPool_p);

            s_threadPool_p          = 0;
            s_threadPoolAllocator_p = 0;

            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (!s_threadPoolExit) {
            bsl::atexit(&Async::exit);
            s_threadPoolExit = true;
        }
    }

    return ntsa::Error();
}

void Async::privateExit()
{
    if (s_threadPool_p != 0) {
        s_threadPool_p->stop();

        typedef bdlmt::ThreadPool Type;
        s_threadPool_p->~Type();

        BSLS_ASSERT_OPT(s_threadPoolAllocator_p);
        s_threadPoolAllocator_p->deallocate(s_threadPool_p);

        s_threadPool_p          = 0;
        s_threadPoolAllocator_p = 0;
        s_threadPoolDestroyed   = true;
    }
}

ntsa::Error Async::initialize()
{
    bslmt::QLockGuard guard(&s_threadPoolLock);

    return Async::privateInitialize();
}

bsl::shared_ptr<ntci::Strand> Async::createStrand(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcs::AsyncStrand> result;
    result.createInplace(allocator, allocator);
    return result;
}

bsl::shared_ptr<ntci::Timer> Async::createTimer(
    const ntca::TimerOptions&                  options,
    const bsl::shared_ptr<ntci::TimerSession>& session,
    bslma::Allocator*                          basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcs::AsyncTimer> result;
    result.createInplace(allocator, options, session, allocator);
    return result;
}

bsl::shared_ptr<ntci::Timer> Async::createTimer(
    const ntca::TimerOptions&  options,
    const ntci::TimerCallback& callback,
    bslma::Allocator*          basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcs::AsyncTimer> result;
    result.createInplace(allocator, options, callback, allocator);
    return result;
}

void Async::execute(const Functor& function)
{
    bslmt::QLockGuard guard(&s_threadPoolLock);

    ntsa::Error error = Async::privateInitialize();
    BSLS_ASSERT_OPT(!error);

    BSLS_ASSERT_OPT(s_threadPool_p);

    int rc = s_threadPool_p->enqueueJob(function);
    BSLS_ASSERT_OPT(rc == 0);
}

void Async::moveAndExecute(FunctorSequence* functorSequence,
                           const Functor&   functor)
{
    bslmt::QLockGuard guard(&s_threadPoolLock);

    ntsa::Error error = Async::privateInitialize();
    BSLS_ASSERT_OPT(!error);

    BSLS_ASSERT_OPT(s_threadPool_p);

    for (FunctorSequence::const_iterator it = functorSequence->begin();
         it != functorSequence->end();
         ++it)
    {
        int rc = s_threadPool_p->enqueueJob(*it);
        BSLS_ASSERT_OPT(rc == 0);
    }

    functorSequence->clear();

    {
        int rc = s_threadPool_p->enqueueJob(functor);
        BSLS_ASSERT_OPT(rc == 0);
    }
}

bsls::TimeInterval Async::currentTime()
{
    return bdlt::CurrentTime::now();
}

void Async::exit()
{
    bslmt::QLockGuard guard(&s_threadPoolLock);

    Async::privateExit();
}

void AsyncStrand::invoke()
{
    while (true) {
        bdlb::NullableValue<FunctorQueue> functorQueue(d_allocator_p);
        {
            LockGuard lock(&d_functorQueueMutex);

            BSLS_ASSERT(d_pending);

            if (!d_functorQueue.empty()) {
                functorQueue.makeValue().swap(d_functorQueue);
            }
            else {
                d_pending = false;
                break;
            }
        }

        FunctorQueue::iterator it = functorQueue.value().begin();
        FunctorQueue::iterator et = functorQueue.value().end();

        {
            ntci::StrandGuard strandGuard(this);

            while (it != et) {
                (*it)();
                ++it;
            }
        }
    }
}

AsyncStrand::AsyncStrand(bslma::Allocator* basicAllocator)
: d_object("ntcs::AsyncStrand")
, d_functorQueueMutex(NTCCFG_LOCK_INIT)
, d_functorQueue(basicAllocator)
, d_pending(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

AsyncStrand::~AsyncStrand()
{
    BSLS_ASSERT(d_functorQueue.empty());
}

void AsyncStrand::execute(const Functor& function)
{
    bool activate = false;
    {
        LockGuard lock(&d_functorQueueMutex);

        d_functorQueue.push_back(function);

        if (!d_pending) {
            d_pending = true;
            activate  = true;
        }
    }

    if (activate) {
        ntcs::Async::execute(
            NTCCFG_BIND(&AsyncStrand::invoke, this->getSelf(this)));
    }
}

void AsyncStrand::moveAndExecute(FunctorSequence* functorSequence,
                                 const Functor&   functor)
{
    bool activate = false;
    {
        LockGuard lock(&d_functorQueueMutex);

        d_functorQueue.splice(d_functorQueue.end(), *functorSequence);
        if (functor) {
            d_functorQueue.push_back(functor);
        }

        if (!d_pending) {
            d_pending = true;
            activate  = true;
        }
    }

    if (activate) {
        ntcs::Async::execute(
            NTCCFG_BIND(&AsyncStrand::invoke, this->getSelf(this)));
    }
}

void AsyncStrand::drain()
{
    while (true) {
        bdlb::NullableValue<FunctorQueue> functorQueue(d_allocator_p);
        {
            LockGuard lock(&d_functorQueueMutex);

            BSLS_ASSERT(!d_pending);

            if (!d_functorQueue.empty()) {
                functorQueue.makeValue().swap(d_functorQueue);
            }
            else {
                break;
            }
        }

        FunctorQueue::iterator it = functorQueue.value().begin();
        FunctorQueue::iterator et = functorQueue.value().end();

        {
            ntci::StrandGuard strandGuard(this);

            while (it != et) {
                (*it)();
                ++it;
            }
        }
    }
}

void AsyncStrand::clear()
{
    LockGuard lock(&d_functorQueueMutex);

    d_functorQueue.clear();
}

bool AsyncStrand::isRunningInCurrentThread() const
{
    const ntci::Strand* current = ntci::Strand::getThreadLocal();
    return (current == this);
}

AsyncTimer::AsyncTimer(const ntca::TimerOptions&                  options,
                       const bsl::shared_ptr<ntci::TimerSession>& session,
                       bslma::Allocator* basicAllocator)
: d_options(options)
, d_callback(basicAllocator)
, d_session_sp(session)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

AsyncTimer::AsyncTimer(const ntca::TimerOptions&  options,
                       const ntci::TimerCallback& callback,
                       bslma::Allocator*          basicAllocator)
: d_options(options)
, d_callback(callback, basicAllocator)
, d_session_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

AsyncTimer::~AsyncTimer()
{
}

ntsa::Error AsyncTimer::schedule(const bsls::TimeInterval& deadline,
                                 const bsls::TimeInterval& period)
{
    NTCCFG_WARNING_UNUSED(deadline);
    NTCCFG_WARNING_UNUSED(period);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AsyncTimer::cancel()
{
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AsyncTimer::close()
{
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

void AsyncTimer::arrive(const bsl::shared_ptr<ntci::Timer>& self,
                        const bsls::TimeInterval&           now,
                        const bsls::TimeInterval&           deadline)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(now);
    NTCCFG_WARNING_UNUSED(deadline);
}

void* AsyncTimer::handle() const
{
    return d_options.handle();
}

int AsyncTimer::id() const
{
    return d_options.id();
}

bool AsyncTimer::oneShot() const
{
    return d_options.oneShot();
}

bdlb::NullableValue<bsls::TimeInterval> AsyncTimer::deadline() const
{
    return bdlb::NullableValue<bsls::TimeInterval>();
}

bdlb::NullableValue<bsls::TimeInterval> AsyncTimer::period() const
{
    return bdlb::NullableValue<bsls::TimeInterval>();
}

bslmt::ThreadUtil::Handle AsyncTimer::threadHandle() const
{
    return bslmt::ThreadUtil::invalidHandle();
}

bsl::size_t AsyncTimer::threadIndex() const
{
    return 0;
}

const bsl::shared_ptr<ntci::Strand>& AsyncTimer::strand() const
{
    if (NTCCFG_LIKELY(d_callback)) {
        return d_callback.strand();
    }
    else if (d_session_sp) {
        return d_session_sp->strand();
    }
    else {
        return ntci::Strand::unspecified();
    }
}

bsls::TimeInterval AsyncTimer::currentTime() const
{
    return Async::currentTime();
}

}  // close package namespace
}  // close enterprise namespace
