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

#include <ntsi_resolver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsi_resolver_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntsi {

Resolver::~Resolver()
{
}

ntsa::Error Resolver::setIpAddress(
    const bslstl::StringRef&            name,
    const bsl::vector<ntsa::IpAddress>& addressList)
{
    NTSCFG_WARNING_UNUSED(name);
    NTSCFG_WARNING_UNUSED(addressList);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Resolver::addIpAddress(
    const bslstl::StringRef&            domainName,
    const bsl::vector<ntsa::IpAddress>& ipAddressList)
{
    NTSCFG_WARNING_UNUSED(domainName);
    NTSCFG_WARNING_UNUSED(ipAddressList);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Resolver::addIpAddress(const bslstl::StringRef& domainName,
                                   const ntsa::IpAddress&   ipAddress)
{
    NTSCFG_WARNING_UNUSED(domainName);
    NTSCFG_WARNING_UNUSED(ipAddress);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Resolver::setPort(const bslstl::StringRef&       serviceName,
                              const bsl::vector<ntsa::Port>& portList,
                              ntsa::Transport::Value         transport)
{
    NTSCFG_WARNING_UNUSED(serviceName);
    NTSCFG_WARNING_UNUSED(portList);
    NTSCFG_WARNING_UNUSED(transport);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Resolver::addPort(const bslstl::StringRef&       serviceName,
                              const bsl::vector<ntsa::Port>& portList,
                              ntsa::Transport::Value         transport)
{
    NTSCFG_WARNING_UNUSED(serviceName);
    NTSCFG_WARNING_UNUSED(portList);
    NTSCFG_WARNING_UNUSED(transport);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Resolver::addPort(const bslstl::StringRef& serviceName,
                              ntsa::Port               port,
                              ntsa::Transport::Value   transport)
{
    NTSCFG_WARNING_UNUSED(serviceName);
    NTSCFG_WARNING_UNUSED(port);
    NTSCFG_WARNING_UNUSED(transport);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Resolver::setLocalIpAddress(
    const bsl::vector<ntsa::IpAddress>& ipAddressList)
{
    NTSCFG_WARNING_UNUSED(ipAddressList);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Resolver::setHostname(const bsl::string& name)
{
    NTSCFG_WARNING_UNUSED(name);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Resolver::setHostnameFullyQualified(const bsl::string& name)
{
    NTSCFG_WARNING_UNUSED(name);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Resolver::getIpAddress(bsl::vector<ntsa::IpAddress>* result,
                                   const bslstl::StringRef&      domainName,
                                   const ntsa::IpAddressOptions& options)
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_WARNING_UNUSED(domainName);
    NTSCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Resolver::getDomainName(bsl::string*           result,
                                    const ntsa::IpAddress& ipAddress)
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_WARNING_UNUSED(ipAddress);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Resolver::getPort(bsl::vector<ntsa::Port>* result,
                              const bslstl::StringRef& serviceName,
                              const ntsa::PortOptions& options)
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_WARNING_UNUSED(serviceName);
    NTSCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Resolver::getServiceName(bsl::string*           result,
                                     ntsa::Port             port,
                                     ntsa::Transport::Value transport)
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_WARNING_UNUSED(port);
    NTSCFG_WARNING_UNUSED(transport);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Resolver::getEndpoint(ntsa::Endpoint*              result,
                                  const bslstl::StringRef&     text,
                                  const ntsa::EndpointOptions& options)
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_WARNING_UNUSED(text);
    NTSCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Resolver::getEndpoint(bsl::vector<ntsa::Endpoint>* result,
                                  const bslstl::StringRef&     text,
                                  const ntsa::EndpointOptions& options)
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_WARNING_UNUSED(text);
    NTSCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Resolver::getLocalIpAddress(bsl::vector<ntsa::IpAddress>* result,
                                        const ntsa::IpAddressOptions& options)
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Resolver::getHostname(bsl::string* result)
{
    NTSCFG_WARNING_UNUSED(result);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Resolver::getHostnameFullyQualified(bsl::string* result)
{
    NTSCFG_WARNING_UNUSED(result);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

}  // close package namespace
}  // close enterprise namespace
