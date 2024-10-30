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

#ifndef INCLUDED_NTCDNS_PROTOCOL
#define INCLUDED_NTCDNS_PROTOCOL

#include <ntcdns_vocabulary.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bdlb_bigendian.h>
#include <bdlbb_blob.h>
#include <bsls_platform.h>
#include <bsl_memory.h>
#include <bsl_ostream.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcdns {

/// @internal @brief
/// Provide utilities for validating the correctness of the serialization of
/// the DNS protocol.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcdns
class Validation
{
  public:
    /// Verify the specified 'numBytesRemaining' is sufficient to store the
    /// specified 'numBytesNeeded'. Return the error.
    static ntsa::Error checkOverflow(bsl::size_t numBytesRemaining,
                                     bsl::size_t numBytesNeeded);

    /// Verify the specified 'numBytesRemaining' is sufficient to store the
    /// specified 'numBytesNeeded'. Return the error.
    static ntsa::Error checkUnderflow(bsl::size_t numBytesRemaining,
                                      bsl::size_t numBytesNeeded);

    /// Verify the specified 'token' in the specified 'name'. Return the error.
    static ntsa::Error checkToken(const bsl::string&       name,
                                  const bslstl::StringRef& token);

    /// Verify the specify character-string 'value'. Return the error.
    static ntsa::Error checkCharacterString(const bsl::string& value);

    /// Verify the specify resource record 'value'. Return the error.
    static ntsa::Error checkRdata(const bdlbb::Blob& value);

    /// Verify the specified 'expected' RDATA length matches the specified
    /// 'found' RDATA length. Return the error.
    static ntsa::Error checkExpectedRdataLength(bsl::size_t expected,
                                                bsl::size_t found);

    /// Verify the specified 'expected' RDATA length matches the specified
    /// 'found' RDATA length. Return the error.
    static ntsa::Error checkCoherentRdataLength(bsl::size_t expected,
                                                bsl::size_t found);

    /// The length of a typical domain name.
    static const bsl::size_t k_TYPICAL_NAME_LENGTH;

    /// The maximum length of a single label within a domain name.
    static const bsl::size_t k_MAX_LABEL_LENGTH;

    /// The maximum length of a character-string.
    static const bsl::size_t k_MAX_CHARACTER_STRING_LENGTH;

    /// The maximum data of raw resource record data.
    static const bsl::size_t k_MAX_RDATA_LENGTH;

    /// The maximum recursion depth to follow when recursively decompressing
    /// labels.
    static const bsl::size_t k_MAX_LABEL_RESOLUTION_RECURSION_DEPTH;
};

/// @internal @brief
/// Provide an encoder of DNS vocabulary to a contiguous range of a memory.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcdns
class MemoryEncoder
{
    bsl::uint8_t* d_begin;
    bsl::uint8_t* d_current;
    bsl::uint8_t* d_end;

  private:
    MemoryEncoder(const MemoryEncoder&) BSLS_KEYWORD_DELETED;
    MemoryEncoder& operator=(const MemoryEncoder&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new memory encoder to the specified 'data' having the
    /// specified 'size'.
    MemoryEncoder(bsl::uint8_t* data, bsl::size_t size);

    /// Destroy this object.
    ~MemoryEncoder();

    /// Encode the specified unsigned 8-bit integer 'value'. Return the error.
    ntsa::Error encodeUint8(bsl::uint8_t value);

    /// Encode the specified unsigned 16-bit integer 'value' in network byte
    /// order. Return the error.
    ntsa::Error encodeUint16(bsl::uint16_t value);

    /// Encode the specified unsigned 16-bit integer 'value' in network byte
    /// order. Return the error.
    ntsa::Error encodeUint16(const bdlb::BigEndianUint16& value);

    /// Encode the specified unsigned 32-bit integer 'value' in network byte
    /// order. Return the error.
    ntsa::Error encodeUint32(bsl::uint32_t value);

    /// Encode the specified unsigned 32-bit integer 'value' in network byte
    /// order. Return the error.
    ntsa::Error encodeUint32(const bdlb::BigEndianUint32& value);

    /// Encode the specified domain name 'value'. Return the error.
    ntsa::Error encodeDomainName(const bsl::string& value);

    /// Encode the specified character string 'value'. Return the error.
    ntsa::Error encodeCharacterString(const bsl::string& value);

    /// Encode the specified raw resource record data 'value'. Return the
    /// error.
    ntsa::Error encodeRdata(const bdlbb::Blob& value);

    /// Encode the specified raw 'value' having the specified 'size',
    /// exactly as it is represented. Return the error.
    ntsa::Error encodeRaw(const void* value, bsl::size_t size);

    /// Seek the position to the specified 'position'. Return the error.
    ntsa::Error seek(bsl::size_t position);

    /// Increment the position by the specified 'amount'. Return the error.
    ntsa::Error advance(bsl::size_t amount);

    /// Decrement the position by the specified 'amount'. Return the error.
    ntsa::Error rewind(bsl::size_t amount);

    /// Return the pointer to the beginning of the data.
    bsl::uint8_t* begin() const;

    /// Return the pointer to the current position.
    bsl::uint8_t* current() const;

    /// Return the pointer to the end of the data.
    bsl::uint8_t* end() const;

    /// Return the current position.
    bsl::size_t position() const;

    /// Return the capacity.
    bsl::size_t capacity() const;
};

/// @internal @brief
/// Provide a decoder of DNS vocabulary from a contiguous range of a memory.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcdns
class MemoryDecoder
{
    const bsl::uint8_t* d_begin;
    const bsl::uint8_t* d_current;
    const bsl::uint8_t* d_end;

  private:
    MemoryDecoder(const MemoryDecoder&) BSLS_KEYWORD_DELETED;
    MemoryDecoder& operator=(const MemoryDecoder&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new memory decoder from the specified 'data' having the
    /// specified 'size'.
    MemoryDecoder(const bsl::uint8_t* data, bsl::size_t size);

    /// Destroy this object.
    ~MemoryDecoder();

    /// Decode the specified unsigned 8-bit integer 'value'. Return the error.
    ntsa::Error decodeUint8(bsl::uint8_t* value);

    /// Decode the specified unsigned 16-bit integer 'value' in network byte
    /// order. Return the error.
    ntsa::Error decodeUint16(bsl::uint16_t* value);

    /// Decode the specified unsigned 16-bit integer 'value' in network byte
    /// order. Return the error.
    ntsa::Error decodeUint16(bdlb::BigEndianUint16* value);

    /// Decode the specified unsigned 32-bit integer 'value' in network byte
    /// order. Return the error.
    ntsa::Error decodeUint32(bsl::uint32_t* value);

    /// Decode the specified unsigned 32-bit integer 'value' in network byte
    /// order. Return the error.
    ntsa::Error decodeUint32(bdlb::BigEndianUint32* value);

    /// Decode the specified domain name 'value'. Return the error.
    ntsa::Error decodeDomainName(bsl::string* value);

    /// Load into the specified 'value' the domain name or list of labels
    /// found at the specified 'offset' from the start of a DNS header
    /// (i.e., the first octet of the "ID" field of the DNS header.) The
    /// recursion begins at the specified 'depth'. Return the error.
    ntsa::Error decodeLabel(bsl::string* value,
                            bsl::size_t  depth,
                            bsl::size_t  offset);

    /// Decode the specified character string 'value'. Return the error.
    ntsa::Error decodeCharacterString(bsl::string* value);

    /// Decode the specified raw resource record data 'value'. Return the
    /// error.
    ntsa::Error decodeRdata(bdlbb::Blob* value);

    /// Decode the specified raw 'value' having the specified 'size',
    /// exactly as it is represented. Return the error.
    ntsa::Error decodeRaw(void* value, bsl::size_t size);

    /// Seek the position to the specified 'position'. Return the error.
    ntsa::Error seek(bsl::size_t position);

    /// Increment the position by the specified 'amount'. Return the error.
    ntsa::Error advance(bsl::size_t amount);

    /// Decrement the position by the specified 'amount'. Return the error.
    ntsa::Error rewind(bsl::size_t amount);

    /// Return the pointer to the beginning of the data.
    const bsl::uint8_t* begin() const;

    /// Return the pointer to the current position.
    const bsl::uint8_t* current() const;

    /// Return the pointer to the end of the data.
    const bsl::uint8_t* end() const;

    /// Return the current position.
    bsl::size_t position() const;

    /// Return the capacity.
    bsl::size_t capacity() const;
};

/// @internal @brief
/// Describe a header in the DNS protocol.
///
/// @details
/// The DNS message header is described in RFC 1035 section 4.1.1.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b ID:
/// The identifier generated by the client. This identifier is copied to the
/// response to a request and can be used to correlate the response to a
/// request.
///
/// @li @b QR:
/// The message type, indicating that the message is either a query or a
/// response.
///
/// @li @b OPCODE:
/// The operation code that indicates the type of query.
///
/// @li @b AA:
/// The "Authoritative Answer" flag. When set in a response it indicates that
/// the responding name server is an authority for a domain name.
///
/// @li @b TC:
/// The "Truncation" flag. When set in a response it indicates that this the
/// message was truncated due because its required length is greater than the
/// maximum length permitted by the transport.
///
/// @li @b RD:
/// The "Recursion Desired" flag. If set is a query it indicates the name
/// server should process the query recursively. If set in a query it is also
/// set in the response to that query.
///
/// @li @b RA:
/// The "Recursion Available" flag. If set in a response it indicates that
/// recursive processing of queries is supported by the name server.
///
/// @li @b Z:
/// Reserved for future use. Must be zero in all queries and responses.
///
/// @li @b AD:
/// The "Authentic Data" flag. If set in a response it indicates the contents
/// of the response have been verified.
///
/// @li @b CD:
/// The "Checking Disabled" flag. If set in a query it indicates in that
/// non-authentic data is acceptable to the resolver.
///
/// @li @b RCODE:
/// The "Response Code" that indicates how the query was processed.
///
/// @li @b QDCOUNT:
/// The number of entries in the question section.
///
/// @li @b ANCOUNT:
/// The number of resource records in the answer section.
///
/// @li @b NSCOUNT:
/// The number of name server resource records in the authority records
/// section.
///
/// @li @b ARCOUNT:
/// The number of resource records in the additional records section.
///
/// @par Wire Format
/// The following diagram describes the binary layout of this class.
///
///                                     1  1  1  1  1  1
///       0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///     |                      ID                       |
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///     |QR|   Opcode  |AA|TC|RD|RA| Z|AD|CD|   RCODE   |
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///     |                    QDCOUNT                    |
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///     |                    ANCOUNT                    |
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///     |                    NSCOUNT                    |
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///     |                    ARCOUNT                    |
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcdns
class Header
{
#if defined(BSLS_PLATFORM_CMP_IBM)
#pragma pack(1)
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)
#pragma pack(1)
#endif

#if defined(BSLS_PLATFORM_CMP_SUN)
#pragma pack(1)
#endif

    struct Flags {
#if BSLS_PLATFORM_IS_BIG_ENDIAN
        unsigned short qr : 1;
        unsigned short opcode : 4;
        unsigned short aa : 1;
        unsigned short tc : 1;
        unsigned short rd : 1;
        unsigned short ra : 1;
        unsigned short z : 1;
        unsigned short ad : 1;
        unsigned short cd : 1;
        unsigned short rcode : 4;
#else
        unsigned short rd : 1;
        unsigned short tc : 1;
        unsigned short aa : 1;
        unsigned short opcode : 4;
        unsigned short qr : 1;
        unsigned short rcode : 4;
        unsigned short cd : 1;
        unsigned short ad : 1;
        unsigned short z : 1;
        unsigned short ra : 1;
#endif
    }
#if defined(BSLS_PLATFORM_CMP_CLANG) || defined(BSLS_PLATFORM_CMP_GNU)
    __attribute__((packed))
#endif
    ;

#if defined(BSLS_PLATFORM_CMP_IBM)
#pragma pack()
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)
#pragma pack()
#endif

#if defined(BSLS_PLATFORM_CMP_SUN)
#pragma pack()
#endif

    bsl::uint16_t            d_id;
    ntcdns::Direction::Value d_direction;
    ntcdns::Operation::Value d_operation;
    ntcdns::Error::Value     d_error;
    bool                     d_aa;
    bool                     d_tc;
    bool                     d_rd;
    bool                     d_ra;
    bool                     d_ad;
    bool                     d_cd;
    bsl::uint16_t            d_qdcount;
    bsl::uint16_t            d_ancount;
    bsl::uint16_t            d_nscount;
    bsl::uint16_t            d_arcount;

  private:
    /// Validate the correctness and coherency of the message header. Return
    /// the error.
    ntsa::Error validate();

  public:
    /// Create a new header having a default value.
    Header();

    /// Create a new header having the same value as the specified 'other'
    /// object.
    Header(const Header& other);

    /// Destroy this object.
    ~Header();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Header& operator=(const Header& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the "ID" field to the specified 'value'. The "ID" field is an
    /// identifier assigned by the program that generates any kind of query.
    /// This identifier is copied the corresponding reply and can be used by
    /// the requester to match up replies to outstanding queries.
    void setId(bsl::uint16_t value);

    /// Set the "QR" field to the specified 'value'. THe "QR" field
    /// indicates whether the message is a query or a response.
    void setDirection(ntcdns::Direction::Value value);

    /// Set the "OPCODE" field to the specified 'value'. The "OPCODE" field
    /// indicates the kind of query in the message. This value is set by the
    /// originator of a query and copied into the response.
    void setOperation(ntcdns::Operation::Value value);

    /// Set the "AA" field to the specified 'value'. The "AA" field
    /// indicates that the responding name server is an authority for the
    /// domain name in question section. Note that the contents of the
    /// answer section may have multiple owner names because of aliases. The
    /// "AA" field corresponds to the name which matches the query name, or
    /// the first owner name in the answer section.
    void setAa(bool value);

    /// Set the "TC" field to the specified 'value'. The "TC" field
    /// indicates that this message was truncated due to length greater than
    /// that permitted by the transport.
    void setTc(bool value);

    /// Set the "RD" field to the specified 'value'. The "RD" field may be
    /// set in a query and is copied into the response.  If "RD" is set, it
    /// directs the name server to pursue the query recursively. Recursive
    /// query support is optional.
    void setRd(bool value);

    /// Set the "RA" field to the specified 'value'. The "RA" field is set
    /// or cleared in a response, and denotes whether recursive query
    /// support is available in the name server.
    void setRa(bool value);

    /// Set the "AD" field to the specified 'value'. The "AD" field is set
    /// or cleared in a response, and that the data included has been
    /// verified by the server providing it.
    void setAd(bool value);

    /// Set the "CD" field to the specified 'value'. The "CD" field may be
    /// set in a query and indicates that non-verified data is acceptable to
    /// the resolver sending the query.
    void setCd(bool value);

    /// Set the "RCODE" field to the specified 'value'. The "RCODE" field is
    /// set as part of responses and indicates the status of the request.
    void setError(ntcdns::Error::Value value);

    /// Set the "QDCOUNT" field to the specified 'value'. The "QDCOUNT"
    /// field indicates the number of entries in the question section.
    void setQdcount(bsl::size_t value);

    /// Set the "ANCOUNT" field to the specified 'value'. The "ANCOUNT"
    /// field indicates the number of entries in the answer section.
    void setAncount(bsl::size_t value);

    /// Set the "NSCOUNT" field to the specified 'value'. The "NSCOUNT"
    /// field indicates the number of name server resource records in the
    /// authority records section.
    void setNscount(bsl::size_t value);

    /// Set the "ARCOUNT" field to the specified 'value'. The "ARCOUNT"
    /// field indicates the number of resource records in the additional
    /// records section.
    void setArcount(bsl::size_t value);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(MemoryDecoder* decoder);

    /// Return the "ID" field. The "ID" field is an identifier assigned by the
    /// program that generates any kind of query. This identifier is copied the
    /// corresponding reply and can be used by the requester to match up replies
    /// to outstanding queries.
    bsl::uint16_t id() const;

    /// Return the "QR" field. THe "QR" field indicates whether the message
    /// is a query or a response.
    ntcdns::Direction::Value direction() const;

    /// Return the "OPCODE" field. The "OPCODE" field indicates the kind of
    /// query in the message. This value is set by the originator of a query
    /// and copied into the response.
    ntcdns::Operation::Value operation() const;

    /// Return the "AA" field. The "AA" field indicates that the responding
    /// name server is an authority for the domain name in question section.
    /// Note that the contents of the answer section may have multiple owner
    /// names because of aliases. The "AA" field corresponds to the name
    /// which matches the query name, or the first owner name in the answer
    /// section.
    bool aa() const;

    /// Return the "TC" field. The "TC" field indicates that this message
    /// was truncated due to length greater than that permitted by the
    /// transport.
    bool tc() const;

    /// Return the "RD" field. The "RD" field may be set in a query and is
    /// copied into the response.  If "RD" is set, it directs the name
    /// server to pursue the query recursively. Recursive query support is
    /// optional.
    bool rd() const;

    /// Return the "RA" field. The "RA" field is set or cleared in
    /// a response, and denotes whether recursive query support is available
    /// in the name server.
    bool ra() const;

    /// Return the "AD" field. The "AD" field is set or cleared in
    /// a response, and that the data included has been verified by the
    /// server providing it.
    bool ad() const;

    /// Return the "CD" field. The "CD" field may be set in a query and
    /// indicates that non-verified data is acceptable to the resolver
    /// sending the query.
    bool cd() const;

    /// Set the "RCODE" field to the specified 'value'. The "RCODE" field is
    /// set as part of responses and indicates the status of the request.
    ntcdns::Error::Value error() const;

    /// Return the "QDCOUNT" field. The "QDCOUNT" field indicates the number
    /// of entries in the question section.
    bsl::size_t qdcount() const;

    /// Return the "ANCOUNT" field. The "ANCOUNT" field indicates the number
    /// of entries in the answer section.
    bsl::size_t ancount() const;

    /// Return the "NSCOUNT" field. The "NSCOUNT" field indicates the number
    /// of name server resource records in the authority records section.
    bsl::size_t nscount() const;

    /// Return the "ARCOUNT" field. The "ARCOUNT" field indicates the number
    /// of resource records in the additional records section.
    bsl::size_t arcount() const;

    /// Encode the object to the specified 'encoder. Return the error.
    ntsa::Error encode(MemoryEncoder* encoder) const;

    // FRIENDS

    /// Write a formatted, human-readable description of the specified
    /// 'object' to the specified 'stream'.
    friend bsl::ostream& operator<<(bsl::ostream&         stream,
                                    const ntcdns::Header& object);
};

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
bool operator==(const Header& lhs, const Header& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
bool operator!=(const Header& lhs, const Header& rhs);

/// @internal @brief
/// Describe a question section in the DNS protocol.
///
/// @details
/// See RFC 1035 section 4.1.2.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b QNAME:
/// The domain name represented as a sequence of labels. Each label is encoded
/// as single octet defining the length followed by that number of octets
/// defining the label characters. The domain name is terminated with a
/// zero-length octet.
///
/// @li @b QTYPE:
/// The type of query.
///
/// @li @b QCLASS:
/// The class of the query.
///
/// @par Wire Format
/// The following diagram describes the binary layout of this class.
///
///                                     1  1  1  1  1  1
///       0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///     |                                               |
///     /                     QNAME                     /
///     /                                               /
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///     |                     QTYPE                     |
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///     |                     QCLASS                    |
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcdns
class Question
{
    bsl::string                   d_name;
    ntcdns::Type::Value           d_type;
    ntcdns::Classification::Value d_classification;

  private:
    /// Validate the correctness and coherency of the message header. Return
    /// the error.
    ntsa::Error validate();

  public:
    /// Create a new object having a default value. Optionally specify an
    /// 'allocator' used to supply memory. If 'allocator' is null, the
    /// currently installed default allocator is used.
    explicit Question(bslma::Allocator* allocator = 0);

    /// Create a new object having the same value as the specified 'other'
    /// object. Optionally specify an 'allocator' used to supply memory. If
    /// 'allocator' is null, the currently installed default allocator is
    /// used.
    Question(const Question& other, bslma::Allocator* allocator = 0);

    /// Destroy this object.
    ~Question();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Question& operator=(const Question& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the "QNAME" field to the specified 'value'. The "QNAME" field is
    /// a domain name represented as a sequence of labels.
    void setName(const bsl::string& value);

    /// Set the "QTYPE" field to the specified 'value'. The "QTYPE" field
    /// indicates the type of the query. The values for this field include
    /// all codes valid for a "TYPE" field, together with some more general
    /// codes which can match more than one type of RR.
    void setType(ntcdns::Type::Value value);

    /// Set the "QCLASS" field to the specified 'value'. The "QCLASS" field
    /// indicates the class of the query. For example, the QCLASS field is
    /// IN for the Internet.
    void setClassification(ntcdns::Classification::Value value);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(MemoryDecoder* decoder);

    /// Return the "QNAME" field. The "QNAME" field is a domain name
    /// represented as a sequence of labels.
    const bsl::string& name() const;

    /// Return the "QTYPE" field. The "QTYPE" field indicates the type of
    /// the query. The values for this field include all codes valid for
    /// a "TYPE" field, together with some more general codes which can
    /// match more than one type of RR.
    ntcdns::Type::Value type() const;

    /// Return the "QCLASS" field. The "QCLASS" field indicates the class of
    /// the query. For example, the QCLASS field is IN for the Internet.
    ntcdns::Classification::Value classification() const;

    /// Encode the object to the specified 'encoder'. Return the error.
    ntsa::Error encode(MemoryEncoder* encoder) const;

    // FRIENDS

    /// Write a formatted, human-readable description of the specified
    /// 'object' to the specified 'stream'.
    friend bsl::ostream& operator<<(bsl::ostream&           stream,
                                    const ntcdns::Question& object);
};

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntcdns::Question
bool operator==(const Question& lhs, const Question& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntcdns::Question
bool operator!=(const Question& lhs, const Question& rhs);

/// @internal @brief
/// Describe an answer, authority, and additional sections in the DNS protocol.
///
/// @details
/// See RFC 1035 section 4.1.3.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b NAME:
/// The domain name described by the resource record.
///
/// @li @b TYPE:
/// The type of the resource record data.
/// The type describe the content and format of the data field.
///
/// @li @b CLASS:
/// The class of the resource record data.
///
/// @li @b TTL:
/// The "time-to-live" field: the maximum amount of time, in seconds, that the
/// resource record may be cached.
///
/// @li @b RDLENGTH:
/// The number of octets in the data field.
///
/// @li @b RDATA:
/// The variable length sequence of octets that describes the resource. The
/// format of the data is determined by the TYPE and CLASS of the resource
/// record.
///
/// @par Wire Format
/// The following diagram describes the binary layout of this class.
///
///                                     1  1  1  1  1  1
///       0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///     |                                               |
///     /                                               /
///     /                      NAME                     /
///     |                                               |
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///     |                      TYPE                     |
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///     |                     CLASS                     |
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///     |                      TTL                      |
///     |                                               |
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///     |                   RDLENGTH                    |
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--|
///     /                     RDATA                     /
///     /                                               /
///     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcdns
class ResourceRecord
{
    bsl::string         d_name;
    ntcdns::Type::Value d_type;

    ntcdns::Classification::Value d_class;
    bsl::uint32_t                 d_ttl;

    bsl::uint16_t d_optSize;   // OPT UDP payload size
    bsl::uint32_t d_optFlags;  // OPT extended rcode and flags

    ntcdns::ResourceRecordData d_rdata;

  private:
    /// Validate the correctness and coherency of the message header. Return
    /// the error.
    ntsa::Error validate();

  public:
    /// Create a new object having a default value. Optionally specify an
    /// 'allocator' used to supply memory. If 'allocator' is null, the
    /// currently installed default allocator is used.
    explicit ResourceRecord(bslma::Allocator* allocator = 0);

    /// Create a new object having the same value as the specified 'other'
    /// object. Optionally specify an 'allocator' used to supply memory. If
    /// 'allocator' is null, the currently installed default allocator is
    /// used.
    ResourceRecord(const ResourceRecord& other,
                   bslma::Allocator*     allocator = 0);

    /// Destroy this object.
    ~ResourceRecord();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ResourceRecord& operator=(const ResourceRecord& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the "NAME" field to the specified 'value'. The "NAME" field is
    /// a domain name to which this resource record pertains.
    void setName(const bsl::string& value);

    /// Set the "TYPE" field to the specified 'value'. The "TYPE" field
    /// indicates the meaning of the data in the "RDATA" field.
    void setType(ntcdns::Type::Value value);

    /// Set the "CLASS" field to the specified 'value'. The "CLASS" field
    /// indicates the class of the data in the "RDATA" field.
    void setClassification(ntcdns::Classification::Value value);

    /// Set the "TTL" field to the specified 'value'. The "TTL" field
    /// indicates the time interval (in seconds) that the resource record
    /// may be cached before it should be discarded.  Zero values are
    /// interpreted to mean that the RR can only be used for the transaction
    /// in progress, and should not be cached.
    void setTtl(bsl::uint32_t value);

    /// Set the "UDP Payload Size" field relevant for EDNS OPT
    /// pseudo-record types to the specified 'value'.
    void setPayloadSize(bsl::uint16_t value);

    /// Set the "Extended RCODE and Flags" field relevant for EDNS OPT
    /// pseudo-record types to the specified 'value'.
    void setFlags(bsl::uint32_t value);

    /// Set the "RDATA" field to the specified 'value' and, if the 'value'
    /// is defined and not "raw", the associate "TYPE" field. The "RDATA"
    /// field describes the resource. The format of this information varies
    /// according to the TYPE and CLASS of the resource record.  For
    /// example, the if the TYPE is A and the CLASS is IN, the RDATA field
    /// is a 4 octet ARPA Internet address.
    void setRdata(const ntcdns::ResourceRecordData& value);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(MemoryDecoder* decoder);

    /// Return the "NAME" field. The "NAME" field is a domain name to which
    /// this resource record pertains.
    const bsl::string& name() const;

    /// Return the "TYPE" field. The "TYPE" field indicates the meaning of
    /// the data in the "RDATA" field.
    ntcdns::Type::Value type() const;

    /// Return the "CLASS" field. The "CLASS" field indicates the class of
    /// the data in the "RDATA" field.
    ntcdns::Classification::Value classification() const;

    /// Return the "TTL" field. The "TTL" field indicates the time interval
    /// (in seconds) that the resource record may be cached before it should
    /// be discarded.  Zero values are interpreted to mean that the RR can
    /// only be used for the transaction in progress, and should not be
    /// cached..
    bsl::uint32_t ttl() const;

    /// Return the "UDP Payload Size" field relevant for EDNS OPT
    /// pseudo-record types.
    bsl::uint16_t payloadSize() const;

    /// Return the "Extended RCODE and Flags" field relevant for EDNS OPT
    /// pseudo-record types.
    bsl::uint32_t flags() const;

    /// Return the "RDATA" field. The "RDATA" field describes the resource.
    /// The format of this information varies according to the TYPE and
    /// CLASS of the resource record.  For example, the if the TYPE is A and
    /// the CLASS is IN, the RDATA field is a 4 octet ARPA Internet address.
    const ntcdns::ResourceRecordData& rdata() const;

    /// Encode the object to the specified 'encoder'. Return the error.
    ntsa::Error encode(MemoryEncoder* encoder) const;

    // FRIENDS

    /// Write a formatted, human-readable description of the specified
    /// 'object' to the specified 'stream'.
    friend bsl::ostream& operator<<(bsl::ostream&                 stream,
                                    const ntcdns::ResourceRecord& object);
};

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntcdns::ResourceRecord
bool operator==(const ResourceRecord& lhs, const ResourceRecord& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntcdns::ResourceRecord
bool operator!=(const ResourceRecord& lhs, const ResourceRecord& rhs);

/// @internal @brief
/// Describe a DNS header, questions, and resource records.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcdns
class Message
{
    ntcdns::Header                      d_header;
    bsl::vector<ntcdns::Question>       d_qd;
    bsl::vector<ntcdns::ResourceRecord> d_an;
    bsl::vector<ntcdns::ResourceRecord> d_ns;
    bsl::vector<ntcdns::ResourceRecord> d_ar;

  private:
    /// Validate the correctness and coherency of the message header.
    void validate();

  public:
    /// Create a new object having a default value. Optionally specify an
    /// 'allocator' used to supply memory. If 'allocator' is null, the
    /// currently installed default allocator is used.
    explicit Message(bslma::Allocator* allocator = 0);

    /// Create a new object having the same value as the specified 'other'
    /// object. Optionally specify an 'allocator' used to supply memory. If
    /// 'allocator' is null, the currently installed default allocator is
    /// used.
    Message(const Message& other, bslma::Allocator* allocator = 0);

    /// Destroy this object.
    ~Message();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Message& operator=(const Message& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the "ID" field to the specified 'value'. The "ID" field is an
    /// identifier assigned by the program that generates any kind of query.
    /// This identifier is copied the corresponding reply and can be used by
    /// the requester to match up replies to outstanding queries.
    void setId(bsl::uint16_t value);

    /// Set the "QR" field to the specified 'value'. THe "QR" field
    /// indicates whether the message is a query or a response.
    void setDirection(ntcdns::Direction::Value value);

    /// Set the "OPCODE" field to the specified 'value'. The "OPCODE" field
    /// indicates the kind of query in the message. This value is set by the
    /// originator of a query and copied into the response.
    void setOperation(ntcdns::Operation::Value value);

    /// Set the "AA" field to the specified 'value'. The "AA" field
    /// indicates that the responding name server is an authority for the
    /// domain name in question section. Note that the contents of the
    /// answer section may have multiple owner names because of aliases. The
    /// "AA" field corresponds to the name which matches the query name, or
    /// the first owner name in the answer section.
    void setAa(bool value);

    /// Set the "TC" field to the specified 'value'. The "TC" field
    /// indicates that this message was truncated due to length greater than
    /// that permitted by the transport.
    void setTc(bool value);

    /// Set the "RD" field to the specified 'value'. The "RD" field may be
    /// set in a query and is copied into the response.  If "RD" is set, it
    /// directs the name server to pursue the query recursively. Recursive
    /// query support is optional.
    void setRd(bool value);

    /// Set the "RA" field to the specified 'value'. The "RA" field is set
    /// or cleared in a response, and denotes whether recursive query
    /// support is available in the name server.
    void setRa(bool value);

    /// Set the "AD" field to the specified 'value'. The "AD" field is set
    /// or cleared in a response, and that the data included has been
    /// verified by the server providing it.
    void setAd(bool value);

    /// Set the "CD" field to the specified 'value'. The "CD" field may be
    /// set in a query and indicates that non-verified data is acceptable to
    /// the resolver sending the query.
    void setCd(bool value);

    /// Set the "RCODE" field to the specified 'value'. The "RCODE" field is
    /// set as part of responses and indicates the status of the request.
    void setError(ntcdns::Error::Value value);

    /// Decode the object from the specified 'decoder'. Return the error.
    ntsa::Error decode(MemoryDecoder* decoder);

    /// Add a new question to the questions section. Return a reference to
    /// the modifiable resource record just added.
    ntcdns::Question& addQd();

    /// Add a new question to the questions section having the same value as
    /// the specified 'qd' record. Return a reference to the modifiable
    /// resource record just added.
    ntcdns::Question& addQd(const ntcdns::Question& qd);

    /// Add a new resource record to the answers section. Return a reference
    /// to the modifiable resource record just added.
    ntcdns::ResourceRecord& addAn();

    /// Add a new resource record to the answers section having the same
    /// value as the specified 'an' record. Return a reference to the
    /// modifiable resource record just added.
    ntcdns::ResourceRecord& addAn(const ntcdns::ResourceRecord& an);

    /// Add a new resource record to the name server section. Return
    /// a reference to the modifiable resource record just added.
    ntcdns::ResourceRecord& addNs();

    /// Add a new resource record to the name server section having the same
    /// value as the specified 'ns' record. Return a reference to the
    /// modifiable resource record just added.
    ntcdns::ResourceRecord& addNs(const ntcdns::ResourceRecord& ns);

    /// Add a new resource record to the additional records section. Return
    /// a reference to the modifiable resource record just added.
    ntcdns::ResourceRecord& addAr();

    /// Add a new resource record to the additional records section having
    /// the same value as the specified 'ar' record. Return a reference to
    /// the modifiable resource record just added.
    ntcdns::ResourceRecord& addAr(const ntcdns::ResourceRecord& ar);

    /// Return the "ID" field. The "ID" field is an identifier assigned by
    /// the program that generates any kind of query. This identifier is
    /// copied the corresponding reply and can be used by the requester to
    /// match up replies to outstanding queries.
    bsl::uint16_t id() const;

    /// Return the "QR" field. THe "QR" field indicates whether the message
    /// is a query or a response.
    ntcdns::Direction::Value direction() const;

    /// Return the "OPCODE" field. The "OPCODE" field indicates the kind of
    /// query in the message. This value is set by the originator of a query
    /// and copied into the response.
    ntcdns::Operation::Value operation() const;

    /// Return the "AA" field. The "AA" field indicates that the responding
    /// name server is an authority for the domain name in question section.
    /// Note that the contents of the answer section may have multiple owner
    /// names because of aliases. The "AA" field corresponds to the name
    /// which matches the query name, or the first owner name in the answer
    /// section.
    bool aa() const;

    /// Return the "TC" field. The "TC" field indicates that this message
    /// was truncated due to length greater than that permitted by the
    /// transport.
    bool tc() const;

    /// Return the "RD" field. The "RD" field may be set in a query and is
    /// copied into the response.  If "RD" is set, it directs the name
    /// server to pursue the query recursively. Recursive query support is
    /// optional.
    bool rd() const;

    /// Return the "RA" field. The "RA" field is set or cleared in
    /// a response, and denotes whether recursive query support is available
    /// in the name server.
    bool ra() const;

    /// Return the "AD" field. The "AD" field is set or cleared in
    /// a response, and that the data included has been verified by the
    /// server providing it.
    bool ad() const;

    /// Return the "CD" field. The "CD" field may be set in a query and
    /// indicates that non-verified data is acceptable to the resolver
    /// sending the query.
    bool cd() const;

    /// Set the "RCODE" field to the specified 'value'. The "RCODE" field is
    /// set as part of responses and indicates the status of the request.
    ntcdns::Error::Value error() const;

    /// Return the "QDCOUNT" field. The "QDCOUNT" field indicates the number
    /// of entries in the question section.
    bsl::size_t qdcount() const;

    /// Return the "ANCOUNT" field. The "ANCOUNT" field indicates the number
    /// of entries in the answer section.
    bsl::size_t ancount() const;

    /// Return the "NSCOUNT" field. The "NSCOUNT" field indicates the number
    /// of name server resource records in the authority records section.
    bsl::size_t nscount() const;

    /// Return the "ARCOUNT" field. The "ARCOUNT" field indicates the number
    /// of resource records in the additional records section.
    bsl::size_t arcount() const;

    /// Return the question in the question section at the specified
    /// 'index'. The behavior is undefined unless 'index < qdcount()'.
    const ntcdns::Question& qd(bsl::size_t index) const;

    /// Return the resource record in the answers section at the specified
    /// 'index'. The behavior is undefined unless 'index < ancount()'.
    const ntcdns::ResourceRecord& an(bsl::size_t index) const;

    /// Return the resource record in the name server section at the
    /// specified 'index'. The behavior is undefined unless 'index
    /// < nscount()'.
    const ntcdns::ResourceRecord& ns(bsl::size_t index) const;

    /// Return the resource record in the additional records section at the
    /// specified 'index'. The behavior is undefined unless 'index
    /// < arcount()'.
    const ntcdns::ResourceRecord& ar(bsl::size_t index) const;

    /// Encode the object to the specified 'encoder'. Return the error.
    ntsa::Error encode(MemoryEncoder* encoder) const;

    // FRIENDS

    /// Write a formatted, human-readable description of the specified
    /// 'object' to the specified 'stream'.
    friend bsl::ostream& operator<<(bsl::ostream&          stream,
                                    const ntcdns::Message& object);

    /// Return true if the specified 'lhs' has the same value as the
    /// specified 'rhs', otherwise return false.
    friend bool operator==(const Message& lhs, const Message& rhs);

    /// Return true if the specified 'lhs' does not have the same value as
    /// the specified 'rhs', otherwise return false.
    friend bool operator!=(const Message& lhs, const Message& rhs);
};

}  // close package namespace
}  // close enterprise namespace
#endif
