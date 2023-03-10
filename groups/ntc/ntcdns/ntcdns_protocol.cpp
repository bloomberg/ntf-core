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

#include <ntcdns_protocol.h>

#include <ntci_log.h>
#include <ntsa_ipv4address.h>
#include <ntsa_ipv6address.h>

#include <bdlb_chartype.h>
#include <bdlb_string.h>
#include <bdlb_stringrefutil.h>
#include <bdlb_tokenizer.h>
#include <bdlbb_blobutil.h>
#include <ball_log.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcdns {

namespace {

// The length of a typical domain name.
const bsl::size_t k_TYPICAL_NAME_LENGTH = 128;

// The maximum length of a single label within a domain name.
const bsl::size_t k_MAX_LABEL_LENGTH = 63;

// The maximum length of a character-string.
const bsl::size_t k_MAX_CHARACTER_STRING_LENGTH = 255;

// The maximum data of raw resource record data.
const bsl::size_t k_MAX_RDATA_LENGTH = 65535;

// The maximum recursion depth to follow when recursively decompressing labels.
const bsl::size_t k_MAX_LABEL_RESOLUTION_RECURSION_DEPTH = 32;

ntsa::Error checkOverflow(bsl::size_t numBytesRemaining,
                          bsl::size_t numBytesNeeded)
{
    // Verify the specified 'numBytesRemaining' is sufficient to store the
    // specified 'numBytesNeeded'. Return the error.

    NTCI_LOG_CONTEXT();

    if (numBytesRemaining < numBytesNeeded) {
        NTCI_LOG_STREAM_ERROR
            << "Failed to encode: the buffer is too small: need "
            << numBytesNeeded << ", have " << numBytesRemaining
            << NTCI_LOG_STREAM_END;

        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error checkUnderflow(bsl::size_t numBytesRemaining,
                           bsl::size_t numBytesNeeded)
{
    // Verify the specified 'numBytesRemaining' is sufficient to store the
    // specified 'numBytesNeeded'. Return the error.

    NTCI_LOG_CONTEXT();

    if (numBytesRemaining < numBytesNeeded) {
        NTCI_LOG_STREAM_ERROR
            << "Failed to decode: the buffer is too small: need "
            << numBytesNeeded << ", have " << numBytesRemaining
            << NTCI_LOG_STREAM_END;

        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error checkToken(const bsl::string& name, const bslstl::StringRef& token)
{
    // Verify the specified 'token' in the specified 'name'. Return the error.

    NTCI_LOG_CONTEXT();

    if (token.size() > k_MAX_LABEL_LENGTH) {
        NTCI_LOG_STREAM_ERROR << "Failed to encode DNS question: token '"
                              << token << "' in name '" << name
                              << "' is too long" << NTCI_LOG_STREAM_END;

        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error checkCharacterString(const bsl::string& value)
{
    // Verify the specify character-string 'value'. Return the error.

    NTCI_LOG_CONTEXT();

    if (value.size() > k_MAX_CHARACTER_STRING_LENGTH) {
        NTCI_LOG_STREAM_ERROR
            << "Invalid DNS character string '" << value << "': the length "
            << value.size() << " is greater than the maximum length "
            << k_MAX_CHARACTER_STRING_LENGTH << NTCI_LOG_STREAM_END;

        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error checkRdata(const bdlbb::Blob& value)
{
    // Verify the specify resource record 'value'. Return the error.

    NTCI_LOG_CONTEXT();

    if (NTCCFG_WARNING_PROMOTE(bsl::size_t, value.length()) >
        k_MAX_RDATA_LENGTH)
    {
        NTCI_LOG_STREAM_ERROR
            << "Invalid DNS resource record data: the length "
            << value.length() << " is greater than the maximum length "
            << k_MAX_RDATA_LENGTH << NTCI_LOG_STREAM_END;

        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error checkExpectedRdataLength(bsl::size_t expected, bsl::size_t found)
{
    // Verify the specified 'expected' RDATA length matches the specified
    // 'found' RDATA length. Return the error.

    NTCI_LOG_CONTEXT();

    if (found != expected) {
        NTCI_LOG_STREAM_ERROR << "Unexpected RDATA length: expected "
                              << expected << ", found " << found
                              << NTCI_LOG_STREAM_END;

        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error checkCoherentRdataLength(bsl::size_t expected, bsl::size_t found)
{
    // Verify the specified 'expected' RDATA length matches the specified
    // 'found' RDATA length. Return the error.

    NTCI_LOG_CONTEXT();

    if (found != expected) {
        NTCI_LOG_STREAM_ERROR << "Incoherent RDATA length: expected "
                              << expected << ", found " << found
                              << NTCI_LOG_STREAM_END;

        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

}  // close unnamed namespace

MemoryEncoder::MemoryEncoder(uint8_t* data, bsl::size_t size)
: d_begin(data)
, d_current(data)
, d_end(data + size)
{
}

MemoryEncoder::~MemoryEncoder()
{
}

ntsa::Error MemoryEncoder::encodeUint8(bsl::uint8_t value)
{
    ntsa::Error error = checkOverflow(d_end - d_current, sizeof value);
    if (error) {
        return error;
    }

    *d_current = value;
    d_current  += sizeof value;

    return ntsa::Error();
}

ntsa::Error MemoryEncoder::encodeUint16(bsl::uint16_t value)
{
    bdlb::BigEndianUint16 bigEndianValue = bdlb::BigEndianUint16::make(value);
    return encodeUint16(bigEndianValue);
}

ntsa::Error MemoryEncoder::encodeUint16(const bdlb::BigEndianUint16& value)
{
    ntsa::Error error = checkOverflow(d_end - d_current, sizeof value);
    if (error) {
        return error;
    }

    bsl::memcpy(d_current, &value, sizeof value);
    d_current += sizeof value;

    return ntsa::Error();
}

ntsa::Error MemoryEncoder::encodeUint32(bsl::uint32_t value)
{
    bdlb::BigEndianUint32 bigEndianValue = bdlb::BigEndianUint32::make(value);
    return encodeUint32(bigEndianValue);
}

ntsa::Error MemoryEncoder::encodeUint32(const bdlb::BigEndianUint32& value)
{
    ntsa::Error error = checkOverflow(d_end - d_current, sizeof value);
    if (error) {
        return error;
    }

    bsl::memcpy(d_current, &value, sizeof value);
    d_current += sizeof value;

    return ntsa::Error();
}

ntsa::Error MemoryEncoder::encodeDomainName(const bsl::string& value)
{
    ntsa::Error error;

    bdlb::Tokenizer tokenizer(value, ".", "");

    while (tokenizer.isValid()) {
        bslstl::StringRef token = tokenizer.token();

        error = checkToken(value, token);
        if (error) {
            return error;
        }

        bsl::uint8_t length = static_cast<bsl::uint8_t>(token.size());

        error = checkOverflow(d_end - d_current, sizeof length);
        if (error) {
            return error;
        }

        *d_current = length;
        d_current  += sizeof length;

        error = checkOverflow(d_end - d_current, token.size());
        if (error) {
            return error;
        }

        bsl::memcpy(d_current, token.data(), token.size());
        d_current += token.size();

        ++tokenizer;
    }

    error = checkOverflow(d_end - d_current, 1);
    if (error) {
        return error;
    }

    *d_current = 0;
    d_current  += 1;

    return ntsa::Error();
}

ntsa::Error MemoryEncoder::encodeCharacterString(const bsl::string& value)
{
    ntsa::Error error;

    error = checkCharacterString(value);
    if (error) {
        return error;
    }

    bsl::uint8_t length = static_cast<bsl::uint8_t>(value.size());

    error = checkOverflow(d_end - d_current, sizeof length);
    if (error) {
        return error;
    }

    *d_current = length;
    d_current  += sizeof length;

    error = checkOverflow(d_end - d_current, value.size());
    if (error) {
        return error;
    }

    bsl::memcpy(d_current, value.c_str(), value.size());
    d_current += value.size();

    return ntsa::Error();
}

ntsa::Error MemoryEncoder::encodeRdata(const bdlbb::Blob& value)
{
    ntsa::Error error;

    error = checkRdata(value);
    if (error) {
        return error;
    }

    bsl::uint16_t rdataLength = static_cast<bsl::uint16_t>(value.length());

    bdlb::BigEndianUint16 bigEndianRdataLength =
        bdlb::BigEndianUint16::make(rdataLength);

    error = checkOverflow(d_end - d_current, sizeof bigEndianRdataLength);
    if (error) {
        return error;
    }

    bsl::memcpy(d_current, &bigEndianRdataLength, sizeof bigEndianRdataLength);
    d_current += sizeof bigEndianRdataLength;

    if (rdataLength > 0) {
        error = checkOverflow(d_end - d_current, rdataLength);
        if (error) {
            return error;
        }

        bdlbb::BlobUtil::copy(reinterpret_cast<char*>(d_current),
                              value,
                              0,
                              rdataLength);
        d_current += value.length();
    }

    return ntsa::Error();
}

ntsa::Error MemoryEncoder::encodeRaw(const void* value, bsl::size_t size)
{
    ntsa::Error error = checkOverflow(d_end - d_current, size);
    if (error) {
        return error;
    }

    bsl::memcpy(d_current, value, size);
    d_current += size;

    return ntsa::Error();
}

ntsa::Error MemoryEncoder::seek(bsl::size_t position)
{
    bsl::uint8_t* target = d_begin + position;

    if (target > d_end) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_current = target;

    return ntsa::Error();
}

ntsa::Error MemoryEncoder::advance(bsl::size_t amount)
{
    bsl::uint8_t* target = d_current + amount;

    if (target > d_end) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_current = target;

    return ntsa::Error();
}

ntsa::Error MemoryEncoder::rewind(bsl::size_t amount)
{
    bsl::uint8_t* target = d_current - amount;

    if (target < d_begin) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_current = target;

    return ntsa::Error();
}

bsl::uint8_t* MemoryEncoder::begin() const
{
    return d_begin;
}

bsl::uint8_t* MemoryEncoder::current() const
{
    return d_current;
}

bsl::uint8_t* MemoryEncoder::end() const
{
    return d_end;
}

bsl::size_t MemoryEncoder::position() const
{
    return d_current - d_begin;
}

bsl::size_t MemoryEncoder::capacity() const
{
    return d_end - d_begin;
}

MemoryDecoder::MemoryDecoder(const bsl::uint8_t* data, bsl::size_t size)
: d_begin(data)
, d_current(data)
, d_end(data + size)
{
}

MemoryDecoder::~MemoryDecoder()
{
}

ntsa::Error MemoryDecoder::decodeUint8(bsl::uint8_t* value)
{
    ntsa::Error error = checkUnderflow(d_end - d_current, sizeof *value);
    if (error) {
        return error;
    }

    *value    = *d_current;
    d_current += sizeof *value;

    return ntsa::Error();
}

ntsa::Error MemoryDecoder::decodeUint16(bsl::uint16_t* value)
{
    bdlb::BigEndianUint16 bigEndianValue;
    ntsa::Error           error = decodeUint16(&bigEndianValue);
    if (error) {
        return error;
    }

    *value = static_cast<bsl::uint16_t>(bigEndianValue);

    return ntsa::Error();
}

ntsa::Error MemoryDecoder::decodeUint16(bdlb::BigEndianUint16* value)
{
    ntsa::Error error = checkUnderflow(d_end - d_current, sizeof *value);
    if (error) {
        return error;
    }

    bsl::memcpy(value, d_current, sizeof *value);
    d_current += sizeof *value;

    return ntsa::Error();
}

ntsa::Error MemoryDecoder::decodeUint32(bsl::uint32_t* value)
{
    bdlb::BigEndianUint32 bigEndianValue;
    ntsa::Error           error = decodeUint32(&bigEndianValue);
    if (error) {
        return error;
    }

    *value = static_cast<bsl::uint32_t>(bigEndianValue);

    return ntsa::Error();
}

ntsa::Error MemoryDecoder::decodeUint32(bdlb::BigEndianUint32* value)
{
    ntsa::Error error = checkUnderflow(d_end - d_current, sizeof *value);
    if (error) {
        return error;
    }

    bsl::memcpy(value, d_current, sizeof *value);
    d_current += sizeof *value;

    return ntsa::Error();
}

ntsa::Error MemoryDecoder::decodeDomainName(bsl::string* value)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    value->clear();
    value->reserve(k_TYPICAL_NAME_LENGTH);

    while (true) {
        bsl::uint8_t length = 0;

        error = checkUnderflow(d_end - d_current, sizeof length);
        if (error) {
            return error;
        }

        length    = *d_current;
        d_current += sizeof length;

        if (length == 0) {
            break;
        }

        if (length <= k_MAX_LABEL_LENGTH) {
            if (!value->empty()) {
                value->append(1, '.');
            }

            error = checkUnderflow(d_end - d_current, length);
            if (error) {
                return error;
            }

            value->append(reinterpret_cast<const char*>(d_current),
                          static_cast<bsl::size_t>(length));

            d_current += length;
        }
        else {
            if ((length & 0xC0) == 0xC0) {
                bsl::uint8_t offsetUpper = length & 0x3F;
                bsl::uint8_t offsetLower = 0;

                error = checkUnderflow(d_end - d_current, sizeof offsetLower);
                if (error) {
                    return error;
                }

                offsetLower = *d_current;
                d_current   += sizeof offsetLower;

                bsl::uint16_t offset =
                    NTCCFG_WARNING_NARROW(bsl::uint16_t,
                                          (offsetUpper << 8) | offsetLower);

                bsl::string reference;

                error = decodeLabel(&reference, 0, offset);
                if (error) {
                    return error;
                }

                if (!value->empty()) {
                    value->append(1, '.');
                }

                value->append(reference);
                break;
            }
            else {
                NTCI_LOG_STREAM_ERROR << "Invalid message compression tag"
                                      << NTCI_LOG_STREAM_END;
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error MemoryDecoder::decodeLabel(bsl::string* value,
                                       bsl::size_t  depth,
                                       bsl::size_t  offset)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (offset > static_cast<bsl::size_t>(d_end - d_begin)) {
        NTCI_LOG_STREAM_ERROR << "Failed to resolve label: offset " << offset
                              << " greater than maximum length "
                              << d_end - d_begin << NTCI_LOG_STREAM_END;

        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint8_t* current = d_begin + offset;

    value->clear();
    value->reserve(k_TYPICAL_NAME_LENGTH);

    while (true) {
        bsl::uint8_t length = 0;

        error = checkUnderflow(d_end - current, sizeof length);
        if (error) {
            return error;
        }

        length  = *current;
        current += sizeof length;

        if (length == 0) {
            break;
        }

        if (length <= k_MAX_LABEL_LENGTH) {
            if (!value->empty()) {
                value->append(1, '.');
            }

            error = checkUnderflow(d_end - current, length);
            if (error) {
                return error;
            }

            value->append(reinterpret_cast<const char*>(current),
                          static_cast<bsl::size_t>(length));

            current += length;
        }
        else if (depth < k_MAX_LABEL_RESOLUTION_RECURSION_DEPTH) {
            if ((length & 0xC0) == 0xC0) {
                bsl::uint8_t offsetUpper = length & 0x3F;
                bsl::uint8_t offsetLower = 0;

                error = checkUnderflow(d_end - current, sizeof offsetLower);
                if (error) {
                    return error;
                }

                offsetLower = *current;
                current     += sizeof offsetLower;

                bsl::uint16_t offset =
                    NTCCFG_WARNING_NARROW(bsl::uint16_t,
                                          (offsetUpper << 8) | offsetLower);

                bsl::string reference;
                error = decodeLabel(&reference, depth + 1, offset);
                if (error) {
                    return error;
                }

                if (!value->empty()) {
                    value->append(1, '.');
                }

                value->append(reference);
                break;
            }
            else {
                NTCI_LOG_STREAM_ERROR << "Invalid message compression tag"
                                      << NTCI_LOG_STREAM_END;
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
        else {
            NTCI_LOG_STREAM_ERROR
                << "Invalid recursive message compression tag"
                << NTCI_LOG_STREAM_END;
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error MemoryDecoder::decodeCharacterString(bsl::string* value)
{
    ntsa::Error error;

    bsl::uint8_t length = 0;

    error = checkUnderflow(d_end - d_current, sizeof length);
    if (error) {
        return error;
    }

    length    = *d_current;
    d_current += sizeof length;

    if (length > 0) {
        error = checkUnderflow(d_end - d_current, length);
        if (error) {
            return error;
        }

        value->append(reinterpret_cast<const char*>(d_current),
                      static_cast<bsl::size_t>(length));

        d_current += length;
    }

    return ntsa::Error();
}

ntsa::Error MemoryDecoder::decodeRdata(bdlbb::Blob* value)
{
    ntsa::Error error;

    bdlb::BigEndianUint16 bigEndianRdataLength;

    error = checkUnderflow(d_end - d_current, sizeof bigEndianRdataLength);
    if (error) {
        return error;
    }

    bsl::memcpy(&bigEndianRdataLength, d_current, sizeof bigEndianRdataLength);
    d_current += sizeof bigEndianRdataLength;

    bsl::uint16_t rdataLength =
        static_cast<bsl::uint16_t>(bigEndianRdataLength);

    if (rdataLength > 0) {
        error = checkUnderflow(d_end - d_current, rdataLength);
        if (error) {
            return error;
        }

        bdlbb::BlobUtil::copy(value,
                              0,
                              reinterpret_cast<const char*>(d_current),
                              rdataLength);
        d_current += rdataLength;
    }

    return ntsa::Error();
}

ntsa::Error MemoryDecoder::decodeRaw(void* value, bsl::size_t size)
{
    ntsa::Error error = checkUnderflow(d_end - d_current, size);
    if (error) {
        return error;
    }

    bsl::memcpy(value, d_current, size);
    d_current += size;

    return ntsa::Error();
}

ntsa::Error MemoryDecoder::seek(bsl::size_t position)
{
    const bsl::uint8_t* target = d_begin + position;

    if (target > d_end) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_current = target;

    return ntsa::Error();
}

ntsa::Error MemoryDecoder::advance(bsl::size_t amount)
{
    const bsl::uint8_t* target = d_current + amount;

    if (target > d_end) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_current = target;

    return ntsa::Error();
}

ntsa::Error MemoryDecoder::rewind(bsl::size_t amount)
{
    const bsl::uint8_t* target = d_current - amount;

    if (target < d_begin) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_current = target;

    return ntsa::Error();
}

const bsl::uint8_t* MemoryDecoder::begin() const
{
    return d_begin;
}

const bsl::uint8_t* MemoryDecoder::current() const
{
    return d_current;
}

const bsl::uint8_t* MemoryDecoder::end() const
{
    return d_end;
}

bsl::size_t MemoryDecoder::position() const
{
    return d_current - d_begin;
}

bsl::size_t MemoryDecoder::capacity() const
{
    return d_end - d_begin;
}

Header::Header()
: d_id(0)
, d_direction(ntcdns::Direction::e_REQUEST)
, d_operation(ntcdns::Operation::e_STANDARD)
, d_error(ntcdns::Error::e_OK)
, d_aa(false)
, d_tc(false)
, d_rd(false)
, d_ra(false)
, d_ad(false)
, d_cd(false)
, d_qdcount(0)
, d_ancount(0)
, d_nscount(0)
, d_arcount(0)
{
}

Header::Header(const Header& other)
: d_id(other.d_id)
, d_direction(other.d_direction)
, d_operation(other.d_operation)
, d_error(other.d_error)
, d_aa(other.d_aa)
, d_tc(other.d_tc)
, d_rd(other.d_rd)
, d_ra(other.d_ra)
, d_ad(other.d_ad)
, d_cd(other.d_cd)
, d_qdcount(other.d_qdcount)
, d_ancount(other.d_ancount)
, d_nscount(other.d_nscount)
, d_arcount(other.d_arcount)
{
}

Header::~Header()
{
}

Header& Header::operator=(const Header& other)
{
    if (this != &other) {
        d_id        = other.d_id;
        d_direction = other.d_direction;
        d_operation = other.d_operation;
        d_error     = other.d_error;
        d_aa        = other.d_aa;
        d_tc        = other.d_tc;
        d_rd        = other.d_rd;
        d_ra        = other.d_ra;
        d_ad        = other.d_ad;
        d_cd        = other.d_cd;
        d_qdcount   = other.d_qdcount;
        d_ancount   = other.d_ancount;
        d_nscount   = other.d_nscount;
        d_arcount   = other.d_arcount;
    }

    return *this;
}

void Header::reset()
{
    d_id        = 0;
    d_direction = ntcdns::Direction::e_REQUEST;
    d_operation = ntcdns::Operation::e_STANDARD;
    d_error     = ntcdns::Error::e_OK;
    d_aa        = false;
    d_tc        = false;
    d_rd        = false;
    d_ra        = false;
    d_ad        = false;
    d_cd        = false;
    d_qdcount   = 0;
    d_ancount   = 0;
    d_nscount   = 0;
    d_arcount   = 0;
}

void Header::setId(bsl::uint16_t value)
{
    d_id = value;
}

void Header::setDirection(ntcdns::Direction::Value value)
{
    d_direction = value;
}

void Header::setOperation(ntcdns::Operation::Value value)
{
    d_operation = value;
}

void Header::setAa(bool value)
{
    d_aa = value;
}

void Header::setTc(bool value)
{
    d_tc = value;
}

void Header::setRd(bool value)
{
    d_rd = value;
}

void Header::setRa(bool value)
{
    d_ra = value;
}

void Header::setAd(bool value)
{
    d_ad = value;
}

void Header::setCd(bool value)
{
    d_cd = value;
}

void Header::setError(ntcdns::Error::Value value)
{
    d_error = value;
}

void Header::setQdcount(bsl::size_t value)
{
    BSLS_ASSERT(value <= USHRT_MAX);

    d_qdcount = static_cast<bsl::uint16_t>(value);
}

void Header::setAncount(bsl::size_t value)
{
    BSLS_ASSERT(value <= USHRT_MAX);

    d_ancount = static_cast<bsl::uint16_t>(value);
}

void Header::setNscount(bsl::size_t value)
{
    BSLS_ASSERT(value <= USHRT_MAX);

    d_nscount = static_cast<bsl::uint16_t>(value);
}

void Header::setArcount(bsl::size_t value)
{
    BSLS_ASSERT(value <= USHRT_MAX);

    d_arcount = static_cast<bsl::uint16_t>(value);
}

ntsa::Error Header::decode(MemoryDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeUint16(&d_id);
    if (error) {
        return error;
    }

    {
        BSLMF_ASSERT(sizeof(Flags) == 2);

        Flags flags;
        error = decoder->decodeRaw(&flags, sizeof flags);
        if (error) {
            return error;
        }

        if (0 != ntcdns::Direction::fromInt(&d_direction,
                                            static_cast<int>(flags.qr)))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (0 != ntcdns::Operation::fromInt(&d_operation,
                                            static_cast<int>(flags.opcode)))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        d_aa = flags.aa;
        d_tc = flags.tc;
        d_rd = flags.rd;
        d_ra = flags.ra;
        d_cd = flags.cd;

        if (0 !=
            ntcdns::Error::fromInt(&d_error, static_cast<int>(flags.rcode)))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    error = decoder->decodeUint16(&d_qdcount);
    if (error) {
        return error;
    }

    error = decoder->decodeUint16(&d_ancount);
    if (error) {
        return error;
    }

    error = decoder->decodeUint16(&d_nscount);
    if (error) {
        return error;
    }

    error = decoder->decodeUint16(&d_arcount);
    if (error) {
        return error;
    }

    error = validate();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

bsl::uint16_t Header::id() const
{
    return d_id;
}

ntcdns::Direction::Value Header::direction() const
{
    return d_direction;
}

ntcdns::Operation::Value Header::operation() const
{
    return d_operation;
}

bool Header::aa() const
{
    return d_aa;
}

bool Header::tc() const
{
    return d_tc;
}

bool Header::rd() const
{
    return d_rd;
}

bool Header::ra() const
{
    return d_ra;
}

bool Header::ad() const
{
    return d_ad;
}

bool Header::cd() const
{
    return d_cd;
}

ntcdns::Error::Value Header::error() const
{
    return d_error;
}

bsl::size_t Header::qdcount() const
{
    return static_cast<bsl::size_t>(d_qdcount);
}

bsl::size_t Header::ancount() const
{
    return static_cast<bsl::size_t>(d_ancount);
}

bsl::size_t Header::nscount() const
{
    return static_cast<bsl::size_t>(d_nscount);
}

bsl::size_t Header::arcount() const
{
    return static_cast<bsl::size_t>(d_arcount);
}

ntsa::Error Header::encode(MemoryEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeUint16(d_id);
    if (error) {
        return error;
    }

    {
        BSLMF_ASSERT(sizeof(Flags) == 2);

        Flags flags;
        bsl::memset(&flags, 0, sizeof flags);

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

        flags.qr     = d_direction;
        flags.opcode = d_operation;
        flags.aa     = d_aa;
        flags.tc     = d_tc;
        flags.rd     = d_rd;
        flags.ra     = d_ra;
        flags.cd     = d_cd;
        flags.rcode  = d_error;

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
#pragma GCC diagnostic pop
#endif

        error = encoder->encodeRaw(&flags, sizeof flags);
        if (error) {
            return error;
        }
    }

    error = encoder->encodeUint16(d_qdcount);
    if (error) {
        return error;
    }

    error = encoder->encodeUint16(d_ancount);
    if (error) {
        return error;
    }

    error = encoder->encodeUint16(d_nscount);
    if (error) {
        return error;
    }

    error = encoder->encodeUint16(d_arcount);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

bsl::ostream& operator<<(bsl::ostream& stream, const ntcdns::Header& object)
{
    stream << "[ id = " << object.id() << " direction = " << object.direction()
           << " operation = " << object.operation()
           << " error = " << object.error() << " aa = " << object.aa()
           << " tc = " << object.tc() << " rd = " << object.rd()
           << " ra = " << object.ra() << " ad = " << object.ad()
           << " cd = " << object.cd() << " qdcount = " << object.qdcount()
           << " ancount = " << object.ancount()
           << " nscount = " << object.nscount()
           << " arcount = " << object.arcount() << " ]";

    return stream;
}

ntsa::Error Header::validate()
{
    return ntsa::Error();
}

bool operator==(const Header& lhs, const Header& rhs)
{
    return lhs.id() == rhs.id() && lhs.direction() == rhs.direction() &&
           lhs.operation() == rhs.operation() && lhs.aa() == rhs.aa() &&
           lhs.tc() == rhs.tc() && lhs.rd() == rhs.rd() &&
           lhs.ra() == rhs.ra() && lhs.ad() == rhs.ad() &&
           lhs.cd() == rhs.cd() && lhs.error() == rhs.error() &&
           lhs.qdcount() == rhs.qdcount() && lhs.ancount() == rhs.ancount() &&
           lhs.nscount() == rhs.nscount() && lhs.arcount() == rhs.arcount();
}

bool operator!=(const Header& lhs, const Header& rhs)
{
    return !operator==(lhs, rhs);
}

Question::Question(bslma::Allocator* allocator)
: d_name(allocator)
, d_type(ntcdns::Type::e_A)
, d_classification(ntcdns::Classification::e_INTERNET)
{
}

Question::Question(const Question& other, bslma::Allocator* allocator)
: d_name(other.d_name, allocator)
, d_type(other.d_type)
, d_classification(other.d_classification)
{
}

Question::~Question()
{
}

Question& Question::operator=(const Question& other)
{
    if (this != &other) {
        d_name           = other.d_name;
        d_type           = other.d_type;
        d_classification = other.d_classification;
    }

    return *this;
}

void Question::reset()
{
    d_name.clear();
    d_type           = ntcdns::Type::e_A;
    d_classification = ntcdns::Classification::e_INTERNET;
}

void Question::setName(const bsl::string& value)
{
    d_name = value;
}

void Question::setType(ntcdns::Type::Value value)
{
    d_type = value;
}

void Question::setClassification(ntcdns::Classification::Value value)
{
    d_classification = value;
}

ntsa::Error Question::decode(MemoryDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeDomainName(&d_name);
    if (error) {
        return ntsa::Error();
    }

    {
        bsl::uint16_t qtypeValue;
        error = decoder->decodeUint16(&qtypeValue);
        if (error) {
            return error;
        }

        if (0 != ntcdns::Type::fromInt(&d_type, qtypeValue)) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    {
        bsl::uint16_t qclassValue;
        error = decoder->decodeUint16(&qclassValue);
        if (error) {
            return error;
        }

        if (0 !=
            ntcdns::Classification::fromInt(&d_classification, qclassValue))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return error;
}

const bsl::string& Question::name() const
{
    return d_name;
}

ntcdns::Type::Value Question::type() const
{
    return d_type;
}

ntcdns::Classification::Value Question::classification() const
{
    return d_classification;
}

ntsa::Error Question::encode(MemoryEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeDomainName(d_name);
    if (error) {
        return error;
    }

    {
        bsl::uint16_t qtypeValue = static_cast<bsl::uint16_t>(d_type);
        error                    = encoder->encodeUint16(qtypeValue);
        if (error) {
            return error;
        }
    }

    {
        bsl::uint16_t qclassValue =
            static_cast<bsl::uint16_t>(d_classification);
        error = encoder->encodeUint16(qclassValue);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

bsl::ostream& operator<<(bsl::ostream& stream, const ntcdns::Question& object)
{
    stream << "[ name = " << object.name() << " type = " << object.type()
           << " classification = " << object.classification() << " ]";

    return stream;
}

ntsa::Error Question::validate()
{
    return ntsa::Error();
}

bool operator==(const Question& lhs, const Question& rhs)
{
    return lhs.name() == rhs.name() && lhs.type() == rhs.type() &&
           lhs.classification() == rhs.classification();
}

bool operator!=(const Question& lhs, const Question& rhs)
{
    return !operator==(lhs, rhs);
}

ResourceRecord::ResourceRecord(bslma::Allocator* allocator)
: d_name(allocator)
, d_type(ntcdns::Type::e_A)
, d_class(ntcdns::Classification::e_INTERNET)
, d_ttl(0)
, d_optSize(0)
, d_optFlags(0)
, d_rdata(allocator)
{
}

ResourceRecord::ResourceRecord(const ResourceRecord& other,
                               bslma::Allocator*     allocator)
: d_name(other.d_name, allocator)
, d_type(other.d_type)
, d_class(other.d_class)
, d_ttl(other.d_ttl)
, d_optSize(other.d_optSize)
, d_optFlags(other.d_optFlags)
, d_rdata(other.d_rdata, allocator)
{
}

ResourceRecord::~ResourceRecord()
{
}

ResourceRecord& ResourceRecord::operator=(const ResourceRecord& other)
{
    if (this != &other) {
        d_name     = other.d_name;
        d_type     = other.d_type;
        d_class    = other.d_class;
        d_ttl      = other.d_ttl;
        d_optSize  = other.d_optSize;
        d_optFlags = other.d_optFlags;
        d_rdata    = other.d_rdata;
    }

    return *this;
}

void ResourceRecord::reset()
{
    d_name.clear();
    d_type     = ntcdns::Type::e_A;
    d_class    = ntcdns::Classification::e_INTERNET;
    d_ttl      = 0;
    d_optSize  = 0;
    d_optFlags = 0;
    d_rdata.reset();
}

void ResourceRecord::setName(const bsl::string& value)
{
    d_name = value;
}

void ResourceRecord::setType(ntcdns::Type::Value value)
{
    d_type = value;
}

void ResourceRecord::setClassification(ntcdns::Classification::Value value)
{
    d_class = value;
}

void ResourceRecord::setTtl(bsl::uint32_t value)
{
    d_ttl = value;
}

void ResourceRecord::setPayloadSize(bsl::uint16_t value)
{
    d_optSize = value;
}

void ResourceRecord::setFlags(bsl::uint32_t value)
{
    d_optFlags = value;
}

void ResourceRecord::setRdata(const ntcdns::ResourceRecordData& value)
{
    d_rdata = value;

    if (value.isCanonicalNameValue()) {
        d_type = ntcdns::Type::e_CNAME;
    }
    else if (value.isHostInfoValue()) {
        d_type = ntcdns::Type::e_HINFO;
    }
    else if (value.isMailExchangeValue()) {
        d_type = ntcdns::Type::e_MX;
    }
    else if (value.isNameServerValue()) {
        d_type = ntcdns::Type::e_NS;
    }
    else if (value.isPointerValue()) {
        d_type = ntcdns::Type::e_PTR;
    }
    else if (value.isZoneAuthorityValue()) {
        d_type = ntcdns::Type::e_SOA;
    }
    else if (value.isWellKnownServiceValue()) {
        d_type = ntcdns::Type::e_WKS;
    }
    else if (value.isTextValue()) {
        d_type = ntcdns::Type::e_TXT;
    }
    else if (value.isIpv4Value()) {
        d_type = ntcdns::Type::e_A;
    }
    else if (value.isIpv6Value()) {
        d_type = ntcdns::Type::e_AAAA;
    }
    else if (value.isServerValue()) {
        d_type = ntcdns::Type::e_SVR;
    }
}

ntsa::Error ResourceRecord::decode(MemoryDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeDomainName(&d_name);
    if (error) {
        return error;
    }

    {
        bsl::uint16_t typeValue;
        error = decoder->decodeUint16(&typeValue);
        if (error) {
            return error;
        }

        if (0 != ntcdns::Type::fromInt(&d_type, static_cast<int>(typeValue))) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (d_type != ntcdns::Type::e_OPT) {
        bsl::uint16_t classificationValue;
        error = decoder->decodeUint16(&classificationValue);
        if (error) {
            return error;
        }

        if (0 != ntcdns::Classification::fromInt(
                     &d_class,
                     static_cast<int>(classificationValue)))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        d_class = ntcdns::Classification::e_INTERNET;
        error   = decoder->decodeUint16(&d_optSize);
        if (error) {
            return error;
        }
    }

    if (d_type != ntcdns::Type::e_OPT) {
        error = decoder->decodeUint32(&d_ttl);
        if (error) {
            return error;
        }
    }
    else {
        error = decoder->decodeUint32(&d_optFlags);
        if (error) {
            return error;
        }
    }

    bsl::uint16_t rdataLength;
    error = decoder->decodeUint16(&rdataLength);
    if (error) {
        return error;
    }

    if (rdataLength > 0) {
        bsl::size_t p0 = decoder->position();

        if (d_type == ntcdns::Type::e_A) {
            ntcdns::ResourceRecordDataA& rdata = d_rdata.makeIpv4();

            BSLMF_ASSERT(sizeof rdata == 4);
            error = checkExpectedRdataLength(sizeof rdata, rdataLength);
            if (error) {
                return error;
            }

            error = decoder->decodeRaw(&rdata, sizeof rdata);
            if (error) {
                return error;
            }
        }
        else if (d_type == ntcdns::Type::e_NS) {
            ntcdns::ResourceRecordDataNs& rdata = d_rdata.makeNameServer();

            error = decoder->decodeDomainName(&rdata.nsdname());
            if (error) {
                return error;
            }
        }
        else if (d_type == ntcdns::Type::e_CNAME) {
            ntcdns::ResourceRecordDataCname& rdata =
                d_rdata.makeCanonicalName();

            error = decoder->decodeDomainName(&rdata.cname());
            if (error) {
                return error;
            }
        }
        else if (d_type == ntcdns::Type::e_SOA) {
            ntcdns::ResourceRecordDataSoa& rdata = d_rdata.makeZoneAuthority();

            error = decoder->decodeDomainName(&rdata.mname());
            if (error) {
                return error;
            }

            error = decoder->decodeDomainName(&rdata.rname());
            if (error) {
                return error;
            }

            error = decoder->decodeUint32(&rdata.serial());
            if (error) {
                return error;
            }

            error = decoder->decodeUint32(&rdata.refresh());
            if (error) {
                return error;
            }

            error = decoder->decodeUint32(&rdata.retry());
            if (error) {
                return error;
            }

            error = decoder->decodeUint32(&rdata.expire());
            if (error) {
                return error;
            }

            error = decoder->decodeUint32(&rdata.minimum());
            if (error) {
                return error;
            }
        }
        else if (d_type == ntcdns::Type::e_WKS) {
            ntcdns::ResourceRecordDataWks& rdata =
                d_rdata.makeWellKnownService();

            error = decoder->decodeUint32(&rdata.address());
            if (error) {
                return error;
            }

            error = decoder->decodeUint8(&rdata.protocol());
            if (error) {
                return error;
            }

            if (rdataLength > rdata.address() + rdata.protocol()) {
                bsl::size_t bitsetSize =
                    rdataLength - (rdata.address() + rdata.protocol());

                bsl::vector<bsl::uint8_t> bitset(bitsetSize);

                error = decoder->decodeRaw(&bitset[0], bitsetSize);
                if (error) {
                    return error;
                }

                for (bsl::size_t i = 0; i < bitsetSize; ++i) {
                    for (bsl::size_t j = 0; i < 8; ++i) {
                        if ((bitset[i] & (1 << j)) != 0) {
                            unsigned short portNumber =
                                NTCCFG_WARNING_NARROW(unsigned short,
                                                      (i * 8) + j);
                            rdata.port().push_back(portNumber);
                        }
                    }
                }
            }
        }
        else if (d_type == ntcdns::Type::e_PTR) {
            ntcdns::ResourceRecordDataPtr& rdata = d_rdata.makePointer();

            error = decoder->decodeDomainName(&rdata.ptrdname());
            if (error) {
                return error;
            }
        }
        else if (d_type == ntcdns::Type::e_HINFO) {
            ntcdns::ResourceRecordDataHinfo& rdata = d_rdata.makeHostInfo();

            error = decoder->decodeCharacterString(&rdata.cpu());
            if (error) {
                return error;
            }

            error = decoder->decodeCharacterString(&rdata.os());
            if (error) {
                return error;
            }
        }
        else if (d_type == ntcdns::Type::e_MX) {
            ntcdns::ResourceRecordDataMx& rdata = d_rdata.makeMailExchange();

            error = decoder->decodeUint16(&rdata.preference());
            if (error) {
                return error;
            }

            error = decoder->decodeDomainName(&rdata.exchange());
            if (error) {
                return error;
            }
        }
        else if (d_type == ntcdns::Type::e_TXT) {
            ntcdns::ResourceRecordDataTxt& rdata = d_rdata.makeText();

            bsl::size_t numBytesRemaining = rdataLength;

            while (numBytesRemaining > 0) {
                bsl::size_t s0 = decoder->position();

                rdata.text().resize(rdata.text().size() + 1);
                error = decoder->decodeCharacterString(&rdata.text().back());
                if (error) {
                    return error;
                }

                bsl::size_t s1 = decoder->position();

                bsl::size_t numBytesRead = s1 - s0;
                if (numBytesRead >= numBytesRemaining) {
                    numBytesRemaining -= numBytesRead;
                }
                else {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }
            }
        }
        else if (d_type == ntcdns::Type::e_AAAA) {
            ntcdns::ResourceRecordDataAAAA& rdata = d_rdata.makeIpv6();

            BSLMF_ASSERT(sizeof rdata == 16);
            error = checkExpectedRdataLength(sizeof rdata, rdataLength);
            if (error) {
                return error;
            }

            error = decoder->decodeRaw(&rdata, sizeof rdata);
            if (error) {
                return error;
            }
        }
        else if (d_type == ntcdns::Type::e_SVR) {
            return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
        }
        else if (d_type == ntcdns::Type::e_OPT) {
            // Parse options as raw records until the format is supported.

            ntcdns::ResourceRecordDataRaw& rdata = d_rdata.makeRaw();

            rdata.data().resize(rdataLength);
            error = decoder->decodeRaw(&rdata.data()[0], rdata.data().size());
            if (error) {
                return error;
            }
        }
        else {
            ntcdns::ResourceRecordDataRaw& rdata = d_rdata.makeRaw();

            rdata.data().resize(rdataLength);
            error = decoder->decodeRaw(&rdata.data()[0], rdata.data().size());
            if (error) {
                return error;
            }
        }

        bsl::size_t p1 = decoder->position();

        error = checkCoherentRdataLength(rdataLength, p1 - p0);
        if (error) {
            return error;
        }
    }

    error = this->validate();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

const bsl::string& ResourceRecord::name() const
{
    return d_name;
}

ntcdns::Type::Value ResourceRecord::type() const
{
    return d_type;
}

ntcdns::Classification::Value ResourceRecord::classification() const
{
    return d_class;
}

bsl::uint32_t ResourceRecord::ttl() const
{
    return d_ttl;
}

bsl::uint16_t ResourceRecord::payloadSize() const
{
    return d_optSize;
}

bsl::uint32_t ResourceRecord::flags() const
{
    return d_optFlags;
}

const ntcdns::ResourceRecordData& ResourceRecord::rdata() const
{
    return d_rdata;
}

ntsa::Error ResourceRecord::encode(MemoryEncoder* encoder) const
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    error = encoder->encodeDomainName(d_name);
    if (error) {
        return error;
    }

    {
        bsl::uint16_t typeValue = static_cast<bsl::uint16_t>(d_type);
        error                   = encoder->encodeUint16(typeValue);
        if (error) {
            return error;
        }
    }

    if (d_type != ntcdns::Type::e_OPT) {
        {
            bsl::uint16_t classValue = static_cast<bsl::uint16_t>(d_class);
            error                    = encoder->encodeUint16(classValue);
            if (error) {
                return error;
            }
        }

        error = encoder->encodeUint32(d_ttl);
        if (error) {
            return error;
        }
    }
    else {
        error = encoder->encodeUint16(d_optSize);
        if (error) {
            return error;
        }

        error = encoder->encodeUint32(d_optFlags);
        if (error) {
            return error;
        }
    }

    bsl::size_t p0 = encoder->position();

    bsl::uint16_t rdataLength = 0;
    error                     = encoder->encodeUint16(rdataLength);
    if (error) {
        return error;
    }

    bsl::size_t p1 = encoder->position();

    if (d_rdata.isIpv4Value()) {
        BSLMF_ASSERT(sizeof d_rdata.ipv4() == 4);

        error = encoder->encodeRaw(&d_rdata.ipv4(), sizeof d_rdata.ipv4());
        if (error) {
            return error;
        }
    }
    else if (d_rdata.isIpv6Value()) {
        BSLMF_ASSERT(sizeof d_rdata.ipv6() == 16);

        error = encoder->encodeRaw(&d_rdata.ipv6(), sizeof d_rdata.ipv6());
        if (error) {
            return error;
        }
    }
    else if (d_rdata.isNameServerValue()) {
        error = encoder->encodeDomainName(d_rdata.nameServer().nsdname());
        if (error) {
            return error;
        }
    }
    else if (d_rdata.isCanonicalNameValue()) {
        error = encoder->encodeDomainName(d_rdata.canonicalName().cname());
        if (error) {
            return error;
        }
    }
    else if (d_rdata.isZoneAuthorityValue()) {
        error = encoder->encodeDomainName(d_rdata.zoneAuthority().mname());
        if (error) {
            return error;
        }

        error = encoder->encodeDomainName(d_rdata.zoneAuthority().rname());
        if (error) {
            return error;
        }

        error = encoder->encodeUint32(d_rdata.zoneAuthority().serial());
        if (error) {
            return error;
        }

        error = encoder->encodeUint32(d_rdata.zoneAuthority().refresh());
        if (error) {
            return error;
        }

        error = encoder->encodeUint32(d_rdata.zoneAuthority().retry());
        if (error) {
            return error;
        }

        error = encoder->encodeUint32(d_rdata.zoneAuthority().expire());
        if (error) {
            return error;
        }

        error = encoder->encodeUint32(d_rdata.zoneAuthority().minimum());
        if (error) {
            return error;
        }
    }
    else if (d_rdata.isWellKnownServiceValue()) {
        error = encoder->encodeUint32(d_rdata.wellKnownService().address());
        if (error) {
            return error;
        }

        error = encoder->encodeUint8(d_rdata.wellKnownService().protocol());
        if (error) {
            return error;
        }

        if (!d_rdata.wellKnownService().port().empty()) {
            bsl::vector<unsigned short> portVector =
                d_rdata.wellKnownService().port();

            bsl::sort(portVector.begin(), portVector.end());

            bsl::size_t maxPort = portVector.back();

            bsl::size_t bitsetSize = (maxPort / 8) + 1;

            bsl::vector<bsl::uint8_t> bitset(bitsetSize);

            for (bsl::size_t i = 0; i < portVector.size(); ++i) {
                bsl::size_t portNumber = portVector[i];
                bitset[portNumber / 8] &=
                    NTCCFG_WARNING_NARROW(unsigned char,
                                          (1 << (portNumber % 8)));
            }

            error = encoder->encodeRaw(&bitset[0], bitset.size());
            if (error) {
                return error;
            }
        }
    }
    else if (d_rdata.isPointerValue()) {
        error = encoder->encodeDomainName(d_rdata.pointer().ptrdname());
        if (error) {
            return error;
        }
    }
    else if (d_rdata.isHostInfoValue()) {
        error = encoder->encodeCharacterString(d_rdata.hostInfo().cpu());
        if (error) {
            return error;
        }

        error = encoder->encodeCharacterString(d_rdata.hostInfo().os());
        if (error) {
            return error;
        }
    }
    else if (d_rdata.isMailExchangeValue()) {
        error = encoder->encodeUint16(d_rdata.mailExchange().preference());
        if (error) {
            return error;
        }

        error = encoder->encodeDomainName(d_rdata.mailExchange().exchange());
        if (error) {
            return error;
        }
    }
    else if (d_rdata.isTextValue()) {
        for (bsl::vector<bsl::string>::const_iterator it =
                 d_rdata.text().text().begin();
             it != d_rdata.text().text().end();
             ++it)
        {
            error = encoder->encodeCharacterString(*it);
            if (error) {
                return error;
            }
        }
    }
    else if (d_rdata.isServerValue()) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }
    else if (d_rdata.isRawValue()) {
        error = encoder->encodeRaw(&d_rdata.raw().data()[0],
                                   d_rdata.raw().data().size());
        if (error) {
            return error;
        }
    }
    else if (d_rdata.isUndefinedValue()) {
        // no-op
    }
    else {
        NTCI_LOG_STREAM_ERROR << "Unsupported rdata type: " << d_rdata
                              << NTCI_LOG_STREAM_END;
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::size_t p2 = encoder->position();

    rdataLength = NTCCFG_WARNING_NARROW(bsl::uint16_t, p2 - p1);

    error = encoder->seek(p0);
    if (error) {
        return error;
    }

    error = encoder->encodeUint16(rdataLength);
    if (error) {
        return error;
    }

    error = encoder->seek(p2);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const ntcdns::ResourceRecord& object)
{
    stream << "[ name = " << object.name() << " type = " << object.type();

    if (object.type() != ntcdns::Type::e_OPT) {
        stream << " classification = " << object.classification()
               << " ttl = " << object.ttl();
    }
    else {
        stream << " payloadSize = " << object.d_optSize
               << " flags = " << object.d_optFlags;
    }

    if (!object.rdata().isUndefinedValue()) {
        stream << " rdata = ";

        if (object.rdata().isIpv4Value()) {
            BSLMF_ASSERT(sizeof object.rdata().ipv4() == 4);
            ntsa::Ipv4Address ipv4Address;
            ipv4Address.copyFrom(&object.rdata().ipv4(),
                                 sizeof object.rdata().ipv4());
            stream << ipv4Address;
        }
        else if (object.rdata().isIpv6Value()) {
            BSLMF_ASSERT(sizeof object.rdata().ipv6() == 16);
            ntsa::Ipv6Address ipv6Address;
            ipv6Address.copyFrom(&object.rdata().ipv6(),
                                 sizeof object.rdata().ipv6());
            stream << ipv6Address;
        }
        else {
            stream << object.rdata();
        }
    }

    stream << " ]";

    return stream;
}

ntsa::Error ResourceRecord::validate()
{
    return ntsa::Error();
}

bool operator==(const ResourceRecord& lhs, const ResourceRecord& rhs)
{
    return lhs.name() == rhs.name() && lhs.type() == rhs.type() &&
           lhs.classification() == rhs.classification() &&
           lhs.ttl() == rhs.ttl() && lhs.rdata() == rhs.rdata();
}

bool operator!=(const ResourceRecord& lhs, const ResourceRecord& rhs)
{
    return !operator==(lhs, rhs);
}

Message::Message(bslma::Allocator* allocator)
: d_header()
, d_qd(allocator)
, d_an(allocator)
, d_ns(allocator)
, d_ar(allocator)
{
}

Message::Message(const Message& other, bslma::Allocator* allocator)
: d_header(other.d_header)
, d_qd(other.d_qd, allocator)
, d_an(other.d_an, allocator)
, d_ns(other.d_ns, allocator)
, d_ar(other.d_ar, allocator)
{
}

Message::~Message()
{
}

Message& Message::operator=(const Message& other)
{
    if (this != &other) {
        d_header = other.d_header;
        d_qd     = other.d_qd;
        d_an     = other.d_an;
        d_ns     = other.d_ns;
        d_ar     = other.d_ar;
    }

    return *this;
}

void Message::reset()
{
    d_header.reset();
    d_qd.clear();
    d_an.clear();
    d_ns.clear();
    d_ar.clear();
}

void Message::setId(bsl::uint16_t value)
{
    d_header.setId(value);
}

void Message::setDirection(ntcdns::Direction::Value value)
{
    d_header.setDirection(value);
}

void Message::setOperation(ntcdns::Operation::Value value)
{
    d_header.setOperation(value);
}

void Message::setAa(bool value)
{
    d_header.setAa(value);
}

void Message::setTc(bool value)
{
    d_header.setTc(value);
}

void Message::setRd(bool value)
{
    d_header.setRd(value);
}

void Message::setRa(bool value)
{
    d_header.setRa(value);
}

void Message::setAd(bool value)
{
    d_header.setAd(value);
}

void Message::setCd(bool value)
{
    d_header.setCd(value);
}

void Message::setError(ntcdns::Error::Value value)
{
    d_header.setError(value);
}

ntcdns::Question& Message::addQd()
{
    d_qd.resize(d_qd.size() + 1);
    ntcdns::Question& result = d_qd.back();
    d_header.setQdcount(d_qd.size());
    return result;
}

ntcdns::Question& Message::addQd(const ntcdns::Question& qd)
{
    d_qd.resize(d_qd.size() + 1);
    ntcdns::Question& result = d_qd.back();
    result                   = qd;
    d_header.setQdcount(d_qd.size());
    return result;
}

ntcdns::ResourceRecord& Message::addAn()
{
    d_an.resize(d_an.size() + 1);
    ntcdns::ResourceRecord& result = d_an.back();
    d_header.setAncount(d_an.size());
    return result;
}

ntcdns::ResourceRecord& Message::addAn(const ntcdns::ResourceRecord& an)
{
    d_an.resize(d_an.size() + 1);
    ntcdns::ResourceRecord& result = d_an.back();
    result                         = an;
    d_header.setAncount(d_an.size());
    return result;
}

ntcdns::ResourceRecord& Message::addNs()
{
    d_ns.resize(d_ns.size() + 1);
    ntcdns::ResourceRecord& result = d_ns.back();
    d_header.setNscount(d_ns.size());
    return result;
}

ntcdns::ResourceRecord& Message::addNs(const ntcdns::ResourceRecord& ns)
{
    d_ns.resize(d_ns.size() + 1);
    ntcdns::ResourceRecord& result = d_ns.back();
    result                         = ns;
    d_header.setNscount(d_ns.size());
    return result;
}

ntcdns::ResourceRecord& Message::addAr()
{
    d_ar.resize(d_ar.size() + 1);
    ntcdns::ResourceRecord& result = d_ar.back();
    d_header.setArcount(d_ar.size());
    return result;
}

ntcdns::ResourceRecord& Message::addAr(const ntcdns::ResourceRecord& ar)
{
    d_ar.resize(d_ar.size() + 1);
    ntcdns::ResourceRecord& result = d_ar.back();
    result                         = ar;
    d_header.setArcount(d_ar.size());
    return result;
}

ntsa::Error Message::decode(MemoryDecoder* decoder)
{
    ntsa::Error error;

    error = d_header.decode(decoder);
    if (error) {
        return error;
    }

    bsl::size_t numQdRecords = d_header.qdcount();
    if (numQdRecords > 0) {
        d_qd.resize(numQdRecords);
        for (bsl::size_t i = 0; i < numQdRecords; ++i) {
            error = d_qd[i].decode(decoder);
            if (error) {
                return error;
            }
        }
    }

    bsl::size_t numAnRecords = d_header.ancount();
    if (numAnRecords > 0) {
        d_an.resize(numAnRecords);
        for (bsl::size_t i = 0; i < numAnRecords; ++i) {
            error = d_an[i].decode(decoder);
            if (error) {
                return error;
            }
        }
    }

    bsl::size_t numNsRecords = d_header.nscount();
    if (numNsRecords > 0) {
        d_ns.resize(numNsRecords);
        for (bsl::size_t i = 0; i < numNsRecords; ++i) {
            error = d_ns[i].decode(decoder);
            if (error) {
                return error;
            }
        }
    }

    bsl::size_t numArRecords = d_header.arcount();
    if (numArRecords > 0) {
        d_ar.resize(numArRecords);
        for (bsl::size_t i = 0; i < numArRecords; ++i) {
            error = d_ar[i].decode(decoder);
            if (error) {
                return error;
            }
        }
    }

    return ntsa::Error();
}

bsl::uint16_t Message::id() const
{
    return d_header.id();
}

ntcdns::Direction::Value Message::direction() const
{
    return d_header.direction();
}

ntcdns::Operation::Value Message::operation() const
{
    return d_header.operation();
}

bool Message::aa() const
{
    return d_header.aa();
}

bool Message::tc() const
{
    return d_header.tc();
}

bool Message::rd() const
{
    return d_header.rd();
}

bool Message::ra() const
{
    return d_header.ra();
}

bool Message::ad() const
{
    return d_header.ad();
}

bool Message::cd() const
{
    return d_header.cd();
}

ntcdns::Error::Value Message::error() const
{
    return d_header.error();
}

bsl::size_t Message::qdcount() const
{
    return d_header.qdcount();
}

bsl::size_t Message::ancount() const
{
    return d_header.ancount();
}

bsl::size_t Message::nscount() const
{
    return d_header.nscount();
}

bsl::size_t Message::arcount() const
{
    return d_header.arcount();
}

const ntcdns::Question& Message::qd(bsl::size_t index) const
{
    BSLS_ASSERT(index < d_header.qdcount());
    return d_qd[index];
}

const ntcdns::ResourceRecord& Message::an(bsl::size_t index) const
{
    BSLS_ASSERT(index < d_header.ancount());
    return d_an[index];
}

const ntcdns::ResourceRecord& Message::ns(bsl::size_t index) const
{
    BSLS_ASSERT(index < d_header.nscount());
    return d_ns[index];
}

const ntcdns::ResourceRecord& Message::ar(bsl::size_t index) const
{
    BSLS_ASSERT(index < d_header.arcount());
    return d_ar[index];
}

ntsa::Error Message::encode(MemoryEncoder* encoder) const
{
    ntsa::Error error;

    error = d_header.encode(encoder);
    if (error) {
        return error;
    }

    bsl::size_t numQdRecords = d_header.qdcount();
    for (bsl::size_t i = 0; i < numQdRecords; ++i) {
        error = d_qd[i].encode(encoder);
        if (error) {
            return error;
        }
    }

    bsl::size_t numAnRecords = d_header.ancount();
    for (bsl::size_t i = 0; i < numAnRecords; ++i) {
        error = d_an[i].encode(encoder);
        if (error) {
            return error;
        }
    }

    bsl::size_t numNsRecords = d_header.nscount();
    for (bsl::size_t i = 0; i < numNsRecords; ++i) {
        error = d_ns[i].encode(encoder);
        if (error) {
            return error;
        }
    }

    bsl::size_t numArRecords = d_header.arcount();
    for (bsl::size_t i = 0; i < numArRecords; ++i) {
        error = d_ar[i].encode(encoder);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

bsl::ostream& operator<<(bsl::ostream& stream, const ntcdns::Message& object)
{
    stream << "[ header = " << object.d_header;

    bsl::size_t numQdRecords = object.d_header.qdcount();
    if (numQdRecords > 0) {
        stream << " qd = [";
        for (bsl::size_t i = 0; i < numQdRecords; ++i) {
            stream << ' ';
            stream << object.d_qd[i];
        }
        stream << " ]";
    }

    bsl::size_t numAnRecords = object.d_header.ancount();
    if (numAnRecords > 0) {
        stream << " an = [";
        for (bsl::size_t i = 0; i < numAnRecords; ++i) {
            stream << ' ';
            stream << object.d_an[i];
        }
        stream << " ]";
    }

    bsl::size_t numNsRecords = object.d_header.nscount();
    if (numNsRecords > 0) {
        stream << " ns = [";
        for (bsl::size_t i = 0; i < numNsRecords; ++i) {
            stream << ' ';
            stream << object.d_ns[i];
        }
        stream << " ]";
    }

    bsl::size_t numArRecords = object.d_header.arcount();
    if (numArRecords > 0) {
        stream << " ar = [";
        for (bsl::size_t i = 0; i < numArRecords; ++i) {
            stream << ' ';
            stream << object.d_ar[i];
        }
        stream << " ]";
    }

    stream << " ]";

    return stream;
}

bool operator==(const Message& lhs, const Message& rhs)
{
    return (lhs.d_header == rhs.d_header && lhs.d_qd == rhs.d_qd &&
            lhs.d_an == rhs.d_an && lhs.d_ns == rhs.d_ns &&
            lhs.d_ar == rhs.d_ar);
}

bool operator!=(const Message& lhs, const Message& rhs)
{
    return !operator==(lhs, rhs);
}

}  // close package namespace
}  // close enterprise namespace
