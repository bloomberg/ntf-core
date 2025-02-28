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

#include <ntci_compression.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_compression_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntci {

ntsa::Error Compression::deflateRep(ntca::DeflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const bdlbb::Blob&          data,
                                    const ntca::DeflateOptions& options)
{
    ntsa::Error error;

    const int numDataBuffers = data.numDataBuffers();

    for (int i = 0; i < numDataBuffers; ++i) {
        const bdlbb::BlobBuffer& buffer = data.buffer(i);

        const bsl::uint8_t* bufferData =
            reinterpret_cast<bsl::uint8_t*>(buffer.data());

        bsl::size_t bufferSize;
        if (i != numDataBuffers - 1) {
            bufferSize = static_cast<bsl::size_t>(buffer.size());
        }
        else {
            bufferSize = static_cast<bsl::size_t>(data.lastDataBufferLength());
        }

        error = this->deflateNext(context,
                                  result,
                                  bufferData,
                                  bufferSize,
                                  options);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Compression::deflateRep(ntca::DeflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const bdlbb::BlobBuffer&    data,
                                    const ntca::DeflateOptions& options)
{
    ntsa::Error error;

    error = this->deflateNext(context,
                              result,
                              reinterpret_cast<bsl::uint8_t*>(data.data()),
                              static_cast<bsl::size_t>(data.size()),
                              options);

    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Compression::deflateRep(ntca::DeflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const ntsa::ConstBuffer&    data,
                                    const ntca::DeflateOptions& options)
{
    ntsa::Error error;

    error =
        this->deflateNext(context,
                          result,
                          reinterpret_cast<const bsl::uint8_t*>(data.data()),
                          data.size(),
                          options);

    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Compression::deflateRep(ntca::DeflateContext*         context,
                                    bdlbb::Blob*                  result,
                                    const ntsa::ConstBufferArray& data,
                                    const ntca::DeflateOptions&   options)
{
    ntsa::Error error;

    const bsl::size_t numDataBuffers = data.numBuffers();

    for (bsl::size_t i = 0; i < numDataBuffers; ++i) {
        const ntsa::ConstBuffer& constBuffer = data.buffer(i);

        error = this->deflateNext(
            context,
            result,
            reinterpret_cast<const bsl::uint8_t*>(constBuffer.data()),
            constBuffer.size(),
            options);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Compression::deflateRep(ntca::DeflateContext*            context,
                                    bdlbb::Blob*                     result,
                                    const ntsa::ConstBufferPtrArray& data,
                                    const ntca::DeflateOptions&      options)
{
    ntsa::Error error;

    const bsl::size_t numDataBuffers = data.numBuffers();

    for (bsl::size_t i = 0; i < numDataBuffers; ++i) {
        const ntsa::ConstBuffer& constBuffer = data.buffer(i);

        error = this->deflateNext(
            context,
            result,
            reinterpret_cast<const bsl::uint8_t*>(constBuffer.data()),
            constBuffer.size(),
            options);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Compression::deflateRep(ntca::DeflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const ntsa::MutableBuffer&  data,
                                    const ntca::DeflateOptions& options)
{
    ntsa::Error error;

    error =
        this->deflateNext(context,
                          result,
                          reinterpret_cast<const bsl::uint8_t*>(data.data()),
                          data.size(),
                          options);

    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Compression::deflateRep(ntca::DeflateContext*           context,
                                    bdlbb::Blob*                    result,
                                    const ntsa::MutableBufferArray& data,
                                    const ntca::DeflateOptions&     options)
{
    ntsa::Error error;

    const bsl::size_t numDataBuffers = data.numBuffers();

    for (bsl::size_t i = 0; i < numDataBuffers; ++i) {
        const ntsa::ConstBuffer& constBuffer = data.buffer(i);

        error = this->deflateNext(
            context,
            result,
            reinterpret_cast<const bsl::uint8_t*>(constBuffer.data()),
            constBuffer.size(),
            options);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Compression::deflateRep(ntca::DeflateContext*              context,
                                    bdlbb::Blob*                       result,
                                    const ntsa::MutableBufferPtrArray& data,
                                    const ntca::DeflateOptions&        options)
{
    ntsa::Error error;

    const bsl::size_t numDataBuffers = data.numBuffers();

    for (bsl::size_t i = 0; i < numDataBuffers; ++i) {
        const ntsa::ConstBuffer& constBuffer = data.buffer(i);

        error = this->deflateNext(
            context,
            result,
            reinterpret_cast<const bsl::uint8_t*>(constBuffer.data()),
            constBuffer.size(),
            options);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Compression::deflateRep(ntca::DeflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const bsl::string&          data,
                                    const ntca::DeflateOptions& options)
{
    ntsa::Error error;

    error =
        this->deflateNext(context,
                          result,
                          reinterpret_cast<const bsl::uint8_t*>(data.data()),
                          data.size(),
                          options);

    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Compression::deflateRep(ntca::DeflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const ntsa::File&           data,
                                    const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(data);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error Compression::inflateRep(ntca::InflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const bdlbb::Blob&          data,
                                    const ntca::InflateOptions& options)
{
    return this->inflateNext(context, result, data, options);
}

ntsa::Error Compression::inflateRep(ntca::InflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const bdlbb::BlobBuffer&    data,
                                    const ntca::InflateOptions& options)
{
    ntsa::Error error;

    error = this->inflateNext(context,
                              result,
                              reinterpret_cast<bsl::uint8_t*>(data.data()),
                              static_cast<bsl::size_t>(data.size()),
                              options);

    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Compression::inflateRep(ntca::InflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const ntsa::ConstBuffer&    data,
                                    const ntca::InflateOptions& options)
{
    ntsa::Error error;

    error =
        this->inflateNext(context,
                          result,
                          reinterpret_cast<const bsl::uint8_t*>(data.data()),
                          data.size(),
                          options);

    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Compression::inflateRep(ntca::InflateContext*         context,
                                    bdlbb::Blob*                  result,
                                    const ntsa::ConstBufferArray& data,
                                    const ntca::InflateOptions&   options)
{
    ntsa::Error error;

    const bsl::size_t numDataBuffers = data.numBuffers();

    for (bsl::size_t i = 0; i < numDataBuffers; ++i) {
        const ntsa::ConstBuffer& constBuffer = data.buffer(i);

        error = this->inflateNext(
            context,
            result,
            reinterpret_cast<const bsl::uint8_t*>(constBuffer.data()),
            constBuffer.size(),
            options);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Compression::inflateRep(ntca::InflateContext*            context,
                                    bdlbb::Blob*                     result,
                                    const ntsa::ConstBufferPtrArray& data,
                                    const ntca::InflateOptions&      options)
{
    ntsa::Error error;

    const bsl::size_t numDataBuffers = data.numBuffers();

    for (bsl::size_t i = 0; i < numDataBuffers; ++i) {
        const ntsa::ConstBuffer& constBuffer = data.buffer(i);

        error = this->inflateNext(
            context,
            result,
            reinterpret_cast<const bsl::uint8_t*>(constBuffer.data()),
            constBuffer.size(),
            options);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Compression::inflateRep(ntca::InflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const ntsa::MutableBuffer&  data,
                                    const ntca::InflateOptions& options)
{
    ntsa::Error error;

    error =
        this->inflateNext(context,
                          result,
                          reinterpret_cast<const bsl::uint8_t*>(data.data()),
                          data.size(),
                          options);

    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Compression::inflateRep(ntca::InflateContext*           context,
                                    bdlbb::Blob*                    result,
                                    const ntsa::MutableBufferArray& data,
                                    const ntca::InflateOptions&     options)
{
    ntsa::Error error;

    const bsl::size_t numDataBuffers = data.numBuffers();

    for (bsl::size_t i = 0; i < numDataBuffers; ++i) {
        const ntsa::ConstBuffer& constBuffer = data.buffer(i);

        error = this->inflateNext(
            context,
            result,
            reinterpret_cast<const bsl::uint8_t*>(constBuffer.data()),
            constBuffer.size(),
            options);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Compression::inflateRep(ntca::InflateContext*              context,
                                    bdlbb::Blob*                       result,
                                    const ntsa::MutableBufferPtrArray& data,
                                    const ntca::InflateOptions&        options)
{
    ntsa::Error error;

    const bsl::size_t numDataBuffers = data.numBuffers();

    for (bsl::size_t i = 0; i < numDataBuffers; ++i) {
        const ntsa::ConstBuffer& constBuffer = data.buffer(i);

        error = this->inflateNext(
            context,
            result,
            reinterpret_cast<const bsl::uint8_t*>(constBuffer.data()),
            constBuffer.size(),
            options);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Compression::inflateRep(ntca::InflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const bsl::string&          data,
                                    const ntca::InflateOptions& options)
{
    ntsa::Error error;

    error =
        this->inflateNext(context,
                          result,
                          reinterpret_cast<const bsl::uint8_t*>(data.data()),
                          data.size(),
                          options);

    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Compression::inflateRep(ntca::InflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const ntsa::File&           data,
                                    const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(data);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error Compression::deflateBegin(ntca::DeflateContext*       context,
                                      bdlbb::Blob*                result,
                                      const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error();
}

ntsa::Error Compression::deflateNext(ntca::DeflateContext*       context,
                                     bdlbb::Blob*                result,
                                     const bsl::uint8_t*         data,
                                     bsl::size_t                 size,
                                     const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(data);
    NTCCFG_WARNING_UNUSED(size);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Compression::deflateEnd(ntca::DeflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error();
}

ntsa::Error Compression::inflateBegin(ntca::InflateContext*       context,
                                      bdlbb::Blob*                result,
                                      const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error();
}

ntsa::Error Compression::inflateNext(ntca::InflateContext*       context,
                                     bdlbb::Blob*                result,
                                     const bsl::uint8_t*         data,
                                     bsl::size_t                 size,
                                     const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(data);
    NTCCFG_WARNING_UNUSED(size);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Compression::inflateNext(ntca::InflateContext*       context,
                                     bdlbb::Blob*                result,
                                     const bdlbb::Blob&          data,
                                     const ntca::InflateOptions& options)
{
    ntsa::Error error;

    const int numDataBuffers = data.numDataBuffers();

    for (int i = 0; i < numDataBuffers; ++i) {
        const bdlbb::BlobBuffer& buffer = data.buffer(i);

        const bsl::uint8_t* bufferData =
            reinterpret_cast<bsl::uint8_t*>(buffer.data());

        bsl::size_t bufferSize;
        if (i != numDataBuffers - 1) {
            bufferSize = static_cast<bsl::size_t>(buffer.size());
        }
        else {
            bufferSize = static_cast<bsl::size_t>(data.lastDataBufferLength());
        }

        error = this->inflateNext(context,
                                  result,
                                  bufferData,
                                  bufferSize,
                                  options);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Compression::inflateEnd(ntca::InflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error();
}

Compression::~Compression()
{
}

ntsa::Error Compression::deflate(ntca::DeflateContext*       context,
                                 bdlbb::Blob*                result,
                                 const bdlbb::Blob&          data,
                                 const ntca::DeflateOptions& options)
{
    ntsa::Error error;

    context->reset();

    error = this->deflateBegin(context, result, options);
    if (error) {
        return error;
    }

    error = this->deflateRep(context, result, data, options);
    if (error) {
        return error;
    }

    error = this->deflateEnd(context, result, options);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Compression::deflate(ntca::DeflateContext*       context,
                                 bdlbb::Blob*                result,
                                 const ntsa::Data&           data,
                                 const ntca::DeflateOptions& options)
{
    ntsa::Error error;

    context->reset();

    error = this->deflateBegin(context, result, options);
    if (error) {
        return error;
    }

    if (data.isBlob()) {
        error = this->deflateRep(context, result, data.blob(), options);
    }
    else if (data.isSharedBlob() && data.sharedBlob().get() != 0) {
        error = this->deflateRep(context, result, *data.sharedBlob(), options);
    }
    else if (data.isBlobBuffer()) {
        error = this->deflateRep(context, result, data.blobBuffer(), options);
    }
    else if (data.isConstBuffer()) {
        error = this->deflateRep(context, result, data.constBuffer(), options);
    }
    else if (data.isConstBufferArray()) {
        error = this->deflateRep(context,
                                 result,
                                 data.constBufferArray(),
                                 options);
    }
    else if (data.isConstBufferPtrArray()) {
        error = this->deflateRep(context,
                                 result,
                                 data.constBufferPtrArray(),
                                 options);
    }
    else if (data.isMutableBuffer()) {
        error =
            this->deflateRep(context, result, data.mutableBuffer(), options);
    }
    else if (data.isMutableBufferArray()) {
        error = this->deflateRep(context,
                                 result,
                                 data.mutableBufferArray(),
                                 options);
    }
    else if (data.isMutableBufferPtrArray()) {
        error = this->deflateRep(context,
                                 result,
                                 data.mutableBufferPtrArray(),
                                 options);
    }
    else if (data.isString()) {
        error = this->deflateRep(context, result, data.string(), options);
    }
    else if (data.isFile()) {
        error = this->deflateRep(context, result, data.file(), options);
    }
    else {
        error = ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    if (error) {
        return error;
    }

    error = this->deflateEnd(context, result, options);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Compression::inflate(ntca::InflateContext*       context,
                                 bdlbb::Blob*                result,
                                 const bdlbb::Blob&          data,
                                 const ntca::InflateOptions& options)
{
    ntsa::Error error;

    context->reset();

    error = this->inflateBegin(context, result, options);
    if (error) {
        return error;
    }

    error = this->inflateRep(context, result, data, options);
    if (error) {
        return error;
    }

    error = this->inflateEnd(context, result, options);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Compression::inflate(ntca::InflateContext*       context,
                                 bdlbb::Blob*                result,
                                 const ntsa::Data&           data,
                                 const ntca::InflateOptions& options)
{
    ntsa::Error error;

    context->reset();

    error = this->inflateBegin(context, result, options);
    if (error) {
        return error;
    }

    if (data.isBlob()) {
        error = this->inflateRep(context, result, data.blob(), options);
    }
    else if (data.isSharedBlob() && data.sharedBlob().get() != 0) {
        error = this->inflateRep(context, result, *data.sharedBlob(), options);
    }
    else if (data.isBlobBuffer()) {
        error = this->inflateRep(context, result, data.blobBuffer(), options);
    }
    else if (data.isConstBuffer()) {
        error = this->inflateRep(context, result, data.constBuffer(), options);
    }
    else if (data.isConstBufferArray()) {
        error = this->inflateRep(context,
                                 result,
                                 data.constBufferArray(),
                                 options);
    }
    else if (data.isConstBufferPtrArray()) {
        error = this->inflateRep(context,
                                 result,
                                 data.constBufferPtrArray(),
                                 options);
    }
    else if (data.isMutableBuffer()) {
        error =
            this->inflateRep(context, result, data.mutableBuffer(), options);
    }
    else if (data.isMutableBufferArray()) {
        error = this->inflateRep(context,
                                 result,
                                 data.mutableBufferArray(),
                                 options);
    }
    else if (data.isMutableBufferPtrArray()) {
        error = this->inflateRep(context,
                                 result,
                                 data.mutableBufferPtrArray(),
                                 options);
    }
    else if (data.isString()) {
        error = this->inflateRep(context, result, data.string(), options);
    }
    else if (data.isFile()) {
        error = this->inflateRep(context, result, data.file(), options);
    }
    else {
        error = ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    error = this->inflateEnd(context, result, options);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntca::CompressionType::Value Compression::type() const
{
    return ntca::CompressionType::e_UNDEFINED;
}

}  // close package namespace
}  // close enterprise namespace
