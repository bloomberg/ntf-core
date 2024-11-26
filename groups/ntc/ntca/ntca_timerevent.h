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

#ifndef INCLUDED_NTCA_TIMEREVENT
#define INCLUDED_NTCA_TIMEREVENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_timercontext.h>
#include <ntca_timereventtype.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe a timer event.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b type:
/// The type of timer event.
///
/// @li @b context:
/// The state of the timer at the time of the event.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_timer
class TimerEvent
{
    ntca::TimerEventType::Value d_type;
    ntca::TimerContext          d_context;

  public:
    /// Create a new timer event having the default value.
    TimerEvent();

    /// Create a new timer event having the specified 'type' and 'context'.
    TimerEvent(ntca::TimerEventType::Value type,
               const ntca::TimerContext&   context);

    /// Create a new timer event having the same value as the specified
    /// 'original' object.
    TimerEvent(const TimerEvent& original);

    /// Destroy this object.
    ~TimerEvent();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    TimerEvent& operator=(const TimerEvent& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the type of timer queue event to the specified 'value'.
    void setType(ntca::TimerEventType::Value value);

    /// Set the state of the timerion attempt at the time of the event to
    /// the specified 'value'.
    void setContext(const ntca::TimerContext& value);

    /// Return the type of timer queue event.
    ntca::TimerEventType::Value type() const;

    /// Return the state of the timerion attempt at the time of the event.
    const ntca::TimerContext& context() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const TimerEvent& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const TimerEvent& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(TimerEvent);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(TimerEvent);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @ingroup ntca::TimerEvent
bsl::ostream& operator<<(bsl::ostream& stream, const TimerEvent& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @ingroup ntca::TimerEvent
bool operator==(const TimerEvent& lhs, const TimerEvent& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @ingroup ntca::TimerEvent
bool operator!=(const TimerEvent& lhs, const TimerEvent& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @ingroup ntca::TimerEvent
bool operator<(const TimerEvent& lhs, const TimerEvent& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @ingroup ntca::TimerEvent
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TimerEvent& value);

NTCCFG_INLINE
TimerEvent::TimerEvent()
: d_type(ntca::TimerEventType::e_DEADLINE)
, d_context()
{
}

NTCCFG_INLINE
TimerEvent::TimerEvent(ntca::TimerEventType::Value type,
                       const ntca::TimerContext&   context)
: d_type(type)
, d_context(context)
{
}

NTCCFG_INLINE
TimerEvent::TimerEvent(const TimerEvent& original)
: d_type(original.d_type)
, d_context(original.d_context)
{
}

NTCCFG_INLINE
TimerEvent::~TimerEvent()
{
}

NTCCFG_INLINE
TimerEvent& TimerEvent::operator=(const TimerEvent& other)
{
    d_type    = other.d_type;
    d_context = other.d_context;
    return *this;
}

NTCCFG_INLINE
void TimerEvent::reset()
{
    d_type = ntca::TimerEventType::e_DEADLINE;
    d_context.reset();
}

NTCCFG_INLINE
void TimerEvent::setType(ntca::TimerEventType::Value value)
{
    d_type = value;
}

NTCCFG_INLINE
void TimerEvent::setContext(const ntca::TimerContext& context)
{
    d_context = context;
}

NTCCFG_INLINE
ntca::TimerEventType::Value TimerEvent::type() const
{
    return d_type;
}

NTCCFG_INLINE
const ntca::TimerContext& TimerEvent::context() const
{
    return d_context;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const TimerEvent& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const TimerEvent& lhs, const TimerEvent& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const TimerEvent& lhs, const TimerEvent& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const TimerEvent& lhs, const TimerEvent& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TimerEvent& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.type());
    hashAppend(algorithm, value.context());
}

}  // close package namespace
}  // close enterprise namespace
#endif
