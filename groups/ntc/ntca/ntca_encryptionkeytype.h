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

#ifndef INCLUDED_NTCA_ENCRYPTIONKEYTYPE
#define INCLUDED_NTCA_ENCRYPTIONKEYTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_abstract.h>

namespace BloombergLP {
namespace ntca {

/// Enumerate the encryption key types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntci_encryption
struct EncryptionKeyType {
  public:
    /// Enumerate the encryption key types.
    enum Value {
        /// The key uses the RSA algorithm.
        e_DSA = 0,

        /// The key uses the RSA algorithm.
        e_RSA = 1,

        /// The key uses the NIST P-256 version 1 elliptic curve.
        e_NIST_P256 = 2,

        /// The key uses the NIST P-384 elliptic curve.
        e_NIST_P384 = 3,

        /// The key uses the NIST P-521 elliptic curve.
        e_NIST_P521 = 4,

        /// The key uses the "ed25519" elliptic curve.
        e_ED25519 = 5,

        /// The key uses the "ed448" elliptic curve.
        e_ED448 = 6
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the object identifier corresponding
    /// to the specified 'value'.
    static void toObjectIdentifier(ntsa::AbstractObjectIdentifier* result, 
                                   Value                           value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified 'result' the enumerator matching the specified
    /// object 'identifier'. Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'identifier' does not match any
    /// enumerator).
    static int fromObjectIdentifier(
        Value*                                result, 
        const ntsa::AbstractObjectIdentifier& identifier);

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
/// @related ntca::EncryptionKeyType
bsl::ostream& operator<<(bsl::ostream& stream, EncryptionKeyType::Value rhs);

}  // end namespace ntca
}  // end namespace BloombergLP
#endif
