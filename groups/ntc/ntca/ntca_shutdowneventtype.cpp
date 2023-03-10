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

#include <ntca_shutdowneventtype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_shutdowneventtype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int ShutdownEventType::fromInt(ShutdownEventType::Value* result, int number)
{
    switch (number) {
    case ShutdownEventType::e_INITIATED:
    case ShutdownEventType::e_SEND:
    case ShutdownEventType::e_RECEIVE:
    case ShutdownEventType::e_COMPLETE:
        *result = static_cast<ShutdownEventType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int ShutdownEventType::fromString(ShutdownEventType::Value* result,
                                  const bslstl::StringRef&  string)
{
    if (bdlb::String::areEqualCaseless(string, "INITIATED")) {
        *result = e_INITIATED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SEND")) {
        *result = e_SEND;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "RECEIVE")) {
        *result = e_RECEIVE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "COMPLETE")) {
        *result = e_COMPLETE;
        return 0;
    }

    return -1;
}

const char* ShutdownEventType::toString(ShutdownEventType::Value value)
{
    switch (value) {
    case e_INITIATED: {
        return "INITIATED";
    } break;
    case e_SEND: {
        return "SEND";
    } break;
    case e_RECEIVE: {
        return "RECEIVE";
    } break;
    case e_COMPLETE: {
        return "COMPLETE";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& ShutdownEventType::print(bsl::ostream&            stream,
                                       ShutdownEventType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, ShutdownEventType::Value rhs)
{
    return ShutdownEventType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
