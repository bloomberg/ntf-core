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

#ifndef INCLUDED_NTSA_HANDLE
#define INCLUDED_NTSA_HANDLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsls_platform.h>
#include <bsl_cstddef.h>

namespace BloombergLP {
namespace ntsa {

#if defined(BSLS_PLATFORM_OS_UNIX)

/// Provide a portable definition of a socket handle.
///
/// @details
/// Define a type alias for the native socket type on the current
/// platform.
///
/// @ingroup module_ntsa_system
typedef int Handle;

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

/// Provide a portable definition of a socket handle.
///
/// @details
/// Define a type alias for the native socket type on the current
/// platform.
///
/// @ingroup module_ntsa_system
typedef bsl::size_t Handle;

#else
#error Not implemented
#endif

/// The invalid socket handle.
///
/// @ingroup module_ntsa_system
extern const ntsa::Handle k_INVALID_HANDLE;

}  // close package namespace
}  // close enterprise namespace
#endif
