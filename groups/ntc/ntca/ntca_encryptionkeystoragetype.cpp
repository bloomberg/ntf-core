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

#include <ntca_encryptionkeystoragetype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptionkeystoragetype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int EncryptionKeyStorageType::fromInt(EncryptionKeyStorageType::Value* result, int number)
{
    switch (number) {
    case EncryptionKeyStorageType::e_PEM:
    case EncryptionKeyStorageType::e_DER:
    case EncryptionKeyStorageType::e_PKCS12:
    case EncryptionKeyStorageType::e_PKCS8:
        *result = static_cast<EncryptionKeyStorageType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int EncryptionKeyStorageType::fromString(EncryptionKeyStorageType::Value*  result,
                                const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "PEM")) {
        *result = e_PEM;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "DER")) {
        *result = e_DER;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "PKCS12")) {
        *result = e_PKCS12;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "PKCS8")) {
        *result = e_PKCS8;
        return 0;
    }

    return -1;
}

const char* EncryptionKeyStorageType::toString(EncryptionKeyStorageType::Value value)
{
    switch (value) {
    case e_PEM: {
        return "PEM";
    } break;
    case e_DER: {
        return "DER";
    } break;
    case e_PKCS12: {
        return "PKCS12";
    } break;
    case e_PKCS8: {
        return "PKCS8";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& EncryptionKeyStorageType::print(bsl::ostream&          stream,
                                     EncryptionKeyStorageType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, EncryptionKeyStorageType::Value rhs)
{
    return EncryptionKeyStorageType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
