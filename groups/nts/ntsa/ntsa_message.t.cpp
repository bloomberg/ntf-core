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

#include <ntsa_message.h>
#include <ntscfg_test.h>
#include <bslma_testallocator.h>
#include <bsl_cstdint.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_unordered_set.h>

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

const char s_buffer[1] = {0};

const void* constBufferData(bsl::size_t bufferIndex)
{
    return &s_buffer[0] + bufferIndex;
}

bsl::size_t constBufferSize(bsl::size_t bufferIndex)
{
    return 10 * bufferIndex;
}

void* mutableBufferData(bsl::size_t bufferIndex)
{
    return const_cast<char*>(&s_buffer[0]) + bufferIndex;
}

bsl::size_t mutableBufferSize(bsl::size_t bufferIndex)
{
    return 100 * bufferIndex;
}

}  // close namespace 'test'

NTSCFG_TEST_CASE(1)
{
    // Concern: The semantics of 'ntsa::ConstMessage' are correct.
    // Plan:

    const bsl::size_t MAX_RESET_ITERATION = 2;
    const bsl::size_t MAX_BUFFER_INDEX    = 64;

    ntscfg::TestAllocator ta;
    {
        ntsa::ConstMessage constMessage(&ta);

        for (bsl::size_t resetIteration = 0; resetIteration < 2;
             ++resetIteration)
        {
            NTSCFG_TEST_EQ(constMessage.endpoint(), ntsa::Endpoint());
            NTSCFG_TEST_EQ(constMessage.numBuffers(), 0);
            NTSCFG_TEST_EQ(constMessage.size(), 0);
            NTSCFG_TEST_EQ(constMessage.capacity(), 0);

            bsl::vector<ntsa::ConstBuffer> constBufferArray;

            for (bsl::size_t bufferIndex = 0; bufferIndex < MAX_BUFFER_INDEX;
                 ++bufferIndex)
            {
                ntsa::ConstBuffer constBuffer(
                    test::constBufferData(bufferIndex),
                    test::constBufferSize(bufferIndex));

                constMessage.appendBuffer(constBuffer);
                constBufferArray.push_back(constBuffer);

                bsl::size_t expectedNumBuffers = bufferIndex + 1;

                NTSCFG_TEST_EQ(constMessage.numBuffers(), expectedNumBuffers);

                bsl::size_t expectedCapacity = 0;
                for (bsl::size_t existingBufferIndex = 0;
                     existingBufferIndex <= bufferIndex;
                     ++existingBufferIndex)
                {
                    NTSCFG_TEST_EQ(
                        constMessage.buffer(existingBufferIndex).data(),
                        constBufferArray[existingBufferIndex].data());
                    NTSCFG_TEST_EQ(
                        constMessage.buffer(existingBufferIndex).size(),
                        constBufferArray[existingBufferIndex].size());

                    expectedCapacity +=
                        test::constBufferSize(existingBufferIndex);
                }

                bsl::size_t expectedSize = expectedCapacity;

                NTSCFG_TEST_EQ(constMessage.size(), expectedSize);
                NTSCFG_TEST_EQ(constMessage.capacity(), expectedCapacity);
            }

            constMessage.reset();
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(2)
{
    // Concern: The semantics of 'ntsa::MutableMessage' are correct.
    // Plan:

    const bsl::size_t MAX_RESET_ITERATION = 2;
    const bsl::size_t MAX_BUFFER_INDEX    = 64;

    ntscfg::TestAllocator ta;
    {
        ntsa::MutableMessage mutableMessage(&ta);

        for (bsl::size_t resetIteration = 0; resetIteration < 2;
             ++resetIteration)
        {
            NTSCFG_TEST_EQ(mutableMessage.endpoint(), ntsa::Endpoint());
            NTSCFG_TEST_EQ(mutableMessage.numBuffers(), 0);
            NTSCFG_TEST_EQ(mutableMessage.size(), 0);
            NTSCFG_TEST_EQ(mutableMessage.capacity(), 0);

            bsl::vector<ntsa::MutableBuffer> mutableBufferArray;

            for (bsl::size_t bufferIndex = 0; bufferIndex < MAX_BUFFER_INDEX;
                 ++bufferIndex)
            {
                ntsa::MutableBuffer mutableBuffer(
                    test::mutableBufferData(bufferIndex),
                    test::mutableBufferSize(bufferIndex));

                mutableMessage.appendBuffer(mutableBuffer);
                mutableBufferArray.push_back(mutableBuffer);

                bsl::size_t expectedNumBuffers = bufferIndex + 1;

                NTSCFG_TEST_EQ(mutableMessage.numBuffers(),
                               expectedNumBuffers);

                bsl::size_t expectedCapacity = 0;
                for (bsl::size_t existingBufferIndex = 0;
                     existingBufferIndex <= bufferIndex;
                     ++existingBufferIndex)
                {
                    NTSCFG_TEST_EQ(
                        mutableMessage.buffer(existingBufferIndex).data(),
                        mutableBufferArray[existingBufferIndex].data());
                    NTSCFG_TEST_EQ(
                        mutableMessage.buffer(existingBufferIndex).size(),
                        mutableBufferArray[existingBufferIndex].size());

                    expectedCapacity +=
                        test::mutableBufferSize(existingBufferIndex);
                }

                bsl::size_t expectedSize = 0;

                NTSCFG_TEST_EQ(mutableMessage.size(), expectedSize);
                NTSCFG_TEST_EQ(mutableMessage.capacity(), expectedCapacity);
            }

            mutableMessage.reset();
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
}
NTSCFG_TEST_DRIVER_END;
