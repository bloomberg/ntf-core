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

#ifndef INCLUDED_NTSA_IPENDPOINT
#define INCLUDED_NTSA_IPENDPOINT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ipaddress.h>
#include <ntsa_port.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a tuple of an IP address and a port number.
///
/// @details
/// Provide a value-semantic type that represents a tuple of an IP
/// address and a port number.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par Usage Example 1: Represent an IPv4 address and port number
/// The following example illustrates how to construct a 'ntsa::IpEndpoint'
/// to represent an IPv4 address and port number.
///
///     ntsa::IpEndpoint ipEndpoint("127.0.0.1:12345");
///
///     NTSCFG_TEST_TRUE(ipEndpoint.host().isV4());
///     NTSCFG_TEST_EQ(ipEndpoint.host().v4(), ntsa::Ipv4Address::loopback());
///     NTSCFG_TEST_EQ(ipEndpoint.port(), 12345);
///
/// @par Example 2: Represent an IPv4 address and port number
/// The following example illustrates how to construct a 'ntsa::IpEndpoint'
/// to represent an IPv6 address and port number.
///
///     ntsa::IpEndpoint ipEndpoint("[::1]:12345");
///
///     NTSCFG_TEST_TRUE(ipEndpoint.host().isV6());
///     NTSCFG_TEST_EQ(ipEndpoint.host().v6(), ntsa::Ipv6Address::loopback());
///     NTSCFG_TEST_EQ(ipEndpoint.port(), 12345);
///
/// @ingroup module_ntsa_identity
class IpEndpoint
{
    ntsa::IpAddress d_host;
    ntsa::Port      d_port;

  public:
    enum {
        /// The maximum required capacity of a buffer to store the longest
        /// textual representation of an IPv4 or IPv6 address, including the
        /// IPv6 scope ID, if any, enclosed by brackets, if necessary for
        /// IPv6 addresses, followed by a colon followed by the port number,
        /// but not including the null terminator.
        MAX_TEXT_LENGTH = (1) + (39 + 4) + (1) + (1) + (5)
    };

    /// Create a new IP endpoint having a default value.
    IpEndpoint();

    /// Create a new IP endpoint having the specified IP 'address' and
    /// 'port' number.
    explicit IpEndpoint(const ntsa::IpAddress& address, ntsa::Port port);

    /// Create a new IP endpoint having the specified IPv4 'address' and
    /// 'port' number.
    explicit IpEndpoint(const ntsa::Ipv4Address& address, ntsa::Port port);

    /// Create a new IP endpoint having the specified IPv6 'address' and
    /// 'port' number.
    explicit IpEndpoint(const ntsa::Ipv6Address& address, ntsa::Port port);

    /// Create a new IP endpoint parsed from the specified 'text'
    /// representation.
    explicit IpEndpoint(const bslstl::StringRef& text);

    /// Create a new IP endpoint parsed from the specified 'addressText'
    /// representation and specified 'port' number.
    explicit IpEndpoint(const bslstl::StringRef& addressText, ntsa::Port port);

    /// Create a new IP endpoint having the same value as the specified
    /// 'other' object.
    IpEndpoint(const IpEndpoint& other);

    /// Destroy this object.
    ~IpEndpoint();

    /// Assign the value of the specified 'other' primitive representation
    /// to this primitive representation.
    IpEndpoint& operator=(const IpEndpoint& other);

    /// Set the value of the object from the specified 'text'.
    IpEndpoint& operator=(const bslstl::StringRef& text);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the value of this object from the value parsed from any of its
    /// textual representations. Return true if the 'text' is in a valid
    /// format and was parsed successfully, otherwise return false.
    bool parse(const bslstl::StringRef& text);

    /// Set the address to the specified 'value'.
    void setHost(const ntsa::IpAddress& value);

    /// Set the address to the specified 'value'.
    void setHost(const ntsa::Ipv4Address& value);

    /// Set the address to the specified 'value'.
    void setHost(const ntsa::Ipv6Address& value);

    /// Set the address to the specified 'addressText' textual
    /// representation.
    void setHost(const bslstl::StringRef& addressText);

    /// Set the port to the specified 'value'.
    void setPort(ntsa::Port value);

    /// Return the address.
    const ntsa::IpAddress& host() const;

    /// Return the port.
    ntsa::Port port() const;

    /// Format the endpoint into the specified 'buffer' having the specified
    /// 'capacity'. If the specified 'collapse' flag is true and the address
    /// is an IPv6 address, collapse longest successive runs of the result
    /// matching the regular expression '/(^0|:)[:0]{2,}/' with "::",
    /// turning the result into the canonical textual representation of the
    /// address.  Return the number of bytes written.
    bsl::size_t format(char*       buffer,
                       bsl::size_t capacity,
                       bool        collapse = true) const;

    /// Return the textual representation of this object.
    bsl::string text() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const IpEndpoint& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const IpEndpoint& other) const;

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
    static ntsa::IpAddress anyIpv4Address();

    /// Return the IPv6 address that represents any address.
    static ntsa::IpAddress anyIpv6Address();

    /// Return the IPv4 address that represents the loopback address.
    static ntsa::IpAddress loopbackIpv4Address();

    /// Return the IPv6 address that represents the loopback address.
    static ntsa::IpAddress loopbackIpv6Address();

    /// Return the address of the specified 'addressType' that represents
    /// any address.
    static ntsa::IpAddress anyAddress(ntsa::IpAddressType::Value addressType);

    /// Return the address of the specified 'addressType' that represents
    /// the loopback address.
    static ntsa::IpAddress loopbackAddress(
        ntsa::IpAddressType::Value addressType);

    /// Return the port number that represents any port.
    static ntsa::Port anyPort();

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(IpEndpoint);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::IpEndpoint
bsl::ostream& operator<<(bsl::ostream& stream, const IpEndpoint& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::IpEndpoint
bool operator==(const IpEndpoint& lhs, const IpEndpoint& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::IpEndpoint
bool operator!=(const IpEndpoint& lhs, const IpEndpoint& rhs);

/// Return true if the specified 'lhs' is less than the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::IpEndpoint
bool operator<(const IpEndpoint& lhs, const IpEndpoint& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::IpEndpoint
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IpEndpoint& value);

NTSCFG_INLINE
IpEndpoint::IpEndpoint()
: d_host()
, d_port(0)
{
}

NTSCFG_INLINE
IpEndpoint::IpEndpoint(const ntsa::IpAddress& address, ntsa::Port port)
: d_host(address)
, d_port(port)
{
}

NTSCFG_INLINE
IpEndpoint::IpEndpoint(const ntsa::Ipv4Address& address, ntsa::Port port)
: d_host(address)
, d_port(port)
{
}

NTSCFG_INLINE
IpEndpoint::IpEndpoint(const ntsa::Ipv6Address& address, ntsa::Port port)
: d_host(address)
, d_port(port)
{
}

NTSCFG_INLINE
IpEndpoint::IpEndpoint(const IpEndpoint& other)
: d_host(other.d_host)
, d_port(other.d_port)
{
}

NTSCFG_INLINE
IpEndpoint::~IpEndpoint()
{
}

NTSCFG_INLINE
IpEndpoint& IpEndpoint::operator=(const IpEndpoint& other)
{
    if (this != &other) {
        d_host = other.d_host;
        d_port = other.d_port;
    }

    return *this;
}

NTSCFG_INLINE
void IpEndpoint::reset()
{
    d_host.reset();
    d_port = 0;
}

NTSCFG_INLINE
void IpEndpoint::setHost(const ntsa::IpAddress& value)
{
    d_host = value;
}

NTSCFG_INLINE
void IpEndpoint::setHost(const ntsa::Ipv4Address& value)
{
    d_host = value;
}

NTSCFG_INLINE
void IpEndpoint::setHost(const ntsa::Ipv6Address& value)
{
    d_host = value;
}

NTSCFG_INLINE
void IpEndpoint::setHost(const bslstl::StringRef& addressText)
{
    d_host = addressText;
}

NTSCFG_INLINE
void IpEndpoint::setPort(ntsa::Port value)
{
    d_port = value;
}

NTSCFG_INLINE
const ntsa::IpAddress& IpEndpoint::host() const
{
    return d_host;
}

NTSCFG_INLINE
ntsa::Port IpEndpoint::port() const
{
    return d_port;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const IpEndpoint& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.host());
    hashAppend(algorithm, value.port());
}

}  // close package namespace
}  // close enterprise namespace
#endif
