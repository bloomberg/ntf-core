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

#ifndef INCLUDED_NTCF_TESTCLIENT_CPP
#define INCLUDED_NTCF_TESTCLIENT_CPP

#include <ntcf_testclient.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcf_testclient_cpp, "$Id$ $CSID$")

#define NTCF_TESTCLIENT_DATAGRAM_SOCKET_ENABLED 0

namespace BloombergLP {
namespace ntcf {

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_ESTABLISHED(datagramSocket)       \
    do {                                                                      \
        BALL_LOG_INFO << "Client datagram socket at "                         \
                      << (datagramSocket)->sourceEndpoint() << " to "         \
                      << (datagramSocket)->remoteEndpoint() << " established" \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_CONNECTED(datagramSocket)         \
    do {                                                                      \
        BALL_LOG_INFO << "Client datagram socket at "                         \
                      << (datagramSocket)->sourceEndpoint() << " to "         \
                      << (datagramSocket)->remoteEndpoint() << " connected"   \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_CLOSED(datagramSocket)            \
    do {                                                                      \
        BALL_LOG_INFO << "Client datagram socket at "                         \
                      << (datagramSocket)->sourceEndpoint() << " to "         \
                      << (datagramSocket)->remoteEndpoint() << " closed"      \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,             \
                                                  type,                       \
                                                  event)                      \
    do {                                                                      \
        BALL_LOG_TRACE << "Client datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint()                  \
                       << " processing " << (type) << " event " << (event)    \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RECEIVE_WOULD_BLOCK(              \
    datagramSocket)                                                           \
    do {                                                                      \
        BALL_LOG_TRACE << "Client datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint()                  \
                       << " receive WOULD_BLOCK" << BALL_LOG_END;             \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RECEIVE_EOF(datagramSocket)       \
    do {                                                                      \
        BALL_LOG_TRACE << "Client datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint()                  \
                       << " receive EOF" << BALL_LOG_END;                     \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RECEIVE_FAILED(datagramSocket,    \
                                                           error)             \
    do {                                                                      \
        BALL_LOG_TRACE << "Client datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint()                  \
                       << " receive failed: " << (error) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RECEIVE_CONTEXT(datagramSocket,   \
                                                            blob,             \
                                                            receiveContext)   \
    do {                                                                      \
        BALL_LOG_TRACE << "Client datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint() << " received "  \
                       << (blob).length() << " bytes from context "           \
                       << (receiveContext) << BALL_LOG_END;                   \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_PARSE_FAILED(datagramSocket,      \
                                                         error)               \
    do {                                                                      \
        BALL_LOG_ERROR << "Client datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint()                  \
                       << " parsing failed: " << (error) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RESPONSE_IGNORED(datagramSocket,  \
                                                             response)        \
    do {                                                                      \
        BALL_LOG_WARN << "Client datagram socket at "                         \
                      << (datagramSocket)->sourceEndpoint() << " to "         \
                      << (datagramSocket)->remoteEndpoint()                   \
                      << " ignoring response " << (*response)                 \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RESPONSE_UNSOLICITED(             \
    datagramSocket,                                                           \
    response)                                                                 \
    do {                                                                      \
        BALL_LOG_WARN << "Client datagram socket at "                         \
                      << (datagramSocket)->sourceEndpoint() << " to "         \
                      << (datagramSocket)->remoteEndpoint()                   \
                      << " received stale or unsolicited response "           \
                      << (*response) << BALL_LOG_END;                         \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_UNEXPECTED_MESSAGE(               \
    datagramSocket,                                                           \
    message)                                                                  \
    do {                                                                      \
        BALL_LOG_WARN << "Client stream socket at "                           \
                      << (datagramSocket)->sourceEndpoint() << " to "         \
                      << (datagramSocket)->remoteEndpoint()                   \
                      << " ignoring unexpected message " << (*message)        \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_INCOMING_MESSAGE(datagramSocket,  \
                                                             message)         \
    do {                                                                      \
        BALL_LOG_DEBUG << "Client datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint()                  \
                       << " received message " << (*message) << BALL_LOG_END; \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_OUTGOING_MESSAGE(datagramSocket,  \
                                                             message)         \
    do {                                                                      \
        BALL_LOG_DEBUG << "Client datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint()                  \
                       << " sending message " << (*message) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_OUTGOING_BLOB(datagramSocket,     \
                                                          blob)               \
    do {                                                                      \
        BALL_LOG_TRACE << "Client datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint() << " sending:\n" \
                       << bdlbb::BlobUtilHexDumper(&(blob)) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_INCOMING_BLOB(datagramSocket,     \
                                                          blob)               \
    do {                                                                      \
        BALL_LOG_TRACE << "Client datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint()                  \
                       << " received:\n"                                      \
                       << bdlbb::BlobUtilHexDumper(&(blob)) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_CONNECTED(streamSocket)             \
    do {                                                                      \
        BALL_LOG_INFO << "Client stream socket at "                           \
                      << (streamSocket)->sourceEndpoint() << " to "           \
                      << (streamSocket)->remoteEndpoint() << " connected"     \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_CLOSED(streamSocket)                \
    do {                                                                      \
        BALL_LOG_INFO << "Client stream socket at "                           \
                      << (streamSocket)->sourceEndpoint() << " to "           \
                      << (streamSocket)->remoteEndpoint() << " closed"        \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, type, event)    \
    do {                                                                      \
        BALL_LOG_TRACE << "Client stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " processing "  \
                       << (type) << " event " << (event) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RECEIVE_WOULD_BLOCK(streamSocket)   \
    do {                                                                      \
        BALL_LOG_TRACE << "Client stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " receive WOULD_BLOCK" << BALL_LOG_END;             \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RECEIVE_EOF(streamSocket)           \
    do {                                                                      \
        BALL_LOG_TRACE << "Client stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " receive EOF"  \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RECEIVE_FAILED(streamSocket, error) \
    do {                                                                      \
        BALL_LOG_TRACE << "Client stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " receive failed: " << (error) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RECEIVE_CONTEXT(streamSocket,       \
                                                          blob,               \
                                                          receiveContext)     \
    do {                                                                      \
        BALL_LOG_TRACE << "Client stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " received "    \
                       << (blob).length() << " bytes from context "           \
                       << (receiveContext) << BALL_LOG_END;                   \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_PARSE_FAILED(streamSocket, error)   \
    do {                                                                      \
        BALL_LOG_ERROR << "Client stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " parsing failed: " << (error) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RESPONSE_IGNORED(streamSocket,      \
                                                           response)          \
    do {                                                                      \
        BALL_LOG_WARN << "Client stream socket at "                           \
                      << (streamSocket)->sourceEndpoint() << " to "           \
                      << (streamSocket)->remoteEndpoint()                     \
                      << " ignoring response " << (*response)                 \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RESPONSE_UNSOLICITED(streamSocket,  \
                                                               response)      \
    do {                                                                      \
        BALL_LOG_WARN << "Client stream socket at "                           \
                      << (streamSocket)->sourceEndpoint() << " to "           \
                      << (streamSocket)->remoteEndpoint()                     \
                      << " received stale or unsolicited response "           \
                      << (*response) << BALL_LOG_END;                         \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_UNEXPECTED_MESSAGE(streamSocket,    \
                                                             message)         \
    do {                                                                      \
        BALL_LOG_WARN << "Client stream socket at "                           \
                      << (streamSocket)->sourceEndpoint() << " to "           \
                      << (streamSocket)->remoteEndpoint()                     \
                      << " ignoring unexpected message " << (*message)        \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_INCOMING_MESSAGE(streamSocket,      \
                                                           message)           \
    do {                                                                      \
        BALL_LOG_DEBUG << "Client stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " received message " << (*message) << BALL_LOG_END; \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_OUTGOING_MESSAGE(streamSocket,      \
                                                           message)           \
    do {                                                                      \
        BALL_LOG_DEBUG << "Client stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " sending message " << (*message) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_OUTGOING_BLOB(streamSocket, blob)   \
    do {                                                                      \
        BALL_LOG_TRACE << "Client stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " sending:\n"   \
                       << bdlbb::BlobUtilHexDumper(&(blob)) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_INCOMING_BLOB(streamSocket, blob)   \
    do {                                                                      \
        BALL_LOG_TRACE << "Client stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " received:\n"  \
                       << bdlbb::BlobUtilHexDumper(&(blob)) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTCLIENT_LOG_SIGNAL_FAILURE(streamSocket, failure) \
    do {                                                                      \
        BALL_LOG_ERROR << "Client stream socket at "                          \
                    << (streamSocket)->sourceEndpoint() << " to "          \
                    << (streamSocket)->remoteEndpoint() \
                    << " failed to execute 'signal': fault = "  \
                    << (failure) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTCLIENT_LOG_ENCRYPTION_FAILURE(streamSocket, failure) \
    do {                                                                      \
        BALL_LOG_ERROR << "Client stream socket at "                          \
                    << (streamSocket)->sourceEndpoint() << " to "          \
                    << (streamSocket)->remoteEndpoint() \
                    << " failed to execute 'encrypt': fault = "  \
                    << (failure) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTCLIENT_LOG_COMPRESS_FAILURE(streamSocket, failure) \
    do {                                                                      \
        BALL_LOG_ERROR << "Client stream socket at "                          \
                    << (streamSocket)->sourceEndpoint() << " to "          \
                    << (streamSocket)->remoteEndpoint() \
                    << " failed to execute 'compress': fault = "  \
                    << (failure) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTCLIENT_LOG_HEARTBEAT_FAILURE(streamSocket, failure) \
    do {                                                                      \
        BALL_LOG_ERROR << "Client stream socket at "                          \
                    << (streamSocket)->sourceEndpoint() << " to "          \
                    << (streamSocket)->remoteEndpoint() \
                    << " failed to execute 'heartbeat': fault = "  \
                    << (failure) << BALL_LOG_END;  \
    } while (false)

TestClientTransaction::TestClientTransaction(
    bsl::uint64_t                             id,
    const bsl::shared_ptr<ntcf::TestMessage>& request,
    const ntcf::TestOptions&                  options,
    const ntcf::TestMessageCallback&          callback,
    bslma::Allocator*                         basicAllocator)
: d_lock(bsls::SpinLock::s_unlocked)
, d_id(id)
, d_request_sp(request)
, d_timer_sp()
, d_options(options)
, d_callback(callback, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

TestClientTransaction::~TestClientTransaction()
{
}

void TestClientTransaction::setTimer(const bsl::shared_ptr<ntci::Timer>& timer)
{
    bsls::SpinLockGuard lock(&d_lock);
    d_timer_sp = timer;
}

bool TestClientTransaction::invoke(
    const ntcf::TestContext&                  context,
    const ntcf::TestFault&                    fault,
    const bsl::shared_ptr<ntcf::TestMessage>& response,
    const bsl::shared_ptr<ntci::Strand>&      strand)
{
    bsl::shared_ptr<ntci::Timer> timer;
    ntcf::TestMessageCallback    callback(d_allocator_p);

    {
        bsls::SpinLockGuard lock(&d_lock);

        timer.swap(d_timer_sp);
        callback.swap(d_callback);
    }

    if (timer) {
        timer->close();
        timer.reset();
    }

    if (callback) {
        callback.execute(context, fault, response, strand);
        return true;
    }

    return false;
}

bool TestClientTransaction::dispatch(
    const ntcf::TestContext&                  context,
    const ntcf::TestFault&                    fault,
    const bsl::shared_ptr<ntcf::TestMessage>& response,
    const bsl::shared_ptr<ntci::Strand>&      strand,
    const bsl::shared_ptr<ntci::Executor>&    executor,
    bool                                      defer,
    ntci::Mutex*                              mutex)
{
    bsl::shared_ptr<ntci::Timer> timer;
    ntcf::TestMessageCallback    callback(d_allocator_p);

    {
        bsls::SpinLockGuard lock(&d_lock);

        timer.swap(d_timer_sp);
        callback.swap(d_callback);
    }

    if (timer) {
        timer->close();
        timer.reset();
    }

    if (callback) {
        callback.dispatch(context,
                          fault,
                          response,
                          strand,
                          executor,
                          defer,
                          mutex);
        return true;
    }

    return false;
}

void TestClientTransaction::timeout(
    const bsl::shared_ptr<ntci::Strand>& strand)
{
    ntcf::TestContext                  context;
    ntcf::TestFault                    fault;
    bsl::shared_ptr<ntcf::TestMessage> response;

    context.error = ntsa::Error(ntsa::Error::e_WOULD_BLOCK).number();

    fault.role        = ntcf::TestRole::e_CLIENT;
    fault.code        = ntcf::TestFaultCode::e_TIMEOUT;
    fault.description = "The operation timed out";

    bsl::shared_ptr<ntci::Timer> timer;
    ntcf::TestMessageCallback    callback(d_allocator_p);

    {
        bsls::SpinLockGuard lock(&d_lock);

        timer.swap(d_timer_sp);
        callback.swap(d_callback);
    }

    if (timer) {
        timer->close();
        timer.reset();
    }

    if (callback) {
        callback.execute(context, fault, response, strand);
    }
}

bsl::uint64_t TestClientTransaction::id() const
{
    return d_id;
}

const bsl::shared_ptr<ntcf::TestMessage>& TestClientTransaction::request()
    const
{
    return d_request_sp;
}

const ntcf::TestOptions& TestClientTransaction::options() const
{
    return d_options;
}

bool TestClientTransaction::hasTimer() const
{
    bsls::SpinLockGuard lock(&d_lock);
    return static_cast<bool>(d_timer_sp);
}

bool TestClientTransaction::hasCallback() const
{
    bsls::SpinLockGuard lock(&d_lock);
    return static_cast<bool>(d_callback);
}

TestClientTransactionPool::TestClientTransactionPool(
    bslma::Allocator* basicAllocator)
: d_pool(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

TestClientTransactionPool::~TestClientTransactionPool()
{
}

TestClientTransactionCatalog::TestClientTransactionCatalog(
    bslma::Allocator* basicAllocator)
: d_mutex()
, d_pool(basicAllocator)
, d_map(basicAllocator)
, d_generation(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

TestClientTransactionCatalog::~TestClientTransactionCatalog()
{
}

bsl::shared_ptr<ntcf::TestClientTransaction> TestClientTransactionCatalog::
    create(const bsl::shared_ptr<ntcf::TestMessage>& request,
           const ntcf::TestOptions&                  options,
           const ntcf::TestMessageCallback&          callback)
{
    LockGuard lock(&d_mutex);

    bsl::uint64_t id = 0;
    do {
        id = ++d_generation;
    } while (id == 0);

    bsl::shared_ptr<ntcf::TestClientTransaction> transaction =
        d_pool.create(id, request, options, callback);

    bsl::pair<Map::iterator, bool> insertResult =
        d_map.emplace(id, transaction);
    BSLS_ASSERT_OPT(insertResult.second);

    request->setTransaction(id);

    return transaction;
}

bool TestClientTransactionCatalog::remove(
    const bsl::shared_ptr<ntcf::TestMessage>& message)
{
    LockGuard lock(&d_mutex);

    const bsl::uint64_t id = message->transaction();

    if (id != 0) {
        bsl::size_t n = d_map.erase(id);
        return n != 0;
    }

    return false;
}

bool TestClientTransactionCatalog::remove(
    bsl::shared_ptr<ntcf::TestClientTransaction>* result,
    const bsl::shared_ptr<ntcf::TestMessage>&     message)
{
    LockGuard lock(&d_mutex);

    const bsl::uint64_t id = message->transaction();

    if (id != 0) {
        Map::iterator it = d_map.find(id);
        if (it == d_map.end()) {
            return false;
        }

        *result = it->second;
        d_map.erase(it);

        return true;
    }

    return false;
}

void TestClient::processDatagramSocketEstablished(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
{
    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_ESTABLISHED(datagramSocket);

    bsl::shared_ptr<Self> self(this->getSelf(this));
    LockGuard             lock(&d_mutex);

    this->privateDatagramSocketUp(self, datagramSocket);
}

void TestClient::processDatagramSocketClosed(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
{
    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_CLOSED(datagramSocket);

    bsl::shared_ptr<Self> self(this->getSelf(this));
    LockGuard             lock(&d_mutex);

    this->privateDatagramSocketDown(self, datagramSocket);
}

void TestClient::processStreamSocketEstablished(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
}

void TestClient::processStreamSocketClosed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCCFG_WARNING_UNUSED(streamSocket);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_CLOSED(streamSocket);

    bsl::shared_ptr<Self> self(this->getSelf(this));
    LockGuard             lock(&d_mutex);

    this->privateStreamSocketDown(self, streamSocket);
}

void TestClient::processReadQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "read queue",
                                              event);
}

void TestClient::processReadQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "read queue",
                                              event);
}

void TestClient::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "read queue",
                                              event);

    bsl::shared_ptr<Self> self(this->getSelf(this));

    ntsa::Error error;

    ntca::ReceiveContext receiveContext;
    ntca::ReceiveOptions receiveOptions;
    bdlbb::Blob          blob;

    receiveOptions.setMinSize(1);
    receiveOptions.setMaxSize(65507);

    error =
        d_datagramSocket_sp->receive(&receiveContext, &blob, receiveOptions);
    if (error) {
        if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK)) {
            NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RECEIVE_WOULD_BLOCK(
                d_datagramSocket_sp);
        }
        else if (error == ntsa::Error(ntsa::Error::e_EOF)) {
            NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RECEIVE_EOF(
                d_datagramSocket_sp);
        }
        else {
            NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RECEIVE_FAILED(
                d_datagramSocket_sp,
                error);
        }

        d_datagramSocket_sp->close();
        return;
    }

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RECEIVE_CONTEXT(d_datagramSocket_sp,
                                                        blob,
                                                        receiveContext);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_INCOMING_BLOB(d_datagramSocket_sp,
                                                      blob);

    error = d_datagramParser_sp->add(blob);
    if (error) {
        NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_PARSE_FAILED(d_datagramSocket_sp,
                                                         error);
        d_datagramSocket_sp->close();
        return;
    }

    while (d_datagramParser_sp->hasAnyAvailable()) {
        bsl::shared_ptr<ntcf::TestMessage> message;
        error = d_datagramParser_sp->dequeue(&message);
        if (error) {
            NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_PARSE_FAILED(
                d_datagramSocket_sp,
                error);
            d_datagramSocket_sp->close();
            return;
        }

        NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_INCOMING_MESSAGE(
            d_datagramSocket_sp,
            message);

        if (message->isResponse()) {
            bsl::shared_ptr<ntcf::TestClientTransaction> transaction;
            if (d_transactionCatalog.remove(&transaction, message)) {
                ntcf::TestContext                  context;
                ntcf::TestFault                    fault;
                bsl::shared_ptr<ntcf::TestMessage> response;

                this->analyzeIncomingMessage(&context,
                                             &fault,
                                             &response,
                                             message);

                if (!transaction->invoke(context,
                                         fault,
                                         response,
                                         ntci::Strand::unknown()))
                {
                    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RESPONSE_IGNORED(
                        d_datagramSocket_sp,
                        message);
                }
            }
            else {
                NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RESPONSE_UNSOLICITED(
                    d_datagramSocket_sp,
                    message);

                NTCCFG_ABORT();
            }
        }
        else if (message->isPublication()) {
            // MRM: TODO
        }
        else {
            NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_UNEXPECTED_MESSAGE(
                d_datagramSocket_sp,
                message);
        }
    }
}

void TestClient::processReadQueueHighWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "read queue",
                                              event);
}

void TestClient::processReadQueueDiscarded(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "read queue",
                                              event);
}

void TestClient::processWriteQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "write queue",
                                              event);
}

void TestClient::processWriteQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "write queue",
                                              event);
}

void TestClient::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "write queue",
                                              event);
}

void TestClient::processWriteQueueHighWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "write queue",
                                              event);
}

void TestClient::processWriteQueueDiscarded(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "write queue",
                                              event);
}

void TestClient::processShutdownInitiated(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "shutdown",
                                              event);
}

void TestClient::processShutdownReceive(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "shutdown",
                                              event);
}

void TestClient::processShutdownSend(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "shutdown",
                                              event);
}

void TestClient::processShutdownComplete(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "shutdown",
                                              event);
}

void TestClient::processError(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ErrorEvent&                      event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "error", event);
}

void TestClient::processReadQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "read queue", event);
}

void TestClient::processReadQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "read queue", event);
}

void TestClient::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "read queue", event);

    bsl::shared_ptr<Self> self(this->getSelf(this));

    ntsa::Error error;

    ntca::ReceiveContext receiveContext;
    ntca::ReceiveOptions receiveOptions;
    bdlbb::Blob          blob;

    receiveOptions.setMinSize(1);
    receiveOptions.setMaxSize(bsl::numeric_limits<bsl::size_t>::max());

    error = d_streamSocket_sp->receive(&receiveContext, &blob, receiveOptions);
    if (error) {
        if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK)) {
            NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RECEIVE_WOULD_BLOCK(
                d_streamSocket_sp);
        }
        else if (error == ntsa::Error(ntsa::Error::e_EOF)) {
            NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RECEIVE_EOF(d_streamSocket_sp);
        }
        else {
            NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RECEIVE_FAILED(d_streamSocket_sp,
                                                             error);
        }

        d_streamSocket_sp->close();
        return;
    }

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RECEIVE_CONTEXT(d_streamSocket_sp,
                                                      blob,
                                                      receiveContext);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_INCOMING_BLOB(d_streamSocket_sp, blob);

    error = d_streamParser_sp->add(blob);
    if (error) {
        NTCF_TESTCLIENT_LOG_STREAM_SOCKET_PARSE_FAILED(d_streamSocket_sp,
                                                       error);
        d_streamSocket_sp->close();
        return;
    }

    while (d_streamParser_sp->hasAnyAvailable()) {
        bsl::shared_ptr<ntcf::TestMessage> message;
        error = d_streamParser_sp->dequeue(&message);
        if (error) {
            NTCF_TESTCLIENT_LOG_STREAM_SOCKET_PARSE_FAILED(d_streamSocket_sp,
                                                           error);
            d_streamSocket_sp->close();
            return;
        }

        NTCF_TESTCLIENT_LOG_STREAM_SOCKET_INCOMING_MESSAGE(d_streamSocket_sp,
                                                           message);

        if (message->isResponse()) {
            bsl::shared_ptr<ntcf::TestClientTransaction> transaction;
            if (d_transactionCatalog.remove(&transaction, message)) {
                ntcf::TestContext                  context;
                ntcf::TestFault                    fault;
                bsl::shared_ptr<ntcf::TestMessage> response;

                this->analyzeIncomingMessage(&context,
                                             &fault,
                                             &response,
                                             message);

                if (!transaction->invoke(context,
                                         fault,
                                         response,
                                         ntci::Strand::unknown()))
                {
                    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RESPONSE_IGNORED(
                        d_streamSocket_sp,
                        message);
                }
            }
            else {
                NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RESPONSE_UNSOLICITED(
                    d_streamSocket_sp,
                    message);

                NTCCFG_ABORT();
            }
        }
        else if (message->isPublication()) {
            // MRM: TODO
        }
        else {
            NTCF_TESTCLIENT_LOG_STREAM_SOCKET_UNEXPECTED_MESSAGE(
                d_streamSocket_sp,
                message);
        }
    }

    d_streamSocket_sp->setReadQueueLowWatermark(
        d_streamParser_sp->numNeeded());
}

void TestClient::processReadQueueHighWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "read queue", event);
}

void TestClient::processReadQueueDiscarded(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "read queue", event);
}

void TestClient::processWriteQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket,
                                            "write queue",
                                            event);
}

void TestClient::processWriteQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket,
                                            "write queue",
                                            event);
}

void TestClient::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket,
                                            "write queue",
                                            event);
}

void TestClient::processWriteQueueHighWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket,
                                            "write queue",
                                            event);
}

void TestClient::processWriteQueueDiscarded(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket,
                                            "write queue",
                                            event);
}

void TestClient::processDowngradeInitiated(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::DowngradeEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "downgrade", event);
}

void TestClient::processDowngradeComplete(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::DowngradeEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "downgrade", event);

    bsl::shared_ptr<Self> self(this->getSelf(this));
    LockGuard             lock(&d_mutex);

    this->privateStreamSocketCompleteDowngrade(self, streamSocket, event);
}

void TestClient::processShutdownInitiated(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "shutdown", event);
}

void TestClient::processShutdownReceive(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "shutdown", event);
}

void TestClient::processShutdownSend(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "shutdown", event);
}

void TestClient::processShutdownComplete(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "shutdown", event);
}

void TestClient::processError(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ErrorEvent&                    event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "error", event);
}

void TestClient::processDatagramSocketConnectEvent(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ConnectEvent&                    event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCCFG_NOT_IMPLEMENTED();
}

void TestClient::processStreamSocketConnectEvent(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ConnectEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCCFG_NOT_IMPLEMENTED();
}

void TestClient::processStreamSocketUpgradeEvent(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::UpgradeEvent&                  event)
{
    bsl::shared_ptr<Self> self(this->getSelf(this));
    LockGuard             lock(&d_mutex);

    this->privateStreamSocketCompleteUpgrade(self, streamSocket, event);
}

void TestClient::privateStreamSocketInitiateUpgrade(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(streamSocket);

    NTCCFG_NOT_IMPLEMENTED();
}

void TestClient::privateStreamSocketCompleteUpgrade(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::UpgradeEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void TestClient::privateStreamSocketInitiateDowngrade(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(streamSocket);
}

void TestClient::privateStreamSocketCompleteDowngrade(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::DowngradeEvent&                event)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    d_downgradeSemaphore.post();
}

void TestClient::privateDatagramSocketUp(
    const bsl::shared_ptr<Self>&                 self,
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(datagramSocket);
}

void TestClient::privateDatagramSocketDown(
    const bsl::shared_ptr<Self>&                 self,
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(datagramSocket);
}

void TestClient::privateDatagramSocketError(
    const bsl::shared_ptr<Self>&                 self,
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntsa::Error&                           error)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(error);

    datagramSocket->close();
}

void TestClient::privateStreamSocketUp(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(streamSocket);
}

void TestClient::privateStreamSocketDown(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(streamSocket);
}

void TestClient::privateStreamSocketError(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntsa::Error&                         error)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(error);

    streamSocket->close();
}

void TestClient::analyzeIncomingMessage(
    ntcf::TestContext*                        context,
    ntcf::TestFault*                          fault,
    bsl::shared_ptr<ntcf::TestMessage>*       response,
    const bsl::shared_ptr<ntcf::TestMessage>& message)
{
    *response = message;

    const bsls::TimeInterval now = d_streamSocket_sp->currentTime();

    const bsls::TimeInterval clientTimestamp = message->clientTimestamp();

    const bsls::TimeInterval serverTimestamp = message->serverTimestamp();

    if (serverTimestamp > clientTimestamp) {
        context->latencyFromClient = serverTimestamp - clientTimestamp;
    }

    if (now > serverTimestamp) {
        context->latencyFromServer = now - serverTimestamp;
    }

    if (now > clientTimestamp) {
        context->latencyOverall = now - clientTimestamp;
    }

    if (message->pragma().has_value()) {
        const ntcf::TestMessagePragma& pragma = message->pragma().value();

        if (pragma.fault.has_value()) {
            *fault = pragma.fault.value();
            (*response)->reset();
        }
    }
    else if (message->entity().has_value()) {
        const ntcf::TestMessageEntity& entity = message->entity().value();

        if (entity.isContentValue()) {
            const ntcf::TestContent& content = entity.content();
            if (content.isFaultValue()) {
                *fault = content.fault();
                (*response)->reset();
            }
        }
        else if (entity.isContentValue()) {
            const ntcf::TestControl& control = entity.control();
            if (control.isFaultValue()) {
                *fault = control.fault();
                (*response)->reset();
            }
        }
    }
}

void TestClient::describeInititationFailure(ntcf::TestFault* fault)
{
    fault->reset();

    fault->role        = ntcf::TestRole::e_CLIENT;
    fault->code        = ntcf::TestFaultCode::e_INVALID;
    fault->description = "Failed to initiate operation";
    fault->uri         = d_streamSocket_sp->sourceEndpoint().text();
}

void TestClient::describeWaitFailure(ntcf::TestFault* fault)
{
    fault->reset();

    fault->role        = ntcf::TestRole::e_CLIENT;
    fault->code        = ntcf::TestFaultCode::e_INVALID;
    fault->description = "Failed to wait for operation to complete";
    fault->uri         = d_streamSocket_sp->sourceEndpoint().text();
}

void TestClient::describeResultTypeFailure(ntcf::TestFault* fault)
{
    fault->reset();

    fault->role        = ntcf::TestRole::e_CLIENT;
    fault->code        = ntcf::TestFaultCode::e_INVALID;
    fault->description = "The operation result type is not recognized";
    fault->uri         = d_streamSocket_sp->sourceEndpoint().text();
}

void TestClient::dispatchConnect(
        const bsl::shared_ptr<ntci::Connector>& connector,
        const ntca::ConnectEvent&               event,
        const ntci::ConnectCallback&            callback)
{
    ntsa::Error error;

    BSLS_ASSERT(connector == d_streamSocket_sp);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(d_streamSocket_sp,
        "connect",
        event);

    if (event.type() == ntca::ConnectEventType::e_COMPLETE) {
        if (d_datagramSocket_sp) {
            error = d_datagramSocket_sp->relaxFlowControl(
                ntca::FlowControlType::e_RECEIVE);
            BSLS_ASSERT_OPT(!error);
        }
    
        if (d_streamSocket_sp) {
            error = d_streamSocket_sp->setReadQueueLowWatermark(
                d_streamParser_sp->numNeeded());
            BSLS_ASSERT_OPT(!error);
        
            error = d_streamSocket_sp->relaxFlowControl(
                ntca::FlowControlType::e_RECEIVE);
            BSLS_ASSERT_OPT(!error);

            NTCF_TESTCLIENT_LOG_STREAM_SOCKET_CONNECTED(d_streamSocket_sp);
        }
    }
    else {
        BSLS_ASSERT_OPT(event.context().error());
    }

    if (callback) {
        callback.execute(connector, event, d_strand_sp);
    }
}

void TestClient::dispatchTrade(
    const ntcf::TestContext&                  context,
    const ntcf::TestFault&                    fault,
    const bsl::shared_ptr<ntcf::TestMessage>& message,
    const ntcf::TestTradeCallback&            callback)
{
    ntcf::TestTradeResult result;
    result.context = context;

    if (fault.code != ntcf::TestFaultCode::e_OK) {
        BSLS_ASSERT(message.get() == 0);
        result.value.makeFailure(fault);
    }
    else if (message) {
        if (message->entity().has_value()) {
            const ntcf::TestMessageEntity& entity = message->entity().value();
            if (entity.isContentValue()) {
                const ntcf::TestContent& content = entity.content();
                if (content.isTradeValue()) {
                    const ntcf::TestTrade& trade = content.trade();
                    result.value.makeSuccess(trade);
                }
            }
        }
    }

    if (result.value.isUndefinedValue()) {
        this->describeResultTypeFailure(&result.value.makeFailure());
    }

    if (callback) {
        callback.execute(result, d_strand_sp);
        return;
    }
    else {
        BALL_LOG_WARN << "Ignoring result " << result << BALL_LOG_END;
    }
}

void TestClient::dispatchEcho(
    const ntcf::TestContext&                  context,
    const ntcf::TestFault&                    fault,
    const bsl::shared_ptr<ntcf::TestMessage>& message,
    const ntcf::TestEchoCallback&             callback)
{
    ntcf::TestEchoResult result;
    result.context = context;

    if (fault.code != ntcf::TestFaultCode::e_OK) {
        BSLS_ASSERT(message.get() == 0);
        result.value.makeFailure(fault);
    }
    else if (message) {
        if (message->entity().has_value()) {
            const ntcf::TestMessageEntity& entity = message->entity().value();
            if (entity.isControlValue()) {
                const ntcf::TestControl& control = entity.control();
                if (control.isEchoValue()) {
                    const ntcf::TestEcho& echo = control.echo();
                    result.value.makeSuccess(echo);
                }
            }
        }
    }

    if (result.value.isUndefinedValue()) {
        this->describeResultTypeFailure(&result.value.makeFailure());
    }

    if (callback) {
        callback.execute(result, d_strand_sp);
        return;
    }
    else {
        BALL_LOG_WARN << "Ignoring result " << result << BALL_LOG_END;
    }
}

void TestClient::dispatchAcknowledgment(
    const ntcf::TestContext&                  context,
    const ntcf::TestFault&                    fault,
    const bsl::shared_ptr<ntcf::TestMessage>& message,
    const ntcf::TestAcknowledgmentCallback    callback)
{
    ntcf::TestAcknowledgmentResult result;
    result.context = context;

    if (fault.code != ntcf::TestFaultCode::e_OK) {
        BSLS_ASSERT(message.get() == 0);
        result.value.makeFailure(fault);
    }
    else if (message) {
        if (message->entity().has_value()) {
            const ntcf::TestMessageEntity& entity = message->entity().value();
            if (entity.isContentValue()) {
                const ntcf::TestContent& content = entity.content();
                if (content.isAcknowledgmentValue()) {
                    const ntcf::TestAcknowledgment& acknowledgment =
                        content.acknowledgment();
                    result.value.makeSuccess(acknowledgment);
                }
            }
            else if (entity.isControlValue()) {
                const ntcf::TestControl& control = entity.control();
                if (control.isAcknowledgmentValue()) {
                    const ntcf::TestAcknowledgment& acknowledgment =
                        control.acknowledgment();
                    result.value.makeSuccess(acknowledgment);
                }
            }
        }
    }

    if (result.value.isUndefinedValue()) {
        this->describeResultTypeFailure(&result.value.makeFailure());
    }

    if (callback) {
        callback.execute(result, d_strand_sp);
        return;
    }
    else {
        BALL_LOG_WARN << "Ignoring result " << result << BALL_LOG_END;
    }
}

const bsl::shared_ptr<ntci::Strand>& TestClient::strand() const
{
    return d_strand_sp;
}

TestClient::TestClient(const ntcf::TestClientConfig& configuration,
                       const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                       const bsl::shared_ptr<ntci::DataPool>&  dataPool,
                       const bsl::shared_ptr<ntcf::TestMessageEncryption>& encryption,
                       const ntsa::Endpoint&         tcpEndpoint,
                       const ntsa::Endpoint&         udpEndpoint,
                       bslma::Allocator*             basicAllocator)
: d_mutex()
, d_dataPool_sp(dataPool)
, d_messagePool_sp()
, d_serialization_sp()
, d_compression_sp()
, d_scheduler_sp(scheduler)
, d_datagramSocket_sp()
, d_datagramParser_sp()
, d_streamSocket_sp()
, d_streamParser_sp()
, d_encryption_sp(encryption)
, d_transactionCatalog(basicAllocator)
, d_downgradeSemaphore()
, d_tcpEndpoint(tcpEndpoint)
, d_udpEndpoint(udpEndpoint)
, d_closed(false)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    // MRM: BALL_LOG_INFO << "Client constructing starting" << BALL_LOG_END;

    ntsa::Error error;

    if (d_config.name.isNull()) {
        d_config.name = "client";
    }

    d_messagePool_sp.createInplace(d_allocator_p, d_allocator_p);

    ntca::SerializationConfig serializationConfig;

    error = ntcf::System::createSerialization(&d_serialization_sp, 
                                              serializationConfig, 
                                              d_allocator_p);
    BSLS_ASSERT_OPT(!error);

    ntca::CompressionConfig compressionConfig;

#if NTC_BUILD_WITH_ZLIB
    compressionConfig.setType(ntca::CompressionType::e_ZLIB);
#elif NTC_BUILD_WITH_LZ4
    compressionConfig.setType(ntca::CompressionType::e_LZ4);
#else
    compressionConfig.setType(ntca::CompressionType::e_RLE);
#endif
    
    compressionConfig.setGoal(ntca::CompressionGoal::e_BALANCED);

    error = ntcf::System::createCompression(&d_compression_sp,
                                            compressionConfig,
                                            d_dataPool_sp,
                                            d_allocator_p);
    BSLS_ASSERT_OPT(!error);

    d_datagramParser_sp.createInplace(d_allocator_p,
                                      d_dataPool_sp,
                                      d_messagePool_sp,
                                      d_serialization_sp,
                                      d_compression_sp,
                                      d_allocator_p);

    d_streamParser_sp.createInplace(d_allocator_p,
                                    d_dataPool_sp,
                                    d_messagePool_sp,
                                    d_serialization_sp,
                                    d_compression_sp,
                                    d_allocator_p);

    // MRM: BALL_LOG_INFO << "Client construction complete" << BALL_LOG_END;
}

TestClient::~TestClient()
{
    this->close();
}

ntsa::Error TestClient::connect()
{
    ntsa::Error error;

    bsl::shared_ptr<Self> self(this->getSelf(this));

#if NTCF_TESTCLIENT_DATAGRAM_SOCKET_ENABLED

    // MRM: BALL_LOG_INFO << "Client datagram socket construction starting"
    // MRM:               << BALL_LOG_END;

    ntca::DatagramSocketOptions datagramSocketOptions;
    datagramSocketOptions.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    datagramSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::Ipv4Endpoint(ntsa::Ipv4Address::loopback(), 0)));

    d_datagramSocket_sp =
        d_scheduler_sp->createDatagramSocket(datagramSocketOptions,
                                             d_allocator_p);

    error = d_datagramSocket_sp->registerSession(self);
    if (error) {
        return error;
    }

    error = d_datagramSocket_sp->registerManager(self);
    if (error) {
        return error;
    }

    error = d_datagramSocket_sp->open();
    if (error) {
        return error;
    }

    {
        ntci::ConnectFuture connectFuture;
        error = d_datagramSocket_sp->connect(d_udpEndpoint,
                                             ntca::ConnectOptions(),
                                             connectFuture);
        if (error) {
            return error;
        }

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        if (error) {
            return error;
        }

        NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(d_datagramSocket_sp,
                                                  "connect",
                                                  connectResult.event());

        if (connectResult.event().context().error()) {
            return connectResult.event().context().error();
        }

        NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_CONNECTED(d_datagramSocket_sp);
    }

#endif

    // MRM: BALL_LOG_INFO << "Client datagram socket construction complete"
    // MRM:               << BALL_LOG_END;

    // MRM: BALL_LOG_INFO << "Client stream socket construction starting"
    // MRM:               << BALL_LOG_END;

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    d_streamSocket_sp =
        d_scheduler_sp->createStreamSocket(streamSocketOptions, d_allocator_p);

    error = d_streamSocket_sp->registerSession(self);
    if (error) {
        return error;
    }

    error = d_streamSocket_sp->registerManager(self);
    if (error) {
        return error;
    }

    error = d_streamSocket_sp->open();
    if (error) {
        return error;
    }

    {
        ntci::ConnectFuture connectFuture;
        error = d_streamSocket_sp->connect(d_tcpEndpoint,
                                           ntca::ConnectOptions(),
                                           connectFuture);
        if (error) {
            return error;
        }

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        if (error) {
            return error;
        }

        NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(d_streamSocket_sp,
                                                "connect",
                                                connectResult.event());

        if (connectResult.event().context().error()) {
            return connectResult.event().context().error();
        }

        NTCF_TESTCLIENT_LOG_STREAM_SOCKET_CONNECTED(d_streamSocket_sp);
    }

    // MRM: BALL_LOG_INFO << "Client stream socket construction complete"
    // MRM:               << BALL_LOG_END;


#if NTCF_TESTCLIENT_DATAGRAM_SOCKET_ENABLED

    error = d_datagramSocket_sp->relaxFlowControl(
        ntca::FlowControlType::e_RECEIVE);
    if (error) {
        return error;
    }

#endif

    error = d_streamSocket_sp->setReadQueueLowWatermark(
        d_streamParser_sp->numNeeded());
    if (error) {
        return error;
    }

    error =
        d_streamSocket_sp->relaxFlowControl(ntca::FlowControlType::e_RECEIVE);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error TestClient::connect(const ntci::ConnectCallback& callback)
{
    ntsa::Error error;

    bsl::shared_ptr<Self> self(this->getSelf(this));

#if NTCF_TESTCLIENT_DATAGRAM_SOCKET_ENABLED
    
    // MRM: BALL_LOG_INFO << "Client datagram socket construction starting"
    // MRM:               << BALL_LOG_END;

    ntca::DatagramSocketOptions datagramSocketOptions;
    datagramSocketOptions.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    datagramSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::Ipv4Endpoint(ntsa::Ipv4Address::loopback(), 0)));

    d_datagramSocket_sp =
        d_scheduler_sp->createDatagramSocket(datagramSocketOptions,
                                             d_allocator_p);

    error = d_datagramSocket_sp->registerSession(self);
    if (error) {
        return error;
    }

    error = d_datagramSocket_sp->registerManager(self);
    if (error) {
        return error;
    }

    error = d_datagramSocket_sp->open();
    if (error) {
        return error;
    }

    {
        ntci::ConnectFuture connectFuture;
        error = d_datagramSocket_sp->connect(d_udpEndpoint,
                                             ntca::ConnectOptions(),
                                             connectFuture);
        if (error) {
            return error;
        }

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        if (error) {
            return error;
        }

        NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(d_datagramSocket_sp,
                                                  "connect",
                                                  connectResult.event());

        if (connectResult.event().context().error()) {
            return connectResult.event().context().error();
        }

        NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_CONNECTED(d_datagramSocket_sp);
    }

    // MRM: BALL_LOG_INFO << "Client datagram socket construction complete"
    // MRM:               << BALL_LOG_END;

#endif

    // MRM: BALL_LOG_INFO << "Client stream socket construction starting"
    // MRM:               << BALL_LOG_END;

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    d_streamSocket_sp =
        d_scheduler_sp->createStreamSocket(streamSocketOptions, d_allocator_p);

    error = d_streamSocket_sp->registerSession(self);
    if (error) {
        return error;
    }

    error = d_streamSocket_sp->registerManager(self);
    if (error) {
        return error;
    }

    error = d_streamSocket_sp->open();
    if (error) {
        return error;
    }

    ntci::ConnectCallback callbackProxy = this->createConnectCallback(
        NTCCFG_BIND(&TestClient::dispatchConnect, 
                    this,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    callback),
        d_allocator_p);

    {
        error = d_streamSocket_sp->connect(d_tcpEndpoint,
                                           ntca::ConnectOptions(),
                                           callbackProxy);
        if (error) {
            return error;
        }
    }

    // MRM: BALL_LOG_INFO << "Client stream socket construction complete"
    // MRM:               << BALL_LOG_END;

    return ntsa::Error();
}

bsl::shared_ptr<ntcf::TestMessage> TestClient::createMessage()
{
    return d_messagePool_sp->create();
}

bsl::shared_ptr<ntcf::TestMessage> TestClient::createMessage(
    const ntcf::TestOptions& options)
{
    bsl::shared_ptr<ntcf::TestMessage> message = this->createMessage();

    message->setSerializationType(ntca::SerializationType::e_BER);
    message->setCompressionType(ntca::CompressionType::e_NONE);

    return message;
}

ntsa::Error TestClient::message(
    ntcf::TestContext*                        context,
    ntcf::TestFault*                          fault,
    bsl::shared_ptr<ntcf::TestMessage>*       response,
    const bsl::shared_ptr<ntcf::TestMessage>& request,
    const ntcf::TestOptions&                  options)
{
    ntsa::Error error;

    context->reset();
    fault->reset();
    response->reset();

    ntcf::TestMessageFuture future;
    error = this->message(request, options, future);
    if (error) {
        this->describeInititationFailure(fault);
        return error;
    }

    error = future.wait(context, fault, response);
    if (error) {
        this->describeWaitFailure(fault);
        return error;
    }

    return ntsa::Error();
}

ntsa::Error TestClient::message(
    const bsl::shared_ptr<ntcf::TestMessage>& request,
    const ntcf::TestOptions&                  options,
    const ntcf::TestMessageCallback&          callback)
{
    ntsa::Error error;

    if (request->clientTimestamp() == bsls::TimeInterval()) {
        request->setClientTimestamp(d_streamSocket_sp->currentTime());
    }

    bsl::shared_ptr<ntcf::TestClientTransaction> transaction =
        d_transactionCatalog.create(request, options, callback);

    BSLS_ASSERT(request->transaction() != 0);

    bsl::shared_ptr<bdlbb::Blob> blob =
        d_streamSocket_sp->createOutgoingBlob();

    error = request->encode(blob.get(),
                            d_serialization_sp.get(),
                            d_compression_sp.get());

    if (error) {
        return error;
    }

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_OUTGOING_MESSAGE(d_streamSocket_sp,
                                                       request);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_OUTGOING_BLOB(d_streamSocket_sp,
                                                        *blob);

    ntsa::Data data(blob);

    error = d_streamSocket_sp->send(data, ntca::SendOptions());
    if (error) {
        d_transactionCatalog.remove(request);
        return error;
    }

    return ntsa::Error();
}

ntsa::Error TestClient::bid(ntcf::TestContext*       context,
                            ntcf::TestFault*         fault,
                            ntcf::TestTrade*         trade,
                            const ntcf::TestBid&     bid,
                            const ntcf::TestOptions& options)
{
    ntsa::Error error;

    context->reset();
    fault->reset();
    trade->reset();

    ntcf::TestTradeFuture future;
    error = this->bid(bid, options, future);
    if (error) {
        this->describeInititationFailure(fault);
        return error;
    }

    ntcf::TestTradeResult result;
    error = future.wait(&result);
    if (error) {
        this->describeWaitFailure(fault);
        return error;
    }

    *context = result.context;

    if (result.value.isSuccessValue()) {
        *trade = result.value.success();
        return ntsa::Error();
    }
    else if (result.value.isFailureValue()) {
        *fault = result.value.failure();
        if (result.context.error != 0) {
            return ntsa::Error(result.context.error);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    this->describeResultTypeFailure(fault);
    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error TestClient::bid(ntcf::TestTradeResult*   result,
                            const ntcf::TestBid&     bid,
                            const ntcf::TestOptions& options)
{
    ntsa::Error error;

    result->reset();

    ntcf::TestTradeFuture future;
    error = this->bid(bid, options, future);
    if (error) {
        this->describeInititationFailure(&result->value.makeFailure());
        return error;
    }

    error = future.wait(result);
    if (error) {
        this->describeWaitFailure(&result->value.makeFailure());
        return error;
    }

    if (result->value.isUndefinedValue()) {
        this->describeResultTypeFailure(&result->value.makeFailure());
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    else if (result->value.isFailureValue()) {
        if (result->context.error != 0) {
            return ntsa::Error(result->context.error);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error TestClient::bid(const ntcf::TestBid&           bid,
                            const ntcf::TestOptions&       options,
                            const ntcf::TestTradeCallback& callback)
{
    ntsa::Error error;

    bsl::shared_ptr<ntcf::TestMessage> message = this->createMessage(options);

    message->setType(ntcf::TestMessageType::e_BID);
    message->setFlag(ntcf::TestMessageFlag::e_REQUEST);

    message->makeEntity().makeContent().makeBid(bid);

    ntcf::TestMessageCallback messageCallback =
        this->createMessageCallback(NTCCFG_BIND(&TestClient::dispatchTrade,
                                                this,
                                                NTCCFG_BIND_PLACEHOLDER_1,
                                                NTCCFG_BIND_PLACEHOLDER_2,
                                                NTCCFG_BIND_PLACEHOLDER_3,
                                                callback),
                                    d_allocator_p);

    error = this->message(message, options, messageCallback);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error TestClient::ask(ntcf::TestContext*       context,
                            ntcf::TestFault*         fault,
                            ntcf::TestTrade*         trade,
                            const ntcf::TestAsk&     ask,
                            const ntcf::TestOptions& options)
{
    ntsa::Error error;

    context->reset();
    fault->reset();
    trade->reset();

    ntcf::TestTradeFuture future;
    error = this->ask(ask, options, future);
    if (error) {
        this->describeInititationFailure(fault);
        return error;
    }

    ntcf::TestTradeResult result;
    error = future.wait(&result);
    if (error) {
        this->describeWaitFailure(fault);
        return error;
    }

    *context = result.context;

    if (result.value.isSuccessValue()) {
        *trade = result.value.success();
        return ntsa::Error();
    }
    else if (result.value.isFailureValue()) {
        *fault = result.value.failure();
        if (result.context.error != 0) {
            return ntsa::Error(result.context.error);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    this->describeResultTypeFailure(fault);
    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error TestClient::ask(ntcf::TestTradeResult*   result,
                            const ntcf::TestAsk&     ask,
                            const ntcf::TestOptions& options)
{
    ntsa::Error error;

    result->reset();

    ntcf::TestTradeFuture future;
    error = this->ask(ask, options, future);
    if (error) {
        this->describeInititationFailure(&result->value.makeFailure());
        return error;
    }

    error = future.wait(result);
    if (error) {
        this->describeWaitFailure(&result->value.makeFailure());
        return error;
    }

    if (result->value.isUndefinedValue()) {
        this->describeResultTypeFailure(&result->value.makeFailure());
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    else if (result->value.isFailureValue()) {
        if (result->context.error != 0) {
            return ntsa::Error(result->context.error);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error TestClient::ask(const ntcf::TestAsk&           ask,
                            const ntcf::TestOptions&       options,
                            const ntcf::TestTradeCallback& callback)
{
    ntsa::Error error;

    bsl::shared_ptr<ntcf::TestMessage> message = this->createMessage(options);

    message->setType(ntcf::TestMessageType::e_ASK);
    message->setFlag(ntcf::TestMessageFlag::e_REQUEST);

    message->makeEntity().makeContent().makeAsk(ask);

    ntcf::TestMessageCallback messageCallback =
        this->createMessageCallback(NTCCFG_BIND(&TestClient::dispatchTrade,
                                                this,
                                                NTCCFG_BIND_PLACEHOLDER_1,
                                                NTCCFG_BIND_PLACEHOLDER_2,
                                                NTCCFG_BIND_PLACEHOLDER_3,
                                                callback),
                                    d_allocator_p);

    error = this->message(message, options, messageCallback);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error TestClient::signal(ntcf::TestContext*       context,
                               ntcf::TestFault*         fault,
                               ntcf::TestEcho*          echo,
                               const ntcf::TestSignal&  signal,
                               const ntcf::TestOptions& options)
{
    ntsa::Error error;

    context->reset();
    fault->reset();
    echo->reset();

    ntcf::TestEchoFuture future;
    error = this->signal(signal, options, future);
    if (error) {
        this->describeInititationFailure(fault);
        return error;
    }

    ntcf::TestEchoResult result;
    error = future.wait(&result);
    if (error) {
        this->describeWaitFailure(fault);
        return error;
    }

    *context = result.context;

    if (result.value.isSuccessValue()) {
        *echo = result.value.success();
        return ntsa::Error();
    }
    else if (result.value.isFailureValue()) {
        *fault = result.value.failure();
        if (result.context.error != 0) {
            return ntsa::Error(result.context.error);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    this->describeResultTypeFailure(fault);
    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error TestClient::signal(
    ntcf::TestEchoResult*    result,
    const ntcf::TestSignal&  signal,
    const ntcf::TestOptions& options)
{
    ntsa::Error error;

    result->reset();

    ntcf::TestEchoFuture future;
    error = this->signal(signal, options, future);
    if (error) {
        this->describeInititationFailure(&result->value.makeFailure());
        return error;
    }

    error = future.wait(result);
    if (error) {
        this->describeWaitFailure(&result->value.makeFailure());
        return error;
    }

    if (result->value.isUndefinedValue()) {
        this->describeResultTypeFailure(&result->value.makeFailure());
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    else if (result->value.isFailureValue()) {
        if (result->context.error != 0) {
            return ntsa::Error(result->context.error);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error TestClient::signal(const ntcf::TestSignal&       signal,
                               const ntcf::TestOptions&      options,
                               const ntcf::TestEchoCallback& callback)
{
    ntsa::Error error;

    bsl::shared_ptr<ntcf::TestMessage> message = this->createMessage(options);

    message->setType(ntcf::TestMessageType::e_SIGNAL);
    message->setFlag(ntcf::TestMessageFlag::e_REQUEST);

    message->makeEntity().makeControl().makeSignal(signal);

    ntcf::TestMessageCallback messageCallback =
        this->createMessageCallback(NTCCFG_BIND(&TestClient::dispatchEcho,
                                                this,
                                                NTCCFG_BIND_PLACEHOLDER_1,
                                                NTCCFG_BIND_PLACEHOLDER_2,
                                                NTCCFG_BIND_PLACEHOLDER_3,
                                                callback),
                                    d_allocator_p);

    error = this->message(message, options, messageCallback);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error TestClient::encrypt(ntcf::TestContext*           context,
                                ntcf::TestFault*             fault,
                                ntcf::TestAcknowledgment*    acknowledgment,
                                const TestControlEncryption& encryption,
                                const ntcf::TestOptions&     options)
{
    ntsa::Error error;

    context->reset();
    fault->reset();
    acknowledgment->reset();

    ntcf::TestAcknowledgmentFuture future;
    error = this->encrypt(encryption, options, future);
    if (error) {
        this->describeInititationFailure(fault);
        return error;
    }

    ntcf::TestAcknowledgmentResult result;
    error = future.wait(&result);
    if (error) {
        this->describeWaitFailure(fault);
        return error;
    }

    *context = result.context;

    if (result.value.isSuccessValue()) {
        *acknowledgment = result.value.success();
        return ntsa::Error();
    }
    else if (result.value.isFailureValue()) {
        *fault = result.value.failure();
        if (result.context.error != 0) {
            return ntsa::Error(result.context.error);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    this->describeResultTypeFailure(fault);
    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error TestClient::encrypt(
    ntcf::TestAcknowledgmentResult*     result,
    const ntcf::TestControlEncryption&  encryption,
    const ntcf::TestOptions&            options)
{
    ntsa::Error error;

    result->reset();

    if (!encryption.acknowledge) {
        this->describeInititationFailure(&result->value.makeFailure());
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntcf::TestAcknowledgmentFuture future;
    error = this->encrypt(encryption, options, future);
    if (error) {
        this->describeInititationFailure(&result->value.makeFailure());
        return error;
    }

    error = future.wait(result);
    if (error) {
        this->describeWaitFailure(&result->value.makeFailure());
        return error;
    }

    if (result->value.isUndefinedValue()) {
        this->describeResultTypeFailure(&result->value.makeFailure());
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    else if (result->value.isFailureValue()) {
        if (result->context.error != 0) {
            return ntsa::Error(result->context.error);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error TestClient::encrypt(
    const ntcf::TestControlEncryption&      encryption,
    const ntcf::TestOptions&                options,
    const ntcf::TestAcknowledgmentCallback& callback)
{
    ntsa::Error error;

    bsl::shared_ptr<ntcf::TestMessage> message = this->createMessage(options);

    message->setType(ntcf::TestMessageType::e_ENCRYPT);
    message->setFlag(ntcf::TestMessageFlag::e_REQUEST);
    if (!encryption.acknowledge) {
        message->setFlag(ntcf::TestMessageFlag::e_UNACKNOWLEDGED);
    }

    message->makeEntity().makeControl().makeEncryption(encryption);

    ntcf::TestMessageCallback messageCallback;

    if (encryption.acknowledge) {
        if (callback) {
            messageCallback = this->createMessageCallback(
                NTCCFG_BIND(&TestClient::dispatchAcknowledgment,
                            this,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            callback),
                d_allocator_p);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (callback) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = this->message(message, options, messageCallback);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error TestClient::encrypt(const ntcf::TestControlEncryption& encryption,
                                const ntcf::TestOptions&           options)
{
    ntsa::Error error;

    bsl::shared_ptr<ntcf::TestMessage> message = this->createMessage(options);

    message->setType(ntcf::TestMessageType::e_ENCRYPT);
    message->setFlag(ntcf::TestMessageFlag::e_REQUEST);
    if (!encryption.acknowledge) {
        message->setFlag(ntcf::TestMessageFlag::e_UNACKNOWLEDGED);
    }

    message->makeEntity().makeControl().makeEncryption(encryption);

    ntcf::TestMessageCallback messageCallback;

    if (encryption.acknowledge) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = this->message(message, options, messageCallback);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error TestClient::compress(ntcf::TestContext*            context,
                                 ntcf::TestFault*              fault,
                                 ntcf::TestAcknowledgment*     acknowledgment,
                                 const TestControlCompression& compression,
                                 const ntcf::TestOptions&      options)
{
    ntsa::Error error;

    context->reset();
    fault->reset();
    acknowledgment->reset();

    ntcf::TestAcknowledgmentFuture future;
    error = this->compress(compression, options, future);
    if (error) {
        this->describeInititationFailure(fault);
        return error;
    }

    ntcf::TestAcknowledgmentResult result;
    error = future.wait(&result);
    if (error) {
        this->describeWaitFailure(fault);
        return error;
    }

    *context = result.context;

    if (result.value.isSuccessValue()) {
        *acknowledgment = result.value.success();
        return ntsa::Error();
    }
    else if (result.value.isFailureValue()) {
        *fault = result.value.failure();
        if (result.context.error != 0) {
            return ntsa::Error(result.context.error);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    this->describeResultTypeFailure(fault);
    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error TestClient::compress(
    ntcf::TestAcknowledgmentResult*      result,
    const ntcf::TestControlCompression&  compression,
    const ntcf::TestOptions&             options)
{
    ntsa::Error error;

    result->reset();

    if (!compression.acknowledge) {
        this->describeInititationFailure(&result->value.makeFailure());
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntcf::TestAcknowledgmentFuture future;
    error = this->compress(compression, options, future);
    if (error) {
        this->describeInititationFailure(&result->value.makeFailure());
        return error;
    }

    error = future.wait(result);
    if (error) {
        this->describeWaitFailure(&result->value.makeFailure());
        return error;
    }

    if (result->value.isUndefinedValue()) {
        this->describeResultTypeFailure(&result->value.makeFailure());
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    else if (result->value.isFailureValue()) {
        NTCF_TESTCLIENT_LOG_COMPRESS_FAILURE(
            d_streamSocket_sp, result->value.failure());

        if (result->context.error != 0) {
            return ntsa::Error(result->context.error);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error TestClient::compress(
    const ntcf::TestControlCompression&     compression,
    const ntcf::TestOptions&                options,
    const ntcf::TestAcknowledgmentCallback& callback)
{
    ntsa::Error error;

    bsl::shared_ptr<ntcf::TestMessage> message = this->createMessage(options);

    message->setType(ntcf::TestMessageType::e_COMPRESS);
    message->setFlag(ntcf::TestMessageFlag::e_REQUEST);
    if (!compression.acknowledge) {
        message->setFlag(ntcf::TestMessageFlag::e_UNACKNOWLEDGED);
    }

    message->makeEntity().makeControl().makeCompression(compression);

    ntcf::TestMessageCallback messageCallback;

    if (compression.acknowledge) {
        if (callback) {
            messageCallback = this->createMessageCallback(
                NTCCFG_BIND(&TestClient::dispatchAcknowledgment,
                            this,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            callback),
                d_allocator_p);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (callback) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = this->message(message, options, messageCallback);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error TestClient::compress(
    const ntcf::TestControlCompression& compression,
    const ntcf::TestOptions&            options)
{
    ntsa::Error error;

    bsl::shared_ptr<ntcf::TestMessage> message = this->createMessage(options);

    message->setType(ntcf::TestMessageType::e_COMPRESS);
    message->setFlag(ntcf::TestMessageFlag::e_REQUEST);
    if (!compression.acknowledge) {
        message->setFlag(ntcf::TestMessageFlag::e_UNACKNOWLEDGED);
    }

    message->makeEntity().makeControl().makeCompression(compression);

    ntcf::TestMessageCallback messageCallback;

    if (compression.acknowledge) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = this->message(message, options, messageCallback);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error TestClient::heartbeat(ntcf::TestContext*          context,
                                  ntcf::TestFault*            fault,
                                  ntcf::TestAcknowledgment*   acknowledgment,
                                  const TestControlHeartbeat& heartbeat,
                                  const ntcf::TestOptions&    options)
{
    ntsa::Error error;

    context->reset();
    fault->reset();
    acknowledgment->reset();

    ntcf::TestAcknowledgmentFuture future;
    error = this->heartbeat(heartbeat, options, future);
    if (error) {
        this->describeInititationFailure(fault);
        return error;
    }

    ntcf::TestAcknowledgmentResult result;
    error = future.wait(&result);
    if (error) {
        this->describeWaitFailure(fault);
        return error;
    }

    *context = result.context;

    if (result.value.isSuccessValue()) {
        *acknowledgment = result.value.success();
        return ntsa::Error();
    }
    else if (result.value.isFailureValue()) {
        *fault = result.value.failure();
        if (result.context.error != 0) {
            return ntsa::Error(result.context.error);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    this->describeResultTypeFailure(fault);
    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error TestClient::heartbeat(ntcf::TestAcknowledgmentResult*    result,
                                  const ntcf::TestControlHeartbeat&  heartbeat,
                                  const ntcf::TestOptions&           options)
{
    ntsa::Error error;

    result->reset();

    if (!heartbeat.acknowledge) {
        this->describeInititationFailure(&result->value.makeFailure());
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntcf::TestAcknowledgmentFuture future;
    error = this->heartbeat(heartbeat, options, future);
    if (error) {
        this->describeInititationFailure(&result->value.makeFailure());
        return error;
    }

    error = future.wait(result);
    if (error) {
        this->describeWaitFailure(&result->value.makeFailure());
        return error;
    }

    if (result->value.isUndefinedValue()) {
        this->describeResultTypeFailure(&result->value.makeFailure());
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    else if (result->value.isFailureValue()) {
        if (result->context.error != 0) {
            return ntsa::Error(result->context.error);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error TestClient::heartbeat(
    const ntcf::TestControlHeartbeat&       heartbeat,
    const ntcf::TestOptions&                options,
    const ntcf::TestAcknowledgmentCallback& callback)
{
    ntsa::Error error;

    bsl::shared_ptr<ntcf::TestMessage> message = this->createMessage(options);

    message->setType(ntcf::TestMessageType::e_HEARTBEAT);
    message->setFlag(ntcf::TestMessageFlag::e_REQUEST);
    if (!heartbeat.acknowledge) {
        message->setFlag(ntcf::TestMessageFlag::e_UNACKNOWLEDGED);
    }

    message->makeEntity().makeControl().makeHeartbeat(heartbeat);

    ntcf::TestMessageCallback messageCallback;

    if (heartbeat.acknowledge) {
        if (callback) {
            messageCallback = this->createMessageCallback(
                NTCCFG_BIND(&TestClient::dispatchAcknowledgment,
                            this,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            callback),
                d_allocator_p);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (callback) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = this->message(message, options, messageCallback);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error TestClient::heartbeat(const ntcf::TestControlHeartbeat& heartbeat,
                                  const ntcf::TestOptions&          options)
{
    ntsa::Error error;

    bsl::shared_ptr<ntcf::TestMessage> message = this->createMessage(options);

    message->setType(ntcf::TestMessageType::e_HEARTBEAT);
    message->setFlag(ntcf::TestMessageFlag::e_REQUEST);
    if (!heartbeat.acknowledge) {
        message->setFlag(ntcf::TestMessageFlag::e_UNACKNOWLEDGED);
    }

    message->makeEntity().makeControl().makeHeartbeat(heartbeat);

    ntcf::TestMessageCallback messageCallback;

    if (heartbeat.acknowledge) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = this->message(message, options, messageCallback);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

void TestClient::enableCompression()
{
    if (d_streamSocket_sp) {
        d_streamSocket_sp->setWriteDeflater(d_compression_sp);
        d_streamSocket_sp->setReadInflater(d_compression_sp);
    }
    else if (d_datagramSocket_sp) {
        d_datagramSocket_sp->setWriteDeflater(d_compression_sp);
        d_datagramSocket_sp->setReadInflater(d_compression_sp);
    }
}

void TestClient::enableEncryption()
{
    ntsa::Error error;

    ntca::EncryptionClientOptions encryptionClientOptions;

    encryptionClientOptions.addAuthority(
        d_encryption_sp->authorityCertificate());

    bsl::shared_ptr<ntci::EncryptionClient> encryptionClient;
    error = ntcf::System::createEncryptionClient(&encryptionClient,
                                                 encryptionClientOptions,
                                                 d_allocator_p);
    BSLS_ASSERT_OPT(!error);

    ntca::UpgradeOptions upgradeOptions;
    ntci::UpgradeFuture  upgradeFuture;

    error = d_streamSocket_sp->upgrade(encryptionClient,
                                       upgradeOptions,
                                       upgradeFuture);
    BSLS_ASSERT_OPT(!error);

    ntci::UpgradeResult upgradeResult;
    error = upgradeFuture.wait(&upgradeResult);
    BSLS_ASSERT_OPT(!error);

    if (upgradeResult.event().type() == ntca::UpgradeEventType::e_COMPLETE) {
        BSLS_ASSERT_OPT(!upgradeResult.event().context().error());

        bsl::shared_ptr<ntci::EncryptionCertificate> remoteCertificate =
                d_streamSocket_sp->remoteCertificate();

        if (remoteCertificate) {
            ntca::EncryptionCertificate remoteCertificateRecord;
            remoteCertificate->unwrap(&remoteCertificateRecord);

            BALL_LOG_INFO << "Client stream socket at "
                            << d_streamSocket_sp->sourceEndpoint() << " to "
                            << d_streamSocket_sp->remoteEndpoint()
                            << " upgrade complete: " 
                            << upgradeResult.event().context()
                            << BALL_LOG_END;

            BALL_LOG_INFO
                << "Client stream socket at " 
                << d_streamSocket_sp->sourceEndpoint() << " to "
                << d_streamSocket_sp->remoteEndpoint()
                << " encryption session has been established with "
                << remoteCertificate->subject() << " issued by "
                << remoteCertificate->issuer() << ": "
                << remoteCertificateRecord << BALL_LOG_END;
        }
        else {
            BALL_LOG_INFO << "Client stream socket at "
                        << d_streamSocket_sp->sourceEndpoint() 
                        << " to "
                        << d_streamSocket_sp->remoteEndpoint()
                        << " encryption session has been established"
                        << BALL_LOG_END;
        }
    }
    else {
        BALL_LOG_ERROR
            << "Client stream socket at "
            << d_streamSocket_sp->sourceEndpoint() 
            << " to "
            << d_streamSocket_sp->remoteEndpoint()
            << " upgrade error: " 
            << upgradeResult.event().context() 
            << BALL_LOG_END;

        BSLS_ASSERT_OPT(!upgradeResult.event().context().error());
    }
}

void TestClient::disableCompression()
{
    bsl::shared_ptr<ntci::Compression> none;
    
    if (d_streamSocket_sp) {
        d_streamSocket_sp->setWriteDeflater(none);
        d_streamSocket_sp->setReadInflater(none);
    }
    else if (d_datagramSocket_sp) {
        d_datagramSocket_sp->setWriteDeflater(none);
        d_datagramSocket_sp->setReadInflater(none);
    }
}

void TestClient::disableEncryption()
{
    ntsa::Error error;

    if (d_streamSocket_sp) {
        error = d_streamSocket_sp->downgrade();
        BSLS_ASSERT_OPT(!error);

        d_downgradeSemaphore.wait();
    }
}

void TestClient::close()
{
    if (d_closed) {
        return;
    }

    d_closed = true;

    // MRM: BALL_LOG_INFO << "Client destruction starting" << BALL_LOG_END;

    // MRM: BALL_LOG_INFO << "Client stream socket destruction starting"
    // MRM:               << BALL_LOG_END;

    if (d_streamSocket_sp) {
        ntci::StreamSocketCloseGuard closeGuard(d_streamSocket_sp);
    }

    d_streamSocket_sp.reset();

    // MRM: BALL_LOG_INFO << "Client stream socket destruction complete"
    // MRM:               << BALL_LOG_END;

    // MRM: BALL_LOG_INFO << "Client datagram socket destruction starting"
    // MRM:               << BALL_LOG_END;

    if (d_datagramSocket_sp) {
        ntci::DatagramSocketCloseGuard closeGuard(d_datagramSocket_sp);
    }

    d_datagramSocket_sp.reset();

    // MRM: BALL_LOG_INFO << "Client datagram socket destruction complete"
    // MRM:               << BALL_LOG_END;

    // MRM: BALL_LOG_INFO << "Client destruction complete" << BALL_LOG_END;
}

}  // end namespace ntcf
}  // end namespace BloombergLP
#endif
