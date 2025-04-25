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
BSLS_IDENT_RCSID(ntcs_strand_t_cpp, "$Id$ $CSID$")

#include <ntcs_strand.h>

#include <ntci_executor.h>
#include <ntci_log.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::Strand'.
class StrandTest
{
    /// Provide an interface to execute a function.
    class Executor;

    /// This class keeps track of the count of an action performed by a thread.
    class Count;

    static void processFunction(bsl::size_t        sequenceNumber,
                                bsls::AtomicUint*  dequeueSequenceNumber,
                                StrandTest::Count* dequeueCount,
                                bsl::size_t        maxSequenceNumber,
                                bslmt::Semaphore*  semaphore);

    static void dequeueFunction(
        const bsl::shared_ptr<StrandTest::Executor>& executor,
        bsl::size_t                                  threadIndex,
        bslmt::Barrier*                              dequeueBarrier,
        StrandTest::Count*                           dequeueCount);

    static void enqueueFunction(
        const bsl::shared_ptr<StrandTest::Executor>& executor,
        const bsl::shared_ptr<ntci::Strand>&         strand,
        bsl::size_t                                  threadIndex,
        bslmt::Barrier*                              enqueueBarrier,
        ntccfg::Mutex*                               enqueueMutex,
        bsl::size_t*                                 enqueueSequenceNumber,
        StrandTest::Count*                           enqueueCount,
        bsls::AtomicUint*                            dequeueSequenceNumber,
        StrandTest::Count*                           dequeueCount,
        bsl::size_t                                  maxSequenceNumber,
        bslmt::Semaphore*                            semaphore,
        bslma::Allocator*                            allocator);

    static void executeCaseOneThreadGroups(
        bsl::size_t       numEnqueueThreads,
        bsl::size_t       numDequeueThreads,
        bsl::size_t       maxSequenceNumber,
        bslma::Allocator* basicAllocator = 0);

    static void verifyCurrentStrandOne(
        ntccfg::Object*                      strandOneObject,
        const bsl::shared_ptr<ntcs::Strand>& strandOne,
        bsls::AtomicInt*                     strandOneJobsComplete,
        bsls::AtomicInt*                     strandOneJobsRemaining,
        bsl::size_t                          strandOneJobsTotal,
        bslmt::Semaphore*                    strandOneSemaphore,
        ntccfg::Object*                      strandTwoObject,
        const bsl::shared_ptr<ntcs::Strand>& strandTwo,
        bsls::AtomicInt*                     strandTwoJobsComplete,
        bsls::AtomicInt*                     strandTwoJobsRemaining,
        bsl::size_t                          strandTwoJobsTotal,
        bslmt::Semaphore*                    strandTwoSemaphore);

    static void verifyCurrentStrandTwo(
        ntccfg::Object*                      strandOneObject,
        const bsl::shared_ptr<ntcs::Strand>& strandOne,
        bsls::AtomicInt*                     strandOneJobsComplete,
        bsls::AtomicInt*                     strandOneJobsRemaining,
        bsl::size_t                          strandOneJobsTotal,
        bslmt::Semaphore*                    strandOneSemaphore,
        ntccfg::Object*                      strandTwoObject,
        const bsl::shared_ptr<ntcs::Strand>& strandTwo,
        bsls::AtomicInt*                     strandTwoJobsComplete,
        bsls::AtomicInt*                     strandTwoJobsRemaining,
        bsl::size_t                          strandTwoJobsTotal,
        bslmt::Semaphore*                    strandTwoSemaphore);

    static void executeCaseTwoThread(
        const bsl::shared_ptr<StrandTest::Executor>& executor,
        bsl::size_t                                  threadIndex);

  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();
};

/// Provide an interface to execute a function.
class StrandTest::Executor : public ntci::Executor
{
    ntccfg::ConditionMutex d_functorQueueMutex;
    ntccfg::Condition      d_functorQueueCondition;
    FunctorSequence        d_functorQueue;
    bool                   d_stop;
    bslma::Allocator*      d_allocator_p;

  private:
    Executor(const Executor&) BSLS_KEYWORD_DELETED;
    Executor& operator=(const Executor&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new executor. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Executor(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Executor() BSLS_KEYWORD_OVERRIDE;

    /// Block until a job is available then execute it.
    void run();

    /// Unblock all threads running this object.
    void stop();

    /// Defer the execution of the specified 'functor'.
    void execute(const Functor& functor) BSLS_KEYWORD_OVERRIDE;

    /// Atomically defer the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE;
};

StrandTest::Executor::Executor(bslma::Allocator* basicAllocator)
: d_functorQueueMutex()
, d_functorQueueCondition()
, d_functorQueue(basicAllocator)
, d_stop(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

StrandTest::Executor::~Executor()
{
}

void StrandTest::Executor::run()
{
    ntccfg::ConditionMutexGuard guard(&d_functorQueueMutex);

    while (!d_stop) {
        if (d_functorQueue.empty()) {
            d_functorQueueCondition.wait(&d_functorQueueMutex);
        }
        else {
            FunctorSequence functorQueue(d_allocator_p);
            functorQueue.swap(d_functorQueue);

            {
                ntccfg::ConditionMutexUnLockGuard unlockGuard(
                    &d_functorQueueMutex);

                for (FunctorSequence::iterator it = functorQueue.begin();
                     it != functorQueue.end();
                     ++it)
                {
                    (*it)();
                }
            }
        }
    }
}

void StrandTest::Executor::stop()
{
    ntccfg::ConditionMutexGuard guard(&d_functorQueueMutex);
    d_stop = true;
    d_functorQueueCondition.broadcast();
}

void StrandTest::Executor::execute(const Functor& functor)
{
    ntccfg::ConditionMutexGuard guard(&d_functorQueueMutex);
    d_functorQueue.push_back(functor);

    d_functorQueueCondition.signal();
}

void StrandTest::Executor::moveAndExecute(FunctorSequence* functorSequence,
                                          const Functor&   functor)
{
    ntccfg::ConditionMutexGuard guard(&d_functorQueueMutex);

    d_functorQueue.splice(d_functorQueue.end(), *functorSequence);
    if (functor) {
        d_functorQueue.push_back(functor);
    }

    d_functorQueueCondition.signal();
}

/// This class keeps track of the count of an action performed by a thread.
/// This class is thread safe.
class StrandTest::Count
{
    typedef bsl::unordered_map<bsl::uint64_t, bsl::size_t> CountMap;
    typedef bsl::unordered_map<bsl::uint64_t, bsl::size_t> IndexMap;

    bsls::SpinLock d_lock;
    CountMap       d_countMap;
    IndexMap       d_indexMap;
    bsl::string    d_name;
    bsl::size_t    d_maxThreads;
    bsl::size_t    d_maxCount;

  private:
    Count(const Count&);
    Count& operator=(const Count&);

  public:
    /// Create a new thread action counter having the specified 'name'
    /// expecting the specified 'maxCount' possible for each thread.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit Count(const bsl::string& name,
                   bsl::size_t        maxThreads,
                   bsl::size_t        maxCount,
                   bslma::Allocator*  basicAllocator = 0);

    /// Destroy this object.
    ~Count();

    /// Set the thread index associated with the specified 'threadHandle'
    /// to the specified 'threadIndex'.
    void setThreadIndex(bslmt::ThreadUtil::Handle threadHandle,
                        bsl::size_t               threadIndex);

    /// Record the specified 'threadHandle' performing an action.
    void record(bslmt::ThreadUtil::Handle threadHandle);

    /// Print the summary of each count to the specified 'stream'.
    void print(bsl::ostream& stream);

    /// Return the name of the object.
    const bsl::string& name() const;
};

StrandTest::Count::Count(const bsl::string& name,
                         bsl::size_t        maxThreads,
                         bsl::size_t        maxCount,
                         bslma::Allocator*  basicAllocator)
: d_lock(bsls::SpinLock::s_unlocked)
, d_countMap(basicAllocator)
, d_indexMap(basicAllocator)
, d_name(name, basicAllocator)
, d_maxThreads(maxThreads)
, d_maxCount(maxCount)
{
}

StrandTest::Count::~Count()
{
}

void StrandTest::Count::setThreadIndex(bslmt::ThreadUtil::Handle threadHandle,
                                       bsl::size_t               threadIndex)
{
    BSLS_ASSERT(threadIndex < d_maxThreads);

    bsl::uint64_t key = bslmt::ThreadUtil::idAsUint64(
        bslmt::ThreadUtil::handleToId(threadHandle));

    bsls::SpinLockGuard guard(&d_lock);
    d_indexMap[key] = threadIndex;
}

void StrandTest::Count::record(bslmt::ThreadUtil::Handle threadHandle)
{
    bsl::uint64_t key = bslmt::ThreadUtil::idAsUint64(
        bslmt::ThreadUtil::handleToId(threadHandle));

    bsls::SpinLockGuard guard(&d_lock);
    ++d_countMap[key];
}

void StrandTest::Count::print(bsl::ostream& stream)
{
    stream << "\n" << d_name << ":\n";

    bsl::size_t maxBarLength = 50;

    bsl::size_t total = 0;

    bsl::vector<bsl::size_t> countVector(d_maxThreads);

    for (CountMap::const_iterator it = d_countMap.begin();
         it != d_countMap.end();
         ++it)
    {
        bsl::uint64_t key   = it->first;
        bsl::size_t   count = it->second;

        total += count;

        IndexMap::const_iterator jt = d_indexMap.find(key);

        if (jt != d_indexMap.end()) {
            bsl::size_t threadIndex = jt->second;
            BSLS_ASSERT(threadIndex < d_maxThreads);

            countVector[threadIndex] = count;
        }
    }

    for (bsl::size_t threadIndex = 0; threadIndex < countVector.size();
         ++threadIndex)
    {
        bsl::size_t count = countVector[threadIndex];

        stream << "    " << bsl::setw(8) << bsl::left << threadIndex
               << bsl::setw(11) << bsl::right << count << "  " << bsl::left;

        double percentage =
            static_cast<double>(count) / static_cast<double>(d_maxCount);

        bsl::size_t numBarCharacters =
            static_cast<bsl::size_t>(percentage * maxBarLength);

        for (bsl::size_t i = 0; i < numBarCharacters; ++i) {
            stream << '#';
        }

        stream << "\n";
    }

    stream << "    " << bsl::setw(8) << bsl::left << "Total" << bsl::setw(11)
           << bsl::right << total << "\n";
}

const bsl::string& StrandTest::Count::name() const
{
    return d_name;
}

void StrandTest::processFunction(bsl::size_t        sequenceNumber,
                                 bsls::AtomicUint*  dequeueSequenceNumber,
                                 StrandTest::Count* dequeueCount,
                                 bsl::size_t        maxSequenceNumber,
                                 bslmt::Semaphore*  semaphore)
{
    NTCI_LOG_CONTEXT();

    // Verify the functions enqueued on the strand are processed
    // non-concurrently, in the same order.

    bsl::size_t expectedSequenceNumber = (*dequeueSequenceNumber)++;

    NTCI_LOG_DEBUG("[-] %d", (int)(sequenceNumber));

    NTSCFG_TEST_EQ(sequenceNumber, expectedSequenceNumber);

    dequeueCount->record(bslmt::ThreadUtil::self());

    // bslmt::ThreadUtil::sleep(bsls::TimeInterval(0.1));

    // Stop when all sequence numbers have been enqueued.

    if (sequenceNumber == maxSequenceNumber - 1) {
        semaphore->post();
    }
}

void StrandTest::dequeueFunction(
    const bsl::shared_ptr<StrandTest::Executor>& executor,
    bsl::size_t                                  threadIndex,
    bslmt::Barrier*                              dequeueBarrier,
    StrandTest::Count*                           dequeueCount)
{
    int rc;

    bsl::string threadName;
    {
        bsl::stringstream ss;
        ss << "dequeue-" << threadIndex;
        threadName = ss.str();
    }

    bslmt::ThreadUtil::setThreadName(threadName);

    dequeueCount->setThreadIndex(bslmt::ThreadUtil::self(), threadIndex);

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("dequeue");
    NTCI_LOG_CONTEXT_GUARD_THREAD(threadIndex);

    // Wait until all dequeue threads have reached the rendezvous point.

    dequeueBarrier->wait();

    // Process deferred functions.

    executor->run();
}

void StrandTest::enqueueFunction(
    const bsl::shared_ptr<StrandTest::Executor>& executor,
    const bsl::shared_ptr<ntci::Strand>&         strand,
    bsl::size_t                                  threadIndex,
    bslmt::Barrier*                              enqueueBarrier,
    ntccfg::Mutex*                               enqueueMutex,
    bsl::size_t*                                 enqueueSequenceNumber,
    StrandTest::Count*                           enqueueCount,
    bsls::AtomicUint*                            dequeueSequenceNumber,
    StrandTest::Count*                           dequeueCount,
    bsl::size_t                                  maxSequenceNumber,
    bslmt::Semaphore*                            semaphore,
    bslma::Allocator*                            allocator)
{
    bsl::string threadName;
    {
        bsl::stringstream ss;
        ss << "enqueue-" << threadIndex;
        threadName = ss.str();
    }

    bslmt::ThreadUtil::setThreadName(threadName);

    enqueueCount->setThreadIndex(bslmt::ThreadUtil::self(), threadIndex);

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("enqueue");
    NTCI_LOG_CONTEXT_GUARD_THREAD(threadIndex);

    // Wait until all enqueue threads have reached the rendezvous point.

    enqueueBarrier->wait();

    // While there remains sequence numbers to enqueue...

    bsl::size_t iteration = 0;

    while (true) {
        // Attempt to allow some other threads to grab the lock.

        if (iteration % 100 == 0) {
            // bslmt::ThreadUtil::yield();
        }

        // Lock function submission to that functions are properly sequenced
        // between enqueueing threads.

        ntccfg::LockGuard guard(enqueueMutex);

        // Stop when all sequence numbers have been enqueued.

        if (*enqueueSequenceNumber == maxSequenceNumber) {
            break;
        }

        // Defer a function to process this sequence number on the strand.

        bsl::size_t sequenceNumber = (*enqueueSequenceNumber)++;

        NTCI_LOG_DEBUG("[+] %d", (int)(sequenceNumber));

        enqueueCount->record(bslmt::ThreadUtil::self());

        strand->execute(bdlf::BindUtil::bindS(allocator,
                                              &StrandTest::processFunction,
                                              sequenceNumber,
                                              dequeueSequenceNumber,
                                              dequeueCount,
                                              maxSequenceNumber,
                                              semaphore));

        ++iteration;
    }
}

void StrandTest::executeCaseOneThreadGroups(bsl::size_t numEnqueueThreads,
                                            bsl::size_t numDequeueThreads,
                                            bsl::size_t maxSequenceNumber,
                                            bslma::Allocator* basicAllocator)
{
    NTCI_LOG_CONTEXT();

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    // Create the executor.

    bsl::shared_ptr<StrandTest::Executor> executor;
    executor.createInplace(allocator, allocator);

    bsl::size_t      enqueueSequenceNumber = 0;
    bsls::AtomicUint dequeueSequenceNumber(0);

    bslmt::ThreadGroup enqueueThreadGroup(allocator);
    bslmt::ThreadGroup dequeueThreadGroup(allocator);

    bslmt::Semaphore semaphore;

    bslmt::Barrier enqueueBarrier(numEnqueueThreads);
    bslmt::Barrier dequeueBarrier(numDequeueThreads + 1);

    ntccfg::Mutex enqueueMutex;

    StrandTest::Count enqueueCount("Enqueue",
                                   numEnqueueThreads,
                                   maxSequenceNumber,
                                   allocator);
    StrandTest::Count dequeueCount("Dequeue",
                                   numDequeueThreads,
                                   maxSequenceNumber,
                                   allocator);

    bsls::Stopwatch stopwatch;
    stopwatch.start(true);

    double wallTime0, cpuTimeUser0, cpuTimeSystem0;
    stopwatch.accumulatedTimes(&cpuTimeSystem0, &cpuTimeUser0, &wallTime0);

    // Add dequeue threads to run the executor.

    for (bsl::size_t dequeueThreadIndex = 0;
         dequeueThreadIndex < numDequeueThreads;
         ++dequeueThreadIndex)
    {
        dequeueThreadGroup.addThread(
            bdlf::BindUtil::bindS(allocator,
                                  &StrandTest::dequeueFunction,
                                  executor,
                                  dequeueThreadIndex,
                                  &dequeueBarrier,
                                  &dequeueCount));
    }

    // Wait until all dequeue threads are running the executor.

    dequeueBarrier.wait();

    // Create a strand onto which to enqueue the execution of
    // functions.

    bsl::shared_ptr<ntcs::Strand> strand;
    strand.createInplace(allocator, executor, allocator);

    // Add enqueue threads to defer the execution of functions by the threads
    // running the excecutor.

    for (bsl::size_t enqueueThreadIndex = 0;
         enqueueThreadIndex < numEnqueueThreads;
         ++enqueueThreadIndex)
    {
        enqueueThreadGroup.addThread(
            bdlf::BindUtil::bindS(allocator,
                                  &StrandTest::enqueueFunction,
                                  executor,
                                  strand,
                                  enqueueThreadIndex,
                                  &enqueueBarrier,
                                  &enqueueMutex,
                                  &enqueueSequenceNumber,
                                  &enqueueCount,
                                  &dequeueSequenceNumber,
                                  &dequeueCount,
                                  maxSequenceNumber,
                                  &semaphore,
                                  allocator));
    }

    // Wait until all sequences numbers have been enqueued and
    // dequeued.

    semaphore.wait();

    BSLS_ASSERT(enqueueSequenceNumber == maxSequenceNumber);
    BSLS_ASSERT(dequeueSequenceNumber == maxSequenceNumber);

    double wallTime1, cpuTimeUser1, cpuTimeSystem1;
    stopwatch.accumulatedTimes(&cpuTimeSystem1, &cpuTimeUser1, &wallTime1);

    stopwatch.stop();

    // Stop the executor.

    executor->stop();

    // Join all threads.

    enqueueThreadGroup.joinAll();
    dequeueThreadGroup.joinAll();

    // Print a graph of which thread did what.

    {
        bsl::stringstream ss;
        enqueueCount.print(ss);
        dequeueCount.print(ss);

        BSLS_LOG_TRACE("%s", ss.str().c_str());
    }

    // Print wall time and CPU time.

    {
        bsl::stringstream ss;

        double wallTime      = wallTime1 - wallTime0;
        double cpuTimeUser   = cpuTimeUser1 - cpuTimeUser0;
        double cpuTimeSystem = cpuTimeSystem1 - cpuTimeSystem0;

        double throughput = static_cast<double>(maxSequenceNumber) / wallTime;

        ss << "\nTime:\n"
           << "Wall:       " << bsl::fixed << bsl::setprecision(8) << wallTime
           << "\n"
           << "User:       " << bsl::fixed << bsl::setprecision(8)
           << cpuTimeUser << "\n"
           << "System:     " << bsl::fixed << bsl::setprecision(8)
           << cpuTimeSystem << "\n\n"
           << "Throughput: " << bsl::fixed << bsl::setprecision(2)
           << throughput << " functors/sec\n";

        BSLS_LOG_DEBUG("%s", ss.str().c_str());
    }
}

void StrandTest::verifyCurrentStrandOne(
    ntccfg::Object*                      strandOneObject,
    const bsl::shared_ptr<ntcs::Strand>& strandOne,
    bsls::AtomicInt*                     strandOneJobsComplete,
    bsls::AtomicInt*                     strandOneJobsRemaining,
    bsl::size_t                          strandOneJobsTotal,
    bslmt::Semaphore*                    strandOneSemaphore,
    ntccfg::Object*                      strandTwoObject,
    const bsl::shared_ptr<ntcs::Strand>& strandTwo,
    bsls::AtomicInt*                     strandTwoJobsComplete,
    bsls::AtomicInt*                     strandTwoJobsRemaining,
    bsl::size_t                          strandTwoJobsTotal,
    bslmt::Semaphore*                    strandTwoSemaphore)
{
    NTCCFG_OBJECT_GUARD(strandOneObject);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Strand one job %d/%d complete",
                  strandOneJobsComplete->load() + 1,
                  strandOneJobsTotal);

    // Ensure strand one is active in this thread, but strand two is not.

    ntci::Strand* current = ntci::Strand::getThreadLocal();
    NTSCFG_TEST_EQ(current, strandOne.get());

    bool strandOneIsActive = strandOne->isRunningInCurrentThread();
    bool strandTwoIsActive = strandTwo->isRunningInCurrentThread();

    NTSCFG_TEST_TRUE(strandOneIsActive);
    NTSCFG_TEST_FALSE(strandTwoIsActive);

    // Note that a job on strand one is complete.

    int previousNumJobsComplete = strandOneJobsComplete->add(1);

    // Kick off another job on strand one, if there are any left remaining.

    if (strandOneJobsRemaining->subtract(1) >= 0) {
        strandOne->execute(
            bdlf::BindUtil::bind(&StrandTest::verifyCurrentStrandOne,
                                 strandOneObject,
                                 strandOne,
                                 strandOneJobsComplete,
                                 strandOneJobsRemaining,
                                 strandOneJobsTotal,
                                 strandOneSemaphore,
                                 strandTwoObject,
                                 strandTwo,
                                 strandTwoJobsComplete,
                                 strandTwoJobsRemaining,
                                 strandTwoJobsTotal,
                                 strandTwoSemaphore));
    }

    // Kick off another job on strand two, if there are any left remaining.

    if (strandTwoJobsRemaining->subtract(1) >= 0) {
        strandTwo->execute(
            bdlf::BindUtil::bind(&StrandTest::verifyCurrentStrandTwo,
                                 strandOneObject,
                                 strandOne,
                                 strandOneJobsComplete,
                                 strandOneJobsRemaining,
                                 strandOneJobsTotal,
                                 strandOneSemaphore,
                                 strandTwoObject,
                                 strandTwo,
                                 strandTwoJobsComplete,
                                 strandTwoJobsRemaining,
                                 strandTwoJobsTotal,
                                 strandTwoSemaphore));
    }

    if (previousNumJobsComplete == strandOneJobsTotal) {
        strandOneSemaphore->post();
    }
}

void StrandTest::verifyCurrentStrandTwo(
    ntccfg::Object*                      strandOneObject,
    const bsl::shared_ptr<ntcs::Strand>& strandOne,
    bsls::AtomicInt*                     strandOneJobsComplete,
    bsls::AtomicInt*                     strandOneJobsRemaining,
    bsl::size_t                          strandOneJobsTotal,
    bslmt::Semaphore*                    strandOneSemaphore,
    ntccfg::Object*                      strandTwoObject,
    const bsl::shared_ptr<ntcs::Strand>& strandTwo,
    bsls::AtomicInt*                     strandTwoJobsComplete,
    bsls::AtomicInt*                     strandTwoJobsRemaining,
    bsl::size_t                          strandTwoJobsTotal,
    bslmt::Semaphore*                    strandTwoSemaphore)
{
    NTCCFG_OBJECT_GUARD(strandTwoObject);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Strand two job %d/%d complete",
                  strandTwoJobsComplete->load() + 1,
                  strandTwoJobsTotal);

    // Ensure strand two is active in this thread, but strand one is not.

    ntci::Strand* current = ntci::Strand::getThreadLocal();
    NTSCFG_TEST_EQ(current, strandTwo.get());

    bool strandOneIsActive = strandOne->isRunningInCurrentThread();
    bool strandTwoIsActive = strandTwo->isRunningInCurrentThread();

    NTSCFG_TEST_FALSE(strandOneIsActive);
    NTSCFG_TEST_TRUE(strandTwoIsActive);

    // Note that a job on strand two is complete.

    int previousNumJobsComplete = strandTwoJobsComplete->add(1);

    // Kick off another job on strand one, if there are any left remaining.

    if (strandOneJobsRemaining->subtract(1) >= 0) {
        strandOne->execute(
            bdlf::BindUtil::bind(&StrandTest::verifyCurrentStrandOne,
                                 strandOneObject,
                                 strandOne,
                                 strandOneJobsComplete,
                                 strandOneJobsRemaining,
                                 strandOneJobsTotal,
                                 strandOneSemaphore,
                                 strandTwoObject,
                                 strandTwo,
                                 strandTwoJobsComplete,
                                 strandTwoJobsRemaining,
                                 strandTwoJobsTotal,
                                 strandTwoSemaphore));
    }

    // Kick off another job on strand two, if there are any left remaining.

    if (strandTwoJobsRemaining->subtract(1) >= 0) {
        strandTwo->execute(
            bdlf::BindUtil::bind(&StrandTest::verifyCurrentStrandTwo,
                                 strandOneObject,
                                 strandOne,
                                 strandOneJobsComplete,
                                 strandOneJobsRemaining,
                                 strandOneJobsTotal,
                                 strandOneSemaphore,
                                 strandTwoObject,
                                 strandTwo,
                                 strandTwoJobsComplete,
                                 strandTwoJobsRemaining,
                                 strandTwoJobsTotal,
                                 strandTwoSemaphore));
    }

    if (previousNumJobsComplete == strandTwoJobsTotal) {
        strandTwoSemaphore->post();
    }
}

void StrandTest::executeCaseTwoThread(
    const bsl::shared_ptr<StrandTest::Executor>& executor,
    bsl::size_t                                  threadIndex)
{
    NTCI_LOG_CONTEXT();

    int rc;

    bsl::string threadName;
    {
        bsl::stringstream ss;
        ss << "executor-" << threadIndex;
        threadName = ss.str();
    }

    bslmt::ThreadUtil::setThreadName(threadName);

    NTCI_LOG_CONTEXT_GUARD_OWNER("executor");
    NTCI_LOG_CONTEXT_GUARD_THREAD(threadIndex);

    executor->run();
}

NTSCFG_TEST_FUNCTION(ntcs::StrandTest::verifyCase1)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    const bsl::size_t MAX_SEQUENCE_NUMBER = 1000000;

#else

    const bsl::size_t MAX_SEQUENCE_NUMBER = 1000;

#endif

    // clang format off
    struct Data {
        bsl::size_t d_numEnqueueThreads;
        bsl::size_t d_numDequeueThreads;
    } DATA[] = {
        { 1,  1},
        { 1, 10},
        {10,  1},
        {10, 10}
    };

    /// clang format on
    enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

    bdlma::ConcurrentMultipoolAllocator d_memoryPools(8);

    for (bsl::size_t iteration = 0; iteration < NUM_DATA; ++iteration) {
        BSLS_LOG_DEBUG("Testing %d enqueue threads, %d dequeue threads",
                      (int)(DATA[iteration].d_numEnqueueThreads),
                      (int)(DATA[iteration].d_numDequeueThreads));

        StrandTest::executeCaseOneThreadGroups(
            DATA[iteration].d_numEnqueueThreads,
            DATA[iteration].d_numDequeueThreads,
            MAX_SEQUENCE_NUMBER,
            &d_memoryPools);
    }
}

NTSCFG_TEST_FUNCTION(ntcs::StrandTest::verifyCase2)
{
    const bsl::size_t NUM_THREADS = 4;
    const bsl::size_t NUM_JOBS    = 1000;

    ntsa::Error error;
    int         rc;

    // Ensure there is no current strand active in this thread.

    {
        ntci::Strand* current = ntci::Strand::getThreadLocal();
        NTSCFG_TEST_EQ(current, 0);
    }

    // Create the executor.

    bsl::shared_ptr<StrandTest::Executor> executor;
    executor.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Create two strands.

    ntccfg::Object strandOneObject("one");
    ntccfg::Object strandTwoObject("two");

    bsl::shared_ptr<ntcs::Strand> strandOne;
    strandOne.createInplace(NTSCFG_TEST_ALLOCATOR,
                            executor,
                            NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntcs::Strand> strandTwo;
    strandTwo.createInplace(NTSCFG_TEST_ALLOCATOR,
                            executor,
                            NTSCFG_TEST_ALLOCATOR);

    const bsl::size_t strandOneJobsTotal = NUM_JOBS;
    const bsl::size_t strandTwoJobsTotal = NUM_JOBS;

    bsls::AtomicInt strandOneJobsComplete(0);
    bsls::AtomicInt strandOneJobsRemaining(strandOneJobsTotal);

    bsls::AtomicInt strandTwoJobsComplete(0);
    bsls::AtomicInt strandTwoJobsRemaining(strandTwoJobsTotal);

    bslmt::Semaphore strandOneSemaphore;
    bslmt::Semaphore strandTwoSemaphore;

    // Create a set of threads that will drive the strands.

    bslmt::ThreadGroup threadGroup(NTSCFG_TEST_ALLOCATOR);

    for (bsl::size_t threadIndex = 0; threadIndex < NUM_THREADS; ++threadIndex)
    {
        rc = threadGroup.addThread(
            bdlf::BindUtil::bind(&StrandTest::executeCaseTwoThread,
                                 executor,
                                 threadIndex));
        NTSCFG_TEST_EQ(rc, 0);
    }

    // Kick off a job on strand one. The job will invoke other jobs for
    // strand one and strand two as necessary.

    if (strandOneJobsRemaining.subtract(1) > 0) {
        strandOne->execute(
            bdlf::BindUtil::bind(&StrandTest::verifyCurrentStrandOne,
                                 &strandOneObject,
                                 strandOne,
                                 &strandOneJobsComplete,
                                 &strandOneJobsRemaining,
                                 strandOneJobsTotal,
                                 &strandOneSemaphore,
                                 &strandTwoObject,
                                 strandTwo,
                                 &strandTwoJobsComplete,
                                 &strandTwoJobsRemaining,
                                 strandTwoJobsTotal,
                                 &strandTwoSemaphore));
    }

    // Kick off a job on strand two. The job will invoke other jobs for
    // strand one and strand two as necessary.

    if (strandTwoJobsRemaining.subtract(1) > 0) {
        strandTwo->execute(
            bdlf::BindUtil::bind(&StrandTest::verifyCurrentStrandTwo,
                                 &strandOneObject,
                                 strandOne,
                                 &strandOneJobsComplete,
                                 &strandOneJobsRemaining,
                                 strandOneJobsTotal,
                                 &strandOneSemaphore,
                                 &strandTwoObject,
                                 strandTwo,
                                 &strandTwoJobsComplete,
                                 &strandTwoJobsRemaining,
                                 strandTwoJobsTotal,
                                 &strandTwoSemaphore));
    }

    // Wait for all jobs from both strands to complete.

    strandOneSemaphore.wait();
    strandTwoSemaphore.wait();

    // Stop the executor.

    executor->stop();

    // Join all threads.

    threadGroup.joinAll();
}

}  // close namespace ntcs
}  // close namespace BloombergLP
