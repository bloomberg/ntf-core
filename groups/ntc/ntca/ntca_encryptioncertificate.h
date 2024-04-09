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

#ifndef INCLUDED_NTCA_ENCRYPTIONCERTIFICATE
#define INCLUDED_NTCA_ENCRYPTIONCERTIFICATE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlt_datetimetz.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntca {

/// Describe TODO.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b todo:
/// TODO
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateTemplate
{
    bsl::size_t       d_value;
    bslma::Allocator *d_allocator_p;

  public:
    /// Create a new certificate having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionCertificateTemplate(
                                  bslma::Allocator* basicAllocator = 0);

    /// Create a new certificate having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionCertificateTemplate(
        const EncryptionCertificateTemplate& original,
        bslma::Allocator*                    basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateTemplate();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateTemplate& operator=(
        const EncryptionCertificateTemplate& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the value to the specified 'value'. 
    void setValue(bsl::size_t value);

    /// Return the value.
    bsl::size_t value() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateTemplate& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateTemplate& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateTemplate);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const EncryptionCertificateTemplate& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const EncryptionCertificateTemplate& lhs,
                const EncryptionCertificateTemplate& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const EncryptionCertificateTemplate& lhs,
                const EncryptionCertificateTemplate& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const EncryptionCertificateTemplate& lhs,
               const EncryptionCertificateTemplate& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, 
                const EncryptionCertificateTemplate& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm, 
                const EncryptionCertificateTemplate& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.value());
}
























/// Describe a certificate of identity and authenticity as used in public key
/// cryptography.
///
/// @details
///
/// Certificate  ::=  SEQUENCE  {
///         tbsCertificate       TBSCertificate,
///         signatureAlgorithm   AlgorithmIdentifier,
///         signatureValue       BIT STRING  }
/// 
///    TBSCertificate  ::=  SEQUENCE  {
///         version         [0]  EXPLICIT Version DEFAULT v1,
///         serialNumber         CertificateSerialNumber,
///         signature            AlgorithmIdentifier,
///         issuer               Name,
///         validity             Validity,
///         subject              Name,
///         subjectPublicKeyInfo SubjectPublicKeyInfo,
///         issuerUniqueID  [1]  IMPLICIT UniqueIdentifier OPTIONAL,
///                              -- If present, version MUST be v2 or v3
///         subjectUniqueID [2]  IMPLICIT UniqueIdentifier OPTIONAL,
///                              -- If present, version MUST be v2 or v3
///         extensions      [3]  EXPLICIT Extensions OPTIONAL
///                              -- If present, version MUST be v3
///         }
/// 
///    Version  ::=  INTEGER  {  v1(0), v2(1), v3(2)  }
/// 
///    CertificateSerialNumber  ::=  INTEGER
/// 
///    Validity ::= SEQUENCE {
///         notBefore      Time,
///         notAfter       Time }
/// 
///    Time ::= CHOICE {
///         utcTime        UTCTime,
///         generalTime    GeneralizedTime }
/// 
///    UniqueIdentifier  ::=  BIT STRING
/// 
///    SubjectPublicKeyInfo  ::=  SEQUENCE  {
///         algorithm            AlgorithmIdentifier,
///         subjectPublicKey     BIT STRING  }
/// 
///    Extensions  ::=  SEQUENCE SIZE (1..MAX) OF Extension
/// 
///    Extension  ::=  SEQUENCE  {
///         extnID      OBJECT IDENTIFIER,
///         critical    BOOLEAN DEFAULT FALSE,
///         extnValue   OCTET STRING
///                     -- contains the DER encoding of an ASN.1 value
///                     -- corresponding to the extension type identified
///                     -- by extnID
///         }
///


/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b serialNumber:
/// The unique number assigned to the certificate.
///
/// @li @b startTime:
/// The starting time from which the certificate is valid.
///
/// @li @b expirationTime:
/// The time at which the certificate expires.
///
/// @li @b authority:
/// The flag that indicates the certificate is a Certficate Authority (CA).
///
/// @li @b hosts:
/// The list of domain names for which the certificate is valid (i.e., the
/// subject alternative names.)
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificate
{
    enum SignatureAlgorithm {
        e_UNDEFINED
    };

    // int                       d_version;
    int                       d_serialNumber;
    bdlt::DatetimeTz          d_startTime;
    bdlt::DatetimeTz          d_expirationTime;
    bool                      d_authority;
    bsl::vector<bsl::string>  d_hosts;

    // SignatureAlgorithm        d_signatureAlgorithm;
    bsl::vector<bsl::uint8_t> d_signatureValue;

  public:
    /// Create a new certificate having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionCertificate(bslma::Allocator* basicAllocator = 0);

    /// Create a new certificate having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionCertificate(const EncryptionCertificate& original,
                          bslma::Allocator*            basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificate();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificate& operator=(const EncryptionCertificate& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the serial number to the specified 'value'.
    void setSerialNumber(int value);

    /// Set the start time from which the certificate is valid to the
    /// specified 'value'.
    void setStartTime(const bdlt::DatetimeTz& value);

    /// Set the expiration time at which the certificate is no longer valid
    /// to the specified 'value'.
    void setExpirationTime(const bdlt::DatetimeTz& value);

    /// Set the flag that indicates the certificate is a certificate
    /// authority according to the specified 'value'.
    void setAuthority(bool value);

    /// Set the domain names for which the certificate is valid to the
    /// specified 'value'.
    void setHostList(const bsl::vector<bsl::string>& value);

    /// Add the specified 'value' to the list of domain names for which the
    /// certificate is valid.
    void addHost(const bsl::string& value);

    /// Return the serial number.
    int serialNumber() const;

    /// Return the start time from which the certificate is valid.
    const bdlt::DatetimeTz& startTime() const;

    /// Return the expiration time at which the certificate is no longer
    /// valid.
    const bdlt::DatetimeTz& expirationTime() const;

    /// Return flag that indicates the certificate is a certificate
    /// authority.
    bool authority() const;

    /// Return the domain names for which the certificate is valid.
    const bsl::vector<bsl::string>& hosts() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificate& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificate& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificate);
};

/// Define a type alias for a vector of encryption certificates.
///
/// @ingroup module_ntci_encryption
typedef bsl::vector<ntca::EncryptionCertificate> EncryptionCertificateVector;

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const EncryptionCertificate& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const EncryptionCertificate& lhs,
                const EncryptionCertificate& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const EncryptionCertificate& lhs,
                const EncryptionCertificate& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const EncryptionCertificate& lhs,
               const EncryptionCertificate& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionCertificate& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionCertificate& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.serialNumber());
    hashAppend(algorithm, value.startTime());
    hashAppend(algorithm, value.expirationTime());
    hashAppend(algorithm, value.authority());
    hashAppend(algorithm, value.hosts());
}

}  // close package namespace
}  // close enterprise namespace
#endif
