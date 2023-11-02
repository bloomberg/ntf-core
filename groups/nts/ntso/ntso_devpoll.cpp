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

#if defined(NTSO_DEVPOLL_ENABLED)

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

// MRM: System headers

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
    bslma::Allocator *d_allocator_p;

  private:
    Devpoll(const Devpoll&) BSLS_KEYWORD_DELETED;
    Devpoll& operator=(const Devpoll&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Devpoll(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Devpoll() BSLS_KEYWORD_OVERRIDE;

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

Devpoll::Devpoll(bslma::Allocator* basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Devpoll::~Devpoll()
{
}

ntsa::Error Devpoll::add(ntsa::Handle socket)
{
    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Devpoll::add(const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->add(socket->handle());
}

ntsa::Error Devpoll::add(const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->add(socket->handle());
}

ntsa::Error Devpoll::add(const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->add(socket->handle());
}

ntsa::Error Devpoll::remove(ntsa::Handle socket)
{
    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Devpoll::remove(const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->remove(socket->handle());
}

ntsa::Error Devpoll::remove(const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->remove(socket->handle());
}

ntsa::Error Devpoll::remove(const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->remove(socket->handle());
}

ntsa::Error Devpoll::showReadable(ntsa::Handle socket)
{
    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Devpoll::showReadable(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->showReadable(socket->handle());
}

ntsa::Error Devpoll::showReadable(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->showReadable(socket->handle());
}

ntsa::Error Devpoll::showReadable(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->showReadable(socket->handle());
}

ntsa::Error Devpoll::showWritable(ntsa::Handle socket)
{
    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Devpoll::showWritable(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->showWritable(socket->handle());
}

ntsa::Error Devpoll::showWritable(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->showWritable(socket->handle());
}

ntsa::Error Devpoll::showWritable(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->showWritable(socket->handle());
}

ntsa::Error Devpoll::hideReadable(ntsa::Handle socket)
{
    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Devpoll::hideReadable(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->hideReadable(socket->handle());
}

ntsa::Error Devpoll::hideReadable(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->hideReadable(socket->handle());
}

ntsa::Error Devpoll::hideReadable(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->hideReadable(socket->handle());
}

ntsa::Error Devpoll::hideWritable(ntsa::Handle socket)
{
    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Devpoll::hideWritable(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->hideReadable(socket->handle());
}

ntsa::Error Devpoll::hideWritable(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->hideWritable(socket->handle());
}

ntsa::Error Devpoll::hideWritable(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->hideWritable(socket->handle());
}

ntsa::Error Devpoll::wait(
    bsl::vector<ntsa::Event>*                      result,
    const bdlb::NullableValue<bsls::TimeInterval>& timeout)
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_WARNING_UNUSED(timeout);


    return ntsa::Error();
}

// MRM
#if 0
ntsa::Error Devpoll::wait(
        bsl::vector<ntsa::Event>                       *result, 
        const bsl::vector<ntsa::Handle>&                readable, 
        const bsl::vector<ntsa::Handle>&                writable,
        const bsl::vector<ntsa::Handle>&                exceptional,
        const bdlb::NullableValue<bsls::TimeInterval>&  timeout)
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_WARNING_UNUSED(readable);
    NTSCFG_WARNING_UNUSED(writable);
    NTSCFG_WARNING_UNUSED(exceptional);
    NTSCFG_WARNING_UNUSED(timeout);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}
#endif

bsl::shared_ptr<ntsi::Reactor> DevpollUtil::createReactor(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntso::Devpoll> reactor;
    reactor.createInplace(allocator, allocator);

    return reactor;
}

}  // close package namespace
}  // close enterprise namespace
#endif
