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

#ifndef INCLUDED_NTCA_DOWNGRADEOPTIONS
#define INCLUDED_NTCA_DOWNGRADEOPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Provide a description of the options to downgrade communication from
/// encrypted to plaintext.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b abortive:
/// The flag that indicates the downgrade should be performed abortively: 
/// no encryption protocol-specific shutdown sequence is performed. If not
/// specified, the default value is false.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_downgrade
class DowngradeOptions
{
    bdlb::NullableValue<bool> d_abortive;

  public:
    /// Create new deflate options having the default value.
    DowngradeOptions();

    /// Create new deflate options having the same value as the specified
    /// 'original' object.
    DowngradeOptions(const DowngradeOptions& original);

    /// Destroy this object.
    ~DowngradeOptions();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    DowngradeOptions& operator=(const DowngradeOptions& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the flag indicating the downgrade is performed abortively to the
    /// specified 'value'.
    void setAbortive(bool value);

    /// Return the flag indicating the downgrade is performed abortively.
    const bdlb::NullableValue<bool>& abortive() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const DowngradeOptions& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const DowngradeOptions& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(DowngradeOptions);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(DowngradeOptions);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::DowngradeOptions
bsl::ostream& operator<<(bsl::ostream& stream, const DowngradeOptions& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::DowngradeOptions
bool operator==(const DowngradeOptions& lhs, const DowngradeOptions& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::DowngradeOptions
bool operator!=(const DowngradeOptions& lhs, const DowngradeOptions& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::DowngradeOptions
bool operator<(const DowngradeOptions& lhs, const DowngradeOptions& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::DowngradeOptions
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const DowngradeOptions& value);

NTCCFG_INLINE
DowngradeOptions::DowngradeOptions()
: d_abortive()
{
}

NTCCFG_INLINE
DowngradeOptions::DowngradeOptions(const DowngradeOptions& original)
: d_abortive(original.d_abortive)
{
}

NTCCFG_INLINE
DowngradeOptions::~DowngradeOptions()
{
}

NTCCFG_INLINE
DowngradeOptions& DowngradeOptions::operator=(const DowngradeOptions& other)
{
    d_abortive = other.d_abortive;
    return *this;
}

NTCCFG_INLINE
void DowngradeOptions::reset()
{
    d_abortive.reset();
}

NTCCFG_INLINE
void DowngradeOptions::setAbortive(bool value)
{
    d_abortive = value;
}

NTCCFG_INLINE
const bdlb::NullableValue<bool>& DowngradeOptions::abortive() const
{
    return d_abortive;
}

template <typename HASH_ALGORITHM>
NTCCFG_INLINE void DowngradeOptions::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_abortive);
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const DowngradeOptions& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const DowngradeOptions& lhs, const DowngradeOptions& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const DowngradeOptions& lhs, const DowngradeOptions& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const DowngradeOptions& lhs, const DowngradeOptions& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTCCFG_INLINE void hashAppend(HASH_ALGORITHM&       algorithm,
                              const DowngradeOptions& value)
{
    value.hash(algorithm);
}

}  // close namespace ntca
}  // close namespace BloombergLP
#endif
