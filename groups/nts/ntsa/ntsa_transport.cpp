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
#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

int TransportSecurity::fromInt(TransportSecurity::Value* result, int number)
{
    switch (number) {
    case TransportSecurity::e_UNDEFINED:
    case TransportSecurity::e_TLS:
    case TransportSecurity::e_SSH:
        *result = static_cast<TransportSecurity::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int TransportSecurity::fromString(TransportSecurity::Value* result,
                                  const bslstl::StringRef&  string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TLS")) {
        *result = e_TLS;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SSH")) {
        *result = e_SSH;
        return 0;
    }

    return -1;
}

const char* TransportSecurity::toString(TransportSecurity::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_TLS: {
        return "TLS";
    } break;
    case e_SSH: {
        return "SSH";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& TransportSecurity::print(bsl::ostream&            stream,
                                       TransportSecurity::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, TransportSecurity::Value rhs)
{
    return TransportSecurity::print(stream, rhs);
}

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

int TransportRole::fromInt(TransportRole::Value* result, int number)
{
    switch (number) {
    case TransportRole::e_UNDEFINED:
    case TransportRole::e_CLIENT:
    case TransportRole::e_SERVER:
        *result = static_cast<TransportRole::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int TransportRole::fromString(TransportRole::Value*    result,
                              const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CLIENT")) {
        *result = e_CLIENT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SERVER")) {
        *result = e_SERVER;
        return 0;
    }

    return -1;
}

const char* TransportRole::toString(TransportRole::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_CLIENT: {
        return "CLIENT";
    } break;
    case e_SERVER: {
        return "SERVER";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& TransportRole::print(bsl::ostream&        stream,
                                   TransportRole::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, TransportRole::Value rhs)
{
    return TransportRole::print(stream, rhs);
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

TransportSuite::TransportSuite(bslma::Allocator* basicAllocator)
: d_application(basicAllocator)
, d_transportSecurity(ntsa::TransportSecurity::e_UNDEFINED)
, d_transportProtocol(ntsa::TransportProtocol::e_UNDEFINED)
, d_transportDomain(ntsa::TransportDomain::e_UNDEFINED)
, d_transportMode(ntsa::TransportMode::e_UNDEFINED)
{
}

TransportSuite::TransportSuite(const TransportSuite& original,
                                 bslma::Allocator*      basicAllocator)
: d_application(original.d_application, basicAllocator)
, d_transportSecurity(original.d_transportSecurity)
, d_transportProtocol(original.d_transportProtocol)
, d_transportDomain(original.d_transportDomain)
, d_transportMode(original.d_transportMode)
{
}

TransportSuite::~TransportSuite()
{
}

TransportSuite& TransportSuite::operator=(const TransportSuite& other)
{
    if (this != &other) {
        d_application       = other.d_application;
        d_transportSecurity = other.d_transportSecurity;
        d_transportProtocol = other.d_transportProtocol;
        d_transportDomain   = other.d_transportDomain;
        d_transportMode     = other.d_transportMode;
    }

    return *this;
}

void TransportSuite::reset()
{
    d_application.clear();
    d_transportSecurity = ntsa::TransportSecurity::e_UNDEFINED;
    d_transportProtocol = ntsa::TransportProtocol::e_UNDEFINED;
    d_transportDomain   = ntsa::TransportDomain::e_UNDEFINED;
    d_transportMode     = ntsa::TransportMode::e_UNDEFINED;
}

void TransportSuite::setApplication(const bslstl::StringRef& value)
{
    d_application = value;
}

void TransportSuite::setTransportSecurity(
    ntsa::TransportSecurity::Value value)
{
    d_transportSecurity = value;
}

void TransportSuite::setTransportProtocol(
    ntsa::TransportProtocol::Value value)
{
    d_transportProtocol = value;
}

void TransportSuite::setTransportDomain(ntsa::TransportDomain::Value value)
{
    d_transportDomain = value;
}

void TransportSuite::setTransportMode(ntsa::TransportMode::Value value)
{
    d_transportMode = value;
}

const bsl::string& TransportSuite::application() const
{
    return d_application;
}

ntsa::TransportSecurity::Value TransportSuite::transportSecurity() const
{
    return d_transportSecurity;
}

ntsa::TransportProtocol::Value TransportSuite::transportProtocol() const
{
    return d_transportProtocol;
}

ntsa::TransportDomain::Value TransportSuite::transportDomain() const
{
    return d_transportDomain;
}

ntsa::TransportMode::Value TransportSuite::transportMode() const
{
    return d_transportMode;
}

ntsa::Transport::Value TransportSuite::transport() const
{
    if (d_transportProtocol == ntsa::TransportProtocol::e_TCP) {
        if (d_transportDomain == ntsa::TransportDomain::e_IPV4) {
            if (d_transportMode == ntsa::TransportMode::e_STREAM) {
                return ntsa::Transport::e_TCP_IPV4_STREAM;
            }
            else {
                return ntsa::Transport::e_UNDEFINED;
            }
        }
        else if (d_transportDomain == ntsa::TransportDomain::e_IPV6) {
            if (d_transportMode == ntsa::TransportMode::e_STREAM) {
                return ntsa::Transport::e_TCP_IPV6_STREAM;
            }
            else {
                return ntsa::Transport::e_UNDEFINED;
            }
        }
        else {
            return ntsa::Transport::e_UNDEFINED;
        }
    }
    else if (d_transportProtocol == ntsa::TransportProtocol::e_UDP) {
        if (d_transportDomain == ntsa::TransportDomain::e_IPV4) {
            if (d_transportMode == ntsa::TransportMode::e_DATAGRAM) {
                return ntsa::Transport::e_UDP_IPV4_DATAGRAM;
            }
            else {
                return ntsa::Transport::e_UNDEFINED;
            }
        }
        else if (d_transportDomain == ntsa::TransportDomain::e_IPV6) {
            if (d_transportMode == ntsa::TransportMode::e_DATAGRAM) {
                return ntsa::Transport::e_UDP_IPV6_DATAGRAM;
            }
            else {
                return ntsa::Transport::e_UNDEFINED;
            }
        }
        else {
            return ntsa::Transport::e_UNDEFINED;
        }
    }
    else if (d_transportProtocol == ntsa::TransportProtocol::e_LOCAL) {
        if (d_transportMode == ntsa::TransportMode::e_STREAM) {
            return ntsa::Transport::e_LOCAL_STREAM;
        }
        else if (d_transportMode == ntsa::TransportMode::e_DATAGRAM) {
            return ntsa::Transport::e_LOCAL_DATAGRAM;
        }
        else {
            return ntsa::Transport::e_UNDEFINED;
        }
    }
    else if (d_transportDomain == ntsa::TransportDomain::e_LOCAL) {
        if (d_transportMode == ntsa::TransportMode::e_STREAM) {
            return ntsa::Transport::e_LOCAL_STREAM;
        }
        else if (d_transportMode == ntsa::TransportMode::e_DATAGRAM) {
            return ntsa::Transport::e_LOCAL_DATAGRAM;
        }
        else {
            return ntsa::Transport::e_UNDEFINED;
        }
    }
    else {
        return ntsa::Transport::e_UNDEFINED;
    }
}

bslma::Allocator* TransportSuite::allocator() const
{
    return d_application.get_allocator().mechanism();
}

bool TransportSuite::equals(const TransportSuite& other) const
{
    return d_application       == other.d_application &&
           d_transportSecurity == other.d_transportSecurity &&
           d_transportProtocol == other.d_transportProtocol &&
           d_transportDomain   == other.d_transportDomain &&
           d_transportMode     == other.d_transportMode;
}

bool TransportSuite::less(const TransportSuite& other) const
{
    if (d_application < other.d_application) {
        return true;
    }

    if (other.d_application < d_application) {
        return false;
    }

    if (d_transportSecurity < other.d_transportSecurity) {
        return true;
    }

    if (other.d_transportSecurity < d_transportSecurity) {
        return false;
    }

    if (d_transportProtocol < other.d_transportProtocol) {
        return true;
    }

    if (other.d_transportProtocol < d_transportProtocol) {
        return false;
    }

    if (d_transportDomain < other.d_transportDomain) {
        return true;
    }

    if (other.d_transportDomain < d_transportDomain) {
        return false;
    }

    return d_transportMode < other.d_transportMode;
}

bsl::ostream& TransportSuite::print(bsl::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_application.empty()) {
        printer.printAttribute("application", d_application);
    }

    if (d_transportSecurity != ntsa::TransportSecurity::e_UNDEFINED) {
        printer.printAttribute("security", d_transportSecurity);
    }

    if (d_transportProtocol != ntsa::TransportProtocol::e_UNDEFINED) {
        printer.printAttribute("protocol", d_transportProtocol);
    }

    if (d_transportDomain != ntsa::TransportDomain::e_UNDEFINED) {
        printer.printAttribute("domain", d_transportDomain);
    }

    if (d_transportMode != ntsa::TransportMode::e_UNDEFINED) {
        printer.printAttribute("mode", d_transportMode);
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const TransportSuite& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const TransportSuite& lhs, const TransportSuite& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TransportSuite& lhs, const TransportSuite& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const TransportSuite& lhs, const TransportSuite& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
