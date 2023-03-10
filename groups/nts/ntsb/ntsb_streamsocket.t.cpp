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

#include <ntsb_streamsocket.h>

#include <ntscfg_test.h>
#include <ntsu_adapterutil.h>
#include <ntsu_socketoptionutil.h>
#include <ntsu_socketutil.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobutil.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslmt_threadgroup.h>
#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

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

namespace {

/// Provide a suite of utilities for generating test data.
/// This struct is completely thread safe.
struct DataUtil {
    /// Return the byte at the specified 'position' in the specified
    /// 'dataset'.
    static char generateByte(int position, int dataset);

    /// Load into the specified 'result' the specified 'size' sequence of
    /// bytes from the specified 'dataset' starting at the specified
    /// 'offset'.
    static void generateData(bsl::string* result,
                             int          size,
                             int          offset  = 0,
                             int          dataset = 0);

    /// Load into the specified 'result' the specified 'size' sequence of
    /// bytes from the specified 'dataset' starting at the specified
    /// 'offset'.
    static void generateData(bdlbb::Blob* result,
                             int          size,
                             int          offset  = 0,
                             int          dataset = 0);
};

char DataUtil::generateByte(int position, int dataset)
{
    struct {
        const char* source;
        int         length;
    } DATA[] = {
        {"abcdefghijklmnopqrstuvwxyz", 26},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26}
    };

    enum { NUM_DATA = sizeof(DATA) / sizeof(*DATA) };

    dataset = dataset % NUM_DATA;
    return DATA[dataset].source[position % DATA[dataset].length];
}

void DataUtil::generateData(bsl::string* result,
                            int          size,
                            int          offset,
                            int          dataset)
{
    result->clear();
    result->resize(size);

    for (int i = offset; i < offset + size; ++i) {
        (*result)[i] = generateByte(offset + i, dataset);
    }
}

void DataUtil::generateData(bdlbb::Blob* result,
                            int          size,
                            int          offset,
                            int          dataset)
{
    result->removeAll();
    result->setLength(size);

    int k = 0;

    for (int i = 0; i < result->numDataBuffers(); ++i) {
        const bdlbb::BlobBuffer& buffer = result->buffer(i);

        int numBytesToWrite = i == result->numDataBuffers() - 1
                                  ? result->lastDataBufferLength()
                                  : buffer.size();

        for (int j = 0; j < numBytesToWrite; ++j) {
            buffer.data()[j] = generateByte(offset + k, dataset);
            ++k;
        }
    }
}

/// Provide utilities for testing stream sockets.
struct StreamSocketUtil {
    /// Send all the specified 'data' having the specified 'size' through
    /// the specified 'socket', using as many system calls as necessary.
    /// Return the error.
    static ntsa::Error sendAll(const void*  data,
                               bsl::size_t  size,
                               ntsa::Handle socket);

    /// Receive all the specified 'size' bytes into specified 'data' from
    /// the specified 'socket', using as many system calls as necessary.
    /// Return the error.
    static ntsa::Error receiveAll(void*        data,
                                  bsl::size_t  size,
                                  ntsa::Handle socket);

    /// Send all the specified 'data' through the specified 'socket', using
    /// as many system calls as necessary. Return the error.
    static ntsa::Error sendAll(const bdlbb::Blob& data, ntsa::Handle socket);

    /// Receive all the specified 'data->totalSize()' bytes into specified
    /// 'data' from the specified 'socket', using as many system calls as
    /// necessary. Return the error.
    static ntsa::Error receiveAll(bdlbb::Blob* data, ntsa::Handle socket);
};

ntsa::Error StreamSocketUtil::sendAll(const void*  data,
                                      bsl::size_t  size,
                                      ntsa::Handle socket)
{
    if (size == 0) {
        return ntsa::Error();
    }

    NTSCFG_TEST_TRUE(data);

    bsl::size_t numBytesRemaining = size;
    const char* position          = static_cast<const char*>(data);

    while (numBytesRemaining > 0) {
        ntsa::SendContext context;
        ntsa::SendOptions options;

        ntsa::Data data(ntsa::ConstBuffer(position, numBytesRemaining));

        ntsa::Error error =
            ntsu::SocketUtil::send(&context, data, options, socket);
        if (error) {
            if (error == ntsa::Error::e_INTERRUPTED) {
                continue;
            }
            return error;
        }

        BSLS_LOG_DEBUG("[+]"
                       "\nnumBytesSendable:   %d"
                       "\nnumBytesSent:       %d",
                       context.bytesSendable(),
                       context.bytesSent());

        NTSCFG_TEST_LE(context.bytesSent(), numBytesRemaining);

        position          += context.bytesSent();
        numBytesRemaining -= context.bytesSent();
    }

    NTSCFG_TEST_EQ(numBytesRemaining, 0);
    NTSCFG_TEST_EQ(position, static_cast<const char*>(data) + size);

    return ntsa::Error();
}

ntsa::Error StreamSocketUtil::receiveAll(void*        data,
                                         bsl::size_t  size,
                                         ntsa::Handle socket)
{
    if (size == 0) {
        return ntsa::Error();
    }

    NTSCFG_TEST_TRUE(data);

    bsl::size_t numBytesRemaining = size;
    char*       position          = static_cast<char*>(data);

    while (numBytesRemaining > 0) {
        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        ntsa::Data data(ntsa::MutableBuffer(position, numBytesRemaining));

        ntsa::Error error =
            ntsu::SocketUtil::receive(&context, &data, options, socket);
        if (error) {
            if (error == ntsa::Error::e_INTERRUPTED) {
                continue;
            }
            return error;
        }

        BSLS_LOG_DEBUG("[-]"
                       "\nnumBytesReceivable: %d"
                       "\nnumBytesReceived:   %d",
                       context.bytesReceivable(),
                       context.bytesReceived());

        NTSCFG_TEST_LE(context.bytesReceived(), numBytesRemaining);

        position          += context.bytesReceived();
        numBytesRemaining -= context.bytesReceived();
    }

    NTSCFG_TEST_EQ(numBytesRemaining, 0);
    NTSCFG_TEST_EQ(position, static_cast<char*>(data) + size);

    return ntsa::Error();
}

ntsa::Error StreamSocketUtil::sendAll(const bdlbb::Blob& data,
                                      ntsa::Handle       socket)
{
    bdlbb::Blob dataRemaining = data;

    while (dataRemaining.length() > 0) {
        ntsa::SendContext context;
        ntsa::SendOptions options;

        ntsa::Error error =
            ntsu::SocketUtil::send(&context, dataRemaining, options, socket);
        if (error) {
            if (error == ntsa::Error::e_INTERRUPTED) {
                continue;
            }
            return error;
        }

        BSLS_LOG_DEBUG("[+]"
                       "\nnumBytesSendable:   %d"
                       "\nnumBytesSent:       %d",
                       context.bytesSendable(),
                       context.bytesSent());

        bdlbb::BlobUtil::erase(&dataRemaining, 0, context.bytesSent());
    }

    return ntsa::Error();
}

ntsa::Error StreamSocketUtil::receiveAll(bdlbb::Blob* data,
                                         ntsa::Handle socket)
{
    do {
        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        ntsa::Error error =
            ntsu::SocketUtil::receive(&context, data, options, socket);
        if (error) {
            if (error == ntsa::Error::e_INTERRUPTED) {
                continue;
            }
            return error;
        }

        BSLS_LOG_DEBUG("[-]"
                       "\nnumBytesReceivable: %d"
                       "\nnumBytesReceived:   %d",
                       context.bytesReceivable(),
                       context.bytesReceived());
    } while (data->length() < data->totalSize());

    return ntsa::Error();
}

/// Provide utilities for sending and receiving data.
struct AsyncUtil {
    /// Send from the specified 'client' all the specified 'clientData'
    /// to the peer of the 'client'.
    static void sendString(const bsl::shared_ptr<ntsb::StreamSocket>& client,
                           const bsl::string* clientData);

    /// Receive from the specified 'server' into the pre-ressized
    /// 'serverData' all the data sent by the peer of the 'server'.
    static void receiveString(
        const bsl::shared_ptr<ntsb::StreamSocket>& server,
        bsl::string*                               serverData);

    /// Send from the specified 'client' all the specified 'clientData'
    /// to the peer of the 'client'.
    static void sendBlob(const bsl::shared_ptr<ntsb::StreamSocket>& client,
                         const bdlbb::Blob* clientData);

    /// Receive from the specified 'server' into the pre-ressized
    /// 'serverData' all the data sent by the peer of the 'server'.
    static void receiveBlob(const bsl::shared_ptr<ntsb::StreamSocket>& server,
                            bdlbb::Blob* serverData);
};

void AsyncUtil::sendString(const bsl::shared_ptr<ntsb::StreamSocket>& client,
                           const bsl::string* clientData)
{
    ntsa::Error error = StreamSocketUtil::sendAll(&(*clientData)[0],
                                                  clientData->size(),
                                                  client->handle());
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
}

void AsyncUtil::receiveString(
    const bsl::shared_ptr<ntsb::StreamSocket>& server,
    bsl::string*                               serverData)
{
    ntsa::Error error = StreamSocketUtil::receiveAll(&(*serverData)[0],
                                                     serverData->size(),
                                                     server->handle());
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
}

void AsyncUtil::sendBlob(const bsl::shared_ptr<ntsb::StreamSocket>& client,
                         const bdlbb::Blob*                         clientData)
{
    ntsa::Error error =
        StreamSocketUtil::sendAll(*clientData, client->handle());
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
}

void AsyncUtil::receiveBlob(const bsl::shared_ptr<ntsb::StreamSocket>& server,
                            bdlbb::Blob* serverData)
{
    ntsa::Error error =
        StreamSocketUtil::receiveAll(serverData, server->handle());
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
}

/// Provide facilities to test different stream sockets
/// conveniently in one place.
struct StreamSocketTester {
    /// Test the implementations of the specified 'client' and 'server'
    /// send and receive data correctly using basic, contiguous buffers.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    static void testBufferIO(const bsl::shared_ptr<ntsb::StreamSocket>& client,
                             const bsl::shared_ptr<ntsb::StreamSocket>& server,
                             bslma::Allocator* basicAllocator = 0);

    /// Test the implementations of the specified 'client' and 'server'
    /// send and receive data correctly using vectored I/O and the
    /// scatter/gather paradigm. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static void testVectorIO(const bsl::shared_ptr<ntsb::StreamSocket>& client,
                             const bsl::shared_ptr<ntsb::StreamSocket>& server,
                             bslma::Allocator* basicAllocator = 0);
};

void StreamSocketTester::testBufferIO(
    const bsl::shared_ptr<ntsb::StreamSocket>& client,
    const bsl::shared_ptr<ntsb::StreamSocket>& server,
    bslma::Allocator*                          basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntsa::Error error;

    const bsl::size_t SIZE = 1024 * 1024 * 64;

    bsl::string clientData(allocator);
    DataUtil::generateData(&clientData, SIZE);

    bsl::string serverData(allocator);
    serverData.resize(SIZE);

    bslmt::ThreadGroup threadGroup(allocator);
    threadGroup.addThread(
        bdlf::BindUtil::bind(&AsyncUtil::sendString, client, &clientData));
    threadGroup.addThread(
        bdlf::BindUtil::bind(&AsyncUtil::receiveString, server, &serverData));

    threadGroup.joinAll();

    NTSCFG_TEST_EQ(serverData, clientData);

    ntsa::Endpoint clientSourceEndpoint;
    error = client->sourceEndpoint(&clientSourceEndpoint);
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    ntsa::Endpoint serverSourceEndpoint;
    error = server->sourceEndpoint(&serverSourceEndpoint);
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    {
        bsl::string clientSourceEndpointDescription;
        {
            bsl::stringstream ss;
            ss << clientSourceEndpoint;
            clientSourceEndpointDescription = ss.str();
        }

        bsl::string serverSourceEndpointDescription;
        {
            bsl::stringstream ss;
            ss << serverSourceEndpoint;
            serverSourceEndpointDescription = ss.str();
        }

        BSLS_LOG_DEBUG(
            "Test buffer I/O complete using stream socket pair %s / %s",
            clientSourceEndpointDescription.c_str(),
            serverSourceEndpointDescription.c_str());
    }
}

void StreamSocketTester::testVectorIO(
    const bsl::shared_ptr<ntsb::StreamSocket>& client,
    const bsl::shared_ptr<ntsb::StreamSocket>& server,
    bslma::Allocator*                          basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntsa::Error error;

    bdlbb::PooledBlobBufferFactory blobBufferFactory(4096, allocator);

    const bsl::size_t SIZE = 1024 * 1024 * 64;

    bdlbb::Blob clientData(&blobBufferFactory, allocator);
    DataUtil::generateData(&clientData, SIZE);

    bdlbb::Blob serverData(&blobBufferFactory, allocator);
    serverData.setLength(SIZE);
    serverData.setLength(0);
    NTSCFG_TEST_EQ(serverData.length(), 0);
    NTSCFG_TEST_EQ(serverData.totalSize(), SIZE);

    bslmt::ThreadGroup threadGroup(allocator);
    threadGroup.addThread(
        bdlf::BindUtil::bind(&AsyncUtil::sendBlob, client, &clientData));
    threadGroup.addThread(
        bdlf::BindUtil::bind(&AsyncUtil::receiveBlob, server, &serverData));

    threadGroup.joinAll();

    NTSCFG_TEST_EQ(bdlbb::BlobUtil::compare(serverData, clientData), 0);

    ntsa::Endpoint clientSourceEndpoint;
    error = client->sourceEndpoint(&clientSourceEndpoint);
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    ntsa::Endpoint serverSourceEndpoint;
    error = server->sourceEndpoint(&serverSourceEndpoint);
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    {
        bsl::string clientSourceEndpointDescription;
        {
            bsl::stringstream ss;
            ss << clientSourceEndpoint;
            clientSourceEndpointDescription = ss.str();
        }

        bsl::string serverSourceEndpointDescription;
        {
            bsl::stringstream ss;
            ss << serverSourceEndpoint;
            serverSourceEndpointDescription = ss.str();
        }

        BSLS_LOG_DEBUG(
            "Test vector I/O complete using stream socket pair %s / %s",
            clientSourceEndpointDescription.c_str(),
            serverSourceEndpointDescription.c_str());
    }
}

}  // close unnamed namespace

}  // close namespace test

NTSCFG_TEST_CASE(1)
{
    // Concern:
    // Plan:

    bsl::vector<ntsa::Transport::Value> socketTypes;

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_TCP_IPV4_STREAM))
    {
        socketTypes.push_back(ntsa::Transport::e_TCP_IPV4_STREAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_TCP_IPV6_STREAM))
    {
        socketTypes.push_back(ntsa::Transport::e_TCP_IPV6_STREAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(ntsa::Transport::e_LOCAL_STREAM))
    {
        socketTypes.push_back(ntsa::Transport::e_LOCAL_STREAM);
    }

    for (bsl::size_t i = 0; i < socketTypes.size(); ++i) {
        ntsa::Transport::Value transport = socketTypes[i];

        ntscfg::TestAllocator ta;
        {
            ntsa::Error error;

            bsl::shared_ptr<ntsb::StreamSocket> client;
            bsl::shared_ptr<ntsb::StreamSocket> server;

            error = ntsb::StreamSocket::pair(&client, &server, transport, &ta);
            NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

            test::StreamSocketTester::testBufferIO(client, server, &ta);

            error = client->shutdown(ntsa::ShutdownType::e_SEND);
            NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

            error = server->shutdown(ntsa::ShutdownType::e_SEND);
            NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

            error = client->close();
            NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

            error = server->close();
            NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        }
        NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
    }
}

NTSCFG_TEST_CASE(2)
{
    // Concern:
    // Plan:

    bsl::vector<ntsa::Transport::Value> socketTypes;

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_TCP_IPV4_STREAM))
    {
        socketTypes.push_back(ntsa::Transport::e_TCP_IPV4_STREAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_TCP_IPV6_STREAM))
    {
        socketTypes.push_back(ntsa::Transport::e_TCP_IPV6_STREAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(ntsa::Transport::e_LOCAL_STREAM))
    {
        socketTypes.push_back(ntsa::Transport::e_LOCAL_STREAM);
    }

    for (bsl::size_t i = 0; i < socketTypes.size(); ++i) {
        ntsa::Transport::Value transport = socketTypes[i];

        ntscfg::TestAllocator ta;
        {
            ntsa::Error error;

            bsl::shared_ptr<ntsb::StreamSocket> client;
            bsl::shared_ptr<ntsb::StreamSocket> server;

            error = ntsb::StreamSocket::pair(&client, &server, transport, &ta);
            NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

            test::StreamSocketTester::testVectorIO(client, server, &ta);

            error = client->shutdown(ntsa::ShutdownType::e_SEND);
            NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

            error = server->shutdown(ntsa::ShutdownType::e_SEND);
            NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

            error = client->close();
            NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

            error = server->close();
            NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        }
        NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
    }
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
}
NTSCFG_TEST_DRIVER_END;
