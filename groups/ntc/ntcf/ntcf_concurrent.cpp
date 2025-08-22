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

ntcf::Concurrent::Execute Concurrent::resume(
    const bsl::shared_ptr<ntci::Executor>& executor)
{
    return Concurrent::Execute(executor);
}

ntcf::Concurrent::Connect Concurrent::connect(
    const bsl::shared_ptr<ntci::Connector>& connector,
    const ntsa::Endpoint&                   endpoint,
    const ntca::ConnectOptions&             options)
{
    return Concurrent::Connect(connector, endpoint, options);
}

ntcf::Concurrent::Connect Concurrent::connect(
    const bsl::shared_ptr<ntci::Connector>& connector,
    const bsl::string&                      name,
    const ntca::ConnectOptions&             options)
{
    return Concurrent::Connect(connector, name, options);
}

ntcf::Concurrent::Accept Concurrent::accept(
    const bsl::shared_ptr<ntci::Acceptor>& acceptor,
    const ntca::AcceptOptions&             options)
{
    return Concurrent::Accept(acceptor, options);
}

ntcf::Concurrent::Send Concurrent::send(
    const bsl::shared_ptr<ntci::Sender>& sender,
    const bsl::shared_ptr<bdlbb::Blob>&  data,
    const ntca::SendOptions&             options)
{
    return Concurrent::Send(sender, data, options);
}

ntcf::Concurrent::Receive Concurrent::receive(
    const bsl::shared_ptr<ntci::Receiver>& receiver,
    const ntca::ReceiveOptions&            options)
{
    return Concurrent::Receive(receiver, options);
}

Concurrent::Close Concurrent::close(
    const bsl::shared_ptr<ntci::Closable>& closable)
{
    return Concurrent::Close(closable);
}

void Concurrent::exit()
{
}

Concurrent::Execute::Execute(const bsl::shared_ptr<ntci::Executor>& executor)
: d_executor(executor)
{
}

Concurrent::Execute::Awaiter Concurrent::Execute::operator co_await()
{
    return Concurrent::Execute::Awaiter(this);
}

Concurrent::Execute::Awaiter::Awaiter(Concurrent::Execute* awaitable) noexcept
: d_awaitable(awaitable)
{
}

bool Concurrent::Execute::Awaiter::await_ready() const noexcept
{
    return false;
}

void Concurrent::Execute::Awaiter::await_suspend(
    bsl::coroutine_handle<void> coroutine) noexcept
{
    d_awaitable->d_executor->execute(
        NTCCFG_BIND(&Concurrent::Execute::Awaiter::complete, coroutine));
}

void Concurrent::Execute::Awaiter::await_resume() const noexcept
{
}

void Concurrent::Execute::Awaiter::complete(
    bsl::coroutine_handle<void> coroutine)
{
    coroutine.resume();
}

Concurrent::Connect::Connect(const bsl::shared_ptr<ntci::Connector>& connector,
                             const ntsa::Endpoint&                   endpoint,
                             ntca::ConnectOptions                    options)
: d_connector(connector)
, d_endpoint(endpoint)
, d_name()
, d_options(options)
, d_result()
{
}

Concurrent::Connect::Connect(const bsl::shared_ptr<ntci::Connector>& connector,
                             const bsl::string&                      name,
                             ntca::ConnectOptions                    options)
: d_connector(connector)
, d_endpoint()
, d_name(name)
, d_options(options)
, d_result()
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
    bsl::coroutine_handle<void> coroutine) noexcept
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
        ntca::ConnectContext context;
        context.setError(error);

        ntca::ConnectEvent event;
        event.setType(ntca::ConnectEventType::e_ERROR);
        event.setContext(context);

        d_awaitable->d_result.setConnector(d_awaitable->d_connector);
        d_awaitable->d_result.setEvent(event);

        coroutine.resume();
    }
}

ntci::ConnectResult Concurrent::Connect::Awaiter::await_resume() const noexcept
{
    return d_awaitable->d_result;
}

void Concurrent::Connect::Awaiter::complete(
    const bsl::shared_ptr<ntci::Connector>& connector,
    const ntca::ConnectEvent&               event,
    bsl::coroutine_handle<void>             coroutine)
{
    BSLS_ASSERT(connector == d_awaitable->d_connector);

    d_awaitable->d_result.setConnector(connector);
    d_awaitable->d_result.setEvent(event);

    coroutine.resume();
}

Concurrent::Accept::Accept(const bsl::shared_ptr<ntci::Acceptor>& acceptor,
                           ntca::AcceptOptions                    options)
: d_acceptor(acceptor)
, d_options(options)
, d_result()
{
}

Concurrent::Accept::Awaiter Concurrent::Accept::operator co_await()
{
    return Concurrent::Accept::Awaiter(this);
}

Concurrent::Accept::Awaiter::Awaiter(Concurrent::Accept* awaitable) noexcept
: d_awaitable(awaitable)
{
}

bool Concurrent::Accept::Awaiter::await_ready() const noexcept
{
    return false;
}

void Concurrent::Accept::Awaiter::await_suspend(
    bsl::coroutine_handle<void> coroutine) noexcept
{
    ntsa::Error error;

    ntci::AcceptCallback acceptCallback =
        d_awaitable->d_acceptor->createAcceptCallback(
            NTCCFG_BIND(&Concurrent::Accept::Awaiter::complete,
                        this,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        coroutine));

    error = d_awaitable->d_acceptor->accept(d_awaitable->d_options,
                                            acceptCallback);

    if (error) {
        ntca::AcceptContext context;
        context.setError(error);

        ntca::AcceptEvent event;
        event.setType(ntca::AcceptEventType::e_ERROR);
        event.setContext(context);

        d_awaitable->d_result.setAcceptor(d_awaitable->d_acceptor);
        d_awaitable->d_result.setEvent(event);

        coroutine.resume();
    }
}

ntci::AcceptResult Concurrent::Accept::Awaiter::await_resume() const noexcept
{
    return d_awaitable->d_result;
}

void Concurrent::Accept::Awaiter::complete(
    const bsl::shared_ptr<ntci::Acceptor>&     acceptor,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::AcceptEvent&                   event,
    bsl::coroutine_handle<void>                coroutine)
{
    BSLS_ASSERT(acceptor == d_awaitable->d_acceptor);

    d_awaitable->d_result.setAcceptor(acceptor);
    d_awaitable->d_result.setStreamSocket(streamSocket);
    d_awaitable->d_result.setEvent(event);

    coroutine.resume();
}

Concurrent::Send::Send(const bsl::shared_ptr<ntci::Sender>& sender,
                       const bsl::shared_ptr<bdlbb::Blob>&  data,
                       ntca::SendOptions                    options)
: d_sender(sender)
, d_data()
, d_options(options)
, d_result()
{
    d_data.makeSharedBlob(data);
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
    bsl::coroutine_handle<void> coroutine) noexcept
{
    ntsa::Error error;

    ntci::SendCallback sendCallback =
        d_awaitable->d_sender->createSendCallback(
            NTCCFG_BIND(&Concurrent::Send::Awaiter::complete,
                        this,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        coroutine));

    error = d_awaitable->d_sender->send(d_awaitable->d_data,
                                        d_awaitable->d_options,
                                        sendCallback);

    if (error) {
        ntca::SendContext context;
        context.setError(error);

        ntca::SendEvent event;
        event.setType(ntca::SendEventType::e_ERROR);
        event.setContext(context);

        d_awaitable->d_result.setSender(d_awaitable->d_sender);
        d_awaitable->d_result.setEvent(event);

        coroutine.resume();
    }
}

ntci::SendResult Concurrent::Send::Awaiter::await_resume() const noexcept
{
    return d_awaitable->d_result;
}

void Concurrent::Send::Awaiter::complete(
    const bsl::shared_ptr<ntci::Sender>& sender,
    const ntca::SendEvent&               event,
    bsl::coroutine_handle<void>          coroutine)
{
    BSLS_ASSERT(sender == d_awaitable->d_sender);

    d_awaitable->d_result.setSender(sender);
    d_awaitable->d_result.setEvent(event);

    coroutine.resume();
}

Concurrent::Receive::Receive(const bsl::shared_ptr<ntci::Receiver>& receiver,
                             ntca::ReceiveOptions                   options)
: d_receiver(receiver)
, d_options(options)
, d_result()
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
    bsl::coroutine_handle<void> coroutine) noexcept
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
        ntca::ReceiveContext context;
        context.setError(error);

        ntca::ReceiveEvent event;
        event.setType(ntca::ReceiveEventType::e_ERROR);
        event.setContext(context);

        d_awaitable->d_result.setReceiver(d_awaitable->d_receiver);
        d_awaitable->d_result.setEvent(event);

        coroutine.resume();
    }
}

ntci::ReceiveResult Concurrent::Receive::Awaiter::await_resume() const noexcept
{
    return d_awaitable->d_result;
}

void Concurrent::Receive::Awaiter::complete(
    const bsl::shared_ptr<ntci::Receiver>& receiver,
    const bsl::shared_ptr<bdlbb::Blob>&    data,
    const ntca::ReceiveEvent&              event,
    bsl::coroutine_handle<void>            coroutine)
{
    BSLS_ASSERT(receiver == d_awaitable->d_receiver);

    d_awaitable->d_result.setReceiver(receiver);
    d_awaitable->d_result.setData(data);
    d_awaitable->d_result.setEvent(event);

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
    bsl::coroutine_handle<void> coroutine) noexcept
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

void Concurrent::Close::Awaiter::complete(
    bsl::coroutine_handle<void> coroutine)
{
    coroutine.resume();
}

}  // close namespace ntcf
}  // close namespace BloombergLP
#endif
