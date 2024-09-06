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

#ifndef INCLUDED_NTSA_IPADDRESS
#define INCLUDED_NTSA_IPADDRESS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ipaddresstype.h>
#include <ntsa_ipv4address.h>
#include <ntsa_ipv6address.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a union of Internet Protocol version 4 and 6
/// addresses.
///
/// @details
/// Provide a value-semantic type that represents a discriminated
/// union of either an Internet Protocol version 4 address or an Internet
/// Protocol version 6 address.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par Usage Example 1: Represent an IPv4 address
/// The following example illustrates how to construct a 'ntsa::IpAddress'
/// to represent an IPv4 address.
///
///     ntsa::IpAddress ipAddress("127.0.0.1");
///     NTSCFG_TEST_TRUE(ipAddress.isV4());
///     NTSCFG_TEST_EQ(ipAddress.v4(), ntsa::Ipv4Address::loopback());
///
///
/// @par Usage Example 2: Represent an IPv6 address
/// The following example illustrates how to construct a 'ntsa::IpAddress'
/// to represent an IPv6 address.
///
///     ntsa::IpAddress ipAddress("::1");
///     NTSCFG_TEST_TRUE(ipAddress.isV6());
///     NTSCFG_TEST_EQ(ipAddress.v6(), ntsa::Ipv6Address::loopback());
///
/// @ingroup module_ntsa_identity
class IpAddress
{
    union {
        bsls::ObjectBuffer<ntsa::Ipv4Address> d_v4;
        bsls::ObjectBuffer<ntsa::Ipv6Address> d_v6;
    };

    ntsa::IpAddressType::Value d_type;

  public:
    /// Create a new IP address having an undefined selection.
    IpAddress();

    /// Create a new address having a "v4" representation having the
    /// specified 'value'.
    explicit IpAddress(const ntsa::Ipv4Address& value);

    /// Create a new address having a "v6" representation having the
    /// specified 'value'.
    explicit IpAddress(const ntsa::Ipv6Address& value);

    /// Create a new address parsed from the specified 'text'
    /// representation.
    explicit IpAddress(const bslstl::StringRef& text);

    /// Create a new IP address having the same value as the specified
    /// 'other' object.
    IpAddress(const IpAddress& ip_other);

    /// Destroy this object.
    ~IpAddress();

    /// Assign the value of the specified 'other' primitive representation
    /// to this primitive representation.
    IpAddress& operator=(const IpAddress& other);

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IpAddress& operator=(const Ipv4Address& other);

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    IpAddress& operator=(const Ipv6Address& other);

    /// Set the value of the object from the specified 'text'.
    IpAddress& operator=(const bslstl::StringRef& text);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the value of this object from the value parsed from any of its
    /// textual representations. Return true if the 'text' is in a valid
    /// format and was parsed successfully, otherwise return false. If false
    /// is returned then the value of this object was set to its value upon
    /// default construction.
    bool parse(const bslstl::StringRef& text);

    /// Select the "v4" address representation. Return a reference to the
    /// modifiable representation.
    ntsa::Ipv4Address& makeV4();

    /// Select the "v4" address representation initially having the
    /// specified 'value'. Return a reference to the modifiable
    /// representation.
    ntsa::Ipv4Address& makeV4(const ntsa::Ipv4Address& value);

    /// Select the "v6" address representation. Return a reference to the
    /// modifiable representation.
    ntsa::Ipv6Address& makeV6();

    /// Select the "v6" address representation initially having the
    /// specified 'value'. Return a reference to the modifiable
    /// representation.
    ntsa::Ipv6Address& makeV6(const ntsa::Ipv6Address& value);

    /// Return a reference to the modifiable "v4" address representation.
    /// The behavior is undefined unless 'isV4()' is true.
    ntsa::Ipv4Address& v4();

    /// Return a reference to the modifiable "v6" address representation.
    /// The behavior is undefined unless 'isV6()' is true.
    ntsa::Ipv6Address& v6();

    /// Return the textual representation of this object.
    bsl::string text() const;

    /// Return a reference to the non-modifiable "v4" address
    /// representation.  The behavior is undefined unless 'isV4()' is true.
    const ntsa::Ipv4Address& v4() const;

    /// Return a reference to the non-modifiable "v6" address
    /// representation.  The behavior is undefined unless 'isV6()' is true.
    const ntsa::Ipv6Address& v6() const;

    /// Return the type of the IP address representation.
    ntsa::IpAddressType::Value type() const;

    /// Return true if the address representation is undefined, otherwise
    /// return false.
    bool isUndefined() const;

    /// Return true if the "v4" address representation is currently
    /// selected, otherwise return false.
    bool isV4() const;

    /// Return true if the "v6" address representation is currently
    /// selected, otherwise return false.
    bool isV6() const;

    /// Return true if the address is the wildcard address of the specified
    /// 'addressType', otherwise return false.
    bool isAny(ntsa::IpAddressType::Value addressType) const;

    /// Return true if the address identifies the loopback device of the
    /// specified 'addressType', otherwise return false.
    bool isLoopback(ntsa::IpAddressType::Value addressType) const;

    /// Return true if the address is a private address of the specified
    /// 'addressType', otherwise return false.
    bool isPrivate(ntsa::IpAddressType::Value addressType) const;

    /// Return true if the address is a link-local address of the specified
    /// 'addressType', otherwise return false.
    bool isLinkLocal(ntsa::IpAddressType::Value addressType) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const IpAddress& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const IpAddress& other) const;

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

    /// Return the IPv4 address that represents any address.
    static ntsa::IpAddress anyIpv4();

    /// Return the IPv6 address that represents any address.
    static ntsa::IpAddress anyIpv6();

    /// Return the IPv4 address that represents the loopback address.
    static ntsa::IpAddress loopbackIpv4();

    /// Return the IPv6 address that represents the loopback address.
    static ntsa::IpAddress loopbackIpv6();

    /// Return the address of the specified 'addressType' that represents
    /// any address.
    static ntsa::IpAddress any(ntsa::IpAddressType::Value addressType);

    /// Return the address of the specified 'addressType' that represents
    /// the loopback address.
    static ntsa::IpAddress loopback(ntsa::IpAddressType::Value addressType);

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(IpAddress);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::IpAddress
bsl::ostream& operator<<(bsl::ostream& stream, const IpAddress& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IpAddress
bool operator==(const IpAddress& lhs, const IpAddress& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IpAddress
bool operator!=(const IpAddress& lhs, const IpAddress& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IpAddress
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IpAddress& value);

NTSCFG_INLINE
IpAddress::IpAddress()
: d_type(ntsa::IpAddressType::e_UNDEFINED)
{
}

NTSCFG_INLINE
IpAddress::IpAddress(const ntsa::Ipv4Address& value)
: d_type(ntsa::IpAddressType::e_V4)
{
    new (d_v4.buffer()) ntsa::Ipv4Address(value);
}

NTSCFG_INLINE
IpAddress::IpAddress(const ntsa::Ipv6Address& value)
: d_type(ntsa::IpAddressType::e_V6)
{
    new (d_v6.buffer()) ntsa::Ipv6Address(value);
}

NTSCFG_INLINE
IpAddress::IpAddress(const IpAddress& other)
: d_type(other.d_type)
{
    switch (d_type) {
    case ntsa::IpAddressType::e_V4:
        new (d_v4.buffer()) ntsa::Ipv4Address(other.d_v4.object());
        break;
    case ntsa::IpAddressType::e_V6:
        new (d_v6.buffer()) ntsa::Ipv6Address(other.d_v6.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::IpAddressType::e_UNDEFINED);
    }
}

NTSCFG_INLINE
IpAddress::~IpAddress()
{
}

NTSCFG_INLINE
IpAddress& IpAddress::operator=(const IpAddress& other)
{
    if (this == &other) {
        return *this;
    }

    switch (other.d_type) {
    case ntsa::IpAddressType::e_V4:
        this->makeV4(other.d_v4.object());
        break;
    case ntsa::IpAddressType::e_V6:
        this->makeV6(other.d_v6.object());
        break;
    default:
        BSLS_ASSERT(other.d_type == ntsa::IpAddressType::e_UNDEFINED);
        this->reset();
    }

    return *this;
}

NTSCFG_INLINE
IpAddress& IpAddress::operator=(const Ipv4Address& other)
{
    this->makeV4(other);
    return *this;
}

NTSCFG_INLINE
IpAddress& IpAddress::operator=(const Ipv6Address& other)
{
    this->makeV6(other);
    return *this;
}

NTSCFG_INLINE
void IpAddress::reset()
{
    d_type = ntsa::IpAddressType::e_UNDEFINED;
}

NTSCFG_INLINE
ntsa::Ipv4Address& IpAddress::makeV4()
{
    if (d_type == ntsa::IpAddressType::e_V4) {
        d_v4.object().reset();
    }
    else {
        this->reset();
        new (d_v4.buffer()) ntsa::Ipv4Address();
        d_type = ntsa::IpAddressType::e_V4;
    }

    return d_v4.object();
}

NTSCFG_INLINE
ntsa::Ipv4Address& IpAddress::makeV4(const ntsa::Ipv4Address& value)
{
    if (d_type == ntsa::IpAddressType::e_V4) {
        d_v4.object() = value;
    }
    else {
        this->reset();
        new (d_v4.buffer()) ntsa::Ipv4Address(value);
        d_type = ntsa::IpAddressType::e_V4;
    }

    return d_v4.object();
}

NTSCFG_INLINE
ntsa::Ipv6Address& IpAddress::makeV6()
{
    if (d_type == ntsa::IpAddressType::e_V6) {
        d_v6.object().reset();
    }
    else {
        this->reset();
        new (d_v6.buffer()) ntsa::Ipv6Address();
        d_type = ntsa::IpAddressType::e_V6;
    }

    return d_v6.object();
}

NTSCFG_INLINE
ntsa::Ipv6Address& IpAddress::makeV6(const ntsa::Ipv6Address& value)
{
    if (d_type == ntsa::IpAddressType::e_V6) {
        d_v6.object() = value;
    }
    else {
        this->reset();
        new (d_v6.buffer()) ntsa::Ipv6Address(value);
        d_type = ntsa::IpAddressType::e_V6;
    }

    return d_v6.object();
}

NTSCFG_INLINE
ntsa::Ipv4Address& IpAddress::v4()
{
    BSLS_ASSERT(d_type == ntsa::IpAddressType::e_V4);
    return d_v4.object();
}

NTSCFG_INLINE
ntsa::Ipv6Address& IpAddress::v6()
{
    BSLS_ASSERT(d_type == ntsa::IpAddressType::e_V6);
    return d_v6.object();
}

NTSCFG_INLINE
const ntsa::Ipv4Address& IpAddress::v4() const
{
    BSLS_ASSERT(d_type == ntsa::IpAddressType::e_V4);
    return d_v4.object();
}

NTSCFG_INLINE
const ntsa::Ipv6Address& IpAddress::v6() const
{
    BSLS_ASSERT(d_type == ntsa::IpAddressType::e_V6);
    return d_v6.object();
}

NTSCFG_INLINE
ntsa::IpAddressType::Value IpAddress::type() const
{
    return d_type;
}

NTSCFG_INLINE
bool IpAddress::isUndefined() const
{
    return (d_type == ntsa::IpAddressType::e_UNDEFINED);
}

NTSCFG_INLINE
bool IpAddress::isV4() const
{
    return (d_type == ntsa::IpAddressType::e_V4);
}

NTSCFG_INLINE
bool IpAddress::isV6() const
{
    return (d_type == ntsa::IpAddressType::e_V6);
}

NTSCFG_INLINE
bool IpAddress::isAny(ntsa::IpAddressType::Value addressType) const
{
    if (d_type != addressType) {
        return false;
    }

    if (d_type == ntsa::IpAddressType::e_V4) {
        return d_v4.object().isAny();
    }
    else if (d_type == ntsa::IpAddressType::e_V6) {
        return d_v6.object().isAny();
    }
    else {
        return false;
    }
}

NTSCFG_INLINE
bool IpAddress::isLoopback(ntsa::IpAddressType::Value addressType) const
{
    if (d_type != addressType) {
        return false;
    }

    if (d_type == ntsa::IpAddressType::e_V4) {
        return d_v4.object().isLoopback();
    }
    else if (d_type == ntsa::IpAddressType::e_V6) {
        return d_v6.object().isLoopback();
    }
    else {
        return false;
    }
}

NTSCFG_INLINE
bool IpAddress::isPrivate(ntsa::IpAddressType::Value addressType) const
{
    if (d_type != addressType) {
        return false;
    }

    if (d_type == ntsa::IpAddressType::e_V4) {
        return d_v4.object().isPrivate();
    }
    else if (d_type == ntsa::IpAddressType::e_V6) {
        return d_v6.object().isPrivate();
    }
    else {
        return false;
    }
}

NTSCFG_INLINE
bool IpAddress::isLinkLocal(ntsa::IpAddressType::Value addressType) const
{
    if (d_type != addressType) {
        return false;
    }

    if (d_type == ntsa::IpAddressType::e_V4) {
        return d_v4.object().isLinkLocal();
    }
    else if (d_type == ntsa::IpAddressType::e_V6) {
        return d_v6.object().isLinkLocal();
    }
    else {
        return false;
    }
}

NTSCFG_INLINE
ntsa::IpAddress IpAddress::anyIpv4()
{
    return ntsa::IpAddress(ntsa::Ipv4Address::any());
}

NTSCFG_INLINE
ntsa::IpAddress IpAddress::anyIpv6()
{
    return ntsa::IpAddress(ntsa::Ipv6Address::any());
}

NTSCFG_INLINE
ntsa::IpAddress IpAddress::loopbackIpv4()
{
    return ntsa::IpAddress(ntsa::Ipv4Address::loopback());
}

NTSCFG_INLINE
ntsa::IpAddress IpAddress::loopbackIpv6()
{
    return ntsa::IpAddress(ntsa::Ipv6Address::loopback());
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const IpAddress& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const IpAddress& lhs, const IpAddress& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const IpAddress& lhs, const IpAddress& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const IpAddress& lhs, const IpAddress& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IpAddress& value)
{
    using bslh::hashAppend;

    if (value.isV4()) {
        hashAppend(algorithm, value.v4());
    }
    else if (value.isV6()) {
        hashAppend(algorithm, value.v6());
    }
}

}  // close package namespace
}  // close enterprise namespace
#endif
