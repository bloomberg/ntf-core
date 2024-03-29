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

#ifndef INCLUDED_NTCA_ENCRYPTIONCERTIFICATESTORAGETYPE
#define INCLUDED_NTCA_ENCRYPTIONCERTIFICATESTORAGETYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntca {

/// Enumerate the encryption certificate storage types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntci_encryption
struct EncryptionCertificateStorageType {
  public:
    /// Enumerate the encryption certificate storage types.
    enum Value {
        /// The certificate is stored as the ASN.1 encoding of an X.509
        /// certificate structure. Files of this type of encoding usually have
        /// a suffix of ".der".
        e_X509,

        /// The certificate is stored as the ASN.1 encoding of an X.509
        /// certificate structure, then base-64-encoded and wrapped in the
        /// Privacy Enhanced Mail (PEM) format. Files of this type of encoding
        /// usually have the suffix ".pem".
        e_X509_PEM,

        /// The certificate is stored as the ASN.1 encoding of an X.509
        /// certificate structure, stored within the ASN.1 encoding of a PKCS12
        /// (PFX) multi-purpose container structure. Files of this type of
        /// encoding usually have the suffix ".pkcs12", ".p12", or ".pfx".
        e_X509_PKCS12
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
/// @related ntca::EncryptionCertificateStorageType
bsl::ostream& operator<<(bsl::ostream&                           stream,
                         EncryptionCertificateStorageType::Value rhs);

}  // end namespace ntca
}  // end namespace BloombergLP
#endif
