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

#ifndef INCLUDED_NTCS_WATERMARKS
#define INCLUDED_NTCS_WATERMARKS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Describe the low and high watermarks of a queue.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b low:
/// The low watermark of a queue.
///
/// @li @b high:
/// The high watermark of a queue.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class Watermarks
{
    bsl::size_t d_current;
    bsl::size_t d_low;
    bsl::size_t d_high;

  public:
    /// Create new send options having the default value.
    Watermarks();

    /// Create new send options having the same value as the specified
    /// 'original' object.
    Watermarks(const Watermarks& original);

    /// Destroy this object.
    ~Watermarks();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Watermarks& operator=(const Watermarks& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the current watermark to the specified 'value'.
    void setCurrent(bsl::size_t value);

    /// Set the low watermark to the specified 'value'.
    void setLow(bsl::size_t value);

    /// Set the high watermark to the specified 'value'.
    void setHigh(bsl::size_t value);

    /// Return the current watermark.
    bsl::size_t current() const;

    /// Return the low watermark.
    bsl::size_t low() const;

    /// Return the high watermark.
    bsl::size_t high() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Watermarks& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Watermarks& other) const;

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
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(Watermarks);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntcs::Watermarks
bsl::ostream& operator<<(bsl::ostream& stream, const Watermarks& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntcs::Watermarks
bool operator==(const Watermarks& lhs, const Watermarks& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntcs::Watermarks
bool operator!=(const Watermarks& lhs, const Watermarks& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntcs::Watermarks
bool operator<(const Watermarks& lhs, const Watermarks& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntcs::Watermarks
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Watermarks& value);

NTCCFG_INLINE
Watermarks::Watermarks()
: d_current(0)
, d_low(0)
, d_high(bsl::numeric_limits<bsl::size_t>::max())
{
}

NTCCFG_INLINE
Watermarks::Watermarks(const Watermarks& original)
: d_current(original.d_current)
, d_low(original.d_low)
, d_high(original.d_high)
{
}

NTCCFG_INLINE
Watermarks::~Watermarks()
{
}

NTCCFG_INLINE
Watermarks& Watermarks::operator=(const Watermarks& other)
{
    d_current = other.d_current;
    d_low     = other.d_low;
    d_high    = other.d_high;
    return *this;
}

NTCCFG_INLINE
void Watermarks::reset()
{
    d_current = 0;
    d_low     = 0;
    d_high    = bsl::numeric_limits<bsl::size_t>::max();
}

NTCCFG_INLINE
void Watermarks::setCurrent(bsl::size_t value)
{
    d_current = value;
}

NTCCFG_INLINE
void Watermarks::setLow(bsl::size_t value)
{
    d_low = value;
}

NTCCFG_INLINE
void Watermarks::setHigh(bsl::size_t value)
{
    d_high = value;
}

NTCCFG_INLINE
bsl::size_t Watermarks::current() const
{
    return d_current;
}

NTCCFG_INLINE
bsl::size_t Watermarks::low() const
{
    return d_low;
}

NTCCFG_INLINE
bsl::size_t Watermarks::high() const
{
    return d_high;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Watermarks& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const Watermarks& lhs, const Watermarks& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const Watermarks& lhs, const Watermarks& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const Watermarks& lhs, const Watermarks& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Watermarks& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.current());
    hashAppend(algorithm, value.low());
    hashAppend(algorithm, value.high());
}

}  // close package namespace
}  // close enterprise namespace
#endif
