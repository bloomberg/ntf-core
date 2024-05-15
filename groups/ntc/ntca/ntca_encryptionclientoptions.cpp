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

#include <ntca_encryptionclientoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptionclientoptions_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

EncryptionClientOptions::EncryptionClientOptions(
    bslma::Allocator* basicAllocator)
: d_options(basicAllocator)
, d_optionsMap(basicAllocator)
, d_serverNameIndication(basicAllocator)
{
}

EncryptionClientOptions::EncryptionClientOptions(
    const EncryptionClientOptions& other,
    bslma::Allocator*              basicAllocator)
: d_options(other.d_options, basicAllocator)
, d_optionsMap(other.d_optionsMap, basicAllocator)
, d_serverNameIndication(other.d_serverNameIndication, basicAllocator)
{
}

EncryptionClientOptions::~EncryptionClientOptions()
{
}

EncryptionClientOptions& EncryptionClientOptions::operator=(
    const EncryptionClientOptions& other)
{
    if (this != &other) {
        d_options              = other.d_options;
        d_optionsMap           = other.d_optionsMap;
        d_serverNameIndication = other.d_serverNameIndication;
    }

    return *this;
}

void EncryptionClientOptions::reset()
{
    d_options.reset();
    d_optionsMap.clear();
    d_serverNameIndication.reset();
}

void EncryptionClientOptions::setMinMethod(
    ntca::EncryptionMethod::Value minMethod)
{
    d_options.setMinMethod(minMethod);
}

void EncryptionClientOptions::setMaxMethod(
    ntca::EncryptionMethod::Value maxMethod)
{
    d_options.setMaxMethod(maxMethod);
}

void EncryptionClientOptions::setCipherSpec(const bsl::string& cipherSpec)
{
    d_options.setCipherSpec(cipherSpec);
}

void EncryptionClientOptions::setAuthentication(
    ntca::EncryptionAuthentication::Value authentication)
{
    d_options.setAuthentication(authentication);
}

void EncryptionClientOptions::setAuthorityDirectory(
    const bsl::string& authorityDirectory)
{
    d_options.setAuthorityDirectory(authorityDirectory);
}

void EncryptionClientOptions::addAuthorityList(
    const ntca::EncryptionCertificateVector& certificates)
{
    d_options.addAuthorityList(certificates);
}

void EncryptionClientOptions::addAuthority(
    const ntca::EncryptionCertificate& certificate)
{
    d_options.addAuthority(certificate);
}

void EncryptionClientOptions::addAuthorityData(
    const bsl::vector<char>& resourceData)
{
    d_options.addAuthorityData(resourceData);
}

void EncryptionClientOptions::addAuthorityData(
    const bsl::vector<char>&               resourceData,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    d_options.addAuthorityData(resourceData, resourceOptions);
}

void EncryptionClientOptions::addAuthorityFile(const bsl::string& resourcePath)
{
    d_options.addAuthorityFile(resourcePath);
}

void EncryptionClientOptions::addAuthorityFile(
    const bsl::string&                     resourcePath,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    d_options.addAuthorityFile(resourcePath, resourceOptions);
}

void EncryptionClientOptions::setIdentity(
    const ntca::EncryptionCertificate& certificate)
{
    d_options.setIdentity(certificate);
}

void EncryptionClientOptions::setIdentityData(
    const bsl::vector<char>& resourceData)
{
    d_options.setIdentityData(resourceData);
}

void EncryptionClientOptions::setIdentityData(
    const bsl::vector<char>&               resourceData,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    d_options.setIdentityData(resourceData, resourceOptions);
}

void EncryptionClientOptions::setIdentityFile(const bsl::string& resourcePath)
{
    d_options.setIdentityFile(resourcePath);
}

void EncryptionClientOptions::setIdentityFile(
    const bsl::string&                     resourcePath,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    d_options.setIdentityFile(resourcePath, resourceOptions);
}

void EncryptionClientOptions::setPrivateKey(const ntca::EncryptionKey& key)
{
    d_options.setPrivateKey(key);
}

void EncryptionClientOptions::setPrivateKeyData(
    const bsl::vector<char>& resourceData)
{
    d_options.setPrivateKeyData(resourceData);
}

void EncryptionClientOptions::setPrivateKeyData(
    const bsl::vector<char>&               resourceData,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    d_options.setPrivateKeyData(resourceData, resourceOptions);
}

void EncryptionClientOptions::setPrivateKeyFile(
    const bsl::string& resourcePath)
{
    d_options.setPrivateKeyFile(resourcePath);
}

void EncryptionClientOptions::setPrivateKeyFile(
    const bsl::string&                     resourcePath,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    d_options.setPrivateKeyFile(resourcePath, resourceOptions);
}

void EncryptionClientOptions::addResource(
    const ntca::EncryptionResource& resource)
{
    d_options.addResource(resource);
}

void EncryptionClientOptions::addResourceData(
    const bsl::vector<char>& resourceData)
{
    d_options.addResourceData(resourceData);
}

void EncryptionClientOptions::addResourceData(
    const bsl::vector<char>&               resourceData,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    d_options.addResourceData(resourceData, resourceOptions);
}

void EncryptionClientOptions::addResourceFile(const bsl::string& resourcePath)
{
    d_options.addResourceFile(resourcePath);
}

void EncryptionClientOptions::addResourceFile(
    const bsl::string&                     resourcePath,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    d_options.addResourceFile(resourcePath, resourceOptions);
}

void EncryptionClientOptions::addOverrides(
    const bsl::string&             serverName,
    const ntca::EncryptionOptions& options)
{
    if (serverName.empty() || serverName == "*") {
        d_options = options;
    }
    else {
        d_optionsMap[serverName] = options;
    }
}

void EncryptionClientOptions::setServerNameIndication(const bsl::string& name)
{
    ntsa::Uri uri;
    if (uri.parse(name)) {
        this->setServerNameIndication(uri);
    }
    else {
        d_serverNameIndication.makeValue(name);
    }
}

void EncryptionClientOptions::setServerNameIndication(
    const ntsa::Endpoint& endpoint)
{
    if (endpoint.isIp()) {
        this->setServerNameIndication(endpoint.ip());
    }
    else if (endpoint.isLocal()) {
        this->setServerNameIndication(endpoint.local());
    }
}

void EncryptionClientOptions::setServerNameIndication(
    const ntsa::IpEndpoint& ipEndpoint)
{
    if (ipEndpoint.host().isV4()) {
        this->setServerNameIndication(ipEndpoint.host().v4());
    }
    else if (ipEndpoint.host().isV6()) {
        this->setServerNameIndication(ipEndpoint.host().v6());
    }
}

void EncryptionClientOptions::setServerNameIndication(
    const ntsa::IpAddress& ipAddress)
{
    d_serverNameIndication.makeValue(ipAddress.text());
}

void EncryptionClientOptions::setServerNameIndication(
    const ntsa::Ipv4Address& ipAddress)
{
    d_serverNameIndication.makeValue(ipAddress.text());
}

void EncryptionClientOptions::setServerNameIndication(
    const ntsa::Ipv6Address& ipAddress)
{
    d_serverNameIndication.makeValue(ipAddress.text());
}

void EncryptionClientOptions::setServerNameIndication(
    const ntsa::LocalName& localName)
{
    d_serverNameIndication.makeValue(localName.value());
}

void EncryptionClientOptions::setServerNameIndication(const ntsa::Uri& uri)
{
    if (!uri.authority().isNull()) {
        const ntsa::UriAuthority& authority = uri.authority().value();
        if (!authority.host().isNull()) {
            const ntsa::Host& host = authority.host().value();
            this->setServerNameIndication(host);
        }
    }
}

void EncryptionClientOptions::setServerNameIndication(const ntsa::Host& host)
{
    if (host.isDomainName()) {
        this->setServerNameIndication(host.domainName());
    }
    else if (host.isIp()) {
        this->setServerNameIndication(host.ip());
    }
}

void EncryptionClientOptions::setServerNameIndication(
    const ntsa::DomainName& domainName)
{
    d_serverNameIndication.makeValue(domainName.text());
}

ntca::EncryptionMethod::Value EncryptionClientOptions::minMethod() const
{
    return d_options.minMethod();
}

ntca::EncryptionMethod::Value EncryptionClientOptions::maxMethod() const
{
    return d_options.maxMethod();
}

const bdlb::NullableValue<bsl::string>& EncryptionClientOptions::cipherSpec()
    const
{
    return d_options.cipherSpec();
}

ntca::EncryptionAuthentication::Value EncryptionClientOptions::authentication()
    const
{
    return d_options.authentication();
}

const bdlb::NullableValue<bsl::string>& EncryptionClientOptions::
    authorityDirectory() const
{
    return d_options.authorityDirectory();
}

const ntca::EncryptionResourceVector& EncryptionClientOptions::resources()
    const
{
    return d_options.resources();
}

const bdlb::NullableValue<bsl::string>& EncryptionClientOptions::
    serverNameIndication() const
{
    return d_serverNameIndication;
}

void EncryptionClientOptions::loadServerNameList(
    bsl::vector<bsl::string>* result) const
{
    result->clear();

    result->push_back("*");

    for (OptionsMap::const_iterator it = d_optionsMap.begin();
         it != d_optionsMap.end();
         ++it)
    {
        result->push_back(it->first);
    }
}

bool EncryptionClientOptions::loadServerNameOptions(
    ntca::EncryptionOptions* result,
    const bsl::string&       serverName) const
{
    result->reset();

    if (serverName.empty() || serverName == "*") {
        *result = d_options;
    }
    else {
        OptionsMap::const_iterator it = d_optionsMap.find(serverName);
        if (it == d_optionsMap.end()) {
            return false;
        }

        *result = it->second;
    }

    return true;
}

bool EncryptionClientOptions::equals(
    const EncryptionClientOptions& other) const
{
    return d_options == other.d_options &&
           d_optionsMap == other.d_optionsMap &&
           d_serverNameIndication == other.d_serverNameIndication;
}

bsl::ostream& EncryptionClientOptions::print(bsl::ostream& stream,
                                             int           level,
                                             int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("minMethod", d_options.minMethod());
    printer.printAttribute("maxMethod", d_options.maxMethod());
    printer.printAttribute("authentication", d_options.authentication());

    if (!d_options.authorityDirectory().isNull()) {
        printer.printAttribute("authorityDirectory",
                               d_options.authorityDirectory().value());
    }

    if (!d_options.cipherSpec().isNull()) {
        printer.printAttribute("cipherSpec", d_options.cipherSpec().value());
    }

    if (!d_options.resources().empty()) {
        printer.printAttribute("resource", d_options.resources());
    }

    printer.printAttribute("map", d_optionsMap);

    if (!d_serverNameIndication.isNull()) {
        printer.printAttribute("serverNameIndication", d_serverNameIndication);
    }

    printer.end();
    return stream;
}

bool operator==(const EncryptionClientOptions& lhs,
                const EncryptionClientOptions& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionClientOptions& lhs,
                const EncryptionClientOptions& rhs)
{
    return !operator==(lhs, rhs);
}

bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const EncryptionClientOptions& object)
{
    return object.print(stream, 0, -1);
}

}  // close package namespace
}  // close enterprise namespace
