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

#include <ntsa_ipendpoint.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipendpoint_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntsa {

namespace {

void throwEndpointInvalidFormat(const bslstl::StringRef& text)
{
    bsl::stringstream ss;
    ss << "Failed to parse endpoint: the text '" << text << "' is invalid";

    NTSCFG_THROW(ss.str());
}

}  // close unnamed namespace

IpEndpoint::IpEndpoint(const bslstl::StringRef& text)
{
    if (!this->parse(text)) {
        throwEndpointInvalidFormat(text);
    }
}

IpEndpoint::IpEndpoint(const bslstl::StringRef& addressText, ntsa::Port port)
: d_host(addressText)
, d_port(port)
{
}

IpEndpoint& IpEndpoint::operator=(const bslstl::StringRef& text)
{
    if (!this->parse(text)) {
        throwEndpointInvalidFormat(text);
    }

    return *this;
}

bool IpEndpoint::parse(const bslstl::StringRef& text)
{
    const char* begin = text.begin();
    const char* end   = text.end();

    if (begin == end) {
        return false;
    }

    if (*begin == '[') {
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

        if (!d_host.makeV6().parse(
                bslstl::StringRef(ipv6AddressBegin, ipv6AddressEnd)))
        {
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
    }
    else {
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

        if (!d_host.makeV4().parse(
                bslstl::StringRef(ipv4AddressBegin, ipv4AddressEnd)))
        {
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
    }

    return true;
}

bsl::size_t IpEndpoint::format(char*       buffer,
                               bsl::size_t capacity,
                               bool        collapse) const
{
    char*       t = buffer;
    bsl::size_t c = capacity;
    bsl::size_t n;

    if (d_host.isV4()) {
        n = d_host.v4().format(t, c);
        BSLS_ASSERT(n != 0);

        t += n;
        c -= n;

        *t  = ':';
        t  += 1;
        c  -= 1;

        n = ntsa::PortUtil::format(t, c, d_port);

        t += n;
        c -= n;
    }
    else if (d_host.isV6()) {
        if (c == 0) {
            return capacity - c;
        }

        *t  = '[';
        t  += 1;
        c  -= 1;

        n = d_host.v6().format(t, c, collapse);
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
    }

    if (c > 0) {
        *t = 0;
    }

    return capacity - c;
}

bsl::string IpEndpoint::text() const
{
    char        buffer[ntsa::IpEndpoint::MAX_TEXT_LENGTH + 1];
    bsl::size_t size = this->format(buffer, sizeof buffer);

    return bsl::string(buffer, buffer + size);
}

bool IpEndpoint::equals(const IpEndpoint& other) const
{
    return d_host.equals(other.d_host) && d_port == other.d_port;
}

bool IpEndpoint::less(const IpEndpoint& other) const
{
    if (d_host.less(other.d_host)) {
        return false;
    }

    if (other.d_host.less(d_host)) {
        return true;
    }

    return d_port < other.d_port;
}

bsl::ostream& IpEndpoint::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    char buffer[ntsa::IpEndpoint::MAX_TEXT_LENGTH + 1];
    this->format(buffer, sizeof buffer);

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);
    stream << buffer;
    printer.end(true);

    return stream;
}

ntsa::IpAddress IpEndpoint::anyIpv4Address()
{
    return ntsa::IpAddress(ntsa::Ipv4Address::any());
}

ntsa::IpAddress IpEndpoint::anyIpv6Address()
{
    return ntsa::IpAddress(ntsa::Ipv6Address::any());
}

ntsa::IpAddress IpEndpoint::loopbackIpv4Address()
{
    return ntsa::IpAddress(ntsa::Ipv4Address::loopback());
}

ntsa::IpAddress IpEndpoint::loopbackIpv6Address()
{
    return ntsa::IpAddress(ntsa::Ipv6Address::loopback());
}

ntsa::IpAddress IpEndpoint::anyAddress(ntsa::IpAddressType::Value addressType)
{
    if (addressType == ntsa::IpAddressType::e_V4) {
        return IpEndpoint::anyIpv4Address();
    }
    else if (addressType == ntsa::IpAddressType::e_V6) {
        return IpEndpoint::anyIpv6Address();
    }
    else {
        return ntsa::IpAddress();
    }
}

ntsa::IpAddress IpEndpoint::loopbackAddress(
    ntsa::IpAddressType::Value addressType)
{
    if (addressType == ntsa::IpAddressType::e_V4) {
        return IpEndpoint::loopbackIpv4Address();
    }
    else if (addressType == ntsa::IpAddressType::e_V6) {
        return IpEndpoint::loopbackIpv6Address();
    }
    else {
        return ntsa::IpAddress();
    }
}

ntsa::Port IpEndpoint::anyPort()
{
    return 0;
}

bsl::ostream& operator<<(bsl::ostream& stream, const IpEndpoint& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const IpEndpoint& lhs, const IpEndpoint& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const IpEndpoint& lhs, const IpEndpoint& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const IpEndpoint& lhs, const IpEndpoint& rhs)
{
    return lhs.less(rhs);
}

const bdlat_AttributeInfo* IpEndpoint::lookupAttributeInfo(int id)
{
    const int numAttributes = 
        sizeof(ATTRIBUTE_INFO_ARRAY) / sizeof(ATTRIBUTE_INFO_ARRAY[0]);

    if (id < 0 || id >= numAttributes) {
        return 0;
    }

    return &ATTRIBUTE_INFO_ARRAY[id];
}

const bdlat_AttributeInfo* IpEndpoint::lookupAttributeInfo(
    const char* name, 
    int         nameLength)
{
    const bsl::size_t numAttributes = 
        sizeof(ATTRIBUTE_INFO_ARRAY) / sizeof(ATTRIBUTE_INFO_ARRAY[0]);

    for (bsl::size_t i = 0; i < numAttributes; ++i) {
        const bdlat_AttributeInfo& attributeInfo = ATTRIBUTE_INFO_ARRAY[i];
        if (attributeInfo.d_nameLength == nameLength) {
            const int compare = 
                bsl::memcmp(attributeInfo.d_name_p, name, nameLength);
            if (compare == 0) {
                return &attributeInfo;
            }
        }
    }

    return 0;
}

const char IpEndpoint::CLASS_NAME[17] = "ntsa::IpEndpoint";

// clang-format off
const bdlat_AttributeInfo IpEndpoint::ATTRIBUTE_INFO_ARRAY[2] =
{
    { e_ATTRIBUTE_ID_HOST, "host", 4, "", 0 },
    { e_ATTRIBUTE_ID_PORT, "port", 4, "", 0 },
};
// clang-format on

}  // close package namespace
}  // close enterprise namespace
