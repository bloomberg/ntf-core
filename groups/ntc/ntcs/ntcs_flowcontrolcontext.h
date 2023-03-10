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

#ifndef INCLUDED_NTCS_FLOWCONTROLCONTEXT
#define INCLUDED_NTCS_FLOWCONTROLCONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Describe the result of the application of flow control.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b enableSend:
/// The desire to proactively write to the socket or to react when the socket
/// is writable.
///
/// @li @b enableReceive:
/// The desire to proactively read from the socket or to react when the socket
/// is readable.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class FlowControlContext
{
    bool d_enableSend;
    bool d_enableReceive;

  public:
    /// Create new send options having the default value.
    FlowControlContext();

    /// Create new send options having the same value as the specified
    /// 'original' object.
    FlowControlContext(const FlowControlContext& original);

    /// Destroy this object.
    ~FlowControlContext();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    FlowControlContext& operator=(const FlowControlContext& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the flag indicating the desire to proactively write to the
    /// socket or to react when the socket is writable to the specified
    /// 'value'.
    void setEnableSend(bool value);

    /// Set the flag indicating the desire to proactively read from the
    /// socket or to react when the socket is readable to the specified
    /// 'value'.
    void setEnableReceive(bool value);

    /// Return the flag indicating the desire to proactivly write to the
    /// socket or to react when the socket is writable.
    bool enableSend() const;

    /// Return the flag indicating the desire to proactivly write to the
    /// socket or to react when the socket is writable.
    bool enableReceive() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const FlowControlContext& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const FlowControlContext& other) const;

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
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(FlowControlContext);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntcs::FlowControlContext
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const FlowControlContext& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntcs::FlowControlContext
bool operator==(const FlowControlContext& lhs, const FlowControlContext& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntcs::FlowControlContext
bool operator!=(const FlowControlContext& lhs, const FlowControlContext& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntcs::FlowControlContext
bool operator<(const FlowControlContext& lhs, const FlowControlContext& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntcs::FlowControlContext
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const FlowControlContext& value);

NTCCFG_INLINE
FlowControlContext::FlowControlContext()
: d_enableSend(false)
, d_enableReceive(false)
{
}

NTCCFG_INLINE
FlowControlContext::FlowControlContext(const FlowControlContext& original)
: d_enableSend(original.d_enableSend)
, d_enableReceive(original.d_enableReceive)
{
}

NTCCFG_INLINE
FlowControlContext::~FlowControlContext()
{
}

NTCCFG_INLINE
FlowControlContext& FlowControlContext::operator=(
    const FlowControlContext& other)
{
    d_enableSend    = other.d_enableSend;
    d_enableReceive = other.d_enableReceive;
    return *this;
}

NTCCFG_INLINE
void FlowControlContext::reset()
{
    d_enableSend    = false;
    d_enableReceive = false;
}

NTCCFG_INLINE
void FlowControlContext::setEnableSend(bool value)
{
    d_enableSend = value;
}

NTCCFG_INLINE
void FlowControlContext::setEnableReceive(bool value)
{
    d_enableReceive = value;
}

NTCCFG_INLINE
bool FlowControlContext::enableSend() const
{
    return d_enableSend;
}

NTCCFG_INLINE
bool FlowControlContext::enableReceive() const
{
    return d_enableReceive;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const FlowControlContext& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const FlowControlContext& lhs, const FlowControlContext& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const FlowControlContext& lhs, const FlowControlContext& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const FlowControlContext& lhs, const FlowControlContext& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const FlowControlContext& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.enableSend());
    hashAppend(algorithm, value.enableReceive());
}

}  // close package namespace
}  // close enterprise namespace
#endif
