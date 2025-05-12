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

#include <ntci_encryption.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_encryption_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntci {

Encryption::~Encryption()
{
}

ntsa::Error Encryption::initiateHandshake(const HandshakeCallback& callback)
{
    NTCCFG_WARNING_UNUSED(callback);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Encryption::initiateHandshake(
    const ntca::UpgradeOptions& upgradeOptions,
    const HandshakeCallback&    callback)
{
    NTCCFG_WARNING_UNUSED(upgradeOptions);
    NTCCFG_WARNING_UNUSED(callback);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Encryption::pushIncomingCipherText(const bdlbb::Blob& input)
{
    NTCCFG_WARNING_UNUSED(input);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Encryption::pushIncomingCipherText(const ntsa::Data& input)
{
    NTCCFG_WARNING_UNUSED(input);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Encryption::pushOutgoingPlainText(const bdlbb::Blob& input)
{
    NTCCFG_WARNING_UNUSED(input);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Encryption::pushOutgoingPlainText(const ntsa::Data& input)
{
    NTCCFG_WARNING_UNUSED(input);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Encryption::popIncomingPlainText(bdlbb::Blob* output)
{
    NTCCFG_WARNING_UNUSED(output);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Encryption::popOutgoingCipherText(bdlbb::Blob* output)
{
    NTCCFG_WARNING_UNUSED(output);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Encryption::popIncomingLeftovers(bdlbb::Blob* output)
{
    NTCCFG_WARNING_UNUSED(output);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Encryption::popOutgoingLeftovers(bdlbb::Blob* output)
{
    NTCCFG_WARNING_UNUSED(output);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Encryption::shutdown()
{
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bool Encryption::hasIncomingPlainText() const
{
    return false;
}

bool Encryption::hasOutgoingCipherText() const
{
    return false;
}

bool Encryption::hasIncomingLeftovers() const
{
    return false;
}

bool Encryption::hasOutgoingLeftovers() const
{
    return false;
}

bool Encryption::getSourceCertificate(
    ntca::EncryptionCertificate* result) const
{
    result->reset();
    return false;
}

bool Encryption::getSourceCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result) const
{
    result->reset();
    return false;
}

bool Encryption::getRemoteCertificate(
    ntca::EncryptionCertificate* result) const
{
    result->reset();
    return false;
}

bool Encryption::getRemoteCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result) const
{
    result->reset();
    return false;
}

bool Encryption::getCipher(bsl::string* result) const
{
    result->clear();
    return false;
}

bool Encryption::isHandshakeFinished() const
{
    return false;
}

bool Encryption::isShutdownSent() const
{
    return false;
}

bool Encryption::isShutdownReceived() const
{
    return false;
}

bool Encryption::isShutdownFinished() const
{
    return false;
}

bsl::shared_ptr<ntci::EncryptionCertificate> Encryption::sourceCertificate()
    const
{
    return bsl::shared_ptr<ntci::EncryptionCertificate>();
}

bsl::shared_ptr<ntci::EncryptionCertificate> Encryption::remoteCertificate()
    const
{
    return bsl::shared_ptr<ntci::EncryptionCertificate>();
}

bsl::shared_ptr<ntci::EncryptionKey> Encryption::privateKey() const
{
    return bsl::shared_ptr<ntci::EncryptionKey>();
}

ntsa::Error Encryption::serverNameIndication(bsl::string* result) const
{
    result->clear();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

}  // close package namespace
}  // close enterprise namespace
