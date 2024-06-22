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

#include <ntcd_streamsocket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcd_streamsocket_cpp, "$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcd {

StreamSocket::StreamSocket(bslma::Allocator* basicAllocator)
: d_mutex()
, d_machine_sp(ntcd::Machine::getDefault())
, d_session_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT_OPT(d_machine_sp);
}

StreamSocket::StreamSocket(const bsl::shared_ptr<ntcd::Machine>& machine,
                           bslma::Allocator* basicAllocator)
: d_mutex()
, d_machine_sp(machine)
, d_session_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT_OPT(d_machine_sp);
}

StreamSocket::StreamSocket(const bsl::shared_ptr<ntcd::Machine>& machine,
                           const bsl::shared_ptr<ntcd::Session>& session,
                           bslma::Allocator* basicAllocator)
: d_mutex()
, d_machine_sp(machine)
, d_session_sp(session)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT_OPT(d_machine_sp);
    BSLS_ASSERT_OPT(d_session_sp);
}

StreamSocket::~StreamSocket()
{
}

ntsa::Error StreamSocket::open(ntsa::Transport::Value transport)
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

ntsa::Error StreamSocket::acquire(ntsa::Handle handle)
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

ntsa::Handle StreamSocket::release()
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::k_INVALID_HANDLE;
    }

    ntsa::Handle result = d_session_sp->release();
    d_session_sp.reset();

    return result;
}

ntsa::Error StreamSocket::bind(const ntsa::Endpoint& endpoint,
                               bool                  reuseAddress)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->bind(endpoint, reuseAddress);
}

ntsa::Error StreamSocket::bindAny(ntsa::Transport::Value transport,
                                  bool                   reuseAddress)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->bindAny(transport, reuseAddress);
}

ntsa::Error StreamSocket::connect(const ntsa::Endpoint& endpoint)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->connect(endpoint);
}

ntsa::Error StreamSocket::send(ntsa::SendContext*       context,
                               const bdlbb::Blob&       data,
                               const ntsa::SendOptions& options)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->send(context, data, options);
}

ntsa::Error StreamSocket::send(ntsa::SendContext*       context,
                               const ntsa::Data&        data,
                               const ntsa::SendOptions& options)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->send(context, data, options);
}

ntsa::Error StreamSocket::send(ntsa::SendContext*       context,
                               const ntsa::ConstBuffer* data,
                               bsl::size_t              size,
                               const ntsa::SendOptions& options)
{
    return ntsi::StreamSocket::send(context, data, size, options);
}

ntsa::Error StreamSocket::receive(ntsa::ReceiveContext*       context,
                                  bdlbb::Blob*                data,
                                  const ntsa::ReceiveOptions& options)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->receive(context, data, options);
}

ntsa::Error StreamSocket::receive(ntsa::ReceiveContext*       context,
                                  ntsa::Data*                 data,
                                  const ntsa::ReceiveOptions& options)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->receive(context, data, options);
}

ntsa::Error StreamSocket::shutdown(ntsa::ShutdownType::Value direction)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->shutdown(direction);
}

ntsa::Error StreamSocket::unlink()
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->unlink();
}

ntsa::Error StreamSocket::close()
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Error error = d_session_sp->close();
    d_session_sp.reset();

    return error;
}

ntsa::Error StreamSocket::sourceEndpoint(ntsa::Endpoint* result) const
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->sourceEndpoint(result);
}

ntsa::Error StreamSocket::remoteEndpoint(ntsa::Endpoint* result) const
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->remoteEndpoint(result);
}

ntsa::Handle StreamSocket::handle() const
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::k_INVALID_HANDLE;
    }

    return d_session_sp->handle();
}

// *** Socket Options ***

ntsa::Error StreamSocket::setBlocking(bool blocking)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->setBlocking(blocking);
}

ntsa::Error StreamSocket::setOption(const ntsa::SocketOption& option)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->setOption(option);
}

ntsa::Error StreamSocket::getOption(ntsa::SocketOption*           option,
                                    ntsa::SocketOptionType::Value type)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->getOption(option, type);
}

ntsa::Error StreamSocket::getLastError(ntsa::Error* result)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->getLastError(result);
}

bsl::size_t StreamSocket::maxBuffersPerSend() const
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return 1;
    }

    return d_session_sp->maxBuffersPerSend();
}

bsl::size_t StreamSocket::maxBuffersPerReceive() const
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return 1;
    }

    return d_session_sp->maxBuffersPerReceive();
}

StreamSocketFactory::StreamSocketFactory(const Function&   function,
                                         bslma::Allocator* basicAllocator)
: d_function(bsl::allocator_arg, basicAllocator, function)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

StreamSocketFactory::~StreamSocketFactory()
{
}

bsl::shared_ptr<ntci::StreamSocket> StreamSocketFactory::createStreamSocket(
    const ntca::StreamSocketOptions& options,
    bslma::Allocator*                basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);
    return d_function(options, allocator);
}

}  // close package namespace
}  // close enterprise namespace
