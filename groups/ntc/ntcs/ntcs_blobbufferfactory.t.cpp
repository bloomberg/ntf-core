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
BSLS_IDENT_RCSID(ntcs_blobbufferfactory_t_cpp, "$Id$ $CSID$")

#include <ntcs_blobbufferfactory.h>

#include <ntci_log.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::BlobBufferFactory'.
class BlobBufferFactoryTest
{
    class Object;

    static void logAfterAllocation(
        const ntcs::BlobBufferFactory& blobBufferFactory);

    static void logAfterAllocation(const ntcs::BlobBufferPool& blobBufferPool);

    static void* allocate(bsl::size_t alignment);

    static void free(void* address);

    static void work(bdlbb::BlobBufferFactory* blobBufferFactory,
                     bslmt::Barrier*           barrier,
                     bsl::size_t               numIterations,
                     bsl::size_t               maxThreads);

    template <unsigned ALIGNMENT>
    static void verifyTagSize(bsl::size_t maxTag);

  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifyCase3();

    // TODO
    static void verifyCase4();

    // TODO
    static void verifyCase5();

    // TODO
    static void verifyCase6();

    // TODO
    static void verifyCase7();

    // TODO
    static void verifyCase8();

    // TODO
    static void verifyCase9();

    // TODO
    static void verifyCase10();

    // TODO
    static void verifyCase11();
};

class BlobBufferFactoryTest::Object
{
    int d_value;

  public:
    Object()
    : d_value(0)
    {
    }

    void setValue(int value)
    {
        d_value = value;
    }

    int value() const
    {
        return d_value;
    }
};

void BlobBufferFactoryTest::logAfterAllocation(
    const ntcs::BlobBufferFactory& blobBufferFactory)
{
    BSLS_LOG_TRACE("Allocated blob buffer: "
                   "numBuffersAllocated = %d, "
                   "numBuffersAvailable = %d, "
                   "numBuffersPooled = %d, "
                   "numBytesInUse = %d",
                   (int)(blobBufferFactory.numBuffersAllocated()),
                   (int)(blobBufferFactory.numBuffersAvailable()),
                   (int)(blobBufferFactory.numBuffersPooled()),
                   (int)(blobBufferFactory.numBytesInUse()));
}

void BlobBufferFactoryTest::logAfterAllocation(
    const ntcs::BlobBufferPool& blobBufferPool)
{
    BSLS_LOG_TRACE("Allocated blob buffer: "
                   "numBuffersAllocated = %d, "
                   "numBuffersAvailable = %d, "
                   "numBuffersPooled = %d, "
                   "numBytesInUse = %d",
                   (int)(blobBufferPool.numBuffersAllocated()),
                   (int)(blobBufferPool.numBuffersAvailable()),
                   (int)(blobBufferPool.numBuffersPooled()),
                   (int)(blobBufferPool.numBytesInUse()));
}

void* BlobBufferFactoryTest::allocate(bsl::size_t alignment)
{
    return reinterpret_cast<void*>(0 + alignment);
}

void BlobBufferFactoryTest::free(void* address)
{
}

void BlobBufferFactoryTest::work(bdlbb::BlobBufferFactory* blobBufferFactory,
                                 bslmt::Barrier*           barrier,
                                 bsl::size_t               numIterations,
                                 bsl::size_t               maxThreads)
{
    barrier->wait();
    barrier->wait();

    for (bsl::size_t iteration = 0; iteration < numIterations; ++iteration) {
        bdlbb::BlobBuffer blobBuffer;
        blobBufferFactory->allocate(&blobBuffer);
        bsl::memcpy(blobBuffer.data(), &iteration, sizeof iteration);
        // bslmt::ThreadUtil::yield();
    }

    barrier->wait();
    barrier->wait();
}

template <unsigned ALIGNMENT>
void BlobBufferFactoryTest::verifyTagSize(bsl::size_t maxTag)
{
    NTCI_LOG_CONTEXT();

    typedef ntcs::BlobBufferPoolHandleUtil<void, ALIGNMENT> Util;

    NTSCFG_TEST_EQ(Util::maxTag(), maxTag);

    bsl::size_t alignment = ALIGNMENT;
    {
        bsl::size_t tag = 0;
        while (true) {
            bool isValid = Util::isValid(tag);
            if (tag <= maxTag) {
                NTSCFG_TEST_TRUE(isValid);
                NTCI_LOG_DEBUG("Align %zu tag %zu = 1",
                               (bsl::size_t)(alignment),
                               (bsl::size_t)(tag));
            }
            else {
                NTSCFG_TEST_FALSE(isValid);
                NTCI_LOG_DEBUG("Align %zu tag %zu = 0",
                               (bsl::size_t)(alignment),
                               (bsl::size_t)(tag));
                break;
            }

            ++tag;
        }
    }

    void*       memory   = BlobBufferFactoryTest::allocate(alignment);
    void*       original = memory;
    void*       ptr      = 0;
    bsl::size_t tag      = 0;

    NTSCFG_TEST_EQ(reinterpret_cast<bsl::size_t>(memory) % alignment, 0);

    ptr = Util::getPtr(memory);
    tag = Util::getTag(memory);

    NTSCFG_TEST_EQ(ptr, memory);
    NTSCFG_TEST_EQ(tag, 0);

    for (bsl::size_t newTag = 1; newTag <= maxTag; ++newTag) {
        Util::setTag(&memory, newTag);

        ptr = Util::getPtr(memory);
        tag = Util::getTag(memory);

        NTSCFG_TEST_EQ(ptr, original);
        NTSCFG_TEST_EQ(tag, newTag);
    }

    ptr = Util::getPtr(memory);
    tag = Util::getTag(memory);

    NTSCFG_TEST_EQ(ptr, original);
    NTSCFG_TEST_EQ(tag, maxTag);

    for (bsl::size_t newPtrMultiple = 2; newPtrMultiple <= 1024;
         ++newPtrMultiple)
    {
        void* newPtr = reinterpret_cast<void*>(
            reinterpret_cast<bsl::size_t>(original) * newPtrMultiple);

        Util::setPtr(&memory, newPtr);

        ptr = Util::getPtr(memory);
        tag = Util::getTag(memory);

        NTSCFG_TEST_EQ(ptr, newPtr);
        NTSCFG_TEST_EQ(tag, maxTag);
    }

    Util::set(&memory, original, 0);

    ptr = Util::getPtr(memory);
    tag = Util::getTag(memory);

    NTSCFG_TEST_EQ(ptr, original);
    NTSCFG_TEST_EQ(tag, 0);

    BlobBufferFactoryTest::free(memory);
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase1)
{
    // Concern:
    // Plan:

    const int BLOB_BUFFER_SIZE = 4096;

    ntcs::BlobBufferFactory blobBufferFactory(BLOB_BUFFER_SIZE,
                                              NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

    {
        bdlbb::BlobBuffer blobBuffer1;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

            blobBuffer1 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

    {
        bdlbb::BlobBuffer blobBuffer1;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

            blobBuffer1 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase2)
{
    // Concern:
    // Plan:

    const int BLOB_BUFFER_SIZE = 4096;

    ntcs::BlobBufferFactory blobBufferFactory(BLOB_BUFFER_SIZE,
                                              NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

    {
        bdlbb::BlobBuffer blobBuffer1;
        bdlbb::BlobBuffer blobBuffer2;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

            blobBuffer1 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 2");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

            blobBuffer2 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 2");
        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 2);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

    {
        bdlbb::BlobBuffer blobBuffer1;
        bdlbb::BlobBuffer blobBuffer2;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

            blobBuffer1 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 2");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

            blobBuffer2 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 2");
        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase3)
{
    // Concern:
    // Plan:

    const int BLOB_BUFFER_SIZE = 4096;

    ntcs::BlobBufferFactory blobBufferFactory(BLOB_BUFFER_SIZE,
                                              NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

    {
        bdlbb::BlobBuffer blobBuffer1;
        bdlbb::BlobBuffer blobBuffer2;
        bdlbb::BlobBuffer blobBuffer3;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

            blobBuffer1 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 2");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

            blobBuffer2 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 3");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 3);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

            blobBuffer3 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 3");
        BSLS_LOG_TRACE("Destroying blob buffer 2");
        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 3);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

    {
        bdlbb::BlobBuffer blobBuffer1;
        bdlbb::BlobBuffer blobBuffer2;
        bdlbb::BlobBuffer blobBuffer3;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

            blobBuffer1 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 2");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

            blobBuffer2 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 3");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 3);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

            blobBuffer3 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 3");
        BSLS_LOG_TRACE("Destroying blob buffer 2");
        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase4)
{
    // Concern:
    // Plan:

    const int BLOB_BUFFER_SIZE = 4096;

    ntcs::BlobBufferPool blobBufferFactory(BLOB_BUFFER_SIZE,
                                           NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

    {
        bdlbb::BlobBuffer blobBuffer1;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

            blobBuffer1 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

    {
        bdlbb::BlobBuffer blobBuffer1;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

            blobBuffer1 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase5)
{
    // Concern:
    // Plan:

    const int BLOB_BUFFER_SIZE = 4096;

    ntcs::BlobBufferPool blobBufferFactory(BLOB_BUFFER_SIZE,
                                           NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

    {
        bdlbb::BlobBuffer blobBuffer1;
        bdlbb::BlobBuffer blobBuffer2;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

            blobBuffer1 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 2");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

            blobBuffer2 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 2");
        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 2);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

    {
        bdlbb::BlobBuffer blobBuffer1;
        bdlbb::BlobBuffer blobBuffer2;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

            blobBuffer1 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 2");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

            blobBuffer2 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 2");
        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase6)
{
    // Concern:
    // Plan:

    const int BLOB_BUFFER_SIZE = 4096;

    ntcs::BlobBufferPool blobBufferFactory(BLOB_BUFFER_SIZE,
                                           NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

    {
        bdlbb::BlobBuffer blobBuffer1;
        bdlbb::BlobBuffer blobBuffer2;
        bdlbb::BlobBuffer blobBuffer3;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

            blobBuffer1 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 2");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

            blobBuffer2 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 3");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 3);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

            blobBuffer3 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 3");
        BSLS_LOG_TRACE("Destroying blob buffer 2");
        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }

    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 3);
    NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

    {
        bdlbb::BlobBuffer blobBuffer1;
        bdlbb::BlobBuffer blobBuffer2;
        bdlbb::BlobBuffer blobBuffer3;

        {
            BSLS_LOG_TRACE("Allocating blob buffer 1");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

            blobBuffer1 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 2");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

            blobBuffer2 = blobBuffer;
        }

        {
            BSLS_LOG_TRACE("Allocating blob buffer 3");

            bdlbb::BlobBuffer blobBuffer;
            blobBufferFactory.allocate(&blobBuffer);

            BlobBufferFactoryTest::logAfterAllocation(blobBufferFactory);

            NTSCFG_TEST_NE(blobBuffer.data(), 0);
            NTSCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 3);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
            NTSCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

            blobBuffer3 = blobBuffer;
        }

        BSLS_LOG_TRACE("Destroying blob buffer 3");
        BSLS_LOG_TRACE("Destroying blob buffer 2");
        BSLS_LOG_TRACE("Destroying blob buffer 1");
    }
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase7)
{
    bslma::Allocator* allocator = bslma::Default::defaultAllocator();

    // clang-format off
    struct Data {
        bsl::size_t d_numThreads;
        bsl::size_t d_numIterations;
    }
    DATA[] = {
#if NTC_BUILD_WITH_VALGRIND
        { 1,    100000 },
        { 2,     50000 },
        { 4,     25000 },
        { 10,    10000 }
#else
        { 1,  10000000 },
        { 2,   5000000 },
        { 4,   2500000 },
        { 10,  1000000 }
#endif
    };
    // clang-format on

    enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

    bsl::vector<bsls::Stopwatch> stopwatchList(NUM_DATA);

    for (bsl::size_t variation = 0; variation < NUM_DATA; ++variation) {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(64);
        bslmt::Barrier barrier(DATA[variation].d_numThreads + 1);

        bslmt::ThreadGroup threadGroup;
        threadGroup.addThreads(
            bdlf::BindUtil::bind(&BlobBufferFactoryTest::work,
                                 &blobBufferFactory,
                                 &barrier,
                                 DATA[variation].d_numIterations,
                                 DATA[variation].d_numThreads),
            DATA[variation].d_numThreads);

        barrier.wait();
        stopwatchList[variation].start(true);
        barrier.wait();

        barrier.wait();
        stopwatchList[variation].stop();
        barrier.wait();

        threadGroup.joinAll();
    }

    const bsl::size_t W = 15;

    bsl::cout << "bdlbb::PooledBlobBufferFactory" << bsl::endl;
    bsl::cout << bsl::setw(W) << bsl::right << "Threads";
    bsl::cout << bsl::setw(W) << bsl::right << "Iterations";
    bsl::cout << bsl::setw(W) << bsl::right << "Wall";
    bsl::cout << bsl::setw(W) << bsl::right << "CPU";
    bsl::cout << bsl::setw(W) << bsl::right << "User";
    bsl::cout << bsl::setw(W) << bsl::right << "System";
    bsl::cout << bsl::endl;

    for (bsl::size_t variation = 0; variation < NUM_DATA; ++variation) {
        bsl::cout << bsl::setw(W) << bsl::right
                  << DATA[variation].d_numThreads;
        bsl::cout << bsl::setw(W) << bsl::right
                  << DATA[variation].d_numIterations;
        bsl::cout << bsl::setw(W) << bsl::right
                  << stopwatchList[variation].accumulatedWallTime();
        bsl::cout << bsl::setw(W) << bsl::right
                  << stopwatchList[variation].accumulatedUserTime() +
                         stopwatchList[variation].accumulatedSystemTime();
        bsl::cout << bsl::setw(W) << bsl::right
                  << stopwatchList[variation].accumulatedUserTime();
        bsl::cout << bsl::setw(W) << bsl::right
                  << stopwatchList[variation].accumulatedSystemTime();
        bsl::cout << bsl::endl;
    }
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase8)
{
    bslma::Allocator* allocator = bslma::Default::defaultAllocator();

    // clang-format off
    struct Data {
        bsl::size_t d_numThreads;
        bsl::size_t d_numIterations;
    }
    DATA[] = {
#if NTC_BUILD_WITH_VALGRIND
        { 1,    100000 },
        { 2,     50000 },
        { 4,     25000 },
        { 10,    10000 }
#else
        { 1,  10000000 },
        { 2,   5000000 },
        { 4,   2500000 },
        { 10,  1000000 }
#endif
    };
    // clang-format on

    enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

    bsl::vector<bsl::pair<bsls::Stopwatch, bsl::size_t> > stopwatchList(
        NUM_DATA);

    for (bsl::size_t variation = 0; variation < NUM_DATA; ++variation) {
        ntcs::BlobBufferFactory blobBufferFactory(64);
        bslmt::Barrier          barrier(DATA[variation].d_numThreads + 1);

        // MRM: blobBufferFactory.reserve(DATA[variation].d_numThreads + 1);

        bslmt::ThreadGroup threadGroup;
        threadGroup.addThreads(
            bdlf::BindUtil::bind(&BlobBufferFactoryTest::work,
                                 &blobBufferFactory,
                                 &barrier,
                                 DATA[variation].d_numIterations,
                                 DATA[variation].d_numThreads),
            DATA[variation].d_numThreads);

        barrier.wait();
        stopwatchList[variation].first.start(true);
        barrier.wait();

        barrier.wait();
        stopwatchList[variation].first.stop();
        barrier.wait();

        threadGroup.joinAll();

        stopwatchList[variation].second = blobBufferFactory.numBuffersPooled();
    }

    const bsl::size_t W = 15;

    bsl::cout << "ntcs::PooledBlobBufferFactory" << bsl::endl;
    bsl::cout << bsl::setw(W) << bsl::right << "Threads";
    bsl::cout << bsl::setw(W) << bsl::right << "Iterations";
    bsl::cout << bsl::setw(W) << bsl::right << "Wall";
    bsl::cout << bsl::setw(W) << bsl::right << "CPU";
    bsl::cout << bsl::setw(W) << bsl::right << "User";
    bsl::cout << bsl::setw(W) << bsl::right << "System";
    bsl::cout << bsl::setw(W) << bsl::right << "NumPooled";
    bsl::cout << bsl::endl;

    for (bsl::size_t variation = 0; variation < NUM_DATA; ++variation) {
        bsl::cout << bsl::setw(W) << bsl::right
                  << DATA[variation].d_numThreads;
        bsl::cout << bsl::setw(W) << bsl::right
                  << DATA[variation].d_numIterations;
        bsl::cout << bsl::setw(W) << bsl::right
                  << stopwatchList[variation].first.accumulatedWallTime();
        bsl::cout
            << bsl::setw(W) << bsl::right
            << stopwatchList[variation].first.accumulatedUserTime() +
                   stopwatchList[variation].first.accumulatedSystemTime();
        bsl::cout << bsl::setw(W) << bsl::right
                  << stopwatchList[variation].first.accumulatedUserTime();
        bsl::cout << bsl::setw(W) << bsl::right
                  << stopwatchList[variation].first.accumulatedSystemTime();
        bsl::cout << bsl::setw(W) << bsl::right
                  << stopwatchList[variation].second;
        bsl::cout << bsl::endl;
    }
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase9)
{
#if 0
    bslma::Allocator *allocator = bslma::Default::defaultAllocator();

    // clang-format off
    struct Data {
        bsl::size_t d_numThreads;
        bsl::size_t d_numIterations;
    }
    DATA[] = {
#if NTC_BUILD_WITH_VALGRIND
        { 1,    100000 },
        { 2,     50000 },
        { 4,     25000 },
        { 10,    10000 }
#else
        { 1,  10000000 },
        { 2,   5000000 },
        { 4,   2500000 },
        { 10,  1000000 }
#endif
    };
    // clang-format on

    enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

    bsl::vector<bsl::pair<bsls::Stopwatch, bsl::size_t> > stopwatchList(
                                                                     NUM_DATA);

    for (bsl::size_t variation = 0; variation < NUM_DATA; ++variation) {
        ntcs::BlobBufferPool blobBufferFactory(64);
        bslmt::Barrier       barrier(DATA[variation].d_numThreads + 1);

        // MRM: blobBufferFactory.reserve(DATA[variation].d_numThreads + 1);

        bslmt::ThreadGroup threadGroup;
        threadGroup.addThreads(
            bdlf::BindUtil::bind(&BlobBufferFactoryTest::work,
                                 &blobBufferFactory,
                                 &barrier,
                                 DATA[variation].d_numIterations,
                                 DATA[variation].d_numThreads),
            DATA[variation].d_numThreads);

        barrier.wait();
        stopwatchList[variation].first.start(true);
        barrier.wait();

        barrier.wait();
        stopwatchList[variation].first.stop();
        barrier.wait();

        threadGroup.joinAll();

        stopwatchList[variation].second = blobBufferFactory.numBuffersPooled();
    }

    const bsl::size_t W = 15;

    bsl::cout << "ntcs::PooledBlobBufferPool (*)" << bsl::endl;
    bsl::cout << bsl::setw(W) << bsl::right << "Threads";
    bsl::cout << bsl::setw(W) << bsl::right << "Iterations";
    bsl::cout << bsl::setw(W) << bsl::right << "Wall";
    bsl::cout << bsl::setw(W) << bsl::right << "CPU";
    bsl::cout << bsl::setw(W) << bsl::right << "User";
    bsl::cout << bsl::setw(W) << bsl::right << "System";
    bsl::cout << bsl::setw(W) << bsl::right << "NumPooled";
    bsl::cout << bsl::endl;

    for (bsl::size_t variation = 0; variation < NUM_DATA; ++variation) {
        bsl::cout << bsl::setw(W) << bsl::right
                  << DATA[variation].d_numThreads;
        bsl::cout << bsl::setw(W) << bsl::right
                  << DATA[variation].d_numIterations;
        bsl::cout << bsl::setw(W) << bsl::right
                  << stopwatchList[variation].first.accumulatedWallTime();
        bsl::cout
            << bsl::setw(W) << bsl::right
            << stopwatchList[variation].first.accumulatedUserTime() +
                   stopwatchList[variation].first.accumulatedSystemTime();
        bsl::cout << bsl::setw(W) << bsl::right
                  << stopwatchList[variation].first.accumulatedUserTime();
        bsl::cout << bsl::setw(W) << bsl::right
                  << stopwatchList[variation].first.accumulatedSystemTime();
        bsl::cout << bsl::setw(W) << bsl::right
                  << stopwatchList[variation].second;
        bsl::cout << bsl::endl;
    }
#endif
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase10)
{
    BlobBufferFactoryTest::verifyTagSize<2>(1);
    BlobBufferFactoryTest::verifyTagSize<4>(3);
    BlobBufferFactoryTest::verifyTagSize<8>(7);
    BlobBufferFactoryTest::verifyTagSize<16>(15);
    BlobBufferFactoryTest::verifyTagSize<4096>(4095);
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferFactoryTest::verifyCase11)
{
    enum { ALIGNMENT = 4 };

    bslma::Allocator* allocator = NTSCFG_TEST_ALLOCATOR;

    Object* object1 = new (*allocator) Object();
    NTSCFG_TEST_EQ((bsl::size_t)(bsl::uintptr_t)(object1) % ALIGNMENT, 0);

    Object* object2 = new (*allocator) Object();
    NTSCFG_TEST_EQ((bsl::size_t)(bsl::uintptr_t)(object2) % ALIGNMENT, 0);

    typedef ntcs::BlobBufferPoolHandle<Object, ALIGNMENT> Handle;

    Handle handle;

    {
        Object*         currentObject;
        Handle::TagType currentTag;

        handle.load(&currentObject, &currentTag);

        NTSCFG_TEST_EQ(currentObject, 0);
        NTSCFG_TEST_EQ(currentTag, 0);
    }

    {
        Object*         previousObject;
        Handle::TagType previousTag;

        handle.swap(&previousObject, &previousTag, object1, 1);

        NTSCFG_TEST_EQ(previousObject, 0);
        NTSCFG_TEST_EQ(previousTag, 0);
    }

    {
        Object*         currentObject;
        Handle::TagType currentTag;

        handle.load(&currentObject, &currentTag);

        NTSCFG_TEST_EQ(currentObject, object1);
        NTSCFG_TEST_EQ(currentTag, 1);
    }

    handle.store(0, 0);

    {
        Object*         currentObject;
        Handle::TagType currentTag;

        handle.load(&currentObject, &currentTag);

        NTSCFG_TEST_EQ(currentObject, 0);
        NTSCFG_TEST_EQ(currentTag, 0);
    }

    {
        Object*         previousObject;
        Handle::TagType previousTag;

        bool result = handle.testAndSwap(&previousObject,
                                         &previousTag,
                                         0,
                                         0,
                                         object1,
                                         1);

        NTSCFG_TEST_TRUE(result);

        NTSCFG_TEST_EQ(previousObject, 0);
        NTSCFG_TEST_EQ(previousTag, 0);
    }

    {
        Object*         currentObject;
        Handle::TagType currentTag;

        handle.load(&currentObject, &currentTag);

        NTSCFG_TEST_EQ(currentObject, object1);
        NTSCFG_TEST_EQ(currentTag, 1);
    }

    handle.store(object1, 2);

    {
        Object*         previousObject;
        Handle::TagType previousTag;

        bool result = handle.testAndSwap(&previousObject,
                                         &previousTag,
                                         object1,
                                         1,
                                         object2,
                                         2);

        NTSCFG_TEST_FALSE(result);

        NTSCFG_TEST_EQ(previousObject, object1);
        NTSCFG_TEST_EQ(previousTag, 2);
    }

    {
        Object*         currentObject;
        Handle::TagType currentTag;

        handle.load(&currentObject, &currentTag);

        NTSCFG_TEST_EQ(currentObject, object1);
        NTSCFG_TEST_EQ(currentTag, 2);
    }

    {
        Object*         previousObject;
        Handle::TagType previousTag;

        bool result = handle.testAndSwap(&previousObject,
                                         &previousTag,
                                         object2,
                                         2,
                                         object2,
                                         3);

        NTSCFG_TEST_FALSE(result);

        NTSCFG_TEST_EQ(previousObject, object1);
        NTSCFG_TEST_EQ(previousTag, 2);
    }

    {
        Object*         previousObject;
        Handle::TagType previousTag;

        bool result = handle.testAndSwap(&previousObject,
                                         &previousTag,
                                         object1,
                                         2,
                                         object2,
                                         3);

        NTSCFG_TEST_TRUE(result);

        NTSCFG_TEST_EQ(previousObject, object1);
        NTSCFG_TEST_EQ(previousTag, 2);
    }

    {
        Object*         currentObject;
        Handle::TagType currentTag;

        handle.load(&currentObject, &currentTag);

        NTSCFG_TEST_EQ(currentObject, object2);
        NTSCFG_TEST_EQ(currentTag, 3);
    }

    allocator->deleteObject(object1);
    allocator->deleteObject(object2);
}

}  // close namespace ntcs
}  // close namespace BloombergLP
