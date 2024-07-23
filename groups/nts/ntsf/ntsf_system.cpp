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
#include <ntso_devpoll.h>
#include <ntso_epoll.h>
#include <ntso_eventport.h>
#include <ntso_kqueue.h>
#include <ntso_poll.h>
#include <ntso_pollset.h>
#include <ntso_select.h>
#include <ntsu_adapterutil.h>
#include <ntsu_socketoptionutil.h>
#include <ntsu_socketutil.h>

#include <bdlb_string.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_newdeleteallocator.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_once.h>
#include <bsls_assert.h>
#include <bsl_fstream.h>

namespace BloombergLP {
namespace ntsf {

namespace {

bslma::Allocator*    s_globalAllocator_p;
ntscfg::Mutex*       s_globalMutex_p;
ntsi::Resolver*      s_globalResolver_p;
bslma::SharedPtrRep* s_globalResolverRep_p;

}  // close unnamed namespace

ntsa::Error System::initialize()
{
    BSLMT_ONCE_DO
    {
        int rc = ntscfg::Platform::initialize();
        if (rc != 0) {
            return ntsa::Error(ntsa::Error::last());
        }

        // We use a new delete allocator instead of the global allocator here
        // because we want to prevent a visible "memory leak" if the global
        // allocator has been replaced in main.  This is because the memory
        // allocated by the plugins won't be freed until the application exits.

        s_globalAllocator_p = &bslma::NewDeleteAllocator::singleton();

        s_globalMutex_p = new (*s_globalAllocator_p) ntscfg::Mutex();

        bsl::atexit(&System::exit);
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

bsl::shared_ptr<ntsi::Reactor> System::createReactor(
    bslma::Allocator* basicAllocator)
{
    ntsa::ReactorConfig reactorConfig;
    return System::createReactor(reactorConfig, basicAllocator);
}

bsl::shared_ptr<ntsi::Reactor> System::createReactor(
    const ntsa::ReactorConfig& configuration,
    bslma::Allocator*          basicAllocator)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntsa::ReactorConfig effectiveConfig = configuration;
    if (effectiveConfig.driverName().isNull() ||
        effectiveConfig.driverName().value().empty())
    {
#if defined(BSLS_PLATFORM_OS_AIX)
#if NTSO_POLLSET_ENABLED
        effectiveConfig.setDriverName("pollset");
#elif NTSO_POLL_ENABLED
        effectiveConfig.setDriverName("poll");
#elif NTSO_SELECT_ENABLED
        effectiveConfig.setDriverName("select");
#endif
#elif defined(BSLS_PLATFORM_OS_DARWIN)
#if NTSO_KQUEUE_ENABLED
        effectiveConfig.setDriverName("kqueue");
#elif NTSO_POLL_ENABLED
        effectiveConfig.setDriverName("poll");
#elif NTSO_SELECT_ENABLED
        effectiveConfig.setDriverName("select");
#endif
#elif defined(BSLS_PLATFORM_OS_LINUX)
#if NTSO_EPOLL_ENABLED
        effectiveConfig.setDriverName("epoll");
#elif NTSO_POLL_ENABLED
        effectiveConfig.setDriverName("poll");
#elif NTSO_SELECT_ENABLED
        effectiveConfig.setDriverName("select");
#endif
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
#if NTSO_EVENTPORT_ENABLED
        effectiveConfig.setDriverName("eventport");
#elif NTSO_DEVPOLL_ENABLED
        effectiveConfig.setDriverName("devpoll");
#elif NTSO_POLL_ENABLED
        effectiveConfig.setDriverName("poll");
#elif NTSO_SELECT_ENABLED
        effectiveConfig.setDriverName("select");
#endif
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
#if NTSO_POLL_ENABLED
        effectiveConfig.setDriverName("poll");
#elif NTSO_SELECT_ENABLED
        effectiveConfig.setDriverName("select");
#endif
#else
#error Not implemented
#endif
    }

    if (effectiveConfig.autoAttach().isNull()) {
        effectiveConfig.setAutoAttach(false);
    }

    if (effectiveConfig.autoDetach().isNull()) {
        effectiveConfig.setAutoDetach(false);
    }

    bsl::shared_ptr<ntsi::Reactor> reactor;
    if (effectiveConfig.driverName().isNull() ||
        effectiveConfig.driverName().value().empty())
    {
        reactor.createInplace(allocator);
    }
#if NTSO_EPOLL_ENABLED
    else if (bdlb::String::areEqualCaseless(
                 effectiveConfig.driverName().value(),
                 "epoll"))
    {
        reactor = ntso::EpollUtil::createReactor(effectiveConfig, allocator);
    }
#endif
#if NTSO_KQUEUE_ENABLED
    else if (bdlb::String::areEqualCaseless(
                 effectiveConfig.driverName().value(),
                 "kqueue"))
    {
        reactor = ntso::KqueueUtil::createReactor(effectiveConfig, allocator);
    }
#endif
#if NTSO_EVENTPORT_ENABLED
    else if (bdlb::String::areEqualCaseless(
                 effectiveConfig.driverName().value(),
                 "eventport"))
    {
        reactor =
            ntso::EventPortUtil::createReactor(effectiveConfig, allocator);
    }
#endif
#if NTSO_DEVPOLL_ENABLED
    else if (bdlb::String::areEqualCaseless(
                 effectiveConfig.driverName().value(),
                 "devpoll"))
    {
        reactor = ntso::DevpollUtil::createReactor(effectiveConfig, allocator);
    }
#endif
#if NTSO_POLLSET_ENABLED
    else if (bdlb::String::areEqualCaseless(
                 effectiveConfig.driverName().value(),
                 "pollset"))
    {
        reactor = ntso::PollsetUtil::createReactor(effectiveConfig, allocator);
    }
#endif
#if NTSO_POLL_ENABLED
    else if (bdlb::String::areEqualCaseless(
                 effectiveConfig.driverName().value(),
                 "poll"))
    {
        reactor = ntso::PollUtil::createReactor(effectiveConfig, allocator);
    }
#endif
#if NTSO_SELECT_ENABLED
    else if (bdlb::String::areEqualCaseless(
                 effectiveConfig.driverName().value(),
                 "select"))
    {
        reactor = ntso::SelectUtil::createReactor(effectiveConfig, allocator);
    }
#endif
    else {
        reactor.createInplace(allocator);
    }

    BSLS_ASSERT_OPT(reactor);
    return reactor;
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

bsl::shared_ptr<ntsi::Resolver> System::createResolver(
    const ntsa::ResolverConfig& configuration,
    bslma::Allocator*           basicAllocator)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntsb::Resolver> resolver;
    resolver.createInplace(allocator, configuration, allocator);

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

ntsa::Error System::receive(bsl::size_t* numBytesReceived,
                            void*        data,
                            bsl::size_t  capacity,
                            ntsa::Handle socket)
{
    return ntsu::SocketUtil::receive(numBytesReceived, data, capacity, socket);
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

ntsa::Error System::joinMulticastGroupSource(
    ntsa::Handle           socket,
    const ntsa::IpAddress& interface,
    const ntsa::IpAddress& group,
    const ntsa::IpAddress& source)
{
    return ntsu::SocketOptionUtil::joinMulticastGroupSource(
        socket, interface, group, source);
}

ntsa::Error System::leaveMulticastGroupSource(
    ntsa::Handle           socket,
    const ntsa::IpAddress& interface,
    const ntsa::IpAddress& group,
    const ntsa::IpAddress& source)
{
    return ntsu::SocketOptionUtil::leaveMulticastGroupSource(
        socket, interface, group, source);
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

ntsa::Error System::getBlocking(ntsa::Handle socket, bool* blocking)
{
    return ntsu::SocketOptionUtil::getBlocking(socket, blocking);
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

ntsa::Error System::setIpAddress(
    const bslstl::StringRef&            domainName,
    const bsl::vector<ntsa::IpAddress>& ipAddressList)
{
    bsl::shared_ptr<ntsi::Resolver> resolver;
    System::getDefault(&resolver);

    return resolver->setIpAddress(domainName, ipAddressList);
}

ntsa::Error System::addIpAddress(
    const bslstl::StringRef&            domainName,
    const bsl::vector<ntsa::IpAddress>& ipAddressList)
{
    bsl::shared_ptr<ntsi::Resolver> resolver;
    System::getDefault(&resolver);

    return resolver->addIpAddress(domainName, ipAddressList);
}

ntsa::Error System::addIpAddress(const bslstl::StringRef& domainName,
                                 const ntsa::IpAddress&   ipAddress)
{
    bsl::shared_ptr<ntsi::Resolver> resolver;
    System::getDefault(&resolver);

    return resolver->addIpAddress(domainName, ipAddress);
}

ntsa::Error System::setPort(const bslstl::StringRef&       serviceName,
                            const bsl::vector<ntsa::Port>& portList,
                            ntsa::Transport::Value         transport)
{
    bsl::shared_ptr<ntsi::Resolver> resolver;
    System::getDefault(&resolver);

    return resolver->setPort(serviceName, portList, transport);
}

ntsa::Error System::addPort(const bslstl::StringRef&       serviceName,
                            const bsl::vector<ntsa::Port>& portList,
                            ntsa::Transport::Value         transport)
{
    bsl::shared_ptr<ntsi::Resolver> resolver;
    System::getDefault(&resolver);

    return resolver->addPort(serviceName, portList, transport);
}

ntsa::Error System::addPort(const bslstl::StringRef& serviceName,
                            ntsa::Port               port,
                            ntsa::Transport::Value   transport)
{
    bsl::shared_ptr<ntsi::Resolver> resolver;
    System::getDefault(&resolver);

    return resolver->addPort(serviceName, port, transport);
}

ntsa::Error System::setLocalIpAddress(
    const bsl::vector<ntsa::IpAddress>& ipAddressList)
{
    bsl::shared_ptr<ntsi::Resolver> resolver;
    System::getDefault(&resolver);

    return resolver->setLocalIpAddress(ipAddressList);
}

ntsa::Error System::setHostname(const bsl::string& name)
{
    bsl::shared_ptr<ntsi::Resolver> resolver;
    System::getDefault(&resolver);

    return resolver->setHostname(name);
}

ntsa::Error System::setHostnameFullyQualified(const bsl::string& name)
{
    bsl::shared_ptr<ntsi::Resolver> resolver;
    System::getDefault(&resolver);

    return resolver->setHostnameFullyQualified(name);
}

ntsa::Error System::getIpAddress(bsl::vector<ntsa::IpAddress>* result,
                                 const bslstl::StringRef&      domainName,
                                 const ntsa::IpAddressOptions& options)
{
    bsl::shared_ptr<ntsi::Resolver> resolver;
    System::getDefault(&resolver);

    return resolver->getIpAddress(result, domainName, options);
}

ntsa::Error System::getDomainName(bsl::string*           result,
                                  const ntsa::IpAddress& ipAddress)
{
    bsl::shared_ptr<ntsi::Resolver> resolver;
    System::getDefault(&resolver);

    return resolver->getDomainName(result, ipAddress);
}

ntsa::Error System::getPort(bsl::vector<ntsa::Port>* result,
                            const bslstl::StringRef& serviceName,
                            const ntsa::PortOptions& options)
{
    bsl::shared_ptr<ntsi::Resolver> resolver;
    System::getDefault(&resolver);

    return resolver->getPort(result, serviceName, options);
}

ntsa::Error System::getServiceName(bsl::string*           result,
                                   ntsa::Port             port,
                                   ntsa::Transport::Value transport)
{
    bsl::shared_ptr<ntsi::Resolver> resolver;
    System::getDefault(&resolver);

    return resolver->getServiceName(result, port, transport);
}

ntsa::Error System::getEndpoint(ntsa::Endpoint*              result,
                                const bslstl::StringRef&     text,
                                const ntsa::EndpointOptions& options)
{
    bsl::shared_ptr<ntsi::Resolver> resolver;
    System::getDefault(&resolver);

    return resolver->getEndpoint(result, text, options);
}

ntsa::Error System::getLocalIpAddress(bsl::vector<ntsa::IpAddress>* result,
                                      const ntsa::IpAddressOptions& options)
{
    bsl::shared_ptr<ntsi::Resolver> resolver;
    System::getDefault(&resolver);

    return resolver->getLocalIpAddress(result, options);
}

ntsa::Error System::getHostname(bsl::string* result)
{
    bsl::shared_ptr<ntsi::Resolver> resolver;
    System::getDefault(&resolver);

    return resolver->getHostname(result);
}

ntsa::Error System::getHostnameFullyQualified(bsl::string* result)
{
    bsl::shared_ptr<ntsi::Resolver> resolver;
    System::getDefault(&resolver);

    return resolver->getHostnameFullyQualified(result);
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

    ntsa::MutableBufferArray::scatter(&numBuffersTotal,
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

    ntsa::ConstBufferArray::gather(&numBuffersTotal,
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

void System::setDefault(const bsl::shared_ptr<ntsi::Resolver>& resolver)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntsi::Resolver> result;

    ntscfg::LockGuard lock(s_globalMutex_p);

    if (s_globalResolver_p != 0) {
        BSLS_ASSERT_OPT(s_globalResolverRep_p);

        s_globalResolverRep_p->releaseRef();

        s_globalResolver_p    = 0;
        s_globalResolverRep_p = 0;
    }

    BSLS_ASSERT_OPT(s_globalResolver_p == 0);
    BSLS_ASSERT_OPT(s_globalResolverRep_p == 0);

    bsl::shared_ptr<ntsi::Resolver>                  temp = resolver;
    bsl::pair<ntsi::Resolver*, bslma::SharedPtrRep*> pair = temp.release();

    temp.reset();

    s_globalResolver_p    = pair.first;
    s_globalResolverRep_p = pair.second;

    BSLS_ASSERT_OPT(s_globalResolver_p);
    BSLS_ASSERT_OPT(s_globalResolverRep_p);

    s_globalResolverRep_p->acquireRef();
}

void System::getDefault(bsl::shared_ptr<ntsi::Resolver>* result)
{
    ntsa::Error error;

    error = ntsf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntscfg::LockGuard lock(s_globalMutex_p);

    if (NTSCFG_UNLIKELY(s_globalResolver_p == 0)) {
        BSLS_ASSERT_OPT(s_globalAllocator_p);

        bsl::shared_ptr<ntsb::Resolver> resolver;
        resolver.createInplace(s_globalAllocator_p, s_globalAllocator_p);

        bsl::pair<ntsb::Resolver*, bslma::SharedPtrRep*> pair =
            resolver.release();

        resolver.reset();

        s_globalResolver_p    = pair.first;
        s_globalResolverRep_p = pair.second;

        BSLS_ASSERT_OPT(s_globalResolverRep_p->numReferences() == 1);
    }

    BSLS_ASSERT_OPT(s_globalResolver_p);
    BSLS_ASSERT_OPT(s_globalResolverRep_p);

    s_globalResolverRep_p->acquireRef();

    *result = bsl::shared_ptr<ntsi::Resolver>(s_globalResolver_p,
                                              s_globalResolverRep_p);
    BSLS_ASSERT_OPT(*result);
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

ntsa::Error System::loadTcpCongestionControlAlgorithmSupport(
    bsl::vector<bsl::string>* result)
{
#if defined(BSLS_PLATFORM_OS_LINUX)
    enum { e_ROOT_EUID = 0 };
    const uid_t id = geteuid();

    const char* path =
        (e_ROOT_EUID == id)
            ? "/proc/sys/net/ipv4/tcp_available_congestion_control"
            : "/proc/sys/net/ipv4/tcp_allowed_congestion_control";

    bsl::ifstream file(path);
    if (!file.is_open()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }
    bsl::string line;
    bsl::getline(file, line);
    if (line.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }
    bsl::stringstream ssline(line);
    result->clear();

    // it is guaranteed that algorithm names are in one line separated
    // by spaces, so `while` below is enough
    bsl::string algorithmName;
    while (ssline >> algorithmName) {
        result->push_back(
            BloombergLP::bslmf::MovableRef<bsl::string>(algorithmName));
    }
    return ntsa::Error();
#else
    result->clear();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
#endif
}

bool System::testTcpCongestionControlAlgorithmSupport(
    const bsl::string& algorithmName)
{
    bsl::vector<bsl::string> supportedAlgorithms;
    const ntsa::Error        error =
        loadTcpCongestionControlAlgorithmSupport(&supportedAlgorithms);

    if (error) {
        return false;
    }

    const bsl::vector<bsl::string>::const_iterator it =
        bsl::find(supportedAlgorithms.cbegin(),
                  supportedAlgorithms.cend(),
                  algorithmName);

    return (it != supportedAlgorithms.cend());
}

void System::exit()
{
    BSLMT_ONCE_DO
    {
        if (s_globalResolver_p != 0) {
            BSLS_ASSERT_OPT(s_globalResolverRep_p);
            s_globalResolverRep_p->releaseRef();
            s_globalResolver_p    = 0;
            s_globalResolverRep_p = 0;
        }

        if (s_globalMutex_p != 0) {
            BSLS_ASSERT_OPT(s_globalAllocator_p);
            s_globalAllocator_p->deleteObject(s_globalMutex_p);
            s_globalMutex_p = 0;
        }

        s_globalAllocator_p = 0;

        int rc = ntscfg::Platform::exit();
        BSLS_ASSERT_OPT(rc == 0);
    }
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
