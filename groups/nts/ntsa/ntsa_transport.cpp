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

#include <ntsa_transport.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_protocol_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>

namespace BloombergLP {
namespace ntsa {

int TransportProtocol::fromInt(TransportProtocol::Value* result, int number)
{
    switch (number) {
    case TransportProtocol::e_UNDEFINED:
    case TransportProtocol::e_TCP:
    case TransportProtocol::e_UDP:
    case TransportProtocol::e_LOCAL:
        *result = static_cast<TransportProtocol::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int TransportProtocol::fromString(TransportProtocol::Value* result,
                                  const bslstl::StringRef&  string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TCP")) {
        *result = e_TCP;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "UDP")) {
        *result = e_UDP;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "LOCAL")) {
        *result = e_LOCAL;
        return 0;
    }

    return -1;
}

const char* TransportProtocol::toString(TransportProtocol::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_TCP: {
        return "TCP";
    } break;
    case e_UDP: {
        return "UDP";
    } break;
    case e_LOCAL: {
        return "LOCAL";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& TransportProtocol::print(bsl::ostream&            stream,
                                       TransportProtocol::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, TransportProtocol::Value rhs)
{
    return TransportProtocol::print(stream, rhs);
}

int TransportDomain::fromInt(TransportDomain::Value* result, int number)
{
    switch (number) {
    case TransportDomain::e_UNDEFINED:
    case TransportDomain::e_IPV4:
    case TransportDomain::e_IPV6:
    case TransportDomain::e_LOCAL:
        *result = static_cast<TransportDomain::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int TransportDomain::fromString(TransportDomain::Value*  result,
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
    if (bdlb::String::areEqualCaseless(string, "IPV6")) {
        *result = e_IPV6;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "LOCAL")) {
        *result = e_LOCAL;
        return 0;
    }

    return -1;
}

const char* TransportDomain::toString(TransportDomain::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_IPV4: {
        return "IPV4";
    } break;
    case e_IPV6: {
        return "IPV6";
    } break;
    case e_LOCAL: {
        return "LOCAL";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& TransportDomain::print(bsl::ostream&          stream,
                                     TransportDomain::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, TransportDomain::Value rhs)
{
    return TransportDomain::print(stream, rhs);
}

int TransportMode::fromInt(TransportMode::Value* result, int number)
{
    switch (number) {
    case TransportMode::e_UNDEFINED:
    case TransportMode::e_STREAM:
    case TransportMode::e_DATAGRAM:
        *result = static_cast<TransportMode::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int TransportMode::fromString(TransportMode::Value*    result,
                              const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "STREAM")) {
        *result = e_STREAM;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "DATAGRAM")) {
        *result = e_DATAGRAM;
        return 0;
    }

    return -1;
}

const char* TransportMode::toString(TransportMode::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_STREAM: {
        return "STREAM";
    } break;
    case e_DATAGRAM: {
        return "DATAGRAM";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& TransportMode::print(bsl::ostream&        stream,
                                   TransportMode::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, TransportMode::Value rhs)
{
    return TransportMode::print(stream, rhs);
}

int Transport::fromInt(Transport::Value* result, int number)
{
    switch (number) {
    case Transport::e_UNDEFINED:
    case Transport::e_TCP_IPV4_STREAM:
    case Transport::e_TCP_IPV6_STREAM:
    case Transport::e_UDP_IPV4_DATAGRAM:
    case Transport::e_UDP_IPV6_DATAGRAM:
    case Transport::e_LOCAL_STREAM:
    case Transport::e_LOCAL_DATAGRAM:
        *result = static_cast<Transport::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int Transport::fromString(Transport::Value*        result,
                          const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TCP_IPV4_STREAM")) {
        *result = e_TCP_IPV4_STREAM;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TCP_IPV6_STREAM")) {
        *result = e_TCP_IPV6_STREAM;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "UDP_IPV4_DATAGRAM")) {
        *result = e_UDP_IPV4_DATAGRAM;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "UDP_IPV6_DATAGRAM")) {
        *result = e_UDP_IPV6_DATAGRAM;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "LOCAL_STREAM")) {
        *result = e_LOCAL_STREAM;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "LOCAL_DATAGRAM")) {
        *result = e_LOCAL_DATAGRAM;
        return 0;
    }

    return -1;
}

int Transport::fromString(Value* result, const char* string, int stringLength)
{
    return Transport::fromString(
        result,
        bslstl::StringRef(string, static_cast<bsl::size_t>(stringLength)));
}

const char* Transport::toString(Transport::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_TCP_IPV4_STREAM: {
        return "TCP_IPV4_STREAM";
    } break;
    case e_TCP_IPV6_STREAM: {
        return "TCP_IPV6_STREAM";
    } break;
    case e_UDP_IPV4_DATAGRAM: {
        return "UDP_IPV4_DATAGRAM";
    } break;
    case e_UDP_IPV6_DATAGRAM: {
        return "UDP_IPV6_DATAGRAM";
    } break;
    case e_LOCAL_STREAM: {
        return "LOCAL_STREAM";
    } break;
    case e_LOCAL_DATAGRAM: {
        return "LOCAL_DATAGRAM";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

ntsa::TransportMode::Value Transport::getMode(Value value)
{
    if (value == ntsa::Transport::e_TCP_IPV4_STREAM ||
        value == ntsa::Transport::e_TCP_IPV6_STREAM ||
        value == ntsa::Transport::e_LOCAL_STREAM)
    {
        return ntsa::TransportMode::e_STREAM;
    }

    if (value == ntsa::Transport::e_UDP_IPV4_DATAGRAM ||
        value == ntsa::Transport::e_UDP_IPV6_DATAGRAM ||
        value == ntsa::Transport::e_LOCAL_DATAGRAM)
    {
        return ntsa::TransportMode::e_DATAGRAM;
    }

    return ntsa::TransportMode::e_UNDEFINED;
}

ntsa::TransportDomain::Value Transport::getDomain(Value value)
{
    if (value == ntsa::Transport::e_TCP_IPV4_STREAM ||
        value == ntsa::Transport::e_UDP_IPV4_DATAGRAM)
    {
        return ntsa::TransportDomain::e_IPV4;
    }

    if (value == ntsa::Transport::e_TCP_IPV6_STREAM ||
        value == ntsa::Transport::e_UDP_IPV6_DATAGRAM)
    {
        return ntsa::TransportDomain::e_IPV6;
    }

    if (value == ntsa::Transport::e_LOCAL_STREAM ||
        value == ntsa::Transport::e_LOCAL_DATAGRAM)
    {
        return ntsa::TransportDomain::e_LOCAL;
    }

    return ntsa::TransportDomain::e_UNDEFINED;
}

ntsa::TransportProtocol::Value Transport::getProtocol(Value value)
{
    if (value == ntsa::Transport::e_TCP_IPV4_STREAM ||
        value == ntsa::Transport::e_TCP_IPV6_STREAM)
    {
        return ntsa::TransportProtocol::e_TCP;
    }

    if (value == ntsa::Transport::e_UDP_IPV4_DATAGRAM ||
        value == ntsa::Transport::e_UDP_IPV6_DATAGRAM)
    {
        return ntsa::TransportProtocol::e_UDP;
    }

    if (value == ntsa::Transport::e_LOCAL_STREAM ||
        value == ntsa::Transport::e_LOCAL_DATAGRAM)
    {
        return ntsa::TransportProtocol::e_LOCAL;
    }

    return ntsa::TransportProtocol::e_UNDEFINED;
}

bsl::ostream& Transport::print(bsl::ostream& stream, Transport::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, Transport::Value rhs)
{
    return Transport::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
