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

#ifndef INCLUDED_NTCI_STRAND
#define INCLUDED_NTCI_STRAND

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_executor.h>
#include <ntcscm_version.h>
#include <bsl_functional.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to asynchronous but sequential, non-concurrent
/// execution.
///
/// @details
/// This class provides a mechanism to guarantee sequential, non-concurrent
/// execution of functions, although not necessarily on the same thread, nor
/// necessarily on different threads.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Strand : public ntci::Executor
{
  public:
    /// Destroy this object.
    virtual ~Strand();

    /// Execute all pending operations on the calling thread. The behavior
    /// is undefined unless no other thread is processing pending
    /// operations.
    virtual void drain() = 0;

    /// Clear all pending operations.
    virtual void clear() = 0;

    /// Return true if operations in this strand are currently being invoked
    /// by the current thread, otherwise return false.
    virtual bool isRunningInCurrentThread() const = 0;

    /// Return true if the specified 'calleeStrand' is 'unspecified()' or
    /// 'unknown()' or the 'calleeStrand' is the same as the 'callerStrand'.
    static bool passthrough(const bsl::shared_ptr<ntci::Strand>& calleeStrand,
                            const bsl::shared_ptr<ntci::Strand>& callerStrand);

    /// Return the strand that represents the strand on which a function
    /// should be executed is unspecified (i.e. unconstrained).
    static const bsl::shared_ptr<ntci::Strand>& unspecified();

    /// Return the strand that represents the current strand is unknown or
    /// uncertain.
    static const bsl::shared_ptr<ntci::Strand>& unknown();

    /// Set the specified 'strand' as the strand currently active in this
    /// thread. Return the previous strand active in this thread, if any.
    static ntci::Strand* setThreadLocal(ntci::Strand* strand);

    /// Return the strand active the current thread, if any.
    static ntci::Strand* getThreadLocal();
};

/// Provide a guard to automatically install a pointer to a
/// strand currently active in a thread into thread local storage when the
/// guard is constructed and un-install it from thread local storage when
/// the guard is destroyed. The guard automatically saves and restores the
/// strand previously active in the current thread, if any.
class StrandGuard
{
    ntci::Strand* d_current_p;
    ntci::Strand* d_previous_p;

  private:
    StrandGuard(const StrandGuard&) BSLS_KEYWORD_DELETED;
    StrandGuard& operator=(const StrandGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new strand guard that installs the specified 'strand'
    /// object into thread local storage and uninstalls it when this object
    /// is destroyed.
    explicit StrandGuard(ntci::Strand* strand);

    /// Uninstall the underlying strand object from thread local storage
    /// then destroy this object.
    ~StrandGuard();
};

NTCCFG_INLINE
bool Strand::passthrough(const bsl::shared_ptr<ntci::Strand>& calleeStrand,
                         const bsl::shared_ptr<ntci::Strand>& callerStrand)
{
    return !calleeStrand || calleeStrand == callerStrand;
}

NTCCFG_INLINE
StrandGuard::StrandGuard(ntci::Strand* strand)
: d_current_p(strand)
, d_previous_p(0)
{
    if (d_current_p) {
        d_previous_p = ntci::Strand::setThreadLocal(d_current_p);
    }
}

NTCCFG_INLINE
StrandGuard::~StrandGuard()
{
    if (d_current_p) {
        ntci::Strand::setThreadLocal(d_previous_p);
    }
}

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
