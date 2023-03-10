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

#include <ntca_reactoreventtype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_reactoreventtype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int ReactorEventType::fromInt(ReactorEventType::Value* result, int number)
{
    switch (number) {
    case ReactorEventType::e_NONE:
    case ReactorEventType::e_READABLE:
    case ReactorEventType::e_WRITABLE:
    case ReactorEventType::e_ERROR:
        *result = static_cast<ReactorEventType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int ReactorEventType::fromString(ReactorEventType::Value* result,
                                 const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "NONE")) {
        *result = e_NONE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "READABLE")) {
        *result = e_READABLE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "WRITABLE")) {
        *result = e_WRITABLE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ERROR")) {
        *result = e_ERROR;
        return 0;
    }

    return -1;
}

const char* ReactorEventType::toString(ReactorEventType::Value value)
{
    switch (value) {
    case e_NONE: {
        return "NONE";
    } break;
    case e_READABLE: {
        return "READABLE";
    } break;
    case e_WRITABLE: {
        return "WRITABLE";
    } break;
    case e_ERROR: {
        return "ERROR";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& ReactorEventType::print(bsl::ostream&           stream,
                                      ReactorEventType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, ReactorEventType::Value rhs)
{
    return ReactorEventType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
