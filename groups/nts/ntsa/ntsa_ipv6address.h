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

#ifndef INCLUDED_NTSA_IPV6ADDRESS
#define INCLUDED_NTSA_IPV6ADDRESS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ipv6scopeid.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsls_byteorder.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an Internet Protocol version 6 address.
///
/// @details
/// Provide a value-semantic type that represents an Internet
/// Protocol version 6 address.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par Usage Example 1: Represent an IPv6 address
/// The following example illustrates how to construct a 'ntsa::Ipv6Address'
/// to represent an IPv6 address.
///
///     ntsa::Ipv6Address ipv6Address("::1");
///     NTSCFG_TEST_EQ(ipv6Address, ntsa::Ipv6Address::loopback());
///
/// @ingroup module_ntsa_identity
class Ipv6Address
{
    /// This union describes the various ways to interpret the 16 bytes
    /// of an IPv6 address.
    union Representation {
        bsl::uint64_t d_byQword[2];
        bsl::uint16_t d_bySword[8];
        bsl::uint8_t  d_asBytes[16];
    };

    Representation    d_value;
    ntsa::Ipv6ScopeId d_scopeId;

  public:
    enum {
        /// The maximum required capacity of a buffer to store the longest
        /// textual representation of an IPv6 addresses, including the scope
        /// ID, but not including the null terminator.
        MAX_TEXT_LENGTH = 39 + 4
    };

    /// Create a new IPv6 address having a default value.
    Ipv6Address() NTSCFG_NOEXCEPT;

    /// Create a new IPv6 address parsed from the specified 'text'
    /// representation.
    explicit Ipv6Address(const bslstl::StringRef& text);

    /// Create a new IPv4 address having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    Ipv6Address(bslmf::MovableRef<Ipv6Address> original) NTSCFG_NOEXCEPT;

    /// Create a new IPv6 address having the same value as the specified
    /// 'original' object.
    Ipv6Address(const Ipv6Address& original) NTSCFG_NOEXCEPT;

    /// Destroy this object.
    ~Ipv6Address() NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    Ipv6Address& operator=(bslmf::MovableRef<Ipv6Address> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Ipv6Address& operator=(const Ipv6Address& other) NTSCFG_NOEXCEPT;

    /// Set the value of the object from the specified 'text'.
    Ipv6Address& operator=(const bslstl::StringRef& text);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset() NTSCFG_NOEXCEPT;

    /// Set the value of this object from the value parsed from its textual
    /// representation. Return true if the 'text' is in a valid format and
    /// was parsed successfully, otherwise return false.
    bool parse(const bslstl::StringRef& text) NTSCFG_NOEXCEPT;

    /// Copy the representation of the IPv6 address from the specified
    /// 'source' having the specified 'size' to this object. Return the
    /// number of bytes read.
    bsl::size_t copyFrom(const void* source, bsl::size_t size) NTSCFG_NOEXCEPT;

    /// Return a reference to the modifiable byte at the specified 'index'.
    /// The behavior is undefined unless 'index < 16'.
    bsl::uint8_t& operator[](bsl::size_t index) NTSCFG_NOEXCEPT;

    /// Set the scope ID of the IPv6 address to the specified 'scopeId'.
    void setScopeId(ntsa::Ipv6ScopeId scopeId) NTSCFG_NOEXCEPT;

    /// Copy the value of this object to the representation in the specified
    /// 'destination' having the specified 'capacity'. Return the number of
    /// bytes written.
    bsl::size_t copyTo(void*       destination,
                       bsl::size_t capacity) const NTSCFG_NOEXCEPT;

    /// Format the IPv6 address into the specified 'buffer' having the
    /// specified 'capacity'. If the specified 'collapse' flag is true,
    /// collapse longesta successive runs of result matching the regular
    /// expression '/(^0|:)[:0]{2,}/' with "::", turning the result into the
    /// canonical textual representation of the address. Return the number
    /// of bytes written.
    bsl::size_t format(char*       buffer,
                       bsl::size_t capacity,
                       bool        collapse = true) const NTSCFG_NOEXCEPT;

    /// Return the textual representation of this object.
    bsl::string text() const;

    /// Return the value by quad-word at the specified 'index'. The behavior
    /// is undefined unless 'index' < 2.
    bsl::uint64_t byQword(bsl::size_t index) const NTSCFG_NOEXCEPT;

    /// Return true if the IPv6 address is the wildcard address, otherwise
    /// return false.
    bool isAny() const NTSCFG_NOEXCEPT;

    /// Return true if the IPv6 address identifies the loopback device,
    /// otherwise return false.
    bool isLoopback() const NTSCFG_NOEXCEPT;

    /// Return true if the IPv6 address is a private address, otherwise
    /// return false.
    bool isPrivate() const NTSCFG_NOEXCEPT;

    /// Return true if the IPv6 address is a link-local address, otherwise
    /// return false.
    bool isLinkLocal() const NTSCFG_NOEXCEPT;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Ipv6Address& other) const NTSCFG_NOEXCEPT;

    /// Return true if this object has the same value as the specified
    /// 'other' object, without considering the scope ID, otherwise return
    /// false.
    bool equalsScopeless(const Ipv6Address& other) const NTSCFG_NOEXCEPT;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Ipv6Address& other) const NTSCFG_NOEXCEPT;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, without considering the scope ID,
    /// otherwise return false.
    bool lessScopeless(const Ipv6Address& other) const NTSCFG_NOEXCEPT;

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
    /// is undefined unless 'index < 16'.
    bsl::uint8_t operator[](bsl::size_t index) const NTSCFG_NOEXCEPT;

    /// Return the scope ID of the IPv6 address.
    ntsa::Ipv6ScopeId scopeId() const NTSCFG_NOEXCEPT;

    /// Return the wildcard address.
    static Ipv6Address any() NTSCFG_NOEXCEPT;

    /// Return the loopback address.
    static Ipv6Address loopback() NTSCFG_NOEXCEPT;

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(Ipv6Address);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(Ipv6Address);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(Ipv6Address);

    /// This type's equality-comparison operator is equivalent to comparing
    /// each byte of one comparand's footprint to each corresponding byte of
    /// the other comparand's footprint. Note that this trait implies that an
    /// object of this type has no padding bytes between data members.
    NTSCFG_TYPE_TRAIT_BITWISE_COMPARABLE(Ipv6Address);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::Ipv6Address
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv6Address& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Ipv6Address
bool operator==(const Ipv6Address& lhs,
                const Ipv6Address& rhs) NTSCFG_NOEXCEPT;

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Ipv6Address
bool operator!=(const Ipv6Address& lhs,
                const Ipv6Address& rhs) NTSCFG_NOEXCEPT;

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::Ipv6Address
bool operator<(const Ipv6Address& lhs, const Ipv6Address& rhs) NTSCFG_NOEXCEPT;

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Ipv6Address
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Ipv6Address& value);

// GCC 9 warnings about possibly uninitialized data members of
// 'ntsa::Ipv6Address' in the constructor of 'ntca::GetDomainNameContext',
// but this appears spurious.

#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

NTSCFG_INLINE
Ipv6Address::Ipv6Address() NTSCFG_NOEXCEPT
{
    d_value.d_byQword[0] = 0;
    d_value.d_byQword[1] = 0;
    d_scopeId            = 0;
}

NTSCFG_INLINE
Ipv6Address::Ipv6Address(bslmf::MovableRef<Ipv6Address> original)
    NTSCFG_NOEXCEPT
{
    d_value.d_byQword[0] = NTSCFG_MOVE_FROM(original, d_value.d_byQword[0]);
    d_value.d_byQword[1] = NTSCFG_MOVE_FROM(original, d_value.d_byQword[1]);
    d_scopeId            = NTSCFG_MOVE_FROM(original, d_scopeId);

    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
Ipv6Address::Ipv6Address(const Ipv6Address& original) NTSCFG_NOEXCEPT
{
    d_value.d_byQword[0] = original.d_value.d_byQword[0];
    d_value.d_byQword[1] = original.d_value.d_byQword[1];
    d_scopeId            = original.d_scopeId;
}

NTSCFG_INLINE
Ipv6Address::~Ipv6Address() NTSCFG_NOEXCEPT
{
}

#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic pop
#endif

NTSCFG_INLINE
Ipv6Address& Ipv6Address::operator=(bslmf::MovableRef<Ipv6Address> other)
    NTSCFG_NOEXCEPT
{
    d_value.d_byQword[0] = NTSCFG_MOVE_FROM(other, d_value.d_byQword[0]);
    d_value.d_byQword[1] = NTSCFG_MOVE_FROM(other, d_value.d_byQword[1]);
    d_scopeId            = NTSCFG_MOVE_FROM(other, d_scopeId);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
Ipv6Address& Ipv6Address::operator=(const Ipv6Address& other) NTSCFG_NOEXCEPT
{
    d_value.d_byQword[0] = other.d_value.d_byQword[0];
    d_value.d_byQword[1] = other.d_value.d_byQword[1];
    d_scopeId            = other.d_scopeId;

    return *this;
}

NTSCFG_INLINE
void Ipv6Address::reset() NTSCFG_NOEXCEPT
{
    d_value.d_byQword[0] = 0;
    d_value.d_byQword[1] = 0;
    d_scopeId            = 0;
}

NTSCFG_INLINE
bsl::uint8_t& Ipv6Address::operator[](bsl::size_t index) NTSCFG_NOEXCEPT
{
    BSLS_ASSERT(index < sizeof d_value);
    return d_value.d_asBytes[index];
}

NTSCFG_INLINE
void Ipv6Address::setScopeId(ntsa::Ipv6ScopeId scopeId) NTSCFG_NOEXCEPT
{
    d_scopeId = scopeId;
}

NTSCFG_INLINE
bool Ipv6Address::isAny() const NTSCFG_NOEXCEPT
{
    return this->equals(Ipv6Address::any());
}

NTSCFG_INLINE
bool Ipv6Address::isLoopback() const NTSCFG_NOEXCEPT
{
    return this->equals(Ipv6Address::loopback());
}

NTSCFG_INLINE
bool Ipv6Address::isPrivate() const NTSCFG_NOEXCEPT
{
    return (d_value.d_asBytes[0] == 0xFD && d_value.d_asBytes[1] == 0x00);
}

NTSCFG_INLINE
bool Ipv6Address::isLinkLocal() const NTSCFG_NOEXCEPT
{
    return (d_value.d_asBytes[0] == 0xFE && d_value.d_asBytes[1] == 0x80);
}

NTSCFG_INLINE
bool Ipv6Address::equals(const Ipv6Address& other) const NTSCFG_NOEXCEPT
{
    return d_value.d_byQword[0] == other.d_value.d_byQword[0] &&
           d_value.d_byQword[1] == other.d_value.d_byQword[1] &&
           d_scopeId == other.d_scopeId;
}

NTSCFG_INLINE
bool Ipv6Address::equalsScopeless(const Ipv6Address& other) const
    NTSCFG_NOEXCEPT
{
    return d_value.d_byQword[0] == other.d_value.d_byQword[0] &&
           d_value.d_byQword[1] == other.d_value.d_byQword[1];
}

NTSCFG_INLINE
bool Ipv6Address::less(const Ipv6Address& other) const NTSCFG_NOEXCEPT
{
    bsl::uint64_t lc0 = BSLS_BYTEORDER_NTOHLL(d_value.d_byQword[0]);
    bsl::uint64_t rc0 = BSLS_BYTEORDER_NTOHLL(other.d_value.d_byQword[0]);

    if (lc0 < rc0) {
        return true;
    }

    if (lc0 > rc0) {
        return false;
    }

    bsl::uint64_t lc1 = BSLS_BYTEORDER_NTOHLL(d_value.d_byQword[1]);
    bsl::uint64_t rc1 = BSLS_BYTEORDER_NTOHLL(other.d_value.d_byQword[1]);

    if (lc1 < rc1) {
        return true;
    }

    if (lc1 > rc1) {
        return false;
    }

    return d_scopeId < other.d_scopeId;
}

NTSCFG_INLINE
bool Ipv6Address::lessScopeless(const Ipv6Address& other) const NTSCFG_NOEXCEPT
{
    bsl::uint64_t lc0 = BSLS_BYTEORDER_NTOHLL(d_value.d_byQword[0]);
    bsl::uint64_t rc0 = BSLS_BYTEORDER_NTOHLL(other.d_value.d_byQword[0]);

    if (lc0 < rc0) {
        return true;
    }

    if (lc0 > rc0) {
        return false;
    }

    bsl::uint64_t lc1 = BSLS_BYTEORDER_NTOHLL(d_value.d_byQword[1]);
    bsl::uint64_t rc1 = BSLS_BYTEORDER_NTOHLL(other.d_value.d_byQword[1]);

    return lc1 < rc1;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void Ipv6Address::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_value.d_byQword[0]);
    hashAppend(algorithm, d_value.d_byQword[1]);
}

NTSCFG_INLINE
bsl::uint8_t Ipv6Address::operator[](bsl::size_t index) const NTSCFG_NOEXCEPT
{
    BSLS_ASSERT(index < sizeof d_value);
    return d_value.d_asBytes[index];
}

NTSCFG_INLINE
ntsa::Ipv6ScopeId Ipv6Address::scopeId() const NTSCFG_NOEXCEPT
{
    return d_scopeId;
}

NTSCFG_INLINE
bsl::uint64_t Ipv6Address::byQword(bsl::size_t index) const NTSCFG_NOEXCEPT
{
    BSLS_ASSERT(index < 2);
    return d_value.d_byQword[index];
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Ipv6Address& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const Ipv6Address& lhs, const Ipv6Address& rhs) NTSCFG_NOEXCEPT
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const Ipv6Address& lhs, const Ipv6Address& rhs) NTSCFG_NOEXCEPT
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const Ipv6Address& lhs, const Ipv6Address& rhs) NTSCFG_NOEXCEPT
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Ipv6Address& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
