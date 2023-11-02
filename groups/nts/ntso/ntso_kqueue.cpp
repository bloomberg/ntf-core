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

#include <ntso_kqueue.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntso_kqueue_cpp, "$Id$ $CSID$")

#if NTSO_KQUEUE_ENABLED

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
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define NTSO_KQUEUE_LOG_DEVICE_CREATE(fd)                                     \
    BSLS_LOG_TRACE("Reactor 'kqueue' device descriptor %d created", fd)

#define NTSO_KQUEUE_LOG_DEVICE_CREATE_FAILURE(error)                          \
    BSLS_LOG_ERROR("Reactor 'kqueue' device descriptor failed to create: %s", \
                   (error).text().c_str())

#define NTSO_KQUEUE_LOG_DEVICE_CLOSE(fd)                                      \
    BSLS_LOG_TRACE("Reactor 'kqueue' device descriptor %d closed", fd)

#define NTSO_KQUEUE_LOG_DEVICE_CLOSE_FAILURE(fd, error)                       \
    BSLS_LOG_ERROR(                                                           \
        "Reactor 'kqueue' device descriptor %d failed to close: %s",          \
        fd,                                                                   \
        (error).text().c_str())

#define NTSO_KQUEUE_LOG_WAIT_INDEFINITE()                                     \
    BSLS_LOG_TRACE("Polling for socket events indefinitely")

#define NTSO_KQUEUE_LOG_WAIT_TIMED(timeout)                                   \
    BSLS_LOG_TRACE(                                                           \
        "Polling for sockets events or until %d milliseconds have elapsed",   \
        (int)(timeout))

#define NTSO_KQUEUE_LOG_WAIT_FAILURE(error)                                   \
    BSLS_LOG_ERROR("Failed to poll for socket events: %s",                    \
                   error.text().c_str())

#define NTSO_KQUEUE_LOG_WAIT_TIMEOUT()                                        \
    BSLS_LOG_TRACE("Timed out polling for socket events")

#define NTSO_KQUEUE_LOG_WAIT_RESULT(numEvents)                                \
    BSLS_LOG_TRACE("Polled %d socket events", numEvents)

#define NTSO_KQUEUE_LOG_EVENT_APPLY(kevent)                                   \
    BSLS_LOG_TRACE(                                                           \
        "Reactor 'kqueue' device applying socket descriptor %d update "       \
        "filter%s%s%s%s%s%s%s flags%s%s%s%s%s%s%s%s%s "                       \
        "(%zu) fflags %zu data %zu udata %zu",                                \
        (int)((kevent).ident),                                                \
        ((((kevent).filter == EVFILT_READ)) ? " READ" : ""),                  \
        ((((kevent).filter == EVFILT_WRITE)) ? " WRITE" : ""),                \
        ((((kevent).filter == EVFILT_AIO)) ? " AIO" : ""),                    \
        ((((kevent).filter == EVFILT_VNODE)) ? " VNODE" : ""),                \
        ((((kevent).filter == EVFILT_PROC)) ? " PROC" : ""),                  \
        ((((kevent).filter == EVFILT_SIGNAL)) ? " SIGNAL" : ""),              \
        ((((kevent).filter == EVFILT_TIMER)) ? " TIMER" : ""),                \
        ((((kevent).flags & EV_ADD) != 0) ? " ADD" : ""),                     \
        ((((kevent).flags & EV_ENABLE) != 0) ? " ENABLE" : ""),               \
        ((((kevent).flags & EV_DISABLE) != 0) ? " DISABLE" : ""),             \
        ((((kevent).flags & EV_DELETE) != 0) ? " DELETE" : ""),               \
        ((((kevent).flags & EV_RECEIPT) != 0) ? " RECEIPT" : ""),             \
        ((((kevent).flags & EV_ONESHOT) != 0) ? " ONESHOT" : ""),             \
        ((((kevent).flags & EV_CLEAR) != 0) ? " CLEAR" : ""),                 \
        ((((kevent).flags & EV_EOF) != 0) ? " EOF" : ""),                     \
        ((((kevent).flags & EV_ERROR) != 0) ? " ERROR" : ""),                 \
        (bsl::size_t)((kevent).flags),                                        \
        (bsl::size_t)((kevent).fflags),                                       \
        (bsl::size_t)((kevent).data),                                         \
        (bsl::size_t)((kevent).udata))

#define NTSO_KQUEUE_LOG_EVENT_POLL(kevent)                                    \
    BSLS_LOG_TRACE(                                                           \
        "Reactor 'kqueue' device polled socket descriptor %d event "          \
        "filter%s%s%s%s%s%s%s flags%s%s%s%s%s%s%s%s%s "                       \
        "(%zu) fflags %zu data %zu udata %zu",                                \
        (int)((kevent).ident),                                                \
        ((((kevent).filter == EVFILT_READ)) ? " READ" : ""),                  \
        ((((kevent).filter == EVFILT_WRITE)) ? " WRITE" : ""),                \
        ((((kevent).filter == EVFILT_AIO)) ? " AIO" : ""),                    \
        ((((kevent).filter == EVFILT_VNODE)) ? " VNODE" : ""),                \
        ((((kevent).filter == EVFILT_PROC)) ? " PROC" : ""),                  \
        ((((kevent).filter == EVFILT_SIGNAL)) ? " SIGNAL" : ""),              \
        ((((kevent).filter == EVFILT_TIMER)) ? " TIMER" : ""),                \
        ((((kevent).flags & EV_ADD) != 0) ? " ADD" : ""),                     \
        ((((kevent).flags & EV_ENABLE) != 0) ? " ENABLE" : ""),               \
        ((((kevent).flags & EV_DISABLE) != 0) ? " DISABLE" : ""),             \
        ((((kevent).flags & EV_DELETE) != 0) ? " DELETE" : ""),               \
        ((((kevent).flags & EV_RECEIPT) != 0) ? " RECEIPT" : ""),             \
        ((((kevent).flags & EV_ONESHOT) != 0) ? " ONESHOT" : ""),             \
        ((((kevent).flags & EV_CLEAR) != 0) ? " CLEAR" : ""),                 \
        ((((kevent).flags & EV_EOF) != 0) ? " EOF" : ""),                     \
        ((((kevent).flags & EV_ERROR) != 0) ? " ERROR" : ""),                 \
        (bsl::size_t)((kevent).flags),                                        \
        (bsl::size_t)((kevent).fflags),                                       \
        (bsl::size_t)((kevent).data),                                         \
        (bsl::size_t)((kevent).udata))

#define NTSO_KQUEUE_LOG_ADD(handle)                                           \
    BSLS_LOG_TRACE("Descriptor %d added", handle)

#define NTSO_KQUEUE_LOG_UPDATE(handle, interestSet)                           \
    BSLS_LOG_TRACE("Descriptor %d updated [%s%s ]",                           \
                   (handle),                                                  \
                   (((interestSet).wantReadable((handle))) ? " READ" : ""),   \
                   (((interestSet).wantWritable((handle))) ? " WRITE" : ""))

#define NTSO_KQUEUE_LOG_REMOVE(handle)                                        \
    BSLS_LOG_TRACE("Descriptor %d removed", handle)

namespace BloombergLP {
namespace ntso {

/// @brief @internal
/// Provide an implementation of the 'ntsi::Reactor' interface to poll for
/// socket events using the 'kqueue' API on Darwin and FreeBSD.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntso
class Kqueue : public ntsi::Reactor
{
    typedef bsl::vector<struct ::kevent> EventList;

    int               d_kqueue;
    ntsa::InterestSet d_interestSet;
    EventList         d_outputList;
    EventList         d_changeList;
    bsl::size_t       d_generation;
    bslma::Allocator *d_allocator_p;

  private:
    Kqueue(const Kqueue&) BSLS_KEYWORD_DELETED;
    Kqueue& operator=(const Kqueue&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Kqueue(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Kqueue() BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'socket' to the reactor. Return the error.
    ntsa::Error add(ntsa::Handle socket) BSLS_KEYWORD_OVERRIDE;

    /// Remove the specified 'socket' from the reactor. Return the error.
    ntsa::Error remove(ntsa::Handle socket) BSLS_KEYWORD_OVERRIDE;

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

Kqueue::Kqueue(bslma::Allocator* basicAllocator)
: d_kqueue(ntsa::k_INVALID_HANDLE)
, d_interestSet(basicAllocator)
, d_outputList(basicAllocator)
, d_changeList(basicAllocator)
, d_generation(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_kqueue = ::kqueue();
    if (d_kqueue < 0) {
        NTSO_KQUEUE_LOG_DEVICE_CREATE_FAILURE(ntsa::Error(errno));
        NTSCFG_ABORT();
    }

    NTSO_KQUEUE_LOG_DEVICE_CREATE(d_kqueue);
}

Kqueue::~Kqueue()
{
    int rc;

    if (d_kqueue >= 0) {
        rc = ::close(d_kqueue);
        if (rc != 0) {
            ntsa::Error error(errno);
            NTSO_KQUEUE_LOG_DEVICE_CLOSE_FAILURE(d_kqueue, error);
            NTSCFG_ABORT();
        }

        NTSO_KQUEUE_LOG_DEVICE_CLOSE(d_kqueue);
        d_kqueue = ntsa::k_INVALID_HANDLE;
    }
}

ntsa::Error Kqueue::add(ntsa::Handle socket)
{
    ntsa::Error error;

    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_interestSet.attach(socket);
    if (error) {
        return error;
    }

    {
        struct ::kevent e;
        e.ident  = socket;
        e.filter = EVFILT_READ;
        e.flags  = EV_ADD | EV_DISABLE;
        e.fflags = 0;
        e.data   = 0;
        e.udata  = (void*)(++d_generation);

        d_changeList.push_back(e);
    }

    {
        struct ::kevent e;
        e.ident  = socket;
        e.filter = EVFILT_WRITE;
        e.flags  = EV_ADD | EV_DISABLE;
        e.fflags = 0;
        e.data   = 0;
        e.udata  = (void*)(++d_generation);

        d_changeList.push_back(e);
    }

    NTSO_KQUEUE_LOG_ADD(socket);

    return ntsa::Error();
}

ntsa::Error Kqueue::remove(ntsa::Handle socket)
{
    ntsa::Error error;

    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_interestSet.detach(socket);
    if (error) {
        return error;
    }

    {
        struct ::kevent e;

        e.ident  = socket;
        e.filter = EVFILT_READ;
        e.flags  = EV_DELETE;
        e.fflags = 0;
        e.data   = 0;
        e.udata  = (void*)(++d_generation);

        d_changeList.push_back(e);
    }

    {
        struct ::kevent e;
        e.ident  = socket;
        e.filter = EVFILT_WRITE;
        e.flags  = EV_DELETE;
        e.fflags = 0;
        e.data   = 0;
        e.udata  = (void*)(++d_generation);

        d_changeList.push_back(e);
    }

    NTSO_KQUEUE_LOG_REMOVE(socket);

    return ntsa::Error();
}

ntsa::Error Kqueue::showReadable(ntsa::Handle socket)
{
    ntsa::Error error;

    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_interestSet.showReadable(socket);
    if (error) {
        return error;
    }

    {
        struct ::kevent e;
        e.ident  = socket;
        e.filter = EVFILT_READ;
        e.flags  = EV_ADD | EV_ENABLE;
        e.fflags = 0;
        e.data   = 0;
        e.udata  = (void*)(++d_generation);

        d_changeList.push_back(e);
    }

    NTSO_KQUEUE_LOG_UPDATE(socket, d_interestSet);

    return ntsa::Error();
}

ntsa::Error Kqueue::showWritable(ntsa::Handle socket)
{
    ntsa::Error error;
    
    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_interestSet.showWritable(socket);
    if (error) {
        return error;
    }

    {
        struct ::kevent e;
        e.ident  = socket;
        e.filter = EVFILT_WRITE;
        e.flags  = EV_ADD | EV_ENABLE;
        e.fflags = 0;
        e.data   = 0;
        e.udata  = (void*)(++d_generation);

        d_changeList.push_back(e);
    }

    NTSO_KQUEUE_LOG_UPDATE(socket, d_interestSet);

    return ntsa::Error();
}

ntsa::Error Kqueue::hideReadable(ntsa::Handle socket)
{
    ntsa::Error error;
    
    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_interestSet.hideReadable(socket);
    if (error) {
        return error;
    }

    {
        struct ::kevent e;
        e.ident  = socket;
        e.filter = EVFILT_READ;
        e.flags  = EV_ADD | EV_DISABLE;
        e.fflags = 0;
        e.data   = 0;
        e.udata  = (void*)(++d_generation);

        d_changeList.push_back(e);
    }

    NTSO_KQUEUE_LOG_UPDATE(socket, d_interestSet);

    return ntsa::Error();
}

ntsa::Error Kqueue::hideWritable(ntsa::Handle socket)
{
    ntsa::Error error;
    
    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_interestSet.hideWritable(socket);
    if (error) {
        return error;
    }

    {
        struct ::kevent e;
        e.ident  = socket;
        e.filter = EVFILT_WRITE;
        e.flags  = EV_ADD | EV_DISABLE;
        e.fflags = 0;
        e.data   = 0;
        e.udata  = (void*)(++d_generation);

        d_changeList.push_back(e);
    }

    NTSO_KQUEUE_LOG_UPDATE(socket, d_interestSet);

    return ntsa::Error();
}

ntsa::Error Kqueue::wait(
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

            NTSO_KQUEUE_LOG_WAIT_TIMED(delta.totalMilliseconds());
        }
        else {
            ts.tv_sec = 0;
            ts.tv_nsec = 0;

            NTSO_KQUEUE_LOG_WAIT_TIMED(0);
        }

        tsPtr = &ts;
    }
    else {
        NTSO_KQUEUE_LOG_WAIT_INDEFINITE();
    }

    const bsl::size_t outputListSizeRequired = 
        (2 * d_interestSet.numSockets()) + d_changeList.size();

    if (d_outputList.size() < outputListSizeRequired) {
        d_outputList.resize(outputListSizeRequired);
    }

    if (NTSCFG_UNLIKELY(!d_changeList.empty())) {
        BSLS_LOG_TRACE("Applying change list size = %zu", d_changeList.size());
        for (bsl::size_t i = 0; i < d_changeList.size(); ++i) {
            NTSO_KQUEUE_LOG_EVENT_APPLY(d_changeList[i]);
        }
    }

    rc = ::kevent(d_kqueue, 
                  &d_changeList[0], 
                  static_cast<int>(d_changeList.size()), 
                  &d_outputList[0], 
                  static_cast<int>(d_outputList.size()), 
                  tsPtr);

    if (NTSCFG_LIKELY(rc > 0)) {
        NTSO_KQUEUE_LOG_WAIT_RESULT(rc);

        int numResults = rc;

        result->reserve(result->size() + static_cast<bsl::size_t>(numResults));

        d_changeList.clear();

        for (int eventIndex = 0; eventIndex < numResults; ++eventIndex) {
            struct ::kevent e = d_outputList[eventIndex];

            NTSO_KQUEUE_LOG_EVENT_POLL(e);

            ntsa::Handle handle = static_cast<ntsa::Handle>(e.ident);

            ntsa::Event event;
            event.setHandle(handle);

            if (NTSCFG_UNLIKELY((e.flags & EV_ERROR) != 0)) {
                if (e.data != 0) {
                    event.setError(ntsa::Error(static_cast<int>(e.data)));
                }
                else {
                    ntsa::Error lastError;
                    ntsa::Error error =
                        ntsu::SocketOptionUtil::getLastError(&lastError, 
                                                             handle);
                    if (error) {
                        event.setError(error);
                    }
                    else if (lastError) {
                        event.setError(lastError);
                    }
                    else {
                        event.setExceptional();
                    }
                }
            }
            else if (e.filter == EVFILT_WRITE) {
                event.setWritable();
                event.setBytesWritable(e.data);
            }
            else if (e.filter == EVFILT_READ) {
                event.setReadable();
                event.setBytesReadable(e.data);
            }
            else {
                continue;
            }
            
            result->merge(event);
        }
    }
    else if (rc == 0) {
        NTSO_KQUEUE_LOG_WAIT_TIMEOUT();
        d_changeList.clear();
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }
    else {
        error = ntsa::Error::last();
        NTSO_KQUEUE_LOG_WAIT_FAILURE(error);
        d_changeList.clear();
        return error;
    }

    return ntsa::Error();
}

bsl::shared_ptr<ntsi::Reactor> KqueueUtil::createReactor(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntso::Kqueue> reactor;
    reactor.createInplace(allocator, allocator);

    return reactor;
}

}  // close package namespace
}  // close enterprise namespace
#endif
