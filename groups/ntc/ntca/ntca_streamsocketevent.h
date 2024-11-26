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

#ifndef INCLUDED_NTCA_STREAMSOCKETEVENT
#define INCLUDED_NTCA_STREAMSOCKETEVENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_downgradeevent.h>
#include <ntca_errorevent.h>
#include <ntca_readqueueevent.h>
#include <ntca_shutdownevent.h>
#include <ntca_streamsocketeventtype.h>
#include <ntca_writequeueevent.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe an event detected during the asynchronous operation of a stream
/// socket.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_socket
class StreamSocketEvent
{
    union {
        bsls::ObjectBuffer<ntca::ReadQueueEvent>  d_readQueueEvent;
        bsls::ObjectBuffer<ntca::WriteQueueEvent> d_writeQueueEvent;
        bsls::ObjectBuffer<ntca::DowngradeEvent>  d_downgradeEvent;
        bsls::ObjectBuffer<ntca::ShutdownEvent>   d_shutdownEvent;
        bsls::ObjectBuffer<ntca::ErrorEvent>      d_errorEvent;
    };

    ntca::StreamSocketEventType::Value d_type;
    bslma::Allocator*                  d_allocator_p;

  public:
    /// Create a new, initially undefined stream socket event. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit StreamSocketEvent(bslma::Allocator* basicAllocator = 0);

    /// Create a new stream socket have initially represented by the
    /// specified 'readQueueEvent'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit StreamSocketEvent(const ntca::ReadQueueEvent& readQueueEvent,
                               bslma::Allocator*           basicAllocator = 0);

    /// Create a new stream socket have initially represented by the
    /// specified 'writeQueueEvent'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit StreamSocketEvent(const ntca::WriteQueueEvent& writeQueueEvent,
                               bslma::Allocator* basicAllocator = 0);

    /// Create a new stream socket have initially represented by the
    /// specified 'downgradeEvent'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit StreamSocketEvent(const ntca::DowngradeEvent& downgradeEvent,
                               bslma::Allocator*           basicAllocator = 0);

    /// Create a new stream socket have initially represented by the
    /// specified 'shutdownEvent'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit StreamSocketEvent(const ntca::ShutdownEvent& shutdownEvent,
                               bslma::Allocator*          basicAllocator = 0);

    /// Create a new stream socket have initially represented by the
    /// specified 'errorEvent'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit StreamSocketEvent(const ntca::ErrorEvent& errorEvent,
                               bslma::Allocator*       basicAllocator = 0);

    /// Create a new stream socket event having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    StreamSocketEvent(const StreamSocketEvent& original,
                      bslma::Allocator*        basicAllocator = 0);

    /// Destroy this object.
    ~StreamSocketEvent();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    StreamSocketEvent& operator=(const StreamSocketEvent& other);

    /// Make the representation of this object a read queue event having the
    /// same value as the specified 'other' object.  Return the reference
    /// to this modifiable object.
    StreamSocketEvent& operator=(const ntca::ReadQueueEvent& other);

    /// Make the representation of this object a write queue event having
    /// the same value as the specified 'other' object.  Return the
    /// reference to this modifiable object.
    StreamSocketEvent& operator=(const ntca::WriteQueueEvent& other);

    /// Make the representation of this object a downgrade event having the
    /// same value as the specified 'other' object.  Return the reference
    /// to this modifiable object.
    StreamSocketEvent& operator=(const ntca::DowngradeEvent& other);

    /// Make the representation of this object a shutdown event having the
    /// same value as the specified 'other' object.  Return the reference
    /// to this modifiable object.
    StreamSocketEvent& operator=(const ntca::ShutdownEvent& other);

    /// Make the representation of this object an error event having the
    /// same value as the specified 'other' object.  Return the reference
    /// to this modifiable object.
    StreamSocketEvent& operator=(const ntca::ErrorEvent& other);

    /// Reset the value of the this object to its value upon default
    /// construction.
    void reset();

    /// Make the representation of this object match the specified 'type'.
    void make(ntca::StreamSocketEventType::Value type);

    /// Make the representation of this object a read queue event having a
    /// default value.  Return the reference to the modifiable object
    /// represented as a read queue event.
    ntca::ReadQueueEvent& makeReadQueueEvent();

    /// Make the representation of this object a read queue event having the
    /// same value as the specified 'other' object.  Return the reference to
    /// the modifiable object represented as a read queue event.
    ntca::ReadQueueEvent& makeReadQueueEvent(
        const ntca::ReadQueueEvent& other);

    /// Make the representation of this object a write queue event having a
    /// default value.  Return the reference to the modifiable object
    /// represented as a write queue event.
    ntca::WriteQueueEvent& makeWriteQueueEvent();

    /// Make the representation of this object a write queue event having
    /// the same value as the specified 'other' object.  Return the
    /// reference to the modifiable object represented as a write queue
    /// event.
    ntca::WriteQueueEvent& makeWriteQueueEvent(
        const ntca::WriteQueueEvent& other);

    /// Make the representation of this object a downgrade event having a
    /// default value.  Return the reference to the modifiable object
    /// represented as a downgrade event.
    ntca::DowngradeEvent& makeDowngradeEvent();

    /// Make the representation of this object a downgrade event having the
    /// same value as the specified 'other' object.  Return the reference to
    /// the modifiable object represented as a downgrade event.
    ntca::DowngradeEvent& makeDowngradeEvent(
        const ntca::DowngradeEvent& other);

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
    /// an read queue event. The behavior is undefined unless
    /// 'isReadQueueEvent()' is true.
    const ntca::ReadQueueEvent& readQueueEvent() const;

    /// Return the non-modifiable reference to the object represented as
    /// a write queue event. The behavior is undefined unless
    /// 'isWriteQueueEvent()' is true.
    const ntca::WriteQueueEvent& writeQueueEvent() const;

    /// Return the non-modifiable reference to the object represented as
    /// a downgrade queue event. The behavior is undefined unless
    /// 'isDowngradeEvent()' is true.
    const ntca::DowngradeEvent& downgradeEvent() const;

    /// Return the non-modifiable reference to the object represented as
    /// a shutdown event. The behavior is undefined unless
    /// 'isShutdownEvent()' is true.
    const ntca::ShutdownEvent& shutdownEvent() const;

    /// Return the non-modifiable reference to the object represented as
    /// an error event. The behavior is undefined unless
    /// 'isErrorEvent()' is true.
    const ntca::ErrorEvent& errorEvent() const;

    /// Return the stream socket event type.
    ntca::StreamSocketEventType::Value type() const;

    /// Return true if the stream socket event type is undefined, otherwise
    /// return false.
    bool isUndefined() const;

    /// Return true if the stream socket event type is a read queue event,
    /// otherwise return false.
    bool isReadQueueEvent() const;

    /// Return true if the stream socket event type is a write queue event,
    /// otherwise return false.
    bool isWriteQueueEvent() const;

    /// Return true if the stream socket event type is a downgrade event,
    /// otherwise return false.
    bool isDowngradeEvent() const;

    /// Return true if the stream socket event type is a shutdown event,
    /// otherwise return false.
    bool isShutdownEvent() const;

    /// Return true if the stream socket event type is an error event,
    /// otherwise return false.
    bool isErrorEvent() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const StreamSocketEvent& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const StreamSocketEvent& other) const;

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

    /// Return the allocator used by this object.
    bslma::Allocator* allocator() const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(StreamSocketEvent);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::StreamSocketEvent
bsl::ostream& operator<<(bsl::ostream&            stream,
                         const StreamSocketEvent& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::StreamSocketEvent
bool operator==(const StreamSocketEvent& lhs, const StreamSocketEvent& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::StreamSocketEvent
bool operator!=(const StreamSocketEvent& lhs, const StreamSocketEvent& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::StreamSocketEvent
bool operator<(const StreamSocketEvent& lhs, const StreamSocketEvent& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::StreamSocketEvent
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const StreamSocketEvent& value);

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const StreamSocketEvent& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const StreamSocketEvent& lhs, const StreamSocketEvent& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const StreamSocketEvent& lhs, const StreamSocketEvent& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const StreamSocketEvent& lhs, const StreamSocketEvent& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const StreamSocketEvent& value)
{
    using bslh::hashAppend;

    if (value.isReadQueueEvent()) {
        hashAppend(algorithm, value.readQueueEvent());
    }
    else if (value.isWriteQueueEvent()) {
        hashAppend(algorithm, value.writeQueueEvent());
    }
    else if (value.isDowngradeEvent()) {
        hashAppend(algorithm, value.downgradeEvent());
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
