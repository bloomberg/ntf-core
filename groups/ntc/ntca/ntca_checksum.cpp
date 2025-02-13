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

namespace BloombergLP {
namespace ntca {

Checksum::Checksum()
: d_type(ntca::ChecksumType::e_CRC32)
{
    new(d_crc32.buffer()) Crc32();
}

Checksum::Checksum(ntca::ChecksumType::Value type)
: d_type(type)
{
    if (d_type == ntca::ChecksumType::e_CRC32) {
        new(d_crc32.buffer()) Crc32();
    }
    else if (d_type == ntca::ChecksumType::e_CRC32C) {
        new(d_crc32c.buffer()) Crc32c(bdlde::Crc32c::k_NULL_CRC32C);
    }
    else if (d_type == ntca::ChecksumType::e_XXHASH32) {
        new(d_xxHash32.buffer()) XxHash32(0);
    }
    else if (d_type == ntca::ChecksumType::e_XXHASH64) {
        new(d_xxHash64.buffer()) XxHash64(0);
    }
}

Checksum::Checksum(const Checksum& original)
: d_type(original.d_type)
{
    if (original.d_type == ntca::ChecksumType::e_CRC32) {
        new(d_crc32.buffer()) Crc32(original.d_crc32.object());
    }
    else if (original.d_type == ntca::ChecksumType::e_CRC32C) {
        new(d_crc32c.buffer()) Crc32c(original.d_crc32c.object());
    }
    else if (original.d_type == ntca::ChecksumType::e_XXHASH32) {
        new(d_xxHash32.buffer()) XxHash32(original.d_xxHash32.object());
    }
    else if (original.d_type == ntca::ChecksumType::e_XXHASH64) {
        new(d_xxHash64.buffer()) XxHash64(original.d_xxHash64.object());
    }
}

Checksum::~Checksum()
{
}

Checksum& Checksum::operator=(const Checksum& other)
{
    if (this != &other) {
        if (other.d_type == ntca::ChecksumType::e_CRC32) {
            new(d_crc32.buffer()) Crc32(other.d_crc32.object());
        }
        else if (other.d_type == ntca::ChecksumType::e_CRC32C) {
            new(d_crc32c.buffer()) Crc32c(other.d_crc32c.object());
        }
        else if (other.d_type == ntca::ChecksumType::e_XXHASH32) {
            new(d_xxHash32.buffer()) XxHash32(other.d_xxHash32.object());
        }
        else if (other.d_type == ntca::ChecksumType::e_XXHASH64) {
            new(d_xxHash64.buffer()) XxHash64(other.d_xxHash64.object());
        }
        d_type = other.d_type;
    }

    return *this;
}

void Checksum::reset()
{
    if (d_type == ntca::ChecksumType::e_CRC32) {
        d_crc32.object().reset();
    }
    else if (d_type == ntca::ChecksumType::e_CRC32C) {
        d_crc32c.object() = bdlde::Crc32c::k_NULL_CRC32C;
    }
    else if (d_type == ntca::ChecksumType::e_XXHASH32) {
        d_xxHash32.object() = 0;
    }
    else if (d_type == ntca::ChecksumType::e_XXHASH64) {
        d_xxHash64.object() = 0;
    }
    else {
        NTCCFG_UNREACHABLE();
    }
}

ntsa::Error Checksum::store(ntca::ChecksumType::Value type, 
                            const void*               value, 
                            bsl::size_t               size)
{
    if (type == ntca::ChecksumType::e_CRC32) {
        if (size != 4) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        BSLMF_ASSERT(sizeof(Crc32) == 4);

        new(d_crc32.buffer()) Crc32();
        bsl::memcpy(d_crc32.buffer(), value, 4);
    }
    else if (type == ntca::ChecksumType::e_CRC32C) {
        if (size != 4) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        BSLMF_ASSERT(sizeof(Crc32c) == 4);

        new(d_crc32c.buffer()) Crc32c();
        bsl::memcpy(d_crc32c.buffer(), value, 4);
    }
    else if (type == ntca::ChecksumType::e_XXHASH32) {
        if (size != 4) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        BSLMF_ASSERT(sizeof(XxHash32) == 4);

        new(d_xxHash32.buffer()) XxHash32();
        bsl::memcpy(d_xxHash32.buffer(), value, 4);
    }
    else if (type == ntca::ChecksumType::e_XXHASH64) {
        if (size != 8) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        BSLMF_ASSERT(sizeof(XxHash64) == 8);

        new(d_xxHash64.buffer()) XxHash64();
        bsl::memcpy(d_xxHash64.buffer(), value, 8);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Checksum::update(const void* data, bsl::size_t size)
{
    if (d_type == ntca::ChecksumType::e_CRC32) {
        d_crc32.object().update(data, size);
    }
    else if (d_type == ntca::ChecksumType::e_CRC32C) {
        d_crc32c.object() = 
            bdlde::Crc32c::calculate(data, size, d_crc32c.object());
    }
    else if (d_type == ntca::ChecksumType::e_XXHASH32) {
        NTCCFG_NOT_IMPLEMENTED();
    }
    else if (d_type == ntca::ChecksumType::e_XXHASH64) {
        NTCCFG_NOT_IMPLEMENTED();
    }
    else {
        NTCCFG_UNREACHABLE();
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
    if (d_type == ntca::ChecksumType::e_CRC32) {
        return 4;
    }
    else if (d_type == ntca::ChecksumType::e_CRC32C) {
        return 4;
    }
    else if (d_type == ntca::ChecksumType::e_XXHASH32) {
        return 4;
    }
    else if (d_type == ntca::ChecksumType::e_XXHASH64) {
        return 8;
    }
    else {
        NTCCFG_UNREACHABLE();
        return 0;
    }
}

bsl::size_t Checksum::load(void* result, bsl::size_t capacity) const
{
    if (d_type == ntca::ChecksumType::e_CRC32) {
        if (capacity < 4) {
            return 0;
        }
        bsl::uint32_t temp = d_crc32.object().checksum();
        bsl::memcpy(result, &temp, sizeof temp);
        return 4;
    }
    else if (d_type == ntca::ChecksumType::e_CRC32C) {
        if (capacity < 4) {
            return 0;
        }
        bsl::memcpy(result, &d_crc32c.object(), 4);
        return 4;
    }
    else if (d_type == ntca::ChecksumType::e_XXHASH32) {
        if (capacity < 4) {
            return 0;
        }
        bsl::memcpy(result, &d_xxHash32.object(), 4);
        return 4;
    }
    else if (d_type == ntca::ChecksumType::e_XXHASH64) {
        if (capacity < 8) {
            return 0;
        }
        bsl::memcpy(result, &d_xxHash64.object(), 4);
        return 8;
    }
    else {
        NTCCFG_UNREACHABLE();
        return 0;
    }
}

bool Checksum::equals(const Checksum& other) const
{
    if (d_type != other.d_type) {
        return false;    
    }
    
    if (d_type == ntca::ChecksumType::e_CRC32) {
        return d_crc32.object() == other.d_crc32.object();
    }
    else if (d_type == ntca::ChecksumType::e_CRC32C) {
        return d_crc32c.object() == other.d_crc32c.object();
    }
    else if (d_type == ntca::ChecksumType::e_XXHASH32) {
        return d_xxHash32.object() == other.d_xxHash32.object();
    }
    else if (d_type == ntca::ChecksumType::e_XXHASH64) {
        return d_xxHash64.object() == other.d_xxHash64.object();
    }
    else {
        NTCCFG_UNREACHABLE();
        return false;
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

    if (d_type == ntca::ChecksumType::e_CRC32) {
        return d_crc32.object().checksum() < other.d_crc32.object().checksum();
    }
    else if (d_type == ntca::ChecksumType::e_CRC32C) {
        return d_crc32c.object() < other.d_crc32c.object();
    }
    else if (d_type == ntca::ChecksumType::e_XXHASH32) {
        return d_xxHash32.object() < other.d_xxHash32.object();
    }
    else if (d_type == ntca::ChecksumType::e_XXHASH64) {
        return d_xxHash64.object() < other.d_xxHash64.object();
    }
    else {
        NTCCFG_UNREACHABLE();
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

    if (d_type == ntca::ChecksumType::e_CRC32) {
        printer.printAttribute("value", d_crc32.object().checksum());
    }
    else if (d_type == ntca::ChecksumType::e_CRC32C) {
        printer.printAttribute("value", d_crc32c.object());
    }
    else if (d_type == ntca::ChecksumType::e_XXHASH32) {
        printer.printAttribute("value", d_xxHash32.object());
    }
    else if (d_type == ntca::ChecksumType::e_XXHASH64) {
        printer.printAttribute("value", d_xxHash64.object());
    }
    else {
        NTCCFG_UNREACHABLE();
    }

    printer.end();
    return stream;
}

}  // close namespace ntca
}  // close namespace BloombergLP

