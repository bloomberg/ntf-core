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

#ifndef INCLUDED_NTCI_SCHEDULER
#define INCLUDED_NTCI_SCHEDULER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_strand.h>
#include <ntci_timerfactory.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to a scheduler of asynchronous operations.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Scheduler : public ntci::TimerFactory
{
  public:
    /// Destroy this object.
    virtual ~Scheduler();
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
