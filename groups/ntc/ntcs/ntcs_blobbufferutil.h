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

#ifndef INCLUDED_NTCS_BLOBBUFFERUTIL
#define INCLUDED_NTCS_BLOBBUFFERUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcs_metrics.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bdlbb_blob.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide utilities for allocating blob buffers.
///
/// @details
/// This component provides a utility for allocating the buffer necessary to
/// perform an efficient vectored read into a blob, avoiding previously defined
/// data but leveraging previously allocated, but as yet unused, capacity.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcs
struct BlobBufferUtil {
    /// Return the number of bytes to allocate to accomodate a new read into
    /// a read queue having the specified 'size' and 'capacity' to satisfy
    /// the specified 'lowWatermark', ensuring at least the specified
    /// 'minReceiveSize' but no more than the specified 'maxReceiveSize',
    /// inclusive.
    static size_t calculateNumBytesToAllocate(size_t size,
                                              size_t capacity,
                                              size_t lowWatermark,
                                              size_t minReceiveSize,
                                              size_t maxReceiveSize);

    /// Load more capacity buffers allocated from the specified
    /// 'blobBufferFactory' into the specified 'readQueue' to accomodate
    /// a new read into the unused capacity buffers of the 'readQueue' to
    /// satisfy the specified 'lowWatermark', ensuring at least the
    /// specified 'minReadSize' but no more than the specified
    /// 'maxReceiveSize', inclusive.
    static void reserveCapacity(bdlbb::Blob*              readQueue,
                                bdlbb::BlobBufferFactory* blobBufferFactory,
                                ntcs::Metrics*            metrics,
                                size_t                    lowWatermark,
                                size_t                    minReceiveSize,
                                size_t                    maxReceiveSize);
};

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
