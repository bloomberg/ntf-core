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

#include <ntci_serialization.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_serialization_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntci {

void Serialization::initialize(balber::BerEncoderOptions* berEncoderOptions)
{
    berEncoderOptions->setTraceLevel(5);
    berEncoderOptions->setEncodeEmptyArrays(false);
    berEncoderOptions->setEncodeDateAndTimeTypesAsBinary(true);
    berEncoderOptions->setPreserveSignOfNegativeZero(true);
    berEncoderOptions->setDisableUnselectedChoiceEncoding(true);
    berEncoderOptions->setDatetimeFractionalSecondPrecision(6);
}

void Serialization::initialize(balber::BerDecoderOptions* berDecoderOptions)
{
    berDecoderOptions->setMaxDepth(128);
    berDecoderOptions->setMaxSequenceSize(1024 * 1024 * 256);
    berDecoderOptions->setSkipUnknownElements(true);
    berDecoderOptions->setTraceLevel(5);
    berDecoderOptions->setDefaultEmptyStrings(false);
}

void Serialization::initialize(baljsn::EncoderOptions* jsnEncoderOptions)
{
    jsnEncoderOptions->setDatetimeFractionalSecondPrecision(6);
    jsnEncoderOptions->setEncodeEmptyArrays(false);
    jsnEncoderOptions->setEncodeNullElements(false);
    jsnEncoderOptions->setEncodeInfAndNaNAsStrings(true);
    jsnEncoderOptions->setEncodeQuotedDecimal64(true);
    jsnEncoderOptions->setEncodingStyle(baljsn::EncoderOptions::e_COMPACT);
    jsnEncoderOptions->setInitialIndentLevel(0);
    jsnEncoderOptions->setSpacesPerLevel(4);
}

void Serialization::initialize(baljsn::DecoderOptions* jsnDecoderOptions)
{
    jsnDecoderOptions->setMaxDepth(128);
    jsnDecoderOptions->setSkipUnknownElements(true);
    jsnDecoderOptions->setValidateInputIsUtf8(false);
}

void Serialization::initialize(balxml::EncoderOptions* xmlEncoderOptions)
{
    xmlEncoderOptions->setEncodingStyle(balxml::EncodingStyle::e_COMPACT);
    xmlEncoderOptions->setDatetimeFractionalSecondPrecision(6);
    xmlEncoderOptions->setInitialIndentLevel(0);
    xmlEncoderOptions->setSpacesPerLevel(4);
    xmlEncoderOptions->setOutputXMLHeader(true);
    xmlEncoderOptions->setOutputXSIAlias(false);
    xmlEncoderOptions->setUseZAbbreviationForUtc(true);
    xmlEncoderOptions->setObjectNamespace("");
    xmlEncoderOptions->setTag("object");
}

void Serialization::initialize(balxml::DecoderOptions* xmlDecoderOptions)
{
    xmlDecoderOptions->setMaxDepth(128);
    xmlDecoderOptions->setSkipUnknownElements(true);
    xmlDecoderOptions->setValidateInputIsUtf8(false);
}

Serialization::Serialization(bslma::Allocator* basicAllocator)
: d_berEncoderOptions()
, d_berDecoderOptions()
, d_jsnEncoderOptions()
, d_jsnDecoderOptions()
, d_xmlEncoderOptions(basicAllocator)
, d_xmlDecoderOptions()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->initialize(&d_berEncoderOptions);
    this->initialize(&d_berDecoderOptions);
    this->initialize(&d_jsnEncoderOptions);
    this->initialize(&d_jsnDecoderOptions);
    this->initialize(&d_xmlEncoderOptions);
    this->initialize(&d_xmlDecoderOptions);
}

Serialization::Serialization(const ntca::SerializationConfig& configuration,
                             bslma::Allocator*                basicAllocator)
: d_berEncoderOptions()
, d_berDecoderOptions()
, d_jsnEncoderOptions()
, d_jsnDecoderOptions()
, d_xmlEncoderOptions(basicAllocator)
, d_xmlDecoderOptions()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (configuration.berEncoderOptions().has_value()) {
        d_berEncoderOptions = configuration.berEncoderOptions().value();
    }
    else {
        this->initialize(&d_berEncoderOptions);
    }

    if (configuration.berDecoderOptions().has_value()) {
        d_berDecoderOptions = configuration.berDecoderOptions().value();
    }
    else {
        this->initialize(&d_berDecoderOptions);
    }

    if (configuration.jsnEncoderOptions().has_value()) {
        d_jsnEncoderOptions = configuration.jsnEncoderOptions().value();
    }
    else {
        this->initialize(&d_jsnEncoderOptions);
    }

    if (configuration.jsnDecoderOptions().has_value()) {
        d_jsnDecoderOptions = configuration.jsnDecoderOptions().value();
    }
    else {
        this->initialize(&d_jsnDecoderOptions);
    }

    if (configuration.xmlEncoderOptions().has_value()) {
        d_xmlEncoderOptions = configuration.xmlEncoderOptions().value();
    }
    else {
        this->initialize(&d_xmlEncoderOptions);
    }

    if (configuration.xmlDecoderOptions().has_value()) {
        d_xmlDecoderOptions = configuration.xmlDecoderOptions().value();
    }
    else {
        this->initialize(&d_xmlDecoderOptions);
    }
}

Serialization::~Serialization()
{
}

ntca::SerializationType::Value Serialization::type(
    const bsl::string& contentType,
    const bsl::string& contentSubtype)
{
    if (contentType == "application") {
        if (contentSubtype == "octet-stream") {
            return ntca::SerializationType::e_NONE;
        }
        else if (contentSubtype == "ber") {
            return ntca::SerializationType::e_BER;
        }
        else if (contentSubtype == "json") {
            return ntca::SerializationType::e_JSON;
        }
        else if (contentSubtype == "xml") {
            return ntca::SerializationType::e_XML;
        }
    }
    else if (contentType == "text") {
        if (contentSubtype == "json") {
            return ntca::SerializationType::e_JSON;
        }
        else if (contentSubtype == "xml") {
            return ntca::SerializationType::e_XML;
        }
    }

    return ntca::SerializationType::e_UNDEFINED;
}

}  // close package namespace
}  // close enterprise namespace
