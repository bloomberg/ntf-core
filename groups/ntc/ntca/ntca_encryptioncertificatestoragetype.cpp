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

#include <ntca_encryptioncertificatestoragetype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptioncertificatestoragetype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int EncryptionCertificateStorageType::fromInt(EncryptionCertificateStorageType::Value* result, int number)
{
    switch (number) {
    case EncryptionCertificateStorageType::e_PEM:
    case EncryptionCertificateStorageType::e_DER:
    case EncryptionCertificateStorageType::e_PKCS12:
        *result = static_cast<EncryptionCertificateStorageType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int EncryptionCertificateStorageType::fromString(EncryptionCertificateStorageType::Value*  result,
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

    return -1;
}

const char* EncryptionCertificateStorageType::toString(EncryptionCertificateStorageType::Value value)
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
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& EncryptionCertificateStorageType::print(bsl::ostream&          stream,
                                     EncryptionCertificateStorageType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, EncryptionCertificateStorageType::Value rhs)
{
    return EncryptionCertificateStorageType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
