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

#include <ntsa_shutdownorigin.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_shutdownorigin_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

int ShutdownOrigin::fromInt(ShutdownOrigin::Value* result, int number)
{
    switch (number) {
    case ShutdownOrigin::e_SOURCE:
    case ShutdownOrigin::e_REMOTE:
        *result = static_cast<ShutdownOrigin::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int ShutdownOrigin::fromString(ShutdownOrigin::Value*   result,
                               const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "SOURCE")) {
        *result = e_SOURCE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "REMOTE")) {
        *result = e_REMOTE;
        return 0;
    }

    return -1;
}

const char* ShutdownOrigin::toString(ShutdownOrigin::Value value)
{
    switch (value) {
    case e_SOURCE: {
        return "SOURCE";
    } break;
    case e_REMOTE: {
        return "REMOTE";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& ShutdownOrigin::print(bsl::ostream&         stream,
                                    ShutdownOrigin::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, ShutdownOrigin::Value rhs)
{
    return ShutdownOrigin::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
