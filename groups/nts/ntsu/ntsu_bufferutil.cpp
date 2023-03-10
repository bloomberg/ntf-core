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

#include <ntsu_bufferutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsu_bufferutil_cpp, "$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace ntsu {

void BufferUtil::gather(bsl::size_t*       numBuffersTotal,
                        bsl::size_t*       numBytesTotal,
                        ntsa::ConstBuffer* iovecArray,
                        bsl::size_t        iovecArraySize,
                        const bdlbb::Blob& data,
                        bsl::size_t        maxBytes)
{
    bsl::size_t cumulativeBytesTotal   = 0;
    bsl::size_t cumulativeBuffersTotal = 0;

    if (maxBytes == 0) {
        maxBytes = bsl::numeric_limits<bsl::size_t>::max();
    }

    bsl::size_t numBytesLeft = maxBytes;

    const bsl::size_t numDataBuffers = data.numDataBuffers();

    for (bsl::size_t dataBufferIndex = 0; dataBufferIndex < numDataBuffers;
         ++dataBufferIndex)
    {
        const bdlbb::BlobBuffer& buffer =
            data.buffer(NTSCFG_WARNING_NARROW(int, dataBufferIndex));

        bsl::size_t numBytesToWrite = dataBufferIndex != numDataBuffers - 1
                                          ? buffer.size()
                                          : data.lastDataBufferLength();

        if (numBytesToWrite > numBytesLeft) {
            numBytesToWrite = numBytesLeft;
        }

        ntsa::ConstBuffer* iovec = &iovecArray[cumulativeBuffersTotal];

        iovec->setBuffer(buffer.data(), numBytesToWrite);

        cumulativeBytesTotal   += numBytesToWrite;
        cumulativeBuffersTotal += 1;

        numBytesLeft -= numBytesToWrite;

        if (cumulativeBuffersTotal >= iovecArraySize) {
            break;
        }

        if (cumulativeBytesTotal >= maxBytes) {
            break;
        }
    }

    *numBytesTotal   = cumulativeBytesTotal;
    *numBuffersTotal = cumulativeBuffersTotal;
}

void BufferUtil::gather(bsl::size_t*            numBuffersTotal,
                        bsl::size_t*            numBytesTotal,
                        ntsa::ConstBufferArray* iovecArray,
                        bsl::size_t             iovecArraySize,
                        const bdlbb::Blob&      data,
                        bsl::size_t             maxBytes)
{
    bsl::size_t cumulativeBytesTotal   = 0;
    bsl::size_t cumulativeBuffersTotal = 0;

    if (maxBytes == 0) {
        maxBytes = bsl::numeric_limits<bsl::size_t>::max();
    }

    bsl::size_t numBytesLeft = maxBytes;

    const bsl::size_t numDataBuffers = data.numDataBuffers();

    for (bsl::size_t dataBufferIndex = 0; dataBufferIndex < numDataBuffers;
         ++dataBufferIndex)
    {
        const bdlbb::BlobBuffer& buffer =
            data.buffer(NTSCFG_WARNING_NARROW(int, dataBufferIndex));

        bsl::size_t numBytesToWrite = dataBufferIndex != numDataBuffers - 1
                                          ? buffer.size()
                                          : data.lastDataBufferLength();

        if (numBytesToWrite > numBytesLeft) {
            numBytesToWrite = numBytesLeft;
        }

        iovecArray->append(buffer.data(), numBytesToWrite);

        cumulativeBytesTotal   += numBytesToWrite;
        cumulativeBuffersTotal += 1;

        numBytesLeft -= numBytesToWrite;

        if (cumulativeBuffersTotal >= iovecArraySize) {
            break;
        }

        if (cumulativeBytesTotal >= maxBytes) {
            break;
        }
    }

    *numBytesTotal   = cumulativeBytesTotal;
    *numBuffersTotal = cumulativeBuffersTotal;
}

void BufferUtil::scatter(bsl::size_t*         numBuffersTotal,
                         bsl::size_t*         numBytesTotal,
                         ntsa::MutableBuffer* iovecArray,
                         bsl::size_t          iovecArraySize,
                         bdlbb::Blob*         data,
                         bsl::size_t          maxBytes)
{
    bsl::size_t cumulativeBytesTotal   = 0;
    bsl::size_t cumulativeBuffersTotal = 0;

    const bsl::size_t numBuffers     = data->numBuffers();
    const bsl::size_t numDataBuffers = data->numDataBuffers();

    bsl::size_t bufferIndex;

    if (numDataBuffers != 0) {
        bufferIndex = numDataBuffers - 1;

        const bsl::size_t currentBufferSize =
            data->buffer(static_cast<int>(bufferIndex)).size();
        const bsl::size_t lastDataBufferSize = data->lastDataBufferLength();

        if (currentBufferSize > lastDataBufferSize) {
            bsl::size_t numBytesToRead =
                currentBufferSize - lastDataBufferSize;

            ntsa::MutableBuffer* iovec = &iovecArray[cumulativeBuffersTotal];

            iovec->setBuffer(
                data->buffer(static_cast<int>(bufferIndex)).data() +
                    lastDataBufferSize,
                numBytesToRead);

            cumulativeBytesTotal   += numBytesToRead;
            cumulativeBuffersTotal += 1;
        }

        ++bufferIndex;
    }
    else {
        bufferIndex = 0;
    }

    for (; bufferIndex < numBuffers; ++bufferIndex) {
        if (cumulativeBuffersTotal >= iovecArraySize) {
            break;
        }

        if (maxBytes > 0 && cumulativeBytesTotal >= maxBytes) {
            break;
        }

        bsl::size_t numBytesToRead =
            data->buffer(static_cast<int>(bufferIndex)).size();

        ntsa::MutableBuffer* iovec = &iovecArray[cumulativeBuffersTotal];

        iovec->setBuffer(data->buffer(static_cast<int>(bufferIndex)).data(),
                         numBytesToRead);

        cumulativeBytesTotal   += numBytesToRead;
        cumulativeBuffersTotal += 1;
    }

    *numBytesTotal   = cumulativeBytesTotal;
    *numBuffersTotal = cumulativeBuffersTotal;
}

void BufferUtil::scatter(bsl::size_t*              numBuffersTotal,
                         bsl::size_t*              numBytesTotal,
                         ntsa::MutableBufferArray* iovecArray,
                         bsl::size_t               iovecArraySize,
                         bdlbb::Blob*              data,
                         bsl::size_t               maxBytes)
{
    bsl::size_t cumulativeBytesTotal   = 0;
    bsl::size_t cumulativeBuffersTotal = 0;

    const bsl::size_t numBuffers     = data->numBuffers();
    const bsl::size_t numDataBuffers = data->numDataBuffers();

    bsl::size_t bufferIndex;

    if (numDataBuffers != 0) {
        bufferIndex = numDataBuffers - 1;

        const bsl::size_t currentBufferSize =
            data->buffer(static_cast<int>(bufferIndex)).size();
        const bsl::size_t lastDataBufferSize = data->lastDataBufferLength();

        if (currentBufferSize > lastDataBufferSize) {
            bsl::size_t numBytesToRead =
                currentBufferSize - lastDataBufferSize;

            iovecArray->append(
                data->buffer(static_cast<int>(bufferIndex)).data() +
                    lastDataBufferSize,
                numBytesToRead);

            cumulativeBytesTotal   += numBytesToRead;
            cumulativeBuffersTotal += 1;
        }

        ++bufferIndex;
    }
    else {
        bufferIndex = 0;
    }

    for (; bufferIndex < numBuffers; ++bufferIndex) {
        if (cumulativeBuffersTotal >= iovecArraySize) {
            break;
        }

        if (maxBytes > 0 && cumulativeBytesTotal >= maxBytes) {
            break;
        }

        bsl::size_t numBytesToRead =
            data->buffer(static_cast<int>(bufferIndex)).size();

        iovecArray->append(data->buffer(static_cast<int>(bufferIndex)).data(),
                           numBytesToRead);

        cumulativeBytesTotal   += numBytesToRead;
        cumulativeBuffersTotal += 1;
    }

    *numBytesTotal   = cumulativeBytesTotal;
    *numBuffersTotal = cumulativeBuffersTotal;
}

void BufferUtil::copy(bdlbb::Blob*             blob,
                      const ntsa::ConstBuffer* vecs,
                      bsl::size_t              numVecs,
                      bsl::size_t              offset)
{
    BSLS_ASSERT(numVecs > 0);

    // Set up loop invariant stated below.  Note that if blobLength is 0,
    // or if last data buffer is complete, the call 'blob->setLength(...)'
    // below will create additional buffers as needed, so that the call to
    // 'blob->buffer(currentBufIndex)' will always be legal.

    int currentBufIndex  = blob->numDataBuffers() - 1;
    int currentBufOffset = blob->lastDataBufferLength();
    if (currentBufIndex < 0 ||
        currentBufOffset == blob->buffer(currentBufIndex).size())
    {
        // Blob is empty or last data buffer is complete: skip to next buffer.
        ++currentBufIndex;
        currentBufOffset = 0;
    }

    // Skip to 'offset' in 'vecs'.

    int currentVecIndex = 0;
    int currentVecAvailable =
        NTSCFG_WARNING_NARROW(int, vecs[currentVecIndex].length());
    BSLS_ASSERT(0 < currentVecAvailable);

    int prefixLength = 0;
    while (static_cast<bsl::size_t>(prefixLength + currentVecAvailable) <=
           offset)
    {
        prefixLength += currentVecAvailable;
        ++currentVecIndex;
        currentVecAvailable =
            NTSCFG_WARNING_NARROW(int, vecs[currentVecIndex].length());
        BSLS_ASSERT(0 < currentVecAvailable);
    }
    int currentVecOffset = static_cast<int>(offset) - prefixLength;
    currentVecAvailable  -= currentVecOffset;
    BSLS_ASSERT(0 <= currentVecOffset);
    BSLS_ASSERT(0 < currentVecAvailable);

    // For simplicity,  finish computing the iovec lengths, and reserve blob's
    // length in a single setLength call.  Since prefixLength isn't used, we
    // reset it and use it for that computation.

    prefixLength = currentVecAvailable;
    for (int i = currentVecIndex + 1; i < static_cast<int>(numVecs); ++i) {
        prefixLength += NTSCFG_WARNING_NARROW(int, vecs[i].length());
    }
    blob->setLength(blob->length() + prefixLength);

    // Compute number of bytes available to read or write in current vec or
    // buffer.  This must be done *after* setting the blob length.

    int currentBufAvailable = blob->buffer(currentBufIndex).size();
    currentBufAvailable     -= currentBufOffset;
    BSLS_ASSERT(0 < currentBufAvailable);

    // Append the iovecs as individual blob buffers, re-segmented to take
    // advantage of the factory's buffer size: this is a classic merge.

    while (1) {
        // Invariants:
        // 1. 0 <= currentVec < numVecs
        // 2. 0 <= currentVecOffset < vecs[currentVec].length()
        // 3. 0 <  currentVecAvailable
        // 4. 0 <= currentBuf < blobs.numDataBuffers()
        // 5. 0 <= currentBufOffset < blob->buffer(currentBuf).size()
        // 6. 0 <  currentBufAvailable

        int numBytesCopied =
            bsl::min(currentVecAvailable, currentBufAvailable);

        bsl::memcpy(blob->buffer(currentBufIndex).data() + currentBufOffset,
                    static_cast<const char*>(vecs[currentVecIndex].buffer()) +
                        currentVecOffset,
                    numBytesCopied);

        currentBufOffset += numBytesCopied;
        currentVecOffset += numBytesCopied;

        if (currentVecAvailable == numBytesCopied) {
            currentVecOffset = 0;
            if (static_cast<bsl::size_t>(++currentVecIndex) == numVecs) {
                return;
            }
            currentVecAvailable =
                NTSCFG_WARNING_NARROW(int, vecs[currentVecIndex].length());
        }
        else {
            currentVecAvailable -= numBytesCopied;
        }

        if (currentBufAvailable == numBytesCopied) {
            currentBufOffset = 0;
            ++currentBufIndex;
            currentBufAvailable = blob->buffer(currentBufIndex).size();
        }
        else {
            currentBufAvailable -= numBytesCopied;
        }
    }
}

void BufferUtil::copy(bdlbb::Blob*               blob,
                      const ntsa::MutableBuffer* vecs,
                      bsl::size_t                numVecs,
                      bsl::size_t                offset)
{
    BufferUtil::copy(blob,
                     reinterpret_cast<const ntsa::ConstBuffer*>(vecs),
                     numVecs,
                     offset);
}

bsl::size_t BufferUtil::calculateTotalBytes(
    const ntsa::ConstBuffer* iovecArray,
    bsl::size_t              iovecArraySize)
{
    bsl::size_t numBytesTotal = 0;
    for (bsl::size_t i = 0; i < iovecArraySize; ++i) {
        numBytesTotal += iovecArray[i].length();
    }
    return numBytesTotal;
}

bsl::size_t BufferUtil::calculateTotalBytes(
    const ntsa::MutableBuffer* iovecArray,
    bsl::size_t                iovecArraySize)
{
    return BufferUtil::calculateTotalBytes(
        reinterpret_cast<const ntsa::ConstBuffer*>(iovecArray),
        iovecArraySize);
}

}  // end namespace ntsu
}  // end namespace BloombergLP
