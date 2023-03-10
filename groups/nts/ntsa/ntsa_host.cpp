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

#include <ntsa_host.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_host_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntsa {

namespace {

void throwAddressInvalidFormat(const bslstl::StringRef& text)
{
    bsl::stringstream ss;
    ss << "Failed to parse address: the text '" << text << "' is invalid";

    NTSCFG_THROW(ss.str());
}

}  // close unnamed namespace

Host::Host(const bslstl::StringRef& text)
: d_type(ntsa::HostType::e_UNDEFINED)
{
    if (!this->parse(text)) {
        throwAddressInvalidFormat(text);
    }
}

Host::Host(const ntsa::DomainName& value)
: d_type(ntsa::HostType::e_DOMAIN_NAME)
{
    new (d_domainName.buffer()) ntsa::DomainName(value);
}

Host::Host(const ntsa::IpAddress& value)
: d_type(ntsa::HostType::e_UNDEFINED)
{
    new (d_ip.buffer()) ntsa::IpAddress(value);
    d_type = ntsa::HostType::e_IP;
}

Host::Host(const Host& other)
: d_type(other.d_type)
{
    switch (d_type) {
    case ntsa::HostType::e_DOMAIN_NAME:
        new (d_domainName.buffer())
            ntsa::DomainName(other.d_domainName.object());
        break;
    case ntsa::HostType::e_IP:
        new (d_ip.buffer()) ntsa::IpAddress(other.d_ip.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::HostType::e_UNDEFINED);
    }
}

Host& Host::operator=(const Host& other)
{
    if (this == &other) {
        return *this;
    }

    switch (other.d_type) {
    case ntsa::HostType::e_DOMAIN_NAME:
        this->makeDomainName(other.d_domainName.object());
        break;
    case ntsa::HostType::e_IP:
        this->makeIp(other.d_ip.object());
        break;
    default:
        BSLS_ASSERT(other.d_type == ntsa::HostType::e_UNDEFINED);
        this->reset();
    }

    return *this;
}

Host& Host::operator=(const ntsa::DomainName& other)
{
    this->makeDomainName(other);
    return *this;
}

Host& Host::operator=(const ntsa::IpAddress& other)
{
    this->makeIp(other);
    return *this;
}

Host& Host::operator=(const bslstl::StringRef& text)
{
    this->reset();

    if (text.empty()) {
        return *this;
    }

    bool valid;

    valid = this->makeIp().parse(text);
    if (valid) {
        return *this;
    }

    valid = this->makeDomainName().parse(text);
    if (valid) {
        return *this;
    }

    this->reset();

    throwAddressInvalidFormat(text);
    return *this;
}

bool Host::parse(const bslstl::StringRef& text)
{
    this->reset();

    if (text.empty()) {
        return true;
    }

    bool valid;

    valid = this->makeIp().parse(text);
    if (valid) {
        return true;
    }

    valid = this->makeDomainName().parse(text);
    if (valid) {
        return true;
    }

    this->reset();
    return false;
}

ntsa::DomainName& Host::makeDomainName()
{
    if (d_type == ntsa::HostType::e_DOMAIN_NAME) {
        d_domainName.object().reset();
    }
    else {
        this->reset();
        new (d_domainName.buffer()) ntsa::DomainName();
        d_type = ntsa::HostType::e_DOMAIN_NAME;
    }

    return d_domainName.object();
}

ntsa::DomainName& Host::makeDomainName(const ntsa::DomainName& value)
{
    if (d_type == ntsa::HostType::e_DOMAIN_NAME) {
        d_domainName.object() = value;
    }
    else {
        this->reset();
        new (d_domainName.buffer()) ntsa::DomainName(value);
        d_type = ntsa::HostType::e_DOMAIN_NAME;
    }

    return d_domainName.object();
}

ntsa::IpAddress& Host::makeIp()
{
    if (d_type == ntsa::HostType::e_IP) {
        d_ip.object().reset();
    }
    else {
        this->reset();
        new (d_ip.buffer()) ntsa::IpAddress();
        d_type = ntsa::HostType::e_IP;
    }

    return d_ip.object();
}

ntsa::IpAddress& Host::makeIp(const ntsa::IpAddress& value)
{
    if (d_type == ntsa::HostType::e_IP) {
        d_ip.object() = value;
    }
    else {
        this->reset();
        new (d_ip.buffer()) ntsa::IpAddress(value);
        d_type = ntsa::HostType::e_IP;
    }

    return d_ip.object();
}

bsl::string Host::text() const
{
    switch (d_type) {
    case ntsa::HostType::e_DOMAIN_NAME:
        return d_domainName.object().text();
    case ntsa::HostType::e_IP:
        return d_ip.object().text();
    default:
        return "";
    }
}

bool Host::equals(const Host& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::HostType::e_DOMAIN_NAME:
        return d_domainName.object() == other.d_domainName.object();
    case ntsa::HostType::e_IP:
        return d_ip.object().equals(other.d_ip.object());
    default:
        return true;
    }
}

bool Host::less(const Host& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::HostType::e_DOMAIN_NAME:
        return d_domainName.object().less(other.d_domainName.object());
    case ntsa::HostType::e_IP:
        return d_ip.object().less(other.d_ip.object());
    default:
        return true;
    }
}

bsl::ostream& Host::print(bsl::ostream& stream,
                          int           level,
                          int           spacesPerLevel) const
{
    switch (d_type) {
    case ntsa::HostType::e_DOMAIN_NAME:
        d_domainName.object().print(stream, level, spacesPerLevel);
        break;
    case ntsa::HostType::e_IP:
        d_ip.object().print(stream, level, spacesPerLevel);
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::HostType::e_UNDEFINED);
        stream << "UNDEFINED";
        break;
    }

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
