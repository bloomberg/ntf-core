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

namespace BloombergLP {
namespace ntcf {


#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_ESTABLISHED(datagramSocket)    \
    do {                                                                      \
        BALL_LOG_INFO << "Client datagram socket at "                                    \
                      << (datagramSocket)->sourceEndpoint() << " to "         \
                      << (datagramSocket)->remoteEndpoint() << " established" \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_CONNECTED(datagramSocket)    \
    do {                                                                      \
        BALL_LOG_INFO << "Client datagram socket at "                                    \
                      << (datagramSocket)->sourceEndpoint() << " to "         \
                      << (datagramSocket)->remoteEndpoint() << " connected" \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_CLOSED(datagramSocket)  \
    do {                                                                      \
        BALL_LOG_INFO << "Client datagram socket at "                                    \
                      << (datagramSocket)->sourceEndpoint() << " to "         \
                      << (datagramSocket)->remoteEndpoint() << " closed"      \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, type, event)        \
    do {                                                                      \
        BALL_LOG_TRACE << "Client datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint()                  \
                       << " processing " << (type) << " event " << (event)    \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RECEIVE_WOULD_BLOCK(datagramSocket)       \
    do {                                                                      \
        BALL_LOG_TRACE << "Client datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint()                  \
                       << " receive WOULD_BLOCK" << BALL_LOG_END;             \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RECEIVE_EOF(datagramSocket)               \
    do {                                                                      \
        BALL_LOG_TRACE << "Client datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint()                  \
                       << " receive EOF" << BALL_LOG_END;                     \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RECEIVE_FAILED(datagramSocket, error)     \
    do {                                                                      \
        BALL_LOG_TRACE << "Client datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint()                  \
                       << " receive failed: " << (error) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RECEIVE_CONTEXT(datagramSocket,           \
                                                    blob,                     \
                                                    receiveContext)           \
    do {                                                                      \
        BALL_LOG_TRACE << "Client datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint() << " received "  \
                       << (blob).length() << " bytes from context "           \
                       << (receiveContext) << BALL_LOG_END;                   \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_PARSE_FAILED(datagramSocket, error)       \
    do {                                                                      \
        BALL_LOG_ERROR << "Client datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint()                  \
                       << " parsing failed: " << (error) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RESPONSE_IGNORED(datagramSocket,          \
                                                     response)                \
    do {                                                                      \
        BALL_LOG_WARN << "Client datagram socket at "                                    \
                      << (datagramSocket)->sourceEndpoint() << " to "         \
                      << (datagramSocket)->remoteEndpoint()                   \
                      << " ignoring response " << (*response)                 \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RESPONSE_UNSOLICTED(datagramSocket,       \
                                                        response)             \
    do {                                                                      \
        BALL_LOG_WARN << "Client datagram socket at "                                    \
                      << (datagramSocket)->sourceEndpoint() << " to "         \
                      << (datagramSocket)->remoteEndpoint()                   \
                      << " received stale or unsolicited response "           \
                      << (*response) << BALL_LOG_END;                         \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_INCOMING_MESSAGE(datagramSocket, message) \
    do {                                                                      \
        BALL_LOG_DEBUG << "Client datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint()                  \
                       << " received message " << (*message) << BALL_LOG_END; \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_OUTGOING_MESSAGE(datagramSocket, message) \
    do {                                                                      \
        BALL_LOG_DEBUG << "Client datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint()                  \
                       << " sending message " << (*message) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_OUTGOING_BLOB(datagramSocket, blob)       \
    do {                                                                      \
        BALL_LOG_TRACE << "Client datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint() << " sending:\n" \
                       << bdlbb::BlobUtilHexDumper(&(blob)) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_INCOMING_BLOB(datagramSocket, blob)       \
    do {                                                                      \
        BALL_LOG_TRACE << "Client datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint() << " to "        \
                       << (datagramSocket)->remoteEndpoint()                  \
                       << " received:\n"                                      \
                       << bdlbb::BlobUtilHexDumper(&(blob)) << BALL_LOG_END;  \
    } while (false)





#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_CONNECTED(streamSocket)    \
    do {                                                                      \
        BALL_LOG_INFO << "Client stream socket at "                                    \
                      << (streamSocket)->sourceEndpoint() << " to "         \
                      << (streamSocket)->remoteEndpoint() << " connected" \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_CLOSED(streamSocket)  \
    do {                                                                      \
        BALL_LOG_INFO << "Client stream socket at "                                    \
                      << (streamSocket)->sourceEndpoint() << " to "         \
                      << (streamSocket)->remoteEndpoint() << " closed"      \
                      << BALL_LOG_END;                                        \
    } while (false)


#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, type, event)            \
    do {                                                                      \
        BALL_LOG_TRACE << "Client stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " processing "  \
                       << (type) << " event " << (event) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RECEIVE_WOULD_BLOCK(streamSocket)           \
    do {                                                                      \
        BALL_LOG_TRACE << "Client stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " receive WOULD_BLOCK" << BALL_LOG_END;             \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RECEIVE_EOF(streamSocket)                   \
    do {                                                                      \
        BALL_LOG_TRACE << "Client stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " receive EOF"  \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RECEIVE_FAILED(streamSocket, error)         \
    do {                                                                      \
        BALL_LOG_TRACE << "Client stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " receive failed: " << (error) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RECEIVE_CONTEXT(streamSocket,               \
                                                  blob,                       \
                                                  receiveContext)             \
    do {                                                                      \
        BALL_LOG_TRACE << "Client stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " received "    \
                       << (blob).length() << " bytes from context "           \
                       << (receiveContext) << BALL_LOG_END;                   \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_PARSE_FAILED(streamSocket, error)           \
    do {                                                                      \
        BALL_LOG_ERROR << "Client stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " parsing failed: " << (error) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RESPONSE_IGNORED(streamSocket, response)    \
    do {                                                                      \
        BALL_LOG_WARN << "Client stream socket at "                                    \
                      << (streamSocket)->sourceEndpoint() << " to "           \
                      << (streamSocket)->remoteEndpoint()                     \
                      << " ignoring response " << (*response)                 \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RESPONSE_UNSOLICTED(streamSocket, response) \
    do {                                                                      \
        BALL_LOG_WARN << "Client stream socket at "                                    \
                      << (streamSocket)->sourceEndpoint() << " to "           \
                      << (streamSocket)->remoteEndpoint()                     \
                      << " received stale or unsolicited response "           \
                      << (*response) << BALL_LOG_END;                         \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_INCOMING_MESSAGE(streamSocket, message)     \
    do {                                                                      \
        BALL_LOG_DEBUG << "Client stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " received message " << (*message) << BALL_LOG_END; \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_OUTGOING_MESSAGE(streamSocket, message)     \
    do {                                                                      \
        BALL_LOG_DEBUG << "Client stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " sending message " << (*message) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_OUTGOING_BLOB(streamSocket, blob)           \
    do {                                                                      \
        BALL_LOG_TRACE << "Client stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " sending:\n"   \
                       << bdlbb::BlobUtilHexDumper(&(blob)) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTCLIENT_LOG_STREAM_SOCKET_INCOMING_BLOB(streamSocket, blob)           \
    do {                                                                      \
        BALL_LOG_TRACE << "Client stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " received:\n"  \
                       << bdlbb::BlobUtilHexDumper(&(blob)) << BALL_LOG_END;  \
    } while (false)













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

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "read queue", event);
}

void TestClient::processReadQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "read queue", event);
}

void TestClient::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "read queue", event);

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
            NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RECEIVE_EOF(d_datagramSocket_sp);
        }
        else {
            NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RECEIVE_FAILED(d_datagramSocket_sp,
                                                       error);
        }

        d_datagramSocket_sp->close();
        return;
    }

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RECEIVE_CONTEXT(d_datagramSocket_sp,
                                                blob,
                                                receiveContext);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_INCOMING_BLOB(d_datagramSocket_sp, blob);

    // MRM
    #if 0
    error = d_messageParser.add(blob);
    if (error) {
        NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_PARSE_FAILED(d_datagramSocket_sp, error);
        d_datagramSocket_sp->close();
        return;
    }

    while (d_messageParser.hasAnyAvailable()) {
        bsl::shared_ptr<ntva::AsmpMessage> message;
        error = d_messageParser.dequeue(&message);
        if (error) {
            NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_PARSE_FAILED(d_datagramSocket_sp,
                                                     error);
            d_datagramSocket_sp->close();
            return;
        }

        NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_INCOMING_MESSAGE(d_datagramSocket_sp,
                                                     message);

        // d_manager_sp->processSocketIncomingMessage(self, message);

        if (message->isResponse()) {
            bsl::shared_ptr<ntvc::AsmpTransmission> transmission;
            if (d_transmissionCatalog.remove(&transmission,
                                             message->transaction()))
            {
                if (!transmission->invoke(self,
                                          message,
                                          ntsa::Error(),
                                          ntci::Strand::unknown()))
                {
                    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RESPONSE_IGNORED(
                        d_datagramSocket_sp,
                        message);
                }
                continue;
            }
            else {
                NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_RESPONSE_UNSOLICTED(
                    d_datagramSocket_sp,
                    message);
                continue;
            }
        }

        ntvi::AsmpMessageCallback callback;
        if (d_callbackQueue.pop(&callback)) {
            callback.execute(self,
                             message,
                             ntsa::Error(),
                             ntci::Strand::unknown());
            continue;
        }

        bsl::shared_ptr<ntvi::AsmpTransaction> transaction;
        bsl::shared_ptr<ntvi::AsmpProvider>    provider;

        if (receiveContext.endpoint().has_value()) {
            error = d_manager_sp->resolve(&transaction,
                                          &provider,
                                          self,
                                          message,
                                          receiveContext.endpoint().value());
        }
        else {
            error =
                d_manager_sp->resolve(&transaction, &provider, self, message);
        }

        if (error) {
            BALL_LOG_ERROR << "No provider registered for message " << *message
                           << BALL_LOG_END;

            bsl::shared_ptr<ntva::AsmpMessage> response =
                d_manager_sp->createMessage();
            response->reflect(*message);

            response->setAction(ntva::AsmpAction::e_REFUSED);
            response->setEntityEncoding(ntva::Encoding::e_NONE);

            this->send(response, ntva::AsmpMessageOptions());
            continue;
        }

        provider->processMessage(transaction);
    }
    #endif
}

void TestClient::processReadQueueHighWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "read queue", event);
}

void TestClient::processReadQueueDiscarded(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "read queue", event);
}

void TestClient::processWriteQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "write queue", event);
}

void TestClient::processWriteQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "write queue", event);
}

void TestClient::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "write queue", event);
}

void TestClient::processWriteQueueHighWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "write queue", event);
}

void TestClient::processWriteQueueDiscarded(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "write queue", event);
}

void TestClient::processShutdownInitiated(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "shutdown", event);
}

void TestClient::processShutdownReceive(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "shutdown", event);
}

void TestClient::processShutdownSend(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "shutdown", event);
}

void TestClient::processShutdownComplete(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "shutdown", event);
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
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "read queue", event);

    // MRM
#if 0
    bsl::shared_ptr<Self> self(this->getSelf(this));

    ntsa::Error error;

    ntca::ReceiveContext receiveContext;
    ntca::ReceiveOptions receiveOptions;
    bdlbb::Blob          blob;

    receiveOptions.setMinSize(d_messageParser.numNeeded());
    receiveOptions.setMaxSize(bsl::numeric_limits<bsl::size_t>::max());

    error = d_streamSocket_sp->receive(&receiveContext, &blob, receiveOptions);
    if (error) {
        if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK)) {
            NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RECEIVE_WOULD_BLOCK(d_streamSocket_sp);
        }
        else if (error == ntsa::Error(ntsa::Error::e_EOF)) {
            NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RECEIVE_EOF(d_streamSocket_sp);
        }
        else {
            NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RECEIVE_FAILED(d_streamSocket_sp, error);
        }

        d_streamSocket_sp->close();
        return;
    }

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RECEIVE_CONTEXT(d_streamSocket_sp,
                                              blob,
                                              receiveContext);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_INCOMING_BLOB(d_streamSocket_sp, blob);

    error = d_messageParser.add(blob);
    if (error) {
        NTCF_TESTCLIENT_LOG_STREAM_SOCKET_PARSE_FAILED(d_streamSocket_sp, error);
        d_streamSocket_sp->close();
        return;
    }

    while (d_messageParser.hasAnyAvailable()) {
        bsl::shared_ptr<ntva::AsmpMessage> message;
        error = d_messageParser.dequeue(&message);
        if (error) {
            NTCF_TESTCLIENT_LOG_STREAM_SOCKET_PARSE_FAILED(d_streamSocket_sp, error);
            d_streamSocket_sp->close();
            return;
        }

        NTCF_TESTCLIENT_LOG_STREAM_SOCKET_INCOMING_MESSAGE(d_streamSocket_sp, message);

        if (message->isResponse()) {
            bsl::shared_ptr<ntvc::AsmpTransmission> transmission;
            if (d_transmissionCatalog.remove(&transmission,
                                             message->transaction()))
            {
                if (!transmission->invoke(self,
                                          message,
                                          ntsa::Error(),
                                          ntci::Strand::unknown()))
                {
                    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RESPONSE_IGNORED(
                        d_streamSocket_sp,
                        message);
                }
                continue;
            }
            else {
                NTCF_TESTCLIENT_LOG_STREAM_SOCKET_RESPONSE_UNSOLICTED(
                    d_streamSocket_sp,
                    message);
                continue;
            }
        }

        ntvi::AsmpMessageCallback callback;
        if (d_callbackQueue.pop(&callback)) {
            callback.execute(self,
                             message,
                             ntsa::Error(),
                             ntci::Strand::unknown());
            continue;
        }

        bsl::shared_ptr<ntvi::AsmpTransaction> transaction;
        bsl::shared_ptr<ntvi::AsmpProvider>    provider;

        error = d_manager_sp->resolve(&transaction, &provider, self, message);
        if (error) {
            BALL_LOG_ERROR << "No provider registered for message " << *message
                           << BALL_LOG_END;

            bsl::shared_ptr<ntva::AsmpMessage> response =
                d_manager_sp->createMessage();
            response->reflect(*message);

            response->setAction(ntva::AsmpAction::e_REFUSED);
            response->setEntityEncoding(ntva::Encoding::e_NONE);

            this->send(response, ntva::AsmpMessageOptions());
            continue;
        }

        provider->processMessage(transaction);
    }
#endif
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

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "write queue", event);
}

void TestClient::processWriteQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "write queue", event);
}

void TestClient::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "write queue", event);
}

void TestClient::processWriteQueueHighWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "write queue", event);
}

void TestClient::processWriteQueueDiscarded(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(streamSocket, "write queue", event);
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
    const ntca::UpgradeEvent&                      event)
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

    NTCCFG_NOT_IMPLEMENTED();
}



void TestClient::privateStreamSocketInitiateDowngrade(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(streamSocket);

    NTCCFG_NOT_IMPLEMENTED();
}

void TestClient::privateStreamSocketCompleteDowngrade(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::DowngradeEvent&                event)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCCFG_NOT_IMPLEMENTED();
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

const bsl::shared_ptr<ntci::Strand>& TestClient::strand() const
{
    return d_strand_sp;
}

TestClient::TestClient(const ntcf::TestClientConfig& configuration,
                       const ntsa::Endpoint&         tcpEndpoint,
                       const ntsa::Endpoint&         udpEndpoint,
                       bslma::Allocator*             basicAllocator)
: d_mutex()
, d_dataPool_sp()
, d_messagePool_sp()
, d_serialization_sp()
, d_compression_sp()
, d_scheduler_sp()
, d_datagramSocket_sp()
, d_datagramParser_sp()
, d_streamSocket_sp()
, d_streamParser_sp()
, d_tcpEndpoint(tcpEndpoint)
, d_udpEndpoint(udpEndpoint)
, d_closed(false)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BALL_LOG_INFO << "Client constructing starting" << BALL_LOG_END;

    bsl::shared_ptr<Self> self(this->getSelf(this));

    ntsa::Error error;

    if (d_config.name.isNull()) {
        d_config.name = "client";
    }

    d_dataPool_sp = ntcf::System::createDataPool(d_allocator_p);

    d_messagePool_sp.createInplace(d_allocator_p, d_allocator_p);

    ntca::SerializationConfig serializationConfig;

    d_serialization_sp =
        ntcf::System::createSerialization(serializationConfig, d_allocator_p);

    ntca::CompressionConfig compressionConfig;
    compressionConfig.setType(ntca::CompressionType::e_RLE);
    compressionConfig.setGoal(ntca::CompressionGoal::e_BALANCED);

    d_compression_sp = ntcf::System::createCompression(compressionConfig,
                                                       d_dataPool_sp,
                                                       d_allocator_p);

    BALL_LOG_INFO << "Client scheduler construction starting" << BALL_LOG_END;

    ntca::SchedulerConfig schedulerConfig;

    schedulerConfig.setThreadName(d_config.name.value());

    if (d_config.driver.has_value()) {
        schedulerConfig.setDriverName(d_config.driver.value());
    }

    if (d_config.numNetworkingThreads.has_value()) {
        schedulerConfig.setMinThreads(d_config.numNetworkingThreads.value());
        schedulerConfig.setMaxThreads(d_config.numNetworkingThreads.value());
    }

    if (d_config.dynamicLoadBalancing.has_value()) {
        schedulerConfig.setDynamicLoadBalancing(
            d_config.dynamicLoadBalancing.value());
    }

    if (d_config.keepAlive.has_value()) {
        schedulerConfig.setKeepAlive(d_config.keepAlive.value());
    }

    if (d_config.keepHalfOpen.has_value()) {
        schedulerConfig.setKeepHalfOpen(d_config.keepHalfOpen.value());
    }

    if (d_config.backlog.has_value()) {
        schedulerConfig.setBacklog(d_config.backlog.value());
    }

    if (d_config.sendBufferSize.has_value()) {
        schedulerConfig.setSendBufferSize(d_config.sendBufferSize.value());
    }

    if (d_config.receiveBufferSize.has_value()) {
        schedulerConfig.setBacklog(d_config.receiveBufferSize.value());
    }

    if (d_config.acceptGreedily.has_value()) {
        schedulerConfig.setAcceptGreedily(d_config.acceptGreedily.value());
    }

    if (d_config.acceptQueueLowWatermark.has_value()) {
        schedulerConfig.setAcceptQueueLowWatermark(
            d_config.acceptQueueLowWatermark.value());
    }

    if (d_config.acceptQueueHighWatermark.has_value()) {
        schedulerConfig.setAcceptQueueHighWatermark(
            d_config.acceptQueueHighWatermark.value());
    }

    if (d_config.readQueueLowWatermark.has_value()) {
        schedulerConfig.setReadQueueLowWatermark(
            d_config.readQueueLowWatermark.value());
    }

    if (d_config.readQueueHighWatermark.has_value()) {
        schedulerConfig.setReadQueueHighWatermark(
            d_config.readQueueHighWatermark.value());
    }

    if (d_config.writeQueueLowWatermark.has_value()) {
        schedulerConfig.setWriteQueueLowWatermark(
            d_config.writeQueueLowWatermark.value());
    }

    if (d_config.writeQueueHighWatermark.has_value()) {
        schedulerConfig.setWriteQueueHighWatermark(
            d_config.writeQueueHighWatermark.value());
    }

    d_scheduler_sp = ntcf::System::createScheduler(schedulerConfig,
                                                   d_dataPool_sp,
                                                   d_allocator_p);

    error = d_scheduler_sp->start();
    BSLS_ASSERT_OPT(!error);

    BALL_LOG_INFO << "Client scheduler construction complete" << BALL_LOG_END;

    BALL_LOG_INFO << "Client datagram socket construction starting"
                  << BALL_LOG_END;

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

    // MRM
#if 0
    {
        ntca::ConnectOptions connectOptions;

        ntci::ConnectCallback connectCallback = 
            d_streamSocket_sp->createConnectCallback(
                NTCCFG_BIND(&TestClient::processDatagramSocketConnectEvent,
                            this,
                            d_datagramSocket_sp,
                            NTCCFG_BIND_PLACEHOLDER_2),
                d_allocator_p);

        error = d_datagramSocket_sp->connect(udpEndpoint, 
                                             connectOptions, 
                                             connectCallback);
        BSLS_ASSERT_OPT(!error);
    }
#endif

    {
        ntci::ConnectFuture connectFuture;
        error = d_datagramSocket_sp->connect(d_udpEndpoint,
                                            ntca::ConnectOptions(),
                                            connectFuture);
        BSLS_ASSERT_OPT(!error);

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        BSLS_ASSERT_OPT(!error); 
        BSLS_ASSERT_OPT(!connectResult.event().context().error());

        NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_EVENT(
            d_datagramSocket_sp, "connect", connectResult.event());

        NTCF_TESTCLIENT_LOG_DATAGRAM_SOCKET_CONNECTED(d_datagramSocket_sp);
    }

    BALL_LOG_INFO << "Client datagram socket construction complete"
                  << BALL_LOG_END;

    BALL_LOG_INFO << "Client stream socket construction starting"
                  << BALL_LOG_END;

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    d_streamSocket_sp =
        d_scheduler_sp->createStreamSocket(streamSocketOptions,
                                           d_allocator_p);

    error = d_streamSocket_sp->registerSession(self);
    BSLS_ASSERT_OPT(!error);
    
    error = d_streamSocket_sp->registerManager(self);
    BSLS_ASSERT_OPT(!error);

    error = d_streamSocket_sp->open();
    BSLS_ASSERT_OPT(!error);

    // MRM
#if 0
    {
        ntca::ConnectOptions connectOptions;

        ntci::ConnectCallback connectCallback = 
            d_streamSocket_sp->createConnectCallback(
                NTCCFG_BIND(&TestClient::processStreamSocketConnectEvent,
                            this,
                            d_streamSocket_sp,
                            NTCCFG_BIND_PLACEHOLDER_2),
                d_allocator_p);

        error = d_streamSocket_sp->connect(tcpEndpoint, 
                                           connectOptions, 
                                           connectCallback);
        BSLS_ASSERT_OPT(!error);
    }
#endif

    {
        ntci::ConnectFuture connectFuture;
        error = d_streamSocket_sp->connect(d_tcpEndpoint,
                                        ntca::ConnectOptions(),
                                        connectFuture);
        BSLS_ASSERT_OPT(!error); 

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        BSLS_ASSERT_OPT(!error); 
        BSLS_ASSERT_OPT(!connectResult.event().context().error());

        NTCF_TESTCLIENT_LOG_STREAM_SOCKET_EVENT(
            d_streamSocket_sp, "connect", connectResult.event());

        NTCF_TESTCLIENT_LOG_STREAM_SOCKET_CONNECTED(d_streamSocket_sp);
    }

    BALL_LOG_INFO << "Client stream socket construction complete"
                  << BALL_LOG_END;

    BALL_LOG_INFO << "Client construction complete" << BALL_LOG_END;

    error = d_datagramSocket_sp->relaxFlowControl(
        ntca::FlowControlType::e_RECEIVE);
    BSLS_ASSERT_OPT(!error);

    error = d_streamSocket_sp->relaxFlowControl(
        ntca::FlowControlType::e_RECEIVE);
    BSLS_ASSERT_OPT(!error);
}

TestClient::~TestClient()
{
    BALL_LOG_INFO << "Client destruction starting" << BALL_LOG_END;

    BALL_LOG_INFO << "Client stream socket destruction starting"
                  << BALL_LOG_END;

    {
        ntci::StreamSocketCloseGuard closeGuard(d_streamSocket_sp);
    }

    d_streamSocket_sp.reset();

    BALL_LOG_INFO << "Client stream socket destruction complete"
                  << BALL_LOG_END;

    BALL_LOG_INFO << "Client datagram socket destruction starting"
                  << BALL_LOG_END;

    {
        ntci::DatagramSocketCloseGuard closeGuard(d_datagramSocket_sp);
    }

    d_datagramSocket_sp.reset();

    BALL_LOG_INFO << "Client datagram socket destruction complete"
                  << BALL_LOG_END;

    BALL_LOG_INFO << "Client scheduler destruction starting" << BALL_LOG_END;

    d_scheduler_sp->shutdown();
    d_scheduler_sp->linger();
    d_scheduler_sp.reset();

    BALL_LOG_INFO << "Client scheduler destruction complete" << BALL_LOG_END;

    BALL_LOG_INFO << "Client destruction complete" << BALL_LOG_END;
}

ntsa::Error TestClient::bid(
    ntcf::TestFault* fault,
    ntcf::TestTrade* trade, 
    const ntcf::TestBid& bid)
{
    NTCCFG_WARNING_UNUSED(fault);
    NTCCFG_WARNING_UNUSED(trade);
    NTCCFG_WARNING_UNUSED(bid);

    NTCCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error TestClient::bid(
    const ntcf::TestBid&           bid,
    const ntcf::TestTradeCallback& callback)
{
    NTCCFG_WARNING_UNUSED(bid);
    NTCCFG_WARNING_UNUSED(callback);

    NTCCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error TestClient::ask(
    ntcf::TestFault*     fault,
    ntcf::TestTrade*     trade, 
    const ntcf::TestAsk& ask)
{
    NTCCFG_WARNING_UNUSED(fault);
    NTCCFG_WARNING_UNUSED(trade);
    NTCCFG_WARNING_UNUSED(ask);

    NTCCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error TestClient::ask(
    const ntcf::TestAsk&           ask,
    const ntcf::TestTradeCallback& callback)
{
    NTCCFG_WARNING_UNUSED(ask);
    NTCCFG_WARNING_UNUSED(callback);

    NTCCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error TestClient::encrypt(
    ntcf::TestFault*     fault,
    ntcf::TestAcknowledgment*    acknowledgement, 
    const TestControlEncryption& encryption)
{
    NTCCFG_WARNING_UNUSED(fault);
    NTCCFG_WARNING_UNUSED(acknowledgement);
    NTCCFG_WARNING_UNUSED(encryption);

    NTCCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error TestClient::encrypt(
    const ntcf::TestControlEncryption&      encryption,
    const ntcf::TestAcknowledgmentCallback& callback)
{
    NTCCFG_WARNING_UNUSED(encryption);
    NTCCFG_WARNING_UNUSED(callback);

    NTCCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error TestClient::compress(
    ntcf::TestFault*              fault,
    ntcf::TestAcknowledgment*     acknowledgement, 
    const TestControlCompression& compression)
{
    NTCCFG_WARNING_UNUSED(fault);
    NTCCFG_WARNING_UNUSED(acknowledgement);
    NTCCFG_WARNING_UNUSED(compression);

    NTCCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error TestClient::compress(
    const ntcf::TestControlCompression&     compression,
    const ntcf::TestAcknowledgmentCallback& callback)
{
    NTCCFG_WARNING_UNUSED(compression);
    NTCCFG_WARNING_UNUSED(callback);

    NTCCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error TestClient::heartbeat(
        ntcf::TestFault*     fault,
        ntcf::TestAcknowledgment*   acknowledgement, 
        const TestControlHeartbeat& heartbeat)
{
    NTCCFG_WARNING_UNUSED(fault);
    NTCCFG_WARNING_UNUSED(acknowledgement);
    NTCCFG_WARNING_UNUSED(heartbeat);

    NTCCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error TestClient::heartbeat(
    const ntcf::TestControlHeartbeat&       heartbeat,
    const ntcf::TestAcknowledgmentCallback& callback)
{
    NTCCFG_WARNING_UNUSED(heartbeat);
    NTCCFG_WARNING_UNUSED(callback);

    NTCCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

}  // end namespace ntcf
}  // end namespace BloombergLP
#endif
