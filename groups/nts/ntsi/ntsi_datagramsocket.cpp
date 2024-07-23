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

#include <ntsi_datagramsocket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsi_datagramsocket_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntsi {

DatagramSocket::~DatagramSocket()
{
}

ntsa::Error DatagramSocket::bind(const ntsa::Endpoint& endpoint,
                                 bool                  reuseAddress)
{
    NTSCFG_WARNING_UNUSED(endpoint);
    NTSCFG_WARNING_UNUSED(reuseAddress);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::bindAny(ntsa::Transport::Value transport,
                                    bool                   reuseAddress)
{
    NTSCFG_WARNING_UNUSED(transport);
    NTSCFG_WARNING_UNUSED(reuseAddress);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::connect(const ntsa::Endpoint& endpoint)
{
    NTSCFG_WARNING_UNUSED(endpoint);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::send(ntsa::SendContext*       context,
                                 const bdlbb::Blob&       data,
                                 const ntsa::SendOptions& options)
{
    NTSCFG_WARNING_UNUSED(context);
    NTSCFG_WARNING_UNUSED(data);
    NTSCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::send(ntsa::SendContext*       context,
                                 const ntsa::Data&        data,
                                 const ntsa::SendOptions& options)
{
    NTSCFG_WARNING_UNUSED(context);
    NTSCFG_WARNING_UNUSED(data);
    NTSCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::send(ntsa::SendContext*       context,
                                 const ntsa::ConstBuffer *data,
                                 bsl::size_t              size,
                                 const ntsa::SendOptions& options)
{
    ntsa::ConstBufferArray array;
    array.append(data, size);

    return this->send(context, ntsa::Data(array), options);
}

ntsa::Error DatagramSocket::receive(ntsa::ReceiveContext*       context,
                                    bdlbb::Blob*                data,
                                    const ntsa::ReceiveOptions& options)
{
    NTSCFG_WARNING_UNUSED(context);
    NTSCFG_WARNING_UNUSED(data);
    NTSCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::receive(ntsa::ReceiveContext*       context,
                                    ntsa::Data*                 data,
                                    const ntsa::ReceiveOptions& options)
{
    NTSCFG_WARNING_UNUSED(context);
    NTSCFG_WARNING_UNUSED(data);
    NTSCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::receiveNotifications(
    ntsa::NotificationQueue* notifications)
{
    NTSCFG_WARNING_UNUSED(notifications);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::shutdown(ntsa::ShutdownType::Value direction)
{
    NTSCFG_WARNING_UNUSED(direction);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::unlink()
{
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::close()
{
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::sourceEndpoint(ntsa::Endpoint* result) const
{
    NTSCFG_WARNING_UNUSED(result);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::remoteEndpoint(ntsa::Endpoint* result) const
{
    NTSCFG_WARNING_UNUSED(result);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::setMulticastLoopback(bool enabled)
{
    NTSCFG_WARNING_UNUSED(enabled);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::setMulticastInterface(
    const ntsa::IpAddress& interface)
{
    NTSCFG_WARNING_UNUSED(interface);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::setMulticastTimeToLive(bsl::size_t maxHops)
{
    NTSCFG_WARNING_UNUSED(maxHops);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::joinMulticastGroup(
    const ntsa::IpAddress& interface,
    const ntsa::IpAddress& group)
{
    NTSCFG_WARNING_UNUSED(interface);
    NTSCFG_WARNING_UNUSED(group);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::leaveMulticastGroup(
    const ntsa::IpAddress& interface,
    const ntsa::IpAddress& group)
{
    NTSCFG_WARNING_UNUSED(interface);
    NTSCFG_WARNING_UNUSED(group);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::joinMulticastGroupSource(
        const ntsa::IpAddress& interface,
        const ntsa::IpAddress& group,
        const ntsa::IpAddress& source)
{
    NTSCFG_WARNING_UNUSED(interface);
    NTSCFG_WARNING_UNUSED(group);
    NTSCFG_WARNING_UNUSED(source);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::leaveMulticastGroupSource(
        const ntsa::IpAddress& interface,
        const ntsa::IpAddress& group,
        const ntsa::IpAddress& source)
{
    NTSCFG_WARNING_UNUSED(interface);
    NTSCFG_WARNING_UNUSED(group);
    NTSCFG_WARNING_UNUSED(source);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::setBlocking(bool blocking)
{
    NTSCFG_WARNING_UNUSED(blocking);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::setOption(const ntsa::SocketOption& option)
{
    NTSCFG_WARNING_UNUSED(option);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::getBlocking(bool* blocking) const
{
    *blocking = false;
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::getOption(ntsa::SocketOption*           option,
                                      ntsa::SocketOptionType::Value type)
{
    NTSCFG_WARNING_UNUSED(option);
    NTSCFG_WARNING_UNUSED(type);

    option->reset();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bsl::size_t DatagramSocket::maxBuffersPerSend() const
{
    return 1;
}

bsl::size_t DatagramSocket::maxBuffersPerReceive() const
{
    return 1;
}

}  // close package namespace
}  // close enterprise namespace
