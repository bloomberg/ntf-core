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

#include <ntci_log.h>
#include <ntcs_chronology.h>
#include <ntcs_strand.h>
#include <ntcs_threadutil.h>
#include <bdlt_currenttime.h>
#include <bslmt_once.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsl_cstdlib.h>

namespace BloombergLP {
namespace ntcs {

/// Provide global asynchronous state.
class Async::State : public ntci::Executor,
                     public ntci::TimerFactory,
                     public ntcs::Interruptor
{
  public:
    /// Create new asynchronous state. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit State(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~State();

    /// Execute the function on a thread managed by the process-wide
    /// asynchronous executor.
    void execute(const Functor& function) BSLS_KEYWORD_OVERRIDE;

    /// Atomically defer the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE;

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'session' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&                  options,
        const bsl::shared_ptr<ntci::TimerSession>& session,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'callback' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&  options,
        const ntci::TimerCallback& callback,
        bslma::Allocator*          basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Return the strand on which this object's functions should be called.
    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current elapsed time since the Unix epoch.
    bsls::TimeInterval currentTime() const BSLS_KEYWORD_OVERRIDE;

    /// Return the memory allocator.
    bslma::Allocator* allocator() const;

    /// Return the singleton object.
    static State& singleton();

  private:
    /// Unblock one waiter blocked on 'wait'.
    void interruptOne() BSLS_KEYWORD_OVERRIDE;

    /// Unblock all waiters blocked on 'wait'.
    void interruptAll() BSLS_KEYWORD_OVERRIDE;

    /// Return the handle of the thread that will be calling 'wait()', or
    /// the default value if no such thread has been set.
    bslmt::ThreadUtil::Handle threadHandle() const BSLS_KEYWORD_OVERRIDE;

    /// Return the index of the thread that will be calling 'wait()', or
    /// the default value if no such thread has been set.
    bsl::size_t threadIndex() const BSLS_KEYWORD_OVERRIDE;

    /// Wait for functions and timers.
    static void* run(Async::State* state);

  private:
    /// Enumerates the constants used by this implementation.
    enum Constants {
        /// The thread is stopped.
        k_RUN_STATE_STOPPED = 0,

        /// The thread is started.
        k_RUN_STATE_STARTED = 1,

        /// The thread is stopping.
        k_RUN_STATE_STOPPING = 2
    };

    /// The thread name.
    bsl::string d_threadName;

    /// The thread handle.
    bslmt::ThreadUtil::Handle d_threadHandle;

    /// The thread attributes.
    bslmt::ThreadAttributes d_threadAttributes;

    /// The run mutex.
    ntccfg::ConditionMutex d_runMutex;

    /// The run condition.
    ntccfg::Condition d_runCondition;

    /// The run state.
    bsls::AtomicInt d_runState;

    /// The flag that indicates if functions and timers are dynamically
    /// load balanced across multiple threads.
    const bool d_dynamic;

    /// The chronology.
    bsl::shared_ptr<ntcs::Chronology> d_chronology_sp;

    /// The memory allocator.
    bslma::Allocator* d_allocator_p;

    /// The global asynchronous state.
    static State* s_global_p;
};

/// Create new asynchronous state.
Async::State::State(bslma::Allocator* basicAllocator)
: d_threadName("universal", basicAllocator)
, d_threadHandle(bslmt::ThreadUtil::invalidHandle())
, d_threadAttributes(basicAllocator)
, d_runMutex()
, d_runCondition()
, d_runState(k_RUN_STATE_STOPPED)
, d_dynamic(false)
, d_chronology_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_threadName.c_str());

    NTCI_LOG_TRACE("Thread '%s' is starting", d_threadName.c_str());

    d_chronology_sp.createInplace(d_allocator_p, this, d_allocator_p);

    d_threadAttributes.setThreadName(d_threadName);

    bslmt::ThreadUtil::ThreadFunction threadFunction =
        (bslmt::ThreadUtil::ThreadFunction)(&Async::State::run);
    void* threadUserData = this;

    ntsa::Error error = ntcs::ThreadUtil::create(&d_threadHandle,
                                                 d_threadAttributes,
                                                 threadFunction,
                                                 threadUserData);
    if (error) {
        NTCI_LOG_ERROR("Failed to create thread: %s", error.text().c_str());
        NTCCFG_ABORT();
    }

    ntccfg::ConditionMutexGuard guard(&d_runMutex);
    while (d_runState != k_RUN_STATE_STARTED) {
        d_runCondition.wait(&d_runMutex);
    }
}

/// Destroy this object.
Async::State::~State()
{
    int   rc;
    void* status;

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_threadName.c_str());

    NTCI_LOG_TRACE("Thread '%s' is stopping", d_threadName.c_str());

    d_runState = k_RUN_STATE_STOPPING;

    {
        ntccfg::ConditionMutexGuard guard(&d_runMutex);
        d_runCondition.broadcast();
    }

    rc = bslmt::ThreadUtil::join(d_threadHandle, &status);
    if (rc != 0) {
        NTCI_LOG_FATAL("Failed to join thread: rc = %d", rc);
        NTCCFG_ABORT();
    }

    d_runState = k_RUN_STATE_STOPPED;

    NTCI_LOG_TRACE("Thread '%s' has stopped", d_threadName.c_str());
}

void Async::State::execute(const Functor& function)
{
    d_chronology_sp->execute(function);
}

void Async::State::moveAndExecute(FunctorSequence* functorSequence,
                                  const Functor&   functor)
{
    d_chronology_sp->moveAndExecute(functorSequence, functor);
}

bsl::shared_ptr<ntci::Timer> Async::State::createTimer(
    const ntca::TimerOptions&                  options,
    const bsl::shared_ptr<ntci::TimerSession>& session,
    bslma::Allocator*                          basicAllocator)
{
    return d_chronology_sp->createTimer(options, session, basicAllocator);
}

bsl::shared_ptr<ntci::Timer> Async::State::createTimer(
    const ntca::TimerOptions&  options,
    const ntci::TimerCallback& callback,
    bslma::Allocator*          basicAllocator)
{
    return d_chronology_sp->createTimer(options, callback, basicAllocator);
}

const bsl::shared_ptr<ntci::Strand>& Async::State::strand() const
{
    return ntci::Strand::unspecified();
}

bsls::TimeInterval Async::State::currentTime() const
{
    return d_chronology_sp->currentTime();
}

bslma::Allocator* Async::State::allocator() const
{
    return d_allocator_p;
}

Async::State& Async::State::singleton()
{
    BSLMT_ONCE_DO
    {
        static Async::State state(&bslma::NewDeleteAllocator::singleton());
        s_global_p = &state;
    }

    return *s_global_p;
}

void Async::State::interruptOne()
{
    ntccfg::ConditionMutexGuard guard(&d_runMutex);
    d_runCondition.signal();
}

void Async::State::interruptAll()
{
    ntccfg::ConditionMutexGuard guard(&d_runMutex);
    d_runCondition.broadcast();
}

bslmt::ThreadUtil::Handle Async::State::threadHandle() const
{
    return d_threadHandle;
}

bsl::size_t Async::State::threadIndex() const
{
    return 0;
}

void* Async::State::run(Async::State* state)
{
    if (!state->d_threadAttributes.threadName().empty()) {
        bslmt::ThreadUtil::setThreadName(
            state->d_threadAttributes.threadName());
    }

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(state->d_threadName.c_str());

    NTCI_LOG_TRACE("Thread '%s' has started", state->d_threadName.c_str());

    {
        ntccfg::ConditionMutexGuard guard(&state->d_runMutex);
        state->d_runState = k_RUN_STATE_STARTED;
        state->d_runCondition.signal();
    }

    while (state->d_runState == k_RUN_STATE_STARTED) {
        bdlb::NullableValue<bsls::TimeInterval> deadline =
            state->d_chronology_sp->earliest();

        ntccfg::ConditionMutexGuard guard(&state->d_runMutex);

        if (deadline.isNull()) {
            state->d_runCondition.wait(&state->d_runMutex);
        }
        else {
            state->d_runCondition.timedWait(&state->d_runMutex,
                                            deadline.value());
        }

        while (state->d_chronology_sp->hasAnyScheduledOrDeferred()) {
            state->d_chronology_sp->announce(state->d_dynamic);
        }
    }

    state->d_chronology_sp->drain();

    return 0;
}

Async::State* Async::State::s_global_p;

bsl::shared_ptr<ntci::Strand> Async::createStrand(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntci::Executor> executor(
        &Async::State::singleton(),
        bslstl::SharedPtrNilDeleter(),
        Async::State::singleton().allocator());

    bsl::shared_ptr<ntcs::Strand> strand;
    strand.createInplace(allocator, executor, allocator);

    return strand;
}

bsl::shared_ptr<ntci::Timer> Async::createTimer(
    const ntca::TimerOptions&                  options,
    const bsl::shared_ptr<ntci::TimerSession>& session,
    bslma::Allocator*                          basicAllocator)
{
    return Async::State::singleton().createTimer(options,
                                                 session,
                                                 basicAllocator);
}

bsl::shared_ptr<ntci::Timer> Async::createTimer(
    const ntca::TimerOptions&  options,
    const ntci::TimerCallback& callback,
    bslma::Allocator*          basicAllocator)
{
    return Async::State::singleton().createTimer(options,
                                                 callback,
                                                 basicAllocator);
}

void Async::execute(const Functor& function)
{
    return Async::State::singleton().execute(function);
}

void Async::moveAndExecute(FunctorSequence* functorSequence,
                           const Functor&   functor)
{
    return Async::State::singleton().moveAndExecute(functorSequence, functor);
}

bsls::TimeInterval Async::currentTime()
{
    return Async::State::singleton().currentTime();
}

}  // close package namespace
}  // close enterprise namespace
