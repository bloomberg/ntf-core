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
BSLS_IDENT_RCSID(ntctlc_plugin_t_cpp, "$Id$ $CSID$")

#include <ntci_log.h>
#include <ntcs_datapool.h>
#include <ntctlc_plugin.h>
#include <bslim_printer.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntctlc {

// Uncomment to test only a specific parameter variation.
// #define NTCTLC_PLUGIN_TEST_VARIATION 1339

// Provide tests for 'ntctlc::Plugin'.
class PluginTest
{
  public:
    // Describes the parameters of the test.
    class Parameters;

    // TODO.
    static void verifyRle();

    // TODO.
    static void verifyZlib();

    // TODO.
    static void verifyGzip();

    // TODO.
    static void verifyLz4();

    // TODO.
    static void verifyZstd();

    // TODO.
    static void verifyAll();

    // TODO.
    static void verifyUsage();

  private:
    /// Verify the integrity of inflating and deflating a data stream
    /// according to the specified 'parameters'.
    static void verifyParameters(const Parameters& parameters);

    /// Declare the log category for this class.
    BALL_LOG_SET_CLASS_CATEGORY("NTC.COMPRESSION");
};

// Describes the parameters of the test.
class PluginTest::Parameters
{
  public:
    // Enumerates the constants used by this implementation.
    enum Constant {
        k_DEFAULT_BLOB_BUFFER_SIZE = 8192,
        k_DEFAULT_MESSAGE_COUNT    = 1,
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

    // Set the number of messages to deflate to the specified 'value'.
    void setMessageCount(bsl::size_t value);

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

    // Return the number of messages to deflate.
    bsl::size_t messageCount() const;

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
    bsl::size_t                  d_messageCount;
    bsl::size_t                  d_writeSize;
    bsl::size_t                  d_readSize;
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::CompressionConfig
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const PluginTest::Parameters& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::CompressionConfig
bool operator==(const PluginTest::Parameters& lhs,
                const PluginTest::Parameters& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::CompressionConfig
bool operator!=(const PluginTest::Parameters& lhs,
                const PluginTest::Parameters& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::CompressionConfig
bool operator<(const PluginTest::Parameters& lhs,
               const PluginTest::Parameters& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::CompressionConfig
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&               algorithm,
                const PluginTest::Parameters& value);

PluginTest::Parameters::Parameters()
: d_variationIndex(0)
, d_variationCount(0)
, d_type(ntca::CompressionType::e_ZLIB)
, d_goal(ntca::CompressionGoal::e_BALANCED)
, d_blobBufferSize(k_DEFAULT_BLOB_BUFFER_SIZE)
, d_messageCount(k_DEFAULT_MESSAGE_COUNT)
, d_writeSize(k_DEFAULT_WRITE_SIZE)
, d_readSize(k_DEFAULT_READ_SIZE)
{
}

PluginTest::Parameters::Parameters(const Parameters& original)
: d_variationIndex(original.d_variationIndex)
, d_variationCount(original.d_variationCount)
, d_type(original.d_type)
, d_goal(original.d_goal)
, d_blobBufferSize(original.d_blobBufferSize)
, d_messageCount(original.d_messageCount)
, d_writeSize(original.d_writeSize)
, d_readSize(original.d_readSize)
{
}

PluginTest::Parameters::~Parameters()
{
}

PluginTest::Parameters& PluginTest::Parameters::operator=(
    const Parameters& other)
{
    if (this != &other) {
        d_variationIndex = other.d_variationIndex;
        d_variationCount = other.d_variationCount;
        d_type           = other.d_type;
        d_goal           = other.d_goal;
        d_blobBufferSize = other.d_blobBufferSize;
        d_messageCount   = other.d_messageCount;
        d_writeSize      = other.d_writeSize;
        d_readSize       = other.d_readSize;
    }

    return *this;
}

void PluginTest::Parameters::reset()
{
    d_variationIndex = 0;
    d_variationCount = 0;
    d_type           = ntca::CompressionType::e_ZLIB;
    d_goal           = ntca::CompressionGoal::e_BALANCED;
    d_blobBufferSize = k_DEFAULT_BLOB_BUFFER_SIZE;
    d_messageCount   = k_DEFAULT_MESSAGE_COUNT;
    d_writeSize      = k_DEFAULT_WRITE_SIZE;
    d_readSize       = k_DEFAULT_READ_SIZE;
}

void PluginTest::Parameters::setVariationIndex(bsl::size_t value)
{
    d_variationIndex = value;
}

void PluginTest::Parameters::setVariationCount(bsl::size_t value)
{
    d_variationCount = value;
}

void PluginTest::Parameters::setType(ntca::CompressionType::Value value)
{
    d_type = value;
}

void PluginTest::Parameters::setGoal(ntca::CompressionGoal::Value value)
{
    d_goal = value;
}

void PluginTest::Parameters::setBlobBufferSize(bsl::size_t value)
{
    d_blobBufferSize = value;
}

void PluginTest::Parameters::setMessageCount(bsl::size_t value)
{
    d_messageCount = value;
}

void PluginTest::Parameters::setWriteSize(bsl::size_t value)
{
    d_writeSize = value;
}

void PluginTest::Parameters::setReadSize(bsl::size_t value)
{
    d_readSize = value;
}

bsl::size_t PluginTest::Parameters::variationIndex() const
{
    return d_variationIndex;
}

bsl::size_t PluginTest::Parameters::variationCount() const
{
    return d_variationCount;
}

ntca::CompressionType::Value PluginTest::Parameters::type() const
{
    return d_type;
}

ntca::CompressionGoal::Value PluginTest::Parameters::goal() const
{
    return d_goal;
}

bsl::size_t PluginTest::Parameters::blobBufferSize() const
{
    return d_blobBufferSize;
}

bsl::size_t PluginTest::Parameters::messageCount() const
{
    return d_messageCount;
}

bsl::size_t PluginTest::Parameters::writeSize() const
{
    return d_writeSize;
}

bsl::size_t PluginTest::Parameters::readSize() const
{
    return d_readSize;
}

bool PluginTest::Parameters::equals(const Parameters& other) const
{
    return d_variationIndex == other.d_variationIndex &&
           d_variationCount == other.d_variationCount &&
           d_type == other.d_type && d_goal == other.d_goal &&
           d_blobBufferSize == other.d_blobBufferSize &&
           d_messageCount == other.d_messageCount &&
           d_writeSize == other.d_writeSize && d_readSize == other.d_readSize;
}

bool PluginTest::Parameters::less(const Parameters& other) const
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

    if (d_messageCount < other.d_messageCount) {
        return true;
    }

    if (other.d_messageCount < d_messageCount) {
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
void PluginTest::Parameters::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_variationIndex);
    hashAppend(algorithm, d_variationCount);
    hashAppend(algorithm, d_type);
    hashAppend(algorithm, d_goal);
    hashAppend(algorithm, d_blobBufferSize);
    hashAppend(algorithm, d_messageCount);
    hashAppend(algorithm, d_writeSize);
    hashAppend(algorithm, d_readSize);
}

bsl::ostream& PluginTest::Parameters::print(bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("variationIndex", d_variationIndex);
    printer.printAttribute("variationCount", d_variationCount);
    printer.printAttribute("type", d_type);
    printer.printAttribute("goal", d_goal);
    printer.printAttribute("blobBufferSize", d_blobBufferSize);
    printer.printAttribute("messageCount", d_messageCount);
    printer.printAttribute("writeSize", d_writeSize);
    printer.printAttribute("readSize", d_readSize);
    printer.end();
    return stream;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const PluginTest::Parameters& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const PluginTest::Parameters& lhs,
                const PluginTest::Parameters& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const PluginTest::Parameters& lhs,
                const PluginTest::Parameters& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const PluginTest::Parameters& lhs,
               const PluginTest::Parameters& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&               algorithm,
                              const PluginTest::Parameters& value)
{
    value.hash(algorithm);
}

void PluginTest::verifyParameters(const Parameters& parameters)
{
    ntsa::Error error;

    BALL_LOG_INFO << "Testing " << parameters << BALL_LOG_END;

    bsl::shared_ptr<ntci::CompressionDriver> driver;
    ntctlc::Plugin::load(&driver);

    ntca::CompressionConfig compressionConfig;
    compressionConfig.setType(parameters.type());
    compressionConfig.setGoal(parameters.goal());

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(NTSCFG_TEST_ALLOCATOR,
                           parameters.blobBufferSize(),
                           parameters.blobBufferSize(),
                           NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Compression> compression;
    error = driver->createCompression(&compression,
                                      compressionConfig,
                                      dataPool,
                                      NTSCFG_TEST_ALLOCATOR);
    NTSCFG_TEST_OK(error);

    typedef bsl::vector<bsl::shared_ptr<bdlbb::Blob> > BlobVector;

    BlobVector originalMessageBlobVector;

    bsl::shared_ptr<bdlbb::Blob> originalBlob = dataPool->createIncomingBlob();

    NTSCFG_TEST_GT(parameters.messageCount(), 0);

    for (bsl::size_t i = 0; i < parameters.messageCount(); ++i) {
        bsl::shared_ptr<bdlbb::Blob> originalMessage =
            dataPool->createIncomingBlob();

        const bsl::size_t dataset =
            i % 2 == 0 ? ntscfg::TestDataUtil::k_DATASET_CLIENT_COMPRESSABLE
                       : ntscfg::TestDataUtil::k_DATASET_SERVER_COMPRESSABLE;

        ntscfg::TestDataUtil::generateData(originalMessage.get(),
                                           parameters.writeSize(),
                                           0,
                                           dataset);

        originalMessageBlobVector.push_back(originalMessage);
        bdlbb::BlobUtil::append(originalBlob.get(), *originalMessage);
    }

    bsl::shared_ptr<bdlbb::Blob> inflatedBlob = dataPool->createIncomingBlob();
    bsl::shared_ptr<bdlbb::Blob> deflatedBlob = dataPool->createOutgoingBlob();

    BALL_LOG_TRACE << "I:\n"
                   << bdlbb::BlobUtilHexDumper(originalBlob.get())
                   << BALL_LOG_END;

    NTSCFG_TEST_EQ(originalMessageBlobVector.size(),
                   parameters.messageCount());

    for (bsl::size_t i = 0; i < parameters.messageCount(); ++i) {
        ntca::DeflateOptions deflateOptions;
        ntca::DeflateContext deflateContext;

        error = compression->deflate(&deflateContext,
                                     deflatedBlob.get(),
                                     *originalMessageBlobVector[i],
                                     deflateOptions);
        NTSCFG_TEST_OK(error);

        BALL_LOG_DEBUG << "Deflate context = " << deflateContext
                       << BALL_LOG_END;
    }

    BALL_LOG_TRACE << "D:\n"
                   << bdlbb::BlobUtilHexDumper(deflatedBlob.get())
                   << BALL_LOG_END;

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

        error = compression->inflate(&inflateContext,
                                     inflatedBlob.get(),
                                     chunk,
                                     inflateOptions);
        NTSCFG_TEST_OK(error);

        BALL_LOG_DEBUG << "Inflate context = " << inflateContext
                       << BALL_LOG_END;
    }

    BALL_LOG_TRACE << "F:\n"
                   << bdlbb::BlobUtilHexDumper(inflatedBlob.get())
                   << BALL_LOG_END;

    int comparison = bdlbb::BlobUtil::compare(*inflatedBlob, *originalBlob);
    NTSCFG_TEST_EQ(comparison, 0);
}

NTSCFG_TEST_FUNCTION(ntctlc::PluginTest::verifyRle)
{
    ntctlc::PluginTest::Parameters parameters;
    parameters.setVariationIndex(0);
    parameters.setVariationCount(1);
    parameters.setType(ntca::CompressionType::e_RLE);
    parameters.setGoal(ntca::CompressionGoal::e_BALANCED);
    parameters.setBlobBufferSize(512);
    parameters.setWriteSize(1024);
    parameters.setReadSize(16);
    parameters.setMessageCount(2);

    ntctlc::PluginTest::verifyParameters(parameters);
}

NTSCFG_TEST_FUNCTION(ntctlc::PluginTest::verifyZlib)
{
#if NTC_BUILD_WITH_ZLIB
    ntctlc::PluginTest::Parameters parameters;
    parameters.setVariationIndex(0);
    parameters.setVariationCount(1);
    parameters.setType(ntca::CompressionType::e_ZLIB);
    parameters.setGoal(ntca::CompressionGoal::e_BALANCED);
    parameters.setBlobBufferSize(512);
    parameters.setWriteSize(1024);
    parameters.setReadSize(16);
    parameters.setMessageCount(2);

    ntctlc::PluginTest::verifyParameters(parameters);
#endif
}

NTSCFG_TEST_FUNCTION(ntctlc::PluginTest::verifyGzip)
{
#if NTC_BUILD_WITH_ZLIB
    ntctlc::PluginTest::Parameters parameters;
    parameters.setVariationIndex(0);
    parameters.setVariationCount(1);
    parameters.setType(ntca::CompressionType::e_GZIP);
    parameters.setGoal(ntca::CompressionGoal::e_BALANCED);
    parameters.setBlobBufferSize(512);
    parameters.setWriteSize(1024);
    parameters.setReadSize(16);
    parameters.setMessageCount(2);

    ntctlc::PluginTest::verifyParameters(parameters);
#endif
}

NTSCFG_TEST_FUNCTION(ntctlc::PluginTest::verifyLz4)
{
#if NTC_BUILD_WITH_LZ4
    ntctlc::PluginTest::Parameters parameters;
    parameters.setVariationIndex(0);
    parameters.setVariationCount(1);
    parameters.setType(ntca::CompressionType::e_LZ4);
    parameters.setGoal(ntca::CompressionGoal::e_BALANCED);
    parameters.setBlobBufferSize(512);
    parameters.setWriteSize(1024);
    parameters.setReadSize(16);
    parameters.setMessageCount(2);

    ntctlc::PluginTest::verifyParameters(parameters);
#endif
}

NTSCFG_TEST_FUNCTION(ntctlc::PluginTest::verifyZstd)
{
#if NTC_BUILD_WITH_ZSTD
    ntctlc::PluginTest::Parameters parameters;
    parameters.setVariationIndex(0);
    parameters.setVariationCount(1);
    parameters.setType(ntca::CompressionType::e_ZSTD);
    parameters.setGoal(ntca::CompressionGoal::e_BALANCED);
    parameters.setBlobBufferSize(512);
    parameters.setWriteSize(1024);
    parameters.setReadSize(16);
    parameters.setMessageCount(2);

    ntctlc::PluginTest::verifyParameters(parameters);
#endif
}

NTSCFG_TEST_FUNCTION(ntctlc::PluginTest::verifyAll)
{
    bsl::vector<ntca::CompressionType::Value> algorithmVector;

    // algorithmVector.push_back(ntca::CompressionType::e_RLE);

#if NTC_BUILD_WITH_LZ4
    algorithmVector.push_back(ntca::CompressionType::e_LZ4);
#endif

#if NTC_BUILD_WITH_ZSTD
    algorithmVector.push_back(ntca::CompressionType::e_ZSTD);
#endif

#if NTC_BUILD_WITH_ZLIB
    algorithmVector.push_back(ntca::CompressionType::e_ZLIB);
    algorithmVector.push_back(ntca::CompressionType::e_GZIP);
#endif

    bsl::vector<ntca::CompressionGoal::Value> goalVector;
    goalVector.push_back(ntca::CompressionGoal::e_BALANCED);

    bsl::vector<bsl::size_t> bufferSizeVector;
    bufferSizeVector.push_back(16);
    bufferSizeVector.push_back(17);
    bufferSizeVector.push_back(18);
    bufferSizeVector.push_back(19);
    bufferSizeVector.push_back(20);
    bufferSizeVector.push_back(21);
    bufferSizeVector.push_back(22);
    bufferSizeVector.push_back(23);
    bufferSizeVector.push_back(24);
    bufferSizeVector.push_back(25);
    bufferSizeVector.push_back(26);
    bufferSizeVector.push_back(27);
    bufferSizeVector.push_back(28);
    bufferSizeVector.push_back(29);
    bufferSizeVector.push_back(30);
    bufferSizeVector.push_back(31);
    bufferSizeVector.push_back(32);
    bufferSizeVector.push_back(64);
    bufferSizeVector.push_back(1024);
    bufferSizeVector.push_back(1024 * 4);
    bufferSizeVector.push_back(1024 * 8);
    bufferSizeVector.push_back(1024 * 32);
    bufferSizeVector.push_back(1024 * 128);

    bsl::vector<bsl::size_t> ioSizeVector;
    ioSizeVector.push_back(1);
    ioSizeVector.push_back(2);
    ioSizeVector.push_back(3);
    ioSizeVector.push_back(4);
    ioSizeVector.push_back(5);
    ioSizeVector.push_back(6);
    ioSizeVector.push_back(7);
    ioSizeVector.push_back(8);
    ioSizeVector.push_back(9);
    ioSizeVector.push_back(10);
    ioSizeVector.push_back(11);
    ioSizeVector.push_back(12);
    ioSizeVector.push_back(13);
    ioSizeVector.push_back(14);
    ioSizeVector.push_back(15);
    ioSizeVector.push_back(16);
    ioSizeVector.push_back(32);
    ioSizeVector.push_back(64);
    ioSizeVector.push_back(1024);
    ioSizeVector.push_back(1024 * 4);
    ioSizeVector.push_back(1024 * 8);
    ioSizeVector.push_back(1024 * 32);
    ioSizeVector.push_back(1024 * 128);

    bsl::vector<ntctlc::PluginTest::Parameters> parametersVector;
    bsl::size_t                                 variationIndex = 0;

    for (bsl::size_t i = 0; i < algorithmVector.size(); ++i) {
        for (bsl::size_t j = 0; j < goalVector.size(); ++j) {
            for (bsl::size_t k = 0; k < bufferSizeVector.size(); ++k) {
                for (bsl::size_t l = 0; l < ioSizeVector.size(); ++l) {
                    for (bsl::size_t m = 0; m < ioSizeVector.size(); ++m) {
                        ntctlc::PluginTest::Parameters parameters;
                        parameters.setVariationIndex(variationIndex++);
                        parameters.setType(algorithmVector[i]);
                        parameters.setGoal(goalVector[j]);
                        parameters.setBlobBufferSize(bufferSizeVector[k]);
                        parameters.setWriteSize(ioSizeVector[l]);
                        parameters.setReadSize(ioSizeVector[m]);
                        parameters.setMessageCount(4);

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
#if defined(NTCTLC_PLUGIN_TEST_VARIATION)
        if (parametersVector[i].variationIndex() !=
            NTCTLC_PLUGIN_TEST_VARIATION)
        {
            continue;
        }
#endif

        ntctlc::PluginTest::verifyParameters(parametersVector[i]);
    }
}

NTSCFG_TEST_FUNCTION(ntctlc::PluginTest::verifyUsage)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::CompressionDriver> driver;
    ntctlc::Plugin::load(&driver);

    ntca::CompressionConfig compressionConfig;
    compressionConfig.setType(ntca::CompressionType::e_ZLIB);
    compressionConfig.setGoal(ntca::CompressionGoal::e_BALANCED);

    bsl::shared_ptr<ntci::Compression> compression;
    error = driver->createCompression(&compression,
                                      compressionConfig);
    NTSCFG_TEST_OK(error);

    bdlbb::SimpleBlobBufferFactory blobBufferFactory(64);

    const char k_DATA[] = "abbcccddddeeeffg";

    bdlbb::Blob initial(&blobBufferFactory);
    bdlbb::BlobUtil::append(&initial, k_DATA, sizeof k_DATA - 1);

    ntca::DeflateContext deflateContext;
    ntca::DeflateOptions deflateOptions;

    bdlbb::Blob deflated(&blobBufferFactory);
    error = compression->deflate(&deflateContext,
                                 &deflated,
                                 initial,
                                 deflateOptions);
    NTSCFG_TEST_OK(error);

    ntca::InflateContext inflateContext;
    ntca::InflateOptions inflateOptions;

    bdlbb::Blob inflated(&blobBufferFactory);
    error = compression->inflate(&inflateContext,
                                 &inflated,
                                 deflated,
                                 inflateOptions);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_EQ(bdlbb::BlobUtil::compare(inflated, initial), 0);
}

}  // close namespace ntctlc
}  // close namespace BloombergLP
