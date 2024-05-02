// Copyright 2024 Bloomberg Finance L.P.
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
#ifndef INCLUDED_NTCD_RESOLVER
#define INCLUDED_NTCD_RESOLVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntci_resolver.h>
#include <ntccfg_test.h>

namespace BloombergLP {
namespace ntcd {

NTF_MOCK_CLASS(ResolverMock, ntci::Resolver)

NTF_MOCK_METHOD(void, execute, const Functor&)
NTF_MOCK_METHOD(void, moveAndExecute, FunctorSequence*, const Functor&)
NTF_MOCK_METHOD_CONST(bsl::shared_ptr<ntci::Strand>&, strand)

NTF_MOCK_METHOD(ntsa::Error, start)
NTF_MOCK_METHOD(void, shutdown)
NTF_MOCK_METHOD(void, linger)
NTF_MOCK_METHOD(ntsa::Error,
                setIpAddress,
                const bslstl::StringRef&,
                const bsl::vector<ntsa::IpAddress>&)
NTF_MOCK_METHOD(ntsa::Error,
                addIpAddress,
                const bslstl::StringRef&,
                const bsl::vector<ntsa::IpAddress>&)
NTF_MOCK_METHOD(ntsa::Error,
                addIpAddress,
                const bslstl::StringRef&,
                const ntsa::IpAddress&)

NTF_MOCK_METHOD(ntsa::Error,
                setPort,
                const bslstl::StringRef&,
                const bsl::vector<ntsa::Port>&,
                ntsa::Transport::Value)

NTF_MOCK_METHOD(ntsa::Error,
                addPort,
                const bslstl::StringRef&,
                const bsl::vector<ntsa::Port>&,
                ntsa::Transport::Value)

NTF_MOCK_METHOD(ntsa::Error,
                addPort,
                const bslstl::StringRef&,
                ntsa::Port,
                ntsa::Transport::Value)

NTF_MOCK_METHOD(ntsa::Error,
                setLocalIpAddress,
                const bsl::vector<ntsa::IpAddress>&)
NTF_MOCK_METHOD(ntsa::Error, setHostname, const bsl::string&)
NTF_MOCK_METHOD(ntsa::Error, setHostnameFullyQualified, const bsl::string&)

NTF_MOCK_METHOD(ntsa::Error,
                getIpAddress,
                const bslstl::StringRef&,
                const ntca::GetIpAddressOptions&,
                const ntci::GetIpAddressCallback&)

NTF_MOCK_METHOD(ntsa::Error,
                getDomainName,
                const ntsa::IpAddress&,
                const ntca::GetDomainNameOptions&,
                const ntci::GetDomainNameCallback&)

NTF_MOCK_METHOD(ntsa::Error,
                getPort,
                const bslstl::StringRef&,
                const ntca::GetPortOptions&,
                const ntci::GetPortCallback&)

NTF_MOCK_METHOD(ntsa::Error,
                getServiceName,
                ntsa::Port,
                const ntca::GetServiceNameOptions&,
                const ntci::GetServiceNameCallback&)

NTF_MOCK_METHOD(ntsa::Error,
                getEndpoint,
                const bslstl::StringRef&,
                const ntca::GetEndpointOptions&,
                const ntci::GetEndpointCallback&)

NTF_MOCK_METHOD(ntsa::Error,
                getLocalIpAddress,
                bsl::vector<ntsa::IpAddress>*,
                const ntsa::IpAddressOptions&)
NTF_MOCK_METHOD(ntsa::Error, getHostname, bsl::string*)
NTF_MOCK_METHOD(ntsa::Error, getHostnameFullyQualified, bsl::string*)
NTF_MOCK_METHOD(bsl::shared_ptr<ntci::Strand>, createStrand, bslma::Allocator*)

NTF_MOCK_METHOD(bsl::shared_ptr<ntci::Timer>,
                createTimer,
                const ntca::TimerOptions&,
                const bsl::shared_ptr<ntci::TimerSession>&,
                bslma::Allocator*)

NTF_MOCK_METHOD(bsl::shared_ptr<ntci::Timer>,
                createTimer,
                const ntca::TimerOptions&,
                const ntci::TimerCallback&,
                bslma::Allocator*)
NTF_MOCK_METHOD_CONST(bsls::TimeInterval, currentTime)

NTF_MOCK_CLASS_END;


}  // close package namespace
}  // close enterprise namespace

#endif
