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

#ifndef INCLUDED_NTSA_SOCKETINFO
#define INCLUDED_NTSA_SOCKETINFO

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_endpoint.h>
#include <ntsa_handle.h>
#include <ntsa_socketstate.h>
#include <ntsa_transport.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bslmt_threadutil.h>
#include <bsls_timeinterval.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a description of a socket in the operating system.
///
/// @details
/// Provide a value-semantic type that describes a socket in the
/// operating system.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b descriptor:
/// The socket file descriptor, if known.
///
/// @li @b threadId:
/// The thread ID assigned to perform I/O for this socket, if statically
/// load-balanced and known.
///
/// @li @b creationTime:
/// The creation time of the socket, if known.
///
/// @li @b transport:
/// The transport used by the socket.
///
/// @li @b sourceEndpoint:
/// The source endpoint to which the socket is bound, if any.
///
/// @li @b remoteEndpoint:
/// The remote endpoint to which the socket is connected, if any.
///
/// @li @b state:
/// The state of the socket in its protocol.
///
/// @li @b sendQueueSize:
/// The current size of the outgoing queue. For datagram sockets and stream
/// sockets, this value represents the number of bytes in the send buffer
/// waiting to be transmitted. For listening sockets, this value is always
/// zero.
///
/// @li @b receiveQueueSize:
/// The current size of the incoming queue. For datagram sockets and stream
/// sockets, this value represents the number of bytes in the receive buffer
/// that may be copied into userspace. For listening sockets this value
/// represents the number of incoming connections that may be accepted.
///
/// @li @b userId:
/// The effective user ID of the process that created the socket.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class SocketInfo
{
    bdlb::NullableValue<ntsa::Handle>       d_descriptor;
    bdlb::NullableValue<bsl::uint64_t>      d_threadId;
    bdlb::NullableValue<bsls::TimeInterval> d_creationTime;
    ntsa::Transport::Value                  d_transport;
    ntsa::Endpoint                          d_sourceEndpoint;
    ntsa::Endpoint                          d_remoteEndpoint;
    ntsa::SocketState::Value                d_state;
    bsl::size_t                             d_sendQueueSize;
    bsl::size_t                             d_receiveQueueSize;
    bsl::uint32_t                           d_userId;

  public:
    /// Create a new socket status.
    SocketInfo();

    /// Create a new socket status having the same value as the specified
    /// original object.
    SocketInfo(const SocketInfo& original);

    /// Destroy this object.
    ~SocketInfo();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    SocketInfo& operator=(const SocketInfo& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the socket descriptor to the specified 'value'.
    void setDescriptor(ntsa::Handle value);

    /// Set the thread ID of the thread assigned to perform I/O for the socket
    /// to the specified 'value'.
    void setThreadId(bsl::uint64_t value);

    /// Set the creation time of the socket to the specified 'value'.
    void setCreationTime(const bsls::TimeInterval& value);

    /// Set the transport to the specified 'transport'.
    void setTransport(ntsa::Transport::Value value);

    /// Set the source endpoint to the specified 'value'.
    void setSourceEndpoint(const ntsa::Endpoint& value);

    /// Set the remote endpoint to the specified 'value'.
    void setRemoteEndpoint(const ntsa::Endpoint& value);

    /// Set the state to the specified 'value'.
    void setState(ntsa::SocketState::Value value);

    /// Set the send queue size to the specified 'value'.
    void setSendQueueSize(bsl::size_t value);

    /// Set the receive queue size to the specified 'value'.
    void setReceiveQueueSize(bsl::size_t value);

    /// Set the user ID to the specified 'value'.
    void setUserId(bsl::uint32_t value);

    /// Return the socket descriptor, if known.
    const bdlb::NullableValue<ntsa::Handle>& descriptor() const;

    /// Return the thread ID of the thread assigned to perform I/O for the
    /// socket, if known and the thread is statically load-balanced.
    const bdlb::NullableValue<bsl::uint64_t>& threadId() const;

    /// Return the creation time of the socket, if known.
    const bdlb::NullableValue<bsls::TimeInterval>& creationTime() const;

    /// Return the transport.
    ntsa::Transport::Value transport() const;

    /// Return the source endpoint.
    const ntsa::Endpoint& sourceEndpoint() const;

    /// Return the remote endpoint.
    const ntsa::Endpoint& remoteEndpoint() const;

    /// Return the state.
    ntsa::SocketState::Value state() const;

    /// Return the send queue size.
    bsl::size_t sendQueueSize() const;

    /// Return the receive queue size.
    bsl::size_t receiveQueueSize() const;

    /// Return the user ID.
    bsl::uint32_t userId() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const SocketInfo& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const SocketInfo& other) const;

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

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(SocketInfo);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(SocketInfo);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::SocketInfo
bsl::ostream& operator<<(bsl::ostream& stream, const SocketInfo& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::SocketInfo
bool operator==(const SocketInfo& lhs, const SocketInfo& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::SocketInfo
bool operator!=(const SocketInfo& lhs, const SocketInfo& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::SocketInfo
bool operator<(const SocketInfo& lhs, const SocketInfo& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::SocketInfo
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const SocketInfo& value);

NTSCFG_INLINE
SocketInfo::SocketInfo()
: d_descriptor()
, d_threadId()
, d_creationTime()
, d_transport(ntsa::Transport::e_UNDEFINED)
, d_sourceEndpoint()
, d_remoteEndpoint()
, d_state(ntsa::SocketState::e_UNDEFINED)
, d_sendQueueSize(0)
, d_receiveQueueSize(0)
, d_userId(0)
{
}

NTSCFG_INLINE
SocketInfo::SocketInfo(const SocketInfo& original)
: d_descriptor(original.d_descriptor)
, d_threadId(original.d_threadId)
, d_creationTime(original.d_creationTime)
, d_transport(original.d_transport)
, d_sourceEndpoint(original.d_sourceEndpoint)
, d_remoteEndpoint(original.d_remoteEndpoint)
, d_state(original.d_state)
, d_sendQueueSize(original.d_sendQueueSize)
, d_receiveQueueSize(original.d_receiveQueueSize)
, d_userId(original.d_userId)
{
}

NTSCFG_INLINE
SocketInfo::~SocketInfo()
{
}

NTSCFG_INLINE
SocketInfo& SocketInfo::operator=(const SocketInfo& other)
{
    if (this != &other) {
        d_descriptor       = other.d_descriptor;
        d_threadId         = other.d_threadId;
        d_creationTime     = other.d_creationTime;
        d_transport        = other.d_transport;
        d_sourceEndpoint   = other.d_sourceEndpoint;
        d_remoteEndpoint   = other.d_remoteEndpoint;
        d_state            = other.d_state;
        d_sendQueueSize    = other.d_sendQueueSize;
        d_receiveQueueSize = other.d_receiveQueueSize;
        d_userId           = other.d_userId;
    }

    return *this;
}

NTSCFG_INLINE
void SocketInfo::reset()
{
    d_descriptor.reset();
    d_threadId.reset();
    d_creationTime.reset();
    d_transport = ntsa::Transport::e_UNDEFINED;
    d_sourceEndpoint.reset();
    d_remoteEndpoint.reset();
    d_state            = ntsa::SocketState::e_UNDEFINED;
    d_sendQueueSize    = 0;
    d_receiveQueueSize = 0;
    d_userId           = 0;
}

NTSCFG_INLINE
void SocketInfo::setDescriptor(ntsa::Handle value)
{
    d_descriptor = value;
}

NTSCFG_INLINE
void SocketInfo::setThreadId(bsl::uint64_t value)
{
    d_threadId = value;
}

NTSCFG_INLINE
void SocketInfo::setCreationTime(const bsls::TimeInterval& value)
{
    d_creationTime = value;
}

NTSCFG_INLINE
void SocketInfo::setTransport(ntsa::Transport::Value value)
{
    d_transport = value;
}

NTSCFG_INLINE
void SocketInfo::setSourceEndpoint(const ntsa::Endpoint& value)
{
    d_sourceEndpoint = value;
}

NTSCFG_INLINE
void SocketInfo::setRemoteEndpoint(const ntsa::Endpoint& value)
{
    d_remoteEndpoint = value;
}

NTSCFG_INLINE
void SocketInfo::setState(ntsa::SocketState::Value value)
{
    d_state = value;
}

NTSCFG_INLINE
void SocketInfo::setSendQueueSize(bsl::size_t value)
{
    d_sendQueueSize = value;
}

NTSCFG_INLINE
void SocketInfo::setReceiveQueueSize(bsl::size_t value)
{
    d_receiveQueueSize = value;
}

NTSCFG_INLINE
void SocketInfo::setUserId(bsl::uint32_t value)
{
    d_userId = value;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Handle>& SocketInfo::descriptor() const
{
    return d_descriptor;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::uint64_t>& SocketInfo::threadId() const
{
    return d_threadId;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsls::TimeInterval>& SocketInfo::creationTime() const
{
    return d_creationTime;
}

NTSCFG_INLINE
ntsa::Transport::Value SocketInfo::transport() const
{
    return d_transport;
}

NTSCFG_INLINE
const ntsa::Endpoint& SocketInfo::sourceEndpoint() const
{
    return d_sourceEndpoint;
}

NTSCFG_INLINE
const ntsa::Endpoint& SocketInfo::remoteEndpoint() const
{
    return d_remoteEndpoint;
}

NTSCFG_INLINE
ntsa::SocketState::Value SocketInfo::state() const
{
    return d_state;
}

NTSCFG_INLINE
bsl::size_t SocketInfo::sendQueueSize() const
{
    return d_sendQueueSize;
}

NTSCFG_INLINE
bsl::size_t SocketInfo::receiveQueueSize() const
{
    return d_receiveQueueSize;
}

NTSCFG_INLINE
bsl::uint32_t SocketInfo::userId() const
{
    return d_userId;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const SocketInfo& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const SocketInfo& lhs, const SocketInfo& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const SocketInfo& lhs, const SocketInfo& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const SocketInfo& lhs, const SocketInfo& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const SocketInfo& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.descriptor());
    hashAppend(algorithm, value.threadId());
    hashAppend(algorithm, value.creationTime());
    hashAppend(algorithm, value.transport());
    hashAppend(algorithm, value.sourceEndpoint());
    hashAppend(algorithm, value.remoteEndpoint());
    hashAppend(algorithm, value.state());
    hashAppend(algorithm, value.sendQueueSize());
    hashAppend(algorithm, value.receiveQueueSize());
    hashAppend(algorithm, value.userId());
}

}  // close package namespace
}  // close enterprise namespace
#endif
