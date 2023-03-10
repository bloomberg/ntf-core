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

#ifndef INCLUDED_NTCS_PROCESSUTIL
#define INCLUDED_NTCS_PROCESSUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcs_processstatistics.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide utilities for the current process and child processes.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
struct ProcessUtil {
    /// Load into the specified 'result' the resource usage of the current
    /// process.
    static void getResourceUsage(ntcs::ProcessStatistics* result);
};

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
