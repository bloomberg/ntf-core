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

#ifndef INCLUDED_NTSU_SOCKETOPTIONUTIL
#define INCLUDED_NTSU_SOCKETOPTIONUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_handle.h>
#include <ntsa_socketoption.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsls_timeinterval.h>

namespace BloombergLP {
namespace ntsu {

/// @internal @brief
/// Provide utilities for getting and setting socket options.
///
/// @details
/// Provide a portable interface to get and set operating system
/// socket options by wrapping the relevant operating system interfaces, hiding
/// their various differences and homogenizing their various inconsistences.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsu
struct SocketOptionUtil {
    /// Set the specified 'option' for the specified 'socket'. Return the
    /// error.
    static ntsa::Error setOption(ntsa::Handle              socket,
                                 const ntsa::SocketOption& option);

    /// Set the option for the 'socket' that controls its blocking mode
    /// according to the specified 'blocking' flag. Return the error.
    static ntsa::Error setBlocking(ntsa::Handle socket, bool blocking);

    /// Set the option for the specified 'socket' that controls automatic
    /// periodic transmission of TCP keep-alive packets according to the
    /// specified 'keepAlive' flag. Return the error.
    static ntsa::Error setKeepAlive(ntsa::Handle socket, bool keepAlive);

    /// Set the option for the specified 'socket' that controls its TCP "no
    /// delay" mode (i.e. packet coalescing via Nagle's algorithm) according
    /// to the specified 'noDelay' flag. Return the error.
    static ntsa::Error setNoDelay(ntsa::Handle socket, bool noDelay);

    /// Set the option for the specified 'socket' that enables debug mode
    /// according to the specified 'debugFlag'.
    static ntsa::Error setDebug(ntsa::Handle socket, bool debugFlag);

    /// Set the option for the specified 'socket' that controls whether this
    /// socket can bind to addresses already in use by the operating system
    /// according to the specified 'reuseAddress' flag. Return the error.
    static ntsa::Error setReuseAddress(ntsa::Handle socket, bool reuseAddress);

    /// Set the option for the specified 'oscket' that controls how the
    /// operating system will linger its underlying resources after it has
    /// been closed to the specified 'linger' flag for the specified
    /// 'duration'. Return the error.
    static ntsa::Error setLinger(ntsa::Handle              socket,
                                 bool                      linger,
                                 const bsls::TimeInterval& duration);

    /// Set the option for the specified 'socket' that controls the
    /// maximum size of the send buffer to the specified 'size'. Return the
    /// error.
    static ntsa::Error setSendBufferSize(ntsa::Handle socket,
                                         bsl::size_t  size);

    /// Set the option for the specified 'socket' that controls the
    /// minimum amount of data left in the send buffer before the operating
    /// system indicates the socket is writable to the specified 'size'.
    /// Return the error.
    static ntsa::Error setSendBufferLowWatermark(ntsa::Handle socket,
                                                 bsl::size_t  size);

    /// Set the option for the specified 'socket' that controls the maximum
    /// size of the receive buffer to the specified 'size'. Return the
    /// error.
    static ntsa::Error setReceiveBufferSize(ntsa::Handle socket,
                                            bsl::size_t  size);

    /// Set the option for the specified 'socket' that controls the minimum
    /// amount of data available in the receive buffer before the operating
    /// system indicates the socket is readable to the specified 'size'.
    /// Return the error.
    static ntsa::Error setReceiveBufferLowWatermark(ntsa::Handle socket,
                                                    bsl::size_t  size);

    /// Set the option for the specified 'socket' that control whether
    /// datagrams may be broadcast from the socket according to the
    /// specified 'broadcastFlag'. Return the error.
    static ntsa::Error setBroadcast(ntsa::Handle socket, bool broadcastFlag);

    /// Set the option for the specified 'socket' that bypasses normal
    /// message routing according to the specified 'debugFlag'.
    static ntsa::Error setBypassRouting(ntsa::Handle socket, bool bypassFlag);

    /// Set the option for the specified 'socket' that places out-of-band
    /// data into the normal input queue according to the specified
    /// 'inlineFlag'.
    static ntsa::Error setInlineOutOfBandData(ntsa::Handle socket,
                                              bool         inlineFlag);

    /// Set the option for the specified 'socket' that enables or disables
    /// application of both software and hardware timestamps for incoming data
    /// according to the specified 'timestampFlag' flag. Note that if an error
    /// is returned then timestamps will never be generated. If there is no
    /// error returned then in some cases OS can also refuse to generate
    /// timestamps.
    static ntsa::Error setTimestampIncomingData(ntsa::Handle socket,
                                                bool         timestampFlag);

    /// Set the option for the specified 'socket' that enables or disables
    /// application of timestamps for outgoing data according to the specified
    /// 'timestampFlag' flag. Note that if an error is returned then timestamps
    /// will never be generated. If there is no error returned then in some
    /// cases OS can also refuse to generate timestamps.
    static ntsa::Error setTimestampOutgoingData(ntsa::Handle socket,
                                                bool         timestampFlag);

    /// Set the option for the specified 'socket' that allows Linux
    /// MSG_ZEROCOPY mechanism usage according to the specified 'zeroCopy'
    /// flag. Return the error.
    static ntsa::Error setZeroCopy(ntsa::Handle socket, bool zeroCopy);

    /// Set the option for the specified 'socket' that sets
    /// TCP_CONGESTION_CONTROL algorithm to the specified `algorithm`.
    /// Return the error.
    static ntsa::Error setTcpCongestionControl(
        ntsa::Handle                      socket,
        const ntsa::TcpCongestionControl& algorithm);

    /// Load into the specified 'option' the socket option of the specified
    /// 'type' for the specified 'socket'. Return the error.
    static ntsa::Error getOption(ntsa::SocketOption*           option,
                                 ntsa::SocketOptionType::Value type,
                                 ntsa::Handle                  socket);

    /// Load into the specified 'blocking' flag the blocking mode of the
    /// specified 'socket'. Return the error. Note that this function always
    /// returns an error on Windows, as determination of the blocking mode is
    /// not supported on that platform.
    static ntsa::Error getBlocking(ntsa::Handle handle, bool* blocking);

    /// Load into the specified 'keepAlive' flag the option for the
    /// specified 'socket' that controls automatic periodic transmission
    /// of TCP keep-alive packets. Return the error.
    static ntsa::Error getKeepAlive(bool* keepAlive, ntsa::Handle socket);

    /// Load into the spefied 'noDelay' flag the option for the specified
    /// 'socket' that controls its TCP "no delay" mode (i.e. packet
    /// coalescing via Nagle's algorithm.) Return the error.
    static ntsa::Error getNoDelay(bool* noDelay, ntsa::Handle socket);

    /// Load into the spefied 'debugFlag' flag the option for the specified
    /// 'socket' that indicates debug mode is enabled. Return the error.
    static ntsa::Error getDebug(bool* debugFlag, ntsa::Handle socket);

    /// Load into the specified 'noDelay' flag the option for the specified
    /// 'socket' that controls whether this socket can bind to addresses
    /// already in use by the operating system. Return the error.
    static ntsa::Error getReuseAddress(bool*        reuseAddress,
                                       ntsa::Handle socket);

    /// Load into the specified 'linger' flag and the specified 'duration'
    /// the option for the specified 'socket' that controls how the
    /// operating system will linger its underlying resources after it
    /// has been closed. Return the error.
    static ntsa::Error getLinger(bool*               linger,
                                 bsls::TimeInterval* duration,
                                 ntsa::Handle        socket);

    /// Load into the specified 'size' the option for the specified
    /// 'socket' that controls the maximum size of the send buffer. Return
    /// the error.
    static ntsa::Error getSendBufferSize(bsl::size_t* size,
                                         ntsa::Handle socket);

    /// Load into the specified 'size' the option for the specified 'socket'
    /// that controls the minimum amount of data left in the send buffer
    /// before the operating system indicates the socket is writable. Return
    /// the error.
    static ntsa::Error getSendBufferLowWatermark(bsl::size_t* size,
                                                 ntsa::Handle socket);

    /// Load into the specified 'size' the option for the specified
    /// 'socket' that controls the maximum size of the receive buffer.
    /// Return the error.
    static ntsa::Error getReceiveBufferSize(bsl::size_t* size,
                                            ntsa::Handle socket);

    /// Load into the specified 'size' the option for the specified 'socket'
    /// that controls the minimum amount of data available in the receive
    /// buffer before the operating system indicates the socket is readable.
    /// Return the error.
    static ntsa::Error getReceiveBufferLowWatermark(bsl::size_t* size,
                                                    ntsa::Handle socket);

    /// Load into the specified 'broadcastFlag' the option for the specified
    /// 'socket' that control whether datagrams may be broadcast from the
    /// socket . Return the error.
    static ntsa::Error getBroadcast(bool* broadcastFlag, ntsa::Handle socket);

    /// Load into the specified 'bypassFlag' the option for the specified
    /// 'socket' that indicates normal routing should by bypassed. Return
    /// the error.
    static ntsa::Error getBypassRouting(bool* bypassFlag, ntsa::Handle socket);

    /// Load into the specified 'inlineFlag' the option for the specified
    /// 'socket' that indicates out-of-band data should be put into the
    /// normal input queue. Return the error.
    static ntsa::Error getInlineOutOfBandData(bool*        inlineFlag,
                                              ntsa::Handle socket);

    /// Load into the specified 'timestampFlag' the option for the specified
    /// 'socket' that indicates application of any type of timestamps for
    /// incoming data.  Return the error.
    static ntsa::Error getTimestampIncomingData(bool*        timestampFlag,
                                                ntsa::Handle socket);

    /// Load into the specified 'timestampFlag' the option for the specified
    /// 'socket' that indicates application of any type of timestamps for
    /// outgoing data.  Return the error.
    static ntsa::Error getTimestampOutgoingData(bool*        timestampFlag,
                                                ntsa::Handle socket);

    /// Load into the specified 'zeroCopyFlag' the option for the specified
    /// 'socket' that indicates if Linux MSG_ZEROCOPY mechanism can be used.
    ///  Return the error.
    static ntsa::Error getZeroCopy(bool* zeroCopyFlag, ntsa::Handle socket);

    /// Load into the specified 'algorithm' the option for the specified
    /// 'socket' that indicates which TCP_CONGESTION_CONTROL algorithm is used.
    /// Return the error.
    static ntsa::Error getTcpCongestionControl(
        ntsa::TcpCongestionControl* algorithm,
        ntsa::Handle                socket);

    /// Load into the specified 'size' the option for the specified 'socket'
    /// that indicates the amount of space left in the send buffer. Return
    /// the error.
    static ntsa::Error getSendBufferRemaining(bsl::size_t* size,
                                              ntsa::Handle socket);

    /// Load into the specified 'size' the option for the specified 'socket'
    /// that indicates the amount of data filled in the receive buffer.
    /// Return the error.
    static ntsa::Error getReceiveBufferAvailable(bsl::size_t* size,
                                                 ntsa::Handle socket);

    /// Load into the specified 'error' the last known error encountered
    /// when connecting the socket. Return the error (retrieving the error).
    static ntsa::Error getLastError(ntsa::Error* error, ntsa::Handle socket);

    /// Set the flag that indicates multicast datagrams should be looped
    /// back to the local host to the specified 'value'. Return the error.
    static ntsa::Error setMulticastLoopback(ntsa::Handle socket, bool enabled);

    /// Set the network interface on which multicast datagrams will be
    /// sent for the specified 'socket' to the network interface assigned
    /// the specified 'interface' address. Return the error.
    static ntsa::Error setMulticastInterface(ntsa::Handle           socket,
                                             const ntsa::IpAddress& interface);

    /// Set multicast on the specified 'socket' limited to the specified
    /// 'numHops'. Return the error.
    static ntsa::Error setMulticastTimeToLive(ntsa::Handle socket,
                                              bsl::size_t  maxHops);

    /// Issue an IGMP message to add the specified 'socket' to the
    /// specified multicast 'group' on the adapter identified by the
    /// specified 'interface'. Return the error.
    static ntsa::Error joinMulticastGroup(ntsa::Handle           socket,
                                          const ntsa::IpAddress& interface,
                                          const ntsa::IpAddress& group);

    /// Issue an IGMP message to remove the specified 'socket' from the
    /// specified multicast 'group' on the adapter identified by the
    /// specified 'interface'. Return the error.
    static ntsa::Error leaveMulticastGroup(ntsa::Handle           socket,
                                           const ntsa::IpAddress& interface,
                                           const ntsa::IpAddress& group);

    /// Issue an IGMP message to add the specified 'socket' to the specified
    /// source-specific multicast 'group' on the adapter identified by the
    /// specified 'interface' and allow receiving datagrams only from the
    /// specified 'source'. Return the error. Note that this function may be
    /// called multiple times to allow receiving multicast datagrams only from
    /// a particular set of source addresses.
    static ntsa::Error joinMulticastGroupSource(
        ntsa::Handle           socket,
        const ntsa::IpAddress& interface,
        const ntsa::IpAddress& group,
        const ntsa::IpAddress& source);

    /// Issue an IGMP message to remove the specified 'socket' from the
    /// specified source-specific multicast 'group' on the adapter identified
    /// by the specified 'interface', disallowing datagrams sent by the
    /// specified 'source'. Return the error. If the 'socket' has subscribed to
    /// multiple sources within the same group, data from the remaining sources
    /// will still be delivered.  To stop receiving data from all sources at
    /// once, use 'leaveMulticastGroup'.
    static ntsa::Error leaveMulticastGroupSource(
        ntsa::Handle           socket,
        const ntsa::IpAddress& interface,
        const ntsa::IpAddress& group,
        const ntsa::IpAddress& source);    

    /// Load into the specified 'result' the flag that indicates if the
    /// socket type is a stream socket. Return the error.
    static ntsa::Error isStream(bool* result, ntsa::Handle socket);

    /// Load into the specified 'result' the flag that indicates if the
    /// socket type is a datagram socket. Return the error.
    static ntsa::Error isDatagram(bool* result, ntsa::Handle socket);

    /// Load into the specified 'result' the flag that indicates if the
    /// socket type is a local (aka Unix) socket. Return the error.
    static ntsa::Error isLocal(bool* result, ntsa::Handle socket);

    /// Return true if the specified 'socket' supports notifications, otherwise
    /// return false.
    static bool supportsNotifications(ntsa::Handle socket);

    /// Return true if the specified 'socket' supports timestamping, otherwise
    /// return false.
    static bool supportsTimestamping(ntsa::Handle socket);

    /// Return true if the specified 'socket' supports zero-copy, otherwise
    /// return false.
    static bool supportsZeroCopy(ntsa::Handle socket);
};

}  // end namespace ntsu
}  // end namespace BloombergLP
#endif
