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

#ifndef INCLUDED_NTCA_ENCRYPTIONKEY
#define INCLUDED_NTCA_ENCRYPTIONKEY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_encryptionkeytype.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_abstract.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntca {

// MRM: Keys are stored in either:
//    1) RsaPrivateKey (PKCS1, RFC 8017)
//    2) ECPrivateKey (RFC 5915)
//    3) PrivateKeyInfo version 0 (PKCS8, RFC 5208)
//    4) PrivateKeyInfo version 1 (PKCS8, RFC 5958)
//
// OpenSSL's EVP_PKEY i2d chooses 1) for RSA keys, 2) for named elliptic
// curves, and 3) for Edwards curves (ED25519 and ED448).

// Rsa
// EllipticCurve

// PrivateKeyRsa
//     modulus
//     encryptionExponent
//     decryptionExponent
//     etc.
// PrivateKeyEllipticCurve
//     parameters
//         identifier, or
//         any
//     privateKey
//     publicKey
// PrivateKeyInfo
//     PrivateKeyInfoRaw
//         algorithm
//             identifier
//             parameters
//         privateKey
//         attributes [0]
//         publicKey  [1]
//     PrivateKeyInfoEnc
//         [ cipher ]
//         [ PrivateKeyInfoRaw ]

// In ntca_encryptioncertificate.h/cpp:
//
// Replace EncryptionCertificatePublicKeyAlgorithmParametersRsa
//    with EncryptionKeyRsaParameters
//
// Replace EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve
//    with EncryptionKeyEllipticCurveParameters
// 
// Erase   
// EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier
// EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifierType

class EncryptionKeyRsaParameters;
class EncryptionKeyRsaExtra;
class EncryptionKeyRsa;

class EncryptionKeyEllipticCurveParametersIdentifierType;
class EncryptionKeyEllipticCurveParametersIdentifier;
class EncryptionKeyEllipticCurveParameters;
class EncryptionKeyEllipticCurve;

class EncryptionKey;



/// Describe encryption certificate public key algorithm parameters for the RSA
/// algorithm.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionKeyRsaParameters
{
    ntsa::AbstractValue d_value;
    bslma::Allocator*   d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionKeyRsaParameters(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionKeyRsaParameters(
        const EncryptionKeyRsaParameters& original,
        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionKeyRsaParameters();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionKeyRsaParameters& operator=(
        const EncryptionKeyRsaParameters& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionKeyRsaParameters&
                    other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionKeyRsaParameters&
                  other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(
        EncryptionKeyRsaParameters);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionKeyRsaParameters
bsl::ostream& operator<<(
    bsl::ostream&                                               stream,
    const EncryptionKeyRsaParameters& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionKeyRsaParameters
bool operator==(
    const EncryptionKeyRsaParameters& lhs,
    const EncryptionKeyRsaParameters& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionKeyRsaParameters
bool operator!=(
    const EncryptionKeyRsaParameters& lhs,
    const EncryptionKeyRsaParameters& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionKeyRsaParameters
bool operator<(
    const EncryptionKeyRsaParameters& lhs,
    const EncryptionKeyRsaParameters& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionKeyRsaParameters
template <typename HASH_ALGORITHM>
void hashAppend(
    HASH_ALGORITHM&                                             algorithm,
    const EncryptionKeyRsaParameters& value);

template <typename HASH_ALGORITHM>
void EncryptionKeyRsaParameters::hash(
    HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(
    HASH_ALGORITHM&                                             algorithm,
    const EncryptionKeyRsaParameters& value)
{
    value.hash(algorithm);
}


/// Describe an encryption certificate structure with an unknown composition.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionKeyRsaExtra
{
    ntsa::AbstractInteger d_prime;
    ntsa::AbstractInteger d_exponent;
    ntsa::AbstractInteger d_coefficient;
    bslma::Allocator*     d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionKeyRsaExtra(bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionKeyRsaExtra(const EncryptionKeyRsaExtra& original,
                                bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionKeyRsaExtra();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionKeyRsaExtra& operator=(
        const EncryptionKeyRsaExtra& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the prime to the specified 'value'.
    void setPrime(const ntsa::AbstractInteger& value);

    /// Set the exponent to the specified 'value'.
    void setExponent(const ntsa::AbstractInteger& value);

    /// Set the coefficient to the specified 'value'.
    void setCoefficient(const ntsa::AbstractInteger& value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the prime.
    const ntsa::AbstractInteger& prime() const;

    /// Return the exponent.
    const ntsa::AbstractInteger& exponent() const;

    /// Return the coefficient.
    const ntsa::AbstractInteger& coefficient() const;
    
    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionKeyRsaExtra& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionKeyRsaExtra& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionKeyRsaExtra);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionKeyRsaExtra
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const EncryptionKeyRsaExtra& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionKeyRsaExtra
bool operator==(const EncryptionKeyRsaExtra& lhs,
                const EncryptionKeyRsaExtra& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionKeyRsaExtra
bool operator!=(const EncryptionKeyRsaExtra& lhs,
                const EncryptionKeyRsaExtra& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionKeyRsaExtra
bool operator<(const EncryptionKeyRsaExtra& lhs,
               const EncryptionKeyRsaExtra& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionKeyRsaExtra
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                    algorithm,
                const EncryptionKeyRsaExtra& value);

template <typename HASH_ALGORITHM>
void EncryptionKeyRsaExtra::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_prime);
    hashAppend(algorithm, d_exponent);
    hashAppend(algorithm, d_coefficient);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                    algorithm,
                const EncryptionKeyRsaExtra& value)
{
    value.hash(algorithm);
}

/// Describe a RSA private key.
///
/// @details
/// TODO
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b version:
/// The version of the key.
///
/// @li @b modulus:
/// TODO. 'n'. This parameter is also part of the public portion of the key.
///
/// @li @b encryptionExponent:
/// TODO. 'e'. This parameter is also part of the public portion of the key.
///
/// @li @b decryptionExponent:
/// TODO. 'd'. 
///
/// @li @b prime1:
/// TODO. 'p'.
///
/// @li @b prime2:
/// TODO. 'q'.
///
/// @li @b exponent1:
/// TODO. 'd mod (p - 1)'.
///
/// @li @b exponent2:
/// TODO. 'd mod (q - 1)'.
///
/// @li @b coefficient:
/// TODO. '(inverse of q) mod p'.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par ASN.1 Schema
/// The ASN.1 definition of an RSA private key is defined in RFC 2313:
///
///     RSAPrivateKey ::= SEQUENCE {
///         version            Version,
///         modulus            INTEGER,
///         encryptionExponent INTEGER,
///         decryptionExponent INTEGER,
///         prime1             INTEGER,
///         prime2             INTEGER,
///         exponent1          INTEGER,
///         exponent2          INTEGER,
///         coefficient        INTEGER
///         otherPrimeInfos    OtherPrimeInfos OPTIONAL
///     }
///
///     OtherPrimeInfos ::= SEQUENCE SIZE(1..MAX) OF OtherPrimeInfo
///
///     OtherPrimeInfo ::= SEQUENCE {
///         prime       INTEGER,  -- ri
///         exponent    INTEGER,  -- di
///         coefficient INTEGER   -- ti
///     }
///
/// @ingroup module_ntci_encryption
class EncryptionKeyRsa
{
    enum Format {
        // The elliptic curve is encoded in the RsaPrivateKey format specified
        // in RFC 8017 (PKCS1).
        e_RSAPRIVATEKEY,

        // The elliptic curve is encoded in the PrivateKeyInfo format specified
        // in RFC 5208 (PKCS8).
        e_PRIVATEKEYINFO
    };

    typedef bsl::vector<ntca::EncryptionKeyRsaExtra> ExtraVector;

    ntsa::AbstractInteger      d_version;
    ntsa::AbstractInteger      d_modulus;             // n
    ntsa::AbstractInteger      d_encryptionExponent;  // e
    ntsa::AbstractInteger      d_decryptionExponent;  // d
    ntsa::AbstractInteger      d_prime1;              // p
    ntsa::AbstractInteger      d_prime2;              // q
    ntsa::AbstractInteger      d_exponent1;           // d mod (p-1)
    ntsa::AbstractInteger      d_exponent2;           // d mod (q-1)
    ntsa::AbstractInteger      d_coefficient;         // (inverse of q) mod p
    ExtraVector                d_extra;

  public:
    /// Create a new RSA key having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionKeyRsa(bslma::Allocator* basicAllocator = 0);

    /// Create a new RSA key having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionKeyRsa(const EncryptionKeyRsa& original,
                     bslma::Allocator*       basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionKeyRsa();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionKeyRsa& operator=(const EncryptionKeyRsa& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the version to the specified 'value'.
    void setVersion(const ntsa::AbstractInteger& value);

    /// Set the modulus to the specified 'value'.
    void setModulus(const ntsa::AbstractInteger& value);

    /// Set the public exponent to the specified 'value'.
    void setEncryptionExponent(const ntsa::AbstractInteger& value);

    /// Set the private exponent to the specified 'value'.
    void setDecryptionExponent(const ntsa::AbstractInteger& value);

    /// Set the first prime to the specified 'value'.
    void setPrime1(const ntsa::AbstractInteger& value);

    /// Set the second prime to the specified 'value'.
    void setPrime2(const ntsa::AbstractInteger& value);

    /// Set the first exponent to the specified 'value'.
    void setExponent1(const ntsa::AbstractInteger& value);

    /// Set the second exponent to the specified 'value'.
    void setExponent2(const ntsa::AbstractInteger& value);

    /// Set the coefficient to the specified 'value'.
    void setCoefficient(const ntsa::AbstractInteger& value);

    /// Set the extra primes to the specified 'value'.
    void setExtra(const bsl::vector<ntca::EncryptionKeyRsaExtra>& value);

    /// Add the specified 'value' to the other prime information.
    void addExtra(const ntca::EncryptionKeyRsaExtra& value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the version.
    const ntsa::AbstractInteger& version() const;

    /// Return the modulus.
    const ntsa::AbstractInteger& modulus() const;

    /// Return the public exponent.
    const ntsa::AbstractInteger& encryptionExponent() const;

    /// Return the private exponent.
    const ntsa::AbstractInteger& decryptionExponent() const;

    /// Return the frist prime.
    const ntsa::AbstractInteger& prime1() const;

    /// Return the second prime.
    const ntsa::AbstractInteger& prime2() const;

    /// Return the first exponent.
    const ntsa::AbstractInteger& exponent1() const;

    /// Return the second exponent.
    const ntsa::AbstractInteger& exponent2() const;

    /// Return the coefficient.
    const ntsa::AbstractInteger& coefficient() const;

    /// Return the other prime information.
    const bsl::vector<ntca::EncryptionKeyRsaExtra>& extra() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionKeyRsa& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionKeyRsa& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionKeyRsa);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionKeyRsa
bsl::ostream& operator<<(bsl::ostream& stream, const EncryptionKeyRsa& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionKeyRsa
bool operator==(const EncryptionKeyRsa& lhs, const EncryptionKeyRsa& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionKeyRsa
bool operator!=(const EncryptionKeyRsa& lhs, const EncryptionKeyRsa& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionKeyRsa
bool operator<(const EncryptionKeyRsa& lhs, const EncryptionKeyRsa& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionKeyRsa
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionKeyRsa& value);

template <typename HASH_ALGORITHM>
void EncryptionKeyRsa::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_version);
    hashAppend(algorithm, d_modulus);
    hashAppend(algorithm, d_encryptionExponent);
    hashAppend(algorithm, d_decryptionExponent);
    hashAppend(algorithm, d_prime1);
    hashAppend(algorithm, d_prime2);
    hashAppend(algorithm, d_exponent1);
    hashAppend(algorithm, d_exponent2);
    hashAppend(algorithm, d_coefficient);
    hashAppend(algorithm, d_extra);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                    algorithm,
                const EncryptionKeyRsa& value)
{
    value.hash(algorithm);
}













/// Enumerate the well-known encryption certificate public key algorithm types.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_data
class EncryptionKeyEllipticCurveParametersIdentifierType
{
  public:
    /// Enumerate the well-known encryption certificate public key algorithm
    /// types.
    enum Value {
        /// Elliptic curve with 256-bit field size and domain paraters chosen
        /// randomly. This enumerator corresponds to the object identifier
        /// 1.2.840.10045.3.1.7.
        e_SEC_P256_R1 = 0,

        /// Elliptic curve with 256-bit field size and domain paraters chosen
        /// randomly. This enumerator corresponds to the object identifier
        /// 1.3.132.0.34.
        e_SEC_P384_R1 = 1,

        /// Elliptic curve with 256-bit field size and domain paraters chosen
        /// randomly. This enumerator corresponds to the object identifier
        /// 1.3.132.0.35.
        e_SEC_P521_R1 = 2
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the object identifier corresponding
    /// to the specified 'value'.
    static void toObjectIdentifier(ntsa::AbstractObjectIdentifier* result,
                                   Value                           value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'. Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified 'result' the enumerator matching the specified
    /// object 'identifier'. Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'identifier' does not match any
    /// enumerator).
    static int fromObjectIdentifier(
        Value*                                result,
        const ntsa::AbstractObjectIdentifier& identifier);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionKeyEllipticCurveParametersIdentifierType
bsl::ostream& operator<<(
    bsl::ostream& stream,
    EncryptionKeyEllipticCurveParametersIdentifierType::
        Value rhs);

/// Describe an identifier for well-known elliptic curve algorithm parameters.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionKeyEllipticCurveParametersIdentifier
{
    ntsa::AbstractObjectIdentifier d_identifier;
    bslma::Allocator*              d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionKeyEllipticCurveParametersIdentifier(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionKeyEllipticCurveParametersIdentifier(
        const EncryptionKeyEllipticCurveParametersIdentifier&
                          original,
        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionKeyEllipticCurveParametersIdentifier();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionKeyEllipticCurveParametersIdentifier&
    operator=(
        const EncryptionKeyEllipticCurveParametersIdentifier&
            other);

    /// Assign the value of the specified 'value' to this object. Return a
    /// reference to this modifiable object.
    EncryptionKeyEllipticCurveParametersIdentifier&
    operator=(const ntsa::AbstractObjectIdentifier& value);

    /// Assign the value of the specified 'value' to this object. Return a
    /// reference to this modifiable object.
    EncryptionKeyEllipticCurveParametersIdentifier&
    operator=(
        EncryptionKeyEllipticCurveParametersIdentifierType::
            Value value);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the identifier to the specified 'value'.
    void setIdentifier(const ntsa::AbstractObjectIdentifier& value);

    /// Set the identifier to the object identifier corresponding to the
    /// specified 'value'.
    void setIdentifer(
        EncryptionKeyEllipticCurveParametersIdentifierType::
            Value value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the identifier.
    const ntsa::AbstractObjectIdentifier& identifier() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(
        const EncryptionKeyEllipticCurveParametersIdentifier&
            other) const;

    /// Return true if this object has the same value as the specified
    /// 'value', otherwise return false.
    bool equals(
        EncryptionKeyEllipticCurveParametersIdentifierType::
            Value value) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(
        const EncryptionKeyEllipticCurveParametersIdentifier&
            other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(
        EncryptionKeyEllipticCurveParametersIdentifier);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionKeyEllipticCurveParametersIdentifier
bsl::ostream& operator<<(
    bsl::ostream& stream,
    const EncryptionKeyEllipticCurveParametersIdentifier&
        object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionKeyEllipticCurveParametersIdentifier
bool operator==(
    const EncryptionKeyEllipticCurveParametersIdentifier&
        lhs,
    const EncryptionKeyEllipticCurveParametersIdentifier&
        rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionKeyEllipticCurveParametersIdentifier
bool operator!=(
    const EncryptionKeyEllipticCurveParametersIdentifier&
        lhs,
    const EncryptionKeyEllipticCurveParametersIdentifier&
        rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionKeyEllipticCurveParametersIdentifier
bool operator<(
    const EncryptionKeyEllipticCurveParametersIdentifier&
        lhs,
    const EncryptionKeyEllipticCurveParametersIdentifier&
        rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionKeyEllipticCurveParametersIdentifier
template <typename HASH_ALGORITHM>
void hashAppend(
    HASH_ALGORITHM& algorithm,
    const EncryptionKeyEllipticCurveParametersIdentifier&
        value);

template <typename HASH_ALGORITHM>
void EncryptionKeyEllipticCurveParametersIdentifier::
    hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_identifier);
}

template <typename HASH_ALGORITHM>
void hashAppend(
    HASH_ALGORITHM& algorithm,
    const EncryptionKeyEllipticCurveParametersIdentifier&
        value)
{
    value.hash(algorithm);
}

/// Describe parameters to an elliptic curve algorithm.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionKeyEllipticCurveParameters
{
    enum Type { e_UNDEFINED = -1, e_IDENTIFIER = 1, e_ANY = 2 };

    union {
        bsls::ObjectBuffer<
            EncryptionKeyEllipticCurveParametersIdentifier>
                                                d_identifier;
        bsls::ObjectBuffer<ntsa::AbstractValue> d_any;
    };

    Type              d_type;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionKeyEllipticCurveParameters(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionKeyEllipticCurveParameters(
        const EncryptionKeyEllipticCurveParameters&
                          original,
        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionKeyEllipticCurveParameters();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionKeyEllipticCurveParameters& operator=(
        const EncryptionKeyEllipticCurveParameters&
            other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Select the "identifier" representation. Return a reference to the
    /// modifiable representation.
    EncryptionKeyEllipticCurveParametersIdentifier&
    makeIdentifier();

    /// Select the "identifier" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    EncryptionKeyEllipticCurveParametersIdentifier&
    makeIdentifier(
        const EncryptionKeyEllipticCurveParametersIdentifier&
            value);

    /// Select the "any" representation. Return a reference to the modifiable
    /// representation.
    ntsa::AbstractValue& makeAny();

    /// Select the "any" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::AbstractValue& makeAny(const ntsa::AbstractValue& value);

    /// Return a reference to the modifiable "identifier" representation.
    /// The behavior is undefined unless 'isIdentifier()' is true.
    EncryptionKeyEllipticCurveParametersIdentifier&
    identifier();

    /// Return a reference to the modifiable "any" representation. The
    /// behavior is undefined unless 'isAny()' is true.
    ntsa::AbstractValue& any();

    /// Return a reference to the non-modifiable "identifier"
    /// representation. The behavior is undefined unless 'isIdentifier()'
    /// is true.
    const EncryptionKeyEllipticCurveParametersIdentifier&
    identifier() const;

    /// Return a reference to the non-modifiable "any" representation. The
    /// behavior is undefined unless 'isAny()' is true.
    const ntsa::AbstractValue& any() const;

    /// Return true if the representation is undefined, otherwise return false.
    bool isUndefined() const;

    /// Return true if the "identifier" representation is currently
    /// selected, otherwise return false.
    bool isIdentifier() const;

    /// Return true if the "any" representation is currently selected,
    /// otherwise return false.
    bool isAny() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(
        const EncryptionKeyEllipticCurveParameters&
            other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(
        const EncryptionKeyEllipticCurveParameters&
            other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    /// Format this object to the specified output 'stream' at the optionally
    /// specified indentation 'level' and return a reference to the modifiable
    /// 'stream'.  If 'level' is specified, optionally specify
    /// 'spacesPerLevel', the number of spaces per indentation level for this
    /// and all of its nested objects.  Each line is indented by the absolute
    /// value of 'level * spacesPerLevel'.  If 'level' is negative, suppress
    /// indentation of the first line.  If 'spacesPerLevel' is negative,
    /// suppress line breaks and format the entire output on one line.  If
    /// 'stream' is initially invalid, this operation has no effect.  Note that
    /// a trailing newline is provided in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(
        EncryptionKeyEllipticCurveParameters);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionKeyEllipticCurveParameters
bsl::ostream& operator<<(
    bsl::ostream& stream,
    const EncryptionKeyEllipticCurveParameters&
        object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionKeyEllipticCurveParameters
bool operator==(
    const EncryptionKeyEllipticCurveParameters& lhs,
    const EncryptionKeyEllipticCurveParameters& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionKeyEllipticCurveParameters
bool operator!=(
    const EncryptionKeyEllipticCurveParameters& lhs,
    const EncryptionKeyEllipticCurveParameters& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionKeyEllipticCurveParameters
bool operator<(
    const EncryptionKeyEllipticCurveParameters& lhs,
    const EncryptionKeyEllipticCurveParameters& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionKeyEllipticCurveParameters
template <typename HASH_ALGORITHM>
void hashAppend(
    HASH_ALGORITHM& algorithm,
    const EncryptionKeyEllipticCurveParameters&
        value);

template <typename HASH_ALGORITHM>
void EncryptionKeyEllipticCurveParameters::hash(
    HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    if (d_type == e_IDENTIFIER) {
        hashAppend(algorithm, d_identifier.object());
    }
    else if (d_type == e_ANY) {
        hashAppend(algorithm, d_any.object());
    }
}

template <typename HASH_ALGORITHM>
void hashAppend(
    HASH_ALGORITHM& algorithm,
    const EncryptionKeyEllipticCurveParameters&
        value)
{
    value.hash(algorithm);
}












/// Describe an elliptic curve private key.
///
/// @details
/// TODO
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b version:
/// The version of the key.
///
/// @li @b name:
/// The ASN.1 object identifier of the curve.
///
/// @li @b value:
/// The bytes of the private key data.
///
/// @li @b publicData:
/// The bytes of the public key data.
///
/// @li @b publicDataBits:
/// The number of bites defined in the public key data.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par ASN.1 Schema
/// The ASN.1 definition of an elliptic curve private key is defined in 
/// RFC 5915:
///
///     ECPrivateKey ::= SEQUENCE {
///         version        INTEGER { ecPrivkeyVer1(1) } (ecPrivkeyVer1),
///         privateKey     OCTET STRING,
///         parameters [0] ECParameters {{ NamedCurve }} OPTIONAL,
///         publicKey  [1] BIT STRING OPTIONAL
///     }
///
///     ECParameters ::= CHOICE {
///         namedCurve         OBJECT IDENTIFIER
///         -- implicitCurve   NULL
///         -- specifiedCurve  SpecifiedECDomain
///     }
///
/// @ingroup module_ntci_encryption
class EncryptionKeyEllipticCurve
{
    enum Format {
        // The elliptic curve is encoded in the ECPrivateKey format specified
        // in RFC 5915.
        e_ECPRIVATEKEY,

        // The elliptic curve is encoded in the PrivateKeyInfo format specified
        // in RFC 5208 (PKCS8).
        e_PRIVATEKEYINFO
    };

    typedef ntca::EncryptionKeyEllipticCurveParameters Parameters;

    ntsa::AbstractInteger                         d_version;
    ntsa::AbstractOctetString                     d_privateKey;
    bdlb::NullableValue<Parameters>               d_parameters;
    bdlb::NullableValue<ntsa::AbstractBitString>  d_publicKey;

  public:
    /// Create a new elliptic curve key having the default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit EncryptionKeyEllipticCurve(bslma::Allocator* basicAllocator = 0);

    /// Create a new elliptic curve key having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    EncryptionKeyEllipticCurve(const EncryptionKeyEllipticCurve& original,
                               bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionKeyEllipticCurve();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionKeyEllipticCurve& operator=(
        const EncryptionKeyEllipticCurve& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionKeyEllipticCurve& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionKeyEllipticCurve& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionKeyEllipticCurve);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionKeyEllipticCurve
bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const EncryptionKeyEllipticCurve& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionKeyEllipticCurve
bool operator==(const EncryptionKeyEllipticCurve& lhs,
                const EncryptionKeyEllipticCurve& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionKeyEllipticCurve
bool operator!=(const EncryptionKeyEllipticCurve& lhs,
                const EncryptionKeyEllipticCurve& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionKeyEllipticCurve
bool operator<(const EncryptionKeyEllipticCurve& lhs,
               const EncryptionKeyEllipticCurve& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionKeyEllipticCurve
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                   algorithm,
                const EncryptionKeyEllipticCurve& value);

template <typename HASH_ALGORITHM>
void EncryptionKeyEllipticCurve::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_version);
    hashAppend(algorithm, d_privateKey);
    hashAppend(algorithm, d_parameters);
    hashAppend(algorithm, d_publicKey);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                   algorithm,
                const EncryptionKeyEllipticCurve& value)
{
    value.hash(algorithm);
}

/// Describe the parameters to an encryption key generation operation.
///
/// The ASN.1 definition of an RSA private key is defined in RFC 2313:
///
/// RSAPrivateKey ::= SEQUENCE {
///     version Version,
///     modulus INTEGER,
///     encryptionExponent INTEGER,
///     decryptionExponent INTEGER,
///     prime1 INTEGER,
///     prime2 INTEGER,
///     exponent1 INTEGER,
///     exponent2 INTEGER,
///     coefficient INTEGER
///     otherPrimeInfos   OtherPrimeInfos OPTIONAL
/// }
///
/// ECPrivateKey ::= SEQUENCE {
///     version        INTEGER { ecPrivkeyVer1(1) } (ecPrivkeyVer1),
///     privateKey     OCTET STRING,
///     parameters [0] ECParameters {{ NamedCurve }} OPTIONAL,
///     publicKey  [1] BIT STRING OPTIONAL
/// }
///
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
///  Version ::= INTEGER
///  PrivateKeyAlgorithmIdentifier ::= AlgorithmIdentifier // From PKCS1 ?
///  PrivateKey ::= OCTET STRING // choice of RSAPrivateKey or ECPrivateKey
///
///  EncryptedPrivateKeyInfo ::= SEQUENCE {
///     encryptionAlgorithm EncryptionAlgorithmIdentifier,
///     encryptedData EncryptedData
///  }
///
///  EncryptionAlgorithmIdentifier ::= AlgorithmIdentifier // From PKCS5 ?
///  EncryptedData ::= OCTET STRING // Encrypted PrivateKeyInfo
///
/// Questions:
/// Symmetric encryption can be applied to either, or both, PEM and PKCS8?
/// How is the symmetric encryption method self-described by PEM?
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
class EncryptionKey
{
    enum Type { e_UNDEFINED, e_RSA, e_ELLIPTIC_CURVE };

    union {
        bsls::ObjectBuffer<ntca::EncryptionKeyRsa>           d_rsa;
        bsls::ObjectBuffer<ntca::EncryptionKeyEllipticCurve> d_ellipticCurve;
    };

    Type              d_type;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new key having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionKey(bslma::Allocator* basicAllocator = 0);

    /// Create a new key having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionKey(const EncryptionKey& original,
                  bslma::Allocator*    basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionKey();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionKey& operator=(const EncryptionKey& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Select the "rsa" representation. Return a reference to the modifiable
    /// representation.
    ntca::EncryptionKeyRsa& makeRsa();

    /// Select the "rsa" representation initially having the specified 'value'.
    /// Return a reference to the modifiable representation.
    ntca::EncryptionKeyRsa& makeRsa(const ntca::EncryptionKeyRsa& value);

    /// Select the "ellipticCurve" representation. Return a reference to the
    /// modifiable representation.
    ntca::EncryptionKeyEllipticCurve& makeEllipticCurve();

    /// Select the "ellipticCurve" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    ntca::EncryptionKeyEllipticCurve& makeEllipticCurve(
        const ntca::EncryptionKeyEllipticCurve& value);

    /// Return a reference to the modifiable "rsa" representation. The
    /// behavior is undefined unless 'isRsa()' is true.
    ntca::EncryptionKeyRsa& rsa();

    /// Return a reference to the modifiable "ellipticCurve" representation.
    /// The behavior is undefined unless 'isRsa()' is true.
    ntca::EncryptionKeyEllipticCurve& ellipticCurve();

    /// Return a reference to the non-modifiable "rsa" representation. The
    /// behavior is undefined unless 'isRsa()' is true.
    const ntca::EncryptionKeyRsa& rsa() const;

    /// Return a reference to the non-modifiable "ellipticCurve"
    /// representation. The behavior is undefined unless 'isRsa()' is true.
    const ntca::EncryptionKeyEllipticCurve& ellipticCurve() const;

    /// Return true if the representation is undefined, otherwise return false.
    bool isUndefined() const;

    /// Return true if the "rsa" representation is currently selected,
    /// otherwise return false.
    bool isRsa() const;

    /// Return true if the "ellipticCurve" representation is currently
    /// selected, otherwise return false.
    bool isEllipticCurve() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionKey& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionKey& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionKey);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionKey
bsl::ostream& operator<<(bsl::ostream& stream, const EncryptionKey& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionKey
bool operator==(const EncryptionKey& lhs, const EncryptionKey& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionKey
bool operator!=(const EncryptionKey& lhs, const EncryptionKey& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionKey
bool operator<(const EncryptionKey& lhs, const EncryptionKey& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionKey
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionKey& value);

template <typename HASH_ALGORITHM>
void EncryptionKey::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    if (this->isRsa()) {
        hashAppend(algorithm, this->rsa());
    }
    else if (this->isEllipticCurve()) {
        hashAppend(algorithm, this->ellipticCurve());
    }
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionKey& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
