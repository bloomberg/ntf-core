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

#include <ntsa_shutdowntype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_shutdowntype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

int ShutdownType::fromInt(ShutdownType::Value* result, int number)
{
    switch (number) {
    case ShutdownType::e_SEND:
    case ShutdownType::e_RECEIVE:
    case ShutdownType::e_BOTH:
        *result = static_cast<ShutdownType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int ShutdownType::fromString(ShutdownType::Value*     result,
                             const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "SEND")) {
        *result = e_SEND;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "RECEIVE")) {
        *result = e_RECEIVE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "BOTH")) {
        *result = e_BOTH;
        return 0;
    }

    return -1;
}

const char* ShutdownType::toString(ShutdownType::Value value)
{
    switch (value) {
    case e_SEND: {
        return "SEND";
    } break;
    case e_RECEIVE: {
        return "RECEIVE";
    } break;
    case e_BOTH: {
        return "BOTH";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& ShutdownType::print(bsl::ostream&       stream,
                                  ShutdownType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, ShutdownType::Value rhs)
{
    return ShutdownType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
