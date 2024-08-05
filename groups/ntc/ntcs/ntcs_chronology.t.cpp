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

#include <ntccfg_test.h>
#include <ntci_log.h>
#include <ntci_timer.h>
#include <ntci_timersession.h>
#include <ntcs_chronology.h>
#include <ntcs_driver.h>
#include <ntcs_strand.h>
#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bdlmt_fixedthreadpool.h>
#include <bdlt_currenttime.h>
#include <bslma_testallocator.h>
#include <bslmt_barrier.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>
#include <bsl_queue.h>

using namespace BloombergLP;

namespace test {

const int k_THREAD_INDEX = 5;
const int k_TIMER_ID_0   = 10;
const int k_TIMER_ID_1   = 22;
const int k_TIMER_ID_2   = 33;
const int k_TIMER_ID_3   = 158;
const int k_TIMER_ID_4   = 8;
const int k_TIMER_ID_5   = 751;

/// This class mocks behavior of ntcs::Driver
class DriverMock : public ntcs::Driver
{
    int d_interruptOneCallCtr;
    int d_interruptAllCallCtr;

  private:
    DriverMock(const DriverMock&) BSLS_KEYWORD_DELETED;
    DriverMock& operator=(const DriverMock&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new driver
    DriverMock();

    /// Destroy this object.
    ~DriverMock() BSLS_KEYWORD_OVERRIDE;

    /// Register a thread described by the specified 'waiterOptions' that
    /// will drive this object. Return the handle to the waiter.
    ntci::Waiter registerWaiter(const ntca::WaiterOptions& waiterOptions)
        BSLS_KEYWORD_OVERRIDE;

    /// Deregister the specified 'waiter'.
    void deregisterWaiter(ntci::Waiter waiter) BSLS_KEYWORD_OVERRIDE;

    /// Unblock one waiter blocked on 'wait'.
    void interruptOne() BSLS_KEYWORD_OVERRIDE;

    /// Unblock all waiters blocked on 'wait'.
    void interruptAll() BSLS_KEYWORD_OVERRIDE;

    /// Clear all resources managed by this object.
    void clear() BSLS_KEYWORD_OVERRIDE;

    /// Return the name of the driver.
    const char* name() const BSLS_KEYWORD_OVERRIDE;

    /// Return the handle of the thread that will be calling 'wait()', or
    /// the default value if no such thread has been set.
    bslmt::ThreadUtil::Handle threadHandle() const BSLS_KEYWORD_OVERRIDE;

    /// Return the index of the thread that will be calling 'wait()', or
    /// the default value if no such thread has been set.
    bsl::size_t threadIndex() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current number of registered waiters.
    bsl::size_t numWaiters() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current number of descriptors being monitored.
    bsl::size_t numSockets() const BSLS_KEYWORD_OVERRIDE;

    /// Return the maximum number of descriptors capable of being monitored
    /// at one time.
    bsl::size_t maxSockets() const BSLS_KEYWORD_OVERRIDE;

    void validateInterruptOneCalled()
    {
        NTCCFG_TEST_TRUE(d_interruptOneCallCtr > 0);
        --d_interruptOneCallCtr;
    }

    void validateInterruptAllCalled()
    {
        NTCCFG_TEST_TRUE(d_interruptAllCallCtr > 0);
        --d_interruptAllCallCtr;
    }
};

struct TimerCallbackCollector {
    TimerCallbackCollector(bslma::Allocator* basicAllocator)
    : d_events(basicAllocator)
    {
    }

    ~TimerCallbackCollector()
    {
        NTCCFG_TEST_TRUE(d_events.empty());
    }

    void processTimer(const bsl::shared_ptr<ntci::Timer>& timer,
                      const ntca::TimerEvent&             event)
    {
        NTCI_LOG_CONTEXT();

        NTCI_LOG_STREAM_DEBUG << "Processing timer event " << event
                              << " from timer id " << timer->id()
                              << NTCI_LOG_STREAM_END;

        d_events.push(bsl::make_pair(timer->id(), event));
    }

    void validateEventReceived(int timerId, ntca::TimerEventType::Value event)
    {
        NTCCFG_TEST_FALSE(d_events.empty());

        const TimerIdAndEvent& idAndEv = d_events.front();
        NTCCFG_TEST_EQ(idAndEv.first, timerId);
        NTCCFG_TEST_EQ(idAndEv.second.type(), event);
        d_events.pop();
    }

    void validateNoEventReceived() const
    {
        NTCCFG_TEST_TRUE(d_events.empty());
    }

  private:
    typedef std::pair<int, ntca::TimerEvent> TimerIdAndEvent;
    bsl::queue<TimerIdAndEvent>              d_events;
};

class StrandMock : public ntci::Strand
{
  public:
    /// mocking base class method
    void drain() BSLS_KEYWORD_OVERRIDE
    {
    }

    /// mocking base class method
    void clear() BSLS_KEYWORD_OVERRIDE
    {
    }

    bool isRunningInCurrentThread() const BSLS_KEYWORD_OVERRIDE
    {
        return true;
    }
    // mocking base class method

    void execute(const Functor& functor) BSLS_KEYWORD_OVERRIDE {};
    // mocking base class method

    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE {};
    // mocking base class method
};

class TimerSessionMock : public ntci::TimerSession
{
  public:
    ~TimerSessionMock()
    {
        NTCCFG_TEST_FALSE(d_deadline.has_value());
        NTCCFG_TEST_FALSE(d_cancel.has_value());
        NTCCFG_TEST_FALSE(d_close.has_value());
    }

    void processTimerDeadline(const bsl::shared_ptr<ntci::Timer>& timer,
                              const ntca::TimerEvent&             event)
        BSLS_KEYWORD_OVERRIDE
    {
        NTCI_LOG_CONTEXT();

        NTCI_LOG_STREAM_DEBUG << "processTimerDeadline event " << event
                              << NTCI_LOG_STREAM_END;

        NTCCFG_TEST_FALSE(d_deadline.has_value());
        d_deadline = event;
    }

    void processTimerCancelled(const bsl::shared_ptr<ntci::Timer>& timer,
                               const ntca::TimerEvent&             event)
        BSLS_KEYWORD_OVERRIDE
    {
        NTCI_LOG_CONTEXT();

        NTCI_LOG_STREAM_DEBUG << "processTimerCancelled event " << event
                              << NTCI_LOG_STREAM_END;

        NTCCFG_TEST_FALSE(d_cancel.has_value());
        d_cancel = event;
    }

    void processTimerClosed(const bsl::shared_ptr<ntci::Timer>& timer,
                            const ntca::TimerEvent&             event)
        BSLS_KEYWORD_OVERRIDE
    {
        NTCI_LOG_CONTEXT();

        NTCI_LOG_STREAM_DEBUG << "processTimerClosed event " << event
                              << NTCI_LOG_STREAM_END;

        NTCCFG_TEST_FALSE(d_close.has_value());
        d_close = event;
    }

    void validateEventReceivedAndClear(ntca::TimerEventType::Value v)
    {
        if (ntca::TimerEventType::e_DEADLINE == v) {
            NTCCFG_TEST_TRUE(d_deadline.has_value());
            d_deadline.reset();
        }
        else if (ntca::TimerEventType::e_CANCELED == v) {
            NTCCFG_TEST_TRUE(d_cancel.has_value());
            d_cancel.reset();
        }
        else {
            NTCCFG_TEST_TRUE(d_close.has_value());
            d_close.reset();
        }
    }

    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE
    {
        static bsl::shared_ptr<ntci::Strand> s;
        return s;
    }

    void validateNoEventReceived() const
    {
        NTCCFG_TEST_FALSE(d_deadline.has_value());
        NTCCFG_TEST_FALSE(d_cancel.has_value());
        NTCCFG_TEST_FALSE(d_close.has_value());
    }

  private:
    bsl::optional<ntca::TimerEvent> d_deadline;
    bsl::optional<ntca::TimerEvent> d_cancel;
    bsl::optional<ntca::TimerEvent> d_close;
};

class TestClock
{
  public:
    TestClock();
    ~TestClock();

    void advance(const bsls::TimeInterval&);

    static bsls::TimeInterval currentTime()
    {
        return s_currentTime;
    }

  private:
    bdlt::CurrentTime::CurrentTimeCallback d_previousCallback;
    static bsls::TimeInterval              s_currentTime;
};

class TestSuite
{
  public:
    TestSuite();
    ~TestSuite();

    void validateRegisteredAndScheduled(int registered, int scheduled) const;

    static ntca::TimerOptions createOptionsAllDisabled(int id);
    static void               incrementCallback(int&);

    static const bsls::TimeInterval oneSecond;
    static const bsls::TimeInterval oneMinute;
    static const bsls::TimeInterval oneHour;

    ntccfg::TestAllocator ta;
    TestClock             clock;

    bsl::shared_ptr<TimerCallbackCollector> callbacks;
    ntci::TimerCallback                     timerCallback;

    bsl::shared_ptr<test::DriverMock> driver;
    bsl::shared_ptr<ntcs::Chronology> chronology;
};

const bsls::TimeInterval TestSuite::oneSecond = bsls::TimeInterval(1, 0);
const bsls::TimeInterval TestSuite::oneMinute = bsls::TimeInterval(60, 0);
const bsls::TimeInterval TestSuite::oneHour   = bsls::TimeInterval(3600, 0);

TestSuite::TestSuite()
{
    callbacks.createInplace(&ta, &ta);

    ntci::TimerCallback tmp(
        NTCCFG_BIND(&test::TimerCallbackCollector::processTimer,
                    callbacks.get(),
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2));
    timerCallback.swap(tmp);

    driver.createInplace(&ta);
    chronology.createInplace(&ta, driver, &ta);

    NTCCFG_TEST_EQ(chronology->numRegistered(), 0);
    NTCCFG_TEST_EQ(chronology->numScheduled(), 0);
    NTCCFG_TEST_FALSE(chronology->hasAnyDeferred());
    NTCCFG_TEST_FALSE(chronology->hasAnyRegistered());
    NTCCFG_TEST_FALSE(chronology->hasAnyScheduled());
    NTCCFG_TEST_FALSE(chronology->hasAnyScheduledOrDeferred());
    NTCCFG_TEST_FALSE(chronology->earliest().has_value());
    NTCCFG_TEST_FALSE(chronology->timeoutInterval().has_value());
}

TestSuite::~TestSuite()
{
    NTCCFG_TEST_EQ(chronology->numScheduled(), 0);
    NTCCFG_TEST_EQ(chronology->numRegistered(), 0);
    NTCCFG_TEST_FALSE(chronology->hasAnyScheduledOrDeferred());
    chronology.reset();
    driver.reset();
    callbacks.reset();
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

void TestSuite::validateRegisteredAndScheduled(int registered,
                                               int scheduled) const
{
    NTCCFG_TEST_EQ(chronology->numRegistered(), registered);
    NTCCFG_TEST_EQ(chronology->numScheduled(), scheduled);
}

void TestSuite::incrementCallback(int& val)
{
    ++val;
}

ntca::TimerOptions TestSuite::createOptionsAllDisabled(int id)
{
    ntca::TimerOptions timerOptions;
    timerOptions.setId(id);
    timerOptions.setOneShot(false);
    timerOptions.hideEvent(ntca::TimerEventType::e_DEADLINE);
    timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
    return timerOptions;
}

DriverMock::DriverMock()
{
    d_interruptAllCallCtr = 0;
    d_interruptOneCallCtr = 0;
}

DriverMock::~DriverMock()
{
    NTCCFG_TEST_EQ(d_interruptOneCallCtr, 0);
    NTCCFG_TEST_EQ(d_interruptAllCallCtr, 0);
}

ntci::Waiter DriverMock::registerWaiter(
    const ntca::WaiterOptions& waiterOptions)
{
    return reinterpret_cast<ntci::Waiter>(0);
}

void DriverMock::deregisterWaiter(ntci::Waiter waiter)
{
}

void DriverMock::interruptOne()
{
    NTCCFG_TEST_EQ(d_interruptOneCallCtr, 0);
    ++d_interruptOneCallCtr;
}

void DriverMock::interruptAll()
{
    NTCCFG_TEST_EQ(d_interruptAllCallCtr, 0);
    ++d_interruptAllCallCtr;
}

void DriverMock::clear()
{
}

const char* DriverMock::name() const
{
    return "DriverMock";
}

bslmt::ThreadUtil::Handle DriverMock::threadHandle() const
{
    return bslmt::ThreadUtil::Handle();
}

bsl::size_t DriverMock::threadIndex() const
{
    return k_THREAD_INDEX;
}

bsl::size_t DriverMock::numWaiters() const
{
    return 0;
}

bsl::size_t DriverMock::numSockets() const
{
    return 0;
}

bsl::size_t DriverMock::maxSockets() const
{
    return 0;
}

bsls::TimeInterval TestClock::s_currentTime = bsls::TimeInterval();

TestClock::TestClock()
: d_previousCallback(bdlt::CurrentTime::currentTimeCallback())
{
    bsls::TimeInterval startTime =
        bdlt::EpochUtil::convertToTimeInterval(bdlt::EpochUtil::epoch())
            .addDays(1);
    s_currentTime = startTime;
    bdlt::CurrentTime::setCurrentTimeCallback(TestClock::currentTime);
}

TestClock::~TestClock()
{
    bdlt::CurrentTime::setCurrentTimeCallback(d_previousCallback);
}

void TestClock::advance(const bsls::TimeInterval& timePassed)
{
    s_currentTime += timePassed;
}

/// Multithreaded test driver
class MtDriver : public ntcs::Driver,
                 public ntci::Executor,
                 public ntccfg::Shared<ntci::Executor>
{
  private:
    MtDriver(const MtDriver&) BSLS_KEYWORD_DELETED;
    MtDriver& operator=(const MtDriver&) BSLS_KEYWORD_DELETED;

  public:
    explicit MtDriver(bslma::Allocator* basicAllocator = 0)
    : d_allocator_p(basicAllocator)
    {
        d_chronology = bsl::allocate_shared<ntcs::Chronology>(d_allocator_p,
                                                              this,
                                                              d_allocator_p);
        d_run        = true;
        d_blocked    = true;
    }

    ~MtDriver() BSLS_KEYWORD_OVERRIDE
    {
    }

    ntci::Waiter registerWaiter(const ntca::WaiterOptions& waiterOptions)
        BSLS_KEYWORD_OVERRIDE
    {
        return reinterpret_cast<ntci::Waiter>(0);
    }

    void deregisterWaiter(ntci::Waiter waiter) BSLS_KEYWORD_OVERRIDE
    {
    }

    void interruptOne() BSLS_KEYWORD_OVERRIDE
    {
        ntccfg::ConditionMutexGuard guard(&d_mutex);
        d_blocked = false;
        d_condition.signal();
    }

    void interruptAll() BSLS_KEYWORD_OVERRIDE
    {
        ntccfg::ConditionMutexGuard guard(&d_mutex);
        d_blocked = false;
        d_condition.broadcast();
    }

    void clear() BSLS_KEYWORD_OVERRIDE
    {
    }

    void run()
    {
        NTCI_LOG_CONTEXT();

        while (d_run) {
            NTCI_LOG_DEBUG("iterating inside ");
            d_mutex.lock();
            while (d_blocked && d_run) {
                const bdlb::NullableValue<bsls::TimeInterval> earliest =
                    chronology().earliest();
                int res = 0;
                if (earliest.has_value()) {
                    res = d_condition.timedWait(&d_mutex, earliest.value());
                }
                else {
                    res = d_condition.wait(&d_mutex);
                }

                //check that timedWait was used and then check for e_TIMED_OUT
                if (earliest.has_value() &&
                    res == bslmt::Condition::e_TIMED_OUT)  //wake up by timout
                {
                    d_blocked = false;
                }
                else if (res == 0) {
                    if (d_blocked == true) {
                        //spurious wakeup, ignore it
                        continue;
                    }
                }
                else {
                    NTCCFG_TEST_ASSERT(false);
                }
            }

            d_blocked = true;
            d_mutex.unlock();
            d_chronology->announce();
        }
    }

    void stop()
    {
        d_run = false;
        interruptAll();
    }

    ntcs::Chronology& chronology()
    {
        return *d_chronology;
    }

    /// Defer the execution of the specified 'functor'.
    void execute(const Functor& functor) BSLS_KEYWORD_OVERRIDE
    {
        d_chronology->execute(functor);
    }

    /// Atomically execute the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE
    {
        d_chronology->moveAndExecute(functorSequence, functor);
    }

    bsl::shared_ptr<ntcs::Strand> createStrand()
    {
        bsl::shared_ptr<ntcs::Strand> strand =
            bsl::allocate_shared<ntcs::Strand>(d_allocator_p,
                                               getSelf(this),
                                               d_allocator_p);
        return strand;
    }

    const char* name() const BSLS_KEYWORD_OVERRIDE
    {
        return "test::MtDriver";
    }

    bslmt::ThreadUtil::Handle threadHandle() const BSLS_KEYWORD_OVERRIDE
    {
        return bslmt::ThreadUtil::Handle();
    }

    bsl::size_t threadIndex() const BSLS_KEYWORD_OVERRIDE
    {
        return 0;
    }

    bsl::size_t numWaiters() const BSLS_KEYWORD_OVERRIDE
    {
        return 0;
    }

    bsl::size_t numSockets() const BSLS_KEYWORD_OVERRIDE
    {
        return 0;
    }

    bsl::size_t maxSockets() const BSLS_KEYWORD_OVERRIDE
    {
        return 0;
    }

  private:
    bslma::Allocator* d_allocator_p;

    ntccfg::ConditionMutex            d_mutex;
    ntccfg::Condition                 d_condition;
    bool                              d_blocked;
    bsls::AtomicBool                  d_run;
    bsl::shared_ptr<ntcs::Chronology> d_chronology;
};

class MtTestSuite
{
  private:
    MtTestSuite(const MtTestSuite&);
    MtTestSuite& operator=(const MtTestSuite&);

  public:
    MtTestSuite()
    {
        d_driver.createInplace(&d_ta, &d_ta);
        d_consumers.createInplace(&d_ta, &d_ta);
        d_producers.createInplace(&d_ta, &d_ta);
    }

    ~MtTestSuite()
    {
        NTCCFG_TEST_EQ(d_numOneShotTimersToConsume, 0);
        NTCCFG_TEST_EQ(d_numTimersToProduce, 0);
        NTCCFG_TEST_EQ(d_numExpectedCloseEvents, 0);
        NTCCFG_TEST_EQ(d_numPeriodicTimersShot, 0);

        NTCCFG_TEST_EQ(d_consumers->numThreads(), 0);
        NTCCFG_TEST_EQ(d_producers->numThreads(), 0);
        NTCCFG_TEST_EQ(d_driver->chronology().numScheduled(), 0);
        NTCCFG_TEST_EQ(d_driver->chronology().numRegistered(), 0);

        d_driver.reset();
        d_consumers.reset();
        d_producers.reset();
    }

    void createAndLaunchConsumers(int numConsumers)
    {
        for (int i = 0; i < numConsumers; ++i) {
            bslmt::ThreadAttributes attributes;
            attributes.setThreadName("consumer-" + bsl::to_string(i));

            int rc = d_consumers->addThread(
                NTCCFG_BIND(&test::MtTestSuite::consumerThread, d_driver),
                attributes);
            NTCCFG_TEST_EQ(rc, 0);
        }
    }

    void createAndLaunchProducers(int numProducers)
    {
        for (int i = 0; i < numProducers; ++i) {
            bslmt::ThreadAttributes attributes;
            attributes.setThreadName("producer-" + bsl::to_string(i));

            int rc = d_producers->addThread(
                NTCCFG_BIND(&test::MtTestSuite::producerThread, this),
                attributes);
            NTCCFG_TEST_EQ(rc, 0);
        }
    }

    void createAndLaunchProducersWithStrands(int numProducers)
    {
        for (int i = 0; i < numProducers; ++i) {
            bslmt::ThreadAttributes attributes;
            attributes.setThreadName("producer-" + bsl::to_string(i));

            int rc = d_producers->addThread(
                NTCCFG_BIND(&test::MtTestSuite::producerThreadWithStrand,
                            this,
                            i % d_strands.size()),
                attributes);
            NTCCFG_TEST_EQ(rc, 0);
        }
    }

    static void consumerThread(const bsl::shared_ptr<test::MtDriver>& driver)
    {
        NTCI_LOG_CONTEXT();
        NTCI_LOG_DEBUG("Starting consumer thread");

        driver->run();

        NTCI_LOG_DEBUG("Consumer thread finished");
    }

    void producerThread()
    {
        NTCI_LOG_CONTEXT();

        while (true) {
            //code below could be written easier if letting d_numTimersToProduce go below zero
            //e.g. while(d_numTimersToProduce.subtract(1) >= 0)
            int current = d_numTimersToProduce.load();
            if (current == 0)
                return;
            int next    = current - 1;
            int swapped = d_numTimersToProduce.testAndSwap(current, next);
            if (swapped != current)
                continue;

            ntca::TimerOptions timerOptions;
            timerOptions.setOneShot(true);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
            timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
            timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

            ntci::TimerCallback callback(
                NTCCFG_BIND(&test::MtTestSuite::processTimer,
                            this,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2));

            bsl::shared_ptr<ntci::Timer> timer =
                d_driver->chronology().createTimer(timerOptions,
                                                   callback,
                                                   &d_ta);
            const ntsa::Error error = timer->schedule(timer->currentTime());
            BSLS_ASSERT(error == ntsa::Error());
            NTCI_LOG_DEBUG("Timer scheduled");
        }
    }

    void producerThreadWithStrand(int strandIndex)
    {
        NTCI_LOG_CONTEXT();

        while (true) {
            //code below could be written easier if letting d_numTimersToProduce go below zero
            //e.g. while(d_numTimersToProduce.subtract(1) >= 0)
            int current = d_numTimersToProduce.load();
            if (current == 0)
                return;
            int next    = current - 1;
            int swapped = d_numTimersToProduce.testAndSwap(current, next);
            if (swapped != current)
                continue;

            ntca::TimerOptions timerOptions;
            timerOptions.setOneShot(true);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
            timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
            timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

            ntci::TimerCallback callback(
                NTCCFG_BIND(&test::MtTestSuite::processTimerAtStrand,
                            this,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            strandIndex),
                static_cast<bsl::shared_ptr<ntci::Strand> >(
                    d_strands[strandIndex]->first));

            bsl::shared_ptr<ntci::Timer> timer =
                d_driver->chronology().createTimer(timerOptions,
                                                   callback,
                                                   &d_ta);
            timer->schedule(timer->currentTime());
            NTCI_LOG_DEBUG("Timer scheduled");
        }
    }

    void waitAllOneShotTimersConsumed()
    {
        d_allOneShotConsumedMutex.lock();
        while (d_numOneShotTimersToConsume != 0) {
            d_allOneShotConsumedCondition.wait(&d_allOneShotConsumedMutex);
        }
        d_allOneShotConsumedMutex.unlock();
    }

    void processTimer(const bsl::shared_ptr<ntci::Timer>& timer,
                      const ntca::TimerEvent&             event)
    {
        NTCI_LOG_CONTEXT();

        int res = d_numOneShotTimersToConsume.subtract(1);

        NTCI_LOG_DEBUG("processTimer called: remaining = %d", res);

        if (res == 0) {
            ntccfg::ConditionMutexGuard lock(&d_allOneShotConsumedMutex);
            d_allOneShotConsumedCondition.signal();
        }

        NTCCFG_TEST_ASSERT(res >= 0);
        if (res == 0 && d_numExpectedCloseEvents.load() == 0) {
            d_driver->stop();
        }
    }

    void processTimerAtStrand(const bsl::shared_ptr<ntci::Timer>& timer,
                              const ntca::TimerEvent&             event,
                              int                                 strandIndex)
    {
        NTCI_LOG_CONTEXT();

        bsls::AtomicBool& executionFlag = d_strands[strandIndex]->second;
        bool              prev = executionFlag.testAndSwap(false, true);
        NTCCFG_TEST_FALSE(prev);

        int res = d_numOneShotTimersToConsume.subtract(1);

        NTCI_LOG_DEBUG("processTimerAtStrand called: remaining = %d", res);

        NTCCFG_TEST_ASSERT(res >= 0);
        if (res == 0 && d_numExpectedCloseEvents.load() == 0) {
            d_driver->stop();
        }

        prev = executionFlag.testAndSwap(true, false);
        NTCCFG_TEST_TRUE(prev);
    }

    void processPeriodicTimer(const bsl::shared_ptr<ntci::Timer>& timer,
                              const ntca::TimerEvent&             event)
    {
        NTCI_LOG_CONTEXT();
        NTCI_LOG_DEBUG("processPeriodicTimer called");
        if (event.type() == ntca::TimerEventType::e_DEADLINE) {
            ++d_numPeriodicTimersShot;
        }
        else if (event.type() == ntca::TimerEventType::e_CLOSED) {
            int res = d_numExpectedCloseEvents.subtract(1);
            NTCCFG_TEST_ASSERT(res >= 0);
            if (res == 0 && d_numOneShotTimersToConsume.load() == 0) {
                d_driver->stop();
            }
        }
    }

    void waitConsumers()
    {
        d_consumers->joinAll();
    }

    void waitProducers()
    {
        d_producers->joinAll();
    }

    void waitAll()
    {
        waitProducers();
        waitConsumers();
    }

    void createStrands(int numStrands)
    {
        for (int i = 0; i < numStrands; ++i) {
            bsl::shared_ptr<StrandAndFlag> pair =
                bsl::allocate_shared<StrandAndFlag>(&d_ta);

            pair->first = d_driver->createStrand();
            pair->second.store(false);
            d_strands.push_back(bslmf::MovableRefUtil::move(pair));
        }
    }

    ntccfg::TestAllocator d_ta;

    bsl::shared_ptr<test::MtDriver> d_driver;

    bsl::shared_ptr<bslmt::ThreadGroup> d_consumers;
    bsl::shared_ptr<bslmt::ThreadGroup> d_producers;

    bsls::AtomicInt        d_numTimersToProduce;
    bsls::AtomicInt        d_numOneShotTimersToConsume;
    bsls::AtomicInt        d_numExpectedCloseEvents;
    bsls::AtomicInt        d_numPeriodicTimersShot;
    ntccfg::Condition      d_allOneShotConsumedCondition;
    ntccfg::ConditionMutex d_allOneShotConsumedMutex;

    typedef bsl::pair<bsl::shared_ptr<ntcs::Strand>, bsls::AtomicBool>
        StrandAndFlag;

    bsl::vector<bsl::shared_ptr<StrandAndFlag> > d_strands;
};

}  // close namespace 'test'

NTCCFG_TEST_CASE(1)
{
    // Concern: check Timer::id()

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        ntca::TimerOptions timerOptions =
            s.createOptionsAllDisabled(test::k_TIMER_ID_0);

        bsl::shared_ptr<ntci::Timer> timer1 =
            s.chronology->createTimer(timerOptions, s.timerCallback, &s.ta);

        timerOptions.setId(test::k_TIMER_ID_1);
        bsl::shared_ptr<ntci::Timer> timer2 =
            s.chronology->createTimer(timerOptions, s.timerCallback, &s.ta);

        NTCCFG_TEST_EQ(timer1->id(), test::k_TIMER_ID_0);
        NTCCFG_TEST_EQ(timer2->id(), test::k_TIMER_ID_1);

        s.chronology->clearTimers();
    }
}

NTCCFG_TEST_CASE(2)
{
    // Concern: check Timer::oneShot()

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        ntca::TimerOptions timerOptions =
            s.createOptionsAllDisabled(test::k_TIMER_ID_1);
        timerOptions.setOneShot(true);

        bsl::shared_ptr<ntci::Timer> timer1 =
            s.chronology->createTimer(timerOptions, s.timerCallback, &s.ta);

        timerOptions.setId(test::k_TIMER_ID_2);
        timerOptions.setOneShot(false);
        bsl::shared_ptr<ntci::Timer> timer2 =
            s.chronology->createTimer(timerOptions, s.timerCallback, &s.ta);

        NTCCFG_TEST_TRUE(timer1->oneShot());
        NTCCFG_TEST_FALSE(timer2->oneShot());

        s.chronology->clearTimers();
    }
}

NTCCFG_TEST_CASE(3)
{
    // Concern: check Timer::handle()

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        int handle1 = 0;
        int handle2 = 0;

        ntca::TimerOptions timerOptions =
            s.createOptionsAllDisabled(test::k_TIMER_ID_2);
        timerOptions.setHandle(&handle1);

        bsl::shared_ptr<ntci::Timer> timer1 =
            s.chronology->createTimer(timerOptions, s.timerCallback, &s.ta);

        timerOptions.setId(test::k_TIMER_ID_3);
        timerOptions.setHandle(&handle2);
        bsl::shared_ptr<ntci::Timer> timer2 =
            s.chronology->createTimer(timerOptions, s.timerCallback, &s.ta);

        NTCCFG_TEST_EQ(timer1->handle(), &handle1);
        NTCCFG_TEST_EQ(timer2->handle(), &handle2);

        s.chronology->clearTimers();
    }
}

NTCCFG_TEST_CASE(4)
{
    // Concern: check Timer::strand()

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        ntca::TimerOptions timerOptions =
            s.createOptionsAllDisabled(test::k_TIMER_ID_4);

        bsl::shared_ptr<test::StrandMock> strand1;
        bsl::shared_ptr<test::StrandMock> strand2;
        strand1.createInplace(&s.ta);
        strand2.createInplace(&s.ta);

        ntci::TimerCallback timerCallback1(
            NTCCFG_BIND(&test::TimerCallbackCollector::processTimer,
                        s.callbacks.get(),
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2),
            static_cast<bsl::shared_ptr<ntci::Strand> >(strand1));

        ntci::TimerCallback timerCallback2(
            NTCCFG_BIND(&test::TimerCallbackCollector::processTimer,
                        s.callbacks.get(),
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2),
            static_cast<bsl::shared_ptr<ntci::Strand> >(strand2));

        bsl::shared_ptr<ntci::Timer> timer1 =
            s.chronology->createTimer(timerOptions, timerCallback1, &s.ta);

        bsl::shared_ptr<ntci::Timer> timer2 =
            s.chronology->createTimer(timerOptions, timerCallback2, &s.ta);

        NTCCFG_TEST_EQ(timer1->strand().get(), strand1.get());
        NTCCFG_TEST_EQ(timer2->strand().get(), strand2.get());

        s.chronology->clearTimers();
    }
}

NTCCFG_TEST_CASE(5)
{
    // Concern: check Timer::threadIndex()

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        ntca::TimerOptions timerOptions =
            s.createOptionsAllDisabled(test::k_TIMER_ID_5);
        bsl::shared_ptr<ntci::Timer> timer =
            s.chronology->createTimer(timerOptions, s.timerCallback, &s.ta);
        NTCCFG_TEST_EQ(timer->threadIndex(), test::k_THREAD_INDEX);
        s.chronology->clearTimers();
    }
}

NTCCFG_TEST_CASE(6)
{
    // Concern: check Timer::currentTime()

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        ntca::TimerOptions timerOptions =
            s.createOptionsAllDisabled(test::k_TIMER_ID_0);

        bsl::shared_ptr<ntci::Timer> timer =
            s.chronology->createTimer(timerOptions, s.timerCallback, &s.ta);

        NTCCFG_TEST_EQ(timer->currentTime(), s.clock.currentTime());
        s.clock.advance(s.oneHour);
        NTCCFG_TEST_EQ(timer->currentTime(), s.clock.currentTime());

        s.chronology->clearTimers();
    }
}

NTCCFG_TEST_CASE(7)
{
    // Concern: check Chronology::currentTime

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        const bsls::TimeInterval now = s.clock.currentTime();
        NTCCFG_TEST_EQ(s.chronology->currentTime(), now);

        s.clock.advance(s.oneSecond);
        NTCCFG_TEST_EQ(s.chronology->currentTime(), now + s.oneSecond);
        NTCCFG_TEST_EQ(s.chronology->currentTime(), s.clock.currentTime());

        s.clock.advance(s.oneHour);
        NTCCFG_TEST_EQ(s.chronology->currentTime(),
                       now + s.oneSecond + s.oneHour);
        NTCCFG_TEST_EQ(s.chronology->currentTime(), s.clock.currentTime());
    }
}

NTCCFG_TEST_CASE(8)
{
    // Concern: check hasAnyRegistered and hasAnyScheduled

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        ntca::TimerOptions timerOptions =
            s.createOptionsAllDisabled(test::k_TIMER_ID_1);
        timerOptions.setOneShot(true);
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

        NTCI_LOG_DEBUG("Part 1, create and schedule timer");
        {
            bsl::shared_ptr<ntci::Timer> timer =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);
            NTCCFG_TEST_TRUE(s.chronology->hasAnyRegistered());
            NTCCFG_TEST_FALSE(s.chronology->hasAnyScheduled());

            //to be fired immediately
            const ntsa::Error error =
                timer->schedule(s.chronology->currentTime());
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();

            NTCCFG_TEST_TRUE(s.chronology->hasAnyRegistered());
            NTCCFG_TEST_TRUE(s.chronology->hasAnyScheduled());
        }
        NTCI_LOG_DEBUG("Part 2, announce");
        {
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_1,
                ntca::TimerEventType::e_DEADLINE);
            NTCCFG_TEST_FALSE(s.chronology->hasAnyRegistered());
            NTCCFG_TEST_FALSE(s.chronology->hasAnyScheduled());
        }
    }
}

NTCCFG_TEST_CASE(9)
{
    // Concern: check earliest and timeoutInterval
    // add timer to be shot at t1, check earliest and timeoutInterval
    // then add timer to be shot at t0 and check that earliest and timeoutInterval changed

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        ntca::TimerOptions timerOptions =
            s.createOptionsAllDisabled(test::k_TIMER_ID_0);
        timerOptions.setOneShot(true);
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

        NTCI_LOG_DEBUG("Part 0, check no earliest or timeoutInterval");
        {
            const bdlb::NullableValue<bsls::TimeInterval> earliest =
                s.chronology->earliest();
            const bdlb::NullableValue<bsls::TimeInterval> timeoutInterval =
                s.chronology->timeoutInterval();
            NTCCFG_TEST_TRUE(earliest.isNull());
            NTCCFG_TEST_TRUE(timeoutInterval.isNull());
        }

        NTCI_LOG_DEBUG(
            "Part 1, create one timer to be due at t1, check methods");
        {
            bsl::shared_ptr<ntci::Timer> timer =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);
            const ntsa::Error error =
                timer->schedule(s.chronology->currentTime() + s.oneHour);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();
            s.validateRegisteredAndScheduled(1, 1);

            const bdlb::NullableValue<bsls::TimeInterval> earliest =
                s.chronology->earliest();
            const bdlb::NullableValue<bsls::TimeInterval> timeoutInterval =
                s.chronology->timeoutInterval();
            NTCCFG_TEST_EQ(earliest.value(),
                           s.clock.currentTime() + s.oneHour);
            NTCCFG_TEST_EQ(timeoutInterval.value(), s.oneHour);
        }
        NTCI_LOG_DEBUG("Part 2, advance a bit and check values again");
        {
            s.clock.advance(s.oneSecond);
            s.chronology->announce();
            const bdlb::NullableValue<bsls::TimeInterval> earliest =
                s.chronology->earliest();
            const bdlb::NullableValue<bsls::TimeInterval> timeoutInterval =
                s.chronology->timeoutInterval();
            NTCCFG_TEST_EQ(earliest.value(),
                           s.clock.currentTime() + s.oneHour - s.oneSecond);
            NTCCFG_TEST_EQ(timeoutInterval.value(), s.oneHour - s.oneSecond);
        }
        NTCI_LOG_DEBUG("Part 3, add another timer which should fire earlier");
        {
            bsl::shared_ptr<ntci::Timer> timer =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);
            const ntsa::Error error =
                timer->schedule(s.chronology->currentTime() + s.oneMinute);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();
            s.validateRegisteredAndScheduled(2, 2);

            const bdlb::NullableValue<bsls::TimeInterval> earliest =
                s.chronology->earliest();
            const bdlb::NullableValue<bsls::TimeInterval> timeoutInterval =
                s.chronology->timeoutInterval();
            NTCCFG_TEST_EQ(earliest.value(),
                           s.clock.currentTime() + s.oneMinute);
            NTCCFG_TEST_EQ(timeoutInterval.value(), s.oneMinute);
        }
        s.chronology->closeAll();
    }
}

NTCCFG_TEST_CASE(10)
{
    // Concern: test Chronology::clearFunctions

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        int                           callCounter = 0;
        const ntci::Executor::Functor f =
            NTCCFG_BIND(&test::TestSuite::incrementCallback,
                        bsl::ref(callCounter));

        ntci::Executor::FunctorSequence seq;
        seq.push_back(f);
        seq.push_back(f);
        seq.push_back(f);
        s.chronology->moveAndExecute(&seq, f);
        NTCCFG_TEST_EQ(s.chronology->numDeferred(), 4);

        s.driver->validateInterruptAllCalled();

        s.chronology->clearFunctions();

        s.chronology->announce();
        NTCCFG_TEST_EQ(callCounter, 0);
    }
}

NTCCFG_TEST_CASE(11)
{
    // Concern: test Chronology::drain

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        int                           callCounter = 0;
        const ntci::Executor::Functor f =
            NTCCFG_BIND(&test::TestSuite::incrementCallback,
                        bsl::ref(callCounter));

        ntci::Executor::FunctorSequence seq;
        seq.push_back(f);
        seq.push_back(f);
        seq.push_back(f);
        s.chronology->moveAndExecute(&seq, f);
        NTCCFG_TEST_EQ(s.chronology->numDeferred(), 4);

        s.driver->validateInterruptAllCalled();

        s.chronology->drain();

        NTCCFG_TEST_EQ(callCounter, 4);
    }
}

NTCCFG_TEST_CASE(12)
{
    // Concern: Test single timer creation and destruction without being
    // scheduled

    // Plan: create one timer, then it should be aytomaticaly deleted.
    // Create two timers, memory left from the first timer should be reused.
    // Create 3 timers, memory from previous two timers should be reused.

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        void* address1 = 0;
        void* address2 = 0;

        ntca::TimerOptions timerOptions =
            s.createOptionsAllDisabled(test::k_TIMER_ID_0);
        timerOptions.setOneShot(true);
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

        NTCI_LOG_DEBUG("Part 1, create one timer");
        {
            bsl::shared_ptr<ntci::Timer> timer1 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            address1 = timer1.get();

            NTCCFG_TEST_EQ(s.chronology->numRegistered(), 1);
        }
        NTCCFG_TEST_EQ(s.chronology->numRegistered(), 0);

        NTCI_LOG_DEBUG("Part 2, create two timers, address1 should be reused");
        {
            bsl::shared_ptr<ntci::Timer> timer1 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            NTCCFG_TEST_EQ(timer1.get(), address1);

            NTCCFG_TEST_EQ(s.chronology->numRegistered(), 1);

            bsl::shared_ptr<ntci::Timer> timer2 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            address2 = timer2.get();

            NTCCFG_TEST_EQ(s.chronology->numRegistered(), 2);
        }

        NTCCFG_TEST_EQ(s.chronology->numRegistered(), 0);

        NTCI_LOG_DEBUG(
            "Part 3, create 3 timers, address 1 & 2 should be reused");
        {
            bsl::shared_ptr<ntci::Timer> timer1 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            NTCCFG_TEST_EQ(timer1.get(), address1);
            NTCCFG_TEST_EQ(s.chronology->numRegistered(), 1);

            bsl::shared_ptr<ntci::Timer> timer2 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            NTCCFG_TEST_EQ(timer2.get(), address2);
            NTCCFG_TEST_EQ(s.chronology->numRegistered(), 2);

            bsl::shared_ptr<ntci::Timer> timer3 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            NTCCFG_TEST_EQ(s.chronology->numRegistered(), 3);
        }
    }
}

NTCCFG_TEST_CASE(13)
{
    // Concern: Test single timer creation and destruction without being
    // scheduled with a weak reference outstanding.
    // Plan: create one timer (sptr), create weak reference to it, let sptr
    // destroy, check that weak ptr cannot be transformed to sptr.
    // Repeat the same for two ptrs
    // Repeat the same for three ptrs

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        void* address1 = 0;
        void* address2 = 0;

        ntca::TimerOptions timerOptions =
            s.createOptionsAllDisabled(test::k_TIMER_ID_0);
        timerOptions.setOneShot(true);
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

        NTCI_LOG_DEBUG("Part 1, cannot obtain strong reference from weak one");
        {
            bsl::weak_ptr<ntci::Timer> weakTimer1;

            {
                bsl::shared_ptr<ntci::Timer> timer1 =
                    s.chronology->createTimer(timerOptions,
                                              s.timerCallback,
                                              &s.ta);

                address1 = timer1.get();

                NTCCFG_TEST_EQ(s.chronology->numRegistered(), 1);

                weakTimer1 = timer1;
            }

            {
                bsl::shared_ptr<ntci::Timer> timer1 = weakTimer1.lock();
                NTCCFG_TEST_FALSE(timer1);
            }
        }
        NTCCFG_TEST_EQ(s.chronology->numRegistered(), 0);

        NTCI_LOG_DEBUG("Part 2");
        {
            bsl::weak_ptr<ntci::Timer> weakTimer1;
            bsl::weak_ptr<ntci::Timer> weakTimer2;

            {
                bsl::shared_ptr<ntci::Timer> timer1 =
                    s.chronology->createTimer(timerOptions,
                                              s.timerCallback,
                                              &s.ta);

                NTCCFG_TEST_EQ(timer1.get(), address1);
                NTCCFG_TEST_EQ(s.chronology->numRegistered(), 1);

                weakTimer1 = timer1;

                bsl::shared_ptr<ntci::Timer> timer2 =
                    s.chronology->createTimer(timerOptions,
                                              s.timerCallback,
                                              &s.ta);

                address2 = timer2.get();

                NTCCFG_TEST_EQ(s.chronology->numRegistered(), 2);

                weakTimer2 = timer2;
            }

            {
                bsl::shared_ptr<ntci::Timer> timer1 = weakTimer1.lock();
                NTCCFG_TEST_FALSE(timer1);

                bsl::shared_ptr<ntci::Timer> timer2 = weakTimer2.lock();
                NTCCFG_TEST_FALSE(timer2);
            }
        }
        NTCCFG_TEST_EQ(s.chronology->numRegistered(), 0);

        NTCI_LOG_DEBUG("Part 3");
        {
            bsl::weak_ptr<ntci::Timer> weakTimer1;
            bsl::weak_ptr<ntci::Timer> weakTimer2;
            bsl::weak_ptr<ntci::Timer> weakTimer3;

            {
                bsl::shared_ptr<ntci::Timer> timer1 =
                    s.chronology->createTimer(timerOptions,
                                              s.timerCallback,
                                              &s.ta);

                NTCCFG_TEST_EQ(timer1.get(), address1);
                NTCCFG_TEST_EQ(s.chronology->numRegistered(), 1);

                weakTimer1 = timer1;

                bsl::shared_ptr<ntci::Timer> timer2 =
                    s.chronology->createTimer(timerOptions,
                                              s.timerCallback,
                                              &s.ta);

                NTCCFG_TEST_EQ(timer2.get(), address2);
                NTCCFG_TEST_EQ(s.chronology->numRegistered(), 2);

                weakTimer2 = timer2;

                bsl::shared_ptr<ntci::Timer> timer3 =
                    s.chronology->createTimer(timerOptions,
                                              s.timerCallback,
                                              &s.ta);

                NTCCFG_TEST_EQ(s.chronology->numRegistered(), 3);

                weakTimer3 = timer3;
            }

            {
                bsl::shared_ptr<ntci::Timer> timer1 = weakTimer1.lock();
                NTCCFG_TEST_FALSE(timer1);

                bsl::shared_ptr<ntci::Timer> timer2 = weakTimer2.lock();
                NTCCFG_TEST_FALSE(timer2);

                bsl::shared_ptr<ntci::Timer> timer3 = weakTimer2.lock();
                NTCCFG_TEST_FALSE(timer3);
            }
        }
    }
}

NTCCFG_TEST_CASE(14)
{
    // Concern: Test scheduling a one-shot timer.
    // Plan: create a timer and schedule it to be due now
    // Announce and check that timer was fired and then removed

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        {
            ntca::TimerOptions timerOptions =
                s.createOptionsAllDisabled(test::k_TIMER_ID_0);
            timerOptions.setOneShot(true);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

            bsl::shared_ptr<ntci::Timer> timer =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            s.validateRegisteredAndScheduled(1, 0);

            const ntsa::Error error = timer->schedule(s.clock.currentTime());
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();
            s.validateRegisteredAndScheduled(1, 1);
        }

        s.validateRegisteredAndScheduled(1, 1);

        s.chronology->announce();
        s.callbacks->validateEventReceived(test::k_TIMER_ID_0,
                                           ntca::TimerEventType::e_DEADLINE);

        s.validateRegisteredAndScheduled(0, 0);
    }
}

NTCCFG_TEST_CASE(15)
{
    // Concern: Test scheduling a non-recurring timer then closing it, when
    // the close event is disabled.
    // Plan: create a timer, schedule it to be due in the future
    // announce and check that it is not fired
    // close the timers and check that no callback was called

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        bsl::shared_ptr<ntci::Timer> timer;
        {
            ntca::TimerOptions timerOptions =
                s.createOptionsAllDisabled(test::k_TIMER_ID_0);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

            timer = s.chronology->createTimer(timerOptions,
                                              s.timerCallback,
                                              &s.ta);

            s.validateRegisteredAndScheduled(1, 0);

            const ntsa::Error error =
                timer->schedule(s.clock.currentTime() + s.oneMinute);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();
            s.validateRegisteredAndScheduled(1, 1);
        }

        s.validateRegisteredAndScheduled(1, 1);

        s.chronology->announce();

        NTCI_LOG_DEBUG("Closing timer");

        timer->close();
        timer.reset();
        s.validateRegisteredAndScheduled(0, 0);
    }
}

NTCCFG_TEST_CASE(16)
{
    // Concern: Test scheduling a non-recurring timer then closing it, when
    // the close event is enabled.
    // Plan: create and schedule timer t be due now
    // fire the timer, check deadlineevent received
    // mannually close the timer, check closed event received

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        bsl::shared_ptr<ntci::Timer> timerExt;
        {
            ntca::TimerOptions timerOptions =
                s.createOptionsAllDisabled(test::k_TIMER_ID_0);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
            timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);

            bsl::shared_ptr<ntci::Timer> timer =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            s.validateRegisteredAndScheduled(1, 0);

            const ntsa::Error error = timer->schedule(s.clock.currentTime());
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();
            s.validateRegisteredAndScheduled(1, 1);

            timerExt = timer;
        }

        s.validateRegisteredAndScheduled(1, 1);

        s.chronology->announce();
        s.callbacks->validateEventReceived(test::k_TIMER_ID_0,
                                           ntca::TimerEventType::e_DEADLINE);

        s.validateRegisteredAndScheduled(1, 0);

        NTCI_LOG_DEBUG("Closing and resetting the timer");

        timerExt->close();
        s.driver->validateInterruptAllCalled();
        timerExt.reset();
        s.validateRegisteredAndScheduled(1, 0);

        s.chronology->announce();
        s.callbacks->validateEventReceived(test::k_TIMER_ID_0,
                                           ntca::TimerEventType::e_CLOSED);
        s.validateRegisteredAndScheduled(0, 0);
    }
}

NTCCFG_TEST_CASE(17)
{
    // Concern: Test clearing the chronology when a timer is pending
    // Plan: create and schedule a timer to be due at the future
    // announce and check that timer did not fired
    // clear chronology

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();
        {
            ntca::TimerOptions timerOptions =
                s.createOptionsAllDisabled(test::k_TIMER_ID_1);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
            timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);

            bsl::shared_ptr<ntci::Timer> timer =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            s.validateRegisteredAndScheduled(1, 0);

            const ntsa::Error error =
                timer->schedule(s.chronology->currentTime() + s.oneSecond);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();
            s.validateRegisteredAndScheduled(1, 1);
        }
        s.chronology->announce();

        s.validateRegisteredAndScheduled(1, 1);

        s.chronology->clear();
        s.validateRegisteredAndScheduled(0, 0);
    }
}

NTCCFG_TEST_CASE(18)
{
    // Concern: Test scheduling a non-recurring timer then closing it
    // before it is due.

    // Plan: Create one timer
    // schedule it and then close.
    // Check that e_DEADLINE event was not fired

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        bsl::shared_ptr<ntci::Timer> timerExt;
        NTCI_LOG_DEBUG("Part 1, schedule one timer");
        {
            ntca::TimerOptions timerOptions =
                s.createOptionsAllDisabled(test::k_TIMER_ID_3);
            timerOptions.setOneShot(false);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
            timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);

            bsl::shared_ptr<ntci::Timer> timer =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            s.validateRegisteredAndScheduled(1, 0);

            const ntsa::Error error =
                timer->schedule(s.clock.currentTime() + s.oneHour);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();
            s.validateRegisteredAndScheduled(1, 1);

            timerExt = timer;
        }

        NTCI_LOG_DEBUG(
            "Part 2, check that it was not triggered at announcement");
        {
            s.chronology->announce();
            s.validateRegisteredAndScheduled(1, 1);
        }
        NTCI_LOG_DEBUG("Part 3, close it and reset the sptr");
        {
            timerExt->close();
            s.driver->validateInterruptAllCalled();
            timerExt.reset();
            s.validateRegisteredAndScheduled(1, 0);

            s.chronology->announce();
            s.callbacks->validateEventReceived(test::k_TIMER_ID_3,
                                               ntca::TimerEventType::e_CLOSED);
            s.validateRegisteredAndScheduled(0, 0);
        }
    }
}

NTCCFG_TEST_CASE(19)
{
    // Concern: Test scheduling a one-shot timer then cancelling it before it
    // is due, then schedule it again.  After rescheduling wait till it is
    // due, then fail to reschedule

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        ntca::TimerOptions timerOptions =
            s.createOptionsAllDisabled(test::k_TIMER_ID_4);
        timerOptions.setOneShot(true);
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
        timerOptions.showEvent(ntca::TimerEventType::e_CANCELED);

        bsl::shared_ptr<ntci::Timer> timerExt;
        NTCI_LOG_DEBUG("Part 1, schedule a timer");
        {
            bsl::shared_ptr<ntci::Timer> timer =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            s.validateRegisteredAndScheduled(1, 0);

            const ntsa::Error error =
                timer->schedule(s.clock.currentTime() + s.oneHour);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();
            timerExt = timer;

            s.validateRegisteredAndScheduled(1, 1);
        }

        NTCI_LOG_DEBUG("Part 2, move time slightly, timer should not fire");
        {
            s.clock.advance(s.oneMinute);
            s.chronology->announce();

            s.validateRegisteredAndScheduled(1, 1);
        }

        NTCI_LOG_DEBUG(
            "Part 3, cancel timer and check that cancel event was received");
        {
            timerExt->cancel();
            s.driver->validateInterruptAllCalled();
            s.validateRegisteredAndScheduled(1, 0);

            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_4,
                ntca::TimerEventType::e_CANCELED);
            s.validateRegisteredAndScheduled(1, 0);
        }

        NTCI_LOG_DEBUG("Part 4, reschedule timer and wait for it to shot");
        {
            const ntsa::Error error =
                timerExt->schedule(s.clock.currentTime() + s.oneMinute);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();
            s.validateRegisteredAndScheduled(1, 1);

            s.clock.advance(s.oneMinute);
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_4,
                ntca::TimerEventType::e_DEADLINE);

            s.validateRegisteredAndScheduled(1, 0);
        }

        NTCI_LOG_DEBUG("Part 5, try to schedule timer again. Should fail");
        {
            const ntsa::Error error =
                timerExt->schedule(s.clock.currentTime() + s.oneMinute);
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_INVALID);

            s.clock.advance(s.oneMinute);
            s.chronology->announce();

            s.validateRegisteredAndScheduled(1, 0);

            timerExt.reset();
            s.validateRegisteredAndScheduled(0, 0);
        }
    }
}

NTCCFG_TEST_CASE(20)
{
    // Concern: Test scheduling a non-recurring timer then cancelling it
    // before it is due, then rescheduling it
    // Plan: create and schedule a timer to be due at the future
    // advance time slightly so that it is no shot and cancel it
    // reschedule the timer
    // cancel it again

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        bsl::shared_ptr<ntci::Timer> timerExt;
        NTCI_LOG_DEBUG("Part 1,schedule a timer");
        {
            ntca::TimerOptions timerOptions =
                s.createOptionsAllDisabled(test::k_TIMER_ID_5);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
            timerOptions.showEvent(ntca::TimerEventType::e_CANCELED);

            bsl::shared_ptr<ntci::Timer> timer =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            s.validateRegisteredAndScheduled(1, 0);

            const ntsa::Error error =
                timer->schedule(s.clock.currentTime() + s.oneMinute);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();
            timerExt = timer;

            s.validateRegisteredAndScheduled(1, 1);
        }
        NTCI_LOG_DEBUG("Part 2, cancel the timer and ensure that timer was "
                       "not triggered");
        {
            s.clock.advance(s.oneSecond);
            s.chronology->announce();
            s.validateRegisteredAndScheduled(1, 1);

            const ntsa::Error error = timerExt->cancel();
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_CANCELLED);
            s.driver->validateInterruptAllCalled();
            s.validateRegisteredAndScheduled(1, 0);

            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_5,
                ntca::TimerEventType::e_CANCELED);

            s.validateRegisteredAndScheduled(1, 0);
        }

        NTCI_LOG_DEBUG("Part 3, reschedule the timer");
        {
            const ntsa::Error error =
                timerExt->schedule(s.clock.currentTime() + s.oneMinute);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();
            s.validateRegisteredAndScheduled(1, 1);
        }

        NTCI_LOG_DEBUG("Part 4, cancel the timer again and delete it");
        {
            const ntsa::Error error = timerExt->cancel();
            s.driver->validateInterruptAllCalled();
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_CANCELLED);
            timerExt.reset();

            s.validateRegisteredAndScheduled(1, 0);

            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_5,
                ntca::TimerEventType::e_CANCELED);

            s.validateRegisteredAndScheduled(0, 0);
        }
    }
}

NTCCFG_TEST_CASE(21)
{
    // Concern: Test announcement of 1 timer being due at a time point.
    //
    // Plan: Add one timer at t0, one timer at t1, and one timer at t2.
    // Advance time to t0, announce any due timers, verify the expected 1
    // timer was announced. Advance time to t1, announce any due timers,
    // verify the expected 1 timer was announced. Advance time to t2,
    // announce any due timers, verify the expected 1 timer was announced.

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        ntca::TimerOptions timerOptions =
            s.createOptionsAllDisabled(test::k_TIMER_ID_0);
        timerOptions.setOneShot(true);
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

        ntsa::Error error;
        NTCI_LOG_DEBUG("Part 1, create 3 timers");
        {
            timerOptions.setId(test::k_TIMER_ID_0);
            bsl::shared_ptr<ntci::Timer> timer1 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);
            s.validateRegisteredAndScheduled(1, 0);

            timerOptions.setId(test::k_TIMER_ID_1);
            bsl::shared_ptr<ntci::Timer> timer2 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);
            s.validateRegisteredAndScheduled(2, 0);

            timerOptions.setId(test::k_TIMER_ID_2);
            bsl::shared_ptr<ntci::Timer> timer3 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);
            s.validateRegisteredAndScheduled(3, 0);

            error = timer1->schedule(s.chronology->currentTime() + s.oneHour);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();
            s.validateRegisteredAndScheduled(3, 1);

            error = timer2->schedule(s.chronology->currentTime() + s.oneHour +
                                     s.oneHour);
            NTCCFG_TEST_OK(error);
            s.validateRegisteredAndScheduled(3, 2);

            error = timer3->schedule(s.chronology->currentTime() + s.oneHour +
                                     s.oneHour + s.oneHour);
            NTCCFG_TEST_OK(error);
            s.validateRegisteredAndScheduled(3, 3);

            s.chronology->announce();
            s.validateRegisteredAndScheduled(3, 3);
        }

        NTCI_LOG_DEBUG("Part 2, advance time to trigger timers");
        {
            s.clock.advance(s.oneHour);
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_0,
                ntca::TimerEventType::e_DEADLINE);

            s.validateRegisteredAndScheduled(2, 2);

            s.clock.advance(s.oneHour);
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_1,
                ntca::TimerEventType::e_DEADLINE);

            s.validateRegisteredAndScheduled(1, 1);

            s.clock.advance(s.oneHour);
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_2,
                ntca::TimerEventType::e_DEADLINE);

            s.validateRegisteredAndScheduled(0, 0);
        }
    }
}

NTCCFG_TEST_CASE(22)
{
    // Concern: Test announcement of 2 timers being due at a time point.
    //
    // Plan: Add two timers at t0, two timers at t1, and two timers at t2.
    // Advance time to t0, announce any due timers, verify the expected 2
    // timers were announced. Advance time to t1, announce any due timers,
    // verify the expected 2 timers were announced. Advance time to t2,
    // announce any due timers, verify the expected 2 timers were announced.

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        ntca::TimerOptions timerOptions =
            s.createOptionsAllDisabled(test::k_TIMER_ID_0);
        timerOptions.setOneShot(true);
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

        ntsa::Error error;

        NTCI_LOG_DEBUG("Part 1, create and schedule two t0 timers");
        {
            timerOptions.setId(test::k_TIMER_ID_0);
            bsl::shared_ptr<ntci::Timer> timer1 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            error = timer1->schedule(s.chronology->currentTime() + s.oneHour);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();

            timerOptions.setId(test::k_TIMER_ID_1);
            bsl::shared_ptr<ntci::Timer> timer2 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            error = timer2->schedule(s.chronology->currentTime() + s.oneHour);
            NTCCFG_TEST_OK(error);

            s.validateRegisteredAndScheduled(2, 2);
        }

        NTCI_LOG_DEBUG("Part 2, create and schedule two t1 timers");
        {
            timerOptions.setId(test::k_TIMER_ID_2);
            bsl::shared_ptr<ntci::Timer> timer1 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            error = timer1->schedule(s.chronology->currentTime() + s.oneHour +
                                     s.oneHour);
            NTCCFG_TEST_OK(error);

            timerOptions.setId(test::k_TIMER_ID_3);
            bsl::shared_ptr<ntci::Timer> timer2 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            error = timer2->schedule(s.chronology->currentTime() + s.oneHour +
                                     s.oneHour);
            NTCCFG_TEST_OK(error);

            s.validateRegisteredAndScheduled(4, 4);
        }

        NTCI_LOG_DEBUG("Part 3, trigger t0 timers");
        {
            s.clock.advance(s.oneHour);
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_0,
                ntca::TimerEventType::e_DEADLINE);
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_1,
                ntca::TimerEventType::e_DEADLINE);

            s.validateRegisteredAndScheduled(2, 2);
        }

        NTCI_LOG_DEBUG("Part 4, trigger t1 timers");
        {
            s.clock.advance(s.oneHour);
            s.chronology->announce();

            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_2,
                ntca::TimerEventType::e_DEADLINE);
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_3,
                ntca::TimerEventType::e_DEADLINE);

            s.validateRegisteredAndScheduled(0, 0);
        }
    }
}

NTCCFG_TEST_CASE(23)
{
    // Concern: Test announcement of multiple timers being due at multiple
    // times in the past.
    //
    // Plan: Add two timers at t0, two timers at t1, and two timers at t2.
    // Advance time to t1, announce any due timers, verify the expected 4
    // timers were announced. Advance time to t2, announce any due timers,
    // verify the expected 2 timers were announced.

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        ntsa::Error error;

        ntca::TimerOptions timerOptions =
            s.createOptionsAllDisabled(test::k_TIMER_ID_0);
        timerOptions.setOneShot(true);
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

        NTCI_LOG_DEBUG("Part 1, create and schedule two t0 timers");
        {
            timerOptions.setId(test::k_TIMER_ID_0);
            bsl::shared_ptr<ntci::Timer> timer1 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            error = timer1->schedule(s.chronology->currentTime() + s.oneHour);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();

            timerOptions.setId(test::k_TIMER_ID_1);
            bsl::shared_ptr<ntci::Timer> timer2 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            error = timer2->schedule(s.chronology->currentTime() + s.oneHour);
            NTCCFG_TEST_OK(error);

            s.validateRegisteredAndScheduled(2, 2);
        }

        NTCI_LOG_DEBUG("Part 2, create and schedule two t1 timers");
        {
            timerOptions.setId(test::k_TIMER_ID_2);
            bsl::shared_ptr<ntci::Timer> timer1 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            error = timer1->schedule(s.chronology->currentTime() + s.oneHour +
                                     s.oneHour);
            NTCCFG_TEST_OK(error);

            timerOptions.setId(test::k_TIMER_ID_3);
            bsl::shared_ptr<ntci::Timer> timer2 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            error = timer2->schedule(s.chronology->currentTime() + s.oneHour +
                                     s.oneHour);
            NTCCFG_TEST_OK(error);

            s.validateRegisteredAndScheduled(4, 4);
        }
        NTCI_LOG_DEBUG("Part 3, create and schedule two t2 timers");
        {
            timerOptions.setId(test::k_TIMER_ID_4);
            bsl::shared_ptr<ntci::Timer> timer1 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            error = timer1->schedule(s.chronology->currentTime() + s.oneHour +
                                     s.oneHour + s.oneHour);
            NTCCFG_TEST_OK(error);

            timerOptions.setId(test::k_TIMER_ID_5);
            bsl::shared_ptr<ntci::Timer> timer2 =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);

            error = timer2->schedule(s.chronology->currentTime() + s.oneHour +
                                     s.oneHour + s.oneHour);
            NTCCFG_TEST_OK(error);

            s.validateRegisteredAndScheduled(6, 6);
        }
        NTCI_LOG_DEBUG("Part 4, advance time to t1, 4 timers should expire");
        {
            s.clock.advance(s.oneHour + s.oneHour);
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_0,
                ntca::TimerEventType::e_DEADLINE);
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_1,
                ntca::TimerEventType::e_DEADLINE);
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_2,
                ntca::TimerEventType::e_DEADLINE);
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_3,
                ntca::TimerEventType::e_DEADLINE);

            s.validateRegisteredAndScheduled(2, 2);
        }
        NTCI_LOG_DEBUG(
            "Part 5, advance time to t2, last 2 timers should expire");
        {
            s.clock.advance(s.oneHour);
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_4,
                ntca::TimerEventType::e_DEADLINE);
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_5,
                ntca::TimerEventType::e_DEADLINE);

            s.validateRegisteredAndScheduled(0, 0);
        }
    }
}

NTCCFG_TEST_CASE(24)
{
    // Concern: Test recurring timers. Create and schedule one timer with deadline t0 and period p0.
    // t0 > p0. Check that till t0 it does not shot. Then check that it shots every period

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        NTCI_LOG_DEBUG("Part 1, create and schedule periodic timer");
        bsl::shared_ptr<ntci::Timer> timer;
        {
            ntca::TimerOptions timerOptions =
                s.createOptionsAllDisabled(test::k_TIMER_ID_5);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

            timer = s.chronology->createTimer(timerOptions,
                                              s.timerCallback,
                                              &s.ta);

            const ntsa::Error error =
                timer->schedule(s.chronology->currentTime() + s.oneHour,
                                s.oneMinute);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();

            s.validateRegisteredAndScheduled(1, 1);
        }
        NTCI_LOG_DEBUG("Part 2, advance till time point earlier than t0, "
                       "check that it was not shot");
        {
            s.clock.advance(s.oneMinute);
            s.callbacks->validateNoEventReceived();
            s.validateRegisteredAndScheduled(1, 1);
        }
        NTCI_LOG_DEBUG(
            "Part 3, advance till deadline, check that timer was shot");
        {
            s.clock.advance(s.oneHour - s.oneMinute);
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_5,
                ntca::TimerEventType::e_DEADLINE);
            s.validateRegisteredAndScheduled(1, 1);

            timer->close();
            timer.reset();
        }
    }
}

NTCCFG_TEST_CASE(25)
{
    // Concern: Test recurring timer when time between consequetive announcements is X times biggger than its period
    // Plan: create recurring timer and schedule it to be due at t0 with period t1
    // advance till t0 and check that it is fired
    // advance till t0 + t1 + t1 + t1 +t1 and check that it is fired
    // advance less than one period and check that one baclocked shot is fired
    // advance more and check that no event fired

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        NTCI_LOG_DEBUG("Part 1, create and schedule periodic timer");
        bsl::shared_ptr<ntci::Timer> timer;
        {
            ntca::TimerOptions timerOptions =
                s.createOptionsAllDisabled(test::k_TIMER_ID_0);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

            timer = s.chronology->createTimer(timerOptions,
                                              s.timerCallback,
                                              &s.ta);
            const ntsa::Error error =
                timer->schedule(s.chronology->currentTime() + s.oneHour,
                                s.oneMinute);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();

            s.validateRegisteredAndScheduled(1, 1);
        }
        NTCI_LOG_DEBUG("Part 2, wait till first deadline");
        {
            s.clock.advance(s.oneHour);
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_0,
                ntca::TimerEventType::e_DEADLINE);
            s.validateRegisteredAndScheduled(1, 1);
        }
        NTCI_LOG_DEBUG("Part 3, wait 4*period times");
        {
            s.clock.advance(s.oneMinute + s.oneMinute + s.oneMinute +
                            s.oneMinute);
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_0,
                ntca::TimerEventType::e_DEADLINE);
            s.validateRegisteredAndScheduled(1, 1);
        }
        NTCI_LOG_DEBUG("Part 4, wait even more and ensure that previous "
                       "intervals were backloged only once");
        {
            s.clock.advance(s.oneSecond);
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_0,
                ntca::TimerEventType::e_DEADLINE);
        }
        NTCI_LOG_DEBUG(
            "Part 5, wait even more and ensure that now nothing is backloged");
        {
            s.clock.advance(s.oneSecond);
            s.chronology->announce();
            s.callbacks->validateNoEventReceived();

            timer->close();
            timer.reset();
        }
    }
}

NTCCFG_TEST_CASE(26)
{
    // Concern: Reccuring timers send closed event on close
    // Plan: create and schedule a timer to be due at t0 with period t1
    // wait till first deadline and check that it shots
    // advance less than a period and check that it does not shot
    // close the timer and check that close event received

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        NTCI_LOG_DEBUG("Part 1, create and schedule reccuring timer");
        bsl::shared_ptr<ntci::Timer> timer;
        {
            ntca::TimerOptions timerOptions =
                s.createOptionsAllDisabled(test::k_TIMER_ID_0);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
            timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);

            timer = s.chronology->createTimer(timerOptions,
                                              s.timerCallback,
                                              &s.ta);
            const ntsa::Error error =
                timer->schedule(s.chronology->currentTime() + s.oneHour,
                                s.oneMinute);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();

            s.validateRegisteredAndScheduled(1, 1);
        }
        NTCI_LOG_DEBUG("Part 2, wait till first deadline");
        {
            s.clock.advance(s.oneHour);
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_0,
                ntca::TimerEventType::e_DEADLINE);
            s.validateRegisteredAndScheduled(1, 1);
        }
        NTCI_LOG_DEBUG("Part 3, advance less than a period and close");
        {
            s.clock.advance(s.oneSecond);
            s.chronology->announce();
            s.callbacks->validateNoEventReceived();
            s.validateRegisteredAndScheduled(1, 1);

            timer->close();
            s.driver->validateInterruptAllCalled();
            s.callbacks->validateNoEventReceived();
            s.validateRegisteredAndScheduled(1, 0);

            s.clock.advance(s.oneMinute);
            s.chronology->announce();
            s.callbacks->validateEventReceived(test::k_TIMER_ID_0,
                                               ntca::TimerEventType::e_CLOSED);

            timer.reset();
        }
    }
}

NTCCFG_TEST_CASE(27)
{
    // Concern: Mix of one-shot, non one-shot non-recurring and recurring timers
    // schedule one shot to be fired at t1. Schedule non one-shot non-recurring
    // to be fired at t2.  Schedule recurring to start now with period less than t1 & t2
    // Plan: create and schedule one-shot timer to be due at t1
    // create and schedule non one-shot non recurring timer to be due at t2
    // create and schedule recurring timer to be due now with small period

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        NTCI_LOG_DEBUG("Part 1, create and schedule one shot timer");
        {
            ntca::TimerOptions timerOptions =
                s.createOptionsAllDisabled(test::k_TIMER_ID_0);
            timerOptions.setOneShot(true);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

            bsl::shared_ptr<ntci::Timer> timer =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);
            const ntsa::Error error =
                timer->schedule(s.chronology->currentTime() + s.oneHour);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();

            s.validateRegisteredAndScheduled(1, 1);
        }
        NTCI_LOG_DEBUG("Part 2, create and schedule non one shot timer non "
                       "recurring timer");
        bsl::shared_ptr<ntci::Timer> timerNonOneShot;
        {
            ntca::TimerOptions timerOptions =
                s.createOptionsAllDisabled(test::k_TIMER_ID_1);
            ;
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

            timerNonOneShot         = s.chronology->createTimer(timerOptions,
                                                        s.timerCallback,
                                                        &s.ta);
            const ntsa::Error error = timerNonOneShot->schedule(
                s.chronology->currentTime() + s.oneHour + s.oneMinute);
            NTCCFG_TEST_OK(error);

            s.validateRegisteredAndScheduled(2, 2);
        }
        NTCI_LOG_DEBUG("Part 3, create and schedule recurring timer");
        {
            ntca::TimerOptions timerOptions =
                s.createOptionsAllDisabled(test::k_TIMER_ID_2);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

            bsl::shared_ptr<ntci::Timer> timer =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);
            //to be fired immediately
            const ntsa::Error error =
                timer->schedule(s.chronology->currentTime(), s.oneMinute);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();

            s.validateRegisteredAndScheduled(3, 3);
        }
        NTCI_LOG_DEBUG(
            "Part 4, advance till t1, only periodic should continiously shot");
        {
            for (int i = 0; i < 59; ++i) {
                s.clock.advance(s.oneMinute);
                s.chronology->announce();
                s.callbacks->validateEventReceived(
                    test::k_TIMER_ID_2,
                    ntca::TimerEventType::e_DEADLINE);
                s.validateRegisteredAndScheduled(3, 3);
            }
        }
        NTCI_LOG_DEBUG(
            "Part 5, advance one more minute: periodic & one-shot would fire");
        {
            s.clock.advance(s.oneMinute);
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_2,
                ntca::TimerEventType::e_DEADLINE);
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_0,
                ntca::TimerEventType::e_DEADLINE);
            s.validateRegisteredAndScheduled(2, 2);
        }
        NTCI_LOG_DEBUG("Part 6, advance one more minute: periodic & non "
                       "one-shot would fire");
        {
            s.clock.advance(s.oneMinute);
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_2,
                ntca::TimerEventType::e_DEADLINE);
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_1,
                ntca::TimerEventType::e_DEADLINE);
            s.validateRegisteredAndScheduled(2, 1);
        }
        NTCI_LOG_DEBUG("Part 7, close all");
        {
            s.chronology->closeAll();
            s.clock.advance(s.oneHour);
            s.callbacks->validateNoEventReceived();
        }
    }
}

NTCCFG_TEST_CASE(28)
{
    // Concern: test execute (functor and functor sequence),
    // numDeferred and hasAnyDeferred

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        int                           callCounter = 0;
        const ntci::Executor::Functor f =
            NTCCFG_BIND(&test::TestSuite::incrementCallback,
                        bsl::ref(callCounter));

        s.chronology->execute(f);
        NTCCFG_TEST_EQ(s.chronology->numDeferred(), 1);
        NTCCFG_TEST_TRUE(s.chronology->hasAnyDeferred());
        NTCCFG_TEST_EQ(callCounter, 0);

        s.driver->validateInterruptAllCalled();

        s.chronology->announce();
        NTCCFG_TEST_EQ(callCounter, 1);
        --callCounter;
        NTCCFG_TEST_EQ(s.chronology->numDeferred(), 0);
        NTCCFG_TEST_FALSE(s.chronology->hasAnyDeferred());

        s.chronology->announce();
        NTCCFG_TEST_EQ(callCounter, 0);
        NTCCFG_TEST_EQ(s.chronology->numDeferred(), 0);
        NTCCFG_TEST_FALSE(s.chronology->hasAnyDeferred());

        s.chronology->execute(f);
        s.driver->validateInterruptAllCalled();
        s.chronology->execute(f);
        s.driver->validateInterruptAllCalled();
        NTCCFG_TEST_EQ(s.chronology->numDeferred(), 2);
        NTCCFG_TEST_TRUE(s.chronology->hasAnyDeferred());
        s.chronology->announce();
        NTCCFG_TEST_EQ(callCounter, 2);
        callCounter = 0;

        ntci::Executor::FunctorSequence seq;
        seq.push_back(f);
        seq.push_back(f);
        seq.push_back(f);
        s.chronology->moveAndExecute(&seq, f);
        s.driver->validateInterruptAllCalled();
        NTCCFG_TEST_EQ(s.chronology->numDeferred(), 4);
        s.chronology->announce();
        NTCCFG_TEST_EQ(callCounter, 4);
    }
}

NTCCFG_TEST_CASE(29)
{
    // Concern: check hasAnyScheduledOrDeferred

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();
        int                           functorCallCtr = 0;
        const ntci::Executor::Functor f =
            NTCCFG_BIND(&test::TestSuite::incrementCallback,
                        bsl::ref(functorCallCtr));

        ntca::TimerOptions timerOptions =
            s.createOptionsAllDisabled(test::k_TIMER_ID_2);
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

        NTCI_LOG_DEBUG("Part 1, check with only a timer");
        bsl::shared_ptr<ntci::Timer> timer;
        {
            timer = s.chronology->createTimer(timerOptions,
                                              s.timerCallback,
                                              &s.ta);

            const ntsa::Error error =
                timer->schedule(s.chronology->currentTime() + s.oneMinute);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();

            s.validateRegisteredAndScheduled(1, 1);
            NTCCFG_TEST_TRUE(s.chronology->hasAnyScheduledOrDeferred());

            timer->cancel();
            s.validateRegisteredAndScheduled(1, 0);
            NTCCFG_TEST_FALSE(s.chronology->hasAnyScheduledOrDeferred());
        }
        NTCI_LOG_DEBUG("Part 2, add deferred functor");
        {
            s.chronology->execute(f);
            s.driver->validateInterruptAllCalled();
            NTCCFG_TEST_EQ(functorCallCtr, 0);
            NTCCFG_TEST_TRUE(s.chronology->hasAnyScheduledOrDeferred());
        }
        NTCI_LOG_DEBUG(
            "Part 3, reschedule the timer and advance time slightly");
        {
            const ntsa::Error error =
                timer->schedule(s.chronology->currentTime() + s.oneMinute);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();
            s.clock.advance(s.oneSecond);
            s.chronology->announce();
            NTCCFG_TEST_EQ(functorCallCtr, 1);
            --functorCallCtr;
            NTCCFG_TEST_TRUE(s.chronology->hasAnyScheduledOrDeferred());
        }
        NTCI_LOG_DEBUG("Part 4, advance even more and trigger the timer");
        {
            s.clock.advance(s.oneMinute);
            s.chronology->announce();
            NTCCFG_TEST_EQ(functorCallCtr, 0);
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_2,
                ntca::TimerEventType::e_DEADLINE);
            NTCCFG_TEST_FALSE(s.chronology->hasAnyScheduledOrDeferred());
            timer.reset();
        }
    }
}

NTCCFG_TEST_CASE(30)
{
    // Concern: test Chronology::load

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        ntca::TimerOptions timerOptions =
            s.createOptionsAllDisabled(test::k_TIMER_ID_0);
        timerOptions.setOneShot(true);
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

        NTCI_LOG_DEBUG("Part 1, create and schedule oneshot timer");
        {
            timerOptions.setId(test::k_TIMER_ID_0);
            bsl::shared_ptr<ntci::Timer> timer =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);
            timer->schedule(s.clock.currentTime() + s.oneSecond);
            s.driver->validateInterruptAllCalled();
            s.validateRegisteredAndScheduled(1, 1);
        }
        NTCI_LOG_DEBUG(
            "Part 2, create and schedule non-oneshot non-recurring timer");
        {
            timerOptions.setOneShot(false);
            timerOptions.setId(test::k_TIMER_ID_1);
            bsl::shared_ptr<ntci::Timer> timer =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);
            timer->schedule(s.clock.currentTime() + s.oneSecond);
            s.validateRegisteredAndScheduled(2, 2);
        }
        NTCI_LOG_DEBUG("Part 3, create periodic timer");
        {
            timerOptions.setId(test::k_TIMER_ID_2);
            bsl::shared_ptr<ntci::Timer> timer =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);
            timer->schedule(s.clock.currentTime() + s.oneMinute, s.oneSecond);
            s.validateRegisteredAndScheduled(3, 3);
        }
        NTCI_LOG_DEBUG(
            "Part 4, create one shot timer which is to be due far later");
        {
            timerOptions.setId(test::k_TIMER_ID_3);
            timerOptions.setOneShot(true);
            bsl::shared_ptr<ntci::Timer> timer =
                s.chronology->createTimer(timerOptions,
                                          s.timerCallback,
                                          &s.ta);
            timer->schedule(s.clock.currentTime() + s.oneHour);
            s.validateRegisteredAndScheduled(4, 4);
        }
        NTCI_LOG_DEBUG("Part 5, check current timer results");
        {
            ntcs::Chronology::TimerVector v(&s.ta);

            s.chronology->load(&v);
            NTCCFG_TEST_EQ(v.size(), 4);

            NTCCFG_TEST_EQ(v[0]->id(), test::k_TIMER_ID_0);
            NTCCFG_TEST_EQ(v[1]->id(), test::k_TIMER_ID_1);
            NTCCFG_TEST_EQ(v[2]->id(), test::k_TIMER_ID_2);
            NTCCFG_TEST_EQ(v[3]->id(), test::k_TIMER_ID_3);
        }
        NTCI_LOG_DEBUG("Part 6, advance and check timer results");
        {
            s.clock.advance(s.oneSecond);
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_0,
                ntca::TimerEventType::e_DEADLINE);
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_1,
                ntca::TimerEventType::e_DEADLINE);
            s.validateRegisteredAndScheduled(2, 2);

            ntcs::Chronology::TimerVector v(&s.ta);
            s.chronology->load(&v);
            NTCCFG_TEST_EQ(v.size(), 2);

            NTCCFG_TEST_EQ(v[0]->id(), test::k_TIMER_ID_2);
            NTCCFG_TEST_EQ(v[1]->id(), test::k_TIMER_ID_3);

            s.chronology->clear();
        }
        NTCI_LOG_DEBUG("Done");
    }
}

NTCCFG_TEST_CASE(31)
{
    // Concern: test timers with TimerSession

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        bsl::shared_ptr<test::TimerSessionMock> ts1 =
            bsl::allocate_shared<test::TimerSessionMock>(&s.ta);
        bsl::shared_ptr<test::TimerSessionMock> ts2 =
            bsl::allocate_shared<test::TimerSessionMock>(&s.ta);
        bsl::shared_ptr<test::TimerSessionMock> ts3 =
            bsl::allocate_shared<test::TimerSessionMock>(&s.ta);

        bsl::shared_ptr<ntci::Timer> timer1;
        NTCI_LOG_DEBUG("Part 1, create and schedule one-shot timer");
        {
            ntca::TimerOptions timerOptions =
                s.createOptionsAllDisabled(test::k_TIMER_ID_0);
            timerOptions.setOneShot(true);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

            timer1 = s.chronology->createTimer(timerOptions, ts1, &s.ta);
            timer1->schedule(s.clock.currentTime() + s.oneMinute);
            s.driver->validateInterruptAllCalled();
        }

        bsl::shared_ptr<ntci::Timer> timer2;
        NTCI_LOG_DEBUG("Part 2, create and schedule non one-shot timer");
        {
            ntca::TimerOptions timerOptions =
                s.createOptionsAllDisabled(test::k_TIMER_ID_1);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
            timerOptions.showEvent(ntca::TimerEventType::e_CANCELED);

            timer2 = s.chronology->createTimer(timerOptions, ts2, &s.ta);
            timer2->schedule(s.clock.currentTime() + s.oneHour);
        }

        bsl::shared_ptr<ntci::Timer> timer3;
        NTCI_LOG_DEBUG("Part 3, create and schedule recurrin timer");
        {
            ntca::TimerOptions timerOptions =
                s.createOptionsAllDisabled(test::k_TIMER_ID_2);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
            timerOptions.showEvent(ntca::TimerEventType::e_CANCELED);
            timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);

            timer3 = s.chronology->createTimer(timerOptions, ts3, &s.ta);
            timer3->schedule(s.clock.currentTime() + s.oneHour, s.oneMinute);
        }

        NTCI_LOG_DEBUG("Part 3, advance time and check the first timer");
        {
            s.clock.advance(s.oneSecond);
            s.chronology->announce();
            ts1->validateNoEventReceived();

            s.clock.advance(s.oneMinute);
            s.chronology->announce();
            ts1->validateEventReceivedAndClear(
                ntca::TimerEventType::e_DEADLINE);

            timer1.reset();
        }

        NTCI_LOG_DEBUG(
            "Part 4, advance time sightly and cancel the second timer");
        {
            s.clock.advance(s.oneSecond);
            s.chronology->announce();
            ts2->validateNoEventReceived();

            timer2->cancel();
            s.driver->validateInterruptAllCalled();
            s.chronology->announce();
            ts2->validateEventReceivedAndClear(
                ntca::TimerEventType::e_CANCELED);

            timer2.reset();
        }

        NTCI_LOG_DEBUG("Part 5, advance time and check the third timer");
        {
            s.clock.advance(s.oneHour);
            s.chronology->announce();
            ts3->validateEventReceivedAndClear(
                ntca::TimerEventType::e_DEADLINE);

            s.clock.advance(s.oneMinute);
            s.chronology->announce();
            ts3->validateEventReceivedAndClear(
                ntca::TimerEventType::e_DEADLINE);

            timer3->close();
            s.driver->validateInterruptAllCalled();
            timer3.reset();
            s.chronology->announce();
            ts3->validateEventReceivedAndClear(
                ntca::TimerEventType::e_CANCELED);
            ts3->validateEventReceivedAndClear(ntca::TimerEventType::e_CLOSED);
        }
    }
}

NTCCFG_TEST_CASE(32)
{
    // Concern: Illustrate behaviour of a non one-shot non recurring timer
    // and a recurring timer in a special case when close event is required
    // but timers are cancelled and then external references are dropped
    // (close() not called directly)

    // Plan: create and schedule non one-shot non recurring timer and a
    // recurring timer which are due in the future
    // advance time up to a point when recurring timer is due
    // cancel both timers and reset pointers -> no closed evend should be
    // received

    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        NTCI_LOG_DEBUG(
            "Part 1, create and schedule non one-shot non recurring timer");
        bsl::shared_ptr<ntci::Timer> timer1;
        {
            ntca::TimerOptions timerOptions =
                s.createOptionsAllDisabled(test::k_TIMER_ID_1);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
            timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);

            timer1 = s.chronology->createTimer(timerOptions,
                                               s.timerCallback,
                                               &s.ta);

            const ntsa::Error error =
                timer1->schedule(s.chronology->currentTime() + s.oneHour);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();

            s.validateRegisteredAndScheduled(1, 1);
        }
        bsl::shared_ptr<ntci::Timer> timer2;
        NTCI_LOG_DEBUG("Part 2, create and schedule recurring timer");
        {
            ntca::TimerOptions timerOptions =
                s.createOptionsAllDisabled(test::k_TIMER_ID_2);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
            timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);

            timer2 = s.chronology->createTimer(timerOptions,
                                               s.timerCallback,
                                               &s.ta);

            const ntsa::Error error =
                timer2->schedule(s.chronology->currentTime() + s.oneMinute,
                                 s.oneSecond);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();

            s.validateRegisteredAndScheduled(2, 2);
        }
        NTCI_LOG_DEBUG(
            "Part 3, advance time till periodic timer starts shooting");
        {
            s.clock.advance(s.oneMinute);
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_2,
                ntca::TimerEventType::e_DEADLINE);
            s.validateRegisteredAndScheduled(2, 2);
        }
        NTCI_LOG_DEBUG("Part 4, close both timers and reset pointers so that "
                       "references are decremented before announcement");
        {
            timer1->cancel();
            timer2->cancel();
            timer1.reset();
            timer2.reset();
        }
        NTCI_LOG_DEBUG(
            "Part 5, announce, no e_CLOSED event should be received");
        {
            s.clock.advance(s.oneSecond);
            s.chronology->announce();
            s.callbacks->validateNoEventReceived();
            s.validateRegisteredAndScheduled(0, 0);
        }
    }
}

NTCCFG_TEST_CASE(33)
{
    test::TestSuite s;
    {
        NTCI_LOG_CONTEXT();

        ntca::TimerOptions timerOptions =
            s.createOptionsAllDisabled(test::k_TIMER_ID_1);

        timerOptions.setOneShot(false);
        timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

        bsl::shared_ptr<ntci::Timer> timer =
            s.chronology->createTimer(timerOptions, s.timerCallback, &s.ta);

        NTCI_LOG_DEBUG("Schedule timer");
        {
            const ntsa::Error error =
                timer->schedule(s.chronology->currentTime() + s.oneHour);
            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();

            s.validateRegisteredAndScheduled(1, 1);
        }

        NTCI_LOG_DEBUG("Advance and check it is not fired");
        {
            s.clock.advance(s.oneSecond);
            s.chronology->announce();
            s.callbacks->validateNoEventReceived();
        }

        NTCI_LOG_DEBUG("Reschedule timer");
        {
            const ntsa::Error error =
                timer->schedule(s.chronology->currentTime() + s.oneSecond,
                                s.oneMinute);

            NTCCFG_TEST_OK(error);
            s.driver->validateInterruptAllCalled();
            s.validateRegisteredAndScheduled(1, 1);
        }

        NTCI_LOG_DEBUG("Advance and check it is fired");
        {
            s.clock.advance(s.oneSecond);
            s.chronology->announce();
            s.callbacks->validateEventReceived(
                test::k_TIMER_ID_1,
                ntca::TimerEventType::e_DEADLINE);
        }
        NTCI_LOG_DEBUG("Validate it fires periodically");
        {
            const int periodsToValidate = 10;
            for (int i = 0; i < periodsToValidate; ++i) {
                s.clock.advance(s.oneMinute);
                s.chronology->announce();
                s.callbacks->validateEventReceived(
                    test::k_TIMER_ID_1,
                    ntca::TimerEventType::e_DEADLINE);
            }
        }

        NTCI_LOG_DEBUG("Stop the timer");
        {
            NTCCFG_TEST_EQ(timer->cancel(),
                           ntsa::Error(ntsa::Error::e_CANCELLED));
            NTCCFG_TEST_OK(timer->close());
        }

        NTCI_LOG_DEBUG("Done");
    }
}

NTCCFG_TEST_CASE(34)
{
    // Concern: basic multithreaded test
    // Plan:  launch several consumer threads
    // launch several producer threads
    // check that all one shot timers were fired

    NTCI_LOG_CONTEXT();

    test::MtTestSuite ts;
    {
#if NTCS_CHRONOLOGY_TEST_MT_HEAVY
        const int numThreads = bslmt::ThreadUtil::hardwareConcurrency() > 0
                                   ? bslmt::ThreadUtil::hardwareConcurrency()
                                   : 8;

        const int numConsumers = numThreads / 2;
        const int numProducers = (numThreads + 1) / 2;
        const int iterations   = numThreads * 100 * 1000;
#else
        const int numConsumers = 8;
        const int numProducers = 8;
        const int iterations   = 1000000;
#endif

        ts.d_numOneShotTimersToConsume = iterations;
        ts.d_numTimersToProduce        = iterations;

        ts.createAndLaunchConsumers(numConsumers);
        ts.createAndLaunchProducers(numProducers);

        ts.waitAll();
    }
}

NTCCFG_TEST_CASE(35)
{
    // Concern: multithreaded test with a mix of one-shot and periodic timers
    // Plan: launch consumer threads
    // create and schedule periodic timers
    // launch producer threads
    // wait for all one-shot timers to fire
    // cancel all periodic timers and wait for all cancel events to be processed

    NTCI_LOG_CONTEXT();

    test::MtTestSuite ts;
    {
#if NTCS_CHRONOLOGY_TEST_MT_HEAVY
        const int numThreads = bslmt::ThreadUtil::hardwareConcurrency() > 0
                                   ? bslmt::ThreadUtil::hardwareConcurrency()
                                   : 8;

        const int numConsumers      = numThreads / 2;
        const int numProducers      = (numThreads + 1) / 2;
        const int iterations        = 1000 * 1000;
        const int numPeriodicTimers = 1000;
#else
        const int numConsumers      = 8;
        const int numProducers      = 8;
        const int iterations        = 1000000;
        const int numPeriodicTimers = 1000;
#endif

        ts.d_numOneShotTimersToConsume = iterations;
        ts.d_numTimersToProduce        = iterations;
        ts.d_numExpectedCloseEvents    = numPeriodicTimers;

        NTCI_LOG_INFO("Starting consumer threads");
        ts.createAndLaunchConsumers(numConsumers);

        NTCI_LOG_INFO("Creating periodic timers");
        for (int i = 0; i < numPeriodicTimers; ++i) {
            ntca::TimerOptions timerOptions;
            timerOptions.setOneShot(false);
            timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
            timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
            timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);

            ntci::TimerCallback callback(
                NTCCFG_BIND(&test::MtTestSuite::processPeriodicTimer,
                            &ts,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2));

            bsl::shared_ptr<ntci::Timer> timer =
                ts.d_driver->chronology().createTimer(timerOptions,
                                                      callback,
                                                      &ts.d_ta);
            const bsls::TimeInterval period =
                bsls::TimeInterval(0, 1000 * 1000 * 40);  //40ms
            timer->schedule(timer->currentTime(), period);
        }

        NTCI_LOG_INFO("Starting producing threads");
        ts.createAndLaunchProducers(numProducers);
        ts.waitProducers();
        ts.waitAllOneShotTimersConsumed();

        NTCI_LOG_INFO("Cancelling periodic timers");
        {
            ntcs::Chronology::TimerVector v(&ts.d_ta);
            ts.d_driver->chronology().load(&v);
            NTCCFG_TEST_EQ(v.size(), numPeriodicTimers);

            for (int i = 0; i < numPeriodicTimers; ++i) {
                v[i]->close();
            }
        }

        NTCI_LOG_INFO("Waiting for consumer threads to finish");
        ts.waitConsumers();
        NTCCFG_TEST_TRUE(ts.d_numPeriodicTimersShot >= numPeriodicTimers);
        ts.d_numPeriodicTimersShot = 0;
    }
}

NTCCFG_TEST_CASE(36)
{
    // Concern: test strand() functionality
    // Plan: create several producers and consumers
    // create several strands
    // launch consumers and producers, ensure that
    // callbacks at the same strand are never exeuted in parallel
    NTCI_LOG_CONTEXT();

    test::MtTestSuite ts;
    {
#if NTCS_CHRONOLOGY_TEST_MT_HEAVY
        const int numThreads = bslmt::ThreadUtil::hardwareConcurrency() > 0
                                   ? bslmt::ThreadUtil::hardwareConcurrency()
                                   : 8;

        const int numConsumers = numThreads / 2;
        const int numProducers = (numThreads + 1) / 2;
        const int iterations   = 1000 * 1000;
#else
        const int numConsumers = 8;
        const int numProducers = 8;
        const int iterations   = 1000000;
#endif

        ts.d_numOneShotTimersToConsume = iterations;
        ts.d_numTimersToProduce        = iterations;

        ts.createStrands(3);

        ts.createAndLaunchConsumers(numConsumers);

        ts.createAndLaunchProducersWithStrands(numProducers);

        ts.waitAll();
    }
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
    NTCCFG_TEST_REGISTER(9);
    NTCCFG_TEST_REGISTER(10);
    NTCCFG_TEST_REGISTER(11);
    NTCCFG_TEST_REGISTER(12);
    NTCCFG_TEST_REGISTER(13);
    NTCCFG_TEST_REGISTER(14);
    NTCCFG_TEST_REGISTER(15);
    NTCCFG_TEST_REGISTER(16);
    NTCCFG_TEST_REGISTER(17);
    NTCCFG_TEST_REGISTER(18);
    NTCCFG_TEST_REGISTER(19);
    NTCCFG_TEST_REGISTER(20);
    NTCCFG_TEST_REGISTER(21);
    NTCCFG_TEST_REGISTER(22);
    NTCCFG_TEST_REGISTER(23);
    NTCCFG_TEST_REGISTER(24);
    NTCCFG_TEST_REGISTER(25);
    NTCCFG_TEST_REGISTER(26);
    NTCCFG_TEST_REGISTER(27);
    NTCCFG_TEST_REGISTER(28);
    NTCCFG_TEST_REGISTER(29);
    NTCCFG_TEST_REGISTER(30);
    NTCCFG_TEST_REGISTER(31);
    NTCCFG_TEST_REGISTER(32);
    NTCCFG_TEST_REGISTER(33);

    //multithreaded tests below
    NTCCFG_TEST_REGISTER(34);
    NTCCFG_TEST_REGISTER(35);
    NTCCFG_TEST_REGISTER(36);
}
NTCCFG_TEST_DRIVER_END;
