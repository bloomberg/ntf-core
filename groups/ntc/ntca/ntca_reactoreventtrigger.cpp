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

#include <ntca_reactoreventtrigger.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_reactoreventtrigger_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int ReactorEventTrigger::fromInt(ReactorEventTrigger::Value* result,
                                 int                         number)
{
    switch (number) {
    case ReactorEventTrigger::e_LEVEL:
    case ReactorEventTrigger::e_EDGE:
        *result = static_cast<ReactorEventTrigger::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int ReactorEventTrigger::fromString(ReactorEventTrigger::Value* result,
                                    const bslstl::StringRef&    string)
{
    if (bdlb::String::areEqualCaseless(string, "LEVEL")) {
        *result = e_LEVEL;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "EDGE")) {
        *result = e_EDGE;
        return 0;
    }

    return -1;
}

const char* ReactorEventTrigger::toString(ReactorEventTrigger::Value value)
{
    switch (value) {
    case e_LEVEL: {
        return "LEVEL";
    } break;
    case e_EDGE: {
        return "EDGE";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& ReactorEventTrigger::print(bsl::ostream&              stream,
                                         ReactorEventTrigger::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, ReactorEventTrigger::Value rhs)
{
    return ReactorEventTrigger::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
