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

#include <ntcs_blobbufferutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_blobbufferutil_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntcs {

size_t BlobBufferUtil::calculateNumBytesToAllocate(size_t size,
                                                   size_t capacity,
                                                   size_t lowWatermark,
                                                   size_t minReceiveSize,
                                                   size_t maxReceiveSize)
{
    BSLS_ASSERT(capacity >= size);
    BSLS_ASSERT(minReceiveSize > 0);
    BSLS_ASSERT(maxReceiveSize > 0);

    if (minReceiveSize > maxReceiveSize) {
        minReceiveSize = maxReceiveSize;
    }

    bsl::size_t numBytesToAllocate = 0;
    if (lowWatermark > capacity) {
        numBytesToAllocate = lowWatermark - capacity;
    }

    bsl::size_t numBytesToBeAvailable = (capacity - size) + numBytesToAllocate;

    if (numBytesToBeAvailable < minReceiveSize) {
        bsl::size_t numBytesToAdjust = minReceiveSize - numBytesToBeAvailable;
        numBytesToAllocate           += numBytesToAdjust;
    }

    if (numBytesToBeAvailable > maxReceiveSize) {
        bsl::size_t numBytesToAdjust = numBytesToBeAvailable - maxReceiveSize;
        if (numBytesToAdjust > numBytesToAllocate) {
            numBytesToAllocate = 0;
        }
        else {
            numBytesToAllocate -= numBytesToAdjust;
        }
    }

    BSLS_ASSERT((capacity - size) + numBytesToAllocate >= 1);
    BSLS_ASSERT(numBytesToAllocate <= maxReceiveSize);

    return numBytesToAllocate;
}

void BlobBufferUtil::reserveCapacity(
    bdlbb::Blob*              readQueue,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    ntcs::Metrics*            metrics,
    size_t                    lowWatermark,
    size_t                    minReceiveSize,
    size_t                    maxReceiveSize)
{
    BSLS_ASSERT(minReceiveSize > 0);
    BSLS_ASSERT(maxReceiveSize > 0);

    size_t numBytesToAllocate =
        BlobBufferUtil::calculateNumBytesToAllocate(readQueue->length(),
                                                    readQueue->totalSize(),
                                                    lowWatermark,
                                                    minReceiveSize,
                                                    maxReceiveSize);

    bsl::size_t numBytesAllocated = 0;
    while (numBytesAllocated < numBytesToAllocate) {
        bdlbb::BlobBuffer buffer;
        blobBufferFactory->allocate(&buffer);

        bsl::size_t blobBufferCapacity = buffer.size();

        readQueue->appendBuffer(buffer);
        numBytesAllocated += blobBufferCapacity;

        if (metrics) {
            metrics->logBlobBufferAllocation(blobBufferCapacity);
        }
    }

    BSLS_ASSERT(static_cast<bsl::size_t>(readQueue->totalSize() -
                                         readQueue->length()) >=
                bsl::min(minReceiveSize, maxReceiveSize));
}

}  // close package namespace
}  // close enterprise namespace
