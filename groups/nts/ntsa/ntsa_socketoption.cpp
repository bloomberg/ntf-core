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

#include <ntsa_socketoption.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_socketoption_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntsa {

SocketOption::SocketOption(const SocketOption& other)
: d_type(other.d_type)
{
    switch (d_type) {
    case ntsa::SocketOptionType::e_REUSE_ADDRESS:
        new (d_reuseAddress.buffer()) bool(other.d_reuseAddress.object());
        break;
    case ntsa::SocketOptionType::e_KEEP_ALIVE:
        new (d_keepAlive.buffer()) bool(other.d_keepAlive.object());
        break;
    case ntsa::SocketOptionType::e_CORK:
        new (d_cork.buffer()) bool(other.d_cork.object());
        break;
    case ntsa::SocketOptionType::e_DELAY_TRANSMISSION:
        new (d_delayTransmission.buffer()) bool(
            other.d_delayTransmission.object());
        break;
    case ntsa::SocketOptionType::e_DELAY_ACKNOWLEDGEMENT:
        new (d_delayAcknowledgement.buffer()) bool(
            other.d_delayAcknowledgement.object());
        break;
    case ntsa::SocketOptionType::e_SEND_BUFFER_SIZE:
        new (d_sendBufferSize.buffer())
            bsl::size_t(other.d_sendBufferSize.object());
        break;
    case ntsa::SocketOptionType::e_SEND_BUFFER_LOW_WATERMARK:
        new (d_sendBufferLowWatermark.buffer())
            bsl::size_t(other.d_sendBufferLowWatermark.object());
        break;
    case ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE:
        new (d_receiveBufferSize.buffer())
            bsl::size_t(other.d_receiveBufferSize.object());
        break;
    case ntsa::SocketOptionType::e_RECEIVE_BUFFER_LOW_WATERMARK:
        new (d_receiveBufferLowWatermark.buffer())
            bsl::size_t(other.d_receiveBufferLowWatermark.object());
        break;
    case ntsa::SocketOptionType::e_DEBUG:
        new (d_debug.buffer()) bool(other.d_debug.object());
        break;
    case ntsa::SocketOptionType::e_LINGER:
        new (d_linger.buffer()) ntsa::Linger(other.d_linger.object());
        break;
    case ntsa::SocketOptionType::e_BROADCAST:
        new (d_broadcast.buffer()) bool(other.d_broadcast.object());
        break;
    case ntsa::SocketOptionType::e_BYPASS_ROUTING:
        new (d_bypassRouting.buffer()) bool(other.d_bypassRouting.object());
        break;
    case ntsa::SocketOptionType::e_INLINE_OUT_OF_BAND_DATA:
        new (d_inlineOutOfBandData.buffer()) bool(
            other.d_inlineOutOfBandData.object());
        break;
    case ntsa::SocketOptionType::e_TX_TIMESTAMPING:
        new (d_timestampOutgoingData.buffer()) bool(
            other.d_timestampOutgoingData.object());
        break;
    case ntsa::SocketOptionType::e_RX_TIMESTAMPING:
        new (d_timestampIncomingData.buffer()) bool(
            other.d_timestampIncomingData.object());
        break;
    case ntsa::SocketOptionType::e_ZERO_COPY:
        new (d_zeroCopy.buffer()) bool(
            other.d_zeroCopy.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_UNDEFINED);
    }
}

SocketOption& SocketOption::operator=(const SocketOption& other)
{
    if (this == &other) {
        return *this;
    }

    this->reset();

    switch (other.d_type) {
    case ntsa::SocketOptionType::e_REUSE_ADDRESS:
        new (d_reuseAddress.buffer()) bool(other.d_reuseAddress.object());
        break;
    case ntsa::SocketOptionType::e_KEEP_ALIVE:
        new (d_keepAlive.buffer()) bool(other.d_keepAlive.object());
        break;
    case ntsa::SocketOptionType::e_CORK:
        new (d_cork.buffer()) bool(other.d_cork.object());
        break;
    case ntsa::SocketOptionType::e_DELAY_TRANSMISSION:
        new (d_delayTransmission.buffer()) bool(
            other.d_delayTransmission.object());
        break;
    case ntsa::SocketOptionType::e_DELAY_ACKNOWLEDGEMENT:
        new (d_delayAcknowledgement.buffer()) bool(
            other.d_delayAcknowledgement.object());
        break;
    case ntsa::SocketOptionType::e_SEND_BUFFER_SIZE:
        new (d_sendBufferSize.buffer())
            bsl::size_t(other.d_sendBufferSize.object());
        break;
    case ntsa::SocketOptionType::e_SEND_BUFFER_LOW_WATERMARK:
        new (d_sendBufferLowWatermark.buffer())
            bsl::size_t(other.d_sendBufferLowWatermark.object());
        break;
    case ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE:
        new (d_receiveBufferSize.buffer())
            bsl::size_t(other.d_receiveBufferSize.object());
        break;
    case ntsa::SocketOptionType::e_RECEIVE_BUFFER_LOW_WATERMARK:
        new (d_receiveBufferLowWatermark.buffer())
            bsl::size_t(other.d_receiveBufferLowWatermark.object());
        break;
    case ntsa::SocketOptionType::e_DEBUG:
        new (d_debug.buffer()) bool(other.d_debug.object());
        break;
    case ntsa::SocketOptionType::e_LINGER:
        new (d_linger.buffer()) ntsa::Linger(other.d_linger.object());
        break;
    case ntsa::SocketOptionType::e_BROADCAST:
        new (d_broadcast.buffer()) bool(other.d_broadcast.object());
        break;
    case ntsa::SocketOptionType::e_BYPASS_ROUTING:
        new (d_bypassRouting.buffer()) bool(other.d_bypassRouting.object());
        break;
    case ntsa::SocketOptionType::e_INLINE_OUT_OF_BAND_DATA:
        new (d_inlineOutOfBandData.buffer()) bool(
            other.d_inlineOutOfBandData.object());
        break;
    case ntsa::SocketOptionType::e_RX_TIMESTAMPING:
        new (d_timestampIncomingData.buffer()) bool(
            other.d_timestampIncomingData.object());
        break;
    case ntsa::SocketOptionType::e_TX_TIMESTAMPING:
        new (d_timestampOutgoingData.buffer()) bool(
            other.d_timestampOutgoingData.object());
        break;
    case ntsa::SocketOptionType::e_ZERO_COPY:
        new (d_zeroCopy.buffer()) bool(
            other.d_zeroCopy.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_UNDEFINED);
    }

    d_type = other.d_type;

    return *this;
}

bool& SocketOption::makeReuseAddress()
{
    if (d_type == ntsa::SocketOptionType::e_REUSE_ADDRESS) {
        d_reuseAddress.object() = false;
    }
    else {
        this->reset();
        new (d_reuseAddress.buffer()) bool();
        d_type = ntsa::SocketOptionType::e_REUSE_ADDRESS;
    }

    return d_reuseAddress.object();
}

bool& SocketOption::makeReuseAddress(bool value)
{
    if (d_type == ntsa::SocketOptionType::e_REUSE_ADDRESS) {
        d_reuseAddress.object() = value;
    }
    else {
        this->reset();
        new (d_reuseAddress.buffer()) bool(value);
        d_type = ntsa::SocketOptionType::e_REUSE_ADDRESS;
    }

    return d_reuseAddress.object();
}

bool& SocketOption::makeKeepAlive()
{
    if (d_type == ntsa::SocketOptionType::e_KEEP_ALIVE) {
        d_keepAlive.object() = false;
    }
    else {
        this->reset();
        new (d_keepAlive.buffer()) bool();
        d_type = ntsa::SocketOptionType::e_KEEP_ALIVE;
    }

    return d_keepAlive.object();
}

bool& SocketOption::makeKeepAlive(bool value)
{
    if (d_type == ntsa::SocketOptionType::e_KEEP_ALIVE) {
        d_keepAlive.object() = value;
    }
    else {
        this->reset();
        new (d_keepAlive.buffer()) bool(value);
        d_type = ntsa::SocketOptionType::e_KEEP_ALIVE;
    }

    return d_keepAlive.object();
}

bool& SocketOption::makeCork()
{
    if (d_type == ntsa::SocketOptionType::e_CORK) {
        d_cork.object() = false;
    }
    else {
        this->reset();
        new (d_cork.buffer()) bool();
        d_type = ntsa::SocketOptionType::e_CORK;
    }

    return d_cork.object();
}

bool& SocketOption::makeCork(bool value)
{
    if (d_type == ntsa::SocketOptionType::e_CORK) {
        d_cork.object() = value;
    }
    else {
        this->reset();
        new (d_cork.buffer()) bool(value);
        d_type = ntsa::SocketOptionType::e_CORK;
    }

    return d_cork.object();
}

bool& SocketOption::makeDelayTransmission()
{
    if (d_type == ntsa::SocketOptionType::e_DELAY_TRANSMISSION) {
        d_delayTransmission.object() = false;
    }
    else {
        this->reset();
        new (d_delayTransmission.buffer()) bool();
        d_type = ntsa::SocketOptionType::e_DELAY_TRANSMISSION;
    }

    return d_delayTransmission.object();
}

bool& SocketOption::makeDelayTransmission(bool value)
{
    if (d_type == ntsa::SocketOptionType::e_DELAY_TRANSMISSION) {
        d_delayTransmission.object() = value;
    }
    else {
        this->reset();
        new (d_delayTransmission.buffer()) bool(value);
        d_type = ntsa::SocketOptionType::e_DELAY_TRANSMISSION;
    }

    return d_delayTransmission.object();
}

bool& SocketOption::makeDelayAcknowledgement()
{
    if (d_type == ntsa::SocketOptionType::e_DELAY_ACKNOWLEDGEMENT) {
        d_delayAcknowledgement.object() = false;
    }
    else {
        this->reset();
        new (d_delayAcknowledgement.buffer()) bool();
        d_type = ntsa::SocketOptionType::e_DELAY_ACKNOWLEDGEMENT;
    }

    return d_delayAcknowledgement.object();
}

bool& SocketOption::makeDelayAcknowledgement(bool value)
{
    if (d_type == ntsa::SocketOptionType::e_DELAY_ACKNOWLEDGEMENT) {
        d_delayAcknowledgement.object() = value;
    }
    else {
        this->reset();
        new (d_delayAcknowledgement.buffer()) bool(value);
        d_type = ntsa::SocketOptionType::e_DELAY_ACKNOWLEDGEMENT;
    }

    return d_delayAcknowledgement.object();
}

bsl::size_t& SocketOption::makeSendBufferSize()
{
    if (d_type == ntsa::SocketOptionType::e_SEND_BUFFER_SIZE) {
        d_sendBufferSize.object() = 0;
    }
    else {
        this->reset();
        new (d_sendBufferSize.buffer()) bsl::size_t();
        d_type = ntsa::SocketOptionType::e_SEND_BUFFER_SIZE;
    }

    return d_sendBufferSize.object();
}

bsl::size_t& SocketOption::makeSendBufferSize(bsl::size_t value)
{
    if (d_type == ntsa::SocketOptionType::e_SEND_BUFFER_SIZE) {
        d_sendBufferSize.object() = value;
    }
    else {
        this->reset();
        new (d_sendBufferSize.buffer()) bsl::size_t(value);
        d_type = ntsa::SocketOptionType::e_SEND_BUFFER_SIZE;
    }

    return d_sendBufferSize.object();
}

bsl::size_t& SocketOption::makeSendBufferLowWatermark()
{
    if (d_type == ntsa::SocketOptionType::e_SEND_BUFFER_LOW_WATERMARK) {
        d_sendBufferLowWatermark.object() = 0;
    }
    else {
        this->reset();
        new (d_sendBufferLowWatermark.buffer()) bsl::size_t();
        d_type = ntsa::SocketOptionType::e_SEND_BUFFER_LOW_WATERMARK;
    }

    return d_sendBufferLowWatermark.object();
}

bsl::size_t& SocketOption::makeSendBufferLowWatermark(bsl::size_t value)
{
    if (d_type == ntsa::SocketOptionType::e_SEND_BUFFER_LOW_WATERMARK) {
        d_sendBufferLowWatermark.object() = value;
    }
    else {
        this->reset();
        new (d_sendBufferLowWatermark.buffer()) bsl::size_t(value);
        d_type = ntsa::SocketOptionType::e_SEND_BUFFER_LOW_WATERMARK;
    }

    return d_sendBufferLowWatermark.object();
}

bsl::size_t& SocketOption::makeReceiveBufferSize()
{
    if (d_type == ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE) {
        d_receiveBufferSize.object() = 0;
    }
    else {
        this->reset();
        new (d_receiveBufferSize.buffer()) bsl::size_t();
        d_type = ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE;
    }

    return d_receiveBufferSize.object();
}

bsl::size_t& SocketOption::makeReceiveBufferSize(bsl::size_t value)
{
    if (d_type == ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE) {
        d_receiveBufferSize.object() = value;
    }
    else {
        this->reset();
        new (d_receiveBufferSize.buffer()) bsl::size_t(value);
        d_type = ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE;
    }

    return d_receiveBufferSize.object();
}

bsl::size_t& SocketOption::makeReceiveBufferLowWatermark()
{
    if (d_type == ntsa::SocketOptionType::e_RECEIVE_BUFFER_LOW_WATERMARK) {
        d_receiveBufferLowWatermark.object() = 0;
    }
    else {
        this->reset();
        new (d_receiveBufferLowWatermark.buffer()) bsl::size_t();
        d_type = ntsa::SocketOptionType::e_RECEIVE_BUFFER_LOW_WATERMARK;
    }

    return d_receiveBufferLowWatermark.object();
}

bsl::size_t& SocketOption::makeReceiveBufferLowWatermark(bsl::size_t value)
{
    if (d_type == ntsa::SocketOptionType::e_RECEIVE_BUFFER_LOW_WATERMARK) {
        d_receiveBufferLowWatermark.object() = value;
    }
    else {
        this->reset();
        new (d_receiveBufferLowWatermark.buffer()) bsl::size_t(value);
        d_type = ntsa::SocketOptionType::e_RECEIVE_BUFFER_LOW_WATERMARK;
    }

    return d_receiveBufferLowWatermark.object();
}

bool& SocketOption::makeDebug()
{
    if (d_type == ntsa::SocketOptionType::e_DEBUG) {
        d_debug.object() = false;
    }
    else {
        this->reset();
        new (d_debug.buffer()) bool();
        d_type = ntsa::SocketOptionType::e_DEBUG;
    }

    return d_debug.object();
}

bool& SocketOption::makeDebug(bool value)
{
    if (d_type == ntsa::SocketOptionType::e_DEBUG) {
        d_debug.object() = value;
    }
    else {
        this->reset();
        new (d_debug.buffer()) bool(value);
        d_type = ntsa::SocketOptionType::e_DEBUG;
    }

    return d_debug.object();
}

ntsa::Linger& SocketOption::makeLinger()
{
    if (d_type == ntsa::SocketOptionType::e_LINGER) {
        d_linger.object().reset();
    }
    else {
        this->reset();
        new (d_reuseAddress.buffer()) bool();
        d_type = ntsa::SocketOptionType::e_LINGER;
    }

    return d_linger.object();
}

ntsa::Linger& SocketOption::makeLinger(const ntsa::Linger& value)
{
    if (d_type == ntsa::SocketOptionType::e_LINGER) {
        d_linger.object().reset();
    }
    else {
        this->reset();
        new (d_linger.buffer()) ntsa::Linger(value);
        d_type = ntsa::SocketOptionType::e_LINGER;
    }

    return d_linger.object();
}

bool& SocketOption::makeBroadcast()
{
    if (d_type == ntsa::SocketOptionType::e_BROADCAST) {
        d_broadcast.object() = false;
    }
    else {
        this->reset();
        new (d_broadcast.buffer()) bool();
        d_type = ntsa::SocketOptionType::e_BROADCAST;
    }

    return d_broadcast.object();
}

bool& SocketOption::makeBroadcast(bool value)
{
    if (d_type == ntsa::SocketOptionType::e_BROADCAST) {
        d_broadcast.object() = value;
    }
    else {
        this->reset();
        new (d_broadcast.buffer()) bool(value);
        d_type = ntsa::SocketOptionType::e_BROADCAST;
    }

    return d_broadcast.object();
}

bool& SocketOption::makeBypassRouting()
{
    if (d_type == ntsa::SocketOptionType::e_BYPASS_ROUTING) {
        d_bypassRouting.object() = false;
    }
    else {
        this->reset();
        new (d_bypassRouting.buffer()) bool();
        d_type = ntsa::SocketOptionType::e_BYPASS_ROUTING;
    }

    return d_bypassRouting.object();
}

bool& SocketOption::makeBypassRouting(bool value)
{
    if (d_type == ntsa::SocketOptionType::e_BYPASS_ROUTING) {
        d_bypassRouting.object() = value;
    }
    else {
        this->reset();
        new (d_bypassRouting.buffer()) bool(value);
        d_type = ntsa::SocketOptionType::e_BYPASS_ROUTING;
    }

    return d_bypassRouting.object();
}

bool& SocketOption::makeInlineOutOfBandData()
{
    if (d_type == ntsa::SocketOptionType::e_INLINE_OUT_OF_BAND_DATA) {
        d_inlineOutOfBandData.object() = false;
    }
    else {
        this->reset();
        new (d_inlineOutOfBandData.buffer()) bool();
        d_type = ntsa::SocketOptionType::e_INLINE_OUT_OF_BAND_DATA;
    }

    return d_inlineOutOfBandData.object();
}

bool& SocketOption::makeInlineOutOfBandData(bool value)
{
    if (d_type == ntsa::SocketOptionType::e_INLINE_OUT_OF_BAND_DATA) {
        d_inlineOutOfBandData.object() = value;
    }
    else {
        this->reset();
        new (d_inlineOutOfBandData.buffer()) bool(value);
        d_type = ntsa::SocketOptionType::e_INLINE_OUT_OF_BAND_DATA;
    }

    return d_inlineOutOfBandData.object();
}

bool& SocketOption::makeTimestampIncomingData()
{
    if (d_type == ntsa::SocketOptionType::e_RX_TIMESTAMPING) {
        d_timestampIncomingData.object() = false;
    }
    else {
        this->reset();
        new (d_timestampIncomingData.buffer()) bool();
        d_type = ntsa::SocketOptionType::e_RX_TIMESTAMPING;
    }

    return d_timestampIncomingData.object();
}

bool& SocketOption::makeTimestampIncomingData(bool value)
{
    if (d_type == ntsa::SocketOptionType::e_RX_TIMESTAMPING) {
        d_timestampIncomingData.object() = value;
    }
    else {
        this->reset();
        new (d_timestampIncomingData.buffer()) bool(value);
        d_type = ntsa::SocketOptionType::e_RX_TIMESTAMPING;
    }

    return d_timestampIncomingData.object();
}

bool& SocketOption::makeTimestampOutgoingData()
{
    if (d_type == ntsa::SocketOptionType::e_TX_TIMESTAMPING) {
        d_timestampOutgoingData.object() = false;
    }
    else {
        this->reset();
        new (d_timestampOutgoingData.buffer()) bool();
        d_type = ntsa::SocketOptionType::e_TX_TIMESTAMPING;
    }

    return d_timestampOutgoingData.object();
}

bool& SocketOption::makeTimestampOutgoingData(bool value)
{
    if (d_type == ntsa::SocketOptionType::e_TX_TIMESTAMPING) {
        d_timestampOutgoingData.object() = value;
    }
    else {
        this->reset();
        new (d_timestampOutgoingData.buffer()) bool(value);
        d_type = ntsa::SocketOptionType::e_TX_TIMESTAMPING;
    }

    return d_timestampOutgoingData.object();
}

bool& SocketOption::makeZeroCopy()
{
    if (d_type == ntsa::SocketOptionType::e_ZERO_COPY) {
        d_zeroCopy.object() = false;
    }
    else {
        this->reset();
        new (d_zeroCopy.buffer()) bool();
        d_type = ntsa::SocketOptionType::e_ZERO_COPY;
    }

    return d_zeroCopy.object();
}

bool& SocketOption::makeZeroCopy(bool value)
{
    if (d_type == ntsa::SocketOptionType::e_ZERO_COPY) {
        d_zeroCopy.object() = value;
    }
    else {
        this->reset();
        new (d_zeroCopy.buffer()) bool(value);
        d_type = ntsa::SocketOptionType::e_ZERO_COPY;
    }

    return d_zeroCopy.object();
}

bool SocketOption::equals(const SocketOption& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::SocketOptionType::e_REUSE_ADDRESS:
        return d_reuseAddress.object() == other.d_reuseAddress.object();
    case ntsa::SocketOptionType::e_KEEP_ALIVE:
        return d_keepAlive.object() == other.d_keepAlive.object();
    case ntsa::SocketOptionType::e_CORK:
        return d_cork.object() == other.d_cork.object();
    case ntsa::SocketOptionType::e_DELAY_TRANSMISSION:
        return d_delayTransmission.object() == 
               other.d_delayTransmission.object();
    case ntsa::SocketOptionType::e_DELAY_ACKNOWLEDGEMENT:
        return d_delayAcknowledgement.object() == 
               other.d_delayAcknowledgement.object();
    case ntsa::SocketOptionType::e_SEND_BUFFER_SIZE:
        return d_sendBufferSize.object() == 
               other.d_sendBufferSize.object();
    case ntsa::SocketOptionType::e_SEND_BUFFER_LOW_WATERMARK:
        return d_sendBufferLowWatermark.object() == 
               other.d_sendBufferLowWatermark.object();
    case ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE:
        return d_receiveBufferSize.object() == 
               other.d_receiveBufferSize.object();
    case ntsa::SocketOptionType::e_RECEIVE_BUFFER_LOW_WATERMARK:
        return d_receiveBufferLowWatermark.object() == 
               other.d_receiveBufferLowWatermark.object();
    case ntsa::SocketOptionType::e_DEBUG:
        return d_debug.object() == other.d_debug.object();
    case ntsa::SocketOptionType::e_LINGER:
        return d_linger.object().equals(other.d_linger.object());
    case ntsa::SocketOptionType::e_BROADCAST:
        return d_broadcast.object() == other.d_broadcast.object();
    case ntsa::SocketOptionType::e_BYPASS_ROUTING:
        return d_bypassRouting.object() == other.d_bypassRouting.object();
    case ntsa::SocketOptionType::e_INLINE_OUT_OF_BAND_DATA:
        return d_inlineOutOfBandData.object() == 
               other.d_inlineOutOfBandData.object();
    case ntsa::SocketOptionType::e_RX_TIMESTAMPING:
        return d_timestampIncomingData.object() == 
               other.d_timestampIncomingData.object();
    case ntsa::SocketOptionType::e_TX_TIMESTAMPING:
        return d_timestampOutgoingData.object() == 
               other.d_timestampOutgoingData.object();
    case ntsa::SocketOptionType::e_ZERO_COPY:
        return d_zeroCopy.object() == 
               other.d_zeroCopy.object();
    default:
        return true;
    }
}

bool SocketOption::less(const SocketOption& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::SocketOptionType::e_REUSE_ADDRESS:
        return d_reuseAddress.object() < other.d_reuseAddress.object();
    case ntsa::SocketOptionType::e_KEEP_ALIVE:
        return d_keepAlive.object() < other.d_keepAlive.object();
    case ntsa::SocketOptionType::e_CORK:
        return d_cork.object() < other.d_cork.object();
    case ntsa::SocketOptionType::e_DELAY_TRANSMISSION:
        return d_delayTransmission.object() < 
               other.d_delayTransmission.object();
    case ntsa::SocketOptionType::e_DELAY_ACKNOWLEDGEMENT:
        return d_delayAcknowledgement.object() < 
               other.d_delayAcknowledgement.object();
    case ntsa::SocketOptionType::e_SEND_BUFFER_SIZE:
        return d_sendBufferSize.object() < 
               other.d_sendBufferSize.object();
    case ntsa::SocketOptionType::e_SEND_BUFFER_LOW_WATERMARK:
        return d_sendBufferLowWatermark.object() < 
               other.d_sendBufferLowWatermark.object();
    case ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE:
        return d_receiveBufferSize.object() < 
               other.d_receiveBufferSize.object();
    case ntsa::SocketOptionType::e_RECEIVE_BUFFER_LOW_WATERMARK:
        return d_receiveBufferLowWatermark.object() < 
               other.d_receiveBufferLowWatermark.object();
    case ntsa::SocketOptionType::e_DEBUG:
        return d_debug.object() < other.d_debug.object();
    case ntsa::SocketOptionType::e_LINGER:
        return d_linger.object().less(other.d_linger.object());
    case ntsa::SocketOptionType::e_BROADCAST:
        return d_broadcast.object() < other.d_broadcast.object();
    case ntsa::SocketOptionType::e_BYPASS_ROUTING:
        return d_bypassRouting.object() < other.d_bypassRouting.object();
    case ntsa::SocketOptionType::e_INLINE_OUT_OF_BAND_DATA:
        return d_inlineOutOfBandData.object() < 
               other.d_inlineOutOfBandData.object();
    case ntsa::SocketOptionType::e_RX_TIMESTAMPING:
        return d_timestampIncomingData.object() < 
               other.d_timestampIncomingData.object();
    case ntsa::SocketOptionType::e_TX_TIMESTAMPING:
        return d_timestampOutgoingData.object() < 
               other.d_timestampOutgoingData.object();
    case ntsa::SocketOptionType::e_ZERO_COPY:
        return d_zeroCopy.object() < other.d_zeroCopy.object();
    default:
        return true;
    }
}

bsl::ostream& SocketOption::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    switch (d_type) {
    case ntsa::SocketOptionType::e_REUSE_ADDRESS:
        stream << d_reuseAddress.object();
        break;
    case ntsa::SocketOptionType::e_KEEP_ALIVE:
        stream << d_keepAlive.object();
        break;
    case ntsa::SocketOptionType::e_CORK:
        stream << d_cork.object();
        break;
    case ntsa::SocketOptionType::e_DELAY_TRANSMISSION:
        stream << d_delayTransmission.object();
        break;
    case ntsa::SocketOptionType::e_DELAY_ACKNOWLEDGEMENT:
        stream << d_delayAcknowledgement.object();
        break;
    case ntsa::SocketOptionType::e_SEND_BUFFER_SIZE:
        stream << d_sendBufferSize.object();
        break;
    case ntsa::SocketOptionType::e_SEND_BUFFER_LOW_WATERMARK:
        stream << d_sendBufferLowWatermark.object();
        break;
    case ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE:
        stream << d_receiveBufferSize.object();
        break;
    case ntsa::SocketOptionType::e_RECEIVE_BUFFER_LOW_WATERMARK:
        stream << d_receiveBufferLowWatermark.object();
        break;
    case ntsa::SocketOptionType::e_DEBUG:
        stream << d_debug.object();
        break;
    case ntsa::SocketOptionType::e_LINGER:
        d_linger.object().print(stream, level, spacesPerLevel);
        break;
    case ntsa::SocketOptionType::e_BROADCAST:
        stream << d_broadcast.object();
        break;
    case ntsa::SocketOptionType::e_BYPASS_ROUTING:
        stream << d_bypassRouting.object();
        break;
    case ntsa::SocketOptionType::e_INLINE_OUT_OF_BAND_DATA:
        stream << d_inlineOutOfBandData.object();
        break;
    case ntsa::SocketOptionType::e_RX_TIMESTAMPING:
        stream << d_timestampIncomingData.object();
        break;
    case ntsa::SocketOptionType::e_TX_TIMESTAMPING:
        stream << d_timestampOutgoingData.object();
        break;
    case ntsa::SocketOptionType::e_ZERO_COPY:
        stream << d_zeroCopy.object();
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::SocketOptionType::e_UNDEFINED);
        stream << "UNDEFINED";
        break;
    }

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
