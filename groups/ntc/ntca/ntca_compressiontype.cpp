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

#include <ntca_compressiontype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_compressiontype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int CompressionType::fromInt(CompressionType::Value* result, int number)
{
    switch (number) {
    case CompressionType::e_UNDEFINED:
    case CompressionType::e_NONE:
    case CompressionType::e_RLE:
    case CompressionType::e_ZLIB:
    case CompressionType::e_GZIP:
    case CompressionType::e_LZ4:
    case CompressionType::e_ZSTD:
        *result = static_cast<CompressionType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int CompressionType::fromString(CompressionType::Value*  result,
                                const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "NONE")) {
        *result = e_NONE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "RLE")) {
        *result = e_RLE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ZLIB")) {
        *result = e_ZLIB;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "GZIP")) {
        *result = e_GZIP;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "LZ4")) {
        *result = e_LZ4;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ZSTD")) {
        *result = e_ZSTD;
        return 0;
    }

    return -1;
}

const char* CompressionType::toString(CompressionType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_NONE: {
        return "NONE";
    } break;
    case e_RLE: {
        return "RLE";
    } break;
    case e_ZLIB: {
        return "ZLIB";
    } break;
    case e_GZIP: {
        return "GZIP";
    } break;
    case e_LZ4: {
        return "LZ4";
    } break;
    case e_ZSTD: {
        return "ZSTD";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& CompressionType::print(bsl::ostream&          stream,
                                     CompressionType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, CompressionType::Value rhs)
{
    return CompressionType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
