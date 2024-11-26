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

#ifndef INCLUDED_NTCA_ENCRYPTIONRESOURCEOPTIONS
#define INCLUDED_NTCA_ENCRYPTIONRESOURCEOPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_encryptionresourcetype.h>
#include <ntca_encryptionsecret.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_functional.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntca {

/// Describe the parameters to an encryption resource storage operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b type:
/// The type of resource storage format. If undefined, when the resource is
/// encoded it is encoded as one or more ASN.1 encodings of a private key
/// structure (e.g., DSA, RSA, or Elliptic Curve, depending on the type of
/// key), or certificate structure (X.509), then base-64-encoded and wrapped in
/// the Privacy Enhanced Mail (PEM) format; when the resource is decoded the
/// format of the resource is auto-detected.
///
/// @li @b label:
/// The label, or friendly name, attached to the resource.
///
/// @li @b secret:
/// The shared secret required to use the resource storage.
///
/// @li @b secretCallback
/// The function to invoke to resolve the shared secret required to use the
/// resource storage.
///
/// @li @b encrypted:
/// The resource is, or should be, symmetrically encrypted using the the shared
/// secret explicitly defined or resolved through the secret callback.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionResourceOptions
{
  public:
    /// Enumerate the hints of what the resource should contain.
    enum Hint {
        /// The resource should contain a private key.
        e_PRIVATE_KEY = 0,

        /// The resource should contain an end-user certificate.
        e_CERTIFICATE = 1,

        /// The resource should contain one or more trusted certificate
        /// authorities.
        e_CERTIFICATE_AUTHORITY = 2,

        /// The contents of the resource are unknown.
        e_ANY = 3
    };

    Hint                                                     d_hint;
    bdlb::NullableValue<ntca::EncryptionResourceType::Value> d_type;
    bdlb::NullableValue<bsl::string>                         d_label;
    bdlb::NullableValue<ntca::EncryptionSecret>              d_secret;
    bdlb::NullableValue<ntca::EncryptionSecretCallback>      d_secretCallback;
    bdlb::NullableValue<bool>                                d_encrypted;

  public:
    /// Create new encryption resource options having the default value.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit EncryptionResourceOptions(bslma::Allocator* basicAllocator = 0);

    /// Create new resource resource options having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    EncryptionResourceOptions(const EncryptionResourceOptions& original,
                              bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionResourceOptions();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionResourceOptions& operator=(
        const EncryptionResourceOptions& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the hint that aids in prioritizing the contents of the resource to
    /// the specified 'value'.
    void setHint(Hint value);

    /// Set the resource type to the specified 'value'.
    void setType(ntca::EncryptionResourceType::Value value);

    /// Set the label, or friendly name, to the specified 'value'.
    void setLabel(const bsl::string& value);

    /// Set the secret to the specified 'value'.
    void setSecret(const ntca::EncryptionSecret& value);

    /// Set the secret callback to the specified 'value'.
    void setSecretCallback(const ntca::EncryptionSecretCallback& value);

    /// Set the flag that indicates the resource is, or should be,
    /// symmetrically-encrypted to the specified 'value'.
    void setEncrypted(bool value);

    /// Return hint that aids in prioritizing the contents of the resource.
    Hint hint() const;

    /// Return the resource type.
    const bdlb::NullableValue<ntca::EncryptionResourceType::Value>& type()
        const;

    /// Return the label, or friendly name.
    const bdlb::NullableValue<bsl::string>& label() const;

    /// Return the secret.
    const bdlb::NullableValue<ntca::EncryptionSecret>& secret() const;

    /// Return the secret callback.
    const bdlb::NullableValue<ntca::EncryptionSecretCallback>& secretCallback()
        const;

    /// Return the flag that indicates the resource is, or should be,
    /// symmetrically-encrypted.
    const bdlb::NullableValue<bool>& encrypted() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionResourceOptions& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionResourceOptions& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(EncryptionResourceOptions);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionResourceOptions
bsl::ostream& operator<<(bsl::ostream&                    stream,
                         const EncryptionResourceOptions& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionResourceOptions
bool operator==(const EncryptionResourceOptions& lhs,
                const EncryptionResourceOptions& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionResourceOptions
bool operator!=(const EncryptionResourceOptions& lhs,
                const EncryptionResourceOptions& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionResourceOptions
bool operator<(const EncryptionResourceOptions& lhs,
               const EncryptionResourceOptions& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionResourceOptions
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                  algorithm,
                const EncryptionResourceOptions& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                  algorithm,
                const EncryptionResourceOptions& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.hint());
    hashAppend(algorithm, value.type());
    hashAppend(algorithm, value.label());
    hashAppend(algorithm, value.secret());
    hashAppend(algorithm, value.encrypted());
}

}  // close package namespace
}  // close enterprise namespace
#endif
