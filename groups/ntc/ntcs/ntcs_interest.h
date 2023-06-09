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

#ifndef INCLUDED_NTCS_INTEREST
#define INCLUDED_NTCS_INTEREST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_reactoreventtrigger.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntcs {

/// Describe the interest in socket events.
///
/// @details
/// Provide a value-semantic type that descibes the interest in a
/// socket's events, including the trigger condition (i.e., level-triggered or
///  edge-triggered) and whether events are delivered in normal mode or
/// one-shot mode.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class Interest
{
    enum Flag {
        // Defines the flags used by this object.

        e_READABLE     = 1,
        e_WRITABLE     = 2,
        e_ERROR        = 4,
        e_EDGE         = 8,
        e_ONE_SHOT     = 16,
        e_NOTIFICATION = 32
    };

    bsl::uint32_t d_value;

  public:
    /// Create a new object initially representing interest in errors, using
    /// level-triggering in normal (not one-shot) mode.
    Interest();

    /// Create a new object initially representing interest in errors, using
    /// the specified 'trigger' mode and 'oneShot' mode.
    Interest(ntca::ReactorEventTrigger::Value trigger, bool oneShot);

    /// Create a new object having the same value as the specified
    /// 'original' object.
    Interest(const Interest& original);

    /// Destroy this object.
    ~Interest();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Interest& operator=(const Interest& other);

    /// Set the trigger of events to the specified 'value'. When events are
    /// level-triggered, the event will occur as long as the conditions for
    /// the event continue to be satisfied. When events are edge-triggered,
    /// the event is raised when conditions for the event change are first
    /// satisfied, but the event is not subsequently raised until the
    /// conditions are "reset".
    void setTrigger(ntca::ReactorEventTrigger::Value value);

    /// Set the one-shot mode to the specified 'value'. When 'oneShot' mode
    /// is enabled, after a reactor detects the socket is readable or
    /// writable, interest in readability or writability must be explicitly
    /// re-registered before the reactor will again detect the socket is
    /// readable or writable.
    void setOneShot(bool value);

    /// Gain interest in readability. A socket is readable when the size of
    /// its receive buffer is greater than or equal to the receive
    /// low watermark set for the socket.
    void showReadable();

    /// Lose interest in readability.
    void hideReadable();

    /// Gain interest in writability. A socket is writable when the capacity
    /// of its send buffer is greater than or equal to the send buffer
    /// low watermark set for the socket.
    void showWritable();

    /// Lose interest in writability.
    void hideWritable();

    /// Gain interest in errors.
    void showError();

    /// Lose interest in errors.
    void hideError();

    /// Gain interest in notifications.
    void showNotifications();

    /// Lose interest in notifications.
    void hideNotifications();

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Return true if there is interest in the readability of the socket,
    /// otherwise return false. A socket is readable when the size of
    /// its receive buffer is greater than or equal to the receive
    /// low watermark set for the socket.
    bool wantReadable() const;

    /// Return true if there is interest in the writability of the socket,
    /// otherwise return false. A socket is writable when the capacity
    /// of its send buffer is greater than or equal to the send buffer
    /// low watermark set for the socket.
    bool wantWritable() const;

    /// Return true if there is interest in either the readability or the
    /// writability of the socket. A socket is readable when the size of
    /// its receive buffer is greater than or equal to the receive
    /// low watermark set for the socket. A socket is writable when the
    /// capacity of its send buffer is greater than or equal to the send
    /// buffer low watermark set for the socket.
    bool wantReadableOrWritable() const;

    /// Return true if there is interest in errors that have been detected
    /// for the socket.
    bool wantError() const;

    /// Return true if there is interest in notifications that has been
    /// detected for the socket.
    bool wantNotifications() const;

    /// Return the trigger mode. When events are level-triggered, the event
    /// will occur as long as the conditions for the event continue to be
    /// satisfied. When events are edge-triggered, the event is raised when
    /// conditions for the event change are first satisfied, but the event
    /// is not subsequently raised until the conditions are "reset".
    ntca::ReactorEventTrigger::Value trigger() const;

    /// Return the one-shot mode. When 'oneShot' mode is enabled, after a
    /// reactor detects the socket is readable or writable, interest in
    /// readability or writability must be explicitly re-registered before
    /// the reactor will again detect the socket is readable or writable.
    bool oneShot() const;

    /// Return the value of this object.
    uint32_t value() const;

    /// Return the string representation of this object.
    bsl::string text() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Interest& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Interest& other) const;

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
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(Interest);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
bsl::ostream& operator<<(bsl::ostream& stream, const Interest& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
bool operator==(const Interest& lhs, const Interest& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
bool operator!=(const Interest& lhs, const Interest& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
bool operator<(const Interest& lhs, const Interest& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Interest& value);

NTCCFG_INLINE
Interest::Interest()
: d_value(e_ERROR | e_NOTIFICATION)
{
}

NTCCFG_INLINE
Interest::Interest(ntca::ReactorEventTrigger::Value trigger, bool oneShot)
: d_value(e_ERROR | e_NOTIFICATION)
{
    if (trigger == ntca::ReactorEventTrigger::e_EDGE) {
        d_value |= e_EDGE;
    }

    if (oneShot) {
        d_value |= e_ONE_SHOT;
    }
}

NTCCFG_INLINE
Interest::Interest(const Interest& original)
: d_value(original.d_value)
{
}

NTCCFG_INLINE
Interest::~Interest()
{
}

NTCCFG_INLINE
Interest& Interest::operator=(const Interest& other)
{
    d_value = other.d_value;
    return *this;
}

NTCCFG_INLINE
void Interest::setTrigger(ntca::ReactorEventTrigger::Value value)
{
    if (value == ntca::ReactorEventTrigger::e_EDGE) {
        d_value |= e_EDGE;
    }
    else {
        d_value &= ~e_EDGE;
    }
}

NTCCFG_INLINE
void Interest::setOneShot(bool value)
{
    if (value) {
        d_value |= e_ONE_SHOT;
    }
    else {
        d_value &= ~e_ONE_SHOT;
    }
}

NTCCFG_INLINE
void Interest::showReadable()
{
    d_value |= e_READABLE;
}

NTCCFG_INLINE
void Interest::hideReadable()
{
    d_value &= ~e_READABLE;
}

NTCCFG_INLINE
void Interest::showWritable()
{
    d_value |= e_WRITABLE;
}

NTCCFG_INLINE
void Interest::hideWritable()
{
    d_value &= ~e_WRITABLE;
}

NTCCFG_INLINE
void Interest::showError()
{
    d_value |= e_ERROR;
}

NTCCFG_INLINE
void Interest::hideError()
{
    d_value &= ~e_ERROR;
}

NTCCFG_INLINE
void Interest::showNotifications()
{
    d_value |= e_NOTIFICATION;
}

NTCCFG_INLINE
void Interest::hideNotifications()
{
    d_value &= ~e_NOTIFICATION;
}

NTCCFG_INLINE
void Interest::reset()
{
    d_value = e_ERROR;
}

NTCCFG_INLINE
bool Interest::wantReadable() const
{
    return (d_value & e_READABLE) != 0;
}

NTCCFG_INLINE
bool Interest::wantWritable() const
{
    return (d_value & e_WRITABLE) != 0;
}

NTCCFG_INLINE
bool Interest::wantReadableOrWritable() const
{
    return (d_value & (e_READABLE | e_WRITABLE)) != 0;
}

NTCCFG_INLINE
bool Interest::wantError() const
{
    return (d_value & e_ERROR) != 0;
}

NTCCFG_INLINE
bool Interest::wantNotifications() const
{
    return (d_value & e_NOTIFICATION) != 0;
}

NTCCFG_INLINE
ntca::ReactorEventTrigger::Value Interest::trigger() const
{
    if ((d_value & e_EDGE) != 0) {
        return ntca::ReactorEventTrigger::e_EDGE;
    }
    else {
        return ntca::ReactorEventTrigger::e_LEVEL;
    }
}

NTCCFG_INLINE
bool Interest::oneShot() const
{
    return (d_value & e_ONE_SHOT) != 0;
}

NTCCFG_INLINE
uint32_t Interest::value() const
{
    return d_value;
}

NTCCFG_INLINE
bsl::string Interest::text() const
{
    bsl::stringstream ss;
    ss << *this;
    return ss.str();
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Interest& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const Interest& lhs, const Interest& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const Interest& lhs, const Interest& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const Interest& lhs, const Interest& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Interest& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.value());
}

}  // close package namespace
}  // close enterprise namespace
#endif
