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
#include <ntsa_abstract.h>
#include <ntsa_uri.h>
#include <ntsa_ipaddress.h>
#include <bdlt_datetimetz.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntca {


class EncryptionCertificateVersion;
class EncryptionCertificateNameAttributeType;
class EncryptionCertificateNameAttribute;
class EncryptionCertificateNameComponent;
class EncryptionCertificateName;
class EncryptionCertificateNameAlternative;
class EncryptionCertificateNameAlternativeList;
class EncryptionCertificateValidity;
class EncryptionCertificatePublicKeyInfo;
class EncryptionCertificateSignatureAlgorithmType;
class EncryptionCertificateSignatureAlgorithm;
class EncryptionCertificateSignature;

class EncryptionCertificateExtensionAttributeType;
class EncryptionCertificateExtensionAttribute;
class EncryptionCertificateExtensionValue;
class EncryptionCertificateExtension;
class EncryptionCertificateExtensionList;

class EncryptionCertificateEntity;
class EncryptionCertificate;

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
    bslma::Allocator* d_allocator_p;

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

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the value.
    bsl::size_t value() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateTemplate& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateTemplate& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateTemplate);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                        stream,
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
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateTemplate& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateTemplate::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateTemplate& value)
{
    value.hash(algorithm);
}









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
class EncryptionCertificateVersion
{
    bsl::size_t       d_value;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new certificate having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionCertificateVersion(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new certificate having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionCertificateVersion(
        const EncryptionCertificateVersion& original,
        bslma::Allocator*                    basicAllocator = 0);

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
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateVersion& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const EncryptionCertificateVersion& lhs,
                const EncryptionCertificateVersion& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const EncryptionCertificateVersion& lhs,
                const EncryptionCertificateVersion& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const EncryptionCertificateVersion& lhs,
               const EncryptionCertificateVersion& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateVersion& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateVersion::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateVersion& value)
{
    value.hash(algorithm);
}



/// Enumerate Abstract Syntax Notation (ASN.1) well-known object identifiers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_data
class EncryptionCertificateNameAttributeType 
{
public:
    /// Enumerate Abstract Syntax Notation (ASN.1) well-known object 
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

        /// Generational suffix, e.g., Jr., or III.
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
/// @related ntsa::AbstractSyntaxTagClass
bsl::ostream& operator<<(bsl::ostream&             stream,
                         EncryptionCertificateNameAttributeType::Value rhs);

























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
class EncryptionCertificateNameAttribute
{
    ntsa::AbstractObjectIdentifier d_identifier;
    bslma::Allocator*              d_allocator_p;

  public:
    /// Create a new certificate having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionCertificateNameAttribute(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new certificate having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionCertificateNameAttribute(
        const EncryptionCertificateNameAttribute& original,
        bslma::Allocator*                    basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateNameAttribute();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateNameAttribute& operator=(
        const EncryptionCertificateNameAttribute& other);

    /// Assign the value of the specified 'value' to this object. Return a
    /// reference to this modifiable object.
    EncryptionCertificateNameAttribute& operator=(
        const ntsa::AbstractObjectIdentifier& value);

    /// Assign the value of the specified 'value' to this object. Return a
    /// reference to this modifiable object.
    EncryptionCertificateNameAttribute& operator=(
        EncryptionCertificateNameAttributeType::Value value);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the identifier to the specified 'value'.
    void setIdentifier(const ntsa::AbstractObjectIdentifier& value);

    /// Set the identifier to the object identifier corresponding to the
    /// specified 'value'. 
    void setIdentifer(EncryptionCertificateNameAttributeType::Value value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the identifier.
    const ntsa::AbstractObjectIdentifier& identifier() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateNameAttribute& other) const;

    /// Return true if this object has the same value as the specified
    /// 'value', otherwise return false.
    bool equals(EncryptionCertificateNameAttributeType::Value value) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateNameAttribute);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateNameAttribute& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const EncryptionCertificateNameAttribute& lhs,
                const EncryptionCertificateNameAttribute& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const EncryptionCertificateNameAttribute& lhs,
                const EncryptionCertificateNameAttribute& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const EncryptionCertificateNameAttribute& lhs,
               const EncryptionCertificateNameAttribute& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateNameAttribute& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateNameAttribute::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_identifier);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateNameAttribute& value)
{
    value.hash(algorithm);
}










































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
class EncryptionCertificateNameComponent
{
    EncryptionCertificateNameAttribute d_attribute;
    bsl::string                        d_value;
    bslma::Allocator*                  d_allocator_p;

  public:
    /// Create a new certificate having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionCertificateNameComponent(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new certificate having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionCertificateNameComponent(
        const EncryptionCertificateNameComponent& original,
        bslma::Allocator*                    basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateNameComponent();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateNameComponent& operator=(
        const EncryptionCertificateNameComponent& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the attribute to the specified 'value'.
    void setAttribute(const EncryptionCertificateNameAttribute& value);

    /// Set the attribute to the specified 'value'.
    void setAttribute(const ntsa::AbstractObjectIdentifier& value);

    /// Set the attribute to the object identifier corresponding to the
    /// specified 'value'. 
    void setAttribute(EncryptionCertificateNameAttributeType::Value value);

    /// Set the attribute value to the specified 'value'.
    void setValue(const bsl::string& value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the component attribute.
    const EncryptionCertificateNameAttribute& attribute() const;

    /// Return the component value.
    const bsl::string& value() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateNameComponent& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateNameComponent& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateNameComponent);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateNameComponent& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const EncryptionCertificateNameComponent& lhs,
                const EncryptionCertificateNameComponent& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const EncryptionCertificateNameComponent& lhs,
                const EncryptionCertificateNameComponent& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const EncryptionCertificateNameComponent& lhs,
               const EncryptionCertificateNameComponent& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateNameComponent& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateNameComponent::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_attribute);
    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateNameComponent& value)
{
    value.hash(algorithm);
}


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
class EncryptionCertificateName
{
    typedef bsl::vector<EncryptionCertificateNameComponent> AttributeVector;

    AttributeVector   d_attributeVector;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new certificate having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionCertificateName(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new certificate having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionCertificateName(
        const EncryptionCertificateName& original,
        bslma::Allocator*                    basicAllocator = 0);

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
        const bsl::vector<EncryptionCertificateNameComponent>& value);

    /// Append a name component having the specified 'value'.
    void append(const EncryptionCertificateNameComponent& value);

    /// Append a name component having the specified 'attribute' with the 
    /// specified 'value'.
    void append(const EncryptionCertificateNameAttribute& attribute,
                const bsl::string&                        value);

    /// Append a name component having the specified 'attribute' with the 
    /// specified 'value'.
    void append(const ntsa::AbstractObjectIdentifier& attribute,
                const bsl::string&                    value);

    /// Append a name component having the specified 'attribute' with the 
    /// specified 'value'.
    void append(EncryptionCertificateNameAttributeType::Value attribute,
                const bsl::string&                            value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the attribute sequence.
    const bsl::vector<EncryptionCertificateNameComponent>& attributeSequence() const;

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
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateName& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const EncryptionCertificateName& lhs,
                const EncryptionCertificateName& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const EncryptionCertificateName& lhs,
                const EncryptionCertificateName& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const EncryptionCertificateName& lhs,
               const EncryptionCertificateName& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateName& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateName::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_attributeVector);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateName& value)
{
    value.hash(algorithm);
}




















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
class EncryptionCertificateNameAlternative
{
    enum Type {
        e_UNDEFINED = -1,
        e_OTHER = 0,
        e_EMAIL = 1,
        e_DOMAIN = 2,
        e_X400 = 3,
        e_DIRECTORY = 4,
        e_EDI = 5,
        e_URI = 6,
        e_IP = 7,
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
    /// Create a new certificate having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionCertificateNameAlternative(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new certificate having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionCertificateNameAlternative(
        const EncryptionCertificateNameAlternative& original,
        bslma::Allocator*                    basicAllocator = 0);

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
    EncryptionCertificateName& makeDirectory(const EncryptionCertificateName& value);

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
    ntsa::AbstractObjectIdentifier& makeIdentifier(const ntsa::AbstractObjectIdentifier& value);



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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateNameAlternative);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateNameAlternative& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const EncryptionCertificateNameAlternative& lhs,
                const EncryptionCertificateNameAlternative& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const EncryptionCertificateNameAlternative& lhs,
                const EncryptionCertificateNameAlternative& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const EncryptionCertificateNameAlternative& lhs,
               const EncryptionCertificateNameAlternative& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
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
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateNameAlternative& value)
{
    value.hash(algorithm);
}




















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
class EncryptionCertificateNameAlternativeList
{
    bsl::vector<EncryptionCertificateNameAlternative> d_container;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new certificate having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionCertificateNameAlternativeList(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new certificate having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionCertificateNameAlternativeList(
        const EncryptionCertificateNameAlternativeList& original,
        bslma::Allocator*                    basicAllocator = 0);

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateNameAlternativeList);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateNameAlternativeList& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const EncryptionCertificateNameAlternativeList& lhs,
                const EncryptionCertificateNameAlternativeList& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const EncryptionCertificateNameAlternativeList& lhs,
                const EncryptionCertificateNameAlternativeList& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const EncryptionCertificateNameAlternativeList& lhs,
               const EncryptionCertificateNameAlternativeList& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateNameAlternativeList& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateNameAlternativeList::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_container);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionCertificateNameAlternativeList& value)
{
    value.hash(algorithm);
}




















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
class EncryptionCertificateValidity
{
    bdlt::DatetimeTz  d_from;
    bdlt::DatetimeTz  d_thru;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new certificate having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionCertificateValidity(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new certificate having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
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
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateValidity& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const EncryptionCertificateValidity& lhs,
                const EncryptionCertificateValidity& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const EncryptionCertificateValidity& lhs,
                const EncryptionCertificateValidity& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const EncryptionCertificateValidity& lhs,
               const EncryptionCertificateValidity& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
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
class EncryptionCertificatePublicKeyInfo
{
    ntsa::AbstractObjectIdentifier d_publicKeyFamily;
    ntsa::AbstractObjectIdentifier d_publicKeyType;
    ntsa::AbstractBitSequence      d_publicKeyValue;
    bslma::Allocator*              d_allocator_p;

  public:
    /// Create a new certificate having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionCertificatePublicKeyInfo(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new certificate having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionCertificatePublicKeyInfo(
        const EncryptionCertificatePublicKeyInfo& original,
        bslma::Allocator*                    basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificatePublicKeyInfo();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificatePublicKeyInfo& operator=(
        const EncryptionCertificatePublicKeyInfo& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificatePublicKeyInfo& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificatePublicKeyInfo& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificatePublicKeyInfo);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificatePublicKeyInfo& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const EncryptionCertificatePublicKeyInfo& lhs,
                const EncryptionCertificatePublicKeyInfo& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const EncryptionCertificatePublicKeyInfo& lhs,
                const EncryptionCertificatePublicKeyInfo& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const EncryptionCertificatePublicKeyInfo& lhs,
               const EncryptionCertificatePublicKeyInfo& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificatePublicKeyInfo& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificatePublicKeyInfo::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_publicKeyFamily);
    hashAppend(algorithm, d_publicKeyType);
    hashAppend(algorithm, d_publicKeyValue);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionCertificatePublicKeyInfo& value)
{
    value.hash(algorithm);
}











/// Enumerate the well-known encryption certificate signature algorithm types.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_data
class EncryptionCertificateSignatureAlgorithmType 
{
public:
    /// Enumerate the well-known encryption certificate signature algorithm 
    /// types.
    enum Value {
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
        e_ECDSA_SHA512
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
/// @related ntsa::AbstractSyntaxTagClass
bsl::ostream& operator<<(bsl::ostream&             stream,
                         EncryptionCertificateSignatureAlgorithmType::Value rhs);
















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
class EncryptionCertificateSignatureAlgorithm
{
    ntsa::AbstractObjectIdentifier d_identifier;
    bslma::Allocator*              d_allocator_p;

  public:
    /// Create a new certificate having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionCertificateSignatureAlgorithm(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new certificate having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionCertificateSignatureAlgorithm(
        const EncryptionCertificateSignatureAlgorithm& original,
        bslma::Allocator*                    basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateSignatureAlgorithm();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateSignatureAlgorithm& operator=(
        const EncryptionCertificateSignatureAlgorithm& other);

    /// Assign the value of the specified 'value' to this object. Return a
    /// reference to this modifiable object.
    EncryptionCertificateSignatureAlgorithm& operator=(
        const ntsa::AbstractObjectIdentifier& value);

    /// Assign the value of the specified 'value' to this object. Return a
    /// reference to this modifiable object.
    EncryptionCertificateSignatureAlgorithm& operator=(
        EncryptionCertificateSignatureAlgorithmType::Value value);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the identifier to the specified 'value'.
    void setIdentifier(const ntsa::AbstractObjectIdentifier& value);

    /// Set the identifier to the object identifier corresponding to the
    /// specified 'value'. 
    void setIdentifer(EncryptionCertificateSignatureAlgorithmType::Value value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the identifier.
    const ntsa::AbstractObjectIdentifier& identifier() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateSignatureAlgorithm& other) const;

    /// Return true if this object has the same value as the specified
    /// 'value', otherwise return false.
    bool equals(EncryptionCertificateSignatureAlgorithmType::Value value) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateSignatureAlgorithm);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateSignatureAlgorithm& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const EncryptionCertificateSignatureAlgorithm& lhs,
                const EncryptionCertificateSignatureAlgorithm& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const EncryptionCertificateSignatureAlgorithm& lhs,
                const EncryptionCertificateSignatureAlgorithm& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const EncryptionCertificateSignatureAlgorithm& lhs,
               const EncryptionCertificateSignatureAlgorithm& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateSignatureAlgorithm& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateSignatureAlgorithm::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_identifier);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateSignatureAlgorithm& value)
{
    value.hash(algorithm);
}










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
class EncryptionCertificateSignature
{
    ntsa::AbstractBitSequence d_value;
    bslma::Allocator*         d_allocator_p;

  public:
    /// Create a new certificate having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionCertificateSignature(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new certificate having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionCertificateSignature(
        const EncryptionCertificateSignature& original,
        bslma::Allocator*                    basicAllocator = 0);

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
    void setValue(const ntsa::AbstractBitSequence& value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the value.
    const ntsa::AbstractBitSequence& value() const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateSignature);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateSignature& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const EncryptionCertificateSignature& lhs,
                const EncryptionCertificateSignature& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const EncryptionCertificateSignature& lhs,
                const EncryptionCertificateSignature& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const EncryptionCertificateSignature& lhs,
               const EncryptionCertificateSignature& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateSignature& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateSignature::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_value);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateSignature& value)
{
    value.hash(algorithm);
}



























/// Enumerate Abstract Syntax Notation (ASN.1) well-known object identifiers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_data
class EncryptionCertificateExtensionAttributeType {
public:
    /// Enumerate Abstract Syntax Notation (ASN.1) well-known object 
    /// identifiers.
    enum Value {
        /// Basic constraints.
        e_BASIC_CONSTRAINTS,

        /// Key usage.
        e_KEY_USAGE,

        /// Extended key usage.
        e_KEY_USAGE_EXTENDED,

        /// Subject key identifier.
        e_SUBJECT_KEY_IDENTIFIER,

        /// Subject alternative name.
        e_SUBJECT_ALTERNATIVE_NAME,

        /// Authority key identifier.
        e_AUTHORITY_KEY_IDENTIFIER
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
/// @related ntsa::AbstractSyntaxTagClass
bsl::ostream& operator<<(bsl::ostream&             stream,
                         EncryptionCertificateExtensionAttributeType::Value rhs);














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
class EncryptionCertificateExtensionAttribute
{
    ntsa::AbstractObjectIdentifier d_identifier;
    bslma::Allocator*              d_allocator_p;

  public:
    /// Create a new certificate having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionCertificateExtensionAttribute(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new certificate having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionCertificateExtensionAttribute(
        const EncryptionCertificateExtensionAttribute& original,
        bslma::Allocator*                    basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateExtensionAttribute();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateExtensionAttribute& operator=(
        const EncryptionCertificateExtensionAttribute& other);

    /// Assign the value of the specified 'value' to this object. Return a
    /// reference to this modifiable object.
    EncryptionCertificateExtensionAttribute& operator=(
        const ntsa::AbstractObjectIdentifier& value);

    /// Assign the value of the specified 'value' to this object. Return a
    /// reference to this modifiable object.
    EncryptionCertificateExtensionAttribute& operator=(
        EncryptionCertificateExtensionAttributeType::Value value);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the identifier to the specified 'value'.
    void setIdentifier(const ntsa::AbstractObjectIdentifier& value);

    /// Set the identifier to the object identifier corresponding to the
    /// specified 'value'. 
    void setIdentifer(EncryptionCertificateExtensionAttributeType::Value value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return the identifier.
    const ntsa::AbstractObjectIdentifier& identifier() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionCertificateExtensionAttribute& other) const;

    /// Return true if this object has the same value as the specified
    /// 'value', otherwise return false.
    bool equals(EncryptionCertificateExtensionAttributeType::Value value) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionCertificateExtensionAttribute& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateExtensionAttribute);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateExtensionAttribute& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const EncryptionCertificateExtensionAttribute& lhs,
                const EncryptionCertificateExtensionAttribute& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const EncryptionCertificateExtensionAttribute& lhs,
                const EncryptionCertificateExtensionAttribute& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const EncryptionCertificateExtensionAttribute& lhs,
               const EncryptionCertificateExtensionAttribute& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateExtensionAttribute& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateExtensionAttribute::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_identifier);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateExtensionAttribute& value)
{
    value.hash(algorithm);
}














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
class EncryptionCertificateExtensionValue
{
    enum Type {
        e_UNDEFINED = -1,
        e_BOOLEAN = 0,
        e_NAME_ALTERNATIVE = 1,
        e_ANY = 2
    };

    union {
        bsls::ObjectBuffer<bool> d_boolean;
        bsls::ObjectBuffer<EncryptionCertificateNameAlternativeList> d_nameAlternative;
        bsls::ObjectBuffer<ntsa::AbstractValue> d_any;
    }; 

    Type              d_type;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new certificate having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionCertificateExtensionValue(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new certificate having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionCertificateExtensionValue(
        const EncryptionCertificateExtensionValue& original,
        bslma::Allocator*                    basicAllocator = 0);

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

    /// Select the "nameAlternative" representation. Return a reference to the
    /// modifiable representation.
    EncryptionCertificateNameAlternativeList& makeNameAlternative();

    /// Select the "nameAlternative" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    EncryptionCertificateNameAlternativeList& makeNameAlternative(
        const EncryptionCertificateNameAlternativeList& value);

    /// Select the "any" representation. Return a reference to the modifiable
    /// representation.
    ntsa::AbstractValue& makeAny();

    /// Select the "any" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::AbstractValue& makeAny(const ntsa::AbstractValue& value);

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    /// Return a reference to the modifiable "boolean" representation. The
    /// behavior is undefined unless 'isBoolean()' is true.
    bool& boolean();

    /// Return a reference to the modifiable "nameAlternative" representation.
    /// The behavior is undefined unless 'isNameAlternative()' is true.
    EncryptionCertificateNameAlternativeList& nameAlternative();

    /// Return a reference to the modifiable "any" representation. The
    /// behavior is undefined unless 'isAny()' is true.
    ntsa::AbstractValue& any();

    /// Return a reference to the non-modifiable "boolean" representation. The
    /// behavior is undefined unless 'isBoolean()' is true.
    bool boolean() const;

    /// Return a reference to the non-modifiable "nameAlternative"
    /// representation. The behavior is undefined unless 'isNameAlternative()'
    /// is true.
    const EncryptionCertificateNameAlternativeList& nameAlternative() const;

    /// Return a reference to the non-modifiable "any" representation. The
    /// behavior is undefined unless 'isAny()' is true.
    const ntsa::AbstractValue& any() const;

    /// Return true if the representation is undefined, otherwise return false.
    bool isUndefined() const;

    /// Return true if the "boolean" representation is currently selected,
    /// otherwise return false.
    bool isBoolean() const;

    /// Return true if the "nameAlternative" representation is currently
    /// selected, otherwise return false.
    bool isNameAlternative() const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateExtensionValue);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateExtensionValue& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const EncryptionCertificateExtensionValue& lhs,
                const EncryptionCertificateExtensionValue& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const EncryptionCertificateExtensionValue& lhs,
                const EncryptionCertificateExtensionValue& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const EncryptionCertificateExtensionValue& lhs,
               const EncryptionCertificateExtensionValue& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateExtensionValue& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateExtensionValue::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    if (d_type == e_BOOLEAN) {
        hashAppend(algorithm, d_boolean.object());
    }
    else if (d_type == e_NAME_ALTERNATIVE) {
        hashAppend(algorithm, d_nameAlternative.object());
    }
    else if (d_type == e_ANY) {
        hashAppend(algorithm, d_any.object());
    }
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateExtensionValue& value)
{
    value.hash(algorithm);
}











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
class EncryptionCertificateExtension
{
    EncryptionCertificateExtensionAttribute d_attribute;
    bdlb::NullableValue<bool>               d_critical;
    EncryptionCertificateExtensionValue     d_value;
    bslma::Allocator*                       d_allocator_p;

  public:
    /// Create a new certificate having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionCertificateExtension(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new certificate having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionCertificateExtension(
        const EncryptionCertificateExtension& original,
        bslma::Allocator*                    basicAllocator = 0);

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
    void setAttribute(const EncryptionCertificateExtensionAttribute& value);

    /// Set the attribute to the specified 'value'.
    void setAttribute(const ntsa::AbstractObjectIdentifier& value);

    /// Set the attribute to the object identifier corresponding to the
    /// specified 'value'. 
    void setAttribute(EncryptionCertificateExtensionAttributeType::Value value);    

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
    const EncryptionCertificateExtensionAttribute& attribute() const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateExtension);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateExtension& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const EncryptionCertificateExtension& lhs,
                const EncryptionCertificateExtension& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const EncryptionCertificateExtension& lhs,
                const EncryptionCertificateExtension& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const EncryptionCertificateExtension& lhs,
               const EncryptionCertificateExtension& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
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
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionCertificateExtension& value)
{
    value.hash(algorithm);
}





















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
class EncryptionCertificateExtensionList
{
    bsl::vector<EncryptionCertificateExtension> d_container;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new certificate having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionCertificateExtensionList(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new certificate having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionCertificateExtensionList(
        const EncryptionCertificateExtensionList& original,
        bslma::Allocator*                    basicAllocator = 0);

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionCertificateExtensionList);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateExtensionList& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const EncryptionCertificateExtensionList& lhs,
                const EncryptionCertificateExtensionList& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const EncryptionCertificateExtensionList& lhs,
                const EncryptionCertificateExtensionList& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const EncryptionCertificateExtensionList& lhs,
               const EncryptionCertificateExtensionList& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const EncryptionCertificateExtensionList& value);

template <typename HASH_ALGORITHM>
void EncryptionCertificateExtensionList::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_container);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionCertificateExtensionList& value)
{
    value.hash(algorithm);
}























/// Describe a certificate of identity and authenticity as used in public key
/// cryptography.
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
class EncryptionCertificateEntity
{
    typedef ntsa::AbstractBitSequence                UniqueIdentifier;
    typedef ntca::EncryptionCertificateExtension     Extension;
    typedef ntca::EncryptionCertificateExtensionList ExtensionList;

    ntca::EncryptionCertificateVersion            d_version;
    ntsa::AbstractInteger                         d_serialNumber;
    ntca::EncryptionCertificateSignatureAlgorithm d_signatureAlgorithm;
    ntca::EncryptionCertificateName               d_issuer;
    ntca::EncryptionCertificateValidity           d_validity;
    ntca::EncryptionCertificateName               d_subject;
    ntca::EncryptionCertificatePublicKeyInfo      d_subjectPublicKeyInfo;
    bdlb::NullableValue<UniqueIdentifier>         d_issuerUniqueId;
    bdlb::NullableValue<UniqueIdentifier>         d_subjectUniqueId;
    bdlb::NullableValue<ExtensionList>            d_extensionList;

  public:
    /// Create a new certificate having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionCertificateEntity(bslma::Allocator* basicAllocator = 0);

    /// Create a new certificate having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    EncryptionCertificateEntity(const EncryptionCertificateEntity& original,
                          bslma::Allocator*            basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificateEntity();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionCertificateEntity& operator=(const EncryptionCertificateEntity& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    // MRM
#if 0
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
#endif

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    // MRM
#if 0
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
#endif

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
bsl::ostream& operator<<(bsl::ostream&                stream,
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
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionCertificateEntity& value);


template <typename HASH_ALGORITHM>
void EncryptionCertificateEntity::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_version);
    hashAppend(algorithm, d_serialNumber);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionCertificateEntity& value)
{
    value.hash(algorithm);
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
    ntca::EncryptionCertificateEntity    d_entity;
    ntca::EncryptionCertificateSignatureAlgorithm d_signatureAlgorithm;
    ntca::EncryptionCertificateSignature d_signature;

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

    // MRM
#if 0
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
#endif

    /// Decode this object using the specified 'decoder'. Return the error.
    ntsa::Error decode(ntsa::AbstractSyntaxDecoder* decoder);

    /// Encode this object using the specified 'encoder'. Return the error.
    ntsa::Error encode(ntsa::AbstractSyntaxEncoder* encoder) const;

    // MRM
#if 0
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
#endif


    /// Return the certificate entity. 
    const ntca::EncryptionCertificateEntity& entity() const;

    /// Return the certificate signature algorithm.
    const ntca::EncryptionCertificateSignatureAlgorithm& signatureAlgorithm() const;

    /// Return the certificate signature value.
    const ntca::EncryptionCertificateSignature& signature() const;

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

}  // close package namespace
}  // close enterprise namespace
#endif
