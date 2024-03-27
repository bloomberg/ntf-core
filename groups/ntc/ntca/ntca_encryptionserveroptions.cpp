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
: d_minMethod(ntca::EncryptionMethod::e_TLS_V1_X)
, d_maxMethod(ntca::EncryptionMethod::e_TLS_V1_X)
, d_authentication(ntca::EncryptionAuthentication::e_NONE)
, d_identity(basicAllocator)
, d_privateKey(basicAllocator)
, d_authorityList(basicAllocator)
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
, d_identity(other.d_identity, basicAllocator)
, d_privateKey(other.d_privateKey, basicAllocator)
, d_authorityList(other.d_authorityList, basicAllocator)
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
        d_identity           = other.d_identity;
        d_privateKey         = other.d_privateKey;
        d_authorityList      = other.d_authorityList;
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
    d_identity.reset();
    d_privateKey.reset();
    d_authorityList.clear();
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

void EncryptionServerOptions::setAuthentication(
    ntca::EncryptionAuthentication::Value authentication)
{
    d_authentication = authentication;
}

void EncryptionServerOptions::setIdentityData(
    const bsl::vector<char>& identityData)
{
    d_identity.reset();
    d_identity.makeValue();

    ntca::EncryptionCertificateStorageDescriptor identityDescriptor;
    identityDescriptor.makeData(identityData);

    d_identity.value().setDescriptor(identityDescriptor);
}

void EncryptionServerOptions::setIdentityData(
    const bsl::vector<char>&                         identityData,
    const ntca::EncryptionCertificateStorageOptions& identityOptions)
{
    d_identity.reset();
    d_identity.makeValue();

    ntca::EncryptionCertificateStorageDescriptor identityDescriptor;
    identityDescriptor.makeData(identityData);

    d_identity.value().setDescriptor(identityDescriptor);
    d_identity.value().setOptions(identityOptions);
}

void EncryptionServerOptions::setIdentityFile(const bsl::string& identityFile)
{
    d_identity.reset();
    d_identity.makeValue();

    ntca::EncryptionCertificateStorageDescriptor identityDescriptor;
    identityDescriptor.makePath(identityFile);

    d_identity.value().setDescriptor(identityDescriptor);
}

void EncryptionServerOptions::setIdentityFile(
    const bsl::string&                               identityFile,
    const ntca::EncryptionCertificateStorageOptions& identityOptions)
{
    d_identity.reset();
    d_identity.makeValue();

    ntca::EncryptionCertificateStorageDescriptor identityDescriptor;
    identityDescriptor.makePath(identityFile);

    d_identity.value().setDescriptor(identityDescriptor);
    d_identity.value().setOptions(identityOptions);
}

void EncryptionServerOptions::setPrivateKeyData(
    const bsl::vector<char>& privateKeyData)
{
    d_privateKey.reset();
    d_privateKey.makeValue();

    ntca::EncryptionKeyStorageDescriptor privateKeyDescriptor;
    privateKeyDescriptor.makeData(privateKeyData);

    d_privateKey.value().setDescriptor(privateKeyDescriptor);
}

void EncryptionServerOptions::setPrivateKeyData(
    const bsl::vector<char>&                 privateKeyData,
    const ntca::EncryptionKeyStorageOptions& privateKeyOptions)
{
    d_privateKey.reset();
    d_privateKey.makeValue();

    ntca::EncryptionKeyStorageDescriptor privateKeyDescriptor;
    privateKeyDescriptor.makeData(privateKeyData);

    d_privateKey.value().setDescriptor(privateKeyDescriptor);
    d_privateKey.value().setOptions(privateKeyOptions);
}

void EncryptionServerOptions::setPrivateKeyFile(
    const bsl::string& privateKeyFile)
{
    d_privateKey.reset();
    d_privateKey.makeValue();

    ntca::EncryptionKeyStorageDescriptor privateKeyDescriptor;
    privateKeyDescriptor.makePath(privateKeyFile);

    d_privateKey.value().setDescriptor(privateKeyDescriptor);
}

void EncryptionServerOptions::setPrivateKeyFile(
    const bsl::string&                       privateKeyFile,
    const ntca::EncryptionKeyStorageOptions& privateKeyOptions)
{
    d_privateKey.reset();
    d_privateKey.makeValue();

    ntca::EncryptionKeyStorageDescriptor privateKeyDescriptor;
    privateKeyDescriptor.makePath(privateKeyFile);

    d_privateKey.value().setDescriptor(privateKeyDescriptor);
    d_privateKey.value().setOptions(privateKeyOptions);
}

void EncryptionServerOptions::addAuthorityData(
    const bsl::vector<char>& authorityData)
{
    ntca::EncryptionCertificateStorageDescriptor authorityDescriptor;
    authorityDescriptor.makeData(authorityData);

    ntca::EncryptionCertificateStorageData authority;
    authority.setDescriptor(authorityDescriptor);

    d_authorityList.push_back(authority);
}

void EncryptionServerOptions::addAuthorityData(
    const bsl::vector<char>&                         authorityData,
    const ntca::EncryptionCertificateStorageOptions& authorityOptions)
{
    ntca::EncryptionCertificateStorageDescriptor authorityDescriptor;
    authorityDescriptor.makeData(authorityData);

    ntca::EncryptionCertificateStorageData authority;
    authority.setDescriptor(authorityDescriptor);
    authority.setOptions(authorityOptions);

    d_authorityList.push_back(authority);
}

void EncryptionServerOptions::addAuthorityFile(
    const bsl::string& authorityFile)
{
    ntca::EncryptionCertificateStorageDescriptor authorityDescriptor;
    authorityDescriptor.makePath(authorityFile);

    ntca::EncryptionCertificateStorageData authority;
    authority.setDescriptor(authorityDescriptor);

    d_authorityList.push_back(authority);
}

void EncryptionServerOptions::addAuthorityFile(
    const bsl::string&                               authorityFile,
    const ntca::EncryptionCertificateStorageOptions& authorityOptions)
{
    ntca::EncryptionCertificateStorageDescriptor authorityDescriptor;
    authorityDescriptor.makePath(authorityFile);

    ntca::EncryptionCertificateStorageData authority;
    authority.setDescriptor(authorityDescriptor);
    authority.setOptions(authorityOptions);

    d_authorityList.push_back(authority);
}

void EncryptionServerOptions::setAuthorityDirectory(
    const bsl::string& authorityDirectory)
{
    d_authorityDirectory = authorityDirectory;
}

void EncryptionServerOptions::setCipherSpec(const bsl::string& cipherSpec)
{
    d_cipherSpec = cipherSpec;
}

ntca::EncryptionMethod::Value EncryptionServerOptions::minMethod() const
{
    return d_minMethod;
}

ntca::EncryptionMethod::Value EncryptionServerOptions::maxMethod() const
{
    return d_maxMethod;
}

ntca::EncryptionAuthentication::Value EncryptionServerOptions::authentication()
    const
{
    return d_authentication;
}

const bdlb::NullableValue<ntca::EncryptionCertificateStorageData>&
EncryptionServerOptions::identity() const
{
    return d_identity;
}

const bdlb::NullableValue<ntca::EncryptionKeyStorageData>&
EncryptionServerOptions::privateKey() const
{
    return d_privateKey;
}

const bsl::vector<ntca::EncryptionCertificateStorageData>&
EncryptionServerOptions::authorityList() const
{
    return d_authorityList;
}

const bdlb::NullableValue<bsl::string>& EncryptionServerOptions::
    authorityDirectory() const
{
    return d_authorityDirectory;
}

const bdlb::NullableValue<bsl::string>& EncryptionServerOptions::cipherSpec()
    const
{
    return d_cipherSpec;
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

    if (!d_identity.isNull()) {
        printer.printAttribute("identity", d_identity.value());
    }

    if (!d_privateKey.isNull()) {
        printer.printAttribute("privateKey", d_privateKey.value());
    }

    if (!d_authorityList.empty()) {
        printer.printAttribute("authorityList", d_authorityList);
    }

    if (!d_authorityDirectory.isNull()) {
        printer.printAttribute("authorityDirectoryList",
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
           lhs.identity() == rhs.identity() &&
           lhs.privateKey() == rhs.privateKey() &&
           lhs.authorityList() == rhs.authorityList() &&
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
