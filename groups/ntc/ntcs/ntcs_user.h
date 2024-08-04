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

#ifndef INCLUDED_NTCS_USER
#define INCLUDED_NTCS_USER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_datapool.h>
#include <ntci_proactormetrics.h>
#include <ntci_reactormetrics.h>
#include <ntci_reservation.h>
#include <ntci_resolver.h>
#include <ntci_user.h>
#include <ntcs_chronology.h>
#include <ntcs_metrics.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide implementation for an operational environment.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class User : public ntci::User
{
    bsl::shared_ptr<ntci::DataPool>        d_dataPool_sp;
    bsl::shared_ptr<ntci::Resolver>        d_resolver_sp;
    bsl::shared_ptr<ntci::Chronology>      d_chronology_sp;
    bsl::shared_ptr<ntci::Reservation>     d_connectionLimiter_sp;
    bsl::shared_ptr<ntci::ReactorMetrics>  d_reactorMetrics_sp;
    bsl::shared_ptr<ntci::ProactorMetrics> d_proactorMetrics_sp;
    bslma::Allocator*                      d_allocator_p;

  private:
    User(const User&) BSLS_KEYWORD_DELETED;
    User& operator=(const User&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new user. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit User(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~User() BSLS_KEYWORD_OVERRIDE;

    /// Set the data pool to the specified 'dataPool'.
    void setDataPool(const bsl::shared_ptr<ntci::DataPool>& dataPool);

    /// Set the resolver to the specified 'resolver'.
    void setResolver(const bsl::shared_ptr<ntci::Resolver>& resolver);

    /// Set the connection limiter to the specified 'connectionLimiter'.
    void setConnectionLimiter(
        const bsl::shared_ptr<ntci::Reservation>& connectionLimiter);

    /// Set the reactor metrics to the specified 'reactorMetrics'.
    void setReactorMetrics(
        const bsl::shared_ptr<ntci::ReactorMetrics>& reactorMetrics);

    /// Set the proactor metrics to the specified 'proactorMetrics'.
    void setProactorMetrics(
        const bsl::shared_ptr<ntci::ProactorMetrics>& proactorMetrics);

    /// Set the overall chronology perceived by all reactors or proactors to
    /// the specified 'chronology'.
    void setChronology(const bsl::shared_ptr<ntci::Chronology>& chronology);

    /// Return the data pool, if any.
    const bsl::shared_ptr<ntci::DataPool>& dataPool() const
        BSLS_KEYWORD_OVERRIDE;

    /// Return the resolver, if any.
    const bsl::shared_ptr<ntci::Resolver>& resolver() const
        BSLS_KEYWORD_OVERRIDE;

    // Return the shared chronology, if any.
    const bsl::shared_ptr<ntci::Chronology>& chronology() const 
    BSLS_KEYWORD_OVERRIDE;

    /// Return the connection amount limiter, if any.
    const bsl::shared_ptr<ntci::Reservation>& connectionLimiter() const
        BSLS_KEYWORD_OVERRIDE;

    /// Return the resolver, if any.
    const bsl::shared_ptr<ntci::ReactorMetrics>& reactorMetrics() const
        BSLS_KEYWORD_OVERRIDE;

    /// Return the resolver, if any.
    const bsl::shared_ptr<ntci::ProactorMetrics>& proactorMetrics() const
        BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace
#endif
