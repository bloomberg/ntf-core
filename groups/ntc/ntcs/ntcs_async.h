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

#ifndef INCLUDED_NTCS_ASYNC
#define INCLUDED_NTCS_ASYNC

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_timeroptions.h>
#include <ntccfg_platform.h>
#include <ntci_executor.h>
#include <ntci_timer.h>
#include <ntci_timercallback.h>
#include <ntci_timersession.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bsl_functional.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide default asynchronous function execution for the process.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcs
class Async
{
  public:
    /// Define a type alias for a function.
    typedef ntci::Executor::Functor Functor;

    /// Define a type alias for a queue of functions.
    typedef ntci::Executor::FunctorSequence FunctorSequence;

    /// Create a new strand to serialize execution of functors. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static bsl::shared_ptr<ntci::Strand> createStrand(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'session' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&                  options,
        const bsl::shared_ptr<ntci::TimerSession>& session,
        bslma::Allocator*                          basicAllocator = 0);

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'callback' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&  options,
        const ntci::TimerCallback& callback,
        bslma::Allocator*          basicAllocator = 0);

    /// Execute the function on a thread managed by the process-wide
    /// asynchronous executor.
    static void execute(const Functor& function);

    /// Atomically defer the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    static void moveAndExecute(FunctorSequence* functorSequence,
                               const Functor&   functor);

    /// Return the current elapsed time since the Unix epoch.
    static bsls::TimeInterval currentTime();

  private:
    /// Provide global asynchronous state.
    class State;
};

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
