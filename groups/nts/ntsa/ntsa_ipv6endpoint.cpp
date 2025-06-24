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

#include <ntsa_ipv6endpoint.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv6endpoint_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a private implementation.
class Ipv6Endpoint::Impl
{
  public:
    /// Throw an exception indicating the specified 'text' is in an invalid
    /// format.
    static void throwEndpointInvalidFormat(const bslstl::StringRef& text);
};

void Ipv6Endpoint::Impl::throwEndpointInvalidFormat(
    const bslstl::StringRef& text)
{
    bsl::stringstream ss;
    ss << "Failed to parse endpoint: the text '" << text << "' is invalid";

    NTSCFG_THROW(ss.str());
}

Ipv6Endpoint::Ipv6Endpoint()
: d_host()
, d_port(0)
{
}

Ipv6Endpoint::Ipv6Endpoint(const ntsa::Ipv6Address& address, ntsa::Port port)
: d_host(address)
, d_port(port)
{
}

Ipv6Endpoint::Ipv6Endpoint(const bslstl::StringRef& text)
{
    if (!this->parse(text)) {
        Impl::throwEndpointInvalidFormat(text);
    }
}

Ipv6Endpoint::Ipv6Endpoint(const bslstl::StringRef& addressText,
                           ntsa::Port               port)
: d_host(addressText)
, d_port(port)
{
}

Ipv6Endpoint::Ipv6Endpoint(bslmf::MovableRef<Ipv6Endpoint> other)
    NTSCFG_NOEXCEPT
: d_host(NTSCFG_MOVE_FROM(other, d_host))
, d_port(NTSCFG_MOVE_FROM(other, d_port))
{
    NTSCFG_MOVE_RESET(other);
}

Ipv6Endpoint::Ipv6Endpoint(const Ipv6Endpoint& other)
: d_host(other.d_host)
, d_port(other.d_port)
{
}

Ipv6Endpoint::~Ipv6Endpoint()
{
}

Ipv6Endpoint& Ipv6Endpoint::operator=(bslmf::MovableRef<Ipv6Endpoint> other)
    NTSCFG_NOEXCEPT
{
    if (this != &NTSCFG_MOVE_ACCESS(other)) {
        d_host = NTSCFG_MOVE_FROM(other, d_host);
        d_port = NTSCFG_MOVE_FROM(other, d_port);

        NTSCFG_MOVE_RESET(other);
    }

    return *this;
}

Ipv6Endpoint& Ipv6Endpoint::operator=(const Ipv6Endpoint& other)
{
    if (this != &other) {
        d_host = other.d_host;
        d_port = other.d_port;
    }

    return *this;
}

Ipv6Endpoint& Ipv6Endpoint::operator=(const bslstl::StringRef& text)
{
    if (!this->parse(text)) {
        Impl::throwEndpointInvalidFormat(text);
    }

    return *this;
}

void Ipv6Endpoint::reset()
{
    d_host.reset();
    d_port = 0;
}

bool Ipv6Endpoint::parse(const bslstl::StringRef& text)
{
    const char* begin = text.begin();
    const char* end   = text.end();

    if (begin == end) {
        return false;
    }

    if (*begin != '[') {
        return false;
    }

    const char* mark = end - 1;
    while (mark > begin) {
        if (*mark == ']') {
            break;
        }

        --mark;
    }

    if (mark == begin) {
        return false;  // missing ']'
    }

    const char* ipv6AddressBegin = begin + 1;
    const char* ipv6AddressEnd   = mark;

    if (!d_host.parse(bslstl::StringRef(ipv6AddressBegin, ipv6AddressEnd))) {
        return false;
    }

    const char* colon = mark + 1;

    if (colon >= end) {
        return false;  // missing ':'
    }

    if (*colon != ':') {
        return false;  // missing ':'
    }

    const char* portBegin = colon + 1;
    const char* portEnd   = end;

    if (portBegin >= portEnd) {
        return false;  // missing port
    }

    if (!ntsa::PortUtil::parse(&d_port, portBegin, portEnd - portBegin)) {
        return false;
    }

    return true;
}

void Ipv6Endpoint::setHost(const ntsa::Ipv6Address& value)
{
    d_host = value;
}

void Ipv6Endpoint::setHost(const bslstl::StringRef& addressText)
{
    d_host = addressText;
}

void Ipv6Endpoint::setPort(ntsa::Port value)
{
    d_port = value;
}

const ntsa::Ipv6Address& Ipv6Endpoint::host() const
{
    return d_host;
}

ntsa::Port Ipv6Endpoint::port() const
{
    return d_port;
}

bsl::size_t Ipv6Endpoint::format(char*       buffer,
                                 bsl::size_t capacity,
                                 bool        collapse) const
{
    char*       t = buffer;
    bsl::size_t c = capacity;
    bsl::size_t n;

    if (c == 0) {
        return capacity - c;
    }

    *t  = '[';
    t  += 1;
    c  -= 1;

    n = d_host.format(t, c, collapse);
    BSLS_ASSERT(n != 0);

    t += n;
    c -= n;

    if (c == 0) {
        return capacity - c;
    }

    *t  = ']';
    t  += 1;
    c  -= 1;

    if (c == 0) {
        return capacity - c;
    }

    *t  = ':';
    t  += 1;
    c  -= 1;

    n = ntsa::PortUtil::format(t, c, d_port);

    t += n;
    c -= n;

    if (c > 0) {
        *t = 0;
    }

    return capacity - c;
}

bsl::string Ipv6Endpoint::text() const
{
    char        buffer[ntsa::Ipv6Endpoint::MAX_TEXT_LENGTH + 1];
    bsl::size_t size = this->format(buffer, sizeof buffer);

    return bsl::string(buffer, buffer + size);
}

bool Ipv6Endpoint::equals(const Ipv6Endpoint& other) const
{
    return d_host.equals(other.d_host) && d_port == other.d_port;
}

bool Ipv6Endpoint::less(const Ipv6Endpoint& other) const
{
    if (d_host.less(other.d_host)) {
        return true;
    }

    if (other.d_host.less(d_host)) {
        return false;
    }

    return d_port < other.d_port;
}

bsl::ostream& Ipv6Endpoint::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    char buffer[ntsa::Ipv6Endpoint::MAX_TEXT_LENGTH + 1];
    this->format(buffer, sizeof buffer);

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);
    stream << buffer;
    printer.end(true);

    return stream;
}

ntsa::Ipv6Address Ipv6Endpoint::anyIpv6Address()
{
    return ntsa::Ipv6Address::any();
}

ntsa::Ipv6Address Ipv6Endpoint::loopbackIpv6Address()
{
    return ntsa::Ipv6Address::loopback();
}

ntsa::Port Ipv6Endpoint::anyPort()
{
    return 0;
}

bsl::ostream& operator<<(bsl::ostream& stream, const Ipv6Endpoint& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const Ipv6Endpoint& lhs, const Ipv6Endpoint& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const Ipv6Endpoint& lhs, const Ipv6Endpoint& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const Ipv6Endpoint& lhs, const Ipv6Endpoint& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
