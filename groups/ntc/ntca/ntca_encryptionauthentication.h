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

#ifndef INCLUDED_NTCA_ENCRYPTIONAUTHENTICATION
#define INCLUDED_NTCA_ENCRYPTIONAUTHENTICATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntca {

/// Enumerate the authentication modes.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntci_encryption
struct EncryptionAuthentication {
  public:
    /// Enumerate the authentication modes.
    enum Value {
        /// If the socket context is operating in server mode, the server will
        /// not request a certificate from the client.  If the socket is
        /// operating in client mode, the handshake will fail if there is no
        /// trusted certificate authority through which to verify the server's
        /// identity.
        e_DEFAULT,

        /// If the socket context is operating in server mode, the server
        /// will not request a certificate from the client.  If the socket
        /// context is operating in client mode, the handshake will succeed
        /// regardless of the certificate used by the server.
        e_NONE,

        /// If the socket context is operating in server mode, the server
        /// will request a certificate from the client, and the handshake
        /// will fail if either the client does not return a certificate or
        /// if there is no trusted certificate authority through which to
        /// verify the client's identity. If the socket is operating in
        /// client mode, the handshake will fail if there is no trusted
        /// certificate authority through which to verify the server's
        /// identity.
        e_VERIFY
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
/// @related ntca::EncryptionAuthentication
bsl::ostream& operator<<(bsl::ostream&                   stream,
                         EncryptionAuthentication::Value rhs);

}  // close package namespace
}  // close enterprise namespace
#endif
