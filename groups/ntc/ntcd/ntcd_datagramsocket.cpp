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

#include <ntcd_datagramsocket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcd_datagramsocket_cpp, "$Id$ $CSID$")

#include <bslmt_lockguard.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcd {

DatagramSocket::DatagramSocket(bslma::Allocator* basicAllocator)
: d_mutex()
, d_machine_sp(ntcd::Machine::getDefault())
, d_session_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT_OPT(d_machine_sp);
}

DatagramSocket::DatagramSocket(const bsl::shared_ptr<ntcd::Machine>& machine,
                               bslma::Allocator* basicAllocator)
: d_mutex()
, d_machine_sp(machine)
, d_session_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT_OPT(d_machine_sp);
}

DatagramSocket::~DatagramSocket()
{
}

ntsa::Error DatagramSocket::open(ntsa::Transport::Value transport)
{
    LockGuard lock(&d_mutex);

    if (d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_session_sp = d_machine_sp->createSession(d_allocator_p);

    ntsa::Error error = d_session_sp->open(transport);
    if (error) {
        d_session_sp.reset();
        return error;
    }

    return ntsa::Error();
}

ntsa::Error DatagramSocket::acquire(ntsa::Handle handle)
{
    LockGuard lock(&d_mutex);

    if (d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_session_sp = d_machine_sp->createSession(d_allocator_p);

    ntsa::Error error = d_session_sp->acquire(handle);
    if (error) {
        d_session_sp.reset();
        return error;
    }

    return ntsa::Error();
}

ntsa::Handle DatagramSocket::release()
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::k_INVALID_HANDLE;
    }

    ntsa::Handle result = d_session_sp->release();
    d_session_sp.reset();

    return result;
}

ntsa::Error DatagramSocket::bind(const ntsa::Endpoint& endpoint,
                                 bool                  reuseAddress)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->bind(endpoint, reuseAddress);
}

ntsa::Error DatagramSocket::bindAny(ntsa::Transport::Value transport,
                                    bool                   reuseAddress)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->bindAny(transport, reuseAddress);
}

ntsa::Error DatagramSocket::connect(const ntsa::Endpoint& endpoint)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->connect(endpoint);
}

ntsa::Error DatagramSocket::send(ntsa::SendContext*       context,
                                 const bdlbb::Blob&       data,
                                 const ntsa::SendOptions& options)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->send(context, data, options);
}

ntsa::Error DatagramSocket::send(ntsa::SendContext*       context,
                                 const ntsa::Data&        data,
                                 const ntsa::SendOptions& options)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->send(context, data, options);
}

ntsa::Error DatagramSocket::send(ntsa::SendContext*       context,
                                 const ntsa::ConstBuffer* data,
                                 bsl::size_t              size,
                                 const ntsa::SendOptions& options)
{
    return ntsi::DatagramSocket::send(context, data, size, options);
}

ntsa::Error DatagramSocket::receive(ntsa::ReceiveContext*       context,
                                    bdlbb::Blob*                data,
                                    const ntsa::ReceiveOptions& options)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->receive(context, data, options);
}

ntsa::Error DatagramSocket::receive(ntsa::ReceiveContext*       context,
                                    ntsa::Data*                 data,
                                    const ntsa::ReceiveOptions& options)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->receive(context, data, options);
}

ntsa::Error DatagramSocket::shutdown(ntsa::ShutdownType::Value direction)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->shutdown(direction);
}

ntsa::Error DatagramSocket::unlink()
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->unlink();
}

ntsa::Error DatagramSocket::close()
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Error error = d_session_sp->close();
    d_session_sp.reset();

    return error;
}

ntsa::Error DatagramSocket::sourceEndpoint(ntsa::Endpoint* result) const
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->sourceEndpoint(result);
}

ntsa::Error DatagramSocket::remoteEndpoint(ntsa::Endpoint* result) const
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->remoteEndpoint(result);
}

ntsa::Handle DatagramSocket::handle() const
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::k_INVALID_HANDLE;
    }

    return d_session_sp->handle();
}

// *** Multicasting ***

ntsa::Error DatagramSocket::setMulticastLoopback(bool enabled)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->setMulticastLoopback(enabled);
}

ntsa::Error DatagramSocket::setMulticastInterface(
    const ntsa::IpAddress& interface)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->setMulticastInterface(interface);
}

ntsa::Error DatagramSocket::setMulticastTimeToLive(bsl::size_t maxHops)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->setMulticastTimeToLive(maxHops);
}

ntsa::Error DatagramSocket::joinMulticastGroup(
    const ntsa::IpAddress& interface,
    const ntsa::IpAddress& group)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->joinMulticastGroup(interface, group);
}

ntsa::Error DatagramSocket::leaveMulticastGroup(
    const ntsa::IpAddress& interface,
    const ntsa::IpAddress& group)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->leaveMulticastGroup(interface, group);
}

// *** Socket Options ***

ntsa::Error DatagramSocket::setBlocking(bool blocking)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->setBlocking(blocking);
}

ntsa::Error DatagramSocket::setOption(const ntsa::SocketOption& option)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->setOption(option);
}

ntsa::Error DatagramSocket::getOption(ntsa::SocketOption*           option,
                                      ntsa::SocketOptionType::Value type)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->getOption(option, type);
}

bsl::size_t DatagramSocket::maxBuffersPerSend() const
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return 1;
    }

    return d_session_sp->maxBuffersPerSend();
}

bsl::size_t DatagramSocket::maxBuffersPerReceive() const
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return 1;
    }

    return d_session_sp->maxBuffersPerReceive();
}

DatagramSocketFactory::DatagramSocketFactory(const Function&   function,
                                             bslma::Allocator* basicAllocator)
: d_function(bsl::allocator_arg, basicAllocator, function)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

DatagramSocketFactory::~DatagramSocketFactory()
{
}

bsl::shared_ptr<ntci::DatagramSocket> DatagramSocketFactory::
    createDatagramSocket(const ntca::DatagramSocketOptions& options,
                         bslma::Allocator*                  basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);
    return d_function(options, allocator);
}

}  // close package namespace
}  // close enterprise namespace
