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

#ifndef INCLUDED_NTCCFG_BIT
#define INCLUDED_NTCCFG_BIT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_config.h>
#include <ntcscm_version.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsl_cstddef.h>
#include <bsl_cstdint.h>

/// @internal @brief
/// Return true if all the bits in the specified 'mask' are set in the
/// specified 'value', otherwise return false.
///
/// @ingroup module_ntccfg
#define NTCCFG_BIT_IS_SET(value, mask) (((value) & (mask)) == (mask))

/// @internal @brief
/// Return true if any the bits in the specified 'mask' are set in the
/// specified 'value', otherwise return false.
///
/// @ingroup module_ntccfg
#define NTCCFG_BIT_IS_SET_ANY(value, mask) (((value) & (mask)) != 0)

/// @internal @brief
/// Return true if all the bits in the specified 'mask' are not set in the
/// specified 'value', otherwise return false.
///
/// @ingroup module_ntccfg
#define NTCCFG_BIT_IS_NOT_SET(value, mask) (((value) & (mask)) != (mask))

/// @internal @brief
/// Set the bits of the specified 'mask' in the specified 'value'.
///
/// @ingroup module_ntccfg
#define NTCCFG_BIT_SET(value, mask) ((*(value)) |= (mask))

/// @internal @brief
/// Clear the bits of the specified 'mask' in the specified 'value'.
///
/// @ingroup module_ntccfg
#define NTCCFG_BIT_CLEAR(value, mask) ((*(value)) &= ~(mask))

/// @internal @brief
/// Return true if all the bits in the specified 0-based bit 'index' are set in
/// the specified 'value', otherwise return false.
///
/// @ingroup module_ntccfg
#define NTCCFG_BIT_INDEX_IS_SET(value, index)                                 \
    (((value) & (1 << (index))) == (1 << (index)))

/// @internal @brief
/// Set the bits of the specified 0-based bit 'index' in the specified 'value'.
///
/// @ingroup module_ntccfg
#define NTCCFG_BIT_INDEX_SET(value, index) ((*(value)) |= ((1 << (index))))

/// @internal @brief
/// Clear the bits of the specified 0-based bit 'index' in the specified
/// 'value'.
///
/// @ingroup module_ntccfg
#define NTCCFG_BIT_INDEX_CLEAR(value, index) ((*(value)) &= ~((1 << (index))))

/// @internal @brief
/// Return a 16-bit bit mask with the bit at the specified 'x' position set.
///
/// @ingroup module_ntccfg
#define NTCCFG_BIT_FLAG_16(x) ((bsl::uint16_t)(1U << (x)))

/// @internal @brief
/// Return a 32-bit bit mask with the bit at the specified 'x' position set.
///
/// @ingroup module_ntccfg
#define NTCCFG_BIT_FLAG_32(x) ((bsl::uint32_t)(1U << (x)))

/// @internal @brief
/// Return a 64-bit bit mask with the bit at the specified 'x' position set.
///
/// @ingroup module_ntccfg
#define NTCCFG_BIT_FLAG_64(x) ((bsl::uint64_t)(1U << (x)))

/// @internal @brief
/// Return a 16-bit bit mask with the specified bit positions 'a' through 'b',
/// inclusive, set to 1. Bit positions are zero-based, so 'a' and 'b' must be
/// between 0-15.
///
/// @ingroup module_ntccfg
#define NTCCFG_BIT_MASK_16(a, b)                                              \
    ((bsl::uint16_t)((((bsl::uint16_t)(-1)) >> (15 - (b))) &                  \
                     (~((1U << (a)) - 1))))

/// @internal @brief
/// Return a 32-bit bit mask with the specified bit positions 'a' through 'b',
/// inclusive, set to 1. Bit positions are zero-based, so 'a' and 'b' must be
/// between 0-31.
///
/// @ingroup module_ntccfg
#define NTCCFG_BIT_MASK_32(a, b)                                              \
    ((bsl::uint32_t)((((bsl::uint32_t)(-1)) >> (31 - (b))) &                  \
                     (~((1U << (a)) - 1))))

/// @internal @brief
/// Return a 64-bit bit mask with the specified bit positions 'a' through 'b',
/// inclusive, set to 1. Bit positions are zero-based, so 'a' and 'b' must be
/// between 0-63.
///
/// @ingroup module_ntccfg
#define NTCCFG_BIT_MASK_64(a, b)                                              \
    ((bsl::uint64_t)((((bsl::uint64_t)(-1)) >> (63 - (b))) &                  \
                     (~((1U << (a)) - 1))))

#endif
