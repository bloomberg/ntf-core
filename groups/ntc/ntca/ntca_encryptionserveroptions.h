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

/// Describe the configuration of encryption in the server role.
///
/// @details
/// This class describes the configuration of a source of encryption sessions
/// operating in the server role. Encryption servers passively wait for the
/// peer to initiate a cryptographically secure session of communication,
/// typically according to either the Transport Layer Security (TLS) protocol
/// or Secure Shell (SSH) protocol, within which data is transformed from from
/// cleartext to ciphertext. An encryption server may generate one or more 
/// encryption sessions acting in the server role.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b minMethod:
/// The type and minimum version of the encryption protocol acceptable for use.
/// If set to 'ntca::EncryptionMethod::e_DEFAULT' (the default value) the
/// minimum version is interpreted as the minimum version suggested by the
/// current standards of cryptography.
///
/// @li @b maxMethod:
/// The type and maximum version of the encryption protocol acceptable for use.
/// If set to 'ntca::EncryptionMethod::e_DEFAULT' (the default value) the
/// maximum version is interpreted as the maximum version supported by the 
/// TLS implementation.
///
/// @li @b authentication:
/// Flag that determines whether the peer's certificate is verified as signed
/// by a trusted issuer. If set to 'ntca::EncryptionAuthentication::e_DEFAULT'
/// (the default value), the server does not verify any of its clients's 
/// identities.
///
/// @li @b validation:
/// The peer certificate validation requirements and allowances. The default
/// value verifies the peer's certificate is signed by a trusted certificate
/// authority, if peer authentication is enabled.
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
/// The optional, effective options to use when listening as a specific server
/// name. Note that a server name, in this context, may be an IP address,
/// domain name, a domain name wildcard such as "*.example.com", or any 
/// sequence of characters used to identify the server.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par Usage Example: Defining trusted certificate authorities
/// Typically, servers that desire to communicate securely with a remote client
/// do not also also verify the identity claimed by the client is authentic
/// (i.e., the client is who they say they are.). However, in certain cases
/// authentication of the clients is desired. In TLS, authentication is
/// achieved by verifying the issuer and signer of the peer's certificate is
/// trusted by server, forming a chain a trust: if the server trusts the signer
/// of the peer's certificate, the server trusts the signer has verified the
/// peer's identity, and transitively trusts the peer. 
///
/// To build and verify this chain of trust, the server must explicitly define
/// which certificates form the roots of each chain. By default, all
/// certificate authorities trusted by the system are also trusted by the
/// server, or alternatively, the server may specify a directory from which
/// trusted certificates are discovered and loaded. Additionally, the server
/// may specify paths to the trusted certificate authorities stored on disk.
///
/// For the purposes of this example, let's imagine there exists a directory
/// "/etc/pki/trust/default" that contains certificates of trusted certificate
/// authorities. Furthermore, let's imagine there exists two additional files,
/// "/etc/pki/trust/extra/mozilla.pki" and "/etc/pki/trust/extra/google.pki",
/// each containing certificate(s) that should also be trusted.
///
/// Now, let's define encryption server options for an encryption server whose
/// encryption sessions authenticate the peer's certificate against these
/// trusted certificate authorities.
///
///     ntca::EncryptionServerOptions encryptionServerOptions;
///
///     encryptionServerOptions.setAuthentication(
///         ntca::EncryptionAuthentication::e_VERIFY);
///
///     encryptionServerOptions.setAuthorityDirectory(
///         "/etc/pki/trust/default");
///
///     encryptionServerOptions.addAuthorityFile(
///         "/etc/pki/trust/extra/mozilla.pki");
///
///     encryptionServerOptions.addAuthorityFile(
///         "/etc/pki/trust/extra/google.pki");
///
/// Note that "mozilla.pki" and "google.pki" may be any supported format
/// enumerated by ntca::EncryptionResourceType, and may even contain a
/// concatenation of multiple objects of that resource type (the files may
/// contain a concatenation of PEM-encoded certificates, for example.) 
///
/// Also note that if any file contains more than one resource type (i.e. a
/// private key, and/or an end-user certificate, and/or a set of trusted
/// certificate authorities) then the whole bundle can be loaded simultanously.
/// Let's imagine there exists "/etc/pki/application/task.pki" that contains
/// all the public key cryptography objects required for operation: the
/// server's private key, its end-user certificate, and all the trusted
/// certificate authorities. All those objects can be loaded by calling the
/// single function:
///
///     encryptionServerOptions.addResourceFile(
///         "/etc/pki/application/task.pki");
///
/// This function will set any private key found in the file to the servers's
/// private key, set any end-user certificate found in the file as the server's
/// certificate, and the server will trust any certificate authorities found 
/// in the file.
///
/// @par Usage Example: Defining end-user certificates and private keys
/// Typically, clients that desire to communicate securely with a remote server
/// also desire to verify the identity claimed by the server is authentic
/// (i.e., the server is who they say they are.). The user must assign an
/// end-user certificate and private key to the encryption server options, with
/// the certificate signed by some authority that the client must trust for the
/// handshake to succeed.
///
/// Let's imagine this servers's certificate and keys are stored in
/// "/etc/pki/my/certificate.pki" and "/etc/pki/my/key.pki". Let's register
/// that certificate and private key to be use during the encryption session.
///
///     ntca::EncryptionServerOptions encryptionServerOptions;
///
///     encryptionServerOptions.setIdentityFile("/etc/pki/my/certificate.pki");
///     encryptionServerOptions.setPrivateKeyFile("/etc/pki/my/key.pki");
///
/// If the end-user certificate and private key are bundled in the same
/// file, they may be loaded simulatenously. Let's say the aforementioned
/// certificate and private key are bundled together into the same file
/// "/etc/pki/my/bundle.pki", the certificate and private key may be registered
/// simulatenously:
///
///     encryptionServerOptions.addResourceFile("/etc/pki/my/bundle.pki");
///
/// @par Usage Example: Loading symmetrically-encrypted private keys
/// Since resources may be bundled together into the same file, or the user may
/// wish the store or transmit this file containing the private key unsecurely,
/// the private key itself may be symmetrically encrypted and require a
/// "passphrase" to decrypt it. Processing such symmetrically-encrypted private
/// keys requires the user to install a callback, invoked to resolve the
/// required passphrase to decode the private key.
///
/// Let's imagine a symmetrically-encrypted private key is stored at
/// "/etc/pki/my/key.pki" encrypted using the passphrase "My$3cret!". To
/// register this private key, the user must implement a passphrase callback
/// and register than callback to be invoked when the private key is decoded:
///
///     ntsa::Error passphraseCallback(ntca::EncryptionSecret* result)
///     {
///         result->append("My$3cret!", 9);
///         return ntsa::Error(ntsa::Error::e_INVALID);
///     }
///
/// Now, let's register the private key to be used by the server, associated
/// with our passphrase callback.
///
///     ntca::EncryptionResourceOptions resourceOptions;
///     resourceOptions.setSecretCallback(&passphraseCallback);
///
///     ntca::EncryptionServerOptions encryptionServerOptions;
///     
///     encryptionServerOptions.setPrivateKeyFile(
///         "/etc/pki/my/key.pki", resourceOptions);
///
/// @par Usage Example: Defining server name-specific configuration
/// The TLS protocol allows the client to specify the "name" of the server with
/// which they wish to establish secure communication. This feature is termed
/// "server name indication", or SNI. See RFC 6066 section 3 for more
/// information on how an why SNI should be used. Clients may optionally
/// specify the "server name" to which they are connecting when initiating the
/// upgrade operation (see 'ntci::StreamSocket::upgrade'.) The server may
/// define a configuration "override" for that server name, which, for example,
/// uses different end-user certificates and private keys depending on the
/// targetr server name.
///
/// Let's imagine there exists three files, "/etc/pki/trust/server/default.pki"
/// "/etc/pki/trust/server/beta.pki", and "/etc/pki/trust/server/internal.pki",
/// each containing a certificate and private key; the first file should be
/// used when the client does not specify a server name, the second when the
/// client indicates it requests the server name "beta.example.com", and the
/// third when client indicates it requests "internal.example.com". (Note the
/// server name is usually a domain name, but this is not required.)
///
/// First, let's define the default server configuration. This configuration
/// is effective when the client does not specify an explicit server name
/// during the handshake to establishing the TLS session.
///
///     ntca::EncryptionServerOptions encryptionServerOptions;
///
///     encryptionServerOptions.addResourceFile(
///         "/etc/pki/trust/server/default.pki");
///
/// Next, let's define the server configuration effective when the user 
/// upgrades specifically to "beta.example.com".
///
///     ntca::EncryptionOptions betaServerNameOverrides;
///     
///     betaServerNameOverrides.addResourceFile(
///         "/etc/pki/trust/server/beta.pk");
///
///     encryptionClientOptions.addOverrides(
///         "beta.example.com", betaServerNameOverrides);
///
/// Finally, let's define the server configuration effective when the user 
/// upgrades specifically to "internal.example.com".
///
///     ntca::EncryptionOptions internalServerNameOverrides;
///     
///     internalServerNameOverrides.addResourceFile(
///         "/etc/pki/trust/server/internal.pk");
///
///     encryptionClientOptions.addOverrides(
///         "internal.example.com", internalServerNameOverrides);
///
/// Note that wildcard name matching is available, so for example, if overrides
/// are registered for "*.example.com" then the overrides would match for 
/// TLS sessions to either "test.example.com" or "production.example.com".
///
/// @ingroup module_ntci_encryption
class EncryptionServerOptions
{
    typedef bsl::map<bsl::string, ntca::EncryptionOptions> OptionsMap;

    ntca::EncryptionOptions d_options;
    OptionsMap              d_optionsMap;

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

    /// Add the specified 'options' to be used when listening for sessions as
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
    bool equals(const EncryptionServerOptions& other) const;

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
