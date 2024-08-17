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

#include <ntsa_ethernetprotocol.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ethernetprotocol_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>

namespace BloombergLP {
namespace ntsa {

int EthernetProtocol::fromInt(EthernetProtocol::Value* result, int number)
{
    switch (number) {
    case EthernetProtocol::e_UNDEFINED:
    case EthernetProtocol::e_IPV4:
    case EthernetProtocol::e_ARP:
    case EthernetProtocol::e_RARP:
    case EthernetProtocol::e_IPV6:
        *result = static_cast<EthernetProtocol::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int EthernetProtocol::fromString(EthernetProtocol::Value*    result,
                                 const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "IPV4")) {
        *result = e_IPV4;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ARP")) {
        *result = e_ARP;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "RARP")) {
        *result = e_RARP;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "IPV6")) {
        *result = e_IPV6;
        return 0;
    }

    return -1;
}

const char* EthernetProtocol::toString(EthernetProtocol::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_IPV4: {
        return "IPV4";
    } break;
    case e_ARP: {
        return "ARP";
    } break;
    case e_RARP: {
        return "RARP";
    } break;
    case e_IPV6: {
        return "IPV6";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& EthernetProtocol::print(bsl::ostream&        stream,
                                      EthernetProtocol::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, EthernetProtocol::Value rhs)
{
    return EthernetProtocol::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
