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

// Include sendmmsg/recvmmsg
#if defined(__linux__)
#define _GNU_SOURCE 1
#endif

#include <ntsu_socketutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsu_socketutil_cpp, "$Id$ $CSID$")

#include <ntsa_adapter.h>
#include <ntscfg_limits.h>
#include <ntsu_adapterutil.h>
#include <ntsu_bufferutil.h>
#include <ntsu_zerocopyutil.h>
#include <ntsu_socketoptionutil.h>
#include <ntsu_timestamputil.h>

#include <bdlb_chartype.h>
#include <bdlb_guid.h>
#include <bdlb_guidutil.h>
#include <bdlb_string.h>
#include <bdlt_currenttime.h>
#include <bslim_printer.h>
#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsls_platform.h>
#include <bsls_spinlock.h>
#include <bsls_timeinterval.h>
#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_set.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>

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
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <unistd.h>
#if defined(BSLS_PLATFORM_OS_SOLARIS)
#include <sys/filio.h>
#endif
#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_SOLARIS)
#include <sys/sendfile.h>
#endif
#if defined(BSLS_PLATFORM_OS_LINUX)
#include <linux/errqueue.h>
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
#include <ws2def.h>
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
// Unix Domain Socket requirements on Windows:
//
// 1) Support for the AF_UNIX socket driver can be detected by running
// 'sc query afunix' from the command line with Administrator privileges.
//
// 2) The sockaddr_un structure is declared in <afunix.h>. It is not known
// if this header is present in all platform SDK versions on the supported
// operating systems. For now, do not include the <afunix.h> header to allow
// compilation on machines not having the platform SDK version that contains
// that header. The sole necessary structure, sockaddr_un, is "manually"
// redeclared, below.
//
// 2) When an AF_UNIX socket is created, C:\Windows\System32\wshunix.dll is
// dynamically loaded. If this DLL is missing, it is not currently known if
// the either the WSASocket function call simply fails or if some other, more
// drastic negative consequence to the process occurs.
#endif
// clang-format on
#ifdef interface
#undef interface
#endif
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "mswsock")

#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
// Declare the sockaddr_un structure "manually" to allow compilation on
// machines not having the platform SDK version that contains the <afunix.h>
// header.
#define SOCKADDR_UN_WIN32_PATH_MAX 108
struct sockaddr_un_win32 {
    ADDRESS_FAMILY sun_family;
    char           sun_path[SOCKADDR_UN_WIN32_PATH_MAX];
};
#define sockaddr_un sockaddr_un_win32
#endif
#endif

// Define and set to 1 to always call 'sendmsg' instead of 'send' or 'writev'.
// Uncomment or set to 0 to use 'send' for single contiguous buffers and
// 'writev' for multiple non-contiguous buffers.
#define NTSU_SOCKETUTIL_SENDMSG_ALWAYS 1

// Define and set to 1 to always call 'recvmsg' instead of 'recv' or 'readv'.
// Uncomment or set to 0 to use 'recv' for single contiguous buffers and
// 'readv' for multiple non-contiguous buffers.
#define NTSU_SOCKETUTIL_RECVMSG_ALWAYS 1

#if defined(BSLS_PLATFORM_OS_LINUX)
// Define the default flags to 'sendmsg'.
#define NTSU_SOCKETUTIL_SENDMSG_FLAGS MSG_NOSIGNAL
#else
// Define the default flags to 'sendmsg'.
#define NTSU_SOCKETUTIL_SENDMSG_FLAGS 0
#endif

// Define the default flags to 'recvmsg'.
#define NTSU_SOCKETUTIL_RECVMSG_FLAGS 0

// The maximum number of buffers to submit to a call to a gathered write,
// regardless of a greater system limit.
#define NTSU_SOCKETUTIL_LIMIT_MAX_BUFFERS_PER_SEND 256

// The maximum number of buffers to submit to a call to a scattered read,
// regardless of a greater system limit.
#define NTSU_SOCKETUTIL_LIMIT_MAX_BUFFERS_PER_RECEIVE 256

// The maximum number of messages to submit to a call to a simultaneously
// send multiple messages in a single system call, regardless of a greater
// system limit.
#define NTSU_SOCKETUTIL_LIMIT_MAX_MESSAGES_PER_SEND 16

// The maximum number of messages to submit to a call to a simultaneously
// receive multiple messages in a single system call, regardless of a greater
// system limit.
#define NTSU_SOCKETUTIL_LIMIT_MAX_MESSAGES_PER_RECEIVE 16

// Flag to limit the maximum number of bytes submitted per call to
// a gathered write by the size of the send buffer. If true, the implementation
// will perform a system call before attempting to copy data to the socket
// send buffer, otherwise, the implementation tries to copy the data specified
// by the user, potentially limiting it by the internally-enforced buffer
// limits.
#define NTSU_SOCKETUTIL_LIMIT_MAX_BYTES_PER_SEND 0

// The flag to limit the maximum number of bytes submitted per call to
// a scattered read by the number of bytes available in the receive buffer. If
// true, the implementation will perform a system call before attempting to
// copy data to the socket send buffer, otherwise, the implementation tries to
// copy the data specified by the user, potentially limiting it by the
// internally-enforced buffer limits.
#define NTSU_SOCKETUTIL_LIMIT_MAX_BYTES_PER_RECEIVE 0

#if defined(BSLS_PLATFORM_OS_UNIX)

#if defined(IOV_MAX)
#define NTSU_SOCKETUTIL_MAX_BUFFERS_PER_SEND IOV_MAX
#else
#define NTSU_SOCKETUTIL_MAX_BUFFERS_PER_SEND                                  \
    NTSU_SOCKETUTIL_LIMIT_MAX_BUFFERS_PER_SEND
#endif

#if defined(IOV_MAX)
#define NTSU_SOCKETUTIL_MAX_BUFFERS_PER_RECEIVE IOV_MAX
#else
#define NTSU_SOCKETUTIL_MAX_BUFFERS_PER_RECEIVE                               \
    NTSU_SOCKETUTIL_LIMIT_MAX_BUFFERS_PER_RECEIVE
#endif

#if defined(BSLS_PLATFORM_OS_LINUX)
#if defined(UIO_MAXIOV)
#if (UIO_MAXIOV > NTSU_SOCKETUTIL_LIMIT_MAX_MESSAGES_PER_SEND)
#define NTSU_SOCKETUTIL_MAX_MESSAGES_PER_SEND                                 \
    NTSU_SOCKETUTIL_LIMIT_MAX_MESSAGES_PER_SEND
#else
#define NTSU_SOCKETUTIL_MAX_MESSAGES_PER_SEND UIO_MAXIOV
#endif
#else
#define NTSU_SOCKETUTIL_MAX_MESSAGES_PER_SEND                                 \
    NTSU_SOCKETUTIL_LIMIT_MAX_MESSAGES_PER_SEND
#endif

#if defined(UIO_MAXIOV)
#if (UIO_MAXIOV > NTSU_SOCKETUTIL_LIMIT_MAX_MESSAGES_PER_RECEIVE)
#define NTSU_SOCKETUTIL_MAX_MESSAGES_PER_RECEIVE                              \
    NTSU_SOCKETUTIL_LIMIT_MAX_MESSAGES_PER_RECEIVE
#else
#define NTSU_SOCKETUTIL_MAX_MESSAGES_PER_RECEIVE UIO_MAXIOV
#endif
#else
#define NTSU_SOCKETUTIL_MAX_MESSAGES_PER_RECEIVE                              \
    NTSU_SOCKETUTIL_LIMIT_MAX_MESSAGES_PER_RECEIVE
#endif
#else
#define NTSU_SOCKETUTIL_MAX_MESSAGES_PER_SEND 0
#define NTSU_SOCKETUTIL_MAX_MESSAGES_PER_RECEIVE 0
#endif

#define NTSU_SOCKETUTIL_MAX_HANDLES_PER_OUTGOING_CONTROLMSG 1
#define NTSU_SOCKETUTIL_MAX_HANDLES_PER_INCOMING_CONTROLMSG 1

BSLMF_ASSERT(sizeof(BloombergLP::ntsa::Handle) == sizeof(int));

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

#define NTSU_SOCKETUTIL_MAX_BUFFERS_PER_SEND                                  \
    NTSU_SOCKETUTIL_LIMIT_MAX_BUFFERS_PER_SEND
#define NTSU_SOCKETUTIL_MAX_BUFFERS_PER_RECEIVE                               \
    NTSU_SOCKETUTIL_LIMIT_MAX_BUFFERS_PER_RECEIVE
#define NTSU_SOCKETUTIL_MAX_MESSAGES_PER_SEND 0
#define NTSU_SOCKETUTIL_MAX_MESSAGES_PER_RECEIVE 0

#else
#error Not implemented
#endif

#if defined(BSLS_PLATFORM_CMP_CLANG)
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

// Comment or set to zero to disable maintaining a process-wide set of
// handles opened by this component.
#define NTSU_SOCKETUTIL_DEBUG_HANDLE_MAP 0

// Comment or set to 0 to disable logging the creation and closing of handles.
#define NTSU_SOCKETUTIL_DEBUG_LIFETIME 0

// Set to the system log macro to use when logging the creation and closing of
// handles.
#define NTSU_SOCKETUTIL_DEBUG_LIFETIME_LOG BSLS_LOG_WARN

// Comment or set to 0 to disable logging the parameters and results of
// sendmsg.
#define NTSU_SOCKETUTIL_DEBUG_SENDMSG 0

// Comment or set to 0 to disable logging the parameters and results of
// recvmsg.
#define NTSU_SOCKETUTIL_DEBUG_RECVMSG 0

namespace BloombergLP {
namespace ntsu {

namespace {

#if NTSU_SOCKETUTIL_DEBUG_HANDLE_MAP
bsls::SpinLock s_handleSetLock = BSLS_SPINLOCK_UNLOCKED;

typedef bsl::set<ntsa::Handle> HandleSet;
HandleSet                      s_handleSet(bslma::Default::globalAllocator());
#endif

#if NTSU_SOCKETUTIL_DEBUG_SENDMSG

// Describe the statistics measured for 'sendmsg'.
class OutoingDataStats
{
    bsls::AtomicUint64 d_numSyscalls;
    bsls::AtomicUint64 d_totalBuffersSendable;
    bsls::AtomicUint64 d_totalBytesSendable;
    bsls::AtomicUint64 d_totalBytesSent;
    bsls::AtomicUint64 d_numWouldBlocks;

  public:
    OutoingDataStats();

    ~OutoingDataStats();

    void update(const struct iovec* buffersSendable,
                bsl::size_t         numBuffersSendable,
                ssize_t             numBytesSent,
                int                 errorNumber);
};

OutoingDataStats s_outgoingDataStats;

OutoingDataStats::OutoingDataStats()
: d_numSyscalls(0)
, d_totalBuffersSendable(0)
, d_totalBytesSendable(0)
, d_totalBytesSent(0)
, d_numWouldBlocks(0)
{
}

OutoingDataStats::~OutoingDataStats()
{
    if (d_numSyscalls == 0) {
        return;
    }

    double avgBuffersSendablePerSyscall =
        static_cast<double>(d_totalBuffersSendable) /
        static_cast<double>(d_numSyscalls);

    double avgBytesSendablePerSyscall =
        static_cast<double>(d_totalBytesSendable) /
        static_cast<double>(d_numSyscalls);

    double avgBytesSentPerSyscall = static_cast<double>(d_totalBytesSent) /
                                    static_cast<double>(d_numSyscalls);

    char buffer[4096];

    bsl::sprintf(buffer,
                 "--\n"
                 "NTSU PID:                                   %-20d\n"
                 "NTSU Total sendmsg syscalls:                %-20zu\n"
                 "NTSU Total sendmsg EWOULDBLOCK results:     %-20zu\n"
                 "NTSU Average buffers sendable per syscall:  %-20.2f\n"
                 "NTSU Average bytes sendable per syscall:    %-20.2f\n"
                 "NTSU Average bytes sent per syscall:        %-20.2f\n",
                 (int)(::getpid()),
                 (bsl::size_t)(d_numSyscalls),
                 (bsl::size_t)(d_numWouldBlocks),
                 (double)(avgBuffersSendablePerSyscall),
                 (double)(avgBytesSendablePerSyscall),
                 (double)(avgBytesSentPerSyscall));

    bsl::fprintf(stdout, "%s", buffer);
    bsl::fflush(stdout);
}

void OutoingDataStats::update(const struct iovec* buffersSendable,
                              bsl::size_t         numBuffersSendable,
                              ssize_t             numBytesSent,
                              int                 errorNumber)
{
    bsl::size_t bytesSendable = 0;
    for (bsl::size_t i = 0; i < numBuffersSendable; ++i) {
        bytesSendable += static_cast<bsl::size_t>(buffersSendable[i].iov_len);
    }

    d_totalBuffersSendable.addRelaxed(numBuffersSendable);
    d_totalBytesSendable.addRelaxed(bytesSendable);

    if (numBytesSent > 0) {
        d_totalBytesSent.addRelaxed(static_cast<bsl::size_t>(numBytesSent));
    }

#if EWOULDBLOCK != EAGAIN
    if (errorNumber == EWOULDBLOCK || errorNumber == EAGAIN) {
        d_numWouldBlocks.addRelaxed(1);
    }
#else
    if (errorNumber == EWOULDBLOCK) {
        d_numWouldBlocks.addRelaxed(1);
    }
#endif

    d_numSyscalls.addRelaxed(1);
}

#define NTSU_SOCKETUTIL_DEBUG_SENDMSG_UPDATE(buffersSendable,                 \
                                             numBuffersSendable,              \
                                             numBytesSent,                    \
                                             errorNumber)                     \
    s_outgoingDataStats.update(buffersSendable,                               \
                               numBuffersSendable,                            \
                               numBytesSent,                                  \
                               errorNumber);

#else

#define NTSU_SOCKETUTIL_DEBUG_SENDMSG_UPDATE(buffersSendable,                 \
                                             numBuffersSendable,              \
                                             numBytesSent,                    \
                                             errorNumber)

#endif

#if NTSU_SOCKETUTIL_DEBUG_RECVMSG

// Describe the statistics measured for 'recvmsg'.
class IncomingDataStats
{
    bsls::AtomicUint64 d_numSyscalls;
    bsls::AtomicUint64 d_totalBuffersReceivable;
    bsls::AtomicUint64 d_totalBytesReceivable;
    bsls::AtomicUint64 d_totalBytesReceived;
    bsls::AtomicUint64 d_numWouldBlocks;

  public:
    IncomingDataStats();

    ~IncomingDataStats();

    void update(const struct iovec* buffersReceivable,
                bsl::size_t         numBuffersReceivable,
                ssize_t             numBytesSent,
                int                 errorNumber);
};

IncomingDataStats s_incomingDataStats;

IncomingDataStats::IncomingDataStats()
: d_numSyscalls(0)
, d_totalBuffersReceivable(0)
, d_totalBytesReceivable(0)
, d_totalBytesReceived(0)
, d_numWouldBlocks(0)
{
}

IncomingDataStats::~IncomingDataStats()
{
    if (d_numSyscalls == 0) {
        return;
    }

    double avgBuffersReceivablePerSyscall =
        static_cast<double>(d_totalBuffersReceivable) /
        static_cast<double>(d_numSyscalls);

    double avgBytesReceivablePerSyscall =
        static_cast<double>(d_totalBytesReceivable) /
        static_cast<double>(d_numSyscalls);

    double avgBytesReceivedPerSyscall =
        static_cast<double>(d_totalBytesReceived) /
        static_cast<double>(d_numSyscalls);

    char buffer[4096];

    bsl::sprintf(buffer,
                 "--\n"
                 "NTSU PID:                                    %-20d\n"
                 "NTSU Total recvmsg syscalls:                 %-20zu\n"
                 "NTSU Total recvmsg EWOULDBLOCK results:      %-20zu\n"
                 "NTSU Average buffers receivable per syscall: %-20.2f\n"
                 "NTSU Average bytes receivable per syscall:   %-20.2f\n"
                 "NTSU Average bytes received per syscall:     %-20.2f\n",
                 (int)(::getpid()),
                 (bsl::size_t)(d_numSyscalls),
                 (bsl::size_t)(d_numWouldBlocks),
                 (double)(avgBuffersReceivablePerSyscall),
                 (double)(avgBytesReceivablePerSyscall),
                 (double)(avgBytesReceivedPerSyscall));

    bsl::fprintf(stdout, "%s", buffer);
    bsl::fflush(stdout);
}

void IncomingDataStats::update(const struct iovec* buffersReceivable,
                               bsl::size_t         numBuffersReceivable,
                               ssize_t             numBytesReceived,
                               int                 errorNumber)
{
    bsl::size_t bytesReceivable = 0;
    for (bsl::size_t i = 0; i < numBuffersReceivable; ++i) {
        bytesReceivable +=
            static_cast<bsl::size_t>(buffersReceivable[i].iov_len);
    }

    d_totalBuffersReceivable.addRelaxed(numBuffersReceivable);
    d_totalBytesReceivable.addRelaxed(bytesReceivable);

    if (numBytesReceived > 0) {
        d_totalBytesReceived.addRelaxed(
            static_cast<bsl::size_t>(numBytesReceived));
    }

#if EWOULDBLOCK != EAGAIN
    if (errorNumber == EWOULDBLOCK || errorNumber == EAGAIN) {
        d_numWouldBlocks.addRelaxed(1);
    }
#else
    if (errorNumber == EWOULDBLOCK) {
        d_numWouldBlocks.addRelaxed(1);
    }
#endif

    d_numSyscalls.addRelaxed(1);
}

#define NTSU_SOCKETUTIL_DEBUG_RECVMSG_UPDATE(buffersReceivable,               \
                                             numBuffersReceivable,            \
                                             numBytesReceived,                \
                                             errorNumber)                     \
    s_incomingDataStats.update(buffersReceivable,                             \
                               numBuffersReceivable,                          \
                               numBytesReceived,                              \
                               errorNumber);

#else

#define NTSU_SOCKETUTIL_DEBUG_RECVMSG_UPDATE(buffersReceivable,               \
                                             numBuffersReceivable,            \
                                             numBytesReceived,                \
                                             errorNumber)

#endif

#if defined(BSLS_PLATFORM_OS_UNIX)

// Provide a buffer suitable for attaching to a call to 'sendmsg' to send
// control data to the peer of a socket.
class SendControl
{
    enum {
        // The payload size required to send any meta-data (viz. open file
        // descriptors) to the peer of a socket.
        k_SEND_CONTROL_PAYLOAD_SIZE = static_cast<int>(
            NTSU_SOCKETUTIL_MAX_HANDLES_PER_OUTGOING_CONTROLMSG *
            sizeof(ntsa::Handle)),

        // The control buffer capacity required to send any meta-data (viz.
        // open file descriptors) to the peer of a socket.
        k_SEND_CONTROL_BUFFER_SIZE =
            static_cast<int>(CMSG_SPACE(k_SEND_CONTROL_PAYLOAD_SIZE))
    };

    // Define a type alias for a maximimally-aligned buffer of suitable size to
    // send any meta-data (viz. open file descriptors) to the peer of the
    // socket.
    typedef bsls::AlignedBuffer<k_SEND_CONTROL_BUFFER_SIZE> Arena;

    Arena d_arena;

  private:
    SendControl(const SendControl&) BSLS_KEYWORD_DELETED;
    SendControl& operator=(const SendControl) BSLS_KEYWORD_DELETED;

  public:
    // Create a new send control buffer.
    SendControl();

    // Destroy this object.
    ~SendControl();

    // Encode the specified 'options' into the control buffer. Return the
    // error.
    ntsa::Error encode(msghdr* msg, const ntsa::SendOptions& options);
};

// Provide a buffer suitable for attaching to a call to 'recvmsg' to receive
// control data from the peer of a socket.
class ReceiveControl
{
    enum {
        // The payload size required to receive any meta-data  (e.g. open
        // file descriptors, timestamps, etc.) buffered by the operating system
        // for a socket.
        k_RECEIVE_CONTROL_PAYLOAD_SIZE =
            static_cast<int>(
                NTSU_SOCKETUTIL_MAX_HANDLES_PER_INCOMING_CONTROLMSG *
                sizeof(ntsa::Handle))
#if defined(BSLS_PLATFORM_OS_LINUX)
            + static_cast<int>(sizeof(TimestampUtil::ScmTimestamping))
#endif

        // The control buffer capacity required to receive any meta-data (e.g.
        // open file descriptors, timestamps, etc.) buffered by the operating
        // system for a socket.
        ,
        k_RECEIVE_CONTROL_BUFFER_SIZE =
            static_cast<int>(CMSG_SPACE(k_RECEIVE_CONTROL_PAYLOAD_SIZE))
    };

    // Define a type alias for a maximimally-aligned buffer of suitable size to
    // receive any meta-data (e.g. foreign handles, timestamps, etc.) buffered
    // by the operating system for a socket.
    typedef bsls::AlignedBuffer<k_RECEIVE_CONTROL_BUFFER_SIZE> Arena;

    Arena d_arena;

  private:
    ReceiveControl(const ReceiveControl&) BSLS_KEYWORD_DELETED;
    ReceiveControl& operator=(const ReceiveControl) BSLS_KEYWORD_DELETED;

  public:
    // Create a new receive control buffer.
    ReceiveControl();

    // Destroy this object.
    ~ReceiveControl();

    // Zero the control buffer.
    void initialize(msghdr* msg);

    // Decode the control buffer of specified 'msg' and decode its contents
    // into the specified 'context' according to the specified 'options'.
    ntsa::Error decode(ntsa::ReceiveContext*       context,
                       const msghdr&               msg,
                       const ntsa::ReceiveOptions& options);
};

NTSCFG_INLINE
SendControl::SendControl()
{
}

NTSCFG_INLINE
SendControl::~SendControl()
{
}

ntsa::Error SendControl::encode(msghdr* msg, const ntsa::SendOptions& options)
{
    if (options.foreignHandle().isNull()) {
        return ntsa::Error();
    }

    bsl::memset(d_arena.buffer(), 0, k_SEND_CONTROL_BUFFER_SIZE);

    msg->msg_control    = d_arena.buffer();
    msg->msg_controllen = static_cast<socklen_t>(k_SEND_CONTROL_BUFFER_SIZE);

    ntsa::Handle foreignHandle = options.foreignHandle().value();

    struct cmsghdr* ctl = CMSG_FIRSTHDR(msg);

    ctl->cmsg_level = SOL_SOCKET;
    ctl->cmsg_type  = SCM_RIGHTS;
    ctl->cmsg_len   = CMSG_LEN(sizeof foreignHandle);

    bsl::memcpy(CMSG_DATA(ctl), &foreignHandle, sizeof foreignHandle);

    return ntsa::Error();
}

NTSCFG_INLINE
ReceiveControl::ReceiveControl()
{
}

NTSCFG_INLINE
ReceiveControl::~ReceiveControl()
{
}

void ReceiveControl::initialize(msghdr* msg)
{
    bsl::memset(d_arena.buffer(), 0, k_RECEIVE_CONTROL_BUFFER_SIZE);

    msg->msg_control = d_arena.buffer();
    msg->msg_controllen =
        static_cast<socklen_t>(k_RECEIVE_CONTROL_BUFFER_SIZE);
}

ntsa::Error ReceiveControl::decode(ntsa::ReceiveContext*       context,
                                   const msghdr&               msg,
                                   const ntsa::ReceiveOptions& options)
{
    for (cmsghdr* hdr = CMSG_FIRSTHDR(&msg); hdr != 0;
         hdr          = CMSG_NXTHDR(const_cast<msghdr*>(&msg), hdr))
    {
        if (hdr->cmsg_level == SOL_SOCKET) {
            if (hdr->cmsg_type == SCM_RIGHTS) {
                ntsa::Handle fd = ntsa::k_INVALID_HANDLE;

                if (NTSCFG_UNLIKELY(hdr->cmsg_len != CMSG_LEN(sizeof fd))) {
                    BSLS_LOG_WARN("Ignoring received control block meta-data: "
                                  "Unexpected control message payload size: "
                                  "expected %d bytes, found %d bytes",
                                  (int)(CMSG_LEN(sizeof fd)),
                                  (int)(hdr->cmsg_len));
                    continue;
                }

                bsl::memcpy(&fd, CMSG_DATA(hdr), sizeof fd);

                if (options.wantForeignHandles()) {
                    context->setForeignHandle(fd);
                }
                else {
                    BSLS_LOG_WARN("Ignoring received control block meta-data: "
                                  "Unexpected foreign file descriptor");
                    ::close(fd);
                }
            }
#if defined(BSLS_PLATFORM_OS_LINUX)
            else if (hdr->cmsg_type == TimestampUtil::e_SCM_TIMESTAMPING) {
                TimestampUtil::ScmTimestamping ts;

                if (NTSCFG_UNLIKELY(hdr->cmsg_len != CMSG_LEN(sizeof ts))) {
                    BSLS_LOG_WARN("Ignoring received control block meta-data: "
                                  "Unexpected control message payload size: "
                                  "expected %d bytes, found %d bytes",
                                  (int)(CMSG_LEN(sizeof ts)),
                                  (int)(hdr->cmsg_len));
                    continue;
                }

                bsl::memcpy(&ts, CMSG_DATA(hdr), sizeof ts);

                if (ts.softwareTs.tv_sec || ts.softwareTs.tv_nsec) {
                    context->setSoftwareTimestamp(bsls::TimeInterval(
                        ts.softwareTs.tv_sec,
                        static_cast<int>(ts.softwareTs.tv_nsec)));
                }
                if (ts.hardwareTs.tv_sec || ts.hardwareTs.tv_nsec) {
                    context->setHardwareTimestamp(bsls::TimeInterval(
                        ts.hardwareTs.tv_sec,
                        static_cast<int>(ts.hardwareTs.tv_nsec)));
                }
            }
            else if (hdr->cmsg_type == TimestampUtil::e_SCM_TIMESTAMPNS) {
                TimestampUtil::Timespec ts;

                if (NTSCFG_UNLIKELY(hdr->cmsg_len != CMSG_LEN(sizeof ts))) {
                    BSLS_LOG_WARN("Ignoring received control block meta-data: "
                                  "Unexpected control message payload size: "
                                  "expected %d bytes, found %d bytes",
                                  (int)(CMSG_LEN(sizeof ts)),
                                  (int)(hdr->cmsg_len));
                    continue;
                }

                bsl::memcpy(&ts, CMSG_DATA(hdr), sizeof(ts));
                if (ts.tv_sec || ts.tv_nsec) {
                    context->setSoftwareTimestamp(
                        bsls::TimeInterval(ts.tv_sec,
                                           static_cast<int>(ts.tv_nsec)));
                }
            }
#endif
        }
    }

    return ntsa::Error();
}

#endif

/// Provide utilities for converting between 'ntsa::Endpoint'
/// the platform 'struct sockaddr_storage'.
struct SocketStorageUtil {
    /// Initialize the specified 'socketAddress'. Load into the specified
    /// 'socketAddressSize' the size of the resulting 'socketAddress'.
    static void initialize(sockaddr_storage* socketAddress,
                           socklen_t*        socketAddressSize);

    /// Load into the specified 'endpoint' the conversion of the specified
    /// 'socketAddress' having the specified 'socketAddressSize'. Return
    /// the error.
    static ntsa::Error convert(ntsa::Endpoint*         endpoint,
                               const sockaddr_storage* socketAddress,
                               socklen_t               socketAddressSize);

    /// Load into the specified 'socketAddress' and 'socketAddressSize' the
    /// conversion of the specified 'endpoint'. Return the error.
    static ntsa::Error convert(sockaddr_storage*     socketAddress,
                               socklen_t*            socketAddressSize,
                               const ntsa::Endpoint& endpoint);
};

void SocketStorageUtil::initialize(sockaddr_storage* socketAddress,
                                   socklen_t*        socketAddressSize)
{
    bsl::memset(socketAddress, 0, sizeof(sockaddr_storage));
    *socketAddressSize = static_cast<socklen_t>(sizeof(sockaddr_storage));
}

ntsa::Error SocketStorageUtil::convert(ntsa::Endpoint*         endpoint,
                                       const sockaddr_storage* socketAddress,
                                       socklen_t socketAddressSize)
{
    endpoint->reset();

    if (socketAddress->ss_family == AF_INET) {
        const sockaddr_in* socketAddressIpv4 =
            reinterpret_cast<const sockaddr_in*>(socketAddress);

        if (socketAddressIpv4->sin_family != AF_INET) {
            return ntsa::Error::invalid();
        }

        if (socketAddressSize != 0) {
            if (socketAddressSize != sizeof(sockaddr_in)) {
                return ntsa::Error::invalid();
            }

            ntsa::IpEndpoint* ipEndpoint = &endpoint->makeIp();

            ntsa::Ipv4Address ipv4Address;
            ipv4Address.copyFrom(&socketAddressIpv4->sin_addr,
                                 sizeof socketAddressIpv4->sin_addr);

            ipEndpoint->setHost(ipv4Address);
            ipEndpoint->setPort(ntohs(socketAddressIpv4->sin_port));
        }
        else {
            *endpoint =
                ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::any(), 0));
        }
    }
    else if (socketAddress->ss_family == AF_INET6) {
        const sockaddr_in6* socketAddressIpv6 =
            reinterpret_cast<const sockaddr_in6*>(socketAddress);

        if (socketAddressIpv6->sin6_family != AF_INET6) {
            return ntsa::Error::invalid();
        }

        if (socketAddressSize != 0) {
            if (socketAddressSize != sizeof(sockaddr_in6)) {
                return ntsa::Error::invalid();
            }

            ntsa::IpEndpoint* ipEndpoint = &endpoint->makeIp();

            ntsa::Ipv6Address ipv6Address;
            ipv6Address.copyFrom(&socketAddressIpv6->sin6_addr,
                                 sizeof socketAddressIpv6->sin6_addr);

            ipv6Address.setScopeId(socketAddressIpv6->sin6_scope_id);

            ipEndpoint->setHost(ipv6Address);
            ipEndpoint->setPort(ntohs(socketAddressIpv6->sin6_port));
        }
        else {
            *endpoint =
                ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::any(), 0));
        }
    }
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
    else if (socketAddress->ss_family == AF_UNIX) {
        const sockaddr_un* socketAddressLocal =
            reinterpret_cast<const sockaddr_un*>(socketAddress);

        if (socketAddressLocal->sun_family != AF_UNIX) {
            return ntsa::Error::invalid();
        }

        ntsa::LocalName* localName = &endpoint->makeLocal();

        if (socketAddressSize != 0) {
            const bsl::size_t pathOffset =
                offsetof(struct sockaddr_un, sun_path);

            if (socketAddressSize == pathOffset) {
                localName->setUnnamed();
            }
            else {
                const char* begin = socketAddressLocal->sun_path;

                if (*begin == 0) {
                    localName->setAbstract();
                    ++begin;
                }

                const char* end = begin;
                while (*end != 0) {
                    ++end;
                }

                if (begin == end) {
                    localName->setUnnamed();
                }
                else {
                    localName->setValue(bslstl::StringRef(begin, end));
                }
            }
        }
        else {
            localName->setUnnamed();
        }
    }
#endif
#if defined(BSLS_PLATFORM_OS_AIX) || defined(BSLS_PLATFORM_OS_SOLARIS)
    else if (socketAddress->ss_family == AF_UNSPEC) {
        // On AIX, getsockname on an fd returned by socketpair returns
        // successfully but does not modify the output socket address storage.
        // Assume this indicates an unnamed unix domain socket.  On Solaris,
        // getsockname on an fd that is has not had 'bind', 'connect', or
        // 'sendto' called on it returns successfully but does not modify
        // socket address storage. Assume this indicates an unnamed unix domain
        // socket.

        ntsa::LocalName* localName = &endpoint->makeLocal();
        localName->setUnnamed();
    }
#endif
    else {
        return ntsa::Error::invalid();
    }

    return ntsa::Error();
}

ntsa::Error SocketStorageUtil::convert(sockaddr_storage*     socketAddress,
                                       socklen_t*            socketAddressSize,
                                       const ntsa::Endpoint& endpoint)
{
    bsl::memset(socketAddress, 0, sizeof(sockaddr_storage));
    *socketAddressSize = 0;

    if (endpoint.isIp()) {
        if (endpoint.ip().host().isV4()) {
            sockaddr_in* socketAddressIpv4 =
                reinterpret_cast<sockaddr_in*>(socketAddress);

            *socketAddressSize = sizeof(sockaddr_in);

            socketAddressIpv4->sin_family = AF_INET;
            socketAddressIpv4->sin_port   = htons(endpoint.ip().port());

            endpoint.ip().host().v4().copyTo(
                &socketAddressIpv4->sin_addr,
                sizeof socketAddressIpv4->sin_addr);
        }
        else if (endpoint.ip().host().isV6()) {
            sockaddr_in6* socketAddressIpv6 =
                reinterpret_cast<sockaddr_in6*>(socketAddress);

            *socketAddressSize = sizeof(sockaddr_in6);

            socketAddressIpv6->sin6_family = AF_INET6;
            socketAddressIpv6->sin6_port   = htons(endpoint.ip().port());

            endpoint.ip().host().v6().copyTo(
                &socketAddressIpv6->sin6_addr,
                sizeof socketAddressIpv6->sin6_addr);

            socketAddressIpv6->sin6_scope_id =
                endpoint.ip().host().v6().scopeId();
        }
        else {
            return ntsa::Error::invalid();
        }
    }
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
    else if (endpoint.isLocal()) {
        sockaddr_un* socketAddressLocal =
            reinterpret_cast<sockaddr_un*>(socketAddress);

        socketAddressLocal->sun_family = AF_UNIX;
        *socketAddressSize = offsetof(struct sockaddr_un, sun_path);

        if (!endpoint.local().isUnnamed()) {
            bsl::size_t offset = 0;
            if (endpoint.local().isAbstract()) {
                socketAddressLocal->sun_path[offset]  = 0;
                *socketAddressSize                   += 1;
                ++offset;
            }

            BSLS_ASSERT_OPT(ntsa::LocalName::k_MAX_PATH_LENGTH <=
                            sizeof(sockaddr_un) -
                                offsetof(struct sockaddr_un, sun_path) -
                                offset);

            bslstl::StringRef value = endpoint.local().value();

            bsl::memcpy(&socketAddressLocal->sun_path[offset],
                        value.data(),
                        value.size());

            *socketAddressSize += static_cast<socklen_t>(value.size());

            if (!endpoint.local().isAbstract()) {
                // Non-abstract Unix domain socket names must be
                // null-terminated, and the null-terminator is included in the
                // length.
                offset                               += value.size();
                socketAddressLocal->sun_path[offset]  = 0;
                *socketAddressSize                   += 1;
            }
        }
    }
#endif
    else {
        return ntsa::Error::invalid();
    }

    return ntsa::Error();
}

}  // close unnamed namespace

#if defined(BSLS_PLATFORM_OS_UNIX)

// Cast 'variable' to for suitable assignment to the 'msg_iov' field of
// 'struct msghdr' as defined on the current platform.
#define NTSU_SOCKETUTIL_MSG_IOV_BUF(variable)                                 \
    const_cast<struct iovec*>(reinterpret_cast<const struct iovec*>(variable))

// Cast 'variable' to for suitable assignment to the 'msg_iovlen' field of
// 'struct msghdr' as defined on the current platform.
#if defined(BSLS_PLATFORM_OS_LINUX)
#define NTSU_SOCKETUTIL_MSG_IOV_LEN(variable)                                 \
    static_cast<bsl::size_t>(variable)
#else
#define NTSU_SOCKETUTIL_MSG_IOV_LEN(variable) static_cast<int>(variable)
#endif

ntsa::Error SocketUtil::create(ntsa::Handle*          result,
                               ntsa::Transport::Value type)
{
    int rc;
    int domain;
    int mode;
    int protocol;

    if (type == ntsa::Transport::e_TCP_IPV4_STREAM) {
        domain   = AF_INET;
        mode     = SOCK_STREAM;
        protocol = IPPROTO_TCP;
    }
    else if (type == ntsa::Transport::e_TCP_IPV6_STREAM) {
        domain   = AF_INET6;
        mode     = SOCK_STREAM;
        protocol = IPPROTO_TCP;
    }
    else if (type == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
        domain   = AF_INET;
        mode     = SOCK_DGRAM;
        protocol = IPPROTO_UDP;
    }
    else if (type == ntsa::Transport::e_UDP_IPV6_DATAGRAM) {
        domain   = AF_INET6;
        mode     = SOCK_DGRAM;
        protocol = IPPROTO_UDP;
    }
    else if (type == ntsa::Transport::e_LOCAL_STREAM) {
        domain   = AF_UNIX;
        mode     = SOCK_STREAM;
        protocol = 0;
    }
    else if (type == ntsa::Transport::e_LOCAL_DATAGRAM) {
        domain   = AF_UNIX;
        mode     = SOCK_DGRAM;
        protocol = 0;
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

#if defined(BSLS_PLATFORM_OS_LINUX)
    mode |= SOCK_CLOEXEC;
#endif

    *result = ::socket(domain, mode, protocol);
    if (*result == -1) {
        return ntsa::Error(errno);
    }

#if !defined(BSLS_PLATFORM_OS_LINUX)
    rc = fcntl(*result, FD_CLOEXEC);
    if (rc != 0) {
        return ntsa::Error(errno);
    }
#endif

    if (domain == AF_INET6) {
        int optionValue = 1;
        rc = ::setsockopt(*result,
                          IPPROTO_IPV6,
                          IPV6_V6ONLY,
                          &optionValue,
                          sizeof optionValue);
        if (rc != 0) {
            return ntsa::Error(errno);
        }
    }

#if NTSU_SOCKETUTIL_DEBUG_LIFETIME
    NTSU_SOCKETUTIL_DEBUG_LIFETIME_LOG("Socket handle %d created", *result);
#endif

#if NTSU_SOCKETUTIL_DEBUG_HANDLE_MAP
    {
        bsls::SpinLockGuard guard(&s_handleSetLock);
        s_handleSet.insert(*result);
    }
#endif

    return ntsa::Error();
}

ntsa::Error SocketUtil::duplicate(ntsa::Handle* result, ntsa::Handle socket)
{
    ntsa::Handle duplicateSocket = ::dup(socket);
    if (duplicateSocket == -1) {
        return ntsa::Error(errno);
    }

    *result = duplicateSocket;
    return ntsa::Error();
}

ntsa::Error SocketUtil::bind(const ntsa::Endpoint& endpoint,
                             bool                  reuseAddress,
                             ntsa::Handle          socket)
{
    ntsa::Error error;

    error = ntsu::SocketOptionUtil::setReuseAddress(socket, reuseAddress);
    if (error) {
        return error;
    }

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    error = SocketStorageUtil::convert(&socketAddress,
                                       &socketAddressSize,
                                       endpoint);
    if (error) {
        return error;
    }

    if (endpoint.isIp() && endpoint.ip().host().isV6()) {
        sockaddr_in6* socketAddressIpv6 =
            reinterpret_cast<sockaddr_in6*>(&socketAddress);
        if (socketAddressIpv6->sin6_scope_id == 0) {
            socketAddressIpv6->sin6_scope_id =
                ntsu::AdapterUtil::discoverScopeId(endpoint.ip().host().v6());
        }
    }

#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
    if (endpoint.isLocal() && !reuseAddress) {
        if (!endpoint.local().isUnnamed() && !endpoint.local().isAbstract()) {
            bsl::string path = endpoint.local().value();
            int         rc   = ::unlink(path.c_str());
            if (rc != 0 && errno != ENOENT) {
                ntsa::Error error(errno);
                BSLS_LOG_WARN("Failed to unlink %s: %s",
                              path.c_str(),
                              error.text().c_str());
            }
        }
    }
#endif

    int rc = ::bind(socket,
                    reinterpret_cast<const sockaddr*>(&socketAddress),
                    socketAddressSize);

    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::bindAny(ntsa::Transport::Value type,
                                bool                   reuseAddress,
                                ntsa::Handle           socket)
{
    ntsa::Error error;

    ntsa::Endpoint endpoint;

    if (type == ntsa::Transport::e_TCP_IPV4_STREAM) {
        endpoint =
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::IpEndpoint::anyIpv4Address(),
                                            ntsa::IpEndpoint::anyPort()));
    }
    else if (type == ntsa::Transport::e_TCP_IPV6_STREAM) {
        endpoint =
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::IpEndpoint::anyIpv6Address(),
                                            ntsa::IpEndpoint::anyPort()));
    }
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
    else if (type == ntsa::Transport::e_LOCAL_STREAM) {
        ntsa::LocalName   localName;
        const ntsa::Error error = ntsa::LocalName::generateUnique(&localName);
        if (error) {
            return error;
        }
        endpoint.makeLocal(localName);
    }
#endif
    else if (type == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
        endpoint =
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::IpEndpoint::anyIpv4Address(),
                                            ntsa::IpEndpoint::anyPort()));
    }
    else if (type == ntsa::Transport::e_UDP_IPV6_DATAGRAM) {
        endpoint =
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::IpEndpoint::anyIpv6Address(),
                                            ntsa::IpEndpoint::anyPort()));
    }
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
    else if (type == ntsa::Transport::e_LOCAL_DATAGRAM) {
        ntsa::LocalName   localName;
        const ntsa::Error error = ntsa::LocalName::generateUnique(&localName);
        if (error) {
            return error;
        }
        endpoint.makeLocal(localName);
    }
#endif
    else {
        return ntsa::Error::invalid();
    }

    error = ntsu::SocketUtil::bind(endpoint, reuseAddress, socket);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::listen(bsl::size_t backlog, ntsa::Handle socket)
{
    int effectiveBacklog;

    if (backlog == 0) {
        effectiveBacklog = 65535;
    }
    else {
        effectiveBacklog = static_cast<int>(backlog);
    }

    int rc = ::listen(socket, effectiveBacklog);
    if (rc != 0) {
        return ntsa::Error(errno);
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::connect(const ntsa::Endpoint& endpoint,
                                ntsa::Handle          socket)
{
    ntsa::Error error;
    int         rc;

    sockaddr_storage remoteSocketAddress;
    socklen_t        remoteSocketAddressSize;

    error = SocketStorageUtil::convert(&remoteSocketAddress,
                                       &remoteSocketAddressSize,
                                       endpoint);
    if (error) {
        return error;
    }

    rc = ::connect(socket,
                   reinterpret_cast<sockaddr*>(&remoteSocketAddress),
                   remoteSocketAddressSize);

    int lastError = errno;

    if ((rc == 0) || (lastError == EINPROGRESS || lastError == EWOULDBLOCK ||
                      lastError == EAGAIN))
    {
        // Unlink the file name for explicitly bound, Unix domain sockets,
        // if necessary.
        ntsu::SocketUtil::unlink(socket);
    }

    if (rc != 0) {
        return ntsa::Error(lastError);
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::accept(ntsa::Handle* result, ntsa::Handle socket)
{
    do {
#if defined(BSLS_PLATFORM_OS_LINUX)
        *result = ::accept4(socket, 0, 0, SOCK_CLOEXEC);
#else
        *result = ::accept(socket, 0, 0);
#endif
    } while ((*result == -1) && (errno == EINTR));

    if (*result == -1) {
        return ntsa::Error(errno);
    }

#if !defined(BSLS_PLATFORM_OS_LINUX)
    int rc = fcntl(*result, FD_CLOEXEC);
    if (rc != 0) {
        return ntsa::Error(errno);
    }
#endif

#if NTSU_SOCKETUTIL_DEBUG_LIFETIME
    NTSU_SOCKETUTIL_DEBUG_LIFETIME_LOG("Socket handle %d accepted", *result);
#endif

#if NTSU_SOCKETUTIL_DEBUG_HANDLE_MAP
    {
        bsls::SpinLockGuard guard(&s_handleSetLock);
        s_handleSet.insert(*result);
    }
#endif

    return ntsa::Error();
}

ntsa::Error SocketUtil::send(ntsa::SendContext*       context,
                             const void*              data,
                             bsl::size_t              size,
                             const ntsa::SendOptions& options,
                             ntsa::Handle             socket)
{
    ntsa::Error error;

    context->reset();

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyMetaData = !options.foreignHandle().isNull();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        error = SocketStorageUtil::convert(&socketAddress,
                                           &socketAddressSize,
                                           options.endpoint().value());
        if (error) {
            return error;
        }

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    SendControl control;
    if (specifyMetaData) {
        error = control.encode(&msg, options);
        if (error) {
            return error;
        }
    }

    struct iovec iovec;
    iovec.iov_base = const_cast<void*>(data);
    iovec.iov_len  = size;

    msg.msg_iov    = &iovec;
    msg.msg_iovlen = 1;

    context->setBytesSendable(size);

    int sendFlags = NTSU_SOCKETUTIL_SENDMSG_FLAGS;

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (options.zeroCopy()) {
        sendFlags |= ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
    }
#endif

    ssize_t sendmsgResult = ::sendmsg(socket, &msg, sendFlags);

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (NTSCFG_UNLIKELY(sendmsgResult < 0 &&
                        errno == ENOBUFS &&
                        options.zeroCopy()))
    {
        sendFlags &= ~ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
        sendmsgResult = ::sendmsg(socket, &msg, sendFlags);
    }
#endif

    NTSU_SOCKETUTIL_DEBUG_SENDMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         sendmsgResult,
                                         errno);

    if (sendmsgResult < 0) {
        return ntsa::Error(errno);
    }

#if defined(BSLS_PLATFORM_OS_LINUX)
    if ((sendFlags & ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY) != 0) {
        context->setZeroCopy(true);
    }
#endif

    context->setBytesSent(sendmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::send(ntsa::SendContext*       context,
                             const ntsa::ConstBuffer& buffer,
                             const ntsa::SendOptions& options,
                             ntsa::Handle             socket)
{
    ntsa::Error error;

    context->reset();

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyMetaData = !options.foreignHandle().isNull();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        error = SocketStorageUtil::convert(&socketAddress,
                                           &socketAddressSize,
                                           options.endpoint().value());
        if (error) {
            return error;
        }

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    SendControl control;
    if (specifyMetaData) {
        error = control.encode(&msg, options);
        if (error) {
            return error;
        }
    }

    const void*       data = buffer.data();
    const bsl::size_t size = buffer.size();

    struct iovec iovec;
    iovec.iov_base = const_cast<void*>(data);
    iovec.iov_len  = size;

    msg.msg_iov    = &iovec;
    msg.msg_iovlen = 1;

    context->setBytesSendable(size);

    int sendFlags = NTSU_SOCKETUTIL_SENDMSG_FLAGS;

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (options.zeroCopy()) {
        sendFlags |= ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
    }
#endif

    ssize_t sendmsgResult = ::sendmsg(socket, &msg, sendFlags);

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (NTSCFG_UNLIKELY(sendmsgResult < 0 &&
                        errno == ENOBUFS &&
                        options.zeroCopy()))
    {
        sendFlags &= ~ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
        sendmsgResult = ::sendmsg(socket, &msg, sendFlags);
    }
#endif

    NTSU_SOCKETUTIL_DEBUG_SENDMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         sendmsgResult,
                                         errno);

    if (sendmsgResult < 0) {
        return ntsa::Error(errno);
    }

#if defined(BSLS_PLATFORM_OS_LINUX)
    if ((sendFlags & ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY) != 0) {
        context->setZeroCopy(true);
    }
#endif

    context->setBytesSent(sendmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::send(ntsa::SendContext*       context,
                             const ntsa::ConstBuffer* bufferArray,
                             bsl::size_t              bufferCount,
                             const ntsa::SendOptions& options,
                             ntsa::Handle             socket)
{
    ntsa::Error error;

    context->reset();

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyMetaData = !options.foreignHandle().isNull();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        error = SocketStorageUtil::convert(&socketAddress,
                                           &socketAddressSize,
                                           options.endpoint().value());
        if (error) {
            return error;
        }

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    SendControl control;
    if (specifyMetaData) {
        error = control.encode(&msg, options);
        if (error) {
            return error;
        }
    }

    const bsl::size_t numBytesTotal =
        ntsa::ConstBuffer::totalSize(bufferArray, bufferCount);

    msg.msg_iov    = NTSU_SOCKETUTIL_MSG_IOV_BUF(bufferArray);
    msg.msg_iovlen = NTSU_SOCKETUTIL_MSG_IOV_LEN(bufferCount);

    context->setBytesSendable(numBytesTotal);

    int sendFlags = NTSU_SOCKETUTIL_SENDMSG_FLAGS;

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (options.zeroCopy()) {
        sendFlags |= ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
    }
#endif

    ssize_t sendmsgResult = ::sendmsg(socket, &msg, sendFlags);

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (NTSCFG_UNLIKELY(sendmsgResult < 0 &&
                        errno == ENOBUFS &&
                        options.zeroCopy()))
    {
        sendFlags &= ~ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
        sendmsgResult = ::sendmsg(socket, &msg, sendFlags);
    }
#endif

    NTSU_SOCKETUTIL_DEBUG_SENDMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         sendmsgResult,
                                         errno);

    if (sendmsgResult < 0) {
        return ntsa::Error(errno);
    }

#if defined(BSLS_PLATFORM_OS_LINUX)
    if ((sendFlags & ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY) != 0) {
        context->setZeroCopy(true);
    }
#endif

    context->setBytesSent(sendmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::send(ntsa::SendContext*            context,
                             const ntsa::ConstBufferArray& bufferArray,
                             const ntsa::SendOptions&      options,
                             ntsa::Handle                  socket)
{
    ntsa::Error error;

    context->reset();

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyMetaData = !options.foreignHandle().isNull();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        error = SocketStorageUtil::convert(&socketAddress,
                                           &socketAddressSize,
                                           options.endpoint().value());
        if (error) {
            return error;
        }

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    SendControl control;
    if (specifyMetaData) {
        error = control.encode(&msg, options);
        if (error) {
            return error;
        }
    }

    const bsl::size_t numBuffersTotal = bufferArray.numBuffers();
    const bsl::size_t numBytesTotal   = bufferArray.numBytes();

    msg.msg_iov    = NTSU_SOCKETUTIL_MSG_IOV_BUF(bufferArray.base());
    msg.msg_iovlen = NTSU_SOCKETUTIL_MSG_IOV_LEN(numBuffersTotal);

    context->setBytesSendable(numBytesTotal);

    int sendFlags = NTSU_SOCKETUTIL_SENDMSG_FLAGS;

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (options.zeroCopy()) {
        sendFlags |= ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
    }
#endif

    ssize_t sendmsgResult = ::sendmsg(socket, &msg, sendFlags);

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (NTSCFG_UNLIKELY(sendmsgResult < 0 &&
                        errno == ENOBUFS &&
                        options.zeroCopy()))
    {
        sendFlags &= ~ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
        sendmsgResult = ::sendmsg(socket, &msg, sendFlags);
    }
#endif

    NTSU_SOCKETUTIL_DEBUG_SENDMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         sendmsgResult,
                                         errno);

    if (sendmsgResult < 0) {
        return ntsa::Error(errno);
    }

#if defined(BSLS_PLATFORM_OS_LINUX)
    if ((sendFlags & ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY) != 0) {
        context->setZeroCopy(true);
    }
#endif

    context->setBytesSent(sendmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::send(ntsa::SendContext*               context,
                             const ntsa::ConstBufferPtrArray& bufferArray,
                             const ntsa::SendOptions&         options,
                             ntsa::Handle                     socket)
{
    ntsa::Error error;

    context->reset();

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyMetaData = !options.foreignHandle().isNull();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        error = SocketStorageUtil::convert(&socketAddress,
                                           &socketAddressSize,
                                           options.endpoint().value());
        if (error) {
            return error;
        }

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    SendControl control;
    if (specifyMetaData) {
        error = control.encode(&msg, options);
        if (error) {
            return error;
        }
    }

    const bsl::size_t numBuffersTotal = bufferArray.numBuffers();
    const bsl::size_t numBytesTotal   = bufferArray.numBytes();

    msg.msg_iov    = NTSU_SOCKETUTIL_MSG_IOV_BUF(bufferArray.base());
    msg.msg_iovlen = NTSU_SOCKETUTIL_MSG_IOV_LEN(numBuffersTotal);

    context->setBytesSendable(numBytesTotal);

    int sendFlags = NTSU_SOCKETUTIL_SENDMSG_FLAGS;

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (options.zeroCopy()) {
        sendFlags |= ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
    }
#endif

    ssize_t sendmsgResult = ::sendmsg(socket, &msg, sendFlags);

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (NTSCFG_UNLIKELY(sendmsgResult < 0 &&
                        errno == ENOBUFS &&
                        options.zeroCopy()))
    {
        sendFlags &= ~ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
        sendmsgResult = ::sendmsg(socket, &msg, sendFlags);
    }
#endif

    NTSU_SOCKETUTIL_DEBUG_SENDMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         sendmsgResult,
                                         errno);

    if (sendmsgResult < 0) {
        return ntsa::Error(errno);
    }

#if defined(BSLS_PLATFORM_OS_LINUX)
    if ((sendFlags & ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY) != 0) {
        context->setZeroCopy(true);
    }
#endif

    context->setBytesSent(sendmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::send(ntsa::SendContext*         context,
                             const ntsa::MutableBuffer& buffer,
                             const ntsa::SendOptions&   options,
                             ntsa::Handle               socket)
{
    ntsa::Error error;

    context->reset();

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyMetaData = !options.foreignHandle().isNull();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        error = SocketStorageUtil::convert(&socketAddress,
                                           &socketAddressSize,
                                           options.endpoint().value());
        if (error) {
            return error;
        }

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    SendControl control;
    if (specifyMetaData) {
        error = control.encode(&msg, options);
        if (error) {
            return error;
        }
    }

    void*             data = buffer.data();
    const bsl::size_t size = buffer.size();

    struct iovec iovec;
    iovec.iov_base = data;
    iovec.iov_len  = size;

    msg.msg_iov    = &iovec;
    msg.msg_iovlen = 1;

    context->setBytesSendable(size);

    int sendFlags = NTSU_SOCKETUTIL_SENDMSG_FLAGS;

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (options.zeroCopy()) {
        sendFlags |= ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
    }
#endif

    ssize_t sendmsgResult = ::sendmsg(socket, &msg, sendFlags);

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (NTSCFG_UNLIKELY(sendmsgResult < 0 &&
                        errno == ENOBUFS &&
                        options.zeroCopy()))
    {
        sendFlags &= ~ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
        sendmsgResult = ::sendmsg(socket, &msg, sendFlags);
    }
#endif

    NTSU_SOCKETUTIL_DEBUG_SENDMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         sendmsgResult,
                                         errno);

    if (sendmsgResult < 0) {
        return ntsa::Error(errno);
    }

#if defined(BSLS_PLATFORM_OS_LINUX)
    if ((sendFlags & ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY) != 0) {
        context->setZeroCopy(true);
    }
#endif

    context->setBytesSent(sendmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::send(ntsa::SendContext*         context,
                             const ntsa::MutableBuffer* bufferArray,
                             bsl::size_t                bufferCount,
                             const ntsa::SendOptions&   options,
                             ntsa::Handle               socket)
{
    ntsa::Error error;

    context->reset();

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyMetaData = !options.foreignHandle().isNull();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        error = SocketStorageUtil::convert(&socketAddress,
                                           &socketAddressSize,
                                           options.endpoint().value());
        if (error) {
            return error;
        }

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    SendControl control;
    if (specifyMetaData) {
        error = control.encode(&msg, options);
        if (error) {
            return error;
        }
    }

    const bsl::size_t numBytesTotal =
        ntsa::MutableBuffer::totalSize(bufferArray, bufferCount);

    msg.msg_iov    = NTSU_SOCKETUTIL_MSG_IOV_BUF(bufferArray);
    msg.msg_iovlen = NTSU_SOCKETUTIL_MSG_IOV_LEN(bufferCount);

    context->setBytesSendable(numBytesTotal);

    int sendFlags = NTSU_SOCKETUTIL_SENDMSG_FLAGS;

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (options.zeroCopy()) {
        sendFlags |= ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
    }
#endif

    ssize_t sendmsgResult = ::sendmsg(socket, &msg, sendFlags);

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (NTSCFG_UNLIKELY(sendmsgResult < 0 &&
                        errno == ENOBUFS &&
                        options.zeroCopy()))
    {
        sendFlags &= ~ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
        sendmsgResult = ::sendmsg(socket, &msg, sendFlags);
    }
#endif

    NTSU_SOCKETUTIL_DEBUG_SENDMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         sendmsgResult,
                                         errno);

    if (sendmsgResult < 0) {
        return ntsa::Error(errno);
    }

#if defined(BSLS_PLATFORM_OS_LINUX)
    if ((sendFlags & ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY) != 0) {
        context->setZeroCopy(true);
    }
#endif

    context->setBytesSent(sendmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::send(ntsa::SendContext*              context,
                             const ntsa::MutableBufferArray& bufferArray,
                             const ntsa::SendOptions&        options,
                             ntsa::Handle                    socket)
{
    ntsa::Error error;

    context->reset();

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyMetaData = !options.foreignHandle().isNull();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        error = SocketStorageUtil::convert(&socketAddress,
                                           &socketAddressSize,
                                           options.endpoint().value());
        if (error) {
            return error;
        }

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    SendControl control;
    if (specifyMetaData) {
        error = control.encode(&msg, options);
        if (error) {
            return error;
        }
    }

    const bsl::size_t numBuffersTotal = bufferArray.numBuffers();
    const bsl::size_t numBytesTotal   = bufferArray.numBytes();

    msg.msg_iov    = NTSU_SOCKETUTIL_MSG_IOV_BUF(bufferArray.base());
    msg.msg_iovlen = NTSU_SOCKETUTIL_MSG_IOV_LEN(numBuffersTotal);

    context->setBytesSendable(numBytesTotal);

    int sendFlags = NTSU_SOCKETUTIL_SENDMSG_FLAGS;

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (options.zeroCopy()) {
        sendFlags |= ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
    }
#endif

    ssize_t sendmsgResult = ::sendmsg(socket, &msg, sendFlags);

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (NTSCFG_UNLIKELY(sendmsgResult < 0 &&
                        errno == ENOBUFS &&
                        options.zeroCopy()))
    {
        sendFlags &= ~ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
        sendmsgResult = ::sendmsg(socket, &msg, sendFlags);
    }
#endif

    NTSU_SOCKETUTIL_DEBUG_SENDMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         sendmsgResult,
                                         errno);

    if (sendmsgResult < 0) {
        return ntsa::Error(errno);
    }

#if defined(BSLS_PLATFORM_OS_LINUX)
    if ((sendFlags & ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY) != 0) {
        context->setZeroCopy(true);
    }
#endif

    context->setBytesSent(sendmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::send(ntsa::SendContext*                 context,
                             const ntsa::MutableBufferPtrArray& bufferArray,
                             const ntsa::SendOptions&           options,
                             ntsa::Handle                       socket)
{
    ntsa::Error error;

    context->reset();

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyMetaData = !options.foreignHandle().isNull();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        error = SocketStorageUtil::convert(&socketAddress,
                                           &socketAddressSize,
                                           options.endpoint().value());
        if (error) {
            return error;
        }

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    SendControl control;
    if (specifyMetaData) {
        error = control.encode(&msg, options);
        if (error) {
            return error;
        }
    }

    const bsl::size_t numBuffersTotal = bufferArray.numBuffers();
    const bsl::size_t numBytesTotal   = bufferArray.numBytes();

    msg.msg_iov    = NTSU_SOCKETUTIL_MSG_IOV_BUF(bufferArray.base());
    msg.msg_iovlen = NTSU_SOCKETUTIL_MSG_IOV_LEN(numBuffersTotal);

    context->setBytesSendable(numBytesTotal);
    int sendFlags = NTSU_SOCKETUTIL_SENDMSG_FLAGS;

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (options.zeroCopy()) {
        sendFlags |= ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
    }
#endif

    ssize_t sendmsgResult = ::sendmsg(socket, &msg, sendFlags);

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (NTSCFG_UNLIKELY(sendmsgResult < 0 &&
                        errno == ENOBUFS &&
                        options.zeroCopy()))
    {
        sendFlags &= ~ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
        sendmsgResult = ::sendmsg(socket, &msg, sendFlags);
    }
#endif

    NTSU_SOCKETUTIL_DEBUG_SENDMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         sendmsgResult,
                                         errno);

    if (sendmsgResult < 0) {
        return ntsa::Error(errno);
    }

#if defined(BSLS_PLATFORM_OS_LINUX)
    if ((sendFlags & ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY) != 0) {
        context->setZeroCopy(true);
    }
#endif

    context->setBytesSent(sendmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::send(ntsa::SendContext*       context,
                             const bslstl::StringRef& string,
                             const ntsa::SendOptions& options,
                             ntsa::Handle             socket)
{
    ntsa::Error error;

    context->reset();

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyMetaData = !options.foreignHandle().isNull();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        error = SocketStorageUtil::convert(&socketAddress,
                                           &socketAddressSize,
                                           options.endpoint().value());
        if (error) {
            return error;
        }

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    SendControl control;
    if (specifyMetaData) {
        error = control.encode(&msg, options);
        if (error) {
            return error;
        }
    }

    const void*       data = string.data();
    const bsl::size_t size = string.size();

    struct iovec iovec;
    iovec.iov_base = const_cast<void*>(data);
    iovec.iov_len  = size;

    msg.msg_iov    = &iovec;
    msg.msg_iovlen = 1;

    context->setBytesSendable(size);
    int sendFlags = NTSU_SOCKETUTIL_SENDMSG_FLAGS;

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (options.zeroCopy()) {
        sendFlags |= ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
    }
#endif

    ssize_t sendmsgResult = ::sendmsg(socket, &msg, sendFlags);

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (NTSCFG_UNLIKELY(sendmsgResult < 0 &&
                        errno == ENOBUFS &&
                        options.zeroCopy()))
    {
        sendFlags &= ~ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
        sendmsgResult = ::sendmsg(socket, &msg, sendFlags);
    }
#endif

    NTSU_SOCKETUTIL_DEBUG_SENDMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         sendmsgResult,
                                         errno);

    if (sendmsgResult < 0) {
        return ntsa::Error(errno);
    }

#if defined(BSLS_PLATFORM_OS_LINUX)
    if ((sendFlags & ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY) != 0) {
        context->setZeroCopy(true);
    }
#endif

    context->setBytesSent(sendmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::send(ntsa::SendContext*       context,
                             const bdlbb::Blob&       blob,
                             const ntsa::SendOptions& options,
                             ntsa::Handle             socket)
{
    ntsa::Error error;

    context->reset();

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyMetaData = !options.foreignHandle().isNull();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        error = SocketStorageUtil::convert(&socketAddress,
                                           &socketAddressSize,
                                           options.endpoint().value());
        if (error) {
            return error;
        }

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    SendControl control;
    if (specifyMetaData) {
        error = control.encode(&msg, options);
        if (error) {
            return error;
        }
    }

    struct iovec iovecArray[NTSU_SOCKETUTIL_MAX_BUFFERS_PER_SEND];

    if (blob.numDataBuffers() == 1) {
        void*             data = blob.buffer(0).data();
        const bsl::size_t size = blob.lastDataBufferLength();

        iovecArray[0].iov_base = data;
        iovecArray[0].iov_len  = size;

        msg.msg_iov    = iovecArray;
        msg.msg_iovlen = 1;

        context->setBytesSendable(size);
    }
    else {
        bsl::size_t numBytesMax = options.maxBytes();
        if (numBytesMax == 0) {
            numBytesMax = SocketUtil::maxBytesPerSend(socket);
        }

        bsl::size_t numBuffersMax = options.maxBuffers();
        if (numBuffersMax == 0) {
            numBuffersMax = NTSU_SOCKETUTIL_MAX_BUFFERS_PER_SEND;
        }
        else if (numBuffersMax > NTSU_SOCKETUTIL_MAX_BUFFERS_PER_SEND) {
            numBuffersMax = NTSU_SOCKETUTIL_MAX_BUFFERS_PER_SEND;
        }

        bsl::size_t numBuffersTotal;
        bsl::size_t numBytesTotal;

        ntsu::BufferUtil::gather(
            &numBuffersTotal,
            &numBytesTotal,
            reinterpret_cast<ntsa::ConstBuffer*>(iovecArray),
            numBuffersMax,
            blob,
            numBytesMax);

        msg.msg_iov    = iovecArray;
        msg.msg_iovlen = NTSU_SOCKETUTIL_MSG_IOV_LEN(numBuffersTotal);

        context->setBytesSendable(numBytesTotal);
    }

    int sendFlags = NTSU_SOCKETUTIL_SENDMSG_FLAGS;

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (options.zeroCopy()) {
        sendFlags |= ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
    }
#endif

    ssize_t sendmsgResult = ::sendmsg(socket, &msg, sendFlags);

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (NTSCFG_UNLIKELY(sendmsgResult < 0 &&
                        errno == ENOBUFS &&
                        options.zeroCopy()))
    {
        sendFlags &= ~ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
        sendmsgResult = ::sendmsg(socket, &msg, sendFlags);
    }
#endif

    NTSU_SOCKETUTIL_DEBUG_SENDMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         sendmsgResult,
                                         errno);

    if (sendmsgResult < 0) {
        return ntsa::Error(errno);
    }

#if defined(BSLS_PLATFORM_OS_LINUX)
    if ((sendFlags & ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY) != 0) {
        context->setZeroCopy(true);
    }
#endif

    context->setBytesSent(sendmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::send(ntsa::SendContext*       context,
                             const bdlbb::BlobBuffer& blobBuffer,
                             const ntsa::SendOptions& options,
                             ntsa::Handle             socket)
{
    ntsa::Error error;

    context->reset();

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyMetaData = !options.foreignHandle().isNull();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    void*       data = blobBuffer.data();
    bsl::size_t size = NTSCFG_WARNING_PROMOTE(bsl::size_t, blobBuffer.size());

    struct iovec iovec;
    iovec.iov_base = data;
    iovec.iov_len  = size;

    msg.msg_iov    = &iovec;
    msg.msg_iovlen = 1;

    context->setBytesSendable(size);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        error = SocketStorageUtil::convert(&socketAddress,
                                           &socketAddressSize,
                                           options.endpoint().value());
        if (error) {
            return error;
        }

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    SendControl control;
    if (specifyMetaData) {
        error = control.encode(&msg, options);
        if (error) {
            return error;
        }
    }

    int sendFlags = NTSU_SOCKETUTIL_SENDMSG_FLAGS;

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (options.zeroCopy()) {
        sendFlags |= ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
    }
#endif

    ssize_t sendmsgResult = ::sendmsg(socket, &msg, sendFlags);

#if defined(BSLS_PLATFORM_OS_LINUX)
    if (NTSCFG_UNLIKELY(sendmsgResult < 0 &&
                        errno == ENOBUFS &&
                        options.zeroCopy()))
    {
        sendFlags &= ~ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY;
        sendmsgResult = ::sendmsg(socket, &msg, sendFlags);
    }
#endif

    NTSU_SOCKETUTIL_DEBUG_SENDMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         sendmsgResult,
                                         errno);

    if (sendmsgResult < 0) {
        return ntsa::Error(errno);
    }

#if defined(BSLS_PLATFORM_OS_LINUX)
    if ((sendFlags & ntsu::ZeroCopyUtil::e_MSG_ZEROCOPY) != 0) {
        context->setZeroCopy(true);
    }
#endif

    context->setBytesSent(sendmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::send(ntsa::SendContext*       context,
                             const ntsa::File&        file,
                             const ntsa::SendOptions& options,
                             ntsa::Handle             socket)
{
#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_SOLARIS)

    context->reset();

    if (!options.endpoint().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!options.foreignHandle().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::size_t size   = NTSCFG_WARNING_NARROW(bsl::size_t, file.size());
    off_t       offset = static_cast<off_t>(file.position());

    context->setBytesSendable(size);

    ssize_t sendfileResult =
        ::sendfile(socket, file.descriptor(), &offset, size);

    if (sendfileResult < 0) {
        return ntsa::Error(errno);
    }

    BSLS_ASSERT(offset >= file.position());
    BSLS_ASSERT(offset - file.position() == sendfileResult);

    context->setBytesSent(sendfileResult);

    return ntsa::Error();

#else

    NTSCFG_WARNING_UNUSED(file);
    NTSCFG_WARNING_UNUSED(options);
    NTSCFG_WARNING_UNUSED(socket);

    context->reset();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);

#endif
}

ntsa::Error SocketUtil::send(ntsa::SendContext*       context,
                             const ntsa::Data&        data,
                             const ntsa::SendOptions& options,
                             ntsa::Handle             socket)
{
    if (NTSCFG_LIKELY(data.isBlob())) {
        return SocketUtil::send(context, data.blob(), options, socket);
    }
    else if (data.isSharedBlob()) {
        const bsl::shared_ptr<bdlbb::Blob>& blob = data.sharedBlob();
        if (blob) {
            return SocketUtil::send(context, *blob, options, socket);
        }
        else {
            context->reset();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (data.isBlobBuffer()) {
        return SocketUtil::send(context, data.blobBuffer(), options, socket);
    }
    else if (data.isConstBuffer()) {
        return SocketUtil::send(context, data.constBuffer(), options, socket);
    }
    else if (data.isConstBufferArray()) {
        return SocketUtil::send(context,
                                data.constBufferArray(),
                                options,
                                socket);
    }
    else if (data.isConstBufferPtrArray()) {
        return SocketUtil::send(context,
                                data.constBufferPtrArray(),
                                options,
                                socket);
    }
    else if (data.isMutableBuffer()) {
        return SocketUtil::send(context,
                                data.mutableBuffer(),
                                options,
                                socket);
    }
    else if (data.isMutableBufferArray()) {
        return SocketUtil::send(context,
                                data.mutableBufferArray(),
                                options,
                                socket);
    }
    else if (data.isMutableBufferPtrArray()) {
        return SocketUtil::send(context,
                                data.mutableBufferPtrArray(),
                                options,
                                socket);
    }
    else if (data.isString()) {
        return SocketUtil::send(context, data.string(), options, socket);
    }
    else if (data.isFile()) {
        return SocketUtil::send(context, data.file(), options, socket);
    }
    else {
        context->reset();
        return ntsa::Error::invalid();
    }
}

ntsa::Error SocketUtil::sendToMultiple(bsl::size_t* numBytesSendable,
                                       bsl::size_t* numBytesSent,
                                       bsl::size_t* numMessagesSendable,
                                       bsl::size_t* numMessagesSent,
                                       const ntsa::ConstMessage* messages,
                                       bsl::size_t               numMessages,
                                       ntsa::Handle              socket)
{
#if defined(BSLS_PLATFORM_OS_UNIX)
#if defined(BSLS_PLATFORM_OS_LINUX) &&                                        \
    ((__GLIBC__ >= 3) || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 17))
    *numBytesSent = 0;

    if (numBytesSendable) {
        *numBytesSendable = 0;
    }

    *numMessagesSent = 0;

    if (numMessagesSendable) {
        *numMessagesSendable = 0;
    }

    bsl::size_t numMessagesTotal = numMessages;
    if (numMessagesTotal > NTSU_SOCKETUTIL_MAX_MESSAGES_PER_SEND) {
        numMessagesTotal = NTSU_SOCKETUTIL_MAX_MESSAGES_PER_SEND;
    }

    if (numMessagesTotal == 0) {
        return ntsa::Error::invalid();
    }

    if (numMessagesSendable) {
        *numMessagesSendable = numMessagesTotal;
    }

    mmsghdr mmsg[NTSU_SOCKETUTIL_MAX_MESSAGES_PER_SEND];
    bsl::memset(mmsg, 0, sizeof(mmsghdr) * numMessagesTotal);

    sockaddr_storage socketAddress[NTSU_SOCKETUTIL_MAX_MESSAGES_PER_SEND];
    bsl::memset(socketAddress, 0, sizeof(sockaddr_storage) * numMessagesTotal);

    for (bsl::size_t mmsgIndex = 0; mmsgIndex < numMessagesTotal; ++mmsgIndex)
    {
        msghdr& msg = mmsg[mmsgIndex].msg_hdr;

        bsl::size_t numBuffersTotal = messages[mmsgIndex].numBuffers();
        if (numBuffersTotal > NTSU_SOCKETUTIL_MAX_BUFFERS_PER_SEND) {
            numBuffersTotal = NTSU_SOCKETUTIL_MAX_BUFFERS_PER_SEND;
        }

        bsl::size_t numBytesTotal = 0;
        for (bsl::size_t i = 0; i < numBuffersTotal; ++i) {
            numBytesTotal += messages[mmsgIndex].buffer(i).length();
        }

        if (numBuffersTotal == 0) {
            return ntsa::Error::invalid();
        }

        if (numBytesTotal == 0) {
            return ntsa::Error::invalid();
        }

        if (numBytesSendable) {
            *numBytesSendable += numBytesTotal;
        }

        socklen_t socketAddressSize;
        {
            ntsa::Error error =
                SocketStorageUtil::convert(&socketAddress[mmsgIndex],
                                           &socketAddressSize,
                                           messages[mmsgIndex].endpoint());
            if (error) {
                return error;
            }
        }

        msg.msg_name    = &socketAddress[mmsgIndex];
        msg.msg_namelen = socketAddressSize;
        msg.msg_iov     = const_cast<struct ::iovec*>(
            reinterpret_cast<const struct ::iovec*>(
                &messages[mmsgIndex].buffer(0)));
        msg.msg_iovlen = static_cast<int>(numBuffersTotal);
    }

    int sendmmsgResult =
        ::sendmmsg(socket,
                   mmsg,
                   NTSCFG_WARNING_NARROW(int, numMessagesTotal),
                   0);

    if (sendmmsgResult < 0) {
        return ntsa::Error(errno);
    }

    for (int mmsgIndex = 0; mmsgIndex < sendmmsgResult; ++mmsgIndex) {
        *numBytesSent += static_cast<bsl::size_t>(mmsg[mmsgIndex].msg_len);
    }

    *numMessagesSent = static_cast<bsl::size_t>(sendmmsgResult);

    return ntsa::Error();

#else

    NTSCFG_WARNING_UNUSED(numBytesSendable);
    NTSCFG_WARNING_UNUSED(numBytesSent);
    NTSCFG_WARNING_UNUSED(numMessagesSendable);
    NTSCFG_WARNING_UNUSED(numMessagesSent);
    NTSCFG_WARNING_UNUSED(messages);
    NTSCFG_WARNING_UNUSED(numMessages);
    NTSCFG_WARNING_UNUSED(socket);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED, ENOTSUP);

#endif
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED, WSAENOTSUP);
#else
#error Not implemented
#endif
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*       context,
                                void*                       data,
                                bsl::size_t                 capacity,
                                const ntsa::ReceiveOptions& options,
                                ntsa::Handle                socket)
{
    context->reset();

    const bool wantEndpoint = options.wantEndpoint();
    const bool wantMetaData = options.wantMetaData();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (wantEndpoint) {
        SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    ReceiveControl control;
    if (wantMetaData) {
        control.initialize(&msg);
    }

    if (capacity == 0) {
        return ntsa::Error::invalid();
    }

    struct iovec iovec;
    iovec.iov_base = data;
    iovec.iov_len  = capacity;

    msg.msg_iov    = &iovec;
    msg.msg_iovlen = 1;

    context->setBytesReceivable(capacity);

    ssize_t recvmsgResult =
        ::recvmsg(socket, &msg, NTSU_SOCKETUTIL_RECVMSG_FLAGS);

    NTSU_SOCKETUTIL_DEBUG_RECVMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         recvmsgResult,
                                         errno);

    if (recvmsgResult < 0) {
        return ntsa::Error(errno);
    }

    if (wantEndpoint) {
        ntsa::Endpoint endpoint;
        SocketStorageUtil::convert(
            &endpoint,
            reinterpret_cast<sockaddr_storage*>(msg.msg_name),
            msg.msg_namelen);

        context->setEndpoint(endpoint);
    }

    if (wantMetaData) {
        control.decode(context, msg, options);
    }

    context->setBytesReceived(recvmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*       context,
                                ntsa::MutableBuffer*        buffer,
                                const ntsa::ReceiveOptions& options,
                                ntsa::Handle                socket)
{
    context->reset();

    const bool wantEndpoint = options.wantEndpoint();
    const bool wantMetaData = options.wantMetaData();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (wantEndpoint) {
        SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    ReceiveControl control;
    if (wantMetaData) {
        control.initialize(&msg);
    }

    void*             data     = buffer->data();
    const bsl::size_t capacity = buffer->size();

    if (capacity == 0) {
        return ntsa::Error::invalid();
    }

    struct iovec iovec;
    iovec.iov_base = data;
    iovec.iov_len  = capacity;

    msg.msg_iov    = &iovec;
    msg.msg_iovlen = 1;

    context->setBytesReceivable(capacity);

    ssize_t recvmsgResult =
        ::recvmsg(socket, &msg, NTSU_SOCKETUTIL_RECVMSG_FLAGS);

    NTSU_SOCKETUTIL_DEBUG_RECVMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         recvmsgResult,
                                         errno);

    if (recvmsgResult < 0) {
        return ntsa::Error(errno);
    }

    if (wantEndpoint) {
        ntsa::Endpoint endpoint;
        SocketStorageUtil::convert(
            &endpoint,
            reinterpret_cast<sockaddr_storage*>(msg.msg_name),
            msg.msg_namelen);

        context->setEndpoint(endpoint);
    }

    if (wantMetaData) {
        control.decode(context, msg, options);
    }

    context->setBytesReceived(recvmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*       context,
                                ntsa::MutableBuffer*        bufferArray,
                                bsl::size_t                 bufferCount,
                                const ntsa::ReceiveOptions& options,
                                ntsa::Handle                socket)
{
    context->reset();

    const bool wantEndpoint = options.wantEndpoint();
    const bool wantMetaData = options.wantMetaData();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (wantEndpoint) {
        SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    ReceiveControl control;
    if (wantMetaData) {
        control.initialize(&msg);
    }

    bsl::size_t capacity =
        ntsa::MutableBuffer::totalSize(bufferArray, bufferCount);
    if (capacity == 0) {
        return ntsa::Error::invalid();
    }

    msg.msg_iov    = NTSU_SOCKETUTIL_MSG_IOV_BUF(bufferArray);
    msg.msg_iovlen = NTSU_SOCKETUTIL_MSG_IOV_LEN(bufferCount);

    context->setBytesReceivable(capacity);

    ssize_t recvmsgResult =
        ::recvmsg(socket, &msg, NTSU_SOCKETUTIL_RECVMSG_FLAGS);

    NTSU_SOCKETUTIL_DEBUG_RECVMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         recvmsgResult,
                                         errno);

    if (recvmsgResult < 0) {
        return ntsa::Error(errno);
    }

    if (wantEndpoint) {
        ntsa::Endpoint endpoint;
        SocketStorageUtil::convert(
            &endpoint,
            reinterpret_cast<sockaddr_storage*>(msg.msg_name),
            msg.msg_namelen);

        context->setEndpoint(endpoint);
    }

    if (wantMetaData) {
        control.decode(context, msg, options);
    }

    context->setBytesReceived(recvmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*       context,
                                ntsa::MutableBufferArray*   bufferArray,
                                const ntsa::ReceiveOptions& options,
                                ntsa::Handle                socket)
{
    context->reset();

    const bool wantEndpoint = options.wantEndpoint();
    const bool wantMetaData = options.wantMetaData();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (wantEndpoint) {
        SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    ReceiveControl control;
    if (wantMetaData) {
        control.initialize(&msg);
    }

    bsl::size_t numBuffersTotal = bufferArray->numBuffers();
    if (numBuffersTotal == 0) {
        return ntsa::Error::invalid();
    }

    bsl::size_t numBytesTotal = bufferArray->numBytes();
    if (numBytesTotal == 0) {
        return ntsa::Error::invalid();
    }

    msg.msg_iov    = NTSU_SOCKETUTIL_MSG_IOV_BUF(bufferArray->base());
    msg.msg_iovlen = NTSU_SOCKETUTIL_MSG_IOV_LEN(numBuffersTotal);

    context->setBytesReceivable(numBytesTotal);

    ssize_t recvmsgResult =
        ::recvmsg(socket, &msg, NTSU_SOCKETUTIL_RECVMSG_FLAGS);

    NTSU_SOCKETUTIL_DEBUG_RECVMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         recvmsgResult,
                                         errno);

    if (recvmsgResult < 0) {
        return ntsa::Error(errno);
    }

    if (wantEndpoint) {
        ntsa::Endpoint endpoint;
        SocketStorageUtil::convert(
            &endpoint,
            reinterpret_cast<sockaddr_storage*>(msg.msg_name),
            msg.msg_namelen);

        context->setEndpoint(endpoint);
    }

    if (wantMetaData) {
        control.decode(context, msg, options);
    }

    context->setBytesReceived(recvmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*        context,
                                ntsa::MutableBufferPtrArray* bufferArray,
                                const ntsa::ReceiveOptions&  options,
                                ntsa::Handle                 socket)
{
    context->reset();

    const bool wantEndpoint = options.wantEndpoint();
    const bool wantMetaData = options.wantMetaData();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (wantEndpoint) {
        SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    ReceiveControl control;
    if (wantMetaData) {
        control.initialize(&msg);
    }

    bsl::size_t numBuffersTotal = bufferArray->numBuffers();
    if (numBuffersTotal == 0) {
        return ntsa::Error::invalid();
    }

    bsl::size_t numBytesTotal = bufferArray->numBytes();
    if (numBytesTotal == 0) {
        return ntsa::Error::invalid();
    }

    msg.msg_iov    = NTSU_SOCKETUTIL_MSG_IOV_BUF(bufferArray->base());
    msg.msg_iovlen = NTSU_SOCKETUTIL_MSG_IOV_LEN(numBuffersTotal);

    context->setBytesReceivable(numBytesTotal);

    ssize_t recvmsgResult =
        ::recvmsg(socket, &msg, NTSU_SOCKETUTIL_RECVMSG_FLAGS);

    NTSU_SOCKETUTIL_DEBUG_RECVMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         recvmsgResult,
                                         errno);

    if (recvmsgResult < 0) {
        return ntsa::Error(errno);
    }

    if (wantEndpoint) {
        ntsa::Endpoint endpoint;
        SocketStorageUtil::convert(
            &endpoint,
            reinterpret_cast<sockaddr_storage*>(msg.msg_name),
            msg.msg_namelen);

        context->setEndpoint(endpoint);
    }

    if (wantMetaData) {
        control.decode(context, msg, options);
    }

    context->setBytesReceived(recvmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*       context,
                                bsl::string*                string,
                                const ntsa::ReceiveOptions& options,
                                ntsa::Handle                socket)
{
    context->reset();

    const bool wantEndpoint = options.wantEndpoint();
    const bool wantMetaData = options.wantMetaData();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (wantEndpoint) {
        SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    ReceiveControl control;
    if (wantMetaData) {
        control.initialize(&msg);
    }

    bsl::size_t size     = string->size();
    bsl::size_t capacity = string->capacity() - size;
    void*       data     = string->data() + size;

    if (capacity == 0) {
        return ntsa::Error::invalid();
    }

    struct iovec iovec;
    iovec.iov_base = data;
    iovec.iov_len  = capacity;

    msg.msg_iov    = &iovec;
    msg.msg_iovlen = 1;

    context->setBytesReceivable(capacity);

    ssize_t recvmsgResult =
        ::recvmsg(socket, &msg, NTSU_SOCKETUTIL_RECVMSG_FLAGS);

    NTSU_SOCKETUTIL_DEBUG_RECVMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         recvmsgResult,
                                         errno);

    if (recvmsgResult < 0) {
        return ntsa::Error(errno);
    }

    if (wantEndpoint) {
        ntsa::Endpoint endpoint;
        SocketStorageUtil::convert(
            &endpoint,
            reinterpret_cast<sockaddr_storage*>(msg.msg_name),
            msg.msg_namelen);

        context->setEndpoint(endpoint);
    }

    if (wantMetaData) {
        control.decode(context, msg, options);
    }

    context->setBytesReceived(recvmsgResult);
    string->resize(size + recvmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*       context,
                                bdlbb::Blob*                blob,
                                const ntsa::ReceiveOptions& options,
                                ntsa::Handle                socket)
{
    context->reset();

    const bool wantEndpoint = options.wantEndpoint();
    const bool wantMetaData = options.wantMetaData();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (wantEndpoint) {
        SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    ReceiveControl control;
    if (wantMetaData) {
        control.initialize(&msg);
    }

    bsl::size_t numBytesMax = options.maxBytes();
    if (numBytesMax == 0) {
        numBytesMax = SocketUtil::maxBytesPerReceive(socket);
    }

    bsl::size_t numBuffersMax = options.maxBuffers();
    if (numBuffersMax == 0) {
        numBuffersMax = NTSU_SOCKETUTIL_MAX_BUFFERS_PER_RECEIVE;
    }
    else if (numBuffersMax > NTSU_SOCKETUTIL_MAX_BUFFERS_PER_RECEIVE) {
        numBuffersMax = NTSU_SOCKETUTIL_MAX_BUFFERS_PER_RECEIVE;
    }

    bsl::size_t size     = blob->length();
    bsl::size_t capacity = blob->totalSize() - size;
    if (capacity == 0) {
        return ntsa::Error::invalid();
    }

    struct iovec iovecArray[NTSU_SOCKETUTIL_MAX_BUFFERS_PER_RECEIVE];

    bsl::size_t numBuffersTotal;
    bsl::size_t numBytesTotal;

    ntsu::BufferUtil::scatter(
        &numBuffersTotal,
        &numBytesTotal,
        reinterpret_cast<ntsa::MutableBuffer*>(iovecArray),
        numBuffersMax,
        blob,
        numBytesMax);

    msg.msg_iov    = iovecArray;
    msg.msg_iovlen = NTSU_SOCKETUTIL_MSG_IOV_LEN(numBuffersTotal);

    context->setBytesReceivable(numBytesTotal);

    ssize_t recvmsgResult =
        ::recvmsg(socket, &msg, NTSU_SOCKETUTIL_RECVMSG_FLAGS);

    NTSU_SOCKETUTIL_DEBUG_RECVMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         recvmsgResult,
                                         errno);

    if (recvmsgResult < 0) {
        return ntsa::Error(errno);
    }

    if (wantEndpoint) {
        ntsa::Endpoint endpoint;
        SocketStorageUtil::convert(
            &endpoint,
            reinterpret_cast<sockaddr_storage*>(msg.msg_name),
            msg.msg_namelen);

        context->setEndpoint(endpoint);
    }

    if (wantMetaData) {
        control.decode(context, msg, options);
    }

    context->setBytesReceived(recvmsgResult);
    blob->setLength(NTSCFG_WARNING_NARROW(int, size + recvmsgResult));

    return ntsa::Error();
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*       context,
                                bdlbb::BlobBuffer*          blobBuffer,
                                const ntsa::ReceiveOptions& options,
                                ntsa::Handle                socket)
{
    context->reset();

    const bool wantEndpoint = options.wantEndpoint();
    const bool wantMetaData = options.wantMetaData();

    msghdr msg;
    bsl::memset(&msg, 0, sizeof msg);

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (wantEndpoint) {
        SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

        msg.msg_name    = &socketAddress;
        msg.msg_namelen = socketAddressSize;
    }

    ReceiveControl control;
    if (wantMetaData) {
        control.initialize(&msg);
    }

    void*       data     = blobBuffer->data();
    bsl::size_t capacity = blobBuffer->size();
    if (capacity == 0) {
        return ntsa::Error::invalid();
    }

    struct iovec iovec;
    iovec.iov_base = data;
    iovec.iov_len  = capacity;

    msg.msg_iov    = &iovec;
    msg.msg_iovlen = 1;

    context->setBytesReceivable(capacity);

    ssize_t recvmsgResult =
        ::recvmsg(socket, &msg, NTSU_SOCKETUTIL_RECVMSG_FLAGS);

    NTSU_SOCKETUTIL_DEBUG_RECVMSG_UPDATE(msg.msg_iov,
                                         msg.msg_iovlen,
                                         recvmsgResult,
                                         errno);

    if (recvmsgResult < 0) {
        return ntsa::Error(errno);
    }

    if (wantEndpoint) {
        ntsa::Endpoint endpoint;
        SocketStorageUtil::convert(
            &endpoint,
            reinterpret_cast<sockaddr_storage*>(msg.msg_name),
            msg.msg_namelen);

        context->setEndpoint(endpoint);
    }

    if (wantMetaData) {
        control.decode(context, msg, options);
    }

    context->setBytesReceived(recvmsgResult);

    return ntsa::Error();
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*       context,
                                ntsa::Data*                 data,
                                const ntsa::ReceiveOptions& options,
                                ntsa::Handle                socket)
{
    if (NTSCFG_LIKELY(data->isBlob())) {
        return SocketUtil::receive(context, &data->blob(), options, socket);
    }
    else if (data->isSharedBlob()) {
        bsl::shared_ptr<bdlbb::Blob>& blob = data->sharedBlob();
        if (blob) {
            return SocketUtil::receive(context, blob.get(), options, socket);
        }
        else {
            context->reset();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (data->isBlobBuffer()) {
        return SocketUtil::receive(context,
                                   &data->blobBuffer(),
                                   options,
                                   socket);
    }
    else if (data->isMutableBuffer()) {
        return SocketUtil::receive(context,
                                   &data->mutableBuffer(),
                                   options,
                                   socket);
    }
    else if (data->isMutableBufferArray()) {
        return SocketUtil::receive(context,
                                   &data->mutableBufferArray(),
                                   options,
                                   socket);
    }
    else if (data->isMutableBufferPtrArray()) {
        return SocketUtil::receive(context,
                                   &data->mutableBufferPtrArray(),
                                   options,
                                   socket);
    }
    else if (data->isString()) {
        return SocketUtil::receive(context, &data->string(), options, socket);
    }
    else {
        context->reset();
        return ntsa::Error::invalid();
    }
}

ntsa::Error SocketUtil::receiveFromMultiple(bsl::size_t* numBytesReceivable,
                                            bsl::size_t* numBytesReceived,
                                            bsl::size_t* numMessagesReceivable,
                                            bsl::size_t* numMessagesReceived,
                                            ntsa::MutableMessage* messages,
                                            bsl::size_t           numMessages,
                                            ntsa::Handle          socket)
{
#if defined(BSLS_PLATFORM_OS_UNIX)
#if defined(BSLS_PLATFORM_OS_LINUX) &&                                        \
    ((__GLIBC__ >= 3) || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 17))
    *numBytesReceived = 0;

    if (numBytesReceivable) {
        *numBytesReceivable = 0;
    }

    *numMessagesReceived = 0;

    if (numMessagesReceivable) {
        *numMessagesReceivable = 0;
    }

    bsl::size_t numMessagesTotal = numMessages;
    if (numMessagesTotal > NTSU_SOCKETUTIL_MAX_MESSAGES_PER_RECEIVE) {
        numMessagesTotal = NTSU_SOCKETUTIL_MAX_MESSAGES_PER_RECEIVE;
    }

    if (numMessagesTotal == 0) {
        return ntsa::Error::invalid();
    }

    if (numMessagesReceivable) {
        *numMessagesReceivable = numMessagesTotal;
    }

    mmsghdr mmsg[NTSU_SOCKETUTIL_MAX_MESSAGES_PER_RECEIVE];
    bsl::memset(mmsg, 0, sizeof(mmsghdr) * numMessagesTotal);

    sockaddr_storage socketAddress[NTSU_SOCKETUTIL_MAX_MESSAGES_PER_RECEIVE];
    bsl::memset(socketAddress, 0, sizeof(sockaddr_storage) * numMessagesTotal);

    socklen_t socketAddressSize[NTSU_SOCKETUTIL_MAX_MESSAGES_PER_RECEIVE];
    bsl::memset(&socketAddressSize, 0, sizeof(socklen_t) * numMessagesTotal);

    for (bsl::size_t mmsgIndex = 0; mmsgIndex < numMessagesTotal; ++mmsgIndex)
    {
        msghdr& msg = mmsg[mmsgIndex].msg_hdr;

        bsl::size_t numBuffersTotal = messages[mmsgIndex].numBuffers();
        if (numBuffersTotal > NTSU_SOCKETUTIL_MAX_BUFFERS_PER_RECEIVE) {
            numBuffersTotal = NTSU_SOCKETUTIL_MAX_BUFFERS_PER_RECEIVE;
        }

        bsl::size_t numBytesTotal = 0;
        for (bsl::size_t i = 0; i < numBuffersTotal; ++i) {
            numBytesTotal += messages[mmsgIndex].buffer(i).length();
        }

        if (numBuffersTotal == 0) {
            return ntsa::Error::invalid();
        }

        if (numBytesTotal == 0) {
            return ntsa::Error::invalid();
        }

        if (numBytesReceivable) {
            *numBytesReceivable += numBytesTotal;
        }

        SocketStorageUtil::initialize(&socketAddress[mmsgIndex],
                                      &socketAddressSize[mmsgIndex]);

        msg.msg_name    = &socketAddress[mmsgIndex];
        msg.msg_namelen = socketAddressSize[mmsgIndex];
        msg.msg_iov     = const_cast<struct ::iovec*>(
            reinterpret_cast<const struct ::iovec*>(
                &messages[mmsgIndex].buffer(0)));
        msg.msg_iovlen = static_cast<int>(numBuffersTotal);
    }

    int recvmmsgResult =
        ::recvmmsg(socket,
                   mmsg,
                   NTSCFG_WARNING_NARROW(int, numMessagesTotal),
                   0,
                   0);

    if (recvmmsgResult < 0) {
        return ntsa::Error(errno);
    }

    for (int mmsgIndex = 0; mmsgIndex < recvmmsgResult; ++mmsgIndex) {
        msghdr& msg = mmsg[mmsgIndex].msg_hdr;

        ntsa::Endpoint endpoint;
        SocketStorageUtil::convert(
            &endpoint,
            reinterpret_cast<sockaddr_storage*>(msg.msg_name),
            msg.msg_namelen);

        messages[mmsgIndex].setEndpoint(endpoint);
        messages[mmsgIndex].setSize(
            static_cast<bsl::size_t>(mmsg[mmsgIndex].msg_len));

        *numBytesReceived += static_cast<bsl::size_t>(mmsg[mmsgIndex].msg_len);
    }

    *numMessagesReceived = static_cast<bsl::size_t>(recvmmsgResult);

    return ntsa::Error();

#else

    NTSCFG_WARNING_UNUSED(numBytesReceivable);
    NTSCFG_WARNING_UNUSED(numBytesReceived);
    NTSCFG_WARNING_UNUSED(numMessagesReceivable);
    NTSCFG_WARNING_UNUSED(numMessagesReceived);
    NTSCFG_WARNING_UNUSED(messages);
    NTSCFG_WARNING_UNUSED(numMessages);
    NTSCFG_WARNING_UNUSED(socket);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED, ENOTSUP);

#endif
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED, WSAENOTSUP);
#else
#error Not implemented
#endif
}

ntsa::Error SocketUtil::receiveNotifications(
    ntsa::NotificationQueue* notifications,
    ntsa::Handle             socket)
{
    NTSCFG_WARNING_UNUSED(notifications);
    NTSCFG_WARNING_UNUSED(socket);

#if defined(BSLS_PLATFORM_OS_LINUX)

    const size_t k_BUF_SIZE = CMSG_SPACE(1024);
    bsls::AlignedBuffer<static_cast<int>(k_BUF_SIZE),
                        bsls::AlignmentFromType<cmsghdr>::VALUE>
        buf;

    while (true) {
        bsl::memset(buf.buffer(), 0, k_BUF_SIZE);

        msghdr msg;
        bsl::memset(&msg, 0, sizeof(msg));

        msg.msg_control    = buf.buffer();
        msg.msg_controllen = k_BUF_SIZE;

        ssize_t recvMsgResult = ::recvmsg(socket, &msg, MSG_ERRQUEUE);

        if (recvMsgResult < 0) {
            if (errno == EAGAIN) {
                return ntsa::Error();
            }
            else {
                return ntsa::Error(errno);
            }
        }

        // The socket error queue must be drained even if there is no space to
        // store the notification.

        if (notifications == 0) {
            continue;
        }

        // Assume the timestamp information comes in pairs: meta data +
        // timestamp message.
        // Meta data is sock_extend_err structure: for IPv4:
        // (cmsg_level == SOL_IP && cmsg_type == IP_RECVERR) and for IPv6:
        // (cmsg_level == SOL_IPV6 && cmsg_type == IPV6_RECVERR)
        // and timestamp information is (cmsg_level == SOL_SOCKET &&
        // hdr->cmsg_type == SO_TIMESTAMPING).
        //
        // The messages may be received in any order, e.g.
        // IP_RECVERR -> SO_TIMESTAMPING or SO_TIMESTAMPING -> IP_RECVERR.

        bool tsMetaDataReceived = false;
        bool timestampReceived = false;

        ntsa::Timestamp    ts;
        ntsa::Notification notification;

        for (cmsghdr* hdr = CMSG_FIRSTHDR(&msg); hdr != 0;
             hdr          = CMSG_NXTHDR(&msg, hdr))
        {
            if ((hdr->cmsg_level == SOL_IP && hdr->cmsg_type == IP_RECVERR) ||
                (hdr->cmsg_level == SOL_IPV6 &&
                 hdr->cmsg_type == IPV6_RECVERR))
            {
                sock_extended_err ser;
                std::memcpy(&ser, CMSG_DATA(hdr), sizeof(ser));

                if (ser.ee_origin == SO_EE_ORIGIN_TIMESTAMPING) {
                    ts.setId(ser.ee_data);
                    switch (ser.ee_info) {
                    case (TimestampUtil::e_SCM_TSTAMP_SCHED): {
                        ts.setType(ntsa::TimestampType::e_SCHEDULED);
                    } break;
                    case (TimestampUtil::e_SCM_TSTAMP_SND): {
                        ts.setType(ntsa::TimestampType::e_SENT);
                    } break;
                    case (TimestampUtil::e_SCM_TSTAMP_ACK): {
                        ts.setType(ntsa::TimestampType::e_ACKNOWLEDGED);
                    } break;
                    default: {
                        //error, drop timestamp
                    }
                    }
                    tsMetaDataReceived = true;
                    if (timestampReceived) {
                        notification.reset();
                        notification.makeTimestamp(ts);

                        notifications->addNotification(notification);
                        tsMetaDataReceived = false;
                        timestampReceived = false;
                    }
                }
                else if (ser.ee_origin ==
                         ntsu::ZeroCopyUtil::e_SO_EE_ORIGIN_ZEROCOPY)
                {
                    ntsa::ZeroCopy zeroCopy;

                    zeroCopy.setFrom(ser.ee_info);
                    zeroCopy.setThru(ser.ee_data);

                    if (ser.ee_code ==
                        ntsu::ZeroCopyUtil::e_SO_EE_CODE_ZEROCOPY_COPIED)
                    {
                        zeroCopy.setType(ntsa::ZeroCopyType::e_DEFERRED);
                    }
                    else {
                        zeroCopy.setType(ntsa::ZeroCopyType::e_AVOIDED);
                    }

                    notification.makeZeroCopy(zeroCopy);
                    notifications->addNotification(notification);
                }
            }
            else if (hdr->cmsg_level == SOL_SOCKET &&
                     hdr->cmsg_type == TimestampUtil::e_SCM_TIMESTAMPING)
            {
                TimestampUtil::ScmTimestamping sts;
                std::memcpy(&sts, CMSG_DATA(hdr), sizeof(sts));

                bsls::TimeInterval ti(
                    static_cast<int>(sts.softwareTs.tv_sec),
                    static_cast<int>(sts.softwareTs.tv_nsec));

                ts.setTime(ti);

                timestampReceived = true;
                if (tsMetaDataReceived) {
                    notification.reset();
                    notification.makeTimestamp(ts);

                    notifications->addNotification(notification);

                    tsMetaDataReceived = false;
                    timestampReceived = false;
                }
            }
            else {
                BSLS_LOG_WARN(
                    "Unexpected control message received: cmsg_level = %d, "
                    "cmsg_type = %d, tsMetaDataReceied = %d, "
                    "timestampReceived = %d",
                    hdr->cmsg_level,
                    hdr->cmsg_type,
                    tsMetaDataReceived,
                    timestampReceived);
            }
        }
    }

    return ntsa::Error();
#else
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
#endif
}

ntsa::Error SocketUtil::shutdown(ntsa::ShutdownType::Value direction,
                                 ntsa::Handle              socket)
{
    int how = direction == ntsa::ShutdownType::e_SEND      ? SHUT_WR
              : direction == ntsa::ShutdownType::e_RECEIVE ? SHUT_RD
                                                           : SHUT_RDWR;

    int rc = ::shutdown(socket, how);
    if (rc != 0) {
        int lastError = errno;
        if (lastError != EINVAL && lastError != ENOTCONN) {
            return ntsa::Error(lastError);
        }
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::unlink(ntsa::Handle socket)
{
    int rc;

    if (socket == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error();
    }

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

    rc = ::getsockname(socket,
                       reinterpret_cast<sockaddr*>(&socketAddress),
                       &socketAddressSize);
    if (rc == 0) {
        if (socketAddress.ss_family == AF_UNIX) {
            const sockaddr_un* socketAddressLocal =
                reinterpret_cast<const sockaddr_un*>(&socketAddress);

            if (socketAddressLocal->sun_path[0] != 0) {
                const bsl::size_t pathOffset =
                    offsetof(struct sockaddr_un, sun_path);
                if (socketAddressSize > pathOffset) {
                    bsl::size_t pathLength = socketAddressSize - pathOffset;
                    bsl::string path(socketAddressLocal->sun_path, pathLength);

                    if (!path.empty()) {
                        rc = ::unlink(path.c_str());
                        if (rc != 0 && errno != ENOENT) {
                            ntsa::Error error(errno);
                            BSLS_LOG_WARN("Failed to unlink %s: %s",
                                          path.c_str(),
                                          error.text().c_str());
                        }
                    }
                }
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::close(ntsa::Handle socket)
{
    int rc;

    if (socket == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error();
    }

    bool unlinkFlag = false;

    {
        int       optionValue;
        socklen_t optionSize = sizeof(optionValue);

        rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_TYPE,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

        if (rc == 0) {
            if (optionValue == SOCK_DGRAM) {
                unlinkFlag = true;
            }
        }
    }

    if (!unlinkFlag) {
        int       optionValue;
        socklen_t optionSize = sizeof(optionValue);

        rc = ::getsockopt(socket,
                          SOL_SOCKET,
                          SO_ACCEPTCONN,
                          &optionValue,
                          &optionSize);
        if (rc == 0) {
            if (optionValue != 0) {
                unlinkFlag = true;
            }
        }
        else {
            rc = ::listen(socket, SOMAXCONN);
            if (rc == 0) {
                unlinkFlag = true;
            }
        }
    }

    if (unlinkFlag) {
        ntsu::SocketUtil::unlink(socket);
    }

#if NTSU_SOCKETUTIL_DEBUG_LIFETIME
    NTSU_SOCKETUTIL_DEBUG_LIFETIME_LOG("Socket handle %d closing", socket);
#endif

#if NTSU_SOCKETUTIL_DEBUG_HANDLE_MAP
    {
        bsls::SpinLockGuard guard(&s_handleSetLock);
        s_handleSet.erase(socket);
    }
#endif

    rc = ::close(socket);
    if (rc != 0) {
#if NTSU_SOCKETUTIL_DEBUG_LIFETIME
        NTSU_SOCKETUTIL_DEBUG_LIFETIME_LOG("Failed to close socket handle %d",
                                           socket);
#endif
        return ntsa::Error(errno);
    }

#if NTSU_SOCKETUTIL_DEBUG_LIFETIME
    NTSU_SOCKETUTIL_DEBUG_LIFETIME_LOG("Socket handle %d closed", socket);
#endif

    return ntsa::Error();
}

ntsa::Error SocketUtil::sourceEndpoint(ntsa::Endpoint* result,
                                       ntsa::Handle    socket)
{
    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

    int rc = ::getsockname(socket,
                           reinterpret_cast<sockaddr*>(&socketAddress),
                           &socketAddressSize);
    if (rc != 0) {
        return ntsa::Error(errno);
    }

    ntsa::Error error =
        SocketStorageUtil::convert(result, &socketAddress, socketAddressSize);
    if (error) {
        return error;
    }

    if (result->isIp() && result->ip().host().isV6()) {
        if (result->ip().host().v6().scopeId() == 0) {
            ntsa::Ipv6ScopeId scopeId =
                ntsu::AdapterUtil::discoverScopeId(result->ip().host().v6());
            if (scopeId != 0) {
                ntsa::Ipv6Address host(result->ip().host().v6());
                host.setScopeId(scopeId);
                result->ip().setHost(host);
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::remoteEndpoint(ntsa::Endpoint* result,
                                       ntsa::Handle    socket)
{
    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

    int rc = ::getpeername(socket,
                           reinterpret_cast<sockaddr*>(&socketAddress),
                           &socketAddressSize);
    if (rc != 0) {
        return ntsa::Error(errno);
    }

    ntsa::Error error =
        SocketStorageUtil::convert(result, &socketAddress, socketAddressSize);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::waitUntilReadable(ntsa::Handle socket)
{
    struct ::pollfd pfd;

    pfd.fd      = socket;
    pfd.events  = POLLIN | POLLHUP | POLLERR | POLLNVAL;
    pfd.revents = 0;

    int rc = ::poll(&pfd, 1, -1);
    if (rc < 0) {
        return ntsa::Error(errno);
    }

    if (rc == 0) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    if (((pfd.revents & POLLIN) != 0) || ((pfd.revents & POLLHUP) != 0)) {
        return ntsa::Error();
    }

    return ntsa::Error::invalid();
}

ntsa::Error SocketUtil::waitUntilReadable(ntsa::Handle              socket,
                                          const bsls::TimeInterval& timeout)
{
    struct ::pollfd pfd;

    pfd.fd      = socket;
    pfd.events  = POLLIN | POLLHUP | POLLERR | POLLNVAL;
    pfd.revents = 0;

    bsls::TimeInterval now = bdlt::CurrentTime::now();

    bsls::TimeInterval delta;
    if (timeout > now) {
        delta = timeout - now;
    }

    bsl::int64_t milliseconds =
        static_cast<bsl::int64_t>(delta.totalMilliseconds());

    if (milliseconds > bsl::numeric_limits<int>::max()) {
        milliseconds = bsl::numeric_limits<int>::max();
    }

    int rc = ::poll(&pfd, 1, static_cast<int>(milliseconds));
    if (rc < 0) {
        return ntsa::Error(errno);
    }

    if (rc == 0) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    if (((pfd.revents & POLLIN) != 0) || ((pfd.revents & POLLHUP) != 0)) {
        return ntsa::Error();
    }

    return ntsa::Error::invalid();
}

ntsa::Error SocketUtil::waitUntilWritable(ntsa::Handle socket)
{
    struct ::pollfd pfd;

    pfd.fd      = socket;
    pfd.events  = POLLOUT | POLLERR | POLLNVAL;
    pfd.revents = 0;

    int rc = ::poll(&pfd, 1, -1);
    if (rc < 0) {
        return ntsa::Error(errno);
    }

    if (rc == 0) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    if ((pfd.revents & POLLOUT) != 0) {
        return ntsa::Error();
    }

    return ntsa::Error::invalid();
}

ntsa::Error SocketUtil::waitUntilWritable(ntsa::Handle              socket,
                                          const bsls::TimeInterval& timeout)
{
    struct ::pollfd pfd;

    pfd.fd      = socket;
    pfd.events  = POLLOUT | POLLERR | POLLNVAL;
    pfd.revents = 0;

    bsls::TimeInterval now = bdlt::CurrentTime::now();

    bsls::TimeInterval delta;
    if (timeout > now) {
        delta = timeout - now;
    }

    bsl::int64_t milliseconds =
        static_cast<bsl::int64_t>(delta.totalMilliseconds());

    if (milliseconds > bsl::numeric_limits<int>::max()) {
        milliseconds = bsl::numeric_limits<int>::max();
    }

    int rc = ::poll(&pfd, 1, static_cast<int>(milliseconds));
    if (rc < 0) {
        return ntsa::Error(errno);
    }

    if (rc == 0) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    if ((pfd.revents & POLLOUT) != 0) {
        return ntsa::Error();
    }

    return ntsa::Error::invalid();
}

ntsa::Error SocketUtil::pair(ntsa::Handle*          client,
                             ntsa::Handle*          server,
                             ntsa::Transport::Value type)
{
    ntsa::Error error;

    if (type == ntsa::Transport::e_TCP_IPV4_STREAM) {
        ntsa::Handle listener;
        error = ntsu::SocketUtil::create(&listener, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard listenerGuard(listener);

        error = ntsu::SocketUtil::bind(
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
            true,
            listener);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::listen(1, listener);
        if (error) {
            return error;
        }

        ntsa::Endpoint listenerEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&listenerEndpoint, listener);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::create(client, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard clientGuard(*client);

        error = ntsu::SocketUtil::connect(listenerEndpoint, *client);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::accept(server, listener);
        if (error) {
            return error;
        }

        clientGuard.release();
    }
    else if (type == ntsa::Transport::e_TCP_IPV6_STREAM) {
        ntsa::Handle listener;
        error = ntsu::SocketUtil::create(&listener, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard listenerGuard(listener);

        error = ntsu::SocketUtil::bind(
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), 0)),
            true,
            listener);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::listen(1, listener);
        if (error) {
            return error;
        }

        ntsa::Endpoint listenerEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&listenerEndpoint, listener);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::create(client, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard clientGuard(*client);

        error = ntsu::SocketUtil::connect(listenerEndpoint, *client);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::accept(server, listener);
        if (error) {
            return error;
        }

        clientGuard.release();
    }
    else if (type == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
        error = ntsu::SocketUtil::create(client, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard clientGuard(*client);

        error = ntsu::SocketUtil::create(server, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard serverGuard(*server);

        error = ntsu::SocketUtil::bind(
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
            true,
            *client);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::bind(
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
            true,
            *server);
        if (error) {
            return error;
        }

        ntsa::Endpoint clientEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&clientEndpoint, *client);
        if (error) {
            return error;
        }

        ntsa::Endpoint serverEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint, *server);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::connect(serverEndpoint, *client);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::connect(clientEndpoint, *server);
        if (error) {
            return error;
        }

        clientGuard.release();
        serverGuard.release();
    }
    else if (type == ntsa::Transport::e_UDP_IPV6_DATAGRAM) {
        error = ntsu::SocketUtil::create(client, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard clientGuard(*client);

        error = ntsu::SocketUtil::create(server, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard serverGuard(*server);

        error = ntsu::SocketUtil::bind(
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), 0)),
            true,
            *client);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::bind(
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), 0)),
            true,
            *server);
        if (error) {
            return error;
        }

        ntsa::Endpoint clientEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&clientEndpoint, *client);
        if (error) {
            return error;
        }

        ntsa::Endpoint serverEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint, *server);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::connect(serverEndpoint, *client);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::connect(clientEndpoint, *server);
        if (error) {
            return error;
        }

        clientGuard.release();
        serverGuard.release();
    }
    else if (type == ntsa::Transport::e_LOCAL_STREAM) {
        ntsa::Handle result[2];
        int          rc = ::socketpair(AF_UNIX, SOCK_STREAM, 0, result);
        if (rc != 0) {
            return ntsa::Error(errno);
        }

        *client = result[0];
        *server = result[1];
    }
    else if (type == ntsa::Transport::e_LOCAL_DATAGRAM) {
        ntsa::Handle result[2];
        int          rc = ::socketpair(AF_UNIX, SOCK_DGRAM, 0, result);
        if (rc != 0) {
            return ntsa::Error(errno);
        }

        *client = result[0];
        *server = result[1];
    }
    else {
        return ntsa::Error::invalid();
    }

    return ntsa::Error();
}

bsl::size_t SocketUtil::maxBuffersPerSend()
{
    return NTSU_SOCKETUTIL_MAX_BUFFERS_PER_SEND;
}

bsl::size_t SocketUtil::maxBuffersPerReceive()
{
    return NTSU_SOCKETUTIL_MAX_BUFFERS_PER_RECEIVE;
}

#if defined(BSLS_PLATFORM_OS_LINUX)
bsl::size_t SocketUtil::maxMessagesPerSend()
{
    return NTSU_SOCKETUTIL_MAX_MESSAGES_PER_SEND;
}

bsl::size_t SocketUtil::maxMessagesPerReceive()
{
    return NTSU_SOCKETUTIL_MAX_MESSAGES_PER_RECEIVE;
}
#endif

bsl::size_t SocketUtil::maxBacklog()
{
    return SOMAXCONN;
}

bool SocketUtil::isSocket(ntsa::Handle socket)
{
    struct ::stat stat;
    int           rc = ::fstat(socket, &stat);
    if (rc != 0) {
        return false;
    }
    return S_ISSOCK(stat.st_mode);
}

#if defined(BSLS_PLATFORM_OS_LINUX)

/// Provide a scanner of each line in the "/proc/net/tcp"
/// file. This class is not thread safe.
class Scanner
{
    const char* d_begin;
    const char* d_current;
    const char* d_end;

  private:
    Scanner(const Scanner&) BSLS_KEYWORD_DELETED;
    Scanner& operator=(const Scanner&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new scanner from the specified 'begin' until the specified
    /// 'end'.
    Scanner(const char* begin, const char* end);

    /// Destroy this object.
    ~Scanner();

    /// Skip spaces. Return the error.
    ntsa::Error skipSpaces();

    /// Skip the colon. Return the error.
    ntsa::Error skipColon();

    /// Parse an integer in the specified 'base'. Load into the specified
    /// 'result' the range of the integer. Return the error.
    ntsa::Error parseInteger(bslstl::StringRef* result, int base);

    /// Parse a 16-bit integer in the specified 'base'. Load into the
    /// specified 'result' the parsed value. Return the error.
    ntsa::Error parseInteger(bsl::uint16_t* result, int base);

    /// Parse a 32-bit integer in the specified 'base'. Load into the
    /// specified 'result' the parsed value. Return the error.
    ntsa::Error parseInteger(bsl::uint32_t* result, int base);
};

Scanner::Scanner(const char* begin, const char* end)
: d_begin(begin)
, d_current(begin)
, d_end(end)
{
    NTSCFG_WARNING_UNUSED(d_begin);
}

Scanner::~Scanner()
{
}

ntsa::Error Scanner::skipSpaces()
{
    while (d_current < d_end) {
        const char ch = *d_current;

        if (bdlb::CharType::isSpace(ch)) {
            ++d_current;
            continue;
        }

        break;
    }

    return ntsa::Error();
}

ntsa::Error Scanner::skipColon()
{
    if (d_current >= d_end) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const char ch = *d_current;

    if (ch != ':') {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ++d_current;

    return ntsa::Error();
}

ntsa::Error Scanner::parseInteger(bslstl::StringRef* result, int base)
{
    const char* begin = d_current;

    while (d_current < d_end) {
        const char ch = *d_current;

        if (base == 10) {
            if ((ch == '+' || ch == '-') && d_current == begin) {
                ++d_current;
                continue;
            }

            if (bdlb::CharType::isDigit(ch)) {
                ++d_current;
                continue;
            }
        }
        else if (base == 16) {
            if (bdlb::CharType::isXdigit(ch)) {
                ++d_current;
                continue;
            }
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        break;
    }

    const char* end = d_current;

    if (begin == end) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    result->assign(begin, end);

    return ntsa::Error();
}

ntsa::Error Scanner::parseInteger(bsl::uint16_t* result, int base)
{
    ntsa::Error error;

    bslstl::StringRef token;
    error = this->parseInteger(&token, base);
    if (error) {
        return error;
    }

    char* end;
    *result = NTSCFG_WARNING_NARROW(bsl::uint16_t,
                                    bsl::strtoul(token.begin(), &end, base));
    if (end != token.end()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Scanner::parseInteger(bsl::uint32_t* result, int base)
{
    ntsa::Error error;

    bslstl::StringRef token;
    error = this->parseInteger(&token, base);
    if (error) {
        return error;
    }

    char* end;
    *result = NTSCFG_WARNING_NARROW(bsl::uint32_t,
                                    bsl::strtoul(token.begin(), &end, base));
    if (end != token.end()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error reportInfoProcNetTcpLine(bsl::vector<ntsa::SocketInfo>* result,
                                     const char*                    line,
                                     bsl::size_t                    lineSize,
                                     int                            family,
                                     const bsls::TimeInterval&      now,
                                     bsl::uint32_t currentUserId,
                                     bool          all)
{
    NTSCFG_WARNING_UNUSED(now);

    ntsa::Error error;

    Scanner scanner(line, line + lineSize);

    ntsa::SocketInfo socketInfo;

    if (family == AF_INET) {
        socketInfo.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
    }
    else if (family == AF_INET6) {
        socketInfo.setTransport(ntsa::Transport::e_TCP_IPV6_STREAM);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = scanner.skipSpaces();
    if (error) {
        return error;
    }

    // Parse ID.

    bsl::uint32_t index;
    error = scanner.parseInteger(&index, 10);
    if (error) {
        return error;
    }

    error = scanner.skipColon();
    if (error) {
        return error;
    }

    // Parse source endpoint.

    error = scanner.skipSpaces();
    if (error) {
        return error;
    }

    if (family == AF_INET) {
        bsl::uint32_t address;
        error = scanner.parseInteger(&address, 16);
        if (error) {
            return error;
        }

        error = scanner.skipColon();
        if (error) {
            return error;
        }

        bsl::uint16_t port;
        error = scanner.parseInteger(&port, 16);
        if (error) {
            return error;
        }

        ntsa::Ipv4Address ipv4Address(address);

        ntsa::IpEndpoint ipEndpoint(ipv4Address, port);

        ntsa::Endpoint endpoint(ipEndpoint);

        socketInfo.setSourceEndpoint(endpoint);
    }
    else if (family == AF_INET6) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    // Parse remote endpoint.

    error = scanner.skipSpaces();
    if (error) {
        return error;
    }

    if (family == AF_INET) {
        bsl::uint32_t address;
        error = scanner.parseInteger(&address, 16);
        if (error) {
            return error;
        }

        error = scanner.skipColon();
        if (error) {
            return error;
        }

        bsl::uint16_t port;
        error = scanner.parseInteger(&port, 16);
        if (error) {
            return error;
        }

        ntsa::Ipv4Address ipv4Address(address);

        ntsa::IpEndpoint ipEndpoint(ipv4Address, port);

        ntsa::Endpoint endpoint(ipEndpoint);

        socketInfo.setRemoteEndpoint(endpoint);
    }
    else if (family == AF_INET6) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    // Parse connection state.

    error = scanner.skipSpaces();
    if (error) {
        return error;
    }

    {
        bsl::uint16_t connectionState;
        error = scanner.parseInteger(&connectionState, 16);
        if (error) {
            return error;
        }

        switch (connectionState) {
        case TCP_ESTABLISHED:
            socketInfo.setState(ntsa::SocketState::e_ESTABLISHED);
            break;
        case TCP_SYN_SENT:
            socketInfo.setState(ntsa::SocketState::e_SYN_SENT);
            break;
        case TCP_SYN_RECV:
            socketInfo.setState(ntsa::SocketState::e_SYN_RECV);
            break;
        case TCP_FIN_WAIT1:
            socketInfo.setState(ntsa::SocketState::e_FIN_WAIT1);
            break;
        case TCP_FIN_WAIT2:
            socketInfo.setState(ntsa::SocketState::e_FIN_WAIT2);
            break;
        case TCP_TIME_WAIT:
            socketInfo.setState(ntsa::SocketState::e_TIME_WAIT);
            break;
        case TCP_CLOSE:
            socketInfo.setState(ntsa::SocketState::e_CLOSED);
            break;
        case TCP_CLOSE_WAIT:
            socketInfo.setState(ntsa::SocketState::e_CLOSE_WAIT);
            break;
        case TCP_LAST_ACK:
            socketInfo.setState(ntsa::SocketState::e_LAST_ACK);
            break;
        case TCP_LISTEN:
            socketInfo.setState(ntsa::SocketState::e_LISTEN);
            break;
        case TCP_CLOSING:
            socketInfo.setState(ntsa::SocketState::e_CLOSING);
            break;
        default:
            socketInfo.setState(ntsa::SocketState::e_UNDEFINED);
        }
    }

    // Parse queues.

    error = scanner.skipSpaces();
    if (error) {
        return error;
    }

    {
        bsl::uint32_t sendQueueSize;
        error = scanner.parseInteger(&sendQueueSize, 10);
        if (error) {
            return error;
        }

        socketInfo.setSendQueueSize(sendQueueSize);
    }

    error = scanner.skipColon();
    if (error) {
        return error;
    }

    {
        bsl::uint32_t receiveQueueSize;
        error = scanner.parseInteger(&receiveQueueSize, 10);
        if (error) {
            return error;
        }

        socketInfo.setReceiveQueueSize(receiveQueueSize);
    }

    // Parse timer state.

    bsl::uint32_t timerActive            = 0;
    bsl::uint32_t timerJiffies           = 0;
    bsl::uint32_t unrecoveredRtoTimeouts = 0;

    error = scanner.skipSpaces();
    if (error) {
        return error;
    }

    {
        error = scanner.parseInteger(&timerActive, 10);
        if (error) {
            return error;
        }
    }

    error = scanner.skipColon();
    if (error) {
        return error;
    }

    {
        error = scanner.parseInteger(&timerJiffies, 10);
        if (error) {
            return error;
        }
    }

    error = scanner.skipSpaces();
    if (error) {
        return error;
    }

    {
        error = scanner.parseInteger(&unrecoveredRtoTimeouts, 10);
        if (error) {
            return error;
        }
    }

    // Parse user.

    bsl::uint32_t userId;

    error = scanner.skipSpaces();
    if (error) {
        return error;
    }

    {
        error = scanner.parseInteger(&userId, 10);
        if (error) {
            return error;
        }

        socketInfo.setUserId(userId);
    }

    // ...

    if (all || userId == currentUserId) {
        result->push_back(socketInfo);
    }

    return ntsa::Error();
}

ntsa::Error reportInfoProcNetUdpLine(bsl::vector<ntsa::SocketInfo>* result,
                                     const char*                    line,
                                     bsl::size_t                    lineSize,
                                     int                            family,
                                     const bsls::TimeInterval&      now,
                                     bsl::uint32_t currentUserId,
                                     bool          all)
{
    NTSCFG_WARNING_UNUSED(now);

    ntsa::Error error;

    Scanner scanner(line, line + lineSize);

    ntsa::SocketInfo socketInfo;

    if (family == AF_INET) {
        socketInfo.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    }
    else if (family == AF_INET6) {
        socketInfo.setTransport(ntsa::Transport::e_UDP_IPV6_DATAGRAM);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = scanner.skipSpaces();
    if (error) {
        return error;
    }

    // Parse ID.

    bsl::uint32_t index;
    error = scanner.parseInteger(&index, 10);
    if (error) {
        return error;
    }

    error = scanner.skipColon();
    if (error) {
        return error;
    }

    // Parse source endpoint.

    error = scanner.skipSpaces();
    if (error) {
        return error;
    }

    if (family == AF_INET) {
        bsl::uint32_t address;
        error = scanner.parseInteger(&address, 16);
        if (error) {
            return error;
        }

        error = scanner.skipColon();
        if (error) {
            return error;
        }

        bsl::uint16_t port;
        error = scanner.parseInteger(&port, 16);
        if (error) {
            return error;
        }

        ntsa::Ipv4Address ipv4Address(address);

        ntsa::IpEndpoint ipEndpoint(ipv4Address, port);

        ntsa::Endpoint endpoint(ipEndpoint);

        socketInfo.setSourceEndpoint(endpoint);
    }
    else if (family == AF_INET6) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    // Parse remote endpoint.

    error = scanner.skipSpaces();
    if (error) {
        return error;
    }

    if (family == AF_INET) {
        bsl::uint32_t address;
        error = scanner.parseInteger(&address, 16);
        if (error) {
            return error;
        }

        error = scanner.skipColon();
        if (error) {
            return error;
        }

        bsl::uint16_t port;
        error = scanner.parseInteger(&port, 16);
        if (error) {
            return error;
        }

        ntsa::Ipv4Address ipv4Address(address);

        ntsa::IpEndpoint ipEndpoint(ipv4Address, port);

        ntsa::Endpoint endpoint(ipEndpoint);

        socketInfo.setRemoteEndpoint(endpoint);
    }
    else if (family == AF_INET6) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    // Parse connection state.

    error = scanner.skipSpaces();
    if (error) {
        return error;
    }

    {
        bsl::uint16_t connectionState;
        error = scanner.parseInteger(&connectionState, 16);
        if (error) {
            return error;
        }

        switch (connectionState) {
        case TCP_ESTABLISHED:
            socketInfo.setState(ntsa::SocketState::e_ESTABLISHED);
            break;
        case TCP_SYN_SENT:
            socketInfo.setState(ntsa::SocketState::e_SYN_SENT);
            break;
        case TCP_SYN_RECV:
            socketInfo.setState(ntsa::SocketState::e_SYN_RECV);
            break;
        case TCP_FIN_WAIT1:
            socketInfo.setState(ntsa::SocketState::e_FIN_WAIT1);
            break;
        case TCP_FIN_WAIT2:
            socketInfo.setState(ntsa::SocketState::e_FIN_WAIT2);
            break;
        case TCP_TIME_WAIT:
            socketInfo.setState(ntsa::SocketState::e_TIME_WAIT);
            break;
        case TCP_CLOSE:
            socketInfo.setState(ntsa::SocketState::e_CLOSED);
            break;
        case TCP_CLOSE_WAIT:
            socketInfo.setState(ntsa::SocketState::e_CLOSE_WAIT);
            break;
        case TCP_LAST_ACK:
            socketInfo.setState(ntsa::SocketState::e_LAST_ACK);
            break;
        case TCP_LISTEN:
            socketInfo.setState(ntsa::SocketState::e_LISTEN);
            break;
        case TCP_CLOSING:
            socketInfo.setState(ntsa::SocketState::e_CLOSING);
            break;
        default:
            socketInfo.setState(ntsa::SocketState::e_UNDEFINED);
        }
    }

    // Parse queues.

    error = scanner.skipSpaces();
    if (error) {
        return error;
    }

    {
        bsl::uint32_t sendQueueSize;
        error = scanner.parseInteger(&sendQueueSize, 10);
        if (error) {
            return error;
        }

        socketInfo.setSendQueueSize(sendQueueSize);
    }

    error = scanner.skipColon();
    if (error) {
        return error;
    }

    {
        bsl::uint32_t receiveQueueSize;
        error = scanner.parseInteger(&receiveQueueSize, 10);
        if (error) {
            return error;
        }

        socketInfo.setReceiveQueueSize(receiveQueueSize);
    }

    // Parse timer state.

    bsl::uint32_t timerActive            = 0;
    bsl::uint32_t timerJiffies           = 0;
    bsl::uint32_t unrecoveredRtoTimeouts = 0;

    error = scanner.skipSpaces();
    if (error) {
        return error;
    }

    {
        error = scanner.parseInteger(&timerActive, 10);
        if (error) {
            return error;
        }
    }

    error = scanner.skipColon();
    if (error) {
        return error;
    }

    {
        error = scanner.parseInteger(&timerJiffies, 10);
        if (error) {
            return error;
        }
    }

    error = scanner.skipSpaces();
    if (error) {
        return error;
    }

    {
        error = scanner.parseInteger(&unrecoveredRtoTimeouts, 10);
        if (error) {
            return error;
        }
    }

    // Parse user.

    bsl::uint32_t userId;

    error = scanner.skipSpaces();
    if (error) {
        return error;
    }

    {
        error = scanner.parseInteger(&userId, 10);
        if (error) {
            return error;
        }

        socketInfo.setUserId(userId);
    }

    // ...

    if (all || userId == currentUserId) {
        result->push_back(socketInfo);
    }

    return ntsa::Error();
}

ntsa::Error reportInfoProcNetTcp(bsl::vector<ntsa::SocketInfo>* result,
                                 const ntsa::SocketInfoFilter&  filter,
                                 const char*                    fileName,
                                 int                            family)
{
    ntsa::Error error;

    bsls::TimeInterval now = bdlt::CurrentTime::now();

    bsl::uint32_t currentUserId = geteuid();

    bool all = false;
    if (!filter.all().isNull()) {
        all = filter.all().value();
    }

    FILE* file = bsl::fopen(fileName, "r");
    if (file == 0) {
        error = ntsa::Error(bsl::ferror(file));
        BSLS_LOG_ERROR("Failed to open '%s': %s",
                       fileName,
                       error.text().c_str());
        return error;
    }

    char line[256];

    if (bsl::fgets(line, sizeof(line), file) == 0) {
        error = ntsa::Error(bsl::ferror(file));
        BSLS_LOG_ERROR("Failed to read '%s': "
                       "failed to read header line: %s",
                       fileName,
                       error.text().c_str());

        bsl::fclose(file);
        return error;
    }

    while (bsl::fgets(line, sizeof(line), file) != 0) {
        bsl::size_t n = bsl::strlen(line);
        if (n == 0 || line[n - 1] != '\n') {
            break;
        }

        line[n - 1] = 0;

        error = reportInfoProcNetTcpLine(result,
                                         line,
                                         n - 1,
                                         family,
                                         now,
                                         currentUserId,
                                         all);
        if (error) {
            break;
        }
    }

    bsl::fclose(file);

    return ntsa::Error();
}

ntsa::Error reportInfoProcNetUdp(bsl::vector<ntsa::SocketInfo>* result,
                                 const ntsa::SocketInfoFilter&  filter,
                                 const char*                    fileName,
                                 int                            family)
{
    ntsa::Error error;

    bsls::TimeInterval now = bdlt::CurrentTime::now();

    bsl::uint32_t currentUserId = geteuid();

    bool all = false;
    if (!filter.all().isNull()) {
        all = filter.all().value();
    }

    FILE* file = bsl::fopen(fileName, "r");
    if (file == 0) {
        error = ntsa::Error(bsl::ferror(file));
        BSLS_LOG_ERROR("Failed to open '%s': %s",
                       fileName,
                       error.text().c_str());
        return error;
    }

    char line[256];

    if (bsl::fgets(line, sizeof(line), file) == 0) {
        error = ntsa::Error(bsl::ferror(file));
        BSLS_LOG_ERROR("Failed to read '%s': "
                       "failed to read header line: %s",
                       fileName,
                       error.text().c_str());

        bsl::fclose(file);
        return error;
    }

    while (bsl::fgets(line, sizeof(line), file) != 0) {
        bsl::size_t n = bsl::strlen(line);
        if (n == 0 || line[n - 1] != '\n') {
            break;
        }

        line[n - 1] = 0;

        error = reportInfoProcNetUdpLine(result,
                                         line,
                                         n - 1,
                                         family,
                                         now,
                                         currentUserId,
                                         all);
        if (error) {
            break;
        }
    }

    bsl::fclose(file);

    return ntsa::Error();
}

ntsa::Error reportInfoProcNetTcpIpv4(bsl::vector<ntsa::SocketInfo>* result,
                                     const ntsa::SocketInfoFilter&  filter)
{
    return reportInfoProcNetTcp(result, filter, "/proc/net/tcp", AF_INET);
}

ntsa::Error reportInfoProcNetTcpIpv6(bsl::vector<ntsa::SocketInfo>* result,
                                     const ntsa::SocketInfoFilter&  filter)
{
    return reportInfoProcNetTcp(result, filter, "/proc/net/tcp6", AF_INET6);
}

ntsa::Error reportInfoProcNetUdpIpv4(bsl::vector<ntsa::SocketInfo>* result,
                                     const ntsa::SocketInfoFilter&  filter)
{
    return reportInfoProcNetUdp(result, filter, "/proc/net/udp", AF_INET);
}

ntsa::Error reportInfoProcNetUdpIpv6(bsl::vector<ntsa::SocketInfo>* result,
                                     const ntsa::SocketInfoFilter&  filter)
{
    return reportInfoProcNetUdp(result, filter, "/proc/net/udp6", AF_INET6);
}

#endif

ntsa::Error SocketUtil::reportInfo(bsl::ostream&                 stream,
                                   const ntsa::SocketInfoFilter& filter)
{
    ntsa::Error error;

    bsl::vector<ntsa::SocketInfo> socketInfoList;
    error = ntsu::SocketUtil::reportInfo(&socketInfoList, filter);
    if (error) {
        return error;
    }

    stream << '\n';

    stream << bsl::setw(18) << bsl::left << "Transport";

    stream << bsl::setw(24) << bsl::left << "SourceEndpoint";

    stream << bsl::setw(24) << bsl::left << "RemoteEndpoint";

    stream << bsl::setw(16) << bsl::left << "State";

    stream << bsl::setw(16) << bsl::right << "TxQueueSize";

    stream << bsl::setw(16) << bsl::right << "RxQueueSize";

    stream << bsl::setw(12) << bsl::right << "User";

    for (bsl::vector<ntsa::SocketInfo>::const_iterator it =
             socketInfoList.begin();
         it != socketInfoList.end();
         ++it)
    {
        const ntsa::SocketInfo& socketInfo = *it;

        stream << '\n';

        stream << bsl::setw(18) << bsl::left << socketInfo.transport();

        stream << bsl::setw(24) << bsl::left << socketInfo.sourceEndpoint();

        stream << bsl::setw(24) << bsl::left << socketInfo.remoteEndpoint();

        stream << bsl::setw(16) << bsl::left << socketInfo.state();

        stream << bsl::setw(16) << bsl::right << socketInfo.sendQueueSize();

        stream << bsl::setw(16) << bsl::right << socketInfo.receiveQueueSize();

        stream << bsl::setw(12) << bsl::right << socketInfo.userId();
    }

    stream << '\n';
    stream.flush();

    return ntsa::Error();
}

ntsa::Error SocketUtil::reportInfo(bsl::vector<ntsa::SocketInfo>* result,
                                   const ntsa::SocketInfoFilter&  filter)
{
#if NTSU_SOCKETUTIL_DEBUG_HANDLE_MAP
    {
        bsls::SpinLockGuard guard(&s_handleSetLock);

        ntsa::Error error;

        for (HandleSet::const_iterator it = s_handleSet.begin();
             it != s_handleSet.end();
             ++it)
        {
            ntsa::Handle socket = *it;

            ntsa::Endpoint sourceEndpoint;
            error = ntsu::SocketUtil::sourceEndpoint(&sourceEndpoint, socket);
            if (error) {
                sourceEndpoint.reset();
            }

            ntsa::Endpoint remoteEndpoint;
            error = ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint, socket);
            if (error) {
                remoteEndpoint.reset();
            }

            if (!remoteEndpoint.isUndefined()) {
                BSLS_LOG_TRACE("Socket handle %d at '%s' to '%s' exists",
                               socket,
                               sourceEndpoint.text().c_str(),
                               remoteEndpoint.text().c_str());
            }
            else {
                BSLS_LOG_TRACE("Socket handle %d at '%s' exists",
                               socket,
                               sourceEndpoint.text().c_str());
            }
        }
    }
#endif

#if defined(BSLS_PLATFORM_OS_LINUX)

    ntsa::Error error;

    if (!filter.transport().isNull()) {
        if (filter.transport().value() == ntsa::Transport::e_LOCAL_STREAM ||
            filter.transport().value() == ntsa::Transport::e_LOCAL_DATAGRAM)
        {
            return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
        }
    }

    if (filter.transport().isNull() ||
        filter.transport().value() == ntsa::Transport::e_TCP_IPV4_STREAM)
    {
        error = reportInfoProcNetTcpIpv4(result, filter);
        if (error) {
            return error;
        }
    }

    if (filter.transport().isNull() ||
        filter.transport().value() == ntsa::Transport::e_TCP_IPV6_STREAM)
    {
        error = reportInfoProcNetTcpIpv6(result, filter);
        if (error) {
            return error;
        }
    }

    if (filter.transport().isNull() ||
        filter.transport().value() == ntsa::Transport::e_UDP_IPV4_DATAGRAM)
    {
        error = reportInfoProcNetUdpIpv4(result, filter);
        if (error) {
            return error;
        }
    }

    if (filter.transport().isNull() ||
        filter.transport().value() == ntsa::Transport::e_UDP_IPV6_DATAGRAM)
    {
        error = reportInfoProcNetUdpIpv6(result, filter);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();

#else

    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_WARNING_UNUSED(filter);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);

#endif
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

ntsa::Error SocketUtil::create(ntsa::Handle*          result,
                               ntsa::Transport::Value type)
{
    int domain;
    int mode;
    int protocol;

    if (type == ntsa::Transport::e_TCP_IPV4_STREAM) {
        domain   = AF_INET;
        mode     = SOCK_STREAM;
        protocol = IPPROTO_TCP;
    }
    else if (type == ntsa::Transport::e_TCP_IPV6_STREAM) {
        domain   = AF_INET6;
        mode     = SOCK_STREAM;
        protocol = IPPROTO_TCP;
    }
    else if (type == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
        domain   = AF_INET;
        mode     = SOCK_DGRAM;
        protocol = IPPROTO_UDP;
    }
    else if (type == ntsa::Transport::e_UDP_IPV6_DATAGRAM) {
        domain   = AF_INET6;
        mode     = SOCK_DGRAM;
        protocol = IPPROTO_UDP;
    }
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
    else if (type == ntsa::Transport::e_LOCAL_STREAM) {
        domain   = AF_UNIX;
        mode     = SOCK_STREAM;
        protocol = 0;
    }
    else if (type == ntsa::Transport::e_LOCAL_DATAGRAM) {
        domain   = AF_UNIX;
        mode     = SOCK_DGRAM;
        protocol = 0;
    }
#endif
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *result = WSASocket(domain, mode, protocol, 0, 0, WSA_FLAG_OVERLAPPED);
    if (*result == INVALID_SOCKET) {
        return ntsa::Error(WSAGetLastError());
    }

    if (domain == AF_INET6) {
        int value = 1;
        int rc    = ::setsockopt(*result,
                              IPPROTO_IPV6,
                              IPV6_V6ONLY,
                              (const char*)(&value),
                              sizeof value);
        if (rc != 0) {
            return ntsa::Error(WSAGetLastError());
        }
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::duplicate(ntsa::Handle* result, ntsa::Handle socket)
{
    int rc;

    WSAPROTOCOL_INFO protocolInfo;
    bsl::memset(&protocolInfo, 0, sizeof protocolInfo);

    rc = WSADuplicateSocket(socket, GetCurrentProcessId(), &protocolInfo);
    if (rc == SOCKET_ERROR) {
        return ntsa::Error(WSAGetLastError());
    }

    *result = WSASocket(protocolInfo.iAddressFamily,
                        protocolInfo.iSocketType,
                        protocolInfo.iProtocol,
                        &protocolInfo,
                        0,
                        WSA_FLAG_OVERLAPPED);

    if (*result == INVALID_SOCKET) {
        return ntsa::Error(WSAGetLastError());
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::bind(const ntsa::Endpoint& endpoint,
                             bool                  reuseAddress,
                             ntsa::Handle          socket)
{
    ntsa::Error error;

#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
    const bool isLocal = endpoint.isLocal();
#else
    const bool isLocal = false;
#endif

    if (!isLocal) {
        error = ntsu::SocketOptionUtil::setReuseAddress(socket, reuseAddress);
        if (error) {
            return error;
        }
    }

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    error = SocketStorageUtil::convert(&socketAddress,
                                       &socketAddressSize,
                                       endpoint);
    if (error) {
        return error;
    }

    if (endpoint.isIp() && endpoint.ip().host().isV6()) {
        sockaddr_in6* socketAddressIpv6 =
            reinterpret_cast<sockaddr_in6*>(&socketAddress);
        if (socketAddressIpv6->sin6_scope_id == 0) {
            socketAddressIpv6->sin6_scope_id =
                ntsu::AdapterUtil::discoverScopeId(endpoint.ip().host().v6());
        }
    }

#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
    if (endpoint.isLocal() && !reuseAddress) {
        if (!endpoint.local().isUnnamed() && !endpoint.local().isAbstract()) {
            bsl::string path = endpoint.local().value();
            BOOL        rc   = DeleteFile(path.c_str());
            if (rc == 0) {
                DWORD lastError = GetLastError();
                if (lastError != ERROR_FILE_NOT_FOUND) {
                    ntsa::Error error(errno);
                    BSLS_LOG_WARN("Failed to unlink %s: %s",
                                  path.c_str(),
                                  error.text().c_str());
                }
            }
        }
    }
#endif

    int rc = ::bind(socket,
                    reinterpret_cast<const sockaddr*>(&socketAddress),
                    socketAddressSize);

    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::bindAny(ntsa::Transport::Value type,
                                bool                   reuseAddress,
                                ntsa::Handle           socket)
{
    ntsa::Error error;

    ntsa::Endpoint endpoint;

    if (type == ntsa::Transport::e_TCP_IPV4_STREAM) {
        endpoint =
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::IpEndpoint::anyIpv4Address(),
                                            ntsa::IpEndpoint::anyPort()));
    }
    else if (type == ntsa::Transport::e_TCP_IPV6_STREAM) {
        endpoint =
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::IpEndpoint::anyIpv6Address(),
                                            ntsa::IpEndpoint::anyPort()));
    }
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
    else if (type == ntsa::Transport::e_LOCAL_STREAM) {
        ntsa::LocalName   localName;
        const ntsa::Error error = ntsa::LocalName::generateUnique(&localName);
        if (error) {
            return error;
        }
        endpoint.makeLocal(localName);
    }
#endif
    else if (type == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
        endpoint =
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::IpEndpoint::anyIpv4Address(),
                                            ntsa::IpEndpoint::anyPort()));
    }
    else if (type == ntsa::Transport::e_UDP_IPV6_DATAGRAM) {
        endpoint =
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::IpEndpoint::anyIpv6Address(),
                                            ntsa::IpEndpoint::anyPort()));
    }
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
    else if (type == ntsa::Transport::e_LOCAL_DATAGRAM) {
        ntsa::LocalName   localName;
        const ntsa::Error error = ntsa::LocalName::generateUnique(&localName);
        if (error) {
            return error;
        }
        endpoint.makeLocal(localName);
    }
#endif
    else {
        return ntsa::Error::invalid();
    }

    error = ntsu::SocketUtil::bind(endpoint, reuseAddress, socket);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::listen(bsl::size_t backlog, ntsa::Handle socket)
{
    int rc = ::listen(
        socket,
        backlog == 0 || backlog > SOMAXCONN ? -1 : static_cast<int>(backlog));
    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::connect(const ntsa::Endpoint& endpoint,
                                ntsa::Handle          socket)
{
    ntsa::Error error;
    int         rc;

    sockaddr_storage remoteSocketAddress;
    socklen_t        remoteSocketAddressSize;

    error = SocketStorageUtil::convert(&remoteSocketAddress,
                                       &remoteSocketAddressSize,
                                       endpoint);
    if (error) {
        return error;
    }

    rc = ::connect(socket,
                   reinterpret_cast<sockaddr*>(&remoteSocketAddress),
                   remoteSocketAddressSize);

    int lastError = WSAGetLastError();

#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
    if ((rc == 0) ||
        (lastError == WSAEINPROGRESS || lastError == WSAEWOULDBLOCK))
    {
        // Unlink the file name for explicitly bound, Unix domain sockets,
        // if necessary.
        ntsu::SocketUtil::unlink(socket);
    }
#endif

    if (rc != 0) {
        return ntsa::Error(lastError);
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::accept(ntsa::Handle* result, ntsa::Handle socket)
{
    *result = ::accept(socket, 0, 0);
    if (*result == INVALID_SOCKET) {
        return ntsa::Error(WSAGetLastError());
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::send(ntsa::SendContext*       context,
                             const void*              data,
                             bsl::size_t              size,
                             const ntsa::SendOptions& options,
                             ntsa::Handle             socket)
{
    context->reset();

    context->setBytesSendable(size);

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyControl  = false;

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        ntsa::Error error =
            SocketStorageUtil::convert(&socketAddress,
                                       &socketAddressSize,
                                       options.endpoint().value());
        if (error) {
            return error;
        }
    }

    WSABUF wsaBuf;
    wsaBuf.buf = const_cast<char*>(static_cast<const char*>(data));
    wsaBuf.len = static_cast<ULONG>(size);

    if (specifyControl) {
        WSAMSG msg;
        bsl::memset(&msg, 0, sizeof msg);

        if (specifyEndpoint) {
            msg.name    = reinterpret_cast<sockaddr*>(&socketAddress);
            msg.namelen = socketAddressSize;
        }

        msg.lpBuffers     = &wsaBuf;
        msg.dwBufferCount = 1;

        DWORD wsaNumBytesSent = 0;
        int   wsaSendMsgResult =
            WSASendMsg(socket, &msg, 0, &wsaNumBytesSent, 0, 0);

        if (wsaSendMsgResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else if (specifyEndpoint) {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendToResult =
            WSASendTo(socket,
                      &wsaBuf,
                      1,
                      &wsaNumBytesSent,
                      0,
                      reinterpret_cast<sockaddr*>(&socketAddress),
                      socketAddressSize,
                      0,
                      0);

        if (wsaSendToResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendResult =
            WSASend(socket, &wsaBuf, 1, &wsaNumBytesSent, 0, 0, 0);

        if (wsaSendResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::send(ntsa::SendContext*       context,
                             const ntsa::ConstBuffer& buffer,
                             const ntsa::SendOptions& options,
                             ntsa::Handle             socket)
{
    context->reset();

    context->setBytesSendable(buffer.size());

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyControl  = false;

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        ntsa::Error error =
            SocketStorageUtil::convert(&socketAddress,
                                       &socketAddressSize,
                                       options.endpoint().value());
        if (error) {
            return error;
        }
    }

    if (specifyControl) {
        WSAMSG msg;
        bsl::memset(&msg, 0, sizeof msg);

        if (specifyEndpoint) {
            msg.name    = reinterpret_cast<sockaddr*>(&socketAddress);
            msg.namelen = socketAddressSize;
        }

        msg.lpBuffers =
            reinterpret_cast<WSABUF*>(const_cast<ntsa::ConstBuffer*>(&buffer));
        msg.dwBufferCount = 1;

        DWORD wsaNumBytesSent = 0;
        int   wsaSendMsgResult =
            WSASendMsg(socket, &msg, 0, &wsaNumBytesSent, 0, 0);

        if (wsaSendMsgResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else if (specifyEndpoint) {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendToResult =
            WSASendTo(socket,
                      (WSABUF*)(&buffer),
                      1,
                      &wsaNumBytesSent,
                      0,
                      reinterpret_cast<sockaddr*>(&socketAddress),
                      socketAddressSize,
                      0,
                      0);

        if (wsaSendToResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendResult =
            WSASend(socket, (WSABUF*)(&buffer), 1, &wsaNumBytesSent, 0, 0, 0);

        if (wsaSendResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::send(ntsa::SendContext*       context,
                             const ntsa::ConstBuffer* bufferArray,
                             bsl::size_t              bufferCount,
                             const ntsa::SendOptions& options,
                             ntsa::Handle             socket)
{
    context->reset();

    bsl::size_t numBytesTotal =
        ntsa::ConstBuffer::totalSize(bufferArray, bufferCount);

    context->setBytesSendable(numBytesTotal);

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyControl  = false;

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        ntsa::Error error =
            SocketStorageUtil::convert(&socketAddress,
                                       &socketAddressSize,
                                       options.endpoint().value());
        if (error) {
            return error;
        }
    }

    if (specifyControl) {
        WSAMSG msg;
        bsl::memset(&msg, 0, sizeof msg);

        if (specifyEndpoint) {
            msg.name    = reinterpret_cast<sockaddr*>(&socketAddress);
            msg.namelen = socketAddressSize;
        }

        msg.lpBuffers = reinterpret_cast<WSABUF*>(
            const_cast<ntsa::ConstBuffer*>(bufferArray));
        msg.dwBufferCount = static_cast<ULONG>(bufferCount);

        DWORD wsaNumBytesSent = 0;
        int   wsaSendMsgResult =
            WSASendMsg(socket, &msg, 0, &wsaNumBytesSent, 0, 0);

        if (wsaSendMsgResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else if (specifyEndpoint) {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendToResult =
            WSASendTo(socket,
                      (WSABUF*)(bufferArray),
                      (DWORD)(bufferCount),
                      &wsaNumBytesSent,
                      0,
                      reinterpret_cast<sockaddr*>(&socketAddress),
                      socketAddressSize,
                      0,
                      0);

        if (wsaSendToResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendResult   = WSASend(socket,
                                    (WSABUF*)(bufferArray),
                                    (DWORD)(bufferCount),
                                    &wsaNumBytesSent,
                                    0,
                                    0,
                                    0);

        if (wsaSendResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::send(ntsa::SendContext*            context,
                             const ntsa::ConstBufferArray& bufferArray,
                             const ntsa::SendOptions&      options,
                             ntsa::Handle                  socket)
{
    context->reset();

    bsl::size_t numBuffersTotal = bufferArray.numBuffers();
    bsl::size_t numBytesTotal   = bufferArray.numBytes();

    context->setBytesSendable(numBytesTotal);

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyControl  = false;

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        ntsa::Error error =
            SocketStorageUtil::convert(&socketAddress,
                                       &socketAddressSize,
                                       options.endpoint().value());
        if (error) {
            return error;
        }
    }

    if (specifyControl) {
        WSAMSG msg;
        bsl::memset(&msg, 0, sizeof msg);

        if (specifyEndpoint) {
            msg.name    = reinterpret_cast<sockaddr*>(&socketAddress);
            msg.namelen = socketAddressSize;
        }

        msg.lpBuffers =
            reinterpret_cast<WSABUF*>(const_cast<void*>(bufferArray.base()));
        msg.dwBufferCount = static_cast<ULONG>(numBuffersTotal);

        DWORD wsaNumBytesSent = 0;
        int   wsaSendMsgResult =
            WSASendMsg(socket, &msg, 0, &wsaNumBytesSent, 0, 0);

        if (wsaSendMsgResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else if (specifyEndpoint) {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendToResult =
            WSASendTo(socket,
                      (WSABUF*)(bufferArray.base()),
                      NTSCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                      &wsaNumBytesSent,
                      0,
                      reinterpret_cast<sockaddr*>(&socketAddress),
                      socketAddressSize,
                      0,
                      0);

        if (wsaSendToResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendResult   = WSASend(socket,
                                    (WSABUF*)(bufferArray.base()),
                                    (DWORD)(numBuffersTotal),
                                    &wsaNumBytesSent,
                                    0,
                                    0,
                                    0);

        if (wsaSendResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::send(ntsa::SendContext*               context,
                             const ntsa::ConstBufferPtrArray& bufferArray,
                             const ntsa::SendOptions&         options,
                             ntsa::Handle                     socket)
{
    context->reset();

    bsl::size_t numBuffersTotal = bufferArray.numBuffers();
    bsl::size_t numBytesTotal   = bufferArray.numBytes();

    context->setBytesSendable(numBytesTotal);

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyControl  = false;

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        ntsa::Error error =
            SocketStorageUtil::convert(&socketAddress,
                                       &socketAddressSize,
                                       options.endpoint().value());
        if (error) {
            return error;
        }
    }

    if (specifyControl) {
        WSAMSG msg;
        bsl::memset(&msg, 0, sizeof msg);

        if (specifyEndpoint) {
            msg.name    = reinterpret_cast<sockaddr*>(&socketAddress);
            msg.namelen = socketAddressSize;
        }

        msg.lpBuffers =
            reinterpret_cast<WSABUF*>(const_cast<void*>(bufferArray.base()));
        msg.dwBufferCount = static_cast<ULONG>(numBuffersTotal);

        DWORD wsaNumBytesSent = 0;
        int   wsaSendMsgResult =
            WSASendMsg(socket, &msg, 0, &wsaNumBytesSent, 0, 0);

        if (wsaSendMsgResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else if (specifyEndpoint) {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendToResult =
            WSASendTo(socket,
                      (WSABUF*)(bufferArray.base()),
                      NTSCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                      &wsaNumBytesSent,
                      0,
                      reinterpret_cast<sockaddr*>(&socketAddress),
                      socketAddressSize,
                      0,
                      0);

        if (wsaSendToResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendResult   = WSASend(socket,
                                    (WSABUF*)(bufferArray.base()),
                                    (DWORD)(numBuffersTotal),
                                    &wsaNumBytesSent,
                                    0,
                                    0,
                                    0);

        if (wsaSendResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::send(ntsa::SendContext*         context,
                             const ntsa::MutableBuffer& buffer,
                             const ntsa::SendOptions&   options,
                             ntsa::Handle               socket)
{
    context->reset();

    context->setBytesSendable(buffer.size());

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyControl  = false;

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        ntsa::Error error =
            SocketStorageUtil::convert(&socketAddress,
                                       &socketAddressSize,
                                       options.endpoint().value());
        if (error) {
            return error;
        }
    }

    if (specifyControl) {
        WSAMSG msg;
        bsl::memset(&msg, 0, sizeof msg);

        if (specifyEndpoint) {
            msg.name    = reinterpret_cast<sockaddr*>(&socketAddress);
            msg.namelen = socketAddressSize;
        }

        msg.lpBuffers = reinterpret_cast<WSABUF*>(
            const_cast<ntsa::MutableBuffer*>(&buffer));
        msg.dwBufferCount = 1;

        DWORD wsaNumBytesSent = 0;
        int   wsaSendMsgResult =
            WSASendMsg(socket, &msg, 0, &wsaNumBytesSent, 0, 0);

        if (wsaSendMsgResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else if (specifyEndpoint) {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendToResult =
            WSASendTo(socket,
                      (WSABUF*)(&buffer),
                      1,
                      &wsaNumBytesSent,
                      0,
                      reinterpret_cast<sockaddr*>(&socketAddress),
                      socketAddressSize,
                      0,
                      0);

        if (wsaSendToResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendResult =
            WSASend(socket, (WSABUF*)(&buffer), 1, &wsaNumBytesSent, 0, 0, 0);

        if (wsaSendResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::send(ntsa::SendContext*         context,
                             const ntsa::MutableBuffer* bufferArray,
                             bsl::size_t                bufferCount,
                             const ntsa::SendOptions&   options,
                             ntsa::Handle               socket)
{
    context->reset();

    bsl::size_t numBytesTotal =
        ntsa::MutableBuffer::totalSize(bufferArray, bufferCount);

    context->setBytesSendable(numBytesTotal);

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyControl  = false;

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        ntsa::Error error =
            SocketStorageUtil::convert(&socketAddress,
                                       &socketAddressSize,
                                       options.endpoint().value());
        if (error) {
            return error;
        }
    }

    if (specifyControl) {
        WSAMSG msg;
        bsl::memset(&msg, 0, sizeof msg);

        if (specifyEndpoint) {
            msg.name    = reinterpret_cast<sockaddr*>(&socketAddress);
            msg.namelen = socketAddressSize;
        }

        msg.lpBuffers = reinterpret_cast<WSABUF*>(
            const_cast<ntsa::MutableBuffer*>(bufferArray));
        msg.dwBufferCount = static_cast<ULONG>(bufferCount);

        DWORD wsaNumBytesSent = 0;
        int   wsaSendMsgResult =
            WSASendMsg(socket, &msg, 0, &wsaNumBytesSent, 0, 0);

        if (wsaSendMsgResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else if (specifyEndpoint) {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendToResult =
            WSASendTo(socket,
                      (WSABUF*)(bufferArray),
                      (DWORD)(bufferCount),
                      &wsaNumBytesSent,
                      0,
                      reinterpret_cast<sockaddr*>(&socketAddress),
                      socketAddressSize,
                      0,
                      0);

        if (wsaSendToResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendResult   = WSASend(socket,
                                    (WSABUF*)(bufferArray),
                                    (DWORD)(bufferCount),
                                    &wsaNumBytesSent,
                                    0,
                                    0,
                                    0);

        if (wsaSendResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::send(ntsa::SendContext*              context,
                             const ntsa::MutableBufferArray& bufferArray,
                             const ntsa::SendOptions&        options,
                             ntsa::Handle                    socket)
{
    context->reset();

    bsl::size_t numBuffersTotal = bufferArray.numBuffers();
    bsl::size_t numBytesTotal   = bufferArray.numBytes();

    context->setBytesSendable(numBytesTotal);

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyControl  = false;

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        ntsa::Error error =
            SocketStorageUtil::convert(&socketAddress,
                                       &socketAddressSize,
                                       options.endpoint().value());
        if (error) {
            return error;
        }
    }

    if (specifyControl) {
        WSAMSG msg;
        bsl::memset(&msg, 0, sizeof msg);

        if (specifyEndpoint) {
            msg.name    = reinterpret_cast<sockaddr*>(&socketAddress);
            msg.namelen = socketAddressSize;
        }

        msg.lpBuffers =
            reinterpret_cast<WSABUF*>(const_cast<void*>(bufferArray.base()));
        msg.dwBufferCount = static_cast<ULONG>(numBuffersTotal);

        DWORD wsaNumBytesSent = 0;
        int   wsaSendMsgResult =
            WSASendMsg(socket, &msg, 0, &wsaNumBytesSent, 0, 0);

        if (wsaSendMsgResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else if (specifyEndpoint) {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendToResult =
            WSASendTo(socket,
                      (WSABUF*)(bufferArray.base()),
                      NTSCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                      &wsaNumBytesSent,
                      0,
                      reinterpret_cast<sockaddr*>(&socketAddress),
                      socketAddressSize,
                      0,
                      0);

        if (wsaSendToResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendResult   = WSASend(socket,
                                    (WSABUF*)(bufferArray.base()),
                                    (DWORD)(numBuffersTotal),
                                    &wsaNumBytesSent,
                                    0,
                                    0,
                                    0);

        if (wsaSendResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::send(ntsa::SendContext*                 context,
                             const ntsa::MutableBufferPtrArray& bufferArray,
                             const ntsa::SendOptions&           options,
                             ntsa::Handle                       socket)
{
    context->reset();

    bsl::size_t numBuffersTotal = bufferArray.numBuffers();
    bsl::size_t numBytesTotal   = bufferArray.numBytes();

    context->setBytesSendable(numBytesTotal);

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyControl  = false;

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        ntsa::Error error =
            SocketStorageUtil::convert(&socketAddress,
                                       &socketAddressSize,
                                       options.endpoint().value());
        if (error) {
            return error;
        }
    }

    if (specifyControl) {
        WSAMSG msg;
        bsl::memset(&msg, 0, sizeof msg);

        if (specifyEndpoint) {
            msg.name    = reinterpret_cast<sockaddr*>(&socketAddress);
            msg.namelen = socketAddressSize;
        }

        msg.lpBuffers =
            reinterpret_cast<WSABUF*>(const_cast<void*>(bufferArray.base()));
        msg.dwBufferCount = static_cast<ULONG>(numBuffersTotal);

        DWORD wsaNumBytesSent = 0;
        int   wsaSendMsgResult =
            WSASendMsg(socket, &msg, 0, &wsaNumBytesSent, 0, 0);

        if (wsaSendMsgResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else if (specifyEndpoint) {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendToResult =
            WSASendTo(socket,
                      (WSABUF*)(bufferArray.base()),
                      NTSCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                      &wsaNumBytesSent,
                      0,
                      reinterpret_cast<sockaddr*>(&socketAddress),
                      socketAddressSize,
                      0,
                      0);

        if (wsaSendToResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendResult   = WSASend(socket,
                                    (WSABUF*)(bufferArray.base()),
                                    (DWORD)(numBuffersTotal),
                                    &wsaNumBytesSent,
                                    0,
                                    0,
                                    0);

        if (wsaSendResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::send(ntsa::SendContext*       context,
                             const bslstl::StringRef& string,
                             const ntsa::SendOptions& options,
                             ntsa::Handle             socket)
{
    context->reset();

    bsl::size_t size = string.size();

    context->setBytesSendable(size);

    const void* data = string.data();

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyControl  = false;

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        ntsa::Error error =
            SocketStorageUtil::convert(&socketAddress,
                                       &socketAddressSize,
                                       options.endpoint().value());
        if (error) {
            return error;
        }
    }

    WSABUF wsaBuf;
    wsaBuf.buf = const_cast<char*>(static_cast<const char*>(data));
    wsaBuf.len = static_cast<ULONG>(size);

    if (specifyControl) {
        WSAMSG msg;
        bsl::memset(&msg, 0, sizeof msg);

        if (specifyEndpoint) {
            msg.name    = reinterpret_cast<sockaddr*>(&socketAddress);
            msg.namelen = socketAddressSize;
        }

        msg.lpBuffers     = &wsaBuf;
        msg.dwBufferCount = 1;

        DWORD wsaNumBytesSent = 0;
        int   wsaSendMsgResult =
            WSASendMsg(socket, &msg, 0, &wsaNumBytesSent, 0, 0);

        if (wsaSendMsgResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else if (specifyEndpoint) {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendToResult =
            WSASendTo(socket,
                      &wsaBuf,
                      1,
                      &wsaNumBytesSent,
                      0,
                      reinterpret_cast<sockaddr*>(&socketAddress),
                      socketAddressSize,
                      0,
                      0);

        if (wsaSendToResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendResult =
            WSASend(socket, &wsaBuf, 1, &wsaNumBytesSent, 0, 0, 0);

        if (wsaSendResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::send(ntsa::SendContext*       context,
                             const bdlbb::Blob&       blob,
                             const ntsa::SendOptions& options,
                             ntsa::Handle             socket)
{
    context->reset();

    bsl::size_t numBytesMax = options.maxBytes();
    if (numBytesMax == 0) {
        numBytesMax = SocketUtil::maxBytesPerSend(socket);
    }

    bsl::size_t numBuffersMax = options.maxBuffers();
    if (numBuffersMax == 0) {
        numBuffersMax = NTSU_SOCKETUTIL_MAX_BUFFERS_PER_SEND;
    }
    else if (numBuffersMax > NTSU_SOCKETUTIL_MAX_BUFFERS_PER_SEND) {
        numBuffersMax = NTSU_SOCKETUTIL_MAX_BUFFERS_PER_SEND;
    }

    WSABUF wsaBufArray[NTSU_SOCKETUTIL_MAX_BUFFERS_PER_SEND];

    bsl::size_t numBuffersTotal;
    bsl::size_t numBytesTotal;

    ntsu::BufferUtil::gather(&numBuffersTotal,
                             &numBytesTotal,
                             reinterpret_cast<ntsa::ConstBuffer*>(wsaBufArray),
                             numBuffersMax,
                             blob,
                             numBytesMax);

    context->setBytesSendable(numBytesTotal);

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyControl  = false;

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        ntsa::Error error =
            SocketStorageUtil::convert(&socketAddress,
                                       &socketAddressSize,
                                       options.endpoint().value());
        if (error) {
            return error;
        }
    }

    if (specifyControl) {
        WSAMSG msg;
        bsl::memset(&msg, 0, sizeof msg);

        if (specifyEndpoint) {
            msg.name    = reinterpret_cast<sockaddr*>(&socketAddress);
            msg.namelen = socketAddressSize;
        }

        msg.lpBuffers     = wsaBufArray;
        msg.dwBufferCount = static_cast<ULONG>(numBuffersTotal);

        DWORD wsaNumBytesSent = 0;
        int   wsaSendMsgResult =
            WSASendMsg(socket, &msg, 0, &wsaNumBytesSent, 0, 0);

        if (wsaSendMsgResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else if (specifyEndpoint) {
        DWORD wsaNumBytesSent = 0;

        int wsaSendToResult =
            WSASendTo(socket,
                      wsaBufArray,
                      NTSCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                      &wsaNumBytesSent,
                      0,
                      reinterpret_cast<sockaddr*>(&socketAddress),
                      socketAddressSize,
                      0,
                      0);

        if (wsaSendToResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);
        return ntsa::Error();
    }
    else {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendResult =
            WSASend(socket,
                    wsaBufArray,
                    NTSCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                    &wsaNumBytesSent,
                    0,
                    0,
                    0);

        if (wsaSendResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);
        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::send(ntsa::SendContext*       context,
                             const bdlbb::BlobBuffer& blobBuffer,
                             const ntsa::SendOptions& options,
                             ntsa::Handle             socket)
{
    NTSCFG_WARNING_UNUSED(options);

    context->reset();

    bsl::size_t size = blobBuffer.size();

    context->setBytesSendable(size);

    const void* data = blobBuffer.data();

    const bool specifyEndpoint = !options.endpoint().isNull();
    const bool specifyControl  = false;

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    if (specifyEndpoint) {
        ntsa::Error error =
            SocketStorageUtil::convert(&socketAddress,
                                       &socketAddressSize,
                                       options.endpoint().value());
        if (error) {
            return error;
        }
    }

    WSABUF wsaBuf;
    wsaBuf.buf = const_cast<char*>(static_cast<const char*>(data));
    wsaBuf.len = static_cast<ULONG>(size);

    if (specifyControl) {
        WSAMSG msg;
        bsl::memset(&msg, 0, sizeof msg);

        if (specifyEndpoint) {
            msg.name    = reinterpret_cast<sockaddr*>(&socketAddress);
            msg.namelen = socketAddressSize;
        }

        msg.lpBuffers     = &wsaBuf;
        msg.dwBufferCount = 1;

        DWORD wsaNumBytesSent = 0;
        int   wsaSendMsgResult =
            WSASendMsg(socket, &msg, 0, &wsaNumBytesSent, 0, 0);

        if (wsaSendMsgResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else if (specifyEndpoint) {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendToResult =
            WSASendTo(socket,
                      &wsaBuf,
                      1,
                      &wsaNumBytesSent,
                      0,
                      reinterpret_cast<sockaddr*>(&socketAddress),
                      socketAddressSize,
                      0,
                      0);

        if (wsaSendToResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
    else {
        DWORD wsaNumBytesSent = 0;
        int   wsaSendResult =
            WSASend(socket, &wsaBuf, 1, &wsaNumBytesSent, 0, 0, 0);

        if (wsaSendResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesSent(wsaNumBytesSent);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::send(ntsa::SendContext*       context,
                             const ntsa::File&        file,
                             const ntsa::SendOptions& options,
                             ntsa::Handle             socket)
{
    context->reset();

    if (!options.endpoint().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    DWORD size;
    if (file.size() <= bsl::numeric_limits<DWORD>::max()) {
        size = NTSCFG_WARNING_NARROW(DWORD, file.size());
    }
    else {
        size = bsl::numeric_limits<DWORD>::max();
    }

    bdls::FilesystemUtil::Offset offset = bdls::FilesystemUtil::seek(
        file.descriptor(),
        file.position(),
        bdls::FilesystemUtil::e_SEEK_FROM_BEGINNING);

    if (offset != file.position()) {
        return ntsa::Error(GetLastError());
    }

    context->setBytesSendable(size);

    BOOL transmitFileResult =
        TransmitFile(socket, file.descriptor(), size, 0, 0, 0, 0);

    if (!transmitFileResult) {
        return ntsa::Error(WSAGetLastError());
    }

    context->setBytesSent(size);

    return ntsa::Error();
}

ntsa::Error SocketUtil::send(ntsa::SendContext*       context,
                             const ntsa::Data&        data,
                             const ntsa::SendOptions& options,
                             ntsa::Handle             socket)
{
    if (NTSCFG_LIKELY(data.isBlob())) {
        return SocketUtil::send(context, data.blob(), options, socket);
    }
    else if (data.isSharedBlob()) {
        const bsl::shared_ptr<bdlbb::Blob>& blob = data.sharedBlob();
        if (blob) {
            return SocketUtil::send(context, *blob, options, socket);
        }
        else {
            context->reset();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (data.isBlobBuffer()) {
        return SocketUtil::send(context, data.blobBuffer(), options, socket);
    }
    else if (data.isConstBuffer()) {
        return SocketUtil::send(context, data.constBuffer(), options, socket);
    }
    else if (data.isConstBufferArray()) {
        return SocketUtil::send(context,
                                data.constBufferArray(),
                                options,
                                socket);
    }
    else if (data.isConstBufferPtrArray()) {
        return SocketUtil::send(context,
                                data.constBufferPtrArray(),
                                options,
                                socket);
    }
    else if (data.isMutableBuffer()) {
        return SocketUtil::send(context,
                                data.mutableBuffer(),
                                options,
                                socket);
    }
    else if (data.isMutableBufferArray()) {
        return SocketUtil::send(context,
                                data.mutableBufferArray(),
                                options,
                                socket);
    }
    else if (data.isMutableBufferPtrArray()) {
        return SocketUtil::send(context,
                                data.mutableBufferPtrArray(),
                                options,
                                socket);
    }
    else if (data.isString()) {
        return SocketUtil::send(context, data.string(), options, socket);
    }
    else if (data.isFile()) {
        return SocketUtil::send(context, data.file(), options, socket);
    }
    else {
        context->reset();
        return ntsa::Error::invalid();
    }
}

ntsa::Error SocketUtil::sendToMultiple(bsl::size_t* numBytesSendable,
                                       bsl::size_t* numBytesSent,
                                       bsl::size_t* numMessagesSendable,
                                       bsl::size_t* numMessagesSent,
                                       const ntsa::ConstMessage* messages,
                                       bsl::size_t               numMessages,
                                       ntsa::Handle              socket)
{
    NTSCFG_WARNING_UNUSED(messages);
    NTSCFG_WARNING_UNUSED(numMessages);
    NTSCFG_WARNING_UNUSED(socket);

    *numBytesSent = 0;

    if (numBytesSendable) {
        *numBytesSendable = 0;
    }

    *numMessagesSent = 0;

    if (numMessagesSendable) {
        *numMessagesSendable = 0;
    }

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*       context,
                                void*                       data,
                                bsl::size_t                 capacity,
                                const ntsa::ReceiveOptions& options,
                                ntsa::Handle                socket)
{
    NTSCFG_WARNING_UNUSED(options);

    context->reset();

    if (capacity == 0) {
        return ntsa::Error::invalid();
    }

    context->setBytesReceivable(capacity);

    const bool wantEndpoint = options.wantEndpoint();

    if (wantEndpoint) {
        sockaddr_storage socketAddress;
        socklen_t        socketAddressSize;

        SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

        WSABUF wsaBuf;
        wsaBuf.buf = static_cast<char*>(data);
        wsaBuf.len = static_cast<DWORD>(capacity);

        DWORD wsaNumBytesReceived = 0;
        DWORD wsaFlags            = 0;

        int wsaRecvFromResult =
            WSARecvFrom(socket,
                        &wsaBuf,
                        1,
                        &wsaNumBytesReceived,
                        &wsaFlags,
                        reinterpret_cast<sockaddr*>(&socketAddress),
                        &socketAddressSize,
                        0,
                        0);

        if (wsaRecvFromResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        ntsa::Endpoint endpoint;
        ntsa::Error    endpointError =
            SocketStorageUtil::convert(&endpoint,
                                       &socketAddress,
                                       socketAddressSize);
        if (endpointError) {
            endpointError =
                ntsu::SocketUtil::remoteEndpoint(&endpoint, socket);
        }

        if (!endpoint.isUndefined()) {
            context->setEndpoint(endpoint);
        }

        context->setBytesReceived(wsaNumBytesReceived);

        return ntsa::Error();
    }
    else {
        WSABUF wsaBuf;
        wsaBuf.buf = static_cast<char*>(data);
        wsaBuf.len = static_cast<ULONG>(capacity);

        DWORD wsaNumBytesReceived = 0;
        DWORD wsaFlags            = 0;

        int wsaRecvResult =
            WSARecv(socket, &wsaBuf, 1, &wsaNumBytesReceived, &wsaFlags, 0, 0);

        if (wsaRecvResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesReceived(wsaNumBytesReceived);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*       context,
                                ntsa::MutableBuffer*        buffer,
                                const ntsa::ReceiveOptions& options,
                                ntsa::Handle                socket)
{
    NTSCFG_WARNING_UNUSED(options);

    context->reset();

    bsl::size_t capacity = buffer->size();
    if (capacity == 0) {
        return ntsa::Error::invalid();
    }

    context->setBytesReceivable(capacity);

    const bool wantEndpoint = options.wantEndpoint();

    if (wantEndpoint) {
        sockaddr_storage socketAddress;
        socklen_t        socketAddressSize;

        SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

        DWORD wsaNumBytesReceived = 0;
        DWORD wsaFlags            = 0;

        int wsaRecvFromResult =
            WSARecvFrom(socket,
                        (WSABUF*)(buffer),
                        1,
                        &wsaNumBytesReceived,
                        &wsaFlags,
                        reinterpret_cast<sockaddr*>(&socketAddress),
                        &socketAddressSize,
                        0,
                        0);

        if (wsaRecvFromResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        ntsa::Endpoint endpoint;
        ntsa::Error    endpointError =
            SocketStorageUtil::convert(&endpoint,
                                       &socketAddress,
                                       socketAddressSize);
        if (endpointError) {
            endpointError =
                ntsu::SocketUtil::remoteEndpoint(&endpoint, socket);
        }

        if (!endpoint.isUndefined()) {
            context->setEndpoint(endpoint);
        }

        context->setBytesReceived(wsaNumBytesReceived);

        return ntsa::Error();
    }
    else {
        DWORD wsaNumBytesReceived = 0;
        DWORD wsaFlags            = 0;

        int wsaRecvResult = WSARecv(socket,
                                    (WSABUF*)(buffer),
                                    1,
                                    &wsaNumBytesReceived,
                                    &wsaFlags,
                                    0,
                                    0);

        if (wsaRecvResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesReceived(wsaNumBytesReceived);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*       context,
                                ntsa::MutableBuffer*        bufferArray,
                                bsl::size_t                 bufferCount,
                                const ntsa::ReceiveOptions& options,
                                ntsa::Handle                socket)
{
    NTSCFG_WARNING_UNUSED(options);

    context->reset();

    bsl::size_t capacity =
        ntsa::MutableBuffer::totalSize(bufferArray, bufferCount);
    if (capacity == 0) {
        return ntsa::Error::invalid();
    }

    context->setBytesReceivable(capacity);

    const bool wantEndpoint = options.wantEndpoint();

    if (wantEndpoint) {
        sockaddr_storage socketAddress;
        socklen_t        socketAddressSize;

        SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

        DWORD wsaNumBytesReceived = 0;
        DWORD wsaFlags            = 0;

        int wsaRecvFromResult =
            WSARecvFrom(socket,
                        (WSABUF*)(bufferArray),
                        NTSCFG_WARNING_NARROW(DWORD, bufferCount),
                        &wsaNumBytesReceived,
                        &wsaFlags,
                        reinterpret_cast<sockaddr*>(&socketAddress),
                        &socketAddressSize,
                        0,
                        0);

        if (wsaRecvFromResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        ntsa::Endpoint endpoint;
        ntsa::Error    endpointError =
            SocketStorageUtil::convert(&endpoint,
                                       &socketAddress,
                                       socketAddressSize);
        if (endpointError) {
            endpointError =
                ntsu::SocketUtil::remoteEndpoint(&endpoint, socket);
        }

        if (!endpoint.isUndefined()) {
            context->setEndpoint(endpoint);
        }

        context->setBytesReceived(wsaNumBytesReceived);

        return ntsa::Error();
    }
    else {
        DWORD wsaNumBytesReceived = 0;
        DWORD wsaFlags            = 0;

        int wsaRecvResult = WSARecv(socket,
                                    (WSABUF*)(bufferArray),
                                    (DWORD)(bufferCount),
                                    &wsaNumBytesReceived,
                                    &wsaFlags,
                                    0,
                                    0);

        if (wsaRecvResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesReceived(wsaNumBytesReceived);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*       context,
                                ntsa::MutableBufferArray*   bufferArray,
                                const ntsa::ReceiveOptions& options,
                                ntsa::Handle                socket)
{
    NTSCFG_WARNING_UNUSED(options);

    context->reset();

    bsl::size_t numBuffersTotal = bufferArray->numBuffers();
    if (numBuffersTotal == 0) {
        return ntsa::Error::invalid();
    }

    bsl::size_t numBytesTotal = bufferArray->numBytes();
    if (numBytesTotal == 0) {
        return ntsa::Error::invalid();
    }

    context->setBytesReceivable(numBytesTotal);

    const bool wantEndpoint = options.wantEndpoint();

    if (wantEndpoint) {
        sockaddr_storage socketAddress;
        socklen_t        socketAddressSize;

        SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

        DWORD wsaNumBytesReceived = 0;
        DWORD wsaFlags            = 0;

        int wsaRecvFromResult =
            WSARecvFrom(socket,
                        (WSABUF*)(bufferArray->base()),
                        NTSCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                        &wsaNumBytesReceived,
                        &wsaFlags,
                        reinterpret_cast<sockaddr*>(&socketAddress),
                        &socketAddressSize,
                        0,
                        0);

        if (wsaRecvFromResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        ntsa::Endpoint endpoint;
        ntsa::Error    endpointError =
            SocketStorageUtil::convert(&endpoint,
                                       &socketAddress,
                                       socketAddressSize);
        if (endpointError) {
            endpointError =
                ntsu::SocketUtil::remoteEndpoint(&endpoint, socket);
        }

        if (!endpoint.isUndefined()) {
            context->setEndpoint(endpoint);
        }

        context->setBytesReceived(wsaNumBytesReceived);

        return ntsa::Error();
    }
    else {
        DWORD wsaNumBytesReceived = 0;
        DWORD wsaFlags            = 0;

        int wsaRecvResult =
            WSARecv(socket,
                    (WSABUF*)(bufferArray->base()),
                    NTSCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                    &wsaNumBytesReceived,
                    &wsaFlags,
                    0,
                    0);

        if (wsaRecvResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesReceived(wsaNumBytesReceived);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*        context,
                                ntsa::MutableBufferPtrArray* bufferArray,
                                const ntsa::ReceiveOptions&  options,
                                ntsa::Handle                 socket)
{
    NTSCFG_WARNING_UNUSED(options);

    context->reset();

    bsl::size_t numBuffersTotal = bufferArray->numBuffers();
    if (numBuffersTotal == 0) {
        return ntsa::Error::invalid();
    }

    bsl::size_t numBytesTotal = bufferArray->numBytes();
    if (numBytesTotal == 0) {
        return ntsa::Error::invalid();
    }

    context->setBytesReceivable(numBytesTotal);

    const bool wantEndpoint = options.wantEndpoint();

    if (wantEndpoint) {
        sockaddr_storage socketAddress;
        socklen_t        socketAddressSize;

        SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

        DWORD wsaNumBytesReceived = 0;
        DWORD wsaFlags            = 0;

        int wsaRecvFromResult =
            WSARecvFrom(socket,
                        (WSABUF*)(bufferArray->base()),
                        NTSCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                        &wsaNumBytesReceived,
                        &wsaFlags,
                        reinterpret_cast<sockaddr*>(&socketAddress),
                        &socketAddressSize,
                        0,
                        0);

        if (wsaRecvFromResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        ntsa::Endpoint endpoint;
        ntsa::Error    endpointError =
            SocketStorageUtil::convert(&endpoint,
                                       &socketAddress,
                                       socketAddressSize);
        if (endpointError) {
            endpointError =
                ntsu::SocketUtil::remoteEndpoint(&endpoint, socket);
        }

        if (!endpoint.isUndefined()) {
            context->setEndpoint(endpoint);
        }

        context->setBytesReceived(wsaNumBytesReceived);

        return ntsa::Error();
    }
    else {
        DWORD wsaNumBytesReceived = 0;
        DWORD wsaFlags            = 0;

        int wsaRecvResult =
            WSARecv(socket,
                    (WSABUF*)(bufferArray->base()),
                    NTSCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                    &wsaNumBytesReceived,
                    &wsaFlags,
                    0,
                    0);

        if (wsaRecvResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesReceived(wsaNumBytesReceived);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*       context,
                                bsl::string*                string,
                                const ntsa::ReceiveOptions& options,
                                ntsa::Handle                socket)
{
    NTSCFG_WARNING_UNUSED(options);

    context->reset();

    bsl::size_t size     = string->size();
    bsl::size_t capacity = string->capacity() - size;
    if (capacity == 0) {
        return ntsa::Error::invalid();
    }

    context->setBytesReceivable(capacity);

    const bool wantEndpoint = options.wantEndpoint();

    if (wantEndpoint) {
        WSABUF wsaBuf;
        wsaBuf.buf = string->data() + size;
        wsaBuf.len = static_cast<DWORD>(capacity);

        sockaddr_storage socketAddress;
        socklen_t        socketAddressSize;

        SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

        DWORD wsaNumBytesReceived = 0;
        DWORD wsaFlags            = 0;

        int wsaRecvFromResult =
            WSARecvFrom(socket,
                        &wsaBuf,
                        1,
                        &wsaNumBytesReceived,
                        &wsaFlags,
                        reinterpret_cast<sockaddr*>(&socketAddress),
                        &socketAddressSize,
                        0,
                        0);

        if (wsaRecvFromResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        ntsa::Endpoint endpoint;
        ntsa::Error    endpointError =
            SocketStorageUtil::convert(&endpoint,
                                       &socketAddress,
                                       socketAddressSize);
        if (endpointError) {
            endpointError =
                ntsu::SocketUtil::remoteEndpoint(&endpoint, socket);
        }

        if (!endpoint.isUndefined()) {
            context->setEndpoint(endpoint);
        }

        context->setBytesReceived(wsaNumBytesReceived);
        string->resize(size + wsaNumBytesReceived);

        return ntsa::Error();
    }
    else {
        WSABUF wsaBuf;
        wsaBuf.buf = string->data() + size;
        wsaBuf.len = static_cast<DWORD>(capacity);

        DWORD wsaNumBytesReceived = 0;
        DWORD wsaFlags            = 0;

        int wsaRecvResult =
            WSARecv(socket, &wsaBuf, 1, &wsaNumBytesReceived, &wsaFlags, 0, 0);

        if (wsaRecvResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesReceived(wsaNumBytesReceived);
        string->resize(size + wsaNumBytesReceived);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*       context,
                                bdlbb::Blob*                blob,
                                const ntsa::ReceiveOptions& options,
                                ntsa::Handle                socket)
{
    NTSCFG_WARNING_UNUSED(options);

    context->reset();

    bsl::size_t numBytesMax = options.maxBytes();
    if (numBytesMax == 0) {
        numBytesMax = SocketUtil::maxBytesPerReceive(socket);
    }

    bsl::size_t numBuffersMax = options.maxBuffers();
    if (numBuffersMax == 0) {
        numBuffersMax = NTSU_SOCKETUTIL_MAX_BUFFERS_PER_RECEIVE;
    }
    else if (numBuffersMax > NTSU_SOCKETUTIL_MAX_BUFFERS_PER_RECEIVE) {
        numBuffersMax = NTSU_SOCKETUTIL_MAX_BUFFERS_PER_RECEIVE;
    }

    bsl::size_t size     = blob->length();
    bsl::size_t capacity = blob->totalSize() - size;
    if (capacity == 0) {
        return ntsa::Error::invalid();
    }

    WSABUF wsaBufArray[NTSU_SOCKETUTIL_MAX_BUFFERS_PER_RECEIVE];

    bsl::size_t numBuffersTotal;
    bsl::size_t numBytesTotal;

    ntsu::BufferUtil::scatter(
        &numBuffersTotal,
        &numBytesTotal,
        reinterpret_cast<ntsa::MutableBuffer*>(wsaBufArray),
        numBuffersMax,
        blob,
        numBytesMax);

    context->setBytesReceivable(numBytesTotal);

    const bool wantEndpoint = options.wantEndpoint();

    if (wantEndpoint) {
        sockaddr_storage socketAddress;
        socklen_t        socketAddressSize;

        SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

        DWORD wsaNumBytesReceived = 0;
        DWORD wsaFlags            = 0;

        int wsaRecvFromResult =
            WSARecvFrom(socket,
                        wsaBufArray,
                        NTSCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                        &wsaNumBytesReceived,
                        &wsaFlags,
                        reinterpret_cast<sockaddr*>(&socketAddress),
                        &socketAddressSize,
                        0,
                        0);

        if (wsaRecvFromResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        ntsa::Endpoint endpoint;
        ntsa::Error    endpointError =
            SocketStorageUtil::convert(&endpoint,
                                       &socketAddress,
                                       socketAddressSize);
        if (endpointError) {
            endpointError =
                ntsu::SocketUtil::remoteEndpoint(&endpoint, socket);
        }

        if (!endpoint.isUndefined()) {
            context->setEndpoint(endpoint);
        }

        context->setBytesReceived(wsaNumBytesReceived);
        blob->setLength(blob->length() + wsaNumBytesReceived);

        return ntsa::Error();
    }
    else {
        DWORD wsaNumBytesReceived = 0;
        DWORD wsaFlags            = 0;

        int wsaRecvResult =
            WSARecv(socket,
                    wsaBufArray,
                    NTSCFG_WARNING_NARROW(DWORD, numBuffersTotal),
                    &wsaNumBytesReceived,
                    &wsaFlags,
                    0,
                    0);

        if (wsaRecvResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesReceived(wsaNumBytesReceived);
        blob->setLength(blob->length() + wsaNumBytesReceived);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*       context,
                                bdlbb::BlobBuffer*          blobBuffer,
                                const ntsa::ReceiveOptions& options,
                                ntsa::Handle                socket)
{
    NTSCFG_WARNING_UNUSED(options);

    context->reset();

    bsl::size_t capacity = blobBuffer->size();
    if (capacity == 0) {
        return ntsa::Error::invalid();
    }

    context->setBytesReceivable(capacity);

    void* data = blobBuffer->data();

    const bool wantEndpoint = options.wantEndpoint();

    if (wantEndpoint) {
        sockaddr_storage socketAddress;
        socklen_t        socketAddressSize;

        SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

        WSABUF wsaBuf;
        wsaBuf.buf = static_cast<char*>(data);
        wsaBuf.len = static_cast<DWORD>(capacity);

        DWORD wsaNumBytesReceived = 0;
        DWORD wsaFlags            = 0;

        int wsaRecvFromResult =
            WSARecvFrom(socket,
                        &wsaBuf,
                        1,
                        &wsaNumBytesReceived,
                        &wsaFlags,
                        reinterpret_cast<sockaddr*>(&socketAddress),
                        &socketAddressSize,
                        0,
                        0);

        if (wsaRecvFromResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        ntsa::Endpoint endpoint;
        ntsa::Error    endpointError =
            SocketStorageUtil::convert(&endpoint,
                                       &socketAddress,
                                       socketAddressSize);
        if (endpointError) {
            endpointError =
                ntsu::SocketUtil::remoteEndpoint(&endpoint, socket);
        }

        if (!endpoint.isUndefined()) {
            context->setEndpoint(endpoint);
        }

        context->setBytesReceived(wsaNumBytesReceived);

        return ntsa::Error();
    }
    else {
        WSABUF wsaBuf;
        wsaBuf.buf = static_cast<char*>(data);
        wsaBuf.len = static_cast<ULONG>(capacity);

        DWORD wsaNumBytesReceived = 0;
        DWORD wsaFlags            = 0;

        int wsaRecvResult =
            WSARecv(socket, &wsaBuf, 1, &wsaNumBytesReceived, &wsaFlags, 0, 0);

        if (wsaRecvResult != 0) {
            return ntsa::Error(WSAGetLastError());
        }

        context->setBytesReceived(wsaNumBytesReceived);

        return ntsa::Error();
    }
}

ntsa::Error SocketUtil::receive(ntsa::ReceiveContext*       context,
                                ntsa::Data*                 data,
                                const ntsa::ReceiveOptions& options,
                                ntsa::Handle                socket)
{
    if (NTSCFG_LIKELY(data->isBlob())) {
        return SocketUtil::receive(context, &data->blob(), options, socket);
    }
    else if (data->isSharedBlob()) {
        bsl::shared_ptr<bdlbb::Blob>& blob = data->sharedBlob();
        if (blob) {
            return SocketUtil::receive(context, blob.get(), options, socket);
        }
        else {
            context->reset();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (data->isBlobBuffer()) {
        return SocketUtil::receive(context,
                                   &data->blobBuffer(),
                                   options,
                                   socket);
    }
    else if (data->isMutableBuffer()) {
        return SocketUtil::receive(context,
                                   &data->mutableBuffer(),
                                   options,
                                   socket);
    }
    else if (data->isMutableBufferArray()) {
        return SocketUtil::receive(context,
                                   &data->mutableBufferArray(),
                                   options,
                                   socket);
    }
    else if (data->isMutableBufferPtrArray()) {
        return SocketUtil::receive(context,
                                   &data->mutableBufferPtrArray(),
                                   options,
                                   socket);
    }
    else if (data->isString()) {
        return SocketUtil::receive(context, &data->string(), options, socket);
    }
    else {
        context->reset();
        return ntsa::Error::invalid();
    }
}

ntsa::Error SocketUtil::receiveFromMultiple(bsl::size_t* numBytesReceivable,
                                            bsl::size_t* numBytesReceived,
                                            bsl::size_t* numMessagesReceivable,
                                            bsl::size_t* numMessagesReceived,
                                            ntsa::MutableMessage* messages,
                                            bsl::size_t           numMessages,
                                            ntsa::Handle          socket)
{
    NTSCFG_WARNING_UNUSED(messages);
    NTSCFG_WARNING_UNUSED(numMessages);
    NTSCFG_WARNING_UNUSED(socket);

    *numBytesReceived = 0;

    if (numBytesReceivable) {
        *numBytesReceivable = 0;
    }

    *numMessagesReceived = 0;

    if (numMessagesReceivable) {
        *numMessagesReceivable = 0;
    }

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error SocketUtil::receiveNotifications(
    ntsa::NotificationQueue* notifications,
    ntsa::Handle             socket)
{
    NTSCFG_WARNING_UNUSED(notifications);
    NTSCFG_WARNING_UNUSED(socket);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error SocketUtil::shutdown(ntsa::ShutdownType::Value direction,
                                 ntsa::Handle              socket)
{
    int how = direction == ntsa::ShutdownType::e_SEND      ? SD_SEND
              : direction == ntsa::ShutdownType::e_RECEIVE ? SD_RECEIVE
                                                           : SD_BOTH;

    int rc = ::shutdown(socket, how);
    if (rc != 0) {
        DWORD lastError = WSAGetLastError();
        if (lastError != WSAEINVAL && lastError != WSAENOTCONN) {
            return ntsa::Error(lastError);
        }
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::unlink(ntsa::Handle socket)
{
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL

    // Unlink 'socket' as in the Unix implementation, but call 'DeleteFile'
    // instead of 'unlink'.

    int rc;

    if (socket == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error();
    }

    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

    rc = ::getsockname(socket,
                       reinterpret_cast<sockaddr*>(&socketAddress),
                       &socketAddressSize);
    if (rc == 0) {
        if (socketAddress.ss_family == AF_UNIX) {
            const sockaddr_un* socketAddressLocal =
                reinterpret_cast<const sockaddr_un*>(&socketAddress);

            if (socketAddressLocal->sun_path[0] != 0) {
                const bsl::size_t pathOffset =
                    offsetof(struct sockaddr_un, sun_path);
                if (socketAddressSize > pathOffset) {
                    bsl::size_t pathLength = socketAddressSize - pathOffset;
                    bsl::string path(socketAddressLocal->sun_path, pathLength);

                    if (!path.empty()) {
                        // BSLS_LOG_TRACE("Unlinking %s", path.c_str());
                        BOOL result = DeleteFile(path.c_str());
                        if (result == 0) {
                            DWORD lastError = GetLastError();
                            if (lastError != ERROR_FILE_NOT_FOUND) {
                                ntsa::Error error(errno);
                                BSLS_LOG_WARN("Failed to unlink %s: %s",
                                              path.c_str(),
                                              error.text().c_str());
                            }
                        }
                    }
                }
            }
        }
    }

    return ntsa::Error();

#else

    return ntsa::Error();

#endif
}

ntsa::Error SocketUtil::close(ntsa::Handle socket)
{
    int rc;

    if (socket == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error();
    }

#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL

    bool unlinkFlag = false;

    {
        int       optionValue;
        socklen_t optionSize = sizeof(optionValue);

        rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_TYPE,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

        if (rc == 0) {
            if (optionValue == SOCK_DGRAM) {
                unlinkFlag = true;
            }
        }
    }

    if (!unlinkFlag) {
        int       optionValue;
        socklen_t optionSize = sizeof(optionValue);

        rc = ::getsockopt(socket,
                          SOL_SOCKET,
                          SO_ACCEPTCONN,
                          reinterpret_cast<char*>(&optionValue),
                          &optionSize);
        if (rc == 0) {
            if (optionValue != 0) {
                unlinkFlag = true;
            }
        }
        else {
            rc = ::listen(socket, SOMAXCONN);
            if (rc == 0) {
                unlinkFlag = true;
            }
        }
    }

    if (unlinkFlag) {
        ntsu::SocketUtil::unlink(socket);
    }

#endif

    rc = ::closesocket(socket);
    if (rc != 0) {
        ntsa::Error error(WSAGetLastError());
        return error;
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::sourceEndpoint(ntsa::Endpoint* result,
                                       ntsa::Handle    socket)
{
    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

    int rc = ::getsockname(socket,
                           reinterpret_cast<sockaddr*>(&socketAddress),
                           &socketAddressSize);
    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    ntsa::Error error =
        SocketStorageUtil::convert(result, &socketAddress, socketAddressSize);
    if (error) {
        return error;
    }

    if (result->isIp() && result->ip().host().isV6()) {
        if (result->ip().host().v6().scopeId() == 0) {
            ntsa::Ipv6ScopeId scopeId =
                ntsu::AdapterUtil::discoverScopeId(result->ip().host().v6());
            if (scopeId != 0) {
                ntsa::Ipv6Address host(result->ip().host().v6());
                host.setScopeId(scopeId);
                result->ip().setHost(host);
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::remoteEndpoint(ntsa::Endpoint* result,
                                       ntsa::Handle    socket)
{
    sockaddr_storage socketAddress;
    socklen_t        socketAddressSize;

    SocketStorageUtil::initialize(&socketAddress, &socketAddressSize);

    int rc = ::getpeername(socket,
                           reinterpret_cast<sockaddr*>(&socketAddress),
                           &socketAddressSize);
    if (rc != 0) {
        return ntsa::Error(WSAGetLastError());
    }

    ntsa::Error error =
        SocketStorageUtil::convert(result, &socketAddress, socketAddressSize);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error SocketUtil::waitUntilReadable(ntsa::Handle socket)
{
    WSAPOLLFD pfd;

    pfd.fd      = socket;
    pfd.events  = POLLIN | POLLHUP | POLLERR | POLLNVAL;
    pfd.revents = 0;

    int rc = WSAPoll(&pfd, 1, -1);
    if (rc < 0) {
        return ntsa::Error(WSAGetLastError());
    }

    if (rc == 0) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    if (((pfd.revents & POLLIN) != 0) || ((pfd.revents & POLLHUP) != 0)) {
        return ntsa::Error();
    }

    return ntsa::Error::invalid();
}

ntsa::Error SocketUtil::waitUntilReadable(ntsa::Handle              socket,
                                          const bsls::TimeInterval& timeout)
{
    WSAPOLLFD pfd;

    pfd.fd      = socket;
    pfd.events  = POLLIN | POLLHUP | POLLERR | POLLNVAL;
    pfd.revents = 0;

    bsls::TimeInterval now = bdlt::CurrentTime::now();

    bsls::TimeInterval delta;
    if (timeout > now) {
        delta = timeout - now;
    }

    bsl::int64_t milliseconds =
        static_cast<bsl::int64_t>(delta.totalMilliseconds());

    if (milliseconds > bsl::numeric_limits<int>::max()) {
        milliseconds = bsl::numeric_limits<int>::max();
    }

    int rc = WSAPoll(&pfd, 1, static_cast<int>(milliseconds));
    if (rc < 0) {
        return ntsa::Error(WSAGetLastError());
    }

    if (rc == 0) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    if (((pfd.revents & POLLIN) != 0) || ((pfd.revents & POLLHUP) != 0)) {
        return ntsa::Error();
    }

    return ntsa::Error::invalid();
}

ntsa::Error SocketUtil::waitUntilWritable(ntsa::Handle socket)
{
    WSAPOLLFD pfd;

    pfd.fd      = socket;
    pfd.events  = POLLOUT | POLLERR | POLLNVAL;
    pfd.revents = 0;

    int rc = WSAPoll(&pfd, 1, -1);
    if (rc < 0) {
        return ntsa::Error(WSAGetLastError());
    }

    if (rc == 0) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    if ((pfd.revents & POLLOUT) != 0) {
        return ntsa::Error();
    }

    return ntsa::Error::invalid();
}

ntsa::Error SocketUtil::waitUntilWritable(ntsa::Handle              socket,
                                          const bsls::TimeInterval& timeout)
{
    WSAPOLLFD pfd;

    pfd.fd      = socket;
    pfd.events  = POLLOUT | POLLERR | POLLNVAL;
    pfd.revents = 0;

    bsls::TimeInterval now = bdlt::CurrentTime::now();

    bsls::TimeInterval delta;
    if (timeout > now) {
        delta = timeout - now;
    }

    bsl::int64_t milliseconds =
        static_cast<bsl::int64_t>(delta.totalMilliseconds());

    if (milliseconds > bsl::numeric_limits<int>::max()) {
        milliseconds = bsl::numeric_limits<int>::max();
    }

    int rc = WSAPoll(&pfd, 1, static_cast<int>(milliseconds));
    if (rc < 0) {
        return ntsa::Error(WSAGetLastError());
    }

    if (rc == 0) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    if ((pfd.revents & POLLOUT) != 0) {
        return ntsa::Error();
    }

    return ntsa::Error::invalid();
}

ntsa::Error SocketUtil::pair(ntsa::Handle*          client,
                             ntsa::Handle*          server,
                             ntsa::Transport::Value type)
{
    ntsa::Error error;

    if (type == ntsa::Transport::e_TCP_IPV4_STREAM) {
        ntsa::Handle listener;
        error = ntsu::SocketUtil::create(&listener, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard listenerGuard(listener);

        error = ntsu::SocketUtil::bind(
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
            false,
            listener);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::listen(1, listener);
        if (error) {
            return error;
        }

        ntsa::Endpoint listenerEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&listenerEndpoint, listener);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::create(client, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard clientGuard(*client);

        error = ntsu::SocketUtil::connect(listenerEndpoint, *client);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::accept(server, listener);
        if (error) {
            return error;
        }

        clientGuard.release();
    }
    else if (type == ntsa::Transport::e_TCP_IPV6_STREAM) {
        ntsa::Handle listener;
        error = ntsu::SocketUtil::create(&listener, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard listenerGuard(listener);

        error = ntsu::SocketUtil::bind(
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), 0)),
            false,
            listener);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::listen(1, listener);
        if (error) {
            return error;
        }

        ntsa::Endpoint listenerEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&listenerEndpoint, listener);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::create(client, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard clientGuard(*client);

        error = ntsu::SocketUtil::connect(listenerEndpoint, *client);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::accept(server, listener);
        if (error) {
            return error;
        }

        clientGuard.release();
    }
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
    else if (type == ntsa::Transport::e_LOCAL_STREAM) {
        ntsa::Handle listener;
        error = ntsu::SocketUtil::create(&listener, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard listenerGuard(listener);

        {
            ntsa::LocalName localName;
            error = ntsa::LocalName::generateUnique(&localName);
            if (error) {
                return error;
            }

            error = ntsu::SocketUtil::bind(ntsa::Endpoint(localName),
                                           false,
                                           listener);
            if (error) {
                return error;
            }
        }

        error = ntsu::SocketUtil::listen(1, listener);
        if (error) {
            return error;
        }

        ntsa::Endpoint listenerEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&listenerEndpoint, listener);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::create(client, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard clientGuard(*client);

        {
            ntsa::LocalName localName;
            error = ntsa::LocalName::generateUnique(&localName);
            if (error) {
                return error;
            }

            error = ntsu::SocketUtil::bind(ntsa::Endpoint(localName),
                                           false,
                                           *client);
            if (error) {
                return error;
            }
        }

        error = ntsu::SocketUtil::connect(listenerEndpoint, *client);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::accept(server, listener);
        if (error) {
            return error;
        }

        clientGuard.release();
    }
#endif
    else if (type == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
        error = ntsu::SocketUtil::create(client, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard clientGuard(*client);

        error = ntsu::SocketUtil::create(server, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard serverGuard(*server);

        error = ntsu::SocketUtil::bind(
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
            false,
            *client);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::bind(
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)),
            false,
            *server);
        if (error) {
            return error;
        }

        ntsa::Endpoint clientEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&clientEndpoint, *client);
        if (error) {
            return error;
        }

        ntsa::Endpoint serverEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint, *server);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::connect(serverEndpoint, *client);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::connect(clientEndpoint, *server);
        if (error) {
            return error;
        }

        clientGuard.release();
        serverGuard.release();
    }
    else if (type == ntsa::Transport::e_UDP_IPV6_DATAGRAM) {
        error = ntsu::SocketUtil::create(client, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard clientGuard(*client);

        error = ntsu::SocketUtil::create(server, type);
        if (error) {
            return error;
        }

        ntsu::SocketUtil::Guard serverGuard(*server);

        error = ntsu::SocketUtil::bind(
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), 0)),
            false,
            *client);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::bind(
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), 0)),
            false,
            *server);
        if (error) {
            return error;
        }

        ntsa::Endpoint clientEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&clientEndpoint, *client);
        if (error) {
            return error;
        }

        ntsa::Endpoint serverEndpoint;
        error = ntsu::SocketUtil::sourceEndpoint(&serverEndpoint, *server);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::connect(serverEndpoint, *client);
        if (error) {
            return error;
        }

        error = ntsu::SocketUtil::connect(clientEndpoint, *server);
        if (error) {
            return error;
        }

        clientGuard.release();
        serverGuard.release();
    }
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
    else if (type == ntsa::Transport::e_UDP_IPV6_DATAGRAM) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }
#endif
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

bsl::size_t SocketUtil::maxBuffersPerSend()
{
    return NTSU_SOCKETUTIL_LIMIT_MAX_BUFFERS_PER_SEND;
}

bsl::size_t SocketUtil::maxBuffersPerReceive()
{
    return NTSU_SOCKETUTIL_LIMIT_MAX_BUFFERS_PER_RECEIVE;
}

bsl::size_t SocketUtil::maxBacklog()
{
    return SOMAXCONN;
}

bool SocketUtil::isSocket(ntsa::Handle socket)
{
    DWORD     optionValue;
    socklen_t optionSize = sizeof(optionValue);

    int rc = getsockopt(socket,
                        SOL_SOCKET,
                        SO_TYPE,
                        reinterpret_cast<char*>(&optionValue),
                        &optionSize);

    if (rc != 0) {
        return false;
    }

    if (optionValue != SOCK_STREAM && optionValue != SOCK_DGRAM) {
        return false;
    }

    return true;
}

ntsa::Error SocketUtil::reportInfo(bsl::ostream&                 stream,
                                   const ntsa::SocketInfoFilter& filter)
{
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error SocketUtil::reportInfo(bsl::vector<ntsa::SocketInfo>* result,
                                   const ntsa::SocketInfoFilter&  filter)
{
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

#else
#error Not implemented
#endif

ntsa::Error SocketUtil::decodeEndpoint(ntsa::Endpoint* endpoint,
                                       const void*     socketAddress,
                                       bsl::size_t     socketAddressSize)
{
    return SocketStorageUtil::convert(
        endpoint,
        reinterpret_cast<const sockaddr_storage*>(socketAddress),
        static_cast<socklen_t>(socketAddressSize));
}

ntsa::Error SocketUtil::encodeEndpoint(void*                 socketAddress,
                                       bsl::size_t*          socketAddressSize,
                                       const ntsa::Endpoint& endpoint)
{
    socklen_t length = 0;

    SocketStorageUtil::initialize(
        reinterpret_cast<sockaddr_storage*>(socketAddress),
        &length);

    ntsa::Error error = SocketStorageUtil::convert(
        reinterpret_cast<sockaddr_storage*>(socketAddress),
        &length,
        endpoint);

    if (error) {
        return error;
    }

    *socketAddressSize = static_cast<bsl::size_t>(length);
    return ntsa::Error();
}

bsl::size_t SocketUtil::maxBytesPerSend(ntsa::Handle socket)
{
#if NTSU_SOCKETUTIL_LIMIT_MAX_BYTES_PER_SEND

    ntsa::Error error;
    bool        isStream;

    error = ntsu::SocketOptionUtil::isStream(&isStream, socket);
    if (error) {
        return 0;
    }

    if (!isStream) {
        return 0;
    }

    bsl::size_t result;
    error = ntsu::SocketOptionUtil::getSendBufferRemaining(&result, socket);
    if (error) {
        return 0;
    }

    return result;

#else

    NTSCFG_WARNING_UNUSED(socket);
    return 0;

#endif
}

bsl::size_t SocketUtil::maxBytesPerReceive(ntsa::Handle socket)
{
#if NTSU_SOCKETUTIL_LIMIT_MAX_BYTES_PER_RECEIVE

    ntsa::Error error;
    bool        isStream;

    error = ntsu::SocketOptionUtil::isStream(&isStream, socket);
    if (error) {
        return 0;
    }

    if (!isStream) {
        return 0;
    }

    bsl::size_t result;
    error = ntsu::SocketOptionUtil::getReceiveBufferAvailable(&result, socket);
    if (error) {
        return 0;
    }

    return result;

#else
    NTSCFG_WARNING_UNUSED(socket);
    return 0;
#endif
}

}  // close package namespace
}  // close enterprise namespace
