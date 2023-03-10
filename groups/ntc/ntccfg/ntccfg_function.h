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

#ifndef INCLUDED_NTCCFG_FUNCTION
#define INCLUDED_NTCCFG_FUNCTION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_config.h>
#include <ntcscm_version.h>
#include <bsl_functional.h>
#include <bsl_memory.h>

/// @internal @brief
/// Declare a bindable function returning void with the specified arguments.
///
/// @ingroup module_ntccfg
#define NTCCFG_FUNCTION(...) bsl::function<void(__VA_ARGS__)>

/// @internal @brief
/// Declare a bindable function returning a value of the specified
/// 'RETURN_TYPE' with the specified arguments.
///
/// @ingroup module_ntccfg
#define NTCCFG_FUNCTION_NV(ReturnType, ...)                                   \
    bsl::function<ReturnType(__VA_ARGS__)>

/// @internal @brief
/// The initialization expression for the default constructor of a bindable
/// function using the specified 'allocator'.
///
/// @ingroup module_ntccfg
#define NTCCFG_FUNCTION_INIT(allocator) bsl::allocator_arg, allocator

/// @internal @brief
/// The initialization expression for the copy constructor of a bindable
/// function that copies the specified 'source' bindable function using the
/// specified 'allocator'.
///
/// @ingroup module_ntccfg
#define NTCCFG_FUNCTION_COPY(source, allocator)                               \
    bsl::allocator_arg, allocator, source

#endif
