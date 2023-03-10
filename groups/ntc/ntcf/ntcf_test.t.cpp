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

#include <ntcf_test.h>

#include <ntccfg_test.h>

#include <bdlbb_blob.h>
#include <bdlbb_blobutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

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

NTCCFG_TEST_CASE(1)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        // Create a stream socket pair driven asynchronously by an interface.

        ntsa::Error error;

        bsl::shared_ptr<ntci::Interface>    interface;
        bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket;
        bsl::shared_ptr<ntci::StreamSocket> serverStreamSocket;

        error = ntcf::Test::createStreamSocketPair(
            &interface,
            &clientStreamSocket,
            &serverStreamSocket,
            ntsa::Transport::e_TCP_IPV4_STREAM,
            &ta);
        NTCCFG_TEST_OK(error);

        ntcf::TestGuard guard(interface,
                              clientStreamSocket,
                              serverStreamSocket);

        BSLS_LOG_DEBUG("Client stream socket at %s to %s",
                       clientStreamSocket->sourceEndpoint().text().c_str(),
                       clientStreamSocket->remoteEndpoint().text().c_str());

        BSLS_LOG_DEBUG("Server stream socket at %s to %s",
                       serverStreamSocket->sourceEndpoint().text().c_str(),
                       serverStreamSocket->remoteEndpoint().text().c_str());

        // Send data from the client to the server.

        const char CLIENT_DATA[] = "Hello, server!";

        {
            ntci::SendFuture sendFuture;
            error = clientStreamSocket->send(
                ntsa::Data(
                    ntsa::ConstBuffer(CLIENT_DATA, sizeof CLIENT_DATA - 1)),
                ntca::SendOptions(),
                sendFuture);
            NTCCFG_TEST_OK(error);

            ntci::SendResult sendResult;
            error = sendFuture.wait(&sendResult);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(sendResult.event().type(),
                           ntca::SendEventType::e_COMPLETE);
        }

        // Receive data at the server from the client.

        {
            ntca::ReceiveOptions receiveOptions;
            receiveOptions.setSize(sizeof CLIENT_DATA - 1);

            ntci::ReceiveFuture receiveFuture;
            error = serverStreamSocket->receive(receiveOptions, receiveFuture);
            NTCCFG_TEST_OK(error);

            ntci::ReceiveResult receiveResult;
            error = receiveFuture.wait(&receiveResult);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(receiveResult.event().type(),
                           ntca::ReceiveEventType::e_COMPLETE);
        }

        // Send data from the server to the client.

        const char SERVER_DATA[] = "Hello, client!";

        {
            ntci::SendFuture sendFuture;
            error = serverStreamSocket->send(
                ntsa::Data(
                    ntsa::ConstBuffer(SERVER_DATA, sizeof SERVER_DATA - 1)),
                ntca::SendOptions(),
                sendFuture);
            NTCCFG_TEST_OK(error);

            ntci::SendResult sendResult;
            error = sendFuture.wait(&sendResult);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(sendResult.event().type(),
                           ntca::SendEventType::e_COMPLETE);
        }

        // Receive data at the client from the server.

        {
            ntca::ReceiveOptions receiveOptions;
            receiveOptions.setSize(sizeof SERVER_DATA - 1);

            ntci::ReceiveFuture receiveFuture;
            error = clientStreamSocket->receive(receiveOptions, receiveFuture);
            NTCCFG_TEST_OK(error);

            ntci::ReceiveResult receiveResult;
            error = receiveFuture.wait(&receiveResult);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(receiveResult.event().type(),
                           ntca::ReceiveEventType::e_COMPLETE);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
}
NTCCFG_TEST_DRIVER_END;
