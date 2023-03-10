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

#ifndef INCLUDED_NTCA_RECEIVEEVENT
#define INCLUDED_NTCA_RECEIVEEVENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_receivecontext.h>
#include <ntca_receiveeventtype.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe an event detected for an asynchronous receive operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b type:
/// The type of receive event.
///
/// @li @b context:
/// The context of the receive operation at the time of the event.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_receive
class ReceiveEvent
{
    ntca::ReceiveEventType::Value d_type;
    ntca::ReceiveContext          d_context;

  public:
    /// Create a new receive event having the default value.
    ReceiveEvent();

    /// Create a new receive event having the same value as the specified
    /// 'original' object.
    ReceiveEvent(const ReceiveEvent& original);

    /// Destroy this object.
    ~ReceiveEvent();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ReceiveEvent& operator=(const ReceiveEvent& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the type of receive event to the specified 'value'.
    void setType(ntca::ReceiveEventType::Value value);

    /// Set the context of the receive operation at the time of the event to
    /// the specified 'value'.
    void setContext(const ntca::ReceiveContext& value);

    /// Return the type of receive event.
    ntca::ReceiveEventType::Value type() const;

    /// Return the context of the receive operation at the time of the
    /// event.
    const ntca::ReceiveContext& context() const;

    /// Return true if 'type() == ntca::ReceiveEventType::e_COMPLETE', i.e.,
    /// the receive operation successfully completed without an error.
    /// Otherwise, return false.
    bool isComplete() const;

    /// Return true if 'type() == ntca::ReceiveEventType::e_ERROR', i.e.,
    /// the receive operation failed because of an error. Otherwise, return
    /// false. Note that the exact error is stored at 'context().error()'.
    bool isError() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ReceiveEvent& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ReceiveEvent& other) const;

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
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(ReceiveEvent);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::ReceiveEvent
bsl::ostream& operator<<(bsl::ostream& stream, const ReceiveEvent& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ReceiveEvent
bool operator==(const ReceiveEvent& lhs, const ReceiveEvent& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ReceiveEvent
bool operator!=(const ReceiveEvent& lhs, const ReceiveEvent& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ReceiveEvent
bool operator<(const ReceiveEvent& lhs, const ReceiveEvent& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ReceiveEvent
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReceiveEvent& value);

NTCCFG_INLINE
ReceiveEvent::ReceiveEvent()
: d_type(ntca::ReceiveEventType::e_COMPLETE)
, d_context()
{
}

NTCCFG_INLINE
ReceiveEvent::ReceiveEvent(const ReceiveEvent& original)
: d_type(original.d_type)
, d_context(original.d_context)
{
}

NTCCFG_INLINE
ReceiveEvent::~ReceiveEvent()
{
}

NTCCFG_INLINE
ReceiveEvent& ReceiveEvent::operator=(const ReceiveEvent& other)
{
    d_type    = other.d_type;
    d_context = other.d_context;
    return *this;
}

NTCCFG_INLINE
void ReceiveEvent::reset()
{
    d_type = ntca::ReceiveEventType::e_COMPLETE;
    d_context.reset();
}

NTCCFG_INLINE
void ReceiveEvent::setType(ntca::ReceiveEventType::Value value)
{
    d_type = value;
}

NTCCFG_INLINE
void ReceiveEvent::setContext(const ntca::ReceiveContext& context)
{
    d_context = context;
}

NTCCFG_INLINE
ntca::ReceiveEventType::Value ReceiveEvent::type() const
{
    return d_type;
}

NTCCFG_INLINE
const ntca::ReceiveContext& ReceiveEvent::context() const
{
    return d_context;
}

NTCCFG_INLINE
bool ReceiveEvent::isComplete() const
{
    return d_type == ntca::ReceiveEventType::e_COMPLETE;
}

NTCCFG_INLINE
bool ReceiveEvent::isError() const
{
    return d_type == ntca::ReceiveEventType::e_ERROR;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ReceiveEvent& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ReceiveEvent& lhs, const ReceiveEvent& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const ReceiveEvent& lhs, const ReceiveEvent& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const ReceiveEvent& lhs, const ReceiveEvent& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReceiveEvent& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.type());
    hashAppend(algorithm, value.context());
}

}  // close package namespace
}  // close enterprise namespace
#endif
