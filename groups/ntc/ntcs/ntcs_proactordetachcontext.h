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

#ifndef INCLUDED_NTCS_PROACTORDETACHCONTEXT
#define INCLUDED_NTCS_PROACTORDETACHCONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <bsls_atomic.h>

namespace BloombergLP {
namespace ntcs {

class ProactorDetachContext
{
  public:
    enum DetachState {
        e_DETACH_NOT_REQUIRED = 0,
        e_DETACH_REQUIRED     = 1,
        e_DETACH_SCHEDULED    = 2
    };

    ProactorDetachContext();

    /// Return current number of threads actively working with the socket
    unsigned int processCounter() const;

    /// Return true if detachment is not required for the socket.
    bool noDetach() const;

    /// Atomically increment number of threads actively working with the socket
    /// and return resulting value
    unsigned int incrementProcessCounter();

    /// Atomically increment number of threads actively working with the socket
    /// and return resulting value
    unsigned int decrementProcessCounter();

    /// Atomically try to transit detachment state to e_DETACH_SCHEDULED.
    /// Return true in case of success and false otherwise.
    bool trySetDetachScheduled();

    /// Atomically try to transit detachment state to e_DETACH_REQUIRED.
    /// Return true in case of success and false otherwise.
    bool trySetDetachRequired();

  private:
    bsls::AtomicUint d_processCounter;
    bsls::AtomicUint d_detachState;
};


}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
