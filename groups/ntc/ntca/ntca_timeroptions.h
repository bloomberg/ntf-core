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

#ifndef INCLUDED_NTCA_TIMEROPTIONS
#define INCLUDED_NTCA_TIMEROPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_timereventtype.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsls_timeinterval.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe the configuration of a timer.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b handle:
/// A user-defined handle to the timer.
///
/// @li @b id:
/// A user-defined identifier of the timer.
///
/// @li @b oneShot:
/// The flag indicating the timer is intended to be scheduled only once and not
/// recur at any period. Such timers are automatically removed when their
/// deadline is reached and do not need to be explicitly removed.
///
/// @li @b drift:
/// The flag indicating that the exact time between the scheduled deadline of
/// the timer and the time the timer deadline event is invoked should be
/// calculated and reported in the timer event context.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_timer
class TimerOptions
{
    enum Flags { k_FLAG_ONE_SHOT = (1 << 1), k_FLAG_DRIFT = (1 << 2) };

    void*        d_handle;
    int          d_id;
    unsigned int d_flags;
    unsigned int d_eventMask;

  public:
    /// Create new timer options having the default value.
    TimerOptions();

    /// Create new timer options having the same value as the specified
    /// 'original' object.
    TimerOptions(const TimerOptions& original);

    /// Create new timer options having the same value as the specified
    /// 'original' object. The value of the 'original' object becomes
    /// unspecified but valid.
    TimerOptions(bslmf::MovableRef<TimerOptions> original);

    /// Destroy this object.
    ~TimerOptions();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    TimerOptions& operator=(const TimerOptions& other);

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object. The value of the
    /// 'other' object becomes unspecified but valid.
    TimerOptions& operator=(bslmf::MovableRef<TimerOptions> other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the user-defined handle of this timer to the specified 'value'.
    void setHandle(void* value);

    /// Set the user-defined identifier of this timer to the specified
    /// 'value'.
    void setId(int value);

    /// Set the flag indicating one-shot mode to the specified 'value'.
    /// Timers in one-shot mode are intended to be scheduled only once and
    /// not recur at any period. Such timers are automatically closed when
    /// their deadline is reached and do not need to be explicitly closed.
    /// The default timer options indicate the timer is *not* in one-shot
    /// mode.
    void setOneShot(bool value);

    /// Set the flag indicating that the exact drift should be calculated
    /// from the timer deadline to the time the timer deadline event is
    /// invoked to the specified 'value'.
    void setDrift(bool value);

    /// Enable the announcement of events of the specified 'timerEventType'.
    /// The default timer options indicate that *all* events should be
    /// announced.
    void showEvent(ntca::TimerEventType::Value timerEventType);

    /// Disable the announcement of events of the specified
    /// 'timerEventType'. The default timer options indicate that *all*
    /// events should be announced.
    void hideEvent(ntca::TimerEventType::Value timerEventType);

    /// Return the user-defined handle of this timer.
    void* handle() const;

    /// Return the user-defined identifier of this timer.
    int id() const;

    /// Return the one-shot mode. Timers in one-shot mode are intended to be
    /// scheduled only once and not recur at any period. Such timers are
    /// automatically closed when their deadline is reached and do not
    /// need to be explicitly closed. The default timer options indicate
    /// the timer is *not* in one-shot mode.
    bool oneShot() const;

    /// Return the flag indicating that the exact drift should be calculated
    /// from the timer deadline to the time the timer deadline event is
    /// invoked.
    bool drift() const;

    /// Return true if the timer options indicate that events of the
    /// specified 'timerEventType' should be announced, otherwise return
    /// false.
    bool wantEvent(ntca::TimerEventType::Value timerEventType) const;

    /// Return the flags.
    unsigned int flags() const;

    /// Return the event mask.
    unsigned int eventMask() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const TimerOptions& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const TimerOptions& other) const;

    /// Return the hash value of this object according to the default
    /// hash algorithm.
    bsl::size_t hash() const;

    /// Return the hash value of this object according to the parameterized
    /// hash algorithm.
    template <typename HASH_ALGORITHM>
    bsl::size_t hash() const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(TimerOptions);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(TimerOptions);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::TimerOptions
bsl::ostream& operator<<(bsl::ostream& stream, const TimerOptions& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::TimerOptions
bool operator==(const TimerOptions& lhs, const TimerOptions& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::TimerOptions
bool operator!=(const TimerOptions& lhs, const TimerOptions& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::TimerOptions
bool operator<(const TimerOptions& lhs, const TimerOptions& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::TimerOptions
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TimerOptions& value);

NTCCFG_INLINE
TimerOptions::TimerOptions()
: d_handle(0)
, d_id(0)
, d_flags(0)
, d_eventMask(0)
{
}

NTCCFG_INLINE
TimerOptions::TimerOptions(const TimerOptions& original)
: d_handle(original.d_handle)
, d_id(original.d_id)
, d_flags(original.d_flags)
, d_eventMask(original.d_eventMask)
{
}

NTCCFG_INLINE
TimerOptions::TimerOptions(bslmf::MovableRef<TimerOptions> original)
: d_handle(NTSCFG_MOVE_FROM(original, d_handle))
, d_id(NTSCFG_MOVE_FROM(original, d_id))
, d_flags(NTSCFG_MOVE_FROM(original, d_flags))
, d_eventMask(NTSCFG_MOVE_FROM(original, d_eventMask))
{
    NTSCFG_MOVE_RESET(original);
}

NTCCFG_INLINE
TimerOptions::~TimerOptions()
{
}

NTCCFG_INLINE
TimerOptions& TimerOptions::operator=(const TimerOptions& other)
{
    d_handle    = other.d_handle;
    d_id        = other.d_id;
    d_flags     = other.d_flags;
    d_eventMask = other.d_eventMask;

    return *this;
}

NTCCFG_INLINE
TimerOptions& TimerOptions::operator=(bslmf::MovableRef<TimerOptions> other)
{
    d_handle    = NTSCFG_MOVE_FROM(other, d_handle);
    d_id        = NTSCFG_MOVE_FROM(other, d_id);
    d_flags     = NTSCFG_MOVE_FROM(other, d_flags);
    d_eventMask = NTSCFG_MOVE_FROM(other, d_eventMask);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTCCFG_INLINE
void TimerOptions::reset()
{
    d_handle    = 0;
    d_id        = 0;
    d_flags     = 0;
    d_eventMask = 0;
}

NTCCFG_INLINE
void TimerOptions::setHandle(void* value)
{
    d_handle = value;
}

NTCCFG_INLINE
void TimerOptions::setId(int value)
{
    d_id = value;
}

NTCCFG_INLINE
void TimerOptions::setOneShot(bool value)
{
    if (value) {
        d_flags |= k_FLAG_ONE_SHOT;
    }
    else {
        d_flags &= ~k_FLAG_ONE_SHOT;
    }
}

NTCCFG_INLINE
void TimerOptions::setDrift(bool value)
{
    if (value) {
        d_flags |= k_FLAG_DRIFT;
    }
    else {
        d_flags &= ~k_FLAG_DRIFT;
    }
}

NTCCFG_INLINE
void TimerOptions::showEvent(ntca::TimerEventType::Value timerEventType)
{
    d_eventMask &= ~(1 << timerEventType);
}

NTCCFG_INLINE
void TimerOptions::hideEvent(ntca::TimerEventType::Value timerEventType)
{
    d_eventMask |= (1 << timerEventType);
}

NTCCFG_INLINE
void* TimerOptions::handle() const
{
    return d_handle;
}

NTCCFG_INLINE
int TimerOptions::id() const
{
    return d_id;
}

NTCCFG_INLINE
bool TimerOptions::oneShot() const
{
    return (d_flags & k_FLAG_ONE_SHOT) != 0;
}

NTCCFG_INLINE
bool TimerOptions::drift() const
{
    return (d_flags & k_FLAG_DRIFT) != 0;
}

NTCCFG_INLINE
bool TimerOptions::wantEvent(ntca::TimerEventType::Value timerEventType) const
{
    return ((d_eventMask & (1 << timerEventType)) == 0);
}

NTCCFG_INLINE
unsigned int TimerOptions::flags() const
{
    return d_flags;
}

NTCCFG_INLINE
unsigned int TimerOptions::eventMask() const
{
    return d_eventMask;
}

NTCCFG_INLINE
bsl::size_t TimerOptions::hash() const
{
    bslh::DefaultHashAlgorithm algorithm;
    hashAppend(algorithm, *this);
    return static_cast<bsl::size_t>(algorithm.computeHash());
}

template <typename HASH_ALGORITHM>
NTCCFG_INLINE bsl::size_t TimerOptions::hash() const
{
    HASH_ALGORITHM algorithm;
    hashAppend(algorithm, *this);
    return static_cast<bsl::size_t>(algorithm.computeHash());
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const TimerOptions& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const TimerOptions& lhs, const TimerOptions& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const TimerOptions& lhs, const TimerOptions& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const TimerOptions& lhs, const TimerOptions& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TimerOptions& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, reinterpret_cast<bsl::uintptr_t>(value.handle()));
    hashAppend(algorithm, value.id());
    hashAppend(algorithm, value.flags());
    hashAppend(algorithm, value.eventMask());
}

}  // close package namespace
}  // close enterprise namespace
#endif
