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

#ifndef INCLUDED_NTCI_ENCRYPTIONRESOURCE
#define INCLUDED_NTCI_ENCRYPTIONRESOURCE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_encryptionresourceoptions.h>
#include <ntca_encryptionresourcetype.h>
#include <ntccfg_platform.h>
#include <ntci_encryptioncertificate.h>
#include <ntci_encryptionkey.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bdlbb_blob.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to a storage of private keys and certificates as used
/// in public key cryptography.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionResource
{
  public:
    /// Destroy this object.
    virtual ~EncryptionResource();

    /// Set the private key to the specified 'key'. Return the error.
    virtual ntsa::Error setPrivateKey(
        const bsl::shared_ptr<ntci::EncryptionKey>& key);

    /// Set the user's certificate to the specified 'certificate'. Return the
    /// error.
    virtual ntsa::Error setCertificate(
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate);

    /// Add the specified 'certificate' to the list of trusted certificates.
    /// Return the error.
    virtual ntsa::Error addCertificateAuthority(
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate);

    /// Decode the resource in PEM format from the specified 'source'. Return
    /// the error.
    virtual ntsa::Error decode(bsl::streambuf* source);

    /// Decode the resource according to the specified 'options' from the
    /// specified 'source'. Return the error.
    virtual ntsa::Error decode(bsl::streambuf*                        source,
                               const ntca::EncryptionResourceOptions& options);

    /// Decode the resource in PEM format from the specified 'source'. Return
    /// the error.
    virtual ntsa::Error decode(const bdlbb::Blob& source);

    /// Decode the resource according to the specified 'options' from the
    /// specified 'source'. Return the error.
    virtual ntsa::Error decode(const bdlbb::Blob&                     source,
                               const ntca::EncryptionResourceOptions& options);

    /// Decode the resource in PEM format from the specified 'source'. Return
    /// the error.
    virtual ntsa::Error decode(const bsl::string& source);

    /// Decode the resource according to the specified 'options' from the
    /// specified 'source'. Return the error.
    virtual ntsa::Error decode(const bsl::string&                     source,
                               const ntca::EncryptionResourceOptions& options);

    /// Decode the resource in PEM format from the specified 'source'. Return
    /// the error.
    virtual ntsa::Error decode(const bsl::vector<char>& source);

    /// Decode the resource according to the specified 'options' from the
    /// specified 'source'. Return the error.
    virtual ntsa::Error decode(const bsl::vector<char>&               source,
                               const ntca::EncryptionResourceOptions& options);

    /// Encode the resource in PEM format to the specified 'destination'.
    /// Return the error.
    virtual ntsa::Error encode(bsl::streambuf* destination) const;

    /// Encode the resource according to the specified 'options' to the
    /// specified 'destination'. Return the error.
    virtual ntsa::Error encode(
        bsl::streambuf*                        destination,
        const ntca::EncryptionResourceOptions& options) const;

    /// Encode the resource in PEM format to the specified 'destination'.
    /// Return the error.
    virtual ntsa::Error encode(bdlbb::Blob* destination) const;

    /// Encode the resource according to the specified 'options' to the
    /// specified 'destination'. Return the error.
    virtual ntsa::Error encode(
        bdlbb::Blob*                           destination,
        const ntca::EncryptionResourceOptions& options) const;

    /// Encode the resource in PEM format to the specified 'destination'.
    /// Return the error.
    virtual ntsa::Error encode(bsl::string* destination) const;

    /// Encode the resource according to the specified 'options' to the
    /// specified 'destination'. Return the error.
    virtual ntsa::Error encode(
        bsl::string*                           destination,
        const ntca::EncryptionResourceOptions& options) const;

    /// Encode the resource in PEM format to the specified 'destination'.
    /// Return the error.
    virtual ntsa::Error encode(bsl::vector<char>* destination) const;

    /// Encode the resource according to the specified 'options' to the
    /// specified 'destination'. Return the error.
    virtual ntsa::Error encode(
        bsl::vector<char>*                     destination,
        const ntca::EncryptionResourceOptions& options) const;

    /// Load into the specified 'result' the private key stored in the
    /// resource. Return the error.
    virtual ntsa::Error getPrivateKey(
        bsl::shared_ptr<ntci::EncryptionKey>* result) const;

    /// Load into the specified 'result' the user's certificate stored in the
    /// resource. Return the error.
    virtual ntsa::Error getCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result) const;

    /// Load into the specified 'result' the user's certificate stored in the
    /// resource. Return the error.
    virtual ntsa::Error getCertificateAuthoritySet(
        bsl::vector<bsl::shared_ptr<ntci::EncryptionCertificate> >* result)
        const;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
