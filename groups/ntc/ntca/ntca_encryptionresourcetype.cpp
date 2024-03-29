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

#include <ntca_encryptionresourcetype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptionresourcetype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int EncryptionResourceType::fromInt(EncryptionResourceType::Value* result,
                                    int                            number)
{
    switch (number) {
    case EncryptionResourceType::e_ASN1:
    case EncryptionResourceType::e_ASN1_PEM:
    case EncryptionResourceType::e_PKCS8:
    case EncryptionResourceType::e_PKCS8_PEM:
    case EncryptionResourceType::e_PKCS12:
        *result = static_cast<EncryptionResourceType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int EncryptionResourceType::fromString(EncryptionResourceType::Value* result,
                                       const bslstl::StringRef&       string)
{
    if (bdlb::String::areEqualCaseless(string, "ASN1")) {
        *result = e_ASN1;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ASN1_PEM")) {
        *result = e_ASN1_PEM;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "PKCS8")) {
        *result = e_PKCS8;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "PKCS8_PEM")) {
        *result = e_PKCS8_PEM;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "PKCS12")) {
        *result = e_PKCS12;
        return 0;
    }

    return -1;
}

const char* EncryptionResourceType::toString(
    EncryptionResourceType::Value value)
{
    switch (value) {
    case e_ASN1: {
        return "ASN1";
    } break;
    case e_ASN1_PEM: {
        return "ASN1_PEM";
    } break;
    case e_PKCS8: {
        return "PKCS8";
    } break;
    case e_PKCS8_PEM: {
        return "PKCS8_PEM";
    } break;
    case e_PKCS12: {
        return "PKCS12";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& EncryptionResourceType::print(
    bsl::ostream&                 stream,
    EncryptionResourceType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream&                 stream,
                         EncryptionResourceType::Value rhs)
{
    return EncryptionResourceType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
