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

#include <ntca_checksum.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_checksum_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bslmf_assert.h>

#if NTC_BUILD_WITH_LZ4
#include <lz4.h>
#include <lz4frame.h>
#endif

#if NTC_BUILD_WITH_ZSTD
#include <zstd.h>
#include <zstd_errors.h>
#endif

#if NTC_BUILD_WITH_ZLIB
#include <zlib.h>
#endif

namespace BloombergLP {
namespace ntca {

ChecksumAdler32::ChecksumAdler32()
: d_value(1)
{
}

ChecksumAdler32::ChecksumAdler32(Digest digest)
: d_value(digest)
{
}

ChecksumAdler32::ChecksumAdler32(const ChecksumAdler32& original)
: d_value(original.d_value)
{
}

ChecksumAdler32::ChecksumAdler32(bslmf::MovableRef<ChecksumAdler32> original)
    NTSCFG_NOEXCEPT
{
    d_value = NTSCFG_MOVE_FROM(original, d_value);
    NTSCFG_MOVE_RESET(original);
}

ChecksumAdler32::~ChecksumAdler32()
{
}

ChecksumAdler32& ChecksumAdler32::operator=(const ChecksumAdler32& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

ChecksumAdler32& ChecksumAdler32::operator=(
    bslmf::MovableRef<ChecksumAdler32> other) NTSCFG_NOEXCEPT
{
    d_value = NTSCFG_MOVE_FROM(other, d_value);
    NTSCFG_MOVE_RESET(other);

    return *this;
}

void ChecksumAdler32::reset()
{
    d_value = 1;
}

ntsa::Error ChecksumAdler32::update(const void* data, bsl::size_t size)
{
    const bsl::uint8_t* current = reinterpret_cast<const bsl::uint8_t*>(data);
    const bsl::uint8_t* end     = current + size;

    bsl::uint32_t s1 = d_value & 0xffff;
    bsl::uint32_t s2 = (d_value >> 16) & 0xffff;

    while (current < end) {
        const bsl::uint8_t byte = *current++;
        s1                      = (s1 + byte) % 65521;
        s2                      = (s2 + s1) % 65521;
    }

    d_value = (s2 << 16) + s1;

    return ntsa::Error();
}

ntsa::Error ChecksumAdler32::update(const bdlbb::Blob& data)
{
    return this->update(data, data.length());
}

ntsa::Error ChecksumAdler32::update(const bdlbb::Blob& data, bsl::size_t size)
{
    if (size == 0) {
        return ntsa::Error();
    }

    bsl::size_t numBytesRemaining = size;

    const int numDataBuffers = data.numDataBuffers();

    bool done = false;

    for (int i = 0; i < numDataBuffers; ++i) {
        const bdlbb::BlobBuffer& buffer = data.buffer(i);

        const bsl::uint8_t* bufferData =
            reinterpret_cast<bsl::uint8_t*>(buffer.data());

        bsl::size_t bufferSize;
        if (i != numDataBuffers - 1) {
            bufferSize = static_cast<bsl::size_t>(buffer.size());
        }
        else {
            bufferSize = static_cast<bsl::size_t>(data.lastDataBufferLength());
        }

        bsl::size_t numBytesToUpdate;
        if (bufferSize < numBytesRemaining) {
            numBytesToUpdate   = bufferSize;
            numBytesRemaining -= bufferSize;
        }
        else {
            numBytesToUpdate  = numBytesRemaining;
            numBytesRemaining = 0;
            done              = true;
        }

        this->update(bufferData, numBytesToUpdate);

        if (done) {
            break;
        }
    }

    return ntsa::Error();
}

bsl::uint32_t ChecksumAdler32::value() const
{
    return d_value;
}

bool ChecksumAdler32::equals(const ChecksumAdler32& other) const
{
    return d_value == other.d_value;
}

bool ChecksumAdler32::less(const ChecksumAdler32& other) const
{
    return d_value < other.d_value;
}

bsl::ostream& ChecksumAdler32::print(bsl::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel) const
{
    NTCCFG_WARNING_UNUSED(level);
    NTCCFG_WARNING_UNUSED(spacesPerLevel);

    return stream << d_value;
}

ChecksumCrc32::ChecksumCrc32()
: d_value(0)
{
}

ChecksumCrc32::ChecksumCrc32(Digest digest)
: d_value(digest)
{
}

ChecksumCrc32::ChecksumCrc32(const ChecksumCrc32& original)
: d_value(original.d_value)
{
}

ChecksumCrc32::ChecksumCrc32(bslmf::MovableRef<ChecksumCrc32> original)
    NTSCFG_NOEXCEPT
{
    d_value = NTSCFG_MOVE_FROM(original, d_value);
    NTSCFG_MOVE_RESET(original);
}

ChecksumCrc32::~ChecksumCrc32()
{
}

ChecksumCrc32& ChecksumCrc32::operator=(const ChecksumCrc32& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

ChecksumCrc32& ChecksumCrc32::operator=(bslmf::MovableRef<ChecksumCrc32> other)
    NTSCFG_NOEXCEPT
{
    d_value = NTSCFG_MOVE_FROM(other, d_value);
    NTSCFG_MOVE_RESET(other);

    return *this;
}

void ChecksumCrc32::reset()
{
    d_value = 0;
}

ntsa::Error ChecksumCrc32::update(const void* data, bsl::size_t size)
{
    const bsl::uint8_t* current = reinterpret_cast<const bsl::uint8_t*>(data);
    const bsl::uint8_t* end     = current + size;

    bsl::uint32_t result = ~d_value;

    while (current < end) {
        const bsl::uint8_t byte = *current++;

        result = result ^ byte;

        for (bsl::size_t i = 0; i < 8; ++i) {
            if ((result & 1) != 0) {
                result = (result >> 1) ^ 0xEDB88320;
            }
            else {
                result = (result >> 1);
            }
        }
    }

    d_value = ~result;

    return ntsa::Error();
}

ntsa::Error ChecksumCrc32::update(const bdlbb::Blob& data)
{
    return this->update(data, data.length());
}

ntsa::Error ChecksumCrc32::update(const bdlbb::Blob& data, bsl::size_t size)
{
    if (size == 0) {
        return ntsa::Error();
    }

    bsl::size_t numBytesRemaining = size;

    const int numDataBuffers = data.numDataBuffers();

    bool done = false;

    for (int i = 0; i < numDataBuffers; ++i) {
        const bdlbb::BlobBuffer& buffer = data.buffer(i);

        const bsl::uint8_t* bufferData =
            reinterpret_cast<bsl::uint8_t*>(buffer.data());

        bsl::size_t bufferSize;
        if (i != numDataBuffers - 1) {
            bufferSize = static_cast<bsl::size_t>(buffer.size());
        }
        else {
            bufferSize = static_cast<bsl::size_t>(data.lastDataBufferLength());
        }

        bsl::size_t numBytesToUpdate;
        if (bufferSize < numBytesRemaining) {
            numBytesToUpdate   = bufferSize;
            numBytesRemaining -= bufferSize;
        }
        else {
            numBytesToUpdate  = numBytesRemaining;
            numBytesRemaining = 0;
            done              = true;
        }

        this->update(bufferData, numBytesToUpdate);

        if (done) {
            break;
        }
    }

    return ntsa::Error();
}

bsl::uint32_t ChecksumCrc32::value() const
{
    return d_value;
}

bool ChecksumCrc32::equals(const ChecksumCrc32& other) const
{
    return d_value == other.d_value;
}

bool ChecksumCrc32::less(const ChecksumCrc32& other) const
{
    return d_value < other.d_value;
}

bsl::ostream& ChecksumCrc32::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    NTCCFG_WARNING_UNUSED(level);
    NTCCFG_WARNING_UNUSED(spacesPerLevel);

    return stream << d_value;
}

bsl::uint32_t ChecksumXxHash32::decode(const bsl::uint8_t* data,
                                       bsl::size_t         offset)
{
    const bsl::uint8_t* source = data + offset;

    const bsl::uint32_t byte0 = static_cast<bsl::uint32_t>(*source++);
    const bsl::uint32_t byte1 = static_cast<bsl::uint32_t>(*source++);
    const bsl::uint32_t byte2 = static_cast<bsl::uint32_t>(*source++);
    const bsl::uint32_t byte3 = static_cast<bsl::uint32_t>(*source);

    return byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24);
}

bsl::uint32_t ChecksumXxHash32::rotate(bsl::uint32_t value,
                                       bsl::uint32_t amount)
{
    return (value << (amount % 32)) | (value >> (32 - amount % 32));
}

ChecksumXxHash32::ChecksumXxHash32()
{
    BSLMF_ASSERT(sizeof(ChecksumXxHash32) == 44);
    this->reset();
}

ChecksumXxHash32::ChecksumXxHash32(Digest digest)
{
    this->reset(digest);
}

ChecksumXxHash32::ChecksumXxHash32(const ChecksumXxHash32& original)
{
    bsl::memcpy(this, &original, sizeof *this);
}

ChecksumXxHash32::ChecksumXxHash32(
    bslmf::MovableRef<ChecksumXxHash32> original) NTSCFG_NOEXCEPT
{
    bsl::memcpy(this, &original, sizeof *this);
    NTSCFG_MOVE_RESET(original);
}

ChecksumXxHash32::~ChecksumXxHash32()
{
}

ChecksumXxHash32& ChecksumXxHash32::operator=(const ChecksumXxHash32& other)
{
    if (this != &other) {
        bsl::memcpy(this, &other, sizeof *this);
    }

    return *this;
}

ChecksumXxHash32& ChecksumXxHash32::operator=(
    bslmf::MovableRef<ChecksumXxHash32> other) NTSCFG_NOEXCEPT
{
    bsl::memcpy(this, &other, sizeof *this);
    NTSCFG_MOVE_RESET(other);

    return *this;
}

void ChecksumXxHash32::reset()
{
    d_accumulator[0] = Self::k_SEED + Self::k_P1 + Self::k_P2;
    d_accumulator[1] = Self::k_SEED + Self::k_P2;
    d_accumulator[2] = Self::k_SEED + 0;
    d_accumulator[3] = Self::k_SEED - Self::k_P1;

    bsl::memset(d_buffer, 0, sizeof d_buffer);

    d_bufferSize = 0;
    d_entireSize = 0;
    d_full       = 0;
}

void ChecksumXxHash32::reset(Digest digest)
{
    d_accumulator[0] = digest + Self::k_P1 + Self::k_P2;
    d_accumulator[1] = digest + Self::k_P2;
    d_accumulator[2] = digest + 0;
    d_accumulator[3] = digest - Self::k_P1;

    bsl::memset(d_buffer, 0, sizeof d_buffer);

    d_bufferSize = 0;
    d_entireSize = 0;
    d_full       = 0;
}

ntsa::Error ChecksumXxHash32::update(const void* data, bsl::size_t size)
{
    const bsl::uint8_t* source = reinterpret_cast<const bsl::uint8_t*>(data);

    d_entireSize += static_cast<bsl::uint32_t>(size);

    if (d_full == 0 && (size >= 16 || d_entireSize >= 16)) {
        d_full = 1;
    }

    if (d_bufferSize + size < 16) {
        bsl::memcpy(&d_buffer[d_bufferSize], source, size);
        d_bufferSize += static_cast<bsl::uint32_t>(size);
        return ntsa::Error();
    }

    bsl::size_t offset    = 0;
    bsl::size_t remaining = d_bufferSize + size;

    while (remaining >= 16) {
        bsl::memcpy(&d_buffer[d_bufferSize],
                    &source[offset],
                    16 - d_bufferSize);

        for (bsl::size_t accumulator = 0; accumulator < 4; ++accumulator) {
            for (bsl::size_t offset = 0; offset < 16; offset += 16) {
                const bsl::uint32_t next = 
                    ChecksumXxHash32::decode(d_buffer, offset);

                bsl::uint32_t value = d_accumulator[accumulator];

                value += next * Self::k_P2;
                value  = Self::rotate(value, 13);
                value *= Self::k_P1;

                d_accumulator[accumulator] = value;
            }
        }

        offset       += 16 - d_bufferSize;
        remaining    -= 16;
        d_bufferSize  = 0;
    }

    if (remaining != 0) {
        memcpy(d_buffer, &source[offset], remaining);
        d_bufferSize = static_cast<bsl::uint32_t>(remaining);
    }

    return ntsa::Error();
}

ntsa::Error ChecksumXxHash32::update(const bdlbb::Blob& data)
{
    return this->update(data, data.length());
}

ntsa::Error ChecksumXxHash32::update(const bdlbb::Blob& data, bsl::size_t size)
{
    if (size == 0) {
        return ntsa::Error();
    }

    bsl::size_t numBytesRemaining = size;

    const int numDataBuffers = data.numDataBuffers();

    bool done = false;

    for (int i = 0; i < numDataBuffers; ++i) {
        const bdlbb::BlobBuffer& buffer = data.buffer(i);

        const bsl::uint8_t* bufferData =
            reinterpret_cast<bsl::uint8_t*>(buffer.data());

        bsl::size_t bufferSize;
        if (i != numDataBuffers - 1) {
            bufferSize = static_cast<bsl::size_t>(buffer.size());
        }
        else {
            bufferSize = static_cast<bsl::size_t>(data.lastDataBufferLength());
        }

        bsl::size_t numBytesToUpdate;
        if (bufferSize < numBytesRemaining) {
            numBytesToUpdate   = bufferSize;
            numBytesRemaining -= bufferSize;
        }
        else {
            numBytesToUpdate  = numBytesRemaining;
            numBytesRemaining = 0;
            done              = true;
        }

        this->update(bufferData, numBytesToUpdate);

        if (done) {
            break;
        }
    }

    return ntsa::Error();
}

bsl::uint32_t ChecksumXxHash32::value() const
{
    bsl::uint32_t result = 0;

    if (d_full == 1) {
        result = Self::rotate(d_accumulator[0], 1) +
                 Self::rotate(d_accumulator[1], 7) +
                 Self::rotate(d_accumulator[2], 12) +
                 Self::rotate(d_accumulator[3], 18);
    }
    else {
        result = d_accumulator[2] + Self::k_P5;
    }

    result += d_entireSize;

    bsl::uint32_t offset = 0;
    bsl::uint32_t length = d_bufferSize;

    while (length >= sizeof(bsl::uint32_t)) {
        result += Self::decode(d_buffer, offset) * Self::k_P3;
        result  = Self::rotate(result, 17);
        result *= Self::k_P4;
        offset += sizeof(bsl::uint32_t);
        length -= sizeof(bsl::uint32_t);
    }

    while (length != 0) {
        result += static_cast<uint32_t>(d_buffer[offset]) * Self::k_P5;
        result  = Self::rotate(result, 11);
        result *= Self::k_P1;

        --length;
        ++offset;
    }

    result ^= result >> 15;
    result *= Self::k_P2;
    result ^= result >> 13;
    result *= Self::k_P3;
    result ^= result >> 16;

    return result;
}

bool ChecksumXxHash32::equals(const ChecksumXxHash32& other) const
{
    return bsl::memcmp(this, &other, sizeof *this) == 0;
}

bool ChecksumXxHash32::less(const ChecksumXxHash32& other) const
{
    return bsl::memcmp(this, &other, sizeof *this) < 0;
}

bsl::ostream& ChecksumXxHash32::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    NTCCFG_WARNING_UNUSED(level);
    NTCCFG_WARNING_UNUSED(spacesPerLevel);

    const bsl::uint32_t value = this->value();
    return stream << value;
}

Checksum::Checksum()
: d_type(ntca::ChecksumType::e_UNDEFINED)
{
    BSLMF_ASSERT(sizeof(ntca::ChecksumAdler32::Digest) == 4);
    BSLMF_ASSERT(sizeof(ntca::ChecksumCrc32::Digest) == 4);
    BSLMF_ASSERT(sizeof(ntca::ChecksumXxHash32::Digest) == 4);
}

Checksum::Checksum(ntca::ChecksumType::Value type)
: d_type(type)
{
    if (d_type == ntca::ChecksumType::e_ADLER32) {
        new (d_adler32.buffer()) ntca::ChecksumAdler32();
    }
    else if (d_type == ntca::ChecksumType::e_CRC32) {
        new (d_crc32.buffer()) ntca::ChecksumCrc32();
    }
    else if (d_type == ntca::ChecksumType::e_XXH32) {
        new (d_xxHash32.buffer()) ntca::ChecksumXxHash32();
    }
}

Checksum::Checksum(const Checksum& original)
: d_type(original.d_type)
{
    if (original.d_type == ntca::ChecksumType::e_ADLER32) {
        new (d_adler32.buffer())
            ntca::ChecksumAdler32(original.d_adler32.object());
    }
    else if (original.d_type == ntca::ChecksumType::e_CRC32) {
        new (d_crc32.buffer()) ntca::ChecksumCrc32(original.d_crc32.object());
    }
    else if (original.d_type == ntca::ChecksumType::e_XXH32) {
        new (d_xxHash32.buffer())
            ntca::ChecksumXxHash32(original.d_xxHash32.object());
    }
}

Checksum::Checksum(bslmf::MovableRef<Checksum> original) NTSCFG_NOEXCEPT
{
    d_type = NTSCFG_MOVE_FROM(original, d_type);

    if (d_type == ntca::ChecksumType::e_ADLER32) {
        new (d_adler32.buffer()) ntca::ChecksumAdler32();
        d_adler32.object() = NTSCFG_MOVE_FROM(original, d_adler32.object());
    }
    else if (d_type == ntca::ChecksumType::e_CRC32) {
        new (d_crc32.buffer()) ntca::ChecksumCrc32();
        d_crc32.object() = NTSCFG_MOVE_FROM(original, d_crc32.object());
    }
    else if (d_type == ntca::ChecksumType::e_XXH32) {
        new (d_xxHash32.buffer()) ntca::ChecksumXxHash32();
        d_xxHash32.object() = NTSCFG_MOVE_FROM(original, d_xxHash32.object());
    }

    NTSCFG_MOVE_RESET(original);
}

Checksum::~Checksum()
{
}

Checksum& Checksum::operator=(const Checksum& other)
{
    if (this != &other) {
        if (other.d_type == ntca::ChecksumType::e_ADLER32) {
            new (d_adler32.buffer())
                ntca::ChecksumAdler32(other.d_adler32.object());
        }
        else if (other.d_type == ntca::ChecksumType::e_CRC32) {
            new (d_crc32.buffer()) ntca::ChecksumCrc32(other.d_crc32.object());
        }
        else if (other.d_type == ntca::ChecksumType::e_XXH32) {
            new (d_xxHash32.buffer())
                ntca::ChecksumXxHash32(other.d_xxHash32.object());
        }
        d_type = other.d_type;
    }

    return *this;
}

Checksum& Checksum::operator=(bslmf::MovableRef<Checksum> other)
    NTSCFG_NOEXCEPT
{
    d_type = NTSCFG_MOVE_FROM(other, d_type);

    if (d_type == ntca::ChecksumType::e_ADLER32) {
        new (d_adler32.buffer()) ntca::ChecksumAdler32();
        d_adler32.object() = NTSCFG_MOVE_FROM(other, d_adler32.object());
    }
    else if (d_type == ntca::ChecksumType::e_CRC32) {
        new (d_crc32.buffer()) ntca::ChecksumCrc32();
        d_crc32.object() = NTSCFG_MOVE_FROM(other, d_crc32.object());
    }
    else if (d_type == ntca::ChecksumType::e_XXH32) {
        new (d_xxHash32.buffer()) ntca::ChecksumXxHash32();
        d_xxHash32.object() = NTSCFG_MOVE_FROM(other, d_xxHash32.object());
    }

    NTSCFG_MOVE_RESET(other);

    return *this;
}

void Checksum::reset()
{
    d_type = ntca::ChecksumType::e_UNDEFINED;
}

void Checksum::reset(ntca::ChecksumType::Value type)
{
    d_type = type;

    if (d_type == ntca::ChecksumType::e_ADLER32) {
        new (d_adler32.buffer()) ntca::ChecksumAdler32();
    }
    else if (d_type == ntca::ChecksumType::e_CRC32) {
        new (d_crc32.buffer()) ntca::ChecksumCrc32();
    }
    else if (d_type == ntca::ChecksumType::e_XXH32) {
        new (d_xxHash32.buffer()) ntca::ChecksumXxHash32();
    }
}

ntsa::Error Checksum::store(ntca::ChecksumType::Value type,
                            const void*               value,
                            bsl::size_t               size)
{
    this->reset();

    if (type == ntca::ChecksumType::e_ADLER32) {
        BSLMF_ASSERT(sizeof(ntca::ChecksumAdler32::Digest) == 4);

        if (size != sizeof(ntca::ChecksumAdler32::Digest)) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        ntca::ChecksumAdler32::Digest digest = 0;
        bsl::memcpy(&digest, value, sizeof digest);

        new (d_adler32.buffer()) ntca::ChecksumAdler32(digest);
        d_type = type;
    }
    else if (type == ntca::ChecksumType::e_CRC32) {
        BSLMF_ASSERT(sizeof(ntca::ChecksumCrc32::Digest) == 4);

        if (size != sizeof(ntca::ChecksumCrc32::Digest)) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        ntca::ChecksumCrc32::Digest digest = 0;
        bsl::memcpy(&digest, value, sizeof digest);

        new (d_crc32.buffer()) ntca::ChecksumCrc32(digest);
        d_type = type;
    }
    else if (type == ntca::ChecksumType::e_XXH32) {
        BSLMF_ASSERT(sizeof(ntca::ChecksumXxHash32::Digest) == 4);

        if (size != sizeof(ntca::ChecksumXxHash32::Digest)) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        ntca::ChecksumXxHash32::Digest digest = 0;
        bsl::memcpy(&digest, value, sizeof digest);

        new (d_crc32.buffer()) ntca::ChecksumXxHash32(digest);
        d_type = type;
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Checksum::update(const void* data, bsl::size_t size)
{
    if (d_type == ntca::ChecksumType::e_ADLER32) {
        d_adler32.object().update(data, size);
    }
    else if (d_type == ntca::ChecksumType::e_CRC32) {
        d_crc32.object().update(data, size);
    }
    else if (d_type == ntca::ChecksumType::e_XXH32) {
        d_xxHash32.object().update(data, size);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Checksum::update(const bdlbb::Blob& data)
{
    return this->update(data, data.length());
}

ntsa::Error Checksum::update(const bdlbb::Blob& data, bsl::size_t size)
{
    if (size == 0) {
        return ntsa::Error();
    }

    bsl::size_t numBytesRemaining = size;

    const int numDataBuffers = data.numDataBuffers();

    bool done = false;

    for (int i = 0; i < numDataBuffers; ++i) {
        const bdlbb::BlobBuffer& buffer = data.buffer(i);

        const bsl::uint8_t* bufferData =
            reinterpret_cast<bsl::uint8_t*>(buffer.data());

        bsl::size_t bufferSize;
        if (i != numDataBuffers - 1) {
            bufferSize = static_cast<bsl::size_t>(buffer.size());
        }
        else {
            bufferSize = static_cast<bsl::size_t>(data.lastDataBufferLength());
        }

        bsl::size_t numBytesToUpdate;
        if (bufferSize < numBytesRemaining) {
            numBytesToUpdate   = bufferSize;
            numBytesRemaining -= bufferSize;
        }
        else {
            numBytesToUpdate  = numBytesRemaining;
            numBytesRemaining = 0;
            done              = true;
        }

        this->update(bufferData, numBytesToUpdate);

        if (done) {
            break;
        }
    }

    return ntsa::Error();
}

ntca::ChecksumType::Value Checksum::type() const
{
    return d_type;
}

bsl::size_t Checksum::size() const
{
    if (d_type == ntca::ChecksumType::e_ADLER32) {
        return sizeof(ntca::ChecksumAdler32::Digest);
    }
    else if (d_type == ntca::ChecksumType::e_CRC32) {
        return sizeof(ntca::ChecksumCrc32::Digest);
    }
    else if (d_type == ntca::ChecksumType::e_XXH32) {
        return sizeof(ntca::ChecksumXxHash32::Digest);
    }
    else {
        return 0;
    }
}

bsl::size_t Checksum::load(void* result, bsl::size_t capacity) const
{
    if (d_type == ntca::ChecksumType::e_ADLER32) {
        if (capacity < sizeof(ntca::ChecksumAdler32::Digest)) {
            return 0;
        }

        ntca::ChecksumAdler32::Digest digest = d_adler32.object().value();
        bsl::memcpy(result, &digest, sizeof(ntca::ChecksumAdler32::Digest));

        return sizeof(ntca::ChecksumAdler32::Digest);
    }
    else if (d_type == ntca::ChecksumType::e_CRC32) {
        if (capacity < sizeof(ntca::ChecksumCrc32::Digest)) {
            return 0;
        }

        ntca::ChecksumCrc32::Digest digest = d_crc32.object().value();
        bsl::memcpy(result, &digest, sizeof(ntca::ChecksumCrc32::Digest));

        return sizeof(ntca::ChecksumCrc32::Digest);
    }
    else if (d_type == ntca::ChecksumType::e_XXH32) {
        if (capacity < sizeof(ntca::ChecksumXxHash32::Digest)) {
            return 0;
        }

        ntca::ChecksumXxHash32::Digest digest = d_xxHash32.object().value();
        bsl::memcpy(result, &digest, sizeof(ntca::ChecksumXxHash32::Digest));

        return sizeof(ntca::ChecksumXxHash32::Digest);
    }
    else {
        return 0;
    }
}

bool Checksum::equals(const Checksum& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == ntca::ChecksumType::e_ADLER32) {
        return d_adler32.object() == other.d_adler32.object();
    }
    else if (d_type == ntca::ChecksumType::e_CRC32) {
        return d_crc32.object() == other.d_crc32.object();
    }
    else if (d_type == ntca::ChecksumType::e_XXH32) {
        return d_xxHash32.object() == other.d_xxHash32.object();
    }
    else {
        return true;
    }
}

bool Checksum::less(const Checksum& other) const
{
    if (d_type < other.d_type) {
        return true;
    }

    if (other.d_type < d_type) {
        return false;
    }

    if (d_type == ntca::ChecksumType::e_ADLER32) {
        return d_adler32.object() < other.d_adler32.object();
    }
    else if (d_type == ntca::ChecksumType::e_CRC32) {
        return d_crc32.object() < other.d_crc32.object();
    }
    else if (d_type == ntca::ChecksumType::e_XXH32) {
        return d_xxHash32.object() < other.d_xxHash32.object();
    }
    else {
        return false;
    }
}

bsl::ostream& Checksum::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("type", d_type);

    if (d_type == ntca::ChecksumType::e_ADLER32) {
        printer.printAttribute("value", d_adler32.object());
    }
    else if (d_type == ntca::ChecksumType::e_CRC32) {
        printer.printAttribute("value", d_crc32.object());
    }
    else if (d_type == ntca::ChecksumType::e_XXH32) {
        printer.printAttribute("value", d_xxHash32.object());
    }

    printer.end();
    return stream;
}

}  // close namespace ntca
}  // close namespace BloombergLP
