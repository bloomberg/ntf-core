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

#ifndef INCLUDED_NTSB_RESOLVER
#define INCLUDED_NTSB_RESOLVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_ipaddress.h>
#include <ntsa_port.h>
#include <ntsa_resolverconfig.h>
#include <ntscfg_platform.h>
#include <ntsi_resolver.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslmt_mutex.h>
#include <bsls_atomic.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsb {

/// @internal @brief
/// Provides overrides to a resolver.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsb
class ResolverOverrides
{
    /// Define a type alias for a vector of IP addresses.
    typedef bsl::vector<ntsa::IpAddress> IpAddressVector;

    /// Define a type alias for a map of domain names to an
    /// associated vector of IP addresses.
    typedef bsl::unordered_map<bsl::string, IpAddressVector>
        IpAddressByDomainName;

    /// Define a type alias for a map of IP addresses to
    /// domain names.
    typedef bsl::unordered_map<ntsa::IpAddress, bsl::string>
        DomainNameByIpAddress;

    /// Define a type alias for a vector of port numbers.
    typedef bsl::vector<ntsa::Port> PortVector;

    /// Define a type alias for a map of service names to an
    /// associated vector of port numbers.
    typedef bsl::unordered_map<bsl::string, PortVector> PortByServiceName;

    /// Define a type alias for a map of port numbers to
    /// service names.
    typedef bsl::unordered_map<ntsa::Port, bsl::string> ServiceNameByPort;

    mutable ntscfg::Mutex            d_mutex;
    IpAddressByDomainName            d_ipAddressByDomainName;
    DomainNameByIpAddress            d_domainNameByIpAddress;
    PortByServiceName                d_tcpPortByServiceName;
    ServiceNameByPort                d_tcpServiceNameByPort;
    PortByServiceName                d_udpPortByServiceName;
    ServiceNameByPort                d_udpServiceNameByPort;
    IpAddressVector                  d_localIpAddressList;
    bdlb::NullableValue<bsl::string> d_hostname;
    bdlb::NullableValue<bsl::string> d_hostnameFullyQualified;
    bslma::Allocator*                d_allocator_p;

  private:
    ResolverOverrides(const ResolverOverrides&) BSLS_KEYWORD_DELETED;
    ResolverOverrides& operator=(const ResolverOverrides&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit ResolverOverrides(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ResolverOverrides();

    /// Set the specified 'domainName' to resolve to the specified
    /// 'ipAddressList', and vice-versa. Return the error.
    ntsa::Error setIpAddress(
        const bslstl::StringRef&            domainName,
        const bsl::vector<ntsa::IpAddress>& ipAddressList);

    /// Add the specified 'domainName' to resolve to the specified
    /// 'ipAddressList', and vice-versa, in addition to any previously,
    /// explicitly defined associations. Return the error.
    ntsa::Error addIpAddress(
        const bslstl::StringRef&            domainName,
        const bsl::vector<ntsa::IpAddress>& ipAddressList);

    /// Add the specified 'domainName' to resolve to the specified
    /// 'ipAddress', and vice-versa, in addition to any previously,
    /// explicitly defined associations. Return the error.
    ntsa::Error addIpAddress(const bslstl::StringRef& domainName,
                             const ntsa::IpAddress&   ipAddress);

    /// Set the specified 'serviceName' to resolve to the specified
    /// 'portList' for use by the specified 'transport', and vice-versa.
    /// Return the error.
    ntsa::Error setPort(const bslstl::StringRef&       serviceName,
                        const bsl::vector<ntsa::Port>& portList,
                        ntsa::Transport::Value         transport);

    /// Add the specified 'serviceName' to resolve to the specified
    /// 'portList' for use by the specified 'transport', and vice-versa,
    /// in addition to any previously, explicitly defined associations.
    /// Return the error.
    ntsa::Error addPort(const bslstl::StringRef&       serviceName,
                        const bsl::vector<ntsa::Port>& portList,
                        ntsa::Transport::Value         transport);

    /// Add the specified 'serviceName' to resolve to the specified
    /// 'port' for use by the specified 'transport', and vice-versa,
    /// in addition to any previously, explicitly defined associations.
    /// Return the error.
    ntsa::Error addPort(const bslstl::StringRef& serviceName,
                        ntsa::Port               port,
                        ntsa::Transport::Value   transport);

    /// Set the local IP addresses assigned to the local machine to the
    /// specified 'ipAddressList'. Return the error.
    ntsa::Error setLocalIpAddress(
        const bsl::vector<ntsa::IpAddress>& ipAddressList);

    /// Set the hostname of the local machine to the specified 'name'.
    /// Return the error.
    ntsa::Error setHostname(const bsl::string& name);

    /// Set the canonical, fully-qualified hostname of the local machine to
    /// the specified 'name'. Return the error.
    ntsa::Error setHostnameFullyQualified(const bsl::string& name);

    /// Load into the specified 'result' the IP addresses assigned to
    /// the specified 'domainName'. Perform all resolution and validation of
    /// the characteristics of the desired 'result' according to the
    /// specified 'options'. Return the error.
    ntsa::Error getIpAddress(bsl::vector<ntsa::IpAddress>* result,
                             const bslstl::StringRef&      domainName,
                             const ntsa::IpAddressOptions& options) const;

    /// Load into the specified 'result' the domain name to which the
    /// specified 'ipAddress' is assigned. Return the error.
    ntsa::Error getDomainName(bsl::string*           result,
                              const ntsa::IpAddress& ipAddress) const;

    /// Load into the specified 'result' the port numbers assigned to the
    /// the specified 'serviceName'. Perform all resolution and validation
    /// of the characteristics of the desired 'result' according to the
    /// specified 'options'. Return the error.
    ntsa::Error getPort(bsl::vector<ntsa::Port>* result,
                        const bslstl::StringRef& serviceName,
                        const ntsa::PortOptions& options) const;

    /// Load into the specified 'result' the service name to which the
    /// specified 'port' is assigned for use by the specified 'transport'.
    /// Return the error.
    ntsa::Error getServiceName(bsl::string*           result,
                               ntsa::Port             port,
                               ntsa::Transport::Value transport) const;

    /// Load into the specified 'result' the IP addresses assigned to the
    /// local machine. Perform all resolution and validation of the
    /// characteristics of the desired 'result' according to the specified
    /// 'options'. Return the error.
    ntsa::Error getLocalIpAddress(bsl::vector<ntsa::IpAddress>* result,
                                  const ntsa::IpAddressOptions& options) const;

    /// Return the hostname of the local machine.
    ntsa::Error getHostname(bsl::string* result) const;

    /// Return the canonical, fully-qualified hostname of the local machine.
    ntsa::Error getHostnameFullyQualified(bsl::string* result) const;
};

/// Provide a blocking resolver implemented by the system.
///
/// @par Thread Safey
/// This class is thread safe.
class Resolver : public ntsi::Resolver
{
    ntsb::ResolverOverrides d_overrides;
    bsls::AtomicBool        d_overridesExist;
    const bool              d_overridesEnabled;
    const bool              d_systemEnabled;
    bslma::Allocator*       d_allocator_p;

  private:
    Resolver(const Resolver&) BSLS_KEYWORD_DELETED;
    Resolver& operator=(const Resolver&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new resolver. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Resolver(bslma::Allocator* basicAllocator = 0);

    /// Create a new resolver with the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit Resolver(const ntsa::ResolverConfig& configuration,
                      bslma::Allocator*           basicAllocator = 0);

    /// Destroy this object.
    ~Resolver() BSLS_KEYWORD_OVERRIDE;

    /// Set the specified 'domainName' to resolve to the specified
    /// 'addressList', and vice-versa. Return the error. Note that calling
    /// this function affects the future behavior of this object only:
    /// 'addressList' will be subsequently returned from calling
    /// 'getIpAddress()' of 'domainName', and 'domainName' will be returned
    /// from calling 'getDomainName()' of any of the addresses in
    /// 'ipAddressList', but this function does not affect any name
    /// resolution caches maintained elsewhere by the system.
    ntsa::Error setIpAddress(const bslstl::StringRef&            domainName,
                             const bsl::vector<ntsa::IpAddress>& ipAddressList)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'domainName' to resolve to the specified
    /// 'ipAddressList', and vice-versa, in addition to any previously,
    /// explicitly defined associations. Return the error. Note that calling
    /// this function affects the future behavior of this object only:
    /// 'addressList' will be subsequently returned from calling
    /// 'getIpAddress()' of 'domainName', and 'domainName' will be returned
    /// from calling 'getDomainName()' of any of the addresses in
    /// 'ipAddressList', but this function does not affect any name
    /// resolution caches maintained elsewhere by the system.
    ntsa::Error addIpAddress(const bslstl::StringRef&            domainName,
                             const bsl::vector<ntsa::IpAddress>& ipAddressList)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'domainName' to resolve to the specified
    /// 'ipAddress', and vice-versa, in addition to any previously,
    /// explicitly defined associations. Return the error. Note that calling
    /// this function affects the future behavior of this object only:
    /// 'addressList' will be subsequently returned from calling
    /// 'getIpAddress()' of 'domainName', and 'domainName' will be returned
    /// from calling 'getDomainName()' of any of the addresses in
    /// 'ipAddressList', but this function does not affect any name
    /// resolution caches maintained elsewhere by the system.
    ntsa::Error addIpAddress(const bslstl::StringRef& domainName,
                             const ntsa::IpAddress&   ipAddress)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the specified 'serviceName' to resolve to the specified
    /// 'portList' for use by the specified 'transport', and vice-versa.
    /// Return the error. Note that calling this function affects the future
    /// behavior of this object only: 'port' will be subsequently returned
    /// from calling 'getPort()' of 'serviceName', and 'serviceName' will be
    /// returned from calling 'getServiceName()' of any of the ports in
    /// 'portList', but this function does not affect any name resolution
    /// caches maintained elsewhere by the system.
    ntsa::Error setPort(const bslstl::StringRef&       serviceName,
                        const bsl::vector<ntsa::Port>& portList,
                        ntsa::Transport::Value         transport)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'serviceName' to resolve to the specified
    /// 'portList' for use by the specified 'transport', and vice-versa,
    /// in addition to any previously, explicitly defined associations.
    /// Return the error. Note that calling this function affects the future
    /// behavior of this object only: 'port' will be subsequently returned
    /// from calling 'getPort()' of 'serviceName', and 'serviceName' will be
    /// returned from calling 'getServiceName()' of any of the ports in
    /// 'portList', but this function does not affect any name resolution
    /// caches maintained elsewhere by the system.
    ntsa::Error addPort(const bslstl::StringRef&       serviceName,
                        const bsl::vector<ntsa::Port>& portList,
                        ntsa::Transport::Value         transport)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'serviceName' to resolve to the specified
    /// 'port' for use by the specified 'transport', and vice-versa,
    /// in addition to any previously, explicitly defined associations.
    /// Return the error. Note that calling this function affects the future
    /// behavior of this object only: 'port' will be subsequently returned
    /// from calling 'getPort()' of 'serviceName', and 'serviceName' will be
    /// returned from calling 'getServiceName()' of any of the ports in
    /// 'portList', but this function does not affect any name resolution
    /// caches maintained elsewhere by the system.
    ntsa::Error addPort(const bslstl::StringRef& serviceName,
                        ntsa::Port               port,
                        ntsa::Transport::Value   transport)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the local IP addresses assigned to the local machine to the
    /// specified 'ipAddressList'. Return the error. Note that calling this
    /// function affects the future behavior of this object only:
    /// 'ipAddressList' will be subsequently returned from calling
    /// 'getLocalIpAddress()' but this function does not set the local IP
    /// addresses of the system or have any wider effect on other objects or
    /// name resolution functionality in this process.
    ntsa::Error setLocalIpAddress(const bsl::vector<ntsa::IpAddress>&
                                      ipAddressList) BSLS_KEYWORD_OVERRIDE;

    /// Set the hostname of the local machine to the specified 'name'.
    /// Return the error. Note that calling this function affects the future
    /// behavior of this object only: 'name' will be subsequently returned
    /// from calling 'getHostname()' but this function does not set the
    /// hostname of the system or have any wider effect on other objects or
    /// name resolution functionality in this process.
    ntsa::Error setHostname(const bsl::string& name) BSLS_KEYWORD_OVERRIDE;

    /// Set the canonical, fully-qualified hostname of the local machine to
    /// the specified 'name'. Return the error. Note that calling this
    /// function affects the future behavior of this object only: 'name'
    /// will be subsequently returned from calling
    /// 'getHostnameFullyQualified()' but this function does not set the
    /// hostname of the system or have any wider effect on other objects or
    /// name resolution functionality in this process.
    ntsa::Error setHostnameFullyQualified(const bsl::string& name)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the IP addresses assigned to
    /// the specified 'domainName'. Perform all resolution and validation of
    /// the characteristics of the desired 'result' according to the
    /// specified 'options'. Return the error.
    ntsa::Error getIpAddress(bsl::vector<ntsa::IpAddress>* result,
                             const bslstl::StringRef&      domainName,
                             const ntsa::IpAddressOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the domain name to which the
    /// specified 'ipAddress' is assigned. Return the error.
    ntsa::Error getDomainName(bsl::string*           result,
                              const ntsa::IpAddress& ipAddress)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the port numbers assigned to the
    /// the specified 'serviceName'. Perform all resolution and validation
    /// of the characteristics of the desired 'result' according to the
    /// specified 'options'. Return the error.
    ntsa::Error getPort(bsl::vector<ntsa::Port>* result,
                        const bslstl::StringRef& serviceName,
                        const ntsa::PortOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the service name to which the
    /// specified 'port' is assigned for use by the specified 'transport'.
    /// Return the error.
    ntsa::Error getServiceName(bsl::string*           result,
                               ntsa::Port             port,
                               ntsa::Transport::Value transport)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the endpoint parsed and potentially
    /// resolved from the components of the specified 'text', in the format
    /// of '<port>' or '[<host>][:<port>]'. If the optionally specified
    /// '<host>' component is not an IP address, interpret the '<host>' as
    /// a domain name and resolve it into an IP address. If the optionally
    /// specified '<port>' is a name and not a number, interpret the
    /// '<port>' as a service name and resolve it into a port. Perform all
    /// resolution and validation of the characteristics of the desired
    /// 'result' according to the specified 'options'. Return the error.
    ntsa::Error getEndpoint(ntsa::Endpoint*              result,
                            const bslstl::StringRef&     text,
                            const ntsa::EndpointOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the IP addresses assigned to the
    /// local machine. Perform all resolution and validation of the
    /// characteristics of the desired 'result' according to the specified
    /// 'options'. Return the error.
    ntsa::Error getLocalIpAddress(bsl::vector<ntsa::IpAddress>* result,
                                  const ntsa::IpAddressOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Return the hostname of the local machine.
    ntsa::Error getHostname(bsl::string* result) BSLS_KEYWORD_OVERRIDE;

    /// Return the canonical, fully-qualified hostname of the local machine.
    ntsa::Error getHostnameFullyQualified(bsl::string* result)
        BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace
#endif
