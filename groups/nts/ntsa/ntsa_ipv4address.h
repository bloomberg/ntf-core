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
/// This class is value-semantic type that represents an Internet Protocol
/// version 4 address.
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
    /// Declare constants used by this implementation.
    enum Constant {
        /// The maximum required capacity of a buffer to store the longest
        /// textual representation of an IPv4 addresses, not including the
        /// null terminator.
        MAX_TEXT_LENGTH = 15
    };

    /// Create a new IPv4 address having a default value.
    Ipv4Address() NTSCFG_NOEXCEPT;

    /// Create a new IPv4 address having the specified 'value' encoded in
    /// network byte order.
    explicit Ipv4Address(bsl::uint32_t value) NTSCFG_NOEXCEPT;

    /// Create a new IPv4 address parsed from the specified 'text'
    /// representation.
    explicit Ipv4Address(const bslstl::StringRef& text);

    /// Create a new IPv4 address having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    Ipv4Address(bslmf::MovableRef<Ipv4Address> original) NTSCFG_NOEXCEPT;

    /// Create a new IPv4 address having the same value as the specified
    /// 'original' object.
    Ipv4Address(const Ipv4Address& original) NTSCFG_NOEXCEPT;

    /// Destroy this object.
    ~Ipv4Address() NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    Ipv4Address& operator=(bslmf::MovableRef<Ipv4Address> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Ipv4Address& operator=(const Ipv4Address& other) NTSCFG_NOEXCEPT;

    /// Set the value of the object from the specified 'value' encoded in
    /// network byte order. Return a reference to this modifiable object.
    Ipv4Address& operator=(bsl::uint32_t value) NTSCFG_NOEXCEPT;

    /// Set the value of the object from the specified 'text'. Return
    /// a reference to this modifiable object.
    Ipv4Address& operator=(const bslstl::StringRef& text);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset() NTSCFG_NOEXCEPT;

    /// Set the value of this object from the value parsed from its textual
    /// representation. Return true if the 'text' is in a valid format and
    /// was parsed successfully, otherwise return false.
    bool parse(const bslstl::StringRef& text) NTSCFG_NOEXCEPT;

    /// Copy the representation of the IPv4 address from the specified
    /// 'source' having the specified 'size' to this object. Return the
    /// number of bytes read.
    bsl::size_t copyFrom(const void* source, bsl::size_t size) NTSCFG_NOEXCEPT;

    /// Return a reference to the modifiable byte at the specified 'index'.
    /// The behavior is undefined unless 'index < 4'.
    bsl::uint8_t& operator[](bsl::size_t index) NTSCFG_NOEXCEPT;

    /// Copy the value of this object to the representation in the specified
    /// 'destination' having the specified 'capacity'. Return the number of
    /// bytes written.
    bsl::size_t copyTo(void*       destination,
                       bsl::size_t capacity) const NTSCFG_NOEXCEPT;

    /// Format the IPv4 address into the specified 'buffer' having the
    /// specified 'capacity'. Return the number of bytes written.
    bsl::size_t format(char*       buffer,
                       bsl::size_t capacity) const NTSCFG_NOEXCEPT;

    /// Return the value of this object encoded in network byte order.
    bsl::uint32_t value() const NTSCFG_NOEXCEPT;

    /// Return the textual representation of this object.
    bsl::string text() const;

    /// Return true if the IPv4 address is the wildcard address, otherwise
    /// return false.
    bool isAny() const NTSCFG_NOEXCEPT;

    /// Return true if the IPv4 address identifies the loopback device,
    /// otherwise return false.
    bool isLoopback() const NTSCFG_NOEXCEPT;

    /// Return true if the IPv4 address is a private address, otherwise
    /// return false.
    bool isPrivate() const NTSCFG_NOEXCEPT;

    /// Return true if the IPv4 address is a link-local address, otherwise
    /// return false.
    bool isLinkLocal() const NTSCFG_NOEXCEPT;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Ipv4Address& other) const NTSCFG_NOEXCEPT;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Ipv4Address& other) const NTSCFG_NOEXCEPT;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

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
    bsl::uint8_t operator[](bsl::size_t index) const NTSCFG_NOEXCEPT;

    /// Return the wildcard address.
    static Ipv4Address any() NTSCFG_NOEXCEPT;

    /// Return the loopback address.
    static Ipv4Address loopback() NTSCFG_NOEXCEPT;

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(Ipv4Address);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(Ipv4Address);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(Ipv4Address);

    /// This type's equality-comparison operator is equivalent to comparing
    /// each byte of one comparand's footprint to each corresponding byte of
    /// the other comparand's footprint. Note that this trait implies that an
    /// object of this type has no padding bytes between data members.
    NTSCFG_TYPE_TRAIT_BITWISE_COMPARABLE(Ipv4Address);
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
bool operator==(const Ipv4Address& lhs, const Ipv4Address& rhs) NTSCFG_NOEXCEPT;

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Ipv4Address
bool operator!=(const Ipv4Address& lhs, const Ipv4Address& rhs) NTSCFG_NOEXCEPT;

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::Ipv4Address
bool operator<(const Ipv4Address& lhs, const Ipv4Address& rhs) NTSCFG_NOEXCEPT;

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Ipv4Address
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Ipv4Address& value);

NTSCFG_INLINE
Ipv4Address::Ipv4Address() NTSCFG_NOEXCEPT
{
    d_value.d_asDword = 0;
}

NTSCFG_INLINE
Ipv4Address::Ipv4Address(bsl::uint32_t value) NTSCFG_NOEXCEPT
{
    d_value.d_asDword = value;
}

NTSCFG_INLINE
Ipv4Address::Ipv4Address(bslmf::MovableRef<Ipv4Address> original)
    NTSCFG_NOEXCEPT
{
    d_value.d_asDword = NTSCFG_MOVE_FROM(original, d_value.d_asDword);
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
Ipv4Address::Ipv4Address(const Ipv4Address& original) NTSCFG_NOEXCEPT
{
    d_value.d_asDword = original.d_value.d_asDword;
}

NTSCFG_INLINE
Ipv4Address::~Ipv4Address() NTSCFG_NOEXCEPT
{
}

NTSCFG_INLINE
Ipv4Address& Ipv4Address::operator=(bslmf::MovableRef<Ipv4Address> other)
    NTSCFG_NOEXCEPT
{
    d_value.d_asDword = NTSCFG_MOVE_FROM(other, d_value.d_asDword);
    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
Ipv4Address& Ipv4Address::operator=(const Ipv4Address& other) NTSCFG_NOEXCEPT
{
    d_value.d_asDword = other.d_value.d_asDword;
    return *this;
}

NTSCFG_INLINE
Ipv4Address& Ipv4Address::operator=(bsl::uint32_t value) NTSCFG_NOEXCEPT
{
    d_value.d_asDword = value;
    return *this;
}

NTSCFG_INLINE
void Ipv4Address::reset() NTSCFG_NOEXCEPT
{
    d_value.d_asDword = 0;
}

NTSCFG_INLINE
bsl::uint8_t& Ipv4Address::operator[](bsl::size_t index) NTSCFG_NOEXCEPT
{
    BSLS_ASSERT(index < sizeof d_value);
    return d_value.d_asBytes[index];
}

NTSCFG_INLINE
bsl::uint32_t Ipv4Address::value() const NTSCFG_NOEXCEPT
{
    return d_value.d_asDword;
}

NTSCFG_INLINE
bool Ipv4Address::isAny() const NTSCFG_NOEXCEPT
{
    return (*this == Ipv4Address::any());
}

NTSCFG_INLINE
bool Ipv4Address::isLoopback() const NTSCFG_NOEXCEPT
{
    return (*this == Ipv4Address::loopback());
}

NTSCFG_INLINE
bool Ipv4Address::isPrivate() const NTSCFG_NOEXCEPT
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
bool Ipv4Address::isLinkLocal() const NTSCFG_NOEXCEPT
{
    return (d_value.d_asBytes[0] == 169 && d_value.d_asBytes[1] == 254);
}

NTSCFG_INLINE
bool Ipv4Address::equals(const Ipv4Address& other) const NTSCFG_NOEXCEPT
{
    return d_value.d_asDword == other.d_value.d_asDword;
}

NTSCFG_INLINE
bool Ipv4Address::less(const Ipv4Address& other) const NTSCFG_NOEXCEPT
{
    bsl::uint32_t lc0 = BSLS_BYTEORDER_NTOHL(d_value.d_asDword);
    bsl::uint32_t rc0 = BSLS_BYTEORDER_NTOHL(other.d_value.d_asDword);

    return lc0 < rc0;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void Ipv4Address::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, BSLS_BYTEORDER_NTOHL(d_value.d_asDword));
}

NTSCFG_INLINE
bsl::uint8_t Ipv4Address::operator[](bsl::size_t index) const NTSCFG_NOEXCEPT
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
bool operator==(const Ipv4Address& lhs, const Ipv4Address& rhs) NTSCFG_NOEXCEPT
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const Ipv4Address& lhs, const Ipv4Address& rhs) NTSCFG_NOEXCEPT
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const Ipv4Address& lhs, const Ipv4Address& rhs) NTSCFG_NOEXCEPT
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&    algorithm,
                              const Ipv4Address& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
