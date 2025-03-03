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
#include <bdlb_nullablevalue.h>
#include <bdlbb_blob.h>
#include <bdlde_crc32.h>
#include <bslh_hash.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Provide a checksum calculated according to the Adler-32 algorithm.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_todo
class ChecksumAdler32
{
    bsl::uint32_t d_value;

  public:
    /// Defines a type alias for the unsigned 32-bit integer that represents
    /// the value of the checksum.
    typedef bsl::uint32_t Digest;

    /// Create a new checksum having a default value.
    ChecksumAdler32();

    /// Create a new checksum having the specified 'digest'.
    explicit ChecksumAdler32(Digest digest);

    /// Create a new checksum having the same value as the specified
    /// 'original' object.
    ChecksumAdler32(const ChecksumAdler32& original);

    /// Create a new checksum having the same value as the specified 'original'
    /// object. The value of the 'original' object becomes unspecified but
    /// valid.
    ChecksumAdler32(bslmf::MovableRef<ChecksumAdler32> original)
        NTSCFG_NOEXCEPT;

    /// Destroy this object.
    ~ChecksumAdler32();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    ChecksumAdler32& operator=(const ChecksumAdler32& other);

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    ChecksumAdler32& operator=(bslmf::MovableRef<ChecksumAdler32> other)
        NTSCFG_NOEXCEPT;

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Update the checksum for the specified 'data' over the specified 'size'
    /// number of bytes. Return the error.
    ntsa::Error update(const void* data, bsl::size_t size);

    /// Update the checksum for the specified 'data'. Return the error.
    ntsa::Error update(const bdlbb::Blob& data);

    /// Update the checksum for the specified 'data' over the specified 'size'
    /// number of bytes. Return the error.
    ntsa::Error update(const bdlbb::Blob& data, bsl::size_t size);

    /// Return the digest.
    bsl::uint32_t value() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const ChecksumAdler32& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const ChecksumAdler32& other) const;

    /// Return the hash value of this object according to the default hash
    /// algorithm.
    bsl::size_t hash() const;

    /// Return the hash value of this object according to the parameterized
    /// hash algorithm.
    template <typename HASH_ALGORITHM>
    bsl::size_t hash() const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(ChecksumAdler32);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(ChecksumAdler32);
};

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
///
/// @related ntca::ChecksumAdler32
bsl::ostream& operator<<(bsl::ostream& stream, const ChecksumAdler32& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ChecksumAdler32
bool operator==(const ChecksumAdler32& lhs, const ChecksumAdler32& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ChecksumAdler32
bool operator!=(const ChecksumAdler32& lhs, const ChecksumAdler32& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ChecksumAdler32
bool operator<(const ChecksumAdler32& lhs, const ChecksumAdler32& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ChecksumAdler32
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ChecksumAdler32& value);

/// Provide a checksum calculated according to the CRC-32 algorithm.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_todo
class ChecksumCrc32
{
    bsl::uint32_t d_value;

  public:
    /// Defines a type alias for the unsigned 32-bit integer that represents
    /// the value of the checksum.
    typedef bsl::uint32_t Digest;

    /// Create a new checksum having a default value.
    ChecksumCrc32();

    /// Create a new checksum having the specified 'digest'.
    explicit ChecksumCrc32(Digest digest);

    /// Create a new checksum having the same value as the specified
    /// 'original' object.
    ChecksumCrc32(const ChecksumCrc32& original);

    /// Create a new checksum having the same value as the specified 'original'
    /// object. The value of the 'original' object becomes unspecified but
    /// valid.
    ChecksumCrc32(bslmf::MovableRef<ChecksumCrc32> original) NTSCFG_NOEXCEPT;

    /// Destroy this object.
    ~ChecksumCrc32();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    ChecksumCrc32& operator=(const ChecksumCrc32& other);

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    ChecksumCrc32& operator=(bslmf::MovableRef<ChecksumCrc32> other)
        NTSCFG_NOEXCEPT;

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Update the checksum for the specified 'data' over the specified 'size'
    /// number of bytes. Return the error.
    ntsa::Error update(const void* data, bsl::size_t size);

    /// Update the checksum for the specified 'data'. Return the error.
    ntsa::Error update(const bdlbb::Blob& data);

    /// Update the checksum for the specified 'data' over the specified 'size'
    /// number of bytes. Return the error.
    ntsa::Error update(const bdlbb::Blob& data, bsl::size_t size);

    /// Return the digest.
    bsl::uint32_t value() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const ChecksumCrc32& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const ChecksumCrc32& other) const;

    /// Return the hash value of this object according to the default hash
    /// algorithm.
    bsl::size_t hash() const;

    /// Return the hash value of this object according to the parameterized
    /// hash algorithm.
    template <typename HASH_ALGORITHM>
    bsl::size_t hash() const;

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
    /// corresponding byte of the destination object's footprint.`
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(ChecksumCrc32);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(ChecksumCrc32);
};

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
///
/// @related ntca::ChecksumCrc32
bsl::ostream& operator<<(bsl::ostream& stream, const ChecksumCrc32& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ChecksumCrc32
bool operator==(const ChecksumCrc32& lhs, const ChecksumCrc32& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ChecksumCrc32
bool operator!=(const ChecksumCrc32& lhs, const ChecksumCrc32& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ChecksumCrc32
bool operator<(const ChecksumCrc32& lhs, const ChecksumCrc32& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ChecksumCrc32
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ChecksumCrc32& value);

/// Provide a checksum calculated according to the XXHASH-32 algorithm.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_todo
class ChecksumXxHash32
{
    typedef ChecksumXxHash32 Self;

    static const uint32_t k_P1 = 0x9E3779B1U;
    static const uint32_t k_P2 = 0x85EBCA77U;
    static const uint32_t k_P3 = 0xC2B2AE3DU;
    static const uint32_t k_P4 = 0x27D4EB2FU;
    static const uint32_t k_P5 = 0x165667B1U;

    static const uint32_t k_SEED = 0;

    bsl::uint32_t d_accumulator[4];
    bsl::uint8_t  d_buffer[16];
    bsl::uint32_t d_bufferSize;
    bsl::uint32_t d_entireSize;
    bsl::uint32_t d_full;

  private:
    /// Decode a 32-bit unsigned integer from the specified 'offset' into the
    /// specified 'data'. Return the result.
    static bsl::uint32_t decode(const bsl::uint8_t* data, bsl::size_t offset);

    /// Shift the specified 'value' left by the specified 'amount'. Return the
    /// result.
    static bsl::uint32_t rotate(bsl::uint32_t value, bsl::uint32_t amount);

  public:
    /// Defines a type alias for the unsigned 32-bit integer that represents
    /// the value of the checksum.
    typedef bsl::uint32_t Digest;

    /// Create a new checksum having a default value.
    ChecksumXxHash32();

    /// Create a new checksum having the specified 'digest'.
    explicit ChecksumXxHash32(Digest digest);

    /// Create a new checksum having the same value as the specified
    /// 'original' object.
    ChecksumXxHash32(const ChecksumXxHash32& original);

    /// Create a new checksum having the same value as the specified 'original'
    /// object. The value of the 'original' object becomes unspecified but
    /// valid.
    ChecksumXxHash32(bslmf::MovableRef<ChecksumXxHash32> original)
        NTSCFG_NOEXCEPT;

    /// Destroy this object.
    ~ChecksumXxHash32();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    ChecksumXxHash32& operator=(const ChecksumXxHash32& other);

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    ChecksumXxHash32& operator=(bslmf::MovableRef<ChecksumXxHash32> other)
        NTSCFG_NOEXCEPT;

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Reset the value of this object to its value upon construction with the
    /// specified 'digest'.
    void reset(Digest digest);

    /// Update the checksum for the specified 'data' over the specified 'size'
    /// number of bytes. Return the error.
    ntsa::Error update(const void* data, bsl::size_t size);

    /// Update the checksum for the specified 'data'. Return the error.
    ntsa::Error update(const bdlbb::Blob& data);

    /// Update the checksum for the specified 'data' over the specified 'size'
    /// number of bytes. Return the error.
    ntsa::Error update(const bdlbb::Blob& data, bsl::size_t size);

    /// Return the digest.
    bsl::uint32_t value() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const ChecksumXxHash32& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const ChecksumXxHash32& other) const;

    /// Return the hash value of this object according to the default hash
    /// algorithm.
    bsl::size_t hash() const;

    /// Return the hash value of this object according to the parameterized
    /// hash algorithm.
    template <typename HASH_ALGORITHM>
    bsl::size_t hash() const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(ChecksumXxHash32);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(ChecksumXxHash32);
};

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
///
/// @related ntca::ChecksumXxHash32
bsl::ostream& operator<<(bsl::ostream& stream, const ChecksumXxHash32& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ChecksumXxHash32
bool operator==(const ChecksumXxHash32& lhs, const ChecksumXxHash32& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ChecksumXxHash32
bool operator!=(const ChecksumXxHash32& lhs, const ChecksumXxHash32& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ChecksumXxHash32
bool operator<(const ChecksumXxHash32& lhs, const ChecksumXxHash32& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ChecksumXxHash32
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ChecksumXxHash32& value);

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
    union {
        bsls::ObjectBuffer<ntca::ChecksumAdler32>  d_adler32;
        bsls::ObjectBuffer<ntca::ChecksumCrc32>    d_crc32;
        bsls::ObjectBuffer<ntca::ChecksumXxHash32> d_xxHash32;
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

    /// Create a new checksum having the same value as the specified 'original'
    /// object. The value of the 'original' object becomes unspecified but
    /// valid.
    Checksum(bslmf::MovableRef<Checksum> original) NTSCFG_NOEXCEPT;

    /// Destroy this object.
    ~Checksum();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    Checksum& operator=(const Checksum& other);

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    Checksum& operator=(bslmf::MovableRef<Checksum> other) NTSCFG_NOEXCEPT;

    /// Reset the value of this object to its value upon construction.
    void reset();

    /// Reset the value of this object to its value upon construction for the
    /// specified checksum 'type'.
    void reset(ntca::ChecksumType::Value type);

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

    /// Return the hash value of this object according to the default hash
    /// algorithm.
    bsl::size_t hash() const;

    /// Return the hash value of this object according to the parameterized
    /// hash algorithm.
    template <typename HASH_ALGORITHM>
    bsl::size_t hash() const;

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

NTCCFG_INLINE bsl::size_t ChecksumAdler32::hash() const
{
    bslh::DefaultHashAlgorithm algorithm;
    hashAppend(algorithm, *this);
    return static_cast<bsl::size_t>(algorithm.computeHash());
}

template <typename HASH_ALGORITHM>
NTCCFG_INLINE bsl::size_t ChecksumAdler32::hash() const
{
    HASH_ALGORITHM algorithm;
    hashAppend(algorithm, *this);
    return static_cast<bsl::size_t>(algorithm.computeHash());
}

template <typename HASH_ALGORITHM>
NTCCFG_INLINE void ChecksumAdler32::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_value);
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ChecksumAdler32& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ChecksumAdler32& lhs, const ChecksumAdler32& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const ChecksumAdler32& lhs, const ChecksumAdler32& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const ChecksumAdler32& lhs, const ChecksumAdler32& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&        algorithm,
                              const ChecksumAdler32& value)
{
    value.hash(algorithm);
}

NTCCFG_INLINE bsl::size_t ChecksumCrc32::hash() const
{
    bslh::DefaultHashAlgorithm algorithm;
    hashAppend(algorithm, *this);
    return static_cast<bsl::size_t>(algorithm.computeHash());
}

template <typename HASH_ALGORITHM>
NTCCFG_INLINE bsl::size_t ChecksumCrc32::hash() const
{
    HASH_ALGORITHM algorithm;
    hashAppend(algorithm, *this);
    return static_cast<bsl::size_t>(algorithm.computeHash());
}

template <typename HASH_ALGORITHM>
NTCCFG_INLINE void ChecksumCrc32::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_value);
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ChecksumCrc32& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ChecksumCrc32& lhs, const ChecksumCrc32& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const ChecksumCrc32& lhs, const ChecksumCrc32& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const ChecksumCrc32& lhs, const ChecksumCrc32& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&      algorithm,
                              const ChecksumCrc32& value)
{
    value.hash(algorithm);
}

NTCCFG_INLINE bsl::size_t ChecksumXxHash32::hash() const
{
    bslh::DefaultHashAlgorithm algorithm;
    hashAppend(algorithm, *this);
    return static_cast<bsl::size_t>(algorithm.computeHash());
}

template <typename HASH_ALGORITHM>
NTCCFG_INLINE bsl::size_t ChecksumXxHash32::hash() const
{
    HASH_ALGORITHM algorithm;
    hashAppend(algorithm, *this);
    return static_cast<bsl::size_t>(algorithm.computeHash());
}

template <typename HASH_ALGORITHM>
NTCCFG_INLINE void ChecksumXxHash32::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    algorithm(reinterpret_cast<const char*>(d_accumulator),
              sizeof d_accumulator);
    algorithm(reinterpret_cast<const char*>(d_buffer), d_bufferSize);
    hashAppend(algorithm, d_entireSize);
    hashAppend(algorithm, d_full);
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ChecksumXxHash32& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ChecksumXxHash32& lhs, const ChecksumXxHash32& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const ChecksumXxHash32& lhs, const ChecksumXxHash32& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const ChecksumXxHash32& lhs, const ChecksumXxHash32& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&         algorithm,
                              const ChecksumXxHash32& value)
{
    value.hash(algorithm);
}

NTCCFG_INLINE bsl::size_t Checksum::hash() const
{
    bslh::DefaultHashAlgorithm algorithm;
    hashAppend(algorithm, *this);
    return static_cast<bsl::size_t>(algorithm.computeHash());
}

template <typename HASH_ALGORITHM>
NTCCFG_INLINE bsl::size_t Checksum::hash() const
{
    HASH_ALGORITHM algorithm;
    hashAppend(algorithm, *this);
    return static_cast<bsl::size_t>(algorithm.computeHash());
}

template <typename HASH_ALGORITHM>
NTCCFG_INLINE void Checksum::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, static_cast<int>(d_type));
    if (d_type == ntca::ChecksumType::e_ADLER32) {
        hashAppend(algorithm, d_adler32.object());
    }
    else if (d_type == ntca::ChecksumType::e_CRC32) {
        hashAppend(algorithm, d_crc32.object());
    }
    else if (d_type == ntca::ChecksumType::e_XXH32) {
        hashAppend(algorithm, d_xxHash32.object());
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

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM& algorithm, const Checksum& value)
{
    value.hash(algorithm);
}

}  // close namespace ntca
}  // close namespace BloombergLP
#endif
