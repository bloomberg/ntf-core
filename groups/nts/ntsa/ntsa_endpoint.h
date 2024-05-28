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

#ifndef INCLUDED_NTSA_ENDPOINT
#define INCLUDED_NTSA_ENDPOINT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_endpointtype.h>
#include <ntsa_ipendpoint.h>
#include <ntsa_ipv4endpoint.h>
#include <ntsa_ipv6endpoint.h>
#include <ntsa_localname.h>
#include <ntsa_transport.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a union of address family-specific endpoints.
///
/// @details
/// Provide a value-semantic type that represents a discriminated
/// union of endpoint types specific to the Internet Protocol (IP) and local
/// (a.k.a. Unix) address families.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par Usage Example: Represent an IPv4 address and port number
/// The following example illustrates how to construct a 'ntsa::Endpoint'
/// to represent an IPv4 address and port number.
///
///     ntsa::Endpoint endpoint("127.0.0.1:12345");
///
///     NTSCFG_TEST_TRUE(endpoint.isIp());
///     NTSCFG_TEST_TRUE(endpoint.ip().host().isV4());
///     NTSCFG_TEST_EQ(endpoint.ip().host().v4(),
///                    ntsa::Ipv4Address::loopback());
///     NTSCFG_TEST_EQ(endpoint.ip().port(), 12345);
///
///
/// @par Usage Example: Represent an IPv4 address and port number
/// The following example illustrates how to construct a 'ntsa::Endpoint'
/// to represent an IPv6 address and port number.
///
///     ntsa::Endpoint endpoint("[::1]:12345");
///
///     NTSCFG_TEST_TRUE(endpoint.isIp());
///     NTSCFG_TEST_TRUE(endpoint.ip().host().isV6());
///     NTSCFG_TEST_EQ(endpoint.ip().host().v6(),
///                    ntsa::Ipv6Address::loopback());
///     NTSCFG_TEST_EQ(endpoint.ip().port(), 12345);
///
///
/// @par Usage Example: Represent a local (a.k.a Unix) name
/// The following example illustrates how to construct a 'ntsa::Endpoint'
/// to represent an IPv4 address and port number.
///
///     ntsa::Endpoint endpoint("/tmp/server");
///
///     NTSCFG_TEST_TRUE(endpoint.isLocal());
///     NTSCFG_TEST_TRUE(endpoint.local().value() ==
///                      bsl::string("/tmp/server"));
///
/// @ingroup module_ntsa_identity
class Endpoint
{
    union {
        bsls::ObjectBuffer<ntsa::IpEndpoint> d_ip;
        bsls::ObjectBuffer<ntsa::LocalName>  d_local;
    };

    ntsa::EndpointType::Value d_type;

  public:
    /// Create a new endpoint having an undefined type.
    Endpoint();

    /// Create a new endpoint parsed from the specified 'text'.
    explicit Endpoint(const bslstl::StringRef& text);

    /// Create a new endpoint having an "ip" representation having the
    /// specified 'value'.
    explicit Endpoint(const ntsa::IpEndpoint& value);

    /// Create a new endpoint having an "ip" representation having the
    /// specified 'value'.
    explicit Endpoint(const ntsa::Ipv4Endpoint& value);

    /// Create a new endpoint having an "ip" representation having the
    /// specified 'value'.
    explicit Endpoint(const ntsa::Ipv6Endpoint& value);

    /// Create a new endpoint having a "local" representation having the
    /// specified 'value'.
    explicit Endpoint(const ntsa::LocalName& value);

    /// Create a new endpoint having an "ip" representation having the
    /// value of the specified 'address' and 'port'.
    Endpoint(const ntsa::IpAddress& address, ntsa::Port port);

    /// Create a new endpoint having an "ip" representation having the
    /// value of the specified 'address' and 'port'.
    Endpoint(const ntsa::Ipv4Address& address, ntsa::Port port);

    /// Create a new endpoint having an "ip" representation having the
    /// value of the specified 'address' and 'port'.
    Endpoint(const ntsa::Ipv6Address& address, ntsa::Port port);

    /// Create a new address having the same value as the specified 'other'
    /// object.
    Endpoint(const Endpoint& other);

    /// Destroy this object.
    ~Endpoint();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Endpoint& operator=(const Endpoint& other);

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Endpoint& operator=(const ntsa::IpEndpoint& other);

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Endpoint& operator=(const ntsa::LocalName& other);

    /// Set the value of the object from the specified 'text'.
    Endpoint& operator=(const bslstl::StringRef& text);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the value of this object from the value parsed from any of its
    /// textual representations. Return true if the 'text' is in a valid
    /// format and was parsed successfully, otherwise return false.
    bool parse(const bslstl::StringRef& text);

    /// Select the "ip" address representation. Return a reference to the
    /// modifiable representation.
    ntsa::IpEndpoint& makeIp();

    /// Select the "ip" address representation initially having the
    /// specified 'value'. Return a reference to the modifiable
    /// representation.
    ntsa::IpEndpoint& makeIp(const ntsa::IpEndpoint& value);

    /// Select the "local name" address representation. Return a reference
    /// to the modifiable representation.
    ntsa::LocalName& makeLocal();

    /// Select the "local name" address representation initially having the
    /// specified 'value'. Return a reference to the modifiable
    /// representation.
    ntsa::LocalName& makeLocal(const ntsa::LocalName& value);

    /// Return a reference to the modifiable "ip" address representation.
    /// The behavior is undefined unless 'isIp()' is true.
    ntsa::IpEndpoint& ip();

    /// Return a reference to the modifiable "local name" address
    /// representation. The behavior is undefined unless 'isLocal()'
    /// is true.
    ntsa::LocalName& local();

    /// Return the textual representation of this object.
    bsl::string text() const;

    /// Return a reference to the non-modifiable "ip" address
    /// representation.  The behavior is undefined unless 'isIp()' is true.
    const ntsa::IpEndpoint& ip() const;

    /// Return a reference to the non-modifiable "local name" address
    /// representation. The behavior is undefined unless 'isLocal()'
    /// is true.
    const ntsa::LocalName& local() const;

    /// Return the transport used by this endpoint in the specified 'mode'.
    ntsa::Transport::Value transport(ntsa::TransportMode::Value mode) const;

    /// Return the type of the address representation.
    ntsa::EndpointType::Value type() const;

    /// Return true if the address representation is undefined, otherwise
    /// return false.
    bool isUndefined() const;

    /// Return true if the "ip" address representation is currently
    /// selected, otherwise return false.
    bool isIp() const;

    /// Return true if the "domain name" address representation is currently
    /// selected, otherwise return false.
    bool isLocal() const;

    /// Return true if the the endpoint *either* represents an IP address is
    /// *not* the wildcard "any" address (i.e., '0.0.0.0:0' for IPv4
    /// addresses, '[:0]:0' for IPv6 addresses), *or* if the endpoint
    /// represents a local name that is *not* unnamed. Otherwise, return
    /// false. Note that this function returns the negation of
    /// 'isImplict()'.
    bool isExplicit() const;

    /// Return true if the endpoint *either* represents an IP address whose
    /// host is the wildcard "any" address (i.e. '0.0.0.0:0' for IPv4
    /// addresses, '[:0]:0' for IPv6 addresses), *or* if the endpoint
    /// represents a local name that is unnamed, *or* if the endpoint is
    /// undefined. Otherwise, return false. Note that this function returns
    /// the negation of 'isExplicit()'.
    bool isImplicit() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Endpoint& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Endpoint& other) const;

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
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(Endpoint);
};

/// Write the specified 'object' to the specified 'stream'. Return a
/// modifiable reference to the 'stream'.
///
/// @related ntsa::Endpoint
bsl::ostream& operator<<(bsl::ostream& stream, const Endpoint& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Endpoint
bool operator==(const Endpoint& lhs, const Endpoint& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Endpoint
bool operator!=(const Endpoint& lhs, const Endpoint& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Endpoint
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Endpoint& value);

NTSCFG_INLINE
Endpoint::Endpoint()
: d_type(ntsa::EndpointType::e_UNDEFINED)
{
}

NTSCFG_INLINE
Endpoint::Endpoint(const ntsa::IpEndpoint& value)
: d_type(ntsa::EndpointType::e_IP)
{
    new (d_ip.buffer()) ntsa::IpEndpoint(value);
}

NTSCFG_INLINE
Endpoint::Endpoint(const ntsa::Ipv4Endpoint& value)
: d_type(ntsa::EndpointType::e_IP)
{
    new (d_ip.buffer()) ntsa::IpEndpoint(value.host(), value.port());
}

NTSCFG_INLINE
Endpoint::Endpoint(const ntsa::Ipv6Endpoint& value)
: d_type(ntsa::EndpointType::e_IP)
{
    new (d_ip.buffer()) ntsa::IpEndpoint(value.host(), value.port());
}

NTSCFG_INLINE
Endpoint::Endpoint(const ntsa::LocalName& value)
: d_type(ntsa::EndpointType::e_LOCAL)
{
    new (d_local.buffer()) ntsa::LocalName(value);
}

NTSCFG_INLINE
Endpoint::Endpoint(const ntsa::IpAddress& address, ntsa::Port port)
: d_type(ntsa::EndpointType::e_IP)
{
    new (d_ip.buffer()) ntsa::IpEndpoint(address, port);
}

NTSCFG_INLINE
Endpoint::Endpoint(const ntsa::Ipv4Address& address, ntsa::Port port)
: d_type(ntsa::EndpointType::e_IP)
{
    new (d_ip.buffer()) ntsa::IpEndpoint(address, port);
}

NTSCFG_INLINE
Endpoint::Endpoint(const ntsa::Ipv6Address& address, ntsa::Port port)
: d_type(ntsa::EndpointType::e_IP)
{
    new (d_ip.buffer()) ntsa::IpEndpoint(address, port);
}

NTSCFG_INLINE
Endpoint::Endpoint(const Endpoint& other)
: d_type(other.d_type)
{
    switch (d_type) {
    case ntsa::EndpointType::e_IP:
        new (d_ip.buffer()) ntsa::IpEndpoint(other.d_ip.object());
        break;
    case ntsa::EndpointType::e_LOCAL:
        new (d_local.buffer()) ntsa::LocalName(other.d_local.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::EndpointType::e_UNDEFINED);
    }
}

NTSCFG_INLINE
Endpoint::~Endpoint()
{
}

NTSCFG_INLINE
Endpoint& Endpoint::operator=(const Endpoint& other)
{
    if (this == &other) {
        return *this;
    }

    switch (other.d_type) {
    case ntsa::EndpointType::e_IP:
        this->makeIp(other.d_ip.object());
        break;
    case ntsa::EndpointType::e_LOCAL:
        this->makeLocal(other.d_local.object());
        break;
    default:
        BSLS_ASSERT(other.d_type == ntsa::EndpointType::e_UNDEFINED);
        this->reset();
    }

    return *this;
}

NTSCFG_INLINE
Endpoint& Endpoint::operator=(const ntsa::IpEndpoint& other)
{
    this->makeIp(other);
    return *this;
}

NTSCFG_INLINE
Endpoint& Endpoint::operator=(const ntsa::LocalName& other)
{
    this->makeLocal(other);
    return *this;
}

NTSCFG_INLINE
void Endpoint::reset()
{
    d_type = ntsa::EndpointType::e_UNDEFINED;
}

NTSCFG_INLINE
ntsa::IpEndpoint& Endpoint::makeIp()
{
    if (d_type == ntsa::EndpointType::e_IP) {
        d_ip.object().reset();
    }
    else {
        this->reset();
        new (d_ip.buffer()) ntsa::IpEndpoint();
        d_type = ntsa::EndpointType::e_IP;
    }

    return d_ip.object();
}

NTSCFG_INLINE
ntsa::IpEndpoint& Endpoint::makeIp(const ntsa::IpEndpoint& value)
{
    if (d_type == ntsa::EndpointType::e_IP) {
        d_ip.object() = value;
    }
    else {
        this->reset();
        new (d_ip.buffer()) ntsa::IpEndpoint(value);
        d_type = ntsa::EndpointType::e_IP;
    }

    return d_ip.object();
}

NTSCFG_INLINE
ntsa::LocalName& Endpoint::makeLocal()
{
    if (d_type == ntsa::EndpointType::e_LOCAL) {
        d_local.object().reset();
    }
    else {
        this->reset();
        new (d_local.buffer()) ntsa::LocalName();
        d_type = ntsa::EndpointType::e_LOCAL;
    }

    return d_local.object();
}

NTSCFG_INLINE
ntsa::LocalName& Endpoint::makeLocal(const ntsa::LocalName& value)
{
    if (d_type == ntsa::EndpointType::e_LOCAL) {
        d_local.object() = value;
    }
    else {
        this->reset();
        new (d_local.buffer()) ntsa::LocalName(value);
        d_type = ntsa::EndpointType::e_LOCAL;
    }

    return d_local.object();
}

NTSCFG_INLINE
ntsa::IpEndpoint& Endpoint::ip()
{
    BSLS_ASSERT(d_type == ntsa::EndpointType::e_IP);
    return d_ip.object();
}

NTSCFG_INLINE
ntsa::LocalName& Endpoint::local()
{
    BSLS_ASSERT(d_type == ntsa::EndpointType::e_LOCAL);
    return d_local.object();
}

NTSCFG_INLINE
const ntsa::IpEndpoint& Endpoint::ip() const
{
    BSLS_ASSERT(d_type == ntsa::EndpointType::e_IP);
    return d_ip.object();
}

NTSCFG_INLINE
const ntsa::LocalName& Endpoint::local() const
{
    BSLS_ASSERT(d_type == ntsa::EndpointType::e_LOCAL);
    return d_local.object();
}

NTSCFG_INLINE
ntsa::EndpointType::Value Endpoint::type() const
{
    return d_type;
}

NTSCFG_INLINE
bool Endpoint::isUndefined() const
{
    return (d_type == ntsa::EndpointType::e_UNDEFINED);
}

NTSCFG_INLINE
bool Endpoint::isIp() const
{
    return (d_type == ntsa::EndpointType::e_IP);
}

NTSCFG_INLINE
bool Endpoint::isLocal() const
{
    return (d_type == ntsa::EndpointType::e_LOCAL);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Endpoint& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const Endpoint& lhs, const Endpoint& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const Endpoint& lhs, const Endpoint& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const Endpoint& lhs, const Endpoint& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Endpoint& value)
{
    using bslh::hashAppend;

    if (value.isIp()) {
        hashAppend(algorithm, value.ip());
    }
    else if (value.isLocal()) {
        hashAppend(algorithm, value.local());
    }
}

}  // close package namespace
}  // close enterprise namespace
#endif
