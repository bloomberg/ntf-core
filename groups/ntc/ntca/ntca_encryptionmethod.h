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

#ifndef INCLUDED_NTCA_ENCRYPTIONMETHOD
#define INCLUDED_NTCA_ENCRYPTIONMETHOD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntca {

/// Enumerate the methods of encryption.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntci_encryption
struct EncryptionMethod {
  public:
    /// Enumerate the methods of encryption.
    enum Value {
        /// When specified as a minimum version, the minimum version is
        /// interpreted as the minimum version suggested by the current
        /// standards of cryptography. When specified as a maximum version, the
        /// maximum version is interpreted as the maximum version supported by
        /// the implementation.
        e_DEFAULT,

        /// A TLS/SSL connection established with these methods will only
        /// understand the TLSv1 protocol.
        e_TLS_V1_0,

        /// A TLS/SSL connection established with these methods will only
        /// understand the TLSv1.1 protocol.
        e_TLS_V1_1,

        /// A TLS/SSL connection established with these methods will only
        /// understand the TLSv1.2 protocol.
        e_TLS_V1_2,

        /// A TLS/SSL connection established with these methods will only
        /// understand the TLSv1.3 protocol.
        e_TLS_V1_3,

        /// A TLS/SSL connection established with these methods may understand
        /// the TLSv1, TLSv1.1 and TLSv1.2 protocols.
        e_TLS_V1_X
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
/// @related ntca::EncryptionMethod
bsl::ostream& operator<<(bsl::ostream& stream, EncryptionMethod::Value rhs);

}  // close package namespace
}  // close enterprise namespace
#endif
