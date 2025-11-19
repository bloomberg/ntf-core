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

#ifndef INCLUDED_NTSI_RESOLVER
#define INCLUDED_NTSI_RESOLVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_endpoint.h>
#include <ntsa_endpointoptions.h>
#include <ntsa_error.h>
#include <ntsa_ipaddress.h>
#include <ntsa_ipaddressoptions.h>
#include <ntsa_port.h>
#include <ntsa_portoptions.h>
#include <ntsa_transport.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsi {

/// Provide a resolver of names to IP addresses and TCP/UDP ports, and the
/// reverse.
///
/// @details
/// Provide an abstract mechanism to resolve names to IP addresses
/// and names to TCP/UDP ports. This class also provide reserve-resolution of
/// IP addresses to names, and TCP/UDP ports to names. The public member
/// functions of this class roughly correspond to the name resolution
/// functionality in the POSIX specification.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @par Usage Example: Getting the Local Hostname
/// This example shows how to use a resolver to get the local hostname.
///
/// To start, initialize the library.
///
///     ntsa::Error error = ntsf::System::initialize();
///     BSLS_ASSERT(!error);
///
/// Next, create a resolver.
///
///     bsl::shared_ptr<ntsi::Resolver> resolver =
///                                     ntsf::System::createResolver();
///
/// Next, get the unqualified lost hostname.
///
///     bsl::string hostname;
///     error = resolver->getHostname(&hostname);
///     BSLS_ASSERT(!error);
///
/// Next, get the fully-qualified domain name of the local host.
///
///     bsl::string hostnameFullyQualified;
///     resolver->getHostnameFullyQualified(&hostnameFullyQualified);
///     BSLS_ASSERT(!error);
///
///
/// @par Usage Example: Resolving a Domain Name to an IP Address
/// This example shows how to use a resolver to resolve a domain name to an
/// IP address.
///
/// To start, initialize the library.
///
///     ntsa::Error error = ntsf::System::initialize();
///     BSLS_ASSERT(!error);
///
/// Next, create a resolver.
///
///     bsl::shared_ptr<ntsi::Resolver> resolver =
///                                     ntsf::System::createResolver();
///
/// Next, resolve "dns.google.com" to the IP addresses it is assigned.
///
///     bsl::vector<ntsa::IpAddress> ipAddressList;
///     error = resolver->getIpAddress(&ipAddressList,
///                                    "dns.google.com",
///                                    ntsa::IpAddressOptions());
///     BSLS_ASSERT(!error);
///
/// Next, verify that the resolver has resolved "dns.google.com" into at least
/// some of the expected IP addresses (assigned as of January 1, 2020.)
///
///     bsl::set<ntsa::IpAddress> ipAddressSet;
///     ipAddressSet.insert(ntsa::IpAddress("8.8.8.8"));
///     ipAddressSet.insert(ntsa::IpAddress("8.8.4.4"));
///
///     for (bsl::vector<ntsa::IpAddress>::const_iterator
///             it  = ipAddressList.begin();
///             it != ipAddressList.end();
///           ++it)
///     {
///         bsl::size_t n = ipAddressSet.erase(*it);
///         BSLS_ASSERT(n == 1);
///     }
///
///     BSLS_ASSERT(ipAddressSet.empty());
///
///
/// @par Usage Example: Resolving an IP Address to a Domain Name
/// This example shows how to use a resolver to resolve an IP address to a
/// domain name.
///
/// To start, initialize the library.
///
///     ntsa::Error error = ntsf::System::initialize();
///     BSLS_ASSERT(!error);
///
/// Next, create a resolver.
///
///     bsl::shared_ptr<ntsi::Resolver> resolver =
///                                     ntsf::System::createResolver();
///
/// Next, resolve "8.8.8.8", one of the IP addresses of Google's Public DNS
/// servers, to the domain name assigned that IP address (assigned as of
/// January 1, 2020.)
///
///     bsl::string domainName;
///     error = resolver->getDomainName(&domainName,
///                                     ntsa::IpAddress("8.8.8.8"));
///     BSLS_ASSERT(!error);
///
///     BSLS_ASSERT(domainName == "dns.google");
///
///
/// @see ntsa_ipaddress.h, ntsa_domainname.h
///
/// @ingroup module_ntsi
class Resolver
{
  public:
    /// Destroy this object.
    virtual ~Resolver();

    /// Set the specified 'domainName' to resolve to the specified
    /// 'ipAddressList', and vice-versa. Return the error. Note that calling
    /// this function affects the future behavior of this object only:
    /// 'ipAddressList' will be subsequently returned from calling
    /// 'getIpAddress()' of 'domainName', and 'domainName' will be returned
    /// from calling 'getDomainName()' of any of the addresses in
    /// 'ipAddressList', but this function does not affect any name
    /// resolution caches maintained elsewhere by the system.
    virtual ntsa::Error setIpAddress(
        const bslstl::StringRef&            domainName,
        const bsl::vector<ntsa::IpAddress>& ipAddressList);

    /// Add the specified 'domainName' to resolve to the specified
    /// 'ipAddressList', and vice-versa, in addition to any previously,
    /// explicitly defined associations. Return the error. Note that calling
    /// this function affects the future behavior of this object only:
    /// 'ipAddressList' will be subsequently returned from calling
    /// 'getIpAddress()' of 'domainName', and 'domainName' will be returned
    /// from calling 'getDomainName()' of any of the addresses in
    /// 'ipAddressList', but this function does not affect any name
    /// resolution caches maintained elsewhere by the system.
    virtual ntsa::Error addIpAddress(
        const bslstl::StringRef&            domainName,
        const bsl::vector<ntsa::IpAddress>& ipAddressList);

    /// Add the specified 'domainName' to resolve to the specified
    /// 'ipAddress', and vice-versa, in addition to any previously,
    /// explicitly defined associations. Return the error. Note that calling
    /// this function affects the future behavior of this object only:
    /// 'ipAddress' will be subsequently returned from calling
    /// 'getIpAddress()' of 'domainName', and 'domainName' will be returned
    /// from calling 'getDomainName()' of any of the addresses in
    /// 'ipAddressList', but this function does not affect any name
    /// resolution caches maintained elsewhere by the system.
    virtual ntsa::Error addIpAddress(const bslstl::StringRef& domainName,
                                     const ntsa::IpAddress&   ipAddress);

    /// Set the specified 'serviceName' to resolve to the specified
    /// 'portList' for use by the specified 'transport', and vice-versa.
    /// Return the error. Note that calling this function affects the future
    /// behavior of this object only: 'port' will be subsequently returned
    /// from calling 'getPort()' of 'serviceName', and 'serviceName' will be
    /// returned from calling 'getServiceName()' of any of the ports in
    /// 'portList', but this function does not affect any name resolution
    /// caches maintained elsewhere by the system.
    virtual ntsa::Error setPort(const bslstl::StringRef&       serviceName,
                                const bsl::vector<ntsa::Port>& portList,
                                ntsa::Transport::Value         transport);

    /// Add the specified 'serviceName' to resolve to the specified
    /// 'portList' for use by the specified 'transport', and vice-versa,
    /// in addition to any previously, explicitly defined associations.
    /// Return the error. Note that calling this function affects the future
    /// behavior of this object only: 'port' will be subsequently returned
    /// from calling 'getPort()' of 'serviceName', and 'serviceName' will be
    /// returned from calling 'getServiceName()' of any of the ports in
    /// 'portList', but this function does not affect any name resolution
    /// caches maintained elsewhere by the system.
    virtual ntsa::Error addPort(const bslstl::StringRef&       serviceName,
                                const bsl::vector<ntsa::Port>& portList,
                                ntsa::Transport::Value         transport);

    /// Add the specified 'serviceName' to resolve to the specified
    /// 'port' for use by the specified 'transport', and vice-versa,
    /// in addition to any previously, explicitly defined associations.
    /// Return the error. Note that calling this function affects the future
    /// behavior of this object only: 'port' will be subsequently returned
    /// from calling 'getPort()' of 'serviceName', and 'serviceName' will be
    /// returned from calling 'getServiceName()' of any of the ports in
    /// 'portList', but this function does not affect any name resolution
    /// caches maintained elsewhere by the system.
    virtual ntsa::Error addPort(const bslstl::StringRef& serviceName,
                                ntsa::Port               port,
                                ntsa::Transport::Value   transport);

    /// Set the local IP addresses assigned to the local machine to the
    /// specified 'ipAddressList'. Return the error. Note that calling this
    /// function affects the future behavior of this object only:
    /// 'ipAddressList' will be subsequently returned from calling
    /// 'getLocalIpAddress()' but this function does not set the local IP
    /// addresses of the system or have any wider effect on other objects or
    /// name resolution functionality in this process.
    virtual ntsa::Error setLocalIpAddress(
        const bsl::vector<ntsa::IpAddress>& ipAddressList);

    /// Set the hostname of the local machine to the specified 'name'.
    /// Return the error. Note that calling this function affects the future
    /// behavior of this object only: 'name' will be subsequently returned
    /// from calling 'getHostname()' but this function does not set the
    /// hostname of the system or have any wider effect on other objects or
    /// name resolution functionality in this process.
    virtual ntsa::Error setHostname(const bsl::string& name);

    /// Set the canonical, fully-qualified hostname of the local machine to
    /// the specified 'name'. Return the error. Note that calling this
    /// function affects the future behavior of this object only: 'name'
    /// will be subsequently returned from calling
    /// 'getHostnameFullyQualified()' but this function does not set the
    /// hostname of the system or have any wider effect on other objects or
    /// name resolution functionality in this process.
    virtual ntsa::Error setHostnameFullyQualified(const bsl::string& name);

    /// Load into the specified 'result' the IP addresses assigned to
    /// the specified 'domainName'. Perform all resolution and validation of
    /// the characteristics of the desired 'result' according to the
    /// specified 'options'. Return the error.
    virtual ntsa::Error getIpAddress(bsl::vector<ntsa::IpAddress>* result,
                                     const bslstl::StringRef&      domainName,
                                     const ntsa::IpAddressOptions& options);

    /// Load into the specified 'result' the domain name to which the
    /// specified 'ipAddress' is assigned. Return the error.
    virtual ntsa::Error getDomainName(bsl::string*           result,
                                      const ntsa::IpAddress& ipAddress);

    /// Load into the specified 'result' the port numbers assigned to the
    /// the specified 'serviceName'. Perform all resolution and validation
    /// of the characteristics of the desired 'result' according to the
    /// specified 'options'. Return the error.
    virtual ntsa::Error getPort(bsl::vector<ntsa::Port>* result,
                                const bslstl::StringRef& serviceName,
                                const ntsa::PortOptions& options);

    /// Load into the specified 'result' the service name to which the
    /// specified 'port' is assigned for use by the specified 'transport'.
    /// Return the error.
    virtual ntsa::Error getServiceName(bsl::string*           result,
                                       ntsa::Port             port,
                                       ntsa::Transport::Value transport);

    /// Load into the specified 'result' the endpoint parsed and potentially
    /// resolved from the components of the specified 'text', in the format
    /// of '<port>' or '[<host>][:<port>]'. If the optionally specified
    /// '<host>' component is not an IP address, interpret the '<host>' as
    /// a domain name and resolve it into an IP address. If the optionally
    /// specified '<port>' is a name and not a number, interpret the
    /// '<port>' as a service name and resolve it into a port. Perform all
    /// resolution and validation of the characteristics of the desired
    /// 'result' according to the specified 'options'. Return the error.
    virtual ntsa::Error getEndpoint(ntsa::Endpoint*              result,
                                    const bslstl::StringRef&     text,
                                    const ntsa::EndpointOptions& options);

    /// Load into the specified 'result' the endpoints parsed and potentially
    /// resolved from the components of the specified 'text', in the format of
    /// '<port>' or '[<host>][:<port>]'. If the optionally specified '<host>'
    /// component is not an IP address, interpret the '<host>' as a domain name
    /// and resolve it into the IP addresses assigned to that domain name. If
    /// the optionally specified '<port>' is a name and not a number, interpret
    /// the '<port>' as a service name and resolve it into the ports assigned
    /// to that service name. Perform all resolution and validation of the
    /// characteristics of the desired 'result' according to the specified
    /// 'options'. Return the error. Note if 'text' contains no component that
    /// needs resolution, and its format is valid, 'result' will contain a
    /// single element. Also note that if 'text' contains components that
    /// require resolution, and those components resolve to more than one
    /// IP address or port number, then 'result' will contain all combinations
    /// of the resolved IP addresses and port numbers.
    virtual ntsa::Error getEndpoint(bsl::vector<ntsa::Endpoint>* result,
                                    const bslstl::StringRef&     text,
                                    const ntsa::EndpointOptions& options);

    /// Load into the specified 'result' the IP addresses assigned to the
    /// local machine. Perform all resolution and validation of the
    /// characteristics of the desired 'result' according to the specified
    /// 'options'. Return the error.
    virtual ntsa::Error getLocalIpAddress(
        bsl::vector<ntsa::IpAddress>* result,
        const ntsa::IpAddressOptions& options);

    /// Return the hostname of the local machine.
    virtual ntsa::Error getHostname(bsl::string* result);

    /// Return the canonical, fully-qualified hostname of the local machine.
    virtual ntsa::Error getHostnameFullyQualified(bsl::string* result);
};

}  // close package namespace
}  // close enterprise namespace
#endif
