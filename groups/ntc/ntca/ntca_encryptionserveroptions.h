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

#ifndef INCLUDED_NTCA_ENCRYPTIONSERVEROPTIONS
#define INCLUDED_NTCA_ENCRYPTIONSERVEROPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_encryptionauthentication.h>
#include <ntca_encryptioncertificatestoragedata.h>
#include <ntca_encryptioncertificatestoragedescriptor.h>
#include <ntca_encryptioncertificatestorageoptions.h>
#include <ntca_encryptionkeystoragedata.h>
#include <ntca_encryptionkeystoragedescriptor.h>
#include <ntca_encryptionkeystorageoptions.h>
#include <ntca_encryptionmethod.h>
#include <ntca_encryptionrole.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bdlbb_blob.h>
#include <bslma_usesbslmaallocator.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntca {

/// Describe the configuration of encryption in the server role.
///
/// @details
/// This class describes the configuration of a 'ntca::Encryptor' operating in
/// the server role. Such 'ntca::Encryption' interfaces are used to actively
/// initiate a cryptographically secure session of communication according to
/// the Transport Layer Security (TLS) protocol, within which data is
/// transformed from from cleartext to ciphertext.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b minMethod:
/// The minimum version of the TLS protocol acceptable for use.
///
/// @li @b maxMethod:
/// The maximum version of the TLS protocol acceptable for use.
///
/// @li @b authentication:
/// Flag that determines whether the peer's certificate is verified as signed
/// by a trusted issuer.
///
/// @li @b identity:
/// The certificate including the server's identity, public key, issuer, etc.
///
/// @li @b privateKey:
/// The private key of the server.
///
/// @li @b authorityList:
/// The list of trusted certificate authorities.
///
/// @li @b authorityDirectory:
/// The directory containing files of PEM-encoded certificates for each trusted
/// certificate authority.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionServerOptions
{
  public:
    /// Define a type alias for a vector of certificate data.
    typedef bsl::vector<ntca::EncryptionCertificateStorageData>
        CertificateStorageDataVector;

    /// Define a type alias for a vector of key data.
    typedef bsl::vector<ntca::EncryptionKeyStorageData> KeyStorageDataVector;

  private:
    /// Define a type alias for optional certificate data.
    typedef bdlb::NullableValue<ntca::EncryptionCertificateStorageData>
        NullableCertificateStorageData;

    /// Define a type alias for optional key data.
    typedef bdlb::NullableValue<ntca::EncryptionKeyStorageData>
        NullableKeyStorageData;

  private:
    ntca::EncryptionMethod::Value         d_minMethod;
    ntca::EncryptionMethod::Value         d_maxMethod;
    ntca::EncryptionAuthentication::Value d_authentication;
    NullableCertificateStorageData        d_identity;
    NullableKeyStorageData                d_privateKey;
    CertificateStorageDataVector          d_authorityList;
    bdlb::NullableValue<bsl::string>      d_authorityDirectory;
    bdlb::NullableValue<bsl::string>      d_cipherSpec;

  public:
    /// Create new encryption server options.  Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionServerOptions(bslma::Allocator* basicAllocator = 0);

    /// Create new encryption server options having the same value as the
    /// specified 'other' object.  Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    EncryptionServerOptions(const EncryptionServerOptions& other,
                            bslma::Allocator*              basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionServerOptions();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionServerOptions& operator=(const EncryptionServerOptions& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the minimum permitted encryption method, inclusive, to the
    /// specified 'minMethod'.
    void setMinMethod(ntca::EncryptionMethod::Value minMethod);

    /// Set the maximum permitted encryption method, inclusive, to the
    /// specified 'maxMethod'.
    void setMaxMethod(ntca::EncryptionMethod::Value maxMethod);

    /// Set the peer authentication to the specified 'authentication'.
    void setAuthentication(
        ntca::EncryptionAuthentication::Value authentication);

    /// Set the server's certificate to the specified 'identityData'. The
    /// 'identityData' is assumed to be in the PEM format and not
    /// symmetrically-encrypted.
    void setIdentityData(const bsl::vector<char>& identityData);

    /// Set the server's certificate to the specified 'identityData'. Interpret
    /// the 'identityData' according to the specified 'identityOptions'.
    void setIdentityData(
        const bsl::vector<char>&                         identityData,
        const ntca::EncryptionCertificateStorageOptions& identityOptions);

    /// Set the server's certificate to the specified 'identityFile'. The
    /// format of the 'identityFile' is deduced from the file extension, but is
    /// assumed to be not symmetrically-encrypted.
    void setIdentityFile(const bsl::string& identityFile);

    /// Set the server's certificate to the specified 'identityFile'. Interpret
    /// the 'identityFile' according to the specified 'identityOptions'.
    void setIdentityFile(
        const bsl::string&                               identityFile,
        const ntca::EncryptionCertificateStorageOptions& identityOptions);

    /// Set the server's private key to the specified 'privateKeyData'. The
    /// 'privateKeyData' is assumed to be in the PEM format and not
    /// symmetrically-encrypted.
    void setPrivateKeyData(const bsl::vector<char>& privateKeyData);

    /// Set the server's private key to the specified 'privateKeyData'.
    /// Interpret the 'privateKeyData' according to the specified
    /// 'privateKeyOptions'.
    void setPrivateKeyData(
        const bsl::vector<char>&                 privateKeyData,
        const ntca::EncryptionKeyStorageOptions& privateKeyOptions);

    /// Set the server's private key to the specified 'privateKeyFile'. The
    /// format of the 'privateKeyFile' is deduced from the file extension, but
    /// is assumed to be not symmetrically-encrypted.
    void setPrivateKeyFile(const bsl::string& privateKeyFile);

    /// Set the server's private key to the specified 'privateKeyFile'.
    /// Interpret the 'privateKeyData' according to the specified
    /// 'privateKeyOptions'.
    void setPrivateKeyFile(
        const bsl::string&                       privateKeyFile,
        const ntca::EncryptionKeyStorageOptions& privateKeyOptions);

    /// Add the specified certificate 'authorityData'. The 'authorityData' is
    /// assumed to be in the PEM format and not symmetrically-encrypted.
    void addAuthorityData(const bsl::vector<char>& authorityData);

    /// Add the specified certificate 'authorityData'. Interpret
    /// the 'identityData' according to the specified 'identityOptions'.
    void addAuthorityData(
        const bsl::vector<char>&                         authorityData,
        const ntca::EncryptionCertificateStorageOptions& authorityOptions);

    /// Add the specified certificate 'authorityFile'. The format of the
    /// 'authorityFile' is deduced from the file extension, but is assumed to
    /// be not symmetrically-encrypted.
    void addAuthorityFile(const bsl::string& authorityFile);

    /// Add the specified certificate 'authorityFile'. Interpret the
    /// 'identityFile' according to the specified 'identityOptions'.
    void addAuthorityFile(
        const bsl::string&                               authorityFile,
        const ntca::EncryptionCertificateStorageOptions& authorityOptions);

    /// Set the directory from which to load trusted certificate authorities
    /// to the specified 'authorityDirectory'.
    void setAuthorityDirectory(const bsl::string& authorityDirectory);

    /// Restrict available ciphers to only those in the specified 'cipherSpec'.
    void setCipherSpec(const bsl::string& cipherSpec);

    /// Return the minimum permitted encryption method, inclusive.
    ntca::EncryptionMethod::Value minMethod() const;

    /// Return the maximum permitted encryption method, inclusive.
    ntca::EncryptionMethod::Value maxMethod() const;

    /// Return the peer authentication.
    ntca::EncryptionAuthentication::Value authentication() const;

    /// Return the server's certificate.
    const bdlb::NullableValue<ntca::EncryptionCertificateStorageData>& identity()
        const;

    /// Return the server's private key.
    const bdlb::NullableValue<ntca::EncryptionKeyStorageData>& privateKey()
        const;

    /// Return the server's trusted certificate authorities.
    const bsl::vector<ntca::EncryptionCertificateStorageData>& authorityList()
        const;

    /// Return the directory path to the directory containing the
    /// certificates of additional trusted authorities.
    const bdlb::NullableValue<bsl::string>& authorityDirectory() const;

    /// Return the cipher specification.
    const bdlb::NullableValue<bsl::string>& cipherSpec() const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    BSLMF_NESTED_TRAIT_DECLARATION(EncryptionServerOptions,
                                   bslma::UsesBslmaAllocator);
};

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionServerOptions
bool operator==(const EncryptionServerOptions& lhs,
                const EncryptionServerOptions& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionServerOptions
bool operator!=(const EncryptionServerOptions& lhs,
                const EncryptionServerOptions& rhs);

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionServerOptions
bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const EncryptionServerOptions& object);

}  // end namespace ntca
}  // end namespace BloombergLP
#endif
