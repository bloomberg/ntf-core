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

#ifndef INCLUDED_NTCCFG_INLINE
#define INCLUDED_NTCCFG_INLINE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_config.h>
#include <ntcscm_version.h>
#include <bsls_platform.h>

#if NTC_BUILD_WITH_INLINING_FORCED

#if defined(NDEBUG) || defined(BDE_BUILD_TARGET_OPT)
#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)

/// Force the following function be be inlined at its call site.
/// @ingroup module_ntccfg
#define NTCCFG_INLINE inline __attribute__((always_inline))

#elif defined(BSLS_PLATFORM_CMP_MSVC)

/// Force the following function be inlined at its call site.
/// @ingroup module_ntccfg
#define NTCCFG_INLINE __forceinline

#else

/// Hint that the following function can be inlined at its call site.
/// @ingroup module_ntccfg
#define NTCCFG_INLINE inline

#endif
#else

/// Hint that the following function can be inlined at its call site.
/// @ingroup module_ntccfg
#define NTCCFG_INLINE inline

#endif

#elif NTC_BUILD_WITH_INLINING_SUGGESTED

/// Hint that the following function can be inlined at its call site.
/// @ingroup module_ntccfg
#define NTCCFG_INLINE inline

#elif NTC_BUILD_WITH_INLINING_DISABLED
#error Not implemented
#else
#error Not implemented
#endif

#endif
