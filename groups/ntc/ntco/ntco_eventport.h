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

#ifndef INCLUDED_NTCO_EVENTPORT
#define INCLUDED_NTCO_EVENTPORT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_reactorconfig.h>
#include <ntccfg_platform.h>
#include <ntci_reactor.h>
#include <ntci_reactorfactory.h>
#include <ntci_user.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

#if NTC_BUILD_WITH_EVENTPORT
#if defined(BSLS_PLATFORM_OS_SOLARIS)

namespace BloombergLP {
namespace ntco {

/// @internal @brief
/// Provide a factory to produce reactors implemented using the "eventport"
/// API.
///
/// @details
/// This class implements the 'ntci::ReactorFactory' interface to produce
/// reactors implemented using the "eventport" API.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntco
class EventPortFactory : public ntci::ReactorFactory
{
    bslma::Allocator* d_allocator_p;

  private:
    EventPortFactory(const EventPortFactory&) BSLS_KEYWORD_DELETED;
    EventPortFactory& operator=(const EventPortFactory&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new reactor factory that produces reactors implemented
    /// using the "eventport" API. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit EventPortFactory(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EventPortFactory() BSLS_KEYWORD_OVERRIDE;

    /// Create a new reactor with the specified 'configuration' operating
    /// in the environment of the specified 'user'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    bsl::shared_ptr<ntci::Reactor> createReactor(
        const ntca::ReactorConfig&         configuration,
        const bsl::shared_ptr<ntci::User>& user,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace

#endif
#endif

#endif
