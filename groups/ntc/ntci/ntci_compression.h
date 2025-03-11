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

#ifndef INCLUDED_NTCI_COMPRESSION
#define INCLUDED_NTCI_COMPRESSION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_compressionconfig.h>
#include <ntca_compressiongoal.h>
#include <ntca_compressiontype.h>
#include <ntca_deflatecontext.h>
#include <ntca_deflateoptions.h>
#include <ntca_inflatecontext.h>
#include <ntca_inflateoptions.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_buffer.h>
#include <ntsa_data.h>
#include <ntsa_error.h>
#include <bdlbb_blob.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to a deflation/inflation data transformation stage.
///
/// @details
/// This class provides a mechanism to reduce the size of a data stream
/// according to a compression technique.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par Usage Example
/// This example illustrates how to create a compression mechanism and use it
/// to deflate data to a (usually) reduced size, then inflate it back to its
/// original form.
///
/// First, create a compression mechanism using a particular compression 
/// technique. For the purposes of this example, we'll choose the popular
/// "DEFLATE" algorithm framed by the "zlib" framing protocol as described in
/// RFC 1950. For brevity, error handling is elided from this example.
///
///     ntsa::Error error;
///
///
///     ntca::CompressionConfig compressionConfig;
///     compressionConfig.setType(ntca::CompressionType::e_ZLIB);
///     compressionConfig.setGoal(ntca::CompressionGoal::e_BALANCED);
///
///     bsl::shared_ptr<ntci::Compression> compression;
///     error = ntcf::System::createCompression(&compression,
///                                      compressionConfig);
///     BSLS_ASSERT(!error);
///
/// Next, let's initialize some data that we want to compress. We'll choose
/// something with a repeating pattern, which most compression algorithms
/// usually reduce to somthing smaller.
///
///     bdlbb::SimpleBlobBufferFactory blobBufferFactory(64);
///
///     const char k_DATA[] = "abbcccddddeeeffg";
///
///     bdlbb::Blob initial(&blobBufferFactory);
///     bdlbb::BlobUtil::append(&initial, k_DATA, sizeof k_DATA - 1);
///
/// Then, deflate our original data in to a (hopefully) reduced size.
///
///     ntca::DeflateContext deflateContext;
///     ntca::DeflateOptions deflateOptions;
///
///     bdlbb::Blob deflated(&blobBufferFactory);
///     error = compression->deflate(&deflateContext,
///                                  &deflated,
///                                  initial,
///                                  deflateOptions);
///     NTSCFG_TEST_OK(error);
///
/// Now, inflate the deflated data.
///
///     ntca::InflateContext inflateContext;
///     ntca::InflateOptions inflateOptions;
///
///     bdlbb::Blob inflated(&blobBufferFactory);
///     error = compression->inflate(&inflateContext,
///                                  &inflated,
///                                  deflated,
///                                  inflateOptions);
///     NTSCFG_TEST_OK(error);
///
/// Finally, ensure the inflated data matches our original data.
///
///     NTSCFG_TEST_EQ(bdlbb::BlobUtil::compare(inflated, initial), 0);
///
/// @ingroup module_ntci_compression
class Compression
{
  private:
    /// Deflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error deflateRep(ntca::DeflateContext*       context,
                           bdlbb::Blob*                result,
                           const bdlbb::Blob&          data,
                           const ntca::DeflateOptions& options);

    /// Deflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error deflateRep(ntca::DeflateContext*       context,
                           bdlbb::Blob*                result,
                           const bdlbb::BlobBuffer&    data,
                           const ntca::DeflateOptions& options);

    /// Deflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error deflateRep(ntca::DeflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntsa::ConstBuffer&    data,
                           const ntca::DeflateOptions& options);

    /// Deflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error deflateRep(ntca::DeflateContext*         context,
                           bdlbb::Blob*                  result,
                           const ntsa::ConstBufferArray& data,
                           const ntca::DeflateOptions&   options);

    /// Deflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error deflateRep(ntca::DeflateContext*            context,
                           bdlbb::Blob*                     result,
                           const ntsa::ConstBufferPtrArray& data,
                           const ntca::DeflateOptions&      options);

    /// Deflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error deflateRep(ntca::DeflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntsa::MutableBuffer&  data,
                           const ntca::DeflateOptions& options);

    /// Deflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error deflateRep(ntca::DeflateContext*           context,
                           bdlbb::Blob*                    result,
                           const ntsa::MutableBufferArray& data,
                           const ntca::DeflateOptions&     options);

    /// Deflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error deflateRep(ntca::DeflateContext*              context,
                           bdlbb::Blob*                       result,
                           const ntsa::MutableBufferPtrArray& data,
                           const ntca::DeflateOptions&        options);

    /// Deflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error deflateRep(ntca::DeflateContext*       context,
                           bdlbb::Blob*                result,
                           const bsl::string&          data,
                           const ntca::DeflateOptions& options);

    /// Deflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error deflateRep(ntca::DeflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntsa::File&           data,
                           const ntca::DeflateOptions& options);

    /// Inflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error inflateRep(ntca::InflateContext*       context,
                           bdlbb::Blob*                result,
                           const bdlbb::Blob&          data,
                           const ntca::InflateOptions& options);

    /// Inflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error inflateRep(ntca::InflateContext*       context,
                           bdlbb::Blob*                result,
                           const bdlbb::BlobBuffer&    data,
                           const ntca::InflateOptions& options);

    /// Inflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error inflateRep(ntca::InflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntsa::ConstBuffer&    data,
                           const ntca::InflateOptions& options);

    /// Inflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error inflateRep(ntca::InflateContext*         context,
                           bdlbb::Blob*                  result,
                           const ntsa::ConstBufferArray& data,
                           const ntca::InflateOptions&   options);

    /// Inflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error inflateRep(ntca::InflateContext*            context,
                           bdlbb::Blob*                     result,
                           const ntsa::ConstBufferPtrArray& data,
                           const ntca::InflateOptions&      options);

    /// Inflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error inflateRep(ntca::InflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntsa::MutableBuffer&  data,
                           const ntca::InflateOptions& options);

    /// Inflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error inflateRep(ntca::InflateContext*           context,
                           bdlbb::Blob*                    result,
                           const ntsa::MutableBufferArray& data,
                           const ntca::InflateOptions&     options);

    /// Inflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error inflateRep(ntca::InflateContext*              context,
                           bdlbb::Blob*                       result,
                           const ntsa::MutableBufferPtrArray& data,
                           const ntca::InflateOptions&        options);

    /// Inflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error inflateRep(ntca::InflateContext*       context,
                           bdlbb::Blob*                result,
                           const bsl::string&          data,
                           const ntca::InflateOptions& options);

    /// Inflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error inflateRep(ntca::InflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntsa::File&           data,
                           const ntca::InflateOptions& options);

  protected:
    /// Begin a deflation stream into the specified 'result' according to the
    /// specified 'options.
    virtual ntsa::Error deflateBegin(ntca::DeflateContext*       context,
                                     bdlbb::Blob*                result,
                                     const ntca::DeflateOptions& options);

    /// Deflate the specified 'data' having the specified 'size' and append the
    /// result to the specified 'result'. Return the error.
    virtual ntsa::Error deflateNext(ntca::DeflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const bsl::uint8_t*         data,
                                    bsl::size_t                 size,
                                    const ntca::DeflateOptions& options);

    /// End a deflation stream into the specified 'result' according to the
    /// specified 'options.
    virtual ntsa::Error deflateEnd(ntca::DeflateContext*       context,
                                   bdlbb::Blob*                result,
                                   const ntca::DeflateOptions& options);

    /// Begin an inflation stream into the specified 'result' according to the
    /// specified 'options.
    virtual ntsa::Error inflateBegin(ntca::InflateContext*       context,
                                     bdlbb::Blob*                result,
                                     const ntca::InflateOptions& options);

    /// Inflate the specified 'data' having the specified 'size' and append the
    /// result to the specified 'result'. Return the error.
    virtual ntsa::Error inflateNext(ntca::InflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const bsl::uint8_t*         data,
                                    bsl::size_t                 size,
                                    const ntca::InflateOptions& options);

    /// Inflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    virtual ntsa::Error inflateNext(ntca::InflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const bdlbb::Blob&          data,
                                    const ntca::InflateOptions& options);

    /// End an inflation stream into the specified 'result' according to the
    /// specified 'options.
    virtual ntsa::Error inflateEnd(ntca::InflateContext*       context,
                                   bdlbb::Blob*                result,
                                   const ntca::InflateOptions& options);

  public:
    /// Destroy this object.
    virtual ~Compression();

    /// Deflate the specified 'data' according to the specified 'options' and
    /// append the result to the specified 'result'. Load the context in which
    /// the operation completes into the specified 'context'. Return the error.
    ntsa::Error deflate(ntca::DeflateContext*       context,
                        bdlbb::Blob*                result,
                        const bdlbb::Blob&          data,
                        const ntca::DeflateOptions& options);

    /// Deflate the specified 'data' according to the specified 'options' and
    /// append the result to the specified 'result'. Load the context in which
    /// the operation completes into the specified 'context'. Return the error.
    ntsa::Error deflate(ntca::DeflateContext*       context,
                        bdlbb::Blob*                result,
                        const ntsa::Data&           data,
                        const ntca::DeflateOptions& options);

    /// Inflate the specified 'data' according to the specified 'options' and
    /// append the result to the specified 'result'. Load the context in which
    /// the operation completes into the specified 'context'. Return the error.
    ntsa::Error inflate(ntca::InflateContext*       context,
                        bdlbb::Blob*                result,
                        const bdlbb::Blob&          data,
                        const ntca::InflateOptions& options);

    /// Inflate the specified 'data' according to the specified 'options' and
    /// append the result to the specified 'result'. Load the context in which
    /// the operation completes into the specified 'context'. Return the error.
    ntsa::Error inflate(ntca::InflateContext*       context,
                        bdlbb::Blob*                result,
                        const ntsa::Data&           data,
                        const ntca::InflateOptions& options);

    /// Return the compression type implemented by this mechanism.
    virtual ntca::CompressionType::Value type() const;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
