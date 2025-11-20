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
#include <bsls_objectbuffer.h>
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
/// @li @b minLimit:
/// The minimum value.
///
/// @li @b maxLimit:
/// The maximum value.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class Backoff
{
    /// Enumerates the backoff progressions.
    enum Type {
        /// The backoff progression is undefined.
        e_UNDEFINED = 0,

        /// The backoff progression is arithmetic.
        e_ARITHMETIC = 1,

        /// The backoff progression is geometric.
        e_GEOMETRIC = 2
    };

    /// Defines a type alias for the representation of an arithmetic
    /// progression.
    typedef bsls::TimeInterval ArithmeticType;

    /// Defines a type alias for the representation of a geometric progression.
    typedef double GeometricType;

    /// Describes the union of possible representations.
    union Rep {
        /// The parameter of arithmetic progression.
        bsls::ObjectBuffer<ArithmeticType> d_arithmetic;

        /// The parameter of geometric progression.
        bsls::ObjectBuffer<GeometricType> d_geometric;
    };

    /// The progression type.
    Type d_type;

    /// The union of possible progression representations.
    Rep d_rep;

    /// The minimum jitter.
    bsls::TimeInterval d_minJitter;

    /// The maximum jitter.
    bsls::TimeInterval d_maxJitter;

    /// The minimum limit.
    bdlb::NullableValue<bsls::TimeInterval> d_minLimit;

    /// The maximum limit.
    bdlb::NullableValue<bsls::TimeInterval> d_maxLimit;

  public:
    /// Create a new backoff with the default value.
    Backoff();

    /// Create a new backoff having the same value as the specified 'original'
    /// object.
    Backoff(const Backoff& original);

    /// Create a new backoff having the same value as the specified 'original'
    /// object. The value of the 'original' object becomes unspecified but
    /// valid.
    Backoff(bslmf::MovableRef<Backoff> original);

    /// Destroy this object.
    ~Backoff();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    Backoff& operator=(const Backoff& other);

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object. The value of the 'other' object
    /// becomes unspecified but valid.
    Backoff& operator=(bslmf::MovableRef<Backoff> other);

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

    /// Set the minimum limit to the specified 'value'.
    void setMinLimit(const bsls::TimeInterval& value);

    /// Set the maximum limit to the specified 'value'.
    void setMaxLimit(const bsls::TimeInterval& value);

    /// Return the arithmetic difference between intervals. The behavior is
    /// undefined unless the progression is arithmetic.
    const bsls::TimeInterval& arithmetic() const;

    /// Return the geometric multiplier between intervals. The behavior is
    /// undefined unless the progression is geometric.
    double geometric() const;

    /// Return the minimum jitter.
    const bsls::TimeInterval& minJitter() const;

    /// Return the maximum jitter.
    const bsls::TimeInterval& maxJitter() const;

    /// Return the minimum limit.
    const bdlb::NullableValue<bsls::TimeInterval>& minLimit() const;

    /// Return the maximum limit.
    const bdlb::NullableValue<bsls::TimeInterval>& maxLimit() const;

    /// Apply this backoff to the specified 'value' and return the result.
    bsls::TimeInterval apply(const bsls::TimeInterval& value) const;

    /// Return true of the back progression is undefined, otherwise return
    /// false.
    bool isUndefined() const;

    /// Return true of the back progression is arithmetic, otherwise return
    /// false.
    bool isArithmetic() const;

    /// Return true of the back progression is geometric, otherwise return
    /// false.
    bool isGeometric() const;

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

    if (d_type == e_ARITHMETIC) {
        const bsls::TimeInterval arithmetic = d_rep.d_arithmetic.object();
        hashAppend(algorithm, arithmetic.totalMicroseconds());
    }
    else if (d_type == e_GEOMETRIC) {
        const double geometric = d_rep.d_geometric.object();
        hashAppend(algorithm, geometric);
    }

    hashAppend(algorithm, d_minJitter.totalMicroseconds());
    hashAppend(algorithm, d_maxJitter.totalMicroseconds());

    bsls::TimeInterval minLimit;
    if (d_minLimit.has_value()) {
        minLimit = d_minLimit.value();
    }

    hashAppend(algorithm, minLimit.totalMicroseconds());

    bsls::TimeInterval maxLimit;
    if (d_maxLimit.has_value()) {
        maxLimit = d_maxLimit.value();
    }

    hashAppend(algorithm, maxLimit.totalMicroseconds());
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM& algorithm, const Backoff& value)
{
    value.hash(algorithm);
}

}  // close namespace ntsa
}  // close namespace BloombergLP

#endif
