// Copyright 2023 Bloomberg Finance L.P.
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

#include <ntsa_timestamptype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_timestamptype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>

namespace BloombergLP {
namespace ntsa {

int TimestampType::fromInt(TimestampType::Value* result, int number)
{
    switch (number) {
    case TimestampType::e_UNDEFINED:
    case TimestampType::e_SCHEDULED:
    case TimestampType::e_SENT:
    case TimestampType::e_ACKNOWLEDGED:
        *result = static_cast<TimestampType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int TimestampType::fromString(TimestampType::Value*    result,
                              const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SCHEDULED")) {
        *result = e_SCHEDULED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SENT")) {
        *result = e_SENT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ACKNOWLEDGED")) {
        *result = e_ACKNOWLEDGED;
        return 0;
    }

    return -1;
}

const char* TimestampType::toString(TimestampType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_SCHEDULED: {
        return "SCHEDULED";
    } break;
    case e_SENT: {
        return "SENT";
    } break;
    case e_ACKNOWLEDGED: {
        return "ACKNOWLEDGED";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& TimestampType::print(bsl::ostream&        stream,
                                   TimestampType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, TimestampType::Value rhs)
{
    return TimestampType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
