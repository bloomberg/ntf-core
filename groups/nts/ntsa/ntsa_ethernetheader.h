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

#ifndef INCLUDED_NTSA_ETHERNETHEADER
#define INCLUDED_NTSA_ETHERNETHEADER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ethernetaddress.h>
#include <ntsa_ethernetprotocol.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an Ethernet header.
///
/// @details
/// This class is value-semantic type that represents an Ethernet header.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_identity
class EthernetHeader
{
    ntsa::EthernetAddress         d_source;
    ntsa::EthernetAddress         d_destination;
    ntsa::EthernetProtocol::Value d_protocol;

  public:
    /// Create a new IPv4 address having a default value.
    EthernetHeader();

    /// Create a new IPv4 address having the same value as the specified
    /// 'original' object. Assign an unspecified but valid value to the
    /// 'original' original.
    EthernetHeader(bslmf::MovableRef<EthernetHeader> original) NTSCFG_NOEXCEPT;

    /// Create a new IPv4 address having the same value as the specified
    /// 'original' object.
    EthernetHeader(const EthernetHeader& original);

    /// Destroy this object.
    ~EthernetHeader();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    EthernetHeader& operator=(bslmf::MovableRef<EthernetHeader> other)
        NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EthernetHeader& operator=(const EthernetHeader& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the source address to the specified 'value'. 
    void setSource(const ntsa::EthernetAddress& value);

    /// Set the destination address to the specified 'value'. 
    void setDestination(const ntsa::EthernetAddress& value);

    /// Set the protocol to the specified 'value'. 
    void setProtocol(ntsa::EthernetProtocol::Value value);

    /// Return the source address. 
    const ntsa::EthernetAddress& source() const;

    /// Return the destination address. 
    const ntsa::EthernetAddress& destination() const;

    /// Return the protocol.
    ntsa::EthernetProtocol::Value protocol() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EthernetHeader& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EthernetHeader& other) const;

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

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(EthernetHeader);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(EthernetHeader);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(EthernetHeader);
};

/// Write a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::EthernetHeader
bsl::ostream& operator<<(bsl::ostream& stream, const EthernetHeader& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::EthernetHeader
bool operator==(const EthernetHeader& lhs, const EthernetHeader& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::EthernetHeader
bool operator!=(const EthernetHeader& lhs, const EthernetHeader& rhs);

/// Return true if the specified 'lhs' is "less than" the specified 'rhs',
/// otherwise return false.
///
/// @related ntsa::EthernetHeader
bool operator<(const EthernetHeader& lhs, const EthernetHeader& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::EthernetHeader
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EthernetHeader& value);

NTSCFG_INLINE
EthernetHeader::EthernetHeader()
: d_source()
, d_destination()
, d_protocol(ntsa::EthernetProtocol::e_UNDEFINED)
{
}

NTSCFG_INLINE
EthernetHeader::EthernetHeader(
    bslmf::MovableRef<EthernetHeader> original) NTSCFG_NOEXCEPT
: d_source(NTSCFG_MOVE_FROM(original, d_source))
, d_destination(NTSCFG_MOVE_FROM(original, d_destination))
, d_protocol(NTSCFG_MOVE_FROM(original, d_protocol))
{
    NTSCFG_MOVE_RESET(original);
}

NTSCFG_INLINE
EthernetHeader::EthernetHeader(const EthernetHeader& original)
: d_source(original.d_source)
, d_destination(original.d_destination)
, d_protocol(original.d_protocol)
{
}

NTSCFG_INLINE
EthernetHeader::~EthernetHeader()
{
}

NTSCFG_INLINE
EthernetHeader& EthernetHeader::operator=(
    bslmf::MovableRef<EthernetHeader> other) NTSCFG_NOEXCEPT
{
    d_source      = NTSCFG_MOVE_FROM(other, d_source);
    d_destination = NTSCFG_MOVE_FROM(other, d_destination);
    d_protocol    = NTSCFG_MOVE_FROM(other, d_protocol);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
EthernetHeader& EthernetHeader::operator=(const EthernetHeader& other)
{
    d_source = other.d_source;
    d_destination = other.d_destination;
    d_protocol = other.d_protocol;
    return *this;
}

NTSCFG_INLINE
void EthernetHeader::reset()
{
    d_source.reset();
    d_destination.reset();
    d_protocol = ntsa::EthernetProtocol::e_UNDEFINED;
}

NTSCFG_INLINE
void EthernetHeader::setSource(const ntsa::EthernetAddress& value)
{
    d_source = value;
}

NTSCFG_INLINE
void EthernetHeader::setDestination(const ntsa::EthernetAddress& value)
{
    d_destination = value;
}

NTSCFG_INLINE
void EthernetHeader::setProtocol(ntsa::EthernetProtocol::Value value)
{
    d_protocol = value;
}

NTSCFG_INLINE
const ntsa::EthernetAddress& EthernetHeader::source() const
{
    return d_source;
}

NTSCFG_INLINE
const ntsa::EthernetAddress& EthernetHeader::destination() const
{
    return d_destination;
}

NTSCFG_INLINE
ntsa::EthernetProtocol::Value EthernetHeader::protocol() const
{
    return d_protocol;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void EthernetHeader::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_source);
    hashAppend(algorithm, d_destination);
    hashAppend(algorithm, d_protocol);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const EthernetHeader& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const EthernetHeader& lhs, const EthernetHeader& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const EthernetHeader& lhs, const EthernetHeader& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const EthernetHeader& lhs, const EthernetHeader& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&    algorithm,
                              const EthernetHeader& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
