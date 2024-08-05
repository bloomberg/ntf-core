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

#ifndef INCLUDED_NTCS_CHRONOLOGY
#define INCLUDED_NTCS_CHRONOLOGY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_timeroptions.h>
#include <ntccfg_platform.h>
#include <ntci_chronology.h>
#include <ntci_executor.h>
#include <ntci_mutex.h>
#include <ntci_timer.h>
#include <ntci_timercallback.h>
#include <ntci_timersession.h>
#include <ntcs_driver.h>
#include <ntcs_skiplist.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bdlma_concurrentmultipoolallocator.h>
#include <bdlma_pool.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bsls_atomic.h>
#include <bsls_spinlock.h>
#include <bsls_timeinterval.h>
#include <bsl_functional.h>
#include <bsl_map.h>
#include <bsl_memory.h>
#include <bsl_unordered_map.h>
#include <bsl_vector.h>

// Define and set to 1 to use an unordered hash table for the map of timers to
// iterators into the deadline multimap. Undefine or set to 0 to use an ordered
// red/black tree.
#define NTCS_CHRONOLOGY_USE_TIMER_MAP_UNORDERED 0

// Define and set to 1 to use a contiguous vector for the function queue.
// Undefine or set to 0 to use a linked list.
#define NTCS_CHRONOLOGY_USE_FUNCTOR_QUEUE_VECTOR 1

namespace BloombergLP {
namespace ntcs {
class Chronology;
}
namespace ntcs {

/// @internal @brief
/// Provide a priority queue of functions and timers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class Chronology NTSCFG_FINAL : public ntci::Chronology
{
    struct TimerNode;
    // This struct describes a node in the timer object catalog.

    struct DeadlineMapEntry {
        DeadlineMapEntry()
        : d_node_p(0)
        {
        }

        explicit DeadlineMapEntry(TimerNode* node)
        : d_node_p(node)
        {
        }

        TimerNode* d_node_p;

        NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(DeadlineMapEntry);
    };

    struct DueEntry {
        DueEntry()
        : d_node_p(0)
        , d_deadline()
        , d_period()
        , d_oneShot(false)
        , d_recurring(false)
        {
        }

        DueEntry(TimerNode*                node,
                 const bsls::TimeInterval& deadline,
                 const bsls::TimeInterval& period,
                 bool                      oneShot,
                 bool                      recurring)
        : d_node_p(node)
        , d_deadline(deadline)
        , d_period(period)
        , d_oneShot(oneShot)
        , d_recurring(recurring)
        {
        }

        TimerNode*         d_node_p;
        bsls::TimeInterval d_deadline;
        bsls::TimeInterval d_period;
        const bool         d_oneShot;
        const bool         d_recurring;

        NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(DueEntry);
    };

    /// Define a type alias for a signed 64-bit integer type
    /// representing a number of microseconds.
    typedef bsl::int64_t Microseconds;

    /// Define a type alias for a map of deadlines to the
    /// timers that should fire at those deadlines.
    typedef ntcs::SkipList<Microseconds, DeadlineMapEntry> DeadlineMap;

    /// This typedef defines a functor.
    typedef ntci::Executor::Functor Functor;

#if NTCS_CHRONOLOGY_USE_FUNCTOR_QUEUE_VECTOR
    /// This typedef defines a sequence of functors.
    typedef bsl::vector<Functor> FunctorQueue;
#else
    typedef ntci::Executor::FunctorSequence FunctorQueue;
    // This typedef defines a sequence of functors.
#endif

    /// Provide an implementation of the 'ntci::Timer' interface
    /// using an 'ntcs::Chronology' object.
    class Timer : public ntci::Timer
    {
        enum State { e_STATE_WAITING, e_STATE_SCHEDULED, e_STATE_CLOSED };

        ntccfg::Object                      d_object;
        mutable bsls::SpinLock              d_lock;
        ntcs::Chronology*                   d_chronology_p;
        TimerNode*                          d_node_p;
        const ntca::TimerOptions            d_options;
        ntci::TimerCallback                 d_callback;
        bsl::shared_ptr<ntci::TimerSession> d_session_sp;
        bsls::TimeInterval                  d_period;
        State                               d_state;
        DeadlineMap::Pair*                  d_deadlineMapHandle;
        bslma::Allocator*                   d_allocator_p;

        friend class Chronology;

      private:
        Timer(const Timer&) BSLS_KEYWORD_DELETED;
        Timer& operator=(const Timer&) BSLS_KEYWORD_DELETED;

      private:
        /// Dispatch the specified 'callback' or 'session' to announce the
        /// auto-closure of this one-shot timer.
        void autoClose(const bsl::shared_ptr<ntci::Timer>&        timer,
                       const ntci::TimerCallback&                 callback,
                       const bsl::shared_ptr<ntci::TimerSession>& session);

        /// Load into the specified 'timerContext' the difference between
        /// the the specified desired 'deadline' and the current time that
        /// the timer's deadline was announced or dispatched.
        void calculateDrift(ntca::TimerContext*       timerContext,
                            const bsls::TimeInterval& deadline);

        /// Invoke or defer the announcement of the arrival of the
        /// specified deadline 'event' for the specified 'timer' to the
        /// specified 'session'.
        static void dispatchSessionDeadline(
            const bsl::shared_ptr<ntci::Timer>&        timer,
            const bsl::shared_ptr<ntci::TimerSession>& session,
            const ntca::TimerEvent&                    event);

        /// Invoke the specified 'callback' for the specified 'timer'
        /// indicating that the timer has been cancelled.
        static void processCallbackCancelled(
            const bsl::shared_ptr<ntci::Timer>& timer,
            const ntci::TimerCallback&          callback);

        /// Invoke the specified 'session' for the specified 'timer'
        /// indicating that the timer has been cancelled.
        static void processSessionCancelled(
            const bsl::shared_ptr<ntci::Timer>&        timer,
            const bsl::shared_ptr<ntci::TimerSession>& session);

        /// Invoke the specified 'callback' for the specified 'timer'
        /// indicating that the timer has been closed.
        static void processCallbackClosed(
            const bsl::shared_ptr<ntci::Timer>& timer,
            const ntci::TimerCallback&          callback);

        /// Invoke the specified 'session' for the specified 'timer'
        /// indicating that the timer has been closed.
        static void processSessionClosed(
            const bsl::shared_ptr<ntci::Timer>&        timer,
            const bsl::shared_ptr<ntci::TimerSession>& session);

        /// Invoke the specified 'callback' for the specified 'timer'
        /// indicating that the timer has been closed.
        static void processCallbackCancelledAndClosed(
            const bsl::shared_ptr<ntci::Timer>& timer,
            const ntci::TimerCallback&          callback);

        /// Invoke the specified 'session' for the specified 'timer'
        /// indicating that the timer has been closed.
        static void processSessionCancelledAndClosed(
            const bsl::shared_ptr<ntci::Timer>&        timer,
            const bsl::shared_ptr<ntci::TimerSession>& session);

      public:
        /// Create a new timer managed by the specified 'chronology' using
        /// the specified 'options'. Invoke the specified 'callback' for
        /// each timer event. Optionally specify a
        /// 'basicAllocator' used to supply memory. If 'basicAllocator' is
        /// 0, the currently installed default allocator is used.
        Timer(ntcs::Chronology*          d_chronology_p,
              TimerNode*                 node,
              const ntca::TimerOptions&  options,
              const ntci::TimerCallback& callback,
              bslma::Allocator*          basicAllocator = 0);

        /// Create a new timer managed by the specified 'chronology' using
        /// the specified 'options'. Invoke the specified 'session' for
        /// each timer event. Optionally specify a
        /// 'basicAllocator' used to supply memory. If 'basicAllocator' is
        /// 0, the currently installed default allocator is used.
        Timer(ntcs::Chronology*                          d_chronology_p,
              TimerNode*                                 node,
              const ntca::TimerOptions&                  options,
              const bsl::shared_ptr<ntci::TimerSession>& session,
              bslma::Allocator*                          basicAllocator = 0);

        /// Destroy this object.
        ~Timer() BSLS_KEYWORD_OVERRIDE;

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
        const bsl::shared_ptr<ntci::Strand>& strand() const
            BSLS_KEYWORD_OVERRIDE;

        /// Return the current elapsed time since the Unix epoch.
        bsls::TimeInterval currentTime() const BSLS_KEYWORD_OVERRIDE;
    };

    friend class Timer;
    // Allow the 'Timer' class to access the private members of this
    // class.

    /// Define a type alias for shared pointer to a timer.
    typedef bsl::shared_ptr<Timer> TimerPtr;

    /// Provide a shared pointer representation for a timer.
    class TimerRep : public bslma::SharedPtrRep
    {
        ntccfg::Object            d_object;
        ntcs::Chronology*         d_chronology_p;
        TimerNode*                d_node_p;
        bsls::ObjectBuffer<Timer> d_storage;

      private:
        TimerRep(const TimerRep&) BSLS_KEYWORD_DELETED;
        TimerRep& operator=(const TimerRep&) BSLS_KEYWORD_DELETED;

      public:
        /// Create a new timer representation.
        TimerRep(ntcs::Chronology*          chronology,
                 TimerNode*                 node,
                 const ntca::TimerOptions&  options,
                 const ntci::TimerCallback& callback,
                 bslma::Allocator*          basicAllocator);

        /// Create a new timer representation.
        TimerRep(ntcs::Chronology*                          chronology,
                 TimerNode*                                 node,
                 const ntca::TimerOptions&                  options,
                 const bsl::shared_ptr<ntci::TimerSession>& session,
                 bslma::Allocator*                          basicAllocator);

        /// Destroy this object.
        ~TimerRep() BSLS_KEYWORD_OVERRIDE;

        /// Dispose of the shared object referred to by this representation.
        /// This method is automatically invoked by 'releaseRef' when the
        /// number of shared references reaches zero and should not be
        /// explicitly invoked otherwise.  Note that this virtual
        /// 'disposeObject' method effectively serves as the shared object's
        /// destructor.  Also note that derived classes must override this
        /// method to perform the appropriate action such as deleting the
        /// shared object.
        void disposeObject() BSLS_KEYWORD_OVERRIDE;

        /// Dispose of this representation object.  This method is
        /// automatically invoked by 'releaseRef' and 'releaseWeakRef' when
        /// the number of weak references and the number of shared
        /// references both reach zero and should not be explicitly invoked
        /// otherwise.  The behavior is undefined unless 'disposeObject' has
        /// already been called for this representation.  Note that this
        /// virtual 'disposeRep' method effectively serves as the
        /// representation object's destructor.  Also note that derived
        /// classes must override this method to perform appropriate action
        /// such as deleting this representation, or returning it to an
        /// object pool.
        void disposeRep() BSLS_KEYWORD_OVERRIDE;

        /// Return a pointer to the deleter stored by the derived
        /// representation (if any) if the deleter has the same type as that
        /// described by the specified 'type', and a null pointer otherwise.
        /// Note that while this methods appears to be a simple accessor, it
        /// is declared as non-'const' qualified to support representations
        /// storing the deleter directly as a data member.
        void* getDeleter(const std::type_info& type) BSLS_KEYWORD_OVERRIDE;

        /// Return the (untyped) address of the modifiable shared object
        /// referred to by this representation.
        void* originalPtr() const BSLS_KEYWORD_OVERRIDE;

        /// Return the typed address of the modifiable shared object
        /// referred to by this representation.
        Timer* getObject() const;
    };

    friend class TimerRep;
    // Allow the 'TimerRep' class to access the private members of this
    // class.

    /// This struct describes a node in the timer object catalog.
    struct TimerNode {
        TimerNode()
        : d_storage()
        , d_next_p(0)
        {
        }

        bsls::ObjectBuffer<TimerRep> d_storage;
        TimerNode*                   d_next_p;
    };

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    ntccfg::Object                      d_object;
    mutable Mutex                       d_mutex;
    bslma::Allocator*                   d_allocator_p;
    bsl::shared_ptr<ntcs::Interruptor>  d_interruptor_sp;
    bsl::shared_ptr<ntci::Chronology>   d_parent_sp;
    bdlma::Pool                         d_nodePool;
    bsl::vector<TimerNode*>             d_nodeArray;
    TimerNode*                          d_nodeFree_p;
    bsl::size_t                         d_nodeCount;
    bdlma::ConcurrentMultipoolAllocator d_deadlineMapPool;
    bslma::Allocator*                   d_deadlineMapAllocator_p;
    DeadlineMap                         d_deadlineMap;
    bsls::AtomicBool                    d_deadlineMapEmpty;
    bsls::AtomicInt64                   d_deadlineMapEarliest;
    bdlma::ConcurrentMultipoolAllocator d_functorQueuePool;
    bslma::Allocator*                   d_functorQueueAllocator_p;
    FunctorQueue                        d_functorQueue;
    bsls::AtomicBool                    d_functorQueueEmpty;

  private:
    Chronology(const Chronology&) BSLS_KEYWORD_DELETED;
    Chronology& operator=(const Chronology&) BSLS_KEYWORD_DELETED;

    /// Load into the specified 'result' the time the earliest timer is
    /// due, if any.
    void findEarliest(bdlb::NullableValue<bsls::TimeInterval>* result) const;

    /// Allocate a new timer node. The behavior is undefined unless
    /// 'd_mutex' is locked.
    TimerNode* privateNodeAllocate();

    /// Return the description of the specified 'timeInMicroseconds' from
    /// the Unix epoch in a date/time format.
    static bsl::string convertToDateTime(Microseconds timeInMicroseconds);

  public:
    /// The time interval that is LLONG_MAX microseconds from the Unix
    /// epoch.
    static const bsls::TimeInterval k_MAX_TIME_INTERVAL;

    /// Equivalent to LLONG_MAX microseconds.
    static const bsl::int64_t k_MAX_TIME_INTERVAL_IN_MICROSECONDS;

    /// Create a new timer chronology that drives each timer and deferred
    /// function using the specified 'interruptor'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Chronology(ntcs::Interruptor* interruptor,
                        bslma::Allocator*  basicAllocator = 0);

    /// Create a new timer chronology. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit Chronology(const bsl::shared_ptr<ntcs::Interruptor>& interruptor,
                        bslma::Allocator* basicAllocator = 0);

    /// Create a new timer chronology that drives each timer and deferred
    /// function using the specified 'interruptor'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Chronology(ntcs::Interruptor*                       interruptor,
                        const bsl::shared_ptr<ntci::Chronology>& parent,
                        bslma::Allocator* basicAllocator = 0);

    /// Create a new timer chronology. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit Chronology(const bsl::shared_ptr<ntcs::Interruptor>& interruptor,
                        const bsl::shared_ptr<ntci::Chronology>& parent,
                        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Chronology() BSLS_KEYWORD_OVERRIDE;

    /// Set the parent to the specified 'parent'.
    void setParent(const bsl::shared_ptr<ntci::Chronology>& parent);

    /// Remove all functions and timers from the chronology.
    void clear() BSLS_KEYWORD_OVERRIDE;

    /// Remove all functions from the chronology.
    void clearFunctions() BSLS_KEYWORD_OVERRIDE;

    /// Remove all timers from the chronology.
    void clearTimers() BSLS_KEYWORD_OVERRIDE;

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'session' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&                  options,
        const bsl::shared_ptr<ntci::TimerSession>& session,
        bslma::Allocator*                          basicAllocator = 0) 
        BSLS_KEYWORD_OVERRIDE;

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

    /// Invoke all deferred functions and announce the deadline event of any
    /// timer whose deadline is earlier than or equal to the current time.
    void announce(bool single = false) BSLS_KEYWORD_OVERRIDE;

    /// Invoke all deferred functions.
    void drain() BSLS_KEYWORD_OVERRIDE;

    /// Close all timers.
    void closeAll() BSLS_KEYWORD_OVERRIDE;

    /// Push the specified 'functor' on the queue.
    void execute(const ntci::Executor::Functor& functor) BSLS_KEYWORD_OVERRIDE;

    /// Atomically push the specified 'functorSequence' immediately followed
    /// by the specified 'functor', then clear the 'functorSequence'.
    void moveAndExecute(
        ntci::Executor::FunctorSequence* functorSequence,
        const ntci::Executor::Functor&   functor) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' all the scheduled timers in the
    /// chronology.
    void load(TimerVector* result) const BSLS_KEYWORD_OVERRIDE;

    /// Return the absolute time the earliest scheduled timer is due, if any.
    bdlb::NullableValue<bsls::TimeInterval> earliest() const 
    BSLS_KEYWORD_OVERRIDE;

    /// Return the relative time interval to wait until the earliest timer
    /// is due, if any, from the current time, or null if no timer is
    /// scheduled.
    bdlb::NullableValue<bsls::TimeInterval> timeoutInterval() const 
    BSLS_KEYWORD_OVERRIDE;

    /// Return the number of milliseconds to wait until the earliest timer
    /// is due, if any, from the current time, or -1 if no timer is
    /// scheduled.
    int timeoutInMilliseconds() const BSLS_KEYWORD_OVERRIDE;

    /// Return the number of registered, but not necessarily scheduled
    /// timers in the chronology.
    bsl::size_t numRegistered() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if there are any registered, but not necessarily
    /// scheduled timers in the chronology, otherwise return false.
    bool hasAnyRegistered() const BSLS_KEYWORD_OVERRIDE;

    /// Return the number of scheduled timers in the chronology.
    bsl::size_t numScheduled() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if there are any scheduled timers in the chronology,
    /// otherwise return false.
    bool hasAnyScheduled() const BSLS_KEYWORD_OVERRIDE;

    /// Return the number of deferred functions in the chronology.
    bsl::size_t numDeferred() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if there are any deferred functions in the chronology,
    /// otherwise return false.
    bool hasAnyDeferred() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if there are any scheduled timers or deferred functions
    /// in the chronology, otherwise return false.
    bool hasAnyScheduledOrDeferred() const BSLS_KEYWORD_OVERRIDE;

    /// Return the strand on which this object's functions should be called.
    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current elapsed time since the Unix epoch.
    bsls::TimeInterval currentTime() const BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace
#endif
