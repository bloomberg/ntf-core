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

#include <ntsb_resolver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsb_resolver_cpp, "$Id$ $CSID$")

#include <ntsu_resolverutil.h>

#include <bdlb_chartype.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntsb {

ResolverOverrides::ResolverOverrides(bslma::Allocator* basicAllocator)
: d_mutex()
, d_ipAddressByDomainName(basicAllocator)
, d_domainNameByIpAddress(basicAllocator)
, d_tcpPortByServiceName(basicAllocator)
, d_tcpServiceNameByPort(basicAllocator)
, d_udpPortByServiceName(basicAllocator)
, d_udpServiceNameByPort(basicAllocator)
, d_localIpAddressList(basicAllocator)
, d_hostname(basicAllocator)
, d_hostnameFullyQualified(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ResolverOverrides::~ResolverOverrides()
{
}

ntsa::Error ResolverOverrides::setIpAddress(
    const bslstl::StringRef&            domainName,
    const bsl::vector<ntsa::IpAddress>& ipAddressList)
{
    ntscfg::LockGuard lock(&d_mutex);

    IpAddressVector& target = d_ipAddressByDomainName[domainName];

    for (IpAddressVector::iterator it = target.begin(); it != target.end();
         ++it)
    {
        const ntsa::IpAddress& ipAddress = *it;
        d_domainNameByIpAddress.erase(ipAddress);
    }

    target.clear();

    for (bsl::vector<ntsa::IpAddress>::const_iterator it =
             ipAddressList.begin();
         it != ipAddressList.end();
         ++it)
    {
        const ntsa::IpAddress& ipAddress = *it;
        target.push_back(ipAddress);
        d_domainNameByIpAddress[ipAddress] = domainName;
    }

    return ntsa::Error();
}

ntsa::Error ResolverOverrides::addIpAddress(
    const bslstl::StringRef&            domainName,
    const bsl::vector<ntsa::IpAddress>& ipAddressList)
{
    ntscfg::LockGuard lock(&d_mutex);

    IpAddressVector& target = d_ipAddressByDomainName[domainName];

    for (bsl::vector<ntsa::IpAddress>::const_iterator it =
             ipAddressList.begin();
         it != ipAddressList.end();
         ++it)
    {
        const ntsa::IpAddress& ipAddress = *it;
        target.push_back(ipAddress);
        d_domainNameByIpAddress[ipAddress] = domainName;
    }

    return ntsa::Error();
}

ntsa::Error ResolverOverrides::addIpAddress(
    const bslstl::StringRef& domainName,
    const ntsa::IpAddress&   ipAddress)
{
    ntscfg::LockGuard lock(&d_mutex);

    IpAddressVector& target = d_ipAddressByDomainName[domainName];
    target.push_back(ipAddress);
    d_domainNameByIpAddress[ipAddress] = domainName;

    return ntsa::Error();
}

ntsa::Error ResolverOverrides::setPort(const bslstl::StringRef& serviceName,
                                       const bsl::vector<ntsa::Port>& portList,
                                       ntsa::Transport::Value transport)
{
    ntscfg::LockGuard lock(&d_mutex);

    if (transport == ntsa::Transport::e_TCP_IPV4_STREAM ||
        transport == ntsa::Transport::e_TCP_IPV6_STREAM)
    {
        PortVector& target = d_tcpPortByServiceName[serviceName];

        for (PortVector::iterator it = target.begin(); it != target.end();
             ++it)
        {
            ntsa::Port port = *it;
            d_tcpServiceNameByPort.erase(port);
        }

        target.clear();

        for (bsl::vector<ntsa::Port>::const_iterator it = portList.begin();
             it != portList.end();
             ++it)
        {
            ntsa::Port port = *it;
            target.push_back(port);
            d_tcpServiceNameByPort[port] = serviceName;
        }
    }
    else if (transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM ||
             transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM)
    {
        PortVector& target = d_udpPortByServiceName[serviceName];

        for (PortVector::iterator it = target.begin(); it != target.end();
             ++it)
        {
            ntsa::Port port = *it;
            d_udpServiceNameByPort.erase(port);
        }

        target.clear();

        for (bsl::vector<ntsa::Port>::const_iterator it = portList.begin();
             it != portList.end();
             ++it)
        {
            ntsa::Port port = *it;
            target.push_back(port);
            d_udpServiceNameByPort[port] = serviceName;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error ResolverOverrides::addPort(const bslstl::StringRef& serviceName,
                                       const bsl::vector<ntsa::Port>& portList,
                                       ntsa::Transport::Value transport)
{
    ntscfg::LockGuard lock(&d_mutex);

    if (transport == ntsa::Transport::e_TCP_IPV4_STREAM ||
        transport == ntsa::Transport::e_TCP_IPV6_STREAM)
    {
        PortVector& target = d_tcpPortByServiceName[serviceName];

        for (bsl::vector<ntsa::Port>::const_iterator it = portList.begin();
             it != portList.end();
             ++it)
        {
            ntsa::Port port = *it;
            target.push_back(port);
            d_tcpServiceNameByPort[port] = serviceName;
        }
    }
    else if (transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM ||
             transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM)
    {
        PortVector& target = d_udpPortByServiceName[serviceName];

        for (bsl::vector<ntsa::Port>::const_iterator it = portList.begin();
             it != portList.end();
             ++it)
        {
            ntsa::Port port = *it;
            target.push_back(port);
            d_udpServiceNameByPort[port] = serviceName;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error ResolverOverrides::addPort(const bslstl::StringRef& serviceName,
                                       ntsa::Port               port,
                                       ntsa::Transport::Value   transport)
{
    ntscfg::LockGuard lock(&d_mutex);

    if (transport == ntsa::Transport::e_TCP_IPV4_STREAM ||
        transport == ntsa::Transport::e_TCP_IPV6_STREAM)
    {
        PortVector& target = d_tcpPortByServiceName[serviceName];
        target.push_back(port);
        d_tcpServiceNameByPort[port] = serviceName;
    }
    else if (transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM ||
             transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM)
    {
        PortVector& target = d_udpPortByServiceName[serviceName];
        target.push_back(port);
        d_tcpServiceNameByPort[port] = serviceName;
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error ResolverOverrides::setLocalIpAddress(
    const bsl::vector<ntsa::IpAddress>& ipAddressList)
{
    ntscfg::LockGuard lock(&d_mutex);
    d_localIpAddressList = ipAddressList;
    return ntsa::Error();
}

ntsa::Error ResolverOverrides::setHostname(const bsl::string& name)
{
    ntscfg::LockGuard lock(&d_mutex);
    d_hostname = name;
    return ntsa::Error();
}

ntsa::Error ResolverOverrides::setHostnameFullyQualified(
    const bsl::string& name)
{
    ntscfg::LockGuard lock(&d_mutex);
    d_hostnameFullyQualified = name;
    return ntsa::Error();
}

ntsa::Error ResolverOverrides::getIpAddress(
    bsl::vector<ntsa::IpAddress>* result,
    const bslstl::StringRef&      domainName,
    const ntsa::IpAddressOptions& options) const
{
    result->clear();

    ntsa::Error                  error;
    bsl::vector<ntsa::IpAddress> ipAddressList;

    bdlb::NullableValue<ntsa::IpAddressType::Value> ipAddressType;
    error = ntsu::ResolverUtil::classifyIpAddressType(&ipAddressType, options);
    if (error) {
        return error;
    }

    {
        ntscfg::LockGuard lock(&d_mutex);

        IpAddressByDomainName::const_iterator it =
            d_ipAddressByDomainName.find(domainName);

        if (it == d_ipAddressByDomainName.end()) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }

        if (ipAddressType.isNull()) {
            ipAddressList = it->second;
            ntsu::ResolverUtil::sortIpAddressList(&ipAddressList);
        }
        else {
            const IpAddressVector& target = it->second;

            for (IpAddressVector::const_iterator jt = target.begin();
                 jt != target.end();
                 ++jt)
            {
                const ntsa::IpAddress& ipAddress = *jt;
                if (ipAddress.type() == ipAddressType.value()) {
                    ipAddressList.push_back(ipAddress);
                }
            }
        }
    }

    if (ipAddressList.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    if (options.ipAddressSelector().isNull()) {
        *result = ipAddressList;
    }
    else {
        result->push_back(ipAddressList[options.ipAddressSelector().value() %
                                        ipAddressList.size()]);
    }

    return ntsa::Error();
}

ntsa::Error ResolverOverrides::getDomainName(
    bsl::string*           result,
    const ntsa::IpAddress& ipAddress) const
{
    ntscfg::LockGuard lock(&d_mutex);

    DomainNameByIpAddress::const_iterator it =
        d_domainNameByIpAddress.find(ipAddress);

    if (it == d_domainNameByIpAddress.end()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    if (!it->second.empty()) {
        *result = it->second;
    }
    else {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    return ntsa::Error();
}

ntsa::Error ResolverOverrides::getPort(bsl::vector<ntsa::Port>* result,
                                       const bslstl::StringRef& serviceName,
                                       const ntsa::PortOptions& options) const
{
    result->clear();

    bsl::vector<ntsa::Port> portList;

    bool examineTcpPortList = false;
    bool examineUdpPortList = false;

    if (options.transport().isNull()) {
        examineTcpPortList = true;
        examineUdpPortList = true;
    }
    else {
        if (options.transport().value() ==
                ntsa::Transport::e_TCP_IPV4_STREAM ||
            options.transport().value() == ntsa::Transport::e_TCP_IPV6_STREAM)
        {
            examineTcpPortList = true;
        }
        else if (options.transport().value() ==
                     ntsa::Transport::e_UDP_IPV4_DATAGRAM ||
                 options.transport().value() ==
                     ntsa::Transport::e_UDP_IPV6_DATAGRAM)
        {
            examineUdpPortList = true;
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    {
        ntscfg::LockGuard lock(&d_mutex);

        if (examineTcpPortList) {
            PortByServiceName::const_iterator it =
                d_tcpPortByServiceName.find(serviceName);

            if (it != d_tcpPortByServiceName.end()) {
                portList.insert(portList.end(),
                                it->second.begin(),
                                it->second.end());
            }
        }

        if (examineUdpPortList) {
            PortByServiceName::const_iterator it =
                d_udpPortByServiceName.find(serviceName);

            if (it != d_udpPortByServiceName.end()) {
                portList.insert(portList.end(),
                                it->second.begin(),
                                it->second.end());
            }
        }
    }

    if (portList.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    if (options.portSelector().isNull()) {
        *result = portList;
    }
    else {
        result->push_back(
            portList[options.portSelector().value() % portList.size()]);
    }

    return ntsa::Error();
}

ntsa::Error ResolverOverrides::getServiceName(
    bsl::string*           result,
    ntsa::Port             port,
    ntsa::Transport::Value transport) const
{
    ntscfg::LockGuard lock(&d_mutex);

    bool found = false;

    if (transport == ntsa::Transport::e_TCP_IPV4_STREAM ||
        transport == ntsa::Transport::e_TCP_IPV6_STREAM)
    {
        ServiceNameByPort::const_iterator it =
            d_tcpServiceNameByPort.find(port);

        if (it == d_tcpServiceNameByPort.end()) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }

        if (!it->second.empty()) {
            *result = it->second;
            found   = true;
        }
    }
    else if (transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM ||
             transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM)
    {
        ServiceNameByPort::const_iterator it =
            d_udpServiceNameByPort.find(port);

        if (it == d_udpServiceNameByPort.end()) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }

        if (!it->second.empty()) {
            *result = it->second;
            found   = true;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!found) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    return ntsa::Error();
}

ntsa::Error ResolverOverrides::getLocalIpAddress(
    bsl::vector<ntsa::IpAddress>* result,
    const ntsa::IpAddressOptions& options) const
{
    result->clear();

    ntsa::Error                  error;
    bsl::vector<ntsa::IpAddress> ipAddressList;

    bdlb::NullableValue<ntsa::IpAddressType::Value> ipAddressType;
    error = ntsu::ResolverUtil::classifyIpAddressType(&ipAddressType, options);
    if (error) {
        return error;
    }

    {
        ntscfg::LockGuard lock(&d_mutex);

        if (ipAddressType.isNull()) {
            ipAddressList = d_localIpAddressList;
            ntsu::ResolverUtil::sortIpAddressList(&ipAddressList);
        }
        else {
            for (IpAddressVector::const_iterator it =
                     d_localIpAddressList.begin();
                 it != d_localIpAddressList.end();
                 ++it)
            {
                const ntsa::IpAddress& ipAddress = *it;
                if (ipAddress.type() == ipAddressType.value()) {
                    ipAddressList.push_back(ipAddress);
                }
            }
        }
    }

    if (ipAddressList.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    if (options.ipAddressSelector().isNull()) {
        *result = ipAddressList;
    }
    else {
        result->push_back(ipAddressList[options.ipAddressSelector().value() %
                                        ipAddressList.size()]);
    }

    return ntsa::Error();
}

ntsa::Error ResolverOverrides::getHostname(bsl::string* result) const
{
    ntscfg::LockGuard lock(&d_mutex);

    if (d_hostname.isNull()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    *result = d_hostname.value();
    return ntsa::Error();
}

ntsa::Error ResolverOverrides::getHostnameFullyQualified(
    bsl::string* result) const
{
    ntscfg::LockGuard lock(&d_mutex);

    if (d_hostnameFullyQualified.isNull()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    *result = d_hostnameFullyQualified.value();
    return ntsa::Error();
}

Resolver::Resolver(bslma::Allocator* basicAllocator)
: d_overrides(basicAllocator)
, d_overridesExist(false)
, d_overridesEnabled(true)
, d_systemEnabled(true)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Resolver::Resolver(const ntsa::ResolverConfig& configuration,
                   bslma::Allocator*           basicAllocator)
: d_overrides(basicAllocator)
, d_overridesExist(false)
, d_overridesEnabled(configuration.overridesEnabled().valueOr(true))
, d_systemEnabled(configuration.overridesEnabled().valueOr(true))
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Resolver::~Resolver()
{
}

ntsa::Error Resolver::setIpAddress(
    const bslstl::StringRef&            domainName,
    const bsl::vector<ntsa::IpAddress>& ipAddressList)
{
    ntsa::Error error = d_overrides.setIpAddress(domainName, ipAddressList);
    if (error) {
        return error;
    }

    d_overridesExist = true;
    return ntsa::Error();
}

ntsa::Error Resolver::addIpAddress(
    const bslstl::StringRef&            domainName,
    const bsl::vector<ntsa::IpAddress>& ipAddressList)
{
    ntsa::Error error = d_overrides.addIpAddress(domainName, ipAddressList);
    if (error) {
        return error;
    }

    d_overridesExist = true;
    return ntsa::Error();
}

ntsa::Error Resolver::addIpAddress(const bslstl::StringRef& domainName,
                                   const ntsa::IpAddress&   ipAddress)
{
    ntsa::Error error = d_overrides.addIpAddress(domainName, ipAddress);
    if (error) {
        return error;
    }

    d_overridesExist = true;
    return ntsa::Error();
}

ntsa::Error Resolver::setPort(const bslstl::StringRef&       serviceName,
                              const bsl::vector<ntsa::Port>& portList,
                              ntsa::Transport::Value         transport)
{
    ntsa::Error error = d_overrides.setPort(serviceName, portList, transport);
    if (error) {
        return error;
    }

    d_overridesExist = true;
    return ntsa::Error();
}

ntsa::Error Resolver::addPort(const bslstl::StringRef&       serviceName,
                              const bsl::vector<ntsa::Port>& portList,
                              ntsa::Transport::Value         transport)
{
    ntsa::Error error = d_overrides.addPort(serviceName, portList, transport);
    if (error) {
        return error;
    }

    d_overridesExist = true;
    return ntsa::Error();
}

ntsa::Error Resolver::addPort(const bslstl::StringRef& serviceName,
                              ntsa::Port               port,
                              ntsa::Transport::Value   transport)
{
    ntsa::Error error = d_overrides.addPort(serviceName, port, transport);
    if (error) {
        return error;
    }

    d_overridesExist = true;
    return ntsa::Error();
}

ntsa::Error Resolver::setLocalIpAddress(
    const bsl::vector<ntsa::IpAddress>& ipAddressList)
{
    ntsa::Error error = d_overrides.setLocalIpAddress(ipAddressList);
    if (error) {
        return error;
    }

    d_overridesExist = true;
    return ntsa::Error();
}

ntsa::Error Resolver::setHostname(const bsl::string& name)
{
    ntsa::Error error = d_overrides.setHostname(name);
    if (error) {
        return error;
    }

    d_overridesExist = true;
    return ntsa::Error();
}

ntsa::Error Resolver::setHostnameFullyQualified(const bsl::string& name)
{
    ntsa::Error error = d_overrides.setHostnameFullyQualified(name);
    if (error) {
        return error;
    }

    d_overridesExist = true;
    return ntsa::Error();
}

ntsa::Error Resolver::getIpAddress(bsl::vector<ntsa::IpAddress>* result,
                                   const bslstl::StringRef&      domainName,
                                   const ntsa::IpAddressOptions& options)
{
    if (d_overridesEnabled && d_overridesExist) {
        ntsa::Error error =
            d_overrides.getIpAddress(result, domainName, options);
        if (!error) {
            return ntsa::Error();
        }
    }

    if (d_systemEnabled) {
        return ntsu::ResolverUtil::getIpAddress(result, domainName, options);
    }

    return ntsa::Error(ntsa::Error::e_EOF);
}

ntsa::Error Resolver::getDomainName(bsl::string*           result,
                                    const ntsa::IpAddress& ipAddress)
{
    if (d_overridesEnabled && d_overridesExist) {
        ntsa::Error error = d_overrides.getDomainName(result, ipAddress);
        if (!error) {
            return ntsa::Error();
        }
    }

    if (d_systemEnabled) {
        return ntsu::ResolverUtil::getDomainName(result, ipAddress);
    }

    return ntsa::Error(ntsa::Error::e_EOF);
}

ntsa::Error Resolver::getPort(bsl::vector<ntsa::Port>* result,
                              const bslstl::StringRef& serviceName,
                              const ntsa::PortOptions& options)
{
    if (d_overridesEnabled && d_overridesExist) {
        ntsa::Error error = d_overrides.getPort(result, serviceName, options);
        if (!error) {
            return ntsa::Error();
        }
    }

    if (d_systemEnabled) {
        return ntsu::ResolverUtil::getPort(result, serviceName, options);
    }

    return ntsa::Error(ntsa::Error::e_EOF);
}

ntsa::Error Resolver::getServiceName(bsl::string*           result,
                                     ntsa::Port             port,
                                     ntsa::Transport::Value transport)
{
    if (d_overridesEnabled && d_overridesExist) {
        ntsa::Error error =
            d_overrides.getServiceName(result, port, transport);
        if (!error) {
            return ntsa::Error();
        }
    }

    if (d_systemEnabled) {
        return ntsu::ResolverUtil::getServiceName(result, port, transport);
    }

    return ntsa::Error(ntsa::Error::e_EOF);
}

ntsa::Error Resolver::getEndpoint(ntsa::Endpoint*              result,
                                  const bslstl::StringRef&     text,
                                  const ntsa::EndpointOptions& options)
{
    ntsa::Error error;

    ntsa::IpAddress                 ipAddress;
    bdlb::NullableValue<ntsa::Port> port;

    bslstl::StringRef unresolvedDomainName;
    bslstl::StringRef unresolvedPort;

    result->reset();

    if (!options.transport().isNull()) {
        if (options.transport().value() == ntsa::Transport::e_LOCAL_STREAM ||
            options.transport().value() == ntsa::Transport::e_LOCAL_DATAGRAM)
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    const char* begin = text.begin();
    const char* end   = text.end();

    if (begin != end) {
        bool isNumber = true;
        {
            for (const char* current = begin; current < end; ++current) {
                if (!bdlb::CharType::isDigit(*current)) {
                    isNumber = false;
                    break;
                }
            }
        }

        if (isNumber) {
            if (!ntsa::PortUtil::parse(&port.makeValue(), begin, end - begin))
            {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
        else {
            if (*begin == '[') {
                const char* mark = end - 1;
                while (mark > begin) {
                    if (*mark == ']') {
                        break;
                    }

                    --mark;
                }

                if (mark == begin) {
                    return ntsa::Error(ntsa::Error::e_INVALID);  // missing ']'
                }

                const char* ipv6AddressBegin = begin + 1;
                const char* ipv6AddressEnd   = mark;

                ntsa::Ipv6Address ipv6Address;
                if (ipv6Address.parse(
                        bslstl::StringRef(ipv6AddressBegin, ipv6AddressEnd)))
                {
                    ipAddress = ipv6Address;
                }
                else {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }

                const char* colon = mark + 1;

                if (colon >= end) {
                    return ntsa::Error(ntsa::Error::e_INVALID);  // missing ':'
                }

                if (*colon != ':') {
                    return ntsa::Error(ntsa::Error::e_INVALID);  // missing ':'
                }

                const char* portBegin = colon + 1;
                const char* portEnd   = end;

                if (portBegin >= portEnd) {
                    return ntsa::Error(
                        ntsa::Error::e_INVALID);  // missing port
                }

                if (!ntsa::PortUtil::parse(&port.makeValue(),
                                           portBegin,
                                           portEnd - portBegin))
                {
                    port.reset();
                    unresolvedPort.assign(portBegin, portEnd);
                }
            }
            else {
                const char* current   = end - 1;
                const char* mark      = 0;
                bsl::size_t numColons = 0;
                while (current > begin) {
                    if (*current == ':') {
                        if (mark == 0) {
                            mark = current;
                        }
                        ++numColons;
                        if (numColons > 1) {
                            break;
                        }
                    }

                    --current;
                }

                if (numColons == 0) {
                    // <ip-address-or-host> (missing ':' therefore no port)

                    const char* ipAddressBegin = begin;
                    const char* ipAddressEnd   = end;

                    if (!ipAddress.parse(
                            bslstl::StringRef(ipAddressBegin, ipAddressEnd)))
                    {
                        unresolvedDomainName.assign(ipAddressBegin,
                                                    ipAddressEnd);
                    }
                }
                else if (numColons == 1) {
                    // <ipv4-address-or-host>:<port>

                    const char* ipv4AddressBegin = begin;
                    const char* ipv4AddressEnd   = mark;

                    ntsa::Ipv4Address ipv4Address;
                    if (ipv4Address.parse(bslstl::StringRef(ipv4AddressBegin,
                                                            ipv4AddressEnd)))
                    {
                        ipAddress = ipv4Address;
                    }
                    else {
                        unresolvedDomainName.assign(ipv4AddressBegin,
                                                    ipv4AddressEnd);
                    }

                    const char* portBegin = mark + 1;
                    const char* portEnd   = end;

                    if (portBegin >= portEnd) {
                        return ntsa::Error(
                            ntsa::Error::e_INVALID);  // missing port
                    }

                    if (!ntsa::PortUtil::parse(&port.makeValue(),
                                               portBegin,
                                               portEnd - portBegin))
                    {
                        port.reset();
                        unresolvedPort.assign(portBegin, portEnd);
                    }
                }
                else {
                    // <ipv6-address> (more than one ':' found)

                    const char* ipv6AddressBegin = begin;
                    const char* ipv6AddressEnd   = end;

                    ntsa::Ipv6Address ipv6Address;
                    if (ipv6Address.parse(bslstl::StringRef(ipv6AddressBegin,
                                                            ipv6AddressEnd)))
                    {
                        ipAddress = ipv6Address;
                    }
                    else {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }
            }
        }
    }

    if (!unresolvedPort.empty()) {
        bsl::vector<ntsa::Port> portList;
        ntsa::PortOptions       portOptions;

        if (!options.portSelector().isNull()) {
            portOptions.setPortSelector(options.portSelector().value());
        }

        if (!options.transport().isNull()) {
            portOptions.setTransport(options.transport().value());
        }

        error = this->getPort(&portList, unresolvedPort, portOptions);
        if (error) {
            return error;
        }

        if (portList.empty()) {
            return ntsa::Error(ntsa::Error::e_EOF);  // unresolvable port
        }

        port = portList.front();
    }
    else if (port.isNull()) {
        if (!options.portFallback().isNull()) {
            port = options.portFallback().value();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (!unresolvedDomainName.empty()) {
        bsl::vector<ntsa::IpAddress> ipAddressList;
        ntsa::IpAddressOptions       ipAddressOptions;

        if (!options.ipAddressType().isNull()) {
            ipAddressOptions.setIpAddressType(options.ipAddressType().value());
        }

        if (!options.ipAddressSelector().isNull()) {
            ipAddressOptions.setIpAddressSelector(
                options.ipAddressSelector().value());
        }

        if (!options.transport().isNull()) {
            ipAddressOptions.setTransport(options.transport().value());
        }

        error = this->getIpAddress(&ipAddressList,
                                   unresolvedDomainName,
                                   ipAddressOptions);
        if (error) {
            return error;
        }

        if (ipAddressList.empty()) {
            return ntsa::Error(ntsa::Error::e_EOF);  // unresolvable host
        }

        ipAddress = ipAddressList.front();
    }
    else if (ipAddress.isUndefined()) {
        if (!options.ipAddressFallback().isNull()) {
            ipAddress = options.ipAddressFallback().value();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (ipAddress.isUndefined()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (port.isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!options.ipAddressType().isNull()) {
        if (ipAddress.type() != options.ipAddressType().value()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (!options.transport().isNull()) {
        if (options.transport().value() ==
                ntsa::Transport::e_TCP_IPV4_STREAM ||
            options.transport().value() ==
                ntsa::Transport::e_UDP_IPV4_DATAGRAM)
        {
            if (!ipAddress.isV4()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
        else if (options.transport().value() ==
                     ntsa::Transport::e_TCP_IPV6_STREAM ||
                 options.transport().value() ==
                     ntsa::Transport::e_UDP_IPV6_DATAGRAM)
        {
            if (!ipAddress.isV6()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    *result = ntsa::Endpoint(ntsa::IpEndpoint(ipAddress, port.value()));

    return ntsa::Error();
}

ntsa::Error Resolver::getLocalIpAddress(bsl::vector<ntsa::IpAddress>* result,
                                        const ntsa::IpAddressOptions& options)
{
    if (d_overridesEnabled && d_overridesExist) {
        ntsa::Error error = d_overrides.getLocalIpAddress(result, options);
        if (!error) {
            return ntsa::Error();
        }
    }

    if (d_systemEnabled) {
        return ntsu::ResolverUtil::getLocalIpAddress(result, options);
    }

    return ntsa::Error(ntsa::Error::e_EOF);
}

ntsa::Error Resolver::getHostname(bsl::string* result)
{
    if (d_overridesEnabled && d_overridesExist) {
        ntsa::Error error = d_overrides.getHostname(result);
        if (!error) {
            return ntsa::Error();
        }
    }

    if (d_systemEnabled) {
        return ntsu::ResolverUtil::getHostname(result);
    }

    return ntsa::Error(ntsa::Error::e_EOF);
}

ntsa::Error Resolver::getHostnameFullyQualified(bsl::string* result)
{
    if (d_overridesEnabled && d_overridesExist) {
        ntsa::Error error = d_overrides.getHostnameFullyQualified(result);
        if (!error) {
            return ntsa::Error();
        }
    }

    if (d_systemEnabled) {
        return ntsu::ResolverUtil::getHostnameFullyQualified(result);
    }

    return ntsa::Error(ntsa::Error::e_EOF);
}

}  // close package namespace
}  // close enterprise namespace
