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

#ifndef INCLUDED_NTCA_COMPRESSIONCONFIG
#define INCLUDED_NTCA_COMPRESSIONCONFIG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_compressiongoal.h>
#include <ntca_compressiontype.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe the configurable parameters to deflate and inflate data
/// according to a compression algorithm.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b type:
/// The compression algorithm. If not specified, the "zlib" algorithm is used.
///
/// @li @b goal:
/// The compression goal, indicating whether the user favors size over speed,
/// or speed over size. If not specified, the default value is a balanced goal
/// that favors neither size nor speed.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_todo
class CompressionConfig
{
    bdlb::NullableValue<ntca::CompressionType::Value> d_type;
    bdlb::NullableValue<ntca::CompressionGoal::Value> d_goal;

  public:
    /// Create new deflate options having the default value.
    CompressionConfig();

    /// Create new deflate options having the same value as the specified
    /// 'original' object.
    CompressionConfig(const CompressionConfig& original);

    /// Destroy this object.
    ~CompressionConfig();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    CompressionConfig& operator=(const CompressionConfig& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the compression algorithm to the specified 'value'.
    void setType(ntca::CompressionType::Value value);

    /// Set the compression goal to the specified 'value'.
    void setGoal(ntca::CompressionGoal::Value value);

    /// Return the compression algorithm.
    const bdlb::NullableValue<ntca::CompressionType::Value>& type() const;

    /// Return the compression goal.
    const bdlb::NullableValue<ntca::CompressionGoal::Value>& goal() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const CompressionConfig& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const CompressionConfig& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

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

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(CompressionConfig);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(CompressionConfig);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::CompressionConfig
bsl::ostream& operator<<(bsl::ostream&            stream,
                         const CompressionConfig& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::CompressionConfig
bool operator==(const CompressionConfig& lhs, const CompressionConfig& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::CompressionConfig
bool operator!=(const CompressionConfig& lhs, const CompressionConfig& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::CompressionConfig
bool operator<(const CompressionConfig& lhs, const CompressionConfig& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::CompressionConfig
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const CompressionConfig& value);

NTSCFG_INLINE
CompressionConfig::CompressionConfig()
: d_type()
, d_goal()
{
}

NTSCFG_INLINE
CompressionConfig::CompressionConfig(const CompressionConfig& original)
: d_type(original.d_type)
, d_goal(original.d_goal)
{
}

NTSCFG_INLINE
CompressionConfig::~CompressionConfig()
{
}

NTSCFG_INLINE
CompressionConfig& CompressionConfig::operator=(const CompressionConfig& other)
{
    d_goal = other.d_goal;
    d_type = other.d_type;
    return *this;
}

NTSCFG_INLINE
void CompressionConfig::reset()
{
    d_type.reset();
    d_goal.reset();
}

NTSCFG_INLINE
void CompressionConfig::setType(ntca::CompressionType::Value value)
{
    d_type = value;
}

NTSCFG_INLINE
void CompressionConfig::setGoal(ntca::CompressionGoal::Value value)
{
    d_goal = value;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntca::CompressionType::Value>& CompressionConfig::
    type() const
{
    return d_type;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntca::CompressionGoal::Value>& CompressionConfig::
    goal() const
{
    return d_goal;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void CompressionConfig::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_type);
    hashAppend(algorithm, d_goal);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const CompressionConfig& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const CompressionConfig& lhs, const CompressionConfig& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const CompressionConfig& lhs, const CompressionConfig& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const CompressionConfig& lhs, const CompressionConfig& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&          algorithm,
                              const CompressionConfig& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
