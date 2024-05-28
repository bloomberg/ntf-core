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
#include <ntca_encryptioncertificate.h>
#include <ntca_encryptionmethod.h>
#include <ntca_encryptionoptions.h>
#include <ntca_encryptionresource.h>
#include <ntca_encryptionresourcedescriptor.h>
#include <ntca_encryptionresourceoptions.h>
#include <ntca_encryptionrole.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bdlbb_blob.h>
#include <bslma_usesbslmaallocator.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bsl_iosfwd.h>
#include <bsl_map.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntca {

/// Describe the configuration of encryption in the client role.
///
/// @details
/// This class describes the configuration of an encryption session operating
/// in the client role. Encryption clients actively initiate a
/// cryptographically secure session of communication, typically according to
/// either the Transport Layer Security (TLS) protocol or Secure Shell (SSH)
/// protocol, within which data is transformed from from cleartext to
/// ciphertext.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b minMethod:
/// The type and minimum version of the encryption protocol acceptable for use.
///
/// @li @b maxMethod:
/// The type and maximum version of the encryption protocol acceptable for use.
///
/// @li @b authentication:
/// Flag that determines whether the peer's certificate is verified as signed
/// by a trusted issuer.
///
/// @li @b validation:
/// The peer certificate validation requirements and allowances.
///
/// @li @b resources:
/// The resources containing the private key, certificate, and trusted
/// certificate authorities.
///
/// @li @b authorityDirectory:
/// The directory containing files of encoded certificates for each trusted
/// certificate authority.
///
/// @li @b optionsMap
/// The optional, effective options to use when connecting to a specific server
/// name. Note that a server name, in this context, may be an IP address,
/// domain name, or domain name wildcard such as "*.example.com".
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionClientOptions
{
    typedef bsl::map<bsl::string, ntca::EncryptionOptions> OptionsMap;

    ntca::EncryptionOptions d_options;
    OptionsMap              d_optionsMap;

  public:
    /// Create new encryption client options.  Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionClientOptions(bslma::Allocator* basicAllocator = 0);

    /// Create new encryption client options having the same value as the
    /// specified 'other' object.  Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    EncryptionClientOptions(const EncryptionClientOptions& other,
                            bslma::Allocator*              basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionClientOptions();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionClientOptions& operator=(const EncryptionClientOptions& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the minimum permitted encryption method, inclusive, to the
    /// specified 'minMethod'.
    void setMinMethod(ntca::EncryptionMethod::Value minMethod);

    /// Set the maximum permitted encryption method, inclusive, to the
    /// specified 'maxMethod'.
    void setMaxMethod(ntca::EncryptionMethod::Value maxMethod);

    /// Restrict available ciphers to only those in the specified 'cipherSpec'.
    void setCipherSpec(const bsl::string& cipherSpec);

    /// Set the peer authentication to the specified 'authentication'.
    void setAuthentication(
        ntca::EncryptionAuthentication::Value authentication);

    /// Set the peer certificate validation requirements and allowances to the
    /// specified 'validation'.
    void setValidation(const ntca::EncryptionValidation& validation);

    /// Set the directory from which to load trusted certificate authorities to
    /// the specified 'authorityDirectory'.
    void setAuthorityDirectory(const bsl::string& authorityDirectory);

    /// Add the specified 'certificates' as trusted certificate authorities.
    /// Note that the effect of calling this function is identical to simply
    /// repeatedly calling 'addResource' with resource options that indicate
    /// the resource contains only trusted certificate authorities, for each
    /// certificate in the 'certficates' vector.
    void addAuthorityList(
        const ntca::EncryptionCertificateVector& certificates);

    /// Add the specified 'certificate' as a trusted certificate authority.
    /// Note that the effect of calling this function is identical to simply
    /// calling 'addResource' with resource options that indicate the resource
    /// contains only trusted certificate authorities.
    void addAuthority(const ntca::EncryptionCertificate& certificate);

    /// Add the specified 'resourceData' as encoded resource data for one or
    /// more trusted certificate authority. Note that the effect of calling
    /// this function is identical to simply calling 'addResourceData' with
    /// resource options that indicate the resource contains only trusted
    /// certificate authorities.
    void addAuthorityData(const bsl::vector<char>& resourceData);

    /// Add the specified 'resourceData' as encoded resource data for one or
    /// more trusted certificate authority that should be decoded according to
    /// the specified 'resourceOptions'. Note that the effect of calling this
    /// function is identical to simply calling 'addResourceData' with resource
    /// options that indicate the resource contains only trusted certificate
    /// authorities.
    void addAuthorityData(
        const bsl::vector<char>&               resourceData,
        const ntca::EncryptionResourceOptions& resourceOptions);

    /// Add the specified 'resourcePath' to encoded resource data on disk for
    /// one or more trusted certificate authorities. Note that the effect of
    /// calling this function is identical to simply calling 'addResourcePath'
    /// with resource options that indicate the resource contains only trusted
    /// certificate authorities.
    void addAuthorityFile(const bsl::string& resourcePath);

    /// Add the specified 'resourcePath' to encoded resource data on disk for
    /// one or more trusted certificate authorities. Note that the effect of
    /// calling this function is identical to simply calling 'addResourcePath'
    /// with resource options that indicate the resource contains only trusted
    /// certificate authorities.
    void addAuthorityFile(
        const bsl::string&                     resourcePath,
        const ntca::EncryptionResourceOptions& resourceOptions);

    /// Set the end-user identity to the specified 'certificate'. Note that the
    /// effect of calling this function is identical to simply calling
    /// 'addResource' with resource options that indicate the resource contains
    /// an end-user certificate.
    void setIdentity(const ntca::EncryptionCertificate& certificate);

    /// Set the end-user identity data to the specified encoded 'resourceData'.
    /// Note that the effect of calling this function is identical to simply
    /// calling 'addResourceData' with resource options that indicate the
    /// resource contains an end-user certificate.
    void setIdentityData(const bsl::vector<char>& resourceData);

    /// Set the end-user identity data to the specified encoded 'resourceData'
    /// decoded according to the specified 'resourceOptions'. Note that the
    /// effect of calling this function is identical to simply calling
    /// 'addResourceData' with resource options that indicate the resource
    /// contains an end-user certificate.
    void setIdentityData(
        const bsl::vector<char>&               resourceData,
        const ntca::EncryptionResourceOptions& resourceOptions);

    /// Set the path to the encoded end-user identity data on disk to the
    /// specified 'resourcePath'. Note that the effect of calling this function
    /// is identical to simply calling 'addResourcePath' with resource options
    /// that indicate the resource contains an end-user certificate.
    void setIdentityFile(const bsl::string& resourcePath);

    /// Set the path to the encoded end-user identity data on disk to the
    /// specified 'resourcePath' decoded according to the specified
    /// 'resourceOptions'. Note that the effect of calling this function is
    /// identical to simply calling 'addResourcePath' with resource options
    /// that indicate the resource contains an end-user certificate.
    void setIdentityFile(
        const bsl::string&                     resourcePath,
        const ntca::EncryptionResourceOptions& resourceOptions);

    /// Set the private key to the specified 'certificate'. Note that the
    /// effect of calling this function is identical to simply calling
    /// 'addResource' with resource options that indicate the resource contains
    /// a private key.
    void setPrivateKey(const ntca::EncryptionKey& key);

    /// Set the private key data to the specified encoded 'resourceData'. Note
    /// that the effect of calling this function is identical to simply calling
    /// 'addResourceData' with resource options that indicate the resource
    /// contains a private key.
    void setPrivateKeyData(const bsl::vector<char>& resourceData);

    /// Set the private key data to the specified encoded 'resourceData'
    /// decoded according to the specified 'resourceOptions'. Note that the
    /// effect of calling this function is identical to simply calling
    /// 'addResourceData' with resource options that indicate the resource
    /// contains a private key.
    void setPrivateKeyData(
        const bsl::vector<char>&               resourceData,
        const ntca::EncryptionResourceOptions& resourceOptions);

    /// Set the path to the encoded private key data on disk to the specified
    /// 'resourcePath'. Note that the effect of calling this function is
    /// identical to simply calling 'addResourcePath' with resource options
    /// that indicate the resource contains a private key.
    void setPrivateKeyFile(const bsl::string& resourcePath);

    /// Set the path to the encoded private key data on disk to the specified
    /// 'resourcePath' decoded according to the specified 'resourceOptions'.
    /// Note that the effect of calling this function is identical to simply
    /// calling 'addResourcePath' with resource options that indicate the
    /// resource contains a private key.
    void setPrivateKeyFile(
        const bsl::string&                     resourcePath,
        const ntca::EncryptionResourceOptions& resourceOptions);

    /// Add the specified 'certificate' as a resource contributing either
    /// an end-user certificate or a trusted certificate authority.
    void addResource(const ntca::EncryptionCertificate& certificate);

    /// Add the specified 'key' as a resource to contribute a private key.
    void addResource(const ntca::EncryptionKey& key);

    /// Add the specified encoded 'resource' to contribute an optional private
    /// key, optional end-user certificate, and optional list of trusted
    /// certificate authorities.
    void addResource(const ntca::EncryptionResource& resourceData);

    /// Add the specified encoded 'resourceData' to contribute an optional
    /// private key, optional end-user certificate, and optional list of
    /// trusted certificate authorities.
    void addResourceData(const bsl::vector<char>& resourceData);

    /// Add the specified encoded 'resourceData' to contribute an optional
    /// private key, optional certificate, and optional list of trusted
    /// certificate authorities. Interpret the 'resourceData' according to the
    /// specified 'resourceOptions'.
    void addResourceData(
        const bsl::vector<char>&               resourceData,
        const ntca::EncryptionResourceOptions& resourceOptions);

    /// Add the encoded contents of the file at the specified 'resourcePath' to
    /// contribute an optional private key, optional certificate, and optional
    /// list of trusted certificate authorities. Interpret the 'resourceFile'
    /// according to the specified 'resourceOptions'.
    void addResourceFile(const bsl::string& resourcePath);

    /// Add the encoded contents of the file at the specified 'resourcePath' to
    /// contribute an optional private key, optional certificate, and optional
    /// list of trusted certificate authorities. Interpret the 'resourceFile'
    /// according to the specified 'resourceOptions'.
    void addResourceFile(
        const bsl::string&                     resourcePath,
        const ntca::EncryptionResourceOptions& resourceOptions);

    /// Add the specified 'options' to be used when connecting sessions to
    /// the specified 'serverName'. If 'options' is empty or "*", interpret
    /// 'options' as the default options. Note that 'serverName' may be an IP
    /// address, domain name, or domain name wildcard such as "*.example.com".
    void addOverrides(const bsl::string&             serverName,
                      const ntca::EncryptionOptions& options);

    /// Return the minimum permitted encryption method, inclusive.
    ntca::EncryptionMethod::Value minMethod() const;

    /// Return the maximum permitted encryption method, inclusive.
    ntca::EncryptionMethod::Value maxMethod() const;

    /// Return the cipher specification.
    const bdlb::NullableValue<bsl::string>& cipherSpec() const;

    /// Return the peer authentication.
    ntca::EncryptionAuthentication::Value authentication() const;

    /// Return the peer certificate validation requirements and allowances.
    const bdlb::NullableValue<ntca::EncryptionValidation>& validation() const;

    /// Return the directory path to the directory containing the
    /// certificates of additional trusted authorities.
    const bdlb::NullableValue<bsl::string>& authorityDirectory() const;

    /// Return the resources.
    const ntca::EncryptionResourceVector& resources() const;

    /// Load into the specified 'result' the names of each registered server.
    /// Note that 'serverName' may be an IP address, domain name, or domain
    /// name wildcard such as "*.example.com". Also note that the first name
    /// will always be "*" to denote the default options.
    void loadServerNameList(bsl::vector<bsl::string>* result) const;

    ///  Load into the specified 'result' the options for the specified
    /// 'serverName'. Note that 'serverName' may be an IP address, domain name,
    /// or domain name wildcard such as "*.example.com".
    bool loadServerNameOptions(ntca::EncryptionOptions* result,
                               const bsl::string&       serverName) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionClientOptions& other) const;

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
