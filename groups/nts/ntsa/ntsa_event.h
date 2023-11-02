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

#ifndef INCLUDED_NTSA_EVENT
#define INCLUDED_NTSA_EVENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_handle.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_map.h>
#include <bsl_set.h>
#include <bsl_cstddef.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a description of a socket event.
///
/// @details
/// This class provides a value-semantic type that describes a socket event, or
/// condition, detected when polling a socket.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class Event
{
    /// Provide an enumeration the states of a socket.
    enum State {
        /// The socket is readable.
        e_READABLE = 1,

        /// The socket is writable.
        e_WRITABLE = 2,

        /// The socket has an exceptional condition.
        e_EXCEPTIONAL = 3,

        /// The socket has an error or an exceptional condition.
        e_ERROR = 4,

        /// The remote socket has shut down writing from its side of the 
        /// connection.
        e_SHUTDOWN = 5,

        /// Both the source socket and the remote socket have shut down writing
        /// from their sides of the connection and all data has been received;
        /// no more data may be sent or received.
        e_HANGUP = 6
    };

    ntsa::Handle                     d_handle;
    bsl::uint32_t                    d_state;
    bdlb::NullableValue<bsl::size_t> d_bytesReadable;
    bdlb::NullableValue<bsl::size_t> d_bytesWritable;
    bdlb::NullableValue<bsl::size_t> d_backlog;
    ntsa::Error                      d_error;

  public:
    /// Create a new event having a default value.
    Event();

    /// Create a new event having the same value as the specified 'original'
    /// object.
    Event(const Event& original);

    /// Destroy this object.
    ~Event();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Event& operator=(const Event& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the handle for the socket to the specified 'value'.
    void setHandle(ntsa::Handle value);

    /// Set the flag indicating the socket is readable.
    void setReadable();

    /// Set the flag indicating the socket is writable.
    void setWritable();

    /// Set the flag indicating the socket is exceptional.
    void setExceptional();

    /// Set the flag indicating the peer has shut down the connection.
    void setShutdown();

    /// Set the flag indicating the socket is disconnected.
    void setHangup();

    /// Set the number of bytes readable from the socket to the specified
    /// 'value'.
    void setBytesReadable(bsl::size_t value);

    /// Set the number of bytes writable to the socket to the specified
    /// 'value'.
    void setBytesWritable(bsl::size_t value);

    /// Set the number of connections in the socket backlog to the specified
    /// 'value'.
    void setBacklog(bsl::size_t backlog);

    /// Set the error detected for the socket at the time of the event to
    /// the specified 'value'.
    void setError(const ntsa::Error& error);

    /// Merge this event with the specified 'event'. Return the error.
    ntsa::Error merge(const ntsa::Event& event);

    /// Return the socket handle.
    ntsa::Handle handle() const;

    /// Return the event state.
    bsl::uint32_t state() const;

    /// Return the number of bytes readable from the socket, if known.
    const bdlb::NullableValue<bsl::size_t>& bytesReadable() const;

    /// Return the number of bytes writable to the socket, if known.
    const bdlb::NullableValue<bsl::size_t>& bytesWritable() const;

    /// Return the number of connections in the socket backlog, if known.
    const bdlb::NullableValue<bsl::size_t>& backlog() const;

    /// Return the error detected for the socket, if any. Note that errors
    /// may be detected for readble or writable events in addition to
    /// error events.
    const ntsa::Error& error() const;

    /// Return the flag indicating the socket is readable.
    bool isReadable() const;

    /// Return the flag indicating the socket is writable.
    bool isWritable() const;

    /// Return the flag indicating the socket is exceptional. 
    bool isExceptional() const;

    /// Return the flag indicating an error has been detected for
    /// the socket.
    bool isError() const;

    /// Return the flag indicating the peer has shut down the connection.
    bool isShutdown() const;

    /// Return the flag indicating the socket is disconnected.
    bool isHangup() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Event& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Event& other) const;

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
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(Event);
};

/// Insert a formatted, human-readable description of the specified 'value'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::Event
bsl::ostream& operator<<(bsl::ostream& stream, const Event& value);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Event
bool operator==(const Event& lhs, const Event& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Event
bool operator!=(const Event& lhs, const Event& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::Event
bool operator<(const Event& lhs, const Event& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Event
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Event& value);

/// Provide a set of events.
///
/// @details
/// This class provides a data structure to represent a set of events polled
/// from a reactor.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class EventSet
{
    typedef bsl::map<ntsa::Handle, ntsa::Event> Map;

    Map               d_map;
    bslma::Allocator *d_allocator_p;

  public:
    /// Create a new, initially empty event set. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit EventSet(bslma::Allocator* basicAllocator = 0);

    /// Create a new event set having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    EventSet(const EventSet& original, bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EventSet();

    // Assign the value of the specified 'other' event set to this object.
    // Return a reference to this modifiable object.
    EventSet& operator=(const EventSet& other);

    /// Remove all events from the set.
    void clear();

    /// Unconditionally insert the specified 'event' into the set. 
    void overwrite(const ntsa::Event& event);

    /// Merge the specified 'event' with the existing event, if any, in the
    /// set. 
    void merge(const ntsa::Event& event);

    /// Set the flag indicating the specified 'socket' is readable.
    void setReadable(ntsa::Handle socket);

    /// Set the flag indicating the specified 'socket' is readable.
    void setReadable(ntsa::Handle socket, bsl::size_t units);

    /// Set the flag indicating the specified 'socket' is writable.
    void setWritable(ntsa::Handle socket);

    /// Set the flag indicating the specified 'socket' is writable.
    void setWritable(ntsa::Handle socket, bsl::size_t units);

    /// Set the flag indicating the specified 'socket' is exceptional.
    void setExceptional(ntsa::Handle socket);

    /// Set the flag indicating the peer of the specified 'socket' has shut
    /// down the connection.
    void setShutdown(ntsa::Handle socket);

    /// Set the flag indicating the specified 'socket' is disconnected.
    void setHangup(ntsa::Handle socket);

    /// Set the error detected for the specified 'socket' at the time of the
    /// event to the specified 'value'.
    void setError(ntsa::Handle socket, const ntsa::Error& error);

    /// Return the flag indicating the socket is readable.
    bool isReadable(ntsa::Handle socket) const;

    /// Return the flag indicating the socket is writable.
    bool isWritable(ntsa::Handle socket) const;

    /// Return the flag indicating the socket is exceptional. 
    bool isExceptional(ntsa::Handle socket) const;

    /// Return the flag indicating an error has been detected for
    /// the socket.
    bool isError(ntsa::Handle socket) const;

    /// Return the flag indicating the peer has shut down the connection.
    bool isShutdown(ntsa::Handle socket) const;

    /// Return the flag indicating the socket is disconnected.
    bool isHangup(ntsa::Handle socket) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EventSet& other) const;

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
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EventSet);
};

/// Insert a formatted, human-readable description of the specified 'value'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::EventSet
bsl::ostream& operator<<(bsl::ostream& stream, const EventSet& value);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::EventSet
bool operator==(const EventSet& lhs, const EventSet& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::EventSet
bool operator!=(const EventSet& lhs, const EventSet& rhs);

NTSCFG_INLINE
Event::Event()
: d_handle(ntsa::k_INVALID_HANDLE)
, d_state(0)
, d_bytesReadable()
, d_bytesWritable()
, d_backlog()
, d_error()
{
}

NTSCFG_INLINE
Event::Event(const Event& original)
: d_handle(original.d_handle)
, d_state(original.d_state)
, d_bytesReadable(original.d_bytesReadable)
, d_bytesWritable(original.d_bytesWritable)
, d_backlog(original.d_backlog)
, d_error(original.d_error)
{
}

NTSCFG_INLINE
Event::~Event()
{
}

NTSCFG_INLINE
Event& Event::operator=(const Event& other)
{
    if (this != &other) {
        d_handle        = other.d_handle;
        d_state         = other.d_state;
        d_bytesReadable = other.d_bytesReadable;
        d_bytesWritable = other.d_bytesWritable;
        d_backlog       = other.d_backlog;
        d_error         = other.d_error;
    }
    return *this;
}

NTSCFG_INLINE
void Event::reset()
{
    d_handle = ntsa::k_INVALID_HANDLE;
    d_state  = 0;
    d_bytesReadable.reset();
    d_bytesWritable.reset();
    d_backlog.reset();
    d_error = ntsa::Error();
}

NTSCFG_INLINE
void Event::setHandle(ntsa::Handle value)
{
    d_handle = value;
}

NTSCFG_INLINE
void Event::setReadable()
{
    d_state |= (1U << e_READABLE);
}

NTSCFG_INLINE
void Event::setWritable()
{
    d_state |= (1U << e_WRITABLE);
}

NTSCFG_INLINE
void Event::setExceptional()
{
    d_state |= (1U << e_EXCEPTIONAL);
}

NTSCFG_INLINE
void Event::setShutdown()
{
    d_state |= (1U << e_SHUTDOWN);
}

NTSCFG_INLINE
void Event::setHangup()
{
    d_state |= (1U << e_HANGUP);
}

NTSCFG_INLINE
void Event::setBytesReadable(bsl::size_t value)
{
    d_bytesReadable = value;
}

NTSCFG_INLINE
void Event::setBytesWritable(bsl::size_t value)
{
    d_bytesWritable = value;
}

NTSCFG_INLINE
void Event::setBacklog(bsl::size_t value)
{
    d_backlog = value;
}

NTSCFG_INLINE
void Event::setError(const ntsa::Error& error)
{
    d_state |= (1 << e_ERROR);
    d_error = error;
}

NTSCFG_INLINE
ntsa::Handle Event::handle() const
{
    return d_handle;
}

NTSCFG_INLINE
bsl::uint32_t Event::state() const
{
    return d_state;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& Event::bytesReadable() const
{
    return d_bytesReadable;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& Event::bytesWritable() const
{
    return d_bytesWritable;
}

NTSCFG_INLINE
const ntsa::Error& Event::error() const
{
    return d_error;
}

NTSCFG_INLINE
bool Event::isReadable() const
{
    return ((d_state & (1U << e_READABLE)) != 0);
}

NTSCFG_INLINE
bool Event::isWritable() const
{
    return ((d_state & (1U << e_WRITABLE)) != 0);
}

NTSCFG_INLINE
bool Event::isExceptional() const
{
    return ((d_state & (1U << e_EXCEPTIONAL)) != 0);
}

NTSCFG_INLINE
bool Event::isError() const
{
    return ((d_state & (1U << e_ERROR)) != 0);
}

NTSCFG_INLINE
bool Event::isShutdown() const
{
    return ((d_state & (1U << e_SHUTDOWN)) != 0);
}

NTSCFG_INLINE
bool Event::isHangup() const
{
    return ((d_state & (1U << e_HANGUP)) != 0);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Event& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const Event& lhs, const Event& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const Event& lhs, const Event& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const Event& lhs, const Event& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Event& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.handle());
    hashAppend(algorithm, value.state());
    hashAppend(algorithm, value.bytesReadable());
    hashAppend(algorithm, value.bytesWritable());
    hashAppend(algorithm, value.backlog());
    hashAppend(algorithm, value.error());
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const EventSet& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const EventSet& lhs, const EventSet& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const EventSet& lhs, const EventSet& rhs)
{
    return !operator==(lhs, rhs);
}

}  // end namespace ntsa
}  // end namespace BloombergLP
#endif
