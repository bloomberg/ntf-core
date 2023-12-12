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

#include <ntsa_adapter.h>
#include <ntscfg_test.h>
#include <ntsu_adapterutil.h>
#include <ntsu_socketoptionutil.h>
#include <ntsu_socketutil.h>
#include <ntsu_timestamputil.h>
#include <bslma_testallocator.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>

#if defined(BSLS_PLATFORM_OS_LINUX)
#include <linux/net_tstamp.h>
#include <linux/socket.h>
#include <linux/version.h>
#include <sys/socket.h>
#endif

using namespace BloombergLP;

// Undefine to test all socket types.
// #define NTSU_SOCKETOPTUTIL_TEST_SOCKET_TYPE
//         ntsa::Transport::e_TCP_IPV4_STREAM

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------

// [ 1]
//-----------------------------------------------------------------------------
// [ 1]
//-----------------------------------------------------------------------------

NTSCFG_TEST_CASE(1)
{
    // Concern: Socket options on TCP/IPv4 sockets, using the raw API.
    // Plan:

    const ntsa::Transport::Value SOCKET_TYPES[] = {
        ntsa::Transport::e_TCP_IPV4_STREAM,
        ntsa::Transport::e_TCP_IPV6_STREAM,
#if !defined(BSLS_PLATFORM_OS_WINDOWS)
        ntsa::Transport::e_LOCAL_STREAM,
#endif
        ntsa::Transport::e_UDP_IPV4_DATAGRAM,
        ntsa::Transport::e_UDP_IPV6_DATAGRAM,
#if !defined(BSLS_PLATFORM_OS_WINDOWS)
        ntsa::Transport::e_LOCAL_DATAGRAM,
#endif
    };

    for (bsl::size_t socketTypeIndex = 0;
         socketTypeIndex < sizeof(SOCKET_TYPES) / sizeof(SOCKET_TYPES[0]);
         ++socketTypeIndex)
    {
        ntsa::Transport::Value transport = SOCKET_TYPES[socketTypeIndex];

#if defined(NTSU_SOCKETOPTUTIL_TEST_SOCKET_TYPE)
        if (transport != NTSU_SOCKETOPTUTIL_TEST_SOCKET_TYPE) {
            continue;
        }
#endif

        if (transport == ntsa::Transport::e_TCP_IPV4_STREAM ||
            transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM)
        {
            if (!ntsu::AdapterUtil::supportsIpv4()) {
                continue;
            }
        }

        if (transport == ntsa::Transport::e_TCP_IPV6_STREAM ||
            transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM)
        {
            if (!ntsu::AdapterUtil::supportsIpv6()) {
                continue;
            }
        }

        NTSCFG_TEST_LOG_WARN << "Testing " << transport << NTSCFG_TEST_LOG_END;

        ntsa::Error error;

        // Create the socket.

        ntsa::Handle socket;
        error = ntsu::SocketUtil::create(&socket, transport);
        NTSCFG_TEST_FALSE(error);

        // Test SOL_SOCKET/SO_KEEPALIVE.

        {
            const bool INPUT[] = {false, true};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                error = ntsu::SocketOptionUtil::setKeepAlive(socket, INPUT[i]);

                NTSCFG_TEST_LOG_INFO << "setKeepAlive: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    bool output;
                    error =
                        ntsu::SocketOptionUtil::getKeepAlive(&output, socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_EQ(output, INPUT[i]);
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_DEBUG.

        {
            const bool INPUT[] = {false, true};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                error = ntsu::SocketOptionUtil::setDebug(socket, INPUT[i]);

                NTSCFG_TEST_LOG_INFO << "setDebug: " << error
                                     << NTSCFG_TEST_LOG_END;

                // On Linux, at least, setting SO_DEBUG to true results
                // in EACCESS.
                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED ||
                                     error == ntsa::Error::e_NOT_AUTHORIZED);
                }
                else {
                    bool output;
                    error = ntsu::SocketOptionUtil::getDebug(&output, socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_EQ(output, INPUT[i]);
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_REUSEADDR

        {
            const bool INPUT[] = {false, true};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                error =
                    ntsu::SocketOptionUtil::setReuseAddress(socket, INPUT[i]);

                NTSCFG_TEST_LOG_INFO << "setReuseAddress: " << error
                                     << NTSCFG_TEST_LOG_END;

                NTSCFG_TEST_FALSE(error);

                bool output;
                error =
                    ntsu::SocketOptionUtil::getReuseAddress(&output, socket);
                NTSCFG_TEST_FALSE(error);

                NTSCFG_TEST_EQ(output, INPUT[i]);
            }
        }

        // Test SOL_SOCKET/SO_LINGER

        {
            const struct Data {
                bool linger;
                int  seconds;
            } INPUT[] = {
                {false, 0},
                { true, 0},
                { true, 1}
            };

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                error = ntsu::SocketOptionUtil::setLinger(
                    socket,
                    INPUT[i].linger,
                    bsls::TimeInterval(INPUT[i].seconds));

                NTSCFG_TEST_LOG_INFO << "setLinger: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    bool               outputLinger;
                    bsls::TimeInterval outputSeconds;
                    error = ntsu::SocketOptionUtil::getLinger(&outputLinger,
                                                              &outputSeconds,
                                                              socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_EQ(outputLinger, INPUT[i].linger);
                        NTSCFG_TEST_EQ(outputSeconds,
                                       bsls::TimeInterval(INPUT[i].seconds));
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_SNDBUF.

        {
            const bsl::size_t INPUT[] = {1024, 1024 * 1024};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                error = ntsu::SocketOptionUtil::setSendBufferSize(socket,
                                                                  INPUT[i]);

                NTSCFG_TEST_LOG_INFO << "setSendBufferSize: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    bsl::size_t output;
                    error = ntsu::SocketOptionUtil::getSendBufferSize(&output,
                                                                      socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
// Linux and Solaris manages the actual size of the
// send buffer itself, using the requested size as
// a hint.
#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_SOLARIS)
                        NTSCFG_TEST_GT(output, 0);
#else
                        NTSCFG_TEST_EQ(output, INPUT[i]);
#endif
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_SNDLOWAT

        {
            const bsl::size_t INPUT[] = {1024, 1024 * 1024};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                error = ntsu::SocketOptionUtil::setSendBufferLowWatermark(
                    socket,
                    INPUT[i]);

                NTSCFG_TEST_LOG_INFO << "setSendBufferLowWatermark: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    bsl::size_t output;
                    error = ntsu::SocketOptionUtil::getSendBufferLowWatermark(
                        &output,
                        socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_EQ(output, INPUT[i]);
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_RCVBUF.

        {
            const bsl::size_t INPUT[] = {1024, 1024 * 1024};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                error = ntsu::SocketOptionUtil::setReceiveBufferSize(socket,
                                                                     INPUT[i]);

                NTSCFG_TEST_LOG_INFO << "setReceiveBufferSize: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    bsl::size_t output;
                    error =
                        ntsu::SocketOptionUtil::getReceiveBufferSize(&output,
                                                                     socket);

                    if (error) {
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
// Linux and Solaris manages the actual size of the
// receive buffer itself, using the requested size as
// a hint.
#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_SOLARIS)
                        NTSCFG_TEST_GT(output, 0);
#else
                        NTSCFG_TEST_EQ(output, INPUT[i]);
#endif
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_RCVLOWAT

        {
            const bsl::size_t INPUT[] = {1024, 1024 * 1024};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                error = ntsu::SocketOptionUtil::setReceiveBufferLowWatermark(
                    socket,
                    INPUT[i]);

                NTSCFG_TEST_LOG_INFO
                    << "setReceiveBufferLowWatermark: " << error
                    << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    bsl::size_t output;
                    error =
                        ntsu::SocketOptionUtil::getReceiveBufferLowWatermark(
                            &output,
                            socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
// Linux and Solaris manages the actual size of the
// receive buffer itself, using the requested size as
// a hint.
#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_SOLARIS)
                        NTSCFG_TEST_GT(output, 0);
#else
                        NTSCFG_TEST_EQ(output, INPUT[i]);
#endif
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_BROADCAST.

        {
            const bool INPUT[] = {false, true};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                error = ntsu::SocketOptionUtil::setBroadcast(socket, INPUT[i]);

                NTSCFG_TEST_LOG_INFO << "setBroadcast: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    bool output;
                    error =
                        ntsu::SocketOptionUtil::getBroadcast(&output, socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_EQ(output, INPUT[i]);
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_DONTROUTE

        {
            const bool INPUT[] = {false, true};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                error =
                    ntsu::SocketOptionUtil::setBypassRouting(socket, INPUT[i]);

                NTSCFG_TEST_LOG_INFO << "setBypassRouting: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    bool output;
                    error = ntsu::SocketOptionUtil::getBypassRouting(&output,
                                                                     socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
// Linux returns false even if set to true (sometimes).
#if defined(BSLS_PLATFORM_OS_LINUX)
// NTSCFG_TEST_EQ(output, false);
#else
                        NTSCFG_TEST_EQ(output, INPUT[i]);
#endif
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_OOBINLINE.

        {
            const bool INPUT[] = {false, true};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                error =
                    ntsu::SocketOptionUtil::setInlineOutOfBandData(socket,
                                                                   INPUT[i]);

                NTSCFG_TEST_LOG_INFO << "setInlineOutOfBandData: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    bool output;
                    error =
                        ntsu::SocketOptionUtil::getInlineOutOfBandData(&output,
                                                                       socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_EQ(output, INPUT[i]);
                    }
                }
            }
        }

        // Test probing the amount of capacity left in the send buffer.

        {
            bsl::size_t size;
            error =
                ntsu::SocketOptionUtil::getSendBufferRemaining(&size, socket);

            NTSCFG_TEST_LOG_INFO << "getSendBufferRemaining: " << error
                                 << NTSCFG_TEST_LOG_END;

#if defined(BSLS_PLATFORM_OS_SOLARIS) || defined(BSLS_PLATFORM_OS_AIX) ||     \
    defined(BSLS_PLATFORM_OS_WINDOWS)
            NTSCFG_TEST_EQ(error, ntsa::Error::e_NOT_IMPLEMENTED);
#else
            NTSCFG_TEST_FALSE(error);
#endif
        }

        // Test probing the amount of data filled in the receive buffer.

        {
            bsl::size_t size;
            error = ntsu::SocketOptionUtil::getReceiveBufferAvailable(&size,
                                                                      socket);

            NTSCFG_TEST_LOG_INFO << "getReceiveBufferAvailable: " << error
                                 << NTSCFG_TEST_LOG_END;

            NTSCFG_TEST_FALSE(error);
        }

        // Retrieve the last error associated with the socket.

        {
            ntsa::Error lastError;
            error = ntsu::SocketOptionUtil::getLastError(&lastError, socket);

            NTSCFG_TEST_LOG_INFO << "getLastError: " << error
                                 << NTSCFG_TEST_LOG_END;

            NTSCFG_TEST_FALSE(error);
        }

        // Test IPPROTO_TCP/TCP_NODELAY.

        {
            const bool INPUT[] = {false, true};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                error = ntsu::SocketOptionUtil::setNoDelay(socket, INPUT[i]);

                NTSCFG_TEST_LOG_INFO << "setNoDelay: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    bool output;
                    error =
                        ntsu::SocketOptionUtil::getNoDelay(&output, socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_EQ(output, INPUT[i]);
                    }
                }
            }
        }

        // Close the socket.

        ntsu::SocketUtil::close(socket);
    }
}

NTSCFG_TEST_CASE(2)
{
    // Concern: Socket options on TCP/IPv4 sockets, using the union API.
    // Plan:

    const ntsa::Transport::Value SOCKET_TYPES[] = {
        ntsa::Transport::e_TCP_IPV4_STREAM,
        ntsa::Transport::e_TCP_IPV6_STREAM,
#if !defined(BSLS_PLATFORM_OS_WINDOWS)
        ntsa::Transport::e_LOCAL_STREAM,
#endif
        ntsa::Transport::e_UDP_IPV4_DATAGRAM,
        ntsa::Transport::e_UDP_IPV6_DATAGRAM,
#if !defined(BSLS_PLATFORM_OS_WINDOWS)
        ntsa::Transport::e_LOCAL_DATAGRAM,
#endif
    };

    for (bsl::size_t socketTypeIndex = 0;
         socketTypeIndex < sizeof(SOCKET_TYPES) / sizeof(SOCKET_TYPES[0]);
         ++socketTypeIndex)
    {
        ntsa::Transport::Value transport = SOCKET_TYPES[socketTypeIndex];

#if defined(NTSU_SOCKETOPTUTIL_TEST_SOCKET_TYPE)
        if (transport != NTSU_SOCKETOPTUTIL_TEST_SOCKET_TYPE) {
            continue;
        }
#endif

        if (transport == ntsa::Transport::e_TCP_IPV4_STREAM ||
            transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM)
        {
            if (!ntsu::AdapterUtil::supportsIpv4()) {
                continue;
            }
        }

        if (transport == ntsa::Transport::e_TCP_IPV6_STREAM ||
            transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM)
        {
            if (!ntsu::AdapterUtil::supportsIpv6()) {
                continue;
            }
        }

        NTSCFG_TEST_LOG_WARN << "Testing " << transport << NTSCFG_TEST_LOG_END;

        ntsa::Error error;

        // Create the socket.

        ntsa::Handle socket;
        error = ntsu::SocketUtil::create(&socket, transport);
        NTSCFG_TEST_FALSE(error);

        // Test SOL_SOCKET/SO_KEEPALIVE.

        {
            const bool INPUT[] = {false, true};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                ntsa::SocketOption input;
                input.makeKeepAlive(INPUT[i]);
                error = ntsu::SocketOptionUtil::setOption(socket, input);

                NTSCFG_TEST_LOG_INFO << "setKeepAlive: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    ntsa::SocketOption output;
                    error = ntsu::SocketOptionUtil::getOption(
                        &output,
                        ntsa::SocketOptionType::e_KEEP_ALIVE,
                        socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(output.isUndefined());
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_TRUE(output.isKeepAlive());
                        NTSCFG_TEST_EQ(output.keepAlive(), INPUT[i]);
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_DEBUG.

        {
            const bool INPUT[] = {false, true};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                ntsa::SocketOption input;
                input.makeDebug(INPUT[i]);
                error = ntsu::SocketOptionUtil::setOption(socket, input);

                NTSCFG_TEST_LOG_INFO << "setDebug: " << error
                                     << NTSCFG_TEST_LOG_END;

                // On Linux, at least, setting SO_DEBUG to true results
                // in EACCESS.
                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED ||
                                     error == ntsa::Error::e_NOT_AUTHORIZED);
                }
                else {
                    ntsa::SocketOption output;
                    error = ntsu::SocketOptionUtil::getOption(
                        &output,
                        ntsa::SocketOptionType::e_DEBUG,
                        socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(output.isUndefined());
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_TRUE(output.isDebug());
                        NTSCFG_TEST_EQ(output.debug(), INPUT[i]);
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_REUSEADDR

        {
            const bool INPUT[] = {false, true};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                ntsa::SocketOption input;
                input.makeReuseAddress(INPUT[i]);
                error = ntsu::SocketOptionUtil::setOption(socket, input);

                NTSCFG_TEST_LOG_INFO << "setReuseAddress: " << error
                                     << NTSCFG_TEST_LOG_END;

                NTSCFG_TEST_FALSE(error);

                ntsa::SocketOption output;
                error = ntsu::SocketOptionUtil::getOption(
                    &output,
                    ntsa::SocketOptionType::e_REUSE_ADDRESS,
                    socket);
                NTSCFG_TEST_FALSE(error);

                NTSCFG_TEST_TRUE(output.isReuseAddress());
                NTSCFG_TEST_EQ(output.reuseAddress(), INPUT[i]);
            }
        }

        // Test SOL_SOCKET/SO_LINGER

        {
            const struct Data {
                bool linger;
                int  seconds;
            } INPUT[] = {
                {false, 0},
                { true, 0},
                { true, 1}
            };

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                ntsa::SocketOption input;
                {
                    bool enabled = INPUT[i].linger;

                    bsls::TimeInterval duration;
                    duration.setTotalSeconds(INPUT[i].seconds);

                    ntsa::Linger linger;
                    linger.setEnabled(enabled);
                    linger.setDuration(duration);

                    input.makeLinger(linger);
                }
                error = ntsu::SocketOptionUtil::setOption(socket, input);

                NTSCFG_TEST_LOG_INFO << "setLinger: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    ntsa::SocketOption output;
                    error = ntsu::SocketOptionUtil::getOption(
                        &output,
                        ntsa::SocketOptionType::e_LINGER,
                        socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(output.isUndefined());
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_TRUE(output.isLinger());
                        NTSCFG_TEST_EQ(output.linger().enabled(),
                                       INPUT[i].linger);
                        NTSCFG_TEST_EQ(
                            output.linger().duration().totalSeconds(),
                            bsls::TimeInterval(INPUT[i].seconds));
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_SNDBUF.

        {
            const bsl::size_t INPUT[] = {1024, 1024 * 1024};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                ntsa::SocketOption input;
                input.makeSendBufferSize(INPUT[i]);
                error = ntsu::SocketOptionUtil::setOption(socket, input);

                NTSCFG_TEST_LOG_INFO << "setSendBufferSize: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    ntsa::SocketOption output;
                    error = ntsu::SocketOptionUtil::getOption(
                        &output,
                        ntsa::SocketOptionType::e_SEND_BUFFER_SIZE,
                        socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(output.isUndefined());
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_TRUE(output.isSendBufferSize());
// Linux and Solaris manages the actual size of the
// send buffer itself, using the requested size as
// a hint.
#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_SOLARIS)
                        NTSCFG_TEST_GT(output.sendBufferSize(), 0);
#else
                        NTSCFG_TEST_EQ(output.sendBufferSize(), INPUT[i]);
#endif
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_SNDLOWAT

        {
            const bsl::size_t INPUT[] = {1024, 1024 * 1024};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                ntsa::SocketOption input;
                input.makeSendBufferLowWatermark(INPUT[i]);
                error = ntsu::SocketOptionUtil::setOption(socket, input);

                NTSCFG_TEST_LOG_INFO << "setSendBufferLowWatermark: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    ntsa::SocketOption output;
                    error = ntsu::SocketOptionUtil::getOption(
                        &output,
                        ntsa::SocketOptionType::e_SEND_BUFFER_LOW_WATERMARK,
                        socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(output.isUndefined());
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_TRUE(output.isSendBufferLowWatermark());
                        NTSCFG_TEST_EQ(output.sendBufferLowWatermark(),
                                       INPUT[i]);
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_RCVBUF.

        {
            const bsl::size_t INPUT[] = {1024, 1024 * 1024};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                ntsa::SocketOption input;
                input.makeReceiveBufferSize(INPUT[i]);
                error = ntsu::SocketOptionUtil::setOption(socket, input);

                NTSCFG_TEST_LOG_INFO << "setReceiveBufferSize: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    ntsa::SocketOption output;
                    error = ntsu::SocketOptionUtil::getOption(
                        &output,
                        ntsa::SocketOptionType::e_RECEIVE_BUFFER_SIZE,
                        socket);

                    if (error) {
                        NTSCFG_TEST_TRUE(output.isUndefined());
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_TRUE(output.isReceiveBufferSize());
// Linux and Solaris manages the actual size of the
// receive buffer itself, using the requested size as
// a hint.
#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_SOLARIS)
                        NTSCFG_TEST_GT(output.receiveBufferSize(), 0);
#else
                        NTSCFG_TEST_EQ(output.receiveBufferSize(), INPUT[i]);
#endif
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_RCVLOWAT

        {
            const bsl::size_t INPUT[] = {1024, 1024 * 1024};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                ntsa::SocketOption input;
                input.makeReceiveBufferLowWatermark(INPUT[i]);
                error = ntsu::SocketOptionUtil::setOption(socket, input);

                NTSCFG_TEST_LOG_INFO
                    << "setReceiveBufferLowWatermark: " << error
                    << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    ntsa::SocketOption output;
                    error = ntsu::SocketOptionUtil::getOption(
                        &output,
                        ntsa::SocketOptionType::e_RECEIVE_BUFFER_LOW_WATERMARK,
                        socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(output.isUndefined());
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_TRUE(output.isReceiveBufferLowWatermark());
// Linux and Solaris manages the actual size of the
// receive buffer itself, using the requested size as
// a hint.
#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_SOLARIS)
                        NTSCFG_TEST_GT(output.receiveBufferLowWatermark(), 0);
#else
                        NTSCFG_TEST_EQ(output.receiveBufferLowWatermark(),
                                       INPUT[i]);
#endif
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_BROADCAST.

        {
            const bool INPUT[] = {false, true};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                ntsa::SocketOption input;
                input.makeBroadcast(INPUT[i]);
                error = ntsu::SocketOptionUtil::setOption(socket, input);

                NTSCFG_TEST_LOG_INFO << "setBroadcast: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    ntsa::SocketOption output;
                    error = ntsu::SocketOptionUtil::getOption(
                        &output,
                        ntsa::SocketOptionType::e_BROADCAST,
                        socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(output.isUndefined());
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_TRUE(output.isBroadcast());
                        NTSCFG_TEST_EQ(output.broadcast(), INPUT[i]);
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_DONTROUTE

        {
            const bool INPUT[] = {false, true};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                ntsa::SocketOption input;
                input.makeBypassRouting(INPUT[i]);
                error = ntsu::SocketOptionUtil::setOption(socket, input);

                NTSCFG_TEST_LOG_INFO << "setBypassRouting: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    ntsa::SocketOption output;
                    error = ntsu::SocketOptionUtil::getOption(
                        &output,
                        ntsa::SocketOptionType::e_BYPASS_ROUTING,
                        socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(output.isUndefined());
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_TRUE(output.isBypassRouting());
// Linux returns false even if set to true (sometimes).
#if defined(BSLS_PLATFORM_OS_LINUX)
// NTSCFG_TEST_EQ(output, false);
#else
                        NTSCFG_TEST_EQ(output.bypassRouting(), INPUT[i]);
#endif
                    }
                }
            }
        }

        // Test SOL_SOCKET/SO_OOBINLINE.

        {
            const bool INPUT[] = {false, true};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                ntsa::SocketOption input;
                input.makeInlineOutOfBandData(INPUT[i]);
                error = ntsu::SocketOptionUtil::setOption(socket, input);

                NTSCFG_TEST_LOG_INFO << "setInlineOutOfBandData: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    ntsa::SocketOption output;
                    error = ntsu::SocketOptionUtil::getOption(
                        &output,
                        ntsa::SocketOptionType::e_INLINE_OUT_OF_BAND_DATA,
                        socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(output.isUndefined());
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_TRUE(output.isInlineOutOfBandData());
                        NTSCFG_TEST_EQ(output.inlineOutOfBandData(), INPUT[i]);
                    }
                }
            }
        }

        // Test probing the amount of capacity left in the send buffer.

        {
            bsl::size_t size;
            error =
                ntsu::SocketOptionUtil::getSendBufferRemaining(&size, socket);

            NTSCFG_TEST_LOG_INFO << "getSendBufferRemaining: " << error
                                 << NTSCFG_TEST_LOG_END;

#if defined(BSLS_PLATFORM_OS_SOLARIS) || defined(BSLS_PLATFORM_OS_AIX) ||     \
    defined(BSLS_PLATFORM_OS_WINDOWS)
            NTSCFG_TEST_EQ(error, ntsa::Error::e_NOT_IMPLEMENTED);
#else
            NTSCFG_TEST_FALSE(error);
#endif
        }

        // Test probing the amount of data filled in the receive buffer.

        {
            bsl::size_t size;
            error = ntsu::SocketOptionUtil::getReceiveBufferAvailable(&size,
                                                                      socket);

            NTSCFG_TEST_LOG_INFO << "getReceiveBufferAvailable: " << error
                                 << NTSCFG_TEST_LOG_END;

            NTSCFG_TEST_FALSE(error);
        }

        // Retrieve the last error associated with the socket.

        {
            ntsa::Error lastError;
            error = ntsu::SocketOptionUtil::getLastError(&lastError, socket);

            NTSCFG_TEST_LOG_INFO << "getLastError: " << error
                                 << NTSCFG_TEST_LOG_END;

            NTSCFG_TEST_FALSE(error);
        }

        // Test IPPROTO_TCP/TCP_NODELAY.

        {
            const bool INPUT[] = {false, true};

            for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0]; ++i) {
                ntsa::SocketOption input;
                input.makeDelayTransmission(INPUT[i]);
                error = ntsu::SocketOptionUtil::setOption(socket, input);

                NTSCFG_TEST_LOG_INFO << "setNoDelay: " << error
                                     << NTSCFG_TEST_LOG_END;

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    ntsa::SocketOption output;
                    error = ntsu::SocketOptionUtil::getOption(
                        &output,
                        ntsa::SocketOptionType::e_DELAY_TRANSMISSION,
                        socket);
                    if (error) {
                        NTSCFG_TEST_TRUE(output.isUndefined());
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                    else {
                        NTSCFG_TEST_TRUE(output.isDelayTransmission());
                        NTSCFG_TEST_EQ(output.delayTransmission(), INPUT[i]);
                    }
                }
            }
        }

        // Close the socket.

        ntsu::SocketUtil::close(socket);
    }
}

NTSCFG_TEST_CASE(3)
{
    // Concern: Multicast options
    // Plan:

    const ntsa::Transport::Value SOCKET_TYPES[] = {
        ntsa::Transport::e_UDP_IPV4_DATAGRAM,
        ntsa::Transport::e_UDP_IPV6_DATAGRAM};

    bsl::vector<ntsa::Adapter> adapters;
    ntsu::AdapterUtil::discoverAdapterList(&adapters);

    for (bsl::size_t adapterIndex = 0; adapterIndex < adapters.size();
         ++adapterIndex)
    {
        const ntsa::Adapter& adapter = adapters[adapterIndex];

        if (!adapter.multicast()) {
            continue;
        }

        BSLS_LOG_WARN("Testing adapter '%s'", adapter.name().c_str());

        for (bsl::size_t socketTypeIndex = 0;
             socketTypeIndex < sizeof(SOCKET_TYPES) / sizeof(SOCKET_TYPES[0]);
             ++socketTypeIndex)
        {
            ntsa::Transport::Value transport = SOCKET_TYPES[socketTypeIndex];

#if defined(NTSU_SOCKETOPTUTIL_TEST_SOCKET_TYPE)
            if (transport != NTSU_SOCKETOPTUTIL_TEST_SOCKET_TYPE) {
                continue;
            }
#endif

            if (transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM &&
                adapter.ipv4Address().isNull())
            {
                continue;
            }

            if (transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM &&
                adapter.ipv6Address().isNull())
            {
                continue;
            }

            {
                bsl::stringstream ss;
                ss << transport;
                BSLS_LOG_WARN("Testing %s", ss.str().c_str());
            }

            ntsa::Error error;

            // Create the socket.

            ntsa::Handle socket;
            error = ntsu::SocketUtil::create(&socket, transport);
            NTSCFG_TEST_FALSE(error);

            // Bind the socket to the adapter's address.

            if (transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
                ntsa::Endpoint endpoint(
                    ntsa::IpEndpoint(adapter.ipv4Address().value(), 0));
                error = ntsu::SocketUtil::bind(endpoint, true, socket);
                NTSCFG_TEST_FALSE(error);
            }
            else {
                ntsa::Endpoint endpoint(
                    ntsa::IpEndpoint(adapter.ipv6Address().value(), 0));
                error = ntsu::SocketUtil::bind(endpoint, true, socket);
                NTSCFG_TEST_FALSE(error);
            }

            // Test multicast loopback.

            {
                const bool INPUT[] = {false, true};

                for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0];
                     ++i)
                {
                    error =
                        ntsu::SocketOptionUtil::setMulticastLoopback(socket,
                                                                     INPUT[i]);

                    BSLS_LOG_INFO("setMulticastLoopback: %s",
                                  error.text().c_str());

                    if (error) {
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                }
            }

            // Test multicast time-to-live.

            {
                const bsl::size_t INPUT[] = {0, 1, 2, 3};

                for (bsl::size_t i = 0; i < sizeof INPUT / sizeof INPUT[0];
                     ++i)
                {
                    error = ntsu::SocketOptionUtil::setMulticastTimeToLive(
                        socket,
                        INPUT[i]);

                    BSLS_LOG_INFO("setMulticastTimeToLive: %s",
                                  error.text().c_str());

                    if (error) {
                        NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                         error ==
                                             ntsa::Error::e_NOT_IMPLEMENTED);
                    }
                }
            }

            // Test multicast interface affinity.

            {
                if (transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
                    error = ntsu::SocketOptionUtil::setMulticastInterface(
                        socket,
                        ntsa::IpAddress(adapter.ipv4Address().value()));
                }
                else {
                    error = ntsu::SocketOptionUtil::setMulticastInterface(
                        socket,
                        ntsa::IpAddress(adapter.ipv6Address().value()));
                }

                BSLS_LOG_INFO("setMulticastInterface: %s",
                              error.text().c_str());

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
            }

            // Test join multicast group.

            {
                if (transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
                    error = ntsu::SocketOptionUtil::joinMulticastGroup(
                        socket,
                        ntsa::IpAddress(adapter.ipv4Address().value()),
                        ntsa::IpAddress("224.0.0.0"));
                }
                else {
                    error = ntsu::SocketOptionUtil::joinMulticastGroup(
                        socket,
                        ntsa::IpAddress(adapter.ipv6Address().value()),
                        ntsa::IpAddress(
                            "ff00:0000:0000:0000:0000:0000:0000:0000"));
                }

                BSLS_LOG_INFO("joinMulticastGroup: %s", error.text().c_str());

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
            }

            // Test leave multicast group.

            {
                if (transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
                    error = ntsu::SocketOptionUtil::leaveMulticastGroup(
                        socket,
                        ntsa::IpAddress(adapter.ipv4Address().value()),
                        ntsa::IpAddress("224.0.0.0"));
                }
                else {
                    error = ntsu::SocketOptionUtil::leaveMulticastGroup(
                        socket,
                        ntsa::IpAddress(adapter.ipv6Address().value()),
                        ntsa::IpAddress(
                            "ff00:0000:0000:0000:0000:0000:0000:0000"));
                }

                BSLS_LOG_INFO("leaveMulticastGroup: %s", error.text().c_str());

                if (error) {
                    NTSCFG_TEST_TRUE(error == ntsa::Error::e_INVALID ||
                                     error == ntsa::Error::e_NOT_IMPLEMENTED);
                }
            }

            // Close the socket.

            ntsu::SocketUtil::close(socket);
        }
    }
}

NTSCFG_TEST_CASE(4)
{
    // Concern: Default Socket options on TCP/IPv4 sockets.
    // Plan:

    ntsa::Error error;

    // Create the socket.

    ntsa::Handle socket;
    error =
        ntsu::SocketUtil::create(&socket, ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_FALSE(error);

    bool noDelay = false;
    error        = ntsu::SocketOptionUtil::getNoDelay(&noDelay, socket);
    NTSCFG_TEST_FALSE(error);

    if (NTSCFG_TEST_VERBOSITY) {
        bsl::cout << "TCP_NODELAY: " << noDelay << bsl::endl;
    }

    // Close the socket.

    ntsu::SocketUtil::close(socket);
}

NTSCFG_TEST_CASE(5)
{
    // Concern: test switching RX timestamping on and off

    const ntsa::Transport::Value SOCKET_TYPES[] = {
        ntsa::Transport::e_TCP_IPV4_STREAM,
        ntsa::Transport::e_TCP_IPV6_STREAM,
#if !defined(BSLS_PLATFORM_OS_WINDOWS)
        ntsa::Transport::e_LOCAL_STREAM,
#endif
        ntsa::Transport::e_UDP_IPV4_DATAGRAM,
        ntsa::Transport::e_UDP_IPV6_DATAGRAM,
#if !defined(BSLS_PLATFORM_OS_WINDOWS)
        ntsa::Transport::e_LOCAL_DATAGRAM,
#endif
    };

    for (bsl::size_t socketTypeIndex = 0;
         socketTypeIndex < sizeof(SOCKET_TYPES) / sizeof(SOCKET_TYPES[0]);
         ++socketTypeIndex)
    {
        ntsa::Transport::Value transport = SOCKET_TYPES[socketTypeIndex];

        if (transport == ntsa::Transport::e_TCP_IPV4_STREAM ||
            transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM)
        {
            if (!ntsu::AdapterUtil::supportsIpv4()) {
                continue;
            }
        }

        if (transport == ntsa::Transport::e_TCP_IPV6_STREAM ||
            transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM)
        {
            if (!ntsu::AdapterUtil::supportsIpv6()) {
                continue;
            }
        }

        NTSCFG_TEST_LOG_WARN << "Testing " << transport << NTSCFG_TEST_LOG_END;

        ntsa::Error error;

        // Create the socket.

        ntsa::Handle socket;
        error = ntsu::SocketUtil::create(&socket, transport);
        NTSCFG_TEST_OK(error);

        error = ntsu::SocketOptionUtil::setTimestampIncomingData(socket, true);
#if defined(BSLS_PLATFORM_OS_LINUX)
        NTSCFG_TEST_OK(error);

        int       optVal = 0;
        socklen_t optLen = sizeof(optVal);

        if (transport == ntsa::Transport::e_LOCAL_DATAGRAM ||
            transport == ntsa::Transport::e_LOCAL_STREAM)
        {
            const int rc = getsockopt(socket,
                                      SOL_SOCKET,
                                      ntsu::TimestampUtil::e_SO_TIMESTAMPNS,
                                      &optVal,
                                      &optLen);
            NTSCFG_TEST_EQ(rc, 0);
            NTSCFG_TEST_NE(optVal, 0);
        }
        else {
            const int expected =
                ntsu::TimestampUtil::e_SOF_TIMESTAMPING_RX_HARDWARE |
                ntsu::TimestampUtil::e_SOF_TIMESTAMPING_RX_SOFTWARE |
                ntsu::TimestampUtil::e_SOF_TIMESTAMPING_RAW_HARDWARE |
                ntsu::TimestampUtil::e_SOF_TIMESTAMPING_SOFTWARE;
            const int rc = getsockopt(socket,
                                      SOL_SOCKET,
                                      ntsu::TimestampUtil::e_SO_TIMESTAMPING,
                                      &optVal,
                                      &optLen);
            NTSCFG_TEST_EQ(rc, 0);
            NTSCFG_TEST_EQ(optVal, expected);
        }

        // now switch off timestamping
        error =
            ntsu::SocketOptionUtil::setTimestampIncomingData(socket, false);
        NTSCFG_TEST_OK(error);

        optVal = 0;
        optLen = sizeof(optVal);

        if (transport == ntsa::Transport::e_LOCAL_DATAGRAM ||
            transport == ntsa::Transport::e_LOCAL_STREAM)
        {
            const int rc = getsockopt(socket,
                                      SOL_SOCKET,
                                      ntsu::TimestampUtil::e_SO_TIMESTAMPNS,
                                      &optVal,
                                      &optLen);
            NTSCFG_TEST_EQ(rc, 0);
            NTSCFG_TEST_EQ(optVal, 0);
        }
        else {
            const int rc = getsockopt(socket,
                                      SOL_SOCKET,
                                      ntsu::TimestampUtil::e_SO_TIMESTAMPING,
                                      &optVal,
                                      &optLen);
            NTSCFG_TEST_EQ(rc, 0);
            NTSCFG_TEST_EQ(optVal, 0);
        }
#else
        NTSCFG_TEST_ERROR(error, ntsa::Error::e_NOT_IMPLEMENTED);
#endif

        error = ntsu::SocketUtil::close(socket);
        NTSCFG_TEST_OK(error);
    }
}

NTSCFG_TEST_CASE(6)
{
    // Concern: test set and get timestampIncomingData

    const ntsa::Transport::Value SOCKET_TYPES[] = {
        ntsa::Transport::e_TCP_IPV4_STREAM,
        ntsa::Transport::e_TCP_IPV6_STREAM,
#if !defined(BSLS_PLATFORM_OS_WINDOWS)
        ntsa::Transport::e_LOCAL_STREAM,
#endif
        ntsa::Transport::e_UDP_IPV4_DATAGRAM,
        ntsa::Transport::e_UDP_IPV6_DATAGRAM,
#if !defined(BSLS_PLATFORM_OS_WINDOWS)
        ntsa::Transport::e_LOCAL_DATAGRAM,
#endif
    };

    for (bsl::size_t socketTypeIndex = 0;
         socketTypeIndex < sizeof(SOCKET_TYPES) / sizeof(SOCKET_TYPES[0]);
         ++socketTypeIndex)
    {
        ntsa::Transport::Value transport = SOCKET_TYPES[socketTypeIndex];

        if (transport == ntsa::Transport::e_TCP_IPV4_STREAM ||
            transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM)
        {
            if (!ntsu::AdapterUtil::supportsIpv4()) {
                continue;
            }
        }

        if (transport == ntsa::Transport::e_TCP_IPV6_STREAM ||
            transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM)
        {
            if (!ntsu::AdapterUtil::supportsIpv6()) {
                continue;
            }
        }

        NTSCFG_TEST_LOG_WARN << "Testing " << transport << NTSCFG_TEST_LOG_END;

        ntsa::Handle socket;
        {
            ntsa::Error error = ntsu::SocketUtil::create(&socket, transport);
            NTSCFG_TEST_OK(error);
        }

#if defined(BSLS_PLATFORM_OS_LINUX)
        ntsa::Error error =
            ntsu::SocketOptionUtil::setTimestampIncomingData(socket, true);
        NTSCFG_TEST_OK(error);

        bool enabled = false;
        error =
            ntsu::SocketOptionUtil::getTimestampIncomingData(&enabled, socket);
        NTSCFG_TEST_OK(error);
        NTSCFG_TEST_TRUE(enabled);

        error =
            ntsu::SocketOptionUtil::setTimestampIncomingData(socket, false);
        NTSCFG_TEST_OK(error);

        error =
            ntsu::SocketOptionUtil::getTimestampIncomingData(&enabled, socket);
        NTSCFG_TEST_OK(error);
        NTSCFG_TEST_FALSE(enabled);
#else
        ntsa::Error error =
            ntsu::SocketOptionUtil::setTimestampIncomingData(socket, true);
        NTSCFG_TEST_ERROR(error, ntsa::Error::e_NOT_IMPLEMENTED);

        bool enabled = false;
        error =
            ntsu::SocketOptionUtil::getTimestampIncomingData(&enabled, socket);
        NTSCFG_TEST_ERROR(error, ntsa::Error::e_NOT_IMPLEMENTED);
#endif
    }
}

NTSCFG_TEST_CASE(7)
{
    // Concern: test switching TX timestamping on and off for dgram sockets
    // and not connected stream sockets

    const ntsa::Transport::Value SOCKET_TYPES[] = {
        ntsa::Transport::e_TCP_IPV4_STREAM,
        ntsa::Transport::e_TCP_IPV6_STREAM,
#if !defined(BSLS_PLATFORM_OS_WINDOWS)
        ntsa::Transport::e_LOCAL_STREAM,
#endif
        ntsa::Transport::e_UDP_IPV4_DATAGRAM,
        ntsa::Transport::e_UDP_IPV6_DATAGRAM,
#if !defined(BSLS_PLATFORM_OS_WINDOWS)
        ntsa::Transport::e_LOCAL_DATAGRAM,
#endif
    };

    for (bsl::size_t socketTypeIndex = 0;
         socketTypeIndex < sizeof(SOCKET_TYPES) / sizeof(SOCKET_TYPES[0]);
         ++socketTypeIndex)
    {
        ntsa::Transport::Value transport = SOCKET_TYPES[socketTypeIndex];

        if (transport == ntsa::Transport::e_TCP_IPV4_STREAM ||
            transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM)
        {
            if (!ntsu::AdapterUtil::supportsIpv4()) {
                continue;
            }
        }

        if (transport == ntsa::Transport::e_TCP_IPV6_STREAM ||
            transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM)
        {
            if (!ntsu::AdapterUtil::supportsIpv6()) {
                continue;
            }
        }

        NTSCFG_TEST_LOG_WARN << "Testing " << transport << NTSCFG_TEST_LOG_END;

        ntsa::Error error;

        // Create the socket.

        ntsa::Handle socket;
        error = ntsu::SocketUtil::create(&socket, transport);
        NTSCFG_TEST_OK(error);

        error = ntsu::SocketOptionUtil::setTimestampOutgoingData(socket, true);
#if defined(BSLS_PLATFORM_OS_LINUX)
        if (!ntscfg::Platform::supportsTimestamps()) {
            NTSCFG_TEST_ERROR(error, ntsa::Error::e_INVALID);
        }
        else if (transport == ntsa::Transport::e_LOCAL_DATAGRAM ||
                 transport == ntsa::Transport::e_LOCAL_STREAM ||
                 // all stream sockets do not support tx timestamping unless
                 // they are in connected state
                 transport == ntsa::Transport::e_TCP_IPV4_STREAM ||
                 transport == ntsa::Transport::e_TCP_IPV6_STREAM)

        {
            NTSCFG_TEST_ERROR(error, ntsa::Error::e_INVALID);
        }
        else {
            NTSCFG_TEST_OK(error);

            int       optVal = 0;
            socklen_t optLen = sizeof(optVal);

            const int expected =
                ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_SOFTWARE |
                ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_SCHED |
                ntsu::TimestampUtil::e_SOF_TIMESTAMPING_SOFTWARE |
                ntsu::TimestampUtil::e_SOF_TIMESTAMPING_OPT_ID |
                ntsu::TimestampUtil::e_SOF_TIMESTAMPING_OPT_TSONLY;
            const int rc = getsockopt(socket,
                                      SOL_SOCKET,
                                      ntsu::TimestampUtil::e_SO_TIMESTAMPING,
                                      &optVal,
                                      &optLen);
            NTSCFG_TEST_EQ(rc, 0);
            NTSCFG_TEST_EQ(optVal, expected);

            // now switch off timestamping
            error = ntsu::SocketOptionUtil::setTimestampOutgoingData(socket,
                                                                     false);
            NTSCFG_TEST_OK(error);
        }

#else
        NTSCFG_TEST_ERROR(error, ntsa::Error::e_NOT_IMPLEMENTED);
#endif

        error = ntsu::SocketUtil::close(socket);
        NTSCFG_TEST_OK(error);
    }
}

NTSCFG_TEST_CASE(8)
{
    // Concern: test isLocal

    ntsa::Error error;

    const ntsa::Transport::Value SOCKET_TYPES[] = {
        ntsa::Transport::e_TCP_IPV4_STREAM,
        ntsa::Transport::e_TCP_IPV6_STREAM,
#if !defined(BSLS_PLATFORM_OS_WINDOWS)
        ntsa::Transport::e_LOCAL_STREAM,
#endif
        ntsa::Transport::e_UDP_IPV4_DATAGRAM,
        ntsa::Transport::e_UDP_IPV6_DATAGRAM,
#if !defined(BSLS_PLATFORM_OS_WINDOWS)
        ntsa::Transport::e_LOCAL_DATAGRAM,
#endif
    };

    for (bsl::size_t socketTypeIndex = 0;
         socketTypeIndex < sizeof(SOCKET_TYPES) / sizeof(SOCKET_TYPES[0]);
         ++socketTypeIndex)
    {
        ntsa::Transport::Value transport = SOCKET_TYPES[socketTypeIndex];

        if (transport == ntsa::Transport::e_TCP_IPV4_STREAM ||
            transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM)
        {
            if (!ntsu::AdapterUtil::supportsIpv4()) {
                continue;
            }
        }

        if (transport == ntsa::Transport::e_TCP_IPV6_STREAM ||
            transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM)
        {
            if (!ntsu::AdapterUtil::supportsIpv6()) {
                continue;
            }
        }

        NTSCFG_TEST_LOG_WARN << "Testing " << transport << NTSCFG_TEST_LOG_END;

        ntsa::Handle socket;
        {
            ntsa::Error error = ntsu::SocketUtil::create(&socket, transport);
            NTSCFG_TEST_OK(error);
        }

        bool isLocal;
        error = ntsu::SocketOptionUtil::isLocal(&isLocal, socket);

        if (transport == ntsa::Transport::e_LOCAL_DATAGRAM ||
            transport == ntsa::Transport::e_LOCAL_STREAM)
        {
            NTSCFG_TEST_OK(error);
            NTSCFG_TEST_TRUE(isLocal);
        }
        else {
            NTSCFG_TEST_OK(error);
            NTSCFG_TEST_FALSE(isLocal);
        }
    }
}

NTSCFG_TEST_CASE(9)
{
    // test SO_ZEROCOPY
    const ntsa::Transport::Value SOCKET_TYPES[] = {
        ntsa::Transport::e_TCP_IPV4_STREAM,
        ntsa::Transport::e_TCP_IPV6_STREAM,
#if !defined(BSLS_PLATFORM_OS_WINDOWS)
        ntsa::Transport::e_LOCAL_STREAM,
#endif
        ntsa::Transport::e_UDP_IPV4_DATAGRAM,
        ntsa::Transport::e_UDP_IPV6_DATAGRAM,
#if !defined(BSLS_PLATFORM_OS_WINDOWS)
        ntsa::Transport::e_LOCAL_DATAGRAM,
#endif
    };

    for (bsl::size_t socketTypeIndex = 0;
         socketTypeIndex < sizeof(SOCKET_TYPES) / sizeof(SOCKET_TYPES[0]);
         ++socketTypeIndex)
    {
        ntsa::Transport::Value transport = SOCKET_TYPES[socketTypeIndex];

        if (transport == ntsa::Transport::e_TCP_IPV4_STREAM ||
            transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM)
        {
            if (!ntsu::AdapterUtil::supportsIpv4()) {
                continue;
            }
        }

        if (transport == ntsa::Transport::e_TCP_IPV6_STREAM ||
            transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM)
        {
            if (!ntsu::AdapterUtil::supportsIpv6()) {
                continue;
            }
        }

#if defined(BSLS_PLATFORM_OS_LINUX)

        bool setSupported = false;
        bool getSupported = false;

        {
            int major, minor, patch, build;
            NTSCFG_TEST_ASSERT(ntsscm::Version::systemVersion(&major,
                                                              &minor,
                                                              &patch,
                                                              &build) == 0);

            // Linux kernels versions < 4.14.0 do not support MSG_ZEROCOPY at
            // all
            // Linux kernels versions < 5.0.0 do not support MSG_ZEROCOPY for
            // DGRAM sockets
            if (KERNEL_VERSION(major, minor, patch) < KERNEL_VERSION(4, 14, 0))
            {
                setSupported = false;
                getSupported = false;
            }
            else if (KERNEL_VERSION(major, minor, patch) <
                     KERNEL_VERSION(5, 0, 0))
            {
                if (transport == ntsa::Transport::e_TCP_IPV4_STREAM ||
                    transport == ntsa::Transport::e_TCP_IPV4_STREAM)
                {
                    setSupported = true;
                }
                getSupported = true;
            }
            else {
                setSupported =
                    (transport != ntsa::Transport::e_LOCAL_STREAM) &&
                    (transport != ntsa::Transport::e_LOCAL_DATAGRAM);
                getSupported = true;
            }
        }

#else
        const bool setSupported = false;
        const bool getSupported = false;
#endif

        NTSCFG_TEST_LOG_WARN << "Testing " << transport << NTSCFG_TEST_LOG_END;

        ntsa::Error error;

        // Create the socket.

        ntsa::Handle socket;
        error = ntsu::SocketUtil::create(&socket, transport);
        NTSCFG_TEST_OK(error);

        if (setSupported && getSupported) {
            bool zeroCopy = true;
            error =
                ntsu::SocketOptionUtil::getZeroCopy(&zeroCopy, socket);
            NTSCFG_TEST_OK(error);
            NTSCFG_TEST_FALSE(zeroCopy);

            error = ntsu::SocketOptionUtil::setZeroCopy(socket, true);
            NTSCFG_TEST_OK(error);

            zeroCopy = false;
            error =
                ntsu::SocketOptionUtil::getZeroCopy(&zeroCopy, socket);
            NTSCFG_TEST_OK(error);
            NTSCFG_TEST_TRUE(zeroCopy);

            error = ntsu::SocketOptionUtil::setZeroCopy(socket, false);
            NTSCFG_TEST_OK(error);

            zeroCopy = true;
            error =
                ntsu::SocketOptionUtil::getZeroCopy(&zeroCopy, socket);
            NTSCFG_TEST_OK(error);
            NTSCFG_TEST_FALSE(zeroCopy);
        }
        else if (setSupported == false && getSupported == true) {
            bool zeroCopy = true;
            error =
                ntsu::SocketOptionUtil::getZeroCopy(&zeroCopy, socket);
            NTSCFG_TEST_OK(error);
            NTSCFG_TEST_FALSE(zeroCopy);

            error = ntsu::SocketOptionUtil::setZeroCopy(socket, false);
            NTSCFG_TEST_TRUE(error);
        }
        else if (setSupported == false && getSupported == false) {
            bool zeroCopy = true;
            error =
                ntsu::SocketOptionUtil::getZeroCopy(&zeroCopy, socket);
            NTSCFG_TEST_ERROR(error,
                              ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED));
            NTSCFG_TEST_TRUE(zeroCopy);
        }
    }
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
    NTSCFG_TEST_REGISTER(4);
    NTSCFG_TEST_REGISTER(5);
    NTSCFG_TEST_REGISTER(6);
    NTSCFG_TEST_REGISTER(7);
    NTSCFG_TEST_REGISTER(8);
    NTSCFG_TEST_REGISTER(9);
}
NTSCFG_TEST_DRIVER_END;
