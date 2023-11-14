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

#include <ntso_eventport.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntso_eventport_cpp, "$Id$ $CSID$")

#if NTSO_EVENTPORT_ENABLED

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

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <port.h>
#include <signal.h>
#include <sys/queue.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define NTSO_EVENTPORT_LOG_DEVICE_CREATE(fd)                                  \
    BSLS_LOG_TRACE("Event poll descriptor %d created", fd)

#define NTSO_EVENTPORT_LOG_DEVICE_CREATE_FAILURE(error)                       \
    BSLS_LOG_ERROR("Failed to create event poll descriptor: %s",              \
                   (error).text().c_str())

#define NTSO_EVENTPORT_LOG_DEVICE_CLOSE(fd)                                   \
    BSLS_LOG_TRACE("Event poll descriptor %d closed", fd)

#define NTSO_EVENTPORT_LOG_DEVICE_CLOSE_FAILURE(error)                        \
    BSLS_LOG_ERROR("Failed to close event poll descriptor: %s",               \
                   (error).text().c_str())

#define NTSO_EVENTPORT_LOG_WAIT_INDEFINITE()                                  \
    BSLS_LOG_TRACE("Polling for socket events indefinitely")

#define NTSO_EVENTPORT_LOG_WAIT_TIMED(timeout)                                \
    BSLS_LOG_TRACE(                                                           \
        "Polling for sockets events or until %d milliseconds have elapsed",   \
        (int)(timeout))

#define NTSO_EVENTPORT_LOG_WAIT_FAILURE(error)                                \
    BSLS_LOG_ERROR("Failed to poll for socket events: %s",                    \
                   error.text().c_str())

#define NTSO_EVENTPORT_LOG_WAIT_TIMEOUT()                                     \
    BSLS_LOG_TRACE("Timed out polling for socket events")

#define NTSO_EVENTPORT_LOG_WAIT_INTERRUPTED()                                 \
    BSLS_LOG_TRACE("Interrupted polling for socket events")

#define NTSO_EVENTPORT_LOG_WAIT_RESULT(numEvents)                             \
    BSLS_LOG_TRACE("Polled %d socket events", numEvents)

#define NTSO_EVENTPORT_LOG_EVENTS(handle, events)                             \
    BSLS_LOG_TRACE("Descriptor %d polled%s%s%s%s%s",                          \
                   handle,                                                    \
                   ((((events) & POLLIN) != 0) ? " POLLIN" : ""),             \
                   ((((events) & POLLOUT) != 0) ? " POLLOUT" : ""),           \
                   ((((events) & POLLERR) != 0) ? " POLLERR" : ""),           \
                   ((((events) & POLLHUP) != 0) ? " POLLHUP" : ""),           \
                   ((((events) & POLLNVAL) != 0) ? " POLLNVAL" : ""))

#define NTSO_EVENTPORT_LOG_ADD(handle)                                        \
    BSLS_LOG_TRACE("Descriptor %d added", handle)

#define NTSO_EVENTPORT_LOG_ADD_FAILURE(handle, error)                         \
    BSLS_LOG_ERROR("Failed to add descriptor %d: %s",                         \
                   handle,                                                    \
                   (error).text().c_str())

#define NTSO_EVENTPORT_LOG_UPDATE(handle, interest)                           \
    BSLS_LOG_TRACE("Descriptor %d updated [%s%s ]",                           \
                   (handle),                                                  \
                   (((interest).wantReadable()) ? " POLLIN" : ""),            \
                   (((interest).wantWritable()) ? " POLLOUT" : ""))

#define NTSO_EVENTPORT_LOG_UPDATE_FAILURE(handle, error)                      \
    BSLS_LOG_ERROR("Failed to update descriptor %d: %s",                      \
                   (handle),                                                  \
                   (error).text().c_str())

#define NTSO_EVENTPORT_LOG_REMOVE(handle)                                     \
    BSLS_LOG_TRACE("Descriptor %d removed", handle)

#define NTSO_EVENTPORT_LOG_REMOVE_FAILURE(handle, error)                      \
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
/// socket events using the 'port' API on Solaris.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntso
class EventPort : public ntsi::Reactor
{
    /// This typedef defines a vector of poll structures.
    typedef bsl::vector<port_event_t> DescriptorVector;

    int                 d_device;
    ntsa::InterestSet   d_interestSet;
    DescriptorVector    d_outputList;
    ntsa::ReactorConfig d_config;
    bslma::Allocator*   d_allocator_p;

  private:
    EventPort(const EventPort&) BSLS_KEYWORD_DELETED;
    EventPort& operator=(const EventPort&) BSLS_KEYWORD_DELETED;

  private:
    /// Update the specified 'socket' to have the specified 'interset' in the
    /// device. Return the error.
    ntsa::Error update(ntsa::Handle          socket,
                       const ntsa::Interest& interest);

    /// Return the events that correspond to the specified 'interest'.
    static int specify(const ntsa::Interest& interest);

  public:
    /// Create a new reactor having the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit EventPort(const ntsa::ReactorConfig& configuration,
                       bslma::Allocator*          basicAllocator = 0);

    /// Destroy this object.
    ~EventPort() BSLS_KEYWORD_OVERRIDE;

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
    ntsa::Error wait(
        ntsa::EventSet*                                result,
        const bdlb::NullableValue<bsls::TimeInterval>& deadline)
        BSLS_KEYWORD_OVERRIDE;
};

ntsa::Error EventPort::update(ntsa::Handle          socket,
                              const ntsa::Interest& interest)
{
    int rc;

    const int events = EventPort::specify(interest);

    rc = port_associate(d_device, PORT_SOURCE_FD, socket, events, 0);
    if (rc != 0) {
        int lastError = errno;
        ntsa::Error error(errno);
        NTSO_EVENTPORT_LOG_UPDATE_FAILURE(socket, error);
        return error;
    }

    return ntsa::Error();
}

int EventPort::specify(const ntsa::Interest& interest)
{
    int result = 0;

    if (interest.wantReadable()) {
        result |= POLLIN;
    }

    if (interest.wantWritable()) {
        result |= POLLOUT;
    }

    return result;
}

EventPort::EventPort(const ntsa::ReactorConfig& configuration,
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

    d_device = ::port_create();
    if (d_device < 0) {
        ntsa::Error error(errno);
        NTSO_EVENTPORT_LOG_DEVICE_CREATE_FAILURE(error);
        NTSCFG_ABORT();
    }

    NTSO_EVENTPORT_LOG_DEVICE_CREATE(d_device);
}

EventPort::~EventPort()
{
    int rc;

    if (d_device >= 0) {
        rc = ::close(d_device);
        if (rc != 0) {
            ntsa::Error error(errno);
            NTSO_EVENTPORT_LOG_DEVICE_CLOSE_FAILURE(error);
            NTSCFG_ABORT();
        }

        NTSO_EVENTPORT_LOG_DEVICE_CLOSE(d_device);
        d_device = ntsa::k_INVALID_HANDLE;
    }
}

ntsa::Error EventPort::attachSocket(ntsa::Handle socket)
{
    ntsa::Error error;
    int rc;

    error = d_interestSet.attach(socket);
    if (error) {
        return error;
    }

    rc = port_associate(d_device, PORT_SOURCE_FD, socket, 0, 0);
    if (rc != 0) {
        int lastError = errno;
        if (lastError != EEXIST) {
            ntsa::Error error(errno);
            NTSO_EVENTPORT_LOG_ADD_FAILURE(socket, error);
            d_interestSet.detach(socket);
            return error;
        }
    }

    NTSO_EVENTPORT_LOG_ADD(socket);

    return ntsa::Error();
}

ntsa::Error EventPort::detachSocket(ntsa::Handle socket)
{
    ntsa::Error error;
    int rc;

    error = d_interestSet.detach(socket);
    if (error) {
        return error;
    }

    rc = port_dissociate(d_device, PORT_SOURCE_FD, socket);
    if (rc != 0) {
        int lastError = errno;
        if (lastError != ENOENT) {
            ntsa::Error error(lastError);
            NTSO_EVENTPORT_LOG_REMOVE_FAILURE(socket, error);
            return error;
        }
    }

    NTSO_EVENTPORT_LOG_REMOVE(socket);

    return ntsa::Error();
}

ntsa::Error EventPort::showReadable(ntsa::Handle socket)
{
    ntsa::Error error;
    int rc;

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

    NTSO_EVENTPORT_LOG_UPDATE(socket, interest);

    return ntsa::Error();
}

ntsa::Error EventPort::showWritable(ntsa::Handle socket)
{
    ntsa::Error error;
    int rc;

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

    NTSO_EVENTPORT_LOG_UPDATE(socket, interest);

    return ntsa::Error();
}

ntsa::Error EventPort::hideReadable(ntsa::Handle socket)
{
    ntsa::Error error;
    int rc;

    ntsa::Interest interest;
    error = d_interestSet.hideReadable(&interest, socket);
    if (error) {
        return error;
    }

    error = this->update(socket, interest);
    if (error) {
        return error;
    }

    NTSO_EVENTPORT_LOG_UPDATE(socket, interest);

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

ntsa::Error EventPort::hideWritable(ntsa::Handle socket)
{
    ntsa::Error error;
    int rc;

    ntsa::Interest interest;
    error = d_interestSet.hideWritable(&interest, socket);
    if (error) {
        return error;
    }

    error = this->update(socket, interest);
    if (error) {
        return error;
    }

    NTSO_EVENTPORT_LOG_UPDATE(socket, interest);

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

ntsa::Error EventPort::wait(
    ntsa::EventSet*                                result,
    const bdlb::NullableValue<bsls::TimeInterval>& timeout)
{
    ntsa::Error error;
    int         rc;

    result->clear();

    struct ::timespec ts;
    struct ::timespec* tsPtr = 0;

    if (!timeout.isNull()) {
        const bsls::TimeInterval now = bdlt::CurrentTime::now();
        if (NTSCFG_LIKELY(timeout.value() > now)) {
            const bsls::TimeInterval delta = timeout.value() - now;
            ts.tv_sec = delta.seconds();
            ts.tv_nsec = delta.nanoseconds();

            NTSO_EVENTPORT_LOG_WAIT_TIMED(delta.totalMilliseconds());
        }
        else {
            ts.tv_sec = 0;
            ts.tv_nsec = 0;

            NTSO_EVENTPORT_LOG_WAIT_TIMED(0);
        }

        tsPtr = &ts;
    }
    else {
        NTSO_EVENTPORT_LOG_WAIT_INDEFINITE();
    }

    const bsl::size_t outputListSizeRequired = d_interestSet.numSockets();

    if (d_outputList.size() < outputListSizeRequired) {
        d_outputList.resize(outputListSizeRequired);
    }

    uint_t eventCount = 1;
    rc = ::port_getn(d_device,
                     &d_outputList[0],
                     d_outputList.size(),
                     &eventCount,
                     tsPtr);

    if (rc == 0 && eventCount > 0) {
        result->reserve(result->size() + static_cast<bsl::size_t>(eventCount));

        for (uint_t eventIndex = 0; eventIndex < eventCount; ++eventIndex) {
            port_event_t e = d_outputList[eventIndex];

            BSLS_ASSERT(e.portev_source == PORT_SOURCE_FD);

            ntsa::Handle socket = e.portev_object;
            BSLS_ASSERT(socket != ntsa::k_INVALID_HANDLE);

            BSLS_ASSERT(e.portev_events != 0);
            int events = e.portev_events;

            ntsa::Interest interest;
            if (!d_interestSet.find(&interest, socket)) {
                continue;
            }

            NTSO_EVENTPORT_LOG_EVENTS(socket, events);

            ntsa::Event event;
            event.setHandle(socket);

            if ((events & POLLIN) != 0) {
                event.setReadable();
            }

            if ((events & POLLOUT) != 0) {
                event.setWritable();
            }

            if ((events & POLLHUP) != 0) {
                event.setHangup();
            }

            if ((events & POLLERR) != 0) {
                ntsa::Error lastError;
                ntsa::Error error =
                    ntsu::SocketOptionUtil::getLastError(&lastError, socket);
                if (!error && lastError) {
                    event.setError(lastError);
                }
                else {
                    event.setExceptional();
                    event.setHangup();
                }
            }

            if ((events & POLLNVAL) != 0) {
                ntsa::Error lastError;
                ntsa::Error error =
                    ntsu::SocketOptionUtil::getLastError(&lastError, socket);

                if (error) {
                    event.setError(error);
                }
                else if (lastError) {
                    event.setError(lastError);
                }
                else {
                    event.setError(ntsa::Error(EBADF));
                }

                this->detachSocket(socket);
            }

            result->merge(event);

            this->update(socket, interest);
        }
    }
    else if (rc == 0 && eventCount == 0) {
        NTSO_EVENTPORT_LOG_WAIT_TIMEOUT();
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }
    else {
        int lastError = errno;

        if (lastError == ETIME) {
            NTSO_EVENTPORT_LOG_WAIT_TIMEOUT();
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }
        else if (lastError == EINTR) {
            NTSO_EVENTPORT_LOG_WAIT_INTERRUPTED();
            return ntsa::Error();
        }
        else {
            ntsa::Error error(lastError);
            NTSO_EVENTPORT_LOG_WAIT_FAILURE(error);

            if (error == ntsa::Error(ntsa::Error::e_NOT_OPEN) ||
                error == ntsa::Error(ntsa::Error::e_NOT_SOCKET))
            {
                typedef bsl::vector<ntsa::Handle> HandleVector;
                HandleVector garbage;

                {
                    ntsa::InterestSet::const_iterator it =
                        d_interestSet.begin();
                    ntsa::InterestSet::const_iterator et =
                        d_interestSet.end();

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

bsl::shared_ptr<ntsi::Reactor> EventPortUtil::createReactor(
    const ntsa::ReactorConfig& configuration,
    bslma::Allocator*          basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntso::EventPort> reactor;
    reactor.createInplace(allocator, configuration, allocator);

    return reactor;
}

}  // close package namespace
}  // close enterprise namespace
#endif
