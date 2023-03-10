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

#include <ntsb_datagramsocket.h>

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
#include <bsls_atomic.h>
#include <bsls_log.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;

// On Linux, at least, sendto on a local datagram socket created and connected
// by ::socketpair returns EINVAL even when the peer address matches the source
// address of the peer.
#define NTSB_DATAGRAMSOCKETTESTER_SENDTO_RECEIVEFROM 0

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

/// Provide utilities for sending and receiving data.
struct AsyncUtil {
    /// Send from the specified 'client' all the specified 'clientData'
    /// to the peer of the 'client'.
    static void sendString(const bsl::shared_ptr<ntsb::DatagramSocket>& client,
                           const ntsa::Endpoint& serverEndpoint,
                           const bsl::string*    clientData,
                           bsl::size_t           numMessages,
                           bslma::Allocator*     allocator);

    /// Receive from the specified 'server' into the pre-ressized
    /// 'serverData' all the data sent by the peer of the 'server'.
    static void receiveString(
        const bsl::shared_ptr<ntsb::DatagramSocket>& server,
        const ntsa::Endpoint&                        clientEndpoint,
        const bsl::string*                           clientData,
        bsl::size_t                                  numMessagesExpected,
        bsls::AtomicInt*                             numMessagesReceived,
        bslma::Allocator*                            allocator);

    /// Send from the specified 'client' all the specified 'clientData'
    /// to the peer of the 'client'.
    static void sendBlob(const bsl::shared_ptr<ntsb::DatagramSocket>& client,
                         const ntsa::Endpoint& serverEndpoint,
                         const bdlbb::Blob*    clientData,
                         bsl::size_t           numMessages,
                         bslma::Allocator*     allocator);

    /// Receive from the specified 'server' into the pre-ressized
    /// 'serverData' all the data sent by the peer of the 'server'.
    static void receiveBlob(
        const bsl::shared_ptr<ntsb::DatagramSocket>& server,
        const ntsa::Endpoint&                        clientEndpoint,
        const bdlbb::Blob*                           clientData,
        bsl::size_t                                  numMessagesExpected,
        bsls::AtomicInt*                             numMessagesReceived,
        bdlbb::BlobBufferFactory*                    blobBufferFactory,
        bslma::Allocator*                            allocator);
};

void AsyncUtil::sendString(const bsl::shared_ptr<ntsb::DatagramSocket>& client,
                           const ntsa::Endpoint& serverEndpoint,
                           const bsl::string*    clientData,
                           bsl::size_t           numMessages,
                           bslma::Allocator*     allocator)
{
    NTSCFG_WARNING_UNUSED(allocator);

    for (bsl::size_t i = 0; i < numMessages; ++i) {
#if NTSB_DATAGRAMSOCKETTESTER_SENDTO_RECEIVEFROM
        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setEndpoint(serverEndpoint);

        ntsa::Data data(
            ntsa::ConstBuffer(clientData->c_str(), clientData->size()));

        ntsa::Error error = client->send(&context, data, options);

        if (error) {
            BSLS_LOG_INFO("Sender error %s", error.text().c_str());
            if (error == ntsa::Error::e_LIMIT) {
                continue;
            }
        }

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(context.bytesSent(), clientData->size());

        BSLS_LOG_DEBUG("[+]"
                       "\nnumBytesSendable:   %d"
                       "\nnumBytesSent:       %d",
                       context.bytesSendable(),
                       context.bytesSent());
#else
        NTSCFG_WARNING_UNUSED(serverEndpoint);

        ntsa::SendContext context;
        ntsa::SendOptions options;

        ntsa::Data data(
            ntsa::ConstBuffer(clientData->c_str(), clientData->size()));

        ntsa::Error error = client->send(&context, data, options);

        if (error) {
            BSLS_LOG_INFO("Sender error %s", error.text().c_str());
            if (error == ntsa::Error::e_LIMIT) {
                continue;
            }
        }

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(context.bytesSent(), clientData->size());

        BSLS_LOG_DEBUG("[+]"
                       "\nnumBytesSendable:   %d"
                       "\nnumBytesSent:       %d",
                       context.bytesSendable(),
                       context.bytesSent());
#endif
    }
}

void AsyncUtil::receiveString(
    const bsl::shared_ptr<ntsb::DatagramSocket>& server,
    const ntsa::Endpoint&                        clientEndpoint,
    const bsl::string*                           clientData,
    bsl::size_t                                  numMessagesExpected,
    bsls::AtomicInt*                             numMessagesReceived,
    bslma::Allocator*                            allocator)
{
    for (bsl::size_t i = 0; i < numMessagesExpected; ++i) {
        ntsa::Error error = ntsu::SocketUtil::waitUntilReadable(
            server->handle(),
            bdlt::CurrentTime::now() + bsls::TimeInterval(3.0));

        if (error) {
            return;
        }

        bsl::string serverData(allocator);
        serverData.resize(clientData->size());

#if NTSB_DATAGRAMSOCKETTESTER_SENDTO_RECEIVEFROM
        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        ntsa::Data data(
            ntsa::MutableBuffer(&serverData[0], serverData.size()));

        error = server->receive(&context, &data, options);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(context.bytesReceived(), serverData.size());
        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), clientEndpoint);
        NTSCFG_TEST_EQ(serverData, *clientData);
#else
        NTSCFG_WARNING_UNUSED(clientEndpoint);

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        ntsa::Data data(
            ntsa::MutableBuffer(&serverData[0], serverData.size()));

        error = server->receive(&context, &data, options);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(context.bytesReceived(), serverData.size());
        NTSCFG_TEST_EQ(serverData, *clientData);
#endif

        ++(*numMessagesReceived);

        BSLS_LOG_DEBUG("[-]"
                       "\nnumBytesReceivable: %d"
                       "\nnumBytesReceived:   %d",
                       context.bytesReceivable(),
                       context.bytesReceived());
    }
}

void AsyncUtil::sendBlob(const bsl::shared_ptr<ntsb::DatagramSocket>& client,
                         const ntsa::Endpoint& serverEndpoint,
                         const bdlbb::Blob*    clientData,
                         bsl::size_t           numMessages,
                         bslma::Allocator*     allocator)
{
    NTSCFG_WARNING_UNUSED(allocator);

    for (bsl::size_t i = 0; i < numMessages; ++i) {
#if NTSB_DATAGRAMSOCKETTESTER_SENDTO_RECEIVEFROM
        ntsa::SendContext context;
        ntsa::SendOptions options;

        options.setEndpoint(serverEndpoint);

        ntsa::Error error = client->send(&context, *clientData, options);

        if (error) {
            BSLS_LOG_INFO("Sender error %s", error.text().c_str());
            if (error == ntsa::Error::e_LIMIT) {
                continue;
            }
        }

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(context.bytesSent(),
                       static_cast<bsl::size_t>(clientData->length()));

        BSLS_LOG_DEBUG("[+]"
                       "\nnumBytesSendable:   %d"
                       "\nnumBytesSent:       %d",
                       context.bytesSendable(),
                       context.bytesSent());
#else
        NTSCFG_WARNING_UNUSED(serverEndpoint);

        ntsa::SendContext context;
        ntsa::SendOptions options;

        ntsa::Error error = client->send(&context, *clientData, options);

        if (error) {
            BSLS_LOG_INFO("Sender error %s", error.text().c_str());
            if (error == ntsa::Error::e_LIMIT) {
                continue;
            }
        }

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(context.bytesSent(),
                       static_cast<bsl::size_t>(clientData->length()));

        BSLS_LOG_DEBUG("[+]"
                       "\nnumBytesSendable:   %d"
                       "\nnumBytesSent:       %d",
                       context.bytesSendable(),
                       context.bytesSent());
#endif
    }
}

void AsyncUtil::receiveBlob(
    const bsl::shared_ptr<ntsb::DatagramSocket>& server,
    const ntsa::Endpoint&                        clientEndpoint,
    const bdlbb::Blob*                           clientData,
    bsl::size_t                                  numMessagesExpected,
    bsls::AtomicInt*                             numMessagesReceived,
    bdlbb::BlobBufferFactory*                    blobBufferFactory,
    bslma::Allocator*                            allocator)
{
    for (bsl::size_t i = 0; i < numMessagesExpected; ++i) {
        ntsa::Error error = ntsu::SocketUtil::waitUntilReadable(
            server->handle(),
            bdlt::CurrentTime::now() + bsls::TimeInterval(3.0));

        if (error) {
            return;
        }

        bdlbb::Blob serverData(blobBufferFactory, allocator);
        serverData.setLength(clientData->length());
        serverData.setLength(0);
        NTSCFG_TEST_EQ(serverData.length(), 0);
        NTSCFG_TEST_EQ(serverData.totalSize(), clientData->length());

#if NTSB_DATAGRAMSOCKETTESTER_SENDTO_RECEIVEFROM
        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &serverData, options);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(context.bytesReceived(), serverData.length());
        NTSCFG_TEST_FALSE(context.endpoint().isNull());
        NTSCFG_TEST_EQ(context.endpoint().value(), clientEndpoint);
        NTSCFG_TEST_EQ(bdlbb::BlobUtil::compare(serverData, *clientData), 0);
#else
        NTSCFG_WARNING_UNUSED(clientEndpoint);

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;

        error = server->receive(&context, &serverData, options);

        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
        NTSCFG_TEST_EQ(context.bytesReceived(),
                       static_cast<bsl::size_t>(serverData.length()));
        NTSCFG_TEST_EQ(bdlbb::BlobUtil::compare(serverData, *clientData), 0);
#endif

        ++(*numMessagesReceived);

        BSLS_LOG_DEBUG("[-]"
                       "\nnumBytesReceivable: %d"
                       "\nnumBytesReceived:   %d",
                       context.bytesReceivable(),
                       context.bytesReceived());
    }
}

}  // close unnamed namespace

/// Provide facilities to test different datagram sockets
/// conveniently in one place.
struct DatagramSocketTester {
    /// Test the implementations of the specified 'client' and 'server'
    /// send and receive data correctly using basic, contiguous buffers.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    static void testBufferIO(
        const bsl::shared_ptr<ntsb::DatagramSocket>& client,
        const bsl::shared_ptr<ntsb::DatagramSocket>& server,
        bslma::Allocator*                            basicAllocator = 0);

    /// Test the implementations of the specified 'client' and 'server'
    /// send and receive data correctly using vectored I/O and the
    /// scatter/gather paradigm. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static void testVectorIO(
        const bsl::shared_ptr<ntsb::DatagramSocket>& client,
        const bsl::shared_ptr<ntsb::DatagramSocket>& server,
        bslma::Allocator*                            basicAllocator = 0);
};

void DatagramSocketTester::testBufferIO(
    const bsl::shared_ptr<ntsb::DatagramSocket>& client,
    const bsl::shared_ptr<ntsb::DatagramSocket>& server,
    bslma::Allocator*                            basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntsa::Error error;

    const bsl::size_t NUM_MESSAGES = 100000;     // was: 100
    const bsl::size_t MESSAGE_SIZE = 1024 * 32;  // was: 256

    ntsa::Endpoint clientSourceEndpoint;
    error = client->sourceEndpoint(&clientSourceEndpoint);
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    ntsa::Endpoint serverSourceEndpoint;
    error = server->sourceEndpoint(&serverSourceEndpoint);
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    bsl::string clientData(allocator);
    DataUtil::generateData(&clientData, MESSAGE_SIZE);

    bsls::AtomicInt numMessagesReceived;

    bslmt::ThreadGroup threadGroup(allocator);
    threadGroup.addThread(bdlf::BindUtil::bind(&AsyncUtil::sendString,
                                               client,
                                               serverSourceEndpoint,
                                               &clientData,
                                               NUM_MESSAGES,
                                               allocator));

    threadGroup.addThread(bdlf::BindUtil::bind(&AsyncUtil::receiveString,
                                               server,
                                               clientSourceEndpoint,
                                               &clientData,
                                               NUM_MESSAGES,
                                               &numMessagesReceived,
                                               allocator));

    threadGroup.joinAll();

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

        BSLS_LOG_INFO(
            "Test buffer I/O %d/%d (%d%%) complete "
            "using datagram socket pair %s / %s",
            numMessagesReceived.load(),
            NUM_MESSAGES,
            int(((double(numMessagesReceived.load()) / NUM_MESSAGES) * 100)),
            clientSourceEndpointDescription.c_str(),
            serverSourceEndpointDescription.c_str());
    }
}

void DatagramSocketTester::testVectorIO(
    const bsl::shared_ptr<ntsb::DatagramSocket>& client,
    const bsl::shared_ptr<ntsb::DatagramSocket>& server,
    bslma::Allocator*                            basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntsa::Error error;

    const bsl::size_t NUM_MESSAGES = 100000;     // was: 100
    const bsl::size_t MESSAGE_SIZE = 1024 * 32;  // was: 256

    ntsa::Endpoint clientSourceEndpoint;
    error = client->sourceEndpoint(&clientSourceEndpoint);
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    ntsa::Endpoint serverSourceEndpoint;
    error = server->sourceEndpoint(&serverSourceEndpoint);
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    bdlbb::PooledBlobBufferFactory blobBufferFactory(MESSAGE_SIZE / 8,
                                                     allocator);

    bdlbb::Blob clientData(&blobBufferFactory, allocator);
    DataUtil::generateData(&clientData, MESSAGE_SIZE);

    bsls::AtomicInt numMessagesReceived;

    bslmt::ThreadGroup threadGroup(allocator);
    threadGroup.addThread(bdlf::BindUtil::bind(&AsyncUtil::sendBlob,
                                               client,
                                               serverSourceEndpoint,
                                               &clientData,
                                               NUM_MESSAGES,
                                               allocator));

    threadGroup.addThread(bdlf::BindUtil::bind(&AsyncUtil::receiveBlob,
                                               server,
                                               clientSourceEndpoint,
                                               &clientData,
                                               NUM_MESSAGES,
                                               &numMessagesReceived,
                                               &blobBufferFactory,
                                               allocator));

    threadGroup.joinAll();

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

        BSLS_LOG_INFO(
            "Test buffer I/O %d/%d (%d%%) complete "
            "using datagram socket pair %s / %s",
            numMessagesReceived.load(),
            NUM_MESSAGES,
            int(((double(numMessagesReceived.load()) / NUM_MESSAGES) * 100)),
            clientSourceEndpointDescription.c_str(),
            serverSourceEndpointDescription.c_str());
    }
}

}  // close namespace test

NTSCFG_TEST_CASE(1)
{
    // Concern: Datagram socket buffer I/O
    // Plan:

    bsl::vector<ntsa::Transport::Value> socketTypes;

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_UDP_IPV4_DATAGRAM))
    {
        socketTypes.push_back(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_UDP_IPV6_DATAGRAM))
    {
        socketTypes.push_back(ntsa::Transport::e_UDP_IPV6_DATAGRAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_LOCAL_DATAGRAM))
    {
        socketTypes.push_back(ntsa::Transport::e_LOCAL_DATAGRAM);
    }

    for (bsl::size_t i = 0; i < socketTypes.size(); ++i) {
        ntsa::Transport::Value transport = socketTypes[i];

        ntscfg::TestAllocator ta;
        {
            ntsa::Error error;

            bsl::shared_ptr<ntsb::DatagramSocket> client;
            bsl::shared_ptr<ntsb::DatagramSocket> server;

            error =
                ntsb::DatagramSocket::pair(&client, &server, transport, &ta);
            NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

            test::DatagramSocketTester::testBufferIO(client, server, &ta);

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
    // Concern: Datagram socket vector I/O
    // Plan:

    bsl::vector<ntsa::Transport::Value> socketTypes;

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_UDP_IPV4_DATAGRAM))
    {
        socketTypes.push_back(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_UDP_IPV6_DATAGRAM))
    {
        socketTypes.push_back(ntsa::Transport::e_UDP_IPV6_DATAGRAM);
    }

    if (ntsu::AdapterUtil::supportsTransport(
            ntsa::Transport::e_LOCAL_DATAGRAM))
    {
        socketTypes.push_back(ntsa::Transport::e_LOCAL_DATAGRAM);
    }

    for (bsl::size_t i = 0; i < socketTypes.size(); ++i) {
        ntsa::Transport::Value transport = socketTypes[i];

        ntscfg::TestAllocator ta;
        {
            ntsa::Error error;

            bsl::shared_ptr<ntsb::DatagramSocket> client;
            bsl::shared_ptr<ntsb::DatagramSocket> server;

            error =
                ntsb::DatagramSocket::pair(&client, &server, transport, &ta);
            NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

            test::DatagramSocketTester::testVectorIO(client, server, &ta);

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
