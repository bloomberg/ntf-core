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

/// Describe a RSA private key.
///
/// @details
/// The ASN.1 definition of an RSA private key is defined in RFC 2313:
///
/// RSAPrivateKey ::= SEQUENCE {
///     version Version,
///     modulus INTEGER,
///     publicExponent INTEGER,
///     privateExponent INTEGER,
///     prime1 INTEGER,
///     prime2 INTEGER,
///     exponent1 INTEGER,
///     exponent2 INTEGER,
///     coefficient INTEGER
///     otherPrimeInfos   OtherPrimeInfos OPTIONAL
/// }
/// OtherPrimeInfos ::= SEQUENCE SIZE(1..MAX) OF OtherPrimeInfo
///
/// OtherPrimeInfo ::= SEQUENCE {
///     prime             INTEGER,  -- ri
///     exponent          INTEGER,  -- di
///     coefficient       INTEGER   -- ti
/// }
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b version:
/// The version of the key.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionKeyRsa
{
  public:
    typedef bsl::int64_t Number;

    struct Prime {
        Number d_prime;
        Number d_exponent;
        Number d_coefficient;

        friend bool operator==(const Prime& lhs, const Prime& rhs)
        {
            return lhs.d_prime == rhs.d_prime &&
                   lhs.d_exponent == rhs.d_exponent &&
                   lhs.d_coefficient == rhs.d_coefficient;
        }

        friend bool operator<(const Prime& lhs, const Prime& rhs)
        {
            if (lhs.d_prime < rhs.d_prime) {
                return true;
            }

            if (rhs.d_prime < lhs.d_prime) {
                return false;
            }

            if (lhs.d_exponent < rhs.d_exponent) {
                return true;
            }

            if (rhs.d_exponent < lhs.d_exponent) {
                return false;
            }

            return lhs.d_coefficient < rhs.d_coefficient;
        }
    };

    typedef bsl::vector<Prime> PrimeVector;

  private:
    Number      d_version;
    Number      d_modulus;          // n
    Number      d_publicExponent;   // rename to encryptionExponent
    Number      d_privateExponent;  // rename to decryptionExponent  
    Number      d_prime1;           // p?
    Number      d_prime2;           // q?
    Number      d_exponent1;
    Number      d_exponent2;
    Number      d_coefficient;
    PrimeVector d_extraPrimeInfo;

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
    void setVersion(Number value);

    /// Set the modulus to the specified 'value'.
    void setModulus(Number value);

    /// Set the public exponent to the specified 'value'.
    void setPublicExponent(Number value);

    /// Set the private exponent to the specified 'value'.
    void setPrivateExponent(Number value);

    /// Set the first prime to the specified 'value'.
    void setPrime1(Number value);

    /// Set the second prime to the specified 'value'.
    void setPrime2(Number value);

    /// Set the first exponent to the specified 'value'.
    void setExponent1(Number value);

    /// Set the second exponent to the specified 'value'.
    void setExponent2(Number value);

    /// Set the coefficient to the specified 'value'.
    void setCoefficient(Number value);

    /// Add the specified 'value' to the other prime information.
    void addExtraPrimeInfo(const Prime& value);

    /// Return the version.
    Number version() const;

    /// Return the modulus.
    Number modulus() const;

    /// Return the public exponent.
    Number publicExponent() const;

    /// Return the private exponent.
    Number privateExponent() const;

    /// Return the frist prime.
    Number prime1() const;

    /// Return the second prime.
    Number prime2() const;

    /// Return the first exponent.
    Number exponent1() const;

    /// Return the second exponent.
    Number exponent2() const;

    /// Return the coefficient.
    Number coefficient() const;

    /// Return the other prime information.
    const PrimeVector& extraPrimeInfo() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionKeyRsa& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionKeyRsa& other) const;

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

/// Describe an elliptic curve private key.
///
/// @details
/// ECPrivateKey ::= SEQUENCE {
///     version        INTEGER { ecPrivkeyVer1(1) } (ecPrivkeyVer1),
///     privateKey     OCTET STRING,
///     parameters [0] ECParameters {{ NamedCurve }} OPTIONAL,
///     publicKey  [1] BIT STRING OPTIONAL
/// }
///
/// ECParameters ::= CHOICE {
///     namedCurve         OBJECT IDENTIFIER
///     -- implicitCurve   NULL
///     -- specifiedCurve  SpecifiedECDomain
/// }
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
/// @ingroup module_ntci_encryption
class EncryptionKeyEllipticCurve
{
  public:
    typedef bsl::int64_t Number;

  private:
    Number                    d_version;
    bsl::vector<bsl::uint8_t> d_name;
    bsl::vector<bsl::uint8_t> d_privateKey;
    bsl::vector<bsl::uint8_t> d_publicKey;

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

    /// Set the version to the specified 'value'.
    void setVersion(Number value);

    /// Return the version.
    Number version() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionKeyEllipticCurve& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionKeyEllipticCurve& other) const;

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

/// Describe the parameters to an encryption key generation operation.
///
/// The ASN.1 definition of an RSA private key is defined in RFC 2313:
///
/// RSAPrivateKey ::= SEQUENCE {
///     version Version,
///     modulus INTEGER,
///     publicExponent INTEGER,
///     privateExponent INTEGER,
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
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionKeyRsa& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.version());
    hashAppend(algorithm, value.modulus());
    hashAppend(algorithm, value.publicExponent());
    hashAppend(algorithm, value.privateExponent());
    hashAppend(algorithm, value.prime1());
    hashAppend(algorithm, value.prime2());
    hashAppend(algorithm, value.exponent1());
    hashAppend(algorithm, value.exponent2());
    hashAppend(algorithm, value.coefficient());
    hashAppend(algorithm, value.extraPrimeInfo());
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                   algorithm,
                const EncryptionKeyEllipticCurve& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.version());
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionKey& value)
{
    using bslh::hashAppend;

    if (value.isRsa()) {
        hashAppend(algorithm, value.rsa());
    }
    else if (value.isEllipticCurve()) {
        hashAppend(algorithm, value.ellipticCurve());
    }
}

}  // close package namespace
}  // close enterprise namespace
#endif
