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

#include <ntsa_buffer.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_buffer_cpp, "$Id$ $CSID$")

#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_limits.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <winsock2.h>
#endif

// #define NTSA_BUFFER_OFFSETOF(Type, member)
//     ((bsl::size_t)((char *)(&((Type*)(0))->member) - (char*)(0)))

#if defined(BSLS_PLATFORM_CMP_IBM)
#define NTSA_BUFFER_OFFSETOF(Type, member) offsetof(Type, member)
#else
#define NTSA_BUFFER_OFFSETOF(Type, member) offsetof(Type, member)
#endif

namespace BloombergLP {
namespace ntsa {

bool MutableBuffer::validate() const
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    struct ::iovec systemObject;

    BSLMF_ASSERT(sizeof(d_data) == sizeof(systemObject.iov_base));
    BSLMF_ASSERT(sizeof(d_size) == sizeof(systemObject.iov_len));

    BSLMF_ASSERT(NTSA_BUFFER_OFFSETOF(MutableBuffer, d_data) ==
                 NTSA_BUFFER_OFFSETOF(struct ::iovec, iov_base));

    BSLMF_ASSERT(NTSA_BUFFER_OFFSETOF(MutableBuffer, d_size) ==
                 NTSA_BUFFER_OFFSETOF(struct ::iovec, iov_len));

    BSLMF_ASSERT(sizeof(MutableBuffer) == sizeof(struct ::iovec));

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    WSABUF systemObject;

    BSLMF_ASSERT(sizeof(d_data) == sizeof(systemObject.buf));
    BSLMF_ASSERT(sizeof(d_size) == sizeof(systemObject.len));

    BSLMF_ASSERT(NTSA_BUFFER_OFFSETOF(MutableBuffer, d_data) ==
                 NTSA_BUFFER_OFFSETOF(WSABUF, buf));

    BSLMF_ASSERT(NTSA_BUFFER_OFFSETOF(MutableBuffer, d_size) ==
                 NTSA_BUFFER_OFFSETOF(WSABUF, len));

    BSLMF_ASSERT(sizeof(MutableBuffer) == sizeof(WSABUF));

#else
#error Not implemented
#endif

    return true;
}

void MutableBuffer::scatter(bsl::size_t*         numBuffersTotal,
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

void MutableBuffer::copy(bdlbb::Blob*               blob,
                         const ntsa::MutableBuffer* bufferArray,
                         bsl::size_t                bufferCount,
                         bsl::size_t                offset)
{
    ConstBuffer::copy(blob,
                      reinterpret_cast<const ntsa::ConstBuffer*>(bufferArray),
                      bufferCount,
                      offset);
}

bool ConstBuffer::validate() const
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    struct ::iovec systemObject;

    BSLMF_ASSERT(sizeof(d_data) == sizeof(systemObject.iov_base));
    BSLMF_ASSERT(sizeof(d_size) == sizeof(systemObject.iov_len));

    BSLMF_ASSERT(NTSA_BUFFER_OFFSETOF(ConstBuffer, d_data) ==
                 NTSA_BUFFER_OFFSETOF(struct ::iovec, iov_base));

    BSLMF_ASSERT(NTSA_BUFFER_OFFSETOF(ConstBuffer, d_size) ==
                 NTSA_BUFFER_OFFSETOF(struct ::iovec, iov_len));

    BSLMF_ASSERT(sizeof(ConstBuffer) == sizeof(struct ::iovec));

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    WSABUF systemObject;

    BSLMF_ASSERT(sizeof(d_data) == sizeof(systemObject.buf));
    BSLMF_ASSERT(sizeof(d_size) == sizeof(systemObject.len));

    BSLMF_ASSERT(NTSA_BUFFER_OFFSETOF(ConstBuffer, d_data) ==
                 NTSA_BUFFER_OFFSETOF(WSABUF, buf));

    BSLMF_ASSERT(NTSA_BUFFER_OFFSETOF(ConstBuffer, d_size) ==
                 NTSA_BUFFER_OFFSETOF(WSABUF, len));

    BSLMF_ASSERT(sizeof(ConstBuffer) == sizeof(WSABUF));

#else
#error Not implemented
#endif

    return true;
}

void ConstBuffer::gather(bsl::size_t*       numBuffersTotal,
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

void ConstBuffer::copy(bdlbb::Blob*             blob,
                       const ntsa::ConstBuffer* bufferArray,
                       bsl::size_t              bufferCount,
                       bsl::size_t              offset)
{
    BSLS_ASSERT(bufferCount > 0);

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

    // Skip to 'offset' in 'bufferArray'.

    int currentVecIndex = 0;
    int currentVecAvailable =
        NTSCFG_WARNING_NARROW(int, bufferArray[currentVecIndex].length());
    BSLS_ASSERT(0 < currentVecAvailable);

    int prefixLength = 0;
    while (static_cast<bsl::size_t>(prefixLength + currentVecAvailable) <=
           offset)
    {
        prefixLength += currentVecAvailable;
        ++currentVecIndex;
        currentVecAvailable =
            NTSCFG_WARNING_NARROW(int, bufferArray[currentVecIndex].length());
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
    for (int i = currentVecIndex + 1; i < static_cast<int>(bufferCount); ++i) {
        prefixLength += NTSCFG_WARNING_NARROW(int, bufferArray[i].length());
    }

    blob->setLength(blob->length() + prefixLength);

    // Compute number of bytes available to read or write in current vec or
    // buffer.  This must be done *after* setting the blob length.

    int currentBufAvailable = blob->buffer(currentBufIndex).size();
    currentBufAvailable     -= currentBufOffset;
    BSLS_ASSERT(0 < currentBufAvailable);

    // Append the source buffers as individual blob buffers, re-segmented to
    // take advantage of the factory's buffer size: this is a classic merge.

    while (true) {
        // Invariants:
        // 1. 0 <= currentVec < bufferCount
        // 2. 0 <= currentVecOffset < bufferArray[currentVec].length()
        // 3. 0 <  currentVecAvailable
        // 4. 0 <= currentBuf < blobs.numDataBuffers()
        // 5. 0 <= currentBufOffset < blob->buffer(currentBuf).size()
        // 6. 0 <  currentBufAvailable

        int numBytesCopied =
            bsl::min(currentVecAvailable, currentBufAvailable);

        bsl::memcpy(blob->buffer(currentBufIndex).data() + currentBufOffset,
                    static_cast<const char*>(
                        bufferArray[currentVecIndex].buffer()) +
                        currentVecOffset,
                    numBytesCopied);

        currentBufOffset += numBytesCopied;
        currentVecOffset += numBytesCopied;

        if (currentVecAvailable == numBytesCopied) {
            currentVecOffset = 0;
            if (static_cast<bsl::size_t>(++currentVecIndex) == bufferCount) {
                return;
            }
            currentVecAvailable =
                NTSCFG_WARNING_NARROW(
                    int, bufferArray[currentVecIndex].length());
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

void MutableBufferArray::scatter(bsl::size_t*              numBuffersTotal,
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

void ConstBufferArray::gather(bsl::size_t*            numBuffersTotal,
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

}  // close package namespace
}  // close enterprise namespace
