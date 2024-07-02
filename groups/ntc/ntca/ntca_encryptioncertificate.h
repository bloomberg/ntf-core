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

#include <ntca_encryptionkey.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_abstract.h>
#include <ntsa_ipaddress.h>
#include <ntsa_uri.h>
#include <bdlt_datetimetz.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntca {

/// Describe an encryption certificate structure with an unknown composition.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateObject
{
    ntsa::AbstractValue d_value;
    bslma::Allocator*   d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateObject(bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateObject(const EncryptionCertificateObject& original,
                                bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateObject();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateObject& operator=(
        const EncryptionCertificateObject& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateObject& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateObject& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateObject);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateObject
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const EncryptionCertificateObject& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateObject
bool operator==(const EncryptionCertificateObject& lhs,
                const EncryptionCertificateObject& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateObject
bool operator!=(const EncryptionCertificateObject& lhs,
                const EncryptionCertificateObject& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateObject
bool operator<(const EncryptionCertificateObject& lhs,
               const EncryptionCertificateObject& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateObject
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                    algorithm,
                const EncryptionCertificateObject& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateObject::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                    algorithm,
                const EncryptionCertificateObject& value)
{
    value.hash(algorithm);
}

/// Describe an encryption certificate version.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateVersion
{
    bsl::size_t       d_value;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateVersion(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateVersion(const EncryptionCertificateVersion& original,
                                 bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateVersion();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateVersion& operator=(
        const EncryptionCertificateVersion& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the value to the specified 'value'.
    void setValue(bsl::size_t value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the value.
    bsl::size_t value() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateVersion& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateVersion& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateVersion);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateVersion
bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const EncryptionCertificateVersion& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateVersion
bool operator==(const EncryptionCertificateVersion& lhs,
                const EncryptionCertificateVersion& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateVersion
bool operator!=(const EncryptionCertificateVersion& lhs,
                const EncryptionCertificateVersion& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateVersion
bool operator<(const EncryptionCertificateVersion& lhs,
               const EncryptionCertificateVersion& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateVersion
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                     algorithm,
                const EncryptionCertificateVersion& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateVersion::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                     algorithm,
                const EncryptionCertificateVersion& value)
{
    value.hash(algorithm);
}

/// Enumerate well-known encryption certificate name attribute object
/// identifiers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_data
class EncryptionCertificateNameAttributeIdentifierType
{
  public:
    /// Enumerate well-known encryption certificate name attribute object
    /// identifiers.
    enum Value {
        /// Title or prefix.
        e_NAME_PREFIX,

        /// Given name.
        e_NAME_GIVEN,

        /// Family name.
        e_NAME_FAMILY,

        /// Pseudo or nickname.
        e_NAME_PSEUDO,

        /// Generational suffix, e.g., "Jr.", or "III".
        e_NAME_SUFFIX,

        /// Common name.
        e_NAME_COMMON,

        /// Street number.
        e_ADDRESS_STREET,

        /// Locality, county, or municipality.
        e_ADDRESS_LOCALITY,

        /// State or province.
        e_ADDRESS_STATE,

        /// Country or nation.
        e_ADDRESS_COUNTRY,

        /// Company organizational division.
        e_ORGANIZATION,

        /// Company organizational subdivision.
        e_ORGANIZATION_UNIT,

        /// Domain name component.
        e_DOMAIN_COMPONENT,

        /// User identifier.
        e_USER_ID,

        // Electronic mail address.
        e_EMAIL
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
/// @related ntca::EncryptionCertificateNameAttributeIdentifierType
bsl::ostream& operator<<(
    bsl::ostream&                                           stream,
    EncryptionCertificateNameAttributeIdentifierType::Value rhs);

/// Describe an encryption certificate name attribute identifier.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateNameAttributeIdentifier
{
    ntsa::AbstractObjectIdentifier d_identifier;
    bslma::Allocator*              d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateNameAttributeIdentifier(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateNameAttributeIdentifier(
        const EncryptionCertificateNameAttributeIdentifier& original,
        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateNameAttributeIdentifier();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateNameAttributeIdentifier& operator=(
        const EncryptionCertificateNameAttributeIdentifier& other);

    /// Assign the value of the specified 'value' to this object. Return a
    /// reference to this modifiable object.
    EncryptionCertificateNameAttributeIdentifier& operator=(
        const ntsa::AbstractObjectIdentifier& value);

    /// Assign the value of the specified 'value' to this object. Return a
    /// reference to this modifiable object.
    EncryptionCertificateNameAttributeIdentifier& operator=(
        EncryptionCertificateNameAttributeIdentifierType::Value value);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the identifier to the specified 'value'.
    void setIdentifier(const ntsa::AbstractObjectIdentifier& value);

    /// Set the identifier to the object identifier corresponding to the
    /// specified 'value'.
    void setIdentifer(
        EncryptionCertificateNameAttributeIdentifierType::Value value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the identifier.
    const ntsa::AbstractObjectIdentifier& identifier() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(
        const EncryptionCertificateNameAttributeIdentifier& other) const;

    /// Return true if this object has the same value as the specified
    /// 'value', otherwise return false.
    bool equals(
        EncryptionCertificateNameAttributeIdentifierType::Value value) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateNameAttributeIdentifier& other) const;

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
        EncryptionCertificateNameAttributeIdentifier);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateNameAttributeIdentifier
bsl::ostream& operator<<(
    bsl::ostream&                                       stream,
    const EncryptionCertificateNameAttributeIdentifier& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateNameAttributeIdentifier
bool operator==(const EncryptionCertificateNameAttributeIdentifier& lhs,
                const EncryptionCertificateNameAttributeIdentifier& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateNameAttributeIdentifier
bool operator!=(const EncryptionCertificateNameAttributeIdentifier& lhs,
                const EncryptionCertificateNameAttributeIdentifier& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateNameAttributeIdentifier
bool operator<(const EncryptionCertificateNameAttributeIdentifier& lhs,
               const EncryptionCertificateNameAttributeIdentifier& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateNameAttributeIdentifier
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                     algorithm,
                const EncryptionCertificateNameAttributeIdentifier& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateNameAttributeIdentifier::hash(
    HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_identifier);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                     algorithm,
                const EncryptionCertificateNameAttributeIdentifier& value)
{
    value.hash(algorithm);
}

/// Describe a component of an encryption certificate name.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateNameAttribute
{
    EncryptionCertificateNameAttributeIdentifier d_attribute;
    ntsa::AbstractString                         d_value;
    bslma::Allocator*                            d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateNameAttribute(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateNameAttribute(
        const EncryptionCertificateNameAttribute& original,
        bslma::Allocator*                         basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateNameAttribute();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateNameAttribute& operator=(
        const EncryptionCertificateNameAttribute& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the attribute to the specified 'value'.
    void setAttribute(
        const EncryptionCertificateNameAttributeIdentifier& value);

    /// Set the attribute to the specified 'value'.
    void setAttribute(const ntsa::AbstractObjectIdentifier& value);

    /// Set the attribute to the object identifier corresponding to the
    /// specified 'value'.
    void setAttribute(
        EncryptionCertificateNameAttributeIdentifierType::Value value);

    /// Set the attribute value to the specified 'value'.
    void setValue(const bsl::string& value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the component attribute.
    const EncryptionCertificateNameAttributeIdentifier& attribute() const;

    /// Return the component value.
    bsl::string value() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateNameAttribute& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateNameAttribute& other) const;

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
        EncryptionCertificateNameAttribute);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateNameAttribute
bsl::ostream& operator<<(bsl::ostream&                             stream,
                         const EncryptionCertificateNameAttribute& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateNameAttribute
bool operator==(const EncryptionCertificateNameAttribute& lhs,
                const EncryptionCertificateNameAttribute& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateNameAttribute
bool operator!=(const EncryptionCertificateNameAttribute& lhs,
                const EncryptionCertificateNameAttribute& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateNameAttribute
bool operator<(const EncryptionCertificateNameAttribute& lhs,
               const EncryptionCertificateNameAttribute& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateNameAttribute
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                           algorithm,
                const EncryptionCertificateNameAttribute& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateNameAttribute::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_attribute);
    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                           algorithm,
                const EncryptionCertificateNameAttribute& value)
{
    value.hash(algorithm);
}

/// Describe an encryption certificate name.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateName
{
    typedef bsl::vector<EncryptionCertificateNameAttribute> AttributeVector;

    AttributeVector   d_attributeVector;
    bslma::Allocator* d_allocator_p;

  private:
    /// Append to the specified 'result' the values of each component having
    /// the specified well-known attribute 'type'. Separate each value by a
    /// comma followed by a space.
    void format(bsl::string* result,
                ntca::EncryptionCertificateNameAttributeIdentifierType::Value
                    type) const;

    /// Append to the specified 'result' the values of each component having
    /// the specified well-known attribute 'type'. Separate each value by a
    /// dot.
    void formatDot(
        bsl::string*                                                  result,
        ntca::EncryptionCertificateNameAttributeIdentifierType::Value type)
        const;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateName(bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateName(const EncryptionCertificateName& original,
                              bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateName();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateName& operator=(
        const EncryptionCertificateName& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the value to the specified 'value'.
    void setAttributeSequence(
        const bsl::vector<EncryptionCertificateNameAttribute>& value);

    /// Append a name component having the specified 'value'.
    void append(const EncryptionCertificateNameAttribute& value);

    /// Append a name component having the specified 'attribute' with the
    /// specified 'value'.
    void append(const EncryptionCertificateNameAttributeIdentifier& attribute,
                const bsl::string&                                  value);

    /// Append a name component having the specified 'attribute' with the
    /// specified 'value'.
    void append(const ntsa::AbstractObjectIdentifier& attribute,
                const bsl::string&                    value);

    /// Append a name component having the specified 'attribute' with the
    /// specified 'value'.
    void append(
        EncryptionCertificateNameAttributeIdentifierType::Value attribute,
        const bsl::string&                                      value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the attribute sequence.
    const bsl::vector<EncryptionCertificateNameAttribute>& attributes() const;

    /// Return the concatenation of all standard attributes, or the empty
    /// string if the name contains no standard attributes.
    bsl::string standard() const;

    /// Return the concatenation of all common name attributes, or the empty
    /// string if the name contains no common name attributes.
    bsl::string common() const;

    /// Return the concatenation of all prefix name attributes, or the empty
    /// string if the name contains no prefix name attributes.
    bsl::string prefix() const;

    /// Return the concatenation of all given name attributes, or the empty
    /// string if the name contains no given name attributes.
    bsl::string given() const;

    /// Return the concatenation of all family name attributes, or the empty
    /// string if the name contains no family name attributes.
    bsl::string family() const;

    /// Return the concatenation of all pseudo name attributes, or the empty
    /// string if the name contains no pseudo name attributes.
    bsl::string pseudo() const;

    /// Return the concatenation of all suffix name attributes, or the empty
    /// string if the name contains no suffix name attributes.
    bsl::string suffix() const;

    /// Return the concatenation of all organization attributes, or the empty
    /// string if the name contains no organization attributes.
    bsl::string organization() const;

    /// Return the concatenation of all organization unit attributes, or the
    /// empty string if the name contains no organization unit attributes.
    bsl::string organizationUnit() const;

    /// Return the concatenation of all street attributes, or the empty string
    /// if the name contains no street attributes.
    bsl::string addressStreet() const;

    /// Return the concatenation of all locality attributes, or the empty
    /// string if the name contains no locality attributes.
    bsl::string addressLocality() const;

    /// Return the concatenation of all state attributes, or the empty string
    /// if the name contains no state attributes.
    bsl::string addressState() const;

    /// Return the concatenation of all country attributes, or the empty string
    /// if the name contains no country attributes.
    bsl::string addressCountry() const;

    /// Return the concatenation of all domain attributes, or the empty string
    /// if the name contains no domain attributes.
    bsl::string domain() const;

    /// Return the concatenation of all user ID attributes, or the empty string
    /// if the name contains no user ID attributes.
    bsl::string userId() const;

    /// Return the concatenation of all email attributes, or the empty string
    /// if the name contains no email attributes.
    bsl::string email() const;

    /// Return true if the certificate is authentic for a request to the
    /// specified 'domainName', otherwise return false.
    bool matchesDomainName(const bsl::string& domainName) const;

    /// Return true if the certificate is authentic for a request to the
    /// specified 'ipAddress', otherwise return false.
    bool matchesIpAddress(const ntsa::IpAddress& ipAddress) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateName& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateName& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateName);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateName
bsl::ostream& operator<<(bsl::ostream&                    stream,
                         const EncryptionCertificateName& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateName
bool operator==(const EncryptionCertificateName& lhs,
                const EncryptionCertificateName& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateName
bool operator!=(const EncryptionCertificateName& lhs,
                const EncryptionCertificateName& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateName
bool operator<(const EncryptionCertificateName& lhs,
               const EncryptionCertificateName& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateName
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                  algorithm,
                const EncryptionCertificateName& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateName::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_attributeVector);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                  algorithm,
                const EncryptionCertificateName& value)
{
    value.hash(algorithm);
}

/// Describe an alternative name associated with an encryption certificate.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateNameAlternative
{
    enum Type {
        e_UNDEFINED  = -1,
        e_OTHER      = 0,
        e_EMAIL      = 1,
        e_DOMAIN     = 2,
        e_X400       = 3,
        e_DIRECTORY  = 4,
        e_EDI        = 5,
        e_URI        = 6,
        e_IP         = 7,
        e_IDENTIFIER = 8
    };

    union {
        bsls::ObjectBuffer<ntsa::AbstractValue>            d_other;
        bsls::ObjectBuffer<bsl::string>                    d_email;
        bsls::ObjectBuffer<bsl::string>                    d_domain;
        bsls::ObjectBuffer<ntsa::AbstractValue>            d_x400;
        bsls::ObjectBuffer<EncryptionCertificateName>      d_directory;
        bsls::ObjectBuffer<ntsa::AbstractValue>            d_edi;
        bsls::ObjectBuffer<ntsa::Uri>                      d_uri;
        bsls::ObjectBuffer<ntsa::IpAddress>                d_ip;
        bsls::ObjectBuffer<ntsa::AbstractObjectIdentifier> d_identifier;
    };

    Type              d_type;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateNameAlternative(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateNameAlternative(
        const EncryptionCertificateNameAlternative& original,
        bslma::Allocator*                           basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateNameAlternative();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateNameAlternative& operator=(
        const EncryptionCertificateNameAlternative& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Select the "other" representation. Return a reference to the modifiable
    /// representation.
    ntsa::AbstractValue& makeOther();

    /// Select the "other" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::AbstractValue& makeOther(const ntsa::AbstractValue& value);

    /// Select the "email" representation. Return a reference to the modifiable
    /// representation.
    bsl::string& makeEmail();

    /// Select the "email" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    bsl::string& makeEmail(const bsl::string& value);

    /// Select the "domain" representation. Return a reference to the
    /// modifiable representation.
    bsl::string& makeDomain();

    /// Select the "domain" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    bsl::string& makeDomain(const bsl::string& value);

    /// Select the "x400" representation. Return a reference to the modifiable
    /// representation.
    ntsa::AbstractValue& makeX400();

    /// Select the "x400" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::AbstractValue& makeX400(const ntsa::AbstractValue& value);

    /// Select the "directory" representation. Return a reference to the
    /// modifiable representation.
    EncryptionCertificateName& makeDirectory();

    /// Select the "directory" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    EncryptionCertificateName& makeDirectory(
        const EncryptionCertificateName& value);

    /// Select the "edi" representation. Return a reference to the modifiable
    /// representation.
    ntsa::AbstractValue& makeEdi();

    /// Select the "edi" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::AbstractValue& makeEdi(const ntsa::AbstractValue& value);

    /// Select the "uri" representation. Return a reference to the modifiable
    /// representation.
    ntsa::Uri& makeUri();

    /// Select the "uri" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::Uri& makeUri(const ntsa::Uri& value);

    /// Select the "ip" representation. Return a reference to the modifiable
    /// representation.
    ntsa::IpAddress& makeIp();

    /// Select the "ip" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::IpAddress& makeIp(const ntsa::IpAddress& value);

    /// Select the "identifier" representation. Return a reference to the
    /// modifiable representation.
    ntsa::AbstractObjectIdentifier& makeIdentifier();

    /// Select the "identifier" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::AbstractObjectIdentifier& makeIdentifier(
        const ntsa::AbstractObjectIdentifier& value);

    /// Return a reference to the modifiable "other" representation. The
    /// behavior is undefined unless 'isOther()' is true.
    ntsa::AbstractValue& other();

    /// Return a reference to the modifiable "email" representation. The
    /// behavior is undefined unless 'isEmail()' is true.
    bsl::string& email();

    /// Return a reference to the modifiable "domain" representation. The
    /// behavior is undefined unless 'isDomain()' is true.
    bsl::string& domain();

    /// Return a reference to the modifiable "X400" representation. The
    /// behavior is undefined unless 'isX400()' is true.
    ntsa::AbstractValue& x400();

    /// Return a reference to the modifiable "directory" representation. The
    /// behavior is undefined unless 'isDirectory()' is true.
    EncryptionCertificateName& directory();

    /// Return a reference to the modifiable "edi" representation. The
    /// behavior is undefined unless 'isEdi()' is true.
    ntsa::AbstractValue& edi();

    /// Return a reference to the modifiable "uri" representation. The
    /// behavior is undefined unless 'isUri()' is true.
    ntsa::Uri& uri();

    /// Return a reference to the modifiable "ip" representation. The
    /// behavior is undefined unless 'isIp()' is true.
    ntsa::IpAddress& ip();

    /// Return a reference to the modifiable "identifier" representation. The
    /// behavior is undefined unless 'isIdentifier()' is true.
    ntsa::AbstractObjectIdentifier& identifier();

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return a reference to the non-modifiable "other" representation. The
    /// behavior is undefined unless 'isOther()' is true.
    const ntsa::AbstractValue& other() const;

    /// Return a reference to the non-modifiable "email" representation. The
    /// behavior is undefined unless 'isEmail()' is true.
    const bsl::string& email() const;

    /// Return a reference to the non-modifiable "domain" representation. The
    /// behavior is undefined unless 'isDomain()' is true.
    const bsl::string& domain() const;

    /// Return a reference to the non-modifiable "X400" representation. The
    /// behavior is undefined unless 'isX400()' is true.
    const ntsa::AbstractValue& x400() const;

    /// Return a reference to the non-modifiable "directory" representation.
    /// The behavior is undefined unless 'isDirectory()' is true.
    const EncryptionCertificateName& directory() const;

    /// Return a reference to the non-modifiable "edi" representation. The
    /// behavior is undefined unless 'isEdi()' is true.
    const ntsa::AbstractValue& edi() const;

    /// Return a reference to the non-modifiable "uri" representation. The
    /// behavior is undefined unless 'isUri()' is true.
    const ntsa::Uri& uri() const;

    /// Return a reference to the non-modifiable "ip" representation. The
    /// behavior is undefined unless 'isIp()' is true.
    const ntsa::IpAddress& ip() const;

    /// Return a reference to the non-modifiable "identifier" representation.
    /// The behavior is undefined unless 'isIdentifier()' is true.
    const ntsa::AbstractObjectIdentifier& identifier() const;

    /// Return true if the representation is undefined, otherwise return false.
    bool isUndefined() const;

    /// Return true if the "other" representation is currently selected,
    /// otherwise return false.
    bool isOther() const;

    /// Return true if the "email" representation is currently selected,
    /// otherwise return false.
    bool isEmail() const;

    /// Return true if the "domain" representation is currently selected,
    /// otherwise return false.
    bool isDomain() const;

    /// Return true if the "x400" representation is currently selected,
    /// otherwise return false.
    bool isX400() const;

    /// Return true if the "directory" representation is currently selected,
    /// otherwise return false.
    bool isDirectory() const;

    /// Return true if the "edi" representation is currently selected,
    /// otherwise return false.
    bool isEdi() const;

    /// Return true if the "uri" representation is currently selected,
    /// otherwise return false.
    bool isUri() const;

    /// Return true if the "ip" representation is currently selected,
    /// otherwise return false.
    bool isIp() const;

    /// Return true if the "identifier" representation is currently selected,
    /// otherwise return false.
    bool isIdentifier() const;

    /// Return true if the certificate is authentic for a request to the
    /// specified 'domainName', otherwise return false.
    bool matchesDomainName(const bsl::string& domainName) const;

    /// Return true if the certificate is authentic for a request to the
    /// specified 'ipAddress', otherwise return false.
    bool matchesIpAddress(const ntsa::IpAddress& ipAddress) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateNameAlternative& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateNameAlternative& other) const;

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
        EncryptionCertificateNameAlternative);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateNameAlternative
bsl::ostream& operator<<(bsl::ostream&                               stream,
                         const EncryptionCertificateNameAlternative& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateNameAlternative
bool operator==(const EncryptionCertificateNameAlternative& lhs,
                const EncryptionCertificateNameAlternative& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateNameAlternative
bool operator!=(const EncryptionCertificateNameAlternative& lhs,
                const EncryptionCertificateNameAlternative& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateNameAlternative
bool operator<(const EncryptionCertificateNameAlternative& lhs,
               const EncryptionCertificateNameAlternative& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateNameAlternative
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                             algorithm,
                const EncryptionCertificateNameAlternative& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateNameAlternative::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    if (d_type == e_OTHER) {
        hashAppend(algorithm, d_other.object());
    }
    else if (d_type == e_EMAIL) {
        hashAppend(algorithm, d_email.object());
    }
    else if (d_type == e_DOMAIN) {
        hashAppend(algorithm, d_domain.object());
    }
    else if (d_type == e_X400) {
        hashAppend(algorithm, d_x400.object());
    }
    else if (d_type == e_DIRECTORY) {
        hashAppend(algorithm, d_directory.object());
    }
    else if (d_type == e_EDI) {
        hashAppend(algorithm, d_edi.object());
    }
    else if (d_type == e_URI) {
        hashAppend(algorithm, d_uri.object());
    }
    else if (d_type == e_IP) {
        hashAppend(algorithm, d_ip.object());
    }
    else if (d_type == e_IDENTIFIER) {
        hashAppend(algorithm, d_identifier.object());
    }
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                             algorithm,
                const EncryptionCertificateNameAlternative& value)
{
    value.hash(algorithm);
}

/// Describe a collection of alternative names associated with an encryption
/// certificate.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateNameAlternativeList
{
    bsl::vector<EncryptionCertificateNameAlternative> d_container;
    bslma::Allocator*                                 d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateNameAlternativeList(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateNameAlternativeList(
        const EncryptionCertificateNameAlternativeList& original,
        bslma::Allocator*                               basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateNameAlternativeList();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateNameAlternativeList& operator=(
        const EncryptionCertificateNameAlternativeList& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the vector of name alternatives.
    const bsl::vector<EncryptionCertificateNameAlternative>& alternatives()
        const;

    /// Return true if the certificate is authentic for a request to the
    /// specified 'domainName', otherwise return false.
    bool matchesDomainName(const bsl::string& domainName) const;

    /// Return true if the certificate is authentic for a request to the
    /// specified 'ipAddress', otherwise return false.
    bool matchesIpAddress(const ntsa::IpAddress& ipAddress) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateNameAlternativeList& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateNameAlternativeList& other) const;

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
        EncryptionCertificateNameAlternativeList);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateNameAlternativeList
bsl::ostream& operator<<(
    bsl::ostream&                                   stream,
    const EncryptionCertificateNameAlternativeList& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateNameAlternativeList
bool operator==(const EncryptionCertificateNameAlternativeList& lhs,
                const EncryptionCertificateNameAlternativeList& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateNameAlternativeList
bool operator!=(const EncryptionCertificateNameAlternativeList& lhs,
                const EncryptionCertificateNameAlternativeList& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateNameAlternativeList
bool operator<(const EncryptionCertificateNameAlternativeList& lhs,
               const EncryptionCertificateNameAlternativeList& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateNameAlternativeList
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                 algorithm,
                const EncryptionCertificateNameAlternativeList& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateNameAlternativeList::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_container);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                 algorithm,
                const EncryptionCertificateNameAlternativeList& value)
{
    value.hash(algorithm);
}

/// Describe constraints on an encryption certificate name.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateNameConstraints
{
    ntsa::AbstractValue d_value;
    bslma::Allocator*   d_allocator_p;

  public:
    /// Create new certificate name constraints having the default value.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit EncryptionCertificateNameConstraints(
        bslma::Allocator* basicAllocator = 0);

    /// Create new certificate name constraints having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    EncryptionCertificateNameConstraints(
        const EncryptionCertificateNameConstraints& original,
        bslma::Allocator*                           basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateNameConstraints();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateNameConstraints& operator=(
        const EncryptionCertificateNameConstraints& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateNameConstraints& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateNameConstraints& other) const;

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
        EncryptionCertificateNameConstraints);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateNameConstraints
bsl::ostream& operator<<(bsl::ostream&                               stream,
                         const EncryptionCertificateNameConstraints& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateNameConstraints
bool operator==(const EncryptionCertificateNameConstraints& lhs,
                const EncryptionCertificateNameConstraints& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateNameConstraints
bool operator!=(const EncryptionCertificateNameConstraints& lhs,
                const EncryptionCertificateNameConstraints& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateNameConstraints
bool operator<(const EncryptionCertificateNameConstraints& lhs,
               const EncryptionCertificateNameConstraints& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateNameConstraints
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                             algorithm,
                const EncryptionCertificateNameConstraints& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateNameConstraints::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                             algorithm,
                const EncryptionCertificateNameConstraints& value)
{
    value.hash(algorithm);
}

/// Describe a date/time interval in which a certificate is valid.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateValidity
{
    ntsa::AbstractSyntaxTagNumber::Value d_fromTagNumber;
    bdlt::DatetimeTz                     d_from;
    ntsa::AbstractSyntaxTagNumber::Value d_thruTagNumber;
    bdlt::DatetimeTz                     d_thru;
    bslma::Allocator*                    d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateValidity(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateValidity(
        const EncryptionCertificateValidity& original,
        bslma::Allocator*                    basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateValidity();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateValidity& operator=(
        const EncryptionCertificateValidity& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the start time from which the certificate is valid to the
    /// specified 'value'.
    void setFrom(const bdlt::DatetimeTz& value);

    /// Set the expiration time after which the certificate is no longer valid
    /// to the specified 'value'.
    void setThru(const bdlt::DatetimeTz& value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the start time from which the certificate is valid.
    const bdlt::DatetimeTz& from() const;

    /// Return the expiration time after which the certificate is no longer
    /// valid.
    const bdlt::DatetimeTz& thru() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateValidity& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateValidity& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateValidity);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateValidity
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateValidity& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateValidity
bool operator==(const EncryptionCertificateValidity& lhs,
                const EncryptionCertificateValidity& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateValidity
bool operator!=(const EncryptionCertificateValidity& lhs,
                const EncryptionCertificateValidity& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateValidity
bool operator<(const EncryptionCertificateValidity& lhs,
               const EncryptionCertificateValidity& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateValidity
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateValidity& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateValidity::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_from);
    hashAppend(algorithm, d_thru);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateValidity& value)
{
    value.hash(algorithm);
}

/// Enumerate the well-known encryption certificate signature algorithm types.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_data
class EncryptionCertificateSignatureAlgorithmIdentifierType
{
  public:
    /// Enumerate the well-known encryption certificate signature algorithm
    /// types.
    enum Value {
        /// Message Digest 2 (MD2) checksum with Rivest, Shamir, and Adleman
        /// (RSA) encryption.
        e_MD2_RSA,

        /// Message Digest 4 (MD4) checksum with Rivest, Shamir, and Adleman
        /// (RSA) encryption.
        e_MD4_RSA,

        /// Message Digest 5 (MD5) checksum with Rivest, Shamir, and Adleman
        /// (RSA) encryption.
        e_MD5_RSA,

        /// PKCS1 version 1.5 signature algorithm with Secure Hash Algorithm 1
        /// (SHA-1) and Rivest, Shamir, and Adleman (RSA) encryption.
        e_SHA1_RSA,

        /// PKCS1 version 1.5 signature algorithm with Secure Hash Algorithm
        /// 256 (SHA-256) and Rivest, Shamir, and Adleman (RSA) encryption.
        e_SHA256_RSA,

        /// PKCS1 version 1.5 signature algorithm with Secure Hash Algorithm
        /// 384 (SHA-384) and Rivest, Shamir, and Adleman (RSA) encryption.
        e_SHA384_RSA,

        /// PKCS1 version 1.5 signature algorithm with Secure Hash Algorithm
        /// 512 (SHA-512) and Rivest, Shamir, and Adleman (RSA) encryption.
        e_SHA512_RSA,

        /// ANSI X9.62 Elliptic Curve Digital Signature Algorithm (ECDSA)
        /// coupled with the Secure Hash Algorithm 1 (SHA-1). This algorithm
        /// is assigned the object identifier 1.2.840.10045.4.1.
        e_ECDSA_SHA1,

        /// ANSI X9.62 Elliptic Curve Digital Signature Algorithm (ECDSA)
        /// coupled with the Secure Hash Algorithm 224 (SHA-224). This
        /// algorithm is assigned the object identifier 1.2.840.10045.4.3.1.
        e_ECDSA_SHA224,

        /// ANSI X9.62 Elliptic Curve Digital Signature Algorithm (ECDSA)
        /// coupled with the Secure Hash Algorithm 256 (SHA-256). This
        /// algorithm is assigned the object identifier 1.2.840.10045.4.3.2.
        e_ECDSA_SHA256,

        /// ANSI X9.62 Elliptic Curve Digital Signature Algorithm (ECDSA)
        /// coupled with the Secure Hash Algorithm 256 (SHA-256). This
        /// algorithm is assigned the object identifier 1.2.840.10045.4.3.3.
        e_ECDSA_SHA384,

        /// ANSI X9.62 Elliptic Curve Digital Signature Algorithm (ECDSA)
        /// coupled with the Secure Hash Algorithm 256 (SHA-256). This
        /// algorithm is assigned the object identifier 1.2.840.10045.4.3.4.
        e_ECDSA_SHA512,

        /// Edwards curve 25591. This enumerator corresponds to the object
        /// identifier 1.3.101.112.
        e_EDDSA_25519,

        /// Edwards curve 448. This enumerator corresponds to the object
        /// identifier 1.3.101.113.
        e_EDDSA_448
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
/// @related ntca::EncryptionCertificateSignatureAlgorithmIdentifierType
bsl::ostream& operator<<(
    bsl::ostream&                                                stream,
    EncryptionCertificateSignatureAlgorithmIdentifierType::Value rhs);

/// Describe an encryption certificate signature algorithm.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateSignatureAlgorithmIdentifier
{
    ntsa::AbstractObjectIdentifier d_identifier;
    bslma::Allocator*              d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateSignatureAlgorithmIdentifier(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateSignatureAlgorithmIdentifier(
        const EncryptionCertificateSignatureAlgorithmIdentifier& original,
        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateSignatureAlgorithmIdentifier();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateSignatureAlgorithmIdentifier& operator=(
        const EncryptionCertificateSignatureAlgorithmIdentifier& other);

    /// Assign the value of the specified 'value' to this object. Return a
    /// reference to this modifiable object.
    EncryptionCertificateSignatureAlgorithmIdentifier& operator=(
        const ntsa::AbstractObjectIdentifier& value);

    /// Assign the value of the specified 'value' to this object. Return a
    /// reference to this modifiable object.
    EncryptionCertificateSignatureAlgorithmIdentifier& operator=(
        EncryptionCertificateSignatureAlgorithmIdentifierType::Value value);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the identifier to the specified 'value'.
    void setIdentifier(const ntsa::AbstractObjectIdentifier& value);

    /// Set the identifier to the object identifier corresponding to the
    /// specified 'value'.
    void setIdentifer(
        EncryptionCertificateSignatureAlgorithmIdentifierType::Value value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the identifier.
    const ntsa::AbstractObjectIdentifier& identifier() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(
        const EncryptionCertificateSignatureAlgorithmIdentifier& other) const;

    /// Return true if this object has the same value as the specified
    /// 'value', otherwise return false.
    bool equals(EncryptionCertificateSignatureAlgorithmIdentifierType::Value
                    value) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(
        const EncryptionCertificateSignatureAlgorithmIdentifier& other) const;

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
        EncryptionCertificateSignatureAlgorithmIdentifier);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateSignatureAlgorithmIdentifier
bsl::ostream& operator<<(
    bsl::ostream&                                            stream,
    const EncryptionCertificateSignatureAlgorithmIdentifier& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateSignatureAlgorithmIdentifier
bool operator==(const EncryptionCertificateSignatureAlgorithmIdentifier& lhs,
                const EncryptionCertificateSignatureAlgorithmIdentifier& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateSignatureAlgorithmIdentifier
bool operator!=(const EncryptionCertificateSignatureAlgorithmIdentifier& lhs,
                const EncryptionCertificateSignatureAlgorithmIdentifier& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateSignatureAlgorithmIdentifier
bool operator<(const EncryptionCertificateSignatureAlgorithmIdentifier& lhs,
               const EncryptionCertificateSignatureAlgorithmIdentifier& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateSignatureAlgorithmIdentifier
template <typename HASH_ALGORITHM>
void hashAppend(
    HASH_ALGORITHM&                                          algorithm,
    const EncryptionCertificateSignatureAlgorithmIdentifier& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateSignatureAlgorithmIdentifier::hash(
    HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_identifier);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm,
                const EncryptionCertificateSignatureAlgorithmIdentifier& value)
{
    value.hash(algorithm);
}

/// Describe an encryption certificate signature algorithm parameters.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateSignatureAlgorithmParameters
{
    ntsa::AbstractValue d_value;
    bslma::Allocator*   d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateSignatureAlgorithmParameters(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateSignatureAlgorithmParameters(
        const EncryptionCertificateSignatureAlgorithmParameters& original,
        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateSignatureAlgorithmParameters();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateSignatureAlgorithmParameters& operator=(
        const EncryptionCertificateSignatureAlgorithmParameters& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(
        const EncryptionCertificateSignatureAlgorithmParameters& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(
        const EncryptionCertificateSignatureAlgorithmParameters& other) const;

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
        EncryptionCertificateSignatureAlgorithmParameters);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateSignatureAlgorithmParameters
bsl::ostream& operator<<(
    bsl::ostream&                                            stream,
    const EncryptionCertificateSignatureAlgorithmParameters& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateSignatureAlgorithmParameters
bool operator==(const EncryptionCertificateSignatureAlgorithmParameters& lhs,
                const EncryptionCertificateSignatureAlgorithmParameters& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateSignatureAlgorithmParameters
bool operator!=(const EncryptionCertificateSignatureAlgorithmParameters& lhs,
                const EncryptionCertificateSignatureAlgorithmParameters& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateSignatureAlgorithmParameters
bool operator<(const EncryptionCertificateSignatureAlgorithmParameters& lhs,
               const EncryptionCertificateSignatureAlgorithmParameters& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateSignatureAlgorithmParameters
template <typename HASH_ALGORITHM>
void hashAppend(
    HASH_ALGORITHM&                                          algorithm,
    const EncryptionCertificateSignatureAlgorithmParameters& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateSignatureAlgorithmParameters::hash(
    HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm,
                const EncryptionCertificateSignatureAlgorithmParameters& value)
{
    value.hash(algorithm);
}

/// Describe an encryption certificate signature algorithm.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateSignatureAlgorithm
{
    EncryptionCertificateSignatureAlgorithmIdentifier d_identifier;
    bdlb::NullableValue<EncryptionCertificateSignatureAlgorithmParameters>
                      d_parameters;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateSignatureAlgorithm(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateSignatureAlgorithm(
        const EncryptionCertificateSignatureAlgorithm& original,
        bslma::Allocator*                              basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateSignatureAlgorithm();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateSignatureAlgorithm& operator=(
        const EncryptionCertificateSignatureAlgorithm& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the identifier to the specified 'value'.
    void setIdentifier(
        const EncryptionCertificateSignatureAlgorithmIdentifier& value);

    /// Set the parameters to the specified 'value'.
    void setParameters(
        const EncryptionCertificateSignatureAlgorithmParameters& value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the identifier.
    const EncryptionCertificateSignatureAlgorithmIdentifier& identifier()
        const;

    /// Return the parameters.
    const bdlb::NullableValue<
        EncryptionCertificateSignatureAlgorithmParameters>&
    parameters() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateSignatureAlgorithm& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateSignatureAlgorithm& other) const;

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
        EncryptionCertificateSignatureAlgorithm);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateSignatureAlgorithm
bsl::ostream& operator<<(
    bsl::ostream&                                  stream,
    const EncryptionCertificateSignatureAlgorithm& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateSignatureAlgorithm
bool operator==(const EncryptionCertificateSignatureAlgorithm& lhs,
                const EncryptionCertificateSignatureAlgorithm& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateSignatureAlgorithm
bool operator!=(const EncryptionCertificateSignatureAlgorithm& lhs,
                const EncryptionCertificateSignatureAlgorithm& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateSignatureAlgorithm
bool operator<(const EncryptionCertificateSignatureAlgorithm& lhs,
               const EncryptionCertificateSignatureAlgorithm& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateSignatureAlgorithm
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                algorithm,
                const EncryptionCertificateSignatureAlgorithm& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateSignatureAlgorithm::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_identifier);
    hashAppend(algorithm, d_parameters);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                algorithm,
                const EncryptionCertificateSignatureAlgorithm& value)
{
    value.hash(algorithm);
}

/// Describe an encryption certificate signature.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateSignature
{
    ntsa::AbstractBitString d_value;
    bslma::Allocator*       d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateSignature(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateSignature(
        const EncryptionCertificateSignature& original,
        bslma::Allocator*                     basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateSignature();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateSignature& operator=(
        const EncryptionCertificateSignature& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the value to the specified 'value'.
    void setValue(const ntsa::AbstractBitString& value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the value.
    const ntsa::AbstractBitString& value() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateSignature& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateSignature& other) const;

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
        EncryptionCertificateSignature);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateSignature
bsl::ostream& operator<<(bsl::ostream&                         stream,
                         const EncryptionCertificateSignature& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateSignature
bool operator==(const EncryptionCertificateSignature& lhs,
                const EncryptionCertificateSignature& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateSignature
bool operator!=(const EncryptionCertificateSignature& lhs,
                const EncryptionCertificateSignature& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateSignature
bool operator<(const EncryptionCertificateSignature& lhs,
               const EncryptionCertificateSignature& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateSignature
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                       algorithm,
                const EncryptionCertificateSignature& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateSignature::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                       algorithm,
                const EncryptionCertificateSignature& value)
{
    value.hash(algorithm);
}

/// Describe an encryption certificate policy constraints.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificatePolicyConstraints
{
    ntsa::AbstractValue d_value;
    bslma::Allocator*   d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificatePolicyConstraints(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificatePolicyConstraints(
        const EncryptionCertificatePolicyConstraints& original,
        bslma::Allocator*                             basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificatePolicyConstraints();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificatePolicyConstraints& operator=(
        const EncryptionCertificatePolicyConstraints& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificatePolicyConstraints& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificatePolicyConstraints& other) const;

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
        EncryptionCertificatePolicyConstraints);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificatePolicyConstraints
bsl::ostream& operator<<(bsl::ostream&                                 stream,
                         const EncryptionCertificatePolicyConstraints& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificatePolicyConstraints
bool operator==(const EncryptionCertificatePolicyConstraints& lhs,
                const EncryptionCertificatePolicyConstraints& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificatePolicyConstraints
bool operator!=(const EncryptionCertificatePolicyConstraints& lhs,
                const EncryptionCertificatePolicyConstraints& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificatePolicyConstraints
bool operator<(const EncryptionCertificatePolicyConstraints& lhs,
               const EncryptionCertificatePolicyConstraints& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificatePolicyConstraints
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                               algorithm,
                const EncryptionCertificatePolicyConstraints& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificatePolicyConstraints::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                               algorithm,
                const EncryptionCertificatePolicyConstraints& value)
{
    value.hash(algorithm);
}

/// Describe an encryption certificate policy mapping.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificatePolicyMappings
{
    ntsa::AbstractValue d_value;
    bslma::Allocator*   d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificatePolicyMappings(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificatePolicyMappings(
        const EncryptionCertificatePolicyMappings& original,
        bslma::Allocator*                          basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificatePolicyMappings();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificatePolicyMappings& operator=(
        const EncryptionCertificatePolicyMappings& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificatePolicyMappings& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificatePolicyMappings& other) const;

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
        EncryptionCertificatePolicyMappings);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificatePolicyMappings
bsl::ostream& operator<<(bsl::ostream&                              stream,
                         const EncryptionCertificatePolicyMappings& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificatePolicyMappings
bool operator==(const EncryptionCertificatePolicyMappings& lhs,
                const EncryptionCertificatePolicyMappings& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificatePolicyMappings
bool operator!=(const EncryptionCertificatePolicyMappings& lhs,
                const EncryptionCertificatePolicyMappings& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificatePolicyMappings
bool operator<(const EncryptionCertificatePolicyMappings& lhs,
               const EncryptionCertificatePolicyMappings& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificatePolicyMappings
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                            algorithm,
                const EncryptionCertificatePolicyMappings& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificatePolicyMappings::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                            algorithm,
                const EncryptionCertificatePolicyMappings& value)
{
    value.hash(algorithm);
}

/// Describe an encryption certificate policy set.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificatePolicy
{
    ntsa::AbstractValue d_value;
    bslma::Allocator*   d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificatePolicy(bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificatePolicy(const EncryptionCertificatePolicy& original,
                                bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificatePolicy();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificatePolicy& operator=(
        const EncryptionCertificatePolicy& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificatePolicy& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificatePolicy& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificatePolicy);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificatePolicy
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const EncryptionCertificatePolicy& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificatePolicy
bool operator==(const EncryptionCertificatePolicy& lhs,
                const EncryptionCertificatePolicy& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificatePolicy
bool operator!=(const EncryptionCertificatePolicy& lhs,
                const EncryptionCertificatePolicy& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificatePolicy
bool operator<(const EncryptionCertificatePolicy& lhs,
               const EncryptionCertificatePolicy& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificatePolicy
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                    algorithm,
                const EncryptionCertificatePolicy& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificatePolicy::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                    algorithm,
                const EncryptionCertificatePolicy& value)
{
    value.hash(algorithm);
}

/// Describe an encryption certificate issuer key identifier.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateIssuerKeyIdentifier
{
    ntsa::AbstractValue d_value;
    bslma::Allocator*   d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateIssuerKeyIdentifier(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateIssuerKeyIdentifier(
        const EncryptionCertificateIssuerKeyIdentifier& original,
        bslma::Allocator*                               basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateIssuerKeyIdentifier();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateIssuerKeyIdentifier& operator=(
        const EncryptionCertificateIssuerKeyIdentifier& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateIssuerKeyIdentifier& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateIssuerKeyIdentifier& other) const;

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
        EncryptionCertificateIssuerKeyIdentifier);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateIssuerKeyIdentifier
bsl::ostream& operator<<(
    bsl::ostream&                                   stream,
    const EncryptionCertificateIssuerKeyIdentifier& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateIssuerKeyIdentifier
bool operator==(const EncryptionCertificateIssuerKeyIdentifier& lhs,
                const EncryptionCertificateIssuerKeyIdentifier& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateIssuerKeyIdentifier
bool operator!=(const EncryptionCertificateIssuerKeyIdentifier& lhs,
                const EncryptionCertificateIssuerKeyIdentifier& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateIssuerKeyIdentifier
bool operator<(const EncryptionCertificateIssuerKeyIdentifier& lhs,
               const EncryptionCertificateIssuerKeyIdentifier& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateIssuerKeyIdentifier
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                 algorithm,
                const EncryptionCertificateIssuerKeyIdentifier& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateIssuerKeyIdentifier::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                 algorithm,
                const EncryptionCertificateIssuerKeyIdentifier& value)
{
    value.hash(algorithm);
}

/// Describe an encryption certificate issuer information access.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateIssuerInformationAccess
{
    ntsa::AbstractValue d_value;
    bslma::Allocator*   d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateIssuerInformationAccess(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateIssuerInformationAccess(
        const EncryptionCertificateIssuerInformationAccess& original,
        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateIssuerInformationAccess();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateIssuerInformationAccess& operator=(
        const EncryptionCertificateIssuerInformationAccess& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(
        const EncryptionCertificateIssuerInformationAccess& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateIssuerInformationAccess& other) const;

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
        EncryptionCertificateIssuerInformationAccess);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateIssuerInformationAccess
bsl::ostream& operator<<(
    bsl::ostream&                                       stream,
    const EncryptionCertificateIssuerInformationAccess& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateIssuerInformationAccess
bool operator==(const EncryptionCertificateIssuerInformationAccess& lhs,
                const EncryptionCertificateIssuerInformationAccess& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateIssuerInformationAccess
bool operator!=(const EncryptionCertificateIssuerInformationAccess& lhs,
                const EncryptionCertificateIssuerInformationAccess& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateIssuerInformationAccess
bool operator<(const EncryptionCertificateIssuerInformationAccess& lhs,
               const EncryptionCertificateIssuerInformationAccess& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateIssuerInformationAccess
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                     algorithm,
                const EncryptionCertificateIssuerInformationAccess& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateIssuerInformationAccess::hash(
    HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                     algorithm,
                const EncryptionCertificateIssuerInformationAccess& value)
{
    value.hash(algorithm);
}

/// Describe an encryption certificate issuer.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateIssuer
{
    ntca::EncryptionCertificateName d_name;
    bslma::Allocator*               d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateIssuer(bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateIssuer(const EncryptionCertificateIssuer& original,
                                bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateIssuer();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateIssuer& operator=(
        const EncryptionCertificateIssuer& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the name to the specified 'value'.
    void setName(const ntca::EncryptionCertificateName& value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the name.
    const ntca::EncryptionCertificateName& name() const;

    /// Return true if the certificate is authentic for a request to the
    /// specified 'domainName', otherwise return false.
    bool matchesDomainName(const bsl::string& domainName) const;

    /// Return true if the certificate is authentic for a request to the
    /// specified 'ipAddress', otherwise return false.
    bool matchesIpAddress(const ntsa::IpAddress& ipAddress) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateIssuer& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateIssuer& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateIssuer);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateIssuer
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const EncryptionCertificateIssuer& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateIssuer
bool operator==(const EncryptionCertificateIssuer& lhs,
                const EncryptionCertificateIssuer& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateIssuer
bool operator!=(const EncryptionCertificateIssuer& lhs,
                const EncryptionCertificateIssuer& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateIssuer
bool operator<(const EncryptionCertificateIssuer& lhs,
               const EncryptionCertificateIssuer& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateIssuer
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                    algorithm,
                const EncryptionCertificateIssuer& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateIssuer::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_name);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                    algorithm,
                const EncryptionCertificateIssuer& value)
{
    value.hash(algorithm);
}

/// Enumerate well-known encryption certificate subject extended key usage
/// object identifiers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_data
class EncryptionCertificateSubjectKeyUsageExtendedType
{
  public:
    /// Enumerate well-known encryption certificate extension object
    /// identifiers.
    enum Value {
        /// TLS client.
        e_TLS_CLIENT,

        /// TLS server.
        e_TLS_SERVER,

        /// SSH client.
        e_SSH_CLIENT,

        /// SSH server.
        e_SSH_SERVER,

        /// IPSEC user.
        e_IPSEC_USER,

        // IPSEC tunnel.
        e_IPSEC_TUNNEL,

        // IPSEC endpoint.
        e_IPSEC_ENDPOINT,

        // PKIX key purpose timestamping.
        e_KEY_TIMESTAMPING,

        // Code signature.
        e_CODE_SIGNING,

        // Email authenticiation.
        e_EMAIL
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
/// @related ntca::EncryptionCertificateSubjectKeyUsageExtendedType
bsl::ostream& operator<<(
    bsl::ostream&                                           stream,
    EncryptionCertificateSubjectKeyUsageExtendedType::Value rhs);

/// Describe encryption certificate subject extended key usage.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateSubjectKeyUsageExtended
{
    bsl::vector<ntsa::AbstractObjectIdentifier> d_identifiers;
    bslma::Allocator*                           d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateSubjectKeyUsageExtended(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateSubjectKeyUsageExtended(
        const EncryptionCertificateSubjectKeyUsageExtended& original,
        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateSubjectKeyUsageExtended();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateSubjectKeyUsageExtended& operator=(
        const EncryptionCertificateSubjectKeyUsageExtended& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the identifier list to the specified 'value'.
    void setIdentifierList(
        const bsl::vector<ntsa::AbstractObjectIdentifier>& value);

    /// Add the specified 'value' to the identifier list.
    void addIdentifier(const ntsa::AbstractObjectIdentifier& value);

    /// Add the identifier corresponding to the specified 'value' to the
    /// identifier list.
    void addIdentifier(
        EncryptionCertificateSubjectKeyUsageExtendedType::Value value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the identifier list.
    const bsl::vector<ntsa::AbstractObjectIdentifier>& identifierList() const;

    /// Return true if the extended usage contains the specified 'identifier',
    /// otherwise return false.
    bool allows(const ntsa::AbstractObjectIdentifier& identifier) const;

    /// Return true if the extended usage contains the identifier corresponding
    /// to the specified 'value', otherwise return false.
    bool allows(
        EncryptionCertificateSubjectKeyUsageExtendedType::Value value) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(
        const EncryptionCertificateSubjectKeyUsageExtended& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateSubjectKeyUsageExtended& other) const;

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
        EncryptionCertificateSubjectKeyUsageExtended);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateSubjectKeyUsageExtended
bsl::ostream& operator<<(
    bsl::ostream&                                       stream,
    const EncryptionCertificateSubjectKeyUsageExtended& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateSubjectKeyUsageExtended
bool operator==(const EncryptionCertificateSubjectKeyUsageExtended& lhs,
                const EncryptionCertificateSubjectKeyUsageExtended& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateSubjectKeyUsageExtended
bool operator!=(const EncryptionCertificateSubjectKeyUsageExtended& lhs,
                const EncryptionCertificateSubjectKeyUsageExtended& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateSubjectKeyUsageExtended
bool operator<(const EncryptionCertificateSubjectKeyUsageExtended& lhs,
               const EncryptionCertificateSubjectKeyUsageExtended& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateSubjectKeyUsageExtended
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                     algorithm,
                const EncryptionCertificateSubjectKeyUsageExtended& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateSubjectKeyUsageExtended::hash(
    HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_identifiers);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                     algorithm,
                const EncryptionCertificateSubjectKeyUsageExtended& value)
{
    value.hash(algorithm);
}

/// Enumerate subject key usage types.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_data
class EncryptionCertificateSubjectKeyUsageType
{
  public:
    /// Enumerate subject key usage types.
    enum Value {
        /// The subject's key can be used to verify digital signatures that
        /// aren't explicitly signatures in certicates and certificate
        /// revocation lists.
        e_DIGITAL_SIGNATURE = 0,

        /// The subject's key can be used to verify digital signatures that
        /// aren't explicitly signatures in certicates and certificate
        /// revocation lists to provide non-repudiation, i.e., to prevent a
        /// signer from falsly denying their signature.
        e_CONTENT_COMMITMENT = 1,

        /// The subject's key can be used to encipher private keys during the
        /// transporation of keys.
        e_KEY_ENCIPHER = 2,

        /// The subject's key can be used to encipher user data without the
        /// use of an intermediate symmetric cipher.
        e_DATA_ENCIPHER = 3,

        /// The subject's key can be used for key agreement.
        e_KEY_AGREEMENT = 4,

        /// The subject's key can be used to verify signatures on certificates.
        e_KEY_CERTFICATE_SIGNATURE = 5,

        /// The subject's key can be used to verify signatures on certificate
        /// revocation lists.
        e_CERTIFICATE_REVOCATION_LIST_SIGNATURE = 6,

        /// The subject's key can only be used to encipher data during key
        /// agreement.
        e_ENCIPHER_ONLY = 7,

        /// The subject's key can only be used to decipher data during key
        /// agreement.
        e_DECIPHER_ONLY = 8
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'. Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateSubjectKeyUsageType
bsl::ostream& operator<<(bsl::ostream& stream,
                         EncryptionCertificateSubjectKeyUsageType::Value rhs);

/// Describe encryption certificate subject key usage.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateSubjectKeyUsage
{
    ntsa::AbstractBitString d_value;
    bslma::Allocator*       d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateSubjectKeyUsage(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateSubjectKeyUsage(
        const EncryptionCertificateSubjectKeyUsage& original,
        bslma::Allocator*                           basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateSubjectKeyUsage();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateSubjectKeyUsage& operator=(
        const EncryptionCertificateSubjectKeyUsage& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Enable the usage of the specified 'value' type.
    void enable(EncryptionCertificateSubjectKeyUsageType::Value value);

    /// Disable the usage of the specified 'value' type.
    void disable(EncryptionCertificateSubjectKeyUsageType::Value value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return true if the usage supports the specified 'value' type, otherwise
    /// return false.
    bool allows(EncryptionCertificateSubjectKeyUsageType::Value value) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateSubjectKeyUsage& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateSubjectKeyUsage& other) const;

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
        EncryptionCertificateSubjectKeyUsage);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateSubjectKeyUsage
bsl::ostream& operator<<(bsl::ostream&                               stream,
                         const EncryptionCertificateSubjectKeyUsage& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateSubjectKeyUsage
bool operator==(const EncryptionCertificateSubjectKeyUsage& lhs,
                const EncryptionCertificateSubjectKeyUsage& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateSubjectKeyUsage
bool operator!=(const EncryptionCertificateSubjectKeyUsage& lhs,
                const EncryptionCertificateSubjectKeyUsage& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateSubjectKeyUsage
bool operator<(const EncryptionCertificateSubjectKeyUsage& lhs,
               const EncryptionCertificateSubjectKeyUsage& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateSubjectKeyUsage
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                             algorithm,
                const EncryptionCertificateSubjectKeyUsage& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateSubjectKeyUsage::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                             algorithm,
                const EncryptionCertificateSubjectKeyUsage& value)
{
    value.hash(algorithm);
}

/// Describe an encryption certificate subject key identifier.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateSubjectKeyIdentifier
{
    ntsa::AbstractValue d_value;
    bslma::Allocator*   d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateSubjectKeyIdentifier(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateSubjectKeyIdentifier(
        const EncryptionCertificateSubjectKeyIdentifier& original,
        bslma::Allocator*                                basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateSubjectKeyIdentifier();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateSubjectKeyIdentifier& operator=(
        const EncryptionCertificateSubjectKeyIdentifier& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateSubjectKeyIdentifier& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateSubjectKeyIdentifier& other) const;

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
        EncryptionCertificateSubjectKeyIdentifier);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateSubjectKeyIdentifier
bsl::ostream& operator<<(
    bsl::ostream&                                    stream,
    const EncryptionCertificateSubjectKeyIdentifier& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateSubjectKeyIdentifier
bool operator==(const EncryptionCertificateSubjectKeyIdentifier& lhs,
                const EncryptionCertificateSubjectKeyIdentifier& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateSubjectKeyIdentifier
bool operator!=(const EncryptionCertificateSubjectKeyIdentifier& lhs,
                const EncryptionCertificateSubjectKeyIdentifier& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateSubjectKeyIdentifier
bool operator<(const EncryptionCertificateSubjectKeyIdentifier& lhs,
               const EncryptionCertificateSubjectKeyIdentifier& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateSubjectKeyIdentifier
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                  algorithm,
                const EncryptionCertificateSubjectKeyIdentifier& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateSubjectKeyIdentifier::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                  algorithm,
                const EncryptionCertificateSubjectKeyIdentifier& value)
{
    value.hash(algorithm);
}

/// Describe encryption certificate subject (i.e. basic) contraints.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateSubjectConstraints
{
    bdlb::NullableValue<bool>        d_authority;
    bdlb::NullableValue<bsl::size_t> d_pathLength;
    bslma::Allocator*                d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateSubjectConstraints(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateSubjectConstraints(
        const EncryptionCertificateSubjectConstraints& original,
        bslma::Allocator*                              basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateSubjectConstraints();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateSubjectConstraints& operator=(
        const EncryptionCertificateSubjectConstraints& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the flag indicating the subject may act as a certificate authority
    /// to the specified 'value'.
    void setAuthority(bool value);

    /// Set the maximum number of certificates to follow through the chain
    /// of trust when verifying a certificate signed by the subject to the
    /// specified 'value'.
    void setPathLength(bsl::size_t value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the flag indicating the subject may act as a certificate
    /// authority, or null to indicate the subject may not act as a certificate
    /// authority.
    const bdlb::NullableValue<bool>& authority() const;

    /// Return the maximum number of certificates to follow through the chain
    /// of trust when verifying a certificate signed by the subject, or null to
    /// indicate the path length is unlimited.
    const bdlb::NullableValue<bsl::size_t>& pathLength() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateSubjectConstraints& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateSubjectConstraints& other) const;

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
        EncryptionCertificateSubjectConstraints);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateSubjectConstraints
bsl::ostream& operator<<(
    bsl::ostream&                                  stream,
    const EncryptionCertificateSubjectConstraints& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateSubjectConstraints
bool operator==(const EncryptionCertificateSubjectConstraints& lhs,
                const EncryptionCertificateSubjectConstraints& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateSubjectConstraints
bool operator!=(const EncryptionCertificateSubjectConstraints& lhs,
                const EncryptionCertificateSubjectConstraints& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateSubjectConstraints
bool operator<(const EncryptionCertificateSubjectConstraints& lhs,
               const EncryptionCertificateSubjectConstraints& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateSubjectConstraints
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                algorithm,
                const EncryptionCertificateSubjectConstraints& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateSubjectConstraints::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_authority);
    hashAppend(algorithm, d_pathLength);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                algorithm,
                const EncryptionCertificateSubjectConstraints& value)
{
    value.hash(algorithm);
}

/// Describe an encryption certificate subject.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateSubject
{
    ntca::EncryptionCertificateName d_name;
    bslma::Allocator*               d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateSubject(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateSubject(const EncryptionCertificateSubject& original,
                                 bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateSubject();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateSubject& operator=(
        const EncryptionCertificateSubject& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the name to the specified 'value'.
    void setName(const ntca::EncryptionCertificateName& value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the name.
    const ntca::EncryptionCertificateName& name() const;

    /// Return true if the certificate is authentic for a request to the
    /// specified 'domainName', otherwise return false.
    bool matchesDomainName(const bsl::string& domainName) const;

    /// Return true if the certificate is authentic for a request to the
    /// specified 'ipAddress', otherwise return false.
    bool matchesIpAddress(const ntsa::IpAddress& ipAddress) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateSubject& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateSubject& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateSubject);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateSubject
bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const EncryptionCertificateSubject& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateSubject
bool operator==(const EncryptionCertificateSubject& lhs,
                const EncryptionCertificateSubject& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateSubject
bool operator!=(const EncryptionCertificateSubject& lhs,
                const EncryptionCertificateSubject& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateSubject
bool operator<(const EncryptionCertificateSubject& lhs,
               const EncryptionCertificateSubject& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateSubject
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                     algorithm,
                const EncryptionCertificateSubject& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateSubject::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_name);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                     algorithm,
                const EncryptionCertificateSubject& value)
{
    value.hash(algorithm);
}

/// Enumerate well-known encryption certificate extension object identifiers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_data
class EncryptionCertificateExtensionIdentifierType
{
  public:
    /// Enumerate well-known encryption certificate extension object
    /// identifiers.
    enum Value {
        /// Subject alternative name.
        e_SUBJECT_ALTERNATIVE_NAME,

        /// Subject key identifier.
        e_SUBJECT_KEY_IDENTIFIER,

        /// Subject key usage.
        e_SUBJECT_KEY_USAGE,

        /// Subject key usage extension list.
        e_SUBJECT_KEY_USAGE_EXTENDED,

        /// Subject constraints (i.e. basic constraints).
        e_SUBJECT_CONSTRAINTS,

        /// Issuer alternative name.
        e_ISSUER_ALTERNATIVE_NAME,

        /// Issuer key identifier.
        e_ISSUER_KEY_IDENTIFIER,

        /// Issuer information access.
        e_ISSUER_INFORMATION_ACCESS,

        /// Policies.
        e_POLICY,

        /// Policy mappings.
        e_POLICY_MAPPINGS,

        /// Policy constraints.
        e_POLICY_CONSTRAINTS
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
/// @related ntca::EncryptionCertificateExtensionIdentifierType
bsl::ostream& operator<<(
    bsl::ostream&                                       stream,
    EncryptionCertificateExtensionIdentifierType::Value rhs);

/// Describe an encryption certificate extension attribute.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateExtensionIdentifier
{
    ntsa::AbstractObjectIdentifier d_identifier;
    bslma::Allocator*              d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateExtensionIdentifier(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateExtensionIdentifier(
        const EncryptionCertificateExtensionIdentifier& original,
        bslma::Allocator*                               basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateExtensionIdentifier();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateExtensionIdentifier& operator=(
        const EncryptionCertificateExtensionIdentifier& other);

    /// Assign the value of the specified 'value' to this object. Return a
    /// reference to this modifiable object.
    EncryptionCertificateExtensionIdentifier& operator=(
        const ntsa::AbstractObjectIdentifier& value);

    /// Assign the value of the specified 'value' to this object. Return a
    /// reference to this modifiable object.
    EncryptionCertificateExtensionIdentifier& operator=(
        EncryptionCertificateExtensionIdentifierType::Value value);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the identifier to the specified 'value'.
    void setIdentifier(const ntsa::AbstractObjectIdentifier& value);

    /// Set the identifier to the object identifier corresponding to the
    /// specified 'value'.
    void setIdentifer(
        EncryptionCertificateExtensionIdentifierType::Value value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the identifier.
    const ntsa::AbstractObjectIdentifier& identifier() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateExtensionIdentifier& other) const;

    /// Return true if this object has the same value as the specified
    /// 'value', otherwise return false.
    bool equals(
        EncryptionCertificateExtensionIdentifierType::Value value) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateExtensionIdentifier& other) const;

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
        EncryptionCertificateExtensionIdentifier);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateExtensionIdentifier
bsl::ostream& operator<<(
    bsl::ostream&                                   stream,
    const EncryptionCertificateExtensionIdentifier& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateExtensionIdentifier
bool operator==(const EncryptionCertificateExtensionIdentifier& lhs,
                const EncryptionCertificateExtensionIdentifier& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateExtensionIdentifier
bool operator!=(const EncryptionCertificateExtensionIdentifier& lhs,
                const EncryptionCertificateExtensionIdentifier& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateExtensionIdentifier
bool operator<(const EncryptionCertificateExtensionIdentifier& lhs,
               const EncryptionCertificateExtensionIdentifier& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateExtensionIdentifier
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                 algorithm,
                const EncryptionCertificateExtensionIdentifier& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateExtensionIdentifier::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_identifier);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                                 algorithm,
                const EncryptionCertificateExtensionIdentifier& value)
{
    value.hash(algorithm);
}

/// Describe an encryption certificate extension value.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateExtensionValue
{
    enum Type {
        // The extension value is undefined.
        e_UNDEFINED = -1,

        // The extension value represents a boolean.
        e_BOOLEAN = 0,

        // The extension value represents a subject or issuer name.
        e_NAME = 1,

        // The extension value represents a subject or issuer name alternative.
        e_NAME_ALTERNATIVE = 2,

        // The extension value represents a certficate subject.
        e_SUBJECT = 3,

        // The extension value represents a certficate subject key identifier.
        e_SUBJECT_KEY_IDENTIFIER = 4,

        // The extension value represents subject key usage.
        e_SUBJECT_KEY_USAGE = 5,

        // The extension value represents subject key usage extensions.
        e_SUBJECT_KEY_USAGE_EXTENDED = 6,

        // The extension value represents subject constraints.
        e_SUBJECT_CONSTRAINTS = 7,

        // The extension value represents a certficate issuer.
        e_ISSUER = 8,

        // The extension value represents a certficate issuer key identifier.
        e_ISSUER_KEY_IDENTIFIER = 9,

        // The extension value represents a certificate issuer information.
        e_ISSUER_INFORMATION_ACCESS = 10,

        // The extension value represents certficate policies.
        e_POLICY = 11,

        // The extension value represents certficate policy mappings.
        e_POLICY_MAPPINGS = 12,

        // The extension value represents certficate policy constraints.
        e_POLICY_CONSTRAINTS = 13,

        // The extension value represents an octet string.
        e_BYTE_SEQUENCE = 14,

        // The extension value represents any ASN.1 encoded value.
        e_ANY = 15
    };

    union {
        bsls::ObjectBuffer<bool>                      d_boolean;
        bsls::ObjectBuffer<EncryptionCertificateName> d_name;
        bsls::ObjectBuffer<EncryptionCertificateNameAlternativeList>
            d_nameAlternative;

        bsls::ObjectBuffer<EncryptionCertificateSubject> d_subject;
        bsls::ObjectBuffer<EncryptionCertificateSubjectKeyIdentifier>
            d_subjectKeyIdentifier;
        bsls::ObjectBuffer<EncryptionCertificateSubjectKeyUsage>
            d_subjectKeyUsage;
        bsls::ObjectBuffer<EncryptionCertificateSubjectKeyUsageExtended>
            d_subjectKeyUsageExtended;
        bsls::ObjectBuffer<EncryptionCertificateSubjectConstraints>
            d_subjectConstraints;

        bsls::ObjectBuffer<EncryptionCertificateIssuer> d_issuer;
        bsls::ObjectBuffer<EncryptionCertificateIssuerKeyIdentifier>
            d_issuerKeyIdentifier;
        bsls::ObjectBuffer<EncryptionCertificateIssuerInformationAccess>
            d_issuerInformationAccess;

        bsls::ObjectBuffer<EncryptionCertificatePolicy> d_policy;
        bsls::ObjectBuffer<EncryptionCertificatePolicyMappings>
            d_policyMappings;
        bsls::ObjectBuffer<EncryptionCertificatePolicyConstraints>
            d_policyConstraints;

        bsls::ObjectBuffer<ntsa::AbstractOctetString> d_byteSequence;
        bsls::ObjectBuffer<ntsa::AbstractValue>       d_any;
    };

    Type              d_type;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateExtensionValue(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateExtensionValue(
        const EncryptionCertificateExtensionValue& original,
        bslma::Allocator*                          basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateExtensionValue();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateExtensionValue& operator=(
        const EncryptionCertificateExtensionValue& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Select the "boolean" representation. Return a reference to the
    /// modifiable representation.
    bool& makeBoolean();

    /// Select the "boolean" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    bool& makeBoolean(bool value);

    /// Select the "name" representation. Return a reference to the modifiable
    /// representation.
    EncryptionCertificateName& makeName();

    /// Select the "name" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    EncryptionCertificateName& makeName(
        const EncryptionCertificateName& value);

    /// Select the "nameAlternative" representation. Return a reference to the
    /// modifiable representation.
    EncryptionCertificateNameAlternativeList& makeNameAlternative();

    /// Select the "nameAlternative" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    EncryptionCertificateNameAlternativeList& makeNameAlternative(
        const EncryptionCertificateNameAlternativeList& value);

    /// Select the "subject" representation. Return a reference to the
    /// modifiable representation.
    EncryptionCertificateSubject& makeSubject();

    /// Select the "subject" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    EncryptionCertificateSubject& makeSubject(
        const EncryptionCertificateSubject& value);

    /// Select the "subjectKeyIdentifier" representation. Return a reference to
    /// the modifiable representation.
    EncryptionCertificateSubjectKeyIdentifier& makeSubjectKeyIdentifier();

    /// Select the "subjectKeyIdentifier" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    EncryptionCertificateSubjectKeyIdentifier& makeSubjectKeyIdentifier(
        const EncryptionCertificateSubjectKeyIdentifier& value);

    /// Select the "subjectKeyUsage" representation. Return a reference to the
    /// modifiable representation.
    EncryptionCertificateSubjectKeyUsage& makeSubjectKeyUsage();

    /// Select the "subjectKeyUsage" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    EncryptionCertificateSubjectKeyUsage& makeSubjectKeyUsage(
        const EncryptionCertificateSubjectKeyUsage& value);

    /// Select the "subjectKeyUsageExtended" representation. Return a reference
    /// to the modifiable representation.
    EncryptionCertificateSubjectKeyUsageExtended& makeSubjectKeyUsageExtended();

    /// Select the "subjectKeyUsageExtended" representation initially having
    /// the specified 'value'. Return a reference to the modifiable
    /// representation.
    EncryptionCertificateSubjectKeyUsageExtended& makeSubjectKeyUsageExtended(
        const EncryptionCertificateSubjectKeyUsageExtended& value);

    /// Select the "subjectConstraints" representation. Return a reference to
    /// the modifiable representation.
    EncryptionCertificateSubjectConstraints& makeSubjectConstraints();

    /// Select the "subjectConstraints" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    EncryptionCertificateSubjectConstraints& makeSubjectConstraints(
        const EncryptionCertificateSubjectConstraints& value);

    /// Select the "issuer" representation. Return a reference to the
    /// modifiable representation.
    EncryptionCertificateIssuer& makeIssuer();

    /// Select the "issuer" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    EncryptionCertificateIssuer& makeIssuer(
        const EncryptionCertificateIssuer& value);

    /// Select the "issuerKeyIdentifier" representation. Return a reference to
    /// the modifiable representation.
    EncryptionCertificateIssuerKeyIdentifier& makeIssuerKeyIdentifier();

    /// Select the "issuerKeyIdentifier" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    EncryptionCertificateIssuerKeyIdentifier& makeIssuerKeyIdentifier(
        const EncryptionCertificateIssuerKeyIdentifier& value);

    /// Select the "issuerInformationAccess" representation. Return a reference
    /// to the modifiable representation.
    EncryptionCertificateIssuerInformationAccess& makeIssuerInformationAccess();

    /// Select the "issuerInformationAccess" representation initially having
    /// the specified 'value'. Return a reference to the modifiable
    /// representation.
    EncryptionCertificateIssuerInformationAccess& makeIssuerInformationAccess(
        const EncryptionCertificateIssuerInformationAccess& value);

    /// Select the "policy" representation. Return a reference to the
    /// modifiable representation.
    EncryptionCertificatePolicy& makePolicy();

    /// Select the "policy" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    EncryptionCertificatePolicy& makePolicy(
        const EncryptionCertificatePolicy& value);

    /// Select the "policyMappings" representation. Return a reference to the
    /// modifiable representation.
    EncryptionCertificatePolicyMappings& makePolicyMappings();

    /// Select the "policyMappings" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    EncryptionCertificatePolicyMappings& makePolicyMappings(
        const EncryptionCertificatePolicyMappings& value);

    /// Select the "policyConstraints" representation. Return a reference to
    /// the modifiable representation.
    EncryptionCertificatePolicyConstraints& makePolicyConstraints();

    /// Select the "policyConstraints" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    EncryptionCertificatePolicyConstraints& makePolicyConstraints(
        const EncryptionCertificatePolicyConstraints& value);

    /// Select the "byteSequence" representation. Return a reference to the
    /// modifiable representation.
    ntsa::AbstractOctetString& makeByteSequence();

    /// Select the "byteSequence" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::AbstractOctetString& makeByteSequence(
        const ntsa::AbstractOctetString& value);

    /// Select the "any" representation. Return a reference to the modifiable
    /// representation.
    ntsa::AbstractValue& makeAny();

    /// Select the "any" representation initially having the specified 'value'.
    /// Return a reference to the modifiable representation.
    ntsa::AbstractValue& makeAny(const ntsa::AbstractValue& value);

    /// Return a reference to the modifiable "boolean" representation. The
    /// behavior is undefined unless 'isBoolean()' is true.
    bool& boolean();

    /// Return a reference to the modifiable "name" representation.
    /// The behavior is undefined unless 'isName()' is true.
    EncryptionCertificateName& name();

    /// Return a reference to the modifiable "nameAlternative" representation.
    /// The behavior is undefined unless 'isNameAlternative()' is true.
    EncryptionCertificateNameAlternativeList& nameAlternative();

    /// Return a reference to the modifiable "subject" representation.
    /// The behavior is undefined unless 'isSubject()' is true.
    EncryptionCertificateSubject& subject();

    /// Return a reference to the modifiable "subjectKeyIdentifier"
    /// representation. The behavior is undefined unless 'isSubject()' is true.
    EncryptionCertificateSubjectKeyIdentifier& subjectKeyIdentifier();

    /// Return a reference to the modifiable "subjectKeyUsage" representation.
    /// The behavior is undefined unless 'isSubjectKeyUsage()' is true.
    EncryptionCertificateSubjectKeyUsage& subjectKeyUsage();

    /// Return a reference to the modifiable "subjectKeyUsageExtended"
    /// representation. The behavior is undefined unless
    /// 'isSubjectKeyUsageExtended()' is true.
    EncryptionCertificateSubjectKeyUsageExtended& subjectKeyUsageExtended();

    /// Return a reference to the modifiable "subjectConstraints"
    /// representation. The behavior is undefined unless
    /// 'isSubjectConstraints()' is true.
    EncryptionCertificateSubjectConstraints& subjectConstraints();

    /// Return a reference to the modifiable "issuer" representation. The
    /// behavior is undefined unless 'isIssuer()' is true.
    EncryptionCertificateIssuer& issuer();

    /// Return a reference to the modifiable "issuerKeyIdentifier"
    /// representation. The behavior is undefined unless
    /// 'isIssuerKeyIdentifier()' is true.
    EncryptionCertificateIssuerKeyIdentifier& issuerKeyIdentifier();

    /// Return a reference to the modifiable "issuerInformationAccess"
    /// representation. The behavior is undefined unless
    /// 'isIssuerInformationAccess()' is true.
    EncryptionCertificateIssuerInformationAccess& issuerInformationAccess();

    /// Return a reference to the modifiable "policy" representation. The
    /// behavior is undefined unless 'isPolicy()' is true.
    EncryptionCertificatePolicy& policy();

    /// Return a reference to the modifiable "policyMappings" representation.
    /// The behavior is undefined unless 'isPolicyMappings()' is true.
    EncryptionCertificatePolicyMappings& policyMappings();

    /// Return a reference to the modifiable "policyConstraints"
    /// representation. The behavior is undefined unless
    /// 'isPolicyConstraints()' is true.
    EncryptionCertificatePolicyConstraints& policyConstraints();

    /// Return a reference to the modifiable "byteSequence" representation. The
    /// behavior is undefined unless 'isAny()' is true.
    ntsa::AbstractOctetString& byteSequence();

    /// Return a reference to the modifiable "any" representation. The
    /// behavior is undefined unless 'isAny()' is true.
    ntsa::AbstractValue& any();

    /// Return a reference to the non-modifiable "boolean" representation. The
    /// behavior is undefined unless 'isBoolean()' is true.
    bool boolean() const;

    /// Return a reference to the non-modifiable "name" representation.
    /// The behavior is undefined unless 'isName()' is true.
    const EncryptionCertificateName& name() const;

    /// Return a reference to the non-modifiable "nameAlternative"
    /// representation. The behavior is undefined unless 'isNameAlternative()'
    /// is true.
    const EncryptionCertificateNameAlternativeList& nameAlternative() const;

    /// Return a reference to the non-modifiable "subject" representation.
    /// The behavior is undefined unless 'isSubject()' is true.
    const EncryptionCertificateSubject& subject() const;

    /// Return a reference to the non-modifiable "subjectKeyIdentifier"
    /// representation. The behavior is undefined unless 'isSubject()' is true.
    const EncryptionCertificateSubjectKeyIdentifier& subjectKeyIdentifier()
        const;

    /// Return a reference to the non-modifiable "subjectKeyUsage"
    /// representation. The behavior is undefined unless 'isSubjectKeyUsage()'
    /// is true.
    const EncryptionCertificateSubjectKeyUsage& subjectKeyUsage() const;

    /// Return a reference to the non-modifiable "subjectKeyUsageExtended"
    /// representation. The behavior is undefined unless
    /// 'isSubjectKeyUsageExtended()' is true.
    const EncryptionCertificateSubjectKeyUsageExtended& subjectKeyUsageExtended()
        const;

    /// Return a reference to the non-modifiable "subjectConstraints"
    /// representation. The behavior is undefined unless
    /// 'isSubjectConstraints()' is true.
    const EncryptionCertificateSubjectConstraints& subjectConstraints() const;

    /// Return a reference to the non-modifiable "issuer" representation. The
    /// behavior is undefined unless 'isIssuer()' is true.
    const EncryptionCertificateIssuer& issuer() const;

    /// Return a reference to the non-modifiable "issuerKeyIdentifier"
    /// representation. The behavior is undefined unless
    /// 'isIssuerKeyIdentifier()' is true.
    const EncryptionCertificateIssuerKeyIdentifier& issuerKeyIdentifier()
        const;

    /// Return a reference to the non-modifiable "issuerInformationAccess"
    /// representation. The behavior is undefined unless
    /// 'isIssuerInformationAccess()' is true.
    const EncryptionCertificateIssuerInformationAccess& issuerInformationAccess()
        const;

    /// Return a reference to the non-modifiable "policy" representation. The
    /// behavior is undefined unless 'isPolicy()' is true.
    const EncryptionCertificatePolicy& policy() const;

    /// Return a reference to the non-modifiable "policyMappings"
    /// representation. The behavior is undefined unless 'isPolicyMappings()'
    /// is true.
    const EncryptionCertificatePolicyMappings& policyMappings() const;

    /// Return a reference to the non-modifiable "policyConstraints"
    /// representation. The behavior is undefined unless
    /// 'isPolicyConstraints()' is true.
    const EncryptionCertificatePolicyConstraints& policyConstraints() const;

    /// Return a reference to the non-modifiable "byteSequence" representation.
    /// The behavior is undefined unless 'isByteSequence()' is true.
    const ntsa::AbstractOctetString& byteSequence() const;

    /// Return a reference to the non-modifiable "any" representation. The
    /// behavior is undefined unless 'isAny()' is true.
    const ntsa::AbstractValue& any() const;

    /// Return true if the representation is undefined, otherwise return false.
    bool isUndefined() const;

    /// Return true if the "boolean" representation is currently selected,
    /// otherwise return false.
    bool isBoolean() const;

    /// Return true if the "name" representation is currently selected,
    /// otherwise return false.
    bool isName() const;

    /// Return true if the "nameAlternative" representation is currently
    /// selected, otherwise return false.
    bool isNameAlternative() const;

    /// Return true if the "subject" representation is currently selected,
    /// otherwise return false.
    bool isSubject() const;

    /// Return true if the "subjectKeyIdentifier" representation is currently
    /// selected, otherwise return false.
    bool isSubjectKeyIdentifier() const;

    /// Return true if the "subjectKeyUsage" representation is currently
    /// selected, otherwise return false.
    bool isSubjectKeyUsage() const;

    /// Return true if the "subjectKeyUsageExtended" representation is
    /// currently selected, otherwise return false.
    bool isSubjectKeyUsageExtended() const;

    /// Return true if the "subjectConstraints" representation is currently
    /// selected, otherwise return false.
    bool isSubjectConstraints() const;

    /// Return true if the "issuer" representation is currently selected,
    /// otherwise return false.
    bool isIssuer() const;

    /// Return true if the "issuerKeyIdentifier" representation is currently
    /// selected, otherwise return false.
    bool isIssuerKeyIdentifier() const;

    /// Return true if the "issuerInformationAccess" representation is
    /// currently selected, otherwise return false.
    bool isIssuerInformationAccess() const;

    /// Return true if the "policy" representation is currently selected,
    /// otherwise return false.
    bool isPolicy() const;

    /// Return true if the "policyMappings" representation is currently
    /// selected, otherwise return false.
    bool isPolicyMappings() const;

    /// Return true if the "policyConstraints" representation is currently
    /// selected, otherwise return false.
    bool isPolicyConstraints() const;

    /// Return true if the "byteSequence" representation is currently selected,
    /// otherwise return false.
    bool isByteSequence() const;

    /// Return true if the "any" representation is currently selected,
    /// otherwise return false.
    bool isAny() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateExtensionValue& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateExtensionValue& other) const;

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
        EncryptionCertificateExtensionValue);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateExtensionValue
bsl::ostream& operator<<(bsl::ostream&                              stream,
                         const EncryptionCertificateExtensionValue& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateExtensionValue
bool operator==(const EncryptionCertificateExtensionValue& lhs,
                const EncryptionCertificateExtensionValue& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateExtensionValue
bool operator!=(const EncryptionCertificateExtensionValue& lhs,
                const EncryptionCertificateExtensionValue& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateExtensionValue
bool operator<(const EncryptionCertificateExtensionValue& lhs,
               const EncryptionCertificateExtensionValue& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateExtensionValue
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                            algorithm,
                const EncryptionCertificateExtensionValue& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateExtensionValue::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    if (d_type == e_BOOLEAN) {
        hashAppend(algorithm, d_boolean.object());
    }
    else if (d_type == e_NAME) {
        hashAppend(algorithm, d_name.object());
    }
    else if (d_type == e_NAME_ALTERNATIVE) {
        hashAppend(algorithm, d_nameAlternative.object());
    }
    else if (d_type == e_SUBJECT) {
        hashAppend(algorithm, d_subject.object());
    }
    else if (d_type == e_SUBJECT_KEY_IDENTIFIER) {
        hashAppend(algorithm, d_subjectKeyIdentifier.object());
    }
    else if (d_type == e_SUBJECT_KEY_USAGE) {
        hashAppend(algorithm, d_subjectKeyUsage.object());
    }
    else if (d_type == e_SUBJECT_KEY_USAGE_EXTENDED) {
        hashAppend(algorithm, d_subjectKeyUsageExtended.object());
    }
    else if (d_type == e_SUBJECT_CONSTRAINTS) {
        hashAppend(algorithm, d_subjectConstraints.object());
    }
    else if (d_type == e_ISSUER) {
        hashAppend(algorithm, d_issuer.object());
    }
    else if (d_type == e_ISSUER_KEY_IDENTIFIER) {
        hashAppend(algorithm, d_issuerKeyIdentifier.object());
    }
    else if (d_type == e_ISSUER_INFORMATION_ACCESS) {
        hashAppend(algorithm, d_issuerInformationAccess.object());
    }
    else if (d_type == e_POLICY) {
        hashAppend(algorithm, d_policy.object());
    }
    else if (d_type == e_POLICY_MAPPINGS) {
        hashAppend(algorithm, d_policyMappings.object());
    }
    else if (d_type == e_POLICY_CONSTRAINTS) {
        hashAppend(algorithm, d_policyConstraints.object());
    }
    else if (d_type == e_BYTE_SEQUENCE) {
        hashAppend(algorithm, d_byteSequence.object());
    }
    else if (d_type == e_ANY) {
        hashAppend(algorithm, d_any.object());
    }
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                            algorithm,
                const EncryptionCertificateExtensionValue& value)
{
    value.hash(algorithm);
}

/// Describe an encryption certificate extension.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateExtension
{
    EncryptionCertificateExtensionIdentifier d_attribute;
    bdlb::NullableValue<bool>                d_critical;
    EncryptionCertificateExtensionValue      d_value;
    bslma::Allocator*                        d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateExtension(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateExtension(
        const EncryptionCertificateExtension& original,
        bslma::Allocator*                     basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateExtension();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateExtension& operator=(
        const EncryptionCertificateExtension& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the attribute to the specified 'value'.
    void setAttribute(const EncryptionCertificateExtensionIdentifier& value);

    /// Set the attribute to the specified 'value'.
    void setAttribute(const ntsa::AbstractObjectIdentifier& value);

    /// Set the attribute to the object identifier corresponding to the
    /// specified 'value'.
    void setAttribute(
        EncryptionCertificateExtensionIdentifierType::Value value);

    /// Set the value to the specified 'value'.
    void setValue(bool value);

    /// Set the value to the specified 'value'.
    void setValue(const EncryptionCertificateNameAlternativeList& value);

    /// Set the value to the specified 'value'.
    void setValue(const ntsa::AbstractValue& value);

    /// Set the value to the specified 'value'.
    void setValue(const EncryptionCertificateExtensionValue& value);

    /// Set the critical flag to the specified 'value'.
    void setCritical(bool value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the extension attribute.
    const EncryptionCertificateExtensionIdentifier& attribute() const;

    /// Return the extension value.
    const EncryptionCertificateExtensionValue& value() const;

    /// Return the flag indicating the extension is critical.
    const bdlb::NullableValue<bool>& critical() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateExtension& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateExtension& other) const;

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
        EncryptionCertificateExtension);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateExtension
bsl::ostream& operator<<(bsl::ostream&                         stream,
                         const EncryptionCertificateExtension& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateExtension
bool operator==(const EncryptionCertificateExtension& lhs,
                const EncryptionCertificateExtension& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateExtension
bool operator!=(const EncryptionCertificateExtension& lhs,
                const EncryptionCertificateExtension& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateExtension
bool operator<(const EncryptionCertificateExtension& lhs,
               const EncryptionCertificateExtension& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateExtension
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                       algorithm,
                const EncryptionCertificateExtension& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateExtension::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_attribute);
    hashAppend(algorithm, d_critical);
    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                       algorithm,
                const EncryptionCertificateExtension& value)
{
    value.hash(algorithm);
}

/// Describe an encryption certificate extension list.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateExtensionList
{
    bsl::vector<EncryptionCertificateExtension> d_container;
    bslma::Allocator*                           d_allocator_p;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateExtensionList(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateExtensionList(
        const EncryptionCertificateExtensionList& original,
        bslma::Allocator*                         basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateExtensionList();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateExtensionList& operator=(
        const EncryptionCertificateExtensionList& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return true if the certificate is a certificate authority, i.e. it
    /// is granted permission to authenticate and sign intermediate
    /// certificates or end-user certificates, otherwise return false.
    bool isAuthority() const;

    /// Return true if the certificate subject matches the specified
    /// 'domainName', otherwise return false.
    bool matchesSubjectDomainName(const bsl::string& domainName) const;

    /// Return true if the certificate subject matches specified 'ipAddress',
    /// otherwise return false.
    bool matchesSubjectIpAddress(const ntsa::IpAddress& ipAddress) const;

    /// Return true if the certificate issuer matches the specified
    /// 'domainName', otherwise return false.
    bool matchesIssuerDomainName(const bsl::string& domainName) const;

    /// Return true if the certificate issuer matches specified 'ipAddress',
    /// otherwise return false.
    bool matchesIssuerIpAddress(const ntsa::IpAddress& ipAddress) const;

    /// Return true if the certificate supports the specified well-known key
    /// 'usage', otherwise return false.
    bool allowsKeyUsage(
        ntca::EncryptionCertificateSubjectKeyUsageType::Value usage) const;
        
    /// Return true if the certificate supports the specified well-known 
    /// extended key 'usage', otherwise return false.
    bool allowsKeyUsage(
        ntca::EncryptionCertificateSubjectKeyUsageExtendedType::Value usage) 
        const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateExtensionList& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateExtensionList& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    /// Format this object to the specified output 'stream' at the optionally
    /// specified indentation 'level' and return a reference to the modifiable
    /// 'stream'. If 'level' is specified, optionally specify 'spacesPerLevel',
    /// the number of spaces per indentation level for this and all of its
    /// nested objects.  Each line is indented by the absolute value of 'level
    /// * spacesPerLevel'. If 'level' is negative, suppress indentation of the
    /// first line. If 'spacesPerLevel' is negative, suppress line breaks and
    /// format the entire output on one line. If 'stream' is initially invalid,
    /// this operation has no effect. Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(
        EncryptionCertificateExtensionList);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateExtensionList
bsl::ostream& operator<<(bsl::ostream&                             stream,
                         const EncryptionCertificateExtensionList& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateExtensionList
bool operator==(const EncryptionCertificateExtensionList& lhs,
                const EncryptionCertificateExtensionList& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateExtensionList
bool operator!=(const EncryptionCertificateExtensionList& lhs,
                const EncryptionCertificateExtensionList& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateExtensionList
bool operator<(const EncryptionCertificateExtensionList& lhs,
               const EncryptionCertificateExtensionList& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateExtensionList
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                           algorithm,
                const EncryptionCertificateExtensionList& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateExtensionList::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_container);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                           algorithm,
                const EncryptionCertificateExtensionList& value)
{
    value.hash(algorithm);
}

/// Describes an un-verified certificate of identity and authenticity as used
/// in public key cryptography.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b serialNumber:
/// The unique number assigned to the certificate.
///
/// @li @b subject:
/// The subject of the certificate.
///
/// @li @b subjectUniqueId:
/// The unique identifier of the subject.
///
/// @li @b subjectPublicKeyInfo:
/// The subject's public key.
///
/// @li @b issuer:
/// The issuer of the certificate.
///
/// @li @b issuerUniqueId:
/// The unique identifier of the issuer.
///
/// @li @b extensionList:
/// The list of extensions that futher define properties of the certificate.
///
/// @li @b validity:
/// The date/time interval in which the certificate is valid.
///
/// @li @b signatureAlgorithm:
/// The algorithm used to sign the certificate.
///
/// @li @b signature:
/// The signature of the certificate that proves its authenticity.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateEntity
{
    typedef ntca::EncryptionCertificateVersion       Version;
    typedef ntsa::AbstractBitString                  UniqueIdentifier;
    typedef ntca::EncryptionCertificateExtension     Extension;
    typedef ntca::EncryptionCertificateExtensionList ExtensionList;

    bdlb::NullableValue<Version>                  d_version;
    ntsa::AbstractInteger                         d_serialNumber;
    ntca::EncryptionCertificateSignatureAlgorithm d_signatureAlgorithm;
    ntca::EncryptionCertificateIssuer             d_issuer;
    ntca::EncryptionCertificateValidity           d_validity;
    ntca::EncryptionCertificateSubject            d_subject;
    ntca::EncryptionKeyInfoPublic                 d_subjectPublicKeyInfo;
    bdlb::NullableValue<UniqueIdentifier>         d_issuerUniqueId;
    bdlb::NullableValue<UniqueIdentifier>         d_subjectUniqueId;
    bdlb::NullableValue<ExtensionList>            d_extensionList;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificateEntity(bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificateEntity(const EncryptionCertificateEntity& original,
                                bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateEntity();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateEntity& operator=(
        const EncryptionCertificateEntity& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the serial number.
    const ntsa::AbstractInteger& serialNumber() const;

    // Return the subject.
    const ntca::EncryptionCertificateSubject& subject() const;

    // Return the unique identifier of the subject.
    const bdlb::NullableValue<ntsa::AbstractBitString>& subjectUniqueId()
        const;

    /// Return the subject's public key information.
    const ntca::EncryptionKeyInfoPublic& subjectPublicKeyInfo() const;

    // Return the issuer.
    const ntca::EncryptionCertificateIssuer& issuer() const;

    // Return the unique identifier of the issuer.
    const bdlb::NullableValue<ntsa::AbstractBitString>& issuerUniqueId() const;

    // Return the list of extensions.
    const bdlb::NullableValue<ntca::EncryptionCertificateExtensionList>&
    extensionList() const;

    /// Return the date/time interval within which the certificate is valid.
    const ntca::EncryptionCertificateValidity& validity() const;

    /// Return the signature algorithm.
    const ntca::EncryptionCertificateSignatureAlgorithm& signatureAlgorithm()
        const;

    /// Return true if the certificate is a certificate authority, i.e. it
    /// is granted permission to authenticate and sign intermediate
    /// certificates or end-user certificates, otherwise return false.
    bool isAuthority() const;

    /// Return true if the certificate subject matches the specified
    /// 'domainName', otherwise return false.
    bool matchesSubjectDomainName(const bsl::string& domainName) const;

    /// Return true if the certificate subject matches specified 'ipAddress',
    /// otherwise return false.
    bool matchesSubjectIpAddress(const ntsa::IpAddress& ipAddress) const;

    /// Return true if the certificate issuer matches the specified
    /// 'domainName', otherwise return false.
    bool matchesIssuerDomainName(const bsl::string& domainName) const;

    /// Return true if the certificate issuer matches specified 'ipAddress',
    /// otherwise return false.
    bool matchesIssuerIpAddress(const ntsa::IpAddress& ipAddress) const;

    /// Return true if the certificate supports the specified well-known key
    /// 'usage', otherwise return false.
    bool allowsKeyUsage(
        ntca::EncryptionCertificateSubjectKeyUsageType::Value usage) const;
        
    /// Return true if the certificate supports the specified well-known 
    /// extended key 'usage', otherwise return false.
    bool allowsKeyUsage(
        ntca::EncryptionCertificateSubjectKeyUsageExtendedType::Value usage) 
        const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateEntity& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateEntity& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateEntity);
};

/// Define a type alias for a vector of encryption certificates.
///
/// @ingroup module_ntci_encryption
typedef bsl::vector<ntca::EncryptionCertificateEntity> EncryptionEntityVector;

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificateEntity
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const EncryptionCertificateEntity& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificateEntity
bool operator==(const EncryptionCertificateEntity& lhs,
                const EncryptionCertificateEntity& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificateEntity
bool operator!=(const EncryptionCertificateEntity& lhs,
                const EncryptionCertificateEntity& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificateEntity
bool operator<(const EncryptionCertificateEntity& lhs,
               const EncryptionCertificateEntity& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificateEntity
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                    algorithm,
                const EncryptionCertificateEntity& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateEntity::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_version);
    hashAppend(algorithm, d_serialNumber);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                    algorithm,
                const EncryptionCertificateEntity& value)
{
    value.hash(algorithm);
}

/// Describes a verified certificate of identity and authenticity as used in
/// public key cryptography.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b serialNumber:
/// The unique number assigned to the certificate.
///
/// @li @b subject:
/// The subject of the certificate.
///
/// @li @b subjectUniqueId:
/// The unique identifier of the subject.
///
/// @li @b subjectPublicKeyInfo:
/// The subject's public key.
///
/// @li @b issuer:
/// The issuer of the certificate.
///
/// @li @b issuerUniqueId:
/// The unique identifier of the issuer.
///
/// @li @b extensionList:
/// The list of extensions that futher define properties of the certificate.
///
/// @li @b validity:
/// The date/time interval in which the certificate is valid.
///
/// @li @b signatureAlgorithm:
/// The algorithm used to sign the certificate.
///
/// @li @b signature:
/// The signature of the certificate that proves its authenticity.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificate
{
    ntca::EncryptionCertificateEntity             d_entity;
    ntca::EncryptionCertificateSignatureAlgorithm d_signatureAlgorithm;
    ntca::EncryptionCertificateSignature          d_signature;

  public:
    /// Create a new object having the default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EncryptionCertificate(bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
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

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the certificate entity.
    const ntca::EncryptionCertificateEntity& entity() const;

    /// Return the serial number.
    const ntsa::AbstractInteger& serialNumber() const;

    // Return the subject.
    const ntca::EncryptionCertificateSubject& subject() const;

    // Return the unique identifier of the subject.
    const bdlb::NullableValue<ntsa::AbstractBitString>& subjectUniqueId()
        const;

    /// Return the subject's public key information.
    const ntca::EncryptionKeyInfoPublic& subjectPublicKeyInfo() const;

    // Return the issuer.
    const ntca::EncryptionCertificateIssuer& issuer() const;

    // Return the unique identifier of the issuer.
    const bdlb::NullableValue<ntsa::AbstractBitString>& issuerUniqueId() const;

    // Return the list of extensions.
    const bdlb::NullableValue<ntca::EncryptionCertificateExtensionList>&
    extensionList() const;

    /// Return the date/time interval within which the certificate is valid.
    const ntca::EncryptionCertificateValidity& validity() const;

    /// Return the certificate signature algorithm.
    const ntca::EncryptionCertificateSignatureAlgorithm& signatureAlgorithm()
        const;

    /// Return the certificate signature value.
    const ntca::EncryptionCertificateSignature& signature() const;

    /// Return true if the certificate is a certificate authority, i.e. it
    /// is granted permission to authenticate and sign intermediate
    /// certificates or end-user certificates, otherwise return false.
    bool isAuthority() const;

    /// Return true if the certificate subject matches the specified
    /// 'domainName', otherwise return false.
    bool matchesSubjectDomainName(const bsl::string& domainName) const;

    /// Return true if the certificate subject matches specified 'ipAddress',
    /// otherwise return false.
    bool matchesSubjectIpAddress(const ntsa::IpAddress& ipAddress) const;

    /// Return true if the certificate issuer matches the specified
    /// 'domainName', otherwise return false.
    bool matchesIssuerDomainName(const bsl::string& domainName) const;

    /// Return true if the certificate issuer matches specified 'ipAddress',
    /// otherwise return false.
    bool matchesIssuerIpAddress(const ntsa::IpAddress& ipAddress) const;

    /// Return true if the certificate supports the specified well-known key
    /// 'usage', otherwise return false.
    bool allowsKeyUsage(
        ntca::EncryptionCertificateSubjectKeyUsageType::Value usage) const;
        
    /// Return true if the certificate supports the specified well-known 
    /// extended key 'usage', otherwise return false.
    bool allowsKeyUsage(
        ntca::EncryptionCertificateSubjectKeyUsageExtendedType::Value usage) 
        const;

    /// Return true if the certificate uses the specified well-known public key
    /// 'algorithm', otherwise return false.
    bool usesSubjectPublicKeyAlgorithm(
        ntca::EncryptionKeyAlgorithmIdentifierType::Value algorithm) const;
    
    /// Return true if the certificate uses the specified well-known public key
    /// algorithm 'parameters', otherwise return false. 
    bool usesSubjectPublicKeyAlgorithmParameters(
        ntca::EncryptionKeyEllipticCurveParametersIdentifierType::Value 
        parameters) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificate& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificate& other) const;

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
void EncryptionCertificate::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_entity);
    hashAppend(algorithm, d_signatureAlgorithm);
    hashAppend(algorithm, d_signature);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionCertificate& value)
{
    value.hash(algorithm);
}

/// Provide utilities for certificates.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateUtil
{
  public:
    /// Return true if the specified 'requested' domain name or IP address
    /// matches the specified 'certified' domain name or IP address, otherwise
    /// return false.
    static bool matches(const bsl::string& requested,
                        const bsl::string& certified);

    /// Return true if the specified 'requested' domain name or IP address
    /// matches the specified 'certified' IP address, otherwise return false.
    static bool matches(const bsl::string&     requested,
                        const ntsa::IpAddress& certified);

    /// Return true if the specified 'requested' domain name or IP address
    /// matches the specified 'certified' URI, otherwise return false.
    static bool matches(const bsl::string& requested,
                        const ntsa::Uri&   certified);

    /// Return true if the specified 'requested' endpoint matches the specified
    /// 'certified' domain name or IP address, otherwise return false.
    static bool matches(const ntsa::Endpoint& requested,
                        const bsl::string&    certified);

    /// Return true if the specified 'requested' endpoint matches the specified
    /// 'certified' IP address, otherwise return false.
    static bool matches(const ntsa::Endpoint&  requested,
                        const ntsa::IpAddress& certified);

    /// Return true if the specified 'requested' endpoint matches the specified
    /// 'certified' URI, otherwise return false.
    static bool matches(const ntsa::Endpoint& requested,
                        const ntsa::Uri&      certified);

    /// Return true if the specified 'requested' IP address matches the
    /// specified 'certified' domain name or IP address , otherwise return
    /// false.
    static bool matches(const ntsa::IpAddress& requested,
                        const bsl::string&     certified);

    /// Return true if the specified 'requested' IP address matches the
    /// specified 'certified' IP address, otherwise return false.
    static bool matches(const ntsa::IpAddress& requested,
                        const ntsa::IpAddress& certified);

    /// Return true if the specified 'requested' IP address matches the
    /// specified 'certified' URI, otherwise return false.
    static bool matches(const ntsa::IpAddress& requested,
                        const ntsa::Uri&       certified);

    /// Return true if the specified 'requested' local name matches the
    /// specified 'certified' domain name or IP address, otherwise return
    /// false.
    static bool matches(const ntsa::LocalName& requested,
                        const bsl::string&     certified);

    /// Return true if the specified 'requested' local name matches the
    /// specified 'certified' IP address, otherwise return false.
    static bool matches(const ntsa::LocalName& requested,
                        const ntsa::IpAddress& certified);

    /// Return true if the specified 'requested' local name matches the
    /// specified 'certified' URI, otherwise return false.
    static bool matches(const ntsa::LocalName& requested,
                        const ntsa::Uri&       certified);

    /// Return true if the specified 'requested' URI matches the specified
    /// 'certified' domain name or IP address, otherwise return false.
    static bool matches(const ntsa::Uri&   requested,
                        const bsl::string& certified);

    /// Return true if the specified 'requested' URI matches the specified
    /// 'certified' IP address, otherwise return false.
    static bool matches(const ntsa::Uri&       requested,
                        const ntsa::IpAddress& certified);

    /// Return true if the specified 'requested' URI matches the specified
    /// 'certified' URI, otherwise return false.
    static bool matches(const ntsa::Uri& requested,
                        const ntsa::Uri& certified);
};

/// Define a type alias for a function invoked to perform user-defined
/// validation of the specified 'certificate'. Return true if the certificate
/// is valid, and false otherwise.
typedef bsl::function<bool(const ntca::EncryptionCertificate& certificate)>
    EncryptionCertificateValidator;

}  // close package namespace
}  // close enterprise namespace
#endif
