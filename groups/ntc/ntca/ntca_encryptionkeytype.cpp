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

#include <ntca_encryptionkeytype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptionkeytype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int EncryptionKeyType::fromInt(EncryptionKeyType::Value* result, int number)
{
    switch (number) {
    case EncryptionKeyType::e_DSA:
    case EncryptionKeyType::e_RSA:
    case EncryptionKeyType::e_NIST_P256:
    case EncryptionKeyType::e_NIST_P384:
    case EncryptionKeyType::e_NIST_P521:
    case EncryptionKeyType::e_ED25519:
    case EncryptionKeyType::e_ED448:
        *result = static_cast<EncryptionKeyType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int EncryptionKeyType::fromString(EncryptionKeyType::Value* result,
                                  const bslstl::StringRef&  string)
{
    if (bdlb::String::areEqualCaseless(string, "DSA")) {
        *result = e_DSA;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "RSA")) {
        *result = e_RSA;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "NIST_P256")) {
        *result = e_NIST_P256;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "NIST_P384")) {
        *result = e_NIST_P384;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "NIST_P521")) {
        *result = e_NIST_P521;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ED25519")) {
        *result = e_ED25519;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ED448")) {
        *result = e_ED448;
        return 0;
    }

    return -1;
}

int EncryptionKeyType::fromObjectIdentifier(
        Value*                                result, 
        const ntsa::AbstractObjectIdentifier& identifier)
{
    // MRM
    #if 0
    if (identifier.equals(0)) {
        *result = e_DSA;
        return 0;
    }

    if (identifier.equals(0)) {
        *result = e_RSA;
        return 0;
    }
    #endif

    if (identifier.equals(1, 2, 840, 10045, 3, 1, 7)) {
        *result = e_NIST_P256;
        return 0;
    }

    if (identifier.equals(1, 3, 132, 0, 34)) {
        *result = e_NIST_P384;
        return 0;
    }

    if (identifier.equals(1, 3, 132, 0, 35)) {
        *result = e_NIST_P521;
        return 0;
    }

    // MRM
    #if 0
    if (identifier.equals(0)) {
        *result = e_ED25519;
        return 0;
    }

    if (identifier.equals(0)) {
        *result = e_ED448;
        return 0;
    }
    #endif

    return -1;
}

const char* EncryptionKeyType::toString(EncryptionKeyType::Value value)
{
    switch (value) {
    case e_DSA: {
        return "DSA";
    } break;
    case e_RSA: {
        return "RSA";
    } break;
    case e_NIST_P256: {
        return "NIST_P256";
    } break;
    case e_NIST_P384: {
        return "NIST_P384";
    } break;
    case e_NIST_P521: {
        return "NIST_P256";
    } break;
    case e_ED25519: {
        return "ED25519";
    } break;
    case e_ED448: {
        return "ED448";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

void EncryptionKeyType::toObjectIdentifier(
    ntsa::AbstractObjectIdentifier* result, 
    Value                           value)
{
    result->reset();

    // MRM
#if 0
    if (value == e_DSA) {
        result->set(0);
    }
    else if (value == e_RSA) {
        result->set(0);
    }
#endif
    
    if (value == e_NIST_P256) {
        result->set(1, 2, 840, 10045, 3, 1, 7);
    }
    else if (value == e_NIST_P384) {
        result->set(1, 3, 132, 0, 34);
    }
    else if (value == e_NIST_P521) {
        result->set(1, 3, 132, 0, 35);
    }

    // MRM
#if 0
    else if (value == e_ED25519) {
        result->set(0);
    }
    else if (value == e_ED448) {
        result->set(0);
    }
#endif
}

bsl::ostream& EncryptionKeyType::print(bsl::ostream&            stream,
                                       EncryptionKeyType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, EncryptionKeyType::Value rhs)
{
    return EncryptionKeyType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
