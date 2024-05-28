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

#ifndef INCLUDED_NTCA_ENCRYPTIONRESOURCEDESCRIPTOR
#define INCLUDED_NTCA_ENCRYPTIONRESOURCEDESCRIPTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_encryptioncertificate.h>
#include <ntca_encryptionkey.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntca {

/// Describe the contents of the storage of an encryption resource.
///
/// @details
/// Provide a value-semantic type that represents a discriminated union of
/// either a path to a file on a filesystem or an encoding of literal data.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b path:
/// The path to the stored encryption resource.
///
/// @li @b literal:
/// The literal data of the stored encryption resource.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionResourceDescriptor
{
    typedef ntca::EncryptionCertificate CertificateType;
    typedef ntca::EncryptionKey         KeyType;
    typedef bsl::string                 PathType;
    typedef bsl::vector<char>           DataType;

    enum Selector { e_UNDEFINED, e_CERTIFICATE, e_KEY, e_PATH, e_DATA };

    union {
        bsls::ObjectBuffer<CertificateType> d_certificate;
        bsls::ObjectBuffer<KeyType>         d_key;
        bsls::ObjectBuffer<PathType>        d_path;
        bsls::ObjectBuffer<DataType>        d_data;
    };

    Selector          d_type;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new encryption resource descriptor having the default value.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit EncryptionResourceDescriptor(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new encryption resource descriptor having the same value as
    /// the specified 'original' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    EncryptionResourceDescriptor(const EncryptionResourceDescriptor& original,
                                 bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionResourceDescriptor();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    EncryptionResourceDescriptor& operator=(
        const EncryptionResourceDescriptor& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Select the "certificate" representation. Return a reference to the
    /// modifiable representation.
    ntca::EncryptionCertificate& makeCertificate();

    /// Select the "certificate" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntca::EncryptionCertificate& makeCertificate(
        const ntca::EncryptionCertificate& value);

    /// Select the "key" representation. Return a reference to the modifiable
    /// representation.
    ntca::EncryptionKey& makeKey();

    /// Select the "key" representation initially having the specified 'value'.
    /// Return a reference to the modifiable representation.
    ntca::EncryptionKey& makeKey(const ntca::EncryptionKey& value);

    /// Select the "path" representation. Return a reference to the modifiable
    /// representation.
    bsl::string& makePath();

    /// Select the "path" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    bsl::string& makePath(const bsl::string& value);

    /// Select the "data" representation. Return a reference to the modifiable
    /// representation.
    bsl::vector<char>& makeData();

    /// Select the "data" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    bsl::vector<char>& makeData(const bsl::vector<char>& value);

    /// Return a reference to the modifiable "certificate" representation. The
    /// behavior is undefined unless 'isCertificate()' is true.
    ntca::EncryptionCertificate& certificate();

    /// Return a reference to the modifiable "key" representation. The
    /// behavior is undefined unless 'isKey()' is true.
    ntca::EncryptionKey& key();

    /// Return a reference to the modifiable "path" representation. The
    /// behavior is undefined unless 'isPath()' is true.
    bsl::string& path();

    /// Return a reference to the modifiable "data" representation. The
    /// behavior is undefined unless 'isData()' is true.
    bsl::vector<char>& data();

    /// Return a reference to the non-modifiable "certificate" representation.
    /// The behavior is undefined unless 'isCertificate()' is true.
    const ntca::EncryptionCertificate& certificate() const;

    /// Return a reference to the non-modifiable "key" representation. The
    /// behavior is undefined unless 'isKey()' is true.
    const ntca::EncryptionKey& key() const;

    /// Return a reference to the non-modifiable "path" representation. The
    /// behavior is undefined unless 'isPath()' is true.
    const bsl::string& path() const;

    /// Return a reference to the non-modifiable "data" representation.  The
    /// behavior is undefined unless 'isData()' is true.
    const bsl::vector<char>& data() const;

    /// Return true if the representation is undefined, otherwise return false.
    bool isUndefined() const;

    /// Return true if the "certificate" representation is currently selected,
    /// otherwise return false.
    bool isCertificate() const;

    /// Return true if the "key" representation is currently selected,
    /// otherwise return false.
    bool isKey() const;

    /// Return true if the "path" representation is currently selected,
    /// otherwise return false.
    bool isPath() const;

    /// Return true if the "data" representation is currently selected,
    /// otherwise return false.
    bool isData() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const EncryptionResourceDescriptor& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const EncryptionResourceDescriptor& other) const;

    /// Format this object to the specified output 'stream' at the optionally
    /// specified indentation 'level' and return a reference to the modifiable
    /// 'stream'.  If 'level' is specified, optionally specify
    /// 'spacesPerLevel', the number of spaces per indentation level for this
    /// and all of its nested objects.  Each line is indented by the absolute
    /// value of 'level * spacesPerLevel'.  If 'level' is negative, suppress
    /// indentation of the first line.  If 'spacesPerLevel' is negative,
    /// suppress line breaks and format the entire output on one line.  If
    /// 'stream' is initially invalid, this operation has no effect.  Note that
    /// a trailing newline is provided in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Defines the traits of this type. These traits can be used to select, at
    /// compile-time, the most efficient algorithm to manipulate objects of
    /// this type.
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionResourceDescriptor);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionResourceDescriptor
bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const EncryptionResourceDescriptor& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionResourceDescriptor
bool operator==(const EncryptionResourceDescriptor& lhs,
                const EncryptionResourceDescriptor& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionResourceDescriptor
bool operator!=(const EncryptionResourceDescriptor& lhs,
                const EncryptionResourceDescriptor& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionResourceDescriptor
bool operator<(const EncryptionResourceDescriptor& lhs,
               const EncryptionResourceDescriptor& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionResourceDescriptor
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                     algorithm,
                const EncryptionResourceDescriptor& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                     algorithm,
                const EncryptionResourceDescriptor& value)
{
    using bslh::hashAppend;

    if (value.isCertificate()) {
        hashAppend(algorithm, value.certificate());
    }
    else if (value.isKey()) {
        hashAppend(algorithm, value.key());
    }
    else if (value.isPath()) {
        hashAppend(algorithm, value.path());
    }
    else if (value.isData()) {
        hashAppend(algorithm, value.data());
    }
}

}  // close package namespace
}  // close enterprise namespace
#endif
