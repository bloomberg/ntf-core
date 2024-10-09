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

#ifndef INCLUDED_NTCCFG_TRAITS
#define INCLUDED_NTCCFG_TRAITS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntcscm_version.h>
#include <bslalg_typetraits.h>
#include <bsl_memory.h>

/// Declare the specified 'TYPE' is bitwise-movable.
/// @ingroup module_ntccfg
#define NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(TYPE)                    \
    BSLALG_DECLARE_NESTED_TRAITS2(                                            \
        TYPE,                                                                 \
        bslalg::TypeTraitBitwiseMoveable,                                     \
        bslalg::TypeTraitBitwiseCopyable)

/// Declare the specified 'TYPE' uses an allocator to supply memory.
/// @ingroup module_ntccfg
#define NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(TYPE)                     \
    BSLALG_DECLARE_NESTED_TRAITS(TYPE, bslalg::TypeTraitUsesBslmaAllocator)

#endif
