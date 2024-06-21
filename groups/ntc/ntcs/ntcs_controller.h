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

#ifndef INCLUDED_NTCS_CONTROLLER
#define INCLUDED_NTCS_CONTROLLER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_reactor.h>
#include <ntci_reactorsocket.h>
#include <ntcscm_version.h>
#include <ntsi_descriptor.h>
#include <ntsu_socketutil.h>
#include <bslmt_mutex.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide a mechanism to force a thread waiting on a reactor to wake up.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class Controller : public ntci::ReactorSocket
{
    /// Enumerates the types of implementation.
    enum Type {
        /// The controller is closed.
        e_NONE,

        /// The controller is implemented by a TCP socket pair.
        e_TCP,

        /// The controller is implemented by a Unix domain socket pair.
        e_UDS,

        /// The controller is implemented by an anonymous pipe.
        e_PIPE,

        /// The controller is implemented by an eventfd operating in semaphore
        /// mode.
        e_EVENT
    };

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    Mutex                         d_mutex;
    ntsa::Handle                  d_clientHandle;
    ntsa::Handle                  d_serverHandle;
    bsl::size_t                   d_pending;
    Type                          d_type;
    bsl::shared_ptr<ntci::Strand> d_strand_sp;

  private:
    Controller(const Controller&) BSLS_KEYWORD_DELETED;
    Controller& operator=(const Controller&) BSLS_KEYWORD_DELETED;

  private:
    /// Process the readability of the descriptor.
    void processSocketReadable(const ntca::ReactorEvent& event)
        BSLS_KEYWORD_OVERRIDE;

    /// Process the writability of the descriptor.
    void processSocketWritable(const ntca::ReactorEvent& event)
        BSLS_KEYWORD_OVERRIDE;

    /// Process an descriptor that has occurred on the socket.
    void processSocketError(const ntca::ReactorEvent& event)
        BSLS_KEYWORD_OVERRIDE;

    /// Process the specified 'notifications' of the socket.
    void processNotifications(const ntsa::NotificationQueue& notifications)
        BSLS_KEYWORD_OVERRIDE;

    /// Return the strand on which this object's functions should be called.
    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;

  public:
    /// Create a new controller and automatically open it.
    Controller();

    /// Destroy this object.
    ~Controller();

    // Open the controller. Return the error.
    ntsa::Error open();

    /// Close the controller.
    void close() BSLS_KEYWORD_OVERRIDE;

    /// Ensure the specified 'numWakeups' number of signals are
    /// acknowledgable. Return the error. Note that the controller's handle
    /// will be polled as readable as long as at least one signal is
    /// unacknowledged.
    ntsa::Error interrupt(unsigned int numWakeups);

    /// Read one signal.  Return the error. Note that the controller's
    /// handle will be polled as readable as long as at least one signal is
    /// unacknowledged.
    ntsa::Error acknowledge();

    /// Return the handle to the descriptor.
    ntsa::Handle handle() const BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace
#endif
