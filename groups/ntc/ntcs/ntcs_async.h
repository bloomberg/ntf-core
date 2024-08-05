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

#ifndef INCLUDED_NTCS_ASYNC
#define INCLUDED_NTCS_ASYNC

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_timeroptions.h>
#include <ntccfg_platform.h>
#include <ntci_executor.h>
#include <ntci_timer.h>
#include <ntci_timercallback.h>
#include <ntci_timersession.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bsl_functional.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide default asynchronous function execution for the process.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcs
struct Async {
  private:
    /// Initialize the process-wide asynchronous executor. Return the error.
    /// The behavior is undefined unless the process-wide asynchronous
    /// executor is locked.
    static ntsa::Error privateInitialize();

    /// Clean up the resources for the process-wide asynchronous executor.
    /// The behavior is undefined unless the process-wide asynchronous
    /// executor is locked.
    static void privateExit();

  public:
    /// Define a type alias for a function.
    typedef ntci::Executor::Functor Functor;

    /// Define a type alias for a queue of functions.
    typedef ntci::Executor::FunctorSequence FunctorSequence;

    /// Initialize the process-wide asynchronous executor. Return the error.
    static ntsa::Error initialize();

    /// Create a new strand to serialize execution of functors. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static bsl::shared_ptr<ntci::Strand> createStrand(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'session' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&                  options,
        const bsl::shared_ptr<ntci::TimerSession>& session,
        bslma::Allocator*                          basicAllocator = 0);

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'callback' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&  options,
        const ntci::TimerCallback& callback,
        bslma::Allocator*          basicAllocator = 0);

    /// Execute the function on a thread managed by the process-wide
    /// asynchronous executor.
    static void execute(const Functor& function);

    /// Atomically defer the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    static void moveAndExecute(FunctorSequence* functorSequence,
                               const Functor&   functor);

    /// Return the current elapsed time since the Unix epoch.
    static bsls::TimeInterval currentTime();

    /// Clean up the resources for the process-wide asynchronous executor.
    static void exit();
};

/// @internal @brief
/// Provide a strand implemented using the process-wide asynchronous executor.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class AsyncStrand : public ntci::Strand, public ntccfg::Shared<AsyncStrand>
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
    bool              d_pending;
    bslma::Allocator* d_allocator_p;

  private:
    AsyncStrand(const AsyncStrand&) BSLS_KEYWORD_DELETED;
    AsyncStrand& operator=(const AsyncStrand&) BSLS_KEYWORD_DELETED;

  private:
    /// Invoke the next functor in the queue.
    void invoke();

  public:
    /// Create a new inactive strand. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit AsyncStrand(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~AsyncStrand() BSLS_KEYWORD_OVERRIDE;

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

    /// Clear all pending operations.
    void clear() BSLS_KEYWORD_OVERRIDE;

    /// Return true if operations in this strand are currently being invoked
    /// by the current thread, otherwise return false.
    bool isRunningInCurrentThread() const BSLS_KEYWORD_OVERRIDE;
};

/// @internal @brief
/// Provide a timer implemented using the process-wide asynchronous executor.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class AsyncTimer : public ntci::Timer
{
    ntca::TimerOptions                  d_options;
    ntci::TimerCallback                 d_callback;
    bsl::shared_ptr<ntci::TimerSession> d_session_sp;
    bslma::Allocator*                   d_allocator_p;

  private:
    AsyncTimer(const AsyncTimer&) BSLS_KEYWORD_DELETED;
    AsyncTimer& operator=(const AsyncTimer&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new inactive timer that invokes the specified 'session'
    /// according to the specified 'options'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit AsyncTimer(const ntca::TimerOptions&                  options,
                        const bsl::shared_ptr<ntci::TimerSession>& session,
                        bslma::Allocator* basicAllocator = 0);

    /// Create a new inactive timer that invokes the specified 'callback'
    /// according to the specified 'options'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit AsyncTimer(const ntca::TimerOptions&  options,
                        const ntci::TimerCallback& callback,
                        bslma::Allocator*          basicAllocator = 0);

    /// Destroy this object.
    ~AsyncTimer() BSLS_KEYWORD_OVERRIDE;

    /// Set or update the deadline of this timer to the specified
    /// 'deadline', which, if the total number of milliseconds in the
    /// specified 'period' is greater than zero, automatically advances
    /// and recurs according to that 'period'. Return the error.
    ntsa::Error schedule(const bsls::TimeInterval& deadline,
                         const bsls::TimeInterval& period)
        BSLS_KEYWORD_OVERRIDE;

    /// Cancel this timer if its deadline has not already occurred, but
    /// allow this timer to be rescheduled. Return the error, notably
    /// 'ntca::Error::e_CANCELED' if another occurrence of this timer's
    /// deadline is scheduled but has not yet occurred (in which case
    /// the timer will be announced as cancelled instead),
    /// 'ntsa::Error::e_INVALID' if the 'timer' has not been registered
    /// or already removed, and 'ntsa::Error::e_OK' if this timer's
    /// deadline has already occured, is not recurring nor has been
    /// rescheduled.
    ntsa::Error cancel() BSLS_KEYWORD_OVERRIDE;

    /// Cancel this timer if its deadline has not already occured, close
    /// the timer for subsequent scheduling, and remove its resources.
    /// Return the error, notably 'ntca::Error::e_CANCELED' if another
    /// occurrence of this timer's deadline is scheduled but has not yet
    /// occurred (in which case the timer will be announced as cancelled
    /// instead), 'ntsa::Error::e_INVALID' if the 'timer' has not been
    /// registered or already removed, and 'ntsa::Error::e_OK' if this
    /// timer's deadline has already occured, is not recurring nor has
    /// been rescheduled.
    ntsa::Error close() BSLS_KEYWORD_OVERRIDE;

    /// Announce the arrival of the last specified 'deadline' of this
    /// timer, computed using the specified 'now', and, if the timer is
    /// in one-shot mode, also announce its automatic closure.
    void arrive(const bsl::shared_ptr<ntci::Timer>& self,
                const bsls::TimeInterval&           now,
                const bsls::TimeInterval& deadline) BSLS_KEYWORD_OVERRIDE;

    /// Return the user-defined handle of this timer.
    void* handle() const BSLS_KEYWORD_OVERRIDE;

    /// Return the user-defined identifier assigned to this timer.
    int id() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if this timer is in one-shot mode, otherwise return
    /// false.
    bool oneShot() const BSLS_KEYWORD_OVERRIDE;

    /// Return the deadline, or null if no deadline is scheduled.
    bdlb::NullableValue<bsls::TimeInterval> deadline() const 
    BSLS_KEYWORD_OVERRIDE;

    /// Return the period, or null if the timer is not periodic.
    bdlb::NullableValue<bsls::TimeInterval> period() const 
    BSLS_KEYWORD_OVERRIDE;

    /// Return the handle of the thread that manages this socket, or
    /// the default value if no such thread has been set.
    bslmt::ThreadUtil::Handle threadHandle() const BSLS_KEYWORD_OVERRIDE;

    /// Return the index in the thread pool of the thread that manages
    /// this socket, or 0 if no such thread has been set.
    bsl::size_t threadIndex() const BSLS_KEYWORD_OVERRIDE;

    /// Return the strand on which this object's functions should be
    /// called.
    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current elapsed time since the Unix epoch.
    bsls::TimeInterval currentTime() const BSLS_KEYWORD_OVERRIDE;
};

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
