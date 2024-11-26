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

#ifndef INCLUDED_NTCA_SENDCONTEXT
#define INCLUDED_NTCA_SENDCONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe the context of a send operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b error:
/// The error detected when performing the operation.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_send
class SendContext
{
    ntsa::Error d_error;

  public:
    /// Create a new send context having the default value.
    SendContext();

    /// Create a new send context having the same value as the specified
    /// 'original' object.
    SendContext(const SendContext& original);

    /// Destroy this object.
    ~SendContext();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    SendContext& operator=(const SendContext& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the error detected when performing the operation to the
    /// specified 'value'.
    void setError(const ntsa::Error& value);

    /// Return the error detected when performing the operation.
    const ntsa::Error& error() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const SendContext& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const SendContext& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(SendContext);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(SendContext);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::SendContext
bsl::ostream& operator<<(bsl::ostream& stream, const SendContext& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::SendContext
bool operator==(const SendContext& lhs, const SendContext& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::SendContext
bool operator!=(const SendContext& lhs, const SendContext& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::SendContext
bool operator<(const SendContext& lhs, const SendContext& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::SendContext
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const SendContext& value);

NTCCFG_INLINE
SendContext::SendContext()
: d_error()
{
}

NTCCFG_INLINE
SendContext::SendContext(const SendContext& original)
: d_error(original.d_error)
{
}

NTCCFG_INLINE
SendContext::~SendContext()
{
}

NTCCFG_INLINE
SendContext& SendContext::operator=(const SendContext& other)
{
    d_error = other.d_error;
    return *this;
}

NTCCFG_INLINE
void SendContext::reset()
{
    d_error = ntsa::Error();
}

NTCCFG_INLINE
void SendContext::setError(const ntsa::Error& value)
{
    d_error = value;
}

NTCCFG_INLINE
const ntsa::Error& SendContext::error() const
{
    return d_error;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const SendContext& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const SendContext& lhs, const SendContext& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const SendContext& lhs, const SendContext& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const SendContext& lhs, const SendContext& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const SendContext& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.error());
}

}  // close package namespace
}  // close enterprise namespace
#endif
