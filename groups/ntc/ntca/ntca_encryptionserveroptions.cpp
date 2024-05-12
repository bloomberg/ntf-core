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

namespace {

bool sortResource(const ntca::EncryptionResource& lhs,
                  const ntca::EncryptionResource& rhs)
{
    ntca::EncryptionResourceOptions::Hint lhsHint =
        ntca::EncryptionResourceOptions::e_ANY;

    if (!lhs.options().isNull()) {
        lhsHint = lhs.options().value().hint();
    }

    ntca::EncryptionResourceOptions::Hint rhsHint =
        ntca::EncryptionResourceOptions::e_ANY;

    if (!rhs.options().isNull()) {
        rhsHint = rhs.options().value().hint();
    }

    return static_cast<int>(lhsHint) < static_cast<int>(rhsHint);
}

}  // close unnamed namespace

EncryptionServerOptions::EncryptionServerOptions(
    bslma::Allocator* basicAllocator)
: d_minMethod(ntca::EncryptionMethod::e_TLS_V1_X)
, d_maxMethod(ntca::EncryptionMethod::e_TLS_V1_X)
, d_authentication(ntca::EncryptionAuthentication::e_NONE)
, d_resourceVector(basicAllocator)
, d_authorityDirectory(basicAllocator)
, d_cipherSpec(basicAllocator)
{
}

EncryptionServerOptions::EncryptionServerOptions(
    const EncryptionServerOptions& other,
    bslma::Allocator*              basicAllocator)
: d_minMethod(other.d_minMethod)
, d_maxMethod(other.d_maxMethod)
, d_authentication(other.d_authentication)
, d_resourceVector(other.d_resourceVector, basicAllocator)
, d_authorityDirectory(other.d_authorityDirectory, basicAllocator)
, d_cipherSpec(other.d_cipherSpec, basicAllocator)
{
}

EncryptionServerOptions::~EncryptionServerOptions()
{
}

EncryptionServerOptions& EncryptionServerOptions::operator=(
    const EncryptionServerOptions& other)
{
    if (this != &other) {
        d_minMethod          = other.d_minMethod;
        d_maxMethod          = other.d_maxMethod;
        d_authentication     = other.d_authentication;
        d_resourceVector     = other.d_resourceVector;
        d_authorityDirectory = other.d_authorityDirectory;
        d_cipherSpec         = other.d_cipherSpec;
    }

    return *this;
}

void EncryptionServerOptions::reset()
{
    d_minMethod      = ntca::EncryptionMethod::e_TLS_V1_X;
    d_maxMethod      = ntca::EncryptionMethod::e_TLS_V1_X;
    d_authentication = ntca::EncryptionAuthentication::e_NONE;
    d_resourceVector.clear();
    d_authorityDirectory.reset();
    d_cipherSpec.reset();
}

void EncryptionServerOptions::setMinMethod(
    ntca::EncryptionMethod::Value minMethod)
{
    d_minMethod = minMethod;
}

void EncryptionServerOptions::setMaxMethod(
    ntca::EncryptionMethod::Value maxMethod)
{
    d_maxMethod = maxMethod;
}

void EncryptionServerOptions::setCipherSpec(const bsl::string& cipherSpec)
{
    d_cipherSpec = cipherSpec;
}

void EncryptionServerOptions::setAuthentication(
    ntca::EncryptionAuthentication::Value authentication)
{
    d_authentication = authentication;
}

void EncryptionServerOptions::setAuthorityDirectory(
    const bsl::string& authorityDirectory)
{
    d_authorityDirectory = authorityDirectory;
}

void EncryptionServerOptions::addAuthorityList(
    const ntca::EncryptionCertificateVector& certificates)
{
    for (bsl::size_t i = 0; i < certificates.size(); ++i) {
        this->addAuthority(certificates[i]);
    }
}

void EncryptionServerOptions::addAuthority(
    const ntca::EncryptionCertificate& certificate)
{
    ntca::EncryptionResourceOptions effectiveResourceOptions;
    effectiveResourceOptions.setHint(
        ntca::EncryptionResourceOptions::e_CERTIFICATE_AUTHORITY);

    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makeCertificate(certificate);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(effectiveResourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::addAuthorityData(
    const bsl::vector<char>& resourceData)
{
    ntca::EncryptionResourceOptions effectiveResourceOptions;
    effectiveResourceOptions.setHint(
        ntca::EncryptionResourceOptions::e_CERTIFICATE_AUTHORITY);

    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makeData(resourceData);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(effectiveResourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::addAuthorityData(
    const bsl::vector<char>&               resourceData,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    ntca::EncryptionResourceOptions effectiveResourceOptions = resourceOptions;
    effectiveResourceOptions.setHint(
        ntca::EncryptionResourceOptions::e_CERTIFICATE_AUTHORITY);

    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makeData(resourceData);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(effectiveResourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::addAuthorityFile(const bsl::string& resourcePath)
{
    ntca::EncryptionResourceOptions effectiveResourceOptions;
    effectiveResourceOptions.setHint(
        ntca::EncryptionResourceOptions::e_CERTIFICATE_AUTHORITY);

    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makePath(resourcePath);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(effectiveResourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::addAuthorityFile(
    const bsl::string&                     resourcePath,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    ntca::EncryptionResourceOptions effectiveResourceOptions = resourceOptions;
    effectiveResourceOptions.setHint(
        ntca::EncryptionResourceOptions::e_CERTIFICATE_AUTHORITY);

    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makePath(resourcePath);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(effectiveResourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::setIdentity(
    const ntca::EncryptionCertificate& certificate)
{
    ntca::EncryptionResourceOptions effectiveResourceOptions;
    effectiveResourceOptions.setHint(
        ntca::EncryptionResourceOptions::e_CERTIFICATE);

    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makeCertificate(certificate);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(effectiveResourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::setIdentityData(
    const bsl::vector<char>& resourceData)
{
    ntca::EncryptionResourceOptions effectiveResourceOptions;
    effectiveResourceOptions.setHint(
        ntca::EncryptionResourceOptions::e_CERTIFICATE);

    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makeData(resourceData);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(effectiveResourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::setIdentityData(
    const bsl::vector<char>&               resourceData,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    ntca::EncryptionResourceOptions effectiveResourceOptions = resourceOptions;
    effectiveResourceOptions.setHint(
        ntca::EncryptionResourceOptions::e_CERTIFICATE);

    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makeData(resourceData);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(effectiveResourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::setIdentityFile(const bsl::string& resourcePath)
{
    ntca::EncryptionResourceOptions effectiveResourceOptions;
    effectiveResourceOptions.setHint(
        ntca::EncryptionResourceOptions::e_CERTIFICATE);

    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makePath(resourcePath);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(effectiveResourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::setIdentityFile(
    const bsl::string&                     resourcePath,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    ntca::EncryptionResourceOptions effectiveResourceOptions = resourceOptions;
    effectiveResourceOptions.setHint(
        ntca::EncryptionResourceOptions::e_CERTIFICATE);

    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makePath(resourcePath);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(effectiveResourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::setPrivateKey(const ntca::EncryptionKey& key)
{
    ntca::EncryptionResourceOptions effectiveResourceOptions;
    effectiveResourceOptions.setHint(
        ntca::EncryptionResourceOptions::e_PRIVATE_KEY);

    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makeKey(key);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(effectiveResourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::setPrivateKeyData(
    const bsl::vector<char>& resourceData)
{
    ntca::EncryptionResourceOptions effectiveResourceOptions;
    effectiveResourceOptions.setHint(
        ntca::EncryptionResourceOptions::e_PRIVATE_KEY);

    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makeData(resourceData);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(effectiveResourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::setPrivateKeyData(
    const bsl::vector<char>&               resourceData,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    ntca::EncryptionResourceOptions effectiveResourceOptions = resourceOptions;
    effectiveResourceOptions.setHint(
        ntca::EncryptionResourceOptions::e_PRIVATE_KEY);

    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makeData(resourceData);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(effectiveResourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::setPrivateKeyFile(
    const bsl::string& resourcePath)
{
    ntca::EncryptionResourceOptions effectiveResourceOptions;
    effectiveResourceOptions.setHint(
        ntca::EncryptionResourceOptions::e_PRIVATE_KEY);

    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makePath(resourcePath);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(effectiveResourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::setPrivateKeyFile(
    const bsl::string&                     resourcePath,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    ntca::EncryptionResourceOptions effectiveResourceOptions = resourceOptions;
    effectiveResourceOptions.setHint(
        ntca::EncryptionResourceOptions::e_PRIVATE_KEY);

    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makePath(resourcePath);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(effectiveResourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::addResource(
    const ntca::EncryptionResource& resource)
{
    d_resourceVector.push_back(resource);

    bsl::sort(d_resourceVector.begin(), d_resourceVector.end(), &sortResource);
}

void EncryptionServerOptions::addResourceData(
    const bsl::vector<char>& resourceData)
{
    ntca::EncryptionResourceOptions resourceOptions;

    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makeData(resourceData);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(resourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::addResourceData(
    const bsl::vector<char>&               resourceData,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makeData(resourceData);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(resourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::addResourceFile(const bsl::string& resourcePath)
{
    ntca::EncryptionResourceOptions resourceOptions;

    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makePath(resourcePath);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(resourceOptions);

    this->addResource(resource);
}

void EncryptionServerOptions::addResourceFile(
    const bsl::string&                     resourcePath,
    const ntca::EncryptionResourceOptions& resourceOptions)
{
    ntca::EncryptionResourceDescriptor resourceDescriptor;
    resourceDescriptor.makePath(resourcePath);

    ntca::EncryptionResource resource;
    resource.setDescriptor(resourceDescriptor);
    resource.setOptions(resourceOptions);

    this->addResource(resource);
}

ntca::EncryptionMethod::Value EncryptionServerOptions::minMethod() const
{
    return d_minMethod;
}

ntca::EncryptionMethod::Value EncryptionServerOptions::maxMethod() const
{
    return d_maxMethod;
}

const bdlb::NullableValue<bsl::string>& EncryptionServerOptions::cipherSpec()
    const
{
    return d_cipherSpec;
}

ntca::EncryptionAuthentication::Value EncryptionServerOptions::authentication()
    const
{
    return d_authentication;
}

const bdlb::NullableValue<bsl::string>& EncryptionServerOptions::
    authorityDirectory() const
{
    return d_authorityDirectory;
}

const ntca::EncryptionResourceVector& EncryptionServerOptions::resources()
    const
{
    return d_resourceVector;
}

bsl::ostream& EncryptionServerOptions::print(bsl::ostream& stream,
                                             int           level,
                                             int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("minMethod", d_minMethod);
    printer.printAttribute("maxMethod", d_maxMethod);
    printer.printAttribute("authentication", d_authentication);

    if (!d_resourceVector.empty()) {
        printer.printAttribute("resource", d_resourceVector);
    }

    if (!d_authorityDirectory.isNull()) {
        printer.printAttribute("authorityDirectory",
                               d_authorityDirectory.value());
    }

    if (!d_cipherSpec.isNull()) {
        printer.printAttribute("cipherSpec", d_cipherSpec.value());
    }

    printer.end();
    return stream;
}

bool operator==(const EncryptionServerOptions& lhs,
                const EncryptionServerOptions& rhs)
{
    return lhs.minMethod() == rhs.minMethod() &&
           lhs.maxMethod() == rhs.maxMethod() &&
           lhs.authentication() == rhs.authentication() &&
           lhs.resources() == rhs.resources() &&
           lhs.authorityDirectory() == rhs.authorityDirectory() &&
           lhs.cipherSpec() == rhs.cipherSpec();
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
