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

#ifndef INCLUDED_NTCI_STREAMSOCKET
#define INCLUDED_NTCI_STREAMSOCKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_connectoptions.h>
#include <ntca_connecttoken.h>
#include <ntca_flowcontrolmode.h>
#include <ntca_flowcontroltype.h>
#include <ntca_receiveoptions.h>
#include <ntca_receivetoken.h>
#include <ntca_sendoptions.h>
#include <ntca_sendtoken.h>
#include <ntca_streamsocketevent.h>
#include <ntccfg_platform.h>
#include <ntci_bindable.h>
#include <ntci_closable.h>
#include <ntci_closecallback.h>
#include <ntci_closecallbackfactory.h>
#include <ntci_connectcallback.h>
#include <ntci_connectcallbackfactory.h>
#include <ntci_connector.h>
#include <ntci_datapool.h>
#include <ntci_encryption.h>
#include <ntci_encryptioncertificate.h>
#include <ntci_encryptionkey.h>
#include <ntci_executor.h>
#include <ntci_listenersocket.h>
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
#include <ntci_timer.h>
#include <ntci_timerfactory.h>
#include <ntci_upgradable.h>
#include <ntcscm_version.h>
#include <ntsa_buffer.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_shutdownmode.h>
#include <ntsa_shutdowntype.h>
#include <ntsi_descriptor.h>
#include <ntsi_streamsocket.h>
#include <bdlbb_blob.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadutil.h>
#include <bsl_functional.h>
#include <bsl_memory.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntci {
class StreamSocketManager;
}
namespace ntci {
class StreamSocketSession;
}
namespace ntci {

/// Provide an interface to an asynchronous stream socket.
///
/// @details
/// Stream sockets provide connection-oriented, reliable, streaming unicast
/// communication between two endpoints. An 'ntci::StreamSocket' behaves
/// fundamentally like an operating system stream socket with the following
/// enhancements:
///
/// 1. All operations on a 'ntci::StreamSocket' may be performed
///    asynchronously, rather than simply blocking or non-blocking.
///
/// 2. Users may interact with an 'ntci::StreamSocket' either "reactively"
///    (i.e. similarly to the Unix readiness model exemplified by 'select' and
///    'poll') or "proactively" (i.e. similarly to the Windows I/O completion
///    port model), regardless of whether the implementation details of the
///    interface to the operating system operates "reactively" or
///    "proactively".
///
/// 3. Each 'ntci::StreamSocket' maintains a "write queue" that extends the
///    concept of the operating system socket send buffer. The write queue
///    buffers an arbitrary amount of outgoing data up to a configurable limit
///    specified by the user. The write queue provides greater tolerance of
///    a discrepency between the rate of transmission and the rate of
///    reception, enables multiple ordered, concurrent transmissions, and
///    guarantees the data of each transmission is not interleaved to the data
///    stream sent to the peer of the socket.
///
/// 4. Each 'ntci::StreamSocket' maintains a "read queue" that extends the
///    concept of the operating system socket receive buffer. The read queue
///    buffers an arbitrary amount of incoming data up to a configurable limit
///    specified by the user. The read queue provides greater tolerance of a
///    discrepency between the rate of reception and the rate of transmission,
///    enables multiple ordered, concurrent receptions, and guarantees the data
///    of each reception is not interleaved from the data stream received from
///    the peer of the socket.
///
/// 5. Users may explicitly cancel and/or specified deadlines (i.e. "timeout")
///    when initiating connect, send, receive, and close operations.
///
/// An 'ntci::StreamSocket' operates over either the Transmission Control
/// Protocol (TCP) transport over an Internet Protocol (IP) version 4 or version
/// 6 network, or, on Unix-like platforms, using a local (a.k.a. Unix) domain
/// transport. The type of transport is either chosen at the time the socket is
/// opened, or alternatively, at the time the socket is explicitly bound to an
/// endpoint on the local host or connected to a peer on a remote host.
///
/// For a popular explanation of sockets, address families, protocols, see
/// "UNIX Network Programming, Volume 1: The Sockets Networking API", by
/// W. Richard Stevens.
///
/// Essentially, an 'ntci::StreamSocket' implements the Berkeley Sockets API
/// for stream sockets with an asynchronous API rather than a non-blocking
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
/// An 'ntci::StreamSocket' is always connection-oriented so must become bound
/// to an endpoint on the local host before or during the establishment of a
/// connection to a peer. Users are not required to explicitly bind a socket
/// before connecting it to a peer, but may choose to do so if that behavior is
/// required. Users may specify the endpoint to which an 'ntci::StreamSocket'
/// is bound either explicitly, in the form of an exact 'ntsa::Endpoint', or
/// implicitly, in the form of a Domain name and optional service name, both of
/// which are automatically and asynchronously resolved to a specific
/// 'ntsa::Endpoint' to which the socket becomes bound.
///
/// @par Connecting to a Remote Endpoint
/// An 'ntci::StreamSocket' is always connection-oriented. That is, before data
/// may be sent to or received from a peer, it must be connected to the enpoint
/// of that peer. The connection persists for the lifetime of the stream
/// socket. If the 'ntci::StreamSocket' is not explicitly bound to a local
/// endpoint before the connection is initiated, the socket becomes implicitly
/// bound to a suitable local endpoint during the course of establishing the
/// connection. Users may specify the endpoint to which an 'ntci::StreamSocket'
/// is connected either explicitly, in the form of an exact 'ntsa::Endpoint',
/// or implicitly, in the form of a Domain name and optional service name, both
/// of which are automatically and asynchronously resolved to a specific
/// 'ntsa::Endpoint' to which the socket becomes connected. Users may choose to
/// configure an 'ntci::StreamSocket' such that connection attempts are
/// automatically retried, up to a certain number of attempts, according to a
/// certain period that is independant from any transport-level connection
/// timeout.
///
/// @par Ordered Concurrent Writes
/// Operating system stream sockets have a distinction of having stream
/// semantics as opposed to datagram semantics. With stream semantics the
/// network transport sends and receives data potentially arbitrary-sized
/// chunks, regardless of the amount of data specified when a transmission is
/// initiated. Because of these stream semantics, it is difficult to implement
/// multiple senders concurrently attempting to copy to an operating system
/// socket send buffer, because those writes may only partially complete and
/// must be carefully ordered to prevent corrupting the data stream (if, as is
/// typical, the data stream has some sort of integrity, such as is the case
/// when using a stream socket to send and receive a higher-level application
/// messaging protocol, such as DNS or HTTP). Each 'ntci::StreamSocket'
/// supports multiple, concurrent senders and guarantees the data specified to
/// each 'send' operation is atomically written without being interleaved with
/// the data from any other concurrent 'send' operation.
///
/// @par The Write Queue
/// Each 'ntci::StreamSocket' maintains a "write queue", which conceptually
/// extends of the operating system socket send buffer. If data being sent
/// cannot be immediately and entirely copied to the operating system socket
/// send buffer, the remainder is automatically enqueued to the back of the
/// write queue until capacity in the operating system socket send buffer
/// becomes available. The maximum size of an operating system socket buffer is
/// typically rather small, sometimes much smaller than the size of messages of
/// a user's application message protocol. The operating system send buffer can
/// become full when the rate of data being sent is greater than the rate of
/// the operating system to process the transmission of the transport protocol
/// and ready the data to be transmitted by the network hardware, or (more
/// typically) when the remote receiver is receiving data slower than the rate
/// at which data is being sent and the transport protocol uses a positive
/// acknowledgement-like system, as TCP does. The write queue introduces a
/// greater tolerance for transient differences in the rate of senders and
/// receivers, and allows the implementation of the 'ntci::StreamSocket' to
/// itself take care of the details to correctly allow concurrent, atomic
/// writes from multiple threads, the implementation of which at least requires
/// all senders to be aware of each other to be coordinated once the operating
/// system send buffer becomes full; a requirement not easily satisfied in many
/// applications. The greater tolerance of queued data results in larger
/// variance in latency, this tradeoff can be bounded by the user configuring a
/// small maximum size for the write queue rather than a large one. Regardless
/// of the configured maximum size of the write queue, an 'ntci::StreamSocket'
/// always permits a single 'send' of arbitrary size to be enqueued in the
/// write queue.
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
/// @par Ordered Concurrent Reads
/// Typical usage of an 'ntci::StreamSocket' is to receive data structured by
/// some higher-level application messaging protocol. Such message protocols
/// usually represent each message as a composition of multiple, variably-sized
/// parts, some parts (usually the first) describing meta-data and later parts
/// describing the payload. These sections in these message protocols sometimes
/// delimited by some byte sequence (e.g. HTTP), but a common protocol design
/// consists of a leading fixed-size header that describes the length of
/// subsequent parts. Users of an 'ntci::StreamSocket' can leverage the read
/// queue to initially request a read of the size of the fixed-length header,
/// then subsequently (and perhaps concurrently) request the reads of each
/// following variably-sized section of the message. This type of reading is
/// facilitated by the read queue.
///
/// @par The Read Queue
/// Each 'ntci::StreamSocket' mantains a "read queue" which conceptually
/// extends the operation system socket receive buffer. The automatic operation
/// of the 'ntci::StreamSocket' results in data being copied from the socket
/// receive buffer and enqueued to the read queue, which is then available for
/// the user to dequeue. The implementation of the 'ntci::StreamSocket' takes
/// care of the details of how and when data is copied from the socket send
/// buffer. The maximum size of an operating system socket send buffer is
/// typically rather small, sometimes much smaller than individual, coherent
/// portions of a user's application message protocol. The read queue allows
/// the user to specify criteria for the size of the read queue for a receive
/// operation to complete.
///
/// @par Receiving Data
/// The 'receive' function copies data from the read queue and may be called
/// with either non-blocking semantics or asynchronous semantics, according to
/// whether a callback is also specified. As applied here, the term
/// "non-blocking" means "instantaneously tried" and "asynchronous" means
/// "queued for later completion". The read queue is asynchronously and
/// automatically filled by draining the socket receive buffer as data is
/// received from its source. The 'receive' function returns the following
/// synchronous error codes.
///
/// - ntsa::Error::e_OK:          The receive operation succeeded: the read
///                               queue contains at least the amount of data
///                               desired to receive.
///
/// - ntsa::Error::e_WOULD_BLOCK: The receive operation failed: The current
///                               size of the read queue does not satify the
///                               criteria for the receive operation to
///                               complete successfully. If a callback is
///                               specified, that callback will be
///                               asynchronously invoked if and when the read
///                               queue becomes filled with satisfactory amount
///                               of data. Future receive operations may
///                               succeed, but if this operation is queued,
///                               they will not complete before this operation.
///
/// - ntsa::Error::e_EOF:         The receive operation failed and all future
///                               receive operations will also fail: The socket
///                               has been shut down for receiving.
///
/// - All others:                 The receive operation failed and all future
///                               receive operations will also fail, at least
///                               initially for the reason described by the
///                               error code.
///
/// A callback may be optionally specified to a 'receive' function.  If such a
/// callback is supplied, this callback is invoked under the following
/// conditions with the following asynchronous error codes:
///
/// - ntsa::Error::e_OK:          The receive operation succeeded and the
///                               indicated data has been dequeued from the
///                               read queue.
///
/// - ntsa::Error::e_CANCELLED:   The receive operation failed because it was
///                               explicitly cancelled: The 'cancel' function
///                               was called with the receive token specified
///                               in the options at the time the receive
///                               operation before the receive criteria could
///                               be satisfied.
///
/// - ntsa::Error::e_WOULD_BLOCK: The receive operation failed because it
///                               implicitly timed out: The deadline specified
///                               in the receive options at the time the
///                               operation was initiated elapsed before the
///                               receive criteria could be satisfied.
///
/// - ntsa::Error::e_EOF:         The receive operation failed and all future
///                               receive operations will also fail: The socket
///                               has been shut down for receiving.
///
/// - All others:                 The receive operation failed and all future
///                               receive operations will also fail, at least
///                               initially for the reason described by the
///                               error code.
///
/// Note that only the 'ntsa::Error::e_OK' code indicates "success": the data
/// has been entirely copied from the read queue. All other errors indicate
/// "failure" but must be interpreted to understand if future receive
/// operations may succeed. The 'ntsa::Error::e_WOULD_BLOCK' and
/// 'ntsa::Error::e_CANCELLED' error codes indicate nothing has been dequeued
/// from the read queue and subsequent receive operations may succeed. All
/// other errors indicate the receive operation failed and all subsequent
/// receive operations will fail.
///
/// @par Reacting to Passive Events
/// Certain events for an 'ntci::StreamSocket' occur passively, either because
/// they may be initiated by the remote peer (such as the peer shutting down
/// writes from its side of the connection) or because the events occur
/// asynchronously during the automatic operation of the 'ntci::StreamSocket'
/// (such as reaching the high and low watermarks of the read and write queue.)
/// Users may register their interest to become asynchronously notified of these
/// type of events either by registering a callback or an implementation of
/// 'ntci::StreamSocketSession'.
///
/// @par Shutting Down
/// Connection-oriented sockets like 'ntci::StreamSocket' support a two-way
/// shutdown sequence. Each side of the connection may initiate the shutdown
/// sequence by shutting down writes from its side of the connection. Shutting
/// down a socket for writing does not prevent it from reading. When a remote
/// peer shuts down writes from its side of the connection, the shutdown is
/// detected by a receive operation failing with the 'ntsa::Error::e_EOF' error
/// code or a receive callback being invoked with the same code. The connection
/// is completely shut down. Closing a socket implictly shuts down writes from
/// its side of the connection. Explicitly shutting down a socket is optional.
/// Sockets may be configured to automatically shut down both reading and
/// writing whenever the socket is shut down locally or by the remote peer.
/// When explicitly shutting down writes, the user may choose to shut down
/// gracefully: such a shutdown ensure that any pending data in the write queue
/// is first copied to the operating system for transmission before the socket
/// is actually shut down. Furthermore, sockets may be configured to
/// "abortively" close, which for some types of transports such as TCP,
/// prevents the operating system from lingering the socket in certain states
/// after userspace as indicated the socket should be closed.
///
/// @par Closing
/// Each 'ntci::StreamSocket' is shared between the user and this library's
/// asynchronous machinery. It is not sufficient for users to simply release
/// their reference counts on a stream socket object to close and destroy it.
/// Users *must* explicitly close each 'ntci::StreamSocket'. Closing a socket
/// implicitly shuts it down, unless the socket is already shut down or an
/// "abortive close" is configured. Closing a socket is asynchronous, users
/// must wait until the close callback is invoked before assuming the socket is
/// completely closed. After a socket's close callback is invoked, the socket
/// remains in a valid state but all member functions with failure modes
/// will return an error. The socket object will be destroyed only after it has
/// been closed and all references are released.
///
/// @par Asynchronous Operation
/// Asynchronous operation is classified into two categories: operations that
/// are explicitly initiated, and events that passively occur during the
/// asynchronous operation of the socket.
///
/// The operations 'bind', 'connect', 'send', 'receive', and 'close' are
/// explicitly initiated and always execute asynchronously. A function supplied
/// by the user at the time the operation is initiated is invoked when the
/// operation asychronous completes or fails. Each operation has a
/// corresponding "event" and completion "function", with the "function"
/// represented by either a 'bsl::function' or a composition this library terms
/// a "callback", that is a 'bsl::function', an 'ntci::Strand' on which that
/// function must be invoked (if any), and an 'ntci::Authorization' that may be
/// used to prevent the function from being invoked when the operation
/// completes (but note that de-authorizing a function does not cancel the
/// operation. See the section in this component document on operation
/// cancellation and callback authorization for more details.) For example,
/// when a 'send' operation is initiated the user supplies either a
/// 'bsl::function' or a 'ntci::SendCallback' (ideally created from the
/// 'ntci::StreamSocket::createSendCallback' factory function.) The ability to
/// provide a basic 'bsl::function' when an operation is initiated is provided
/// only for convenience, for those users that do not care or wish to control
/// strand semantics or callback function de-authorization. When an
/// asynchronous operation is initiated and a 'bsl::function' is supplied, that
/// function is executed on the strand associated with the socket object, if
/// any. Users who wish to control strand semantics or callback function
/// de-authorization are encouraged to supply the "callback" objects when these
/// types of asynchronous operations are initiated.
///
/// While the operations 'bind', 'connect', 'send', 'receive' and 'close' are
/// explicitly initiated, other events passively occur during the asynchronous
/// operation of a socket. An example of such an event is the write queue low
/// watermark event. When a user attempts to send data and a portion of that
/// data becomes queued that causes the resulting queue size to breach the
/// write queue high watermark limit, an "high watermark" event is announced.
/// As data from this queue is asynchronously copied to the socket send buffer,
/// this queue is drained and a "low watermark" event is announced once the
/// queue is reduced down to the low watermark limit. Users can register their
/// interest to receive an announcement when these watermark events and other
/// similar events occur. Interest is registered by calling 'registerSession'
/// supplying either an implementation of the 'ntci::StreamSocketSession'
/// abstract class, or a 'bsl::function'.
///
/// @par Sesssions vs. Managers
/// Certain events occurs asynchronous during the operation of a socket. Users
/// can register their interest in this events by calling 'registerSession'.
/// Users can also optionally register an implementation of the
/// 'ntci::StreamSocketManager' abstract class to listen for a subset of these
/// events pertaining to the lifetime of the underlying operatin system socket
/// or sockets. This design is facilitates the implementation of some
/// applications, where their is a "client" or "server" (acting as the
/// "manager") each managing multiple sockets (each having a "session").
///
/// @par Operation Timeouts
/// Each 'bind', 'connect', 'send', and 'receive' operation can have a deadline
/// associated with it. The operation automatically fails if it does not
/// complete by that deadline and its associated callback function is invoked
/// with an event context containing an error that indicates the operation
/// timed out.  The failure of a 'bind' or 'connect' operation to complete by
/// its deadline results in the socket being automatically closed. A 'send'
/// operation will only 'timeout' if the entirety of its data is queued in the
/// write queue when its deadline is reached. In other words, once a portion of
/// the data of a send operation is copied to the socket send buffer the
/// entirety of the remaining data must also be copied to retain the integrity
/// of the data stream.
///
/// @par Operation Cancellation
/// Each 'bind', 'connect', 'send' and 'receive' operation may be explicitly
/// cancelled. When an operation is successfully cancelled its associated
/// callback function is invoked with an event context containing an error that
/// indicates the operation was cancelled. The cancellation of a 'bind' or
/// 'connect' operation results in the socket being automatically closed. A
/// 'send' operation may only be successfully cancelled if the entirety of its
/// data is queued in the write queue when its deadline is reached. In other
/// words, once a portion of the data of a send operation is copied to the
/// socket send buffer the entirety of the remaining data must also be copied
/// to retain the integrity of the data stream.
///
/// @par Operation Callback Authorization
/// Each 'bind', 'connect', 'send', and 'receive' operation may have an
/// implementation of an 'ntci::Authorization' abstract class associated with
/// its callback function. See the "Asynchronous Operation" section of this
/// component documentation for more explanation of an operation's "callback".
/// Users may de-authorize the invocation of an operation's callback function,
/// which prevents that function from being invoked when an operation
/// completes. It is important to note that this authorization feature is
/// different from cancellation; a de-authorized callback does not cancel the
/// operation (i.e. it still may affect the system) the user is simply choosing
/// the ignore its completion.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @par Usage Example
/// This example illustrates how to listen for connections, connect a socket,
/// accept a socket, exchange data between those two sockets, gracefully shut
/// down each socket and detect the shutdown of each peer, then close each
/// socket. Note that all operations execute asynchronously, but for the
/// purposes of this example, the thread that initiates each asynchronous
/// operation blocks on a semaphore (posted by each asynchronous operation's
/// callback), to illustrate the functionality in a linear fashion despite the
/// operations executing asynchronously. This example shows how to create and
/// use TCP/IPv4 sockets, but the usage for TCP/IPv6 sockets and Unix domain
/// sockets is identical, with the only difference being the type of transport
/// specified in the options used to construct the socket and/or the type of
/// endpoint to which a socket is bound or connected.
///
/// First, initialize the library.
///
///     ntcf::System::initialize();
///     ntcf::System::ignore(ntscfg::Signal::e_PIPE);
///
/// Next, acquire implementations of the 'ntci::ListenerSocketFactory' and
/// 'ntci::StreamSocketFactory' abstract classes. For this example, create and
/// start an 'ntci::Interface', which implements both those abstract classes.
/// Note that stream sockets and listener sockets may also be created from
/// other objects; see the component documentation for more details.
///
///     bsl::shared_ptr<ntci::Interface> interface =
///         ntcf::System::createInterface(ntca::InterfaceConfig());
///
///     interface->start();
///
/// Declare an error used to store the synchronous result of each operation,
/// and a semaphore used to serialize the execution of each asynchronous
/// operation.
///
///     ntsa::Error      error;
///     bslmt::Semaphore semaphore;
///
/// Create a listener socket.
///
///     bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
///         interface->createListenerSocket(ntca::ListenerSocketOptions());
///
/// Bind the listener socket to any endpoint on the local host and wait for
/// the operation to complete.
///
///     error = listenerSocket->bind(
///         ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::any(), 0)),
///         ntca::BindOptions(),
///         [&](auto bindable, auto event) {
///             BSLS_ASSERT(bindable == listenerSocket);
///             BSLS_ASSERT(event.isComplete());
///             semaphore.post();
///         });
///
///     BSLS_ASSERT(!error);
///     semaphore.wait();
///
/// Begin listening.
///
///     error = listenerSocket->listen();
///     BSLS_ASSERT(!error);
///
/// Create a stream socket to act as a client.
///
///     bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket =
///         interface->createStreamSocket(ntca::StreamSocketOptions());
///
/// Connect the client stream socket to the endpoint of the listener socket
/// and wait for the operation to complete.
///
///     error = clientStreamSocket->connect(
///         listenerSocket->sourceEndpoint(),
///         ntca::ConnectOptions(),
///         [&](auto connector, auto event) {
///             BSLS_ASSERT(connector == clientStreamSocket);
///             BSLS_ASSERT(event.isComplete());
///             semaphore.post();
///         });
///
///     BSLS_ASSERT(!error);
///     semaphore.wait();
///
/// Accept a stream socket to act as the server and wait for the operation to
/// complete.
///
///     bsl::shared_ptr<ntci::StreamSocket> serverStreamSocket;
///     error = listenerSocket->accept(
///         ntca::AcceptOptions(),
///         [&](auto acceptor, auto streamSocket, auto event) {
///             BSLS_ASSERT(acceptor == listenerSocket);
///             BSLS_ASSERT(event.isComplete());
///             serverStreamSocket = streamSocket;
///             semaphore.post();
///         });
///
///     BSLS_ASSERT(!error);
///     semaphore.wait();
///
/// Send data from the client stream socket to the server stream socket and
/// wait for the operation to complete.
///
///     const char CLIENT_SEND_DATA[] = "Hello, server!";
///
///     bdlbb::Blob clientSendData(
///         clientStreamSocket->outgoingBlobBufferFactory().get());
///
///     bdlbb::BlobUtil::append(
///         &clientSendData, CLIENT_SEND_DATA, sizeof CLIENT_SEND_DATA - 1);
///
///     error = clientStreamSocket->send(
///         clientSendData,
///         ntca::SendOptions(),
///         [&](auto sender, auto event) {
///             BSLS_ASSERT(sender == clientStreamSocket);
///             BSLS_ASSERT(event.isComplete());
///             semaphore.post();
///         });
///
///     BSLS_ASSERT(!error);
///     semaphore.wait();
///
/// Receive data at the server stream socket and wait for the operation to
/// complete. Require that exactly the amount of data sent by the client has
/// been received for the operation to complete.
///
///     ntca::ReceiveOptions serverReceiveOptions;
///     serverReceiveOptions.setMinSize(clientSendData.length());
///     serverReceiveOptions.setMaxSize(clientSendData.length());
///
///     error = serverStreamSocket->receive(
///         serverReceiveOptions,
///         [&](auto receiver, auto data, auto event) {
///             BSLS_ASSERT(receiver == serverStreamSocket);
///             BSLS_ASSERT(event.isComplete());
///             BSLS_ASSERT(data);
///             BSLS_ASSERT(bdlbb::BlobUtil::compare(*data, clientSendData)
///                         == 0);
///             semaphore.post();
///         });
///
///     BSLS_ASSERT(!error);
///     semaphore.wait();
///
/// Send data from the server stream socket to the client stream socket and
/// wait for the operation to complete.
///
///     const char SERVER_SEND_DATA[] = "Hello, client!";
///
///     bdlbb::Blob serverSendData(
///         serverStreamSocket->outgoingBlobBufferFactory().get());
///
///     bdlbb::BlobUtil::append(
///         &serverSendData, SERVER_SEND_DATA, sizeof SERVER_SEND_DATA - 1);
///
///     error = serverStreamSocket->send(
///         serverSendData,
///         ntca::SendOptions(),
///         [&](auto sender, auto event) {
///             BSLS_ASSERT(sender == serverStreamSocket);
///             BSLS_ASSERT(event.isComplete());
///             semaphore.post();
///         });
///
///     BSLS_ASSERT(!error);
///     semaphore.wait();
///
/// Receive data at the client stream socket and wait for the operation to
/// complete. Require that exactly the amount of data sent by the server has
/// been received for the receive operation to complete.
///
///     ntca::ReceiveOptions clientReceiveOptions;
///     clientReceiveOptions.setMinSize(serverSendData.length());
///     clientReceiveOptions.setMaxSize(serverSendData.length());
///
///     error = clientStreamSocket->receive(
///         clientReceiveOptions,
///         [&](auto receiver, auto data, auto event) {
///             BSLS_ASSERT(receiver == clientStreamSocket);
///             BSLS_ASSERT(event.isComplete());
///             BSLS_ASSERT(data);
///             BSLS_ASSERT(bdlbb::BlobUtil::compare(*data, serverSendData)
///                         == 0);
///             semaphore.post();
///         });
///
///     BSLS_ASSERT(!error);
///     semaphore.wait();
///
/// Shutdown the client socket.
///
///     error = clientStreamSocket->shutdown(ntsa::ShutdownType::e_SEND,
///                                          ntsa::ShutdownMode::e_GRACEFUL);
///     BSLS_ASSERT(!error);
///
/// Receive data at the server stream socket to detect the shutdown by
/// the client stream socket and wait for the operation to complete, but note
/// the shutdown by the client stream may have already been detected, in which
/// case the receive operation synchronously fails with ntsa::Error::e_EOF,
/// otherwise the shutdown is detected asynchronously.
///
///     error = serverStreamSocket->receive(
///         serverReceiveOptions,
///         [&](auto receiver, auto data, auto event) {
///             BSLS_ASSERT(receiver == serverStreamSocket);
///             BSLS_ASSERT(event.isError());
///             BSLS_ASSERT(event.context().error() ==
///                         ntsa::Error(ntsa::Error::e_EOF));
///             semaphore.post();
///         });
///
///     if (!error) {
///         semaphore.wait();
///     }
///     else {
///         BSLS_ASSERT(error == ntsa::Error(ntsa::Error::e_EOF));
///     }
///
/// Shutdown the server socket.
///
///     error = serverStreamSocket->shutdown(ntsa::ShutdownType::e_SEND,
///                                          ntsa::ShutdownMode::e_GRACEFUL);
///     BSLS_ASSERT(!error);
///
/// Receive data at the client stream socket to detect the shutdown by
/// the server stream socket and wait for the operation to complete, but note
/// the shutdown by the server stream may have already been detected, in which
/// case the receive operation synchronously fails with ntsa::Error::e_EOF,
/// otherwise the shutdown is detected asynchronously.
///
///     error = clientStreamSocket->receive(
///         clientReceiveOptions,
///         [&](auto receiver, auto data, auto event) {
///             BSLS_ASSERT(receiver == clientStreamSocket);
///             BSLS_ASSERT(event.isError());
///             BSLS_ASSERT(event.context().error() ==
///                         ntsa::Error(ntsa::Error::e_EOF));
///             semaphore.post();
///         });
///
///     if (!error || error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK)) {
///         semaphore.wait();
///     }
///     else {
///         BSLS_ASSERT(error == ntsa::Error(ntsa::Error::e_EOF));
///     }
///
/// Close the client stream socket and wait for the operation to complete.
///
///     clientStreamSocket->close([&]() {
///         semaphore.post();
///     });
///
///     semaphore.wait();
///
/// Close the server stream socket and wait for the operation to complete.
///
///     serverStreamSocket->close([&]() {
///         semaphore.post();
///     });
///
///     semaphore.wait();
///
/// Close the listener socket and wait for the operation to complete.
///
///     listenerSocket->close([&]() {
///         semaphore.post();
///     });
///
///     semaphore.wait();
///
/// Finally, stop the interface.
///
///     interface->shutdown();
///     interface->linger();
///
/// @ingroup module_ntci_socket
class StreamSocket : public ntsi::Descriptor,
                     public ntci::Executor,
                     public ntci::StrandFactory,
                     public ntci::TimerFactory,
                     public ntci::Closable,
                     public ntci::Bindable,
                     public ntci::Connector,
                     public ntci::Upgradable,
                     public ntci::Sender,
                     public ntci::Receiver,
                     public ntci::DataPool
{
  public:
    /// Define a type alias for a callback function invoked
    /// when an event occurs on a stream socket.
    typedef NTCCFG_FUNCTION(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::StreamSocketEvent&             event) SessionCallback;

    /// Destroy this object.
    virtual ~StreamSocket();

    /// Open the stream socket. Return the error. This function has no
    /// effect if this object is already open and not yet closed. This
    /// function fails if this object has been closed. This function fails
    /// if an effective transport is not somehow specified at the time this
    /// object was created.
    virtual ntsa::Error open() = 0;

    /// Open the stream socket using the specified 'transport'. Return the
    /// error. This function has no effect if this object is already open
    /// using the same 'transport' and not yet closed. This function fails
    /// if this object has been closed.
    virtual ntsa::Error open(ntsa::Transport::Value transport) = 0;

    /// Open the stream socket using the specified 'transport' imported
    /// from the specified 'handle'. Return the error. This function fails
    /// if this object is already open or if this object has been closed.
    /// The socket identified by the specified 'handle' will become
    /// effectively "owned" by this object and closed when this object is
    /// closed. After this function is called, the behavior of any operation
    /// on the 'handle', other than through this object's
    /// member functions, is not specified.
    virtual ntsa::Error open(ntsa::Transport::Value transport,
                             ntsa::Handle           handle) = 0;

    /// Open the stream socket using the specified 'transport' imported from
    /// the specified 'streamSocket'. Return the error. This function fails
    /// if this object is already open or if this object has been closed.
    /// The 'streamSocket' will become effectively "owned" by this object
    /// and closed when this object is closed. After this function is
    /// called, the behavior of any operation on the 'streamSocket', other
    /// than through this object's member functions, is not specified.
    virtual ntsa::Error open(
        ntsa::Transport::Value                     transport,
        const bsl::shared_ptr<ntsi::StreamSocket>& streamSocket) = 0;

    /// Open the stream socket using the specified 'transport' imported from
    /// the specified 'handle' accepted by the specified 'acceptor'. Return
    /// the error. This function fails if this object is already open or if
    /// this object has been closed. The socket identified by the specified
    /// 'handle' will become effectively "owned" by this object and closed
    /// when this object is closed. After this function is called, the
    /// behavior of any operation on the 'handle', other than through this
    /// object's member functions, is not specified.
    virtual ntsa::Error open(
        ntsa::Transport::Value                       transport,
        ntsa::Handle                                 handle,
        const bsl::shared_ptr<ntci::ListenerSocket>& acceptor) = 0;

    /// Open the stream socket using the specified 'transport' imported from
    /// the specified 'streamSocket' accepted by the specified 'acceptor'.
    /// Return the error. This function fails if this object is already open
    /// or if this object has been closed. The 'streamSocket' will become
    /// effectively "owned" by this object and closed when this object is
    /// closed. After this function is called, the behavior of any operation
    /// on the 'streamSocket', other than through this object's member
    /// functions, is not specified.
    virtual ntsa::Error open(
        ntsa::Transport::Value                       transport,
        const bsl::shared_ptr<ntsi::StreamSocket>&   streamSocket,
        const bsl::shared_ptr<ntci::ListenerSocket>& acceptor) = 0;

    /// Bind to the specified source 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on this object's strand,
    /// if any, when the socket has been bound or any error occurs. Return
    /// the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created. Also note
    /// that this function will implicitly open the socket if necessary.
    virtual ntsa::Error bind(const ntsa::Endpoint&     endpoint,
                             const ntca::BindOptions&  options,
                             const ntci::BindFunction& callback) = 0;

    /// Bind to the specified source 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on the callback's strand,
    /// if any, when the socket has been bound or any error occurs. Return
    /// the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created. Also note
    /// that this function will implicitly open the socket if necessary.
    virtual ntsa::Error bind(const ntsa::Endpoint&     endpoint,
                             const ntca::BindOptions&  options,
                             const ntci::BindCallback& callback) = 0;

    /// Bind to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on this
    /// object's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    /// Also note that this function will implicitly open the socket if
    /// necessary.
    virtual ntsa::Error bind(const bsl::string&        name,
                             const ntca::BindOptions&  options,
                             const ntci::BindFunction& callback) = 0;

    /// Bind to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    /// Also note that this function will implicitly open the socket if
    /// necessary.
    virtual ntsa::Error bind(const bsl::string&        name,
                             const ntca::BindOptions&  options,
                             const ntci::BindCallback& callback) = 0;

    /// Connect to the specified 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on this object's strand,
    /// if any, when the connection is established or an error occurs.
    /// Return the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created. Also note
    /// that this function will implicitly open the socket if necessary.
    virtual ntsa::Error connect(const ntsa::Endpoint&        endpoint,
                                const ntca::ConnectOptions&  options,
                                const ntci::ConnectFunction& callback) = 0;

    /// Connect to the specified 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on the callback's strand,
    /// if any, when the connection is established or an error occurs.
    /// Return the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created. Also note
    /// that this function will implicitly open the socket if necessary.
    virtual ntsa::Error connect(const ntsa::Endpoint&        endpoint,
                                const ntca::ConnectOptions&  options,
                                const ntci::ConnectCallback& callback) = 0;

    /// Connect to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on this
    /// objects's strand, if any, when the connection is established or an
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    /// Also note that this function will implicitly open the socket if
    /// necessary.
    virtual ntsa::Error connect(const bsl::string&           name,
                                const ntca::ConnectOptions&  options,
                                const ntci::ConnectFunction& callback) = 0;

    /// Connect to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the connection is established or an
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    /// Also note that this function will implicitly open the socket if
    /// necessary.
    virtual ntsa::Error connect(const bsl::string&           name,
                                const ntca::ConnectOptions&  options,
                                const ntci::ConnectCallback& callback) = 0;

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryption'. If the 'encryption' was created from an encryption
    /// client, the upgrade process will proceed in the client (i.e.
    /// connector) role. If the 'encryption' was created from an encryption
    /// server, the upgrade process will proceed in the server (i.e.
    /// acceptor) role. The upgrade process will re-use state cached from
    /// previous uses the 'encryption', if any. Invoke the specified
    /// 'callback' on this objects's strand, if any, when the upgrade is
    /// complete or any error occurs. Return the error. Note that callbacks
    /// created by this object will automatically be invoked on this
    /// object's strand unless an explicit strand is specified at the time
    /// the callback is created. Also note that this function will
    /// implicitly open the socket if necessary.
    virtual ntsa::Error upgrade(
        const bsl::shared_ptr<ntci::Encryption>& encryption,
        const ntca::UpgradeOptions&              options,
        const ntci::UpgradeFunction&             callback) = 0;

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryption'. If the 'encryption' was created from an encryption
    /// client, the upgrade process will proceed in the client (i.e.
    /// connector) role. If the 'encryption' was created from an encryption
    /// server, the upgrade process will proceed in the server (i.e.
    /// acceptor) role. The upgrade process will re-use state cached from
    /// previous uses the 'encryption', if any. Invoke the specified
    /// 'callback' on the callback's strand, if any, when the upgrade is
    /// complete or any error occurs. Return the error. Note that callbacks
    /// created by this object will automatically be invoked on this
    /// object's strand unless an explicit strand is specified at the time
    /// the callback is created.
    virtual ntsa::Error upgrade(
        const bsl::shared_ptr<ntci::Encryption>& encryption,
        const ntca::UpgradeOptions&              options,
        const ntci::UpgradeCallback&             callback) = 0;

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryptionClient'. The upgrade process will proceed in the client
    /// (i.e. connector) role. Invoke the specified 'callback' on this
    /// objects's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error upgrade(
        const bsl::shared_ptr<ntci::EncryptionClient>& encryptionClient,
        const ntca::UpgradeOptions&                    options,
        const ntci::UpgradeFunction&                   callback) = 0;

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryptionClient'. The upgrade process will proceed in the client
    /// (i.e. connector) role. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error upgrade(
        const bsl::shared_ptr<ntci::EncryptionClient>& encryptionClient,
        const ntca::UpgradeOptions&                    options,
        const ntci::UpgradeCallback&                   callback) = 0;

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryptionServer'. The upgrade process will proceed in the server
    /// (i.e. acceptor) role. Invoke the specified 'callback' on this
    /// objects's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error upgrade(
        const bsl::shared_ptr<ntci::EncryptionServer>& encryptionServer,
        const ntca::UpgradeOptions&                    options,
        const ntci::UpgradeFunction&                   callback) = 0;

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryptionServer'. The upgrade process will proceed in the server
    /// (i.e. acceptor) role. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error upgrade(
        const bsl::shared_ptr<ntci::EncryptionServer>& encryptionServer,
        const ntca::UpgradeOptions&                    options,
        const ntci::UpgradeCallback&                   callback) = 0;

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
    /// 'callback' on this object's strand, if any. Return the error,
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
    /// 'callback' on this object's strand, if any. Return the error,
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
    /// invoke the 'callback' on this objects's strand, if any, with that
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

    /// Register the specified 'resolver' for this socket to be used when
    /// a domain name or service name needs to be resolved when binding
    /// or connecting. Return the error.
    virtual ntsa::Error registerResolver(
        const bsl::shared_ptr<ntci::Resolver>& resolver) = 0;

    /// Deregister the current resolver for this socket. Return the error.
    virtual ntsa::Error deregisterResolver() = 0;

    /// Register the specified 'manager' for this socket to be invoked
    /// when the socket becomes established and closed. Return the error.
    virtual ntsa::Error registerManager(
        const bsl::shared_ptr<ntci::StreamSocketManager>& manager) = 0;

    /// Deregister the current manager or manager callback for this socket.
    /// Return the error.
    virtual ntsa::Error deregisterManager() = 0;

    /// Register the specified 'session' for this socket to be invoked
    /// for each event that passively occurs during the asynchronous
    /// operation of this object. Return the error.
    virtual ntsa::Error registerSession(
        const bsl::shared_ptr<ntci::StreamSocketSession>& session) = 0;

    /// Register the specified session 'callback' for this socket to be
    /// invoked on this socket's strand. Return the error.
    virtual ntsa::Error registerSessionCallback(
        const ntci::StreamSocket::SessionCallback& callback) = 0;

    /// Register the specified session 'callback' for this socket to be
    /// invoked on the specified 'strand'. Return the error.
    virtual ntsa::Error registerSessionCallback(
        const ntci::StreamSocket::SessionCallback& callback,
        const bsl::shared_ptr<ntci::Strand>&       strand) = 0;

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

    /// Request the implementation to start timestamping outgoing data if the
    /// specified 'enable' flag is true. Otherwise, request the implementation
    /// to stop timestamping outgoing data. Return the error.
    virtual ntsa::Error timestampOutgoingData(bool enable);

    /// Request the implementation to start timestamping incoming data if the
    /// specified 'enable' flag is true. Otherwise, request the implementation
    /// to stop timestamping outgoing data. Return the error.
    virtual ntsa::Error timestampIncomingData(bool enable);

    /// Enable copying from the socket buffers in the specified 'direction'.
    /// Return the error.
    virtual ntsa::Error relaxFlowControl(
        ntca::FlowControlType::Value direction) = 0;

    /// Disable copying from socket buffers in the specified 'direction'
    /// according to the specified 'mode'. Return the error.
    virtual ntsa::Error applyFlowControl(
        ntca::FlowControlType::Value direction,
        ntca::FlowControlMode::Value mode) = 0;

    /// Cancel the bind operation identified by the specified 'token'.
    /// Return the error. The attempt to cancel an operation fails if the
    /// token is not recognized or if the operation has already completed.
    /// Successful cancellation results in the socket being automatically
    /// closed.  Unsuccessful cancellation has no effect on the socket.
    virtual ntsa::Error cancel(const ntca::BindToken& token) = 0;

    /// Cancel the connect operation identified by the specified 'token'.
    /// Return the error. The attempt to cancel an operation fails if the
    /// token is not recognized or if the operation has already completed.
    /// Successful cancellation results in the socket being automatically
    /// closed.  Unsuccessful cancellation has no effect on the socket.
    virtual ntsa::Error cancel(const ntca::ConnectToken& token) = 0;

    /// Cancel the send operation identified by the specified 'token'.
    /// Return the error. The attempt to cancel an operation fails if the
    /// token is not recognized, or the operation has begun in
    /// a non-cancellable way (e.g. 'upgrade' operations cannot be cancelled
    /// once their data has begun to have been copied to the socket send
    /// buffer), or if the operation has already completed. Neither
    /// successful or unsuccessful cancellation has any effect on the
    /// socket.
    virtual ntsa::Error cancel(const ntca::UpgradeToken& token) = 0;

    /// Cancel the send operation identified by the specified 'token'.
    /// Return the error. The attempt to cancel an operation fails if the
    /// token is not recognized, or the operation has begun in
    /// a non-cancellable way (e.g. 'send' operations cannot be cancelled
    /// once their data has begun to have been copied to the socket send
    /// buffer), or if the operation has already completed. Neither
    /// successful or unsuccessful cancellation has any effect on the
    /// socket.
    virtual ntsa::Error cancel(const ntca::SendToken& token) = 0;

    /// Cancel the receive operation identified by the specified 'token'.
    /// Return the error. The attempt to cancel an operation fails if the
    /// token is not recognized or if the operation has already completed.
    /// Neither successful or unsuccessful cancellation has any effect on
    /// the socket.
    virtual ntsa::Error cancel(const ntca::ReceiveToken& token) = 0;

    /// Downgrade the stream socket from encrypted to unencrypted. Return
    /// the error.
    virtual ntsa::Error downgrade() = 0;

    /// Shutdown the stream socket in the specified 'direction' according
    /// to the specified 'mode' of shutdown. Return the error.
    virtual ntsa::Error shutdown(ntsa::ShutdownType::Value direction,
                                 ntsa::ShutdownMode::Value mode) = 0;

    /// Close the stream socket.
    virtual void close() = 0;

    /// Close the stream socket and invoke the specified 'callback' on
    /// the callback's strand, if any, when the stream socket is closed.
    /// Note that callbacks created by this object will automatically be
    /// invoked on this object's strand unless an explicit strand is
    /// specified at the time the callback is created.
    virtual void close(const ntci::CloseFunction& callback) = 0;

    /// Close the stream socket and invoke the specified 'callback' on
    /// the callback's strand, if any, when the stream socket is closed.
    /// Note that callbacks created by this object will automatically be
    /// invoked on this object's strand unless an explicit strand is
    /// specified at the time the callback is created.
    virtual void close(const ntci::CloseCallback& callback) = 0;

    /// Return the handle to the descriptor.
    virtual ntsa::Handle handle() const = 0;

    /// Return the transport of the stream socket.
    virtual ntsa::Transport::Value transport() const = 0;

    /// Return the source endpoint.
    virtual ntsa::Endpoint sourceEndpoint() const = 0;

    /// Return the remote endpoint.
    virtual ntsa::Endpoint remoteEndpoint() const = 0;

    /// Return the source certificate used by the encryption session, if
    /// any.
    virtual bsl::shared_ptr<ntci::EncryptionCertificate> sourceCertificate()
        const = 0;

    /// Return the remote certificate used by the encryption session, if
    /// any.
    virtual bsl::shared_ptr<ntci::EncryptionCertificate> remoteCertificate()
        const = 0;

    /// Return the private key used by the encryption session, if any.
    virtual bsl::shared_ptr<ntci::EncryptionKey> privateKey() const = 0;

    /// Return the listener socket that accepted this socket, or null if
    /// the socket acted in the connecting role or if the socket was
    /// imported.
    virtual bsl::shared_ptr<ntci::ListenerSocket> acceptor() const = 0;

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

/// Provide a guard to automatically close a stream socket
/// and block until the stream socket is asynchronously closed.
class StreamSocketCloseGuard
{
    bsl::shared_ptr<ntci::StreamSocket> d_streamSocket_sp;

  private:
    StreamSocketCloseGuard(const StreamSocketCloseGuard&) BSLS_KEYWORD_DELETED;
    StreamSocketCloseGuard& operator=(const StreamSocketCloseGuard&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Post to the specified 'semaphore'.
    static void complete(bslmt::Semaphore* semaphore);

  public:
    /// Create a new stream socket guard for the specified
    /// 'streamSocket', if any.
    explicit StreamSocketCloseGuard(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket);

    /// Destroy this object: close the managed stream socket, if any, and
    /// block until the stream socket is asynchronously closed.
    ~StreamSocketCloseGuard();

    /// Release the guard and return the guarded object.
    bsl::shared_ptr<ntci::StreamSocket> release();
};

}  // close package namespace
}  // close enterprise namespace
#endif
