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

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsb_datagramsocket_cpp, "$Id$ $CSID$")

#include <ntsu_socketoptionutil.h>
#include <ntsu_socketutil.h>

namespace BloombergLP {
namespace ntsb {

DatagramSocket::DatagramSocket()
: d_handle(ntsa::k_INVALID_HANDLE)
{
}

DatagramSocket::DatagramSocket(ntsa::Handle handle)
: d_handle(handle)
{
}

DatagramSocket::~DatagramSocket()
{
    if (d_handle != ntsa::k_INVALID_HANDLE) {
        ntsu::SocketUtil::close(d_handle);
    }
}

ntsa::Error DatagramSocket::open(ntsa::Transport::Value transport)
{
    if (d_handle != ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Handle handle;
    ntsa::Error  error = ntsu::SocketUtil::create(&handle, transport);

    if (error) {
        return error;
    }

    d_handle = handle;

    return ntsa::Error();
}

ntsa::Error DatagramSocket::acquire(ntsa::Handle handle)
{
    if (d_handle != ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_handle = handle;

    return ntsa::Error();
}

ntsa::Handle DatagramSocket::release()
{
    ntsa::Handle result = d_handle;
    d_handle            = ntsa::k_INVALID_HANDLE;
    return result;
}

ntsa::Error DatagramSocket::bind(const ntsa::Endpoint& endpoint,
                                 bool                  reuseAddress)
{
    return ntsu::SocketUtil::bind(endpoint, reuseAddress, d_handle);
}

ntsa::Error DatagramSocket::bindAny(ntsa::Transport::Value transport,
                                    bool                   reuseAddress)
{
    return ntsu::SocketUtil::bindAny(transport, reuseAddress, d_handle);
}

ntsa::Error DatagramSocket::connect(const ntsa::Endpoint& endpoint)
{
    return ntsu::SocketUtil::connect(endpoint, d_handle);
}

ntsa::Error DatagramSocket::send(ntsa::SendContext*       context,
                                 const bdlbb::Blob&       data,
                                 const ntsa::SendOptions& options)
{
    return ntsu::SocketUtil::send(context, data, options, d_handle);
}

ntsa::Error DatagramSocket::send(ntsa::SendContext*       context,
                                 const ntsa::Data&        data,
                                 const ntsa::SendOptions& options)
{
    return ntsu::SocketUtil::send(context, data, options, d_handle);
}

ntsa::Error DatagramSocket::send(ntsa::SendContext*       context,
                                 const ntsa::ConstBuffer *data,
                                 bsl::size_t              size,
                                 const ntsa::SendOptions& options)
{
    return ntsu::SocketUtil::send(context, data, size, options, d_handle);
}

ntsa::Error DatagramSocket::receive(ntsa::ReceiveContext*       context,
                                    bdlbb::Blob*                data,
                                    const ntsa::ReceiveOptions& options)
{
    return ntsu::SocketUtil::receive(context, data, options, d_handle);
}

ntsa::Error DatagramSocket::receive(ntsa::ReceiveContext*       context,
                                    ntsa::Data*                 data,
                                    const ntsa::ReceiveOptions& options)
{
    return ntsu::SocketUtil::receive(context, data, options, d_handle);
}

ntsa::Error DatagramSocket::receiveNotifications(
    ntsa::NotificationQueue* notifications)
{
    return ntsu::SocketUtil::receiveNotifications(notifications, d_handle);
}

ntsa::Error DatagramSocket::shutdown(ntsa::ShutdownType::Value direction)
{
    return ntsu::SocketUtil::shutdown(direction, d_handle);
}

ntsa::Error DatagramSocket::unlink()
{
    return ntsu::SocketUtil::unlink(d_handle);
}

ntsa::Error DatagramSocket::close()
{
    ntsa::Handle handle = d_handle;
    d_handle            = ntsa::k_INVALID_HANDLE;

    return ntsu::SocketUtil::close(handle);
}

ntsa::Error DatagramSocket::sourceEndpoint(ntsa::Endpoint* result) const
{
    return ntsu::SocketUtil::sourceEndpoint(result, d_handle);
}

ntsa::Error DatagramSocket::remoteEndpoint(ntsa::Endpoint* result) const
{
    return ntsu::SocketUtil::remoteEndpoint(result, d_handle);
}

ntsa::Handle DatagramSocket::handle() const
{
    return d_handle;
}

// *** Multicasting ***

ntsa::Error DatagramSocket::setMulticastLoopback(bool enabled)
{
    return ntsu::SocketOptionUtil::setMulticastLoopback(d_handle, enabled);
}

ntsa::Error DatagramSocket::setMulticastInterface(
    const ntsa::IpAddress& interface)
{
    return ntsu::SocketOptionUtil::setMulticastInterface(d_handle, interface);
}

ntsa::Error DatagramSocket::setMulticastTimeToLive(bsl::size_t maxHops)
{
    return ntsu::SocketOptionUtil::setMulticastTimeToLive(d_handle, maxHops);
}

ntsa::Error DatagramSocket::joinMulticastGroup(
    const ntsa::IpAddress& interface,
    const ntsa::IpAddress& group)
{
    return ntsu::SocketOptionUtil::joinMulticastGroup(d_handle,
                                                      interface,
                                                      group);
}

ntsa::Error DatagramSocket::joinMulticastGroupSource(
        const ntsa::IpAddress& interface,
        const ntsa::IpAddress& group,
        const ntsa::IpAddress& source)
{
    return ntsu::SocketOptionUtil::joinMulticastGroupSource(
        d_handle, interface, group, source);
}

ntsa::Error DatagramSocket::leaveMulticastGroupSource(
        const ntsa::IpAddress& interface,
        const ntsa::IpAddress& group,
        const ntsa::IpAddress& source)
{
    return ntsu::SocketOptionUtil::leaveMulticastGroupSource(
        d_handle, interface, group, source);
}

ntsa::Error DatagramSocket::leaveMulticastGroup(
    const ntsa::IpAddress& interface,
    const ntsa::IpAddress& group)
{
    return ntsu::SocketOptionUtil::leaveMulticastGroup(d_handle,
                                                       interface,
                                                       group);
}

// *** Socket Options ***

ntsa::Error DatagramSocket::setBlocking(bool blocking)
{
    return ntsu::SocketOptionUtil::setBlocking(d_handle, blocking);
}

ntsa::Error DatagramSocket::setOption(const ntsa::SocketOption& option)
{
    return ntsu::SocketOptionUtil::setOption(d_handle, option);
}

ntsa::Error DatagramSocket::getBlocking(bool* blocking) const
{
    return ntsu::SocketOptionUtil::getBlocking(d_handle, blocking);
}

ntsa::Error DatagramSocket::getOption(ntsa::SocketOption*           option,
                                      ntsa::SocketOptionType::Value type)
{
    return ntsu::SocketOptionUtil::getOption(option, type, d_handle);
}

bsl::size_t DatagramSocket::maxBuffersPerSend() const
{
    return ntsu::SocketUtil::maxBuffersPerSend();
}

bsl::size_t DatagramSocket::maxBuffersPerReceive() const
{
    return ntsu::SocketUtil::maxBuffersPerReceive();
}

ntsa::Error DatagramSocket::pair(ntsb::DatagramSocket*  client,
                                 ntsb::DatagramSocket*  server,
                                 ntsa::Transport::Value type)
{
    ntsa::Handle clientHandle;
    ntsa::Handle serverHandle;

    ntsa::Error error =
        ntsu::SocketUtil::pair(&clientHandle, &serverHandle, type);

    if (error) {
        return error;
    }

    client->acquire(clientHandle);
    server->acquire(serverHandle);

    return ntsa::Error();
}

ntsa::Error DatagramSocket::pair(
    bslma::ManagedPtr<ntsb::DatagramSocket>* client,
    bslma::ManagedPtr<ntsb::DatagramSocket>* server,
    ntsa::Transport::Value                   type,
    bslma::Allocator*                        basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntsa::Handle clientHandle;
    ntsa::Handle serverHandle;

    ntsa::Error error =
        ntsu::SocketUtil::pair(&clientHandle, &serverHandle, type);

    if (error) {
        return error;
    }

    client->load(new (*allocator) ntsb::DatagramSocket(clientHandle),
                 allocator);

    server->load(new (*allocator) ntsb::DatagramSocket(serverHandle),
                 allocator);

    return ntsa::Error();
}

ntsa::Error DatagramSocket::pair(bsl::shared_ptr<ntsb::DatagramSocket>* client,
                                 bsl::shared_ptr<ntsb::DatagramSocket>* server,
                                 ntsa::Transport::Value                 type,
                                 bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntsa::Handle clientHandle;
    ntsa::Handle serverHandle;

    ntsa::Error error =
        ntsu::SocketUtil::pair(&clientHandle, &serverHandle, type);

    if (error) {
        return error;
    }

    client->createInplace(allocator, clientHandle);
    server->createInplace(allocator, serverHandle);

    return ntsa::Error();
}

}  // close package namespace
}  // close enterprise namespace
