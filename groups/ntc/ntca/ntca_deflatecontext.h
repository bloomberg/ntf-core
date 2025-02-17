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

#ifndef INCLUDED_NTCA_DEFLATECONTEXT
#define INCLUDED_NTCA_DEFLATECONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_checksum.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Provide a description of the result of a deflate operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b bytesRead:
/// The number of bytes read from the input.
///
/// @li @b bytesWritten:
/// The number of bytes written to the output.
///
/// @li @b checksum:
/// The checksum of the bytes processed.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_todo
class DeflateContext
{
    bsl::size_t    d_bytesRead;
    bsl::size_t    d_bytesWritten;
    ntca::Checksum d_checksum;

  public:
    /// Create a new deflate context having the default value.
    DeflateContext();

    /// Create a new default context having the same value as the specified
    /// 'original' object.
    DeflateContext(const DeflateContext& original);

    /// Destroy this object.
    ~DeflateContext();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    DeflateContext& operator=(const DeflateContext& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the number of bytes read to the specified 'value'.
    void setBytesRead(bsl::size_t value);

    /// Set the number of bytes written to the specified 'value'.
    void setBytesWritten(bsl::size_t value);

    /// Set the checksum to the specified 'value'.
    void setChecksum(const ntca::Checksum& value);

    /// Return the number of bytes read.
    bsl::size_t bytesRead() const;

    /// Return the number of bytes written.
    bsl::size_t bytesWritten() const;

    /// Return the checksum.
    const ntca::Checksum& checksum() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const DeflateContext& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const DeflateContext& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(DeflateContext);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(DeflateContext);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::DeflateContext
bsl::ostream& operator<<(bsl::ostream& stream, const DeflateContext& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::DeflateContext
bool operator==(const DeflateContext& lhs, const DeflateContext& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::DeflateContext
bool operator!=(const DeflateContext& lhs, const DeflateContext& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::DeflateContext
bool operator<(const DeflateContext& lhs, const DeflateContext& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::DeflateContext
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const DeflateContext& value);

NTCCFG_INLINE
DeflateContext::DeflateContext()
: d_bytesRead(0)
, d_bytesWritten(0)
, d_checksum()
{
}

NTCCFG_INLINE
DeflateContext::DeflateContext(const DeflateContext& original)
: d_bytesRead(original.d_bytesRead)
, d_bytesWritten(original.d_bytesWritten)
, d_checksum(original.d_checksum)
{
}

NTCCFG_INLINE
DeflateContext::~DeflateContext()
{
}

NTCCFG_INLINE
DeflateContext& DeflateContext::operator=(const DeflateContext& other)
{
    d_bytesRead    = other.d_bytesRead;
    d_bytesWritten = other.d_bytesWritten;
    d_checksum     = other.d_checksum;

    return *this;
}

NTCCFG_INLINE
void DeflateContext::reset()
{
    d_bytesRead    = 0;
    d_bytesWritten = 0;
    d_checksum.reset();
}

NTCCFG_INLINE
void DeflateContext::setBytesRead(bsl::size_t value)
{
    d_bytesRead = value;
}

NTCCFG_INLINE
void DeflateContext::setBytesWritten(bsl::size_t value)
{
    d_bytesWritten = value;
}

NTCCFG_INLINE
void DeflateContext::setChecksum(const ntca::Checksum& value)
{
    d_checksum = value;
}

NTCCFG_INLINE
bsl::size_t DeflateContext::bytesRead() const
{
    return d_bytesRead;
}

NTCCFG_INLINE
bsl::size_t DeflateContext::bytesWritten() const
{
    return d_bytesWritten;
}

NTCCFG_INLINE
const ntca::Checksum& DeflateContext::checksum() const
{
    return d_checksum;
}

template <typename HASH_ALGORITHM>
NTCCFG_INLINE void DeflateContext::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_bytesRead);
    hashAppend(algorithm, d_bytesWritten);
    hashAppend(algorithm, d_checksum);
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const DeflateContext& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const DeflateContext& lhs, const DeflateContext& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const DeflateContext& lhs, const DeflateContext& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const DeflateContext& lhs, const DeflateContext& rhs)
{
    return lhs.less(rhs);
}

}  // close namespace ntca
}  // close namespace BloombergLP
#endif
