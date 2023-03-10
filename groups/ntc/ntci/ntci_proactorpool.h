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

#ifndef INCLUDED_NTCI_PROACTORPOOL
#define INCLUDED_NTCI_PROACTORPOOL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_loadbalancingoptions.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {
class Proactor;
}
namespace ntci {

/// Provide a pool of proactors within which sockets are load balanced.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_proactor
class ProactorPool
{
  public:
    /// Destroy this object.
    virtual ~ProactorPool();

    /// Acquire usage of the most suitable proactor selected according to
    /// the specified load balancing 'options'.
    virtual bsl::shared_ptr<ntci::Proactor> acquireProactor(
        const ntca::LoadBalancingOptions& options) = 0;

    /// Release usage of the specified 'proactor' selected according to the
    /// specified load balancing 'options'.
    virtual void releaseProactor(
        const bsl::shared_ptr<ntci::Proactor>& proactor,
        const ntca::LoadBalancingOptions&      options) = 0;

    /// Increment the current number of handle reservations, if permitted.
    /// Return true if the resulting number of handle reservations is
    /// permitted, and false otherwise.
    virtual bool acquireHandleReservation() = 0;

    /// Decrement the current number of handle reservations.
    virtual void releaseHandleReservation() = 0;

    /// Return the number of proactors in the thread pool.
    virtual bsl::size_t numProactors() const = 0;

    /// Return the current number of threads in the thread pool.
    virtual bsl::size_t numThreads() const = 0;

    /// Return the minimum number of threads in the thread pool.
    virtual bsl::size_t minThreads() const = 0;

    /// Return the maximum number of threads in the thread pool.
    virtual bsl::size_t maxThreads() const = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
