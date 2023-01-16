// Copyright 2023 Bloomberg Finance L.P.
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

#ifndef INCLUDED_NTSA_TIMESTAMP
#define INCLUDED_NTSA_TIMESTAMP

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsscm_version.h>

#include <bslh_hash.h>
#include <bsls_timeinterval.h>

#include <ntsa_timestamptype.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a type holding a transmit timestamp.
///
/// @details
/// Provide a value-semantic type that holds a timestamp.
///
/// @par Attributes
/// This class is composed of the following attributes..
///
/// @li @b type:
/// Type of the timestamp. It describes source of the timestamp. Default value
/// is e_UNDEFINED.
///
/// @li @b time:
/// The timestamp value. Default value is 0.
///
/// @par Thread Safety
/// This class is not thread safe.
class Timestamp
{
    ntsa::TimestampType::Value d_type;
    bsl::uint32_t              d_id;
    bsls::TimeInterval         d_time;

  public:
    /// Create new timestamp having the default value.
    Timestamp();

    /// Create new timestamp having the same value as the specified
    /// 'original' object.
    Timestamp(const Timestamp& original);

    /// Destroy this object.
    ~Timestamp();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Timestamp& operator=(const Timestamp& other);

    /// Set timestamp type to the specifued 'value'.
    void setType(ntsa::TimestampType::Value value);

    /// Set id of the timestamp to the specified 'value'.
    void setId(bsl::uint32_t value);

    /// Set timestamp time to the specified 'value'.
    void setTime(const bsls::TimeInterval& value);

    /// Return type of the timestamp.
    ntsa::TimestampType::Value type() const;

    /// Get id of the timestamp.
    bsl::uint32_t id() const;

    /// Return time of the timestamp.
    const bsls::TimeInterval& time() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Timestamp& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Timestamp& other) const;

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
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(Timestamp);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::Timestamp
bsl::ostream& operator<<(bsl::ostream& stream, const Timestamp& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Timestamp
bool operator==(const Timestamp& lhs, const Timestamp& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Timestamp
bool operator!=(const Timestamp& lhs, const Timestamp& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::Timestamp
bool operator<(const Timestamp& lhs, const Timestamp& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Timestamp
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Timestamp& value);

NTSCFG_INLINE
Timestamp::Timestamp()
: d_type()
, d_id()
, d_time()
{
}

NTSCFG_INLINE
Timestamp::Timestamp(const Timestamp& original)
: d_type(original.d_type)
, d_id(original.d_id)
, d_time(original.d_time)
{
}

NTSCFG_INLINE
Timestamp::~Timestamp()
{
}

NTSCFG_INLINE
Timestamp& Timestamp::operator=(const Timestamp& other)
{
    d_type = other.d_type;
    d_id   = other.d_id;
    d_time = other.d_time;
    return *this;
}

NTSCFG_INLINE
void Timestamp::setType(ntsa::TimestampType::Value value)
{
    d_type = value;
}

NTSCFG_INLINE
void Timestamp::setId(bsl::uint32_t value)
{
    d_id = value;
}

NTSCFG_INLINE
void Timestamp::setTime(const bsls::TimeInterval& value)
{
    d_time = value;
}

NTSCFG_INLINE
ntsa::TimestampType::Value Timestamp::type() const
{
    return d_type;
}

NTSCFG_INLINE
bsl::uint32_t Timestamp::id() const
{
    return d_id;
}

NTSCFG_INLINE
const bsls::TimeInterval& Timestamp::time() const
{
    return d_time;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Timestamp& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const Timestamp& lhs, const Timestamp& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const Timestamp& lhs, const Timestamp& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const Timestamp& lhs, const Timestamp& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Timestamp& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.type());
    hashAppend(algorithm, value.id());
    hashAppend(algorithm, value.time());
}

}  // close package namespace
}  // close enterprise namespace
#endif
