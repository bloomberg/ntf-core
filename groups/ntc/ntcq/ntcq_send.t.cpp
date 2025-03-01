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
BSLS_IDENT_RCSID(ntcq_send_t_cpp, "$Id$ $CSID$")

#include <ntcq_send.h>

#include <ntccfg_bind.h>
#include <ntci_mutex.h>
#include <ntsa_data.h>
#include <ntsa_temporary.h>
#include <ntsd_datautil.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcq {

// Provide tests for 'ntcq::SendQueue'.
class SendQueueTest
{
    /// Provide an interface to guarantee sequential, non-concurrent
    /// execution.
    class Strand;

    // Process the specified 'event' from the specified 'sender'. Ensure the
    // event indicates the operation completed successfully. Increment the
    // specified 'numInvoked'.
    static void processComplete(bsls::AtomicUint* numInvoked,
                                const bsl::shared_ptr<ntci::Sender>& sender,
                                const ntca::SendEvent&               event);

  public:
    // Concern: Announcement of the low watermark is not authorized until the
    // queue is filled up to the high watermark then drained down the low
    // watermark. Announcement of the high watermark is authorized once the
    // queue is filled up to the high watermark, but not announced again until
    // the queue is drained down to the low watermark.
    static void verifyCase1();

    // Concern: Batching next suitable entries: empty.
    static void verifyCase2();

    // Concern: Batching next suitable entries: blob.
    static void verifyCase3();

    // Concern: Batching next suitable entries: blob -> blob.
    static void verifyCase4();

    // Concern: Batching next suitable entries: blob -> blob -> file -> blob.
    static void verifyCase5();

    // Concern: Batching next suitable entries: limit maximum buffers to the
    // maximum number sendable per system call (to avoid EMSGBUF).
    static void verifyCase6();
};

/// Provide an interface to guarantee sequential, non-concurrent
/// execution.
class SendQueueTest::Strand : public ntci::Strand,
                              public ntccfg::Shared<Strand>
{
    /// Define a type alias for a mutex.
    typedef ntci::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntci::LockGuard LockGuard;

    /// Define a type alias for a queue of callbacks to
    /// execute on this thread.
    typedef ntci::Executor::FunctorSequence FunctorQueue;

    ntccfg::Object    d_object;
    mutable Mutex     d_functorQueueMutex;
    FunctorQueue      d_functorQueue;
    bslma::Allocator* d_allocator_p;

  private:
    Strand(const Strand&) BSLS_KEYWORD_DELETED;
    Strand& operator=(const Strand&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new strand with characteristics suitable for this
    /// test driver. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit Strand(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Strand() BSLS_KEYWORD_OVERRIDE;

    /// Defer the specified 'function' to execute sequentially, and
    /// non-concurrently, after all previously deferred functions. Note that
    /// the 'function' is not necessarily guaranteed to execute on the same
    /// thread as previously deferred functions were executed, or is it
    /// necessarily guaranteed to execute on a different thread than
    /// previously deferred functions were executed.
    void execute(const Functor& function) BSLS_KEYWORD_OVERRIDE;

    /// Atomically defer the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE;

    /// Execute all pending operations on the calling thread. The behavior
    /// is undefined unless no other thread is processing pending
    /// operations.
    void drain() BSLS_KEYWORD_OVERRIDE;

    /// Cancel all pending functions.
    void clear() BSLS_KEYWORD_OVERRIDE;

    /// Return true if operations in this strand are currently being invoked
    /// by the current thread, otherwise return false.
    bool isRunningInCurrentThread() const BSLS_KEYWORD_OVERRIDE;
};

SendQueueTest::Strand::Strand(bslma::Allocator* basicAllocator)
: d_object("test::Strand")
, d_functorQueueMutex()
, d_functorQueue(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

SendQueueTest::Strand::~Strand()
{
}

void SendQueueTest::Strand::execute(const Functor& function)
{
    LockGuard lock(&d_functorQueueMutex);

    d_functorQueue.push_back(function);
}

void SendQueueTest::Strand::moveAndExecute(FunctorSequence* functorSequence,
                                           const Functor&   functor)
{
    LockGuard lock(&d_functorQueueMutex);

    d_functorQueue.splice(d_functorQueue.end(), *functorSequence);
    if (functor) {
        d_functorQueue.push_back(functor);
    }
}

void SendQueueTest::Strand::drain()
{
    FunctorQueue functorQueue;
    {
        LockGuard lock(&d_functorQueueMutex);
        functorQueue.swap(d_functorQueue);
    }

    ntci::StrandGuard strandGuard(this);

    for (FunctorQueue::iterator it = functorQueue.begin();
         it != functorQueue.end();
         ++it)
    {
        (*it)();
    }
}

void SendQueueTest::Strand::clear()
{
    LockGuard lock(&d_functorQueueMutex);

    d_functorQueue.clear();
}

bool SendQueueTest::Strand::isRunningInCurrentThread() const
{
    const ntci::Strand* current = ntci::Strand::getThreadLocal();
    return (current == this);
}

void SendQueueTest::processComplete(
    bsls::AtomicUint*                    numInvoked,
    const bsl::shared_ptr<ntci::Sender>& sender,
    const ntca::SendEvent&               event)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Event = " << event << NTCI_LOG_STREAM_END;

    NTSCFG_TEST_EQ(event.type(), ntca::SendEventType::e_COMPLETE);
    NTSCFG_TEST_FALSE(event.context().error());

    ++(*numInvoked);
}

NTSCFG_TEST_FUNCTION(ntcq::SendQueueTest::verifyCase1)
{
    // Concern: Announcement of the low watermark is not authorized until the
    // queue is filled up to the high watermark then drained down the low
    // watermark. Announcement of the high watermark is authorized once the
    // queue is filled up to the high watermark, but not announced again until
    // the queue is drained down to the low watermark.

    const bsl::size_t k_BLOB_BUFFER_SIZE = 32;
    const bsl::size_t k_MESSAGE_SIZE     = k_BLOB_BUFFER_SIZE * 4;
    const bsl::size_t k_LOW_WATERMARK    = 0;
    const bsl::size_t k_HIGH_WATERMARK   = 2 * k_MESSAGE_SIZE;

    bdlbb::SimpleBlobBufferFactory blobBufferFactory(k_BLOB_BUFFER_SIZE,
                                                     NTSCFG_TEST_ALLOCATOR);

    ntcq::SendQueue sendQueue(NTSCFG_TEST_ALLOCATOR);

    sendQueue.setLowWatermark(k_LOW_WATERMARK);
    sendQueue.setHighWatermark(k_HIGH_WATERMARK);

    NTSCFG_TEST_EQ(sendQueue.lowWatermark(), k_LOW_WATERMARK);
    NTSCFG_TEST_EQ(sendQueue.highWatermark(), k_HIGH_WATERMARK);

    for (bsl::size_t round = 0; round < 2; ++round) {
        NTSCFG_TEST_EQ(sendQueue.size(), 0);

        NTSCFG_TEST_TRUE(sendQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_FALSE(sendQueue.authorizeLowWatermarkEvent());

        NTSCFG_TEST_FALSE(sendQueue.isHighWatermarkViolated());
        NTSCFG_TEST_FALSE(sendQueue.authorizeHighWatermarkEvent());

        {
            bsl::shared_ptr<ntsa::Data> data;
            data.createInplace(NTSCFG_TEST_ALLOCATOR,
                               &blobBufferFactory,
                               NTSCFG_TEST_ALLOCATOR);
            ntsd::DataUtil::generateData(data.get(), k_MESSAGE_SIZE);

            ntcq::SendQueueEntry sendQueueEntry;
            sendQueueEntry.setId(sendQueue.generateEntryId());
            sendQueueEntry.setData(data);
            sendQueueEntry.setLength(data->size());

            sendQueue.pushEntry(sendQueueEntry);
        }

        NTSCFG_TEST_EQ(sendQueue.size(), k_MESSAGE_SIZE * 1);

        NTSCFG_TEST_FALSE(sendQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_FALSE(sendQueue.authorizeLowWatermarkEvent());

        NTSCFG_TEST_FALSE(sendQueue.isHighWatermarkViolated());
        NTSCFG_TEST_FALSE(sendQueue.authorizeHighWatermarkEvent());

        {
            bsl::shared_ptr<ntsa::Data> data;
            data.createInplace(NTSCFG_TEST_ALLOCATOR,
                               &blobBufferFactory,
                               NTSCFG_TEST_ALLOCATOR);
            ntsd::DataUtil::generateData(data.get(), k_MESSAGE_SIZE);

            ntcq::SendQueueEntry sendQueueEntry;
            sendQueueEntry.setId(sendQueue.generateEntryId());
            sendQueueEntry.setData(data);
            sendQueueEntry.setLength(data->size());

            sendQueue.pushEntry(sendQueueEntry);
        }

        NTSCFG_TEST_EQ(sendQueue.size(), k_MESSAGE_SIZE * 2);

        NTSCFG_TEST_FALSE(sendQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_FALSE(sendQueue.authorizeLowWatermarkEvent());

        NTSCFG_TEST_TRUE(sendQueue.isHighWatermarkViolated());
        NTSCFG_TEST_TRUE(sendQueue.authorizeHighWatermarkEvent());

        NTSCFG_TEST_TRUE(sendQueue.isHighWatermarkViolated());
        NTSCFG_TEST_FALSE(sendQueue.authorizeHighWatermarkEvent());

        NTSCFG_TEST_FALSE(sendQueue.popEntry());

        NTSCFG_TEST_EQ(sendQueue.size(), k_MESSAGE_SIZE * 1);

        NTSCFG_TEST_FALSE(sendQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_FALSE(sendQueue.authorizeLowWatermarkEvent());

        NTSCFG_TEST_FALSE(sendQueue.isHighWatermarkViolated());
        NTSCFG_TEST_FALSE(sendQueue.authorizeHighWatermarkEvent());

        NTSCFG_TEST_TRUE(sendQueue.popEntry());

        NTSCFG_TEST_EQ(sendQueue.size(), 0);

        NTSCFG_TEST_TRUE(sendQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_TRUE(sendQueue.authorizeLowWatermarkEvent());

        NTSCFG_TEST_TRUE(sendQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_FALSE(sendQueue.authorizeLowWatermarkEvent());

        NTSCFG_TEST_FALSE(sendQueue.isHighWatermarkViolated());
        NTSCFG_TEST_FALSE(sendQueue.authorizeHighWatermarkEvent());
    }
}

NTSCFG_TEST_FUNCTION(ntcq::SendQueueTest::verifyCase2)
{
    // Concern: Batching next suitable entries: empty

    ntcq::SendQueue sendQueue(NTSCFG_TEST_ALLOCATOR);

    ntsa::Data batch(NTSCFG_TEST_ALLOCATOR);
    batch.makeConstBufferArray();

    bool result =
        sendQueue.batchNext(&batch.constBufferArray(), ntsa::SendOptions());
    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_EQ(batch.constBufferArray().numBuffers(), 0);
    NTSCFG_TEST_EQ(batch.constBufferArray().numBytes(), 0);
}

NTSCFG_TEST_FUNCTION(ntcq::SendQueueTest::verifyCase3)
{
    // Concern: Batching next suitable entries: blob

    const bsl::size_t k_BLOB_BUFFER_SIZE = 32;
    const bsl::size_t k_MESSAGE_SIZE     = 1024;

    bdlbb::SimpleBlobBufferFactory blobBufferFactory(k_BLOB_BUFFER_SIZE,
                                                     NTSCFG_TEST_ALLOCATOR);

    ntcq::SendQueue sendQueue(NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    ntsd::DataUtil::generateData(&blob, k_MESSAGE_SIZE, 0, 0);

    {
        bsl::shared_ptr<ntsa::Data> data;
        data.createInplace(NTSCFG_TEST_ALLOCATOR,
                           blob,
                           &blobBufferFactory,
                           NTSCFG_TEST_ALLOCATOR);

        ntcq::SendQueueEntry sendQueueEntry;
        sendQueueEntry.setId(sendQueue.generateEntryId());
        sendQueueEntry.setData(data);
        sendQueueEntry.setLength(data->size());

        sendQueue.pushEntry(sendQueueEntry);
    }

    NTSCFG_TEST_EQ(sendQueue.size(), static_cast<bsl::size_t>(blob.length()));

    ntsa::Data batch(NTSCFG_TEST_ALLOCATOR);
    batch.makeConstBufferArray();

    bool result =
        sendQueue.batchNext(&batch.constBufferArray(), ntsa::SendOptions());
    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_EQ(batch.constBufferArray().numBuffers(), 0);
    NTSCFG_TEST_EQ(batch.constBufferArray().numBytes(), 0);
}

NTSCFG_TEST_FUNCTION(ntcq::SendQueueTest::verifyCase4)
{
    // Concern: Batching next suitable entries: blob -> blob

    const bsl::size_t k_BLOB_BUFFER_SIZE = 32;
    const bsl::size_t k_MESSAGE_SIZE     = 1024;
    const bsl::size_t k_MAX_BUFFERS      = 1024;

    bdlbb::SimpleBlobBufferFactory blobBufferFactory(k_BLOB_BUFFER_SIZE,
                                                     NTSCFG_TEST_ALLOCATOR);

    ntcq::SendQueue sendQueue(NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob1(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);
    bdlbb::Blob blob2(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    ntsd::DataUtil::generateData(&blob1, k_MESSAGE_SIZE * 1, 0, 0);
    ntsd::DataUtil::generateData(&blob2, k_MESSAGE_SIZE * 2, 0, 1);

    {
        bsl::shared_ptr<ntsa::Data> data;
        data.createInplace(NTSCFG_TEST_ALLOCATOR,
                           blob1,
                           &blobBufferFactory,
                           NTSCFG_TEST_ALLOCATOR);

        ntcq::SendQueueEntry sendQueueEntry;
        sendQueueEntry.setId(sendQueue.generateEntryId());
        sendQueueEntry.setData(data);
        sendQueueEntry.setLength(data->size());

        sendQueue.pushEntry(sendQueueEntry);
    }

    {
        bsl::shared_ptr<ntsa::Data> data;
        data.createInplace(NTSCFG_TEST_ALLOCATOR,
                           blob2,
                           &blobBufferFactory,
                           NTSCFG_TEST_ALLOCATOR);

        ntcq::SendQueueEntry sendQueueEntry;
        sendQueueEntry.setId(sendQueue.generateEntryId());
        sendQueueEntry.setData(data);
        sendQueueEntry.setLength(data->size());

        sendQueue.pushEntry(sendQueueEntry);
    }

    NTSCFG_TEST_EQ(sendQueue.size(),
                   static_cast<bsl::size_t>(blob1.length() + blob2.length()));

    ntsa::Data batch(NTSCFG_TEST_ALLOCATOR);
    batch.makeConstBufferArray();

    ntsa::SendOptions sendOptions;
    sendOptions.setMaxBuffers(k_MAX_BUFFERS);

    bool result = sendQueue.batchNext(&batch.constBufferArray(), sendOptions);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_EQ(batch.constBufferArray().numBuffers(),
                   static_cast<bsl::size_t>(blob1.numDataBuffers() +
                                            blob2.numDataBuffers()));

    NTSCFG_TEST_EQ(batch.constBufferArray().numBytes(),
                   static_cast<bsl::size_t>(blob1.length() + blob2.length()));

    ntsa::Data batchExpected(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);
    batchExpected.makeBlob();

    bdlbb::BlobUtil::append(&batchExpected.blob(), blob1);
    bdlbb::BlobUtil::append(&batchExpected.blob(), blob2);

    NTSCFG_TEST_TRUE(ntsa::DataUtil::equals(batch, batchExpected));
}

NTSCFG_TEST_FUNCTION(ntcq::SendQueueTest::verifyCase5)
{
    // Concern: Batching next suitable entries: blob -> blob -> file -> blob

    const bsl::size_t k_BLOB_BUFFER_SIZE = 32;
    const bsl::size_t k_MESSAGE_SIZE     = 1024;
    const bsl::size_t k_MAX_BUFFERS      = 1024;

    bdlbb::SimpleBlobBufferFactory blobBufferFactory(k_BLOB_BUFFER_SIZE,
                                                     NTSCFG_TEST_ALLOCATOR);

    ntcq::SendQueue sendQueue(NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob1(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);
    bdlbb::Blob blob2(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);
    ntsa::File  file3;
    bdlbb::Blob blob4(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    ntsd::DataUtil::generateData(&blob1, k_MESSAGE_SIZE * 1, 0, 0);
    ntsd::DataUtil::generateData(&blob2, k_MESSAGE_SIZE * 2, 0, 1);

    file3.setDescriptor((bdls::FilesystemUtil::FileDescriptor)(1));
    file3.setBytesRemaining(static_cast<bdls::FilesystemUtil::Offset>(1));

    ntsd::DataUtil::generateData(&blob4, k_MESSAGE_SIZE * 4, 0, 0);

    {
        bsl::shared_ptr<ntsa::Data> data;
        data.createInplace(NTSCFG_TEST_ALLOCATOR,
                           blob1,
                           &blobBufferFactory,
                           NTSCFG_TEST_ALLOCATOR);

        ntcq::SendQueueEntry sendQueueEntry;
        sendQueueEntry.setId(sendQueue.generateEntryId());
        sendQueueEntry.setData(data);
        sendQueueEntry.setLength(data->size());

        sendQueue.pushEntry(sendQueueEntry);
    }

    {
        bsl::shared_ptr<ntsa::Data> data;
        data.createInplace(NTSCFG_TEST_ALLOCATOR,
                           blob2,
                           &blobBufferFactory,
                           NTSCFG_TEST_ALLOCATOR);

        ntcq::SendQueueEntry sendQueueEntry;
        sendQueueEntry.setId(sendQueue.generateEntryId());
        sendQueueEntry.setData(data);
        sendQueueEntry.setLength(data->size());

        sendQueue.pushEntry(sendQueueEntry);
    }

    {
        bsl::shared_ptr<ntsa::Data> data;
        data.createInplace(NTSCFG_TEST_ALLOCATOR,
                           file3,
                           NTSCFG_TEST_ALLOCATOR);

        ntcq::SendQueueEntry sendQueueEntry;
        sendQueueEntry.setId(sendQueue.generateEntryId());
        sendQueueEntry.setData(data);
        sendQueueEntry.setLength(data->size());

        sendQueue.pushEntry(sendQueueEntry);
    }

    {
        bsl::shared_ptr<ntsa::Data> data;
        data.createInplace(NTSCFG_TEST_ALLOCATOR,
                           blob4,
                           &blobBufferFactory,
                           NTSCFG_TEST_ALLOCATOR);

        ntcq::SendQueueEntry sendQueueEntry;
        sendQueueEntry.setId(sendQueue.generateEntryId());
        sendQueueEntry.setData(data);
        sendQueueEntry.setLength(data->size());

        sendQueue.pushEntry(sendQueueEntry);
    }

    NTSCFG_TEST_EQ(
        sendQueue.size(),
        static_cast<bsl::size_t>(blob1.length() + blob2.length() +
                                 file3.bytesRemaining() + blob4.length()));

    ntsa::Data batch(NTSCFG_TEST_ALLOCATOR);
    batch.makeConstBufferArray();

    ntsa::SendOptions sendOptions;
    sendOptions.setMaxBuffers(k_MAX_BUFFERS);

    bool result = sendQueue.batchNext(&batch.constBufferArray(), sendOptions);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_EQ(batch.constBufferArray().numBuffers(),
                   static_cast<bsl::size_t>(blob1.numDataBuffers() +
                                            blob2.numDataBuffers()));

    NTSCFG_TEST_EQ(batch.constBufferArray().numBytes(),
                   static_cast<bsl::size_t>(blob1.length() + blob2.length()));

    ntsa::Data batchExpected(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);
    batchExpected.makeBlob();

    bdlbb::BlobUtil::append(&batchExpected.blob(), blob1);
    bdlbb::BlobUtil::append(&batchExpected.blob(), blob2);

    NTSCFG_TEST_TRUE(ntsa::DataUtil::equals(batch, batchExpected));
}

NTSCFG_TEST_FUNCTION(ntcq::SendQueueTest::verifyCase6)
{
    // Concern: Batching next suitable entries: limit maximum buffers to the
    // maximum number sendable per system call (to avoid EMSGBUF).

    const bsl::size_t k_BLOB_BUFFER_SIZE = 32;
    const bsl::size_t k_MESSAGE_SIZE     = 1024;

    bdlbb::SimpleBlobBufferFactory blobBufferFactory(k_BLOB_BUFFER_SIZE,
                                                     NTSCFG_TEST_ALLOCATOR);

    ntcq::SendQueue sendQueue(NTSCFG_TEST_ALLOCATOR);

    bdlbb::Blob blob1(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);
    bdlbb::Blob blob2(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

    ntsd::DataUtil::generateData(&blob1, k_MESSAGE_SIZE * 1, 0, 0);
    ntsd::DataUtil::generateData(&blob2, k_MESSAGE_SIZE * 2, 0, 1);

    {
        bsl::shared_ptr<ntsa::Data> data;
        data.createInplace(NTSCFG_TEST_ALLOCATOR,
                           blob1,
                           &blobBufferFactory,
                           NTSCFG_TEST_ALLOCATOR);

        ntcq::SendQueueEntry sendQueueEntry;
        sendQueueEntry.setId(sendQueue.generateEntryId());
        sendQueueEntry.setData(data);
        sendQueueEntry.setLength(data->size());

        sendQueue.pushEntry(sendQueueEntry);
    }

    {
        bsl::shared_ptr<ntsa::Data> data;
        data.createInplace(NTSCFG_TEST_ALLOCATOR,
                           blob2,
                           &blobBufferFactory,
                           NTSCFG_TEST_ALLOCATOR);

        ntcq::SendQueueEntry sendQueueEntry;
        sendQueueEntry.setId(sendQueue.generateEntryId());
        sendQueueEntry.setData(data);
        sendQueueEntry.setLength(data->size());

        sendQueue.pushEntry(sendQueueEntry);
    }

    NTSCFG_TEST_EQ(sendQueue.size(),
                   static_cast<bsl::size_t>(blob1.length() + blob2.length()));

    ntsa::Data batch(NTSCFG_TEST_ALLOCATOR);
    batch.makeConstBufferArray();

    ntsa::SendOptions sendOptions;
    sendOptions.setMaxBuffers(
        static_cast<bsl::size_t>(blob1.numDataBuffers()));

    bool result = sendQueue.batchNext(&batch.constBufferArray(), sendOptions);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_EQ(batch.constBufferArray().numBuffers(),
                   static_cast<bsl::size_t>(blob1.numDataBuffers()));

    NTSCFG_TEST_EQ(batch.constBufferArray().numBytes(),
                   static_cast<bsl::size_t>(blob1.length()));

    ntsa::Data batchExpected(&blobBufferFactory, NTSCFG_TEST_ALLOCATOR);
    batchExpected.makeBlob();

    bdlbb::BlobUtil::append(&batchExpected.blob(), blob1);

    NTSCFG_TEST_TRUE(ntsa::DataUtil::equals(batch, batchExpected));
}

}  // close namespace ntcq
}  // close namespace BloombergLP
