
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

#include <ntscfg_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_checksum_t_cpp, "$Id$ $CSID$")

#include <ntca_checksum.h>

#if NTC_BUILD_WITH_ZLIB
#include <zlib.h>
#endif

using namespace BloombergLP;

namespace BloombergLP {
namespace ntca {

// Provide tests for 'ntca::Checksum'.
class ChecksumTest
{
    // Defines a type alias for a vector of bytes.
    typedef bsl::vector<bsl::uint8_t> ByteVector;

    // Load into the specified 'result' test data for having the specified
    // 'size'.
    static void generateData(ByteVector* result, bsl::size_t size);

    // Verify the update function for Adler-32 checksums.
    static void verifyUpdateAdler32(const ByteVector& input);

    // Verify the update function for Adler-32 checksums.
    static void verifyUpdateCrc32(const ByteVector& input);

    // Verify the update function for Adler-32 checksums.
    static void verifyUpdateXxHash32(const ByteVector& input);

    // Verify the update function for inputs of the specified 'size' for all
    // checksum types.
    static void verifyUpdateVariation(bsl::size_t size);

    BALL_LOG_SET_CLASS_CATEGORY("NTCA.CHECKSUM.TEST");

  public:
    // Test default constructor.
    static void verifyDefaultConstructor();

    // Test copy constructor.
    static void verifyCopyConstructor();

    // Test move constructor.
    static void verifyMoveConstructor();

    // Test assignment operator.
    static void verifyCopyAssignmentOperator();

    // Test move-assignment operator.
    static void verifyMoveAssignmentOperator();

    // Test 'reset'.
    static void verifyReset();

    // Test 'move'.
    static void verifyMove();

    // Test 'update'.
    static void verifyUpdate();

    // Test 'equals'.
    static void verifyEquals();

    // Test 'less'.
    static void verifyLess();

    // Test 'hash'.
    static void verifyHash();

    // Test 'print'.
    static void verifyPrint();
};

void ChecksumTest::generateData(bsl::vector<bsl::uint8_t>* result,
                                bsl::size_t                size)
{
    // 0b10011110001101110111100110110001
    const bsl::uint32_t k_PRIME32_1 = 0x9E3779B1U;

    // 0b10000101111010111100101001110111
    const bsl::uint32_t k_PRIME32_2 = 0x85EBCA77U;

    // 0b11000010101100101010111000111101
    const bsl::uint32_t k_PRIME32_3 = 0xC2B2AE3DU;

    // 0b00100111110101001110101100101111
    const bsl::uint32_t k_PRIME32_4 = 0x27D4EB2FU;

    // 0b00010110010101100110011110110001
    const bsl::uint32_t k_PRIME32_5 = 0x165667B1U;

    result->clear();
    result->resize(size);

    const uint32_t prime = k_PRIME32_1;

    bsl::uint32_t generator = prime;
    for (bsl::size_t i = 0; i < size; ++i) {
        (*result)[i]  = static_cast<bsl::uint8_t>(generator >> 24);
        generator    *= generator;
    }
}

void ChecksumTest::verifyUpdateAdler32(const ByteVector& input)
{
    ntsa::Error error;

#if NTC_BUILD_WITH_ZLIB

    // Verify datagram with 'adler32' from zlib.

    {
        const bsl::uint32_t z = adler32(0L, Z_NULL, 0);
        NTSCFG_TEST_EQ(z, 1);

        const bsl::uint32_t e = static_cast<bsl::uint32_t>(
            adler32(z,
                    reinterpret_cast<const Bytef*>(&input.front()),
                    static_cast<uInt>(input.size())));

        ntca::ChecksumAdler32 checksum;
        bsl::uint32_t         f = checksum.value();
        NTSCFG_TEST_EQ(f, z);

        error = checksum.update(&input.front(), input.size());
        NTSCFG_TEST_OK(error);

        f = checksum.value();
        NTSCFG_TEST_EQ(f, e);
    }

    // Verify stream with 'adler32' from zlib.

    {
        const bsl::uint32_t z = adler32(0L, Z_NULL, 0);
        NTSCFG_TEST_EQ(z, 1);

        const bsl::uint32_t e = static_cast<bsl::uint32_t>(
            adler32(z,
                    reinterpret_cast<const Bytef*>(&input.front()),
                    static_cast<uInt>(input.size())));

        for (bsl::size_t chunkSize = 1; chunkSize <= 4; ++chunkSize) {
            const bsl::uint8_t* p = &input.front();
            bsl::size_t         c = input.size();

            ntca::ChecksumAdler32 checksum;
            bsl::uint32_t         f = checksum.value();
            NTSCFG_TEST_EQ(f, z);

            while (c != 0) {
                bsl::size_t n = chunkSize;
                if (n > c) {
                    n = c;
                }

                error = checksum.update(p, n);
                NTSCFG_TEST_OK(error);

                p += n;
                c -= n;
            }

            NTSCFG_TEST_EQ(p, &input.front() + input.size());
            NTSCFG_TEST_EQ(c, 0);

            f = checksum.value();
            NTSCFG_TEST_EQ(f, e);
        }
    }

#endif
}

void ChecksumTest::verifyUpdateCrc32(const ByteVector& input)
{
    ntsa::Error error;

    // Verify datagram with 'bdlde::Crc32'.

    {
        bdlde::Crc32 x;

        const bsl::uint32_t z = x.checksum();
        NTSCFG_TEST_EQ(z, 0);

        x.update(&input.front(), input.size());
        const bsl::uint32_t e = x.checksum();

        ntca::ChecksumCrc32 checksum;
        bsl::uint32_t       f = checksum.value();
        NTSCFG_TEST_EQ(f, z);

        error = checksum.update(&input.front(), input.size());
        NTSCFG_TEST_OK(error);

        f = checksum.value();
        NTSCFG_TEST_EQ(f, e);
    }

    // Verify stream with 'bdlde::Crc32'.

    {
        bdlde::Crc32 x;

        const bsl::uint32_t z = x.checksum();
        NTSCFG_TEST_EQ(z, 0);

        x.update(&input.front(), input.size());
        const bsl::uint32_t e = x.checksum();

        for (bsl::size_t chunkSize = 1; chunkSize <= 4; ++chunkSize) {
            const bsl::uint8_t* p = &input.front();
            bsl::size_t         c = input.size();

            ntca::ChecksumCrc32 checksum;
            bsl::uint32_t       f = checksum.value();
            NTSCFG_TEST_EQ(f, z);

            while (c != 0) {
                bsl::size_t n = chunkSize;
                if (n > c) {
                    n = c;
                }

                error = checksum.update(p, n);
                NTSCFG_TEST_OK(error);

                p += n;
                c -= n;
            }

            NTSCFG_TEST_EQ(p, &input.front() + input.size());
            NTSCFG_TEST_EQ(c, 0);

            f = checksum.value();
            NTSCFG_TEST_EQ(f, e);
        }
    }

#if NTC_BUILD_WITH_ZLIB

    // Verify datagram with 'crc32' from zlib.

    {
        const bsl::uint32_t z = crc32(0L, Z_NULL, 0);
        NTSCFG_TEST_EQ(z, 0);

        const bsl::uint32_t e = static_cast<bsl::uint32_t>(
            crc32(z,
                  reinterpret_cast<const Bytef*>(&input.front()),
                  static_cast<uInt>(input.size())));

        ntca::ChecksumCrc32 checksum;
        bsl::uint32_t       f = checksum.value();
        NTSCFG_TEST_EQ(f, z);

        error = checksum.update(&input.front(), input.size());
        NTSCFG_TEST_OK(error);

        f = checksum.value();
        NTSCFG_TEST_EQ(f, e);
    }

    // Verify stream with 'crc32' from zlib.

    {
        const bsl::uint32_t z = crc32(0L, Z_NULL, 0);
        NTSCFG_TEST_EQ(z, 0);

        const bsl::uint32_t e = static_cast<bsl::uint32_t>(
            crc32(z,
                  reinterpret_cast<const Bytef*>(&input.front()),
                  static_cast<uInt>(input.size())));

        for (bsl::size_t chunkSize = 1; chunkSize <= 4; ++chunkSize) {
            const bsl::uint8_t* p = &input.front();
            bsl::size_t         c = input.size();

            ntca::ChecksumCrc32 checksum;
            bsl::uint32_t       f = checksum.value();
            NTSCFG_TEST_EQ(f, z);

            while (c != 0) {
                bsl::size_t n = chunkSize;
                if (n > c) {
                    n = c;
                }

                error = checksum.update(p, n);
                NTSCFG_TEST_OK(error);

                p += n;
                c -= n;
            }

            NTSCFG_TEST_EQ(p, &input.front() + input.size());
            NTSCFG_TEST_EQ(c, 0);

            f = checksum.value();
            NTSCFG_TEST_EQ(f, e);
        }
    }

#endif
}

void ChecksumTest::verifyUpdateXxHash32(const ByteVector& input)
{
    ntsa::Error error;

    ntca::ChecksumXxHash32 checksum;
    error = checksum.update(&input.front(), input.size());
    NTSCFG_TEST_OK(error);

    bsl::uint32_t e = checksum.value();

    for (bsl::size_t chunkSize = 1; chunkSize <= 4; ++chunkSize) {
        const bsl::uint8_t* p = &input.front();
        bsl::size_t         c = input.size();

        ntca::ChecksumXxHash32 otherChecksum;
        while (c != 0) {
            bsl::size_t n = chunkSize;
            if (n > c) {
                n = c;
            }

            error = otherChecksum.update(p, n);
            NTSCFG_TEST_OK(error);

            p += n;
            c -= n;
        }

        NTSCFG_TEST_EQ(p, &input.front() + input.size());
        NTSCFG_TEST_EQ(c, 0);

        bsl::uint32_t f = otherChecksum.value();
        NTSCFG_TEST_EQ(f, e);
    }
}

void ChecksumTest::verifyUpdateVariation(bsl::size_t size)
{
    ByteVector input;
    ntca::ChecksumTest::generateData(&input, size);

    BALL_LOG_DEBUG << "Test update size " << size << BALL_LOG_END;

    ntca::ChecksumTest::verifyUpdateAdler32(input);
    ntca::ChecksumTest::verifyUpdateCrc32(input);
    ntca::ChecksumTest::verifyUpdateXxHash32(input);
}

NTSCFG_TEST_FUNCTION(ntca::ChecksumTest::verifyDefaultConstructor)
{
    ntca::Checksum checksum;
    NTSCFG_TEST_EQ(checksum.type(), ntca::ChecksumType::e_UNDEFINED);
}

NTSCFG_TEST_FUNCTION(ntca::ChecksumTest::verifyCopyConstructor)
{
    ntsa::Error error;

    // Define a distinct value that does not have the same value as a
    // default-constructed value.

    bsl::uint32_t otherChecksumInitializer = 0xFFFFFFFF;

    ntca::Checksum otherChecksum;
    error = otherChecksum.store(ntca::ChecksumType::e_CRC32,
                                &otherChecksumInitializer,
                                sizeof otherChecksumInitializer);
    NTSCFG_TEST_OK(error);

    // Ensure the other value has the expected value.

    bsl::uint32_t otherChecksumValue = 0;

    bsl::size_t otherChecksumValueLength =
        otherChecksum.load(&otherChecksumValue, sizeof otherChecksumValue);
    NTSCFG_TEST_EQ(otherChecksumValueLength, sizeof otherChecksumValue);
    NTSCFG_TEST_EQ(otherChecksumValue, otherChecksumInitializer);

    // Copy-construct a value from the other value.

    char            buffer[sizeof(ntca::Checksum)];
    ntca::Checksum* checksum = new (buffer) ntca::Checksum(otherChecksum);

    // Ensure the copy-constructed value has the expected value.

    bsl::uint32_t checksumValue = 0;

    bsl::size_t checksumValueLength =
        checksum->load(&checksumValue, sizeof checksumValue);
    NTSCFG_TEST_EQ(checksumValueLength, sizeof checksumValue);
    NTSCFG_TEST_EQ(checksumValue, otherChecksumInitializer);

    // Ensure the other value still has its original value.

    otherChecksumValueLength =
        otherChecksum.load(&otherChecksumValue, sizeof otherChecksumValue);
    NTSCFG_TEST_EQ(otherChecksumValueLength, sizeof otherChecksumValue);
    NTSCFG_TEST_EQ(otherChecksumValue, otherChecksumInitializer);
}

NTSCFG_TEST_FUNCTION(ntca::ChecksumTest::verifyMoveConstructor)
{
    ntsa::Error error;

    // Define a distinct value that does not have the same value as a
    // default-constructed value.

    bsl::uint32_t otherChecksumInitializer = 0xFFFFFFFF;

    ntca::Checksum otherChecksum;
    error = otherChecksum.store(ntca::ChecksumType::e_CRC32,
                                &otherChecksumInitializer,
                                sizeof otherChecksumInitializer);
    NTSCFG_TEST_OK(error);

    // Ensure the other value has the expected value.

    bsl::uint32_t otherChecksumValue = 0;

    bsl::size_t otherChecksumValueLength =
        otherChecksum.load(&otherChecksumValue, sizeof otherChecksumValue);
    NTSCFG_TEST_EQ(otherChecksumValueLength, sizeof otherChecksumValue);
    NTSCFG_TEST_EQ(otherChecksumValue, otherChecksumInitializer);

    // Move-construct a value from the other value.

    char            buffer[sizeof(ntca::Checksum)];
    ntca::Checksum* checksum = new (buffer)
        ntca::Checksum(bslmf::MovableRefUtil::move(otherChecksum));

    // Ensure the move-constructed value has the expected value.

    bsl::uint32_t checksumValue = 0;

    bsl::size_t checksumValueLength =
        checksum->load(&checksumValue, sizeof checksumValue);
    NTSCFG_TEST_EQ(checksumValueLength, sizeof checksumValue);
    NTSCFG_TEST_EQ(checksumValue, otherChecksumInitializer);

    if (NTSCFG_MOVE_RESET_ENABLED) {
        // Ensure the other value has the default value.

        NTSCFG_TEST_EQ(otherChecksum.type(), ntca::ChecksumType::e_UNDEFINED);
    }
}

NTSCFG_TEST_FUNCTION(ntca::ChecksumTest::verifyCopyAssignmentOperator)
{
    ntsa::Error error;

    // Define a distinct value that does not have the same value as a
    // default-constructed value.

    bsl::uint32_t otherChecksumInitializer = 0xFFFFFFFF;

    ntca::Checksum otherChecksum;
    error = otherChecksum.store(ntca::ChecksumType::e_CRC32,
                                &otherChecksumInitializer,
                                sizeof otherChecksumInitializer);
    NTSCFG_TEST_OK(error);

    // Ensure the other value has the expected value.

    bsl::uint32_t otherChecksumValue = 0;

    bsl::size_t otherChecksumValueLength =
        otherChecksum.load(&otherChecksumValue, sizeof otherChecksumValue);
    NTSCFG_TEST_EQ(otherChecksumValueLength, sizeof otherChecksumValue);
    NTSCFG_TEST_EQ(otherChecksumValue, otherChecksumInitializer);

    // Assign the other value to the value.

    ntca::Checksum checksum;
    checksum = otherChecksum;

    // Ensure the copy-constructed value has the expected value.

    bsl::uint32_t checksumValue = 0;

    bsl::size_t checksumValueLength =
        checksum.load(&checksumValue, sizeof checksumValue);
    NTSCFG_TEST_EQ(checksumValueLength, sizeof checksumValue);
    NTSCFG_TEST_EQ(checksumValue, otherChecksumInitializer);

    // Ensure the other value still has its original value.

    otherChecksumValueLength =
        otherChecksum.load(&otherChecksumValue, sizeof otherChecksumValue);
    NTSCFG_TEST_EQ(otherChecksumValueLength, sizeof otherChecksumValue);
    NTSCFG_TEST_EQ(otherChecksumValue, otherChecksumInitializer);
}

NTSCFG_TEST_FUNCTION(ntca::ChecksumTest::verifyMoveAssignmentOperator)
{
    ntsa::Error error;

    // Define a distinct value that does not have the same value as a
    // default-constructed value.

    bsl::uint32_t otherChecksumInitializer = 0xFFFFFFFF;

    ntca::Checksum otherChecksum;
    error = otherChecksum.store(ntca::ChecksumType::e_CRC32,
                                &otherChecksumInitializer,
                                sizeof otherChecksumInitializer);
    NTSCFG_TEST_OK(error);

    // Ensure the other value has the expected value.

    bsl::uint32_t otherChecksumValue = 0;

    bsl::size_t otherChecksumValueLength =
        otherChecksum.load(&otherChecksumValue, sizeof otherChecksumValue);
    NTSCFG_TEST_EQ(otherChecksumValueLength, sizeof otherChecksumValue);
    NTSCFG_TEST_EQ(otherChecksumValue, otherChecksumInitializer);

    // Move-assign the other value to the value.

    ntca::Checksum checksum;
    checksum = bslmf::MovableRefUtil::move(otherChecksum);

    // Ensure the move-constructed value has the expected value.

    bsl::uint32_t checksumValue = 0;

    bsl::size_t checksumValueLength =
        checksum.load(&checksumValue, sizeof checksumValue);
    NTSCFG_TEST_EQ(checksumValueLength, sizeof checksumValue);
    NTSCFG_TEST_EQ(checksumValue, otherChecksumInitializer);

    if (NTSCFG_MOVE_RESET_ENABLED) {
        // Ensure the other value has the default value.

        NTSCFG_TEST_EQ(otherChecksum.type(), ntca::ChecksumType::e_UNDEFINED);
    }
}

NTSCFG_TEST_FUNCTION(ntca::ChecksumTest::verifyReset)
{
    ntsa::Error error;

    // Define a distinct value that does not have the same value as a
    // default-constructed value.

    bsl::uint32_t checksumInitializer = 0xFFFFFFFF;

    ntca::Checksum checksum;
    error = checksum.store(ntca::ChecksumType::e_CRC32,
                           &checksumInitializer,
                           sizeof checksumInitializer);
    NTSCFG_TEST_OK(error);

    // Ensure the other value has the expected value.

    bsl::uint32_t checksumValue = 0;

    bsl::size_t checksumValueLength =
        checksum.load(&checksumValue, sizeof checksumValue);
    NTSCFG_TEST_EQ(checksumValueLength, sizeof checksumValue);
    NTSCFG_TEST_EQ(checksumValue, checksumInitializer);

    // Reset the value of the value to its value upon default construction.

    checksum.reset();

    // Ensure the value has the default value.

    NTSCFG_TEST_EQ(checksum.type(), ntca::ChecksumType::e_UNDEFINED);
}

NTSCFG_TEST_FUNCTION(ntca::ChecksumTest::verifyMove)
{
}

NTSCFG_TEST_FUNCTION(ntca::ChecksumTest::verifyUpdate)
{
    ntsa::Error error;

    bsl::vector<bsl::size_t> sizeVector;
    sizeVector.push_back(1);
    sizeVector.push_back(2);
    sizeVector.push_back(3);
    sizeVector.push_back(4);
    sizeVector.push_back(5);
    sizeVector.push_back(6);
    sizeVector.push_back(7);
    sizeVector.push_back(8);
    sizeVector.push_back(13);
    sizeVector.push_back(31);
    sizeVector.push_back(101);
    sizeVector.push_back(256);
    sizeVector.push_back(1024);
    sizeVector.push_back(4096);

    for (bsl::size_t i = 0; i < sizeVector.size(); ++i) {
        ntca::ChecksumTest::verifyUpdateVariation(sizeVector[i]);
    }
}

NTSCFG_TEST_FUNCTION(ntca::ChecksumTest::verifyEquals)
{
    ntsa::Error error;

    {
        ntca::Checksum checksum;
        ntca::Checksum otherChecksum;

        {
            bool result = checksum == otherChecksum;
            NTSCFG_TEST_EQ(result, true);
        }

        {
            bool result = checksum != otherChecksum;
            NTSCFG_TEST_EQ(result, false);
        }
    }

    {
        ntca::Checksum checksum;
        ntca::Checksum otherChecksum;

        bsl::uint32_t checksumInitializer = 0xFFFFFFFF;

        error = checksum.store(ntca::ChecksumType::e_CRC32,
                               &checksumInitializer,
                               sizeof checksumInitializer);
        NTSCFG_TEST_OK(error);

        {
            bool result = checksum == otherChecksum;
            NTSCFG_TEST_EQ(result, false);
        }

        {
            bool result = checksum != otherChecksum;
            NTSCFG_TEST_EQ(result, true);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntca::ChecksumTest::verifyLess)
{
    ntsa::Error error;

    {
        ntca::Checksum checksum;
        ntca::Checksum otherChecksum;

        {
            bool result = checksum < otherChecksum;
            NTSCFG_TEST_EQ(result, false);
        }
    }

    {
        ntca::Checksum checksum;
        ntca::Checksum otherChecksum;

        bsl::uint32_t checksumInitializer = 1;

        error = checksum.store(ntca::ChecksumType::e_CRC32,
                               &checksumInitializer,
                               sizeof checksumInitializer);
        NTSCFG_TEST_OK(error);

        bsl::uint32_t otherChecksumInitializer = 2;

        error = otherChecksum.store(ntca::ChecksumType::e_CRC32,
                                    &otherChecksumInitializer,
                                    sizeof otherChecksumInitializer);
        NTSCFG_TEST_OK(error);

        {
            bool result = checksum < otherChecksum;
            NTSCFG_TEST_EQ(result, true);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntca::ChecksumTest::verifyHash)
{
    ntsa::Error error;

    // Test hash member function with default hash algorithm.

    {
        ntca::Checksum checksum;
        ntca::Checksum otherChecksum;

        bsl::size_t h1 = checksum.hash();
        bsl::size_t h2 = otherChecksum.hash();

        NTSCFG_TEST_EQ(h1, h2);
    }

    {
        ntca::Checksum checksum;
        ntca::Checksum otherChecksum;

        bsl::uint32_t otherChecksumInitializer = 0xFFFFFFFF;

        error = otherChecksum.store(ntca::ChecksumType::e_CRC32,
                                    &otherChecksumInitializer,
                                    sizeof otherChecksumInitializer);
        NTSCFG_TEST_OK(error);

        bsl::size_t h1 = checksum.hash();
        bsl::size_t h2 = otherChecksum.hash();

        NTSCFG_TEST_NE(h1, h2);
    }

    // Test hash member function with default hash algorithm.

    {
        ntca::Checksum checksum;
        ntca::Checksum otherChecksum;

        bsl::size_t h1 = checksum.hash<bslh::DefaultHashAlgorithm>();
        bsl::size_t h2 = otherChecksum.hash<bslh::DefaultHashAlgorithm>();

        NTSCFG_TEST_EQ(h1, h2);
    }

    {
        ntca::Checksum checksum;
        ntca::Checksum otherChecksum;

        bsl::uint32_t otherChecksumInitializer = 0xFFFFFFFF;

        error = otherChecksum.store(ntca::ChecksumType::e_CRC32,
                                    &otherChecksumInitializer,
                                    sizeof otherChecksumInitializer);
        NTSCFG_TEST_OK(error);

        bsl::size_t h1 = checksum.hash<bslh::DefaultHashAlgorithm>();
        bsl::size_t h2 = otherChecksum.hash<bslh::DefaultHashAlgorithm>();

        NTSCFG_TEST_NE(h1, h2);
    }

    // Test using bslh::Hash.

    {
        ntca::Checksum checksum;
        ntca::Checksum otherChecksum;

        bsl::size_t h1 = bslh::Hash<>()(checksum);
        bsl::size_t h2 = bslh::Hash<>()(otherChecksum);

        NTSCFG_TEST_EQ(h1, h2);
    }

    {
        ntca::Checksum checksum;
        ntca::Checksum otherChecksum;

        bsl::uint32_t otherChecksumInitializer = 0xFFFFFFFF;

        error = otherChecksum.store(ntca::ChecksumType::e_CRC32,
                                    &otherChecksumInitializer,
                                    sizeof otherChecksumInitializer);
        NTSCFG_TEST_OK(error);

        bsl::size_t h1 = bslh::Hash<>()(checksum);
        bsl::size_t h2 = bslh::Hash<>()(otherChecksum);

        NTSCFG_TEST_NE(h1, h2);
    }
}

NTSCFG_TEST_FUNCTION(ntca::ChecksumTest::verifyPrint)
{
    ntsa::Error    error;
    ntca::Checksum checksum;

    {
        bsl::stringstream ss;
        checksum.print(ss, 0, -1);
        if (NTSCFG_TEST_VERBOSITY) {
            bsl::cout << "Checksum = " << ss.str() << bsl::endl;
        }
    }

    {
        bsl::stringstream ss;
        checksum.print(ss, 1, 4);
        if (NTSCFG_TEST_VERBOSITY) {
            bsl::cout << "Checksum = " << ss.str() << bsl::endl;
        }
    }

    // Define other value that does not have the same value as a
    // default-constructed value.

    ntca::Checksum otherChecksum;

    bsl::uint32_t otherChecksumInitializer = 0xFFFFFFFF;

    error = otherChecksum.store(ntca::ChecksumType::e_CRC32,
                                &otherChecksumInitializer,
                                sizeof otherChecksumInitializer);
    NTSCFG_TEST_OK(error);

    {
        bsl::stringstream ss;
        otherChecksum.print(ss, 0, -1);
        if (NTSCFG_TEST_VERBOSITY) {
            bsl::cout << "Checksum = " << ss.str() << bsl::endl;
        }
    }

    {
        bsl::stringstream ss;
        otherChecksum.print(ss, 1, 4);
        if (NTSCFG_TEST_VERBOSITY) {
            bsl::cout << "Checksum = " << ss.str() << bsl::endl;
        }
    }
}

}  // close namespace ntca
}  // close namespace BloombergLP
