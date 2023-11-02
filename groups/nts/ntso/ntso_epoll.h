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

#ifndef INCLUDED_NTSO_EPOLL
#define INCLUDED_NTSO_EPOLL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsi_reactor.h>
#include <ntsscm_version.h>
#include <bsl_memory.h>

#if defined(BSLS_PLATFORM_OS_LINUX)
#define NTSO_EPOLL_ENABLED 1
#else
#define NTSO_EPOLL_ENABLED 0
#endif

#if NTSO_EPOLL_ENABLED
namespace BloombergLP {
namespace ntso {

/// @internal @brief
/// Provide utilities for creating reactors implemented using the 'epoll' API
/// on Linux.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntso
struct EpollUtil {
    /// Create a new reactor. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static bsl::shared_ptr<ntsi::Reactor> createReactor(
        bslma::Allocator* basicAllocator = 0);
};

}  // close package namespace
}  // close enterprise namespace
#endif
#endif
