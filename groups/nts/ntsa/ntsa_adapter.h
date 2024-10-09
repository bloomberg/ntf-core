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

#ifndef INCLUDED_NTSA_ADAPTER
#define INCLUDED_NTSA_ADAPTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ipv4address.h>
#include <ntsa_ipv6address.h>
#include <ntsa_ipv6scopeid.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a description of a network interface.
///
/// @details
/// Provide a value-semantic type that describes a network
/// interface present on the local host.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class Adapter
{
    bsl::string                            d_name;
    bsl::uint32_t                          d_index;
    bsl::string                            d_ethernetAddress;
    bdlb::NullableValue<ntsa::Ipv4Address> d_ipv4Address;
    bdlb::NullableValue<ntsa::Ipv6Address> d_ipv6Address;
    bool                                   d_multicast;

  public:
    /// Create a new adapter. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Adapter(bslma::Allocator* basicAllocator = 0);

    /// Create a new adapter having the same value as the specified 'other'
    /// object. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    Adapter(const Adapter& other, bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Adapter();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Adapter& operator=(const Adapter& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the name of the adapter to the specified 'name'.
    void setName(const bsl::string& name);

    /// Set the adapter index to the specified 'index'.
    void setIndex(bsl::uint32_t index);

    /// Set the Ethernet (MAC) address of the adapter to the specified
    /// 'ethernet_address'.
    void setEthernetAddress(const bsl::string& ethernetAddress);

    /// Set the IPv4 address of the adapter to the specified 'ipv4_address'.
    void setIpv4Address(const ntsa::Ipv4Address& ipv4Address);

    /// Set the IPv6 address of the adapter to the specified 'ipv6_address'.
    void setIpv6Address(const ntsa::Ipv6Address& ipv6Address);

    /// Set the flag that indicates this adapter can be a multicast
    /// recipient to the specified 'multicast'.
    void setMulticast(bool multicast);

    /// Return the name of the adapter.
    const bsl::string& name() const;

    /// Return the adapter index.
    bsl::uint32_t index() const;

    /// Return the Ethernet (MAC) address of the adapter.
    const bsl::string& ethernetAddress() const;

    /// Return the IPv4 address of the adapter, or null if no such IPv4
    /// address has been assigned to this adapter.
    const bdlb::NullableValue<ntsa::Ipv4Address>& ipv4Address() const;

    /// Return the IPv6 address of the adapter, or null if no such IPv6
    /// address has been assigned to this adapter.
    const bdlb::NullableValue<ntsa::Ipv6Address>& ipv6Address() const;

    /// Return the flag that indicates this adapter can be a multicast
    /// recipient.
    bool multicast() const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Adapter);
};

}  // close package namespace
}  // close enterprise namespace
#endif
