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

#include <ntcd_listenersocket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcd_listenersocket_cpp, "$Id$ $CSID$")

#include <ntcd_streamsocket.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcd {

ListenerSocket::ListenerSocket(bslma::Allocator* basicAllocator)
: d_mutex()
, d_machine_sp(ntcd::Machine::getDefault())
, d_session_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT_OPT(d_machine_sp);
}

ListenerSocket::ListenerSocket(const bsl::shared_ptr<ntcd::Machine>& machine,
                               bslma::Allocator* basicAllocator)
: d_mutex()
, d_machine_sp(machine)
, d_session_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT_OPT(d_machine_sp);
}

ListenerSocket::~ListenerSocket()
{
}

ntsa::Error ListenerSocket::open(ntsa::Transport::Value transport)
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

ntsa::Error ListenerSocket::acquire(ntsa::Handle handle)
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

ntsa::Handle ListenerSocket::release()
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::k_INVALID_HANDLE;
    }

    ntsa::Handle result = d_session_sp->release();
    d_session_sp.reset();

    return result;
}

ntsa::Error ListenerSocket::bind(const ntsa::Endpoint& endpoint,
                                 bool                  reuseAddress)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->bind(endpoint, reuseAddress);
}

ntsa::Error ListenerSocket::bindAny(ntsa::Transport::Value transport,
                                    bool                   reuseAddress)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->bindAny(transport, reuseAddress);
}

ntsa::Error ListenerSocket::listen(bsl::size_t backlog)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->listen(backlog);
}

ntsa::Error ListenerSocket::accept(ntsa::Handle* result)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->accept(result);
}

ntsa::Error ListenerSocket::accept(
    bslma::ManagedPtr<ntsi::StreamSocket>* result,
    bslma::Allocator*                      basicAllocator)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::Session> session;
    ntsa::Error error = d_session_sp->accept(&session, allocator);
    if (error) {
        return error;
    }

    result->load(new (*allocator)
                     ntcd::StreamSocket(d_machine_sp, session, allocator),
                 allocator);

    return ntsa::Error();
}

ntsa::Error ListenerSocket::accept(bsl::shared_ptr<ntsi::StreamSocket>* result,
                                   bslma::Allocator* basicAllocator)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::Session> session;
    ntsa::Error error = d_session_sp->accept(&session, allocator);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntcd::StreamSocket> streamSocket;
    streamSocket.createInplace(allocator, d_machine_sp, session, allocator);

    *result = streamSocket;

    return ntsa::Error();
}

ntsa::Error ListenerSocket::shutdown(ntsa::ShutdownType::Value direction)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->shutdown(direction);
}

ntsa::Error ListenerSocket::unlink()
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->unlink();
}

ntsa::Error ListenerSocket::close()
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Error error = d_session_sp->close();
    d_session_sp.reset();

    return error;
}

ntsa::Error ListenerSocket::sourceEndpoint(ntsa::Endpoint* result) const
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->sourceEndpoint(result);
}

ntsa::Handle ListenerSocket::handle() const
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::k_INVALID_HANDLE;
    }

    return d_session_sp->handle();
}

// *** Socket Options ***

ntsa::Error ListenerSocket::setBlocking(bool blocking)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->setBlocking(blocking);
}

ntsa::Error ListenerSocket::setOption(const ntsa::SocketOption& option)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->setOption(option);
}

ntsa::Error ListenerSocket::getOption(ntsa::SocketOption*           option,
                                      ntsa::SocketOptionType::Value type)
{
    LockGuard lock(&d_mutex);

    if (!d_session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_session_sp->getOption(option, type);
}

ListenerSocketFactory::ListenerSocketFactory(const Function&   function,
                                             bslma::Allocator* basicAllocator)
: d_function(bsl::allocator_arg, basicAllocator, function)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ListenerSocketFactory::~ListenerSocketFactory()
{
}

bsl::shared_ptr<ntci::ListenerSocket> ListenerSocketFactory::
    createListenerSocket(const ntca::ListenerSocketOptions& options,
                         bslma::Allocator*                  basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);
    return d_function(options, allocator);
}

}  // close package namespace
}  // close enterprise namespace
