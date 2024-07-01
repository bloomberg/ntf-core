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

#include <ntca_encryptionauthentication.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptionauthentication_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int EncryptionAuthentication::fromInt(EncryptionAuthentication::Value* result,
                                      int                              number)
{
    switch (number) {
    case EncryptionAuthentication::e_DEFAULT:
    case EncryptionAuthentication::e_NONE:
    case EncryptionAuthentication::e_VERIFY:
        *result = static_cast<EncryptionAuthentication::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int EncryptionAuthentication::fromString(
    EncryptionAuthentication::Value* result,
    const bslstl::StringRef&         string)
{
    if (bdlb::String::areEqualCaseless(string, "DEFAULT")) {
        *result = e_DEFAULT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "NONE")) {
        *result = e_NONE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "VERIFY")) {
        *result = e_VERIFY;
        return 0;
    }

    return -1;
}

const char* EncryptionAuthentication::toString(
    EncryptionAuthentication::Value value)
{
    switch (value) {
    case e_DEFAULT: {
        return "DEFAULT";
    } break;
    case e_NONE: {
        return "NONE";
    } break;
    case e_VERIFY: {
        return "VERIFY";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& EncryptionAuthentication::print(
    bsl::ostream&                   stream,
    EncryptionAuthentication::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream&                   stream,
                         EncryptionAuthentication::Value rhs)
{
    return EncryptionAuthentication::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
