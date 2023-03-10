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

#ifndef INCLUDED_NTCI_DRIVER
#define INCLUDED_NTCI_DRIVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_waiteroptions.h>
#include <ntccfg_platform.h>
#include <ntci_datagramsocketfactory.h>
#include <ntci_datapool.h>
#include <ntci_executor.h>
#include <ntci_listenersocketfactory.h>
#include <ntci_strand.h>
#include <ntci_strandfactory.h>
#include <ntci_streamsocketfactory.h>
#include <ntci_timerfactory.h>
#include <ntci_waiter.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to drive the I/O and events for sockets and timers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Driver : public ntci::Executor,
               public ntci::TimerFactory,
               public ntci::StrandFactory,
               public ntci::DatagramSocketFactory,
               public ntci::ListenerSocketFactory,
               public ntci::StreamSocketFactory,
               public ntci::DataPool
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

    /// Block the calling thread until stopped. As each socket enters the
    /// state in which interest has been registered, or each timer fires,
    /// invoke the corresponding processing function on the associated
    /// descriptor or timer. The behavior is undefined unless the calling
    /// thread has previously registered the 'waiter'. Note that after this
    /// function returns, the 'restart' function must be called before this
    /// or the 'run' function can be called again.
    virtual void run(ntci::Waiter waiter) = 0;

    /// Block the calling thread identified by the specified 'waiter', until
    /// at least one socket enters the state in which interest has been
    /// registered, or timer fires. For each socket that has entered the
    /// state in which interest has been registered, or each timer that has
    /// fired, invoke the corresponding processing function on the
    /// associated descriptor or timer. The behavior is undefined unless the
    /// calling thread has previously registered the 'waiter'. Note that
    /// if this function returns because 'stop' was called, the 'restart'
    /// function must be called before this or the 'run' function can be
    /// called again.
    virtual void poll(ntci::Waiter waiter) = 0;

    /// Unblock and return one caller blocked on either 'poll' or 'run'.
    virtual void interruptOne() = 0;

    /// Unblock and return any caller blocked on either 'poll' or 'run'.
    virtual void interruptAll() = 0;

    /// Unblock and return any caller blocked on either 'poll' or 'run',
    /// and stop running, if necessary.
    virtual void stop() = 0;

    /// Prepare the reactor for 'run' to be called again after previously
    /// being stopped.
    virtual void restart() = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
