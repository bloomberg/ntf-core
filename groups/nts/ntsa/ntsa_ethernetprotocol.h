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

#ifndef INCLUDED_NTSA_ETHERNETPROTOCOL
#define INCLUDED_NTSA_ETHERNETPROTOCOL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an enumeration of the protocols carried by an ethernet frame.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_identity
struct EthernetProtocol {
  public:
    /// Provide an enumeration of the protocols carried by an ethernet frame.
    enum Value {
        /// The protocol carried by the ethernet frame is undefined.
        e_UNDEFINED = 0,

        /// The protocol carried by the ethernet frame is the Internet
        /// Protocol, version 4 (IPv4).
        e_IPV4 = 0x0800,

        /// The protocol carried by the ethernet frame is the Address
        /// Resolution Protocol (ARP).
        e_ARP = 0x0806,

        /// The protocol carried by the ethernet frame is the Reverse Address
        /// Resolution Protocol (RARP).
        e_RARP = 0x8035,

        /// The protocol carried by the ethernet frame is the Internet
        /// Protocol, version 6 (IPv6).
        e_IPV6 = 0x86DD
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
/// @related ntsa::EthernetProtocol
bsl::ostream& operator<<(bsl::ostream& stream, EthernetProtocol::Value rhs);

}  // close package namespace
}  // close enterprise namespace
#endif
