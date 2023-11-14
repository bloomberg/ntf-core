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

#ifndef INCLUDED_NTSA_SENDOPTIONS
#define INCLUDED_NTSA_SENDOPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_endpoint.h>
#include <ntsa_handle.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a description of the options to a send operation.
///
/// @details
/// Provide a value-semantic type that describes the parameters to
/// a send operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b endpoint:
/// The remote endpoint to which the data should be send. If this value is
/// null, the data is sent to the remote endpoint to which the socket is
/// connected. The default value is null.
///
/// @li @b foreignHandle:
/// The handle to the open socket to send to the peer. If successfully received
/// the handle is effectively duplicated in the receiving process, but note
/// that the sender is still responsible for closing the socket handle even if
/// it has been sent successfully.
///
/// @li @b maxBytes:
/// The hint for the maximum number of bytes to copy to the socket send buffer.
/// This value does not stricly imply the maximum number of bytes to copy to
/// the socket send buffer; this value is only used, potentially, for internal
/// optimizations when defining native scatter/gather buffer arrays on-the-fly.
/// Setting this value to the maximum size of the socket send buffer may help
/// the implemention avoid attempting to copy more bytes than the maximum
/// number of bytes that can effectively be copied, improving the efficiency of
/// the copy operation. Note that setting this value to a number less than the
/// size of the socket send buffer may risk silently truncating a message when
/// copying to the send buffer of a datagram socket. If this value is zero, no
/// limit is applied. It is recommended to either set this value to zero or to
/// the maximum size of the socket send buffer. Note that this value is
/// currently only honored when sending blobs.
///
/// @li @b maxBuffers:
/// The hint for the maximum number of buffers to copy to the socket send
/// buffer. This value does not stricly imply the maximum number of buffers to
/// copy to the socket send buffer; this value is only used, potentially, for
/// internal optimizations when defining native scatter/gather buffer arrays
/// on-the-fly. Setting this value less than the system limit may help the
/// efficiency of the implementation allocating scatter/gather buffer
/// structures when performing vectored I/O. Note that setting this value to a
/// number less than the system limit may risk silently truncating a message
/// when copying to the send buffer of a datagram socket. If this value is
/// zero, the system limit is applied. It is recommended to set this value to
/// zero, or, for stream sockets only, to NTSCFG_DEFAULT_MAX_INPLACE_BUFFERS.
/// Note that this value is currently only honored when sending blobs.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_operation
class SendOptions
{
    bdlb::NullableValue<ntsa::Endpoint> d_endpoint;
    bdlb::NullableValue<ntsa::Handle>   d_foreignHandle;
    bsl::size_t                         d_maxBytes;
    bsl::size_t                         d_maxBuffers;

  public:
    /// Create new send options having the default value.
    SendOptions();

    /// Create new send options having the same value as the specified
    /// 'original' object.
    SendOptions(const SendOptions& original);

    /// Destroy this object.
    ~SendOptions();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    SendOptions& operator=(const SendOptions& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the remote endpoint to which the data should be sent to the
    /// specified 'value'.
    void setEndpoint(const ntsa::Endpoint& value);

    // Set the handle to the open socket to send to the peer to the specified
    // 'value'.
    void setForeignHandle(ntsa::Handle value);

    /// Set the maximum number of bytes to copy to the specified 'value'.
    void setMaxBytes(bsl::size_t value);

    /// Set the maximum number of buffers to copy to the specified 'value'.
    void setMaxBuffers(bsl::size_t value);

    /// Return the remote endpoint to which the data should be sent.
    const bdlb::NullableValue<ntsa::Endpoint>& endpoint() const;

    /// Return the handle to the open socket to send to the peer.
    const bdlb::NullableValue<ntsa::Handle>& foreignHandle() const;

    /// Return the maximum number of bytes to copy.
    bsl::size_t maxBytes() const;

    /// Return the maximum number of buffers to copy.
    bsl::size_t maxBuffers() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const SendOptions& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const SendOptions& other) const;

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
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(SendOptions);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::SendOptions
bsl::ostream& operator<<(bsl::ostream& stream, const SendOptions& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::SendOptions
bool operator==(const SendOptions& lhs, const SendOptions& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::SendOptions
bool operator!=(const SendOptions& lhs, const SendOptions& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::SendOptions
bool operator<(const SendOptions& lhs, const SendOptions& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::SendOptions
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const SendOptions& value);

NTSCFG_INLINE
SendOptions::SendOptions()
: d_endpoint()
, d_foreignHandle()
, d_maxBytes(0)
, d_maxBuffers(0)
{
}

NTSCFG_INLINE
SendOptions::SendOptions(const SendOptions& original)
: d_endpoint(original.d_endpoint)
, d_foreignHandle(original.d_foreignHandle)
, d_maxBytes(original.d_maxBytes)
, d_maxBuffers(original.d_maxBuffers)
{
}

NTSCFG_INLINE
SendOptions::~SendOptions()
{
}

NTSCFG_INLINE
SendOptions& SendOptions::operator=(const SendOptions& other)
{
    d_endpoint   = other.d_endpoint;
    d_foreignHandle = other.d_foreignHandle;
    d_maxBytes   = other.d_maxBytes;
    d_maxBuffers = other.d_maxBuffers;
    return *this;
}

NTSCFG_INLINE
void SendOptions::reset()
{
    d_endpoint.reset();
    d_foreignHandle.reset();
    d_maxBytes   = 0;
    d_maxBuffers = 0;
}

NTSCFG_INLINE
void SendOptions::setEndpoint(const ntsa::Endpoint& value)
{
    d_endpoint = value;
}

NTSCFG_INLINE
void SendOptions::setForeignHandle(ntsa::Handle value)
{
    d_foreignHandle = value;
}

NTSCFG_INLINE
void SendOptions::setMaxBytes(bsl::size_t value)
{
    d_maxBytes = value;
}

NTSCFG_INLINE
void SendOptions::setMaxBuffers(bsl::size_t value)
{
    d_maxBuffers = value;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Endpoint>& SendOptions::endpoint() const
{
    return d_endpoint;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Handle>& SendOptions::foreignHandle() const
{
    return d_foreignHandle;
}

NTSCFG_INLINE
bsl::size_t SendOptions::maxBytes() const
{
    return d_maxBytes;
}

NTSCFG_INLINE
bsl::size_t SendOptions::maxBuffers() const
{
    return d_maxBuffers;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const SendOptions& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const SendOptions& lhs, const SendOptions& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const SendOptions& lhs, const SendOptions& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const SendOptions& lhs, const SendOptions& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const SendOptions& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.endpoint());
    hashAppend(algorithm, value.foreignHandle());
    hashAppend(algorithm, value.maxBytes());
    hashAppend(algorithm, value.maxBuffers());
}

}  // close package namespace
}  // close enterprise namespace
#endif
