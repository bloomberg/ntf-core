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

#include <ntsa_ipv4endpoint.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv4endpoint_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a private implementation.
class Ipv4Endpoint::Impl
{
  public:
    /// Throw an exception indicating the specified 'text' is in an invalid
    /// format.
    static void throwEndpointInvalidFormat(const bslstl::StringRef& text);
};

void Ipv4Endpoint::Impl::throwEndpointInvalidFormat(
    const bslstl::StringRef& text)
{
    bsl::stringstream ss;
    ss << "Failed to parse endpoint: the text '" << text << "' is invalid";

    NTSCFG_THROW(ss.str());
}

Ipv4Endpoint::Ipv4Endpoint()
: d_host()
, d_port(0)
{
}

Ipv4Endpoint::Ipv4Endpoint(const ntsa::Ipv4Address& address, ntsa::Port port)
: d_host(address)
, d_port(port)
{
}

Ipv4Endpoint::Ipv4Endpoint(const bslstl::StringRef& text)
{
    if (!this->parse(text)) {
        Impl::throwEndpointInvalidFormat(text);
    }
}

Ipv4Endpoint::Ipv4Endpoint(const bslstl::StringRef& addressText,
                           ntsa::Port               port)
: d_host(addressText)
, d_port(port)
{
}

Ipv4Endpoint::Ipv4Endpoint(bslmf::MovableRef<Ipv4Endpoint> other)
    NTSCFG_NOEXCEPT
: d_host(NTSCFG_MOVE_FROM(other, d_host))
, d_port(NTSCFG_MOVE_FROM(other, d_port))
{
    NTSCFG_MOVE_RESET(other);
}

Ipv4Endpoint::Ipv4Endpoint(const Ipv4Endpoint& other)
: d_host(other.d_host)
, d_port(other.d_port)
{
}

Ipv4Endpoint::~Ipv4Endpoint()
{
}

Ipv4Endpoint& Ipv4Endpoint::operator=(bslmf::MovableRef<Ipv4Endpoint> other)
    NTSCFG_NOEXCEPT
{
    if (this != &other) {
        d_host = NTSCFG_MOVE_FROM(other, d_host);
        d_port = NTSCFG_MOVE_FROM(other, d_port);

        NTSCFG_MOVE_RESET(other);
    }

    return *this;
}

Ipv4Endpoint& Ipv4Endpoint::operator=(const Ipv4Endpoint& other)
{
    if (this != &other) {
        d_host = other.d_host;
        d_port = other.d_port;
    }

    return *this;
}

Ipv4Endpoint& Ipv4Endpoint::operator=(const bslstl::StringRef& text)
{
    if (!this->parse(text)) {
        Impl::throwEndpointInvalidFormat(text);
    }

    return *this;
}

void Ipv4Endpoint::reset()
{
    d_host.reset();
    d_port = 0;
}

bool Ipv4Endpoint::parse(const bslstl::StringRef& text)
{
    const char* begin = text.begin();
    const char* end   = text.end();

    if (begin == end) {
        return false;
    }

    const char* mark = end - 1;
    while (mark > begin) {
        if (*mark == ':') {
            break;
        }

        --mark;
    }

    if (mark == begin) {
        return false;  // missing ':'
    }

    const char* ipv4AddressBegin = begin;
    const char* ipv4AddressEnd   = mark;

    if (!d_host.parse(bslstl::StringRef(ipv4AddressBegin, ipv4AddressEnd))) {
        return false;
    }

    const char* portBegin = mark + 1;
    const char* portEnd   = end;

    if (portBegin >= portEnd) {
        return false;  // missing port
    }

    if (!ntsa::PortUtil::parse(&d_port, portBegin, portEnd - portBegin)) {
        return false;
    }

    return true;
}

void Ipv4Endpoint::setHost(const ntsa::Ipv4Address& value)
{
    d_host = value;
}

void Ipv4Endpoint::setHost(const bslstl::StringRef& addressText)
{
    d_host = addressText;
}

void Ipv4Endpoint::setPort(ntsa::Port value)
{
    d_port = value;
}

const ntsa::Ipv4Address& Ipv4Endpoint::host() const
{
    return d_host;
}

ntsa::Port Ipv4Endpoint::port() const
{
    return d_port;
}

bsl::size_t Ipv4Endpoint::format(char*       buffer,
                                 bsl::size_t capacity,
                                 bool        collapse) const
{
    NTSCFG_WARNING_UNUSED(collapse);

    char*       t = buffer;
    bsl::size_t c = capacity;
    bsl::size_t n;

    n = d_host.format(t, c);
    BSLS_ASSERT(n != 0);

    t += n;
    c -= n;

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

bsl::string Ipv4Endpoint::text() const
{
    char        buffer[ntsa::Ipv4Endpoint::MAX_TEXT_LENGTH + 1];
    bsl::size_t size = this->format(buffer, sizeof buffer);

    return bsl::string(buffer, buffer + size);
}

bool Ipv4Endpoint::equals(const Ipv4Endpoint& other) const
{
    return d_host.equals(other.d_host) && d_port == other.d_port;
}

bool Ipv4Endpoint::less(const Ipv4Endpoint& other) const
{
    if (d_host.less(other.d_host)) {
        return true;
    }

    if (other.d_host.less(d_host)) {
        return false;
    }

    return d_port < other.d_port;
}

bsl::ostream& Ipv4Endpoint::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    char buffer[ntsa::Ipv4Endpoint::MAX_TEXT_LENGTH + 1];
    this->format(buffer, sizeof buffer);

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);
    stream << buffer;
    printer.end(true);

    return stream;
}

ntsa::Ipv4Address Ipv4Endpoint::anyIpv4Address()
{
    return ntsa::Ipv4Address::any();
}

ntsa::Ipv4Address Ipv4Endpoint::loopbackIpv4Address()
{
    return ntsa::Ipv4Address::loopback();
}

ntsa::Port Ipv4Endpoint::anyPort()
{
    return 0;
}

bsl::ostream& operator<<(bsl::ostream& stream, const Ipv4Endpoint& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const Ipv4Endpoint& lhs, const Ipv4Endpoint& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const Ipv4Endpoint& lhs, const Ipv4Endpoint& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const Ipv4Endpoint& lhs, const Ipv4Endpoint& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
