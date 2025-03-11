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

#ifndef INCLUDED_NTCF_TESTMESSAGE
#define INCLUDED_NTCF_TESTMESSAGE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntcf_testvocabulary.h>

#include <ntcf_system.h>
#include <ntci_log.h>
#include <ntci_serialization.h>
#include <ntsf_system.h>

#include <balber_berdecoder.h>
#include <balber_berdecoderoptions.h>
#include <balber_berencoder.h>
#include <balber_berencoderoptions.h>
#include <baljsn_decoder.h>
#include <baljsn_decoderoptions.h>
#include <baljsn_encoder.h>
#include <baljsn_encoderoptions.h>
#include <ball_administration.h>
#include <ball_fileobserver2.h>
#include <ball_log.h>
#include <ball_recordstringformatter.h>
#include <ball_streamobserver.h>
#include <balst_stacktracetestallocator.h>
#include <balxml_decoder.h>
#include <balxml_decoderoptions.h>
#include <balxml_encoder.h>
#include <balxml_encoderoptions.h>
#include <balxml_errorinfo.h>
#include <balxml_minireader.h>

#include <bdlb_bigendian.h>
#include <bdlb_bitutil.h>
#include <bdlb_chartype.h>
#include <bdlb_guid.h>
#include <bdlb_guidutil.h>
#include <bdlb_nullablevalue.h>
#include <bdlb_numericparseutil.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_random.h>
#include <bdlb_randomdevice.h>
#include <bdlb_string.h>
#include <bdlb_stringrefutil.h>
#include <bdlb_stringviewutil.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobstreambuf.h>
#include <bdlbb_blobutil.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bdlbb_simpleblobbufferfactory.h>
#include <bdlcc_objectcatalog.h>
#include <bdlcc_objectpool.h>
#include <bdlde_charconvertutf16.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bdlma_concurrentmultipoolallocator.h>
#include <bdlmt_eventscheduler.h>
#include <bdls_filedescriptorguard.h>
#include <bdls_filesystemutil.h>
#include <bdls_memoryutil.h>
#include <bdls_pathutil.h>
#include <bdls_pipeutil.h>
#include <bdls_processutil.h>
#include <bdlt_currenttime.h>
#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_datetz.h>
#include <bdlt_epochutil.h>
#include <bdlt_iso8601util.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bdlmt_fixedthreadpool.h>
#include <bdlmt_threadpool.h>
#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_destructionutil.h>
#include <bslma_mallocfreeallocator.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bslmt_barrier.h>
#include <bslmt_condition.h>
#include <bslmt_latch.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_once.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>
#include <bsls_alignmentfromtype.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_atomicoperations.h>
#include <bsls_byteorder.h>
#include <bsls_keyword.h>
#include <bsls_linkcoercion.h>
#include <bsls_log.h>
#include <bsls_logseverity.h>
#include <bsls_platform.h>
#include <bsls_platformutil.h>
#include <bsls_stopwatch.h>
#include <bsls_timeinterval.h>
#include <bsls_timeutil.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_array.h>
#include <bsl_cfloat.h>
#include <bsl_cmath.h>
#include <bsl_cstdint.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_ctime.h>
#include <bsl_exception.h>
#include <bsl_fstream.h>
#include <bsl_functional.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>
#include <bsl_map.h>
#include <bsl_memory.h>
#include <bsl_ostream.h>
#include <bsl_queue.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_stack.h>
#include <bsl_stdexcept.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

#include <bsl_c_errno.h>
#include <bsl_c_signal.h>

namespace BloombergLP {
namespace ntcf {

/// Provide a test message.
///
/// @par Thread Safety
/// This class is not thread safe.
class TestMessage
{
    /// The maximum size, in bytes, of the pragma section.
    static const bsl::size_t k_MAX_PRAGMA_SIZE = 1024 * 1024 * 8;

    /// The maximum size, in bytes, of the entity section.
    static const bsl::size_t k_MAX_ENTITY_SIZE = 1024 * 1024 * 1024;

    ntcf::TestMessageFrame d_frame;
    bslma::Allocator*      d_allocator_p;

  public:
    /// Create a new message. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit TestMessage(bslma::Allocator* basicAllocator = 0);

    /// Create a new message having the same value as the specified 'original'
    /// object. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    TestMessage(const TestMessage& original,
                bslma::Allocator*  basicAllocator = 0);

    /// Destroy this object.
    ~TestMessage();

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    TestMessage& operator=(const TestMessage& other);

    /// Set the message type to the specified 'value'.
    void setType(ntcf::TestMessageType::Value value);

    /// Set the header to the specified 'value'.
    void setHeader(const ntcf::TestMessageHeader& value);

    /// Set the pragma to the specified 'value'.
    void setPragma(const ntcf::TestMessagePragma& value);

    /// Set the entity to the specified 'value'.
    void setPragma(const ntcf::TestMessageEntity& value);

    /// Set the transaction identifier to the specified 'value'.
    void setTransaction(bsl::uint64_t value);

    /// Set the compression type to the specified 'value'.
    void setCompressionType(ntca::CompressionType::Value value);

    /// Set the serialization type to the specified 'value'.
    void setSerializationType(ntca::SerializationType::Value value);

    /// Set the client timestamp to the specified 'value', as a relative
    /// duration since the Unix epoch.
    void setClientTimestamp(const bsls::TimeInterval& value);

    /// Set the server timestamp to the specified 'value', as a relative
    /// duration since the Unix epoch.
    void setServerTimestamp(const bsls::TimeInterval& value);

    /// Set the deadline to the specified 'value', as a relative duration since
    /// the Unix epoch.
    void setDeadline(const bsls::TimeInterval& value);

    /// Set the flag corresponding to the specified 'value'.
    void setFlag(ntcf::TestMessageFlag::Value value);

    /// Set the flag corresponding to the specified 'value1', and 'value2'.
    void setFlag(ntcf::TestMessageFlag::Value value1,
                 ntcf::TestMessageFlag::Value value2);

    /// Set the flag corresponding to the specified 'value1', 'value2', and
    /// 'value3'.
    void setFlag(ntcf::TestMessageFlag::Value value1,
                 ntcf::TestMessageFlag::Value value2,
                 ntcf::TestMessageFlag::Value value3);

    /// Define a pragma collection for the message. Return a reference to the
    /// modifiable pragmas.
    ntcf::TestMessagePragma& makePragma();

    /// Define an entity for the message. Return a reference to the modifiable
    /// pragmas.
    ntcf::TestMessageEntity& makeEntity();

    /// Decode this object from the specified 'source' and pop the number
    /// of bytes decoded from the from of 'source'.
    ntsa::Error decode(bdlbb::Blob*         source,
                       ntci::Serialization* serialization,
                       ntci::Compression*   compression);

    /// Encode this object to the specified 'destination'.
    ntsa::Error encode(bdlbb::Blob*         destination,
                       ntci::Serialization* serialization,
                       ntci::Compression*   compression);

    /// Return the message type.
    ntcf::TestMessageType::Value type() const;

    /// Return the size of the message, in bytes.
    bsl::size_t messageSize() const;

    /// Return the header size, in bytes.
    bsl::size_t headerSize() const;

    /// Return the pragma size, in bytes.
    bsl::size_t pragmaSize() const;

    /// Return the entity size, in bytes.
    bsl::size_t entitySize() const;

    /// Return the checksum.
    bsl::uint32_t checksum() const;

    /// Return the transaction identifier.
    bsl::uint64_t transaction() const;

    /// Return the compression type.
    ntca::CompressionType::Value compressionType() const;

    /// Return the serialization type.
    ntca::SerializationType::Value serializationType() const;

    /// Return the client timestamp, as a relative duration since the Unix
    /// epoch.
    bsls::TimeInterval clientTimestamp() const;

    /// Return the server timestamp, as a relative duration since the Unix
    /// epoch.
    bsls::TimeInterval serverTimestamp() const;

    /// Return the deadline, as a relative duration since the Unix epoch.
    bsls::TimeInterval deadline() const;

    /// Return the defined pragma collection.
    const bdlb::NullableValue<ntcf::TestMessagePragma>& pragma() const;

    /// Return the entity.
    const bdlb::NullableValue<ntcf::TestMessageEntity>& entity() const;

    /// Return true if the message has the flag set corresponding to the
    /// specified 'value'.
    bool hasFlag(ntcf::TestMessageFlag::Value value) const;

    /// Return true if the message has the flag set corresponding to the
    /// specified 'value1' and 'value2'.
    bool hasFlag(ntcf::TestMessageFlag::Value value1,
                 ntcf::TestMessageFlag::Value value2) const;

    /// Return true if the message has the flag set corresponding to the
    /// specified 'value1', 'value2', and 'value3'.
    bool hasFlag(ntcf::TestMessageFlag::Value value1,
                 ntcf::TestMessageFlag::Value value2,
                 ntcf::TestMessageFlag::Value value3) const;

    /// Return true if the message defines a pragma collection, otherwise
    /// return false.
    bool hasPragma() const;

    /// Return true if the message defines an entity, otherwise return false.
    bool hasEntity() const;

    /// Return true if the message is a subscription, otherwise return false.
    bool isSubscription() const;

    /// Return true if the message is a one-way publication, uncorrelated to
    /// any specific request, otherwise return false.
    bool isPublication() const;

    /// Return true if the message is a request, otherwise return false.
    bool isRequest() const;

    /// Return true if the message reprsents a response, otherwise return
    /// false.
    bool isResponse() const;

    /// Return true if the message is a request that expects a response,
    /// otherwise return false.
    bool isResponseExpected() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const TestMessage& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const TestMessage& other) const;

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

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(TestMessage);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntcf::TestMessage
bsl::ostream& operator<<(bsl::ostream& stream, const TestMessage& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntcf::TestMessage
bool operator==(const TestMessage& lhs, const TestMessage& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntcf::TestMessage
bool operator!=(const TestMessage& lhs, const TestMessage& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntcf::TestMessage
bool operator<(const TestMessage& lhs, const TestMessage& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntcf::TestMessage
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestMessage& value);

/// Defines a type alias for shared pointer to a test message.
typedef bsl::shared_ptr<ntcf::TestMessage> TestMessagePtr;

/// Defines a type alias for vector of shared pointers to test messages.
typedef bsl::vector<ntcf::TestMessagePtr> TestMessageVector;

/// Provide a pool of test messages.
///
/// @par Thread Safety
/// This class is thread safe.
class TestMessagePool
{
    typedef bdlcc::SharedObjectPool<
        ntcf::TestMessage,
        bdlcc::ObjectPoolFunctors::DefaultCreator,
        bdlcc::ObjectPoolFunctors::Reset<ntcf::TestMessage> >
        Pool;

    Pool              d_pool;
    bslma::Allocator* d_allocator_p;

  private:
    TestMessagePool(const TestMessagePool&) BSLS_KEYWORD_DELETED;
    TestMessagePool& operator=(const TestMessagePool&) BSLS_KEYWORD_DELETED;

  private:
    /// Construct a new message at the specified 'address' using the
    /// specified 'dataPool' to allocate blob buffers and the specified
    /// 'allocator' to supply memory.
    static void construct(void* address, bslma::Allocator* allocator);

  public:
    /// Create a new message pool. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit TestMessagePool(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~TestMessagePool();

    /// Return a shared pointer to an send callback queue entry in the
    /// pool having a default value. The resulting send callback queue
    /// entry is automatically returned to this pool when its reference
    /// count reaches zero.
    bsl::shared_ptr<ntcf::TestMessage> create();
};

/// Provide a parser of test messages from a data stream.
///
/// @par Thread Safety
/// This class is not thread safe.
class TestMessageParser
{
    /// Enumerates the constants used by this implementation.
    enum Constant {
        // Defines the encoded, fixed size of a message header.
        k_HEADER_SIZE = sizeof(ntcf::TestMessageHeader)
    };

    /// Enumerates the parser states.
    enum State {
        /// The parser expectes the next bytes to form the header.
        e_WANT_HEADER,

        /// The parser expectes the next bytes to form the message.
        e_WANT_MESSAGE
    };

    /// This typedef defines a type alias for a vector of ASMP requests.
    typedef bsl::vector<bsl::shared_ptr<ntcf::TestMessage> > MessageVector;

    bsl::shared_ptr<ntci::DataPool>        d_dataPool_sp;
    bsl::shared_ptr<bdlbb::Blob>           d_data_sp;
    bsl::shared_ptr<ntcf::TestMessagePool> d_messagePool_sp;
    MessageVector                          d_messageQueue;
    bsl::shared_ptr<ntcf::TestMessage>     d_message_sp;
    bsl::shared_ptr<ntci::Serialization>   d_serialization_sp;
    bsl::shared_ptr<ntci::Compression>     d_compression_sp;
    State                                  d_state;
    bsl::size_t                            d_numNeeded;
    bool                                   d_closed;
    bslma::Allocator*                      d_allocator_p;

  private:
    TestMessageParser(const TestMessageParser&) BSLS_KEYWORD_DELETED;
    TestMessageParser& operator=(const TestMessageParser&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Process the data stream.
    ntsa::Error process();

    /// Reset the state of the parser.
    void reset();

  public:
    /// Create a new ASMP message parser. Use the specified 'dataPool' to
    /// allocate buffers. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit TestMessageParser(
        const bsl::shared_ptr<ntci::DataPool>&        dataPool,
        const bsl::shared_ptr<ntcf::TestMessagePool>& messagePool,
        const bsl::shared_ptr<ntci::Serialization>&   serialization,
        const bsl::shared_ptr<ntci::Compression>&     compression,
        bslma::Allocator*                             basicAllocator = 0);

    /// Destroy this object.
    ~TestMessageParser();

    /// Add the specified 'blob' to the parser. Return the error.
    ntsa::Error add(const bdlbb::Blob& blob);

    /// Add the specified 'blob' to the parser. Return the error.
    ntsa::Error add(const bsl::shared_ptr<bdlbb::Blob>& blob);

    /// Close the parser, indicating no more data will be added and
    /// completing the parsing of any request in progress whose length
    /// is not indicated by a definite content length header.
    void close();

    /// Load into the specified 'request' the next message parsed from
    /// the data stream. Return the error.
    ntsa::Error dequeue(bsl::shared_ptr<ntcf::TestMessage>* request);

    /// Return the number of bytes needed to completed the next parser state.
    bsl::size_t numNeeded() const;

    /// Return true if any ASMP requests parsed from the data stream are
    /// available, otherwise return false.
    bool hasAnyAvailable() const;
};

/// Define a type alias for callback invoked on a optional strand with an
/// optional cancelable authorization mechanism when message is received..
typedef ntci::Callback<void(const ntcf::TestContext&                  context,
                            const ntcf::TestFault&                    fault,
                            const bsl::shared_ptr<ntcf::TestMessage>& message)>
    TestMessageCallback;

/// Define a type alias for function invoked when a message is received.
typedef TestMessageCallback::FunctionType TestMessageFunction;

/// Provide an interface to create message callbacks.
///
/// @details
/// Unless otherwise specified, the callbacks created by this class will be
/// invoked on the object's strand.
///
/// @par Thread Safety
/// This class is thread safe.
class TestMessageCallbackFactory
{
  public:
    /// Destroy this object.
    virtual ~TestMessageCallbackFactory();

    /// Create a new message callback to invoke the specified 'function' with
    /// no cancellable authorization mechanism on this object's strand.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ntcf::TestMessageCallback createMessageCallback(
        const ntcf::TestMessageFunction& function,
        bslma::Allocator*                basicAllocator = 0);

    /// Create a new message callback to invoke the specified 'function' with
    /// the specified cancellable 'authorization' mechanism on this object's
    /// strand. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    ntcf::TestMessageCallback createMessageCallback(
        const ntcf::TestMessageFunction&            function,
        const bsl::shared_ptr<ntci::Authorization>& authorization,
        bslma::Allocator*                           basicAllocator = 0);

    /// Create a new message callback to invoke the specified 'function' with
    /// no cancellable authorization mechanism on the specified 'strand'.
    /// Optionally specify a  'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ntcf::TestMessageCallback createMessageCallback(
        const ntcf::TestMessageFunction&     function,
        const bsl::shared_ptr<ntci::Strand>& strand,
        bslma::Allocator*                    basicAllocator = 0);

    /// Create a new message callback to invoke the specified 'function' with
    /// the specified cancellable 'authorization' mechanism on the specified
    /// 'strand'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    ntcf::TestMessageCallback createMessageCallback(
        const ntcf::TestMessageFunction&            function,
        const bsl::shared_ptr<ntci::Authorization>& authorization,
        const bsl::shared_ptr<ntci::Strand>&        strand,
        bslma::Allocator*                           basicAllocator = 0);

    /// Return the strand on which this object's functions should be called.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const = 0;
};

/// Provide a future asynchronous result of a message operation.
///
/// @par Thread Safety
/// This class is thread safe.
class TestMessageFuture : public ntcf::TestMessageCallback
{
    /// Defines a type alias for a pair of two elements, one representing the
    /// context, the other representing the fault.
    typedef bsl::pair<ntcf::TestContext, ntcf::TestFault> Meta;

    /// Defines a type alias for a pair of two elemnts, on representing the
    /// context and fault, and the other representing the response.
    typedef bsl::pair<Meta, bsl::shared_ptr<ntcf::TestMessage> > Entry;

    /// Define a type alias for a queue of results.
    typedef bsl::list<Entry> ResultQueue;

    ntccfg::ConditionMutex d_mutex;
    ntccfg::Condition      d_condition;
    ResultQueue            d_resultQueue;

  private:
    TestMessageFuture(const TestMessageFuture&) BSLS_KEYWORD_DELETED;
    TestMessageFuture& operator=(const TestMessageFuture&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Arrive at the specified 'result'.
    void arrive(const ntcf::TestContext&                  context,
                const ntcf::TestFault&                    fault,
                const bsl::shared_ptr<ntcf::TestMessage>& result);

  public:
    /// Create a new message future. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default allocator is used.
    explicit TestMessageFuture(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~TestMessageFuture();

    /// Wait for the message operation to complete and load the result into
    /// the specified 'result'. Return the error.
    ntsa::Error wait(ntcf::TestContext*                  context,
                     ntcf::TestFault*                    fault,
                     bsl::shared_ptr<ntcf::TestMessage>* result);

    /// Wait for the message operation to complete or until the specified
    /// 'timeout', in absolute time since the Unix epoch, elapses. Return
    /// the error.
    ntsa::Error wait(ntcf::TestContext*                  context,
                     ntcf::TestFault*                    fault,
                     bsl::shared_ptr<ntcf::TestMessage>* result,
                     const bsls::TimeInterval&           timeout);
};

/// Provide suite of encryption certificates and keys used to secure a
/// transport through which to send and receive test messages.
///
/// @par Thread Safety
/// This class is thread safe.
class TestMessageEncryption
{
    ntca::EncryptionKey         d_authorityPrivateKey;
    ntca::EncryptionCertificate d_authorityCertificate;
    ntca::EncryptionKey         d_serverPrivateKey;
    ntca::EncryptionCertificate d_serverCertificate;
    bslma::Allocator*           d_allocator_p;

  public:
    /// Create a new test message encryption suite. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit TestMessageEncryption(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~TestMessageEncryption();

    /// Return the certificate authority's private key.
    const ntca::EncryptionKey& authorityPrivateKey() const;

    /// Return the certificate authority's certificate.
    const ntca::EncryptionCertificate& authorityCertificate() const;

    /// Return the server's private key.
    const ntca::EncryptionKey& serverPrivateKey() const;

    /// Return the server's certificate.
    const ntca::EncryptionCertificate& serverCertificate() const;
};

/// Define a type alias for callback invoked on a optional strand with an
/// optional cancelable authorization mechanism when a bid or ask completes or
/// fails.
typedef ntci::Callback<void(const ntcf::TestTradeResult& result)>
    TestTradeCallback;

/// Define a type alias for function invoked when a bid or ask completes or
/// fails.
typedef TestTradeCallback::FunctionType TestTradeFunction;

/// Provide an interface to create trade callbacks.
///
/// @details
/// Unless otherwise specified, the callbacks created by this class will be
/// invoked on the object's strand.
///
/// @par Thread Safety
/// This class is thread safe.
class TestTradeCallbackFactory
{
  public:
    /// Destroy this object.
    virtual ~TestTradeCallbackFactory();

    /// Create a new trade callback to invoke the specified 'function' with
    /// no cancellable authorization mechanism on this object's strand.
    /// Optionally specify a  'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ntcf::TestTradeCallback createTradeCallback(
        const ntcf::TestTradeFunction& function,
        bslma::Allocator*              basicAllocator = 0);

    /// Create a new trade callback to invoke the specified 'function' with
    /// the specified cancellable 'authorization' mechanism on this object's
    /// strand. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    ntcf::TestTradeCallback createTradeCallback(
        const ntcf::TestTradeFunction&              function,
        const bsl::shared_ptr<ntci::Authorization>& authorization,
        bslma::Allocator*                           basicAllocator = 0);

    /// Create a new trade callback to invoke the specified 'function' with
    /// no cancellable authorization mechanism on the specified 'strand'.
    /// Optionally specify a  'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ntcf::TestTradeCallback createTradeCallback(
        const ntcf::TestTradeFunction&       function,
        const bsl::shared_ptr<ntci::Strand>& strand,
        bslma::Allocator*                    basicAllocator = 0);

    /// Create a new trade callback to invoke the specified 'function' with
    /// the specified cancellable 'authorization' mechanism on the specified
    /// 'strand'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    ntcf::TestTradeCallback createTradeCallback(
        const ntcf::TestTradeFunction&              function,
        const bsl::shared_ptr<ntci::Authorization>& authorization,
        const bsl::shared_ptr<ntci::Strand>&        strand,
        bslma::Allocator*                           basicAllocator = 0);

    /// Return the strand on which this object's functions should be called.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const = 0;
};

/// Provide a future asynchronous result of a trade operation.
///
/// @par Thread Safety
/// This class is thread safe.
class TestTradeFuture : public ntcf::TestTradeCallback
{
    /// Define a type alias for a queue of results.
    typedef bsl::list<ntcf::TestTradeResult> ResultQueue;

    ntccfg::ConditionMutex d_mutex;
    ntccfg::Condition      d_condition;
    ResultQueue            d_resultQueue;

  private:
    TestTradeFuture(const TestTradeFuture&) BSLS_KEYWORD_DELETED;
    TestTradeFuture& operator=(const TestTradeFuture&) BSLS_KEYWORD_DELETED;

  private:
    /// Arrive at the specified 'result'.
    void arrive(const ntcf::TestTradeResult& result);

  public:
    /// Create a new trade future. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default allocator is used.
    explicit TestTradeFuture(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~TestTradeFuture();

    /// Wait for the trade operation to complete and load the result into
    /// the specified 'result'. Return the error.
    ntsa::Error wait(ntcf::TestTradeResult* result);

    /// Wait for the trade operation to complete or until the specified
    /// 'timeout', in absolute time since the Unix epoch, elapses. Return
    /// the error.
    ntsa::Error wait(ntcf::TestTradeResult*    result,
                     const bsls::TimeInterval& timeout);
};

/// Define a type alias for callback invoked on a optional strand with an
/// optional cancelable authorization mechanism when a signal completes or
/// fails.
typedef ntci::Callback<void(const ntcf::TestEchoResult& result)>
    TestEchoCallback;

/// Define a type alias for function invoked when a signal completes or
/// fails.
typedef TestEchoCallback::FunctionType TestEchoFunction;

/// Provide an interface to create echo callbacks.
///
/// @details
/// Unless otherwise specified, the callbacks created by this class will be
/// invoked on the object's strand.
///
/// @par Thread Safety
/// This class is thread safe.
class TestEchoCallbackFactory
{
  public:
    /// Destroy this object.
    virtual ~TestEchoCallbackFactory();

    /// Create a new echo callback to invoke the specified 'function' with
    /// no cancellable authorization mechanism on this object's strand.
    /// Optionally specify a  'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ntcf::TestEchoCallback createEchoCallback(
        const ntcf::TestEchoFunction& function,
        bslma::Allocator*             basicAllocator = 0);

    /// Create a new echo callback to invoke the specified 'function' with
    /// the specified cancellable 'authorization' mechanism on this object's
    /// strand. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    ntcf::TestEchoCallback createEchoCallback(
        const ntcf::TestEchoFunction&               function,
        const bsl::shared_ptr<ntci::Authorization>& authorization,
        bslma::Allocator*                           basicAllocator = 0);

    /// Create a new echo callback to invoke the specified 'function' with
    /// no cancellable authorization mechanism on the specified 'strand'.
    /// Optionally specify a  'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ntcf::TestEchoCallback createEchoCallback(
        const ntcf::TestEchoFunction&        function,
        const bsl::shared_ptr<ntci::Strand>& strand,
        bslma::Allocator*                    basicAllocator = 0);

    /// Create a new echo callback to invoke the specified 'function' with
    /// the specified cancellable 'authorization' mechanism on the specified
    /// 'strand'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    ntcf::TestEchoCallback createEchoCallback(
        const ntcf::TestEchoFunction&               function,
        const bsl::shared_ptr<ntci::Authorization>& authorization,
        const bsl::shared_ptr<ntci::Strand>&        strand,
        bslma::Allocator*                           basicAllocator = 0);

    /// Return the strand on which this object's functions should be called.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const = 0;
};

/// Provide a future asynchronous result of a echo operation.
///
/// @par Thread Safety
/// This class is thread safe.
class TestEchoFuture : public ntcf::TestEchoCallback
{
    /// Define a type alias for a queue of results.
    typedef bsl::list<ntcf::TestEchoResult> ResultQueue;

    ntccfg::ConditionMutex d_mutex;
    ntccfg::Condition      d_condition;
    ResultQueue            d_resultQueue;

  private:
    TestEchoFuture(const TestEchoFuture&) BSLS_KEYWORD_DELETED;
    TestEchoFuture& operator=(const TestEchoFuture&) BSLS_KEYWORD_DELETED;

  private:
    /// Arrive at the specified 'result'.
    void arrive(const ntcf::TestEchoResult& result);

  public:
    /// Create a new echo future. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default allocator is used.
    explicit TestEchoFuture(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~TestEchoFuture();

    /// Wait for the echo operation to complete and load the result into
    /// the specified 'result'. Return the error.
    ntsa::Error wait(ntcf::TestEchoResult* result);

    /// Wait for the echo operation to complete or until the specified
    /// 'timeout', in absolute time since the Unix epoch, elapses. Return
    /// the error.
    ntsa::Error wait(ntcf::TestEchoResult*     result,
                     const bsls::TimeInterval& timeout);
};

/// Define a type alias for callback invoked on a optional strand with an
/// optional cancelable authorization mechanism when a control message
/// completes or fails.
typedef ntci::Callback<void(const ntcf::TestAcknowledgmentResult& result)>
    TestAcknowledgmentCallback;

/// Define a type alias for function invoked when a control message completes
/// or fails.
typedef TestAcknowledgmentCallback::FunctionType TestAcknowledgmentFunction;

/// Provide an interface to create trade callbacks.
///
/// @details
/// Unless otherwise specified, the callbacks created by this class will be
/// invoked on the object's strand.
///
/// @par Thread Safety
/// This class is thread safe.
class TestAcknowledgmentCallbackFactory
{
  public:
    /// Destroy this object.
    virtual ~TestAcknowledgmentCallbackFactory();

    /// Create a new trade callback to invoke the specified 'function' with
    /// no cancellable authorization mechanism on this object's strand.
    /// Optionally specify a  'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ntcf::TestAcknowledgmentCallback createAcknowledgmentCallback(
        const ntcf::TestAcknowledgmentFunction& function,
        bslma::Allocator*                       basicAllocator = 0);

    /// Create a new trade callback to invoke the specified 'function' with
    /// the specified cancellable 'authorization' mechanism on this object's
    /// strand. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    ntcf::TestAcknowledgmentCallback createAcknowledgmentCallback(
        const ntcf::TestAcknowledgmentFunction&     function,
        const bsl::shared_ptr<ntci::Authorization>& authorization,
        bslma::Allocator*                           basicAllocator = 0);

    /// Create a new trade callback to invoke the specified 'function' with
    /// no cancellable authorization mechanism on the specified 'strand'.
    /// Optionally specify a  'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ntcf::TestAcknowledgmentCallback createAcknowledgmentCallback(
        const ntcf::TestAcknowledgmentFunction& function,
        const bsl::shared_ptr<ntci::Strand>&    strand,
        bslma::Allocator*                       basicAllocator = 0);

    /// Create a new trade callback to invoke the specified 'function' with
    /// the specified cancellable 'authorization' mechanism on the specified
    /// 'strand'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    ntcf::TestAcknowledgmentCallback createAcknowledgmentCallback(
        const ntcf::TestAcknowledgmentFunction&     function,
        const bsl::shared_ptr<ntci::Authorization>& authorization,
        const bsl::shared_ptr<ntci::Strand>&        strand,
        bslma::Allocator*                           basicAllocator = 0);

    /// Return the strand on which this object's functions should be called.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const = 0;
};

/// Provide a future asynchronous result of a trade operation.
///
/// @par Thread Safety
/// This class is thread safe.
class TestAcknowledgmentFuture : public ntcf::TestAcknowledgmentCallback
{
    /// Define a type alias for a queue of results.
    typedef bsl::list<ntcf::TestAcknowledgmentResult> ResultQueue;

    ntccfg::ConditionMutex d_mutex;
    ntccfg::Condition      d_condition;
    ResultQueue            d_resultQueue;

  private:
    TestAcknowledgmentFuture(const TestAcknowledgmentFuture&)
        BSLS_KEYWORD_DELETED;
    TestAcknowledgmentFuture& operator=(const TestAcknowledgmentFuture&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Arrive at the specified 'result'.
    void arrive(const ntcf::TestAcknowledgmentResult& result);

  public:
    /// Create a new trade future. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default allocator is used.
    explicit TestAcknowledgmentFuture(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~TestAcknowledgmentFuture();

    /// Wait for the trade operation to complete and load the result into
    /// the specified 'result'. Return the error.
    ntsa::Error wait(ntcf::TestAcknowledgmentResult* result);

    /// Wait for the trade operation to complete or until the specified
    /// 'timeout', in absolute time since the Unix epoch, elapses. Return
    /// the error.
    ntsa::Error wait(ntcf::TestAcknowledgmentResult* result,
                     const bsls::TimeInterval&       timeout);
};

NTCCFG_INLINE
TestMessage::TestMessage(bslma::Allocator* basicAllocator)
: d_frame(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

NTCCFG_INLINE
TestMessage::TestMessage(const TestMessage& original,
                         bslma::Allocator*  basicAllocator)
: d_frame(original.d_frame, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

NTCCFG_INLINE
TestMessage::~TestMessage()
{
}

NTCCFG_INLINE
TestMessage& TestMessage::operator=(const TestMessage& other)
{
    if (this != &other) {
        d_frame = other.d_frame;
    }

    return *this;
}

NTCCFG_INLINE
void TestMessage::reset()
{
    d_frame.reset();
}

NTCCFG_INLINE
void TestMessage::setType(ntcf::TestMessageType::Value value)
{
    d_frame.header.messageType =
        static_cast<bsl::uint16_t>(static_cast<int>(value));
}

NTCCFG_INLINE
void TestMessage::setHeader(const ntcf::TestMessageHeader& value)
{
    d_frame.header = value;
}

NTCCFG_INLINE
void TestMessage::setPragma(const ntcf::TestMessagePragma& value)
{
    d_frame.pragma = value;
}

NTCCFG_INLINE
void TestMessage::setPragma(const ntcf::TestMessageEntity& value)
{
    d_frame.entity = value;
}

NTCCFG_INLINE
void TestMessage::setTransaction(bsl::uint64_t value)
{
    d_frame.header.transaction = static_cast<bsls::Types::Uint64>(value);
}

NTCCFG_INLINE
void TestMessage::setCompressionType(ntca::CompressionType::Value value)
{
    d_frame.header.compression = static_cast<bsl::uint32_t>(value);
}

NTCCFG_INLINE
void TestMessage::setSerializationType(ntca::SerializationType::Value value)
{
    d_frame.header.serialization = static_cast<bsl::uint32_t>(value);
}

NTCCFG_INLINE
void TestMessage::setClientTimestamp(const bsls::TimeInterval& value)
{
    d_frame.header.clientTimestamp =
        static_cast<bsls::Types::Uint64>(value.totalNanoseconds());
}

NTCCFG_INLINE
void TestMessage::setServerTimestamp(const bsls::TimeInterval& value)
{
    d_frame.header.serverTimestamp =
        static_cast<bsls::Types::Uint64>(value.totalNanoseconds());
}

NTCCFG_INLINE
void TestMessage::setDeadline(const bsls::TimeInterval& value)
{
    d_frame.header.deadline =
        static_cast<bsls::Types::Uint64>(value.totalNanoseconds());
}

NTCCFG_INLINE
void TestMessage::setFlag(ntcf::TestMessageFlag::Value value)
{
    bsl::uint16_t flags =
        static_cast<bsl::uint16_t>(d_frame.header.messageFlags);

    flags |= (1 << value);

    d_frame.header.messageFlags = flags;
}

NTCCFG_INLINE
void TestMessage::setFlag(ntcf::TestMessageFlag::Value value1,
                          ntcf::TestMessageFlag::Value value2)
{
    bsl::uint16_t flags =
        static_cast<bsl::uint16_t>(d_frame.header.messageFlags);

    flags |= (1 << value1);
    flags |= (1 << value2);

    d_frame.header.messageFlags = flags;
}

NTCCFG_INLINE
void TestMessage::setFlag(ntcf::TestMessageFlag::Value value1,
                          ntcf::TestMessageFlag::Value value2,
                          ntcf::TestMessageFlag::Value value3)
{
    bsl::uint16_t flags =
        static_cast<bsl::uint16_t>(d_frame.header.messageFlags);

    flags |= (1 << value1);
    flags |= (1 << value2);
    flags |= (1 << value3);

    d_frame.header.messageFlags = flags;
}

NTCCFG_INLINE
ntcf::TestMessagePragma& TestMessage::makePragma()
{
    return d_frame.pragma.makeValue();
}

NTCCFG_INLINE
ntcf::TestMessageEntity& TestMessage::makeEntity()
{
    return d_frame.entity.makeValue();
}

NTCCFG_INLINE
ntcf::TestMessageType::Value TestMessage::type() const
{
    return static_cast<ntcf::TestMessageType::Value>(
        static_cast<int>(d_frame.header.messageType));
}

NTCCFG_INLINE
bsl::size_t TestMessage::messageSize() const
{
    return static_cast<bsl::size_t>(
        static_cast<bsl::uint32_t>(d_frame.header.messageSize));
}

NTCCFG_INLINE
bsl::size_t TestMessage::headerSize() const
{
    return static_cast<bsl::size_t>(
        static_cast<bsl::uint32_t>(d_frame.header.headerSize));
}

NTCCFG_INLINE
bsl::size_t TestMessage::pragmaSize() const
{
    return static_cast<bsl::size_t>(
        static_cast<bsl::uint32_t>(d_frame.header.pragmaSize));
}

NTCCFG_INLINE
bsl::size_t TestMessage::entitySize() const
{
    return static_cast<bsl::size_t>(
        static_cast<bsl::uint32_t>(d_frame.header.entitySize));
}

NTCCFG_INLINE
bsl::uint32_t TestMessage::checksum() const
{
    return static_cast<bsl::size_t>(
        static_cast<bsl::uint32_t>(d_frame.header.checksum));
}

NTCCFG_INLINE
bsl::uint64_t TestMessage::transaction() const
{
    return static_cast<bsl::uint64_t>(d_frame.header.transaction);
}

NTCCFG_INLINE
ntca::CompressionType::Value TestMessage::compressionType() const
{
    return static_cast<ntca::CompressionType::Value>(
        static_cast<int>(d_frame.header.compression));
}

NTCCFG_INLINE
ntca::SerializationType::Value TestMessage::serializationType() const
{
    return static_cast<ntca::SerializationType::Value>(
        static_cast<int>(d_frame.header.serialization));
}

NTCCFG_INLINE
bsls::TimeInterval TestMessage::clientTimestamp() const
{
    bsls::TimeInterval result;
    result.addNanoseconds(static_cast<bsls::Types::Int64>(
        static_cast<bsl::uint64_t>(d_frame.header.clientTimestamp)));
    return result;
}

NTCCFG_INLINE
bsls::TimeInterval TestMessage::serverTimestamp() const
{
    bsls::TimeInterval result;
    result.addNanoseconds(static_cast<bsls::Types::Int64>(
        static_cast<bsl::uint64_t>(d_frame.header.serverTimestamp)));
    return result;
}

NTCCFG_INLINE
bsls::TimeInterval TestMessage::deadline() const
{
    bsls::TimeInterval result;
    result.addNanoseconds(static_cast<bsls::Types::Int64>(
        static_cast<bsl::uint64_t>(d_frame.header.deadline)));
    return result;
}

NTCCFG_INLINE
const bdlb::NullableValue<ntcf::TestMessagePragma>& TestMessage::pragma() const
{
    return d_frame.pragma;
}

NTCCFG_INLINE
const bdlb::NullableValue<ntcf::TestMessageEntity>& TestMessage::entity() const
{
    return d_frame.entity;
}

NTCCFG_INLINE
bool TestMessage::hasFlag(ntcf::TestMessageFlag::Value value) const
{
    const bsl::uint16_t flags =
        static_cast<bsl::uint16_t>(d_frame.header.messageFlags);

    const bsl::uint16_t mask = (1 << value);

    return ((flags & mask) != 0);
}

NTCCFG_INLINE
bool TestMessage::hasFlag(ntcf::TestMessageFlag::Value value1,
                          ntcf::TestMessageFlag::Value value2) const
{
    const bsl::uint16_t flags =
        static_cast<bsl::uint16_t>(d_frame.header.messageFlags);

    const bsl::uint16_t mask = (1 << value1) | (1 << value2);

    return ((flags & mask) != 0);
}

NTCCFG_INLINE
bool TestMessage::hasFlag(ntcf::TestMessageFlag::Value value1,
                          ntcf::TestMessageFlag::Value value2,
                          ntcf::TestMessageFlag::Value value3) const
{
    const bsl::uint16_t flags =
        static_cast<bsl::uint16_t>(d_frame.header.messageFlags);

    const bsl::uint16_t mask = (1 << value1) | (1 << value2) | (1 << value3);

    return ((flags & mask) != 0);
}

NTCCFG_INLINE
bool TestMessage::isSubscription() const
{
    return this->hasFlag(ntcf::TestMessageFlag::e_SUBSCRIPTION);
}

NTCCFG_INLINE
bool TestMessage::isPublication() const
{
    return this->hasFlag(ntcf::TestMessageFlag::e_PUBLICATION);
}

NTCCFG_INLINE
bool TestMessage::isRequest() const
{
    return this->hasFlag(ntcf::TestMessageFlag::e_REQUEST);
}

NTCCFG_INLINE
bool TestMessage::isResponse() const
{
    return this->hasFlag(ntcf::TestMessageFlag::e_RESPONSE);
}

NTCCFG_INLINE
bool TestMessage::isResponseExpected() const
{
    return !this->hasFlag(ntcf::TestMessageFlag::e_UNACKNOWLEDGED);
}

NTCCFG_INLINE
bool TestMessage::hasPragma() const
{
    return d_frame.pragma.has_value();
}

NTCCFG_INLINE
bool TestMessage::hasEntity() const
{
    return d_frame.entity.has_value();
}

template <typename HASH_ALGORITHM>
NTCCFG_INLINE void TestMessage::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_frame);
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const TestMessage& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const TestMessage& lhs, const TestMessage& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const TestMessage& lhs, const TestMessage& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const TestMessage& lhs, const TestMessage& rhs)
{
    return lhs.less(rhs);
}

NTCCFG_INLINE
bsl::shared_ptr<ntcf::TestMessage> TestMessagePool::create()
{
    return d_pool.getObject();
}

NTCCFG_INLINE
ntcf::TestMessageCallback TestMessageCallbackFactory::createMessageCallback(
    const ntcf::TestMessageFunction& function,
    bslma::Allocator*                basicAllocator)
{
    return ntcf::TestMessageCallback(function, this->strand(), basicAllocator);
}

NTCCFG_INLINE
ntcf::TestMessageCallback TestMessageCallbackFactory::createMessageCallback(
    const ntcf::TestMessageFunction&            function,
    const bsl::shared_ptr<ntci::Authorization>& authorization,
    bslma::Allocator*                           basicAllocator)
{
    return ntcf::TestMessageCallback(function,
                                     authorization,
                                     this->strand(),
                                     basicAllocator);
}

NTCCFG_INLINE
ntcf::TestMessageCallback TestMessageCallbackFactory::createMessageCallback(
    const ntcf::TestMessageFunction&     function,
    const bsl::shared_ptr<ntci::Strand>& strand,
    bslma::Allocator*                    basicAllocator)
{
    return ntcf::TestMessageCallback(function, strand, basicAllocator);
}

NTCCFG_INLINE
ntcf::TestMessageCallback TestMessageCallbackFactory::createMessageCallback(
    const ntcf::TestMessageFunction&            function,
    const bsl::shared_ptr<ntci::Authorization>& authorization,
    const bsl::shared_ptr<ntci::Strand>&        strand,
    bslma::Allocator*                           basicAllocator)
{
    return ntcf::TestMessageCallback(function,
                                     authorization,
                                     strand,
                                     basicAllocator);
}

NTCCFG_INLINE
ntcf::TestTradeCallback TestTradeCallbackFactory::createTradeCallback(
    const ntcf::TestTradeFunction& function,
    bslma::Allocator*              basicAllocator)
{
    return ntcf::TestTradeCallback(function, this->strand(), basicAllocator);
}

NTCCFG_INLINE
ntcf::TestTradeCallback TestTradeCallbackFactory::createTradeCallback(
    const ntcf::TestTradeFunction&              function,
    const bsl::shared_ptr<ntci::Authorization>& authorization,
    bslma::Allocator*                           basicAllocator)
{
    return ntcf::TestTradeCallback(function,
                                   authorization,
                                   this->strand(),
                                   basicAllocator);
}

NTCCFG_INLINE
ntcf::TestTradeCallback TestTradeCallbackFactory::createTradeCallback(
    const ntcf::TestTradeFunction&       function,
    const bsl::shared_ptr<ntci::Strand>& strand,
    bslma::Allocator*                    basicAllocator)
{
    return ntcf::TestTradeCallback(function, strand, basicAllocator);
}

NTCCFG_INLINE
ntcf::TestTradeCallback TestTradeCallbackFactory::createTradeCallback(
    const ntcf::TestTradeFunction&              function,
    const bsl::shared_ptr<ntci::Authorization>& authorization,
    const bsl::shared_ptr<ntci::Strand>&        strand,
    bslma::Allocator*                           basicAllocator)
{
    return ntcf::TestTradeCallback(function,
                                   authorization,
                                   strand,
                                   basicAllocator);
}

NTCCFG_INLINE
ntcf::TestEchoCallback TestEchoCallbackFactory::createEchoCallback(
    const ntcf::TestEchoFunction& function,
    bslma::Allocator*             basicAllocator)
{
    return ntcf::TestEchoCallback(function, this->strand(), basicAllocator);
}

NTCCFG_INLINE
ntcf::TestEchoCallback TestEchoCallbackFactory::createEchoCallback(
    const ntcf::TestEchoFunction&               function,
    const bsl::shared_ptr<ntci::Authorization>& authorization,
    bslma::Allocator*                           basicAllocator)
{
    return ntcf::TestEchoCallback(function,
                                  authorization,
                                  this->strand(),
                                  basicAllocator);
}

NTCCFG_INLINE
ntcf::TestEchoCallback TestEchoCallbackFactory::createEchoCallback(
    const ntcf::TestEchoFunction&        function,
    const bsl::shared_ptr<ntci::Strand>& strand,
    bslma::Allocator*                    basicAllocator)
{
    return ntcf::TestEchoCallback(function, strand, basicAllocator);
}

NTCCFG_INLINE
ntcf::TestEchoCallback TestEchoCallbackFactory::createEchoCallback(
    const ntcf::TestEchoFunction&               function,
    const bsl::shared_ptr<ntci::Authorization>& authorization,
    const bsl::shared_ptr<ntci::Strand>&        strand,
    bslma::Allocator*                           basicAllocator)
{
    return ntcf::TestEchoCallback(function,
                                  authorization,
                                  strand,
                                  basicAllocator);
}

NTCCFG_INLINE
ntcf::TestAcknowledgmentCallback TestAcknowledgmentCallbackFactory::
    createAcknowledgmentCallback(
        const ntcf::TestAcknowledgmentFunction& function,
        bslma::Allocator*                       basicAllocator)
{
    return ntcf::TestAcknowledgmentCallback(function,
                                            this->strand(),
                                            basicAllocator);
}

NTCCFG_INLINE
ntcf::TestAcknowledgmentCallback TestAcknowledgmentCallbackFactory::
    createAcknowledgmentCallback(
        const ntcf::TestAcknowledgmentFunction&     function,
        const bsl::shared_ptr<ntci::Authorization>& authorization,
        bslma::Allocator*                           basicAllocator)
{
    return ntcf::TestAcknowledgmentCallback(function,
                                            authorization,
                                            this->strand(),
                                            basicAllocator);
}

NTCCFG_INLINE
ntcf::TestAcknowledgmentCallback TestAcknowledgmentCallbackFactory::
    createAcknowledgmentCallback(
        const ntcf::TestAcknowledgmentFunction& function,
        const bsl::shared_ptr<ntci::Strand>&    strand,
        bslma::Allocator*                       basicAllocator)
{
    return ntcf::TestAcknowledgmentCallback(function, strand, basicAllocator);
}

NTCCFG_INLINE
ntcf::TestAcknowledgmentCallback TestAcknowledgmentCallbackFactory::
    createAcknowledgmentCallback(
        const ntcf::TestAcknowledgmentFunction&     function,
        const bsl::shared_ptr<ntci::Authorization>& authorization,
        const bsl::shared_ptr<ntci::Strand>&        strand,
        bslma::Allocator*                           basicAllocator)
{
    return ntcf::TestAcknowledgmentCallback(function,
                                            authorization,
                                            strand,
                                            basicAllocator);
}

}  // end namespace ntcf
}  // end namespace BloombergLP
#endif
