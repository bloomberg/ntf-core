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

/// Provide a private implementation.
class Host::Impl
{
  public:
    /// Throw an exception indicating the specified 'text' is in an invalid
    /// format.
    static void throwAddressInvalidFormat(const bslstl::StringRef& text);
};

void Host::Impl::throwAddressInvalidFormat(const bslstl::StringRef& text)
{
    bsl::stringstream ss;
    ss << "Failed to parse address: the text '" << text << "' is invalid";

    NTSCFG_THROW(ss.str());
}

Host::Host(const bslstl::StringRef& text)
: d_type(ntsa::HostType::e_UNDEFINED)
{
    if (!this->parse(text)) {
        Impl::throwAddressInvalidFormat(text);
    }
}

Host::Host(const ntsa::DomainName& value)
: d_type(ntsa::HostType::e_DOMAIN_NAME)
{
    new (d_domainName.buffer()) ntsa::DomainName(value);
}

Host::Host(const ntsa::IpAddress& value)
: d_type(ntsa::HostType::e_IP)
{
    new (d_ip.buffer()) ntsa::IpAddress(value);
}

Host::Host(const ntsa::LocalName& value)
: d_type(ntsa::HostType::e_LOCAL_NAME)
{
    new (d_localName.buffer()) ntsa::LocalName(value);
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
    case ntsa::HostType::e_LOCAL_NAME:
        new (d_localName.buffer()) ntsa::LocalName(other.d_localName.object());
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
    case ntsa::HostType::e_LOCAL_NAME:
        this->makeLocalName(other.d_localName.object());
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

Host& Host::operator=(const ntsa::LocalName& other)
{
    this->makeLocalName(other);
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

    if (text.size() > 0 && text[0] == '/') {
        this->makeLocalName().setValue(text);
        return *this;
    }

    this->reset();

    Impl::throwAddressInvalidFormat(text);
    return *this;
}

bool Host::parse(const bslstl::StringRef& text)
{
    ntsa::Error error;

    this->reset();

    if (text.empty()) {
        return true;
    }

    bool valid;

    if (text[0] == '/') {
        error = this->makeLocalName().setValue(text);
        if (error) {
            this->reset();
            return false;
        }

        return true;
    }

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

ntsa::LocalName& Host::makeLocalName()
{
    if (d_type == ntsa::HostType::e_LOCAL_NAME) {
        d_localName.object().reset();
    }
    else {
        this->reset();
        new (d_localName.buffer()) ntsa::LocalName();
        d_type = ntsa::HostType::e_LOCAL_NAME;
    }

    return d_localName.object();
}

ntsa::LocalName& Host::makeLocalName(const ntsa::LocalName& value)
{
    if (d_type == ntsa::HostType::e_LOCAL_NAME) {
        d_localName.object() = value;
    }
    else {
        this->reset();
        new (d_localName.buffer()) ntsa::LocalName(value);
        d_type = ntsa::HostType::e_LOCAL_NAME;
    }

    return d_localName.object();
}

bsl::string Host::text() const
{
    switch (d_type) {
    case ntsa::HostType::e_DOMAIN_NAME:
        return d_domainName.object().text();
    case ntsa::HostType::e_IP:
        return d_ip.object().text();
    case ntsa::HostType::e_LOCAL_NAME:
        return d_localName.object().value();
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
    case ntsa::HostType::e_LOCAL_NAME:
        return d_localName.object().equals(other.d_localName.object());
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
    case ntsa::HostType::e_LOCAL_NAME:
        return d_localName.object().less(other.d_localName.object());
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
    case ntsa::HostType::e_LOCAL_NAME:
        d_localName.object().print(stream, level, spacesPerLevel);
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::HostType::e_UNDEFINED);
        stream << "UNDEFINED";
        break;
    }

    return stream;
}

const bdlat_SelectionInfo* Host::lookupSelectionInfo(int id)
{
    const int numSelections =
        sizeof(SELECTION_INFO_ARRAY) / sizeof(SELECTION_INFO_ARRAY[0]);

    if (id < 0 || id >= numSelections) {
        return 0;
    }

    return &SELECTION_INFO_ARRAY[id];
}

const bdlat_SelectionInfo* Host::lookupSelectionInfo(const char* name,
                                                     int         nameLength)
{
    const bsl::size_t numSelections =
        sizeof(SELECTION_INFO_ARRAY) / sizeof(SELECTION_INFO_ARRAY[0]);

    for (bsl::size_t i = 0; i < numSelections; ++i) {
        const bdlat_SelectionInfo& selectionInfo = SELECTION_INFO_ARRAY[i];
        if (selectionInfo.d_nameLength == nameLength) {
            const int compare =
                bsl::memcmp(selectionInfo.d_name_p, name, nameLength);
            if (compare == 0) {
                return &selectionInfo;
            }
        }
    }

    return 0;
}

const char Host::CLASS_NAME[11] = "ntsa::Host";

// clang-format off
const bdlat_SelectionInfo Host::SELECTION_INFO_ARRAY[4] =
{
    { ntsa::HostType::e_UNDEFINED,   "undefined", 9, "", 0 },
    { ntsa::HostType::e_DOMAIN_NAME, "domain",    6, "", 0 },
    { ntsa::HostType::e_IP,          "ip",        2, "", 0 },
    { ntsa::HostType::e_LOCAL_NAME,  "local",     5, "", 0 }
};
// clang-format on

}  // close package namespace
}  // close enterprise namespace
