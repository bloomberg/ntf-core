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

#ifndef INCLUDED_NTCI_USER
#define INCLUDED_NTCI_USER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_datapool.h>
#include <ntci_chronology.h>
#include <ntci_proactormetrics.h>
#include <ntci_reactormetrics.h>
#include <ntci_reservation.h>
#include <ntci_resolver.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface for an operational environment.
///
/// @details
// The operational environment defines the optional shared data pools,
// resolver, limits, and metrics shared between reactors and proactors and
// their sockets.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_utility
class User
{
  public:
    /// Destroy this object.
    virtual ~User();

    /// Return the data pool, if any.
    virtual const bsl::shared_ptr<ntci::DataPool>& dataPool() const = 0;

    /// Return the resolver, if any.
    virtual const bsl::shared_ptr<ntci::Resolver>& resolver() const = 0;

    // Return the shared chronology, if any.
    virtual const bsl::shared_ptr<ntci::Chronology>& chronology() const = 0;

    /// Return the connection amount limiter, if any.
    virtual const bsl::shared_ptr<ntci::Reservation>& connectionLimiter()
        const = 0;

    /// Return the reactor metrics, if any.
    virtual const bsl::shared_ptr<ntci::ReactorMetrics>& reactorMetrics()
        const = 0;

    /// Return the proactor metrics, if any.
    virtual const bsl::shared_ptr<ntci::ProactorMetrics>& proactorMetrics()
        const = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
