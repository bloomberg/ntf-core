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

#ifndef INCLUDED_NTCI_MUTEX
#define INCLUDED_NTCI_MUTEX

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslmt_mutex.h>
#include <bsls_atomic.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#if defined(BSLS_PLATFORM_OS_LINUX) &&                                        \
    (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))
#define NTCI_MUTEX_SYSTEM 1
#else
#define NTCI_MUTEX_SYSTEM 0
#endif

namespace BloombergLP {
namespace ntci {

#if NTCI_MUTEX_SYSTEM

// Some versions of GCC issue a spurious warning that the 'current' paramter
// is set but not used when 'Mutex::compareAndSwap' is called.
#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-parameter"
#endif

/// @internal @brief
/// Provide a synchronization primitive for mutually-exclusive access.
///
/// @par Thread Safety
// This class is thread safe.
///
/// @ingroup module_ntci_utility
class __attribute__((__aligned__(sizeof(int)))) Mutex
{
    int d_value;

  private:
    Mutex(const Mutex&) BSLS_KEYWORD_DELETED;
    Mutex& operator=(const Mutex&) BSLS_KEYWORD_DELETED;

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
    Mutex();

    /// Destroy this object.
    ~Mutex();

    /// Lock the mutex.
    void lock();

    /// Unlock the mutex.
    void unlock();
};

/// @internal @brief
/// Define a type alias for a guard to lock and unlock a mutex.
///
/// @ingroup module_ntci_utility
typedef bslmt::LockGuard<ntci::Mutex> LockGuard;

/// @internal @brief
/// Define a type alias for a guard to unlock and lock a mutex.
///
/// @ingroup module_ntci_utility
typedef bslmt::UnLockGuard<ntci::Mutex> UnLockGuard;

NTCCFG_INLINE
int Mutex::compareAndSwap(int* current, int expected, int desired)
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
Mutex::Mutex()
{
    __atomic_store_n(&d_value, 0, __ATOMIC_RELAXED);
}

NTCCFG_INLINE
Mutex::~Mutex()
{
}

NTCCFG_INLINE
void Mutex::lock()
{
    int previous = compareAndSwap(&d_value, 0, 1);
    if (previous != 0) {
        this->lockContention(previous);
    }
}

NTCCFG_INLINE
void Mutex::unlock()
{
    int previous = __atomic_fetch_sub(&d_value, 1, __ATOMIC_SEQ_CST);
    if (previous != 1) {
        this->unlockContention();
    }
}

#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic pop
#endif

#else

/// @internal @brief
/// Provide a synchronization primitive for mutually-exclusive access.
///
/// @par Thread Safety
// This class is thread safe.
typedef bslmt::Mutex Mutex;

/// @internal @brief
/// Define a type alias for a guard to lock and unlock a mutex.
typedef bslmt::LockGuard<bslmt::Mutex> LockGuard;

/// @internal @brief
/// Define a type alias for a guard to unlock and lock a mutex.
typedef bslmt::UnLockGuard<bslmt::Mutex> UnLockGuard;

#endif

}  // close package namespace
}  // close enterprise namespace
#endif
