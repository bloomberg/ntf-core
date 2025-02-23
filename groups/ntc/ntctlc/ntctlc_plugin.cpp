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
    z_stream                        d_deflaterStream;
    bdlbb::BlobBuffer               d_deflaterBuffer;
    bsl::size_t                     d_deflaterBufferSize;
    z_stream                        d_inflaterStream;
    bdlbb::BlobBuffer               d_inflaterBuffer;
    bsl::size_t                     d_inflaterBufferSize;
    int                             d_level;
    bsl::shared_ptr<ntci::DataPool> d_dataPool_sp;
    ntca::CompressionConfig         d_config;
    bslma::Allocator*               d_allocator_p;

  private:
    Zlib(const Zlib&) BSLS_KEYWORD_DELETED;
    Zlib& operator=(const Zlib&) BSLS_KEYWORD_DELETED;

  private:
    /// Handle the lack of lack of output capacity available to the deflater
    /// by committing the previous buffer, if any, and allocating a new one.
    void deflateOverflow(bdlbb::Blob* result);

    /// Commit the deflater buffer, if any bytes have been been written to it,
    /// to the specified 'result'.
    void deflateBufferCommit(bdlbb::Blob* result);

    /// Feed the input bytes available to the deflate algorithm and write
    /// any output bytes to the output buffer. Return the error. 
    int deflateCycle(bsl::size_t* numBytesRead, 
                     bsl::size_t* numBytesWritten, 
                     int          mode);

    /// Handle the lack of lack of output capacity available to the inflater
    /// by committing the previous buffer, if any, and allocating a new one.
    void inflateOverflow(bdlbb::Blob* result);

    /// Commit the inflater buffer, if any bytes have been been written to it,
    /// to the specified 'result'.
    void inflateBufferCommit(bdlbb::Blob* result);

    /// Feed the input bytes available to the inflate algorithm and write
    /// any output bytes to the output buffer. Return the error. 
    int inflateCycle(bsl::size_t* numBytesRead, 
                     bsl::size_t* numBytesWritten, 
                     int          mode);

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

NTCCFG_INLINE
void Zlib::deflateOverflow(bdlbb::Blob* result)
{
    if (d_deflaterBufferSize != 0) {
        this->deflateBufferCommit(result);
    }

    d_deflaterBuffer.reset();
    d_deflaterBufferSize = 0;

    d_dataPool_sp->createOutgoingBlobBuffer(&d_deflaterBuffer);

    d_deflaterStream.next_out = 
        reinterpret_cast<unsigned char*>(d_deflaterBuffer.data());
    d_deflaterStream.avail_out = d_deflaterBuffer.size();    
}

NTCCFG_INLINE
void Zlib::deflateBufferCommit(bdlbb::Blob* result)
{
    if (d_deflaterBufferSize != 0) {
        d_deflaterBuffer.setSize(d_deflaterBufferSize);

        BSLS_ASSERT(d_deflaterBuffer.buffer().get() != 0);
        BSLS_ASSERT(d_deflaterBuffer.size() > 0);

        result->appendDataBuffer(d_deflaterBuffer);
        
        d_deflaterBuffer.reset();
        d_deflaterBufferSize = 0;

        d_deflaterStream.next_out = 0;
        d_deflaterStream.avail_out = 0;
    }
}

NTCCFG_INLINE
int Zlib::deflateCycle(bsl::size_t* numBytesRead, 
                       bsl::size_t* numBytesWritten, 
                       int          mode)
{
    int rc = 0;

    *numBytesRead = 0;
    *numBytesWritten = 0;

    uInt availIn0  = d_deflaterStream.avail_in;
    uInt availOut0 = d_deflaterStream.avail_out;
    
    rc = ::deflate(&d_deflaterStream, mode);

    uInt availIn1  = d_deflaterStream.avail_in;
    uInt availOut1 = d_deflaterStream.avail_out;
    
    BSLS_ASSERT(availIn0 >= availIn1);
    BSLS_ASSERT(availOut0 >= availOut1);

    uInt availInDiff = availIn0 - availIn1;
    uInt availOutDiff = availOut0 - availOut1;
    
    *numBytesRead    = availInDiff;
    *numBytesWritten = availOutDiff;

    d_deflaterBufferSize += availOutDiff;

    return rc;
}

NTCCFG_INLINE
void Zlib::inflateOverflow(bdlbb::Blob* result)
{
    if (d_inflaterBufferSize != 0) {
        this->inflateBufferCommit(result);
    }

    d_inflaterBuffer.reset();
    d_inflaterBufferSize = 0;

    d_dataPool_sp->createIncomingBlobBuffer(&d_inflaterBuffer);
    
    d_inflaterStream.next_out = 
        reinterpret_cast<unsigned char*>(d_inflaterBuffer.data());
    d_inflaterStream.avail_out = d_inflaterBuffer.size();
}

NTCCFG_INLINE
void Zlib::inflateBufferCommit(bdlbb::Blob* result)
{
    if (d_inflaterBufferSize != 0) {
        d_inflaterBuffer.setSize(d_inflaterBufferSize);

        BSLS_ASSERT(d_inflaterBuffer.buffer().get() != 0);
        BSLS_ASSERT(d_inflaterBuffer.size() > 0);

        result->appendDataBuffer(d_inflaterBuffer);
        
        d_inflaterBuffer.reset();
        d_inflaterBufferSize = 0;

        d_inflaterStream.next_out = 0;
        d_inflaterStream.avail_out = 0;
    }
}

NTCCFG_INLINE
int Zlib::inflateCycle(bsl::size_t* numBytesRead, 
                       bsl::size_t* numBytesWritten, 
                       int          mode)
{
    int rc = 0;

    *numBytesRead = 0;
    *numBytesWritten = 0;

    uInt availIn0  = d_inflaterStream.avail_in;
    uInt availOut0 = d_inflaterStream.avail_out;
    
    rc = ::inflate(&d_inflaterStream, mode);

    uInt availIn1  = d_inflaterStream.avail_in;
    uInt availOut1 = d_inflaterStream.avail_out;
    
    BSLS_ASSERT(availIn0 >= availIn1);
    BSLS_ASSERT(availOut0 >= availOut1);

    uInt availInDiff = availIn0 - availIn1;
    uInt availOutDiff = availOut0 - availOut1;
    
    *numBytesRead    = availInDiff;
    *numBytesWritten = availOutDiff;

    d_inflaterBufferSize += availOutDiff;

    return rc;
}

ntsa::Error Zlib::deflateBegin(ntca::DeflateContext*       context,
                               bdlbb::Blob*                result,
                               const ntca::DeflateOptions& options)
{
    d_deflaterStream.next_in  = 0;
    d_deflaterStream.avail_in = 0;
    d_deflaterStream.total_in = 0;

    d_deflaterStream.next_out  = 0;
    d_deflaterStream.avail_out = 0;
    d_deflaterStream.total_out = 0;
    d_deflaterStream.adler     = 0;

    return ntsa::Error();
}

ntsa::Error Zlib::deflateNext(ntca::DeflateContext*       context,
                              bdlbb::Blob*                result,
                              const bsl::uint8_t*         data,
                              bsl::size_t                 size,
                              const ntca::DeflateOptions& options)
{
    int rc = 0;

    bsl::size_t totalBytesWritten = 0;
    bsl::size_t totalBytesRead = 0;

    d_deflaterStream.next_in  = const_cast<bsl::uint8_t*>(data);
    d_deflaterStream.avail_in = size;

    while (d_deflaterStream.avail_in != 0) {
        if (d_deflaterStream.avail_out == 0) {
            this->deflateOverflow(result);
        }

        bsl::size_t numBytesRead    = 0;
        bsl::size_t numBytesWritten = 0;

        rc = this->deflateCycle(&numBytesRead, &numBytesWritten, Z_NO_FLUSH);

        totalBytesRead += numBytesRead;
        totalBytesWritten += numBytesWritten;

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
    int rc = 0;

    bsl::size_t totalBytesWritten = 0;
    bsl::size_t totalBytesRead = 0;

    while (true) {
        if (d_deflaterStream.avail_out == 0) {
            this->deflateOverflow(result);
        }

        bsl::size_t numBytesRead    = 0;
        bsl::size_t numBytesWritten = 0;

        rc = this->deflateCycle(&numBytesRead, &numBytesWritten, Z_SYNC_FLUSH);

        totalBytesRead += numBytesRead;
        totalBytesWritten += numBytesWritten;

        if (rc == Z_BUF_ERROR && numBytesRead == 0 && numBytesWritten == 0) {
            if (d_deflaterBufferSize != 0) {
                this->deflateBufferCommit(result);
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
                   &d_deflaterStream.adler, 
                   sizeof d_deflaterStream.adler);

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

    BSLS_ASSERT(d_inflaterStream.next_in  == 0);
    BSLS_ASSERT(d_inflaterStream.avail_in == 0);

    d_inflaterStream.next_in  = const_cast<bsl::uint8_t*>(data);
    d_inflaterStream.avail_in = size;

    while (d_inflaterStream.avail_in != 0) {
        if (d_inflaterStream.avail_out == 0) {
            this->inflateOverflow(result);
        }

        bsl::size_t numBytesRead    = 0;
        bsl::size_t numBytesWritten = 0;

        rc = this->inflateCycle(&numBytesRead, &numBytesWritten, Z_NO_FLUSH);

        totalBytesRead += numBytesRead;
        totalBytesWritten += numBytesWritten;

        if (rc != Z_OK && rc != Z_BUF_ERROR) {
            return this->translateError(rc, "inflate");
        }
    }

    d_inflaterStream.next_in  = 0;
    d_inflaterStream.avail_in = 0;

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

    BSLS_ASSERT(d_inflaterStream.next_in  == 0);
    BSLS_ASSERT(d_inflaterStream.avail_in == 0);

    while (true) {
        if (d_inflaterStream.avail_out == 0) {
            this->inflateOverflow(result);
        }

        bsl::size_t numBytesRead    = 0;
        bsl::size_t numBytesWritten = 0;

        rc = this->inflateCycle(&numBytesRead, &numBytesWritten, Z_SYNC_FLUSH);

        totalBytesRead += numBytesRead;
        totalBytesWritten += numBytesWritten;

        if (rc == Z_BUF_ERROR && numBytesRead == 0 && numBytesWritten == 0) {
            if (d_inflaterBufferSize != 0) {
                this->inflateBufferCommit(result);
            }

            break;
        }

        if (rc != Z_OK) {
            return this->translateError(rc, "inflate");
        }
    }

    d_inflaterStream.next_in  = 0;
    d_inflaterStream.avail_in = 0;

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
: d_deflaterBuffer()
, d_deflaterBufferSize(0)
, d_inflaterBuffer()
, d_inflaterBufferSize(0)
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

    bsl::memset(&d_deflaterStream, 0, sizeof d_deflaterStream);
    bsl::memset(&d_inflaterStream, 0, sizeof d_inflaterStream);

    d_deflaterStream.zalloc = &Zlib::allocate;
    d_deflaterStream.zfree  = &Zlib::free;
    d_deflaterStream.opaque = d_allocator_p;

    rc = deflateInit(&d_deflaterStream, d_level);
    BSLS_ASSERT_OPT(rc == Z_OK);

    d_inflaterStream.zalloc = &Zlib::allocate;
    d_inflaterStream.zfree  = &Zlib::free;
    d_inflaterStream.opaque = d_allocator_p;

    rc = inflateInit(&d_inflaterStream);
    BSLS_ASSERT_OPT(rc == Z_OK);
}

Zlib::~Zlib()
{
    int rc;

    d_inflaterStream.next_in  = 0;
    d_inflaterStream.avail_in = 0;
    d_inflaterStream.total_in = 0;

    d_inflaterStream.next_out  = 0;
    d_inflaterStream.avail_out = 0;
    d_inflaterStream.total_out = 0;
    d_inflaterStream.adler     = 0;

    rc = ::inflateReset(&d_inflaterStream);
    if (rc != Z_OK) {
        translateError(rc, "reset inflater");
    }

    rc = ::inflateEnd(&d_inflaterStream);
    if (rc != Z_OK) {
        translateError(rc, "close inflater");
    }

    d_deflaterStream.next_in  = 0;
    d_deflaterStream.avail_in = 0;
    d_deflaterStream.total_in = 0;

    d_deflaterStream.next_out  = 0;
    d_deflaterStream.avail_out = 0;
    d_deflaterStream.total_out = 0;
    d_deflaterStream.adler     = 0;

    rc = ::deflateReset(&d_deflaterStream);
    if (rc != Z_OK) {
        translateError(rc, "reset deflater");
    }

    rc = ::deflateEnd(&d_deflaterStream);
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
