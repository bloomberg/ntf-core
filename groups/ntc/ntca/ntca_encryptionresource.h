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

#ifndef INCLUDED_NTCA_ENCRYPTIONRESOURCE
#define INCLUDED_NTCA_ENCRYPTIONRESOURCE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_encryptionresourcedescriptor.h>
#include <ntca_encryptionresourceoptions.h>
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

/// Describe the definition of an encryption resource.
///
/// @details
/// An encryption resource is composed of an optional private key, optional
/// user certificate, and/or zero or more trusted certificates.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b descriptor:
/// The descriptor of the resource: either a path to a file on a file
/// system, or an encoding of the literal data.
///
/// @li @b options:
/// The options used to interpret the resource from its storage.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionResource
{
    ntca::EncryptionResourceDescriptor                   d_descriptor;
    bdlb::NullableValue<ntca::EncryptionResourceOptions> d_options;

  public:
    /// Create new encryption resource data having the default value.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit EncryptionResource(bslma::Allocator* basicAllocator = 0);

    /// Create new encryption resource data having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    EncryptionResource(const EncryptionResource& original,
                       bslma::Allocator*         basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionResource();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionResource& operator=(const EncryptionResource& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the descriptor to the specified 'value'.
    void setDescriptor(const ntca::EncryptionResourceDescriptor& value);

    /// Set the options to the specified 'value'.
    void setOptions(const ntca::EncryptionResourceOptions& value);

    /// Return the descriptor.
    const ntca::EncryptionResourceDescriptor& descriptor() const;

    /// Return the options.
    const bdlb::NullableValue<ntca::EncryptionResourceOptions>& options()
        const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const EncryptionResource& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const EncryptionResource& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(EncryptionResource);
};

/// Define a type alias for a vector of encryption resource data.
typedef bsl::vector<ntca::EncryptionResource> EncryptionResourceVector;

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionResource
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const EncryptionResource& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionResource
bool operator==(const EncryptionResource& lhs, const EncryptionResource& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionResource
bool operator!=(const EncryptionResource& lhs, const EncryptionResource& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionResource
bool operator<(const EncryptionResource& lhs, const EncryptionResource& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionResource
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionResource& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionResource& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.descriptor());
    hashAppend(algorithm, value.options());
}

}  // close package namespace
}  // close enterprise namespace
#endif
