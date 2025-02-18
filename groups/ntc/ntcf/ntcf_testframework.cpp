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

#ifndef INCLUDED_NTCF_TESTFRAMEWORK_CPP
#define INCLUDED_NTCF_TESTFRAMEWORK_CPP

#include <ntcf_testframework.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcf_testframework_cpp, "$Id$ $CSID$")

#include <ntcs_blobutil.h>

namespace BloombergLP {
namespace ntcf {

ntsa::Error TestMessage::decode(bdlbb::Blob* source)
{
    ntsa::Error error;

    ntci::Serialization serialization;

    const bsl::size_t sourceSize = static_cast<bsl::size_t>(source->length());

    if (sourceSize < sizeof(ntcf::TestMessageHeader)) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    bdlbb::InBlobStreamBuf isb(source);

    bsl::streampos offsetToHeader =
            isb.pubseekoff(0, bsl::ios_base::cur, bsl::ios_base::in);
    if (offsetToHeader == bsl::streampos(-1)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::streamsize numHeaderBytesRead = 
        isb.sgetn(reinterpret_cast<char*>(&d_frame.header), 
        static_cast<bsl::streamsize>(sizeof(ntcf::TestMessageHeader)));

    if (numHeaderBytesRead != 
        static_cast<bsl::streamsize>(sizeof(ntcf::TestMessageHeader))) 
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t headerSize = 
        static_cast<bsl::size_t>(d_frame.header.headerSize);
    if (headerSize != sizeof(ntcf::TestMessageHeader)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t pragmaSize = 
        static_cast<bsl::size_t>(d_frame.header.pragmaSize);
    if (pragmaSize != static_cast<bsl::size_t>(k_MAX_PRAGMA_SIZE)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t entitySize = 
        static_cast<bsl::size_t>(d_frame.header.entitySize);
    if (entitySize != static_cast<bsl::size_t>(k_MAX_ENTITY_SIZE)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t messageSize = 
        static_cast<bsl::size_t>(d_frame.header.messageSize);

    if (messageSize != headerSize + pragmaSize + entitySize) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntca::SerializationType::Value serializationType;
    if (ntca::SerializationType::fromInt(
        &serializationType, 
        static_cast<int>(d_frame.header.serialization)) != 0)
    {
        NTCCFG_WARNING_UNUSED(serializationType);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntca::CompressionType::Value compressionType;
    if (ntca::CompressionType::fromInt(
        &compressionType, 
        static_cast<int>(d_frame.header.compression)) != 0)
    {
        NTCCFG_WARNING_UNUSED(compressionType);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (pragmaSize > 0) {
        error = serialization.decode(&d_frame.pragma.makeValue(), 
                                     *source, 
                                     ntca::SerializationType::e_BER);
        if (error) {
            return error;
        }
    }

    if (entitySize > 0) {
        error = serialization.decode(&d_frame.entity.makeValue(), 
                                     *source, 
                                     serializationType);
        if (error) {
            return error;
        }
    }

    bsl::streampos offsetToEnd =
            isb.pubseekoff(0, bsl::ios_base::cur, bsl::ios_base::in);
    if (offsetToEnd == bsl::streampos(-1)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::size_t numBytesRead = 
        static_cast<bsl::size_t>(offsetToEnd - offsetToHeader);

    isb.reset();

    bdlbb::BlobUtil::erase(source, 0, static_cast<int>(numBytesRead));

    return ntsa::Error();
}

ntsa::Error TestMessage::encode(bdlbb::Blob* destination) const
{
    ntsa::Error error;
    int         rc;

    ntci::Serialization serialization;

    ntcf::TestMessageHeader header = d_frame.header;

    const ntca::SerializationType::Value serializationType = 
        static_cast<ntca::SerializationType::Value>(
            static_cast<int>(d_frame.header.serialization));
    NTCCFG_WARNING_UNUSED(serializationType);

    const ntca::CompressionType::Value compressionType = 
        static_cast<ntca::CompressionType::Value>(
            static_cast<int>(d_frame.header.compression));
    NTCCFG_WARNING_UNUSED(compressionType);

    bdlbb::OutBlobStreamBuf osb(destination);

    const int blobLengthInitial = destination->length();

    bsl::streampos offsetToHeader = 0;

    if (blobLengthInitial > 0) {
        offsetToHeader =
            osb.pubseekoff(0, bsl::ios_base::end, bsl::ios_base::out);
        if (offsetToHeader == bsl::streampos(-1)) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    {
        bsl::streamsize numHeaderBytesWritten = 
            osb.sputn(reinterpret_cast<const char*>(&header),                   
            static_cast<bsl::streamsize>(sizeof(ntcf::TestMessageHeader)));
        if (numHeaderBytesWritten != 
            static_cast<bsl::streamsize>(sizeof(ntcf::TestMessageHeader))) 
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
   }
    
    rc = osb.pubsync();
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::streampos offsetToPragma =
        osb.pubseekoff(0, bsl::ios_base::cur, bsl::ios_base::out);
    if (offsetToPragma == bsl::streampos(-1)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::streampos offsetToEntity = 0;

    if (d_frame.pragma.has_value()) {
        error = serialization.encode(&osb, 
                                     d_frame.pragma.value(), 
                                     ntca::SerializationType::e_BER);
        if (error) {
            return error;
        }

        rc = osb.pubsync();
        if (rc != 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        offsetToEntity =
            osb.pubseekoff(0, bsl::ios_base::cur, bsl::ios_base::out);
        if (offsetToEntity == bsl::streampos(-1)) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        offsetToEntity = offsetToPragma;
    }

    bsl::streampos offsetToEnd = 0;

    if (d_frame.entity.has_value()) {
        error = serialization.encode(&osb, 
                                     d_frame.entity.value(), 
                                     serializationType);
        if (error) {
            return error;
        }

        rc = osb.pubsync();
        if (rc != 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        offsetToEnd =
            osb.pubseekoff(0, bsl::ios_base::cur, bsl::ios_base::out);
        if (offsetToEnd == bsl::streampos(-1)) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        offsetToEnd = offsetToEntity;
    }

    {
        bsl::streamsize numHeaderBytesWritten = 
            osb.sputn(reinterpret_cast<const char*>(&header),                   
            static_cast<bsl::streamsize>(sizeof(ntcf::TestMessageHeader)));
        if (numHeaderBytesWritten != 
            static_cast<bsl::streamsize>(sizeof(ntcf::TestMessageHeader))) 
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    
    rc = osb.pubsync();
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    
    const bsl::size_t headerSize =
        static_cast<bsl::size_t>(offsetToPragma - offsetToHeader);

    const bsl::size_t pragmaSize =
        static_cast<bsl::size_t>(offsetToEntity - offsetToPragma);

    const bsl::size_t entitySize =
        static_cast<bsl::size_t>(offsetToEnd - offsetToEntity);

    header.headerSize = static_cast<bsl::uint32_t>(headerSize);
    header.pragmaSize = static_cast<bsl::uint32_t>(pragmaSize);
    header.entitySize = static_cast<bsl::uint32_t>(entitySize);

    header.messageSize = 
        static_cast<bsl::uint32_t>(headerSize + pragmaSize + entitySize);

    bsl::streampos offsetToFixup = osb.pubseekpos(offsetToHeader,
                                                  bsl::ios_base::out);

    if (offsetToFixup != offsetToHeader) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    {
        bsl::streamsize numHeaderBytesWritten = 
            osb.sputn(reinterpret_cast<const char*>(&header),                   
            static_cast<bsl::streamsize>(sizeof(ntcf::TestMessageHeader)));
        if (numHeaderBytesWritten != 
            static_cast<bsl::streamsize>(sizeof(ntcf::TestMessageHeader))) 
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
   }

    rc = osb.pubsync();
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    osb.reset();

    const int blobLengthFinal = destination->length();
    NTSCFG_WARNING_UNUSED(blobLengthFinal);

    BSLS_ASSERT(headerSize + pragmaSize + entitySize ==
                static_cast<bsl::size_t>(blobLengthFinal - blobLengthInitial));

    return ntsa::Error();
}

bool TestMessage::equals(const TestMessage& other) const
{
    return d_frame == other.d_frame;
}

bool TestMessage::less(const TestMessage& other) const
{
    return d_frame < other.d_frame;
}

bsl::ostream& TestMessage::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("header", d_frame.header);
    printer.printAttribute("pragma", d_frame.pragma);
    printer.printAttribute("entity", d_frame.entity);
    printer.end();
    return stream;
}

void TestMessagePool::construct(
    void*                                  address,
    bslma::Allocator*                      allocator)
{
    new (address) ntcf::TestMessage(allocator);
}

TestMessagePool::TestMessagePool(
    bslma::Allocator*                      basicAllocator)
: d_pool(NTCCFG_BIND(&TestMessagePool::construct,
                     NTCCFG_BIND_PLACEHOLDER_1,
                     NTCCFG_BIND_PLACEHOLDER_2),
         16,
         basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

TestMessagePool::~TestMessagePool()
{
}


ntsa::Error TestMessageParser::process()
{
    ntsa::Error error;

    while (d_data_sp) {
        bsl::size_t dataLength = static_cast<bsl::size_t>(d_data_sp->length());

        if (dataLength == 0) {
            break;
        }

        if (d_state == e_WANT_HEADER) {
            if (dataLength >= k_HEADER_SIZE) {
                ntcf::TestMessageHeader header;
                {
                    bdlbb::InBlobStreamBuf isb(d_data_sp.get());

                    bsl::streamsize numHeaderBytesDecoded = 
                        isb.sgetn(
                            reinterpret_cast<char*>(&header), 
                            static_cast<bsl::streamsize>(k_HEADER_SIZE));

                    if (numHeaderBytesDecoded != 
                        static_cast<bsl::streamsize>(k_HEADER_SIZE))
                    {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }

                bsl::size_t messageSize = static_cast<bsl::size_t>(
                    static_cast<bsl::uint32_t>(header.messageSize));

                d_state     = e_WANT_MESSAGE;
                d_numNeeded = messageSize;
            }
            else {
                break;
            }
        }

        if (d_state == e_WANT_MESSAGE) {
            if (dataLength >= d_numNeeded) {
                bsl::shared_ptr<ntcf::TestMessage> message = 
                    d_messagePool_sp->create();

                error = message->decode(d_data_sp.get());
                if (error) {
                    return error;
                }

                d_messageQueue.resize(d_messageQueue.size() + 1);
                d_messageQueue.back().swap(message);

                d_state     = e_WANT_HEADER;
                d_numNeeded = k_HEADER_SIZE;
                continue;
            }
            else {
                break;
            }
        }
    }

    return ntsa::Error();
}

void TestMessageParser::reset()
{
    d_data_sp.reset();
    d_message_sp.reset();
    d_messageQueue.clear();
    d_state     = e_WANT_HEADER;
    d_numNeeded = k_HEADER_SIZE;
}

TestMessageParser::TestMessageParser(
    const bsl::shared_ptr<ntci::DataPool>&        dataPool,
    const bsl::shared_ptr<ntcf::TestMessagePool>& messagePool,
    const bsl::shared_ptr<ntci::Serialization>&   serialization,
    const bsl::shared_ptr<ntci::Compression>&     compression,
    bslma::Allocator*                             basicAllocator)
: d_dataPool_sp(dataPool)
, d_data_sp()
, d_messagePool_sp(messagePool)
, d_messageQueue(basicAllocator)
, d_message_sp()
, d_serialization_sp(serialization)
, d_compression_sp(compression)
, d_state(e_WANT_HEADER)
, d_numNeeded(k_HEADER_SIZE)
, d_closed(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

TestMessageParser::~TestMessageParser()
{
}

ntsa::Error TestMessageParser::add(const bdlbb::Blob& blob)
{
    ntsa::Error error;

    if (d_closed) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!d_data_sp) {
        d_data_sp = d_dataPool_sp->createIncomingBlob();
    }

    bdlbb::BlobUtil::append(d_data_sp.get(), blob);

    error = this->process();
    if (error) {
        this->reset();
        return error;
    }

    return ntsa::Error();
}

ntsa::Error TestMessageParser::add(const bsl::shared_ptr<bdlbb::Blob>& blob)
{
    ntsa::Error error;

    if (d_closed) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_data_sp) {
        bdlbb::BlobUtil::append(d_data_sp.get(), *blob);
    }
    else {
        d_data_sp = blob;
    }

    error = this->process();
    if (error) {
        this->reset();
        return error;
    }

    return ntsa::Error();
}

void TestMessageParser::close()
{
    if (d_closed) {
        return;
    }

    d_closed = true;
}

ntsa::Error TestMessageParser::dequeue(
    bsl::shared_ptr<ntcf::TestMessage>* request)
{
    request->reset();

    if (d_messageQueue.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    *request = d_messageQueue.front();
    d_messageQueue.erase(d_messageQueue.begin());

    return ntsa::Error();
}

bsl::size_t TestMessageParser::numNeeded() const
{
    return d_numNeeded;
}

bool TestMessageParser::hasAnyAvailable() const
{
    return !d_messageQueue.empty();
}

TestTradeCallbackFactory::~TestTradeCallbackFactory()
{
}

void TestTradeFuture::arrive(const ntcf::TestTradeResult& result)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);
    d_resultQueue.push_back(result);
    d_condition.signal();
}

TestTradeFuture::TestTradeFuture(bslma::Allocator* basicAllocator)
: ntcf::TestTradeCallback(basicAllocator)
, d_mutex()
, d_condition()
, d_resultQueue(basicAllocator)
{
    this->setFunction(bdlf::MemFnUtil::memFn(&TestTradeFuture::arrive, this));
}

TestTradeFuture::~TestTradeFuture()
{
}

ntsa::Error TestTradeFuture::wait(ntcf::TestTradeResult* result)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    while (d_resultQueue.empty()) {
        d_condition.wait(&d_mutex);
    }

    *result = d_resultQueue.front();
    d_resultQueue.pop_front();

    return ntsa::Error();
}

ntsa::Error TestTradeFuture::wait(ntcf::TestTradeResult*    result,
                                  const bsls::TimeInterval& timeout)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    while (d_resultQueue.empty()) {
        int rc = d_condition.timedWait(&d_mutex, timeout);
        if (rc == 0) {
            break;
        }
        else if (rc == bslmt::Condition::e_TIMED_OUT) {
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    *result = d_resultQueue.front();
    d_resultQueue.pop_front();

    return ntsa::Error();
}

















TestAcknowledgmentCallbackFactory::~TestAcknowledgmentCallbackFactory()
{
}

void TestAcknowledgmentFuture::arrive(const ntcf::TestAcknowledgmentResult& result)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);
    d_resultQueue.push_back(result);
    d_condition.signal();
}

TestAcknowledgmentFuture::TestAcknowledgmentFuture(bslma::Allocator* basicAllocator)
: ntcf::TestAcknowledgmentCallback(basicAllocator)
, d_mutex()
, d_condition()
, d_resultQueue(basicAllocator)
{
    this->setFunction(bdlf::MemFnUtil::memFn(&TestAcknowledgmentFuture::arrive, this));
}

TestAcknowledgmentFuture::~TestAcknowledgmentFuture()
{
}

ntsa::Error TestAcknowledgmentFuture::wait(ntcf::TestAcknowledgmentResult* result)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    while (d_resultQueue.empty()) {
        d_condition.wait(&d_mutex);
    }

    *result = d_resultQueue.front();
    d_resultQueue.pop_front();

    return ntsa::Error();
}

ntsa::Error TestAcknowledgmentFuture::wait(ntcf::TestAcknowledgmentResult*    result,
                                  const bsls::TimeInterval& timeout)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    while (d_resultQueue.empty()) {
        int rc = d_condition.timedWait(&d_mutex, timeout);
        if (rc == 0) {
            break;
        }
        else if (rc == bslmt::Condition::e_TIMED_OUT) {
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    *result = d_resultQueue.front();
    d_resultQueue.pop_front();

    return ntsa::Error();
}



}  // close package namespace
}  // close enterprise namespace
#endif
