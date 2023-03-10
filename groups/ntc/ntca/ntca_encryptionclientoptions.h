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

#ifndef INCLUDED_NTCA_ENCRYPTIONCLIENTOPTIONS
#define INCLUDED_NTCA_ENCRYPTIONCLIENTOPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_encryptionauthentication.h>
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

/// Describe the configuration of encryption in the client role.
///
/// @details
/// This class describes the configuration of a 'ntca::Encryptor' operating in
/// the client role. Such 'ntca::Encryption' interfaces are used to actively
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
/// The certificate including the client's identity, public key, issuer, etc.
/// If defined, the identity object takes precendence over the identity data.
///
/// @li @b identityData:
/// The PEM-encoded certificate containing the client's identity, public key,
/// and issuer. If defined, the identity data takes precedence over the
/// identity file.
///
/// @li @b identityFile:
/// The path to the PEM-encoded certificate on disk containing the client's
/// identity, public key, issuer, etc.
///
/// @li @b privateKey:
/// The private key of the client. If defined, the private key object takes
/// precendence over the private key data.
///
/// @li @b privateKeyData:
/// The PEM-encoded private key of the client. If defined the private key data
/// takes precedence over the private key file.
///
/// @li @b privateKeyFile:
/// The path to the PEM-encoded private key of the client.
///
/// @li @b authorityList:
/// The list of certificates for each trusted issuer, a.k.a. certificate
/// authority (CA). If defined the authority list takes precedence over the
/// authority data list.
///
/// @li @b authorityDataList:
/// If list of PEM-encoded certificates for each trusted issuer, a.k.a
/// certificate authority (CA). If defined, the authority data list takes
/// precedence over the authority directory.
///
/// @li @b authorityDirectory:
/// The directory containing files of PEM-encoded certificates for each trusted
/// issuer, a.k.a. certificate authority (CA).
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionClientOptions
{
  public:
    /// Define a type alias for a vector of certificate data
    /// blobs.
    typedef bsl::vector<bsl::vector<char> > CertificateDataList;

  private:
    ntca::EncryptionMethod::Value           d_minMethod;
    ntca::EncryptionMethod::Value           d_maxMethod;
    ntca::EncryptionAuthentication::Value   d_authentication;
    bdlb::NullableValue<bsl::vector<char> > d_identityData;
    bdlb::NullableValue<bsl::string>        d_identityFile;
    bdlb::NullableValue<bsl::vector<char> > d_privateKeyData;
    bdlb::NullableValue<bsl::string>        d_privateKeyFile;
    CertificateDataList                     d_authorityDataList;
    bdlb::NullableValue<bsl::string>        d_authorityDirectory;
    bdlb::NullableValue<bsl::string>        d_cipherSpec;

  public:
    /// Create new encryption client options.
    explicit EncryptionClientOptions(bslma::Allocator* basicAllocator = 0);

    /// Create new encryption client options having the same value as the
    /// specified 'other' object.
    EncryptionClientOptions(const EncryptionClientOptions& other,
                            bslma::Allocator*              basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionClientOptions();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionClientOptions& operator=(const EncryptionClientOptions& other);

    /// Set the minimum permitted encryption method, inclusive, to the
    /// specified 'minMethod'.
    void setMinMethod(ntca::EncryptionMethod::Value minMethod);

    /// Set the maximum permitted encryption method, inclusive, to the
    /// specified 'maxMethod'.
    void setMaxMethod(ntca::EncryptionMethod::Value maxMethod);

    /// Set the peer authentication to the specified 'authentication'.
    void setAuthentication(
        ntca::EncryptionAuthentication::Value authentication);

    /// Set the PEM-encoded certificate containing the client's
    /// identity, public key, and issuer to the specified 'identityData'.
    void setIdentityData(const bsl::vector<char>& identityData);

    /// Set the file path to the certificate containing the client's
    /// identity, public key, and issuer to the specified 'identityFile'.
    void setIdentityFile(const bslstl::StringRef& identityFile);

    /// Set the PEM-encoded private key of the client to the specified
    /// 'privateKeyData'.
    void setPrivateKeyData(const bsl::vector<char>& privateKeyData);

    /// Set the file path to the private key of the client to the specified
    /// 'privateKey'.
    void setPrivateKeyFile(const bslstl::StringRef& privateKeyFile);

    /// Add the PEM-encoded certificate containing a trusted authority's
    /// identity, public key, and issuer to the specified 'authorityData'.
    void addAuthorityData(const bsl::vector<char>& authorityData);

    /// Set the directory containing all the identity, public key, and
    /// issuer of all the trusted authorities.
    void setAuthorityDirectory(const bslstl::StringRef& authorityDirectory);

    /// Restrict available ciphers to only those in the specified
    /// 'cipherSpec'.  See
    /// https://www.openssl.org/docs/apps/ciphers.html#CIPHER-LIST-FORMAT
    /// for a description of the cipher specification format.
    void setCipherSpec(const bslstl::StringRef& cipherSpec);

    /// Return the minimum permitted encryption method, inclusive.
    ntca::EncryptionMethod::Value minMethod() const;

    /// Return the maximum permitted encryption method, inclusive.
    ntca::EncryptionMethod::Value maxMethod() const;

    /// Return the peer authentication.
    ntca::EncryptionAuthentication::Value authentication() const;

    /// Return the PEM-encoded certificate containing the client's
    /// identity, public key, and issuer.
    const bdlb::NullableValue<bsl::vector<char> >& identityData() const;

    /// Return the file path to the certificate containing the client's
    /// identity, public key, and issuer.
    const bdlb::NullableValue<bsl::string>& identityFile() const;

    /// Set the PEM-encoded private key of the client to the specified
    /// 'privateKeyData'.
    const bdlb::NullableValue<bsl::vector<char> >& privateKeyData() const;

    /// Return the file path to the client's private key.
    const bdlb::NullableValue<bsl::string>& privateKeyFile() const;

    /// Return the list of trusted authority PEM-encoded certificates.
    const CertificateDataList& authorityDataList() const;

    /// Return the directory path to the directory containing the
    /// certificates of all trusted authorities.
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

    BSLMF_NESTED_TRAIT_DECLARATION(EncryptionClientOptions,
                                   bslma::UsesBslmaAllocator);
};

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionClientOptions
bool operator==(const EncryptionClientOptions& lhs,
                const EncryptionClientOptions& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionClientOptions
bool operator!=(const EncryptionClientOptions& lhs,
                const EncryptionClientOptions& rhs);

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionClientOptions
bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const EncryptionClientOptions& object);

}  // end namespace ntca
}  // end namespace BloombergLP
#endif
