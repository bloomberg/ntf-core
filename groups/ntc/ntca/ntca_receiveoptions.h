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

#ifndef INCLUDED_NTCA_RECEIVEOPTIONS
#define INCLUDED_NTCA_RECEIVEOPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_receivetoken.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_handle.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsls_timeinterval.h>
#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace ntca {

/// Describe the parameters to a receive operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b token:
/// The token used to cancel the operation.
///
/// @li @b minSize:
/// The minimum size of the data in the read queue that must exist to satisfy
/// the receive operation.
///
/// @li @b maxSize:
/// The maximum amount of data in the read queue to receive.
///
/// @li @b deadline:
/// The deadline within which the message must be received, in absolute time
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
/// @ingroup module_ntci_operation_receive
class ReceiveOptions
{
    bdlb::NullableValue<ntca::ReceiveToken> d_token;
    bsl::size_t                             d_minSize;
    bsl::size_t                             d_maxSize;
    bdlb::NullableValue<bsls::TimeInterval> d_deadline;
    bool                                    d_recurse;

  public:
    /// Create new receive options having the default value.
    ReceiveOptions();

    /// Create new receive options having the same value as the specified
    /// 'original' object.
    ReceiveOptions(const ReceiveOptions& original);

    /// Destroy this object.
    ~ReceiveOptions();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ReceiveOptions& operator=(const ReceiveOptions& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the token used to cancel the operation to the specified 'value'.
    void setToken(const ntca::ReceiveToken& value);

    /// Set the minimum number of bytes to copy to the specified 'value'.
    void setMinSize(bsl::size_t value);

    /// Set the maximum number of bytes to copy to the specified 'value'.
    void setMaxSize(bsl::size_t value);

    /// Set the exact number of bytes to copy to the specified 'value'.
    /// Note that this function is a synonym for setting the minimum and
    /// maximum number of bytes to the same 'value'.
    void setSize(bsl::size_t value);

    /// Set the deadline within which the data must be received to the
    /// specified 'value'.
    void setDeadline(const bsls::TimeInterval& value);

    /// Set the flag that allows callbacks to be invoked immediately and
    /// recursively if their constraints are already satisified at the time
    /// the asynchronous operation is initiated.
    void setRecurse(bool value);

    /// Return the token used to cancel the operation.
    const bdlb::NullableValue<ntca::ReceiveToken>& token() const;

    /// Return the minimum number of bytes to copy.
    bsl::size_t minSize() const;

    /// Return the maximum number of bytes to copy.
    bsl::size_t maxSize() const;

    /// Return the deadline within which the data must be received.
    const bdlb::NullableValue<bsls::TimeInterval>& deadline() const;

    /// Return true if callbacks are allowed to be invoked immediately and
    /// recursively if their constraints are already satisified at the time
    /// the asynchronous operation is initiated, otherwise return false.
    bool recurse() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ReceiveOptions& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ReceiveOptions& other) const;

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
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(ReceiveOptions);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::ReceiveOptions
bsl::ostream& operator<<(bsl::ostream& stream, const ReceiveOptions& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ReceiveOptions
bool operator==(const ReceiveOptions& lhs, const ReceiveOptions& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ReceiveOptions
bool operator!=(const ReceiveOptions& lhs, const ReceiveOptions& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ReceiveOptions
bool operator<(const ReceiveOptions& lhs, const ReceiveOptions& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ReceiveOptions
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReceiveOptions& value);

NTCCFG_INLINE
ReceiveOptions::ReceiveOptions()
: d_token()
, d_minSize(1)
, d_maxSize(bsl::numeric_limits<bsl::size_t>::max())
, d_deadline()
, d_recurse(false)
{
}

NTCCFG_INLINE
ReceiveOptions::ReceiveOptions(const ReceiveOptions& original)
: d_token(original.d_token)
, d_minSize(original.d_minSize)
, d_maxSize(original.d_maxSize)
, d_deadline(original.d_deadline)
, d_recurse(original.d_recurse)
{
}

NTCCFG_INLINE
ReceiveOptions::~ReceiveOptions()
{
}

NTCCFG_INLINE
ReceiveOptions& ReceiveOptions::operator=(const ReceiveOptions& other)
{
    d_token    = other.d_token;
    d_minSize  = other.d_minSize;
    d_maxSize  = other.d_maxSize;
    d_deadline = other.d_deadline;
    d_recurse  = other.d_recurse;
    return *this;
}

NTCCFG_INLINE
void ReceiveOptions::reset()
{
    d_token.reset();
    d_minSize = 1;
    d_maxSize = bsl::numeric_limits<bsl::size_t>::max();
    d_deadline.reset();
    d_recurse = false;
}

NTCCFG_INLINE
void ReceiveOptions::setToken(const ntca::ReceiveToken& value)
{
    d_token = value;
}

NTCCFG_INLINE
void ReceiveOptions::setMinSize(bsl::size_t value)
{
    d_minSize = value;
    if (d_minSize == 0) {
        d_minSize = 1;
    }
    else if (d_minSize > d_maxSize) {
        d_maxSize = d_minSize;
    }
}

NTCCFG_INLINE
void ReceiveOptions::setSize(bsl::size_t value)
{
    d_minSize = value;
    if (d_minSize == 0) {
        d_minSize = 1;
    }
    d_maxSize = d_minSize;
}

NTCCFG_INLINE
void ReceiveOptions::setMaxSize(bsl::size_t value)
{
    d_maxSize = value;
    if (d_maxSize < d_minSize) {
        d_minSize = d_maxSize;
    }
}

NTCCFG_INLINE
void ReceiveOptions::setDeadline(const bsls::TimeInterval& value)
{
    d_deadline = value;
}

NTCCFG_INLINE
void ReceiveOptions::setRecurse(bool value)
{
    d_recurse = value;
}

NTCCFG_INLINE
const bdlb::NullableValue<ntca::ReceiveToken>& ReceiveOptions::token() const
{
    return d_token;
}

NTCCFG_INLINE
bsl::size_t ReceiveOptions::minSize() const
{
    BSLS_ASSERT(d_minSize != 0);
    BSLS_ASSERT(d_minSize <= d_maxSize);
    return d_minSize;
}

NTCCFG_INLINE
bsl::size_t ReceiveOptions::maxSize() const
{
    BSLS_ASSERT(d_maxSize >= d_minSize);
    return d_maxSize;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsls::TimeInterval>& ReceiveOptions::deadline() const
{
    return d_deadline;
}

NTCCFG_INLINE
bool ReceiveOptions::recurse() const
{
    return d_recurse;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ReceiveOptions& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ReceiveOptions& lhs, const ReceiveOptions& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const ReceiveOptions& lhs, const ReceiveOptions& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const ReceiveOptions& lhs, const ReceiveOptions& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReceiveOptions& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.token());
    hashAppend(algorithm, value.minSize());
    hashAppend(algorithm, value.maxSize());
    hashAppend(algorithm, value.deadline());
    hashAppend(algorithm, value.recurse());
}

}  // close package namespace
}  // close enterprise namespace
#endif
