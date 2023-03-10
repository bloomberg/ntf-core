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

#include <ntsa_datatype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_datatype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>

namespace BloombergLP {
namespace ntsa {

int DataType::fromInt(DataType::Value* result, int number)
{
    switch (number) {
    case DataType::e_UNDEFINED:
    case DataType::e_BLOB_BUFFER:
    case DataType::e_CONST_BUFFER:
    case DataType::e_CONST_BUFFER_ARRAY:
    case DataType::e_CONST_BUFFER_PTR_ARRAY:
    case DataType::e_MUTABLE_BUFFER:
    case DataType::e_MUTABLE_BUFFER_ARRAY:
    case DataType::e_MUTABLE_BUFFER_PTR_ARRAY:
    case DataType::e_BLOB:
    case DataType::e_SHARED_BLOB:
    case DataType::e_STRING:
    case DataType::e_FILE:
        *result = static_cast<DataType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int DataType::fromString(DataType::Value*         result,
                         const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "BLOB_BUFFER")) {
        *result = e_BLOB_BUFFER;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CONST_BUFFER")) {
        *result = e_CONST_BUFFER;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CONST_BUFFER_ARRAY")) {
        *result = e_CONST_BUFFER_ARRAY;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CONST_BUFFER_PTR_ARRAY")) {
        *result = e_CONST_BUFFER_PTR_ARRAY;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "MUTABLE_BUFFER")) {
        *result = e_MUTABLE_BUFFER;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "MUTABLE_BUFFER_ARRAY")) {
        *result = e_MUTABLE_BUFFER_ARRAY;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "MUTABLE_BUFFER_PTR_ARRAY")) {
        *result = e_MUTABLE_BUFFER_PTR_ARRAY;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "BLOB")) {
        *result = e_BLOB;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SHARED_BLOB")) {
        *result = e_SHARED_BLOB;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "STRING")) {
        *result = e_STRING;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "FILE")) {
        *result = e_FILE;
        return 0;
    }

    return -1;
}

const char* DataType::toString(DataType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_BLOB_BUFFER: {
        return "BLOB_BUFFER";
    } break;
    case e_CONST_BUFFER: {
        return "CONST_BUFFER";
    } break;
    case e_CONST_BUFFER_ARRAY: {
        return "CONST_BUFFER_ARRAY";
    } break;
    case e_CONST_BUFFER_PTR_ARRAY: {
        return "CONST_BUFFER_PTR_ARRAY";
    } break;
    case e_MUTABLE_BUFFER: {
        return "MUTABLE_BUFFER";
    } break;
    case e_MUTABLE_BUFFER_ARRAY: {
        return "MUTABLE_BUFFER_ARRAY";
    } break;
    case e_MUTABLE_BUFFER_PTR_ARRAY: {
        return "MUTABLE_BUFFER_PTR_ARRAY";
    } break;
    case e_BLOB: {
        return "BLOB";
    } break;
    case e_SHARED_BLOB: {
        return "SHARED_BLOB";
    } break;
    case e_STRING: {
        return "STRING";
    } break;
    case e_FILE: {
        return "FILE";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& DataType::print(bsl::ostream& stream, DataType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, DataType::Value rhs)
{
    return DataType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
