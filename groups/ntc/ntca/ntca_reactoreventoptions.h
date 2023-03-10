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

#ifndef INCLUDED_NTCA_REACTOREVENTOPTIONS
#define INCLUDED_NTCA_REACTOREVENTOPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_reactoreventtrigger.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace ntca {

/// Describe the parameters an operation to gain or lose interest in the state
/// of a socket.
///
/// @details
/// This class provides a value-semantic type that describes the parameters to
/// a reactor event registration. These parameters may be used to explicitly
/// change the trigger mode to "level" or "edge", or to enable or disable
/// "one-shot" mode. Note that both the trigger mode and the one-shot mode, if
/// set through these options, subsequently applies to *all* events for the
/// associated socket, not just the event being registered.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b oneShot:
/// Process events detected by the reactor in one-shot mode: once an event is
/// detected, it must be explicitly re-armed before being detected again. When
/// running the same reactor simulataneously by multiple threads, configuring
/// the reactor in one-shot mode is practically required. The default value is
/// unset, indicating the one-shot mode is inherited from the default one-shot
/// mode of the target reactor.
///
/// @li @b trigger:
/// Specify the conditions that trigger events. When events are
/// level-triggered, the event will occur as long as the conditions for the
/// event continue to be satisfied. When events are edge-triggered, the event
/// is raised when conditions for the event change are first satisfied, but the
/// event is not subsequently raised until the conditions are "reset". The
/// default value is unset, indicating the trigger mode is inherited from the
/// default trigger mode of the target reactor.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_reactor
class ReactorEventOptions
{
    bdlb::NullableValue<ntca::ReactorEventTrigger::Value> d_trigger;
    bdlb::NullableValue<bool>                             d_oneShot;

  public:
    /// Create new receive options having the default value.
    ReactorEventOptions();

    /// Create new receive options having the same value as the specified
    /// 'original' object.
    ReactorEventOptions(const ReactorEventOptions& original);

    /// Destroy this object.
    ~ReactorEventOptions();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ReactorEventOptions& operator=(const ReactorEventOptions& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the trigger of events to the specified 'value'. When events are
    /// level-triggered, the event will occur as long as the conditions for
    /// the event continue to be satisfied. When events are edge-triggered,
    /// the event is raised when conditions for the event change are first
    /// satisfied, but the event is not subsequently raised until the
    /// conditions are "reset". If the trigger mode is not explicitly set
    /// through this function, the effective trigger mode is inherited from
    /// the default trigger mode of the target reactor. Note that the target
    /// reactor must support the specified trigger mode, otherwise event
    /// registration will fail.
    void setTrigger(ntca::ReactorEventTrigger::Value value);

    /// Set the one-shot mode to the specified 'value'. When 'oneShot' mode
    /// is enabled, after a reactor detects the socket is readable or
    /// writable, interest in readability or writability must be explicitly
    /// re-registered before the reactor will again detect the socket is
    /// readable or writable. If the one-shot mode is not explicitly set
    /// through this function, the effective one-shot mode is inherited from
    /// the default one-shot mode of the target reactor. Note that the
    /// target reactor must support the specified one-shot mode, otherwise
    /// event registration will fail.
    void setOneShot(bool value);

    /// Return the trigger mode. When events are level-triggered, the event
    /// will occur as long as the conditions for the event continue to be
    /// satisfied. When events are edge-triggered, the event is raised when
    /// conditions for the event change are first satisfied, but the event
    /// is not subsequently raised until the conditions are "reset". The
    /// default value is unset, indicating the trigger mode is inherited
    /// from the default trigger mode of the target reactor.
    const bdlb::NullableValue<ntca::ReactorEventTrigger::Value>& trigger()
        const;

    /// Return the one-shot mode. When 'oneShot' mode is enabled, after
    /// a reactor detects the socket is readable or writable, interest in
    /// readability or writability must be explicitly re-registered before
    /// the reactor will again detect the socket is readable or writable.
    /// The default value is unset, indicating the one-shot mode is
    /// inherited from the default one-shot mode of the target reactor.
    const bdlb::NullableValue<bool>& oneShot() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ReactorEventOptions& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ReactorEventOptions& other) const;

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
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(ReactorEventOptions);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::ReactorEventOptions
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const ReactorEventOptions& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ReactorEventOptions
bool operator==(const ReactorEventOptions& lhs,
                const ReactorEventOptions& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ReactorEventOptions
bool operator!=(const ReactorEventOptions& lhs,
                const ReactorEventOptions& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ReactorEventOptions
bool operator<(const ReactorEventOptions& lhs, const ReactorEventOptions& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ReactorEventOptions
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReactorEventOptions& value);

NTCCFG_INLINE
ReactorEventOptions::ReactorEventOptions()
: d_trigger()
, d_oneShot()
{
}

NTCCFG_INLINE
ReactorEventOptions::ReactorEventOptions(const ReactorEventOptions& original)
: d_trigger(original.d_trigger)
, d_oneShot(original.d_oneShot)
{
}

NTCCFG_INLINE
ReactorEventOptions::~ReactorEventOptions()
{
}

NTCCFG_INLINE
ReactorEventOptions& ReactorEventOptions::operator=(
    const ReactorEventOptions& other)
{
    d_trigger = other.d_trigger;
    d_oneShot = other.d_oneShot;
    return *this;
}

NTCCFG_INLINE
void ReactorEventOptions::reset()
{
    d_trigger.reset();
    d_oneShot.reset();
}

NTCCFG_INLINE
void ReactorEventOptions::setTrigger(ntca::ReactorEventTrigger::Value value)
{
    d_trigger = value;
}

NTCCFG_INLINE
void ReactorEventOptions::setOneShot(bool value)
{
    d_oneShot = value;
}

NTCCFG_INLINE
const bdlb::NullableValue<ntca::ReactorEventTrigger::Value>& ReactorEventOptions::
    trigger() const
{
    return d_trigger;
}

NTCCFG_INLINE
const bdlb::NullableValue<bool>& ReactorEventOptions::oneShot() const
{
    return d_oneShot;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const ReactorEventOptions& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ReactorEventOptions& lhs, const ReactorEventOptions& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const ReactorEventOptions& lhs, const ReactorEventOptions& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const ReactorEventOptions& lhs, const ReactorEventOptions& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReactorEventOptions& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.trigger());
    hashAppend(algorithm, value.oneShot());
}

}  // close package namespace
}  // close enterprise namespace
#endif
