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
BSLS_IDENT_RCSID(ntccfg_mutex_t_cpp, "$Id$ $CSID$")

#include <ntccfg_mutex.h>

#include <ntccfg_bind.h>
#include <ntccfg_function.h>

#include <bslmt_barrier.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntccfg {

// Provide tests for 'ntccfg::Mutex'.
class MutexTest
{
    static void thread(bslmt::Barrier*   barrier,
                       bslmt::Semaphore* callee,
                       bslmt::Semaphore* caller, 
                       ntccfg::Mutex*    mutex, 
                       int*              value,
                       int               index);

    static void execute(bsl::size_t numThreads);

  public:
    // TODO
    static void verify1();

    // TODO
    static void verify2();

    // TODO
    static void verify3();

    // TODO
    static void verify4();
};

void ntccfg::MutexTest::thread(bslmt::Barrier*   barrier,
                               bslmt::Semaphore* callee,
                               bslmt::Semaphore* caller, 
                               ntccfg::Mutex*    mutex, 
                               int*              value,
                               int               index)
{
    BSLS_LOG_TRACE("Thread %d: started", index);
    
    barrier->wait();

    callee->wait();
    BSLS_LOG_TRACE("Thread %d: locking", index);
    mutex->lock();
    *value = index;
    BSLS_LOG_TRACE("Thread %d: locked", index);
    caller->post();

    // bslmt::ThreadUtil::sleep(bsls::TimeInterval(3));

    callee->wait();
    BSLS_LOG_TRACE("Thread %d: unlocking", index);
    *value = -1;
    mutex->unlock();
    BSLS_LOG_TRACE("Thread %d: unlocked", index);
    caller->post();

    BSLS_LOG_TRACE("Thread %d: complete", index);
}

void ntccfg::MutexTest::execute(bsl::size_t numThreads)
{
    NTSCFG_TEST_GE(numThreads, 2);

    ntccfg::Mutex mutex;
    int           value = -1;

    bslmt::ThreadGroup threadGroup(NTSCFG_TEST_ALLOCATOR);

    bslmt::Barrier startup(static_cast<int>(numThreads + 1));

    typedef bsl::vector< bsl::shared_ptr<bslmt::Semaphore> > SemaphoreVector;
 
    SemaphoreVector waiterCallee(NTSCFG_TEST_ALLOCATOR);
    SemaphoreVector waiterCaller(NTSCFG_TEST_ALLOCATOR);

    waiterCallee.reserve(2);
    waiterCaller.reserve(2);

    for (bsl::size_t i = 0; i < 2; ++i) {
        bsl::shared_ptr<bslmt::Semaphore> calleeSemaphore;
        calleeSemaphore.createInplace(NTSCFG_TEST_ALLOCATOR);

        waiterCallee.push_back(calleeSemaphore);
    }

    for (bsl::size_t i = 0; i < 2; ++i) {
        bsl::shared_ptr<bslmt::Semaphore> callerSemaphore;
        callerSemaphore.createInplace(NTSCFG_TEST_ALLOCATOR);

        waiterCaller.push_back(callerSemaphore);
    }

    NTSCFG_TEST_EQ(waiterCallee.size(), 2);
    NTSCFG_TEST_EQ(waiterCaller.size(), 2);

    for (bsl::size_t i = 0; i < numThreads; ++i) {
        bslmt::ThreadAttributes threadAttributes;
        threadAttributes.setThreadName("thread-" + bsl::to_string(i));

        bsl::shared_ptr<bslmt::Semaphore> calleeSemaphore;
        if (i == 0) {
            calleeSemaphore = waiterCallee[0];
        }
        else {
            calleeSemaphore = waiterCallee[1];
        }

        bsl::shared_ptr<bslmt::Semaphore> callerSemaphore;
        if (i == 0) {
            callerSemaphore = waiterCaller[0];
        }
        else {
            callerSemaphore = waiterCaller[1];
        }

        int rc = threadGroup.addThread(
        NTCCFG_BIND(
            &ntccfg::MutexTest::thread,
            &startup,
            calleeSemaphore.get(),
            callerSemaphore.get(),
            &mutex,
            &value,
            static_cast<int>(i)),
        threadAttributes);

        NTSCFG_TEST_EQ(rc, 0);
    }

    NTSCFG_TEST_EQ(value, -1);

    // Wait for all threads to start.

    startup.wait();

    // All threads have started and are blocked waiting to execute the
    // next operation.

    BSLS_LOG_TRACE("Ready");

    // Unblock the driver thread and wait until it acquires the mutex.

    waiterCallee[0]->post();
    waiterCaller[0]->wait();

    NTSCFG_TEST_EQ(value, 0);

    // Unblock the remaining waiters.

    for (bsl::size_t i = 1; i < numThreads; ++i) {
        waiterCallee[1]->post();
    }

    // Wait until the remaining waiters are blocked waiting to acquire the
    // mutex.

    // bslmt::ThreadUtil::sleep(bsls::TimeInterval(1));

    // Unblock the driver thread and wait until it releases the mutex.

    waiterCallee[0]->post();
    waiterCaller[0]->wait();

    // Wait until a waiter acquires the mutex, but note that there is no
    // guarantee which one the scheduler will pick.

    for (bsl::size_t i = 1; i < numThreads; ++i) {
        waiterCaller[1]->wait();

        // int chosen = value;
        // BSLS_LOG_TRACE("Detected locker: %d", chosen);

        waiterCallee[1]->post();
    }

    threadGroup.joinAll();
}


NTSCFG_TEST_FUNCTION(ntccfg::MutexTest::verify1)
{
    ntccfg::Mutex mutex;

    mutex.lock();
    mutex.unlock();

    mutex.lock();
    mutex.unlock();
}

NTSCFG_TEST_FUNCTION(ntccfg::MutexTest::verify2)
{
    ntccfg::MutexTest::execute(2);
}

NTSCFG_TEST_FUNCTION(ntccfg::MutexTest::verify3)
{
    ntccfg::MutexTest::execute(3);
}

NTSCFG_TEST_FUNCTION(ntccfg::MutexTest::verify4)
{
    ntccfg::MutexTest::execute(4);
}

}  // close namespace ntccfg
}  // close namespace BloombergLP
