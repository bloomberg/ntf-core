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

#include <ntsb_streamsocket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsb_streamsocket_cpp, "$Id$ $CSID$")

#include <ntsu_socketoptionutil.h>
#include <ntsu_socketutil.h>

namespace BloombergLP {
namespace ntsb {

StreamSocket::StreamSocket()
: d_handle(ntsa::k_INVALID_HANDLE)
{
}

StreamSocket::StreamSocket(ntsa::Handle handle)
: d_handle(handle)
{
}

StreamSocket::~StreamSocket()
{
    if (d_handle != ntsa::k_INVALID_HANDLE) {
        ntsu::SocketUtil::close(d_handle);
    }
}

ntsa::Error StreamSocket::open(ntsa::Transport::Value transport)
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

ntsa::Error StreamSocket::acquire(ntsa::Handle handle)
{
    if (d_handle != ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_handle = handle;

    return ntsa::Error();
}

ntsa::Handle StreamSocket::release()
{
    ntsa::Handle result = d_handle;
    d_handle            = ntsa::k_INVALID_HANDLE;
    return result;
}

ntsa::Error StreamSocket::bind(const ntsa::Endpoint& endpoint,
                               bool                  reuseAddress)
{
    return ntsu::SocketUtil::bind(endpoint, reuseAddress, d_handle);
}

ntsa::Error StreamSocket::bindAny(ntsa::Transport::Value transport,
                                  bool                   reuseAddress)
{
    return ntsu::SocketUtil::bindAny(transport, reuseAddress, d_handle);
}

ntsa::Error StreamSocket::connect(const ntsa::Endpoint& endpoint)
{
    return ntsu::SocketUtil::connect(endpoint, d_handle);
}

ntsa::Error StreamSocket::send(ntsa::SendContext*       context,
                               const bdlbb::Blob&       data,
                               const ntsa::SendOptions& options)
{
    return ntsu::SocketUtil::send(context, data, options, d_handle);
}

ntsa::Error StreamSocket::send(ntsa::SendContext*       context,
                               const ntsa::Data&        data,
                               const ntsa::SendOptions& options)
{
    return ntsu::SocketUtil::send(context, data, options, d_handle);
}

ntsa::Error StreamSocket::receive(ntsa::ReceiveContext*       context,
                                  bdlbb::Blob*                data,
                                  const ntsa::ReceiveOptions& options)
{
    return ntsu::SocketUtil::receive(context, data, options, d_handle);
}

ntsa::Error StreamSocket::receive(ntsa::ReceiveContext*       context,
                                  ntsa::Data*                 data,
                                  const ntsa::ReceiveOptions& options)
{
    return ntsu::SocketUtil::receive(context, data, options, d_handle);
}

ntsa::Error StreamSocket::receiveNotifications(
    ntsa::NotificationQueue* notifications)
{
    return ntsu::SocketUtil::receiveNotifications(notifications, d_handle);
}

ntsa::Error StreamSocket::shutdown(ntsa::ShutdownType::Value direction)
{
    return ntsu::SocketUtil::shutdown(direction, d_handle);
}

ntsa::Error StreamSocket::unlink()
{
    return ntsu::SocketUtil::unlink(d_handle);
}

ntsa::Error StreamSocket::close()
{
    ntsa::Handle handle = d_handle;
    d_handle            = ntsa::k_INVALID_HANDLE;

    return ntsu::SocketUtil::close(handle);
}

ntsa::Error StreamSocket::sourceEndpoint(ntsa::Endpoint* result) const
{
    return ntsu::SocketUtil::sourceEndpoint(result, d_handle);
}

ntsa::Error StreamSocket::remoteEndpoint(ntsa::Endpoint* result) const
{
    return ntsu::SocketUtil::remoteEndpoint(result, d_handle);
}

ntsa::Handle StreamSocket::handle() const
{
    return d_handle;
}

// *** Socket Options ***

ntsa::Error StreamSocket::setBlocking(bool blocking)
{
    return ntsu::SocketOptionUtil::setBlocking(d_handle, blocking);
}

ntsa::Error StreamSocket::setOption(const ntsa::SocketOption& option)
{
    return ntsu::SocketOptionUtil::setOption(d_handle, option);
}

ntsa::Error StreamSocket::getOption(ntsa::SocketOption*           option,
                                    ntsa::SocketOptionType::Value type)
{
    return ntsu::SocketOptionUtil::getOption(option, type, d_handle);
}

ntsa::Error StreamSocket::getLastError(ntsa::Error* result)
{
    return ntsu::SocketOptionUtil::getLastError(result, d_handle);
}

bsl::size_t StreamSocket::maxBuffersPerSend() const
{
    return ntsu::SocketUtil::maxBuffersPerSend();
}

bsl::size_t StreamSocket::maxBuffersPerReceive() const
{
    return ntsu::SocketUtil::maxBuffersPerReceive();
}

ntsa::Error StreamSocket::pair(ntsb::StreamSocket*    client,
                               ntsb::StreamSocket*    server,
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

ntsa::Error StreamSocket::pair(bslma::ManagedPtr<ntsb::StreamSocket>* client,
                               bslma::ManagedPtr<ntsb::StreamSocket>* server,
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

    client->load(new (*allocator) ntsb::StreamSocket(clientHandle), allocator);
    server->load(new (*allocator) ntsb::StreamSocket(serverHandle), allocator);

    return ntsa::Error();
}

ntsa::Error StreamSocket::pair(bsl::shared_ptr<ntsb::StreamSocket>* client,
                               bsl::shared_ptr<ntsb::StreamSocket>* server,
                               ntsa::Transport::Value               type,
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
