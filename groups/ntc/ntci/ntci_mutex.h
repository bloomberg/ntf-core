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
#include <ntccfg_mutex.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntci {

/// @internal @brief
/// Provide a synchronization primitive for mutually-exclusive access.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_utility
typedef ntccfg::Mutex Mutex;

/// @internal @brief
/// Define a type alias for a guard to lock and unlock a mutex.
///
/// @ingroup module_ntci_utility
typedef ntccfg::LockGuard LockGuard;

/// @internal @brief
/// Define a type alias for a guard to unlock and lock a mutex.
///
/// @ingroup module_ntci_utility
typedef ntccfg::UnLockGuard UnLockGuard;

}  // close package namespace
}  // close enterprise namespace
#endif
