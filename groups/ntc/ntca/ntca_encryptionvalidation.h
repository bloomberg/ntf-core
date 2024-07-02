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

#ifndef INCLUDED_NTCA_ENCRYPTIONVALIDATION
#define INCLUDED_NTCA_ENCRYPTIONVALIDATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_encryptioncertificate.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_endpoint.h>
#include <ntsa_uri.h>
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

/// Describe the configurable parameters and behavior of certificate
/// validation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b host:
/// The domain name or IP address that must be present in the certificate
/// offered by the server, either in the subject common name, or as one of the
/// subject's alternative names. Multiple hosts may be specified; a certificate
/// is considered valid if its contents match any of the specified domain names
/// or IP addresses. If undefined, there are no domain name or IP address
/// requirements made on the certificate's subject name or subject alternative
/// names.
///
/// @li @b mail:
/// The electronic mail address that must be present in the certificate offered
/// by the server, either in the subject common name, or as one of the
/// subject's alternative names.  Multiple electron mail addresses may be
/// specified; a certificate is considered valid if its contents match any of
/// the specified electronic mail addresses. If undefined, there are no
/// electron mail address requirements made on the certificate's subject name
/// or subject alternative names.
///
/// @li @b usage:
/// The usage flags that must be present on the certificate. Multiple usages
/// may be specified; a certificate is considered valid if its contents match
/// all of the specified usages.
///
/// @li @b allowSelfSigned:
/// Trust all end-user self-signed certificates. Note that this option should
/// be set with care; when set, it bypasses the standard validation that the
/// certificate is signed by a trusted issuer.
///
/// @li @b allowAll:
/// Trust all certificates. Note that this option should be set with care; when
/// set, it bypasses the standard validation that the certificate is signed by
/// a trusted issuer. Also note that, if this option is set and a user-defined
/// validation callback is also set, that user-defined callback is assigned the
/// complete responsibility of accepting or rejecting the peer's certificate.
///
/// @li @b callback:
/// The callback to be invoked to perform additional, user-defined validation
/// of the peer's certificate. Note that most common validation is
/// automatically performed by the encryption driver implementation. Also note
/// that this callback can only reject certificates, it can validate an invalid
/// certificate.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionValidation
{
  public:
    /// Define a type alias for a nullable boolean.
    typedef bdlb::NullableValue<bool> NullableBool;

    /// Define a type alias for a vector of strings.
    typedef bsl::vector<bsl::string> StringVector;

    /// Define a type alias for a nullable vector of strings.
    typedef bdlb::NullableValue<StringVector> NullableStringVector;

    /// Define a type alias for a vector of standard key usages.
    typedef bsl::vector<ntca::EncryptionCertificateSubjectKeyUsageType::Value>
        UsageVector;

    /// Define a type alias for a nullable vector of standard key usages.
    typedef bdlb::NullableValue<UsageVector> NullableUsageVector;

    /// Define a type alias for a nullable collection of object identifiers
    /// that describe the required extended usage permitted for a key.
    typedef bdlb::NullableValue<
        ntca::EncryptionCertificateSubjectKeyUsageExtended>
        NullableUsageExtended;

    /// Define a type alias for a nullable callback invoked to validate a
    /// certificate.
    typedef bdlb::NullableValue<ntca::EncryptionCertificateValidator>
        NullableValidatorCallback;

  private:
    NullableStringVector      d_host;
    NullableStringVector      d_mail;
    NullableUsageVector       d_usage;
    NullableUsageExtended     d_usageExtensions;
    NullableBool              d_allowSelfSigned;
    NullableBool              d_allowAll;
    NullableValidatorCallback d_callback;

  public:
    /// Create new encryption validation. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit EncryptionValidation(bslma::Allocator* basicAllocator = 0);

    /// Create new encryption validation having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    EncryptionValidation(const EncryptionValidation& original,
                         bslma::Allocator*           basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionValidation();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionValidation& operator=(const EncryptionValidation& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Require a valid certificate be associated with the specified domain
    /// name or IP address 'value'. Note that multiple hosts may be specified,
    /// but a certificate is considered valid if either its subject common name
    /// or alternative names match any of the required hosts.
    void requireHost(const bsl::string& value);

    /// Require a valid certificate be associated with the specified endpoint
    /// 'value'. Note that multiple hosts may be specified, but a certificate
    /// is considered valid if either its subject common name or alternative
    /// names match any of the required hosts.
    void requireHost(const ntsa::Endpoint& value);

    /// Require a valid certificate be associated with the specified IP address
    /// 'value'. Note that multiple hosts may be specified, but a certificate
    /// is considered valid if either its subject common name or alternative
    /// names match any of the required hosts.
    void requireHost(const ntsa::IpEndpoint& value);

    /// Require a valid certificate be associated with the specified IP address
    /// 'value'. Note that multiple hosts may be specified, but a certificate
    /// is considered valid if either its subject common name or alternative
    /// names match any of the required hosts.
    void requireHost(const ntsa::IpAddress& value);

    /// Require a valid certificate be associated with the specified IP address
    /// 'value'. Note that multiple hosts may be specified, but a certificate
    /// is considered valid if either its subject common name or alternative
    /// names match any of the required hosts.
    void requireHost(const ntsa::Ipv4Address& value);

    /// Require a valid certificate be associated with the specified IP address
    /// 'value'. Note that multiple hosts may be specified, but a certificate
    /// is considered valid if either its subject common name or alternative
    /// names match any of the required hosts.
    void requireHost(const ntsa::Ipv6Address& value);

    /// Require a valid certificate be associated with the specified local name
    /// 'value'. Note that multiple hosts may be specified, but a certificate
    /// is considered valid if either its subject common name or alternative
    /// names match any of the required hosts.
    void requireHost(const ntsa::LocalName& value);

    /// Require a valid certificate be associated with the specified host
    /// 'value'. Note that multiple hosts may be specified, but a certificate
    /// is considered valid if either its subject common name or alternative
    /// names match any of the required hosts.
    void requireHost(const ntsa::Host& value);

    /// Require a valid certificate be associated with the specified domain
    /// name 'value'. Note that multiple hosts may be specified, but a
    /// certificate is considered valid if either its subject common name or
    /// alternative names match any of the required hosts.
    void requireHost(const ntsa::DomainName& value);

    /// Require a valid certificate be associated with the host portion of the
    /// specified URI 'value'. Note that multiple hosts may be specified, but a
    /// certificate is considered valid if either its subject common name or
    /// alternative names match any of the required hosts.
    void requireHost(const ntsa::Uri& value);

    /// Require a valid certificate be associated with the specified electronic
    /// mail address 'value', in the form specified by RFC 822. Note that
    /// multiple email addresses may be specified, but a certificate is
    /// considered valid if its associated email adddress matches any of the
    /// required email addresses.
    void requireMail(const bsl::string& value);

    /// Require a valid certificate have permissions for the specified
    /// standard usage 'value'.
    void requireUsage(EncryptionCertificateSubjectKeyUsageType::Value value);

    /// Require a valid certificate have permissions for the specified
    /// extended usage 'value'.
    void requireUsageExtension(
        EncryptionCertificateSubjectKeyUsageExtendedType::Value value);

    /// Require a valid certificate have permissions for the specified
    /// extended usage 'value'.
    void requireUsageExtension(const ntsa::AbstractObjectIdentifier& value);

    /// Require a valid certificate have permissions for the specified
    /// extended usage 'value'.
    void requireUsageExtension(
        const ntca::EncryptionCertificateSubjectKeyUsageExtended& value);

    /// Set the flag that indicates self-signed end-user certificates are
    /// allowed to the specified 'value'.
    void permitSelfSigned(bool value);

    /// Set the flag that indicates all certificates are allowed to the
    /// specified 'value'.
    void permitAll(bool value);

    /// Set the specified 'callback' to be invoked to perform user-defined
    /// validation of the peer's certificate.
    void setCallback(const ntca::EncryptionCertificateValidator& callback);

    /// Return the domain names and/or IP addresses associated with a valid
    /// certificate, if any.
    const NullableStringVector& host() const;

    /// Return the electronic mail addresses associated with a valid
    /// certificate, if any.
    const NullableStringVector& mail() const;

    /// Return the standard usage permissions associated with a valid
    /// certificate.
    const NullableUsageVector& usage() const;

    /// Return the extended usage permissions associated with a valid
    /// certificate.
    const NullableUsageExtended& usageExtensions() const;

    /// Return the flag that indicates self-signed end-user certificates
    /// are trusted.
    const NullableBool& allowSelfSigned() const;

    /// Return the flag that indicates all certificates are trusted.
    const NullableBool& allowAll() const;

    /// Return the callback to be invoked to perform user-defined validation of
    /// the peer's certificate.
    const NullableValidatorCallback& callback() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionValidation& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionValidation& other) const;

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
    BSLMF_NESTED_TRAIT_DECLARATION(EncryptionValidation,
                                   bslma::UsesBslmaAllocator);
};

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionValidation
bool operator==(const EncryptionValidation& lhs,
                const EncryptionValidation& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionValidation
bool operator!=(const EncryptionValidation& lhs,
                const EncryptionValidation& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionValidation
bool operator<(const EncryptionValidation& lhs,
               const EncryptionValidation& rhs);

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionValidation
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const EncryptionValidation& object);

}  // end namespace ntca
}  // end namespace BloombergLP
#endif
