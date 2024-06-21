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

#include <ntcs_controller.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_controller_cpp, "$Id$ $CSID$")

#include <ntci_log.h>
#include <ntsu_socketoptionutil.h>
#include <bslmt_lockguard.h>
#include <bsls_log.h>
#include <bsls_platform.h>
#include <bsl_cstdlib.h>

// Experimentation has shown that polling a Unix domain socket does not
// reliably wake up all waiters, even when the socket being polled has data
// in the receive buffer.

#define NTCS_CONTROLLER_IMP_TCP_SOCKET 0
#define NTCS_CONTROLLER_IMP_UNIX_DOMAIN_SOCKET 1
#define NTCS_CONTROLLER_IMP_ANONYMOUS_PIPE 2
#define NTCS_CONTROLLER_IMP_EVENTFD 3

// Uncomment to force the use of a particular implementation, regardless of
// the implementation most suitable for the platform.
// #define NTCS_CONTROLLER_IMP NTCS_CONTROLLER_TCP_SOCKET

#ifndef NTCS_CONTROLLER_IMP
#if defined(BSLS_PLATFORM_OS_LINUX)
#define NTCS_CONTROLLER_IMP NTCS_CONTROLLER_IMP_EVENTFD
#elif defined(BSLS_PLATFORM_OS_UNIX)
#define NTCS_CONTROLLER_IMP NTCS_CONTROLLER_IMP_ANONYMOUS_PIPE
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
#define NTCS_CONTROLLER_IMP NTCS_CONTROLLER_IMP_UNIX_DOMAIN_SOCKET
#else
#error Not implemented
#endif
#endif

#if NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_ANONYMOUS_PIPE
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#if NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_EVENTFD
#include <errno.h>
#include <sys/eventfd.h>
#include <unistd.h>
#endif

#define NTCS_CONTROLLER_LOG_DEFAULT 1

#ifndef NTCS_CONTROLLER_LOG
#ifndef NTCS_LOG
#define NTCS_CONTROLLER_LOG NTCS_CONTROLLER_LOG_DEFAULT
#else
#define NTCS_CONTROLLER_LOG NTCS_LOG
#endif
#endif

#if NTCS_CONTROLLER_LOG == 0
#undef NTCI_LOG_FATAL
#define NTCI_LOG_FATAL(...)
#undef NTCI_LOG_ERROR
#define NTCI_LOG_ERROR(...)
#undef NTCI_LOG_WARN
#define NTCI_LOG_WARN(...)
#undef NTCI_LOG_INFO
#define NTCI_LOG_INFO(...)
#undef NTCI_LOG_DEBUG
#define NTCI_LOG_DEBUG(...)
#undef NTCI_LOG_TRACE
#define NTCI_LOG_TRACE(...)
#endif

#define NTCS_CONTROLLER_LOG_ENQUEUE(amount, total)                            \
    NTCI_LOG_TRACE("Controller signaled %d more wakeup(s) with %d total "     \
                   "wakeups now pending",                                     \
                   (int)(amount),                                             \
                   (int)(total))

#define NTCS_CONTROLLER_LOG_DEQUEUE(amount, total)                            \
    NTCI_LOG_TRACE("Controller acknowledged %d wakeup(s) with %d total "      \
                   "wakeups now pending",                                     \
                   (int)(amount),                                             \
                   (int)(total))

namespace BloombergLP {
namespace ntcs {

namespace {

#if (NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_TCP_SOCKET) ||                \
    (NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_UNIX_DOMAIN_SOCKET)
ntsa::Error initTcpPair(ntsa::Handle* clientHandle, ntsa::Handle* serverHandle)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    ntsa::Handle client = ntsa::k_INVALID_HANDLE;
    ntsa::Handle server = ntsa::k_INVALID_HANDLE;

    error = ntsu::SocketUtil::pair(&client,
                                   &server,
                                   ntsa::Transport::e_TCP_IPV4_STREAM);

    if (error) {
        NTCI_LOG_FATAL("Failed to create controller socket pair: %s",
                       error.text().c_str());
        return error;
    }

    ntsu::SocketUtil::Guard clientGuard(client);
    ntsu::SocketUtil::Guard serverGuard(server);

    error = ntsu::SocketOptionUtil::setNoDelay(client, true);
    if (error) {
        NTCI_LOG_FATAL("Failed to set TCP_NODELAY: %s", error.text().c_str());
        return error;
    }

    error = ntsu::SocketOptionUtil::setKeepAlive(client, true);
    if (error) {
        NTCI_LOG_FATAL("Failed to set TCP_KEEPALIVE: %s",
                       error.text().c_str());
        return error;
    }

    error = ntsu::SocketOptionUtil::setBlocking(client, true);
    if (error) {
        NTCI_LOG_FATAL("Failed to set controller client socket "
                       "to blocking mode: %s",
                       error.text().c_str());
        return error;
    }

    error = ntsu::SocketOptionUtil::setBlocking(server, false);
    if (error) {
        NTCI_LOG_FATAL("Failed to set controller server socket "
                       "to non-blocking mode: %s",
                       error.text().c_str());
        return error;
    }

    *clientHandle = client;
    *serverHandle = server;

    clientGuard.release();
    serverGuard.release();

    NTCI_LOG_TRACE("Controller created from TCP socket pair with "
                   "client descriptor %d and server descriptor %d",
                   *clientHandle,
                   *serverHandle);

    return ntsa::Error();
}
#endif

#if NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_UNIX_DOMAIN_SOCKET
ntsa::Error initUdsPair(ntsa::Handle* clientHandle, ntsa::Handle* serverHandle)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    ntsa::Handle client = ntsa::k_INVALID_HANDLE;
    ntsa::Handle server = ntsa::k_INVALID_HANDLE;

    error = ntsu::SocketUtil::pair(&client,
                                   &server,
                                   ntsa::Transport::e_LOCAL_STREAM);

    if (error) {
        NTCI_LOG_WARN("Failed to create controller socket pair: %s",
                      error.text().c_str());
        return error;
    }

    ntsu::SocketUtil::Guard clientGuard(client);
    ntsu::SocketUtil::Guard serverGuard(server);

    error = ntsu::SocketOptionUtil::setBlocking(client, true);
    if (error) {
        NTCI_LOG_WARN("Failed to set controller client socket "
                      "to blocking mode: %s",
                      error.text().c_str());
        return error;
    }

    error = ntsu::SocketOptionUtil::setBlocking(server, false);
    if (error) {
        NTCI_LOG_WARN("Failed to set controller server socket "
                      "to non-blocking mode: %s",
                      error.text().c_str());
        return error;
    }

    *clientHandle = client;
    *serverHandle = server;

    clientGuard.release();
    serverGuard.release();

    NTCI_LOG_TRACE("Controller created from Unix domain socket pair with "
                   "client descriptor %d and server descriptor %d",
                   *clientHandle,
                   *serverHandle);

    return ntsa::Error();
}
#endif

#if NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_ANONYMOUS_PIPE
ntsa::Error initPipePair(ntsa::Handle* clientHandle,
                         ntsa::Handle* serverHandle)
{
    NTCI_LOG_CONTEXT();

    int pipes[2];
    int rc = pipe(pipes);
    if (rc != 0) {
        ntsa::Error error(errno);
        NTCI_LOG_FATAL("Failed to create anonymous pipe: %s",
                       error.text().c_str());
        return error;
    }

    ::fcntl(pipes[0], F_SETFL, O_NONBLOCK);
    ::fcntl(pipes[1], F_SETFL, O_NONBLOCK);

#if defined(FD_CLOEXEC)
    ::fcntl(pipes[0], F_SETFD, FD_CLOEXEC);
    ::fcntl(pipes[1], F_SETFD, FD_CLOEXEC);
#endif

    *serverHandle = pipes[0];
    *clientHandle = pipes[1];

    NTCI_LOG_TRACE("Controller created from anonymous pipe pair with "
                   "client descriptor %d and server descriptor %d",
                   *clientHandle,
                   *serverHandle);

    return ntsa::Error();
}
#endif

#if NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_EVENTFD
ntsa::Error initEventFdPair(ntsa::Handle* clientHandle,
                            ntsa::Handle* serverHandle)
{
    NTCI_LOG_CONTEXT();

    *serverHandle = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK | EFD_SEMAPHORE);
    if (*serverHandle < 0) {
        ntsa::Error error(errno);
        NTCI_LOG_FATAL("Failed to create event: %s", error.text().c_str());
        return error;
    }

    *clientHandle = *serverHandle;

    NTCI_LOG_TRACE("Controller created from event semaphore descriptor %d",
                   *serverHandle);

    return ntsa::Error();
}
#endif

}  // close unnamed namespace

void Controller::processSocketReadable(const ntca::ReactorEvent& event)
{
    NTCCFG_WARNING_UNUSED(event);
}

void Controller::processSocketWritable(const ntca::ReactorEvent& event)
{
    NTCCFG_WARNING_UNUSED(event);
}

void Controller::processSocketError(const ntca::ReactorEvent& event)
{
    NTCCFG_WARNING_UNUSED(event);
}

void Controller::processNotifications(
    const ntsa::NotificationQueue& notifications)
{
    NTCCFG_WARNING_UNUSED(notifications);
}

const bsl::shared_ptr<ntci::Strand>& Controller::strand() const
{
    return d_strand_sp;
}

Controller::Controller()
: d_mutex(NTCCFG_LOCK_INIT)
, d_clientHandle(ntsa::k_INVALID_HANDLE)
, d_serverHandle(ntsa::k_INVALID_HANDLE)
, d_pending(0)
, d_type(e_NONE)
, d_strand_sp()
{
    ntsa::Error error;

    error = this->open();
    if (error) {
        NTCCFG_ABORT();
    }
}

ntsa::Error Controller::open()
{
    ntsa::Error error;

    if (d_serverHandle != ntsa::k_INVALID_HANDLE) {
        return ntsa::Error();
    }

#if NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_TCP_SOCKET

    error = initTcpPair(&d_clientHandle, &d_serverHandle);
    if (error) {
        return error;
    }
    else {
        d_type = e_TCP;
    }

#elif NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_UNIX_DOMAIN_SOCKET

    error = initUdsPair(&d_clientHandle, &d_serverHandle);
    if (error) {
        error = initTcpPair(&d_clientHandle, &d_serverHandle);
        if (error) {
            return error;
        }
        else {
            d_type = e_TCP;
        }
    }
    else {
        d_type = e_UDS;
    }

#elif NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_ANONYMOUS_PIPE
    error = initPipePair(&d_clientHandle, &d_serverHandle);
    if (error) {
        return error;
    }
    else {
        d_type = e_PIPE;
    }

#elif NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_EVENTFD

    error = initEventFdPair(&d_clientHandle, &d_serverHandle);
    if (error) {
        return error;
    }
    else {
        d_type = e_EVENT;
    }

#else
#error Not implemented
#endif

    return ntsa::Error();
}

Controller::~Controller()
{
    this->close();
}

void Controller::close()
{
    NTCI_LOG_CONTEXT();

    if (d_serverHandle == ntsa::k_INVALID_HANDLE) {
        return;
    }

#if NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_TCP_SOCKET ||                  \
    NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_UNIX_DOMAIN_SOCKET

    ntsu::SocketUtil::shutdown(ntsa::ShutdownType::e_BOTH, d_clientHandle);
    ntsu::SocketUtil::shutdown(ntsa::ShutdownType::e_BOTH, d_serverHandle);

    ntsu::SocketUtil::close(d_clientHandle);
    ntsu::SocketUtil::close(d_serverHandle);

#elif NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_ANONYMOUS_PIPE

    ::close(d_clientHandle);
    ::close(d_serverHandle);

#elif NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_EVENTFD

    BSLS_ASSERT(d_clientHandle == d_serverHandle);
    ::close(d_serverHandle);

#else
#error Not implemented
#endif

    d_clientHandle = ntsa::k_INVALID_HANDLE;
    d_serverHandle = ntsa::k_INVALID_HANDLE;

    d_type = e_NONE;

    NTCI_LOG_TRACE("Controller destroyed");
}

ntsa::Error Controller::interrupt(unsigned int numWakeups)
{
#if NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_TCP_SOCKET ||                  \
    NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_UNIX_DOMAIN_SOCKET

    NTCI_LOG_CONTEXT();

    LockGuard lock(&d_mutex);

    if (numWakeups <= d_pending) {
        return ntsa::Error();
    }

    unsigned int numToWrite =
        NTCCFG_WARNING_NARROW(unsigned int, numWakeups - d_pending);

    bsl::vector<char> buffer(numToWrite);

    const char* p = &buffer[0];
    bsl::size_t c = buffer.size();

    while (c > 0) {
        ntsa::SendContext context;
        ntsa::SendOptions options;

        ntsa::Data data(ntsa::ConstBuffer(p, c));

        ntsa::Error error =
            ntsu::SocketUtil::send(&context, data, options, d_clientHandle);
        if (error) {
            if (error == ntsa::Error::e_INTERRUPTED) {
                continue;
            }
            else {
                NTCI_LOG_ERROR("Failed to write to controller: %s",
                               error.text().c_str());
                return error;
            }
        }

        p += context.bytesSent();
        c -= context.bytesSent();

        d_pending += context.bytesSent();
    }

    NTCS_CONTROLLER_LOG_ENQUEUE(numToWrite, d_pending);

    return ntsa::Error();

#elif NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_ANONYMOUS_PIPE

    NTCI_LOG_CONTEXT();

    LockGuard lock(&d_mutex);

    if (numWakeups <= d_pending) {
        return ntsa::Error();
    }

    unsigned int numToWrite =
        NTCCFG_WARNING_NARROW(unsigned int, numWakeups - d_pending);

    bsl::vector<char> buffer(numToWrite);

    const char* p = &buffer[0];
    bsl::size_t c = buffer.size();

    while (c > 0) {
        ssize_t n = ::write(d_clientHandle, p, c);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            else {
                ntsa::Error error(errno);
                NTCI_LOG_ERROR("Failed to write to controller: %s",
                               error.text().c_str());
                return error;
            }
        }

        p += n;
        c -= n;

        d_pending += n;
    }

    NTCS_CONTROLLER_LOG_ENQUEUE(numToWrite, d_pending);

    return ntsa::Error();

#elif NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_EVENTFD

    NTCI_LOG_CONTEXT();

    LockGuard lock(&d_mutex);

    if (numWakeups <= d_pending) {
        return ntsa::Error();
    }

    unsigned int numToWrite =
        NTCCFG_WARNING_NARROW(unsigned int, numWakeups - d_pending);

    bsl::uint64_t value = static_cast<bsl::uint64_t>(numToWrite);

    const char* p = reinterpret_cast<const char*>(&value);
    bsl::size_t c = sizeof(bsl::uint64_t);

    while (c > 0) {
        ssize_t n = ::write(d_clientHandle, p, c);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            else {
                ntsa::Error error(errno);
                NTCI_LOG_ERROR("Failed to write to controller: %s",
                               error.text().c_str());
                return error;
            }
        }

        BSLS_ASSERT(n == sizeof(bsl::uint64_t));

        p += n;
        c -= n;

        d_pending += numToWrite;

        NTCS_CONTROLLER_LOG_ENQUEUE(numToWrite, d_pending);
    }

    return ntsa::Error();

#else
#error Not implemented
#endif
}

ntsa::Error Controller::acknowledge()
{
#if NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_TCP_SOCKET ||                  \
    NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_UNIX_DOMAIN_SOCKET

    NTCI_LOG_CONTEXT();

    LockGuard lock(&d_mutex);

    char buffer;

    ntsa::Data data(ntsa::MutableBuffer(&buffer, 1));

    ntsa::ReceiveContext context;
    ntsa::ReceiveOptions options;

    ntsa::Error error =
        ntsu::SocketUtil::receive(&context, &data, options, d_serverHandle);

    if (error) {
        if (error != ntsa::Error::e_WOULD_BLOCK &&
            error != ntsa::Error::e_INTERRUPTED)
        {
            NTCI_LOG_ERROR("Failed to read from controller: %s",
                           error.text().c_str());
            return error;
        }
    }

    d_pending -= context.bytesReceived();

    NTCS_CONTROLLER_LOG_DEQUEUE(context.bytesReceived(), d_pending);

    return ntsa::Error();

#elif NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_ANONYMOUS_PIPE

    NTCI_LOG_CONTEXT();

    LockGuard lock(&d_mutex);

    char        buffer;
    bsl::size_t bytesRead = 0;

    ssize_t n;
    do {
        n = ::read(d_serverHandle, &buffer, 1);
    } while (n < 0 && errno == EINTR);

    if (n < 0) {
        ntsa::Error error(errno);
        if (error != ntsa::Error::e_WOULD_BLOCK) {
            NTCI_LOG_ERROR("Failed to read from controller: %s",
                           error.text().c_str());
            return error;
        }
    }
    else {
        bytesRead = n;
    }

    d_pending -= bytesRead;

    NTCS_CONTROLLER_LOG_DEQUEUE(bytesRead, d_pending);

    return ntsa::Error();

#elif NTCS_CONTROLLER_IMP == NTCS_CONTROLLER_IMP_EVENTFD

    NTCI_LOG_CONTEXT();

    LockGuard lock(&d_mutex);

    bsl::uint64_t value = 0;

    ssize_t n;
    do {
        n = ::read(d_serverHandle, &value, sizeof(bsl::uint64_t));
    } while (n < 0 && errno == EINTR);

    if (n < 0) {
        ntsa::Error error(errno);
        if (error != ntsa::Error::e_WOULD_BLOCK) {
            NTCI_LOG_ERROR("Failed to read from controller: %s",
                           error.text().c_str());
            return error;
        }
    }
    else {
        BSLS_ASSERT(n == sizeof(bsl::uint64_t));
        BSLS_ASSERT(value == 1);
    }

    d_pending -= static_cast<bsl::size_t>(value);

    NTCS_CONTROLLER_LOG_DEQUEUE(value, d_pending);

    return ntsa::Error();

#else
#error Not implemented
#endif
}

ntsa::Handle Controller::handle() const
{
    return d_serverHandle;
}

}  // close package namespace
}  // close enterprise namespace
