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

#include <ntca_connectstrategy.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_connectstrategy_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int ConnectStrategy::fromInt(ConnectStrategy::Value* result, int number)
{
    switch (number) {
    case ConnectStrategy::e_RESOLVE_INTO_SINGLE:
    case ConnectStrategy::e_RESOLVE_INTO_LIST:
        *result = static_cast<ConnectStrategy::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int ConnectStrategy::fromString(ConnectStrategy::Value*  result,
                                const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "RESOLVE_INTO_SINGLE")) {
        *result = e_RESOLVE_INTO_SINGLE;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "RESOLVE_INTO_LIST")) {
        *result = e_RESOLVE_INTO_LIST;
        return 0;
    }

    return -1;
}

const char* ConnectStrategy::toString(ConnectStrategy::Value value)
{
    switch (value) {
    case e_RESOLVE_INTO_SINGLE: {
        return "RESOLVE_INTO_SINGLE";
    } break;
    case e_RESOLVE_INTO_LIST: {
        return "RESOLVE_INTO_LIST";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& ConnectStrategy::print(bsl::ostream&          stream,
                                     ConnectStrategy::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, ConnectStrategy::Value rhs)
{
    return ConnectStrategy::print(stream, rhs);
}

}  // close namespace ntca
}  // close namespace BloombergLP
