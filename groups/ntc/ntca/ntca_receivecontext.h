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

#ifndef INCLUDED_NTCA_RECEIVECONTEXT
#define INCLUDED_NTCA_RECEIVECONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_handle.h>
#include <ntsa_transport.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe the context of a receive operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b transport:
/// The transport the receiver.
///
/// @li @b endpoint:
/// The remote endpoint from which the data was sent. This value might be null
/// for connected receivers.
///
/// @li @b foreignHandle:
/// The foreign handle sent by the peer, if any. If a foreign handle is 
/// defined, it is the receivers responsibility to close it.
///
/// @li @b error:
/// The error detected when performing the operation.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_receive
class ReceiveContext
{
    ntsa::Transport::Value              d_transport;
    bdlb::NullableValue<ntsa::Endpoint> d_endpoint;
    bdlb::NullableValue<ntsa::Handle>   d_foreignHandle;
    ntsa::Error                         d_error;

  public:
    /// Create a new receive context having the default value.
    ReceiveContext();

    /// Create a new receive context having the same value as the specified
    /// 'original' object.
    ReceiveContext(const ReceiveContext& original);

    /// Destroy this object.
    ~ReceiveContext();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ReceiveContext& operator=(const ReceiveContext& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the transport of the receiver to the specified 'value'.
    void setTransport(ntsa::Transport::Value value);

    /// Set the endpoint from which the data was sent to the specified
    /// 'endpoint'. This value should not be set for connected receivers.
    void setEndpoint(const ntsa::Endpoint& value);

    /// Set the foreign handle sent by the peer to the specified 'value'. 
    void setForeignHandle(ntsa::Handle value);

    /// Set the error detected when performing the operation to the
    /// specified 'value'.
    void setError(const ntsa::Error& value);

    /// Return the transport of the receiver.
    ntsa::Transport::Value transport() const;

    /// Return the transport mode of the receiver. This function is provided
    /// for convenience; the resulting value is interpreted from the
    /// transport of the receiver.
    ntsa::TransportMode::Value transportMode() const;

    /// Return the transport domain of the receiver. This function is
    /// provided for convenience; the resulting value is interpreted from
    /// the transport of the receiver.
    ntsa::TransportDomain::Value transportDomain() const;

    /// Return the transport protocol of the receiver. This function is
    /// provided for convenience; the resulting value is interpreted from
    /// the transport of the receiver.
    ntsa::TransportProtocol::Value transportProtocol() const;

    /// Return the endpoint from which the data was sent. This value might
    /// be null for connected receivers.
    const bdlb::NullableValue<ntsa::Endpoint>& endpoint() const;

    /// Return the foreign handle sent by the peer, if any.
    const bdlb::NullableValue<ntsa::Handle>& foreignHandle() const;

    /// Return the error detected when performing the operation.
    const ntsa::Error& error() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ReceiveContext& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ReceiveContext& other) const;

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
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(ReceiveContext);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::ReceiveContext
bsl::ostream& operator<<(bsl::ostream& stream, const ReceiveContext& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ReceiveContext
bool operator==(const ReceiveContext& lhs, const ReceiveContext& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ReceiveContext
bool operator!=(const ReceiveContext& lhs, const ReceiveContext& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ReceiveContext
bool operator<(const ReceiveContext& lhs, const ReceiveContext& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ReceiveContext
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReceiveContext& value);

NTCCFG_INLINE
ReceiveContext::ReceiveContext()
: d_transport(ntsa::Transport::e_UNDEFINED)
, d_endpoint()
, d_foreignHandle()
, d_error()
{
}

NTCCFG_INLINE
ReceiveContext::ReceiveContext(const ReceiveContext& original)
: d_transport(original.d_transport)
, d_endpoint(original.d_endpoint)
, d_foreignHandle(original.d_foreignHandle)
, d_error(original.d_error)
{
}

NTCCFG_INLINE
ReceiveContext::~ReceiveContext()
{
}

NTCCFG_INLINE
ReceiveContext& ReceiveContext::operator=(const ReceiveContext& other)
{
    d_transport     = other.d_transport;
    d_endpoint      = other.d_endpoint;
    d_foreignHandle = other.d_foreignHandle;
    d_error         = other.d_error;
    return *this;
}

NTCCFG_INLINE
void ReceiveContext::reset()
{
    d_transport = ntsa::Transport::e_UNDEFINED;
    d_endpoint.reset();
    d_foreignHandle.reset();
    d_error = ntsa::Error();
}

NTCCFG_INLINE
void ReceiveContext::setTransport(ntsa::Transport::Value value)
{
    d_transport = value;
}

NTCCFG_INLINE
void ReceiveContext::setEndpoint(const ntsa::Endpoint& value)
{
    d_endpoint = value;
}

NTSCFG_INLINE
void ReceiveContext::setForeignHandle(ntsa::Handle value)
{
    d_foreignHandle = value;
}

NTCCFG_INLINE
void ReceiveContext::setError(const ntsa::Error& value)
{
    d_error = value;
}

NTCCFG_INLINE
ntsa::Transport::Value ReceiveContext::transport() const
{
    return d_transport;
}

NTCCFG_INLINE
ntsa::TransportMode::Value ReceiveContext::transportMode() const
{
    return ntsa::Transport::getMode(d_transport);
}

NTCCFG_INLINE
ntsa::TransportDomain::Value ReceiveContext::transportDomain() const
{
    return ntsa::Transport::getDomain(d_transport);
}

NTCCFG_INLINE
ntsa::TransportProtocol::Value ReceiveContext::transportProtocol() const
{
    return ntsa::Transport::getProtocol(d_transport);
}

NTCCFG_INLINE
const bdlb::NullableValue<ntsa::Endpoint>& ReceiveContext::endpoint() const
{
    return d_endpoint;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Handle>& ReceiveContext::foreignHandle() const
{
    return d_foreignHandle;
}

NTCCFG_INLINE
const ntsa::Error& ReceiveContext::error() const
{
    return d_error;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ReceiveContext& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ReceiveContext& lhs, const ReceiveContext& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const ReceiveContext& lhs, const ReceiveContext& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const ReceiveContext& lhs, const ReceiveContext& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReceiveContext& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.transport());
    hashAppend(algorithm, value.endpoint());
    hashAppend(algorithm, value.foreignHandle());
    hashAppend(algorithm, value.error());
}

}  // close package namespace
}  // close enterprise namespace
#endif
