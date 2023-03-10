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

#ifndef INCLUDED_NTCO_IOCP
#define INCLUDED_NTCO_IOCP

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_proactorconfig.h>
#include <ntccfg_platform.h>
#include <ntci_proactor.h>
#include <ntci_proactorfactory.h>
#include <ntci_user.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

#if NTC_BUILD_WITH_IOCP
#if defined(BSLS_PLATFORM_OS_WINDOWS)

namespace BloombergLP {
namespace ntco {

/// @internal @brief
/// Provide a factory to produce proactors implemented using the I/O completion
/// port API.
///
/// @details
/// This class implements the 'ntci::ProactorFactory' interface to produce
/// proactors implemented using the I/O completion port API.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntco
class IocpFactory : public ntci::ProactorFactory
{
    bslma::Allocator* d_allocator_p;

  private:
    IocpFactory(const IocpFactory&) BSLS_KEYWORD_DELETED;
    IocpFactory& operator=(const IocpFactory&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new proactor factory that produces proactors implemented
    /// using the I/O completion port API. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit IocpFactory(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~IocpFactory() BSLS_KEYWORD_OVERRIDE;

    /// Create a new proactor with the specified 'configuration' operating
    /// in the environment of the specified 'user'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    bsl::shared_ptr<ntci::Proactor> createProactor(
        const ntca::ProactorConfig&        configuration,
        const bsl::shared_ptr<ntci::User>& user,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace

#endif
#endif

#endif
