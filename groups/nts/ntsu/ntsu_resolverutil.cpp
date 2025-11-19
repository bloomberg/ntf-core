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

#include <ntsu_resolverutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsu_resolverutil_cpp, "$Id$ $CSID$")

#include <ntsu_adapterutil.h>
#include <ntsu_socketutil.h>

#include <bdlb_chartype.h>
#include <bdlb_numericparseutil.h>
#include <bdlb_stringviewutil.h>
#include <bdlma_bufferedsequentialallocator.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
// clang-format off
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
// clang-format on
#pragma comment(lib, "iphlpapi.lib")
#endif

#include <bsl_algorithm.h>
#include <bsl_climits.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_functional.h>

namespace BloombergLP {
namespace ntsu {

/// Provide a private implementation.
class ResolverUtil::Impl
{
  public:
    /// Provide a guard to automatically free address information.
    class AddrInfoGuard;

    /// Return the effective error from the specified 'rc'.
    static ntsa::Error convertGetAddrInfoError(int rc);
};

/// Provide a guard to automatically free address information.
class ResolverUtil::Impl::AddrInfoGuard
{
    struct addrinfo* d_info_p;

  private:
    AddrInfoGuard(const AddrInfoGuard&) BSLS_KEYWORD_DELETED;
    AddrInfoGuard& operator=(const AddrInfoGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new guard that automatically releases resources for the
    /// specified 'info'.
    explicit AddrInfoGuard(struct addrinfo* info);

    /// Destroy this object.
    ~AddrInfoGuard();
};

ResolverUtil::Impl::AddrInfoGuard::AddrInfoGuard(struct addrinfo* info)
: d_info_p(info)
{
}

ResolverUtil::Impl::AddrInfoGuard::~AddrInfoGuard()
{
    freeaddrinfo(d_info_p);
}

ntsa::Error ResolverUtil::Impl::convertGetAddrInfoError(int rc)
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    switch (rc) {
    case 0:
        return ntsa::Error();
#if defined(EAI_ADDRFAMILY)
    case EAI_ADDRFAMILY:
        return ntsa::Error(ntsa::Error::e_EOF);
#endif
    case EAI_AGAIN:
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    case EAI_BADFLAGS:
        return ntsa::Error(ntsa::Error::e_INVALID);
    case EAI_FAIL:
        return ntsa::Error(ntsa::Error::e_INVALID);
    case EAI_FAMILY:
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    case EAI_MEMORY:
        return ntsa::Error(ntsa::Error::e_LIMIT);
#if defined(EAI_NODATA)
    case EAI_NODATA:
        return ntsa::Error(ntsa::Error::e_EOF);
#endif
    case EAI_NONAME:
        return ntsa::Error(ntsa::Error::e_EOF);
    case EAI_SERVICE:
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    case EAI_SOCKTYPE:
        return ntsa::Error(ntsa::Error::e_INVALID);
    case EAI_SYSTEM:
        return ntsa::Error(ntsa::Error::last());
    default:
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    if (rc == 0) {
        return ntsa::Error();
    }
    else {
        DWORD lastError = GetLastError();
        if (lastError != 0) {
            return ntsa::Error(lastError);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

#else
#error Not implemented
#endif
}

ntsa::Error ResolverUtil::getIpAddress(bsl::vector<ntsa::IpAddress>* result,
                                       const bslstl::StringRef& domainName,
                                       const ntsa::IpAddressOptions& options)
{
    result->clear();

    ntsa::Error error;
    int         rc;

    bsl::string                  nameString = domainName;
    bsl::vector<ntsa::IpAddress> ipAddressList;

    bdlb::NullableValue<ntsa::IpAddressType::Value> ipAddressType;
    error = ntsu::ResolverUtil::classifyIpAddressType(&ipAddressType, options);
    if (error) {
        return error;
    }

    addrinfo hints;
    bsl::memset(&hints, 0, sizeof(addrinfo));

    if (ipAddressType.isNull()) {
        hints.ai_family = AF_UNSPEC;
    }
    else {
        if (ipAddressType == ntsa::IpAddressType::e_V4) {
            hints.ai_family = AF_INET;
        }
        else if (ipAddressType == ntsa::IpAddressType::e_V6) {
            hints.ai_family = AF_INET6;
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    hints.ai_flags = AI_ADDRCONFIG;

    struct addrinfo* info = 0;
    rc                    = getaddrinfo(nameString.c_str(), 0, &hints, &info);

    if (rc != 0) {
        return Impl::convertGetAddrInfoError(rc);
    }

    Impl::AddrInfoGuard guard(info);

    for (addrinfo* current = info; current != 0; current = current->ai_next) {
        if (current->ai_addr == 0) {
            continue;
        }

        if (current->ai_addr->sa_family == AF_INET) {
            struct sockaddr_in* socketAddressIpv4 =
                reinterpret_cast<struct sockaddr_in*>(current->ai_addr);

            ntsa::Ipv4Address ipv4Address;
            ipv4Address.copyFrom(&socketAddressIpv4->sin_addr,
                                 sizeof socketAddressIpv4->sin_addr);

            ntsa::IpAddress ipAddress(ipv4Address);

            if (bsl::find(ipAddressList.begin(),
                          ipAddressList.end(),
                          ipAddress) == ipAddressList.end())
            {
                ipAddressList.push_back(ipAddress);
            }
        }
        else if (current->ai_addr->sa_family == AF_INET6) {
            struct sockaddr_in6* socketAddressIpv6 =
                reinterpret_cast<struct sockaddr_in6*>(current->ai_addr);

            ntsa::Ipv6Address ipv6Address;
            ipv6Address.copyFrom(&socketAddressIpv6->sin6_addr,
                                 sizeof socketAddressIpv6->sin6_addr);

            ipv6Address.setScopeId(socketAddressIpv6->sin6_scope_id);

            ntsa::IpAddress ipAddress(ipv6Address);

            if (bsl::find(ipAddressList.begin(),
                          ipAddressList.end(),
                          ipAddress) == ipAddressList.end())
            {
                ipAddressList.push_back(ipAddress);
            }
        }
    }

    if (ipAddressType.isNull()) {
        ntsu::ResolverUtil::sortIpAddressList(&ipAddressList);
    }

    if (ipAddressList.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    if (options.ipAddressFilter().has_value()) {
        if (options.ipAddressFilter().value()) {
            options.ipAddressFilter().value()(&ipAddressList);

            if (ipAddressList.empty()) {
                return ntsa::Error(ntsa::Error::e_EOF);
            }
        }
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

ntsa::Error ResolverUtil::getDomainName(bsl::string*           result,
                                        const ntsa::IpAddress& address)
{
    ntsa::Error error;

    sockaddr_storage socketAddressStorage;
    bsl::size_t      socketAddressStorageSize;

    error = ntsu::SocketUtil::encodeEndpoint(&socketAddressStorage,
                                             &socketAddressStorageSize,
                                             ntsa::Endpoint(address, 0));
    if (error) {
        return error;
    }

    char nodeName[1025];
    int  rc =
        getnameinfo(reinterpret_cast<const sockaddr*>(&socketAddressStorage),
                    NTSCFG_WARNING_NARROW(socklen_t, socketAddressStorageSize),
                    nodeName,
                    sizeof nodeName,
                    0,
                    0,
                    NI_NAMEREQD);
    if (rc != 0) {
#if defined(BSLS_PLATFORM_OS_UNIX)
        if (rc == EAI_SYSTEM) {
            return ntsa::Error(ntsa::Error::last());
        }
        else {
            return ntsa::Error::invalid();
        }
#else
        return ntsa::Error(ntsa::Error::last());
#endif
    }

    result->assign(nodeName);
    return ntsa::Error();
}

ntsa::Error ResolverUtil::getPort(bsl::vector<ntsa::Port>* result,
                                  const bslstl::StringRef& serviceName,
                                  const ntsa::PortOptions& options)
{
    result->clear();

    ntsa::Error error;
    int         rc;

    bsl::string_view serviceNameTrimmed =
        bdlb::StringViewUtil::trim(serviceName);

    if (serviceNameTrimmed.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (serviceNameTrimmed[0] == '-') {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    {
        unsigned short   portNumberLiteral = 0;
        bsl::string_view portNumberRemainder;

        rc = bdlb::NumericParseUtil::parseUshort(&portNumberLiteral,
                                                 &portNumberRemainder,
                                                 serviceNameTrimmed);

        if (rc == 0) {
            if (!portNumberRemainder.empty()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            result->push_back(static_cast<ntsa::Port>(portNumberLiteral));
            return ntsa::Error();
        }
    }

    bsl::string nameString(serviceNameTrimmed.data(),
                           serviceNameTrimmed.size());

    bsl::vector<ntsa::Port> portList;

    addrinfo hints;
    bsl::memset(&hints, 0, sizeof(addrinfo));

    if (options.transport().isNull()) {
        hints.ai_family = AF_UNSPEC;
    }
    else {
        if (options.transport().value() == ntsa::Transport::e_TCP_IPV4_STREAM)
        {
            hints.ai_family   = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
        }
        else if (options.transport().value() ==
                 ntsa::Transport::e_UDP_IPV4_DATAGRAM)
        {
            hints.ai_family   = AF_INET;
            hints.ai_socktype = SOCK_DGRAM;
            hints.ai_protocol = IPPROTO_UDP;
        }
        else if (options.transport().value() ==
                 ntsa::Transport::e_TCP_IPV6_STREAM)
        {
            hints.ai_family   = AF_INET6;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
        }
        else if (options.transport().value() ==
                 ntsa::Transport::e_UDP_IPV6_DATAGRAM)
        {
            hints.ai_family   = AF_INET6;
            hints.ai_socktype = SOCK_DGRAM;
            hints.ai_protocol = IPPROTO_UDP;
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    hints.ai_flags = AI_ADDRCONFIG;

    struct addrinfo* info = 0;
    rc                    = getaddrinfo(0, nameString.c_str(), &hints, &info);

    if (rc != 0) {
        return Impl::convertGetAddrInfoError(rc);
    }

    Impl::AddrInfoGuard guard(info);

    for (addrinfo* current = info; current != 0; current = current->ai_next) {
#if defined(BSLS_PLATFORM_OS_UNIX)
        if (current->ai_protocol != IPPROTO_TCP &&
            current->ai_protocol != IPPROTO_UDP)
        {
            continue;
        }
#endif

        if (current->ai_addr == 0) {
            continue;
        }

        if (current->ai_addr->sa_family == AF_INET) {
            struct sockaddr_in* socketAddressIpv4 =
                reinterpret_cast<struct sockaddr_in*>(current->ai_addr);

            ntsa::Port port = ntohs(socketAddressIpv4->sin_port);
            if (port != 0) {
                if (bsl::find(portList.begin(), portList.end(), port) ==
                    portList.end())
                {
                    portList.push_back(port);
                }
            }
        }
        else if (current->ai_addr->sa_family == AF_INET6) {
            struct sockaddr_in6* socketAddressIpv6 =
                reinterpret_cast<struct sockaddr_in6*>(current->ai_addr);

            ntsa::Port port = ntohs(socketAddressIpv6->sin6_port);
            if (port != 0) {
                if (bsl::find(portList.begin(), portList.end(), port) ==
                    portList.end())
                {
                    portList.push_back(port);
                }
            }
        }
    }

    if (portList.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    if (options.portFilter().has_value()) {
        if (options.portFilter().value()) {
            options.portFilter().value()(&portList);

            if (portList.empty()) {
                return ntsa::Error(ntsa::Error::e_EOF);
            }
        }
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

ntsa::Error ResolverUtil::getServiceName(bsl::string*           result,
                                         ntsa::Port             port,
                                         ntsa::Transport::Value transport)
{
    ntsa::Error error;

    ntsa::Endpoint endpoint;
    if (transport == ntsa::Transport::e_TCP_IPV4_STREAM ||
        transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM)
    {
        endpoint = ntsa::Endpoint(
            ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), port));
    }
    else if (transport == ntsa::Transport::e_TCP_IPV6_STREAM ||
             transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM)
    {
        endpoint = ntsa::Endpoint(
            ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), port));
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    sockaddr_storage socketAddressStorage;
    bsl::size_t      socketAddressStorageSize;

    error = ntsu::SocketUtil::encodeEndpoint(&socketAddressStorage,
                                             &socketAddressStorageSize,
                                             endpoint);
    if (error) {
        return error;
    }

    char serviceName[1025];
    int  rc =
        getnameinfo(reinterpret_cast<const sockaddr*>(&socketAddressStorage),
                    NTSCFG_WARNING_NARROW(socklen_t, socketAddressStorageSize),
                    0,
                    0,
                    serviceName,
                    sizeof serviceName,
                    0);
    if (rc != 0) {
#if defined(BSLS_PLATFORM_OS_UNIX)
        if (rc == EAI_SYSTEM) {
            return ntsa::Error(ntsa::Error::last());
        }
        else {
            return ntsa::Error::invalid();
        }
#else
        return ntsa::Error(ntsa::Error::last());
#endif
    }

    result->assign(serviceName);
    return ntsa::Error();
}

ntsa::Error ResolverUtil::getLocalIpAddress(
    bsl::vector<ntsa::IpAddress>* result,
    const ntsa::IpAddressOptions& options)
{
    result->clear();

    ntsa::Error                  error;
    bsl::vector<ntsa::IpAddress> ipAddressList;

    bdlb::NullableValue<ntsa::IpAddressType::Value> ipAddressType;
    error = ntsu::ResolverUtil::classifyIpAddressType(&ipAddressType, options);
    if (error) {
        return error;
    }

    bsl::vector<ntsa::Adapter> adapterList;
    ntsu::AdapterUtil::discoverAdapterList(&adapterList);

    if (adapterList.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    if (ipAddressType.isNull() ||
        ipAddressType.value() == ntsa::IpAddressType::e_V4)
    {
        for (bsl::vector<ntsa::Adapter>::const_iterator it =
                 adapterList.begin();
             it != adapterList.end();
             ++it)
        {
            const ntsa::Adapter& adapter = *it;

            if (!adapter.ipv4Address().isNull()) {
                ipAddressList.push_back(
                    ntsa::IpAddress(adapter.ipv4Address().value()));
            }
        }
    }

    if (ipAddressType.isNull() ||
        ipAddressType.value() == ntsa::IpAddressType::e_V6)
    {
        for (bsl::vector<ntsa::Adapter>::const_iterator it =
                 adapterList.begin();
             it != adapterList.end();
             ++it)
        {
            const ntsa::Adapter& adapter = *it;

            if (!adapter.ipv6Address().isNull()) {
                ipAddressList.push_back(
                    ntsa::IpAddress(adapter.ipv6Address().value()));
            }
        }
    }

    if (ipAddressList.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    if (options.ipAddressFilter().has_value()) {
        if (options.ipAddressFilter().value()) {
            options.ipAddressFilter().value()(&ipAddressList);

            if (ipAddressList.empty()) {
                return ntsa::Error(ntsa::Error::e_EOF);
            }
        }
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

ntsa::Error ResolverUtil::getHostname(bsl::string* result)
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    char buffer[MAXHOSTNAMELEN + 1];
    if (0 != gethostname(buffer, sizeof buffer)) {
        return ntsa::Error(errno);
    }

    result->assign(buffer);
    return ntsa::Error();

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    char  buffer[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof buffer;

    if (0 == GetComputerNameEx(ComputerNameDnsHostname, buffer, &size)) {
        return ntsa::Error(GetLastError());
    }

    result->assign(buffer, size);
    return ntsa::Error();

#else
#error Not implemented
#endif
}

ntsa::Error ResolverUtil::getHostnameFullyQualified(bsl::string* result)
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    int rc;

    char buffer[MAXHOSTNAMELEN + 1];
    if (0 != gethostname(buffer, sizeof buffer)) {
        return ntsa::Error(errno);
    }

    struct addrinfo hints;
    bsl::memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_flags  = AI_CANONNAME;

    struct addrinfo* info = 0;
    rc                    = getaddrinfo(buffer, 0, &hints, &info);
    if (rc != 0) {
        return Impl::convertGetAddrInfoError(rc);
    }

    Impl::AddrInfoGuard guard(info);

    result->assign(info->ai_canonname);

    return ntsa::Error();

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    char  buffer[256];  // MAX_COMPUTERNAME_LENGTH + 1
    DWORD size = sizeof buffer;

    if (0 == GetComputerNameEx(ComputerNameDnsFullyQualified, buffer, &size)) {
        return ntsa::Error(GetLastError());
    }

    result->assign(buffer, size);
    return ntsa::Error();

#else
#error Not implemented
#endif
}

ntsa::Error ResolverUtil::classifyIpAddressType(
    bdlb::NullableValue<ntsa::IpAddressType::Value>* result,
    const ntsa::IpAddressOptions&                    options)
{
    result->reset();

    bdlb::NullableValue<ntsa::IpAddressType::Value> ipAddressType =
        options.ipAddressType();

    if (!ipAddressType.isNull()) {
        if (ipAddressType.value() == ntsa::IpAddressType::e_UNDEFINED) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (!options.transport().isNull()) {
            if (options.transport().value() ==
                    ntsa::Transport::e_TCP_IPV4_STREAM ||
                options.transport().value() ==
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM)
            {
                if (ipAddressType.value() != ntsa::IpAddressType::e_V4) {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }
            }
            else if (options.transport().value() ==
                         ntsa::Transport::e_TCP_IPV6_STREAM ||
                     options.transport().value() ==
                         ntsa::Transport::e_UDP_IPV6_DATAGRAM)
            {
                if (ipAddressType.value() != ntsa::IpAddressType::e_V6) {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
    }
    else if (!options.transport().isNull()) {
        if (options.transport().value() ==
                ntsa::Transport::e_TCP_IPV4_STREAM ||
            options.transport().value() ==
                ntsa::Transport::e_UDP_IPV4_DATAGRAM)
        {
            ipAddressType = ntsa::IpAddressType::e_V4;
        }
        else if (options.transport().value() ==
                     ntsa::Transport::e_TCP_IPV6_STREAM ||
                 options.transport().value() ==
                     ntsa::Transport::e_UDP_IPV6_DATAGRAM)
        {
            ipAddressType = ntsa::IpAddressType::e_V6;
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    *result = ipAddressType;
    return ntsa::Error();
}

void ResolverUtil::sortIpAddressList(
    bsl::vector<ntsa::IpAddress>* ipAddressList)
{
    if (ipAddressList->empty()) {
        return;
    }

    char ARENA[256];

    bdlma::BufferedSequentialAllocator allocator(ARENA, sizeof ARENA);

    bsl::vector<ntsa::IpAddress> ipv4AddressList(&allocator);
    bsl::vector<ntsa::IpAddress> ipv6AddressList(&allocator);

    ipv4AddressList.reserve(ipAddressList->size());
    ipv6AddressList.reserve(ipAddressList->size());

    for (bsl::vector<ntsa::IpAddress>::const_iterator it =
             ipAddressList->begin();
         it != ipAddressList->end();
         ++it)
    {
        const ntsa::IpAddress& ipAddress = *it;

        if (ipAddress.isV4()) {
            ipv4AddressList.push_back(ipAddress);
        }
        else if (ipAddress.isV6()) {
            ipv6AddressList.push_back(ipAddress);
        }
    }

    ipAddressList->clear();

    if (!ipv4AddressList.empty()) {
        ipAddressList->insert(ipAddressList->end(),
                              ipv4AddressList.begin(),
                              ipv4AddressList.end());
    }

    if (!ipv6AddressList.empty()) {
        ipAddressList->insert(ipAddressList->end(),
                              ipv6AddressList.begin(),
                              ipv6AddressList.end());
    }
}

}  // close package namespace
}  // close enterprise namespace
