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

#include <ntsa_socketconfig.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_socketconfig_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

void SocketConfig::setOption(const ntsa::SocketOption& option)
{
    if (option.isReuseAddress()) {
        d_reuseAddress = option.reuseAddress();
    }
    else if (option.isKeepAlive()) {
        d_keepAlive = option.keepAlive();
    }
    else if (option.isCork()) {
        d_cork = option.cork();
    }
    else if (option.isDelayTransmission()) {
        d_delayTransmission = option.delayTransmission();
    }
    else if (option.isDelayAcknowledgment()) {
        d_delayAcknowledgement = option.delayAcknowledgement();
    }
    else if (option.isSendBufferSize()) {
        d_sendBufferSize = option.sendBufferSize();
    }
    else if (option.isSendBufferLowWatermark()) {
        d_sendBufferLowWatermark = option.sendBufferLowWatermark();
    }
    else if (option.isReceiveBufferSize()) {
        d_receiveBufferSize = option.receiveBufferSize();
    }
    else if (option.isReceiveBufferLowWatermark()) {
        d_receiveBufferLowWatermark = option.receiveBufferLowWatermark();
    }
    else if (option.isDebug()) {
        d_debug = option.debug();
    }
    else if (option.isLinger()) {
        d_linger = option.linger();
    }
    else if (option.isBroadcast()) {
        d_broadcast = option.broadcast();
    }
    else if (option.isBypassRouting()) {
        d_bypassRouting = option.bypassRouting();
    }
    else if (option.isInlineOutOfBandData()) {
        d_inlineOutOfBandData = option.inlineOutOfBandData();
    }
    else if (option.isTimestampIncomingData()) {
        d_timestampIncomingData = option.timestampIncomingData();
    }
    else if (option.isTimestampOutgoingData()) {
        d_timestampOutgoingData = option.timestampOutgoingData();
    }
    else if (option.isZeroCopy()) {
        d_zeroCopy = option.zeroCopy();
    }
}

void SocketConfig::getOption(ntsa::SocketOption*           option,
                             ntsa::SocketOptionType::Value type)
{
    option->reset();

    if (type == ntsa::SocketOptionType::e_REUSE_ADDRESS) {
        if (!d_reuseAddress.isNull()) {
            option->makeReuseAddress(d_reuseAddress.value());
        }
    }
    else if (type == ntsa::SocketOptionType::e_KEEP_ALIVE) {
        if (!d_keepAlive.isNull()) {
            option->makeKeepAlive(d_keepAlive.value());
        }
    }
    else if (type == ntsa::SocketOptionType::e_CORK) {
        if (!d_cork.isNull()) {
            option->makeCork(d_cork.value());
        }
    }
    else if (type == ntsa::SocketOptionType::e_DELAY_TRANSMISSION) {
        if (!d_delayTransmission.isNull()) {
            option->makeDelayTransmission(d_delayTransmission.value());
        }
    }
    else if (type == ntsa::SocketOptionType::e_DELAY_ACKNOWLEDGEMENT) {
        if (!d_delayAcknowledgement.isNull()) {
            option->makeDelayAcknowledgement(d_delayAcknowledgement.value());
        }
    }
    else if (type == ntsa::SocketOptionType::e_SEND_BUFFER_SIZE) {
        if (!d_sendBufferSize.isNull()) {
            option->makeSendBufferSize(d_sendBufferSize.value());
        }
    }
    else if (type == ntsa::SocketOptionType::e_SEND_BUFFER_LOW_WATERMARK) {
        if (!d_sendBufferLowWatermark.isNull()) {
            option->makeSendBufferLowWatermark(
                d_sendBufferLowWatermark.value());
        }
    }
    else if (type == ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE) {
        if (!d_receiveBufferSize.isNull()) {
            option->makeReceiveBufferSize(d_receiveBufferSize.value());
        }
    }
    else if (type == ntsa::SocketOptionType::e_RECEIVE_BUFFER_LOW_WATERMARK) {
        if (!d_receiveBufferLowWatermark.isNull()) {
            option->makeReceiveBufferLowWatermark(
                d_receiveBufferLowWatermark.value());
        }
    }
    else if (type == ntsa::SocketOptionType::e_DEBUG) {
        if (!d_debug.isNull()) {
            option->makeDebug(d_debug.value());
        }
    }
    else if (type == ntsa::SocketOptionType::e_LINGER) {
        if (!d_linger.isNull()) {
            option->makeLinger(d_linger.value());
        }
    }
    else if (type == ntsa::SocketOptionType::e_BROADCAST) {
        if (!d_broadcast.isNull()) {
            option->makeBroadcast(d_broadcast.value());
        }
    }
    else if (type == ntsa::SocketOptionType::e_BYPASS_ROUTING) {
        if (!d_bypassRouting.isNull()) {
            option->makeBypassRouting(d_bypassRouting.value());
        }
    }
    else if (type == ntsa::SocketOptionType::e_INLINE_OUT_OF_BAND_DATA) {
        if (!d_inlineOutOfBandData.isNull()) {
            option->makeInlineOutOfBandData(d_inlineOutOfBandData.value());
        }
    }
    else if (type == ntsa::SocketOptionType::e_RX_TIMESTAMPING) {
        if (!d_timestampIncomingData.isNull()) {
            option->makeTimestampIncomingData(d_timestampIncomingData.value());
        }
    }
    else if (type == ntsa::SocketOptionType::e_TX_TIMESTAMPING) {
        if (!d_timestampOutgoingData.isNull()) {
            option->makeTimestampOutgoingData(d_timestampOutgoingData.value());
        }
    }
    else if (type == ntsa::SocketOptionType::e_ZERO_COPY) {
        if (!d_zeroCopy.isNull()) {
            option->makeZeroCopy(d_zeroCopy.value());
        }
    }
}

bool SocketConfig::equals(const SocketConfig& other) const
{
    return d_reuseAddress == other.d_reuseAddress &&
           d_keepAlive == other.d_keepAlive && d_cork == other.d_cork &&
           d_delayTransmission == other.d_delayTransmission &&
           d_delayAcknowledgement == other.d_delayAcknowledgement &&
           d_sendBufferSize == other.d_sendBufferSize &&
           d_sendBufferLowWatermark == other.d_sendBufferLowWatermark &&
           d_receiveBufferSize == other.d_receiveBufferSize &&
           d_receiveBufferLowWatermark == other.d_receiveBufferLowWatermark &&
           d_debug == other.d_debug && d_linger == other.d_linger &&
           d_broadcast == other.d_broadcast &&
           d_bypassRouting == other.d_bypassRouting &&
           d_inlineOutOfBandData == other.d_inlineOutOfBandData &&
           d_timestampIncomingData == other.d_timestampIncomingData &&
           d_timestampOutgoingData == other.d_timestampOutgoingData &&
           d_zeroCopy == other.d_zeroCopy;
}

bool SocketConfig::less(const SocketConfig& other) const
{
    if (d_reuseAddress < other.d_reuseAddress) {
        return true;
    }

    if (other.d_reuseAddress < d_reuseAddress) {
        return false;
    }

    if (d_keepAlive < other.d_keepAlive) {
        return true;
    }

    if (other.d_keepAlive < d_keepAlive) {
        return false;
    }

    if (d_cork < other.d_cork) {
        return true;
    }

    if (other.d_cork < d_cork) {
        return false;
    }

    if (d_delayTransmission < other.d_delayTransmission) {
        return true;
    }

    if (other.d_delayTransmission < d_delayTransmission) {
        return false;
    }

    if (d_delayAcknowledgement < other.d_delayAcknowledgement) {
        return true;
    }

    if (other.d_delayAcknowledgement < d_delayAcknowledgement) {
        return false;
    }

    if (d_sendBufferSize < other.d_sendBufferSize) {
        return true;
    }

    if (other.d_sendBufferSize < d_sendBufferSize) {
        return false;
    }

    if (d_sendBufferLowWatermark < other.d_sendBufferLowWatermark) {
        return true;
    }

    if (other.d_sendBufferLowWatermark < d_sendBufferLowWatermark) {
        return false;
    }

    if (d_receiveBufferSize < other.d_receiveBufferSize) {
        return true;
    }

    if (other.d_receiveBufferSize < d_receiveBufferSize) {
        return false;
    }

    if (d_receiveBufferLowWatermark < other.d_receiveBufferLowWatermark) {
        return true;
    }

    if (other.d_receiveBufferLowWatermark < d_receiveBufferLowWatermark) {
        return false;
    }

    if (d_debug < other.d_debug) {
        return true;
    }

    if (other.d_debug < d_debug) {
        return false;
    }

    if (d_linger < other.d_linger) {
        return true;
    }

    if (other.d_linger < d_linger) {
        return false;
    }

    if (d_broadcast < other.d_broadcast) {
        return true;
    }

    if (other.d_broadcast < d_broadcast) {
        return false;
    }

    if (d_bypassRouting < other.d_bypassRouting) {
        return true;
    }

    if (other.d_bypassRouting < d_bypassRouting) {
        return false;
    }

    if (d_inlineOutOfBandData < other.d_inlineOutOfBandData) {
        return true;
    }

    if (other.d_inlineOutOfBandData < d_inlineOutOfBandData) {
        return false;
    }

    if (d_timestampIncomingData < other.d_timestampIncomingData) {
        return true;
    }

    if (other.d_timestampIncomingData < d_timestampIncomingData) {
        return false;
    }

    if (d_timestampOutgoingData < other.d_timestampOutgoingData) {
        return true;
    }

    if (other.d_timestampOutgoingData < d_timestampOutgoingData) {
        return false;
    }

    return d_zeroCopy < other.d_zeroCopy;
}

bsl::ostream& SocketConfig::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_reuseAddress.isNull()) {
        printer.printAttribute("reuseAddress", d_reuseAddress.value());
    }

    if (!d_keepAlive.isNull()) {
        printer.printAttribute("keepAlive", d_keepAlive.value());
    }

    if (!d_cork.isNull()) {
        printer.printAttribute("cork", d_cork.value());
    }

    if (!d_delayTransmission.isNull()) {
        printer.printAttribute("delayTransmission", 
                               d_delayTransmission.value());
    }

    if (!d_delayAcknowledgement.isNull()) {
        printer.printAttribute("delayAcknowledgement", 
                               d_delayAcknowledgement.value());
    }

    if (!d_sendBufferSize.isNull()) {
        printer.printAttribute("sendBufferSize", d_sendBufferSize.value());
    }

    if (!d_sendBufferLowWatermark.isNull()) {
        printer.printAttribute("sendBufferLowWatermark",
                               d_sendBufferLowWatermark.value());
    }

    if (!d_receiveBufferSize.isNull()) {
        printer.printAttribute("receiveBufferSize", 
                               d_receiveBufferSize.value());
    }

    if (!d_receiveBufferLowWatermark.isNull()) {
        printer.printAttribute("receiveBufferLowWatermark",
                               d_receiveBufferLowWatermark.value());
    }

    if (!d_debug.isNull()) {
        printer.printAttribute("debug", d_debug.value());
    }

    if (!d_linger.isNull()) {
        printer.printAttribute("linger", d_linger.value());
    }

    if (!d_broadcast.isNull()) {
        printer.printAttribute("broadcast", d_broadcast.value());
    }

    if (!d_bypassRouting.isNull()) {
        printer.printAttribute("bypassRouting", d_bypassRouting.value());
    }

    if (!d_inlineOutOfBandData.isNull()) {
        printer.printAttribute("inlineOutOfBandData", 
                               d_inlineOutOfBandData.value());
    }

    if (!d_timestampIncomingData.isNull()) {
        printer.printAttribute("timestampIncomingData", 
                               d_timestampIncomingData.value());
    }

    if (!d_timestampOutgoingData.isNull()) {
        printer.printAttribute("timestampOutgoingData", 
                               d_timestampOutgoingData.value());
    }

    if (!d_zeroCopy.isNull()) {
        printer.printAttribute("zeroCopy", d_zeroCopy.value());
    }

    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
