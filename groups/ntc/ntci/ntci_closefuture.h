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

#ifndef INCLUDED_NTCI_CLOSEFUTURE
#define INCLUDED_NTCI_CLOSEFUTURE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_closecallback.h>
#include <ntci_closeresult.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bsls_timeinterval.h>
#include <bsl_list.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {
class Closable;
}
namespace ntci {

/// Provide a future asynchronous result of a close operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_operation_close
class CloseFuture : public ntci::CloseCallback
{
    /// Define a type alias for a queue of results.
    typedef bsl::list<ntci::CloseResult> ResultQueue;

    ntccfg::ConditionMutex d_mutex;
    ntccfg::Condition      d_condition;
    ResultQueue            d_resultQueue;

  private:
    CloseFuture(const CloseFuture&) BSLS_KEYWORD_DELETED;
    CloseFuture& operator=(const CloseFuture&) BSLS_KEYWORD_DELETED;

  private:
    /// Arrive at a close result.
    void arrive();

  public:
    /// Create a new close result. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default allocator is used.
    explicit CloseFuture(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~CloseFuture();

    /// Wait for the close operation to complete and load the result into
    /// the specified 'result'. Return the error.
    ntsa::Error wait(ntci::CloseResult* result);

    /// Wait for the close operation to complete or until the specified
    /// 'timeout', in absolute time since the Unix epoch, elapses. Return
    /// the error.
    ntsa::Error wait(ntci::CloseResult*        result,
                     const bsls::TimeInterval& timeout);
};

}  // close package namespace
}  // close enterprise namespace
#endif
