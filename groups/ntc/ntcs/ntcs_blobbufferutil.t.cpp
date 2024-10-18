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
BSLS_IDENT_RCSID(ntcs_blobbufferutil_t_cpp, "$Id$ $CSID$")

#include <ntcs_blobbufferutil.h>

#include <ntci_log.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::BlobBufferUtil'.
class BlobBufferUtilTest
{
    enum Constant { k_DEFAULT_MAX_RECEIVE_SIZE = 1024 * 1024 * 32 };

  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifyCase3();
};

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferUtilTest::verifyCase1)
{
    // clang-format off

    struct Data {
        size_t d_size;
        size_t d_capacity;
        size_t d_lowWatermark;
        size_t d_minReceiveSize;
        size_t d_numBytesToAllocate;
    } DATA[] = {
  // Legend:
  //
  // LWM: Low Watermark
  // NBA: Number of bytes to be available to use for reading
  // MRS: Minimum receive size (i.e., the lower bound on NBA)
  // NBR: Number of bytes required to allocate to get the necessary NBA
  //

  // Case A: LWM <= capacity, NBA >= MRS
  //
  //              |<---- MRS ---->|
  //              |<------------ NBA ------------->|
  // +============+================================+
  // |    Size    |            Capacity            |
  // +============+================================+
  //        ^
  //        |
  //       LWM

        {10, 100,   5,  50,   0},

 // Case B: LWM <= capacity, NBA < MRS
  //
  //              |<--------------------- MRS ------------------>|
  //              |<----------- NBA -------------->|
  // +============+================================+`~`~`~`~`~`~`|
  // |    Size    |            Capacity            |     NBR     |
  // +============+================================+`~`~`~`~`~`~`'
  //        ^
  //        |
  //       LWM

        {10, 100,   5, 150,  60},

 // Case 1: LWM <= capacity, NBA >= MRS
  //
  //              |<---- MRS ---->|
  //              |<------------ NBA ------------->|
  // +============+================================+
  // |    Size    |            Capacity            |
  // +============+================================+
  //                                        ^
  //                                        |
  //                                       LWM

        {10, 100,  75,  50,   0},

 // Case 2: LWM <= capacity, NBA < MRS
  //
  //              |<--------------------- MRS ------------------>|
  //              |<----------- NBA -------------->|
  // +============+================================+`~`~`~`~`~`~`|
  // |    Size    |            Capacity            |     NBR     |
  // +============+================================+`~`~`~`~`~`~`'
  //                                        ^
  //                                        |
  //                                       LWM

        {10, 100,  75, 150,  60},

 // Case 3: LWM > capacity, NBA >= MRS
  //
  //              |<---- MRS ---->|
  //              |<-------------------- NBA ------------------->|
  // +============+================================+`~`~`~`~`~`~`|
  // |    Size    |            Capacity            |     NBR     |
  // +============+================================+`~`~`~`~`~`~`'
  //                                                             ^
  //                                                             |
  //                                                            LWM

        {10, 100, 300,  75, 200},

 // Case 4: LWM > capacity, NBA < MRS
  //
  //              |<-------------------- MRS ------------------->|
  //              |<----------------- NBA ---------------->|
  // +============+================================+`~`~`~`~`~`~`|
  // |    Size    |            Capacity            |     NBR     |
  // +============+================================+`~`~`~`~`~`~`'
  //                                                       ^
  //                                                       |
  //                                                      LWM

        {10, 100, 125, 150,  60}
    };

    enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

    for (int i = 0; i < NUM_DATA; ++i) {
        size_t result = ntcs::BlobBufferUtil::calculateNumBytesToAllocate(
            DATA[i].d_size,
            DATA[i].d_capacity,
            DATA[i].d_lowWatermark,
            DATA[i].d_minReceiveSize,
            BlobBufferUtilTest::k_DEFAULT_MAX_RECEIVE_SIZE);

        NTSCFG_TEST_GE(result + (DATA[i].d_capacity - DATA[i].d_size),
                       DATA[i].d_minReceiveSize);

        NTSCFG_TEST_EQ(result, DATA[i].d_numBytesToAllocate);
    }

    // clang-format on
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferUtilTest::verifyCase2)
{
    // clang-format off

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    const bsl::size_t k_MIN_CAPACITY     = 0;
    const bsl::size_t k_MAX_CAPACITY     = 8;

    for (bsl::size_t capacity  = k_MIN_CAPACITY;
                     capacity <= k_MAX_CAPACITY;
                   ++capacity)
    {
        for (bsl::size_t size = 0; size <= capacity; ++size) {
            for (bsl::size_t lowWatermark  = 1;
                             lowWatermark <= 2 * capacity;
                           ++lowWatermark)
            {
                for (bsl::size_t minReceiveSize  = 1;
                                 minReceiveSize <= 2 * lowWatermark;
                               ++minReceiveSize)
                {
                    for (bsl::size_t maxReceiveSize  = 1;
                                     maxReceiveSize <= 2 * minReceiveSize;
                                   ++maxReceiveSize)
                    {
                        NTCI_LOG_TRACE("Enter"
                                       ": size = %zu"
                                       ", capacity = %zu"
                                       ", lowWatermark = %zu"
                                       ", minReceiveSize = %zu"
                                       ", maxReceiveSize = %zu",
                                       size,
                                       capacity,
                                       lowWatermark,
                                       minReceiveSize,
                                       maxReceiveSize);

                        size_t result =
                            ntcs::BlobBufferUtil
                                ::calculateNumBytesToAllocate(
                                    size,
                                    capacity,
                                    lowWatermark,
                                    minReceiveSize,
                                    maxReceiveSize);

                        NTCI_LOG_TRACE("Leave"
                                       ": size = %zu"
                                       ", capacity = %zu"
                                       ", lowWatermark = %zu"
                                       ", minReceiveSize = %zu"
                                       ", maxReceiveSize = %zu"
                                       ", numBytesToAllocate = %zu",
                                       size,
                                       capacity,
                                       lowWatermark,
                                       minReceiveSize,
                                       maxReceiveSize,
                                       result);

                        NTSCFG_TEST_LE(size, capacity);

                        NTSCFG_TEST_GE(
                            result + (capacity - size), 
                            bsl::min(minReceiveSize, maxReceiveSize));

                        NTSCFG_TEST_LE(result, maxReceiveSize);
                    }
                }
            }
        }
    }

    // clang-format on
}

NTSCFG_TEST_FUNCTION(ntcs::BlobBufferUtilTest::verifyCase3)
{
    ntsa::Error error;

    bdlbb::PooledBlobBufferFactory blobBufferFactory(4, NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    blob.setLength(100);
    blob.setLength(10);

    NTSCFG_TEST_EQ(blob.length(), 10);
    NTSCFG_TEST_EQ(blob.totalSize(), 100);

    ntcs::BlobBufferUtil::reserveCapacity(
        &blob,
        &blobBufferFactory,
        0,
        118,
        1,
        BlobBufferUtilTest::k_DEFAULT_MAX_RECEIVE_SIZE);

    NTSCFG_TEST_EQ(blob.length(), 10);
    NTSCFG_TEST_EQ(blob.totalSize(), 120);
}

}  // close namespace ntcs
}  // close namespace BloombergLP
