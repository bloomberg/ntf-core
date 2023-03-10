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

#ifndef INCLUDED_NTCCFG_LOCK
#define INCLUDED_NTCCFG_LOCK

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_config.h>
#include <ntcscm_version.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_recursivemutex.h>
#include <bsls_platform.h>
#include <bsls_spinlock.h>

namespace BloombergLP {
namespace ntccfg {

/// @internal @brief
/// Concatenate two compile-type identifier into a single identifier.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_SCOPE_NAME_JOIN(a, b) a##b

/// @internal @brief
/// The identifier of a lock scope, formed from the specified 'prefix'
/// concatenated with the specified 'disambiguator'.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_SCOPE_NAME(prefix, disambiguator)                         \
    NTCCFG_LOCK_SCOPE_NAME_JOIN(prefix, disambiguator)

/// @internal @brief
/// Define a type alias for a spin lock.
///
/// @ingroup module_ntccfg
typedef bsls::SpinLock SpinLock;

/// @internal @brief
/// The initializer for a spin lock.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_INIT_SPIN bsls::SpinLock::s_unlocked

/// @internal @brief
/// Enter a stack scope, lock the specified spin-lock 'mutex' pointer, and
/// automatically unlock the 'mutex' when the stack scope is left.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_SCOPE_ENTER_SPIN(mutex)                                   \
    {                                                                         \
        bsls::SpinLockGuard NTCCFG_LOCK_SCOPE_NAME(SPIN_LOCK_SCOPE_GUARD_,    \
                                                   __LINE__)((mutex));        \
        {
/// @internal @brief
/// Leave the current stack scope locked by the specified spin-lock 'mutex'.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_SCOPE_LEAVE_SPIN(mutex)                                   \
    }                                                                         \
    }

/// @internal @brief
/// Define a type alias for a basic, non-recursive mutex.
///
/// @ingroup module_ntccfg
typedef bslmt::Mutex BasicMutex;

/// @internal @brief
/// The initializer for a basic, non-recursive mutex.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_INIT_BASIC

/// @internal @brief
/// Enter a stack scope, lock the specified basic, non-recursive 'mutex'
/// pointer, and automatically unlock the 'mutex' when the stack scope is left.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_SCOPE_ENTER_BASIC(mutex)                                  \
    {                                                                         \
        bslmt::LockGuard<bslmt::Mutex> NTCCFG_LOCK_SCOPE_NAME(                \
            BASIC_MUTEX_SCOPE_GUARD_,                                         \
            __LINE__)((mutex));                                               \
        {
/// @internal @brief
/// Leave the current stack scope locked by the specified basic, non-recursive
/// 'mutex'.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_SCOPE_LEAVE_BASIC(mutex)                                  \
    }                                                                         \
    }

/// @internal @brief
/// Define a type alias for a recursive mutex.
///
/// @ingroup module_ntccfg
typedef bslmt::RecursiveMutex RecursiveMutex;

/// @internal @brief
/// The initializer for a recursive mutex.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_INIT_RECURSIVE

/// @internal @brief
/// Enter a stack scope, lock the specified recursive 'mutex' pointer, and
/// automatically unlock the 'mutex' when the stack scope is left.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_SCOPE_ENTER_RECURSIVE(mutex)                              \
    {                                                                         \
        bslmt::LockGuard<bslmt::RecursiveMutex> NTCCFG_LOCK_SCOPE_NAME(       \
            RECURSIVE_MUTEX_SCOPE_GUARD_,                                     \
            __LINE__)((mutex));                                               \
        {
/// @internal @brief
/// Leave the current stack scope locked by the specified recursive 'mutex'.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_SCOPE_LEAVE_RECURSIVE(mutex)                              \
    }                                                                         \
    }

#if NTC_BUILD_WITH_SPIN_LOCKS

/// @internal @brief
/// Define a type alias for a non-recursive mutex, which may be
/// either a regular system mutex, a recursive mutex, or a spin lock, depending
/// on the build configuration.
///
/// @ingroup module_ntccfg
typedef ntccfg::SpinLock Mutex;

/// @internal @brief
/// The initializer for a non-recursive mutex, which may be either a regular
/// system mutex, a recursive mutex, or a spin lock, depending on the build
/// configuration.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_INIT NTCCFG_LOCK_INIT_SPIN

/// @internal @brief
/// Enter a stack scope, lock the specified 'mutex' pointer, which may be
/// either a regular system mutex, a recursive mutex, or a spin lock, depending
/// on the build configuration, and automatically unlock the 'mutex' when the
/// stack scope is left.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_SCOPE_ENTER(mutex) NTCCFG_LOCK_SCOPE_ENTER_SPIN(mutex)

/// @internal @brief
/// Leave the current stack scope locked by the specified 'mutex' pointer,
/// which may be either a regular system mutex, a recursive mutex, or a spin
/// lock, depending on the build configuration.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_SCOPE_LEAVE(mutex) NTCCFG_LOCK_SCOPE_LEAVE_SPIN(mutex)

#elif NTC_BUILD_WITH_RECURSIVE_MUTEXES

/// @internal @brief
/// Define a type alias for a non-recursive mutex, which may be
/// either a regular system mutex, a recursive mutex, or a spin lock, depending
/// on the build configuration.
///
/// @ingroup module_ntccfg
typedef ntccfg::RecursiveMutex Mutex;

/// @internal @brief
/// The initializer for a non-recursive mutex, which may be either a regular
/// system mutex, a recursive mutex, or a spin lock, depending on the build
/// configuration.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_INIT NTCCFG_LOCK_INIT_RECURSIVE

/// @internal @brief
/// Enter a stack scope, lock the specified 'mutex' pointer, which may be
/// either a regular system mutex, a recursive mutex, or a spin lock, depending
/// on the build configuration, and automatically unlock the 'mutex' when the
/// stack scope is left.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_SCOPE_ENTER(mutex) NTCCFG_LOCK_SCOPE_ENTER_RECURSIVE(mutex)

/// @internal @brief
/// Leave the current stack scope locked by the specified 'mutex' pointer,
/// which may be either a regular system mutex, a recursive mutex, or a spin
/// lock, depending on the build configuration.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_SCOPE_LEAVE(mutex) NTCCFG_LOCK_SCOPE_LEAVE_RECURSIVE(mutex)

#else

/// @internal @brief
/// Define a type alias for a non-recursive mutex, which may be
/// either a regular system mutex, a recursive mutex, or a spin lock, depending
/// on the build configuration.
///
/// @ingroup module_ntccfg
typedef ntccfg::BasicMutex Mutex;

/// @internal @brief
/// The initializer for a non-recursive mutex, which may be either a regular
/// system mutex, a recursive mutex, or a spin lock, depending on the build
/// configuration.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_INIT NTCCFG_LOCK_INIT_BASIC

/// @internal @brief
/// Enter a stack scope, lock the specified 'mutex' pointer, which may be
/// either a regular system mutex, a recursive mutex, or a spin lock, depending
/// on the build configuration, and automatically unlock the 'mutex' when the
/// stack scope is left.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_SCOPE_ENTER(mutex) NTCCFG_LOCK_SCOPE_ENTER_BASIC(mutex)

/// @internal @brief
/// Leave the current stack scope locked by the specified 'mutex' pointer,
/// which may be either a regular system mutex, a recursive mutex, or a spin
/// lock, depending on the build configuration.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_SCOPE_LEAVE(mutex) NTCCFG_LOCK_SCOPE_LEAVE_BASIC(mutex)

#endif

}  // close package namespace
}  // close enterprise namespace
#endif
