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

#ifndef INCLUDED_NTSA_BUFFER
#define INCLUDED_NTSA_BUFFER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_limits.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlbb_blob.h>
#include <bdlma_localsequentialallocator.h>
#include <bslalg_typetraits.h>
#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>
#include <bsl_cstddef.h>
#include <bsl_ios.h>
#include <bsl_iterator.h>
#include <bsl_streambuf.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a modifiable buffer.
///
/// @details
/// These buffers and sequences are used to reference data that is intended to
/// be copied into and out of socket send and receive buffers. The
/// 'ntsa::MutableBuffer' and 'ntsa::ConstBuffer' are exact replicas of the
/// 'iovec' structure, on POSIX systems, and the 'WSABUF' structure, on
/// Windows. Users may 'reinterpret_cast' pointers between these types.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class MutableBuffer
{
#if defined(BSLS_PLATFORM_OS_UNIX)
    typedef void*       BufferType;
    typedef bsl::size_t LengthType;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    typedef unsigned long LengthType;
    typedef void*         BufferType;
#else
#error Not implemented
#endif

#if defined(BSLS_PLATFORM_OS_UNIX)
    BufferType d_data;
    LengthType d_size;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    LengthType            d_size;
    BufferType            d_data;
#else
#error Not implemented
#endif

  public:
    /// Create a new, empty buffer.
    MutableBuffer() BSLS_KEYWORD_NOEXCEPT;

    /// Create a new buffer describing the specified 'blobBuffer'.
    explicit MutableBuffer(const bdlbb::BlobBuffer& blobBuffer)
        BSLS_KEYWORD_NOEXCEPT;

    /// Create a new buffer describing the specified 'data' having the
    /// specified 'size'.
    MutableBuffer(void* data, bsl::size_t size) BSLS_KEYWORD_NOEXCEPT;

    /// Create a new buffer having the same value as the specified
    /// 'original' object.
    MutableBuffer(const MutableBuffer& original) BSLS_KEYWORD_NOEXCEPT;

    /// Destroy this object.
    ~MutableBuffer() BSLS_KEYWORD_NOEXCEPT;

    /// Assign to this object the value of the specified 'other' object and
    /// return a reference to this modifiable object.
    MutableBuffer& operator=(const MutableBuffer& other) BSLS_KEYWORD_NOEXCEPT;

    /// Increment the address of the data of the buffer by the specified 'n'
    /// number of bytes and decrement its size by the equivalent amount.
    MutableBuffer& operator+=(bsl::size_t n) BSLS_KEYWORD_NOEXCEPT;

    /// Set the buffer to describe the specified 'blobBuffer'.
    void setBuffer(const bdlbb::BlobBuffer& blobBuffer) BSLS_KEYWORD_NOEXCEPT;

    /// Set the buffer to describe the specified 'data' and 'length'.
    void setBuffer(void* data, bsl::size_t length) BSLS_KEYWORD_NOEXCEPT;

    /// Increment the address of the data of the buffer by the specified 'n'
    /// number of bytes and decrement its size by the equivalent amount.
    void advance(bsl::size_t n) BSLS_KEYWORD_NOEXCEPT;

    /// Return the pointer to the beginning of the memory range.
    void* data() const BSLS_KEYWORD_NOEXCEPT;

    /// Return the size of the memory range.
    bsl::size_t size() const BSLS_KEYWORD_NOEXCEPT;

    /// Return the pointer to the beginning of the memory range.
    void* buffer() const BSLS_KEYWORD_NOEXCEPT;

    /// Return the size of the memory range.
    bsl::size_t length() const BSLS_KEYWORD_NOEXCEPT;

    /// Validate the integrity of the buffer and its structure.
    bool validate() const;

    /// Return the total number of bytes referenced in the specified
    /// 'bufferArray' having the specified 'bufferCount'.
    static bsl::size_t totalSize(const ntsa::MutableBuffer* bufferArray,
                                 bsl::size_t                bufferCount);

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

    /// Append to the specified 'blob' a copy of the data referenced by the
    /// specified 'bufferArray' having the specified 'bufferCount' number of
    /// buffer starting at the specified logical 'offset', in bytes, in the
    /// logical seqeuence of bytes referenced by the buffers in 'bufferArray'.
    static void copy(bdlbb::Blob*               blob,
                     const ntsa::MutableBuffer* bufferArray,
                     bsl::size_t                bufferCount,
                     bsl::size_t                offset);

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    BSLALG_DECLARE_NESTED_TRAITS(MutableBuffer,
                                 bslalg::TypeTraitBitwiseCopyable);
};

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::MutableBuffer
bool operator==(const ntsa::MutableBuffer& lhs,
                const ntsa::MutableBuffer& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::MutableBuffer
bool operator!=(const ntsa::MutableBuffer& lhs,
                const ntsa::MutableBuffer& rhs);

/// Provide a non-modifiable buffer.
///
/// @details
/// These buffers and sequences are used to reference data that is intended to
/// be copied into and out of socket send and receive buffers. The
/// 'ntsa::MutableBuffer' and 'ntsa::ConstBuffer' are exact replicas of the
/// 'iovec' structure, on POSIX systems, and the 'WSABUF' structure, on
/// Windows. Users may 'reinterpret_cast' pointers between these types.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class ConstBuffer
{
#if defined(BSLS_PLATFORM_OS_UNIX)
    typedef const void* BufferType;
    typedef bsl::size_t LengthType;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    typedef unsigned long LengthType;
    typedef const void*   BufferType;
#else
#error Not implemented
#endif

#if defined(BSLS_PLATFORM_OS_UNIX)
    BufferType d_data;
    LengthType d_size;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    LengthType            d_size;
    BufferType            d_data;
#else
#error Not implemented
#endif

  public:
    /// Create a new, empty buffer.
    ConstBuffer() BSLS_KEYWORD_NOEXCEPT;

    /// Create a new buffer describing the specified 'blobBuffer'.
    explicit ConstBuffer(const bdlbb::BlobBuffer& blobBuffer)
        BSLS_KEYWORD_NOEXCEPT;

    /// Create a new buffer describing the specified 'data' having the
    /// specified 'size'.
    ConstBuffer(const void* data, bsl::size_t size) BSLS_KEYWORD_NOEXCEPT;

    /// Create a new buffer of non-modifiable data from the specified
    /// 'other' buffer of modifiable data.
    ConstBuffer(const MutableBuffer& other) BSLS_KEYWORD_NOEXCEPT;

    /// Create a new buffer having the same value as the specified
    /// 'original' object.
    ConstBuffer(const ConstBuffer& original) BSLS_KEYWORD_NOEXCEPT;

    /// Destroy this object.
    ~ConstBuffer() BSLS_KEYWORD_NOEXCEPT;

    /// Assign to this object the value of the specified 'other' object, and
    /// return a reference to this modifiable object.
    ConstBuffer& operator=(const ConstBuffer& other) BSLS_KEYWORD_NOEXCEPT;

    /// Assign to this object the value of the specified 'other' object, and
    /// return a reference to this modifiable object.
    ConstBuffer& operator=(const MutableBuffer& other) BSLS_KEYWORD_NOEXCEPT;

    /// Increment the address of the data of the buffer by the specified 'n'
    /// number of bytes and decrement its size by the equivalent amount.
    ConstBuffer& operator+=(bsl::size_t n) BSLS_KEYWORD_NOEXCEPT;

    /// Set the buffer to describe the specified 'blobBuffer'.
    void setBuffer(const bdlbb::BlobBuffer& blobBuffer) BSLS_KEYWORD_NOEXCEPT;

    /// Set the buffer to describe the specified 'data' and 'length'.
    void setBuffer(const void* buffer,
                   bsl::size_t length) BSLS_KEYWORD_NOEXCEPT;

    /// Increment the address of the data of the buffer by the specified 'n'
    /// number of bytes and decrement its size by the equivalent amount.
    void advance(bsl::size_t n) BSLS_KEYWORD_NOEXCEPT;

    /// Return the pointer to the beginning of the memory range.
    const void* data() const BSLS_KEYWORD_NOEXCEPT;

    /// Return the size of the memory range.
    bsl::size_t size() const BSLS_KEYWORD_NOEXCEPT;

    /// Return the pointer to the beginning of the memory range.
    const void* buffer() const BSLS_KEYWORD_NOEXCEPT;

    /// Return the size of the memory range.
    bsl::size_t length() const BSLS_KEYWORD_NOEXCEPT;

    /// Validate the integrity of the buffer and its structure.
    bool validate() const;

    /// Return the total number of bytes referenced in the specified
    /// 'bufferArray' having the specified 'bufferCount'.
    static bsl::size_t totalSize(const ntsa::ConstBuffer* bufferArray,
                                 bsl::size_t              bufferCount);

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

    /// Append to the specified 'blob' a copy of the data referenced by the
    /// specified 'bufferArray' having the specified 'bufferCount' number of
    /// buffer starting at the specified logical 'offset', in bytes, in the
    /// logical seqeuence of bytes referenced by the buffers in 'bufferArray'.
    static void copy(bdlbb::Blob*             blob,
                     const ntsa::ConstBuffer* bufferArray,
                     bsl::size_t              bufferCount,
                     bsl::size_t              offset);

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    BSLALG_DECLARE_NESTED_TRAITS(ConstBuffer,
                                 bslalg::TypeTraitBitwiseCopyable);
};

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::ConstBuffer
bool operator==(const ntsa::ConstBuffer& lhs, const ntsa::ConstBuffer& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::ConstBuffer
bool operator!=(const ntsa::ConstBuffer& lhs, const ntsa::ConstBuffer& rhs);

/// Provide an array of contiguous pointers to potentially discontiguous,
/// non-modifiable data.
///
/// @details
/// These buffers and sequences are used to reference data that is intended to
/// be copied into and out of socket send and receive buffers. The
/// 'ntsa::MutableBuffer' and 'ntsa::ConstBuffer' are exact replicas of the
/// 'iovec' structure, on POSIX systems, and the 'WSABUF' structure, on
/// Windows. Users may 'reinterpret_cast' pointers between these types.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class ConstBufferArray
{
    enum {
        NUM_LOCALLY_STORED_BUFFERS = NTSCFG_DEFAULT_MAX_INPLACE_BUFFERS,
        // The number of buffers to store in the local arena

        BUFFER_ARRAY_ARENA_SIZE =
            sizeof(ntsa::ConstBuffer) * NUM_LOCALLY_STORED_BUFFERS
        // The size of the local arena, in bytes.
    };

    bdlma::LocalSequentialAllocator<BUFFER_ARRAY_ARENA_SIZE>
                                   d_bufferArrayAllocator;
    bsl::vector<ntsa::ConstBuffer> d_bufferArray;
    bsl::size_t                    d_numBytes;

  public:
    /// Create a new, empty array of buffers pointing to non-modifable
    /// data. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit ConstBufferArray(bslma::Allocator* basicAllocator = 0);

    /// Create a new array of buffers having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    ConstBufferArray(const ConstBufferArray& original,
                     bslma::Allocator*       basicAllocator = 0);

    /// Destroy this object.
    ~ConstBufferArray();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ConstBufferArray& operator=(const ConstBufferArray& other);

    /// Clear the array.
    void clear();

    /// Reserve memory to contiguously store at least the specified
    /// 'numBuffers'.
    void reserve(bsl::size_t numBuffers);

    /// Resize the buffer array so that it stores at least the specified
    /// 'numBuffers'.
    void resize(bsl::size_t numBuffers);

    /// Append a new buffer referencing the specified 'data' having the
    /// specified 'size' to this buffer array.
    void append(const void* data, bsl::size_t size);

    /// Append the specified 'buffer' to this buffer array.
    void append(const ntsa::ConstBuffer& buffer);

    /// Append the specified 'bufferList' having the specified 'numBuffers'
    /// to this buffer array.
    void append(const ntsa::ConstBuffer* bufferList, bsl::size_t numBuffers);

    /// Pop the specified 'numBytes' of referenced data by buffers from the
    /// front of the array.
    void pop(bsl::size_t numBytes);

    /// Return a reference to the modifiable buffer at the specified
    /// 'index'.
    ntsa::ConstBuffer& buffer(bsl::size_t index);

    /// Return the starting address of the array of contiguous buffers,
    /// each pointing to potentially non-contiguous memory. This address
    /// is suitable for reinterpreting as an array of the native system
    /// scatter/gather I/O buffer type (e.g. 'struct iovec' on POSIX
    /// systems), to supply to a native scatter/gather I/O system call
    /// (e.g. 'readv', or 'writev' on POSIX systems.) This function is a
    /// synonym for '&buffer(0)'.
    const void* base() const;

    /// Return a reference to the non-modifiable buffer at the specified
    /// 'index'.
    const ntsa::ConstBuffer& buffer(bsl::size_t index) const;

    /// Return the number of buffers in the array.
    bsl::size_t numBuffers() const;

    /// Return the total number of bytes referenced by all buffers in the
    /// array.
    bsl::size_t numBytes() const;

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

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    BSLALG_DECLARE_NESTED_TRAITS(ConstBufferArray,
                                 bslalg::TypeTraitUsesBslmaAllocator);
};

/// Provide a held pointer to an array of contiguous pointers to potentially
/// discontiguous, non-modifiable data.
///
/// @details
/// These buffers and sequences are used to reference data that is intended to
/// be copied into and out of socket send and receive buffers. The
/// 'ntsa::MutableBuffer' and 'ntsa::ConstBuffer' are exact replicas of the
/// 'iovec' structure, on POSIX systems, and the 'WSABUF' structure, on
/// Windows. Users may 'reinterpret_cast' pointers between these types.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class ConstBufferPtrArray
{
    ntsa::ConstBuffer* d_bufferArray;
    bsl::size_t        d_numBuffers;
    bsl::size_t        d_numBytes;

  public:
    /// Create a new, empty buffer pointer array.
    ConstBufferPtrArray();

    /// Create a new buffer pointer array to the specified 'numBuffers'
    /// starting at the specified 'bufferList'.
    ConstBufferPtrArray(ntsa::ConstBuffer* bufferList, bsl::size_t numBuffers);

    /// Create a new buffer pointer array having the same value as the
    /// specified 'original' object.
    ConstBufferPtrArray(const ConstBufferPtrArray& original);

    /// Destroy this object.
    ~ConstBufferPtrArray();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ConstBufferPtrArray& operator=(const ConstBufferPtrArray& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Assign this object to represent a pointer to an array of the
    /// specified 'numBuffers' starting at the specified 'bufferList'.
    void assign(ntsa::ConstBuffer* bufferList, bsl::size_t numBuffers);

    /// Pop the specified 'numBytes' of referenced data by buffers from the
    /// front of the array.
    void pop(bsl::size_t numBytes);

    /// Return a reference to the modifiable buffer at the specified
    /// 'index'.
    ntsa::ConstBuffer& buffer(bsl::size_t index);

    /// Return the starting address of the array of contiguous buffers,
    /// each pointing to potentially non-contiguous memory. This address
    /// is suitable for reinterpreting as an array of the native system
    /// scatter/gather I/O buffer type (e.g. 'struct iovec' on POSIX
    /// systems), to supply to a native scatter/gather I/O system call
    /// (e.g. 'readv', or 'writev' on POSIX systems.) This function is a
    /// synonym for '&buffer(0)'.
    const void* base() const;

    /// Return a reference to the non-modifiable buffer at the specified
    /// 'index'.
    const ntsa::ConstBuffer& buffer(bsl::size_t index) const;

    /// Return the number of buffers in the array.
    bsl::size_t numBuffers() const;

    /// Return the total number of bytes referenced by all buffers in the
    /// array.
    bsl::size_t numBytes() const;

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    BSLALG_DECLARE_NESTED_TRAITS(ConstBufferPtrArray,
                                 bslalg::TypeTraitBitwiseCopyable);
};

/// Provide an array of contiguous pointers to potentially discontiguous,
/// modifiable data.
///
/// @details
/// These buffers and sequences are used to reference data that is intended to
/// be copied into and out of socket send and receive buffers. The
/// 'ntsa::MutableBuffer' and 'ntsa::ConstBuffer' are exact replicas of the
/// 'iovec' structure, on POSIX systems, and the 'WSABUF' structure, on
/// Windows. Users may 'reinterpret_cast' pointers between these types.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class MutableBufferArray
{
    enum {
        NUM_LOCALLY_STORED_BUFFERS = NTSCFG_DEFAULT_MAX_INPLACE_BUFFERS,
        // The number of buffers to store in the local arena

        BUFFER_ARRAY_ARENA_SIZE =
            sizeof(ntsa::ConstBuffer) * NUM_LOCALLY_STORED_BUFFERS
        // The size of the local arena, in bytes.
    };

    bdlma::LocalSequentialAllocator<BUFFER_ARRAY_ARENA_SIZE>
                                     d_bufferArrayAllocator;
    bsl::vector<ntsa::MutableBuffer> d_bufferArray;
    bsl::size_t                      d_numBytes;

  public:
    /// Create a new, empty array of buffers pointing to non-modifable
    /// data. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit MutableBufferArray(bslma::Allocator* basicAllocator = 0);

    /// Create a new buffer array having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    MutableBufferArray(const MutableBufferArray& original,
                       bslma::Allocator*         basicAllocator = 0);

    /// Destroy this object.
    ~MutableBufferArray();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    MutableBufferArray& operator=(const MutableBufferArray& other);

    /// Clear the array.
    void clear();

    /// Reserve memory to contiguously store at least the specified
    /// 'numBuffers'.
    void reserve(bsl::size_t numBuffers);

    /// Resize the buffer array so that it stores at least the specified
    /// 'numBuffers'.
    void resize(bsl::size_t numBuffers);

    /// Append a new buffer referencing the specified 'data' having the
    /// specified 'size' to this buffer array.
    void append(void* data, bsl::size_t size);

    /// Append the specified 'buffer' to this buffer array.
    void append(const ntsa::MutableBuffer& buffer);

    /// Append the specified 'bufferList' having the specified 'numBuffers'
    /// to this buffer array.
    void append(ntsa::MutableBuffer* bufferList, bsl::size_t numBuffers);

    /// Pop the specified 'numBytes' of referenced data by buffers from the
    /// front of the array.
    void pop(bsl::size_t numBytes);

    /// Return a reference to the modifiable buffer at the specified
    /// 'index'.
    ntsa::MutableBuffer& buffer(bsl::size_t index);

    /// Return the starting address of the array of contiguous buffers,
    /// each pointing to potentially non-contiguous memory. This address
    /// is suitable for reinterpreting as an array of the native system
    /// scatter/gather I/O buffer type (e.g. 'struct iovec' on POSIX
    /// systems), to supply to a native scatter/gather I/O system call
    /// (e.g. 'readv', or 'writev' on POSIX systems.) This function is a
    /// synonym for '&buffer(0)'.
    const void* base() const;

    /// Return a reference to the non-modifiable buffer at the specified
    /// 'index'.
    const ntsa::MutableBuffer& buffer(bsl::size_t index) const;

    /// Return the number of buffers in the array.
    bsl::size_t numBuffers() const;

    /// Return the total number of bytes referenced by all buffers in the
    /// array.
    bsl::size_t numBytes() const;

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

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    BSLALG_DECLARE_NESTED_TRAITS(MutableBufferArray,
                                 bslalg::TypeTraitUsesBslmaAllocator);
};

/// Provide a held pointer to an array of contiguous pointers to potentially
/// discontiguous, modifiable data.
///
/// @details
/// These buffers and sequences are used to reference data that is intended to
/// be copied into and out of socket send and receive buffers. The
/// 'ntsa::MutableBuffer' and 'ntsa::ConstBuffer' are exact replicas of the
/// 'iovec' structure, on POSIX systems, and the 'WSABUF' structure, on
/// Windows. Users may 'reinterpret_cast' pointers between these types.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class MutableBufferPtrArray
{
    ntsa::MutableBuffer* d_bufferArray;
    bsl::size_t          d_numBuffers;
    bsl::size_t          d_numBytes;

  public:
    /// Create a new, empty buffer pointer array.
    MutableBufferPtrArray();

    /// Create a new buffer pointer array to the specified 'numBuffers'
    /// starting at the specified 'bufferList'.
    MutableBufferPtrArray(ntsa::MutableBuffer* bufferList,
                          bsl::size_t          numBuffers);

    /// Create a new buffer pointer array having the same value as the
    /// specified 'original' object.
    MutableBufferPtrArray(const MutableBufferPtrArray& original);

    /// Destroy this object.
    ~MutableBufferPtrArray();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    MutableBufferPtrArray& operator=(const MutableBufferPtrArray& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Assign this object to represent a pointer to an array of the
    /// specified 'numBuffers' starting at the specified 'bufferList'.
    void assign(ntsa::MutableBuffer* bufferList, bsl::size_t numBuffers);

    /// Pop the specified 'numBytes' of referenced data by buffers from the
    /// front of the array.
    void pop(bsl::size_t numBytes);

    /// Return a reference to the modifiable buffer at the specified
    /// 'index'.
    ntsa::MutableBuffer& buffer(bsl::size_t index);

    /// Return the starting address of the array of contiguous buffers,
    /// each pointing to potentially non-contiguous memory. This address
    /// is suitable for reinterpreting as an array of the native system
    /// scatter/gather I/O buffer type (e.g. 'struct iovec' on POSIX
    /// systems), to supply to a native scatter/gather I/O system call
    /// (e.g. 'readv', or 'writev' on POSIX systems.) This function is a
    /// synonym for '&buffer(0)'.
    const void* base() const;

    /// Return a reference to the non-modifiable buffer at the specified
    /// 'index'.
    const ntsa::MutableBuffer& buffer(bsl::size_t index) const;

    /// Return the number of buffers in the array.
    bsl::size_t numBuffers() const;

    /// Return the total number of bytes referenced by all buffers in the
    /// array.
    bsl::size_t numBytes() const;

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    BSLALG_DECLARE_NESTED_TRAITS(MutableBufferPtrArray,
                                 bslalg::TypeTraitBitwiseCopyable);
};

/// Provide a sequence of non-modifiable buffers.
///
/// These buffers and sequences are used to reference data that is intended to
/// be copied into and out of socket send and receive buffers. The
/// 'ntsa::MutableBuffer' and 'ntsa::ConstBuffer' are exact replicas of the
/// 'iovec' structure, on POSIX systems, and the 'WSABUF' structure, on
/// Windows. Users may 'reinterpret_cast' pointers between these types.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
template <typename CONST_BUFFER>
class ConstBufferSequence
{
    const bdlbb::Blob* d_blob_p;

  public:
    /// Provide a forward iterator over a sequence of non-modifiable buffers.
    class Iterator;

    /// Create a new non-modifiable buffer sequence for the specified 'blob'.
    explicit ConstBufferSequence(const bdlbb::Blob* blob)
        BSLS_KEYWORD_NOEXCEPT;

    /// Return the iterator to the beginning of the non-modifiable buffer
    /// sequence.
    Iterator begin() const BSLS_KEYWORD_NOEXCEPT;

    /// Return the iterator to the end of the non-modifiable buffer sequence.
    Iterator end() const BSLS_KEYWORD_NOEXCEPT;
};

/// Provide a forward iterator over a sequence of non-modifiable buffers.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par Usage Example: Iterating over the readable data of a blob
/// This example illustrates how to use 'ntsa::ConstBufferSequence' to
/// iterate over the contiguous regions of readable data in virtual address
/// space that are referenced by a blob, with each range described by a
/// 'ntsa::ConstBuffer'.
///
///     bdlbb::PooledBlobBufferFactory blobBufferFactory(8);
///     bdlbb::Blob blob(&blobBufferFactory);
///
///     bdlbb::BlobUtil::append(&blob, "Hello, world!", 0, 13);
///
///     BSLS_ASSERT(blob.numBuffers() == 2);
//
///     const bdlbb::BlobBuffer& blobBuffer1 = blob.buffer(0);
///     const bdlbb::BlobBuffer& blobBuffer2 = blob.buffer(1);
///
///     ntsa::ConstBufferSequence<ntsa::ConstBuffer> constBufferSequence(&blob);
///
///     ntsa::ConstBufferSequence<ntsa::ConstBuffer>::Iterator it;
///
///     it = constBufferSequence.begin();
///     BSLS_ASSERT(it != constBufferSequence.end());
///
///     {
///         ntsa::ConstBuffer constBuffer = *it;
///         BSLS_ASSERT(constBuffer.data() == blobBuffer1.data());
///         BSLS_ASSERT(constBuffer.size() == 8);
///     }
///
///     ++it;
///     BSLS_ASSERT(it != constBufferSequence.end());
///
///     {
///         ntsa::ConstBuffer constBuffer = *it;
///         BSLS_ASSERT(constBuffer.data() == blobBuffer2.data());
///         BSLS_ASSERT(constBuffer.size() == 5);
///     }
///
///     ++it;
///     BSLS_ASSERT(it == constBufferSequence.end());
///
/// @ingroup module_ntsa_data
template <typename CONST_BUFFER>
class ConstBufferSequence<CONST_BUFFER>::Iterator
{
  public:
    /// Define a type alias for the iterator category to which
    /// this iterator belongs.
    typedef bsl::forward_iterator_tag iterator_category;

    /// Define a type alias for the type of the result of
    /// dereferencing this iterator.
    typedef CONST_BUFFER value_type;

    /// Define a type alias for the integral type capable of
    /// representing the difference between two addresses.
    typedef bsl::ptrdiff_t difference_type;

    /// Define a type alias for the type that is a pointer to
    /// type that is the result of dererencing this iterator.
    typedef value_type* pointer;

    /// Define a type alias for the type that is a reference
    /// to the type that is the result of dereferencing this iterator.
    typedef value_type& reference;

  private:
    const bdlbb::Blob* d_blob_p;
    bsl::size_t        d_index;
    value_type         d_data;

  public:
    /// Create an invalid iterator.
    Iterator() BSLS_KEYWORD_NOEXCEPT;

    /// Create an iterator to a buffer in the specified 'blob' at the
    /// specified 'index' that has the specified 'data' and specified
    /// 'size'.
    Iterator(const bdlbb::Blob* blob,
             bsl::size_t        index,
             const char*        data,
             bsl::size_t        size) BSLS_KEYWORD_NOEXCEPT;

    /// Increment this iterator to alias the next buffer in the blob and
    /// return this iterator.
    Iterator& operator++() BSLS_KEYWORD_NOEXCEPT;

    /// Increment this iterator to alias the next buffer in the blob and
    /// return and iterator that aliases the byte this iterator alias before
    /// it was incremented.
    Iterator operator++(int) BSLS_KEYWORD_NOEXCEPT;

    /// Return a reference to the non-modifiable buffer aliased by this
    /// iterator.
    const value_type& operator*() const BSLS_KEYWORD_NOEXCEPT;

    /// Return true if this iterator aliases the same non-modifiable buffer
    /// as the specified 'other' iterator, otherwise return false.
    bool operator==(const Iterator& other) const BSLS_KEYWORD_NOEXCEPT;

    /// Return true if this iterator does not alias the same non-modifiable
    /// buffer as the specified 'other' iterator, otherwise return false.
    bool operator!=(const Iterator& other) const BSLS_KEYWORD_NOEXCEPT;
};

/// Provide a sequence of modifiable buffers.
///
/// These buffers and sequences are used to reference data that is intended to
/// be copied into and out of socket send and receive buffers. The
/// 'ntsa::MutableBuffer' and 'ntsa::ConstBuffer' are exact replicas of the
/// 'iovec' structure, on POSIX systems, and the 'WSABUF' structure, on
/// Windows. Users may 'reinterpret_cast' pointers between these types.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par Usage Example: Iterating over the writable capacity of a blob
/// This example illustrates how to use 'ntsa::MutableBufferSequence' to
/// iterate over the contiguous regions of writable data in virtual address
/// space that are referenced by a blob, with each range described by a
/// 'ntsa::MutableBuffer'.
///
///     bdlbb::PooledBlobBufferFactory blobBufferFactory(8);
///     bdlbb::Blob blob(&blobBufferFactory);
///
///     bdlbb::BlobUtil::append(&blob, "Hello, world!", 0, 13);
///
///     BSLS_ASSERT(blob.numBuffers() == 2);
///
///     const bdlbb::BlobBuffer& blobBuffer1 = blob.buffer(0);
///     const bdlbb::BlobBuffer& blobBuffer2 = blob.buffer(1);
///
///     ntsa::MutableBufferSequence<ntsa::MutableBuffer> mutableBufferSequence(
///         &blob);
//
///     ntsa::MutableBufferSequence<ntsa::MutableBuffer>::Iterator it;
///
///     it = mutableBufferSequence.begin();
///     BSLS_ASSERT(it != mutableBufferSequence.end());
///
///     {
///         ntsa::MutableBuffer mutableBuffer = *it;
///         BSLS_ASSERT(mutableBuffer.data() == blobBuffer2.data() + 5);
///         BSLS_ASSERT(mutableBuffer.size() == 3);
///     }
///
///     ++it;
///     BSLS_ASSERT(it == mutableBufferSequence.end());
///
/// @ingroup module_ntsa_data
template <typename MUTABLE_BUFFER>
class MutableBufferSequence
{
    bdlbb::Blob* d_blob_p;

  public:
    /// Provide a forward iterator over a sequence of modifiable buffers.
    class Iterator;

    /// Create a new non-modifiable buffer sequence for the specified
    /// 'blob'.
    explicit MutableBufferSequence(bdlbb::Blob* blob) BSLS_KEYWORD_NOEXCEPT;

    /// Return the iterator to the beginning of the modifiable buffer
    /// sequence.
    Iterator begin() const BSLS_KEYWORD_NOEXCEPT;

    /// Return the iterator to the end of the modifiable buffer sequence.
    Iterator end() const BSLS_KEYWORD_NOEXCEPT;
};

/// Provide a forward iterator over a sequence of modifiable buffers.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
template <typename MUTABLE_BUFFER>
class MutableBufferSequence<MUTABLE_BUFFER>::Iterator
{
  public:
    /// Define a type alias for the iterator category to which
    /// this iterator belongs.
    typedef bsl::forward_iterator_tag iterator_category;

    /// Define a type alias for the type of the result of
    /// dereferencing this iterator.
    typedef MUTABLE_BUFFER value_type;

    /// Define a type alias for the integral type capable of
    /// representing the difference between two addresses.
    typedef bsl::ptrdiff_t difference_type;

    /// Define a type alias for the type that is a pointer to
    /// type that is the result of dererencing this iterator.
    typedef value_type* pointer;

    /// Define a type alias for the type that is a reference
    /// to the type that is the result of dereferencing this iterator.
    typedef value_type& reference;

  private:
    bdlbb::Blob* d_blob_p;
    bsl::size_t  d_index;
    value_type   d_data;

  public:
    /// Create an invalid iterator.
    Iterator() BSLS_KEYWORD_NOEXCEPT;

    /// Create an iterator to a buffer in the specified 'blob' at the
    /// specified 'index' that has the specified 'data' and specified
    /// 'size'.
    Iterator(bdlbb::Blob* blob,
             bsl::size_t  index,
             char*        data,
             bsl::size_t  size) BSLS_KEYWORD_NOEXCEPT;

    /// Increment this iterator to alias the next buffer in the blob and
    /// return this iterator.
    Iterator& operator++() BSLS_KEYWORD_NOEXCEPT;

    /// Increment this iterator to alias the next buffer in the blob and
    /// return and iterator that aliases the byte this iterator alias before
    /// it was incremented.
    Iterator operator++(int) BSLS_KEYWORD_NOEXCEPT;

    /// Return a reference to the non-modifiable buffer aliased by this
    /// iterator.
    const value_type& operator*() const BSLS_KEYWORD_NOEXCEPT;

    /// Return true if this iterator aliases the same non-modifiable buffer
    /// as the specified 'other' iterator, otherwise return false.
    bool operator==(const Iterator& other) const BSLS_KEYWORD_NOEXCEPT;

    /// Return true if this iterator does not alias the same non-modifiable
    /// buffer as the specified 'other' iterator, otherwise return false.
    bool operator!=(const Iterator& other) const BSLS_KEYWORD_NOEXCEPT;
};

/// Provide a memory stream buffer using memory optionally supplied by the
/// client and supplemented by an allocator.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class StreamBuffer : public bsl::streambuf
{
    /// Enumerate the access policy of this stream buffer.
    enum Access {
        /// The buffer is readable but not writable.
        e_READONLY,

        /// The buffer is both readable and writable.
        e_WRITABLE
    };

    /// Enumerate the ownership semantics of the underlying memory.
    enum Ownership {
        /// The underlying memory has been allocated during the internal
        /// operation of the object and must be freed.
        e_INTERNAL,

        /// The underlying memory has been externally injected into the object
        /// and must not be freed.
        e_EXTERNAL
    };

    /// Define constants that influence the behavior of the implementation.
    enum Constants {
        /// The minimum number of bytes to allocate when the buffer needs to
        /// grow.
        e_MIN_CAPACITY = 256
    };

    Access            d_access;
    Ownership         d_ownership;
    bslma::Allocator* d_allocator_p;

  private:
    StreamBuffer(const StreamBuffer&) BSLS_KEYWORD_DELETED;
    StreamBuffer& operator=(const StreamBuffer&) BSLS_KEYWORD_DELETED;

    /// Return the beginning of the readable byte sequence.
    const char* getReaderBegin() const;

    /// Return the current position in the readable byte sequence, i.e. the
    /// next byte that will be read.
    const char* getReaderCurrent() const;

    /// Return the end of the readable byte sequence.
    const char* getReaderEnd() const;

    /// Return the distance between the current position in the readable
    /// byte sequence and the beginning of the readable byte sequence, i.e.,
    /// the number of bytes already read.
    bsl::size_t getReaderOffset() const;

    /// Return the distance between the end of the readable byte sequence and
    /// the current position in the readable byte sequence, i.e., the number
    /// of bytes that may be still be read.
    bsl::size_t getReaderAvailable() const;

    /// Return the beginning of the writable byte sequence.
    char* getWriterBegin() const;

    /// Return the current position in the writable byte sequence, i.e. the
    /// next byte that will be written.
    char* getWriterCurrent() const;

    /// Return the end of the writable byte sequence.
    char* getWriterEnd() const;

    /// Return the distance between the current position in the writable byte
    /// sequence and the beginning of the writable byte sequence, i.e. the
    /// number of bytes already written.
    bsl::size_t getWriterOffset() const;

    /// Return the distance between the end of the writable byte sequence and
    /// the current position in the writable byte sequence, i.e., the number
    /// of bytes that may still be written.
    bsl::size_t getWriterAvailable() const;

    /// Return the currently allocated memory, or null if no such memory is
    /// allocated.
    void* getMemory() const;

    /// Return the number of bytes written to the currently allocated memory.
    bsl::size_t getMemorySize() const;

    /// Return the number of bytes writable to the currently allocated memory.
    bsl::size_t getMemoryCapacity() const;

    /// Increment the current position in the readable byte sequence by the
    /// specified 'size'.
    void incrementReaderPosition(bsl::size_t size);

    /// Increment the current position in the writable byte sequence by the
    /// specified 'size'.
    void incrementWriterPosition(bsl::size_t size);

    /// Initialize the readable and writable byte sequences to empty.
    void initialize();

    /// Initialize the readable byte sequence to the specified 'data' having
    /// the specified 'size' and initialize the writable byte sequence to
    /// empty. Note that the readable position is set to 'data'.
    void initialize(const void* data, bsl::size_t size);

    /// Initialize the readable byte sequence to the specified 'data' having
    /// the specified 'size', and initialize the writable byte sequence to
    /// the specified 'data' having the specified 'capacity'. Note both the
    /// readable and writable positions are set to 'data'.
    void initialize(void* data, bsl::size_t size, bsl::size_t capacity);

    /// Allocate sufficient memory to write the number of specified 'overflow'
    /// bytes, and re-home both the readable and writable byte sequences.
    void expand(bsl::size_t overflow);

    /// Store at most the specified 'size' number of bytes from specified
    /// 'source' starting at current position in the writable byte sequence and
    /// advance the current position in the writable byte sequence by the
    /// number of bytes written. Return the number of bytes written.
    bsl::size_t store(const char* source, bsl::size_t size);

    /// Load at most the specified 'size' number of bytes starting at the
    /// current position in the readable byte sequence into the specified
    /// 'destination', and advance the current position in the readable byte
    /// sequence by the number of bytes read. Return the number of bytes read.
    bsl::size_t fetch(char* destination, bsl::size_t size);

  protected:
    /// Insert the optionally specified 'meta' element into the (potentially)
    /// full stream buffer. On success, return 'traits_type::not_eof(meta)'.
    /// Otherwise, return 'traits_type::eof()' or throw an exception. The
    /// default implementation returns 'traits_type::eof()'.
    int_type overflow(int_type meta) BSLS_KEYWORD_OVERRIDE;

    /// Return the number of characters that can be extracted from the input
    /// stream such that the program will not be subject to an indefinite
    /// wait.
    bsl::streamsize showmanyc() BSLS_KEYWORD_OVERRIDE;

    /// Read the specified 'size' number of characters into the specified
    /// 'destination'.  Return the number of characters successfully read.
    /// The behavior is undefined unless '0 <= length'.
    bsl::streamsize xsgetn(char_type*      destination,
                           bsl::streamsize size) BSLS_KEYWORD_OVERRIDE;

    /// Write the specified 'size' characters from the specified
    /// 'source' to the stream buffer.  Return the number of characters
    /// successfully written.  The behavior is undefined unless '(source &&
    /// 0 < numChars) || 0 == numChars'.
    bsl::streamsize xsputn(const char_type* source,
                           bsl::streamsize  size) BSLS_KEYWORD_OVERRIDE;

    /// Alter the get and/or put positions specified by the 'mode' flags by the
    /// specified 'offset' according to the specified 'way'. Return the new
    /// stream position (or one of the stream positions) on success or an
    /// invalid stream position otherwise.
    pos_type seekoff(off_type                offset,
                     bsl::ios_base::seekdir  way,
                     bsl::ios_base::openmode mode) BSLS_KEYWORD_OVERRIDE;

    /// Set the absolute get and/or put positions specified by the 'mode' flags
    /// to the specified 'position'. Return the new stream position (or one of
    /// the stream positions) on success or an invalid stream position
    /// otherwise.
    pos_type seekpos(pos_type                position,
                     bsl::ios_base::openmode mode) BSLS_KEYWORD_OVERRIDE;

    /// Synchronize the stream buffer with the underlying device. Return 0 on
    /// success or -1 otherwise.
    int sync() BSLS_KEYWORD_OVERRIDE;

  public:
    /// Create a new, read/writable memory stream buffer that is initially
    /// empty. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit StreamBuffer(bslma::Allocator* basicAllocator = 0);

    /// Create a new, read-only memory buffer of the specified 'data' having
    /// the specified 'size'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    StreamBuffer(const void*       data,
                 bsl::size_t       size,
                 bslma::Allocator* basicAllocator = 0);

    /// Construct a new memory stream buffer initially backed by the specified
    /// 'data' of the specified 'capacity' where the first 'size' bytes
    /// represent valid, existing data. That is, 'size' indicates the initial
    /// size of the get area, 'data + size' marks the initial put position, and
    /// 'capacity' marks the maximum size of both the get and the put areas.
    /// The initial get position is always indicated by 'data' (i.e. the start
    /// of the get area.) Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    StreamBuffer(void*             data,
                 bsl::size_t       size,
                 bsl::size_t       capacity,
                 bslma::Allocator* allocator = 0);

    /// Destroy this object.
    ~StreamBuffer();

    /// Free all memory allocated by the stream buffer.
    void reset();

    /// Reserve at least the specified 'capacity' number of writable bytes.
    void reserve(bsl::size_t capacity);

    /// Return the address of the underlying memory.
    char* data();

    /// Return the address of the underlying memory.
    const char* data() const;

    /// Return the number of readable bytes.
    bsl::size_t size() const;

    /// Return the number of writable bytes.
    bsl::size_t capacity() const;
};

NTSCFG_INLINE
MutableBuffer::MutableBuffer() BSLS_KEYWORD_NOEXCEPT
#if defined(BSLS_PLATFORM_OS_UNIX)
: d_data(0),
  d_size(0)
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
: d_size(0),
  d_data(0)
#else
#error Not implemented
#endif
{
}

NTSCFG_INLINE MutableBuffer::MutableBuffer(const bdlbb::BlobBuffer& blobBuffer)
    BSLS_KEYWORD_NOEXCEPT
#if defined(BSLS_PLATFORM_OS_UNIX)
: d_data((BufferType)(blobBuffer.data())),
  d_size((LengthType)(blobBuffer.size()))
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
: d_size((LengthType)(blobBuffer.size())),
  d_data((BufferType)(blobBuffer.data()))
#else
#error Not implemented
#endif
{
}

NTSCFG_INLINE
MutableBuffer::MutableBuffer(void*       data,
                             bsl::size_t size) BSLS_KEYWORD_NOEXCEPT
#if defined(BSLS_PLATFORM_OS_UNIX)
: d_data((BufferType)(data)),
  d_size((LengthType)(size))
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
: d_size((LengthType)(size)),
  d_data((BufferType)(data))
#else
#error Not implemented
#endif
{
}

NTSCFG_INLINE
MutableBuffer::MutableBuffer(const MutableBuffer& original)
    BSLS_KEYWORD_NOEXCEPT
#if defined(BSLS_PLATFORM_OS_UNIX)
: d_data(original.d_data),
  d_size(original.d_size)
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
: d_size(original.d_size),
  d_data(original.d_data)
#else
#error Not implemented
#endif
{
}

NTSCFG_INLINE
MutableBuffer::~MutableBuffer() BSLS_KEYWORD_NOEXCEPT
{
}

NTSCFG_INLINE
MutableBuffer& MutableBuffer::operator=(const MutableBuffer& other)
    BSLS_KEYWORD_NOEXCEPT
{
    d_data = other.d_data;
    d_size = other.d_size;
    return *this;
}

NTSCFG_INLINE
MutableBuffer& MutableBuffer::operator+=(bsl::size_t n) BSLS_KEYWORD_NOEXCEPT
{
    this->advance(n);
    return *this;
}

NTSCFG_INLINE
void MutableBuffer::setBuffer(const bdlbb::BlobBuffer& blobBuffer)
    BSLS_KEYWORD_NOEXCEPT
{
    d_data = (BufferType)(blobBuffer.data());
    d_size = (LengthType)(blobBuffer.size());
}

NTSCFG_INLINE
void MutableBuffer::setBuffer(void*       buffer,
                              bsl::size_t length) BSLS_KEYWORD_NOEXCEPT
{
    d_data = (BufferType)(buffer);
    d_size = (LengthType)(length);
}

NTSCFG_INLINE
void MutableBuffer::advance(bsl::size_t n) BSLS_KEYWORD_NOEXCEPT
{
    bsl::size_t offset = n < d_size ? n : d_size;

    d_data  = static_cast<char*>(d_data) + offset;
    d_size -= NTSCFG_WARNING_NARROW(LengthType, offset);
}

NTSCFG_INLINE
void* MutableBuffer::data() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data;
}

NTSCFG_INLINE
bsl::size_t MutableBuffer::size() const BSLS_KEYWORD_NOEXCEPT
{
    return d_size;
}

NTSCFG_INLINE
void* MutableBuffer::buffer() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data;
}

NTSCFG_INLINE
bsl::size_t MutableBuffer::length() const BSLS_KEYWORD_NOEXCEPT
{
    return d_size;
}

NTSCFG_INLINE
bsl::size_t MutableBuffer::totalSize(const ntsa::MutableBuffer* bufferArray,
                                     bsl::size_t                bufferCount)
{
    bsl::size_t result = 0;

    for (bsl::size_t i = 0; i < bufferCount; ++i) {
        result += bufferArray[i].size();
    }

    return result;
}

NTSCFG_INLINE
bool operator==(const ntsa::MutableBuffer& lhs, const ntsa::MutableBuffer& rhs)
{
    return (lhs.data() == rhs.data() && lhs.size() == rhs.size());
}

NTSCFG_INLINE
bool operator!=(const ntsa::MutableBuffer& lhs, const ntsa::MutableBuffer& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
ConstBuffer::ConstBuffer() BSLS_KEYWORD_NOEXCEPT
#if defined(BSLS_PLATFORM_OS_UNIX)
: d_data(0),
  d_size(0)
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
: d_size(0),
  d_data(0)
#else
#error Not implemented
#endif
{
}

NTSCFG_INLINE ConstBuffer::ConstBuffer(const bdlbb::BlobBuffer& blobBuffer)
    BSLS_KEYWORD_NOEXCEPT
#if defined(BSLS_PLATFORM_OS_UNIX)
: d_data((BufferType)(blobBuffer.data())),
  d_size((LengthType)(blobBuffer.size()))
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
: d_size((LengthType)(blobBuffer.size())),
  d_data((BufferType)(blobBuffer.data()))
#else
#error Not implemented
#endif
{
}

NTSCFG_INLINE
ConstBuffer::ConstBuffer(const void* data,
                         bsl::size_t size) BSLS_KEYWORD_NOEXCEPT
#if defined(BSLS_PLATFORM_OS_UNIX)
: d_data((BufferType)(data)),
  d_size((LengthType)(size))
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
: d_size((LengthType)(size)),
  d_data((BufferType)(data))
#else
#error Not implemented
#endif
{
}

NTSCFG_INLINE
ConstBuffer::ConstBuffer(const MutableBuffer& other) BSLS_KEYWORD_NOEXCEPT
#if defined(BSLS_PLATFORM_OS_UNIX)
: d_data((BufferType)(other.data())),
  d_size((LengthType)(other.size()))
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
: d_size((LengthType)(other.size())),
  d_data((BufferType)(other.data()))
#else
#error Not implemented
#endif
{
}

NTSCFG_INLINE
ConstBuffer::ConstBuffer(const ConstBuffer& original) BSLS_KEYWORD_NOEXCEPT
#if defined(BSLS_PLATFORM_OS_UNIX)
: d_data(original.d_data),
  d_size(original.d_size)
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
: d_size(original.d_size),
  d_data(original.d_data)
#else
#error Not implemented
#endif
{
}

NTSCFG_INLINE
ConstBuffer::~ConstBuffer() BSLS_KEYWORD_NOEXCEPT
{
}

NTSCFG_INLINE
ConstBuffer& ConstBuffer::operator=(const ConstBuffer& other)
    BSLS_KEYWORD_NOEXCEPT
{
    d_data = other.d_data;
    d_size = other.d_size;
    return *this;
}

NTSCFG_INLINE
ConstBuffer& ConstBuffer::operator=(const MutableBuffer& other)
    BSLS_KEYWORD_NOEXCEPT
{
    d_data = other.data();
    d_size = NTSCFG_WARNING_NARROW(LengthType, other.size());
    return *this;
}

NTSCFG_INLINE
ConstBuffer& ConstBuffer::operator+=(bsl::size_t n) BSLS_KEYWORD_NOEXCEPT
{
    this->advance(n);
    return *this;
}

NTSCFG_INLINE
void ConstBuffer::setBuffer(const bdlbb::BlobBuffer& blobBuffer)
    BSLS_KEYWORD_NOEXCEPT
{
    d_data = (BufferType)(blobBuffer.data());
    d_size = (LengthType)(blobBuffer.size());
}

NTSCFG_INLINE
void ConstBuffer::setBuffer(const void* buffer,
                            bsl::size_t length) BSLS_KEYWORD_NOEXCEPT
{
    d_data = (BufferType)(buffer);
    d_size = (LengthType)(length);
}

NTSCFG_INLINE
void ConstBuffer::advance(bsl::size_t n) BSLS_KEYWORD_NOEXCEPT
{
    bsl::size_t offset = n < d_size ? n : d_size;

    d_data  = static_cast<const char*>(d_data) + offset;
    d_size -= NTSCFG_WARNING_NARROW(LengthType, offset);
}

NTSCFG_INLINE
const void* ConstBuffer::data() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data;
}

NTSCFG_INLINE
bsl::size_t ConstBuffer::size() const BSLS_KEYWORD_NOEXCEPT
{
    return d_size;
}

NTSCFG_INLINE
const void* ConstBuffer::buffer() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data;
}

NTSCFG_INLINE
bsl::size_t ConstBuffer::length() const BSLS_KEYWORD_NOEXCEPT
{
    return d_size;
}

NTSCFG_INLINE
bsl::size_t ConstBuffer::totalSize(const ntsa::ConstBuffer* bufferArray,
                                   bsl::size_t              bufferCount)
{
    bsl::size_t result = 0;

    for (bsl::size_t i = 0; i < bufferCount; ++i) {
        result += bufferArray[i].size();
    }

    return result;
}

NTSCFG_INLINE
bool operator==(const ntsa::ConstBuffer& lhs, const ntsa::ConstBuffer& rhs)
{
    return (lhs.data() == rhs.data() && lhs.size() == rhs.size());
}

NTSCFG_INLINE
bool operator!=(const ntsa::ConstBuffer& lhs, const ntsa::ConstBuffer& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
ConstBufferArray::ConstBufferArray(bslma::Allocator* basicAllocator)
: d_bufferArrayAllocator(basicAllocator)
, d_bufferArray(&d_bufferArrayAllocator)
, d_numBytes(0)
{
}

NTSCFG_INLINE
ConstBufferArray::ConstBufferArray(const ConstBufferArray& original,
                                   bslma::Allocator*       basicAllocator)
: d_bufferArrayAllocator(basicAllocator)
, d_bufferArray(original.d_bufferArray, &d_bufferArrayAllocator)
, d_numBytes(original.d_numBytes)
{
}

NTSCFG_INLINE
ConstBufferArray::~ConstBufferArray()
{
}

NTSCFG_INLINE
ConstBufferArray& ConstBufferArray::operator=(const ConstBufferArray& other)
{
    if (this != &other) {
        d_bufferArray = other.d_bufferArray;
        d_numBytes    = other.d_numBytes;
    }

    return *this;
}

NTSCFG_INLINE
void ConstBufferArray::clear()
{
    d_bufferArray.clear();
    d_numBytes = 0;
}

NTSCFG_INLINE
void ConstBufferArray::reserve(bsl::size_t numBuffers)
{
    d_bufferArray.reserve(numBuffers);
}

NTSCFG_INLINE
void ConstBufferArray::resize(bsl::size_t numBuffers)
{
    d_bufferArray.resize(numBuffers);
}

NTSCFG_INLINE
void ConstBufferArray::append(const void* data, bsl::size_t size)
{
    this->append(ntsa::ConstBuffer(data, size));
}

NTSCFG_INLINE
void ConstBufferArray::append(const ntsa::ConstBuffer& buffer)
{
    d_bufferArray.push_back(buffer);
    d_numBytes += buffer.size();
}

NTSCFG_INLINE
void ConstBufferArray::append(const ntsa::ConstBuffer* bufferList,
                              bsl::size_t              numBuffers)
{
    const ntsa::ConstBuffer* bufferListCurrent = bufferList;
    const ntsa::ConstBuffer* bufferListEnd     = bufferList + numBuffers;

    while (bufferListCurrent != bufferListEnd) {
        this->append(*bufferListCurrent);
        ++bufferListCurrent;
    }
}

NTSCFG_INLINE
void ConstBufferArray::pop(bsl::size_t numBytes)
{
    bsl::size_t numBytesRemaining = numBytes;

    if (numBytesRemaining > d_numBytes) {
        numBytesRemaining = d_numBytes;
    }

    while (numBytesRemaining > 0) {
        if (d_bufferArray.empty()) {
            break;
        }

        bsl::size_t numBytesToPop = numBytesRemaining;
        if (numBytesToPop > d_bufferArray.front().size()) {
            numBytesToPop = d_bufferArray.front().size();
            d_bufferArray.erase(d_bufferArray.begin());
            BSLS_ASSERT(d_numBytes >= numBytesToPop);
            d_numBytes -= numBytesToPop;
        }
        else {
            d_bufferArray.front().advance(numBytesToPop);
            BSLS_ASSERT(d_numBytes >= numBytesToPop);
            d_numBytes -= numBytesToPop;
            break;
        }
    }
}

NTSCFG_INLINE
ntsa::ConstBuffer& ConstBufferArray::buffer(bsl::size_t index)
{
    return d_bufferArray[index];
}

NTSCFG_INLINE
const void* ConstBufferArray::base() const
{
    return &this->buffer(0);
}

NTSCFG_INLINE
const ntsa::ConstBuffer& ConstBufferArray::buffer(bsl::size_t index) const
{
    return d_bufferArray[index];
}

NTSCFG_INLINE
bsl::size_t ConstBufferArray::numBuffers() const
{
    return d_bufferArray.size();
}

NTSCFG_INLINE
bsl::size_t ConstBufferArray::numBytes() const
{
    return d_numBytes;
}

NTSCFG_INLINE
ConstBufferPtrArray::ConstBufferPtrArray()
: d_bufferArray(0)
, d_numBuffers(0)
, d_numBytes(0)
{
}

NTSCFG_INLINE
ConstBufferPtrArray::ConstBufferPtrArray(ntsa::ConstBuffer* bufferList,
                                         bsl::size_t        numBuffers)
: d_bufferArray(bufferList)
, d_numBuffers(numBuffers)
, d_numBytes(0)
{
    const ntsa::ConstBuffer* current = d_bufferArray;
    const ntsa::ConstBuffer* end     = d_bufferArray + d_numBuffers;

    while (current != end) {
        d_numBytes += current->size();
        ++current;
    }
}

NTSCFG_INLINE
ConstBufferPtrArray::ConstBufferPtrArray(const ConstBufferPtrArray& original)
: d_bufferArray(original.d_bufferArray)
, d_numBuffers(original.d_numBuffers)
, d_numBytes(original.d_numBytes)
{
}

NTSCFG_INLINE
ConstBufferPtrArray::~ConstBufferPtrArray()
{
}

NTSCFG_INLINE
ConstBufferPtrArray& ConstBufferPtrArray::operator=(
    const ConstBufferPtrArray& other)
{
    d_bufferArray = other.d_bufferArray;
    d_numBuffers  = other.d_numBuffers;
    d_numBytes    = other.d_numBytes;

    return *this;
}

NTSCFG_INLINE
void ConstBufferPtrArray::reset()
{
    d_bufferArray = 0;
    d_numBuffers  = 0;
    d_numBytes    = 0;
}

NTSCFG_INLINE
void ConstBufferPtrArray::assign(ntsa::ConstBuffer* bufferList,
                                 bsl::size_t        numBuffers)
{
    d_bufferArray = bufferList;
    d_numBuffers  = numBuffers;
    d_numBytes    = 0;

    const ntsa::ConstBuffer* current = d_bufferArray;
    const ntsa::ConstBuffer* end     = d_bufferArray + d_numBuffers;

    while (current != end) {
        d_numBytes += current->size();
        ++current;
    }
}

NTSCFG_INLINE
void ConstBufferPtrArray::pop(bsl::size_t numBytes)
{
    bsl::size_t numBytesRemaining = numBytes;

    if (numBytesRemaining > d_numBytes) {
        numBytesRemaining = d_numBytes;
    }

    while (numBytesRemaining > 0) {
        if (d_numBuffers == 0) {
            break;
        }

        bsl::size_t numBytesToPop = numBytesRemaining;
        if (numBytesToPop > d_bufferArray[0].size()) {
            numBytesToPop = d_bufferArray[0].size();
            d_bufferArray = d_bufferArray + 1;
            BSLS_ASSERT(d_numBytes >= numBytesToPop);
            d_numBytes -= numBytesToPop;
        }
        else {
            d_bufferArray[0].advance(numBytesToPop);
            BSLS_ASSERT(d_numBytes >= numBytesToPop);
            d_numBytes -= numBytesToPop;
            break;
        }
    }
}

NTSCFG_INLINE
ntsa::ConstBuffer& ConstBufferPtrArray::buffer(bsl::size_t index)
{
    return d_bufferArray[index];
}

NTSCFG_INLINE
const void* ConstBufferPtrArray::base() const
{
    return &this->buffer(0);
}

NTSCFG_INLINE
const ntsa::ConstBuffer& ConstBufferPtrArray::buffer(bsl::size_t index) const
{
    return d_bufferArray[index];
}

NTSCFG_INLINE
bsl::size_t ConstBufferPtrArray::numBuffers() const
{
    return d_numBuffers;
}

NTSCFG_INLINE
bsl::size_t ConstBufferPtrArray::numBytes() const
{
    return d_numBytes;
}

NTSCFG_INLINE
MutableBufferArray::MutableBufferArray(bslma::Allocator* basicAllocator)
: d_bufferArrayAllocator(basicAllocator)
, d_bufferArray(&d_bufferArrayAllocator)
, d_numBytes(0)
{
}

NTSCFG_INLINE
MutableBufferArray::MutableBufferArray(const MutableBufferArray& original,
                                       bslma::Allocator* basicAllocator)
: d_bufferArrayAllocator(basicAllocator)
, d_bufferArray(original.d_bufferArray, &d_bufferArrayAllocator)
, d_numBytes(original.d_numBytes)
{
}

NTSCFG_INLINE
MutableBufferArray::~MutableBufferArray()
{
}

NTSCFG_INLINE
MutableBufferArray& MutableBufferArray::operator=(
    const MutableBufferArray& other)
{
    if (this != &other) {
        d_bufferArray = other.d_bufferArray;
        d_numBytes    = other.d_numBytes;
    }

    return *this;
}

NTSCFG_INLINE
void MutableBufferArray::clear()
{
    d_bufferArray.clear();
    d_numBytes = 0;
}

NTSCFG_INLINE
void MutableBufferArray::reserve(bsl::size_t numBuffers)
{
    d_bufferArray.reserve(numBuffers);
}

NTSCFG_INLINE
void MutableBufferArray::resize(bsl::size_t numBuffers)
{
    d_bufferArray.resize(numBuffers);
}

NTSCFG_INLINE
void MutableBufferArray::append(void* data, bsl::size_t size)
{
    this->append(ntsa::MutableBuffer(data, size));
}

NTSCFG_INLINE
void MutableBufferArray::append(const ntsa::MutableBuffer& buffer)
{
    d_bufferArray.push_back(buffer);
    d_numBytes += buffer.size();
}

NTSCFG_INLINE
void MutableBufferArray::append(ntsa::MutableBuffer* bufferList,
                                bsl::size_t          numBuffers)
{
    const ntsa::MutableBuffer* bufferListCurrent = bufferList;
    const ntsa::MutableBuffer* bufferListEnd     = bufferList + numBuffers;

    while (bufferListCurrent != bufferListEnd) {
        this->append(*bufferListCurrent);
        ++bufferListCurrent;
    }
}

NTSCFG_INLINE
void MutableBufferArray::pop(bsl::size_t numBytes)
{
    bsl::size_t numBytesRemaining = numBytes;

    if (numBytesRemaining > d_numBytes) {
        numBytesRemaining = d_numBytes;
    }

    while (numBytesRemaining > 0) {
        if (d_bufferArray.empty()) {
            break;
        }

        bsl::size_t numBytesToPop = numBytesRemaining;
        if (numBytesToPop > d_bufferArray.front().size()) {
            numBytesToPop = d_bufferArray.front().size();
            d_bufferArray.erase(d_bufferArray.begin());
            BSLS_ASSERT(d_numBytes >= numBytesToPop);
            d_numBytes -= numBytesToPop;
        }
        else {
            d_bufferArray.front().advance(numBytesToPop);
            BSLS_ASSERT(d_numBytes >= numBytesToPop);
            d_numBytes -= numBytesToPop;
            break;
        }
    }
}

NTSCFG_INLINE
ntsa::MutableBuffer& MutableBufferArray::buffer(bsl::size_t index)
{
    return d_bufferArray[index];
}

NTSCFG_INLINE
const void* MutableBufferArray::base() const
{
    return &this->buffer(0);
}

NTSCFG_INLINE
const ntsa::MutableBuffer& MutableBufferArray::buffer(bsl::size_t index) const
{
    return d_bufferArray[index];
}

NTSCFG_INLINE
bsl::size_t MutableBufferArray::numBuffers() const
{
    return d_bufferArray.size();
}

NTSCFG_INLINE
bsl::size_t MutableBufferArray::numBytes() const
{
    return d_numBytes;
}

NTSCFG_INLINE
MutableBufferPtrArray::MutableBufferPtrArray()
: d_bufferArray(0)
, d_numBuffers(0)
, d_numBytes(0)
{
}

NTSCFG_INLINE
MutableBufferPtrArray::MutableBufferPtrArray(ntsa::MutableBuffer* bufferList,
                                             bsl::size_t          numBuffers)
: d_bufferArray(bufferList)
, d_numBuffers(numBuffers)
, d_numBytes(0)
{
    const ntsa::MutableBuffer* current = d_bufferArray;
    const ntsa::MutableBuffer* end     = d_bufferArray + d_numBuffers;

    while (current != end) {
        d_numBytes += current->size();
        ++current;
    }
}

NTSCFG_INLINE
MutableBufferPtrArray::MutableBufferPtrArray(
    const MutableBufferPtrArray& original)
: d_bufferArray(original.d_bufferArray)
, d_numBuffers(original.d_numBuffers)
, d_numBytes(original.d_numBytes)
{
}

NTSCFG_INLINE
MutableBufferPtrArray::~MutableBufferPtrArray()
{
}

NTSCFG_INLINE
MutableBufferPtrArray& MutableBufferPtrArray::operator=(
    const MutableBufferPtrArray& other)
{
    d_bufferArray = other.d_bufferArray;
    d_numBuffers  = other.d_numBuffers;
    d_numBytes    = other.d_numBytes;

    return *this;
}

NTSCFG_INLINE
void MutableBufferPtrArray::reset()
{
    d_bufferArray = 0;
    d_numBuffers  = 0;
    d_numBytes    = 0;
}

NTSCFG_INLINE
void MutableBufferPtrArray::assign(ntsa::MutableBuffer* bufferList,
                                   bsl::size_t          numBuffers)
{
    d_bufferArray = bufferList;
    d_numBuffers  = numBuffers;
    d_numBytes    = 0;

    const ntsa::MutableBuffer* current = d_bufferArray;
    const ntsa::MutableBuffer* end     = d_bufferArray + d_numBuffers;

    while (current != end) {
        d_numBytes += current->size();
        ++current;
    }
}

NTSCFG_INLINE
void MutableBufferPtrArray::pop(bsl::size_t numBytes)
{
    bsl::size_t numBytesRemaining = numBytes;

    if (numBytesRemaining > d_numBytes) {
        numBytesRemaining = d_numBytes;
    }

    while (numBytesRemaining > 0) {
        if (d_numBuffers == 0) {
            break;
        }

        bsl::size_t numBytesToPop = numBytesRemaining;
        if (numBytesToPop > d_bufferArray[0].size()) {
            numBytesToPop = d_bufferArray[0].size();
            d_bufferArray = d_bufferArray + 1;
            BSLS_ASSERT(d_numBytes >= numBytesToPop);
            d_numBytes -= numBytesToPop;
        }
        else {
            d_bufferArray[0].advance(numBytesToPop);
            BSLS_ASSERT(d_numBytes >= numBytesToPop);
            d_numBytes -= numBytesToPop;
            break;
        }
    }
}

NTSCFG_INLINE
ntsa::MutableBuffer& MutableBufferPtrArray::buffer(bsl::size_t index)
{
    return d_bufferArray[index];
}

NTSCFG_INLINE
const void* MutableBufferPtrArray::base() const
{
    return &this->buffer(0);
}

NTSCFG_INLINE
const ntsa::MutableBuffer& MutableBufferPtrArray::buffer(
    bsl::size_t index) const
{
    return d_bufferArray[index];
}

NTSCFG_INLINE
bsl::size_t MutableBufferPtrArray::numBuffers() const
{
    return d_numBuffers;
}

NTSCFG_INLINE
bsl::size_t MutableBufferPtrArray::numBytes() const
{
    return d_numBytes;
}

template <typename CONST_BUFFER>
NTSCFG_INLINE ConstBufferSequence<CONST_BUFFER>::ConstBufferSequence(
    const bdlbb::Blob* blob) BSLS_KEYWORD_NOEXCEPT : d_blob_p(blob)
{
}

template <typename CONST_BUFFER>
NTSCFG_INLINE typename ConstBufferSequence<CONST_BUFFER>::Iterator
ConstBufferSequence<CONST_BUFFER>::begin() const BSLS_KEYWORD_NOEXCEPT
{
    if (d_blob_p->length() == 0) {
        return typename ConstBufferSequence<CONST_BUFFER>::Iterator();
    }
    else {
        int index = 0;

        const char* data = d_blob_p->buffer(index).data();
        bsl::size_t size = index == d_blob_p->numDataBuffers() - 1
                               ? d_blob_p->lastDataBufferLength()
                               : d_blob_p->buffer(index).size();

        return typename ConstBufferSequence<CONST_BUFFER>::Iterator(d_blob_p,
                                                                    index,
                                                                    data,
                                                                    size);
    }
}

template <typename CONST_BUFFER>
NTSCFG_INLINE typename ConstBufferSequence<CONST_BUFFER>::Iterator
ConstBufferSequence<CONST_BUFFER>::end() const BSLS_KEYWORD_NOEXCEPT
{
    return typename ConstBufferSequence<CONST_BUFFER>::Iterator();
}

template <typename CONST_BUFFER>
NTSCFG_INLINE ConstBufferSequence<CONST_BUFFER>::Iterator::Iterator()
    BSLS_KEYWORD_NOEXCEPT : d_blob_p(0),
                            d_index(0),
                            d_data(0, 0)
{
}

template <typename CONST_BUFFER>
NTSCFG_INLINE ConstBufferSequence<CONST_BUFFER>::Iterator::Iterator(
    const bdlbb::Blob* blob,
    bsl::size_t        index,
    const char*        data,
    bsl::size_t        size) BSLS_KEYWORD_NOEXCEPT : d_blob_p(blob),
                                              d_index(index),
                                              d_data(data, size)
{
}

template <typename CONST_BUFFER>
NTSCFG_INLINE typename ConstBufferSequence<CONST_BUFFER>::Iterator&
ConstBufferSequence<CONST_BUFFER>::Iterator::operator++() BSLS_KEYWORD_NOEXCEPT
{
    ++d_index;

    if (d_index == static_cast<bsl::size_t>(d_blob_p->numDataBuffers())) {
        d_blob_p = 0;
        d_index  = 0;
        d_data   = value_type(0, 0);
    }
    else {
        const char* data = d_blob_p->buffer(static_cast<int>(d_index)).data();
        bsl::size_t size =
            d_index == static_cast<bsl::size_t>(d_blob_p->numDataBuffers() - 1)
                ? d_blob_p->lastDataBufferLength()
                : d_blob_p->buffer(static_cast<int>(d_index)).size();

        d_data = value_type(data, size);
    }

    return *this;
}

template <typename CONST_BUFFER>
NTSCFG_INLINE typename ConstBufferSequence<CONST_BUFFER>::Iterator
ConstBufferSequence<CONST_BUFFER>::Iterator::operator++(int)
    BSLS_KEYWORD_NOEXCEPT
{
    typename ConstBufferSequence<CONST_BUFFER>::Iterator temp(*this);
    ++(*this);
    return temp;
}

template <typename CONST_BUFFER>
NTSCFG_INLINE const typename ConstBufferSequence<
    CONST_BUFFER>::Iterator::value_type&
ConstBufferSequence<CONST_BUFFER>::Iterator::operator*() const
    BSLS_KEYWORD_NOEXCEPT
{
    return d_data;
}

template <typename CONST_BUFFER>
NTSCFG_INLINE bool ConstBufferSequence<CONST_BUFFER>::Iterator::operator==(
    const Iterator& other) const BSLS_KEYWORD_NOEXCEPT
{
    return d_blob_p == other.d_blob_p &&
           d_data.data() == other.d_data.data() &&
           d_data.size() == other.d_data.size();
}

template <typename CONST_BUFFER>
NTSCFG_INLINE bool ConstBufferSequence<CONST_BUFFER>::Iterator::operator!=(
    const Iterator& other) const BSLS_KEYWORD_NOEXCEPT
{
    return !this->operator==(other);
}

template <typename MUTABLE_BUFFER>
NTSCFG_INLINE MutableBufferSequence<MUTABLE_BUFFER>::MutableBufferSequence(
    bdlbb::Blob* blob) BSLS_KEYWORD_NOEXCEPT : d_blob_p(blob)
{
}

template <typename MUTABLE_BUFFER>
NTSCFG_INLINE typename MutableBufferSequence<MUTABLE_BUFFER>::Iterator
MutableBufferSequence<MUTABLE_BUFFER>::begin() const BSLS_KEYWORD_NOEXCEPT
{
    const int blobLength   = d_blob_p->length();
    const int blobCapacity = d_blob_p->totalSize();

    if (blobLength == blobCapacity) {
        return typename MutableBufferSequence<MUTABLE_BUFFER>::Iterator();
    }
    else if (blobLength == 0) {
        return typename MutableBufferSequence<MUTABLE_BUFFER>::Iterator(
            d_blob_p,
            0,
            d_blob_p->buffer(0).data(),
            d_blob_p->buffer(0).size());
    }
    else {
        const int numDataBuffers       = d_blob_p->numDataBuffers();
        const int lastDataBufferLength = d_blob_p->lastDataBufferLength();

        int index = numDataBuffers - 1;

        if (lastDataBufferLength < d_blob_p->buffer(index).size()) {
            char* data = d_blob_p->buffer(index).data() + lastDataBufferLength;

            const bsl::size_t size =
                d_blob_p->buffer(index).size() - lastDataBufferLength;

            return typename MutableBufferSequence<MUTABLE_BUFFER>::Iterator(
                d_blob_p,
                index,
                data,
                size);
        }
        else {
            ++index;
            BSLS_ASSERT(index != d_blob_p->numBuffers());
            return typename MutableBufferSequence<MUTABLE_BUFFER>::Iterator(
                d_blob_p,
                index,
                d_blob_p->buffer(index).data(),
                d_blob_p->buffer(index).size());
        }
    }
}

template <typename MUTABLE_BUFFER>
NTSCFG_INLINE typename MutableBufferSequence<MUTABLE_BUFFER>::Iterator
MutableBufferSequence<MUTABLE_BUFFER>::end() const BSLS_KEYWORD_NOEXCEPT
{
    return typename MutableBufferSequence<MUTABLE_BUFFER>::Iterator();
}

template <typename MUTABLE_BUFFER>
NTSCFG_INLINE MutableBufferSequence<MUTABLE_BUFFER>::Iterator::Iterator()
    BSLS_KEYWORD_NOEXCEPT : d_blob_p(0),
                            d_index(0),
                            d_data(0, 0)
{
}

template <typename MUTABLE_BUFFER>
NTSCFG_INLINE MutableBufferSequence<MUTABLE_BUFFER>::Iterator::Iterator(
    bdlbb::Blob* blob,
    bsl::size_t  index,
    char*        data,
    bsl::size_t  size) BSLS_KEYWORD_NOEXCEPT : d_blob_p(blob),
                                              d_index(index),
                                              d_data(data, size)
{
}

template <typename MUTABLE_BUFFER>
NTSCFG_INLINE typename MutableBufferSequence<MUTABLE_BUFFER>::Iterator&
MutableBufferSequence<MUTABLE_BUFFER>::Iterator::operator++()
    BSLS_KEYWORD_NOEXCEPT
{
    ++d_index;

    if (d_index == static_cast<bsl::size_t>(d_blob_p->numBuffers())) {
        d_blob_p = 0;
        d_index  = 0;
        d_data   = value_type(0, 0);
    }
    else {
        d_data =
            value_type(d_blob_p->buffer(static_cast<int>(d_index)).data(),
                       d_blob_p->buffer(static_cast<int>(d_index)).size());
    }

    return *this;
}

template <typename MUTABLE_BUFFER>
NTSCFG_INLINE typename MutableBufferSequence<MUTABLE_BUFFER>::Iterator
MutableBufferSequence<MUTABLE_BUFFER>::Iterator::operator++(int)
    BSLS_KEYWORD_NOEXCEPT
{
    typename MutableBufferSequence<MUTABLE_BUFFER>::Iterator temp(*this);
    ++(*this);
    return temp;
}

template <typename MUTABLE_BUFFER>
NTSCFG_INLINE const typename MutableBufferSequence<
    MUTABLE_BUFFER>::Iterator::value_type&
MutableBufferSequence<MUTABLE_BUFFER>::Iterator::operator*() const
    BSLS_KEYWORD_NOEXCEPT
{
    return d_data;
}

template <typename MUTABLE_BUFFER>
NTSCFG_INLINE bool MutableBufferSequence<MUTABLE_BUFFER>::Iterator::operator==(
    const Iterator& other) const BSLS_KEYWORD_NOEXCEPT
{
    return d_blob_p == other.d_blob_p &&
           d_data.data() == other.d_data.data() &&
           d_data.size() == other.d_data.size();
}

template <typename MUTABLE_BUFFER>
NTSCFG_INLINE bool MutableBufferSequence<MUTABLE_BUFFER>::Iterator::operator!=(
    const Iterator& other) const BSLS_KEYWORD_NOEXCEPT
{
    return !this->operator==(other);
}

}  // close package namespace
}  // close enterprise namespace
#endif
