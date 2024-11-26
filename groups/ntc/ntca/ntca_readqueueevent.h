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

#ifndef INCLUDED_NTCA_READQUEUEEVENT
#define INCLUDED_NTCA_READQUEUEEVENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_readqueuecontext.h>
#include <ntca_readqueueeventtype.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe an event detected for a read queue during the asynchronous
/// operation of a socket.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b type:
/// The type of read queue event.
///
/// @li @b context:
/// The state of the read queue at the time of the event.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_receive
class ReadQueueEvent
{
    ntca::ReadQueueEventType::Value d_type;
    ntca::ReadQueueContext          d_context;

  public:
    /// Create a new read queue event having the default value.
    ReadQueueEvent();

    /// Create a new read queue event having the same value as the
    /// specified 'original' object.
    ReadQueueEvent(const ReadQueueEvent& original);

    /// Destroy this object.
    ~ReadQueueEvent();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ReadQueueEvent& operator=(const ReadQueueEvent& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the type of read queue event to the specified 'value'.
    void setType(ntca::ReadQueueEventType::Value value);

    /// Set the state of the read queue at the time of the event to the
    /// specified 'value'.
    void setContext(const ntca::ReadQueueContext& value);

    /// Return the type of read queue event.
    ntca::ReadQueueEventType::Value type() const;

    /// Return the state of the read queue at the time of the event.
    const ntca::ReadQueueContext& context() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ReadQueueEvent& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ReadQueueEvent& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(ReadQueueEvent);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(ReadQueueEvent);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::ReadQueueEvent
bsl::ostream& operator<<(bsl::ostream& stream, const ReadQueueEvent& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ReadQueueEvent
bool operator==(const ReadQueueEvent& lhs, const ReadQueueEvent& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ReadQueueEvent
bool operator!=(const ReadQueueEvent& lhs, const ReadQueueEvent& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ReadQueueEvent
bool operator<(const ReadQueueEvent& lhs, const ReadQueueEvent& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ReadQueueEvent
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReadQueueEvent& value);

NTCCFG_INLINE
ReadQueueEvent::ReadQueueEvent()
: d_type(ntca::ReadQueueEventType::e_LOW_WATERMARK)
, d_context()
{
}

NTCCFG_INLINE
ReadQueueEvent::ReadQueueEvent(const ReadQueueEvent& original)
: d_type(original.d_type)
, d_context(original.d_context)
{
}

NTCCFG_INLINE
ReadQueueEvent::~ReadQueueEvent()
{
}

NTCCFG_INLINE
ReadQueueEvent& ReadQueueEvent::operator=(const ReadQueueEvent& other)
{
    d_type    = other.d_type;
    d_context = other.d_context;
    return *this;
}

NTCCFG_INLINE
void ReadQueueEvent::reset()
{
    d_type = ntca::ReadQueueEventType::e_LOW_WATERMARK;
    d_context.reset();
}

NTCCFG_INLINE
void ReadQueueEvent::setType(ntca::ReadQueueEventType::Value value)
{
    d_type = value;
}

NTCCFG_INLINE
void ReadQueueEvent::setContext(const ntca::ReadQueueContext& context)
{
    d_context = context;
}

NTCCFG_INLINE
ntca::ReadQueueEventType::Value ReadQueueEvent::type() const
{
    return d_type;
}

NTCCFG_INLINE
const ntca::ReadQueueContext& ReadQueueEvent::context() const
{
    return d_context;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ReadQueueEvent& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ReadQueueEvent& lhs, const ReadQueueEvent& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const ReadQueueEvent& lhs, const ReadQueueEvent& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const ReadQueueEvent& lhs, const ReadQueueEvent& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReadQueueEvent& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.type());
    hashAppend(algorithm, value.context());
}

}  // close package namespace
}  // close enterprise namespace
#endif
