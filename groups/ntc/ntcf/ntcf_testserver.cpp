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

namespace BloombergLP {
namespace ntcf {

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_ESTABLISHED(datagramSocket)    \
    do {                                                                      \
        BALL_LOG_INFO << "Server datagram socket at "                                    \
                      << (datagramSocket)->sourceEndpoint() << " established" \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_CLOSED(datagramSocket)  \
    do {                                                                      \
        BALL_LOG_INFO << "Server datagram socket at "                                    \
                      << (datagramSocket)->sourceEndpoint() << " closed"      \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, type, event)        \
    do {                                                                      \
        BALL_LOG_TRACE << "Server datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint()                 \
                       << " processing " << (type) << " event " << (event)    \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RECEIVE_WOULD_BLOCK(datagramSocket)       \
    do {                                                                      \
        BALL_LOG_TRACE << "Server datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint()                \
                       << " receive WOULD_BLOCK" << BALL_LOG_END;             \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RECEIVE_EOF(datagramSocket)               \
    do {                                                                      \
        BALL_LOG_TRACE << "Server datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint() \
                       << " receive EOF" << BALL_LOG_END;                     \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RECEIVE_FAILED(datagramSocket, error)     \
    do {                                                                      \
        BALL_LOG_TRACE << "Server datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint()                   \
                       << " receive failed: " << (error) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RECEIVE_CONTEXT(datagramSocket,           \
                                                    blob,                     \
                                                    receiveContext)           \
    do {                                                                      \
        BALL_LOG_TRACE << "Server datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint() << " received "  \
                       << (blob).length() << " bytes from context "           \
                       << (receiveContext) << BALL_LOG_END;                   \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_PARSE_FAILED(datagramSocket, error)       \
    do {                                                                      \
        BALL_LOG_ERROR << "Server datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint()                   \
                       << " parsing failed: " << (error) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RESPONSE_IGNORED(datagramSocket,          \
                                                     response)                \
    do {                                                                      \
        BALL_LOG_WARN << "Server datagram socket at "                                    \
                      << (datagramSocket)->sourceEndpoint()                    \
                      << " ignoring response " << (*response)                 \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RESPONSE_UNSOLICTED(datagramSocket,       \
                                                        response)             \
    do {                                                                      \
        BALL_LOG_WARN << "Server datagram socket at "                                    \
                      << (datagramSocket)->sourceEndpoint()                   \
                      << " received stale or unsolicited response "           \
                      << (*response) << BALL_LOG_END;                         \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_INCOMING_MESSAGE(datagramSocket, message) \
    do {                                                                      \
        BALL_LOG_DEBUG << "Server datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint()                   \
                       << " received message " << (*message) << BALL_LOG_END; \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_OUTGOING_MESSAGE(datagramSocket, message) \
    do {                                                                      \
        BALL_LOG_DEBUG << "Server datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint()                   \
                       << " sending message " << (*message) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_OUTGOING_BLOB(datagramSocket, blob)       \
    do {                                                                      \
        BALL_LOG_TRACE << "Server datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint() << " sending:\n" \
                       << bdlbb::BlobUtilHexDumper(&(blob)) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_INCOMING_BLOB(datagramSocket, blob)       \
    do {                                                                      \
        BALL_LOG_TRACE << "Server datagram socket at "                                   \
                       << (datagramSocket)->sourceEndpoint()                   \
                       << " received:\n"                                      \
                       << bdlbb::BlobUtilHexDumper(&(blob)) << BALL_LOG_END;  \
    } while (false)





#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_ACCEPTED(streamSocket)      \
    do {                                                                      \
        BALL_LOG_INFO << "Server stream socket at "                                    \
                      << (streamSocket)->sourceEndpoint() << " to "           \
                      << (streamSocket)->remoteEndpoint() << " accepted"      \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_CLOSED(streamSocket)        \
    do {                                                                      \
        BALL_LOG_INFO << "Server stream socket at "                                    \
                      << (streamSocket)->sourceEndpoint() << " to "           \
                      << (streamSocket)->remoteEndpoint() << " closed"        \
                      << BALL_LOG_END;                                        \
    } while (false)


#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, type, event)            \
    do {                                                                      \
        BALL_LOG_TRACE << "Server stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " processing "  \
                       << (type) << " event " << (event) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_RECEIVE_WOULD_BLOCK(streamSocket)           \
    do {                                                                      \
        BALL_LOG_TRACE << "Server stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " receive WOULD_BLOCK" << BALL_LOG_END;             \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_RECEIVE_EOF(streamSocket)                   \
    do {                                                                      \
        BALL_LOG_TRACE << "Server stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " receive EOF"  \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_RECEIVE_FAILED(streamSocket, error)         \
    do {                                                                      \
        BALL_LOG_TRACE << "Server stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " receive failed: " << (error) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_RECEIVE_CONTEXT(streamSocket,               \
                                                  blob,                       \
                                                  receiveContext)             \
    do {                                                                      \
        BALL_LOG_TRACE << "Server stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " received "    \
                       << (blob).length() << " bytes from context "           \
                       << (receiveContext) << BALL_LOG_END;                   \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_PARSE_FAILED(streamSocket, error)           \
    do {                                                                      \
        BALL_LOG_ERROR << "Server stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " parsing failed: " << (error) << BALL_LOG_END;     \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_RESPONSE_IGNORED(streamSocket, response)    \
    do {                                                                      \
        BALL_LOG_WARN << "Server stream socket at "                                    \
                      << (streamSocket)->sourceEndpoint() << " to "           \
                      << (streamSocket)->remoteEndpoint()                     \
                      << " ignoring response " << (*response)                 \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_RESPONSE_UNSOLICTED(streamSocket, response) \
    do {                                                                      \
        BALL_LOG_WARN << "Server stream socket at "                                    \
                      << (streamSocket)->sourceEndpoint() << " to "           \
                      << (streamSocket)->remoteEndpoint()                     \
                      << " received stale or unsolicited response "           \
                      << (*response) << BALL_LOG_END;                         \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_INCOMING_MESSAGE(streamSocket, message)     \
    do {                                                                      \
        BALL_LOG_DEBUG << "Server stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " received message " << (*message) << BALL_LOG_END; \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_OUTGOING_MESSAGE(streamSocket, message)     \
    do {                                                                      \
        BALL_LOG_DEBUG << "Server stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint()                    \
                       << " sending message " << (*message) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_OUTGOING_BLOB(streamSocket, blob)           \
    do {                                                                      \
        BALL_LOG_TRACE << "Server stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " sending:\n"   \
                       << bdlbb::BlobUtilHexDumper(&(blob)) << BALL_LOG_END;  \
    } while (false)

#define NTCF_TESTSERVER_LOG_STREAM_SOCKET_INCOMING_BLOB(streamSocket, blob)           \
    do {                                                                      \
        BALL_LOG_TRACE << "Server stream socket at "                                   \
                       << (streamSocket)->sourceEndpoint() << " to "          \
                       << (streamSocket)->remoteEndpoint() << " received:\n"  \
                       << bdlbb::BlobUtilHexDumper(&(blob)) << BALL_LOG_END;  \
    } while (false)







#define NTCF_TESTSERVER_LOG_LISTENER_SOCKET_ESTABLISHED(              \
    listenerSocket)                                                           \
    do {                                                                      \
        BALL_LOG_DEBUG << "Server listener socket at "                                 \
                       << (listenerSocket)->sourceEndpoint()                  \
                       << " established" << BALL_LOG_END;                     \
    } while (false)

#define NTCF_TESTSERVER_LOG_LISTENER_SOCKET_CLOSED(listenerSocket)    \
    do {                                                                      \
        BALL_LOG_DEBUG << "Server listener socket at "                                 \
                       << (listenerSocket)->sourceEndpoint() << " closed"     \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTCF_TESTSERVER_LOG_LISTENER_SOCKET_EVENT(listenerSocket,     \
                                                          type,               \
                                                          event)              \
    do {                                                                      \
        BALL_LOG_TRACE << "Server listener socket at "                                 \
                       << (listenerSocket)->sourceEndpoint()                  \
                       << " processing " << (type) << " event " << (event)    \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTCF_TESTSERVER_LOG_LISTENER_SOCKET_ACCEPT_WOULD_BLOCK(       \
    listenerSocket)                                                           \
    do {                                                                      \
        BALL_LOG_TRACE << "Server listener socket at "                                 \
                       << (listenerSocket)->sourceEndpoint()                  \
                       << " accept WOULD_BLOCK" << BALL_LOG_END;              \
    } while (false)

#define NTCF_TESTSERVER_LOG_LISTENER_SOCKET_ACCEPT_EOF(               \
    listenerSocket)                                                           \
    do {                                                                      \
        BALL_LOG_TRACE << "Server listener socket at "                                 \
                       << (listenerSocket)->sourceEndpoint() << " accept EOF" \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTCF_TESTSERVER_LOG_LISTENER_SOCKET_ACCEPT_FAILED(            \
    listenerSocket,                                                           \
    error)                                                                    \
    do {                                                                      \
        BALL_LOG_TRACE << "Server listener socket at "                                 \
                       << (listenerSocket)->sourceEndpoint()                  \
                       << " accept failed: " << (error) << BALL_LOG_END;      \
    } while (false)

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

    this->privateListenerSocketUp(
        self, 
        listenerSocket);
}

void TestServer::processListenerSocketClosed(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);

    NTCF_TESTSERVER_LOG_LISTENER_SOCKET_CLOSED(listenerSocket);

    bsl::shared_ptr<Self> self(this->getSelf(this));
    LockGuard             lock(&d_mutex);

    this->privateListenerSocketDown(
        self, 
        listenerSocket);
}










void TestServer::processReadQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "read queue", event);
}

void TestServer::processReadQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "read queue", event);
}

void TestServer::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "read queue", event);

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
            NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RECEIVE_WOULD_BLOCK(
                d_datagramSocket_sp);
        }
        else if (error == ntsa::Error(ntsa::Error::e_EOF)) {
            NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RECEIVE_EOF(d_datagramSocket_sp);
        }
        else {
            NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RECEIVE_FAILED(d_datagramSocket_sp,
                                                       error);
        }

        d_datagramSocket_sp->close();
        return;
    }

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RECEIVE_CONTEXT(d_datagramSocket_sp,
                                                blob,
                                                receiveContext);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_INCOMING_BLOB(d_datagramSocket_sp, blob);

    // MRM
    #if 0
    error = d_messageParser.add(blob);
    if (error) {
        NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_PARSE_FAILED(d_datagramSocket_sp, error);
        d_datagramSocket_sp->close();
        return;
    }

    while (d_messageParser.hasAnyAvailable()) {
        bsl::shared_ptr<ntva::AsmpMessage> message;
        error = d_messageParser.dequeue(&message);
        if (error) {
            NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_PARSE_FAILED(d_datagramSocket_sp,
                                                     error);
            d_datagramSocket_sp->close();
            return;
        }

        NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_INCOMING_MESSAGE(d_datagramSocket_sp,
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
                    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RESPONSE_IGNORED(
                        d_datagramSocket_sp,
                        message);
                }
                continue;
            }
            else {
                NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_RESPONSE_UNSOLICTED(
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

void TestServer::processReadQueueHighWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "read queue", event);
}

void TestServer::processReadQueueDiscarded(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "read queue", event);
}

void TestServer::processWriteQueueFlowControlRelaxed(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "write queue", event);
}

void TestServer::processWriteQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "write queue", event);
}

void TestServer::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "write queue", event);
}

void TestServer::processWriteQueueHighWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "write queue", event);
}

void TestServer::processWriteQueueDiscarded(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "write queue", event);
}

void TestServer::processShutdownInitiated(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "shutdown", event);
}

void TestServer::processShutdownReceive(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "shutdown", event);
}

void TestServer::processShutdownSend(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "shutdown", event);
}

void TestServer::processShutdownComplete(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_DATAGRAM_SOCKET_EVENT(datagramSocket, "shutdown", event);
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
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "read queue", event);

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
            NTCF_TESTSERVER_LOG_STREAM_SOCKET_RECEIVE_WOULD_BLOCK(d_streamSocket_sp);
        }
        else if (error == ntsa::Error(ntsa::Error::e_EOF)) {
            NTCF_TESTSERVER_LOG_STREAM_SOCKET_RECEIVE_EOF(d_streamSocket_sp);
        }
        else {
            NTCF_TESTSERVER_LOG_STREAM_SOCKET_RECEIVE_FAILED(d_streamSocket_sp, error);
        }

        d_streamSocket_sp->close();
        return;
    }

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_RECEIVE_CONTEXT(d_streamSocket_sp,
                                              blob,
                                              receiveContext);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_INCOMING_BLOB(d_streamSocket_sp, blob);

    error = d_messageParser.add(blob);
    if (error) {
        NTCF_TESTSERVER_LOG_STREAM_SOCKET_PARSE_FAILED(d_streamSocket_sp, error);
        d_streamSocket_sp->close();
        return;
    }

    while (d_messageParser.hasAnyAvailable()) {
        bsl::shared_ptr<ntva::AsmpMessage> message;
        error = d_messageParser.dequeue(&message);
        if (error) {
            NTCF_TESTSERVER_LOG_STREAM_SOCKET_PARSE_FAILED(d_streamSocket_sp, error);
            d_streamSocket_sp->close();
            return;
        }

        NTCF_TESTSERVER_LOG_STREAM_SOCKET_INCOMING_MESSAGE(d_streamSocket_sp, message);

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
                    NTCF_TESTSERVER_LOG_STREAM_SOCKET_RESPONSE_IGNORED(
                        d_streamSocket_sp,
                        message);
                }
                continue;
            }
            else {
                NTCF_TESTSERVER_LOG_STREAM_SOCKET_RESPONSE_UNSOLICTED(
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

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "write queue", event);
}

void TestServer::processWriteQueueFlowControlApplied(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "write queue", event);
}

void TestServer::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "write queue", event);
}

void TestServer::processWriteQueueHighWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "write queue", event);
}

void TestServer::processWriteQueueDiscarded(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCF_TESTSERVER_LOG_STREAM_SOCKET_EVENT(streamSocket, "write queue", event);
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
                NTCF_TESTSERVER_LOG_LISTENER_SOCKET_ACCEPT_EOF(
                    listenerSocket);
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

    NTCF_TESTSERVER_LOG_LISTENER_SOCKET_EVENT(listenerSocket,
                                                      "error",
                                                      event);

    bsl::shared_ptr<Self> self(this->getSelf(this));
    LockGuard             lock(&d_mutex);

    this->privateListenerSocketError(
        self, 
        listenerSocket, 
        event.context().error());
}



void TestServer::processStreamSocketUpgradeEvent(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::UpgradeEvent&                      event)
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

    NTCCFG_NOT_IMPLEMENTED();
}

void TestServer::privateStreamSocketCompleteUpgrade(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::UpgradeEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCCFG_NOT_IMPLEMENTED();
}



void TestServer::privateStreamSocketInitiateDowngrade(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(streamSocket);

    NTCCFG_NOT_IMPLEMENTED();
}

void TestServer::privateStreamSocketCompleteDowngrade(
    const bsl::shared_ptr<Self>&               self,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::DowngradeEvent&                event)
{
    NTCCFG_WARNING_UNUSED(self);
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    NTCCFG_NOT_IMPLEMENTED();
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

    error = streamSocket->relaxFlowControl(
        ntca::FlowControlType::e_RECEIVE);
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

TestServer::TestServer(const ntcf::TestServerConfig& configuration,
                       bslma::Allocator*             basicAllocator)
: d_mutex()
, d_dataPool_sp()
, d_messagePool_sp()
, d_serialization_sp()
, d_compression_sp()
, d_scheduler_sp()
, d_datagramSocket_sp()
, d_datagramParser_sp()
, d_datagramEndpoint()
, d_listenerSocket_sp()
, d_listenerEndpoint()
, d_streamSocketMap(basicAllocator)
, d_closed(false)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BALL_LOG_INFO << "Server constructing starting" << BALL_LOG_END;

    bsl::shared_ptr<Self> self(this->getSelf(this));

    ntsa::Error error;

    if (d_config.name.isNull()) {
        d_config.name = "server";
    }

    d_dataPool_sp = ntcf::System::createDataPool(d_allocator_p);

    d_messagePool_sp.createInplace(d_allocator_p, d_allocator_p);

    ntca::SerializationConfig serializationConfig;

    d_serialization_sp = ntcf::System::createSerialization(
        serializationConfig, d_allocator_p);

    ntca::CompressionConfig compressionConfig;
    compressionConfig.setType(ntca::CompressionType::e_RLE);
    compressionConfig.setGoal(ntca::CompressionGoal::e_BALANCED);

    d_compression_sp = ntcf::System::createCompression(
            compressionConfig, d_dataPool_sp, d_allocator_p);

    BALL_LOG_INFO << "Server scheduler construction starting" 
                  << BALL_LOG_END;

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
        schedulerConfig.setKeepAlive(
            d_config.keepAlive.value());
    }

    if (d_config.keepHalfOpen.has_value()) {
        schedulerConfig.setKeepHalfOpen(
            d_config.keepHalfOpen.value());
    }

    if (d_config.backlog.has_value()) {
        schedulerConfig.setBacklog(d_config.backlog.value());
    }

    if (d_config.sendBufferSize.has_value()) {
        schedulerConfig.setSendBufferSize(
            d_config.sendBufferSize.value());
    }

    if (d_config.receiveBufferSize.has_value()) {
        schedulerConfig.setBacklog(
            d_config.receiveBufferSize.value());
    }

    if (d_config.acceptGreedily.has_value()) {
        schedulerConfig.setAcceptGreedily(
            d_config.acceptGreedily.value());
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

    d_scheduler_sp = ntcf::System::createScheduler(
        schedulerConfig, d_dataPool_sp, d_allocator_p);

    error = d_scheduler_sp->start();
    BSLS_ASSERT_OPT(!error);

    BALL_LOG_INFO << "Server scheduler construction complete" 
                  << BALL_LOG_END;

    BALL_LOG_INFO << "Server datagram socket construction starting" 
                  << BALL_LOG_END;

    ntca::DatagramSocketOptions datagramSocketOptions;
    datagramSocketOptions.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
    datagramSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::Ipv4Endpoint(ntsa::Ipv4Address::loopback(), 0)));

    d_datagramSocket_sp = 
        d_scheduler_sp->createDatagramSocket(
            datagramSocketOptions, d_allocator_p);

    error = d_datagramSocket_sp->registerSession(self);
    BSLS_ASSERT_OPT(!error);
    
    error = d_datagramSocket_sp->registerManager(self);
    BSLS_ASSERT_OPT(!error);

    error = d_datagramSocket_sp->open();
    BSLS_ASSERT_OPT(!error);

    d_datagramEndpoint = d_datagramSocket_sp->sourceEndpoint();

    BALL_LOG_INFO << "Server datagram socket construction complete" 
                  << BALL_LOG_END;

    BALL_LOG_INFO << "Server listening socket construction starting" 
                  << BALL_LOG_END;

    ntca::ListenerSocketOptions listenerSocketOptions;
    listenerSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
    listenerSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::Ipv4Endpoint(ntsa::Ipv4Address::loopback(), 0)));

    d_listenerSocket_sp = 
        d_scheduler_sp->createListenerSocket(
            listenerSocketOptions, d_allocator_p);

    error = d_listenerSocket_sp->registerSession(self);
    BSLS_ASSERT_OPT(!error);

    error = d_listenerSocket_sp->registerManager(self);
    BSLS_ASSERT_OPT(!error);

    error = d_listenerSocket_sp->open();
    BSLS_ASSERT_OPT(!error);

    error = d_listenerSocket_sp->listen();
    BSLS_ASSERT_OPT(!error);

    d_listenerEndpoint = d_listenerSocket_sp->sourceEndpoint();

    BALL_LOG_INFO << "Server listening socket construction complete" 
                  << BALL_LOG_END;

    BALL_LOG_INFO << "Server construction complete" << BALL_LOG_END;

    d_datagramParser_sp.createInplace(d_allocator_p, 
                                      d_dataPool_sp, 
                                      d_messagePool_sp, 
                                      d_serialization_sp, 
                                      d_compression_sp, 
                                      d_allocator_p);

    error = d_datagramSocket_sp->relaxFlowControl(
        ntca::FlowControlType::e_RECEIVE);
    BSLS_ASSERT_OPT(!error);

    error = d_listenerSocket_sp->relaxFlowControl(
        ntca::FlowControlType::e_RECEIVE);
    BSLS_ASSERT_OPT(!error);
}

TestServer::~TestServer()
{
    BALL_LOG_INFO << "Server destruction starting" << BALL_LOG_END;

    StreamSocketMap streamSocketMap(d_allocator_p);
    streamSocketMap.swap(d_streamSocketMap);

    typedef bsl::vector< bsl::shared_ptr<ntci::StreamSocket> > 
    StreamSocketVector;

    StreamSocketVector streamSocketVector(d_allocator_p);
    streamSocketVector.reserve(streamSocketMap.size());

    for (StreamSocketMap::iterator it = streamSocketMap.begin(); 
            it != streamSocketMap.end(); ++it)
    {
        streamSocketVector.push_back(it->first);
    }

    streamSocketMap.clear();

    while (!streamSocketVector.empty()) {
        bsl::shared_ptr<ntci::StreamSocket> streamSocket = 
            streamSocketVector.back();
        streamSocketVector.pop_back();

        BALL_LOG_INFO << "Server stream socket destruction starting" 
                      << BALL_LOG_END;

        {
            ntci::StreamSocketCloseGuard closeGuard(streamSocket);
        }

        streamSocket.reset();

        BALL_LOG_INFO << "Server stream socket destruction complete" 
                      << BALL_LOG_END;
    }

    BALL_LOG_INFO << "Server listening socket destruction starting" 
                  << BALL_LOG_END;

    {
        ntci::ListenerSocketCloseGuard closeGuard(d_listenerSocket_sp);
    }
    
    d_listenerSocket_sp.reset();

    BALL_LOG_INFO << "Server listening socket destruction complete" 
                  << BALL_LOG_END;

    BALL_LOG_INFO << "Server datagram socket destruction starting" 
                  << BALL_LOG_END;

    {
        ntci::DatagramSocketCloseGuard closeGuard(d_datagramSocket_sp);
    }

    d_datagramSocket_sp.reset();

    BALL_LOG_INFO << "Server datagram socket destruction complete" 
                  << BALL_LOG_END;


    BALL_LOG_INFO << "Server scheduler destruction starting" 
                  << BALL_LOG_END;

    d_scheduler_sp->shutdown();
    d_scheduler_sp->linger();
    d_scheduler_sp.reset();

    BALL_LOG_INFO << "Server scheduler destruction complete" 
                  << BALL_LOG_END;

    BALL_LOG_INFO << "Server destruction complete" << BALL_LOG_END;
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
