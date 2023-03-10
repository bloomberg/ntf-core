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

#ifndef INCLUDED_NTSA_HOST
#define INCLUDED_NTSA_HOST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_domainname.h>
#include <ntsa_hosttype.h>
#include <ntsa_ipaddress.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a union of a Domain Name and an IP address.
///
/// @details
/// Provide a value-semantic type that represents a discriminated
/// union of either a Domain Name or an IP address. Such a representation
/// identifies a host on an Internet network.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par Usage Example: Represent a Domain Name
/// The following example illustrates how to construct a 'ntsa::Host' to
/// represent a Domain Name.
///
///     ntsa::Host host("localhost.localdomain");
///     NTSCFG_TEST_TRUE(host.isDomainName());
///     NTSCFG_TEST_EQ(host.domainName().text() ==
///                    bsl::string("localhost.localdomain"));
///
///
/// @par Usage Example: Represent an IPv4 address
/// The following example illustrates how to construct a 'ntsa::Host' to
/// represent an IPv4 address.
///
///     ntsa::Host host("127.0.0.1");
///     NTSCFG_TEST_TRUE(host.isIp());
///     NTSCFG_TEST_TRUE(host.ip().isV4());
///     NTSCFG_TEST_EQ(host.ip().v4() == ntsa::Ipv4Address::loopback());
///
///
/// @par Usage Example: Represent an IPv6 address
/// The following example illustrates how to construct a 'ntsa::Host' to
/// represent an IPv6 address.
///
///     ntsa::Host host("::1");
///     NTSCFG_TEST_TRUE(host.isIp());
///     NTSCFG_TEST_TRUE(host.ip().isV6());
///     NTSCFG_TEST_EQ(host.ip().v6() == ntsa::Ipv6Address::loopback());
///
/// @ingroup module_ntsa_identity
class Host
{
    union {
        bsls::ObjectBuffer<ntsa::DomainName> d_domainName;
        bsls::ObjectBuffer<ntsa::IpAddress>  d_ip;
    };

    ntsa::HostType::Value d_type;

  public:
    /// Create a new address having an undefined type.
    Host();

    /// Create a new address having a "domain name" representation having
    /// the specified 'value'.
    explicit Host(const ntsa::DomainName& value);

    /// Create a new address having a "ip" representation having the
    /// specified 'value'.
    explicit Host(const ntsa::IpAddress& value);

    /// Create a new address parsed from the specified 'text'
    /// representation.
    explicit Host(const bslstl::StringRef& text);

    /// Create a new address having the same value as the specified 'other'
    /// object.
    Host(const Host& other);

    /// Destroy this object.
    ~Host();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Host& operator=(const Host& other);

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Host& operator=(const ntsa::DomainName& other);

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Host& operator=(const ntsa::IpAddress& other);

    /// Set the value of the object from the specified 'text'.
    Host& operator=(const bslstl::StringRef& text);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the value of this object from the value parsed from any of its
    /// textual representations. Return true if the 'text' is in a valid
    /// format and was parsed successfully, otherwise return false.
    bool parse(const bslstl::StringRef& text);

    /// Select the "domain name" address representation. Return a reference
    /// to the modifiable representation.
    ntsa::DomainName& makeDomainName();

    /// Select the "domain name" address representation initially having the
    /// specified 'value'. Return a reference to the modifiable
    /// representation.
    ntsa::DomainName& makeDomainName(const ntsa::DomainName& value);

    /// Select the "ip" address representation. Return a reference to the
    /// modifiable representation.
    ntsa::IpAddress& makeIp();

    /// Select the "ip" address representation initially having the
    /// specified 'value'. Return a reference to the modifiable
    /// representation.
    ntsa::IpAddress& makeIp(const ntsa::IpAddress& value);

    /// Return a reference to the modifiable "domain name" address
    /// representation. The behavior is undefined unless 'is_DomainName()'
    /// is true.
    ntsa::DomainName& domainName();

    /// Return a reference to the modifiable "ip" address representation.
    /// The behavior is undefined unless 'is_ip()' is true.
    ntsa::IpAddress& ip();

    /// Return the textual representation of this object.
    bsl::string text() const;

    /// Return a reference to the non-modifiable "domain name" address
    /// representation. The behavior is undefined unless 'is_DomainName()'
    /// is true.
    const ntsa::DomainName& domainName() const;

    /// Return a reference to the non-modifiable "ip" address
    /// representation.  The behavior is undefined unless 'is_ip()' is true.
    const ntsa::IpAddress& ip() const;

    /// Return the type of the address representation.
    ntsa::HostType::Value type() const;

    /// Return true if the address representation is undefined, otherwise
    /// return false.
    bool isUndefined() const;

    /// Return true if the "domain name" address representation is currently
    /// selected, otherwise return false.
    bool isDomainName() const;

    /// Return true if the "ip" address representation is currently
    /// selected, otherwise return false.
    bool isIp() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Host& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Host& other) const;

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

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(Host);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::Host
bsl::ostream& operator<<(bsl::ostream& stream, const Host& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Host
bool operator==(const Host& lhs, const Host& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Host
bool operator!=(const Host& lhs, const Host& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Host
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Host& value);

NTSCFG_INLINE
Host::Host()
: d_type(ntsa::HostType::e_UNDEFINED)
{
}

NTSCFG_INLINE
Host::~Host()
{
}

NTSCFG_INLINE
void Host::reset()
{
    d_type = ntsa::HostType::e_UNDEFINED;
}

NTSCFG_INLINE
ntsa::DomainName& Host::domainName()
{
    BSLS_ASSERT(d_type == ntsa::HostType::e_DOMAIN_NAME);
    return d_domainName.object();
}

NTSCFG_INLINE
ntsa::IpAddress& Host::ip()
{
    BSLS_ASSERT(d_type == ntsa::HostType::e_IP);
    return d_ip.object();
}

NTSCFG_INLINE
const ntsa::DomainName& Host::domainName() const
{
    BSLS_ASSERT(d_type == ntsa::HostType::e_DOMAIN_NAME);
    return d_domainName.object();
}

NTSCFG_INLINE
const ntsa::IpAddress& Host::ip() const
{
    BSLS_ASSERT(d_type == ntsa::HostType::e_IP);
    return d_ip.object();
}

NTSCFG_INLINE
ntsa::HostType::Value Host::type() const
{
    return d_type;
}

NTSCFG_INLINE
bool Host::isUndefined() const
{
    return (d_type == ntsa::HostType::e_UNDEFINED);
}

NTSCFG_INLINE
bool Host::isDomainName() const
{
    return (d_type == ntsa::HostType::e_DOMAIN_NAME);
}

NTSCFG_INLINE
bool Host::isIp() const
{
    return (d_type == ntsa::HostType::e_IP);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Host& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const Host& lhs, const Host& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const Host& lhs, const Host& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const Host& lhs, const Host& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Host& value)
{
    using bslh::hashAppend;

    if (value.isIp()) {
        hashAppend(algorithm, value.ip());
    }
    else if (value.isDomainName()) {
        hashAppend(algorithm, value.domainName());
    }
}

}  // close package namespace
}  // close enterprise namespace
#endif
