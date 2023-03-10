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

#ifndef INCLUDED_NTCI_WAITER
#define INCLUDED_NTCI_WAITER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntci {

/// Define a type alias for a handle to a thread authorized to call 'wait'.
///
/// @ingroup module_ntci_runtime
typedef void* Waiter;

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
