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

#ifndef INCLUDED_NTCI_CONCURRENT
#define INCLUDED_NTCI_CONCURRENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_acceptoptions.h>
#include <ntca_bindcontext.h>
#include <ntca_bindevent.h>
#include <ntca_bindoptions.h>
#include <ntca_connectcontext.h>
#include <ntca_connectevent.h>
#include <ntca_connectoptions.h>
#include <ntca_receivecontext.h>
#include <ntca_receiveevent.h>
#include <ntca_receiveoptions.h>
#include <ntca_sendevent.h>
#include <ntca_sendoptions.h>
#include <ntccfg_config.h>
#include <ntccfg_platform.h>
#include <ntci_acceptor.h>
#include <ntci_acceptresult.h>
#include <ntci_bindable.h>
#include <ntci_bindresult.h>
#include <ntci_closable.h>
#include <ntci_connector.h>
#include <ntci_connectresult.h>
#include <ntci_datagramsocket.h>
#include <ntci_executor.h>
#include <ntci_listenersocket.h>
#include <ntci_receiver.h>
#include <ntci_receiveresult.h>
#include <ntci_sender.h>
#include <ntci_sendresult.h>
#include <ntci_streamsocket.h>
#include <ntsa_coroutine.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntscfg_config.h>
#include <ntscfg_platform.h>

#if NTC_BUILD_WITH_COROUTINES

namespace BloombergLP {
namespace ntci {

/// Provide an interface for concurrent operations.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Concurrent
{
  public:
    /// Provide an awaitable for an execute operation, which when awaited,
    /// resumes the awaiting coroutine on an I/O thread.
    class Execute;

    // Provide an awaitable for a bind operation, which when awaited,
    /// returns the 'ntci::BindResult' that is the asynchronous result of
    /// the bind operation.
    class Bind;

    /// Provide an awaitable for a connect operation, which when awaited,
    /// returns the 'ntci::ConnectResult' that is the asynchronous result of
    /// the connect operation.
    class Connect;

    /// Provide an awaitable for a accept operation, which when awaited,
    /// returns the 'ntci::AcceptResult' that is the asynchronous result of
    /// the accept operation.
    class Accept;

    /// Provide an awaitable for a send operation, which when awaited,
    /// returns the 'ntci::SendResult' that is the asynchronous result of
    /// the send operation.
    class Send;

    /// Provide an awaitable for a receive operation, which when awaited,
    /// returns the 'ntci::ReceiveResult' that is the asynchronous result of
    /// the receive operation.
    class Receive;

    /// Provide an awaitable for a close operation, which when awaited, returns
    /// void and indicates the closable object has been asynchronously closed.
    class Close;

    /// Resume the current coroutine on the specified 'executor'. Return an
    /// awaitable, which when awaited resumes the awaiting coroutine on the
    /// 'executor' thread.
    static ntci::Concurrent::Execute resume(
        const bsl::shared_ptr<ntci::Executor>& executor);

    /// Bind the specified 'bindable' object to the specified 'endpoint'
    /// according to the specified 'options'. Return an awaitable, which when
    /// awaited, returns the 'ntci::BindResult' that is the asynchronous result
    /// of this operation.
    static ntci::Concurrent::Bind bind(
        const bsl::shared_ptr<ntci::Bindable>& bindable,
        const ntsa::Endpoint&                  endpoint,
        const ntca::BindOptions&               options);

    /// Bind the specified 'bindable' object to the resolution of the specified
    /// 'name' according to the specified 'options'. Return an awaitable, which
    /// when awaited, returns the 'ntci::BindResult' that is the asynchronous
    /// result of this operation.
    static ntci::Concurrent::Bind bind(
        const bsl::shared_ptr<ntci::Bindable>& bindable,
        const bsl::string&                     name,
        const ntca::BindOptions&               options);

    /// Connect the specified 'connector' to the specified 'endpoint' according
    /// to the specified 'options'. Return an awaitable, which when awaited,
    /// returns the 'ntci::ConnectResult' that is the asynchronous result of
    /// this operation.
    static ntci::Concurrent::Connect connect(
        const bsl::shared_ptr<ntci::Connector>& connector,
        const ntsa::Endpoint&                   endpoint,
        const ntca::ConnectOptions&             options);

    /// Connect the specified 'connector' to the resolution of the specified
    /// 'name' according to the specified 'options'. Return an awaitable, which
    /// when awaited, returns the 'ntci::ConnectResult' that is the
    /// asynchronous result of this operation.
    static ntci::Concurrent::Connect connect(
        const bsl::shared_ptr<ntci::Connector>& connector,
        const bsl::string&                      name,
        const ntca::ConnectOptions&             options);

    /// Dequeue a connection from the backlog of the specified 'acceptor'
    /// according to the specified 'options'. Return an awaitable, which when
    /// awaited, returns the 'ntci::AcceptResult' that is the asynchronous
    /// result of this operation.
    static ntci::Concurrent::Accept accept(
        const bsl::shared_ptr<ntci::Acceptor>& acceptor,
        const ntca::AcceptOptions&             options);

    /// Enqueue the specified 'data' for transmission by the specified 'sender'
    /// according to the specified 'options'. Return an awaitable, which when
    /// awaited, returns the 'ntci::SendResult' that is the asynchronous result
    /// of this operation.
    static ntci::Concurrent::Send send(
        const bsl::shared_ptr<ntci::Sender>& sender,
        const bsl::shared_ptr<bdlbb::Blob>&  data,
        const ntca::SendOptions&             options);

    /// Dequeue received data from the specified 'receiver' according to the
    /// specified 'options'. Return an awaitable, which when awaited, returns
    /// the 'ntci::ReceiveResult' that is the asynchronous result of this
    /// operation.
    static ntci::Concurrent::Receive receive(
        const bsl::shared_ptr<ntci::Receiver>& receiver,
        const ntca::ReceiveOptions&            options);

    /// Close the specified 'closable' object. Return an awaitable, which when
    /// awaited, indicates the 'closable' object has been asyncrhonously
    /// closed.
    static ntci::Concurrent::Close close(
        const bsl::shared_ptr<ntci::Closable>& closable);
};

/// Provide an awaitable for an execute operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Concurrent::Execute
{
  public:
    /// Provide an awaiter for an execute operation.
    class Awaiter;

    /// Create a new awaitable that, when awaited, resumes the awaiting
    /// coroutine on one of the threads managed by the specified 'executor'.
    explicit Execute(const bsl::shared_ptr<ntci::Executor>& executor);

    /// Await the completion of the operation. Return the awaiter.
    Awaiter operator co_await();

  private:
    /// This class is not copy-constructable.
    Execute(const Execute&) = delete;

    /// This class is not move-constructable.
    Execute(Execute&&) = delete;

    /// This class is not copy-assignable.
    Execute& operator=(const Execute&) = delete;

    /// This class is not move-assignable.
    Execute& operator=(Execute&&) = delete;

  private:
    /// Allow the associated awaiter to access this class's private data.
    friend class Awaiter;

    /// The coroutine.
    bsl::shared_ptr<ntci::Executor> d_executor;
};

/// Provide an awaiter for an execute operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Concurrent::Execute::Awaiter
{
  public:
    /// Create a new awaiter that is the result of 'co_await'-ing the specified
    /// 'awaitable'.
    explicit Awaiter(Concurrent::Execute* awaitable) noexcept;

    /// Return false to suspend the awaiting coroutine.
    bool await_ready() const noexcept;

    /// Initiate the asynchronous operation and resume the specified
    /// 'coroutine' when the operation asynchronously completes or fails.
    void await_suspend(bsl::coroutine_handle<> coroutine) noexcept;

    /// Return the asynchronous result.
    void await_resume() const noexcept;

  private:
    /// This class is not copy-constructable.
    Awaiter(const Awaiter&) = delete;

    /// This class is not move-constructable.
    Awaiter(Awaiter&&) = delete;

    /// This class is not copy-assignable.
    Awaiter& operator=(const Awaiter&) = delete;

    /// This class is not move-assignable.
    Awaiter& operator=(Awaiter&&) = delete;

  private:
    /// Resume the specified 'coroutine'.
    static void complete(bsl::coroutine_handle<void> coroutine);

    /// The awaitable.
    Concurrent::Execute* d_awaitable;
};

/// Provide an awaitable for a bind operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Concurrent::Bind
{
  public:
    /// Provide an awaiter for a bind operation.
    class Awaiter;

    /// Create a new awaitable that, when awaited, binds the 'bindable' object
    /// to the specified 'endpoint' according to the specified 'options'.
    explicit Bind(const bsl::shared_ptr<ntci::Bindable>& bindable,
                  const ntsa::Endpoint&                  endpoint,
                  ntca::BindOptions                      options);

    /// Create a new awaitable that, when awaited, binds the 'bindable' object
    /// to the resolution of the specified 'name' according to the specified
    /// 'options'.
    explicit Bind(const bsl::shared_ptr<ntci::Bindable>& bindable,
                  const bsl::string&                     name,
                  ntca::BindOptions                      options);

    /// Await the completion of the operation. Return the awaiter.
    Awaiter operator co_await();

  private:
    /// This class is not copy-constructable.
    Bind(const Bind&) = delete;

    /// This class is not move-constructable.
    Bind(Bind&&) = delete;

    /// This class is not copy-assignable.
    Bind& operator=(const Bind&) = delete;

    /// This class is not move-assignable.
    Bind& operator=(Bind&&) = delete;

  private:
    /// Allow the associated awaiter to access this class's private data.
    friend class Awaiter;

    /// The bindable.
    bsl::shared_ptr<ntci::Bindable> d_bindable;

    /// The input endpoint.
    ntsa::Endpoint d_endpoint;

    /// The input name.
    bsl::string d_name;

    /// The input options.
    ntca::BindOptions d_options;

    /// The output result.
    ntci::BindResult d_result;
};

/// Provide an awaiter for a bind operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Concurrent::Bind::Awaiter
{
  public:
    /// Create a new awaiter that is the result of 'co_await'-ing the specified
    /// 'awaitable'.
    explicit Awaiter(Concurrent::Bind* awaitable) noexcept;

    /// Return false to suspend the awaiting coroutine.
    bool await_ready() const noexcept;

    /// Initiate the asynchronous operation and resume the specified
    /// 'coroutine' when the operation asynchronously completes or fails.
    void await_suspend(bsl::coroutine_handle<> coroutine) noexcept;

    /// Return the asynchronous result.
    ntci::BindResult await_resume() const noexcept;

  private:
    /// This class is not copy-constructable.
    Awaiter(const Awaiter&) = delete;

    /// This class is not move-constructable.
    Awaiter(Awaiter&&) = delete;

    /// This class is not copy-assignable.
    Awaiter& operator=(const Awaiter&) = delete;

    /// This class is not move-assignable.
    Awaiter& operator=(Awaiter&&) = delete;

  private:
    /// Process a transmission by the specified 'bindable' according to the
    /// specified 'event'. Resume the specified 'coroutine'.
    void complete(const bsl::shared_ptr<ntci::Bindable>& bindable,
                  const ntca::BindEvent&                 event,
                  bsl::coroutine_handle<void>            coroutine);

    /// The awaitable.
    Concurrent::Bind* d_awaitable;
};

/// Provide an awaitable for a connect operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Concurrent::Connect
{
  public:
    /// Provide an awaiter for a connect operation.
    class Awaiter;

    /// Create a new awaitable that, when awaited, connects the 'connector'
    /// to the specified 'endpoint' according to the specified 'options'.
    explicit Connect(const bsl::shared_ptr<ntci::Connector>& connector,
                     const ntsa::Endpoint&                   endpoint,
                     ntca::ConnectOptions                    options);

    /// Create a new awaitable that, when awaited, connects the 'connector'
    /// to the resolution of the specified 'name' according to the specified
    /// 'options'.
    explicit Connect(const bsl::shared_ptr<ntci::Connector>& connector,
                     const bsl::string&                      name,
                     ntca::ConnectOptions                    options);

    /// Await the completion of the operation. Return the awaiter.
    Awaiter operator co_await();

  private:
    /// This class is not copy-constructable.
    Connect(const Connect&) = delete;

    /// This class is not move-constructable.
    Connect(Connect&&) = delete;

    /// This class is not copy-assignable.
    Connect& operator=(const Connect&) = delete;

    /// This class is not move-assignable.
    Connect& operator=(Connect&&) = delete;

  private:
    /// Allow the associated awaiter to access this class's private data.
    friend class Awaiter;

    /// The connector.
    bsl::shared_ptr<ntci::Connector> d_connector;

    /// The input endpoint.
    ntsa::Endpoint d_endpoint;

    /// The input name.
    bsl::string d_name;

    /// The input options.
    ntca::ConnectOptions d_options;

    /// The output result.
    ntci::ConnectResult d_result;
};

/// Provide an awaiter for a connect operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Concurrent::Connect::Awaiter
{
  public:
    /// Create a new awaiter that is the result of 'co_await'-ing the specified
    /// 'awaitable'.
    explicit Awaiter(Concurrent::Connect* awaitable) noexcept;

    /// Return false to suspend the awaiting coroutine.
    bool await_ready() const noexcept;

    /// Initiate the asynchronous operation and resume the specified
    /// 'coroutine' when the operation asynchronously completes or fails.
    void await_suspend(bsl::coroutine_handle<> coroutine) noexcept;

    /// Return the asynchronous result.
    ntci::ConnectResult await_resume() const noexcept;

  private:
    /// This class is not copy-constructable.
    Awaiter(const Awaiter&) = delete;

    /// This class is not move-constructable.
    Awaiter(Awaiter&&) = delete;

    /// This class is not copy-assignable.
    Awaiter& operator=(const Awaiter&) = delete;

    /// This class is not move-assignable.
    Awaiter& operator=(Awaiter&&) = delete;

  private:
    /// Process a transmission by the specified 'connector' according to the
    /// specified 'event'. Resume the specified 'coroutine'.
    void complete(const bsl::shared_ptr<ntci::Connector>& connector,
                  const ntca::ConnectEvent&               event,
                  bsl::coroutine_handle<void>             coroutine);

    /// The awaitable.
    Concurrent::Connect* d_awaitable;
};

/// Provide an awaitable for a accept operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Concurrent::Accept
{
  public:
    /// Provide an awaiter for a accept operation.
    class Awaiter;

    /// Create a new awaitable that, when awaited, dequeues a stream socket
    /// from the specified 'acceptor' backlog according to the specified
    /// 'options'.
    explicit Accept(const bsl::shared_ptr<ntci::Acceptor>& acceptor,
                    ntca::AcceptOptions                    options);

    /// Await the completion of the operation. Return the awaiter.
    Awaiter operator co_await();

  private:
    /// This class is not copy-constructable.
    Accept(const Accept&) = delete;

    /// This class is not move-constructable.
    Accept(Accept&&) = delete;

    /// This class is not copy-assignable.
    Accept& operator=(const Accept&) = delete;

    /// This class is not move-assignable.
    Accept& operator=(Accept&&) = delete;

  private:
    /// Allow the associated awaiter to access this class's private data.
    friend class Awaiter;

    /// The acceptor.
    bsl::shared_ptr<ntci::Acceptor> d_acceptor;

    /// The input options.
    ntca::AcceptOptions d_options;

    /// The output result.
    ntci::AcceptResult d_result;
};

/// Provide an awaiter for a accept operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Concurrent::Accept::Awaiter
{
  public:
    /// Create a new awaiter that is the result of 'co_await'-ing the specified
    /// 'awaitable'.
    explicit Awaiter(Concurrent::Accept* awaitable) noexcept;

    /// Return false to suspend the awaiting coroutine.
    bool await_ready() const noexcept;

    /// Initiate the asynchronous operation and resume the specified
    /// 'coroutine' when the operation asynchronously completes or fails.
    void await_suspend(bsl::coroutine_handle<> coroutine) noexcept;

    /// Return the asynchronous result.
    ntci::AcceptResult await_resume() const noexcept;

  private:
    /// This class is not copy-constructable.
    Awaiter(const Awaiter&) = delete;

    /// This class is not move-constructable.
    Awaiter(Awaiter&&) = delete;

    /// This class is not copy-assignable.
    Awaiter& operator=(const Awaiter&) = delete;

    /// This class is not move-assignable.
    Awaiter& operator=(Awaiter&&) = delete;

  private:
    /// Process a acceptance of the specified 'streamSocket' by the specified
    /// 'acceptor' according to the specified 'event'. Resume the specified
    /// 'coroutine'.
    void complete(const bsl::shared_ptr<ntci::Acceptor>&     acceptor,
                  const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                  const ntca::AcceptEvent&                   event,
                  bsl::coroutine_handle<void>                coroutine);

    /// The awaitable.
    Concurrent::Accept* d_awaitable;
};

/// Provide an awaitable for a send operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Concurrent::Send
{
  public:
    /// Provide an awaiter for a send operation.
    class Awaiter;

    /// Create a new awaitable that, when awaited, enqueues the specified
    /// 'data' for transmission through the specified 'sender' according to the
    /// specified 'options'.
    explicit Send(const bsl::shared_ptr<ntci::Sender>& sender,
                  const bsl::shared_ptr<bdlbb::Blob>&  data,
                  ntca::SendOptions                    options);

    /// Await the completion of the operation. Return the awaiter.
    Awaiter operator co_await();

  private:
    /// This class is not copy-constructable.
    Send(const Send&) = delete;

    /// This class is not move-constructable.
    Send(Send&&) = delete;

    /// This class is not copy-assignable.
    Send& operator=(const Send&) = delete;

    /// This class is not move-assignable.
    Send& operator=(Send&&) = delete;

  private:
    /// Allow the associated awaiter to access this class's private data.
    friend class Awaiter;

    /// The sender.
    bsl::shared_ptr<ntci::Sender> d_sender;

    /// The input data.
    ntsa::Data d_data;

    /// The input options.
    ntca::SendOptions d_options;

    /// The output result.
    ntci::SendResult d_result;
};

/// Provide an awaiter for a send operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Concurrent::Send::Awaiter
{
  public:
    /// Create a new awaiter that is the result of 'co_await'-ing the specified
    /// 'awaitable'.
    explicit Awaiter(Concurrent::Send* awaitable) noexcept;

    /// Return false to suspend the awaiting coroutine.
    bool await_ready() const noexcept;

    /// Initiate the asynchronous operation and resume the specified
    /// 'coroutine' when the operation asynchronously completes or fails.
    void await_suspend(bsl::coroutine_handle<> coroutine) noexcept;

    /// Return the asynchronous result.
    ntci::SendResult await_resume() const noexcept;

  private:
    /// This class is not copy-constructable.
    Awaiter(const Awaiter&) = delete;

    /// This class is not move-constructable.
    Awaiter(Awaiter&&) = delete;

    /// This class is not copy-assignable.
    Awaiter& operator=(const Awaiter&) = delete;

    /// This class is not move-assignable.
    Awaiter& operator=(Awaiter&&) = delete;

  private:
    /// Process a transmission by the specified 'sender' according to the
    /// specified 'event'. Resume the specified 'coroutine'.
    void complete(const bsl::shared_ptr<ntci::Sender>& sender,
                  const ntca::SendEvent&               event,
                  bsl::coroutine_handle<void>          coroutine);

    /// The awaitable.
    Concurrent::Send* d_awaitable;
};

/// Provide an awaitable for a receive operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Concurrent::Receive
{
  public:
    /// Provide an awaiter for a receive operation.
    class Awaiter;

    /// Create a new awaitable that, when awaited, dequeues received data from
    /// the specified 'receiver' according to the specified 'options'.
    explicit Receive(const bsl::shared_ptr<ntci::Receiver>& receiver,
                     ntca::ReceiveOptions                   options);

    /// Await the completion of the operation. Return the awaiter.
    Awaiter operator co_await();

  private:
    /// This class is not copy-constructable.
    Receive(const Receive&) = delete;

    /// This class is not move-constructable.
    Receive(Receive&&) = delete;

    /// This class is not copy-assignable.
    Receive& operator=(const Receive&) = delete;

    /// This class is not move-assignable.
    Receive& operator=(Receive&&) = delete;

  private:
    /// Allow the associated awaiter to access this class's private data.
    friend class Awaiter;

    /// The receiver.
    bsl::shared_ptr<ntci::Receiver> d_receiver;

    /// The input options.
    ntca::ReceiveOptions d_options;

    /// The output result.
    ntci::ReceiveResult d_result;
};

/// Provide an awaiter for a receive operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Concurrent::Receive::Awaiter
{
  public:
    /// Create a new awaiter that is the result of 'co_await'-ing the specified
    /// 'awaitable'.
    explicit Awaiter(Concurrent::Receive* awaitable) noexcept;

    /// Return false to suspend the awaiting coroutine.
    bool await_ready() const noexcept;

    /// Initiate the asynchronous operation and resume the specified
    /// 'coroutine' when the operation asynchronously completes or fails.
    void await_suspend(bsl::coroutine_handle<> coroutine) noexcept;

    /// Return the asynchronous result.
    ntci::ReceiveResult await_resume() const noexcept;

  private:
    /// This class is not copy-constructable.
    Awaiter(const Awaiter&) = delete;

    /// This class is not move-constructable.
    Awaiter(Awaiter&&) = delete;

    /// This class is not copy-assignable.
    Awaiter& operator=(const Awaiter&) = delete;

    /// This class is not move-assignable.
    Awaiter& operator=(Awaiter&&) = delete;

  private:
    /// Process a reception of the specified 'data' by the specified
    /// 'receiver' according to the specified 'event'. Resume the specified
    /// 'coroutine'.
    void complete(const bsl::shared_ptr<ntci::Receiver>& receiver,
                  const bsl::shared_ptr<bdlbb::Blob>&    data,
                  const ntca::ReceiveEvent&              event,
                  bsl::coroutine_handle<void>            coroutine);

    /// The awaitable.
    Concurrent::Receive* d_awaitable;
};

/// Provide an awaitable for a close operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Concurrent::Close
{
  public:
    /// Provide an awaiter for a close operation.
    class Awaiter;

    /// Create a new awaitable that, when awaited, closes the specified
    /// 'closable' object.
    explicit Close(const bsl::shared_ptr<ntci::Closable>& closable);

    /// Await the completion of the operation. Return the awaiter.
    Awaiter operator co_await();

  private:
    /// This class is not copy-constructable.
    Close(const Close&) = delete;

    /// This class is not move-constructable.
    Close(Close&&) = delete;

    /// This class is not copy-assignable.
    Close& operator=(const Close&) = delete;

    /// This class is not move-assignable.
    Close& operator=(Close&&) = delete;

  private:
    /// Allow the associated awaiter to access this class's private data.
    friend class Awaiter;

    /// The closable object.
    bsl::shared_ptr<ntci::Closable> d_closable;
};

/// Provide an awaiter for a close operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Concurrent::Close::Awaiter
{
  public:
    /// Create a new awaiter that is the result of 'co_await'-ing the specified
    /// 'awaitable'.
    explicit Awaiter(Concurrent::Close* awaitable) noexcept;

    /// Return false to suspend the awaiting coroutine.
    bool await_ready() const noexcept;

    /// Initiate the asynchronous operation and resume the specified
    /// 'coroutine' when the operation asynchronously completes or fails.
    void await_suspend(bsl::coroutine_handle<> coroutine) noexcept;

    /// Return the asynchronous result.
    void await_resume() const noexcept;

  private:
    /// This class is not copy-constructable.
    Awaiter(const Awaiter&) = delete;

    /// This class is not move-constructable.
    Awaiter(Awaiter&&) = delete;

    /// This class is not copy-assignable.
    Awaiter& operator=(const Awaiter&) = delete;

    /// This class is not move-assignable.
    Awaiter& operator=(Awaiter&&) = delete;

  private:
    /// Process the closure of the closable object. Resume the specified
    /// 'coroutine'.
    void complete(bsl::coroutine_handle<> coroutine);

    /// The awaitable.
    Concurrent::Close* d_awaitable;
};

}  // close namespace ntci
}  // close namespace BloombergLP

#endif
#endif
