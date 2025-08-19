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

#ifndef INCLUDED_NTCI_CONCURRENT
#define INCLUDED_NTCI_CONCURRENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_config.h>
#include <ntccfg_platform.h>

#if NTC_BUILD_WITH_COROUTINES

#include <ntsa_error.h>
#include <ntsa_coroutine.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface for concurrent operations.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_utility
class Concurrent
{
public:
    /// Initialize concurrent operations.
    static void initialize();

    /// Clean up the resources required by all concurrent operations.
    static void exit();
};

}  // close namespace ntci
}  // close namespace BloombergLP

#endif
#endif

