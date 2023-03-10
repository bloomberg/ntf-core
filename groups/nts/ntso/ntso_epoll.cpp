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

#if defined(BSLS_PLATFORM_OS_LINUX)

#include <ntsb_controller.h>
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

#define NTSO_EPOLL_LOG 0

#if NTSO_EPOLL_LOG

#define NTSO_EPOLL_LOG_EPOLLFD_CREATE(fd)                                     \
    BSLS_LOG_TRACE("Event poll descriptor %d created", fd)

#define NTSO_EPOLL_LOG_EPOLLFD_CREATE_FAILURE(error)                          \
    BSLS_LOG_ERROR("Failed to create event poll descriptor: %s",              \
                   (error).text().c_str())

#define NTSO_EPOLL_LOG_EPOLLFD_CLOSE(fd)                                      \
    BSLS_LOG_TRACE("Event poll descriptor %d closed", fd)

#define NTSO_EPOLL_LOG_EPOLLFD_CLOSE_FAILURE(error)                           \
    BSLS_LOG_ERROR("Failed to close event poll descriptor: %s",               \
                   (error).text().c_str())

#define NTSO_EPOLL_LOG_EVENTS(handle, event)                                  \
    BSLS_LOG_TRACE(                                                           \
        "Descriptor %d polled%s%s%s%s%s%s%s%s",                               \
        event.data.fd,                                                        \
        (((event.events & EPOLLIN) != 0) ? " EPOLLIN" : ""),                  \
        (((event.events & EPOLLOUT) != 0) ? " EPOLLOUT" : ""),                \
        (((event.events & EPOLLERR) != 0) ? " EPOLLERR" : ""),                \
        (((event.events & EPOLLHUP) != 0) ? " EPOLLHUP" : ""),                \
        (((event.events & EPOLLRDHUP) != 0) ? " EPOLLRDHUP" : ""),            \
        (((event.events & EPOLLPRI) != 0) ? " EPOLLPRI" : ""),                \
        (((event.events & EPOLLET) != 0) ? " EPOLLET" : ""),                  \
        (((event.events & EPOLLONESHOT) != 0) ? " EPOLLONESHOT" : ""))

#define NTSO_EPOLL_LOG_ADD(handle, event)                                     \
    BSLS_LOG_TRACE(                                                           \
        "Descriptor %d added%s%s%s%s%s%s%s%s",                                \
        event.data.fd,                                                        \
        (((event.events & EPOLLIN) != 0) ? " EPOLLIN" : ""),                  \
        (((event.events & EPOLLOUT) != 0) ? " EPOLLOUT" : ""),                \
        (((event.events & EPOLLERR) != 0) ? " EPOLLERR" : ""),                \
        (((event.events & EPOLLHUP) != 0) ? " EPOLLHUP" : ""),                \
        (((event.events & EPOLLRDHUP) != 0) ? " EPOLLRDHUP" : ""),            \
        (((event.events & EPOLLPRI) != 0) ? " EPOLLPRI" : ""),                \
        (((event.events & EPOLLET) != 0) ? " EPOLLET" : ""),                  \
        (((event.events & EPOLLONESHOT) != 0) ? " EPOLLONESHOT" : ""))

#define NTSO_EPOLL_LOG_ADD_FAILURE(handle, error)                             \
    BSLS_LOG_ERROR("Failed to add descriptor %d: %s",                         \
                   handle,                                                    \
                   error.text().c_str())

#define NTSO_EPOLL_LOG_UPDATE(handle, event)                                  \
    BSLS_LOG_TRACE(                                                           \
        "Descriptor %d updated%s%s%s%s%s%s%s%s",                              \
        event.data.fd,                                                        \
        (((event.events & EPOLLIN) != 0) ? " EPOLLIN" : ""),                  \
        (((event.events & EPOLLOUT) != 0) ? " EPOLLOUT" : ""),                \
        (((event.events & EPOLLERR) != 0) ? " EPOLLERR" : ""),                \
        (((event.events & EPOLLHUP) != 0) ? " EPOLLHUP" : ""),                \
        (((event.events & EPOLLRDHUP) != 0) ? " EPOLLRDHUP" : ""),            \
        (((event.events & EPOLLPRI) != 0) ? " EPOLLPRI" : ""),                \
        (((event.events & EPOLLET) != 0) ? " EPOLLET" : ""),                  \
        (((event.events & EPOLLONESHOT) != 0) ? " EPOLLONESHOT" : ""))

#define NTSO_EPOLL_LOG_UPDATE_FAILURE(handle, error)                          \
    BSLS_LOG_ERROR("Failed to update descriptor %d: %s",                      \
                   handle,                                                    \
                   error.text().c_str())

#define NTSO_EPOLL_LOG_REMOVE(handle)                                         \
    BSLS_LOG_TRACE("Descriptor %d removed", handle)

#define NTSO_EPOLL_LOG_REMOVE_FAILURE(handle, error)                          \
    BSLS_LOG_ERROR("Failed to remove descriptor %d: %s",                      \
                   handle,                                                    \
                   error.text().c_str())

#else

#define NTSO_EPOLL_LOG_EPOLLFD_CREATE(fd)
#define NTSO_EPOLL_LOG_EPOLLFD_CREATE_FAILURE(error)
#define NTSO_EPOLL_LOG_EPOLLFD_CLOSE(fd)
#define NTSO_EPOLL_LOG_EPOLLFD_CLOSE_FAILURE(error)
#define NTSO_EPOLL_LOG_EVENTS(handle, event)
#define NTSO_EPOLL_LOG_ADD(handle, event)
#define NTSO_EPOLL_LOG_ADD_FAILURE(handle, error)
#define NTSO_EPOLL_LOG_UPDATE(handle, event)
#define NTSO_EPOLL_LOG_UPDATE_FAILURE(handle, error)
#define NTSO_EPOLL_LOG_REMOVE(handle)
#define NTSO_EPOLL_LOG_REMOVE_FAILURE(handle, error)

#endif

namespace BloombergLP {
namespace ntso {

/// Provide an implementation of the 'ntsi::Reactor' interface
/// to poll for socket events using the 'epoll' API on Linux. This class is
/// thread safe.
class Epoll : public ntsi::Reactor
{
    /// This class describes an entry in the interest set.
    struct Entry {
        ntsa::Handle d_fd;
        uint32_t     d_events;

        /// Create a new entry describing an invalid file descriptor with
        /// no event intereset.
        Entry()
        : d_fd(ntsa::k_INVALID_HANDLE)
        , d_events(0)
        {
        }

        /// Defines the traits of this type. These traits can be used to
        /// select, at compile-time, the most efficient algorithm to manipulate
        /// objects of this type.
        NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(Entry);
    };

    /// Define a type alias for a vector of entries.
    typedef bsl::vector<Entry> EntryVector;

    bslmt::Mutex      d_mutex;
    int               d_epoll;
    bsls::AtomicUint  d_count;
    EntryVector       d_entryVector;
    ntsb::Controller  d_controller;
    ntsa::Handle      d_controllerHandle;
    bslma::Allocator* d_allocator_p;

  private:
    Epoll(const Epoll&) BSLS_KEYWORD_DELETED;
    Epoll& operator=(const Epoll&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Epoll(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Epoll() BSLS_KEYWORD_OVERRIDE;

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

    /// Block until at least one socket has a condition of interest. Load
    /// into the specified 'result' the events that describe the sockets
    /// and their conditions. Return the error.
    ntsa::Error wait(bsl::vector<ntsa::Event>* result) BSLS_KEYWORD_OVERRIDE;

    /// Block until at least one socket has a condition of interest or the
    /// specified absolute 'timeout' elapses. Load into the specified
    /// 'result' the events that describe the sockets and their conditions.
    /// Return the error.
    ntsa::Error wait(bsl::vector<ntsa::Event>* result,
                     const bsls::TimeInterval& timeout) BSLS_KEYWORD_OVERRIDE;

    /// Unblock a thread waiting on the reactor. Return the error.
    void interrupt() BSLS_KEYWORD_OVERRIDE;
};

Epoll::Epoll(bslma::Allocator* basicAllocator)
: d_mutex()
, d_epoll(ntsa::k_INVALID_HANDLE)
, d_count(1)
, d_entryVector(basicAllocator)
, d_controller()
, d_controllerHandle(ntsa::k_INVALID_HANDLE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    int rc;

    d_epoll = ::epoll_create1(EPOLL_CLOEXEC);
    if (d_epoll < 0) {
        ntsa::Error error(errno);
        NTSO_EPOLL_LOG_EPOLLFD_CREATE_FAILURE(error);
        NTSCFG_ABORT();
    }

    NTSO_EPOLL_LOG_EPOLLFD_CREATE(d_epoll);

    d_controllerHandle = d_controller.handle();
    BSLS_ASSERT(d_controllerHandle != ntsa::k_INVALID_HANDLE);

    {
        ::epoll_event e;

        e.data.fd = d_controllerHandle;
        e.events  = EPOLLIN;

        rc = ::epoll_ctl(d_epoll, EPOLL_CTL_ADD, d_controller.handle(), &e);
        if (rc != 0) {
            ntsa::Error error(errno);
            NTSO_EPOLL_LOG_ADD_FAILURE(e.data.fd, error);
            NTSCFG_ABORT();
        }

        NTSO_EPOLL_LOG_ADD(e.data.fd, e);
    }
}

Epoll::~Epoll()
{
    int rc;

    {
        ::epoll_event e;

        e.data.fd = d_controllerHandle;
        e.events  = 0;

        rc = ::epoll_ctl(d_epoll, EPOLL_CTL_DEL, d_controllerHandle, &e);
        if (rc != 0) {
            ntsa::Error error(errno);
            NTSO_EPOLL_LOG_REMOVE_FAILURE(e.data.fd, error);
            NTSCFG_ABORT();
        }

        NTSO_EPOLL_LOG_REMOVE(d_controllerHandle);
    }

    if (d_epoll >= 0) {
        rc = ::close(d_epoll);
        if (rc != 0) {
            ntsa::Error error(errno);
            NTSO_EPOLL_LOG_EPOLLFD_CLOSE_FAILURE(error);
            NTSCFG_ABORT();
        }

        NTSO_EPOLL_LOG_EPOLLFD_CLOSE(d_epoll);
        d_epoll = -1;
    }
}

ntsa::Error Epoll::add(ntsa::Handle socket)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    int rc;

    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t index = static_cast<bsl::size_t>(socket);

    if (index >= d_entryVector.size()) {
        d_entryVector.resize(index + 1);
    }

    Entry& entry = d_entryVector[index];

    entry.d_fd     = socket;
    entry.d_events = 0;

    {
        ::epoll_event e;

        e.data.fd = entry.d_fd;
        e.events  = entry.d_events;

        rc = ::epoll_ctl(d_epoll, EPOLL_CTL_ADD, entry.d_fd, &e);
        if (rc != 0) {
            ntsa::Error error(errno);
            NTSO_EPOLL_LOG_ADD_FAILURE(e.data.fd, error);
            return error;
        }

        NTSO_EPOLL_LOG_ADD(e.data.fd, e);
    }

    ++d_count;

    d_controller.interrupt(1);

    return ntsa::Error();
}

ntsa::Error Epoll::add(const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->add(socket->handle());
}

ntsa::Error Epoll::add(const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->add(socket->handle());
}

ntsa::Error Epoll::add(const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->add(socket->handle());
}

ntsa::Error Epoll::remove(ntsa::Handle socket)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    int rc;

    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t index = static_cast<bsl::size_t>(socket);

    if (index >= d_entryVector.size()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    Entry& entry = d_entryVector[index];

    BSLS_ASSERT(entry.d_fd == socket);

    entry.d_fd     = ntsa::k_INVALID_HANDLE;
    entry.d_events = 0;

    {
        ::epoll_event e;

        e.data.fd = socket;
        e.events  = 0;

        rc = ::epoll_ctl(d_epoll, EPOLL_CTL_DEL, socket, &e);
        if (rc != 0) {
            ntsa::Error error(errno);
            NTSO_EPOLL_LOG_REMOVE_FAILURE(e.data.fd, error);
            return error;
        }

        NTSO_EPOLL_LOG_REMOVE(e.data.fd);
    }

    --d_count;

    d_controller.interrupt(1);

    return ntsa::Error();
}

ntsa::Error Epoll::remove(const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->remove(socket->handle());
}

ntsa::Error Epoll::remove(const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->remove(socket->handle());
}

ntsa::Error Epoll::remove(const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->remove(socket->handle());
}

ntsa::Error Epoll::showReadable(ntsa::Handle socket)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    int rc;

    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t index = static_cast<bsl::size_t>(socket);

    if (index >= d_entryVector.size()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    Entry& entry = d_entryVector[index];

    BSLS_ASSERT(entry.d_fd == socket);
    entry.d_events |= EPOLLIN;

    {
        ::epoll_event e;

        e.data.fd = entry.d_fd;
        e.events  = entry.d_events;

        rc = ::epoll_ctl(d_epoll, EPOLL_CTL_MOD, entry.d_fd, &e);
        if (rc != 0) {
            ntsa::Error error(errno);
            NTSO_EPOLL_LOG_UPDATE_FAILURE(e.data.fd, error);
            return error;
        }

        NTSO_EPOLL_LOG_UPDATE(e.data.fd, e);
    }

    d_controller.interrupt(1);

    return ntsa::Error();
}

ntsa::Error Epoll::showReadable(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->showReadable(socket->handle());
}

ntsa::Error Epoll::showReadable(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->showReadable(socket->handle());
}

ntsa::Error Epoll::showReadable(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->showReadable(socket->handle());
}

ntsa::Error Epoll::showWritable(ntsa::Handle socket)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    int rc;

    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t index = static_cast<bsl::size_t>(socket);

    if (index >= d_entryVector.size()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    Entry& entry = d_entryVector[index];

    BSLS_ASSERT(entry.d_fd == socket);
    entry.d_events |= EPOLLOUT;

    {
        ::epoll_event e;

        e.data.fd = entry.d_fd;
        e.events  = entry.d_events;

        rc = ::epoll_ctl(d_epoll, EPOLL_CTL_MOD, entry.d_fd, &e);
        if (rc != 0) {
            ntsa::Error error(errno);
            NTSO_EPOLL_LOG_UPDATE_FAILURE(e.data.fd, error);
            return error;
        }

        NTSO_EPOLL_LOG_UPDATE(e.data.fd, e);
    }

    d_controller.interrupt(1);

    return ntsa::Error();
}

ntsa::Error Epoll::showWritable(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->showWritable(socket->handle());
}

ntsa::Error Epoll::showWritable(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->showWritable(socket->handle());
}

ntsa::Error Epoll::showWritable(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->showWritable(socket->handle());
}

ntsa::Error Epoll::hideReadable(ntsa::Handle socket)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    int rc;

    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t index = static_cast<bsl::size_t>(socket);

    if (index >= d_entryVector.size()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    Entry& entry = d_entryVector[index];

    BSLS_ASSERT(entry.d_fd == socket);
    entry.d_events &= ~EPOLLIN;

    {
        ::epoll_event e;

        e.data.fd = entry.d_fd;
        e.events  = entry.d_events;

        rc = ::epoll_ctl(d_epoll, EPOLL_CTL_MOD, entry.d_fd, &e);
        if (rc != 0) {
            ntsa::Error error(errno);
            NTSO_EPOLL_LOG_UPDATE_FAILURE(e.data.fd, error);
            return error;
        }

        NTSO_EPOLL_LOG_UPDATE(e.data.fd, e);
    }

    d_controller.interrupt(1);

    return ntsa::Error();
}

ntsa::Error Epoll::hideReadable(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->hideReadable(socket->handle());
}

ntsa::Error Epoll::hideReadable(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->hideReadable(socket->handle());
}

ntsa::Error Epoll::hideReadable(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->hideReadable(socket->handle());
}

ntsa::Error Epoll::hideWritable(ntsa::Handle socket)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    int rc;

    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t index = static_cast<bsl::size_t>(socket);

    if (index >= d_entryVector.size()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    Entry& entry = d_entryVector[index];

    BSLS_ASSERT(entry.d_fd == socket);
    entry.d_events &= ~EPOLLOUT;

    {
        ::epoll_event e;

        e.data.fd = entry.d_fd;
        e.events  = entry.d_events;

        rc = ::epoll_ctl(d_epoll, EPOLL_CTL_MOD, entry.d_fd, &e);
        if (rc != 0) {
            ntsa::Error error(errno);
            NTSO_EPOLL_LOG_UPDATE_FAILURE(e.data.fd, error);
            return error;
        }

        NTSO_EPOLL_LOG_UPDATE(e.data.fd, e);
    }

    d_controller.interrupt(1);

    return ntsa::Error();
}

ntsa::Error Epoll::hideWritable(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->hideReadable(socket->handle());
}

ntsa::Error Epoll::hideWritable(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->hideWritable(socket->handle());
}

ntsa::Error Epoll::hideWritable(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->hideWritable(socket->handle());
}

ntsa::Error Epoll::wait(bsl::vector<ntsa::Event>* result)
{
    ntsa::Error error;
    int         rc;

    result->clear();

    while (true) {
        bdlma::LocalSequentialAllocator<1024 * sizeof(::epoll_event)> arena;

        bsl::vector< ::epoll_event> events(&arena);
        events.resize(static_cast<bsl::size_t>(
            static_cast<unsigned int>(d_count.load())));

        rc = ::epoll_wait(d_epoll,
                          &events.front(),
                          static_cast<int>(events.size()),
                          -1);

        if (NTSCFG_LIKELY(rc > 0)) {
            const int numEvents = rc;
            result->reserve(static_cast<bsl::size_t>(numEvents));

            for (int i = 0; i < numEvents; ++i) {
                ::epoll_event e = events[i];

                if (NTSCFG_LIKELY(e.data.fd != d_controllerHandle)) {
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
                        error =
                            ntsu::SocketOptionUtil::getLastError(&lastError,
                                                                 e.data.fd);
                        if (!error && lastError) {
                            event.setError(lastError);
                        }
                        else {
                            event.setError(
                                ntsa::Error(ntsa::Error::e_INVALID));
                        }
                    }

                    if ((e.events & EPOLLRDHUP) != 0) {
                        event.setShutdown();
                    }

                    if ((e.events & EPOLLHUP) != 0) {
                        event.setDisconnected();
                    }

                    result->push_back(event);
                }
                else {
                    error = d_controller.acknowledge();
                    if (error) {
                        NTSCFG_ABORT();
                    }
                }
            }

            if (result->size() > 0) {
                break;
            }
            else {
                continue;
            }
        }
        else if (rc == 0) {
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }
        else {
            int lastError = errno;
            if (lastError == EINTR) {
                continue;
            }
            else {
                return ntsa::Error(lastError);
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error Epoll::wait(bsl::vector<ntsa::Event>* result,
                        const bsls::TimeInterval& timeout)
{
    ntsa::Error error;
    int         rc;

    result->clear();

    while (true) {
        bdlma::LocalSequentialAllocator<1024 * sizeof(::epoll_event)> arena;

        bsl::vector< ::epoll_event> events(&arena);
        events.resize(static_cast<bsl::size_t>(
            static_cast<unsigned int>(d_count.load())));

        int timeoutInMs = -1;
        {
            const bsls::TimeInterval now = bdlt::CurrentTime::now();
            if (NTSCFG_LIKELY(timeout > now)) {
                const bsls::TimeInterval delta = timeout - now;
                const bsl::int64_t       deltaInMs =
                    static_cast<bsl::int64_t>(delta.totalMilliseconds());

                if (NTSCFG_LIKELY(deltaInMs < bsl::numeric_limits<int>::max()))
                {
                    timeoutInMs = static_cast<int>(deltaInMs);
                }
                else {
                    timeoutInMs = bsl::numeric_limits<int>::max();
                }
            }
            else {
                timeoutInMs = 0;
            }
        }

        rc = ::epoll_wait(d_epoll,
                          &events.front(),
                          static_cast<int>(events.size()),
                          timeoutInMs);

        if (NTSCFG_LIKELY(rc > 0)) {
            const int numEvents = rc;
            result->reserve(static_cast<bsl::size_t>(numEvents));

            for (int i = 0; i < numEvents; ++i) {
                ::epoll_event e = events[i];

                if (NTSCFG_LIKELY(e.data.fd != d_controllerHandle)) {
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
                        error =
                            ntsu::SocketOptionUtil::getLastError(&lastError,
                                                                 e.data.fd);
                        if (!error && lastError) {
                            event.setError(lastError);
                        }
                        else {
                            event.setError(
                                ntsa::Error(ntsa::Error::e_INVALID));
                        }
                    }

                    if ((e.events & EPOLLRDHUP) != 0) {
                        event.setShutdown();
                    }

                    if ((e.events & EPOLLHUP) != 0) {
                        event.setDisconnected();
                    }

                    result->push_back(event);
                }
                else {
                    error = d_controller.acknowledge();
                    if (error) {
                        NTSCFG_ABORT();
                    }
                }
            }

            if (result->size() > 0) {
                break;
            }
            else {
                continue;
            }
        }
        else if (rc == 0) {
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }
        else {
            int lastError = errno;
            if (lastError == EINTR) {
                continue;
            }
            else {
                return ntsa::Error(lastError);
            }
        }
    }

    return ntsa::Error();
}

void Epoll::interrupt()
{
    d_controller.interrupt(1);
}

bsl::shared_ptr<ntsi::Reactor> EpollUtil::createReactor(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntso::Epoll> reactor;
    reactor.createInplace(allocator, allocator);

    return reactor;
}

}  // close package namespace
}  // close enterprise namespace
#endif
