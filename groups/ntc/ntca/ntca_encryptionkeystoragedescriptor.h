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

#ifndef INCLUDED_NTCA_ENCRYPTIONKEYSTORAGEDESCRIPTOR
#define INCLUDED_NTCA_ENCRYPTIONKEYSTORAGEDESCRIPTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

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

/// Describe the contents of the storage of a key.
///
/// @details
/// Provide a value-semantic type that represents a discriminated union of
/// either a path to a file on a filesystem or an encoding of literal data.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b path:
/// The path to the stored key.
///
/// @li @b literal:
/// The literal data of the stored key.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionKeyStorageDescriptor
{
    typedef bsl::string       PathType;
    typedef bsl::vector<char> DataType;

    enum Type { e_UNDEFINED, e_PATH, e_DATA };

    union {
        bsls::ObjectBuffer<PathType> d_path;
        bsls::ObjectBuffer<DataType> d_data;
    };

    Type              d_type;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new key storage descriptor having the default value.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit EncryptionKeyStorageDescriptor(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new key storage descriptor having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    EncryptionKeyStorageDescriptor(
        const EncryptionKeyStorageDescriptor& original,
        bslma::Allocator*                     basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionKeyStorageDescriptor();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    EncryptionKeyStorageDescriptor& operator=(
        const EncryptionKeyStorageDescriptor& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

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

    /// Return a reference to the modifiable "path" representation. The
    /// behavior is undefined unless 'isPath()' is true.
    bsl::string& path();

    /// Return a reference to the modifiable "data" representation. The
    /// behavior is undefined unless 'isData()' is true.
    bsl::vector<char>& data();

    /// Return a reference to the non-modifiable "path" representation. The
    /// behavior is undefined unless 'isPath()' is true.
    const bsl::string& path() const;

    /// Return a reference to the non-modifiable "data" representation.  The
    /// behavior is undefined unless 'isData()' is true.
    const bsl::vector<char>& data() const;

    /// Return true if the representation is undefined, otherwise return false.
    bool isUndefined() const;

    /// Return true if the "path" representation is currently selected,
    /// otherwise return false.
    bool isPath() const;

    /// Return true if the "data" representation is currently selected,
    /// otherwise return false.
    bool isData() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const EncryptionKeyStorageDescriptor& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const EncryptionKeyStorageDescriptor& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(
        EncryptionKeyStorageDescriptor);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionKeyStorageDescriptor
bsl::ostream& operator<<(bsl::ostream&                         stream,
                         const EncryptionKeyStorageDescriptor& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionKeyStorageDescriptor
bool operator==(const EncryptionKeyStorageDescriptor& lhs,
                const EncryptionKeyStorageDescriptor& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionKeyStorageDescriptor
bool operator!=(const EncryptionKeyStorageDescriptor& lhs,
                const EncryptionKeyStorageDescriptor& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionKeyStorageDescriptor
bool operator<(const EncryptionKeyStorageDescriptor& lhs,
               const EncryptionKeyStorageDescriptor& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionKeyStorageDescriptor
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                       algorithm,
                const EncryptionKeyStorageDescriptor& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                       algorithm,
                const EncryptionKeyStorageDescriptor& value)
{
    using bslh::hashAppend;

    if (value.isPath()) {
        hashAppend(algorithm, value.path());
    }
    else if (value.isData()) {
        hashAppend(algorithm, value.data());
    }
}

}  // close package namespace
}  // close enterprise namespace
#endif
