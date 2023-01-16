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

#include <ntsa_notificationtype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_notificationtype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>

namespace BloombergLP {
namespace ntsa {

int NotificationType::fromInt(NotificationType::Value* result, int number)
{
    switch (number) {
    case NotificationType::e_UNDEFINED:
    case NotificationType::e_TIMESTAMP:
        *result = static_cast<NotificationType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int NotificationType::fromString(NotificationType::Value* result,
                                 const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TIMESTAMP")) {
        *result = e_TIMESTAMP;
        return 0;
    }

    return -1;
}

const char* NotificationType::toString(NotificationType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_TIMESTAMP: {
        return "TIMESTAMP";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& NotificationType::print(bsl::ostream&           stream,
                                      NotificationType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, NotificationType::Value rhs)
{
    return NotificationType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
