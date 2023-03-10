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

#include <ntca_resolversource.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_resolversource_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int ResolverSource::fromInt(ResolverSource::Value* result, int number)
{
    switch (number) {
    case ResolverSource::e_UNKNOWN:
    case ResolverSource::e_OVERRIDE:
    case ResolverSource::e_DATABASE:
    case ResolverSource::e_CACHE:
    case ResolverSource::e_SERVER:
    case ResolverSource::e_SYSTEM:
        *result = static_cast<ResolverSource::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int ResolverSource::fromString(ResolverSource::Value*   result,
                               const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNKNOWN")) {
        *result = e_UNKNOWN;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "OVERRIDE")) {
        *result = e_OVERRIDE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "DATABASE")) {
        *result = e_DATABASE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CACHE")) {
        *result = e_CACHE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SERVER")) {
        *result = e_SERVER;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SYSTEM")) {
        *result = e_SYSTEM;
        return 0;
    }

    return -1;
}

const char* ResolverSource::toString(ResolverSource::Value value)
{
    switch (value) {
    case e_UNKNOWN: {
        return "UNKNOWN";
    } break;
    case e_OVERRIDE: {
        return "OVERRIDE";
    } break;
    case e_DATABASE: {
        return "DATABASE";
    } break;
    case e_CACHE: {
        return "CACHE";
    } break;
    case e_SERVER: {
        return "SERVER";
    } break;
    case e_SYSTEM: {
        return "SYSTEM";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& ResolverSource::print(bsl::ostream&         stream,
                                    ResolverSource::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, ResolverSource::Value rhs)
{
    return ResolverSource::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
