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
BSLS_IDENT_RCSID(ntsa_data_t_cpp, "$Id$ $CSID$")

#include <ntsa_data.h>

#include <ntsa_temporary.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Data'.
class DataTest
{
    static const bsl::size_t k_BLOB_BUFFER_SIZE;

    static const char        k_DATA[];
    static const bsl::size_t k_DATA_SIZE;

    static const char        k_DATA_A[];
    static const bsl::size_t k_DATA_A_SIZE;

    static const char        k_DATA_B[];
    static const bsl::size_t k_DATA_B_SIZE;

    static const char        k_DATA_C[];
    static const bsl::size_t k_DATA_C_SIZE;

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

  public:
    // Verify selection: undefined.
    static void verifyCase1();

    // Verify selection: bdlbb::BlobBuffer
    static void verifyCase2();

    // Verify selection: ntsa::ConstBuffer
    static void verifyCase3();

    // Verify selection: ntsa::ConstBufferArray
    static void verifyCase4();

    // Verify selection: ntsa::ConstBufferPtrArray
    static void verifyCase5();

    // Verify selection: ntsa::MutableBuffer
    static void verifyCase6();

    // Verify selection: ntsa::MutableBufferArray
    static void verifyCase7();

    // Verify selection: ntsa::MutableBufferPtrArray
    static void verifyCase8();

    // Verify selection: bdlbb::Blob
    static void verifyCase9();

    // Verify selection: bsl::shared_ptr<bdlbb::Blob>
    static void verifyCase10();

     // Verify selection: bsl::string
    static void verifyCase11();

    // Verify selection: ntsa::File
    static void verifyCase12();

    // Verify append: undefined
    static void verifyCase13();

    // Verify append: bdlbb::BlobBuffer
    static void verifyCase14();

    // Verify append: ntsa::ConstBuffer
    static void verifyCase15();

    // Verify append: ntsa::ConstBufferArray
    static void verifyCase16();

    // Verify append: ntsa::ConstBufferPtrArray
    static void verifyCase17();

    // Verify append: ntsa::MutableBuffer
    static void verifyCase18();

    // Verify append: ntsa::MutableBufferArray
    static void verifyCase19();

    // Verify append: ntsa::MutableBufferPtrArray
    static void verifyCase20();

    // Verify append: bdlbb::Blob
    static void verifyCase21();

    // Verify append: bsl::shared_ptr<bdlbb::Blob>
    static void verifyCase22();

    // Verify append: bsl::string
    static void verifyCase23();

    // Verify append: ntsa::File
    static void verifyCase24();

    // Verify copy: undefined 
    static void verifyCase25();

    // Verify copy: bdlbb::BlobBuffer
    static void verifyCase26();

    // Verify copy: ntsa::ConstBuffer
    static void verifyCase27();

    // Verify copy: ntsa::ConstBufferArray
    static void verifyCase28();

    // Verify copy: ntsa::ConstBufferPtrArray
    static void verifyCase29();

    // Verify copy: ntsa::MutableBuffer
    static void verifyCase30();

    // Verify copy: ntsa::MutableBufferArray
    static void verifyCase31();

    // Verify copy: ntsa::MutableBufferPtrArray
    static void verifyCase32();

    // Verify copy: bdlbb::Blob
    static void verifyCase33();

    // Verify copy: bsl::shared_ptr<bdlbb::Blob>
    static void verifyCase34();

    // Verify copy: bsl::string
    static void verifyCase35();

    // Verify copy: ntsa::File
    static void verifyCase36();
};

const bsl::size_t DataTest::k_BLOB_BUFFER_SIZE = 4;

const char        DataTest::k_DATA[]    = "Hello, world!";
const bsl::size_t DataTest::k_DATA_SIZE = sizeof(DataTest::k_DATA) - 1;

const char        DataTest::k_DATA_A[]    = "Hello, world! ";
const bsl::size_t DataTest::k_DATA_A_SIZE = sizeof(DataTest::k_DATA_A) - 1;

const char        DataTest::k_DATA_B[] = "This is a test of the data utility algorithms. ";
const bsl::size_t DataTest::k_DATA_B_SIZE = sizeof(DataTest::k_DATA_B) - 1;

const char        DataTest::k_DATA_C[]    = "The blob should match its expected value. ";
const bsl::size_t DataTest::k_DATA_C_SIZE = sizeof(DataTest::k_DATA_C) - 1;

bool DataTest::equals(const bdlbb::Blob& blob, const bsl::string& string)
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

bool DataTest::equals(const bdlbb::Blob& blob,
                      const bsl::string& stringA,
                      const bsl::string& stringB,
                      const bsl::string& stringC)
{
    return DataTest::equals(blob, stringA + stringB + stringC);
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase1)
{
    ntsa::Data data(NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(data.type(), ntsa::DataType::e_UNDEFINED);
    NTSCFG_TEST_TRUE(data.isUndefined());
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase2)
{
    ntsa::Data data(NTSCFG_TEST_ALLOCATOR);

    bdlbb::BlobBuffer& blobBuffer = data.makeBlobBuffer();
    {
        bsl::shared_ptr<char> blobBufferData(
            static_cast<char*>(NTSCFG_TEST_ALLOCATOR->allocate(64)),
            NTSCFG_TEST_ALLOCATOR,
            0);

        blobBuffer.reset(blobBufferData, 64);
    }

    NTSCFG_TEST_EQ(data.type(), ntsa::DataType::e_BLOB_BUFFER);
    NTSCFG_TEST_TRUE(data.isBlobBuffer());
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase3)
{
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase4)
{
    ntsa::Data data(NTSCFG_TEST_ALLOCATOR);

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

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase5)
{
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase6)
{
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase7)
{
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase8)
{
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase9)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(1024, NTSCFG_TEST_ALLOCATOR);

    ntsa::Data data(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob& blob = data.makeBlob();
    bdlbb::BlobUtil::append(&blob, "Hello, world!", 13);
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase10)
{
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase11)
{
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase12)
{
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase13)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
        ntsa::DataUtil::append(&blob, data);
    }

    NTSCFG_TEST_EQ(blob.length(), 0);
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase14)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
        {
            bdlbb::BlobBuffer& blobBuffer = data.makeBlobBuffer();

            bsl::shared_ptr<char> blobBufferData(
                static_cast<char*>(NTSCFG_TEST_ALLOCATOR->allocate(k_DATA_SIZE)),
                NTSCFG_TEST_ALLOCATOR,
                0);

            bsl::memcpy(blobBufferData.get(), k_DATA, k_DATA_SIZE);
            blobBuffer.reset(blobBufferData, k_DATA_SIZE);
        }

        ntsa::DataUtil::append(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob, k_DATA));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase15)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
        {
            ntsa::ConstBuffer& constBuffer = data.makeConstBuffer();
            constBuffer.setBuffer(k_DATA, k_DATA_SIZE);
        }

        ntsa::DataUtil::append(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob, k_DATA));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase16)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
        {
            ntsa::ConstBufferArray& constBufferArray =
                data.makeConstBufferArray();

            constBufferArray.append(k_DATA_A, k_DATA_A_SIZE);
            constBufferArray.append(k_DATA_B, k_DATA_B_SIZE);
            constBufferArray.append(k_DATA_C, k_DATA_C_SIZE);
        }

        ntsa::DataUtil::append(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob,
                                            k_DATA_A,
                                            k_DATA_B,
                                            k_DATA_C));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase17)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    ntsa::ConstBuffer constBufferArray[3];
    constBufferArray[0].setBuffer(k_DATA_A, k_DATA_A_SIZE);
    constBufferArray[1].setBuffer(k_DATA_B, k_DATA_B_SIZE);
    constBufferArray[2].setBuffer(k_DATA_C, k_DATA_C_SIZE);

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
        {
            ntsa::ConstBufferPtrArray& constBufferPtrArray =
                data.makeConstBufferPtrArray();

            constBufferPtrArray.assign(constBufferArray, 3);
        }

        ntsa::DataUtil::append(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob,
                                            k_DATA_A,
                                            k_DATA_B,
                                            k_DATA_C));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase18)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    bsl::string mutableBufferStorage = k_DATA;

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
        {
            ntsa::MutableBuffer& mutableBuffer = data.makeMutableBuffer();

            mutableBuffer.setBuffer(&mutableBufferStorage[0],
                                    mutableBufferStorage.size());
        }

        ntsa::DataUtil::append(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob, k_DATA));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase19)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    bsl::string mutableBufferStorageA = k_DATA_A;
    bsl::string mutableBufferStorageB = k_DATA_B;
    bsl::string mutableBufferStorageC = k_DATA_C;

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
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

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob,
                                            k_DATA_A,
                                            k_DATA_B,
                                            k_DATA_C));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase20)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    bsl::string mutableBufferStorageA = k_DATA_A;
    bsl::string mutableBufferStorageB = k_DATA_B;
    bsl::string mutableBufferStorageC = k_DATA_C;

    ntsa::MutableBuffer mutableBufferArray[3];

    mutableBufferArray[0].setBuffer(&mutableBufferStorageA[0],
                                    mutableBufferStorageA.size());

    mutableBufferArray[1].setBuffer(&mutableBufferStorageB[0],
                                    mutableBufferStorageB.size());

    mutableBufferArray[2].setBuffer(&mutableBufferStorageC[0],
                                    mutableBufferStorageC.size());

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
        {
            ntsa::MutableBufferPtrArray& mutableBufferPtrArray =
                data.makeMutableBufferPtrArray();

            mutableBufferPtrArray.assign(mutableBufferArray, 3);
        }

        ntsa::DataUtil::append(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob,
                                            k_DATA_A,
                                            k_DATA_B,
                                            k_DATA_C));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase21)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    {
        ntsa::Data data(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);
        {
            bdlbb::Blob& dataBlob = data.makeBlob();

            bdlbb::BlobUtil::append(&dataBlob,
                                    k_DATA_A,
                                    k_DATA_A_SIZE);
            bdlbb::BlobUtil::append(&dataBlob,
                                    k_DATA_B,
                                    k_DATA_B_SIZE);
            bdlbb::BlobUtil::append(&dataBlob,
                                    k_DATA_C,
                                    k_DATA_C_SIZE);
        }

        ntsa::DataUtil::append(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob,
                                            k_DATA_A,
                                            k_DATA_B,
                                            k_DATA_C));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase22)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    {
        ntsa::Data data(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);
        {
            bsl::shared_ptr<bdlbb::Blob>& dataSharedBlob =
                data.makeSharedBlob();
            dataSharedBlob.createInplace(NTSCFG_TEST_ALLOCATOR, &blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

            bdlbb::BlobUtil::append(dataSharedBlob.get(),
                                    k_DATA_A,
                                    k_DATA_A_SIZE);
            bdlbb::BlobUtil::append(dataSharedBlob.get(),
                                    k_DATA_B,
                                    k_DATA_B_SIZE);
            bdlbb::BlobUtil::append(dataSharedBlob.get(),
                                    k_DATA_C,
                                    k_DATA_C_SIZE);
        }

        ntsa::DataUtil::append(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob,
                                            k_DATA_A,
                                            k_DATA_B,
                                            k_DATA_C));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase23)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
        {
            bsl::string& dataString = data.makeString();
            dataString.assign(k_DATA, k_DATA_SIZE);
        }

        ntsa::DataUtil::append(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob, k_DATA));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase24)
{
    ntsa::Error error;

    ntsa::TemporaryFile tempFile(NTSCFG_TEST_ALLOCATOR);

    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    error = tempFile.write(bsl::string(k_DATA, k_DATA_SIZE));
    NTSCFG_TEST_OK(error);

    bdls::FilesystemUtil::FileDescriptor fileDescriptor =
        bdls::FilesystemUtil::open(tempFile.path().c_str(),
                                    bdls::FilesystemUtil::e_OPEN,
                                    bdls::FilesystemUtil::e_READ_ONLY);

    NTSCFG_TEST_NE(fileDescriptor, bdls::FilesystemUtil::k_INVALID_FD);

    ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
    {
        ntsa::File& file = data.makeFile();

        file.setDescriptor(fileDescriptor);
        file.setBytesRemaining(k_DATA_SIZE);
    }

    ntsa::DataUtil::append(&blob, data);

    bdls::FilesystemUtil::close(fileDescriptor);

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob, k_DATA));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase25)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
        ntsa::DataUtil::copy(&blob, data);
    }

    NTSCFG_TEST_EQ(blob.length(), 0);
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase26)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
        {
            bdlbb::BlobBuffer& blobBuffer = data.makeBlobBuffer();

            bsl::shared_ptr<char> blobBufferData(
                static_cast<char*>(NTSCFG_TEST_ALLOCATOR->allocate(k_DATA_SIZE)),
                NTSCFG_TEST_ALLOCATOR,
                0);

            bsl::memcpy(blobBufferData.get(), k_DATA, k_DATA_SIZE);
            blobBuffer.reset(blobBufferData, k_DATA_SIZE);
        }

        ntsa::DataUtil::copy(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob, k_DATA));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase27)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
        {
            ntsa::ConstBuffer& constBuffer = data.makeConstBuffer();
            constBuffer.setBuffer(k_DATA, k_DATA_SIZE);
        }

        ntsa::DataUtil::copy(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob, k_DATA));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase28)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
        {
            ntsa::ConstBufferArray& constBufferArray =
                data.makeConstBufferArray();

            constBufferArray.append(k_DATA_A, k_DATA_A_SIZE);
            constBufferArray.append(k_DATA_B, k_DATA_B_SIZE);
            constBufferArray.append(k_DATA_C, k_DATA_C_SIZE);
        }

        ntsa::DataUtil::copy(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob,
                                            k_DATA_A,
                                            k_DATA_B,
                                            k_DATA_C));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase29)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    ntsa::ConstBuffer constBufferArray[3];
    constBufferArray[0].setBuffer(k_DATA_A, k_DATA_A_SIZE);
    constBufferArray[1].setBuffer(k_DATA_B, k_DATA_B_SIZE);
    constBufferArray[2].setBuffer(k_DATA_C, k_DATA_C_SIZE);

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
        {
            ntsa::ConstBufferPtrArray& constBufferPtrArray =
                data.makeConstBufferPtrArray();

            constBufferPtrArray.assign(constBufferArray, 3);
        }

        ntsa::DataUtil::copy(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob,
                                            k_DATA_A,
                                            k_DATA_B,
                                            k_DATA_C));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase30)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    bsl::string mutableBufferStorage = k_DATA;

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
        {
            ntsa::MutableBuffer& mutableBuffer = data.makeMutableBuffer();

            mutableBuffer.setBuffer(&mutableBufferStorage[0],
                                    mutableBufferStorage.size());
        }

        ntsa::DataUtil::copy(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob, k_DATA));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase31)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    bsl::string mutableBufferStorageA = k_DATA_A;
    bsl::string mutableBufferStorageB = k_DATA_B;
    bsl::string mutableBufferStorageC = k_DATA_C;

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
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

        ntsa::DataUtil::copy(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob,
                                            k_DATA_A,
                                            k_DATA_B,
                                            k_DATA_C));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase32)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    bsl::string mutableBufferStorageA = k_DATA_A;
    bsl::string mutableBufferStorageB = k_DATA_B;
    bsl::string mutableBufferStorageC = k_DATA_C;

    ntsa::MutableBuffer mutableBufferArray[3];

    mutableBufferArray[0].setBuffer(&mutableBufferStorageA[0],
                                    mutableBufferStorageA.size());

    mutableBufferArray[1].setBuffer(&mutableBufferStorageB[0],
                                    mutableBufferStorageB.size());

    mutableBufferArray[2].setBuffer(&mutableBufferStorageC[0],
                                    mutableBufferStorageC.size());

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
        {
            ntsa::MutableBufferPtrArray& mutableBufferPtrArray =
                data.makeMutableBufferPtrArray();

            mutableBufferPtrArray.assign(mutableBufferArray, 3);
        }

        ntsa::DataUtil::copy(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob,
                                            k_DATA_A,
                                            k_DATA_B,
                                            k_DATA_C));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase33)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    {
        ntsa::Data data(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);
        {
            bdlbb::Blob& dataBlob = data.makeBlob();

            bdlbb::BlobUtil::append(&dataBlob,
                                    k_DATA_A,
                                    k_DATA_A_SIZE);
            bdlbb::BlobUtil::append(&dataBlob,
                                    k_DATA_B,
                                    k_DATA_B_SIZE);
            bdlbb::BlobUtil::append(&dataBlob,
                                    k_DATA_C,
                                    k_DATA_C_SIZE);
        }

        ntsa::DataUtil::copy(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob,
                                            k_DATA_A,
                                            k_DATA_B,
                                            k_DATA_C));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase34)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    {
        ntsa::Data data(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);
        {
            bsl::shared_ptr<bdlbb::Blob>& dataSharedBlob =
                data.makeSharedBlob();
            dataSharedBlob.createInplace(NTSCFG_TEST_ALLOCATOR, &blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

            bdlbb::BlobUtil::append(dataSharedBlob.get(),
                                    k_DATA_A,
                                    k_DATA_A_SIZE);
            bdlbb::BlobUtil::append(dataSharedBlob.get(),
                                    k_DATA_B,
                                    k_DATA_B_SIZE);
            bdlbb::BlobUtil::append(dataSharedBlob.get(),
                                    k_DATA_C,
                                    k_DATA_C_SIZE);
        }

        ntsa::DataUtil::copy(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob,
                                            k_DATA_A,
                                            k_DATA_B,
                                            k_DATA_C));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase35)
{
    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    {
        ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
        {
            bsl::string& dataString = data.makeString();
            dataString.assign(k_DATA, k_DATA_SIZE);
        }

        ntsa::DataUtil::copy(&blob, data);
    }

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob, k_DATA));
}

NTSCFG_TEST_FUNCTION(ntsa::DataTest::verifyCase36)
{
    ntsa::Error error;

    ntsa::TemporaryFile tempFile(NTSCFG_TEST_ALLOCATOR);

    bdlbb::PooledBlobBufferFactory blobBufferFactory(
        k_BLOB_BUFFER_SIZE,
        NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    error = tempFile.write(bsl::string(k_DATA, k_DATA_SIZE));
    NTSCFG_TEST_OK(error);

    bdls::FilesystemUtil::FileDescriptor fileDescriptor =
        bdls::FilesystemUtil::open(tempFile.path().c_str(),
                                    bdls::FilesystemUtil::e_OPEN,
                                    bdls::FilesystemUtil::e_READ_ONLY);

    NTSCFG_TEST_NE(fileDescriptor, bdls::FilesystemUtil::k_INVALID_FD);

    ntsa::Data data(NTSCFG_TEST_ALLOCATOR);
    {
        ntsa::File& file = data.makeFile();

        file.setDescriptor(fileDescriptor);
        file.setBytesRemaining(k_DATA_SIZE);
    }

    error = ntsa::DataUtil::copy(&blob, data);
    NTSCFG_TEST_OK(error);

    bdls::FilesystemUtil::close(fileDescriptor);

    NTSCFG_TEST_TRUE(ntsa::DataTest::equals(blob, k_DATA));
}

}  // close namespace ntsa
}  // close namespace BloombergLP
