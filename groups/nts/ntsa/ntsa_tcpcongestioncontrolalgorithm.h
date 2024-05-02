// Copyright 2024 Bloomberg Finance L.P.
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

#ifndef INCLUDED_NTSA_TCPCONGESTIONCONTROLALGORITHM
#define INCLUDED_NTSA_TCPCONGESTIONCONTROLALGORITHM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an enumeration of the TCP congestion control algorithms.
///
/// @par Thread Safety
/// This struct is thread safe.
///
struct TcpCongestionControlAlgorithm {
  public:
    /// Provide an enumeration of the TCP congestion control algorithms.
    enum Value {
        e_RENO = 0,
        e_CUBIC,
        e_BBR,
        e_BIC,
        e_DCTCP,
        e_DIAG,
        e_HIGHSPEED,
        e_HTCP,
        e_HYBLA,
        e_ILLINOIS,
        e_LP,
        e_NV,
        e_SCALABLE,
        e_VEGAS,
        e_VENO,
        e_WESTWOOD,
        e_YEAH
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

// FREE OPERATORS

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntsa::TcpCongestionControlAlgorithm
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         TcpCongestionControlAlgorithm::Value rhs);

}  // close package namespace
}  // close enterprise namespace
#endif
