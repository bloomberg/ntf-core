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

#include <ntsu_socketoptionutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsu_socketoptionutil_cpp, "$Id$ $CSID$")

#include <ntscfg_platform.h>
#include <ntsu_adapterutil.h>
#include <ntsu_timestamputil.h>
#include <ntsu_zerocopyutil.h>

#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsls_platform.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#if defined(BSLS_PLATFORM_OS_LINUX)
#include <linux/version.h>
#endif
#if defined(BSLS_PLATFORM_OS_SOLARIS)
#include <sys/filio.h>
#endif
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif
#ifdef WINVER
#undef WINVER
#endif
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define NTDDI_VERSION 0x06000100
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// clang-format off
#include <windows.h>
#include <winerror.h>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
// clang-format on
#ifdef interface
#undef interface
#endif
#pragma comment(lib, "ws2_32")
#endif

namespace BloombergLP {
namespace ntsu {

namespace {

/// Provide utilities for converting between 'ntsa::Endpoint'
/// the platform 'struct sockaddr_storage'.
struct SocketStorageUtil {
    /// Load into the specified 'socketAddress' and 'socketAddressSize' the
    /// conversion of the specified 'endpoint'. Return the error.
    static ntsa::Error convert(sockaddr_storage*      socketAddress,
                               socklen_t*             socketAddressSize,
                               const ntsa::IpAddress& ipAddress);
};

ntsa::Error SocketStorageUtil::convert(
    sockaddr_storage*      socketAddress,
    socklen_t*             socketAddressSize,
    const ntsa::IpAddress& ipAddress)
{
    bsl::memset(socketAddress, 0, sizeof(sockaddr_storage));
    *socketAddressSize = 0;

    if (ipAddress.isV4()) {
        sockaddr_in* socketAddressIpv4 =
            reinterpret_cast<sockaddr_in*>(socketAddress);

        *socketAddressSize = sizeof(sockaddr_in);

        socketAddressIpv4->sin_family = AF_INET;
        socketAddressIpv4->sin_port   = 0;

        ipAddress.v4().copyTo(
            &socketAddressIpv4->sin_addr,
            sizeof socketAddressIpv4->sin_addr);
    }
    else if (ipAddress.isV6()) {
        sockaddr_in6* socketAddressIpv6 =
            reinterpret_cast<sockaddr_in6*>(socketAddress);

        *socketAddressSize = sizeof(sockaddr_in6);

        socketAddressIpv6->sin6_family = AF_INET6;
        socketAddressIpv6->sin6_port   = 0;

        ipAddress.v6().copyTo(
            &socketAddressIpv6->sin6_addr,
            sizeof socketAddressIpv6->sin6_addr);

        socketAddressIpv6->sin6_scope_id =
            ipAddress.v6().scopeId();
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

} // close unnamed namespace

ntsa::Error SocketOptionUtil::setOption(ntsa::Handle              socket,
                                        const ntsa::SocketOption& option)
{
    if (option.isReuseAddress()) {
        return SocketOptionUtil::setReuseAddress(socket,
                                                 option.reuseAddress());
    }
    else if (option.isKeepAlive()) {
        return SocketOptionUtil::setKeepAlive(socket, option.keepAlive());
    }
    else if (option.isCork()) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }
    else if (option.isDelayTransmission()) {
        return SocketOptionUtil::setNoDelay(socket,
                                            !option.delayTransmission());
    }
    else if (option.isDelayAcknowledgment()) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }
    else if (option.isSendBufferSize()) {
        return SocketOptionUtil::setSendBufferSize(socket,
                                                   option.sendBufferSize());
    }
    else if (option.isSendBufferLowWatermark()) {
        return SocketOptionUtil::setSendBufferLowWatermark(
            socket,
            option.sendBufferLowWatermark());
    }
    else if (option.isReceiveBufferSize()) {
        return SocketOptionUtil::setReceiveBufferSize(
            socket,
            option.receiveBufferSize());
    }
    else if (option.isReceiveBufferLowWatermark()) {
        return SocketOptionUtil::setReceiveBufferLowWatermark(
            socket,
            option.receiveBufferLowWatermark());
    }
    else if (option.isDebug()) {
        return SocketOptionUtil::setDebug(socket, option.debug());
    }
    else if (option.isLinger()) {
        return SocketOptionUtil::setLinger(socket,
                                           option.linger().enabled(),
                                           option.linger().duration());
    }
    else if (option.isBroadcast()) {
        return SocketOptionUtil::setBroadcast(socket, option.broadcast());
    }
    else if (option.isBypassRouting()) {
        return SocketOptionUtil::setBypassRouting(socket,
                                                  option.bypassRouting());
    }
    else if (option.isInlineOutOfBandData()) {
        return SocketOptionUtil::setInlineOutOfBandData(
            socket,
            option.inlineOutOfBandData());
    }
    else if (option.isTimestampIncomingData()) {
        return SocketOptionUtil::setTimestampIncomingData(
            socket,
            option.timestampIncomingData());
    }
    else if (option.isTimestampOutgoingData()) {
        return SocketOptionUtil::setTimestampOutgoingData(
            socket,
            option.timestampOutgoingData());
    }
    else if (option.isZeroCopy()) {
        return SocketOptionUtil::setZeroCopy(socket, option.zeroCopy());
    }
    else if (option.isTcpCongestionControl()) {
        return SocketOptionUtil::setTcpCongestionControl(
            socket,
            option.tcpCongestionControl());
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error SocketOptionUtil::getOption(ntsa::SocketOption*           option,
                                        ntsa::SocketOptionType::Value type,
                                        ntsa::Handle                  socket)
{
    ntsa::Error error;

    option->reset();

    if (type == ntsa::SocketOptionType::e_REUSE_ADDRESS) {
        bool value;
        error = SocketOptionUtil::getReuseAddress(&value, socket);
        if (error) {
            return error;
        }
        option->makeReuseAddress(value);
        return ntsa::Error();
    }
    else if (type == ntsa::SocketOptionType::e_KEEP_ALIVE) {
        bool value;
        error = SocketOptionUtil::getKeepAlive(&value, socket);
        if (error) {
            return error;
        }
        option->makeKeepAlive(value);
        return ntsa::Error();
    }
    else if (type == ntsa::SocketOptionType::e_CORK) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }
    else if (type == ntsa::SocketOptionType::e_DELAY_TRANSMISSION) {
        bool value;
        error = SocketOptionUtil::getNoDelay(&value, socket);
        if (error) {
            return error;
        }
        option->makeDelayTransmission(!value);
        return ntsa::Error();
    }
    else if (type == ntsa::SocketOptionType::e_DELAY_ACKNOWLEDGEMENT) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }
    else if (type == ntsa::SocketOptionType::e_SEND_BUFFER_SIZE) {
        bsl::size_t value;
        error = SocketOptionUtil::getSendBufferSize(&value, socket);
        if (error) {
            return error;
        }
        option->makeSendBufferSize(value);
        return ntsa::Error();
    }
    else if (type == ntsa::SocketOptionType::e_SEND_BUFFER_LOW_WATERMARK) {
        bsl::size_t value;
        error = SocketOptionUtil::getSendBufferLowWatermark(&value, socket);
        if (error) {
            return error;
        }
        option->makeSendBufferLowWatermark(value);
        return ntsa::Error();
    }
    else if (type == ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE) {
        bsl::size_t value;
        error = SocketOptionUtil::getReceiveBufferSize(&value, socket);
        if (error) {
            return error;
        }
        option->makeReceiveBufferSize(value);
        return ntsa::Error();
    }
    else if (type == ntsa::SocketOptionType::e_RECEIVE_BUFFER_LOW_WATERMARK) {
        bsl::size_t value;
        error = SocketOptionUtil::getReceiveBufferLowWatermark(&value, socket);
        if (error) {
            return error;
        }
        option->makeReceiveBufferLowWatermark(value);
        return ntsa::Error();
    }
    else if (type == ntsa::SocketOptionType::e_DEBUG) {
        bool value;
        error = SocketOptionUtil::getDebug(&value, socket);
        if (error) {
            return error;
        }
        option->makeDebug(value);
        return ntsa::Error();
    }
    else if (type == ntsa::SocketOptionType::e_LINGER) {
        bool               enabled;
        bsls::TimeInterval duration;
        error = SocketOptionUtil::getLinger(&enabled, &duration, socket);
        if (error) {
            return error;
        }
        ntsa::Linger linger;
        linger.setEnabled(enabled);
        linger.setDuration(duration);
        option->makeLinger(linger);
        return ntsa::Error();
    }
    else if (type == ntsa::SocketOptionType::e_BROADCAST) {
        bool value;
        error = SocketOptionUtil::getBroadcast(&value, socket);
        if (error) {
            return error;
        }
        option->makeBroadcast(value);
        return ntsa::Error();
    }
    else if (type == ntsa::SocketOptionType::e_BYPASS_ROUTING) {
        bool value;
        error = SocketOptionUtil::getBypassRouting(&value, socket);
        if (error) {
            return error;
        }
        option->makeBypassRouting(value);
        return ntsa::Error();
    }
    else if (type == ntsa::SocketOptionType::e_INLINE_OUT_OF_BAND_DATA) {
        bool value;
        error = SocketOptionUtil::getInlineOutOfBandData(&value, socket);
        if (error) {
            return error;
        }
        option->makeInlineOutOfBandData(value);
        return ntsa::Error();
    }
    else if (type == ntsa::SocketOptionType::e_RX_TIMESTAMPING) {
        bool value = false;
        error = SocketOptionUtil::getTimestampIncomingData(&value, socket);
        if (error) {
            return error;
        }
        option->makeTimestampIncomingData(value);
        return ntsa::Error();
    }
    else if (type == ntsa::SocketOptionType::e_TX_TIMESTAMPING) {
        bool value = false;
        error = SocketOptionUtil::getTimestampOutgoingData(&value, socket);
        if (error) {
            return error;
        }
        option->makeTimestampOutgoingData(value);
        return ntsa::Error();
    }
    else if (type == ntsa::SocketOptionType::e_ZERO_COPY) {
        bool value = false;
        error      = SocketOptionUtil::getZeroCopy(&value, socket);
        if (error) {
            return error;
        }
        option->makeZeroCopy(value);
        return ntsa::Error();
    }
    else if (type == ntsa::SocketOptionType::e_TCP_CONGESTION_CONTROL) {
        ntsa::TcpCongestionControl value;
        error = SocketOptionUtil::getTcpCongestionControl(&value, socket);
        if (error) {
            return error;
        }
        option->makeTcpCongestionControl(value);
        return ntsa::Error();
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

#if defined(BSLS_PLATFORM_OS_UNIX)

ntsa::Error SocketOptionUtil::setBlocking(ntsa::Handle socket, bool blocking)
{
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags < 0) {
        return ntsa::Error(errno);
    }

    if (blocking) {
        flags &= ~O_NONBLOCK;
    }
    else {
        flags |= O_NONBLOCK;
    }

    if (-1 == fcntl(socket, F_SETFL, flags)) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setKeepAlive(ntsa::Handle socket, bool keepAlive)
{
    int optionValue = static_cast<int>(keepAlive);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_KEEPALIVE,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setNoDelay(ntsa::Handle socket, bool noDelay)
{
    int optionValue = static_cast<int>(noDelay);

    int rc = setsockopt(socket,
                        IPPROTO_TCP,
                        TCP_NODELAY,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setDebug(ntsa::Handle socket, bool debugFlag)
{
    int optionValue = static_cast<int>(debugFlag);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_DEBUG,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setReuseAddress(ntsa::Handle socket,
                                              bool         reuseAddress)
{
    {
        int optionValue = static_cast<int>(reuseAddress);

        int rc = setsockopt(socket,
                            SOL_SOCKET,
                            SO_REUSEADDR,
                            reinterpret_cast<char*>(&optionValue),
                            sizeof(optionValue));

        if (rc != 0) {
            return ntsa::Error(errno);
        }
    }

#if defined(BSLS_PLATFORM_OS_AIX) || defined(BSLS_PLATFORM_OS_DARWIN) ||      \
    defined(BSLS_PLATFORM_OS_FREEBSD)

    {
        int optionValue = static_cast<int>(reuseAddress);

        int rc = setsockopt(socket,
                            SOL_SOCKET,
                            SO_REUSEPORT,
                            reinterpret_cast<char*>(&optionValue),
                            sizeof(optionValue));

        if (rc != 0) {
            return ntsa::Error(errno);
        }
    }

#endif

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setTimestampIncomingData(ntsa::Handle socket,
                                                       bool timestampFlag)
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    ntsa::Error error;
    int         rc;

    if (!SocketOptionUtil::supportsTimestamping(socket)) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    int       optionValue  = 0;
    socklen_t optionLength = static_cast<socklen_t>(sizeof(optionValue));

    rc = getsockopt(socket,
                    SOL_SOCKET,
                    ntsu::TimestampUtil::e_SO_TIMESTAMPING,
                    &optionValue,
                    &optionLength);
    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (optionLength != static_cast<socklen_t>(sizeof(optionValue))) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (timestampFlag) {
        optionValue |= ntsu::TimestampUtil::e_SOF_TIMESTAMPING_RX_GENERATION;
        optionValue |= ntsu::TimestampUtil::e_SOF_TIMESTAMPING_REPORTING;
        optionValue |= ntsu::TimestampUtil::e_SOF_TIMESTAMPING_OPTIONS;
    }
    else {
        optionValue &= ~ntsu::TimestampUtil::e_SOF_TIMESTAMPING_RX_GENERATION;
        if ((optionValue &
             ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_GENERATION) == 0)
        {
            optionValue &= ~ntsu::TimestampUtil::e_SOF_TIMESTAMPING_REPORTING;
            optionValue &= ~ntsu::TimestampUtil::e_SOF_TIMESTAMPING_OPTIONS;
        }
    }

#if NTSU_SOCKETOPTIONUTIL_TIMESTAMPING_SAFE

    optionValue = ntsu::TimestampUtil::removeUnsupported(optionValue);

#endif

    rc = setsockopt(socket,
                    SOL_SOCKET,
                    ntsu::TimestampUtil::e_SO_TIMESTAMPING,
                    &optionValue,
                    sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();

#else

    NTSCFG_WARNING_UNUSED(socket);
    NTSCFG_WARNING_UNUSED(timestampFlag);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);

#endif
}

ntsa::Error SocketOptionUtil::setTimestampOutgoingData(ntsa::Handle socket,
                                                       bool timestampFlag)
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    ntsa::Error error;
    int         rc;

    if (!SocketOptionUtil::supportsTimestamping(socket)) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    int       optionValue  = 0;
    socklen_t optionLength = static_cast<socklen_t>(sizeof(optionValue));

    rc = getsockopt(socket,
                    SOL_SOCKET,
                    ntsu::TimestampUtil::e_SO_TIMESTAMPING,
                    &optionValue,
                    &optionLength);
    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (optionLength != static_cast<socklen_t>(sizeof(optionValue))) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (timestampFlag) {
        optionValue |= ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_GENERATION;
        optionValue |= ntsu::TimestampUtil::e_SOF_TIMESTAMPING_REPORTING;
        optionValue |= ntsu::TimestampUtil::e_SOF_TIMESTAMPING_OPTIONS;
    }
    else {
        optionValue &= ~ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_GENERATION;
        if ((optionValue &
             ntsu::TimestampUtil::e_SOF_TIMESTAMPING_RX_GENERATION) == 0)
        {
            optionValue &= ~ntsu::TimestampUtil::e_SOF_TIMESTAMPING_REPORTING;
            optionValue &= ~ntsu::TimestampUtil::e_SOF_TIMESTAMPING_OPTIONS;
        }
    }

#if NTSU_SOCKETOPTIONUTIL_TIMESTAMPING_SAFE

    optionValue = ntsu::TimestampUtil::removeUnsupported(optionValue);

#endif

    rc = setsockopt(socket,
                    SOL_SOCKET,
                    ntsu::TimestampUtil::e_SO_TIMESTAMPING,
                    &optionValue,
                    sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();

#else

    NTSCFG_WARNING_UNUSED(socket);
    NTSCFG_WARNING_UNUSED(timestampFlag);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);

#endif
}

ntsa::Error SocketOptionUtil::setLinger(ntsa::Handle              socket,
                                        bool                      linger,
                                        const bsls::TimeInterval& duration)
{
    struct linger optionValue = {};
    optionValue.l_onoff       = static_cast<int>(linger);
    optionValue.l_linger      = static_cast<int>(duration.totalSeconds());

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_LINGER,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setSendBufferSize(ntsa::Handle socket,
                                                bsl::size_t  size)
{
    int optionValue = static_cast<int>(size);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_SNDBUF,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setSendBufferLowWatermark(ntsa::Handle socket,
                                                        bsl::size_t  size)
{
    int optionValue = static_cast<int>(size);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_SNDLOWAT,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setReceiveBufferSize(ntsa::Handle socket,
                                                   bsl::size_t  size)
{
    int optionValue = static_cast<int>(size);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_RCVBUF,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setReceiveBufferLowWatermark(ntsa::Handle socket,
                                                           bsl::size_t  size)
{
    int optionValue = static_cast<int>(size);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_RCVLOWAT,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setBroadcast(ntsa::Handle socket,
                                           bool         broadcastFlag)
{
    int optionValue = static_cast<int>(broadcastFlag);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_BROADCAST,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setBypassRouting(ntsa::Handle socket,
                                               bool         bypassFlag)
{
    int optionValue = static_cast<int>(bypassFlag);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_DONTROUTE,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setInlineOutOfBandData(ntsa::Handle socket,
                                                     bool         inlineFlag)
{
    int optionValue = static_cast<int>(inlineFlag);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_OOBINLINE,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setZeroCopy(ntsa::Handle socket, bool zeroCopy)
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    ntsa::Error error;
    int         rc;

    if (!SocketOptionUtil::supportsZeroCopy(socket)) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    int optionValue = static_cast<int>(zeroCopy);

    rc = setsockopt(socket,
                    SOL_SOCKET,
                    ntsu::ZeroCopyUtil::e_SO_ZEROCOPY,
                    &optionValue,
                    sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();

#else

    NTSCFG_WARNING_UNUSED(socket);
    NTSCFG_WARNING_UNUSED(zeroCopy);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);

#endif
}

ntsa::Error SocketOptionUtil::setTcpCongestionControl(
    ntsa::Handle                      socket,
    const ntsa::TcpCongestionControl& algorithm)
{
#if defined(BSLS_PLATFORM_OS_LINUX)
    const bsl::string& name     = algorithm.algorithm();
    const char*        optValue = name.c_str();
    const socklen_t    optLen   = name.length();

    const int rc =
        setsockopt(socket, IPPROTO_TCP, TCP_CONGESTION, optValue, optLen);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();
#else
    NTSCFG_WARNING_UNUSED(socket);
    NTSCFG_WARNING_UNUSED(algorithm);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);

#endif
}

ntsa::Error SocketOptionUtil::getBlocking(ntsa::Handle socket, bool* blocking)
{
    *blocking = false;

    const int flags = fcntl(socket, F_GETFL, 0);
    if (flags < 0) {
        return ntsa::Error(errno);
    }

    *blocking = ((flags & O_NONBLOCK) == 0);

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getKeepAlive(bool*        keepAlive,
                                           ntsa::Handle socket)
{
    *keepAlive = false;

    int       optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_KEEPALIVE,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    *keepAlive = static_cast<bool>(optionValue);

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getNoDelay(bool* noDelay, ntsa::Handle socket)
{
    *noDelay = false;

    int       optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        IPPROTO_TCP,
                        TCP_NODELAY,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    *noDelay = static_cast<bool>(optionValue);

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getDebug(bool* debugFlag, ntsa::Handle socket)
{
    *debugFlag = false;

    int       optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_DEBUG,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    *debugFlag = static_cast<bool>(optionValue);

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getReuseAddress(bool*        reuseAddress,
                                              ntsa::Handle socket)
{
    *reuseAddress = false;

    int       optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_REUSEADDR,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    *reuseAddress = static_cast<bool>(optionValue);

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getLinger(bool*               linger,
                                        bsls::TimeInterval* duration,
                                        ntsa::Handle        socket)
{
    *linger   = false;
    *duration = bsls::TimeInterval();

    struct linger optionValue = {0, 0};
    socklen_t     optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_LINGER,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    *linger = static_cast<bool>(optionValue.l_onoff);
    duration->setTotalSeconds(optionValue.l_linger);

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getSendBufferSize(bsl::size_t* size,
                                                ntsa::Handle socket)
{
    *size = 0;

    int       optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_SNDBUF,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (optionValue > 0) {
#if defined(BSLS_PLATFORM_OS_LINUX)
        // Linux internally doubles the requested send buffer size, and returns
        // the doubled value. See 'man 7 socket'.
        optionValue /= 2;
#endif
        *size = static_cast<bsl::size_t>(optionValue);
    }
    else {
        *size = 0;
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getSendBufferLowWatermark(bsl::size_t* size,
                                                        ntsa::Handle socket)
{
    *size = 0;

    int       optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_SNDLOWAT,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (optionValue > 0) {
        *size = static_cast<bsl::size_t>(optionValue);
    }
    else {
        *size = 0;
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getReceiveBufferSize(bsl::size_t* size,
                                                   ntsa::Handle socket)
{
    *size = 0;

    int       optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_RCVBUF,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (optionValue > 0) {
#if defined(BSLS_PLATFORM_OS_LINUX)
        // Linux internally doubles the requested receive buffer size, and
        // returns the doubled value. See 'man 7 socket'.
        optionValue /= 2;
#endif
        *size = static_cast<bsl::size_t>(optionValue);
    }
    else {
        *size = 0;
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getReceiveBufferLowWatermark(bsl::size_t* size,
                                                           ntsa::Handle socket)
{
    *size = 0;

    int       optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_RCVLOWAT,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (optionValue > 0) {
        *size = static_cast<bsl::size_t>(optionValue);
    }
    else {
        *size = 0;
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getBroadcast(bool*        broadcastFlag,
                                           ntsa::Handle socket)
{
    *broadcastFlag = false;

    int       optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_BROADCAST,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (optionValue > 0) {
        *broadcastFlag = static_cast<bool>(optionValue);
    }
    else {
        *broadcastFlag = 0;
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getBypassRouting(bool*        bypassFlag,
                                               ntsa::Handle socket)
{
    *bypassFlag = false;

    int       optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_DONTROUTE,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (optionValue > 0) {
        *bypassFlag = static_cast<bool>(optionValue);
    }
    else {
        *bypassFlag = 0;
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getInlineOutOfBandData(bool*        inlineFlag,
                                                     ntsa::Handle socket)
{
    *inlineFlag = false;

    int       optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_OOBINLINE,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (optionValue > 0) {
        *inlineFlag = static_cast<bool>(optionValue);
    }
    else {
        *inlineFlag = 0;
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getTimestampIncomingData(bool* timestampFlag,
                                                       ntsa::Handle socket)
{
    *timestampFlag = false;

#if defined(BSLS_PLATFORM_OS_LINUX)

    ntsa::Error error;
    int         rc;

    if (!SocketOptionUtil::supportsTimestamping(socket)) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    int       optionValue  = 0;
    socklen_t optionLength = static_cast<socklen_t>(sizeof(optionValue));

    rc = getsockopt(socket,
                    SOL_SOCKET,
                    ntsu::TimestampUtil::e_SO_TIMESTAMPING,
                    &optionValue,
                    &optionLength);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (optionLength != static_cast<socklen_t>(sizeof(optionValue))) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if ((optionValue &
         ntsu::TimestampUtil::e_SOF_TIMESTAMPING_RX_GENERATION) != 0)
    {
        *timestampFlag = true;
    }

    return ntsa::Error();

#else

    NTSCFG_WARNING_UNUSED(socket);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);

#endif
}

ntsa::Error SocketOptionUtil::getTimestampOutgoingData(bool* timestampFlag,
                                                       ntsa::Handle socket)
{
    *timestampFlag = false;

#if defined(BSLS_PLATFORM_OS_LINUX)

    ntsa::Error error;
    int         rc;

    if (!SocketOptionUtil::supportsTimestamping(socket)) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    int       optionValue  = 0;
    socklen_t optionLength = static_cast<socklen_t>(sizeof(optionValue));

    rc = getsockopt(socket,
                    SOL_SOCKET,
                    ntsu::TimestampUtil::e_SO_TIMESTAMPING,
                    &optionValue,
                    &optionLength);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (optionLength != static_cast<socklen_t>(sizeof(optionValue))) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if ((optionValue &
         ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_GENERATION) != 0)
    {
        *timestampFlag = true;
    }

    return ntsa::Error();

#else

    NTSCFG_WARNING_UNUSED(socket);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);

#endif
}

ntsa::Error SocketOptionUtil::getZeroCopy(bool*        zeroCopyFlag,
                                          ntsa::Handle socket)
{
    *zeroCopyFlag = false;

#if defined(BSLS_PLATFORM_OS_LINUX)

    ntsa::Error error;
    int         rc;

    if (!SocketOptionUtil::supportsZeroCopy(socket)) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    int       optionValue  = 0;
    socklen_t optionLength = static_cast<socklen_t>(sizeof(optionValue));

    rc = getsockopt(socket,
                    SOL_SOCKET,
                    ntsu::ZeroCopyUtil::e_SO_ZEROCOPY,
                    &optionValue,
                    &optionLength);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (optionLength != static_cast<socklen_t>(sizeof(optionValue))) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (optionValue != 0) {
        *zeroCopyFlag = true;
    }
    else {
        *zeroCopyFlag = false;
    }

    return ntsa::Error();

#else

    NTSCFG_WARNING_UNUSED(zeroCopyFlag);
    NTSCFG_WARNING_UNUSED(socket);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);

#endif
}

ntsa::Error SocketOptionUtil::getTcpCongestionControl(
    ntsa::TcpCongestionControl* algorithm,
    ntsa::Handle                socket)
{
#if defined(BSLS_PLATFORM_OS_LINUX)
    enum { e_BUFFER_SIZE = 64 };
    char optionValue[e_BUFFER_SIZE];
    bsl::fill(optionValue, optionValue + e_BUFFER_SIZE, '\0');

    socklen_t optionLength = static_cast<socklen_t>(sizeof(optionValue) - 1);

    const int rc = getsockopt(socket,
                              IPPROTO_TCP,
                              TCP_CONGESTION,
                              optionValue,
                              &optionLength);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    algorithm->reset();

    ntsa::Error error = algorithm->setAlgorithmName(optionValue);
    return error;

#else

    NTSCFG_WARNING_UNUSED(algorithm);
    NTSCFG_WARNING_UNUSED(socket);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);

#endif
}

ntsa::Error SocketOptionUtil::getSendBufferRemaining(bsl::size_t* size,
                                                     ntsa::Handle socket)
{
    *size = 0;

#if defined(BSLS_PLATFORM_OS_FREEBSD)

    int sendBufferCapacityRemaining;

    int rc = ioctl(socket, FIONSPACE, &sendBufferCapacityRemaining);
    if (rc != 0) {
        ntsa::Error(errno);
    }

    *size = sendBufferCapacityRemaining;
    return ntsa::Error();

#elif defined(BSLS_PLATFORM_OS_SOLARIS) || defined(BSLS_PLATFORM_OS_AIX)

    *size = 0;
    return ntsa::Error(ENOTSUP);

#else

    int sendBufferFilled = 0;

#if defined(BSLS_PLATFORM_OS_LINUX)

    {
        int rc = ioctl(socket, TIOCOUTQ, &sendBufferFilled);
        if (rc != 0) {
            return ntsa::Error(errno);
        }
    }

#elif defined(BSLS_PLATFORM_OS_DARWIN)

    {
        int       optionValue = 0;
        socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

        int rc = getsockopt(socket,
                            SOL_SOCKET,
                            SO_NWRITE,
                            &optionValue,
                            &optionSize);

        if (rc != 0) {
            return ntsa::Error(errno);
        }

        if (optionValue > 0) {
            sendBufferFilled = optionValue;
        }
        else {
            sendBufferFilled = 0;
        }
    }

#else
    *size = 0;
    return ntsa::Error(ENOTSUP);

#endif

    int sendBufferSize = 0;
    {
        int       optionValue = 0;
        socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

        int rc = getsockopt(socket,
                            SOL_SOCKET,
                            SO_SNDBUF,
                            reinterpret_cast<char*>(&optionValue),
                            &optionSize);

        if (rc != 0) {
            return ntsa::Error(errno);
        }

        if (optionValue > 0) {
#if defined(BSLS_PLATFORM_OS_LINUX)
            // Linux internally doubles the requested send buffer size, and
            // returns the doubled value. See 'man 7 socket'.
            optionValue /= 2;
#endif

            sendBufferSize = optionValue;
        }
        else {
            sendBufferSize = 0;
        }
    }

    if (sendBufferSize < sendBufferFilled) {
        *size = 0;
        return ntsa::Error(EINVAL);
    }

    *size = sendBufferSize - sendBufferFilled;
    return ntsa::Error();

#endif
}

ntsa::Error SocketOptionUtil::getReceiveBufferAvailable(bsl::size_t* size,
                                                        ntsa::Handle socket)
{
    *size = 0;

    int value = 0;

    int rc = ioctl(socket, FIONREAD, &value);
    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (value > 0) {
        *size = value;
    }
    else {
        *size = 0;
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getLastError(ntsa::Error* error,
                                           ntsa::Handle socket)
{
    *error = ntsa::Error();

    int       optionValue;
    socklen_t optionSize = sizeof(optionValue);

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_ERROR,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    *error = ntsa::Error(optionValue);

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setMulticastLoopback(ntsa::Handle socket,
                                                   bool         enabled)
{
    sockaddr_storage socketAddress = {};
    socklen_t        socketAddressSize =
        static_cast<socklen_t>(sizeof(sockaddr_storage));

    int rc = ::getsockname(socket,
                           reinterpret_cast<sockaddr*>(&socketAddress),
                           &socketAddressSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (socketAddress.ss_family == AF_INET) {
        u_char optionValue = enabled ? 1 : 0;
        int    rc          = ::setsockopt(socket,
                              IPPROTO_IP,
                              IP_MULTICAST_LOOP,
                              reinterpret_cast<char*>(&optionValue),
                              sizeof optionValue);

        if (rc != 0) {
            return ntsa::Error(errno);
        }
    }
    else if (socketAddress.ss_family == AF_INET6) {
        u_char optionValue = enabled ? 1 : 0;
        int    rc          = ::setsockopt(socket,
                              IPPROTO_IPV6,
                              IPV6_MULTICAST_LOOP,
                              reinterpret_cast<char*>(&optionValue),
                              sizeof optionValue);

        if (rc != 0) {
            return ntsa::Error(errno);
        }
    }
    else {
        return ntsa::Error::invalid();
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setMulticastInterface(
    ntsa::Handle           socket,
    const ntsa::IpAddress& interface)
{
    if (interface.isV4()) {
        in_addr optionValue;
        interface.v4().copyTo(&optionValue, sizeof optionValue);

        int rc = ::setsockopt(socket,
                              IPPROTO_IP,
                              IP_MULTICAST_IF,
                              reinterpret_cast<char*>(&optionValue),
                              sizeof optionValue);

        if (rc != 0) {
            return ntsa::Error(errno);
        }
    }
    else if (interface.isV6()) {
        bsl::uint32_t optionValue =
            ntsu::AdapterUtil::discoverInterfaceIndex(interface.v6());

        int rc = ::setsockopt(socket,
                              IPPROTO_IPV6,
                              IPV6_MULTICAST_IF,
                              reinterpret_cast<char*>(&optionValue),
                              sizeof optionValue);

        if (rc != 0) {
            return ntsa::Error(errno);
        }
    }
    else {
        return ntsa::Error::invalid();
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setMulticastTimeToLive(ntsa::Handle socket,
                                                     bsl::size_t  maxHops)
{
    sockaddr_storage socketAddress = {};
    socklen_t        socketAddressSize =
        static_cast<socklen_t>(sizeof(sockaddr_storage));

    int rc = ::getsockname(socket,
                           reinterpret_cast<sockaddr*>(&socketAddress),
                           &socketAddressSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (socketAddress.ss_family == AF_INET) {
        u_char optionValue = static_cast<u_char>(maxHops);

        rc = ::setsockopt(socket,
                          IPPROTO_IP,
                          IP_MULTICAST_TTL,
                          reinterpret_cast<char*>(&optionValue),
                          sizeof optionValue);

        if (rc != 0) {
            return ntsa::Error(errno);
        }
    }
    else if (socketAddress.ss_family == AF_INET6) {
        int optionValue = static_cast<int>(maxHops);

        rc = ::setsockopt(socket,
                          IPPROTO_IPV6,
                          IPV6_MULTICAST_HOPS,
                          reinterpret_cast<char*>(&optionValue),
                          sizeof optionValue);

        if (rc != 0) {
            return ntsa::Error(errno);
        }
    }
    else {
        return ntsa::Error::invalid();
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::joinMulticastGroup(
    ntsa::Handle           socket,
    const ntsa::IpAddress& interface,
    const ntsa::IpAddress& group)
{
    sockaddr_storage socketAddress = {};
    socklen_t        socketAddressSize =
        static_cast<socklen_t>(sizeof(sockaddr_storage));

    int rc = ::getsockname(socket,
                           reinterpret_cast<sockaddr*>(&socketAddress),
                           &socketAddressSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (socketAddress.ss_family == AF_INET) {
        if (interface.isV4() && group.isV4()) {
            ip_mreq mreq = {};

            group.v4().copyTo(&mreq.imr_multiaddr, sizeof mreq.imr_multiaddr);

            interface.v4().copyTo(&mreq.imr_interface.s_addr,
                                  sizeof mreq.imr_interface.s_addr);

            rc = ::setsockopt(socket,
                              IPPROTO_IP,
                              IP_ADD_MEMBERSHIP,
                              reinterpret_cast<char*>(&mreq),
                              sizeof mreq);

            if (rc != 0) {
                return ntsa::Error(errno);
            }
        }
        else {
            return ntsa::Error::invalid();
        }
    }
    else if (socketAddress.ss_family == AF_INET6) {
        if (interface.isV6() && group.isV6()) {
            ipv6_mreq mreq = {};

            group.v6().copyTo(&mreq.ipv6mr_multiaddr,
                              sizeof mreq.ipv6mr_multiaddr);

            mreq.ipv6mr_interface = interface.v6().scopeId();
            if (mreq.ipv6mr_interface == 0) {
                mreq.ipv6mr_interface =
                    ntsu::AdapterUtil::discoverInterfaceIndex(interface.v6());
            }

            rc = ::setsockopt(socket,
                              IPPROTO_IPV6,
                              IPV6_JOIN_GROUP,
                              reinterpret_cast<char*>(&mreq),
                              sizeof mreq);

            if (rc != 0) {
                return ntsa::Error(errno);
            }
        }
        else {
            return ntsa::Error::invalid();
        }
    }
    else {
        return ntsa::Error::invalid();
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::leaveMulticastGroup(
    ntsa::Handle           socket,
    const ntsa::IpAddress& interface,
    const ntsa::IpAddress& group)
{
    sockaddr_storage socketAddress = {};
    socklen_t        socketAddressSize =
        static_cast<socklen_t>(sizeof(sockaddr_storage));

    int rc = ::getsockname(socket,
                           reinterpret_cast<sockaddr*>(&socketAddress),
                           &socketAddressSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (socketAddress.ss_family == AF_INET) {
        if (interface.isV4() && group.isV4()) {
            ip_mreq mreq = {};

            group.v4().copyTo(&mreq.imr_multiaddr, sizeof mreq.imr_multiaddr);

            interface.v4().copyTo(&mreq.imr_interface.s_addr,
                                  sizeof mreq.imr_interface.s_addr);

            rc = ::setsockopt(socket,
                              IPPROTO_IP,
                              IP_DROP_MEMBERSHIP,
                              reinterpret_cast<char*>(&mreq),
                              sizeof mreq);

            if (rc != 0) {
                return ntsa::Error(errno);
            }
        }
        else {
            return ntsa::Error::invalid();
        }
    }
    else if (socketAddress.ss_family == AF_INET6) {
        if (interface.isV6() && group.isV6()) {
            ipv6_mreq mreq = {};

            group.v6().copyTo(&mreq.ipv6mr_multiaddr,
                              sizeof mreq.ipv6mr_multiaddr);

            mreq.ipv6mr_interface = interface.v6().scopeId();
            if (mreq.ipv6mr_interface == 0) {
                mreq.ipv6mr_interface =
                    ntsu::AdapterUtil::discoverInterfaceIndex(interface.v6());
            }

            rc = ::setsockopt(socket,
                              IPPROTO_IPV6,
                              IPV6_LEAVE_GROUP,
                              reinterpret_cast<char*>(&mreq),
                              sizeof mreq);

            if (rc != 0) {
                return ntsa::Error(errno);
            }
        }
        else {
            return ntsa::Error::invalid();
        }
    }
    else {
        return ntsa::Error::invalid();
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::joinMulticastGroupSource(
        ntsa::Handle           socket,
        const ntsa::IpAddress& interface,
        const ntsa::IpAddress& group,
        const ntsa::IpAddress& source)
{
#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_DARWIN)

    ntsa::Error error;
    int         rc;

    sockaddr_storage socketAddress = {};
    socklen_t        socketAddressSize =
        static_cast<socklen_t>(sizeof(sockaddr_storage));

    rc = ::getsockname(socket,
                       reinterpret_cast<sockaddr*>(&socketAddress),
                       &socketAddressSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (socketAddress.ss_family != AF_INET && 
        socketAddress.ss_family != AF_INET6) 
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint32_t interfaceIndex = 
        ntsu::AdapterUtil::discoverInterfaceIndex(interface);

    if (interfaceIndex == 0) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    group_source_req req = {};

    req.gsr_interface = interfaceIndex;

    socklen_t groupAddressSize = 0;
    error = SocketStorageUtil::convert(
        &req.gsr_group, &groupAddressSize, group);
    if (error) {
        return error;
    }

    socklen_t sourceAddressSize = 0;
    error = SocketStorageUtil::convert(
        &req.gsr_source, &sourceAddressSize, source);
    if (error) {
        return error;
    }

    rc = ::setsockopt(socket,
                      IPPROTO_IP,
                      MCAST_JOIN_SOURCE_GROUP,
                      reinterpret_cast<char*>(&req),
                      sizeof req);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();

#else

    sockaddr_storage socketAddress = {};
    socklen_t        socketAddressSize =
        static_cast<socklen_t>(sizeof(sockaddr_storage));

    int rc = ::getsockname(socket,
                           reinterpret_cast<sockaddr*>(&socketAddress),
                           &socketAddressSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (socketAddress.ss_family == AF_INET) {
        if (interface.isV4() && group.isV4() && source.isV4()) {
            ip_mreq_source mreq = {};

            group.v4().copyTo(&mreq.imr_multiaddr, sizeof mreq.imr_multiaddr);

            interface.v4().copyTo(&mreq.imr_interface.s_addr,
                                  sizeof mreq.imr_interface.s_addr);

            source.v4().copyTo(&mreq.imr_sourceaddr.s_addr,
                               sizeof mreq.imr_sourceaddr.s_addr);

            errno = 0;
            rc = ::setsockopt(socket,
                              IPPROTO_IP,
                              IP_ADD_SOURCE_MEMBERSHIP,
                              reinterpret_cast<char*>(&mreq),
                              sizeof mreq);

            if (rc != 0) {
                return ntsa::Error(errno);
            }
        }
        else {
            return ntsa::Error::invalid();
        }
    }
    else if (socketAddress.ss_family == AF_INET6) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }
    else {
        return ntsa::Error::invalid();
    }

    return ntsa::Error();

#endif
}

ntsa::Error SocketOptionUtil::leaveMulticastGroupSource(
        ntsa::Handle           socket,
        const ntsa::IpAddress& interface,
        const ntsa::IpAddress& group,
        const ntsa::IpAddress& source)
{
#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_DARWIN)

    ntsa::Error error;
    int         rc;

    sockaddr_storage socketAddress = {};
    socklen_t        socketAddressSize =
        static_cast<socklen_t>(sizeof(sockaddr_storage));

    rc = ::getsockname(socket,
                       reinterpret_cast<sockaddr*>(&socketAddress),
                       &socketAddressSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (socketAddress.ss_family != AF_INET && 
        socketAddress.ss_family != AF_INET6) 
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint32_t interfaceIndex = 
        ntsu::AdapterUtil::discoverInterfaceIndex(interface);

    if (interfaceIndex == 0) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    group_source_req req = {};

    req.gsr_interface = interfaceIndex;

    socklen_t groupAddressSize = 0;
    error = SocketStorageUtil::convert(
        &req.gsr_group, &groupAddressSize, group);
    if (error) {
        return error;
    }

    socklen_t sourceAddressSize = 0;
    error = SocketStorageUtil::convert(
        &req.gsr_source, &sourceAddressSize, source);
    if (error) {
        return error;
    }

    rc = ::setsockopt(socket,
                      IPPROTO_IP,
                      MCAST_LEAVE_SOURCE_GROUP,
                      reinterpret_cast<char*>(&req),
                      sizeof req);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();

#else

    sockaddr_storage socketAddress = {};
    socklen_t        socketAddressSize =
        static_cast<socklen_t>(sizeof(sockaddr_storage));

    int rc = ::getsockname(socket,
                           reinterpret_cast<sockaddr*>(&socketAddress),
                           &socketAddressSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (socketAddress.ss_family == AF_INET) {
        if (interface.isV4() && group.isV4() && source.isV4()) {
            ip_mreq_source mreq = {};

            group.v4().copyTo(&mreq.imr_multiaddr, sizeof mreq.imr_multiaddr);

            interface.v4().copyTo(&mreq.imr_interface.s_addr,
                                  sizeof mreq.imr_interface.s_addr);

            source.v4().copyTo(&mreq.imr_sourceaddr.s_addr,
                               sizeof mreq.imr_sourceaddr.s_addr);

            rc = ::setsockopt(socket,
                              IPPROTO_IP,
                              IP_DROP_SOURCE_MEMBERSHIP,
                              reinterpret_cast<char*>(&mreq),
                              sizeof mreq);

            if (rc != 0) {
                return ntsa::Error(errno);
            }
        }
        else {
            return ntsa::Error::invalid();
        }
    }
    else if (socketAddress.ss_family == AF_INET6) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    else {
        return ntsa::Error::invalid();
    }

    return ntsa::Error();

#endif
}

ntsa::Error SocketOptionUtil::isStream(bool* result, ntsa::Handle socket)
{
    *result = false;

    int       optionValue;
    socklen_t optionSize = sizeof(optionValue);

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_TYPE,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    *result = optionValue == SOCK_STREAM;
    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::isDatagram(bool* result, ntsa::Handle socket)
{
    *result = false;

    int       optionValue;
    socklen_t optionSize = sizeof(optionValue);

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_TYPE,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    *result = optionValue == SOCK_DGRAM;
    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::isLocal(bool* result, ntsa::Handle socket)
{
#if defined(BSLS_PLATFORM_OS_AIX)

    *result = false;

    int rc;

    sockaddr_storage socketAddress;
    bsl::memset(&socketAddress, 0, sizeof socketAddress);

    socklen_t socketAddressSize = static_cast<socklen_t>(sizeof socketAddress);

    rc = getsockname(socket,
                     reinterpret_cast<sockaddr*>(&socketAddress),
                     &socketAddressSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (socketAddress.ss_family == AF_UNIX) {
        *result = true;
        return ntsa::Error();
    }

    if (socketAddress.ss_family != AF_UNSPEC) {
        *result = false;
        return ntsa::Error();
    }

    struct peercred_struct credentials;
    bsl::memset(&credentials, 0, sizeof credentials);

    socklen_t optionSize = static_cast<socklen_t>(sizeof credentials);

    rc = getsockopt(socket,
                    SOL_SOCKET,
                    SO_PEERID,
                    reinterpret_cast<char*>(&credentials),
                    &optionSize);

    if (rc != 0) {
        int lastError = errno;
        if (lastError == ENOPROTOOPT) {
            *result = false;
            return ntsa::Error();
        }
        else if (lastError == ENOTCONN) {
            *result = true;
            return ntsa::Error();
        }
        else {
            *result = false;
            return ntsa::Error(lastError);
        }
    }

    *result = (optionSize > 0);
    return ntsa::Error();

#elif defined(BSLS_PLATFORM_OS_DARWIN)

    *result = false;

    int rc;

    sockaddr_storage socketAddress;
    bsl::memset(&socketAddress, 0, sizeof socketAddress);

    socklen_t socketAddressSize = static_cast<socklen_t>(sizeof socketAddress);

    rc = getsockname(socket,
                     reinterpret_cast<sockaddr*>(&socketAddress),
                     &socketAddressSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (socketAddress.ss_family == AF_UNIX) {
        *result = true;
        return ntsa::Error();
    }

    if (socketAddress.ss_family == AF_INET ||
        socketAddress.ss_family == AF_INET6)
    {
        *result = false;
        return ntsa::Error();
    }

    return ntsa::Error(ntsa::Error::e_INVALID);

#else

    *result = false;

    int       optionValue;
    socklen_t optionSize = sizeof(optionValue);

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_DOMAIN,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    *result = (optionValue == AF_UNIX);
    return ntsa::Error();

#endif
}

bool SocketOptionUtil::supportsNotifications(ntsa::Handle socket)
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    ntsa::Error error;
    int         rc;

    // Notifications are not supported on local sockets.

    bool isLocalFlag = false;
    error            = ntsu::SocketOptionUtil::isLocal(&isLocalFlag, socket);
    if (error) {
        return false;
    }

    if (isLocalFlag) {
        return false;
    }

    // Notifications for TCP and UDP sockets are only supported on Linux kernel
    // versions >= 4.18.0.

    int major = 0;
    int minor = 0;
    int patch = 0;
    int build = 0;

    rc = ntsscm::Version::systemVersion(&major, &minor, &patch, &build);
    if (rc != 0) {
        return false;
    }

    if (KERNEL_VERSION(major, minor, patch) < KERNEL_VERSION(4, 18, 0)) {
        return false;
    }

    return true;

#else

    NTSCFG_WARNING_UNUSED(socket);
    return false;

#endif
}

bool SocketOptionUtil::supportsTimestamping(ntsa::Handle socket)
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    ntsa::Error error;
    int         rc;

    // Notifications are not supported on local sockets.

    bool isLocalFlag = false;
    error            = ntsu::SocketOptionUtil::isLocal(&isLocalFlag, socket);
    if (error) {
        return false;
    }

    if (isLocalFlag) {
        return false;
    }

    // Timestamping for TCP and UDP sockets are only supported on Linux kernel
    // versions >= 4.18.0.

    int major = 0;
    int minor = 0;
    int patch = 0;
    int build = 0;

    rc = ntsscm::Version::systemVersion(&major, &minor, &patch, &build);
    if (rc != 0) {
        return false;
    }

    if (KERNEL_VERSION(major, minor, patch) < KERNEL_VERSION(4, 18, 0)) {
        return false;
    }

    return true;

#else

    NTSCFG_WARNING_UNUSED(socket);
    return false;

#endif
}

bool SocketOptionUtil::supportsZeroCopy(ntsa::Handle socket)
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    ntsa::Error error;
    int         rc;

    bool isLocalFlag = false;
    error            = ntsu::SocketOptionUtil::isLocal(&isLocalFlag, socket);
    if (error) {
        return false;
    }

    if (isLocalFlag) {
        return false;
    }

    int major = 0;
    int minor = 0;
    int patch = 0;
    int build = 0;

    rc = ntsscm::Version::systemVersion(&major, &minor, &patch, &build);
    if (rc != 0) {
        return false;
    }

    bool isStreamFlag = false;
    error = ntsu::SocketOptionUtil::isStream(&isStreamFlag, socket);
    if (error) {
        return false;
    }

    if (isStreamFlag) {
        if (KERNEL_VERSION(major, minor, patch) < KERNEL_VERSION(4, 18, 0)) {
            return false;
        }
    }
    else {
        if (KERNEL_VERSION(major, minor, patch) < KERNEL_VERSION(5, 14, 0)) {
            return false;
        }
    }

    return true;

#else

    NTSCFG_WARNING_UNUSED(socket);
    return false;

#endif
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

ntsa::Error SocketOptionUtil::setBlocking(ntsa::Handle socket, bool blocking)
{
    u_long argument = blocking ? 0 : 1;
    int    rc       = ioctlsocket(socket, FIONBIO, &argument);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setKeepAlive(ntsa::Handle socket, bool keepAlive)
{
    BOOL optionValue = static_cast<BOOL>(keepAlive);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_KEEPALIVE,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setNoDelay(ntsa::Handle socket, bool noDelay)
{
    BOOL optionValue = static_cast<BOOL>(noDelay);

    int rc = setsockopt(socket,
                        IPPROTO_TCP,
                        TCP_NODELAY,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setDebug(ntsa::Handle socket, bool debugFlag)
{
    BOOL optionValue = static_cast<BOOL>(debugFlag);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_DEBUG,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setReuseAddress(ntsa::Handle socket,
                                              bool         reuseAddress)
{
    BOOL optionValue = static_cast<int>(reuseAddress);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_REUSEADDR,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setTimestampIncomingData(ntsa::Handle socket,
                                                       bool timestampFlag)
{
    NTSCFG_WARNING_UNUSED(socket);
    NTSCFG_WARNING_UNUSED(timestampFlag);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error SocketOptionUtil::setTimestampOutgoingData(ntsa::Handle socket,
                                                       bool timestampFlag)
{
    NTSCFG_WARNING_UNUSED(socket);
    NTSCFG_WARNING_UNUSED(timestampFlag);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error SocketOptionUtil::setZeroCopy(ntsa::Handle socket, bool zeroCopy)
{
    NTSCFG_WARNING_UNUSED(socket);
    NTSCFG_WARNING_UNUSED(zeroCopy);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error SocketOptionUtil::setTcpCongestionControl(
    ntsa::Handle                      socket,
    const ntsa::TcpCongestionControl& algorithm)
{
    NTSCFG_WARNING_UNUSED(socket);
    NTSCFG_WARNING_UNUSED(algorithm);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error SocketOptionUtil::setLinger(ntsa::Handle              socket,
                                        bool                      linger,
                                        const bsls::TimeInterval& duration)
{
    struct linger optionValue = {};
    optionValue.l_onoff       = static_cast<int>(linger);
    optionValue.l_linger =
        NTSCFG_WARNING_NARROW(u_short, duration.totalSeconds());

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_LINGER,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setSendBufferSize(ntsa::Handle socket,
                                                bsl::size_t  size)
{
    int optionValue = static_cast<int>(size);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_SNDBUF,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setSendBufferLowWatermark(ntsa::Handle socket,
                                                        bsl::size_t  size)
{
    int optionValue = static_cast<int>(size);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_SNDLOWAT,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setReceiveBufferSize(ntsa::Handle socket,
                                                   bsl::size_t  size)
{
    int optionValue = static_cast<int>(size);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_RCVBUF,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setReceiveBufferLowWatermark(ntsa::Handle socket,
                                                           bsl::size_t  size)
{
    int optionValue = static_cast<int>(size);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_RCVLOWAT,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setBroadcast(ntsa::Handle socket,
                                           bool         broadcastFlag)
{
    BOOL optionValue = static_cast<int>(broadcastFlag);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_BROADCAST,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setBypassRouting(ntsa::Handle socket,
                                               bool         bypassFlag)
{
    BOOL optionValue = static_cast<BOOL>(bypassFlag);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_DONTROUTE,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setInlineOutOfBandData(ntsa::Handle socket,
                                                     bool         inlineFlag)
{
    BOOL optionValue = static_cast<BOOL>(inlineFlag);

    int rc = setsockopt(socket,
                        SOL_SOCKET,
                        SO_OOBINLINE,
                        reinterpret_cast<char*>(&optionValue),
                        sizeof(optionValue));

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getBlocking(ntsa::Handle socket, bool* blocking)
{
    NTSCFG_WARNING_UNUSED(socket);

    *blocking = false;
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error SocketOptionUtil::getKeepAlive(bool*        keepAlive,
                                           ntsa::Handle socket)
{
    *keepAlive = false;

    BOOL      optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_KEEPALIVE,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    *keepAlive = static_cast<bool>(optionValue);

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getNoDelay(bool* noDelay, ntsa::Handle socket)
{
    *noDelay = false;

    BOOL      optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        IPPROTO_TCP,
                        TCP_NODELAY,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    *noDelay = static_cast<bool>(optionValue);

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getDebug(bool* debugFlag, ntsa::Handle socket)
{
    *debugFlag = false;

    BOOL      optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_DEBUG,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    *debugFlag = static_cast<bool>(optionValue);

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getReuseAddress(bool*        reuseAddress,
                                              ntsa::Handle socket)
{
    *reuseAddress = false;

    BOOL      optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_REUSEADDR,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    *reuseAddress = static_cast<bool>(optionValue);

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getLinger(bool*               linger,
                                        bsls::TimeInterval* duration,
                                        ntsa::Handle        socket)
{
    *linger   = false;
    *duration = bsls::TimeInterval();

    struct linger optionValue = {};
    socklen_t     optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_LINGER,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    *linger = static_cast<bool>(optionValue.l_onoff);
    duration->setTotalSeconds(optionValue.l_linger);

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getSendBufferSize(bsl::size_t* size,
                                                ntsa::Handle socket)
{
    *size = 0;

    int       optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_SNDBUF,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    if (optionValue > 0) {
        *size = static_cast<bsl::size_t>(optionValue);
    }
    else {
        *size = 0;
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getSendBufferLowWatermark(bsl::size_t* size,
                                                        ntsa::Handle socket)
{
    *size = 0;

    int       optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_SNDLOWAT,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    if (optionValue > 0) {
        *size = static_cast<bsl::size_t>(optionValue);
    }
    else {
        *size = 0;
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getReceiveBufferSize(bsl::size_t* size,
                                                   ntsa::Handle socket)
{
    *size = 0;

    int       optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_RCVBUF,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    if (optionValue > 0) {
        *size = static_cast<bsl::size_t>(optionValue);
    }
    else {
        *size = 0;
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getReceiveBufferLowWatermark(bsl::size_t* size,
                                                           ntsa::Handle socket)
{
    *size = 0;

    int       optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_RCVLOWAT,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    if (optionValue > 0) {
        *size = static_cast<bsl::size_t>(optionValue);
    }
    else {
        *size = 0;
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getBroadcast(bool*        broadcastFlag,
                                           ntsa::Handle socket)
{
    *broadcastFlag = false;

    BOOL      optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_BROADCAST,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    if (optionValue > 0) {
        *broadcastFlag = static_cast<bool>(optionValue);
    }
    else {
        *broadcastFlag = 0;
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getBypassRouting(bool*        bypassFlag,
                                               ntsa::Handle socket)
{
    *bypassFlag = false;

    BOOL      optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_DONTROUTE,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    if (optionValue > 0) {
        *bypassFlag = static_cast<bool>(optionValue);
    }
    else {
        *bypassFlag = 0;
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getInlineOutOfBandData(bool*        inlineFlag,
                                                     ntsa::Handle socket)
{
    *inlineFlag = false;

    BOOL      optionValue = 0;
    socklen_t optionSize  = static_cast<socklen_t>(sizeof(optionValue));

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_OOBINLINE,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    if (optionValue > 0) {
        *inlineFlag = static_cast<bool>(optionValue);
    }
    else {
        *inlineFlag = 0;
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getTimestampIncomingData(bool* timestampFlag,
                                                       ntsa::Handle socket)
{
    NTSCFG_WARNING_UNUSED(timestampFlag);
    NTSCFG_WARNING_UNUSED(socket);

    *timestampFlag = false;

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error SocketOptionUtil::getTimestampOutgoingData(bool* timestampFlag,
                                                       ntsa::Handle socket)
{
    NTSCFG_WARNING_UNUSED(timestampFlag);
    NTSCFG_WARNING_UNUSED(socket);

    *timestampFlag = false;

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error SocketOptionUtil::getZeroCopy(bool*        zeroCopyFlag,
                                          ntsa::Handle socket)
{
    NTSCFG_WARNING_UNUSED(zeroCopyFlag);
    NTSCFG_WARNING_UNUSED(socket);

    *zeroCopyFlag = false;

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error SocketOptionUtil::getTcpCongestionControl(
    ntsa::TcpCongestionControl* algorithm,
    ntsa::Handle                socket)
{
    NTSCFG_WARNING_UNUSED(algorithm);
    NTSCFG_WARNING_UNUSED(socket);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error SocketOptionUtil::getSendBufferRemaining(bsl::size_t* size,
                                                     ntsa::Handle socket)
{
    *size = 0;
    return ntsa::Error(WSAEOPNOTSUPP);
}

ntsa::Error SocketOptionUtil::getReceiveBufferAvailable(bsl::size_t* size,
                                                        ntsa::Handle socket)
{
    *size = 0;

    u_long value = 0;
    int    rc    = ioctlsocket(socket, FIONREAD, &value);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    *size = static_cast<bsl::size_t>(value);

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::getLastError(ntsa::Error* error,
                                           ntsa::Handle socket)
{
    *error = ntsa::Error();

    int optionValue = 0;
    int optionSize  = sizeof(optionValue);

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_ERROR,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    *error = ntsa::Error(optionValue);

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setMulticastLoopback(ntsa::Handle socket,
                                                   bool         enabled)
{
    sockaddr_storage socketAddress = {};
    socklen_t        socketAddressSize =
        static_cast<socklen_t>(sizeof(sockaddr_storage));

    int rc = ::getsockname(socket,
                           reinterpret_cast<sockaddr*>(&socketAddress),
                           &socketAddressSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    if (socketAddress.ss_family == AF_INET) {
        u_char optionValue = enabled ? 1 : 0;
        int    rc          = ::setsockopt(socket,
                              IPPROTO_IP,
                              IP_MULTICAST_LOOP,
                              reinterpret_cast<char*>(&optionValue),
                              sizeof optionValue);

        if (rc != 0) {
            return ntsa::Error(WSAGetLastError());
        }
    }
    else if (socketAddress.ss_family == AF_INET6) {
        u_char optionValue = enabled ? 1 : 0;
        int    rc          = ::setsockopt(socket,
                              IPPROTO_IPV6,
                              IPV6_MULTICAST_LOOP,
                              reinterpret_cast<char*>(&optionValue),
                              sizeof optionValue);

        if (rc != 0) {
            return ntsa::Error(WSAGetLastError());
        }
    }
    else {
        return ntsa::Error::invalid();
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setMulticastInterface(
    ntsa::Handle           socket,
    const ntsa::IpAddress& interface)
{
    if (interface.isV4()) {
        in_addr optionValue;
        interface.v4().copyTo(&optionValue, sizeof optionValue);

        int rc = ::setsockopt(socket,
                              IPPROTO_IP,
                              IP_MULTICAST_IF,
                              reinterpret_cast<char*>(&optionValue),
                              sizeof optionValue);

        if (rc != 0) {
            return ntsa::Error(WSAGetLastError());
        }
    }
    else if (interface.isV6()) {
        bsl::uint32_t optionValue =
            ntsu::AdapterUtil::discoverInterfaceIndex(interface.v6());

        int rc = ::setsockopt(socket,
                              IPPROTO_IPV6,
                              IPV6_MULTICAST_IF,
                              reinterpret_cast<char*>(&optionValue),
                              sizeof optionValue);

        if (rc != 0) {
            return ntsa::Error(WSAGetLastError());
        }
    }
    else {
        return ntsa::Error::invalid();
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::setMulticastTimeToLive(ntsa::Handle socket,
                                                     bsl::size_t  maxHops)
{
    sockaddr_storage socketAddress = {};
    socklen_t        socketAddressSize =
        static_cast<socklen_t>(sizeof(sockaddr_storage));

    int rc = ::getsockname(socket,
                           reinterpret_cast<sockaddr*>(&socketAddress),
                           &socketAddressSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    if (socketAddress.ss_family == AF_INET) {
        u_char optionValue = static_cast<u_char>(maxHops);

        rc = ::setsockopt(socket,
                          IPPROTO_IP,
                          IP_MULTICAST_TTL,
                          reinterpret_cast<char*>(&optionValue),
                          sizeof optionValue);

        if (rc != 0) {
            return ntsa::Error(WSAGetLastError());
        }
    }
    else if (socketAddress.ss_family == AF_INET6) {
        int optionValue = static_cast<int>(maxHops);

        rc = ::setsockopt(socket,
                          IPPROTO_IPV6,
                          IPV6_MULTICAST_HOPS,
                          reinterpret_cast<char*>(&optionValue),
                          sizeof optionValue);

        if (rc != 0) {
            return ntsa::Error(WSAGetLastError());
        }
    }
    else {
        return ntsa::Error::invalid();
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::joinMulticastGroup(
    ntsa::Handle           socket,
    const ntsa::IpAddress& interface,
    const ntsa::IpAddress& group)
{
    sockaddr_storage socketAddress = {};
    socklen_t        socketAddressSize =
        static_cast<socklen_t>(sizeof(sockaddr_storage));

    int rc = ::getsockname(socket,
                           reinterpret_cast<sockaddr*>(&socketAddress),
                           &socketAddressSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    if (socketAddress.ss_family == AF_INET) {
        if (interface.isV4() && group.isV4()) {
            ip_mreq mreq = {};

            group.v4().copyTo(&mreq.imr_multiaddr, sizeof mreq.imr_multiaddr);

            interface.v4().copyTo(&mreq.imr_interface.s_addr,
                                  sizeof mreq.imr_interface.s_addr);

            rc = ::setsockopt(socket,
                              IPPROTO_IP,
                              IP_ADD_MEMBERSHIP,
                              reinterpret_cast<char*>(&mreq),
                              sizeof mreq);

            if (rc != 0) {
                return ntsa::Error(WSAGetLastError());
            }
        }
        else {
            return ntsa::Error::invalid();
        }
    }
    else if (socketAddress.ss_family == AF_INET6) {
        if (interface.isV6() && group.isV6()) {
            ipv6_mreq mreq = {};

            group.v6().copyTo(&mreq.ipv6mr_multiaddr,
                              sizeof mreq.ipv6mr_multiaddr);

            mreq.ipv6mr_interface = interface.v6().scopeId();
            if (mreq.ipv6mr_interface == 0) {
                mreq.ipv6mr_interface =
                    ntsu::AdapterUtil::discoverInterfaceIndex(interface.v6());
            }

            rc = ::setsockopt(socket,
                              IPPROTO_IPV6,
                              IPV6_JOIN_GROUP,
                              reinterpret_cast<char*>(&mreq),
                              sizeof mreq);

            if (rc != 0) {
                return ntsa::Error(WSAGetLastError());
            }
        }
        else {
            return ntsa::Error::invalid();
        }
    }
    else {
        return ntsa::Error::invalid();
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::leaveMulticastGroup(
    ntsa::Handle           socket,
    const ntsa::IpAddress& interface,
    const ntsa::IpAddress& group)
{
    sockaddr_storage socketAddress = {};
    socklen_t        socketAddressSize =
        static_cast<socklen_t>(sizeof(sockaddr_storage));

    int rc = ::getsockname(socket,
                           reinterpret_cast<sockaddr*>(&socketAddress),
                           &socketAddressSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    if (socketAddress.ss_family == AF_INET) {
        if (interface.isV4() && group.isV4()) {
            ip_mreq mreq = {};

            group.v4().copyTo(&mreq.imr_multiaddr, sizeof mreq.imr_multiaddr);

            interface.v4().copyTo(&mreq.imr_interface.s_addr,
                                  sizeof mreq.imr_interface.s_addr);

            rc = ::setsockopt(socket,
                              IPPROTO_IP,
                              IP_DROP_MEMBERSHIP,
                              reinterpret_cast<char*>(&mreq),
                              sizeof mreq);

            if (rc != 0) {
                return ntsa::Error(WSAGetLastError());
            }
        }
        else {
            return ntsa::Error::invalid();
        }
    }
    else if (socketAddress.ss_family == AF_INET6) {
        if (interface.isV6() && group.isV6()) {
            ipv6_mreq mreq = {};

            group.v6().copyTo(&mreq.ipv6mr_multiaddr,
                              sizeof mreq.ipv6mr_multiaddr);

            mreq.ipv6mr_interface = interface.v6().scopeId();
            if (mreq.ipv6mr_interface == 0) {
                mreq.ipv6mr_interface =
                    ntsu::AdapterUtil::discoverInterfaceIndex(interface.v6());
            }

            rc = ::setsockopt(socket,
                              IPPROTO_IPV6,
                              IPV6_LEAVE_GROUP,
                              reinterpret_cast<char*>(&mreq),
                              sizeof mreq);

            if (rc != 0) {
                return ntsa::Error(WSAGetLastError());
            }
        }
        else {
            return ntsa::Error::invalid();
        }
    }
    else {
        return ntsa::Error::invalid();
    }

    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::joinMulticastGroupSource(
        ntsa::Handle           socket,
        const ntsa::IpAddress& interface,
        const ntsa::IpAddress& group,
        const ntsa::IpAddress& source)
{
    NTSCFG_WARNING_UNUSED(socket);
    NTSCFG_WARNING_UNUSED(interface);
    NTSCFG_WARNING_UNUSED(group);
    NTSCFG_WARNING_UNUSED(source);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error SocketOptionUtil::leaveMulticastGroupSource(
        ntsa::Handle           socket,
        const ntsa::IpAddress& interface,
        const ntsa::IpAddress& group,
        const ntsa::IpAddress& source)
{
    NTSCFG_WARNING_UNUSED(socket);
    NTSCFG_WARNING_UNUSED(interface);
    NTSCFG_WARNING_UNUSED(group);
    NTSCFG_WARNING_UNUSED(source);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error SocketOptionUtil::isStream(bool* result, ntsa::Handle socket)
{
    *result = false;

    DWORD     optionValue;
    socklen_t optionSize = sizeof(optionValue);

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_TYPE,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    *result = optionValue == SOCK_STREAM;
    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::isDatagram(bool* result, ntsa::Handle socket)
{
    *result = false;

    DWORD     optionValue;
    socklen_t optionSize = sizeof(optionValue);

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_TYPE,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    *result = optionValue == SOCK_DGRAM;
    return ntsa::Error();
}

ntsa::Error SocketOptionUtil::isLocal(bool* result, ntsa::Handle socket)
{
    *result = false;

    int rc;

    sockaddr_storage socketAddress;
    bsl::memset(&socketAddress, 0, sizeof socketAddress);

    socklen_t socketAddressSize = static_cast<socklen_t>(sizeof socketAddress);

    rc = getsockname(socket,
                     reinterpret_cast<sockaddr*>(&socketAddress),
                     &socketAddressSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    if (socketAddress.ss_family == AF_UNIX) {
        *result = true;
        return ntsa::Error();
    }

    if (socketAddress.ss_family != AF_UNSPEC) {
        *result = false;
        return ntsa::Error();
    }

    DWORD     optionValue;
    socklen_t optionSize = sizeof(optionValue);

    rc = getsockopt(socket,
                    SOL_SOCKET,
                    SO_MAXDG,
                    reinterpret_cast<char*>(&optionValue),
                    &optionSize);

    if (rc != 0) {
        DWORD lastError = WSAGetLastError();
        if (lastError == WSAENOPROTOOPT) {
            *result = false;
            return ntsa::Error();
        }
        else {
            return ntsa::Error(lastError);
        }
    }

    *result = true;
    return ntsa::Error();
}

bool SocketOptionUtil::supportsNotifications(ntsa::Handle socket)
{
    NTSCFG_WARNING_UNUSED(socket);
    return false;
}

bool SocketOptionUtil::supportsTimestamping(ntsa::Handle socket)
{
    NTSCFG_WARNING_UNUSED(socket);
    return false;
}

bool SocketOptionUtil::supportsZeroCopy(ntsa::Handle socket)
{
    NTSCFG_WARNING_UNUSED(socket);
    return false;
}

#else
#error Not implemented
#endif

}  // close package namespace
}  // close enterprise namespace
