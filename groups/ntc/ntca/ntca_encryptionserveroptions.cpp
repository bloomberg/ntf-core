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

#include <ntca_encryptionserveroptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptionserveroptions_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

EncryptionServerOptions::EncryptionServerOptions(
    bslma::Allocator* basicAllocator)
: d_options(basicAllocator)
, d_optionsMap(basicAllocator)
{
}

EncryptionServerOptions::EncryptionServerOptions(
    const EncryptionServerOptions& other,
    bslma::Allocator*              basicAllocator)
: d_options(other.d_options, basicAllocator)
, d_optionsMap(other.d_optionsMap, basicAllocator)
{
}

EncryptionServerOptions::~EncryptionServerOptions()
{
}

EncryptionServerOptions& EncryptionServerOptions::operator=(
    const EncryptionServerOptions& other)
{
    if (this != &other) {
        d_options    = other.d_options;
        d_optionsMap = other.d_optionsMap;
    }

    return *this;
}

void EncryptionServerOptions::reset()
{
    d_options.reset();
    d_optionsMap.clear();
}

void EncryptionServerOptions::setMinMethod(
    ntca::EncryptionMethod::Value minMethod)
{
    d_options.setMinMethod(minMethod);
}

void EncryptionServerOptions::setMaxMethod(
    ntca::EncryptionMethod::Value maxMethod)
{
    d_options.setMaxMethod(maxMethod);
}

void EncryptionServerOptions::setCipherSpec(const bsl::string& cipherSpec)
{
    d_options.setCipherSpec(cipherSpec);
}

void EncryptionServerOptions::setAuthentication(
    ntca::EncryptionAuthentication::Value authentication)
{
    d_options.setAuthentication(authentication);
}

void EncryptionServerOptions::setValidation(
    const ntca::EncryptionValidation& validation)
{
    d_options.setValidation(validation);
}

void EncryptionServerOptions::setAuthorityDirectory(
    const bsl::string& authorityDirectory)
{
    d_options.setAuthorityDirectory(authorityDirectory);
}

void EncryptionServerOptions::addAuthorityList(
    const ntca::EncryptionCertificateVector& certificates)
{
    d_options.addAuthorityList(certificates);
}

void EncryptionServerOptions::addAuthority(
    const ntca::EncryptionCertificate& certificate)
{
    d_options.addAuthority(certificate);
}

void EncryptionServerOptions::addAuthorityData(
    const bsl::vector<char>& resourceData)
{
    d_options.addAuthorityData(resourceData);
}

void EncryptionServerOptions::addAuthorityData(
    const bsl::vector<char>&               resourceData,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    d_options.addAuthorityData(resourceData, resourceOptions);
}

void EncryptionServerOptions::addAuthorityFile(const bsl::string& resourcePath)
{
    d_options.addAuthorityFile(resourcePath);
}

void EncryptionServerOptions::addAuthorityFile(
    const bsl::string&                     resourcePath,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    d_options.addAuthorityFile(resourcePath, resourceOptions);
}

void EncryptionServerOptions::setIdentity(
    const ntca::EncryptionCertificate& certificate)
{
    d_options.setIdentity(certificate);
}

void EncryptionServerOptions::setIdentityData(
    const bsl::vector<char>& resourceData)
{
    d_options.setIdentityData(resourceData);
}

void EncryptionServerOptions::setIdentityData(
    const bsl::vector<char>&               resourceData,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    d_options.setIdentityData(resourceData, resourceOptions);
}

void EncryptionServerOptions::setIdentityFile(const bsl::string& resourcePath)
{
    d_options.setIdentityFile(resourcePath);
}

void EncryptionServerOptions::setIdentityFile(
    const bsl::string&                     resourcePath,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    d_options.setIdentityFile(resourcePath, resourceOptions);
}

void EncryptionServerOptions::setPrivateKey(const ntca::EncryptionKey& key)
{
    d_options.setPrivateKey(key);
}

void EncryptionServerOptions::setPrivateKeyData(
    const bsl::vector<char>& resourceData)
{
    d_options.setPrivateKeyData(resourceData);
}

void EncryptionServerOptions::setPrivateKeyData(
    const bsl::vector<char>&               resourceData,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    d_options.setPrivateKeyData(resourceData, resourceOptions);
}

void EncryptionServerOptions::setPrivateKeyFile(
    const bsl::string& resourcePath)
{
    d_options.setPrivateKeyFile(resourcePath);
}

void EncryptionServerOptions::setPrivateKeyFile(
    const bsl::string&                     resourcePath,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    d_options.setPrivateKeyFile(resourcePath, resourceOptions);
}

void EncryptionServerOptions::addResource(
    const ntca::EncryptionResource& resource)
{
    d_options.addResource(resource);
}

void EncryptionServerOptions::addResourceData(
    const bsl::vector<char>& resourceData)
{
    d_options.addResourceData(resourceData);
}

void EncryptionServerOptions::addResourceData(
    const bsl::vector<char>&               resourceData,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    d_options.addResourceData(resourceData, resourceOptions);
}

void EncryptionServerOptions::addResourceFile(const bsl::string& resourcePath)
{
    d_options.addResourceFile(resourcePath);
}

void EncryptionServerOptions::addResourceFile(
    const bsl::string&                     resourcePath,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    d_options.addResourceFile(resourcePath, resourceOptions);
}

void EncryptionServerOptions::addOverrides(
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

ntca::EncryptionMethod::Value EncryptionServerOptions::minMethod() const
{
    return d_options.minMethod();
}

ntca::EncryptionMethod::Value EncryptionServerOptions::maxMethod() const
{
    return d_options.maxMethod();
}

const bdlb::NullableValue<bsl::string>& EncryptionServerOptions::cipherSpec()
    const
{
    return d_options.cipherSpec();
}

ntca::EncryptionAuthentication::Value EncryptionServerOptions::authentication()
    const
{
    return d_options.authentication();
}

const bdlb::NullableValue<ntca::EncryptionValidation>& EncryptionServerOptions::
    validation() const
{
    return d_options.validation();
}

const bdlb::NullableValue<bsl::string>& EncryptionServerOptions::
    authorityDirectory() const
{
    return d_options.authorityDirectory();
}

const ntca::EncryptionResourceVector& EncryptionServerOptions::resources()
    const
{
    return d_options.resources();
}

void EncryptionServerOptions::loadServerNameList(
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

bool EncryptionServerOptions::loadServerNameOptions(
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

bool EncryptionServerOptions::equals(
    const EncryptionServerOptions& other) const
{
    return d_options == other.d_options && d_optionsMap == other.d_optionsMap;
}

bsl::ostream& EncryptionServerOptions::print(bsl::ostream& stream,
                                             int           level,
                                             int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("minMethod", d_options.minMethod());
    printer.printAttribute("maxMethod", d_options.maxMethod());
    printer.printAttribute("authentication", d_options.authentication());

    if (!d_options.validation().isNull()) {
        printer.printAttribute("validation", d_options.validation());
    }

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

    printer.end();
    return stream;
}

bool operator==(const EncryptionServerOptions& lhs,
                const EncryptionServerOptions& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionServerOptions& lhs,
                const EncryptionServerOptions& rhs)
{
    return !operator==(lhs, rhs);
}

bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const EncryptionServerOptions& object)
{
    return object.print(stream, 0, -1);
}

}  // close package namespace
}  // close enterprise namespace
