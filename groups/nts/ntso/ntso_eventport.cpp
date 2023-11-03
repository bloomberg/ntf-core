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
    bslma::Allocator *d_allocator_p;

  private:
    EventPort(const EventPort&) BSLS_KEYWORD_DELETED;
    EventPort& operator=(const EventPort&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit EventPort(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EventPort() BSLS_KEYWORD_OVERRIDE;

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

EventPort::EventPort(bslma::Allocator* basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EventPort::~EventPort()
{
}

ntsa::Error EventPort::add(ntsa::Handle socket)
{
    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EventPort::remove(ntsa::Handle socket)
{
    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EventPort::showReadable(ntsa::Handle socket)
{
    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EventPort::showWritable(ntsa::Handle socket)
{
    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EventPort::hideReadable(ntsa::Handle socket)
{
    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EventPort::hideWritable(ntsa::Handle socket)
{
    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EventPort::wait(
    ntsa::EventSet*                                result,
    const bdlb::NullableValue<bsls::TimeInterval>& timeout)
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_WARNING_UNUSED(timeout);


    return ntsa::Error();
}

bsl::shared_ptr<ntsi::Reactor> EventPortUtil::createReactor(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntso::EventPort> reactor;
    reactor.createInplace(allocator, allocator);

    return reactor;
}

}  // close package namespace
}  // close enterprise namespace
#endif
