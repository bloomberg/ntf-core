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

#ifndef INCLUDED_NTCA_GETENDPOINTCONTEXT
#define INCLUDED_NTCA_GETENDPOINTCONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_resolversource.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_ipaddress.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsls_timeinterval.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntca {

/// Describe the context of an operation to get an endpoint from a domain
/// name or IP address and a service name or port.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b authority:
/// The domain name and port requested to be resolved.
///
/// @li @b endpointList:
/// The list of endpoints to which the authority resolves.
///
/// @li @b latency:
/// The length of time to perform the resolution.
///
/// @li @b source:
/// The source of the resolution.
///
/// @li @b nameServer:
/// The endpoint of the name server that successfully responded to the request,
/// if any.
///
/// @li @b timeToLive:
/// The relative duration the results of the operation should be cached, in
/// seconds, if known.
///
/// @li @b error:
/// The error detected when performing the operation.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_resolve
class GetEndpointContext
{
    bsl::string                         d_authority;
    bsl::vector<ntsa::Endpoint>         d_endpointList;
    bsls::TimeInterval                  d_latency;
    ntca::ResolverSource::Value         d_source;
    bdlb::NullableValue<ntsa::Endpoint> d_nameServer;
    bdlb::NullableValue<bsl::size_t>    d_timeToLive;
    ntsa::Error                         d_error;

  public:
    /// Create a new get IP address context having the default value.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit GetEndpointContext(bslma::Allocator* basicAllocator = 0);

    /// Create a new get IP address context having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    GetEndpointContext(const GetEndpointContext& original,
                       bslma::Allocator*         basicAllocator = 0);

    /// Destroy this object.
    ~GetEndpointContext();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    GetEndpointContext& operator=(const GetEndpointContext& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the domain name and port requested to be resolved to the
    /// specified 'value'.
    void setAuthority(const bsl::string& value);

    /// Set the list of endpoints to which the authority resolves to the
    /// specified 'value'.
    void setEndpointList(const bsl::vector<ntsa::Endpoint>& value);

    /// Add the specified 'value' to the list of endpoints to which the
    /// authority resolves.
    void addEndpoint(const ntsa::Endpoint& value);

    /// Set the length of time to perform the resolution to the specified
    /// 'value'.
    void setLatency(const bsls::TimeInterval& value);

    /// Set the source of the resolution to the specified 'value'.
    void setSource(ntca::ResolverSource::Value value);

    /// Set the endpoint of the name server that successfully responded to
    /// the request to the specified 'value'.
    void setNameServer(const ntsa::Endpoint& value);

    /// Set the time-to-live for the results on the operation to the
    /// specified 'value'.
    void setTimeToLive(bsl::size_t value);

    /// Set the error detected when performing the operation to the
    /// specified 'value'.
    void setError(const ntsa::Error& value);

    /// Return the domain name and port requested to be resolved.
    const bsl::string& authority() const;

    /// Return the endpoint list.
    const bsl::vector<ntsa::Endpoint>& endpointList() const;

    /// Return the length of time to perform the resolution.
    const bsls::TimeInterval& latency() const;

    /// Return the source of the resolution.
    ntca::ResolverSource::Value source() const;

    /// Return the endpoint of the name server that successfully responded
    /// to the request.
    const bdlb::NullableValue<ntsa::Endpoint>& nameServer() const;

    /// Return the time-to-live for the results on the operation.
    const bdlb::NullableValue<bsl::size_t>& timeToLive() const;

    /// Return the error detected when performing the operation.
    const ntsa::Error& error() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const GetEndpointContext& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const GetEndpointContext& other) const;

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

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(GetEndpointContext);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::GetEndpointContext
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const GetEndpointContext& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::GetEndpointContext
bool operator==(const GetEndpointContext& lhs, const GetEndpointContext& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::GetEndpointContext
bool operator!=(const GetEndpointContext& lhs, const GetEndpointContext& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::GetEndpointContext
bool operator<(const GetEndpointContext& lhs, const GetEndpointContext& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::GetEndpointContext
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const GetEndpointContext& value);

NTCCFG_INLINE
GetEndpointContext::GetEndpointContext(bslma::Allocator* basicAllocator)
: d_authority(basicAllocator)
, d_endpointList(basicAllocator)
, d_latency()
, d_source(ntca::ResolverSource::e_UNKNOWN)
, d_nameServer()
, d_timeToLive()
, d_error()
{
}

NTCCFG_INLINE
GetEndpointContext::GetEndpointContext(const GetEndpointContext& original,
                                       bslma::Allocator* basicAllocator)
: d_authority(original.d_authority, basicAllocator)
, d_endpointList(original.d_endpointList, basicAllocator)
, d_latency(original.d_latency)
, d_source(original.d_source)
, d_nameServer(original.d_nameServer)
, d_timeToLive(original.d_timeToLive)
, d_error(original.d_error)
{
}

NTCCFG_INLINE
GetEndpointContext::~GetEndpointContext()
{
}

NTCCFG_INLINE
GetEndpointContext& GetEndpointContext::operator=(
    const GetEndpointContext& other)
{
    if (this != &other) {
        d_authority    = other.d_authority;
        d_endpointList = other.d_endpointList;
        d_latency      = other.d_latency;
        d_source       = other.d_source;
        d_nameServer   = other.d_nameServer;
        d_timeToLive   = other.d_timeToLive;
        d_error        = other.d_error;
    }
    return *this;
}

NTCCFG_INLINE
void GetEndpointContext::reset()
{
    d_authority.clear();
    d_endpointList.clear();
    d_latency = bsls::TimeInterval();
    d_source  = ntca::ResolverSource::e_UNKNOWN;
    d_nameServer.reset();
    d_timeToLive.reset();
    d_error = ntsa::Error();
}

NTCCFG_INLINE
void GetEndpointContext::setAuthority(const bsl::string& value)
{
    d_authority = value;
}

NTCCFG_INLINE
void GetEndpointContext::setEndpointList(
    const bsl::vector<ntsa::Endpoint>& value)
{
    d_endpointList = value;
}

NTCCFG_INLINE
void GetEndpointContext::addEndpoint(const ntsa::Endpoint& value)
{
    d_endpointList.push_back(value);
}

NTCCFG_INLINE
void GetEndpointContext::setLatency(const bsls::TimeInterval& value)
{
    d_latency = value;
}

NTCCFG_INLINE
void GetEndpointContext::setSource(ntca::ResolverSource::Value value)
{
    d_source = value;
}

NTCCFG_INLINE
void GetEndpointContext::setNameServer(const ntsa::Endpoint& value)
{
    d_nameServer = value;
}

NTCCFG_INLINE
void GetEndpointContext::setTimeToLive(bsl::size_t value)
{
    d_timeToLive = value;
}

NTCCFG_INLINE
void GetEndpointContext::setError(const ntsa::Error& value)
{
    d_error = value;
}

NTCCFG_INLINE
const bsl::string& GetEndpointContext::authority() const
{
    return d_authority;
}

NTCCFG_INLINE
const bsl::vector<ntsa::Endpoint>& GetEndpointContext::endpointList() const
{
    return d_endpointList;
}

NTCCFG_INLINE
const bsls::TimeInterval& GetEndpointContext::latency() const
{
    return d_latency;
}

NTCCFG_INLINE
ntca::ResolverSource::Value GetEndpointContext::source() const
{
    return d_source;
}

NTCCFG_INLINE
const bdlb::NullableValue<ntsa::Endpoint>& GetEndpointContext::nameServer()
    const
{
    return d_nameServer;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& GetEndpointContext::timeToLive() const
{
    return d_timeToLive;
}

NTCCFG_INLINE
const ntsa::Error& GetEndpointContext::error() const
{
    return d_error;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const GetEndpointContext& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const GetEndpointContext& lhs, const GetEndpointContext& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const GetEndpointContext& lhs, const GetEndpointContext& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const GetEndpointContext& lhs, const GetEndpointContext& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const GetEndpointContext& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.authority());
    hashAppend(algorithm, value.endpointList());
    hashAppend(algorithm, value.latency());
    hashAppend(algorithm, value.source());
    hashAppend(algorithm, value.nameServer());
    hashAppend(algorithm, value.timeToLive());
    hashAppend(algorithm, value.error());
}

}  // close package namespace
}  // close enterprise namespace
#endif
