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
BSLS_IDENT_RCSID(ntscfg_buffer_t_cpp, "$Id$ $CSID$")

#include <ntsa_buffer.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <winsock2.h>
#endif

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntscfg::BufferTest'.
class BufferTest
{
    /// Define a type alias for a non-modifiable buffer sequence.
    typedef ntsa::ConstBufferSequence<ntsa::ConstBuffer> ConstBufferSequence;

    /// Define a type alias for a modifiable buffer sequence.
    typedef ntsa::MutableBufferSequence<ntsa::MutableBuffer>
        MutableBufferSequence;

  public:
    // Verify system structure layout and value semantics.
    static void verifyCase1();

    // Empty
    static void verifyCase2();

    // [ ]
    static void verifyCase3();

    // [X]
    static void verifyCase4();

    // [ ][ ]
    static void verifyCase5();

    // [X][ ]
    static void verifyCase6();

    // [X][X]
    static void verifyCase7();

    // [  ]
    static void verifyCase8();

    // [X ]
    static void verifyCase9();

    // [XX]
    static void verifyCase10();

    // [  ][  ]
    static void verifyCase11();

    // [X ][  ]
    static void verifyCase12();

    // [XX][  ]
    static void verifyCase13();

    // [XX][X ]
    static void verifyCase14();

    // [XX][XX]
    static void verifyCase15();

    // Gathering buffers from a blob
    static void verifyCase16();

    // Scattering buffers to a blob
    static void verifyCase17();

    // Copying data from mutable buffers to a blob
    static void verifyCase18();

    // Copying data from immutable buffers to a blob
    static void verifyCase19();

    // Usage example 1
    static void verifyCase20();

    // Usage example 2
    static void verifyCase21();
};

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase1)
{
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

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase2)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(1, NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(blob.length(), 0);
    NTSCFG_TEST_EQ(blob.totalSize(), 0);

    {
        ConstBufferSequence bufferSequence(&blob);

        ConstBufferSequence::Iterator it = bufferSequence.begin();
        ConstBufferSequence::Iterator et = bufferSequence.end();

        NTSCFG_TEST_TRUE(it == et);
    }

    {
        MutableBufferSequence bufferSequence(&blob);

        MutableBufferSequence::Iterator it = bufferSequence.begin();
        MutableBufferSequence::Iterator et = bufferSequence.end();

        NTSCFG_TEST_TRUE(it == et);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase3)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(1, NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    blob.setLength(1);
    blob.setLength(0);

    NTSCFG_TEST_EQ(blob.length(), 0);
    NTSCFG_TEST_EQ(blob.totalSize(), 1);

    {
        ConstBufferSequence bufferSequence(&blob);

        ConstBufferSequence::Iterator it = bufferSequence.begin();
        ConstBufferSequence::Iterator et = bufferSequence.end();

        NTSCFG_TEST_TRUE(it == et);
    }

    {
        MutableBufferSequence bufferSequence(&blob);

        MutableBufferSequence::Iterator it = bufferSequence.begin();
        MutableBufferSequence::Iterator et = bufferSequence.end();

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

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase4)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(1, NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    blob.setLength(1);
    blob.setLength(0);

    bdlbb::BlobUtil::append(&blob, "X", 1);

    NTSCFG_TEST_EQ(blob.length(), 1);
    NTSCFG_TEST_EQ(blob.totalSize(), 1);

    {
        ConstBufferSequence bufferSequence(&blob);

        ConstBufferSequence::Iterator it = bufferSequence.begin();
        ConstBufferSequence::Iterator et = bufferSequence.end();

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
        MutableBufferSequence bufferSequence(&blob);

        MutableBufferSequence::Iterator it = bufferSequence.begin();
        MutableBufferSequence::Iterator et = bufferSequence.end();

        NTSCFG_TEST_TRUE(it == et);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase5)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(1, NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    blob.setLength(2);
    blob.setLength(0);

    NTSCFG_TEST_EQ(blob.length(), 0);
    NTSCFG_TEST_EQ(blob.totalSize(), 2);

    {
        ConstBufferSequence bufferSequence(&blob);

        ConstBufferSequence::Iterator it = bufferSequence.begin();
        ConstBufferSequence::Iterator et = bufferSequence.end();

        NTSCFG_TEST_TRUE(it == et);
    }

    {
        MutableBufferSequence bufferSequence(&blob);

        MutableBufferSequence::Iterator it = bufferSequence.begin();
        MutableBufferSequence::Iterator et = bufferSequence.end();

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

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase6)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(1, NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    blob.setLength(2);
    blob.setLength(0);

    bdlbb::BlobUtil::append(&blob, "X", 1);

    NTSCFG_TEST_EQ(blob.length(), 1);
    NTSCFG_TEST_EQ(blob.totalSize(), 2);

    {
        ConstBufferSequence bufferSequence(&blob);

        ConstBufferSequence::Iterator it = bufferSequence.begin();
        ConstBufferSequence::Iterator et = bufferSequence.end();

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
        MutableBufferSequence bufferSequence(&blob);

        MutableBufferSequence::Iterator it = bufferSequence.begin();
        MutableBufferSequence::Iterator et = bufferSequence.end();

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

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase7)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(1, NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    blob.setLength(2);
    blob.setLength(0);

    bdlbb::BlobUtil::append(&blob, "XX", 2);

    NTSCFG_TEST_EQ(blob.length(), 2);
    NTSCFG_TEST_EQ(blob.totalSize(), 2);

    {
        ConstBufferSequence bufferSequence(&blob);

        ConstBufferSequence::Iterator it = bufferSequence.begin();
        ConstBufferSequence::Iterator et = bufferSequence.end();

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
        MutableBufferSequence bufferSequence(&blob);

        MutableBufferSequence::Iterator it = bufferSequence.begin();
        MutableBufferSequence::Iterator et = bufferSequence.end();

        NTSCFG_TEST_TRUE(it == et);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase8)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(2, NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    blob.setLength(2);
    blob.setLength(0);

    NTSCFG_TEST_EQ(blob.length(), 0);
    NTSCFG_TEST_EQ(blob.totalSize(), 2);

    {
        ConstBufferSequence bufferSequence(&blob);

        ConstBufferSequence::Iterator it = bufferSequence.begin();
        ConstBufferSequence::Iterator et = bufferSequence.end();

        NTSCFG_TEST_TRUE(it == et);
    }

    {
        MutableBufferSequence bufferSequence(&blob);

        MutableBufferSequence::Iterator it = bufferSequence.begin();
        MutableBufferSequence::Iterator et = bufferSequence.end();

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

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase9)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(2, NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    blob.setLength(2);
    blob.setLength(0);

    bdlbb::BlobUtil::append(&blob, "X", 1);

    NTSCFG_TEST_EQ(blob.length(), 1);
    NTSCFG_TEST_EQ(blob.totalSize(), 2);

    {
        ConstBufferSequence bufferSequence(&blob);

        ConstBufferSequence::Iterator it = bufferSequence.begin();
        ConstBufferSequence::Iterator et = bufferSequence.end();

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
        MutableBufferSequence bufferSequence(&blob);

        MutableBufferSequence::Iterator it = bufferSequence.begin();
        MutableBufferSequence::Iterator et = bufferSequence.end();

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

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase10)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(2, NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    blob.setLength(2);
    blob.setLength(0);

    bdlbb::BlobUtil::append(&blob, "XX", 2);

    NTSCFG_TEST_EQ(blob.length(), 2);
    NTSCFG_TEST_EQ(blob.totalSize(), 2);

    {
        ConstBufferSequence bufferSequence(&blob);

        ConstBufferSequence::Iterator it = bufferSequence.begin();
        ConstBufferSequence::Iterator et = bufferSequence.end();

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
        MutableBufferSequence bufferSequence(&blob);

        MutableBufferSequence::Iterator it = bufferSequence.begin();
        MutableBufferSequence::Iterator et = bufferSequence.end();

        NTSCFG_TEST_TRUE(it == et);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase11)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(2, NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    blob.setLength(4);
    blob.setLength(0);

    NTSCFG_TEST_EQ(blob.length(), 0);
    NTSCFG_TEST_EQ(blob.totalSize(), 4);

    {
        ConstBufferSequence bufferSequence(&blob);

        ConstBufferSequence::Iterator it = bufferSequence.begin();
        ConstBufferSequence::Iterator et = bufferSequence.end();

        NTSCFG_TEST_TRUE(it == et);
    }

    {
        MutableBufferSequence bufferSequence(&blob);

        MutableBufferSequence::Iterator it = bufferSequence.begin();
        MutableBufferSequence::Iterator et = bufferSequence.end();

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

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase12)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(2, NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    blob.setLength(4);
    blob.setLength(0);

    bdlbb::BlobUtil::append(&blob, "X", 1);

    NTSCFG_TEST_EQ(blob.length(), 1);
    NTSCFG_TEST_EQ(blob.totalSize(), 4);

    {
        ConstBufferSequence bufferSequence(&blob);

        ConstBufferSequence::Iterator it = bufferSequence.begin();
        ConstBufferSequence::Iterator et = bufferSequence.end();

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
        MutableBufferSequence bufferSequence(&blob);

        MutableBufferSequence::Iterator it = bufferSequence.begin();
        MutableBufferSequence::Iterator et = bufferSequence.end();

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

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase13)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(2, NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    blob.setLength(4);
    blob.setLength(0);

    bdlbb::BlobUtil::append(&blob, "XX", 2);

    NTSCFG_TEST_EQ(blob.length(), 2);
    NTSCFG_TEST_EQ(blob.totalSize(), 4);

    {
        ConstBufferSequence bufferSequence(&blob);

        ConstBufferSequence::Iterator it = bufferSequence.begin();
        ConstBufferSequence::Iterator et = bufferSequence.end();

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
        MutableBufferSequence bufferSequence(&blob);

        MutableBufferSequence::Iterator it = bufferSequence.begin();
        MutableBufferSequence::Iterator et = bufferSequence.end();

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

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase14)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(2, NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    blob.setLength(4);
    blob.setLength(0);

    bdlbb::BlobUtil::append(&blob, "XXX", 3);

    NTSCFG_TEST_EQ(blob.length(), 3);
    NTSCFG_TEST_EQ(blob.totalSize(), 4);

    {
        ConstBufferSequence bufferSequence(&blob);

        ConstBufferSequence::Iterator it = bufferSequence.begin();
        ConstBufferSequence::Iterator et = bufferSequence.end();

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
        MutableBufferSequence bufferSequence(&blob);

        MutableBufferSequence::Iterator it = bufferSequence.begin();
        MutableBufferSequence::Iterator et = bufferSequence.end();

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

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase15)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(2, NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    blob.setLength(4);
    blob.setLength(0);

    bdlbb::BlobUtil::append(&blob, "XXXX", 4);

    NTSCFG_TEST_EQ(blob.length(), 4);
    NTSCFG_TEST_EQ(blob.totalSize(), 4);

    {
        ConstBufferSequence bufferSequence(&blob);

        ConstBufferSequence::Iterator it = bufferSequence.begin();
        ConstBufferSequence::Iterator et = bufferSequence.end();

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
        MutableBufferSequence bufferSequence(&blob);

        MutableBufferSequence::Iterator it = bufferSequence.begin();
        MutableBufferSequence::Iterator et = bufferSequence.end();

        NTSCFG_TEST_TRUE(it == et);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase16)
{
}

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase17)
{
}

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase18)
{
}

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase19)
{
    const int MIN_SOURCE_BUFFER_SIZE = 1;
    const int MAX_SOURCE_BUFFER_SIZE = 16;

    const int MIN_DESTINATION_BUFFER_SIZE = 1;
    const int MAX_DESTINATION_BUFFER_SIZE = 16;

    const int MIN_MESSAGE_SIZE = 1;
    const int MAX_MESSAGE_SIZE = 64;

    for (int messageSize = MIN_MESSAGE_SIZE; messageSize <= MAX_MESSAGE_SIZE;
         ++messageSize)
    {
        NTSCFG_TEST_LOG_DEBUG << "Testing message size " << messageSize
                              << NTSCFG_TEST_LOG_END;

        for (int sourceBufferSize = MIN_SOURCE_BUFFER_SIZE;
             sourceBufferSize <= MAX_SOURCE_BUFFER_SIZE;
             ++sourceBufferSize)
        {
            NTSCFG_TEST_LOG_DEBUG << "Testing source buffer size "
                                  << sourceBufferSize << NTSCFG_TEST_LOG_END;

            bdlbb::PooledBlobBufferFactory sourceBlobBufferFactory(
                sourceBufferSize,
                NTSCFG_TEST_ALLOCATOR);

            bdlbb::Blob sourceBlob(&sourceBlobBufferFactory,
                                   NTSCFG_TEST_ALLOCATOR);
            ntscfg::TestDataUtil::generateData(&sourceBlob, messageSize);

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

                bdlbb::PooledBlobBufferFactory destinationBlobBufferFactory(
                    destinationBufferSize,
                    NTSCFG_TEST_ALLOCATOR);

                for (int offset = 0; offset < sourceBlob.length(); ++offset) {
                    bdlbb::Blob truncatedSourceBlob = sourceBlob;
                    bdlbb::BlobUtil::erase(&truncatedSourceBlob, 0, offset);

                    {
                        bdlbb::Blob destinationBlob(
                            &destinationBlobBufferFactory,
                            NTSCFG_TEST_ALLOCATOR);

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
                            NTSCFG_TEST_ALLOCATOR);

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

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase20)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(8, NTSCFG_TEST_ALLOCATOR);
    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    bdlbb::BlobUtil::append(&blob, "Hello, world!", 0, 13);

    BSLS_ASSERT(blob.numBuffers() == 2);

    const bdlbb::BlobBuffer& blobBuffer1 = blob.buffer(0);
    const bdlbb::BlobBuffer& blobBuffer2 = blob.buffer(1);

    ntsa::ConstBufferSequence<ntsa::ConstBuffer> constBufferSequence(&blob);

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

NTSCFG_TEST_FUNCTION(ntsa::BufferTest::verifyCase21)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(8, NTSCFG_TEST_ALLOCATOR);
    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

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

}  // close namespace ntsa
}  // close namespace BloombergLP
