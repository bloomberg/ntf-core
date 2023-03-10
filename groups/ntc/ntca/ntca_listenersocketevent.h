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

#ifndef INCLUDED_NTCA_LISTENERSOCKETEVENT
#define INCLUDED_NTCA_LISTENERSOCKETEVENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_acceptqueueevent.h>
#include <ntca_errorevent.h>
#include <ntca_listenersocketeventtype.h>
#include <ntca_shutdownevent.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe an event detected during the asynchronous operation of a listener
/// socket.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_socket
class ListenerSocketEvent
{
    union {
        bsls::ObjectBuffer<ntca::AcceptQueueEvent> d_acceptQueueEvent;
        bsls::ObjectBuffer<ntca::ShutdownEvent>    d_shutdownEvent;
        bsls::ObjectBuffer<ntca::ErrorEvent>       d_errorEvent;
    };

    ntca::ListenerSocketEventType::Value d_type;
    bslma::Allocator*                    d_allocator_p;

  public:
    /// Create a new, initially undefined listener socket event. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit ListenerSocketEvent(bslma::Allocator* basicAllocator = 0);

    /// Create a new listener socket have initially represented by the
    /// specified 'acceptQueueEvent'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit ListenerSocketEvent(
        const ntca::AcceptQueueEvent& acceptQueueEvent,
        bslma::Allocator*             basicAllocator = 0);

    /// Create a new listener socket have initially represented by the
    /// specified 'shutdownEvent'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit ListenerSocketEvent(const ntca::ShutdownEvent& shutdownEvent,
                                 bslma::Allocator* basicAllocator = 0);

    /// Create a new listener socket have initially represented by the
    /// specified 'errorEvent'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit ListenerSocketEvent(const ntca::ErrorEvent& errorEvent,
                                 bslma::Allocator*       basicAllocator = 0);

    /// Create a new listener socket event having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    ListenerSocketEvent(const ListenerSocketEvent& original,
                        bslma::Allocator*          basicAllocator = 0);

    /// Destroy this object.
    ~ListenerSocketEvent();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ListenerSocketEvent& operator=(const ListenerSocketEvent& other);

    /// Make the representation of this object an accept queue event having
    /// the same value as the specified 'other' object.  Return the
    /// reference to this modifiable object.
    ListenerSocketEvent& operator=(const ntca::AcceptQueueEvent& other);

    /// Make the representation of this object a shutdown event having the
    /// same value as the specified 'other' object.  Return the reference
    /// to this modifiable object.
    ListenerSocketEvent& operator=(const ntca::ShutdownEvent& other);

    /// Make the representation of this object an error event having the
    /// same value as the specified 'other' object.  Return the reference
    /// to this modifiable object.
    ListenerSocketEvent& operator=(const ntca::ErrorEvent& other);

    /// Reset the value of the this object to its value upon default
    /// construction.
    void reset();

    /// Make the representation of this object match the specified 'type'.
    void make(ntca::ListenerSocketEventType::Value type);

    /// Make the representation of this object an accept queue event having
    /// a default value.  Return the reference to the modifiable object
    /// represented as a read queue event.
    ntca::AcceptQueueEvent& makeAcceptQueueEvent();

    /// Make the representation of this object an accept queue event having
    /// the same value as the specified 'other' object.  Return the
    /// reference to the modifiable object represented as a read queue
    /// event.
    ntca::AcceptQueueEvent& makeAcceptQueueEvent(
        const ntca::AcceptQueueEvent& other);

    /// Make the representation of this object a shutdown event having a
    /// default value.  Return the reference to the modifiable object
    /// represented as a shutdown event.
    ntca::ShutdownEvent& makeShutdownEvent();

    /// Make the representation of this object a shutdown event having the
    /// same value as the specified 'other' object.  Return the reference to
    /// the modifiable object represented as a shutdown event.
    ntca::ShutdownEvent& makeShutdownEvent(const ntca::ShutdownEvent& other);

    /// Make the representation of this object an error event having a
    /// default value.  Return the reference to the modifiable object
    /// represented as an error event.
    ntca::ErrorEvent& makeErrorEvent();

    /// Make the representation of this object an error event having the
    /// same value as the specified 'other' object.  Return the reference to
    /// the modifiable object represented as an error event.
    ntca::ErrorEvent& makeErrorEvent(const ntca::ErrorEvent& other);

    /// Return the non-modifiable reference to the object represented as
    /// an accept queue event. The behavior is undefined unless
    /// 'isAcceptQueueEvent()' is true.
    const ntca::AcceptQueueEvent& acceptQueueEvent() const;

    /// Return the non-modifiable reference to the object represented as
    /// a shutdown event. The behavior is undefined unless
    /// 'isShutdownEvent()' is true.
    const ntca::ShutdownEvent& shutdownEvent() const;

    /// Return the non-modifiable reference to the object represented as
    /// an error event. The behavior is undefined unless
    /// 'isErrorEvent()' is true.
    const ntca::ErrorEvent& errorEvent() const;

    /// Return the listener socket event type.
    ntca::ListenerSocketEventType::Value type() const;

    /// Return true if the listener socket event type is undefined, otherwise
    /// return false.
    bool isUndefined() const;

    /// Return true if the listener socket event type is an accept queue
    /// event, otherwise return false.
    bool isAcceptQueueEvent() const;

    /// Return true if the listener socket event type is a shutdown event,
    /// otherwise return false.
    bool isShutdownEvent() const;

    /// Return true if the listener socket event type is an error event,
    /// otherwise return false.
    bool isErrorEvent() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ListenerSocketEvent& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ListenerSocketEvent& other) const;

    /// Format this object to the specified output 'listener' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'listener'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'listener' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& listener,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Return the allocator used by this object.
    bslma::Allocator* allocator() const;

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(ListenerSocketEvent);
};

/// Write the specified 'object' to the specified 'listener'. Return
/// a modifiable reference to the 'listener'.
///
/// @related ntca::ListenerSocketEvent
bsl::ostream& operator<<(bsl::ostream&              listener,
                         const ListenerSocketEvent& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ListenerSocketEvent
bool operator==(const ListenerSocketEvent& lhs,
                const ListenerSocketEvent& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ListenerSocketEvent
bool operator!=(const ListenerSocketEvent& lhs,
                const ListenerSocketEvent& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ListenerSocketEvent
bool operator<(const ListenerSocketEvent& lhs, const ListenerSocketEvent& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ListenerSocketEvent
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ListenerSocketEvent& value);

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&              listener,
                         const ListenerSocketEvent& object)
{
    return object.print(listener, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ListenerSocketEvent& lhs, const ListenerSocketEvent& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const ListenerSocketEvent& lhs, const ListenerSocketEvent& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const ListenerSocketEvent& lhs, const ListenerSocketEvent& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ListenerSocketEvent& value)
{
    using bslh::hashAppend;

    if (value.isAcceptQueueEvent()) {
        hashAppend(algorithm, value.acceptQueueEvent());
    }
    else if (value.isShutdownEvent()) {
        hashAppend(algorithm, value.shutdownEvent());
    }
    else if (value.isErrorEvent()) {
        hashAppend(algorithm, value.errorEvent());
    }
    else {
        hashAppend(algorithm, 0);
    }
}

}  // close package namespace
}  // close enterprise namespace
#endif
