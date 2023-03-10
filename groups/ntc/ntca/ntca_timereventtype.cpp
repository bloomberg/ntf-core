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

#include <ntca_timereventtype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_timereventtype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int TimerEventType::fromInt(TimerEventType::Value* result, int number)
{
    switch (number) {
    case TimerEventType::e_DEADLINE:
    case TimerEventType::e_CANCELED:
    case TimerEventType::e_CLOSED:
        *result = static_cast<TimerEventType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int TimerEventType::fromString(TimerEventType::Value*   result,
                               const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "DEADLINE")) {
        *result = e_DEADLINE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CANCELED")) {
        *result = e_CANCELED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CLOSED")) {
        *result = e_CLOSED;
        return 0;
    }

    return -1;
}

const char* TimerEventType::toString(TimerEventType::Value value)
{
    switch (value) {
    case e_DEADLINE: {
        return "DEADLINE";
    } break;
    case e_CANCELED: {
        return "CANCELED";
    } break;
    case e_CLOSED: {
        return "CLOSED";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& TimerEventType::print(bsl::ostream&         stream,
                                    TimerEventType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, TimerEventType::Value rhs)
{
    return TimerEventType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
