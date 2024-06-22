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

#ifndef INCLUDED_NTSB_CONTROLLER
#define INCLUDED_NTSB_CONTROLLER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsi_descriptor.h>
#include <ntsscm_version.h>
#include <ntsu_socketutil.h>
#include <bslmt_mutex.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntsb {

/// @internal @brief
/// Provide a mechanism to force a thread waiting on a reactor to wake up.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsb
class Controller
{
    /// Define a type alias for a mutex.
    typedef ntscfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntscfg::LockGuard LockGuard;

    Mutex        d_mutex;
    ntsa::Handle d_clientHandle;
    ntsa::Handle d_serverHandle;
    bsl::size_t  d_pending;

  private:
    Controller(const Controller&) BSLS_KEYWORD_DELETED;
    Controller& operator=(const Controller&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new controller.
    Controller();

    /// Destroy this object.
    ~Controller();

    /// Ensure the specified 'numWakeups' number of signals are
    /// acknowledgable. Return the error. Note that the controller's handle
    /// will be polled as readable as long as at least one signal is
    /// unacknowledged.
    ntsa::Error interrupt(unsigned int numWakeups);

    /// Read one signal.  Return the error. Note that the controller's
    /// handle will be polled as readable as long as at least one signal is
    /// unacknowledged.
    ntsa::Error acknowledge();

    /// Close the socket.
    void close();

    /// Return the handle to the descriptor.
    ntsa::Handle handle() const;
};

}  // close package namespace
}  // close enterprise namespace
#endif
