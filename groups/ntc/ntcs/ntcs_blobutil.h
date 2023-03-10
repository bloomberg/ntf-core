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

#ifndef INCLUDED_NTCS_BLOBUTIL
#define INCLUDED_NTCS_BLOBUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobutil.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide utilities for blobs.
///
/// @details
/// Provide a utility for accessing and manipulating blobs using
/// standard 'bsl::size_t' types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcs
struct BlobUtil {
    /// Allocate buffers, if necessary, to the specified 'blob' such that
    /// the capacity is at least the specified 'capacity'.
    static void reserve(bdlbb::Blob* blob, bsl::size_t capacity);

    /// Allocate buffers, if necessary, to the specified 'blob' such that
    /// the capacity is at least the specified 'capacity'.
    static void reserve(const bsl::shared_ptr<bdlbb::Blob>& blob,
                        bsl::size_t                         capacity);

    /// Allocate buffers, if necessary, to the specified 'blob' such that
    /// the capacity is at least the specified 'size' and the length is
    /// exactly the specified 'size'. Note that resizing a blob to a size
    /// smaller than its current size has no effect on the capacity of the
    /// blob.
    static void resize(bdlbb::Blob* blob, bsl::size_t size);

    /// Allocate buffers, if necessary, to the specified 'blob' such that
    /// the capacity is at least the specified 'size' and the length is
    /// exactly the specified 'size'. Note that resizing a blob to a size
    /// smaller than its current size has no effect on the capacity of the
    /// blob.
    static void resize(const bsl::shared_ptr<bdlbb::Blob>& blob,
                       bsl::size_t                         size);

    /// Set the size of the last data buffer in the specified 'blob' to
    /// 'blob->lastDataBufferLength()'. If there are no data buffers, or if
    /// the last data buffer is full (i.e., its size is
    /// 'blob->lastDataBufferLength()'), then this method has no effect.
    /// Note that the length of the blob is unchanged, and that capacity
    /// buffers (i.e., of indices 'numDataBuffers()' and higher are *not*
    /// removed.
    static void trim(bdlbb::Blob* blob);

    /// Set the size of the last data buffer in the specified 'blob' to
    /// 'blob->lastDataBufferLength()'. If there are no data buffers, or if
    /// the last data buffer is full (i.e., its size is
    /// 'blob->lastDataBufferLength()'), then this method has no effect.
    /// Note that the length of the blob is unchanged, and that capacity
    /// buffers (i.e., of indices 'numDataBuffers()' and higher are *not*
    /// removed.
    static void trim(const bsl::shared_ptr<bdlbb::Blob>& blob);

    /// Append the specified 'size' number of bytes from the start of the
    /// specified 'source' blob to the specified 'destination' blob.
    static void append(bdlbb::Blob*       destination,
                       const bdlbb::Blob& source,
                       bsl::size_t        size);

    /// Append the specified 'size' number of bytes from the start of the
    /// specified 'source' blob to the specified 'destination' blob.
    static void append(bdlbb::Blob*                        destination,
                       const bsl::shared_ptr<bdlbb::Blob>& source,
                       bsl::size_t                         size);

    /// Append the specified 'size' number of bytes from the start of the
    /// specified 'source' blob to the specified 'destination' blob.
    static void append(const bsl::shared_ptr<bdlbb::Blob>& destination,
                       const bdlbb::Blob&                  source,
                       bsl::size_t                         size);

    /// Append the specified 'size' number of bytes from the start of the
    /// specified 'source' blob to the specified 'destination' blob.
    static void append(const bsl::shared_ptr<bdlbb::Blob>& destination,
                       const bsl::shared_ptr<bdlbb::Blob>& source,
                       bsl::size_t                         size);

    /// Pop the specified 'size' number of bytes from the specified 'blob'.
    static void pop(bdlbb::Blob* blob, bsl::size_t size);

    /// Pop the specified 'size' number of bytes from the specified 'blob'.
    static void pop(const bsl::shared_ptr<bdlbb::Blob>& blob,
                    bsl::size_t                         size);

    /// Return the number of bytes in the specified 'blob' that have been
    /// allocated and written.
    static bsl::size_t size(const bdlbb::Blob& blob);

    /// Return the number of bytes in the specified 'blob' that have been
    /// allocated and written.
    static bsl::size_t size(const bsl::shared_ptr<bdlbb::Blob>& blob);

    /// Return the number of bytes in the specified 'blob' that have been
    /// allocated but not yet written.
    static bsl::size_t capacity(const bdlbb::Blob& blob);

    /// Return the number of bytes in the specified 'blob' that have been
    /// allocated but not yet written.
    static bsl::size_t capacity(const bsl::shared_ptr<bdlbb::Blob>& blob);
};

NTCCFG_INLINE
void BlobUtil::reserve(bdlbb::Blob* blob, bsl::size_t capacity)
{
    int length = blob->length();
    blob->setLength(NTCCFG_WARNING_NARROW(int, capacity));
    blob->setLength(length);
}

NTCCFG_INLINE
void BlobUtil::reserve(const bsl::shared_ptr<bdlbb::Blob>& blob,
                       bsl::size_t                         capacity)
{
    int length = blob->length();
    blob->setLength(NTCCFG_WARNING_NARROW(int, capacity));
    blob->setLength(length);
}

NTCCFG_INLINE
void BlobUtil::resize(bdlbb::Blob* blob, bsl::size_t size)
{
    blob->setLength(NTCCFG_WARNING_NARROW(int, size));
}

NTCCFG_INLINE
void BlobUtil::resize(const bsl::shared_ptr<bdlbb::Blob>& blob,
                      bsl::size_t                         size)
{
    blob->setLength(NTCCFG_WARNING_NARROW(int, size));
}

NTCCFG_INLINE
void BlobUtil::trim(bdlbb::Blob* blob)
{
    blob->trimLastDataBuffer();
}

NTCCFG_INLINE
void BlobUtil::trim(const bsl::shared_ptr<bdlbb::Blob>& blob)
{
    blob->trimLastDataBuffer();
}

NTCCFG_INLINE
void BlobUtil::append(bdlbb::Blob*       destination,
                      const bdlbb::Blob& source,
                      bsl::size_t        size)
{
    bdlbb::BlobUtil::append(destination,
                            source,
                            0,
                            NTCCFG_WARNING_NARROW(int, size));
}

NTCCFG_INLINE
void BlobUtil::append(bdlbb::Blob*                        destination,
                      const bsl::shared_ptr<bdlbb::Blob>& source,
                      bsl::size_t                         size)
{
    bdlbb::BlobUtil::append(destination,
                            *source,
                            0,
                            NTCCFG_WARNING_NARROW(int, size));
}

NTCCFG_INLINE
void BlobUtil::append(const bsl::shared_ptr<bdlbb::Blob>& destination,
                      const bdlbb::Blob&                  source,
                      bsl::size_t                         size)
{
    bdlbb::BlobUtil::append(destination.get(),
                            source,
                            0,
                            NTCCFG_WARNING_NARROW(int, size));
}

NTCCFG_INLINE
void BlobUtil::append(const bsl::shared_ptr<bdlbb::Blob>& destination,
                      const bsl::shared_ptr<bdlbb::Blob>& source,
                      bsl::size_t                         size)
{
    bdlbb::BlobUtil::append(destination.get(),
                            *source,
                            0,
                            NTCCFG_WARNING_NARROW(int, size));
}

NTCCFG_INLINE
void BlobUtil::pop(bdlbb::Blob* blob, bsl::size_t size)
{
    return bdlbb::BlobUtil::erase(blob, 0, NTCCFG_WARNING_NARROW(int, size));
}

NTCCFG_INLINE
void BlobUtil::pop(const bsl::shared_ptr<bdlbb::Blob>& blob, bsl::size_t size)
{
    return bdlbb::BlobUtil::erase(blob.get(),
                                  0,
                                  NTCCFG_WARNING_NARROW(int, size));
}

NTCCFG_INLINE
bsl::size_t BlobUtil::size(const bdlbb::Blob& blob)
{
    return blob.length();
}

NTCCFG_INLINE
bsl::size_t BlobUtil::size(const bsl::shared_ptr<bdlbb::Blob>& blob)
{
    return blob->length();
}

NTCCFG_INLINE
bsl::size_t BlobUtil::capacity(const bdlbb::Blob& blob)
{
    return blob.totalSize();
}

NTCCFG_INLINE
bsl::size_t BlobUtil::capacity(const bsl::shared_ptr<bdlbb::Blob>& blob)
{
    return blob->totalSize();
}

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
