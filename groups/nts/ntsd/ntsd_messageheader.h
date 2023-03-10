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

#ifndef INCLUDED_NTSD_MESSAGEHEADER
#define INCLUDED_NTSD_MESSAGEHEADER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_bigendian.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobutil.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsd {

/// @internal @brief
/// Provide a test message header.
///
/// @details
/// Provide a value-semantic type that describes the fixed-length
/// header of a test message protocol. Objects of this class are 64 bytes long
/// packed in network byte order. The fields are accessed and manipulated in
/// host byte order. This class is not thread safe.
///
/// @par Attributes
/// This class is composed of the following attributes.
//
/// @li @b magic:
/// The magic number identifying this protocol.
///
/// @li @b version:
/// The version of this protocol.
///
/// @li @b crc:
/// The cyclic redudancy check on the entire message contents, including
/// the header.
///
/// @li @b type:
/// The type of message.
///
/// @li @b machineId:
/// The machine identifier.
///
/// @li @b userId:
/// The user identifier.
///
/// @li @b transactionId:
/// The transaction identifier.
///
/// @li @b sequenceNumber:
/// The sequence number.
///
/// @li @b requestSize:
/// The size of the request, in bytes.
///
/// @li @b responseSize:
/// The size of the response, in bytes.
///
/// @li @b requestDelay:
/// The amount of time to sleep before sending a request, in milliseconds.
///
/// @li @b responseDelay:
/// The amount of time to sleep before sending a response, in milliseconds.
///
/// @li @b requestTimestamp:
/// The time the request was generated, in microseconds since the Unix epoch.
///
/// @li @b responseTimestamp:
/// The time the response was generated, in microseconds since the Unix epoch.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsd
class MessageHeader
{
    bdlb::BigEndianUint32 d_magic;
    bdlb::BigEndianUint32 d_version;
    bdlb::BigEndianUint32 d_crc;
    bdlb::BigEndianUint32 d_type;
    bdlb::BigEndianUint32 d_machineId;
    bdlb::BigEndianUint32 d_userId;
    bdlb::BigEndianUint32 d_transactionId;
    bdlb::BigEndianUint32 d_sequenceNumber;
    bdlb::BigEndianUint32 d_requestSize;
    bdlb::BigEndianUint32 d_responseSize;
    bdlb::BigEndianUint32 d_requestDelay;
    bdlb::BigEndianUint32 d_responseDelay;
    bdlb::BigEndianUint64 d_requestTimestamp;
    bdlb::BigEndianUint64 d_responseTimestamp;

  public:
    enum { k_MAGIC = 123456789, k_VERSION = 1 };

    /// Create a new message header having a default value.
    MessageHeader();

    /// Create a new message header having the same value as the specified
    /// 'original' object.
    MessageHeader(const MessageHeader& original);

    /// Destroy this object.
    ~MessageHeader();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    MessageHeader& operator=(const MessageHeader& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the magic number identifying this protocol to the specified
    /// 'value'.
    void setMagic(bsl::uint32_t value);

    /// Set the version of this protocol to the specified 'value'.
    void setVersion(bsl::uint32_t value);

    /// Set the cyclic redudancy check on the entire message contents,
    /// including the header, to the specified 'value'.
    void setCrc(bsl::uint32_t value);

    /// Set the type of message to the specified 'value'.
    void setType(bsl::uint32_t value);

    /// Set the machine identifier to the specified 'value'.
    void setMachineId(bsl::uint32_t value);

    /// Set the user identifier to the specified 'value'.
    void setUserId(bsl::uint32_t value);

    /// Set the transaction identifier to the specified 'value'.
    void setTransactionId(bsl::uint32_t value);

    /// Set the sequence number to the specified 'value'.
    void setSequenceNumber(bsl::uint32_t value);

    /// Set the size of the request, in bytes, to the specified 'value'.
    void setRequestSize(bsl::uint32_t value);

    /// Set the size of the response, in bytes, to the specified 'value'.
    void setResponseSize(bsl::uint32_t value);

    /// Set the amount of time to sleep before sending a request, in
    /// milliseconds, to the specified 'value'.
    void setRequestDelay(bsl::uint32_t value);

    /// Set the amount of time to sleep before returning a response, in
    /// milliseconds, to the specified 'value'.
    void setResponseDelay(bsl::uint32_t value);

    /// Set the time the request was generated, in microseconds since the
    /// Unix epoch, to the specified 'value'.
    void setRequestTimestamp(bsl::uint64_t value);

    /// Set the time the response was generated, in microseconds since the
    /// Unix epoch, to the specified 'value'.
    void setResponseTimestamp(bsl::uint64_t value);

    /// Return the magic number identifying this protocol.
    bsl::uint32_t magic() const;

    /// Return the version of this protocol.
    bsl::uint32_t version() const;

    /// Return the cyclic redudancy check on the entire message contents,
    /// including the header.
    bsl::uint32_t crc() const;

    /// Return the type of message.
    bsl::uint32_t type() const;

    /// Return the machine identifier.
    bsl::uint64_t machineId() const;

    /// Return the user identifier.
    bsl::uint32_t userId() const;

    /// Return the transaction identifier.
    bsl::uint32_t transactionId() const;

    /// Return the sequence number.
    bsl::uint32_t sequenceNumber() const;

    /// Return the size of the request, in bytes.
    bsl::uint32_t requestSize() const;

    /// Return the size of the response, in bytes.
    bsl::uint32_t responseSize() const;

    /// Return the amount of time to sleep before sending a request, in
    /// milliseconds.
    bsl::uint32_t requestDelay() const;

    /// Return the amount of time to sleep before returning a response, in
    /// milliseconds.
    bsl::uint32_t responseDelay() const;

    /// Return the time the request was generated, in microseconds since
    /// the Unix epoch.
    bsl::uint64_t requestTimestamp() const;

    /// Return the time the response was generated, in microseconds since
    /// the Unix epoch.
    bsl::uint64_t responseTimestamp() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const MessageHeader& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const MessageHeader& other) const;

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
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(MessageHeader);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsd::MessageHeader
bsl::ostream& operator<<(bsl::ostream& stream, const MessageHeader& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsd::MessageHeader
bool operator==(const MessageHeader& lhs, const MessageHeader& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsd::MessageHeader
bool operator!=(const MessageHeader& lhs, const MessageHeader& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsd::MessageHeader
bool operator<(const MessageHeader& lhs, const MessageHeader& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsd::MessageHeader
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const MessageHeader& value);

NTSCFG_INLINE
MessageHeader::MessageHeader()
: d_magic()
, d_version()
, d_crc()
, d_type()
, d_machineId()
, d_userId()
, d_transactionId()
, d_sequenceNumber()
, d_requestSize()
, d_responseSize()
, d_requestDelay()
, d_responseDelay()
, d_requestTimestamp()
, d_responseTimestamp()
{
    d_magic   = static_cast<bsl::uint32_t>(k_MAGIC);
    d_version = static_cast<bsl::uint32_t>(k_VERSION);
}

NTSCFG_INLINE
MessageHeader::MessageHeader(const MessageHeader& original)
: d_magic(original.d_magic)
, d_version(original.d_version)
, d_crc(original.d_crc)
, d_type(original.d_type)
, d_machineId(original.d_machineId)
, d_userId(original.d_userId)
, d_transactionId(original.d_transactionId)
, d_sequenceNumber(original.d_sequenceNumber)
, d_requestSize(original.d_requestSize)
, d_responseSize(original.d_responseSize)
, d_requestDelay(original.d_requestDelay)
, d_responseDelay(original.d_responseDelay)
, d_requestTimestamp(original.d_requestTimestamp)
, d_responseTimestamp(original.d_responseTimestamp)
{
}

NTSCFG_INLINE
MessageHeader::~MessageHeader()
{
}

NTSCFG_INLINE
MessageHeader& MessageHeader::operator=(const MessageHeader& other)
{
    if (this != &other) {
        d_magic             = other.d_magic;
        d_version           = other.d_version;
        d_crc               = other.d_crc;
        d_type              = other.d_type;
        d_machineId         = other.d_machineId;
        d_userId            = other.d_userId;
        d_transactionId     = other.d_transactionId;
        d_sequenceNumber    = other.d_sequenceNumber;
        d_requestSize       = other.d_requestSize;
        d_responseSize      = other.d_responseSize;
        d_requestDelay      = other.d_requestDelay;
        d_responseDelay     = other.d_responseDelay;
        d_requestTimestamp  = other.d_requestTimestamp;
        d_responseTimestamp = other.d_responseTimestamp;
    }

    return *this;
}

NTSCFG_INLINE
void MessageHeader::reset()
{
    d_magic             = 0;
    d_version           = 0;
    d_crc               = 0;
    d_type              = 0;
    d_machineId         = 0;
    d_userId            = 0;
    d_transactionId     = 0;
    d_sequenceNumber    = 0;
    d_requestSize       = 0;
    d_responseSize      = 0;
    d_requestDelay      = 0;
    d_responseDelay     = 0;
    d_requestTimestamp  = 0;
    d_responseTimestamp = 0;
}

NTSCFG_INLINE
void MessageHeader::setMagic(bsl::uint32_t value)
{
    d_magic = value;
}

NTSCFG_INLINE
void MessageHeader::setVersion(bsl::uint32_t value)
{
    d_version = value;
}

NTSCFG_INLINE
void MessageHeader::setCrc(bsl::uint32_t value)
{
    d_crc = value;
}

NTSCFG_INLINE
void MessageHeader::setType(bsl::uint32_t value)
{
    d_type = value;
}

NTSCFG_INLINE
void MessageHeader::setMachineId(bsl::uint32_t value)
{
    d_machineId = value;
}

NTSCFG_INLINE
void MessageHeader::setUserId(bsl::uint32_t value)
{
    d_userId = value;
}

NTSCFG_INLINE
void MessageHeader::setTransactionId(bsl::uint32_t value)
{
    d_transactionId = value;
}

NTSCFG_INLINE
void MessageHeader::setSequenceNumber(bsl::uint32_t value)
{
    d_sequenceNumber = value;
}

NTSCFG_INLINE
void MessageHeader::setRequestSize(bsl::uint32_t value)
{
    d_requestSize = value;
}

NTSCFG_INLINE
void MessageHeader::setResponseSize(bsl::uint32_t value)
{
    d_responseSize = value;
}

NTSCFG_INLINE
void MessageHeader::setRequestDelay(bsl::uint32_t value)
{
    d_requestDelay = value;
}

NTSCFG_INLINE
void MessageHeader::setResponseDelay(bsl::uint32_t value)
{
    d_responseDelay = value;
}

NTSCFG_INLINE
void MessageHeader::setRequestTimestamp(bsl::uint64_t value)
{
    d_requestTimestamp = value;
}

NTSCFG_INLINE
void MessageHeader::setResponseTimestamp(bsl::uint64_t value)
{
    d_responseTimestamp = value;
}

NTSCFG_INLINE
bsl::uint32_t MessageHeader::magic() const
{
    return d_magic;
}

NTSCFG_INLINE
bsl::uint32_t MessageHeader::version() const
{
    return d_version;
}

NTSCFG_INLINE
bsl::uint32_t MessageHeader::crc() const
{
    return d_crc;
}

NTSCFG_INLINE
bsl::uint32_t MessageHeader::type() const
{
    return d_type;
}

NTSCFG_INLINE
bsl::uint64_t MessageHeader::machineId() const
{
    return d_machineId;
}

NTSCFG_INLINE
bsl::uint32_t MessageHeader::userId() const
{
    return d_userId;
}

NTSCFG_INLINE
bsl::uint32_t MessageHeader::transactionId() const
{
    return d_transactionId;
}

NTSCFG_INLINE
bsl::uint32_t MessageHeader::sequenceNumber() const
{
    return d_sequenceNumber;
}

NTSCFG_INLINE
bsl::uint32_t MessageHeader::requestSize() const
{
    return d_requestSize;
}

NTSCFG_INLINE
bsl::uint32_t MessageHeader::responseSize() const
{
    return d_responseSize;
}

NTSCFG_INLINE
bsl::uint32_t MessageHeader::requestDelay() const
{
    return d_requestDelay;
}

NTSCFG_INLINE
bsl::uint32_t MessageHeader::responseDelay() const
{
    return d_responseDelay;
}

NTSCFG_INLINE
bsl::uint64_t MessageHeader::requestTimestamp() const
{
    return d_requestTimestamp;
}

NTSCFG_INLINE
bsl::uint64_t MessageHeader::responseTimestamp() const
{
    return d_responseTimestamp;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const MessageHeader& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const MessageHeader& lhs, const MessageHeader& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const MessageHeader& lhs, const MessageHeader& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const MessageHeader& lhs, const MessageHeader& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const MessageHeader& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.magic());
    hashAppend(algorithm, value.version());
    hashAppend(algorithm, value.crc());
    hashAppend(algorithm, value.type());
    hashAppend(algorithm, value.machineId());
    hashAppend(algorithm, value.userId());
    hashAppend(algorithm, value.transactionId());
    hashAppend(algorithm, value.sequenceNumber());
    hashAppend(algorithm, value.requestSize());
    hashAppend(algorithm, value.responseSize());
    hashAppend(algorithm, value.requestDelay());
    hashAppend(algorithm, value.responseDelay());
    hashAppend(algorithm, value.requestTimestamp());
    hashAppend(algorithm, value.responseTimestamp());
}

}  // close package namespace
}  // close enterprise namespace
#endif
