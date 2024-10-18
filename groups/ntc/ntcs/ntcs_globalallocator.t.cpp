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
BSLS_IDENT_RCSID(ntcs_async_t_cpp, "$Id$ $CSID$")

#include <ntcs_globalallocator.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::GlobalAllocator'.
class GlobalAllocatorTest
{
  public:
    // TODO
    static void verify();
};

NTSCFG_TEST_FUNCTION(ntcs::GlobalAllocatorTest::verify)
{
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
        NTSCFG_TEST_NE(address, 0);

        bool isZero = ntscfg::TestMemoryUtil::check0s(address, size);
        NTSCFG_TEST_TRUE(isZero);

        ntscfg::TestMemoryUtil::writeDeadBeef(address, size);

        bool isValid = ntscfg::TestMemoryUtil::checkDeadBeef(address, size);
        NTSCFG_TEST_TRUE(isValid);
    }

    // Ensure that one page is in use.

    NTSCFG_TEST_EQ(globalAllocator->numPagesInUse(), 1);

    // Allocate 'pageSize - 1' bytes (ensure that it is zeroed, then read and
    // write a pattern to that memory.

    {
        const bsl::size_t size = pageSize - 1;

        void* address = globalAllocator->allocate(size);
        NTSCFG_TEST_NE(address, 0);

        bool isZero = ntscfg::TestMemoryUtil::check0s(address, size);
        NTSCFG_TEST_TRUE(isZero);

        ntscfg::TestMemoryUtil::writeFaceFeed(address, size);

        bool isValid = ntscfg::TestMemoryUtil::checkFaceFeed(address, size);
        NTSCFG_TEST_TRUE(isValid);
    }

    // Ensure that one page is still in use.

    NTSCFG_TEST_EQ(globalAllocator->numPagesInUse(), 1);

    // Allocate 'pageSize - 1' bytes (ensure that it is zeroed, then read and
    // write a pattern to that memory.

    {
        const bsl::size_t size = pageSize - 1;

        void* address = globalAllocator->allocate(size);
        NTSCFG_TEST_NE(address, 0);

        bool isZero = ntscfg::TestMemoryUtil::check0s(address, size);
        NTSCFG_TEST_TRUE(isZero);

        ntscfg::TestMemoryUtil::writeDeadBeef(address, size);

        bool isValid = ntscfg::TestMemoryUtil::checkDeadBeef(address, size);
        NTSCFG_TEST_TRUE(isValid);
    }

    // Ensure that two pages are in use.

    NTSCFG_TEST_EQ(globalAllocator->numPagesInUse(), 2);

    // Allocate 1 byte, ensure it is zeroed, then read and write a pattern
    // to that memory.

    {
        const bsl::size_t size = 1;

        void* address = globalAllocator->allocate(size);
        NTSCFG_TEST_NE(address, 0);

        bool isZero = ntscfg::TestMemoryUtil::check0s(address, size);
        NTSCFG_TEST_TRUE(isZero);

        ntscfg::TestMemoryUtil::writeFaceFeed(address, size);

        bool isValid = ntscfg::TestMemoryUtil::checkFaceFeed(address, size);
        NTSCFG_TEST_TRUE(isValid);
    }

    // Ensure that two pages are still in use.

    NTSCFG_TEST_EQ(globalAllocator->numPagesInUse(), 2);

    // Allocate one whole page, ensure it is zeroed, then read and write a
    // pattern to that memory.

    {
        const bsl::size_t size = pageSize;

        void* address = globalAllocator->allocate(size);
        NTSCFG_TEST_NE(address, 0);

        bool isZero = ntscfg::TestMemoryUtil::check0s(address, size);
        NTSCFG_TEST_TRUE(isZero);

        ntscfg::TestMemoryUtil::writeDeadBeef(address, size);

        bool isValid = ntscfg::TestMemoryUtil::checkDeadBeef(address, size);
        NTSCFG_TEST_TRUE(isValid);
    }

    // Ensure that three pages are now in use.

    NTSCFG_TEST_EQ(globalAllocator->numPagesInUse(), 3);

    // Allocate two whole pages, ensure they are zeroed, then read and write a
    // pattern to that memory.

    {
        const bsl::size_t size = 2 * pageSize;

        void* address = globalAllocator->allocate(size);
        NTSCFG_TEST_NE(address, 0);

        bool isZero = ntscfg::TestMemoryUtil::check0s(address, size);
        NTSCFG_TEST_TRUE(isZero);

        ntscfg::TestMemoryUtil::writeFaceFeed(address, size);

        bool isValid = ntscfg::TestMemoryUtil::checkFaceFeed(address, size);
        NTSCFG_TEST_TRUE(isValid);
    }

    // Ensure that five pages are now in use.

    NTSCFG_TEST_EQ(globalAllocator->numPagesInUse(), 5);
}

}  // close namespace ntcs
}  // close namespace BloombergLP
