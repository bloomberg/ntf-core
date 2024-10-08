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
BSLS_IDENT_RCSID(ntsa_message_t_cpp, "$Id$ $CSID$")

#include <ntsa_message.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Message'.
class MessageTest
{
  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

  private:
    static const char s_buffer[1];

    static const void* constBufferData(bsl::size_t bufferIndex);
    static bsl::size_t constBufferSize(bsl::size_t bufferIndex);

    static void*       mutableBufferData(bsl::size_t bufferIndex);
    static bsl::size_t mutableBufferSize(bsl::size_t bufferIndex);
};

NTSCFG_TEST_FUNCTION(ntsa::MessageTest::verifyCase1)
{
    // Concern: The semantics of 'ntsa::ConstMessage' are correct.
    // Plan:

    const bsl::size_t MAX_RESET_ITERATION = 2;
    const bsl::size_t MAX_BUFFER_INDEX    = 64;

    ntsa::ConstMessage constMessage(NTSCFG_TEST_ALLOCATOR);

    for (bsl::size_t resetIteration = 0; resetIteration < 2; ++resetIteration)
    {
        NTSCFG_TEST_EQ(constMessage.endpoint(), ntsa::Endpoint());
        NTSCFG_TEST_EQ(constMessage.numBuffers(), 0);
        NTSCFG_TEST_EQ(constMessage.size(), 0);
        NTSCFG_TEST_EQ(constMessage.capacity(), 0);

        bsl::vector<ntsa::ConstBuffer> constBufferArray;

        for (bsl::size_t bufferIndex = 0; bufferIndex < MAX_BUFFER_INDEX;
             ++bufferIndex)
        {
            ntsa::ConstBuffer constBuffer(constBufferData(bufferIndex),
                                          constBufferSize(bufferIndex));

            constMessage.appendBuffer(constBuffer);
            constBufferArray.push_back(constBuffer);

            bsl::size_t expectedNumBuffers = bufferIndex + 1;

            NTSCFG_TEST_EQ(constMessage.numBuffers(), expectedNumBuffers);

            bsl::size_t expectedCapacity = 0;
            for (bsl::size_t existingBufferIndex = 0;
                 existingBufferIndex <= bufferIndex;
                 ++existingBufferIndex)
            {
                NTSCFG_TEST_EQ(constMessage.buffer(existingBufferIndex).data(),
                               constBufferArray[existingBufferIndex].data());
                NTSCFG_TEST_EQ(constMessage.buffer(existingBufferIndex).size(),
                               constBufferArray[existingBufferIndex].size());

                expectedCapacity += constBufferSize(existingBufferIndex);
            }

            bsl::size_t expectedSize = expectedCapacity;

            NTSCFG_TEST_EQ(constMessage.size(), expectedSize);
            NTSCFG_TEST_EQ(constMessage.capacity(), expectedCapacity);
        }

        constMessage.reset();
    }
}

NTSCFG_TEST_FUNCTION(ntsa::MessageTest::verifyCase2)
{
    // Concern: The semantics of 'ntsa::MutableMessage' are correct.
    // Plan:

    const bsl::size_t MAX_RESET_ITERATION = 2;
    const bsl::size_t MAX_BUFFER_INDEX    = 64;

    ntsa::MutableMessage mutableMessage(NTSCFG_TEST_ALLOCATOR);

    for (bsl::size_t resetIteration = 0; resetIteration < 2; ++resetIteration)
    {
        NTSCFG_TEST_EQ(mutableMessage.endpoint(), ntsa::Endpoint());
        NTSCFG_TEST_EQ(mutableMessage.numBuffers(), 0);
        NTSCFG_TEST_EQ(mutableMessage.size(), 0);
        NTSCFG_TEST_EQ(mutableMessage.capacity(), 0);

        bsl::vector<ntsa::MutableBuffer> mutableBufferArray;

        for (bsl::size_t bufferIndex = 0; bufferIndex < MAX_BUFFER_INDEX;
             ++bufferIndex)
        {
            ntsa::MutableBuffer mutableBuffer(mutableBufferData(bufferIndex),
                                              mutableBufferSize(bufferIndex));

            mutableMessage.appendBuffer(mutableBuffer);
            mutableBufferArray.push_back(mutableBuffer);

            bsl::size_t expectedNumBuffers = bufferIndex + 1;

            NTSCFG_TEST_EQ(mutableMessage.numBuffers(), expectedNumBuffers);

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

                expectedCapacity += mutableBufferSize(existingBufferIndex);
            }

            bsl::size_t expectedSize = 0;

            NTSCFG_TEST_EQ(mutableMessage.size(), expectedSize);
            NTSCFG_TEST_EQ(mutableMessage.capacity(), expectedCapacity);
        }

        mutableMessage.reset();
    }
}

const char MessageTest::s_buffer[1] = {0};

const void* MessageTest::constBufferData(bsl::size_t bufferIndex)
{
    return &s_buffer[0] + bufferIndex;
}

bsl::size_t MessageTest::constBufferSize(bsl::size_t bufferIndex)
{
    return 10 * bufferIndex;
}

void* MessageTest::mutableBufferData(bsl::size_t bufferIndex)
{
    return const_cast<char*>(&s_buffer[0]) + bufferIndex;
}

bsl::size_t MessageTest::mutableBufferSize(bsl::size_t bufferIndex)
{
    return 100 * bufferIndex;
}

}  // close namespace ntsa
}  // close namespace BloombergLP
