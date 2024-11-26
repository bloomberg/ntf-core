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

#ifndef INCLUDED_NTCA_TIMERCONTEXT
#define INCLUDED_NTCA_TIMERCONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bslh_hash.h>
#include <bsls_timeinterval.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe the state of a timer at the time of an event.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b now:
/// The the current time at the point of time at which the timer's deadline was
/// evaluated.
///
/// @li @b deadline:
/// The latest deadline of the timer.
///
/// @li @b drift:
/// The amount of time between the timer deadline and the time the timer event
/// callback is invoked.
///
/// @li @b error:
/// The error, if any.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_timer
class TimerContext
{
    bsls::TimeInterval d_now;
    bsls::TimeInterval d_deadline;
    bsls::TimeInterval d_drift;
    ntsa::Error        d_error;

  public:
    /// Create a new timer context having the default value.
    TimerContext();

    /// Create a new timer context describing the specified 'now' and 'deadline'.
    TimerContext(const bsls::TimeInterval& now,
                 const bsls::TimeInterval& deadline);

    /// Create a new timer context describing the specified 'now', 'deadline', and 'drift'.
    TimerContext(const bsls::TimeInterval& now,
                 const bsls::TimeInterval& deadline,
                 const bsls::TimeInterval& drift);

    /// Create a new timer context having the same value as the specified
    /// 'original' object.
    TimerContext(const TimerContext& original);

    /// Destroy this object.
    ~TimerContext();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    TimerContext& operator=(const TimerContext& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the current time at the point of time at which the timer's
    /// deadline was evaluated to the specified 'value'.
    void setNow(const bsls::TimeInterval& value);

    /// Set the timer deadline to the specified 'value'.
    void setDeadline(const bsls::TimeInterval& value);

    /// Set the amount of time between the timer deadline and when the
    /// timer event callback is invoked to the specified 'value'.
    void setDrift(const bsls::TimeInterval& value);

    /// Set the error detected for the timerion attempt at the time of
    /// the event specified 'value'.
    void setError(const ntsa::Error& value);

    /// The current time at the point of time at which the timer's deadline
    /// was evaluated.
    const bsls::TimeInterval& now() const;

    /// Return the timer deadline.
    const bsls::TimeInterval& deadline() const;

    /// Return the amount of time between the timer deadline and when the
    /// timer event callback is invoked.
    const bsls::TimeInterval& drift() const;

    /// Return the error detected for the timerion attempt at the time of
    /// the event.
    const ntsa::Error& error() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const TimerContext& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const TimerContext& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(TimerContext);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(TimerContext);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::TimerContext
bsl::ostream& operator<<(bsl::ostream& stream, const TimerContext& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::TimerContext
bool operator==(const TimerContext& lhs, const TimerContext& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::TimerContext
bool operator!=(const TimerContext& lhs, const TimerContext& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::TimerContext
bool operator<(const TimerContext& lhs, const TimerContext& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::TimerContext
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TimerContext& value);

NTCCFG_INLINE
TimerContext::TimerContext()
: d_now()
, d_deadline()
, d_drift()
, d_error()
{
}

NTCCFG_INLINE
TimerContext::TimerContext(const bsls::TimeInterval& now,
                           const bsls::TimeInterval& deadline)
: d_now(now)
, d_deadline(deadline)
, d_drift()
, d_error()
{
}

NTCCFG_INLINE
TimerContext::TimerContext(const bsls::TimeInterval& now,
                           const bsls::TimeInterval& deadline,
                           const bsls::TimeInterval& drift)
: d_now(now)
, d_deadline(deadline)
, d_drift(drift)
, d_error()
{
}

NTCCFG_INLINE
TimerContext::TimerContext(const TimerContext& original)
: d_now(original.d_now)
, d_deadline(original.d_deadline)
, d_drift(original.d_drift)
, d_error(original.d_error)
{
}

NTCCFG_INLINE
TimerContext::~TimerContext()
{
}

NTCCFG_INLINE
TimerContext& TimerContext::operator=(const TimerContext& other)
{
    d_now      = other.d_now;
    d_deadline = other.d_deadline;
    d_drift    = other.d_drift;
    d_error    = other.d_error;
    return *this;
}

NTCCFG_INLINE
void TimerContext::reset()
{
    d_now      = bsls::TimeInterval();
    d_deadline = bsls::TimeInterval();
    d_drift    = bsls::TimeInterval();
    d_error    = ntsa::Error();
}

NTCCFG_INLINE
void TimerContext::setNow(const bsls::TimeInterval& value)
{
    d_now = value;
}

NTCCFG_INLINE
void TimerContext::setDeadline(const bsls::TimeInterval& value)
{
    d_deadline = value;
}

NTCCFG_INLINE
void TimerContext::setDrift(const bsls::TimeInterval& value)
{
    d_drift = value;
}

NTCCFG_INLINE
void TimerContext::setError(const ntsa::Error& value)
{
    d_error = value;
}

NTCCFG_INLINE
const bsls::TimeInterval& TimerContext::now() const
{
    return d_now;
}

NTCCFG_INLINE
const bsls::TimeInterval& TimerContext::deadline() const
{
    return d_deadline;
}

NTCCFG_INLINE
const bsls::TimeInterval& TimerContext::drift() const
{
    return d_drift;
}

NTCCFG_INLINE
const ntsa::Error& TimerContext::error() const
{
    return d_error;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const TimerContext& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const TimerContext& lhs, const TimerContext& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const TimerContext& lhs, const TimerContext& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const TimerContext& lhs, const TimerContext& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TimerContext& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.now());
    hashAppend(algorithm, value.deadline());
    hashAppend(algorithm, value.drift());
    hashAppend(algorithm, value.error());
}

}  // close package namespace
}  // close enterprise namespace
#endif
