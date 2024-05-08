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

#ifndef INCLUDED_NTCA_ENCRYPTIONSTORAGE
#define INCLUDED_NTCA_ENCRYPTIONSTORAGE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_encryptioncertificate.h>
#include <ntca_encryptionkey.h>
#include <ntca_encryptionresource.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntca {















/// Describe encryption storage in the PKCS1 format.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b key:
/// The key.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionStoragePkcs1
{
    ntca::EncryptionKey d_key;
    bsl::uint32_t       d_flags;

  public:
    /// Create a new encryption storage the PKCS1 format having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionStoragePkcs1(bslma::Allocator* basicAllocator = 0);

    /// Create a new encryption storage the PKCS1 format having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionStoragePkcs1(const EncryptionStoragePkcs1& original,
                           bslma::Allocator*             basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionStoragePkcs1();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionStoragePkcs1& operator=(const EncryptionStoragePkcs1& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the key to the specified 'value'.
    void setKey(const ntca::EncryptionKey& value);

    /// Return the key.
    const ntca::EncryptionKey& key() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionStoragePkcs1& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionStoragePkcs1& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionStoragePkcs1);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionStoragePkcs1
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const EncryptionStoragePkcs1& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionStoragePkcs1
bool operator==(const EncryptionStoragePkcs1& lhs,
                const EncryptionStoragePkcs1& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionStoragePkcs1
bool operator!=(const EncryptionStoragePkcs1& lhs,
                const EncryptionStoragePkcs1& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionStoragePkcs1
bool operator<(const EncryptionStoragePkcs1& lhs,
               const EncryptionStoragePkcs1& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionStoragePkcs1
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&               algorithm,
                const EncryptionStoragePkcs1& value);




















/// Describe encryption storage in the PKCS7 format.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b certificate:
/// The certificate.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionStoragePkcs7
{
    ntca::EncryptionCertificate d_certificate;
    bsl::uint32_t               d_flags;

  public:
    /// Create a new encryption storage the PKCS7 format having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionStoragePkcs7(bslma::Allocator* basicAllocator = 0);

    /// Create a new encryption storage in the PKCS7 format having the same
    /// value as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionStoragePkcs7(const EncryptionStoragePkcs7& original,
                           bslma::Allocator*             basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionStoragePkcs7();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionStoragePkcs7& operator=(const EncryptionStoragePkcs7& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the certificate to the specified 'value'.
    void setCertificate(const ntca::EncryptionCertificate& value);

    /// Return the certificate.
    const ntca::EncryptionCertificate& certificate() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionStoragePkcs7& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionStoragePkcs7& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionStoragePkcs7);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionStoragePkcs7
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const EncryptionStoragePkcs7& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionStoragePkcs7
bool operator==(const EncryptionStoragePkcs7& lhs,
                const EncryptionStoragePkcs7& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionStoragePkcs7
bool operator!=(const EncryptionStoragePkcs7& lhs,
                const EncryptionStoragePkcs7& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionStoragePkcs7
bool operator<(const EncryptionStoragePkcs7& lhs,
               const EncryptionStoragePkcs7& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionStoragePkcs7
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&               algorithm,
                const EncryptionStoragePkcs7& value);

/// Describe encryption storage in the PKCS8 format.
///
/// @details
/// The ASN.1 definition of a PKCS8 encrypted or un-encrypted private key is
/// defined in RFC 5208:
///
/// PrivateKeyInfo ::= SEQUENCE {
///     version Version,
///     privateKeyAlgorithm PrivateKeyAlgorithmIdentifier,
///     privateKey PrivateKey,
///     attributes [0] IMPLICIT Attributes OPTIONAL
/// }
///
/// Version ::= INTEGER
/// PrivateKeyAlgorithmIdentifier ::= AlgorithmIdentifier // From PKCS1
/// PrivateKey ::= OCTET STRING // choice of RSAPrivateKey or ECPrivateKey
///
/// EncryptedPrivateKeyInfo ::= SEQUENCE {
///     encryptionAlgorithm EncryptionAlgorithmIdentifier,
///     encryptedData EncryptedData
/// }
///
/// EncryptionAlgorithmIdentifier ::= AlgorithmIdentifier // From PKCS5
/// EncryptedData ::= OCTET STRING // Encrypted PrivateKeyInfo
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b key:
/// The key.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionStoragePkcs8
{
    ntca::EncryptionKey d_key;
    bsl::uint32_t       d_flags;

  public:
    /// Create a new encryption storage the PKCS8 format having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionStoragePkcs8(bslma::Allocator* basicAllocator = 0);

    /// Create a new encryption storage the PKCS8 format having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionStoragePkcs8(const EncryptionStoragePkcs8& original,
                           bslma::Allocator*             basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionStoragePkcs8();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionStoragePkcs8& operator=(const EncryptionStoragePkcs8& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the key to the specified 'value'.
    void setKey(const ntca::EncryptionKey& value);

    /// Return the key.
    const ntca::EncryptionKey& key() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionStoragePkcs8& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionStoragePkcs8& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionStoragePkcs8);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionStoragePkcs8
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const EncryptionStoragePkcs8& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionStoragePkcs8
bool operator==(const EncryptionStoragePkcs8& lhs,
                const EncryptionStoragePkcs8& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionStoragePkcs8
bool operator!=(const EncryptionStoragePkcs8& lhs,
                const EncryptionStoragePkcs8& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionStoragePkcs8
bool operator<(const EncryptionStoragePkcs8& lhs,
               const EncryptionStoragePkcs8& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionStoragePkcs8
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&               algorithm,
                const EncryptionStoragePkcs8& value);

/// Describe encryption storage in the PKCS12 format.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b type:
/// The type of the key. If not defined, a suitable key type of strength
/// suggested by current cryptographic standards is chosen.
///
/// @li @b bits:
/// The number of bits in the key. Note that this field is only relevant for
/// RSA key and should be greater than or equal to 2048; some implementations
/// may refuse to generate RSA keys using fewer bits.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionStoragePkcs12
{
    bdlb::NullableValue<ntca::EncryptionKey>         d_key;
    bdlb::NullableValue<ntca::EncryptionCertificate> d_certificate;
    ntca::EncryptionCertificateVector d_certificateAuthorityList;
    bsl::uint32_t                     d_flags;

  public:
    /// Create a new encryption storage the PKCS12 format having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit EncryptionStoragePkcs12(bslma::Allocator* basicAllocator = 0);

    /// Create a new new encryption storage the PKCS12 format having the same
    /// value as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionStoragePkcs12(const EncryptionStoragePkcs12& original,
                            bslma::Allocator*              basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionStoragePkcs12();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionStoragePkcs12& operator=(const EncryptionStoragePkcs12& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the key to the specified 'value'.
    void setKey(const ntca::EncryptionKey& value);

    /// Set the certificate to the specified 'value'.
    void setCertificate(const ntca::EncryptionCertificate& value);

    /// Add the specified 'value' to the list of trusted certificate
    /// authorities.
    void addCertificateAuthority(const ntca::EncryptionCertificate& value);

    /// Return the key.
    const bdlb::NullableValue<ntca::EncryptionKey>& key() const;

    /// Return the certificate.
    const bdlb::NullableValue<ntca::EncryptionCertificate>& certificate()
        const;

    /// Return the list of trusted certificate authorities.
    const ntca::EncryptionCertificateVector& certificateAuthorityList() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionStoragePkcs12& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionStoragePkcs12& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionStoragePkcs12);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionStoragePkcs12
bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const EncryptionStoragePkcs12& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionStoragePkcs12
bool operator==(const EncryptionStoragePkcs12& lhs,
                const EncryptionStoragePkcs12& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionStoragePkcs12
bool operator!=(const EncryptionStoragePkcs12& lhs,
                const EncryptionStoragePkcs12& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionStoragePkcs12
bool operator<(const EncryptionStoragePkcs12& lhs,
               const EncryptionStoragePkcs12& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionStoragePkcs12
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                algorithm,
                const EncryptionStoragePkcs12& value);

/// Describe encryption storage in the Privacy Encoded Mail (PEM) format.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b key:
/// The key.
///
/// @li @b certificate:
/// The certificate.
///
/// @li @b pkcs1:
/// The storage container in the PKCS1 format.
///
/// @li @b pkcs7:
/// The storage container in the PKCS7 format.
///
/// @li @b pkcs8:
/// The storage container in the PKCS8 format.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionStoragePem
{
    enum Selection { 
        e_UNDEFINED, 
        e_KEY, 
        e_CERTIFICATE, 
        e_PKCS1, 
        e_PKCS7, 
        e_PKCS8 
    };

    typedef ntca::EncryptionKey          KeyType;
    typedef ntca::EncryptionCertificate  CertificateType;
    typedef ntca::EncryptionStoragePkcs1 Pkcs1Type;
    typedef ntca::EncryptionStoragePkcs7 Pkcs7Type;
    typedef ntca::EncryptionStoragePkcs8 Pkcs8Type;

    union {
        bsls::ObjectBuffer<KeyType>         d_key;
        bsls::ObjectBuffer<CertificateType> d_certificate;
        bsls::ObjectBuffer<Pkcs1Type>       d_pkcs1;
        bsls::ObjectBuffer<Pkcs7Type>       d_pkcs7;
        bsls::ObjectBuffer<Pkcs8Type>       d_pkcs8;
    };

    Selection         d_type;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new encryption storage the PEM format having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit EncryptionStoragePem(bslma::Allocator* basicAllocator = 0);

    /// Create a new new encryption storage the PEM format having the same
    /// value as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionStoragePem(const EncryptionStoragePem& original,
                         bslma::Allocator*           basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionStoragePem();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionStoragePem& operator=(const EncryptionStoragePem& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Select the "key" representation. Return a reference to the modifiable
    /// representation.
    ntca::EncryptionKey& makeKey();

    /// Select the "key" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntca::EncryptionKey& makeKey(const ntca::EncryptionKey& value);

    /// Select the "certificate" representation. Return a reference to the
    /// modifiable representation.
    ntca::EncryptionCertificate& makeCertificate();

    /// Select the "certificate" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntca::EncryptionCertificate& makeCertificate(
        const ntca::EncryptionCertificate& value);

    /// Select the "pkcs1" representation. Return a reference to the
    /// modifiable representation.
    ntca::EncryptionStoragePkcs1& makePkcs1();

    /// Select the "pkcs1" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntca::EncryptionStoragePkcs1& makePkcs1(
        const ntca::EncryptionStoragePkcs1& value);

    /// Select the "pkcs7" representation. Return a reference to the
    /// modifiable representation.
    ntca::EncryptionStoragePkcs7& makePkcs7();

    /// Select the "pkcs7" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntca::EncryptionStoragePkcs7& makePkcs7(
        const ntca::EncryptionStoragePkcs7& value);

    /// Select the "pkcs8" representation. Return a reference to the
    /// modifiable representation.
    ntca::EncryptionStoragePkcs8& makePkcs8();

    /// Select the "pkcs8" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntca::EncryptionStoragePkcs8& makePkcs8(
        const ntca::EncryptionStoragePkcs8& value);

    /// Return a reference to the modifiable "key" representation. The
    /// behavior is undefined unless 'isKey()' is true.
    ntca::EncryptionKey& key();

    /// Return a reference to the modifiable "certificate" representation. The
    /// behavior is undefined unless 'isCertificate()' is true.
    ntca::EncryptionCertificate& certificate();

    /// Return a reference to the modifiable "pkcs1" representation. The
    /// behavior is undefined unless 'isPkcs1()' is true.
    ntca::EncryptionStoragePkcs1& pkcs1();

    /// Return a reference to the modifiable "pkcs7" representation. The
    /// behavior is undefined unless 'isPkcs7()' is true.
    ntca::EncryptionStoragePkcs7& pkcs7();

    /// Return a reference to the modifiable "pkcs8" representation. The
    /// behavior is undefined unless 'isPkcs8()' is true.
    ntca::EncryptionStoragePkcs8& pkcs8();

    /// Return a reference to the non-modifiable "key" representation. The
    /// behavior is undefined unless 'isKey()' is true.
    const ntca::EncryptionKey& key() const;

    /// Return a reference to the non-modifiable "certificate" representation.
    /// The behavior is undefined unless 'isCertificate()' is true.
    const ntca::EncryptionCertificate& certificate() const;

    /// Return a reference to the non-modifiable "pkcs1" representation. The
    /// behavior is undefined unless 'isPkcs1()' is true.
    const ntca::EncryptionStoragePkcs1& pkcs1() const;

    /// Return a reference to the non-modifiable "pkcs7" representation. The
    /// behavior is undefined unless 'isPkcs7()' is true.
    const ntca::EncryptionStoragePkcs7& pkcs7() const;

    /// Return a reference to the non-modifiable "pkcs8" representation. The
    /// behavior is undefined unless 'isPkcs8()' is true.
    const ntca::EncryptionStoragePkcs8& pkcs8() const;

    /// Return true if the representation is undefined, otherwise return false.
    bool isUndefined() const;

    /// Return true if the "key" representation is currently selected,
    /// otherwise return false.
    bool isKey() const;

    /// Return true if the "certificate" representation is currently selected,
    /// otherwise return false.
    bool isCertificate() const;

    /// Return true if the "pkcs1" representation is currently selected,
    /// otherwise return false.
    bool isPkcs1() const;

    /// Return true if the "pkcs7" representation is currently selected,
    /// otherwise return false.
    bool isPkcs7() const;

    /// Return true if the "pkcs8" representation is currently selected,
    /// otherwise return false.
    bool isPkcs8() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionStoragePem& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionStoragePem& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionStoragePem);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionStoragePem
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const EncryptionStoragePem& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionStoragePem
bool operator==(const EncryptionStoragePem& lhs,
                const EncryptionStoragePem& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionStoragePem
bool operator!=(const EncryptionStoragePem& lhs,
                const EncryptionStoragePem& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionStoragePem
bool operator<(const EncryptionStoragePem& lhs,
               const EncryptionStoragePem& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionStoragePem
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionStoragePem& value);

/// Describe encryption storage.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b key:
/// The key.
///
/// @li @b certificate:
/// The certificate.
///
/// @li @b pkcs1:
/// The storage container in the PKCS1 format.
///
/// @li @b pkcs7:
/// The storage container in the PKCS7 format.
///
/// @li @b pkcs8:
/// The storage container in the PKCS8 format.
///
/// @li @b pkcs12:
/// The storage container in the PKCS12 format.
///
/// @li @b pem:
/// The storage container in the PEM format.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionStorage
{
    enum Selection {
        e_UNDEFINED,
        e_KEY,
        e_CERTIFICATE,
        e_PKCS1,
        e_PKCS7,
        e_PKCS8,
        e_PKCS12,
        e_PEM
    };

    typedef ntca::EncryptionKey           KeyType;
    typedef ntca::EncryptionCertificate   CertificateType;
    typedef ntca::EncryptionStoragePkcs1  Pkcs1Type;
    typedef ntca::EncryptionStoragePkcs7  Pkcs7Type;
    typedef ntca::EncryptionStoragePkcs8  Pkcs8Type;
    typedef ntca::EncryptionStoragePkcs12 Pkcs12Type;
    typedef ntca::EncryptionStoragePem    PemType;

    union {
        bsls::ObjectBuffer<KeyType>         d_key;
        bsls::ObjectBuffer<CertificateType> d_certificate;
        bsls::ObjectBuffer<Pkcs1Type>       d_pkcs1;
        bsls::ObjectBuffer<Pkcs7Type>       d_pkcs7;
        bsls::ObjectBuffer<Pkcs8Type>       d_pkcs8;
        bsls::ObjectBuffer<Pkcs12Type>      d_pkcs12;
        bsls::ObjectBuffer<PemType>         d_pem;
    };

    Selection         d_type;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new encryption storage having the default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit EncryptionStorage(bslma::Allocator* basicAllocator = 0);

    /// Create a new new encryption storage having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    EncryptionStorage(const EncryptionStorage& original,
                      bslma::Allocator*        basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionStorage();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionStorage& operator=(const EncryptionStorage& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Select the "key" representation. Return a reference to the modifiable
    /// representation.
    ntca::EncryptionKey& makeKey();

    /// Select the "key" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntca::EncryptionKey& makeKey(const ntca::EncryptionKey& value);

    /// Select the "certificate" representation. Return a reference to the
    /// modifiable representation.
    ntca::EncryptionCertificate& makeCertificate();

    /// Select the "certificate" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntca::EncryptionCertificate& makeCertificate(
        const ntca::EncryptionCertificate& value);

    /// Select the "pkcs1" representation. Return a reference to the
    /// modifiable representation.
    ntca::EncryptionStoragePkcs1& makePkcs1();

    /// Select the "pkcs1" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntca::EncryptionStoragePkcs1& makePkcs1(
        const ntca::EncryptionStoragePkcs1& value);

    /// Select the "pkcs7" representation. Return a reference to the
    /// modifiable representation.
    ntca::EncryptionStoragePkcs7& makePkcs7();

    /// Select the "pkcs7" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntca::EncryptionStoragePkcs7& makePkcs7(
        const ntca::EncryptionStoragePkcs7& value);

    /// Select the "pkcs8" representation. Return a reference to the
    /// modifiable representation.
    ntca::EncryptionStoragePkcs8& makePkcs8();

    /// Select the "pkcs8" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntca::EncryptionStoragePkcs8& makePkcs8(
        const ntca::EncryptionStoragePkcs8& value);

    /// Select the "pkcs12" representation. Return a reference to the
    /// modifiable representation.
    ntca::EncryptionStoragePkcs12& makePkcs12();

    /// Select the "pkcs12" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntca::EncryptionStoragePkcs12& makePkcs12(
        const ntca::EncryptionStoragePkcs12& value);

    /// Select the "pem" representation. Return a reference to the modifiable
    /// representation.
    ntca::EncryptionStoragePem& makePem();

    /// Select the "pem" representation initially having the specified 'value'.
    /// Return a reference to the modifiable representation.
    ntca::EncryptionStoragePem& makePem(
        const ntca::EncryptionStoragePem& value);

    /// Return a reference to the modifiable "key" representation. The
    /// behavior is undefined unless 'isKey()' is true.
    ntca::EncryptionKey& key();

    /// Return a reference to the modifiable "certificate" representation. The
    /// behavior is undefined unless 'isCertificate()' is true.
    ntca::EncryptionCertificate& certificate();

    /// Return a reference to the modifiable "pkcs1" representation. The
    /// behavior is undefined unless 'isPkcs1()' is true.
    ntca::EncryptionStoragePkcs1& pkcs1();

    /// Return a reference to the modifiable "pkcs7" representation. The
    /// behavior is undefined unless 'isPkcs7()' is true.
    ntca::EncryptionStoragePkcs7& pkcs7();

    /// Return a reference to the modifiable "pkcs8" representation. The
    /// behavior is undefined unless 'isPkcs8()' is true.
    ntca::EncryptionStoragePkcs8& pkcs8();

    /// Return a reference to the modifiable "pkcs12" representation. The
    /// behavior is undefined unless 'isPkcs12()' is true.
    ntca::EncryptionStoragePkcs12& pkcs12();

    /// Return a reference to the modifiable "pem" representation. The
    /// behavior is undefined unless 'isPem()' is true.
    ntca::EncryptionStoragePem& pem();

    /// Return a reference to the non-modifiable "key" representation. The
    /// behavior is undefined unless 'isKey()' is true.
    const ntca::EncryptionKey& key() const;

    /// Return a reference to the non-modifiable "certificate" representation.
    /// The behavior is undefined unless 'isCertificate()' is true.
    const ntca::EncryptionCertificate& certificate() const;

    /// Return a reference to the non-modifiable "pkcs1" representation. The
    /// behavior is undefined unless 'isPkcs1()' is true.
    const ntca::EncryptionStoragePkcs1& pkcs1() const;

    /// Return a reference to the non-modifiable "pkcs7" representation. The
    /// behavior is undefined unless 'isPkcs7()' is true.
    const ntca::EncryptionStoragePkcs7& pkcs7() const;

    /// Return a reference to the non-modifiable "pkcs8" representation. The
    /// behavior is undefined unless 'isPkcs8()' is true.
    const ntca::EncryptionStoragePkcs8& pkcs8() const;

    /// Return a reference to the non-modifiable "pkcs12" representation. The
    /// behavior is undefined unless 'isPkcs12()' is true.
    const ntca::EncryptionStoragePkcs12& pkcs12() const;

    /// Return a reference to the non-modifiable "pem" representation. The
    /// behavior is undefined unless 'isPem()' is true.
    const ntca::EncryptionStoragePem& pem() const;

    /// Return true if the representation is undefined, otherwise return false.
    bool isUndefined() const;

    /// Return true if the "key" representation is currently selected,
    /// otherwise return false.
    bool isKey() const;

    /// Return true if the "certificate" representation is currently selected,
    /// otherwise return false.
    bool isCertificate() const;

    /// Return true if the "pkcs1" representation is currently selected,
    /// otherwise return false.
    bool isPkcs1() const;

    /// Return true if the "pkcs7" representation is currently selected,
    /// otherwise return false.
    bool isPkcs7() const;

    /// Return true if the "pkcs8" representation is currently selected,
    /// otherwise return false.
    bool isPkcs8() const;

    /// Return true if the "pkcs12" representation is currently selected,
    /// otherwise return false.
    bool isPkcs12() const;

    /// Return true if the "pem" representation is currently selected,
    /// otherwise return false.
    bool isPem() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionStorage& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionStorage& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionStorage);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionStorage
bsl::ostream& operator<<(bsl::ostream&            stream,
                         const EncryptionStorage& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionStorage
bool operator==(const EncryptionStorage& lhs, const EncryptionStorage& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionStorage
bool operator!=(const EncryptionStorage& lhs, const EncryptionStorage& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionStorage
bool operator<(const EncryptionStorage& lhs, const EncryptionStorage& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionStorage
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionStorage& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionStoragePkcs7& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.certificate());
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionStoragePkcs8& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.key());
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                algorithm,
                const EncryptionStoragePkcs12& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.key());
    hashAppend(algorithm, value.certificate());
    hashAppend(algorithm, value.certificateAuthorityList());
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionStoragePem& value)
{
    using bslh::hashAppend;

    if (value.isKey()) {
        hashAppend(algorithm, value.key());
    }
    else if (value.isCertificate()) {
        hashAppend(algorithm, value.certificate());
    }
    else if (value.isPkcs1()) {
        hashAppend(algorithm, value.pkcs1());
    }
    else if (value.isPkcs7()) {
        hashAppend(algorithm, value.pkcs7());
    }
    else if (value.isPkcs8()) {
        hashAppend(algorithm, value.pkcs8());
    }
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionStorage& value)
{
    using bslh::hashAppend;

    if (value.isKey()) {
        hashAppend(algorithm, value.key());
    }
    else if (value.isCertificate()) {
        hashAppend(algorithm, value.certificate());
    }
    else if (value.isPkcs1()) {
        hashAppend(algorithm, value.pkcs1());
    }
    else if (value.isPkcs7()) {
        hashAppend(algorithm, value.pkcs7());
    }
    else if (value.isPkcs8()) {
        hashAppend(algorithm, value.pkcs8());
    }
    else if (value.isPkcs12()) {
        hashAppend(algorithm, value.pkcs12());
    }
    else if (value.isPem()) {
        hashAppend(algorithm, value.pem());
    }
}

}  // close package namespace
}  // close enterprise namespace
#endif
