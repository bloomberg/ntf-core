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

#ifndef INCLUDED_NTSA_SOCKETCONFIG
#define INCLUDED_NTSA_SOCKETCONFIG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_linger.h>
#include <ntsa_socketoption.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsls_timeinterval.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a collection of socket options.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b reuseAddress:
/// The flag that indicates the operating system should allow the user to
/// rebind a socket to reuse local addresses.
///
/// @li @b keepAlive:
/// That flag that indicates the operating system implementation should
/// periodically emit transport-level "keep-alive" packets.
///
/// @li @b cork:
/// The flag that indicates that successive writes should be coalesced into the
/// largest packets that can be formed. When set to true, this option indicates
/// the user favors better network efficiency at the expense of worse latency.
/// When set to false, this option indicates the user favors better latency at
/// the expense of worse network efficiency.
///
/// @li @b delayTransmission:
/// The flag that indicates that successive writes should be coalesced into
/// larger packets that would otherwise form. When set to true, this option
/// indicates the user favors better network efficiency at the expense of worse
/// latency. When set to false, this option indicates the user favors better
/// latency at the expense of worse network efficiency.
///
/// @li @b delayAcknowledgement:
/// The flag that indicates acknowledgement of successively-received packets
/// should be coalesced. When set to true, this option indicates the user
/// favors better network efficiency at the expense of worse latency. When set
/// to false, this option indicates the user favors better latency at the
/// expense of worse network efficiency.
///
/// @li @b sendBufferSize:
/// The maximum size of each socket send buffer. On some platforms, this
/// options may serve simply as a hint.
///
/// @li @b sendBufferLowWatermark:
/// The amount of available capacity that must exist in the socket send buffer
/// for the operating system to indicate the socket is writable.
///
/// @li @b receiveBufferSize:
/// The maximum size of each socket receive buffer. On some platforms, this
/// options may serve simply as a hint.
///
/// @li @b receiveBufferLowWatermark:
/// The amount of available data that must exist in the socket receive buffer
/// for the operating system to indicate the socket is readable.
///
/// @li @b debug:
/// This flag indicates that each socket should be put into debug mode in the
/// operating system. The support and meaning of this option is
/// platform-specific.
///
/// @li @b linger:
/// The options that control whether the operating system should gracefully
/// attempt to transmit any data remaining in the socket send buffer before
/// closing the connection.
///
/// @li @b broadcast:
/// The flag that indicates the socket supports sending to a broadcast address.
///
/// @li @b bypassRouting:
/// The flag that indicates that normal routing rules are not used, the route
/// is based upon the destination address only.
///
/// @li @b inlineOutOfBandData:
/// The flag that indicates out-of-band data should be placed into the normal
/// data input queue.
///
/// @li @b timestampIncomingData:
/// The flag that indicates timestamps should be generated for outgoing data.
///
/// @li @b timestampOutgoingData:
/// The flag that indicates timestamps should be generated for incoming data.
///
/// @li @b zeroCopy:
/// The flag that indicates each send operation can request copy avoidance when
/// enqueing data to the socket send buffer.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class SocketConfig
{
    bdlb::NullableValue<bool>         d_reuseAddress;
    bdlb::NullableValue<bool>         d_keepAlive;
    bdlb::NullableValue<bool>         d_cork;
    bdlb::NullableValue<bool>         d_delayTransmission;
    bdlb::NullableValue<bool>         d_delayAcknowledgement;
    bdlb::NullableValue<bsl::size_t>  d_sendBufferSize;
    bdlb::NullableValue<bsl::size_t>  d_sendBufferLowWatermark;
    bdlb::NullableValue<bsl::size_t>  d_receiveBufferSize;
    bdlb::NullableValue<bsl::size_t>  d_receiveBufferLowWatermark;
    bdlb::NullableValue<bool>         d_debug;
    bdlb::NullableValue<ntsa::Linger> d_linger;
    bdlb::NullableValue<bool>         d_broadcast;
    bdlb::NullableValue<bool>         d_bypassRouting;
    bdlb::NullableValue<bool>         d_inlineOutOfBandData;
    bdlb::NullableValue<bool>         d_timestampIncomingData;
    bdlb::NullableValue<bool>         d_timestampOutgoingData;
    bdlb::NullableValue<bool>         d_zeroCopy;

  public:
    /// Create new send options having the default value.
    SocketConfig();

    /// Create new send options having the same value as the specified
    /// 'original' object.
    SocketConfig(const SocketConfig& original);

    /// Destroy this object.
    ~SocketConfig();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    SocketConfig& operator=(const SocketConfig& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the specified 'option'.
    void setOption(const ntsa::SocketOption& option);

    /// Set the flag that indicates the operating system should allow the
    /// user to rebind a socket to reuse local addresses to the specified
    /// 'value'.
    void setReuseAddress(bool value);

    /// Set the flag that indicates the operating system implementation
    /// should periodically emit transport-level "keep-alive" packets to the
    /// specified 'value'.
    void setKeepAlive(bool value);

    /// Set the cork flag to the specified 'value'.
    void setCork(bool value);

    /// Set the flag that indicates that subsequent writes should be
    /// coalesced into the larger packets that would otherwise form to the
    /// specified 'value'.
    void setDelayTransmission(bool value);

    /// Set the delayed acknowledgement flag to the specified 'value'.
    void setDelayAcknowledgment(bool value);

    /// Set the maximum size of each socket send buffer to the specified
    /// 'value'.
    void setSendBufferSize(bsl::size_t value);

    /// Set the amount of available capacity that must exist in the socket
    /// send buffer for the operating system to indicate the socket is
    /// writable to the specified 'value'.
    void setSendBufferLowWatermark(bsl::size_t value);

    /// Set the maximum size of each socket receive buffer to the specified
    /// 'value'.
    void setReceiveBufferSize(bsl::size_t value);

    /// Set the amount of available data that must exist in the socket
    /// receive buffer for the operating system to indicate the socket is
    /// readable to the specified 'value'.
    void setReceiveBufferLowWatermark(bsl::size_t value);

    /// Set the debug flag to the specified 'value'.
    void setDebug(bool value);

    /// Set the linger specification to the specified 'value'.
    void setLinger(const ntsa::Linger& value);

    /// Set the flag that indicates the socket supports sending to a
    /// broadcast address to the specified 'value'.
    void setBroadcast(bool value);

    /// Set the flag that indicates that normal routing rules are not used,
    /// the route is based upon the destination address only to the
    /// specified 'value'.
    void setBypassRouting(bool value);

    /// Set the flag that indicates out-of-band data should be placed into
    /// the normal data input queue to the specified 'value'.
    void setInlineOutOfBandData(bool value);

    /// Set the flag that indicates incoming data should be timestamped to the
    /// specified 'value'.
    void setTimestampIncomingData(bool value);

    /// Set the flag that indicates outgoing data should be timestamped to the
    /// specified 'value'.
    void setTimestampOutgoingData(bool value);

    /// Set the flag that indicates each send operation can request copy
    /// avoidance when enqueing data to the socket send buffer to the specified
    /// 'value'.
    void setZeroCopy(bool value);

    /// Load into the specified 'option' the option for the specified
    /// 'type'. Note that if the option for the 'type' is not set, the
    /// resulting 'option->isUndefined()' will be true.
    void getOption(ntsa::SocketOption*           option,
                   ntsa::SocketOptionType::Value type);

    /// Return the flag that indicates the operating system should allow the
    /// user to rebind a socket to reuse local addresses.
    const bdlb::NullableValue<bool>& reuseAddress() const;

    /// Return flag that indicates the operating system implementation
    /// should periodically emit transport-level "keep-alive" packets.
    const bdlb::NullableValue<bool>& keepAlive() const;

    /// Return the cork flag.
    const bdlb::NullableValue<bool>& cork() const;

    /// Return the flag that indicates that subsequent writes should be
    /// coalesced into the larger packets that would otherwise form
    const bdlb::NullableValue<bool>& delayTransmission() const;

    /// Return the delayed acknowledgement flag.
    const bdlb::NullableValue<bool>& delayAcknowledgement() const;

    /// Return the maximum size of each socket send buffer.
    const bdlb::NullableValue<bsl::size_t>& sendBufferSize() const;

    /// Return the amount of available capacity that must exist in the
    /// socket send buffer for the operating system to indicate the socket
    /// is writable.
    const bdlb::NullableValue<bsl::size_t>& sendBufferLowWatermark() const;

    /// Return the maximum size of each socket receive buffer.
    const bdlb::NullableValue<bsl::size_t>& receiveBufferSize() const;

    /// Return the amount of available data that must exist in the socket
    /// receive buffer for the operating system to indicate the socket is
    /// readable
    const bdlb::NullableValue<bsl::size_t>& receiveBufferLowWatermark() const;

    /// Return the debug flag.
    const bdlb::NullableValue<bool>& debug() const;

    /// Return the linger specification.
    const bdlb::NullableValue<ntsa::Linger>& linger() const;

    /// Return the flag that indicates the socket supports sending to a
    /// broadcast address.
    const bdlb::NullableValue<bool>& broadcast() const;

    /// Return the flag that indicates that normal routing rules are not
    /// used, the route is based upon the destination address only.
    const bdlb::NullableValue<bool>& bypassRouting() const;

    /// Return the flag that indicates out-of-band data should be placed
    /// into the normal data input queue.
    const bdlb::NullableValue<bool>& inlineOutOfBandData() const;

    /// Return the flag that indicates incoming data should be timestamped.
    const bdlb::NullableValue<bool>& timestampIncomingData() const;

    /// Return the flag that indicates outgoing data should be timestamped.
    const bdlb::NullableValue<bool>& timestampOutgoingData() const;

    /// Return the flag that indicates each send operation can request copy
    /// avoidance when enqueing data to the socket send buffer.
    const bdlb::NullableValue<bool>& zeroCopy() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const SocketConfig& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const SocketConfig& other) const;

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
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(SocketConfig);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::SocketConfig
bsl::ostream& operator<<(bsl::ostream& stream, const SocketConfig& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::SocketConfig
bool operator==(const SocketConfig& lhs, const SocketConfig& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::SocketConfig
bool operator!=(const SocketConfig& lhs, const SocketConfig& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::SocketConfig
bool operator<(const SocketConfig& lhs, const SocketConfig& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::SocketConfig
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const SocketConfig& value);

NTSCFG_INLINE
SocketConfig::SocketConfig()
: d_reuseAddress()
, d_keepAlive()
, d_cork()
, d_delayTransmission()
, d_delayAcknowledgement()
, d_sendBufferSize()
, d_sendBufferLowWatermark()
, d_receiveBufferSize()
, d_receiveBufferLowWatermark()
, d_debug()
, d_linger()
, d_broadcast()
, d_bypassRouting()
, d_inlineOutOfBandData()
, d_timestampIncomingData()
, d_timestampOutgoingData()
, d_zeroCopy()
{
}

NTSCFG_INLINE
SocketConfig::SocketConfig(const SocketConfig& original)
: d_reuseAddress(original.d_reuseAddress)
, d_keepAlive(original.d_keepAlive)
, d_cork(original.d_cork)
, d_delayTransmission(original.d_delayTransmission)
, d_delayAcknowledgement(original.d_delayAcknowledgement)
, d_sendBufferSize(original.d_sendBufferSize)
, d_sendBufferLowWatermark(original.d_sendBufferLowWatermark)
, d_receiveBufferSize(original.d_receiveBufferSize)
, d_receiveBufferLowWatermark(original.d_receiveBufferLowWatermark)
, d_debug(original.d_debug)
, d_linger(original.d_linger)
, d_broadcast(original.d_broadcast)
, d_bypassRouting(original.d_bypassRouting)
, d_inlineOutOfBandData(original.d_inlineOutOfBandData)
, d_timestampIncomingData(original.d_timestampIncomingData)
, d_timestampOutgoingData(original.d_timestampOutgoingData)
, d_zeroCopy(original.d_zeroCopy)
{
}

NTSCFG_INLINE
SocketConfig::~SocketConfig()
{
}

NTSCFG_INLINE
SocketConfig& SocketConfig::operator=(const SocketConfig& other)
{
    d_reuseAddress              = other.d_reuseAddress;
    d_keepAlive                 = other.d_keepAlive;
    d_cork                      = other.d_cork;
    d_delayTransmission         = other.d_delayTransmission;
    d_delayAcknowledgement      = other.d_delayAcknowledgement;
    d_sendBufferSize            = other.d_sendBufferSize;
    d_sendBufferLowWatermark    = other.d_sendBufferLowWatermark;
    d_receiveBufferSize         = other.d_receiveBufferSize;
    d_receiveBufferLowWatermark = other.d_receiveBufferLowWatermark;
    d_debug                     = other.d_debug;
    d_linger                    = other.d_linger;
    d_broadcast                 = other.d_broadcast;
    d_bypassRouting             = other.d_bypassRouting;
    d_inlineOutOfBandData       = other.d_inlineOutOfBandData;
    d_timestampIncomingData     = other.d_timestampIncomingData;
    d_timestampOutgoingData     = other.d_timestampOutgoingData;
    d_zeroCopy                  = other.d_zeroCopy;

    return *this;
}

NTSCFG_INLINE
void SocketConfig::reset()
{
    d_reuseAddress.reset();
    d_keepAlive.reset();
    d_cork.reset();
    d_delayTransmission.reset();
    d_delayAcknowledgement.reset();
    d_sendBufferSize.reset();
    d_sendBufferLowWatermark.reset();
    d_receiveBufferSize.reset();
    d_receiveBufferLowWatermark.reset();
    d_debug.reset();
    d_linger.reset();
    d_broadcast.reset();
    d_bypassRouting.reset();
    d_inlineOutOfBandData.reset();
    d_timestampIncomingData.reset();
    d_timestampOutgoingData.reset();
    d_zeroCopy.reset();
}

NTSCFG_INLINE
void SocketConfig::setReuseAddress(bool value)
{
    d_reuseAddress = value;
}

NTSCFG_INLINE
void SocketConfig::setKeepAlive(bool value)
{
    d_keepAlive = value;
}

NTSCFG_INLINE
void SocketConfig::setCork(bool value)
{
    d_cork = value;
}

NTSCFG_INLINE
void SocketConfig::setDelayTransmission(bool value)
{
    d_delayTransmission = value;
}

NTSCFG_INLINE
void SocketConfig::setDelayAcknowledgment(bool value)
{
    d_delayAcknowledgement = value;
}

NTSCFG_INLINE
void SocketConfig::setSendBufferSize(bsl::size_t value)
{
    d_sendBufferSize = value;
}

NTSCFG_INLINE
void SocketConfig::setSendBufferLowWatermark(bsl::size_t value)
{
    d_sendBufferLowWatermark = value;
}

NTSCFG_INLINE
void SocketConfig::setReceiveBufferSize(bsl::size_t value)
{
    d_receiveBufferSize = value;
}

NTSCFG_INLINE
void SocketConfig::setReceiveBufferLowWatermark(bsl::size_t value)
{
    d_receiveBufferLowWatermark = value;
}

NTSCFG_INLINE
void SocketConfig::setDebug(bool value)
{
    d_debug = value;
}

NTSCFG_INLINE
void SocketConfig::setLinger(const ntsa::Linger& value)
{
    d_linger = value;
}

NTSCFG_INLINE
void SocketConfig::setBroadcast(bool value)
{
    d_broadcast = value;
}

NTSCFG_INLINE
void SocketConfig::setBypassRouting(bool value)
{
    d_bypassRouting = value;
}

NTSCFG_INLINE
void SocketConfig::setInlineOutOfBandData(bool value)
{
    d_inlineOutOfBandData = value;
}

NTSCFG_INLINE
void SocketConfig::setTimestampIncomingData(bool value)
{
    d_timestampIncomingData = value;
}

NTSCFG_INLINE
void SocketConfig::setTimestampOutgoingData(bool value)
{
    d_timestampOutgoingData = value;
}

NTSCFG_INLINE
void SocketConfig::setZeroCopy(bool value)
{
    d_zeroCopy = value;
}

NTSCFG_INLINE
const bdlb::NullableValue<bool>& SocketConfig::reuseAddress() const
{
    return d_reuseAddress;
}

NTSCFG_INLINE
const bdlb::NullableValue<bool>& SocketConfig::keepAlive() const
{
    return d_keepAlive;
}

NTSCFG_INLINE
const bdlb::NullableValue<bool>& SocketConfig::cork() const
{
    return d_cork;
}

NTSCFG_INLINE
const bdlb::NullableValue<bool>& SocketConfig::delayTransmission() const
{
    return d_delayTransmission;
}

NTSCFG_INLINE
const bdlb::NullableValue<bool>& SocketConfig::delayAcknowledgement() const
{
    return d_delayAcknowledgement;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& SocketConfig::sendBufferSize() const
{
    return d_sendBufferSize;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& SocketConfig::sendBufferLowWatermark()
    const
{
    return d_sendBufferLowWatermark;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& SocketConfig::receiveBufferSize() const
{
    return d_receiveBufferSize;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& SocketConfig::
    receiveBufferLowWatermark() const
{
    return d_receiveBufferLowWatermark;
}

NTSCFG_INLINE
const bdlb::NullableValue<bool>& SocketConfig::debug() const
{
    return d_debug;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Linger>& SocketConfig::linger() const
{
    return d_linger;
}

NTSCFG_INLINE
const bdlb::NullableValue<bool>& SocketConfig::broadcast() const
{
    return d_broadcast;
}

NTSCFG_INLINE
const bdlb::NullableValue<bool>& SocketConfig::bypassRouting() const
{
    return d_bypassRouting;
}

NTSCFG_INLINE
const bdlb::NullableValue<bool>& SocketConfig::inlineOutOfBandData() const
{
    return d_inlineOutOfBandData;
}

NTSCFG_INLINE
const bdlb::NullableValue<bool>& SocketConfig::timestampIncomingData() const
{
    return d_timestampIncomingData;
}

NTSCFG_INLINE
const bdlb::NullableValue<bool>& SocketConfig::timestampOutgoingData() const
{
    return d_timestampOutgoingData;
}

NTSCFG_INLINE
const bdlb::NullableValue<bool>& SocketConfig::zeroCopy() const
{
    return d_zeroCopy;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const SocketConfig& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const SocketConfig& lhs, const SocketConfig& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const SocketConfig& lhs, const SocketConfig& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const SocketConfig& lhs, const SocketConfig& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const SocketConfig& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.reuseAddress());
    hashAppend(algorithm, value.keepAlive());
    hashAppend(algorithm, value.cork());
    hashAppend(algorithm, value.delayTransmission());
    hashAppend(algorithm, value.delayAcknowledgement());
    hashAppend(algorithm, value.sendBufferSize());
    hashAppend(algorithm, value.sendBufferLowWatermark());
    hashAppend(algorithm, value.receiveBufferSize());
    hashAppend(algorithm, value.receiveBufferLowWatermark());
    hashAppend(algorithm, value.debug());
    hashAppend(algorithm, value.linger());
    hashAppend(algorithm, value.broadcast());
    hashAppend(algorithm, value.bypassRouting());
    hashAppend(algorithm, value.inlineOutOfBandData());
    hashAppend(algorithm, value.timestampIncomingData());
    hashAppend(algorithm, value.timestampOutgoingData());
    hashAppend(algorithm, value.zeroCopy());
}

}  // close package namespace
}  // close enterprise namespace
#endif
