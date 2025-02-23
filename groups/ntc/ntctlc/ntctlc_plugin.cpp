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

#include <ntctlc_plugin.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntctls_plugin_cpp, "$Id$ $CSID$")

#include <ntcd_compression.h>
#include <ntcs_datapool.h>
#include <ntci_log.h>
#include <ntca_checksum.h>

#if NTC_BUILD_WITH_LZ4
#include <lz4.h>
#include <lz4frame.h>
#endif

#if NTC_BUILD_WITH_ZSTD
#include <zstd.h>
#include <zstd_errors.h>
#endif

#if NTC_BUILD_WITH_ZLIB
#include <zlib.h>
#endif

namespace BloombergLP {
namespace ntctlc {

#if NTC_BUILD_WITH_LZ4

/// @internal @brief
/// Provide a mechanism to deflate and inflate a data stream according to the
/// LZ4 algorithm.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntctlc
class Lz4 : public ntci::Compression
{
    bsl::shared_ptr<ntci::DataPool> d_dataPool_sp;
    ntca::CompressionConfig         d_config;
    bslma::Allocator*               d_allocator_p;

  private:
    Lz4(const Lz4&) BSLS_KEYWORD_DELETED;
    Lz4& operator=(const Lz4&) BSLS_KEYWORD_DELETED;

  private:
    /// Begin a deflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error deflateBegin(ntca::DeflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::DeflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Deflate the specified 'data' having the specified 'size' and append the
    /// result to the specified 'result'. Return the error.
    ntsa::Error deflateNext(ntca::DeflateContext*       context,
                            bdlbb::Blob*                result,
                            const bsl::uint8_t*         data,
                            bsl::size_t                 size,
                            const ntca::DeflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// End a deflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error deflateEnd(ntca::DeflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntca::DeflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Begin an inflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error inflateBegin(ntca::InflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::InflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Inflate the specified 'data' having the specified 'size' and append the
    /// result to the specified 'result'. Return the error.
    ntsa::Error inflateNext(ntca::InflateContext*       context,
                            bdlbb::Blob*                result,
                            const bsl::uint8_t*         data,
                            bsl::size_t                 size,
                            const ntca::InflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// End an inflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error inflateEnd(ntca::InflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntca::InflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

  public:
    /// Create a new mechanism to inflate and deflate data according to the
    /// specified 'configuration'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    Lz4(const ntca::CompressionConfig&         configuration,
        const bsl::shared_ptr<ntci::DataPool>& dataPool,
        bslma::Allocator*                      basicAllocator = 0);

    /// Destroy this object.
    ~Lz4() BSLS_KEYWORD_OVERRIDE;

    /// Return the compression type implemented by this mechanism.
    ntca::CompressionType::Value type() const BSLS_KEYWORD_OVERRIDE;
};

#endif

#if NTC_BUILD_WITH_ZSTD

/// @internal @brief
/// Provide a mechanism to deflate and inflate a data stream according to the
/// zstd algorithm.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntctlc
class Zstd : public ntci::Compression
{
    bsl::shared_ptr<ntci::DataPool> d_dataPool_sp;
    ntca::CompressionConfig         d_config;
    bslma::Allocator*               d_allocator_p;

  private:
    Zstd(const Zstd&) BSLS_KEYWORD_DELETED;
    Zstd& operator=(const Zstd&) BSLS_KEYWORD_DELETED;

  private:
    /// Begin a deflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error deflateBegin(ntca::DeflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::DeflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Deflate the specified 'data' having the specified 'size' and append the
    /// result to the specified 'result'. Return the error.
    ntsa::Error deflateNext(ntca::DeflateContext*       context,
                            bdlbb::Blob*                result,
                            const bsl::uint8_t*         data,
                            bsl::size_t                 size,
                            const ntca::DeflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// End a deflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error deflateEnd(ntca::DeflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntca::DeflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Begin an inflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error inflateBegin(ntca::InflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::InflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Inflate the specified 'data' having the specified 'size' and append the
    /// result to the specified 'result'. Return the error.
    ntsa::Error inflateNext(ntca::InflateContext*       context,
                            bdlbb::Blob*                result,
                            const bsl::uint8_t*         data,
                            bsl::size_t                 size,
                            const ntca::InflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// End an inflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error inflateEnd(ntca::InflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntca::InflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

  public:
    /// Create a new mechanism to inflate and deflate data according to the
    /// specified 'configuration'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    Zstd(const ntca::CompressionConfig&         configuration,
         const bsl::shared_ptr<ntci::DataPool>& dataPool,
         bslma::Allocator*                      basicAllocator = 0);

    /// Destroy this object.
    ~Zstd() BSLS_KEYWORD_OVERRIDE;

    /// Return the compression type implemented by this mechanism.
    ntca::CompressionType::Value type() const BSLS_KEYWORD_OVERRIDE;
};

#endif

#if NTC_BUILD_WITH_ZLIB

/// @internal @brief
/// Provide a mechanism to deflate and inflate a data stream according to the
/// zlib algorithm.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntctlc
class Zlib : public ntci::Compression
{
    z_stream                        d_deflater;
    z_stream                        d_inflater;
    bdlbb::BlobBuffer               d_deflatedBuffer;
    bsl::size_t                     d_deflatedBufferBytesWritten;
    bdlbb::BlobBuffer               d_inflatedBuffer;
    bsl::size_t                     d_inflatedBufferBytesWritten;
    int                             d_level;
    bsl::shared_ptr<ntci::DataPool> d_dataPool_sp;
    ntca::CompressionConfig         d_config;
    bslma::Allocator*               d_allocator_p;

  private:
    Zlib(const Zlib&) BSLS_KEYWORD_DELETED;
    Zlib& operator=(const Zlib&) BSLS_KEYWORD_DELETED;

  private:
    /// Begin a deflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error deflateBegin(ntca::DeflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::DeflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Deflate the specified 'data' having the specified 'size' and append the
    /// result to the specified 'result'. Return the error.
    ntsa::Error deflateNext(ntca::DeflateContext*       context,
                            bdlbb::Blob*                result,
                            const bsl::uint8_t*         data,
                            bsl::size_t                 size,
                            const ntca::DeflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// End a deflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error deflateEnd(ntca::DeflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntca::DeflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Begin an inflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error inflateBegin(ntca::InflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::InflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Inflate the specified 'data' having the specified 'size' and append the
    /// result to the specified 'result'. Return the error.
    ntsa::Error inflateNext(ntca::InflateContext*       context,
                            bdlbb::Blob*                result,
                            const bsl::uint8_t*         data,
                            bsl::size_t                 size,
                            const ntca::InflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// End an inflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error inflateEnd(ntca::InflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntca::InflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Allocate the specify 'number' of elements of the specified 'size' in
    /// bytes. The specified 'opaque' field points to the allocator object.
    static void* allocate(void*        opaque, 
                          unsigned int number, 
                          unsigned int size);

    /// Free memory pointed to by the specified 'address'. The specified
    /// 'opaque' field points to the allocator object.
    static void free(void* opaque, void* address);

    /// Return the string description of the specified 'error' code.
    static const char* describeError(int error);

    /// Return the error associated with the specified zlib 'error' code
    /// detected when performing the specified 'operation'.
    static ntsa::Error translateError(int error, const char* operation);

  public:
    /// Create a new mechanism to inflate and deflate data according to the
    /// specified 'configuration'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    Zlib(const ntca::CompressionConfig&         configuration,
         const bsl::shared_ptr<ntci::DataPool>& dataPool,
         bslma::Allocator*                      basicAllocator = 0);

    /// Destroy this object.
    ~Zlib() BSLS_KEYWORD_OVERRIDE;

    /// Return the compression type implemented by this mechanism.
    ntca::CompressionType::Value type() const BSLS_KEYWORD_OVERRIDE;
};

/// @internal @brief
/// Provide a mechanism to deflate and inflate a data stream according to the
/// gzip algorithm.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntctlc
class Gzip : public ntci::Compression
{
    bsl::shared_ptr<ntci::DataPool> d_dataPool_sp;
    ntca::CompressionConfig         d_config;
    bslma::Allocator*               d_allocator_p;

  private:
    Gzip(const Gzip&) BSLS_KEYWORD_DELETED;
    Gzip& operator=(const Gzip&) BSLS_KEYWORD_DELETED;

  private:
    /// Begin a deflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error deflateBegin(ntca::DeflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::DeflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Deflate the specified 'data' having the specified 'size' and append the
    /// result to the specified 'result'. Return the error.
    ntsa::Error deflateNext(ntca::DeflateContext*       context,
                            bdlbb::Blob*                result,
                            const bsl::uint8_t*         data,
                            bsl::size_t                 size,
                            const ntca::DeflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// End a deflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error deflateEnd(ntca::DeflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntca::DeflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Begin an inflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error inflateBegin(ntca::InflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::InflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Inflate the specified 'data' having the specified 'size' and append the
    /// result to the specified 'result'. Return the error.
    ntsa::Error inflateNext(ntca::InflateContext*       context,
                            bdlbb::Blob*                result,
                            const bsl::uint8_t*         data,
                            bsl::size_t                 size,
                            const ntca::InflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// End an inflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error inflateEnd(ntca::InflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntca::InflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

  public:
    /// Create a new mechanism to inflate and deflate data according to the
    /// specified 'configuration'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    Gzip(const ntca::CompressionConfig&         configuration,
         const bsl::shared_ptr<ntci::DataPool>& dataPool,
         bslma::Allocator*                      basicAllocator = 0);

    /// Destroy this object.
    ~Gzip() BSLS_KEYWORD_OVERRIDE;

    /// Return the compression type implemented by this mechanism.
    ntca::CompressionType::Value type() const BSLS_KEYWORD_OVERRIDE;
};

#endif

#if NTC_BUILD_WITH_LZ4

ntsa::Error Lz4::deflateBegin(ntca::DeflateContext*       context,
                              bdlbb::Blob*                result,
                              const ntca::DeflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Lz4::deflateNext(ntca::DeflateContext*       context,
                             bdlbb::Blob*                result,
                             const bsl::uint8_t*         data,
                             bsl::size_t                 size,
                             const ntca::DeflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Lz4::deflateEnd(ntca::DeflateContext*       context,
                            bdlbb::Blob*                result,
                            const ntca::DeflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Lz4::inflateBegin(ntca::InflateContext*       context,
                              bdlbb::Blob*                result,
                              const ntca::InflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Lz4::inflateNext(ntca::InflateContext*       context,
                             bdlbb::Blob*                result,
                             const bsl::uint8_t*         data,
                             bsl::size_t                 size,
                             const ntca::InflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Lz4::inflateEnd(ntca::InflateContext*       context,
                            bdlbb::Blob*                result,
                            const ntca::InflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

Lz4::Lz4(const ntca::CompressionConfig&         configuration,
         const bsl::shared_ptr<ntci::DataPool>& dataPool,
         bslma::Allocator*                      basicAllocator)
: d_dataPool_sp(dataPool)
, d_config(configuration)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Lz4::~Lz4()
{
}

ntca::CompressionType::Value Lz4::type() const
{
    return ntca::CompressionType::e_LZ4;
}

#endif

#if NTC_BUILD_WITH_ZSTD

ntsa::Error Zstd::deflateBegin(ntca::DeflateContext*       context,
                               bdlbb::Blob*                result,
                               const ntca::DeflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Zstd::deflateNext(ntca::DeflateContext*       context,
                              bdlbb::Blob*                result,
                              const bsl::uint8_t*         data,
                              bsl::size_t                 size,
                              const ntca::DeflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Zstd::deflateEnd(ntca::DeflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::DeflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Zstd::inflateBegin(ntca::InflateContext*       context,
                               bdlbb::Blob*                result,
                               const ntca::InflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Zstd::inflateNext(ntca::InflateContext*       context,
                              bdlbb::Blob*                result,
                              const bsl::uint8_t*         data,
                              bsl::size_t                 size,
                              const ntca::InflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Zstd::inflateEnd(ntca::InflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::InflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

Zstd::Zstd(const ntca::CompressionConfig&         configuration,
           const bsl::shared_ptr<ntci::DataPool>& dataPool,
           bslma::Allocator*                      basicAllocator)
: d_dataPool_sp(dataPool)
, d_config(configuration)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Zstd::~Zstd()
{
}

ntca::CompressionType::Value Zstd::type() const
{
    return ntca::CompressionType::e_ZSTD;
}

#endif

#if NTC_BUILD_WITH_ZLIB

ntsa::Error Zlib::deflateBegin(ntca::DeflateContext*       context,
                               bdlbb::Blob*                result,
                               const ntca::DeflateOptions& options)
{
    d_deflater.next_in  = 0;
    d_deflater.avail_in = 0;
    d_deflater.total_in = 0;

    d_deflater.next_out  = 0;
    d_deflater.avail_out = 0;
    d_deflater.total_out = 0;
    d_deflater.adler     = 0;

    return ntsa::Error();
}

ntsa::Error Zlib::deflateNext(ntca::DeflateContext*       context,
                              bdlbb::Blob*                result,
                              const bsl::uint8_t*         data,
                              bsl::size_t                 size,
                              const ntca::DeflateOptions& options)
{
    NTCI_LOG_CONTEXT();

    int rc = 0;

    bsl::size_t totalBytesWritten = 0;
    bsl::size_t totalBytesRead = 0;

    d_deflater.next_in  = const_cast<bsl::uint8_t*>(data);
    d_deflater.avail_in = size;

    while (d_deflater.avail_in != 0) {
        if (d_deflater.avail_out == 0) {
            if (d_deflatedBufferBytesWritten != 0) {
                d_deflatedBuffer.setSize(d_deflatedBufferBytesWritten);

                BSLS_ASSERT(d_deflatedBuffer.buffer().get() != 0);
                BSLS_ASSERT(d_deflatedBuffer.size() > 0);

                result->appendDataBuffer(d_deflatedBuffer);
                
                d_deflatedBuffer.reset();
                d_deflatedBufferBytesWritten = 0;
            }

            d_dataPool_sp->createOutgoingBlobBuffer(&d_deflatedBuffer);

            d_deflater.next_out = 
                reinterpret_cast<unsigned char*>(d_deflatedBuffer.data());
            d_deflater.avail_out = d_deflatedBuffer.size();

            d_deflatedBufferBytesWritten = 0;
        }

        uInt availOut0 = d_deflater.avail_out;
        uInt availIn0  = d_deflater.avail_in;

        rc = ::deflate(&d_deflater, Z_NO_FLUSH);

        uInt availOut1 = d_deflater.avail_out;
        uInt availIn1  = d_deflater.avail_in;

        uInt numBytesWritten = availOut0 - availOut1;
        uInt numBytesRead    = availIn0 - availIn1;

        totalBytesRead += numBytesRead;
        totalBytesWritten += numBytesWritten;

        d_deflatedBufferBytesWritten += numBytesWritten;

        if (rc != Z_OK && rc != Z_BUF_ERROR) {
            return this->translateError(rc, "deflate");
        }
    }

    context->setBytesRead(context->bytesRead() + totalBytesRead);
    context->setBytesWritten(context->bytesWritten() + totalBytesWritten);

    return ntsa::Error();
}

ntsa::Error Zlib::deflateEnd(ntca::DeflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::DeflateOptions& options)
{
    NTCI_LOG_CONTEXT();

    int rc = 0;

    bsl::size_t totalBytesWritten = 0;
    bsl::size_t totalBytesRead = 0;

    while (true) {
        if (d_deflater.avail_out == 0) {
            if (d_deflatedBufferBytesWritten != 0) {
                d_deflatedBuffer.setSize(d_deflatedBufferBytesWritten);

                BSLS_ASSERT(d_deflatedBuffer.buffer().get() != 0);
                BSLS_ASSERT(d_deflatedBuffer.size() > 0);

                result->appendDataBuffer(d_deflatedBuffer);
                
                d_deflatedBuffer.reset();
                d_deflatedBufferBytesWritten = 0;
            }

            d_dataPool_sp->createOutgoingBlobBuffer(&d_deflatedBuffer);

            d_deflater.next_out = 
                reinterpret_cast<unsigned char*>(d_deflatedBuffer.data());
            d_deflater.avail_out = d_deflatedBuffer.size();

            d_deflatedBufferBytesWritten = 0;
        }

        uInt availOut0 = d_deflater.avail_out;
        uInt availIn0  = d_deflater.avail_in;

        rc = ::deflate(&d_deflater, Z_SYNC_FLUSH);

        uInt availOut1 = d_deflater.avail_out;
        uInt availIn1  = d_deflater.avail_in;

        uInt numBytesWritten = availOut0 - availOut1;
        uInt numBytesRead    = availIn0 - availIn1;

        totalBytesRead += numBytesRead;
        totalBytesWritten += numBytesWritten;

        d_deflatedBufferBytesWritten += numBytesWritten;

        if (rc == Z_BUF_ERROR && 
            numBytesRead == 0 && numBytesWritten == 0) 
        {
            if (d_deflatedBufferBytesWritten != 0) {
                d_deflatedBuffer.setSize(d_deflatedBufferBytesWritten);

                BSLS_ASSERT(d_deflatedBuffer.buffer().get() != 0);
                BSLS_ASSERT(d_deflatedBuffer.size() > 0);

                result->appendDataBuffer(d_deflatedBuffer);
                
                d_deflatedBuffer.reset();
                d_deflatedBufferBytesWritten = 0;
            }

            break;
        }

        if (rc != Z_OK) {
            return this->translateError(rc, "deflate");
        }
    }

    context->setBytesRead(context->bytesRead() + totalBytesRead);
    context->setBytesWritten(context->bytesWritten() + totalBytesWritten);

    ntca::Checksum checksum;
    checksum.store(ntca::ChecksumType::e_CRC32, 
                   &d_deflater.adler, 
                   sizeof d_deflater.adler);

    context->setChecksum(checksum);

    return ntsa::Error();
}

ntsa::Error Zlib::inflateBegin(ntca::InflateContext*       context,
                               bdlbb::Blob*                result,
                               const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error();
}

ntsa::Error Zlib::inflateNext(ntca::InflateContext*       context,
                              bdlbb::Blob*                result,
                              const bsl::uint8_t*         data,
                              bsl::size_t                 size,
                              const ntca::InflateOptions& options)
{
    NTCI_LOG_CONTEXT();

    int rc = 0;

    bsl::size_t totalBytesWritten = 0;
    bsl::size_t totalBytesRead = 0;

    BSLS_ASSERT(d_inflater.next_in  == 0);
    BSLS_ASSERT(d_inflater.avail_in == 0);

    d_inflater.next_in  = const_cast<bsl::uint8_t*>(data);
    d_inflater.avail_in = size;

    while (d_inflater.avail_in != 0) {
        if (d_inflater.avail_out == 0) {
            if (d_inflatedBufferBytesWritten != 0) {
                d_inflatedBuffer.setSize(d_inflatedBufferBytesWritten);

                BSLS_ASSERT(d_inflatedBuffer.buffer().get() != 0);
                BSLS_ASSERT(d_inflatedBuffer.size() > 0);

                result->appendDataBuffer(d_inflatedBuffer);
                
                d_inflatedBuffer.reset();
                d_inflatedBufferBytesWritten = 0;

                d_inflater.next_out = 0;
                d_inflater.avail_out = 0;
            }

            d_dataPool_sp->createOutgoingBlobBuffer(&d_inflatedBuffer);

            d_inflater.next_out = 
                reinterpret_cast<unsigned char*>(d_inflatedBuffer.data());
            d_inflater.avail_out = d_inflatedBuffer.size();

            d_inflatedBufferBytesWritten = 0;
        }

        uInt availOut0 = d_inflater.avail_out;
        uInt availIn0  = d_inflater.avail_in;

        rc = ::inflate(&d_inflater, Z_NO_FLUSH);

        uInt availOut1 = d_inflater.avail_out;
        uInt availIn1  = d_inflater.avail_in;

        uInt numBytesWritten = availOut0 - availOut1;
        uInt numBytesRead    = availIn0 - availIn1;

        totalBytesRead += numBytesRead;
        totalBytesWritten += numBytesWritten;

        d_inflatedBufferBytesWritten += numBytesWritten;

        if (rc != Z_OK && rc != Z_BUF_ERROR) {
            return this->translateError(rc, "inflate");
        }
    }

    d_inflater.next_in  = 0;
    d_inflater.avail_in = 0;

    context->setBytesRead(context->bytesRead() + totalBytesRead);
    context->setBytesWritten(context->bytesWritten() + totalBytesWritten);

    return ntsa::Error();
}

ntsa::Error Zlib::inflateEnd(ntca::InflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::InflateOptions& options)
{
    NTCI_LOG_CONTEXT();

    int rc = 0;

    bsl::size_t totalBytesWritten = 0;
    bsl::size_t totalBytesRead = 0;

    BSLS_ASSERT(d_inflater.next_in  == 0);
    BSLS_ASSERT(d_inflater.avail_in == 0);

    while (true) {
        if (d_inflater.avail_out == 0) {
            if (d_inflatedBufferBytesWritten != 0) {
                d_inflatedBuffer.setSize(d_inflatedBufferBytesWritten);

                BSLS_ASSERT(d_inflatedBuffer.buffer().get() != 0);
                BSLS_ASSERT(d_inflatedBuffer.size() > 0);

                result->appendDataBuffer(d_inflatedBuffer);
                
                d_inflatedBuffer.reset();
                d_inflatedBufferBytesWritten = 0;

                d_inflater.next_out = 0;
                d_inflater.avail_out = 0;
            }

            d_dataPool_sp->createOutgoingBlobBuffer(&d_inflatedBuffer);

            d_inflater.next_out = 
                reinterpret_cast<unsigned char*>(d_inflatedBuffer.data());
            d_inflater.avail_out = d_inflatedBuffer.size();

            d_inflatedBufferBytesWritten = 0;
        }

        uInt availOut0 = d_inflater.avail_out;
        uInt availIn0  = d_inflater.avail_in;

        rc = ::inflate(&d_inflater, Z_SYNC_FLUSH);

        uInt availOut1 = d_inflater.avail_out;
        uInt availIn1  = d_inflater.avail_in;

        uInt numBytesWritten = availOut0 - availOut1;
        uInt numBytesRead    = availIn0 - availIn1;

        totalBytesRead += numBytesRead;
        totalBytesWritten += numBytesWritten;

        d_inflatedBufferBytesWritten += numBytesWritten;

        if (rc == Z_BUF_ERROR && 
            numBytesRead == 0 && numBytesWritten == 0) 
        {
            if (d_inflatedBufferBytesWritten != 0) {
                d_inflatedBuffer.setSize(d_inflatedBufferBytesWritten);

                BSLS_ASSERT(d_inflatedBuffer.buffer().get() != 0);
                BSLS_ASSERT(d_inflatedBuffer.size() > 0);

                result->appendDataBuffer(d_inflatedBuffer);
                
                d_inflatedBuffer.reset();
                d_inflatedBufferBytesWritten = 0;

                d_inflater.next_out = 0;
                d_inflater.avail_out = 0;
            }

            break;
        }

        if (rc != Z_OK) {
            return this->translateError(rc, "inflate");
        }
    }

    d_inflater.next_in  = 0;
    d_inflater.avail_in = 0;

    context->setBytesRead(context->bytesRead() + totalBytesRead);
    context->setBytesWritten(context->bytesWritten() + totalBytesWritten);

    return ntsa::Error();
}

void* Zlib::allocate(void* opaque, unsigned int number, unsigned int size)
{
    bslma::Allocator* allocator = reinterpret_cast<bslma::Allocator*>(opaque);

    return allocator->allocate(number * size);
}

void Zlib::free(void* opaque, void* address)
{
    bslma::Allocator* allocator = reinterpret_cast<bslma::Allocator*>(opaque);

    allocator->deallocate(address);
}

const char* Zlib::describeError(int error)
{
    const char* errorDescription;

    switch (error) {
    case Z_OK:
        errorDescription = "OK";
        break;
    case Z_STREAM_END:
        errorDescription = "STREAM_END";
        break;
    case Z_NEED_DICT:
        errorDescription = "NEED_DICT";
        break;
    case Z_ERRNO:
        errorDescription = "ERRNO";
        break;
    case Z_STREAM_ERROR:
        errorDescription = "STREAM_ERROR";
        break;
    case Z_DATA_ERROR:
        errorDescription = "DATA_ERROR";
        break;
    case Z_MEM_ERROR:
        errorDescription = "MEM_ERROR";
        break;
    case Z_BUF_ERROR:
        errorDescription = "BUF_ERROR";
        break;
    case Z_VERSION_ERROR:
        errorDescription = "VERSION_ERROR";
        break;
    default:
        errorDescription = "???";
        break;
    }

    return errorDescription;
}

ntsa::Error Zlib::translateError(int error, const char* operation)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error result;

    switch (error) {
    case Z_OK:
        result = ntsa::Error();
        break;
    case Z_STREAM_END:
        result = ntsa::Error(ntsa::Error::e_EOF);
        break;
    case Z_NEED_DICT:
        result = ntsa::Error(ntsa::Error::e_INVALID);
        break;
    case Z_ERRNO:
        result = ntsa::Error(ntsa::Error::e_INVALID);
        break;
    case Z_STREAM_ERROR:
        result = ntsa::Error(ntsa::Error::e_INVALID);
        break;
    case Z_DATA_ERROR:
        result = ntsa::Error(ntsa::Error::e_INVALID);
        break;
    case Z_MEM_ERROR:
        result = ntsa::Error(ntsa::Error::e_LIMIT);
        break;
    case Z_BUF_ERROR:
        result = ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        break;
    case Z_VERSION_ERROR:
        result = ntsa::Error(ntsa::Error::e_INVALID);
        break;
    default:
        result = ntsa::Error(ntsa::Error::e_INVALID);
        break;
    }

    const char* errorDescription = Zlib::describeError(error);

    if (result) {
        NTCI_LOG_STREAM_WARN << "Failed to " 
                             << operation 
                             << ": " 
                             << errorDescription
                             << " (rc = " 
                             << error 
                             << ")"
                             << NTCI_LOG_STREAM_END;
    }

    return result;
}

Zlib::Zlib(const ntca::CompressionConfig&         configuration,
           const bsl::shared_ptr<ntci::DataPool>& dataPool,
           bslma::Allocator*                      basicAllocator)
: d_deflatedBuffer()
, d_deflatedBufferBytesWritten(0)
, d_inflatedBuffer()
, d_inflatedBufferBytesWritten(0)
, d_level(Z_DEFAULT_COMPRESSION)
, d_dataPool_sp(dataPool)
, d_config(configuration)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    int rc;

    BSLS_ASSERT_OPT(d_config.type().isNull() || 
                    d_config.type().value() == ntca::CompressionType::e_ZLIB);

    if (d_config.goal().has_value()) {
        switch (d_config.goal().value()) {
        case ntca::CompressionGoal::e_BEST_SIZE: // 9
            d_level = Z_BEST_COMPRESSION;
            break;
        case ntca::CompressionGoal::e_BETTER_SIZE: 
            d_level = 7;
            break;
        case ntca::CompressionGoal::e_BALANCED:
            d_level = Z_DEFAULT_COMPRESSION; // 6
            break;
        case ntca::CompressionGoal::e_BETTER_SPEED:
            d_level = 4;
            break;
        case ntca::CompressionGoal::e_BEST_SPEED:
            d_level = Z_BEST_SPEED; // 1
            break;
        default:
            break;
        }
    }

    bsl::memset(&d_deflater, 0, sizeof d_deflater);
    bsl::memset(&d_inflater, 0, sizeof d_inflater);

    d_deflater.zalloc = &Zlib::allocate;
    d_deflater.zfree  = &Zlib::free;
    d_deflater.opaque = d_allocator_p;

    rc = deflateInit(&d_deflater, d_level);
    BSLS_ASSERT_OPT(rc == Z_OK);

    d_inflater.zalloc = &Zlib::allocate;
    d_inflater.zfree  = &Zlib::free;
    d_inflater.opaque = d_allocator_p;

    rc = inflateInit(&d_inflater);
    BSLS_ASSERT_OPT(rc == Z_OK);
}

Zlib::~Zlib()
{
    int rc;

    d_inflater.next_in  = 0;
    d_inflater.avail_in = 0;
    d_inflater.total_in = 0;

    d_inflater.next_out  = 0;
    d_inflater.avail_out = 0;
    d_inflater.total_out = 0;
    d_inflater.adler     = 0;

    rc = ::inflateReset(&d_inflater);
    if (rc != Z_OK) {
        translateError(rc, "reset inflater");
    }

    rc = ::inflateEnd(&d_inflater);
    if (rc != Z_OK) {
        translateError(rc, "close inflater");
    }

    d_deflater.next_in  = 0;
    d_deflater.avail_in = 0;
    d_deflater.total_in = 0;

    d_deflater.next_out  = 0;
    d_deflater.avail_out = 0;
    d_deflater.total_out = 0;
    d_deflater.adler     = 0;

    rc = ::deflateReset(&d_deflater);
    if (rc != Z_OK) {
        translateError(rc, "reset deflater");
    }

    rc = ::deflateEnd(&d_deflater);
    if (rc != Z_OK) {
        translateError(rc, "close deflater");
    }
}

ntca::CompressionType::Value Zlib::type() const
{
    return ntca::CompressionType::e_ZLIB;
}

ntsa::Error Gzip::deflateBegin(ntca::DeflateContext*       context,
                               bdlbb::Blob*                result,
                               const ntca::DeflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Gzip::deflateNext(ntca::DeflateContext*       context,
                              bdlbb::Blob*                result,
                              const bsl::uint8_t*         data,
                              bsl::size_t                 size,
                              const ntca::DeflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Gzip::deflateEnd(ntca::DeflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::DeflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Gzip::inflateBegin(ntca::InflateContext*       context,
                               bdlbb::Blob*                result,
                               const ntca::InflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Gzip::inflateNext(ntca::InflateContext*       context,
                              bdlbb::Blob*                result,
                              const bsl::uint8_t*         data,
                              bsl::size_t                 size,
                              const ntca::InflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Gzip::inflateEnd(ntca::InflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::InflateOptions& options)
{
    NTCCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

Gzip::Gzip(const ntca::CompressionConfig&         configuration,
           const bsl::shared_ptr<ntci::DataPool>& dataPool,
           bslma::Allocator*                      basicAllocator)
: d_dataPool_sp(dataPool)
, d_config(configuration)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Gzip::~Gzip()
{
}

ntca::CompressionType::Value Gzip::type() const
{
    return ntca::CompressionType::e_GZIP;
}

#endif

CompressionDriver::CompressionDriver(bslma::Allocator* basicAllocator)
: d_allocator_p(basicAllocator)
{
}

CompressionDriver::~CompressionDriver()
{
}

ntsa::Error CompressionDriver::createCompression(
    bsl::shared_ptr<ntci::Compression>* result,
    const ntca::CompressionConfig&      configuration,
    bslma::Allocator*                   basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(allocator, allocator);

    return this->createCompression(result, configuration, dataPool, allocator);
}

ntsa::Error CompressionDriver::createCompression(
    bsl::shared_ptr<ntci::Compression>*              result,
    const ntca::CompressionConfig&                   configuration,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(allocator,
                           blobBufferFactory,
                           blobBufferFactory,
                           allocator);

    return this->createCompression(result, configuration, dataPool, allocator);
}

ntsa::Error CompressionDriver::createCompression(
    bsl::shared_ptr<ntci::Compression>*    result,
    const ntca::CompressionConfig&         configuration,
    const bsl::shared_ptr<ntci::DataPool>& dataPool,
    bslma::Allocator*                      basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntca::CompressionType::Value compressionType =
        configuration.type().value_or(ntca::CompressionType::e_ZLIB);

    if (compressionType == ntca::CompressionType::e_RLE) {
        bsl::shared_ptr<ntcd::Compression> compression;
        compression.createInplace(allocator,
                                  configuration,
                                  dataPool,
                                  allocator);
        *result = compression;
        return ntsa::Error();
    }
#if NTC_BUILD_WITH_LZ4
    else if (compressionType == ntca::CompressionType::e_LZ4) {
        bsl::shared_ptr<ntctlc::Lz4> compression;
        compression.createInplace(allocator,
                                  configuration,
                                  dataPool,
                                  allocator);
        *result = compression;
        return ntsa::Error();
    }
#endif
#if NTC_BUILD_WITH_ZSTD
    else if (compressionType == ntca::CompressionType::e_ZSTD) {
        bsl::shared_ptr<ntctlc::Zstd> compression;
        compression.createInplace(allocator,
                                  configuration,
                                  dataPool,
                                  allocator);
        *result = compression;
        return ntsa::Error();
    }
#endif
#if NTC_BUILD_WITH_ZLIB
    else if (compressionType == ntca::CompressionType::e_ZLIB) {
        bsl::shared_ptr<ntctlc::Zlib> compression;
        compression.createInplace(allocator,
                                  configuration,
                                  dataPool,
                                  allocator);
        *result = compression;
        return ntsa::Error();
    }
    else if (compressionType == ntca::CompressionType::e_GZIP) {
        bsl::shared_ptr<ntctlc::Gzip> compression;
        compression.createInplace(allocator,
                                  configuration,
                                  dataPool,
                                  allocator);
        *result = compression;
        return ntsa::Error();
    }
#endif
    else {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }
}

}  // close namespace ntctlc
}  // close namespace BloombergLP
