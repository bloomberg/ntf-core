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

#ifndef INCLUDED_NTCA_REACTOREVENT
#define INCLUDED_NTCA_REACTOREVENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_reactorcontext.h>
#include <ntca_reactoreventtype.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <ntsa_handle.h>
#include <bslh_hash.h>
#include <bsl_cstddef.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Define a type alias for a mask of socket events.
///
/// @ingroup module_ntci_reactor
typedef bsl::size_t ReactorEventMask;

/// Describe an event detected for a reactor socket.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_reactor
class ReactorEvent
{
    ntsa::Handle                  d_handle;
    ntca::ReactorEventType::Value d_type;
    ntca::ReactorContext          d_context;

  public:
    /// Create a new event having a default value.
    ReactorEvent();

    /// Create a new event describing an event of the specified 'type' for
    /// the specified 'handle'.
    ReactorEvent(ntsa::Handle handle, ntca::ReactorEventType::Value type);

    /// Create a new event describing an event of the specified 'type'
    /// having the specified 'error' for the specified 'handle'.
    ReactorEvent(ntsa::Handle                  handle,
                 ntca::ReactorEventType::Value type,
                 const ntsa::Error&            error);

    /// Create a new event describing an event of the specified 'type'
    /// having the specified 'context' for the specified 'handle'.
    ReactorEvent(ntsa::Handle                  handle,
                 ntca::ReactorEventType::Value type,
                 const ntca::ReactorContext&   context);

    /// Create a new event having the same value as the specified 'original'
    /// object.
    ReactorEvent(const ReactorEvent& original);

    /// Destroy this object.
    ~ReactorEvent();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ReactorEvent& operator=(const ReactorEvent& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the handle for the socket to the specified
    /// 'handle'.
    void setHandle(ntsa::Handle handle);

    /// Set the type of the event to the specified 'type'.
    void setType(ntca::ReactorEventType::Value type);

    /// Set the number of bytes readable from the socket to the specified
    /// 'value'.
    void setBytesReadable(bsl::size_t bytesReadable);

    /// Set the number of bytes writable to the socket to the specified
    /// 'value'.
    void setBytesWritable(bsl::size_t bytesWritable);

    /// Set the error detected for the socket at the time of the event to
    /// the specified 'value'.
    void setError(const ntsa::Error& error);

    /// Set the context of the event to the specified 'context'.
    void setContext(const ntca::ReactorContext& context);

    /// Return the socket handle.
    ntsa::Handle handle() const;

    /// Return the event type.
    ntca::ReactorEventType::Value type() const;

    /// Return the number of bytes readable from the socket, if known.
    const bdlb::NullableValue<bsl::size_t>& bytesReadable() const;

    /// Return the number of bytes writable to the socket, if known.
    const bdlb::NullableValue<bsl::size_t>& bytesWritable() const;

    /// Return the error detected for the socket, if any. Note that errors
    /// may be detected for readble or writable events in addition to
    /// error events.
    const ntsa::Error& error() const;

    /// Return the event context.
    const ntca::ReactorContext& context() const;

    /// Return the flag indicating whether the socket is readable.
    bool isReadable() const;

    /// Return the flag indicating whether the socket is writable.
    bool isWritable() const;

    /// Return the flag indicating whether an error has been detected for
    /// the socket.
    bool isError() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ReactorEvent& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ReactorEvent& other) const;

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

    /// Return true indicates readable or writable, otherwise return false.
    static bool isReadableOrWritable(ntca::ReactorEventMask mask);

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(ReactorEvent);
};

/// Insert a formatted, human-readable description of the specified 'value'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntca::ReactorEvent
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const ntca::ReactorEvent& value);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ReactorEvent
bool operator==(const ReactorEvent& lhs, const ReactorEvent& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ReactorEvent
bool operator!=(const ReactorEvent& lhs, const ReactorEvent& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ReactorEvent
bool operator<(const ReactorEvent& lhs, const ReactorEvent& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ReactorEvent
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReactorEvent& value);

NTCCFG_INLINE
ReactorEvent::ReactorEvent()
: d_handle(ntsa::k_INVALID_HANDLE)
, d_type(ntca::ReactorEventType::e_NONE)
, d_context()
{
}

NTCCFG_INLINE
ReactorEvent::ReactorEvent(ntsa::Handle                  handle,
                           ntca::ReactorEventType::Value type)
: d_handle(handle)
, d_type(type)
, d_context()
{
}

NTCCFG_INLINE
ReactorEvent::ReactorEvent(ntsa::Handle                  handle,
                           ntca::ReactorEventType::Value type,
                           const ntsa::Error&            error)
: d_handle(handle)
, d_type(type)
, d_context()
{
    d_context.setError(error);
}

NTCCFG_INLINE
ReactorEvent::ReactorEvent(ntsa::Handle                  handle,
                           ntca::ReactorEventType::Value type,
                           const ntca::ReactorContext&   context)
: d_handle(handle)
, d_type(type)
, d_context(context)
{
}

NTCCFG_INLINE
ReactorEvent::ReactorEvent(const ReactorEvent& original)
: d_handle(original.d_handle)
, d_type(original.d_type)
, d_context(original.d_context)
{
}

NTCCFG_INLINE
ReactorEvent::~ReactorEvent()
{
}

NTCCFG_INLINE
ReactorEvent& ReactorEvent::operator=(const ReactorEvent& other)
{
    if (this != &other) {
        d_handle  = other.d_handle;
        d_type    = other.d_type;
        d_context = other.d_context;
    }
    return *this;
}

NTCCFG_INLINE
void ReactorEvent::reset()
{
    d_handle = ntsa::k_INVALID_HANDLE;
    d_type   = ntca::ReactorEventType::e_NONE;
    d_context.reset();
}

NTCCFG_INLINE
void ReactorEvent::setHandle(ntsa::Handle handle)
{
    d_handle = handle;
}

NTCCFG_INLINE
void ReactorEvent::setType(ntca::ReactorEventType::Value type)
{
    d_type = type;
}

NTCCFG_INLINE
void ReactorEvent::setBytesReadable(bsl::size_t bytesReadable)
{
    d_context.setBytesReadable(bytesReadable);
}

NTCCFG_INLINE
void ReactorEvent::setBytesWritable(bsl::size_t bytesWritable)
{
    d_context.setBytesWritable(bytesWritable);
}

NTCCFG_INLINE
void ReactorEvent::setError(const ntsa::Error& error)
{
    d_context.setError(error);
}

NTCCFG_INLINE
void ReactorEvent::setContext(const ntca::ReactorContext& context)
{
    d_context = context;
}

NTCCFG_INLINE
ntsa::Handle ReactorEvent::handle() const
{
    return d_handle;
}

NTCCFG_INLINE
ntca::ReactorEventType::Value ReactorEvent::type() const
{
    return d_type;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& ReactorEvent::bytesReadable() const
{
    return d_context.bytesReadable();
}

NTCCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& ReactorEvent::bytesWritable() const
{
    return d_context.bytesWritable();
}

NTCCFG_INLINE
const ntsa::Error& ReactorEvent::error() const
{
    return d_context.error();
}

NTCCFG_INLINE
const ntca::ReactorContext& ReactorEvent::context() const
{
    return d_context;
}

NTCCFG_INLINE
bool ReactorEvent::isReadable() const
{
    return d_type == ntca::ReactorEventType::e_READABLE;
}

NTCCFG_INLINE
bool ReactorEvent::isWritable() const
{
    return d_type == ntca::ReactorEventType::e_WRITABLE;
}

NTCCFG_INLINE
bool ReactorEvent::isError() const
{
    return d_type == ntca::ReactorEventType::e_ERROR;
}

NTCCFG_INLINE
bool ReactorEvent::isReadableOrWritable(ntca::ReactorEventMask mask)
{
    return ((mask & (ntca::ReactorEventType::e_READABLE |
                     ntca::ReactorEventType::e_WRITABLE)) != 0);
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ReactorEvent& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ReactorEvent& lhs, const ReactorEvent& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const ReactorEvent& lhs, const ReactorEvent& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const ReactorEvent& lhs, const ReactorEvent& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReactorEvent& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.handle());
    hashAppend(algorithm, value.type());
    hashAppend(algorithm, value.context());
}

}  // end namespace ntca
}  // end namespace BloombergLP
#endif
