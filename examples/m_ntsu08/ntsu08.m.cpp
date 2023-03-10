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

#include <ntsf_system.h>
#include <ntsi_datagramsocket.h>
#include <bsls_assert.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;

int main()
{
    // TODO: Fix multicasting from various networking interfaces and
    // configurations. Currently, some 'send' operations fail with ENETUNREACH.
#if 0
    // Initialize the library.

    ntsf::System::initialize();
    ntsf::System::ignore(ntscfg::Signal::e_PIPE);

    ntsa::Error error;

    // Find the first network interface that supports IPv6 multicasting, but
    // only run this example on machines that support IPv6 multicasting.

    ntsa::Adapter adapter;
    if (!ntsf::System::discoverAdapter(
            &adapter, ntsa::IpAddressType::e_V6, true)) {
        return 0;
    }

    // Choose a multicast group and UDP port number used in this example.

    const ntsa::IpAddress MULTICAST_GROUP =
        ntsa::IpAddress("ff02:0:0:0:0:1:ff00:0000");
    const unsigned short MULTICAST_PORT = 42800;

    //
    // PART 1: BIND
    //

    // Create a blocking socket for the client and bind it to any port on the
    // multicasting network interface.

    bsl::shared_ptr<ntsi::DatagramSocket> client =
        ntsf::System::createDatagramSocket();

    error = client->open(ntsa::Transport::e_UDP_IPV6_DATAGRAM);
    BSLS_ASSERT_OPT(!error);

    error =
        client->bind(ntsa::Endpoint(adapter.ipv6Address().value(), 0), false);
    BSLS_ASSERT_OPT(!error);

    ntsa::Endpoint clientEndpoint;
    error = client->sourceEndpoint(&clientEndpoint);
    BSLS_ASSERT_OPT(!error);

    // Create a blocking socket for the first server and bind it to the chosen
    // port on the multicasting network interface.

    bsl::shared_ptr<ntsi::DatagramSocket> serverOne =
        ntsf::System::createDatagramSocket();

    error = serverOne->open(ntsa::Transport::e_UDP_IPV6_DATAGRAM);
    BSLS_ASSERT_OPT(!error);

    error = serverOne->bind(
        ntsa::Endpoint(ntsa::Ipv6Address::any(), MULTICAST_PORT), true);
    BSLS_ASSERT_OPT(!error);

    ntsa::Endpoint serverOneEndpoint;
    error = serverOne->sourceEndpoint(&serverOneEndpoint);
    BSLS_ASSERT_OPT(!error);

    // Create a blocking socket for the second server and bind it to the chosen
    // port on the multicasting network interface.

    bsl::shared_ptr<ntsi::DatagramSocket> serverTwo =
        ntsf::System::createDatagramSocket();

    error = serverTwo->open(ntsa::Transport::e_UDP_IPV6_DATAGRAM);
    BSLS_ASSERT_OPT(!error);

    error = serverTwo->bind(
        ntsa::Endpoint(ntsa::Ipv6Address::any(), MULTICAST_PORT), true);
    BSLS_ASSERT_OPT(!error);

    ntsa::Endpoint serverTwoEndpoint;
    error = serverTwo->sourceEndpoint(&serverTwoEndpoint);
    BSLS_ASSERT_OPT(!error);

    //
    // PART 2: CONFIGURE MULTICAST PUBLICATION
    //

    // Configure the client socket to send multicast data on specific
    // multicasting network interface limited to the host machine.

    error = client->setMulticastInterface(
        ntsa::IpAddress(adapter.ipv6Address().value()));
    BSLS_ASSERT_OPT(!error);

    error = client->setMulticastTimeToLive(0);
    BSLS_ASSERT_OPT(!error);

    //
    // PART 3: CONFIGURE MULTICAST SUBSCRIPTION
    //

    // Join the first server socket to the multicast group.

    error = serverOne->joinMulticastGroup(
        ntsa::IpAddress(adapter.ipv6Address().value()), MULTICAST_GROUP);
    BSLS_ASSERT_OPT(!error);

    // Join the second server socket to the multicast group.

    error = serverTwo->joinMulticastGroup(
        ntsa::IpAddress(adapter.ipv6Address().value()), MULTICAST_GROUP);
    BSLS_ASSERT_OPT(!error);

    //
    // PART 4: SEND MULTICAST DATA FROM THE CLIENT TO THE SERVERS
    //

    // Enqueue outgoing data to transmit by the client socket.

    {
        ntsa::Endpoint multicastEndpoint(MULTICAST_GROUP, MULTICAST_PORT);

        char storage = 'C';

        ntsa::Data data(ntsa::ConstBuffer(&storage, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;
        options.setEndpoint(multicastEndpoint);

        error = client->send(&context, data, options);
        BSLS_ASSERT_OPT(!error);

        BSLS_ASSERT_OPT(context.bytesSent() == 1);
    }

    // Dequeue incoming data received by the first server socket.

    {
        char storage;

        ntsa::Data data(ntsa::MutableBuffer(&storage, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;
        options.showEndpoint();

        error =
            serverOne->receive(&context, &data, options);
        BSLS_ASSERT_OPT(!error);

        BSLS_ASSERT_OPT(context.endpoint().value() == clientEndpoint);
        BSLS_ASSERT_OPT(context.bytesReceived() == 1);
        BSLS_ASSERT_OPT(storage == 'C');
    }

    // Dequeue incoming data received by the second server socket.

    {
        char storage;

        ntsa::Data data(ntsa::MutableBuffer(&storage, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;
        options.showEndpoint();

        error =
            serverTwo->receive(&context, &data, options);
        BSLS_ASSERT_OPT(!error);

        BSLS_ASSERT_OPT(context.endpoint().value() == clientEndpoint);
        BSLS_ASSERT_OPT(context.bytesReceived() == 1);
        BSLS_ASSERT_OPT(storage == 'C');
    }

    //
    // PART 5: SEND UNICAST DATA FROM THE SERVERS TO THE CLIENT
    //

    // Enqueue outgoing data to transmit by the first server socket.

    {
        char storage = '1';

        ntsa::Data data(ntsa::ConstBuffer(&storage, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;
        options.setEndpoint(clientEndpoint);

        error = serverOne->send(&context, data, options);
        BSLS_ASSERT_OPT(!error);

        BSLS_ASSERT_OPT(context.bytesSent() == 1);
    }

    // Dequeue incoming data received by the client socket.

    {
        char storage;

        ntsa::Data data(ntsa::MutableBuffer(&storage, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;
        options.showEndpoint();

        error = client->receive(&context, &data, options);
        BSLS_ASSERT_OPT(!error);

        BSLS_ASSERT_OPT(
            context.endpoint().value() ==
            ntsa::Endpoint(adapter.ipv6Address().value(), MULTICAST_PORT));
        BSLS_ASSERT_OPT(context.bytesReceived() == 1);
        BSLS_ASSERT_OPT(storage == '1');
    }

    // Enqueue outgoing data to transmit by the second server socket.

    {
        char storage = '2';

        ntsa::Data data(ntsa::ConstBuffer(&storage, 1));

        ntsa::SendContext context;
        ntsa::SendOptions options;
        options.setEndpoint(clientEndpoint);

        error = serverTwo->send(&context, data, options);
        BSLS_ASSERT_OPT(!error);

        BSLS_ASSERT_OPT(context.bytesSent() == 1);
    }

    // Dequeue incoming data received by the client socket.

    {
        char storage;

        ntsa::Data data(ntsa::MutableBuffer(&storage, 1));

        ntsa::ReceiveContext context;
        ntsa::ReceiveOptions options;
        options.showEndpoint();

        error = client->receive(&context, &data, options);
        BSLS_ASSERT_OPT(!error);

        BSLS_ASSERT_OPT(
            context.endpoint().value() ==
            ntsa::Endpoint(adapter.ipv6Address().value(), MULTICAST_PORT));
        BSLS_ASSERT_OPT(context.bytesReceived() == 1);
        BSLS_ASSERT_OPT(storage == '2');
    }
#endif
    return 0;
}
