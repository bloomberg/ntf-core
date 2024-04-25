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

#include <ntsa_buffer.h>
#include <ntscfg_test.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobutil.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bslma_testallocator.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <winsock2.h>
#endif

using namespace BloombergLP;
using namespace ntsa;

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

/// Define a type alias for a non-modifiable buffer sequence.
typedef ntsa::ConstBufferSequence<ntsa::ConstBuffer> ConstBufferSequence;

/// Define a type alias for a modifiable buffer sequence.
typedef ntsa::MutableBufferSequence<ntsa::MutableBuffer> MutableBufferSequence;

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

}  // close namespace 'test'

NTSCFG_TEST_CASE(1)
{
    // Concern: System structure layout and value semantics
    // Plan:

    // MutableBuffer structure definition.

    {
        ntsa::MutableBuffer buffer;
        NTSCFG_TEST_TRUE(buffer.validate());
    }

    {
        char buf1[10];
        char buf2[4];
        char buf3[7];

        ntsa::MutableBuffer vector[3];

        vector[0].setBuffer(buf1, 10);
        vector[1].setBuffer(buf2, 4);
        vector[2].setBuffer(buf3, 7);

#if defined(BSLS_PLATFORM_OS_UNIX)

        struct ::iovec* posixVector = (struct ::iovec*)(vector);

        NTSCFG_TEST_EQ(vector[0].buffer(), posixVector[0].iov_base);
        NTSCFG_TEST_EQ(vector[0].length(), posixVector[0].iov_len);

        NTSCFG_TEST_EQ(vector[1].buffer(), posixVector[1].iov_base);
        NTSCFG_TEST_EQ(vector[1].length(), posixVector[1].iov_len);

        NTSCFG_TEST_EQ(vector[2].buffer(), posixVector[2].iov_base);
        NTSCFG_TEST_EQ(vector[2].length(), posixVector[2].iov_len);

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

        WSABUF* windowsVector = (WSABUF*)(vector);

        NTSCFG_TEST_EQ(vector[0].buffer(), windowsVector[0].buf);
        NTSCFG_TEST_EQ(vector[0].length(), windowsVector[0].len);

        NTSCFG_TEST_EQ(vector[1].buffer(), windowsVector[1].buf);
        NTSCFG_TEST_EQ(vector[1].length(), windowsVector[1].len);

        NTSCFG_TEST_EQ(vector[2].buffer(), windowsVector[2].buf);
        NTSCFG_TEST_EQ(vector[2].length(), windowsVector[2].len);

#endif
    }

    // ConstBuffer structure definition.

    {
        ntsa::ConstBuffer buffer;
        NTSCFG_TEST_TRUE(buffer.validate());
    }

    {
        char buf1[10];
        char buf2[4];
        char buf3[7];

        ntsa::ConstBuffer vector[3];

        vector[0].setBuffer(buf1, 10);
        vector[1].setBuffer(buf2, 4);
        vector[2].setBuffer(buf3, 7);

#if defined(BSLS_PLATFORM_OS_UNIX)

        struct ::iovec* posixVector = (struct ::iovec*)(vector);

        NTSCFG_TEST_EQ(vector[0].buffer(), posixVector[0].iov_base);
        NTSCFG_TEST_EQ(vector[0].length(), posixVector[0].iov_len);

        NTSCFG_TEST_EQ(vector[1].buffer(), posixVector[1].iov_base);
        NTSCFG_TEST_EQ(vector[1].length(), posixVector[1].iov_len);

        NTSCFG_TEST_EQ(vector[2].buffer(), posixVector[2].iov_base);
        NTSCFG_TEST_EQ(vector[2].length(), posixVector[2].iov_len);

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

        WSABUF* windowsVector = (WSABUF*)(vector);

        NTSCFG_TEST_EQ(vector[0].buffer(), windowsVector[0].buf);
        NTSCFG_TEST_EQ(vector[0].length(), windowsVector[0].len);

        NTSCFG_TEST_EQ(vector[1].buffer(), windowsVector[1].buf);
        NTSCFG_TEST_EQ(vector[1].length(), windowsVector[1].len);

        NTSCFG_TEST_EQ(vector[2].buffer(), windowsVector[2].buf);
        NTSCFG_TEST_EQ(vector[2].length(), windowsVector[2].len);

#endif
    }

    void*       DATA = (void*)(123);
    bsl::size_t SIZE = 456;

    // MutableBuffer constructors, manipulators, and accessors.

    {
        ntsa::MutableBuffer b1;

        NTSCFG_TEST_EQ(b1.data(), 0);
        NTSCFG_TEST_EQ(b1.size(), 0);
        NTSCFG_TEST_EQ(b1.data(), b1.buffer());
        NTSCFG_TEST_EQ(b1.size(), b1.length());
    }

    {
        ntsa::MutableBuffer b1(DATA, SIZE);

        NTSCFG_TEST_EQ(b1.data(), DATA);
        NTSCFG_TEST_EQ(b1.size(), SIZE);
        NTSCFG_TEST_EQ(b1.data(), b1.buffer());
        NTSCFG_TEST_EQ(b1.size(), b1.length());
    }

    {
        ntsa::MutableBuffer b1;
        b1.setBuffer(DATA, SIZE);

        NTSCFG_TEST_EQ(b1.data(), DATA);
        NTSCFG_TEST_EQ(b1.size(), SIZE);
        NTSCFG_TEST_EQ(b1.data(), b1.buffer());
        NTSCFG_TEST_EQ(b1.size(), b1.length());
    }

    {
        ntsa::MutableBuffer b1(DATA, SIZE);
        ntsa::MutableBuffer b2(b1);

        NTSCFG_TEST_EQ(b2.data(), DATA);
        NTSCFG_TEST_EQ(b2.size(), SIZE);
        NTSCFG_TEST_EQ(b2.data(), b2.buffer());
        NTSCFG_TEST_EQ(b2.size(), b2.length());
    }

    {
        ntsa::MutableBuffer b1(DATA, SIZE);
        ntsa::MutableBuffer b2;
        b2 = b1;

        NTSCFG_TEST_EQ(b2.data(), DATA);
        NTSCFG_TEST_EQ(b2.size(), SIZE);
        NTSCFG_TEST_EQ(b2.data(), b2.buffer());
        NTSCFG_TEST_EQ(b2.size(), b2.length());
    }

    // ConstBuffer constructors, manipulators, and accessors.

    {
        ntsa::ConstBuffer b1;

        NTSCFG_TEST_EQ(b1.data(), 0);
        NTSCFG_TEST_EQ(b1.size(), 0);
        NTSCFG_TEST_EQ(b1.data(), b1.buffer());
        NTSCFG_TEST_EQ(b1.size(), b1.length());
    }

    {
        ntsa::ConstBuffer b1(DATA, SIZE);

        NTSCFG_TEST_EQ(b1.data(), DATA);
        NTSCFG_TEST_EQ(b1.size(), SIZE);
        NTSCFG_TEST_EQ(b1.data(), b1.buffer());
        NTSCFG_TEST_EQ(b1.size(), b1.length());
    }

    {
        ntsa::ConstBuffer b1;
        b1.setBuffer(DATA, SIZE);

        NTSCFG_TEST_EQ(b1.data(), DATA);
        NTSCFG_TEST_EQ(b1.size(), SIZE);
        NTSCFG_TEST_EQ(b1.data(), b1.buffer());
        NTSCFG_TEST_EQ(b1.size(), b1.length());
    }

    {
        ntsa::ConstBuffer b1(DATA, SIZE);
        ntsa::ConstBuffer b2(b1);

        NTSCFG_TEST_EQ(b2.data(), DATA);
        NTSCFG_TEST_EQ(b2.size(), SIZE);
        NTSCFG_TEST_EQ(b2.data(), b2.buffer());
        NTSCFG_TEST_EQ(b2.size(), b2.length());
    }

    {
        ntsa::ConstBuffer b1(DATA, SIZE);
        ntsa::ConstBuffer b2;
        b2 = b1;

        NTSCFG_TEST_EQ(b2.data(), DATA);
        NTSCFG_TEST_EQ(b2.size(), SIZE);
        NTSCFG_TEST_EQ(b2.data(), b2.buffer());
        NTSCFG_TEST_EQ(b2.size(), b2.length());
    }
}

NTSCFG_TEST_CASE(2)
{
    // Concern: Buffer sequence: Empty
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(1, &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        NTSCFG_TEST_EQ(blob.length(), 0);
        NTSCFG_TEST_EQ(blob.totalSize(), 0);

        {
            test::ConstBufferSequence bufferSequence(&blob);

            test::ConstBufferSequence::Iterator it = bufferSequence.begin();
            test::ConstBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it == et);
        }

        {
            test::MutableBufferSequence bufferSequence(&blob);

            test::MutableBufferSequence::Iterator it = bufferSequence.begin();
            test::MutableBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it == et);
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(3)
{
    // Concern: Buffer sequence: [ ]
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(1, &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        blob.setLength(1);
        blob.setLength(0);

        NTSCFG_TEST_EQ(blob.length(), 0);
        NTSCFG_TEST_EQ(blob.totalSize(), 1);

        {
            test::ConstBufferSequence bufferSequence(&blob);

            test::ConstBufferSequence::Iterator it = bufferSequence.begin();
            test::ConstBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it == et);
        }

        {
            test::MutableBufferSequence bufferSequence(&blob);

            test::MutableBufferSequence::Iterator it = bufferSequence.begin();
            test::MutableBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::MutableBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(0).data());
                NTSCFG_TEST_TRUE(buffer.size() == 1);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(4)
{
    // Concern: Buffer sequence: [X]
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(1, &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        blob.setLength(1);
        blob.setLength(0);

        bdlbb::BlobUtil::append(&blob, "X", 1);

        NTSCFG_TEST_EQ(blob.length(), 1);
        NTSCFG_TEST_EQ(blob.totalSize(), 1);

        {
            test::ConstBufferSequence bufferSequence(&blob);

            test::ConstBufferSequence::Iterator it = bufferSequence.begin();
            test::ConstBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::ConstBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(0).data());
                NTSCFG_TEST_TRUE(buffer.size() == 1);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }

        {
            test::MutableBufferSequence bufferSequence(&blob);

            test::MutableBufferSequence::Iterator it = bufferSequence.begin();
            test::MutableBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it == et);
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(5)
{
    // Concern: Buffer sequence: [ ][ ]
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(1, &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        blob.setLength(2);
        blob.setLength(0);

        NTSCFG_TEST_EQ(blob.length(), 0);
        NTSCFG_TEST_EQ(blob.totalSize(), 2);

        {
            test::ConstBufferSequence bufferSequence(&blob);

            test::ConstBufferSequence::Iterator it = bufferSequence.begin();
            test::ConstBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it == et);
        }

        {
            test::MutableBufferSequence bufferSequence(&blob);

            test::MutableBufferSequence::Iterator it = bufferSequence.begin();
            test::MutableBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::MutableBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(0).data());
                NTSCFG_TEST_TRUE(buffer.size() == 1);
            }

            ++it;
            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::MutableBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(1).data());
                NTSCFG_TEST_TRUE(buffer.size() == 1);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(6)
{
    // Concern: Buffer sequence: [X][ ]
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(1, &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        blob.setLength(2);
        blob.setLength(0);

        bdlbb::BlobUtil::append(&blob, "X", 1);

        NTSCFG_TEST_EQ(blob.length(), 1);
        NTSCFG_TEST_EQ(blob.totalSize(), 2);

        {
            test::ConstBufferSequence bufferSequence(&blob);

            test::ConstBufferSequence::Iterator it = bufferSequence.begin();
            test::ConstBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::ConstBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(0).data());
                NTSCFG_TEST_TRUE(buffer.size() == 1);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }

        {
            test::MutableBufferSequence bufferSequence(&blob);

            test::MutableBufferSequence::Iterator it = bufferSequence.begin();
            test::MutableBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::MutableBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(1).data());
                NTSCFG_TEST_TRUE(buffer.size() == 1);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(7)
{
    // Concern: Buffer sequence: [X][X]
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(1, &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        blob.setLength(2);
        blob.setLength(0);

        bdlbb::BlobUtil::append(&blob, "XX", 2);

        NTSCFG_TEST_EQ(blob.length(), 2);
        NTSCFG_TEST_EQ(blob.totalSize(), 2);

        {
            test::ConstBufferSequence bufferSequence(&blob);

            test::ConstBufferSequence::Iterator it = bufferSequence.begin();
            test::ConstBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::ConstBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(0).data());
                NTSCFG_TEST_TRUE(buffer.size() == 1);
            }

            ++it;
            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::ConstBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(1).data());
                NTSCFG_TEST_TRUE(buffer.size() == 1);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }

        {
            test::MutableBufferSequence bufferSequence(&blob);

            test::MutableBufferSequence::Iterator it = bufferSequence.begin();
            test::MutableBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it == et);
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(8)
{
    // Concern: Buffer sequence: [  ]
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(2, &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        blob.setLength(2);
        blob.setLength(0);

        NTSCFG_TEST_EQ(blob.length(), 0);
        NTSCFG_TEST_EQ(blob.totalSize(), 2);

        {
            test::ConstBufferSequence bufferSequence(&blob);

            test::ConstBufferSequence::Iterator it = bufferSequence.begin();
            test::ConstBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it == et);
        }

        {
            test::MutableBufferSequence bufferSequence(&blob);

            test::MutableBufferSequence::Iterator it = bufferSequence.begin();
            test::MutableBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::MutableBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(0).data());
                NTSCFG_TEST_TRUE(buffer.size() == 2);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(9)
{
    // Concern: Buffer sequence: [X ]
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(2, &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        blob.setLength(2);
        blob.setLength(0);

        bdlbb::BlobUtil::append(&blob, "X", 1);

        NTSCFG_TEST_EQ(blob.length(), 1);
        NTSCFG_TEST_EQ(blob.totalSize(), 2);

        {
            test::ConstBufferSequence bufferSequence(&blob);

            test::ConstBufferSequence::Iterator it = bufferSequence.begin();
            test::ConstBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::ConstBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(0).data());
                NTSCFG_TEST_TRUE(buffer.size() == 1);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }

        {
            test::MutableBufferSequence bufferSequence(&blob);

            test::MutableBufferSequence::Iterator it = bufferSequence.begin();
            test::MutableBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::MutableBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(0).data() + 1);
                NTSCFG_TEST_TRUE(buffer.size() == 1);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(10)
{
    // Concern: Buffer sequence: [XX]
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(2, &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        blob.setLength(2);
        blob.setLength(0);

        bdlbb::BlobUtil::append(&blob, "XX", 2);

        NTSCFG_TEST_EQ(blob.length(), 2);
        NTSCFG_TEST_EQ(blob.totalSize(), 2);

        {
            test::ConstBufferSequence bufferSequence(&blob);

            test::ConstBufferSequence::Iterator it = bufferSequence.begin();
            test::ConstBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::ConstBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(0).data());
                NTSCFG_TEST_TRUE(buffer.size() == 2);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }

        {
            test::MutableBufferSequence bufferSequence(&blob);

            test::MutableBufferSequence::Iterator it = bufferSequence.begin();
            test::MutableBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it == et);
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(11)
{
    // Concern: Buffer sequence: [  ][  ]
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(2, &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        blob.setLength(4);
        blob.setLength(0);

        NTSCFG_TEST_EQ(blob.length(), 0);
        NTSCFG_TEST_EQ(blob.totalSize(), 4);

        {
            test::ConstBufferSequence bufferSequence(&blob);

            test::ConstBufferSequence::Iterator it = bufferSequence.begin();
            test::ConstBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it == et);
        }

        {
            test::MutableBufferSequence bufferSequence(&blob);

            test::MutableBufferSequence::Iterator it = bufferSequence.begin();
            test::MutableBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::MutableBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(0).data());
                NTSCFG_TEST_TRUE(buffer.size() == 2);
            }

            ++it;
            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::MutableBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(1).data());
                NTSCFG_TEST_TRUE(buffer.size() == 2);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(12)
{
    // Concern: Buffer sequence: [X ][  ]
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(2, &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        blob.setLength(4);
        blob.setLength(0);

        bdlbb::BlobUtil::append(&blob, "X", 1);

        NTSCFG_TEST_EQ(blob.length(), 1);
        NTSCFG_TEST_EQ(blob.totalSize(), 4);

        {
            test::ConstBufferSequence bufferSequence(&blob);

            test::ConstBufferSequence::Iterator it = bufferSequence.begin();
            test::ConstBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::ConstBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(0).data());
                NTSCFG_TEST_TRUE(buffer.size() == 1);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }

        {
            test::MutableBufferSequence bufferSequence(&blob);

            test::MutableBufferSequence::Iterator it = bufferSequence.begin();
            test::MutableBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::MutableBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(0).data() + 1);
                NTSCFG_TEST_TRUE(buffer.size() == 1);
            }

            ++it;
            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::MutableBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(1).data());
                NTSCFG_TEST_TRUE(buffer.size() == 2);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(13)
{
    // Concern: Buffer sequence: [XX][  ]
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(2, &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        blob.setLength(4);
        blob.setLength(0);

        bdlbb::BlobUtil::append(&blob, "XX", 2);

        NTSCFG_TEST_EQ(blob.length(), 2);
        NTSCFG_TEST_EQ(blob.totalSize(), 4);

        {
            test::ConstBufferSequence bufferSequence(&blob);

            test::ConstBufferSequence::Iterator it = bufferSequence.begin();
            test::ConstBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::ConstBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(0).data());
                NTSCFG_TEST_TRUE(buffer.size() == 2);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }

        {
            test::MutableBufferSequence bufferSequence(&blob);

            test::MutableBufferSequence::Iterator it = bufferSequence.begin();
            test::MutableBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::MutableBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(1).data());
                NTSCFG_TEST_TRUE(buffer.size() == 2);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(14)
{
    // Concern: Buffer sequence: [XX][X ]
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(2, &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        blob.setLength(4);
        blob.setLength(0);

        bdlbb::BlobUtil::append(&blob, "XXX", 3);

        NTSCFG_TEST_EQ(blob.length(), 3);
        NTSCFG_TEST_EQ(blob.totalSize(), 4);

        {
            test::ConstBufferSequence bufferSequence(&blob);

            test::ConstBufferSequence::Iterator it = bufferSequence.begin();
            test::ConstBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::ConstBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(0).data());
                NTSCFG_TEST_TRUE(buffer.size() == 2);
            }

            ++it;
            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::ConstBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(1).data());
                NTSCFG_TEST_TRUE(buffer.size() == 1);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }

        {
            test::MutableBufferSequence bufferSequence(&blob);

            test::MutableBufferSequence::Iterator it = bufferSequence.begin();
            test::MutableBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::MutableBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(1).data() + 1);
                NTSCFG_TEST_TRUE(buffer.size() == 1);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(15)
{
    // Concern: Buffer sequence: [XX][XX]
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(2, &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        blob.setLength(4);
        blob.setLength(0);

        bdlbb::BlobUtil::append(&blob, "XXXX", 4);

        NTSCFG_TEST_EQ(blob.length(), 4);
        NTSCFG_TEST_EQ(blob.totalSize(), 4);

        {
            test::ConstBufferSequence bufferSequence(&blob);

            test::ConstBufferSequence::Iterator it = bufferSequence.begin();
            test::ConstBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::ConstBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(0).data());
                NTSCFG_TEST_TRUE(buffer.size() == 2);
            }

            ++it;
            NTSCFG_TEST_TRUE(it != et);

            {
                ntsa::ConstBuffer buffer = *it;
                NTSCFG_TEST_TRUE(buffer.data() == blob.buffer(1).data());
                NTSCFG_TEST_TRUE(buffer.size() == 2);
            }

            ++it;
            NTSCFG_TEST_TRUE(it == et);
        }

        {
            test::MutableBufferSequence bufferSequence(&blob);

            test::MutableBufferSequence::Iterator it = bufferSequence.begin();
            test::MutableBufferSequence::Iterator et = bufferSequence.end();

            NTSCFG_TEST_TRUE(it == et);
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(16)
{
    // Concern: Gathering buffers from a blob.
    // Plan:

    ntscfg::TestAllocator ta;
    {
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(17)
{
    // Concern: Scattering buffers to a blob.
    // Plan:

    ntscfg::TestAllocator ta;
    {
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(18)
{
    // Concern: Copying data from mutable buffers to a blob.
    // Plan:

    ntscfg::TestAllocator ta;
    {
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(19)
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

                ntsa::ConstBuffer::gather(&numSourceBuffers,
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

                            ntsa::ConstBuffer::copy(&destinationBlob,
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

                            ntsa::MutableBuffer::copy(
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

NTSCFG_TEST_CASE(20)
{
    // Concern: Usage example 1
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(8, &ta);
        bdlbb::Blob                    blob(&blobBufferFactory, &ta);

        bdlbb::BlobUtil::append(&blob, "Hello, world!", 0, 13);

        BSLS_ASSERT(blob.numBuffers() == 2);

        const bdlbb::BlobBuffer& blobBuffer1 = blob.buffer(0);
        const bdlbb::BlobBuffer& blobBuffer2 = blob.buffer(1);

        ntsa::ConstBufferSequence<ntsa::ConstBuffer> constBufferSequence(
            &blob);

        ntsa::ConstBufferSequence<ntsa::ConstBuffer>::Iterator it;

        it = constBufferSequence.begin();
        BSLS_ASSERT(it != constBufferSequence.end());

        {
            ntsa::ConstBuffer constBuffer = *it;
            BSLS_ASSERT(constBuffer.data() == blobBuffer1.data());
            BSLS_ASSERT(constBuffer.size() == 8);
        }

        ++it;
        BSLS_ASSERT(it != constBufferSequence.end());

        {
            ntsa::ConstBuffer constBuffer = *it;
            BSLS_ASSERT(constBuffer.data() == blobBuffer2.data());
            BSLS_ASSERT(constBuffer.size() == 5);
        }

        ++it;
        BSLS_ASSERT(it == constBufferSequence.end());
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(21)
{
    // Concern: Usage example 2
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(8, &ta);
        bdlbb::Blob                    blob(&blobBufferFactory, &ta);

        bdlbb::BlobUtil::append(&blob, "Hello, world!", 0, 13);

        BSLS_ASSERT(blob.numBuffers() == 2);

        const bdlbb::BlobBuffer& blobBuffer2 = blob.buffer(1);

        ntsa::MutableBufferSequence<ntsa::MutableBuffer> mutableBufferSequence(
            &blob);

        ntsa::MutableBufferSequence<ntsa::MutableBuffer>::Iterator it;

        it = mutableBufferSequence.begin();
        BSLS_ASSERT(it != mutableBufferSequence.end());

        {
            ntsa::MutableBuffer mutableBuffer = *it;
            BSLS_ASSERT(mutableBuffer.data() == blobBuffer2.data() + 5);
            BSLS_ASSERT(mutableBuffer.size() == 3);
        }

        ++it;
        BSLS_ASSERT(it == mutableBufferSequence.end());
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);  // Value semantics

    NTSCFG_TEST_REGISTER(2);  // Empty

    NTSCFG_TEST_REGISTER(3);  // [ ]
    NTSCFG_TEST_REGISTER(4);  // [X]

    NTSCFG_TEST_REGISTER(5);  // [ ][ ]
    NTSCFG_TEST_REGISTER(6);  // [X][ ]
    NTSCFG_TEST_REGISTER(7);  // [X][X]

    NTSCFG_TEST_REGISTER(8);   // [  ]
    NTSCFG_TEST_REGISTER(9);   // [X ]
    NTSCFG_TEST_REGISTER(10);  // [XX]

    NTSCFG_TEST_REGISTER(11);  // [  ][  ]
    NTSCFG_TEST_REGISTER(12);  // [X ][  ]
    NTSCFG_TEST_REGISTER(13);  // [XX][  ]
    NTSCFG_TEST_REGISTER(14);  // [XX][X ]
    NTSCFG_TEST_REGISTER(15);  // [XX][XX]


    NTSCFG_TEST_REGISTER(16);  // Gathering buffers from a blob
    NTSCFG_TEST_REGISTER(17);  // Scattering buffers to a blob
    NTSCFG_TEST_REGISTER(18);  // Copying data from mutable buffers to a blob
    NTSCFG_TEST_REGISTER(19);  // Copying data from immutable buffers to a blob

    NTSCFG_TEST_REGISTER(20);  // Usage example 1
    NTSCFG_TEST_REGISTER(21);  // Usage example 2
}
NTSCFG_TEST_DRIVER_END;
