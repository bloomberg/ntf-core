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

#ifndef INCLUDED_NTCA_ENCRYPTIONRESOURCETYPE
#define INCLUDED_NTCA_ENCRYPTIONRESOURCETYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntca {

/// Enumerate the encryption resource storage types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntci_encryption
struct EncryptionResourceType {
  public:
    /// Enumerate the encryption resource storage types.
    enum Value {
        /// The resource is stored as a single ASN.1 encoding of a private key
        /// structure (e.g., DSA, RSA, or Elliptic Curve, depending on the type
        /// of key), or a certificate structure (X.509). Files of this type of
        /// encoding usually have a suffix of ".der". Note that this type can
        /// only store a single resource.
        e_ASN1,

        /// The resource is stored as one or more ASN.1 encodings of a private
        /// key structure (e.g., DSA, RSA, or Elliptic Curve, depending on the
        /// type of key), or certificate structure (X.509), then
        /// base-64-encoded and wrapped in the Privacy Enhanced Mail (PEM)
        /// format. Files of this type of encoding usually have the suffix
        /// ".pem". Note that this type can store multiple resources. If
        /// multiple resources are stored, the resources must be stored in the
        /// following order: first the private key, then the user certificate,
        /// then any trusted certificates.
        e_ASN1_PEM,

        /// The resource is stored as the ASN.1 encoding of a private key
        /// structure (e.g., DSA, RSA, or Elliptic Curve, depending on the type
        /// of key), stored within the ASN.1 encoding of a PKCS8 private key
        /// container structure. Files of this type of encoding usually have
        /// the suffix ".pkcs8" or ".p8". Note that this type can only store
        /// a single private key.
        e_PKCS8,

        /// The key is stored as the ASN.1 encoding of the private key
        /// structure (e.g., DSA, RSA, or Elliptic Curve, depending on the type
        /// of key), stored within the ASN.1 encoding of a PKCS8 private key
        /// container structure, then base-64-encoded and wrapped in the
        /// Privacy Enhanced Mail (PEM) format. Files of this type of encoding
        /// usually have the suffix ".pem". Note that this type can only store
        /// a single private key.
        e_PKCS8_PEM,

        /// The key is stored as the ASN.1 encoding of the private key
        /// structure (e.g., DSA, RSA, or Elliptic Curve, depending on the type
        /// of key), stored within the ASN.1 encoding of a PKCS12 (PFX)
        /// multi-purpose container structure. Files of this type of encoding
        /// usually have the suffix ".pkcs12", ".p12", or ".pfx". Note that
        /// this type can store multiple resources. If multiple resources are
        /// stored, only a single key and user certificate may be stored, but
        /// any number of trusted certificates are allowed.
        e_PKCS12
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
/// @related ntca::EncryptionResourceType
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         EncryptionResourceType::Value rhs);

}  // end namespace ntca
}  // end namespace BloombergLP
#endif
