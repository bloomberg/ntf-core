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

#ifndef INCLUDED_NTCA_REACTORCONTEXT
#define INCLUDED_NTCA_REACTORCONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe the state of a reactor socket at the time of an event.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b bytesReadable:
/// The number of bytes readable, if known.
///
/// @li @b bytesWritable:
/// The number of bytes writable, if known.
///
/// @li @b error:
/// The error detected for the socket, if any.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_reactor
class ReactorContext
{
    bdlb::NullableValue<bsl::size_t> d_bytesReadable;
    bdlb::NullableValue<bsl::size_t> d_bytesWritable;
    ntsa::Error                      d_error;

  public:
    /// Create a new reactor context having the default value.
    ReactorContext();

    /// Create a new reactor context having the same value as the specified
    /// 'original' object.
    ReactorContext(const ReactorContext& original);

    /// Destroy this object.
    ~ReactorContext();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ReactorContext& operator=(const ReactorContext& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the number of bytes readable from the socket to the specified
    /// 'value'.
    void setBytesReadable(bsl::size_t value);

    /// Set the number of bytes writable to the socket to the specified
    /// 'value'.
    void setBytesWritable(bsl::size_t value);

    /// Set the error detected for the socket at the time of the event to
    /// the specified 'value'.
    void setError(const ntsa::Error& value);

    /// Return the number of bytes readable from the socket, if known.
    const bdlb::NullableValue<bsl::size_t>& bytesReadable() const;

    /// Return the number of bytes writable to the socket, if known.
    const bdlb::NullableValue<bsl::size_t>& bytesWritable() const;

    /// Return the error detected for the socket at the time of the event.
    const ntsa::Error& error() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ReactorContext& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ReactorContext& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(ReactorContext);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(ReactorContext);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::ReactorContext
bsl::ostream& operator<<(bsl::ostream& stream, const ReactorContext& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ReactorContext
bool operator==(const ReactorContext& lhs, const ReactorContext& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ReactorContext
bool operator!=(const ReactorContext& lhs, const ReactorContext& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ReactorContext
bool operator<(const ReactorContext& lhs, const ReactorContext& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ReactorContext
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReactorContext& value);

NTCCFG_INLINE
ReactorContext::ReactorContext()
: d_bytesReadable()
, d_bytesWritable()
, d_error()
{
}

NTCCFG_INLINE
ReactorContext::ReactorContext(const ReactorContext& original)
: d_bytesReadable(original.d_bytesReadable)
, d_bytesWritable(original.d_bytesWritable)
, d_error(original.d_error)
{
}

NTCCFG_INLINE
ReactorContext::~ReactorContext()
{
}

NTCCFG_INLINE
ReactorContext& ReactorContext::operator=(const ReactorContext& other)
{
    d_bytesReadable = other.d_bytesReadable;
    d_bytesWritable = other.d_bytesWritable;
    d_error         = other.d_error;
    return *this;
}

NTCCFG_INLINE
void ReactorContext::reset()
{
    d_bytesReadable.reset();
    d_bytesWritable.reset();
    d_error = ntsa::Error();
}

NTCCFG_INLINE
void ReactorContext::setBytesReadable(bsl::size_t value)
{
    d_bytesReadable = value;
}

NTCCFG_INLINE
void ReactorContext::setBytesWritable(bsl::size_t value)
{
    d_bytesWritable = value;
}

NTCCFG_INLINE
void ReactorContext::setError(const ntsa::Error& value)
{
    d_error = value;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& ReactorContext::bytesReadable() const
{
    return d_bytesReadable;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& ReactorContext::bytesWritable() const
{
    return d_bytesWritable;
}

NTCCFG_INLINE
const ntsa::Error& ReactorContext::error() const
{
    return d_error;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ReactorContext& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ReactorContext& lhs, const ReactorContext& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const ReactorContext& lhs, const ReactorContext& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const ReactorContext& lhs, const ReactorContext& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReactorContext& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.bytesReadable());
    hashAppend(algorithm, value.bytesWritable());
    hashAppend(algorithm, value.error());
}

}  // close package namespace
}  // close enterprise namespace
#endif
