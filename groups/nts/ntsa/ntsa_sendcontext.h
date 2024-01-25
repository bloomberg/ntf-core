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

#ifndef INCLUDED_NTSA_SENDCONTEXT
#define INCLUDED_NTSA_SENDCONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a description of the result of a send operation.
///
/// @details
/// Provide a value-semantic type that describes the result of a
/// send operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b bytesSendable:
/// The number of bytes attempted to copy to the socket send buffer.
///
/// @li @b bytesSent:
/// The actual number of bytes copied to the socket send buffer.
///
/// @li @b buffersSendable:
/// The number of buffers attempted to copy to the socket send buffer.
///
/// @li @b buffersSent:
/// The actual number of buffers copied to the socket send buffer.
///
/// @li @b messagesSendable:
/// The number of messages attempted to copy to the socket send buffer. This
/// value is only relevant when copying to the send buffer of a datagram
/// socket.
///
/// @li @b messagesSent:
/// The actual number of messages copied to the socket send buffer. This value
/// is only relevant when copying to the send buffer of a datagram socket.
///
/// @li @b zeroCopy:
/// The flag that indicates the data was referenced in-place rather than copied
/// to the send buffer. If this flag is true, the application must ensure the
/// data-to-send is neither overwritten nor invalidated (i.e. freed) until the
/// completion of the send operation is indicated in a subsequent notification
/// (which also indicates whether the data was referenced in-place or copied.)
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_operation
class SendContext
{
    bsl::size_t d_bytesSendable;
    bsl::size_t d_bytesSent;
    bsl::size_t d_buffersSendable;
    bsl::size_t d_buffersSent;
    bsl::size_t d_messagesSendable;
    bsl::size_t d_messagesSent;
    bool        d_zeroCopy;

  public:
    /// Create new send options having the default value.
    SendContext();

    /// Create new send options having the same value as the specified
    /// 'original' object.
    SendContext(const SendContext& original);

    /// Destroy this object.
    ~SendContext();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    SendContext& operator=(const SendContext& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the number of bytes attempted to be sent to the specified
    /// 'value'.
    void setBytesSendable(bsl::size_t value);

    /// Set the number of bytes actually sent to the specified 'value'.
    void setBytesSent(bsl::size_t value);

    /// Set the number of buffers attempted to be sent to the specified
    /// 'value'.
    void setBuffersSendable(bsl::size_t value);

    /// Set the number of buffers actually sent to the specified 'value'.
    void setBuffersSent(bsl::size_t value);

    /// Set the number of messages attempted to be sent to the specified
    /// 'value'.
    void setMessagesSendable(bsl::size_t value);

    /// Set the number of messages actually sent to the specified 'value'.
    void setMessagesSent(bsl::size_t value);

    /// Set the flag that indicates the data was referenced in-place rather
    /// than copied to the send buffer to the specified 'value'.
    void setZeroCopy(bool value);

    /// Return the number of bytes attempted to be sent.
    bsl::size_t bytesSendable() const;

    /// Return the number of bytes actually sent.
    bsl::size_t bytesSent() const;

    /// Return the number of buffers attempted to be sent.
    bsl::size_t buffersSendable() const;

    /// Return the number of buffers actually sent.
    bsl::size_t buffersSent() const;

    /// Return the number of messages attempted to be sent.
    bsl::size_t messagesSendable() const;

    /// Return the number of messages actually sent.
    bsl::size_t messagesSent() const;

    /// Return the flag that indicates the data was referenced in-place rather
    /// than copied to the send buffer.
    bool zeroCopy() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const SendContext& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const SendContext& other) const;

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
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(SendContext);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::SendContext
bsl::ostream& operator<<(bsl::ostream& stream, const SendContext& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::SendContext
bool operator==(const SendContext& lhs, const SendContext& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::SendContext
bool operator!=(const SendContext& lhs, const SendContext& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::SendContext
bool operator<(const SendContext& lhs, const SendContext& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::SendContext
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const SendContext& value);

NTSCFG_INLINE
SendContext::SendContext()
: d_bytesSendable(0)
, d_bytesSent(0)
, d_buffersSendable(0)
, d_buffersSent(0)
, d_messagesSendable(0)
, d_messagesSent(0)
, d_zeroCopy(false)
{
}

NTSCFG_INLINE
SendContext::SendContext(const SendContext& original)
: d_bytesSendable(original.d_bytesSendable)
, d_bytesSent(original.d_bytesSent)
, d_buffersSendable(original.d_buffersSendable)
, d_buffersSent(original.d_buffersSent)
, d_messagesSendable(original.d_messagesSendable)
, d_messagesSent(original.d_messagesSent)
, d_zeroCopy(original.d_zeroCopy)
{
}

NTSCFG_INLINE
SendContext::~SendContext()
{
}

NTSCFG_INLINE
SendContext& SendContext::operator=(const SendContext& other)
{
    d_bytesSendable    = other.d_bytesSendable;
    d_bytesSent        = other.d_bytesSent;
    d_buffersSendable  = other.d_buffersSendable;
    d_buffersSent      = other.d_buffersSent;
    d_messagesSendable = other.d_messagesSendable;
    d_messagesSent     = other.d_messagesSent;
    d_zeroCopy         = other.d_zeroCopy;

    return *this;
}

NTSCFG_INLINE
void SendContext::reset()
{
    d_bytesSendable    = 0;
    d_bytesSent        = 0;
    d_buffersSendable  = 0;
    d_buffersSent      = 0;
    d_messagesSendable = 0;
    d_messagesSent     = 0;
    d_zeroCopy         = false;
}

NTSCFG_INLINE
void SendContext::setBytesSendable(bsl::size_t value)
{
    d_bytesSendable = value;
}

NTSCFG_INLINE
void SendContext::setBytesSent(bsl::size_t value)
{
    d_bytesSent = value;
}

NTSCFG_INLINE
void SendContext::setBuffersSendable(bsl::size_t value)
{
    d_buffersSendable = value;
}

NTSCFG_INLINE
void SendContext::setBuffersSent(bsl::size_t value)
{
    d_buffersSent = value;
}

NTSCFG_INLINE
void SendContext::setMessagesSendable(bsl::size_t value)
{
    d_messagesSendable = value;
}

NTSCFG_INLINE
void SendContext::setMessagesSent(bsl::size_t value)
{
    d_messagesSent = value;
}

NTSCFG_INLINE
void SendContext::setZeroCopy(bool value)
{
    d_zeroCopy = value;
}

NTSCFG_INLINE
bsl::size_t SendContext::bytesSendable() const
{
    return d_bytesSendable;
}

NTSCFG_INLINE
bsl::size_t SendContext::bytesSent() const
{
    return d_bytesSent;
}

NTSCFG_INLINE
bsl::size_t SendContext::buffersSendable() const
{
    return d_buffersSendable;
}

NTSCFG_INLINE
bsl::size_t SendContext::buffersSent() const
{
    return d_buffersSent;
}

NTSCFG_INLINE
bsl::size_t SendContext::messagesSendable() const
{
    return d_messagesSendable;
}

NTSCFG_INLINE
bsl::size_t SendContext::messagesSent() const
{
    return d_messagesSent;
}

NTSCFG_INLINE
bool SendContext::zeroCopy() const
{
    return d_zeroCopy;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const SendContext& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const SendContext& lhs, const SendContext& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const SendContext& lhs, const SendContext& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const SendContext& lhs, const SendContext& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const SendContext& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.bytesSendable());
    hashAppend(algorithm, value.bytesSent());
    hashAppend(algorithm, value.buffersSendable());
    hashAppend(algorithm, value.buffersSent());
    hashAppend(algorithm, value.messagesSendable());
    hashAppend(algorithm, value.messagesSent());
    hashAppend(algorithm, value.zeroCopy());
}

}  // close package namespace
}  // close enterprise namespace
#endif
