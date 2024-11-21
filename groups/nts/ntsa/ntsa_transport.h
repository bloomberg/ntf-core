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

}  // end namespace ntsa

BDLAT_DECL_ENUMERATION_TRAITS(ntsa::Transport)

}  // end namespace BloombergLP
#endif
