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

#ifndef INCLUDED_NTSA_ALLOCATOR
#define INCLUDED_NTSA_ALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_config.h>
#include <ntscfg_platform.h>

namespace BloombergLP {
namespace ntsa {

/// Defines a type alias for the standard allocator type.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_system
typedef bsl::allocator<> Allocator;

/// Defines a type alias for the standard allocator argument disambiguator
/// type.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_system
typedef bsl::allocator_arg_t AllocatorArg;

}  // close namespace ntsa
}  // close namespace BloombergLP

#endif
