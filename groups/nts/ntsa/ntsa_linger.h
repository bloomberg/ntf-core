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

#ifndef INCLUDED_NTSA_LINGER
#define INCLUDED_NTSA_LINGER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsls_timeinterval.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a description of the linger behavior after socket shutdown.
///
/// @details
/// Provide a value-semantic type that describes the linger
/// behavior after socket shutdown.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b enabled:
/// The flag that indicates the operating system should gracefully attempt to
/// transmit any data remaining in the socket send buffer before closing the
/// connection.
///
/// @li @b duration:
/// The maximum amount of time to linger.
//
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class Linger
{
    bool               d_enabled;
    bsls::TimeInterval d_duration;

  public:
    /// Create new send options having the default value.
    Linger();

    /// Create new send options having the same value as the specified
    /// 'original' object.
    Linger(const Linger& original);

    /// Destroy this object.
    ~Linger();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Linger& operator=(const Linger& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the flag that indicates the operating system should gracefully
    /// attempt to transmit any data remaining in the send buffer before
    /// closing the connection to the specified 'value'.
    void setEnabled(bool value);

    /// Set the maximum amount of time to linger to specified 'value'.
    void setDuration(const bsls::TimeInterval& value);

    /// Return the the flag that indicates the operating system should
    /// gracefully attempt to transmit any data remaining in the send buffer
    /// before closing the connection.
    bool enabled() const;

    /// Return the maximum amount of time to linger.
    const bsls::TimeInterval& duration() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Linger& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Linger& other) const;

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
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(Linger);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::Linger
bsl::ostream& operator<<(bsl::ostream& stream, const Linger& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Linger
bool operator==(const Linger& lhs, const Linger& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Linger
bool operator!=(const Linger& lhs, const Linger& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::Linger
bool operator<(const Linger& lhs, const Linger& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Linger
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Linger& value);

NTSCFG_INLINE
Linger::Linger()
: d_enabled(false)
, d_duration()
{
}

NTSCFG_INLINE
Linger::Linger(const Linger& original)
: d_enabled(original.d_enabled)
, d_duration(original.d_duration)
{
}

NTSCFG_INLINE
Linger::~Linger()
{
}

NTSCFG_INLINE
Linger& Linger::operator=(const Linger& other)
{
    d_enabled  = other.d_enabled;
    d_duration = other.d_duration;
    return *this;
}

NTSCFG_INLINE
void Linger::reset()
{
    d_enabled  = false;
    d_duration = bsls::TimeInterval();
}

NTSCFG_INLINE
void Linger::setEnabled(bool value)
{
    d_enabled = value;
}

NTSCFG_INLINE
void Linger::setDuration(const bsls::TimeInterval& value)
{
    d_duration = value;
}

NTSCFG_INLINE
bool Linger::enabled() const
{
    return d_enabled;
}

NTSCFG_INLINE
const bsls::TimeInterval& Linger::duration() const
{
    return d_duration;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Linger& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const Linger& lhs, const Linger& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const Linger& lhs, const Linger& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const Linger& lhs, const Linger& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Linger& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.enabled());
    hashAppend(algorithm, value.duration());
}

}  // close package namespace
}  // close enterprise namespace
#endif
