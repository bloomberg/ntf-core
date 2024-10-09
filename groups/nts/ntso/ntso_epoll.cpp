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

#include <ntso_epoll.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntso_epoll_cpp, "$Id$ $CSID$")

#if NTSO_EPOLL_ENABLED

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

#include <errno.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>

#define NTSO_EPOLL_LOG_DEVICE_CREATE(fd)                                      \
    BSLS_LOG_TRACE("Event poll descriptor %d created", fd)

#define NTSO_EPOLL_LOG_DEVICE_CREATE_FAILURE(error)                           \
    BSLS_LOG_ERROR("Failed to create event poll descriptor: %s",              \
                   (error).text().c_str())

#define NTSO_EPOLL_LOG_DEVICE_CLOSE(fd)                                       \
    BSLS_LOG_TRACE("Event poll descriptor %d closed", fd)

#define NTSO_EPOLL_LOG_DEVICE_CLOSE_FAILURE(error)                            \
    BSLS_LOG_ERROR("Failed to close event poll descriptor: %s",               \
                   (error).text().c_str())

#define NTSO_EPOLL_LOG_WAIT_INDEFINITE()                                      \
    BSLS_LOG_TRACE("Polling for socket events indefinitely")

#define NTSO_EPOLL_LOG_WAIT_TIMED(timeout)                                    \
    BSLS_LOG_TRACE(                                                           \
        "Polling for sockets events or until %d milliseconds have elapsed",   \
        (int)(timeout))

#define NTSO_EPOLL_LOG_WAIT_FAILURE(error)                                    \
    BSLS_LOG_ERROR("Failed to poll for socket events: %s",                    \
                   error.text().c_str())

#define NTSO_EPOLL_LOG_WAIT_TIMEOUT()                                         \
    BSLS_LOG_TRACE("Timed out polling for socket events")

#define NTSO_EPOLL_LOG_WAIT_INTERRUPTED()                                     \
    BSLS_LOG_TRACE("Interrupted polling for socket events")

#define NTSO_EPOLL_LOG_WAIT_RESULT(numEvents)                                 \
    BSLS_LOG_TRACE("Polled %d socket events", numEvents)

#define NTSO_EPOLL_LOG_EVENTS(handle, event)                                  \
    BSLS_LOG_TRACE(                                                           \
        "Descriptor %d polled [%s%s%s%s%s%s%s%s ]",                           \
        event.data.fd,                                                        \
        ((((event).events & EPOLLIN) != 0) ? " EPOLLIN" : ""),                \
        ((((event).events & EPOLLOUT) != 0) ? " EPOLLOUT" : ""),              \
        ((((event).events & EPOLLERR) != 0) ? " EPOLLERR" : ""),              \
        ((((event).events & EPOLLHUP) != 0) ? " EPOLLHUP" : ""),              \
        ((((event).events & EPOLLRDHUP) != 0) ? " EPOLLRDHUP" : ""),          \
        ((((event).events & EPOLLPRI) != 0) ? " EPOLLPRI" : ""),              \
        ((((event).events & EPOLLET) != 0) ? " EPOLLET" : ""),                \
        ((((event).events & EPOLLONESHOT) != 0) ? " EPOLLONESHOT" : ""))

#define NTSO_EPOLL_LOG_ADD(handle)                                            \
    BSLS_LOG_TRACE("Descriptor %d added", handle)

#define NTSO_EPOLL_LOG_ADD_FAILURE(handle, error)                             \
    BSLS_LOG_ERROR("Failed to add descriptor %d: %s",                         \
                   handle,                                                    \
                   (error).text().c_str())

#define NTSO_EPOLL_LOG_UPDATE(handle, interest)                               \
    BSLS_LOG_TRACE("Descriptor %d updated [%s%s ]",                           \
                   (handle),                                                  \
                   (((interest).wantReadable()) ? " EPOLLIN" : ""),           \
                   (((interest).wantWritable()) ? " EPOLLOUT" : ""))

#define NTSO_EPOLL_LOG_UPDATE_FAILURE(handle, error)                          \
    BSLS_LOG_ERROR("Failed to update descriptor %d: %s",                      \
                   (handle),                                                  \
                   (error).text().c_str())

#define NTSO_EPOLL_LOG_REMOVE(handle)                                         \
    BSLS_LOG_TRACE("Descriptor %d removed", handle)

#define NTSO_EPOLL_LOG_REMOVE_FAILURE(handle, error)                          \
    BSLS_LOG_ERROR("Failed to remove descriptor %d: %s",                      \
                   (handle),                                                  \
                   (error).text().c_str())

namespace BloombergLP {
namespace ntso {

/// @brief @internal
/// Provide an implementation of the 'ntsi::Reactor' interface to poll for
/// socket events using the 'epoll' API on Linux.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntso
class Epoll : public ntsi::Reactor
{
    typedef bsl::vector<struct ::epoll_event> EventList;

    int                 d_device;
    ntsa::InterestSet   d_interestSet;
    EventList           d_outputList;
    ntsa::ReactorConfig d_config;
    bslma::Allocator*   d_allocator_p;

  private:
    Epoll(const Epoll&) BSLS_KEYWORD_DELETED;
    Epoll& operator=(const Epoll&) BSLS_KEYWORD_DELETED;

  private:
    /// Return the events that correspond to the specified 'interest'.
    static bsl::uint32_t specify(const ntsa::Interest& interest);

  public:
    /// Create a new reactor having the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit Epoll(const ntsa::ReactorConfig& configuration,
                   bslma::Allocator*          basicAllocator = 0);

    /// Destroy this object.
    ~Epoll() BSLS_KEYWORD_OVERRIDE;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Epoll);
};

bsl::uint32_t Epoll::specify(const ntsa::Interest& interest)
{
    bsl::uint32_t result = 0;

    if (interest.wantReadable()) {
        result |= EPOLLIN;
    }

    if (interest.wantWritable()) {
        result |= EPOLLOUT;
    }

    return result;
}

Epoll::Epoll(const ntsa::ReactorConfig& configuration,
             bslma::Allocator*          basicAllocator)
: d_device(ntsa::k_INVALID_HANDLE)
, d_interestSet(basicAllocator)
, d_outputList(basicAllocator)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_config.autoAttach().isNull()) {
        d_config.setAutoAttach(false);
    }

    if (d_config.autoDetach().isNull()) {
        d_config.setAutoDetach(false);
    }

    d_device = ::epoll_create1(EPOLL_CLOEXEC);
    if (d_device < 0) {
        ntsa::Error error(errno);
        NTSO_EPOLL_LOG_DEVICE_CREATE_FAILURE(error);
        NTSCFG_ABORT();
    }

    NTSO_EPOLL_LOG_DEVICE_CREATE(d_device);
}

Epoll::~Epoll()
{
    int rc;

    if (d_device >= 0) {
        rc = ::close(d_device);
        if (rc != 0) {
            ntsa::Error error(errno);
            NTSO_EPOLL_LOG_DEVICE_CLOSE_FAILURE(error);
            NTSCFG_ABORT();
        }

        NTSO_EPOLL_LOG_DEVICE_CLOSE(d_device);
        d_device = ntsa::k_INVALID_HANDLE;
    }
}

ntsa::Error Epoll::attachSocket(ntsa::Handle socket)
{
    ntsa::Error error;
    int         rc;

    error = d_interestSet.attach(socket);
    if (error) {
        return error;
    }

    {
        ::epoll_event e;

        e.data.fd = socket;
        e.events  = 0;

        rc = ::epoll_ctl(d_device, EPOLL_CTL_ADD, socket, &e);
        if (rc != 0) {
            int lastError = errno;
            if (lastError != EEXIST) {
                error = ntsa::Error(lastError);
                NTSO_EPOLL_LOG_ADD_FAILURE(socket, error);
                d_interestSet.detach(socket);
                return error;
            }
        }
    }

    NTSO_EPOLL_LOG_ADD(socket);

    return ntsa::Error();
}

ntsa::Error Epoll::detachSocket(ntsa::Handle socket)
{
    ntsa::Error error;
    int         rc;

    error = d_interestSet.detach(socket);
    if (error) {
        return error;
    }

    {
        ::epoll_event e;

        e.data.fd = socket;
        e.events  = 0;

        rc = ::epoll_ctl(d_device, EPOLL_CTL_DEL, socket, &e);
        if (rc != 0) {
            int lastError = errno;
            if (lastError != ENOENT) {
                error = ntsa::Error(lastError);
                NTSO_EPOLL_LOG_REMOVE_FAILURE(socket, error);
                return error;
            }
        }
    }

    NTSO_EPOLL_LOG_REMOVE(socket);

    return ntsa::Error();
}

ntsa::Error Epoll::showReadable(ntsa::Handle socket)
{
    ntsa::Error error;
    int         rc;

    if (d_config.autoAttach().value()) {
        if (!d_interestSet.contains(socket)) {
            error = this->attachSocket(socket);
            if (error) {
                return error;
            }
        }
    }

    ntsa::Interest interest;
    error = d_interestSet.showReadable(&interest, socket);
    if (error) {
        return error;
    }

    {
        ::epoll_event e;

        e.data.fd = socket;
        e.events  = Epoll::specify(interest);

        rc = ::epoll_ctl(d_device, EPOLL_CTL_MOD, socket, &e);
        if (rc != 0) {
            error = ntsa::Error(errno);
            NTSO_EPOLL_LOG_UPDATE_FAILURE(socket, error);
            d_interestSet.hideReadable(socket);
            return error;
        }
    }

    NTSO_EPOLL_LOG_UPDATE(socket, interest);

    return ntsa::Error();
}

ntsa::Error Epoll::showWritable(ntsa::Handle socket)
{
    ntsa::Error error;
    int         rc;

    if (d_config.autoAttach().value()) {
        if (!d_interestSet.contains(socket)) {
            error = this->attachSocket(socket);
            if (error) {
                return error;
            }
        }
    }

    ntsa::Interest interest;
    error = d_interestSet.showWritable(&interest, socket);
    if (error) {
        return error;
    }

    {
        ::epoll_event e;

        e.data.fd = socket;
        e.events  = Epoll::specify(interest);

        rc = ::epoll_ctl(d_device, EPOLL_CTL_MOD, socket, &e);
        if (rc != 0) {
            error = ntsa::Error(errno);
            NTSO_EPOLL_LOG_UPDATE_FAILURE(socket, error);
            d_interestSet.hideWritable(socket);
            return error;
        }
    }

    NTSO_EPOLL_LOG_UPDATE(socket, interest);

    return ntsa::Error();
}

ntsa::Error Epoll::hideReadable(ntsa::Handle socket)
{
    ntsa::Error error;
    int         rc;

    ntsa::Interest interest;
    error = d_interestSet.hideReadable(&interest, socket);
    if (error) {
        return error;
    }

    {
        ::epoll_event e;

        e.data.fd = socket;
        e.events  = Epoll::specify(interest);

        rc = ::epoll_ctl(d_device, EPOLL_CTL_MOD, socket, &e);
        if (rc != 0) {
            error = ntsa::Error(errno);
            NTSO_EPOLL_LOG_UPDATE_FAILURE(socket, error);
            d_interestSet.showReadable(socket);
            return error;
        }
    }

    NTSO_EPOLL_LOG_UPDATE(socket, interest);

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

ntsa::Error Epoll::hideWritable(ntsa::Handle socket)
{
    ntsa::Error error;
    int         rc;

    ntsa::Interest interest;
    error = d_interestSet.hideWritable(&interest, socket);
    if (error) {
        return error;
    }

    {
        ::epoll_event e;

        e.data.fd = socket;
        e.events  = Epoll::specify(interest);

        rc = ::epoll_ctl(d_device, EPOLL_CTL_MOD, socket, &e);
        if (rc != 0) {
            error = ntsa::Error(errno);
            NTSO_EPOLL_LOG_UPDATE_FAILURE(e.data.fd, error);
            d_interestSet.showWritable(socket);
            return error;
        }
    }

    NTSO_EPOLL_LOG_UPDATE(socket, interest);

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

ntsa::Error Epoll::wait(ntsa::EventSet*                                result,
                        const bdlb::NullableValue<bsls::TimeInterval>& timeout)
{
    ntsa::Error error;
    int         rc;

    result->clear();

    int timeoutInMilliseconds = -1;
    if (!timeout.isNull()) {
        const bsls::TimeInterval now = bdlt::CurrentTime::now();
        if (NTSCFG_LIKELY(timeout.value() > now)) {
            const bsls::TimeInterval delta = timeout.value() - now;

            timeoutInMilliseconds =
                static_cast<int>(delta.totalMilliseconds());

            NTSO_EPOLL_LOG_WAIT_TIMED(delta.totalMilliseconds());
        }
        else {
            timeoutInMilliseconds = 0;
            NTSO_EPOLL_LOG_WAIT_TIMED(0);
        }
    }
    else {
        NTSO_EPOLL_LOG_WAIT_INDEFINITE();
    }

    const bsl::size_t outputListSizeRequired = d_interestSet.numSockets();

    if (d_outputList.size() < outputListSizeRequired) {
        d_outputList.resize(outputListSizeRequired);
    }

    rc = ::epoll_wait(d_device,
                      &d_outputList[0],
                      static_cast<int>(d_outputList.size()),
                      timeoutInMilliseconds);

    if (NTSCFG_LIKELY(rc > 0)) {
        NTSO_EPOLL_LOG_WAIT_RESULT(rc);

        const int numEvents = rc;
        result->reserve(result->size() + static_cast<bsl::size_t>(numEvents));

        for (int eventIndex = 0; eventIndex < numEvents; ++eventIndex) {
            ::epoll_event e = d_outputList[eventIndex];

            NTSO_EPOLL_LOG_EVENTS(e.data.fd, e);

            ntsa::Event event;
            event.setHandle(e.data.fd);

            if ((e.events & EPOLLIN) != 0) {
                event.setReadable();
            }

            if ((e.events & EPOLLOUT) != 0) {
                event.setWritable();
            }

            if ((e.events & EPOLLERR) != 0) {
                ntsa::Error lastError;
                error = ntsu::SocketOptionUtil::getLastError(&lastError,
                                                             e.data.fd);
                if (!error && lastError) {
                    event.setError(lastError);
                }
                else {
                    event.setExceptional();
                    event.setHangup();
                }
            }

            if ((e.events & EPOLLRDHUP) != 0) {
                event.setShutdown();
            }

            if ((e.events & EPOLLHUP) != 0) {
                event.setHangup();
            }

            result->merge(event);
        }
    }
    else if (rc == 0) {
        NTSO_EPOLL_LOG_WAIT_TIMEOUT();
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }
    else {
        int lastError = errno;
        if (lastError == EINTR) {
            NTSO_EPOLL_LOG_WAIT_INTERRUPTED();
            return ntsa::Error();
        }
        else {
            ntsa::Error error(lastError);
            NTSO_EPOLL_LOG_WAIT_FAILURE(error);
            return error;
        }
    }

    return ntsa::Error();
}

bsl::shared_ptr<ntsi::Reactor> EpollUtil::createReactor(
    const ntsa::ReactorConfig& configuration,
    bslma::Allocator*          basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntso::Epoll> reactor;
    reactor.createInplace(allocator, configuration, allocator);

    return reactor;
}

}  // close package namespace
}  // close enterprise namespace
#endif
