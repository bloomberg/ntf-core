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

#ifndef INCLUDED_NTCI_EXECUTOR
#define INCLUDED_NTCI_EXECUTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bsl_functional.h>
#include <bsl_list.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to execute functions asynchronously.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Executor
{
  public:
    /// Define a type alias for a deferred function.
    typedef NTCCFG_FUNCTION() Functor;

    /// This typedef defines a sequence of functors.
    typedef bsl::list<Functor> FunctorSequence;

    /// Destroy this object.
    virtual ~Executor();

    /// Defer the execution of the specified 'functor'.
    virtual void execute(const Functor& functor) = 0;

    /// Atomically defer the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    virtual void moveAndExecute(FunctorSequence* functorSequence,
                                const Functor&   functor) = 0;
};

}  // close package namespace
}  // close enterprise namespace
#endif
