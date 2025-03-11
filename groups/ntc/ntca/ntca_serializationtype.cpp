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

#include <ntca_serializationtype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_serializationtype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int SerializationType::fromInt(SerializationType::Value* result, int number)
{
    switch (number) {
    case SerializationType::e_UNDEFINED:
    case SerializationType::e_NONE:
    case SerializationType::e_BER:
    case SerializationType::e_JSON:
    case SerializationType::e_XML:
        *result = static_cast<SerializationType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int SerializationType::fromString(SerializationType::Value* result,
                                  const bslstl::StringRef&  string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "NONE")) {
        *result = e_NONE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "BER")) {
        *result = e_BER;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "JSON")) {
        *result = e_JSON;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "XML")) {
        *result = e_XML;
        return 0;
    }

    return -1;
}

const char* SerializationType::toString(SerializationType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_NONE: {
        return "NONE";
    } break;
    case e_BER: {
        return "BER";
    } break;
    case e_JSON: {
        return "JSON";
    } break;
    case e_XML: {
        return "XML";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& SerializationType::print(bsl::ostream&            stream,
                                       SerializationType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, SerializationType::Value rhs)
{
    return SerializationType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
