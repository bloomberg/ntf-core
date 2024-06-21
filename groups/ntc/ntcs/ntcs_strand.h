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

#ifndef INCLUDED_NTCS_STRAND
#define INCLUDED_NTCS_STRAND

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_executor.h>
#include <ntci_strand.h>
#include <ntcs_observer.h>
#include <ntcscm_version.h>
#include <bsls_spinlock.h>
#include <bsl_functional.h>
#include <bsl_list.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide a mechanism to execute functions asynchronously but sequentially
/// and not concurrent with one another.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class Strand : public ntci::Strand, public ntccfg::Shared<Strand>
{
    /// Define a type alias for a queue of callbacks to
    /// execute on this thread.
    typedef ntci::Executor::FunctorSequence FunctorQueue;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    ntccfg::Object                 d_object;
    mutable Mutex                  d_functorQueueMutex;
    FunctorQueue                   d_functorQueue;
    ntcs::Observer<ntci::Executor> d_executor;
    bool                           d_pending;
    bslma::Allocator*              d_allocator_p;

  private:
    Strand(const Strand&) BSLS_KEYWORD_DELETED;
    Strand& operator=(const Strand&) BSLS_KEYWORD_DELETED;

  private:
    /// Invoke the next functor in the queue.
    void invoke();

  public:
    /// Create a new strand on the specified 'executor'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Strand(const bsl::shared_ptr<ntci::Executor>& executor,
                    bslma::Allocator*                      basicAllocator = 0);

    /// Destroy this object.
    ~Strand() BSLS_KEYWORD_OVERRIDE;

    /// Defer the specified 'function' to execute sequentially, and
    /// non-concurrently, after all previously deferred functions. Note that
    /// the 'function' is not necessarily guaranteed to execute on the same
    /// thread as previously deferred functions were executed, or is it
    /// necessarily guaranteed to execute on a different thread than
    /// previously deferred functions were executed.
    void execute(const Functor& function) BSLS_KEYWORD_OVERRIDE;

    /// Atomically defer the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE;

    /// Execute all pending operations on the calling thread. The behavior
    /// is undefined unless no other thread is processing pending
    /// operations.
    void drain() BSLS_KEYWORD_OVERRIDE;

    /// Clear all pending operations.
    void clear() BSLS_KEYWORD_OVERRIDE;

    /// Return true if operations in this strand are currently being invoked
    /// by the current thread, otherwise return false.
    bool isRunningInCurrentThread() const BSLS_KEYWORD_OVERRIDE;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
