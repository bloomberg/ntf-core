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

#include <ntsa_shutdownmode.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_shutdownmode_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

int ShutdownMode::fromInt(ShutdownMode::Value* result, int number)
{
    switch (number) {
    case ShutdownMode::e_IMMEDIATE:
    case ShutdownMode::e_GRACEFUL:
        *result = static_cast<ShutdownMode::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int ShutdownMode::fromString(ShutdownMode::Value*     result,
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

const char* ShutdownMode::toString(ShutdownMode::Value value)
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

bsl::ostream& ShutdownMode::print(bsl::ostream&       stream,
                                  ShutdownMode::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, ShutdownMode::Value rhs)
{
    return ShutdownMode::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
