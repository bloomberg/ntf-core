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

#include <ntscfg_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcd_compression_t_cpp, "$Id$ $CSID$")

#include <ntcd_compression.h>

#include <ntccfg_bind.h>
#include <ntci_log.h>
#include <ntcs_blobutil.h>
#include <ntcs_datapool.h>
#include <bslim_printer.h>

// Uncomment to define the single buffer size tested.
// #define NTCD_COMPRESSION_TEST_BUFFER_SIZE 32

// Uncomment to define the single I/O transfer size tested.
// #define NTCD_COMPRESSION_TEST_IO_SIZE 32

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcd {

// Provide tests for 'ntcd::Compression'.
class CompressionTest
{
  public:
    // Describes the parameters of the test.
    class Parameters;

    // Verify a compression using a single buffer.
    static void verifyBasic();

    // Verify a compression using multiple buffers, feed into the inflater
    // in chunks, simulating the arrival of deflated data over a stream.
    static void verifyStreaming();

  private:
    // Verify compression using the specified testing 'parameters'.
    static void verifyParameters(const Parameters& parameters);

    // Dump the specified 'blob' to the log with the specified 'label'.
    static void dump(const char* label, const bdlbb::Blob& blob);
};

// Describes the parameters of the test.
class CompressionTest::Parameters
{
  public:
    // Enumerates the constants used by this implementation.
    enum Constant {
        k_DEFAULT_BLOB_BUFFER_SIZE = 8192,
        k_DEFAULT_WRITE_SIZE       = 1024,
        k_DEFAULT_READ_SIZE        = 64
    };

    // Create new parameters having the default value.
    Parameters();

    // Create new parameters having the same value as the specified 'original'
    // parameters.
    Parameters(const Parameters& original);

    // Destroy this object.
    ~Parameters();

    // Assign the value of the specified 'other' object to this object. Return
    // a reference to this modifiable value.
    Parameters& operator=(const Parameters& other);

    // Reset the value of this object to its value upon default construction.
    void reset();

    // Set the variation index to the specified 'value'.
    void setVariationIndex(bsl::size_t value);

    // Set the variation cout to the specified 'value'.
    void setVariationCount(bsl::size_t value);

    // Set the compression algorithm to the specified 'value'.
    void setType(ntca::CompressionType::Value value);

    // Set the compression goal to the specified 'value'.
    void setGoal(ntca::CompressionGoal::Value value);

    // Set the blob buffer size to the specified 'value'.
    void setBlobBufferSize(bsl::size_t value);

    // Set the write size to the specified 'value'.
    void setWriteSize(bsl::size_t value);

    // Set the read size to the specified 'value'.
    void setReadSize(bsl::size_t value);

    // Return the variation index.
    bsl::size_t variationIndex() const;

    // Return the variation count.
    bsl::size_t variationCount() const;

    // Return the compression algorithm.
    ntca::CompressionType::Value type() const;

    // Return the compression goal.
    ntca::CompressionGoal::Value goal() const;

    // Return the blob buffer size.
    bsl::size_t blobBufferSize() const;

    // Return the write size.
    bsl::size_t writeSize() const;

    // Return the read size.
    bsl::size_t readSize() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const Parameters& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const Parameters& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(Parameters);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(Parameters);

  private:
    ntca::CompressionType::Value d_type;
    ntca::CompressionGoal::Value d_goal;
    bsl::size_t                  d_variationIndex;
    bsl::size_t                  d_variationCount;
    bsl::size_t                  d_blobBufferSize;
    bsl::size_t                  d_writeSize;
    bsl::size_t                  d_readSize;
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::CompressionConfig
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const CompressionTest::Parameters& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::CompressionConfig
bool operator==(const CompressionTest::Parameters& lhs,
                const CompressionTest::Parameters& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::CompressionConfig
bool operator!=(const CompressionTest::Parameters& lhs,
                const CompressionTest::Parameters& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::CompressionConfig
bool operator<(const CompressionTest::Parameters& lhs,
               const CompressionTest::Parameters& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::CompressionConfig
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                    algorithm,
                const CompressionTest::Parameters& value);

CompressionTest::Parameters::Parameters()
: d_variationIndex(0)
, d_variationCount(0)
, d_type(ntca::CompressionType::e_ZLIB)
, d_goal(ntca::CompressionGoal::e_BALANCED)
, d_blobBufferSize(k_DEFAULT_BLOB_BUFFER_SIZE)
, d_writeSize(k_DEFAULT_WRITE_SIZE)
, d_readSize(k_DEFAULT_READ_SIZE)
{
}

CompressionTest::Parameters::Parameters(const Parameters& original)
: d_variationIndex(original.d_variationIndex)
, d_variationCount(original.d_variationCount)
, d_type(original.d_type)
, d_goal(original.d_goal)
, d_blobBufferSize(original.d_blobBufferSize)
, d_writeSize(original.d_writeSize)
, d_readSize(original.d_readSize)
{
}

CompressionTest::Parameters::~Parameters()
{
}

CompressionTest::Parameters& CompressionTest::Parameters::operator=(
    const Parameters& other)
{
    if (this != &other) {
        d_variationIndex = other.d_variationIndex;
        d_variationCount = other.d_variationCount;
        d_type           = other.d_type;
        d_goal           = other.d_goal;
        d_blobBufferSize = other.d_blobBufferSize;
        d_writeSize      = other.d_writeSize;
        d_readSize       = other.d_readSize;
    }

    return *this;
}

void CompressionTest::Parameters::reset()
{
    d_variationIndex = 0;
    d_variationCount = 0;
    d_type           = ntca::CompressionType::e_ZLIB;
    d_goal           = ntca::CompressionGoal::e_BALANCED;
    d_blobBufferSize = k_DEFAULT_BLOB_BUFFER_SIZE;
    d_writeSize      = k_DEFAULT_WRITE_SIZE;
    d_readSize       = k_DEFAULT_READ_SIZE;
}

void CompressionTest::Parameters::setVariationIndex(bsl::size_t value)
{
    d_variationIndex = value;
}

void CompressionTest::Parameters::setVariationCount(bsl::size_t value)
{
    d_variationCount = value;
}

void CompressionTest::Parameters::setType(ntca::CompressionType::Value value)
{
    d_type = value;
}

void CompressionTest::Parameters::setGoal(ntca::CompressionGoal::Value value)
{
    d_goal = value;
}

void CompressionTest::Parameters::setBlobBufferSize(bsl::size_t value)
{
    d_blobBufferSize = value;
}

void CompressionTest::Parameters::setWriteSize(bsl::size_t value)
{
    d_writeSize = value;
}

void CompressionTest::Parameters::setReadSize(bsl::size_t value)
{
    d_readSize = value;
}

bsl::size_t CompressionTest::Parameters::variationIndex() const
{
    return d_variationIndex;
}

bsl::size_t CompressionTest::Parameters::variationCount() const
{
    return d_variationCount;
}

ntca::CompressionType::Value CompressionTest::Parameters::type() const
{
    return d_type;
}

ntca::CompressionGoal::Value CompressionTest::Parameters::goal() const
{
    return d_goal;
}

bsl::size_t CompressionTest::Parameters::blobBufferSize() const
{
    return d_blobBufferSize;
}

bsl::size_t CompressionTest::Parameters::writeSize() const
{
    return d_writeSize;
}

bsl::size_t CompressionTest::Parameters::readSize() const
{
    return d_readSize;
}

bool CompressionTest::Parameters::equals(const Parameters& other) const
{
    return d_variationIndex == other.d_variationIndex &&
           d_variationCount == other.d_variationCount &&
           d_type == other.d_type && d_goal == other.d_goal &&
           d_blobBufferSize == other.d_blobBufferSize &&
           d_writeSize == other.d_writeSize && d_readSize == other.d_readSize;
}

bool CompressionTest::Parameters::less(const Parameters& other) const
{
    if (d_variationIndex < other.d_variationIndex) {
        return true;
    }

    if (other.d_variationIndex < d_variationIndex) {
        return false;
    }

    if (d_variationCount < other.d_variationCount) {
        return true;
    }

    if (other.d_variationCount < d_variationCount) {
        return false;
    }

    if (d_type < other.d_type) {
        return true;
    }

    if (other.d_type < d_type) {
        return false;
    }

    if (d_goal < other.d_goal) {
        return true;
    }

    if (other.d_goal < d_goal) {
        return false;
    }

    if (d_blobBufferSize < other.d_blobBufferSize) {
        return true;
    }

    if (other.d_blobBufferSize < d_blobBufferSize) {
        return false;
    }

    if (d_writeSize < other.d_writeSize) {
        return true;
    }

    if (other.d_writeSize < d_writeSize) {
        return false;
    }

    return d_readSize < other.d_readSize;
}

template <typename HASH_ALGORITHM>
void CompressionTest::Parameters::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_variationIndex);
    hashAppend(algorithm, d_variationCount);
    hashAppend(algorithm, d_type);
    hashAppend(algorithm, d_goal);
    hashAppend(algorithm, d_blobBufferSize);
    hashAppend(algorithm, d_writeSize);
    hashAppend(algorithm, d_readSize);
}

bsl::ostream& CompressionTest::Parameters::print(bsl::ostream& stream,
                                                 int           level,
                                                 int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("variationIndex", d_variationIndex);
    printer.printAttribute("variationCount", d_variationCount);
    printer.printAttribute("type", d_type);
    printer.printAttribute("goal", d_goal);
    printer.printAttribute("blobBufferSize", d_blobBufferSize);
    printer.printAttribute("writeSize", d_writeSize);
    printer.printAttribute("readSize", d_readSize);
    printer.end();
    return stream;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const CompressionTest::Parameters& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const CompressionTest::Parameters& lhs,
                const CompressionTest::Parameters& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const CompressionTest::Parameters& lhs,
                const CompressionTest::Parameters& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const CompressionTest::Parameters& lhs,
               const CompressionTest::Parameters& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&                    algorithm,
                              const CompressionTest::Parameters& value)
{
    value.hash(algorithm);
}

void CompressionTest::verifyParameters(const Parameters& parameters)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    NTCI_LOG_STREAM_DEBUG << "Testing " << parameters << NTCI_LOG_STREAM_END;

    ntca::CompressionConfig compressionConfig;
    compressionConfig.setType(parameters.type());
    compressionConfig.setGoal(parameters.goal());

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(NTSCFG_TEST_ALLOCATOR,
                           parameters.blobBufferSize(),
                           parameters.blobBufferSize(),
                           NTSCFG_TEST_ALLOCATOR);

    ntcd::Compression compression(compressionConfig,
                                  dataPool,
                                  NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<bdlbb::Blob> originalBlob = dataPool->createIncomingBlob();

    ntscfg::TestDataUtil::generateData(
        originalBlob.get(),
        parameters.writeSize(),
        0,
        ntscfg::TestDataUtil::k_DATASET_CLIENT_COMPRESSABLE);

    bsl::shared_ptr<bdlbb::Blob> inflatedBlob = dataPool->createIncomingBlob();
    bsl::shared_ptr<bdlbb::Blob> deflatedBlob = dataPool->createOutgoingBlob();

    ntcd::CompressionTest::dump("O", *originalBlob);

    ntca::DeflateOptions deflateOptions;
    ntca::DeflateContext deflateContext;

    error = compression.deflate(&deflateContext,
                                deflatedBlob.get(),
                                *originalBlob,
                                deflateOptions);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Deflate context  = " << deflateContext
                          << NTCI_LOG_STREAM_END;

    ntcd::CompressionTest::dump("D", *deflatedBlob);

    bdlbb::Blob readBlob = *deflatedBlob;
    while (readBlob.length() > 0) {
        bsl::size_t chunkSize = parameters.readSize();
        if (chunkSize > readBlob.length()) {
            chunkSize = readBlob.length();
        }

        bdlbb::Blob chunk;
        bdlbb::BlobUtil::append(&chunk, readBlob, 0, chunkSize);
        bdlbb::BlobUtil::erase(&readBlob, 0, chunkSize);

        ntca::InflateOptions inflateOptions;
        ntca::InflateContext inflateContext;

        error = compression.inflate(&inflateContext,
                                    inflatedBlob.get(),
                                    chunk,
                                    inflateOptions);
        NTSCFG_TEST_OK(error);

        NTCI_LOG_STREAM_DEBUG << "Inflate context  = " << inflateContext
                              << NTCI_LOG_STREAM_END;
    }

    ntcd::CompressionTest::dump("I", *inflatedBlob);

    int comparison = bdlbb::BlobUtil::compare(*inflatedBlob, *originalBlob);
    NTSCFG_TEST_EQ(comparison, 0);
}

void CompressionTest::dump(const char* label, const bdlbb::Blob& blob)
{
    NTCI_LOG_CONTEXT();

    bsl::stringstream ss;
    ss << label << ":\n" << bdlbb::BlobUtilHexDumper(&blob);

    NTCI_LOG_TRACE("%s", ss.str().c_str());
}

NTSCFG_TEST_FUNCTION(ntcd::CompressionTest::verifyBasic)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    const bsl::size_t incomingBufferSize = 256;
    const bsl::size_t outgoingBufferSize = 256;

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(NTSCFG_TEST_ALLOCATOR,
                           incomingBufferSize,
                           outgoingBufferSize,
                           NTSCFG_TEST_ALLOCATOR);

    bsl::vector<ntca::CompressionType::Value> algorithmVector;
    algorithmVector.push_back(ntca::CompressionType::e_RLE);

    for (bsl::size_t i = 0; i < algorithmVector.size(); ++i) {
        ntca::CompressionConfig compressionConfig;
        compressionConfig.setType(algorithmVector[i]);
        compressionConfig.setGoal(ntca::CompressionGoal::e_BALANCED);

        ntcd::Compression compression(compressionConfig,
                                      dataPool,
                                      NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<bdlbb::Blob> originalData =
            dataPool->createOutgoingBlob();

        ntscfg::TestDataUtil::generateData(
            originalData.get(),
            outgoingBufferSize,
            0,
            ntscfg::TestDataUtil::k_DATASET_CLIENT_COMPRESSABLE);

        ntcd::CompressionTest::dump("O", *originalData);

        bsl::shared_ptr<bdlbb::Blob> deflatedData =
            dataPool->createOutgoingBlob();

        ntca::DeflateOptions deflateOptions;
        ntca::DeflateContext deflateContext;

        error = compression.deflate(&deflateContext,
                                    deflatedData.get(),
                                    *originalData,
                                    deflateOptions);
        NTSCFG_TEST_OK(error);

        NTCI_LOG_STREAM_DEBUG << "Deflate context " << deflateContext
                              << NTCI_LOG_STREAM_END;

        ntcd::CompressionTest::dump("D", *deflatedData);

        NTSCFG_TEST_EQ(deflateContext.bytesRead(),
                       ntcs::BlobUtil::size(originalData));

        NTSCFG_TEST_EQ(deflateContext.bytesWritten(),
                       ntcs::BlobUtil::size(deflatedData));

        bsl::shared_ptr<bdlbb::Blob> inflatedData =
            dataPool->createIncomingBlob();

        ntca::InflateOptions inflateOptions;
        ntca::InflateContext inflateContext;

        error = compression.inflate(&inflateContext,
                                    inflatedData.get(),
                                    *deflatedData,
                                    inflateOptions);
        NTSCFG_TEST_OK(error);

        NTCI_LOG_STREAM_DEBUG << "Inflate context " << inflateContext
                              << NTCI_LOG_STREAM_END;

        ntcd::CompressionTest::dump("I", *inflatedData);

        NTSCFG_TEST_EQ(inflateContext.bytesRead(),
                       ntcs::BlobUtil::size(deflatedData));

        NTSCFG_TEST_EQ(inflateContext.bytesWritten(),
                       ntcs::BlobUtil::size(inflatedData));
    }
}

NTSCFG_TEST_FUNCTION(ntcd::CompressionTest::verifyStreaming)
{
    bsl::vector<ntca::CompressionType::Value> algorithmVector;
    algorithmVector.push_back(ntca::CompressionType::e_RLE);

    bsl::vector<ntca::CompressionGoal::Value> goalVector;
    goalVector.push_back(ntca::CompressionGoal::e_BALANCED);

    bsl::vector<bsl::size_t> bufferSizeVector;
#if NTCD_COMPRESSION_TEST_BUFFER_SIZE
    bufferSizeVector.push_back(NTCD_COMPRESSION_TEST_BUFFER_SIZE);
#else
    bufferSizeVector.push_back(1);
    bufferSizeVector.push_back(2);
    bufferSizeVector.push_back(3);
    bufferSizeVector.push_back(4);
    bufferSizeVector.push_back(8);
    bufferSizeVector.push_back(16);
    bufferSizeVector.push_back(32);
    bufferSizeVector.push_back(64);
#endif

    bsl::vector<bsl::size_t> ioSizeVector;
#if NTCD_COMPRESSION_TEST_IO_SIZE
    ioSizeVector.push_back(NTCD_COMPRESSION_TEST_IO_SIZE);
#else
    ioSizeVector.push_back(1);
    ioSizeVector.push_back(2);
    ioSizeVector.push_back(3);
    ioSizeVector.push_back(4);
    ioSizeVector.push_back(8);
    ioSizeVector.push_back(16);
    ioSizeVector.push_back(32);
    ioSizeVector.push_back(64);
#endif

    bsl::vector<CompressionTest::Parameters> parametersVector;
    bsl::size_t                              variationIndex = 0;

    for (bsl::size_t i = 0; i < algorithmVector.size(); ++i) {
        for (bsl::size_t j = 0; j < goalVector.size(); ++j) {
            for (bsl::size_t k = 0; k < bufferSizeVector.size(); ++k) {
                for (bsl::size_t l = 0; l < ioSizeVector.size(); ++l) {
                    for (bsl::size_t m = 0; m < ioSizeVector.size(); ++m) {
                        CompressionTest::Parameters parameters;
                        parameters.setVariationIndex(variationIndex++);
                        parameters.setType(algorithmVector[i]);
                        parameters.setGoal(goalVector[j]);
                        parameters.setBlobBufferSize(bufferSizeVector[k]);
                        parameters.setWriteSize(ioSizeVector[l]);
                        parameters.setReadSize(ioSizeVector[m]);

                        parametersVector.push_back(parameters);
                    }
                }
            }
        }
    }

    for (bsl::size_t i = 0; i < parametersVector.size(); ++i) {
        parametersVector[i].setVariationCount(parametersVector.size());
    }

    for (bsl::size_t i = 0; i < parametersVector.size(); ++i) {
        CompressionTest::verifyParameters(parametersVector[i]);
    }
}

}  // close namespace ntcd
}  // close namespace BloombergLP
