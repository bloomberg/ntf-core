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
#include <ntccfg_mutex.h>
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
/// The initializer for a basic, non-recursive mutex.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_INIT

/// @internal @brief
/// Enter a stack scope, lock the specified basic, non-recursive 'mutex'
/// pointer, and automatically unlock the 'mutex' when the stack scope is left.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_SCOPE_ENTER(mutex)                                        \
    {                                                                         \
        bslmt::LockGuard<ntccfg::Mutex> NTCCFG_LOCK_SCOPE_NAME(               \
            LOCK_SCOPE_GUARD_,                                                \
            __LINE__)((mutex));                                               \
        {

/// @internal @brief
/// Leave the current stack scope locked by the specified basic, non-recursive
/// 'mutex'.
///
/// @ingroup module_ntccfg
#define NTCCFG_LOCK_SCOPE_LEAVE(mutex)                                        \
    }                                                                         \
    }

}  // close package namespace
}  // close enterprise namespace
#endif
