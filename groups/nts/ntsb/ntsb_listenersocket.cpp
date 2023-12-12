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

#include <ntsb_listenersocket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsb_listenersocket_cpp, "$Id$ $CSID$")

#include <ntsu_socketoptionutil.h>
#include <ntsu_socketutil.h>

namespace BloombergLP {
namespace ntsb {

ListenerSocket::ListenerSocket()
: d_handle(ntsa::k_INVALID_HANDLE)
{
}

ListenerSocket::ListenerSocket(ntsa::Handle handle)
: d_handle(handle)
{
}

ListenerSocket::~ListenerSocket()
{
    if (d_handle != ntsa::k_INVALID_HANDLE) {
        ntsu::SocketUtil::close(d_handle);
    }
}

ntsa::Error ListenerSocket::open(ntsa::Transport::Value transport)
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

ntsa::Error ListenerSocket::acquire(ntsa::Handle handle)
{
    if (d_handle != ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_handle = handle;

    return ntsa::Error();
}

ntsa::Handle ListenerSocket::release()
{
    ntsa::Handle result = d_handle;
    d_handle            = ntsa::k_INVALID_HANDLE;
    return result;
}

ntsa::Error ListenerSocket::bind(const ntsa::Endpoint& endpoint,
                                 bool                  reuseAddress)
{
    return ntsu::SocketUtil::bind(endpoint, reuseAddress, d_handle);
}

ntsa::Error ListenerSocket::bindAny(ntsa::Transport::Value transport,
                                    bool                   reuseAddress)
{
    return ntsu::SocketUtil::bindAny(transport, reuseAddress, d_handle);
}

ntsa::Error ListenerSocket::listen(bsl::size_t backlog)
{
    return ntsu::SocketUtil::listen(backlog, d_handle);
}

ntsa::Error ListenerSocket::accept(ntsa::Handle* result)
{
    return ntsu::SocketUtil::accept(result, d_handle);
}

ntsa::Error ListenerSocket::accept(ntsb::StreamSocket* result)
{
    ntsa::Handle handle;
    ntsa::Error  error = ntsu::SocketUtil::accept(&handle, d_handle);
    if (error) {
        return error;
    }

    result->acquire(handle);
    return ntsa::Error();
}

ntsa::Error ListenerSocket::accept(
    bslma::ManagedPtr<ntsi::StreamSocket>* result,
    bslma::Allocator*                      basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntsa::Handle handle;
    ntsa::Error  error = ntsu::SocketUtil::accept(&handle, d_handle);
    if (error) {
        return error;
    }

    result->load(new (*allocator) ntsb::StreamSocket(handle), allocator);

    return ntsa::Error();
}

ntsa::Error ListenerSocket::accept(bsl::shared_ptr<ntsi::StreamSocket>* result,
                                   bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntsa::Handle handle;
    ntsa::Error  error = ntsu::SocketUtil::accept(&handle, d_handle);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntsb::StreamSocket> channel;
    channel.createInplace(allocator, handle);

    *result = channel;

    return ntsa::Error();
}

ntsa::Error ListenerSocket::receiveNotifications(
    ntsa::NotificationQueue* notifications)
{
    return ntsu::SocketUtil::receiveNotifications(notifications, d_handle);
}

ntsa::Error ListenerSocket::shutdown(ntsa::ShutdownType::Value direction)
{
    return ntsu::SocketUtil::shutdown(direction, d_handle);
}

ntsa::Error ListenerSocket::unlink()
{
    return ntsu::SocketUtil::unlink(d_handle);
}

ntsa::Error ListenerSocket::close()
{
    ntsa::Handle handle = d_handle;
    d_handle            = ntsa::k_INVALID_HANDLE;

    return ntsu::SocketUtil::close(handle);
}

ntsa::Error ListenerSocket::sourceEndpoint(ntsa::Endpoint* result) const
{
    return ntsu::SocketUtil::sourceEndpoint(result, d_handle);
}

ntsa::Handle ListenerSocket::handle() const
{
    return d_handle;
}

// *** Socket Options ***

ntsa::Error ListenerSocket::setBlocking(bool blocking)
{
    return ntsu::SocketOptionUtil::setBlocking(d_handle, blocking);
}

ntsa::Error ListenerSocket::setOption(const ntsa::SocketOption& option)
{
    return ntsu::SocketOptionUtil::setOption(d_handle, option);
}

ntsa::Error ListenerSocket::getOption(ntsa::SocketOption*           option,
                                      ntsa::SocketOptionType::Value type)
{
    return ntsu::SocketOptionUtil::getOption(option, type, d_handle);
}

}  // close package namespace
}  // close enterprise namespace
