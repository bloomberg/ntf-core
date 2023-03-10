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

#include <ntca_flowcontrolmode.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_flowcontrolmode_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int FlowControlMode::fromInt(FlowControlMode::Value* result, int number)
{
    switch (number) {
    case FlowControlMode::e_IMMEDIATE:
    case FlowControlMode::e_GRACEFUL:
        *result = static_cast<FlowControlMode::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int FlowControlMode::fromString(FlowControlMode::Value*  result,
                                const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "IMMEDIATE")) {
        *result = e_IMMEDIATE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "GRACEFUL")) {
        *result = e_GRACEFUL;
        return 0;
    }

    return -1;
}

const char* FlowControlMode::toString(FlowControlMode::Value value)
{
    switch (value) {
    case e_IMMEDIATE: {
        return "IMMEDIATE";
    } break;
    case e_GRACEFUL: {
        return "GRACEFUL";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& FlowControlMode::print(bsl::ostream&          stream,
                                     FlowControlMode::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, FlowControlMode::Value rhs)
{
    return FlowControlMode::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
