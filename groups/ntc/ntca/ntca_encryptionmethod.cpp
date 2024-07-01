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

#include <ntca_encryptionmethod.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptionmethod_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int EncryptionMethod::fromInt(EncryptionMethod::Value* result, int number)
{
    switch (number) {
    case EncryptionMethod::e_DEFAULT:
    case EncryptionMethod::e_TLS_V1_X:
    case EncryptionMethod::e_TLS_V1_0:
    case EncryptionMethod::e_TLS_V1_1:
    case EncryptionMethod::e_TLS_V1_2:
    case EncryptionMethod::e_TLS_V1_3:
        *result = static_cast<EncryptionMethod::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int EncryptionMethod::fromString(EncryptionMethod::Value* result,
                                 const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "DEFAULT")) {
        *result = e_DEFAULT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TLS_V1_X")) {
        *result = e_TLS_V1_X;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TLS_V1_0")) {
        *result = e_TLS_V1_0;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TLS_V1_1")) {
        *result = e_TLS_V1_1;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TLS_V1_2")) {
        *result = e_TLS_V1_2;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TLS_V1_3")) {
        *result = e_TLS_V1_3;
        return 0;
    }

    return -1;
}

const char* EncryptionMethod::toString(EncryptionMethod::Value value)
{
    switch (value) {
    case e_DEFAULT: {
        return "DEFAULT";
    } break;
    case e_TLS_V1_X: {
        return "TLS_V1_X";
    } break;
    case e_TLS_V1_0: {
        return "TLS_V1_0";
    } break;
    case e_TLS_V1_1: {
        return "TLS_V1_1";
    } break;
    case e_TLS_V1_2: {
        return "TLS_V1_2";
    } break;
    case e_TLS_V1_3: {
        return "TLS_V1_3";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& EncryptionMethod::print(bsl::ostream&           stream,
                                      EncryptionMethod::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, EncryptionMethod::Value rhs)
{
    return EncryptionMethod::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
