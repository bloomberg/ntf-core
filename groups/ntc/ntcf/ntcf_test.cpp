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

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcf_test_cpp, "$Id$ $CSID$")

#include <ntcs_compat.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcf {

ntsa::Error Test::createStreamSocketPair(
    bsl::shared_ptr<ntci::Interface>*    interface,
    bsl::shared_ptr<ntci::StreamSocket>* clientStreamSocket,
    bsl::shared_ptr<ntci::StreamSocket>* serverStreamSocket,
    ntsa::Transport::Value               transport,
    bslma::Allocator*                    basicAllocator)
{
    ntca::InterfaceConfig defaultInterfaceConfig;
    defaultInterfaceConfig.setThreadName("test");
    defaultInterfaceConfig.setMinThreads(1);
    defaultInterfaceConfig.setMaxThreads(1);

    ntca::StreamSocketOptions defaultStreamSocketOptions;

    return Test::createStreamSocketPair(interface,
                                        clientStreamSocket,
                                        serverStreamSocket,
                                        transport,
                                        defaultInterfaceConfig,
                                        defaultStreamSocketOptions,
                                        defaultStreamSocketOptions,
                                        basicAllocator);
}

ntsa::Error Test::createStreamSocketPair(
    bsl::shared_ptr<ntci::Interface>*    interface,
    bsl::shared_ptr<ntci::StreamSocket>* clientStreamSocket,
    bsl::shared_ptr<ntci::StreamSocket>* serverStreamSocket,
    ntsa::Transport::Value               transport,
    const ntca::InterfaceConfig&         interfaceConfig,
    const ntca::StreamSocketOptions&     clientStreamSocketOptions,
    const ntca::StreamSocketOptions&     serverStreamSocketOptions,
    bslma::Allocator*                    basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    if (transport == ntsa::Transport::e_UNDEFINED) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntca::ListenerSocketOptions listenerSocketOptions;
    ntcs::Compat::convert(&listenerSocketOptions, serverStreamSocketOptions);

    listenerSocketOptions.setTransport(transport);
    listenerSocketOptions.setBacklog(1);
    listenerSocketOptions.setReuseAddress(true);

    if (transport == ntsa::Transport::e_TCP_IPV4_STREAM) {
        listenerSocketOptions.setSourceEndpoint(ntsa::Endpoint(
            ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)));
    }
    else if (transport == ntsa::Transport::e_TCP_IPV6_STREAM) {
        listenerSocketOptions.setSourceEndpoint(ntsa::Endpoint(
            ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), 0)));
    }
    else if (transport == ntsa::Transport::e_LOCAL_STREAM) {
        ntsa::LocalName localName = ntsa::LocalName::generateUnique();
        listenerSocketOptions.setSourceEndpoint(ntsa::Endpoint(localName));
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *interface = ntcf::System::createInterface(interfaceConfig, allocator);

    ntci::InterfaceStopGuard interfaceGuard(*interface);

    error = (*interface)->start();
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        (*interface)->createListenerSocket(listenerSocketOptions, allocator);

    ntci::ListenerSocketCloseGuard listenerGuard(listenerSocket);

    error = listenerSocket->open();
    if (error) {
        return error;
    }

    error = listenerSocket->listen();
    if (error) {
        return error;
    }

    error = listenerSocket->relaxFlowControl(ntca::FlowControlType::e_RECEIVE);
    if (error) {
        return error;
    }

    *clientStreamSocket =
        (*interface)->createStreamSocket(clientStreamSocketOptions, allocator);

    ntci::StreamSocketCloseGuard clientStreamSocketGuard(*clientStreamSocket);

    ntci::ConnectFuture connectFuture;
    error = (*clientStreamSocket)
                ->connect(listenerSocket->sourceEndpoint(),
                          ntca::ConnectOptions(),
                          connectFuture);
    if (error) {
        return error;
    }

    ntci::ConnectResult connectResult;
    error = connectFuture.wait(&connectResult);
    if (error) {
        return error;
    }

    if (connectResult.event().type() == ntca::ConnectEventType::e_ERROR) {
        return connectResult.event().context().error();
    }
    else if (connectResult.event().type() !=
             ntca::ConnectEventType::e_COMPLETE)
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntci::AcceptFuture acceptFuture;
    error = listenerSocket->accept(ntca::AcceptOptions(), acceptFuture);
    if (error) {
        return error;
    }

    ntci::AcceptResult acceptResult;
    error = acceptFuture.wait(&acceptResult);
    if (error) {
        return error;
    }

    if (acceptResult.event().type() == ntca::AcceptEventType::e_ERROR) {
        return acceptResult.event().context().error();
    }
    else if (acceptResult.event().type() != ntca::AcceptEventType::e_COMPLETE)
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *serverStreamSocket = acceptResult.streamSocket();

    clientStreamSocketGuard.release();
    interfaceGuard.release();

    return ntsa::Error();
}

void Test::closeAll(
    const bsl::shared_ptr<ntci::Interface>&    interface,
    const bsl::shared_ptr<ntci::StreamSocket>& clientStreamSocket,
    const bsl::shared_ptr<ntci::StreamSocket>& serverStreamSocket)
{
    {
        ntci::StreamSocketCloseGuard guardClientStreamSocket(
            clientStreamSocket);
    }

    {
        ntci::StreamSocketCloseGuard guardServerStreamSocket(
            serverStreamSocket);
    }

    {
        ntci::InterfaceStopGuard guardInterface(interface);
    }
}

TestGuard::TestGuard(
    const bsl::shared_ptr<ntci::Interface>&    interface,
    const bsl::shared_ptr<ntci::StreamSocket>& clientStreamSocket,
    const bsl::shared_ptr<ntci::StreamSocket>& serverStreamSocket)
: d_interface_sp(interface)
, d_clientStreamSocket_sp(clientStreamSocket)
, d_serverStreamSocket_sp(serverStreamSocket)
{
}

TestGuard::~TestGuard()
{
    ntcf::Test::closeAll(d_interface_sp,
                         d_clientStreamSocket_sp,
                         d_serverStreamSocket_sp);
}

}  // close package namespace
}  // close enterprise namespace
