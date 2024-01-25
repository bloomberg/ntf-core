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

/// Provide an enumeration of the status of a zero-copy operation.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_system
struct ZeroCopyType {
  public:
    /// Provide an enumeration of the status of a zero-copy operation.
    enum Value {
        /// The copy was avoided.
        e_AVOIDED = 0,

        /// The copy was deferred from the time of the system call until
        /// nearer to the time of transmission but a deep copy was still 
        /// performed.
        e_DEFERRED = 1
    };

    /// Return the string representation exactly matching the enumerator name
    /// corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the specified
    /// 'string'.  Return 0 on success, and a non-zero value with no effect on
    /// 'result' otherwise (i.e., 'string' does not match any enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified 'result' the enumerator matching the specified
    /// 'number'.  Return 0 on success, and a non-zero value with no effect on
    /// 'result' otherwise (i.e., 'number' does not match any enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntsa::ZeroCopyType
bsl::ostream& operator<<(bsl::ostream& stream, ZeroCopyType::Value rhs);

/// Describe a notification for the completion of a closed range of send
/// operations with zero-copy semantics.
///
/// @par Attributes
/// This class is composed of the following attributes:
///
/// @li @b from:
/// The identifier of the first zero-copy send that completed, inclusive.
///
/// @li @b thru:
/// The identifier of the last zero-copy send that completed, inclusive.
///
/// @li @b type:
/// The status of the zero-copy operation. This enumerated indicates whether
/// a copy was avoided or was deferred from the time of the system call until
/// later, more nearer to the time of transmission.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class ZeroCopy
{
    bsl::uint32_t             d_from;
    bsl::uint32_t             d_thru;
    ntsa::ZeroCopyType::Value d_type;

  public:
    /// Create a new zero-copy interval having the default value.
    ZeroCopy();

    /// Create a new '[from, thru]' zero-copy interval completed according to
    /// the specified 'type'. 
    ZeroCopy(bsl::uint32_t             from, 
             bsl::uint32_t             thru, 
             ntsa::ZeroCopyType::Value type);

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
    void setThru(bsl::uint32_t value);

    /// Set the type indicating whether the copy was avoided or was performed
    /// to the specified 'value'.
    void setType(ntsa::ZeroCopyType::Value value);

    /// Return the identifier of the first zero-copy send that completed, 
    /// inclusive.
    BSLS_ANNOTATION_NODISCARD bsl::uint32_t from() const;

    /// Return the identifier of the last zero-copy send that completed, 
    /// inclusive.
    BSLS_ANNOTATION_NODISCARD bsl::uint32_t thru() const;

    /// Return the type indicating whether the copy was avoided or was 
    /// performed.
    BSLS_ANNOTATION_NODISCARD ntsa::ZeroCopyType::Value type() const;

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

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
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

/// Return true if the value of the specified 'lhs' is less than the value of
/// the specified 'rhs', otherwise return false.
///
/// @related ntsa::ZeroCopy
bool operator<(const ZeroCopy& lhs, const ZeroCopy& rhs);

/// Contribute the values of the salient attributes of the specified 'value' to
/// the specified hash 'algorithm'.
///
/// @related ntsa::ZeroCopy
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ZeroCopy& value);

NTSCFG_INLINE
ZeroCopy::ZeroCopy()
: d_from(0)
, d_thru(0)
, d_type(ntsa::ZeroCopyType::e_AVOIDED)
{
}

NTSCFG_INLINE
ZeroCopy::ZeroCopy(bsl::uint32_t             from, 
                   bsl::uint32_t             thru, 
                   ntsa::ZeroCopyType::Value type)
: d_from(from)
, d_thru(thru)
, d_type(type)
{
}

NTSCFG_INLINE
ZeroCopy::ZeroCopy(const ZeroCopy& original)
: d_from(original.d_from)
, d_thru(original.d_thru)
, d_type(original.d_type)
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
    d_thru = other.d_thru;
    d_type = other.d_type;
    return *this;
}

NTSCFG_INLINE
void ZeroCopy::setFrom(bsl::uint32_t value)
{
    d_from = value;
}

NTSCFG_INLINE
void ZeroCopy::setThru(bsl::uint32_t value)
{
    d_thru = value;
}

NTSCFG_INLINE
void ZeroCopy::setType(ntsa::ZeroCopyType::Value value)
{
    d_type = value;
}

NTSCFG_INLINE
bsl::uint32_t ZeroCopy::from() const
{
    return d_from;
}

NTSCFG_INLINE
bsl::uint32_t ZeroCopy::thru() const
{
    return d_thru;
}

NTSCFG_INLINE
ntsa::ZeroCopyType::Value ZeroCopy::type() const
{
    return d_type;
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
    hashAppend(algorithm, value.thru());
    hashAppend(algorithm, value.type());
}

}  // close package namespace
}  // close enterprise namespace
#endif
