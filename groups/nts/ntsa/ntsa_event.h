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
#include <ntsa_event.h>
#include <ntsa_handle.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_cstddef.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a description of a socket event.
///
/// @details
/// This class provies a value-semantic type that describes a socket event, or
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

        /// The socket has an error.
        e_ERROR = 3,

        /// The peer has shutdown the connection.
        e_SHUTDOWN = 4,

        /// The peer has closed its end of the channel.
        e_DISCONNECTED = 5
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

    /// Set the flag indicating the peer has shut down the connection.
    void setShutdown();

    /// Set the flag indicating the socket is disconnected.
    void setDisconnected();

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

    /// Return the flag indicating an error has been detected for
    /// the socket.
    bool isError() const;

    /// Return the flag indicating the peer has shut down the connection.
    bool isShutdown() const;

    /// Return the flag indicating the socket is disconnected.
    bool isDisconnected() const;

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
bsl::ostream& operator<<(bsl::ostream& stream, const ntsa::Event& value);

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
    d_state |= (1 << e_READABLE);
}

NTSCFG_INLINE
void Event::setWritable()
{
    d_state |= (1 << e_WRITABLE);
}

NTSCFG_INLINE
void Event::setShutdown()
{
    d_state |= (1 << e_SHUTDOWN);
}

NTSCFG_INLINE
void Event::setDisconnected()
{
    d_state |= (1 << e_DISCONNECTED);
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
    return ((d_state & (1 << e_READABLE)) != 0);
}

NTSCFG_INLINE
bool Event::isWritable() const
{
    return ((d_state & (1 << e_WRITABLE)) != 0);
}

NTSCFG_INLINE
bool Event::isError() const
{
    return ((d_state & (1 << e_ERROR)) != 0);
}

NTSCFG_INLINE
bool Event::isShutdown() const
{
    return ((d_state & (1 << e_SHUTDOWN)) != 0);
}

NTSCFG_INLINE
bool Event::isDisconnected() const
{
    return ((d_state & (1 << e_DISCONNECTED)) != 0);
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

}  // end namespace ntsa
}  // end namespace BloombergLP
#endif
