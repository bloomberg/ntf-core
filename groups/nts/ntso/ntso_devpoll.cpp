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

#include <ntso_devpoll.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntso_devpoll_cpp, "$Id$ $CSID$")

#if NTSO_DEVPOLL_ENABLED

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
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <sys/devpoll.h>
#include <unistd.h>

#define NTSO_DEVPOLL_LOG_DEVICE_CREATE(fd)                                    \
    BSLS_LOG_TRACE("Event poll descriptor %d created", fd)

#define NTSO_DEVPOLL_LOG_DEVICE_CREATE_FAILURE(error)                         \
    BSLS_LOG_ERROR("Failed to create event poll descriptor: %s",              \
                   (error).text().c_str())

#define NTSO_DEVPOLL_LOG_DEVICE_CLOSE(fd)                                     \
    BSLS_LOG_TRACE("Event poll descriptor %d closed", fd)

#define NTSO_DEVPOLL_LOG_DEVICE_CLOSE_FAILURE(error)                          \
    BSLS_LOG_ERROR("Failed to close event poll descriptor: %s",               \
                   (error).text().c_str())

#define NTSO_DEVPOLL_LOG_WAIT_INDEFINITE()                                    \
    BSLS_LOG_TRACE("Polling for socket events indefinitely")

#define NTSO_DEVPOLL_LOG_WAIT_TIMED(timeout)                                  \
    BSLS_LOG_TRACE(                                                           \
        "Polling for sockets events or until %d milliseconds have elapsed",   \
        (int)(timeout))

#define NTSO_DEVPOLL_LOG_WAIT_FAILURE(error)                                  \
    BSLS_LOG_ERROR("Failed to poll for socket events: %s",                    \
                   error.text().c_str())

#define NTSO_DEVPOLL_LOG_WAIT_TIMEOUT()                                       \
    BSLS_LOG_TRACE("Timed out polling for socket events")

#define NTSO_DEVPOLL_LOG_WAIT_INTERRUPTED()                                   \
    BSLS_LOG_TRACE("Interrupted polling for socket events")

#define NTSO_DEVPOLL_LOG_WAIT_RESULT(numEvents)                               \
    BSLS_LOG_TRACE("Polled %d socket events", numEvents)

#define NTSO_DEVPOLL_LOG_EVENTS(handle, event)                                \
    BSLS_LOG_TRACE("Descriptor %d polled [%s%s%s%s%s ]",                      \
                   handle,                                                    \
                   ((((event).revents & POLLIN) != 0) ? " POLLIN" : ""),      \
                   ((((event).revents & POLLOUT) != 0) ? " POLLOUT" : ""),    \
                   ((((event).revents & POLLERR) != 0) ? " POLLERR" : ""),    \
                   ((((event).revents & POLLHUP) != 0) ? " POLLHUP" : ""),    \
                   ((((event).revents & POLLNVAL) != 0) ? " POLLNVAL" : ""))

#define NTSO_DEVPOLL_LOG_ADD(handle)                                          \
    BSLS_LOG_TRACE("Descriptor %d added", handle)

#define NTSO_DEVPOLL_LOG_ADD_FAILURE(handle, error)                           \
    BSLS_LOG_ERROR("Failed to add descriptor %d: %s",                         \
                   handle,                                                    \
                   (error).text().c_str())

#define NTSO_DEVPOLL_LOG_UPDATE(handle, interest)                             \
    BSLS_LOG_TRACE("Descriptor %d updated [%s%s ]",                           \
                   (handle),                                                  \
                   (((interest).wantReadable()) ? " POLLIN" : ""),            \
                   (((interest).wantWritable()) ? " POLLOUT" : ""))

#define NTSO_DEVPOLL_LOG_UPDATE_FAILURE(handle, error)                        \
    BSLS_LOG_ERROR("Failed to update descriptor %d: %s",                      \
                   (handle),                                                  \
                   (error).text().c_str())

#define NTSO_DEVPOLL_LOG_REMOVE(handle)                                       \
    BSLS_LOG_TRACE("Descriptor %d removed", handle)

#define NTSO_DEVPOLL_LOG_REMOVE_FAILURE(handle, error)                        \
    BSLS_LOG_ERROR("Failed to remove descriptor %d: %s",                      \
                   (handle),                                                  \
                   (error).text().c_str())

#if defined(BSLS_PLATFORM_CMP_SUN)
#pragma error_messages(off, SEC_NULL_PTR_DEREF)
#endif

namespace BloombergLP {
namespace ntso {

/// @brief @internal
/// Provide an implementation of the 'ntsi::Reactor' interface to poll for
/// socket events using the '/dev/poll' API on Solaris.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntso
class Devpoll : public ntsi::Reactor
{
    /// This typedef defines a vector of poll structures.
    typedef bsl::vector<struct ::pollfd> DescriptorVector;

    int                 d_device;
    ntsa::InterestSet   d_interestSet;
    DescriptorVector    d_outputList;
    ntsa::ReactorConfig d_config;
    bslma::Allocator*   d_allocator_p;

  private:
    Devpoll(const Devpoll&) BSLS_KEYWORD_DELETED;
    Devpoll& operator=(const Devpoll&) BSLS_KEYWORD_DELETED;

  private:
    /// Update the specified 'socket' to have the specified 'interset' in the
    /// device. Return the error.
    ntsa::Error update(ntsa::Handle socket, const ntsa::Interest& interest);

    /// Return the events that correspond to the specified 'interest'.
    static short specify(const ntsa::Interest& interest);

  public:
    /// Create a new reactor having the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit Devpoll(const ntsa::ReactorConfig& configuration,
                     bslma::Allocator*          basicAllocator = 0);

    /// Destroy this object.
    ~Devpoll() BSLS_KEYWORD_OVERRIDE;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Devpoll);
};

ntsa::Error Devpoll::update(ntsa::Handle          socket,
                            const ntsa::Interest& interest)
{
    int rc;

    struct ::pollfd pfd[2];

    pfd[0].fd      = socket;
    pfd[0].events  = POLLREMOVE;
    pfd[0].revents = 0;

    pfd[1].fd      = socket;
    pfd[1].events  = Devpoll::specify(interest);
    pfd[1].revents = 0;

    rc = ::write(d_device, pfd, 2 * sizeof(struct ::pollfd));
    if (rc != 2 * sizeof(struct ::pollfd)) {
        ntsa::Error error(errno);
        NTSO_DEVPOLL_LOG_UPDATE_FAILURE(socket, error);
        return error;
    }

    return ntsa::Error();
}

short Devpoll::specify(const ntsa::Interest& interest)
{
    short result = 0;

    if (interest.wantReadable()) {
        result |= POLLIN;
    }

    if (interest.wantWritable()) {
        result |= POLLOUT;
    }

    return result;
}

Devpoll::Devpoll(const ntsa::ReactorConfig& configuration,
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

    d_device = ::open("/dev/poll", O_RDWR);
    if (d_device < 0) {
        ntsa::Error error(errno);
        NTSO_DEVPOLL_LOG_DEVICE_CREATE_FAILURE(error);
        NTSCFG_ABORT();
    }

    NTSO_DEVPOLL_LOG_DEVICE_CREATE(d_device);
}

Devpoll::~Devpoll()
{
    int rc;

    if (d_device >= 0) {
        rc = ::close(d_device);
        if (rc != 0) {
            ntsa::Error error(errno);
            NTSO_DEVPOLL_LOG_DEVICE_CLOSE_FAILURE(error);
            NTSCFG_ABORT();
        }

        NTSO_DEVPOLL_LOG_DEVICE_CLOSE(d_device);
        d_device = ntsa::k_INVALID_HANDLE;
    }
}

ntsa::Error Devpoll::attachSocket(ntsa::Handle socket)
{
    ntsa::Error error;
    int         rc;

    error = d_interestSet.attach(socket);
    if (error) {
        return error;
    }

    {
        struct ::pollfd pfd;

        pfd.fd      = socket;
        pfd.events  = 0;
        pfd.revents = 0;

        rc = ::write(d_device, &pfd, sizeof(struct ::pollfd));
        if (rc != sizeof(struct ::pollfd)) {
            int lastError = errno;
            if (errno != EEXIST) {
                ntsa::Error error(errno);
                NTSO_DEVPOLL_LOG_ADD_FAILURE(socket, error);
                d_interestSet.detach(socket);
                return error;
            }
        }
    }

    NTSO_DEVPOLL_LOG_ADD(socket);

    return ntsa::Error();
}

ntsa::Error Devpoll::detachSocket(ntsa::Handle socket)
{
    ntsa::Error error;
    int         rc;

    error = d_interestSet.detach(socket);
    if (error) {
        return error;
    }

    {
        struct ::pollfd pfd;

        pfd.fd      = socket;
        pfd.events  = POLLREMOVE;
        pfd.revents = 0;

        rc = ::write(d_device, &pfd, sizeof(struct ::pollfd));
        if (rc != sizeof(struct ::pollfd)) {
            int lastError = errno;
            if (lastError != ENOENT) {
                ntsa::Error error(errno);
                NTSO_DEVPOLL_LOG_REMOVE_FAILURE(socket, error);
                return error;
            }
        }
    }

    NTSO_DEVPOLL_LOG_REMOVE(socket);

    return ntsa::Error();
}

ntsa::Error Devpoll::showReadable(ntsa::Handle socket)
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

    error = this->update(socket, interest);
    if (error) {
        return error;
    }

    NTSO_DEVPOLL_LOG_UPDATE(socket, interest);

    return ntsa::Error();
}

ntsa::Error Devpoll::showWritable(ntsa::Handle socket)
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

    error = this->update(socket, interest);
    if (error) {
        return error;
    }

    NTSO_DEVPOLL_LOG_UPDATE(socket, interest);

    return ntsa::Error();
}

ntsa::Error Devpoll::hideReadable(ntsa::Handle socket)
{
    ntsa::Error error;
    int         rc;

    ntsa::Interest interest;
    error = d_interestSet.hideReadable(&interest, socket);
    if (error) {
        return error;
    }

    error = this->update(socket, interest);
    if (error) {
        return error;
    }

    NTSO_DEVPOLL_LOG_UPDATE(socket, interest);

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

ntsa::Error Devpoll::hideWritable(ntsa::Handle socket)
{
    ntsa::Error error;
    int         rc;

    ntsa::Interest interest;
    error = d_interestSet.hideWritable(&interest, socket);
    if (error) {
        return error;
    }

    error = this->update(socket, interest);
    if (error) {
        return error;
    }

    NTSO_DEVPOLL_LOG_UPDATE(socket, interest);

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

ntsa::Error Devpoll::wait(
    ntsa::EventSet*                                result,
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

            NTSO_DEVPOLL_LOG_WAIT_TIMED(delta.totalMilliseconds());
        }
        else {
            timeoutInMilliseconds = 0;
            NTSO_DEVPOLL_LOG_WAIT_TIMED(0);
        }
    }
    else {
        NTSO_DEVPOLL_LOG_WAIT_INDEFINITE();
    }

    const bsl::size_t outputListSizeRequired = d_interestSet.numSockets();

    if (d_outputList.size() < outputListSizeRequired) {
        d_outputList.resize(outputListSizeRequired);
    }

    struct ::dvpoll dvp;

    dvp.dp_fds     = &d_outputList[0];
    dvp.dp_nfds    = static_cast<int>(d_outputList.size());
    dvp.dp_timeout = timeoutInMilliseconds;

    rc = ::ioctl(d_device, DP_POLL, &dvp);

    if (NTSCFG_LIKELY(rc > 0)) {
        NTSO_DEVPOLL_LOG_WAIT_RESULT(rc);

        int numResults          = rc;
        int numResultsRemaining = numResults;

        result->reserve(result->size() + numResults);

        DescriptorVector::const_iterator it = d_outputList.begin();
        DescriptorVector::const_iterator et = d_outputList.end();

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

            NTSO_DEVPOLL_LOG_EVENTS(e.fd, e);

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
                else {
                    event.setExceptional();
                    event.setHangup();
                }
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
                    event.setError(ntsa::Error(EBADF));
                }

                this->detachSocket(e.fd);
            }

            result->merge(event);
        }
    }
    else if (rc == 0) {
        NTSO_DEVPOLL_LOG_WAIT_TIMEOUT();
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }
    else {
        int lastError = errno;

        if (lastError == ETIME) {
            NTSO_DEVPOLL_LOG_WAIT_TIMEOUT();
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }
        else if (lastError == EINTR) {
            NTSO_DEVPOLL_LOG_WAIT_INTERRUPTED();
            return ntsa::Error();
        }
        else {
            ntsa::Error error = ntsa::Error::last();
            NTSO_DEVPOLL_LOG_WAIT_FAILURE(error);

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

bsl::shared_ptr<ntsi::Reactor> DevpollUtil::createReactor(
    const ntsa::ReactorConfig& configuration,
    bslma::Allocator*          basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntso::Devpoll> reactor;
    reactor.createInplace(allocator, configuration, allocator);

    return reactor;
}

}  // close package namespace
}  // close enterprise namespace
#endif
