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

#include <ntcd_simulation.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcd_simulation_cpp, "$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcd {

Simulation::Simulation(bslma::Allocator* basicAllocator)
: d_machine_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_machine_sp.createInplace(d_allocator_p, d_allocator_p);
    ntcd::Machine::setDefault(d_machine_sp);
}

Simulation::~Simulation()
{
    ntcd::Machine::setDefault(bsl::shared_ptr<ntcd::Machine>());
}

bsl::shared_ptr<ntcd::Session> Simulation::createSession(
    bslma::Allocator* basicAllocator)
{
    return d_machine_sp->createSession(basicAllocator);
}

bsl::shared_ptr<ntcd::DatagramSocket> Simulation::createDatagramSocket(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::DatagramSocket> datagramSocket;
    datagramSocket.createInplace(allocator, allocator);

    return datagramSocket;
}

bsl::shared_ptr<ntcd::ListenerSocket> Simulation::createListenerSocket(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::ListenerSocket> listenerSocket;
    listenerSocket.createInplace(allocator, allocator);

    return listenerSocket;
}

bsl::shared_ptr<ntcd::StreamSocket> Simulation::createStreamSocket(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::StreamSocket> streamSocket;
    streamSocket.createInplace(allocator, allocator);

    return streamSocket;
}

bsl::shared_ptr<ntcd::Reactor> Simulation::createReactor(
    const ntca::ReactorConfig&         configuration,
    const bsl::shared_ptr<ntci::User>& user,
    bslma::Allocator*                  basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::Reactor> reactor;
    reactor.createInplace(allocator, configuration, user, allocator);

    return reactor;
}

bsl::shared_ptr<ntcd::Proactor> Simulation::createProactor(
    const ntca::ProactorConfig&        configuration,
    const bsl::shared_ptr<ntci::User>& user,
    bslma::Allocator*                  basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::Proactor> proactor;
    proactor.createInplace(allocator, configuration, user, allocator);

    return proactor;
}

ntsa::Error Simulation::run()
{
    return d_machine_sp->run();
}

ntsa::Error Simulation::step(bool block)
{
    return d_machine_sp->step(block);
}

void Simulation::stop()
{
    return d_machine_sp->stop();
}

ntsa::Error Simulation::lookupSession(bsl::weak_ptr<ntcd::Session>* result,
                                      ntsa::Handle handle) const
{
    return d_machine_sp->lookupSession(result, handle);
}

ntsa::Error Simulation::createStreamSocketPair(
    bsl::shared_ptr<ntcd::StreamSocket>* client,
    bsl::shared_ptr<ntcd::StreamSocket>* server,
    ntsa::Transport::Value               type,
    bslma::Allocator*                    basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::ListenerSocket> listenerSocket;
    listenerSocket.createInplace(allocator, allocator);

    error = listenerSocket->open(type);
    if (error) {
        return error;
    }

    if (type == ntsa::Transport::e_TCP_IPV4_STREAM) {
        error = listenerSocket->bind(
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
            false);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Endpoint listenerSourceEndpoint;
    error = listenerSocket->sourceEndpoint(&listenerSourceEndpoint);
    if (error) {
        return error;
    }

    error = listenerSocket->listen(1);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntcd::StreamSocket> clientStreamSocket;
    clientStreamSocket.createInplace(allocator, allocator);

    error = clientStreamSocket->open(type);
    if (error) {
        return error;
    }

    error = clientStreamSocket->connect(listenerSourceEndpoint);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntsi::StreamSocket> serverStreamSocket;
    error = listenerSocket->accept(&serverStreamSocket);
    if (error) {
        return error;
    }

    listenerSocket->close();

    bslstl::SharedPtrUtil::dynamicCast(server, serverStreamSocket);
    if (!*server) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *client = clientStreamSocket;

    BSLS_ASSERT(*client);
    BSLS_ASSERT(*server);

    return ntsa::Error();
}

}  // close package namespace
}  // close enterprise namespace
