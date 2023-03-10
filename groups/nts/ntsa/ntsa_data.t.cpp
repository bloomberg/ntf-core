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

#include <ntsa_data.h>

#include <ntscfg_test.h>

#include <bdlbb_blob.h>
#include <bdlbb_blobstreambuf.h>
#include <bdlbb_blobutil.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

using namespace BloombergLP;
using namespace ntsa;

namespace test {

const bsl::size_t BLOB_BUFFER_SIZE = 4;

const char        DATA[]    = "Hello, world!";
const bsl::size_t DATA_SIZE = sizeof(DATA) - 1;

const char        DATA_A[]    = "Hello, world! ";
const bsl::size_t DATA_A_SIZE = sizeof(DATA_A) - 1;

const char        DATA_B[] = "This is a test of the data utility algorithms. ";
const bsl::size_t DATA_B_SIZE = sizeof(DATA_B) - 1;

const char        DATA_C[]    = "The blob should match its expected value. ";
const bsl::size_t DATA_C_SIZE = sizeof(DATA_C) - 1;

/// Provide utilities for comparing the contents of a blob
/// for the purposes of this test driver.
struct BlobUtil {
    /// Return true if the contents of the specified 'blob' equals the
    /// contents of the specified 'string', otherwise return false.
    static bool equals(const bdlbb::Blob& blob, const bsl::string& string);

    /// Return true if the contents of the specified 'blob' equals the
    /// contents of the specified 'stringA' concatenated with the specified
    /// 'stringB' concatenated with the specified 'stringC', otherwise
    /// return false.
    static bool equals(const bdlbb::Blob& blob,
                       const bsl::string& stringA,
                       const bsl::string& stringB,
                       const bsl::string& stringC);
};

bool BlobUtil::equals(const bdlbb::Blob& blob, const bsl::string& string)
{
    bsl::string blobAsString;
    {
        bsl::ostringstream oss;
        bdlbb::BlobUtil::asciiDump(oss, blob);
        blobAsString = oss.str();
    }

    BSLS_LOG_DEBUG("Comparing: %s", blobAsString.c_str());

    if (blobAsString != string) {
        BSLS_LOG_ERROR("Blob does not match:\nExpected:\n%s\nFound:\n%s\n",
                       string.c_str(),
                       blobAsString.c_str());
        return false;
    }

    return true;
}

bool BlobUtil::equals(const bdlbb::Blob& blob,
                      const bsl::string& stringA,
                      const bsl::string& stringB,
                      const bsl::string& stringC)
{
    return BlobUtil::equals(blob, stringA + stringB + stringC);
}

}  // close namespace test

NTSCFG_TEST_CASE(1)
{
    // Concern: undefined
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::Data data(&ta);

        NTSCFG_TEST_EQ(data.type(), ntsa::DataType::e_UNDEFINED);
        NTSCFG_TEST_TRUE(data.isUndefined());
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(2)
{
    // Concern: bdlbb::BlobBuffer
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::Data data(&ta);

        bdlbb::BlobBuffer& blobBuffer = data.makeBlobBuffer();
        {
            bsl::shared_ptr<char> blobBufferData(
                static_cast<char*>(ta.allocate(64)),
                &ta,
                0);

            blobBuffer.reset(blobBufferData, 64);
        }

        NTSCFG_TEST_EQ(data.type(), ntsa::DataType::e_BLOB_BUFFER);
        NTSCFG_TEST_TRUE(data.isBlobBuffer());
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(3)
{
    // Concern: ntsa::ConstBuffer
    // Plan:

    ntscfg::TestAllocator ta;
    {
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(4)
{
    // Concern: ntsa::ConstBufferArray
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::Data data(&ta);

        ntsa::ConstBufferArray& constBufferArray = data.makeConstBufferArray();
        {
            bsl::vector<char> region1(128);
            bsl::vector<char> region2(256);
            bsl::vector<char> region3(512);

            constBufferArray.append(&region1.front(), region1.size());
            constBufferArray.append(&region2.front(), region2.size());
            constBufferArray.append(&region3.front(), region3.size());

            NTSCFG_TEST_EQ(constBufferArray.numBuffers(), 3);

            NTSCFG_TEST_TRUE(constBufferArray.buffer(0).data() ==
                             &region1.front());

            NTSCFG_TEST_TRUE(constBufferArray.buffer(1).data() ==
                             &region2.front());

            NTSCFG_TEST_TRUE(constBufferArray.buffer(2).data() ==
                             &region3.front());
        }

        NTSCFG_TEST_EQ(data.type(), ntsa::DataType::e_CONST_BUFFER_ARRAY);
        NTSCFG_TEST_TRUE(data.isConstBufferArray());
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(5)
{
    // Concern: ntsa::ConstBufferPtrArray
    // Plan:

    ntscfg::TestAllocator ta;
    {
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(6)
{
    // Concern: ntsa::MutableBuffer
    // Plan:

    ntscfg::TestAllocator ta;
    {
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(7)
{
    // Concern: ntsa::MutableBufferArray
    // Plan:

    ntscfg::TestAllocator ta;
    {
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(8)
{
    // Concern: ntsa::MutableBufferPtrArray
    // Plan:

    ntscfg::TestAllocator ta;
    {
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(9)
{
    // Concern: bdlbb::Blob
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(1024, &ta);

        ntsa::Data data(&blobBufferFactory, &ta);

        bdlbb::Blob& blob = data.makeBlob();
        bdlbb::BlobUtil::append(&blob, "Hello, world!", 13);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(10)
{
    // Concern: bsl::shared_ptr<bdlbb::Blob>
    // Plan:

    ntscfg::TestAllocator ta;
    {
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(11)
{
    // Concern: bsl::string
    // Plan:

    ntscfg::TestAllocator ta;
    {
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(12)
{
    // Concern: ntsa::File
    // Plan:

    ntscfg::TestAllocator ta;
    {
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(13)
{
    // Concern: Append: undefined
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(
            test::BLOB_BUFFER_SIZE,
            &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        {
            ntsa::Data data(&ta);
            ntsa::DataUtil::append(&blob, data);
        }

        NTSCFG_TEST_EQ(blob.length(), 0);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(14)
{
    // Concern: Append: bdlbb::BlobBuffer
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(
            test::BLOB_BUFFER_SIZE,
            &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        {
            ntsa::Data data(&ta);
            {
                bdlbb::BlobBuffer& blobBuffer = data.makeBlobBuffer();

                bsl::shared_ptr<char> blobBufferData(
                    static_cast<char*>(ta.allocate(test::DATA_SIZE)),
                    &ta,
                    0);

                bsl::memcpy(blobBufferData.get(), test::DATA, test::DATA_SIZE);
                blobBuffer.reset(blobBufferData, test::DATA_SIZE);
            }

            ntsa::DataUtil::append(&blob, data);
        }

        NTSCFG_TEST_TRUE(test::BlobUtil::equals(blob, test::DATA));
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(15)
{
    // Concern: Append: ntsa::ConstBuffer
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(
            test::BLOB_BUFFER_SIZE,
            &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        {
            ntsa::Data data(&ta);
            {
                ntsa::ConstBuffer& constBuffer = data.makeConstBuffer();
                constBuffer.setBuffer(test::DATA, test::DATA_SIZE);
            }

            ntsa::DataUtil::append(&blob, data);
        }

        NTSCFG_TEST_TRUE(test::BlobUtil::equals(blob, test::DATA));
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(16)
{
    // Concern: Append: ntsa::ConstBufferArray
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(
            test::BLOB_BUFFER_SIZE,
            &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        {
            ntsa::Data data(&ta);
            {
                ntsa::ConstBufferArray& constBufferArray =
                    data.makeConstBufferArray();

                constBufferArray.append(test::DATA_A, test::DATA_A_SIZE);
                constBufferArray.append(test::DATA_B, test::DATA_B_SIZE);
                constBufferArray.append(test::DATA_C, test::DATA_C_SIZE);
            }

            ntsa::DataUtil::append(&blob, data);
        }

        NTSCFG_TEST_TRUE(test::BlobUtil::equals(blob,
                                                test::DATA_A,
                                                test::DATA_B,
                                                test::DATA_C));
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(17)
{
    // Concern: Append: ntsa::ConstBufferPtrArray
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(
            test::BLOB_BUFFER_SIZE,
            &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        ntsa::ConstBuffer constBufferArray[3];
        constBufferArray[0].setBuffer(test::DATA_A, test::DATA_A_SIZE);
        constBufferArray[1].setBuffer(test::DATA_B, test::DATA_B_SIZE);
        constBufferArray[2].setBuffer(test::DATA_C, test::DATA_C_SIZE);

        {
            ntsa::Data data(&ta);
            {
                ntsa::ConstBufferPtrArray& constBufferPtrArray =
                    data.makeConstBufferPtrArray();

                constBufferPtrArray.assign(constBufferArray, 3);
            }

            ntsa::DataUtil::append(&blob, data);
        }

        NTSCFG_TEST_TRUE(test::BlobUtil::equals(blob,
                                                test::DATA_A,
                                                test::DATA_B,
                                                test::DATA_C));
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(18)
{
    // Concern: Append: ntsa::MutableBuffer
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(
            test::BLOB_BUFFER_SIZE,
            &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        bsl::string mutableBufferStorage = test::DATA;

        {
            ntsa::Data data(&ta);
            {
                ntsa::MutableBuffer& mutableBuffer = data.makeMutableBuffer();

                mutableBuffer.setBuffer(&mutableBufferStorage[0],
                                        mutableBufferStorage.size());
            }

            ntsa::DataUtil::append(&blob, data);
        }

        NTSCFG_TEST_TRUE(test::BlobUtil::equals(blob, test::DATA));
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(19)
{
    // Concern: Append: ntsa::MutableBufferArray
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(
            test::BLOB_BUFFER_SIZE,
            &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        bsl::string mutableBufferStorageA = test::DATA_A;
        bsl::string mutableBufferStorageB = test::DATA_B;
        bsl::string mutableBufferStorageC = test::DATA_C;

        {
            ntsa::Data data(&ta);
            {
                ntsa::MutableBufferArray& mutableBufferArray =
                    data.makeMutableBufferArray();

                mutableBufferArray.append(&mutableBufferStorageA[0],
                                          mutableBufferStorageA.size());

                mutableBufferArray.append(&mutableBufferStorageB[0],
                                          mutableBufferStorageB.size());

                mutableBufferArray.append(&mutableBufferStorageC[0],
                                          mutableBufferStorageC.size());
            }

            ntsa::DataUtil::append(&blob, data);
        }

        NTSCFG_TEST_TRUE(test::BlobUtil::equals(blob,
                                                test::DATA_A,
                                                test::DATA_B,
                                                test::DATA_C));
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(20)
{
    // Concern: Append: ntsa::MutableBufferPtrArray
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(
            test::BLOB_BUFFER_SIZE,
            &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        bsl::string mutableBufferStorageA = test::DATA_A;
        bsl::string mutableBufferStorageB = test::DATA_B;
        bsl::string mutableBufferStorageC = test::DATA_C;

        ntsa::MutableBuffer mutableBufferArray[3];

        mutableBufferArray[0].setBuffer(&mutableBufferStorageA[0],
                                        mutableBufferStorageA.size());

        mutableBufferArray[1].setBuffer(&mutableBufferStorageB[0],
                                        mutableBufferStorageB.size());

        mutableBufferArray[2].setBuffer(&mutableBufferStorageC[0],
                                        mutableBufferStorageC.size());

        {
            ntsa::Data data(&ta);
            {
                ntsa::MutableBufferPtrArray& mutableBufferPtrArray =
                    data.makeMutableBufferPtrArray();

                mutableBufferPtrArray.assign(mutableBufferArray, 3);
            }

            ntsa::DataUtil::append(&blob, data);
        }

        NTSCFG_TEST_TRUE(test::BlobUtil::equals(blob,
                                                test::DATA_A,
                                                test::DATA_B,
                                                test::DATA_C));
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(21)
{
    // Concern: Append: bdlbb::Blob
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(
            test::BLOB_BUFFER_SIZE,
            &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        {
            ntsa::Data data(&blobBufferFactory, &ta);
            {
                bdlbb::Blob& dataBlob = data.makeBlob();

                bdlbb::BlobUtil::append(&dataBlob,
                                        test::DATA_A,
                                        test::DATA_A_SIZE);
                bdlbb::BlobUtil::append(&dataBlob,
                                        test::DATA_B,
                                        test::DATA_B_SIZE);
                bdlbb::BlobUtil::append(&dataBlob,
                                        test::DATA_C,
                                        test::DATA_C_SIZE);
            }

            ntsa::DataUtil::append(&blob, data);
        }

        NTSCFG_TEST_TRUE(test::BlobUtil::equals(blob,
                                                test::DATA_A,
                                                test::DATA_B,
                                                test::DATA_C));
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(22)
{
    // Concern: Append: bsl::shared_ptr<bdlbb::Blob>
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(
            test::BLOB_BUFFER_SIZE,
            &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        {
            ntsa::Data data(&blobBufferFactory, &ta);
            {
                bsl::shared_ptr<bdlbb::Blob>& dataSharedBlob =
                    data.makeSharedBlob();
                dataSharedBlob.createInplace(&ta, &blobBufferFactory, &ta);

                bdlbb::BlobUtil::append(dataSharedBlob.get(),
                                        test::DATA_A,
                                        test::DATA_A_SIZE);
                bdlbb::BlobUtil::append(dataSharedBlob.get(),
                                        test::DATA_B,
                                        test::DATA_B_SIZE);
                bdlbb::BlobUtil::append(dataSharedBlob.get(),
                                        test::DATA_C,
                                        test::DATA_C_SIZE);
            }

            ntsa::DataUtil::append(&blob, data);
        }

        NTSCFG_TEST_TRUE(test::BlobUtil::equals(blob,
                                                test::DATA_A,
                                                test::DATA_B,
                                                test::DATA_C));
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(23)
{
    // Concern: Append: bsl::string
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bdlbb::PooledBlobBufferFactory blobBufferFactory(
            test::BLOB_BUFFER_SIZE,
            &ta);

        bdlbb::Blob blob(&blobBufferFactory, &ta);

        {
            ntsa::Data data(&ta);
            {
                bsl::string& dataString = data.makeString();
                dataString.assign(test::DATA, test::DATA_SIZE);
            }

            ntsa::DataUtil::append(&blob, data);
        }

        NTSCFG_TEST_TRUE(test::BlobUtil::equals(blob, test::DATA));
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(24)
{
    // Concern: Append: ntsa::File
    // Plan:

    ntscfg::TestAllocator ta;
    {
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
    NTSCFG_TEST_REGISTER(4);
    NTSCFG_TEST_REGISTER(5);
    NTSCFG_TEST_REGISTER(6);
    NTSCFG_TEST_REGISTER(7);
    NTSCFG_TEST_REGISTER(8);
    NTSCFG_TEST_REGISTER(9);
    NTSCFG_TEST_REGISTER(10);
    NTSCFG_TEST_REGISTER(11);
    NTSCFG_TEST_REGISTER(12);

    NTSCFG_TEST_REGISTER(13);
    NTSCFG_TEST_REGISTER(14);
    NTSCFG_TEST_REGISTER(15);
    NTSCFG_TEST_REGISTER(16);
    NTSCFG_TEST_REGISTER(17);
    NTSCFG_TEST_REGISTER(18);
    NTSCFG_TEST_REGISTER(19);
    NTSCFG_TEST_REGISTER(20);
    NTSCFG_TEST_REGISTER(21);
    NTSCFG_TEST_REGISTER(22);
    NTSCFG_TEST_REGISTER(23);
    NTSCFG_TEST_REGISTER(24);
}
NTSCFG_TEST_DRIVER_END;
