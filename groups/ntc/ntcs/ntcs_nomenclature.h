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

#ifndef INCLUDED_NTCS_NOMENCLATURE
#define INCLUDED_NTCS_NOMENCLATURE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide a utility to generate names for anonymous objects.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcs
struct Nomenclature {
    /// Return a metric name for an anonymous reactor.
    static bsl::string createReactorName();

    /// Return a metric name for an anonymous proactor.
    static bsl::string createProactorName();

    /// Return a metric name for an anonymous thread.
    static bsl::string createThreadName();

    /// Return a metric name for an anonymous thread pool.
    static bsl::string createThreadPoolName();

    /// Return a metric name for an anonymous interface.
    static bsl::string createInterfaceName();
};

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
