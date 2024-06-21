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

#include <ntcs_strand.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_strand_cpp, "$Id$ $CSID$")

#include <ntccfg_bind.h>
#include <ntcs_async.h>
#include <bdlb_nullablevalue.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_log.h>

// Uncomment to enable logging from this component.
// #define NTCS_STRAND_LOG 1

#if NTCS_STRAND_LOG

#define NTCS_STRAND_LOG_QUEUE_PUSHED(strandAddress, functorQueue, pending)    \
    BSLS_LOG_INFO(                                                            \
        "Strand %p pushed function onto queue, size = %d, pending = %d",      \
        (strandAddress),                                                      \
        (int)((functorQueue).size()),                                         \
        (int)(pending));

#define NTCS_STRAND_LOG_QUEUE_POPPED(strandAddress, functorQueue)             \
    BSLS_LOG_INFO("Strand %p popped %d functions from queue",                 \
                  (strandAddress),                                            \
                  (int)((functorQueue).size()));

#define NTCS_STRAND_LOG_QUEUE_EMPTY(strandAddress)                            \
    BSLS_LOG_INFO("Strand %p is now empty", (strandAddress));

#define NTCS_STRAND_LOG_EXECUTION_STARTING(strandAddress, functorQueue)       \
    BSLS_LOG_INFO("Strand %p execution starting for %d functions",            \
                  (strandAddress),                                            \
                  (int)((functorQueue).size()));

#define NTCS_STRAND_LOG_EXECUTION_COMPLETE(strandAddress, functorQueue)       \
    BSLS_LOG_INFO("Strand %p execution complete for %d functions",            \
                  (strandAddress),                                            \
                  (int)((functorQueue).size()));

#define NTCS_STRAND_LOG_ACTIVATION(strandAddress)                             \
    BSLS_LOG_INFO("Strand %p activating itself in its reactor",               \
                  (strandAddress));

#else

#define NTCS_STRAND_LOG_QUEUE_PUSHED(strandAddress, functorQueue, pending)
#define NTCS_STRAND_LOG_QUEUE_POPPED(strandAddress, functorQueue)
#define NTCS_STRAND_LOG_QUEUE_EMPTY(strandAddress)
#define NTCS_STRAND_LOG_EXECUTION_STARTING(strandAddress, functorQueue)
#define NTCS_STRAND_LOG_EXECUTION_COMPLETE(strandAddress, functorQueue)
#define NTCS_STRAND_LOG_ACTIVATION(strandAddress)

#endif

// The compile-time constant to implement the strand using a greedy algorithm,
// that maximizes throughput at the expense of fairness.
#define NTCS_STRAND_IMP_GREEDY 1

// The compile-time constant to implement the strand using a fair algorithm,
// that will more fairly allow functors on the strand to be executed by
// different threads (and allows functors from other threads to more fairly
// utilize those threads) at the expense of throughput.
#define NTCS_STRAND_IMP_FAIR 2

// Define the strand implementation algorithm.
#define NTCS_STRAND_IMP NTCS_STRAND_IMP_GREEDY

// Some versions of GCC erroneously warn ntcs::ObserverRef::d_shared may be
// uninitialized.
#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

// IMPLEMENTATION NOTES: Testing indicates that, with 10 threads driving the
// reactor utilized by a strand, fair algorithm achieves 250,000 functors per
// second, evenly distributed across all threads, while the greedy algorithm
// achieves 2,000,000 functors per second, but typically only runs on three or
// four threads.

namespace BloombergLP {
namespace ntcs {

void Strand::invoke()
{
#if (NTCS_STRAND_IMP == NTCS_STRAND_IMP_GREEDY)

    while (true) {
        bdlb::NullableValue<FunctorQueue> functorQueue(d_allocator_p);
        {
            LockGuard lock(&d_functorQueueMutex);

            BSLS_ASSERT(d_pending);

            if (!d_functorQueue.empty()) {
                functorQueue.makeValue().swap(d_functorQueue);
                NTCS_STRAND_LOG_QUEUE_POPPED(this, functorQueue.value());
            }
            else {
                NTCS_STRAND_LOG_QUEUE_EMPTY(this);
                d_pending = false;
                break;
            }
        }

        NTCS_STRAND_LOG_EXECUTION_STARTING(this, functorQueue.value());

        FunctorQueue::iterator it = functorQueue.value().begin();
        FunctorQueue::iterator et = functorQueue.value().end();

        {
            ntci::StrandGuard strandGuard(this);

            while (it != et) {
                (*it)();
                ++it;
            }
        }

        NTCS_STRAND_LOG_EXECUTION_COMPLETE(this, functorQueue.value());
    }

#elif (NTCS_STRAND_IMP == NTCS_STRAND_IMP_FAIR)

    Functor functor(NTCCFG_FUNCTION_INIT(d_allocator_p));
    bool    activate = false;

    {
        LockGuard lock(&d_functorQueueMutex);

        BSLS_ASSERT(d_pending);
        BSLS_ASSERT(!d_functorQueue.empty());

        functor.swap(d_functorQueue.front());

        d_functorQueue.pop_front();

        activate  = !d_functorQueue.empty();
        d_pending = activate;
    }

    {
        ntci::StrandGuard strandGuard(this);

        functor();
    }

    if (activate) {
        ntcs::ObserverRef<ntci::Executor> executorRef(&d_executor);
        if (executorRef) {
            executorRef->execute(
                NTCCFG_BIND(&Strand::invoke, this->getSelf(this)));
        }
        else {
            ntcs::Async::execute(
                NTCCFG_BIND(&Strand::invoke, this->getSelf(this)));
        }
    }

#else
#error Not implemented
#endif
}

Strand::Strand(const bsl::shared_ptr<ntci::Executor>& executor,
               bslma::Allocator*                      basicAllocator)
: d_object("ntcs::Strand")
, d_functorQueueMutex(NTCCFG_LOCK_INIT)
, d_functorQueue(basicAllocator)
, d_executor(bsl::weak_ptr<ntci::Executor>(executor))
, d_pending(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Strand::~Strand()
{
    BSLS_ASSERT(d_functorQueue.empty());
}

void Strand::execute(const Functor& function)
{
    bool activate = false;
    {
        LockGuard lock(&d_functorQueueMutex);

        d_functorQueue.push_back(function);

        NTCS_STRAND_LOG_QUEUE_PUSHED(this, d_functorQueue, d_pending);

        if (!d_pending) {
            d_pending = true;
            activate  = true;
        }
    }

    if (activate) {
        NTCS_STRAND_LOG_ACTIVATION(this);

        ntcs::ObserverRef<ntci::Executor> executorRef(&d_executor);
        if (executorRef) {
            executorRef->execute(
                NTCCFG_BIND(&Strand::invoke, this->getSelf(this)));
        }
        else {
            ntcs::Async::execute(
                NTCCFG_BIND(&Strand::invoke, this->getSelf(this)));
        }
    }
}

void Strand::moveAndExecute(FunctorSequence* functorSequence,
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
        ntcs::ObserverRef<ntci::Executor> executorRef(&d_executor);
        if (executorRef) {
            executorRef->execute(
                NTCCFG_BIND(&Strand::invoke, this->getSelf(this)));
        }
        else {
            ntcs::Async::execute(
                NTCCFG_BIND(&Strand::invoke, this->getSelf(this)));
        }
    }
}

void Strand::drain()
{
    while (true) {
        bdlb::NullableValue<FunctorQueue> functorQueue(d_allocator_p);
        {
            LockGuard lock(&d_functorQueueMutex);

            BSLS_ASSERT(!d_pending);

            if (!d_functorQueue.empty()) {
                functorQueue.makeValue().swap(d_functorQueue);
                NTCS_STRAND_LOG_QUEUE_POPPED(this, functorQueue.value());
            }
            else {
                NTCS_STRAND_LOG_QUEUE_EMPTY(this);
                break;
            }
        }

        NTCS_STRAND_LOG_EXECUTION_STARTING(this, functorQueue.value());

        FunctorQueue::iterator it = functorQueue.value().begin();
        FunctorQueue::iterator et = functorQueue.value().end();

        {
            ntci::StrandGuard strandGuard(this);

            while (it != et) {
                (*it)();
                ++it;
            }
        }

        NTCS_STRAND_LOG_EXECUTION_COMPLETE(this, functorQueue.value());
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

}  // close package namespace
}  // close enterprise namespace
