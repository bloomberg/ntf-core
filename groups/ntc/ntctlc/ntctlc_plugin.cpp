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
BSLS_IDENT_RCSID(ntctlc_plugin_cpp, "$Id$ $CSID$")

#include <ntca_checksum.h>
#include <ntcd_compression.h>
#include <ntci_log.h>
#include <ntcs_blobutil.h>
#include <ntcs_datapool.h>
#include <ntcs_plugin.h>
#include <ntsa_error.h>
#include <ball_log.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobstreambuf.h>
#include <bdlbb_blobutil.h>
#include <bslma_default.h>
#include <bslma_newdeleteallocator.h>
#include <bslmt_once.h>
#include <bsls_assert.h>
#include <bslalg_numericformatterutil.h>

#if NTC_BUILD_WITH_LZ4
#define LZ4_STATIC_LINKING_ONLY
#define LZ4F_STATIC_LINKING_ONLY
#include <lz4.h>
#include <lz4frame.h>
#endif

#if NTC_BUILD_WITH_ZSTD
#define ZSTD_STATIC_LINKING_ONLY
#include <zstd.h>
#include <zstd_errors.h>
#endif

#if NTC_BUILD_WITH_ZLIB
#include <zlib.h>
#endif

// Uncomment and set to 1 to log the hex dump of all deflated and inflated 
// data.
#define NTCTLC_PLUGIN_LOG_HEX_DUMP 0

#if NTCTLC_PLUGIN_LOG_HEX_DUMP

#define NTCTLC_PLUGIN_LOG_DEFLATED_CHAR_BUFFER(buffer, size) \
    do { \
        BALL_LOG_TRACE << "Deflated:\n" \
                       << bdlb::PrintStringHexDumper( \
                            reinterpret_cast<const char*>((buffer)), \
                            static_cast<int>(size)) \
                       << BALL_LOG_END; \
    } while (false)

#define NTCTLC_PLUGIN_LOG_DEFLATED_BLOB_BUFFER(blobBuffer) \
    do { \
        BALL_LOG_TRACE << "Deflated:\n" \
                       << bdlb::PrintStringHexDumper( \
                            (blobBuffer).data(), \
                            (blobBuffer).size()) \
                       << BALL_LOG_END; \
    } while (false)

#define NTCTLC_PLUGIN_LOG_INFLATED_CHAR_BUFFER(buffer, size) \
    do { \
        BALL_LOG_TRACE << "Inflated:\n" \
                       << bdlb::PrintStringHexDumper( \
                            reinterpret_cast<const char*>((buffer)), \
                            static_cast<int>(size)) \
                       << BALL_LOG_END; \
    } while (false)

#define NTCTLC_PLUGIN_LOG_INFLATED_BLOB_BUFFER(blobBuffer) \
    do { \
        BALL_LOG_TRACE << "Inflated:\n" \
                       << bdlb::PrintStringHexDumper( \
                            (blobBuffer).data(), \
                            (blobBuffer).size()) \
                       << BALL_LOG_END; \
    } while (false)

#else

#define NTCTLC_PLUGIN_LOG_DEFLATED_CHAR_BUFFER(buffer, size)
#define NTCTLC_PLUGIN_LOG_DEFLATED_BLOB_BUFFER(blobBuffer)
#define NTCTLC_PLUGIN_LOG_INFLATED_CHAR_BUFFER(buffer, size)
#define NTCTLC_PLUGIN_LOG_INFLATED_BLOB_BUFFER(blobBuffer)

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
    /// Link each block.
    static const bool k_BLOCK_LINKED = true;

    /// Flag indicating that a checksum should be calculated for the content
    /// of each frame when deflating.
    static const bool k_CHECKSUM_CONTENT = true;

    /// Flag indicating that a checksum should be calculated for the content
    /// of each block when deflating.
    static const bool k_CHECKSUM_BLOCK = true;

    /// Flag indicating that checksums should be verified by the inflater.
    static const bool k_CHECKSUM_VERIFY = true;

    LZ4F_cctx*                      d_deflaterContext_p;
    bdlbb::BlobBuffer               d_deflaterBuffer;
    bsl::size_t                     d_deflaterBufferSize;
    bsl::uint8_t*                   d_deflaterArena;
    bsl::size_t                     d_deflaterArenaCapacity;
    LZ4F_compressOptions_t          d_deflaterOptions;
    LZ4F_dctx*                      d_inflaterContext_p;
    bdlbb::BlobBuffer               d_inflaterBuffer;
    bsl::size_t                     d_inflaterBufferSize;
    LZ4F_decompressOptions_t        d_inflaterOptions;
    LZ4F_preferences_t              d_preferences;
    int                             d_level;
    bsl::shared_ptr<ntci::DataPool> d_dataPool_sp;
    ntca::CompressionConfig         d_config;
    bslma::Allocator*               d_allocator_p;

    BALL_LOG_SET_CLASS_CATEGORY("NTF.TLC.LZ4");

  private:
    Lz4(const Lz4&) BSLS_KEYWORD_DELETED;
    Lz4& operator=(const Lz4&) BSLS_KEYWORD_DELETED;

  private:
    /// Create the deflater. Return the error.
    ntsa::Error deflateCreate();

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


    /// Handle the lack of lack of output capacity available to the deflater
    /// by committing the previous buffer, if any, and allocating a new one.
    void deflateOverflow(bdlbb::Blob* result);

    /// Commit the deflater buffer, if any bytes have been been written to it,
    /// to the specified 'result'.
    void deflateCommit(bdlbb::Blob* result);

    /// Feed the input bytes available to the deflate algorithm and write
    /// any output bytes to the output buffer. Return the error.
    LZ4F_errorCode_t deflateCycle(bsl::size_t* numBytesRead,
                                  bsl::size_t* numBytesWritten);    

    /// Reset the deflater to prepare for a new frame. Return the error.
    ntsa::Error deflateReset();

    /// Destroy the deflater. Return the error.
    ntsa::Error deflateDestroy();

    /// Create the inflater. Return the error.
    ntsa::Error inflateCreate();

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

    /// Handle the lack of lack of output capacity available to the inflater
    /// by committing the previous buffer, if any, and allocating a new one.
    void inflateOverflow(bdlbb::Blob* result);

    /// Commit the inflater buffer, if any bytes have been been written to it,
    /// to the specified 'result'.
    void inflateCommit(bdlbb::Blob* result);

    /// Feed the input bytes available to the inflate algorithm and write
    /// any output bytes to the output buffer. Return the error.
    LZ4F_errorCode_t inflateCycle(bsl::size_t* numBytesRead,
                                  bsl::size_t* numBytesWritten);

    /// Reset the inflater to prepare for a new frame. Return the error.
    ntsa::Error inflateReset();

    /// Destroy the deflater. Return the error.
    ntsa::Error inflateDestroy();

    /// Allocate the specified 'size' number of bytes. The specified 'opaque'
    /// field points to the allocator object.
    static void* allocate(void* opaque, bsl::size_t size);

    /// Free memory pointed to by the specified 'address'. The specified
    /// 'opaque' field points to the allocator object.
    static void free(void* opaque, void* address);

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
    ZSTD_CCtx*                      d_deflaterContext_p;
    ZSTD_inBuffer                   d_deflaterInput;
    ZSTD_outBuffer                  d_deflaterOutput;
    bdlbb::BlobBuffer               d_deflaterBuffer;
    bsl::size_t                     d_deflaterBufferSize;
    ZSTD_DCtx*                      d_inflaterContext_p;
    ZSTD_inBuffer                   d_inflaterInput;
    ZSTD_outBuffer                  d_inflaterOutput;
    bdlbb::BlobBuffer               d_inflaterBuffer;
    bsl::size_t                     d_inflaterBufferSize;
    int                             d_level;
    bsl::shared_ptr<ntci::DataPool> d_dataPool_sp;
    ntca::CompressionConfig         d_config;
    bslma::Allocator*               d_allocator_p;

    BALL_LOG_SET_CLASS_CATEGORY("NTF.TLC.ZSTD");

  private:
    Zstd(const Zstd&) BSLS_KEYWORD_DELETED;
    Zstd& operator=(const Zstd&) BSLS_KEYWORD_DELETED;

  private:
    /// Create the deflater. Return the error.
    ntsa::Error deflateCreate();

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

    /// Handle the lack of lack of output capacity available to the deflater
    /// by committing the previous buffer, if any, and allocating a new one.
    void deflateOverflow(bdlbb::Blob* result);

    /// Commit the deflater buffer, if any bytes have been been written to it,
    /// to the specified 'result'.
    void deflateCommit(bdlbb::Blob* result);

    /// Feed the input bytes available to the deflate algorithm and write
    /// any output bytes to the output buffer. Return the error.
    bsl::size_t deflateCycle(bsl::size_t*      numBytesRead,
                             bsl::size_t*      numBytesWritten,
                             ZSTD_EndDirective mode);

    /// Reset the deflater to prepare for a new frame. Return the error.
    ntsa::Error deflateReset();

    /// Destroy the deflater. Return the error.
    ntsa::Error deflateDestroy();

    /// Create the inflater. Return the error.
    ntsa::Error inflateCreate();

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

    /// Handle the lack of lack of output capacity available to the inflater
    /// by committing the previous buffer, if any, and allocating a new one.
    void inflateOverflow(bdlbb::Blob* result);

    /// Commit the inflater buffer, if any bytes have been been written to it,
    /// to the specified 'result'.
    void inflateCommit(bdlbb::Blob* result);

    /// Feed the input bytes available to the inflate algorithm and write
    /// any output bytes to the output buffer. Return the error.
    bsl::size_t inflateCycle(bsl::size_t* numBytesRead,
                             bsl::size_t* numBytesWritten);

    /// Reset the inflater to prepare for a new frame. Return the error.
    ntsa::Error inflateReset();

    /// Destroy the deflater. Return the error.
    ntsa::Error inflateDestroy();

    /// Allocate the specified 'size' number of bytes. The specified 'opaque'
    /// field points to the allocator object.
    static void* allocate(void* opaque, bsl::size_t size);

    /// Free memory pointed to by the specified 'address'. The specified
    /// 'opaque' field points to the allocator object.
    static void free(void* opaque, void* address);

    /// Return the string description of the specified 'error' code.
    static const char* describeError(bsl::size_t error);

    /// Return the error associated with the specified zstd 'error' code
    /// detected when performing the specified 'operation'.
    static ntsa::Error translateError(bsl::size_t error, 
                                      const char* operation);

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
    bsl::uint64_t                   d_deflaterGeneration;
    z_stream                        d_inflaterStream;
    bdlbb::BlobBuffer               d_inflaterBuffer;
    bsl::size_t                     d_inflaterBufferSize;
    bsl::uint64_t                   d_inflaterGeneration;
    int                             d_level;
    bsl::shared_ptr<ntci::DataPool> d_dataPool_sp;
    ntca::CompressionConfig         d_config;
    bslma::Allocator*               d_allocator_p;

    BALL_LOG_SET_CLASS_CATEGORY("NTF.TLC.ZLIB");

  private:
    Zlib(const Zlib&) BSLS_KEYWORD_DELETED;
    Zlib& operator=(const Zlib&) BSLS_KEYWORD_DELETED;

  private:
    /// Create the deflater. Return the error.
    ntsa::Error deflateCreate();

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

    /// Handle the lack of lack of output capacity available to the deflater
    /// by committing the previous buffer, if any, and allocating a new one.
    void deflateOverflow(bdlbb::Blob* result);

    /// Commit the deflater buffer, if any bytes have been been written to it,
    /// to the specified 'result'.
    void deflateCommit(bdlbb::Blob* result);

    /// Feed the input bytes available to the deflate algorithm and write
    /// any output bytes to the output buffer. Return the error.
    int deflateCycle(bsl::size_t* numBytesRead,
                     bsl::size_t* numBytesWritten,
                     int          mode);

    /// Reset the deflater to prepare for a new frame. Return the error.
    ntsa::Error deflateReset();

    /// Destroy the deflater. Return the error.
    ntsa::Error deflateDestroy();

    /// Create the inflater. Return the error.
    ntsa::Error inflateCreate();

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

    /// Handle the lack of lack of output capacity available to the inflater
    /// by committing the previous buffer, if any, and allocating a new one.
    void inflateOverflow(bdlbb::Blob* result);

    /// Commit the inflater buffer, if any bytes have been been written to it,
    /// to the specified 'result'.
    void inflateCommit(bdlbb::Blob* result);

    /// Feed the input bytes available to the inflate algorithm and write
    /// any output bytes to the output buffer. Return the error.
    int inflateCycle(bsl::size_t* numBytesRead,
                     bsl::size_t* numBytesWritten,
                     int          mode);

    /// Reset the inflater to prepare for a new frame. Return the error.
    ntsa::Error inflateReset();

    /// Destroy the deflater. Return the error.
    ntsa::Error inflateDestroy();

    /// Allocate the specified 'number' of elements of the specified 'size' in
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
    gz_header                       d_deflaterHeader;
    z_stream                        d_deflaterStream;
    bdlbb::BlobBuffer               d_deflaterBuffer;
    bsl::size_t                     d_deflaterBufferSize;
    char                            d_deflaterEntityName[128];
    char                            d_deflaterEntityComment[128];
    bsl::uint64_t                   d_deflaterGeneration;
    gz_header                       d_inflaterHeader;
    z_stream                        d_inflaterStream;
    bdlbb::BlobBuffer               d_inflaterBuffer;
    bsl::size_t                     d_inflaterBufferSize;
    char                            d_inflaterEntityName[128];
    char                            d_inflaterEntityComment[128];
    bsl::uint64_t                   d_inflaterGeneration;
    int                             d_level;
    bsl::shared_ptr<ntci::DataPool> d_dataPool_sp;
    ntca::CompressionConfig         d_config;
    bslma::Allocator*               d_allocator_p;

    BALL_LOG_SET_CLASS_CATEGORY("NTF.TLC.GZIP");

  private:
    Gzip(const Gzip&) BSLS_KEYWORD_DELETED;
    Gzip& operator=(const Gzip&) BSLS_KEYWORD_DELETED;

  private:
    /// Create the deflater. Return the error.
    ntsa::Error deflateCreate();

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

    /// Handle the lack of lack of output capacity available to the deflater
    /// by committing the previous buffer, if any, and allocating a new one.
    void deflateOverflow(bdlbb::Blob* result);

    /// Commit the deflater buffer, if any bytes have been been written to it,
    /// to the specified 'result'.
    void deflateCommit(bdlbb::Blob* result);

    /// Feed the input bytes available to the deflate algorithm and write
    /// any output bytes to the output buffer. Return the error.
    int deflateCycle(bsl::size_t* numBytesRead,
                     bsl::size_t* numBytesWritten,
                     int          mode);

    /// Reset the deflater to prepare for a new frame. Return the error.
    ntsa::Error deflateReset();

    /// Destroy the deflater. Return the error.
    ntsa::Error deflateDestroy();

    /// Create the inflater. Return the error.
    ntsa::Error inflateCreate();

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

    /// Handle the lack of lack of output capacity available to the inflater
    /// by committing the previous buffer, if any, and allocating a new one.
    void inflateOverflow(bdlbb::Blob* result);

    /// Commit the inflater buffer, if any bytes have been been written to it,
    /// to the specified 'result'.
    void inflateCommit(bdlbb::Blob* result);

    /// Feed the input bytes available to the inflate algorithm and write
    /// any output bytes to the output buffer. Return the error.
    int inflateCycle(bsl::size_t* numBytesRead,
                     bsl::size_t* numBytesWritten,
                     int          mode);

    /// Reset the inflater to prepare for a new frame. Return the error.
    ntsa::Error inflateReset();

    /// Destroy the deflater. Return the error.
    ntsa::Error inflateDestroy();

    /// Allocate the specified 'number' of elements of the specified 'size' in
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
    Gzip(const ntca::CompressionConfig&         configuration,
         const bsl::shared_ptr<ntci::DataPool>& dataPool,
         bslma::Allocator*                      basicAllocator = 0);

    /// Destroy this object.
    ~Gzip() BSLS_KEYWORD_OVERRIDE;

    /// Return the compression type implemented by this mechanism.
    ntca::CompressionType::Value type() const BSLS_KEYWORD_OVERRIDE;
};

#endif

/// Provide a factory for a mechanism to deflate and inflate a data stream.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntctlc
class CompressionDriver : public ntci::CompressionDriver
{
    bslma::Allocator* d_allocator_p;

  private:
    CompressionDriver(const CompressionDriver&) BSLS_KEYWORD_DELETED;
    CompressionDriver& operator=(const CompressionDriver&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new compression driver. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit CompressionDriver(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~CompressionDriver() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new compression mechanism with the
    /// specified 'configuration'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error createCompression(bsl::shared_ptr<ntci::Compression>* result,
                                  const ntca::CompressionConfig& configuration,
                                  bslma::Allocator* basicAllocator = 0)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new compression mechanism with the
    /// specified 'options'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error createCompression(
        bsl::shared_ptr<ntci::Compression>*              result,
        const ntca::CompressionConfig&                   configuration,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new compression mechanism with the
    /// specified 'configuration'. Allocate data containers using the specified
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used. Return the error.
    ntsa::Error createCompression(
        bsl::shared_ptr<ntci::Compression>*    result,
        const ntca::CompressionConfig&         configuration,
        const bsl::shared_ptr<ntci::DataPool>& dataPool,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;
};

#if NTC_BUILD_WITH_LZ4

#define NTCTLC_PLUGIN_LZ4_VERSION_MAKE(major, minor, patch) \
    (((major) * 100 * 100) + ((minor) * 100) + (patch))

#define NTCTLC_PLUGIN_LZ4_VERSION_NUMBER LZ4_VERSION_NUMBER

NTCCFG_INLINE
ntsa::Error Lz4::deflateCreate()
{
    NTCI_LOG_CONTEXT();

    LZ4F_errorCode_t errorCode = 0;
    NTCCFG_WARNING_UNUSED(errorCode);

    if (d_config.goal().has_value()) {
        switch (d_config.goal().value()) {
        case ntca::CompressionGoal::e_BEST_SIZE:
            d_level = 12;
            break;
        case ntca::CompressionGoal::e_BETTER_SIZE:
            d_level = 8;
            break;
        case ntca::CompressionGoal::e_BALANCED:
            d_level = 3;
            break;
        case ntca::CompressionGoal::e_BETTER_SPEED:
            d_level = 2;
            break;
        case ntca::CompressionGoal::e_BEST_SPEED:
            d_level = 1;
            break;
        default:
            break;
        }
    }

#if defined(LZ4F_STATIC_LINKING_ONLY) && \
    NTCTLC_PLUGIN_LZ4_VERSION_NUMBER >= NTCTLC_PLUGIN_LZ4_VERSION_MAKE(1, 9, 4)

    LZ4F_CustomMem memoryManager;
    bsl::memset(&memoryManager, 0, sizeof memoryManager);

    memoryManager.customAlloc = &Lz4::allocate;
    memoryManager.customFree  = &Lz4::free;
    memoryManager.opaqueState = d_allocator_p;

    d_deflaterContext_p = LZ4F_createCompressionContext_advanced(
        memoryManager, LZ4F_VERSION);
    if (d_deflaterContext_p == 0) {
        NTCI_LOG_ERROR("Failed to create compression context");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    
#else

    errorCode =
        LZ4F_createCompressionContext(&d_deflaterContext_p, LZ4F_VERSION);
    if (LZ4F_isError(errorCode)) {
        NTCI_LOG_ERROR("Failed to create compression context: %s",
                       LZ4F_getErrorName(errorCode));
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

#endif

    bsl::memset(&d_deflaterOptions, 0, sizeof d_deflaterOptions);
    d_deflaterOptions.stableSrc = 0;

    bsl::memset(&d_preferences, 0, sizeof d_preferences);

    d_preferences.frameInfo.blockSizeID = LZ4F_max64KB;

    if (k_BLOCK_LINKED) {
        d_preferences.frameInfo.blockMode = LZ4F_blockLinked;
    }
    else {
        d_preferences.frameInfo.blockMode = LZ4F_blockIndependent;
    }

    if (k_CHECKSUM_CONTENT) {
        d_preferences.frameInfo.contentChecksumFlag =
            LZ4F_contentChecksumEnabled;
    }
    else {
        d_preferences.frameInfo.contentChecksumFlag = 
            LZ4F_noContentChecksum;
    }

    d_preferences.frameInfo.frameType   = LZ4F_frame;
    d_preferences.frameInfo.contentSize = 0;
    d_preferences.frameInfo.dictID      = 0;
    
    if (k_CHECKSUM_BLOCK) {
        d_preferences.frameInfo.blockChecksumFlag = 
            LZ4F_blockChecksumEnabled;
    }
    else {
        d_preferences.frameInfo.blockChecksumFlag = LZ4F_noBlockChecksum;
    }

    d_preferences.compressionLevel = d_level;
    d_preferences.autoFlush        = 1;
    d_preferences.favorDecSpeed    = 0;

    return ntsa::Error();
}

ntsa::Error Lz4::deflateBegin(ntca::DeflateContext*       context,
                              bdlbb::Blob*                result,
                              const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    NTCI_LOG_CONTEXT();

    LZ4F_errorCode_t errorCode = 0;

    char header[LZ4F_HEADER_SIZE_MAX];
    bsl::memset(header, 0, sizeof header);

    errorCode = LZ4F_compressBegin(d_deflaterContext_p,
                                   header,
                                   sizeof header,
                                   &d_preferences);
    if (LZ4F_isError(errorCode)) {
        NTCI_LOG_ERROR("Failed to begin compression frame: %s",
                       LZ4F_getErrorName(errorCode));
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t numBytesRead    = 0;
    const bsl::size_t numBytesWritten = static_cast<bsl::size_t>(errorCode);

    NTCTLC_PLUGIN_LOG_DEFLATED_CHAR_BUFFER(header, numBytesWritten);

    ntcs::BlobUtil::append(result, header, numBytesWritten);

    context->setBytesRead(context->bytesRead() + numBytesRead);
    context->setBytesWritten(context->bytesWritten() + numBytesWritten);

    return ntsa::Error();
}

ntsa::Error Lz4::deflateNext(ntca::DeflateContext*       context,
                             bdlbb::Blob*                result,
                             const bsl::uint8_t*         data,
                             bsl::size_t                 size,
                             const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    NTCI_LOG_CONTEXT();

    LZ4F_errorCode_t errorCode = 0;

    const bsl::size_t k_OVERHEAD = 64;

    const bsl::uint8_t* source = data;
    const bsl::uint8_t* sourceEnd = data + size;

    bsl::size_t totalBytesRead = 0;
    bsl::size_t totalBytesWritten = 0;

    bool permitSlowPath = false;

    while (source < sourceEnd) {
        void* destination = d_deflaterBuffer.data() + d_deflaterBufferSize;

        bsl::size_t destinationCapacity = 
            d_deflaterBuffer.size() - d_deflaterBufferSize;

        if (destinationCapacity == 0) {
            this->deflateOverflow(result);
            continue;
        }

        bsl::size_t sourceSize = static_cast<bsl::size_t>(sourceEnd - source);
        if (sourceSize + k_OVERHEAD > destinationCapacity) {
            if (destinationCapacity <= k_OVERHEAD) {
                // Slow path: We cannot reduce the amount we feed into the
                // compressor enough to guarantee its worst-case deflated size
                // is less than the available blob buffer capacity. Fall back
                // to deflating into a contiguous buffer on the stack then 
                // copy that buffer into the output blob.

                this->deflateOverflow(result);

                if (!permitSlowPath) {
                    permitSlowPath = true;
                    continue;
                }

                if (d_deflaterArena == 0) {
                    d_deflaterArenaCapacity = (1024 * 64) + k_OVERHEAD;
                    d_deflaterArena = 
                        reinterpret_cast<bsl::uint8_t*>(
                            d_allocator_p->allocate(d_deflaterArenaCapacity));
                }

                if (sourceSize > d_deflaterArenaCapacity - k_OVERHEAD) {
                    sourceSize = d_deflaterArenaCapacity - k_OVERHEAD;
                }

                NTCI_LOG_WARN("Deflating %d bytes in the slow path", 
                              (int)(sourceSize));

                errorCode = LZ4F_compressUpdate(d_deflaterContext_p,
                    d_deflaterArena,
                    d_deflaterArenaCapacity,
                    source,
                    sourceSize,
                    &d_deflaterOptions);

                if (LZ4F_isError(errorCode)) {
                    if (LZ4F_getErrorCode(errorCode) == 
                        LZ4F_ERROR_dstMaxSize_tooSmall) 
                    {
                        const bsl::size_t destinationCapacityRequired = 
                            LZ4F_compressBound(sourceSize, &d_preferences);
                        NTCCFG_WARNING_UNUSED(destinationCapacityRequired);

                        NTCI_LOG_ERROR(
                            "Failed to update compression frame: "
                            "destination capacity too small: "
                            "expected at least %d, found %d",
                            (int)(destinationCapacityRequired),
                            (int)(d_deflaterArenaCapacity));
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                    else {
                        NTCI_LOG_ERROR(
                            "Failed to update compression frame: %s",
                            LZ4F_getErrorName(errorCode));
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }

                const bsl::size_t numBytesRead    = sourceSize;
                const bsl::size_t numBytesWritten = 
                    static_cast<bsl::size_t>(errorCode);

                BSLS_ASSERT_OPT(numBytesRead > 0);
                BSLS_ASSERT_OPT(numBytesWritten > 0);

                NTCTLC_PLUGIN_LOG_DEFLATED_CHAR_BUFFER(
                    d_deflaterArena, numBytesWritten);

                ntcs::BlobUtil::append(
                    result, d_deflaterArena, numBytesWritten);

                totalBytesRead += numBytesRead;
                totalBytesWritten += numBytesWritten;

                source += numBytesRead;
                continue;
            }
            else {
                sourceSize = destinationCapacity - k_OVERHEAD;
            }
        }

        // const bsl::size_t destinationCapacityRequired = LZ4F_compressBound(
        //     sourceSize, &d_preferences);
        // NTCCFG_WARNING_UNUSED(destinationCapacityRequired);
        // BSLS_ASSERT(destinationCapacity >= destinationCapacityRequired);

        errorCode = LZ4F_compressUpdate(d_deflaterContext_p,
            destination,
            destinationCapacity,
            source,
            sourceSize,
            &d_deflaterOptions);

        if (LZ4F_isError(errorCode)) {
            NTCI_LOG_ERROR("Failed to update compression frame: %s, sourceSize = %d, destinationCapacity = %d, destinationCapacityRequired = %d",
            LZ4F_getErrorName(errorCode),
            (int)(sourceSize),
            (int)(destinationCapacity),
            (int)(LZ4F_compressBound(sourceSize, &d_preferences)));
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const bsl::size_t numBytesRead    = sourceSize;
        const bsl::size_t numBytesWritten = 
            static_cast<bsl::size_t>(errorCode);

        BSLS_ASSERT_OPT(numBytesRead > 0);
        BSLS_ASSERT_OPT(numBytesWritten > 0);

        totalBytesRead += numBytesRead;
        totalBytesWritten += numBytesWritten;

        source += numBytesRead;
        d_deflaterBufferSize += numBytesWritten;
    }

    context->setBytesRead(context->bytesRead() + totalBytesRead);
    context->setBytesWritten(context->bytesWritten() + totalBytesWritten);

    return ntsa::Error();
}

ntsa::Error Lz4::deflateEnd(ntca::DeflateContext*       context,
                            bdlbb::Blob*                result,
                            const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    NTCI_LOG_CONTEXT();

    LZ4F_errorCode_t errorCode = 0;

    if (d_deflaterBufferSize != 0) {
        this->deflateCommit(result);
    }

    char footer[16];

    errorCode = LZ4F_compressEnd(d_deflaterContext_p,
                                 footer,
                                 sizeof footer,
                                 &d_deflaterOptions);
    if (LZ4F_isError(errorCode)) {
        NTCI_LOG_ERROR("Failed to end compression frame: %s",
                       LZ4F_getErrorName(errorCode));
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t numBytesRead    = 0;
    const bsl::size_t numBytesWritten = static_cast<bsl::size_t>(errorCode);

    NTCTLC_PLUGIN_LOG_DEFLATED_CHAR_BUFFER(footer, numBytesWritten);

    ntcs::BlobUtil::append(result, footer, numBytesWritten);

    context->setCompressionType(ntca::CompressionType::e_LZ4);
    context->setBytesRead(context->bytesRead() + numBytesRead);
    context->setBytesWritten(context->bytesWritten() + numBytesWritten);

    return ntsa::Error();
}

NTCCFG_INLINE
void Lz4::deflateOverflow(bdlbb::Blob* result)
{
    if (d_deflaterBufferSize != 0) {
        this->deflateCommit(result);
    }

    d_deflaterBuffer.reset();
    d_deflaterBufferSize = 0;

    d_dataPool_sp->createOutgoingBlobBuffer(&d_deflaterBuffer);
}

NTCCFG_INLINE
void Lz4::deflateCommit(bdlbb::Blob* result)
{
    if (d_deflaterBufferSize != 0) {
        d_deflaterBuffer.setSize(static_cast<int>(d_deflaterBufferSize));

        BSLS_ASSERT(d_deflaterBuffer.buffer().get() != 0);
        BSLS_ASSERT(d_deflaterBuffer.size() > 0);

        NTCTLC_PLUGIN_LOG_DEFLATED_BLOB_BUFFER(d_deflaterBuffer);

        result->appendDataBuffer(d_deflaterBuffer);

        d_deflaterBuffer.reset();
        d_deflaterBufferSize = 0;
    }
}

NTCCFG_INLINE
LZ4F_errorCode_t Lz4::deflateCycle(bsl::size_t* numBytesRead,
                                   bsl::size_t* numBytesWritten)
{
    NTCCFG_WARNING_UNUSED(numBytesRead);
    NTCCFG_WARNING_UNUSED(numBytesWritten);

    return 0;
}

NTCCFG_INLINE
ntsa::Error Lz4::deflateReset()
{
    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error Lz4::deflateDestroy()
{
    LZ4F_freeCompressionContext(d_deflaterContext_p);
    d_deflaterContext_p = 0;

    d_deflaterBuffer.reset();

    if (d_deflaterArena != 0) {
        d_allocator_p->deleteObject(d_deflaterArena);
        d_deflaterArena = 0;
    }

    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error Lz4::inflateCreate()
{
    NTCI_LOG_CONTEXT();

    LZ4F_errorCode_t errorCode = 0;
    NTCCFG_WARNING_UNUSED(errorCode);

#if defined(LZ4F_STATIC_LINKING_ONLY)  && \
    NTCTLC_PLUGIN_LZ4_VERSION_NUMBER >= NTCTLC_PLUGIN_LZ4_VERSION_MAKE(1, 9, 4)

    LZ4F_CustomMem memoryManager;
    bsl::memset(&memoryManager, 0, sizeof memoryManager);

    memoryManager.customAlloc = &Lz4::allocate;
    memoryManager.customFree  = &Lz4::free;
    memoryManager.opaqueState = d_allocator_p;

    d_inflaterContext_p = LZ4F_createDecompressionContext_advanced(
        memoryManager, LZ4F_VERSION);
    if (d_inflaterContext_p == 0) {
        NTCI_LOG_ERROR("Failed to create decompression context");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

#else

    errorCode =
        LZ4F_createDecompressionContext(&d_inflaterContext_p, LZ4F_VERSION);
    if (LZ4F_isError(errorCode)) {
        NTCI_LOG_ERROR("Failed to create decompression context: %s",
                       LZ4F_getErrorName(errorCode));
        NTCCFG_ABORT();
    }

#endif

    bsl::memset(&d_inflaterOptions, 0, sizeof d_inflaterOptions);

    d_inflaterOptions.stableDst = 0;

#if NTCTLC_PLUGIN_LZ4_VERSION_NUMBER >= \
    NTCTLC_PLUGIN_LZ4_VERSION_MAKE(1, 10, 0)
        d_inflaterOptions.skipChecksums = 
            static_cast<unsigned>(!k_CHECKSUM_VERIFY);
#endif

    return ntsa::Error();
}

ntsa::Error Lz4::inflateBegin(ntca::InflateContext*       context,
                              bdlbb::Blob*                result,
                              const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error();
}

ntsa::Error Lz4::inflateNext(ntca::InflateContext*       context,
                             bdlbb::Blob*                result,
                             const bsl::uint8_t*         data,
                             bsl::size_t                 size,
                             const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    NTCI_LOG_CONTEXT();

    LZ4F_errorCode_t errorCode = 0;

    bsl::size_t totalBytesRead    = 0;
    bsl::size_t totalBytesWritten = 0;

    const bsl::uint8_t* sourceCurrent = data;
    const bsl::uint8_t* sourceEnd     = data + size;

    while (sourceCurrent < sourceEnd) {
        if (d_inflaterBufferSize == 
            static_cast<bsl::size_t>(d_inflaterBuffer.size())) 
        {
            this->inflateOverflow(result);
        }

        char* destination = 
            d_inflaterBuffer.data() + d_inflaterBufferSize;

        bsl::size_t destinationSize = 
            static_cast<bsl::size_t>(
                d_inflaterBuffer.size() - d_inflaterBufferSize);

        bsl::size_t sourceSize = 
            static_cast<bsl::size_t>(sourceEnd - sourceCurrent);

        errorCode = LZ4F_decompress(d_inflaterContext_p,
                                    destination,
                                    &destinationSize,
                                    sourceCurrent,
                                    &sourceSize,
                                    &d_inflaterOptions);

        if (LZ4F_isError(errorCode)) {
            NTCI_LOG_ERROR("Failed to inflate: %s",
                           LZ4F_getErrorName(errorCode));
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (destinationSize > 0) {
            d_inflaterBufferSize += destinationSize;
            totalBytesWritten    += destinationSize;
        }

        if (sourceSize > 0) {
            sourceCurrent  += sourceSize;
            totalBytesRead += sourceSize;
        }
    }

    context->setBytesRead(context->bytesRead() + totalBytesRead);
    context->setBytesWritten(context->bytesWritten() + totalBytesWritten);

    return ntsa::Error();
}

ntsa::Error Lz4::inflateEnd(ntca::InflateContext*       context,
                            bdlbb::Blob*                result,
                            const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    if (d_inflaterBufferSize != 0) {
        this->inflateCommit(result);
    }

    context->setCompressionType(ntca::CompressionType::e_LZ4);

    return ntsa::Error();
}

NTCCFG_INLINE
void Lz4::inflateOverflow(bdlbb::Blob* result)
{
    if (d_inflaterBufferSize != 0) {
        this->inflateCommit(result);
    }

    d_inflaterBuffer.reset();
    d_inflaterBufferSize = 0;

    d_dataPool_sp->createIncomingBlobBuffer(&d_inflaterBuffer);
}

NTCCFG_INLINE
void Lz4::inflateCommit(bdlbb::Blob* result)
{
    if (d_inflaterBufferSize != 0) {
        d_inflaterBuffer.setSize(static_cast<int>(d_inflaterBufferSize));

        BSLS_ASSERT(d_inflaterBuffer.buffer().get() != 0);
        BSLS_ASSERT(d_inflaterBuffer.size() > 0);

        NTCTLC_PLUGIN_LOG_INFLATED_BLOB_BUFFER(d_inflaterBuffer);

        result->appendDataBuffer(d_inflaterBuffer);

        d_inflaterBuffer.reset();
        d_inflaterBufferSize = 0;
    }
}

NTCCFG_INLINE
LZ4F_errorCode_t Lz4::inflateCycle(bsl::size_t* numBytesRead,
                                   bsl::size_t* numBytesWritten)
{
    LZ4F_errorCode_t errorCode = 0;

    *numBytesRead    = 0;
    *numBytesWritten = 0;

    return errorCode;
}

NTCCFG_INLINE
ntsa::Error Lz4::inflateReset()
{
    LZ4F_resetDecompressionContext(d_inflaterContext_p);
    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error Lz4::inflateDestroy()
{
    LZ4F_freeDecompressionContext(d_inflaterContext_p);
    d_inflaterContext_p = 0;

    d_inflaterBuffer.reset();

    return ntsa::Error();
}

void* Lz4::allocate(void* opaque, bsl::size_t size)
{
    bslma::Allocator* allocator = reinterpret_cast<bslma::Allocator*>(opaque);

    return allocator->allocate(size);
}

void Lz4::free(void* opaque, void* address)
{
    bslma::Allocator* allocator = reinterpret_cast<bslma::Allocator*>(opaque);

    allocator->deallocate(address);
}

Lz4::Lz4(const ntca::CompressionConfig&         configuration,
         const bsl::shared_ptr<ntci::DataPool>& dataPool,
         bslma::Allocator*                      basicAllocator)
: d_deflaterContext_p(0)
, d_deflaterBuffer()
, d_deflaterBufferSize(0)
, d_deflaterArena(0)
, d_deflaterArenaCapacity(0)
, d_inflaterContext_p(0)
, d_inflaterBuffer()
, d_inflaterBufferSize(0)
, d_level(1)
, d_dataPool_sp(dataPool)
, d_config(configuration)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntsa::Error error;

    BSLS_ASSERT_OPT(d_config.type().isNull() ||
                    d_config.type().value() == ntca::CompressionType::e_LZ4);

    error = this->deflateCreate();
    BSLS_ASSERT_OPT(!error);

    error = this->inflateCreate();
    BSLS_ASSERT_OPT(!error);

    error = this->inflateReset();
    BSLS_ASSERT_OPT(!error);
}

Lz4::~Lz4()
{
    ntsa::Error error;

    error = this->inflateDestroy();
    BSLS_ASSERT_OPT(!error);

    error = this->deflateDestroy();
    BSLS_ASSERT_OPT(!error);

    d_dataPool_sp.reset();
}

ntca::CompressionType::Value Lz4::type() const
{
    return ntca::CompressionType::e_LZ4;
}

#endif

#if NTC_BUILD_WITH_ZSTD

#define NTCTLC_PLUGIN_ZSTD_VERSION_MAKE(major, minor, patch) \
    (((major) * 100 * 100) + ((minor) * 100) + (patch))

#define NTCTLC_PLUGIN_ZSTD_VERSION_NUMBER ZSTD_VERSION_NUMBER

NTCCFG_INLINE
ntsa::Error Zstd::deflateCreate()
{
    bsl::size_t rc = 0;

    if (d_config.goal().has_value()) {
        switch (d_config.goal().value()) {
        case ntca::CompressionGoal::e_BEST_SIZE:
            d_level = 22;
            break;
        case ntca::CompressionGoal::e_BETTER_SIZE:
            d_level = 11;
            break;
        case ntca::CompressionGoal::e_BALANCED:
            d_level = 3;
            break;
        case ntca::CompressionGoal::e_BETTER_SPEED:
            d_level = 2;
            break;
        case ntca::CompressionGoal::e_BEST_SPEED:
            d_level = 1;
            break;
        default:
            break;
        }
    }

#if defined(ZSTD_STATIC_LINKING_ONLY)

    ZSTD_customMem memoryManager;
    bsl::memset(&memoryManager, 0, sizeof memoryManager);

    memoryManager.customAlloc = &Zstd::allocate;
    memoryManager.customFree  = &Zstd::free;
    memoryManager.opaque      = d_allocator_p;

    d_deflaterContext_p = ZSTD_createCCtx_advanced(memoryManager);
    if (d_deflaterContext_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

#else

    d_deflaterContext_p = ZSTD_createCCtx();
    if (d_deflaterContext_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

#endif

    rc = ZSTD_CCtx_setParameter(d_deflaterContext_p, 
                                ZSTD_c_compressionLevel, 
                                d_level);
    if (ZSTD_isError(rc)) {
        return Zstd::translateError(rc, "set compression level");
    }

    rc = ZSTD_CCtx_setParameter(d_deflaterContext_p, ZSTD_c_checksumFlag, 1);
    if (ZSTD_isError(rc)) {
        return Zstd::translateError(rc, "set checksum flag");
    }

    bsl::memset(&d_deflaterInput, 0, sizeof d_deflaterInput);
    bsl::memset(&d_deflaterOutput, 0, sizeof d_deflaterOutput);

    return ntsa::Error();
}

ntsa::Error Zstd::deflateBegin(ntca::DeflateContext*       context,
                               bdlbb::Blob*                result,
                               const ntca::DeflateOptions& options)
{
    ntsa::Error error;

    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    d_deflaterInput.src  = 0;
    d_deflaterInput.size = 0;
    d_deflaterInput.pos  = 0;

    d_deflaterOutput.dst  = 0;
    d_deflaterOutput.size = 0;
    d_deflaterOutput.pos  = 0;

    error = this->deflateReset();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Zstd::deflateNext(ntca::DeflateContext*       context,
                              bdlbb::Blob*                result,
                              const bsl::uint8_t*         data,
                              bsl::size_t                 size,
                              const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    bsl::size_t rc = 0;

    bsl::size_t totalBytesWritten = 0;
    bsl::size_t totalBytesRead    = 0;

    d_deflaterInput.src  = data;
    d_deflaterInput.size = size;
    d_deflaterInput.pos  = 0;

    while (d_deflaterInput.pos != d_deflaterInput.size) {
        if (d_deflaterOutput.pos == d_deflaterOutput.size) {
            this->deflateOverflow(result);
        }

        bsl::size_t numBytesRead    = 0;
        bsl::size_t numBytesWritten = 0;

        rc = this->deflateCycle(&numBytesRead, 
                                &numBytesWritten, 
                                ZSTD_e_continue);

        totalBytesRead    += numBytesRead;
        totalBytesWritten += numBytesWritten;

        if (ZSTD_isError(rc)) {
            return this->translateError(rc, "deflate");
        }
    }

    context->setBytesRead(context->bytesRead() + totalBytesRead);
    context->setBytesWritten(context->bytesWritten() + totalBytesWritten);

    return ntsa::Error();
}

ntsa::Error Zstd::deflateEnd(ntca::DeflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    bsl::size_t rc = 0;

    bsl::size_t totalBytesWritten = 0;
    bsl::size_t totalBytesRead    = 0;

    while (true) {
        if (d_deflaterOutput.pos == d_deflaterOutput.size) {
            this->deflateOverflow(result);
        }

        bsl::size_t numBytesRead    = 0;
        bsl::size_t numBytesWritten = 0;

        rc = this->deflateCycle(&numBytesRead, &numBytesWritten, ZSTD_e_end);

        totalBytesRead    += numBytesRead;
        totalBytesWritten += numBytesWritten;

        if (rc > 0) {
            continue;
        }
        else if (rc == 0) {
            if (d_deflaterBufferSize != 0) {
                this->deflateCommit(result);
            }
            break;
        }
        else {
            return this->translateError(rc, "deflate");
        }
    }

    context->setCompressionType(ntca::CompressionType::e_ZSTD);
    context->setBytesRead(context->bytesRead() + totalBytesRead);
    context->setBytesWritten(context->bytesWritten() + totalBytesWritten);

    return ntsa::Error();
}

NTCCFG_INLINE
void Zstd::deflateOverflow(bdlbb::Blob* result)
{
    if (d_deflaterBufferSize != 0) {
        this->deflateCommit(result);
    }

    d_deflaterBuffer.reset();
    d_deflaterBufferSize = 0;

    d_dataPool_sp->createOutgoingBlobBuffer(&d_deflaterBuffer);

    d_deflaterOutput.dst  = d_deflaterBuffer.data();
    d_deflaterOutput.size = d_deflaterBuffer.size();
    d_deflaterOutput.pos  = 0;
}

NTCCFG_INLINE
void Zstd::deflateCommit(bdlbb::Blob* result)
{
    if (d_deflaterBufferSize != 0) {
        d_deflaterBuffer.setSize(static_cast<int>(d_deflaterBufferSize));

        BSLS_ASSERT(d_deflaterBuffer.buffer().get() != 0);
        BSLS_ASSERT(d_deflaterBuffer.size() > 0);

        NTCTLC_PLUGIN_LOG_DEFLATED_BLOB_BUFFER(d_deflaterBuffer);

        result->appendDataBuffer(d_deflaterBuffer);

        d_deflaterBuffer.reset();
        d_deflaterBufferSize = 0;

        d_deflaterOutput.dst  = 0;
        d_deflaterOutput.size = 0;
        d_deflaterOutput.pos  = 0;
    }
}

NTCCFG_INLINE
bsl::size_t Zstd::deflateCycle(bsl::size_t*      numBytesRead,
                               bsl::size_t*      numBytesWritten,
                               ZSTD_EndDirective mode)
{
    bsl::size_t rc = 0;

    *numBytesRead    = 0;
    *numBytesWritten = 0;

    bsl::size_t posIn0  = d_deflaterInput.pos;
    bsl::size_t posOut0 = d_deflaterOutput.pos;

    rc = ZSTD_compressStream2(d_deflaterContext_p, 
                              &d_deflaterOutput, 
                              &d_deflaterInput,
                              mode);

    bsl::size_t posIn1  = d_deflaterInput.pos;
    bsl::size_t posOut1 = d_deflaterOutput.pos;

    BSLS_ASSERT(posIn1 >= posIn0);
    BSLS_ASSERT(posOut1 >= posOut0);

    bsl::size_t posInDiff  = static_cast<bsl::size_t>(posIn1 - posIn0);
    bsl::size_t posOutDiff = static_cast<bsl::size_t>(posOut1 - posOut0);

    *numBytesRead    = posInDiff;
    *numBytesWritten = posOutDiff;

    d_deflaterBufferSize += posOutDiff;

    return rc;
}

NTCCFG_INLINE
ntsa::Error Zstd::deflateReset()
{
    bsl::size_t rc = 0;

    rc = ZSTD_CCtx_reset(d_deflaterContext_p, ZSTD_reset_session_only);
    if (ZSTD_isError(rc)) {
        return Zstd::translateError(rc, "reset defflater");
    }

    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error Zstd::deflateDestroy()
{
    bsl::size_t rc = 0;
    
    rc = ZSTD_freeCCtx(d_deflaterContext_p);
    if (ZSTD_isError(rc)) {
        Zstd::translateError(rc, "destroy deflater");
    }

    d_deflaterContext_p = 0;

    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error Zstd::inflateCreate()
{
#if defined(ZSTD_STATIC_LINKING_ONLY)

    ZSTD_customMem memoryManager;
    bsl::memset(&memoryManager, 0, sizeof memoryManager);

    memoryManager.customAlloc = &Zstd::allocate;
    memoryManager.customFree  = &Zstd::free;
    memoryManager.opaque      = d_allocator_p;

    d_inflaterContext_p = ZSTD_createDCtx_advanced(memoryManager);
    if (d_inflaterContext_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

#else

    d_inflaterContext_p = ZSTD_createDCtx();
    if (d_inflaterContext_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

#endif

    bsl::memset(&d_inflaterInput, 0, sizeof d_inflaterInput);
    bsl::memset(&d_inflaterOutput, 0, sizeof d_inflaterOutput);

    return ntsa::Error();
}

ntsa::Error Zstd::inflateBegin(ntca::InflateContext*       context,
                               bdlbb::Blob*                result,
                               const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error();
}

ntsa::Error Zstd::inflateNext(ntca::InflateContext*       context,
                              bdlbb::Blob*                result,
                              const bsl::uint8_t*         data,
                              bsl::size_t                 size,
                              const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    ntsa::Error error;
    bsl::size_t rc = 0;

    bsl::size_t totalBytesWritten = 0;
    bsl::size_t totalBytesRead    = 0;

    d_inflaterInput.src  = data;
    d_inflaterInput.size = size;
    d_inflaterInput.pos  = 0;

    while (d_inflaterInput.pos != d_inflaterInput.size) {
        if (d_inflaterOutput.pos == d_inflaterOutput.size) {
            this->inflateOverflow(result);
        }

        bsl::size_t numBytesRead    = 0;
        bsl::size_t numBytesWritten = 0;

        rc = this->inflateCycle(&numBytesRead, &numBytesWritten);

        totalBytesRead    += numBytesRead;
        totalBytesWritten += numBytesWritten;

        if (rc > 0) {
            continue;
        }
        else if (rc == 0) {
            if (d_inflaterBufferSize != 0) {
                this->inflateCommit(result);
            }

            error = this->inflateReset();
            if (error) {
                return error;
            }

            continue;
        }
        else {
            return this->translateError(rc, "inflate");
        }
    }

    d_inflaterInput.src  = 0;
    d_inflaterInput.size = 0;
    d_inflaterInput.pos  = 0;

    context->setBytesRead(context->bytesRead() + totalBytesRead);
    context->setBytesWritten(context->bytesWritten() + totalBytesWritten);

    return ntsa::Error();
}

ntsa::Error Zstd::inflateEnd(ntca::InflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    context->setCompressionType(ntca::CompressionType::e_ZSTD);

    return ntsa::Error();
}

NTCCFG_INLINE
void Zstd::inflateOverflow(bdlbb::Blob* result)
{
    if (d_inflaterBufferSize != 0) {
        this->inflateCommit(result);
    }

    d_inflaterBuffer.reset();
    d_inflaterBufferSize = 0;

    d_dataPool_sp->createIncomingBlobBuffer(&d_inflaterBuffer);

    d_inflaterOutput.dst  = d_inflaterBuffer.data();
    d_inflaterOutput.size = d_inflaterBuffer.size();
    d_inflaterOutput.pos  = 0;
}

NTCCFG_INLINE
void Zstd::inflateCommit(bdlbb::Blob* result)
{
    if (d_inflaterBufferSize != 0) {
        d_inflaterBuffer.setSize(static_cast<int>(d_inflaterBufferSize));

        BSLS_ASSERT(d_inflaterBuffer.buffer().get() != 0);
        BSLS_ASSERT(d_inflaterBuffer.size() > 0);

        NTCTLC_PLUGIN_LOG_INFLATED_BLOB_BUFFER(d_inflaterBuffer);

        result->appendDataBuffer(d_inflaterBuffer);

        d_inflaterBuffer.reset();
        d_inflaterBufferSize = 0;

        d_inflaterOutput.dst  = 0;
        d_inflaterOutput.size = 0;
        d_inflaterOutput.pos  = 0;
    }
}

NTCCFG_INLINE
bsl::size_t Zstd::inflateCycle(bsl::size_t* numBytesRead,
                               bsl::size_t* numBytesWritten)
{
    bsl::size_t rc = 0;

    *numBytesRead    = 0;
    *numBytesWritten = 0;

    bsl::size_t posIn0  = d_inflaterInput.pos;
    bsl::size_t posOut0 = d_inflaterOutput.pos;

    rc = ZSTD_decompressStream(d_inflaterContext_p, 
                               &d_inflaterOutput, 
                               &d_inflaterInput);

    bsl::size_t posIn1  = d_inflaterInput.pos;
    bsl::size_t posOut1 = d_inflaterOutput.pos;

    BSLS_ASSERT(posIn1 >= posIn0);
    BSLS_ASSERT(posOut1 >= posOut0);

    bsl::size_t posInDiff  = static_cast<bsl::size_t>(posIn1 - posIn0);
    bsl::size_t posOutDiff = static_cast<bsl::size_t>(posOut1 - posOut0);

    *numBytesRead    = posInDiff;
    *numBytesWritten = posOutDiff;

    d_inflaterBufferSize += posOutDiff;

    return rc;
}

NTCCFG_INLINE
ntsa::Error Zstd::inflateReset()
{
    bsl::size_t rc = 0;

    rc = ZSTD_DCtx_reset(d_inflaterContext_p, ZSTD_reset_session_only);
    if (ZSTD_isError(rc)) {
        return Zstd::translateError(rc, "reset inflater");
    }

    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error Zstd::inflateDestroy()
{
    bsl::size_t rc = 0;
    
    rc = ZSTD_freeDCtx(d_inflaterContext_p);
    if (ZSTD_isError(rc)) {
        Zstd::translateError(rc, "destroy inflater");
    }

    d_inflaterContext_p = 0;

    return ntsa::Error();
}

const char* Zstd::describeError(bsl::size_t error)
{
    return ZSTD_getErrorName(error);
}

void* Zstd::allocate(void* opaque, bsl::size_t size)
{
    bslma::Allocator* allocator = reinterpret_cast<bslma::Allocator*>(opaque);

    return allocator->allocate(size);
}

void Zstd::free(void* opaque, void* address)
{
    bslma::Allocator* allocator = reinterpret_cast<bslma::Allocator*>(opaque);

    allocator->deallocate(address);
}

ntsa::Error Zstd::translateError(bsl::size_t error, const char* operation)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error result;

    if (ZSTD_isError(error)) {
        ZSTD_ErrorCode errorCode = ZSTD_getErrorCode(error);
        if ( 
#if NTCTLC_PLUGIN_ZSTD_VERSION_NUMBER >= \
    NTCTLC_PLUGIN_ZSTD_VERSION_MAKE(1, 5, 7)
            errorCode == ZSTD_error_noForwardProgress_destFull ||
            errorCode == ZSTD_error_noForwardProgress_inputEmpty ||
#endif
            errorCode == ZSTD_error_dstBuffer_null ||
            errorCode == ZSTD_error_dstSize_tooSmall) 
        {
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }
        else {
            result = ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        return ntsa::Error();
    }

    const char* errorDescription = Zstd::describeError(error);

    if (result) {
        NTCI_LOG_STREAM_WARN << "Failed to " << operation << ": "
                             << errorDescription << " (rc = " << error << ")"
                             << NTCI_LOG_STREAM_END;
    }

    return result;
}

Zstd::Zstd(const ntca::CompressionConfig&         configuration,
           const bsl::shared_ptr<ntci::DataPool>& dataPool,
           bslma::Allocator*                      basicAllocator)
: d_deflaterContext_p(0)
, d_deflaterBuffer()
, d_deflaterBufferSize(0)
, d_inflaterContext_p(0)
, d_inflaterBuffer()
, d_inflaterBufferSize(0)
, d_level(0)
, d_dataPool_sp(dataPool)
, d_config(configuration)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntsa::Error error;

    BSLS_ASSERT_OPT(d_config.type().isNull() ||
                    d_config.type().value() == ntca::CompressionType::e_ZSTD);

    error = this->deflateCreate();
    BSLS_ASSERT_OPT(!error);

    error = this->inflateCreate();
    BSLS_ASSERT_OPT(!error);

    error = this->inflateReset();
    BSLS_ASSERT_OPT(!error);
}

Zstd::~Zstd()
{
    ntsa::Error error;

    error = this->inflateDestroy();
    BSLS_ASSERT_OPT(!error);

    error = this->deflateDestroy();
    BSLS_ASSERT_OPT(!error);

    d_dataPool_sp.reset();
}

ntca::CompressionType::Value Zstd::type() const
{
    return ntca::CompressionType::e_ZSTD;
}

#endif

#if NTC_BUILD_WITH_ZLIB

#define NTCTLC_ZLIB_DEFLATER_LOG_FRAME(checksum)                              \
    do {                                                                      \
        NTCI_LOG_DEBUG("Deflated frame "                                      \
                       "[ checksum = %u ]",                                   \
                       static_cast<bsl::uint32_t>((checksum)));               \
    } while (false)

#define NTCTLC_ZLIB_INFLATER_LOG_FRAME(checksum)                              \
    do {                                                                      \
        NTCI_LOG_DEBUG("Inflated frame "                                      \
                       "[ checksum = %u ]",                                   \
                       static_cast<bsl::uint32_t>((checksum)));               \
    } while (false)

NTCCFG_INLINE
ntsa::Error Zlib::deflateCreate()
{
    int rc = 0;

    if (d_config.goal().has_value()) {
        switch (d_config.goal().value()) {
        case ntca::CompressionGoal::e_BEST_SIZE:  // 9
            d_level = Z_BEST_COMPRESSION;
            break;
        case ntca::CompressionGoal::e_BETTER_SIZE:
            d_level = 7;
            break;
        case ntca::CompressionGoal::e_BALANCED:
            d_level = Z_DEFAULT_COMPRESSION;  // 6
            break;
        case ntca::CompressionGoal::e_BETTER_SPEED:
            d_level = 4;
            break;
        case ntca::CompressionGoal::e_BEST_SPEED:
            d_level = Z_BEST_SPEED;  // 1
            break;
        default:
            break;
        }
    }

    bsl::memset(&d_deflaterStream, 0, sizeof d_deflaterStream);

    d_deflaterStream.zalloc = &Zlib::allocate;
    d_deflaterStream.zfree  = &Zlib::free;
    d_deflaterStream.opaque = d_allocator_p;

#if defined(Z_PREFIX_SET)
    rc = z_deflateInit(&d_deflaterStream, d_level);
#else
    rc = deflateInit(&d_deflaterStream, d_level);
#endif

    if (rc != Z_OK) {
        return this->translateError(rc, "initializer deflater");
    }

    return ntsa::Error();
}

ntsa::Error Zlib::deflateBegin(ntca::DeflateContext*       context,
                               bdlbb::Blob*                result,
                               const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    ntsa::Error error;

    d_deflaterStream.next_in  = 0;
    d_deflaterStream.avail_in = 0;
    d_deflaterStream.total_in = 0;

    d_deflaterStream.next_out  = 0;
    d_deflaterStream.avail_out = 0;
    d_deflaterStream.total_out = 0;
    d_deflaterStream.adler     = 0;

    error = this->deflateReset();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Zlib::deflateNext(ntca::DeflateContext*       context,
                              bdlbb::Blob*                result,
                              const bsl::uint8_t*         data,
                              bsl::size_t                 size,
                              const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    int rc = 0;

    bsl::size_t totalBytesWritten = 0;
    bsl::size_t totalBytesRead    = 0;

    d_deflaterStream.next_in  = const_cast<bsl::uint8_t*>(data);
    d_deflaterStream.avail_in = static_cast<uInt>(size);

    while (d_deflaterStream.avail_in != 0) {
        if (d_deflaterStream.avail_out == 0) {
            this->deflateOverflow(result);
        }

        bsl::size_t numBytesRead    = 0;
        bsl::size_t numBytesWritten = 0;

        rc = this->deflateCycle(&numBytesRead, &numBytesWritten, Z_NO_FLUSH);

        totalBytesRead    += numBytesRead;
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
    NTCCFG_WARNING_UNUSED(options);

    NTCI_LOG_CONTEXT();

    int rc = 0;

    bsl::size_t totalBytesWritten = 0;
    bsl::size_t totalBytesRead    = 0;

    while (true) {
        if (d_deflaterStream.avail_out == 0) {
            this->deflateOverflow(result);
        }

        bsl::size_t numBytesRead    = 0;
        bsl::size_t numBytesWritten = 0;

        rc = this->deflateCycle(&numBytesRead, &numBytesWritten, Z_FINISH);

        totalBytesRead    += numBytesRead;
        totalBytesWritten += numBytesWritten;

        if (rc == Z_OK || rc == Z_BUF_ERROR) {
            continue;
        }
        else if (rc == Z_STREAM_END) {
            if (d_deflaterBufferSize != 0) {
                this->deflateCommit(result);
            }
            break;
        }
        else {
            return this->translateError(rc, "deflate");
        }
    }

    context->setCompressionType(ntca::CompressionType::e_ZLIB);
    context->setBytesRead(context->bytesRead() + totalBytesRead);
    context->setBytesWritten(context->bytesWritten() + totalBytesWritten);

    bsl::uint32_t checksumValue =
        static_cast<bsl::uint32_t>(d_deflaterStream.adler);

    ntca::Checksum checksum;
    checksum.store(ntca::ChecksumType::e_ADLER32,
                   &checksumValue,
                   sizeof checksumValue);

    context->setChecksum(checksum);

    NTCTLC_ZLIB_DEFLATER_LOG_FRAME(checksumValue);

    return ntsa::Error();
}

NTCCFG_INLINE
void Zlib::deflateOverflow(bdlbb::Blob* result)
{
    if (d_deflaterBufferSize != 0) {
        this->deflateCommit(result);
    }

    d_deflaterBuffer.reset();
    d_deflaterBufferSize = 0;

    d_dataPool_sp->createOutgoingBlobBuffer(&d_deflaterBuffer);

    d_deflaterStream.next_out =
        reinterpret_cast<unsigned char*>(d_deflaterBuffer.data());
    d_deflaterStream.avail_out = d_deflaterBuffer.size();
}

NTCCFG_INLINE
void Zlib::deflateCommit(bdlbb::Blob* result)
{
    if (d_deflaterBufferSize != 0) {
        d_deflaterBuffer.setSize(static_cast<int>(d_deflaterBufferSize));

        BSLS_ASSERT(d_deflaterBuffer.buffer().get() != 0);
        BSLS_ASSERT(d_deflaterBuffer.size() > 0);

        NTCTLC_PLUGIN_LOG_DEFLATED_BLOB_BUFFER(d_deflaterBuffer);

        result->appendDataBuffer(d_deflaterBuffer);

        d_deflaterBuffer.reset();
        d_deflaterBufferSize = 0;

        d_deflaterStream.next_out  = 0;
        d_deflaterStream.avail_out = 0;
    }
}

NTCCFG_INLINE
int Zlib::deflateCycle(bsl::size_t* numBytesRead,
                       bsl::size_t* numBytesWritten,
                       int          mode)
{
    int rc = 0;

    *numBytesRead    = 0;
    *numBytesWritten = 0;

    uInt availIn0  = d_deflaterStream.avail_in;
    uInt availOut0 = d_deflaterStream.avail_out;

    rc = ::deflate(&d_deflaterStream, mode);

    uInt availIn1  = d_deflaterStream.avail_in;
    uInt availOut1 = d_deflaterStream.avail_out;

    BSLS_ASSERT(availIn0 >= availIn1);
    BSLS_ASSERT(availOut0 >= availOut1);

    uInt availInDiff  = availIn0 - availIn1;
    uInt availOutDiff = availOut0 - availOut1;

    *numBytesRead    = availInDiff;
    *numBytesWritten = availOutDiff;

    d_deflaterBufferSize += availOutDiff;

    return rc;
}

NTCCFG_INLINE
ntsa::Error Zlib::deflateReset()
{
    int rc = 0;

    rc = ::deflateReset(&d_deflaterStream);
    if (rc != Z_OK) {
        return this->translateError(rc, "reset deflater");
    }

    d_deflaterGeneration++;

    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error Zlib::deflateDestroy()
{
    int rc = 0;

    d_deflaterStream.next_in  = 0;
    d_deflaterStream.avail_in = 0;
    d_deflaterStream.total_in = 0;

    d_deflaterStream.next_out  = 0;
    d_deflaterStream.avail_out = 0;
    d_deflaterStream.total_out = 0;
    d_deflaterStream.adler     = 0;

    rc = ::deflateReset(&d_deflaterStream);
    if (rc != Z_OK) {
        Zlib::translateError(rc, "reset deflater");
    }

    rc = ::deflateEnd(&d_deflaterStream);
    if (rc != Z_OK) {
        Zlib::translateError(rc, "close deflater");
    }

    d_deflaterBuffer.reset();

    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error Zlib::inflateCreate()
{
    int rc = 0;

    bsl::memset(&d_inflaterStream, 0, sizeof d_inflaterStream);

    d_inflaterStream.zalloc = &Zlib::allocate;
    d_inflaterStream.zfree  = &Zlib::free;
    d_inflaterStream.opaque = d_allocator_p;

#if defined(Z_PREFIX_SET)
    rc = z_inflateInit(&d_inflaterStream);
#else
    rc = inflateInit(&d_inflaterStream);
#endif

    if (rc != Z_OK) {
        return this->translateError(rc, "initialize inflater");
    }

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
    NTCCFG_WARNING_UNUSED(options);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc = 0;

    bsl::size_t totalBytesWritten = 0;
    bsl::size_t totalBytesRead    = 0;

    BSLS_ASSERT(d_inflaterStream.next_in == 0);
    BSLS_ASSERT(d_inflaterStream.avail_in == 0);

    d_inflaterStream.next_in  = const_cast<bsl::uint8_t*>(data);
    d_inflaterStream.avail_in = static_cast<uInt>(size);

    while (d_inflaterStream.avail_in != 0) {
        if (d_inflaterStream.avail_out == 0) {
            this->inflateOverflow(result);
        }

        bsl::size_t numBytesRead    = 0;
        bsl::size_t numBytesWritten = 0;

        rc = this->inflateCycle(&numBytesRead, &numBytesWritten, Z_NO_FLUSH);

        totalBytesRead    += numBytesRead;
        totalBytesWritten += numBytesWritten;

        if (rc == Z_OK || rc == Z_BUF_ERROR) {
            continue;
        }
        else if (rc == Z_STREAM_END) {
            if (d_inflaterBufferSize != 0) {
                this->inflateCommit(result);
            }

            NTCTLC_ZLIB_INFLATER_LOG_FRAME(d_inflaterStream.adler);

            error = this->inflateReset();
            if (error) {
                return error;
            }

            continue;
        }
        else {
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
    NTCCFG_WARNING_UNUSED(options);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc = 0;

    bsl::size_t totalBytesWritten = 0;
    bsl::size_t totalBytesRead    = 0;

    BSLS_ASSERT(d_inflaterStream.next_in == 0);
    BSLS_ASSERT(d_inflaterStream.avail_in == 0);

    while (true) {
        if (d_inflaterStream.avail_out == 0) {
            this->inflateOverflow(result);
        }

        bsl::size_t numBytesRead    = 0;
        bsl::size_t numBytesWritten = 0;

        rc = this->inflateCycle(&numBytesRead, &numBytesWritten, Z_SYNC_FLUSH);

        totalBytesRead    += numBytesRead;
        totalBytesWritten += numBytesWritten;

        if (rc == Z_OK || rc == Z_BUF_ERROR) {
            if (numBytesRead == 0 && numBytesWritten == 0) {
                break;
            }
            else {
                continue;
            }
        }
        else if (rc == Z_STREAM_END) {
            if (d_inflaterBufferSize != 0) {
                this->inflateCommit(result);
            }

            NTCTLC_ZLIB_INFLATER_LOG_FRAME(d_inflaterStream.adler);

            error = this->inflateReset();
            if (error) {
                return error;
            }

            continue;
        }
        else {
            return this->translateError(rc, "inflate");
        }
    }

    d_inflaterStream.next_in  = 0;
    d_inflaterStream.avail_in = 0;

    context->setCompressionType(ntca::CompressionType::e_ZLIB);
    context->setBytesRead(context->bytesRead() + totalBytesRead);
    context->setBytesWritten(context->bytesWritten() + totalBytesWritten);

    return ntsa::Error();
}

NTCCFG_INLINE
void Zlib::inflateOverflow(bdlbb::Blob* result)
{
    if (d_inflaterBufferSize != 0) {
        this->inflateCommit(result);
    }

    d_inflaterBuffer.reset();
    d_inflaterBufferSize = 0;

    d_dataPool_sp->createIncomingBlobBuffer(&d_inflaterBuffer);

    d_inflaterStream.next_out =
        reinterpret_cast<unsigned char*>(d_inflaterBuffer.data());
    d_inflaterStream.avail_out = d_inflaterBuffer.size();
}

NTCCFG_INLINE
void Zlib::inflateCommit(bdlbb::Blob* result)
{
    if (d_inflaterBufferSize != 0) {
        d_inflaterBuffer.setSize(static_cast<int>(d_inflaterBufferSize));

        BSLS_ASSERT(d_inflaterBuffer.buffer().get() != 0);
        BSLS_ASSERT(d_inflaterBuffer.size() > 0);

        NTCTLC_PLUGIN_LOG_INFLATED_BLOB_BUFFER(d_inflaterBuffer);

        result->appendDataBuffer(d_inflaterBuffer);

        d_inflaterBuffer.reset();
        d_inflaterBufferSize = 0;

        d_inflaterStream.next_out  = 0;
        d_inflaterStream.avail_out = 0;
    }
}

NTCCFG_INLINE
int Zlib::inflateCycle(bsl::size_t* numBytesRead,
                       bsl::size_t* numBytesWritten,
                       int          mode)
{
    int rc = 0;

    *numBytesRead    = 0;
    *numBytesWritten = 0;

    uInt availIn0  = d_inflaterStream.avail_in;
    uInt availOut0 = d_inflaterStream.avail_out;

    rc = ::inflate(&d_inflaterStream, mode);

    uInt availIn1  = d_inflaterStream.avail_in;
    uInt availOut1 = d_inflaterStream.avail_out;

    BSLS_ASSERT(availIn0 >= availIn1);
    BSLS_ASSERT(availOut0 >= availOut1);

    uInt availInDiff  = availIn0 - availIn1;
    uInt availOutDiff = availOut0 - availOut1;

    *numBytesRead    = availInDiff;
    *numBytesWritten = availOutDiff;

    d_inflaterBufferSize += availOutDiff;

    return rc;
}

NTCCFG_INLINE
ntsa::Error Zlib::inflateReset()
{
    int rc = 0;

    rc = ::inflateReset(&d_inflaterStream);
    if (rc != Z_OK) {
        return this->translateError(rc, "reset inflater");
    }

    ++d_inflaterGeneration;

    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error Zlib::inflateDestroy()
{
    int rc = 0;

    d_inflaterStream.next_in  = 0;
    d_inflaterStream.avail_in = 0;
    d_inflaterStream.total_in = 0;

    d_inflaterStream.next_out  = 0;
    d_inflaterStream.avail_out = 0;
    d_inflaterStream.total_out = 0;
    d_inflaterStream.adler     = 0;

    rc = ::inflateReset(&d_inflaterStream);
    if (rc != Z_OK) {
        Zlib::translateError(rc, "reset inflater");
    }

    rc = ::inflateEnd(&d_inflaterStream);
    if (rc != Z_OK) {
        Zlib::translateError(rc, "close inflater");
    }

    d_inflaterBuffer.reset();

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
        NTCI_LOG_STREAM_WARN << "Failed to " << operation << ": "
                             << errorDescription << " (rc = " << error << ")"
                             << NTCI_LOG_STREAM_END;
    }

    return result;
}

Zlib::Zlib(const ntca::CompressionConfig&         configuration,
           const bsl::shared_ptr<ntci::DataPool>& dataPool,
           bslma::Allocator*                      basicAllocator)
: d_deflaterBuffer()
, d_deflaterBufferSize(0)
, d_deflaterGeneration(0)
, d_inflaterBuffer()
, d_inflaterBufferSize(0)
, d_inflaterGeneration(0)
, d_level(Z_DEFAULT_COMPRESSION)
, d_dataPool_sp(dataPool)
, d_config(configuration)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntsa::Error error;

    BSLS_ASSERT_OPT(d_config.type().isNull() ||
                    d_config.type().value() == ntca::CompressionType::e_ZLIB);

    error = this->deflateCreate();
    BSLS_ASSERT_OPT(!error);

    error = this->inflateCreate();
    BSLS_ASSERT_OPT(!error);

    error = this->inflateReset();
    BSLS_ASSERT_OPT(!error);
}

Zlib::~Zlib()
{
    ntsa::Error error;

    error = this->inflateDestroy();
    BSLS_ASSERT_OPT(!error);

    error = this->deflateDestroy();
    BSLS_ASSERT_OPT(!error);

    d_dataPool_sp.reset();
}

ntca::CompressionType::Value Zlib::type() const
{
    return ntca::CompressionType::e_ZLIB;
}

#define NTCTLC_GZIP_DEFLATER_LOG_FRAME(checksum, header)                      \
    do {                                                                      \
        NTCI_LOG_DEBUG(                                                       \
            "Deflated frame "                                                 \
            "[ name = %s comment = %s time = %llu checksum = %u ]",           \
            (header).name,                                                    \
            (header).comment,                                                 \
            static_cast<bsl::uint64_t>((header).time),                        \
            static_cast<bsl::uint32_t>((checksum)));                          \
    } while (false)

#define NTCTLC_GZIP_INFLATER_LOG_FRAME(checksum, header)                      \
    do {                                                                      \
        NTCI_LOG_DEBUG(                                                       \
            "Inflated frame "                                                 \
            "[ name = %s comment = %s time = %llu checksum = %u ]",           \
            (header).name,                                                    \
            (header).comment,                                                 \
            static_cast<bsl::uint64_t>((header).time),                        \
            static_cast<bsl::uint32_t>((checksum)));                          \
    } while (false)

NTCCFG_INLINE
ntsa::Error Gzip::deflateCreate()
{
    int rc = 0;

    if (d_config.goal().has_value()) {
        switch (d_config.goal().value()) {
        case ntca::CompressionGoal::e_BEST_SIZE:  // 9
            d_level = Z_BEST_COMPRESSION;
            break;
        case ntca::CompressionGoal::e_BETTER_SIZE:
            d_level = 7;
            break;
        case ntca::CompressionGoal::e_BALANCED:
            d_level = Z_DEFAULT_COMPRESSION;  // 6
            break;
        case ntca::CompressionGoal::e_BETTER_SPEED:
            d_level = 4;
            break;
        case ntca::CompressionGoal::e_BEST_SPEED:
            d_level = Z_BEST_SPEED;  // 1
            break;
        default:
            break;
        }
    }

    bsl::memset(&d_deflaterStream, 0, sizeof d_deflaterStream);
    bsl::memset(&d_deflaterHeader, 0, sizeof d_deflaterHeader);
    bsl::memset(d_deflaterEntityName, 0, sizeof d_deflaterEntityName);
    bsl::memset(d_deflaterEntityComment, 0, sizeof d_deflaterEntityComment);

    d_deflaterStream.zalloc = &Gzip::allocate;
    d_deflaterStream.zfree  = &Gzip::free;
    d_deflaterStream.opaque = d_allocator_p;

#if defined(Z_PREFIX_SET)
    rc = z_deflateInit2(&d_deflaterStream,
                        d_level,
                        Z_DEFLATED,
                        31,
                        8,
                        Z_DEFAULT_STRATEGY);
#else
    rc = deflateInit2(&d_deflaterStream,
                      d_level,
                      Z_DEFLATED,
                      31,
                      8,
                      Z_DEFAULT_STRATEGY);
#endif

    if (rc != Z_OK) {
        return this->translateError(rc, "initialize deflater");
    }

    return ntsa::Error();
}

ntsa::Error Gzip::deflateBegin(ntca::DeflateContext*       context,
                               bdlbb::Blob*                result,
                               const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    ntsa::Error error;

    d_deflaterStream.next_in  = 0;
    d_deflaterStream.avail_in = 0;
    d_deflaterStream.total_in = 0;

    d_deflaterStream.next_out  = 0;
    d_deflaterStream.avail_out = 0;
    d_deflaterStream.total_out = 0;
    d_deflaterStream.adler     = 0;

    error = this->deflateReset();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Gzip::deflateNext(ntca::DeflateContext*       context,
                              bdlbb::Blob*                result,
                              const bsl::uint8_t*         data,
                              bsl::size_t                 size,
                              const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    int rc = 0;

    bsl::size_t totalBytesWritten = 0;
    bsl::size_t totalBytesRead    = 0;

    d_deflaterStream.next_in  = const_cast<bsl::uint8_t*>(data);
    d_deflaterStream.avail_in = static_cast<uInt>(size);

    while (d_deflaterStream.avail_in != 0) {
        if (d_deflaterStream.avail_out == 0) {
            this->deflateOverflow(result);
        }

        bsl::size_t numBytesRead    = 0;
        bsl::size_t numBytesWritten = 0;

        rc = this->deflateCycle(&numBytesRead, &numBytesWritten, Z_NO_FLUSH);

        totalBytesRead    += numBytesRead;
        totalBytesWritten += numBytesWritten;

        if (rc != Z_OK && rc != Z_BUF_ERROR) {
            return this->translateError(rc, "deflate");
        }
    }

    context->setBytesRead(context->bytesRead() + totalBytesRead);
    context->setBytesWritten(context->bytesWritten() + totalBytesWritten);

    return ntsa::Error();
}

ntsa::Error Gzip::deflateEnd(ntca::DeflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    NTCI_LOG_CONTEXT();

    int rc = 0;

    bsl::size_t totalBytesWritten = 0;
    bsl::size_t totalBytesRead    = 0;

    while (true) {
        if (d_deflaterStream.avail_out == 0) {
            this->deflateOverflow(result);
        }

        bsl::size_t numBytesRead    = 0;
        bsl::size_t numBytesWritten = 0;

        rc = this->deflateCycle(&numBytesRead, &numBytesWritten, Z_FINISH);

        totalBytesRead    += numBytesRead;
        totalBytesWritten += numBytesWritten;

        if (rc == Z_OK || rc == Z_BUF_ERROR) {
            continue;
        }
        else if (rc == Z_STREAM_END) {
            if (d_deflaterBufferSize != 0) {
                this->deflateCommit(result);
            }
            break;
        }
        else {
            return this->translateError(rc, "deflate");
        }
    }

    context->setCompressionType(ntca::CompressionType::e_GZIP);
    context->setBytesRead(context->bytesRead() + totalBytesRead);
    context->setBytesWritten(context->bytesWritten() + totalBytesWritten);

    bsl::uint32_t checksumValue =
        static_cast<bsl::uint32_t>(d_deflaterStream.adler);

    ntca::Checksum checksum;
    checksum.store(ntca::ChecksumType::e_CRC32,
                   &checksumValue,
                   sizeof checksumValue);

    context->setChecksum(checksum);

    NTCTLC_GZIP_DEFLATER_LOG_FRAME(checksumValue, d_deflaterHeader);

    return ntsa::Error();
}

NTCCFG_INLINE
void Gzip::deflateOverflow(bdlbb::Blob* result)
{
    if (d_deflaterBufferSize != 0) {
        this->deflateCommit(result);
    }

    d_deflaterBuffer.reset();
    d_deflaterBufferSize = 0;

    d_dataPool_sp->createOutgoingBlobBuffer(&d_deflaterBuffer);

    d_deflaterStream.next_out =
        reinterpret_cast<unsigned char*>(d_deflaterBuffer.data());
    d_deflaterStream.avail_out = d_deflaterBuffer.size();
}

NTCCFG_INLINE
void Gzip::deflateCommit(bdlbb::Blob* result)
{
    if (d_deflaterBufferSize != 0) {
        d_deflaterBuffer.setSize(static_cast<int>(d_deflaterBufferSize));

        BSLS_ASSERT(d_deflaterBuffer.buffer().get() != 0);
        BSLS_ASSERT(d_deflaterBuffer.size() > 0);

        NTCTLC_PLUGIN_LOG_DEFLATED_BLOB_BUFFER(d_deflaterBuffer);

        result->appendDataBuffer(d_deflaterBuffer);

        d_deflaterBuffer.reset();
        d_deflaterBufferSize = 0;

        d_deflaterStream.next_out  = 0;
        d_deflaterStream.avail_out = 0;
    }
}

NTCCFG_INLINE
int Gzip::deflateCycle(bsl::size_t* numBytesRead,
                       bsl::size_t* numBytesWritten,
                       int          mode)
{
    int rc = 0;

    *numBytesRead    = 0;
    *numBytesWritten = 0;

    uInt availIn0  = d_deflaterStream.avail_in;
    uInt availOut0 = d_deflaterStream.avail_out;

    rc = ::deflate(&d_deflaterStream, mode);

    uInt availIn1  = d_deflaterStream.avail_in;
    uInt availOut1 = d_deflaterStream.avail_out;

    BSLS_ASSERT(availIn0 >= availIn1);
    BSLS_ASSERT(availOut0 >= availOut1);

    uInt availInDiff  = availIn0 - availIn1;
    uInt availOutDiff = availOut0 - availOut1;

    *numBytesRead    = availInDiff;
    *numBytesWritten = availOutDiff;

    d_deflaterBufferSize += availOutDiff;

    return rc;
}

NTCCFG_INLINE
ntsa::Error Gzip::deflateReset()
{
    int rc = 0;

    rc = ::deflateReset(&d_deflaterStream);
    if (rc != Z_OK) {
        return this->translateError(rc, "reset deflater");
    }

    bsl::memset(&d_deflaterHeader, 0, sizeof d_deflaterHeader);
    bsl::memset(d_deflaterEntityName, 0, sizeof d_deflaterEntityName);
    bsl::memset(d_deflaterEntityComment, 0, sizeof d_deflaterEntityComment);

    bsl::uint64_t id = d_deflaterGeneration++;

    bsl::strcpy(reinterpret_cast<char*>(d_deflaterEntityName), "NTF");

    char* deflaterEntityCommentLast = bslalg::NumericFormatterUtil::toChars(
        d_deflaterEntityComment, 
        d_deflaterEntityComment + sizeof(d_deflaterEntityComment), 
        id);
    *deflaterEntityCommentLast = 0;

    d_deflaterHeader.name =
        reinterpret_cast<unsigned char*>(d_deflaterEntityName);
    d_deflaterHeader.name_max = 
        static_cast<uInt>(bsl::strlen(d_deflaterEntityName));

    d_deflaterHeader.comment =
        reinterpret_cast<unsigned char*>(d_deflaterEntityComment);
    d_deflaterHeader.comm_max = 
        static_cast<uInt>(bsl::strlen(d_deflaterEntityComment));

    rc = ::deflateSetHeader(&d_deflaterStream, &d_deflaterHeader);
    if (rc != Z_OK) {
        return this->translateError(rc, "set deflated header");
    }

    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error Gzip::deflateDestroy()
{
    int rc = 0;

    d_deflaterStream.next_in  = 0;
    d_deflaterStream.avail_in = 0;
    d_deflaterStream.total_in = 0;

    d_deflaterStream.next_out  = 0;
    d_deflaterStream.avail_out = 0;
    d_deflaterStream.total_out = 0;
    d_deflaterStream.adler     = 0;

    rc = ::deflateReset(&d_deflaterStream);
    if (rc != Z_OK) {
        Gzip::translateError(rc, "reset deflater");
    }

    rc = ::deflateEnd(&d_deflaterStream);
    if (rc != Z_OK) {
        Gzip::translateError(rc, "close deflater");
    }

    d_deflaterBuffer.reset();

    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error Gzip::inflateCreate()
{
    int rc = 0;

    bsl::memset(&d_inflaterStream, 0, sizeof d_inflaterStream);
    bsl::memset(&d_inflaterHeader, 0, sizeof d_inflaterHeader);
    bsl::memset(d_inflaterEntityName, 0, sizeof d_inflaterEntityName);
    bsl::memset(d_inflaterEntityComment, 0, sizeof d_inflaterEntityComment);

    d_inflaterStream.zalloc = &Gzip::allocate;
    d_inflaterStream.zfree  = &Gzip::free;
    d_inflaterStream.opaque = d_allocator_p;

#if defined(Z_PREFIX_SET)
    rc = z_inflateInit2(&d_inflaterStream, 31);
#else
    rc = inflateInit2(&d_inflaterStream, 31);
#endif

    if (rc != Z_OK) {
        return this->translateError(rc, "initialize inflater");
    }

    return ntsa::Error();
}

ntsa::Error Gzip::inflateBegin(ntca::InflateContext*       context,
                               bdlbb::Blob*                result,
                               const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error();
}

ntsa::Error Gzip::inflateNext(ntca::InflateContext*       context,
                              bdlbb::Blob*                result,
                              const bsl::uint8_t*         data,
                              bsl::size_t                 size,
                              const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc = 0;

    bsl::size_t totalBytesWritten = 0;
    bsl::size_t totalBytesRead    = 0;

    BSLS_ASSERT(d_inflaterStream.next_in == 0);
    BSLS_ASSERT(d_inflaterStream.avail_in == 0);

    d_inflaterStream.next_in  = const_cast<bsl::uint8_t*>(data);
    d_inflaterStream.avail_in = static_cast<uInt>(size);

    while (d_inflaterStream.avail_in != 0) {
        if (d_inflaterStream.avail_out == 0) {
            this->inflateOverflow(result);
        }

        bsl::size_t numBytesRead    = 0;
        bsl::size_t numBytesWritten = 0;

        rc = this->inflateCycle(&numBytesRead, &numBytesWritten, Z_NO_FLUSH);

        totalBytesRead    += numBytesRead;
        totalBytesWritten += numBytesWritten;

        if (rc == Z_OK || rc == Z_BUF_ERROR) {
            continue;
        }
        else if (rc == Z_STREAM_END) {
            if (d_inflaterBufferSize != 0) {
                this->inflateCommit(result);
            }

            NTCTLC_GZIP_INFLATER_LOG_FRAME(d_inflaterStream.adler,
                                           d_inflaterHeader);

            error = this->inflateReset();
            if (error) {
                return error;
            }

            continue;
        }
        else {
            return this->translateError(rc, "inflate");
        }
    }

    d_inflaterStream.next_in  = 0;
    d_inflaterStream.avail_in = 0;

    context->setBytesRead(context->bytesRead() + totalBytesRead);
    context->setBytesWritten(context->bytesWritten() + totalBytesWritten);

    return ntsa::Error();
}

ntsa::Error Gzip::inflateEnd(ntca::InflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc = 0;

    bsl::size_t totalBytesWritten = 0;
    bsl::size_t totalBytesRead    = 0;

    BSLS_ASSERT(d_inflaterStream.next_in == 0);
    BSLS_ASSERT(d_inflaterStream.avail_in == 0);

    while (true) {
        if (d_inflaterStream.avail_out == 0) {
            this->inflateOverflow(result);
        }

        bsl::size_t numBytesRead    = 0;
        bsl::size_t numBytesWritten = 0;

        rc = this->inflateCycle(&numBytesRead, &numBytesWritten, Z_SYNC_FLUSH);

        totalBytesRead    += numBytesRead;
        totalBytesWritten += numBytesWritten;

        if (rc == Z_OK || rc == Z_BUF_ERROR) {
            if (numBytesRead == 0 && numBytesWritten == 0) {
                break;
            }
            else {
                continue;
            }
        }
        else if (rc == Z_STREAM_END) {
            if (d_inflaterBufferSize != 0) {
                this->inflateCommit(result);
            }

            NTCTLC_GZIP_INFLATER_LOG_FRAME(d_inflaterStream.adler,
                                           d_inflaterHeader);

            error = this->inflateReset();
            if (error) {
                return error;
            }

            continue;
        }
        else {
            return this->translateError(rc, "inflate");
        }
    }

    d_inflaterStream.next_in  = 0;
    d_inflaterStream.avail_in = 0;

    context->setCompressionType(ntca::CompressionType::e_GZIP);
    context->setBytesRead(context->bytesRead() + totalBytesRead);
    context->setBytesWritten(context->bytesWritten() + totalBytesWritten);

    return ntsa::Error();
}

NTCCFG_INLINE
void Gzip::inflateOverflow(bdlbb::Blob* result)
{
    if (d_inflaterBufferSize != 0) {
        this->inflateCommit(result);
    }

    d_inflaterBuffer.reset();
    d_inflaterBufferSize = 0;

    d_dataPool_sp->createIncomingBlobBuffer(&d_inflaterBuffer);

    d_inflaterStream.next_out =
        reinterpret_cast<unsigned char*>(d_inflaterBuffer.data());
    d_inflaterStream.avail_out = d_inflaterBuffer.size();
}

NTCCFG_INLINE
void Gzip::inflateCommit(bdlbb::Blob* result)
{
    if (d_inflaterBufferSize != 0) {
        d_inflaterBuffer.setSize(static_cast<int>(d_inflaterBufferSize));

        BSLS_ASSERT(d_inflaterBuffer.buffer().get() != 0);
        BSLS_ASSERT(d_inflaterBuffer.size() > 0);

        NTCTLC_PLUGIN_LOG_INFLATED_BLOB_BUFFER(d_inflaterBuffer);

        result->appendDataBuffer(d_inflaterBuffer);

        d_inflaterBuffer.reset();
        d_inflaterBufferSize = 0;

        d_inflaterStream.next_out  = 0;
        d_inflaterStream.avail_out = 0;
    }
}

NTCCFG_INLINE
int Gzip::inflateCycle(bsl::size_t* numBytesRead,
                       bsl::size_t* numBytesWritten,
                       int          mode)
{
    int rc = 0;

    *numBytesRead    = 0;
    *numBytesWritten = 0;

    uInt availIn0  = d_inflaterStream.avail_in;
    uInt availOut0 = d_inflaterStream.avail_out;

    rc = ::inflate(&d_inflaterStream, mode);

    uInt availIn1  = d_inflaterStream.avail_in;
    uInt availOut1 = d_inflaterStream.avail_out;

    BSLS_ASSERT(availIn0 >= availIn1);
    BSLS_ASSERT(availOut0 >= availOut1);

    uInt availInDiff  = availIn0 - availIn1;
    uInt availOutDiff = availOut0 - availOut1;

    *numBytesRead    = availInDiff;
    *numBytesWritten = availOutDiff;

    d_inflaterBufferSize += availOutDiff;

    return rc;
}

NTCCFG_INLINE
ntsa::Error Gzip::inflateReset()
{
    int rc = 0;

    rc = ::inflateReset2(&d_inflaterStream, 31);
    if (rc != Z_OK) {
        return this->translateError(rc, "reset inflater");
    }

    ++d_inflaterGeneration;

    bsl::memset(&d_inflaterHeader, 0, sizeof d_inflaterHeader);
    bsl::memset(d_inflaterEntityName, 0, sizeof d_inflaterEntityName);
    bsl::memset(d_inflaterEntityComment, 0, sizeof d_inflaterEntityComment);

    d_inflaterHeader.name =
        reinterpret_cast<unsigned char*>(d_inflaterEntityName);
    d_inflaterHeader.name_max = sizeof d_inflaterEntityName - 1;

    d_inflaterHeader.comment =
        reinterpret_cast<unsigned char*>(d_inflaterEntityComment);
    d_inflaterHeader.comm_max = sizeof d_inflaterEntityComment - 1;

    rc = ::inflateGetHeader(&d_inflaterStream, &d_inflaterHeader);
    if (rc != Z_OK) {
        return this->translateError(rc, "get inflated header");
    }

    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error Gzip::inflateDestroy()
{
    int rc = 0;

    d_inflaterStream.next_in  = 0;
    d_inflaterStream.avail_in = 0;
    d_inflaterStream.total_in = 0;

    d_inflaterStream.next_out  = 0;
    d_inflaterStream.avail_out = 0;
    d_inflaterStream.total_out = 0;
    d_inflaterStream.adler     = 0;

    rc = ::inflateReset(&d_inflaterStream);
    if (rc != Z_OK) {
        Gzip::translateError(rc, "reset inflater");
    }

    rc = ::inflateEnd(&d_inflaterStream);
    if (rc != Z_OK) {
        Gzip::translateError(rc, "close inflater");
    }

    d_inflaterBuffer.reset();

    return ntsa::Error();
}

void* Gzip::allocate(void* opaque, unsigned int number, unsigned int size)
{
    bslma::Allocator* allocator = reinterpret_cast<bslma::Allocator*>(opaque);

    return allocator->allocate(number * size);
}

void Gzip::free(void* opaque, void* address)
{
    bslma::Allocator* allocator = reinterpret_cast<bslma::Allocator*>(opaque);

    allocator->deallocate(address);
}

const char* Gzip::describeError(int error)
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

ntsa::Error Gzip::translateError(int error, const char* operation)
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

    const char* errorDescription = Gzip::describeError(error);

    if (result) {
        NTCI_LOG_STREAM_WARN << "Failed to " << operation << ": "
                             << errorDescription << " (rc = " << error << ")"
                             << NTCI_LOG_STREAM_END;
    }

    return result;
}

Gzip::Gzip(const ntca::CompressionConfig&         configuration,
           const bsl::shared_ptr<ntci::DataPool>& dataPool,
           bslma::Allocator*                      basicAllocator)
: d_deflaterBuffer()
, d_deflaterBufferSize(0)
, d_deflaterGeneration(0)
, d_inflaterBuffer()
, d_inflaterBufferSize(0)
, d_inflaterGeneration(0)
, d_level(Z_DEFAULT_COMPRESSION)
, d_dataPool_sp(dataPool)
, d_config(configuration)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntsa::Error error;

    BSLS_ASSERT_OPT(d_config.type().isNull() ||
                    d_config.type().value() == ntca::CompressionType::e_GZIP);

    error = this->deflateCreate();
    BSLS_ASSERT_OPT(!error);

    error = this->inflateCreate();
    BSLS_ASSERT_OPT(!error);

    error = this->inflateReset();
    BSLS_ASSERT_OPT(!error);
}

Gzip::~Gzip()
{
    ntsa::Error error;

    error = this->inflateDestroy();
    BSLS_ASSERT_OPT(!error);

    error = this->deflateDestroy();
    BSLS_ASSERT_OPT(!error);

    d_dataPool_sp.reset();
}

ntca::CompressionType::Value Gzip::type() const
{
    return ntca::CompressionType::e_GZIP;
}

#endif

CompressionDriver::CompressionDriver(bslma::Allocator* basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
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

void Plugin::initialize(bslma::Allocator* basicAllocator)
{
    BSLMT_ONCE_DO
    {
        bslma::Allocator* allocator = basicAllocator;
        if (allocator == 0) {
            allocator = &bslma::NewDeleteAllocator::singleton();
        }

        bsl::shared_ptr<ntci::CompressionDriver> compressionDriver(
            new (*allocator) ntctlc::CompressionDriver(allocator),
            allocator);

        ntcs::Plugin::registerCompressionDriver(compressionDriver);
    }
}

void Plugin::load(bsl::shared_ptr<ntci::CompressionDriver>* result)
{
    ntsa::Error error;

    ntctlc::Plugin::initialize(&bslma::NewDeleteAllocator::singleton());

    error = ntcs::Plugin::lookupCompressionDriver(result);
    if (error) {
        bslma::Allocator* allocator = &bslma::NewDeleteAllocator::singleton();

        bsl::shared_ptr<ntctlc::CompressionDriver> driver;
        driver.createInplace(allocator, allocator);

        *result = driver;
    }
}

void Plugin::exit()
{
}

PluginGuard::PluginGuard(bslma::Allocator* basicAllocator)
{
    ntctlc::Plugin::initialize(basicAllocator);
}

PluginGuard::~PluginGuard()
{
    ntctlc::Plugin::exit();
}

}  // close namespace ntctlc
}  // close namespace BloombergLP
