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

#ifndef INCLUDED_NTCA_ACCEPTQUEUEEVENT
#define INCLUDED_NTCA_ACCEPTQUEUEEVENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_acceptqueuecontext.h>
#include <ntca_acceptqueueeventtype.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe an event detected for an accept queue during the asynchronous
/// operation of a socket.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b type: The type of accept queue event.
///
/// @li @b context: The state of the accept queue at the time of the event.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_accept
class AcceptQueueEvent
{
    ntca::AcceptQueueEventType::Value d_type;
    ntca::AcceptQueueContext          d_context;

  public:
    /// Create a new accept queue event having the default value.
    AcceptQueueEvent();

    /// Create a new accept queue event having the same value as the
    /// specified 'original' object.
    AcceptQueueEvent(const AcceptQueueEvent& original);

    /// Destroy this object.
    ~AcceptQueueEvent();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    AcceptQueueEvent& operator=(const AcceptQueueEvent& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the type of accept queue event to the specified 'value'.
    void setType(ntca::AcceptQueueEventType::Value value);

    /// Set the state of the accept queue at the time of the event to the
    /// specified 'value'.
    void setContext(const ntca::AcceptQueueContext& value);

    /// Return the type of accept queue event.
    ntca::AcceptQueueEventType::Value type() const;

    /// Return the state of the accept queue at the time of the event.
    const ntca::AcceptQueueContext& context() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const AcceptQueueEvent& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const AcceptQueueEvent& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(AcceptQueueEvent);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(AcceptQueueEvent);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::AcceptQueueEvent
bsl::ostream& operator<<(bsl::ostream& stream, const AcceptQueueEvent& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::AcceptQueueEvent
bool operator==(const AcceptQueueEvent& lhs, const AcceptQueueEvent& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::AcceptQueueEvent
bool operator!=(const AcceptQueueEvent& lhs, const AcceptQueueEvent& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::AcceptQueueEvent
bool operator<(const AcceptQueueEvent& lhs, const AcceptQueueEvent& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::AcceptQueueEvent
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const AcceptQueueEvent& value);

NTCCFG_INLINE
AcceptQueueEvent::AcceptQueueEvent()
: d_type(ntca::AcceptQueueEventType::e_LOW_WATERMARK)
, d_context()
{
}

NTCCFG_INLINE
AcceptQueueEvent::AcceptQueueEvent(const AcceptQueueEvent& original)
: d_type(original.d_type)
, d_context(original.d_context)
{
}

NTCCFG_INLINE
AcceptQueueEvent::~AcceptQueueEvent()
{
}

NTCCFG_INLINE
AcceptQueueEvent& AcceptQueueEvent::operator=(const AcceptQueueEvent& other)
{
    d_type    = other.d_type;
    d_context = other.d_context;
    return *this;
}

NTCCFG_INLINE
void AcceptQueueEvent::reset()
{
    d_type = ntca::AcceptQueueEventType::e_LOW_WATERMARK;
    d_context.reset();
}

NTCCFG_INLINE
void AcceptQueueEvent::setType(ntca::AcceptQueueEventType::Value value)
{
    d_type = value;
}

NTCCFG_INLINE
void AcceptQueueEvent::setContext(const ntca::AcceptQueueContext& context)
{
    d_context = context;
}

NTCCFG_INLINE
ntca::AcceptQueueEventType::Value AcceptQueueEvent::type() const
{
    return d_type;
}

NTCCFG_INLINE
const ntca::AcceptQueueContext& AcceptQueueEvent::context() const
{
    return d_context;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const AcceptQueueEvent& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const AcceptQueueEvent& lhs, const AcceptQueueEvent& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const AcceptQueueEvent& lhs, const AcceptQueueEvent& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const AcceptQueueEvent& lhs, const AcceptQueueEvent& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const AcceptQueueEvent& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.type());
    hashAppend(algorithm, value.context());
}

}  // close package namespace
}  // close enterprise namespace
#endif
