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

#ifndef INCLUDED_NTCI_TIMERFUTURE
#define INCLUDED_NTCI_TIMERFUTURE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_timercallback.h>
#include <ntci_timerresult.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bsls_timeinterval.h>
#include <bsl_list.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {
class Timer;
}
namespace ntci {

/// Provide a future asynchronous result of a timer operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_timer
class TimerFuture : public ntci::TimerCallback
{
    /// Define a type alias for a queue of results.
    typedef bsl::list<ntci::TimerResult> ResultQueue;

    ntccfg::ConditionMutex d_mutex;
    ntccfg::Condition      d_condition;
    ResultQueue            d_resultQueue;

  private:
    TimerFuture(const TimerFuture&) BSLS_KEYWORD_DELETED;
    TimerFuture& operator=(const TimerFuture&) BSLS_KEYWORD_DELETED;

  private:
    /// Arrive at a timer result from the specified 'timer' according to
    /// the specified 'event'.
    void arrive(const bsl::shared_ptr<ntci::Timer>& timer,
                const ntca::TimerEvent&             event);

  public:
    /// Create a new timer result. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default allocator is used.
    explicit TimerFuture(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~TimerFuture();

    /// Wait for the timer operation to complete and load the result into
    /// the specified 'result'. Return the error.
    ntsa::Error wait(ntci::TimerResult* result);

    /// Wait for the timer operation to complete or until the specified
    /// 'timeout', in absolute time since the Unix epoch, elapses. Return
    /// the error.
    ntsa::Error wait(ntci::TimerResult*        result,
                     const bsls::TimeInterval& timeout);
};

}  // close package namespace
}  // close enterprise namespace
#endif
