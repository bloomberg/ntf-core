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

#include <ntso_poll.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntso_poll_cpp, "$Id$ $CSID$")

#if defined(NTSO_POLL_ENABLED)

#include <ntsi_reactor.h>
#include <ntsu_socketoptionutil.h>
#include <ntsu_socketutil.h>
#include <ntsa_interest.h>

#include <bdlma_localsequentialallocator.h>
#include <bdlt_currenttime.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_log.h>
#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <errno.h>
#include <poll.h>
#include <unistd.h>
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

#define NTSO_POLL_LOG_DEVICE_CREATE()                                       \
    BSLS_LOG_TRACE("Reactor 'poll' device created")

#define NTSO_POLL_LOG_DEVICE_CLOSE()                                        \
    BSLS_LOG_TRACE("Reactor 'poll' device closed")

#define NTSO_POLL_LOG_WAIT_INDEFINITE()                                       \
    BSLS_LOG_TRACE("Polling for socket events indefinitely")

#define NTSO_POLL_LOG_WAIT_TIMED(timeout)                                     \
    BSLS_LOG_TRACE(                                                           \
        "Polling for sockets events or until %d milliseconds have elapsed",   \
        (int)(timeout))

#define NTSO_POLL_LOG_WAIT_FAILURE(error)                                     \
    BSLS_LOG_ERROR("Failed to poll for socket events: %s",                    \
                   error.text().c_str())

#define NTSO_POLL_LOG_WAIT_TIMEOUT()                                          \
    BSLS_LOG_TRACE("Timed out polling for socket events")

#define NTSO_POLL_LOG_WAIT_RESULT(numEvents)                                  \
    BSLS_LOG_TRACE("Polled %d socket events", numEvents)

#define NTSO_POLL_LOG_EVENTS(handle, event)                                   \
    BSLS_LOG_TRACE("Descriptor %d polled [%s%s%s%s%s ]",                      \
                   handle,                                                    \
                   ((((event).revents & POLLIN) != 0) ? " POLLIN" : ""),      \
                   ((((event).revents & POLLOUT) != 0) ? " POLLOUT" : ""),    \
                   ((((event).revents & POLLERR) != 0) ? " POLLERR" : ""),    \
                   ((((event).revents & POLLHUP) != 0) ? " POLLHUP" : ""),    \
                   ((((event).revents & POLLNVAL) != 0) ? " POLLNVAL" : ""))

#define NTSO_POLL_LOG_ADD(handle)                                             \
    BSLS_LOG_TRACE("Descriptor %d added", handle)

#define NTSO_POLL_LOG_UPDATE(handle, interestSet)                             \
    BSLS_LOG_TRACE("Descriptor %d updated [%s%s ]",                           \
                   (handle),                                                  \
                   (((interestSet).wantReadable((handle))) ? " POLLIN" : ""), \
                   (((interestSet).wantWritable((handle))) ? " POLLOUT" : ""))

#define NTSO_POLL_LOG_REMOVE(handle)                                          \
    BSLS_LOG_TRACE("Descriptor %d removed", handle)

namespace BloombergLP {
namespace ntso {

/// @brief @internal
/// Provide an implementation of the 'ntsi::Reactor' interface to poll for
/// socket events using the 'poll' API on all platforms.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntso
class Poll : public ntsi::Reactor
{
    /// This typedef defines a vector of poll structures.
    typedef bsl::vector<struct ::pollfd> DescriptorVector;

    ntsa::InterestSet d_interestSet;
    DescriptorVector  d_descriptorVector;
    bsl::uint64_t     d_thisGeneration;
    bsl::uint64_t     d_nextGeneration;
    bslma::Allocator *d_allocator_p;

  private:
    Poll(const Poll&) BSLS_KEYWORD_DELETED;
    Poll& operator=(const Poll&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Poll(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Poll() BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'socket' to the reactor. Return the error.
    ntsa::Error add(ntsa::Handle socket) BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'socket' to the reactor. Return the error.
    ntsa::Error add(const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'socket' to the reactor. Return the error.
    ntsa::Error add(const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'socket' to the reactor. Return the error.
    ntsa::Error add(const bsl::shared_ptr<ntsi::StreamSocket>& socket)
        BSLS_KEYWORD_OVERRIDE;

    /// Remove the specified 'socket' from the reactor. Return the error.
    ntsa::Error remove(ntsa::Handle socket) BSLS_KEYWORD_OVERRIDE;

    /// Remove the specified 'socket' from the reactor. Return the error.
    ntsa::Error remove(const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
        BSLS_KEYWORD_OVERRIDE;

    /// Remove the specified 'socket' from the reactor. Return the error.
    ntsa::Error remove(const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
        BSLS_KEYWORD_OVERRIDE;

    /// Remove the specified 'socket' from the reactor. Return the error.
    ntsa::Error remove(const bsl::shared_ptr<ntsi::StreamSocket>& socket)
        BSLS_KEYWORD_OVERRIDE;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    ntsa::Error showReadable(ntsa::Handle socket) BSLS_KEYWORD_OVERRIDE;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    ntsa::Error showReadable(const bsl::shared_ptr<ntsi::DatagramSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    ntsa::Error showReadable(const bsl::shared_ptr<ntsi::ListenerSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    ntsa::Error showReadable(const bsl::shared_ptr<ntsi::StreamSocket>& socket)
        BSLS_KEYWORD_OVERRIDE;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    ntsa::Error showWritable(ntsa::Handle socket) BSLS_KEYWORD_OVERRIDE;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    ntsa::Error showWritable(const bsl::shared_ptr<ntsi::DatagramSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    ntsa::Error showWritable(const bsl::shared_ptr<ntsi::ListenerSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    ntsa::Error showWritable(const bsl::shared_ptr<ntsi::StreamSocket>& socket)
        BSLS_KEYWORD_OVERRIDE;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    ntsa::Error hideReadable(ntsa::Handle socket) BSLS_KEYWORD_OVERRIDE;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    ntsa::Error hideReadable(const bsl::shared_ptr<ntsi::DatagramSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    ntsa::Error hideReadable(const bsl::shared_ptr<ntsi::ListenerSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    ntsa::Error hideReadable(const bsl::shared_ptr<ntsi::StreamSocket>& socket)
        BSLS_KEYWORD_OVERRIDE;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    ntsa::Error hideWritable(ntsa::Handle socket) BSLS_KEYWORD_OVERRIDE;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    ntsa::Error hideWritable(const bsl::shared_ptr<ntsi::DatagramSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    ntsa::Error hideWritable(const bsl::shared_ptr<ntsi::ListenerSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    ntsa::Error hideWritable(const bsl::shared_ptr<ntsi::StreamSocket>& socket)
        BSLS_KEYWORD_OVERRIDE;

    /// Block until at least one socket has a condition of interest or the
    /// specified absolute 'deadline', if any, elapses. Load into the specified
    /// 'result' the events that describe the sockets and their conditions.
    /// Return the error.
    ntsa::Error wait(
        bsl::vector<ntsa::Event>*                      result,
        const bdlb::NullableValue<bsls::TimeInterval>& deadline)              
        BSLS_KEYWORD_OVERRIDE;

    // MRM
    #if 0
    /// Block until at least one socket in the specified 'readable',
    /// 'writable', or 'exceptional' sets matches any of those respective
    /// conditions, or the specified 'timeout' elapses, if any. Load into the
    /// specified 'result' the events that describe the sockets and their
    /// conditions. Return the error. 
    ntsa::Error wait(
        bsl::vector<ntsa::Event>                       *result, 
        const bsl::vector<ntsa::Handle>&                readable, 
        const bsl::vector<ntsa::Handle>&                writable,
        const bsl::vector<ntsa::Handle>&                exceptional,
        const bdlb::NullableValue<bsls::TimeInterval>&  timeout) 
        BSLS_KEYWORD_OVERRIDE;
    #endif
};

Poll::Poll(bslma::Allocator* basicAllocator)
: d_interestSet(basicAllocator)
, d_descriptorVector(basicAllocator)
, d_thisGeneration(0)
, d_nextGeneration(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    NTSO_POLL_LOG_DEVICE_CREATE();
}

Poll::~Poll()
{
    NTSO_POLL_LOG_DEVICE_CLOSE();
}

ntsa::Error Poll::add(ntsa::Handle socket)
{
    ntsa::Error error;

    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_interestSet.attach(socket);
    if (error) {
        return error;
    }

    ++d_nextGeneration;

    NTSO_POLL_LOG_ADD(socket);

    return ntsa::Error();
}

ntsa::Error Poll::add(const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->add(socket->handle());
}

ntsa::Error Poll::add(const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->add(socket->handle());
}

ntsa::Error Poll::add(const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->add(socket->handle());
}

ntsa::Error Poll::remove(ntsa::Handle socket)
{
    ntsa::Error error;

    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_interestSet.detach(socket);
    if (error) {
        return error;
    }

    ++d_nextGeneration;

    NTSO_POLL_LOG_REMOVE(socket);

    return ntsa::Error();
}

ntsa::Error Poll::remove(const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->remove(socket->handle());
}

ntsa::Error Poll::remove(const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->remove(socket->handle());
}

ntsa::Error Poll::remove(const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->remove(socket->handle());
}

ntsa::Error Poll::showReadable(ntsa::Handle socket)
{
    ntsa::Error error;

    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_interestSet.showReadable(socket);
    if (error) {
        return error;
    }

    ++d_nextGeneration;

    NTSO_POLL_LOG_UPDATE(socket, d_interestSet);

    return ntsa::Error();
}

ntsa::Error Poll::showReadable(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->showReadable(socket->handle());
}

ntsa::Error Poll::showReadable(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->showReadable(socket->handle());
}

ntsa::Error Poll::showReadable(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->showReadable(socket->handle());
}

ntsa::Error Poll::showWritable(ntsa::Handle socket)
{
    ntsa::Error error;

    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_interestSet.showWritable(socket);
    if (error) {
        return error;
    }

    ++d_nextGeneration;

    NTSO_POLL_LOG_UPDATE(socket, d_interestSet);

    return ntsa::Error();
}

ntsa::Error Poll::showWritable(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->showWritable(socket->handle());
}

ntsa::Error Poll::showWritable(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->showWritable(socket->handle());
}

ntsa::Error Poll::showWritable(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->showWritable(socket->handle());
}

ntsa::Error Poll::hideReadable(ntsa::Handle socket)
{
    ntsa::Error error;

    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_interestSet.hideReadable(socket);
    if (error) {
        return error;
    }

    ++d_nextGeneration;

    NTSO_POLL_LOG_UPDATE(socket, d_interestSet);

    return ntsa::Error();
}

ntsa::Error Poll::hideReadable(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->hideReadable(socket->handle());
}

ntsa::Error Poll::hideReadable(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->hideReadable(socket->handle());
}

ntsa::Error Poll::hideReadable(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->hideReadable(socket->handle());
}

ntsa::Error Poll::hideWritable(ntsa::Handle socket)
{
    ntsa::Error error;

    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_interestSet.hideWritable(socket);
    if (error) {
        return error;
    }

    ++d_nextGeneration;

    NTSO_POLL_LOG_UPDATE(socket, d_interestSet);

    return ntsa::Error();
}

ntsa::Error Poll::hideWritable(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->hideReadable(socket->handle());
}

ntsa::Error Poll::hideWritable(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->hideWritable(socket->handle());
}

ntsa::Error Poll::hideWritable(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->hideWritable(socket->handle());
}

ntsa::Error Poll::wait(
    bsl::vector<ntsa::Event>*                      result,
    const bdlb::NullableValue<bsls::TimeInterval>& timeout)
{
    int rc;

    result->clear();

    int timeoutInMilliseconds = -1;
    if (!timeout.isNull()) {
        const bsls::TimeInterval now = bdlt::CurrentTime::now();
        if (NTSCFG_LIKELY(timeout.value() > now)) {
            const bsls::TimeInterval delta = timeout.value() - now;

            timeoutInMilliseconds = 
                static_cast<int>(delta.totalMilliseconds());

            NTSO_POLL_LOG_WAIT_TIMED(delta.totalMilliseconds());
        }
        else {
            timeoutInMilliseconds = 0;
            NTSO_POLL_LOG_WAIT_TIMED(0);
        }
    }
    else {
        NTSO_POLL_LOG_WAIT_INDEFINITE();
    }

    if (d_nextGeneration > d_thisGeneration) {
        d_descriptorVector.clear();
        d_descriptorVector.reserve(d_interestSet.numSockets());

        ntsa::InterestSet::const_iterator it = d_interestSet.begin();
        ntsa::InterestSet::const_iterator et = d_interestSet.end();

        for (; it != et; ++it) {
            ntsa::Interest interest = *it;

            struct ::pollfd pfd;
            pfd.fd      = interest.handle();
            pfd.events  = 0;
            pfd.revents = 0;

            if (interest.wantReadable()) {
                pfd.events |= POLLIN;
            }

            if (interest.wantWritable()) {
                pfd.events |= POLLOUT;
            }

            d_descriptorVector.push_back(pfd);
        }

        d_thisGeneration = d_nextGeneration;
    }

#if defined(BSLS_PLATFORM_OS_UNIX)

    rc = ::poll(&d_descriptorVector[0],
                static_cast<nfds_t>(d_descriptorVector.size()),
                timeoutInMilliseconds);

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    rc = WSAPoll(&d_descriptorVector[0],
                 static_cast<ULONG>(d_descriptorVector.size()),
                 timeoutInMilliseconds);

#else
#error Not implemented
#endif

    if (NTSCFG_LIKELY(rc > 0)) {
        NTSO_POLL_LOG_WAIT_RESULT(rc);

        int numResults          = rc;
        int numResultsRemaining = numResults;

        result->reserve(result->size() + numResults);

        DescriptorVector::const_iterator it = d_descriptorVector.begin();
        DescriptorVector::const_iterator et = d_descriptorVector.end();

        for (; it != et; ++it) {            
            if (numResultsRemaining == 0) {
                break;
            }

            struct ::pollfd e = *it;

            if (e.revents == 0) {
                continue;
            }

            BSLS_ASSERT(numResultsRemaining > 0);
            --numResultsRemaining;

            NTSO_POLL_LOG_EVENTS(e.fd, e);

            ntsa::Event event;
            event.setHandle(e.fd);

            if ((e.revents & POLLIN) != 0) {
                event.setReadable();
            }

            if ((e.revents & POLLOUT) != 0) {
                event.setWritable();
            }

            if ((e.revents & POLLHUP) != 0) {
                event.setHangup();
            }

            if ((e.revents & POLLERR) != 0) {
                ntsa::Error lastError;
                ntsa::Error error =
                    ntsu::SocketOptionUtil::getLastError(&lastError, e.fd);
                if (!error && lastError) {
                    event.setError(lastError);
                }

                event.setExceptional();
            }

            if ((e.revents & POLLNVAL) != 0) {
                ntsa::Error lastError;
                ntsa::Error error =
                    ntsu::SocketOptionUtil::getLastError(&lastError, e.fd);

                if (error) {
                    event.setError(error);
                }
                else if (lastError) {
                    event.setError(lastError);
                }
                else {
#if defined(BSLS_PLATFORM_OS_UNIX)
                    event.setError(ntsa::Error(EBADF));
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    event.setError(ntsa::Error(ERROR_INVALID_HANDLE));
#else
#error Not implemented
#endif
                }

                this->remove(e.fd);
            }

            result->push_back(event);
        }
    }
    else if (rc == 0) {
        NTSO_POLL_LOG_WAIT_TIMEOUT();
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }
    else {
        ntsa::Error error = ntsa::Error::last();
        NTSO_POLL_LOG_WAIT_FAILURE(error);
        return error;
    }

    return ntsa::Error();
}

bsl::shared_ptr<ntsi::Reactor> PollUtil::createReactor(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntso::Poll> reactor;
    reactor.createInplace(allocator, allocator);

    return reactor;
}

}  // close package namespace
}  // close enterprise namespace
#endif
