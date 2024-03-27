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

#ifndef INCLUDED_NTCA_ENCRYPTIONKEYSTORAGEDATA
#define INCLUDED_NTCA_ENCRYPTIONKEYSTORAGEDATA

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_encryptionkeystoragedescriptor.h>
#include <ntca_encryptionkeystorageoptions.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntca {

/// Describe the storage of a key.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b descriptor:
/// The descriptor of the key storage: either a path to a file on a file
/// system, or an encoding of the literal data.
///
/// @li @b options:
/// The options used to interpret the key from its storage.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionKeyStorageData
{
    ntca::EncryptionKeyStorageDescriptor                   d_descriptor;
    bdlb::NullableValue<ntca::EncryptionKeyStorageOptions> d_options;

  public:
    /// Create a new key generation configuration having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit EncryptionKeyStorageData(bslma::Allocator* basicAllocator = 0);

    /// Create a new key generation configuration having the same value as
    /// the specified 'original' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    EncryptionKeyStorageData(const EncryptionKeyStorageData& original,
                             bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionKeyStorageData();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionKeyStorageData& operator=(const EncryptionKeyStorageData& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the descriptor to the specified 'value'.
    void setDescriptor(const ntca::EncryptionKeyStorageDescriptor& value);

    /// Set the options to the specified 'value'.
    void setOptions(const ntca::EncryptionKeyStorageOptions& value);

    /// Return the descriptor.
    const ntca::EncryptionKeyStorageDescriptor& descriptor() const;

    /// Return the options.
    const bdlb::NullableValue<ntca::EncryptionKeyStorageOptions>& options()
        const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionKeyStorageData& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionKeyStorageData& other) const;

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

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionKeyStorageData);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionKeyStorageData
bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const EncryptionKeyStorageData& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionKeyStorageData
bool operator==(const EncryptionKeyStorageData& lhs,
                const EncryptionKeyStorageData& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionKeyStorageData
bool operator!=(const EncryptionKeyStorageData& lhs,
                const EncryptionKeyStorageData& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionKeyStorageData
bool operator<(const EncryptionKeyStorageData& lhs,
               const EncryptionKeyStorageData& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionKeyStorageData
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                 algorithm,
                const EncryptionKeyStorageData& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                 algorithm,
                const EncryptionKeyStorageData& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.descriptor());
    hashAppend(algorithm, value.options());
}

}  // close package namespace
}  // close enterprise namespace
#endif
