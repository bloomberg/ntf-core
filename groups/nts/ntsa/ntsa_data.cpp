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

#include <ntsa_data.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_data_cpp, "$Id$ $CSID$")

#include <bdlbb_blobstreambuf.h>
#include <bdlbb_blobutil.h>
#include <bdlbb_simpleblobbufferfactory.h>
#include <bdls_filesystemutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntsa {

/// Provide utilities for implementing data representations.
class Data::Impl
{
  public:
    /// Define a type alias for a function to manipulator the specified
    /// 'destination' from the specified 'source' using the specified
    /// 'blobBufferFactory' to supply buffers and the specified 'allocator' to
    /// supply memory.
    typedef void (*Modify)(ntsa::DataRep*            destination,
                           const void*               source,
                           bdlbb::BlobBufferFactory* blobBufferFactory,
                           bslma::Allocator*         allocator);

    /// Define a type alias for a function to return the size of the specified
    /// 'source'.
    typedef bsl::size_t (*Size)(const ntsa::DataRep& source);

    /// Default construct the specified 'destination' assuming the
    /// 'destination' is represented as undefined.
    static void defaultConstructUndefined(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Default construct the specified 'destination' assuming the
    /// 'destination' is represented as a blob buffer.
    static void defaultConstructBlobBuffer(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Default construct the specified 'destination' assuming the
    /// 'destination' is represented as a const buffer.
    static void defaultConstructConstBuffer(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Default construct the specified 'destination' assuming the
    /// 'destination' is represented as a const buffer array.
    static void defaultConstructConstBufferArray(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Default construct the specified 'destination' assuming the
    /// 'destination' is represented as a const buffer pointer array.
    static void defaultConstructConstBufferPtrArray(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Default construct the specified 'destination' assuming the
    /// 'destination' is represented as a mutable buffer.
    static void defaultConstructMutableBuffer(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Default construct the specified 'destination' assuming the
    /// 'destination' is represented as a mutable buffer array.
    static void defaultConstructMutableBufferArray(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Default construct the specified 'destination' assuming the
    /// 'destination' is represented as a mutable buffer ptr array.
    static void defaultConstructMutableBufferPtrArray(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Default construct the specified 'destination' assuming the
    /// 'destination' is represented as a blob.
    static void defaultConstructBlob(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Default construct the specified 'destination' assuming the
    /// 'destination' is represented as a shared blob.
    static void defaultConstructSharedBlob(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Default construct the specified 'destination' assuming the
    /// 'destination' is represented as a string.
    static void defaultConstructString(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Default construct the specified 'destination' assuming the
    /// 'destination' is represented as a file.
    static void defaultConstructFile(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    // *** Copy Constructors ***

    /// Copy construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as undefined.
    static void copyConstructUndefined(
        ntsa::DataRep*            destination,
        const ntsa::DataRep*      source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Copy construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a blob buffer.
    static void copyConstructBlobBuffer(
        ntsa::DataRep*            destination,
        const ntsa::DataRep*      source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Copy construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a const buffer.
    static void copyConstructConstBuffer(
        ntsa::DataRep*            destination,
        const ntsa::DataRep*      source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Copy construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a const buffer array.
    static void copyConstructConstBufferArray(
        ntsa::DataRep*            destination,
        const ntsa::DataRep*      source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Copy construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a const buffer pointer
    /// array.
    static void copyConstructConstBufferPtrArray(
        ntsa::DataRep*            destination,
        const ntsa::DataRep*      source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Copy construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a mutable buffer.
    static void copyConstructMutableBuffer(
        ntsa::DataRep*            destination,
        const ntsa::DataRep*      source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Copy construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a mutable buffer array.
    static void copyConstructMutableBufferArray(
        ntsa::DataRep*            destination,
        const ntsa::DataRep*      source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Copy construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a mutable buffer pointer
    /// array.
    static void copyConstructMutableBufferPtrArray(
        ntsa::DataRep*            destination,
        const ntsa::DataRep*      source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Copy construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a blob.
    static void copyConstructBlob(ntsa::DataRep*            destination,
                                  const ntsa::DataRep*      source,
                                  bdlbb::BlobBufferFactory* blobBufferFactory,
                                  bslma::Allocator*         allocator);

    /// Copy construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a shared blob.
    static void copyConstructSharedBlob(
        ntsa::DataRep*            destination,
        const ntsa::DataRep*      source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Copy construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a string.
    static void copyConstructString(
        ntsa::DataRep*            destination,
        const ntsa::DataRep*      source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Copy construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a file.
    static void copyConstructFile(ntsa::DataRep*            destination,
                                  const ntsa::DataRep*      source,
                                  bdlbb::BlobBufferFactory* blobBufferFactory,
                                  bslma::Allocator*         allocator);

    // *** Initializers ***

    /// Construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as undefined.
    static void constructInitializeUndefined(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a blob buffer.
    static void constructInitializeBlobBuffer(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a const buffer.
    static void constructInitializeConstBuffer(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a const buffer
    /// array.
    static void constructInitializeConstBufferArray(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a const buffer
    /// pointer array.
    static void constructInitializeConstBufferPtrArray(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a mutable buffer.
    static void constructInitializeMutableBuffer(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a mutable buffer
    /// array.
    static void constructInitializeMutableBufferArray(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a mutable buffer
    /// pointer array.
    static void constructInitializeMutableBufferPtrArray(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a blob.
    static void constructInitializeBlob(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a shared blob.
    static void constructInitializeSharedBlob(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a string.
    static void constructInitializeString(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Construct the specified 'destination' from the specified
    /// 'source' assuming the 'source' is represented as a file.
    static void constructInitializeFile(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    // *** Destructors ***

    /// Assume the representation of the specified 'destination' has a
    /// trivial destructor and do nothing.
    static void destructTrivial(ntsa::DataRep*            destination,
                                const void*               source,
                                bdlbb::BlobBufferFactory* blobBufferFactory,
                                bslma::Allocator*         allocator);

    /// Destruct the specified 'destination' assuming the 'destination' is
    /// represented as a blob buffer.
    static void destructBlobBuffer(ntsa::DataRep*            destination,
                                   const void*               source,
                                   bdlbb::BlobBufferFactory* blobBufferFactory,
                                   bslma::Allocator*         allocator);

    /// Destruct the specified 'destination' assuming the 'destination' is
    /// represented as a const buffer array.
    static void destructConstBufferArray(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Destruct the specified 'destination' assuming the 'destination' is
    /// represented as a const buffer array.
    static void destructMutableBufferArray(
        ntsa::DataRep*            destination,
        const void*               source,
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         allocator);

    /// Destruct the specified 'destination' assuming the 'destination' is
    /// represented as a const buffer array.
    static void destructBlob(ntsa::DataRep*            destination,
                             const void*               source,
                             bdlbb::BlobBufferFactory* blobBufferFactory,
                             bslma::Allocator*         allocator);

    /// Destruct the specified 'destination' assuming the 'destination' is
    /// represented as a shared blob.
    static void destructSharedBlob(ntsa::DataRep*            destination,
                                   const void*               source,
                                   bdlbb::BlobBufferFactory* blobBufferFactory,
                                   bslma::Allocator*         allocator);

    /// Destruct the specified 'destination' assuming the 'destination' is
    /// represented as a const buffer array.
    static void destructString(ntsa::DataRep*            destination,
                               const void*               source,
                               bdlbb::BlobBufferFactory* blobBufferFactory,
                               bslma::Allocator*         allocator);

    // *** Size ***

    /// Return the size of the specified 'source' assuming the 'source'
    /// is undefined.
    static bsl::size_t getSizeUndefined(const ntsa::DataRep& source);

    /// Return the size of the specified 'source' assuming the 'source'
    /// is represented as a blob buffer.
    static bsl::size_t getSizeBlobBuffer(const ntsa::DataRep& source);

    /// Return the size of the specified 'source' assuming the 'source'
    /// is represented as a const buffer.
    static bsl::size_t getSizeConstBuffer(const ntsa::DataRep& source);

    /// Return the size of the specified 'source' assuming the 'source'
    /// is represented as a const buffer array.
    static bsl::size_t getSizeConstBufferArray(const ntsa::DataRep& source);

    /// Return the size of the specified 'source' assuming the 'source'
    /// is represented as a const buffer pointer array.
    static bsl::size_t getSizeConstBufferPtrArray(const ntsa::DataRep& source);

    /// Return the size of the specified 'source' assuming the 'source'
    /// is represented as a mutable buffer.
    static bsl::size_t getSizeMutableBuffer(const ntsa::DataRep& source);

    /// Return the size of the specified 'source' assuming the 'source'
    /// is represented as a mutable buffer array.
    static bsl::size_t getSizeMutableBufferArray(const ntsa::DataRep& source);

    /// Return the size of the specified 'source' assuming the 'source'
    /// is represented as a mutable buffer pointer array.
    static bsl::size_t getSizeMutableBufferPtrArray(
        const ntsa::DataRep& source);

    /// Return the size of the specified 'source' assuming the 'source'
    /// is represented as a blob.
    static bsl::size_t getSizeBlob(const ntsa::DataRep& source);

    /// Return the size of the specified 'source' assuming the 'source'
    /// is represented as a shared blob.
    static bsl::size_t getSizeSharedBlob(const ntsa::DataRep& source);

    /// Return the size of the specified 'source' assuming the 'source'
    /// is represented as a string.
    static bsl::size_t getSizeString(const ntsa::DataRep& source);

    /// Return the size of the specified 'source' assuming the 'source'
    /// is represented as a file.
    static bsl::size_t getSizeFile(const ntsa::DataRep& source);

    static const Data::Impl::Modify s_defaultConstruct[12];
    static const Data::Impl::Modify s_copyConstruct[12];
    static const Data::Impl::Modify s_constructInitialize[12];
    static const Data::Impl::Modify s_destruct[12];
    static const Data::Impl::Size   s_getSize[12];
};

void Data::Impl::defaultConstructUndefined(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(destination);
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);
}

void Data::Impl::defaultConstructBlobBuffer(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_blobBuffer.buffer()) bdlbb::BlobBuffer();
}

void Data::Impl::defaultConstructConstBuffer(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_constBuffer.buffer()) ntsa::ConstBuffer();
}

void Data::Impl::defaultConstructConstBufferArray(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);

    new (destination->d_constBufferArray.buffer())
        ntsa::ConstBufferArray(allocator);
}

void Data::Impl::defaultConstructConstBufferPtrArray(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_constBufferPtrArray.buffer())
        ntsa::ConstBufferPtrArray();
}

void Data::Impl::defaultConstructMutableBuffer(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_mutableBuffer.buffer()) ntsa::MutableBuffer();
}

void Data::Impl::defaultConstructMutableBufferArray(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);

    new (destination->d_mutableBufferArray.buffer())
        ntsa::MutableBufferArray(allocator);
}

void Data::Impl::defaultConstructMutableBufferPtrArray(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_mutableBufferPtrArray.buffer())
        ntsa::MutableBufferPtrArray();
}

void Data::Impl::defaultConstructBlob(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(source);

    if (blobBufferFactory) {
        new (destination->d_blob.buffer())
            bdlbb::Blob(blobBufferFactory, allocator);
    }
    else {
        new (destination->d_blob.buffer()) bdlbb::Blob(allocator);
    }
}

void Data::Impl::defaultConstructSharedBlob(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_sharedBlob.buffer()) bsl::shared_ptr<bdlbb::Blob>();
}

void Data::Impl::defaultConstructString(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);

    new (destination->d_string.buffer()) bsl::string(allocator);
}

void Data::Impl::defaultConstructFile(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_file.buffer()) ntsa::File();
}

// *** Copy Constructors ***

void Data::Impl::copyConstructUndefined(
    ntsa::DataRep*            destination,
    const ntsa::DataRep*      source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(destination);
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);
}

void Data::Impl::copyConstructBlobBuffer(
    ntsa::DataRep*            destination,
    const ntsa::DataRep*      source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_blobBuffer.buffer())
        bdlbb::BlobBuffer(source->d_blobBuffer.object());
}

void Data::Impl::copyConstructConstBuffer(
    ntsa::DataRep*            destination,
    const ntsa::DataRep*      source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_constBuffer.buffer())
        ntsa::ConstBuffer(source->d_constBuffer.object());
}

void Data::Impl::copyConstructConstBufferArray(
    ntsa::DataRep*            destination,
    const ntsa::DataRep*      source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);

    new (destination->d_constBufferArray.buffer())
        ntsa::ConstBufferArray(source->d_constBufferArray.object(), allocator);
}

void Data::Impl::copyConstructConstBufferPtrArray(
    ntsa::DataRep*            destination,
    const ntsa::DataRep*      source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_constBufferPtrArray.buffer())
        ntsa::ConstBufferPtrArray(source->d_constBufferPtrArray.object());
}

void Data::Impl::copyConstructMutableBuffer(
    ntsa::DataRep*            destination,
    const ntsa::DataRep*      source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_mutableBuffer.buffer())
        ntsa::MutableBuffer(source->d_mutableBuffer.object());
}

void Data::Impl::copyConstructMutableBufferArray(
    ntsa::DataRep*            destination,
    const ntsa::DataRep*      source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);

    new (destination->d_mutableBufferArray.buffer())
        ntsa::MutableBufferArray(source->d_mutableBufferArray.object(),
                                 allocator);
}

void Data::Impl::copyConstructMutableBufferPtrArray(
    ntsa::DataRep*            destination,
    const ntsa::DataRep*      source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_mutableBufferPtrArray.buffer())
        ntsa::MutableBufferPtrArray(source->d_mutableBufferPtrArray.object());
}

void Data::Impl::copyConstructBlob(ntsa::DataRep*            destination,
                                   const ntsa::DataRep*      source,
                                   bdlbb::BlobBufferFactory* blobBufferFactory,
                                   bslma::Allocator*         allocator)
{
    if (blobBufferFactory) {
        new (destination->d_blob.buffer())
            bdlbb::Blob(source->d_blob.object(), blobBufferFactory, allocator);
    }
    else {
        new (destination->d_blob.buffer())
            bdlbb::Blob(source->d_blob.object(), allocator);
    }
}

void Data::Impl::copyConstructSharedBlob(
    ntsa::DataRep*            destination,
    const ntsa::DataRep*      source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_sharedBlob.buffer())
        bsl::shared_ptr<bdlbb::Blob>(source->d_sharedBlob.object());
}

void Data::Impl::copyConstructString(
    ntsa::DataRep*            destination,
    const ntsa::DataRep*      source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);

    new (destination->d_string.buffer())
        bsl::string(source->d_string.object(), allocator);
}

void Data::Impl::copyConstructFile(ntsa::DataRep*            destination,
                                   const ntsa::DataRep*      source,
                                   bdlbb::BlobBufferFactory* blobBufferFactory,
                                   bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_file.buffer()) ntsa::File(source->d_file.object());
}

// *** Initializers ***

void Data::Impl::constructInitializeUndefined(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(destination);
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);
}

void Data::Impl::constructInitializeBlobBuffer(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_blobBuffer.buffer())
        bdlbb::BlobBuffer(*static_cast<const bdlbb::BlobBuffer*>(source));
}

void Data::Impl::constructInitializeConstBuffer(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_constBuffer.buffer())
        ntsa::ConstBuffer(*static_cast<const ntsa::ConstBuffer*>(source));
}

void Data::Impl::constructInitializeConstBufferArray(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);

    new (destination->d_constBufferArray.buffer()) ntsa::ConstBufferArray(
        *static_cast<const ntsa::ConstBufferArray*>(source),
        allocator);
}

void Data::Impl::constructInitializeConstBufferPtrArray(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_constBufferPtrArray.buffer())
        ntsa::ConstBufferPtrArray(
            *static_cast<const ntsa::ConstBufferPtrArray*>(source));
}

void Data::Impl::constructInitializeMutableBuffer(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_mutableBuffer.buffer())
        ntsa::MutableBuffer(*static_cast<const ntsa::MutableBuffer*>(source));
}

void Data::Impl::constructInitializeMutableBufferArray(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);

    new (destination->d_mutableBufferArray.buffer()) ntsa::MutableBufferArray(
        *static_cast<const ntsa::MutableBufferArray*>(source),
        allocator);
}

void Data::Impl::constructInitializeMutableBufferPtrArray(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_mutableBufferPtrArray.buffer())
        ntsa::MutableBufferPtrArray(
            *static_cast<const ntsa::MutableBufferPtrArray*>(source));
}

void Data::Impl::constructInitializeBlob(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    if (blobBufferFactory) {
        new (destination->d_blob.buffer())
            bdlbb::Blob(*static_cast<const bdlbb::Blob*>(source),
                        blobBufferFactory,
                        allocator);
    }
    else {
        new (destination->d_blob.buffer())
            bdlbb::Blob(*static_cast<const bdlbb::Blob*>(source), allocator);
    }
}

void Data::Impl::constructInitializeSharedBlob(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_sharedBlob.buffer()) bsl::shared_ptr<bdlbb::Blob>(
        *static_cast<const bsl::shared_ptr<bdlbb::Blob>*>(source));
}

void Data::Impl::constructInitializeString(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);

    new (destination->d_string.buffer())
        bsl::string(*static_cast<const bsl::string*>(source), allocator);
}

void Data::Impl::constructInitializeFile(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    new (destination->d_file.buffer())
        ntsa::File(*static_cast<const ntsa::File*>(source));
}

// *** Destructors ***

void Data::Impl::destructTrivial(ntsa::DataRep*            destination,
                                 const void*               source,
                                 bdlbb::BlobBufferFactory* blobBufferFactory,
                                 bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(destination);
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);
}

void Data::Impl::destructBlobBuffer(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    typedef bdlbb::BlobBuffer Type;
    destination->d_blobBuffer.object().~Type();
}

void Data::Impl::destructConstBufferArray(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    typedef ntsa::ConstBufferArray Type;
    destination->d_constBufferArray.object().~Type();
}

void Data::Impl::destructMutableBufferArray(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    typedef ntsa::MutableBufferArray Type;
    destination->d_mutableBufferArray.object().~Type();
}

void Data::Impl::destructBlob(ntsa::DataRep*            destination,
                              const void*               source,
                              bdlbb::BlobBufferFactory* blobBufferFactory,
                              bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    typedef bdlbb::Blob Type;
    destination->d_blob.object().~Type();
}

void Data::Impl::destructSharedBlob(
    ntsa::DataRep*            destination,
    const void*               source,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    typedef bsl::shared_ptr<bdlbb::Blob> Type;
    destination->d_sharedBlob.object().~Type();
}

void Data::Impl::destructString(ntsa::DataRep*            destination,
                                const void*               source,
                                bdlbb::BlobBufferFactory* blobBufferFactory,
                                bslma::Allocator*         allocator)
{
    NTSCFG_WARNING_UNUSED(source);
    NTSCFG_WARNING_UNUSED(blobBufferFactory);
    NTSCFG_WARNING_UNUSED(allocator);

    typedef bsl::string Type;
    destination->d_string.object().~Type();
}

// *** Size ***

bsl::size_t Data::Impl::getSizeUndefined(const ntsa::DataRep& source)
{
    NTSCFG_WARNING_UNUSED(source);

    return 0;
}

bsl::size_t Data::Impl::getSizeBlobBuffer(const ntsa::DataRep& source)
{
    return source.d_blobBuffer.object().size();
}

bsl::size_t Data::Impl::getSizeConstBuffer(const ntsa::DataRep& source)
{
    return source.d_constBuffer.object().size();
}

bsl::size_t Data::Impl::getSizeConstBufferArray(const ntsa::DataRep& source)
{
    return source.d_constBufferArray.object().numBytes();
}

bsl::size_t Data::Impl::getSizeConstBufferPtrArray(const ntsa::DataRep& source)
{
    return source.d_constBufferPtrArray.object().numBytes();
}

bsl::size_t Data::Impl::getSizeMutableBuffer(const ntsa::DataRep& source)
{
    return source.d_mutableBuffer.object().size();
}

bsl::size_t Data::Impl::getSizeMutableBufferArray(const ntsa::DataRep& source)
{
    return source.d_mutableBufferArray.object().numBytes();
}

bsl::size_t Data::Impl::getSizeMutableBufferPtrArray(
    const ntsa::DataRep& source)
{
    return source.d_mutableBufferPtrArray.object().numBytes();
}

bsl::size_t Data::Impl::getSizeBlob(const ntsa::DataRep& source)
{
    return source.d_blob.object().length();
}

bsl::size_t Data::Impl::getSizeSharedBlob(const ntsa::DataRep& source)
{
    return source.d_sharedBlob.object()->length();
}

bsl::size_t Data::Impl::getSizeString(const ntsa::DataRep& source)
{
    return source.d_string.object().size();
}

bsl::size_t Data::Impl::getSizeFile(const ntsa::DataRep& source)
{
    return NTSCFG_WARNING_NARROW(bsl::size_t,
                                 source.d_file.object().bytesRemaining());
}

const Data::Impl::Modify Data::Impl::s_defaultConstruct[12] = {
    (Data::Impl::Modify)&Data::Impl::defaultConstructUndefined,
    (Data::Impl::Modify)&Data::Impl::defaultConstructBlobBuffer,
    (Data::Impl::Modify)&Data::Impl::defaultConstructConstBuffer,
    (Data::Impl::Modify)&Data::Impl::defaultConstructConstBufferArray,
    (Data::Impl::Modify)&Data::Impl::defaultConstructConstBufferPtrArray,
    (Data::Impl::Modify)&Data::Impl::defaultConstructMutableBuffer,
    (Data::Impl::Modify)&Data::Impl::defaultConstructMutableBufferArray,
    (Data::Impl::Modify)&Data::Impl::defaultConstructMutableBufferPtrArray,
    (Data::Impl::Modify)&Data::Impl::defaultConstructBlob,
    (Data::Impl::Modify)&Data::Impl::defaultConstructSharedBlob,
    (Data::Impl::Modify)&Data::Impl::defaultConstructString,
    (Data::Impl::Modify)&Data::Impl::defaultConstructFile};

const Data::Impl::Modify Data::Impl::s_copyConstruct[12] = {
    (Data::Impl::Modify)&Data::Impl::copyConstructUndefined,
    (Data::Impl::Modify)&Data::Impl::copyConstructBlobBuffer,
    (Data::Impl::Modify)&Data::Impl::copyConstructConstBuffer,
    (Data::Impl::Modify)&Data::Impl::copyConstructConstBufferArray,
    (Data::Impl::Modify)&Data::Impl::copyConstructConstBufferPtrArray,
    (Data::Impl::Modify)&Data::Impl::copyConstructMutableBuffer,
    (Data::Impl::Modify)&Data::Impl::copyConstructMutableBufferArray,
    (Data::Impl::Modify)&Data::Impl::copyConstructMutableBufferPtrArray,
    (Data::Impl::Modify)&Data::Impl::copyConstructBlob,
    (Data::Impl::Modify)&Data::Impl::copyConstructSharedBlob,
    (Data::Impl::Modify)&Data::Impl::copyConstructString,
    (Data::Impl::Modify)&Data::Impl::copyConstructFile};

const Data::Impl::Modify Data::Impl::s_constructInitialize[12] = {
    (Data::Impl::Modify)&Data::Impl::constructInitializeUndefined,
    (Data::Impl::Modify)&Data::Impl::constructInitializeBlobBuffer,
    (Data::Impl::Modify)&Data::Impl::constructInitializeConstBuffer,
    (Data::Impl::Modify)&Data::Impl::constructInitializeConstBufferArray,
    (Data::Impl::Modify)&Data::Impl::constructInitializeConstBufferPtrArray,
    (Data::Impl::Modify)&Data::Impl::constructInitializeMutableBuffer,
    (Data::Impl::Modify)&Data::Impl::constructInitializeMutableBufferArray,
    (Data::Impl::Modify)&Data::Impl::constructInitializeMutableBufferPtrArray,
    (Data::Impl::Modify)&Data::Impl::constructInitializeBlob,
    (Data::Impl::Modify)&Data::Impl::constructInitializeSharedBlob,
    (Data::Impl::Modify)&Data::Impl::constructInitializeString,
    (Data::Impl::Modify)&Data::Impl::constructInitializeFile};

const Data::Impl::Modify Data::Impl::s_destruct[12] = {
    (Data::Impl::Modify)&Data::Impl::destructTrivial,
    (Data::Impl::Modify)&Data::Impl::destructBlobBuffer,
    (Data::Impl::Modify)&Data::Impl::destructTrivial,
    (Data::Impl::Modify)&Data::Impl::destructConstBufferArray,
    (Data::Impl::Modify)&Data::Impl::destructTrivial,
    (Data::Impl::Modify)&Data::Impl::destructTrivial,
    (Data::Impl::Modify)&Data::Impl::destructMutableBufferArray,
    (Data::Impl::Modify)&Data::Impl::destructTrivial,
    (Data::Impl::Modify)&Data::Impl::destructBlob,
    (Data::Impl::Modify)&Data::Impl::destructSharedBlob,
    (Data::Impl::Modify)&Data::Impl::destructString,
    (Data::Impl::Modify)&Data::Impl::destructTrivial};

const Data::Impl::Size Data::Impl::s_getSize[12] = {
    (Data::Impl::Size)&Data::Impl::getSizeUndefined,
    (Data::Impl::Size)&Data::Impl::getSizeBlobBuffer,
    (Data::Impl::Size)&Data::Impl::getSizeConstBuffer,
    (Data::Impl::Size)&Data::Impl::getSizeConstBufferArray,
    (Data::Impl::Size)&Data::Impl::getSizeConstBufferPtrArray,
    (Data::Impl::Size)&Data::Impl::getSizeMutableBuffer,
    (Data::Impl::Size)&Data::Impl::getSizeMutableBufferArray,
    (Data::Impl::Size)&Data::Impl::getSizeMutableBufferPtrArray,
    (Data::Impl::Size)&Data::Impl::getSizeBlob,
    (Data::Impl::Size)&Data::Impl::getSizeSharedBlob,
    (Data::Impl::Size)&Data::Impl::getSizeString,
    (Data::Impl::Size)&Data::Impl::getSizeFile};

Data::Data(const Data& original, bslma::Allocator* basicAllocator)
: d_type(original.d_type)
, d_blobBufferFactory_p(original.d_blobBufferFactory_p)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    Data::Impl::s_copyConstruct[d_type](&d_value,
                                        &original.d_value,
                                        d_blobBufferFactory_p,
                                        d_allocator_p);
}

Data::Data(const Data&               original,
           bdlbb::BlobBufferFactory* blobBufferFactory,
           bslma::Allocator*         basicAllocator)
: d_type(original.d_type)
, d_blobBufferFactory_p(blobBufferFactory)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    Data::Impl::s_copyConstruct[d_type](&d_value,
                                        &original.d_value,
                                        d_blobBufferFactory_p,
                                        d_allocator_p);
}

Data::Data(const bdlbb::BlobBuffer& other, bslma::Allocator* basicAllocator)
: d_type(ntsa::DataType::e_BLOB_BUFFER)
, d_blobBufferFactory_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    Data::Impl::s_constructInitialize[ntsa::DataType::e_BLOB_BUFFER](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
}

Data::Data(const ntsa::ConstBuffer& other, bslma::Allocator* basicAllocator)
: d_type(ntsa::DataType::e_CONST_BUFFER)
, d_blobBufferFactory_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    Data::Impl::s_constructInitialize[ntsa::DataType::e_CONST_BUFFER](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
}

Data::Data(const ntsa::ConstBufferArray& other,
           bslma::Allocator*             basicAllocator)
: d_type(ntsa::DataType::e_CONST_BUFFER_ARRAY)
, d_blobBufferFactory_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    Data::Impl::s_constructInitialize[ntsa::DataType::e_CONST_BUFFER_ARRAY](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
}

Data::Data(const ntsa::ConstBufferPtrArray& other,
           bslma::Allocator*                basicAllocator)
: d_type(ntsa::DataType::e_CONST_BUFFER_PTR_ARRAY)
, d_blobBufferFactory_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    Data::Impl::s_constructInitialize
        [ntsa::DataType::e_CONST_BUFFER_PTR_ARRAY](&d_value,
                                                   &other,
                                                   d_blobBufferFactory_p,
                                                   d_allocator_p);
}

Data::Data(const ntsa::MutableBuffer& other, bslma::Allocator* basicAllocator)
: d_type(ntsa::DataType::e_MUTABLE_BUFFER)
, d_blobBufferFactory_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    Data::Impl::s_constructInitialize[ntsa::DataType::e_MUTABLE_BUFFER](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
}

Data::Data(const ntsa::MutableBufferArray& other,
           bslma::Allocator*               basicAllocator)
: d_type(ntsa::DataType::e_MUTABLE_BUFFER_ARRAY)
, d_blobBufferFactory_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    Data::Impl::s_constructInitialize[ntsa::DataType::e_MUTABLE_BUFFER_ARRAY](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
}

Data::Data(const ntsa::MutableBufferPtrArray& other,
           bslma::Allocator*                  basicAllocator)
: d_type(ntsa::DataType::e_MUTABLE_BUFFER_PTR_ARRAY)
, d_blobBufferFactory_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    Data::Impl::s_constructInitialize
        [ntsa::DataType::e_MUTABLE_BUFFER_PTR_ARRAY](&d_value,
                                                     &other,
                                                     d_blobBufferFactory_p,
                                                     d_allocator_p);
}

Data::Data(const bdlbb::Blob& other, bslma::Allocator* basicAllocator)
: d_type(ntsa::DataType::e_BLOB)
, d_blobBufferFactory_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    Data::Impl::s_constructInitialize[ntsa::DataType::e_BLOB](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
}

Data::Data(const bdlbb::Blob&        other,
           bdlbb::BlobBufferFactory* blobBufferFactory,
           bslma::Allocator*         basicAllocator)
: d_type(ntsa::DataType::e_BLOB)
, d_blobBufferFactory_p(blobBufferFactory)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    Data::Impl::s_constructInitialize[ntsa::DataType::e_BLOB](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
}

Data::Data(const bsl::shared_ptr<bdlbb::Blob>& other,
           bslma::Allocator*                   basicAllocator)
: d_type(ntsa::DataType::e_SHARED_BLOB)
, d_blobBufferFactory_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    Data::Impl::s_constructInitialize[ntsa::DataType::e_SHARED_BLOB](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
}

Data::Data(const bsl::string& other, bslma::Allocator* basicAllocator)
: d_type(ntsa::DataType::e_STRING)
, d_blobBufferFactory_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    Data::Impl::s_constructInitialize[ntsa::DataType::e_STRING](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
}

Data::Data(const ntsa::File& other, bslma::Allocator* basicAllocator)
: d_type(ntsa::DataType::e_FILE)
, d_blobBufferFactory_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    Data::Impl::s_constructInitialize[ntsa::DataType::e_FILE](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
}

Data::~Data()
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_UNDEFINED;
}

Data& Data::operator=(const Data& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = other.d_type;
    Data::Impl::s_copyConstruct[d_type](&d_value,
                                        &other,
                                        d_blobBufferFactory_p,
                                        d_allocator_p);
    return *this;
}

Data& Data::operator=(const bdlbb::BlobBuffer& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_BLOB_BUFFER;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_BLOB_BUFFER](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return *this;
}

Data& Data::operator=(const ntsa::ConstBuffer& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_CONST_BUFFER;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_CONST_BUFFER](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return *this;
}

Data& Data::operator=(const ntsa::ConstBufferArray& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_CONST_BUFFER_ARRAY;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_CONST_BUFFER_ARRAY](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return *this;
}

Data& Data::operator=(const ntsa::ConstBufferPtrArray& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_CONST_BUFFER_PTR_ARRAY;
    Data::Impl::s_constructInitialize
        [ntsa::DataType::e_CONST_BUFFER_PTR_ARRAY](&d_value,
                                                   &other,
                                                   d_blobBufferFactory_p,
                                                   d_allocator_p);
    return *this;
}

Data& Data::operator=(const ntsa::MutableBuffer& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_MUTABLE_BUFFER;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_MUTABLE_BUFFER](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return *this;
}

Data& Data::operator=(const ntsa::MutableBufferArray& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_MUTABLE_BUFFER_ARRAY;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_MUTABLE_BUFFER_ARRAY](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return *this;
}

Data& Data::operator=(const ntsa::MutableBufferPtrArray& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_MUTABLE_BUFFER_PTR_ARRAY;
    Data::Impl::s_constructInitialize
        [ntsa::DataType::e_MUTABLE_BUFFER_PTR_ARRAY](&d_value,
                                                     &other,
                                                     d_blobBufferFactory_p,
                                                     d_allocator_p);
    return *this;
}

Data& Data::operator=(const bdlbb::Blob& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_BLOB;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_BLOB](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return *this;
}

Data& Data::operator=(const bsl::shared_ptr<bdlbb::Blob>& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_SHARED_BLOB;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_SHARED_BLOB](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return *this;
}

Data& Data::operator=(const bsl::string& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_STRING;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_STRING](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return *this;
}

Data& Data::operator=(const ntsa::File& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_FILE;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_FILE](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return *this;
}

void Data::reset()
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_UNDEFINED;
}

void Data::make(ntsa::DataType::Value type)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = type;
    Data::Impl::s_defaultConstruct[d_type](&d_value,
                                           0,
                                           d_blobBufferFactory_p,
                                           d_allocator_p);
}

bdlbb::BlobBuffer& Data::makeBlobBuffer()
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_BLOB_BUFFER;
    Data::Impl::s_defaultConstruct[ntsa::DataType::e_BLOB_BUFFER](
        &d_value,
        0,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_blobBuffer.object();
}

bdlbb::BlobBuffer& Data::makeBlobBuffer(const bdlbb::BlobBuffer& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_BLOB_BUFFER;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_BLOB_BUFFER](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_blobBuffer.object();
}

ntsa::ConstBuffer& Data::makeConstBuffer()
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_CONST_BUFFER;
    Data::Impl::s_defaultConstruct[ntsa::DataType::e_CONST_BUFFER](
        &d_value,
        0,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_constBuffer.object();
}

ntsa::ConstBuffer& Data::makeConstBuffer(const ntsa::ConstBuffer& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_CONST_BUFFER;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_CONST_BUFFER](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_constBuffer.object();
}

ntsa::ConstBufferArray& Data::makeConstBufferArray()
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_CONST_BUFFER_ARRAY;
    Data::Impl::s_defaultConstruct[ntsa::DataType::e_CONST_BUFFER_ARRAY](
        &d_value,
        0,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_constBufferArray.object();
}

ntsa::ConstBufferArray& Data::makeConstBufferArray(
    const ntsa::ConstBufferArray& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_CONST_BUFFER_ARRAY;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_CONST_BUFFER_ARRAY](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_constBufferArray.object();
}

ntsa::ConstBufferPtrArray& Data::makeConstBufferPtrArray()
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_CONST_BUFFER_PTR_ARRAY;
    Data::Impl::s_defaultConstruct[ntsa::DataType::e_CONST_BUFFER_PTR_ARRAY](
        &d_value,
        0,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_constBufferPtrArray.object();
}

ntsa::ConstBufferPtrArray& Data::makeConstBufferPtrArray(
    const ntsa::ConstBufferPtrArray& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_CONST_BUFFER_PTR_ARRAY;
    Data::Impl::s_constructInitialize
        [ntsa::DataType::e_CONST_BUFFER_PTR_ARRAY](&d_value,
                                                   &other,
                                                   d_blobBufferFactory_p,
                                                   d_allocator_p);
    return d_value.d_constBufferPtrArray.object();
}

ntsa::MutableBuffer& Data::makeMutableBuffer()
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_MUTABLE_BUFFER;
    Data::Impl::s_defaultConstruct[ntsa::DataType::e_MUTABLE_BUFFER](
        &d_value,
        0,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_mutableBuffer.object();
}

ntsa::MutableBuffer& Data::makeMutableBuffer(const ntsa::MutableBuffer& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_MUTABLE_BUFFER;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_MUTABLE_BUFFER](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_mutableBuffer.object();
}

ntsa::MutableBufferArray& Data::makeMutableBufferArray()
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_MUTABLE_BUFFER_ARRAY;
    Data::Impl::s_defaultConstruct[ntsa::DataType::e_MUTABLE_BUFFER_ARRAY](
        &d_value,
        0,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_mutableBufferArray.object();
}

ntsa::MutableBufferArray& Data::makeMutableBufferArray(
    const ntsa::MutableBufferArray& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_MUTABLE_BUFFER_ARRAY;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_MUTABLE_BUFFER_ARRAY](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_mutableBufferArray.object();
}

ntsa::MutableBufferPtrArray& Data::makeMutableBufferPtrArray()
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_MUTABLE_BUFFER_PTR_ARRAY;
    Data::Impl::s_defaultConstruct[ntsa::DataType::e_MUTABLE_BUFFER_PTR_ARRAY](
        &d_value,
        0,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_mutableBufferPtrArray.object();
}

ntsa::MutableBufferPtrArray& Data::makeMutableBufferPtrArray(
    const ntsa::MutableBufferPtrArray& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_MUTABLE_BUFFER_PTR_ARRAY;
    Data::Impl::s_constructInitialize
        [ntsa::DataType::e_MUTABLE_BUFFER_PTR_ARRAY](&d_value,
                                                     &other,
                                                     d_blobBufferFactory_p,
                                                     d_allocator_p);
    return d_value.d_mutableBufferPtrArray.object();
}

bdlbb::Blob& Data::makeBlob()
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_BLOB;
    Data::Impl::s_defaultConstruct[ntsa::DataType::e_BLOB](
        &d_value,
        0,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_blob.object();
}

bdlbb::Blob& Data::makeBlob(const bdlbb::Blob& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_BLOB;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_BLOB](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_blob.object();
}

bsl::shared_ptr<bdlbb::Blob>& Data::makeSharedBlob()
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_SHARED_BLOB;
    Data::Impl::s_defaultConstruct[ntsa::DataType::e_SHARED_BLOB](
        &d_value,
        0,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_sharedBlob.object();
}

bsl::shared_ptr<bdlbb::Blob>& Data::makeSharedBlob(
    const bsl::shared_ptr<bdlbb::Blob>& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_SHARED_BLOB;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_SHARED_BLOB](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_sharedBlob.object();
}

bsl::string& Data::makeString()
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_STRING;
    Data::Impl::s_defaultConstruct[ntsa::DataType::e_STRING](
        &d_value,
        0,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_string.object();
}

bsl::string& Data::makeString(const bsl::string& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_STRING;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_STRING](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_string.object();
}

ntsa::File& Data::makeFile()
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_FILE;
    Data::Impl::s_defaultConstruct[ntsa::DataType::e_FILE](
        &d_value,
        0,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_file.object();
}

ntsa::File& Data::makeFile(const ntsa::File& other)
{
    Data::Impl::s_destruct[d_type](&d_value,
                                   0,
                                   d_blobBufferFactory_p,
                                   d_allocator_p);
    d_type = ntsa::DataType::e_FILE;
    Data::Impl::s_constructInitialize[ntsa::DataType::e_FILE](
        &d_value,
        &other,
        d_blobBufferFactory_p,
        d_allocator_p);
    return d_value.d_file.object();
}

bsl::size_t Data::size() const
{
    return Data::Impl::s_getSize[d_type](d_value);
}

/// Provide algorithms for data containers. This struct is
/// thread safe.
class DataUtil::Impl
{
  public:
    /// Define a type alias for a function to append the specified 'source' to
    /// the specified 'destination'.
    typedef bsl::size_t (*Append)(bdlbb::Blob*      destination,
                                  const ntsa::Data& source);

    /// Define a type alias for a function to pop the specified 'numBytes'
    /// from the specified 'data'.
    typedef void (*Pop)(ntsa::Data* data, bsl::size_t numBytes);

    /// Append the specified 'source' to the specified 'destination',
    /// assuming the 'source' is represented as undefined.
    static bsl::size_t appendUndefined(bdlbb::Blob*      destination,
                                       const ntsa::Data& source);

    /// Append the specified 'source' to the specified 'destination',
    /// assuming the 'source' is represented as a blob buffer.
    static bsl::size_t appendBlobBuffer(bdlbb::Blob*      destination,
                                        const ntsa::Data& source);

    /// Append the specified 'source' to the specified 'destination',
    /// assuming the 'source' is represented as a const buffer.
    static bsl::size_t appendConstBuffer(bdlbb::Blob*      destination,
                                         const ntsa::Data& source);

    /// Append the specified 'source' to the specified 'destination',
    /// assuming the 'source' is represented as a const buffer array.
    static bsl::size_t appendConstBufferArray(bdlbb::Blob*      destination,
                                              const ntsa::Data& source);

    /// Append the specified 'source' to the specified 'destination',
    /// assuming the 'source' is represented as a const buffer pointer
    /// array.
    static bsl::size_t appendConstBufferPtrArray(bdlbb::Blob*      destination,
                                                 const ntsa::Data& source);

    /// Append the specified 'source' to the specified 'destination',
    /// assuming the 'source' is represented as mutable buffer.
    static bsl::size_t appendMutableBuffer(bdlbb::Blob*      destination,
                                           const ntsa::Data& source);

    /// Append the specified 'source' to the specified 'destination',
    /// assuming the 'source' is represented as a mutable buffer array.
    static bsl::size_t appendMutableBufferArray(bdlbb::Blob*      destination,
                                                const ntsa::Data& source);

    /// Append the specified 'source' to the specified 'destination',
    /// assuming the 'source' is represented as mutable buffer pointer
    /// array.
    static bsl::size_t appendMutableBufferPtrArray(bdlbb::Blob* destination,
                                                   const ntsa::Data& source);

    /// Append the specified 'source' to the specified 'destination',
    /// assuming the 'source' is represented as a blob.
    static bsl::size_t appendBlob(bdlbb::Blob*      destination,
                                  const ntsa::Data& source);

    /// Append the specified 'source' to the specified 'destination',
    /// assuming the 'source' is represented as a shared blob.
    static bsl::size_t appendSharedBlob(bdlbb::Blob*      destination,
                                        const ntsa::Data& source);

    /// Append the specified 'source' to the specified 'destination',
    /// assuming the 'source' is represented as a string.
    static bsl::size_t appendString(bdlbb::Blob*      destination,
                                    const ntsa::Data& source);

    /// Append the specified 'source' to the specified 'destination',
    /// assuming the 'source' is represented as a file.
    static bsl::size_t appendFile(bdlbb::Blob*      destination,
                                  const ntsa::Data& source);

    // *** Pop ***

    /// Pop the specified 'numBytes' from the specified 'data' assuming
    /// 'data' is undefined.
    static void popUndefined(ntsa::Data* data, bsl::size_t numBytes);

    /// Pop the specified 'numBytes' from the specified 'data' assuming
    /// 'data' is represented as a blob buffer.
    static void popBlobBuffer(ntsa::Data* data, bsl::size_t numBytes);

    /// Pop the specified 'numBytes' from the specified 'data' assuming
    /// 'data' is represented as a const buffer.
    static void popConstBuffer(ntsa::Data* data, bsl::size_t numBytes);

    /// Pop the specified 'numBytes' from the specified 'data' assuming
    /// 'data' is represented as a const buffer array.
    static void popConstBufferArray(ntsa::Data* data, bsl::size_t numBytes);

    /// Pop the specified 'numBytes' from the specified 'data' assuming
    /// 'data' is represented as a const buffer pointer array.
    static void popConstBufferPtrArray(ntsa::Data* data, bsl::size_t numBytes);

    /// Pop the specified 'numBytes' from the specified 'data' assuming
    /// 'data' is represented as a mutable buffer.
    static void popMutableBuffer(ntsa::Data* data, bsl::size_t numBytes);

    /// Pop the specified 'numBytes' from the specified 'data' assuming
    /// 'data' is represented as a mutable buffer array.
    static void popMutableBufferArray(ntsa::Data* data, bsl::size_t numBytes);

    /// Pop the specified 'numBytes' from the specified 'data' assuming
    /// 'data' is represented as a mutable buffer pointer array.
    static void popMutableBufferPtrArray(ntsa::Data* data,
                                         bsl::size_t numBytes);

    /// Pop the specified 'numBytes' from the specified 'data' assuming
    /// 'data' is represented as a blob.
    static void popBlob(ntsa::Data* data, bsl::size_t numBytes);

    /// Pop the specified 'numBytes' from the specified 'data' assuming
    /// 'data' is represented as a shared blob.
    static void popSharedBlob(ntsa::Data* data, bsl::size_t numBytes);

    /// Pop the specified 'numBytes' from the specified 'data' assuming
    /// 'data' is represented as a string.
    static void popString(ntsa::Data* data, bsl::size_t numBytes);

    /// Pop the specified 'numBytes' from the specified 'data' assuming
    /// 'data' is represented as a file.
    static void popFile(ntsa::Data* data, bsl::size_t numBytes);

    /// *** Copy ***

    /// Copy the specified 'source' into the specified 'destination'. Return
    /// the error.
    static ntsa::Error copyBlob(bsl::streambuf*    destination,
                                const bdlbb::Blob& source);

    /// Copy the specified 'source' into the specified 'destination'. Return
    /// the error.
    static ntsa::Error copyBlobBuffer(bsl::streambuf*          destination,
                                      const bdlbb::BlobBuffer& source);

    /// Copy the specified 'source' into the specified 'destination'. Return
    /// the error.
    static ntsa::Error copyConstBuffer(bsl::streambuf*          destination,
                                       const ntsa::ConstBuffer& source);

    /// Copy the specified 'source' into the specified 'destination'. Return
    /// the error.
    static ntsa::Error copyConstBufferArray(
        bsl::streambuf*               destination,
        const ntsa::ConstBufferArray& source);

    /// Copy the specified 'source' into the specified 'destination'. Return
    /// the error.
    static ntsa::Error copyConstBufferPtrArray(
        bsl::streambuf*                  destination,
        const ntsa::ConstBufferPtrArray& source);

    /// Copy the specified 'source' into the specified 'destination'. Return
    /// the error.
    static ntsa::Error copyMutableBuffer(bsl::streambuf* destination,
                                         const ntsa::MutableBuffer& source);

    /// Copy the specified 'source' into the specified 'destination'. Return
    /// the error.
    static ntsa::Error copyMutableBufferArray(
        bsl::streambuf*                 destination,
        const ntsa::MutableBufferArray& source);

    /// Copy the specified 'source' into the specified 'destination'. Return
    /// the error.
    static ntsa::Error copyMutableBufferPtrArray(
        bsl::streambuf*                    destination,
        const ntsa::MutableBufferPtrArray& source);

    /// Copy the specified 'source' into the specified 'destination'. Return
    /// the error.
    static ntsa::Error copyFile(bsl::streambuf*   destination,
                                const ntsa::File& source);

    /// Copy the specified 'source' into the specified 'destination'. Return
    /// the error.
    static ntsa::Error copyString(bsl::streambuf*    destination,
                                  const bsl::string& source);

    static const DataUtil::Impl::Append s_append[12];
    static const DataUtil::Impl::Pop    s_pop[12];
};

bsl::size_t DataUtil::Impl::appendUndefined(bdlbb::Blob*      destination,
                                            const ntsa::Data& source)
{
    NTSCFG_WARNING_UNUSED(destination);
    NTSCFG_WARNING_UNUSED(source);

    return 0;
}

bsl::size_t DataUtil::Impl::appendBlobBuffer(bdlbb::Blob*      destination,
                                             const ntsa::Data& source)
{
    const bdlbb::BlobBuffer& blobBuffer = source.blobBuffer();

    destination->appendDataBuffer(blobBuffer);
    return blobBuffer.size();
}

bsl::size_t DataUtil::Impl::appendConstBuffer(bdlbb::Blob*      destination,
                                              const ntsa::Data& source)
{
    const ntsa::ConstBuffer& constBuffer = source.constBuffer();

    bdlbb::BlobUtil::append(destination,
                            static_cast<const char*>(constBuffer.data()),
                            NTSCFG_WARNING_NARROW(int, constBuffer.size()));

    return constBuffer.size();
}

bsl::size_t DataUtil::Impl::appendConstBufferArray(bdlbb::Blob* destination,
                                                   const ntsa::Data& source)
{
    const ntsa::ConstBufferArray& constBufferArray = source.constBufferArray();

    bsl::size_t total = 0;

    const bsl::size_t numBuffers = constBufferArray.numBuffers();
    for (bsl::size_t i = 0; i < numBuffers; ++i) {
        const ntsa::ConstBuffer& constBuffer = constBufferArray.buffer(i);

        bdlbb::BlobUtil::append(
            destination,
            static_cast<const char*>(constBuffer.data()),
            NTSCFG_WARNING_NARROW(int, constBuffer.size()));

        total += constBuffer.size();
    }

    return total;
}

bsl::size_t DataUtil::Impl::appendConstBufferPtrArray(bdlbb::Blob* destination,
                                                      const ntsa::Data& source)
{
    const ntsa::ConstBufferPtrArray& constBufferPtrArray =
        source.constBufferPtrArray();

    bsl::size_t total = 0;

    const bsl::size_t numBuffers = constBufferPtrArray.numBuffers();
    for (bsl::size_t i = 0; i < numBuffers; ++i) {
        const ntsa::ConstBuffer& constBuffer = constBufferPtrArray.buffer(i);

        bdlbb::BlobUtil::append(
            destination,
            static_cast<const char*>(constBuffer.data()),
            NTSCFG_WARNING_NARROW(int, constBuffer.size()));

        total += constBuffer.size();
    }

    return total;
}

bsl::size_t DataUtil::Impl::appendMutableBuffer(bdlbb::Blob*      destination,
                                                const ntsa::Data& source)
{
    const ntsa::MutableBuffer& mutableBuffer = source.mutableBuffer();

    bdlbb::BlobUtil::append(destination,
                            static_cast<const char*>(mutableBuffer.data()),
                            NTSCFG_WARNING_NARROW(int, mutableBuffer.size()));

    return mutableBuffer.size();
}

bsl::size_t DataUtil::Impl::appendMutableBufferArray(bdlbb::Blob* destination,
                                                     const ntsa::Data& source)
{
    const ntsa::MutableBufferArray& mutableBufferArray =
        source.mutableBufferArray();

    bsl::size_t total = 0;

    const bsl::size_t numBuffers = mutableBufferArray.numBuffers();
    for (bsl::size_t i = 0; i < numBuffers; ++i) {
        const ntsa::MutableBuffer& mutableBuffer =
            mutableBufferArray.buffer(i);

        bdlbb::BlobUtil::append(
            destination,
            static_cast<const char*>(mutableBuffer.data()),
            NTSCFG_WARNING_NARROW(int, mutableBuffer.size()));

        total += mutableBuffer.size();
    }

    return total;
}

bsl::size_t DataUtil::Impl::appendMutableBufferPtrArray(
    bdlbb::Blob*      destination,
    const ntsa::Data& source)
{
    const ntsa::MutableBufferPtrArray& mutableBufferPtrArray =
        source.mutableBufferPtrArray();

    bsl::size_t total = 0;

    const bsl::size_t numBuffers = mutableBufferPtrArray.numBuffers();
    for (bsl::size_t i = 0; i < numBuffers; ++i) {
        const ntsa::MutableBuffer& mutableBuffer =
            mutableBufferPtrArray.buffer(i);

        bdlbb::BlobUtil::append(
            destination,
            static_cast<const char*>(mutableBuffer.data()),
            NTSCFG_WARNING_NARROW(int, mutableBuffer.size()));

        total += mutableBuffer.size();
    }

    return total;
}

bsl::size_t DataUtil::Impl::appendBlob(bdlbb::Blob*      destination,
                                       const ntsa::Data& source)
{
    const bdlbb::Blob& blob = source.blob();

    bdlbb::BlobUtil::append(destination, blob);
    return blob.length();
}

bsl::size_t DataUtil::Impl::appendSharedBlob(bdlbb::Blob*      destination,
                                             const ntsa::Data& source)
{
    const bsl::shared_ptr<bdlbb::Blob>& sharedBlob = source.sharedBlob();

    bdlbb::BlobUtil::append(destination, *sharedBlob);
    return sharedBlob->length();
}

bsl::size_t DataUtil::Impl::appendString(bdlbb::Blob*      destination,
                                         const ntsa::Data& source)
{
    const bsl::string& string = source.string();

    bdlbb::BlobUtil::append(destination,
                            string.c_str(),
                            NTSCFG_WARNING_NARROW(int, string.size()));
    return string.size();
}

bsl::size_t DataUtil::Impl::appendFile(bdlbb::Blob*      destination,
                                       const ntsa::Data& source)
{
    const ntsa::File& file = source.file();

    bsl::size_t originalLength = destination->length();

    destination->trimLastDataBuffer();
    destination->setLength(
        NTSCFG_WARNING_NARROW(int, originalLength + file.bytesRemaining()));
    destination->setLength(NTSCFG_WARNING_NARROW(int, originalLength));

    BSLS_ASSERT(destination->totalSize() - destination->length() >=
                file.bytesRemaining());

    BSLS_ASSERT(destination->numBuffers() > destination->numDataBuffers());

    bsl::size_t bufferIndex = destination->numDataBuffers() == 0
                                  ? 0
                                  : destination->numDataBuffers() + 1;

    bdls::FilesystemUtil::FileDescriptor fileDescriptor = file.descriptor();

    bdls::FilesystemUtil::Offset fileBytesRemaining = file.bytesRemaining();
    bdls::FilesystemUtil::Offset fileBytesRead      = 0;

    bdls::FilesystemUtil::Offset fileOffset = bdls::FilesystemUtil::seek(
        fileDescriptor,
        file.position(),
        bdls::FilesystemUtil::e_SEEK_FROM_BEGINNING);

    if (fileOffset != file.position()) {
        return 0;
    }

    while (fileBytesRemaining > 0) {
        BSLS_ASSERT(
            bufferIndex <
            NTSCFG_WARNING_PROMOTE(bsl::size_t, destination->numBuffers()));

        const bdlbb::BlobBuffer* buffer =
            &destination->buffer(NTSCFG_WARNING_NARROW(int, bufferIndex));

        bdls::FilesystemUtil::Offset bufferBytesAvailable = buffer->size();

        bdls::FilesystemUtil::Offset numBytesToRead =
            bsl::min(fileBytesRemaining, bufferBytesAvailable);

        int numBytesRead = bdls::FilesystemUtil::read(
            fileDescriptor,
            buffer->data(),
            NTSCFG_WARNING_NARROW(int, numBytesToRead));

        if (numBytesRead <= 0) {
            break;
        }

        fileBytesRead += numBytesRead;
        BSLS_ASSERT(fileBytesRead <= file.bytesRemaining());

        BSLS_ASSERT(fileBytesRemaining >= numBytesRead);
        fileBytesRemaining -= numBytesRead;

        if (numBytesRead != numBytesToRead) {
            break;
        }

        ++bufferIndex;
    }

    destination->setLength(
        NTSCFG_WARNING_NARROW(int, originalLength + fileBytesRead));

    return NTSCFG_WARNING_NARROW(bsl::size_t, fileBytesRead);
}

// *** Pop ***

void DataUtil::Impl::popUndefined(ntsa::Data* data, bsl::size_t numBytes)
{
    NTSCFG_WARNING_UNUSED(data);
    NTSCFG_WARNING_UNUSED(numBytes);
}

void DataUtil::Impl::popBlobBuffer(ntsa::Data* data, bsl::size_t numBytes)
{
    bdlbb::BlobBuffer* blobBuffer = &data->blobBuffer();

    bsl::shared_ptr<char> oldBuffer = blobBuffer->buffer();
    bsl::size_t           oldSize   = blobBuffer->size();

    if (numBytes > oldSize) {
        numBytes = oldSize;
    }

    bsl::shared_ptr<char> newBuffer(oldBuffer, oldBuffer.get() + numBytes);

    BSLS_ASSERT(oldSize >= numBytes);
    bsl::size_t newSize = oldSize - numBytes;

    blobBuffer->reset(newBuffer, NTSCFG_WARNING_NARROW(int, newSize));
}

void DataUtil::Impl::popConstBuffer(ntsa::Data* data, bsl::size_t numBytes)
{
    ntsa::ConstBuffer* constBuffer = &data->constBuffer();

    constBuffer->advance(numBytes);
}

void DataUtil::Impl::popConstBufferArray(ntsa::Data* data,
                                         bsl::size_t numBytes)
{
    ntsa::ConstBufferArray* constBufferArray = &data->constBufferArray();

    constBufferArray->pop(numBytes);
}

void DataUtil::Impl::popConstBufferPtrArray(ntsa::Data* data,
                                            bsl::size_t numBytes)
{
    ntsa::ConstBufferPtrArray* constBufferPtrArray =
        &data->constBufferPtrArray();

    constBufferPtrArray->pop(numBytes);
}

void DataUtil::Impl::popMutableBuffer(ntsa::Data* data, bsl::size_t numBytes)
{
    ntsa::MutableBuffer* mutableBuffer = &data->mutableBuffer();

    mutableBuffer->advance(numBytes);
}

void DataUtil::Impl::popMutableBufferArray(ntsa::Data* data,
                                           bsl::size_t numBytes)
{
    ntsa::MutableBufferArray* mutableBufferArray = &data->mutableBufferArray();

    mutableBufferArray->pop(numBytes);
}

void DataUtil::Impl::popMutableBufferPtrArray(ntsa::Data* data,
                                              bsl::size_t numBytes)
{
    ntsa::MutableBufferPtrArray* mutableBufferPtrArray =
        &data->mutableBufferPtrArray();

    mutableBufferPtrArray->pop(numBytes);
}

void DataUtil::Impl::popBlob(ntsa::Data* data, bsl::size_t numBytes)
{
    bdlbb::Blob* blob = &data->blob();

    if (numBytes > NTSCFG_WARNING_PROMOTE(bsl::size_t, blob->length())) {
        numBytes = blob->length();
    }

    bdlbb::BlobUtil::erase(blob, 0, NTSCFG_WARNING_NARROW(int, numBytes));
}

void DataUtil::Impl::popSharedBlob(ntsa::Data* data, bsl::size_t numBytes)
{
    bsl::shared_ptr<bdlbb::Blob>* sharedBlob = &data->sharedBlob();

    if (numBytes >
        NTSCFG_WARNING_PROMOTE(bsl::size_t, (*sharedBlob)->length()))
    {
        numBytes = (*sharedBlob)->length();
    }

    bdlbb::BlobUtil::erase(sharedBlob->get(),
                           0,
                           NTSCFG_WARNING_NARROW(int, numBytes));
}

void DataUtil::Impl::popString(ntsa::Data* data, bsl::size_t numBytes)
{
    bsl::string* string = &data->string();

    if (numBytes > string->size()) {
        numBytes = string->size();
    }

    string->erase(0, numBytes);
}

void DataUtil::Impl::popFile(ntsa::Data* data, bsl::size_t numBytes)
{
    ntsa::File* file = &data->file();

    bdls::FilesystemUtil::Offset numBytesToPop =
        static_cast<bdls::FilesystemUtil::Offset>(numBytes);

    if (numBytesToPop > file->bytesRemaining()) {
        numBytesToPop = file->bytesRemaining();
    }

    file->setPosition(file->position() + numBytesToPop);
    file->setBytesRemaining(file->bytesRemaining() - numBytesToPop);
}

ntsa::Error DataUtil::Impl::copyBlob(bsl::streambuf*    destination,
                                     const bdlbb::Blob& source)
{
    ntsa::Error error;

    const int numDataBuffers = source.numDataBuffers();

    for (int i = 0; i < numDataBuffers; ++i) {
        const bdlbb::BlobBuffer& buffer = source.buffer(i);

        int size;
        if (i != numDataBuffers - 1) {
            size = buffer.size();
        }
        else {
            size = source.lastDataBufferLength();
        }

        const char* data = buffer.data();

        error = DataUtil::Impl::copyConstBuffer(destination,
                                                ntsa::ConstBuffer(data, size));
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error DataUtil::Impl::copyBlobBuffer(bsl::streambuf* destination,
                                           const bdlbb::BlobBuffer& source)
{
    return DataUtil::Impl::copyConstBuffer(
        destination,
        ntsa::ConstBuffer(source.data(), source.size()));
}

ntsa::Error DataUtil::Impl::copyConstBuffer(bsl::streambuf* destination,
                                            const ntsa::ConstBuffer& source)
{
    const std::streamsize numBytesToCopyMax =
        bsl::numeric_limits<std::streamsize>::max();

    const char* currentDataSource =
        reinterpret_cast<const char*>(source.data());

    bsl::size_t numBytesRemaining = source.size();

    while (true) {
        if (numBytesRemaining == 0) {
            break;
        }

        std::streamsize numBytesToCopy;
        if (numBytesRemaining <= static_cast<bsl::size_t>(numBytesToCopyMax)) {
            numBytesToCopy = static_cast<std::streamsize>(numBytesRemaining);
        }
        else {
            numBytesToCopy = numBytesToCopyMax;
        }

        std::streamsize numBytesCopied =
            destination->sputn(currentDataSource, numBytesToCopy);

        currentDataSource += numBytesCopied;
        numBytesRemaining -= numBytesCopied;

        BSLS_ASSERT(numBytesCopied <= numBytesToCopy);

        if (numBytesCopied != numBytesToCopy) {
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }
    }

    return ntsa::Error();
}

ntsa::Error DataUtil::Impl::copyConstBufferArray(
    bsl::streambuf*               destination,
    const ntsa::ConstBufferArray& source)
{
    ntsa::Error error;

    const bsl::size_t numBuffers = source.numBuffers();

    for (bsl::size_t i = 0; i < numBuffers; ++i) {
        error = DataUtil::Impl::copyConstBuffer(destination, source.buffer(i));
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error DataUtil::Impl::copyConstBufferPtrArray(
    bsl::streambuf*                  destination,
    const ntsa::ConstBufferPtrArray& source)
{
    ntsa::Error error;

    const bsl::size_t numBuffers = source.numBuffers();

    for (bsl::size_t i = 0; i < numBuffers; ++i) {
        error = DataUtil::Impl::copyConstBuffer(destination, source.buffer(i));
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error DataUtil::Impl::copyMutableBuffer(
    bsl::streambuf*            destination,
    const ntsa::MutableBuffer& source)
{
    return DataUtil::Impl::copyConstBuffer(destination,
                                           ntsa::ConstBuffer(source));
}

ntsa::Error DataUtil::Impl::copyMutableBufferArray(
    bsl::streambuf*                 destination,
    const ntsa::MutableBufferArray& source)
{
    ntsa::Error error;

    const bsl::size_t numBuffers = source.numBuffers();

    for (bsl::size_t i = 0; i < numBuffers; ++i) {
        error = DataUtil::Impl::copyConstBuffer(
            destination,
            ntsa::ConstBuffer(source.buffer(i)));
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error DataUtil::Impl::copyMutableBufferPtrArray(
    bsl::streambuf*                    destination,
    const ntsa::MutableBufferPtrArray& source)
{
    ntsa::Error error;

    const bsl::size_t numBuffers = source.numBuffers();

    for (bsl::size_t i = 0; i < numBuffers; ++i) {
        error = DataUtil::Impl::copyConstBuffer(
            destination,
            ntsa::ConstBuffer(source.buffer(i)));
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error DataUtil::Impl::copyFile(bsl::streambuf*   destination,
                                     const ntsa::File& source)
{
    ntsa::Error error;

    bdls::FilesystemUtil::FileDescriptor fileDescriptor = source.descriptor();

    if (fileDescriptor == bdls::FilesystemUtil::k_INVALID_FD) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bdls::FilesystemUtil::Offset currentPosition = bdls::FilesystemUtil::seek(
        fileDescriptor,
        source.position(),
        bdls::FilesystemUtil::e_SEEK_FROM_BEGINNING);

    if (currentPosition != source.position()) {
        error = ntsa::Error::last();
        if (!error) {
            error = ntsa::Error(ntsa::Error::e_EOF);
        }
        return error;
    }

    bdls::FilesystemUtil::Offset numBytesRemaining = source.bytesRemaining();

    while (true) {
        if (numBytesRemaining == 0) {
            break;
        }

        enum { k_BUFFER_SIZE = 512 };
        char buffer[k_BUFFER_SIZE];

        int numBytesToRead;
        if (numBytesRemaining <
            static_cast<bdls::FilesystemUtil::Offset>(k_BUFFER_SIZE))
        {
            numBytesToRead = static_cast<int>(numBytesRemaining);
        }
        else {
            numBytesToRead = static_cast<int>(k_BUFFER_SIZE);
        }

        int numBytesRead = bdls::FilesystemUtil::read(fileDescriptor,
                                                      &buffer,
                                                      numBytesToRead);

        BSLS_ASSERT(numBytesRead <= numBytesToRead);

        if (numBytesRead > 0) {
            error = DataUtil::Impl::copyConstBuffer(
                destination,
                ntsa::ConstBuffer(buffer,
                                  static_cast<bsl::size_t>(numBytesRead)));
            if (error) {
                return error;
            }

            BSLS_ASSERT(numBytesRemaining >= numBytesRead);
            numBytesRemaining -= numBytesRead;
        }

        if (numBytesRead != numBytesToRead) {
            error = ntsa::Error::last();
            if (!error) {
                error = ntsa::Error(ntsa::Error::e_EOF);
            }
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error DataUtil::Impl::copyString(bsl::streambuf*    destination,
                                       const bsl::string& source)
{
    return DataUtil::Impl::copyConstBuffer(
        destination,
        ntsa::ConstBuffer(source.data(), source.size()));
}

const DataUtil::Impl::Append DataUtil::Impl::s_append[12] = {
    (DataUtil::Impl::Append)&DataUtil::Impl::appendUndefined,
    (DataUtil::Impl::Append)&DataUtil::Impl::appendBlobBuffer,
    (DataUtil::Impl::Append)&DataUtil::Impl::appendConstBuffer,
    (DataUtil::Impl::Append)&DataUtil::Impl::appendConstBufferArray,
    (DataUtil::Impl::Append)&DataUtil::Impl::appendConstBufferPtrArray,
    (DataUtil::Impl::Append)&DataUtil::Impl::appendMutableBuffer,
    (DataUtil::Impl::Append)&DataUtil::Impl::appendMutableBufferArray,
    (DataUtil::Impl::Append)&DataUtil::Impl::appendMutableBufferPtrArray,
    (DataUtil::Impl::Append)&DataUtil::Impl::appendBlob,
    (DataUtil::Impl::Append)&DataUtil::Impl::appendSharedBlob,
    (DataUtil::Impl::Append)&DataUtil::Impl::appendString,
    (DataUtil::Impl::Append)&DataUtil::Impl::appendFile};

const DataUtil::Impl::Pop DataUtil::Impl::s_pop[12] = {
    (DataUtil::Impl::Pop)&DataUtil::Impl::popUndefined,
    (DataUtil::Impl::Pop)&DataUtil::Impl::popBlobBuffer,
    (DataUtil::Impl::Pop)&DataUtil::Impl::popConstBuffer,
    (DataUtil::Impl::Pop)&DataUtil::Impl::popConstBufferArray,
    (DataUtil::Impl::Pop)&DataUtil::Impl::popConstBufferPtrArray,
    (DataUtil::Impl::Pop)&DataUtil::Impl::popMutableBuffer,
    (DataUtil::Impl::Pop)&DataUtil::Impl::popMutableBufferArray,
    (DataUtil::Impl::Pop)&DataUtil::Impl::popMutableBufferPtrArray,
    (DataUtil::Impl::Pop)&DataUtil::Impl::popBlob,
    (DataUtil::Impl::Pop)&DataUtil::Impl::popSharedBlob,
    (DataUtil::Impl::Pop)&DataUtil::Impl::popString,
    (DataUtil::Impl::Pop)&DataUtil::Impl::popFile};

bsl::size_t DataUtil::append(bdlbb::Blob*      destination,
                             const ntsa::Data& source)
{
    return DataUtil::Impl::s_append[source.type()](destination, source);
}

void DataUtil::pop(ntsa::Data* data, bsl::size_t numBytes)
{
    DataUtil::Impl::s_pop[data->type()](data, numBytes);
}

ntsa::Error DataUtil::copy(bsl::streambuf*   destination,
                           const ntsa::Data& source)
{
    if (source.isUndefined()) {
        return ntsa::Error();
    }
    else if (source.isBlob()) {
        return DataUtil::Impl::copyBlob(destination, source.blob());
    }
    else if (source.isBlobBuffer()) {
        return DataUtil::Impl::copyBlobBuffer(destination,
                                              source.blobBuffer());
    }
    else if (source.isConstBuffer()) {
        return DataUtil::Impl::copyConstBuffer(destination,
                                               source.constBuffer());
    }
    else if (source.isConstBufferArray()) {
        return DataUtil::Impl::copyConstBufferArray(destination,
                                                    source.constBufferArray());
    }
    else if (source.isConstBufferPtrArray()) {
        return DataUtil::Impl::copyConstBufferPtrArray(
            destination,
            source.constBufferPtrArray());
    }
    else if (source.isMutableBuffer()) {
        return DataUtil::Impl::copyMutableBuffer(destination,
                                                 source.mutableBuffer());
    }
    else if (source.isMutableBufferArray()) {
        return DataUtil::Impl::copyMutableBufferArray(
            destination,
            source.mutableBufferArray());
    }
    else if (source.isMutableBufferPtrArray()) {
        return DataUtil::Impl::copyMutableBufferPtrArray(
            destination,
            source.mutableBufferPtrArray());
    }
    else if (source.isFile()) {
        return DataUtil::Impl::copyFile(destination, source.file());
    }
    else if (source.isString()) {
        return DataUtil::Impl::copyString(destination, source.string());
    }
    else if (source.isSharedBlob()) {
        if (source.sharedBlob()) {
            return DataUtil::Impl::copyBlob(destination, *source.sharedBlob());
        }
        else {
            return ntsa::Error();
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }
}

ntsa::Error DataUtil::copy(ntsa::Data* destination, const ntsa::Data& source)
{
    destination->reset();

    if (destination->blobBufferFactory() != 0) {
        return DataUtil::copy(&destination->makeBlob(), source);
    }
    else {
        bdlbb::SimpleBlobBufferFactory blobBufferFactory(
            1024,
            bslma::Default::globalAllocator());

        bdlbb::Blob blob(&blobBufferFactory);

        ntsa::Error error = DataUtil::copy(&blob, source);
        destination->makeBlob(blob);

        BSLS_ASSERT(destination->blobBufferFactory() == 0);

        return error;
    }
}

ntsa::Error DataUtil::copy(bdlbb::Blob* destination, const ntsa::Data& source)
{
    ntsa::Error error;

    destination->setLength(0);
    BSLS_ASSERT(destination->length() == 0);

    bdlbb::OutBlobStreamBuf osb(destination);
    error = DataUtil::copy(&osb, source);
    osb.pubsync();

    if (error) {
        return error;
    }

    return ntsa::Error();
}

bool DataUtil::equals(const ntsa::Data& lhs, const ntsa::Data& rhs)
{
    ntsa::Error error;
    int         rc;

    const bsl::size_t lhsSize = lhs.size();
    const bsl::size_t rhsSize = rhs.size();

    if (lhsSize != rhsSize) {
        return false;
    }

    if (lhs.isBlob() && rhs.isBlob()) {
        rc = bdlbb::BlobUtil::compare(lhs.blob(), rhs.blob());
        return rc == 0;
    }
    else {
        bdlbb::SimpleBlobBufferFactory blobBufferFactory(
            8192,
            bslma::Default::globalAllocator());

        bdlbb::Blob lhsBlob(&blobBufferFactory);
        bdlbb::Blob rhsBlob(&blobBufferFactory);

        error = DataUtil::copy(&lhsBlob, lhs);
        if (error) {
            return false;
        }

        error = DataUtil::copy(&rhsBlob, rhs);
        if (error) {
            return false;
        }

        rc = bdlbb::BlobUtil::compare(lhsBlob, rhsBlob);
        return rc == 0;
    }
}

}  // close package namespace
}  // close enterprise namespace
