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

#ifndef INCLUDED_NTCI_SERIALIZATION
#define INCLUDED_NTCI_SERIALIZATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_serializationconfig.h>
#include <ntca_serializationtype.h>
#include <ntccfg_platform.h>
#include <ntci_log.h>
#include <ntcscm_version.h>
#include <ntsa_buffer.h>
#include <ntsa_data.h>
#include <ntsa_error.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobstreambuf.h>
#include <bdlbb_blobutil.h>
#include <bsl_istream.h>
#include <bsl_memory.h>
#include <bsl_ostream.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>
#include <balber_berdecoder.h>
#include <balber_berdecoderoptions.h>
#include <balber_berencoder.h>
#include <balber_berencoderoptions.h>
#include <baljsn_decoder.h>
#include <baljsn_decoderoptions.h>
#include <baljsn_encoder.h>
#include <baljsn_encoderoptions.h>
#include <balxml_decoder.h>
#include <balxml_decoderoptions.h>
#include <balxml_encoder.h>
#include <balxml_encoderoptions.h>
#include <balxml_minireader.h>

namespace BloombergLP {
namespace ntci {

/// Provide a mechanism to serialize and deserialize introspectible types.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_todo
class Serialization
{
    balber::BerEncoderOptions d_berEncoderOptions;
    balber::BerDecoderOptions d_berDecoderOptions;
    baljsn::EncoderOptions    d_jsnEncoderOptions;
    baljsn::DecoderOptions    d_jsnDecoderOptions;
    balxml::EncoderOptions    d_xmlEncoderOptions;
    balxml::DecoderOptions    d_xmlDecoderOptions;
    bslma::Allocator*         d_allocator_p;

  private:
    Serialization(const Serialization&) BSLS_KEYWORD_DELETED;
    Serialization& operator=(const Serialization&) BSLS_KEYWORD_DELETED;

  private:
    /// Initialize the specified 'berEncoderOptions' to their default values.
    void initialize(balber::BerEncoderOptions* berEncoderOptions);

    /// Initialize the specified 'berDecoderOptions' to their default values.
    void initialize(balber::BerDecoderOptions* berDecoderOptions);

    /// Initialize the specified 'jsnEncoderOptions' to their default values.
    void initialize(baljsn::EncoderOptions* jsnEncoderOptions);

    /// Initialize the specified 'jsnDecoderOptions' to their default values.
    void initialize(baljsn::DecoderOptions* jsnDecoderOptions);

    /// Initialize the specified 'xmlEncoderOptions' to their default values.
    void initialize(balxml::EncoderOptions* xmlEncoderOptions);

    /// Initialize the specified 'xmlDecoderOptions' to their default values.
    void initialize(balxml::DecoderOptions* xmlDecoderOptions);

  public:
    /// Create a new serialization mechanism with a default configuration.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit Serialization(bslma::Allocator* basicAllocator = 0);

    /// Create a new serialization mechanism with the specified
    /// 'configuration'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit Serialization(const ntca::SerializationConfig& configuration,
                           bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Serialization();

    /// Encode the specified 'source' to the specified 'destination' in the
    /// specified 'encoding'. Return the error.
    template <typename TYPE>
    ntsa::Error encode(bdlbb::Blob*                   destination,
                       const TYPE&                    source,
                       ntca::SerializationType::Value encoding);

    /// Encode the specified 'source' to the specified 'destination' in the
    /// specified 'encoding'. Return the error.
    template <typename TYPE>
    ntsa::Error encode(bsl::streambuf*                destination,
                       const TYPE&                    source,
                       ntca::SerializationType::Value encoding);

    /// Decode the specified 'source' to the specified 'destination' in the
    /// specified 'encoding'. Return the error.
    template <typename TYPE>
    ntsa::Error decode(TYPE*                          destination,
                       const bdlbb::Blob&             source,
                       ntca::SerializationType::Value encoding);

    /// Decode the specified 'source' to the specified 'destination' in the
    /// specified 'encoding'. Return the error.
    template <typename TYPE>
    ntsa::Error decode(TYPE*                          destination,
                       bsl::streambuf*                source,
                       ntca::SerializationType::Value encoding);

    /// Return the sesrialization type indicated by the specified 'contentType'
    /// and 'contentSubtype'.
    static ntca::SerializationType::Value type(
        const bsl::string& contentType,
        const bsl::string& contentSubtype);
};

template <typename TYPE>
ntsa::Error Serialization::encode(bdlbb::Blob*                   destination,
                                  const TYPE&                    source,
                                  ntca::SerializationType::Value encoding)
{
    bdlbb::OutBlobStreamBuf osb(destination);
    return Serialization::encode(&osb, source, encoding);
}

template <typename TYPE>
ntsa::Error Serialization::encode(bsl::streambuf*                destination,
                                  const TYPE&                    source,
                                  ntca::SerializationType::Value encoding)
{
    NTCI_LOG_CONTEXT();

    int rc;

    if (encoding == ntca::SerializationType::e_BER) {
        balber::BerEncoder berEncoder(&d_berEncoderOptions, d_allocator_p);

        rc = berEncoder.encode(destination, source);
        if (rc != 0) {
            bsl::string diagnostics = berEncoder.loggedMessages();
            NTCI_LOG_STREAM_ERROR << "Failed to encode to BER: " << diagnostics
                                  << NTCI_LOG_STREAM_END;

            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (encoding == ntca::SerializationType::e_JSON) {
        baljsn::Encoder jsnEncoder(d_allocator_p);

        rc = jsnEncoder.encode(destination, source, &d_jsnEncoderOptions);
        if (rc != 0) {
            bsl::string diagnostics = jsnEncoder.loggedMessages();
            NTCI_LOG_STREAM_ERROR
                << "Failed to encode to JSON: " << diagnostics
                << NTCI_LOG_STREAM_END;

            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (encoding == ntca::SerializationType::e_XML) {
        balxml::Encoder xmlEncoder(&d_xmlEncoderOptions, d_allocator_p);

        rc = xmlEncoder.encode(destination, source);
        if (rc != 0) {
            bsl::string diagnostics = xmlEncoder.loggedMessages();
            NTCI_LOG_STREAM_ERROR << "Failed to encode to XML: " << diagnostics
                                  << NTCI_LOG_STREAM_END;

            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        NTCI_LOG_STREAM_ERROR << "Unsupported codec encoding: " << encoding
                              << NTCI_LOG_STREAM_END;

        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    rc = destination->pubsync();
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

template <typename TYPE>
ntsa::Error Serialization::decode(TYPE*                          destination,
                                  const bdlbb::Blob&             source,
                                  ntca::SerializationType::Value encoding)
{
    bdlbb::InBlobStreamBuf isb(&source);
    return Serialization::decode(destination, &isb, encoding);
}

template <typename TYPE>
ntsa::Error Serialization::decode(TYPE*                          destination,
                                  bsl::streambuf*                source,
                                  ntca::SerializationType::Value encoding)
{
    NTCI_LOG_CONTEXT();

    int rc;

    if (encoding == ntca::SerializationType::e_BER) {
        balber::BerDecoder berDecoder(&d_berDecoderOptions, d_allocator_p);

        rc = berDecoder.decode(source, destination);
        if (rc != 0) {
            bsl::string diagnostics = berDecoder.loggedMessages();
            NTCI_LOG_STREAM_ERROR
                << "Failed to decode from BER: " << diagnostics
                << NTCI_LOG_STREAM_END;
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (encoding == ntca::SerializationType::e_JSON) {
        baljsn::Decoder jsnDecoder(d_allocator_p);

        rc = jsnDecoder.decode(source, destination, &d_jsnDecoderOptions);
        if (rc != 0) {
            bsl::string diagnostics = jsnDecoder.loggedMessages();
            NTCI_LOG_STREAM_ERROR
                << "Failed to decode from JSON: " << diagnostics
                << NTCI_LOG_STREAM_END;
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (encoding == ntca::SerializationType::e_XML) {
        balxml::MiniReader xmlReader(d_allocator_p);
        balxml::ErrorInfo  xmlErrorInfo(d_allocator_p);

        balxml::Decoder xmlDecoder(&d_xmlDecoderOptions,
                                   &xmlReader,
                                   &xmlErrorInfo,
                                   d_allocator_p);

        rc = xmlDecoder.decode(source, destination);
        if (rc != 0) {
            bsl::string diagnostics = xmlDecoder.loggedMessages();
            NTCI_LOG_STREAM_ERROR
                << "Failed to decode from XML: " << diagnostics
                << NTCI_LOG_STREAM_END;
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        NTCI_LOG_STREAM_ERROR << "Unsupported serialization type: " << encoding
                              << NTCI_LOG_STREAM_END;

        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    return ntsa::Error();
}

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
