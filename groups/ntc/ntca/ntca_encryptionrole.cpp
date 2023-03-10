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

#include <ntca_encryptionrole.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptionrole_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int EncryptionRole::fromInt(EncryptionRole::Value* result, int number)
{
    switch (number) {
    case EncryptionRole::e_CLIENT:
    case EncryptionRole::e_SERVER:
        *result = static_cast<EncryptionRole::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int EncryptionRole::fromString(EncryptionRole::Value*   result,
                               const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "CLIENT")) {
        *result = e_CLIENT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SERVER")) {
        *result = e_SERVER;
        return 0;
    }

    return -1;
}

const char* EncryptionRole::toString(EncryptionRole::Value value)
{
    switch (value) {
    case e_CLIENT: {
        return "CLIENT";
    } break;
    case e_SERVER: {
        return "SERVER";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& EncryptionRole::print(bsl::ostream&         stream,
                                    EncryptionRole::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, EncryptionRole::Value rhs)
{
    return EncryptionRole::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
