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

#include <ntsi_streamsocket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsi_streamsocket_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntsi {

StreamSocket::~StreamSocket()
{
}

ntsa::Error StreamSocket::bind(const ntsa::Endpoint& endpoint,
                               bool                  reuseAddress)
{
    NTSCFG_WARNING_UNUSED(endpoint);
    NTSCFG_WARNING_UNUSED(reuseAddress);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::bindAny(ntsa::Transport::Value transport,
                                  bool                   reuseAddress)
{
    NTSCFG_WARNING_UNUSED(transport);
    NTSCFG_WARNING_UNUSED(reuseAddress);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::connect(const ntsa::Endpoint& endpoint)
{
    NTSCFG_WARNING_UNUSED(endpoint);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::send(ntsa::SendContext*       context,
                               const bdlbb::Blob&       data,
                               const ntsa::SendOptions& options)
{
    NTSCFG_WARNING_UNUSED(context);
    NTSCFG_WARNING_UNUSED(data);
    NTSCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::send(ntsa::SendContext*       context,
                               const ntsa::Data&        data,
                               const ntsa::SendOptions& options)
{
    NTSCFG_WARNING_UNUSED(context);
    NTSCFG_WARNING_UNUSED(data);
    NTSCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::receive(ntsa::ReceiveContext*       context,
                                  bdlbb::Blob*                data,
                                  const ntsa::ReceiveOptions& options)
{
    NTSCFG_WARNING_UNUSED(context);
    NTSCFG_WARNING_UNUSED(data);
    NTSCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::receive(ntsa::ReceiveContext*       context,
                                  ntsa::Data*                 data,
                                  const ntsa::ReceiveOptions& options)
{
    NTSCFG_WARNING_UNUSED(context);
    NTSCFG_WARNING_UNUSED(data);
    NTSCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::receiveNotifications(
    ntsa::NotificationQueue* notifications)
{
    NTSCFG_WARNING_UNUSED(notifications);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::shutdown(ntsa::ShutdownType::Value direction)
{
    NTSCFG_WARNING_UNUSED(direction);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::unlink()
{
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::close()
{
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::sourceEndpoint(ntsa::Endpoint* result) const
{
    NTSCFG_WARNING_UNUSED(result);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::remoteEndpoint(ntsa::Endpoint* result) const
{
    NTSCFG_WARNING_UNUSED(result);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::setBlocking(bool blocking)
{
    NTSCFG_WARNING_UNUSED(blocking);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::setOption(const ntsa::SocketOption& option)
{
    NTSCFG_WARNING_UNUSED(option);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::getOption(ntsa::SocketOption*           option,
                                    ntsa::SocketOptionType::Value type)
{
    NTSCFG_WARNING_UNUSED(option);
    NTSCFG_WARNING_UNUSED(type);

    option->reset();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::getLastError(ntsa::Error* result)
{
    *result = ntsa::Error();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bsl::size_t StreamSocket::maxBuffersPerSend() const
{
    return 1;
}

bsl::size_t StreamSocket::maxBuffersPerReceive() const
{
    return 1;
}

}  // close package namespace
}  // close enterprise namespace
