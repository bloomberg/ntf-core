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

#ifndef INCLUDED_NTCA_ACCEPTOPTIONS
#define INCLUDED_NTCA_ACCEPTOPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_accepttoken.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsls_timeinterval.h>
#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace ntca {

/// Describe the parameters to an accept operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b token:
/// The token used to cancel the operation.
///
/// @li @b deadline:
/// The deadline within which the connection must be accepted, in absolute time
/// since the Unix epoch.
///
/// @li @b recurse:
/// Allow callbacks to be invoked immediately and recursively if their
/// constraints are already satisified at the time the asynchronous operation
/// is initiated.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_accept
class AcceptOptions
{
    bdlb::NullableValue<ntca::AcceptToken>  d_token;
    bdlb::NullableValue<bsls::TimeInterval> d_deadline;
    bool                                    d_recurse;

  public:
    /// Create new accept options having the default value.
    AcceptOptions();

    /// Create new accept options having the same value as the specified
    /// 'original' object.
    AcceptOptions(const AcceptOptions& original);

    /// Destroy this object.
    ~AcceptOptions();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    AcceptOptions& operator=(const AcceptOptions& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the token used to cancel the operation to the specified 'value'.
    void setToken(const ntca::AcceptToken& value);

    /// Set the deadline within which the connection must be accepted to the
    /// specified 'value'.
    void setDeadline(const bsls::TimeInterval& value);

    /// Set the flag that allows callbacks to be invoked immediately and
    /// recursively if their constraints are already satisified at the time
    /// the asynchronous operation is initiated.
    void setRecurse(bool value);

    /// Return the token used to cancel the operation.
    const bdlb::NullableValue<ntca::AcceptToken>& token() const;

    /// Return the deadline within which the connection must be accepted.
    const bdlb::NullableValue<bsls::TimeInterval>& deadline() const;

    /// Return true if callbacks are allowed to be invoked immediately and
    /// recursively if their constraints are already satisified at the time
    /// the asynchronous operation is initiated, otherwise return false.
    bool recurse() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const AcceptOptions& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const AcceptOptions& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(AcceptOptions);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(AcceptOptions);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
bsl::ostream& operator<<(bsl::ostream& stream, const AcceptOptions& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
bool operator==(const AcceptOptions& lhs, const AcceptOptions& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
bool operator!=(const AcceptOptions& lhs, const AcceptOptions& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
bool operator<(const AcceptOptions& lhs, const AcceptOptions& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const AcceptOptions& value);

NTCCFG_INLINE
AcceptOptions::AcceptOptions()
: d_token()
, d_deadline()
, d_recurse(false)
{
}

NTCCFG_INLINE
AcceptOptions::AcceptOptions(const AcceptOptions& original)
: d_token(original.d_token)
, d_deadline(original.d_deadline)
, d_recurse(original.d_recurse)
{
}

NTCCFG_INLINE
AcceptOptions::~AcceptOptions()
{
}

NTCCFG_INLINE
AcceptOptions& AcceptOptions::operator=(const AcceptOptions& other)
{
    d_token    = other.d_token;
    d_deadline = other.d_deadline;
    d_recurse  = other.d_recurse;
    return *this;
}

NTCCFG_INLINE
void AcceptOptions::reset()
{
    d_token.reset();
    d_deadline.reset();
    d_recurse = false;
}

NTCCFG_INLINE
void AcceptOptions::setToken(const ntca::AcceptToken& value)
{
    d_token = value;
}

NTCCFG_INLINE
void AcceptOptions::setDeadline(const bsls::TimeInterval& value)
{
    d_deadline = value;
}

NTCCFG_INLINE
void AcceptOptions::setRecurse(bool value)
{
    d_recurse = value;
}

NTCCFG_INLINE
const bdlb::NullableValue<ntca::AcceptToken>& AcceptOptions::token() const
{
    return d_token;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsls::TimeInterval>& AcceptOptions::deadline() const
{
    return d_deadline;
}

NTCCFG_INLINE
bool AcceptOptions::recurse() const
{
    return d_recurse;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const AcceptOptions& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const AcceptOptions& lhs, const AcceptOptions& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const AcceptOptions& lhs, const AcceptOptions& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const AcceptOptions& lhs, const AcceptOptions& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const AcceptOptions& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.token());
    hashAppend(algorithm, value.deadline());
    hashAppend(algorithm, value.recurse());
}

}  // close package namespace
}  // close enterprise namespace
#endif
