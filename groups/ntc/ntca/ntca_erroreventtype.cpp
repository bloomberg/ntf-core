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

#include <ntca_erroreventtype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_erroreventtype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int ErrorEventType::fromInt(ErrorEventType::Value* result, int number)
{
    switch (number) {
    case ErrorEventType::e_UNKNOWN:
    case ErrorEventType::e_LISTEN:
    case ErrorEventType::e_ACCEPT:
    case ErrorEventType::e_CONNECT:
    case ErrorEventType::e_UPGRADE:
    case ErrorEventType::e_ENCRYPT:
    case ErrorEventType::e_TRANSPORT:
    case ErrorEventType::e_DECRYPT:
    case ErrorEventType::e_DOWNGRADE:
    case ErrorEventType::e_SHUTDOWN:
        *result = static_cast<ErrorEventType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int ErrorEventType::fromString(ErrorEventType::Value*   result,
                               const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNKNOWN")) {
        *result = e_UNKNOWN;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "LISTEN")) {
        *result = e_LISTEN;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ACCEPT")) {
        *result = e_ACCEPT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CONNECT")) {
        *result = e_CONNECT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "UPGRADE")) {
        *result = e_UPGRADE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ENCRYPT")) {
        *result = e_ENCRYPT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TRANSPORT")) {
        *result = e_TRANSPORT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "DECRYPT")) {
        *result = e_DECRYPT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "DOWNGRADE")) {
        *result = e_DOWNGRADE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SHUTDOWN")) {
        *result = e_SHUTDOWN;
        return 0;
    }

    return -1;
}

const char* ErrorEventType::toString(ErrorEventType::Value value)
{
    switch (value) {
    case e_UNKNOWN: {
        return "UNKNOWN";
    } break;
    case e_LISTEN: {
        return "LISTEN";
    } break;
    case e_ACCEPT: {
        return "ACCEPT";
    } break;
    case e_CONNECT: {
        return "CONNECT";
    } break;
    case e_UPGRADE: {
        return "UPGRADE";
    } break;
    case e_ENCRYPT: {
        return "ENCRYPT";
    } break;
    case e_TRANSPORT: {
        return "TRANSPORT";
    } break;
    case e_DECRYPT: {
        return "DECRYPT";
    } break;
    case e_DOWNGRADE: {
        return "DOWNGRADE";
    } break;
    case e_SHUTDOWN: {
        return "SHUTDOWN";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& ErrorEventType::print(bsl::ostream&         stream,
                                    ErrorEventType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, ErrorEventType::Value rhs)
{
    return ErrorEventType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
