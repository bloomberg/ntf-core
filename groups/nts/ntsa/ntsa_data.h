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

#ifndef INCLUDED_NTSA_DATA
#define INCLUDED_NTSA_DATA

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_buffer.h>
#include <ntsa_datatype.h>
#include <ntsa_error.h>
#include <ntsa_file.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlbb_blob.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

// union DataRep

/// @internal @brief
/// Provide a union of the possible representations of data.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
union DataRep {
    bsls::ObjectBuffer<bdlbb::BlobBuffer>             d_blobBuffer;
    bsls::ObjectBuffer<ntsa::ConstBuffer>             d_constBuffer;
    bsls::ObjectBuffer<ntsa::ConstBufferArray>        d_constBufferArray;
    bsls::ObjectBuffer<ntsa::ConstBufferPtrArray>     d_constBufferPtrArray;
    bsls::ObjectBuffer<ntsa::MutableBuffer>           d_mutableBuffer;
    bsls::ObjectBuffer<ntsa::MutableBufferArray>      d_mutableBufferArray;
    bsls::ObjectBuffer<ntsa::MutableBufferPtrArray>   d_mutableBufferPtrArray;
    bsls::ObjectBuffer<bdlbb::Blob>                   d_blob;
    bsls::ObjectBuffer<bsl::shared_ptr<bdlbb::Blob> > d_sharedBlob;
    bsls::ObjectBuffer<bsl::string>                   d_string;
    bsls::ObjectBuffer<ntsa::File>                    d_file;
};

/// Provide a union of various representations of data.
///
/// @details
/// Provide a container that represents the data in the payload of
/// a datagram sent or received by a datagram socket or a fragment of a stream
/// sent or received by a stream socket.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class Data
{
    ntsa::DataRep             d_value;
    ntsa::DataType::Value     d_type;
    bdlbb::BlobBufferFactory* d_blobBufferFactory_p;
    bslma::Allocator*         d_allocator_p;

  public:
    /// Create a new object representing no data. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Data(bslma::Allocator* basicAllocator = 0);

    /// Create a new object representing no data. Use the specified
    /// 'blobBufferFactory' to subsequently supply blob buffers, as needed,
    /// when the representation of this object is a blob. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is used.
    explicit Data(bdlbb::BlobBufferFactory* blobBufferFactory,
                  bslma::Allocator*         basicAllocator = 0);

    /// Create a new object having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    Data(const Data& original, bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the same value as the specified
    /// 'original' object. Use the specified 'blobBufferFactory' to
    /// subsequently supply blob buffers, as needed, when the representation
    /// of this object is a blob. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    Data(const Data&               original,
         bdlbb::BlobBufferFactory* blobBufferFactory,
         bslma::Allocator*         basicAllocator = 0);

    /// Create a new object initially represented as a blob buffer having
    /// the same value as the specified 'other' object. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Data(const bdlbb::BlobBuffer& other,
                  bslma::Allocator*        basicAllocator = 0);

    /// Create a new object initially represented as a const buffer having
    /// the same value as the specified 'other' object. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Data(const ntsa::ConstBuffer& other,
                  bslma::Allocator*        basicAllocator = 0);

    /// Create a new object initially represented as a const buffer array
    /// having the same value as the specified 'other' object. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit Data(const ntsa::ConstBufferArray& other,
                  bslma::Allocator*             basicAllocator = 0);

    /// Create a new object initially represented as a const buffer pointer
    /// array having the same value as the specified 'other' object.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit Data(const ntsa::ConstBufferPtrArray& other,
                  bslma::Allocator*                basicAllocator = 0);

    /// Create a new object initially represented as a mutable buffer having
    /// the same value as the specified 'other' object. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Data(const ntsa::MutableBuffer& other,
                  bslma::Allocator*          basicAllocator = 0);

    /// Create a new object initially represented as a mutable buffer array
    /// having the same value as the specified 'other' object. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit Data(const ntsa::MutableBufferArray& other,
                  bslma::Allocator*               basicAllocator = 0);

    /// Create a new object initially represented as a mutable buffer
    /// pointer array having the same value as the specified 'other' object.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit Data(const ntsa::MutableBufferPtrArray& other,
                  bslma::Allocator*                  basicAllocator = 0);

    /// Create a new object initially represented as a blob having the same
    /// value as the specified 'other' object. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Data(const bdlbb::Blob& other,
                  bslma::Allocator*  basicAllocator = 0);

    /// Create a new object initially represented as a blob having the same
    /// value as the specified 'other' object using the specified
    /// 'blobBufferFactory' to subsequnetly supply blob buffers as needed.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit Data(const bdlbb::Blob&        other,
                  bdlbb::BlobBufferFactory* blobBufferFactory,
                  bslma::Allocator*         basicAllocator = 0);

    /// Create a new object initially represented as a shared blob having
    /// the same value as the specified 'other' object. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Data(const bsl::shared_ptr<bdlbb::Blob>& other,
                  bslma::Allocator*                   basicAllocator = 0);

    /// Create a new object initially represented as a string having the
    /// same value as the specified 'other' object. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Data(const bsl::string& other,
                  bslma::Allocator*  basicAllocator = 0);

    /// Create a new object initially represented as a file having the same
    /// value as the specified 'other' object. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Data(const ntsa::File& other,
                  bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Data();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Data& operator=(const Data& other);

    /// Make the representation of this data a blob buffer having the same
    /// value as the specified 'other' object.  Return the reference to this
    /// modifiable object.
    Data& operator=(const bdlbb::BlobBuffer& other);

    /// Make the representation of this data a const buffer having the same
    /// value as the specified 'other' object.  Return the reference to this
    /// modifiable object.
    Data& operator=(const ntsa::ConstBuffer& other);

    /// Make the representation of this data a const buffer array having the
    /// same value as the specified 'other' object.  Return the reference to
    /// this modifiable object.
    Data& operator=(const ntsa::ConstBufferArray& other);

    /// Make the representation of this data a const buffer pointer array
    /// having the same value as the specified 'other' object.  Return the
    /// reference to this modifiable object.
    Data& operator=(const ntsa::ConstBufferPtrArray& other);

    /// Make the representation of this data a mutable buffer having the
    /// same value as the specified 'other' object.  Return the reference to
    /// this modifiable object.
    Data& operator=(const ntsa::MutableBuffer& other);

    /// Make the representation of this data a mutable buffer array having
    /// the same value as the specified 'other' object.  Return the
    /// reference to this modifiable object.
    Data& operator=(const ntsa::MutableBufferArray& other);

    /// Make the representation of this data a mutable buffer pointer array
    /// having the same value as the specified 'other' object.  Return the
    /// reference to this modifiable object.
    Data& operator=(const ntsa::MutableBufferPtrArray& other);

    /// Make the representation of this data a blob having the same value as
    /// the specified 'other' object.  Return the reference to this
    /// modifiable object.
    Data& operator=(const bdlbb::Blob& other);

    /// Make the representation of this data a shared blob having the same
    /// value as the specified 'other' object.  Return the reference to this
    /// modifiable object.
    Data& operator=(const bsl::shared_ptr<bdlbb::Blob>& other);

    /// Make the representation of this data a string having the same value
    /// as the specified 'other' object.  Return the reference to this
    /// modifiable object.
    Data& operator=(const bsl::string& other);

    /// Make the representation of this data a file having the same value as
    /// the specified 'other' object.  Return the reference to this
    /// modifiable object.
    Data& operator=(const ntsa::File& other);

    /// Reset the value of the this object to its value upon default
    /// construction.
    void reset();

    /// Make the representation of this data match the specified 'type'.
    void make(ntsa::DataType::Value type);

    /// Make the representation of this data a blob buffer having a default
    /// value.  Return the reference to the modifiable data represented as
    /// a blob buffer.
    bdlbb::BlobBuffer& makeBlobBuffer();

    /// Make the representation of this data a blob buffer having the same
    /// value as the specified 'other' object.  Return the reference to the
    /// modifiable data represented as a blob buffer.
    bdlbb::BlobBuffer& makeBlobBuffer(const bdlbb::BlobBuffer& other);

    /// Make the representation of this data a const buffer having a default
    /// value.  Return the reference to the modifiable data represented as
    /// a const buffer.
    ntsa::ConstBuffer& makeConstBuffer();

    /// Make the representation of this data a const buffer having the same
    /// value as the specified 'other' object.  Return the reference to the
    /// modifiable data represented as a const buffer.
    ntsa::ConstBuffer& makeConstBuffer(const ntsa::ConstBuffer& other);

    /// Make the representation of this data a const buffer array having
    /// a default value.  Return the reference to the modifiable data
    /// represented as a const buffer array.
    ntsa::ConstBufferArray& makeConstBufferArray();

    /// Make the representation of this data a const buffer array having the
    /// same value as the specified 'other' object.  Return the reference to
    /// the modifiable data represented as a const buffer array.
    ntsa::ConstBufferArray& makeConstBufferArray(
        const ntsa::ConstBufferArray& other);

    /// Make the representation of this data a const buffer pointer array
    /// having a default value.  Return the reference to the modifiable data
    /// represented as a const buffer pointer array.
    ntsa::ConstBufferPtrArray& makeConstBufferPtrArray();

    /// Make the representation of this data a const buffer pointer array
    /// having the same value as the specified 'other' object.  Return the
    /// reference to the modifiable data represented as a const buffer
    /// pointer array.
    ntsa::ConstBufferPtrArray& makeConstBufferPtrArray(
        const ntsa::ConstBufferPtrArray& other);

    /// Make the representation of this data a mutable buffer having
    /// a default value.  Return the reference to the modifiable data
    /// represented as a mutable buffer.
    ntsa::MutableBuffer& makeMutableBuffer();

    /// Make the representation of this data a mutable buffer having the
    /// same value as the specified 'other' object.  Return the reference to
    /// the modifiable data represented as a mutable buffer.
    ntsa::MutableBuffer& makeMutableBuffer(const ntsa::MutableBuffer& other);

    /// Make the representation of this data a mutable buffer array having
    /// a default value.  Return the reference to the modifiable data
    /// represented as a mutable buffer array.
    ntsa::MutableBufferArray& makeMutableBufferArray();

    /// Make the representation of this data a mutable buffer array having
    /// the same value as the specified 'other' object.  Return the
    /// reference to the modifiable data represented as a mutable buffer
    /// array.
    ntsa::MutableBufferArray& makeMutableBufferArray(
        const ntsa::MutableBufferArray& other);

    /// Make the representation of this data a mutable buffer pointer array
    /// having a default value.  Return the reference to the modifiable data
    /// represented as a blob buffer.
    ntsa::MutableBufferPtrArray& makeMutableBufferPtrArray();

    /// Make the representation of this data a mutable buffer pointer array
    /// having the same value as the specified 'other' object.  Return the
    /// reference to the modifiable data represented as a mutable buffer
    /// pointer array.
    ntsa::MutableBufferPtrArray& makeMutableBufferPtrArray(
        const ntsa::MutableBufferPtrArray& other);

    /// Make the representation of this data a blob having a default value.
    /// Return the reference to the modifiable data represented as a blob.
    bdlbb::Blob& makeBlob();

    /// Make the representation of this data a blob having the same value as
    /// the specified 'other' object.  Return the reference to the
    /// modifiable data represented as a blob.
    bdlbb::Blob& makeBlob(const bdlbb::Blob& other);

    /// Make the representation of this data a shared blob having a default
    /// value.  Return the reference to the modifiable data represented as
    /// a shared blob.
    bsl::shared_ptr<bdlbb::Blob>& makeSharedBlob();

    /// Make the representation of this data a shared blob having the same
    /// value as the specified 'other' object.  Return the reference to the
    /// modifiable data represented as a shared blob.
    bsl::shared_ptr<bdlbb::Blob>& makeSharedBlob(
        const bsl::shared_ptr<bdlbb::Blob>& other);

    /// Make the representation of this data a string having a default
    /// value.  Return the reference to the modifiable data represented as
    /// a string.
    bsl::string& makeString();

    /// Make the representation of this data a string having the same value
    /// as the specified 'other' object.  Return the reference to the
    /// modifiable data represented as a string.
    bsl::string& makeString(const bsl::string& other);

    /// Make the representation of this data a file having a default value.
    /// Return the reference to the modifiable data represented as a file.
    ntsa::File& makeFile();

    /// Make the representation of this data a file having the same value as
    /// the specified 'other' object.  Return the reference to the
    /// modifiable data represented as a file.
    ntsa::File& makeFile(const ntsa::File& other);

    /// Return the modifiable reference to the data represented as a blob
    /// buffer. The behavior is undefined unless 'isBlobBuffer()' is
    /// true.
    bdlbb::BlobBuffer& blobBuffer();

    /// Return the modifiable reference to the data represented as a const
    /// buffer. The behavior is undefined unless 'isConstBuffer()' is true.
    ntsa::ConstBuffer& constBuffer();

    /// Return the modifiable reference to the data represented as a const
    /// buffer array. The behavior is undefined unless
    /// 'isConstBufferArray()' is true.
    ntsa::ConstBufferArray& constBufferArray();

    /// Return the modifiable reference to the data represented as a const
    /// buffer pointer array. The behavior is undefined unless
    /// 'isConstBufferPtrArray()' is true.
    ntsa::ConstBufferPtrArray& constBufferPtrArray();

    /// Return the modifiable reference to the data represented as a mutable
    /// buffer. The behavior is undefined unless 'isConstBuffer()' is true.
    ntsa::MutableBuffer& mutableBuffer();

    /// Return the modifiable reference to the data represented as a mutable
    /// buffer array. The behavior is undefined unless
    /// 'isMutableBufferArray()' is true.
    ntsa::MutableBufferArray& mutableBufferArray();

    /// Return the modifiable reference to the data represented as a mutable
    /// buffer pointer array. The behavior is undefined unless
    /// 'isMutableBufferPtrArray()' is true.
    ntsa::MutableBufferPtrArray& mutableBufferPtrArray();

    /// Return the modifiable reference to the data represented as a blob.
    /// The behavior is undefined unless 'isBlob()' is true.
    bdlbb::Blob& blob();

    /// Return the modifiable reference to the data represented as a shared
    /// pointer to a blob. The behavior is undefined unless 'isSharedBlob()'
    /// is true.
    bsl::shared_ptr<bdlbb::Blob>& sharedBlob();

    /// Return the modifiable reference to the data represented as a string.
    /// The behavior is undefined unless 'isString()' is true.
    bsl::string& string();

    /// Return the modifiable reference to the data represented as a file.
    /// The behavior is undefined unless 'isFile()' is true.
    ntsa::File& file();

    /// Return the non-modifiable reference to the data represented as
    /// a blob buffer. The behavior is undefined unless 'isBlobBuffer()' is
    /// true.
    const bdlbb::BlobBuffer& blobBuffer() const;

    /// Return the non-modifiable reference to the data represented as
    /// a const buffer. The behavior is undefined unless 'isConstBuffer()'
    /// is true.
    const ntsa::ConstBuffer& constBuffer() const;

    /// Return the non-modifiable reference to the data represented as
    /// a const buffer array. The behavior is undefined unless
    /// 'isConstBufferArray()' is true.
    const ntsa::ConstBufferArray& constBufferArray() const;

    /// Return the non-modifiable reference to the data represented as
    /// a const buffer pointer array. The behavior is undefined unless
    /// 'isConstBufferPtrArray()' is true.
    const ntsa::ConstBufferPtrArray& constBufferPtrArray() const;

    /// Return the non-modifiable reference to the data represented as
    /// a mutable buffer. The behavior is undefined unless 'isConstBuffer()'
    /// is true.
    const ntsa::MutableBuffer& mutableBuffer() const;

    /// Return the non-modifiable reference to the data represented as
    /// a mutable buffer array. The behavior is undefined unless
    /// 'isMutableBufferArray()' is true.
    const ntsa::MutableBufferArray& mutableBufferArray() const;

    /// Return the non-modifiable reference to the data represented as
    /// a mutable buffer pointer array. The behavior is undefined unless
    /// 'isMutableBufferPtrArray()' is true.
    const ntsa::MutableBufferPtrArray& mutableBufferPtrArray() const;

    /// Return the non-modifiable reference to the data represented as
    /// a blob. The behavior is undefined unless 'isBlob()' is true.
    const bdlbb::Blob& blob() const;

    /// Return the non-modifiable reference to the data represented as
    /// a shared pointer to a blob. The behavior is undefined unless
    /// 'isSharedBlob()' is true.
    const bsl::shared_ptr<bdlbb::Blob>& sharedBlob() const;

    /// Return the non-modifiable reference to the data represented as
    /// a string. The behavior is undefined unless 'isString()' is true.
    const bsl::string& string() const;

    /// Return the non-modifiable reference to the data represented as
    /// a file. The behavior is undefined unless 'isFile()' is true.
    const ntsa::File& file() const;

    /// Return the data type.
    ntsa::DataType::Value type() const;

    /// Return true if the data type is undefined, otherwise return false.
    bool isUndefined() const;

    /// Return true if the data type is a blob buffer, otherwise return
    /// false.
    bool isBlobBuffer() const;

    /// Return true if the data type is a const buffer, otherwise return
    /// false.
    bool isConstBuffer() const;

    /// Return true if the data type is a const buffer array, otherwise
    /// return false.
    bool isConstBufferArray() const;

    /// Return true if the data type is a const buffer pointer array,
    /// otherwise return false.
    bool isConstBufferPtrArray() const;

    /// Return true if the data type is a mutable buffer, otherwise return
    /// false.
    bool isMutableBuffer() const;

    /// Return true if the data type is a mutable buffer array, otherwise
    /// return false.
    bool isMutableBufferArray() const;

    /// Return true if the data type is a mutable buffer pointer array,
    /// otherwise return false.
    bool isMutableBufferPtrArray() const;

    /// Return true if the data type is a blob, otherwise return false.
    bool isBlob() const;

    /// Return true if the data type is a shared pointer to a blob,
    /// otherwise return false.
    bool isSharedBlob() const;

    /// Return true if the data type is a string, otherwise return false.
    bool isString() const;

    /// Return true if the data type is a file, otherwise return false.
    bool isFile() const;

    /// Return the total number of defined bytes in the data.
    bsl::size_t size() const;

    /// Return the blob buffer factory used by this data container.
    bdlbb::BlobBufferFactory* blobBufferFactory() const;

    /// Return the allocator used by this data container.
    bslma::Allocator* allocator() const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Data);
};

/// Provide utilities for data containers.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_data
struct DataUtil {
    /// Append the specified 'source' to the specified 'destination'. If
    /// 'source' does not have shared ownership semantics, perform a deep
    /// copy of the data in 'source' when appending to 'destination'. Return
    /// the number of bytes copied.
    static bsl::size_t append(bdlbb::Blob*      destination,
                              const ntsa::Data& source);

    /// Pop the specified 'numBytes' from the specified 'destination'.
    static void pop(ntsa::Data* data, bsl::size_t numBytes);

    /// Copy of the specified 'source' into the specified destination. Return
    /// the error. Note that this function may be considerably expensive, as it
    /// performs a deep copy of all data referenced by 'source', including
    /// any data referenced in a file.
    static ntsa::Error copy(bsl::streambuf*   destination,
                            const ntsa::Data& source);

    /// Copy of the specified 'source' into the specified destination. Return
    /// the error. Note that this function may be considerably expensive, as it
    /// performs a deep copy of all data referenced by 'source', including
    /// any data referenced in a file.
    static ntsa::Error copy(ntsa::Data* destination, const ntsa::Data& source);

    /// Copy of the specified 'source' into the specified destination. Return
    /// the error. Note that this function may be considerably expensive, as it
    /// performs a deep copy of all data referenced by 'source', including
    /// any data referenced in a file.
    static ntsa::Error copy(bdlbb::Blob*      destination,
                            const ntsa::Data& source);

    /// Return true if the specified 'lhs' refers to data having the same value
    /// as the specified 'rhs', otherwise return false. Note that this function
    /// may be considerably expensive, as it may perform a deep copy of all
    /// data referenced by 'lhs' or 'rhs', including any data referenced in a
    /// file.
    static bool equals(const ntsa::Data& lhs, const ntsa::Data& rhs);
};

NTSCFG_INLINE
Data::Data(bslma::Allocator* basicAllocator)
: d_type(ntsa::DataType::e_UNDEFINED)
, d_blobBufferFactory_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

NTSCFG_INLINE
Data::Data(bdlbb::BlobBufferFactory* blobBufferFactory,
           bslma::Allocator*         basicAllocator)
: d_type(ntsa::DataType::e_UNDEFINED)
, d_blobBufferFactory_p(blobBufferFactory)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

NTSCFG_INLINE
bdlbb::BlobBuffer& Data::blobBuffer()
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_BLOB_BUFFER);
    return d_value.d_blobBuffer.object();
}

NTSCFG_INLINE
ntsa::ConstBuffer& Data::constBuffer()
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_CONST_BUFFER);
    return d_value.d_constBuffer.object();
}

NTSCFG_INLINE
ntsa::ConstBufferArray& Data::constBufferArray()
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_CONST_BUFFER_ARRAY);
    return d_value.d_constBufferArray.object();
}

NTSCFG_INLINE
ntsa::ConstBufferPtrArray& Data::constBufferPtrArray()
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_CONST_BUFFER_PTR_ARRAY);
    return d_value.d_constBufferPtrArray.object();
}

NTSCFG_INLINE
ntsa::MutableBuffer& Data::mutableBuffer()
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_MUTABLE_BUFFER);
    return d_value.d_mutableBuffer.object();
}

NTSCFG_INLINE
ntsa::MutableBufferArray& Data::mutableBufferArray()
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_MUTABLE_BUFFER_ARRAY);
    return d_value.d_mutableBufferArray.object();
}

NTSCFG_INLINE
ntsa::MutableBufferPtrArray& Data::mutableBufferPtrArray()
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_MUTABLE_BUFFER_PTR_ARRAY);
    return d_value.d_mutableBufferPtrArray.object();
}

NTSCFG_INLINE
bdlbb::Blob& Data::blob()
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_BLOB);
    return d_value.d_blob.object();
}

NTSCFG_INLINE
bsl::shared_ptr<bdlbb::Blob>& Data::sharedBlob()
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_SHARED_BLOB);
    return d_value.d_sharedBlob.object();
}

NTSCFG_INLINE
bsl::string& Data::string()
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_STRING);
    return d_value.d_string.object();
}

NTSCFG_INLINE
ntsa::File& Data::file()
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_FILE);
    return d_value.d_file.object();
}

NTSCFG_INLINE
const bdlbb::BlobBuffer& Data::blobBuffer() const
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_BLOB_BUFFER);
    return d_value.d_blobBuffer.object();
}

NTSCFG_INLINE
const ntsa::ConstBuffer& Data::constBuffer() const
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_CONST_BUFFER);
    return d_value.d_constBuffer.object();
}

NTSCFG_INLINE
const ntsa::ConstBufferArray& Data::constBufferArray() const
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_CONST_BUFFER_ARRAY);
    return d_value.d_constBufferArray.object();
}

NTSCFG_INLINE
const ntsa::ConstBufferPtrArray& Data::constBufferPtrArray() const
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_CONST_BUFFER_PTR_ARRAY);
    return d_value.d_constBufferPtrArray.object();
}

NTSCFG_INLINE
const ntsa::MutableBuffer& Data::mutableBuffer() const
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_MUTABLE_BUFFER);
    return d_value.d_mutableBuffer.object();
}

NTSCFG_INLINE
const ntsa::MutableBufferArray& Data::mutableBufferArray() const
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_MUTABLE_BUFFER_ARRAY);
    return d_value.d_mutableBufferArray.object();
}

NTSCFG_INLINE
const ntsa::MutableBufferPtrArray& Data::mutableBufferPtrArray() const
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_MUTABLE_BUFFER_PTR_ARRAY);
    return d_value.d_mutableBufferPtrArray.object();
}

NTSCFG_INLINE
const bdlbb::Blob& Data::blob() const
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_BLOB);
    return d_value.d_blob.object();
}

NTSCFG_INLINE
const bsl::shared_ptr<bdlbb::Blob>& Data::sharedBlob() const
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_SHARED_BLOB);
    return d_value.d_sharedBlob.object();
}

NTSCFG_INLINE
const bsl::string& Data::string() const
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_STRING);
    return d_value.d_string.object();
}

NTSCFG_INLINE
const ntsa::File& Data::file() const
{
    BSLS_ASSERT(d_type == ntsa::DataType::e_FILE);
    return d_value.d_file.object();
}

NTSCFG_INLINE
ntsa::DataType::Value Data::type() const
{
    return d_type;
}

NTSCFG_INLINE
bool Data::isUndefined() const
{
    return d_type == ntsa::DataType::e_UNDEFINED;
}

NTSCFG_INLINE
bool Data::isBlobBuffer() const
{
    return d_type == ntsa::DataType::e_BLOB_BUFFER;
}

NTSCFG_INLINE
bool Data::isConstBuffer() const
{
    return d_type == ntsa::DataType::e_CONST_BUFFER;
}

NTSCFG_INLINE
bool Data::isConstBufferArray() const
{
    return d_type == ntsa::DataType::e_CONST_BUFFER_ARRAY;
}

NTSCFG_INLINE
bool Data::isConstBufferPtrArray() const
{
    return d_type == ntsa::DataType::e_CONST_BUFFER_PTR_ARRAY;
}

NTSCFG_INLINE
bool Data::isMutableBuffer() const
{
    return d_type == ntsa::DataType::e_MUTABLE_BUFFER;
}

NTSCFG_INLINE
bool Data::isMutableBufferArray() const
{
    return d_type == ntsa::DataType::e_MUTABLE_BUFFER_ARRAY;
}

NTSCFG_INLINE
bool Data::isMutableBufferPtrArray() const
{
    return d_type == ntsa::DataType::e_MUTABLE_BUFFER_PTR_ARRAY;
}

NTSCFG_INLINE
bool Data::isBlob() const
{
    return d_type == ntsa::DataType::e_BLOB;
}

NTSCFG_INLINE
bool Data::isSharedBlob() const
{
    return d_type == ntsa::DataType::e_SHARED_BLOB;
}

NTSCFG_INLINE
bool Data::isString() const
{
    return d_type == ntsa::DataType::e_STRING;
}

NTSCFG_INLINE
bool Data::isFile() const
{
    return d_type == ntsa::DataType::e_FILE;
}

NTSCFG_INLINE
bdlbb::BlobBufferFactory* Data::blobBufferFactory() const
{
    return d_blobBufferFactory_p;
}

NTSCFG_INLINE
bslma::Allocator* Data::allocator() const
{
    return d_allocator_p;
}

}  // close package namespace
}  // close enterprise namespace
#endif
