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

#ifndef INCLUDED_NTCD_COMPRESSION
#define INCLUDED_NTCD_COMPRESSION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_compression.h>
#include <ntci_datapool.h>
#include <ntcscm_version.h>
#include <ntsa_data.h>
#include <bdlb_bigendian.h>
#include <bdlb_nullablevalue.h>
#include <bdlbb_blob.h>
#include <bsls_atomic.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcd {

/// Provide a random access sequence over a blob.
class ByteSequence
{
    bdlbb::Blob* d_blob_p;
    int          d_base;
    bsl::uint8_t d_sentinel;

  private:
    ByteSequence(const ByteSequence&) BSLS_KEYWORD_DELETED;
    ByteSequence& operator=(const ByteSequence&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new writable random access sequence over the specified 'blob'.
    /// Offset all indexed access and manipulation by the specified 'base'
    /// (for example, to append to the blob set 'base' to 'blob->length()'.
    explicit ByteSequence(bdlbb::Blob* blob, bsl::size_t base = 0);

    /// Destroy this object.
    ~ByteSequence();

    // Return the modifiable byte at the specified 'index'.
    bsl::uint8_t& operator[](bsl::size_t index);

    // Return the non-modifiable byte at the specified 'index'.
    bsl::uint8_t operator[](bsl::size_t index) const;
};

/// Provide a description of a compression frame header.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b magic:
/// The 4-byte character code that identifies the frame header in a data
/// stream.
///
/// @li @b length:
/// The number of bytes in the content (i.e. sequence of blocks) in the frame.
/// Note that the length does not include the length header nor the length of
/// the footer.
///
/// @li @b flags:
/// The flags defining the behavior of the deflater that produced the frame.
///
/// @li @b checksum:
/// The initial checksum.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_todo
class CompressionFrameHeader
{
    bsl::uint32_t d_magic;
    bsl::uint32_t d_length;
    bsl::uint32_t d_flags;
    bsl::uint32_t d_checksum;

  public:
    /// Create a new compression frame header having the default value.
    CompressionFrameHeader();

    /// Create a new compression frame header having the same value as the
    /// specified 'original' object.
    CompressionFrameHeader(const CompressionFrameHeader& original);

    /// Destroy this object.
    ~CompressionFrameHeader();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    CompressionFrameHeader& operator=(const CompressionFrameHeader& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the 4-byte character code that identifies the frame header in a
    /// data stream to the specified 'value'.
    void setMagic(bsl::uint32_t value);

    /// Set the number of bytes in the frame to the specified 'value'.
    void setLength(bsl::uint32_t value);

    /// Set the flags defining the behavior of the deflater that produced the
    /// frame to the specified 'value'.
    void setFlags(bsl::uint32_t value);

    /// Set the initial checksum to the specified 'value'.
    void setChecksum(const ntca::Checksum& value);

    /// Decode the object from the specified 'source' having the specified
    /// 'size'. Increment the specified 'numBytesDecoded' with the number of
    /// bytes read from the specified 'source'. Return the error.
    ntsa::Error decode(bsl::size_t* numBytesDecoded,
                       const void*  source,
                       bsl::size_t  size);

    /// Decode the object from the specified 'source'. Increment the specified
    /// 'numBytesDecoded' with the number of bytes read from the specified
    /// 'source'. Return the error.
    ntsa::Error decode(bsl::size_t* numBytesDecoded, bsl::streambuf* source);

    /// Decode the object from the specified 'source'. Increment the specified
    /// 'numBytesDecoded' with the number of bytes read from the specified
    /// 'source'. Return the error.
    ntsa::Error decode(bsl::size_t*       numBytesDecoded,
                       const bdlbb::Blob& source);

    /// Encode the object to the specified 'destination'. Increment the
    /// specified 'numBytesEncoded' with the number of bytes written to the
    /// 'destination'. Return the error.
    ntsa::Error encode(bsl::size_t* numBytesEncoded,
                       bdlbb::Blob* destination) const;

    /// Encode the object to the specified 'destination' at the specified
    /// 'position'. Increment the specified 'numBytesEncoded' with the number
    /// of bytes written to the 'destination'. Return the error.
    ntsa::Error encode(bsl::size_t* numBytesEncoded,
                       bdlbb::Blob* destination,
                       std::size_t  position) const;

    /// Validate the state of this object. Return the error.
    ntsa::Error validate() const;

    /// Return the 4-byte character code that identifies the frame header in a
    /// data stream.
    bsl::uint32_t magic() const;

    /// Return the number of bytes in the frame.
    bsl::uint32_t length() const;

    /// Return the flags defining the behavior of the deflater that produced
    /// the frame.
    bsl::uint32_t flags() const;

    /// Return the initial checksum.
    bsl::uint32_t checksum() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const CompressionFrameHeader& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const CompressionFrameHeader& other) const;

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

    /// The magic value that identifies the frame header in a data stream.
    static const bsl::uint32_t k_MAGIC;

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(CompressionFrameHeader);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(CompressionFrameHeader);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntcd::CompressionFrameHeader
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const CompressionFrameHeader& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntcd::CompressionFrameHeader
bool operator==(const CompressionFrameHeader& lhs,
                const CompressionFrameHeader& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntcd::CompressionFrameHeader
bool operator!=(const CompressionFrameHeader& lhs,
                const CompressionFrameHeader& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntcd::CompressionFrameHeader
bool operator<(const CompressionFrameHeader& lhs,
               const CompressionFrameHeader& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntcd::CompressionFrameHeader
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&               algorithm,
                const CompressionFrameHeader& value);

/// Provide a description of a compression frame footer.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b magic:
/// The 4-byte character code that identifies the frame footer in a data
/// stream.
///
/// @li @b checksum:
/// The checksum of the uncompressed bytes.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_todo
class CompressionFrameFooter
{
    bsl::uint32_t d_magic;
    bsl::uint32_t d_checksum;

  public:
    /// Create a new compression frame footer having the default value.
    CompressionFrameFooter();

    /// Create a new compression frame footer having the same value as the
    /// specified 'original' object.
    CompressionFrameFooter(const CompressionFrameFooter& original);

    /// Destroy this object.
    ~CompressionFrameFooter();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    CompressionFrameFooter& operator=(const CompressionFrameFooter& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the 4-byte character code that identifies the frame footer in a
    /// data stream to the specified 'value'.
    void setMagic(bsl::uint32_t value);

    /// Set the checksum of the uncompressed bytes to the specified 'value'.
    void setChecksum(const ntca::Checksum& value);

    /// Decode the object from the specified 'source' having the specified
    /// 'size'. Increment the specified 'numBytesDecoded' with the number of
    /// bytes read from the specified 'source'. Return the error.
    ntsa::Error decode(bsl::size_t* numBytesDecoded,
                       const void*  source,
                       bsl::size_t  size);

    /// Decode the object from the specified 'source'. Increment the specified
    /// 'numBytesDecoded' with the number of bytes read from the specified
    /// 'source'. Return the error.
    ntsa::Error decode(bsl::size_t* numBytesDecoded, bsl::streambuf* source);

    /// Decode the object from the specified 'source'. Increment the specified
    /// 'numBytesDecoded' with the number of bytes read from the specified
    /// 'source'. Return the error.
    ntsa::Error decode(bsl::size_t*       numBytesDecoded,
                       const bdlbb::Blob& source);

    /// Encode the object to the specified 'destination'. Increment the
    /// specified 'numBytesEncoded' with the number of bytes written to the
    /// 'destination'. Return the error.
    ntsa::Error encode(bsl::size_t* numBytesEncoded,
                       bdlbb::Blob* destination) const;

    /// Validate the state of this object. Return the error.
    ntsa::Error validate() const;

    /// Return the 4-byte character code that identifies the frame footer in a
    /// data stream.
    bsl::uint32_t magic() const;

    /// Return the checksum of the uncompressed bytes.
    bsl::uint32_t checksum() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const CompressionFrameFooter& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const CompressionFrameFooter& other) const;

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

    /// The magic value that identifies the frame footer in a data stream.
    static const bsl::uint32_t k_MAGIC;

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(CompressionFrameFooter);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(CompressionFrameFooter);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntcd::CompressionFrameFooter
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const CompressionFrameFooter& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntcd::CompressionFrameFooter
bool operator==(const CompressionFrameFooter& lhs,
                const CompressionFrameFooter& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntcd::CompressionFrameFooter
bool operator!=(const CompressionFrameFooter& lhs,
                const CompressionFrameFooter& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntcd::CompressionFrameFooter
bool operator<(const CompressionFrameFooter& lhs,
               const CompressionFrameFooter& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntcd::CompressionFrameFooter
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&               algorithm,
                const CompressionFrameFooter& value);

/// Provide an enumeration of the compression block types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_todo
class CompressionBlockType
{
  public:
    /// Provide an enumeration of the compression block types.
    enum Value {
        /// The block type is undefined.
        e_UNDEFINED = 0,

        /// The block represents the original, inflated data, with no
        /// compression applied.
        e_RAW = 1,

        // The block represents a run-length encoded literal.
        e_RLE = 2
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntcd::CompressionBlockType
bsl::ostream& operator<<(bsl::ostream&               stream,
                         CompressionBlockType::Value rhs);

/// Provide a description of a compression block.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b length:
/// The number of bytes in the block.
///
/// @li @b literal:
/// The literal value associated with the block.
///
/// @li @b flags:
/// The characteristics of the block.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_todo
class CompressionBlock
{
    bsl::uint16_t d_length;
    bsl::uint8_t  d_literal;
    bsl::uint8_t  d_flags;

  public:
    /// Create a new compression block having the default value.
    CompressionBlock();

    /// Create a new compression block having the same value as the specified
    /// 'original' object.
    CompressionBlock(const CompressionBlock& original);

    /// Destroy this object.
    ~CompressionBlock();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    CompressionBlock& operator=(const CompressionBlock& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the type of the block to the specified 'value'.
    void setType(ntcd::CompressionBlockType::Value value);

    /// Set the number of bytes in the block to the specified 'value'.
    void setLength(bsl::uint16_t value);

    /// Set the literal associated with the specified 'block' to the specified
    /// 'value'.
    void setLiteral(bsl::uint8_t value);

    /// Decode the object from the specified 'source' having the specified
    /// 'size'. Increment the specified 'numBytesDecoded' with the number of
    /// bytes read from the specified 'source'. Return the error.
    ntsa::Error decode(bsl::size_t* numBytesDecoded,
                       const void*  source,
                       bsl::size_t  size);

    /// Decode the object from the specified 'source'. Increment the specified
    /// 'numBytesDecoded' with the number of bytes read from the specified
    /// 'source'. Return the error.
    ntsa::Error decode(bsl::size_t* numBytesDecoded, bsl::streambuf* source);

    /// Decode the object from the specified 'source'. Increment the specified
    /// 'numBytesDecoded' with the number of bytes read from the specified
    /// 'source'. Return the error.
    ntsa::Error decode(bsl::size_t*       numBytesDecoded,
                       const bdlbb::Blob& source);

    /// Encode the object to the specified 'destination'. Increment the
    /// specified 'numBytesEncoded' with the number of bytes written to the
    /// 'destination'. Return the error.
    ntsa::Error encode(bsl::size_t* numBytesEncoded,
                       bdlbb::Blob* destination) const;

    /// Validate the state of this object. Return the error.
    ntsa::Error validate() const;

    /// Return the type of the block.
    ntcd::CompressionBlockType::Value type() const;

    /// Return the number of bytes in the bloack.
    bsl::uint16_t length() const;

    /// Return the literal associated with the block.
    bsl::uint8_t literal() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const CompressionBlock& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const CompressionBlock& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(CompressionBlock);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(CompressionBlock);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntcd::CompressionBlock
bsl::ostream& operator<<(bsl::ostream& stream, const CompressionBlock& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntcd::CompressionBlock
bool operator==(const CompressionBlock& lhs, const CompressionBlock& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntcd::CompressionBlock
bool operator!=(const CompressionBlock& lhs, const CompressionBlock& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntcd::CompressionBlock
bool operator<(const CompressionBlock& lhs, const CompressionBlock& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntcd::CompressionBlock
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const CompressionBlock& value);

/// @internal @brief
/// Provide an RLE encoder.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class CompressionEncoderRle
{
    ntcd::CompressionFrameHeader d_frameHeader;
    bsl::size_t                  d_frameHeaderPosition;
    bsl::size_t                  d_frameContentBytesTotal;
    ntca::Checksum               d_frameContentCrc;
    ntca::CompressionConfig      d_config;
    bslma::Allocator*            d_allocator_p;

  private:
    CompressionEncoderRle(const CompressionEncoderRle&) BSLS_KEYWORD_DELETED;
    CompressionEncoderRle& operator=(const CompressionEncoderRle&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new RLE encoder with to the specified 'configuration'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit CompressionEncoderRle(
        const ntca::CompressionConfig& configuration,
        bslma::Allocator*              basicAllocator = 0);

    /// Destroy this object.
    ~CompressionEncoderRle();

    /// Begin a deflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error deflateBegin(ntca::DeflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::DeflateOptions& options);

    /// Deflate the specified 'data' having the specified 'size' and append the
    /// result to the specified 'result'. Return the error.
    ntsa::Error deflateNext(ntca::DeflateContext*       context,
                            bdlbb::Blob*                result,
                            const bsl::uint8_t*         data,
                            bsl::size_t                 size,
                            const ntca::DeflateOptions& options);

    /// End a deflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error deflateEnd(ntca::DeflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntca::DeflateOptions& options);
};

/// @internal @brief
/// Provide an RLE decoder.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class CompressionDecoderRle
{
    /// Enumerates the states of decoding.
    enum State {
        /// The decoder wants to read the frame header.
        e_WANT_FRAME_HEADER,

        /// The decoder wants to read a block.
        e_WANT_BLOCK,

        /// The decoder wants to read a block payload.
        e_WANT_BLOCK_PAYLOAD,

        /// The decoder wants to read the frame footer.
        e_WANT_FRAME_FOOTER,

        /// The decoder encountered an error.
        e_ERROR
    };

    State                        d_state;
    bdlbb::Blob                  d_input;
    bsl::vector<bsl::uint8_t>    d_expansion;
    ntcd::CompressionFrameHeader d_frameHeader;
    bsl::size_t                  d_frameContentBytesNeeded;
    ntca::Checksum               d_frameContentCrc;
    ntcd::CompressionBlock       d_block;
    ntca::CompressionConfig      d_config;
    ntsa::Error                  d_error;
    bslma::Allocator*            d_allocator_p;

  private:
    CompressionDecoderRle(const CompressionDecoderRle&) BSLS_KEYWORD_DELETED;
    CompressionDecoderRle& operator=(const CompressionDecoderRle&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Process the input buffer according to the specified 'options' and
    /// append any deflated data to the specified 'result'. Return the error.
    ntsa::Error process(ntca::InflateContext*       context,
                        bdlbb::Blob*                result,
                        const ntca::InflateOptions& options);

    /// Fail the decoder with the specified 'error'. Return the 'error'.
    ntsa::Error fail(ntsa::Error error);

  public:
    /// Create a new RLE decoder with to the specified 'configuration'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit CompressionDecoderRle(
        const ntca::CompressionConfig& configuration,
        bslma::Allocator*              basicAllocator = 0);

    /// Destroy this object.
    ~CompressionDecoderRle();

    /// Begin an inflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error inflateBegin(ntca::InflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::InflateOptions& options);

    /// Inflate the specified 'data' having the specified 'size' and append the
    /// result to the specified 'result'. Return the error.
    ntsa::Error inflateNext(ntca::InflateContext*       context,
                            bdlbb::Blob*                result,
                            const bsl::uint8_t*         data,
                            bsl::size_t                 size,
                            const ntca::InflateOptions& options);

    /// Inflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error inflateNext(ntca::InflateContext*       context,
                            bdlbb::Blob*                result,
                            const bdlbb::Blob&          data,
                            const ntca::InflateOptions& options);

    /// End an inflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error inflateEnd(ntca::InflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntca::InflateOptions& options);
};

/// @internal @brief
/// Provide an LZ4 encoder.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class CompressionEncoderLz4
{
    ntca::CompressionConfig d_config;
    bslma::Allocator*       d_allocator_p;

  private:
    CompressionEncoderLz4(const CompressionEncoderLz4&) BSLS_KEYWORD_DELETED;
    CompressionEncoderLz4& operator=(const CompressionEncoderLz4&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new LZ4 encoder with to the specified 'configuration'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit CompressionEncoderLz4(
        const ntca::CompressionConfig& configuration,
        bslma::Allocator*              basicAllocator = 0);

    /// Destroy this object.
    ~CompressionEncoderLz4();

    /// Begin a deflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error deflateBegin(ntca::DeflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::DeflateOptions& options);

    /// Deflate the specified 'data' having the specified 'size' and append the
    /// result to the specified 'result'. Return the error.
    ntsa::Error deflateNext(ntca::DeflateContext*       context,
                            bdlbb::Blob*                result,
                            const bsl::uint8_t*         data,
                            bsl::size_t                 size,
                            const ntca::DeflateOptions& options);

    /// End a deflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error deflateEnd(ntca::DeflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntca::DeflateOptions& options);
};

/// @internal @brief
/// Provide an LZ4 decoder.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class CompressionDecoderLz4
{
    /// Enumerates the states of decoding.
    enum State {
        /// The decoder wants to read the frame header.
        e_WANT_FRAME_HEADER,

        /// The decoder wants to read a block.
        e_WANT_BLOCK,

        /// The decoder wants to read the frame footer.
        e_WANT_FRAME_FOOTER,

        /// The decoder encountered an error.
        e_ERROR
    };

    State                        d_state;
    ntcd::CompressionFrameHeader d_frameHeader;
    ntcd::CompressionBlock       d_block;
    ntcd::CompressionFrameFooter d_frameFooter;
    ntca::CompressionConfig      d_config;
    ntsa::Error                  d_error;
    bslma::Allocator*            d_allocator_p;

  private:
    CompressionDecoderLz4(const CompressionDecoderLz4&) BSLS_KEYWORD_DELETED;
    CompressionDecoderLz4& operator=(const CompressionDecoderLz4&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new LZ4 decoder with to the specified 'configuration'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit CompressionDecoderLz4(
        const ntca::CompressionConfig& configuration,
        bslma::Allocator*              basicAllocator = 0);

    /// Destroy this object.
    ~CompressionDecoderLz4();

    /// Begin an inflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error inflateBegin(ntca::InflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::InflateOptions& options);

    /// Inflate the specified 'data' having the specified 'size' and append the
    /// result to the specified 'result'. Return the error.
    ntsa::Error inflateNext(ntca::InflateContext*       context,
                            bdlbb::Blob*                result,
                            const bsl::uint8_t*         data,
                            bsl::size_t                 size,
                            const ntca::InflateOptions& options);

    /// Inflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error inflateNext(ntca::InflateContext*       context,
                            bdlbb::Blob*                result,
                            const bdlbb::Blob&          data,
                            const ntca::InflateOptions& options);

    /// End an inflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error inflateEnd(ntca::InflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntca::InflateOptions& options);
};

/// @internal @brief
/// Provide simple compressor for debugging.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class Compression : public ntci::Compression
{
    ntcd::CompressionEncoderRle     d_rleEncoder;
    ntcd::CompressionDecoderRle     d_rleDecoder;
    ntcd::CompressionEncoderLz4     d_lz4Encoder;
    ntcd::CompressionDecoderLz4     d_lz4Decoder;
    bsl::shared_ptr<ntci::DataPool> d_dataPool_sp;
    ntca::CompressionConfig         d_config;
    bslma::Allocator*               d_allocator_p;

  private:
    Compression(const Compression&) BSLS_KEYWORD_DELETED;
    Compression& operator=(const Compression&) BSLS_KEYWORD_DELETED;

  private:
    /// Begin a deflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error deflateBegin(ntca::DeflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::DeflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Deflate the specified 'data' having the specified 'size' and append the
    /// result to the specified 'result'. Return the error.
    ntsa::Error deflateNext(ntca::DeflateContext*       context,
                            bdlbb::Blob*                result,
                            const bsl::uint8_t*         data,
                            bsl::size_t                 size,
                            const ntca::DeflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// End a deflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error deflateEnd(ntca::DeflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntca::DeflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Begin an inflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error inflateBegin(ntca::InflateContext*       context,
                             bdlbb::Blob*                result,
                             const ntca::InflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Inflate the specified 'data' having the specified 'size' and append the
    /// result to the specified 'result'. Return the error.
    ntsa::Error inflateNext(ntca::InflateContext*       context,
                            bdlbb::Blob*                result,
                            const bsl::uint8_t*         data,
                            bsl::size_t                 size,
                            const ntca::InflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Inflate the specified 'data' and append the result to the specified
    /// 'result'. Return the error.
    ntsa::Error inflateNext(ntca::InflateContext*       context,
                            bdlbb::Blob*                result,
                            const bdlbb::Blob&          data,
                            const ntca::InflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// End an inflation stream into the specified 'result' according to the
    /// specified 'options'.
    ntsa::Error inflateEnd(ntca::InflateContext*       context,
                           bdlbb::Blob*                result,
                           const ntca::InflateOptions& options)
        BSLS_KEYWORD_OVERRIDE;

  public:
    /// Create a new mechanism to inflate and deflate data according to the
    /// specified 'configuration'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    Compression(const ntca::CompressionConfig&         configuration,
                const bsl::shared_ptr<ntci::DataPool>& dataPool,
                bslma::Allocator*                      basicAllocator = 0);

    /// Destroy this object.
    ~Compression() BSLS_KEYWORD_OVERRIDE;
};

NTSCFG_INLINE
CompressionFrameHeader::CompressionFrameHeader()
: d_magic(CompressionFrameHeader::k_MAGIC)
, d_length(0)
, d_flags(0)
, d_checksum(0)
{
}

NTSCFG_INLINE
CompressionFrameHeader::CompressionFrameHeader(
    const CompressionFrameHeader& original)
: d_magic(original.d_magic)
, d_length(original.d_length)
, d_flags(original.d_flags)
, d_checksum(original.d_checksum)
{
}

NTSCFG_INLINE
CompressionFrameHeader::~CompressionFrameHeader()
{
}

NTSCFG_INLINE
CompressionFrameHeader& CompressionFrameHeader::operator=(
    const CompressionFrameHeader& other)
{
    if (this != &other) {
        d_magic    = other.d_magic;
        d_length   = other.d_length;
        d_flags    = other.d_flags;
        d_checksum = other.d_checksum;
    }

    return *this;
}

NTSCFG_INLINE
void CompressionFrameHeader::reset()
{
    d_magic    = CompressionFrameHeader::k_MAGIC;
    d_length   = 0;
    d_flags    = 0;
    d_checksum = 0;
}

NTSCFG_INLINE
void CompressionFrameHeader::setMagic(bsl::uint32_t value)
{
    d_magic = value;
}

NTSCFG_INLINE
void CompressionFrameHeader::setLength(bsl::uint32_t value)
{
    d_length = value;
}

NTSCFG_INLINE
void CompressionFrameHeader::setFlags(bsl::uint32_t value)
{
    d_flags = value;
}

NTSCFG_INLINE
void CompressionFrameHeader::setChecksum(const ntca::Checksum& value)
{
    value.load(&d_checksum, sizeof d_checksum);
}

NTSCFG_INLINE
bsl::uint32_t CompressionFrameHeader::magic() const
{
    return d_magic;
}

NTSCFG_INLINE
bsl::uint32_t CompressionFrameHeader::length() const
{
    return d_length;
}

NTSCFG_INLINE
bsl::uint32_t CompressionFrameHeader::flags() const
{
    return d_flags;
}

NTSCFG_INLINE
bsl::uint32_t CompressionFrameHeader::checksum() const
{
    return d_checksum;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void CompressionFrameHeader::hash(
    HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_magic);
    hashAppend(algorithm, d_length);
    hashAppend(algorithm, d_flags);
    hashAppend(algorithm, d_checksum);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const CompressionFrameHeader& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const CompressionFrameHeader& lhs,
                const CompressionFrameHeader& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const CompressionFrameHeader& lhs,
                const CompressionFrameHeader& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const CompressionFrameHeader& lhs,
               const CompressionFrameHeader& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&               algorithm,
                              const CompressionFrameHeader& value)
{
    value.hash(algorithm);
}

NTSCFG_INLINE
CompressionFrameFooter::CompressionFrameFooter()
: d_magic(CompressionFrameFooter::k_MAGIC)
, d_checksum(0)
{
}

NTSCFG_INLINE
CompressionFrameFooter::CompressionFrameFooter(
    const CompressionFrameFooter& original)
: d_magic(original.d_magic)
, d_checksum(original.d_checksum)
{
}

NTSCFG_INLINE
CompressionFrameFooter::~CompressionFrameFooter()
{
}

NTSCFG_INLINE
CompressionFrameFooter& CompressionFrameFooter::operator=(
    const CompressionFrameFooter& other)
{
    if (this != &other) {
        d_magic    = other.d_magic;
        d_checksum = other.d_checksum;
    }

    return *this;
}

NTSCFG_INLINE
void CompressionFrameFooter::reset()
{
    d_magic    = CompressionFrameFooter::k_MAGIC;
    d_checksum = 0;
}

NTSCFG_INLINE
void CompressionFrameFooter::setMagic(bsl::uint32_t value)
{
    d_magic = value;
}

NTSCFG_INLINE
void CompressionFrameFooter::setChecksum(const ntca::Checksum& value)
{
    value.load(&d_checksum, sizeof d_checksum);
}

NTSCFG_INLINE
bsl::uint32_t CompressionFrameFooter::magic() const
{
    return d_magic;
}

NTSCFG_INLINE
bsl::uint32_t CompressionFrameFooter::checksum() const
{
    return d_checksum;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void CompressionFrameFooter::hash(
    HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_magic);
    hashAppend(algorithm, d_checksum);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const CompressionFrameFooter& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const CompressionFrameFooter& lhs,
                const CompressionFrameFooter& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const CompressionFrameFooter& lhs,
                const CompressionFrameFooter& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const CompressionFrameFooter& lhs,
               const CompressionFrameFooter& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&               algorithm,
                              const CompressionFrameFooter& value)
{
    value.hash(algorithm);
}

NTSCFG_INLINE
CompressionBlock::CompressionBlock()
: d_length(0)
, d_literal(0)
, d_flags(0)
{
}

NTSCFG_INLINE
CompressionBlock::CompressionBlock(const CompressionBlock& original)
: d_length(original.d_length)
, d_literal(original.d_literal)
, d_flags(original.d_flags)
{
}

NTSCFG_INLINE
CompressionBlock::~CompressionBlock()
{
}

NTSCFG_INLINE
CompressionBlock& CompressionBlock::operator=(const CompressionBlock& other)
{
    if (this != &other) {
        d_length  = other.d_length;
        d_literal = other.d_literal;
        d_flags   = other.d_flags;
    }

    return *this;
}

NTSCFG_INLINE
void CompressionBlock::reset()
{
    d_length  = 0;
    d_literal = 0;
    d_flags   = 0;
}

NTSCFG_INLINE
void CompressionBlock::setType(ntcd::CompressionBlockType::Value value)
{
    d_flags = static_cast<bsl::uint8_t>(static_cast<int>(value));
}

NTSCFG_INLINE
void CompressionBlock::setLength(bsl::uint16_t value)
{
    d_length = value;
}

NTSCFG_INLINE
void CompressionBlock::setLiteral(bsl::uint8_t value)
{
    d_literal = value;
}

NTSCFG_INLINE
ntcd::CompressionBlockType::Value CompressionBlock::type() const
{
    return static_cast<ntcd::CompressionBlockType::Value>(
        static_cast<int>(d_flags));
}

NTSCFG_INLINE
bsl::uint16_t CompressionBlock::length() const
{
    return d_length;
}

NTSCFG_INLINE
bsl::uint8_t CompressionBlock::literal() const
{
    return d_literal;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void CompressionBlock::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_length);
    hashAppend(algorithm, d_literal);
    hashAppend(algorithm, d_flags);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const CompressionBlock& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const CompressionBlock& lhs, const CompressionBlock& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const CompressionBlock& lhs, const CompressionBlock& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const CompressionBlock& lhs, const CompressionBlock& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&         algorithm,
                              const CompressionBlock& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
