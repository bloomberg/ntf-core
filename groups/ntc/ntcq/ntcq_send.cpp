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

#include <ntcq_send.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcq_send_cpp, "$Id$ $CSID$")

#include <ntccfg_bind.h>
#include <ntccfg_limits.h>
#include <ntsu_socketutil.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace ntcq {

bool SendQueueEntry::batchNext(ntsa::ConstBufferArray*  result,
                               const ntsa::SendOptions& options) const
{
    if (NTCCFG_UNLIKELY(!this->isBatchable())) {
        return false;
    }

    if (NTCCFG_LIKELY(d_data_sp->isBlob())) {
        return this->batchNext(result, d_data_sp->blob(), options);
    }
    else if (d_data_sp->isSharedBlob()) {
        if (d_data_sp->sharedBlob()) {
            return this->batchNext(result, *d_data_sp->sharedBlob(), options);
        }
    }
    else if (d_data_sp->isBlobBuffer()) {
        return this->batchNext(result, d_data_sp->blobBuffer(), options);
    }
    else if (d_data_sp->isConstBuffer()) {
        return this->batchNext(result, d_data_sp->constBuffer(), options);
    }
    else if (d_data_sp->isConstBufferArray()) {
        return this->batchNext(result, d_data_sp->constBufferArray(), options);
    }
    else if (d_data_sp->isConstBufferPtrArray()) {
        return this->batchNext(result,
                               d_data_sp->constBufferPtrArray(),
                               options);
    }
    else if (d_data_sp->isMutableBuffer()) {
        return this->batchNext(result, d_data_sp->mutableBuffer(), options);
    }
    else if (d_data_sp->isMutableBufferArray()) {
        return this->batchNext(result,
                               d_data_sp->mutableBufferArray(),
                               options);
    }
    else if (d_data_sp->isMutableBufferPtrArray()) {
        return this->batchNext(result,
                               d_data_sp->mutableBufferPtrArray(),
                               options);
    }
    else if (d_data_sp->isString()) {
        return this->batchNext(result, d_data_sp->string(), options);
    }

    return false;
}

bool SendQueueEntry::batchNext(ntsa::ConstBufferArray*  result,
                               const bdlbb::Blob&       blob,
                               const ntsa::SendOptions& options) const
{
    const bsl::size_t maxBytes = options.maxBytes();

    bsl::size_t maxBuffers = options.maxBuffers();
    if (NTCCFG_UNLIKELY(maxBuffers == 0)) {
        maxBuffers = ntsu::SocketUtil::maxBuffersPerSend();
    }

    const int numDataBuffers = blob.numDataBuffers();

    for (int i = 0; i < numDataBuffers; ++i) {
        const bdlbb::BlobBuffer& blobBuffer = blob.buffer(i);

        bsl::size_t size;
        if (NTCCFG_LIKELY(i != numDataBuffers - 1)) {
            size = static_cast<bsl::size_t>(blobBuffer.size());
        }
        else {
            size = static_cast<bsl::size_t>(blob.lastDataBufferLength());
        }

        const char* data = blobBuffer.data();

        if (NTCCFG_LIKELY(maxBytes > 0)) {
            if (NTCCFG_UNLIKELY(result->numBytes() >= maxBytes)) {
                return false;
            }
        }

        if (NTCCFG_LIKELY(maxBuffers > 0)) {
            if (NTCCFG_UNLIKELY(result->numBuffers() >= maxBuffers)) {
                return false;
            }
        }

        result->append(data, size);
    }

    return true;
}

bool SendQueueEntry::batchNext(ntsa::ConstBufferArray*  result,
                               const bdlbb::BlobBuffer& blobBuffer,
                               const ntsa::SendOptions& options) const
{
    return this->batchNext(
        result,
        ntsa::ConstBuffer(blobBuffer.data(),
                          static_cast<bsl::size_t>(blobBuffer.size())),
        options);
}

bool SendQueueEntry::batchNext(ntsa::ConstBufferArray*  result,
                               const ntsa::ConstBuffer& constBuffer,
                               const ntsa::SendOptions& options) const
{
    const bsl::size_t maxBytes = options.maxBytes();

    if (maxBytes > 0) {
        if (result->numBytes() >= maxBytes) {
            return false;
        }
    }

    bsl::size_t maxBuffers = options.maxBuffers();
    if (maxBuffers == 0) {
        maxBuffers = ntsu::SocketUtil::maxBuffersPerSend();
    }

    if (maxBuffers > 0) {
        if (result->numBuffers() >= maxBuffers) {
            return false;
        }
    }

    result->append(constBuffer);
    return true;
}

bool SendQueueEntry::batchNext(ntsa::ConstBufferArray*       result,
                               const ntsa::ConstBufferArray& constBufferArray,
                               const ntsa::SendOptions&      options) const
{
    for (bsl::size_t i = 0; i < constBufferArray.numBuffers(); ++i) {
        if (!this->batchNext(result, constBufferArray.buffer(i), options)) {
            return false;
        }
    }

    return true;
}

bool SendQueueEntry::batchNext(
    ntsa::ConstBufferArray*          result,
    const ntsa::ConstBufferPtrArray& constBufferPtrArray,
    const ntsa::SendOptions&         options) const
{
    for (bsl::size_t i = 0; i < constBufferPtrArray.numBuffers(); ++i) {
        if (!this->batchNext(result, constBufferPtrArray.buffer(i), options)) {
            return false;
        }
    }

    return true;
}

bool SendQueueEntry::batchNext(ntsa::ConstBufferArray*    result,
                               const ntsa::MutableBuffer& mutableBuffer,
                               const ntsa::SendOptions&   options) const
{
    const bsl::size_t maxBytes = options.maxBytes();

    if (maxBytes > 0) {
        if (result->numBytes() >= maxBytes) {
            return false;
        }
    }

    bsl::size_t maxBuffers = options.maxBuffers();
    if (maxBuffers == 0) {
        maxBuffers = ntsu::SocketUtil::maxBuffersPerSend();
    }

    if (maxBuffers > 0) {
        if (result->numBuffers() >= maxBuffers) {
            return false;
        }
    }

    result->append(mutableBuffer);
    return true;
}

bool SendQueueEntry::batchNext(
    ntsa::ConstBufferArray*         result,
    const ntsa::MutableBufferArray& mutableBufferArray,
    const ntsa::SendOptions&        options) const
{
    for (bsl::size_t i = 0; i < mutableBufferArray.numBuffers(); ++i) {
        if (!this->batchNext(result, mutableBufferArray.buffer(i), options)) {
            return false;
        }
    }

    return true;
}

bool SendQueueEntry::batchNext(
    ntsa::ConstBufferArray*            result,
    const ntsa::MutableBufferPtrArray& mutableBufferPtrArray,
    const ntsa::SendOptions&           options) const
{
    for (bsl::size_t i = 0; i < mutableBufferPtrArray.numBuffers(); ++i) {
        if (!this->batchNext(result, mutableBufferPtrArray.buffer(i), options))
        {
            return false;
        }
    }

    return true;
}

bool SendQueueEntry::batchNext(ntsa::ConstBufferArray*  result,
                               const bsl::string&       string,
                               const ntsa::SendOptions& options) const
{
    return this->batchNext(result,
                           ntsa::ConstBuffer(string.data(), string.size()),
                           options);
}

SendQueue::SendQueue(bslma::Allocator* basicAllocator)
: d_entryList(basicAllocator)
, d_data_sp()
, d_size(0)
, d_watermarkLow(NTCCFG_DEFAULT_STREAM_SOCKET_WRITE_QUEUE_LOW_WATERMARK)
, d_watermarkLowWanted(false)
, d_watermarkHigh(NTCCFG_DEFAULT_STREAM_SOCKET_WRITE_QUEUE_HIGH_WATERMARK)
, d_watermarkHighWanted(true)
, d_nextEntryId(1)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntcs::WatermarkUtil::sanitizeOutgoingQueueWatermarks(&d_watermarkLow,
                                                         &d_watermarkHigh);
}

SendQueue::~SendQueue()
{
}

bool SendQueue::batchNext(ntsa::ConstBufferArray*  result,
                          const ntsa::SendOptions& options) const
{
    result->clear();

    if (d_entryList.size() < 2) {
        return false;
    }

    ntsa::SendOptions effectiveOptions = options;
    if (effectiveOptions.maxBuffers() == 0) {
        effectiveOptions.setMaxBuffers(ntsu::SocketUtil::maxBuffersPerSend());
    }

    EntryList::const_iterator current = d_entryList.begin();
    EntryList::const_iterator end     = d_entryList.end();

    while (true) {
        if (current == end) {
            break;
        }

        const SendQueueEntry& entry = *current;
        if (!entry.batchNext(result, effectiveOptions)) {
            break;
        }

        ++current;
    }

    if (result->numBuffers() == 0) {
        return false;
    }

    return true;
}

}  // close package namespace
}  // close enterprise namespace
