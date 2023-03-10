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

#ifndef INCLUDED_NTCI_RESOLVER
#define INCLUDED_NTCI_RESOLVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_getdomainnamecontext.h>
#include <ntca_getdomainnameoptions.h>
#include <ntca_getendpointcontext.h>
#include <ntca_getendpointoptions.h>
#include <ntca_getipaddresscontext.h>
#include <ntca_getipaddressoptions.h>
#include <ntca_getportcontext.h>
#include <ntca_getportoptions.h>
#include <ntca_getservicenamecontext.h>
#include <ntca_getservicenameoptions.h>
#include <ntccfg_platform.h>
#include <ntci_executor.h>
#include <ntci_getdomainnamecallbackfactory.h>
#include <ntci_getendpointcallbackfactory.h>
#include <ntci_getipaddresscallbackfactory.h>
#include <ntci_getportcallbackfactory.h>
#include <ntci_getservicenamecallbackfactory.h>
#include <ntci_strand.h>
#include <ntci_strandfactory.h>
#include <ntci_timer.h>
#include <ntci_timerfactory.h>
#include <ntcscm_version.h>
#include <ntsa_endpointoptions.h>
#include <ntsa_error.h>
#include <ntsa_ipaddress.h>
#include <ntsa_ipaddressoptions.h>
#include <ntsa_port.h>
#include <ntsa_portoptions.h>
#include <ntsa_transport.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntci {

/// Provide a resolver of domain names, service names, IP addresses, and ports.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_operation_resolve
class Resolver : public ntci::Executor,
                 public ntci::StrandFactory,
                 public ntci::TimerFactory,
                 public ntci::GetIpAddressCallbackFactory,
                 public ntci::GetDomainNameCallbackFactory,
                 public ntci::GetPortCallbackFactory,
                 public ntci::GetServiceNameCallbackFactory,
                 public ntci::GetEndpointCallbackFactory
{
  public:
    /// Destroy this object.
    virtual ~Resolver();

    /// Start the resolver. Return the error.
    virtual ntsa::Error start() = 0;

    /// Begin stopping the resolver.
    virtual void shutdown() = 0;

    /// Wait until the resolver has stopped.
    virtual void linger() = 0;

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
        const bsl::vector<ntsa::IpAddress>& ipAddressList) = 0;

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
        const bsl::vector<ntsa::IpAddress>& ipAddressList) = 0;

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
                                     const ntsa::IpAddress&   ipAddress) = 0;

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
                                ntsa::Transport::Value         transport) = 0;

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
                                ntsa::Transport::Value         transport) = 0;

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
                                ntsa::Transport::Value   transport) = 0;

    /// Set the local IP addresses assigned to the local machine to the
    /// specified 'ipAddressList'. Return the error. Note that calling this
    /// function affects the future behavior of this object only:
    /// 'ipAddressList' will be subsequently returned from calling
    /// 'getLocalIpAddress()' but this function does not set the local IP
    /// addresses of the system or have any wider effect on other objects or
    /// name resolution functionality in this process.
    virtual ntsa::Error setLocalIpAddress(
        const bsl::vector<ntsa::IpAddress>& ipAddressList) = 0;

    /// Set the hostname of the local machine to the specified 'name'.
    /// Return the error. Note that calling this function affects the future
    /// behavior of this object only: 'name' will be subsequently returned
    /// from calling 'getHostname()' but this function does not set the
    /// hostname of the system or have any wider effect on other objects or
    /// name resolution functionality in this process.
    virtual ntsa::Error setHostname(const bsl::string& name) = 0;

    /// Set the canonical, fully-qualified hostname of the local machine to
    /// the specified 'name'. Return the error. Note that calling this
    /// function affects the future behavior of this object only: 'name'
    /// will be subsequently returned from calling
    /// 'getHostnameFullyQualified()' but this function does not set the
    /// hostname of the system or have any wider effect on other objects or
    /// name resolution functionality in this process.
    virtual ntsa::Error setHostnameFullyQualified(const bsl::string& name) = 0;

    /// Resolve the specified 'domainName' to the IP addresses assigned to
    /// the 'domainName', according to the specified 'options'. When
    /// resolution completes or fails, invoke the specified 'callback' on
    /// the callback's strand, if any, with the IP addresses assigned to
    /// the 'domainName'. Return the error.
    virtual ntsa::Error getIpAddress(
        const bslstl::StringRef&          domainName,
        const ntca::GetIpAddressOptions&  options,
        const ntci::GetIpAddressCallback& callback) = 0;

    /// Resolve the specified 'ipAddress' to the domain name to which the
    /// 'ipAddress' has been assigned, according to the specified 'options'.
    /// When resolution completes or fails, invoke the specified 'callback'
    /// on the callback's strand, if any, with the domain name to which the
    /// 'ipAddress' has been assigned. Return the error.
    virtual ntsa::Error getDomainName(
        const ntsa::IpAddress&             ipAddress,
        const ntca::GetDomainNameOptions&  options,
        const ntci::GetDomainNameCallback& callback) = 0;

    /// Resolve the specified 'serviceName' to the ports assigned to the
    /// 'serviceName', according to the specified 'options'. When resolution
    /// completes or fails, invoke the specified 'callback' on the
    /// callback's strand, if any, with the ports assigned to the
    /// 'serviceName'. Return the error.
    virtual ntsa::Error getPort(const bslstl::StringRef&     serviceName,
                                const ntca::GetPortOptions&  options,
                                const ntci::GetPortCallback& callback) = 0;

    /// Resolve the specified 'port' to the service name to which the 'port'
    /// has been assigned, according to the specified 'options'. When
    /// resolution completes or fails, invoke the specified 'callback' on
    /// the callback's strand, if any, with the service name to which the
    /// 'port' has been assigned. Return the error.
    virtual ntsa::Error getServiceName(
        ntsa::Port                          port,
        const ntca::GetServiceNameOptions&  options,
        const ntci::GetServiceNameCallback& callback) = 0;

    /// Parse and potentially resolve the components of the specified
    /// 'text', in the format of '<port>' or '[<host>][:<port>]'. If the
    /// optionally specified '<host>' component is not an IP address,
    /// interpret the '<host>' as a domain name and resolve it into an IP
    /// address. If the optionally specified '<port>' is a name and not a
    /// number, interpret the '<port>' as a service name and resolve it into
    /// a port. Perform all resolution and validation of the characteristics
    /// of the desired 'result' according to the specified 'options'. When
    /// resolution completes or fails, invoke the specified 'callback' on
    /// the callback's strand, if any, with the endpoint to which the
    /// components of the 'text' resolve. Return the error.
    virtual ntsa::Error getEndpoint(
        const bslstl::StringRef&         text,
        const ntca::GetEndpointOptions&  options,
        const ntci::GetEndpointCallback& callback) = 0;

    /// Load into the specified 'result' the IP addresses assigned to the
    /// local machine. Perform all resolution and validation of the
    /// characteristics of the desired 'result' according to the specified
    /// 'options'. Return the error.
    virtual ntsa::Error getLocalIpAddress(
        bsl::vector<ntsa::IpAddress>* result,
        const ntsa::IpAddressOptions& options) = 0;

    /// Return the hostname of the local machine.
    virtual ntsa::Error getHostname(bsl::string* result) = 0;

    /// Return the canonical, fully-qualified hostname of the local machine.
    virtual ntsa::Error getHostnameFullyQualified(bsl::string* result) = 0;
};

}  // close package namespace
}  // close enterprise namespace
#endif
