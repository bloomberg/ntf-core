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

#ifndef INCLUDED_NTCA_ACCEPTQUEUECONTEXT
#define INCLUDED_NTCA_ACCEPTQUEUECONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe the state of an accept queue at the time of an event.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b size:
/// The size of the accept queue at the time of the event.
///
/// @li @b lowWatermark:
/// The low watermark of the accept queue at the time of the event.
///
/// @li @b highWatermark:
/// The high watermark of the accept queue at the time of the event.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_accept
class AcceptQueueContext
{
    bsl::size_t d_size;
    bsl::size_t d_lowWatermark;
    bsl::size_t d_highWatermark;

  public:
    /// Create a new accept queue context having the default value.
    AcceptQueueContext();

    /// Create a new accept queue context having the same value as the
    /// specified 'original' object.
    AcceptQueueContext(const AcceptQueueContext& original);

    /// Destroy this object.
    ~AcceptQueueContext();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    AcceptQueueContext& operator=(const AcceptQueueContext& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the size of the accept queue at the time of the event to the
    /// specified 'value'.
    void setSize(bsl::size_t value);

    /// Set the low watermark of the accept queue at the time of the event
    /// to the specified 'value'.
    void setLowWatermark(bsl::size_t value);

    /// Set the high watermark of the accept queue at the time of the event
    /// to the specified 'value'.
    void setHighWatermark(bsl::size_t value);

    /// Return the size of the accept queue at the time of the event.
    bsl::size_t size() const;

    /// Return the low watermark of the accept queue at the time of the
    /// event.
    bsl::size_t lowWatermark() const;

    /// Return the high watermark of the accept queue at the time of the
    /// event.
    bsl::size_t highWatermark() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const AcceptQueueContext& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const AcceptQueueContext& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(AcceptQueueContext);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(AcceptQueueContext);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::AcceptQueueContext
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const AcceptQueueContext& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::AcceptQueueContext
bool operator==(const AcceptQueueContext& lhs, const AcceptQueueContext& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::AcceptQueueContext
bool operator!=(const AcceptQueueContext& lhs, const AcceptQueueContext& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::AcceptQueueContext
bool operator<(const AcceptQueueContext& lhs, const AcceptQueueContext& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::AcceptQueueContext
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const AcceptQueueContext& value);

NTCCFG_INLINE
AcceptQueueContext::AcceptQueueContext()
: d_size(0)
, d_lowWatermark(0)
, d_highWatermark(0)
{
}

NTCCFG_INLINE
AcceptQueueContext::AcceptQueueContext(const AcceptQueueContext& original)
: d_size(original.d_size)
, d_lowWatermark(original.d_lowWatermark)
, d_highWatermark(original.d_highWatermark)
{
}

NTCCFG_INLINE
AcceptQueueContext::~AcceptQueueContext()
{
}

NTCCFG_INLINE
AcceptQueueContext& AcceptQueueContext::operator=(
    const AcceptQueueContext& other)
{
    d_size          = other.d_size;
    d_lowWatermark  = other.d_lowWatermark;
    d_highWatermark = other.d_highWatermark;
    return *this;
}

NTCCFG_INLINE
void AcceptQueueContext::reset()
{
    d_size          = 0;
    d_lowWatermark  = 0;
    d_highWatermark = 0;
}

NTCCFG_INLINE
void AcceptQueueContext::setSize(bsl::size_t value)
{
    d_size = value;
}

NTCCFG_INLINE
void AcceptQueueContext::setLowWatermark(bsl::size_t value)
{
    d_lowWatermark = value;
}

NTCCFG_INLINE
void AcceptQueueContext::setHighWatermark(bsl::size_t value)
{
    d_highWatermark = value;
}

NTCCFG_INLINE
bsl::size_t AcceptQueueContext::size() const
{
    return d_size;
}

NTCCFG_INLINE
bsl::size_t AcceptQueueContext::lowWatermark() const
{
    return d_lowWatermark;
}

NTCCFG_INLINE
bsl::size_t AcceptQueueContext::highWatermark() const
{
    return d_highWatermark;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const AcceptQueueContext& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const AcceptQueueContext& lhs, const AcceptQueueContext& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const AcceptQueueContext& lhs, const AcceptQueueContext& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const AcceptQueueContext& lhs, const AcceptQueueContext& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const AcceptQueueContext& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.size());
    hashAppend(algorithm, value.lowWatermark());
    hashAppend(algorithm, value.highWatermark());
}

}  // close package namespace
}  // close enterprise namespace
#endif
