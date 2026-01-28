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
    enum Flag {
        e_MULTICAST = 1 << 1,
        e_TX_SOFTWARE_TIMESTAMPS = 1 << 2,
        e_TX_HARDWARE_TIMESTAMPS = 1 << 3,
        e_RX_SOFTWARE_TIMESTAMPS = 1 << 4,
        e_RX_HARDWARE_TIMESTAMPS = 1 << 5
    };

    bsl::string                            d_name;
    bsl::string                            d_description;
    bsl::uint32_t                          d_index;
    bsl::string                            d_ethernetAddress;
    bdlb::NullableValue<ntsa::Ipv4Address> d_ipv4Address;
    bdlb::NullableValue<ntsa::Ipv6Address> d_ipv6Address;
    bsl::size_t                            d_speed;
    bsl::uint32_t                          d_flags;

  private:
    /// Print a human-readable description of the specified 'flags' to the
    /// specified 'stream'. Return the 'stream'. 
    static bsl::ostream& printFlags(bsl::ostream& stream,
                                    bsl::uint32_t flags,
                                    int           level,
                                    int           spacesPerLevel);

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

    /// Set the name of the adapter to the specified 'value'.
    void setName(const bsl::string& value);

    /// Set the description of the adapter to the specified 'value'.
    void setDescription(const bsl::string& value);

    /// Set the adapter index to the specified 'value'.
    void setIndex(bsl::uint32_t value);

    /// Set the Ethernet (MAC) address of the adapter to the specified 'value'.
    void setEthernetAddress(const bsl::string& value);

    /// Set the IPv4 address of the adapter to the specified 'value'.
    void setIpv4Address(const ntsa::Ipv4Address& value);

    /// Set the IPv6 address of the adapter to the specified 'value'.
    void setIpv6Address(const ntsa::Ipv6Address& value);

    /// Set the speed of this adapter, in megabits per second. 
    void setSpeed(bsl::size_t value);

    /// Set the flag that indicates this adapter can be a multicast
    /// recipient to the specified 'value'.
    void setMulticast(bool value);

    /// Set the flag that indicates this adapter supports software timestamping
    /// on outgoing packets.
    void setTxSoftwareTimestamps(bool value);

    /// Set the flag that indicates this adapter supports hardware timestamping
    /// on outgoing packets.
    void setTxHardwareTimestamps(bool value);

    /// Set the flag that indicates this adapter supports software timestamping
    /// on incoming packets.
    void setRxSoftwareTimestamps(bool value);

    /// Set the flag that indicates this adapter supports hardware timestamping
    /// on incoming packets.
    void setRxHardwareTimestamps(bool value);

    /// Return the name of the adapter.
    const bsl::string& name() const;

    /// Return the description of the adapter. 
    const bsl::string& description() const;

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

    /// Return the speed of this adapter, in bits per second. 
    bsl::size_t speed() const;

    /// Return true if this adapter can be a multicast recipient, otherwise
    /// return false.
    bool multicast() const;

    /// Return true if this adapter supports software timestamping on outgoing
    /// packets, otherwise return false.
    bool txSoftwareTimestamps() const;

    /// Return true if this adapter supports hardware timestamping on outgoing
    /// packets, otherwise return false.
    bool txHardwareTimestamps() const;

    /// Return true if this adapter supports software timestamping on incoming
    /// packets, otherwise return false. 
    bool rxSoftwareTimestamps() const;

    /// Return true if this adapter supports hardware timestamping on incoming
    /// packets, otherwise return false. 
    bool rxHardwareTimestamps() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const Adapter& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const Adapter& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

    /// Format this object to the specified output 'stream' at the optionally
    /// specified indentation 'level' and return a reference to the modifiable
    /// 'stream'.  If 'level' is specified, optionally specify
    /// 'spacesPerLevel', the number of spaces per indentation level for this
    /// and all of its nested objects.  Each line is indented by the absolute
    /// value of 'level * spacesPerLevel'.  If 'level' is negative, suppress
    /// indentation of the first line.  If 'spacesPerLevel' is negative,
    /// suppress line breaks and format the entire output on one line.  If
    /// 'stream' is initially invalid, this operation has no effect.  Note that
    /// a trailing newline is provided in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Adapter);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntsa::Adapter
bsl::ostream& operator<<(bsl::ostream& stream, const Adapter& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntsa::Adapter
bool operator==(const Adapter& lhs, const Adapter& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntsa::Adapter
bool operator!=(const Adapter& lhs, const Adapter& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::Adapter
bool operator<(const Adapter& lhs, const Adapter& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Adapter
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Adapter& value);

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void Adapter::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_index);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM& algorithm, const Adapter& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
