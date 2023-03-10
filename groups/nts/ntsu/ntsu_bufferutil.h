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

#ifndef INCLUDED_NTSU_BUFFERUTIL
#define INCLUDED_NTSU_BUFFERUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_buffer.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlbb_blob.h>

namespace BloombergLP {
namespace ntsu {

/// @internal @brief
/// Provide utilities for scattered/gathered I/O.
///
/// @details
/// Provide utilities to fill 'ntsa::MutableBuffer' or
/// 'ntsa::ConstBuffer' arrays from a 'bdlbb::Blob' for the purposes of
/// performing scattered/gathered I/O.
///
/// @par Thread Safety
// This struct is thread safe.
///
/// @par Usage Example 1: Preparing for a Scattered Read
/// This example illustrates how to define an array of 'ntsa::MutableBuffer'
/// from the non-data "capacity" buffers contained in a 'bdlbb::Blob' for the
/// purposes of performing a scattered read from a socket (i.e., 'readv' on
/// POSIX systems.)
///
///     TODO
///
/// @par Usage Example 1: Preparing for a Gathered Write
/// This example illustrates how to define an array of 'ntsa::ConstBuffer'
/// from the data buffers contained in a 'bdlbb::Blob' for the purposes of
/// performing a gathered write to a socket (i.e., 'writev' on POSIX systems.)
///
///     TODO
///
/// @ingroup module_ntsu
struct BufferUtil {
    /// Prepare to gather data to send from the specified 'data' blob by
    /// referencing up to the specified 'iovecArraySize' elements in the
    /// specified 'iovecArray' to corresponding buffers in 'data'. Stop when
    /// the total size of all buffers referenced exceeds the specified
    /// 'maxBytes'. Return the total size of all buffers referenced.
    static void gather(bsl::size_t*       numBuffersTotal,
                       bsl::size_t*       numBytesTotal,
                       ntsa::ConstBuffer* iovecArray,
                       bsl::size_t        iovecArraySize,
                       const bdlbb::Blob& data,
                       bsl::size_t        maxBytes);

    /// Prepare to gather data to send from the specified 'data' blob by
    /// referencing up to the specified 'iovecArraySize' elements in the
    /// specified 'iovecArray' to corresponding buffers in 'data'. Stop when
    /// the total size of all buffers referenced exceeds the specified
    /// 'maxBytes'. Return the total size of all buffers referenced.
    static void gather(bsl::size_t*            numBuffersTotal,
                       bsl::size_t*            numBytesTotal,
                       ntsa::ConstBufferArray* iovecArray,
                       bsl::size_t             iovecArraySize,
                       const bdlbb::Blob&      data,
                       bsl::size_t             maxBytes);

    /// Prepare to scatter data to receive into the specified 'data' blob by
    /// referencing up to the specified 'iovecArraySize' elements in the
    /// specified 'iovecArray' to corresponding buffers in 'data'. Stop when
    /// the total size of all buffers referenced exceeds the specified
    /// 'maxBytes'. Return the total size of all buffers referenced.
    static void scatter(bsl::size_t*         numBuffersTotal,
                        bsl::size_t*         numBytesTotal,
                        ntsa::MutableBuffer* iovecArray,
                        bsl::size_t          iovecArraySize,
                        bdlbb::Blob*         data,
                        bsl::size_t          maxBytes);

    /// Prepare to scatter data to receive into the specified 'data' blob by
    /// referencing up to the specified 'iovecArraySize' elements in the
    /// specified 'iovecArray' to corresponding buffers in 'data'. Stop when
    /// the total size of all buffers referenced exceeds the specified
    /// 'maxBytes'. Return the total size of all buffers referenced.
    static void scatter(bsl::size_t*              numBuffersTotal,
                        bsl::size_t*              numBytesTotal,
                        ntsa::MutableBufferArray* iovecArray,
                        bsl::size_t               iovecArraySize,
                        bdlbb::Blob*              data,
                        bsl::size_t               maxBytes);

    /// Append to the specified 'blob' a copy of the data referenced by the
    /// specified 'numVecs' in the specified 'vecs' starting at the
    /// specified logical 'offset', in bytes, in the logical seqeuence of
    /// data referenced by 'vecs'.
    static void copy(bdlbb::Blob*             blob,
                     const ntsa::ConstBuffer* vecs,
                     bsl::size_t              numVecs,
                     bsl::size_t              offset);

    /// Append to the specified 'blob' a copy of the data referenced by the
    /// specified 'numVecs' in the specified 'vecs' starting at the
    /// specified logical 'offset', in bytes, in the logical seqeuence of
    /// data referenced by 'vecs'.
    static void copy(bdlbb::Blob*               blob,
                     const ntsa::MutableBuffer* vecs,
                     bsl::size_t                numVecs,
                     bsl::size_t                offset);

    /// Return the total number of bytes referenced by the specified
    /// 'iovecArray' having the specified 'iovecArraySize'.
    static bsl::size_t calculateTotalBytes(const ntsa::ConstBuffer* iovecArray,
                                           bsl::size_t iovecArraySize);

    /// Return the total number of bytes referenced by the specified
    /// 'iovecArray' having the specified 'iovecArraySize'.
    static bsl::size_t calculateTotalBytes(
        const ntsa::MutableBuffer* iovecArray,
        bsl::size_t                iovecArraySize);
};

}  // end namespace ntsu
}  // end namespace BloombergLP
#endif
