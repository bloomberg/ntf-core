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

const char* MemoryBuffer::getReaderBegin() const
{
    return (const char*)(this->eback());
}

const char* MemoryBuffer::getReaderCurrent() const
{
    return (const char*)(this->gptr());
}

const char* MemoryBuffer::getReaderEnd() const
{
    return (const char*)(this->egptr());
}

bsl::size_t MemoryBuffer::getReaderOffset() const
{
    BSLS_ASSERT(this->gptr() >= this->eback());
    return (bsl::size_t)(this->gptr() - this->eback());
}

bsl::size_t MemoryBuffer::getReaderAvailable() const
{
    BSLS_ASSERT(this->egptr() >= this->gptr());
    return (bsl::size_t)(this->egptr() - this->gptr());
}

char* MemoryBuffer::getWriterBegin() const
{
    return (char*)(this->pbase());
}

char* MemoryBuffer::getWriterCurrent() const
{
    return (char*)(this->pptr());
}

char* MemoryBuffer::getWriterEnd() const
{
    return (char*)(this->epptr());
}

bsl::size_t MemoryBuffer::getWriterOffset() const
{
    BSLS_ASSERT(this->pptr() >= this->pbase());
    return (bsl::size_t)(this->pptr() - this->pbase());
}

bsl::size_t MemoryBuffer::getWriterAvailable() const
{
    BSLS_ASSERT(this->epptr() >= this->pptr());
    return (bsl::size_t)(this->epptr() - this->pptr());
}

void* MemoryBuffer::getMemory() const
{
    if (d_access == e_READONLY) {
        return (void*)(this->getReaderBegin());
    }
    else {
        return (void*)(this->getWriterBegin());
    }
}

bsl::size_t MemoryBuffer::getMemorySize() const
{
    if (d_access == e_READONLY) {
        return (bsl::size_t)(this->getReaderEnd() - this->getReaderBegin());
    }
    else {
        return (bsl::size_t)(this->getWriterCurrent() -
                             this->getWriterBegin());
    }
}

bsl::size_t MemoryBuffer::getMemoryCapacity() const
{
    if (d_access == e_READONLY) {
        return (bsl::size_t)(this->getReaderEnd() - this->getReaderBegin());
    }
    else {
        return (bsl::size_t)(this->getWriterEnd() - this->getWriterBegin());
    }
}

void MemoryBuffer::incrementReaderPosition(bsl::size_t size)
{
    BSLMF_ASSERT(bsl::numeric_limits<bsl::streamsize>::is_signed);

    BSLS_ASSERT(this->eback() != 0);
    BSLS_ASSERT(this->gptr() != 0);
    BSLS_ASSERT(this->gptr() >= this->eback());
    BSLS_ASSERT(this->gptr() < this->egptr());

    BSLS_ASSERT(size <= (bsl::size_t)(this->egptr() - this->gptr()));

    const bsl::size_t numToIncrementMax =
        (bsl::size_t)(bsl::numeric_limits<int>::max());

    bsl::size_t numRemaining = size;

    while (true) {
        bsl::size_t numToIncrement = numRemaining;
        if (numToIncrement > numToIncrementMax) {
            numToIncrement = numToIncrementMax;
        }

        this->gbump((int)(numToIncrement));

        numRemaining -= numToIncrement;
        if (numRemaining == 0) {
            break;
        }
    }
}

void MemoryBuffer::incrementWriterPosition(bsl::size_t size)
{
    BSLMF_ASSERT(bsl::numeric_limits<bsl::streamsize>::is_signed);

    BSLS_ASSERT(this->pbase() != 0);
    BSLS_ASSERT(this->pptr() != 0);
    BSLS_ASSERT(this->pptr() >= this->pbase());
    BSLS_ASSERT(this->pptr() < this->epptr());

    BSLS_ASSERT(size <= (bsl::size_t)(this->epptr() - this->pptr()));

    const bsl::size_t numToIncrementMax =
        (bsl::size_t)(bsl::numeric_limits<int>::max());

    bsl::size_t numRemaining = size;

    while (true) {
        bsl::size_t numToIncrement = numRemaining;
        if (numToIncrement > numToIncrementMax) {
            numToIncrement = numToIncrementMax;
        }

        this->pbump((int)(numToIncrement));

        numRemaining -= numToIncrement;
        if (numRemaining == 0) {
            break;
        }
    }
}

void MemoryBuffer::initialize()
{
    this->setp(0, 0);
    this->setg(0, 0, 0);

    BSLS_ASSERT(this->pbase() == 0);
    BSLS_ASSERT(this->pptr() == 0);
    BSLS_ASSERT(this->epptr() == 0);

    BSLS_ASSERT(this->eback() == 0);
    BSLS_ASSERT(this->gptr() == 0);
    BSLS_ASSERT(this->egptr() == 0);
}

void MemoryBuffer::initialize(const void* data, bsl::size_t size)
{
    BSLS_ASSERT(data != 0);

    this->setp(0, 0);
    this->setg((char*)(data), (char*)(data), (char*)(data) + size);

    BSLS_ASSERT(this->pbase() == 0);
    BSLS_ASSERT(this->pptr() == 0);
    BSLS_ASSERT(this->epptr() == 0);

    BSLS_ASSERT(this->gptr() == (char*)(data));
    BSLS_ASSERT(this->eback() == (char*)(data));
    BSLS_ASSERT(this->egptr() == (char*)(data) + size);
}

void MemoryBuffer::initialize(void*       data,
                              bsl::size_t size,
                              bsl::size_t capacity)
{
    BSLS_ASSERT(data != 0);
    BSLS_ASSERT(size <= capacity);

    this->setp((char*)(data), (char*)(data) + capacity);
    this->setg((char*)(data), (char*)(data), (char*)(data) + size);

    BSLS_ASSERT(this->pbase() == (char*)(data));
    BSLS_ASSERT(this->pptr() == (char*)(data));
    BSLS_ASSERT(this->epptr() == (char*)(data) + capacity);

    BSLS_ASSERT(this->gptr() == (char*)(data));
    BSLS_ASSERT(this->eback() == (char*)(data));
    BSLS_ASSERT(this->egptr() == (char*)(data) + size);
}

void MemoryBuffer::expand(bsl::size_t overflow)
{
    BSLS_ASSERT(d_access == e_WRITABLE);

    void*             currentMemory         = this->getMemory();
    const bsl::size_t currentMemoryCapacity = this->getMemoryCapacity();

    const bsl::size_t currentReaderOffset = this->getReaderOffset();
    const bsl::size_t currentWriterOffset = this->getWriterOffset();

    bsl::size_t newMemoryCapacityMin = currentMemoryCapacity * 2;
    if (newMemoryCapacityMin < e_MIN_CAPACITY) {
        newMemoryCapacityMin = e_MIN_CAPACITY;
    }

    bsl::size_t newMemoryCapacity = currentMemoryCapacity + overflow;
    if (newMemoryCapacity < newMemoryCapacityMin) {
        newMemoryCapacity = newMemoryCapacityMin;
    }

    BSLS_ASSERT(d_allocator_p != 0);

    void* newMemory = d_allocator_p->allocate(newMemoryCapacity);

    if (currentMemory != 0) {
        if (currentWriterOffset > 0) {
            bsl::memcpy(newMemory, currentMemory, currentWriterOffset);
        }

        if (d_ownership == e_INTERNAL) {
            d_allocator_p->deallocate(currentMemory);
        }
    }

    this->initialize(newMemory, currentWriterOffset, newMemoryCapacity);
    d_ownership = e_INTERNAL;

    if (currentReaderOffset > 0) {
        this->incrementReaderPosition(currentReaderOffset);
    }

    if (currentWriterOffset > 0) {
        this->incrementWriterPosition(currentWriterOffset);
    }
}

bsl::size_t MemoryBuffer::store(const char* source, bsl::size_t size)
{
    BSLS_ASSERT(source != 0);
    BSLS_ASSERT(size > 0);

    BSLS_ASSERT(d_access == e_WRITABLE);

    const bsl::size_t numAvailable = this->getWriterAvailable();

    if (size > numAvailable) {
        this->expand(size);
    }

    bsl::memcpy(this->getWriterCurrent(), source, size);
    this->incrementWriterPosition(size);

    return size;
}

bsl::size_t MemoryBuffer::fetch(char* destination, bsl::size_t size)
{
    BSLS_ASSERT(destination != 0);
    BSLS_ASSERT(size > 0);

    const bsl::size_t numToCopyMax = this->getReaderAvailable();

    bsl::size_t numToCopy = (bsl::size_t)(size);
    if (numToCopy > numToCopyMax) {
        numToCopy = numToCopyMax;
    }

    if (numToCopy > 0) {
        bsl::memcpy(destination, this->getReaderCurrent(), numToCopy);
        this->incrementReaderPosition(numToCopy);
    }

    return (bsl::streamsize)(numToCopy);
}

MemoryBuffer::int_type MemoryBuffer::overflow(int_type meta)
{
    if (d_access == e_READONLY) {
        return bsl::streambuf::traits_type::eof();
    }

    if (traits_type::eof() == meta) {
        return traits_type::not_eof(meta);
    }

    this->expand(1);

    return this->sputc((char_type)(meta));
}

bsl::streamsize MemoryBuffer::showmanyc()
{
    bsl::streamsize result = (bsl::streamsize)(this->getReaderAvailable());
    if (0 == result) {
        return -1;
    }

    return result;
}

bsl::streamsize MemoryBuffer::xsgetn(char_type*      destination,
                                     bsl::streamsize size)
{
    BSLS_ASSERT(destination != 0);
    BSLS_ASSERT(size > 0);

    return (bsl::streamsize)(this->fetch(destination, (bsl::size_t)(size)));
}

bsl::streamsize MemoryBuffer::xsputn(const char_type* source,
                                     bsl::streamsize  size)
{
    BSLS_ASSERT(source != 0);
    BSLS_ASSERT(size > 0);

    if (d_access == e_READONLY) {
        return 0;
    }

    return (bsl::streamsize)(this->store(source, (bsl::size_t)(size)));
}

MemoryBuffer::pos_type MemoryBuffer::seekoff(off_type                offset,
                                             bsl::ios_base::seekdir  way,
                                             bsl::ios_base::openmode mode)
{
    if ((bsl::ios_base::out & mode) != 0) {
        if (d_access == e_READONLY) {
            return (pos_type)(-1);
        }
    }

    bsl::streambuf::off_type getOffset = 0;
    bsl::streambuf::off_type putOffset = 0;

    if (way == bsl::ios_base::beg) {
        getOffset = offset;
        putOffset = offset;
    }
    else if (way == bsl::ios_base::cur) {
        if (this->gptr() == 0) {
            getOffset = 0;
        }
        else {
            getOffset = (this->gptr() - this->eback()) + offset;
        }

        if (this->pptr() == 0) {
            putOffset = 0;
        }
        else {
            putOffset = (this->pptr() - this->pbase()) + offset;
        }
    }
    else if (way == bsl::ios_base::end) {
        if (this->gptr() == 0) {
            getOffset = 0;
        }
        else {
            getOffset = (off_type)(this->egptr() + offset);
        }

        if (this->pptr() == 0) {
            putOffset = 0;
        }
        else {
            putOffset = (off_type)(this->epptr() + offset);
        }
    }

    char*       data     = this->data();
    bsl::size_t size     = this->size();
    bsl::size_t capacity = this->capacity();

    if ((bsl::ios_base::in & mode) != 0) {
        if (getOffset < 0 || static_cast<bsl::size_t>(getOffset) > size) {
            return (pos_type)(-1);
        }
        else {
            this->setg(data, data + getOffset, data + size);
        }
    }

    if ((bsl::ios_base::out & mode) != 0) {
        if (putOffset < 0 || static_cast<bsl::size_t>(putOffset) > size) {
            return (pos_type)(-1);
        }
        else {
            this->setp(data, data + capacity);
            this->pbump((int)(putOffset));
        }
    }

    return (pos_type)
        (((bsl::ios_base::in & mode) != 0) ? getOffset : putOffset);
}

MemoryBuffer::pos_type MemoryBuffer::seekpos(pos_type                position,
                                             bsl::ios_base::openmode mode)
{
    return this->seekoff((off_type)(position), bsl::ios_base::beg, mode);
}

int MemoryBuffer::sync()
{
    if (d_access == e_READONLY) {
        return 0;
    }

    void*       writerMemory = this->getMemory();
    bsl::size_t writerSize   = this->getMemorySize();
    bsl::size_t readerOffset = this->getReaderOffset();

    BSLS_ASSERT(readerOffset <= writerSize);

    this->setg((char*)(writerMemory),
               (char*)(writerMemory) + readerOffset,
               (char*)(writerMemory) + writerSize);

    return 0;
}

MemoryBuffer::MemoryBuffer(bslma::Allocator* basicAllocator)
: d_access(e_WRITABLE)
, d_ownership(e_INTERNAL)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->initialize();
}

MemoryBuffer::MemoryBuffer(const void*       data,
                           bsl::size_t       size,
                           bslma::Allocator* basicAllocator)
: d_access(e_READONLY)
, d_ownership(e_EXTERNAL)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (data == 0 || size == 0) {
        this->initialize();
    }
    else {
        this->initialize(data, size);
    }
}

MemoryBuffer::MemoryBuffer(void*             data,
                           bsl::size_t       size,
                           bsl::size_t       capacity,
                           bslma::Allocator* basicAllocator)
: d_access(e_WRITABLE)
, d_ownership(e_EXTERNAL)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (data == 0 || capacity == 0) {
        this->initialize();
    }
    else {
        this->initialize(data, size, capacity);
        this->incrementWriterPosition(size);
    }
}

MemoryBuffer::~MemoryBuffer()
{
    void* currentMemory = this->getMemory();
    if (currentMemory != 0) {
        if (d_ownership == e_INTERNAL) {
            BSLS_ASSERT(d_allocator_p != 0);
            d_allocator_p->deallocate(currentMemory);
        }
    }
}

void MemoryBuffer::reset()
{
    void* currentMemory = this->getMemory();
    if (currentMemory != 0) {
        if (d_ownership == e_INTERNAL) {
            BSLS_ASSERT(d_allocator_p != 0);
            d_allocator_p->deallocate(currentMemory);
        }
    }

    this->initialize();

    d_access    = e_WRITABLE;
    d_ownership = e_INTERNAL;
}

void MemoryBuffer::reserve(bsl::size_t capacity)
{
    const bsl::size_t currentCapacity = this->getMemoryCapacity();

    if (capacity <= currentCapacity) {
        return;
    }

    const bsl::size_t overflow = (bsl::size_t)(capacity - currentCapacity);

    this->expand(overflow);
}

char* MemoryBuffer::data()
{
    return (char*)(this->getMemory());
}

const char* MemoryBuffer::data() const
{
    return (const char*)(this->getMemory());
}

bsl::size_t MemoryBuffer::size() const
{
    return this->getMemorySize();
}

bsl::size_t MemoryBuffer::capacity() const
{
    return this->getMemoryCapacity();
}

}  // close package namespace
}  // close enterprise namespace
