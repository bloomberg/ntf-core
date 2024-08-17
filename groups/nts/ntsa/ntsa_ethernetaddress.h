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

#ifndef INCLUDED_NTSA_ETHERNETADDRESS
#define INCLUDED_NTSA_ETHERNETADDRESS

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

/// Provide an Ethernet address.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_identity
class EthernetAddress
{
    bsl::uint8_t d_value[6];

public:
    /// Declare constants used by this implementation.
    enum Constant {
        /// The maximum required capacity of a buffer to store the longest
        /// textual representation of an Ethernet addresses, not including the
        /// null terminator.
        MAX_TEXT_LENGTH = 17
    };

    /// Create a new Ethernet address with a default value.
    EthernetAddress();

    /// Create a new IPv4 address parsed from the specified 'text'
    /// representation.
    explicit EthernetAddress(const bslstl::StringRef& text);

    /// Create a new Ethernet address with the specified 'byte0', 'byte1',
    /// 'byte2', 'byte3', 'byte4', and 'byte5' value.
    EthernetAddress(bsl::uint8_t byte0,
                    bsl::uint8_t byte1,
                    bsl::uint8_t byte2,
                    bsl::uint8_t byte3,
                    bsl::uint8_t byte4,
                    bsl::uint8_t byte5);

    /// Create a new Ethernet address having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    EthernetAddress(
        bslmf::MovableRef<EthernetAddress> original) NTSCFG_NOEXCEPT;

    /// Create a new Ethernet address with the same value as the specified
    /// 'original' object.
    EthernetAddress(const EthernetAddress& original);

    /// Destroy this object.
    ~EthernetAddress();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    EthernetAddress& operator=(bslmf::MovableRef<EthernetAddress> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    EthernetAddress& operator=(const EthernetAddress& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    // Set the value of this object from the value parsed from any of its
    /// textual representations. Return true if the 'text' is in a valid
    /// format and was parsed successfully, otherwise return false. If false
    /// is returned then the value of this object was set to its value upon
    /// default construction.
    bool parse(const bslstl::StringRef& text);

    /// Copy the representation of the Ethernet address from the specified
    /// 'source' having the specified 'size' to this object. Return the
    /// number of bytes read.
    bsl::size_t copyFrom(const void* source, bsl::size_t size);

    /// Return a reference to the modifiable byte at the specified 'index'.
    /// The behavior is undefined unless 'index < 6'.
    bsl::uint8_t& operator[](bsl::size_t index);

    /// Copy the value of this object to the representation in the specified
    /// 'destination' having the specified 'capacity'. Return the number of
    /// bytes written.
    bsl::size_t copyTo(void*       destination,
                       bsl::size_t capacity) const;

    /// Format the IPv4 address into the specified 'buffer' having the
    /// specified 'capacity'. Return the number of bytes written.
    bsl::size_t format(char*       buffer,
                       bsl::size_t capacity) const;

    /// Return the string representation of this object.
    bsl::string text() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EthernetAddress& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EthernetAddress& other) const;

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
    bsl::uint8_t operator[](bsl::size_t index) const;

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(EthernetAddress);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(EthernetAddress);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(EthernetAddress);

    /// This type's equality-comparison operator is equivalent to comparing
    /// each byte of one comparand's footprint to each corresponding byte of
    /// the other comparand's footprint. Note that this trait implies that an
    /// object of this type has no padding bytes between data members.
    NTSCFG_TYPE_TRAIT_BITWISE_COMPARABLE(EthernetAddress);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::EthernetAddress
bsl::ostream& operator<<(bsl::ostream& stream, const EthernetAddress& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::EthernetAddress
bool operator==(const EthernetAddress& lhs, const EthernetAddress& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::EthernetAddress
bool operator!=(const EthernetAddress& lhs, const EthernetAddress& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::EthernetAddress
bool operator<(const EthernetAddress& lhs, const EthernetAddress& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::EthernetAddress
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EthernetAddress& value);

NTSCFG_INLINE
EthernetAddress::EthernetAddress()
{
    bsl::memset(d_value, 0, sizeof d_value);
}

NTSCFG_INLINE
EthernetAddress::EthernetAddress(bsl::uint8_t byte0,
                                 bsl::uint8_t byte1,
                                 bsl::uint8_t byte2,
                                 bsl::uint8_t byte3,
                                 bsl::uint8_t byte4,
                                 bsl::uint8_t byte5)
{
    d_value[0] = byte0;
    d_value[1] = byte1;
    d_value[2] = byte2;
    d_value[3] = byte3;
    d_value[4] = byte4;
    d_value[5] = byte5;
}

NTSCFG_INLINE
EthernetAddress::EthernetAddress(
    bslmf::MovableRef<EthernetAddress> original) NTSCFG_NOEXCEPT
{
    bsl::memcpy(d_value, NTSCFG_MOVE_FROM(original, d_value), sizeof d_value);
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
EthernetAddress::EthernetAddress(const EthernetAddress& original)
{
    bsl::memcpy(d_value, original.d_value, sizeof d_value);
}

NTSCFG_INLINE
EthernetAddress::~EthernetAddress()
{
}

NTSCFG_INLINE
EthernetAddress& EthernetAddress::operator=(bslmf::MovableRef<EthernetAddress> other)
    NTSCFG_NOEXCEPT
{
    bsl::memcpy(d_value, NTSCFG_MOVE_FROM(other, d_value), sizeof d_value);
    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
EthernetAddress& EthernetAddress::operator=(const EthernetAddress& other)
{
    if (this != &other) {
        bsl::memcpy(d_value, other.d_value, sizeof d_value);
    }

    return *this;
}

NTSCFG_INLINE
void EthernetAddress::reset()
{
    bsl::memset(d_value, 0, sizeof d_value);
}

NTSCFG_INLINE
bsl::uint8_t& EthernetAddress::operator[](bsl::size_t index)
{
    BSLS_ASSERT(index < sizeof d_value);
    return d_value[index];
}

NTSCFG_INLINE
bsl::string EthernetAddress::text() const
{
    char              buffer[ntsa::EthernetAddress::MAX_TEXT_LENGTH + 1];
    const bsl::size_t size = EthernetAddress::format(buffer, sizeof buffer);

    return bsl::string(buffer, buffer + size);
}

NTSCFG_INLINE
bool EthernetAddress::equals(const EthernetAddress& other) const
{
    return bsl::memcmp(d_value, other.d_value, sizeof d_value) == 0;
}

NTSCFG_INLINE
bool EthernetAddress::less(const EthernetAddress& other) const
{
    return bsl::memcmp(d_value, other.d_value, sizeof d_value) < 0;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE
void EthernetAddress::hash(HASH_ALGORITHM& algorithm) const
{
    algorithm(reinterpret_cast<const char*>(d_value), sizeof d_value);
}

NTSCFG_INLINE
bsl::uint8_t EthernetAddress::operator[](bsl::size_t index) const
{
    BSLS_ASSERT(index < sizeof d_value);
    return d_value[index];
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const EthernetAddress& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const EthernetAddress& lhs, const EthernetAddress& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const EthernetAddress& lhs, const EthernetAddress& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const EthernetAddress& lhs, const EthernetAddress& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE
void hashAppend(HASH_ALGORITHM& algorithm, const EthernetAddress& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
