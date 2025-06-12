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

#ifndef INCLUDED_NTSA_TRANSPORT
#define INCLUDED_NTSA_TRANSPORT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_ipaddresstype.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlat_enumeratorinfo.h>
#include <bdlat_enumfunctions.h>
#include <bdlat_typetraits.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an enumeration of the socket transport security protocols.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_system
struct TransportSecurity {
  public:
    /// Provide an enumeration of the socket transport security protocols.
    enum Value {
        /// The socket transport security protocol is undefined.
        e_UNDEFINED = 0,

        /// The socket uses the Transport Layer Security protocol.
        e_TLS = 1,

        /// The socket uses the Secure Shell protocol.
        e_SSH = 2
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntsa::TransportSecurity
bsl::ostream& operator<<(bsl::ostream& stream, TransportSecurity::Value rhs);

/// Provide an enumeration of the socket transport protocols.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_system
struct TransportProtocol {
  public:
    /// Provide an enumeration of the socket transport protocols.
    enum Value {
        /// The socket transport protocol is undefined.
        e_UNDEFINED = 0,

        /// The socket uses the Transmission Control Protocol.
        e_TCP = 1,

        /// The socket uses the User Datagram Protocol.
        e_UDP = 2,

        /// The socket uses local (aka Unix) domain protocol.
        e_LOCAL = 3
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntsa::TransportProtocol
bsl::ostream& operator<<(bsl::ostream& stream, TransportProtocol::Value rhs);

/// Provide an enumeration of the socket address families.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_system
struct TransportDomain {
  public:
    /// Provide an enumeration of the socket address families.
    enum Value {
        /// The socket transport domain is undefined.
        e_UNDEFINED = 0,

        /// The socket uses Internet Protocol version 4 addressing.
        e_IPV4 = 1,

        /// The socket uses Internet Protocol version 6 addressing.
        e_IPV6 = 2,

        /// The socket uses local (aka Unix) addressing.
        e_LOCAL = 3
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntsa::TransportDomain
bsl::ostream& operator<<(bsl::ostream& stream, TransportDomain::Value rhs);

/// Provide an enumeration of the socket transport modes.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_system
struct TransportMode {
  public:
    /// Provide an enumeration of the socket transport modes.
    enum Value {
        /// The socket transport mode is undefined.
        e_UNDEFINED = 0,

        /// The socket uses reliable, stream semantics.
        e_STREAM = 1,

        /// The socket uses unreliable, datagram semantics.
        e_DATAGRAM = 2
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntsa::TransportMode
bsl::ostream& operator<<(bsl::ostream& stream, TransportMode::Value rhs);

/// Provide an enumeration of the socket transport roles.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_system
struct TransportRole {
  public:
    /// Provide an enumeration of the socket transport roles.
    enum Value {
        /// The socket transport role is undefined.
        e_UNDEFINED = 0,

        /// The socket is used to actively initiate the establishment the 
        /// transport.
        e_CLIENT = 1,

        /// The socket is used to passively wait for a peer to initiate the
        /// establishment of the transport.
        e_SERVER = 2
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntsa::TransportRole
bsl::ostream& operator<<(bsl::ostream& stream, TransportRole::Value rhs);

/// Provide an enumeration of the socket transport types.
///
/// @details
/// A socket type is defined by its address family, transport protocol, and
/// communication mode.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_system
struct Transport {
  public:
    /// Provide an enumeration of the socket transport types.
    enum Value {
        /// The socket transport is undefined.
        e_UNDEFINED = 0,

        /// The socket uses Internet Protocol, version 4 addressing using
        /// the Transmission Control Protocol using reliable, stream
        /// semantics.
        e_TCP_IPV4_STREAM = 1,

        /// The socket uses Internet Protocol, version 6 addressing using
        /// the Transmission Control Protocol using reliable, stream
        /// sementics.
        e_TCP_IPV6_STREAM = 2,

        /// The socket uses Internet Protocol, version 4 addressing using
        /// the User Datagram Protocol using unreliable, datagram semantics.
        e_UDP_IPV4_DATAGRAM = 3,

        /// The socket uses Internet Protocol, version 6 addressing using
        /// the User Datagram Protocol using unreliable, datagram semantics.
        e_UDP_IPV6_DATAGRAM = 4,

        /// The socket uses local (aka Unix) addressing and transfers data
        /// using reliable, stream semantics.
        e_LOCAL_STREAM = 5,

        /// The socket uses local (aka Unix) addressing and transfers data
        /// using reliable, datagram semantics.
        e_LOCAL_DATAGRAM = 6
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified `result` the enumerator matching the
    /// specified `string` of the specified `stringLength`.  Return 0 on
    /// success, and a non-zero value with no effect on `result` otherwise
    /// (i.e., `string` does not match any enumerator).
    static int fromString(Value* result, const char* string, int stringLength);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Return the transport mode of the specified 'value'.
    static ntsa::TransportMode::Value getMode(Value value);

    /// Return the transport domain of the specified 'value'.
    static ntsa::TransportDomain::Value getDomain(Value value);

    /// Return the transport protocol of the specified 'value'.
    static ntsa::TransportProtocol::Value getProtocol(Value value);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntsa::Transport
bsl::ostream& operator<<(bsl::ostream& stream, Transport::Value rhs);

/// Describe a transport suite.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b application:
/// The application protocol (e.g. "http", or "dns", or some custom proprietary 
/// protocol).
///
/// @li @b transportSecurity:
/// The transport security protocol (e.g., TLS or SSH).
///
/// @li @b transportProtocol:
/// The transport security protocol (e.g., TCP or UDP or the intrinsic 
/// transport of the local domain).
///
/// @li @b transportDomain:
/// The transport domain (e.g., IPv4 or IPv6 or the local domain).
///
/// @li @b transportMode:
/// The transport mode (e.g., stream or datagram).
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class TransportSuite
{
    bsl::string                    d_application;
    ntsa::TransportSecurity::Value d_transportSecurity;
    ntsa::TransportProtocol::Value d_transportProtocol;
    ntsa::TransportDomain::Value   d_transportDomain;
    ntsa::TransportMode::Value     d_transportMode;

  public:
    /// Create a new transport suite having the default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit TransportSuite(bslma::Allocator* basicAllocator = 0);

    /// Create a new transport suite having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    TransportSuite(const TransportSuite& original,
                    bslma::Allocator*      basicAllocator = 0);

    /// Destroy this object.
    ~TransportSuite();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    TransportSuite& operator=(const TransportSuite& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set application protocol name to the specified 'value'.
    void setApplication(const bslstl::StringRef& value);

    /// Set the transport security to the specified 'value'.
    void setTransportSecurity(ntsa::TransportSecurity::Value value);

    /// Set the transport protocol to the specified 'value'.
    void setTransportProtocol(ntsa::TransportProtocol::Value value);

    /// Set the transport domain to the specified 'value'.
    void setTransportDomain(ntsa::TransportDomain::Value value);

    /// Set the transport mode to the specified 'value'.
    void setTransportMode(ntsa::TransportMode::Value value);

    /// Return the application protocol name.
    const bsl::string& application() const;

    /// Return the transport security.
    ntsa::TransportSecurity::Value transportSecurity() const;

    /// Return the transport protocol.
    ntsa::TransportProtocol::Value transportProtocol() const;

    /// Return the transport domain.
    ntsa::TransportDomain::Value transportDomain() const;

    /// Return the transport mode.
    ntsa::TransportMode::Value transportMode() const;

    /// Return the allocator used to supply memory.
    bslma::Allocator* allocator() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const TransportSuite& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const TransportSuite& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(TransportSuite);
};

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
///
/// @related ntsa::TransportSuite
bsl::ostream& operator<<(bsl::ostream& stream, const TransportSuite& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::TransportSuite
bool operator==(const TransportSuite& lhs, const TransportSuite& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::TransportSuite
bool operator!=(const TransportSuite& lhs, const TransportSuite& rhs);

/// Return true if the value of the specified 'lhs' is less than the value of
/// the specified 'rhs', otherwise return false.
///
/// @related ntsa::TransportSuite
bool operator<(const TransportSuite& lhs, const TransportSuite& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::TransportSuite
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TransportSuite& value);

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void TransportSuite::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_application);
    hashAppend(algorithm, d_transportSecurity);
    hashAppend(algorithm, d_transportProtocol);
    hashAppend(algorithm, d_transportDomain);
    hashAppend(algorithm, d_transportMode);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&       algorithm,
                              const TransportSuite& value)
{
    value.hash(algorithm);
}

}  // end namespace ntsa

BDLAT_DECL_ENUMERATION_TRAITS(ntsa::Transport)

}  // end namespace BloombergLP
#endif
