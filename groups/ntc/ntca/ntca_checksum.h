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

#ifndef INCLUDED_NTCA_CHECKSUM
#define INCLUDED_NTCA_CHECKSUM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_checksumtype.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bdlde_crc32.h>
#include <bdlde_crc32c.h>
#include <bdlb_nullablevalue.h>
#include <bdlbb_blob.h>
#include <bslh_hash.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Provide a representation of a checksum.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b type:
/// The type of the checksum.
///
/// @li @b value:
/// The 32-bit unsigned integer value of the checksum.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_todo
class Checksum
{
    /// Defines a type alias for a representation of a checksum calculated by
    /// the CRC32 algorithm.
    typedef bdlde::Crc32 Crc32;

    /// Defines a type alias for a representation of a checksum calculated by
    /// the CRC32-C algorithm.
    typedef bsl::uint32_t Crc32c;

    /// Defines a type alias for a representation of a checksum calculated by
    /// the XXHASH32 algorithm.
    typedef bsl::uint32_t XxHash32;

    /// Defines a type alias for a representation of a checksum calculated by
    /// the XXHASH64 algorithm.
    typedef bsl::uint64_t XxHash64;

    union {
        bsls::ObjectBuffer<Crc32>    d_crc32;
        bsls::ObjectBuffer<Crc32c>   d_crc32c;
        bsls::ObjectBuffer<XxHash32> d_xxHash32;
        bsls::ObjectBuffer<XxHash64> d_xxHash64;
    };

    ntca::ChecksumType::Value d_type;

  public:
    /// Create a new checksum of the default type having the default value for
    /// that type.
    Checksum();

    /// Create a new checksum of the specified 'type' having the default value
    /// for that type.
    explicit Checksum(ntca::ChecksumType::Value type);

    /// Create a new checksum having the same value as the specified
    /// 'original' object.
    Checksum(const Checksum& original);

    /// Destroy this object.
    ~Checksum();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    Checksum& operator=(const Checksum& other);

    /// Reset the value of this object to its value upon construction.
    void reset();

    /// Store the specified 'value' having the specified 'size' as the checksum
    /// of the specified 'type'. 
    ntsa::Error store(ntca::ChecksumType::Value type, 
                      const void*               value, 
                      bsl::size_t               size);

    /// Update the checksum for the specified 'data' over the specified 'size'
    /// number of bytes. Return the error.
    ntsa::Error update(const void* data, bsl::size_t size);

    /// Update the checksum for the specified 'data'. Return the error.
    ntsa::Error update(const bdlbb::Blob& data);

    /// Update the checksum for the specified 'data' over the specified 'size'
    /// number of bytes. Return the error.
    ntsa::Error update(const bdlbb::Blob& data, bsl::size_t size);

    /// Return the checksum type. 
    ntca::ChecksumType::Value type() const;

    /// Return the number of bytes in the representation of the checksum. 
    bsl::size_t size() const;

    /// Load into the specified 'result' having the specified 'capacity' the 
    /// value of the checksum. Return the number of bytes written to 'result'.
    bsl::size_t load(void* result, bsl::size_t capacity) const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const Checksum& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const Checksum& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

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

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(Checksum);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(Checksum);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::Checksum
bsl::ostream& operator<<(bsl::ostream& stream, const Checksum& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::Checksum
bool operator==(const Checksum& lhs, const Checksum& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::Checksum
bool operator!=(const Checksum& lhs, const Checksum& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::Checksum
bool operator<(const Checksum& lhs, const Checksum& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::Checksum
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Checksum& value);

template <typename HASH_ALGORITHM>
NTCCFG_INLINE
void Checksum::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_type);
    if (d_type == ntca::ChecksumType::e_CRC32) {
        hashAppend(algorithm, d_crc32.object());
    }
    else if (d_type == ntca::ChecksumType::e_CRC32C) {
        hashAppend(algorithm, d_crc32c.object());
    }
    else if (ntca::ChecksumType::e_XXHASH32) {
        hashAppend(algorithm, d_xxHash32.object());
    }
    else if (ntca::ChecksumType::e_XXHASH64) {
        hashAppend(algorithm, d_xxHash64.object());
    }
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Checksum& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const Checksum& lhs, const Checksum& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const Checksum& lhs, const Checksum& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const Checksum& lhs, const Checksum& rhs)
{
    return lhs.less(rhs);
}

}  // close namespace ntca
}  // close namespace BloombergLP
#endif
