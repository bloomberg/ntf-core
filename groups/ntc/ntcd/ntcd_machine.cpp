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

#include <ntcd_machine.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcd_machine_cpp, "$Id$ $CSID$")

#include <ntci_log.h>
#include <bdlb_string.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobutil.h>
#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

#define NTCD_SESSION_LOG_OUTGOING_PACKET_QUEUE_ENQUEUE_ERROR(machine,         \
                                                             session,         \
                                                             packet,          \
                                                             error)           \
    do {                                                                      \
        NTCI_LOG_STREAM_TRACE                                                 \
            << "Machine '" << (machine)->name() << "' session " << (session)  \
            << " failed to enqueue packet " << *(packet)                      \
            << " to outgoing packet queue: " << (error).text()                \
            << NTCI_LOG_STREAM_END;                                           \
    } while (false)

#define NTCD_SESSION_LOG_OUTGOING_PACKET_QUEUE_ENQUEUED(machine,              \
                                                        session,              \
                                                        packet)               \
    do {                                                                      \
        NTCI_LOG_STREAM_TRACE                                                 \
            << "Machine '" << (machine)->name() << "' session " << (session)  \
            << " enqueued packet " << *(packet)                               \
            << " to outgoing packet queue" << NTCI_LOG_STREAM_END;            \
    } while (false)

#define NTCD_SESSION_LOG_INCOMING_PACKET_QUEUE_DEQUEUED_UNSUPPORTED(machine,  \
                                                                    session,  \
                                                                    packet)   \
    do {                                                                      \
        NTCI_LOG_STREAM_ERROR << "Machine '" << (machine)->name()             \
                              << "' session " << (session)                    \
                              << " dequeued unsupported packet " << *(packet) \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_SESSION_LOG_STEP_STARTING(machine, session)                      \
    do {                                                                      \
        NTCI_LOG_TRACE("Machine '%s' session %p stepping simulation",         \
                       (machine)->name().c_str(),                             \
                       (session));                                            \
    } while (false)

#define NTCD_SESSION_LOG_STEP_COMPLETE(machine, session)                      \
    do {                                                                      \
        NTCI_LOG_TRACE("Machine '%s' session %p stepped simulation: OK",      \
                       (machine)->name().c_str(),                             \
                       (session));                                            \
    } while (false)

#define NTCD_SESSION_LOG_TRANSFERRING_PACKET(machine, session, packet)        \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG                                                 \
            << "Machine '" << (machine)->name() << "' session " << (session)  \
            << " transferring packet " << *(packet) << NTCI_LOG_STREAM_END;   \
    } while (false)

#define NTCD_SESSION_LOG_TRANSFERRING_PACKET_FAILED_PEER_MISSING(machine,     \
                                                                 session,     \
                                                                 packet)      \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG                                                 \
            << "Machine '" << (machine)->name() << "' session " << (session)  \
            << " failed to transfer packet " << *(packet)                     \
            << ": the remote endpoint " << (packet)->remoteEndpoint()         \
            << " does not exist" << NTCI_LOG_STREAM_END;                      \
    } while (false)

#define NTCD_SESSION_LOG_TRANSFERRING_PACKET_FAILED_PEER_DEAD(machine,        \
                                                              session,        \
                                                              packet)         \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG                                                 \
            << "Machine '" << (machine)->name() << "' session " << (session)  \
            << " failed to transfer packet " << *(packet)                     \
            << ": the remote session is dead" << NTCI_LOG_STREAM_END;         \
    } while (false)

#define NTCD_SESSION_LOG_TRANSFERRING_PACKET_FAILED(machine,                  \
                                                    session,                  \
                                                    packet,                   \
                                                    remoteSession,            \
                                                    error)                    \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG                                                 \
            << "Machine '" << (machine)->name() << "' session " << (session)  \
            << " failed to transfer packet " << *(packet) << " to session "   \
            << (remoteSession).get() << ": " << (error).text()                \
            << NTCI_LOG_STREAM_END;                                           \
    } while (false)

#define NTCD_SESSION_LOG_UPDATE_ENABLE_FAILED(machine,                        \
                                              session,                        \
                                              eventType,                      \
                                              error)                          \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG                                                 \
            << "Machine '" << (machine)->name() << "' session " << (session)  \
            << " failed to enable event "                                     \
            << ntca::ReactorEventType::toString(eventType) << ": "            \
            << (error).text() << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_SESSION_LOG_UPDATE_ENABLE_NOTIFICATIONS_FAILED(machine,          \
                                                            session,          \
                                                            error)            \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG                                                 \
            << "Machine '" << (machine)->name() << "' session " << (session)  \
            << " failed to enable notifications: " << (error).text()          \
            << NTCI_LOG_STREAM_END;                                           \
    } while (false)

#define NTCD_SESSION_LOG_UPDATE_DISABLE_FAILED(machine,                       \
                                               session,                       \
                                               eventType,                     \
                                               error)                         \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG                                                 \
            << "Machine '" << (machine)->name() << "' session " << (session)  \
            << " failed to disable event "                                    \
            << ntca::ReactorEventType::toString(eventType) << ": "            \
            << (error).text() << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_SESSION_LOG_UPDATE_DISABLE_NOTIFICATIONS_FAILED(machine,         \
                                                             session,         \
                                                             error)           \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG                                                 \
            << "Machine '" << (machine)->name() << "' session " << (session)  \
            << " failed to disable event: " << (error).text()                 \
            << NTCI_LOG_STREAM_END;                                           \
    } while (false)

#define NTCD_MACHINE_LOG_STEP_STARTING()                                      \
    do {                                                                      \
        NTCI_LOG_TRACE("Machine '%s' stepping simulation", d_name.c_str());   \
    } while (false)

#define NTCD_MACHINE_LOG_STEP_COMPLETE()                                      \
    do {                                                                      \
        NTCI_LOG_TRACE("Machine '%s' stepped simulation: OK",                 \
                       d_name.c_str());                                       \
    } while (false)

#define NTCD_MACHINE_LOG_STEP_FAILED(error)                                   \
    do {                                                                      \
        NTCI_LOG_TRACE("Machine '%s' failed to step simulation: %s",          \
                       d_name.c_str(),                                        \
                       (error).text().c_str());                               \
    } while (false)

#define NTCD_MONITOR_LOG_SHOW_READABLE(machine, monitor, session)             \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Machine '" << (machine)->name()             \
                              << "' monitor " << (monitor) << " session "     \
                              << (session) << " is interested in readability" \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_MONITOR_LOG_SHOW_WRITABLE(machine, monitor, session)             \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Machine '" << (machine)->name()             \
                              << "' monitor " << (monitor) << " session "     \
                              << (session) << " is interested in writability" \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_MONITOR_LOG_SHOW_ERROR(machine, monitor, session)                \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Machine '" << (machine)->name()             \
                              << "' monitor " << (monitor) << " session "     \
                              << (session) << " is interested in errors"      \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_MONITOR_LOG_SHOW_NOTIFICATIONS(machine, monitor, session)        \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG                                                 \
            << "Machine '" << (machine)->name() << "' monitor " << (monitor)  \
            << " session " << (session) << " is interested in notifications"  \
            << NTCI_LOG_STREAM_END;                                           \
    } while (false)

#define NTCD_MONITOR_LOG_HIDE_READABLE(machine, monitor, session)             \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG                                                 \
            << "Machine '" << (machine)->name() << "' monitor " << (monitor)  \
            << " session " << (session)                                       \
            << " is not interested in readability" << NTCI_LOG_STREAM_END;    \
    } while (false)

#define NTCD_MONITOR_LOG_HIDE_WRITABLE(machine, monitor, session)             \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG                                                 \
            << "Machine '" << (machine)->name() << "' monitor " << (monitor)  \
            << " session " << (session)                                       \
            << " is not interested in writability" << NTCI_LOG_STREAM_END;    \
    } while (false)

#define NTCD_MONITOR_LOG_HIDE_ERROR(machine, monitor, session)                \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Machine '" << (machine)->name()             \
                              << "' monitor " << (monitor) << " session "     \
                              << (session) << " is not interested in errors"  \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_MONITOR_LOG_HIDE_NOTIFICATIONS(machine, monitor, session)        \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG                                                 \
            << "Machine '" << (machine)->name() << "' monitor " << (monitor)  \
            << " session " << (session)                                       \
            << " is not interested in notifications" << NTCI_LOG_STREAM_END;  \
    } while (false)

#define NTCD_MONITOR_LOG_ENABLE_READABLE(machine, monitor, session)           \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Machine '" << (machine)->name()             \
                              << "' monitor " << (monitor) << " session "     \
                              << (session) << " is readable"                  \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_MONITOR_LOG_ENABLE_WRITABLE(machine, monitor, session)           \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Machine '" << (machine)->name()             \
                              << "' monitor " << (monitor) << " session "     \
                              << (session) << " is writable"                  \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_MONITOR_LOG_ENABLE_ERROR(machine, monitor, session)              \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Machine '" << (machine)->name()             \
                              << "' monitor " << (monitor) << " session "     \
                              << (session) << " has failed"                   \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_MONITOR_LOG_ENABLE_NOTIFICATIONS(machine, monitor, session)      \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Machine '" << (machine)->name()             \
                              << "' monitor " << (monitor) << " session "     \
                              << (session) << " has notifications"            \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_MONITOR_LOG_DISABLE_READABLE(machine, monitor, session)          \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Machine '" << (machine)->name()             \
                              << "' monitor " << (monitor) << " session "     \
                              << (session) << " is not readable"              \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_MONITOR_LOG_DISABLE_WRITABLE(machine, monitor, session)          \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Machine '" << (machine)->name()             \
                              << "' monitor " << (monitor) << " session "     \
                              << (session) << " is not writable"              \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_MONITOR_LOG_DISABLE_ERROR(machine, monitor, session)             \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Machine '" << (machine)->name()             \
                              << "' monitor " << (monitor) << " session "     \
                              << (session) << " is operational"               \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_MONITOR_LOG_EVENT(machine, monitor, session, event)              \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Machine '" << (machine)->name()             \
                              << "' monitor " << (monitor) << " session "     \
                              << (session) << " polled event " << event       \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_MONITOR_LOG_READY(machine, monitor)                              \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Machine '" << (machine)->name()             \
                              << "' monitor " << (monitor) << " is ready"     \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_MONITOR_LOG_IDLE(machine, monitor)                               \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Machine '" << (machine)->name()             \
                              << "' monitor " << (monitor) << " is idle"      \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_MONITOR_LOG_WAITING(machine, monitor)                            \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG                                                 \
            << "Machine '" << (machine)->name() << "' monitor " << (monitor)  \
            << " is waiting to become ready" << NTCI_LOG_STREAM_END;          \
    } while (false)

#define NTCD_MONITOR_LOG_PROCESS_STARTING(machine, monitor, queueSize)        \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG                                                 \
            << "Machine '" << (machine)->name() << "' monitor " << (monitor)  \
            << " processing events (queue size = " << (queueSize) << ")"      \
            << NTCI_LOG_STREAM_END;                                           \
    } while (false)

#define NTCD_MONITOR_LOG_PROCESS_COMPLETE(machine, monitor, numEvents)        \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Machine '" << (machine)->name()             \
                              << "' monitor " << (monitor) << " processed "   \
                              << numEvents << " events"                       \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCD_MONITOR_LOG_INTERRUPTION(machine, monitor)                       \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Machine '" << (machine)->name()             \
                              << "' monitor " << (monitor) << " interrupted"  \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

namespace BloombergLP {
namespace ntcd {

namespace {

const bool        k_DEFAULT_REUSE_ADDRESS                = false;
const bool        k_DEFAULT_KEEP_ALIVE                   = false;
const bool        k_DEFAULT_CORK                         = false;
const bool        k_DEFAULT_DELAY_TRANSMISSION           = false;
const bool        k_DEFAULT_DELAY_ACKNOWLEDGEMENT        = false;
const bsl::size_t k_DEFAULT_SEND_BUFFER_SIZE             = 1024 * 256;
const bsl::size_t k_DEFAULT_SEND_BUFFER_LOW_WATERMARK    = 1;
const bsl::size_t k_DEFAULT_RECEIVE_BUFFER_SIZE          = 1024 * 256;
const bsl::size_t k_DEFAULT_RECEIVE_BUFFER_LOW_WATERMARK = 1;
const bsl::size_t k_DEFAULT_BACKLOG                      = 4096;
const bool        k_DEFAULT_DEBUG                        = false;
const bool        k_DEFAULT_LINGER_ENABLED               = true;
const bsl::size_t k_DEFAULT_LINGER_DURATION_IN_SECONDS   = 10;
const bool        k_DEFAULT_BROADCAST                    = false;
const bool        k_DEFAULT_BYPASS_ROUTING               = false;
const bool        k_DEFAULT_INLINE_OUT_OF_BAND_DATA      = false;

const bsl::size_t k_MAX_BUFFERS_PER_SEND    = 64;
const bsl::size_t k_MAX_BUFFERS_PER_RECEIVE = 64;

const ntsa::Handle k_MIN_HANDLE = 3;
const ntsa::Handle k_MAX_HANDLE = 32767;

const bsl::size_t k_DEFAULT_LOW_WATERMARK  = 1;
const bsl::size_t k_DEFAULT_HIGH_WATERMARK = 1024 * 256;

const int k_DEFAULT_BLOB_BUFFER_SIZE = 4096;

const bsl::size_t k_MTU = 64 * 1024;

bsls::SpinLock       s_defaultMachineLock  = BSLS_SPINLOCK_UNLOCKED;
ntcd::Machine*       s_defaultMachine_p    = 0;
bslma::SharedPtrRep* s_defaultMachineRep_p = 0;

/// For the specified 'packet' generate and set id based on the specified
/// 'tsKey' and generate ntsa::Timestamp with type e_SCHEDULED, then put it
/// into the specified 'errorQueue'. Use the specified 'idIncrement' to
/// modify the specified 'tsKey'.
void generateTransmitTimestampScheduled(
    ntcd::Packet*                                   packet,
    bsl::uint32_t*                                  tsKey,
    bsl::shared_ptr<bsl::list<ntsa::Notification> > errorQueue,
    bsl::uint32_t                                   idIncrement)
{
    *tsKey                       += idIncrement;
    const bsl::uint32_t packetId  = *tsKey - 1;
    packet->setId(packetId);
    ntsa::Notification n;
    ntsa::Timestamp&   t = n.makeTimestamp();
    t.setType(ntsa::TimestampType::e_SCHEDULED);
    t.setTime(bdlt::CurrentTime::now());
    t.setId(packetId);
    errorQueue->push_back(n);
}

/// For the specified 'packet' set current time as a timestamp.
void generateReceiveTimestamp(ntcd::Packet* packet)
{
    packet->setRxTimestamp(bdlt::CurrentTime::now());
}

}  // close unnamed namespace

int PacketType::fromInt(PacketType::Value* result, int number)
{
    switch (number) {
    case PacketType::e_UNDEFINED:
    case PacketType::e_CONNECT:
    case PacketType::e_PUSH:
    case PacketType::e_SHUTDOWN:
    case PacketType::e_RESET:
    case PacketType::e_ERROR:
        *result = static_cast<PacketType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int PacketType::fromString(PacketType::Value*       result,
                           const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CONNECT")) {
        *result = e_CONNECT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "PUSH")) {
        *result = e_PUSH;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SHUTDOWN")) {
        *result = e_SHUTDOWN;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "RESET")) {
        *result = e_RESET;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ERROR")) {
        *result = e_ERROR;
        return 0;
    }

    return -1;
}

const char* PacketType::toString(PacketType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_CONNECT: {
        return "CONNECT";
    } break;
    case e_PUSH: {
        return "PUSH";
    } break;
    case e_SHUTDOWN: {
        return "SHUTDOWN";
    } break;
    case e_RESET: {
        return "RESET";
    } break;
    case e_ERROR: {
        return "ERROR";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& PacketType::print(bsl::ostream& stream, PacketType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, PacketType::Value rhs)
{
    return PacketType::print(stream, rhs);
}

Packet::Packet(bdlbb::BlobBufferFactory* blobBufferFactory,
               bslma::Allocator*         basicAllocator)
: d_type(ntcd::PacketType::e_UNDEFINED)
, d_transport(ntsa::Transport::e_UNDEFINED)
, d_sourceEndpoint()
, d_remoteEndpoint()
, d_sourceSession_wp()
, d_remoteSession_wp()
, d_data(blobBufferFactory, basicAllocator)
, d_rxTimestamp()
, d_blobBufferFactory_p(blobBufferFactory)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Packet::~Packet()
{
}

void Packet::setType(ntcd::PacketType::Value type)
{
    d_type = type;
}

void Packet::setTransport(ntsa::Transport::Value transport)
{
    d_transport = transport;
}

void Packet::setSourceEndpoint(const ntsa::Endpoint& sourceEndpoint)
{
    d_sourceEndpoint = sourceEndpoint;
}

void Packet::setRemoteEndpoint(const ntsa::Endpoint& remoteEndpoint)
{
    d_remoteEndpoint = remoteEndpoint;
}

void Packet::setSourceSession(
    const bsl::weak_ptr<ntcd::Session>& sourceSession)
{
    d_sourceSession_wp = sourceSession;
}

void Packet::setRemoteSession(
    const bsl::weak_ptr<ntcd::Session>& remoteSession)
{
    d_remoteSession_wp = remoteSession;
}

void Packet::setData(const bdlbb::Blob& data)
{
    BSLS_ASSERT_OPT(data.length() > 0);

    d_data = data;
}

void Packet::setRxTimestamp(const bsls::TimeInterval& timestamp)
{
    BSLS_ASSERT(d_rxTimestamp.isNull());
    d_rxTimestamp = timestamp;
}

void Packet::setId(bsl::uint32_t id)
{
    BSLS_ASSERT(d_id.isNull());
    d_id = id;
}

ntsa::Error Packet::enqueueData(ntsa::SendContext*       context,
                                const bdlbb::Blob&       data,
                                const ntsa::SendOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    context->reset();

    ntsa::TransportMode::Value transportMode =
        ntsa::Transport::getMode(d_transport);

    bsl::size_t numBytesToSend =
        NTCCFG_WARNING_PROMOTE(bsl::size_t, data.length());

    if (numBytesToSend == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (transportMode == ntsa::TransportMode::e_DATAGRAM) {
        if (numBytesToSend > k_MTU) {
            numBytesToSend = k_MTU;
        }
    }
    else if (transportMode == ntsa::TransportMode::e_STREAM) {
        if (numBytesToSend > k_MTU) {
            numBytesToSend = k_MTU;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    context->setBytesSendable(numBytesToSend);

    bsl::size_t position = d_data.length();
    d_data.setLength(NTCCFG_WARNING_NARROW(int, position + numBytesToSend));

    bdlbb::BlobUtil::copy(&d_data,
                          NTCCFG_WARNING_NARROW(int, position),
                          data,
                          0,
                          NTCCFG_WARNING_NARROW(int, numBytesToSend));

    context->setBytesSent(numBytesToSend);

    return ntsa::Error();
}

ntsa::Error Packet::enqueueData(ntsa::SendContext*       context,
                                const ntsa::Data&        data,
                                const ntsa::SendOptions& options)
{
    if (data.isBlob()) {
        return this->enqueueData(context, data.blob(), options);
    }
    else {
        context->reset();

        if (data.size() == 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        bdlbb::Blob blob(d_blobBufferFactory_p);
        ntsa::DataUtil::append(&blob, data);

        return this->enqueueData(context, blob, options);
    }
}

ntsa::Error Packet::dequeueData(ntsa::ReceiveContext*       context,
                                bdlbb::Blob*                data,
                                const ntsa::ReceiveOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    context->reset();

    bsl::size_t numBytesReceivable = data->totalSize() - data->length();

    if (numBytesReceivable == 0) {
        data->setLength(d_data.length());
        data->setLength(0);
        numBytesReceivable = data->totalSize() - data->length();
    }

    if (numBytesReceivable == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    context->setBytesReceivable(numBytesReceivable);

    bsl::size_t numBytesToCopy = d_data.length();
    if (numBytesToCopy > numBytesReceivable) {
        numBytesToCopy = numBytesReceivable;
    }

    bsl::size_t position = data->length();
    data->setLength(
        NTCCFG_WARNING_NARROW(int, data->length() + numBytesToCopy));

    bdlbb::BlobUtil::copy(data,
                          NTCCFG_WARNING_NARROW(int, position),
                          d_data,
                          0,
                          NTCCFG_WARNING_NARROW(int, numBytesToCopy));

    bdlbb::BlobUtil::erase(&d_data,
                           0,
                           NTCCFG_WARNING_NARROW(int, numBytesToCopy));

    context->setEndpoint(d_sourceEndpoint);
    context->setBytesReceived(numBytesToCopy);

    return ntsa::Error();
}

ntsa::Error Packet::dequeueData(ntsa::ReceiveContext*       context,
                                bdlbb::BlobBuffer*          data,
                                const ntsa::ReceiveOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    context->reset();

    bsl::size_t numBytesReceivable = data->size();
    if (numBytesReceivable == 0) {
        data->reset(bslstl::SharedPtrUtil::createInplaceUninitializedBuffer(
                        d_data.length(),
                        d_allocator_p),
                    d_data.length());
        numBytesReceivable = d_data.length();
    }

    context->setBytesReceivable(numBytesReceivable);

    bsl::size_t numBytesToCopy = d_data.length();
    if (numBytesToCopy > numBytesReceivable) {
        numBytesToCopy = numBytesReceivable;
    }

    bdlbb::BlobUtil::copy(data->data(),
                          d_data,
                          0,
                          NTCCFG_WARNING_NARROW(int, numBytesToCopy));

    bdlbb::BlobUtil::erase(&d_data,
                           0,
                           NTCCFG_WARNING_NARROW(int, numBytesToCopy));

    context->setEndpoint(d_sourceEndpoint);
    context->setBytesReceived(numBytesToCopy);

    return ntsa::Error();
}

ntsa::Error Packet::dequeueData(ntsa::ReceiveContext*       context,
                                ntsa::MutableBuffer*        data,
                                const ntsa::ReceiveOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    context->reset();

    bsl::size_t numBytesReceivable = data->size();
    if (numBytesReceivable == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::size_t numBytesToCopy = d_data.length();
    if (numBytesToCopy > numBytesReceivable) {
        numBytesToCopy = numBytesReceivable;
    }

    context->setBytesReceivable(numBytesReceivable);

    bdlbb::BlobUtil::copy(reinterpret_cast<char*>(data->buffer()),
                          d_data,
                          0,
                          NTCCFG_WARNING_NARROW(int, numBytesToCopy));

    bdlbb::BlobUtil::erase(&d_data,
                           0,
                           NTCCFG_WARNING_NARROW(int, numBytesToCopy));

    context->setEndpoint(d_sourceEndpoint);
    context->setBytesReceived(numBytesToCopy);

    return ntsa::Error();
}

ntsa::Error Packet::dequeueData(ntsa::ReceiveContext*       context,
                                ntsa::MutableBufferArray*   data,
                                const ntsa::ReceiveOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    context->reset();

    bsl::size_t numBytesReceivable = data->numBytes();
    bsl::size_t numBytesReceived   = 0;

    if (numBytesReceivable == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    context->setBytesReceivable(numBytesReceivable);

    bsl::size_t numBuffersReceivable = data->numBuffers();
    // bsl::size_t numBuffersReceived   = 0;

    for (bsl::size_t i = 0; i < numBuffersReceivable; ++i) {
        if (d_data.length() == 0) {
            break;
        }

        ntsa::MutableBuffer* mutableBuffer = &data->buffer(i);

        bsl::size_t numBytesToCopy = d_data.length();
        if (numBytesToCopy > mutableBuffer->size()) {
            numBytesToCopy = mutableBuffer->size();
        }

        bdlbb::BlobUtil::copy(reinterpret_cast<char*>(mutableBuffer->buffer()),
                              d_data,
                              0,
                              NTCCFG_WARNING_NARROW(int, numBytesToCopy));

        bdlbb::BlobUtil::erase(&d_data,
                               0,
                               NTCCFG_WARNING_NARROW(int, numBytesToCopy));

        numBytesReceived += numBytesToCopy;
        // numBuffersReceived += 1;
    }

    context->setEndpoint(d_sourceEndpoint);
    context->setBytesReceived(numBytesReceived);

    return ntsa::Error();
}

ntsa::Error Packet::dequeueData(ntsa::ReceiveContext*        context,
                                ntsa::MutableBufferPtrArray* data,
                                const ntsa::ReceiveOptions&  options)
{
    NTCCFG_WARNING_UNUSED(options);

    context->reset();

    bsl::size_t numBytesReceivable = data->numBytes();
    bsl::size_t numBytesReceived   = 0;

    if (numBytesReceivable == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    context->setBytesReceivable(numBytesReceivable);

    bsl::size_t numBuffersReceivable = data->numBuffers();
    // bsl::size_t numBuffersReceived   = 0;

    for (bsl::size_t i = 0; i < numBuffersReceivable; ++i) {
        if (d_data.length() == 0) {
            break;
        }

        ntsa::MutableBuffer* mutableBuffer = &data->buffer(i);

        bsl::size_t numBytesToCopy = d_data.length();
        if (numBytesToCopy > mutableBuffer->size()) {
            numBytesToCopy = mutableBuffer->size();
        }

        bdlbb::BlobUtil::copy(reinterpret_cast<char*>(mutableBuffer->buffer()),
                              d_data,
                              0,
                              NTCCFG_WARNING_NARROW(int, numBytesToCopy));

        bdlbb::BlobUtil::erase(&d_data,
                               0,
                               NTCCFG_WARNING_NARROW(int, numBytesToCopy));

        numBytesReceived += numBytesToCopy;
        // numBuffersReceived += 1;
    }

    context->setEndpoint(d_sourceEndpoint);
    context->setBytesReceived(numBytesReceived);

    return ntsa::Error();
}

ntsa::Error Packet::dequeueData(ntsa::ReceiveContext*       context,
                                bsl::string*                data,
                                const ntsa::ReceiveOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    context->reset();

    bsl::size_t position = data->size();

    bsl::size_t numBytesReceivable = data->capacity() - data->size();
    if (numBytesReceivable == 0) {
        data->resize(d_data.length());
        numBytesReceivable = d_data.length();
    }

    context->setBytesReceivable(numBytesReceivable);

    bsl::size_t numBytesToCopy = d_data.length();
    if (numBytesToCopy > numBytesReceivable) {
        numBytesToCopy = numBytesReceivable;
    }

    bdlbb::BlobUtil::copy(data->data() + position,
                          d_data,
                          0,
                          NTCCFG_WARNING_NARROW(int, numBytesToCopy));

    bdlbb::BlobUtil::erase(&d_data,
                           0,
                           NTCCFG_WARNING_NARROW(int, numBytesToCopy));

    data->resize(position + numBytesToCopy);

    context->setEndpoint(d_sourceEndpoint);
    context->setBytesReceived(numBytesToCopy);

    return ntsa::Error();
}

ntsa::Error Packet::dequeueData(ntsa::ReceiveContext*       context,
                                ntsa::Data*                 data,
                                const ntsa::ReceiveOptions& options)
{
    context->reset();

    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(data);
    NTCCFG_WARNING_UNUSED(options);

    if (data->isBlob()) {
        return this->dequeueData(context, &data->blob(), options);
    }
    else if (data->isSharedBlob()) {
        return this->dequeueData(context, data->sharedBlob().get(), options);
    }
    else if (data->isBlobBuffer()) {
        return this->dequeueData(context, &data->blobBuffer(), options);
    }
    else if (data->isMutableBuffer()) {
        return this->dequeueData(context, &data->mutableBuffer(), options);
    }
    else if (data->isMutableBufferArray()) {
        return this->dequeueData(context,
                                 &data->mutableBufferArray(),
                                 options);
    }
    else if (data->isMutableBufferPtrArray()) {
        return this->dequeueData(context,
                                 &data->mutableBufferPtrArray(),
                                 options);
    }
    else if (data->isString()) {
        return this->dequeueData(context, &data->string(), options);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntcd::PacketType::Value Packet::type() const
{
    return d_type;
}

ntsa::Transport::Value Packet::transport() const
{
    return d_transport;
}

const ntsa::Endpoint& Packet::sourceEndpoint() const
{
    return d_sourceEndpoint;
}

const ntsa::Endpoint& Packet::remoteEndpoint() const
{
    return d_remoteEndpoint;
}

const bsl::weak_ptr<ntcd::Session>& Packet::sourceSession() const
{
    return d_sourceSession_wp;
}

const bsl::weak_ptr<ntcd::Session>& Packet::remoteSession() const
{
    return d_remoteSession_wp;
}

const bdlbb::Blob& Packet::data() const
{
    return d_data;
}

const bdlb::NullableValue<bsls::TimeInterval>& Packet::rxTimestamp() const
{
    return d_rxTimestamp;
}

const bdlb::NullableValue<bsl::uint32_t>& Packet::id() const
{
    return d_id;
}

bsl::size_t Packet::length() const
{
    return d_data.length();
}

bsl::size_t Packet::cost() const
{
    if (d_type == ntcd::PacketType::e_PUSH) {
        return d_data.length();
    }
    else if (d_type == ntcd::PacketType::e_CONNECT) {
        return 1;
    }
    else {
        return 1;
    }
}

bslma::Allocator* Packet::allocator() const
{
    return d_allocator_p;
}

bool Packet::equals(const Packet& other) const
{
    if (d_sourceEndpoint != other.d_sourceEndpoint) {
        return false;
    }

    if (d_remoteEndpoint != other.d_remoteEndpoint) {
        return false;
    }

    if (bdlbb::BlobUtil::compare(d_data, other.d_data) != 0) {
        return false;
    }

    return true;
}

bool Packet::less(const Packet& other) const
{
    if (d_sourceEndpoint < other.d_sourceEndpoint) {
        return true;
    }

    if (other.d_sourceEndpoint < d_sourceEndpoint) {
        return false;
    }

    if (d_remoteEndpoint < other.d_remoteEndpoint) {
        return true;
    }

    if (other.d_remoteEndpoint < d_remoteEndpoint) {
        return false;
    }

    if (bdlbb::BlobUtil::compare(d_data, other.d_data) < 0) {
        return true;
    }

    return false;
}

bsl::ostream& Packet::print(bsl::ostream& stream,
                            int           level,
                            int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("type", d_type);
    printer.printAttribute("transport", d_transport);
    printer.printAttribute("sourceEndpoint", d_sourceEndpoint);
    printer.printAttribute("remoteEndpoint", d_remoteEndpoint);
    printer.printAttribute("length", this->length());
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const Packet& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const Packet& lhs, const Packet& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const Packet& lhs, const Packet& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const Packet& lhs, const Packet& rhs)
{
    return lhs.less(rhs);
}

PacketQueue::PacketQueue(bslma::Allocator* basicAllocator)
: d_allowDequeue()
, d_allowEnqueue()
, d_storage(basicAllocator)
, d_currentWatermark(0)
, d_lowWatermark(k_DEFAULT_LOW_WATERMARK)
, d_highWatermark(k_DEFAULT_HIGH_WATERMARK)
, d_shutdown(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

PacketQueue::~PacketQueue()
{
}

ntsa::Error PacketQueue::setLowWatermark(bsl::size_t lowWatermark)
{
    if (lowWatermark == 0) {
        lowWatermark = 1;
    }

    d_lowWatermark = lowWatermark;
    d_allowDequeue.broadcast();

    return ntsa::Error();
}

ntsa::Error PacketQueue::setHighWatermark(bsl::size_t highWatermark)
{
    if (highWatermark == 0) {
        highWatermark = 1;
    }

    d_highWatermark = highWatermark;
    d_allowEnqueue.broadcast();

    return ntsa::Error();
}

ntsa::Error PacketQueue::enqueue(ntccfg::ConditionMutex*        mutex,
                                 bsl::shared_ptr<ntcd::Packet>& packet,
                                 bool                           block,
                                 PacketFunctor                  packetFunctor)
{
    while (true) {
        if (block) {
            while (!d_shutdown && d_currentWatermark >= d_highWatermark) {
                d_allowEnqueue.wait(mutex);
            }
        }
        else {
            if (d_currentWatermark >= d_highWatermark) {
                return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
            }
        }

        if (d_shutdown) {
            return ntsa::Error(ntsa::Error::e_CONNECTION_DEAD);
        }
        else if (d_currentWatermark < d_highWatermark) {
            bsl::size_t oldWatermark =
                NTCCFG_WARNING_NARROW(bsl::size_t, d_currentWatermark);
            bsl::size_t newWatermark = oldWatermark + packet->cost();

            d_storage.push_back(packet);
            if (packetFunctor) {
                packetFunctor(packet.get());
            }
            d_currentWatermark = newWatermark;

            if (oldWatermark < d_lowWatermark &&
                newWatermark >= d_lowWatermark)
            {
                d_allowDequeue.signal();
            }

            break;
        }
        else {
            continue;
        }
    }

    return ntsa::Error();
}

void PacketQueue::retry(const bsl::shared_ptr<ntcd::Packet>& packet)
{
    bsl::size_t oldWatermark =
        NTCCFG_WARNING_NARROW(bsl::size_t, d_currentWatermark);
    bsl::size_t newWatermark = oldWatermark + packet->cost();

    d_storage.push_front(packet);
    d_currentWatermark = newWatermark;

    if (oldWatermark < d_lowWatermark && newWatermark >= d_lowWatermark) {
        d_allowDequeue.signal();
    }
}

void PacketQueue::retry(const PacketVector& packetVector)
{
    bsl::size_t totalPacketCost = 0;

    for (PacketVector::const_iterator it = packetVector.begin();
         it != packetVector.end();
         ++it)
    {
        const bsl::shared_ptr<ntcd::Packet>& packet  = *it;
        totalPacketCost                             += packet->cost();
    }

    bsl::size_t oldWatermark =
        NTCCFG_WARNING_NARROW(bsl::size_t, d_currentWatermark);
    bsl::size_t newWatermark = oldWatermark + totalPacketCost;

    for (PacketVector::const_reverse_iterator it = packetVector.rbegin();
         it != packetVector.rend();
         ++it)
    {
        const bsl::shared_ptr<ntcd::Packet>& packet = *it;
        d_storage.push_front(packet);
    }

    d_currentWatermark = newWatermark;

    if (oldWatermark < d_lowWatermark && newWatermark >= d_lowWatermark) {
        d_allowDequeue.signal();
    }
}

ntsa::Error PacketQueue::dequeue(ntccfg::ConditionMutex*        mutex,
                                 bsl::shared_ptr<ntcd::Packet>* result,
                                 bool                           block)
{
    while (true) {
        if (block) {
            while (!d_shutdown && d_currentWatermark < d_lowWatermark) {
                d_allowDequeue.wait(mutex);
            }
        }
        else {
            if (d_currentWatermark < d_lowWatermark) {
                return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
            }
        }

        if (d_currentWatermark >= d_lowWatermark) {
            bsl::shared_ptr<ntcd::Packet> packet = d_storage.front();
            d_storage.pop_front();

            bsl::size_t oldWatermark =
                NTCCFG_WARNING_NARROW(bsl::size_t, d_currentWatermark);
            bsl::size_t newWatermark = oldWatermark - packet->cost();

            if (result) {
                *result = packet;
            }

            d_currentWatermark = newWatermark;

            if (newWatermark < d_highWatermark) {
                d_allowEnqueue.signal();
            }

            break;
        }
        else if (d_shutdown) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }
        else {
            continue;
        }
    }

    return ntsa::Error();
}

ntsa::Error PacketQueue::peek(ntccfg::ConditionMutex*        mutex,
                              bsl::shared_ptr<ntcd::Packet>* result,
                              bool                           block)
{
    while (true) {
        if (block) {
            while (!d_shutdown && d_currentWatermark < d_lowWatermark) {
                d_allowDequeue.wait(mutex);
            }
        }
        else {
            if (d_currentWatermark < d_lowWatermark) {
                return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
            }
        }

        if (d_currentWatermark >= d_lowWatermark) {
            if (result) {
                *result = d_storage.front();
            }
            break;
        }
        else if (d_shutdown) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }
        else {
            continue;
        }
    }

    return ntsa::Error();
}

ntsa::Error PacketQueue::pop(ntccfg::ConditionMutex* mutex, bool block)
{
    return this->dequeue(mutex, 0, block);
}

ntsa::Error PacketQueue::wakeup()
{
    d_allowEnqueue.broadcast();
    d_allowDequeue.broadcast();

    return ntsa::Error();
}

ntsa::Error PacketQueue::shutdown()
{
    if (d_shutdown.swap(true) == false) {
        d_allowEnqueue.broadcast();
        d_allowDequeue.broadcast();
    }

    return ntsa::Error();
}

bool PacketQueue::empty() const
{
    return d_currentWatermark == 0;
}

bsl::size_t PacketQueue::totalSize() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_currentWatermark);
}

bsl::size_t PacketQueue::lowWatermark() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_lowWatermark);
}

bsl::size_t PacketQueue::highWatermark() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_highWatermark);
}

Binding::Binding()
: d_sourceEndpoint()
, d_remoteEndpoint()
{
}

Binding::Binding(const ntsa::Endpoint& sourceEndpoint,
                 const ntsa::Endpoint& remoteEndpoint)
: d_sourceEndpoint(sourceEndpoint)
, d_remoteEndpoint(remoteEndpoint)
{
}

Binding::Binding(const Binding& original)
: d_sourceEndpoint(original.d_sourceEndpoint)
, d_remoteEndpoint(original.d_remoteEndpoint)
{
}

Binding::~Binding()
{
}

Binding& Binding::operator=(const Binding& other)
{
    if (this != &other) {
        d_sourceEndpoint = other.d_sourceEndpoint;
        d_remoteEndpoint = other.d_remoteEndpoint;
    }

    return *this;
}

void Binding::reset()
{
    d_sourceEndpoint.reset();
    d_remoteEndpoint.reset();
}

void Binding::setSourceEndpoint(const ntsa::Endpoint& sourceEndpoint)
{
    d_sourceEndpoint = sourceEndpoint;
}

void Binding::setRemoteEndpoint(const ntsa::Endpoint& remoteEndpoint)
{
    d_remoteEndpoint = remoteEndpoint;
}

const ntsa::Endpoint& Binding::sourceEndpoint() const
{
    return d_sourceEndpoint;
}

const ntsa::Endpoint& Binding::remoteEndpoint() const
{
    return d_remoteEndpoint;
}

bool Binding::equals(const Binding& other) const
{
    return d_sourceEndpoint == other.d_sourceEndpoint &&
           d_remoteEndpoint == other.d_remoteEndpoint;
}

bool Binding::less(const Binding& other) const
{
    if (d_sourceEndpoint < other.d_sourceEndpoint) {
        return true;
    }

    if (other.d_sourceEndpoint < d_sourceEndpoint) {
        return false;
    }

    return d_remoteEndpoint < other.d_remoteEndpoint;
}

ntsa::Endpoint Binding::makeAny(ntsa::Transport::Value transport)
{
    ntsa::Endpoint result;
    if (transport == ntsa::Transport::e_TCP_IPV4_STREAM ||
        transport == ntsa::Transport::e_UDP_IPV4_DATAGRAM)
    {
        result = ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::any(), 0));
    }
    else if (transport == ntsa::Transport::e_TCP_IPV6_STREAM ||
             transport == ntsa::Transport::e_UDP_IPV6_DATAGRAM)
    {
        result = ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv6Address::any(), 0));
    }
    else if (transport == ntsa::Transport::e_LOCAL_DATAGRAM ||
             transport == ntsa::Transport::e_LOCAL_STREAM)
    {
        ntsa::LocalName   localName;
        const ntsa::Error error = ntsa::LocalName::generateUnique(&localName);
        BSLS_ASSERT_OPT(!error);
        result = ntsa::Endpoint(localName);
    }

    return result;
}

bool operator==(const Binding& lhs, const Binding& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const Binding& lhs, const Binding& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const Binding& lhs, const Binding& rhs)
{
    return lhs.less(rhs);
}

PortMap::PortMap(bslma::Allocator* basicAllocator)
: d_mutex()
, d_bitset()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_bitset.reset();
    d_bitset.set(0);
}

PortMap::~PortMap()
{
}

ntsa::Error PortMap::acquire(ntsa::Port* result, ntsa::Port requested)
{
    LockGuard lock(&d_mutex);

    *result = 0;

    if (requested != 0) {
        if (d_bitset.test(requested)) {
            return ntsa::Error(ntsa::Error::e_ADDRESS_IN_USE);
        }

        *result = requested;
    }
    else {
        bool found = false;

        for (ntsa::Port ephemeralPort = k_MIN_EPHEMERAL_PORT;
             ephemeralPort < k_MAX_EPHEMERAL_PORT;
             ++ephemeralPort)
        {
            if (!d_bitset.test(ephemeralPort)) {
                d_bitset.set(ephemeralPort);
                found   = true;
                *result = ephemeralPort;
                break;
            }
        }

        if (!found) {
            return ntsa::Error(ntsa::Error::e_ADDRESS_IN_USE);
        }
    }

    BSLS_ASSERT(*result != 0);
    return ntsa::Error();
}

void PortMap::release(ntsa::Port port)
{
    LockGuard lock(&d_mutex);

    d_bitset.reset(port);
}

bool PortMap::isUsed(ntsa::Port port)
{
    LockGuard lock(&d_mutex);

    return d_bitset.test(port);
}

bool PortMap::isFree(ntsa::Port port)
{
    LockGuard lock(&d_mutex);

    return !d_bitset.test(port);
}

SessionQueue::SessionQueue(bslma::Allocator* basicAllocator)
: d_allowDequeue()
, d_allowEnqueue()
, d_storage(basicAllocator)
, d_currentWatermark(0)
, d_lowWatermark(1)
, d_highWatermark(2048)
, d_shutdownSend(false)
, d_shutdownReceive(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

SessionQueue::~SessionQueue()
{
}

ntsa::Error SessionQueue::setLowWatermark(bsl::size_t lowWatermark)
{
    if (lowWatermark == 0) {
        lowWatermark = 1;
    }

    d_lowWatermark = lowWatermark;
    d_allowDequeue.broadcast();

    return ntsa::Error();
}

ntsa::Error SessionQueue::setHighWatermark(bsl::size_t highWatermark)
{
    if (highWatermark == 0) {
        highWatermark = 1;
    }

    d_highWatermark = highWatermark;
    d_allowEnqueue.broadcast();

    return ntsa::Error();
}

ntsa::Error SessionQueue::enqueueSession(
    ntccfg::ConditionMutex*               mutex,
    const bsl::shared_ptr<ntcd::Session>& session,
    bool                                  block)
{
    while (true) {
        if (block) {
            while (!d_shutdownSend && d_currentWatermark >= d_highWatermark) {
                d_allowEnqueue.wait(mutex);
            }
        }
        else {
            if (d_currentWatermark >= d_highWatermark) {
                return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
            }
        }

        if (d_shutdownSend) {
            return ntsa::Error(ntsa::Error::e_CONNECTION_DEAD);
        }
        else if (d_currentWatermark < d_highWatermark) {
            bsl::size_t oldWatermark =
                NTCCFG_WARNING_NARROW(bsl::size_t, d_currentWatermark);
            bsl::size_t newWatermark = oldWatermark + 1;

            d_storage.push_back(session);
            d_currentWatermark = newWatermark;

            if (oldWatermark < d_lowWatermark &&
                newWatermark >= d_lowWatermark)
            {
                d_allowDequeue.signal();
            }

            break;
        }
        else {
            continue;
        }
    }

    return ntsa::Error();
}

ntsa::Error SessionQueue::dequeueSession(
    ntccfg::ConditionMutex*         mutex,
    bsl::shared_ptr<ntcd::Session>* result,
    bool                            block)
{
    while (true) {
        if (block) {
            while (!d_shutdownReceive && d_currentWatermark < d_lowWatermark) {
                d_allowDequeue.wait(mutex);
            }
        }
        else {
            if (d_currentWatermark < d_lowWatermark) {
                return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
            }
        }

        if (d_currentWatermark >= d_lowWatermark) {
            bsl::shared_ptr<ntcd::Session> session = d_storage.front();
            d_storage.pop_front();

            bsl::size_t oldWatermark =
                NTCCFG_WARNING_NARROW(bsl::size_t, d_currentWatermark);
            bsl::size_t newWatermark = oldWatermark - 1;

            *result            = session;
            d_currentWatermark = newWatermark;

            if (newWatermark < d_highWatermark) {
                d_allowEnqueue.signal();
            }

            break;
        }
        else if (d_shutdownReceive) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }
        else {
            continue;
        }
    }

    return ntsa::Error();
}

ntsa::Error SessionQueue::shutdown(ntsa::ShutdownType::Value type)
{
    if (type == ntsa::ShutdownType::e_SEND ||
        type == ntsa::ShutdownType::e_BOTH)
    {
        d_shutdownSend = true;
        d_allowEnqueue.broadcast();
    }

    if (type == ntsa::ShutdownType::e_RECEIVE ||
        type == ntsa::ShutdownType::e_BOTH)
    {
        d_shutdownReceive = true;
        d_allowDequeue.broadcast();
    }

    return ntsa::Error();
}

bool SessionQueue::empty() const
{
    return d_currentWatermark == 0;
}

bsl::size_t SessionQueue::totalSize() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_currentWatermark);
}

bsl::size_t SessionQueue::lowWatermark() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_lowWatermark);
}

bsl::size_t SessionQueue::highWatermark() const
{
    return NTCCFG_WARNING_NARROW(bsl::size_t, d_highWatermark);
}

/// Provide a guard to automatically update a session's
/// enabled events.
class Session::UpdateGuard
{
    Session* d_session_p;

  private:
    UpdateGuard(const UpdateGuard&) BSLS_KEYWORD_DELETED;
    UpdateGuard& operator=(const UpdateGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new guard that automatically updates the specified
    /// 'session' upon destruction.
    explicit UpdateGuard(Session* session);

    /// Destroy this object.
    ~UpdateGuard();

    /// Dismiss the guard.
    void dismiss();
};

Session::UpdateGuard::UpdateGuard(Session* session)
: d_session_p(session)
{
}

Session::UpdateGuard::~UpdateGuard()
{
    if (d_session_p) {
        d_session_p->update();
    }
}

void Session::UpdateGuard::dismiss()
{
    d_session_p = 0;
}

void Session::reset()
{
    d_handle    = ntsa::k_INVALID_HANDLE;
    d_transport = ntsa::Transport::e_UNDEFINED;

    d_sourceEndpoint.reset();
    d_remoteEndpoint.reset();

    d_monitor_sp.reset();
    d_peer_wp.reset();

    d_sessionQueue_sp.reset();
    d_outgoingPacketQueue_sp.reset();
    d_incomingPacketQueue_sp.reset();
    d_socketErrorQueue_sp.reset();

    d_blocking         = true;
    d_listening        = false;
    d_accepted         = false;
    d_connected        = false;
    d_readable         = false;
    d_readableActive   = false;
    d_readableBytes    = 0;
    d_writable         = false;
    d_writableActive   = false;
    d_writableBytes    = 0;
    d_error            = false;
    d_errorActive      = false;
    d_errorCode        = 0;
    d_hasNotifications = false;
    d_backlog          = 0;
    d_tsKey            = 0;

    d_socketOptions.reset();

    d_socketOptions.setReuseAddress(k_DEFAULT_REUSE_ADDRESS);
    d_socketOptions.setKeepAlive(k_DEFAULT_KEEP_ALIVE);
    d_socketOptions.setCork(k_DEFAULT_CORK);
    d_socketOptions.setDelayTransmission(k_DEFAULT_DELAY_TRANSMISSION);
    d_socketOptions.setDelayAcknowledgment(k_DEFAULT_DELAY_ACKNOWLEDGEMENT);

    d_socketOptions.setSendBufferSize(k_DEFAULT_SEND_BUFFER_SIZE);
    d_socketOptions.setSendBufferLowWatermark(
        k_DEFAULT_SEND_BUFFER_LOW_WATERMARK);

    d_socketOptions.setReceiveBufferSize(k_DEFAULT_RECEIVE_BUFFER_SIZE);
    d_socketOptions.setReceiveBufferLowWatermark(
        k_DEFAULT_RECEIVE_BUFFER_LOW_WATERMARK);

    d_socketOptions.setDebug(k_DEFAULT_DEBUG);

    bsls::TimeInterval lingerDuration;
    lingerDuration.setTotalSeconds(k_DEFAULT_LINGER_DURATION_IN_SECONDS);

    ntsa::Linger linger;
    linger.setEnabled(k_DEFAULT_LINGER_ENABLED);
    linger.setDuration(lingerDuration);

    d_socketOptions.setLinger(linger);

    d_socketOptions.setBroadcast(k_DEFAULT_BROADCAST);
    d_socketOptions.setBypassRouting(k_DEFAULT_BYPASS_ROUTING);
    d_socketOptions.setInlineOutOfBandData(k_DEFAULT_INLINE_OUT_OF_BAND_DATA);

    d_feedbackQueue.removeAll();
}

void Session::update()
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<Session> self = this->getSelf(this);

    const bool readable = this->privateIsReadable();
    if (readable) {
        d_readable      = true;
        d_readableBytes = this->privateBytesReadable();

        if (!d_readableActive) {
            if (d_monitor_sp) {
                error =
                    d_monitor_sp->enable(d_handle,
                                         self,
                                         ntca::ReactorEventType::e_READABLE);
                if (error) {
                    NTCD_SESSION_LOG_UPDATE_ENABLE_FAILED(
                        d_machine_sp,
                        this,
                        ntca::ReactorEventType::e_READABLE,
                        error);
                }
            }
            d_readableActive = true;
        }
    }
    else {
        d_readable      = false;
        d_readableBytes = 0;

        if (d_readableActive) {
            if (d_monitor_sp) {
                error =
                    d_monitor_sp->disable(d_handle,
                                          self,
                                          ntca::ReactorEventType::e_READABLE);
                if (error) {
                    NTCD_SESSION_LOG_UPDATE_DISABLE_FAILED(
                        d_machine_sp,
                        this,
                        ntca::ReactorEventType::e_READABLE,
                        error);
                }
            }
            d_readableActive = false;
        }
    }

    const bool writable = this->privateIsWritable();
    if (writable) {
        d_writable      = true;
        d_writableBytes = this->privateBytesWritable();

        if (!d_writableActive) {
            if (d_monitor_sp) {
                error =
                    d_monitor_sp->enable(d_handle,
                                         self,
                                         ntca::ReactorEventType::e_WRITABLE);
                if (error) {
                    NTCD_SESSION_LOG_UPDATE_ENABLE_FAILED(
                        d_machine_sp,
                        this,
                        ntca::ReactorEventType::e_WRITABLE,
                        error);
                }
            }
            d_writableActive = true;
        }
    }
    else {
        d_writable      = false;
        d_writableBytes = 0;

        if (d_writableActive) {
            if (d_monitor_sp) {
                error =
                    d_monitor_sp->disable(d_handle,
                                          self,
                                          ntca::ReactorEventType::e_WRITABLE);
                if (error) {
                    NTCD_SESSION_LOG_UPDATE_DISABLE_FAILED(
                        d_machine_sp,
                        this,
                        ntca::ReactorEventType::e_WRITABLE,
                        error);
                }
            }
            d_writableActive = false;
        }
    }

    const bool hasNotifications = this->privateHasNotification();
    if (hasNotifications) {
        d_hasNotifications = true;

        if (!d_notificationsActive) {
            if (d_monitor_sp) {
                error = d_monitor_sp->enableNotifications(d_handle, self);
                if (error) {
                    NTCD_SESSION_LOG_UPDATE_ENABLE_NOTIFICATIONS_FAILED(
                        d_machine_sp,
                        this,
                        error);
                }
            }
            d_notificationsActive = true;
        }
    }
    else {
        d_hasNotifications = false;

        if (d_notificationsActive) {
            if (d_monitor_sp) {
                error = d_monitor_sp->disableNotifications(d_handle, self);
                if (error) {
                    NTCD_SESSION_LOG_UPDATE_DISABLE_NOTIFICATIONS_FAILED(
                        d_machine_sp,
                        this,
                        error);
                }
            }
            d_notificationsActive = false;
        }
    }

    if (d_errorCode) {
        if (!d_errorActive) {
            if (d_monitor_sp) {
                error = d_monitor_sp->enable(d_handle,
                                             self,
                                             ntca::ReactorEventType::e_ERROR);
                if (error) {
                    NTCD_SESSION_LOG_UPDATE_ENABLE_FAILED(
                        d_machine_sp,
                        this,
                        ntca::ReactorEventType::e_ERROR,
                        error);
                }
            }
            d_errorActive = true;
        }
    }
    else {
        if (d_errorActive) {
            if (d_monitor_sp) {
                error = d_monitor_sp->disable(d_handle,
                                              self,
                                              ntca::ReactorEventType::e_ERROR);
                if (error) {
                    NTCD_SESSION_LOG_UPDATE_DISABLE_FAILED(
                        d_machine_sp,
                        this,
                        ntca::ReactorEventType::e_ERROR,
                        error);
                }
            }
            d_errorActive = true;
        }
    }

    d_machine_sp->update(self);

    if (!d_remoteEndpoint.isUndefined()) {
        bsl::shared_ptr<ntcd::Session> peer = d_peer_wp.lock();
        if (peer) {
            if (peer->d_machine_sp != d_machine_sp) {
                peer->d_machine_sp->update(peer);
            }
        }
    }
}

bsl::size_t Session::privateBytesReadable() const
{
    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return 0;
    }

    if (d_incomingPacketQueue_sp) {
        return d_incomingPacketQueue_sp->totalSize();
    }

    return 0;
}

bsl::size_t Session::privateBytesWritable() const
{
    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return 0;
    }

    if (d_outgoingPacketQueue_sp) {
        bsl::size_t totalSize     = d_outgoingPacketQueue_sp->totalSize();
        bsl::size_t highWatermark = d_outgoingPacketQueue_sp->highWatermark();

        if (totalSize < highWatermark) {
            return highWatermark - totalSize;
        }
        else {
            return 0;
        }
    }

    return 0;
}

ntsa::Error Session::privateError() const
{
    return ntsa::Error(
        static_cast<ntsa::Error::Code>(static_cast<int>(d_errorCode)));
}

bool Session::privateIsReadable() const
{
    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return false;
    }

    ntsa::TransportMode::Value transportMode =
        ntsa::Transport::getMode(d_transport);

    if (d_listening) {
        if (d_sessionQueue_sp) {
            bsl::size_t totalSize    = d_sessionQueue_sp->totalSize();
            bsl::size_t lowWatermark = d_sessionQueue_sp->lowWatermark();

            if (totalSize >= lowWatermark) {
                return true;
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
    }
    else if (transportMode == ntsa::TransportMode::e_DATAGRAM || d_connected) {
        if (d_incomingPacketQueue_sp) {
            bsl::size_t totalSize = d_incomingPacketQueue_sp->totalSize();
            bsl::size_t lowWatermark =
                d_incomingPacketQueue_sp->lowWatermark();

            if (totalSize >= lowWatermark) {
                return true;
            }
            else {
                return false;
            }
        }
    }

    return false;
}

bool Session::privateIsWritable() const
{
    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return false;
    }

    ntsa::TransportMode::Value transportMode =
        ntsa::Transport::getMode(d_transport);

    if (d_listening) {
        return false;
    }
    else if (transportMode == ntsa::TransportMode::e_DATAGRAM || d_connected) {
        if (d_outgoingPacketQueue_sp) {
            bsl::size_t totalSize = d_outgoingPacketQueue_sp->totalSize();
            bsl::size_t highWatermark =
                d_outgoingPacketQueue_sp->highWatermark();

            if (totalSize < highWatermark) {
                return true;
            }
            else {
                return false;
            }
        }
    }

    return false;
}

bool Session::privateHasError() const
{
    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return false;
    }

    if (d_errorCode) {
        return true;
    }
    else {
        return false;
    }
}

bool Session::privateHasNotification() const
{
    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return false;
    }

    BSLS_ASSERT(d_socketErrorQueue_sp);

    if (!d_socketErrorQueue_sp->empty()) {
        return true;
    }
    else {
        return false;
    }
}

Session::Session(const bsl::shared_ptr<ntcd::Machine>& machine,
                 bslma::Allocator*                     basicAllocator)
: d_mutex()
, d_handle(ntsa::k_INVALID_HANDLE)
, d_transport(ntsa::Transport::e_UNDEFINED)
, d_sourceEndpoint()
, d_remoteEndpoint()
, d_socketOptions()
, d_machine_sp(machine)
, d_monitor_sp()
, d_peer_wp()
, d_sessionQueue_sp()
, d_outgoingPacketQueue_sp()
, d_incomingPacketQueue_sp()
, d_socketErrorQueue_sp()
, d_tsKey(0)
, d_blocking(true)
, d_listening(false)
, d_accepted(false)
, d_connected(false)
, d_readable(false)
, d_readableActive(false)
, d_readableBytes(0)
, d_writable(false)
, d_writableActive(false)
, d_writableBytes(0)
, d_error(false)
, d_errorActive(false)
, d_errorCode(0)
, d_hasNotifications(false)
, d_notificationsActive(false)
, d_backlog(0)
, d_feedbackQueue(bslma::Default::allocator(basicAllocator))
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->reset();
}

Session::~Session()
{
    this->close();
}

ntsa::Error Session::open(ntsa::Transport::Value transport)
{
    NTCCFG_WARNING_UNUSED(transport);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    bsl::shared_ptr<Session> self = this->getSelf(this);

    ntsa::Error error;

    if (d_handle != ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Handle handle;
    error = d_machine_sp->acquireHandle(&handle, transport, self);
    if (error) {
        return error;
    }

    d_handle    = handle;
    d_transport = transport;

    d_outgoingPacketQueue_sp.createInplace(d_allocator_p, d_allocator_p);
    d_incomingPacketQueue_sp.createInplace(d_allocator_p, d_allocator_p);
    d_socketErrorQueue_sp.createInplace(d_allocator_p, d_allocator_p);

    d_outgoingPacketQueue_sp->setHighWatermark(
        d_socketOptions.sendBufferSize().value());

    d_incomingPacketQueue_sp->setHighWatermark(
        d_socketOptions.receiveBufferSize().value());

    return ntsa::Error();
}

ntsa::Error Session::acquire(ntsa::Handle handle)
{
    NTCCFG_WARNING_UNUSED(handle);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Handle Session::release()
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    ntsa::Handle handle = d_handle;

    d_handle    = ntsa::k_INVALID_HANDLE;
    d_transport = ntsa::Transport::e_UNDEFINED;

    d_sourceEndpoint.reset();
    d_remoteEndpoint.reset();

    return handle;
}

ntsa::Error Session::bind(const ntsa::Endpoint& endpoint, bool reuseAddress)
{
    NTCCFG_WARNING_UNUSED(reuseAddress);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    bsl::shared_ptr<Session> self = this->getSelf(this);

    ntsa::Error error;

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!d_sourceEndpoint.isUndefined()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Endpoint sourceEndpoint;
    error = d_machine_sp->acquireSourceEndpoint(&sourceEndpoint,
                                                endpoint,
                                                d_transport,
                                                self);
    if (error) {
        return error;
    }

    d_sourceEndpoint = sourceEndpoint;

    return ntsa::Error();
}

ntsa::Error Session::bindAny(ntsa::Transport::Value transport,
                             bool                   reuseAddress)
{
    NTCCFG_WARNING_UNUSED(reuseAddress);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    bsl::shared_ptr<Session> self = this->getSelf(this);

    ntsa::Error error;

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!d_sourceEndpoint.isUndefined()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Endpoint endpoint;

    ntsa::TransportDomain::Value transportDomain =
        ntsa::Transport::getDomain(transport);

    if (transportDomain == ntsa::TransportDomain::e_IPV4) {
        endpoint =
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0));
    }
    else if (transportDomain == ntsa::TransportDomain::e_IPV6) {
        endpoint =
            ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), 0));
    }
    else if (transportDomain == ntsa::TransportDomain::e_LOCAL) {
        ntsa::LocalName   localName;
        const ntsa::Error error = ntsa::LocalName::generateUnique(&localName);
        if (error) {
            return error;
        }
        endpoint = ntsa::Endpoint(localName);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Endpoint sourceEndpoint;
    error = d_machine_sp->acquireSourceEndpoint(&sourceEndpoint,
                                                endpoint,
                                                d_transport,
                                                self);
    if (error) {
        return error;
    }

    d_sourceEndpoint = sourceEndpoint;

    return ntsa::Error();
}

ntsa::Error Session::listen(bsl::size_t backlog)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_sourceEndpoint.isUndefined()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::TransportMode::Value transportMode =
        ntsa::Transport::getMode(d_transport);

    if (transportMode == ntsa::TransportMode::e_DATAGRAM) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_listening = true;
    d_backlog   = backlog > 0 ? backlog : k_DEFAULT_BACKLOG;

    if (!d_sessionQueue_sp) {
        d_sessionQueue_sp.createInplace(d_allocator_p, d_allocator_p);
        d_sessionQueue_sp->setLowWatermark(1);
    }

    d_sessionQueue_sp->setHighWatermark(d_backlog);

    return ntsa::Error();
}

ntsa::Error Session::accept(ntsa::Handle* result)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    *result = ntsa::k_INVALID_HANDLE;

    ntsa::Error error;

    bsl::shared_ptr<ntcd::Session> session;
    error = this->accept(&session, d_allocator_p);
    if (error) {
        return error;
    }

    *result = session->handle();
    return ntsa::Error();
}

ntsa::Error Session::accept(bslma::ManagedPtr<ntsi::StreamSocket>* result,
                            bslma::Allocator* basicAllocator)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);

    result->reset();

    ntsa::Error error;

    bsl::shared_ptr<ntcd::Session> session;
    error = this->accept(&session, d_allocator_p);
    if (error) {
        return error;
    }

    *result = session.managedPtr();
    return ntsa::Error();
}

ntsa::Error Session::accept(bsl::shared_ptr<ntsi::StreamSocket>* result,
                            bslma::Allocator* basicAllocator)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);

    result->reset();

    ntsa::Error error;

    bsl::shared_ptr<ntcd::Session> session;
    error = this->accept(&session, d_allocator_p);
    if (error) {
        return error;
    }

    *result = session;
    return ntsa::Error();
}

ntsa::Error Session::accept(bsl::shared_ptr<ntcd::Session>* result,
                            bslma::Allocator*               basicAllocator)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    UpdateGuard update(this);

    ntsa::Error error;

    result->reset();

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::TransportMode::Value transportMode =
        ntsa::Transport::getMode(d_transport);

    if (transportMode == ntsa::TransportMode::e_DATAGRAM) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!d_listening) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!d_sessionQueue_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<ntcd::Session> session;
    error = d_sessionQueue_sp->dequeueSession(&d_mutex, &session, d_blocking);
    if (error) {
        return error;
    }

    *result = session;
    return ntsa::Error();
}

ntsa::Error Session::connect(const ntsa::Endpoint& endpoint)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    UpdateGuard update(this);

    ntsa::Error error;

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_listening) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::TransportMode::Value transportMode =
        ntsa::Transport::getMode(d_transport);

    if (d_sourceEndpoint.isUndefined()) {
        ntsa::TransportDomain::Value transportDomain =
            ntsa::Transport::getDomain(d_transport);

        ntsa::Endpoint sourceEndpointRequested;
        if (transportDomain == ntsa::TransportDomain::e_IPV4) {
            sourceEndpointRequested = ntsa::Endpoint(
                ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0));
        }
        else if (transportDomain == ntsa::TransportDomain::e_IPV6) {
            sourceEndpointRequested = ntsa::Endpoint(
                ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), 0));
        }
        else if (transportDomain == ntsa::TransportDomain::e_LOCAL) {
            ntsa::LocalName   localName;
            const ntsa::Error error =
                ntsa::LocalName::generateUnique(&localName);
            if (error) {
                return error;
            }
            sourceEndpointRequested = ntsa::Endpoint(localName);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        ntsa::Endpoint sourceEndpointAssigned;
        error = d_machine_sp->acquireSourceEndpoint(&sourceEndpointAssigned,
                                                    sourceEndpointRequested,
                                                    d_transport,
                                                    this->getSelf(this));
        if (error) {
            return error;
        }

        d_sourceEndpoint = sourceEndpointAssigned;
    }

    if (transportMode == ntsa::TransportMode::e_DATAGRAM) {
        bsl::weak_ptr<ntcd::Session> peer_wp;
        error = d_machine_sp->lookupSession(&peer_wp, endpoint, d_transport);
        if (error) {
            d_errorCode = ntsa::Error::e_CONNECTION_REFUSED;
            return this->privateError();
        }

        d_peer_wp        = peer_wp;
        d_remoteEndpoint = endpoint;
        d_connected      = true;
    }
    else if (transportMode == ntsa::TransportMode::e_STREAM) {
        if (d_connected) {
            BSLS_ASSERT(!d_remoteEndpoint.isUndefined());
            if (endpoint == d_remoteEndpoint) {
                return ntsa::Error();
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }

        BSLS_ASSERT(!d_connected);
        BSLS_ASSERT(d_remoteEndpoint.isUndefined());

        bsl::weak_ptr<ntcd::Session> peer_wp;
        error = d_machine_sp->lookupSession(&peer_wp, endpoint, d_transport);
        if (error) {
            d_errorCode = ntsa::Error::e_CONNECTION_REFUSED;
            return this->privateError();
        }

        bsl::shared_ptr<ntcd::Session> peer = peer_wp.lock();
        if (!peer) {
            d_errorCode = ntsa::Error::e_CONNECTION_REFUSED;
            return this->privateError();
        }

        ntccfg::ConditionMutexGuard peerLock(&peer->d_mutex);

        UpdateGuard peerUpdate(peer.get());

        if (!peer->d_listening) {
            d_errorCode = ntsa::Error::e_CONNECTION_REFUSED;
            return this->privateError();
        }

        bsl::shared_ptr<ntcd::Session> serverSession;
        serverSession.createInplace(d_allocator_p,
                                    d_machine_sp,
                                    d_allocator_p);

        ntsa::Handle handle;
        error =
            d_machine_sp->acquireHandle(&handle, d_transport, serverSession);
        if (error) {
            return error;
        }

        serverSession->d_handle         = handle;
        serverSession->d_transport      = d_transport;
        serverSession->d_sourceEndpoint = peer->d_sourceEndpoint;
        serverSession->d_remoteEndpoint = d_sourceEndpoint;
        serverSession->d_accepted       = true;
        serverSession->d_connected      = true;
        serverSession->d_blocking       = d_blocking;
        serverSession->d_socketOptions  = d_socketOptions;

        serverSession->d_outgoingPacketQueue_sp.createInplace(d_allocator_p,
                                                              d_allocator_p);

        serverSession->d_incomingPacketQueue_sp.createInplace(d_allocator_p,
                                                              d_allocator_p);

        serverSession->d_socketErrorQueue_sp.createInplace(d_allocator_p,
                                                           d_allocator_p);

        serverSession->d_outgoingPacketQueue_sp->setHighWatermark(
            serverSession->d_socketOptions.sendBufferSize().value());

        serverSession->d_incomingPacketQueue_sp->setHighWatermark(
            serverSession->d_socketOptions.receiveBufferSize().value());

        error = peer->d_sessionQueue_sp->enqueueSession(&peer->d_mutex,
                                                        serverSession,
                                                        d_blocking);
        if (error) {
            return error;
        }

        BSLS_ASSERT(endpoint == peer->d_sourceEndpoint);
        BSLS_ASSERT(endpoint == serverSession->d_sourceEndpoint);

        d_peer_wp        = bsl::weak_ptr<ntcd::Session>(serverSession);
        d_remoteEndpoint = endpoint;
        d_connected      = true;
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Session::send(ntsa::SendContext*       context,
                          const bdlbb::Blob&       data,
                          const ntsa::SendOptions& options)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    UpdateGuard update(this);

    ntsa::Error error;

    context->reset();

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_handle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    NTCCFG_WARNING_UNUSED(options);

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_listening) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_errorCode) {
        return this->privateError();
    }

    if (!d_remoteEndpoint.isUndefined()) {
        if (!options.endpoint().isNull()) {
            if (options.endpoint().value() != d_remoteEndpoint) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
    }

    ntsa::Endpoint remoteEndpoint;
    if (!options.endpoint().isNull()) {
        remoteEndpoint = options.endpoint().value();
    }
    else if (!d_remoteEndpoint.isUndefined()) {
        remoteEndpoint = d_remoteEndpoint;
    }

    ntsa::TransportMode::Value transportMode =
        ntsa::Transport::getMode(d_transport);

    if (d_sourceEndpoint.isUndefined()) {
        if (transportMode == ntsa::TransportMode::e_DATAGRAM) {
            ntsa::TransportDomain::Value transportDomain =
                ntsa::Transport::getDomain(d_transport);

            ntsa::Endpoint sourceEndpointRequested;
            if (transportDomain == ntsa::TransportDomain::e_IPV4) {
                sourceEndpointRequested = ntsa::Endpoint(
                    ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0));
            }
            else if (transportDomain == ntsa::TransportDomain::e_IPV6) {
                sourceEndpointRequested = ntsa::Endpoint(
                    ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), 0));
            }
            else if (transportDomain == ntsa::TransportDomain::e_LOCAL) {
                ntsa::LocalName   localName;
                const ntsa::Error error =
                    ntsa::LocalName::generateUnique(&localName);
                if (error) {
                    return error;
                }
                sourceEndpointRequested = ntsa::Endpoint(localName);
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            ntsa::Endpoint sourceEndpointAssigned;
            error =
                d_machine_sp->acquireSourceEndpoint(&sourceEndpointAssigned,
                                                    sourceEndpointRequested,
                                                    d_transport,
                                                    this->getSelf(this));
            if (error) {
                return error;
            }

            d_sourceEndpoint = sourceEndpointAssigned;
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (transportMode == ntsa::TransportMode::e_DATAGRAM) {
        bsl::shared_ptr<ntcd::Packet> packet = d_machine_sp->createPacket();

        packet->setType(ntcd::PacketType::e_PUSH);
        packet->setTransport(d_transport);
        packet->setSourceEndpoint(d_sourceEndpoint);
        packet->setSourceSession(this->weak_from_this());
        packet->setRemoteEndpoint(remoteEndpoint);
        packet->setRemoteSession(d_peer_wp);

        error = packet->enqueueData(context, data, options);
        if (error) {
            NTCD_SESSION_LOG_OUTGOING_PACKET_QUEUE_ENQUEUE_ERROR(d_machine_sp,
                                                                 this,
                                                                 packet,
                                                                 error);

            context->setBytesSent(0);
            return error;
        }

        ntcd::PacketQueue::PacketFunctor functor;
        if (d_socketOptions.timestampOutgoingData().value_or(false)) {
            functor = NTCCFG_BIND(&generateTransmitTimestampScheduled,
                                  NTCCFG_BIND_PLACEHOLDER_1,
                                  &d_tsKey,
                                  d_socketErrorQueue_sp,
                                  1);
        }

        error = d_outgoingPacketQueue_sp->enqueue(&d_mutex,
                                                  packet,
                                                  d_blocking,
                                                  functor);
        if (error) {
            NTCD_SESSION_LOG_OUTGOING_PACKET_QUEUE_ENQUEUE_ERROR(d_machine_sp,
                                                                 this,
                                                                 packet,
                                                                 error);

            context->setBytesSent(0);
            return error;
        }

        NTCD_SESSION_LOG_OUTGOING_PACKET_QUEUE_ENQUEUED(d_machine_sp,
                                                        this,
                                                        packet);
    }
    else if (transportMode == ntsa::TransportMode::e_STREAM) {
        bdlbb::Blob dataCopy = data;

        bsl::size_t numBytesRemaining =
            NTCCFG_WARNING_PROMOTE(bsl::size_t, dataCopy.length());

        context->setBytesSendable(numBytesRemaining);

        while (true) {
            if (numBytesRemaining == 0) {
                break;
            }

            bsl::shared_ptr<ntcd::Packet> packet =
                d_machine_sp->createPacket();

            packet->setType(ntcd::PacketType::e_PUSH);
            packet->setTransport(d_transport);
            packet->setSourceEndpoint(d_sourceEndpoint);
            packet->setSourceSession(this->weak_from_this());
            packet->setRemoteEndpoint(remoteEndpoint);
            packet->setRemoteSession(d_peer_wp);

            ntsa::SendContext perPacketContext;
            error = packet->enqueueData(&perPacketContext, dataCopy, options);
            if (error) {
                NTCD_SESSION_LOG_OUTGOING_PACKET_QUEUE_ENQUEUE_ERROR(
                    d_machine_sp,
                    this,
                    packet,
                    error);
                return error;
            }

            ntcd::PacketQueue::PacketFunctor functor;
            if (d_socketOptions.timestampOutgoingData().value_or(false)) {
                functor = NTCCFG_BIND(&generateTransmitTimestampScheduled,
                                      NTCCFG_BIND_PLACEHOLDER_1,
                                      &d_tsKey,
                                      d_socketErrorQueue_sp,
                                      perPacketContext.bytesSent());
            }

            error = d_outgoingPacketQueue_sp->enqueue(&d_mutex,
                                                      packet,
                                                      d_blocking,
                                                      functor);
            if (error) {
                NTCD_SESSION_LOG_OUTGOING_PACKET_QUEUE_ENQUEUE_ERROR(
                    d_machine_sp,
                    this,
                    packet,
                    error);

                if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK)) {
                    if (context->bytesSent() > 0) {
                        return ntsa::Error();
                    }
                    else {
                        return error;
                    }
                }
                else {
                    return error;
                }
            }

            NTCD_SESSION_LOG_OUTGOING_PACKET_QUEUE_ENQUEUED(d_machine_sp,
                                                            this,
                                                            packet);

            bsl::size_t numBytesSent = perPacketContext.bytesSent();

            context->setBytesSent(context->bytesSent() + numBytesSent);

            bdlbb::BlobUtil::erase(&dataCopy,
                                   0,
                                   NTCCFG_WARNING_NARROW(int, numBytesSent));

            numBytesRemaining =
                NTCCFG_WARNING_PROMOTE(bsl::size_t, dataCopy.length());
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    return ntsa::Error();
}

ntsa::Error Session::send(ntsa::SendContext*       context,
                          const ntsa::Data&        data,
                          const ntsa::SendOptions& options)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    UpdateGuard update(this);

    ntsa::Error error;

    context->reset();

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_handle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    NTCCFG_WARNING_UNUSED(options);

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_listening) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_errorCode) {
        return this->privateError();
    }

    if (!d_remoteEndpoint.isUndefined()) {
        if (!options.endpoint().isNull()) {
            if (options.endpoint().value() != d_remoteEndpoint) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
    }

    ntsa::Endpoint remoteEndpoint;
    if (!options.endpoint().isNull()) {
        remoteEndpoint = options.endpoint().value();
    }
    else if (!d_remoteEndpoint.isUndefined()) {
        remoteEndpoint = d_remoteEndpoint;
    }

    ntsa::TransportMode::Value transportMode =
        ntsa::Transport::getMode(d_transport);

    if (d_sourceEndpoint.isUndefined()) {
        if (transportMode == ntsa::TransportMode::e_DATAGRAM) {
            ntsa::TransportDomain::Value transportDomain =
                ntsa::Transport::getDomain(d_transport);

            ntsa::Endpoint sourceEndpointRequested;
            if (transportDomain == ntsa::TransportDomain::e_IPV4) {
                sourceEndpointRequested = ntsa::Endpoint(
                    ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0));
            }
            else if (transportDomain == ntsa::TransportDomain::e_IPV6) {
                sourceEndpointRequested = ntsa::Endpoint(
                    ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), 0));
            }
            else if (transportDomain == ntsa::TransportDomain::e_LOCAL) {
                ntsa::LocalName   localName;
                const ntsa::Error error =
                    ntsa::LocalName::generateUnique(&localName);
                if (error) {
                    return error;
                }
                sourceEndpointRequested = ntsa::Endpoint(localName);
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            ntsa::Endpoint sourceEndpointAssigned;
            error =
                d_machine_sp->acquireSourceEndpoint(&sourceEndpointAssigned,
                                                    sourceEndpointRequested,
                                                    d_transport,
                                                    this->getSelf(this));
            if (error) {
                return error;
            }

            d_sourceEndpoint = sourceEndpointAssigned;
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (transportMode == ntsa::TransportMode::e_DATAGRAM) {
        bsl::shared_ptr<ntcd::Packet> packet = d_machine_sp->createPacket();

        packet->setType(ntcd::PacketType::e_PUSH);
        packet->setTransport(d_transport);
        packet->setSourceEndpoint(d_sourceEndpoint);
        packet->setSourceSession(this->weak_from_this());
        packet->setRemoteEndpoint(remoteEndpoint);
        packet->setRemoteSession(d_peer_wp);

        error = packet->enqueueData(context, data, options);
        if (error) {
            NTCD_SESSION_LOG_OUTGOING_PACKET_QUEUE_ENQUEUE_ERROR(d_machine_sp,
                                                                 this,
                                                                 packet,
                                                                 error);

            context->setBytesSent(0);
            return error;
        }

        ntcd::PacketQueue::PacketFunctor functor;
        if (d_socketOptions.timestampOutgoingData().value_or(false)) {
            functor = NTCCFG_BIND(&generateTransmitTimestampScheduled,
                                  NTCCFG_BIND_PLACEHOLDER_1,
                                  &d_tsKey,
                                  d_socketErrorQueue_sp,
                                  1);
        }

        error = d_outgoingPacketQueue_sp->enqueue(&d_mutex,
                                                  packet,
                                                  d_blocking,
                                                  functor);
        if (error) {
            NTCD_SESSION_LOG_OUTGOING_PACKET_QUEUE_ENQUEUE_ERROR(d_machine_sp,
                                                                 this,
                                                                 packet,
                                                                 error);

            context->setBytesSent(0);
            return error;
        }

        NTCD_SESSION_LOG_OUTGOING_PACKET_QUEUE_ENQUEUED(d_machine_sp,
                                                        this,
                                                        packet);
    }
    else if (transportMode == ntsa::TransportMode::e_STREAM) {
        ntsa::Data dataCopy = data;

        bsl::size_t numBytesRemaining = dataCopy.size();
        context->setBytesSendable(numBytesRemaining);

        while (true) {
            if (numBytesRemaining == 0) {
                break;
            }

            bsl::shared_ptr<ntcd::Packet> packet =
                d_machine_sp->createPacket();

            packet->setType(ntcd::PacketType::e_PUSH);
            packet->setTransport(d_transport);
            packet->setSourceEndpoint(d_sourceEndpoint);
            packet->setSourceSession(this->weak_from_this());
            packet->setRemoteEndpoint(remoteEndpoint);
            packet->setRemoteSession(d_peer_wp);

            ntsa::SendContext perPacketContext;
            error = packet->enqueueData(&perPacketContext, dataCopy, options);
            if (error) {
                NTCD_SESSION_LOG_OUTGOING_PACKET_QUEUE_ENQUEUE_ERROR(
                    d_machine_sp,
                    this,
                    packet,
                    error);
                return error;
            }

            ntcd::PacketQueue::PacketFunctor functor;
            if (d_socketOptions.timestampOutgoingData().value_or(false)) {
                functor = NTCCFG_BIND(&generateTransmitTimestampScheduled,
                                      NTCCFG_BIND_PLACEHOLDER_1,
                                      &d_tsKey,
                                      d_socketErrorQueue_sp,
                                      perPacketContext.bytesSent());
            }

            error = d_outgoingPacketQueue_sp->enqueue(&d_mutex,
                                                      packet,
                                                      d_blocking,
                                                      functor);
            if (error) {
                NTCD_SESSION_LOG_OUTGOING_PACKET_QUEUE_ENQUEUE_ERROR(
                    d_machine_sp,
                    this,
                    packet,
                    error);

                if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK)) {
                    if (context->bytesSent() > 0) {
                        return ntsa::Error();
                    }
                    else {
                        return error;
                    }
                }
                else {
                    return error;
                }
            }

            NTCD_SESSION_LOG_OUTGOING_PACKET_QUEUE_ENQUEUED(d_machine_sp,
                                                            this,
                                                            packet);

            bsl::size_t numBytesSent = perPacketContext.bytesSent();

            context->setBytesSent(context->bytesSent() + numBytesSent);

            ntsa::DataUtil::pop(&dataCopy, numBytesSent);
            numBytesRemaining = dataCopy.size();
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    return ntsa::Error();
}

ntsa::Error Session::send(ntsa::SendContext*       context,
                          const ntsa::ConstBuffer* data,
                          bsl::size_t              size,
                          const ntsa::SendOptions& options)
{
    ntsa::ConstBufferArray array;
    array.append(data, size);

    return this->send(context, ntsa::Data(array), options);
}

ntsa::Error Session::receive(ntsa::ReceiveContext*       context,
                             bdlbb::Blob*                data,
                             const ntsa::ReceiveOptions& options)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    UpdateGuard update(this);

    ntsa::Error error;

    context->reset();

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_handle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::TransportMode::Value transportMode =
        ntsa::Transport::getMode(d_transport);

    if (transportMode == ntsa::TransportMode::e_DATAGRAM) {
        bsl::shared_ptr<ntcd::Packet> packet;
        while (true) {
            error = d_incomingPacketQueue_sp->dequeue(&d_mutex,
                                                      &packet,
                                                      d_blocking);
            if (error) {
                return error;
            }

            if (packet->remoteEndpoint() != d_sourceEndpoint) {
                continue;
            }

            if (!d_remoteEndpoint.isUndefined()) {
                if (packet->sourceEndpoint() != d_remoteEndpoint) {
                    continue;
                }
            }

            break;
        }

        if (packet->type() == ntcd::PacketType::e_PUSH) {
            error = packet->dequeueData(context, data, options);
            if (error) {
                return error;
            }
        }
        else {
            NTCD_SESSION_LOG_INCOMING_PACKET_QUEUE_DEQUEUED_UNSUPPORTED(
                d_machine_sp,
                this,
                packet);

            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (options.wantTimestamp() && packet->rxTimestamp().has_value()) {
            context->setSoftwareTimestamp(packet->rxTimestamp().value());
        }
    }
    else if (transportMode == ntsa::TransportMode::e_STREAM) {
        bsl::shared_ptr<ntcd::Packet> packet;
        while (true) {
            error = d_incomingPacketQueue_sp->dequeue(&d_mutex,
                                                      &packet,
                                                      d_blocking);
            if (error) {
                return error;
            }

            if (packet->remoteEndpoint() != d_sourceEndpoint) {
                continue;
            }

            if (!d_remoteEndpoint.isUndefined()) {
                if (packet->sourceEndpoint() != d_remoteEndpoint) {
                    continue;
                }
            }

            // Sending back ACK timestamp
            {
                bsl::shared_ptr<ntcd::Session> sourceSession =
                    packet->sourceSession().lock();
                if (sourceSession && packet->id().has_value()) {
                    ntsa::Timestamp t;
                    t.setType(ntsa::TimestampType::e_ACKNOWLEDGED);
                    t.setTime(bdlt::CurrentTime::now());
                    t.setId(packet->id().value());
                    sourceSession->d_feedbackQueue.pushBack(t);
                }
            }

            break;
        }

        if (packet->type() == ntcd::PacketType::e_PUSH) {
            error = packet->dequeueData(context, data, options);
            if (error) {
                return error;
            }

            if (packet->cost() > 0) {
                d_incomingPacketQueue_sp->retry(packet);
            }
        }
        else if (packet->type() == ntcd::PacketType::e_SHUTDOWN) {
            error = d_incomingPacketQueue_sp->shutdown();
            if (error) {
                return error;
            }

            return ntsa::Error(ntsa::Error::e_EOF);
        }
        else {
            NTCD_SESSION_LOG_INCOMING_PACKET_QUEUE_DEQUEUED_UNSUPPORTED(
                d_machine_sp,
                this,
                packet);

            return ntsa::Error(ntsa::Error::e_INVALID);
        }
        if (options.wantTimestamp() && packet->rxTimestamp().has_value()) {
            context->setSoftwareTimestamp(packet->rxTimestamp().value());
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Session::receive(ntsa::ReceiveContext*       context,
                             ntsa::Data*                 data,
                             const ntsa::ReceiveOptions& options)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    UpdateGuard update(this);

    ntsa::Error error;

    context->reset();

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_handle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::TransportMode::Value transportMode =
        ntsa::Transport::getMode(d_transport);

    if (transportMode == ntsa::TransportMode::e_DATAGRAM) {
        bsl::shared_ptr<ntcd::Packet> packet;
        while (true) {
            error = d_incomingPacketQueue_sp->dequeue(&d_mutex,
                                                      &packet,
                                                      d_blocking);
            if (error) {
                return error;
            }

            if (packet->remoteEndpoint() != d_sourceEndpoint) {
                continue;
            }

            if (!d_remoteEndpoint.isUndefined()) {
                if (packet->sourceEndpoint() != d_remoteEndpoint) {
                    continue;
                }
            }

            break;
        }

        if (packet->type() == ntcd::PacketType::e_PUSH) {
            error = packet->dequeueData(context, data, options);
            if (error) {
                return error;
            }
        }
        else {
            NTCD_SESSION_LOG_INCOMING_PACKET_QUEUE_DEQUEUED_UNSUPPORTED(
                d_machine_sp,
                this,
                packet);

            return ntsa::Error(ntsa::Error::e_INVALID);
        }
        if (options.wantTimestamp() && packet->rxTimestamp().has_value()) {
            context->setSoftwareTimestamp(packet->rxTimestamp().value());
        }
    }
    else if (transportMode == ntsa::TransportMode::e_STREAM) {
        bsl::shared_ptr<ntcd::Packet> packet;
        while (true) {
            error = d_incomingPacketQueue_sp->dequeue(&d_mutex,
                                                      &packet,
                                                      d_blocking);

            if (error) {
                return error;
            }

            if (packet->remoteEndpoint() != d_sourceEndpoint) {
                continue;
            }

            if (!d_remoteEndpoint.isUndefined()) {
                if (packet->sourceEndpoint() != d_remoteEndpoint) {
                    continue;
                }
            }

            // Sending back ACK timestamp
            {
                bsl::shared_ptr<ntcd::Session> sourceSession =
                    packet->sourceSession().lock();
                if (sourceSession && packet->id().has_value()) {
                    ntsa::Timestamp t;
                    t.setType(ntsa::TimestampType::e_ACKNOWLEDGED);
                    t.setTime(bdlt::CurrentTime::now());
                    t.setId(packet->id().value());
                    sourceSession->d_feedbackQueue.pushBack(t);
                }
            }

            break;
        }

        if (packet->type() == ntcd::PacketType::e_PUSH) {
            error = packet->dequeueData(context, data, options);
            if (error) {
                return error;
            }

            if (packet->cost() > 0) {
                d_incomingPacketQueue_sp->retry(packet);
            }
        }
        else if (packet->type() == ntcd::PacketType::e_SHUTDOWN) {
            error = d_incomingPacketQueue_sp->shutdown();
            if (error) {
                return error;
            }

            return ntsa::Error(ntsa::Error::e_EOF);
        }
        else {
            NTCD_SESSION_LOG_INCOMING_PACKET_QUEUE_DEQUEUED_UNSUPPORTED(
                d_machine_sp,
                this,
                packet);

            return ntsa::Error(ntsa::Error::e_INVALID);
        }
        if (options.wantTimestamp() && packet->rxTimestamp().has_value()) {
            context->setSoftwareTimestamp(packet->rxTimestamp().value());
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Session::receiveNotifications(
    ntsa::NotificationQueue* notifications)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    UpdateGuard update(this);

    if (notifications) {
        for (SocketErrorQueue::const_iterator it =
                 d_socketErrorQueue_sp->begin();
             it != d_socketErrorQueue_sp->cend();
             ++it)
        {
            notifications->addNotification(*it);
        }
    }

    d_socketErrorQueue_sp->clear();

    return ntsa::Error();
}

ntsa::Error Session::shutdown(ntsa::ShutdownType::Value direction)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    UpdateGuard update(this);

    ntsa::Error error;

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_handle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (direction == ntsa::ShutdownType::e_SEND ||
        direction == ntsa::ShutdownType::e_BOTH)
    {
        ntsa::TransportMode::Value transportMode =
            ntsa::Transport::getMode(d_transport);

        if (transportMode == ntsa::TransportMode::e_STREAM) {
            if (!d_remoteEndpoint.isUndefined()) {
                bsl::shared_ptr<ntcd::Packet> packet =
                    d_machine_sp->createPacket();

                packet->setType(ntcd::PacketType::e_SHUTDOWN);
                packet->setTransport(d_transport);
                packet->setSourceEndpoint(d_sourceEndpoint);
                packet->setSourceSession(this->weak_from_this());
                packet->setRemoteEndpoint(d_remoteEndpoint);
                packet->setRemoteSession(d_peer_wp);

                error = d_outgoingPacketQueue_sp->enqueue(&d_mutex,
                                                          packet,
                                                          d_blocking);
                if (error) {
                    NTCD_SESSION_LOG_OUTGOING_PACKET_QUEUE_ENQUEUE_ERROR(
                        d_machine_sp,
                        this,
                        packet,
                        error);

                    return error;
                }

                NTCD_SESSION_LOG_OUTGOING_PACKET_QUEUE_ENQUEUED(d_machine_sp,
                                                                this,
                                                                packet);
            }
        }

        error = d_outgoingPacketQueue_sp->shutdown();
        if (error) {
            return error;
        }
    }

    if (direction == ntsa::ShutdownType::e_RECEIVE ||
        direction == ntsa::ShutdownType::e_BOTH)
    {
        if (d_listening) {
            error = d_sessionQueue_sp->shutdown(direction);
            if (error) {
                return error;
            }
        }

        error = d_incomingPacketQueue_sp->shutdown();
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Session::unlink()
{
    return ntsa::Error();
}

ntsa::Error Session::close()
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    ntsa::Error error;

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error();
    }

    if (!d_accepted) {
        if (!d_sourceEndpoint.isUndefined()) {
            error = d_machine_sp->releaseSourceEndpoint(d_sourceEndpoint,
                                                        d_transport);
            if (error) {
                return error;
            }
        }
    }

    error = d_machine_sp->releaseHandle(d_handle, d_transport);
    if (error) {
        return error;
    }

    if (d_monitor_sp) {
        d_monitor_sp->remove(this->getSelf(this));
    }

    this->reset();

    return ntsa::Error();
}

ntsa::Error Session::registerMonitor(
    const bsl::shared_ptr<ntcd::Monitor>& monitor)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    if (d_monitor_sp) {
        if (monitor != d_monitor_sp) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        d_readableActive      = false;
        d_writableActive      = false;
        d_errorActive         = false;
        d_notificationsActive = false;

        d_monitor_sp = monitor;

        this->update();
    }

    BSLS_ASSERT(d_monitor_sp == monitor);
    return ntsa::Error();
}

ntsa::Error Session::deregisterMonitor(
    const bsl::shared_ptr<ntcd::Monitor>& monitor)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    if (d_monitor_sp) {
        if (monitor != d_monitor_sp) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    d_monitor_sp.reset();

    return ntsa::Error();
}

ntsa::Error Session::step(bool block)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    UpdateGuard update(this);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(d_handle);
    NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(d_sourceEndpoint);
    NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(d_remoteEndpoint);

    ntsa::Error error;

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error();
    }

    if (!d_machine_sp) {
        return ntsa::Error();
    }

    if (!d_outgoingPacketQueue_sp) {
        return ntsa::Error();
    }

    NTCD_SESSION_LOG_STEP_STARTING(d_machine_sp, this);

    // Process the outgoing packets.

    typedef ntcd::PacketQueue::PacketVector PacketVector;
    PacketVector                            packetsToRetransmit;

    bsl::size_t numPacketsTransferred = 0;

    ntsa::TransportMode::Value transportMode =
        ntsa::Transport::getMode(d_transport);

    while (true) {
        bsl::shared_ptr<ntcd::Packet> packet;
        error = d_outgoingPacketQueue_sp->dequeue(&d_mutex, &packet, block);
        if (error) {
            break;
        }

        if (d_socketOptions.timestampOutgoingData().value_or(false) &&
            packet->id().has_value())
        {
            ntsa::Notification n;
            ntsa::Timestamp&   t = n.makeTimestamp();
            t.setType(ntsa::TimestampType::e_SENT);
            t.setTime(bdlt::CurrentTime::now());
            t.setId(packet->id().value());
            d_socketErrorQueue_sp->push_back(n);
        }

        NTCD_SESSION_LOG_TRANSFERRING_PACKET(d_machine_sp, this, packet);

        bsl::weak_ptr<ntcd::Session> remoteSession_wp =
            packet->remoteSession();

        bsl::shared_ptr<ntcd::Session> remoteSession = remoteSession_wp.lock();

        if (!remoteSession) {
            error = d_machine_sp->lookupSession(&remoteSession_wp,
                                                packet->remoteEndpoint(),
                                                d_transport);
            if (error) {
                NTCD_SESSION_LOG_TRANSFERRING_PACKET_FAILED_PEER_MISSING(
                    d_machine_sp,
                    this,
                    packet);

                d_errorCode = ntsa::Error::e_CONNECTION_DEAD;
                continue;
            }

            remoteSession = remoteSession_wp.lock();

            if (!remoteSession) {
                NTCD_SESSION_LOG_TRANSFERRING_PACKET_FAILED_PEER_DEAD(
                    d_machine_sp,
                    this,
                    packet);

                d_errorCode = ntsa::Error::e_CONNECTION_DEAD;
                continue;
            }
        }

        ntccfg::ConditionMutexGuard remoteLock(&remoteSession->d_mutex);

        if (!remoteSession->d_incomingPacketQueue_sp) {
            NTCD_SESSION_LOG_TRANSFERRING_PACKET_FAILED_PEER_DEAD(d_machine_sp,
                                                                  this,
                                                                  packet);

            d_errorCode = ntsa::Error::e_CONNECTION_DEAD;
            continue;
        }

        ntcd::PacketQueue::PacketFunctor functor;
        if (remoteSession->d_socketOptions.timestampIncomingData().value_or(
                false))
        {
            functor = NTCCFG_BIND(&generateReceiveTimestamp,
                                  NTCCFG_BIND_PLACEHOLDER_1);
        }

        error = remoteSession->d_incomingPacketQueue_sp
                    ->enqueue(&remoteSession->d_mutex, packet, block, functor);
        if (error) {
            NTCD_SESSION_LOG_TRANSFERRING_PACKET_FAILED(d_machine_sp,
                                                        this,
                                                        packet,
                                                        remoteSession,
                                                        error);

            packetsToRetransmit.push_back(packet);

            if (transportMode == ntsa::TransportMode::e_DATAGRAM) {
                continue;
            }
            else if (transportMode == ntsa::TransportMode::e_STREAM) {
                break;
            }
        }
        else {
            UpdateGuard remoteUpdate(remoteSession.get());
            ++numPacketsTransferred;
        }
    }

    if (!packetsToRetransmit.empty()) {
        d_outgoingPacketQueue_sp->retry(packetsToRetransmit);
    }

    bool newFeedback = false;
    if (d_socketOptions.timestampOutgoingData().value_or(false)) {
        ntsa::Timestamp ts;
        while (d_feedbackQueue.tryPopFront(&ts) == 0) {
            ntsa::Notification n;
            n.makeTimestamp(ts);
            d_socketErrorQueue_sp->push_back(n);
            newFeedback = true;
        }
    }

    NTCD_SESSION_LOG_STEP_COMPLETE(d_machine_sp, this);

    if (numPacketsTransferred == 0 && !newFeedback) {
        update.dismiss();
    }

    return ntsa::Error();
}

ntsa::Handle Session::handle() const
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);
    return d_handle;
}

ntsa::Error Session::sourceEndpoint(ntsa::Endpoint* result) const
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    if (d_sourceEndpoint.isUndefined()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *result = d_sourceEndpoint;
    return ntsa::Error();
}

ntsa::Error Session::remoteEndpoint(ntsa::Endpoint* result) const
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    if (d_errorCode) {
        return this->privateError();
    }

    if (d_remoteEndpoint.isUndefined()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *result = d_remoteEndpoint;
    return ntsa::Error();
}

// *** Multicasting ***

ntsa::Error Session::setMulticastLoopback(bool enabled)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    NTCCFG_WARNING_UNUSED(enabled);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Session::setMulticastInterface(const ntsa::IpAddress& interface)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    NTCCFG_WARNING_UNUSED(interface);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Session::setMulticastTimeToLive(bsl::size_t maxHops)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    NTCCFG_WARNING_UNUSED(maxHops);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Session::joinMulticastGroup(const ntsa::IpAddress& interface,
                                        const ntsa::IpAddress& group)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    NTCCFG_WARNING_UNUSED(interface);
    NTCCFG_WARNING_UNUSED(group);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Session::leaveMulticastGroup(const ntsa::IpAddress& interface,
                                         const ntsa::IpAddress& group)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    NTCCFG_WARNING_UNUSED(interface);
    NTCCFG_WARNING_UNUSED(group);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

// *** Socket Options ***

ntsa::Error Session::setBlocking(bool blocking)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_blocking = blocking;

    return ntsa::Error();
}

ntsa::Error Session::setOption(const ntsa::SocketOption& option)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bool prevTsState =
        d_socketOptions.timestampOutgoingData().value_or(false);

    d_socketOptions.setOption(option);

    if (option.isSendBufferSize()) {
        if (d_outgoingPacketQueue_sp) {
            d_outgoingPacketQueue_sp->setHighWatermark(
                d_socketOptions.sendBufferSize().value());
        }
    }
    else if (option.isReceiveBufferSize()) {
        if (d_incomingPacketQueue_sp) {
            d_incomingPacketQueue_sp->setHighWatermark(
                d_socketOptions.receiveBufferSize().value());
        }
    }

    if ((prevTsState == false &&
         d_socketOptions.timestampOutgoingData().value_or(
             false)) ||  //TS was enabled
        (prevTsState == true &&
         !d_socketOptions.timestampOutgoingData().value_or(
             true)))  // TS was disabled
    {
        // (future note) Do not clean the error queue as it can contain not
        // only timestamps
        d_tsKey = 0;
    }

    return ntsa::Error();
}

ntsa::Error Session::getOption(ntsa::SocketOption*           option,
                               ntsa::SocketOptionType::Value type)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    if (d_handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_socketOptions.getOption(option, type);

    return ntsa::Error();
}

ntsa::Error Session::getLastError(ntsa::Error* result)
{
    *result = this->privateError();
    return ntsa::Error();
}

bsl::size_t Session::maxBuffersPerSend() const
{
    return k_MAX_BUFFERS_PER_SEND;
}

bsl::size_t Session::maxBuffersPerReceive() const
{
    return k_MAX_BUFFERS_PER_RECEIVE;
}

bsl::size_t Session::bytesReadable() const
{
    return static_cast<bsl::size_t>(d_readableBytes);
}

bsl::size_t Session::bytesWritable() const
{
    return static_cast<bsl::size_t>(d_writableBytes);
}

bool Session::isReadable() const
{
    return d_readable;
}

bool Session::isWritable() const
{
    return d_writable;
}

bool Session::hasError() const
{
    return static_cast<int>(d_errorCode.load()) != 0;
}

bool Session::hasNotification() const
{
    return d_hasNotifications;
}

/// The struct describes an entry recording a session, the user's
/// interest in events, and the readiness of events.
class Monitor::Entry
{
  public:
    ntsa::Handle                     d_handle;
    bsl::weak_ptr<ntcd::Session>     d_session_wp;
    ntca::ReactorEventTrigger::Value d_trigger;
    bool                             d_oneShot;
    bool                             d_wantReadable;
    bool                             d_haveReadable;
    bool                             d_wantWritable;
    bool                             d_haveWritable;
    bool                             d_wantError;
    bool                             d_haveError;
    bool                             d_wantNotification;
    bool                             d_haveNotification;
    ntsa::Error                      d_error;
    Monitor::EntryQueue::iterator    d_iterator;

  private:
    Entry(const Entry&);
    Entry& operator=(const Entry&);

  public:
    /// Create a new entry that references the element in the entry queue
    /// at the specified 'iterator'.
    explicit Entry(Monitor::EntryQueue::iterator iterator);

    /// Destroy this object.
    ~Entry();
};

Monitor::Entry::Entry(Monitor::EntryQueue::iterator iterator)
: d_handle(ntsa::k_INVALID_HANDLE)
, d_session_wp()
, d_trigger(ntca::ReactorEventTrigger::e_LEVEL)
, d_oneShot(false)
, d_wantReadable(false)
, d_haveReadable(false)
, d_wantWritable(false)
, d_haveWritable(false)
, d_wantError(true)
, d_haveError(false)
, d_wantNotification(true)
, d_haveNotification(false)
, d_error()
, d_iterator(iterator)
{
}

Monitor::Entry::~Entry()
{
}

bsl::size_t Monitor::process(bsl::vector<ntca::ReactorEvent>* result)
{
    NTCI_LOG_CONTEXT();

    typedef bsl::vector<EntryQueue::iterator> EntryQueueIteratorVector;

    EntryQueueIteratorVector entriesToPrioritize;
    EntryQueueIteratorVector entriesToDequeue;
    EntryQueueIteratorVector entriesToRemove;

    bsl::size_t numEvents = 0;

    if (!d_queue.empty()) {
        EntryQueue::iterator it = d_queue.begin();
        EntryQueue::iterator et = d_queue.end();

        for (; it != et; ++it) {
            bsl::shared_ptr<Entry>& entry = *it;

            bsl::shared_ptr<ntcd::Session> session =
                entry->d_session_wp.lock();
            if (!session) {
                entriesToRemove.push_back(it);
                continue;
            }

            ntsa::Handle handle = entry->d_handle;

            bool active = false;

            const bool matchReadable =
                entry->d_wantReadable && entry->d_haveReadable;

            const bool matchWritable =
                entry->d_wantWritable && entry->d_haveWritable;

            const bool matchError = entry->d_wantError && entry->d_haveError;

            const bool matchNotification =
                entry->d_wantNotification && entry->d_haveNotification;

// MRM
#if 0
            if (!matchReadable && !matchWritable && !matchError) {
                entriesToDequeue.push_back(it);
                continue;
            }
#endif

            if (matchReadable) {
                if (!session->isReadable()) {
                    continue;
                }

// MRM
#if 0
                if (entry->d_oneShot) {
                    entry->d_wantReadable = false;
                }
#endif

                ntca::ReactorContext reactorContext;
                reactorContext.setBytesWritable(session->bytesWritable());
                reactorContext.setBytesReadable(session->bytesReadable());

                ntca::ReactorEvent reactorEvent;
                reactorEvent.setType(ntca::ReactorEventType::e_READABLE);
                reactorEvent.setHandle(handle);
                reactorEvent.setContext(reactorContext);

                NTCD_MONITOR_LOG_EVENT(d_machine_sp,
                                       this,
                                       session.get(),
                                       reactorEvent);

                result->push_back(reactorEvent);
                active = true;
                ++numEvents;
            }

            if (matchWritable) {
                if (!session->isWritable()) {
                    continue;
                }

// MRM
#if 0
                if (entry->d_oneShot) {
                    entry->d_wantWritable = false;
                }
#endif

                ntca::ReactorContext reactorContext;
                reactorContext.setBytesReadable(session->bytesReadable());
                reactorContext.setBytesWritable(session->bytesWritable());

                ntca::ReactorEvent reactorEvent;
                reactorEvent.setType(ntca::ReactorEventType::e_WRITABLE);
                reactorEvent.setHandle(handle);
                reactorEvent.setContext(reactorContext);

                NTCD_MONITOR_LOG_EVENT(d_machine_sp,
                                       this,
                                       session.get(),
                                       reactorEvent);

                result->push_back(reactorEvent);
                active = true;
                ++numEvents;
            }

            if (matchError) {
                ntsa::Error lastError;
                ntsa::Error error = session->getLastError(&lastError);
                if (error || !lastError) {
                    lastError = entry->d_error;
                }

                if (!lastError) {
                    continue;
                }

                ntca::ReactorContext reactorContext;
                reactorContext.setError(lastError);

                ntca::ReactorEvent reactorEvent;
                reactorEvent.setType(ntca::ReactorEventType::e_ERROR);
                reactorEvent.setHandle(handle);
                reactorEvent.setContext(reactorContext);

                NTCD_MONITOR_LOG_EVENT(d_machine_sp,
                                       this,
                                       session.get(),
                                       reactorEvent);

                result->push_back(reactorEvent);
                active = true;
                ++numEvents;
            }
            if (matchNotification) {
                ntca::ReactorEvent reactorEvent;
                reactorEvent.setType(ntca::ReactorEventType::e_ERROR);
                reactorEvent.setHandle(handle);

                NTCD_MONITOR_LOG_EVENT(d_machine_sp,
                                       this,
                                       session.get(),
                                       reactorEvent);

                result->push_back(reactorEvent);
                active = true;
                ++numEvents;
            }

            if (active) {
                entriesToPrioritize.push_back(it);
            }
        }
    }

    if (!entriesToPrioritize.empty()) {
        EntryQueueIteratorVector::iterator it = entriesToPrioritize.begin();
        EntryQueueIteratorVector::iterator et = entriesToPrioritize.end();

        for (; it != et; ++it) {
            EntryQueue::iterator jt = *it;

            bsl::shared_ptr<Entry> entry = *jt;
            d_queue.erase(jt);
            entry->d_iterator = d_queue.insert(d_queue.end(), entry);
        }
    }

    if (!entriesToDequeue.empty()) {
        EntryQueueIteratorVector::iterator it = entriesToDequeue.begin();
        EntryQueueIteratorVector::iterator et = entriesToDequeue.end();

        for (; it != et; ++it) {
            EntryQueue::iterator jt = *it;

            bsl::shared_ptr<Entry> entry = *jt;
            entry->d_iterator            = d_queue.end();

            d_queue.erase(jt);
        }
    }

    if (!entriesToRemove.empty()) {
        EntryQueueIteratorVector::iterator it = entriesToRemove.begin();
        EntryQueueIteratorVector::iterator et = entriesToRemove.end();

        for (; it != et; ++it) {
            EntryQueue::iterator jt = *it;

            bsl::shared_ptr<Entry> entry = *jt;
            entry->d_iterator            = d_queue.end();

            d_queue.erase(jt);
            d_map.erase(entry->d_handle);
        }
    }

    return numEvents;
}

void Monitor::insertQueueEntry(const bsl::shared_ptr<Entry>& entry)
{
    NTCI_LOG_CONTEXT();

    if (entry->d_iterator != d_queue.end()) {
        return;
    }

    const bool matchReadable = entry->d_wantReadable && entry->d_haveReadable;
    const bool matchWritable = entry->d_wantWritable && entry->d_haveWritable;
    const bool matchError    = entry->d_wantError && entry->d_haveError;
    const bool matchNotification =
        entry->d_wantNotification && entry->d_haveNotification;

    if (matchReadable || matchWritable || matchError || matchNotification) {
        bool wasEmpty     = d_queue.empty();
        entry->d_iterator = d_queue.insert(d_queue.end(), entry);
        if (wasEmpty) {
            NTCD_MONITOR_LOG_READY(d_machine_sp, this);
            d_condition.broadcast();
        }
    }
}

void Monitor::removeQueueEntry(const bsl::shared_ptr<Entry>& entry)
{
    NTCI_LOG_CONTEXT();

    if (entry->d_iterator == d_queue.end()) {
        return;
    }

    const bool matchReadable = entry->d_wantReadable && entry->d_haveReadable;
    const bool matchWritable = entry->d_wantWritable && entry->d_haveWritable;
    const bool matchError    = entry->d_wantError && entry->d_haveError;
    const bool matchNotification =
        entry->d_wantNotification && entry->d_haveNotification;

    if (!matchReadable && !matchWritable && !matchError && !matchNotification)
    {
        d_queue.erase(entry->d_iterator);
        entry->d_iterator = d_queue.end();
        if (d_queue.empty()) {
            NTCD_MONITOR_LOG_IDLE(d_machine_sp, this);
        }
    }
}

Monitor::Monitor(const bsl::shared_ptr<ntcd::Machine>& machine,
                 bslma::Allocator*                     basicAllocator)
: d_mutex()
, d_condition()
, d_run(true)
, d_interrupt(0)
, d_waiters(0)
, d_map(basicAllocator)
, d_queue(basicAllocator)
, d_machine_sp(machine)
, d_trigger(ntca::ReactorEventTrigger::e_LEVEL)
, d_oneShot(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Monitor::~Monitor()
{
}

void Monitor::setTrigger(ntca::ReactorEventTrigger::Value trigger)
{
    if (!this->supportsTrigger(trigger)) {
        NTCCFG_ABORT();
    }

    d_trigger = trigger;
}

void Monitor::setOneShot(bool oneShot)
{
    if (!this->supportsOneShot(oneShot)) {
        NTCCFG_ABORT();
    }

    d_oneShot = oneShot;
}

void Monitor::registerWaiter()
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    d_waiters += 1;
}

void Monitor::deregisterWaiter()
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    d_waiters -= 1;
}

ntsa::Error Monitor::add(ntsa::Handle handle)
{
    ntsa::Error error;

    bsl::weak_ptr<ntcd::Session> session_wp;
    error = d_machine_sp->lookupSession(&session_wp, handle);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntcd::Session> session_sp = session_wp.lock();
    if (!session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return this->add(session_sp);
}

ntsa::Error Monitor::add(const bsl::shared_ptr<ntcd::Session>& session)
{
    ntsa::Handle handle = session->handle();

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    EntryMap::iterator it = d_map.find(handle);
    if (it != d_map.end()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<Entry> entry;
    entry.createInplace(d_allocator_p, d_queue.end());

    entry->d_handle     = handle;
    entry->d_session_wp = session;

    d_map.insert(EntryMap::value_type(handle, entry));

    lock.release()->unlock();

    session->registerMonitor(this->getSelf(this));

    return ntsa::Error();
}

ntsa::Error Monitor::remove(ntsa::Handle handle)
{
    ntsa::Error error;

    bsl::weak_ptr<ntcd::Session> session_wp;
    error = d_machine_sp->lookupSession(&session_wp, handle);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntcd::Session> session_sp = session_wp.lock();
    if (!session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return this->remove(session_sp);
}

ntsa::Error Monitor::remove(const bsl::shared_ptr<ntcd::Session>& session)
{
    ntsa::Handle handle = session->handle();

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    EntryMap::iterator it = d_map.find(handle);
    if (it == d_map.end()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    {
        const bsl::shared_ptr<Entry>& entry = it->second;
        if (entry->d_iterator != d_queue.end()) {
            d_queue.erase(entry->d_iterator);
        }
    }

    d_map.erase(it);

    lock.release()->unlock();

    session->deregisterMonitor(this->getSelf(this));

    return ntsa::Error();
}

ntsa::Error Monitor::update(ntsa::Handle handle, ntcs::Interest interest)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::weak_ptr<ntcd::Session> session_wp;
    error = d_machine_sp->lookupSession(&session_wp, handle);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntcd::Session> session = session_wp.lock();
    if (!session) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bool isReadable      = session->isReadable();
    const bool isWritable      = session->isWritable();
    const bool hasError        = session->hasError();
    const bool hasNotification = session->hasNotification();

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    EntryMap::iterator it = d_map.find(handle);
    if (it == d_map.end()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<Entry>& entry = it->second;

    bool processInsert = false;
    bool processRemove = false;

    if (interest.trigger() != entry->d_trigger) {
        entry->d_trigger = interest.trigger();
    }

    if (interest.oneShot() != entry->d_oneShot) {
        entry->d_oneShot = interest.oneShot();
    }

    if (interest.wantReadable()) {
        if (!entry->d_wantReadable) {
            NTCD_MONITOR_LOG_SHOW_READABLE(d_machine_sp, this, session.get());
            entry->d_wantReadable = true;

            if (entry->d_haveReadable) {
                processInsert = true;
            }
            else if (isReadable) {
                NTCD_MONITOR_LOG_ENABLE_READABLE(d_machine_sp,
                                                 this,
                                                 session.get());
                entry->d_haveReadable = true;
                processInsert         = true;
            }
        }
    }
    else {
        if (entry->d_wantReadable) {
            NTCD_MONITOR_LOG_HIDE_READABLE(d_machine_sp, this, session.get());
            entry->d_wantReadable = false;
            processRemove         = true;
        }
    }

    if (interest.wantWritable()) {
        if (!entry->d_wantWritable) {
            NTCD_MONITOR_LOG_SHOW_WRITABLE(d_machine_sp, this, session.get());
            entry->d_wantWritable = true;

            if (entry->d_haveWritable) {
                processInsert = true;
            }
            else if (isWritable) {
                NTCD_MONITOR_LOG_ENABLE_WRITABLE(d_machine_sp,
                                                 this,
                                                 session.get());
                entry->d_haveWritable = true;
                processInsert         = true;
            }
        }
    }
    else {
        if (entry->d_wantWritable) {
            NTCD_MONITOR_LOG_HIDE_WRITABLE(d_machine_sp, this, session.get());
            entry->d_wantWritable = false;
            processRemove         = true;
        }
    }

    if (interest.wantError()) {
        if (!entry->d_wantError) {
            NTCD_MONITOR_LOG_SHOW_ERROR(d_machine_sp, this, session.get());
            entry->d_wantError = true;

            if (entry->d_haveError) {
                processInsert = true;
            }
            else if (hasError) {
                NTCD_MONITOR_LOG_ENABLE_ERROR(d_machine_sp,
                                              this,
                                              session.get());
                entry->d_haveError = true;
                processInsert      = true;
            }
        }
    }
    else {
        if (entry->d_wantError) {
            NTCD_MONITOR_LOG_HIDE_ERROR(d_machine_sp, this, session.get());
            entry->d_wantError = false;
            processRemove      = true;
        }
    }

    if (interest.wantNotifications()) {
        if (!entry->d_wantNotification) {
            NTCD_MONITOR_LOG_SHOW_NOTIFICATIONS(d_machine_sp,
                                                this,
                                                session.get());
            entry->d_wantNotification = true;

            if (entry->d_haveNotification) {
                processInsert = true;
            }
            else if (hasNotification) {
                NTCD_MONITOR_LOG_ENABLE_NOTIFICATIONS(d_machine_sp,
                                                      this,
                                                      session.get());
                entry->d_haveNotification = true;
                processInsert             = true;
            }
        }
    }
    else {
        if (entry->d_wantNotification) {
            NTCD_MONITOR_LOG_HIDE_NOTIFICATIONS(d_machine_sp,
                                                this,
                                                session.get());
            entry->d_wantNotification = false;
            processRemove             = true;
        }
    }

    if (!processInsert && !processRemove) {
        return ntsa::Error();
    }

    const bool matchReadable = entry->d_wantReadable && entry->d_haveReadable;
    const bool matchWritable = entry->d_wantWritable && entry->d_haveWritable;
    const bool matchError    = entry->d_wantError && entry->d_haveError;
    const bool matchNotification =
        entry->d_wantNotification && entry->d_haveNotification;

    if (matchReadable || matchWritable || matchError || matchNotification) {
        if (entry->d_iterator == d_queue.end()) {
            bool wasEmpty     = d_queue.empty();
            entry->d_iterator = d_queue.insert(d_queue.end(), entry);
            if (wasEmpty) {
                NTCD_MONITOR_LOG_READY(d_machine_sp, this);
                d_condition.broadcast();
            }
        }
    }
    else if (!matchReadable && !matchWritable && !matchError &&
             !matchNotification)
    {
        if (entry->d_iterator != d_queue.end()) {
            d_queue.erase(entry->d_iterator);
            entry->d_iterator = d_queue.end();
            bool nowEmpty     = d_queue.empty();
            if (nowEmpty) {
                NTCD_MONITOR_LOG_IDLE(d_machine_sp, this);
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error Monitor::update(const bsl::shared_ptr<ntcd::Session>& session,
                            ntcs::Interest                        interest)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    ntsa::Handle handle = session->handle();

    const bool isReadable      = session->isReadable();
    const bool isWritable      = session->isWritable();
    const bool hasError        = session->hasError();
    const bool hasNotification = session->hasNotification();

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    EntryMap::iterator it = d_map.find(handle);
    if (it == d_map.end()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<Entry>& entry = it->second;

    bool processInsert = false;
    bool processRemove = false;

    if (interest.wantReadable()) {
        if (!entry->d_wantReadable) {
            NTCD_MONITOR_LOG_SHOW_READABLE(d_machine_sp, this, session.get());
            entry->d_wantReadable = true;

            if (entry->d_haveReadable) {
                processInsert = true;
            }
            else if (isReadable) {
                NTCD_MONITOR_LOG_ENABLE_READABLE(d_machine_sp,
                                                 this,
                                                 session.get());
                entry->d_haveReadable = true;
                processInsert         = true;
            }
        }
    }
    else {
        if (entry->d_wantReadable) {
            NTCD_MONITOR_LOG_HIDE_READABLE(d_machine_sp, this, session.get());
            entry->d_wantReadable = false;
            processRemove         = true;
        }
    }

    if (interest.wantWritable()) {
        if (!entry->d_wantWritable) {
            NTCD_MONITOR_LOG_SHOW_WRITABLE(d_machine_sp, this, session.get());
            entry->d_wantWritable = true;

            if (entry->d_haveWritable) {
                processInsert = true;
            }
            else if (isWritable) {
                NTCD_MONITOR_LOG_ENABLE_WRITABLE(d_machine_sp,
                                                 this,
                                                 session.get());
                entry->d_haveWritable = true;
                processInsert         = true;
            }
        }
    }
    else {
        if (entry->d_wantWritable) {
            NTCD_MONITOR_LOG_HIDE_WRITABLE(d_machine_sp, this, session.get());
            entry->d_wantWritable = false;
            processRemove         = true;
        }
    }

    if (interest.wantError()) {
        if (!entry->d_wantError) {
            NTCD_MONITOR_LOG_SHOW_ERROR(d_machine_sp, this, session.get());
            entry->d_wantError = true;

            if (entry->d_haveError) {
                processInsert = true;
            }
            else if (hasError) {
                NTCD_MONITOR_LOG_ENABLE_ERROR(d_machine_sp,
                                              this,
                                              session.get());
                entry->d_haveError = true;
                processInsert      = true;
            }
        }
    }
    else {
        if (entry->d_wantError) {
            NTCD_MONITOR_LOG_HIDE_ERROR(d_machine_sp, this, session.get());
            entry->d_wantError = false;
            processRemove      = true;
        }
    }

    if (interest.wantNotifications()) {
        if (!entry->d_wantNotification) {
            NTCD_MONITOR_LOG_SHOW_NOTIFICATIONS(d_machine_sp,
                                                this,
                                                session.get());
            entry->d_wantNotification = true;

            if (entry->d_haveNotification) {
                processInsert = true;
            }
            else if (hasNotification) {
                NTCD_MONITOR_LOG_ENABLE_NOTIFICATIONS(d_machine_sp,
                                                      this,
                                                      session.get());
                entry->d_haveNotification = true;
                processInsert             = true;
            }
        }
    }
    else {
        if (entry->d_wantNotification) {
            NTCD_MONITOR_LOG_HIDE_NOTIFICATIONS(d_machine_sp,
                                                this,
                                                session.get());
            entry->d_wantNotification = false;
            processRemove             = true;
        }
    }

    if (!processInsert && !processRemove) {
        return ntsa::Error();
    }

    const bool matchReadable = entry->d_wantReadable && entry->d_haveReadable;
    const bool matchWritable = entry->d_wantWritable && entry->d_haveWritable;
    const bool matchError    = entry->d_wantError && entry->d_haveError;
    const bool matchNotification =
        entry->d_wantNotification && entry->d_haveNotification;

    if (matchReadable || matchWritable || matchError || matchNotification) {
        if (entry->d_iterator == d_queue.end()) {
            bool wasEmpty     = d_queue.empty();
            entry->d_iterator = d_queue.insert(d_queue.end(), entry);
            if (wasEmpty) {
                NTCD_MONITOR_LOG_READY(d_machine_sp, this);
                d_condition.broadcast();
            }
        }
    }
    else if (!matchReadable && !matchWritable && !matchError &&
             !matchNotification)
    {
        if (entry->d_iterator != d_queue.end()) {
            d_queue.erase(entry->d_iterator);
            entry->d_iterator = d_queue.end();
            bool nowEmpty     = d_queue.empty();
            if (nowEmpty) {
                NTCD_MONITOR_LOG_IDLE(d_machine_sp, this);
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error Monitor::show(ntsa::Handle                  handle,
                          ntca::ReactorEventType::Value type)
{
    ntsa::Error error;

    bsl::weak_ptr<ntcd::Session> session_wp;
    error = d_machine_sp->lookupSession(&session_wp, handle);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntcd::Session> session_sp = session_wp.lock();
    if (!session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return this->show(session_sp, type);
}

ntsa::Error Monitor::show(const bsl::shared_ptr<ntcd::Session>& session,
                          ntca::ReactorEventType::Value         type)
{
    NTCI_LOG_CONTEXT();

    ntsa::Handle handle = session->handle();

    const bool isReadable = session->isReadable();
    const bool isWritable = session->isWritable();
    const bool hasError   = session->hasError();
    // const bool hasNotification   = session->hasNotification();

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    EntryMap::iterator it = d_map.find(handle);
    if (it == d_map.end()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<Entry>& entry = it->second;

    if (type == ntca::ReactorEventType::e_READABLE) {
        if (!entry->d_wantReadable) {
            NTCD_MONITOR_LOG_SHOW_READABLE(d_machine_sp, this, session.get());
            entry->d_wantReadable = true;

            if (entry->d_haveReadable) {
                this->insertQueueEntry(entry);
            }
            else if (isReadable) {
                NTCD_MONITOR_LOG_ENABLE_READABLE(d_machine_sp,
                                                 this,
                                                 session.get());
                entry->d_haveReadable = true;
                this->insertQueueEntry(entry);
            }
        }
    }
    else if (type == ntca::ReactorEventType::e_WRITABLE) {
        if (!entry->d_wantWritable) {
            NTCD_MONITOR_LOG_SHOW_WRITABLE(d_machine_sp, this, session.get());
            entry->d_wantWritable = true;

            if (entry->d_haveWritable) {
                this->insertQueueEntry(entry);
            }
            else if (isWritable) {
                NTCD_MONITOR_LOG_ENABLE_WRITABLE(d_machine_sp,
                                                 this,
                                                 session.get());
                entry->d_haveWritable = true;
                this->insertQueueEntry(entry);
            }
        }
    }
    else if (type == ntca::ReactorEventType::e_ERROR) {
        if (!entry->d_wantError) {
            NTCD_MONITOR_LOG_SHOW_ERROR(d_machine_sp, this, session.get());
            entry->d_wantError = true;

            if (entry->d_haveError) {
                this->insertQueueEntry(entry);
            }
            else if (hasError) {
                NTCD_MONITOR_LOG_ENABLE_ERROR(d_machine_sp,
                                              this,
                                              session.get());
                entry->d_haveError = true;
                this->insertQueueEntry(entry);
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error Monitor::hide(ntsa::Handle                  handle,
                          ntca::ReactorEventType::Value type)
{
    ntsa::Error error;

    bsl::weak_ptr<ntcd::Session> session_wp;
    error = d_machine_sp->lookupSession(&session_wp, handle);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntcd::Session> session_sp = session_wp.lock();
    if (!session_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return this->hide(session_sp, type);
}

ntsa::Error Monitor::hide(const bsl::shared_ptr<ntcd::Session>& session,
                          ntca::ReactorEventType::Value         type)
{
    NTCI_LOG_CONTEXT();

    ntsa::Handle handle = session->handle();

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    EntryMap::iterator it = d_map.find(handle);
    if (it == d_map.end()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<Entry>& entry = it->second;

    if (type == ntca::ReactorEventType::e_READABLE) {
        if (entry->d_wantReadable) {
            NTCD_MONITOR_LOG_HIDE_READABLE(d_machine_sp, this, session.get());
            entry->d_wantReadable = false;
            this->removeQueueEntry(entry);
        }
    }
    else if (type == ntca::ReactorEventType::e_WRITABLE) {
        if (entry->d_wantWritable) {
            NTCD_MONITOR_LOG_HIDE_WRITABLE(d_machine_sp, this, session.get());
            entry->d_wantWritable = false;
            this->removeQueueEntry(entry);
        }
    }
    else if (type == ntca::ReactorEventType::e_ERROR) {
        if (entry->d_wantError) {
            NTCD_MONITOR_LOG_HIDE_ERROR(d_machine_sp, this, session.get());
            entry->d_wantError = false;
            this->removeQueueEntry(entry);
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Monitor::enable(ntsa::Handle                          handle,
                            const bsl::shared_ptr<ntcd::Session>& session,
                            ntca::ReactorEventType::Value         type)
{
    NTCI_LOG_CONTEXT();

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    EntryMap::iterator it = d_map.find(handle);
    if (it == d_map.end()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<Entry>& entry = it->second;

    if (type == ntca::ReactorEventType::e_READABLE) {
        if (!entry->d_haveReadable) {
            NTCD_MONITOR_LOG_ENABLE_READABLE(d_machine_sp,
                                             this,
                                             session.get());
            entry->d_haveReadable = true;
            this->insertQueueEntry(entry);
        }
    }
    else if (type == ntca::ReactorEventType::e_WRITABLE) {
        if (!entry->d_haveWritable) {
            NTCD_MONITOR_LOG_ENABLE_WRITABLE(d_machine_sp,
                                             this,
                                             session.get());
            entry->d_haveWritable = true;
            this->insertQueueEntry(entry);
        }
    }
    else if (type == ntca::ReactorEventType::e_ERROR) {
        if (!entry->d_haveError) {
            NTCD_MONITOR_LOG_ENABLE_ERROR(d_machine_sp, this, session.get());
            entry->d_haveError = true;
            this->insertQueueEntry(entry);
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Monitor::enableNotifications(
    ntsa::Handle                          handle,
    const bsl::shared_ptr<ntcd::Session>& session)
{
    NTCI_LOG_CONTEXT();

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    EntryMap::iterator it = d_map.find(handle);
    if (it == d_map.end()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<Entry>& entry = it->second;

    if (!entry->d_haveNotification) {
        NTCD_MONITOR_LOG_ENABLE_NOTIFICATIONS(d_machine_sp,
                                              this,
                                              session.get());
        entry->d_haveNotification = true;

        this->insertQueueEntry(entry);
    }

    return ntsa::Error();
}

ntsa::Error Monitor::disable(ntsa::Handle                          handle,
                             const bsl::shared_ptr<ntcd::Session>& session,
                             ntca::ReactorEventType::Value         type)
{
    NTCI_LOG_CONTEXT();

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    EntryMap::iterator it = d_map.find(handle);
    if (it == d_map.end()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<Entry>& entry = it->second;

    if (type == ntca::ReactorEventType::e_READABLE) {
        if (entry->d_haveReadable) {
            NTCD_MONITOR_LOG_DISABLE_READABLE(d_machine_sp,
                                              this,
                                              session.get());
            entry->d_haveReadable = false;
            this->removeQueueEntry(entry);
        }
    }
    else if (type == ntca::ReactorEventType::e_WRITABLE) {
        if (entry->d_haveWritable) {
            NTCD_MONITOR_LOG_DISABLE_WRITABLE(d_machine_sp,
                                              this,
                                              session.get());
            entry->d_haveWritable = false;
            this->removeQueueEntry(entry);
        }
    }
    else if (type == ntca::ReactorEventType::e_ERROR) {
        if (entry->d_haveError) {
            NTCD_MONITOR_LOG_DISABLE_ERROR(d_machine_sp, this, session.get());
            entry->d_haveError = false;
            this->removeQueueEntry(entry);
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Monitor::disableNotifications(
    ntsa::Handle                          handle,
    const bsl::shared_ptr<ntcd::Session>& session)
{
    NTCI_LOG_CONTEXT();

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    EntryMap::iterator it = d_map.find(handle);
    if (it == d_map.end()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<Entry>& entry = it->second;

    if (entry->d_haveNotification) {
        NTCD_MONITOR_LOG_DISABLE_ERROR(d_machine_sp, this, session.get());
        entry->d_haveNotification = false;
        this->removeQueueEntry(entry);
    }

    return ntsa::Error();
}

ntsa::Error Monitor::dequeue(bsl::vector<ntca::ReactorEvent>* result)
{
    NTCI_LOG_CONTEXT();

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    while (d_run && d_queue.empty() && d_interrupt == 0) {
        NTCD_MONITOR_LOG_WAITING(d_machine_sp, this);
        int waitResult = d_condition.wait(&d_mutex);
        if (waitResult == 0) {
            break;
        }
        else {
            ntsa::Error lastError = ntsa::Error::last();
            if (lastError) {
                return lastError;
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
    }

    while (true) {
        bsl::uint64_t interrupt = d_interrupt.load();
        if (interrupt == 0) {
            break;
        }

        if (interrupt == d_interrupt.testAndSwap(interrupt, interrupt - 1)) {
            break;
        }
    }

    if (!d_queue.empty()) {
        NTCD_MONITOR_LOG_PROCESS_STARTING(d_machine_sp, this, d_queue.size());
        bsl::size_t numEvents = this->process(result);
        NTCD_MONITOR_LOG_PROCESS_COMPLETE(d_machine_sp, this, numEvents);
    }
    else {
        NTCD_MONITOR_LOG_INTERRUPTION(d_machine_sp, this);
    }

    return ntsa::Error();
}

ntsa::Error Monitor::dequeue(bsl::vector<ntca::ReactorEvent>* result,
                             const bsls::TimeInterval&        timeout)
{
    NTCI_LOG_CONTEXT();

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    while (d_run && d_queue.empty() && d_interrupt == 0) {
        NTCD_MONITOR_LOG_WAITING(d_machine_sp, this);
        int waitResult = d_condition.timedWait(&d_mutex, timeout);
        if (waitResult == 0) {
            break;
        }
        else if (waitResult == bslmt::Condition::e_TIMED_OUT) {
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }
        else {
            ntsa::Error lastError = ntsa::Error::last();
            if (lastError) {
                return lastError;
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
    }

    while (true) {
        bsl::uint64_t interrupt = d_interrupt.load();
        if (interrupt == 0) {
            break;
        }

        if (interrupt == d_interrupt.testAndSwap(interrupt, interrupt - 1)) {
            break;
        }
    }

    if (!d_queue.empty()) {
        NTCD_MONITOR_LOG_PROCESS_STARTING(d_machine_sp, this, d_queue.size());
        bsl::size_t numEvents = this->process(result);
        NTCD_MONITOR_LOG_PROCESS_COMPLETE(d_machine_sp, this, numEvents);
    }
    else {
        NTCD_MONITOR_LOG_INTERRUPTION(d_machine_sp, this);
    }

    return ntsa::Error();
}

void Monitor::interruptOne()
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    bsl::uint64_t interrupt = d_interrupt.load();
    bsl::uint64_t waiters   = d_waiters.load();

    if (interrupt < waiters) {
        d_interrupt += 1;
        d_condition.signal();
    }
}

void Monitor::interruptAll()
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    bsl::uint64_t interrupt = d_interrupt.load();
    bsl::uint64_t waiters   = d_waiters.load();

    if (interrupt < waiters) {
        bsl::uint64_t difference  = waiters - interrupt;
        d_interrupt              += difference;
        d_condition.broadcast();
    }
}

void Monitor::stop()
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    d_run = false;

    bsl::uint64_t interrupt = d_interrupt.load();
    bsl::uint64_t waiters   = d_waiters.load();

    if (interrupt < waiters) {
        bsl::uint64_t difference  = waiters - interrupt;
        d_interrupt              += difference;
        d_condition.broadcast();
    }
}

void Monitor::restart()
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    d_run = true;
    d_condition.broadcast();
}

bool Monitor::supportsOneShot(bool oneShot) const
{
    NTCCFG_WARNING_UNUSED(oneShot);
    return true;
}

bool Monitor::supportsTrigger(ntca::ReactorEventTrigger::Value trigger) const
{
    if (trigger == ntca::ReactorEventTrigger::e_LEVEL) {
        return true;
    }
    else {
        return false;
    }
}

Machine::Machine(bslma::Allocator* basicAllocator)
: d_mutex()
, d_condition()
, d_name("localhost", basicAllocator)
, d_ipAddressList(basicAllocator)
, d_blobBufferFactory(k_DEFAULT_BLOB_BUFFER_SIZE, basicAllocator)
, d_sessionByHandleMap(basicAllocator)
, d_tcpPortMap(basicAllocator)
, d_udpPortMap(basicAllocator)
, d_sessionByTcpEndpointMap(basicAllocator)
, d_sessionByUdpEndpointMap(basicAllocator)
, d_sessionByLocalEndpointMap(basicAllocator)
, d_sessionByTcpBindingMap(basicAllocator)
, d_sessionByUdpBindingMap(basicAllocator)
, d_sessionByLocalBindingMap(basicAllocator)
, d_threadGroup(basicAllocator)
, d_stop(false)
, d_update(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_ipAddressList.push_back(ntsa::IpAddress::loopbackIpv4());
    d_ipAddressList.push_back(ntsa::IpAddress::loopbackIpv6());
}

Machine::~Machine()
{
}

ntsa::Error Machine::acquireHandle(
    ntsa::Handle*                         result,
    ntsa::Transport::Value                transport,
    const bsl::shared_ptr<ntcd::Session>& session)
{
    NTCCFG_WARNING_UNUSED(transport);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    *result = ntsa::k_INVALID_HANDLE;

    ntsa::Handle handle = ntsa::k_INVALID_HANDLE;

    if (d_sessionByHandleMap.empty()) {
        handle = k_MIN_HANDLE;
    }
    else {
        bool found = false;
        for (SessionByHandleMap::const_iterator it =
                 d_sessionByHandleMap.begin();
             it != d_sessionByHandleMap.end();
             ++it)
        {
            ntsa::Handle candidateHandle = it->first;

            if (handle == ntsa::k_INVALID_HANDLE) {
                handle = candidateHandle;
            }
            else if (candidateHandle > handle + 1) {
                handle = handle + 1;
                found  = true;
                break;
            }
            else {
                handle = candidateHandle;
            }
        }

        if (!found) {
            SessionByHandleMap::const_iterator it = d_sessionByHandleMap.end();
            --it;
            handle = it->first + 1;
        }
    }

    if (handle > k_MAX_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!d_sessionByHandleMap
             .insert(SessionByHandleMap::value_type(
                 handle,
                 bsl::weak_ptr<ntcd::Session>(session)))
             .second)
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *result = handle;

    return ntsa::Error();
}

ntsa::Error Machine::releaseHandle(ntsa::Handle           handle,
                                   ntsa::Transport::Value transport)
{
    NTCCFG_WARNING_UNUSED(transport);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    ntsa::Error error;

    if (handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    SessionByHandleMap::iterator it = d_sessionByHandleMap.find(handle);
    if (it == d_sessionByHandleMap.end()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_sessionByHandleMap.erase(it);

    return ntsa::Error();
}

ntsa::Error Machine::acquirePort(ntsa::Port*            result,
                                 ntsa::Port             port,
                                 ntsa::Transport::Value transport)
{
    ntsa::TransportProtocol::Value protocol =
        ntsa::Transport::getProtocol(transport);

    if (protocol == ntsa::TransportProtocol::e_TCP) {
        return d_tcpPortMap.acquire(result, port);
    }
    else if (protocol == ntsa::TransportProtocol::e_UDP) {
        return d_udpPortMap.acquire(result, port);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Machine::releasePort(ntsa::Port             port,
                                 ntsa::Transport::Value transport)
{
    ntsa::TransportProtocol::Value protocol =
        ntsa::Transport::getProtocol(transport);

    if (protocol == ntsa::TransportProtocol::e_TCP) {
        d_tcpPortMap.release(port);
    }
    else if (protocol == ntsa::TransportProtocol::e_UDP) {
        d_udpPortMap.release(port);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Machine::acquireSourceEndpoint(
    ntsa::Endpoint*                       result,
    const ntsa::Endpoint&                 sourceEndpoint,
    ntsa::Transport::Value                transport,
    const bsl::shared_ptr<ntcd::Session>& session)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    ntsa::Error error;

    result->reset();

    ntsa::TransportProtocol::Value protocol =
        ntsa::Transport::getProtocol(transport);

    if (protocol == ntsa::TransportProtocol::e_TCP) {
        if (!sourceEndpoint.isIp()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        ntsa::IpAddress ipAddress;
        {
            bsl::vector<ntsa::IpAddress>::const_iterator it =
                bsl::find(d_ipAddressList.begin(),
                          d_ipAddressList.end(),
                          sourceEndpoint.ip().host());

            if (it == d_ipAddressList.end()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            ipAddress = *it;
        }

        ntsa::Port port = 0;
        error = d_tcpPortMap.acquire(&port, sourceEndpoint.ip().port());
        if (error) {
            return error;
        }

        ntsa::Endpoint effectiveSourceEndpoint =
            ntsa::Endpoint(ntsa::IpEndpoint(ipAddress, port));

        if (!d_sessionByTcpEndpointMap
                 .insert(SessionByEndpointMap::value_type(
                     effectiveSourceEndpoint,
                     bsl::weak_ptr<ntcd::Session>(session)))
                 .second)
        {
            d_tcpPortMap.release(port);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *result = effectiveSourceEndpoint;
    }
    else if (protocol == ntsa::TransportProtocol::e_UDP) {
        if (!sourceEndpoint.isIp()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        ntsa::IpAddress ipAddress;
        {
            bsl::vector<ntsa::IpAddress>::const_iterator it =
                bsl::find(d_ipAddressList.begin(),
                          d_ipAddressList.end(),
                          sourceEndpoint.ip().host());

            if (it == d_ipAddressList.end()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            ipAddress = *it;
        }

        ntsa::Port port = 0;
        error = d_udpPortMap.acquire(&port, sourceEndpoint.ip().port());
        if (error) {
            return error;
        }

        ntsa::Endpoint effectiveSourceEndpoint =
            ntsa::Endpoint(ntsa::IpEndpoint(ipAddress, port));

        if (!d_sessionByUdpEndpointMap
                 .insert(SessionByEndpointMap::value_type(
                     effectiveSourceEndpoint,
                     bsl::weak_ptr<ntcd::Session>(session)))
                 .second)
        {
            d_udpPortMap.release(port);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *result = effectiveSourceEndpoint;
    }
    else if (protocol == ntsa::TransportProtocol::e_LOCAL) {
        if (!sourceEndpoint.isLocal()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (sourceEndpoint.local().value().length() == 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (!d_sessionByLocalEndpointMap
                 .insert(SessionByEndpointMap::value_type(
                     sourceEndpoint,
                     bsl::weak_ptr<ntcd::Session>(session)))
                 .second)
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *result = sourceEndpoint;
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Machine::releaseSourceEndpoint(
    const ntsa::Endpoint&  sourceEndpoint,
    ntsa::Transport::Value transport)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    ntsa::Error error;

    ntsa::TransportProtocol::Value protocol =
        ntsa::Transport::getProtocol(transport);

    if (!error && !sourceEndpoint.isUndefined()) {
        bsl::size_t n;
        if (protocol == ntsa::TransportProtocol::e_TCP) {
            if (!sourceEndpoint.isIp()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            n = d_sessionByTcpEndpointMap.erase(sourceEndpoint);
            if (n != 1) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            d_tcpPortMap.release(sourceEndpoint.ip().port());
        }
        else if (protocol == ntsa::TransportProtocol::e_UDP) {
            if (!sourceEndpoint.isIp()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            n = d_sessionByUdpEndpointMap.erase(sourceEndpoint);
            if (n != 1) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            d_udpPortMap.release(sourceEndpoint.ip().port());
        }
        else if (protocol == ntsa::TransportProtocol::e_LOCAL) {
            if (!sourceEndpoint.isLocal()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            n = d_sessionByLocalEndpointMap.erase(sourceEndpoint);
            if (n != 1) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error Machine::acquireBinding(
    ntcd::Binding*                        result,
    const ntcd::Binding&                  binding,
    ntsa::Transport::Value                transport,
    const bsl::shared_ptr<ntcd::Session>& session)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    ntsa::Error error;

    result->reset();

    ntsa::TransportProtocol::Value protocol =
        ntsa::Transport::getProtocol(transport);

    if (protocol == ntsa::TransportProtocol::e_TCP) {
        if (!binding.sourceEndpoint().isIp()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (!binding.remoteEndpoint().isUndefined() &&
            !binding.remoteEndpoint().isIp())
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        ntsa::IpAddress ipAddress;
        {
            bsl::vector<ntsa::IpAddress>::const_iterator it =
                bsl::find(d_ipAddressList.begin(),
                          d_ipAddressList.end(),
                          binding.sourceEndpoint().ip().host());

            if (it == d_ipAddressList.end()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            ipAddress = *it;
        }

        ntsa::Port port = 0;
        error =
            d_tcpPortMap.acquire(&port, binding.sourceEndpoint().ip().port());
        if (error) {
            return error;
        }

        ntsa::Endpoint effectiveSourceEndpoint =
            ntsa::Endpoint(ntsa::IpEndpoint(ipAddress, port));

        ntsa::Endpoint effectiveRemoteEndpoint = binding.remoteEndpoint();

        ntcd::Binding effectiveBinding(effectiveSourceEndpoint,
                                       effectiveRemoteEndpoint);

        if (!d_sessionByTcpBindingMap
                 .insert(SessionByBindingMap::value_type(
                     effectiveBinding,
                     bsl::weak_ptr<ntcd::Session>(session)))
                 .second)
        {
            d_tcpPortMap.release(port);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *result = effectiveBinding;
    }
    else if (protocol == ntsa::TransportProtocol::e_UDP) {
        if (!binding.sourceEndpoint().isIp()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (!binding.remoteEndpoint().isUndefined() &&
            !binding.remoteEndpoint().isIp())
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        ntsa::IpAddress ipAddress;
        {
            bsl::vector<ntsa::IpAddress>::const_iterator it =
                bsl::find(d_ipAddressList.begin(),
                          d_ipAddressList.end(),
                          binding.sourceEndpoint().ip().host());

            if (it == d_ipAddressList.end()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            ipAddress = *it;
        }

        ntsa::Port port = 0;
        error =
            d_udpPortMap.acquire(&port, binding.sourceEndpoint().ip().port());
        if (error) {
            return error;
        }

        ntsa::Endpoint effectiveSourceEndpoint =
            ntsa::Endpoint(ntsa::IpEndpoint(ipAddress, port));

        ntsa::Endpoint effectiveRemoteEndpoint = binding.remoteEndpoint();

        ntcd::Binding effectiveBinding(effectiveSourceEndpoint,
                                       effectiveRemoteEndpoint);

        if (!d_sessionByUdpBindingMap
                 .insert(SessionByBindingMap::value_type(
                     effectiveBinding,
                     bsl::weak_ptr<ntcd::Session>(session)))
                 .second)
        {
            d_udpPortMap.release(port);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *result = effectiveBinding;
    }
    else if (protocol == ntsa::TransportProtocol::e_LOCAL) {
        if (!binding.sourceEndpoint().isLocal()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (!binding.remoteEndpoint().isUndefined() &&
            !binding.remoteEndpoint().isLocal())
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (binding.sourceEndpoint().local().value().length() == 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (!d_sessionByLocalBindingMap
                 .insert(SessionByBindingMap::value_type(
                     binding,
                     bsl::weak_ptr<ntcd::Session>(session)))
                 .second)
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *result = binding;
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Machine::releaseBinding(const ntcd::Binding&   binding,
                                    ntsa::Transport::Value transport)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    ntsa::TransportProtocol::Value protocol =
        ntsa::Transport::getProtocol(transport);

    if (protocol == ntsa::TransportProtocol::e_TCP) {
        SessionByBindingMap::iterator it =
            d_sessionByTcpBindingMap.find(binding);

        if (it == d_sessionByTcpBindingMap.end()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        d_sessionByTcpBindingMap.erase(it);
    }
    else if (protocol == ntsa::TransportProtocol::e_UDP) {
        SessionByBindingMap::iterator it =
            d_sessionByUdpBindingMap.find(binding);

        if (it == d_sessionByUdpBindingMap.end()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        d_sessionByTcpBindingMap.erase(it);
    }
    else if (protocol == ntsa::TransportProtocol::e_LOCAL) {
        SessionByBindingMap::iterator it =
            d_sessionByLocalBindingMap.find(binding);

        if (it == d_sessionByLocalBindingMap.end()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        d_sessionByLocalBindingMap.erase(it);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

bsl::shared_ptr<ntcd::Session> Machine::createSession(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::Session> session;
    session.createInplace(allocator, this->getSelf(this), allocator);

    return session;
}

bsl::shared_ptr<ntcd::Monitor> Machine::createMonitor(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::Monitor> monitor;
    monitor.createInplace(allocator, this->getSelf(this), allocator);

    return monitor;
}

bsl::shared_ptr<ntcd::Packet> Machine::createPacket(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::Packet> packet;
    packet.createInplace(allocator, &d_blobBufferFactory, allocator);

    return packet;
}

void Machine::update(const bsl::shared_ptr<ntcd::Session>& session)
{
    NTCCFG_WARNING_UNUSED(session);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    bool d_alreadyNeedsUpdate = d_update.swap(true);
    if (!d_alreadyNeedsUpdate) {
        d_condition.broadcast();
    }
}

void Machine::updateNoLock(const bsl::shared_ptr<ntcd::Session>& session)
{
    NTCCFG_WARNING_UNUSED(session);

    bool d_alreadyNeedsUpdate = d_update.swap(true);
    if (!d_alreadyNeedsUpdate) {
        d_condition.broadcast();
    }
}

ntsa::Error Machine::run()
{
    bslmt::ThreadAttributes threadAttributes;
    threadAttributes.setThreadName("machine");

    int rc = d_threadGroup.addThread(
        bdlf::MemFnUtil::memFn(&Machine::execute, this),
        threadAttributes);

    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

void Machine::execute()
{
    while (!d_stop) {
        this->step(true);
    }
}

ntsa::Error Machine::step(bool block)
{
    NTCI_LOG_CONTEXT();

    NTCD_MACHINE_LOG_STEP_STARTING();

    ntsa::Error error;

    typedef bsl::vector<SessionByHandleMap::value_type> SessionVector;

    SessionVector sessions;
    {
        ntccfg::ConditionMutexGuard lock(&d_mutex);

        while (true) {
            bool needsUpdate = d_update.testAndSwap(true, false);
            if (needsUpdate) {
                break;
            }

            if (block) {
                d_condition.wait(&d_mutex);
            }
            else {
                return ntsa::Error();
            }
        }

        sessions.reserve(d_sessionByHandleMap.size());

        for (SessionByHandleMap::iterator it = d_sessionByHandleMap.begin();
             it != d_sessionByHandleMap.end();
             ++it)
        {
            sessions.push_back(*it);
        }
    }

    for (SessionVector::iterator it = sessions.begin(); it != sessions.end();
         ++it)
    {
        const bsl::weak_ptr<ntcd::Session>& session_wp = it->second;

        bsl::shared_ptr<ntcd::Session> session = session_wp.lock();
        if (!session) {
            continue;
        }

        error = session->step(false);
        if (error) {
            NTCD_MACHINE_LOG_STEP_FAILED(error);
            return error;
        }
    }

    NTCD_MACHINE_LOG_STEP_COMPLETE();

    return ntsa::Error();
}

void Machine::stop()
{
    d_stop   = true;
    d_update = true;

    d_condition.broadcast();

    d_threadGroup.joinAll();
}

ntsa::Error Machine::lookupSession(bsl::weak_ptr<ntcd::Session>* result,
                                   ntsa::Handle                  handle) const
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    SessionByHandleMap::const_iterator it = d_sessionByHandleMap.find(handle);
    if (it == d_sessionByHandleMap.end()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *result = it->second;

    return ntsa::Error();
}

ntsa::Error Machine::lookupSession(bsl::weak_ptr<ntcd::Session>* result,
                                   const ntsa::Endpoint&  sourceEndpoint,
                                   ntsa::Transport::Value transport) const
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    ntsa::TransportProtocol::Value protocol =
        ntsa::Transport::getProtocol(transport);

    if (protocol == ntsa::TransportProtocol::e_TCP) {
        SessionByEndpointMap::const_iterator it =
            d_sessionByTcpEndpointMap.find(sourceEndpoint);

        if (it == d_sessionByTcpEndpointMap.end()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *result = it->second;
    }
    else if (protocol == ntsa::TransportProtocol::e_UDP) {
        SessionByEndpointMap::const_iterator it =
            d_sessionByUdpEndpointMap.find(sourceEndpoint);

        if (it == d_sessionByUdpEndpointMap.end()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *result = it->second;
    }
    else if (protocol == ntsa::TransportProtocol::e_LOCAL) {
        SessionByEndpointMap::const_iterator it =
            d_sessionByLocalEndpointMap.find(sourceEndpoint);

        if (it == d_sessionByLocalEndpointMap.end()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *result = it->second;
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Machine::lookupSession(bsl::weak_ptr<ntcd::Session>* result,
                                   const ntsa::Endpoint&  sourceEndpoint,
                                   const ntsa::Endpoint&  remoteEndpoint,
                                   ntsa::Transport::Value transport) const
{
    return this->lookupSession(result,
                               ntcd::Binding(sourceEndpoint, remoteEndpoint),
                               transport);
}

ntsa::Error Machine::lookupSession(bsl::weak_ptr<ntcd::Session>* result,
                                   const ntcd::Binding&          binding,
                                   ntsa::Transport::Value transport) const
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    result->reset();

    ntsa::TransportProtocol::Value protocol =
        ntsa::Transport::getProtocol(transport);

    if (protocol == ntsa::TransportProtocol::e_TCP) {
        SessionByBindingMap::const_iterator it =
            d_sessionByTcpBindingMap.find(binding);

        if (it == d_sessionByTcpBindingMap.end()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *result = it->second;
    }
    else if (protocol == ntsa::TransportProtocol::e_UDP) {
        SessionByBindingMap::const_iterator it =
            d_sessionByUdpBindingMap.find(binding);

        if (it == d_sessionByUdpBindingMap.end()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *result = it->second;
    }
    else if (protocol == ntsa::TransportProtocol::e_LOCAL) {
        SessionByBindingMap::const_iterator it =
            d_sessionByLocalBindingMap.find(binding);

        if (it == d_sessionByLocalBindingMap.end()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *result = it->second;
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

const bsl::string& Machine::name() const
{
    return d_name;
}

const bsl::vector<ntsa::IpAddress>& Machine::ipAddressList() const
{
    return d_ipAddressList;
}

void Machine::discoverAdapterList(bsl::vector<ntsa::Adapter>* result) const
{
    bsl::vector<ntsa::Ipv4Address> ipv4AddressList;
    bsl::vector<ntsa::Ipv4Address> ipv4LoopbackAddressList;
    bsl::vector<ntsa::Ipv6Address> ipv6AddressList;
    bsl::vector<ntsa::Ipv6Address> ipv6LoopbackAddressList;

    for (bsl::size_t i = 0; i < d_ipAddressList.size(); ++i) {
        const ntsa::IpAddress& ipAddress = d_ipAddressList[i];

        if (ipAddress.isUndefined()) {
            continue;
        }
        else if (ipAddress.isV4()) {
            if (ipAddress.v4().isLoopback()) {
                ipv4LoopbackAddressList.push_back(ipAddress.v4());
            }
            else {
                ipv4AddressList.push_back(ipAddress.v4());
            }
        }
        else if (ipAddress.isV6()) {
            if (ipAddress.v6().isLoopback()) {
                ipv6LoopbackAddressList.push_back(ipAddress.v6());
            }
            else {
                ipv6AddressList.push_back(ipAddress.v6());
            }
        }
    }

    for (bsl::size_t i = 0; i < ipv4AddressList.size(); ++i) {
        const ntsa::Ipv4Address& ipv4Address = ipv4AddressList[i];

        bsl::string name;
        {
            bsl::stringstream ss;
            ss << "simulation-ipv4-" << d_name;
            if (ipv4AddressList.size() > 1) {
                ss << "-" << i + 1;
            }
            name = ss.str();
        }

        ntsa::Adapter adapter;
        adapter.setName(name);
        adapter.setIndex(NTCCFG_WARNING_NARROW(bsl::uint32_t, result->size()));
        adapter.setIpv4Address(ipv4Address);
        adapter.setMulticast(false);

        result->push_back(adapter);
    }

    for (bsl::size_t i = 0; i < ipv6AddressList.size(); ++i) {
        const ntsa::Ipv6Address& ipv6Address = ipv6AddressList[i];

        bsl::string name;
        {
            bsl::stringstream ss;
            ss << "simulation-ipv6-" << d_name;
            if (ipv6AddressList.size() > 1) {
                ss << "-" << i + 1;
            }
            name = ss.str();
        }

        ntsa::Adapter adapter;
        adapter.setName(name);
        adapter.setIndex(NTCCFG_WARNING_NARROW(bsl::uint32_t, result->size()));
        adapter.setIpv6Address(ipv6Address);
        adapter.setMulticast(false);

        result->push_back(adapter);
    }

    for (bsl::size_t i = 0; i < ipv4LoopbackAddressList.size(); ++i) {
        const ntsa::Ipv4Address& ipv4LoopbackAddress =
            ipv4LoopbackAddressList[i];

        bsl::string name;
        {
            bsl::stringstream ss;
            ss << "simulation-ipv4-" << d_name << "-loopback";
            if (ipv4LoopbackAddressList.size() > 1) {
                ss << "-" << i + 1;
            }
            name = ss.str();
        }

        ntsa::Adapter adapter;
        adapter.setName(name);
        adapter.setIndex(NTCCFG_WARNING_NARROW(bsl::uint32_t, result->size()));
        adapter.setIpv4Address(ipv4LoopbackAddress);
        adapter.setMulticast(false);

        result->push_back(adapter);
    }

    for (bsl::size_t i = 0; i < ipv6LoopbackAddressList.size(); ++i) {
        const ntsa::Ipv6Address& ipv6LoopbackAddress =
            ipv6LoopbackAddressList[i];

        bsl::string name;
        {
            bsl::stringstream ss;
            ss << "simulation-ipv6-" << d_name << "-loopback";
            if (ipv6LoopbackAddressList.size() > 1) {
                ss << "-" << i + 1;
            }
            name = ss.str();
        }

        ntsa::Adapter adapter;
        adapter.setName(name);
        adapter.setIndex(NTCCFG_WARNING_NARROW(bsl::uint32_t, result->size()));
        adapter.setIpv6Address(ipv6LoopbackAddress);
        adapter.setMulticast(false);

        result->push_back(adapter);
    }
}

bool Machine::discoverAdapter(ntsa::Adapter*             result,
                              ntsa::IpAddressType::Value addressType,
                              bool                       multicast) const
{
    bsl::vector<ntsa::Adapter> adapterList;
    this->discoverAdapterList(&adapterList);
    for (bsl::vector<ntsa::Adapter>::const_iterator it = adapterList.begin();
         it != adapterList.end();
         ++it)
    {
        const ntsa::Adapter& candidateAdapter = *it;

        if (addressType == ntsa::IpAddressType::e_V4) {
            if (!candidateAdapter.ipv4Address().isNull()) {
                if (multicast && !candidateAdapter.multicast()) {
                    continue;
                }

                *result = candidateAdapter;
                return true;
            }
        }
        else if (addressType == ntsa::IpAddressType::e_V6) {
            if (!candidateAdapter.ipv6Address().isNull()) {
                if (multicast && !candidateAdapter.multicast()) {
                    continue;
                }

                *result = candidateAdapter;
                return true;
            }
        }
    }

    return false;
}

bool Machine::hasIpAddress(const ntsa::IpAddress& ipAddress) const
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    bsl::vector<ntsa::IpAddress>::const_iterator it =
        bsl::find(d_ipAddressList.begin(), d_ipAddressList.end(), ipAddress);

    if (it == d_ipAddressList.end()) {
        return false;
    }

    return true;
}

bsl::shared_ptr<ntci::Resolver> Machine::resolver() const
{
    return bsl::shared_ptr<ntci::Resolver>();
}

bsl::shared_ptr<ntcd::Machine> Machine::initialize()
{
    bsls::SpinLockGuard lock(&s_defaultMachineLock);

    if (s_defaultMachine_p) {
        s_defaultMachineRep_p->acquireRef();
        return bsl::shared_ptr<ntcd::Machine>(s_defaultMachine_p,
                                              s_defaultMachineRep_p);
    }

    bslma::Allocator* allocator = bslma::Default::globalAllocator();

    bsl::shared_ptr<ntcd::Machine> machine;
    machine.createInplace(allocator, allocator);

    bsl::pair<ntcd::Machine*, bslma::SharedPtrRep*> pair = machine.release();

    s_defaultMachine_p    = pair.first;
    s_defaultMachineRep_p = pair.second;

    return bsl::shared_ptr<ntcd::Machine>(s_defaultMachine_p,
                                          s_defaultMachineRep_p);
}

void Machine::setDefault(const bsl::shared_ptr<ntcd::Machine>& machine)
{
    bsls::SpinLockGuard lock(&s_defaultMachineLock);

    if (s_defaultMachine_p) {
        s_defaultMachineRep_p->releaseRef();
        s_defaultMachineRep_p = 0;
        s_defaultMachine_p    = 0;
    }

    if (machine) {
        s_defaultMachine_p    = machine.get();
        s_defaultMachineRep_p = machine.rep();
        s_defaultMachineRep_p->acquireRef();
    }
}

bsl::shared_ptr<ntcd::Machine> Machine::getDefault()
{
    bsls::SpinLockGuard lock(&s_defaultMachineLock);

    if (s_defaultMachine_p) {
        s_defaultMachineRep_p->acquireRef();
        return bsl::shared_ptr<ntcd::Machine>(s_defaultMachine_p,
                                              s_defaultMachineRep_p);
    }
    else {
        return bsl::shared_ptr<ntcd::Machine>();
    }
}

void Machine::exit()
{
    if (s_defaultMachine_p) {
        s_defaultMachineRep_p->releaseRef();
        s_defaultMachineRep_p = 0;
        s_defaultMachine_p    = 0;
    }
}

}  // close package namespace
}  // close enterprise namespace
