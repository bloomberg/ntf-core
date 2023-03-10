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

#ifndef INCLUDED_NTCA_SHUTDOWNEVENT
#define INCLUDED_NTCA_SHUTDOWNEVENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_shutdowncontext.h>
#include <ntca_shutdowneventtype.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe an event detected for an asynchronous shutdown operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b type:
/// The type of shutdown event.
///
/// @li @b context:
/// The state of shutdown at the time of the event.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_shutdown
class ShutdownEvent
{
    ntca::ShutdownEventType::Value d_type;
    ntca::ShutdownContext          d_context;

  public:
    /// Create a new shutdown event having the default value.
    ShutdownEvent();

    /// Create a new shutdown event having the same value as the specified
    /// 'original' object.
    ShutdownEvent(const ShutdownEvent& original);

    /// Destroy this object.
    ~ShutdownEvent();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ShutdownEvent& operator=(const ShutdownEvent& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the type of shutdown event to the specified 'value'.
    void setType(ntca::ShutdownEventType::Value value);

    /// Set the state of the shutdown sequence at the time of the event to
    /// the specified 'value'.
    void setContext(const ntca::ShutdownContext& value);

    /// Return the type of shutdown event.
    ntca::ShutdownEventType::Value type() const;

    /// Return the state of the shutdown sequence at the time of the event.
    const ntca::ShutdownContext& context() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ShutdownEvent& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ShutdownEvent& other) const;

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
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(ShutdownEvent);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::ShutdownEvent
bsl::ostream& operator<<(bsl::ostream& stream, const ShutdownEvent& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ShutdownEvent
bool operator==(const ShutdownEvent& lhs, const ShutdownEvent& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ShutdownEvent
bool operator!=(const ShutdownEvent& lhs, const ShutdownEvent& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ShutdownEvent
bool operator<(const ShutdownEvent& lhs, const ShutdownEvent& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ShutdownEvent
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ShutdownEvent& value);

NTCCFG_INLINE
ShutdownEvent::ShutdownEvent()
: d_type(ntca::ShutdownEventType::e_INITIATED)
, d_context()
{
}

NTCCFG_INLINE
ShutdownEvent::ShutdownEvent(const ShutdownEvent& original)
: d_type(original.d_type)
, d_context(original.d_context)
{
}

NTCCFG_INLINE
ShutdownEvent::~ShutdownEvent()
{
}

NTCCFG_INLINE
ShutdownEvent& ShutdownEvent::operator=(const ShutdownEvent& other)
{
    d_type    = other.d_type;
    d_context = other.d_context;
    return *this;
}

NTCCFG_INLINE
void ShutdownEvent::reset()
{
    d_type = ntca::ShutdownEventType::e_INITIATED;
    d_context.reset();
}

NTCCFG_INLINE
void ShutdownEvent::setType(ntca::ShutdownEventType::Value value)
{
    d_type = value;
}

NTCCFG_INLINE
void ShutdownEvent::setContext(const ntca::ShutdownContext& context)
{
    d_context = context;
}

NTCCFG_INLINE
ntca::ShutdownEventType::Value ShutdownEvent::type() const
{
    return d_type;
}

NTCCFG_INLINE
const ntca::ShutdownContext& ShutdownEvent::context() const
{
    return d_context;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ShutdownEvent& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ShutdownEvent& lhs, const ShutdownEvent& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const ShutdownEvent& lhs, const ShutdownEvent& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const ShutdownEvent& lhs, const ShutdownEvent& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ShutdownEvent& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.type());
    hashAppend(algorithm, value.context());
}

}  // close package namespace
}  // close enterprise namespace
#endif
