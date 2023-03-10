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

#include <ntcs_globalallocator.h>

#include <ntccfg_test.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_string.h>

using namespace BloombergLP;

namespace test {

/// Provide utilities for reading and writing to contiguous
/// locations in memory.
struct MemoryUtil {
    /// Generate a byte at the specified 'index' in the specified repeating
    /// 'pattern' of the specified 'size' starting at the specified
    /// 'position'. The behavior is undefined unless 'size > 0'. Note that
    /// if 'position > size' then 'position = position % size'. Also note
    /// that if 'index > size' then 'index = (position + index) % size'.
    static char cycle(bsl::size_t index,
                      const void* pattern,
                      bsl::size_t size,
                      bsl::size_t position);

    /// Write to the region of memory beginning at the specified 'address'
    /// and having the specified 'capacity' the specified repeating
    /// 'pattern' of the specified 'size' starting at the specified
    /// 'position', truncating that pattern appropriately if 'capacity' is
    /// not evenly divisible by 'size'. The behavior is undefined unless
    /// 'capacity > 0' and 'size > 0'.  Note that if 'position > size' then
    /// 'position = position % size'.
    static void write(void*       address,
                      bsl::size_t capacity,
                      const void* pattern,
                      bsl::size_t size,
                      bsl::size_t position);

    /// Write to the region of memory beginning at the specified 'address'
    /// and having the specified 'capacity' a repeating pattern of
    /// 0xDEADBEEF.
    static void writeDeadBeef(void* address, bsl::size_t capacity);

    /// Write to the region of memory beginning at the specified 'address'
    /// and having the specified 'capacity' a repeating pattern of
    /// 0xFACEFEED.
    static void writeFaceFeed(void* address, bsl::size_t capacity);

    /// Write to the region of memory beginning at the specified 'address'
    /// and having the specified 'capacity' a repeating pattern of
    /// 0xCAFEBABE.
    static void writeCafeBabe(void* address, bsl::size_t capacity);

    /// Write to the region of memory beginning at the specified 'address'
    /// and having the specified 'capacity' a repeating pattern of
    /// 0x00000000.
    static void write0s(void* address, bsl::size_t capacity);

    /// Write to the region of memory beginning at the specified 'address'
    /// and having the specified 'capacity' a repeating pattern of
    /// 0xFFFFFFFF.
    static void write1s(void* address, bsl::size_t capacity);

    /// Verify that the region of memory beginning at the specified
    /// 'address' and having the specified 'capacity' of defined data
    /// follows the specified repeating'pattern' of the specified 'size'
    /// starting at the specified 'position', truncated appropriately if
    /// 'capacity' is not evenly divisible by 'size'. The behavior is
    /// undefined unless 'capacity > 0' and 'size > 0'. Note that if
    /// 'position > size' then 'position = position % size'.
    static bool check(const void* address,
                      bsl::size_t capacity,
                      const void* pattern,
                      bsl::size_t size,
                      bsl::size_t position);

    /// Verify that the region of memory beginning at the specified
    /// 'address' and having the specified 'capacity' of defined data
    /// follows the specified repeating pattern of 0xDEADBEEF.
    static bool checkDeadBeef(void* address, bsl::size_t capacity);

    /// Verify that the region of memory beginning at the specified
    /// 'address' and having the specified 'capacity' of defined data
    /// follows the specified repeating pattern of 0xFACEFEED.
    static bool checkFaceFeed(void* address, bsl::size_t capacity);

    /// Verify that the region of memory beginning at the specified
    /// 'address' and having the specified 'capacity' of defined data
    /// follows the specified repeating pattern of 0xCAFEBABE.
    static bool checkCafebabe(void* address, bsl::size_t capacity);

    /// Verify that the region of memory beginning at the specified
    /// 'address' and having the specified 'capacity' of defined data
    /// follows the specified repeating pattern of 0x00000000.
    static bool check0s(void* address, bsl::size_t capacity);

    /// Verify that the region of memory beginning at the specified
    /// 'address' and having the specified 'capacity' of defined data
    /// follows the specified repeating pattern of 0xFFFFFFFF.
    static bool check1s(void* address, bsl::size_t capacity);

    /// The standard pattern.
    static const char PATTERN[26];
};

const char MemoryUtil::PATTERN[26] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

char MemoryUtil::cycle(bsl::size_t index,
                       const void* pattern,
                       bsl::size_t size,
                       bsl::size_t position)
{
    BSLS_ASSERT_OPT(pattern);
    BSLS_ASSERT_OPT(size > 0);

    return ((const char*)(pattern))[(position + index) % size];
}

void MemoryUtil::write(void*       address,
                       bsl::size_t capacity,
                       const void* pattern,
                       bsl::size_t size,
                       bsl::size_t position)
{
    BSLS_ASSERT_OPT(address);
    BSLS_ASSERT_OPT(pattern);

    BSLS_ASSERT_OPT(capacity > 0);
    BSLS_ASSERT_OPT(size > 0);

    char* dc = (char*)(address);
    char* de = dc + capacity;

    const bsl::size_t so = position % size;
    const char*       sb = (const char*)(pattern);
    const char*       sc = sb + so;
    const char*       se = sb + size;

    while (dc != de) {
        *dc++ = *sc++;

        if (sc == se) {
            sc = sb;
        }
    }
}

void MemoryUtil::writeDeadBeef(void* address, bsl::size_t capacity)
{
    const bsl::uint8_t DEADBEEF_ARRAY[4] = {0xDE, 0xAD, 0xBE, 0xEF};

    bsl::uint32_t DEADBEEF;
    bsl::memcpy(&DEADBEEF, DEADBEEF_ARRAY, sizeof DEADBEEF_ARRAY);

    MemoryUtil::write(address, capacity, &DEADBEEF, sizeof DEADBEEF, 0);
}

void MemoryUtil::writeFaceFeed(void* address, bsl::size_t capacity)
{
    const bsl::uint8_t FACEFEED_ARRAY[4] = {0xFA, 0xCE, 0xFE, 0xED};

    bsl::uint32_t FACEFEED;
    bsl::memcpy(&FACEFEED, FACEFEED_ARRAY, sizeof FACEFEED_ARRAY);

    MemoryUtil::write(address, capacity, &FACEFEED, sizeof FACEFEED, 0);
}

void MemoryUtil::writeCafeBabe(void* address, bsl::size_t capacity)
{
    const bsl::uint8_t CAFEBABE_ARRAY[4] = {0xCA, 0xFE, 0xBA, 0xBE};

    bsl::uint32_t CAFEBABE;
    bsl::memcpy(&CAFEBABE, CAFEBABE_ARRAY, sizeof CAFEBABE_ARRAY);

    MemoryUtil::write(address, capacity, &CAFEBABE, sizeof CAFEBABE, 0);
}

void MemoryUtil::write0s(void* address, bsl::size_t capacity)
{
    bsl::memset(address, 0x00, capacity);
}

void MemoryUtil::write1s(void* address, bsl::size_t capacity)
{
    bsl::memset(address, 0xFF, capacity);
}

bool MemoryUtil::check(const void* address,
                       bsl::size_t capacity,
                       const void* pattern,
                       bsl::size_t size,
                       bsl::size_t position)
{
    BSLS_ASSERT_OPT(address);
    BSLS_ASSERT_OPT(pattern);

    BSLS_ASSERT_OPT(capacity > 0);
    BSLS_ASSERT_OPT(size > 0);

    const char* dc = (const char*)(address);
    const char* de = dc + capacity;

    const bsl::size_t so = position % size;
    const char*       sb = (const char*)(pattern);
    const char*       sc = sb + so;
    const char*       se = sb + size;

    while (dc != de) {
        if (*dc++ != *sc++) {
            return false;
        }

        if (sc == se) {
            sc = sb;
        }
    }

    return true;
}

bool MemoryUtil::checkDeadBeef(void* address, bsl::size_t capacity)
{
    const bsl::uint8_t DEADBEEF_ARRAY[4] = {0xDE, 0xAD, 0xBE, 0xEF};

    bsl::uint32_t DEADBEEF;
    bsl::memcpy(&DEADBEEF, DEADBEEF_ARRAY, sizeof DEADBEEF_ARRAY);

    return MemoryUtil::check(address, capacity, &DEADBEEF, sizeof DEADBEEF, 0);
}

bool MemoryUtil::checkFaceFeed(void* address, bsl::size_t capacity)
{
    const bsl::uint8_t FACEFEED_ARRAY[4] = {0xFA, 0xCE, 0xFE, 0xED};

    bsl::uint32_t FACEFEED;
    bsl::memcpy(&FACEFEED, FACEFEED_ARRAY, sizeof FACEFEED_ARRAY);

    return MemoryUtil::check(address, capacity, &FACEFEED, sizeof FACEFEED, 0);
}

bool MemoryUtil::checkCafebabe(void* address, bsl::size_t capacity)
{
    const bsl::uint8_t CAFEBABE_ARRAY[4] = {0xCA, 0xFE, 0xBA, 0xBE};

    bsl::uint32_t CAFEBABE;
    bsl::memcpy(&CAFEBABE, CAFEBABE_ARRAY, sizeof CAFEBABE_ARRAY);

    return MemoryUtil::check(address, capacity, &CAFEBABE, sizeof CAFEBABE, 0);
}

bool MemoryUtil::check0s(void* address, bsl::size_t capacity)
{
    const bsl::uint8_t ALL_ZEROES_ARRAY[4] = {0x00, 0x00, 0x00, 0x00};

    bsl::uint32_t ALL_ZEROES;
    bsl::memcpy(&ALL_ZEROES, ALL_ZEROES_ARRAY, sizeof ALL_ZEROES_ARRAY);

    return MemoryUtil::check(address,
                             capacity,
                             &ALL_ZEROES,
                             sizeof ALL_ZEROES,
                             0);
}

bool MemoryUtil::check1s(void* address, bsl::size_t capacity)
{
    const bsl::uint8_t ALL_ONES_ARRAY[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    bsl::uint32_t ALL_ONES;
    bsl::memcpy(&ALL_ONES, ALL_ONES_ARRAY, sizeof ALL_ONES_ARRAY);

    return MemoryUtil::check(address, capacity, &ALL_ONES, sizeof ALL_ONES, 0);
}

}  // close namespace test

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------

// [ 1]
//-----------------------------------------------------------------------------
// [ 1]
//-----------------------------------------------------------------------------

NTCCFG_TEST_CASE(1)
{
    // Concern:
    // Plan:

    // Get the global allocator.

    ntcs::GlobalAllocator* globalAllocator =
        ntcs::GlobalAllocator::singleton();

    // Determine the page size.

    const bsl::size_t pageSize = globalAllocator->pageSize();

    // Allocate 1 byte, ensure it is zeroed, then read and write a pattern
    // to that memory.

    {
        const bsl::size_t size = 1;

        void* address = globalAllocator->allocate(size);
        NTCCFG_TEST_NE(address, 0);

        bool isZero = test::MemoryUtil::check0s(address, size);
        NTCCFG_TEST_TRUE(isZero);

        test::MemoryUtil::writeDeadBeef(address, size);

        bool isValid = test::MemoryUtil::checkDeadBeef(address, size);
        NTCCFG_TEST_TRUE(isValid);
    }

    // Ensure that one page is in use.

    NTCCFG_TEST_EQ(globalAllocator->numPagesInUse(), 1);

    // Allocate 'pageSize - 1' bytes (ensure that it is zeroed, then read and
    // write a pattern to that memory.

    {
        const bsl::size_t size = pageSize - 1;

        void* address = globalAllocator->allocate(size);
        NTCCFG_TEST_NE(address, 0);

        bool isZero = test::MemoryUtil::check0s(address, size);
        NTCCFG_TEST_TRUE(isZero);

        test::MemoryUtil::writeFaceFeed(address, size);

        bool isValid = test::MemoryUtil::checkFaceFeed(address, size);
        NTCCFG_TEST_TRUE(isValid);
    }

    // Ensure that one page is still in use.

    NTCCFG_TEST_EQ(globalAllocator->numPagesInUse(), 1);

    // Allocate 'pageSize - 1' bytes (ensure that it is zeroed, then read and
    // write a pattern to that memory.

    {
        const bsl::size_t size = pageSize - 1;

        void* address = globalAllocator->allocate(size);
        NTCCFG_TEST_NE(address, 0);

        bool isZero = test::MemoryUtil::check0s(address, size);
        NTCCFG_TEST_TRUE(isZero);

        test::MemoryUtil::writeDeadBeef(address, size);

        bool isValid = test::MemoryUtil::checkDeadBeef(address, size);
        NTCCFG_TEST_TRUE(isValid);
    }

    // Ensure that two pages are in use.

    NTCCFG_TEST_EQ(globalAllocator->numPagesInUse(), 2);

    // Allocate 1 byte, ensure it is zeroed, then read and write a pattern
    // to that memory.

    {
        const bsl::size_t size = 1;

        void* address = globalAllocator->allocate(size);
        NTCCFG_TEST_NE(address, 0);

        bool isZero = test::MemoryUtil::check0s(address, size);
        NTCCFG_TEST_TRUE(isZero);

        test::MemoryUtil::writeFaceFeed(address, size);

        bool isValid = test::MemoryUtil::checkFaceFeed(address, size);
        NTCCFG_TEST_TRUE(isValid);
    }

    // Ensure that two pages are still in use.

    NTCCFG_TEST_EQ(globalAllocator->numPagesInUse(), 2);

    // Allocate one whole page, ensure it is zeroed, then read and write a
    // pattern to that memory.

    {
        const bsl::size_t size = pageSize;

        void* address = globalAllocator->allocate(size);
        NTCCFG_TEST_NE(address, 0);

        bool isZero = test::MemoryUtil::check0s(address, size);
        NTCCFG_TEST_TRUE(isZero);

        test::MemoryUtil::writeDeadBeef(address, size);

        bool isValid = test::MemoryUtil::checkDeadBeef(address, size);
        NTCCFG_TEST_TRUE(isValid);
    }

    // Ensure that three pages are now in use.

    NTCCFG_TEST_EQ(globalAllocator->numPagesInUse(), 3);

    // Allocate two whole pages, ensure they are zeroed, then read and write a
    // pattern to that memory.

    {
        const bsl::size_t size = 2 * pageSize;

        void* address = globalAllocator->allocate(size);
        NTCCFG_TEST_NE(address, 0);

        bool isZero = test::MemoryUtil::check0s(address, size);
        NTCCFG_TEST_TRUE(isZero);

        test::MemoryUtil::writeFaceFeed(address, size);

        bool isValid = test::MemoryUtil::checkFaceFeed(address, size);
        NTCCFG_TEST_TRUE(isValid);
    }

    // Ensure that five pages are now in use.

    NTCCFG_TEST_EQ(globalAllocator->numPagesInUse(), 5);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
}
NTCCFG_TEST_DRIVER_END;
