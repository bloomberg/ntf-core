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

#ifndef INCLUDED_NTCI_DATAGRAMSOCKET
#define INCLUDED_NTCI_DATAGRAMSOCKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_datagramsocketevent.h>
#include <ntca_flowcontrolmode.h>
#include <ntca_flowcontroltype.h>
#include <ntca_receiveoptions.h>
#include <ntca_receivetoken.h>
#include <ntca_sendoptions.h>
#include <ntca_sendtoken.h>
#include <ntccfg_platform.h>
#include <ntci_bindable.h>
#include <ntci_closable.h>
#include <ntci_closecallback.h>
#include <ntci_closecallbackfactory.h>
#include <ntci_connector.h>
#include <ntci_datapool.h>
#include <ntci_executor.h>
#include <ntci_ratelimiter.h>
#include <ntci_receivecallback.h>
#include <ntci_receivecallbackfactory.h>
#include <ntci_receiver.h>
#include <ntci_resolver.h>
#include <ntci_sendcallback.h>
#include <ntci_sendcallbackfactory.h>
#include <ntci_sender.h>
#include <ntci_strand.h>
#include <ntci_strandfactory.h>
#include <ntci_timerfactory.h>
#include <ntcscm_version.h>
#include <ntsa_buffer.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_message.h>
#include <ntsa_shutdownmode.h>
#include <ntsa_shutdowntype.h>
#include <ntsi_datagramsocket.h>
#include <ntsi_descriptor.h>
#include <bdlbb_blob.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadutil.h>
#include <bsl_memory.h>

namespace BloombergLP {

namespace ntci {
class DatagramSocketManager;
}
namespace ntci {
class DatagramSocketSession;
}
namespace ntci {

/// Provide an interface to an asynchronous datagram socket.
///
/// @details
/// Datagram sockets provide connectionless, message-oriented unicast or
/// multicast communication to arbitrary endpoints. An 'ntci::DatagramSocket'
/// behaves fundamentally like an operating system datagram socket with the
/// following enhancements:
///
/// 1. All operations on a 'ntci::DatagramSocket' may be performed
///    asynchronously, rather than simply blocking or non-blocking.
///
/// 2. Users may interact with an 'ntci::DatagramSocket' either "reactively"
///    (i.e. similarly to the Unix readiness model exemplified by 'select' and
///    'poll') or "proactively" (i.e. similarly to the Windows I/O completion
///    port model), regardless of whether the implementation details of the
///    interface to the operating system operates "reactively" or
///    "proactively".
///
/// 3. Each 'ntci::DatagramSocket' maintains a "write queue" that extends the
///    concept of the operating system socket send buffer. The write queue
///    buffers an arbitrary amount of outgoing data up to a configurable limit
///    specified by the user. The write queue provides greater tolerance of
///    a discrepency between the rate of transmission by the user and rate of
///    transmission by the operation system through the networking hardware.
///
/// 4. Each 'ntci::DatagramSocket' maintains a "read queue" that extends the
///    concept of the operating system socket receive buffer. The read queue
///    buffers an arbitrary amount of incoming data up to a configurable limit
///    specified by the user. The read queue provides greater tolerance of a
///    discrepency between the rate of reception and the rate of transmission.
///
/// 5. Users may explicitly cancel and/or specified deadlines (i.e. "timeout")
///    when initiating connect, send, receive, and close operations.
///
/// An 'ntci::DatagramSocket' operates over either the User Datagram Protocol
/// (UDP) transport over an Internet Protocol (IP) version 4 or version
/// 6 network, or, on Unix-like platforms, using a local (a.k.a. Unix) domain
/// transport. The type of transport is either chosen at the time the socket is
/// opened, or alternatively, at the time the socket is explicitly bound to an
/// endpoint on the local host or connected to a peer on a remote host.
///
/// For a popular explanation of sockets, address families, protocols, see
/// "UNIX Network Programming, Volume 1: The Sockets Networking API", by
/// W. Richard Stevens.
///
/// Essentially, an 'ntci::DatagramSocket' implements the Berkeley Sockets API
/// for datagram sockets with an asynchronous API rather than a non-blocking
/// API.
///
/// @par Reactive and Proactive Usage
/// An 'ntci::StreamSocket' internally and automatically manages its
/// asynchronous behavior through an 'ntci::Reactor' or 'ntci::Proactor', whose
/// injection is abstracted from the user by the 'ntci::StreamSocketFactory'
/// used to produce a concrete implementation of the 'ntci::StreamSocket'
/// abstract class. Multiple classes implement the 'ntci::StreamSocketFactory'
/// interface, including the 'ntci::Reactor' or 'ntci::Proactor' classes
/// themselves, but typical usage is to create an 'ntci::StreamSocket' from
/// an 'ntci::Interface' which represents a pool of threads automatically
/// driving one or more 'ntci::Reactor' or 'ntci::Proactor' objects. In other
/// words, users may create 'ntci::StreamSocket' objects whose asynchronous
/// behavior is automatically executing by one or more background threads, but
/// may also choose to control the thread on which asynchronous callbacks are
/// invoked.
///
/// Despite being internally implemented using either an 'ntci::Reactor' or an
/// 'ntci::Proactor' (but never both or more than one at the same time), the
/// interface of an 'ntci::StreamSocket' itself may be used either "reactively"
/// or "proactively"; this feature is enabled by the introduction of the read
/// and write queues, mentioned above. The write queue and the read queue are
/// explained in more detail in following sections. The differences between
/// using a socket "reactively" vs. "proactivly" can be summarized as follows:
///
/// - In "reactive" usage, the user declares their interest to be
///   asynchronously notified when the socket reaches certain conditions on its
///   internal state. Upon being asynchronously notified that the socket has
///   reached one of those conditions, the user invokes a synchronous operation
///   based upon the state of the socket. For example, a "reactive read" is
///   performed by the user declaring their interest to be asynchronously
///   notified when the socket's read queue has at least a certain amount of
///   data available to be dequeued. Upon being asynchronously notified the
///   socket has reached such a state, the user performs one or more
///   synchronous operations to dequeue data from the read queue. This type of
///   usage is exemplified when using an operating system socket in conjunction
///   with the POSIX function 'poll', for example.
///
/// - In "proactive" usage, the user declares their interest to transition the
///   socket into a state, regardless of whether that operation can be
///   synchronously completed at the time the operation is initiated. When the
///   operation cannot be synchronously completed, it is queued and completed
///   asynchronously when the criteria to complete the operation becomes
///   satisfied. For example, a "proactive read" is performed by the user
///   declaring their desire to receive a certain amount of data. If that
///   amount of data is not already available to be dequeued from the read
///   queue, the operation is internally queued until the criteria is
///   satisfied, upon which a callback is asynchronously invoked indicating the
///   completion of the operation. This type of usage is exemplified by using
///   an operating system socket in conjunction with something like a
///   (non-portable) Windows I/O completion port.
///
/// @par Binding to a Local Endpoint
/// An 'ntci::DatagramSocket' is is connectionless but may become bound
/// to an endpoint on the local host before or during the establishment of a
/// connection to a peer or transmission of data to some peer. Users are not
/// required to explicitly bind a socket before connecting it to a peer or
/// sending data to a peer, but may choose to do so if that behavior is
/// required. Users may specify the endpoint to which an 'ntci::DatagramSocket'
/// is bound either explicitly, in the form of an exact 'ntsa::Endpoint', or
/// implicitly, in the form of a Domain name and optional service name, both of
/// which are automatically and asynchronously resolved to a specific
/// 'ntsa::Endpoint' to which the socket becomes bound.
///
/// @par Connecting to a Remote Endpoint
/// An 'ntci::DatagramSocket' is connectionless. It need not be connected to
/// an endpoint of a peer to send data to that peer. However, a user may choose
/// to "connect" a datagram socket to a peer endpoint, which results in the
/// following behavior.
///
/// 1. The peer endpoint may be left unspecified when sending data.
///
/// 2. Only datagrams from that peer will be received, datagrams from other
///    peer endpoints will be dropped.
///
/// Once a datagram socket is connected, it may be reconnected to another peer.
/// If a datagram socket is not connected, the endpoint of the peer must be
/// explicitly specified when sending data.
///
/// @par Unordered Concurrent Writes
/// Operating system datagram sockets have a distinction of having datagram
/// semantics as opposed to stream semantics. With datagram semantics the
/// network transport sends and receives data exactly as requested: each
/// transmission will be received exactly as specified or not at all. Each
/// 'ntci::DatagramSocket' supports multiple, concurrent senders but makes no
/// claim on the order of transmission.
///
/// @par The Write Queue
/// Each 'ntci::DatagramSocket' maintains a "write queue", which conceptually
/// extends of the operating system socket send buffer. If data being sent
/// cannot be immediately and entirely copied to the operating system socket
/// send buffer, the entirety of the data is automatically enqueued to the back
/// of the write queue until capacity in the operating system socket send
/// buffer becomes available. The operating system send buffer can become full
/// when the rate of data being sent is greater than the rate of the operating
/// system to process the transmission of the transport protocol and ready the
/// data to be transmitted by the network hardware.
///
/// @par Sending Data
/// The 'send' function copies data to the socket send buffer and always has
/// asynchronous semantics, regardless of whether a callback is also specified.
/// As applied here, the term "asynchronous" means "queued for later
/// completion".  When the socket send buffer has insufficient capacity to
/// store the entirety of the data, the remainder is enqueued to the write
/// queue. When the 'send' function is called and the write queue is not empty,
/// all data is immediately enqueued to the write queue. As the operating
/// system asynchronously drains the socket send buffer as a result of
/// transmitting data to its destination, the write queue is asynchronously and
/// automatically copied to the socket send buffer, and drained as it is
/// copied. The 'send' function returns the following synchronous error codes:
///
/// - ntsa::Error::e_OK:          The send operation succeeded: the data was
///                               either entirely copied to the socket send
///                               buffer or some or all of the data was
///                               enqueued to the write queue.
///
/// - ntsa::Error::e_WOULD_BLOCK: The send operation failed but may succeed if
///                               retried later: The current size of the write
///                               queue is already greater than the write queue
///                               high watermark.
///
/// - ntsa::Error::e_EOF:         The send operation failed and all future send
///                               operations will also fail: The socket has
///                               been shut down for writing.
///
/// - All others:                 The send operation failed and all future send
///                               operations will also fail, at least initially
///                               for the reason described by the error code.
///
/// Note that only the 'ntsa::Error::e_OK' error code indicates "success": the
/// data is either entirely copied to the socket send buffer or at least
/// partially enqueued to the write queue and the associated callback, if any,
/// will be invoked once the data has been entirely copied to the socket send
/// buffer. While the 'ntsa::Error::e_WOULD_BLOCK' error code indicates
/// "failure" that failure is perhaps temporary; no more data can be sent until
/// the write queue drains but once the write queue drains below write queue
/// low watermark a subsequent send operation may succeed.  All other errors
/// indicate permanent "failure": the operation and all future send operations
/// will fail. Also note that a "success" result does not mean any recipient
/// received or will receive the data, only that it has been enqueued for
/// transmission.
///
/// A callback may be optionally supplied with each send operation. If such a
/// callback is supplied, this callback is invoked under the following
/// conditions with the following asynchronous error codes:
///
/// - ntsa::Error::e_OK:          The send operation succeeded: The data
///                               has been entirely copied to the socket send
///                               buffer.
///
/// - ntsa::Error::e_CANCELLED:   The send operation failed because it was
///                               explicitly cancelled: The 'cancel' function
///                               was called with the send token specified in
///                               the options at the time the send operation
///                               was initiated before any data desired to send
///                               was copied to the socket send buffer. Future
///                               send operations may succeed.
///
/// - ntsa::Error::e_WOULD_BLOCK: The send operation failed because it
///                               implicitly timed out: The deadline specified
///                               in the send options at the time the operation
///                               was initiated elapsed before any the data
///                               desired to send was copied to the socket send
///                               buffer. Future send operations may succeed.
///
/// - ntsa::Error::e_EOF:         The send operation failed and all future send
///                               operations will also fail: The socket has
///                               been shut down for writing.
///
/// - All others:                 The send operation failed and all future send
///                               operations will also fail, at least initially
///                               for the reason described by the error code.
///
/// Note that only the 'ntsa::Error::e_OK' code indicates "success": the data
/// has been entirely copied to the socket send buffer. All other errors
/// indicate "failure" but must be interpreted to understand if data may have
/// been transmitted and if future send operations may succeed. The
/// 'ntsa::Error::e_WOULD_BLOCK' and 'ntsa::Error::e_CANCELLED' error codes
/// indicate no portion of the data has been transmitted and subsequent send
/// operations may succeed. All other errors indicate the send operation failed
/// and all subsequent send operations will fail, but some data may have been
/// transmitted.
///
/// @par Closing
/// Each 'ntci::DatagramSocket' is shared between the user and this library's
/// asynchronous machinery. It is not sufficient for users to simply release
/// their reference counts on a datagram socket object to close and destroy it.
/// Users *must* explicitly close each 'ntci::DatagramSocket'. Closing a socket
/// is asynchronous, users must wait until the close callback is invoked before
/// assuming the socket is completely closed. After a socket's close callback
/// is invoked, the socket remains in a valid state but all member functions
/// with failure modes will return an error. The socket object will be
/// destroyed only after it has been closed and all references are released.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_socket
class DatagramSocket : public ntsi::Descriptor,
                       public ntci::Executor,
                       public ntci::StrandFactory,
                       public ntci::TimerFactory,
                       public ntci::Closable,
                       public ntci::Bindable,
                       public ntci::Connector,
                       public ntci::Sender,
                       public ntci::Receiver,
                       public ntci::DataPool
{
  public:
    /// Define a type alias for a callback function invoked
    /// when an event occurs on a stream socket.
    typedef NTCCFG_FUNCTION(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::DatagramSocketEvent&             event) SessionCallback;

    /// Destroy this object.
    virtual ~DatagramSocket();

    /// Open the datagram socket. Return the error.
    virtual ntsa::Error open() = 0;

    /// Open the datagram socket using the specified 'transport'. Return the
    /// error.
    virtual ntsa::Error open(ntsa::Transport::Value transport) = 0;

    /// Open the datagram socket using the specified 'transport' imported
    /// from the specified 'handle'. Return the error.
    virtual ntsa::Error open(ntsa::Transport::Value transport,
                             ntsa::Handle           handle) = 0;

    /// Open the stream socket using the specified 'transport' imported from
    /// the specified 'datagramSocket'. Return the error.
    virtual ntsa::Error open(
        ntsa::Transport::Value                       transport,
        const bsl::shared_ptr<ntsi::DatagramSocket>& datagramSocket) = 0;

    /// Bind to the specified source 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on the callback's strand,
    /// if any, when the socket has been bound or any error occurs. Return
    /// the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created.
    virtual ntsa::Error bind(const ntsa::Endpoint&     endpoint,
                             const ntca::BindOptions&  options,
                             const ntci::BindFunction& callback) = 0;

    /// Bind to the specified source 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on the callback's strand,
    /// if any, when the socket has been bound or any error occurs. Return
    /// the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created.
    virtual ntsa::Error bind(const ntsa::Endpoint&     endpoint,
                             const ntca::BindOptions&  options,
                             const ntci::BindCallback& callback) = 0;

    /// Bind to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error bind(const bsl::string&        name,
                             const ntca::BindOptions&  options,
                             const ntci::BindFunction& callback) = 0;

    /// Bind to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error bind(const bsl::string&        name,
                             const ntca::BindOptions&  options,
                             const ntci::BindCallback& callback) = 0;

    /// Connect to the specified 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on the callback's strand,
    /// if any, when the connection is established or an error occurs.
    /// Return the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created.
    virtual ntsa::Error connect(const ntsa::Endpoint&        endpoint,
                                const ntca::ConnectOptions&  options,
                                const ntci::ConnectFunction& callback) = 0;

    /// Connect to the specified 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on the callback's strand,
    /// if any, when the connection is established or an error occurs.
    /// Return the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created.
    virtual ntsa::Error connect(const ntsa::Endpoint&        endpoint,
                                const ntca::ConnectOptions&  options,
                                const ntci::ConnectCallback& callback) = 0;

    /// Connect to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the connection is established or an
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error connect(const bsl::string&           name,
                                const ntca::ConnectOptions&  options,
                                const ntci::ConnectFunction& callback) = 0;

    /// Connect to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the connection is established or an
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error connect(const bsl::string&           name,
                                const ntca::ConnectOptions&  options,
                                const ntci::ConnectCallback& callback) = 0;

    /// Enqueue the specified 'data' for transmission according to the
    /// specified 'options'. If the write queue is empty and the write rate
    /// limit, if any, is not exceeded, synchronously copy the 'data' to the
    /// socket send buffer. Otherwise, if the write queue is not empty, or
    /// the write rate limit, if any, is exceeded, or the socket send buffer
    /// has insufficient capacity to store the entirety of the 'data',
    /// enqueue the remainder of the 'data' not copied to the socket send
    /// buffer onto the write queue and asynchronously copy the write queue
    /// to the socket send buffer as capacity in the socket send buffer
    /// becomes available, at the configured write rate limit, if any,
    /// according to the priorities of the individual write operations on
    /// the write queue. The integrity of the entire sequence of the 'data'
    /// is always preserved when transmitting the data stream, even when
    /// other data is sent concurrently by different threads, although the
    /// order of transmission of the entirety of the 'data' in relation to
    /// other transmitted data is unspecified. If enqueuing the 'data' onto
    /// the write queue causes the write queue high watermark to become
    /// breached, announce a write queue high watermark event but continue
    /// to queue the 'data' for transmission. After a write queue high
    /// watermark event is announced, announce a write queue low watermark
    /// event when the write queue is subsequently and asynchronously
    /// drained down to the write queue low watermark. A write queue high
    /// watermark event must be first announced before a write queue low
    /// watermark event will be announced, and thereafter a write queue
    /// low watermark event must be announced before a subsequent write
    /// queue high watermark event will be announced. Return the error,
    /// notably 'ntsa::Error::e_WOULD_BLOCK' if the size of the write queue
    /// has already breached the write queue high watermark. All other
    /// errors indicate the socket is incapable of transmitting data at this
    /// time or any time in the future.
    virtual ntsa::Error send(const bdlbb::Blob&       data,
                             const ntca::SendOptions& options) = 0;

    /// Enqueue the specified 'data' for transmission according to the
    /// specified 'options'. If the write queue is empty and the write rate
    /// limit, if any, is not exceeded, synchronously copy the 'data' to the
    /// socket send buffer. Otherwise, if the write queue is not empty, or
    /// the write rate limit, if any, is exceeded, or the socket send buffer
    /// has insufficient capacity to store the entirety of the 'data',
    /// enqueue the remainder of the 'data' not copied to the socket send
    /// buffer onto the write queue and asynchronously copy the write queue
    /// to the socket send buffer as capacity in the socket send buffer
    /// becomes available, at the configured write rate limit, if any,
    /// according to the priorities of the individual write operations on
    /// the write queue. The integrity of the entire sequence of the 'data'
    /// is always preserved when transmitting the data stream, even when
    /// other data is sent concurrently by different threads, although the
    /// order of transmission of the entirety of the 'data' in relation to
    /// other transmitted data is unspecified. If enqueuing the 'data' onto
    /// the write queue causes the write queue high watermark to become
    /// breached, announce a write queue high watermark event but continue
    /// to queue the 'data' for transmission. After a write queue high
    /// watermark event is announced, announce a write queue low watermark
    /// event when the write queue is subsequently and asynchronously
    /// drained down to the write queue low watermark. A write queue high
    /// watermark event must be first announced before a write queue low
    /// watermark event will be announced, and thereafter a write queue
    /// low watermark event must be announced before a subsequent write
    /// queue high watermark event will be announced. Return the error,
    /// notably 'ntsa::Error::e_WOULD_BLOCK' if the size of the write queue
    /// has already breached the write queue high watermark. All other
    /// errors indicate the socket is incapable of transmitting data at this
    /// time or any time in the future. Note that 'ntsa::Data' is a
    /// container of data with many possibly representations and ownership
    /// semantics, and the 'data' will be queued, copied, shared, or held
    /// according to the user's choice of representation.
    virtual ntsa::Error send(const ntsa::Data&        data,
                             const ntca::SendOptions& options) = 0;

    /// Enqueue the specified 'data' for transmission according to the
    /// specified 'options'. If the write queue is empty and the write rate
    /// limit, if any, is not exceeded, synchronously copy the 'data' to the
    /// socket send buffer. Otherwise, if the write queue is not empty, or
    /// the write rate limit, if any, is exceeded, or the socket send buffer
    /// has insufficient capacity to store the entirety of the 'data',
    /// enqueue the remainder of the 'data' not copied to the socket send
    /// buffer onto the write queue and asynchronously copy the write queue
    /// to the socket send buffer as capacity in the socket send buffer
    /// becomes available, at the configured write rate limit, if any,
    /// according to the priorities of the individual write operations on
    /// the write queue. The integrity of the entire sequence of the 'data'
    /// is always preserved when transmitting the data stream, even when
    /// other data is sent concurrently by different threads, although the
    /// order of transmission of the entirety of the 'data' in relation to
    /// other transmitted data is unspecified. If enqueuing the 'data' onto
    /// the write queue causes the write queue high watermark to become
    /// breached, announce a write queue high watermark event but continue
    /// to queue the 'data' for transmission. After a write queue high
    /// watermark event is announced, announce a write queue low watermark
    /// event when the write queue is subsequently and asynchronously
    /// drained down to the write queue low watermark. A write queue high
    /// watermark event must be first announced before a write queue low
    /// watermark event will be announced, and thereafter a write queue
    /// low watermark event must be announced before a subsequent write
    /// queue high watermark event will be announced. When the 'data' has
    /// been completly copied to the send buffer, invoke the specified
    /// 'callback' on the callbacks's strand, if any. Return the error,
    /// notably 'ntsa::Error::e_WOULD_BLOCK' if the size of the write queue
    /// has already breached the write queue high watermark. All other
    /// errors indicate the socket is incapable of transmitting data at this
    /// time or any time in the future. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error send(const bdlbb::Blob&        data,
                             const ntca::SendOptions&  options,
                             const ntci::SendFunction& callback) = 0;

    /// Enqueue the specified 'data' for transmission according to the
    /// specified 'options'. If the write queue is empty and the write rate
    /// limit, if any, is not exceeded, synchronously copy the 'data' to the
    /// socket send buffer. Otherwise, if the write queue is not empty, or
    /// the write rate limit, if any, is exceeded, or the socket send buffer
    /// has insufficient capacity to store the entirety of the 'data',
    /// enqueue the remainder of the 'data' not copied to the socket send
    /// buffer onto the write queue and asynchronously copy the write queue
    /// to the socket send buffer as capacity in the socket send buffer
    /// becomes available, at the configured write rate limit, if any,
    /// according to the priorities of the individual write operations on
    /// the write queue. The integrity of the entire sequence of the 'data'
    /// is always preserved when transmitting the data stream, even when
    /// other data is sent concurrently by different threads, although the
    /// order of transmission of the entirety of the 'data' in relation to
    /// other transmitted data is unspecified. If enqueuing the 'data' onto
    /// the write queue causes the write queue high watermark to become
    /// breached, announce a write queue high watermark event but continue
    /// to queue the 'data' for transmission. After a write queue high
    /// watermark event is announced, announce a write queue low watermark
    /// event when the write queue is subsequently and asynchronously
    /// drained down to the write queue low watermark. A write queue high
    /// watermark event must be first announced before a write queue low
    /// watermark event will be announced, and thereafter a write queue
    /// low watermark event must be announced before a subsequent write
    /// queue high watermark event will be announced. When the 'data' has
    /// been completly copied to the send buffer, invoke the specified
    /// 'callback' on the callbacks's strand, if any. Return the error,
    /// notably 'ntsa::Error::e_WOULD_BLOCK' if the size of the write queue
    /// has already breached the write queue high watermark. All other
    /// errors indicate the socket is incapable of transmitting data at this
    /// time or any time in the future. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error send(const bdlbb::Blob&        data,
                             const ntca::SendOptions&  options,
                             const ntci::SendCallback& callback) = 0;

    /// Enqueue the specified 'data' for transmission according to the
    /// specified 'options'. If the write queue is empty and the write rate
    /// limit, if any, is not exceeded, synchronously copy the 'data' to the
    /// socket send buffer. Otherwise, if the write queue is not empty, or
    /// the write rate limit, if any, is exceeded, or the socket send buffer
    /// has insufficient capacity to store the entirety of the 'data',
    /// enqueue the remainder of the 'data' not copied to the socket send
    /// buffer onto the write queue and asynchronously copy the write queue
    /// to the socket send buffer as capacity in the socket send buffer
    /// becomes available, at the configured write rate limit, if any,
    /// according to the priorities of the individual write operations on
    /// the write queue. The integrity of the entire sequence of the 'data'
    /// is always preserved when transmitting the data stream, even when
    /// other data is sent concurrently by different threads, although the
    /// order of transmission of the entirety of the 'data' in relation to
    /// other transmitted data is unspecified. If enqueuing the 'data' onto
    /// the write queue causes the write queue high watermark to become
    /// breached, announce a write queue high watermark event but continue
    /// to queue the 'data' for transmission. After a write queue high
    /// watermark event is announced, announce a write queue low watermark
    /// event when the write queue is subsequently and asynchronously
    /// drained down to the write queue low watermark. A write queue high
    /// watermark event must be first announced before a write queue low
    /// watermark event will be announced, and thereafter a write queue
    /// low watermark event must be announced before a subsequent write
    /// queue high watermark event will be announced. When the 'data' has
    /// been completly copied to the send buffer, invoke the specified
    /// 'callback' on the callbacks's strand, if any. Return the error,
    /// notably 'ntsa::Error::e_WOULD_BLOCK' if the size of the write queue
    /// has already breached the write queue high watermark. All other
    /// errors indicate the socket is incapable of transmitting data at this
    /// time or any time in the future. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    /// Also note that 'ntsa::Data' is a container of data with many
    /// possibly representations and ownership semantics, and the 'data'
    /// will be queued, copied, shared, or held according to the user's
    /// choice of representation.
    virtual ntsa::Error send(const ntsa::Data&         data,
                             const ntca::SendOptions&  options,
                             const ntci::SendFunction& callback) = 0;

    /// Enqueue the specified 'data' for transmission according to the
    /// specified 'options'. If the write queue is empty and the write rate
    /// limit, if any, is not exceeded, synchronously copy the 'data' to the
    /// socket send buffer. Otherwise, if the write queue is not empty, or
    /// the write rate limit, if any, is exceeded, or the socket send buffer
    /// has insufficient capacity to store the entirety of the 'data',
    /// enqueue the remainder of the 'data' not copied to the socket send
    /// buffer onto the write queue and asynchronously copy the write queue
    /// to the socket send buffer as capacity in the socket send buffer
    /// becomes available, at the configured write rate limit, if any,
    /// according to the priorities of the individual write operations on
    /// the write queue. The integrity of the entire sequence of the 'data'
    /// is always preserved when transmitting the data stream, even when
    /// other data is sent concurrently by different threads, although the
    /// order of transmission of the entirety of the 'data' in relation to
    /// other transmitted data is unspecified. If enqueuing the 'data' onto
    /// the write queue causes the write queue high watermark to become
    /// breached, announce a write queue high watermark event but continue
    /// to queue the 'data' for transmission. After a write queue high
    /// watermark event is announced, announce a write queue low watermark
    /// event when the write queue is subsequently and asynchronously
    /// drained down to the write queue low watermark. A write queue high
    /// watermark event must be first announced before a write queue low
    /// watermark event will be announced, and thereafter a write queue
    /// low watermark event must be announced before a subsequent write
    /// queue high watermark event will be announced. When the 'data' has
    /// been completly copied to the send buffer, invoke the specified
    /// 'callback' on the callbacks's strand, if any. Return the error,
    /// notably 'ntsa::Error::e_WOULD_BLOCK' if the size of the write queue
    /// has already breached the write queue high watermark. All other
    /// errors indicate the socket is incapable of transmitting data at this
    /// time or any time in the future. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    /// Also note that 'ntsa::Data' is a container of data with many
    /// possibly representations and ownership semantics, and the 'data'
    /// will be queued, copied, shared, or held according to the user's
    /// choice of representation.
    virtual ntsa::Error send(const ntsa::Data&         data,
                             const ntca::SendOptions&  options,
                             const ntci::SendCallback& callback) = 0;

    /// Dequeue received data according to the specified 'options'. If the
    /// read queue has sufficient size to fill the 'data', synchronously
    /// copy the read queue into the specified 'data'. Otherwise,
    /// asynchronously copy the socket receive buffer onto the read queue
    /// as data in the socket receive buffer becomes available, at the
    /// configured read rate limit, if any, up to the read queue high
    /// watermark. After satisfying any queued read operations, when the
    /// read queue is asynchronously filled up to the read queue low
    /// watermark, announce a read queue low watermark event. When
    /// asynchronously enqueing data onto the read queue causes the read
    /// queue high watermark to become breached, stop asynchronously copying
    /// the socket receive buffer to the read queue and announce a read
    /// queue high watermark event. Return the error, notably
    /// 'ntsa::Error::e_WOULD_BLOCK' if neither the read queue nor the
    /// socket receive buffer has sufficient size to fill the 'data', or
    /// 'ntsa::Error::e_EOF' if the read queue is empty and the socket
    /// receive buffer has been shut down. All other errors indicate no more
    /// received data is available at this time or will become available in
    /// the future.
    virtual ntsa::Error receive(ntca::ReceiveContext*       context,
                                bdlbb::Blob*                data,
                                const ntca::ReceiveOptions& options) = 0;

    /// Dequeue received data according to the specified 'options'. If the
    /// read queue has sufficient size to satisfy the read operation,
    /// synchronously copy the read queue into an internally allocated data
    /// structure and invoke the specified 'callback' on the callback's
    /// strand, if any, with that data structure. Otherwise, queue the read
    /// operation and asynchronously copy the socket receive buffer onto the
    /// read queue as data in the socket receive buffer becomes available,
    /// at the configured read rate limit, if any, up to the read queue high
    /// watermark. When the read queue is asynchronously filled to a
    /// sufficient size to satisfy the read operation, synchronously copy
    /// the read queue into an internally allocated data structure and
    /// invoke the 'callback' on the callback's strand, if any, with that
    /// data structure. After satisfying any queued read operations, when
    /// the read queue is asynchronously filled up to the read queue low
    /// watermark, announce a read queue low watermark event. When
    /// asynchronously enqueing data onto the read queue causes the read
    /// queue high watermark to become breached, stop asynchronously copying
    /// the socket receive buffer to the read queue and announce a read
    /// queue high watermark event. Return the error, notably
    /// 'ntsa::Error::e_WOULD_BLOCK' if neither the read queue nor the
    /// socket receive buffer has sufficient size to synchronously satisfy
    /// the read operation, or 'ntsa::Error::e_EOF' if the read queue is
    /// empty and the socket receive buffer has been shut down. All other
    /// errors indicate no more received data is available at this time or
    /// will become available in the future. Note that callbacks created by
    /// this object will automatically be invoked on this object's strand
    /// unless an explicit strand is specified at the time the callback is
    /// created.
    virtual ntsa::Error receive(const ntca::ReceiveOptions&  options,
                                const ntci::ReceiveFunction& callback) = 0;

    /// Dequeue received data according to the specified 'options'. If the
    /// read queue has sufficient size to satisfy the read operation,
    /// synchronously copy the read queue into an internally allocated data
    /// structure and invoke the specified 'callback' on the callback's
    /// strand, if any, with that data structure. Otherwise, queue the read
    /// operation and asynchronously copy the socket receive buffer onto the
    /// read queue as data in the socket receive buffer becomes available,
    /// at the configured read rate limit, if any, up to the read queue high
    /// watermark. When the read queue is asynchronously filled to a
    /// sufficient size to satisfy the read operation, synchronously copy
    /// the read queue into an internally allocated data structure and
    /// invoke the 'callback' on the callback's strand, if any, with that
    /// data structure. After satisfying any queued read operations, when
    /// the read queue is asynchronously filled up to the read queue low
    /// watermark, announce a read queue low watermark event. When
    /// asynchronously enqueing data onto the read queue causes the read
    /// queue high watermark to become breached, stop asynchronously copying
    /// the socket receive buffer to the read queue and announce a read
    /// queue high watermark event. Return the error, notably
    /// 'ntsa::Error::e_WOULD_BLOCK' if neither the read queue nor the
    /// socket receive buffer has sufficient size to synchronously satisfy
    /// the read operation, or 'ntsa::Error::e_EOF' if the read queue is
    /// empty and the socket receive buffer has been shut down. All other
    /// errors indicate no more received data is available at this time or
    /// will become available in the future. Note that callbacks created by
    /// this object will automatically be invoked on this object's strand
    /// unless an explicit strand is specified at the time the callback is
    /// created.
    virtual ntsa::Error receive(const ntca::ReceiveOptions&  options,
                                const ntci::ReceiveCallback& callback) = 0;

    /// Register the specified 'resolver' for this socket. Return the error.
    virtual ntsa::Error registerResolver(
        const bsl::shared_ptr<ntci::Resolver>& resolver) = 0;

    /// Deregister the current resolver for this socket. Return the error.
    virtual ntsa::Error deregisterResolver() = 0;

    /// Register the specified 'manager' for this socket. Return the error.
    virtual ntsa::Error registerManager(
        const bsl::shared_ptr<ntci::DatagramSocketManager>& manager) = 0;

    /// Deregister the current manager or manager callback for this socket.
    /// Return the error.
    virtual ntsa::Error deregisterManager() = 0;

    /// Register the specified 'session' for this socket. Return the
    /// error.
    virtual ntsa::Error registerSession(
        const bsl::shared_ptr<ntci::DatagramSocketSession>& session) = 0;

    /// Register the specified session 'callback' for this socket to be
    /// invoked on this socket's strand. Return the error.
    virtual ntsa::Error registerSessionCallback(
        const ntci::DatagramSocket::SessionCallback& callback) = 0;

    /// Register the specified session 'callback' for this socket to be
    /// invoked on the specified 'strand'. Return the error.
    virtual ntsa::Error registerSessionCallback(
        const ntci::DatagramSocket::SessionCallback& callback,
        const bsl::shared_ptr<ntci::Strand>&         strand) = 0;

    /// Deregister the current session or session callback for this socket.
    /// Return the error.
    virtual ntsa::Error deregisterSession() = 0;

    /// Set the minimum number of bytes that must be available to send in order
    /// to attempt a zero-copy send to the specified 'value'. Return the error.
    virtual ntsa::Error setZeroCopyThreshold(bsl::size_t value);

    /// Set the write rate limiter to the specified 'rateLimiter'. Return
    /// the error.
    virtual ntsa::Error setWriteRateLimiter(
        const bsl::shared_ptr<ntci::RateLimiter>& rateLimiter) = 0;

    /// Set the write queue low watermark to the specified 'lowWatermark'.
    /// Return the error.
    virtual ntsa::Error setWriteQueueLowWatermark(
        bsl::size_t lowWatermark) = 0;

    /// Set the write queue high watermark to the specified 'highWatermark'.
    /// Return the error.
    virtual ntsa::Error setWriteQueueHighWatermark(
        bsl::size_t highWatermark) = 0;

    /// Set the write queue limits to the specified 'lowWatermark' and
    /// 'highWatermark'. Return the error.
    virtual ntsa::Error setWriteQueueWatermarks(bsl::size_t lowWatermark,
                                                bsl::size_t highWatermark) = 0;

    /// Set the read rate limiter to the specified 'rateLimiter'. Return
    /// the error.
    virtual ntsa::Error setReadRateLimiter(
        const bsl::shared_ptr<ntci::RateLimiter>& rateLimiter) = 0;

    /// Set the read queue low watermark to the specified 'lowWatermark'.
    /// Return the error.
    virtual ntsa::Error setReadQueueLowWatermark(bsl::size_t lowWatermark) = 0;

    /// Set the read queue high watermark to the specified 'highWatermark'.
    /// Return the error.
    virtual ntsa::Error setReadQueueHighWatermark(
        bsl::size_t highWatermark) = 0;

    /// Set the read queue limits to the specified 'lowWatermark' and
    /// 'highWatermark'. Return the error.
    virtual ntsa::Error setReadQueueWatermarks(bsl::size_t lowWatermark,
                                               bsl::size_t highWatermark) = 0;

    /// Set the flag that indicates multicast datagrams should be looped
    /// back to the local host to the specified 'value'. Return the error.
    virtual ntsa::Error setMulticastLoopback(bool value) = 0;

    /// Set the maximum number of hops over which multicast datagrams
    /// should be forwarded to the specified 'value'. Return the error.
    virtual ntsa::Error setMulticastTimeToLive(bsl::size_t value) = 0;

    /// Set the address of the network interface on which multicast
    /// datagrams should be sent to the specified 'value'. Return the error.
    virtual ntsa::Error setMulticastInterface(
        const ntsa::IpAddress& value) = 0;

    /// Issue an IGMP message to add this datagram socket to the
    /// specified multicast 'group' on the adapter identified by the
    /// specified 'interface'. Return the error.
    virtual ntsa::Error joinMulticastGroup(const ntsa::IpAddress& interface,
                                           const ntsa::IpAddress& group) = 0;

    /// Issue an IGMP message to remove this datagram socket from the
    /// specified multicast 'group' on the adapter identified by the
    /// specified 'interface'. Return the error.
    virtual ntsa::Error leaveMulticastGroup(const ntsa::IpAddress& interface,
                                            const ntsa::IpAddress& group) = 0;

    /// Request the implementation to start timestamping outgoing data if the
    /// specified 'enable' flag is true. Otherwise, request the implementation
    /// to stop timestamping outgoing data. Return the error.
    virtual ntsa::Error timestampOutgoingData(bool enable);

    /// Request the implementation to start timestamping incoming data if the
    /// specified 'enable' flag is true. Otherwise, request the implementation
    /// to stop timestamping outgoing data. Return the error.
    virtual ntsa::Error timestampIncomingData(bool enable);

    /// Enable copying from the socket buffers in the specified 'direction'.
    virtual ntsa::Error relaxFlowControl(
        ntca::FlowControlType::Value direction) = 0;

    /// Disable copying from socket buffers in the specified 'direction'
    /// according to the specified 'mode'.
    virtual ntsa::Error applyFlowControl(
        ntca::FlowControlType::Value direction,
        ntca::FlowControlMode::Value mode) = 0;

    /// Cancel the bind operation identified by the specified 'token'.
    /// Return the error.
    virtual ntsa::Error cancel(const ntca::BindToken& token) = 0;

    /// Cancel the connect operation identified by the specified 'token'.
    /// Return the error.
    virtual ntsa::Error cancel(const ntca::ConnectToken& token) = 0;

    /// Cancel the send operation identified by the specified 'token'.
    /// Return the error.
    virtual ntsa::Error cancel(const ntca::SendToken& token) = 0;

    /// Cancel the receive operation identified by the specified 'token'.
    /// Return the error.
    virtual ntsa::Error cancel(const ntca::ReceiveToken& token) = 0;

    /// Shutdown the datagram socket in the specified 'direction' according
    /// to the specified 'mode' of shutdown. Return the error.
    virtual ntsa::Error shutdown(ntsa::ShutdownType::Value direction,
                                 ntsa::ShutdownMode::Value mode) = 0;

    /// Close the datagram socket.
    virtual void close() = 0;

    /// Close the datagram socket and invoke the specified 'callback' on
    /// the callback's strand, if any, when the datagram socket is closed.
    /// Note that callbacks created by this object will automatically be
    /// invoked on this object's strand unless an explicit strand is
    /// specified at the time the callback is created.
    virtual void close(const ntci::CloseFunction& callback) = 0;

    /// Close the datagram socket and invoke the specified 'callback' on
    /// the callback's strand, if any, when the datagram socket is closed.
    /// Note that callbacks created by this object will automatically be
    /// invoked on this object's strand unless an explicit strand is
    /// specified at the time the callback is created.
    virtual void close(const ntci::CloseCallback& callback) = 0;

    /// Return the handle to the descriptor.
    virtual ntsa::Handle handle() const = 0;

    /// Return the transport of the datagram socket.
    virtual ntsa::Transport::Value transport() const = 0;

    /// Return the source endpoint.
    virtual ntsa::Endpoint sourceEndpoint() const = 0;

    /// Return the remote endpoint.
    virtual ntsa::Endpoint remoteEndpoint() const = 0;

    /// Return the strand that guarantees sequential, non-current execution
    /// of arbitrary functors on the unspecified threads processing events
    /// for this object.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const = 0;

    /// Return the handle of the thread that manages this socket, or
    /// the default value if no such thread has been set.
    virtual bslmt::ThreadUtil::Handle threadHandle() const = 0;

    /// Return the index in the thread pool of the thread that manages this
    /// socket, or 0 if no such thread has been set.
    virtual bsl::size_t threadIndex() const = 0;

    /// Return the current number of bytes queued after being copied from
    /// the socket receive buffer. These bytes have been received by
    /// the operating system buffers but not yet received by the user.
    virtual bsl::size_t readQueueSize() const = 0;

    /// Return the current read queue low watermark.
    virtual bsl::size_t readQueueLowWatermark() const = 0;

    /// Return the current read queue high watermark.
    virtual bsl::size_t readQueueHighWatermark() const = 0;

    /// Return the current number of bytes queued to be copied to the
    /// socket send buffer. These bytes have by sent by the user but not
    /// yet sent to the operating system.
    virtual bsl::size_t writeQueueSize() const = 0;

    /// Return the current write queue low watermark.
    virtual bsl::size_t writeQueueLowWatermark() const = 0;

    /// Return the current write queue high watermark.
    virtual bsl::size_t writeQueueHighWatermark() const = 0;

    /// Return the total number of bytes copied to the socket send buffer.
    virtual bsl::size_t totalBytesSent() const = 0;

    /// Return the total number of bytes copied from the socket receive
    /// buffer.
    virtual bsl::size_t totalBytesReceived() const = 0;
};

/// Provide a guard to automatically close a datagram socket
/// and block until the datagram socket is asynchronously closed.
class DatagramSocketCloseGuard
{
    bsl::shared_ptr<ntci::DatagramSocket> d_datagramSocket_sp;

  private:
    DatagramSocketCloseGuard(const DatagramSocketCloseGuard&)
        BSLS_KEYWORD_DELETED;
    DatagramSocketCloseGuard& operator=(const DatagramSocketCloseGuard&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Post to the specified 'semaphore'.
    static void complete(bslmt::Semaphore* semaphore);

  public:
    /// Create a new datagram socket guard for the specified
    /// 'datagramSocket', if any.
    explicit DatagramSocketCloseGuard(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket);

    /// Destroy this object: close the managed datagram socket, if any, and
    /// block until the datagram socket is asynchronously closed.
    ~DatagramSocketCloseGuard();

    /// Release the guard and return the guarded object.
    bsl::shared_ptr<ntci::DatagramSocket> release();
};

}  // close package namespace
}  // close enterprise namespace
#endif
