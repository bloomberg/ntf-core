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

#ifndef INCLUDED_NTSA_ABSTRACT
#define INCLUDED_NTSA_ABSTRACT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bdlsb_memoutstreambuf.h>
#include <bdlt_datetimetz.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_ostream.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

class AbstractIntegerRepresentation;
class AbstractIntegerQuantity;
class AbstractIntegerQuantityUtil;
class AbstractInteger;
class AbstractIntegerUtil;
class AbstractString;
class AbstractBitSequence;
class AbstractByteSequence;
class AbstractObjectIdentifier;

/// Enumerate the Abstract Syntax Notation (ASN.1) tag classes.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_data
struct AbstractSyntaxTagClass {
    /// Enumerate the Abstract Syntax Notation (ASN.1) tag classes.
    enum Value {
        /// The type is native to ASN.1.
        e_UNIVERSAL = 0x00,

        /// The type is only valid for one specific application.
        e_APPLICATION = 0x040,

        /// The type is interpreted depending on its context, such as within a
        /// sequence.
        e_CONTEXT_SPECIFIC = 0x80,

        /// The type is defined in a private specification.
        e_PRIVATE = 0xC0
    };

    /// Load into the specified 'result' the enumerator having the specified
    /// 'value'. Return the error.
    static ntsa::Error fromValue(Value* result, bsl::size_t value);

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntsa::AbstractSyntaxTagClass
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         AbstractSyntaxTagClass::Value rhs);

/// Enumerate the Abstract Syntax Notation (ASN.1) tag types.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_data
struct AbstractSyntaxTagType {
    /// Enumerate the Abstract Syntax Notation (ASN.1) tag types.
    enum Value {
        /// The content is primitive, like an integer.
        e_PRIMITIVE = 0x00,

        /// The content is constructed of subsequent Tag-Length-Values.
        e_CONSTRUCTED = 0x20
    };

    /// Load into the specified 'result' the enumerator having the specified
    /// 'value'. Return the error.
    static ntsa::Error fromValue(Value* result, bsl::size_t value);

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntsa::AbstractSyntaxTagType
bsl::ostream& operator<<(bsl::ostream&                stream,
                         AbstractSyntaxTagType::Value rhs);

/// Enumerate the Abstract Syntax Notation (ASN.1) universal tag numbers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_data
struct AbstractSyntaxTagNumber {
    /// Enumerate the Abstract Syntax Notation (ASN.1) universal tag numbers.
    enum Value {
        /// The type is reserved for BER.
        e_END_OF_CONTENTS = 0,

        /// THe type is a boolean value.
        e_BOOLEAN = 1,

        /// The type is an integer value.
        e_INTEGER = 2,

        /// The type is a string of ones and zeroes.
        e_BIT_STRING = 3,

        /// The type is a sequence of bytes.
        e_OCTET_STRING = 4,

        /// The type indicates no value.
        e_NULL = 5,

        /// The type is an object identifier.
        e_OBJECT_IDENTIFIER = 6,

        /// The type is an object descriptor.
        e_OBJECT_DESCRIPTOR = 7,

        /// The type is external.
        e_EXTERNAL = 8,

        /// The type is a real number.
        e_REAL = 9,

        /// The type is an enumerated integer value.
        e_ENUMERATED = 10,

        /// The type is embedded PDV.
        e_EMBEDDED_PDV = 11,

        /// The type is a UTF-8 string.
        e_UTF8_STRING = 12,

        /// The type is a relative object identifier (OID).
        e_RELATIVE_OID = 13,

        /// The type is a sequence.
        e_SEQUENCE = 16,

        /// The type is a set.
        e_SET = 17,

        /// The type is a numeric string.
        e_NUMERIC_STRING = 18,

        /// The type is a printable string.
        e_PRINTABLE_STRING = 19,

        /// The type is a T61 string.
        e_T61_STRING = 20,

        /// The type is a Videotext string.
        e_VIDEOTEXT_STRING = 21,

        /// The type is an IA5 string.
        e_IA5_STRING = 22,

        /// The type is a UTC time.
        e_UTC_TIME = 23,

        /// The type is a generalized time.
        e_GENERALIZED_TIME = 24,

        /// The type is a graphic string.
        e_GRAPHIC_STRING = 25,

        /// The type is a visible string.
        e_VISIBLE_STRING = 26,

        /// The type is a general string.
        e_GENERAL_STRING = 27,

        /// The type is a universal string.
        e_UNIVERSAL_STRING = 28,

        /// The type is a character string.
        e_CHARACTER_STRING = 29,

        /// The type is a BMP string.
        e_BMP_STRING = 30,

        /// The tag number that indicates the long-form versions of encoded
        /// tag numbers.
        e_LONG_FORM = 31
    };

    /// Validate the specified 'value' is a legal tag number. Return the error.
    static ntsa::Error validate(bsl::size_t value);

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntsa::AbstractSyntaxTagNumber
bsl::ostream& operator<<(bsl::ostream&                  stream,
                         AbstractSyntaxTagNumber::Value rhs);

/// Enumerate the Abstract Syntax Notation (ASN.1) encoding formats.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_data
struct AbstractSyntaxFormat {
    /// Enumerate the Abstract Syntax Notation (ASN.1) encoding formats.
    enum Value {
        /// The data is encoding according to the Distinguished Encoding Rules
        /// (DER). DER uses definitive length form, that, is, uses leading
        /// length information instead of end-of-contents octets.
        e_DISTINGUISHED = 0,

        /// The data is encoded according to the Canonical Encoding Rules
        /// (CER). CER uses indefinite length form, that is, uses the
        /// end-of-contents octet instead of providing the length of the
        /// encoded data.
        e_CANONICAL = 1
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntsa::AbstractSyntaxFormat
bsl::ostream& operator<<(bsl::ostream&               stream,
                         AbstractSyntaxFormat::Value rhs);

/// Describe the configurable parameters for an Abstract Syntax Notation
/// (ASN.1) encoder.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b format:
/// The encoding format.
//
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class AbstractSyntaxEncoderOptions
{
    bdlb::NullableValue<AbstractSyntaxFormat::Value> d_format;

  public:
    /// Create a new bind context having the default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit AbstractSyntaxEncoderOptions(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new bind context having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    AbstractSyntaxEncoderOptions(const AbstractSyntaxEncoderOptions& original,
                                 bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~AbstractSyntaxEncoderOptions();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    AbstractSyntaxEncoderOptions& operator=(
        const AbstractSyntaxEncoderOptions& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the format to the specified 'value'.
    void setFormat(const ntsa::AbstractSyntaxFormat::Value& value);

    /// Return the format.
    const bdlb::NullableValue<ntsa::AbstractSyntaxFormat::Value>& format()
        const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const AbstractSyntaxEncoderOptions& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const AbstractSyntaxEncoderOptions& other) const;

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
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(AbstractSyntaxEncoderOptions);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::AbstractSyntaxEncoderOptions
bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const AbstractSyntaxEncoderOptions& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::AbstractSyntaxEncoderOptions
bool operator==(const AbstractSyntaxEncoderOptions& lhs,
                const AbstractSyntaxEncoderOptions& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractSyntaxEncoderOptions
bool operator!=(const AbstractSyntaxEncoderOptions& lhs,
                const AbstractSyntaxEncoderOptions& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractSyntaxEncoderOptions
bool operator<(const AbstractSyntaxEncoderOptions& lhs,
               const AbstractSyntaxEncoderOptions& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::AbstractSyntaxEncoderOptions
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                     algorithm,
                const AbstractSyntaxEncoderOptions& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                     algorithm,
                const AbstractSyntaxEncoderOptions& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.format());
}

/// Provide an Abstract Syntax Notation (ASN.1) encoder frame.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class AbstractSyntaxEncoderFrame
{
    typedef bsl::vector<AbstractSyntaxEncoderFrame*> FrameVector;

    AbstractSyntaxTagClass::Value d_tagClass;
    AbstractSyntaxTagType::Value  d_tagType;
    bsl::size_t                   d_tagNumber;
    bsl::size_t                   d_length;
    bsl::streambuf*               d_buffer_p;
    bdlsb::MemOutStreamBuf        d_header;
    bdlsb::MemOutStreamBuf        d_content;
    AbstractSyntaxEncoderFrame*   d_parent_p;
    FrameVector                   d_children;
    bslma::Allocator*             d_allocator_p;

  private:
    AbstractSyntaxEncoderFrame(const AbstractSyntaxEncoderFrame&)
        BSLS_KEYWORD_DELETED;
    AbstractSyntaxEncoderFrame& operator=(const AbstractSyntaxEncoderFrame&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new ASN.1 encoder frame. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit AbstractSyntaxEncoderFrame(AbstractSyntaxEncoderFrame* parent,
                                        bsl::streambuf*             buffer,
                                        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~AbstractSyntaxEncoderFrame();

    /// Create a new child frame having the specified 'tagClass', 'tagType',
    /// and 'tagNumber'. Return the new frame. Note that this object retains
    /// ownership of the frame.
    AbstractSyntaxEncoderFrame* createNext(
        AbstractSyntaxTagClass::Value  tagClass,
        AbstractSyntaxTagType::Value   tagType,
        AbstractSyntaxTagNumber::Value tagNumber);

    /// Create a new child frame having the specified 'tagClass', 'tagType',
    /// and 'tagNumber'. Return the new frame. Note that this object retains
    /// ownership of the frame.
    AbstractSyntaxEncoderFrame* createNext(
        AbstractSyntaxTagClass::Value tagClass,
        AbstractSyntaxTagType::Value  tagType,
        bsl::size_t                   tagNumber);

    /// Destroy the last child.
    void destroyLast();

    /// Set the tag class to the specified 'value'.
    void setTagClass(AbstractSyntaxTagClass::Value value);

    /// Set the tag type to the specified 'value'.
    void setTagType(AbstractSyntaxTagType::Value value);

    /// Set the tag number to the specified 'value'.
    void setTagNumber(AbstractSyntaxTagNumber::Value value);

    /// Set the tag number to the specified 'value'.
    void setTagNumber(bsl::size_t value);

    /// Write the specified 'data' byte to the content of this frame. Return
    /// the error.
    ntsa::Error writeHeader(bsl::uint8_t data);

    /// Write the specified 'data' having the specified 'size' to the content
    /// of this frame. Return the error.
    ntsa::Error writeHeader(const void* data, bsl::size_t size);

    /// Write the specified 'data' byte to the content of this frame. Return
    /// the error.
    ntsa::Error writeContent(bsl::uint8_t data);

    /// Write the specified 'data' having the specified 'size' to the content
    /// of this frame. Return the error.
    ntsa::Error writeContent(const void* data, bsl::size_t size);

    /// Write the base-128 encoding of the specified 'value' to the content
    /// of this frame. Return the error.
    ntsa::Error encodeContentBase128(bsl::uint64_t value);

    /// Synchronize the frame with its children, or its data if no children are
    /// defined. Return the error.
    ntsa::Error synchronize(bsl::size_t* length);

    /// Write the frame data and the frame data of all children, in depth-first
    /// order, to the specified 'buffer'.
    ntsa::Error flush(bsl::streambuf* buffer);

    /// Return the tag class.
    AbstractSyntaxTagClass::Value tagClass() const;

    /// Return the tag type.
    AbstractSyntaxTagType::Value tagType() const;

    /// Return the tag number.
    bsl::size_t tagNumber() const;

    /// Return the encoded tag and length, or null if there is no encoded
    /// tag and length.
    const bsl::uint8_t* header() const;

    /// Return the size, in bytes, of the encoded tag and length.
    bsl::size_t headerLength() const;

    /// Return the encoded content data, or null if there is no encoded
    /// content data.
    const bsl::uint8_t* content() const;

    /// Return the size, in bytes, of the encoded content data.
    bsl::size_t contentLength() const;

    /// Return the child at the specified 'index', or null if no such child
    /// at the 'index' exists.
    AbstractSyntaxEncoderFrame* childIndex(bsl::size_t index) const;

    /// Return the number of children.
    bsl::size_t childCount() const;

    /// Return the parent frame.
    AbstractSyntaxEncoderFrame* parent() const;
};

/// Provide an Abstract Syntax Notation (ASN.1) encoder.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class AbstractSyntaxEncoder
{
    bsl::streambuf*              d_buffer_p;
    AbstractSyntaxEncoderFrame*  d_root_p;
    AbstractSyntaxEncoderFrame*  d_current_p;
    AbstractSyntaxEncoderOptions d_config;
    bslma::Allocator*            d_allocator_p;

  private:
    AbstractSyntaxEncoder(const AbstractSyntaxEncoder&) BSLS_KEYWORD_DELETED;
    AbstractSyntaxEncoder& operator=(const AbstractSyntaxEncoder&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new ASN.1 encoder to the specified 'buffer'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractSyntaxEncoder(bsl::streambuf*   buffer,
                                   bslma::Allocator* basicAllocator = 0);

    /// Create a new ASN.1 encoder to the specified 'buffer' having the
    /// specified 'configuration'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit AbstractSyntaxEncoder(
        const AbstractSyntaxEncoderOptions& configuration,
        bsl::streambuf*                     buffer,
        bslma::Allocator*                   basicAllocator = 0);

    /// Destroy this object.
    ~AbstractSyntaxEncoder();

    /// Begin encoding a new construction having the specified 'tagClass',
    /// 'tagType', and 'tagNumber'. Return the error.
    ntsa::Error enterFrame(AbstractSyntaxTagClass::Value  tagClass,
                           AbstractSyntaxTagType::Value   tagType,
                           AbstractSyntaxTagNumber::Value tagNumber);

    /// Begin encoding a new construction having the specified 'tagClass',
    /// 'tagType', and 'tagNumber'. Return the error.
    ntsa::Error enterFrame(AbstractSyntaxTagClass::Value tagClass,
                           AbstractSyntaxTagType::Value  tagType,
                           bsl::size_t                   tagNumber);

    /// Encode a NULL primitive. Return the error.
    ntsa::Error encodePrimitiveNull();

    /// Encode a BOOLEAN primitive having the specified 'value'. Return the
    /// error.
    ntsa::Error encodePrimitiveValue(bool value);

    /// Encode an INTEGER primitive having the specified 'value'. Return the
    /// error.
    ntsa::Error encodePrimitiveValue(short value);

    /// Encode an INTEGER primitive having the specified 'value'. Return the
    /// error.
    ntsa::Error encodePrimitiveValue(unsigned short value);

    /// Encode an INTEGER primitive having the specified 'value'. Return the
    /// error.
    ntsa::Error encodePrimitiveValue(int value);

    /// Encode an INTEGER primitive having the specified 'value'. Return the
    /// error.
    ntsa::Error encodePrimitiveValue(unsigned int value);

    /// Encode an INTEGER primitive having the specified 'value'. Return the
    /// error.
    ntsa::Error encodePrimitiveValue(long value);

    /// Encode an INTEGER primitive having the specified 'value'. Return the
    /// error.
    ntsa::Error encodePrimitiveValue(unsigned long value);

    /// Encode an INTEGER primitive having the specified 'value'. Return the
    /// error.
    ntsa::Error encodePrimitiveValue(long long value);

    /// Encode an INTEGER primitive having the specified 'value'. Return the
    /// error.
    ntsa::Error encodePrimitiveValue(unsigned long long value);

    /// Encode an INTEGER primitive having the specified 'value'. Return the
    /// error.
    ntsa::Error encodePrimitiveValue(const AbstractInteger& value);

    /// Encode a UTF8 STRING primitive having the specified 'value'. Return the
    /// error.
    ntsa::Error encodePrimitiveValue(const bsl::string& value);

    /// Encode a string primitive having the specified 'value'. Return the
    /// error.
    ntsa::Error encodePrimitiveValue(const AbstractString& value);

    /// Encode a bit string primitive having the specified 'value'. Return the
    /// error.
    ntsa::Error encodePrimitiveValue(const AbstractBitSequence& value);

    /// Encode an octet primitive having the specified 'value'. Return the
    /// error.
    ntsa::Error encodePrimitiveValue(const AbstractByteSequence& value);

    /// Encode a UTC TIME or GENERALIZED TIME primitive having the specified
    /// 'value'. Return the error.
    ntsa::Error encodePrimitiveValue(const bdlt::Datetime& value);

    /// Encode a UTC TIME or GENERALIZED TIME primitive having the specified
    /// 'value'. Return the error.
    ntsa::Error encodePrimitiveValue(const bdlt::DatetimeTz& value);

    /// Encode an OBJECT IDENTIFIER primitive having the specified 'value'.
    /// Return the error.
    ntsa::Error encodePrimitiveValue(const AbstractObjectIdentifier& value);

    // Complete encoding the current construction. Return the error.
    ntsa::Error leaveFrame();

    /// Return the configuration.
    const AbstractSyntaxEncoderOptions& configuration() const;

    /// Return the buffer.
    bsl::streambuf* buffer() const;
};

/// Provide an Abstract Syntax Notation (ASN.1) encoder utilities.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class AbstractSyntaxEncoderUtil
{
  private:
    /// Return the number of significant bits in the specified 'value'.
    static bsl::size_t numSignificantBits(bsl::uint64_t value);

  public:
    /// Write the specified literal 'data' byte to the specified 'destination'.
    /// Return the error.
    static ntsa::Error write(bsl::streambuf* destination, bsl::uint8_t data);

    /// Write the specified literal 'data' having the specified 'size' to the
    /// specified 'destination'.
    static ntsa::Error write(bsl::streambuf* destination,
                             const void*     data,
                             bsl::size_t     size);

    /// Write the encoding of the specified 'tagClass', 'tagType', and
    /// 'tagNumber' to the specified 'destination'. Return the error.
    static ntsa::Error encodeTag(bsl::streambuf*                destination,
                                 AbstractSyntaxTagClass::Value  tagClass,
                                 AbstractSyntaxTagType::Value   tagType,
                                 AbstractSyntaxTagNumber::Value tagNumber);

    /// Write the encoding of the specified 'tagClass', 'tagType', and
    /// 'tagNumber' to the specified 'destination'. Return the error.
    static ntsa::Error encodeTag(bsl::streambuf*               destination,
                                 AbstractSyntaxTagClass::Value tagClass,
                                 AbstractSyntaxTagType::Value  tagType,
                                 bsl::size_t                   tagNumber);

    /// Write the encoding of the specified definite 'length' to the specified
    /// 'destination'. Return the error.
    static ntsa::Error encodeLength(bsl::streambuf* destination,
                                    bsl::size_t     length);

    /// Write the encoding of the indefinite length marker to the specified
    /// 'destination'. Return the error.
    static ntsa::Error encodeLengthIndefinite(bsl::streambuf* destination);

    /// Write the encoding of the end-of-content marker to the specified
    /// 'destination'. Return the error.
    static ntsa::Error encodeTerminator(bsl::streambuf* destination);

    /// Write the base-128 encoding of the specified 'value' to the specified
    /// 'destination'. Return the error.
    static ntsa::Error encodeIntegerBase128(bsl::streambuf* destination,
                                            bsl::uint64_t   value);

    /// Synchronize the specified 'destination' with its underlying device.
    /// Return the error.
    static ntsa::Error synchronize(bsl::streambuf* destination);
};

/// Describe the configurable parameters for an Abstract Syntax Notation
/// (ASN.1) decoder.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b format:
/// The encoding format.
//
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class AbstractSyntaxDecoderOptions
{
    bdlb::NullableValue<AbstractSyntaxFormat::Value> d_format;

  public:
    /// Create a new bind context having the default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit AbstractSyntaxDecoderOptions(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new bind context having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    AbstractSyntaxDecoderOptions(const AbstractSyntaxDecoderOptions& original,
                                 bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~AbstractSyntaxDecoderOptions();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    AbstractSyntaxDecoderOptions& operator=(
        const AbstractSyntaxDecoderOptions& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the format to the specified 'value'.
    void setFormat(const ntsa::AbstractSyntaxFormat::Value& value);

    /// Return the format.
    const bdlb::NullableValue<ntsa::AbstractSyntaxFormat::Value>& format()
        const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const AbstractSyntaxDecoderOptions& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const AbstractSyntaxDecoderOptions& other) const;

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
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(AbstractSyntaxDecoderOptions);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::AbstractSyntaxDecoderOptions
bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const AbstractSyntaxDecoderOptions& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::AbstractSyntaxDecoderOptions
bool operator==(const AbstractSyntaxDecoderOptions& lhs,
                const AbstractSyntaxDecoderOptions& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractSyntaxDecoderOptions
bool operator!=(const AbstractSyntaxDecoderOptions& lhs,
                const AbstractSyntaxDecoderOptions& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractSyntaxDecoderOptions
bool operator<(const AbstractSyntaxDecoderOptions& lhs,
               const AbstractSyntaxDecoderOptions& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::AbstractSyntaxDecoderOptions
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                     algorithm,
                const AbstractSyntaxDecoderOptions& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                     algorithm,
                const AbstractSyntaxDecoderOptions& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.format());
}

/// Provide an Abstract Syntax Notation (ASN.1) decoder frame.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b error:
/// The error detected when performing the operation.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class AbstractSyntaxDecoderFrame
{
    AbstractSyntaxTagClass::Value    d_tagClass;
    AbstractSyntaxTagType::Value     d_tagType;
    bsl::size_t                      d_tagNumber;
    bsl::uint64_t                    d_tagPosition;
    bsl::size_t                      d_tagLength;
    bsl::uint64_t                    d_contentPosition;
    bdlb::NullableValue<bsl::size_t> d_contentLength;

  public:
    /// Create a new ASN.1 decoder frame having the default value.
    AbstractSyntaxDecoderFrame();

    /// Create a new ASN.1 decoder frame having the same value as the specified
    /// 'original' object.
    AbstractSyntaxDecoderFrame(const AbstractSyntaxDecoderFrame& original);

    /// Destroy this object.
    ~AbstractSyntaxDecoderFrame();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    AbstractSyntaxDecoderFrame& operator=(
        const AbstractSyntaxDecoderFrame& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the tag class to the specified 'value'.
    void setTagClass(AbstractSyntaxTagClass::Value value);

    /// Set the tag type to the specified 'value'.
    void setTagType(AbstractSyntaxTagType::Value value);

    /// Set the tag number to the specified 'value'.
    void setTagNumber(AbstractSyntaxTagNumber::Value value);

    /// Set the tag number to the specified 'value'.
    void setTagNumber(bsl::size_t value);

    /// Set the absolute position of the beginning of the tag in the data
    /// stream to the speicfied 'value'.
    void setTagPosition(bsl::uint64_t value);

    /// Set the number of bytes comprising the tag (and the length) to the 
    /// specified 'value'.
    void setTagLength(bsl::size_t value);

    /// Set the absolute position of the beginning of the content in the data
    /// stream to the specified 'value'.
    void setContentPosition(bsl::uint64_t value);

    /// Set the number of bytes comprising the content to the specified
    /// 'value'.
    void setContentLength(bsl::size_t value);

    /// Return the tag class.
    AbstractSyntaxTagClass::Value tagClass() const;

    /// Return the tag type.
    AbstractSyntaxTagType::Value tagType() const;

    /// Return the tag number.
    bsl::size_t tagNumber() const;

    /// Return the absolute position of the beginning of the tag in the data
    /// stream.
    bsl::uint64_t tagPosition() const;

    /// Return the number of bytes comprising the tag (and the length).
    bsl::size_t tagLength() const;

    /// Return the absolute position of the beginning of the content.
    bsl::uint64_t contentPosition() const;

    /// Return the length, or null if the length is indefinite, instead marked
    /// by an end-of-content octet sequence.
    const bdlb::NullableValue<bsl::size_t>& contentLength() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const AbstractSyntaxDecoderFrame& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const AbstractSyntaxDecoderFrame& other) const;

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
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(AbstractSyntaxDecoderFrame);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::AbstractSyntaxDecoderFrame
bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const AbstractSyntaxDecoderFrame& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::AbstractSyntaxDecoderFrame
bool operator==(const AbstractSyntaxDecoderFrame& lhs,
                const AbstractSyntaxDecoderFrame& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractSyntaxDecoderFrame
bool operator!=(const AbstractSyntaxDecoderFrame& lhs,
                const AbstractSyntaxDecoderFrame& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractSyntaxDecoderFrame
bool operator<(const AbstractSyntaxDecoderFrame& lhs,
               const AbstractSyntaxDecoderFrame& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::AbstractSyntaxDecoderFrame
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                   algorithm,
                const AbstractSyntaxDecoderFrame& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                   algorithm,
                const AbstractSyntaxDecoderFrame& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.tagClass());
    hashAppend(algorithm, value.tagType());
    hashAppend(algorithm, value.tagNumber());
    hashAppend(algorithm, value.tagPosition());
    hashAppend(algorithm, value.tagLength());
    hashAppend(algorithm, value.contentPosition());
    hashAppend(algorithm, value.contentLength());
}

/// Provide an Abstract Syntax Notation (ASN.1) decoder.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class AbstractSyntaxDecoder
{
    typedef bsl::vector<AbstractSyntaxDecoderFrame> ContextStack;

    bsl::streambuf*              d_buffer_p;
    ContextStack                 d_contextStack;
    AbstractSyntaxDecoderFrame   d_contextDefault;
    AbstractSyntaxDecoderOptions d_config;
    bslma::Allocator*            d_allocator_p;

  private:
    AbstractSyntaxDecoder(const AbstractSyntaxDecoder&) BSLS_KEYWORD_DELETED;
    AbstractSyntaxDecoder& operator=(const AbstractSyntaxDecoder&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new ASN.1 decoder from the specified 'buffer'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractSyntaxDecoder(bsl::streambuf*   buffer,
                                   bslma::Allocator* basicAllocator = 0);

    /// Create a new ASN.1 decoder from the specified 'buffer' having the
    /// specified 'configuration'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit AbstractSyntaxDecoder(
        const AbstractSyntaxDecoderOptions& configuration,
        bsl::streambuf*                     buffer,
        bslma::Allocator*                   basicAllocator = 0);

    /// Destroy this object.
    ~AbstractSyntaxDecoder();

    /// Decode a tag and its length. Note that the tag and its length becomes
    /// the new top of the context stack. Also note that the caller is
    /// responsible for calling 'leaveTag' to pop the context stack. Return the
    /// error.
    ntsa::Error decodeContext();

    /// Decode a tag and its length and load the result into the specified
    /// 'result'. Note that the tag and its length becomes the new top of the
    /// context stack. Also note that the caller is responsible for calling
    /// 'leaveTag' to pop the context stack. Return the error.
    ntsa::Error decodeContext(AbstractSyntaxDecoderFrame* result);

    /// Decode a tag and its length having the specified 'tagClass', 'tagType',
    /// and 'tagNumber'. Note that the tag and its length becomes the new top
    /// of the context stack. Also note that the caller is responsible for
    /// calling 'leaveTag' to pop the context stack. Return the error.
    ntsa::Error decodeContext(AbstractSyntaxTagClass::Value  tagClass,
                              AbstractSyntaxTagType::Value   tagType,
                              AbstractSyntaxTagNumber::Value tagNumber);

    /// Decode a tag and its length having the specified 'tagClass', 'tagType',
    /// and 'tagNumber'. Note that the tag and its length becomes the new top
    /// of the context stack. Also note that the caller is responsible for
    /// calling 'leaveTag' to pop the context stack. Return the error.
    ntsa::Error decodeContext(AbstractSyntaxTagClass::Value tagClass,
                              AbstractSyntaxTagType::Value  tagType,
                              bsl::size_t                   tagNumber);

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE tag number
    /// NULL according to the top of the context stack. Return the error.
    ntsa::Error decodePrimitiveNull();

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE tag number
    /// END_OF_CONTENT according to the top of the context stack. Return the
    /// error.
    ntsa::Error decodePrimitiveEnd();

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE tag number
    /// BOOLEAN according to the top of the context stack and load the result
    /// into the specified 'result'. Return the error.
    ntsa::Error decodePrimitiveValue(bool* result);

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE tag number
    /// INTEGER according to the top of the context stack and load the result
    /// into the specified 'result'. Return the error.
    ntsa::Error decodePrimitiveValue(short* result);

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE tag number
    /// INTEGER according to the top of the context stack and load the result
    /// into the specified 'result'. Return the error.
    ntsa::Error decodePrimitiveValue(unsigned short* result);

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE tag number
    /// INTEGER according to the top of the context stack and load the result
    /// into the specified 'result'. Return the error.
    ntsa::Error decodePrimitiveValue(int* result);

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE tag number
    /// INTEGER according to the top of the context stack and load the result
    /// into the specified 'result'. Return the error.
    ntsa::Error decodePrimitiveValue(unsigned int* result);

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE tag number
    /// INTEGER according to the top of the context stack and load the result
    /// into the specified 'result'. Return the error.
    ntsa::Error decodePrimitiveValue(long* result);

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE tag number
    /// INTEGER according to the top of the context stack and load the result
    /// into the specified 'result'. Return the error.
    ntsa::Error decodePrimitiveValue(unsigned long* result);

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE tag number
    /// INTEGER according to the top of the context stack and load the result
    /// into the specified 'result'. Return the error.
    ntsa::Error decodePrimitiveValue(long long* result);

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE tag number
    /// INTEGER according to the top of the context stack and load the result
    /// into the specified 'result'. Return the error.
    ntsa::Error decodePrimitiveValue(unsigned long long* result);

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE tag number
    /// INTEGER according to the top of the context stack and load the result
    /// into the specified 'result'. Return the error.
    ntsa::Error decodePrimitiveValue(AbstractInteger* result);

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE whose tag number
    /// is either VISIBLE_STRING, PRINTABLE_STRING, or UTF8_STRING according to
    /// the top of the context stack and load the result into the specified
    /// 'result'. Return the error.
    ntsa::Error decodePrimitiveValue(bsl::string* result);

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE whose tag number
    /// is any of the string types according to the top of the context stack
    /// and load the result into the specified 'result'. Return the error.
    ntsa::Error decodePrimitiveValue(AbstractString* result);

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE whose tag number
    /// is any of the string types according to the top of the context stack
    /// and load the result into the specified 'result'. Return the error.
    ntsa::Error decodePrimitiveValue(AbstractBitSequence* result);

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE whose tag number
    /// is any of the string types according to the top of the context stack
    /// and load the result into the specified 'result'. Return the error.
    ntsa::Error decodePrimitiveValue(AbstractByteSequence* result);

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE whose tag number
    /// is either e_UTC_TIME or e_GENERALIZED_TIME according to the top
    /// of the context stack and load the result into the specified 'result'.
    /// Return the error.
    ntsa::Error decodePrimitiveValue(bdlt::Datetime* result);

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE whose tag number
    /// is either e_UTC_TIME or e_GENERALIZED_TIME according to the top
    /// of the context stack and load the result into the specified 'result'.
    /// Return the error.
    ntsa::Error decodePrimitiveValue(bdlt::DatetimeTz* result);

    /// Decode a value of tag class UNIVERSAL type PRIMITIVE tag number
    /// OBJECT_IDENTIFIER according to the top of the context stack and load
    /// the result into the specified 'result'. Return the error.
    ntsa::Error decodePrimitiveValue(AbstractObjectIdentifier* result);

    /// Pop the current tag-length-value context from the stack. Return the
    /// error.
    ntsa::Error decodeContextComplete();

    /// Skip over any remaining un-decoded bytes of the current context. Return
    /// the error. 
    ntsa::Error skip();

    /// Seek to the specified absolute read 'position'. Return the error.
    ntsa::Error seek(bsl::uint64_t position);

    /// Return the current read position.
    bsl::uint64_t position() const;

    /// Return the current structural depth. 
    bsl::size_t depth() const;

    /// Return the current tag-length-value context at the top of the stack.
    const AbstractSyntaxDecoderFrame& current() const;

    /// Return the configuration.
    const AbstractSyntaxDecoderOptions& configuration() const;

    /// Return the buffer.
    bsl::streambuf* buffer() const;
};

/// Provide an Abstract Syntax Notation (ASN.1) decoder utilities.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class AbstractSyntaxDecoderUtil
{
  public:
    /// Return the current read position in the specified 'source'.
    static bsl::uint64_t position(bsl::streambuf* source);

    /// Seek to the specified absolute read 'position' in the specified 
    /// 'source'. Return the error.
    static ntsa::Error seek(bsl::streambuf* source, bsl::uint64_t position);

    /// Read a single byte from the input buffer and load the byte read into
    /// the specified 'result'. Return the error.
    static ntsa::Error read(bsl::uint8_t* result, bsl::streambuf* source);

    /// Read the specified 'size' number of bytes from the input buffer and
    /// load the bytes read into the specified 'result'. Return the error.
    static ntsa::Error read(void*           result,
                            bsl::size_t     size,
                            bsl::streambuf* source);

    /// Decode the tag from the input buffer and load the class into the
    /// the specified 'tagClass', the type into the specified 'tagType', and
    /// the number into the specified 'number'. Return the error.
    static ntsa::Error decodeTag(ntsa::AbstractSyntaxTagClass::Value* tagClass,
                                 ntsa::AbstractSyntaxTagType::Value*  tagType,
                                 bsl::size_t*    tagNumber,
                                 bsl::streambuf* source);

    /// Decode the length from the input buffer and load it into the specified
    /// 'result'. Return the error.
    static ntsa::Error decodeLength(bdlb::NullableValue<bsl::size_t>* result,
                                    bsl::streambuf*                   source);

    // Load into the specified 'result' an integer encoded in base-128 from
    // from the specified 'source'. Return the error.
    static ntsa::Error decodeIntegerBase128(bsl::uint64_t*  result,
                                            bsl::streambuf* source);
};

/// Provide an Abstract Syntax Notation (ASN.1) object identifier.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class AbstractObjectIdentifier
{
    bsl::vector<bsl::uint64_t> d_data;

  public:
    /// Create a new ASN.1 object identifier. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit AbstractObjectIdentifier(bslma::Allocator* basicAllocator = 0);

    /// Create a new ASN.1 object identifier having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit AbstractObjectIdentifier(const AbstractObjectIdentifier& original,
                                      bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~AbstractObjectIdentifier();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    AbstractObjectIdentifier& operator=(const AbstractObjectIdentifier& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    // Resize the string to store exactly the specified 'size' number of bytes.
    void resize(bsl::size_t size);

    /// Append the specified 'value' to the end of the data.
    void append(bsl::uint64_t value);

    // Append the specified 'data' having the specified 'size'.
    void append(const bsl::uint64_t* data, bsl::size_t size);

    /// Set the data at the specified 'index' to the specified 'value'.
    void set(bsl::size_t index, bsl::uint64_t value);

    /// Return the data at the specified 'index'.
    bsl::uint64_t get(bsl::size_t index) const;

    /// Return the array of integers in the identifier.
    const bsl::uint64_t* data() const;

    /// Return the number of integers in the identifier.
    bsl::size_t size() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const AbstractObjectIdentifier& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const AbstractObjectIdentifier& other) const;

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
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(AbstractObjectIdentifier);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntsa::AbstractObjectIdentifier
bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const AbstractObjectIdentifier& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntsa::AbstractObjectIdentifier
bool operator==(const AbstractObjectIdentifier& lhs,
                const AbstractObjectIdentifier& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntsa::AbstractObjectIdentifier
bool operator!=(const AbstractObjectIdentifier& lhs,
                const AbstractObjectIdentifier& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractObjectIdentifier
bool operator<(const AbstractObjectIdentifier& lhs,
               const AbstractObjectIdentifier& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::AbstractObjectIdentifier
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                 algorithm,
                const AbstractObjectIdentifier& value);

template <typename HASH_ALGORITHM>
void AbstractObjectIdentifier::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_data);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                 algorithm,
                const AbstractObjectIdentifier& value)
{
    value.hash(algorithm);
}

/// Provide an Abstract Syntax Notation (ASN.1) object identifier.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class AbstractString
{
    AbstractSyntaxTagNumber::Value d_type;
    bsl::vector<bsl::uint8_t>      d_data;

  public:
    /// Create a new ASN.1 string. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit AbstractString(bslma::Allocator* basicAllocator = 0);

    /// Create a new ASN.1 string having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit AbstractString(const AbstractString& original,
                            bslma::Allocator*     basicAllocator = 0);

    /// Destroy this object.
    ~AbstractString();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    AbstractString& operator=(const AbstractString& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    // Resize the string to store exactly the specified 'size' number of bytes.
    void resize(bsl::size_t size);

    /// Append the specified 'value' to the end of the data.
    void append(bsl::uint8_t value);

    /// Set the data at the specified 'index' to the specified 'value'.
    void set(bsl::size_t index, bsl::uint8_t value);

    /// Set the type to the specified 'value'.
    void setType(AbstractSyntaxTagNumber::Value value);

    /// Return the tag number that indicates the type of string.
    AbstractSyntaxTagNumber::Value type() const;

    /// Return the data at the specified 'index'.
    bsl::uint8_t get(bsl::size_t index) const;

    /// Return the data. Note that the data is null if the size is zero, and
    /// may contain embedded nulls but not necessarily null-terminated if the
    /// size is non-zero.
    const bsl::uint8_t* data() const;

    /// Return the number of bytes of data.
    bsl::size_t size() const;

    /// Load into the specified 'result' the printable, ASCII, or UTF-8 string
    /// represented by this object. Return the error.
    ntsa::Error convert(bsl::string* result) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const AbstractString& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const AbstractString& other) const;

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
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(AbstractString);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntsa::AbstractString
bsl::ostream& operator<<(bsl::ostream& stream, const AbstractString& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntsa::AbstractString
bool operator==(const AbstractString& lhs, const AbstractString& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntsa::AbstractString
bool operator!=(const AbstractString& lhs, const AbstractString& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractString
bool operator<(const AbstractString& lhs, const AbstractString& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::AbstractString
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const AbstractString& value);

template <typename HASH_ALGORITHM>
void AbstractString::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_type);
    hashAppend(algorithm, d_data);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const AbstractString& value)
{
    value.hash(algorithm);
}



/// Define a type alias for the representation of an abstract bit.
typedef bool AbstractBit;

/// Provide an Abstract Syntax Notation (ASN.1) bit string.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class AbstractBitSequence
{
    AbstractSyntaxTagNumber::Value d_type;
    bsl::vector<bsl::uint8_t>      d_data;

  public:
    /// Create a new ASN.1 bit string. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit AbstractBitSequence(bslma::Allocator* basicAllocator = 0);

    /// Create a new ASN.1 bit string having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit AbstractBitSequence(const AbstractBitSequence& original,
                            bslma::Allocator*     basicAllocator = 0);

    /// Destroy this object.
    ~AbstractBitSequence();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    AbstractBitSequence& operator=(const AbstractBitSequence& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    // Resize the string to store exactly the specified 'size' number of bytes.
    void resize(bsl::size_t size);

    /// Append the specified 'value' to the end of the data.
    void append(AbstractBit value);

    /// Set the data at the specified 'index' to the specified 'value'.
    void set(bsl::size_t index, AbstractBit value);

    /// Set the type to the specified 'value'.
    void setType(AbstractSyntaxTagNumber::Value value);

    /// Return the tag number that indicates the type of string.
    AbstractSyntaxTagNumber::Value type() const;

    /// Return the data at the specified 'index'.
    AbstractBit get(bsl::size_t index) const;

    /// Return the data. Note that the data is null if the size is zero, and
    /// may contain embedded nulls but not necessarily null-terminated if the
    /// size is non-zero.
    const bsl::uint8_t* data() const;

    /// Return the number of bytes of data.
    bsl::size_t size() const;

    /// Load into the specified 'result' the printable, ASCII, or UTF-8 string
    /// represented by this object. Return the error.
    ntsa::Error convert(bsl::string* result) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const AbstractBitSequence& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const AbstractBitSequence& other) const;

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
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(AbstractBitSequence);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntsa::AbstractBitSequence
bsl::ostream& operator<<(bsl::ostream& stream, const AbstractBitSequence& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntsa::AbstractBitSequence
bool operator==(const AbstractBitSequence& lhs, const AbstractBitSequence& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntsa::AbstractBitSequence
bool operator!=(const AbstractBitSequence& lhs, const AbstractBitSequence& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractBitSequence
bool operator<(const AbstractBitSequence& lhs, const AbstractBitSequence& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::AbstractBitSequence
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const AbstractBitSequence& value);

template <typename HASH_ALGORITHM>
void AbstractBitSequence::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_type);
    hashAppend(algorithm, d_data);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const AbstractBitSequence& value)
{
    value.hash(algorithm);
}



/// Define a type alias for the representation of an abstract byte (octet).
typedef bsl::uint8_t AbstractByte;






/// Provide an Abstract Syntax Notation (ASN.1) byte (octet) string.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class AbstractByteSequence
{
    AbstractSyntaxTagNumber::Value d_type;
    bsl::vector<bsl::uint8_t>      d_data;

  public:
    /// Create a new ASN.1 string. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit AbstractByteSequence(bslma::Allocator* basicAllocator = 0);

    /// Create a new ASN.1 string having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit AbstractByteSequence(const AbstractByteSequence& original,
                            bslma::Allocator*     basicAllocator = 0);

    /// Destroy this object.
    ~AbstractByteSequence();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    AbstractByteSequence& operator=(const AbstractByteSequence& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    // Resize the string to store exactly the specified 'size' number of bytes.
    void resize(bsl::size_t size);

    /// Append the specified 'value' to the end of the data.
    void append(AbstractByte value);

    /// Set the data at the specified 'index' to the specified 'value'.
    void set(bsl::size_t index, AbstractByte value);

    /// Set the type to the specified 'value'.
    void setType(AbstractSyntaxTagNumber::Value value);

    /// Return the tag number that indicates the type of string.
    AbstractSyntaxTagNumber::Value type() const;

    /// Return the data at the specified 'index'.
    AbstractByte get(bsl::size_t index) const;

    /// Return the data. Note that the data is null if the size is zero, and
    /// may contain embedded nulls but not necessarily null-terminated if the
    /// size is non-zero.
    const bsl::uint8_t* data() const;

    /// Return the number of bytes of data.
    bsl::size_t size() const;

    /// Load into the specified 'result' the printable, ASCII, or UTF-8 string
    /// represented by this object. Return the error.
    ntsa::Error convert(bsl::string* result) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const AbstractByteSequence& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const AbstractByteSequence& other) const;

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
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(AbstractByteSequence);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntsa::AbstractByteSequence
bsl::ostream& operator<<(bsl::ostream& stream, const AbstractByteSequence& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntsa::AbstractByteSequence
bool operator==(const AbstractByteSequence& lhs, const AbstractByteSequence& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntsa::AbstractByteSequence
bool operator!=(const AbstractByteSequence& lhs, const AbstractByteSequence& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractByteSequence
bool operator<(const AbstractByteSequence& lhs, const AbstractByteSequence& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::AbstractByteSequence
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const AbstractByteSequence& value);

template <typename HASH_ALGORITHM>
void AbstractByteSequence::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_type);
    hashAppend(algorithm, d_data);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const AbstractByteSequence& value)
{
    value.hash(algorithm);
}






/// Define a type alias for the representation of an abstract octet.
typedef AbstractByte AbstractOctet;

/// Define a type alias for the representation of an abstract octet sequence.
typedef AbstractByteSequence AbstractOctetSequence;

































/// Enumerate the signs of the representation of an abstract integer.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_data
struct AbstractIntegerSign {
    /// Enumerate the signs of the representation of an abstract integer.
    enum Value {
        /// The sign is negative.
        e_NEGATIVE = -1,

        /// The sign is positive.
        e_POSITIVE = 1
    };

    /// Return the specified 'sign', but flipped.
    static Value flip(Value sign);

    /// Return the multiplier for the specified 'sign'.
    static int multiplier(Value sign);

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntsa::AbstractIntegerSign
bsl::ostream& operator<<(bsl::ostream& stream, AbstractIntegerSign::Value rhs);

/// Enumerate the supported bases of the representation of an abstract integer.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_data
struct AbstractIntegerBase {
    /// Enumerate the supported bases of the representation of an abstract
    /// integer.
    enum Value {
        /// The number is represented in a base computed from the native
        /// machine register width.
        e_NATIVE = 0,

        /// The number is represented in base-2.
        e_BINARY = 1,

        /// The number is represented in base-8.
        e_OCTAL = 2,

        /// The number is represented in base-10.
        e_DECIMAL = 3,

        /// The number is represented in base-16.
        e_HEXADECIMAL = 4
    };

    /// Return the radix, or number of representable digits, in the specified
    /// 'base'.
    static bsl::uint64_t radix(AbstractIntegerBase::Value base);

    /// Return true if the specified 'value' is valid within the specified
    /// 'base', otherwise return false.
    static bool validate(AbstractIntegerBase::Value base, bsl::uint64_t value);

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntsa::AbstractIntegerBase
bsl::ostream& operator<<(bsl::ostream& stream, AbstractIntegerBase::Value rhs);

/// Provide a representation for an abstract unsigned integer.
///
/// @details
/// In a positional numeral system, a large integer of any size can be
/// represented as a sequence of digits in a "place" (called blocks in this
/// implementation), where each block fits inside a machine register. We can
/// arbitrarily choose a base `B` (or radix) for our system, where each block
/// stores `[0, B)`. We choose a machine register width so that no single-place
/// multiplication plus a carry will ever overflow the register. So for 32-bit
/// machines, we choose a 16-bit register, and for 64-bit machines, we choose a
/// 32-bit register. Blocks are order least-significant to most-significant.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class AbstractIntegerRepresentation
{
  public:
#if defined(BSLS_PLATFORM_CPU_64_BIT)

    /// Define a type alias for unsigned integer type that represents a block.
    /// Also called a place or a limb in other implementations. This type must
    /// be sufficient to store the desired radix.
    typedef bsl::uint32_t Block;

#else

    /// Define a type alias for unsigned integer type that represents a block.
    /// Also called a place or a limb in other implementations. This type must
    /// be sufficient to store the desired radix.
    typedef bsl::uint16_t Block;

#endif

    /// Define a type alias for a vector of blocks.
    typedef bsl::vector<Block> BlockVector;

    /// Define a type alias for the allocator used by a vector of blocks.
    typedef bsl::allocator<Block> BlockVectorAllocator;

    enum { k_BITS_PER_BLOCK = sizeof(Block) * 8 };

  private:
    AbstractIntegerBase::Value d_base;
    BlockVector                d_data;
    bslma::Allocator*          d_allocator_p;

  private:
    /// Count the number of leading bits that are zero in the specified
    /// 'value'.
    static bsl::size_t countLeadingZeroes(bsl::uint8_t value);

    /// Count the number of leading bits that are zero in the specified
    /// 'value'.
    static bsl::size_t countLeadingZeroes(bsl::uint16_t value);

    /// Count the number of leading bits that are zero in the specified
    /// 'value'.
    static bsl::size_t countLeadingZeroes(bsl::uint32_t value);

    /// Count the number of leading bits that are zero in the specified
    /// 'value'.
    static bsl::size_t countLeadingZeroes(bsl::uint64_t value);

  public:
    /// Create a new abstract unsigned integer representation having the
    /// default value in the default base. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit AbstractIntegerRepresentation(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract unsigned integer representation having the
    /// default value in the specified 'base'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit AbstractIntegerRepresentation(
        AbstractIntegerBase::Value base,
        bslma::Allocator*          basicAllocator = 0);

    /// Create a new abstract unsigned integer representation having the same
    /// value as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    AbstractIntegerRepresentation(
        const AbstractIntegerRepresentation& original,
        bslma::Allocator*                    basicAllocator = 0);

    /// Destroy this object.
    ~AbstractIntegerRepresentation();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    AbstractIntegerRepresentation& operator=(
        const AbstractIntegerRepresentation& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Resize the quantity to contain the specified 'size' number of blocks.
    void resize(bsl::size_t size);

    /// Swap the value of the specified 'other' object with the value of this
    /// object.
    void swap(AbstractIntegerRepresentation& other);

    /// Assign the value of the specified 'other' object to this object.
    void assign(const AbstractIntegerRepresentation& other);

    /// Assign the specified 'value' to this object.
    void assign(bsl::uint64_t value);

    /// Normalize the representation of this object so that the
    /// most-significant block is not zero, unless the value of this
    /// representation is zero.
    void normalize();

    /// Set the block at the specified 'index' to the specified 'value'.
    /// The behavior is undefined unless 'index' is valid at 'value' is a
    /// valid value in the current base of this object.
    void set(bsl::size_t index, Block value);

    /// Push the specified 'value' as the new most-significant block.
    void push(Block value);

    /// Pop the most-significant block.
    void pop();

    /// Return the block at the specified 'index'.
    Block get(bsl::size_t index) const;

    /// Return the base.
    AbstractIntegerBase::Value base() const;

    /// Return the radix, or number of representable digits, in the base.
    bsl::uint64_t radix() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const AbstractIntegerRepresentation& other) const;

    /// Compare the value of this object to the specified 'other' object.
    /// Return -1 if this object is less than the 'other' object, 0 if this
    /// object is equal to the 'other' object, and 0 if this object is equal to
    /// the 'other' object.
    int compare(const AbstractIntegerRepresentation& other) const;

    /// Return the number of blocks.
    bsl::size_t size() const;

    /// Return true if the number is zero, otherwise return false.
    bool isZero() const;

    /// Return true if the number is one, otherwise return false.
    bool isOne() const;

    /// Return true if this object is *not* an alias for the specified 'other'
    /// object, otherwise return false.
    bool isNotAliasOf(const AbstractIntegerRepresentation* other) const;

    /// Return true if this object is *not* an alias for the specified 'other'
    /// object, otherwise return false.
    bool isNotAliasOf(const AbstractIntegerRepresentation& other) const;

    /// Return the allocator used to supply memory.
    bslma::Allocator* allocator() const;

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

    /// Add the specified 'addend2' to the specified 'addend1' and load the
    /// result into the specified 'sum'.
    static void add(AbstractIntegerRepresentation*       sum,
                    const AbstractIntegerRepresentation& addend1,
                    const AbstractIntegerRepresentation& addend2);

    /// Subtract the specified 'subtrahend' from the specified 'minuend' and
    /// load the result into the specified 'difference'. If 'subtrahend' is
    /// greater than 'minuend', clamp the 'difference' to zero.
    static void subtract(AbstractIntegerRepresentation*       difference,
                         const AbstractIntegerRepresentation& minuend,
                         const AbstractIntegerRepresentation& subtrahend);

    /// Multiply the specified 'multiplicand' by the specified 'multiplier'
    /// and load the result into the specified 'product'.
    static void multiply(AbstractIntegerRepresentation*       product,
                         const AbstractIntegerRepresentation& multiplicand,
                         const AbstractIntegerRepresentation& multiplier);

    /// Divide the specified 'dividend' by the specified 'divisor'
    /// and load the result into the specified 'quotient' and the modulus into
    /// the specified 'remainder'.
    static void divide(AbstractIntegerRepresentation*       quotient,
                       AbstractIntegerRepresentation*       remainder,
                       const AbstractIntegerRepresentation& dividend,
                       const AbstractIntegerRepresentation& divisor);

    /// Parse the specified 'text' representation containing a sequence of
    /// characters in either base-2, base-8, base-10, or base-16, and load the
    /// result into the specified 'result' and the sign into the specified
    /// 'sign'. Return true if 'text' is valid, or false otherwise.
    static bool parse(AbstractIntegerRepresentation* result,
                      AbstractIntegerSign::Value*    sign,
                      const bsl::string_view&        text);

    /// Load into the specified 'result' the text representation of the
    /// specified 'value' having the specified 'sign' in the specified 'base'.
    static void generate(bsl::string*                         result,
                         const AbstractIntegerRepresentation& value,
                         AbstractIntegerSign::Value           sign,
                         AbstractIntegerBase::Value           base);

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(AbstractIntegerRepresentation);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntsa::AbstractIntegerRepresentation
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const AbstractIntegerRepresentation& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntsa::AbstractIntegerRepresentation
bool operator==(const AbstractIntegerRepresentation& lhs,
                const AbstractIntegerRepresentation& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntsa::AbstractIntegerRepresentation
bool operator!=(const AbstractIntegerRepresentation& lhs,
                const AbstractIntegerRepresentation& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractIntegerRepresentation
bool operator<(const AbstractIntegerRepresentation& lhs,
               const AbstractIntegerRepresentation& rhs);

/// Return true if the value of the specified 'lhs' is less than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractIntegerRepresentation
bool operator<=(const AbstractIntegerRepresentation& lhs,
                const AbstractIntegerRepresentation& rhs);

/// Return true if the value of the specified 'lhs' is greater than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractIntegerRepresentation
bool operator>(const AbstractIntegerRepresentation& lhs,
               const AbstractIntegerRepresentation& rhs);

/// Return true if the value of the specified 'lhs' is greater than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractIntegerRepresentation
bool operator>=(const AbstractIntegerRepresentation& lhs,
                const AbstractIntegerRepresentation& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::AbstractIntegerRepresentation
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const AbstractIntegerRepresentation& value);

template <typename HASH_ALGORITHM>
void AbstractIntegerRepresentation::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_data);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const AbstractIntegerRepresentation& value)
{
    value.hash(algorithm);
}

/// Provide a non-negative integer of arbitrary size.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class AbstractIntegerQuantity
{
    /// The representation of the abstract integer.
    ntsa::AbstractIntegerRepresentation d_rep;

    /// Grant visibility to the internals of this class to its utility.
    friend class AbstractIntegerQuantityUtil;

  public:
    /// Create a new abstract unsigned integer having the default value.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit AbstractIntegerQuantity(bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract unsigned integer having the specified 'value'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit AbstractIntegerQuantity(short             value,
                                     bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract unsigned integer having the specified 'value'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit AbstractIntegerQuantity(unsigned short    value,
                                     bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract unsigned integer having the specified 'value'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit AbstractIntegerQuantity(int               value,
                                     bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract unsigned integer having the specified 'value'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit AbstractIntegerQuantity(unsigned int      value,
                                     bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract unsigned integer having the specified 'value'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit AbstractIntegerQuantity(long              value,
                                     bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract unsigned integer having the specified 'value'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit AbstractIntegerQuantity(unsigned long     value,
                                     bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract unsigned integer having the specified 'value'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit AbstractIntegerQuantity(long long         value,
                                     bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract unsigned integer having the specified 'value'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit AbstractIntegerQuantity(unsigned long long value,
                                     bslma::Allocator*  basicAllocator = 0);

    /// Create a new abstract unsigned integer having the specified 'value'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    AbstractIntegerQuantity(const AbstractIntegerQuantity& original,
                            bslma::Allocator*              basicAllocator = 0);

    /// Destroy this object.
    ~AbstractIntegerQuantity();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    AbstractIntegerQuantity& operator=(const AbstractIntegerQuantity& other);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& operator=(short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& operator=(unsigned short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& operator=(int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& operator=(unsigned int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& operator=(long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& operator=(unsigned long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& operator=(long long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& operator=(unsigned long long value);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Resize the quantity to contain the specified 'size' number of blocks.
    void resize(bsl::size_t size);

    /// Swap the value of the specified 'other' object with the value of this
    /// object.
    void swap(AbstractIntegerQuantity& other);

    /// Parse the specified 'text' containing a sequence of characters in
    /// either base-2, base-8, base-10, or base-16, and load the result into
    /// this object and load the sign into the specified 'sign'. Return
    /// true if 'text' is valid, or false otherwise.
    bool parse(AbstractIntegerSign::Value* sign, const bsl::string_view& text);

    /// Decode the specified 'data' encoded as an ASN.1 integer.
    void decode(const bsl::vector<bsl::uint8_t>& data);

    /// Encode this object as an ASN.1 integer having the specified 'sign' to
    /// the specified 'result'.
    void encode(AbstractIntegerSign::Value sign,
                bsl::vector<bsl::uint8_t>* result) const;

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(unsigned short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(unsigned int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(unsigned long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(long long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(unsigned long long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(const AbstractIntegerQuantity& value);

    /// Increment the value of this object by one. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& increment();

    /// Decrement the value of this object by one. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& decrement();

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& add(short value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& add(unsigned short value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& add(int value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& add(unsigned int value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& add(long value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& add(unsigned long value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& add(long long value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& add(unsigned long long value);

    /// Add the value of the specified 'other' object to this object. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& add(const AbstractIntegerQuantity& other);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractIntegerQuantity& subtract(short value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractIntegerQuantity& subtract(unsigned short value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractIntegerQuantity& subtract(int value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractIntegerQuantity& subtract(unsigned int value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractIntegerQuantity& subtract(long value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractIntegerQuantity& subtract(unsigned long value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractIntegerQuantity& subtract(long long value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractIntegerQuantity& subtract(unsigned long long value);

    /// Subtract the value of the specified 'other' object from this object.
    /// Return a reference to this modifiable object.
    AbstractIntegerQuantity& subtract(const AbstractIntegerQuantity& other);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& multiply(short value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& multiply(unsigned short value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& multiply(int value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& multiply(unsigned int value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& multiply(long value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& multiply(unsigned long value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& multiply(long long value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& multiply(unsigned long long value);

    /// Multiply the value of this object by the value of the specified 'other'
    /// object.
    AbstractIntegerQuantity& multiply(const AbstractIntegerQuantity& other);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& divide(short value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& divide(unsigned short value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& divide(int value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& divide(unsigned int value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& divide(long value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& divide(unsigned long value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& divide(long long value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& divide(unsigned long long value);

    /// Divide the value of this object by the value of the specified 'other'
    /// object. Return a reference to this modifiable object.
    AbstractIntegerQuantity& divide(const AbstractIntegerQuantity& other);

    /// Divide the value of this object by the value of the specified 'other'
    /// object. Load into the specified 'remainder' the remainder of the
    /// division. Return a reference to this modifiable object.
    AbstractIntegerQuantity& divide(const AbstractIntegerQuantity& other,
                                    AbstractIntegerQuantity*       remainder);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractIntegerQuantity& modulus(short value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractIntegerQuantity& modulus(unsigned short value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractIntegerQuantity& modulus(int value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractIntegerQuantity& modulus(unsigned int value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractIntegerQuantity& modulus(long value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractIntegerQuantity& modulus(unsigned long value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractIntegerQuantity& modulus(long long value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractIntegerQuantity& modulus(unsigned long long value);

    /// Store the remainder of the the value of this object divided by the the
    /// value of the specified 'other' object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& modulus(const AbstractIntegerQuantity& other);

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(short value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(unsigned short value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(int value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(unsigned int value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(long value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(unsigned long value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(long long value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(unsigned long long value) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const AbstractIntegerQuantity& other) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(short value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(unsigned short value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(int value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(unsigned int value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(long value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(unsigned long value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(long long value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(unsigned long long value) const;

    /// Compare the value of this object to the specified 'other' object.
    /// Return -1 if this object is less than the 'other' object, 0 if this
    /// object is equal to the 'other' object, and 0 if this object is equal to
    /// the 'other' object.
    int compare(const AbstractIntegerQuantity& other) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(short* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(unsigned short* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(int* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(unsigned int* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(long* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(unsigned long* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(long long* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(unsigned long long* result) const;

    /// Load into the specified 'result' the text representation of this object
    /// having the specified 'sign' in the specified 'base'.
    void generate(bsl::string*               result,
                  AbstractIntegerSign::Value sign,
                  AbstractIntegerBase::Value base) const;

    /// Return the base.
    AbstractIntegerBase::Value base() const;

    /// Return the radix, or number of representable digits, in the base.
    bsl::uint64_t radix() const;

    /// Return the number of blocks.
    bsl::size_t size() const;

    /// Return true if the number is zero, otherwise return false.
    bool isZero() const;

    /// Return true if the number is one, otherwise return false.
    bool isOne() const;

    /// Return true if this object is *not* an alias for the specified 'other'
    /// object, otherwise return false.
    bool isNotAliasOf(const AbstractIntegerQuantity* other) const;

    /// Return true if this object is *not* an alias for the specified 'other'
    /// object, otherwise return false.
    bool isNotAliasOf(const AbstractIntegerQuantity& other) const;

    /// Return the allocator used to supply memory.
    bslma::Allocator* allocator() const;

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
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(AbstractIntegerQuantity);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntsa::AbstractIntegerQuantity
bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const AbstractIntegerQuantity& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntsa::AbstractIntegerQuantity
bool operator==(const AbstractIntegerQuantity& lhs,
                const AbstractIntegerQuantity& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntsa::AbstractIntegerQuantity
bool operator!=(const AbstractIntegerQuantity& lhs,
                const AbstractIntegerQuantity& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractIntegerQuantity
bool operator<(const AbstractIntegerQuantity& lhs,
               const AbstractIntegerQuantity& rhs);

/// Return true if the value of the specified 'lhs' is less than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractIntegerQuantity
bool operator<=(const AbstractIntegerQuantity& lhs,
                const AbstractIntegerQuantity& rhs);

/// Return true if the value of the specified 'lhs' is greater than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractIntegerQuantity
bool operator>(const AbstractIntegerQuantity& lhs,
               const AbstractIntegerQuantity& rhs);

/// Return true if the value of the specified 'lhs' is greater than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractIntegerQuantity
bool operator>=(const AbstractIntegerQuantity& lhs,
                const AbstractIntegerQuantity& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::AbstractIntegerQuantity
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                algorithm,
                const AbstractIntegerQuantity& value);

template <typename HASH_ALGORITHM>
void AbstractIntegerQuantity::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_rep);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                algorithm,
                const AbstractIntegerQuantity& value)
{
    value.hash(algorithm);
}

/// Provide arithmetic for non-negative integers of arbitrary size.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_data
class AbstractIntegerQuantityUtil
{
  public:
    /// Add the specified 'addend2' to the specified 'addend1' and load the
    /// result into the specified 'sum'. Note that either 'addend1' or
    /// 'addend2' may alias 'sum'. The behavior is undefined if 'sum' is null.
    static void add(AbstractIntegerQuantity*       sum,
                    const AbstractIntegerQuantity& addend1,
                    const AbstractIntegerQuantity& addend2);

    /// Subtract the specified 'subtrahend' from the specified 'minuend' and
    /// load the result into the specified 'difference'. If 'subtrahend' is
    /// greater than 'minuend', clamp the 'difference' to zero. Note that
    /// either 'minuend' or 'subtrahend' may alias 'difference'. The behavior
    /// is undefined if 'difference' is null.
    static void subtract(AbstractIntegerQuantity*       difference,
                         const AbstractIntegerQuantity& minuend,
                         const AbstractIntegerQuantity& subtrahend);

    // Multiply the specified 'multiplicand' by the specified 'multiplier'
    /// and load the result into the specified 'product'. Note that either
    /// 'multiplicand' or 'multiplier' may alias 'product'. The behavior is
    /// undefined if 'product' is null.
    static void multiply(AbstractIntegerQuantity*       product,
                         const AbstractIntegerQuantity& multiplicand,
                         const AbstractIntegerQuantity& multiplier);

    /// Divide the specified 'dividend' by the specified 'divisor'
    /// and load the result into the specified 'quotient' and the modulus into
    /// the specified 'remainder'. Note that either 'dividend' or 'divisor'
    /// may alias 'quotient' or 'remainder'. The behavior is undefined if
    /// either 'quotient' or 'remainder' is null.
    static void divide(AbstractIntegerQuantity*       quotient,
                       AbstractIntegerQuantity*       remainder,
                       const AbstractIntegerQuantity& dividend,
                       const AbstractIntegerQuantity& divisor);
};

/// Provide a signed integer of arbitrary size.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class AbstractInteger
{
    /// Grant visibility to the internals of this class to its utility.
    friend class AbstractIntegerUtil;

    AbstractIntegerSign::Value d_sign;
    AbstractIntegerQuantity    d_magnitude;

  private:
    /// Normalize this object so that its sign is positive if its value is
    /// zero.
    void normalize();

  public:
    /// Create a new abstract integer having the default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(short             value,
                             bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(unsigned short    value,
                             bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(int value, bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(unsigned int      value,
                             bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(long value, bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(unsigned long     value,
                             bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(long long         value,
                             bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(unsigned long long value,
                             bslma::Allocator*  basicAllocator = 0);

    /// Create a new abstract integer having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    AbstractInteger(const AbstractInteger& original,
                    bslma::Allocator*      basicAllocator = 0);

    /// Destroy this object.
    ~AbstractInteger();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    AbstractInteger& operator=(const AbstractInteger& other);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& operator=(short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& operator=(unsigned short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& operator=(int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& operator=(unsigned int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& operator=(long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& operator=(unsigned long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& operator=(long long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& operator=(unsigned long long value);

    /// Add the specified 'other' object to this object and return the sum.
    AbstractInteger operator+(const AbstractInteger& other) const;

    /// Subtract the specified 'other' object from this object and return the
    /// difference.
    AbstractInteger operator-(const AbstractInteger& other) const;

    /// Multiply this object by the specified 'other' object and return the
    /// product.
    AbstractInteger operator*(const AbstractInteger& other) const;

    /// Divide this object by the specified 'other' object and return the
    /// quotient.
    AbstractInteger operator/(const AbstractInteger& other) const;

    /// Divide this object by the specified 'other' object and return the
    /// remainder.
    AbstractInteger operator%(const AbstractInteger& other) const;

    /// Negate this object. Return the result.
    AbstractInteger operator-() const;

    /// Add the specified 'other' object to this object and assign the sum
    /// to this object. Return a reference to this modifiable object.
    AbstractInteger& operator+=(const AbstractInteger& other);

    /// Subtract the specified 'other' object from this object and assign the
    /// difference to this object. Return a reference to this modifiable
    /// object.
    AbstractInteger& operator-=(const AbstractInteger& other);

    /// Multiply this object by the specified 'other' object and assign the
    /// product to this object. Return a reference to this modifiable object.
    AbstractInteger& operator*=(const AbstractInteger& other);

    /// Divide this object by the specified 'other' object and assign the
    /// quotient to this object. Return a reference to this modifiable object.
    AbstractInteger& operator/=(const AbstractInteger& other);

    /// Divide this object by the specified 'other' object and assign the
    /// remainder to this object. Return a reference to this modifiable object.
    AbstractInteger& operator%=(const AbstractInteger& other);

    /// Add one to this object and assign the sum to this object. Return
    /// a reference to this modifiable object.
    AbstractInteger& operator++();

    /// Add one to this object and return the sum.
    AbstractInteger operator++(int);

    /// Subtract one from this object and assign the difference to this object.
    /// Return a reference to this modifiable object.
    AbstractInteger& operator--();

    /// Subtract one from this object and return the difference.
    AbstractInteger operator--(int);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Swap the value of the specified 'other' object with the value of this
    /// object.
    void swap(AbstractInteger& other);

    // Parse the specified 'text' containing a sequence of characters in either
    // base-2, base-8, base-10, or base-16, and load the result into this
    // object. Return true if 'text' is valid, or false otherwise.
    bool parse(const bsl::string_view& text);

    // Decode the specified 'data' encoded as an ASN.1 integer.
    void decode(const bsl::vector<bsl::uint8_t>& data);

    // Encode this object as an ASN.1 integer to the specified 'result'.
    void encode(bsl::vector<bsl::uint8_t>* result) const;

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(unsigned short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(unsigned int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(unsigned long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(long long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(unsigned long long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(const AbstractInteger& value);

    /// Negate the value of this object. Return a reference to this modifiable
    /// object.
    AbstractInteger& negate();

    /// Increment the value of this object by one. Return a reference to this
    /// modifiable object.
    AbstractInteger& increment();

    /// Decrement the value of this object by one. Return a reference to this
    /// modifiable object.
    AbstractInteger& decrement();

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& add(short value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& add(unsigned short value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& add(int value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& add(unsigned int value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& add(long value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& add(unsigned long value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& add(long long value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& add(unsigned long long value);

    /// Add the value of the specified 'other' object to this object. Return a
    /// reference to this modifiable object.
    AbstractInteger& add(const AbstractInteger& other);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractInteger& subtract(short value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractInteger& subtract(unsigned short value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractInteger& subtract(int value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractInteger& subtract(unsigned int value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractInteger& subtract(long value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractInteger& subtract(unsigned long value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractInteger& subtract(long long value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractInteger& subtract(unsigned long long value);

    /// Subtract the value of the specified 'other' object from this object.
    /// Return a reference to this modifiable object.
    AbstractInteger& subtract(const AbstractInteger& other);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& multiply(short value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& multiply(unsigned short value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& multiply(int value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& multiply(unsigned int value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& multiply(long value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& multiply(unsigned long value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& multiply(long long value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& multiply(unsigned long long value);

    /// Multiply the value of this object by the value of the specified 'other'
    /// object.
    AbstractInteger& multiply(const AbstractInteger& other);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& divide(short value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& divide(unsigned short value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& divide(int value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& divide(unsigned int value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& divide(long value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& divide(unsigned long value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& divide(long long value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& divide(unsigned long long value);

    /// Divide the value of this object by the value of the specified 'other'
    /// object. Return a reference to this modifiable object.
    AbstractInteger& divide(const AbstractInteger& other);

    /// Divide the value of this object by the value of the specified 'other'
    /// object. Load into the specified 'remainder' the remainder of the
    /// division. Return a reference to this modifiable object.
    AbstractInteger& divide(const AbstractInteger& other,
                            AbstractInteger*       remainder);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractInteger& modulus(short value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractInteger& modulus(unsigned short value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractInteger& modulus(int value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractInteger& modulus(unsigned int value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractInteger& modulus(long value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractInteger& modulus(unsigned long value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractInteger& modulus(long long value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractInteger& modulus(unsigned long long value);

    /// Store the remainder of the the value of this object divided by the the
    /// value of the specified 'other' object. Return a reference to this
    /// modifiable object.
    AbstractInteger& modulus(const AbstractInteger& other);

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(short value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(unsigned short value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(int value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(unsigned int value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(long value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(unsigned long value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(long long value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(unsigned long long value) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const AbstractInteger& other) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(short value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(unsigned short value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(int value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(unsigned int value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(long value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(unsigned long value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(long long value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(unsigned long long value) const;

    /// Compare the value of this object to the specified 'other' object.
    /// Return -1 if this object is less than the 'other' object, 0 if this
    /// object is equal to the 'other' object, and 0 if this object is equal to
    /// the 'other' object.
    int compare(const AbstractInteger& other) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(short* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(unsigned short* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(int* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(unsigned int* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(long* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(unsigned long* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(long long* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(unsigned long long* result) const;

    /// Load into the specified 'result' a string representation of this number
    /// in the specified 'base'.
    void generate(bsl::string* result, AbstractIntegerBase::Value base) const;

    /// Return true if the number is zero, otherwise return false.
    bool isZero() const;

    /// Return true if the number is positive, otherwise return false.
    bool isPositive() const;

    /// Return true if the number is negative, otherwise return false.
    bool isNegative() const;

    /// Return true if this object is *not* an alias for the specified 'other'
    /// object, otherwise return false.
    bool isNotAliasOf(const AbstractInteger* other) const;

    /// Return true if this object is *not* an alias for the specified 'other'
    /// object, otherwise return false.
    bool isNotAliasOf(const AbstractInteger& other) const;

    /// Return the allocator used to supply memory.
    bslma::Allocator* allocator() const;

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
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(AbstractInteger);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntsa::AbstractInteger
bsl::ostream& operator<<(bsl::ostream& stream, const AbstractInteger& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntsa::AbstractInteger
bool operator==(const AbstractInteger& lhs, const AbstractInteger& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntsa::AbstractInteger
bool operator!=(const AbstractInteger& lhs, const AbstractInteger& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractInteger
bool operator<(const AbstractInteger& lhs, const AbstractInteger& rhs);

/// Return true if the value of the specified 'lhs' is less than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractInteger
bool operator<=(const AbstractInteger& lhs, const AbstractInteger& rhs);

/// Return true if the value of the specified 'lhs' is greater than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractInteger
bool operator>(const AbstractInteger& lhs, const AbstractInteger& rhs);

/// Return true if the value of the specified 'lhs' is greater than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntsa::AbstractInteger
bool operator>=(const AbstractInteger& lhs, const AbstractInteger& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::AbstractInteger
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const AbstractInteger& value);

/// Provide arithmetic for signed integers of arbitrary size.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_data
class AbstractIntegerUtil
{
  public:
    /// Add the specified 'addend2' to the specified 'addend1' and load the
    /// result into the specified 'sum'. Note that either 'addend1' or
    /// 'addend2' may alias 'sum'. The behavior is undefined if 'sum' is null.
    static void add(AbstractInteger*       sum,
                    const AbstractInteger& addend1,
                    const AbstractInteger& addend2);

    /// Subtract the specified 'subtrahend' from the specified 'minuend' and
    /// load the result into the specified 'difference'. If 'subtrahend' is
    /// greater than 'minuend', clamp the 'difference' to zero. Note that
    /// either 'minuend' or 'subtrahend' may alias 'difference'. The behavior
    /// is undefined if 'difference' is null.
    static void subtract(AbstractInteger*       difference,
                         const AbstractInteger& minuend,
                         const AbstractInteger& subtrahend);

    // Multiply the specified 'multiplicand' by the specified 'multiplier'
    /// and load the result into the specified 'product'. Note that either
    /// 'multiplicand' or 'multiplier' may alias 'product'. The behavior is
    /// undefined if 'product' is null.
    static void multiply(AbstractInteger*       product,
                         const AbstractInteger& multiplicand,
                         const AbstractInteger& multiplier);

    /// Divide the specified 'dividend' by the specified 'divisor'
    /// and load the result into the specified 'quotient' and the modulus into
    /// the specified 'remainder'. Note that either 'dividend' or 'divisor'
    /// may alias 'quotient' or 'remainder'. The behavior is undefined if
    /// either 'quotient' or 'remainder' is null.
    static void divide(AbstractInteger*       quotient,
                       AbstractInteger*       remainder,
                       const AbstractInteger& dividend,
                       const AbstractInteger& divisor);
};

template <typename HASH_ALGORITHM>
void AbstractInteger::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_magnitude);
    hashAppend(algorithm, d_sign);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const AbstractInteger& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
