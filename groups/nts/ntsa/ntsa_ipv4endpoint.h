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

#ifndef INCLUDED_NTSA_IPV4ENDPOINT
#define INCLUDED_NTSA_IPV4ENDPOINT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ipv4address.h>
#include <ntsa_port.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a tuple of an IPv4 address and a port number.
///
/// @details
/// Provide a value-semantic type that represents a tuple of an
/// IPv4 address and a port number.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par Usage Example 1: Represent an IPv4 address and port number
/// The following example illustrates how to construct a 'ntsa::Ipv4Endpoint'
/// to represent an IPv4 address and port number.
///
///     ntsa::Ipv4Endpoint ipv4Endpoint("127.0.0.1:12345");
///
///     NTSCFG_TEST_EQ(ipv4Endpoint.host(), ntsa::Ipv4Address::loopback());
///     NTSCFG_TEST_EQ(ipv4Endpoint.port(), 12345);
///
/// @ingroup module_ntsa_identity
class Ipv4Endpoint
{
    ntsa::Ipv4Address d_host;
    ntsa::Port        d_port;

    class Impl;

  public:
    enum {
        /// The maximum required capacity of a buffer to store the longest
        /// textual representation of an IPv4 address, followed by a colon,
        /// followed by the port number, but not including the null
        /// terminator.
        MAX_TEXT_LENGTH = (15) + (1) + (5)
    };

    /// Create a new IPv4 endpoint having a default value.
    Ipv4Endpoint();

    /// Create a new IPv4 endpoint having the specified IPv4 'address' and
    /// 'port' number.
    explicit Ipv4Endpoint(const ntsa::Ipv4Address& address, ntsa::Port port);

    /// Create a new IPv4 endpoint parsed from the specified 'text'
    /// representation.
    explicit Ipv4Endpoint(const bslstl::StringRef& text);

    /// Create a new IPv4 endpoint parsed from the specified 'addressText'
    /// representation and specified 'port' number.
    Ipv4Endpoint(const bslstl::StringRef& addressText, ntsa::Port port);

    /// Create a new IPv4 endpoint having the same value as the specified
    /// 'other' object.
    Ipv4Endpoint(const Ipv4Endpoint& other);

    /// Destroy this object.
    ~Ipv4Endpoint();

    /// Assign the value of the specified 'other' primitive representation
    /// to this primitive representation.
    Ipv4Endpoint& operator=(const Ipv4Endpoint& other);

    /// Set the value of the object from the specified 'text'.
    Ipv4Endpoint& operator=(const bslstl::StringRef& text);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the value of this object from the value parsed from any of its
    /// textual representations. Return true if the 'text' is in a valid
    /// format and was parsed successfully, otherwise return false.
    bool parse(const bslstl::StringRef& text);

    /// Set the address to the specified 'value'.
    void setHost(const ntsa::Ipv4Address& value);

    /// Set the address to the specified 'addressText' textual
    /// representation.
    void setHost(const bslstl::StringRef& addressText);

    /// Set the port to the specified 'value'.
    void setPort(ntsa::Port value);

    /// Return the address.
    const ntsa::Ipv4Address& host() const;

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
    bool equals(const Ipv4Endpoint& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Ipv4Endpoint& other) const;

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
    static ntsa::Ipv4Address anyIpv4Address();

    /// Return the IPv4 address that represents the loopback address.
    static ntsa::Ipv4Address loopbackIpv4Address();

    /// Return the port number that represents any port.
    static ntsa::Port anyPort();

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(Ipv4Endpoint);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(Ipv4Endpoint);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::Ipv4Endpoint
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv4Endpoint& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4Endpoint
bool operator==(const Ipv4Endpoint& lhs, const Ipv4Endpoint& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4Endpoint
bool operator!=(const Ipv4Endpoint& lhs, const Ipv4Endpoint& rhs);

/// Return true if the specified 'lhs' is less than the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::Ipv4Endpoint
bool operator<(const Ipv4Endpoint& lhs, const Ipv4Endpoint& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Ipv4Endpoint
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Ipv4Endpoint& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Ipv4Endpoint& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.host());
    hashAppend(algorithm, value.port());
}

}  // close package namespace
}  // close enterprise namespace
#endif
