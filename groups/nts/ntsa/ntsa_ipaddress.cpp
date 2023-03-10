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

#include <ntsa_ipaddress.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipaddress_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_cctype.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntsa {

namespace {

void throwIpInvalidFormat(const bslstl::StringRef& text)
{
    bsl::stringstream ss;
    ss << "Failed to parse IP address: the text '" << text << "' is invalid";

    NTSCFG_THROW(ss.str());
}

}  // close unnamed namespace

IpAddress::IpAddress(const bslstl::StringRef& text)
: d_type(ntsa::IpAddressType::e_UNDEFINED)
{
    if (!this->parse(text)) {
        throwIpInvalidFormat(text);
    }
}

IpAddress& IpAddress::operator=(const bslstl::StringRef& text)
{
    this->reset();

    if (text.empty()) {
        return *this;
    }

    bool valid;

    valid = this->makeV4().parse(text);
    if (valid) {
        return *this;
    }

    valid = this->makeV6().parse(text);
    if (valid) {
        return *this;
    }

    this->reset();

    throwIpInvalidFormat(text);
    return *this;
}

bool IpAddress::parse(const bslstl::StringRef& text)
{
    this->reset();

    if (text.empty()) {
        return true;
    }

    bool valid;

    valid = this->makeV4().parse(text);
    if (valid) {
        return true;
    }

    valid = this->makeV6().parse(text);
    if (valid) {
        return true;
    }

    this->reset();
    return false;
}

bsl::string IpAddress::text() const
{
    switch (d_type) {
    case ntsa::IpAddressType::e_V4:
        return d_v4.object().text();
    case ntsa::IpAddressType::e_V6:
        return d_v6.object().text();
    default:
        return "";
    }
}

bool IpAddress::equals(const IpAddress& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::IpAddressType::e_V4:
        return d_v4.object().equals(other.d_v4.object());
    case ntsa::IpAddressType::e_V6:
        return d_v6.object().equals(other.d_v6.object());
    default:
        return true;
    }
}

bool IpAddress::less(const IpAddress& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::IpAddressType::e_V4:
        return d_v4.object().less(other.d_v4.object());
    case ntsa::IpAddressType::e_V6:
        return d_v6.object().less(other.d_v6.object());
    default:
        return true;
    }
}

bsl::ostream& IpAddress::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    switch (d_type) {
    case ntsa::IpAddressType::e_V4:
        d_v4.object().print(stream, level, spacesPerLevel);
        break;
    case ntsa::IpAddressType::e_V6:
        d_v6.object().print(stream, level, spacesPerLevel);
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::IpAddressType::e_UNDEFINED);
        stream << "UNDEFINED";
        break;
    }

    return stream;
}

ntsa::IpAddress IpAddress::any(ntsa::IpAddressType::Value addressType)
{
    if (addressType == ntsa::IpAddressType::e_V4) {
        return ntsa::IpAddress::anyIpv4();
    }
    else if (addressType == ntsa::IpAddressType::e_V6) {
        return ntsa::IpAddress::anyIpv6();
    }
    else {
        return ntsa::IpAddress();
    }
}

ntsa::IpAddress IpAddress::loopback(ntsa::IpAddressType::Value addressType)
{
    if (addressType == ntsa::IpAddressType::e_V4) {
        return ntsa::IpAddress::loopbackIpv4();
    }
    else if (addressType == ntsa::IpAddressType::e_V6) {
        return ntsa::IpAddress::loopbackIpv6();
    }
    else {
        return ntsa::IpAddress();
    }
}

}  // close package namespace
}  // close enterprise namespace
