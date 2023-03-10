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

#ifndef INCLUDED_NTSA_IPV4ADDRESS
#define INCLUDED_NTSA_IPV4ADDRESS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an Internet Protocol version 4 address.
///
/// @details
/// Provide a value-semantic type that represents an Internet
/// Protocol version 4 address.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par Usage Example 1: Represent an IPv4 address
/// The following example illustrates how to construct a 'ntsa::Ipv4Address'
/// to represent an IPv4 address.
///
///     ntsa::Ipv4Address ipv4Address("127.0.0.1");
///     NTSCFG_TEST_EQ(ipv4Address, ntsa::Ipv4Address::loopback());
///
/// @ingroup module_ntsa_identity
class Ipv4Address
{
    /// This union describes the various ways to interpret the 4 bytes
    /// of an IPv4 address.
    union Representation {
        bsl::uint32_t d_asDword;
        bsl::uint8_t  d_asBytes[4];
    };

    Representation d_value;

  public:
    enum {
        MAX_TEXT_LENGTH = 15
        // The maximum required capacity of a buffer to store the longest
        // textual representation of an IPv4 addresses, not including the
        // null terminator.
    };

    /// Create a new IPv4 address having a default value.
    Ipv4Address();

    /// Create a new IPv4 address having the specified 'value' encoded in
    /// network byte order.
    explicit Ipv4Address(bsl::uint32_t value);

    /// Create a new IPv4 address parsed from the specified 'text'
    /// representation.
    explicit Ipv4Address(const bslstl::StringRef& text);

    /// Create a new IPv4 address having the same value as the specified
    /// 'other' object.
    Ipv4Address(const Ipv4Address& other);

    /// Destroy this object.
    ~Ipv4Address();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Ipv4Address& operator=(const Ipv4Address& other);

    /// Set the value of the object from the specified 'value' encoded in
    /// network byte order. Return a reference to this modifiable object.
    Ipv4Address& operator=(bsl::uint32_t value);

    /// Set the value of the object from the specified 'text'. Return
    /// a reference to this modifiable object.
    Ipv4Address& operator=(const bslstl::StringRef& text);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the value of this object from the value parsed from its textual
    /// representation. Return true if the 'text' is in a valid format and
    /// was parsed successfully, otherwise return false.
    bool parse(const bslstl::StringRef& text);

    /// Copy the representation of the IPv4 address from the specified
    /// 'source' having the specified 'size' to this object. Return the
    /// number of bytes read.
    bsl::size_t copyFrom(const void* source, bsl::size_t size);

    /// Return a reference to the modifiable byte at the specified 'index'.
    /// The behavior is undefined unless 'index < 4'.
    bsl::uint8_t& operator[](bsl::size_t index);

    /// Copy the value of this object to the representation in the specified
    /// 'destination' having the specified 'capacity'. Return the number of
    /// bytes written.
    bsl::size_t copyTo(void* destination, bsl::size_t capacity) const;

    /// Format the IPv4 address into the specified 'buffer' having the
    /// specified 'capacity'. Return the number of bytes written.
    bsl::size_t format(char* buffer, bsl::size_t capacity) const;

    /// Return the value of this object encoded in network byte order.
    bsl::uint32_t value() const;

    /// Return the textual representation of this object.
    bsl::string text() const;

    /// Return true if the IPv4 address is the wildcard address, otherwise
    /// return false.
    bool isAny() const;

    /// Return true if the IPv4 address identifies the loopback device,
    /// otherwise return false.
    bool isLoopback() const;

    /// Return true if the IPv4 address is a private address, otherwise
    /// return false.
    bool isPrivate() const;

    /// Return true if the IPv4 address is a link-local address, otherwise
    /// return false.
    bool isLinkLocal() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Ipv4Address& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Ipv4Address& other) const;

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

    /// Return the value of the byte at the specified 'index'. The behavior
    /// is undefined unless 'index < 4'.
    bsl::uint8_t operator[](bsl::size_t index) const;

    /// Return the wildcard address.
    static Ipv4Address any();

    /// Return the loopback address.
    static Ipv4Address loopback();

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(Ipv4Address);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::Ipv4Address
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv4Address& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4Address
bool operator==(const Ipv4Address& lhs, const Ipv4Address& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4Address
bool operator!=(const Ipv4Address& lhs, const Ipv4Address& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::Ipv4Address
bool operator<(const Ipv4Address& lhs, const Ipv4Address& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Ipv4Address
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Ipv4Address& value);

NTSCFG_INLINE
Ipv4Address::Ipv4Address()
{
    d_value.d_asDword = 0;
}

NTSCFG_INLINE
Ipv4Address::Ipv4Address(bsl::uint32_t value)
{
    d_value.d_asDword = value;
}

NTSCFG_INLINE
Ipv4Address::Ipv4Address(const Ipv4Address& other)
{
    d_value.d_asDword = other.d_value.d_asDword;
}

NTSCFG_INLINE
Ipv4Address::~Ipv4Address()
{
}

NTSCFG_INLINE
Ipv4Address& Ipv4Address::operator=(const Ipv4Address& other)
{
    d_value.d_asDword = other.d_value.d_asDword;
    return *this;
}

NTSCFG_INLINE
Ipv4Address& Ipv4Address::operator=(bsl::uint32_t value)
{
    d_value.d_asDword = value;
    return *this;
}

NTSCFG_INLINE
void Ipv4Address::reset()
{
    d_value.d_asDword = 0;
}

NTSCFG_INLINE
bsl::uint8_t& Ipv4Address::operator[](bsl::size_t index)
{
    BSLS_ASSERT(index < sizeof d_value);
    return d_value.d_asBytes[index];
}

NTSCFG_INLINE
bsl::uint32_t Ipv4Address::value() const
{
    return d_value.d_asDword;
}

NTSCFG_INLINE
bool Ipv4Address::isAny() const
{
    return (*this == Ipv4Address::any());
}

NTSCFG_INLINE
bool Ipv4Address::isLoopback() const
{
    return (*this == Ipv4Address::loopback());
}

NTSCFG_INLINE
bool Ipv4Address::isPrivate() const
{
    // 10.x.y.z.

    if (d_value.d_asBytes[0] == 10) {
        return true;
    }

    // 172.16.0.0 - 172.31.255.255

    if ((d_value.d_asBytes[0] == 172) && (d_value.d_asBytes[1] >= 16) &&
        (d_value.d_asBytes[1] <= 31))
    {
        return true;
    }

    // 192.168.0.0 - 192.168.255.255

    if ((d_value.d_asBytes[0] == 192) && (d_value.d_asBytes[1] == 168)) {
        return true;
    }

    return false;
}

NTSCFG_INLINE
bool Ipv4Address::isLinkLocal() const
{
    return (d_value.d_asBytes[0] == 169 && d_value.d_asBytes[1] == 254);
}

NTSCFG_INLINE
bool Ipv4Address::equals(const Ipv4Address& other) const
{
    return d_value.d_asDword == other.d_value.d_asDword;
}

NTSCFG_INLINE
bool Ipv4Address::less(const Ipv4Address& other) const
{
    bsl::uint32_t lc0 = BSLS_BYTEORDER_NTOHL(d_value.d_asDword);
    bsl::uint32_t rc0 = BSLS_BYTEORDER_NTOHL(other.d_value.d_asDword);

    return lc0 < rc0;
}

NTSCFG_INLINE
bsl::uint8_t Ipv4Address::operator[](bsl::size_t index) const
{
    BSLS_ASSERT(index < sizeof d_value);
    return d_value.d_asBytes[index];
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv4Address& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const Ipv4Address& lhs, const Ipv4Address& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const Ipv4Address& lhs, const Ipv4Address& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const Ipv4Address& lhs, const Ipv4Address& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Ipv4Address& value)
{
    using bslh::hashAppend;
    hashAppend(algorithm, value.value());
}

}  // close package namespace
}  // close enterprise namespace
#endif
