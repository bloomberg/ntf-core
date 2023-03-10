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

#include <ntsd_messagetype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsd_protocol_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>

namespace BloombergLP {
namespace ntsd {

int MessageType::fromInt(MessageType::Value* result, int number)
{
    switch (number) {
    case MessageType::e_UNDEFINED:
    case MessageType::e_REQUEST:
    case MessageType::e_RESPONSE:
    case MessageType::e_ONE_WAY:
        *result = static_cast<MessageType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int MessageType::fromString(MessageType::Value*      result,
                            const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "REQUEST")) {
        *result = e_REQUEST;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "RESPONSE")) {
        *result = e_RESPONSE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ONE_WAY")) {
        *result = e_ONE_WAY;
        return 0;
    }

    return -1;
}

const char* MessageType::toString(MessageType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_REQUEST: {
        return "REQUEST";
    } break;
    case e_RESPONSE: {
        return "RESPONSE";
    } break;
    case e_ONE_WAY: {
        return "ONE_WAY";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& MessageType::print(bsl::ostream&      stream,
                                 MessageType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, MessageType::Value rhs)
{
    return MessageType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
