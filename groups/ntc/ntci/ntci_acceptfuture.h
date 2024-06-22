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

#ifndef INCLUDED_NTCI_ACCEPTFUTURE
#define INCLUDED_NTCI_ACCEPTFUTURE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_acceptcallback.h>
#include <ntci_acceptresult.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bsls_timeinterval.h>
#include <bsl_list.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {
class Acceptor;
}
namespace ntci {

/// Provide a future asynchronous result of a accept operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_operation_accept
class AcceptFuture : public ntci::AcceptCallback
{
    /// Define a type alias for a queue of results.
    typedef bsl::list<ntci::AcceptResult> ResultQueue;

    ntccfg::ConditionMutex d_mutex;
    ntccfg::Condition      d_condition;
    ResultQueue            d_resultQueue;

  private:
    AcceptFuture(const AcceptFuture&) BSLS_KEYWORD_DELETED;
    AcceptFuture& operator=(const AcceptFuture&) BSLS_KEYWORD_DELETED;

  private:
    /// Arrive at a accept result of the specified 'streamSocket' from the
    /// specified 'acceptor' according to the specified 'event'.
    void arrive(const bsl::shared_ptr<ntci::Acceptor>&     acceptor,
                const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                const ntca::AcceptEvent&                   event);

  public:
    /// Create a new accept result. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default allocator is used.
    explicit AcceptFuture(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~AcceptFuture();

    /// Wait for the accept operation to complete and load the result into
    /// the specified 'result'. Return the error.
    ntsa::Error wait(ntci::AcceptResult* result);

    /// Wait for the accept operation to complete or until the specified
    /// 'timeout', in absolute time since the Unix epoch, elapses. Return
    /// the error.
    ntsa::Error wait(ntci::AcceptResult*       result,
                     const bsls::TimeInterval& timeout);
};

}  // close package namespace
}  // close enterprise namespace
#endif
