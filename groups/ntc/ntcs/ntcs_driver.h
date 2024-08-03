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

#ifndef INCLUDED_NTCS_DRIVER
#define INCLUDED_NTCS_DRIVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_waiteroptions.h>
#include <ntccfg_platform.h>
#include <ntci_waiter.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide an interface to interrupt a reactor or proactor.
///
/// @details
/// This class provide the common interruption functionality present in reactor
/// and proactor drivers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class Interruptor
{
  public:
    /// Destroy this object.
    virtual ~Interruptor();

    /// Unblock one waiter blocked on 'wait'.
    virtual void interruptOne() = 0;

    /// Unblock all waiters blocked on 'wait'.
    virtual void interruptAll() = 0;

    /// Return the handle of the thread that will be calling 'wait()', or
    /// the default value if no such thread has been set.
    virtual bslmt::ThreadUtil::Handle threadHandle() const = 0;

    /// Return the index of the thread that will be calling 'wait()', or
    /// the default value if no such thread has been set.
    virtual bsl::size_t threadIndex() const = 0;
};

/// @internal @brief
/// Provide an interface to drive a reactor or proactor.
///
/// @details
/// This class provides the common waiter registration, interruption, and
/// socket, timer, and deferred function functionality present in reactor and
/// proactor drivers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class Driver : public ntcs::Interruptor
{
  public:
    /// Destroy this object.
    virtual ~Driver();

    /// Register a thread described by the specified 'waiterOptions' that
    /// will drive this object. Return the handle to the waiter.
    virtual ntci::Waiter registerWaiter(
        const ntca::WaiterOptions& waiterOptions) = 0;

    /// Deregister the specified 'waiter'.
    virtual void deregisterWaiter(ntci::Waiter waiter) = 0;

    /// Clear all resources managed by this object.
    virtual void clear() = 0;

    /// Return the name of the driver.
    virtual const char* name() const = 0;

    /// Return the current number of registered waiters.
    virtual bsl::size_t numWaiters() const = 0;

    /// Return the current number of descriptors being monitored.
    virtual bsl::size_t numSockets() const = 0;

    /// Return the maximum number of descriptors capable of being monitored
    /// at one time.
    virtual bsl::size_t maxSockets() const = 0;
};

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
