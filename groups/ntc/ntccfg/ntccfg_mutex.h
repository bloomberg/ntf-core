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

#ifndef INCLUDED_NTCCFG_MUTEX
#define INCLUDED_NTCCFG_MUTEX

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_config.h>
#include <ntccfg_inline.h>
#include <ntcscm_version.h>
#include <bslmt_condition.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_recursivemutex.h>
#include <bsls_atomic.h>
#include <bsls_spinlock.h>
#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_OS_LINUX) &&                                        \
    (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))
#define NTCCFG_FUTEX_ENABLED 1
#else
#define NTCCFG_FUTEX_ENABLED 0
#endif

namespace BloombergLP {
namespace ntccfg {

#if NTCCFG_FUTEX_ENABLED

// Some versions of GCC issue a spurious warning that the 'current' paramter
// is set but not used when 'Futex::compareAndSwap' is called.
#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-parameter"
#endif

/// @internal @brief
/// Provide a synchronization primitive for mutually-exclusive access
/// implemented by a Linux futex.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntccfg
class __attribute__((__aligned__(sizeof(int)))) Futex
{
    int d_value;

  private:
    Futex(const Futex&) BSLS_KEYWORD_DELETED;
    Futex& operator=(const Futex&) BSLS_KEYWORD_DELETED;

  private:
    /// Wait until the lock may be acquired.
    void wait();

    /// Wake the next thread waiting to acquire the lock.
    void wake();

    /// Continue locking the mutex after discovering the mutex was probably
    /// previously unlocked.
    void lockContention(int c);

    /// Continue unlocking the mutex after discovering the mutex probably
    /// has other threads trying to lock the mutex.
    void unlockContention();

    /// Compare the specified '*current' value to the specified 'expected'
    /// value, and if equal, set '*current' to 'desired'. Return the
    /// previous value of 'current'.
    static int compareAndSwap(int* current, int expected, int desired);

  public:
    /// Create a new mutex.
    Futex();

    /// Destroy this object.
    ~Futex();

    /// Lock the mutex.
    void lock();

    /// Unlock the mutex.
    void unlock();
};

NTCCFG_INLINE
int Futex::compareAndSwap(int* current, int expected, int desired)
{
    int* ep = &expected;
    int* dp = &desired;

    __atomic_compare_exchange(current,
                              ep,
                              dp,
                              false,
                              __ATOMIC_SEQ_CST,
                              __ATOMIC_SEQ_CST);

    return *ep;
}

NTCCFG_INLINE
Futex::Futex()
{
    __atomic_store_n(&d_value, 0, __ATOMIC_RELAXED);
}

NTCCFG_INLINE
Futex::~Futex()
{
}

NTCCFG_INLINE
void Futex::lock()
{
    int previous = compareAndSwap(&d_value, 0, 1);
    if (previous != 0) {
        this->lockContention(previous);
    }
}

NTCCFG_INLINE
void Futex::unlock()
{
    int previous = __atomic_fetch_sub(&d_value, 1, __ATOMIC_SEQ_CST);
    if (previous != 1) {
        this->unlockContention();
    }
}

#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic pop
#endif

#endif // NTCCFG_FUTEX_ENABLED

/// @internal @brief
/// Provide a synchronization primitive for mutually-exclusive access
/// implemented by a Linux futex.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntccfg
class SpinLock
{
    bsls::SpinLock d_lock;

  private:
    SpinLock(const SpinLock&) BSLS_KEYWORD_DELETED;
    SpinLock& operator=(const SpinLock&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new spin lock.
    SpinLock();

    explicit SpinLock(int state);

    /// Destroy this object.
    ~SpinLock();

    /// Lock the mutex.
    void lock();

    /// Unlock the mutex.
    void unlock();
};

NTCCFG_INLINE
SpinLock::SpinLock()
: d_lock(bsls::SpinLock::s_unlocked)
{
}

NTCCFG_INLINE
SpinLock::~SpinLock()
{
}

NTCCFG_INLINE
void SpinLock::lock()
{
    d_lock.lock();
}

NTCCFG_INLINE
void SpinLock::unlock()
{
    d_lock.unlock();
}

/// @internal @brief
/// Implement mutexes by the mutex provided by BDE.
///
/// @ingroup module_ntccfg
#define NTCCFG_MUTEX_IMPL_BSLMT_MUTEX 0

/// @internal @brief
/// Implement mutexes by the recursive mutex provided by BDE.
///
/// @ingroup module_ntccfg
#define NTCCFG_MUTEX_IMPL_BSLMT_RECURSIVE_MUTEX 1

/// @internal @brief
/// Implement mutexes by a spin lock provided by BDE.
///
/// @ingroup module_ntccfg
#define NTCCFG_MUTEX_IMPL_BSLMT_SPIN_LOCK 2

/// @internal @brief
/// Implement mutexes by a Linux futex.
///
/// @ingroup module_ntccfg
#define NTCCFG_MUTEX_IMPL_FUTEX 3

#if NTC_BUILD_WITH_SPIN_LOCKS

/// @internal @brief
/// The mutex implementation selector.
///
/// @ingroup module_ntccfg
#define NTCCFG_MUTEX_IMPL NTCCFG_MUTEX_IMPL_BSLMT_SPIN_LOCK

#elif NTC_BUILD_WITH_USERSPACE_MUTEXES

#if NTCCFG_FUTEX_ENABLED

/// @internal @brief
/// The mutex implementation selector.
///
/// @ingroup module_ntccfg
#define NTCCFG_MUTEX_IMPL NTCCFG_MUTEX_IMPL_FUTEX

#else

/// @internal @brief
/// The mutex implementation selector.
///
/// @ingroup module_ntccfg
#define NTCCFG_MUTEX_IMPL NTCCFG_MUTEX_IMPL_BSLMT_MUTEX

#endif

#elif NTC_BUILD_WITH_SYSTEM_MUTEXES

/// @internal @brief
/// The mutex implementation selector.
///
/// @ingroup module_ntccfg
#define NTCCFG_MUTEX_IMPL NTCCFG_MUTEX_IMPL_BSLMT_MUTEX

#elif NTC_BUILD_WITH_RECURSIVE_MUTEXES

/// @internal @brief
/// The mutex implementation selector.
///
/// @ingroup module_ntccfg
#define NTCCFG_MUTEX_IMPL NTCCFG_MUTEX_IMPL_BSLMT_RECURSIVE_MUTEX

#else
#error Not implemented
#endif

#if NTCCFG_MUTEX_IMPL == NTCCFG_MUTEX_IMPL_BSLMT_MUTEX

/// @internal @brief
/// Provide a synchronization primitive for mutually-exclusive access.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntccfg
typedef bslmt::Mutex Mutex;

/// @internal @brief
/// Define a type alias for a guard to lock and unlock a mutex.
///
/// @ingroup module_ntccfg
typedef bslmt::LockGuard<bslmt::Mutex> LockGuard;

/// @internal @brief
/// Define a type alias for a guard to unlock and lock a mutex.
///
/// @ingroup module_ntccfg
typedef bslmt::UnLockGuard<bslmt::Mutex> UnLockGuard;

#elif NTCCFG_MUTEX_IMPL == NTCCFG_MUTEX_IMPL_BSLMT_RECURSIVE_MUTEX

/// @internal @brief
/// Provide a synchronization primitive for mutually-exclusive access.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntccfg
typedef bslmt::RecursiveMutex Mutex;

/// @internal @brief
/// Define a type alias for a guard to lock and unlock a mutex.
///
/// @ingroup module_ntccfg
typedef bslmt::LockGuard<bslmt::RecursiveMutex> LockGuard;

/// @internal @brief
/// Define a type alias for a guard to unlock and lock a mutex.
///
/// @ingroup module_ntccfg
typedef bslmt::UnLockGuard<bslmt::RecursiveMutex> UnLockGuard;

#elif NTCCFG_MUTEX_IMPL == NTCCFG_MUTEX_IMPL_BSLMT_SPIN_LOCK

/// @internal @brief
/// Provide a synchronization primitive for mutually-exclusive access.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntccfg
typedef ntccfg::SpinLock Mutex;

/// @internal @brief
/// Define a type alias for a guard to lock and unlock a mutex.
///
/// @ingroup module_ntccfg
typedef bslmt::LockGuard<ntccfg::SpinLock> LockGuard;

/// @internal @brief
/// Define a type alias for a guard to unlock and lock a mutex.
///
/// @ingroup module_ntccfg
typedef bslmt::UnLockGuard<ntccfg::SpinLock> UnLockGuard;

#elif NTCCFG_MUTEX_IMPL == NTCCFG_MUTEX_IMPL_FUTEX

/// @internal @brief
/// Provide a synchronization primitive for mutually-exclusive access.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntccfg
typedef ntccfg::Futex Mutex;

/// @internal @brief
/// Define a type alias for a guard to lock and unlock a mutex.
///
/// @ingroup module_ntccfg
typedef bslmt::LockGuard<ntccfg::Futex> LockGuard;

/// @internal @brief
/// Define a type alias for a guard to unlock and lock a mutex.
///
/// @ingroup module_ntccfg
typedef bslmt::UnLockGuard<ntccfg::Futex> UnLockGuard;

#else
#error Not implemented
#endif

#define NTCCFG_MUTEX_NULL ((ntccfg::Mutex*)(0))

/// @internal @brief
/// Define a type alias for a condition variable.
///
/// @ingroup module_ntccfg
typedef bslmt::Condition Condition;

/// @internal @brief
/// Define a type alias for a mutex to lock the state associated with a 
/// condition variable.
///
/// @ingroup module_ntccfg
typedef bslmt::Mutex ConditionMutex;

/// @internal @brief
/// Define a type alias for a guard to lock and unlock a condition variable 
/// mutex.
///
/// @ingroup module_ntccfg
typedef bslmt::LockGuard<bslmt::Mutex> ConditionMutexGuard;

/// @internal @brief
/// Define a type alias for a guard to unlock and lock a condition variable 
/// mutex.
///
/// @ingroup module_ntccfg
typedef bslmt::UnLockGuard<bslmt::Mutex> ConditionMutexUnLockGuard;

}  // close package namespace
}  // close enterprise namespace
#endif
