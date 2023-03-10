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
: d_minMethod(ntca::EncryptionMethod::e_TLS_V1_X)
, d_maxMethod(ntca::EncryptionMethod::e_TLS_V1_X)
, d_authentication(ntca::EncryptionAuthentication::e_NONE)
, d_identityData(basicAllocator)
, d_identityFile(basicAllocator)
, d_privateKeyData(basicAllocator)
, d_privateKeyFile(basicAllocator)
, d_authorityDataList(basicAllocator)
, d_authorityDirectory(basicAllocator)
, d_cipherSpec(basicAllocator)
{
}

EncryptionClientOptions::EncryptionClientOptions(
    const EncryptionClientOptions& other,
    bslma::Allocator*              basicAllocator)
: d_minMethod(other.d_minMethod)
, d_maxMethod(other.d_maxMethod)
, d_authentication(other.d_authentication)
, d_identityData(other.d_identityData, basicAllocator)
, d_identityFile(other.d_identityFile, basicAllocator)
, d_privateKeyData(other.d_privateKeyData, basicAllocator)
, d_privateKeyFile(other.d_privateKeyFile, basicAllocator)
, d_authorityDataList(other.d_authorityDataList, basicAllocator)
, d_authorityDirectory(other.d_authorityDirectory, basicAllocator)
, d_cipherSpec(other.d_cipherSpec, basicAllocator)
{
}

EncryptionClientOptions::~EncryptionClientOptions()
{
}

EncryptionClientOptions& EncryptionClientOptions::operator=(
    const EncryptionClientOptions& other)
{
    if (this != &other) {
        d_minMethod          = other.d_minMethod;
        d_maxMethod          = other.d_maxMethod;
        d_authentication     = other.d_authentication;
        d_identityData       = other.d_identityData;
        d_identityFile       = other.d_identityFile;
        d_privateKeyData     = other.d_privateKeyData;
        d_privateKeyFile     = other.d_privateKeyFile;
        d_authorityDataList  = other.d_authorityDataList;
        d_authorityDirectory = other.d_authorityDirectory;
        d_cipherSpec         = other.d_cipherSpec;
    }

    return *this;
}

void EncryptionClientOptions::setMinMethod(
    ntca::EncryptionMethod::Value minMethod)
{
    d_minMethod = minMethod;
}

void EncryptionClientOptions::setMaxMethod(
    ntca::EncryptionMethod::Value maxMethod)
{
    d_maxMethod = maxMethod;
}

void EncryptionClientOptions::setAuthentication(
    ntca::EncryptionAuthentication::Value authentication)
{
    d_authentication = authentication;
}

void EncryptionClientOptions::setIdentityData(
    const bsl::vector<char>& identityData)
{
    d_identityData = identityData;
}

void EncryptionClientOptions::setIdentityFile(
    const bslstl::StringRef& identityFile)
{
    d_identityFile = identityFile;
}

void EncryptionClientOptions::setPrivateKeyData(
    const bsl::vector<char>& privateKeyData)
{
    d_privateKeyData = privateKeyData;
}

void EncryptionClientOptions::setPrivateKeyFile(
    const bslstl::StringRef& privateKeyFile)
{
    d_privateKeyFile = privateKeyFile;
}

void EncryptionClientOptions::addAuthorityData(
    const bsl::vector<char>& authorityData)
{
    d_authorityDataList.push_back(authorityData);
}

void EncryptionClientOptions::setAuthorityDirectory(
    const bslstl::StringRef& authorityDirectory)
{
    d_authorityDirectory = authorityDirectory;
}

void EncryptionClientOptions::setCipherSpec(
    const bslstl::StringRef& cipherSpec)
{
    d_cipherSpec = cipherSpec;
}

ntca::EncryptionMethod::Value EncryptionClientOptions::minMethod() const
{
    return d_minMethod;
}

ntca::EncryptionMethod::Value EncryptionClientOptions::maxMethod() const
{
    return d_maxMethod;
}

ntca::EncryptionAuthentication::Value EncryptionClientOptions::authentication()
    const
{
    return d_authentication;
}

const bdlb::NullableValue<bsl::vector<char> >& EncryptionClientOptions::
    identityData() const
{
    return d_identityData;
}

const bdlb::NullableValue<bsl::string>& EncryptionClientOptions::identityFile()
    const
{
    return d_identityFile;
}

const bdlb::NullableValue<bsl::vector<char> >& EncryptionClientOptions::
    privateKeyData() const
{
    return d_privateKeyData;
}

const bdlb::NullableValue<bsl::string>& EncryptionClientOptions::
    privateKeyFile() const
{
    return d_privateKeyFile;
}

const EncryptionClientOptions::CertificateDataList& EncryptionClientOptions::
    authorityDataList() const
{
    return d_authorityDataList;
}

const bdlb::NullableValue<bsl::string>& EncryptionClientOptions::
    authorityDirectory() const
{
    return d_authorityDirectory;
}

const bdlb::NullableValue<bsl::string>& EncryptionClientOptions::cipherSpec()
    const
{
    return d_cipherSpec;
}

bsl::ostream& EncryptionClientOptions::print(bsl::ostream& stream,
                                             int           level,
                                             int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("minMethod", d_minMethod);
    printer.printAttribute("maxMethod", d_maxMethod);
    printer.printAttribute("authentication", d_authentication);

    if (!d_identityFile.isNull()) {
        printer.printAttribute("identityFile", d_identityFile.value());
    }

    if (!d_privateKeyFile.isNull()) {
        printer.printAttribute("privateKeyFile", d_privateKeyFile.value());
    }

    if (!d_authorityDataList.empty()) {
        printer.printAttribute("authorityDataList",
                               d_authorityDataList.size());
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

bool operator==(const EncryptionClientOptions& lhs,
                const EncryptionClientOptions& rhs)
{
    return lhs.minMethod() == rhs.minMethod() &&
           lhs.maxMethod() == rhs.maxMethod() &&
           lhs.authentication() == rhs.authentication() &&
           lhs.identityData() == rhs.identityData() &&
           lhs.identityFile() == rhs.identityFile() &&
           lhs.privateKeyData() == rhs.privateKeyData() &&
           lhs.privateKeyFile() == rhs.privateKeyFile() &&
           lhs.authorityDataList() == rhs.authorityDataList() &&
           lhs.authorityDirectory() == rhs.authorityDirectory() &&
           lhs.cipherSpec() == rhs.cipherSpec();
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
