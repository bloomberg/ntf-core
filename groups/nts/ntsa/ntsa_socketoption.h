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

#ifndef INCLUDED_NTSA_SOCKETOPTION
#define INCLUDED_NTSA_SOCKETOPTION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_linger.h>
#include <ntsa_socketoptiontype.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a union of socket options.
///
/// @details
/// Provide a value-semantic type that represents a discriminated union of 
/// socket options.
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
class SocketOption
{
    union {
        bsls::ObjectBuffer<bool>         d_reuseAddress;
        bsls::ObjectBuffer<bool>         d_keepAlive;
        bsls::ObjectBuffer<bool>         d_cork;
        bsls::ObjectBuffer<bool>         d_delayTransmission;
        bsls::ObjectBuffer<bool>         d_delayAcknowledgement;
        bsls::ObjectBuffer<bsl::size_t>  d_sendBufferSize;
        bsls::ObjectBuffer<bsl::size_t>  d_sendBufferLowWatermark;
        bsls::ObjectBuffer<bsl::size_t>  d_receiveBufferSize;
        bsls::ObjectBuffer<bsl::size_t>  d_receiveBufferLowWatermark;
        bsls::ObjectBuffer<bool>         d_debug;
        bsls::ObjectBuffer<ntsa::Linger> d_linger;
        bsls::ObjectBuffer<bool>         d_broadcast;
        bsls::ObjectBuffer<bool>         d_bypassRouting;
        bsls::ObjectBuffer<bool>         d_inlineOutOfBandData;
        bsls::ObjectBuffer<bool>         d_timestampIncomingData;
        bsls::ObjectBuffer<bool>         d_timestampOutgoingData;
        bsls::ObjectBuffer<bool>         d_zeroCopy;
    };

    ntsa::SocketOptionType::Value d_type;

  public:
    /// Create a new socketoption having an undefined type.
    SocketOption();

    /// Create a new address having the same value as the specified 'other'
    /// object.
    SocketOption(const SocketOption& other);

    /// Destroy this object.
    ~SocketOption();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    SocketOption& operator=(const SocketOption& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Select the "reuseAddress" representation. Return a reference to the
    /// modifiable representation.
    bool& makeReuseAddress();

    /// Select the "reuseAddress" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    bool& makeReuseAddress(bool value);

    /// Select the "keepAlive" representation. Return a reference to the
    /// modifiable representation.
    bool& makeKeepAlive();

    /// Select the "keepAlive" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    bool& makeKeepAlive(bool value);

    /// Select the "cork" representation. Return a reference to the modifiable
    /// representation.
    bool& makeCork();

    /// Select the "cork" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    bool& makeCork(bool value);

    /// Select the "delayTransmission" representation. Return a reference to
    /// the modifiable representation.
    bool& makeDelayTransmission();

    /// Select the "delayTransmission" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    bool& makeDelayTransmission(bool value);

    /// Select the "delayAcknowledgement" representation. Return a reference to
    /// the modifiable representation.
    bool& makeDelayAcknowledgement();

    /// Select the "delayAcknowledgement" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    bool& makeDelayAcknowledgement(bool value);

    /// Select the "sendBufferSize" representation. Return a reference to the
    /// modifiable representation.
    bsl::size_t& makeSendBufferSize();

    /// Select the "sendBufferSize" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    bsl::size_t& makeSendBufferSize(bsl::size_t value);

    /// Select the "sendBufferLowWatermark" representation. Return a reference
    /// to the modifiable representation.
    bsl::size_t& makeSendBufferLowWatermark();

    /// Select the "sendBufferLowWatermark" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    bsl::size_t& makeSendBufferLowWatermark(bsl::size_t value);

    /// Select the "receiveBufferSize" representation. Return a reference to
    /// the modifiable representation.
    bsl::size_t& makeReceiveBufferSize();

    /// Select the "receiveBufferSize" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    bsl::size_t& makeReceiveBufferSize(bsl::size_t value);

    /// Select the "receiveBufferLowWatermark" representation. Return a
    /// reference to the modifiable representation.
    bsl::size_t& makeReceiveBufferLowWatermark();

    /// Select the "receiveBufferLowWatermark" representation initially having
    /// the specified 'value'. Return a reference to the modifiable
    /// representation.
    bsl::size_t& makeReceiveBufferLowWatermark(bsl::size_t value);

    /// Select the "debug" representation. Return a reference to the modifiable
    /// representation.
    bool& makeDebug();

    /// Select the "debug" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    bool& makeDebug(bool value);

    /// Select the "linger" representation. Return a reference to the
    /// modifiable representation.
    ntsa::Linger& makeLinger();

    /// Select the "linger" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::Linger& makeLinger(const ntsa::Linger& value);

    /// Select the "broadcast" representation. Return a reference to the
    /// modifiable representation.
    bool& makeBroadcast();

    /// Select the "broadcast" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    bool& makeBroadcast(bool value);

    /// Select the "bypassRouting" representation. Return a reference to the
    /// modifiable representation.
    bool& makeBypassRouting();

    /// Select the "bypassRouting" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    bool& makeBypassRouting(bool value);

    /// Select the "inlineOutOfBandData" representation. Return a reference to
    /// the modifiable representation.
    bool& makeInlineOutOfBandData();

    /// Select the "inlineOutOfBandData" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    bool& makeInlineOutOfBandData(bool value);

    /// Select the "timestampIncomingData" representation. Return a reference
    /// to the modifiable representation.
    bool& makeTimestampIncomingData();

    /// Select the "timestampIncomingData" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    bool& makeTimestampIncomingData(bool value);

    /// Select the "timestampOutgoingData" representation. Return a reference
    /// to the modifiable representation.
    bool& makeTimestampOutgoingData();

    /// Select the "timestampOutgoingData" representation initially having the
    /// specified 'value'. Return a reference to the modifiable representation.
    bool& makeTimestampOutgoingData(bool value);

    /// Select the "zeroCopy" representation. Return a reference to the
    /// modifiable representation.
    bool& makeZeroCopy();

    /// Select the "zeroCopy" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    bool& makeZeroCopy(bool value);

    /// Return a reference to the modifiable "reuseAddress" representation. The
    /// behavior is undefined unless 'isReuseAddress()' is true.
    bool& reuseAddress();

    /// Return a reference to the modifiable "keepAlive" representation. The
    /// behavior is undefined unless 'isKeepAlive()' is true.
    bool& keepAlive();

    /// Return a reference to the modifiable "cork" representation. The
    /// behavior is undefined unless 'isCork()' is true.
    bool& cork();

    /// Return a reference to the modifiable "delayTransmission"
    /// representation. The behavior is undefined unless
    /// 'isDelayTransmission()' is true.
    bool& delayTransmission();

    /// Return a reference to the modifiable "delayAcknowledgement"
    /// representation. The behavior is undefined unless
    /// 'isDelayAcknowledgement()' is true.
    bool& delayAcknowledgement();

    /// Return a reference to the modifiable "sendBufferSize" representation.
    /// The behavior is undefined unless 'isSendBufferSize()' is true.
    bsl::size_t& sendBufferSize();

    /// Return a reference to the modifiable "sendBufferLowWatermark"
    /// representation. The behavior is undefined unless
    /// 'isSendBufferLowWatermark()' is true.
    bsl::size_t& sendBufferLowWatermark();

    /// Return a reference to the modifiable "receiveBufferSize"
    /// representation. The behavior is undefined unless
    /// 'isReceiveBufferSize()' is true.
    bsl::size_t& receiveBufferSize();

    /// Return a reference to the modifiable "receiveBufferLowWatermark"
    /// representation. The behavior is undefined unless
    /// 'isReceiveBufferLowWatermark()' is true.
    bsl::size_t& receiveBufferLowWatermark();

    /// Return a reference to the modifiable "debug" representation. The
    /// behavior is undefined unless 'isDebug()' is true.
    bool& debug();

    /// Return a reference to the modifiable "linger" representation. The
    /// behavior is undefined unless 'isLinger()' is true.
    ntsa::Linger& linger();

    /// Return a reference to the modifiable "broadcast" representation. The
    /// behavior is undefined unless 'isBroadcast()' is true.
    bool& broadcast();

    /// Return a reference to the modifiable "bypassRouting" representation.
    /// The behavior is undefined unless 'isBypassRouting()' is true.
    bool& bypassRouting();

    /// Return a reference to the modifiable "inlineOutOfBandData"
    /// representation. The behavior is undefined unless
    /// 'isInlineOutOfBandData()' is true.
    bool& inlineOutOfBandData();

    /// Return a reference to the modifiable "timestampIncomingData"
    /// representation. The behavior is undefined unless
    /// 'isTimestampIncomingData()' is true.
    bool& timestampIncomingData();

    /// Return a reference to the modifiable "timestampOutgoingData"
    /// representation. The behavior is undefined unless
    /// 'isTimestampOutOutgoingData()' is true.
    bool& timestampOutgoingData();

    /// Return a reference to the modifiable "zeroCopy" representation. The
    /// behavior is undefined unless 'isZeroCopy()' is true.
    bool& zeroCopy();

    /// Return the non-modifiable "reuseAddress" representation. The behavior
    /// is undefined unless 'isReuseAddress()' is true.
    bool reuseAddress() const;

    /// Return the non-modifiable "keepAlive" representation. The behavior is
    /// undefined unless 'isKeepAlive()' is true.
    bool keepAlive() const;

    /// Return the non-modifiable "cork" representation. The behavior is
    /// undefined unless 'isCork()' is true.
    bool cork() const;

    /// Return the non-modifiable "delayTransmission" representation. The
    /// behavior is undefined unless 'isDelayTransmission()' is true.
    bool delayTransmission() const;

    /// Return the non-modifiable "delayAcknowledgement" representation. The
    /// behavior is undefined unless 'isDelayAcknowledgement()' is true.
    bool delayAcknowledgement() const;

    /// Return the non-modifiable "sendBufferSize" representation. The behavior
    /// is undefined unless 'isSendBufferSize()' is true.
    bsl::size_t sendBufferSize() const;

    /// Return the non-modifiable "sendBufferLowWatermark" representation. The
    /// behavior is undefined unless 'isSendBufferLowWatermark()' is true.
    bsl::size_t sendBufferLowWatermark() const;

    /// Return the non-modifiable "receiveBufferSize" representation. The
    /// behavior is undefined unless 'isReceiveBufferSize()' is true.
    bsl::size_t receiveBufferSize() const;

    /// Return the non-modifiable "receiveBufferLowWatermark" representation.
    /// The behavior is undefined unless 'isReceiveBufferLowWatermark()' is
    /// true.
    bsl::size_t receiveBufferLowWatermark() const;

    /// Return the non-modifiable "debug" representation. The behavior is
    /// undefined unless 'isDebug()' is true.
    bool debug() const;

    /// Return a reference to the non-modifiable "linger" representation. The
    /// behavior is undefined unless 'isLinger()' is true.
    const ntsa::Linger& linger() const;

    /// Return the non-modifiable "broadcast" representation. The behavior is
    /// undefined unless 'isBroadcast()' is true.
    bool broadcast() const;

    /// Return the non-modifiable "bypassRouting" representation. The behavior
    /// is undefined unless 'isBypassRouting()' is true.
    bool bypassRouting() const;

    /// Return the non-modifiable "inlineOutOfBandData" representation. The
    /// behavior is undefined unless 'isInlineOutOfBandData()' is true.
    bool inlineOutOfBandData() const;

    /// Return the non-modifiable "timestampIncomingData" representation. The
    /// behavior is undefined unless 'isTimestampIncomingData()' is true.
    bool timestampIncomingData() const;

    /// Return the non-modifiable "timestampOutgoingData" representation. The
    /// behavior is undefined unless 'isTimestampOutgoingData()' is true.
    bool timestampOutgoingData() const;

    /// Return the non-modifiable "zeroCopy" representation. The behavior is
    /// undefined unless 'isZeroCopy()' is true.
    bool zeroCopy() const;

    /// Return the type of the option representation.
    enum ntsa::SocketOptionType::Value type() const;

    /// Return true if the option representation is undefined, otherwise return
    /// false.
    bool isUndefined() const;

    /// Return true if the "reuseAddress" representation is currently selected,
    /// otherwise return false.
    bool isReuseAddress() const;

    /// Return true if the "keepAlive" representation is currently selected,
    /// otherwise return false.
    bool isKeepAlive() const;

    /// Return true if the "cork" representation is currently selected,
    /// otherwise return false.
    bool isCork() const;

    /// Return true if the "delayTransmission" representation is currently
    /// selected, otherwise return false.
    bool isDelayTransmission() const;

    /// Return true if the "delayAcknowledgment" representation is currently
    /// selected, otherwise return false.
    bool isDelayAcknowledgment() const;

    /// Return true if the "sendBufferSize" representation is currently
    /// selected, otherwise return false.
    bool isSendBufferSize() const;

    /// Return true if the "sendBufferLowWatermark" representation is currently
    /// selected, otherwise return false.
    bool isSendBufferLowWatermark() const;

    /// Return true if the "receiveBufferSize" representation is currently
    /// selected, otherwise return false.
    bool isReceiveBufferSize() const;

    /// Return true if the "receiveBufferLowWatermark" representation is
    /// currently selected, otherwise return false.
    bool isReceiveBufferLowWatermark() const;

    /// Return true if the "debug" representation is currently selected,
    /// otherwise return false.
    bool isDebug() const;

    /// Return true if the "linger" representation is currently selected,
    /// otherwise return false.
    bool isLinger() const;

    /// Return true if the "broadcast" representation is currently selected,
    /// otherwise return false.
    bool isBroadcast() const;

    /// Return true if the "bypassRouting" representation is currently
    /// selected, otherwise return false.
    bool isBypassRouting() const;

    /// Return true if the "inlineOutOfBandData" representation is currently
    /// selected, otherwise return false.
    bool isInlineOutOfBandData() const;

    /// Return true if the "timestampIncomingData" representation is currently
    /// selected, otherwise return false.
    bool isTimestampIncomingData() const;

    /// Return true if the "timestampOutgoingData" representation is currently
    /// selected, otherwise return false.
    bool isTimestampOutgoingData() const;

    /// Return true if the "zeroCopy" representation is currently selected,
    /// otherwise return false.
    bool isZeroCopy() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const SocketOption& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const SocketOption& other) const;

    /// Format this object to the specified output 'stream' at the optionally
    /// specified indentation 'level' and return a reference to the modifiable
    /// 'stream'.  If 'level' is specified, optionally specify
    /// 'spacesPerLevel', the number of spaces per indentation level for this
    /// and all of its nested objects.  Each line is indented by the absolute
    /// value of 'level * spacesPerLevel'.  If 'level' is negative, suppress
    /// indentation of the first line.  If 'spacesPerLevel' is negative,
    /// suppress line breaks and format the entire output on one line.  If
    /// 'stream' is initially invalid, this operation has no effect.  Note that
    /// a trailing newline is provided in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Defines the traits of this type. These traits can be used to select, at
    /// compile-time, the most efficient algorithm to manipulate objects of
    /// this type.
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(SocketOption);
};

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
///
/// @related ntsa::SocketOption
bsl::ostream& operator<<(bsl::ostream& stream, const SocketOption& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::SocketOption
bool operator==(const SocketOption& lhs, const SocketOption& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::SocketOption
bool operator!=(const SocketOption& lhs, const SocketOption& rhs);

/// Contribute the values of the salient attributes of the specified 'value' to
/// the specified hash 'algorithm'.
///
/// @related ntsa::SocketOption
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const SocketOption& value);

NTSCFG_INLINE
SocketOption::SocketOption()
: d_type(ntsa::SocketOptionType::e_UNDEFINED)
{
}

NTSCFG_INLINE
SocketOption::~SocketOption()
{
}

NTSCFG_INLINE
void SocketOption::reset()
{
    d_type = ntsa::SocketOptionType::e_UNDEFINED;
}

NTSCFG_INLINE
bool& SocketOption::reuseAddress()
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_REUSE_ADDRESS);
    return d_reuseAddress.object();
}

NTSCFG_INLINE
bool& SocketOption::keepAlive()
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_KEEP_ALIVE);
    return d_keepAlive.object();
}

NTSCFG_INLINE
bool& SocketOption::cork()
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_CORK);
    return d_cork.object();
}

NTSCFG_INLINE
bool& SocketOption::delayTransmission()
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_DELAY_TRANSMISSION);
    return d_delayTransmission.object();
}

NTSCFG_INLINE
bool& SocketOption::delayAcknowledgement()
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_DELAY_ACKNOWLEDGEMENT);
    return d_delayAcknowledgement.object();
}

NTSCFG_INLINE
bsl::size_t& SocketOption::sendBufferSize()
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_SEND_BUFFER_SIZE);
    return d_sendBufferSize.object();
}

NTSCFG_INLINE
bsl::size_t& SocketOption::sendBufferLowWatermark()
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_SEND_BUFFER_LOW_WATERMARK);
    return d_sendBufferLowWatermark.object();
}

NTSCFG_INLINE
bsl::size_t& SocketOption::receiveBufferSize()
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE);
    return d_receiveBufferSize.object();
}

NTSCFG_INLINE
bsl::size_t& SocketOption::receiveBufferLowWatermark()
{
    BSLS_ASSERT(d_type ==
                ntsa::SocketOptionType::e_RECEIVE_BUFFER_LOW_WATERMARK);
    return d_receiveBufferLowWatermark.object();
}

NTSCFG_INLINE
bool& SocketOption::debug()
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_DEBUG);
    return d_debug.object();
}

NTSCFG_INLINE
ntsa::Linger& SocketOption::linger()
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_LINGER);
    return d_linger.object();
}

NTSCFG_INLINE
bool& SocketOption::broadcast()
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_BROADCAST);
    return d_broadcast.object();
}

NTSCFG_INLINE
bool& SocketOption::bypassRouting()
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_BYPASS_ROUTING);
    return d_bypassRouting.object();
}

NTSCFG_INLINE
bool& SocketOption::inlineOutOfBandData()
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_INLINE_OUT_OF_BAND_DATA);
    return d_inlineOutOfBandData.object();
}

NTSCFG_INLINE
bool& SocketOption::timestampIncomingData()
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_RX_TIMESTAMPING);
    return d_timestampIncomingData.object();
}

NTSCFG_INLINE
bool& SocketOption::timestampOutgoingData()
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_TX_TIMESTAMPING);
    return d_timestampOutgoingData.object();
}

NTSCFG_INLINE
bool& SocketOption::zeroCopy()
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_ZERO_COPY);
    return d_zeroCopy.object();
}

NTSCFG_INLINE
bool SocketOption::reuseAddress() const
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_REUSE_ADDRESS);
    return d_reuseAddress.object();
}

NTSCFG_INLINE
bool SocketOption::keepAlive() const
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_KEEP_ALIVE);
    return d_keepAlive.object();
}

NTSCFG_INLINE
bool SocketOption::cork() const
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_CORK);
    return d_cork.object();
}

NTSCFG_INLINE
bool SocketOption::delayTransmission() const
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_DELAY_TRANSMISSION);
    return d_delayTransmission.object();
}

NTSCFG_INLINE
bool SocketOption::delayAcknowledgement() const
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_DELAY_ACKNOWLEDGEMENT);
    return d_delayAcknowledgement.object();
}

NTSCFG_INLINE
bsl::size_t SocketOption::sendBufferSize() const
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_SEND_BUFFER_SIZE);
    return d_sendBufferSize.object();
}

NTSCFG_INLINE
bsl::size_t SocketOption::sendBufferLowWatermark() const
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_SEND_BUFFER_LOW_WATERMARK);
    return d_sendBufferLowWatermark.object();
}

NTSCFG_INLINE
bsl::size_t SocketOption::receiveBufferSize() const
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE);
    return d_receiveBufferSize.object();
}

NTSCFG_INLINE
bsl::size_t SocketOption::receiveBufferLowWatermark() const
{
    BSLS_ASSERT(d_type ==
                ntsa::SocketOptionType::e_RECEIVE_BUFFER_LOW_WATERMARK);
    return d_receiveBufferLowWatermark.object();
}

NTSCFG_INLINE
bool SocketOption::debug() const
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_DEBUG);
    return d_debug.object();
}

NTSCFG_INLINE
const ntsa::Linger& SocketOption::linger() const
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_LINGER);
    return d_linger.object();
}

NTSCFG_INLINE
bool SocketOption::broadcast() const
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_BROADCAST);
    return d_broadcast.object();
}

NTSCFG_INLINE
bool SocketOption::bypassRouting() const
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_BYPASS_ROUTING);
    return d_bypassRouting.object();
}

NTSCFG_INLINE
bool SocketOption::inlineOutOfBandData() const
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_INLINE_OUT_OF_BAND_DATA);
    return d_inlineOutOfBandData.object();
}

NTSCFG_INLINE
bool SocketOption::timestampIncomingData() const
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_RX_TIMESTAMPING);
    return d_timestampIncomingData.object();
}

NTSCFG_INLINE
bool SocketOption::timestampOutgoingData() const
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_TX_TIMESTAMPING);
    return d_timestampOutgoingData.object();
}

NTSCFG_INLINE
bool SocketOption::zeroCopy() const
{
    BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_ZERO_COPY);
    return d_zeroCopy.object();
}

NTSCFG_INLINE
ntsa::SocketOptionType::Value SocketOption::type() const
{
    return d_type;
}

NTSCFG_INLINE
bool SocketOption::isUndefined() const
{
    return (d_type == ntsa::SocketOptionType::e_UNDEFINED);
}

NTSCFG_INLINE
bool SocketOption::isReuseAddress() const
{
    return (d_type == ntsa::SocketOptionType::e_REUSE_ADDRESS);
}

NTSCFG_INLINE
bool SocketOption::isKeepAlive() const
{
    return (d_type == ntsa::SocketOptionType::e_KEEP_ALIVE);
}

NTSCFG_INLINE
bool SocketOption::isCork() const
{
    return (d_type == ntsa::SocketOptionType::e_CORK);
}

NTSCFG_INLINE
bool SocketOption::isDelayTransmission() const
{
    return (d_type == ntsa::SocketOptionType::e_DELAY_TRANSMISSION);
}

NTSCFG_INLINE
bool SocketOption::isDelayAcknowledgment() const
{
    return (d_type == ntsa::SocketOptionType::e_DELAY_ACKNOWLEDGEMENT);
}

NTSCFG_INLINE
bool SocketOption::isSendBufferSize() const
{
    return (d_type == ntsa::SocketOptionType::e_SEND_BUFFER_SIZE);
}

NTSCFG_INLINE
bool SocketOption::isSendBufferLowWatermark() const
{
    return (d_type == ntsa::SocketOptionType::e_SEND_BUFFER_LOW_WATERMARK);
}

NTSCFG_INLINE
bool SocketOption::isReceiveBufferSize() const
{
    return (d_type == ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE);
}

NTSCFG_INLINE
bool SocketOption::isReceiveBufferLowWatermark() const
{
    return (d_type == ntsa::SocketOptionType::e_RECEIVE_BUFFER_LOW_WATERMARK);
}

NTSCFG_INLINE
bool SocketOption::isDebug() const
{
    return (d_type == ntsa::SocketOptionType::e_DEBUG);
}

NTSCFG_INLINE
bool SocketOption::isLinger() const
{
    return (d_type == ntsa::SocketOptionType::e_LINGER);
}

NTSCFG_INLINE
bool SocketOption::isBroadcast() const
{
    return (d_type == ntsa::SocketOptionType::e_BROADCAST);
}

NTSCFG_INLINE
bool SocketOption::isBypassRouting() const
{
    return (d_type == ntsa::SocketOptionType::e_BYPASS_ROUTING);
}

NTSCFG_INLINE
bool SocketOption::isInlineOutOfBandData() const
{
    return (d_type == ntsa::SocketOptionType::e_INLINE_OUT_OF_BAND_DATA);
}

NTSCFG_INLINE
bool SocketOption::isTimestampIncomingData() const
{
    return (d_type == ntsa::SocketOptionType::e_RX_TIMESTAMPING);
}

NTSCFG_INLINE
bool SocketOption::isTimestampOutgoingData() const
{
    return (d_type == ntsa::SocketOptionType::e_TX_TIMESTAMPING);
}

NTSCFG_INLINE
bool SocketOption::isZeroCopy() const
{
    return (d_type == ntsa::SocketOptionType::e_ZERO_COPY);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const SocketOption& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const SocketOption& lhs, const SocketOption& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const SocketOption& lhs, const SocketOption& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const SocketOption& lhs, const SocketOption& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const SocketOption& value)
{
    using bslh::hashAppend;

    if (value.isReuseAddress()) {
        hashAppend(algorithm, value.reuseAddress());
    }
    else if (value.isKeepAlive()) {
        hashAppend(algorithm, value.keepAlive());
    }
    else if (value.isCork()) {
        hashAppend(algorithm, value.cork());
    }
    else if (value.isDelayTransmission()) {
        hashAppend(algorithm, value.delayTransmission());
    }
    else if (value.isDelayAcknowledgment()) {
        hashAppend(algorithm, value.delayAcknowledgement());
    }
    else if (value.isSendBufferSize()) {
        hashAppend(algorithm, value.sendBufferSize());
    }
    else if (value.isSendBufferLowWatermark()) {
        hashAppend(algorithm, value.sendBufferLowWatermark());
    }
    else if (value.isReceiveBufferSize()) {
        hashAppend(algorithm, value.receiveBufferSize());
    }
    else if (value.isReceiveBufferLowWatermark()) {
        hashAppend(algorithm, value.receiveBufferLowWatermark());
    }
    else if (value.isDebug()) {
        hashAppend(algorithm, value.debug());
    }
    else if (value.isLinger()) {
        hashAppend(algorithm, value.linger());
    }
    else if (value.isBroadcast()) {
        hashAppend(algorithm, value.broadcast());
    }
    else if (value.isBypassRouting()) {
        hashAppend(algorithm, value.bypassRouting());
    }
    else if (value.isInlineOutOfBandData()) {
        hashAppend(algorithm, value.inlineOutOfBandData());
    }
    else if (value.isTimestampIncomingData()) {
        hashAppend(algorithm, value.timestampIncomingData());
    }
    else if (value.isTimestampOutgoingData()) {
        hashAppend(algorithm, value.timestampOutgoingData());
    }
    else if (value.isZeroCopy()) {
        hashAppend(algorithm, value.zeroCopy());
    }
}

}  // close package namespace
}  // close enterprise namespace
#endif
