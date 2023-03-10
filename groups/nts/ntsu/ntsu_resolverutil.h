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

#ifndef INCLUDED_NTSU_RESOLVERUTIL
#define INCLUDED_NTSU_RESOLVERUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_ipaddress.h>
#include <ntsa_ipaddressoptions.h>
#include <ntsa_port.h>
#include <ntsa_portoptions.h>
#include <ntsa_transport.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsu {

/// @internal @brief
/// Provide utilities to resolve names to IP addresses and ports using the
/// operating system.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsu
struct ResolverUtil {
    /// Load into the specified 'result' the IP addresses assigned to
    /// the specified 'domainName'. Perform all resolution and validation of
    /// the characteristics of the desired 'result' according to the
    /// specified 'options'. Return the error.
    static ntsa::Error getIpAddress(bsl::vector<ntsa::IpAddress>* result,
                                    const bslstl::StringRef&      domainName,
                                    const ntsa::IpAddressOptions& options);

    /// Load into the specified 'result' the domain name corresponding to
    /// the specified 'ipAddress'. Return the error.
    static ntsa::Error getDomainName(bsl::string*           result,
                                     const ntsa::IpAddress& ipAddress);

    /// Load into the specified 'result' the port numbers assigned to the
    /// the specified 'serviceName'. Perform all resolution and validation
    /// of the characteristics of the desired 'result' according to the
    /// specified 'options'. Return the error.
    static ntsa::Error getPort(bsl::vector<ntsa::Port>* result,
                               const bslstl::StringRef& serviceName,
                               const ntsa::PortOptions& options);

    /// Load into the specified 'result' the service name to which the
    /// specified 'port' is assigned for use by the specified 'transport'.
    /// Return the error.
    static ntsa::Error getServiceName(bsl::string*           result,
                                      ntsa::Port             port,
                                      ntsa::Transport::Value transport);

    /// Load into the specified 'result' the IP addresses assigned to the
    /// local machine. Perform all resolution and validation of the
    /// characteristics of the desired 'result' according to the specified
    /// 'options'. Return the error.
    static ntsa::Error getLocalIpAddress(
        bsl::vector<ntsa::IpAddress>* result,
        const ntsa::IpAddressOptions& options);

    /// Return the hostname of the local machine.
    static ntsa::Error getHostname(bsl::string* result);

    /// Return the canonical, fully-qualified hostname of the local machine.
    static ntsa::Error getHostnameFullyQualified(bsl::string* result);

    /// Load into the specified 'result' the effective IP address type, if
    /// any, from the specified validated and evaluated 'options'. Return
    /// the error.
    static ntsa::Error classifyIpAddressType(
        bdlb::NullableValue<ntsa::IpAddressType::Value>* result,
        const ntsa::IpAddressOptions&                    options);

    /// Sort the specified 'ipAddressList' so that all the IPv4 addresses
    /// take precedence over all the IPv6 addresses. Retain the relative
    /// order of the addresses in each family.
    static void sortIpAddressList(bsl::vector<ntsa::IpAddress>* ipAddressList);
};

}  // end namespace ntsu
}  // end namespace BloombergLP
#endif
