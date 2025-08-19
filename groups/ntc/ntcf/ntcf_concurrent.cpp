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

#include <ntcf_concurrent.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcf_concurrent_cpp, "$Id$ $CSID$")

#include <ntcs_blobutil.h>

#if NTC_BUILD_WITH_COROUTINES
namespace BloombergLP {
namespace ntcf {

void Concurrent::initialize()
{
}

ntcf::Concurrent::Connect Concurrent::connect(
    const bsl::shared_ptr<ntci::Connector>& connector,
    ntca::ConnectContext*                   context,
    const ntsa::Endpoint&                   endpoint,
    const ntca::ConnectOptions&             options)
{
    return Concurrent::Connect(connector, context, endpoint, options);
}

ntcf::Concurrent::Connect Concurrent::connect(
    const bsl::shared_ptr<ntci::Connector>& connector,
    ntca::ConnectContext*                   context,
    const bsl::string&                      name,
    const ntca::ConnectOptions&             options)
{
    return Concurrent::Connect(connector, context, name, options);
}

ntcf::Concurrent::Send Concurrent::send(
    const bsl::shared_ptr<ntci::Sender>& sender,
    ntca::SendContext*                   context,
    const bsl::shared_ptr<bdlbb::Blob>&  data,
    const ntca::SendOptions&             options)
{
    return Concurrent::Send(sender, context, data, options);
}

ntcf::Concurrent::Receive Concurrent::receive(
    const bsl::shared_ptr<ntci::Receiver>& receiver,
    ntca::ReceiveContext*                  context,
    bsl::shared_ptr<bdlbb::Blob>*          data,
    const ntca::ReceiveOptions&            options)
{
    return Concurrent::Receive(receiver, context, data, options);
}

Concurrent::Close Concurrent::close(
    const bsl::shared_ptr<ntci::Closable>& closable)
{
    return Concurrent::Close(closable);
}

void Concurrent::exit()
{
}

Concurrent::Connect::Connect(const bsl::shared_ptr<ntci::Connector>& connector,
                             ntca::ConnectContext*                   context,
                             const ntsa::Endpoint&                   endpoint,
                             ntca::ConnectOptions                    options)
: d_connector(connector)
, d_context(context)
, d_endpoint(endpoint)
, d_name()
, d_options(options)
, d_error()
{
}

Concurrent::Connect::Connect(const bsl::shared_ptr<ntci::Connector>& connector,
                             ntca::ConnectContext*                   context,
                             const bsl::string&                      name,
                             ntca::ConnectOptions                    options)
: d_connector(connector)
, d_context(context)
, d_endpoint()
, d_name(name)
, d_options(options)
, d_error()
{
}

Concurrent::Connect::Awaiter Concurrent::Connect::operator co_await()
{
    return Concurrent::Connect::Awaiter(this);
}

Concurrent::Connect::Awaiter::Awaiter(Concurrent::Connect* awaitable) noexcept
: d_awaitable(awaitable)
{
}

bool Concurrent::Connect::Awaiter::await_ready() const noexcept
{
    return false;
}

void Concurrent::Connect::Awaiter::await_suspend(
    std::coroutine_handle<void> coroutine) noexcept
{
    ntsa::Error error;

    ntci::ConnectCallback connectCallback =
        d_awaitable->d_connector->createConnectCallback(
            NTCCFG_BIND(&Concurrent::Connect::Awaiter::complete,
                        this,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        coroutine));

    if (!d_awaitable->d_name.empty()) {
        error = d_awaitable->d_connector->connect(d_awaitable->d_name,
                                                  d_awaitable->d_options,
                                                  connectCallback);
    }
    else {
        error = d_awaitable->d_connector->connect(d_awaitable->d_endpoint,
                                                  d_awaitable->d_options,
                                                  connectCallback);
    }

    if (error) {
        d_awaitable->d_error = error;
        coroutine.resume();
    }
}

ntsa::Error Concurrent::Connect::Awaiter::await_resume() const noexcept
{
    return d_awaitable->d_error;
}

void Concurrent::Connect::Awaiter::complete(
    const bsl::shared_ptr<ntci::Connector>& connector,
    const ntca::ConnectEvent&               event,
    std::coroutine_handle<void>             coroutine)
{
    BSLS_ASSERT(connector == d_awaitable->d_connector);

    if (d_awaitable->d_context != 0) {
        *d_awaitable->d_context = event.context();
    }

    if (event.isComplete()) {
        ;
    }
    else if (event.isError()) {
        d_awaitable->d_error = event.context().error();
    }
    else {
        d_awaitable->d_error = ntsa::Error(ntsa::Error::e_INVALID);
    }

    coroutine.resume();
}

Concurrent::Send::Send(const bsl::shared_ptr<ntci::Sender>& sender,
                       ntca::SendContext*                   context,
                       const bsl::shared_ptr<bdlbb::Blob>&  data,
                       ntca::SendOptions                    options)
: d_sender(sender)
, d_context(context)
, d_data(data)
, d_options(options)
, d_error()
{
}

Concurrent::Send::Awaiter Concurrent::Send::operator co_await()
{
    return Concurrent::Send::Awaiter(this);
}

Concurrent::Send::Awaiter::Awaiter(Concurrent::Send* awaitable) noexcept
: d_awaitable(awaitable)
{
}

bool Concurrent::Send::Awaiter::await_ready() const noexcept
{
    return false;
}

void Concurrent::Send::Awaiter::await_suspend(
    std::coroutine_handle<void> coroutine) noexcept
{
    ntsa::Error error;

    ntci::SendCallback sendCallback =
        d_awaitable->d_sender->createSendCallback(
            NTCCFG_BIND(&Concurrent::Send::Awaiter::complete,
                        this,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        coroutine));

    error = d_awaitable->d_sender->send(*d_awaitable->d_data,
                                        d_awaitable->d_options,
                                        sendCallback);

    if (error) {
        d_awaitable->d_error = error;
        coroutine.resume();
    }
}

ntsa::Error Concurrent::Send::Awaiter::await_resume() const noexcept
{
    return d_awaitable->d_error;
}

void Concurrent::Send::Awaiter::complete(
    const bsl::shared_ptr<ntci::Sender>& sender,
    const ntca::SendEvent&               event,
    std::coroutine_handle<void>          coroutine)
{
    BSLS_ASSERT(sender == d_awaitable->d_sender);

    if (d_awaitable->d_context != 0) {
        *d_awaitable->d_context = event.context();
    }

    if (event.isComplete()) {
        ;
    }
    else if (event.isError()) {
        d_awaitable->d_error = event.context().error();
    }
    else {
        d_awaitable->d_error = ntsa::Error(ntsa::Error::e_INVALID);
    }

    coroutine.resume();
}

Concurrent::Receive::Receive(const bsl::shared_ptr<ntci::Receiver>& receiver,
                             ntca::ReceiveContext*                  context,
                             bsl::shared_ptr<bdlbb::Blob>*          data,
                             ntca::ReceiveOptions                   options)
: d_receiver(receiver)
, d_context(context)
, d_data(data)
, d_options(options)
, d_error()
{
}

Concurrent::Receive::Awaiter Concurrent::Receive::operator co_await()
{
    return Concurrent::Receive::Awaiter(this);
}

Concurrent::Receive::Awaiter::Awaiter(Concurrent::Receive* awaitable) noexcept
: d_awaitable(awaitable)
{
}

bool Concurrent::Receive::Awaiter::await_ready() const noexcept
{
    return false;
}

void Concurrent::Receive::Awaiter::await_suspend(
    std::coroutine_handle<void> coroutine) noexcept
{
    ntsa::Error error;

    ntci::ReceiveCallback receiveCallback =
        d_awaitable->d_receiver->createReceiveCallback(
            NTCCFG_BIND(&Concurrent::Receive::Awaiter::complete,
                        this,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        coroutine));

    error = d_awaitable->d_receiver->receive(d_awaitable->d_options,
                                             receiveCallback);

    if (error) {
        d_awaitable->d_error = error;
        coroutine.resume();
    }
}

ntsa::Error Concurrent::Receive::Awaiter::await_resume() const noexcept
{
    return d_awaitable->d_error;
}

void Concurrent::Receive::Awaiter::complete(
    const bsl::shared_ptr<ntci::Receiver>& receiver,
    const bsl::shared_ptr<bdlbb::Blob>&    data,
    const ntca::ReceiveEvent&              event,
    std::coroutine_handle<void>            coroutine)
{
    BSLS_ASSERT(receiver == d_awaitable->d_receiver);

    if (d_awaitable->d_context != 0) {
        *d_awaitable->d_context = event.context();
    }

    if (event.isComplete()) {
        if (d_awaitable->d_data->get() == 0) {
            *d_awaitable->d_data = data;
        }
        else {
            ntcs::BlobUtil::append(d_awaitable->d_data->get(), data);
        }
    }
    else if (event.isError()) {
        d_awaitable->d_error = event.context().error();
    }
    else {
        d_awaitable->d_error = ntsa::Error(ntsa::Error::e_INVALID);
    }

    coroutine.resume();
}

Concurrent::Close::Close(const bsl::shared_ptr<ntci::Closable>& closable)
: d_closable(closable)
{
}

Concurrent::Close::Awaiter Concurrent::Close::operator co_await()
{
    return Concurrent::Close::Awaiter(this);
}

Concurrent::Close::Awaiter::Awaiter(Concurrent::Close* awaitable) noexcept
: d_awaitable(awaitable)
{
}

bool Concurrent::Close::Awaiter::await_ready() const noexcept
{
    return false;
}

void Concurrent::Close::Awaiter::await_suspend(
    std::coroutine_handle<void> coroutine) noexcept
{
    ntci::CloseCallback closeCallback =
        d_awaitable->d_closable->createCloseCallback(
            NTCCFG_BIND(&Concurrent::Close::Awaiter::complete,
                        this,
                        coroutine));

    d_awaitable->d_closable->close(closeCallback);
}

void Concurrent::Close::Awaiter::await_resume() const noexcept
{
    return;
}

void Concurrent::Close::Awaiter::complete(std::coroutine_handle<> coroutine)
{
    coroutine.resume();
}

}  // close namespace ntcf
}  // close namespace BloombergLP
#endif
