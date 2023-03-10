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

#ifndef INCLUDED_NTSA_IPV6ENDPOINT
#define INCLUDED_NTSA_IPV6ENDPOINT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ipv6address.h>
#include <ntsa_port.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a tuple of an IPv6 address and a port number.
///
/// @details
/// Provide a value-semantic type that represents a tuple of an
/// IPv6 address and a port number.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par Usage Example 1: Represent an IPv6 address and port number
/// The following example illustrates how to construct a 'ntsa::Ipv6Endpoint'
/// to represent an IPv6 address and port number.
///
///     ntsa::Ipv6Endpoint ipv6Endpoint("[::1]]:12345");
///
///     NTSCFG_TEST_EQ(ipv6Endpoint.host(), ntsa::Ipv6Address::loopback());
///     NTSCFG_TEST_EQ(ipv6Endpoint.port(), 12345);
///
/// @ingroup module_ntsa_identity
class Ipv6Endpoint
{
    ntsa::Ipv6Address d_host;
    ntsa::Port        d_port;

  public:
    enum {
        /// The maximum required capacity of a buffer to store the longest
        /// textual representation of anIPv6 address, including the IPv6
        /// scope ID, if any, enclosed by brackets, if necessary for IPv6
        /// addresses, followed by a colon, followed by the port number, but
        /// not including the null terminator.
        MAX_TEXT_LENGTH = (1) + (39 + 4) + (1) + (1) + (5)
    };

    /// Create a new IP endpoint having a default value.
    Ipv6Endpoint();

    /// Create a new IP endpoint having the specified IPv6 'address' and
    /// 'port' number.
    explicit Ipv6Endpoint(const ntsa::Ipv6Address& address, ntsa::Port port);

    /// Create a new IP endpoint parsed from the specified 'text'
    /// representation.
    explicit Ipv6Endpoint(const bslstl::StringRef& text);

    /// Create a new IP endpoint parsed from the specified address 'text'
    /// representation and specified 'port' number.
    explicit Ipv6Endpoint(const bslstl::StringRef& addressText,
                          ntsa::Port               port);

    /// Create a new IP endpoint having the same value as the specified
    /// 'other' object.
    Ipv6Endpoint(const Ipv6Endpoint& other);

    /// Destroy this object.
    ~Ipv6Endpoint();

    /// Assign the value of the specified 'other' primitive representation
    /// to this primitive representation.
    Ipv6Endpoint& operator=(const Ipv6Endpoint& other);

    /// Set the value of the object from the specified 'text'.
    Ipv6Endpoint& operator=(const bslstl::StringRef& text);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the value of this object from the value parsed from any of its
    /// textual representations. Return true if the 'text' is in a valid
    /// format and was parsed successfully, otherwise return false.
    bool parse(const bslstl::StringRef& text);

    /// Set the address to the specified 'value'.
    void setHost(const ntsa::Ipv6Address& value);

    /// Set the address to the specified 'addressText' textual
    /// representation.
    void setHost(const bslstl::StringRef& addressText);

    /// Set the port to the specified 'value'.
    void setPort(ntsa::Port value);

    /// Return the address.
    const ntsa::Ipv6Address& host() const;

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
    bool equals(const Ipv6Endpoint& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Ipv6Endpoint& other) const;

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

    /// Return the IPv6 address that represents any address.
    static ntsa::Ipv6Address anyIpv6Address();

    /// Return the IPv6 address that represents the loopback address.
    static ntsa::Ipv6Address loopbackIpv6Address();

    /// Return the port number that represents any port.
    static ntsa::Port anyPort();

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(Ipv6Endpoint);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::Ipv6Endpoint
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv6Endpoint& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Ipv6Endpoint
bool operator==(const Ipv6Endpoint& lhs, const Ipv6Endpoint& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Ipv6Endpoint
bool operator!=(const Ipv6Endpoint& lhs, const Ipv6Endpoint& rhs);

/// Return true if the specified 'lhs' is less than the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::Ipv6Endpoint
bool operator<(const Ipv6Endpoint& lhs, const Ipv6Endpoint& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Ipv6Endpoint
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Ipv6Endpoint& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Ipv6Endpoint& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.host());
    hashAppend(algorithm, value.port());
}

}  // close package namespace
}  // close enterprise namespace
#endif
