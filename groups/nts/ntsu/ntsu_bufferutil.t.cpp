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

#include <ntsu_bufferutil.h>
#include <bdlbb_blobutil.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bslma_testallocator.h>

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

/// Provide a suite of utilities for generating test data.
/// This struct is completely thread safe.
struct DataUtil {
    /// Return the byte at the specified 'position' in the specified
    /// 'dataset'.
    static char generateByte(bsl::size_t position, bsl::size_t dataset);

    /// Load into the specified 'result' the specified 'size' sequence of
    /// bytes from the specified 'dataset' starting at the specified
    /// 'offset'.
    static void generateData(bsl::string* result,
                             bsl::size_t  size,
                             bsl::size_t  offset  = 0,
                             bsl::size_t  dataset = 0);

    /// Load into the specified 'result' the specified 'size' sequence of
    /// bytes from the specified 'dataset' starting at the specified
    /// 'offset'.
    static void generateData(bdlbb::Blob* result,
                             bsl::size_t  size,
                             bsl::size_t  offset  = 0,
                             bsl::size_t  dataset = 0);
};

char DataUtil::generateByte(bsl::size_t position, bsl::size_t dataset)
{
    struct {
        const char* source;
        bsl::size_t length;
    } DATA[] = {
        {"abcdefghijklmnopqrstuvwxyz", 26},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26}
    };

    enum { NUM_DATA = sizeof(DATA) / sizeof(*DATA) };

    dataset = dataset % NUM_DATA;
    return DATA[dataset].source[position % DATA[dataset].length];
}

void DataUtil::generateData(bsl::string* result,
                            bsl::size_t  size,
                            bsl::size_t  offset,
                            bsl::size_t  dataset)
{
    result->clear();
    result->resize(size);

    for (bsl::size_t i = offset; i < offset + size; ++i) {
        (*result)[i] = generateByte(offset + i, dataset);
    }
}

void DataUtil::generateData(bdlbb::Blob* result,
                            bsl::size_t  size,
                            bsl::size_t  offset,
                            bsl::size_t  dataset)
{
    result->removeAll();
    result->setLength(static_cast<int>(size));

    bsl::size_t k = 0;

    for (int i = 0; i < result->numDataBuffers(); ++i) {
        const bdlbb::BlobBuffer& buffer = result->buffer(i);

        bsl::size_t numBytesToWrite = i == result->numDataBuffers() - 1
                                          ? result->lastDataBufferLength()
                                          : buffer.size();

        for (bsl::size_t j = 0; j < numBytesToWrite; ++j) {
            buffer.data()[j] = generateByte(offset + k, dataset);
            ++k;
        }
    }
}

}  // close namespace test

NTSCFG_TEST_CASE(1)
{
    // Concern: Gathering buffers from a blob.
    // Plan:

    ntscfg::TestAllocator ta;
    {
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(2)
{
    // Concern: Scattering buffers to a blob.
    // Plan:

    ntscfg::TestAllocator ta;
    {
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(3)
{
    // Concern: Copying data from mutable buffers to a blob.
    // Plan:

    ntscfg::TestAllocator ta;
    {
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(4)
{
    // Concern: Copying data from immutable buffers to a blob.
    // Plan:

    const int MIN_SOURCE_BUFFER_SIZE = 1;
    const int MAX_SOURCE_BUFFER_SIZE = 16;

    const int MIN_DESTINATION_BUFFER_SIZE = 1;
    const int MAX_DESTINATION_BUFFER_SIZE = 16;

    const int MIN_MESSAGE_SIZE = 1;
    const int MAX_MESSAGE_SIZE = 64;

    ntscfg::TestAllocator ta;
    {
        for (int messageSize = MIN_MESSAGE_SIZE;
             messageSize <= MAX_MESSAGE_SIZE;
             ++messageSize)
        {
            NTSCFG_TEST_LOG_DEBUG << "Testing message size " << messageSize
                                  << NTSCFG_TEST_LOG_END;

            for (int sourceBufferSize = MIN_SOURCE_BUFFER_SIZE;
                 sourceBufferSize <= MAX_SOURCE_BUFFER_SIZE;
                 ++sourceBufferSize)
            {
                NTSCFG_TEST_LOG_DEBUG << "Testing source buffer size "
                                      << sourceBufferSize
                                      << NTSCFG_TEST_LOG_END;

                bdlbb::PooledBlobBufferFactory sourceBlobBufferFactory(
                    sourceBufferSize,
                    &ta);

                bdlbb::Blob sourceBlob(&sourceBlobBufferFactory, &ta);
                test::DataUtil::generateData(&sourceBlob, messageSize);

                bsl::vector<ntsa::ConstBuffer> sourceBufferArray;
                sourceBufferArray.resize(sourceBlob.numDataBuffers());

                bsl::size_t numSourceBuffers;
                bsl::size_t numSourceBytes;

                ntsu::BufferUtil::gather(&numSourceBuffers,
                                         &numSourceBytes,
                                         &sourceBufferArray[0],
                                         sourceBufferArray.size(),
                                         sourceBlob,
                                         MAX_MESSAGE_SIZE);

                NTSCFG_TEST_EQ(numSourceBuffers, sourceBlob.numDataBuffers());
                NTSCFG_TEST_EQ(numSourceBytes, sourceBlob.length());

                for (int destinationBufferSize = MIN_DESTINATION_BUFFER_SIZE;
                     destinationBufferSize <= MAX_DESTINATION_BUFFER_SIZE;
                     ++destinationBufferSize)
                {
                    NTSCFG_TEST_LOG_DEBUG << "Testing destination buffer size "
                                          << destinationBufferSize
                                          << NTSCFG_TEST_LOG_END;

                    bdlbb::PooledBlobBufferFactory
                        destinationBlobBufferFactory(destinationBufferSize,
                                                     &ta);

                    for (int offset = 0; offset < sourceBlob.length();
                         ++offset)
                    {
                        bdlbb::Blob truncatedSourceBlob = sourceBlob;
                        bdlbb::BlobUtil::erase(&truncatedSourceBlob,
                                               0,
                                               offset);

                        {
                            bdlbb::Blob destinationBlob(
                                &destinationBlobBufferFactory,
                                &ta);
                            ntsu::BufferUtil::copy(&destinationBlob,
                                                   &sourceBufferArray[0],
                                                   sourceBufferArray.size(),
                                                   offset);

                            NTSCFG_TEST_EQ(destinationBlob.length(),
                                           truncatedSourceBlob.length());

                            NTSCFG_TEST_EQ(
                                bdlbb::BlobUtil::compare(destinationBlob,
                                                         truncatedSourceBlob),
                                0);
                        }

                        {
                            bdlbb::Blob destinationBlob(
                                &destinationBlobBufferFactory,
                                &ta);
                            ntsu::BufferUtil::copy(
                                &destinationBlob,
                                reinterpret_cast<const ntsa::MutableBuffer*>(
                                    &sourceBufferArray[0]),
                                sourceBufferArray.size(),
                                offset);

                            NTSCFG_TEST_EQ(destinationBlob.length(),
                                           truncatedSourceBlob.length());

                            NTSCFG_TEST_EQ(
                                bdlbb::BlobUtil::compare(destinationBlob,
                                                         truncatedSourceBlob),
                                0);
                        }
                    }
                }
            }
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
    NTSCFG_TEST_REGISTER(4);
}
NTSCFG_TEST_DRIVER_END;
