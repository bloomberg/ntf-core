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

#include <bsls_platform.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntso_select_cpp, "$Id$ $CSID$")

#include <ntscfg_config.h>

#if defined(BSLS_PLATFORM_OS_DARWIN)
#define _DARWIN_UNLIMITED_SELECT
#endif

#if defined(BSLS_PLATFORM_OS_UNIX)
#ifndef FD_SETSIZE
#define FD_SETSIZE 4096
#endif
#include <errno.h>
#include <sys/time.h>
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
#ifndef FD_SETSIZE
#define FD_SETSIZE 4096
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

#include <ntso_select.h>

#if NTSO_SELECT_ENABLED

#include <ntsa_interest.h>
#include <ntsi_reactor.h>
#include <ntsu_socketoptionutil.h>
#include <ntsu_socketutil.h>

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
#include <bsl_map.h>
#include <bsl_set.h>

#define NTSO_SELECT_LOG_DEVICE_CREATE()                                       \
    BSLS_LOG_TRACE("Reactor 'select' device created")

#define NTSO_SELECT_LOG_DEVICE_CLOSE()                                        \
    BSLS_LOG_TRACE("Reactor 'select' device closed")

#define NTSO_SELECT_LOG_WAIT_INDEFINITE()                                     \
    BSLS_LOG_TRACE("Polling for socket events indefinitely")

#define NTSO_SELECT_LOG_WAIT_TIMED(timeout)                                   \
    BSLS_LOG_TRACE(                                                           \
        "Polling for sockets events or until %d milliseconds have elapsed",   \
        (int)(timeout))

#define NTSO_SELECT_LOG_WAIT_FAILURE(error)                                   \
    BSLS_LOG_ERROR("Failed to poll for socket events: %s",                    \
                   error.text().c_str())

#define NTSO_SELECT_LOG_WAIT_TIMEOUT()                                        \
    BSLS_LOG_TRACE("Timed out polling for socket events")

#define NTSO_SELECT_LOG_WAIT_INTERRUPTED()                                    \
    BSLS_LOG_TRACE("Interrupted polling for socket events")

#define NTSO_SELECT_LOG_WAIT_RESULT(numEvents)                                \
    BSLS_LOG_TRACE("Polled %d socket events", numEvents)

#define NTSO_SELECT_LOG_EVENTS(handle, readable, writable, exceptional)       \
    BSLS_LOG_TRACE(                                                           \
        "Descriptor %d polled [%s%s%s ]",                                     \
        handle,                                                               \
        ((FD_ISSET((handle), &(readable))) ? " READABLE" : ""),               \
        ((FD_ISSET((handle), &(writable))) ? " WRITABLE" : ""),               \
        ((FD_ISSET((handle), &(exceptional))) ? " EXCEPTIONAL" : ""))

#define NTSO_SELECT_LOG_ADD(handle)                                           \
    BSLS_LOG_TRACE("Descriptor %d added", handle)

#define NTSO_SELECT_LOG_UPDATE(handle, readable, writable, exceptional)       \
    BSLS_LOG_TRACE("Descriptor %d updated [%s%s ]",                           \
                   handle,                                                    \
                   ((FD_ISSET((handle), &(readable))) ? " READABLE" : ""),    \
                   ((FD_ISSET((handle), &(writable))) ? " WRITABLE" : ""))

#define NTSO_SELECT_LOG_REMOVE(handle)                                        \
    BSLS_LOG_TRACE("Descriptor %d removed", handle)

#if defined(BSLS_PLATFORM_CMP_SUN)
#pragma error_messages(off, SEC_NULL_PTR_DEREF)
#endif

namespace BloombergLP {
namespace ntso {

/// @brief @internal
/// Provide an implementation of the 'ntsi::Reactor' interface to poll for
/// socket events using the 'select' API on all platforms.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntso
class Select : public ntsi::Reactor
{
    ntsa::InterestSet   d_interestSet;
    fd_set              d_readable;
    fd_set              d_writable;
    fd_set              d_exceptional;
    ntsa::Handle        d_maxHandle;
    ntsa::ReactorConfig d_config;
    bslma::Allocator*   d_allocator_p;

  private:
    Select(const Select&) BSLS_KEYWORD_DELETED;
    Select& operator=(const Select&) BSLS_KEYWORD_DELETED;

  private:
    /// Copy the specified 'source' to the specified 'destination'.
    static void copy(fd_set* destination, const fd_set& source);

  public:
    /// Create a new reactor having the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit Select(const ntsa::ReactorConfig& configuration,
                    bslma::Allocator*          basicAllocator = 0);

    /// Destroy this object.
    ~Select() BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'socket' to the reactor. Return the error.
    ntsa::Error attachSocket(ntsa::Handle socket) BSLS_KEYWORD_OVERRIDE;

    /// Remove the specified 'socket' from the reactor. Return the error.
    ntsa::Error detachSocket(ntsa::Handle socket) BSLS_KEYWORD_OVERRIDE;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    ntsa::Error showReadable(ntsa::Handle socket) BSLS_KEYWORD_OVERRIDE;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    ntsa::Error showWritable(ntsa::Handle socket) BSLS_KEYWORD_OVERRIDE;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    ntsa::Error hideReadable(ntsa::Handle socket) BSLS_KEYWORD_OVERRIDE;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    ntsa::Error hideWritable(ntsa::Handle socket) BSLS_KEYWORD_OVERRIDE;

    /// Block until at least one socket has a condition of interest or the
    /// specified absolute 'deadline', if any, elapses. Load into the specified
    /// 'result' the events that describe the sockets and their conditions.
    /// Return the error.
    ntsa::Error wait(ntsa::EventSet*                                result,
                     const bdlb::NullableValue<bsls::TimeInterval>& deadline)
        BSLS_KEYWORD_OVERRIDE;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Select);
};

void Select::copy(fd_set* destination, const fd_set& source)
{
#if defined(BSLS_PLATFORM_OS_UNIX)
#if defined(FD_COPY)
    FD_COPY(&source, destination);
#else
    *destination = source;
#endif
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
#if defined(FD_COPY)
    FD_COPY(&source, destination);
#else
    destination->fd_count = source.fd_count;
    bsl::memcpy(&destination->fd_array,
                &source.fd_array,
                source.fd_count * sizeof(source.fd_array[0]));
#endif
#else
#error Not implemented
#endif
}

Select::Select(const ntsa::ReactorConfig& configuration,
               bslma::Allocator*          basicAllocator)
: d_interestSet(basicAllocator)
, d_maxHandle(0)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_config.autoAttach().isNull()) {
        d_config.setAutoAttach(false);
    }

    if (d_config.autoDetach().isNull()) {
        d_config.setAutoDetach(false);
    }

    FD_ZERO(&d_readable);
    FD_ZERO(&d_writable);
    FD_ZERO(&d_exceptional);

    NTSO_SELECT_LOG_DEVICE_CREATE();
}

Select::~Select()
{
    NTSO_SELECT_LOG_DEVICE_CLOSE();
}

ntsa::Error Select::attachSocket(ntsa::Handle socket)
{
    ntsa::Error error;

    error = d_interestSet.attach(socket);
    if (error) {
        return error;
    }

    FD_SET(socket, &d_exceptional);

    if (socket > d_maxHandle) {
        d_maxHandle = socket;
    }

    NTSO_SELECT_LOG_ADD(socket);

    return ntsa::Error();
}

ntsa::Error Select::detachSocket(ntsa::Handle socket)
{
    ntsa::Error error;

    error = d_interestSet.detach(socket);
    if (error) {
        return error;
    }

    FD_CLR(socket, &d_readable);
    FD_CLR(socket, &d_writable);
    FD_CLR(socket, &d_exceptional);

    if (socket >= d_maxHandle) {
        while (d_maxHandle != 0) {
            if (FD_ISSET(d_maxHandle, &d_readable) ||
                FD_ISSET(d_maxHandle, &d_writable) ||
                FD_ISSET(d_maxHandle, &d_exceptional))
            {
                break;
            }

            --d_maxHandle;
        }
    }

    NTSO_SELECT_LOG_REMOVE(socket);

    return ntsa::Error();
}

ntsa::Error Select::showReadable(ntsa::Handle socket)
{
    ntsa::Error error;

    if (d_config.autoAttach().value()) {
        if (!d_interestSet.contains(socket)) {
            error = this->attachSocket(socket);
            if (error) {
                return error;
            }
        }
    }

    error = d_interestSet.showReadable(socket);
    if (error) {
        return error;
    }

    FD_SET(socket, &d_readable);

    NTSO_SELECT_LOG_UPDATE(socket, d_readable, d_writable, d_exceptional);

    return ntsa::Error();
}

ntsa::Error Select::showWritable(ntsa::Handle socket)
{
    ntsa::Error error;

    if (d_config.autoAttach().value()) {
        if (!d_interestSet.contains(socket)) {
            error = this->attachSocket(socket);
            if (error) {
                return error;
            }
        }
    }

    error = d_interestSet.showWritable(socket);
    if (error) {
        return error;
    }

    FD_SET(socket, &d_writable);

    NTSO_SELECT_LOG_UPDATE(socket, d_readable, d_writable, d_exceptional);

    return ntsa::Error();
}

ntsa::Error Select::hideReadable(ntsa::Handle socket)
{
    ntsa::Error error;

    ntsa::Interest interest;
    error = d_interestSet.hideReadable(&interest, socket);
    if (error) {
        return error;
    }

    FD_CLR(socket, &d_readable);

    NTSO_SELECT_LOG_UPDATE(socket, d_readable, d_writable, d_exceptional);

    if (d_config.autoDetach().value()) {
        if (interest.wantNone()) {
            error = this->detachSocket(socket);
            if (error) {
                return error;
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error Select::hideWritable(ntsa::Handle socket)
{
    ntsa::Error error;

    ntsa::Interest interest;
    error = d_interestSet.hideWritable(&interest, socket);
    if (error) {
        return error;
    }

    FD_CLR(socket, &d_writable);

    NTSO_SELECT_LOG_UPDATE(socket, d_readable, d_writable, d_exceptional);

    if (d_config.autoDetach().value()) {
        if (interest.wantNone()) {
            error = this->detachSocket(socket);
            if (error) {
                return error;
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error Select::wait(
    ntsa::EventSet*                                result,
    const bdlb::NullableValue<bsls::TimeInterval>& timeout)
{
    result->clear();

    ntsa::Error error;

    int rc;

    fd_set readable;
    fd_set writable;
    fd_set exceptional;

    Select::copy(&readable, d_readable);
    Select::copy(&writable, d_writable);
    Select::copy(&exceptional, d_exceptional);

    bsl::size_t maxDescriptor = d_maxHandle + 1;

    struct ::timeval* tvPtr = 0;
    struct ::timeval  tv;

    if (!timeout.isNull()) {
        const bsls::TimeInterval now = bdlt::CurrentTime::now();
        if (NTSCFG_LIKELY(timeout.value() > now)) {
            const bsls::TimeInterval delta = timeout.value() - now;

#if defined(BSLS_PLATFORM_OS_UNIX)
            tv.tv_sec  = static_cast<time_t>(delta.seconds());
            tv.tv_usec = static_cast<suseconds_t>(delta.nanoseconds() / 1000);
#else
            tv.tv_sec  = static_cast<long>(delta.seconds());
            tv.tv_usec = static_cast<long>(delta.nanoseconds() / 1000);
#endif

            NTSO_SELECT_LOG_WAIT_TIMED(delta.totalMilliseconds());
        }
        else {
            tv.tv_sec  = 0;
            tv.tv_usec = 0;

            NTSO_SELECT_LOG_WAIT_TIMED(0);
        }

        tvPtr = &tv;
    }
    else {
        NTSO_SELECT_LOG_WAIT_INDEFINITE();
    }

    rc = ::select(static_cast<int>(maxDescriptor),
                  &readable,
                  &writable,
                  &exceptional,
                  tvPtr);

    if (NTSCFG_LIKELY(rc > 0)) {
        NTSO_SELECT_LOG_WAIT_RESULT(rc);

        int numResults          = rc;
        int numResultsRemaining = numResults;

        ntsa::InterestSet::const_iterator it = d_interestSet.begin();
        ntsa::InterestSet::const_iterator et = d_interestSet.end();

        for (; it != et; ++it) {
            if (numResultsRemaining == 0) {
                break;
            }

            const ntsa::Interest interest = *it;
            const ntsa::Handle   socket   = interest.handle();

            NTSO_SELECT_LOG_EVENTS(socket, readable, writable, exceptional);

            ntsa::Event event;
            event.setHandle(socket);

            int numResultsFound = 0;

            if (FD_ISSET(socket, &readable)) {
                event.setReadable();
                ++numResultsFound;
            }

            if (FD_ISSET(socket, &writable)) {
                event.setWritable();
                ++numResultsFound;
            }

            if (FD_ISSET(socket, &exceptional)) {
                ntsa::Error lastError;
                ntsa::Error error =
                    ntsu::SocketOptionUtil::getLastError(&lastError, socket);
                if (!error && lastError) {
                    event.setError(lastError);
                }
                else {
                    event.setExceptional();
                }

                ++numResultsFound;
            }

            if (numResultsFound == 0) {
                continue;
            }

            numResultsRemaining -= numResultsFound;

            result->merge(event);
        }
    }
    else if (rc == 0) {
        NTSO_SELECT_LOG_WAIT_TIMEOUT();
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }
    else {
#if defined(BSLS_PLATFORM_OS_UNIX)
        int lastError = errno;
        if (lastError == EINTR) {
            NTSO_SELECT_LOG_WAIT_INTERRUPTED();
            return ntsa::Error();
        }
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
        DWORD lastError = WSAGetLastError();
        if (lastError == WSAEINTR) {
            NTSO_SELECT_LOG_WAIT_INTERRUPTED();
            return ntsa::Error();
        }
#else
#error Not implemented
#endif
        else {
            ntsa::Error error(lastError);
            NTSO_SELECT_LOG_WAIT_FAILURE(error);

            if (error == ntsa::Error(ntsa::Error::e_NOT_OPEN) ||
                error == ntsa::Error(ntsa::Error::e_NOT_SOCKET))
            {
                typedef bsl::vector<ntsa::Handle> HandleVector;
                HandleVector                      garbage;

                {
                    ntsa::InterestSet::const_iterator it =
                        d_interestSet.begin();
                    ntsa::InterestSet::const_iterator et = d_interestSet.end();

                    for (; it != et; ++it) {
                        const ntsa::Interest interest = *it;
                        const ntsa::Handle   socket   = interest.handle();

                        if (!ntsu::SocketUtil::isSocket(socket)) {
                            result->setError(socket, error);
                            garbage.push_back(socket);
                        }
                    }
                }

                {
                    HandleVector::const_iterator it = garbage.begin();
                    HandleVector::const_iterator et = garbage.end();

                    for (; it != et; ++it) {
                        const ntsa::Handle socket = *it;
                        this->detachSocket(socket);
                    }
                }

                return ntsa::Error();
            }
            else {
                return error;
            }
        }
    }

    return ntsa::Error();
}

bsl::shared_ptr<ntsi::Reactor> SelectUtil::createReactor(
    const ntsa::ReactorConfig& configuration,
    bslma::Allocator*          basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntso::Select> reactor;
    reactor.createInplace(allocator, configuration, allocator);

    return reactor;
}

}  // close package namespace
}  // close enterprise namespace
#endif
