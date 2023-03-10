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

#include <ntsa_ipaddresstype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipaddresstype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>

namespace BloombergLP {
namespace ntsa {

int IpAddressType::fromInt(IpAddressType::Value* result, int number)
{
    switch (number) {
    case IpAddressType::e_UNDEFINED:
    case IpAddressType::e_V4:
    case IpAddressType::e_V6:
        *result = static_cast<IpAddressType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int IpAddressType::fromString(IpAddressType::Value*    result,
                              const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "V4")) {
        *result = e_V4;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "V6")) {
        *result = e_V6;
        return 0;
    }

    return -1;
}

const char* IpAddressType::toString(IpAddressType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_V4: {
        return "V4";
    } break;
    case e_V6: {
        return "V6";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& IpAddressType::print(bsl::ostream&        stream,
                                   IpAddressType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, IpAddressType::Value rhs)
{
    return IpAddressType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
