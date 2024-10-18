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
BSLS_IDENT_RCSID(ntcf_test_t_cpp, "$Id$ $CSID$")

#include <ntcf_test.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcf {

// Provide tests for 'ntcf::Test'.
class TestTest
{
  public:
    // TODO
    static void verify();
};

NTSCFG_TEST_FUNCTION(ntcf::TestTest::verify)
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
        NTSCFG_TEST_ALLOCATOR);
    NTSCFG_TEST_OK(error);

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
        NTSCFG_TEST_OK(error);

        ntci::SendResult sendResult;
        error = sendFuture.wait(&sendResult);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(sendResult.event().type(),
                        ntca::SendEventType::e_COMPLETE);
    }

    // Receive data at the server from the client.

    {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setSize(sizeof CLIENT_DATA - 1);

        ntci::ReceiveFuture receiveFuture;
        error = serverStreamSocket->receive(receiveOptions, receiveFuture);
        NTSCFG_TEST_OK(error);

        ntci::ReceiveResult receiveResult;
        error = receiveFuture.wait(&receiveResult);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(receiveResult.event().type(),
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
        NTSCFG_TEST_OK(error);

        ntci::SendResult sendResult;
        error = sendFuture.wait(&sendResult);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(sendResult.event().type(),
                        ntca::SendEventType::e_COMPLETE);
    }

    // Receive data at the client from the server.

    {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setSize(sizeof SERVER_DATA - 1);

        ntci::ReceiveFuture receiveFuture;
        error = clientStreamSocket->receive(receiveOptions, receiveFuture);
        NTSCFG_TEST_OK(error);

        ntci::ReceiveResult receiveResult;
        error = receiveFuture.wait(&receiveResult);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(receiveResult.event().type(),
                        ntca::ReceiveEventType::e_COMPLETE);
    }
}

}  // close namespace ntcf
}  // close namespace BloombergLP
