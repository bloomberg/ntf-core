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

#ifndef INCLUDED_NTCA_SENDOPTIONS
#define INCLUDED_NTCA_SENDOPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_sendtoken.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_endpoint.h>
#include <ntsa_handle.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsls_timeinterval.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe the parameters to a send operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b token:
/// The token used to cancel the operation.
///
/// @li @b endpoint:
/// The remote endpoint to which the data should be sent. This value might not
/// be set for connected senders.
///
/// @li @b foreignHandle:
/// The handle to the open socket to send to the peer. If successfully received
/// the handle is effectively duplicated in the receiving process, but note
/// that the sender is still responsible for closing the socket handle even if
/// it has been sent successfully.
///
/// @li @b priority:
/// The priority of the write on the write queue.
///
/// @li @b highWatermark:
/// The effective write queue high watermark to use when performing the write.
///
/// @li @b deadline:
/// The deadline within which the message must be sent, in absolute time since
/// the Unix epoch.
///
/// @li @b recurse:
/// Allow callbacks to be invoked immediately and recursively if their
/// constraints are already satisified at the time the asynchronous operation
/// is initiated.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_send
class SendOptions
{
    bdlb::NullableValue<ntca::SendToken>    d_token;
    bdlb::NullableValue<ntsa::Endpoint>     d_endpoint;
    bdlb::NullableValue<ntsa::Handle>       d_foreignHandle;
    bdlb::NullableValue<bsl::size_t>        d_priority;
    bdlb::NullableValue<bsl::size_t>        d_highWatermark;
    bdlb::NullableValue<bsls::TimeInterval> d_deadline;
    bool                                    d_recurse;

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

    /// Set the token used to cancel the operation to the specified 'value'.
    void setToken(const ntca::SendToken& value);

    /// Set the remote endpoint to which the data should be sent. This value
    /// should not be set for connected senders.
    void setEndpoint(const ntsa::Endpoint& endpoint);

    // Set the handle to the open socket to send to the peer to the specified
    // 'value'.
    void setForeignHandle(ntsa::Handle value);

    /// Set the priority of the write on the write queue to the specified
    /// 'value'.
    void setPriority(bsl::size_t value);

    /// Set the effective write queue high watermark to the specified
    /// 'value'.
    void setHighWatermark(bsl::size_t value);

    /// Set the deadline within which the data must be sent to the
    /// specified 'value'.
    void setDeadline(const bsls::TimeInterval& value);

    /// Set the flag that allows callbacks to be invoked immediately and
    /// recursively if their constraints are already satisified at the time
    /// the asynchronous operation is initiated.
    void setRecurse(bool value);

    /// Return the token used to cancel the operation.
    const bdlb::NullableValue<ntca::SendToken>& token() const;

    /// Return the remote endpoint to which the data should be sent. This
    /// value might not be set for connected senders.
    const bdlb::NullableValue<ntsa::Endpoint>& endpoint() const;

    /// Return the handle to the open socket to send to the peer.
    const bdlb::NullableValue<ntsa::Handle>& foreignHandle() const;

    /// Return the priority of the write on the write queue.
    const bdlb::NullableValue<bsl::size_t>& priority() const;

    /// Return the effective write queue high watermark.
    const bdlb::NullableValue<bsl::size_t>& highWatermark() const;

    /// Return the deadline within which the data must be sent.
    const bdlb::NullableValue<bsls::TimeInterval>& deadline() const;

    /// Return true if callbacks are allowed to be invoked immediately and
    /// recursively if their constraints are already satisified at the time
    /// the asynchronous operation is initiated, otherwise return false.
    bool recurse() const;

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
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(SendOptions);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::SendOptions
bsl::ostream& operator<<(bsl::ostream& stream, const SendOptions& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::SendOptions
bool operator==(const SendOptions& lhs, const SendOptions& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::SendOptions
bool operator!=(const SendOptions& lhs, const SendOptions& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::SendOptions
bool operator<(const SendOptions& lhs, const SendOptions& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::SendOptions
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const SendOptions& value);

NTCCFG_INLINE
SendOptions::SendOptions()
: d_token()
, d_endpoint()
, d_foreignHandle()
, d_priority()
, d_highWatermark()
, d_deadline()
, d_recurse(false)
{
}

NTCCFG_INLINE
SendOptions::SendOptions(const SendOptions& original)
: d_token(original.d_token)
, d_endpoint(original.d_endpoint)
, d_foreignHandle(original.d_foreignHandle)
, d_priority(original.d_priority)
, d_highWatermark(original.d_highWatermark)
, d_deadline(original.d_deadline)
, d_recurse(original.d_recurse)
{
}

NTCCFG_INLINE
SendOptions::~SendOptions()
{
}

NTCCFG_INLINE
SendOptions& SendOptions::operator=(const SendOptions& other)
{
    d_token         = other.d_token;
    d_endpoint      = other.d_endpoint;
    d_foreignHandle = other.d_foreignHandle;
    d_priority      = other.d_priority;
    d_highWatermark = other.d_highWatermark;
    d_deadline      = other.d_deadline;
    d_recurse       = other.d_recurse;
    return *this;
}

NTCCFG_INLINE
void SendOptions::reset()
{
    d_token.reset();
    d_endpoint.reset();
    d_foreignHandle.reset();
    d_priority.reset();
    d_highWatermark.reset();
    d_deadline.reset();
    d_recurse = false;
}

NTCCFG_INLINE
void SendOptions::setToken(const ntca::SendToken& value)
{
    d_token = value;
}

NTCCFG_INLINE
void SendOptions::setEndpoint(const ntsa::Endpoint& value)
{
    d_endpoint = value;
}

NTSCFG_INLINE
void SendOptions::setForeignHandle(ntsa::Handle value)
{
    d_foreignHandle = value;
}

NTCCFG_INLINE
void SendOptions::setPriority(bsl::size_t value)
{
    d_priority = value;
}

NTCCFG_INLINE
void SendOptions::setHighWatermark(bsl::size_t value)
{
    d_highWatermark = value;
}

NTCCFG_INLINE
void SendOptions::setDeadline(const bsls::TimeInterval& value)
{
    d_deadline = value;
}

NTCCFG_INLINE
void SendOptions::setRecurse(bool value)
{
    d_recurse = value;
}

NTCCFG_INLINE
const bdlb::NullableValue<ntca::SendToken>& SendOptions::token() const
{
    return d_token;
}

NTCCFG_INLINE
const bdlb::NullableValue<ntsa::Endpoint>& SendOptions::endpoint() const
{
    return d_endpoint;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Handle>& SendOptions::foreignHandle() const
{
    return d_foreignHandle;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& SendOptions::priority() const
{
    return d_priority;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& SendOptions::highWatermark() const
{
    return d_highWatermark;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsls::TimeInterval>& SendOptions::deadline() const
{
    return d_deadline;
}

NTCCFG_INLINE
bool SendOptions::recurse() const
{
    return d_recurse;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const SendOptions& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const SendOptions& lhs, const SendOptions& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const SendOptions& lhs, const SendOptions& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const SendOptions& lhs, const SendOptions& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const SendOptions& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.token());
    hashAppend(algorithm, value.endpoint());
    hashAppend(algorithm, value.foreignHandle());
    hashAppend(algorithm, value.priority());
    hashAppend(algorithm, value.highWatermark());
    hashAppend(algorithm, value.deadline());
    hashAppend(algorithm, value.recurse());
}

}  // close package namespace
}  // close enterprise namespace
#endif
