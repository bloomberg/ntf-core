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

#ifndef INCLUDED_NTSA_ZEROCOPY
#define INCLUDED_NTSA_ZEROCOPY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

/// Describe a notification for the completion of one or more send operations
/// with zero-copy semantics.
///
/// @par Attributes
/// This class is composed of the following attributes:
///
/// @li @b from:
/// The identifier of the first zero-copy send that completed, inclusive.
///
/// @li @b to:
/// The identifier of the last zero-copy send that completed, inclusive.
///
/// @li @b code:
/// The code indicating whether the copy was avoided or was performed.
///
/// @par Thread Safety
/// This class is not thread safe.
class ZeroCopy
{
    bsl::uint32_t d_from;
    bsl::uint32_t d_to;
    bsl::uint8_t  d_code;

  public:
    /// Create a new instance having the default value.
    ZeroCopy();

    /// Create a new instance using the specified 'from', the specified 'to'
    /// and the specified 'code' as initial values.
    ZeroCopy(bsl::uint32_t from, bsl::uint32_t to, bsl::uint8_t code);

    /// Create new object having the same value as the specified 'original'
    /// object.
    ZeroCopy(const ZeroCopy& original);

    /// Destroy this object.
    ~ZeroCopy();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    ZeroCopy& operator=(const ZeroCopy& other);

    /// Set the identifier of the first zero-copy send that completed, 
    /// inclusive, to the specified 'value'. 
    void setFrom(bsl::uint32_t value);

    /// Set the identifier of the last zero-copy send that completed, 
    /// inclusive, to the specified 'value'. 
    void setTo(bsl::uint32_t value);

    /// Set the code indicating whether the copy was avoided or was performed
    /// to the specified 'value'.
    void setCode(bsl::uint8_t value);

    /// Return the identifier of the first zero-copy send that completed, 
    /// inclusive.
    BSLS_ANNOTATION_NODISCARD bsl::uint32_t from() const;

    /// Return the identifier of the last zero-copy send that completed, 
    /// inclusive.
    BSLS_ANNOTATION_NODISCARD bsl::uint32_t to() const;

    /// Return the code indicating whether the copy was avoided or was 
    /// performed.
    BSLS_ANNOTATION_NODISCARD bsl::uint8_t code() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    BSLS_ANNOTATION_NODISCARD bool equals(const ZeroCopy& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    BSLS_ANNOTATION_NODISCARD bool less(const ZeroCopy& other) const;

    /// Format this object to the specified output 'stream' at the optionally
    /// specified indentation 'level' and return a reference to the modifiable
    /// 'stream'.  If 'level' is specified, optionally specify
    /// 'spacesPerLevel', the number of spaces per indentation level for this
    /// and all of its nested objects.  Each line is indented by the absolute
    /// value of 'level * spacesPerLevel'.  If 'level' is negative, suppress
    /// indentation of the first line.  If 'spacesPerLevel' is negative,
    /// suppress line breaks and format the entire output on one line.  If
    /// 'stream' is initially invalid, this operation has no effect.  Note that
    /// a trailing newline is provided in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(ZeroCopy);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::ZeroCopy
bsl::ostream& operator<<(bsl::ostream& stream, const ZeroCopy& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::ZeroCopy
bool operator==(const ZeroCopy& lhs, const ZeroCopy& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::ZeroCopy
bool operator!=(const ZeroCopy& lhs, const ZeroCopy& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::ZeroCopy
bool operator<(const ZeroCopy& lhs, const ZeroCopy& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::ZeroCopy
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ZeroCopy& value);

NTSCFG_INLINE
ZeroCopy::ZeroCopy()
: d_from(0)
, d_to(0)
, d_code(0)
{
}

NTSCFG_INLINE
ZeroCopy::ZeroCopy(bsl::uint32_t from, bsl::uint32_t to, bsl::uint8_t code)
: d_from(from)
, d_to(to)
, d_code(code)
{
}

NTSCFG_INLINE
ZeroCopy::ZeroCopy(const ZeroCopy& original)
: d_from(original.d_from)
, d_to(original.d_to)
, d_code(original.d_code)
{
}

NTSCFG_INLINE
ZeroCopy::~ZeroCopy()
{
}

NTSCFG_INLINE
ZeroCopy& ZeroCopy::operator=(const ZeroCopy& other)
{
    d_from = other.d_from;
    d_to   = other.d_to;
    d_code = other.d_code;
    return *this;
}

NTSCFG_INLINE
void ZeroCopy::setFrom(bsl::uint32_t value)
{
    d_from = value;
}

NTSCFG_INLINE
void ZeroCopy::setTo(bsl::uint32_t value)
{
    d_to = value;
}

NTSCFG_INLINE
void ZeroCopy::setCode(bsl::uint8_t value)
{
    d_code = value;
}

NTSCFG_INLINE
bsl::uint32_t ZeroCopy::from() const
{
    return d_from;
}

NTSCFG_INLINE
bsl::uint32_t ZeroCopy::to() const
{
    return d_to;
}

NTSCFG_INLINE
bsl::uint8_t ZeroCopy::code() const
{
    return d_code;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ZeroCopy& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const ZeroCopy& lhs, const ZeroCopy& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const ZeroCopy& lhs, const ZeroCopy& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const ZeroCopy& lhs, const ZeroCopy& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ZeroCopy& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.from());
    hashAppend(algorithm, value.to());
    hashAppend(algorithm, value.code());
}

}  // close package namespace
}  // close enterprise namespace
#endif
