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

#include <ntcs_blobbufferfactory.h>

#include <ntccfg_test.h>
#include <ntci_log.h>

#include <bdlbb_blob.h>
#include <bdlbb_pooledblobbufferfactory.h>

#include <bslmt_barrier.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadgroup.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_stopwatch.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>

using namespace BloombergLP;

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

namespace test {

void logAfterAllocation(const ntcs::BlobBufferFactory& blobBufferFactory)
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

void logAfterAllocation(const ntcs::BlobBufferPool& blobBufferPool)
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

}  // close namespace test

NTCCFG_TEST_CASE(1)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        const int BLOB_BUFFER_SIZE = 4096;

        ntcs::BlobBufferFactory blobBufferFactory(BLOB_BUFFER_SIZE, &ta);

        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

        {
            bdlbb::BlobBuffer blobBuffer1;

            {
                BSLS_LOG_TRACE("Allocating blob buffer 1");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

                blobBuffer1 = blobBuffer;
            }

            BSLS_LOG_TRACE("Destroying blob buffer 1");
        }

        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

        {
            bdlbb::BlobBuffer blobBuffer1;

            {
                BSLS_LOG_TRACE("Allocating blob buffer 1");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

                blobBuffer1 = blobBuffer;
            }

            BSLS_LOG_TRACE("Destroying blob buffer 1");
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        const int BLOB_BUFFER_SIZE = 4096;

        ntcs::BlobBufferFactory blobBufferFactory(BLOB_BUFFER_SIZE, &ta);

        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

        {
            bdlbb::BlobBuffer blobBuffer1;
            bdlbb::BlobBuffer blobBuffer2;

            {
                BSLS_LOG_TRACE("Allocating blob buffer 1");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

                blobBuffer1 = blobBuffer;
            }

            {
                BSLS_LOG_TRACE("Allocating blob buffer 2");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

                blobBuffer2 = blobBuffer;
            }

            BSLS_LOG_TRACE("Destroying blob buffer 2");
            BSLS_LOG_TRACE("Destroying blob buffer 1");
        }

        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 2);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

        {
            bdlbb::BlobBuffer blobBuffer1;
            bdlbb::BlobBuffer blobBuffer2;

            {
                BSLS_LOG_TRACE("Allocating blob buffer 1");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

                blobBuffer1 = blobBuffer;
            }

            {
                BSLS_LOG_TRACE("Allocating blob buffer 2");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

                blobBuffer2 = blobBuffer;
            }

            BSLS_LOG_TRACE("Destroying blob buffer 2");
            BSLS_LOG_TRACE("Destroying blob buffer 1");
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        const int BLOB_BUFFER_SIZE = 4096;

        ntcs::BlobBufferFactory blobBufferFactory(BLOB_BUFFER_SIZE, &ta);

        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

        {
            bdlbb::BlobBuffer blobBuffer1;
            bdlbb::BlobBuffer blobBuffer2;
            bdlbb::BlobBuffer blobBuffer3;

            {
                BSLS_LOG_TRACE("Allocating blob buffer 1");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

                blobBuffer1 = blobBuffer;
            }

            {
                BSLS_LOG_TRACE("Allocating blob buffer 2");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

                blobBuffer2 = blobBuffer;
            }

            {
                BSLS_LOG_TRACE("Allocating blob buffer 3");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 3);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

                blobBuffer3 = blobBuffer;
            }

            BSLS_LOG_TRACE("Destroying blob buffer 3");
            BSLS_LOG_TRACE("Destroying blob buffer 2");
            BSLS_LOG_TRACE("Destroying blob buffer 1");
        }

        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 3);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

        {
            bdlbb::BlobBuffer blobBuffer1;
            bdlbb::BlobBuffer blobBuffer2;
            bdlbb::BlobBuffer blobBuffer3;

            {
                BSLS_LOG_TRACE("Allocating blob buffer 1");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 2);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

                blobBuffer1 = blobBuffer;
            }

            {
                BSLS_LOG_TRACE("Allocating blob buffer 2");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

                blobBuffer2 = blobBuffer;
            }

            {
                BSLS_LOG_TRACE("Allocating blob buffer 3");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 3);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

                blobBuffer3 = blobBuffer;
            }

            BSLS_LOG_TRACE("Destroying blob buffer 3");
            BSLS_LOG_TRACE("Destroying blob buffer 2");
            BSLS_LOG_TRACE("Destroying blob buffer 1");
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        const int BLOB_BUFFER_SIZE = 4096;

        ntcs::BlobBufferPool blobBufferFactory(BLOB_BUFFER_SIZE, &ta);

        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

        {
            bdlbb::BlobBuffer blobBuffer1;

            {
                BSLS_LOG_TRACE("Allocating blob buffer 1");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

                blobBuffer1 = blobBuffer;
            }

            BSLS_LOG_TRACE("Destroying blob buffer 1");
        }

        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

        {
            bdlbb::BlobBuffer blobBuffer1;

            {
                BSLS_LOG_TRACE("Allocating blob buffer 1");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

                blobBuffer1 = blobBuffer;
            }

            BSLS_LOG_TRACE("Destroying blob buffer 1");
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(5)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        const int BLOB_BUFFER_SIZE = 4096;

        ntcs::BlobBufferPool blobBufferFactory(BLOB_BUFFER_SIZE, &ta);

        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

        {
            bdlbb::BlobBuffer blobBuffer1;
            bdlbb::BlobBuffer blobBuffer2;

            {
                BSLS_LOG_TRACE("Allocating blob buffer 1");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

                blobBuffer1 = blobBuffer;
            }

            {
                BSLS_LOG_TRACE("Allocating blob buffer 2");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

                blobBuffer2 = blobBuffer;
            }

            BSLS_LOG_TRACE("Destroying blob buffer 2");
            BSLS_LOG_TRACE("Destroying blob buffer 1");
        }

        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 2);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

        {
            bdlbb::BlobBuffer blobBuffer1;
            bdlbb::BlobBuffer blobBuffer2;

            {
                BSLS_LOG_TRACE("Allocating blob buffer 1");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

                blobBuffer1 = blobBuffer;
            }

            {
                BSLS_LOG_TRACE("Allocating blob buffer 2");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

                blobBuffer2 = blobBuffer;
            }

            BSLS_LOG_TRACE("Destroying blob buffer 2");
            BSLS_LOG_TRACE("Destroying blob buffer 1");
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(6)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        const int BLOB_BUFFER_SIZE = 4096;

        ntcs::BlobBufferPool blobBufferFactory(BLOB_BUFFER_SIZE, &ta);

        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBytesInUse(), 0);

        {
            bdlbb::BlobBuffer blobBuffer1;
            bdlbb::BlobBuffer blobBuffer2;
            bdlbb::BlobBuffer blobBuffer3;

            {
                BSLS_LOG_TRACE("Allocating blob buffer 1");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 1);

                blobBuffer1 = blobBuffer;
            }

            {
                BSLS_LOG_TRACE("Allocating blob buffer 2");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 2);

                blobBuffer2 = blobBuffer;
            }

            {
                BSLS_LOG_TRACE("Allocating blob buffer 3");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 3);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

                blobBuffer3 = blobBuffer;
            }

            BSLS_LOG_TRACE("Destroying blob buffer 3");
            BSLS_LOG_TRACE("Destroying blob buffer 2");
            BSLS_LOG_TRACE("Destroying blob buffer 1");
        }

        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 0);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 3);
        NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

        {
            bdlbb::BlobBuffer blobBuffer1;
            bdlbb::BlobBuffer blobBuffer2;
            bdlbb::BlobBuffer blobBuffer3;

            {
                BSLS_LOG_TRACE("Allocating blob buffer 1");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 1);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 2);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

                blobBuffer1 = blobBuffer;
            }

            {
                BSLS_LOG_TRACE("Allocating blob buffer 2");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 2);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 1);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

                blobBuffer2 = blobBuffer;
            }

            {
                BSLS_LOG_TRACE("Allocating blob buffer 3");

                bdlbb::BlobBuffer blobBuffer;
                blobBufferFactory.allocate(&blobBuffer);

                test::logAfterAllocation(blobBufferFactory);

                NTCCFG_TEST_NE(blobBuffer.data(), 0);
                NTCCFG_TEST_EQ(blobBuffer.size(), BLOB_BUFFER_SIZE);

                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAllocated(), 3);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersAvailable(), 0);
                NTCCFG_TEST_EQ(blobBufferFactory.numBuffersPooled(), 3);

                blobBuffer3 = blobBuffer;
            }

            BSLS_LOG_TRACE("Destroying blob buffer 3");
            BSLS_LOG_TRACE("Destroying blob buffer 2");
            BSLS_LOG_TRACE("Destroying blob buffer 1");
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

namespace test {

void work(bdlbb::BlobBufferFactory* blobBufferFactory,
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

}  // close namespace test

NTCCFG_TEST_CASE(7)
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
            bdlf::BindUtil::bind(&test::work,
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

NTCCFG_TEST_CASE(8)
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
            bdlf::BindUtil::bind(&test::work,
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

NTCCFG_TEST_CASE(9)
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
            bdlf::BindUtil::bind(&test::work,
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

namespace test {
namespace case10 {

void* allocate(bsl::size_t alignment)
{
    return reinterpret_cast<void*>(0 + alignment);
}

void free(void* address)
{
    // Empty
}

template <unsigned ALIGNMENT>
void execute(bsl::size_t maxTag)
{
    NTCI_LOG_CONTEXT();

    typedef ntcs::BlobBufferPoolHandleUtil<void, ALIGNMENT> Util;

    NTCCFG_TEST_EQ(Util::maxTag(), maxTag);

    bsl::size_t alignment = ALIGNMENT;
    {
        bsl::size_t tag = 0;
        while (true) {
            bool isValid = Util::isValid(tag);
            if (tag <= maxTag) {
                NTCCFG_TEST_TRUE(isValid);
                NTCI_LOG_DEBUG("Align %zu tag %zu = 1",
                               (bsl::size_t)(alignment),
                               (bsl::size_t)(tag));
            }
            else {
                NTCCFG_TEST_FALSE(isValid);
                NTCI_LOG_DEBUG("Align %zu tag %zu = 0",
                               (bsl::size_t)(alignment),
                               (bsl::size_t)(tag));
                break;
            }

            ++tag;
        }
    }

    void*       memory   = test::case10::allocate(alignment);
    void*       original = memory;
    void*       ptr      = 0;
    bsl::size_t tag      = 0;

    NTCCFG_TEST_EQ(reinterpret_cast<bsl::size_t>(memory) % alignment, 0);

    ptr = Util::getPtr(memory);
    tag = Util::getTag(memory);

    NTCCFG_TEST_EQ(ptr, memory);
    NTCCFG_TEST_EQ(tag, 0);

    for (bsl::size_t newTag = 1; newTag <= maxTag; ++newTag) {
        Util::setTag(&memory, newTag);

        ptr = Util::getPtr(memory);
        tag = Util::getTag(memory);

        NTCCFG_TEST_EQ(ptr, original);
        NTCCFG_TEST_EQ(tag, newTag);
    }

    ptr = Util::getPtr(memory);
    tag = Util::getTag(memory);

    NTCCFG_TEST_EQ(ptr, original);
    NTCCFG_TEST_EQ(tag, maxTag);

    for (bsl::size_t newPtrMultiple = 2; newPtrMultiple <= 1024;
         ++newPtrMultiple)
    {
        void* newPtr = reinterpret_cast<void*>(
            reinterpret_cast<bsl::size_t>(original) * newPtrMultiple);

        Util::setPtr(&memory, newPtr);

        ptr = Util::getPtr(memory);
        tag = Util::getTag(memory);

        NTCCFG_TEST_EQ(ptr, newPtr);
        NTCCFG_TEST_EQ(tag, maxTag);
    }

    Util::set(&memory, original, 0);

    ptr = Util::getPtr(memory);
    tag = Util::getTag(memory);

    NTCCFG_TEST_EQ(ptr, original);
    NTCCFG_TEST_EQ(tag, 0);

    test::case10::free(memory);
}

}  // close namespace 'case10'
}  // close namespace 'test'

NTCCFG_TEST_CASE(10)
{
    test::case10::execute<2>(1);
    test::case10::execute<4>(3);
    test::case10::execute<8>(7);
    test::case10::execute<16>(15);
    test::case10::execute<4096>(4095);
}

namespace test {
namespace case11 {

class Object
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

}  // close namespace case11
}  // close namespace test

NTCCFG_TEST_CASE(11)
{
    using namespace test::case11;

    enum { ALIGNMENT = 4 };

    ntccfg::TestAllocator ta;
    {
        Object* object1 = new (ta) Object();
        NTCCFG_TEST_EQ((bsl::size_t)(bsl::uintptr_t)(object1) % ALIGNMENT, 0);

        Object* object2 = new (ta) Object();
        NTCCFG_TEST_EQ((bsl::size_t)(bsl::uintptr_t)(object2) % ALIGNMENT, 0);

        typedef ntcs::BlobBufferPoolHandle<Object, ALIGNMENT> Handle;

        Handle handle;

        {
            Object*         currentObject;
            Handle::TagType currentTag;

            handle.load(&currentObject, &currentTag);

            NTCCFG_TEST_EQ(currentObject, 0);
            NTCCFG_TEST_EQ(currentTag, 0);
        }

        {
            Object*         previousObject;
            Handle::TagType previousTag;

            handle.swap(&previousObject, &previousTag, object1, 1);

            NTCCFG_TEST_EQ(previousObject, 0);
            NTCCFG_TEST_EQ(previousTag, 0);
        }

        {
            Object*         currentObject;
            Handle::TagType currentTag;

            handle.load(&currentObject, &currentTag);

            NTCCFG_TEST_EQ(currentObject, object1);
            NTCCFG_TEST_EQ(currentTag, 1);
        }

        handle.store(0, 0);

        {
            Object*         currentObject;
            Handle::TagType currentTag;

            handle.load(&currentObject, &currentTag);

            NTCCFG_TEST_EQ(currentObject, 0);
            NTCCFG_TEST_EQ(currentTag, 0);
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

            NTCCFG_TEST_TRUE(result);

            NTCCFG_TEST_EQ(previousObject, 0);
            NTCCFG_TEST_EQ(previousTag, 0);
        }

        {
            Object*         currentObject;
            Handle::TagType currentTag;

            handle.load(&currentObject, &currentTag);

            NTCCFG_TEST_EQ(currentObject, object1);
            NTCCFG_TEST_EQ(currentTag, 1);
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

            NTCCFG_TEST_FALSE(result);

            NTCCFG_TEST_EQ(previousObject, object1);
            NTCCFG_TEST_EQ(previousTag, 2);
        }

        {
            Object*         currentObject;
            Handle::TagType currentTag;

            handle.load(&currentObject, &currentTag);

            NTCCFG_TEST_EQ(currentObject, object1);
            NTCCFG_TEST_EQ(currentTag, 2);
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

            NTCCFG_TEST_FALSE(result);

            NTCCFG_TEST_EQ(previousObject, object1);
            NTCCFG_TEST_EQ(previousTag, 2);
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

            NTCCFG_TEST_TRUE(result);

            NTCCFG_TEST_EQ(previousObject, object1);
            NTCCFG_TEST_EQ(previousTag, 2);
        }

        {
            Object*         currentObject;
            Handle::TagType currentTag;

            handle.load(&currentObject, &currentTag);

            NTCCFG_TEST_EQ(currentObject, object2);
            NTCCFG_TEST_EQ(currentTag, 3);
        }

        ta.deleteObject(object1);
        ta.deleteObject(object2);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);

    NTCCFG_TEST_REGISTER(4);
    NTCCFG_TEST_REGISTER(5);
    NTCCFG_TEST_REGISTER(6);

    NTCCFG_TEST_REGISTER(7);
    NTCCFG_TEST_REGISTER(8);
    NTCCFG_TEST_REGISTER(9);

    NTCCFG_TEST_REGISTER(10);
}
NTCCFG_TEST_DRIVER_END;
