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

#ifndef INCLUDED_NTCI_TIMERSESSION
#define INCLUDED_NTCI_TIMERSESSION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_timerevent.h>
#include <ntccfg_platform.h>
#include <ntci_strand.h>
#include <ntcscm_version.h>
#include <bsls_timeinterval.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {
class Timer;
}
namespace ntci {

/// Provide an interface to detect events that passively occur during the
/// asynchronous operation of a timer.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_timer
class TimerSession
{
  public:
    /// Destroy this object.
    virtual ~TimerSession();

    /// Process the arrival of the timer's deadline.
    virtual void processTimerDeadline(
        const bsl::shared_ptr<ntci::Timer>& timer,
        const ntca::TimerEvent&             event);

    /// Process the cancellation of the timer.
    virtual void processTimerCancelled(
        const bsl::shared_ptr<ntci::Timer>& timer,
        const ntca::TimerEvent&             event);

    /// Process the removal of the timer.
    virtual void processTimerClosed(const bsl::shared_ptr<ntci::Timer>& timer,
                                    const ntca::TimerEvent&             event);

    /// Return the strand on which this object's functions should be called.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
