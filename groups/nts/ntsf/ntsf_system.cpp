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

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsf_system_cpp, "$Id$ $CSID$")

#include <ntsb_datagramsocket.h>
#include <ntsb_listenersocket.h>
#include <ntsb_resolver.h>
#include <ntsb_streamsocket.h>
#include <ntscfg_limits.h>
#include <ntsu_adapterutil.h>
#include <ntsu_bufferutil.h>
#include <ntsu_socketoptionutil.h>
#include <ntsu_socketutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_once.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntsf {

ntsa::Error System::initialize()
{
    BSLMT_ONCE_DO
    {
        int rc = ntscfg::Platform::initialize();
        if (rc != 0) {
            return ntsa::Error(ntsa::Error::last());
        }
    }
    return ntsa::Error();
}

ntsa::Error System::ignore(ntscfg::Signal::Value signal)
{
    int rc = ntscfg::Platform::ignore(signal);
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::last());
    }
    return ntsa::Error();
}

ntsa::Error System::createDatagramSocket(ntsa::Handle*          result,
                                         ntsa::Transport::Value type)
{
    ntsa::TransportMode::Value transportMode = ntsa::Transport::getMode(type);

    if (transportMode != ntsa::TransportMode::e_DATAGRAM) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsu::SocketUtil::create(result, type);
}

bsl::shared_ptr<ntsi::DatagramSocket> System::createDatagramSocket(
    bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntsb::DatagramSocket> datagramSocket;
    datagramSocket.createInplace(allocator);

    return datagramSocket;
}

bsl::shared_ptr<ntsi::DatagramSocket> System::createDatagramSocket(
    ntsa::Handle      handle,
    bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntsb::DatagramSocket> datagramSocket;
    datagramSocket.createInplace(allocator, handle);

    return datagramSocket;
}

ntsa::Error System::createDatagramSocketPair(ntsa::Handle*          client,
                                             ntsa::Handle*          server,
                                             ntsa::Transport::Value type)
{
    ntsa::TransportMode::Value transportMode = ntsa::Transport::getMode(type);

    if (transportMode != ntsa::TransportMode::e_DATAGRAM) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsu::SocketUtil::pair(client, server, type);
}

ntsa::Error System::createDatagramSocketPair(
    bslma::ManagedPtr<ntsi::DatagramSocket>* client,
    bslma::ManagedPtr<ntsi::DatagramSocket>* server,
    ntsa::Transport::Value                   type,
    bslma::Allocator*                        basicAllocator)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bslma::ManagedPtr<ntsb::DatagramSocket> concreteClient;
    bslma::ManagedPtr<ntsb::DatagramSocket> concreteServer;

    error = ntsb::DatagramSocket::pair(&concreteClient,
                                       &concreteServer,
                                       type,
                                       allocator);
    if (error) {
        return error;
    }

    *client = concreteClient;
    *server = concreteServer;

    return ntsa::Error();
}

ntsa::Error System::createDatagramSocketPair(
    bsl::shared_ptr<ntsi::DatagramSocket>* client,
    bsl::shared_ptr<ntsi::DatagramSocket>* server,
    ntsa::Transport::Value                 type,
    bslma::Allocator*                      basicAllocator)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntsb::DatagramSocket> concreteClient;
    bsl::shared_ptr<ntsb::DatagramSocket> concreteServer;

    error = ntsb::DatagramSocket::pair(&concreteClient,
                                       &concreteServer,
                                       type,
                                       allocator);
    if (error) {
        return error;
    }

    *client = concreteClient;
    *server = concreteServer;

    return ntsa::Error();
}

ntsa::Error System::createListenerSocket(ntsa::Handle*          result,
                                         ntsa::Transport::Value type)
{
    ntsa::TransportMode::Value transportMode = ntsa::Transport::getMode(type);

    if (transportMode != ntsa::TransportMode::e_STREAM) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsu::SocketUtil::create(result, type);
}

bsl::shared_ptr<ntsi::ListenerSocket> System::createListenerSocket(
    bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntsb::ListenerSocket> listenerSocket;
    listenerSocket.createInplace(allocator);

    return listenerSocket;
}

bsl::shared_ptr<ntsi::ListenerSocket> System::createListenerSocket(
    ntsa::Handle      handle,
    bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntsb::ListenerSocket> listenerSocket;
    listenerSocket.createInplace(allocator, handle);

    return listenerSocket;
}

ntsa::Error System::createStreamSocket(ntsa::Handle*          result,
                                       ntsa::Transport::Value type)
{
    ntsa::TransportMode::Value transportMode = ntsa::Transport::getMode(type);

    if (transportMode != ntsa::TransportMode::e_STREAM) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsu::SocketUtil::create(result, type);
}

bsl::shared_ptr<ntsi::StreamSocket> System::createStreamSocket(
    bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntsb::StreamSocket> streamSocket;
    streamSocket.createInplace(allocator);

    return streamSocket;
}

bsl::shared_ptr<ntsi::StreamSocket> System::createStreamSocket(
    ntsa::Handle      handle,
    bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntsb::StreamSocket> streamSocket;
    streamSocket.createInplace(allocator, handle);

    return streamSocket;
}

ntsa::Error System::createStreamSocketPair(ntsa::Handle*          client,
                                           ntsa::Handle*          server,
                                           ntsa::Transport::Value type)
{
    ntsa::TransportMode::Value transportMode = ntsa::Transport::getMode(type);

    if (transportMode != ntsa::TransportMode::e_STREAM) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsu::SocketUtil::pair(client, server, type);
}

ntsa::Error System::createStreamSocketPair(
    bslma::ManagedPtr<ntsi::StreamSocket>* client,
    bslma::ManagedPtr<ntsi::StreamSocket>* server,
    ntsa::Transport::Value                 type,
    bslma::Allocator*                      basicAllocator)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bslma::ManagedPtr<ntsb::StreamSocket> concreteClient;
    bslma::ManagedPtr<ntsb::StreamSocket> concreteServer;

    error = ntsb::StreamSocket::pair(&concreteClient,
                                     &concreteServer,
                                     type,
                                     allocator);
    if (error) {
        return error;
    }

    *client = concreteClient;
    *server = concreteServer;

    return ntsa::Error();
}

ntsa::Error System::createStreamSocketPair(
    bsl::shared_ptr<ntsi::StreamSocket>* client,
    bsl::shared_ptr<ntsi::StreamSocket>* server,
    ntsa::Transport::Value               type,
    bslma::Allocator*                    basicAllocator)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntsb::StreamSocket> concreteClient;
    bsl::shared_ptr<ntsb::StreamSocket> concreteServer;

    error = ntsb::StreamSocket::pair(&concreteClient,
                                     &concreteServer,
                                     type,
                                     allocator);
    if (error) {
        return error;
    }

    *client = concreteClient;
    *server = concreteServer;

    return ntsa::Error();
}

bsl::shared_ptr<ntsi::Resolver> System::createResolver(
    bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntsb::Resolver> resolver;
    resolver.createInplace(allocator, allocator);

    return resolver;
}

ntsa::Error System::bind(ntsa::Handle          socket,
                         const ntsa::Endpoint& endpoint,
                         bool                  reuseAddress)
{
    return ntsu::SocketUtil::bind(endpoint, reuseAddress, socket);
}

ntsa::Error System::bindAny(ntsa::Handle           socket,
                            ntsa::Transport::Value type,
                            bool                   reuseAddress)
{
    return ntsu::SocketUtil::bindAny(type, reuseAddress, socket);
}

ntsa::Error System::listen(ntsa::Handle socket, bsl::size_t backlog)
{
    return ntsu::SocketUtil::listen(backlog, socket);
}

ntsa::Error System::connect(ntsa::Handle          socket,
                            const ntsa::Endpoint& endpoint)
{
    return ntsu::SocketUtil::connect(endpoint, socket);
}

ntsa::Error System::accept(ntsa::Handle* result, ntsa::Handle socket)
{
    return ntsu::SocketUtil::accept(result, socket);
}

ntsa::Error System::send(ntsa::SendContext*       context,
                         const void*              data,
                         bsl::size_t              size,
                         const ntsa::SendOptions& options,
                         ntsa::Handle             socket)
{
    return ntsu::SocketUtil::send(context, data, size, options, socket);
}

ntsa::Error System::send(ntsa::SendContext*       context,
                         const ntsa::ConstBuffer& buffer,
                         const ntsa::SendOptions& options,
                         ntsa::Handle             socket)
{
    return ntsu::SocketUtil::send(context, buffer, options, socket);
}

ntsa::Error System::send(ntsa::SendContext*       context,
                         const ntsa::ConstBuffer* bufferArray,
                         bsl::size_t              bufferCount,
                         const ntsa::SendOptions& options,
                         ntsa::Handle             socket)
{
    return ntsu::SocketUtil::send(context,
                                  bufferArray,
                                  bufferCount,
                                  options,
                                  socket);
}

ntsa::Error System::send(ntsa::SendContext*            context,
                         const ntsa::ConstBufferArray& bufferArray,
                         const ntsa::SendOptions&      options,
                         ntsa::Handle                  socket)
{
    return ntsu::SocketUtil::send(context, bufferArray, options, socket);
}

ntsa::Error System::send(ntsa::SendContext*               context,
                         const ntsa::ConstBufferPtrArray& bufferArray,
                         const ntsa::SendOptions&         options,
                         ntsa::Handle                     socket)
{
    return ntsu::SocketUtil::send(context, bufferArray, options, socket);
}

ntsa::Error System::send(ntsa::SendContext*         context,
                         const ntsa::MutableBuffer& buffer,
                         const ntsa::SendOptions&   options,
                         ntsa::Handle               socket)
{
    return ntsu::SocketUtil::send(context, buffer, options, socket);
}

ntsa::Error System::send(ntsa::SendContext*         context,
                         const ntsa::MutableBuffer* bufferArray,
                         bsl::size_t                bufferCount,
                         const ntsa::SendOptions&   options,
                         ntsa::Handle               socket)
{
    return ntsu::SocketUtil::send(context,
                                  bufferArray,
                                  bufferCount,
                                  options,
                                  socket);
}

ntsa::Error System::send(ntsa::SendContext*              context,
                         const ntsa::MutableBufferArray& bufferArray,
                         const ntsa::SendOptions&        options,
                         ntsa::Handle                    socket)
{
    return ntsu::SocketUtil::send(context, bufferArray, options, socket);
}

ntsa::Error System::send(ntsa::SendContext*                 context,
                         const ntsa::MutableBufferPtrArray& bufferArray,
                         const ntsa::SendOptions&           options,
                         ntsa::Handle                       socket)
{
    return ntsu::SocketUtil::send(context, bufferArray, options, socket);
}

ntsa::Error System::send(ntsa::SendContext*       context,
                         const bslstl::StringRef& string,
                         const ntsa::SendOptions& options,
                         ntsa::Handle             socket)
{
    return ntsu::SocketUtil::send(context, string, options, socket);
}

ntsa::Error System::send(ntsa::SendContext*       context,
                         const bdlbb::Blob&       blob,
                         const ntsa::SendOptions& options,
                         ntsa::Handle             socket)
{
    return ntsu::SocketUtil::send(context, blob, options, socket);
}

ntsa::Error System::send(ntsa::SendContext*       context,
                         const bdlbb::BlobBuffer& blobBuffer,
                         const ntsa::SendOptions& options,
                         ntsa::Handle             socket)
{
    return ntsu::SocketUtil::send(context, blobBuffer, options, socket);
}

ntsa::Error System::send(ntsa::SendContext*       context,
                         const ntsa::File&        file,
                         const ntsa::SendOptions& options,
                         ntsa::Handle             socket)
{
    return ntsu::SocketUtil::send(context, file, options, socket);
}

ntsa::Error System::send(ntsa::SendContext*       context,
                         const ntsa::Data&        data,
                         const ntsa::SendOptions& options,
                         ntsa::Handle             socket)
{
    return ntsu::SocketUtil::send(context, data, options, socket);
}

ntsa::Error System::receive(ntsa::ReceiveContext*       context,
                            void*                       data,
                            bsl::size_t                 capacity,
                            const ntsa::ReceiveOptions& options,
                            ntsa::Handle                socket)
{
    return ntsu::SocketUtil::receive(context, data, capacity, options, socket);
}

ntsa::Error System::receive(ntsa::ReceiveContext*       context,
                            ntsa::MutableBuffer*        buffer,
                            const ntsa::ReceiveOptions& options,
                            ntsa::Handle                socket)
{
    return ntsu::SocketUtil::receive(context, buffer, options, socket);
}

ntsa::Error System::receive(ntsa::ReceiveContext*       context,
                            ntsa::MutableBuffer*        bufferArray,
                            bsl::size_t                 bufferCount,
                            const ntsa::ReceiveOptions& options,
                            ntsa::Handle                socket)
{
    return ntsu::SocketUtil::receive(context,
                                     bufferArray,
                                     bufferCount,
                                     options,
                                     socket);
}

ntsa::Error System::receive(ntsa::ReceiveContext*       context,
                            ntsa::MutableBufferArray*   bufferArray,
                            const ntsa::ReceiveOptions& options,
                            ntsa::Handle                socket)
{
    return ntsu::SocketUtil::receive(context, bufferArray, options, socket);
}

ntsa::Error System::receive(ntsa::ReceiveContext*        context,
                            ntsa::MutableBufferPtrArray* bufferArray,
                            const ntsa::ReceiveOptions&  options,
                            ntsa::Handle                 socket)
{
    return ntsu::SocketUtil::receive(context, bufferArray, options, socket);
}

ntsa::Error System::receive(ntsa::ReceiveContext*       context,
                            bsl::string*                string,
                            const ntsa::ReceiveOptions& options,
                            ntsa::Handle                socket)
{
    return ntsu::SocketUtil::receive(context, string, options, socket);
}

ntsa::Error System::receive(ntsa::ReceiveContext*       context,
                            bdlbb::Blob*                blob,
                            const ntsa::ReceiveOptions& options,
                            ntsa::Handle                socket)
{
    return ntsu::SocketUtil::receive(context, blob, options, socket);
}

ntsa::Error System::receive(ntsa::ReceiveContext*       context,
                            bdlbb::BlobBuffer*          blob,
                            const ntsa::ReceiveOptions& options,
                            ntsa::Handle                socket)
{
    return ntsu::SocketUtil::receive(context, blob, options, socket);
}

ntsa::Error System::receive(ntsa::ReceiveContext*       context,
                            ntsa::Data*                 data,
                            const ntsa::ReceiveOptions& options,
                            ntsa::Handle                socket)
{
    return ntsu::SocketUtil::receive(context, data, options, socket);
}

ntsa::Error System::waitUntilReadable(ntsa::Handle socket)
{
    return ntsu::SocketUtil::waitUntilReadable(socket);
}

ntsa::Error System::waitUntilReadable(ntsa::Handle              socket,
                                      const bsls::TimeInterval& timeout)
{
    return ntsu::SocketUtil::waitUntilReadable(socket, timeout);
}

ntsa::Error System::waitUntilWritable(ntsa::Handle socket)
{
    return ntsu::SocketUtil::waitUntilWritable(socket);
}

ntsa::Error System::waitUntilWritable(ntsa::Handle              socket,
                                      const bsls::TimeInterval& timeout)
{
    return ntsu::SocketUtil::waitUntilWritable(socket, timeout);
}

ntsa::Error System::joinMulticastGroup(ntsa::Handle           socket,
                                       const ntsa::IpAddress& interface,
                                       const ntsa::IpAddress& group)
{
    return ntsu::SocketOptionUtil::joinMulticastGroup(socket,
                                                      interface,
                                                      group);
}

ntsa::Error System::leaveMulticastGroup(ntsa::Handle           socket,
                                        const ntsa::IpAddress& interface,
                                        const ntsa::IpAddress& group)
{
    return ntsu::SocketOptionUtil::leaveMulticastGroup(socket,
                                                       interface,
                                                       group);
}

ntsa::Error System::setBlocking(ntsa::Handle socket, bool blocking)
{
    return ntsu::SocketOptionUtil::setBlocking(socket, blocking);
}

ntsa::Error System::setOption(ntsa::Handle              socket,
                              const ntsa::SocketOption& option)
{
    return ntsu::SocketOptionUtil::setOption(socket, option);
}

ntsa::Error System::setMulticastLoopback(ntsa::Handle socket, bool enabled)
{
    return ntsu::SocketOptionUtil::setMulticastLoopback(socket, enabled);
}

ntsa::Error System::setMulticastInterface(ntsa::Handle           socket,
                                          const ntsa::IpAddress& interface)
{
    return ntsu::SocketOptionUtil::setMulticastInterface(socket, interface);
}

ntsa::Error System::setMulticastTimeToLive(ntsa::Handle socket,
                                           bsl::size_t  maxHops)
{
    return ntsu::SocketOptionUtil::setMulticastTimeToLive(socket, maxHops);
}

ntsa::Error System::getOption(ntsa::SocketOption*           option,
                              ntsa::Handle                  socket,
                              ntsa::SocketOptionType::Value type)
{
    return ntsu::SocketOptionUtil::getOption(option, type, socket);
}

ntsa::Error System::getSourceEndpoint(ntsa::Endpoint* result,
                                      ntsa::Handle    socket)
{
    return ntsu::SocketUtil::sourceEndpoint(result, socket);
}

ntsa::Error System::getRemoteEndpoint(ntsa::Endpoint* result,
                                      ntsa::Handle    socket)
{
    return ntsu::SocketUtil::remoteEndpoint(result, socket);
}

ntsa::Error System::getBytesReadable(bsl::size_t* result, ntsa::Handle socket)
{
    return ntsu::SocketOptionUtil::getReceiveBufferAvailable(result, socket);
}

ntsa::Error System::getBytesWritable(bsl::size_t* result, ntsa::Handle socket)
{
    return ntsu::SocketOptionUtil::getSendBufferRemaining(result, socket);
}

ntsa::Error System::getLastError(ntsa::Error* error, ntsa::Handle socket)
{
    return ntsu::SocketOptionUtil::getLastError(error, socket);
}

ntsa::Error System::shutdown(ntsa::Handle              socket,
                             ntsa::ShutdownType::Value direction)
{
    return ntsu::SocketUtil::shutdown(direction, socket);
}

ntsa::Error System::duplicate(ntsa::Handle* result, ntsa::Handle socket)
{
    return ntsu::SocketUtil::duplicate(result, socket);
}

ntsa::Error System::unlink(ntsa::Handle socket)
{
    return ntsu::SocketUtil::unlink(socket);
}

ntsa::Error System::close(ntsa::Handle socket)
{
    return ntsu::SocketUtil::close(socket);
}

void System::discoverAdapterList(bsl::vector<ntsa::Adapter>* result)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntsu::AdapterUtil::discoverAdapterList(result);
}

bool System::discoverAdapter(ntsa::Adapter*             result,
                             ntsa::IpAddressType::Value addressType,
                             bool                       multicast)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntsu::AdapterUtil::discoverAdapter(result, addressType, multicast);
}

ntsa::Error System::scatter(ntsa::MutableBufferArray*   bufferArray,
                            bsl::size_t*                numBytes,
                            bdlbb::Blob*                data,
                            ntsa::Handle                socket,
                            const ntsa::ReceiveOptions& options)
{
    if (bufferArray->numBuffers() > 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::size_t numBytesMax = options.maxBytes();
    if (numBytesMax == 0) {
        numBytesMax = ntsu::SocketUtil::maxBytesPerReceive(socket);
    }

    bsl::size_t numBuffersMax = options.maxBuffers();
    if (numBuffersMax == 0) {
        numBuffersMax = ntsu::SocketUtil::maxBuffersPerReceive();
    }
    else if (numBuffersMax > ntsu::SocketUtil::maxBuffersPerReceive()) {
        numBuffersMax = ntsu::SocketUtil::maxBuffersPerReceive();
    }

    bsl::size_t size     = data->length();
    bsl::size_t capacity = data->totalSize() - size;
    if (capacity == 0) {
        return ntsa::Error::invalid();
    }

    bufferArray->reserve(NTSCFG_DEFAULT_MAX_INPLACE_BUFFERS);

    bsl::size_t numBuffersTotal;
    bsl::size_t numBytesTotal;

    ntsu::BufferUtil::scatter(&numBuffersTotal,
                              &numBytesTotal,
                              bufferArray,
                              numBuffersMax,
                              data,
                              numBytesMax);

    BSLS_ASSERT_OPT(bufferArray->numBuffers() == numBuffersTotal);

    *numBytes = numBytesTotal;

    return ntsa::Error();
}

ntsa::Error System::gather(ntsa::ConstBufferArray*  bufferArray,
                           bsl::size_t*             numBytes,
                           const bdlbb::Blob&       data,
                           ntsa::Handle             socket,
                           const ntsa::SendOptions& options)
{
    if (bufferArray->numBuffers() > 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::size_t numBytesMax = options.maxBytes();
    if (numBytesMax == 0) {
        numBytesMax = ntsu::SocketUtil::maxBytesPerSend(socket);
    }

    bsl::size_t numBuffersMax = options.maxBuffers();
    if (numBuffersMax == 0) {
        numBuffersMax = ntsu::SocketUtil::maxBuffersPerSend();
    }
    else if (numBuffersMax > ntsu::SocketUtil::maxBuffersPerSend()) {
        numBuffersMax = ntsu::SocketUtil::maxBuffersPerSend();
    }

    bufferArray->reserve(NTSCFG_DEFAULT_MAX_INPLACE_BUFFERS);

    bsl::size_t numBuffersTotal;
    bsl::size_t numBytesTotal;

    ntsu::BufferUtil::gather(&numBuffersTotal,
                             &numBytesTotal,
                             bufferArray,
                             numBuffersMax,
                             data,
                             numBytesMax);

    BSLS_ASSERT_OPT(bufferArray->numBuffers() == numBuffersTotal);

    *numBytes = numBytesTotal;

    return ntsa::Error();
}

ntsa::Error System::reportInfo(bsl::vector<ntsa::SocketInfo>* result,
                               const ntsa::SocketInfoFilter&  filter)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntsu::SocketUtil::reportInfo(result, filter);
}

bool System::supportsIpv4()
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntsu::AdapterUtil::supportsIpv4();
}

bool System::supportsIpv4Multicast()
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntsu::AdapterUtil::supportsIpv4Multicast();
}

bool System::supportsIpv6()
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntsu::AdapterUtil::supportsIpv6();
}

bool System::supportsIpv6Multicast()
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntsu::AdapterUtil::supportsIpv6Multicast();
}

bool System::supportsTcp()
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntsu::AdapterUtil::supportsTcp();
}

bool System::supportsUdp()
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntsu::AdapterUtil::supportsUdp();
}

bool System::supportsLocalStream()
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntsu::AdapterUtil::supportsLocalStream();
}

bool System::supportsLocalDatagram()
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntsu::AdapterUtil::supportsLocalDatagram();
}

bool System::supportsTransport(ntsa::Transport::Value transport)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntsu::AdapterUtil::supportsTransport(transport);
}

ntsa::Error System::exit()
{
    BSLMT_ONCE_DO
    {
        int rc = ntscfg::Platform::exit();
        if (rc != 0) {
            return ntsa::Error(ntsa::Error::last());
        }
    }
    return ntsa::Error();
}

SystemGuard::SystemGuard()
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);
}

SystemGuard::SystemGuard(ntscfg::Signal::Value signal)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    error = ntsf::System::ignore(signal);
    BSLS_ASSERT_OPT(!error);
}

SystemGuard::~SystemGuard()
{
    ntsf::System::exit();
}

HandleGuard::HandleGuard(ntsa::Handle handle)
: d_handle(handle)
{
}

HandleGuard::~HandleGuard()
{
    if (d_handle != ntsa::k_INVALID_HANDLE) {
        ntsu::SocketUtil::close(d_handle);
    }
}

ntsa::Handle HandleGuard::release()
{
    ntsa::Handle result = d_handle;
    d_handle            = ntsa::k_INVALID_HANDLE;
    return result;
}

}  // close package namespace
}  // close enterprise namespace
