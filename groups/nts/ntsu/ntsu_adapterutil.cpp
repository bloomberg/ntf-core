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

#include <ntsu_adapterutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsu_adapterutil_cpp, "$Id$ $CSID$")

#include <ntsa_error.h>

#include <bslma_allocator.h>
#include <bslma_deallocatorguard.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_map.h>
#include <bsl_sstream.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/types.h>
#if defined(BSLS_PLATFORM_OS_FREEBSD) || defined(BSLS_PLATFORM_OS_DARWIN)
#include <net/if_dl.h>
#endif
#if defined(BSLS_PLATFORM_OS_FREEBSD) || defined(BSLS_PLATFORM_OS_DARWIN) ||  \
    defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_SOLARIS)
#include <ifaddrs.h>
#endif
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif
#ifdef WINVER
#undef WINVER
#endif
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define NTDDI_VERSION 0x06000100
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// clang-format off
#include <windows.h>
#include <winerror.h>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
// clang-format on
#define MAXHOSTNAMELEN 256
#pragma comment(lib, "iphlpapi")
#endif

// Uncomment to skip the inclusion of VMware adapters
// #define NTSU_ADAPTERUTIL_SKIP_VMWARE 1

namespace BloombergLP {
namespace ntsu {

namespace {

/// Sort the addresses first by public IP, then the loop back interface, the
/// by private IP.
struct AdapterSorterByIndex {
    bool operator()(const ntsa::Adapter& lhs, const ntsa::Adapter& rhs) const
    {
        return lhs.index() < rhs.index();
    }
};

/// Sort the addresses first by public IP, then the loop back interface, the
/// by private IP.
struct AdapterSorterByAddress {
    bool operator()(const ntsa::Adapter& lhs, const ntsa::Adapter& rhs) const
    {
        if (!lhs.ipv4Address().isNull() && !rhs.ipv4Address().isNull()) {
            if (lhs.ipv4Address().value().isLoopback() &&
                !rhs.ipv4Address().value().isLoopback())
            {
                return false;
            }

            if (lhs.ipv4Address().value().isPrivate() &&
                !rhs.ipv4Address().value().isPrivate())
            {
                return false;
            }

            return lhs.ipv4Address() < rhs.ipv4Address();
        }
        else if (!lhs.ipv6Address().isNull() && !rhs.ipv6Address().isNull()) {
            if (lhs.ipv6Address().value().isLoopback() &&
                !rhs.ipv6Address().value().isLoopback())
            {
                return false;
            }

            if (lhs.ipv6Address().value().isPrivate() &&
                !rhs.ipv6Address().value().isPrivate())
            {
                return false;
            }

            return lhs.ipv6Address() < rhs.ipv6Address();
        }
        else {
            return lhs.ethernetAddress() < rhs.ethernetAddress();
        }
    }
};

#if defined(BSLS_PLATFORM_OS_WINDOWS)

void convertWideString(bsl::string* destination, const WCHAR* source)
{
    // Load into the specified 'destination' string the specified
    // wide-character 'source' string converted into UTF-8.

    int rc;

    destination->clear();

    bsl::size_t sourceLength = wcslen(source);

    if (sourceLength == 0) {
        return;
    }

    rc = WideCharToMultiByte(CP_UTF8,
                             0,
                             (LPCWCH)(&source[0]),
                             (int)sourceLength,
                             0,
                             0,
                             0,
                             0);

    if (rc <= 0) {
        destination->clear();
        return;
    }

    destination->resize(rc, 0);

    rc = WideCharToMultiByte(CP_UTF8,
                             0,
                             (LPCWCH)(&source[0]),
                             (int)sourceLength,
                             &((*destination)[0]),
                             rc,
                             0,
                             0);

    if (rc <= 0) {
        destination->clear();
        return;
    }
}

#endif

}  // close unnamed namespace

void AdapterUtil::discoverAdapterList(bsl::vector<ntsa::Adapter>* result)
{
#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD) ||  \
    defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_SOLARIS)

    // Note: Not all devices that are operational must be marked as RUNNING,
    // e.g. TUN/TAP devices. Do not exclude device that are not RUNNING.

    result->clear();

    struct ifaddrs* interfaceAddressList = 0;
    struct ifaddrs* interfaceAddress     = 0;

    int rc = getifaddrs(&interfaceAddressList);
    if (0 != rc) {
        ntsa::Error error(errno);

        bsl::stringstream ss;
        ss << error;

        BSLS_LOG_ERROR("Failed to discover network adapter list: %s",
                       ss.str().c_str());

        return;
    }

    typedef bsl::map<bsl::string, ntsa::Adapter> AdapterMap;
    AdapterMap                                   adapters;

    for (interfaceAddress = interfaceAddressList; interfaceAddress != 0;
         interfaceAddress = interfaceAddress->ifa_next)
    {
        if (interfaceAddress->ifa_addr == 0) {
            continue;
        }

        if (interfaceAddress->ifa_name == 0 ||
            bsl::strlen(interfaceAddress->ifa_name) == 0)
        {
            continue;
        }

        bsl::string adapterName = interfaceAddress->ifa_name;

        if ((interfaceAddress->ifa_flags & IFF_UP) == 0) {
            continue;
        }

        ntsa::Adapter& adapter = adapters[adapterName];

        adapter.setName(adapterName);

        bsl::uint32_t interfaceIndex = if_nametoindex(adapterName.c_str());

        if (interfaceIndex != 0) {
            adapter.setIndex(interfaceIndex);
        }
        else {
            BSLS_LOG_WARN("No interface index available for adapter '%s'",
                          adapterName.c_str());
        }

        if ((interfaceAddress->ifa_flags & IFF_MULTICAST) != 0) {
            adapter.setMulticast(true);
        }
        else {
            adapter.setMulticast(false);
        }

        if (interfaceAddress->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in* socketAddressIpv4 =
                reinterpret_cast<struct sockaddr_in*>(
                    interfaceAddress->ifa_addr);

            BSLS_ASSERT(AF_INET == socketAddressIpv4->sin_family);

            ntsa::Ipv4Address ipv4Address;
            ipv4Address.copyFrom(&socketAddressIpv4->sin_addr,
                                 sizeof socketAddressIpv4->sin_addr);

            adapter.setIpv4Address(ipv4Address);
        }
        else if (interfaceAddress->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6* socketAddressIpv6 =
                reinterpret_cast<struct sockaddr_in6*>(
                    interfaceAddress->ifa_addr);

            BSLS_ASSERT(AF_INET6 == socketAddressIpv6->sin6_family);

            ntsa::Ipv6Address ipv6Address;
            ipv6Address.copyFrom(&socketAddressIpv6->sin6_addr,
                                 sizeof socketAddressIpv6->sin6_addr);

            ipv6Address.setScopeId(socketAddressIpv6->sin6_scope_id);

            adapter.setIpv6Address(ipv6Address);
        }
#if defined(BSLS_PLATFORM_OS_FREEBSD) || defined(BSLS_PLATFORM_OS_DARWIN)
        else if (interfaceAddress->ifa_addr->sa_family == AF_LINK) {
            struct sockaddr_dl* socketAddressEthernet =
                reinterpret_cast<struct sockaddr_dl*>(
                    interfaceAddress->ifa_addr);

            const bsl::uint8_t* mac =
                (const bsl::uint8_t*)(LLADDR(socketAddressEthernet));

            char  buffer[18];
            char* target = buffer;

            for (bsl::size_t i = 0; i < 6; ++i) {
                const bsl::uint8_t value = mac[i];

                const bsl::uint8_t a = value >> 4;
                const bsl::uint8_t b = value & 0x0F;

                if (a < 10) {
                    *target++ = static_cast<char>('0' + a);
                }
                else {
                    *target++ = static_cast<char>('a' + (a - 10));
                }

                if (b < 10) {
                    *target++ = static_cast<char>('0' + b);
                }
                else {
                    *target++ = static_cast<char>('a' + (b - 10));
                }

                if (i != 5) {
                    *target++ = ':';
                }
            }

            *target++ = 0;

            adapter.setEthernetAddress(bsl::string(buffer));
        }
#else
        else if (interfaceAddress->ifa_addr->sa_family == AF_PACKET) {
            const bsl::uint8_t* mac =
                (const bsl::uint8_t*)(interfaceAddress->ifa_addr->sa_data);

            mac = mac + 10;

            char  buffer[18];
            char* target = buffer;

            for (bsl::size_t i = 0; i < 6; ++i) {
                const bsl::uint8_t value = mac[i];

                const bsl::uint8_t a = value >> 4;
                const bsl::uint8_t b = value & 0x0F;

                if (a < 10) {
                    *target++ = static_cast<char>('0' + a);
                }
                else {
                    *target++ = static_cast<char>('a' + (a - 10));
                }

                if (b < 10) {
                    *target++ = static_cast<char>('0' + b);
                }
                else {
                    *target++ = static_cast<char>('a' + (b - 10));
                }

                if (i != 5) {
                    *target++ = ':';
                }
            }

            *target++ = 0;

            adapter.setEthernetAddress(bsl::string(buffer));
        }
#endif
    }

    if (interfaceAddressList != 0) {
        freeifaddrs(interfaceAddressList);
    }

    for (AdapterMap::const_iterator it = adapters.begin();
         it != adapters.end();
         ++it)
    {
        const ntsa::Adapter& adapter = it->second;

        if (adapter.ipv4Address().isNull() ||
            adapter.ipv4Address().value().isAny())
        {
            continue;
        }

        result->push_back(adapter);
    }

    bsl::sort(result->begin(), result->end(), AdapterSorterByIndex());

#elif defined(BSLS_PLATFORM_OS_AIX)

    int rc;

    result->clear();

    typedef bsl::map<bsl::string, ntsa::Adapter> AdapterMap;
    AdapterMap                                   adapters;

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd <= 0) {
        return;
    }

    bsl::vector<char> buffer;
    {
        int bufferSize;
        rc = ioctl(fd, SIOCGSIZIFCONF, &bufferSize);
        if (rc != 0) {
            close(fd);
            return;
        }
        if (bufferSize <= 0 || bufferSize >= 1024 * 1024 * 4) {
            return;
        }

        buffer.resize(bufferSize);
    }

    struct ifconf ifc;
    bsl::memset(&ifc, 0, sizeof ifc);

    ifc.ifc_buf = &buffer[0];
    ifc.ifc_len = buffer.size();

    rc = ioctl(fd, SIOCGIFCONF, &ifc);
    if (rc != 0) {
        close(fd);
        return;
    }

    char* cursor = ifc.ifc_buf;
    while (cursor < ifc.ifc_buf + ifc.ifc_len) {
        struct ifreq* ifr = reinterpret_cast<struct ifreq*>(cursor);

        if (ifr->ifr_addr.sa_family == AF_INET) {
            bsl::string adapterName = ifr->ifr_name;

            ntsa::Adapter& adapter = adapters[adapterName];

            if (adapter.name().empty()) {
                adapter.setName(adapterName);
            }

            if (adapter.index() == 0) {
                bsl::uint32_t interfaceIndex =
                    if_nametoindex(adapterName.c_str());

                if (interfaceIndex != 0) {
                    adapter.setIndex(interfaceIndex);
                }
            }

            if ((ifr->ifr_flags & IFF_MULTICAST) != 0) {
                adapter.setMulticast(true);
            }

            struct sockaddr_in* socketAddressIpv4 =
                reinterpret_cast<struct sockaddr_in*>(&ifr->ifr_addr);

            BSLS_ASSERT(AF_INET == socketAddressIpv4->sin_family);

            ntsa::Ipv4Address ipv4Address;
            ipv4Address.copyFrom(&socketAddressIpv4->sin_addr,
                                 sizeof socketAddressIpv4->sin_addr);

            adapter.setIpv4Address(ipv4Address);
        }
        else if (ifr->ifr_addr.sa_family == AF_INET6) {
            bsl::string adapterName = ifr->ifr_name;

            ntsa::Adapter& adapter = adapters[adapterName];

            if (adapter.name().empty()) {
                adapter.setName(adapterName);
            }

            if (adapter.index() == 0) {
                bsl::uint32_t interfaceIndex =
                    if_nametoindex(adapterName.c_str());

                if (interfaceIndex != 0) {
                    adapter.setIndex(interfaceIndex);
                }
            }

            if ((ifr->ifr_flags & IFF_MULTICAST) != 0) {
                adapter.setMulticast(true);
            }

            struct sockaddr_in6* socketAddressIpv6 =
                reinterpret_cast<struct sockaddr_in6*>(&ifr->ifr_addr);

            BSLS_ASSERT(AF_INET6 == socketAddressIpv6->sin6_family);

            ntsa::Ipv6Address ipv6Address;
            ipv6Address.copyFrom(&socketAddressIpv6->sin6_addr,
                                 sizeof socketAddressIpv6->sin6_addr);

            ipv6Address.setScopeId(socketAddressIpv6->sin6_scope_id);

            adapter.setIpv6Address(ipv6Address);
        }
        else if (ifr->ifr_addr.sa_family == AF_LINK) {
            bsl::string adapterName = ifr->ifr_name;

            ntsa::Adapter& adapter = adapters[adapterName];

            if (adapter.name().empty()) {
                adapter.setName(adapterName);
            }

            if (adapter.index() == 0) {
                bsl::uint32_t interfaceIndex =
                    if_nametoindex(adapterName.c_str());

                if (interfaceIndex != 0) {
                    adapter.setIndex(interfaceIndex);
                }
            }

            struct ifhwaddr_req ifhwreq;
            bsl::memset(&ifhwreq, 0, sizeof ifhwreq);
            bsl::strcpy(ifhwreq.ifr_name, ifr->ifr_name);

            rc = ioctl(fd, SIOCGIFHWADDR, &ifhwreq);
            if (rc == 0) {
                if (ifhwreq.addr_len == 6) {
                    const bsl::uint8_t* mac =
                        (const bsl::uint8_t*)(ifhwreq.ifr_hwaddr);

                    char  buffer[18];
                    char* target = buffer;

                    for (bsl::size_t i = 0; i < 6; ++i) {
                        const bsl::uint8_t value = mac[i];

                        const bsl::uint8_t a = value >> 4;
                        const bsl::uint8_t b = value & 0x0F;

                        if (a < 10) {
                            *target++ = static_cast<char>('0' + a);
                        }
                        else {
                            *target++ = static_cast<char>('a' + (a - 10));
                        }

                        if (b < 10) {
                            *target++ = static_cast<char>('0' + b);
                        }
                        else {
                            *target++ = static_cast<char>('a' + (b - 10));
                        }

                        if (i != 5) {
                            *target++ = ':';
                        }
                    }

                    *target++ = 0;

                    adapter.setEthernetAddress(bsl::string(buffer));
                }
            }
        }

        cursor += sizeof(ifr->ifr_name) +
                  bsl::max(static_cast<bsl::size_t>(ifr->ifr_addr.sa_len),
                           sizeof(ifr->ifr_addr));
    }

    close(fd);

    for (AdapterMap::const_iterator it = adapters.begin();
         it != adapters.end();
         ++it)
    {
        const ntsa::Adapter& adapter = it->second;

        if (adapter.ipv4Address().isNull() ||
            adapter.ipv4Address().value().isAny())
        {
            continue;
        }

        result->push_back(adapter);
    }

    bsl::sort(result->begin(), result->end(), AdapterSorterByIndex());

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    ULONG interfaceAddressListSize = 0;
    {
        ULONG rc = GetAdaptersAddresses(
            AF_UNSPEC,
            GAA_FLAG_INCLUDE_ALL_INTERFACES | GAA_FLAG_SKIP_MULTICAST,
            0,
            0,
            &interfaceAddressListSize);

        if (rc == ERROR_NO_DATA) {
            return;
        }
        else if (rc != ERROR_BUFFER_OVERFLOW) {
            ntsa::Error error(WSAGetLastError());
            BSLS_LOG_ERROR("Failed to lookup adapters: %s",
                           error.text().c_str());
            return;
        }

        BSLS_ASSERT(interfaceAddressListSize > 0);
    }

    bslma::Allocator* allocator = bslma::Default::defaultAllocator();

    IP_ADAPTER_ADDRESSES* interfaceAddressList =
        static_cast<IP_ADAPTER_ADDRESSES*>(
            allocator->allocate(interfaceAddressListSize));

    bslma::DeallocatorGuard<bslma::Allocator> guard(interfaceAddressList,
                                                    allocator);

    {
        ULONG rc = GetAdaptersAddresses(
            AF_UNSPEC,
            GAA_FLAG_INCLUDE_ALL_INTERFACES | GAA_FLAG_SKIP_MULTICAST,
            0,
            interfaceAddressList,
            &interfaceAddressListSize);

        if (rc != ERROR_SUCCESS) {
            if (rc == ERROR_NO_DATA) {
                return;
            }
            else {
                ntsa::Error error(WSAGetLastError());
                BSLS_LOG_ERROR("Failed to lookup adapters: %s",
                               error.text().c_str());
                return;
            }
        }
    }

    typedef bsl::map<bsl::string, ntsa::Adapter> AdapterMap;
    AdapterMap                                   adapters;

    bool haveLoopbackIpv4 = false;
    bool haveLoopbackIpv6 = false;

    for (IP_ADAPTER_ADDRESSES* interfaceAddress = interfaceAddressList;
         interfaceAddress != 0;
         interfaceAddress = interfaceAddress->Next)
    {
        if (interfaceAddress->OperStatus != IfOperStatusUp) {
            continue;
        }

        if (interfaceAddress->IfType != IF_TYPE_ETHERNET_CSMACD &&
#if defined(IF_TYPE_IEEE80211)
            interfaceAddress->IfType != IF_TYPE_IEEE80211 &&
#endif
            interfaceAddress->IfType != IF_TYPE_SOFTWARE_LOOPBACK)
        {
            continue;
        }

        bsl::string friendlyName;
        convertWideString(&friendlyName, interfaceAddress->FriendlyName);

#if NTSU_ADAPTERUTIL_SKIP_VMWARE
        bsl::string description;
        convertWideString(&description, interfaceAddress->Description);

        // Skip over host side VMware adapters, which have names like:
        // "VMware Virtual Ethernet Adapter for VMnet1".
        bool skip =
            (bsl::strstr(interfaceAddress->AdapterName, "VMnet") != 0) ||
            (bsl::strstr(friendlyName.c_str(), "VMnet") != 0) ||
            (bsl::strstr(description.c_str(), "VMnet") != 0);

        if (skip) {
            continue;
        }
#endif

        ntsa::Adapter& adapter = adapters[friendlyName];
        adapter.setName(friendlyName);
        adapter.setIndex(interfaceAddress->Ipv6IfIndex);

        if ((interfaceAddress->Flags & IP_ADAPTER_NO_MULTICAST) != 0) {
            adapter.setMulticast(false);
        }
        else {
            adapter.setMulticast(true);
        }

        {
            const char* mac = (char*)interfaceAddress->PhysicalAddress;

            char  buffer[18];
            char* target = buffer;

            for (bsl::size_t i = 0; i < 6; ++i) {
                const bsl::uint8_t value = mac[i];

                const bsl::uint8_t a = value >> 4;
                const bsl::uint8_t b = value & 0x0F;

                if (a < 10) {
                    *target++ = static_cast<char>('0' + a);
                }
                else {
                    *target++ = static_cast<char>('a' + (a - 10));
                }

                if (b < 10) {
                    *target++ = static_cast<char>('0' + b);
                }
                else {
                    *target++ = static_cast<char>('a' + (b - 10));
                }

                if (i != 5) {
                    *target++ = ':';
                }
            }

            *target++ = 0;

            adapter.setEthernetAddress(buffer);
        }

        for (IP_ADAPTER_UNICAST_ADDRESS* unicast =
                 interfaceAddress->FirstUnicastAddress;
             unicast != 0;
             unicast = unicast->Next)
        {
            if (unicast->Address.iSockaddrLength == sizeof(SOCKADDR_IN)) {
                struct sockaddr_in* socketAddressIpv4 =
                    reinterpret_cast<struct sockaddr_in*>(
                        unicast->Address.lpSockaddr);

                BSLS_ASSERT(AF_INET == socketAddressIpv4->sin_family);

                ntsa::Ipv4Address ipv4Address;
                ipv4Address.copyFrom(&socketAddressIpv4->sin_addr,
                                     sizeof socketAddressIpv4->sin_addr);

                adapter.setIpv4Address(ipv4Address);

                if (ipv4Address.isLoopback()) {
                    haveLoopbackIpv4 = true;
                }
            }
            else if (unicast->Address.iSockaddrLength == sizeof(SOCKADDR_IN6))
            {
                struct sockaddr_in6* socketAddressIpv6 =
                    reinterpret_cast<struct sockaddr_in6*>(
                        unicast->Address.lpSockaddr);

                BSLS_ASSERT(AF_INET6 == socketAddressIpv6->sin6_family);

                ntsa::Ipv6Address ipv6Address;
                ipv6Address.copyFrom(&socketAddressIpv6->sin6_addr,
                                     sizeof socketAddressIpv6->sin6_addr);

                ipv6Address.setScopeId(socketAddressIpv6->sin6_scope_id);

                adapter.setIpv6Address(ipv6Address);

                if (ipv6Address.isLoopback()) {
                    haveLoopbackIpv6 = true;
                }
            }
        }
    }

    if (!haveLoopbackIpv4 && !haveLoopbackIpv6) {
        ntsa::Adapter& adapter = adapters["Loopback"];
        adapter.setName("Loopback");
        adapter.setIpv4Address(ntsa::Ipv4Address::loopback());
        adapter.setIpv6Address(ntsa::Ipv6Address::loopback());
    }

    for (AdapterMap::const_iterator it = adapters.begin();
         it != adapters.end();
         ++it)
    {
        const ntsa::Adapter& adapter = it->second;

        if (adapter.ipv4Address().isNull() ||
            adapter.ipv4Address().value().isAny())
        {
            continue;
        }

        result->push_back(adapter);
    }

    bsl::sort(result->begin(), result->end(), AdapterSorterByIndex());

#else
#error Not implemented
#endif
}

bool AdapterUtil::discoverAdapter(ntsa::Adapter*             result,
                                  ntsa::IpAddressType::Value addressType,
                                  bool                       multicast)
{
    bsl::vector<ntsa::Adapter> adapterList;
    ntsu::AdapterUtil::discoverAdapterList(&adapterList);
    for (bsl::vector<ntsa::Adapter>::const_iterator it = adapterList.begin();
         it != adapterList.end();
         ++it)
    {
        const ntsa::Adapter& candidateAdapter = *it;

        if (addressType == ntsa::IpAddressType::e_V4) {
            if (!candidateAdapter.ipv4Address().isNull()) {
                if (multicast && !candidateAdapter.multicast()) {
                    continue;
                }

                *result = candidateAdapter;
                return true;
            }
        }
        else if (addressType == ntsa::IpAddressType::e_V6) {
            if (!candidateAdapter.ipv6Address().isNull()) {
                if (multicast && !candidateAdapter.multicast()) {
                    continue;
                }

                *result = candidateAdapter;
                return true;
            }
        }
    }

    return false;
}

bsl::uint32_t AdapterUtil::discoverInterfaceIndex(
    const ntsa::IpAddress& address)
{
    if (address.isV4()) {
        return AdapterUtil::discoverInterfaceIndex(address.v4());
    }
    else if (address.isV6()) {
        return AdapterUtil::discoverInterfaceIndex(address.v6());
    }
    else {
        return 0;
    }
}

bsl::uint32_t AdapterUtil::discoverInterfaceIndex(
    const ntsa::Ipv4Address& address)
{
    bsl::uint32_t interfaceIndex = 0;

    bsl::vector<ntsa::Adapter> adapters;
    ntsu::AdapterUtil::discoverAdapterList(&adapters);

    for (bsl::vector<ntsa::Adapter>::const_iterator it = adapters.begin();
         it != adapters.end();
         ++it)
    {
        const ntsa::Adapter& adapter = *it;
        if (!adapter.ipv4Address().isNull()) {
            if (adapter.ipv4Address().value() == address) {
                interfaceIndex = adapter.index();
                break;
            }
        }
    }

    return interfaceIndex;
}

bsl::uint32_t AdapterUtil::discoverInterfaceIndex(
    const ntsa::Ipv6Address& address)
{
    bsl::uint32_t interfaceIndex = 0;

    bsl::vector<ntsa::Adapter> adapters;
    ntsu::AdapterUtil::discoverAdapterList(&adapters);

    for (bsl::vector<ntsa::Adapter>::const_iterator it = adapters.begin();
         it != adapters.end();
         ++it)
    {
        const ntsa::Adapter& adapter = *it;
        if (!adapter.ipv6Address().isNull()) {
            if (adapter.ipv6Address().value() == address) {
                interfaceIndex = adapter.index();
                break;
            }
        }
    }

    return interfaceIndex;
}

bsl::uint32_t AdapterUtil::discoverScopeId(const ntsa::Ipv6Address& address)
{
    bsl::uint32_t scopeId = 0;

    bsl::vector<ntsa::Adapter> adapters;
    ntsu::AdapterUtil::discoverAdapterList(&adapters);

    for (bsl::vector<ntsa::Adapter>::const_iterator it = adapters.begin();
         it != adapters.end();
         ++it)
    {
        const ntsa::Adapter& adapter = *it;
        if (!adapter.ipv6Address().isNull()) {
            if (adapter.ipv6Address().value().equalsScopeless(address)) {
                scopeId = adapter.ipv6Address().value().scopeId();
                break;
            }
        }
    }

    return scopeId;
}

bool AdapterUtil::supportsIpv4()
{
#if NTSCFG_BUILD_WITH_ADDRESS_FAMILY_IPV4

    bsl::vector<ntsa::Adapter> adapterList;
    ntsu::AdapterUtil::discoverAdapterList(&adapterList);
    for (bsl::vector<ntsa::Adapter>::const_iterator it = adapterList.begin();
         it != adapterList.end();
         ++it)
    {
        const ntsa::Adapter& candidateAdapter = *it;

        if (!candidateAdapter.ipv4Address().isNull()) {
            return true;
        }
    }

    return false;

#else

    return false;

#endif
}

bool AdapterUtil::supportsIpv4Multicast()
{
#if NTSCFG_BUILD_WITH_ADDRESS_FAMILY_IPV4

    bsl::vector<ntsa::Adapter> adapterList;
    ntsu::AdapterUtil::discoverAdapterList(&adapterList);
    for (bsl::vector<ntsa::Adapter>::const_iterator it = adapterList.begin();
         it != adapterList.end();
         ++it)
    {
        const ntsa::Adapter& candidateAdapter = *it;

        if (!candidateAdapter.ipv4Address().isNull() &&
            candidateAdapter.multicast())
        {
            return true;
        }
    }

    return false;

#else

    return false;

#endif
}

bool AdapterUtil::supportsIpv6()
{
#if NTSCFG_BUILD_WITH_ADDRESS_FAMILY_IPV6

    bsl::vector<ntsa::Adapter> adapterList;
    ntsu::AdapterUtil::discoverAdapterList(&adapterList);
    for (bsl::vector<ntsa::Adapter>::const_iterator it = adapterList.begin();
         it != adapterList.end();
         ++it)
    {
        if (!it->ipv6Address().isNull()) {
            return true;
        }
    }

    return false;

#else

    return false;

#endif
}

bool AdapterUtil::supportsIpv6Multicast()
{
#if NTSCFG_BUILD_WITH_ADDRESS_FAMILY_IPV6

    bsl::vector<ntsa::Adapter> adapterList;
    ntsu::AdapterUtil::discoverAdapterList(&adapterList);
    for (bsl::vector<ntsa::Adapter>::const_iterator it = adapterList.begin();
         it != adapterList.end();
         ++it)
    {
        const ntsa::Adapter& candidateAdapter = *it;

        if (!candidateAdapter.ipv6Address().isNull() &&
            candidateAdapter.multicast())
        {
            return true;
        }
    }

    return false;

#else

    return false;

#endif
}

bool AdapterUtil::supportsTcp()
{
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_TCP
#if NTSCFG_BUILD_WITH_ADDRESS_FAMILY_IPV4 ||                                  \
    NTSCFG_BUILD_WITH_ADDRESS_FAMILY_IPV6

    bsl::vector<ntsa::Adapter> adapterList;
    ntsu::AdapterUtil::discoverAdapterList(&adapterList);
    for (bsl::vector<ntsa::Adapter>::const_iterator it = adapterList.begin();
         it != adapterList.end();
         ++it)
    {
#if NTSCFG_BUILD_WITH_ADDRESS_FAMILY_IPV4
        if (!it->ipv4Address().isNull()) {
            return true;
        }
#endif

#if NTSCFG_BUILD_WITH_ADDRESS_FAMILY_IPV6
        if (!it->ipv6Address().isNull()) {
            return true;
        }
#endif
    }

    return false;

#else

    return false;

#endif
#else

    return false;

#endif
}

bool AdapterUtil::supportsUdp()
{
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_UDP
#if NTSCFG_BUILD_WITH_ADDRESS_FAMILY_IPV4 ||                                  \
    NTSCFG_BUILD_WITH_ADDRESS_FAMILY_IPV6

    bsl::vector<ntsa::Adapter> adapterList;
    ntsu::AdapterUtil::discoverAdapterList(&adapterList);
    for (bsl::vector<ntsa::Adapter>::const_iterator it = adapterList.begin();
         it != adapterList.end();
         ++it)
    {
#if NTSCFG_BUILD_WITH_ADDRESS_FAMILY_IPV4
        if (!it->ipv4Address().isNull()) {
            return true;
        }
#endif

#if NTSCFG_BUILD_WITH_ADDRESS_FAMILY_IPV6
        if (!it->ipv6Address().isNull()) {
            return true;
        }
#endif
    }

    return false;

#else

    return false;

#endif
#else

    return false;

#endif
}

bool AdapterUtil::supportsLocalStream()
{
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
#if NTSCFG_BUILD_WITH_ADDRESS_FAMILY_LOCAL

#if defined(BSLS_PLATFORM_OS_UNIX)
    return true;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    return true;
#else
#error Unsupported platform
#endif

#else

    return false;

#endif
#else

    return false;

#endif
}

bool AdapterUtil::supportsLocalDatagram()
{
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
#if NTSCFG_BUILD_WITH_ADDRESS_FAMILY_LOCAL

#if defined(BSLS_PLATFORM_OS_UNIX)
    return true;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    return false;
#else
#error Unsupported platform
#endif

#else

    return false;

#endif
#else

    return false;

#endif
}

bool AdapterUtil::supportsTransport(ntsa::Transport::Value transport)
{
    if (transport == ntsa::Transport::e_TCP_IPV4_STREAM) {
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_TCP
        return AdapterUtil::supportsIpv4();
#else
        return false;
#endif
    }
    else if (transport == ntsa::Transport::e_TCP_IPV6_STREAM) {
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_TCP
        return AdapterUtil::supportsIpv6();
#else
        return false;
#endif
    }
    else if (transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM) {
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_UDP
        return AdapterUtil::supportsIpv4();
#else
        return false;
#endif
    }
    else if (transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM) {
#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_UDP
        return AdapterUtil::supportsIpv6();
#else
        return false;
#endif
    }
    else if (transport == ntsa::Transport::e_LOCAL_STREAM) {
        return AdapterUtil::supportsLocalStream();
    }
    else if (transport == ntsa::Transport::e_LOCAL_DATAGRAM) {
        return AdapterUtil::supportsLocalDatagram();
    }
    else {
        return false;
    }
}

}  // close package namespace
}  // close enterprise namespace
