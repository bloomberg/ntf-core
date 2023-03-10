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

#ifndef INCLUDED_NTCA_CONNECTCONTEXT
#define INCLUDED_NTCA_CONNECTCONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_resolversource.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsls_timeinterval.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe the context of a connect operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b endpoint:
/// The endpoint to which the socket was connected.
///
/// @li @b name:
/// The domain name and port requested to be resolved.
///
/// @li @b latency:
/// The length of time to perform the operation.
///
/// @li @b source:
/// The source of the resolution.
///
/// @li @b nameServer:
/// The endpoint of the name server that resolved the domain name, if any.
///
/// @li @b attemptsRemaining:
/// The number of connection retry attempts remaining.
///
/// @li @b error:
/// The error detected during the operation.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_connect
class ConnectContext
{
    ntsa::Endpoint                                   d_endpoint;
    bdlb::NullableValue<bsl::string>                 d_name;
    bdlb::NullableValue<bsls::TimeInterval>          d_latency;
    bdlb::NullableValue<ntca::ResolverSource::Value> d_source;
    bdlb::NullableValue<ntsa::Endpoint>              d_nameServer;
    bsl::size_t                                      d_attemptsRemaining;
    ntsa::Error                                      d_error;

  public:
    /// Create a new connect context having the default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit ConnectContext(bslma::Allocator* basicAllocator = 0);

    /// Create a new connect context having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    ConnectContext(const ConnectContext& original,
                   bslma::Allocator*     basicAllocator = 0);

    /// Destroy this object.
    ~ConnectContext();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ConnectContext& operator=(const ConnectContext& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the endpoint to which the socket was connected to the specified
    /// 'value'.
    void setEndpoint(const ntsa::Endpoint& value);

    /// Set the domain name and port requested to be resolved to the
    /// specified 'value'.
    void setName(const bsl::string& value);

    /// Set the length of time to perform the operation to the specified
    /// 'value'.
    void setLatency(const bsls::TimeInterval& value);

    /// Set the source of the resolution to the specified 'value'.
    void setSource(ntca::ResolverSource::Value value);

    /// Set the endpoint of the name server that resolved the domain name to
    /// the specified 'value'.
    void setNameServer(const ntsa::Endpoint& value);

    /// Set the number of connection retry attempts remaining to the
    /// specified 'value'.
    void setAttemptsRemaining(bsl::size_t value);

    /// Set the error detected for the connection attempt at the time of
    /// the event specified 'value'.
    void setError(const ntsa::Error& value);

    /// Return the endpoint to which the socket was connected.
    const ntsa::Endpoint& endpoint() const;

    /// Return the domain name and port requested to be resolved.
    const bdlb::NullableValue<bsl::string>& name() const;

    /// Return the length of time to perform the operation.
    const bdlb::NullableValue<bsls::TimeInterval>& latency() const;

    /// Return the source of the resolution.
    const bdlb::NullableValue<ntca::ResolverSource::Value>& source() const;

    /// Return the endpoint of the name server that resolved the domain
    /// name.
    const bdlb::NullableValue<ntsa::Endpoint>& nameServer() const;

    /// Return the number of connection retry attempts remaining.
    bsl::size_t attemptsRemaining() const;

    /// Return the error detected for the connection attempt at the time of
    /// the event.
    const ntsa::Error& error() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ConnectContext& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ConnectContext& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(ConnectContext);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::ConnectContext
bsl::ostream& operator<<(bsl::ostream& stream, const ConnectContext& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ConnectContext
bool operator==(const ConnectContext& lhs, const ConnectContext& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ConnectContext
bool operator!=(const ConnectContext& lhs, const ConnectContext& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ConnectContext
bool operator<(const ConnectContext& lhs, const ConnectContext& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ConnectContext
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ConnectContext& value);

NTCCFG_INLINE
ConnectContext::ConnectContext(bslma::Allocator* basicAllocator)
: d_endpoint()
, d_name(basicAllocator)
, d_latency()
, d_source()
, d_nameServer()
, d_attemptsRemaining(0)
, d_error()
{
}

NTCCFG_INLINE
ConnectContext::ConnectContext(const ConnectContext& original,
                               bslma::Allocator*     basicAllocator)
: d_endpoint(original.d_endpoint)
, d_name(original.d_name, basicAllocator)
, d_latency(original.d_latency)
, d_source(original.d_source)
, d_nameServer(original.d_nameServer)
, d_attemptsRemaining(original.d_attemptsRemaining)
, d_error(original.d_error)
{
}

NTCCFG_INLINE
ConnectContext::~ConnectContext()
{
}

NTCCFG_INLINE
ConnectContext& ConnectContext::operator=(const ConnectContext& other)
{
    if (this != &other) {
        d_endpoint          = other.d_endpoint;
        d_name              = other.d_name;
        d_latency           = other.d_latency;
        d_source            = other.d_source;
        d_nameServer        = other.d_nameServer;
        d_attemptsRemaining = other.d_attemptsRemaining;
        d_error             = other.d_error;
    }
    return *this;
}

NTCCFG_INLINE
void ConnectContext::reset()
{
    d_endpoint.reset();
    d_name.reset();
    d_latency.reset();
    d_source.reset();
    d_nameServer.reset();
    d_attemptsRemaining = 0;
    d_error             = ntsa::Error();
}

NTCCFG_INLINE
void ConnectContext::setEndpoint(const ntsa::Endpoint& value)
{
    d_endpoint = value;
}

NTCCFG_INLINE
void ConnectContext::setName(const bsl::string& value)
{
    d_name = value;
}

NTCCFG_INLINE
void ConnectContext::setLatency(const bsls::TimeInterval& value)
{
    d_latency = value;
}

NTCCFG_INLINE
void ConnectContext::setSource(ntca::ResolverSource::Value value)
{
    d_source = value;
}

NTCCFG_INLINE
void ConnectContext::setNameServer(const ntsa::Endpoint& value)
{
    d_nameServer = value;
}

NTCCFG_INLINE
void ConnectContext::setAttemptsRemaining(bsl::size_t value)
{
    d_attemptsRemaining = value;
}

NTCCFG_INLINE
void ConnectContext::setError(const ntsa::Error& value)
{
    d_error = value;
}

NTCCFG_INLINE
const ntsa::Endpoint& ConnectContext::endpoint() const
{
    return d_endpoint;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsl::string>& ConnectContext::name() const
{
    return d_name;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsls::TimeInterval>& ConnectContext::latency() const
{
    return d_latency;
}

NTCCFG_INLINE
const bdlb::NullableValue<ntca::ResolverSource::Value>& ConnectContext::
    source() const
{
    return d_source;
}

NTCCFG_INLINE
const bdlb::NullableValue<ntsa::Endpoint>& ConnectContext::nameServer() const
{
    return d_nameServer;
}

NTCCFG_INLINE
bsl::size_t ConnectContext::attemptsRemaining() const
{
    return d_attemptsRemaining;
}

NTCCFG_INLINE
const ntsa::Error& ConnectContext::error() const
{
    return d_error;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ConnectContext& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ConnectContext& lhs, const ConnectContext& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const ConnectContext& lhs, const ConnectContext& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const ConnectContext& lhs, const ConnectContext& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ConnectContext& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.endpoint());
    hashAppend(algorithm, value.name());
    hashAppend(algorithm, value.latency());
    hashAppend(algorithm, value.source());
    hashAppend(algorithm, value.nameServer());
    hashAppend(algorithm, value.attemptsRemaining());
    hashAppend(algorithm, value.error());
}

}  // close package namespace
}  // close enterprise namespace
#endif
