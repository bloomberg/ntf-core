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

#ifndef INCLUDED_NTCI_GETDOMAINNAMECALLBACK
#define INCLUDED_NTCI_GETDOMAINNAMECALLBACK

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_getdomainnameevent.h>
#include <ntccfg_platform.h>
#include <ntci_callback.h>
#include <ntcscm_version.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntci {
class Resolver;
}
namespace ntci {

/// Define a type alias for callback invoked on a optional
/// strand with an optional cancelable authorization mechanism when a get
/// domain name operation completes or fails.
///
/// @ingroup module_ntci_operation_resolve
typedef ntci::Callback<void(const bsl::shared_ptr<ntci::Resolver>& resolver,
                            const bsl::string&                     domainName,
                            const ntca::GetDomainNameEvent&        event)>
    GetDomainNameCallback;

/// Define a type alias for function invoked when an get
/// domain name operation completes or fails.
///
/// @ingroup module_ntci_operation_resolve
typedef GetDomainNameCallback::FunctionType GetDomainNameFunction;

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
