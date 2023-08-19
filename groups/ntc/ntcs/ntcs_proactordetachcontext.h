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
        e_DETACH_NOT_REQUIRED = 0 << 30,
        e_DETACH_REQUIRED     = 1 << 30,
        e_DETACH_SCHEDULED    = 2 << 30
    };

    ProactorDetachContext();

    /// Atomically increment number of threads actively working with the socket
    /// and check that detach is neither required nor scheduled. Return true if
    /// so and false otherwise
    bool incrementAndCheckNoDetach();

    /// Atomically increment number of threads actively working with the socket
    /// and try to set detachment state to e_DETACH_SCHEDULED. Return true in
    /// case of success and false otherwise
    bool decrementProcessCounterAndCheckDetachPossible();

    /// Atomically try to transit detachment state to e_DETACH_SCHEDULED.
    /// Return true in case of success and false otherwise.
    bool trySetDetachScheduled();

    /// Atomically try to transit detachment state to e_DETACH_REQUIRED.
    /// Return true in case of success and false otherwise.
    bool trySetDetachRequired();

  private:
    bsls::AtomicUint d_stateAndCounter;

    enum { k_STATE_MASK = 0xC0000000 };
    enum { k_COUNTER_MASK = 0x3FFFFFF };
};

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
