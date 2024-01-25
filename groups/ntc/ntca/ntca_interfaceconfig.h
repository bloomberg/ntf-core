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

#ifndef INCLUDED_NTCA_INTERFACECONFIG
#define INCLUDED_NTCA_INTERFACECONFIG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_resolverconfig.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_ipaddress.h>
#include <bdlb_nullablevalue.h>
#include <bsls_timeinterval.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntca {

/// Describe the configuration of a scheduler of asynchronous operation.
///
/// @details
/// This class provides a value-semantic type that describes the configuration
/// of an 'ntci::Interface'. These parameters control the number of threads in
/// the internally managed thread pool, the default read and write queue sizes,
/// the default connection timeouts, and the default socket options, among
/// others.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b driverName:
/// The name of of the backend implementation of asynchronous behavior.
/// Supported drivers are "select", "poll", "epoll", "devpoll", "eventport",
/// "pollset", "kqueue", "iocp", and "asio". Not all drivers are supported on
/// each platform.
///
/// @li @b monitorableName:
/// The name of the interface as published in metrics. If not defined, the
/// monitorablel name is derived from the thread name.
///
/// @li @b threadName:
/// The name assigned to each thread in the thread pool managed by the
/// interface. For maximum portability, the thread name should be less than or
/// equal to 16 characters.
///
/// @li @b minThreads:
/// The minimum number of threads in the thread pool. If the minumum number of
/// threads is less than the maximum number of threads, new threads will be
/// dynamically and automatically added to the thread pool as necessary, up to
/// the maximum number of threads allowed. Set to 0 to externally manage the
/// threads using reactors or proactors to implement asynchronous behavior.
///
/// @li @b maxThreads:
/// The maximum number of threads in the thread pool. If the minumum number of
/// threads is less than the maximum number of threads, new threads will be
/// dynamically and automatically added to the thread pool as necessary, up to
/// the maximum number of threads allowed. Set to 0 to externally manage the
/// threads using reactors or proactors to implement asynchronous behavior.
///
/// @li @b threadStackSize:
/// The size of the stack of each thread in the thread pool.
///
/// @li @b maxEventsPerWait:
/// The maximum number of events to discover each time the polling mechanism is
/// polled. The default value is null, indicating the driver should select an
/// implementation-defined default value.
///
/// @li @b maxTimersPerWait:
/// The maximum number of timers to discover that are due after each time the
/// polling mechanism is polled. The default value is null, indicating the
/// maximum number of timers is unlimited.
///
/// @li @b maxCyclesPerWait:
/// The maximum number of cycles to perform to both discover if any functions
/// have had their execution deferred to be invoked on the I/O thread and to
/// discover any timers that are due. A higher value mitigates the cost of
/// instantanously polling for socket events each time a batch of functions are
/// deferred or timers scheduled to be executed, if it is likely that no socket
/// events have occured, at the possible expense of starving the I/O thread
/// from being able to process socket events that actually have occurred. The
/// default value is null, indicating that only one cycle is performed.
///
/// @li @b maxConnections:
/// The maximum number of supported simultaneous connections.
///
/// @li @b backlog:
/// The depth of the accept backlog.
///
/// @li @b acceptQueueLowWatermark:
/// The minimum number of connections required to call a user accept callback.
///
/// @li @b acceptQueueHighWatermark:
/// The maximum number of connections in the accept queue before disabling
/// further acceptance.
///
/// @li @b readQueueLowWatermark:
/// The starting limit on the minimum size of the read queue required to call a
/// user read callback.
///
/// @li @b readQueueHighWatermark:
/// The maximum size of the read queue before the connection is automatically
/// closed.
///
/// @li @b writeQueueLowWatermark:
/// The size the write queue must be drained down to before the implemenation
/// announces a low watermark event.
///
/// @li @b writeQueueHighWatermark:
/// The maximum size of the write queue that, once eclipsed, rejects subsequent
/// writes until the write queue has been drained back down to this limit.
///
/// @li @b minIncomingStreamTransferSize:
/// The minimum number of bytes into which to copy the receive buffer when
/// reading from stream sockets. This value may be rounded up to the incoming
/// blob buffer size.
///
/// @li @b maxIncomingStreamTransferSize:
/// The maximum number of bytes into which to copy the receive buffer when
/// reading from stream sockets. This value may be rounded up to the incoming
/// blob buffer size.
///
/// @li @b acceptGreedily:
/// The flag indicating that pending connections on the backlog should be
/// repeatedly acceoted until the operating system indicates the backlog is
/// empty. If false, only one accept attempt is made each time the operating
/// system indicates that the backlog is non-empty. When set to true, this
/// option indicates the user favors a lower minimum latency and higher maximum
/// throughput for an individual listener, at the expense of more varying
/// latency and throughput over all connections. When set to false, this option
/// indicates the user favors fairness and less variance in throughput and
/// latency over all connections, and the expense of higher average latency and
/// lower average throughput.
///
/// @li @b sendGreedily:
/// The flag indicating that data should be repeatedly copied from the write
/// queue to the socket send buffer until the operating system indicates the
/// send buffer is full. If false, only one copy attempt is made each time the
/// operating system indicates that the send buffer has capacity available.
/// When set to true, this option indicates the user favors a lower minimum
/// latency and higher maximum throughput for an individual connection, at the
/// expense of more varying latency and throughput over all connections. When
/// set to false, this option indicates the user favors fairness and less
/// variance in throughput and latency over all connections, and the expense of
/// higher average latency and lower average throughput.
///
/// @li @b receiveGreedily:
/// The flag indicating that data should be repeatedly copied from the socket
/// receive buffer to the read queue until the operating system indicates the
/// receive buffer is empty. If false, only one copy attempt is made each time
/// the operating system indicates that the receive buffer is non-empty. When
/// set to true, this option indicates the user favors a lower minimum latency
/// and higher maximum throughput for an individual connection, at the expense
/// of more varying latency and throughput over all connections. When set to
/// false, this option indicates the user favors fairness and less variance in
/// throughput and latency over all connections, and the expense of higher
/// average latency and lower average throughput.
///
/// @li @b sendBufferSize:
/// The maximum size of each socket send buffer. On some platforms, this
/// options may serve simply as a hint.
///
/// @li @b receiveBufferSize:
/// The maximum size of each socket receive buffer. On some platforms, this
/// options may serve simply as a hint.
///
/// @li @b sendBufferLowWatermark:
/// The amount of available capacity that must exist in the socket send buffer
/// for the operating system to indicate the socket is writable.
///
/// @li @b receiveBufferLowWatermark:
/// The amount of available data that must exist in the socket receive buffer
/// for the operating system to indicate the socket is readable.
///
/// @li @b sendTimeout:
/// The amount of time a blocking send operation must complete before it is
/// deemed to have failed. This option is not widely supported and should be
/// avoided.
///
/// @li @b receiveTimeout:
/// The amount of time a blocking receive operation must complete before it is
/// deemed to have failed. This option is not widely supported and should be
/// avoided.
///
/// @li @b timestampOutgoingData:
/// The flag that indicates outgoing data should be timestamped.
///
/// @li @b timestampIncomingData:
/// The flag that indicates incoming data should be timestamped.
///
/// @li @b zeroCopyThreshold:
/// The minimum number of bytes that must be available to send in order to
/// attempt a zero-copy send.
///
/// @li @b keepAlive:
/// That flag that indicates the operating system implementation should
/// periodically emit transport-level "keep-alive" packets.
///
/// @li @b noDelay:
/// The flag that indicates that subsequent writes should not be coalesced into
/// the larger packets that would otherwise form. When set to true, this option
/// indicates the user favors smaller latency at the expense of less network
/// efficiency. When set to false, this option indicates the user favors
/// greater network efficiency at the expense of greater latency.
///
/// @li @b debugFlag:
/// This flag indicates that each socket should be put into debug mode in the
/// operating system. The support and meaning of this option is
/// platform-specific.
///
/// @li @b allowBroadcasting:
/// The flag that indicates the socket supports sending to a broadcast address.
///
/// @li @b bypassNormalRouting:
/// The flag that indicates that normal routing rules are not used, the route
/// is based upon the destination address only.
///
/// @li @b leaveOutOfBandDataInline:
/// The flag that indicates out-of-band data should be placed into the normal
/// data input queue.
///
/// @li @b lingerFlag:
/// The flag that indicates the connection should be maintained after the user
/// closes it while data is still queued but be not transmitted.
///
/// @li @b lingerTimeout:
/// The amount of time a connection should linger after being closed to send
/// queued but untransmitted data.
///
/// @li @b keepHalfOpen:
/// When this option is true, when the socket has detected that the peer has
/// shutdown sending from the remote side of the connection, do not
/// automatically shut down sending from the local side of the connection and
/// close the socket; when the socket is shut down for sending from the local
/// side of the connection, do not automatically shut down receiving and close
/// the socket. When this option is false, when the socket either detects the
/// peer has shutdown sending from the remote side of the connection or shuts
/// down sending from the local side of the connection, the socket is also
/// automatically shutdown for receiving and closed.
///
/// @li @b maxDatagramSize:
/// The maximum size of each datagram.
///
/// @li @b multicastLoopback:
/// The flag that indicates a datagram sent to a multicast address should be
/// "looped back" to the sender when appropriate.
///
/// @li @b multicastTimeToLive:
/// The maximum number of router hops allowed for a multicast datagram.
///
/// @li @b multicastInterface:
/// The address if the network interface to transmit multicast datagrams.
///
/// @li @b dynamicLoadBalancing:
/// The flag that indicates that a single reactor or proactor should be shared
/// among all threads. If false, each thread mantains its own reactor or
/// proactor. When set to true, this option indicates the user favors fairness
/// and latency at the expense of efficiency, throughput and higher CPU usage.
/// When set to false, this option indicates the user favors greater efficiency
/// and throughput at the expense of a larger variance in latency.
///
/// @li @b driverMetrics:
/// The flag that indicates driver metrics should be collected.
///
/// @li @b driverMetricsPerWaiter:
/// The flag that indicates driver metrics per waiter (i.e. thread) should be
/// collected.
///
/// @li @b socketMetrics:
/// The flag that indicates driver metrics should be collected.
///
/// @li @b socketMetricsPerHandle:
/// The flag that indicates socket metrics per handle should be collected.
///
/// @li @b resolverEnabled:
/// The flag that indicates this interface should run an asynchronous resolver.
/// The default value is null, indicating that a default resolver is *not* run.
///
/// @li @b resolverConfig:
/// The asynchronous resolver configuration. The default value is null,
/// indicating that when an asynchronous resolver is enabled it is configured
/// with the default configuration.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_runtime
class InterfaceConfig
{
    bsl::string d_driverName;
    bsl::string d_metricName;

    bsl::string d_threadName;
    bsl::size_t d_minThreads;
    bsl::size_t d_maxThreads;
    bsl::size_t d_threadStackSize;
    bsl::size_t d_threadLoadFactor;

    bdlb::NullableValue<bsl::size_t> d_maxEventsPerWait;
    bdlb::NullableValue<bsl::size_t> d_maxTimersPerWait;
    bdlb::NullableValue<bsl::size_t> d_maxCyclesPerWait;

    bdlb::NullableValue<bsl::size_t> d_maxConnections;

    bdlb::NullableValue<bsl::size_t> d_backlog;

    bdlb::NullableValue<bsl::size_t> d_acceptQueueLowWatermark;
    bdlb::NullableValue<bsl::size_t> d_acceptQueueHighWatermark;
    bdlb::NullableValue<bsl::size_t> d_readQueueLowWatermark;
    bdlb::NullableValue<bsl::size_t> d_readQueueHighWatermark;
    bdlb::NullableValue<bsl::size_t> d_writeQueueLowWatermark;
    bdlb::NullableValue<bsl::size_t> d_writeQueueHighWatermark;
    bdlb::NullableValue<bsl::size_t> d_minIncomingStreamTransferSize;
    bdlb::NullableValue<bsl::size_t> d_maxIncomingStreamTransferSize;

    bdlb::NullableValue<bool> d_acceptGreedily;
    bdlb::NullableValue<bool> d_sendGreedily;
    bdlb::NullableValue<bool> d_receiveGreedily;

    bdlb::NullableValue<bsl::size_t> d_sendBufferSize;
    bdlb::NullableValue<bsl::size_t> d_receiveBufferSize;
    bdlb::NullableValue<bsl::size_t> d_sendBufferLowWatermark;
    bdlb::NullableValue<bsl::size_t> d_receiveBufferLowWatermark;
    bdlb::NullableValue<bsl::size_t> d_sendTimeout;
    bdlb::NullableValue<bsl::size_t> d_receiveTimeout;

    bdlb::NullableValue<bool>           d_timestampOutgoingData;
    bdlb::NullableValue<bool>           d_timestampIncomingData;
    bdlb::NullableValue<bsl::size_t>    d_zeroCopyThreshold;

    bdlb::NullableValue<bool>        d_keepAlive;
    bdlb::NullableValue<bool>        d_noDelay;
    bdlb::NullableValue<bool>        d_debugFlag;
    bdlb::NullableValue<bool>        d_allowBroadcasting;
    bdlb::NullableValue<bool>        d_bypassNormalRouting;
    bdlb::NullableValue<bool>        d_leaveOutOfBandDataInline;
    bdlb::NullableValue<bool>        d_lingerFlag;
    bdlb::NullableValue<bsl::size_t> d_lingerTimeout;
    bdlb::NullableValue<bool>        d_keepHalfOpen;

    bdlb::NullableValue<bsl::size_t>     d_maxDatagramSize;
    bdlb::NullableValue<bool>            d_multicastLoopback;
    bdlb::NullableValue<bsl::size_t>     d_multicastTimeToLive;
    bdlb::NullableValue<ntsa::IpAddress> d_multicastInterface;

    bdlb::NullableValue<bool> d_dynamicLoadBalancing;

    bdlb::NullableValue<bool> d_driverMetrics;
    bdlb::NullableValue<bool> d_driverMetricsPerWaiter;
    bdlb::NullableValue<bool> d_socketMetrics;
    bdlb::NullableValue<bool> d_socketMetricsPerHandle;

    bdlb::NullableValue<bool>                 d_resolverEnabled;
    bdlb::NullableValue<ntca::ResolverConfig> d_resolverConfig;

  public:
    /// Create a new thread configuration having a default value. Optionally
    /// specified a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit InterfaceConfig(bslma::Allocator* basicAllocator = 0);

    /// Create a new thread configuration having the same value as the
    /// specified 'other' object. Optionally specified a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    InterfaceConfig(const InterfaceConfig& other,
                    bslma::Allocator*      basicAllocator = 0);

    /// Destroy this object.
    ~InterfaceConfig();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    InterfaceConfig& operator=(const InterfaceConfig& other);

    /// Set the name of the driver used to implement the interface to the
    /// specified 'driverName'.
    void setDriverName(const bslstl::StringRef& driverName);

    /// Set the metric name for the interface to the specified 'name'.
    void setMetricName(const bslstl::StringRef& name);

    /// Set the base name of each thread managed by the interface to the
    /// specified 'name'.
    void setThreadName(const bslstl::StringRef& name);

    /// Set the minimum number of threads to the specified 'minThreads'.
    void setMinThreads(bsl::size_t minThreads);

    /// Set the maximum number of threads to the specified 'maxThreads'.
    void setMaxThreads(bsl::size_t minThreads);

    /// Set the stack size of the thread to the specified 'threadStackSize'.
    void setThreadStackSize(bsl::size_t threadStackSize);

    /// Set the load factor that determines the desired number of sockets
    /// per thread to the specified 'threadLoadFactor'.
    void setThreadLoadFactor(bsl::size_t threadLoadFactor);

    /// Set the maximum number of events to discover each time the polling
    /// mechanism is polled.
    void setMaxEventsPerWait(bsl::size_t value);

    /// Set the maximum number of timers to discover that are due after each
    /// time the polling mechanism is polled to the specified 'value'.
    void setMaxTimersPerWait(bsl::size_t value);

    /// Set the maximum number of cycles to perform to both discover if any
    /// functions have had their execution deferred to be invoked on the
    /// I/O thread and to discover any timers that are due to the specified
    /// 'value'.
    void setMaxCyclesPerWait(bsl::size_t value);

    /// Set the maximum number of concurrently supported connections to
    /// the specified 'value'.
    void setMaxConnections(bsl::size_t value);

    /// Set the size of the accept backlog to the specified 'value'.
    void setBacklog(bsl::size_t value);

    /// Set the accept queue low watermark to the specified 'value'.
    void setAcceptQueueLowWatermark(bsl::size_t value);

    /// Set the accept queue high watermark to the specified 'value'.
    void setAcceptQueueHighWatermark(bsl::size_t value);

    /// Set the read queue low watermark to the specified 'value'.
    void setReadQueueLowWatermark(bsl::size_t value);

    /// Set the read queue high watermark to the specified 'value'.
    void setReadQueueHighWatermark(bsl::size_t value);

    /// Set the write queue low watermark to the specified 'value'.
    void setWriteQueueLowWatermark(bsl::size_t value);

    /// Set the write queue high watermark to the specified 'value'.
    void setWriteQueueHighWatermark(bsl::size_t value);

    /// Set the minimum number of bytes into which to copy the receive
    /// buffer when reading from stream sockets to the specified 'value'.
    void setMinIncomingStreamTransferSize(bsl::size_t value);

    /// Set the maximum number of bytes into which to copy the receive
    /// buffer when reading from stream sockets to the specified 'value'.
    void setMaxIncomingStreamTransferSize(bsl::size_t value);

    /// Set the flag that controls greedy accepts to the specified 'value'.
    void setAcceptGreedily(bool value);

    /// Set the flag that controls greedy sends to the specified 'value'.
    void setSendGreedily(bool value);

    /// Set the flag that controls greedy receives to the specified 'value'.
    void setReceiveGreedily(bool value);

    /// Set the maximum size of the send buffer to the specified 'value'.
    void setSendBufferSize(bsl::size_t value);

    /// Set the maximum size of the receive buffer to the specified 'value'.
    void setReceiveBufferSize(bsl::size_t value);

    /// Set the minimum amount of capacity available in the send buffer
    /// before it is considered writable to the specified 'value'.
    void setSendBufferLowWatermark(bsl::size_t value);

    /// Set the minimum amount of data available in the receive buffer
    /// before it is considered readable to the specified 'value'.
    void setReceiveBufferLowWatermark(bsl::size_t value);

    /// Set the send timeout to the specified 'value'.
    void setSendTimeout(bsl::size_t value);

    /// Set the receive timeout to the specified 'value'.
    void setReceiveTimeout(bsl::size_t value);

    /// Set the flag that indicates outgoing data should be timestamped to the
    /// specified 'value'.
    void setTimestampOutgoingData(bool value);

    /// Set the flag that indicates incoming data should be timestamped to the
    /// specified 'value'.
    void setTimestampIncomingData(bool value);

    /// Set the minimum number of bytes that must be available to send in order
    /// to attempt a zero-copy send to the specified 'value'.
    void setZeroCopyThreshold(size_t value);

    /// Set the flag enable protocol-level keep-alive messages to the
    /// specified 'value'.
    void setKeepAlive(bool value);

    /// Set the flag disabling protocol-level packet coalescing to the
    /// specified 'value'.
    void setNoDelay(bool value);

    /// Set the flag indicating debug information should be recorded.
    void setDebugFlag(bool value);

    /// Set the flag enabling broadcasting to the specified 'value'.
    void setAllowBroadcasting(bool value);

    /// Set the flag bypassing normal routing to the specified 'value'.
    void setBypassNormalRouting(bool value);

    /// Set the flag indicating that out-of-band data should be left
    /// inline to the specified 'value'.
    void setLeaveOutofBandDataInline(bool value);

    /// Set the linger flag to the specified 'value'.
    void setLingerFlag(bool value);

    /// Set the linger timeout, in seconds.
    void setLingerTimeout(bsl::size_t value);

    /// Set the flag indicating the socket should be kept half-open (i.e.,
    /// able to receive) even though transmission has been shutdown by
    /// either side of the connection to the specified 'value'.
    void setKeepHalfOpen(bool value);

    /// Set the maximum datagram size to the specified 'value'.
    void setMaxDatagramSize(bsl::size_t value);

    /// Set the flag that indicates multicast datagrams should be looped
    /// back to the local host to the specified 'value'.
    void setMulticastLoopback(bool value);

    /// Set the maximum number of hops over which multicast datagrams
    /// should be forwarded to the specified 'value'.
    void setMulticastTimeToLive(bsl::size_t value);

    /// Set the address of the network interface on which multicast
    /// datagrams should be sent to the specified 'value'.
    void setMulticastInterface(const ntsa::IpAddress& value);

    /// Set the flag that indicates I/O should be balanced across threads
    /// dynamically rather than statically at the time of socket creation
    /// to the specified 'value'.
    void setDynamicLoadBalancing(bool value);

    /// Set the flag that indicates driver metrics should be collected to
    /// the specified 'value'.
    void setDriverMetrics(bool value);

    /// Set the flag that indicates driver metrics per waiter (i.e. thread)
    /// should be collected to the specified 'value'.
    void setDriverMetricsPerWaiter(bool value);

    /// Set the flag that indicates socket metrics should be collected to
    /// the specified 'value'.
    void setSocketMetrics(bool value);

    /// Set the flag that indicates socket metrics per handle should be
    /// collected to the specified 'value'.
    void setSocketMetricsPerHandle(bool value);

    /// Set the flag that indicates this interface should run an
    /// asynchronous resolver to the specified 'value'. The default value is
    /// null, indicating that a default resolver is *not* run.
    void setResolverEnabled(bool value);

    /// Set the asynchronous resolver configuration to the specified
    /// 'value'. The default value is null, indicating that when an
    /// asynchronous resolver is enabled it is configured with the default
    /// configuration.
    void setResolverConfig(const ntca::ResolverConfig& value);

    /// Set the name of the driver used to implement the interface to the
    /// specified 'driverName'.
    const bsl::string& driverName() const;

    /// Return the metric name.
    const bsl::string& metricName() const;

    /// Return the base name of each thread managed by the interface.
    const bsl::string& threadName() const;

    /// Return the minimum number of threads.
    bsl::size_t minThreads() const;

    /// Return the maximum number of threads.
    bsl::size_t maxThreads() const;

    /// Return the stack size of the thread.
    bsl::size_t threadStackSize() const;

    /// Return the load factor that determines the desired number of sockets
    /// per thread.
    bsl::size_t threadLoadFactor() const;

    /// Return the maximum number of events to discover each time
    /// the polling mechanism is polled. If the value is null,
    /// the driver should select an implementation-defined default value.
    const bdlb::NullableValue<bsl::size_t>& maxEventsPerWait() const;

    /// Return the maximum number of timers to discover that are due after
    /// each time the polling mechanism is polled. If the value is null, the
    /// maximum number of timers is unlimited.
    const bdlb::NullableValue<bsl::size_t>& maxTimersPerWait() const;

    /// Return the maximum number of cycles to perform to both discover if
    /// any functions have had their execution deferred to be invoked on the
    /// I/O thread and to discover any timers that are due. If the value is
    /// null, only one cycle is performed.
    const bdlb::NullableValue<bsl::size_t>& maxCyclesPerWait() const;

    /// Return the maximum number of concurrently supported connections.
    const bdlb::NullableValue<bsl::size_t>& maxConnections() const;

    /// Return the size of the accept backlog.
    const bdlb::NullableValue<bsl::size_t>& backlog() const;

    /// Return the accept queue low watermark.
    const bdlb::NullableValue<bsl::size_t>& acceptQueueLowWatermark() const;

    /// Return the accept queue high watermark.
    const bdlb::NullableValue<bsl::size_t>& acceptQueueHighWatermark() const;

    /// Return the read queue low watermark.
    const bdlb::NullableValue<bsl::size_t>& readQueueLowWatermark() const;

    /// Return the read queue high watermark.
    const bdlb::NullableValue<bsl::size_t>& readQueueHighWatermark() const;

    /// Return the write queue low watermark.
    const bdlb::NullableValue<bsl::size_t>& writeQueueLowWatermark() const;

    /// Return the write queue high watermark.
    const bdlb::NullableValue<bsl::size_t>& writeQueueHighWatermark() const;

    /// Return the minimum number of bytes into which to copy the receive
    /// buffer when reading from stream sockets.
    const bdlb::NullableValue<bsl::size_t>& minIncomingStreamTransferSize()
        const;

    /// Return the maximum number of bytes into which to copy the receive
    /// buffer when reading from stream sockets.
    const bdlb::NullableValue<bsl::size_t>& maxIncomingStreamTransferSize()
        const;

    /// Return the flag that controls greedy accepts.
    const bdlb::NullableValue<bool>& acceptGreedily() const;

    /// Return the flag that controls greedy sends.
    const bdlb::NullableValue<bool>& sendGreedily() const;

    /// Return the flag that controls greedy receives.
    const bdlb::NullableValue<bool>& receiveGreedily() const;

    /// Return the maximum size of the send buffer.
    const bdlb::NullableValue<bsl::size_t>& sendBufferSize() const;

    /// Return the maximum size of the receive buffer.
    const bdlb::NullableValue<bsl::size_t>& receiveBufferSize() const;

    /// Return the minimum amount of capacity available in the send buffer
    /// before it is considered writable.
    const bdlb::NullableValue<bsl::size_t>& sendBufferLowWatermark() const;

    /// Return the minimum amount of data available in the receive buffer
    /// before it is considered readable.
    const bdlb::NullableValue<bsl::size_t>& receiveBufferLowWatermark() const;

    /// Return the send timeout.
    const bdlb::NullableValue<bsl::size_t>& sendTimeout() const;

    /// Return the receive timeout.
    const bdlb::NullableValue<bsl::size_t>& receiveTimeout() const;

    /// Return the flag that indicates outgoing data should be timestamped.
    const bdlb::NullableValue<bool>& timestampOutgoingData() const;

    /// Return the flag that indicates incoming data should be timestamped.
    const bdlb::NullableValue<bool>& timestampIncomingData() const;

    /// Return the minimum number of bytes that must be available to send in
    /// order to attempt a zero-copy send.
    const bdlb::NullableValue<bsl::size_t>& zeroCopyThreshold() const;

    /// Return the flag enable protocol-level keep-alive messages.
    const bdlb::NullableValue<bool>& keepAlive() const;

    /// Return the flag disabling protocol-level packet coalescing.
    const bdlb::NullableValue<bool>& noDelay() const;

    /// Return the flag indicating debug information should be recorded.
    const bdlb::NullableValue<bool>& debugFlag() const;

    /// Return the flag enabling broadcasting .
    const bdlb::NullableValue<bool>& allowBroadcasting() const;

    /// Return the flag bypassing normal routing.
    const bdlb::NullableValue<bool>& bypassNormalRouting() const;

    /// Return the flag indicating that out-of-band data should be left
    /// inline.
    const bdlb::NullableValue<bool>& leaveOutofBandDataInline() const;

    /// Return the linger flag.
    const bdlb::NullableValue<bool>& lingerFlag() const;

    /// Return the linger timeout, in seconds.
    const bdlb::NullableValue<bsl::size_t>& lingerTimeout() const;

    /// Return the flag indicating the socket should be kept half-open
    /// (i.e., able to receive) even though transmission has been shutdown
    /// by either side of the connection.
    const bdlb::NullableValue<bool>& keepHalfOpen() const;

    /// Return the maximum datagram size.
    const bdlb::NullableValue<bsl::size_t>& maxDatagramSize() const;

    /// Return flag that indicates multicast datagrams should be looped
    /// back to the local host.
    const bdlb::NullableValue<bool>& multicastLoopback() const;

    /// Return the maximum number of hops over which multicast datagrams
    /// should be forwarded.
    const bdlb::NullableValue<bsl::size_t>& multicastTimeToLive() const;

    /// Return the address of the network interface on which multicast
    /// datagrams should be sent.
    const bdlb::NullableValue<ntsa::IpAddress>& multicastInterface() const;

    /// Return the flag that indicates I/O should be balanced across threads
    /// dynamically rather than statically at the time of socket creation.
    const bdlb::NullableValue<bool>& dynamicLoadBalancing() const;

    /// Set the flag that indicates driver metrics should be collected to
    /// the specified 'value'.
    const bdlb::NullableValue<bool>& driverMetrics() const;

    /// Set the flag that indicates driver metrics per waiter (i.e. thread)
    /// should be collected to the specified 'value'.
    const bdlb::NullableValue<bool>& driverMetricsPerWaiter() const;

    /// Set the flag that indicates socket metrics should be collected to
    /// the specified 'value'.
    const bdlb::NullableValue<bool>& socketMetrics() const;

    /// Set the flag that indicates socket metrics per handle should be
    /// collected to the specified 'value'.
    const bdlb::NullableValue<bool>& socketMetricsPerHandle() const;

    /// Return the flag that indicates this interface should run an
    /// asynchronous resolver. The default value is null, indicating that a
    /// default resolver is *not* run.
    const bdlb::NullableValue<bool>& resolverEnabled() const;

    /// Return the asynchronous resolver configuration. The default value is
    /// null, indicating that when an asynchronous resolver is enabled it is
    /// configured with the default configuration.
    const bdlb::NullableValue<ntca::ResolverConfig>& resolverConfig() const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::InterfaceConfig
bsl::ostream& operator<<(bsl::ostream&          stream,
                         const InterfaceConfig& object);

}  // close package namespace
}  // close enterprise namespace
#endif
