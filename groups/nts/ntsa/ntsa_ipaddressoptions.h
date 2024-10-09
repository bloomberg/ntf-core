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

#ifndef INCLUDED_NTSA_IPADDRESSOPTIONS
#define INCLUDED_NTSA_IPADDRESSOPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ipaddress.h>
#include <ntsa_transport.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide options to get an IP address from a host.
///
/// @details
/// Provide a value-semantic type that describes the options to get
/// an IP address from a host.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b ipAddressFallback:
/// The implied IP address when no domain name or IP address is explicitly
/// defined. The default value is null, which indicates that resolution should
/// fail unless a domain name or IP address is explicitly defined.
///
/// @li @b ipAddressType:
/// The IP address type desired from the domain name resolution. The default
/// value is null, which indicates that a domain name can resolve to any IP
/// address suitable for being bound by a process on the local machine.
///
/// @li @b ipAddressSelector:
/// The round-robin selector of the chosen IP address out of the IP address
/// list assigned to a domain name. This value is always applied modulo the
/// size of the IP address list that is the result of resolving a domain name.
/// The default value is null, indicating all IP addresses are returned.
///
/// @li @b transport:
/// The desired transport with which to use the endpoint. This value affects
/// how domain names resolve to IP addresses. The default value is null,
/// indicating that domain names are allowed to resolve to IP addresses of any
/// type.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_operation
class IpAddressOptions
{
    bdlb::NullableValue<ntsa::IpAddress>            d_ipAddressFallback;
    bdlb::NullableValue<ntsa::IpAddressType::Value> d_ipAddressType;
    bdlb::NullableValue<bsl::size_t>                d_ipAddressSelector;
    bdlb::NullableValue<ntsa::Transport::Value>     d_transport;

  public:
    /// Create new send options having the default value.
    IpAddressOptions();

    /// Create new send options having the same value as the specified
    /// 'original' object.
    IpAddressOptions(const IpAddressOptions& original);

    /// Destroy this object.
    ~IpAddressOptions();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IpAddressOptions& operator=(const IpAddressOptions& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the implied IP address when no domain name or IP address is
    /// explicitly defined to the specified 'value'. The default value is
    /// null, which indicates that resolution should fail unless a domain
    /// name or IP address is explicitly defined.
    void setIpAddressFallback(const ntsa::IpAddress& value);

    /// Set the IP address type desired from the domain name resolution to
    /// the specified 'value'. The default value is null, which indicates
    /// that a domain name can resolve to any IP address suitable for being
    /// bound by a process on the local machine.
    void setIpAddressType(ntsa::IpAddressType::Value value);

    /// Set the round-robin selector of the chosen IP address out of the IP
    /// address list assigned to a domain name to the specified 'value'.
    /// This value is always applied modulo the size of the IP address list
    /// that is the result of resolving a domain name. The default value is
    /// null, indicating the first IP address in the IP address list
    /// is selected.
    void setIpAddressSelector(bsl::size_t value);

    /// Set the desired transport with which to use the endpoint to the
    /// specified 'value'. This value affects how domain names resolve to
    /// IP addresses. The default value is null, indicating that domain
    /// names are allowed to resolve to IP addresses of any type.
    void setTransport(ntsa::Transport::Value value);

    /// Return the implied IP address when no domain name or IP address is
    /// explicitly defined to the specified 'value'. The default value is
    /// null, which indicates that resolution should fail unless a domain
    /// name or IP address is explicitly defined.
    const bdlb::NullableValue<ntsa::IpAddress>& ipAddressFallback() const;

    /// Return the IP address type desired from the domain name resolution
    /// to the specified 'value'. The default value is null, which indicates
    /// that a domain name can resolve to any IP address suitable for being
    /// bound by a process on the local machine.
    const bdlb::NullableValue<ntsa::IpAddressType::Value>& ipAddressType()
        const;

    /// Return the round-robin selector of the chosen IP address out of the
    /// IP address list assigned to a domain name to the specified 'value'.
    /// This value is always applied modulo the size of the IP address list
    /// that is the result of resolving a domain name. The default value is
    /// null, indicating the first IP address in the IP address list is
    /// selected.
    const bdlb::NullableValue<bsl::size_t>& ipAddressSelector() const;

    /// Return the desired transport with which to use the endpoint to the
    /// specified 'value'. This value affects how domain names resolve to
    /// IP addresses. The default value is null, indicating that domain
    /// names are allowed to resolve to IP addresses of any type.
    const bdlb::NullableValue<ntsa::Transport::Value>& transport() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const IpAddressOptions& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const IpAddressOptions& other) const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(IpAddressOptions);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(IpAddressOptions);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::IpAddressOptions
bsl::ostream& operator<<(bsl::ostream& stream, const IpAddressOptions& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IpAddressOptions
bool operator==(const IpAddressOptions& lhs, const IpAddressOptions& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IpAddressOptions
bool operator!=(const IpAddressOptions& lhs, const IpAddressOptions& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::IpAddressOptions
bool operator<(const IpAddressOptions& lhs, const IpAddressOptions& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IpAddressOptions
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IpAddressOptions& value);

NTSCFG_INLINE
IpAddressOptions::IpAddressOptions()
: d_ipAddressFallback()
, d_ipAddressType()
, d_ipAddressSelector()
, d_transport()
{
}

NTSCFG_INLINE
IpAddressOptions::IpAddressOptions(const IpAddressOptions& original)
: d_ipAddressFallback(original.d_ipAddressFallback)
, d_ipAddressType(original.d_ipAddressType)
, d_ipAddressSelector(original.d_ipAddressSelector)
, d_transport(original.d_transport)
{
}

NTSCFG_INLINE
IpAddressOptions::~IpAddressOptions()
{
}

NTSCFG_INLINE
IpAddressOptions& IpAddressOptions::operator=(const IpAddressOptions& other)
{
    d_ipAddressFallback = other.d_ipAddressFallback;
    d_ipAddressType     = other.d_ipAddressType;
    d_ipAddressSelector = other.d_ipAddressSelector;
    d_transport         = other.d_transport;
    return *this;
}

NTSCFG_INLINE
void IpAddressOptions::reset()
{
    d_ipAddressFallback.reset();
    d_ipAddressType.reset();
    d_ipAddressSelector.reset();
    d_transport.reset();
}

NTSCFG_INLINE
void IpAddressOptions::setIpAddressFallback(const ntsa::IpAddress& value)
{
    d_ipAddressFallback = value;
}

NTSCFG_INLINE
void IpAddressOptions::setIpAddressType(ntsa::IpAddressType::Value value)
{
    d_ipAddressType = value;
}

NTSCFG_INLINE
void IpAddressOptions::setIpAddressSelector(bsl::size_t value)
{
    d_ipAddressSelector = value;
}

NTSCFG_INLINE
void IpAddressOptions::setTransport(ntsa::Transport::Value value)
{
    d_transport = value;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::IpAddress>& IpAddressOptions::
    ipAddressFallback() const
{
    return d_ipAddressFallback;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::IpAddressType::Value>& IpAddressOptions::
    ipAddressType() const
{
    return d_ipAddressType;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& IpAddressOptions::ipAddressSelector()
    const
{
    return d_ipAddressSelector;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Transport::Value>& IpAddressOptions::
    transport() const
{
    return d_transport;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const IpAddressOptions& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IpAddressOptions& lhs, const IpAddressOptions& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IpAddressOptions& lhs, const IpAddressOptions& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const IpAddressOptions& lhs, const IpAddressOptions& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IpAddressOptions& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.ipAddressFallback());
    hashAppend(algorithm, value.ipAddressType());
    hashAppend(algorithm, value.ipAddressSelector());
    hashAppend(algorithm, value.transport());
}

}  // close package namespace
}  // close enterprise namespace
#endif
