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

#ifndef INCLUDED_NTSA_FILE
#define INCLUDED_NTSA_FILE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdls_filesystemutil.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a description of a file for transmission.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class File
{
    bdls::FilesystemUtil::FileDescriptor d_fileDescriptor;
    bdls::FilesystemUtil::Offset         d_filePosition;
    bdls::FilesystemUtil::Offset         d_fileBytesRemaining;
    bdls::FilesystemUtil::Offset         d_fileSize;

  public:
    /// Create a new file description.
    File();

    /// Create a new file description of the specified 'size' bytes starting
    /// at the specified 'position' in the file identified by the specified
    /// 'descriptor'
    File(bdls::FilesystemUtil::FileDescriptor descriptor,
         bdls::FilesystemUtil::Offset         position,
         bdls::FilesystemUtil::Offset         size);

    /// Set the descriptor of the file to the specified 'descriptor'.
    void setDescriptor(bdls::FilesystemUtil::FileDescriptor descriptor);

    /// Set the position of the next transmission of the file to the
    /// specified 'position'.
    void setPosition(bdls::FilesystemUtil::Offset position);

    /// Set the number of bytes remaining to transmit to the specified
    /// 'bytesRemaining'.
    void setBytesRemaining(bdls::FilesystemUtil::Offset bytesRemaining);

    /// Set the total size of the file to the specified 'size'.
    void setSize(bdls::FilesystemUtil::Offset size);

    /// Return the descriptor of the file.
    bdls::FilesystemUtil::FileDescriptor descriptor() const;

    /// Return the position of the next transmission of the file.
    bdls::FilesystemUtil::Offset position() const;

    /// Return the number of bytes remaining to transmit.
    bdls::FilesystemUtil::Offset bytesRemaining() const;

    /// Return the total size of the file.
    bdls::FilesystemUtil::Offset size() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const File& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const File& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(File);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(File);
};

/// Write the specified 'object' to the specified 'stream'. Return a
/// modifiable reference to the 'stream'.
///
/// @related ntsa::File
bsl::ostream& operator<<(bsl::ostream& stream, const File& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::File
bool operator==(const File& lhs, const File& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::File
bool operator!=(const File& lhs, const File& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::File
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const File& value);

NTSCFG_INLINE
File::File()
: d_fileDescriptor(bdls::FilesystemUtil::k_INVALID_FD)
, d_filePosition(0)
, d_fileBytesRemaining(0)
, d_fileSize(0)
{
}

NTSCFG_INLINE
File::File(bdls::FilesystemUtil::FileDescriptor descriptor,
           bdls::FilesystemUtil::Offset         position,
           bdls::FilesystemUtil::Offset         size)
: d_fileDescriptor(descriptor)
, d_filePosition(position)
, d_fileBytesRemaining(0)
, d_fileSize(size)
{
}

NTSCFG_INLINE
void File::setDescriptor(bdls::FilesystemUtil::FileDescriptor descriptor)
{
    d_fileDescriptor = descriptor;
}

NTSCFG_INLINE
void File::setPosition(bdls::FilesystemUtil::Offset position)
{
    d_filePosition = position;
}

NTSCFG_INLINE
void File::setBytesRemaining(bdls::FilesystemUtil::Offset bytesRemaining)
{
    d_fileBytesRemaining = bytesRemaining;
}

NTSCFG_INLINE
void File::setSize(bdls::FilesystemUtil::Offset size)
{
    d_fileSize = size;
}

NTSCFG_INLINE
bdls::FilesystemUtil::FileDescriptor File::descriptor() const
{
    return d_fileDescriptor;
}

NTSCFG_INLINE
bdls::FilesystemUtil::Offset File::position() const
{
    return d_filePosition;
}

NTSCFG_INLINE
bdls::FilesystemUtil::Offset File::bytesRemaining() const
{
    return d_fileBytesRemaining;
}

NTSCFG_INLINE
bdls::FilesystemUtil::Offset File::size() const
{
    return d_fileSize;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const File& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const File& lhs, const File& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const File& lhs, const File& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const File& lhs, const File& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const File& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.descriptor());
    hashAppend(algorithm, value.position());
    hashAppend(algorithm, value.bytesRemaining());
    hashAppend(algorithm, value.size());
}

}  // close package namespace
}  // close enterprise namespace
#endif
