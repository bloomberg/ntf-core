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

#ifndef INCLUDED_NTSD_MESSAGE
#define INCLUDED_NTSD_MESSAGE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntscfg_platform.h>
#include <ntsd_messageheader.h>
#include <ntsd_messagetype.h>
#include <ntsscm_version.h>
#include <bdlb_bigendian.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobutil.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsd {

/// @internal @brief
/// Provide a test message.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsd
class Message
{
    ntsd::MessageHeader d_header;
    bdlbb::Blob         d_payload;

  public:
    /// Create a new message having a default value with an undefined
    /// blob buffer factory. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Message(bslma::Allocator* basicAllocator = 0);

    /// Create a new message having a default value using the specified
    /// 'blobBufferFactory' to supply blob buffers. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Message(bdlbb::BlobBufferFactory* blobBufferFactory,
                     bslma::Allocator*         basicAllocator = 0);

    /// Create a new message with the specified 'header' and
    /// 'payload'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    Message(const ntsd::MessageHeader& header,
            const bdlbb::Blob&         payload,
            bslma::Allocator*          basicAllocator = 0);

    /// Create a new message with the specified 'header' and
    /// 'payload'. Supply blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    Message(const ntsd::MessageHeader& header,
            const bdlbb::Blob&         payload,
            bdlbb::BlobBufferFactory*  blobBufferFactory,
            bslma::Allocator*          basicAllocator = 0);

    /// Create a new message having the same value as the specified
    /// 'original' object.  Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.  Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    Message(const Message& original, bslma::Allocator* basicAllocator = 0);

    /// Create a new message having the same value as the specified
    /// 'original' object. Supply blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.  Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    Message(const Message&            original,
            bdlbb::BlobBufferFactory* blobBufferFactory,
            bslma::Allocator*         basicAllocator = 0);

    /// Destroy this object.
    ~Message();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Message& operator=(const Message& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the magic number identifying this protocol to the specified
    /// 'value'.
    void setMagic(bsl::uint32_t value);

    /// Set the version of this protocol to the specified 'value'.
    void setVersion(bsl::uint32_t value);

    /// Set the cyclic redundancy check on the entire message contents,
    /// including the header, to the specified 'value'.
    void setCrc(bsl::uint32_t value);

    /// Set the type of message to the specified 'value'.
    void setType(ntsd::MessageType::Value value);

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
    void setRequestDelay(const bsls::TimeInterval& value);

    /// Set the amount of time to sleep before returning a response, in
    /// milliseconds, to the specified 'value'.
    void setResponseDelay(const bsls::TimeInterval& value);

    /// Set the time the request was generated, in microseconds since the
    /// Unix epoch, to the specified 'value'.
    void setRequestTimestamp(const bsls::TimeInterval& value);

    /// Set the time the response was generated, in microseconds since the
    /// Unix epoch, to the specified 'value'.
    void setResponseTimestamp(const bsls::TimeInterval& value);

    /// Set the header to the specified 'header'.
    void setHeader(const ntsd::MessageHeader& header);

    /// Set the payload to the specified 'payload'.
    void setPayload(const bdlbb::Blob& payload);

    /// Move the specified 'message' into this object and reset the
    /// 'message' to its value upon default construction.
    void moveFrom(ntsd::Message* message);

    /// Assign to the header of this message the first
    /// 'sizeof(ntsa::MessageHeader)' bytes of the specified 'data' and
    /// remove those bytes from the front of 'data'.
    void moveHeader(bdlbb::Blob* data);

    /// Assign to the payload of this message the first specified 'size'
    /// bytes of the specified 'data' and remove those bytes from the
    /// 'data'.
    void movePayload(bdlbb::Blob* data, bsl::size_t size);

    /// Decode a message from the specified 'data' into this object. Return
    /// the error.
    ntsa::Error decode(bdlbb::Blob* data);

    /// Encode this message into the specified 'data'. Return the error.
    ntsa::Error encode(bdlbb::Blob* data) const;

    /// Return the magic number identifying this protocol.
    bsl::uint32_t magic() const;

    /// Return the version of this protocol.
    bsl::uint32_t version() const;

    /// Return the cyclic redundancy check on the entire message contents,
    /// including the header.
    bsl::uint32_t crc() const;

    /// Return the type of message.
    ntsd::MessageType::Value type() const;

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
    bsls::TimeInterval requestDelay() const;

    /// Return the amount of time to sleep before returning a response, in
    /// milliseconds.
    bsls::TimeInterval responseDelay() const;

    /// Return the time the request was generated, in microseconds since
    /// the Unix epoch.
    bsls::TimeInterval requestTimestamp() const;

    /// Return the time the response was generated, in microseconds since
    /// the Unix epoch.
    bsls::TimeInterval responseTimestamp() const;

    /// Return the header.
    const ntsd::MessageHeader& header() const;

    /// Return the payload.
    const bdlbb::Blob& payload() const;

    /// Return the allocator used to supply memory.
    bslma::Allocator* allocator() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Message& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Message& other) const;

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

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Message);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsd::Message
bsl::ostream& operator<<(bsl::ostream& stream, const Message& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsd::Message
bool operator==(const Message& lhs, const Message& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsd::Message
bool operator!=(const Message& lhs, const Message& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsd::Message
bool operator<(const Message& lhs, const Message& rhs);

NTSCFG_INLINE
Message::Message(bslma::Allocator* basicAllocator)
: d_header()
, d_payload(basicAllocator)
{
}

NTSCFG_INLINE
Message::Message(bdlbb::BlobBufferFactory* blobBufferFactory,
                 bslma::Allocator*         basicAllocator)
: d_header()
, d_payload(blobBufferFactory, basicAllocator)
{
}

NTSCFG_INLINE
Message::Message(const ntsd::MessageHeader& header,
                 const bdlbb::Blob&         payload,
                 bslma::Allocator*          basicAllocator)
: d_header(header)
, d_payload(payload, basicAllocator)
{
}

NTSCFG_INLINE
Message::Message(const ntsd::MessageHeader& header,
                 const bdlbb::Blob&         payload,
                 bdlbb::BlobBufferFactory*  blobBufferFactory,
                 bslma::Allocator*          basicAllocator)
: d_header(header)
, d_payload(payload, blobBufferFactory, basicAllocator)
{
}

NTSCFG_INLINE
Message::Message(const Message& original, bslma::Allocator* basicAllocator)
: d_header(original.d_header)
, d_payload(original.d_payload, basicAllocator)
{
}

NTSCFG_INLINE
Message::Message(const Message&            original,
                 bdlbb::BlobBufferFactory* blobBufferFactory,
                 bslma::Allocator*         basicAllocator)
: d_header(original.d_header)
, d_payload(original.d_payload, blobBufferFactory, basicAllocator)
{
}

NTSCFG_INLINE
Message::~Message()
{
}

NTSCFG_INLINE
Message& Message::operator=(const Message& other)
{
    if (this != &other) {
        d_header  = other.d_header;
        d_payload = other.d_payload;
    }

    return *this;
}

NTSCFG_INLINE
void Message::reset()
{
    d_header.reset();
    d_payload.removeAll();
}

NTSCFG_INLINE
void Message::setMagic(bsl::uint32_t value)
{
    d_header.setMagic(value);
}

NTSCFG_INLINE
void Message::setVersion(bsl::uint32_t value)
{
    d_header.setVersion(value);
}

NTSCFG_INLINE
void Message::setCrc(bsl::uint32_t value)
{
    d_header.setCrc(value);
}

NTSCFG_INLINE
void Message::setType(ntsd::MessageType::Value value)
{
    d_header.setType(static_cast<bsl::uint32_t>(value));
}

NTSCFG_INLINE
void Message::setMachineId(bsl::uint32_t value)
{
    d_header.setMachineId(value);
}

NTSCFG_INLINE
void Message::setUserId(bsl::uint32_t value)
{
    d_header.setUserId(value);
}

NTSCFG_INLINE
void Message::setTransactionId(bsl::uint32_t value)
{
    d_header.setTransactionId(value);
}

NTSCFG_INLINE
void Message::setSequenceNumber(bsl::uint32_t value)
{
    d_header.setSequenceNumber(value);
}

NTSCFG_INLINE
void Message::setRequestSize(bsl::uint32_t value)
{
    d_header.setRequestSize(value);
}

NTSCFG_INLINE
void Message::setResponseSize(bsl::uint32_t value)
{
    d_header.setResponseSize(value);
}

NTSCFG_INLINE
void Message::setRequestDelay(const bsls::TimeInterval& value)
{
    d_header.setRequestDelay(
        NTSCFG_WARNING_NARROW(bsl::uint32_t, value.totalMicroseconds()));
}

NTSCFG_INLINE
void Message::setResponseDelay(const bsls::TimeInterval& value)
{
    d_header.setResponseDelay(
        NTSCFG_WARNING_NARROW(bsl::uint32_t, value.totalMicroseconds()));
}

NTSCFG_INLINE
void Message::setRequestTimestamp(const bsls::TimeInterval& value)
{
    d_header.setRequestTimestamp(value.totalMicroseconds());
}

NTSCFG_INLINE
void Message::setResponseTimestamp(const bsls::TimeInterval& value)
{
    d_header.setResponseTimestamp(value.totalMicroseconds());
}

NTSCFG_INLINE
void Message::setHeader(const ntsd::MessageHeader& header)
{
    d_header = header;
}

NTSCFG_INLINE
void Message::setPayload(const bdlbb::Blob& payload)
{
    d_payload = payload;
}

NTSCFG_INLINE
void Message::moveFrom(ntsd::Message* message)
{
    if (message->allocator() == d_payload.allocator()) {
        d_payload.swap(message->d_payload);
        if (message->d_payload.length() > 0) {
            message->d_payload.removeAll();
        }
    }
    else {
        d_payload.moveBuffers(&message->d_payload);
    }

    d_header = message->d_header;
    message->d_header.reset();
}

NTSCFG_INLINE
void Message::moveHeader(bdlbb::Blob* data)
{
    BSLS_ASSERT(NTSCFG_WARNING_PROMOTE(bsl::size_t, data->length()) >=
                sizeof(ntsd::MessageHeader));

    bdlbb::BlobUtil::copy(reinterpret_cast<char*>(&d_header),
                          *data,
                          0,
                          sizeof(ntsd::MessageHeader));

    bdlbb::BlobUtil::erase(
        data,
        0,
        NTSCFG_WARNING_NARROW(int, sizeof(ntsd::MessageHeader)));
}

NTSCFG_INLINE
void Message::movePayload(bdlbb::Blob* data, bsl::size_t size)
{
    BSLS_ASSERT(NTSCFG_WARNING_PROMOTE(bsl::size_t, data->length()) >= size);

    bdlbb::BlobUtil::append(&d_payload,
                            *data,
                            0,
                            NTSCFG_WARNING_NARROW(int, size));

    bdlbb::BlobUtil::erase(data, 0, NTSCFG_WARNING_NARROW(int, size));
}

NTSCFG_INLINE
ntsa::Error Message::decode(bdlbb::Blob* data)
{
    if (NTSCFG_WARNING_PROMOTE(bsl::size_t, data->length()) <
        sizeof(ntsd::MessageHeader))
    {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    bdlbb::BlobUtil::copy(reinterpret_cast<char*>(&d_header),
                          *data,
                          0,
                          sizeof(ntsd::MessageHeader));

    if (d_header.type() == ntsd::MessageType::e_REQUEST ||
        d_header.type() == ntsd::MessageType::e_ONE_WAY)
    {
        if (NTSCFG_WARNING_PROMOTE(bsl::size_t, data->length()) <
            sizeof(ntsd::MessageHeader) + d_header.requestSize())
        {
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }

        bdlbb::BlobUtil::append(&d_payload,
                                *data,
                                sizeof(ntsd::MessageHeader),
                                d_header.requestSize());

        bdlbb::BlobUtil::erase(
            data,
            0,
            NTSCFG_WARNING_NARROW(
                int,
                sizeof(ntsd::MessageHeader) + d_header.requestSize()));

        return ntsa::Error();
    }
    else if (d_header.type() == ntsd::MessageType::e_RESPONSE) {
        if (NTSCFG_WARNING_PROMOTE(bsl::size_t, data->length()) <
            sizeof(ntsd::MessageHeader) + d_header.responseSize())
        {
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }

        bdlbb::BlobUtil::append(&d_payload,
                                *data,
                                sizeof(ntsd::MessageHeader),
                                d_header.responseSize());

        bdlbb::BlobUtil::erase(
            data,
            0,
            NTSCFG_WARNING_NARROW(
                int,
                sizeof(ntsd::MessageHeader) + d_header.responseSize()));

        return ntsa::Error();
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

NTSCFG_INLINE
ntsa::Error Message::encode(bdlbb::Blob* data) const
{
    bdlbb::BlobUtil::append(data,
                            reinterpret_cast<const char*>(&d_header),
                            sizeof(ntsd::MessageHeader));

    if (d_payload.length() > 0) {
        bdlbb::BlobUtil::append(data, d_payload);
    }

    return ntsa::Error();
}

NTSCFG_INLINE
bsl::uint32_t Message::magic() const
{
    return d_header.magic();
}

NTSCFG_INLINE
bsl::uint32_t Message::version() const
{
    return d_header.version();
}

NTSCFG_INLINE
bsl::uint32_t Message::crc() const
{
    return d_header.crc();
}

NTSCFG_INLINE
ntsd::MessageType::Value Message::type() const
{
    ntsd::MessageType::Value result;
    if (ntsd::MessageType::fromInt(&result,
                                   static_cast<int>(d_header.type())) != 0)
    {
        return ntsd::MessageType::e_UNDEFINED;
    }
    return result;
}

NTSCFG_INLINE
bsl::uint64_t Message::machineId() const
{
    return d_header.machineId();
}

NTSCFG_INLINE
bsl::uint32_t Message::userId() const
{
    return d_header.userId();
}

NTSCFG_INLINE
bsl::uint32_t Message::transactionId() const
{
    return d_header.transactionId();
}

NTSCFG_INLINE
bsl::uint32_t Message::sequenceNumber() const
{
    return d_header.sequenceNumber();
}

NTSCFG_INLINE
bsl::uint32_t Message::requestSize() const
{
    return d_header.requestSize();
}

NTSCFG_INLINE
bsl::uint32_t Message::responseSize() const
{
    return d_header.responseSize();
}

NTSCFG_INLINE
bsls::TimeInterval Message::requestDelay() const
{
    bsls::TimeInterval result;
    result.setTotalMicroseconds(d_header.requestDelay());
    return result;
}

NTSCFG_INLINE
bsls::TimeInterval Message::responseDelay() const
{
    bsls::TimeInterval result;
    result.setTotalMicroseconds(d_header.responseDelay());
    return result;
}

NTSCFG_INLINE
bsls::TimeInterval Message::requestTimestamp() const
{
    bsls::TimeInterval result;
    result.setTotalMicroseconds(d_header.requestTimestamp());
    return result;
}

NTSCFG_INLINE
bsls::TimeInterval Message::responseTimestamp() const
{
    bsls::TimeInterval result;
    result.setTotalMicroseconds(d_header.responseTimestamp());
    return result;
}

NTSCFG_INLINE
const ntsd::MessageHeader& Message::header() const
{
    return d_header;
}

NTSCFG_INLINE
const bdlbb::Blob& Message::payload() const
{
    return d_payload;
}

NTSCFG_INLINE
bslma::Allocator* Message::allocator() const
{
    return d_payload.allocator();
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Message& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const Message& lhs, const Message& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const Message& lhs, const Message& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const Message& lhs, const Message& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
#endif
