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

#ifndef INCLUDED_NTCD_MACHINE
#define INCLUDED_NTCD_MACHINE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_reactorevent.h>
#include <ntccfg_platform.h>
#include <ntci_resolver.h>
#include <ntcs_interest.h>
#include <ntcscm_version.h>
#include <ntsa_adapter.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_handle.h>
#include <ntsa_notificationqueue.h>
#include <ntsa_shutdownmode.h>
#include <ntsa_shutdowntype.h>
#include <ntsa_socketconfig.h>
#include <ntsa_socketoption.h>
#include <ntsa_transport.h>
#include <ntsi_datagramsocket.h>
#include <ntsi_listenersocket.h>
#include <ntsi_streamsocket.h>
#include <bdlbb_blob.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bdlcc_singleconsumerqueue.h>
#include <bslh_hash.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>
#include <bsls_atomic.h>
#include <bsl_bitset.h>
#include <bsl_iosfwd.h>
#include <bsl_list.h>
#include <bsl_map.h>
#include <bsl_memory.h>
#include <bsl_set.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcd {
class Packet;
}
namespace ntcd {
class PacketQueue;
}
namespace ntcd {
class Session;
}
namespace ntcd {
class SessionQueue;
}
namespace ntcd {
class Monitor;
}
namespace ntcd {
class Machine;
}
namespace ntcd {

/// @internal @brief
/// Enumerate simulated packet types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcd
struct PacketType {
  public:
    /// Enumerate simulated packet types.
    enum Value {
        /// The packet type is not defined.
        e_UNDEFINED = 0,

        /// The packet requests a connection.
        e_CONNECT = 1,

        /// The packet delivers data.
        e_PUSH = 2,

        /// The packet shuts down a connection.
        e_SHUTDOWN = 3,

        /// The packet resets a connection.
        e_RESET = 4,

        /// The packet describes an asynchronous error.
        e_ERROR = 5
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

// FREE OPERATORS

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
bsl::ostream& operator<<(bsl::ostream& stream, PacketType::Value rhs);

/// @internal @brief
/// Describe a simulated packet.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcd
class Packet
{
    ntcd::PacketType::Value                 d_type;
    ntsa::Transport::Value                  d_transport;
    ntsa::Endpoint                          d_sourceEndpoint;
    ntsa::Endpoint                          d_remoteEndpoint;
    bsl::weak_ptr<ntcd::Session>            d_sourceSession_wp;
    bsl::weak_ptr<ntcd::Session>            d_remoteSession_wp;
    bdlbb::Blob                             d_data;
    bdlb::NullableValue<bsl::uint32_t>      d_id;
    bdlb::NullableValue<bsls::TimeInterval> d_rxTimestamp;
    bdlbb::BlobBufferFactory*               d_blobBufferFactory_p;
    bslma::Allocator*                       d_allocator_p;

  private:
    Packet(const Packet&) BSLS_KEYWORD_DELETED;
    Packet& operator=(const Packet&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new packet using the specified 'blobBufferFactory' to
    /// supply blob buffers. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Packet(bdlbb::BlobBufferFactory* blobBufferFactory,
                    bslma::Allocator*         basicAllocator = 0);

    /// Destroy this object.
    ~Packet();

    /// Set the packet type to the specified 'type'.
    void setType(ntcd::PacketType::Value type);

    /// Set the transport to the specified 'transport'.
    void setTransport(ntsa::Transport::Value transport);

    /// Set the source endpoint of the packet to the specified
    /// 'sourceEndpoint'.
    void setSourceEndpoint(const ntsa::Endpoint& sourceEndpoint);

    /// Set the remote endpoint of the packet to the specified
    /// 'remoteEndpoint'.
    void setRemoteEndpoint(const ntsa::Endpoint& remoteEndpoint);

    /// Set the source endpoint of the packet to the specified
    /// 'sourceSession'.
    void setSourceSession(const bsl::weak_ptr<ntcd::Session>& sourceSession);

    /// Set the remote endpoint of the packet to the specified
    /// 'remoteSession'.
    void setRemoteSession(const bsl::weak_ptr<ntcd::Session>& remoteSession);

    /// Set the packet data to the specified 'data'.
    void setData(const bdlbb::Blob& data);

    /// Set d_rxTimestamp to the specified 'timestamp'.
    void setRxTimestamp(const bsls::TimeInterval& timestamp);

    /// Set packet d_id to the specified 'id'.
    void setId(bsl::uint32_t id);

    /// Copy packet data from the specified 'data' according to the
    /// specified 'options'. Load into the specified 'context' the result
    /// of the operation. Return the error.
    ntsa::Error enqueueData(ntsa::SendContext*       context,
                            const bdlbb::Blob&       data,
                            const ntsa::SendOptions& options);

    /// Copy packet data from the specified 'data' according to the
    /// specified 'options'. Load into the specified 'context' the result
    /// of the operation. Return the error.
    ntsa::Error enqueueData(ntsa::SendContext*       context,
                            const ntsa::Data&        data,
                            const ntsa::SendOptions& options);

    /// Copy packet data to the specified 'data' according to the specified
    /// 'options'. Remove the bytes copied from the front of the packet
    /// data. Load into the specified 'context' the result of the operation.
    /// Return the error.
    ntsa::Error dequeueData(ntsa::ReceiveContext*       context,
                            bdlbb::Blob*                data,
                            const ntsa::ReceiveOptions& options);

    /// Copy packet data to the specified 'data' according to the specified
    /// 'options'. Remove the bytes copied from the front of the packet
    /// data. Load into the specified 'context' the result of the operation.
    /// Return the error.
    ntsa::Error dequeueData(ntsa::ReceiveContext*       context,
                            bdlbb::BlobBuffer*          data,
                            const ntsa::ReceiveOptions& options);

    /// Copy packet data to the specified 'data' according to the specified
    /// 'options'. Remove the bytes copied from the front of the packet
    /// data. Load into the specified 'context' the result of the operation.
    /// Return the error.
    ntsa::Error dequeueData(ntsa::ReceiveContext*       context,
                            ntsa::MutableBuffer*        data,
                            const ntsa::ReceiveOptions& options);

    /// Copy packet data to the specified 'data' according to the specified
    /// 'options'. Remove the bytes copied from the front of the packet
    /// data. Load into the specified 'context' the result of the operation.
    /// Return the error.
    ntsa::Error dequeueData(ntsa::ReceiveContext*       context,
                            ntsa::MutableBufferArray*   data,
                            const ntsa::ReceiveOptions& options);

    /// Copy packet data to the specified 'data' according to the specified
    /// 'options'. Remove the bytes copied from the front of the packet
    /// data. Load into the specified 'context' the result of the operation.
    /// Return the error.
    ntsa::Error dequeueData(ntsa::ReceiveContext*        context,
                            ntsa::MutableBufferPtrArray* data,
                            const ntsa::ReceiveOptions&  options);

    /// Copy packet data to the specified 'data' according to the specified
    /// 'options'. Remove the bytes copied from the front of the packet
    /// data. Load into the specified 'context' the result of the operation.
    /// Return the error.
    ntsa::Error dequeueData(ntsa::ReceiveContext*       context,
                            bsl::string*                data,
                            const ntsa::ReceiveOptions& options);

    /// Copy packet data to the specified 'data' according to the specified
    /// 'options'. Remove the bytes copied from the front of the packet
    /// data. Load into the specified 'context' the result of the operation.
    /// Return the error.
    ntsa::Error dequeueData(ntsa::ReceiveContext*       context,
                            ntsa::Data*                 data,
                            const ntsa::ReceiveOptions& options);

    /// Return the packet type.
    ntcd::PacketType::Value type() const;

    /// Return the transport.
    ntsa::Transport::Value transport() const;

    /// Return the source endpoint.
    const ntsa::Endpoint& sourceEndpoint() const;

    /// Return the remote endpoint.
    const ntsa::Endpoint& remoteEndpoint() const;

    /// Return the source session.
    const bsl::weak_ptr<ntcd::Session>& sourceSession() const;

    /// Return the remote session.
    const bsl::weak_ptr<ntcd::Session>& remoteSession() const;

    /// Return the data, if any.
    const bdlbb::Blob& data() const;

    /// Return the rx timestamp, if any.
    const bdlb::NullableValue<bsls::TimeInterval>& rxTimestamp() const;

    /// Return the id, if any.
    const bdlb::NullableValue<bsl::uint32_t>& id() const;

    /// Return the length of the data of the packet.
    bsl::size_t length() const;

    /// Return the cost of the packet to the packet queue.
    bsl::size_t cost() const;

    /// Return the allocator used to supply memory.
    bslma::Allocator* allocator() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Packet& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Packet& other) const;

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

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(Packet);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntcd::Packet
bsl::ostream& operator<<(bsl::ostream& stream, const Packet& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntcd::Packet
bool operator==(const Packet& lhs, const Packet& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntcd::Packet
bool operator!=(const Packet& lhs, const Packet& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntcd::Packet
bool operator<(const Packet& lhs, const Packet& rhs);

/// @internal @brief
/// Provide a queue of simulated packets.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class PacketQueue
{
    /// This typedef defines a queue of packets.
    typedef bsl::list<bsl::shared_ptr<ntcd::Packet> > Storage;

    bslmt::Condition   d_allowDequeue;
    bslmt::Condition   d_allowEnqueue;
    Storage            d_storage;
    bsls::AtomicUint64 d_currentWatermark;
    bsls::AtomicUint64 d_lowWatermark;
    bsls::AtomicUint64 d_highWatermark;
    bsls::AtomicBool   d_shutdown;
    bslma::Allocator*  d_allocator_p;

  private:
    PacketQueue(const PacketQueue&) BSLS_KEYWORD_DELETED;
    PacketQueue& operator=(const PacketQueue&) BSLS_KEYWORD_DELETED;

  public:
    /// This typedef defines a vector of packets.
    typedef bsl::vector<bsl::shared_ptr<ntcd::Packet> > PacketVector;

    /// This typedef defines a functor which can be applied to a packet
    typedef bsl::function<void(ntcd::Packet*)> PacketFunctor;

    /// Create a new object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit PacketQueue(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~PacketQueue();

    /// Set the low watermark to the specified 'lowWatermark'.
    ntsa::Error setLowWatermark(bsl::size_t lowWatermark);

    /// Set the high watermark to the specified 'highWatermark'.
    ntsa::Error setHighWatermark(bsl::size_t highWatermark);

    /// Enqueue the specified 'packet'. If the specified 'block' flag is
    /// true, block until sufficient capacity is availble to store the
    /// 'packet'. If 'packetFunctor' is set then apply it to the packet in
    /// case of succesful enqueueing. Return the error.
    ntsa::Error enqueue(ntccfg::ConditionMutex*        mutex,
                        bsl::shared_ptr<ntcd::Packet>& packet,
                        bool                           block,
                        PacketFunctor packetFunctor = PacketFunctor());

    /// Enqueue the specified 'packet' to the front of the packet queue.
    void retry(const bsl::shared_ptr<ntcd::Packet>& packet);

    /// Enqueue the specified 'packetVector' to the front of the packet
    /// queue.
    void retry(const PacketVector& packetVector);

    /// Dequeue a packet from the queue and load the dequeued packet into
    /// the specified 'result'. If the specified 'block' flag is true, block
    /// until a packet is available to dequeue. Return the error.
    ntsa::Error dequeue(ntccfg::ConditionMutex*        mutex,
                        bsl::shared_ptr<ntcd::Packet>* result,
                        bool                           block);

    /// Load the packet at the front of the queue into the specified
    /// 'result', but do not dequeue the packet If the specified 'block'
    /// flag is true, block until a packet is available to dequeue. Return
    /// the error.
    ntsa::Error peek(ntccfg::ConditionMutex*        mutex,
                     bsl::shared_ptr<ntcd::Packet>* result,
                     bool                           block);

    /// Dequeue a packet from the queue.  If the specified 'block'
    /// flag is true, block until a packet is available to dequeue. Return
    /// the error.
    ntsa::Error pop(ntccfg::ConditionMutex* mutex, bool block);

    /// Wake up any threads blocked on this queue. Return the error.
    ntsa::Error wakeup();

    /// Shutdown the queue. Return the error.
    ntsa::Error shutdown();

    /// Return true if there are no packets in the queue, otherwise return
    /// false.
    bool empty() const;

    /// Return the total number of bytes in the queue.
    bsl::size_t totalSize() const;

    /// Return the low watermark.
    bsl::size_t lowWatermark() const;

    /// Return the high watermark.
    bsl::size_t highWatermark() const;
};

/// @internal @brief
/// Describe a pair of endpoints in a simulation.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcd
class Binding
{
    ntsa::Endpoint d_sourceEndpoint;
    ntsa::Endpoint d_remoteEndpoint;

  public:
    /// Create a new, uninitialized binding.
    Binding();

    /// Create a new binding consisting of the specified 'sourceEndpoint'
    /// and 'remoteEndpoint'.
    Binding(const ntsa::Endpoint& sourceEndpoint,
            const ntsa::Endpoint& remoteEndpoint);

    /// Create a new binding having the same value as the specified
    /// 'original' object.
    Binding(const Binding& original);

    /// Destroy this object.
    ~Binding();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Binding& operator=(const Binding& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the source endpoint to the specified 'sourceEndpoint'.
    void setSourceEndpoint(const ntsa::Endpoint& sourceEndpoint);

    /// Set the remote endpoint to the specified 'remoteEndpoint'.
    void setRemoteEndpoint(const ntsa::Endpoint& remoteEndpoint);

    /// Return the source endpoint.
    const ntsa::Endpoint& sourceEndpoint() const;

    /// Return the remote endpoint.
    const ntsa::Endpoint& remoteEndpoint() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Binding& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Binding& other) const;

    /// Return the endpoint that represents "any" address for the specified
    /// 'transport'.
    static ntsa::Endpoint makeAny(ntsa::Transport::Value transport);
};

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntcd::Binding
bool operator==(const Binding& lhs, const Binding& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntcd::Binding
bool operator!=(const Binding& lhs, const Binding& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntcd::Binding
bool operator<(const Binding& lhs, const Binding& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntcd::Binding
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Binding& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Binding& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.sourceEndpoint());
    hashAppend(algorithm, value.remoteEndpoint());
}

/// @internal @brief
/// Provide a map of simulated ports in use on a simulated machine.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class PortMap
{
    enum {
        k_MIN_PORT           = 1,
        k_MIN_EPHEMERAL_PORT = 49152,
        k_MAX_EPHEMERAL_PORT = 65535,
        k_MAX_PORT           = k_MAX_EPHEMERAL_PORT
    };

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex           d_mutex;
    bsl::bitset<k_MAX_PORT> d_bitset;
    bslma::Allocator*       d_allocator_p;

  private:
    PortMap(const PortMap&) BSLS_KEYWORD_DELETED;
    PortMap& operator=(const PortMap&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit PortMap(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~PortMap();

    /// Acquire the specified 'request' port and load the actually acquired
    /// port into the specified 'result'. If the 'requested' port is 0,
    /// acquire the next available ephemeral port. Return the error, notable
    /// 'ntsa::Error::e_ADDRESS_IN_USE' if the 'requested' port is not
    /// equal to zero and the 'request' port is already in use, or if the
    /// 'requested' port is 0 but all ephemeral ports are in use. Note that
    /// 'result' may be an alias for 'requested'.
    ntsa::Error acquire(ntsa::Port* result, ntsa::Port requested);

    /// Release the specified 'port'.
    void release(ntsa::Port port);

    /// Return true if the specified 'port' used, otherwise return false.
    bool isUsed(ntsa::Port port);

    /// Return true if the specified 'port' free, otherwise return false.
    bool isFree(ntsa::Port port);
};

/// @internal @brief
/// Provide a queue of simulated socket contexts.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class SessionQueue
{
    /// This typedef defines a queue of contexts.
    typedef bsl::list<bsl::shared_ptr<ntcd::Session> > Storage;

    bslmt::Condition   d_allowDequeue;
    bslmt::Condition   d_allowEnqueue;
    Storage            d_storage;
    bsls::AtomicUint64 d_currentWatermark;
    bsls::AtomicUint64 d_lowWatermark;
    bsls::AtomicUint64 d_highWatermark;
    bsls::AtomicBool   d_shutdownSend;
    bsls::AtomicBool   d_shutdownReceive;
    bslma::Allocator*  d_allocator_p;

  private:
    SessionQueue(const SessionQueue&) BSLS_KEYWORD_DELETED;
    SessionQueue& operator=(const SessionQueue&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit SessionQueue(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~SessionQueue();

    /// Set the low watermark to the specified 'lowWatermark'.
    ntsa::Error setLowWatermark(bsl::size_t lowWatermark);

    /// Set the high watermark to the specified 'highWatermark'.
    ntsa::Error setHighWatermark(bsl::size_t highWatermark);

    /// Enqueue the specified 'session'. If the specified 'block' flag is
    /// true, block until sufficient capacity is availble to store the
    /// 'session'. Return the error.
    ntsa::Error enqueueSession(ntccfg::ConditionMutex*               mutex,
                               const bsl::shared_ptr<ntcd::Session>& session,
                               bool                                  block);

    /// Dequeue a context from the queue and load the dequeued context into
    /// the specified 'result'. If the specified 'block' flag is true, block
    /// until a context is available to dequeue. Return the error.
    ntsa::Error dequeueSession(ntccfg::ConditionMutex*         mutex,
                               bsl::shared_ptr<ntcd::Session>* result,
                               bool                            block);

    /// Shutdown the queue according to the specified shutdown 'type'.
    ntsa::Error shutdown(ntsa::ShutdownType::Value type);

    /// Return true if there are no packets in the queue, otherwise return
    /// false.
    bool empty() const;

    /// Return the total number of bytes in the queue.
    bsl::size_t totalSize() const;

    /// Return the low watermark.
    bsl::size_t lowWatermark() const;

    /// Return the high watermark.
    bsl::size_t highWatermark() const;
};

/// @internal @brief
/// Provide a simulated communication session between two endpoints.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class Session : public ntsi::DatagramSocket,
                public ntsi::ListenerSocket,
                public ntsi::StreamSocket,
                public ntscfg::Shared<Session>
{
    class UpdateGuard;
    // Provide a guard to automatically update a session's
    // enabled events.

    typedef bsl::list<ntsa::Notification> SocketErrorQueue;

    mutable ntccfg::ConditionMutex              d_mutex;
    ntsa::Handle                                d_handle;
    ntsa::Transport::Value                      d_transport;
    ntsa::Endpoint                              d_sourceEndpoint;
    ntsa::Endpoint                              d_remoteEndpoint;
    ntsa::SocketConfig                          d_socketOptions;
    bsl::shared_ptr<ntcd::Machine>              d_machine_sp;
    bsl::shared_ptr<ntcd::Monitor>              d_monitor_sp;
    bsl::weak_ptr<ntcd::Session>                d_peer_wp;
    bsl::shared_ptr<ntcd::SessionQueue>         d_sessionQueue_sp;
    bsl::shared_ptr<ntcd::PacketQueue>          d_outgoingPacketQueue_sp;
    bsl::shared_ptr<ntcd::PacketQueue>          d_incomingPacketQueue_sp;
    bsl::shared_ptr<SocketErrorQueue>           d_socketErrorQueue_sp;
    bsl::uint32_t                               d_tsKey;
    bool                                        d_blocking;
    bool                                        d_listening;
    bool                                        d_accepted;
    bool                                        d_connected;
    bsls::AtomicBool                            d_readable;
    bsls::AtomicBool                            d_readableActive;
    bsls::AtomicUint64                          d_readableBytes;
    bsls::AtomicBool                            d_writable;
    bsls::AtomicBool                            d_writableActive;
    bsls::AtomicUint64                          d_writableBytes;
    bsls::AtomicBool                            d_error;
    bsls::AtomicBool                            d_errorActive;
    bsls::AtomicInt                             d_errorCode;
    bsls::AtomicBool                            d_hasNotifications;
    bsls::AtomicBool                            d_notificationsActive;
    bsl::size_t                                 d_backlog;
    bdlcc::SingleConsumerQueue<ntsa::Timestamp> d_feedbackQueue;
    bslma::Allocator*                           d_allocator_p;

  private:
    Session(const Session&) BSLS_KEYWORD_DELETED;
    Session& operator=(const Session&) BSLS_KEYWORD_DELETED;

  private:
    /// Reset the state of this session to its state upon construction.
    void reset();

    /// Enable or disable readability and writability in the associated
    /// monitor, if any, as necessary.
    void update();

    /// Return the number of bytes readable.
    bsl::size_t privateBytesReadable() const;

    /// Return the number of bytes writable.
    bsl::size_t privateBytesWritable() const;

    /// Return the error.
    ntsa::Error privateError() const;

    /// Return true if the session is readable, otherwise return false.
    bool privateIsReadable() const;

    /// Return true if the session is writable, otherwise return false.
    bool privateIsWritable() const;

    /// Return true if the session has an error, otherwise return false.
    bool privateHasError() const;

    /// Return true if the session has a notification, otherwise return false.
    bool privateHasNotification() const;

  public:
    /// Create a new session on the specified 'machine'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Session(const bsl::shared_ptr<ntcd::Machine>& machine,
                     bslma::Allocator*                     basicAllocator = 0);

    /// Destroy this object.
    ~Session() BSLS_KEYWORD_OVERRIDE;

    /// Create a new socket of the specified 'transport'. Return the
    /// error.
    ntsa::Error open(ntsa::Transport::Value transport) BSLS_KEYWORD_OVERRIDE;

    /// Acquire ownership of the specified 'handle' to implement this
    /// socket. Return the error.
    ntsa::Error acquire(ntsa::Handle handle) BSLS_KEYWORD_OVERRIDE;

    /// Release ownership of the handle that implements this socket.
    ntsa::Handle release() BSLS_KEYWORD_OVERRIDE;

    /// Bind this socket to the specified source 'endpoint'. If the
    /// specified 'reuseAddress' flag is set, allow this socket to bind to
    /// an address already in use by the operating system. Return the error.
    ntsa::Error bind(const ntsa::Endpoint& endpoint,
                     bool                  reuseAddress) BSLS_KEYWORD_OVERRIDE;

    /// Bind this to any suitable source endpoint appropriate for a socket
    /// of the specified 'transport'. If the specified 'reuseAddress' flag
    /// is set, allow this socket to bind to an address already in use by
    /// the operating system. Return the error.
    ntsa::Error bindAny(ntsa::Transport::Value transport,
                        bool reuseAddress) BSLS_KEYWORD_OVERRIDE;

    /// Listen for connections made to this socket's source endpoint. Return
    /// the error.
    ntsa::Error listen(bsl::size_t backlog) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a handle to a connection to this
    /// socket's source endpoint. Return the error.
    ntsa::Error accept(ntsa::Handle* result) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a handle to a connection to this
    /// socket's source endpoint. Return the error. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    ntsa::Error accept(bslma::ManagedPtr<ntsi::StreamSocket>* result,
                       bslma::Allocator* basicAllocator = 0)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a handle to a connection to this
    /// socket's source endpoint. Return the error. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    ntsa::Error accept(bsl::shared_ptr<ntsi::StreamSocket>* result,
                       bslma::Allocator*                    basicAllocator = 0)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a session connected to this
    /// socket's source endpoint. Return the error. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    ntsa::Error accept(bsl::shared_ptr<ntcd::Session>* result,
                       bslma::Allocator*               basicAllocator = 0);

    /// Connect to the specified remote 'endpoint'. Return the error.
    ntsa::Error connect(const ntsa::Endpoint& endpoint) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'data' to the socket send buffer according to
    /// the specified 'options'. Load into the specified 'context' the
    /// result of the operation. Return the error.
    ntsa::Error send(ntsa::SendContext*       context,
                     const bdlbb::Blob&       data,
                     const ntsa::SendOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'data' to the socket send buffer according to
    /// the specified 'options'. Load into the specified 'context' the
    /// result of the operation. Return the error.
    ntsa::Error send(ntsa::SendContext*       context,
                     const ntsa::Data&        data,
                     const ntsa::SendOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Enqueue the specified 'data' having the specified 'size' to the
    /// socket send buffer according to the specified 'options'. Load into
    /// the specified 'context' the result of the operation. Return the
    /// error.
    ntsa::Error send(ntsa::SendContext*       context,
                     const ntsa::ConstBuffer* data,
                     bsl::size_t              size,
                     const ntsa::SendOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Dequeue from the socket receive buffer into the specified 'data'
    /// according to the specified 'options'. Load into the specified
    /// 'context' the result of the operation. Return the error.
    ntsa::Error receive(ntsa::ReceiveContext*       context,
                        bdlbb::Blob*                data,
                        const ntsa::ReceiveOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Dequeue from the socket receive buffer into the specified 'data'
    /// according to the specified 'options'. Load into the specified
    /// 'context' the result of the operation. Return the error.
    ntsa::Error receive(ntsa::ReceiveContext*       context,
                        ntsa::Data*                 data,
                        const ntsa::ReceiveOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Read data from the socket error queue. Then if the specified
    /// 'notifications' is not null parse fetched data to extract control
    /// messages into the specified 'notifications'. Return the error.
    ntsa::Error receiveNotifications(ntsa::NotificationQueue* notifications)
        BSLS_KEYWORD_OVERRIDE;

    /// Shutdown the stream socket in the specified 'direction'. Return the
    /// error.
    ntsa::Error shutdown(ntsa::ShutdownType::Value direction)
        BSLS_KEYWORD_OVERRIDE;

    /// Unlink the file corresponding to the socket, if the socket is a
    /// local (a.k.a. Unix domain) socket bound to a non-abstract path.
    /// Return the error.
    ntsa::Error unlink() BSLS_KEYWORD_OVERRIDE;

    /// Close the socket. Return the error.
    ntsa::Error close() BSLS_KEYWORD_OVERRIDE;

    /// Start enqueuing events that occur for this session to the specified
    /// 'monitor'. Return the error.
    ntsa::Error registerMonitor(const bsl::shared_ptr<ntcd::Monitor>& monitor);

    /// Stop enqueuing events that occur for this session to the specified
    /// 'monitor'. Return the error.
    ntsa::Error deregisterMonitor(
        const bsl::shared_ptr<ntcd::Monitor>& monitor);

    /// Step the simulation of this session. If the specified 'block' flag
    /// is true, block until each packet queue is available to dequeue and
    /// enqueue. Return the error.
    ntsa::Error step(bool block);

    /// Return the handle to the descriptor.
    ntsa::Handle handle() const BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the source endpoint of this socket.
    /// Return the error.
    ntsa::Error sourceEndpoint(ntsa::Endpoint* result) const
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the remote endpoint to which this
    /// socket is connected. Return the error.
    ntsa::Error remoteEndpoint(ntsa::Endpoint* result) const
        BSLS_KEYWORD_OVERRIDE;

    // *** Multicasting ***

    /// Set the flag that indicates multicast datagrams should be looped
    /// back to the local host to the specified 'value'. Return the error.
    ntsa::Error setMulticastLoopback(bool enabled) BSLS_KEYWORD_OVERRIDE;

    /// Set the network interface on which multicast datagrams will be
    /// sent to the network interface assigned the specified 'interface'
    /// address. Return the error.
    ntsa::Error setMulticastInterface(const ntsa::IpAddress& interface)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the multicast time-to-live to the specified 'numHops'. Return
    /// the error.
    ntsa::Error setMulticastTimeToLive(bsl::size_t maxHops)
        BSLS_KEYWORD_OVERRIDE;

    /// Join the specified multicast 'group' on the adapter identified by
    /// the specified 'interface'. Return the error.
    ntsa::Error joinMulticastGroup(const ntsa::IpAddress& interface,
                                   const ntsa::IpAddress& group)
        BSLS_KEYWORD_OVERRIDE;

    /// Leave the specified multicast 'group' on the adapter identified by
    /// the specified 'interface'. Return the error.
    ntsa::Error leaveMulticastGroup(const ntsa::IpAddress& interface,
                                    const ntsa::IpAddress& group)
        BSLS_KEYWORD_OVERRIDE;

    // *** Socket Options ***

    /// Set the option for the 'socket' that controls its blocking mode
    /// according to the specified 'blocking' flag. Return the error.
    ntsa::Error setBlocking(bool blocking) BSLS_KEYWORD_OVERRIDE;

    /// Set the specified 'option' for this socket. Return the error.
    ntsa::Error setOption(const ntsa::SocketOption& option)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'option' the socket option of the specified
    /// 'type' set for this socket. Return the error.
    ntsa::Error getOption(ntsa::SocketOption*           option,
                          ntsa::SocketOptionType::Value type)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the last known error encountered
    /// when connecting the socket. Return the error (retrieving the error).
    ntsa::Error getLastError(ntsa::Error* result) BSLS_KEYWORD_OVERRIDE;

    /// Return the maximum number of buffers that can be the source of a
    /// gathered write. Additional buffers beyond this limit are silently
    /// ignored.
    bsl::size_t maxBuffersPerSend() const BSLS_KEYWORD_OVERRIDE;

    /// Return the maximum number of buffers that can be the destination
    /// of a scattered read. Additional buffers beyond this limit are
    /// silently ignored.
    bsl::size_t maxBuffersPerReceive() const BSLS_KEYWORD_OVERRIDE;

    /// Return the number of bytes readable.
    bsl::size_t bytesReadable() const;

    /// Return the number of bytes writable.
    bsl::size_t bytesWritable() const;

    /// Return true if the session is readable, otherwise return false.
    bool isReadable() const;

    /// Return true if the session is writable, otherwise return false.
    bool isWritable() const;

    /// Return true if the session has an error, otherwise return false.
    bool hasError() const;

    /// Return true if the session has a notification, otherwise return false.
    bool hasNotification() const;
};

/// @internal @brief
/// Provide a mechanism to poll simulated communication sessions.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class Monitor : public ntccfg::Shared<Monitor>
{
    class Entry;
    // The struct describes an entry recording a session, the user's
    // interest in events, and the readiness of events.

    /// Define a type alias for a list of entries.
    typedef bsl::list<bsl::shared_ptr<Entry> > EntryQueue;

    /// Defines a type alias for the map of handles to interest and
    /// readiness of events for the sessions identified by those handles.
    typedef bsl::unordered_map<ntsa::Handle, bsl::shared_ptr<Entry> > EntryMap;

    ntccfg::ConditionMutex           d_mutex;
    ntccfg::Condition                d_condition;
    bsls::AtomicBool                 d_run;
    bsls::AtomicUint64               d_interrupt;
    bsls::AtomicUint64               d_waiters;
    EntryMap                         d_map;
    EntryQueue                       d_queue;
    bsl::shared_ptr<ntcd::Machine>   d_machine_sp;
    ntca::ReactorEventTrigger::Value d_trigger;
    bool                             d_oneShot;
    bslma::Allocator*                d_allocator_p;

  private:
    Monitor(const Monitor&) BSLS_KEYWORD_DELETED;
    Monitor& operator=(const Monitor&) BSLS_KEYWORD_DELETED;

  private:
    /// Dequeue all available events and append them into the specified
    /// 'result'. Return the number of events pushed onto 'result'.
    bsl::size_t process(bsl::vector<ntca::ReactorEvent>* result);

    /// Insert the specified 'entry' into the queue if the entry is not in
    /// the queue and there is interest in an event and that event is
    /// enabled.
    void insertQueueEntry(const bsl::shared_ptr<Entry>& entry);

    /// Remove the specified 'entry' from the queue if the entry is in the
    /// queue and there is no interest in any event and no event is enabled.
    void removeQueueEntry(const bsl::shared_ptr<Entry>& entry);

  public:
    /// Create a new monitor for the specified 'machine'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Monitor(const bsl::shared_ptr<ntcd::Machine>& machine,
                     bslma::Allocator*                     basicAllocator = 0);

    /// Destroy this object.
    ~Monitor();

    /// Set the default trigger behavior to the specified 'trigger'.
    void setTrigger(ntca::ReactorEventTrigger::Value trigger);

    /// Set the default one-shot behavior to the specified 'oneShot'.
    void setOneShot(bool oneShot);

    /// Register a waiter.
    void registerWaiter();

    /// Deregister a waiter.
    void deregisterWaiter();

    /// Add the session identified by the specified 'handle' to the
    /// interest set. Return the error.
    ntsa::Error add(ntsa::Handle handle);

    /// Add the specified 'session' to the interest set. Return the error.
    ntsa::Error add(const bsl::shared_ptr<ntcd::Session>& session);

    /// Remove the session identified by the specified 'handle' from the
    /// interest set. Return the error.
    ntsa::Error remove(ntsa::Handle handle);

    /// Remove the specified 'session' from the interest set. Return the
    /// error.
    ntsa::Error remove(const bsl::shared_ptr<ntcd::Session>& session);

    /// Update the interest for the session identified by the specified
    /// 'handle' to the specified 'interest', gaining or losing interest in
    /// each event type as necessary. Return the error.
    ntsa::Error update(ntsa::Handle handle, ntcs::Interest interest);

    /// Update the interest for the specified 'session' to the specified
    /// 'interest', gaining or losing interest in each event type as
    /// necessary. Return the error.
    ntsa::Error update(const bsl::shared_ptr<ntcd::Session>& session,
                       ntcs::Interest                        interest);

    /// Gain interest in the specified event 'type' for the session
    /// identified by the specified 'handle'. Return the error.
    ntsa::Error show(ntsa::Handle handle, ntca::ReactorEventType::Value type);

    /// Gain interest in the specified event 'type' for the specified
    /// 'session'. Return the error.
    ntsa::Error show(const bsl::shared_ptr<ntcd::Session>& session,
                     ntca::ReactorEventType::Value         type);

    /// Lose interest in the specified event 'type' for the session
    /// identified by the specified 'handle'. Return the error.
    ntsa::Error hide(ntsa::Handle handle, ntca::ReactorEventType::Value type);

    /// Gain interest in the specified event 'type' for the specified
    /// 'session'. Return the error.
    ntsa::Error hide(const bsl::shared_ptr<ntcd::Session>& session,
                     ntca::ReactorEventType::Value         type);

    /// Start returning events of the specified 'type' having the specified
    /// 'context' for the specified 'session' when polled. Return the error.
    ntsa::Error enable(ntsa::Handle                          handle,
                       const bsl::shared_ptr<ntcd::Session>& session,
                       ntca::ReactorEventType::Value         type);

    /// Start returning events of the e_ERROR type (indicating notifications
    /// are there)  for the specified 'handle' for the specified 'session' when
    // polled. Return the error.
    ntsa::Error enableNotifications(
        ntsa::Handle                          handle,
        const bsl::shared_ptr<ntcd::Session>& session);

    /// Stop returning events of the specified 'type' for the specified
    /// 'session' when polled. Return the error.
    ntsa::Error disable(ntsa::Handle                          handle,
                        const bsl::shared_ptr<ntcd::Session>& session,
                        ntca::ReactorEventType::Value         type);

    /// Stop returning notifications for the specified 'handle' for the
    /// specified 'session'.
    ntsa::Error disableNotifications(
        ntsa::Handle                          handle,
        const bsl::shared_ptr<ntcd::Session>& session);

    /// Block until one or more events have been enqueued. Dequeue all
    /// available events and append them into the specified 'result'. Return
    /// the error.
    ntsa::Error dequeue(bsl::vector<ntca::ReactorEvent>* result);

    /// Block until one or more events have been enqueued or the specified
    /// absolute 'timeout' has elapsed. Dequeue all available events and
    /// append them into the specified 'result'. Return the error.
    ntsa::Error dequeue(bsl::vector<ntca::ReactorEvent>* result,
                        const bsls::TimeInterval&        timeout);

    /// Unblock one waiter blocked on 'dequeue'.
    void interruptOne();

    /// Unblock all waiters blocked on 'dequeue'.
    void interruptAll();

    /// Stop the monitor.
    void stop();

    /// Restart the monitor after being stopped.
    void restart();

    /// Return true if the implementation supports registering events in the
    /// specified 'oneShot' mode, otherwise return false.
    bool supportsOneShot(bool oneShot) const;

    /// Return true if the implementation supports registering events having
    /// the specified 'trigger', otherwise return false.
    bool supportsTrigger(ntca::ReactorEventTrigger::Value trigger) const;
};

/// @internal @brief
/// Provide a simulated machine.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class Machine : public ntccfg::Shared<Machine>
{
    /// Defines a type alias for a map of sessions indexed by handle.
    typedef bsl::map<ntsa::Handle, bsl::weak_ptr<ntcd::Session> >
        SessionByHandleMap;

    /// Define a type alias for a map of sessions indexed by
    /// endpoint.
    typedef bsl::unordered_map<ntsa::Endpoint, bsl::weak_ptr<ntcd::Session> >
        SessionByEndpointMap;

    /// Define a type alias for a map of sessions indexed by
    /// endpoint.
    typedef bsl::map<ntcd::Binding, bsl::weak_ptr<ntcd::Session> >
        SessionByBindingMap;

    mutable ntccfg::ConditionMutex d_mutex;
    mutable ntccfg::Condition      d_condition;
    bsl::string                    d_name;
    bsl::vector<ntsa::IpAddress>   d_ipAddressList;
    bdlbb::PooledBlobBufferFactory d_blobBufferFactory;
    SessionByHandleMap             d_sessionByHandleMap;
    ntcd::PortMap                  d_tcpPortMap;
    ntcd::PortMap                  d_udpPortMap;
    SessionByEndpointMap           d_sessionByTcpEndpointMap;
    SessionByEndpointMap           d_sessionByUdpEndpointMap;
    SessionByEndpointMap           d_sessionByLocalEndpointMap;
    SessionByBindingMap            d_sessionByTcpBindingMap;
    SessionByBindingMap            d_sessionByUdpBindingMap;
    SessionByBindingMap            d_sessionByLocalBindingMap;
    bslmt::ThreadGroup             d_threadGroup;
    bsls::AtomicBool               d_stop;
    bsls::AtomicBool               d_update;
    bslma::Allocator*              d_allocator_p;

  private:
    Machine(const Machine&) BSLS_KEYWORD_DELETED;
    Machine& operator=(const Machine&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Machine(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Machine();

    /// Acquire a reservation of the next available handle for the specified
    /// 'transport'. Load into the specified 'result' the handle acquired and
    /// associate the handle with the specified 'session' Return the error.
    ntsa::Error acquireHandle(ntsa::Handle*                         result,
                              ntsa::Transport::Value                transport,
                              const bsl::shared_ptr<ntcd::Session>& session);

    /// Release the reservation of the specified 'handle' for the specified
    /// 'transport' and dissociate the handle from its session. Return the
    /// error.
    ntsa::Error releaseHandle(ntsa::Handle           handle,
                              ntsa::Transport::Value protocol);

    /// Acquire a reservation of the specified 'port' for the specified
    /// 'transport'. If 'port' is zero, acquire any available ephemeral
    /// port. Load into the specified 'result' the port acquired. Return the
    /// error.
    ntsa::Error acquirePort(ntsa::Port*            result,
                            ntsa::Port             port,
                            ntsa::Transport::Value protocol);

    /// Release the reservation of the specified 'port' for the specified
    /// 'transport'. Return the error.
    ntsa::Error releasePort(ntsa::Port port, ntsa::Transport::Value protocol);

    /// Acquire a reservation of the specified 'sourceEndpoint' for the
    /// specified 'transport'. Load into the specified 'result' the source
    /// endpoint acquired and associate the source endpoint with the
    /// specified 'session' Return the error.
    ntsa::Error acquireSourceEndpoint(
        ntsa::Endpoint*                       result,
        const ntsa::Endpoint&                 sourceEndpoint,
        ntsa::Transport::Value                transport,
        const bsl::shared_ptr<ntcd::Session>& session);

    /// Release the reservation of the specified 'sourceEndpoint' for the
    /// specified 'transport'. Return the error.
    ntsa::Error releaseSourceEndpoint(const ntsa::Endpoint&  sourceEndpoint,
                                      ntsa::Transport::Value protocol);

    /// Acquire a reservation of the specified 'binding' for the specified
    /// 'transport' associated with the specified 'session'. Load into the
    /// specified 'result' resulting effective binding. Return the error.
    ntsa::Error acquireBinding(ntcd::Binding*                        result,
                               const ntcd::Binding&                  binding,
                               ntsa::Transport::Value                transport,
                               const bsl::shared_ptr<ntcd::Session>& session);

    /// Release the reservation of the specified 'binding' for the specified
    /// 'transport'. Return the error.
    ntsa::Error releaseBinding(const ntcd::Binding&   binding,
                               ntsa::Transport::Value transport);

    /// Create a new session. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    bsl::shared_ptr<ntcd::Session> createSession(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new monitor. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    bsl::shared_ptr<ntcd::Monitor> createMonitor(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new packet. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    bsl::shared_ptr<ntcd::Packet> createPacket(
        bslma::Allocator* basicAllocator = 0);

    /// Require an update to the simulation caused by the specified
    /// 'session', i.e. unblock the next call to step the simulation.
    void update(const bsl::shared_ptr<ntcd::Session>& session);

    /// Require an update to the simulation caused by the specified
    /// 'session', i.e. unblock the next call to step the simulation. Do
    /// not acquire a lock on the internal mutex.
    void updateNoLock(const bsl::shared_ptr<ntcd::Session>& session);

    /// Start a background thread and continuously step the simulation
    /// of each session on this machine, as necessary, until the machine
    /// is stopped.
    ntsa::Error run();

    /// Execute the background thread.
    void execute();

    /// Step the simulation of each session on this machine, as necessary.
    /// If the specified 'block' flag is true, block until each packet queue
    /// is available to dequeue and enqueue. Return the error.
    ntsa::Error step(bool block);

    /// Stop stepping the simulation and join the background thread.
    void stop();

    /// Load into the specified 'result' the session associated with the
    /// specified 'handle', if any. Return the error.
    ntsa::Error lookupSession(bsl::weak_ptr<ntcd::Session>* result,
                              ntsa::Handle                  handle) const;

    /// Load into the specified 'result' the session associated with the
    /// specified 'sourceEndpoint' for the specified 'transport', if any.
    /// Return the error.
    ntsa::Error lookupSession(bsl::weak_ptr<ntcd::Session>* result,
                              const ntsa::Endpoint&         sourceEndpoint,
                              ntsa::Transport::Value        protocol) const;

    /// Load into the specified 'result' the session associated with the
    /// specified 'sourceEndpoint' and 'remoteEndpoint' for the specified
    /// 'transport', if any. Return the error.
    ntsa::Error lookupSession(bsl::weak_ptr<ntcd::Session>* result,
                              const ntsa::Endpoint&         sourceEndpoint,
                              const ntsa::Endpoint&         remoteEndpoint,
                              ntsa::Transport::Value        transport) const;

    /// Load into the specified 'result' the session associated with the
    /// specified 'sourceEndpoint' for the specified 'transport', if any.
    /// Return the error.
    ntsa::Error lookupSession(bsl::weak_ptr<ntcd::Session>* result,
                              const ntcd::Binding&          binding,
                              ntsa::Transport::Value        transport) const;

    /// Return the name of the host.
    const bsl::string& name() const;

    /// Return the list of IP addresses assigned to this host.
    const bsl::vector<ntsa::IpAddress>& ipAddressList() const;

    /// Load into the specified 'result' the list of all the network
    /// adapters of the local machine. Note that this function loads
    /// descriptions of all network adapters currently available on the
    /// system, which may be expensive; prefer to cache the results of this
    /// function when possible.
    void discoverAdapterList(bsl::vector<ntsa::Adapter>* result) const;

    /// Load into the specified 'adapter' the first adapter found assigned
    /// an IP address of the specified 'addressType'. Require that the
    /// resulting adapter support multicast according to the specified
    /// 'multicast' flag. Return true if such an adapter is found, and false
    /// otherwise. Note that this function loads descriptions of all network
    /// adapters currently available on the system, which may be expensive;
    /// prefer to cache the results of this function when possible.
    bool discoverAdapter(ntsa::Adapter*             result,
                         ntsa::IpAddressType::Value addressType,
                         bool                       multicast) const;

    /// Return true if this host has been assigned the specified
    /// 'ipAddress', otherwise return false.
    bool hasIpAddress(const ntsa::IpAddress& ipAddress) const;

    /// Return the resolver for this machine.
    bsl::shared_ptr<ntci::Resolver> resolver() const;

    /// Initialize the default machine, if necessary. Return the current
    /// default machine.
    static bsl::shared_ptr<ntcd::Machine> initialize();

    /// Set the current default machine to the specified 'machine'.
    static void setDefault(const bsl::shared_ptr<ntcd::Machine>& machine);

    /// Return the current default machine.
    static bsl::shared_ptr<ntcd::Machine> getDefault();

    /// Destroy the default machine.
    static void exit();
};

}  // close package namespace
}  // close enterprise namespace
#endif
