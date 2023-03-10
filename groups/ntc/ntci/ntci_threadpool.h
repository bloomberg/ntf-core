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

#ifndef INCLUDED_NTCI_THREADPOOL
#define INCLUDED_NTCI_THREADPOOL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_executor.h>
#include <ntci_thread.h>
#include <ntcscm_version.h>
#include <bslmt_threadutil.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to a pool of I/O threads automatically driving
/// reactors or proactors.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class ThreadPool
{
  public:
    /// Destroy this object.
    virtual ~ThreadPool();

    /// Close all sockets and timers.
    virtual ntsa::Error closeAll() = 0;

    /// Load into the specified 'result' the executor driven by the thread
    /// identified by the specified 'threadHandle'. Return true if such an
    /// executor exists, and false otherwise.
    virtual bool lookupByThreadHandle(
        bsl::shared_ptr<ntci::Executor>* result,
        bslmt::ThreadUtil::Handle        threadHandle) const = 0;

    /// Load into the specified 'result' the executor driven by the thread
    /// identified by the specified 'threadIndex'. Return true if such a
    /// thread exists, and false otherwise.
    virtual bool lookupByThreadIndex(bsl::shared_ptr<ntci::Executor>* result,
                                     bsl::size_t threadIndex) const = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
