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

#ifndef INCLUDED_NTSU_ADAPTERUTIL
#define INCLUDED_NTSU_ADAPTERUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_adapter.h>
#include <ntsa_ipaddress.h>
#include <ntsa_ipv4address.h>
#include <ntsa_ipv6address.h>
#include <ntsa_ipv6scopeid.h>
#include <ntsa_transport.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsu {

/// @internal @brief
/// Provide utilities for enumerating network devices.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @par Usage Example 1: Enumerating the Network Devices on the Local Host
/// This example illustrates how to enumerate the network interfaces present
/// on the local host.
///
///     bsl::vector<ntsa::Adapter> adapterList;
///     ntsu::AdapterUtil::discoverAdapterList(&adapterList);
///
///     for (bsl::vector<ntsa::Adapter>::const_iterator
///             it  = adapterList.begin();
///             it != adapterList.end();
///           ++it)
///     {
///         bsl::string name            = it->name();
///         bsl::size_t index           = it->index();
///         bsl::string ethernetAddress = it->ethernetAddress();
///         bsl::string ipv4Address     = it->ipv4Address().text();
///         bsl::string ipv6Address;
///
///         if (!it->ipv6Address().isNull()) {
///             ipv6Address = it->ipv6Address().value().text();
///         }
///
///         BSLS_LOG_INFO("Discovered adapter"
///                       "\nName:     %s"
///                       "\nIndex:    %d"
///                       "\nEthernet: %s"
///                       "\nIPv4:     %s"
///                       "\nIPv6:     %s"
///                       "\nMulticast %s",
///                       name.c_str(),
///                       index,
///                       ethernetAddress.c_str(),
///                       ipv4Address.c_str(),
///                       ipv6Address.c_str(),
///                       (it->multicast() ? "YES" : "NO"));
///     }
///
/// @ingroup module_ntsu
struct AdapterUtil {
    /// Load into the specified 'result' the list of all the network
    /// adapters of the local machine.
    static void discoverAdapterList(bsl::vector<ntsa::Adapter>* result);

    /// Load into the specified 'adapter' the first adapter found assigned
    /// an IP address of the specified 'addressType'. Require that the
    /// resulting adapter support multicast according to the specified
    /// 'multicast' flag. Return true if such an adapter is found, and false
    /// otherwise.
    static bool discoverAdapter(ntsa::Adapter*             result,
                                ntsa::IpAddressType::Value addressType,
                                bool                       multicast);

    /// Return the interface index for the adapter assigned the specified
    /// 'address', or 0 if no such adapter is assigned the address.
    static bsl::uint32_t discoverInterfaceIndex(
        const ntsa::IpAddress& address);

    /// Return the interface index for the adapter assigned the specified
    /// 'address', or 0 if no such adapter is assigned the address.
    static bsl::uint32_t discoverInterfaceIndex(
        const ntsa::Ipv4Address& address);

    /// Return the interface index for the adapter assigned the specified
    /// 'address', or 0 if no such adapter is assigned the address.
    static bsl::uint32_t discoverInterfaceIndex(
        const ntsa::Ipv6Address& address);

    /// Return the IPv6 scope ID for the adapter assigned the specified
    /// 'address'.
    static bsl::uint32_t discoverScopeId(const ntsa::Ipv6Address& address);

    /// Return true if the current machine has any adapter assigned an
    /// IPv4 address, otherwise return false.
    static bool supportsIpv4();

    /// Return true if the current machine has any adapter assigned an
    /// IPv4 address that supports multicast, otherwise return false.
    static bool supportsIpv4Multicast();

    /// Return true if the current machine has any adapter assigned an
    /// IPv6 address, otherwise return false.
    static bool supportsIpv6();

    /// Return true if the current machine has any adapter assigned an
    /// IPv6 address that supports multicast, otherwise return false.
    static bool supportsIpv6Multicast();

    /// Return true if the current machine has any adapter assigned to
    /// either an IPv4 or IPv6 address that supports the Transport Control
    /// Protocol (TCP), otherwise return false.
    static bool supportsTcp();

    /// Return true if the current machine has any adapter assigned to
    /// either an IPv4 or IPv6 address that supports the User Datagram
    /// Protocol (UDP), otherwise return false.
    static bool supportsUdp();

    /// Return true if the current machine supports local (a.k.a Unix)
    /// domain stream sockets, otherwise return false.
    static bool supportsLocalStream();

    /// Return true if the current machine supports local (a.k.a Unix)
    /// domain datagram sockets, otherwise return false.
    static bool supportsLocalDatagram();

    /// Return true if the current machine supports the specified
    /// 'transport', otherwise return false.
    static bool supportsTransport(ntsa::Transport::Value transport);
};

}  // end namespace ntsu
}  // end namespace BloombergLP
#endif
