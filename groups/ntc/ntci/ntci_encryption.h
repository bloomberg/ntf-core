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

#ifndef INCLUDED_NTCI_ENCRYPTION
#define INCLUDED_NTCI_ENCRYPTION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_encryptionauthentication.h>
#include <ntca_encryptionmethod.h>
#include <ntca_encryptionrole.h>
#include <ntca_upgradeoptions.h>
#include <ntccfg_platform.h>
#include <ntci_encryptioncertificate.h>
#include <ntci_encryptionkey.h>
#include <ntcscm_version.h>
#include <ntsa_buffer.h>
#include <ntsa_data.h>
#include <ntsa_error.h>
#include <bdlbb_blob.h>
#include <bsl_functional.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to an encryption/decryption session.
///
/// @details
/// This class provides a mechanism to cryptographically encrypt and decrypt
/// a data stream according to the Transport Layer Security (TLS) protocol. In
/// addition, this component provides enumerations, 'ntca::EncryptionMethod',
/// 'ntca::EncryptionAuthentication', and 'ntca::EncryptionRole', to
/// enumerate the TLS protocol versions, roles, and peer verification styles,
/// respectively.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_encryption
class Encryption
{
  public:
    /// Defines a type alias for a function invoked when the initiation of
    /// a handshake is complete. The first parameter indicates the error,
    /// if any. The second parameter indicates the certificate of the peer,
    /// if any. The third parameters indicates the cause of the handshake
    /// error, if an error is indicated.
    typedef NTCCFG_FUNCTION(
        const ntsa::Error&                                  error,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
        const bsl::string& details) HandshakeCallback;

    /// Destroy this object.
    virtual ~Encryption();

    /// Initiate the handshake to begin the session. Invoke the specified
    /// 'callback' when the handshake completes. Return the error.
    virtual ntsa::Error initiateHandshake(
        const HandshakeCallback& callback);

    /// Initiate the handshake to begin the session according to the specified
    /// 'upgradeOptions'. Invoke the specified 'callback' when the handshake 
    /// completes. Return the error.
    virtual ntsa::Error initiateHandshake(
        const ntca::UpgradeOptions& upgradeOptions,
        const HandshakeCallback&    callback);

    /// Add the specified 'input' containing ciphertext read from the peer.
    /// Return 0 on success and a non-zero value otherwise.
    virtual ntsa::Error pushIncomingCipherText(const bdlbb::Blob& input);

    /// Add the specified 'input' containing ciphertext read from the peer.
    /// Return 0 on success and a non-zero value otherwise.
    virtual ntsa::Error pushIncomingCipherText(const ntsa::Data& input);

    /// Add the specified 'input' containing plaintext to be sent to the
    /// peer. Return 0 on success and a non-zero value otherwise.
    virtual ntsa::Error pushOutgoingPlainText(const bdlbb::Blob& input);

    /// Add the specified 'input' containing plaintext to be sent to the
    /// peer. Return 0 on success and a non-zero value otherwise.
    virtual ntsa::Error pushOutgoingPlainText(const ntsa::Data& input);

    /// Pop plaintext read from the peer and append it to the specified
    /// 'output'. Return 0 on success and a non-zero value otherwise.
    virtual ntsa::Error popIncomingPlainText(bdlbb::Blob* output);

    /// Pop ciphertext to be read from the peer and append to the specified
    /// 'output'.
    virtual ntsa::Error popOutgoingCipherText(bdlbb::Blob* output);

    /// Initiate the shutdown of the session.
    virtual ntsa::Error shutdown();

    /// Return true if plaintext data is ready to be read.
    virtual bool hasIncomingPlainText() const;

    /// Return true if ciphertext data is ready to be sent.
    virtual bool hasOutgoingCipherText() const;

    /// Load into the specified 'result' the source certificate used by the
    /// encryption session. Return true if such a certicate is defined, and
    /// false otherwise.
    virtual bool getSourceCertificate(
        ntca::EncryptionCertificate* result) const;

    /// Load into the specified 'result' the source certificate used by the
    /// encryption session. Return true if such a certicate is defined, and
    /// false otherwise.
    virtual bool getSourceCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result) const;

    /// Load into the specified 'result' the remote certificate used by the
    /// encryption session. Return true if such a certicate is defined, and
    /// false otherwise.
    virtual bool getRemoteCertificate(
        ntca::EncryptionCertificate* result) const;

    /// Load into the specified 'result' the remote certificate used by the
    /// encryption session. Return true if such a certicate is defined, and
    /// false otherwise.
    virtual bool getRemoteCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result) const;

    /// Load into the specified 'result' the cipher used to encrypt data
    /// passing through the filter. Return true if such a cipher has been
    /// negotiated, and false otherwise.
    virtual bool getCipher(bsl::string* result) const;

    /// Return true if the handshake is finished, otherwise return false.
    virtual bool isHandshakeFinished() const;

    /// Return true if the shutdown has been sent, otherwise return false.
    virtual bool isShutdownSent() const;

    /// Return true if the shutdown has been received, otherwise return
    /// false.
    virtual bool isShutdownReceived() const;

    /// Return true if the shutdown is finished, otherwise return false.
    virtual bool isShutdownFinished() const;

    /// Return the source certificate used by the encryption session, if
    /// any.
    virtual bsl::shared_ptr<ntci::EncryptionCertificate> sourceCertificate()
        const;

    /// Return the remote certificate used by the encryption session, if
    /// any.
    virtual bsl::shared_ptr<ntci::EncryptionCertificate> remoteCertificate()
        const;

    /// Return the private key used by the encryption session, if any.
    virtual bsl::shared_ptr<ntci::EncryptionKey> privateKey() const;

    /// Load into the specified 'result' the server name indication, if any.
    /// Return the error, for example, when no server name indication is 
    /// explicitly requested or accepted. 
    virtual ntsa::Error serverNameIndication(bsl::string* result) const;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
