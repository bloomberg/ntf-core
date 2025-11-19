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

#ifndef INCLUDED_NTSA_BACKOFF
#define INCLUDED_NTSA_BACKOFF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_config.h>
#include <ntscfg_platform.h>
#include <bdlb_nullablevalue.h>
#include <bsls_timeinterval.h>
#include <bsl_variant.h>

namespace BloombergLP {
namespace ntsa {

/// Describe the parameters to the calculation of backoff (or iterative
/// increase) of the interval between individual attempts of an operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b arithmetic:
/// The parameter of arithmetic progression.
///
/// @li @b geometric:
/// The parameter of geometric progression.
///
/// @li @b minJitter:
/// The minimum jitter.
///
/// @li @b maxJitter:
/// The maximum jitter.
///
/// @li @b limit:
/// The maximum value.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class Backoff
{
    typedef bsl::variant<bsls::TimeInterval, double> Progression;

    Progression                             d_progression;
    bsls::TimeInterval                      d_minJitter;
    bsls::TimeInterval                      d_maxJitter;
    bdlb::NullableValue<bsls::TimeInterval> d_limit;

  public:
    /// Create a new backoff with the default value.
    Backoff();

    /// Create a new backoff having the same value as the specified 'original'
    /// object.
    Backoff(const Backoff& original);

    /// Destroy this object.
    ~Backoff();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    Backoff& operator=(const Backoff& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the arithmetic difference between intervals to the specified 'value'.
    void makeArithmetic(const bsls::TimeInterval& value);

    /// Set the multiplier between intervals to the specified 'value'.
    void makeGeometric(double value);

    /// Set the minimum jitter to the specified 'value'.
    void setMinJitter(const bsls::TimeInterval& value);

    /// Set the maximum jitter to the specified 'value'.
    void setMaxJitter(const bsls::TimeInterval& value);

    /// Set the limit to the specified 'value'.
    void setLimit(const bsls::TimeInterval& value);

    /// Return the arithmetic difference between intervals, or an unset value
    /// if the progression is not arithmetic.
    bdlb::NullableValue<bsls::TimeInterval> arithmetic() const;

    /// Return the multiplier between intervals to the specified 'value', or
    /// an unset value if the progression is not geometric.
    bdlb::NullableValue<double> geometric() const;

    /// Return the minimum jitter.
    const bsls::TimeInterval& minJitter() const;

    /// Return the maximum jitter.
    const bsls::TimeInterval& maxJitter() const;

    /// Return the limit.
    const bdlb::NullableValue<bsls::TimeInterval>& limit() const;

    /// Apply this backoff to the specified 'value' and return the result.
    bsls::TimeInterval apply(const bsls::TimeInterval& value) const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const Backoff& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const Backoff& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(Backoff);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(Backoff);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntsa::Backoff
bsl::ostream& operator<<(bsl::ostream& stream, const Backoff& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntsa::Backoff
bool operator==(const Backoff& lhs, const Backoff& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntsa::Backoff
bool operator!=(const Backoff& lhs, const Backoff& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::Backoff
bool operator<(const Backoff& lhs, const Backoff& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Backoff
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Backoff& value);

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void Backoff::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;

    if (bsl::holds_alternative<bsls::TimeInterval>(d_progression)) {
        hashAppend(algorithm, bsl::get<bsls::TimeInterval>(d_progression));
    }
    else if (bsl::holds_alternative<double>(d_progression)) {
        hashAppend(algorithm, bsl::get<double>(d_progression));
    }

    hashAppend(algorithm, d_minJitter);
    hashAppend(algorithm, d_maxJitter);
    hashAppend(algorithm, d_limit);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM& algorithm, const Backoff& value)
{
    value.hash(algorithm);
}

}  // close namespace ntsa
}  // close namespace BloombergLP

#endif
