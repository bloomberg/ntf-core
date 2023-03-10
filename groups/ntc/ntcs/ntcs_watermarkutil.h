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

#ifndef INCLUDED_NTCS_WATERMARKUTIL
#define INCLUDED_NTCS_WATERMARKUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide a utility for evaluating watermarks.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcs
struct WatermarkUtil {
    /// Sanitize the specified 'incomingQueueLowWatermark' and
    /// 'incomingQueueHighWatermark' to valid values, if necessary.
    static void sanitizeIncomingQueueWatermarks(
        bsl::size_t* incomingQueueLowWatermark,
        bsl::size_t* incomingQueueHighWatermark);

    /// Sanitize the specified 'outgoingQueueLowWatermark' and
    /// 'outgoingQueueHighWatermark' to valid values, if necessary.
    static void sanitizeOutgoingQueueWatermarks(
        bsl::size_t* outgoingQueueLowWatermark,
        bsl::size_t* outgoingQueueHighWatermark);

    /// Return true if the specified 'incomingQueueSize' satisfies the
    /// specified 'incomingQueueLowWatermark', otherwise return false.
    static bool isIncomingQueueLowWatermarkSatisfied(
        bsl::size_t incomingQueueSize,
        bsl::size_t incomingQueueLowWatermark);

    /// Return true if the specified 'incomingQueueSize' violates the
    /// specified 'incomingQueueHighWatermark', otherwise return false.
    static bool isIncomingQueueHighWatermarkViolated(
        bsl::size_t incomingQueueSize,
        bsl::size_t incomingQueueHighWatermark);

    /// Return true if the specified 'outgoingQueueSize' satisfies the
    /// specified 'outgoingQueueLowWatermark', otherwise return false.
    static bool isOutgoingQueueLowWatermarkSatisfied(
        bsl::size_t outgoingQueueSize,
        bsl::size_t outgoingQueueLowWatermark);

    /// Return true if the specified 'outgoingQueueSize' violates the
    /// specified 'outgoingQueueHighWatermark', otherwise return false.
    static bool isOutgoingQueueHighWatermarkViolated(
        bsl::size_t outgoingQueueSize,
        bsl::size_t outgoingQueueHighWatermark);
};

NTCCFG_INLINE
void WatermarkUtil::sanitizeIncomingQueueWatermarks(
    bsl::size_t* incomingQueueLowWatermark,
    bsl::size_t* incomingQueueHighWatermark)
{
    if (*incomingQueueLowWatermark == 0) {
        *incomingQueueLowWatermark = 1;
    }

    if (*incomingQueueHighWatermark < *incomingQueueLowWatermark) {
        *incomingQueueHighWatermark = *incomingQueueLowWatermark;
    }
}

NTCCFG_INLINE
void WatermarkUtil::sanitizeOutgoingQueueWatermarks(
    bsl::size_t* outgoingQueueLowWatermark,
    bsl::size_t* outgoingQueueHighWatermark)
{
    if (*outgoingQueueHighWatermark == 0) {
        *outgoingQueueHighWatermark = 1;
    }

    if (*outgoingQueueLowWatermark >= *outgoingQueueHighWatermark) {
        *outgoingQueueLowWatermark = *outgoingQueueHighWatermark - 1;
    }
}

NTCCFG_INLINE
bool WatermarkUtil::isIncomingQueueLowWatermarkSatisfied(
    bsl::size_t incomingQueueSize,
    bsl::size_t incomingQueueLowWatermark)
{
    return incomingQueueSize >= incomingQueueLowWatermark;
}

NTCCFG_INLINE
bool WatermarkUtil::isIncomingQueueHighWatermarkViolated(
    bsl::size_t incomingQueueSize,
    bsl::size_t incomingQueueHighWatermark)
{
    return incomingQueueSize >= incomingQueueHighWatermark;
}

NTCCFG_INLINE
bool WatermarkUtil::isOutgoingQueueLowWatermarkSatisfied(
    bsl::size_t outgoingQueueSize,
    bsl::size_t outgoingQueueLowWatermark)
{
    return outgoingQueueSize <= outgoingQueueLowWatermark;
}

NTCCFG_INLINE
bool WatermarkUtil::isOutgoingQueueHighWatermarkViolated(
    bsl::size_t outgoingQueueSize,
    bsl::size_t outgoingQueueHighWatermark)
{
    return outgoingQueueSize >= outgoingQueueHighWatermark;
}

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
