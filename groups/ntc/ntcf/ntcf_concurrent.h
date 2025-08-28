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

#ifndef INCLUDED_NTCF_CONCURRENT
#define INCLUDED_NTCF_CONCURRENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_config.h>
#include <ntccfg_platform.h>
#include <ntcf_system.h>
#include <ntci_concurrent.h>
#include <ntsa_coroutine.h>
#include <ntsa_error.h>
#include <ntsf_system.h>

#if NTC_BUILD_WITH_COROUTINES

namespace BloombergLP {
namespace ntcf {

/// Provide an interface for concurrent operations.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Concurrent
{
  private:
    // Start the specified 'coroutine'.
    static void start(bsl::coroutine_handle<void> coroutine);

    // The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTCF.CONCURRENT");

  public:
    /// Provide an awaitable for an execute operation.
    class Execute;

    /// Provide an awaitable for a connect operation.
    class Connect;

    /// Provide an awaitable for a accept operation.
    class Accept;

    /// Provide an awaitable for a send operation.
    class Send;

    /// Provide an awaitable for a receive operation.
    class Receive;

    /// Provide an awaitable for a close operation.
    class Close;

    /// Initialize concurrent operations.
    static void initialize();

    // Resume the specified 'coroutine' on the specified 'executor'.
    static void spawn(const bsl::shared_ptr<ntci::Executor>& executor,
                      bsl::coroutine_handle<void>            coroutine);

    /// Resume the current coroutine on the specified 'executor'.
    static ntcf::Concurrent::Execute resume(
        const bsl::shared_ptr<ntci::Executor>& executor);

    /// Connect to the specified 'endpoint' according to the specified
    /// 'options'. Resume the current coroutine on this object's strand,
    /// if any, when the connection is established or an error occurs.
    /// Return the awaitable error.
    static ntcf::Concurrent::Connect connect(
        const bsl::shared_ptr<ntci::Connector>& connector,
        const ntsa::Endpoint&                   endpoint,
        const ntca::ConnectOptions&             options);

    /// Connect to the resolution of the specified 'name' according to the
    /// specified 'options'. Resume the current coroutine on this
    /// object's strand, if any, when the connection is established or an
    /// error occurs. Return the awaitable error.
    static ntcf::Concurrent::Connect connect(
        const bsl::shared_ptr<ntci::Connector>& connector,
        const bsl::string&                      name,
        const ntca::ConnectOptions&             options);

    /// Dequeue a connection from the backlog according to the specified
    /// 'options'. If the accept queue is not empty, synchronously pop the
    /// front of the accept queue into an internally allocated stream socket
    /// and resume the current coroutine on the objects's strand, if any, with
    /// that stream socket. Otherwise, queue the accept operation and
    /// asynchronously accept connections from the backlog onto the accept
    /// queue as connections in the backlog become accepted, at the configured
    /// accept rate limit, if any, up to the accept queue high watermark. When
    /// the accept queue becomes non-empty, synchronously pop the front of the
    /// accept queue into an internally allocated stream socket and resume the
    /// current corouting on the objects' strand, if any, with that stream
    /// socket. After satisfying any queued accept operations, when the accept
    /// queue is asynchronously filled up to the accept queue low watermark,
    /// announce an accept queue low watermark event. When asynchronously
    /// enqueuing connections onto the accept queue causes the accept queue
    /// high watermark to become breached, stop asynchronously accepting
    /// connections from the backlog onto the accept queue and announce an
    /// accept queue high watermark event. Return the awaitable error, notably
    /// 'ntsa::Error::e_WOULD_BLOCK' if neither the accept queue nor the
    /// backlog is non-empty. All other errors indicate no more connections
    /// have been accepted at this time or will become accepted in the future.
    static ntcf::Concurrent::Accept accept(
        const bsl::shared_ptr<ntci::Acceptor>& acceptor,
        const ntca::AcceptOptions&             options);

    /// Enqueue the specified 'data' for transmission according to the
    /// specified 'options'. If the write queue is empty and the write rate
    /// limit, if any, is not exceeded, synchronously copy the 'data' to the
    /// socket send buffer. Otherwise, if the write queue is not empty, or the
    /// write rate limit, if any, is exceeded, or the socket send buffer has
    /// insufficient capacity to store the entirety of the 'data', enqueue the
    /// remainder of the 'data' not copied to the socket send buffer onto the
    /// write queue and asynchronously copy the write queue to the socket send
    /// buffer as capacity in the socket send buffer becomes available, at the
    /// configured write rate limit, if any, according to the priorities of the
    /// individual write operations on the write queue. The integrity of the
    /// entire sequence of the 'data' is always preserved when transmitting the
    /// data stream, even when other data is sent concurrently by different
    /// threads, although the order of transmission of the entirety of the
    /// 'data' in relation to other transmitted data is unspecified. If
    /// enqueuing the 'data' onto the write queue causes the write queue high
    /// watermark to become breached, announce a write queue high watermark
    /// event but continue to queue the 'data' for transmission. After a write
    /// queue high watermark event is announced, announce a write queue low
    /// watermark event when the write queue is subsequently and asynchronously
    /// drained down to the write queue low watermark. A write queue high
    /// watermark event must be first announced before a write queue low
    /// watermark event will be announced, and thereafter a write queue low
    /// watermark event must be announced before a subsequent write queue high
    /// watermark event will be announced. When the 'data' has been completely
    /// copied to the send buffer, resume the current coroutine on this
    /// object's strand, if any. Return the awaitable error, notably
    /// 'ntsa::Error::e_WOULD_BLOCK' if the size of the write queue has already
    /// breached the write queue high watermark. All other errors indicate the
    /// socket is incapable of transmitting data at this time or any time in
    /// the future.
    static ntcf::Concurrent::Send send(
        const bsl::shared_ptr<ntci::Sender>& sender,
        const bsl::shared_ptr<bdlbb::Blob>&  data,
        const ntca::SendOptions&             options);

    /// Dequeue received data according to the specified 'options'. If the read
    /// queue has sufficient size to satisfy the read operation, synchronously
    /// copy the read queue into an internally allocated data structure and
    /// resume the current coroutine on this object's strand, if any, with that
    /// data structure. Otherwise, queue the read operation and asynchronously
    /// copy the socket receive buffer onto the read queue as data in the
    /// socket receive buffer becomes available, at the configured read rate
    /// limit, if any, up to the read queue high watermark. When the read queue
    /// is asynchronously filled to a sufficient size to satisfy the read
    /// operation, synchronously copy the read queue into an internally
    /// allocated data structure and resume the current coroutine on this
    /// object's strand, if any, with that data structure. After satisfying any
    /// queued read operations, when the read queue is asynchronously filled up
    /// to the read queue low watermark, announce a read queue low watermark
    /// event. When asynchronously enqueuing data onto the read queue causes
    /// the read queue high watermark to become breached, stop asynchronously
    /// copying the socket receive buffer to the read queue and announce a read
    /// queue high watermark event. Return the awaitable error, notably
    /// 'ntsa::Error::e_WOULD_BLOCK' if neither the read queue nor the socket
    /// receive buffer has sufficient size to synchronously satisfy the read
    /// operation, or 'ntsa::Error::e_EOF' if the read queue is empty and the
    /// socket receive buffer has been shut down. All other errors indicate no
    /// more received data is available at this time or will become available
    /// in the future.
    static ntcf::Concurrent::Receive receive(
        const bsl::shared_ptr<ntci::Receiver>& receiver,
        const ntca::ReceiveOptions&            options);

    /// Close the specified 'closable' object. Return the awaitable "void".
    static ntcf::Concurrent::Close close(
        const bsl::shared_ptr<ntci::Closable>& closable);

    /// Clean up the resources required by all concurrent operations.
    static void exit();
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
    /// TODO
    class Awaiter;

    /// TODO
    explicit Execute(const bsl::shared_ptr<ntci::Executor>& executor);

    /// TODO
    Awaiter operator co_await();

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

    // TODO
    bool await_ready() const noexcept;

    // TODO
    void await_suspend(bsl::coroutine_handle<> coroutine) noexcept;

    // TODO
    void await_resume() const noexcept;

  private:
    /// Resume the specified 'coroutine'.
    static void complete(bsl::coroutine_handle<void> coroutine);

    /// The awaitable.
    Concurrent::Execute* d_awaitable;
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
    /// TODO
    class Awaiter;

    /// TODO
    explicit Connect(const bsl::shared_ptr<ntci::Connector>& connector,
                     const ntsa::Endpoint&                   endpoint,
                     ntca::ConnectOptions                    options);

    /// TODO
    explicit Connect(const bsl::shared_ptr<ntci::Connector>& connector,
                     const bsl::string&                      name,
                     ntca::ConnectOptions                    options);

    /// TODO
    Awaiter operator co_await();

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

    // TODO
    bool await_ready() const noexcept;

    // TODO
    void await_suspend(bsl::coroutine_handle<> coroutine) noexcept;

    // TODO
    ntci::ConnectResult await_resume() const noexcept;

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
    /// TODO
    class Awaiter;

    /// TODO
    explicit Accept(const bsl::shared_ptr<ntci::Acceptor>& acceptor,
                    ntca::AcceptOptions                    options);

    /// TODO
    Awaiter operator co_await();

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

    // TODO
    bool await_ready() const noexcept;

    // TODO
    void await_suspend(bsl::coroutine_handle<> coroutine) noexcept;

    // TODO
    ntci::AcceptResult await_resume() const noexcept;

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
    /// TODO
    class Awaiter;

    /// TODO
    explicit Send(const bsl::shared_ptr<ntci::Sender>& sender,
                  const bsl::shared_ptr<bdlbb::Blob>&  data,
                  ntca::SendOptions                    options);

    /// TODO
    Awaiter operator co_await();

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

    // TODO
    bool await_ready() const noexcept;

    // TODO
    void await_suspend(bsl::coroutine_handle<> coroutine) noexcept;

    // TODO
    ntci::SendResult await_resume() const noexcept;

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
    /// TODO
    class Awaiter;

    /// TODO
    explicit Receive(const bsl::shared_ptr<ntci::Receiver>& receiver,
                     ntca::ReceiveOptions                   options);

    /// TODO
    Awaiter operator co_await();

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

    // TODO
    bool await_ready() const noexcept;

    // TODO
    void await_suspend(bsl::coroutine_handle<> coroutine) noexcept;

    // TODO
    ntci::ReceiveResult await_resume() const noexcept;

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
    /// TODO
    class Awaiter;

    /// TODO
    explicit Close(const bsl::shared_ptr<ntci::Closable>& closable);

    /// TODO
    Awaiter operator co_await();

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

    // TODO
    bool await_ready() const noexcept;

    // TODO
    void await_suspend(bsl::coroutine_handle<> coroutine) noexcept;

    // TODO
    void await_resume() const noexcept;

  private:
    /// Process the closure of the closable object. Resume the specified
    /// 'coroutine'.
    void complete(bsl::coroutine_handle<> coroutine);

    /// The awaitable.
    Concurrent::Close* d_awaitable;
};

}  // close namespace ntcf
}  // close namespace BloombergLP

#endif
#endif
