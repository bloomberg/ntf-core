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

#ifndef INCLUDED_NTCA_DEFLATEOPTIONS
#define INCLUDED_NTCA_DEFLATEOPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Provide a description of the options to a deflate operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b partial:
/// The data to deflate represents only a portion of the overall logical
/// data. If true, the deflate engine is not flushed after all input is
/// processed. If not specified, the default value is false.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_todo
class DeflateOptions
{
    bdlb::NullableValue<bool> d_partial;

  public:
    /// Create new deflate options having the default value.
    DeflateOptions();

    /// Create new deflate options having the same value as the specified
    /// 'original' object.
    DeflateOptions(const DeflateOptions& original);

    /// Destroy this object.
    ~DeflateOptions();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    DeflateOptions& operator=(const DeflateOptions& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the flag indicating the data to deflate only indicates a portion of
    /// the overal logical data to be deflated to the specified 'value'.
    void setPartial(bool value);

    /// Return the flag indicating the data to deflate only indicates a portion
    /// of the overal logical data to be deflated.
    const bdlb::NullableValue<bool>& partial() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const DeflateOptions& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const DeflateOptions& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(DeflateOptions);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(DeflateOptions);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::DeflateOptions
bsl::ostream& operator<<(bsl::ostream& stream, const DeflateOptions& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::DeflateOptions
bool operator==(const DeflateOptions& lhs, const DeflateOptions& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::DeflateOptions
bool operator!=(const DeflateOptions& lhs, const DeflateOptions& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::DeflateOptions
bool operator<(const DeflateOptions& lhs, const DeflateOptions& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::DeflateOptions
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const DeflateOptions& value);

NTCCFG_INLINE
DeflateOptions::DeflateOptions()
: d_partial()
{
}

NTCCFG_INLINE
DeflateOptions::DeflateOptions(const DeflateOptions& original)
: d_partial(original.d_partial)
{
}

NTCCFG_INLINE
DeflateOptions::~DeflateOptions()
{
}

NTCCFG_INLINE
DeflateOptions& DeflateOptions::operator=(const DeflateOptions& other)
{
    d_partial = other.d_partial;
    return *this;
}

NTCCFG_INLINE
void DeflateOptions::reset()
{
    d_partial.reset();
}

NTCCFG_INLINE
void DeflateOptions::setPartial(bool value)
{
    d_partial = value;
}

NTCCFG_INLINE
const bdlb::NullableValue<bool>& DeflateOptions::partial() const
{
    return d_partial;
}

template <typename HASH_ALGORITHM>
NTCCFG_INLINE
void DeflateOptions::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_partial);
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const DeflateOptions& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const DeflateOptions& lhs, const DeflateOptions& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const DeflateOptions& lhs, const DeflateOptions& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const DeflateOptions& lhs, const DeflateOptions& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTCCFG_INLINE
void hashAppend(HASH_ALGORITHM& algorithm, const DeflateOptions& value)
{
    value.hash(algorithm);
}

}  // close namespace ntca
}  // close namespace BloombergLP
#endif
