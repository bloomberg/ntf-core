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

#ifndef INCLUDED_NTCF_TESTSERVER_CPP
#define INCLUDED_NTCF_TESTSERVER_CPP

#include <ntcf_testserver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcf_testserver_cpp, "$Id$ $CSID$")

#define NTCF_TESTCLIENT_DATAGRAM_SOCKET_ENABLED 0

namespace BloombergLP {
namespace ntcf {

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_ESTABLISHED(datagramSocket)       \
    do {                                                                      \
        BALL_LOG_INFO << "Server datagram socket at "                         \
                      << (datagramSocket)->sourceEndpoint() << " established" \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_CLOSED(datagramSocket)            \
    do {                                                                      \
        BALL_LOG_INFO << "Server datagram socket at "                         \
                      << (datagramSocket)->sourceEndpoint() << " closed"      \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,             \
                                                  type,                       \
                                                  event)                      \
    do {                                                                      \
        BALL_LOG_TRACE << "Server datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint()                  \
                       << " processing " << (type) << " event " << (event)    \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RECEIVE_WOULD_BLOCK(              \
    datagramSocket)                                                           \
    do {                                                                      \
        BALL_LOG_TRACE << "Server datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint()                  \
                       << " receive WOULD_BLOCK" << BALL_LOG_END;             \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RECEIVE_EOF(datagramSocket)       \
    do {                                                                      \
        BALL_LOG_TRACE << "Server datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint()                  \
                       << " receive EOF" << BALL_LOG_END;                     \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RECEIVE_FAILED(datagramSocket,    \
                                                           error)             \
    do {                                                                      \
        BALL_LOG_TRACE << "Server datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint()                  \
                       << " receive failed: " << (error) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RECEIVE_CONTEXT(datagramSocket,   \
                                                            blob,             \
                                                            receiveContext)   \
    do {                                                                      \
        BALL_LOG_TRACE << "Server datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint() << " received "  \
                       << (blob).length() << " bytes from context "           \
                       << (receiveContext) << BALL_LOG_END;                   \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_PARSE_FAILED(datagramSocket,      \
                                                         error)               \
    do {                                                                      \
        BALL_LOG_ERROR << "Server datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint()                  \
                       << " parsing failed: " << (error) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RESPONSE_IGNORED(datagramSocket,  \
                                                             response)        \
    do {                                                                      \
        BALL_LOG_WARN << "Server datagram socket at "                         \
                      << (datagramSocket)->sourceEndpoint()                   \
                      << " ignoring response " << (*response)                 \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RESPONSE_UNSOLICITED(             \
    datagramSocket,                                                           \
    response)                                                                 \
    do {                                                                      \
        BALL_LOG_WARN << "Server datagram socket at "                         \
                      << (datagramSocket)->sourceEndpoint()                   \
                      << " received stale or unsolicited response "           \
                      << (*response) << BALL_LOG_END;                         \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_UNEXPECTED_MESSAGE(streamSocket,  \
                                                               message)       \
    do {                                                                      \
        BALL_LOG_WARN << "Server datagram socket at "                         \
                      << (streamSocket)->sourceEndpoint() << " to "           \
                      << (streamSocket)->remoteEndpoint()                     \
                      << " ignoring unexpected message " << (*message)        \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_UNEXPECTED_MESSAGE(streamSocket,    \
                                                             message)         \
    do {                                                                      \
        BALL_LOG_WARN << "Client stream socket at "                           \
                      << (streamSocket)->sourceEndpoint() << " to "           \
                      << (streamSocket)->remoteEndpoint()                     \
                      << " ignoring unexpected message " << (*message)        \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_INCOMING_MESSAGE(datagramSocket,  \
                                                             message,         \
                                                             endpoint)        \
    do {                                                                      \
        BALL_LOG_DEBUG << "Server datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (endpoint) << " received message " << (*message)    \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_OUTGOING_MESSAGE(datagramSocket,  \
                                                             message,         \
                                                             endpoint)        \
    do {                                                                      \
        BALL_LOG_DEBUG << "Server datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (endpoint) << " sending message " << (*message)     \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_OUTGOING_BLOB(datagramSocket,     \
                                                          blob,               \
                                                          endpoint)           \
    do {                                                                      \
        BALL_LOG_TRACE << "Server datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (endpoint) << " sending:\n"                         \
                       << bdlbb::BlobUtilHexDumper(&(blob)) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_INCOMING_BLOB(datagramSocket,     \
                                                          blob,               \
                                                          endpoint)           \
    do {                                                                      \
        BALL_LOG_TRACE << "Server datagram socket at "                        \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (endpoint) << " received:\n"                        \
                       << bdlbb::BlobUtilHexDumper(&(blob)) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_ACCEPTED(streamSocket)              \
    do {                                                                      \
        BALL_LOG_INFO << "Server stream socket at "                           \
                      << (streamSocket)->sourceEndpoint() << " to "           \
                      << (streamSocket)->remoteEndpoint() << " accepted"      \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_CLOSED(streamSocket)                \
    do {                                                                      \
        BALL_LOG_INFO << "Server stream socket at "                           \
                      << (streamSocket)->sourceEndpoint() << " to "           \
                      << (streamSocket)->remoteEndpoint() << " closed"        \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, type, event)    \
    do {                                                                      \
        BALL_LOG_TRACE << "Server stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " processing "  \
                       << (type) << " event " << (event) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_RECEIVE_WOULD_BLOCK(streamSocket)   \
    do {                                                                      \
        BALL_LOG_TRACE << "Server stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " receive WOULD_BLOCK" << BALL_LOG_END;             \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_RECEIVE_EOF(streamSocket)           \
    do {                                                                      \
        BALL_LOG_TRACE << "Server stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " receive EOF"  \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_RECEIVE_FAILED(streamSocket, error) \
    do {                                                                      \
        BALL_LOG_TRACE << "Server stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " receive failed: " << (error) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_RECEIVE_CONTEXT(streamSocket,       \
                                                          blob,               \
                                                          receiveContext)     \
    do {                                                                      \
        BALL_LOG_TRACE << "Server stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " received "    \
                       << (blob).length() << " bytes from context "           \
                       << (receiveContext) << BALL_LOG_END;                   \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_PARSE_FAILED(streamSocket, error)   \
    do {                                                                      \
        BALL_LOG_ERROR << "Server stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " parsing failed: " << (error) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_RESPONSE_IGNORED(streamSocket,      \
                                                           response)          \
    do {                                                                      \
        BALL_LOG_WARN << "Server stream socket at "                           \
                      << (streamSocket)->sourceEndpoint() << " to "           \
                      << (streamSocket)->remoteEndpoint()                     \
                      << " ignoring response " << (*response)                 \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_RESPONSE_UNSOLICITED(streamSocket,  \
                                                               response)      \
    do {                                                                      \
        BALL_LOG_WARN << "Server stream socket at "                           \
                      << (streamSocket)->sourceEndpoint() << " to "           \
                      << (streamSocket)->remoteEndpoint()                     \
                      << " received stale or unsolicited response "           \
                      << (*response) << BALL_LOG_END;                         \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_INCOMING_MESSAGE(streamSocket,      \
                                                           message)           \
    do {                                                                      \
        BALL_LOG_DEBUG << "Server stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " received message " << (*message) << BALL_LOG_END; \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_OUTGOING_MESSAGE(streamSocket,      \
                                                           message)           \
    do {                                                                      \
        BALL_LOG_DEBUG << "Server stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " sending message " << (*message) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_OUTGOING_BLOB(streamSocket, blob)   \
    do {                                                                      \
        BALL_LOG_TRACE << "Server stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " sending:\n"   \
                       << bdlbb::BlobUtilHexDumper(&(blob)) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_INCOMING_BLOB(streamSocket, blob)   \
    do {                                                                      \
        BALL_LOG_TRACE << "Server stream socket at "                          \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " received:\n"  \
                       << bdlbb::BlobUtilHexDumper(&(blob)) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTSERVER_LOG_LISTENER_SOCKET_ESTABLISHED(listenerSocket)       \
    do {                                                                      \
        BALL_LOG_DEBUG << "Server listener socket at "                        \
                       << (listenerSocket)->sourceEndpoint()                  \
                       << " established" << BALL_LOG_END;                     \
    } while (false)

#define NTCF_TESTSERVER_LOG_LISTENER_SOCKET_CLOSED(listenerSocket)            \
    do {                                                                      \
        BALL_LOG_DEBUG << "Server listener socket at "                        \
                       << (listenerSocket)->sourceEndpoint() << " closed"     \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTCF_TESTSERVER_LOG_LISTENER_SOCKET_EVENT(listenerSocket,             \
                                                  type,                       \
                                                  event)                      \
    do {                                                                      \
        BALL_LOG_TRACE << "Server listener socket at "                        \
                       << (listenerSocket)->sourceEndpoint()                  \
                       << " processing " << (type) << " event " << (event)    \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTCF_TESTSERVER_LOG_LISTENER_SOCKET_ACCEPT_WOULD_BLOCK(               \
    listenerSocket)                                                           \
    do {                                                                      \
        BALL_LOG_TRACE << "Server listener socket at "                        \
                       << (listenerSocket)->sourceEndpoint()                  \
                       << " accept WOULD_BLOCK" << BALL_LOG_END;              \
    } while (false)

#define NTCF_TESTSERVER_LOG_LISTENER_SOCKET_ACCEPT_EOF(listenerSocket)        \
    do {                                                                      \
        BALL_LOG_TRACE << "Server listener socket at "                        \
                       << (listenerSocket)->sourceEndpoint() << " accept EOF" \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTCF_TESTSERVER_LOG_LISTENER_SOCKET_ACCEPT_FAILED(listenerSocket,     \
                                                          error)              \
    do {                                                                      \
        BALL_LOG_TRACE << "Server listener socket at "                        \
                       << (listenerSocket)->sourceEndpoint()                  \
                       << " accept failed: " << (error) << BALL_LOG_END;      \
    } while (false)

#define NTCF_TESTSERVER_LOG_PROCESSOR_NOT_FOUND(message, sender, endpoint)    \
    do {                                                                      \
        BALL_LOG_TRACE << "Server ignoring unrecognized message "             \
                       << (*message) << " from " << (endpoint)                \
                       << BALL_LOG_END;                                       \
    } while (false)

bsl::shared_ptr<ntcf::TestMessage> TestServerTransaction::createResponse()
{
    bsl::shared_ptr<ntcf::TestMessage> response = d_responsePool_sp->create();

    response->setTransaction(d_request_sp->transaction());

    response->setSerializationType(d_request_sp->serializationType());
    response->setCompressionType(d_request_sp->compressionType());

    response->setClientTimestamp(d_request_sp->clientTimestamp());
    response->setServerTimestamp(d_timestamp);

    response->setDeadline(d_request_sp->deadline());

    return response;
}

void TestServerTransaction::deliverResponse(
    const bsl::shared_ptr<ntcf::TestMessage>& response)
{
    ntsa::Error error;

    ntca::SendToken sendToken;
    sendToken.setValue(d_request_sp->transaction());

    ntca::SendOptions sendOptions;
    sendOptions.setToken(sendToken);
    sendOptions.setDeadline(d_request_sp->deadline());

    if (response->type() == ntcf::TestMessageType::e_FAULT) {
        sendOptions.setHighWatermark(bsl::numeric_limits<bsl::size_t>::max());
    }

    if (d_endpoint.has_value()) {
        sendOptions.setEndpoint(d_endpoint.value());
    }

    bsl::shared_ptr<bdlbb::Blob> blob = d_dataPool_sp->createOutgoingBlob();

    error = response->encode(blob.get(),
                             d_serialization_sp.get(),
                             d_compression_sp.get());
    if (error) {
        BALL_LOG_WARN << "Failed to encode response " << *response << ": "
                      << error << BALL_LOG_END;
        return;
    }

    if (d_streamSocket_sp) {
        NTCF_TESTSERVER_LOG_STREAM_SOCKET_OUTGOING_MESSAGE(d_streamSocket_sp,
                                                           response);

        NTCF_TESTSERVER_LOG_STREAM_SOCKET_OUTGOING_BLOB(d_streamSocket_sp,
                                                        *blob);

        ntsa::Data data(blob);

        error = d_streamSocket_sp->send(data, sendOptions);
        if (error) {
            BALL_LOG_WARN << "Failed to deliver response " << *response << ": "
                          << error << BALL_LOG_END;
            return;
        }
    }
    else if (d_datagramSocket_sp) {
        NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_OUTGOING_MESSAGE(
            d_datagramSocket_sp,
            response,
            d_endpoint.value());

        NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_OUTGOING_BLOB(d_datagramSocket_sp,
                                                          *blob,
                                                          d_endpoint.value());

        ntsa::Data data(blob);

        error = d_datagramSocket_sp->send(data, sendOptions);
        if (error) {
            BALL_LOG_WARN << "Failed to deliver response " << *response << ": "
                          << error << BALL_LOG_END;
            return;
        }
    }
}

void TestServerTransaction::processUpgrade(
    const bsl::shared_ptr<ntci::Upgradable>&   upgradable,
    const ntca::UpgradeEvent&                  event,
    bool                                       acknowledge,
    ntcf::TestControlTransition::Value         transition)
{
    BSLS_ASSERT_OPT(d_streamSocket_sp);
    BSLS_ASSERT_OPT(d_streamSocket_sp == upgradable);

    if (event.type() == ntca::UpgradeEventType::e_COMPLETE) {
        bsl::shared_ptr<ntci::EncryptionCertificate> remoteCertificate =
            upgradable->remoteCertificate();

        if (remoteCertificate) {
            ntca::EncryptionCertificate remoteCertificateRecord;
            remoteCertificate->unwrap(&remoteCertificateRecord);

            BALL_LOG_INFO << "Server stream socket at "
                          << d_streamSocket_sp->sourceEndpoint() << " to "
                          << d_streamSocket_sp->remoteEndpoint()
                          << " upgrade complete: " << event.context()
                          << BALL_LOG_END;

            BALL_LOG_INFO
                << "Server stream socket at " 
                << d_streamSocket_sp->sourceEndpoint() << " to "
                << d_streamSocket_sp->remoteEndpoint()
                << " encryption session has been established with "
                << remoteCertificate->subject() << " issued by "
                << remoteCertificate->issuer() << ": "
                << remoteCertificateRecord << BALL_LOG_END;
        }
        else {
            BALL_LOG_INFO << "Server stream socket at "
                          << d_streamSocket_sp->sourceEndpoint() 
                          << " to "
                          << d_streamSocket_sp->remoteEndpoint()
                          << " encryption session has been established"
                          << BALL_LOG_END;
        }

        if (acknowledge && 
            transition == 
            ntcf::TestControlTransition::e_ACKNOWLEDGE_AFTER) 
        {
            this->acknowledge();
        }
    }
    else if (event.type() == ntca::UpgradeEventType::e_ERROR) {
        BALL_LOG_INFO
            << "Stream socket at "
            << d_streamSocket_sp->sourceEndpoint() << " to "
            << d_streamSocket_sp->remoteEndpoint()
            << " upgrade error: " << event.context() << BALL_LOG_END;

        d_streamSocket_sp->close();
    }
    else {
        NTCCFG_ABORT();
    }
}

TestServerTransaction::TestServerTransaction(
    const bsl::shared_ptr<ntcf::TestMessagePool>& responsePool,
    const bsl::shared_ptr<ntci::DataPool>&        dataPool,
    const bsl::shared_ptr<ntci::Serialization>&   serialization,
    const bsl::shared_ptr<ntci::Compression>&     compression,
    const bsl::shared_ptr<ntcf::TestMessageEncryption>& encryption,
    bslma::Allocator*                             basicAllocator)
: d_mutex()
, d_request_sp()
, d_responsePool_sp(responsePool)
, d_dataPool_sp(dataPool)
, d_datagramSocket_sp()
, d_streamSocket_sp()
, d_serialization_sp(serialization)
, d_compression_sp(compression)
, d_encryption_sp(encryption)
, d_endpoint()
, d_timestamp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

void TestServerTransaction::reset()
{
    d_request_sp.reset();
    d_datagramSocket_sp.reset();
    d_streamSocket_sp.reset();
    d_endpoint.reset();
    d_timestamp = bsls::TimeInterval();
}

void TestServerTransaction::start(
    const bsl::shared_ptr<ntcf::TestMessage>&  request,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsls::TimeInterval&                  timestamp)
{
    d_request_sp      = request;
    d_streamSocket_sp = streamSocket;
    d_timestamp       = timestamp;
}

void TestServerTransaction::start(
    const bsl::shared_ptr<ntcf::TestMessage>&    request,
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const bsls::TimeInterval&                    timestamp,
    const ntsa::Endpoint&                        endpoint)
{
    d_request_sp        = request;
    d_datagramSocket_sp = datagramSocket;
    d_endpoint          = endpoint;
    d_timestamp         = timestamp;
}

void TestServerTransaction::acknowledge()
{
    ntcf::TestAcknowledgment acknowledgment;
    this->acknowledge(acknowledgment);
}

void TestServerTransaction::acknowledge(
    const ntcf::TestAcknowledgment& acknowledgment)
{
    bsl::shared_ptr<ntcf::TestMessage> response = this->createResponse();

    response->setType(ntcf::TestMessageType::e_ACKNOWLEDGMENT);
    response->setFlag(ntcf::TestMessageFlag::e_RESPONSE);

    if (d_request_sp->entity().value().isContentValue()) {
        response->makeEntity().makeContent().makeAcknowledgment(
            acknowledgment);
    }
    else if (d_request_sp->entity().value().isControlValue()) {
        response->makeEntity().makeControl().makeAcknowledgment(
            acknowledgment);
    }

    this->deliverResponse(response);
}

void TestServerTransaction::complete(const ntcf::TestTrade& trade)
{
    bsl::shared_ptr<ntcf::TestMessage> response = this->createResponse();

    response->setType(ntcf::TestMessageType::e_TRADE);
    response->setFlag(ntcf::TestMessageFlag::e_RESPONSE);

    response->makeEntity().makeContent().makeTrade(trade);

    this->deliverResponse(response);
}

void TestServerTransaction::complete(const ntcf::TestEcho& echo)
{
    bsl::shared_ptr<ntcf::TestMessage> response = this->createResponse();

    response->setType(ntcf::TestMessageType::e_ECHO);
    response->setFlag(ntcf::TestMessageFlag::e_RESPONSE);

    response->makeEntity().makeControl().makeEcho(echo);

    this->deliverResponse(response);
}



void TestServerTransaction::fail(const ntcf::TestFault& fault)
{
    bsl::shared_ptr<ntcf::TestMessage> response = this->createResponse();

    response->setType(ntcf::TestMessageType::e_FAULT);
    response->setFlag(ntcf::TestMessageFlag::e_RESPONSE);

    if (d_request_sp->entity().value().isContentValue()) {
        response->makeEntity().makeContent().makeFault(fault);
    }
    else if (d_request_sp->entity().value().isControlValue()) {
        response->makeEntity().makeControl().makeFault(fault);
    }

    this->deliverResponse(response);
}

void TestServerTransaction::enableCompression(
    bool                               acknowledge,
    ntcf::TestControlTransition::Value transition)
{
    if (acknowledge && 
        transition == 
        ntcf::TestControlTransition::e_ACKNOWLEDGE_BEFORE) 
    {
        this->acknowledge();
    }

    if (d_streamSocket_sp) {
        d_streamSocket_sp->setWriteDeflater(d_compression_sp);
        d_streamSocket_sp->setReadInflater(d_compression_sp);
    }
    else if (d_datagramSocket_sp) {
        d_datagramSocket_sp->setWriteDeflater(d_compression_sp);
        d_datagramSocket_sp->setReadInflater(d_compression_sp);
    }

    if (acknowledge && 
        transition == 
        ntcf::TestControlTransition::e_ACKNOWLEDGE_AFTER) 
    {
        this->acknowledge();
    }
}

void TestServerTransaction::enableEncryption(
    bool                               acknowledge,
    ntcf::TestControlTransition::Value transition)
{
    ntsa::Error error;

    if (d_streamSocket_sp) {
        if (acknowledge && 
            transition == 
            ntcf::TestControlTransition::e_ACKNOWLEDGE_BEFORE) 
        {
            this->acknowledge();
        }

        ntca::EncryptionServerOptions encryptionServerOptions;

        encryptionServerOptions.setIdentity(
            d_encryption_sp->serverCertificate());
        encryptionServerOptions.setPrivateKey(
            d_encryption_sp->serverPrivateKey());

        bsl::shared_ptr<ntci::EncryptionServer> encryptionServer;
        error = ntcf::System::createEncryptionServer(&encryptionServer,
                                                     encryptionServerOptions,
                                                     d_allocator_p);
        BSLS_ASSERT_OPT(!error);

        ntca::UpgradeOptions serverUpgradeOptions;

        ntci::UpgradeCallback serverUpgradeCallback =
            d_streamSocket_sp->createUpgradeCallback(
                NTCCFG_BIND(&TestServerTransaction::processUpgrade,
                            this->getSelf(this),
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            acknowledge,
                            transition));

        error = d_streamSocket_sp->upgrade(encryptionServer,
                                           serverUpgradeOptions,
                                           serverUpgradeCallback);
        BSLS_ASSERT_OPT(!error);
    }
}

void TestServerTransaction::disableCompression(
    bool                               acknowledge,
    ntcf::TestControlTransition::Value transition)
{
    bsl::shared_ptr<ntci::Compression> none;

    // MRM
#if 0
    if (d_streamSocket_sp) {
        d_streamSocket_sp->applyFlowControl(
            ntca::FlowControlType::e_RECEIVE, 
            ntca::FlowControlMode::e_IMMEDIATE);
    }
    else if (d_datagramSocket_sp) {
        d_datagramSocket_sp->applyFlowControl(
            ntca::FlowControlType::e_RECEIVE, 
            ntca::FlowControlMode::e_IMMEDIATE);
    }
#endif

    if (acknowledge && 
        transition == 
        ntcf::TestControlTransition::e_ACKNOWLEDGE_BEFORE) 
    {
        this->acknowledge();
    }

    if (d_streamSocket_sp) {
        d_streamSocket_sp->setWriteDeflater(none);
        d_streamSocket_sp->setReadInflater(none);
    }
    else if (d_datagramSocket_sp) {
        d_datagramSocket_sp->setWriteDeflater(none);
        d_datagramSocket_sp->setReadInflater(none);
    }

    if (acknowledge && 
        transition == 
        ntcf::TestControlTransition::e_ACKNOWLEDGE_AFTER) 
    {
        this->acknowledge();
    }

    // MRM
#if 0
    if (d_streamSocket_sp) {
        d_streamSocket_sp->relaxFlowControl(
            ntca::FlowControlType::e_RECEIVE);
    }
    else if (d_datagramSocket_sp) {
        d_datagramSocket_sp->relaxFlowControl(
            ntca::FlowControlType::e_RECEIVE);
    }
#endif
}

void TestServerTransaction::disableEncryption(
    bool                               acknowledge,
    ntcf::TestControlTransition::Value transition)
{
    ntsa::Error error;

    if (acknowledge && 
        transition == 
        ntcf::TestControlTransition::e_ACKNOWLEDGE_BEFORE) 
    {
        this->acknowledge();
    }

    if (d_streamSocket_sp) {
        error = d_streamSocket_sp->downgrade();
        BSLS_ASSERT_OPT(!error);
    }

    if (acknowledge && 
        transition == 
        ntcf::TestControlTransition::e_ACKNOWLEDGE_AFTER) 
    {
        this->acknowledge();
    }
}

void TestServerTransaction::close()
{
    if (d_streamSocket_sp) {
        d_streamSocket_sp->close();
    }
}

void TestServer::processDatagramSocketEstablished(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
{
    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_ESTABLISHED(datagramSocket);

    bsl::shared_ptr<Self> self(this->getSelf(this));
    LockGuard             lock(&d_mutex);

    this->privateDatagramSocketUp(self, datagramSocket);
}

void TestServer::processDatagramSocketClosed(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
{
    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_CLOSED(datagramSocket);

    bsl::shared_ptr<Self> self(this->getSelf(this));
    LockGuard             lock(&d_mutex);

    this->privateDatagramSocketDown(self, datagramSocket);
}

void TestServer::processStreamSocketEstablished(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
}

void TestServer::processStreamSocketClosed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCCFG_WARNING_UNUSED(streamSocket);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_CLOSED(streamSocket);

    bsl::shared_ptr<Self> self(this->getSelf(this));
    LockGuard             lock(&d_mutex);

    this->privateStreamSocketDown(self, streamSocket);
}

void TestServer::processListenerSocketEstablished(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);

    NTCF_TESTSERVER_LOG_LISTENER_SOCKET_ESTABLISHED(listenerSocket);

    bsl::shared_ptr<Self> self(this->getSelf(this));
    LockGuard             lock(&d_mutex);

    this->privateListenerSocketUp(self, listenerSocket);
}

void TestServer::processListenerSocketClosed(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);

    NTCF_TESTSERVER_LOG_LISTENER_SOCKET_CLOSED(listenerSocket);

    bsl::shared_ptr<Self> self(this->getSelf(this));
    LockGuard             lock(&d_mutex);

    this->privateListenerSocketDown(self, listenerSocket);
}

void TestServer::processReadQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "read queue",
                                              event);
}

void TestServer::processReadQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "read queue",
                                              event);
}

void TestServer::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "read queue",
                                              event);

    bsl::shared_ptr<Self> self(this->getSelf(this));

    ntsa::Error error;

    while (true) {
        ntca::ReceiveContext receiveContext;
        ntca::ReceiveOptions receiveOptions;
        bdlbb::Blob          blob;

        receiveOptions.setMinSize(1);
        receiveOptions.setMaxSize(65507);

        error = d_datagramSocket_sp->receive(&receiveContext,
                                             &blob,
                                             receiveOptions);
        if (error) {
            if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK)) {
                NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RECEIVE_WOULD_BLOCK(
                    d_datagramSocket_sp);
            }
            else if (error == ntsa::Error(ntsa::Error::e_EOF)) {
                NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RECEIVE_EOF(
                    d_datagramSocket_sp);
            }
            else {
                NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RECEIVE_FAILED(
                    d_datagramSocket_sp,
                    error);
            }

            d_datagramSocket_sp->close();
            return;
        }

        NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RECEIVE_CONTEXT(
            d_datagramSocket_sp,
            blob,
            receiveContext);

        NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_INCOMING_BLOB(
            d_datagramSocket_sp,
            blob,
            receiveContext.endpoint());

        error = d_datagramParser_sp->add(blob);
        if (error) {
            NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_PARSE_FAILED(
                d_datagramSocket_sp,
                error);
            return;
        }

        if (!receiveContext.endpoint().has_value()) {
            NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_PARSE_FAILED(
                d_datagramSocket_sp,
                error);
            return;
        }

        if (!d_datagramParser_sp->hasAnyAvailable()) {
            NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_PARSE_FAILED(
                d_datagramSocket_sp,
                error);
            return;
        }

        bsl::shared_ptr<ntcf::TestMessage> message;
        error = d_datagramParser_sp->dequeue(&message);
        if (error) {
            NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_PARSE_FAILED(
                d_datagramSocket_sp,
                error);
            return;
        }

        NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_INCOMING_MESSAGE(
            d_datagramSocket_sp,
            message,
            receiveContext.endpoint());

        if (message->isRequest() || message->isSubscription()) {
            bsl::shared_ptr<ntcf::TestServerTransaction> transaction;
            transaction.createInplace(d_allocator_p,
                                      d_messagePool_sp,
                                      d_dataPool_sp,
                                      d_serialization_sp,
                                      d_compression_sp,
                                      d_encryption_sp,
                                      d_allocator_p);

            transaction->start(message,
                               d_datagramSocket_sp,
                               d_datagramSocket_sp->currentTime(),
                               receiveContext.endpoint().value());

            this->dispatchMessage(transaction, message);
        }
        else {
            NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_UNEXPECTED_MESSAGE(
                d_datagramSocket_sp,
                message);
        }
    }
}

void TestServer::processReadQueueHighWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "read queue",
                                              event);
}

void TestServer::processReadQueueDiscarded(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "read queue",
                                              event);
}

void TestServer::processWriteQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "write queue",
                                              event);
}

void TestServer::processWriteQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "write queue",
                                              event);
}

void TestServer::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "write queue",
                                              event);
}

void TestServer::processWriteQueueHighWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "write queue",
                                              event);
}

void TestServer::processWriteQueueDiscarded(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "write queue",
                                              event);
}

void TestServer::processShutdownInitiated(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "shutdown",
                                              event);
}

void TestServer::processShutdownReceive(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "shutdown",
                                              event);
}

void TestServer::processShutdownSend(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "shutdown",
                                              event);
}

void TestServer::processShutdownComplete(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket,
                                              "shutdown",
                                              event);
}

void TestServer::processError(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ErrorEvent&                      event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "error", event);
}

void TestServer::processReadQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "read queue", event);
}

void TestServer::processReadQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "read queue", event);
}

void TestServer::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "read queue", event);

    bsl::shared_ptr<Self> self(this->getSelf(this));

    ntsa::Error error;

    ntca::ReceiveContext receiveContext;
    ntca::ReceiveOptions receiveOptions;
    bdlbb::Blob          blob;

    bsl::shared_ptr<ntcf::TestMessageParser> streamParser;
    {
        LockGuard                 lock(&d_mutex);
        StreamSocketMap::iterator it = d_streamSocketMap.find(streamSocket);
        if (it == d_streamSocketMap.end()) {
            return;
        }

        streamParser = it->second;
    }

    receiveOptions.setMinSize(1);
    receiveOptions.setMaxSize(bsl::numeric_limits<bsl::size_t>::max());

    error = streamSocket->receive(&receiveContext, &blob, receiveOptions);
    if (error) {
        if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK)) {
            NTCF_TESTSERVER_LOG_STREAM_SOCKET_RECEIVE_WOULD_BLOCK(
                streamSocket);
        }
        else if (error == ntsa::Error(ntsa::Error::e_EOF)) {
            NTCF_TESTSERVER_LOG_STREAM_SOCKET_RECEIVE_EOF(streamSocket);
        }
        else {
            NTCF_TESTSERVER_LOG_STREAM_SOCKET_RECEIVE_FAILED(streamSocket,
                                                             error);
        }

        streamSocket->close();
        return;
    }

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_RECEIVE_CONTEXT(streamSocket,
                                                      blob,
                                                      receiveContext);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_INCOMING_BLOB(streamSocket, blob);

    error = streamParser->add(blob);
    if (error) {
        NTCF_TESTSERVER_LOG_STREAM_SOCKET_PARSE_FAILED(streamSocket, error);
        streamSocket->close();
        return;
    }

    while (streamParser->hasAnyAvailable()) {
        bsl::shared_ptr<ntcf::TestMessage> message;
        error = streamParser->dequeue(&message);
        if (error) {
            NTCF_TESTSERVER_LOG_STREAM_SOCKET_PARSE_FAILED(streamSocket,
                                                           error);
            streamParser->close();
            return;
        }

        NTCF_TESTSERVER_LOG_STREAM_SOCKET_INCOMING_MESSAGE(streamSocket,
                                                           message);

        if (message->isRequest() || message->isSubscription()) {
            bsl::shared_ptr<ntcf::TestServerTransaction> transaction;
            transaction.createInplace(d_allocator_p,
                                      d_messagePool_sp,
                                      d_dataPool_sp,
                                      d_serialization_sp,
                                      d_compression_sp,
                                      d_encryption_sp,
                                      d_allocator_p);

            transaction->start(message,
                               streamSocket,
                               streamSocket->currentTime());

            this->dispatchMessage(transaction, message);
        }
        else {
            NTCF_TESTSERVER_LOG_STREAM_SOCKET_UNEXPECTED_MESSAGE(streamSocket,
                                                                 message);
        }
    }
}

void TestServer::processReadQueueHighWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "read queue", event);
}

void TestServer::processReadQueueDiscarded(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "read queue", event);
}

void TestServer::processWriteQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket,
                                            "write queue",
                                            event);
}

void TestServer::processWriteQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket,
                                            "write queue",
                                            event);
}

void TestServer::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket,
                                            "write queue",
                                            event);
}

void TestServer::processWriteQueueHighWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket,
                                            "write queue",
                                            event);
}

void TestServer::processWriteQueueDiscarded(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket,
                                            "write queue",
                                            event);
}

void TestServer::processDowngradeInitiated(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::DowngradeEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "downgrade", event);
}

void TestServer::processDowngradeComplete(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::DowngradeEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "downgrade", event);

    bsl::shared_ptr<Self> self(this->getSelf(this));
    LockGuard             lock(&d_mutex);

    this->privateStreamSocketCompleteDowngrade(self, streamSocket, event);
}

void TestServer::processShutdownInitiated(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "shutdown", event);
}

void TestServer::processShutdownReceive(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "shutdown", event);
}

void TestServer::processShutdownSend(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "shutdown", event);
}

void TestServer::processShutdownComplete(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "shutdown", event);
}

void TestServer::processError(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ErrorEvent&                    event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "error", event);
}

void TestServer::processAcceptQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_LISTENER_SOCKET_EVENT(listenerSocket,
                                              "accept queue",
                                              event);
}

void TestServer::processAcceptQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_LISTENER_SOCKET_EVENT(listenerSocket,
                                              "accept queue",
                                              event);
}

void TestServer::processAcceptQueueLowWatermark(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_LISTENER_SOCKET_EVENT(listenerSocket,
                                              "accept queue",
                                              event);

    bsl::shared_ptr<Self> self(this->getSelf(this));
    LockGuard             lock(&d_mutex);

    ntsa::Error error;

    if (d_closed) {
        return;
    }

    const bsl::size_t numAvailable = event.context().size();

    for (bsl::size_t i = 0; i < numAvailable; ++i) {
        ntca::AcceptContext                 acceptContext;
        ntca::AcceptOptions                 acceptOptions;
        bsl::shared_ptr<ntci::StreamSocket> streamSocket;

        error = listenerSocket->accept(&acceptContext,
                                       &streamSocket,
                                       acceptOptions);
        if (error) {
            if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK)) {
                NTCF_TESTSERVER_LOG_LISTENER_SOCKET_ACCEPT_WOULD_BLOCK(
                    listenerSocket);
                return;
            }
            else if (error == ntsa::Error(ntsa::Error::e_EOF)) {
                NTCF_TESTSERVER_LOG_LISTENER_SOCKET_ACCEPT_EOF(listenerSocket);
            }
            else {
                NTCF_TESTSERVER_LOG_LISTENER_SOCKET_ACCEPT_FAILED(
                    listenerSocket,
                    error);
            }

            listenerSocket->close();
            return;
        }

        NTCF_TESTSERVER_LOG_STREAM_SOCKET_ACCEPTED(streamSocket);

        if (d_config.encryption.value_or(false)) {
            this->privateStreamSocketInitiateUpgrade(self, streamSocket);
        }
        else {
            this->privateStreamSocketUp(self, streamSocket);
        }
    }
}

void TestServer::processAcceptQueueHighWatermark(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_LISTENER_SOCKET_EVENT(listenerSocket,
                                              "accept queue",
                                              event);
}

void TestServer::processAcceptQueueDiscarded(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_LISTENER_SOCKET_EVENT(listenerSocket,
                                              "accept queue",
                                              event);
}

void TestServer::processShutdownInitiated(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_LISTENER_SOCKET_EVENT(listenerSocket,
                                              "shutdown",
                                              event);
}

void TestServer::processShutdownReceive(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_LISTENER_SOCKET_EVENT(listenerSocket,
                                              "shutdown",
                                              event);
}

void TestServer::processShutdownSend(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_LISTENER_SOCKET_EVENT(listenerSocket,
                                              "shutdown",
                                              event);
}

void TestServer::processShutdownComplete(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_LISTENER_SOCKET_EVENT(listenerSocket,
                                              "shutdown",
                                              event);
}

void TestServer::processError(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ErrorEvent&                      event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_LISTENER_SOCKET_EVENT(listenerSocket, "error", event);

    bsl::shared_ptr<Self> self(this->getSelf(this));
    LockGuard             lock(&d_mutex);

    this->privateListenerSocketError(self,
                                     listenerSocket,
                                     event.context().error());
}

void TestServer::processStreamSocketUpgradeEvent(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::UpgradeEvent&                  event)
{
    bsl::shared_ptr<Self> self(this->getSelf(this));
    LockGuard             lock(&d_mutex);

    this->privateStreamSocketCompleteUpgrade(self, streamSocket, event);
}

void TestServer::privateStreamSocketInitiateUpgrade(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(streamSocket);
}

void TestServer::privateStreamSocketCompleteUpgrade(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::UpgradeEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void TestServer::privateStreamSocketInitiateDowngrade(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(streamSocket);
}

void TestServer::privateStreamSocketCompleteDowngrade(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::DowngradeEvent&                event)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void TestServer::privateDatagramSocketUp(
    const bsl::shared_ptr<Self>&                 self,
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(datagramSocket);
}

void TestServer::privateDatagramSocketDown(
    const bsl::shared_ptr<Self>&                 self,
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(datagramSocket);
}

void TestServer::privateDatagramSocketError(
    const bsl::shared_ptr<Self>&                 self,
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntsa::Error&                           error)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(error);

    NTCCFG_NOT_IMPLEMENTED();
}

void TestServer::privateStreamSocketUp(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    ntsa::Error error;

    bsl::shared_ptr<ntcf::TestMessageParser> streamParser;
    streamParser.createInplace(d_allocator_p,
                               d_dataPool_sp,
                               d_messagePool_sp,
                               d_serialization_sp,
                               d_compression_sp,
                               d_allocator_p);

    bsl::pair<StreamSocketMap::iterator, bool> insertResult =
        d_streamSocketMap.insert(bsl::make_pair(streamSocket, streamParser));
    BSLS_ASSERT(insertResult.second);

    error = streamSocket->registerSession(self);
    BSLS_ASSERT_OPT(!error);

    error = streamSocket->registerManager(self);
    BSLS_ASSERT_OPT(!error);

    error = streamSocket->setReadQueueLowWatermark(streamParser->numNeeded());
    BSLS_ASSERT_OPT(!error);

    error = streamSocket->relaxFlowControl(ntca::FlowControlType::e_RECEIVE);
    BSLS_ASSERT_OPT(!error);
}

void TestServer::privateStreamSocketDown(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCCFG_WARNING_UNUSED(self);

    d_streamSocketMap.erase(streamSocket);
}

void TestServer::privateStreamSocketError(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntsa::Error&                         error)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(error);

    streamSocket->close();
}

void TestServer::privateListenerSocketUp(
    const bsl::shared_ptr<Self>&                 self,
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(listenerSocket);
}

void TestServer::privateListenerSocketDown(
    const bsl::shared_ptr<Self>&                 self,
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(listenerSocket);
}

void TestServer::privateListenerSocketError(
    const bsl::shared_ptr<Self>&                 self,
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntsa::Error&                           error)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(error);

    listenerSocket->close();
}

void TestServer::dispatchMessage(
    const bsl::shared_ptr<ntcf::TestServerTransaction>& transaction,
    const bsl::shared_ptr<ntcf::TestMessage>&           request)
{
    if (request->entity().has_value()) {
        const ntcf::TestMessageEntity& entity = request->entity().value();
        if (entity.isContentValue()) {
            const ntcf::TestContent& content = entity.content();
            if (content.isBidValue()) {
                this->processBid(transaction, content.bid());
                return;
            }
            else if (content.isAskValue()) {
                this->processAsk(transaction, content.ask());
                return;
            }
            else if (content.isSubscriptionValue()) {
                this->processSubscription(transaction, content.subscription());
                return;
            }
        }
        else if (entity.isControlValue()) {
            const ntcf::TestControl& control = entity.control();
            if (control.isSignalValue()) {
                this->processSignal(transaction, control.signal());
                return;
            }
            else if (control.isEncryptionValue()) {
                this->processEncryption(transaction, control.encryption());
                return;
            }
            else if (control.isCompressionValue()) {
                this->processCompression(transaction, control.compression());
                return;
            }
            else if (control.isHeartbeatValue()) {
                this->processHeartbeat(transaction, control.heartbeat());
                return;
            }
        }
    }

    // NTCF_TESTSERVER_LOG_PROCESSOR_NOT_FOUND(message, sender, endpoint);
}

void TestServer::processBid(
    const bsl::shared_ptr<ntcf::TestServerTransaction>& transaction,
    const ntcf::TestBid&                                bid)
{
    // MRM: TODO
}

void TestServer::processAsk(
    const bsl::shared_ptr<ntcf::TestServerTransaction>& transaction,
    const ntcf::TestAsk&                                ask)
{
    // MRM: TODO
}

void TestServer::processSubscription(
    const bsl::shared_ptr<ntcf::TestServerTransaction>& transaction,
    const ntcf::TestSubscription&                       subscription)
{
    // MRM: TODO
}

void TestServer::processSignal(
    const bsl::shared_ptr<ntcf::TestServerTransaction>& transaction,
    const ntcf::TestSignal&                             signal)
{
    ntcf::TestEcho echo;
    echo.id = signal.id;

    if (signal.reflect > 0) {
        ntscfg::TestDataUtil::generateData(
            &echo.value, 
            signal.reflect, 
            0, 
            ntscfg::TestDataUtil::k_DATASET_SERVER_COMPRESSABLE);
    }

    transaction->complete(echo);
}

void TestServer::processEncryption(
    const bsl::shared_ptr<ntcf::TestServerTransaction>& transaction,
    const ntcf::TestControlEncryption&                  encryption)
{
    ntsa::Error error;

    if (encryption.enabled) {
        transaction->enableEncryption(
            encryption.acknowledge, 
            encryption.transition);
    }
    else {
        transaction->disableEncryption(
            encryption.acknowledge, 
            encryption.transition);
    }
}

void TestServer::processCompression(
    const bsl::shared_ptr<ntcf::TestServerTransaction>& transaction,
    const ntcf::TestControlCompression&                 compression)
{
    if (compression.enabled) {
        transaction->enableCompression(
            compression.acknowledge, 
            compression.transition);
    }
    else {
        transaction->disableCompression(
            compression.acknowledge, 
            compression.transition);
    }
}

void TestServer::processHeartbeat(
    const bsl::shared_ptr<ntcf::TestServerTransaction>& transaction,
    const ntcf::TestControlHeartbeat&                   heartbeat)
{
    if (heartbeat.acknowledge) {
        transaction->acknowledge();
    }
}

TestServer::TestServer(const ntcf::TestServerConfig&           configuration,
                       const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                       const bsl::shared_ptr<ntci::DataPool>&  dataPool,
                       const bsl::shared_ptr<ntcf::TestMessageEncryption>& encryption,
                       bslma::Allocator*                       basicAllocator)
: d_mutex()
, d_dataPool_sp(dataPool)
, d_messagePool_sp()
, d_serialization_sp()
, d_compression_sp()
, d_scheduler_sp(scheduler)
, d_datagramSocket_sp()
, d_datagramParser_sp()
, d_datagramEndpoint()
, d_listenerSocket_sp()
, d_listenerEndpoint()
, d_streamSocketMap(basicAllocator)
, d_encryption_sp(encryption)
, d_closed(false)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    // MRM: BALL_LOG_INFO << "Server constructing starting" << BALL_LOG_END;

    bsl::shared_ptr<Self> self(this->getSelf(this));

    ntsa::Error error;

    if (d_config.name.isNull()) {
        d_config.name = "server";
    }

    d_dataPool_sp = ntcf::System::createDataPool(d_allocator_p);

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

#if NTCF_TESTCLIENT_DATAGRAM_SOCKET_ENABLED

    // MRM: BALL_LOG_INFO << "Server datagram socket construction starting"
    // MRM:               << BALL_LOG_END;

    ntca::DatagramSocketOptions datagramSocketOptions;
    datagramSocketOptions.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    datagramSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::Ipv4Endpoint(ntsa::Ipv4Address::loopback(), 0)));

    d_datagramSocket_sp =
        d_scheduler_sp->createDatagramSocket(datagramSocketOptions,
                                             d_allocator_p);

    error = d_datagramSocket_sp->registerSession(self);
    BSLS_ASSERT_OPT(!error);

    error = d_datagramSocket_sp->registerManager(self);
    BSLS_ASSERT_OPT(!error);

    error = d_datagramSocket_sp->open();
    BSLS_ASSERT_OPT(!error);

    d_datagramEndpoint = d_datagramSocket_sp->sourceEndpoint();

    // MRM: BALL_LOG_INFO << "Server datagram socket construction complete"
    // MRM:               << BALL_LOG_END;

#endif

    // MRM: BALL_LOG_INFO << "Server listening socket construction starting"
    // MRM:               << BALL_LOG_END;

    ntca::ListenerSocketOptions listenerSocketOptions;
    listenerSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
    listenerSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::Ipv4Endpoint(ntsa::Ipv4Address::loopback(), 0)));

    d_listenerSocket_sp =
        d_scheduler_sp->createListenerSocket(listenerSocketOptions,
                                             d_allocator_p);

    error = d_listenerSocket_sp->registerSession(self);
    BSLS_ASSERT_OPT(!error);

    error = d_listenerSocket_sp->registerManager(self);
    BSLS_ASSERT_OPT(!error);

    error = d_listenerSocket_sp->open();
    BSLS_ASSERT_OPT(!error);

    error = d_listenerSocket_sp->listen();
    BSLS_ASSERT_OPT(!error);

    d_listenerEndpoint = d_listenerSocket_sp->sourceEndpoint();

    // MRM: BALL_LOG_INFO << "Server listening socket construction complete"
    // MRM:               << BALL_LOG_END;

    // MRM: BALL_LOG_INFO << "Server construction complete" << BALL_LOG_END;

#if NTCF_TESTCLIENT_DATAGRAM_SOCKET_ENABLED

    error = d_datagramSocket_sp->relaxFlowControl(
        ntca::FlowControlType::e_RECEIVE);
    BSLS_ASSERT_OPT(!error);

#endif

    error = d_listenerSocket_sp->relaxFlowControl(
        ntca::FlowControlType::e_RECEIVE);
    BSLS_ASSERT_OPT(!error);
}

TestServer::~TestServer()
{
    this->close();
}

void TestServer::setAcceptQueueLowWatermark(bsl::size_t value)
{
    ntsa::Error error;

    error = d_listenerSocket_sp->setAcceptQueueLowWatermark(value);
    BSLS_ASSERT_OPT(!error);
}

void TestServer::setAcceptQueueHighWatermark(bsl::size_t value)
{
    ntsa::Error error;

    error = d_listenerSocket_sp->setAcceptQueueHighWatermark(value);
    BSLS_ASSERT_OPT(!error);
}

void TestServer::relaxFlowControl()
{
    ntsa::Error error;

    error = d_listenerSocket_sp->relaxFlowControl(
        ntca::FlowControlType::e_RECEIVE);
    BSLS_ASSERT_OPT(!error);
}

void TestServer::applyFlowControl()
{
    ntsa::Error error;

    error = d_listenerSocket_sp->applyFlowControl(
        ntca::FlowControlType::e_RECEIVE, ntca::FlowControlMode::e_IMMEDIATE);
    BSLS_ASSERT_OPT(!error);
}

void TestServer::close()
{
    if (d_closed) {
        return;
    }

    d_closed = true;

    // MRM: BALL_LOG_INFO << "Server destruction starting" << BALL_LOG_END;

    StreamSocketMap streamSocketMap(d_allocator_p);
    streamSocketMap.swap(d_streamSocketMap);

    typedef bsl::vector<bsl::shared_ptr<ntci::StreamSocket> >
        StreamSocketVector;

    StreamSocketVector streamSocketVector(d_allocator_p);
    streamSocketVector.reserve(streamSocketMap.size());

    for (StreamSocketMap::iterator it = streamSocketMap.begin();
         it != streamSocketMap.end();
         ++it)
    {
        streamSocketVector.push_back(it->first);
    }

    streamSocketMap.clear();

    while (!streamSocketVector.empty()) {
        bsl::shared_ptr<ntci::StreamSocket> streamSocket =
            streamSocketVector.back();
        streamSocketVector.pop_back();

        // MRM: BALL_LOG_INFO << "Server stream socket destruction starting"
        // MRM:               << BALL_LOG_END;

        {
            ntci::StreamSocketCloseGuard closeGuard(streamSocket);
        }

        streamSocket.reset();

        // MRM: BALL_LOG_INFO << "Server stream socket destruction complete"
        // MRM:               << BALL_LOG_END;
    }

    // MRM: BALL_LOG_INFO << "Server listening socket destruction starting"
    // MRM:               << BALL_LOG_END;

    if (d_listenerSocket_sp) {
        ntci::ListenerSocketCloseGuard closeGuard(d_listenerSocket_sp);
    }

    d_listenerSocket_sp.reset();

    // MRM: BALL_LOG_INFO << "Server listening socket destruction complete"
    // MRM:               << BALL_LOG_END;

    // MRM: BALL_LOG_INFO << "Server datagram socket destruction starting"
    // MRM:               << BALL_LOG_END;

    if (d_datagramSocket_sp) {
        ntci::DatagramSocketCloseGuard closeGuard(d_datagramSocket_sp);
    }

    d_datagramSocket_sp.reset();

    // MRM: BALL_LOG_INFO << "Server datagram socket destruction complete"
    // MRM:               << BALL_LOG_END;

    // MRM: BALL_LOG_INFO << "Server destruction complete" << BALL_LOG_END;
}

const ntsa::Endpoint& TestServer::tcpEndpoint() const
{
    return d_listenerEndpoint;
}

const ntsa::Endpoint& TestServer::udpEndpoint() const
{
    return d_datagramEndpoint;
}

}  // end namespace ntcf
}  // end namespace BloombergLP
#endif
