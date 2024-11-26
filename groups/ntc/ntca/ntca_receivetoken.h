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

#ifndef INCLUDED_NTCA_RECEIVETOKEN
#define INCLUDED_NTCA_RECEIVETOKEN

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsls_types.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe a token to cancel a receive operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b value: The value of the token.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_receive
class ReceiveToken
{
    bsl::uint64_t d_value;

  public:
    /// Create a new receive token having the default value.
    ReceiveToken();

    /// Create a new receive token having the same value as the specified
    /// 'original' object.
    ReceiveToken(const ReceiveToken& original);

    /// Destroy this object.
    ~ReceiveToken();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ReceiveToken& operator=(const ReceiveToken& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the value of the token to the specified 'value'.
    void setValue(bsl::uint64_t value);

    /// Return the value of the token.
    bsl::uint64_t value() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ReceiveToken& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ReceiveToken& other) const;

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

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(ReceiveToken);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(ReceiveToken);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::ReceiveToken
bsl::ostream& operator<<(bsl::ostream& stream, const ReceiveToken& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ReceiveToken
bool operator==(const ReceiveToken& lhs, const ReceiveToken& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ReceiveToken
bool operator!=(const ReceiveToken& lhs, const ReceiveToken& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ReceiveToken
bool operator<(const ReceiveToken& lhs, const ReceiveToken& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ReceiveToken
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReceiveToken& value);

NTCCFG_INLINE
ReceiveToken::ReceiveToken()
: d_value(0)
{
}

NTCCFG_INLINE
ReceiveToken::ReceiveToken(const ReceiveToken& original)
: d_value(original.d_value)
{
}

NTCCFG_INLINE
ReceiveToken::~ReceiveToken()
{
}

NTCCFG_INLINE
ReceiveToken& ReceiveToken::operator=(const ReceiveToken& other)
{
    d_value = other.d_value;
    return *this;
}

NTCCFG_INLINE
void ReceiveToken::reset()
{
    d_value = 0;
}

NTCCFG_INLINE
void ReceiveToken::setValue(bsl::uint64_t value)
{
    d_value = value;
}

NTCCFG_INLINE
bsl::uint64_t ReceiveToken::value() const
{
    return d_value;
}

NTCCFG_INLINE
bool ReceiveToken::equals(const ReceiveToken& other) const
{
    return (d_value == other.d_value);
}

NTCCFG_INLINE
bool ReceiveToken::less(const ReceiveToken& other) const
{
    return d_value < other.d_value;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ReceiveToken& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ReceiveToken& lhs, const ReceiveToken& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const ReceiveToken& lhs, const ReceiveToken& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const ReceiveToken& lhs, const ReceiveToken& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReceiveToken& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.value());
}

}  // close package namespace
}  // close enterprise namespace
#endif
