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

#include <ntsi_listenersocket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsi_listenersocket_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntsi {

ListenerSocket::~ListenerSocket()
{
}

ntsa::Error ListenerSocket::bind(const ntsa::Endpoint& endpoint,
                                 bool                  reuseAddress)
{
    NTSCFG_WARNING_UNUSED(endpoint);
    NTSCFG_WARNING_UNUSED(reuseAddress);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error ListenerSocket::bindAny(ntsa::Transport::Value transport,
                                    bool                   reuseAddress)
{
    NTSCFG_WARNING_UNUSED(transport);
    NTSCFG_WARNING_UNUSED(reuseAddress);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error ListenerSocket::listen(bsl::size_t backlog)
{
    NTSCFG_WARNING_UNUSED(backlog);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error ListenerSocket::accept(ntsa::Handle* result)
{
    NTSCFG_WARNING_UNUSED(result);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error ListenerSocket::accept(
    bslma::ManagedPtr<ntsi::StreamSocket>* result,
    bslma::Allocator*                      basicAllocator)
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_WARNING_UNUSED(basicAllocator);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error ListenerSocket::accept(bsl::shared_ptr<ntsi::StreamSocket>* result,
                                   bslma::Allocator* basicAllocator)
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_WARNING_UNUSED(basicAllocator);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error ListenerSocket::receiveNotifications(
    ntsa::NotificationQueue* notifications)
{
    NTSCFG_WARNING_UNUSED(notifications);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error ListenerSocket::shutdown(ntsa::ShutdownType::Value direction)
{
    NTSCFG_WARNING_UNUSED(direction);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error ListenerSocket::unlink()
{
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error ListenerSocket::close()
{
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error ListenerSocket::sourceEndpoint(ntsa::Endpoint* result) const
{
    NTSCFG_WARNING_UNUSED(result);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error ListenerSocket::setBlocking(bool blocking)
{
    NTSCFG_WARNING_UNUSED(blocking);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error ListenerSocket::setOption(const ntsa::SocketOption& option)
{
    NTSCFG_WARNING_UNUSED(option);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error ListenerSocket::getOption(ntsa::SocketOption*           option,
                                      ntsa::SocketOptionType::Value type)
{
    NTSCFG_WARNING_UNUSED(option);
    NTSCFG_WARNING_UNUSED(type);

    option->reset();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

}  // close package namespace
}  // close enterprise namespace
