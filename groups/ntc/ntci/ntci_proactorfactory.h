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

#ifndef INCLUDED_NTCI_PROACTORFACTORY
#define INCLUDED_NTCI_PROACTORFACTORY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_proactorconfig.h>
#include <ntccfg_platform.h>
#include <ntci_proactor.h>
#include <ntci_user.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to create proactors.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_proactor
class ProactorFactory
{
  public:
    /// Destroy this object.
    virtual ~ProactorFactory();

    /// Create a new proactor with the specified 'configuration' operating
    /// in the environment of the specified 'user'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    virtual bsl::shared_ptr<ntci::Proactor> createProactor(
        const ntca::ProactorConfig&        configuration,
        const bsl::shared_ptr<ntci::User>& user,
        bslma::Allocator*                  basicAllocator = 0) = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
