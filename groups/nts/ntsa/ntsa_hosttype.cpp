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

#include <ntsa_hosttype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_hosttype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>

namespace BloombergLP {
namespace ntsa {

int HostType::fromInt(HostType::Value* result, int number)
{
    switch (number) {
    case HostType::e_UNDEFINED:
    case HostType::e_DOMAIN_NAME:
    case HostType::e_IP:
    case HostType::e_LOCAL_NAME:
        *result = static_cast<HostType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int HostType::fromString(HostType::Value*         result,
                         const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "DOMAIN_NAME")) {
        *result = e_DOMAIN_NAME;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "IP")) {
        *result = e_IP;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "LOCAL_NAME")) {
        *result = e_LOCAL_NAME;
        return 0;
    }

    return -1;
}

const char* HostType::toString(HostType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_DOMAIN_NAME: {
        return "DOMAIN_NAME";
    } break;
    case e_IP: {
        return "IP";
    } break;
    case e_LOCAL_NAME: {
        return "LOCAL_NAME";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& HostType::print(bsl::ostream& stream, HostType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, HostType::Value rhs)
{
    return HostType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
