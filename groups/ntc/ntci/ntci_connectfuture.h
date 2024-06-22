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

#ifndef INCLUDED_NTCI_CONNECTFUTURE
#define INCLUDED_NTCI_CONNECTFUTURE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_connectcallback.h>
#include <ntci_connectresult.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bsls_timeinterval.h>
#include <bsl_list.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {
class Connector;
}
namespace ntci {

/// Provide a future asynchronous result of a connect operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_operation_connect
class ConnectFuture : public ntci::ConnectCallback
{
    /// Define a type alias for a queue of results.
    typedef bsl::list<ntci::ConnectResult> ResultQueue;

    ntccfg::ConditionMutex d_mutex;
    ntccfg::Condition      d_condition;
    ResultQueue            d_resultQueue;

  private:
    ConnectFuture(const ConnectFuture&) BSLS_KEYWORD_DELETED;
    ConnectFuture& operator=(const ConnectFuture&) BSLS_KEYWORD_DELETED;

  private:
    /// Arrive at a connect result from the specified 'connector' according to
    /// the specified 'event'.
    void arrive(const bsl::shared_ptr<ntci::Connector>& connector,
                const ntca::ConnectEvent&               event);

  public:
    /// Create a new connect result. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default allocator is used.
    explicit ConnectFuture(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ConnectFuture();

    /// Wait for the connect operation to complete and load the result into
    /// the specified 'result'. Return the error.
    ntsa::Error wait(ntci::ConnectResult* result);

    /// Wait for the connect operation to complete or until the specified
    /// 'timeout', in absolute time since the Unix epoch, elapses. Return
    /// the error.
    ntsa::Error wait(ntci::ConnectResult*      result,
                     const bsls::TimeInterval& timeout);
};

}  // close package namespace
}  // close enterprise namespace
#endif
