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

#include <ntcf_system.h>

#include <ntccfg_bind.h>
#include <ntccfg_test.h>
#include <ntcd_datautil.h>
#include <ntci_log.h>
#include <ntcs_blobutil.h>
#include <ntcs_datapool.h>
#include <ntcs_ratelimiter.h>
#include <ntsa_adapter.h>
#include <ntsa_endpoint.h>
#include <ntsa_ipaddress.h>
#include <ntsa_ipendpoint.h>
#include <ntsa_ipv4address.h>
#include <ntsa_ipv4endpoint.h>
#include <ntsa_ipv6address.h>
#include <ntsa_ipv6endpoint.h>
#include <ntsf_system.h>
#include <ntsi_datagramsocket.h>
#include <ntsi_streamsocket.h>
#include <ntsu_adapterutil.h>
#include <bdlb_bigendian.h>
#include <bdlb_guid.h>
#include <bdlb_guidutil.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobstreambuf.h>
#include <bdlbb_blobutil.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bdlbb_simpleblobbufferfactory.h>
#include <bdlcc_objectcatalog.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bdlmt_eventscheduler.h>
#include <bdls_processutil.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslmt_barrier.h>
#include <bslmt_latch.h>
#include <bslmt_lockguard.h>
#include <bslmt_semaphore.h>
#include <bslx_genericinstream.h>
#include <bslx_genericoutstream.h>
#include <bsl_algorithm.h>
#include <bsl_cctype.h>
#include <bsl_cstdlib.h>
#include <bsl_fstream.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <balst_stacktracetestallocator.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
// TransferClient and server agree on the number and size of each message that the
// client will be sending to the server on each connection.
//
// TransferClient sends each message, backing off if and when the high watermark has
// been breached, and resuming when the low watermark has been reached.
//
// TransferServer receives each message, and when it has received all expected messages
// shuts down writing from its side of the connection.
//
// TransferClient reads the graceful shutdown of the connection from the server, and
// shuts down writing from its side of the connection.
//
// TransferServer reads graceful shutdown of the connection from the client.
//-----------------------------------------------------------------------------

// [ 1]
//-----------------------------------------------------------------------------
// [ 1]
//-----------------------------------------------------------------------------

// Uncomment to test a particular style of socket-to-thread load balancing,
// instead of both static and dynamic load balancing.
// #define NTCF_SYSTEM_TEST_DYNAMIC_LOAD_BALANCING true

// Uncomment to test a specific driver type, instead of all driver types.
// Available names are:
//
// "SELECT"      Implementation using select()
// "POLL"        Implementation using poll()
// "EPOLL"       Implementation using epoll_create/epoll_ctl
// "DEVPOLL"     Implementation using the /dev/poll device.
// "EVENTPORT"   Implementation using the event port API.
// "POLLSET"     Implementation using the pollset API.
// "KQUEUE"      Implementation using kqueue/kevent
// "IOCP"        Implementation using I/O completion ports
// "IOCP"        Implementation using I/O rings
// #define NTCF_SYSTEM_TEST_DRIVER_TYPE "IOCP"

// Uncomment to test a specific address family, instead of all address
// families.
#define NTCF_SYSTEM_TEST_ADDRESS_FAMILY test::TransferParameters::e_IPV4

// Uncomment to test a specific application, or not, of encryption.
// #define NTCF_SYSTEM_TEST_ENCRYPTION false

// Define to 1 to include tests using the installed encryption driver.
#define NTCF_SYSTEM_TEST_BUILD_WITH_TLS 0

namespace test {

/// Provide functions for returning endpoints used by this
/// test driver.
struct EndpointUtil {
    /// Return an endpoint representing a suitable address to which to
    /// bind a socket of the specified 'transport' type for use by this
    /// test driver.
    static ntsa::Endpoint any(ntsa::Transport::Value transport);
};

ntsa::Endpoint EndpointUtil::any(ntsa::Transport::Value transport)
{
    ntsa::Endpoint endpoint;

    switch (transport) {
    case ntsa::Transport::e_TCP_IPV4_STREAM:
    case ntsa::Transport::e_UDP_IPV4_DATAGRAM:
        endpoint.makeIp(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0));
        break;
    case ntsa::Transport::e_TCP_IPV6_STREAM:
    case ntsa::Transport::e_UDP_IPV6_DATAGRAM:
        endpoint.makeIp(ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), 0));
        break;
    case ntsa::Transport::e_LOCAL_STREAM:
    case ntsa::Transport::e_LOCAL_DATAGRAM: {
        ntsa::LocalName   localName;
        const ntsa::Error error = ntsa::LocalName::generateUnique(&localName);
        NTCCFG_TEST_OK(error);

        endpoint.makeLocal(localName);
        break;
    }
    default:
        NTCCFG_UNREACHABLE();
    }

    return endpoint;
}

/// This struct defines the parameters of a test.
struct DatagramSocketParameters {
    ntsa::Transport::Value             d_transport;
    bsl::size_t                        d_numSocketPairs;
    bsl::size_t                        d_numTimers;
    bsl::size_t                        d_numMessages;
    bsl::size_t                        d_messageSize;
    bsl::shared_ptr<bdlbb::Blob>       d_message_sp;
    bsl::shared_ptr<ntci::RateLimiter> d_readRateLimiter_sp;
    bsl::size_t                        d_readQueueHighWatermark;
    bsl::shared_ptr<ntci::RateLimiter> d_writeRateLimiter_sp;
    bsl::size_t                        d_writeQueueHighWatermark;
    bdlb::NullableValue<bsl::size_t>   d_sendBufferSize;
    bdlb::NullableValue<bsl::size_t>   d_receiveBufferSize;
    bool                               d_useAsyncCallbacks;
    bool                               d_tolerateDataLoss;

    DatagramSocketParameters()
    : d_transport(ntsa::Transport::e_UDP_IPV4_DATAGRAM)
    , d_numSocketPairs(1)
    , d_numTimers(0)
    , d_numMessages(1)
    , d_messageSize(32)
    , d_message_sp()
    , d_readRateLimiter_sp()
    , d_readQueueHighWatermark(static_cast<bsl::size_t>(-1))
    , d_writeRateLimiter_sp()
    , d_writeQueueHighWatermark(static_cast<bsl::size_t>(-1))
    , d_sendBufferSize()
    , d_receiveBufferSize()
    , d_useAsyncCallbacks(false)
    , d_tolerateDataLoss(true)
    {
    }
};

/// This test provides a datagram socket protocol for this test driver.
class DatagramSocketSession : public ntci::DatagramSocketSession
{
    ntccfg::Object                        d_object;
    bsl::shared_ptr<ntci::DatagramSocket> d_datagramSocket_sp;
    ntsa::Endpoint                        d_receiverEndpoint;
    bslmt::Latch                          d_numTimerEvents;
    bsls::AtomicUint                      d_numMessagesLeftToSend;
    bslmt::Latch                          d_numMessagesSent;
    bslmt::Latch                          d_numMessagesReceived;
    test::DatagramSocketParameters        d_parameters;
    bslma::Allocator*                     d_allocator_p;

  private:
    DatagramSocketSession(const DatagramSocketSession&) BSLS_KEYWORD_DELETED;
    DatagramSocketSession& operator=(const DatagramSocketSession&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Process the condition that the size of the read queue is greater
    /// than or equal to the read queue low watermark.
    void processReadQueueLowWatermark(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue is less
    /// than or equal to the write queue low watermark.
    void processWriteQueueLowWatermark(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process a receive completion with the specified 'endpoint' and
    /// 'data' or failure according to the specified 'error'.
    void processRead(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const bsl::shared_ptr<ntci::Receiver>&       receiver,
        const bsl::shared_ptr<bdlbb::Blob>&          data,
        const ntca::ReceiveEvent&                    event);

    /// Process a send completion or failure according to the specified
    /// 'error'.
    void processWrite(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const bsl::shared_ptr<ntci::Sender>&         sender,
        const ntca::SendEvent&                       event);

    /// Process a timer event.
    void processTimer(const bsl::shared_ptr<ntci::Timer>& timer,
                      const ntca::TimerEvent&             event);

  public:
    /// Create a new datagram socket application implemented using the
    /// specified 'datagramSocket' that operates according to the specfied
    /// test 'parameters'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    DatagramSocketSession(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const test::DatagramSocketParameters&        parameters,
        bslma::Allocator*                            basicAllocator = 0);

    /// Destroy this object.
    ~DatagramSocketSession() BSLS_KEYWORD_OVERRIDE;

    /// Schedule the timers for which this socket is responsible.
    void schedule();

    /// Send data to the specified 'endpoint'.
    void send(const ntsa::Endpoint& endpoint);

    /// Begin receiving data.
    void receive();

    /// Wait until all expected timers have fired and messages have been
    /// received.
    void wait();

    /// Close the socket.
    void close();

    /// Return the source endpoint of the datagram socket.
    ntsa::Endpoint sourceEndpoint() const;
};

/// Provide a datagram socket manager for this test driver.
class DatagramSocketManager : public ntci::DatagramSocketManager,
                              public ntccfg::Shared<DatagramSocketManager>
{
    /// Define a type alias for a set of managed datagram
    /// sockets.
    typedef bsl::unordered_map<bsl::shared_ptr<ntci::DatagramSocket>,
                               bsl::shared_ptr<DatagramSocketSession> >
        DatagramSocketApplicationMap;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    ntccfg::Object                   d_object;
    bsl::shared_ptr<ntci::Scheduler> d_interface_sp;
    Mutex                            d_socketMapMutex;
    DatagramSocketApplicationMap     d_socketMap;
    bslmt::Latch                     d_socketsEstablished;
    bslmt::Latch                     d_socketsClosed;
    test::DatagramSocketParameters   d_parameters;
    bslma::Allocator*                d_allocator_p;

  private:
    DatagramSocketManager(const DatagramSocketManager&) BSLS_KEYWORD_DELETED;
    DatagramSocketManager& operator=(const DatagramSocketManager&)
        BSLS_KEYWORD_DELETED;

  private:
    void processDatagramSocketEstablished(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
        BSLS_KEYWORD_OVERRIDE;
    // Process the establishment of the specified 'datagramSocket'.

    void processDatagramSocketClosed(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
        BSLS_KEYWORD_OVERRIDE;
    // Process the closure of the specified 'datagramSocket'.

  public:
    /// Create a new datagram socket manager operating according to the
    /// specified test 'parameters' whose sockets are driven by the
    /// specified 'scheduler'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    DatagramSocketManager(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                          const test::DatagramSocketParameters&   parameters,
                          bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~DatagramSocketManager() BSLS_KEYWORD_OVERRIDE;

    /// Create two datagram sockets, have them send data to each, and wait
    /// for each to receive the data.
    void run();
};

void DatagramSocketSession::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (d_parameters.d_useAsyncCallbacks) {
        return;
    }

    while (true) {
        ntca::ReceiveContext receiveContext;
        bdlbb::Blob          data;
        ntca::ReceiveOptions receiveOptions;

        error = d_datagramSocket_sp->receive(&receiveContext,
                                             &data,
                                             receiveOptions);
        if (error) {
            if (error == ntsa::Error::e_WOULD_BLOCK) {
                break;
            }
            else if (error == ntsa::Error::e_EOF) {
                NTCI_LOG_DEBUG(
                    "Datagram socket %d at %s received EOF",
                    (int)(d_datagramSocket_sp->handle()),
                    d_datagramSocket_sp->sourceEndpoint().text().c_str());
                break;
            }
            else {
                NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            }
        }

        NTCCFG_TEST_EQ(d_datagramSocket_sp->transport(),
                       d_parameters.d_transport);

        NTCCFG_TEST_EQ(receiveContext.transport(),
                       d_datagramSocket_sp->transport());

        NTCCFG_TEST_FALSE(receiveContext.endpoint().isNull());
        NTCCFG_TEST_FALSE(receiveContext.endpoint().value().isUndefined());

        NTCCFG_TEST_EQ(data.length(), d_parameters.d_messageSize);

        NTCI_LOG_DEBUG(
            "Datagram socket %d at %s received message %d/%d from %s",
            (int)(d_datagramSocket_sp->handle()),
            d_datagramSocket_sp->sourceEndpoint().text().c_str(),
            (int)((d_parameters.d_numMessages -
                   d_numMessagesReceived.currentCount()) +
                  1),
            (int)(d_parameters.d_numMessages),
            receiveContext.endpoint().value().text().c_str());

        d_numMessagesReceived.arrive();
    }
}

void DatagramSocketSession::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    while (d_numMessagesLeftToSend > 0) {
        bdlbb::Blob data(
            d_datagramSocket_sp->outgoingBlobBufferFactory().get());
        ntcd::DataUtil::generateData(&data, d_parameters.d_messageSize);

        NTCI_LOG_DEBUG(
            "Datagram socket %d at %s sending message %d/%d to %s",
            (int)(d_datagramSocket_sp->handle()),
            d_datagramSocket_sp->sourceEndpoint().text().c_str(),
            (int)((d_parameters.d_numMessages - d_numMessagesLeftToSend) + 1),
            (int)(d_parameters.d_numMessages),
            d_receiverEndpoint.text().c_str());

        if (d_parameters.d_useAsyncCallbacks) {
            ntci::SendCallback sendCallback =
                d_datagramSocket_sp->createSendCallback(
                    NTCCFG_BIND(&DatagramSocketSession::processWrite,
                                this,
                                d_datagramSocket_sp,
                                NTCCFG_BIND_PLACEHOLDER_1,
                                NTCCFG_BIND_PLACEHOLDER_2),
                    d_allocator_p);

            NTCCFG_TEST_EQ(sendCallback.strand(),
                           d_datagramSocket_sp->strand());

            ntca::SendOptions sendOptions;
            sendOptions.setEndpoint(d_receiverEndpoint);

            error = d_datagramSocket_sp->send(data, sendOptions, sendCallback);
            if (error) {
                NTCI_LOG_ERROR(
                    "Datagram socket %d at %s unable to send message "
                    "%d/%d to %s: %s",
                    (int)(d_datagramSocket_sp->handle()),
                    d_datagramSocket_sp->sourceEndpoint().text().c_str(),
                    (int)((d_parameters.d_numMessages -
                           d_numMessagesLeftToSend) +
                          1),
                    (int)(d_parameters.d_numMessages),
                    d_receiverEndpoint.text().c_str(),
                    error.text().c_str());

                // Linux sometimes returns EPERM from 'sendto'.
                if (error == ntsa::Error::e_NOT_AUTHORIZED) {
                    continue;
                }

                NTCCFG_TEST_EQ(error, ntsa::Error::e_WOULD_BLOCK);
                break;
            }

            --d_numMessagesLeftToSend;
        }
        else {
            ntca::SendOptions sendOptions;
            sendOptions.setEndpoint(d_receiverEndpoint);

            error = d_datagramSocket_sp->send(data, sendOptions);
            if (error) {
                NTCI_LOG_ERROR(
                    "Datagram socket %d at %s unable to send message "
                    "%d/%d to %s: %s",
                    (int)(d_datagramSocket_sp->handle()),
                    d_datagramSocket_sp->sourceEndpoint().text().c_str(),
                    (int)((d_parameters.d_numMessages -
                           d_numMessagesLeftToSend) +
                          1),
                    (int)(d_parameters.d_numMessages),
                    d_receiverEndpoint.text().c_str(),
                    error.text().c_str());

                // Linux sometimes returns EPERM from 'sendto'.
                if (error == ntsa::Error::e_NOT_AUTHORIZED) {
                    continue;
                }

                NTCCFG_TEST_EQ(error, ntsa::Error::e_WOULD_BLOCK);
                break;
            }

            --d_numMessagesLeftToSend;
        }
    }
}

void DatagramSocketSession::processRead(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const bsl::shared_ptr<ntci::Receiver>&       receiver,
    const bsl::shared_ptr<bdlbb::Blob>&          data,
    const ntca::ReceiveEvent&                    event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::ReceiveEventType::e_ERROR) {
        NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_EOF);

        NTCI_LOG_DEBUG("Datagram socket %d at %s asynchronously received EOF",
                       (int)(d_datagramSocket_sp->handle()),
                       d_datagramSocket_sp->sourceEndpoint().text().c_str());
    }
    else {
        NTCCFG_TEST_FALSE(event.context().error());

        NTCCFG_TEST_EQ(datagramSocket->transport(), d_parameters.d_transport);

        NTCCFG_TEST_EQ(event.context().transport(),
                       datagramSocket->transport());

        NTCCFG_TEST_FALSE(event.context().endpoint().isNull());
        NTCCFG_TEST_FALSE(event.context().endpoint().value().isUndefined());

        NTCCFG_TEST_EQ(data->length(), d_parameters.d_messageSize);

        NTCI_LOG_DEBUG("Datagram socket %d at %s asynchronously received "
                       "message %d/%d from %s",
                       (int)(d_datagramSocket_sp->handle()),
                       d_datagramSocket_sp->sourceEndpoint().text().c_str(),
                       (int)((d_parameters.d_numMessages -
                              d_numMessagesReceived.currentCount()) +
                             1),
                       (int)(d_parameters.d_numMessages),
                       event.context().endpoint().value().text().c_str());

        ntci::ReceiveCallback receiveCallback =
            d_datagramSocket_sp->createReceiveCallback(
                NTCCFG_BIND(&DatagramSocketSession::processRead,
                            this,
                            d_datagramSocket_sp,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3),
                d_allocator_p);

        NTCCFG_TEST_EQ(receiveCallback.strand(),
                       d_datagramSocket_sp->strand());

        ntsa::Error receiveError =
            d_datagramSocket_sp->receive(ntca::ReceiveOptions(),
                                         receiveCallback);
        NTCCFG_TEST_OK(receiveError);

        d_numMessagesReceived.arrive();
    }
}

void DatagramSocketSession::processWrite(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const bsl::shared_ptr<ntci::Sender>&         sender,
    const ntca::SendEvent&                       event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::SendEventType::e_ERROR) {
        if (event.context().error() == ntsa::Error::e_CANCELLED) {
            NTCI_LOG_DEBUG(
                "Datagram socket %d at %s asynchronous write cancelled",
                (int)(d_datagramSocket_sp->handle()),
                d_datagramSocket_sp->sourceEndpoint().text().c_str());
            return;
        }
        else {
            // Linux sometimes returns EPERM from 'sendto'.
            NTCI_LOG_ERROR(
                "Datagram socket %d at %s asynchronous write failed: %s",
                (int)(d_datagramSocket_sp->handle()),
                d_datagramSocket_sp->sourceEndpoint().text().c_str(),
                event.context().error().text().c_str());

            // MRM
#if 0
            ++d_numMessagesLeftToSend;

            ntca::WriteQueueEvent event;
            event.setType(ntca::WriteQueueEventType::e_LOW_WATERMARK);

            d_datagramSocket_sp->execute(
                NTCCFG_BIND(
                    &DatagramSocketSession::processWriteQueueLowWatermark,
                    this,
                    d_datagramSocket_sp,
                    event));
#endif
        }
    }
    else {
        NTCCFG_TEST_FALSE(event.context().error());

        NTCI_LOG_DEBUG("Datagram socket %d at %s asynchronously sent "
                       "message %d/%d",
                       (int)(d_datagramSocket_sp->handle()),
                       d_datagramSocket_sp->sourceEndpoint().text().c_str(),
                       (int)((d_parameters.d_numMessages -
                              d_numMessagesSent.currentCount()) +
                             1),
                       (int)(d_parameters.d_numMessages));
    }

    // Intentionally update the latch event on failure, since 'sendto' might
    // fail with EPERM on Linux and the test driver tolerates data loss.
    d_numMessagesSent.arrive();
}

void DatagramSocketSession::processTimer(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        NTCI_LOG_DEBUG("Datagram socket %d at %s timer has fired",
                       (int)(d_datagramSocket_sp->handle()),
                       d_datagramSocket_sp->sourceEndpoint().text().c_str());

        d_numTimerEvents.arrive();
    }
    else if (event.type() == ntca::TimerEventType::e_CANCELED) {
        BSLS_ASSERT(event.context().error() == ntsa::Error::e_CANCELLED);

        NTCI_LOG_DEBUG("Datagram socket %d at %s timer has been canceled",
                       (int)(d_datagramSocket_sp->handle()),
                       d_datagramSocket_sp->sourceEndpoint().text().c_str());

        d_numTimerEvents.arrive();
    }
}

DatagramSocketSession::DatagramSocketSession(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const test::DatagramSocketParameters&        parameters,
    bslma::Allocator*                            basicAllocator)
: d_object("test::DatagramSocketSession")
, d_datagramSocket_sp(datagramSocket)
, d_receiverEndpoint()
, d_numTimerEvents(parameters.d_numTimers)
, d_numMessagesLeftToSend(parameters.d_numMessages)
, d_numMessagesSent(parameters.d_numMessages)
, d_numMessagesReceived(parameters.d_numMessages)
, d_parameters(parameters)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

DatagramSocketSession::~DatagramSocketSession()
{
}

void DatagramSocketSession::schedule()
{
    NTCI_LOG_CONTEXT();

    bsls::TimeInterval now = d_datagramSocket_sp->currentTime();

    NTCCFG_FOREACH(bsl::size_t, timerIndex, 0, d_parameters.d_numTimers)
    {
        ntca::TimerOptions timerOptions;
        timerOptions.setOneShot(true);

        ntci::TimerCallback timerCallback =
            d_datagramSocket_sp->createTimerCallback(
                bdlf::MemFnUtil::memFn(&DatagramSocketSession::processTimer,
                                       this),
                d_allocator_p);

        NTCCFG_TEST_EQ(timerCallback.strand(), d_datagramSocket_sp->strand());

        bsl::shared_ptr<ntci::Timer> timer =
            d_datagramSocket_sp->createTimer(timerOptions,
                                             timerCallback,
                                             d_allocator_p);

        bsls::TimeInterval timerDeadline = now;
        timerDeadline.addMilliseconds(timerIndex);

        NTCI_LOG_DEBUG("Datagram socket %d at %s starting timer %d/%d",
                       (int)(d_datagramSocket_sp->handle()),
                       d_datagramSocket_sp->sourceEndpoint().text().c_str(),
                       (int)(timerIndex + 1),
                       (int)(d_parameters.d_numTimers));

        timer->schedule(timerDeadline);
    }
}

void DatagramSocketSession::send(const ntsa::Endpoint& endpoint)
{
    ntsa::Error error;

    NTCCFG_TEST_TRUE(d_receiverEndpoint.isUndefined());
    d_receiverEndpoint = endpoint;

    ntca::WriteQueueEvent event;
    event.setType(ntca::WriteQueueEventType::e_LOW_WATERMARK);

    d_datagramSocket_sp->execute(
        NTCCFG_BIND(&DatagramSocketSession::processWriteQueueLowWatermark,
                    this,
                    d_datagramSocket_sp,
                    event));
}

void DatagramSocketSession::receive()
{
    ntsa::Error error;

    if (d_parameters.d_useAsyncCallbacks) {
        ntci::ReceiveCallback receiveCallback =
            d_datagramSocket_sp->createReceiveCallback(
                NTCCFG_BIND(&DatagramSocketSession::processRead,
                            this,
                            d_datagramSocket_sp,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3),
                d_allocator_p);

        NTCCFG_TEST_EQ(receiveCallback.strand(),
                       d_datagramSocket_sp->strand());

        error = d_datagramSocket_sp->receive(ntca::ReceiveOptions(),
                                             receiveCallback);
        NTCCFG_TEST_OK(error);
    }

    d_datagramSocket_sp->relaxFlowControl(ntca::FlowControlType::e_RECEIVE);
}

void DatagramSocketSession::wait()
{
    NTCI_LOG_CONTEXT();

    d_numTimerEvents.wait();

    if (d_parameters.d_useAsyncCallbacks) {
        d_numMessagesSent.wait();
    }

    if (d_parameters.d_tolerateDataLoss) {
        // Not all datagrams might be received, so wait up to 5 seconds to try
        // to recieve those that can.
        for (bsl::size_t i = 0; i < 50; ++i) {
            if (d_numMessagesReceived.tryWait()) {
                break;
            }

            NTCI_LOG_DEBUG(
                "Datagram socket %d at %s waiting to receive messages",
                (int)(d_datagramSocket_sp->handle()),
                d_datagramSocket_sp->sourceEndpoint().text().c_str());

            bslmt::ThreadUtil::microSleep(100 * 1000);
        }
    }
    else {
        d_numMessagesReceived.wait();
    }

    bsl::size_t numMessagesReceived =
        (d_parameters.d_numMessages - d_numMessagesReceived.currentCount());

    bsl::size_t numMessagesExpectedToReceive = d_parameters.d_numMessages;

    if (numMessagesReceived < numMessagesExpectedToReceive) {
        NTCI_LOG_WARN(
            "Datagram socket %d at %s received %d/%d messages (%.2f%%)",
            (int)(d_datagramSocket_sp->handle()),
            d_datagramSocket_sp->sourceEndpoint().text().c_str(),
            (int)(numMessagesReceived),
            (int)(numMessagesExpectedToReceive),

            (double)(((double)(numMessagesReceived) /
                      (double)(numMessagesExpectedToReceive)) *
                     100)

        );
    }
}

void DatagramSocketSession::close()
{
    ntsa::Error error;

    error = d_datagramSocket_sp->shutdown(ntsa::ShutdownType::e_BOTH,
                                          ntsa::ShutdownMode::e_IMMEDIATE);
    NTCCFG_TEST_FALSE(error);

    if (d_parameters.d_useAsyncCallbacks) {
        ntci::DatagramSocketCloseGuard guard(d_datagramSocket_sp);
    }
    else {
        d_datagramSocket_sp->close();
    }
}

ntsa::Endpoint DatagramSocketSession::sourceEndpoint() const
{
    return d_datagramSocket_sp->sourceEndpoint();
}

void DatagramSocketManager::processDatagramSocketEstablished(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Datagram socket %d established",
                   (int)(datagramSocket->handle()));

    bsl::shared_ptr<DatagramSocketSession> datagramSocketSession;
    datagramSocketSession.createInplace(d_allocator_p,
                                        datagramSocket,
                                        d_parameters,
                                        d_allocator_p);

    datagramSocket->registerSession(datagramSocketSession);

    if (d_parameters.d_readRateLimiter_sp) {
        datagramSocket->setReadRateLimiter(d_parameters.d_readRateLimiter_sp);
    }

    if (d_parameters.d_writeRateLimiter_sp) {
        datagramSocket->setWriteRateLimiter(
            d_parameters.d_writeRateLimiter_sp);
    }

    {
        LockGuard guard(&d_socketMapMutex);
        d_socketMap.insert(
            DatagramSocketApplicationMap::value_type(datagramSocket,
                                                     datagramSocketSession));
    }

    datagramSocketSession->receive();

    d_socketsEstablished.arrive();
}

void DatagramSocketManager::processDatagramSocketClosed(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Datagram socket %d closed",
                   (int)(datagramSocket->handle()));

    {
        LockGuard guard(&d_socketMapMutex);
        bsl::size_t                    n = d_socketMap.erase(datagramSocket);
        NTCCFG_TEST_EQ(n, 1);
    }

    d_socketsClosed.arrive();
}

DatagramSocketManager::DatagramSocketManager(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    const test::DatagramSocketParameters&   parameters,
    bslma::Allocator*                       basicAllocator)
: d_object("test::DatagramSocketManager")
, d_interface_sp(scheduler)
, d_socketMapMutex()
, d_socketMap(basicAllocator)
, d_socketsEstablished(parameters.d_numSocketPairs * 2)
, d_socketsClosed(parameters.d_numSocketPairs * 2)
, d_parameters(parameters)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

DatagramSocketManager::~DatagramSocketManager()
{
    NTCCFG_TEST_TRUE(d_socketMap.empty());
}

void DatagramSocketManager::run()
{
    ntsa::Error error;

    // Create all the datagram socket pairs.

    for (bsl::size_t i = 0; i < d_parameters.d_numSocketPairs; ++i) {
        for (bsl::size_t j = 0; j < 2; ++j) {
            ntca::DatagramSocketOptions options;
            options.setTransport(d_parameters.d_transport);
            options.setSourceEndpoint(
                test::EndpointUtil::any(d_parameters.d_transport));
            options.setReadQueueLowWatermark(1);
            options.setReadQueueHighWatermark(
                d_parameters.d_readQueueHighWatermark);
            options.setWriteQueueLowWatermark(0);
            options.setWriteQueueHighWatermark(
                d_parameters.d_writeQueueHighWatermark);
            options.setSendGreedily(false);
            options.setReceiveGreedily(false);
            options.setKeepHalfOpen(false);

            if (!d_parameters.d_sendBufferSize.isNull()) {
                options.setSendBufferSize(
                    d_parameters.d_sendBufferSize.value());
            }

            if (!d_parameters.d_receiveBufferSize.isNull()) {
                options.setReceiveBufferSize(
                    d_parameters.d_receiveBufferSize.value());
            }

            bsl::shared_ptr<ntci::Resolver> resolver;

            bsl::shared_ptr<ntci::DatagramSocket> datagramSocket =
                d_interface_sp->createDatagramSocket(options, d_allocator_p);

            error = datagramSocket->registerManager(this->getSelf(this));
            NTCCFG_TEST_FALSE(error);

            error = datagramSocket->open();
            NTCCFG_TEST_FALSE(error);
        }
    }

    // Wait for all the datagram sockets to become established.

    d_socketsEstablished.wait();

    // Start the timers for each datagram socket.

    {
        LockGuard guard(&d_socketMapMutex);

        for (DatagramSocketApplicationMap::const_iterator it =
                 d_socketMap.begin();
             it != d_socketMap.end();
             ++it)
        {
            const bsl::shared_ptr<DatagramSocketSession>& socket = it->second;
            socket->schedule();
        }
    }

    // Send data between each datagram socket pair.

    {
        LockGuard guard(&d_socketMapMutex);

        BSLS_ASSERT(d_socketMap.size() % 2 == 0);

        typedef bsl::vector<bsl::shared_ptr<DatagramSocketSession> >
            DatagramSocketApplicationVector;

        DatagramSocketApplicationVector socketVector;
        socketVector.reserve(d_socketMap.size());

        for (DatagramSocketApplicationMap::const_iterator it =
                 d_socketMap.begin();
             it != d_socketMap.end();
             ++it)
        {
            const bsl::shared_ptr<DatagramSocketSession>& socket = it->second;

            socketVector.push_back(socket);
        }

        BSLS_ASSERT(socketVector.size() % 2 == 0);

        for (bsl::size_t i = 0; i < socketVector.size(); i += 2) {
            const bsl::shared_ptr<DatagramSocketSession>& socketOne =
                socketVector[i + 0];

            const bsl::shared_ptr<DatagramSocketSession>& socketTwo =
                socketVector[i + 1];

            socketOne->send(socketTwo->sourceEndpoint());
            socketTwo->send(socketOne->sourceEndpoint());
        }
    }

    // Wait for all timers to fire and all messages to be received for each
    // datagram socket.

    {
        LockGuard guard(&d_socketMapMutex);

        for (DatagramSocketApplicationMap::const_iterator it =
                 d_socketMap.begin();
             it != d_socketMap.end();
             ++it)
        {
            const bsl::shared_ptr<DatagramSocketSession>& socket = it->second;
            socket->wait();
        }
    }

    // Close all the datagram sockets.

    {
        typedef bsl::vector<bsl::shared_ptr<DatagramSocketSession> >
            DatagramSocketApplicationVector;

        DatagramSocketApplicationVector socketVector;
        socketVector.reserve(d_socketMap.size());

        {
            LockGuard guard(&d_socketMapMutex);

            for (DatagramSocketApplicationMap::const_iterator it =
                     d_socketMap.begin();
                 it != d_socketMap.end();
                 ++it)
            {
                const bsl::shared_ptr<DatagramSocketSession>& socket =
                    it->second;

                socketVector.push_back(socket);
            }
        }

        for (DatagramSocketApplicationVector::const_iterator it =
                 socketVector.begin();
             it != socketVector.end();
             ++it)
        {
            const bsl::shared_ptr<DatagramSocketSession>& socket = *it;
            socket->close();
        }
    }

    // Wait for all datagram sockets to close.

    d_socketsClosed.wait();
}

/// This struct defines the parameters of a test.
struct StreamSocketParameters {
    ntsa::Transport::Value             d_transport;
    bsl::size_t                        d_numListeners;
    bsl::size_t                        d_numConnectionsPerListener;
    bsl::size_t                        d_numTimers;
    bsl::size_t                        d_numMessages;
    bsl::size_t                        d_messageSize;
    bsl::shared_ptr<bdlbb::Blob>       d_message_sp;
    bdlb::NullableValue<bsl::size_t>   d_acceptRate;
    bsl::shared_ptr<ntci::RateLimiter> d_acceptRateLimiter_sp;
    bsl::size_t                        d_acceptQueueHighWatermark;
    bdlb::NullableValue<bsl::size_t>   d_readRate;
    bsl::shared_ptr<ntci::RateLimiter> d_readRateLimiter_sp;
    bsl::size_t                        d_readQueueHighWatermark;
    bdlb::NullableValue<bsl::size_t>   d_writeRate;
    bsl::shared_ptr<ntci::RateLimiter> d_writeRateLimiter_sp;
    bsl::size_t                        d_writeQueueHighWatermark;
    bdlb::NullableValue<bsl::size_t>   d_sendBufferSize;
    bdlb::NullableValue<bsl::size_t>   d_receiveBufferSize;
    bool                               d_useAsyncCallbacks;

    StreamSocketParameters()
    : d_transport(ntsa::Transport::e_TCP_IPV4_STREAM)
    , d_numListeners(1)
    , d_numConnectionsPerListener(1)
    , d_numTimers(0)
    , d_numMessages(1)
    , d_messageSize(32)
    , d_message_sp()
    , d_acceptRate()
    , d_acceptRateLimiter_sp()
    , d_acceptQueueHighWatermark(static_cast<bsl::size_t>(-1))
    , d_readRate()
    , d_readRateLimiter_sp()
    , d_readQueueHighWatermark(static_cast<bsl::size_t>(-1))
    , d_writeRate()
    , d_writeRateLimiter_sp()
    , d_writeQueueHighWatermark(static_cast<bsl::size_t>(-1))
    , d_sendBufferSize()
    , d_receiveBufferSize()
    , d_useAsyncCallbacks(false)
    {
    }
};

/// This test provides a listener socket protocol for this test driver.
class ListenerSocketSession : public ntci::ListenerSocketSession
{
    ntccfg::Object                        d_object;
    bsl::shared_ptr<ntci::ListenerSocket> d_listenerSocket_sp;
    bslmt::Latch                          d_numTimerEvents;
    bslmt::Latch                          d_numSocketsAccepted;
    test::StreamSocketParameters          d_parameters;
    bslma::Allocator*                     d_allocator_p;

  private:
    ListenerSocketSession(const ListenerSocketSession&) BSLS_KEYWORD_DELETED;
    ListenerSocketSession& operator=(const ListenerSocketSession&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Process the condition that the size of the accept queue is greater
    /// than or equal to the accept queue low watermark.
    void processAcceptQueueLowWatermark(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the completion or failure of an asynchronous accept by the
    /// specified 'listenerSocket' of the specified 'streamSocket', if any,
    /// according to the specified 'event'.
    void processAccept(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const bsl::shared_ptr<ntci::Acceptor>&       acceptor,
        const bsl::shared_ptr<ntci::StreamSocket>&   streamSocket,
        const ntca::AcceptEvent&                     event);

    /// Process a timer event.
    void processTimer(const bsl::shared_ptr<ntci::Timer>& timer,
                      const ntca::TimerEvent&             event);

  public:
    /// Create a new listener socket application implemented using the
    /// specified 'listenerSocket' that operates according to the specfied
    /// test 'parameters'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    ListenerSocketSession(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const test::StreamSocketParameters&          parameters,
        bslma::Allocator*                            basicAllocator = 0);

    /// Destroy this object.
    ~ListenerSocketSession() BSLS_KEYWORD_OVERRIDE;

    /// Schedule the timers for which this socket is responsible.
    void schedule();

    /// Begin accepting connections.
    void accept();

    /// Wait until all expected timers have fired and sockets have been
    /// accepted.
    void wait();

    /// Close the socket.
    void close();

    /// Return the source endpoint of the listener socket.
    ntsa::Endpoint sourceEndpoint() const;
};

/// This test provides a stream socket protocol for this test driver.
class StreamSocketSession : public ntci::StreamSocketSession
{
    ntccfg::Object                      d_object;
    bsl::shared_ptr<ntci::StreamSocket> d_streamSocket_sp;
    bdlbb::Blob                         d_dataReceived;
    bslmt::Latch                        d_numTimerEvents;
    bsls::AtomicUint                    d_numMessagesLeftToSend;
    bslmt::Latch                        d_numMessagesSent;
    bslmt::Latch                        d_numMessagesReceived;
    test::StreamSocketParameters        d_parameters;
    bslma::Allocator*                   d_allocator_p;

  private:
    StreamSocketSession(const StreamSocketSession&) BSLS_KEYWORD_DELETED;
    StreamSocketSession& operator=(const StreamSocketSession&)
        BSLS_KEYWORD_DELETED;

  private:
    /// Process the condition that the size of the read queue is greater
    /// than or equal to the read queue low watermark.
    void processReadQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue is less
    /// than or equal to the write queue low watermark.
    void processWriteQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process a read completion with the specified 'data' or failure
    /// according to the specified 'error'.
    void processRead(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                     const bsl::shared_ptr<ntci::Receiver>&     receiver,
                     const bsl::shared_ptr<bdlbb::Blob>&        data,
                     const ntca::ReceiveEvent&                  event);

    /// Process a send completion or failure according to the specified
    /// 'error'.
    void processWrite(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                      const bsl::shared_ptr<ntci::Sender>&       sender,
                      const ntca::SendEvent&                     event);

    /// Process a timer event.
    void processTimer(const bsl::shared_ptr<ntci::Timer>& timer,
                      const ntca::TimerEvent&             event);

  public:
    /// Create a new stream socket application implemented using the
    /// specified 'streamSocket' that operates according to the specfied
    /// test 'parameters'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    StreamSocketSession(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const test::StreamSocketParameters&        parameters,
        bslma::Allocator*                          basicAllocator = 0);

    /// Destroy this object.
    ~StreamSocketSession() BSLS_KEYWORD_OVERRIDE;

    /// Schedule the timers for which this socket is responsible.
    void schedule();

    /// Send data to the peer.
    void send();

    /// Begin receiving data.
    void receive();

    /// Wait until all expected timers have fired and messages have been
    /// received.
    void wait();

    /// Close the socket.
    void close();

    /// Return the source endpoint of the stream socket.
    ntsa::Endpoint sourceEndpoint() const;

    /// Return the remote endpoint of the stream socket.
    ntsa::Endpoint remoteEndpoint() const;
};

/// Provide a stream socket manager for this test driver.
class StreamSocketManager : public ntci::ListenerSocketManager,
                            public ntccfg::Shared<StreamSocketManager>
{
    typedef bsl::unordered_map<bsl::shared_ptr<ntci::ListenerSocket>,
                               bsl::shared_ptr<ListenerSocketSession> >
        ListenerSocketApplicationMap;

    /// Define a type alias for a set of managed stream
    /// sockets.
    typedef bsl::unordered_map<bsl::shared_ptr<ntci::StreamSocket>,
                               bsl::shared_ptr<StreamSocketSession> >
        StreamSocketApplicationMap;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    ntccfg::Object                   d_object;
    bsl::shared_ptr<ntci::Scheduler> d_interface_sp;
    Mutex                            d_listenerSocketMapMutex;
    ListenerSocketApplicationMap     d_listenerSocketMap;
    bslmt::Latch                     d_listenerSocketsEstablished;
    bslmt::Latch                     d_listenerSocketsClosed;
    Mutex                            d_streamSocketMapMutex;
    StreamSocketApplicationMap       d_streamSocketMap;
    bslmt::Latch                     d_streamSocketsConnected;
    bslmt::Latch                     d_streamSocketsEstablished;
    bslmt::Latch                     d_streamSocketsClosed;
    test::StreamSocketParameters     d_parameters;
    bslma::Allocator*                d_allocator_p;

  private:
    StreamSocketManager(const StreamSocketManager&) BSLS_KEYWORD_DELETED;
    StreamSocketManager& operator=(const StreamSocketManager&)
        BSLS_KEYWORD_DELETED;

  private:
    void processListenerSocketEstablished(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
        BSLS_KEYWORD_OVERRIDE;
    // Process the establishment of the specified 'listenerSocket'.

    void processListenerSocketClosed(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
        BSLS_KEYWORD_OVERRIDE;
    // Process the closure of the specified 'listenerSocket'.

    void processStreamSocketEstablished(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
        BSLS_KEYWORD_OVERRIDE;
    // Process the establishment of the specified 'streamSocket'.

    /// Process the closure of the specified 'streamSocket'.
    void processStreamSocketClosed(const bsl::shared_ptr<ntci::StreamSocket>&
                                       streamSocket) BSLS_KEYWORD_OVERRIDE;

    /// Process the specified 'connectEvent' for the specified 'connector'
    /// that is the specified 'streamSocket'.
    void processConnect(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Connector>&    connector,
        const ntca::ConnectEvent&                  connectEvent);

  public:
    /// Create a new stream socket manager operating according to the
    /// specified test 'parameters' whose sockets are driven by the
    /// specified 'reactor'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    StreamSocketManager(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                        const test::StreamSocketParameters&     parameters,
                        bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~StreamSocketManager() BSLS_KEYWORD_OVERRIDE;

    /// Create two stream sockets, have them send data to each, and wait
    /// for each to receive the data.
    void run();
};

void ListenerSocketSession::processAcceptQueueLowWatermark(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (d_parameters.d_useAsyncCallbacks) {
        return;
    }

    while (true) {
        ntca::AcceptContext                 acceptContext;
        bsl::shared_ptr<ntci::StreamSocket> streamSocket;
        error = d_listenerSocket_sp->accept(&acceptContext,
                                            &streamSocket,
                                            ntca::AcceptOptions());
        if (error) {
            if (error == ntsa::Error::e_WOULD_BLOCK) {
                break;
            }
            else if (error == ntsa::Error::e_EOF) {
                NTCI_LOG_DEBUG(
                    "Listener socket %d at %s accepted EOF",
                    (int)(d_listenerSocket_sp->handle()),
                    d_listenerSocket_sp->sourceEndpoint().text().c_str());
                break;
            }
            else {
                NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            }
        }

        error = streamSocket->open();
        NTCCFG_TEST_FALSE(error);

        NTCI_LOG_DEBUG(
            "Listener socket %d at %s accepted stream socket %d at %s to %s "
            "(%d/%d)",
            (int)(d_listenerSocket_sp->handle()),
            d_listenerSocket_sp->sourceEndpoint().text().c_str(),
            (int)(streamSocket->handle()),
            streamSocket->sourceEndpoint().text().c_str(),
            streamSocket->remoteEndpoint().text().c_str(),
            (int)((d_parameters.d_numConnectionsPerListener -
                   d_numSocketsAccepted.currentCount()) +
                  1),
            (int)(d_parameters.d_numConnectionsPerListener));

        d_numSocketsAccepted.arrive();
    }
}

void ListenerSocketSession::processAccept(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const bsl::shared_ptr<ntci::Acceptor>&       acceptor,
    const bsl::shared_ptr<ntci::StreamSocket>&   streamSocket,
    const ntca::AcceptEvent&                     event)
{
    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::AcceptEventType::e_ERROR) {
        NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_EOF);

        NTCI_LOG_DEBUG("Listener socket %d at %s asynchronously accepted EOF",
                       (int)(d_listenerSocket_sp->handle()),
                       d_listenerSocket_sp->sourceEndpoint().text().c_str());
    }
    else {
        NTCCFG_TEST_FALSE(event.context().error());

        ntsa::Error openError = streamSocket->open();
        NTCCFG_TEST_FALSE(openError);

        NTCI_LOG_DEBUG("Listener socket %d at %s asynchronously accepted "
                       "stream socket %d at %s to %s",
                       (int)(d_listenerSocket_sp->handle()),
                       d_listenerSocket_sp->sourceEndpoint().text().c_str(),
                       (int)(streamSocket->handle()),
                       streamSocket->sourceEndpoint().text().c_str(),
                       streamSocket->remoteEndpoint().text().c_str());

        ntci::AcceptCallback acceptCallback =
            d_listenerSocket_sp->createAcceptCallback(
                NTCCFG_BIND(&ListenerSocketSession::processAccept,
                            this,
                            d_listenerSocket_sp,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3),
                d_allocator_p);

        NTCCFG_TEST_EQ(acceptCallback.strand(), d_listenerSocket_sp->strand());

        ntsa::Error acceptError =
            d_listenerSocket_sp->accept(ntca::AcceptOptions(), acceptCallback);
        NTCCFG_TEST_OK(acceptError);

        d_numSocketsAccepted.arrive();
    }
}

void ListenerSocketSession::processTimer(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        NTCI_LOG_DEBUG("Listener socket %d at %s timer %d/%d has fired",
                       (int)(d_listenerSocket_sp->handle()),
                       d_listenerSocket_sp->sourceEndpoint().text().c_str(),
                       (int)((d_parameters.d_numTimers -
                              d_numTimerEvents.currentCount()) +
                             1),
                       (int)(d_parameters.d_numTimers));

        d_numTimerEvents.arrive();
    }
    else if (event.type() == ntca::TimerEventType::e_CANCELED) {
        BSLS_ASSERT(event.context().error() == ntsa::Error::e_CANCELLED);

        NTCI_LOG_DEBUG("Listener socket %d at %s timer has been canceled",
                       (int)(d_listenerSocket_sp->handle()),
                       d_listenerSocket_sp->sourceEndpoint().text().c_str());

        d_numTimerEvents.arrive();
    }
}

ListenerSocketSession::ListenerSocketSession(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const test::StreamSocketParameters&          parameters,
    bslma::Allocator*                            basicAllocator)
: d_object("test::ListenerSocketSession")
, d_listenerSocket_sp(listenerSocket)
, d_numTimerEvents(parameters.d_numTimers)
, d_numSocketsAccepted(parameters.d_numConnectionsPerListener)
, d_parameters(parameters)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ListenerSocketSession::~ListenerSocketSession()
{
}

void ListenerSocketSession::schedule()
{
    NTCI_LOG_CONTEXT();

    bsls::TimeInterval now = d_listenerSocket_sp->currentTime();

    NTCCFG_FOREACH(bsl::size_t, timerIndex, 0, d_parameters.d_numTimers)
    {
        ntca::TimerOptions timerOptions;
        timerOptions.setOneShot(true);

        ntci::TimerCallback timerCallback =
            d_listenerSocket_sp->createTimerCallback(
                bdlf::MemFnUtil::memFn(&ListenerSocketSession::processTimer,
                                       this),
                d_allocator_p);

        NTCCFG_TEST_EQ(timerCallback.strand(), d_listenerSocket_sp->strand());

        bsl::shared_ptr<ntci::Timer> timer =
            d_listenerSocket_sp->createTimer(timerOptions,
                                             timerCallback,
                                             d_allocator_p);

        bsls::TimeInterval timerDeadline = now;
        timerDeadline.addMilliseconds(timerIndex);

        NTCI_LOG_DEBUG("Listener socket %d at %s starting timer %d/%d",
                       (int)(d_listenerSocket_sp->handle()),
                       d_listenerSocket_sp->sourceEndpoint().text().c_str(),
                       (int)(timerIndex + 1),
                       (int)(d_parameters.d_numTimers));

        timer->schedule(timerDeadline);
    }
}

void ListenerSocketSession::accept()
{
    ntsa::Error error;

    if (d_parameters.d_useAsyncCallbacks) {
        ntci::AcceptCallback acceptCallback =
            d_listenerSocket_sp->createAcceptCallback(
                NTCCFG_BIND(&ListenerSocketSession::processAccept,
                            this,
                            d_listenerSocket_sp,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3),
                d_allocator_p);

        NTCCFG_TEST_EQ(acceptCallback.strand(), d_listenerSocket_sp->strand());

        error =
            d_listenerSocket_sp->accept(ntca::AcceptOptions(), acceptCallback);
        NTCCFG_TEST_OK(error);
    }

    d_listenerSocket_sp->relaxFlowControl(ntca::FlowControlType::e_RECEIVE);
}

void ListenerSocketSession::wait()
{
    d_numTimerEvents.wait();
    d_numSocketsAccepted.wait();
}

void ListenerSocketSession::close()
{
    ntsa::Error error;

    error = d_listenerSocket_sp->shutdown();
    NTCCFG_TEST_FALSE(error);

    if (d_parameters.d_useAsyncCallbacks) {
        ntci::ListenerSocketCloseGuard guard(d_listenerSocket_sp);
    }
    else {
        d_listenerSocket_sp->close();
    }
}

ntsa::Endpoint ListenerSocketSession::sourceEndpoint() const
{
    return d_listenerSocket_sp->sourceEndpoint();
}

void StreamSocketSession::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (d_parameters.d_useAsyncCallbacks) {
        return;
    }

    while (true) {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setMinSize(d_parameters.d_messageSize);
        receiveOptions.setMaxSize(d_parameters.d_messageSize);

        ntca::ReceiveContext receiveContext;
        error = d_streamSocket_sp->receive(&receiveContext,
                                           &d_dataReceived,
                                           receiveOptions);
        if (error) {
            if (error == ntsa::Error::e_WOULD_BLOCK) {
                break;
            }
            else if (error == ntsa::Error::e_EOF) {
                NTCI_LOG_DEBUG(
                    "Stream socket %d at %s to %s received EOF",
                    (int)(d_streamSocket_sp->handle()),
                    d_streamSocket_sp->sourceEndpoint().text().c_str(),
                    d_streamSocket_sp->remoteEndpoint().text().c_str());
                break;
            }
            else {
                NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            }
        }

        NTCI_LOG_DEBUG("Stream socket %d at %s to %s received %d bytes",
                       (int)(d_streamSocket_sp->handle()),
                       d_streamSocket_sp->sourceEndpoint().text().c_str(),
                       d_streamSocket_sp->remoteEndpoint().text().c_str(),
                       d_dataReceived.length());

        NTCCFG_TEST_EQ(d_streamSocket_sp->transport(),
                       d_parameters.d_transport);

        NTCCFG_TEST_EQ(receiveContext.transport(),
                       d_streamSocket_sp->transport());

        NTCCFG_TEST_FALSE(receiveContext.endpoint().isNull());
        NTCCFG_TEST_FALSE(receiveContext.endpoint().value().isUndefined());

        NTCCFG_TEST_EQ(d_dataReceived.length(), d_parameters.d_messageSize);

        NTCI_LOG_DEBUG("Stream socket %d at %s to %s received message %d/%d",
                       (int)(d_streamSocket_sp->handle()),
                       d_streamSocket_sp->sourceEndpoint().text().c_str(),
                       d_streamSocket_sp->remoteEndpoint().text().c_str(),
                       (int)((d_parameters.d_numMessages -
                              d_numMessagesReceived.currentCount()) +
                             1),
                       (int)(d_parameters.d_numMessages));

        bdlbb::BlobUtil::erase(&d_dataReceived, 0, d_parameters.d_messageSize);

        d_numMessagesReceived.arrive();
    }
}

void StreamSocketSession::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    while (d_numMessagesLeftToSend > 0) {
        bdlbb::Blob data(d_streamSocket_sp->outgoingBlobBufferFactory().get());
        ntcd::DataUtil::generateData(&data, d_parameters.d_messageSize);

        NTCI_LOG_DEBUG(
            "Stream socket %d at %s to %s sending message %d/%d",
            (int)(d_streamSocket_sp->handle()),
            d_streamSocket_sp->sourceEndpoint().text().c_str(),
            d_streamSocket_sp->remoteEndpoint().text().c_str(),
            (int)((d_parameters.d_numMessages - d_numMessagesLeftToSend) + 1),
            (int)(d_parameters.d_numMessages));

        if (d_parameters.d_useAsyncCallbacks) {
            ntci::SendCallback sendCallback =
                d_streamSocket_sp->createSendCallback(
                    NTCCFG_BIND(&StreamSocketSession::processWrite,
                                this,
                                d_streamSocket_sp,
                                NTCCFG_BIND_PLACEHOLDER_1,
                                NTCCFG_BIND_PLACEHOLDER_2),
                    d_allocator_p);

            NTCCFG_TEST_EQ(sendCallback.strand(), d_streamSocket_sp->strand());

            error = d_streamSocket_sp->send(data,
                                            ntca::SendOptions(),
                                            sendCallback);
            if (error) {
                NTCCFG_TEST_EQ(error, ntsa::Error::e_WOULD_BLOCK);
                NTCI_LOG_DEBUG(
                    "Stream socket %d at %s to %s unable to send message "
                    "%d/%d: %s",
                    (int)(d_streamSocket_sp->handle()),
                    d_streamSocket_sp->sourceEndpoint().text().c_str(),
                    d_streamSocket_sp->remoteEndpoint().text().c_str(),
                    (int)((d_parameters.d_numMessages -
                           d_numMessagesLeftToSend) +
                          1),
                    (int)(d_parameters.d_numMessages),
                    error.text().c_str());
                break;
            }

            --d_numMessagesLeftToSend;

            //            bslmt::ThreadUtil::microSleep(500*1000, 0);
        }
        else {
            error = d_streamSocket_sp->send(data, ntca::SendOptions());
            if (error) {
                NTCCFG_TEST_EQ(error, ntsa::Error::e_WOULD_BLOCK);
                NTCI_LOG_DEBUG(
                    "Stream socket %d at %s to %s unable to send message "
                    "%d/%d: %s",
                    (int)(d_streamSocket_sp->handle()),
                    d_streamSocket_sp->sourceEndpoint().text().c_str(),
                    d_streamSocket_sp->remoteEndpoint().text().c_str(),
                    (int)((d_parameters.d_numMessages -
                           d_numMessagesLeftToSend) +
                          1),
                    (int)(d_parameters.d_numMessages),
                    error.text().c_str());
                break;
            }

            --d_numMessagesLeftToSend;
        }
    }
}

void StreamSocketSession::processRead(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Receiver>&     receiver,
    const bsl::shared_ptr<bdlbb::Blob>&        data,
    const ntca::ReceiveEvent&                  event)
{
    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::ReceiveEventType::e_ERROR) {
        NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_EOF);

        NTCI_LOG_DEBUG(
            "Stream socket %d at %s to %s asynchronously received EOF",
            (int)(d_streamSocket_sp->handle()),
            d_streamSocket_sp->sourceEndpoint().text().c_str(),
            d_streamSocket_sp->remoteEndpoint().text().c_str());
    }
    else {
        NTCCFG_TEST_FALSE(event.context().error());

        NTCCFG_TEST_EQ(streamSocket->transport(), d_parameters.d_transport);

        NTCCFG_TEST_EQ(event.context().transport(), streamSocket->transport());

        NTCCFG_TEST_FALSE(event.context().endpoint().isNull());
        NTCCFG_TEST_FALSE(event.context().endpoint().value().isUndefined());

        NTCCFG_TEST_EQ(data->length(), d_parameters.d_messageSize);

        NTCI_LOG_DEBUG("Stream socket %d at %s to %s asynchronously received "
                       "message %d/%d",
                       (int)(d_streamSocket_sp->handle()),
                       d_streamSocket_sp->sourceEndpoint().text().c_str(),
                       d_streamSocket_sp->remoteEndpoint().text().c_str(),
                       (int)((d_parameters.d_numMessages -
                              d_numMessagesReceived.currentCount()) +
                             1),
                       (int)(d_parameters.d_numMessages));

        ntca::ReceiveOptions options;
        options.setMinSize(d_parameters.d_messageSize);
        options.setMaxSize(d_parameters.d_messageSize);

        ntci::ReceiveCallback receiveCallback =
            d_streamSocket_sp->createReceiveCallback(
                NTCCFG_BIND(&StreamSocketSession::processRead,
                            this,
                            d_streamSocket_sp,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3),
                d_allocator_p);

        NTCCFG_TEST_EQ(receiveCallback.strand(), d_streamSocket_sp->strand());

        ntsa::Error receiveError =
            d_streamSocket_sp->receive(options, receiveCallback);
        NTCCFG_TEST_OK(receiveError);

        d_numMessagesReceived.arrive();
    }
}

void StreamSocketSession::processWrite(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Sender>&       sender,
    const ntca::SendEvent&                     event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::SendEventType::e_ERROR) {
        NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_CANCELLED);

        NTCI_LOG_DEBUG(
            "Stream socket %d at %s to %s asynchronous write cancelled",
            (int)(d_streamSocket_sp->handle()),
            d_streamSocket_sp->sourceEndpoint().text().c_str(),
            d_streamSocket_sp->remoteEndpoint().text().c_str());
    }
    else {
        NTCCFG_TEST_FALSE(event.context().error());

        NTCI_LOG_DEBUG("Stream socket %d at %s to %s asynchronously sent "
                       "message %d/%d",
                       (int)(d_streamSocket_sp->handle()),
                       d_streamSocket_sp->sourceEndpoint().text().c_str(),
                       d_streamSocket_sp->remoteEndpoint().text().c_str(),
                       (int)((d_parameters.d_numMessages -
                              d_numMessagesSent.currentCount()) +
                             1),
                       (int)(d_parameters.d_numMessages));

        d_numMessagesSent.arrive();
    }
}

void StreamSocketSession::processTimer(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_OBJECT_GUARD(&d_object);

    NTCI_LOG_CONTEXT();

    if (event.type() == ntca::TimerEventType::e_DEADLINE) {
        NTCI_LOG_DEBUG("Stream socket %d at %s to %s timer %d/%d has fired",
                       (int)(d_streamSocket_sp->handle()),
                       d_streamSocket_sp->sourceEndpoint().text().c_str(),
                       d_streamSocket_sp->remoteEndpoint().text().c_str(),
                       (int)((d_parameters.d_numTimers -
                              d_numTimerEvents.currentCount()) +
                             1),
                       (int)(d_parameters.d_numTimers));

        d_numTimerEvents.arrive();
    }
    else if (event.type() == ntca::TimerEventType::e_CANCELED) {
        BSLS_ASSERT(event.context().error() == ntsa::Error::e_CANCELLED);

        NTCI_LOG_DEBUG("Stream socket %d at %s to %s timer has been canceled",
                       (int)(d_streamSocket_sp->handle()),
                       d_streamSocket_sp->sourceEndpoint().text().c_str(),
                       d_streamSocket_sp->remoteEndpoint().text().c_str());

        d_numTimerEvents.arrive();
    }
}

StreamSocketSession::StreamSocketSession(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const test::StreamSocketParameters&        parameters,
    bslma::Allocator*                          basicAllocator)
: d_object("test::StreamSocketSession")
, d_streamSocket_sp(streamSocket)
, d_dataReceived(streamSocket->incomingBlobBufferFactory().get(),
                 basicAllocator)
, d_numTimerEvents(parameters.d_numTimers)
, d_numMessagesLeftToSend(parameters.d_numMessages)
, d_numMessagesSent(parameters.d_numMessages)
, d_numMessagesReceived(parameters.d_numMessages)
, d_parameters(parameters)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

StreamSocketSession::~StreamSocketSession()
{
}

void StreamSocketSession::schedule()
{
    NTCI_LOG_CONTEXT();

    bsls::TimeInterval now = d_streamSocket_sp->currentTime();

    NTCCFG_FOREACH(bsl::size_t, timerIndex, 0, d_parameters.d_numTimers)
    {
        ntca::TimerOptions timerOptions;
        timerOptions.setOneShot(true);

        ntci::TimerCallback timerCallback =
            d_streamSocket_sp->createTimerCallback(
                bdlf::MemFnUtil::memFn(&StreamSocketSession::processTimer,
                                       this),
                d_allocator_p);

        NTCCFG_TEST_EQ(timerCallback.strand(), d_streamSocket_sp->strand());

        bsl::shared_ptr<ntci::Timer> timer =
            d_streamSocket_sp->createTimer(timerOptions,
                                           timerCallback,
                                           d_allocator_p);

        bsls::TimeInterval timerDeadline = now;
        timerDeadline.addMilliseconds(timerIndex);

        NTCI_LOG_DEBUG("Stream socket %d at %s to %s starting timer %d/%d",
                       (int)(d_streamSocket_sp->handle()),
                       d_streamSocket_sp->sourceEndpoint().text().c_str(),
                       d_streamSocket_sp->remoteEndpoint().text().c_str(),
                       (int)(timerIndex + 1),
                       (int)(d_parameters.d_numTimers));

        timer->schedule(timerDeadline);
    }
}

void StreamSocketSession::send()
{
    ntca::WriteQueueEvent event;
    event.setType(ntca::WriteQueueEventType::e_LOW_WATERMARK);

    d_streamSocket_sp->execute(
        NTCCFG_BIND(&StreamSocketSession::processWriteQueueLowWatermark,
                    this,
                    d_streamSocket_sp,
                    event));
}

void StreamSocketSession::receive()
{
    ntsa::Error error;

    if (d_parameters.d_useAsyncCallbacks) {
        ntca::ReceiveOptions options;
        options.setMinSize(d_parameters.d_messageSize);
        options.setMaxSize(d_parameters.d_messageSize);

        ntci::ReceiveCallback receiveCallback =
            d_streamSocket_sp->createReceiveCallback(
                NTCCFG_BIND(&StreamSocketSession::processRead,
                            this,
                            d_streamSocket_sp,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3),
                d_allocator_p);

        NTCCFG_TEST_EQ(receiveCallback.strand(), d_streamSocket_sp->strand());

        error = d_streamSocket_sp->receive(options, receiveCallback);
        NTCCFG_TEST_OK(error);
    }

    d_streamSocket_sp->relaxFlowControl(ntca::FlowControlType::e_RECEIVE);
}

void StreamSocketSession::wait()
{
    d_numTimerEvents.wait();
    if (d_parameters.d_useAsyncCallbacks) {
        d_numMessagesSent.wait();
    }
    d_numMessagesReceived.wait();
}

void StreamSocketSession::close()
{
    ntsa::Error error;

    error = d_streamSocket_sp->shutdown(ntsa::ShutdownType::e_BOTH,
                                        ntsa::ShutdownMode::e_IMMEDIATE);
    NTCCFG_TEST_FALSE(error);

    if (d_parameters.d_useAsyncCallbacks) {
        ntci::StreamSocketCloseGuard guard(d_streamSocket_sp);
    }
    else {
        d_streamSocket_sp->close();
    }
}

ntsa::Endpoint StreamSocketSession::sourceEndpoint() const
{
    return d_streamSocket_sp->sourceEndpoint();
}

ntsa::Endpoint StreamSocketSession::remoteEndpoint() const
{
    return d_streamSocket_sp->remoteEndpoint();
}

void StreamSocketManager::processListenerSocketEstablished(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Listener socket %d established",
                   (int)(listenerSocket->handle()));

    bsl::shared_ptr<ListenerSocketSession> listenerSocketApplication;
    listenerSocketApplication.createInplace(d_allocator_p,
                                            listenerSocket,
                                            d_parameters,
                                            d_allocator_p);

    listenerSocket->registerSession(listenerSocketApplication);

    if (d_parameters.d_acceptRateLimiter_sp) {
        listenerSocket->setAcceptRateLimiter(
            d_parameters.d_acceptRateLimiter_sp);
    }
    else if (!d_parameters.d_acceptRate.isNull()) {
        bsl::shared_ptr<ntcs::RateLimiter> rateLimiter;
        rateLimiter.createInplace(bslma::Default::defaultAllocator(),
                                  d_parameters.d_acceptRate.value(),
                                  bsls::TimeInterval(1.0),
                                  d_parameters.d_acceptRate.value(),
                                  bsls::TimeInterval(1.0),
                                  listenerSocket->currentTime());

        listenerSocket->setAcceptRateLimiter(rateLimiter);
    }

    {
        LockGuard guard(&d_listenerSocketMapMutex);
        d_listenerSocketMap.insert(ListenerSocketApplicationMap::value_type(
            listenerSocket,
            listenerSocketApplication));
    }

    listenerSocketApplication->accept();

    d_listenerSocketsEstablished.arrive();
}

void StreamSocketManager::processListenerSocketClosed(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Listener socket %d closed",
                   (int)(listenerSocket->handle()));

    {
        LockGuard guard(&d_listenerSocketMapMutex);
        bsl::size_t n = d_listenerSocketMap.erase(listenerSocket);
        NTCCFG_TEST_EQ(n, 1);
    }

    d_listenerSocketsClosed.arrive();
}

void StreamSocketManager::processStreamSocketEstablished(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket %d established",
                   (int)(streamSocket->handle()));

    bsl::shared_ptr<StreamSocketSession> streamSocketSession;
    streamSocketSession.createInplace(d_allocator_p,
                                      streamSocket,
                                      d_parameters,
                                      d_allocator_p);

    streamSocket->registerSession(streamSocketSession);

    if (d_parameters.d_readRateLimiter_sp) {
        streamSocket->setReadRateLimiter(d_parameters.d_readRateLimiter_sp);
    }
    else if (!d_parameters.d_readRate.isNull()) {
        bsl::shared_ptr<ntcs::RateLimiter> rateLimiter;
        rateLimiter.createInplace(bslma::Default::defaultAllocator(),
                                  d_parameters.d_readRate.value(),
                                  bsls::TimeInterval(1.0),
                                  d_parameters.d_readRate.value(),
                                  bsls::TimeInterval(1.0),
                                  streamSocket->currentTime());

        streamSocket->setReadRateLimiter(rateLimiter);
    }

    if (d_parameters.d_writeRateLimiter_sp) {
        streamSocket->setWriteRateLimiter(d_parameters.d_writeRateLimiter_sp);
    }
    else if (!d_parameters.d_writeRate.isNull()) {
        bsl::shared_ptr<ntcs::RateLimiter> rateLimiter;
        rateLimiter.createInplace(bslma::Default::defaultAllocator(),
                                  d_parameters.d_writeRate.value(),
                                  bsls::TimeInterval(1.0),
                                  d_parameters.d_writeRate.value(),
                                  bsls::TimeInterval(1.0),
                                  streamSocket->currentTime());

        streamSocket->setWriteRateLimiter(rateLimiter);
    }

    {
        LockGuard guard(&d_streamSocketMapMutex);
        d_streamSocketMap.insert(
            StreamSocketApplicationMap::value_type(streamSocket,
                                                   streamSocketSession));
    }

    streamSocketSession->receive();

    d_streamSocketsEstablished.arrive();
}

void StreamSocketManager::processStreamSocketClosed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket %d closed", (int)(streamSocket->handle()));

    {
        LockGuard guard(&d_streamSocketMapMutex);
        bsl::size_t n = d_streamSocketMap.erase(streamSocket);
        NTCCFG_TEST_EQ(n, 1);
    }

    d_streamSocketsClosed.arrive();
}

void StreamSocketManager::processConnect(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Connector>&    connector,
    const ntca::ConnectEvent&                  connectEvent)
{
    NTCCFG_TEST_EQ(connectEvent.type(), ntca::ConnectEventType::e_COMPLETE);
    d_streamSocketsConnected.arrive();
}

StreamSocketManager::StreamSocketManager(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    const test::StreamSocketParameters&     parameters,
    bslma::Allocator*                       basicAllocator)
: d_object("test::StreamSocketManager")
, d_interface_sp(scheduler)
, d_listenerSocketMapMutex()
, d_listenerSocketMap(basicAllocator)
, d_listenerSocketsEstablished(parameters.d_numListeners)
, d_listenerSocketsClosed(parameters.d_numListeners)
, d_streamSocketMapMutex()
, d_streamSocketMap(basicAllocator)
, d_streamSocketsConnected(parameters.d_numListeners *
                           parameters.d_numConnectionsPerListener)
, d_streamSocketsEstablished(
      2 * (parameters.d_numListeners * parameters.d_numConnectionsPerListener))
, d_streamSocketsClosed(
      2 * (parameters.d_numListeners * parameters.d_numConnectionsPerListener))
, d_parameters(parameters)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

StreamSocketManager::~StreamSocketManager()
{
    NTCCFG_TEST_TRUE(d_listenerSocketMap.empty());
    NTCCFG_TEST_TRUE(d_streamSocketMap.empty());
}

void StreamSocketManager::run()
{
    ntsa::Error error;

    // Create all the listener sockets.

    for (bsl::size_t i = 0; i < d_parameters.d_numListeners; ++i) {
        ntca::ListenerSocketOptions options;
        options.setTransport(d_parameters.d_transport);
        options.setSourceEndpoint(
            test::EndpointUtil::any(d_parameters.d_transport));
        options.setAcceptQueueLowWatermark(1);
        options.setAcceptQueueHighWatermark(
            d_parameters.d_acceptQueueHighWatermark);
        options.setReadQueueLowWatermark(d_parameters.d_messageSize);
        options.setReadQueueHighWatermark(
            d_parameters.d_readQueueHighWatermark);
        options.setWriteQueueLowWatermark(0);
        options.setWriteQueueHighWatermark(
            d_parameters.d_writeQueueHighWatermark);
        options.setAcceptGreedily(false);
        options.setSendGreedily(false);
        options.setReceiveGreedily(false);
        options.setKeepHalfOpen(false);

        if (!d_parameters.d_sendBufferSize.isNull()) {
            options.setSendBufferSize(d_parameters.d_sendBufferSize.value());
        }

        if (!d_parameters.d_receiveBufferSize.isNull()) {
            options.setReceiveBufferSize(
                d_parameters.d_receiveBufferSize.value());
        }

        bsl::shared_ptr<ntci::Resolver> resolver;

        bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
            d_interface_sp->createListenerSocket(options, d_allocator_p);

        error = listenerSocket->registerManager(this->getSelf(this));
        NTCCFG_TEST_FALSE(error);

        error = listenerSocket->open();
        NTCCFG_TEST_FALSE(error);

        error = listenerSocket->listen();
        NTCCFG_TEST_FALSE(error);
    }

    // Wait for all the listener sockets to become established.

    d_listenerSocketsEstablished.wait();

    // Connect the configured number of sockets to each listener.

    {
        LockGuard guard(&d_listenerSocketMapMutex);

        for (ListenerSocketApplicationMap::const_iterator it =
                 d_listenerSocketMap.begin();
             it != d_listenerSocketMap.end();
             ++it)
        {
            const bsl::shared_ptr<ListenerSocketSession>& listenerSocket =
                it->second;

            for (bsl::size_t i = 0;
                 i < d_parameters.d_numConnectionsPerListener;
                 ++i)
            {
                ntca::StreamSocketOptions options;
                options.setTransport(d_parameters.d_transport);
                options.setReadQueueLowWatermark(d_parameters.d_messageSize);
                options.setReadQueueHighWatermark(
                    d_parameters.d_readQueueHighWatermark);
                options.setWriteQueueLowWatermark(0);
                options.setWriteQueueHighWatermark(
                    d_parameters.d_writeQueueHighWatermark);
                options.setSendGreedily(false);
                options.setReceiveGreedily(false);
                options.setKeepHalfOpen(false);

                if (!d_parameters.d_sendBufferSize.isNull()) {
                    options.setSendBufferSize(
                        d_parameters.d_sendBufferSize.value());
                }

                if (!d_parameters.d_receiveBufferSize.isNull()) {
                    options.setReceiveBufferSize(
                        d_parameters.d_receiveBufferSize.value());
                }

                bsl::shared_ptr<ntci::Resolver> resolver;

                bsl::shared_ptr<ntci::StreamSocket> streamSocket =
                    d_interface_sp->createStreamSocket(options, d_allocator_p);

                error = streamSocket->registerManager(this->getSelf(this));
                NTCCFG_TEST_FALSE(error);

                ntci::ConnectCallback connectCallback =
                    streamSocket->createConnectCallback(
                        NTCCFG_BIND(&StreamSocketManager::processConnect,
                                    this,
                                    streamSocket,
                                    NTCCFG_BIND_PLACEHOLDER_1,
                                    NTCCFG_BIND_PLACEHOLDER_2),
                        d_allocator_p);

                error = streamSocket->connect(listenerSocket->sourceEndpoint(),
                                              ntca::ConnectOptions(),
                                              connectCallback);
                NTCCFG_TEST_OK(error);
            }
        }
    }

    // Wait for all the stream sockets to become connected.

    d_streamSocketsConnected.wait();

    // Wait for all the stream sockets to become established.

    d_streamSocketsEstablished.wait();

    // Start the timers for each listener socket.

    {
        LockGuard guard(&d_listenerSocketMapMutex);

        for (ListenerSocketApplicationMap::const_iterator it =
                 d_listenerSocketMap.begin();
             it != d_listenerSocketMap.end();
             ++it)
        {
            const bsl::shared_ptr<ListenerSocketSession>& listenerSocket =
                it->second;
            listenerSocket->schedule();
        }
    }

    // Start the timers for each stream socket.

    {
        LockGuard guard(&d_streamSocketMapMutex);

        for (StreamSocketApplicationMap::const_iterator it =
                 d_streamSocketMap.begin();
             it != d_streamSocketMap.end();
             ++it)
        {
            const bsl::shared_ptr<StreamSocketSession>& streamSocket =
                it->second;
            streamSocket->schedule();
        }
    }

    // Send data from each connected socket pair.

    {
        LockGuard guard(&d_streamSocketMapMutex);

        for (StreamSocketApplicationMap::const_iterator it =
                 d_streamSocketMap.begin();
             it != d_streamSocketMap.end();
             ++it)
        {
            const bsl::shared_ptr<StreamSocketSession>& streamSocket =
                it->second;
            streamSocket->send();
        }
    }

    // Wait for all timers to fire and all messages to be received for each
    // stream socket.

    {
        LockGuard guard(&d_streamSocketMapMutex);

        for (StreamSocketApplicationMap::const_iterator it =
                 d_streamSocketMap.begin();
             it != d_streamSocketMap.end();
             ++it)
        {
            const bsl::shared_ptr<StreamSocketSession>& streamSocket =
                it->second;
            streamSocket->wait();
        }
    }

    // Wait for all timers to fire and all sockets to be accepted for each
    // listener socket.

    {
        LockGuard guard(&d_listenerSocketMapMutex);

        for (ListenerSocketApplicationMap::const_iterator it =
                 d_listenerSocketMap.begin();
             it != d_listenerSocketMap.end();
             ++it)
        {
            const bsl::shared_ptr<ListenerSocketSession>& listenerSocket =
                it->second;
            listenerSocket->wait();
        }
    }

    // Close all the stream sockets.

    {
        typedef bsl::vector<bsl::shared_ptr<StreamSocketSession> >
            StreamSocketApplicationVector;

        StreamSocketApplicationVector streamSocketVector;
        streamSocketVector.reserve(d_streamSocketMap.size());

        {
            LockGuard guard(&d_streamSocketMapMutex);

            for (StreamSocketApplicationMap::const_iterator it =
                     d_streamSocketMap.begin();
                 it != d_streamSocketMap.end();
                 ++it)
            {
                const bsl::shared_ptr<StreamSocketSession>& socket =
                    it->second;

                streamSocketVector.push_back(socket);
            }
        }

        for (StreamSocketApplicationVector::const_iterator it =
                 streamSocketVector.begin();
             it != streamSocketVector.end();
             ++it)
        {
            const bsl::shared_ptr<StreamSocketSession>& socket = *it;
            socket->close();
        }
    }

    // Wait for all stream sockets to close.

    d_streamSocketsClosed.wait();

    // Close all the listener sockets.

    {
        typedef bsl::vector<bsl::shared_ptr<ListenerSocketSession> >
            ListenerSocketApplicationVector;

        ListenerSocketApplicationVector listenerSocketVector;
        listenerSocketVector.reserve(d_listenerSocketMap.size());

        {
            LockGuard guard(&d_listenerSocketMapMutex);

            for (ListenerSocketApplicationMap::const_iterator it =
                     d_listenerSocketMap.begin();
                 it != d_listenerSocketMap.end();
                 ++it)
            {
                const bsl::shared_ptr<ListenerSocketSession>& socket =
                    it->second;

                listenerSocketVector.push_back(socket);
            }
        }

        for (ListenerSocketApplicationVector::const_iterator it =
                 listenerSocketVector.begin();
             it != listenerSocketVector.end();
             ++it)
        {
            const bsl::shared_ptr<ListenerSocketSession>& socket = *it;
            socket->close();
        }
    }

    // Wait for all listener sockets to close.

    d_listenerSocketsClosed.wait();
}

/// This struct defines the parameters of a test.
struct TransferParameters {
    enum AddressFamily {
        // Enumerates the address families supported by this server.

        e_IPV4
        // The server binds and listens on an IPv4 address.

        ,
        e_IPV6
        // The server binds and listens on an IPv6 address.

        ,
        e_LOCAL
        // The server binds and listens on a local (UNIX) address.
    };

    AddressFamily d_addressFamily;
    bsl::size_t   d_numListeners;
    bsl::size_t   d_numConnectionsPerListener;
    bsl::size_t   d_readQueueHighWatermark;
    bsl::size_t   d_writeQueueHighWatermark;
    bsl::size_t   d_messageSize;
    bsl::size_t   d_numMessages;
    bool          d_encrypted;

    bsl::shared_ptr<bdlbb::Blob> d_message_sp;
    bool                         d_messageVerification;

    bsl::shared_ptr<ntci::EncryptionCertificate> d_authorityCertificate_sp;
    bsl::shared_ptr<ntci::EncryptionKey>         d_authorityPrivateKey_sp;

    bsl::shared_ptr<ntci::EncryptionCertificate> d_clientCertificate_sp;
    bsl::shared_ptr<ntci::EncryptionKey>         d_clientPrivateKey_sp;

    bsl::shared_ptr<ntci::EncryptionCertificate> d_serverCertificate_sp;
    bsl::shared_ptr<ntci::EncryptionKey>         d_serverPrivateKey_sp;

    TransferParameters()
    : d_addressFamily(e_IPV4)
    , d_numListeners(0)
    , d_numConnectionsPerListener(0)
    , d_readQueueHighWatermark(0)
    , d_writeQueueHighWatermark(0)
    , d_messageSize(0)
    , d_numMessages(0)
    , d_encrypted(false)
    , d_message_sp()
    , d_messageVerification(false)
    , d_authorityCertificate_sp()
    , d_authorityPrivateKey_sp()
    , d_clientCertificate_sp()
    , d_clientPrivateKey_sp()
    , d_serverCertificate_sp()
    , d_serverPrivateKey_sp()
    {
    }
};

/// Provide a connection used by a client.
class TransferClientStreamSocketSession : public ntci::StreamSocketSession
{
    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    Mutex                               d_mutex;
    bsl::shared_ptr<ntci::StreamSocket> d_streamSocket_sp;
    bsls::AtomicInt                     d_numMessagesToSend;
    test::TransferParameters            d_parameters;
    bslma::Allocator*                   d_allocator_p;

  private:
    TransferClientStreamSocketSession(const TransferClientStreamSocketSession&)
        BSLS_KEYWORD_DELETED;
    TransferClientStreamSocketSession& operator=(
        const TransferClientStreamSocketSession&) BSLS_KEYWORD_DELETED;

  private:
    /// Process the condition that the size of the read queue is greater
    /// than or equal to the read queue low watermark.
    void processReadQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the read queue is greater
    /// than the read queue high watermark.
    void processReadQueueHighWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the read queue has been discarded
    /// because a non-transient read error asynchronously occured.
    void processReadQueueDiscarded(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue has been
    /// drained down to less than or equal to the write queue low watermark.
    /// This condition will not occur until the write queue high watermark
    /// condition occurs. The write queue low watermark conditions and the
    /// high watermark conditions are guaranteed to occur serially.
    void processWriteQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue is greater
    /// than the write queue high watermark. This condition will occur the
    /// first time the write queue high watermark has been reached but
    /// then will not subsequently ooccur until the write queue low
    /// watermark. The write queue low watermark conditions and the
    /// high watermark conditions are guaranteed to occur serially.
    void processWriteQueueHighWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the write queue has been discarded
    /// because a non-transient write error asynchronously occured.
    void processWriteQueueDiscarded(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the initiation of a downgrade from encrypted to unencrypted
    /// communication.
    void processDowngradeInitiated(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::DowngradeEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the completion of a downgrade from encrypted to unencrypted
    /// communication.
    void processDowngradeComplete(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::DowngradeEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the initiation of the shutdown sequence from the specified
    /// 'origin'.
    void processShutdownInitiated(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for reading.
    void processShutdownReceive(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for writing.
    void processShutdownSend(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the completion of the shutdown sequence.
    void processShutdownComplete(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process an error.
    void processError(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                      const ntca::ErrorEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the upgrade of the specified 'upgradable' that is the
    /// specified 'streamSocket' according to the specified 'upgradeEvent'.
    void processUpgrade(const bsl::shared_ptr<ntci::Upgradable>& upgradable,
                        const ntca::UpgradeEvent&                upgradeEvent);

  public:
    /// Create a new session implemented using the specified 'streamSocket'.
    /// Perform the test described by the specified 'parameters'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    TransferClientStreamSocketSession(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const test::TransferParameters&            parameters,
        bslma::Allocator*                          basicAllocator = 0);

    /// Destroy this object.
    ~TransferClientStreamSocketSession() BSLS_KEYWORD_OVERRIDE;

    /// Upgrade to a secure connection.
    void upgrade(
        const bsl::shared_ptr<ntci::EncryptionClient>& encryptionClient);

    /// Begin sending messages.
    void send();

    /// Begin receiving messages.
    void receive();
};

/// Provide a client.
class TransferClient : public ntci::StreamSocketManager
{
    /// This typedef defines a map of stream sockets to applications of
    /// those stream sockets.
    typedef bsl::unordered_map<
        bsl::shared_ptr<ntci::StreamSocket>,
        bsl::shared_ptr<test::TransferClientStreamSocketSession> >
        StreamSocketMap;

    /// This typedef defines a vector of stream sockets.
    typedef bsl::vector<bsl::shared_ptr<ntci::StreamSocket> >
        StreamSocketVector;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    Mutex                                   d_mutex;
    bsl::shared_ptr<ntci::Scheduler>        d_interface_sp;
    bsl::shared_ptr<ntci::EncryptionClient> d_encryptionClient_sp;
    StreamSocketMap                         d_streamSockets;
    bslmt::Latch                            d_streamSocketsEstablished;
    bslmt::Latch                            d_streamSocketsClosed;
    test::TransferParameters                d_parameters;
    bslma::Allocator*                       d_allocator_p;

  private:
    TransferClient(const TransferClient&) BSLS_KEYWORD_DELETED;
    TransferClient& operator=(const TransferClient&) BSLS_KEYWORD_DELETED;

  private:
    void processStreamSocketEstablished(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
        BSLS_KEYWORD_OVERRIDE;
    // Process the establishment of the specified 'streamSocket'.

    /// Process the closure of the specified 'streamSocket'.
    void processStreamSocketClosed(const bsl::shared_ptr<ntci::StreamSocket>&
                                       streamSocket) BSLS_KEYWORD_OVERRIDE;

    /// Process the connnection of the specified 'connector' that is the
    /// specified 'streamSocket' according to the specified 'connectEvent'.
    void processConnect(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Connector>&    connector,
        const ntca::ConnectEvent&                  connectEvent);

  public:
    /// Create a new client implemented using the specified 'scheduler'.
    /// Perform the test described by the specified 'parameters'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    TransferClient(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                   const test::TransferParameters&         parameters,
                   bslma::Allocator*                       basicAllocator = 0);

    /// Destroy this object.
    ~TransferClient() BSLS_KEYWORD_OVERRIDE;

    /// Connect the number of sessions indicated by the underlying
    /// parameters to the specified 'remoteEndpoint'.
    void connect(const ntsa::Endpoint& remoteEndpoint);

    /// Wait until each client has connected, sent its data, and received
    /// the data is has expected to receive, then is closed.
    void wait();
};

/// Provide a listener.
class TransferServerListenerSocketSession : public ntci::ListenerSocketSession
{
    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    Mutex                                 d_mutex;
    bsl::shared_ptr<ntci::ListenerSocket> d_listenerSocket_sp;
    test::TransferParameters              d_parameters;
    bslma::Allocator*                     d_allocator_p;

  private:
    TransferServerListenerSocketSession(
        const TransferServerListenerSocketSession&) BSLS_KEYWORD_DELETED;
    TransferServerListenerSocketSession& operator=(
        const TransferServerListenerSocketSession&) BSLS_KEYWORD_DELETED;

  private:
    /// Process the condition that the size of the accept queue is greater
    /// than or equal to the accept queue low watermark.
    void processAcceptQueueLowWatermark(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

  public:
    /// Create a new server listener implemented using the specified
    /// 'listenerSocket'. Perform the test described by the specified
    /// 'parameters'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    TransferServerListenerSocketSession(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const test::TransferParameters&              parameters,
        bslma::Allocator*                            basicAllocator = 0);

    /// Destroy this object.
    ~TransferServerListenerSocketSession() BSLS_KEYWORD_OVERRIDE;

    /// Start accepting connections.
    void run();
};

/// Provide a connection used by a server.
class TransferServerStreamSocketSession : public ntci::StreamSocketSession
{
    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    Mutex                               d_mutex;
    bsl::shared_ptr<ntci::StreamSocket> d_streamSocket_sp;
    bdlbb::Blob                         d_receiveData;
    bsls::AtomicInt                     d_numMessagesToReceive;
    test::TransferParameters            d_parameters;
    bslma::Allocator*                   d_allocator_p;

  private:
    TransferServerStreamSocketSession(const TransferServerStreamSocketSession&)
        BSLS_KEYWORD_DELETED;
    TransferServerStreamSocketSession& operator=(
        const TransferServerStreamSocketSession&) BSLS_KEYWORD_DELETED;

  private:
    /// Process the condition that the size of the read queue is greater
    /// than or equal to the read queue low watermark.
    void processReadQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the read queue is greater
    /// than the read queue high watermark.
    void processReadQueueHighWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the read queue has been discarded
    /// because a non-transient read error asynchronously occured.
    void processReadQueueDiscarded(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue has been
    /// drained down to less than or equal to the write queue low watermark.
    /// This condition will not occur until the write queue high watermark
    /// condition occurs. The write queue low watermark conditions and the
    /// high watermark conditions are guaranteed to occur serially.
    void processWriteQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue is greater
    /// than the write queue high watermark. This condition will occur the
    /// first time the write queue high watermark has been reached but
    /// then will not subsequently ooccur until the write queue low
    /// watermark. The write queue low watermark conditions and the
    /// high watermark conditions are guaranteed to occur serially.
    void processWriteQueueHighWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the write queue has been discarded
    /// because a non-transient write error asynchronously occured.
    void processWriteQueueDiscarded(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the initiation of a downgrade from encrypted to unencrypted
    /// communication.
    void processDowngradeInitiated(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::DowngradeEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the completion of a downgrade from encrypted to unencrypted
    /// communication.
    void processDowngradeComplete(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::DowngradeEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the initiation of the shutdown sequence from the specified
    /// 'origin'.
    void processShutdownInitiated(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for reading.
    void processShutdownReceive(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for writing.
    void processShutdownSend(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the completion of the shutdown sequence.
    void processShutdownComplete(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process an error.
    void processError(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                      const ntca::ErrorEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the upgrade of the specified 'upgradable' that is the
    /// specified 'streamSocket' according to the specified 'upgradeEvent'.
    void processUpgrade(const bsl::shared_ptr<ntci::Upgradable>& upgradable,
                        const ntca::UpgradeEvent&                upgradeEvent);

  public:
    /// Create a new session implemented using the specified 'streamSocket'.
    /// Perform the test described by the specified 'parameters'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    TransferServerStreamSocketSession(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const test::TransferParameters&            parameters,
        bslma::Allocator*                          basicAllocator = 0);

    /// Destroy this object.
    ~TransferServerStreamSocketSession() BSLS_KEYWORD_OVERRIDE;

    /// Upgrade to a secure connection.
    void upgrade(
        const bsl::shared_ptr<ntci::EncryptionServer>& encryptionServer);

    /// Begin receiving messages.
    void receive();
};

/// Provide a server.
class TransferServer : public ntci::ListenerSocketManager
{
    /// This typedef defines a map of listener sockets to applications
    /// of those listener sockets.
    typedef bsl::unordered_map<
        bsl::shared_ptr<ntci::ListenerSocket>,
        bsl::shared_ptr<test::TransferServerListenerSocketSession> >
        ListenerSocketMap;

    /// This typedef defines a vector of listener sockets.
    typedef bsl::vector<bsl::shared_ptr<ntci::ListenerSocket> >
        ListenerSocketVector;

    /// This typedef defines a map of stream sockets to applications of
    /// those stream sockets.
    typedef bsl::unordered_map<
        bsl::shared_ptr<ntci::StreamSocket>,
        bsl::shared_ptr<test::TransferServerStreamSocketSession> >
        StreamSocketMap;

    /// This typedef defines a vector of stream sockets.
    typedef bsl::vector<bsl::shared_ptr<ntci::StreamSocket> >
        StreamSocketVector;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex                           d_mutex;
    bsl::shared_ptr<ntci::Scheduler>        d_interface_sp;
    bsl::shared_ptr<ntci::EncryptionServer> d_encryptionServer_sp;
    ListenerSocketMap                       d_listenerSockets;
    bslmt::Latch                            d_listenerSocketsEstablished;
    bslmt::Latch                            d_listenerSocketsClosed;
    StreamSocketMap                         d_streamSockets;
    bslmt::Latch                            d_streamSocketsEstablished;
    bslmt::Latch                            d_streamSocketsClosed;
    test::TransferParameters                d_parameters;
    bslma::Allocator*                       d_allocator_p;

  private:
    TransferServer(const TransferServer&) BSLS_KEYWORD_DELETED;
    TransferServer& operator=(const TransferServer&) BSLS_KEYWORD_DELETED;

  private:
    void processListenerSocketEstablished(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
        BSLS_KEYWORD_OVERRIDE;
    // Process the establishment of the specified 'listenerSocket'.

    void processListenerSocketClosed(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
        BSLS_KEYWORD_OVERRIDE;
    // Process the closure of the specified 'listenerSocket'.

    void processStreamSocketEstablished(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
        BSLS_KEYWORD_OVERRIDE;
    // Process the establishment of the specified 'streamSocket'.

    /// Process the closure of the specified 'streamSocket'.
    void processStreamSocketClosed(const bsl::shared_ptr<ntci::StreamSocket>&
                                       streamSocket) BSLS_KEYWORD_OVERRIDE;

  public:
    /// Create a new server implemented using the specified 'scheduler'.
    /// Perform the test described by the specified 'parameters'. Optionally
    /// specify a 'basicAllocator used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    TransferServer(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                   const test::TransferParameters&         parameters,
                   bslma::Allocator*                       basicAllocator = 0);

    /// Destroy this object.
    ~TransferServer() BSLS_KEYWORD_OVERRIDE;

    /// Start the server. Block until all listeners have been established.
    void listen();

    /// Wait until all server sessions have been established, transferred
    /// their data, and closed.
    void wait();

    /// Load into the specified 'result' the source endpoint of each
    /// listener.
    void getListenerEndpoints(bsl::vector<ntsa::Endpoint>* result) const;
};

void TransferClientStreamSocketSession::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_TEST_LOG_DEBUG << "TransferClient stream socket descriptor "
                          << d_streamSocket_sp->handle() << " at "
                          << d_streamSocket_sp->sourceEndpoint() << " to "
                          << d_streamSocket_sp->remoteEndpoint()
                          << " read queue low watermark: event = " << event
                          << NTCCFG_TEST_LOG_END;

    ntsa::Error error;

    ntca::ReceiveContext receiveContext;
    bdlbb::Blob          receiveData;

    error = d_streamSocket_sp->receive(&receiveContext,
                                       &receiveData,
                                       ntca::ReceiveOptions());
    NTCCFG_TEST_EQ(error, ntsa::Error::e_EOF);

    NTCCFG_TEST_LOG_DEBUG << "TransferClient stream socket descriptor "
                          << d_streamSocket_sp->handle() << " at "
                          << d_streamSocket_sp->sourceEndpoint() << " to "
                          << d_streamSocket_sp->remoteEndpoint()
                          << " received EOF" << NTCCFG_TEST_LOG_END;
}

void TransferClientStreamSocketSession::processReadQueueHighWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_TEST_FALSE(true);
}

void TransferClientStreamSocketSession::processReadQueueDiscarded(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_TEST_FALSE(true);
}

void TransferClientStreamSocketSession::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_TEST_LOG_DEBUG << "TransferClient stream socket descriptor "
                          << d_streamSocket_sp->handle() << " at "
                          << d_streamSocket_sp->sourceEndpoint() << " to "
                          << d_streamSocket_sp->remoteEndpoint()
                          << " write queue low watermark: event = " << event
                          << NTCCFG_TEST_LOG_END;

    while (d_numMessagesToSend > 0) {
        ntsa::Error error = d_streamSocket_sp->send(*d_parameters.d_message_sp,
                                                    ntca::SendOptions());

        if (error) {
            if (error == ntsa::Error::e_WOULD_BLOCK) {
                break;
            }
            else {
                NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            }
        }

        --d_numMessagesToSend;

        NTCCFG_TEST_LOG_DEBUG
            << "TransferClient stream socket descriptor "
            << d_streamSocket_sp->handle() << " at "
            << d_streamSocket_sp->sourceEndpoint() << " to "
            << d_streamSocket_sp->remoteEndpoint() << " sent "
            << (d_parameters.d_numMessages - d_numMessagesToSend) << "/"
            << d_parameters.d_numMessages << NTCCFG_TEST_LOG_END;
    }
}

void TransferClientStreamSocketSession::processWriteQueueHighWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_TEST_LOG_DEBUG << "TransferClient stream socket descriptor "
                          << d_streamSocket_sp->handle() << " at "
                          << d_streamSocket_sp->sourceEndpoint() << " to "
                          << d_streamSocket_sp->remoteEndpoint()
                          << " write queue high watermark: event = " << event
                          << NTCCFG_TEST_LOG_END;
}

void TransferClientStreamSocketSession::processWriteQueueDiscarded(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    // When using proactor sockets, the peer can shutdown the connection and
    // that shutdown might be detected by an oustanding proactive read before
    // all the proactive sends have been detected to become complete. This
    // can occur in the follow sequence of events.
    //
    // TransferClient sends last data.
    // TransferServer receives last expected data and shuts down the connection.
    // TransferClient completes a pending receive that detects the shutdown.
    // [ Still pending: TransferClient completes the pending send of the data. ]
    //
    // In this scenario, the implementation currently erronously announces
    // that pending data on the write queue was discarded, even though that
    // data was copied to the send buffer. The pending write just hadn't been
    // processed by the proactor to have been completed yet.
    //
    // TODO: Fix this problem in the design and implementation somehow.
    //
    // NTCCFG_TEST_FALSE(true);
}

void TransferClientStreamSocketSession::processDowngradeInitiated(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::DowngradeEvent&                event)
{
}

void TransferClientStreamSocketSession::processDowngradeComplete(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::DowngradeEvent&                event)
{
    NTCCFG_TEST_LOG_INFO << "TransferClient stream socket descriptor "
                         << d_streamSocket_sp->handle() << " at "
                         << d_streamSocket_sp->sourceEndpoint() << " to "
                         << d_streamSocket_sp->remoteEndpoint()
                         << " encryption session has been shutdown by peer"
                         << NTCCFG_TEST_LOG_END;
}

void TransferClientStreamSocketSession::processShutdownInitiated(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_TEST_LOG_DEBUG << "TransferClient stream socket descriptor "
                          << d_streamSocket_sp->handle() << " at "
                          << d_streamSocket_sp->sourceEndpoint() << " to "
                          << d_streamSocket_sp->remoteEndpoint()
                          << " shutdown initiated at "
                          << event.context().origin() << NTCCFG_TEST_LOG_END;
}

void TransferClientStreamSocketSession::processShutdownReceive(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_TEST_LOG_DEBUG << "TransferClient stream socket descriptor "
                          << d_streamSocket_sp->handle() << " at "
                          << d_streamSocket_sp->sourceEndpoint() << " to "
                          << d_streamSocket_sp->remoteEndpoint()
                          << " shut down for reading" << NTCCFG_TEST_LOG_END;

    ntsa::Error error =
        d_streamSocket_sp->shutdown(ntsa::ShutdownType::e_SEND,
                                    ntsa::ShutdownMode::e_GRACEFUL);
    NTCCFG_TEST_OK(error);
}

void TransferClientStreamSocketSession::processShutdownSend(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_TEST_LOG_DEBUG << "TransferClient stream socket descriptor "
                          << d_streamSocket_sp->handle() << " at "
                          << d_streamSocket_sp->sourceEndpoint() << " to "
                          << d_streamSocket_sp->remoteEndpoint()
                          << " shut down for writing" << NTCCFG_TEST_LOG_END;
}

void TransferClientStreamSocketSession::processShutdownComplete(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_TEST_LOG_DEBUG << "TransferClient stream socket descriptor "
                          << d_streamSocket_sp->handle() << " at "
                          << d_streamSocket_sp->sourceEndpoint() << " to "
                          << d_streamSocket_sp->remoteEndpoint()
                          << " shutdown complete" << NTCCFG_TEST_LOG_END;
}

void TransferClientStreamSocketSession::processError(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ErrorEvent&                    event)
{
    // An error can be detected on the socket when keep half open is false
    // and the peer initiates the downgrade of the TLS session immediately
    // followed by the shutdown of the TCP connection. In this case, locally,
    // the socket detects the socket is readable, reads the TLS shutdown, then
    // writes a TLS shutdown in response. Sometimes, that write can succeed,
    // but its transmission races with the reception of the TCP shutdown in
    // the operating system. When the operating system processes the TCP
    // shutdown first, some polling mechanisms, e.g. poll and epoll, will
    // detect POLLERR/EPOLLERR and the error on the socket error queue will
    // be EPIPE. The implementation automatically shuts down the socket and
    // announced it is closed in this case, so this handler does not need to
    // do anything.

    NTCCFG_TEST_LOG_DEBUG << "TransferClient stream socket descriptor "
                          << d_streamSocket_sp->handle() << " at "
                          << d_streamSocket_sp->sourceEndpoint() << " to "
                          << d_streamSocket_sp->remoteEndpoint()
                          << " error: " << event.context().error() << " [ "
                          << event.context().errorDescription() << " ]"
                          << NTCCFG_TEST_LOG_END;

    // NTCCFG_TEST_FALSE(true);
}

void TransferClientStreamSocketSession::processUpgrade(
    const bsl::shared_ptr<ntci::Upgradable>& upgradable,
    const ntca::UpgradeEvent&                upgradeEvent)
{
    if (upgradeEvent.type() == ntca::UpgradeEventType::e_COMPLETE) {
        bsl::shared_ptr<ntci::EncryptionCertificate> remoteCertificate =
            d_streamSocket_sp->remoteCertificate();

        if (remoteCertificate) {
            NTCCFG_TEST_LOG_INFO
                << "TransferClient stream socket descriptor "
                << d_streamSocket_sp->handle() << " at "
                << d_streamSocket_sp->sourceEndpoint() << " to "
                << d_streamSocket_sp->remoteEndpoint()
                << " encryption session has been established with "
                << remoteCertificate->subject() << " issued by "
                << remoteCertificate->issuer() << NTCCFG_TEST_LOG_END;
        }
        else {
            NTCCFG_TEST_LOG_INFO << "TransferClient stream socket descriptor "
                                 << d_streamSocket_sp->handle() << " at "
                                 << d_streamSocket_sp->sourceEndpoint()
                                 << " to "
                                 << d_streamSocket_sp->remoteEndpoint()
                                 << " encryption session has been established"
                                 << NTCCFG_TEST_LOG_END;
        }

        this->send();
    }
    else if (upgradeEvent.type() == ntca::UpgradeEventType::e_ERROR) {
        NTCCFG_TEST_LOG_DEBUG << "TransferClient stream socket descriptor "
                              << d_streamSocket_sp->handle() << " at "
                              << d_streamSocket_sp->sourceEndpoint() << " to "
                              << d_streamSocket_sp->remoteEndpoint()
                              << " upgrade error: " << upgradeEvent.context()
                              << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_TRUE(false);
    }
    else {
        NTCCFG_TEST_TRUE(false);
    }
}

TransferClientStreamSocketSession::TransferClientStreamSocketSession(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const test::TransferParameters&            parameters,
    bslma::Allocator*                          basicAllocator)
: d_mutex()
, d_streamSocket_sp(streamSocket)
, d_numMessagesToSend(static_cast<int>(parameters.d_numMessages))
, d_parameters(parameters)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

TransferClientStreamSocketSession::~TransferClientStreamSocketSession()
{
}

void TransferClientStreamSocketSession::upgrade(
    const bsl::shared_ptr<ntci::EncryptionClient>& encryptionClient)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::Encryption> encryption;
    error = encryptionClient->createEncryption(&encryption, d_allocator_p);
    NTCCFG_TEST_OK(error);

    ntca::UpgradeOptions upgradeOptions;

    ntci::UpgradeCallback upgradeCallback =
        d_streamSocket_sp->createUpgradeCallback(
            bdlf::MemFnUtil::memFn(
                &TransferClientStreamSocketSession::processUpgrade,
                this),
            d_allocator_p);

    error = d_streamSocket_sp->upgrade(encryption,
                                       upgradeOptions,
                                       upgradeCallback);
    NTCCFG_TEST_OK(error);
}

void TransferClientStreamSocketSession::send()
{
    ntca::WriteQueueEvent event;
    event.setType(ntca::WriteQueueEventType::e_LOW_WATERMARK);

    this->processWriteQueueLowWatermark(d_streamSocket_sp, event);
}

void TransferClientStreamSocketSession::receive()
{
    ntsa::Error error;

    error =
        d_streamSocket_sp->relaxFlowControl(ntca::FlowControlType::e_RECEIVE);
    NTCCFG_TEST_OK(error);
}

void TransferClient::processStreamSocketEstablished(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCCFG_TEST_LOG_INFO << "TransferClient stream socket descriptor "
                         << streamSocket->handle() << " at "
                         << streamSocket->sourceEndpoint() << " to "
                         << streamSocket->remoteEndpoint() << " is established"
                         << NTCCFG_TEST_LOG_END;

    bsl::shared_ptr<test::TransferClientStreamSocketSession> clientSession;
    clientSession.createInplace(d_allocator_p,
                                streamSocket,
                                d_parameters,
                                d_allocator_p);

    streamSocket->registerSession(clientSession);

    {
        LockGuard lockGuard(&d_mutex);
        d_streamSockets[streamSocket] = clientSession;
    }

    if (d_parameters.d_encrypted) {
        clientSession->upgrade(d_encryptionClient_sp);
        clientSession->receive();
    }
    else {
        clientSession->send();
        clientSession->receive();
    }

    d_streamSocketsEstablished.arrive();
}

void TransferClient::processStreamSocketClosed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCCFG_TEST_LOG_INFO << "TransferClient stream socket descriptor "
                         << streamSocket->handle() << " at "
                         << streamSocket->sourceEndpoint() << " to "
                         << streamSocket->remoteEndpoint() << " is closed"
                         << NTCCFG_TEST_LOG_END;

    {
        LockGuard lockGuard(&d_mutex);
        bsl::size_t                    n = d_streamSockets.erase(streamSocket);
        NTCCFG_TEST_EQ(n, 1);
    }

    d_streamSocketsClosed.arrive();
}

void TransferClient::processConnect(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Connector>&    connector,
    const ntca::ConnectEvent&                  connectEvent)
{
    // TODO: Move 'TransferClient::processStreamSocketEstablished' to here.
}

TransferClient::TransferClient(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    const test::TransferParameters&         parameters,
    bslma::Allocator*                       basicAllocator)
: d_mutex()
, d_interface_sp(scheduler)
, d_encryptionClient_sp()
, d_streamSockets(basicAllocator)
, d_streamSocketsEstablished(NTCCFG_WARNING_NARROW(
      int,
      parameters.d_numListeners* parameters.d_numConnectionsPerListener))
, d_streamSocketsClosed(NTCCFG_WARNING_NARROW(
      int,
      parameters.d_numListeners* parameters.d_numConnectionsPerListener))
, d_parameters(parameters)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntsa::Error error;

    if (parameters.d_encrypted) {
        ntca::EncryptionClientOptions encryptionClientOptions;

        encryptionClientOptions.setAuthentication(
            ntca::EncryptionAuthentication::e_VERIFY);

        encryptionClientOptions.setMinMethod(
            ntca::EncryptionMethod::e_TLS_V1_1);

        encryptionClientOptions.setMaxMethod(
            ntca::EncryptionMethod::e_TLS_V1_X);

        {
            bsl::vector<char> identityData;
            error = d_parameters.d_clientCertificate_sp->encode(&identityData);
            NTCCFG_TEST_OK(error);

            encryptionClientOptions.addResourceData(identityData);
        }

        {
            bsl::vector<char> privateKeyData;
            error =
                d_parameters.d_clientPrivateKey_sp->encode(&privateKeyData);
            NTCCFG_TEST_OK(error);

            encryptionClientOptions.addResourceData(privateKeyData);
        }

        {
            bsl::vector<char> authorityData;
            error =
                d_parameters.d_authorityCertificate_sp->encode(&authorityData);
            NTCCFG_TEST_OK(error);

            encryptionClientOptions.addResourceData(authorityData);
        }

        error = d_interface_sp->createEncryptionClient(&d_encryptionClient_sp,
                                                       encryptionClientOptions,
                                                       d_allocator_p);
        NTCCFG_TEST_OK(error);
    }
}

TransferClient::~TransferClient()
{
}

void TransferClient::connect(const ntsa::Endpoint& remoteEndpoint)
{
    ntsa::Error error;

    StreamSocketVector streamSockets;

    bsl::shared_ptr<ntci::StreamSocketManager> streamSocketManager(
        this,
        bslstl::SharedPtrNilDeleter(),
        d_allocator_p);

    ntca::StreamSocketOptions options;
    options.setTransport(
        remoteEndpoint.transport(ntsa::TransportMode::e_STREAM));
    if (remoteEndpoint.isLocal()) {
        ntsa::LocalName localName;
        error = ntsa::LocalName::generateUnique(&localName);
        NTCCFG_TEST_OK(error);

        options.setSourceEndpoint(ntsa::Endpoint(localName));
    }
    options.setReadQueueLowWatermark(0);
    options.setReadQueueHighWatermark(d_parameters.d_readQueueHighWatermark);
    options.setWriteQueueLowWatermark(0);
    options.setWriteQueueHighWatermark(d_parameters.d_writeQueueHighWatermark);

    NTCCFG_TEST_LOG_DEBUG << "TransferClient connecting "
                          << d_parameters.d_numConnectionsPerListener
                          << " connections to " << remoteEndpoint
                          << NTCCFG_TEST_LOG_END;

    NTCCFG_FOREACH(bsl::size_t, i, 0, d_parameters.d_numConnectionsPerListener)
    {
        bsl::shared_ptr<ntci::StreamSocket> streamSocket =
            d_interface_sp->createStreamSocket(options, d_allocator_p);
        NTCCFG_TEST_TRUE(streamSocket);

        {
            LockGuard lockGuard(&d_mutex);
            d_streamSockets[streamSocket] = bsl::nullptr_t();
        }

        error = streamSocket->registerManager(streamSocketManager);
        NTCCFG_TEST_OK(error);

        ntca::ConnectOptions connectOptions;

        ntci::ConnectCallback connectCallback =
            streamSocket->createConnectCallback(
                NTCCFG_BIND(&TransferClient::processConnect,
                            this,
                            streamSocket,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2),
                d_allocator_p);

        error = streamSocket->connect(remoteEndpoint,
                                      connectOptions,
                                      connectCallback);
        NTCCFG_TEST_OK(error);
    }
}

void TransferClient::wait()
{
    d_streamSocketsEstablished.wait();
    d_streamSocketsClosed.wait();
}

void TransferServerListenerSocketSession::processAcceptQueueLowWatermark(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_TEST_LOG_DEBUG << "TransferServer listener socket descriptor "
                          << d_listenerSocket_sp->handle() << " at "
                          << d_listenerSocket_sp->sourceEndpoint()
                          << " accept queue low watermark"
                          << NTCCFG_TEST_LOG_END;

    ntsa::Error error;

    while (true) {
        ntca::AcceptContext                 acceptContext;
        bsl::shared_ptr<ntci::StreamSocket> streamSocket;

        error = d_listenerSocket_sp->accept(&acceptContext,
                                            &streamSocket,
                                            ntca::AcceptOptions());
        if (error) {
            if (error == ntsa::Error::e_WOULD_BLOCK) {
                break;
            }
            else if (error == ntsa::Error::e_EOF) {
                NTCCFG_TEST_LOG_DEBUG
                    << "TransferServer listener socket descriptor "
                    << d_listenerSocket_sp->handle() << " at "
                    << d_listenerSocket_sp->sourceEndpoint() << " accept EOF"
                    << NTCCFG_TEST_LOG_END;
                break;
            }
            else {
                NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            }
        }
    }
}

TransferServerListenerSocketSession::TransferServerListenerSocketSession(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const test::TransferParameters&              parameters,
    bslma::Allocator*                            basicAllocator)
: d_mutex()
, d_listenerSocket_sp(listenerSocket)
, d_parameters(parameters)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

TransferServerListenerSocketSession::~TransferServerListenerSocketSession()
{
}

void TransferServerListenerSocketSession::run()
{
    ntsa::Error error;

    error = d_listenerSocket_sp->relaxFlowControl(
        ntca::FlowControlType::e_RECEIVE);
    NTCCFG_TEST_OK(error);
}

void TransferServerStreamSocketSession::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_TEST_LOG_DEBUG << "TransferServer stream socket descriptor "
                          << d_streamSocket_sp->handle() << " at "
                          << d_streamSocket_sp->sourceEndpoint() << " to "
                          << d_streamSocket_sp->remoteEndpoint()
                          << " read queue low watermark: event = " << event
                          << NTCCFG_TEST_LOG_END;

    ntsa::Error error;

    ntca::ReceiveOptions options;
    options.setMinSize(d_parameters.d_messageSize);
    options.setMaxSize(d_parameters.d_messageSize);

    while (true) {
        ntca::ReceiveContext receiveContext;
        error = d_streamSocket_sp->receive(&receiveContext,
                                           &d_receiveData,
                                           options);
        if (error) {
            if (error == ntsa::Error::e_WOULD_BLOCK) {
                break;
            }
            else if (error == ntsa::Error::e_EOF) {
                NTCCFG_TEST_LOG_DEBUG
                    << "TransferServer stream socket descriptor "
                    << d_streamSocket_sp->handle() << " at "
                    << d_streamSocket_sp->sourceEndpoint() << " to "
                    << d_streamSocket_sp->remoteEndpoint() << " received EOF"
                    << NTCCFG_TEST_LOG_END;
                break;
            }
            else {
                NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
            }
        }
    }

    while (d_receiveData.length() >= d_parameters.d_messageSize) {
        if (d_parameters.d_messageVerification) {
            bdlbb::Blob message;
            ntcs::BlobUtil::append(&message,
                                   d_receiveData,
                                   d_parameters.d_messageSize);

            int compare =
                bdlbb::BlobUtil::compare(message, *d_parameters.d_message_sp);
            NTCCFG_TEST_EQ(compare, 0);
        }

        ntcs::BlobUtil::pop(&d_receiveData, d_parameters.d_messageSize);
        --d_numMessagesToReceive;

        NTCCFG_TEST_LOG_DEBUG
            << "TransferServer stream socket descriptor "
            << d_streamSocket_sp->handle() << " at "
            << d_streamSocket_sp->sourceEndpoint() << " to "
            << d_streamSocket_sp->remoteEndpoint() << " received "
            << (d_parameters.d_numMessages - d_numMessagesToReceive) << "/"
            << d_parameters.d_numMessages << NTCCFG_TEST_LOG_END;

        if (d_numMessagesToReceive == 0) {
            ntsa::Error error;

            if (d_parameters.d_encrypted) {
                NTCCFG_TEST_LOG_TRACE
                    << "TransferServer stream socket descriptor "
                    << d_streamSocket_sp->handle() << " at "
                    << d_streamSocket_sp->sourceEndpoint() << " to "
                    << d_streamSocket_sp->remoteEndpoint()
                    << " is shutting down encryption session"
                    << NTCCFG_TEST_LOG_END;

                error = d_streamSocket_sp->downgrade();
                NTCCFG_TEST_OK(error);
            }

            NTCCFG_TEST_LOG_TRACE << "TransferServer stream socket descriptor "
                                  << d_streamSocket_sp->handle() << " at "
                                  << d_streamSocket_sp->sourceEndpoint()
                                  << " to "
                                  << d_streamSocket_sp->remoteEndpoint()
                                  << " is shutting down connection to peer"
                                  << NTCCFG_TEST_LOG_END;

            error =
                d_streamSocket_sp->shutdown(ntsa::ShutdownType::e_SEND,
                                            ntsa::ShutdownMode::e_GRACEFUL);
            NTCCFG_TEST_OK(error);

            break;
        }
    }
}

void TransferServerStreamSocketSession::processReadQueueHighWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_TEST_FALSE(true);
}

void TransferServerStreamSocketSession::processReadQueueDiscarded(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_TEST_FALSE(true);
}

void TransferServerStreamSocketSession::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_TEST_FALSE(true);
}

void TransferServerStreamSocketSession::processWriteQueueHighWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_TEST_FALSE(true);
}

void TransferServerStreamSocketSession::processWriteQueueDiscarded(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    // When using proactor sockets, the peer can shutdown the connection and
    // that shutdown might be detected by an oustanding proactive read before
    // all the proactive sends have been detected to become complete. This
    // can occur in the follow sequence of events.
    //
    // TransferClient sends last data.
    // TransferServer receives last expected data and shuts down the connection.
    // TransferClient completes a pending receive that detects the shutdown.
    // [ Still pending: TransferClient completes the pending send of the data. ]
    //
    // In this scenario, the implementation currently erronously announces
    // that pending data on the write queue was discarded, even though that
    // data was copied to the send buffer. The pending write just hadn't been
    // processed by the proactor to have been completed yet.
    //
    // TODO: Fix this problem in the design and implementation somehow.
    //
    // NTCCFG_TEST_FALSE(true);
}

void TransferServerStreamSocketSession::processDowngradeInitiated(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::DowngradeEvent&                event)
{
}

void TransferServerStreamSocketSession::processDowngradeComplete(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::DowngradeEvent&                event)
{
    NTCCFG_TEST_LOG_INFO << "TransferServer stream socket descriptor "
                         << d_streamSocket_sp->handle() << " at "
                         << d_streamSocket_sp->sourceEndpoint() << " to "
                         << d_streamSocket_sp->remoteEndpoint()
                         << " encryption session has been shutdown by peer"
                         << NTCCFG_TEST_LOG_END;
}

void TransferServerStreamSocketSession::processShutdownInitiated(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_TEST_LOG_DEBUG << "TransferServer stream socket descriptor "
                          << d_streamSocket_sp->handle() << " at "
                          << d_streamSocket_sp->sourceEndpoint() << " to "
                          << d_streamSocket_sp->remoteEndpoint()
                          << " shutdown initiated at "
                          << event.context().origin() << NTCCFG_TEST_LOG_END;
}

void TransferServerStreamSocketSession::processShutdownReceive(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_TEST_LOG_DEBUG << "TransferServer stream socket descriptor "
                          << d_streamSocket_sp->handle() << " at "
                          << d_streamSocket_sp->sourceEndpoint() << " to "
                          << d_streamSocket_sp->remoteEndpoint()
                          << " shut down for reading" << NTCCFG_TEST_LOG_END;

    ntsa::Error error =
        d_streamSocket_sp->shutdown(ntsa::ShutdownType::e_SEND,
                                    ntsa::ShutdownMode::e_GRACEFUL);
    NTCCFG_TEST_OK(error);
}

void TransferServerStreamSocketSession::processShutdownSend(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_TEST_LOG_DEBUG << "TransferServer stream socket descriptor "
                          << d_streamSocket_sp->handle() << " at "
                          << d_streamSocket_sp->sourceEndpoint() << " to "
                          << d_streamSocket_sp->remoteEndpoint()
                          << " shut down for writing" << NTCCFG_TEST_LOG_END;
}

void TransferServerStreamSocketSession::processShutdownComplete(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_TEST_LOG_DEBUG << "TransferServer stream socket descriptor "
                          << d_streamSocket_sp->handle() << " at "
                          << d_streamSocket_sp->sourceEndpoint() << " to "
                          << d_streamSocket_sp->remoteEndpoint()
                          << " shutdown complete" << NTCCFG_TEST_LOG_END;
}

void TransferServerStreamSocketSession::processError(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ErrorEvent&                    event)
{
    // An error can be detected on the socket when keep half open is false
    // and the peer initiates the downgrade of the TLS session immediately
    // followed by the shutdown of the TCP connection. In this case, locally,
    // the socket detects the socket is readable, reads the TLS shutdown, then
    // writes a TLS shutdown in response. Sometimes, that write can succeed,
    // but its transmission races with the reception of the TCP shutdown in
    // the operating system. When the operating system processes the TCP
    // shutdown first, some polling mechanisms, e.g. poll and epoll, will
    // detect POLLERR/EPOLLERR and the error on the socket error queue will
    // be EPIPE. The implementation automatically shuts down the socket and
    // announced it is closed in this case, so this handler does not need to
    // do anything.

    NTCCFG_TEST_LOG_DEBUG << "TransferClient stream socket descriptor "
                          << d_streamSocket_sp->handle() << " at "
                          << d_streamSocket_sp->sourceEndpoint() << " to "
                          << d_streamSocket_sp->remoteEndpoint()
                          << " error: " << event.context().error() << " [ "
                          << event.context().errorDescription() << " ]"
                          << NTCCFG_TEST_LOG_END;

    // NTCCFG_TEST_FALSE(true);
}

void TransferServerStreamSocketSession::processUpgrade(
    const bsl::shared_ptr<ntci::Upgradable>& upgradable,
    const ntca::UpgradeEvent&                upgradeEvent)
{
    if (upgradeEvent.type() == ntca::UpgradeEventType::e_COMPLETE) {
        bsl::shared_ptr<ntci::EncryptionCertificate> remoteCertificate =
            d_streamSocket_sp->remoteCertificate();

        if (remoteCertificate) {
            NTCCFG_TEST_LOG_INFO
                << "TransferServer stream socket descriptor "
                << d_streamSocket_sp->handle() << " at "
                << d_streamSocket_sp->sourceEndpoint() << " to "
                << d_streamSocket_sp->remoteEndpoint()
                << " encryption session has been established with "
                << remoteCertificate->subject() << " issued by "
                << remoteCertificate->issuer() << NTCCFG_TEST_LOG_END;
        }
        else {
            NTCCFG_TEST_LOG_INFO << "TransferServer stream socket descriptor "
                                 << d_streamSocket_sp->handle() << " at "
                                 << d_streamSocket_sp->sourceEndpoint()
                                 << " to "
                                 << d_streamSocket_sp->remoteEndpoint()
                                 << " encryption session has been established"
                                 << NTCCFG_TEST_LOG_END;
        }
    }
    else if (upgradeEvent.type() == ntca::UpgradeEventType::e_ERROR) {
        NTCCFG_TEST_LOG_DEBUG << "TransferServer stream socket descriptor "
                              << d_streamSocket_sp->handle() << " at "
                              << d_streamSocket_sp->sourceEndpoint() << " to "
                              << d_streamSocket_sp->remoteEndpoint()
                              << " upgrade error: " << upgradeEvent.context()
                              << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_TRUE(false);
    }
    else {
        NTCCFG_TEST_TRUE(false);
    }
}

TransferServerStreamSocketSession::TransferServerStreamSocketSession(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const test::TransferParameters&            parameters,
    bslma::Allocator*                          basicAllocator)
: d_mutex()
, d_streamSocket_sp(streamSocket)
, d_receiveData(basicAllocator)
, d_numMessagesToReceive(NTCCFG_WARNING_NARROW(int, parameters.d_numMessages))
, d_parameters(parameters)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

TransferServerStreamSocketSession::~TransferServerStreamSocketSession()
{
}

void TransferServerStreamSocketSession::upgrade(
    const bsl::shared_ptr<ntci::EncryptionServer>& encryptionServer)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::Encryption> encryption;
    error = encryptionServer->createEncryption(&encryption, d_allocator_p);
    NTCCFG_TEST_OK(error);

    ntca::UpgradeOptions upgradeOptions;

    ntci::UpgradeCallback upgradeCallback =
        d_streamSocket_sp->createUpgradeCallback(
            bdlf::MemFnUtil::memFn(
                &TransferServerStreamSocketSession::processUpgrade,
                this),
            d_allocator_p);

    error = d_streamSocket_sp->upgrade(encryption,
                                       upgradeOptions,
                                       upgradeCallback);
    NTCCFG_TEST_OK(error);
}

void TransferServerStreamSocketSession::receive()
{
    ntsa::Error error;

    error = d_streamSocket_sp->setReadQueueLowWatermark(
        d_parameters.d_messageSize);
    NTCCFG_TEST_OK(error);

    error =
        d_streamSocket_sp->relaxFlowControl(ntca::FlowControlType::e_RECEIVE);
    NTCCFG_TEST_OK(error);
}

void TransferServer::processListenerSocketEstablished(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
{
    NTCCFG_TEST_LOG_INFO << "Listener socket descriptor "
                         << listenerSocket->handle() << " at "
                         << listenerSocket->sourceEndpoint()
                         << " is established" << NTCCFG_TEST_LOG_END;

    bsl::shared_ptr<test::TransferServerListenerSocketSession> serverListener;
    serverListener.createInplace(d_allocator_p,
                                 listenerSocket,
                                 d_parameters,
                                 d_allocator_p);

    listenerSocket->registerSession(serverListener);

    {
        LockGuard lockGuard(&d_mutex);
        d_listenerSockets[listenerSocket] = serverListener;
    }

    serverListener->run();

    d_listenerSocketsEstablished.arrive();
}

void TransferServer::processListenerSocketClosed(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
{
    NTCCFG_TEST_LOG_INFO << "Listener socket descriptor "
                         << listenerSocket->handle() << " at "
                         << listenerSocket->sourceEndpoint() << " is closed"
                         << NTCCFG_TEST_LOG_END;

    {
        LockGuard lockGuard(&d_mutex);
        bsl::size_t n = d_listenerSockets.erase(listenerSocket);
        NTCCFG_TEST_EQ(n, 1);
    }

    d_listenerSocketsClosed.arrive();
}

void TransferServer::processStreamSocketEstablished(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCCFG_TEST_LOG_INFO << "TransferServer stream socket descriptor "
                         << streamSocket->handle() << " at "
                         << streamSocket->sourceEndpoint() << " to "
                         << streamSocket->remoteEndpoint() << " is established"
                         << NTCCFG_TEST_LOG_END;

    bsl::shared_ptr<test::TransferServerStreamSocketSession> serverSession;
    serverSession.createInplace(d_allocator_p,
                                streamSocket,
                                d_parameters,
                                d_allocator_p);

    streamSocket->registerSession(serverSession);

    {
        LockGuard lockGuard(&d_mutex);
        d_streamSockets[streamSocket] = serverSession;
    }

    if (d_parameters.d_encrypted) {
        serverSession->upgrade(d_encryptionServer_sp);
        serverSession->receive();
    }
    else {
        serverSession->receive();
    }

    d_streamSocketsEstablished.arrive();
}

void TransferServer::processStreamSocketClosed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    NTCCFG_TEST_LOG_INFO << "TransferServer stream socket descriptor "
                         << streamSocket->handle() << " at "
                         << streamSocket->sourceEndpoint() << " to "
                         << streamSocket->remoteEndpoint() << " is closed"
                         << NTCCFG_TEST_LOG_END;

    {
        LockGuard lockGuard(&d_mutex);
        bsl::size_t                    n = d_streamSockets.erase(streamSocket);
        NTCCFG_TEST_EQ(n, 1);
    }

    d_streamSocketsClosed.arrive();
}

TransferServer::TransferServer(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    const test::TransferParameters&         parameters,
    bslma::Allocator*                       basicAllocator)
: d_mutex()
, d_interface_sp(scheduler)
, d_encryptionServer_sp()
, d_listenerSockets(basicAllocator)
, d_listenerSocketsEstablished(
      NTCCFG_WARNING_NARROW(int, parameters.d_numListeners))
, d_listenerSocketsClosed(
      NTCCFG_WARNING_NARROW(int, parameters.d_numListeners))
, d_streamSockets(basicAllocator)
, d_streamSocketsEstablished(NTCCFG_WARNING_NARROW(
      int,
      parameters.d_numListeners* parameters.d_numConnectionsPerListener))
, d_streamSocketsClosed(NTCCFG_WARNING_NARROW(
      int,
      parameters.d_numListeners* parameters.d_numConnectionsPerListener))
, d_parameters(parameters)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntsa::Error error;

    if (parameters.d_encrypted) {
        ntca::EncryptionServerOptions encryptionServerOptions;

        encryptionServerOptions.setAuthentication(
            ntca::EncryptionAuthentication::e_NONE);

        encryptionServerOptions.setMinMethod(
            ntca::EncryptionMethod::e_TLS_V1_1);

        encryptionServerOptions.setMaxMethod(
            ntca::EncryptionMethod::e_TLS_V1_X);

        {
            bsl::vector<char> identityData;
            error = d_parameters.d_serverCertificate_sp->encode(&identityData);
            NTCCFG_TEST_OK(error);

            encryptionServerOptions.addResourceData(identityData);
        }

        {
            bsl::vector<char> privateKeyData;
            error =
                d_parameters.d_serverPrivateKey_sp->encode(&privateKeyData);
            NTCCFG_TEST_OK(error);

            encryptionServerOptions.addResourceData(privateKeyData);
        }

        {
            bsl::vector<char> authorityData;
            error =
                d_parameters.d_authorityCertificate_sp->encode(&authorityData);
            NTCCFG_TEST_OK(error);

            encryptionServerOptions.addResourceData(authorityData);
        }

        error = d_interface_sp->createEncryptionServer(&d_encryptionServer_sp,
                                                       encryptionServerOptions,
                                                       d_allocator_p);
        NTCCFG_TEST_OK(error);
    }
}

TransferServer::~TransferServer()
{
}

void TransferServer::listen()
{
    ntsa::Error error;

    NTCCFG_FOREACH(bsl::size_t, listenerIndex, 0, d_parameters.d_numListeners)
    {
        ntsa::Endpoint sourceEndpoint;

        if (d_parameters.d_addressFamily == test::TransferParameters::e_IPV4) {
            sourceEndpoint.makeIp(
                ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0));
        }
        else if (d_parameters.d_addressFamily ==
                 test::TransferParameters::e_IPV6)
        {
            sourceEndpoint.makeIp(
                ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), 0));
        }
        else if (d_parameters.d_addressFamily ==
                 test::TransferParameters::e_LOCAL)
        {
            ntsa::LocalName localName;
            error = ntsa::LocalName::generateUnique(&localName);
            NTCCFG_TEST_OK(error);

            sourceEndpoint.makeLocal(localName);
        }

        bsl::shared_ptr<ntci::ListenerSocketManager> listenerSocketManager(
            this,
            bslstl::SharedPtrNilDeleter(),
            d_allocator_p);

        ntca::ListenerSocketOptions options;
        options.setSourceEndpoint(sourceEndpoint);
        options.setReuseAddress(false);
        options.setBacklog(100);
        options.setReadQueueLowWatermark(0);
        options.setReadQueueHighWatermark(
            d_parameters.d_readQueueHighWatermark);
        options.setWriteQueueLowWatermark(0);
        options.setWriteQueueHighWatermark(
            d_parameters.d_writeQueueHighWatermark);

        bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
            d_interface_sp->createListenerSocket(options, d_allocator_p);
        NTCCFG_TEST_TRUE(listenerSocket);

        {
            LockGuard lockGuard(&d_mutex);
            d_listenerSockets[listenerSocket] = bsl::nullptr_t();
        }

        error = listenerSocket->registerManager(listenerSocketManager);
        NTCCFG_TEST_OK(error);

        error = listenerSocket->open();
        NTCCFG_TEST_OK(error);

        error = listenerSocket->listen();
        NTCCFG_TEST_OK(error);
    }

    d_listenerSocketsEstablished.wait();
}

void TransferServer::wait()
{
    d_streamSocketsEstablished.wait();
    d_streamSocketsClosed.wait();

    ListenerSocketVector listenerSockets;
    {
        LockGuard lockGuard(&d_mutex);

        NTCCFG_FOREACH_ITERATOR(ListenerSocketMap::iterator,
                                it,
                                d_listenerSockets)
        {
            listenerSockets.push_back(it->first);
        }
    }

    NTCCFG_FOREACH_ITERATOR(ListenerSocketVector::iterator,
                            it,
                            listenerSockets)
    {
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket = *it;
        listenerSocket->close();
    }

    d_listenerSocketsClosed.wait();
}

void TransferServer::getListenerEndpoints(
    bsl::vector<ntsa::Endpoint>* result) const
{
    LockGuard lockGuard(&d_mutex);

    NTCCFG_FOREACH_ITERATOR(ListenerSocketMap::const_iterator,
                            it,
                            d_listenerSockets)
    {
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket =
            it->first;

        result->push_back(listenerSocket->sourceEndpoint());
    }
}

/// Provide a test resolver for use by this test driver. This class is thread
/// safe.
class Resolver : public ntci::Resolver, public ntccfg::Shared<Resolver>
{
    bsl::shared_ptr<ntsi::Resolver> d_resolver_sp;
    bdlmt::EventScheduler           d_scheduler;
    bsls::TimeInterval              d_delay;
    bsl::shared_ptr<ntci::Strand>   d_strand_sp;
    bslma::Allocator*               d_allocator_p;

  private:
    Resolver(const Resolver&) BSLS_KEYWORD_DELETED;
    Resolver& operator=(const Resolver&) BSLS_KEYWORD_DELETED;

  private:
    /// Resolve the specified 'domainName' to the IP addresses assigned to
    /// the 'domainName', according to the specified 'options'. When
    /// resolution completes or fails, invoke the specified 'callback' on
    /// the callback's strand, if any, with the IP addresses assigned to
    /// the 'domainName'. Return the error.
    void processGetIpAddress(const bslstl::StringRef&          domainName,
                             const ntca::GetIpAddressOptions&  options,
                             const ntci::GetIpAddressCallback& callback);

    /// Resolve the specified 'ipAddress' to the domain name to which the
    /// 'ipAddress' has been assigned, according to the specified 'options'.
    /// When resolution completes or fails, invoke the specified 'callback'
    /// on the callback's strand, if any, with the domain name to which the
    /// 'ipAddress' has been assigned. Return the error.
    void processGetDomainName(const ntsa::IpAddress&             ipAddress,
                              const ntca::GetDomainNameOptions&  options,
                              const ntci::GetDomainNameCallback& callback);

    /// Resolve the specified 'serviceName' to the ports assigned to the
    /// 'serviceName', according to the specified 'options'. When resolution
    /// completes or fails, invoke the specified 'callback' on the
    /// callback's strand, if any, with the ports assigned to the
    /// 'serviceName'. Return the error.
    void processGetPort(const bslstl::StringRef&     serviceName,
                        const ntca::GetPortOptions&  options,
                        const ntci::GetPortCallback& callback);

    /// Resolve the specified 'port' to the service name to which the 'port'
    /// has been assigned, according to the specified 'options'. When
    /// resolution completes or fails, invoke the specified 'callback' on
    /// the callback's strand, if any, with the service name to which the
    /// 'port' has been assigned. Return the error.
    void processGetServiceName(ntsa::Port                          port,
                               const ntca::GetServiceNameOptions&  options,
                               const ntci::GetServiceNameCallback& callback);

    /// Parse and potentially resolve the components of the specified
    /// 'text', in the format of '<port>' or '[<host>][:<port>]'. If the
    /// optionally specified '<host>' component is not an IP address,
    /// interpret the '<host>' as a domain name and resolve it into an IP
    /// address. If the optionally specified '<port>' is a name and not a
    /// number, interpret the '<port>' as a service name and resolve it into
    /// a port. Perform all resolution and validation of the characteristics
    /// of the desired 'result' according to the specified 'options'. When
    /// resolution completes or fails, invoke the specified 'callback' on
    /// the callback's strand, if any, with the endpoint to which the
    /// components of the 'text' resolve. Return the error.
    void processGetEndpoint(const bslstl::StringRef&         text,
                            const ntca::GetEndpointOptions&  options,
                            const ntci::GetEndpointCallback& callback);

  public:
    /// Create a new resolver. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Resolver(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    virtual ~Resolver() BSLS_KEYWORD_OVERRIDE;

    /// Start the resolver. Return the error.
    ntsa::Error start() BSLS_KEYWORD_OVERRIDE;

    /// Begin stopping the resolver.
    void shutdown() BSLS_KEYWORD_OVERRIDE;

    /// Wait until the resolver has stopped.
    void linger() BSLS_KEYWORD_OVERRIDE;

    /// Set the delay before each asynchronous resolution operation begins
    /// to the specified 'delay'.
    void setDelay(const bsls::TimeInterval& delay);

    /// Set the specified 'domainName' to resolve to the specified
    /// 'ipAddressList', and vice-versa. Return the error. Note that calling
    /// this function affects the future behavior of this object only:
    /// 'ipAddressList' will be subsequently returned from calling
    /// 'getIpAddress()' of 'domainName', and 'domainName' will be returned
    /// from calling 'getDomainName()' of any of the addresses in
    /// 'ipAddressList', but this function does not affect any name
    /// resolution caches maintained elsewhere by the system.
    ntsa::Error setIpAddress(const bslstl::StringRef&            domainName,
                             const bsl::vector<ntsa::IpAddress>& ipAddressList)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'domainName' to resolve to the specified
    /// 'ipAddressList', and vice-versa, in addition to any previously,
    /// explicitly defined associations. Return the error. Note that calling
    /// this function affects the future behavior of this object only:
    /// 'ipAddressList' will be subsequently returned from calling
    /// 'getIpAddress()' of 'domainName', and 'domainName' will be returned
    /// from calling 'getDomainName()' of any of the addresses in
    /// 'ipAddressList', but this function does not affect any name
    /// resolution caches maintained elsewhere by the system.
    ntsa::Error addIpAddress(const bslstl::StringRef&            domainName,
                             const bsl::vector<ntsa::IpAddress>& ipAddressList)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'domainName' to resolve to the specified
    /// 'ipAddress', and vice-versa, in addition to any previously,
    /// explicitly defined associations. Return the error. Note that calling
    /// this function affects the future behavior of this object only:
    /// 'ipAddress' will be subsequently returned from calling
    /// 'getIpAddress()' of 'domainName', and 'domainName' will be returned
    /// from calling 'getDomainName()' of any of the addresses in
    /// 'ipAddressList', but this function does not affect any name
    /// resolution caches maintained elsewhere by the system.
    ntsa::Error addIpAddress(const bslstl::StringRef& domainName,
                             const ntsa::IpAddress&   ipAddress)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the specified 'serviceName' to resolve to the specified
    /// 'portList' for use by the specified 'transport', and vice-versa.
    /// Return the error. Note that calling this function affects the future
    /// behavior of this object only: 'port' will be subsequently returned
    /// from calling 'getPort()' of 'serviceName', and 'serviceName' will be
    /// returned from calling 'getServiceName()' of any of the ports in
    /// 'portList', but this function does not affect any name resolution
    /// caches maintained elsewhere by the system.
    ntsa::Error setPort(const bslstl::StringRef&       serviceName,
                        const bsl::vector<ntsa::Port>& portList,
                        ntsa::Transport::Value         transport)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'serviceName' to resolve to the specified
    /// 'portList' for use by the specified 'transport', and vice-versa,
    /// in addition to any previously, explicitly defined associations.
    /// Return the error. Note that calling this function affects the future
    /// behavior of this object only: 'port' will be subsequently returned
    /// from calling 'getPort()' of 'serviceName', and 'serviceName' will be
    /// returned from calling 'getServiceName()' of any of the ports in
    /// 'portList', but this function does not affect any name resolution
    /// caches maintained elsewhere by the system.
    ntsa::Error addPort(const bslstl::StringRef&       serviceName,
                        const bsl::vector<ntsa::Port>& portList,
                        ntsa::Transport::Value         transport)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'serviceName' to resolve to the specified
    /// 'port' for use by the specified 'transport', and vice-versa,
    /// in addition to any previously, explicitly defined associations.
    /// Return the error. Note that calling this function affects the future
    /// behavior of this object only: 'port' will be subsequently returned
    /// from calling 'getPort()' of 'serviceName', and 'serviceName' will be
    /// returned from calling 'getServiceName()' of any of the ports in
    /// 'portList', but this function does not affect any name resolution
    /// caches maintained elsewhere by the system.
    ntsa::Error addPort(const bslstl::StringRef& serviceName,
                        ntsa::Port               port,
                        ntsa::Transport::Value   transport)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the local IP addresses assigned to the local machine to the
    /// specified 'ipAddressList'. Return the error. Note that calling this
    /// function affects the future behavior of this object only:
    /// 'ipAddressList' will be subsequently returned from calling
    /// 'getLocalIpAddress()' but this function does not set the local IP
    /// addresses of the system or have any wider effect on other objects or
    /// name resolution functionality in this process.
    ntsa::Error setLocalIpAddress(const bsl::vector<ntsa::IpAddress>&
                                      ipAddressList) BSLS_KEYWORD_OVERRIDE;

    /// Set the hostname of the local machine to the specified 'name'.
    /// Return the error. Note that calling this function affects the future
    /// behavior of this object only: 'name' will be subsequently returned
    /// from calling 'getHostname()' but this function does not set the
    /// hostname of the system or have any wider effect on other objects or
    /// name resolution functionality in this process.
    ntsa::Error setHostname(const bsl::string& name) BSLS_KEYWORD_OVERRIDE;

    /// Set the canonical, fully-qualified hostname of the local machine to
    /// the specified 'name'. Return the error. Note that calling this
    /// function affects the future behavior of this object only: 'name'
    /// will be subsequently returned from calling
    /// 'getHostnameFullyQualified()' but this function does not set the
    /// hostname of the system or have any wider effect on other objects or
    /// name resolution functionality in this process.
    ntsa::Error setHostnameFullyQualified(const bsl::string& name)
        BSLS_KEYWORD_OVERRIDE;

    /// Resolve the specified 'domainName' to the IP addresses assigned to
    /// the 'domainName', according to the specified 'options'. When
    /// resolution completes or fails, invoke the specified 'callback' on
    /// the callback's strand, if any, with the IP addresses assigned to
    /// the 'domainName'. Return the error.
    ntsa::Error getIpAddress(const bslstl::StringRef&          domainName,
                             const ntca::GetIpAddressOptions&  options,
                             const ntci::GetIpAddressCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Resolve the specified 'ipAddress' to the domain name to which the
    /// 'ipAddress' has been assigned, according to the specified 'options'.
    /// When resolution completes or fails, invoke the specified 'callback'
    /// on the callback's strand, if any, with the domain name to which the
    /// 'ipAddress' has been assigned. Return the error.
    ntsa::Error getDomainName(const ntsa::IpAddress&             ipAddress,
                              const ntca::GetDomainNameOptions&  options,
                              const ntci::GetDomainNameCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Resolve the specified 'serviceName' to the ports assigned to the
    /// 'serviceName', according to the specified 'options'. When resolution
    /// completes or fails, invoke the specified 'callback' on the
    /// callback's strand, if any, with the ports assigned to the
    /// 'serviceName'. Return the error.
    ntsa::Error getPort(const bslstl::StringRef&     serviceName,
                        const ntca::GetPortOptions&  options,
                        const ntci::GetPortCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Resolve the specified 'port' to the service name to which the 'port'
    /// has been assigned, according to the specified 'options'. When
    /// resolution completes or fails, invoke the specified 'callback' on
    /// the callback's strand, if any, with the service name to which the
    /// 'port' has been assigned. Return the error.
    ntsa::Error getServiceName(ntsa::Port                          port,
                               const ntca::GetServiceNameOptions&  options,
                               const ntci::GetServiceNameCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Parse and potentially resolve the components of the specified
    /// 'text', in the format of '<port>' or '[<host>][:<port>]'. If the
    /// optionally specified '<host>' component is not an IP address,
    /// interpret the '<host>' as a domain name and resolve it into an IP
    /// address. If the optionally specified '<port>' is a name and not a
    /// number, interpret the '<port>' as a service name and resolve it into
    /// a port. Perform all resolution and validation of the characteristics
    /// of the desired 'result' according to the specified 'options'. When
    /// resolution completes or fails, invoke the specified 'callback' on
    /// the callback's strand, if any, with the endpoint to which the
    /// components of the 'text' resolve. Return the error.
    ntsa::Error getEndpoint(const bslstl::StringRef&         text,
                            const ntca::GetEndpointOptions&  options,
                            const ntci::GetEndpointCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the IP addresses assigned to the
    /// local machine. Perform all resolution and validation of the
    /// characteristics of the desired 'result' according to the specified
    /// 'options'. Return the error.
    ntsa::Error getLocalIpAddress(bsl::vector<ntsa::IpAddress>* result,
                                  const ntsa::IpAddressOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Return the hostname of the local machine.
    ntsa::Error getHostname(bsl::string* result) BSLS_KEYWORD_OVERRIDE;

    /// Return the canonical, fully-qualified hostname of the local machine.
    ntsa::Error getHostnameFullyQualified(bsl::string* result)
        BSLS_KEYWORD_OVERRIDE;

    /// Defer the execution of the specified 'functor'.
    void execute(const Functor& functor) BSLS_KEYWORD_OVERRIDE;

    /// Atomically defer the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE;

    /// Create a new strand to serialize execution of functors. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::Strand> createStrand(
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'session' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&                  options,
        const bsl::shared_ptr<ntci::TimerSession>& session,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'callback' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&  options,
        const ntci::TimerCallback& callback,
        bslma::Allocator*          basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Return the strand that guarantees sequential, non-current execution
    /// of arbitrary functors on the unspecified threads processing events
    /// for this object.
    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current elapsed time since the Unix epoch.
    bsls::TimeInterval currentTime() const BSLS_KEYWORD_OVERRIDE;
};

void Resolver::processGetIpAddress(const bslstl::StringRef&         domainName,
                                   const ntca::GetIpAddressOptions& options,
                                   const ntci::GetIpAddressCallback& callback)
{
}

void Resolver::processGetDomainName(
    const ntsa::IpAddress&             ipAddress,
    const ntca::GetDomainNameOptions&  options,
    const ntci::GetDomainNameCallback& callback)
{
}

void Resolver::processGetPort(const bslstl::StringRef&     serviceName,
                              const ntca::GetPortOptions&  options,
                              const ntci::GetPortCallback& callback)
{
}

void Resolver::processGetServiceName(
    ntsa::Port                          port,
    const ntca::GetServiceNameOptions&  options,
    const ntci::GetServiceNameCallback& callback)
{
}

void Resolver::processGetEndpoint(const bslstl::StringRef&         text,
                                  const ntca::GetEndpointOptions&  options,
                                  const ntci::GetEndpointCallback& callback)
{
    bsl::shared_ptr<Resolver> self = this->getSelf(this);

    ntsa::Endpoint endpoint;

    ntca::GetEndpointContext getEndpointContext;
    getEndpointContext.setError(ntsa::Error(ntsa::Error::e_EOF));
    getEndpointContext.setSource(ntca::ResolverSource::e_UNKNOWN);

    ntca::GetEndpointEvent getEndpointEvent;
    getEndpointEvent.setType(ntca::GetEndpointEventType::e_ERROR);
    getEndpointEvent.setContext(getEndpointContext);

    callback(self, endpoint, getEndpointEvent, d_strand_sp);
}

Resolver::Resolver(bslma::Allocator* basicAllocator)
: d_resolver_sp()
, d_scheduler(basicAllocator)
, d_delay()
, d_strand_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_resolver_sp = ntsf::System::createResolver(d_allocator_p);
}

Resolver::~Resolver()
{
}

ntsa::Error Resolver::start()
{
    bslmt::ThreadAttributes threadAttributes;
    threadAttributes.setThreadName("test-resolver");

    int rc = d_scheduler.start(threadAttributes);
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

void Resolver::shutdown()
{
}

void Resolver::linger()
{
    d_scheduler.cancelAllEventsAndWait();
    d_scheduler.stop();
}

void Resolver::setDelay(const bsls::TimeInterval& delay)
{
    d_delay = delay;
}

ntsa::Error Resolver::setIpAddress(
    const bslstl::StringRef&            domainName,
    const bsl::vector<ntsa::IpAddress>& ipAddressList)
{
    return d_resolver_sp->setIpAddress(domainName, ipAddressList);
}

ntsa::Error Resolver::addIpAddress(
    const bslstl::StringRef&            domainName,
    const bsl::vector<ntsa::IpAddress>& ipAddressList)
{
    return d_resolver_sp->addIpAddress(domainName, ipAddressList);
}

ntsa::Error Resolver::addIpAddress(const bslstl::StringRef& domainName,
                                   const ntsa::IpAddress&   ipAddress)
{
    return d_resolver_sp->addIpAddress(domainName, ipAddress);
}

ntsa::Error Resolver::setPort(const bslstl::StringRef&       serviceName,
                              const bsl::vector<ntsa::Port>& portList,
                              ntsa::Transport::Value         transport)
{
    return d_resolver_sp->setPort(serviceName, portList, transport);
}

ntsa::Error Resolver::addPort(const bslstl::StringRef&       serviceName,
                              const bsl::vector<ntsa::Port>& portList,
                              ntsa::Transport::Value         transport)
{
    return d_resolver_sp->addPort(serviceName, portList, transport);
}

ntsa::Error Resolver::addPort(const bslstl::StringRef& serviceName,
                              ntsa::Port               port,
                              ntsa::Transport::Value   transport)
{
    return d_resolver_sp->addPort(serviceName, port, transport);
}

ntsa::Error Resolver::setLocalIpAddress(
    const bsl::vector<ntsa::IpAddress>& ipAddressList)
{
    return d_resolver_sp->setLocalIpAddress(ipAddressList);
}

ntsa::Error Resolver::setHostname(const bsl::string& name)
{
    return d_resolver_sp->setHostname(name);
}

ntsa::Error Resolver::setHostnameFullyQualified(const bsl::string& name)
{
    return d_resolver_sp->setHostnameFullyQualified(name);
}

ntsa::Error Resolver::getIpAddress(const bslstl::StringRef&         domainName,
                                   const ntca::GetIpAddressOptions& options,
                                   const ntci::GetIpAddressCallback& callback)
{
    d_scheduler.scheduleEvent(
        bdlt::CurrentTime::now() + d_delay,
        bdlf::BindUtil::bind(&Resolver::processGetIpAddress,
                             this,
                             bsl::string(domainName),
                             options,
                             callback));

    return ntsa::Error();
}

ntsa::Error Resolver::getDomainName(
    const ntsa::IpAddress&             ipAddress,
    const ntca::GetDomainNameOptions&  options,
    const ntci::GetDomainNameCallback& callback)
{
    d_scheduler.scheduleEvent(
        bdlt::CurrentTime::now() + d_delay,
        bdlf::BindUtil::bind(&Resolver::processGetDomainName,
                             this,
                             ipAddress,
                             options,
                             callback));

    return ntsa::Error();
}

ntsa::Error Resolver::getPort(const bslstl::StringRef&     serviceName,
                              const ntca::GetPortOptions&  options,
                              const ntci::GetPortCallback& callback)
{
    d_scheduler.scheduleEvent(bdlt::CurrentTime::now() + d_delay,
                              bdlf::BindUtil::bind(&Resolver::processGetPort,
                                                   this,
                                                   bsl::string(serviceName),
                                                   options,
                                                   callback));

    return ntsa::Error();
}

ntsa::Error Resolver::getServiceName(
    ntsa::Port                          port,
    const ntca::GetServiceNameOptions&  options,
    const ntci::GetServiceNameCallback& callback)
{
    d_scheduler.scheduleEvent(
        bdlt::CurrentTime::now() + d_delay,
        bdlf::BindUtil::bind(&Resolver::processGetServiceName,
                             this,
                             port,
                             options,
                             callback));

    return ntsa::Error();
}

ntsa::Error Resolver::getEndpoint(const bslstl::StringRef&         text,
                                  const ntca::GetEndpointOptions&  options,
                                  const ntci::GetEndpointCallback& callback)
{
    d_scheduler.scheduleEvent(
        bdlt::CurrentTime::now() + d_delay,
        bdlf::BindUtil::bind(&Resolver::processGetEndpoint,
                             this,
                             bsl::string(text),
                             options,
                             callback));

    return ntsa::Error();
}

ntsa::Error Resolver::getLocalIpAddress(bsl::vector<ntsa::IpAddress>* result,
                                        const ntsa::IpAddressOptions& options)
{
    return d_resolver_sp->getLocalIpAddress(result, options);
}

ntsa::Error Resolver::getHostname(bsl::string* result)
{
    return d_resolver_sp->getHostname(result);
}

ntsa::Error Resolver::getHostnameFullyQualified(bsl::string* result)
{
    return d_resolver_sp->getHostnameFullyQualified(result);
}

void Resolver::execute(const Functor& functor)
{
}

void Resolver::moveAndExecute(FunctorSequence* functorSequence,
                              const Functor&   functor)
{
}

bsl::shared_ptr<ntci::Strand> Resolver::createStrand(
    bslma::Allocator* basicAllocator)
{
    return bsl::shared_ptr<ntci::Strand>();
}

bsl::shared_ptr<ntci::Timer> Resolver::createTimer(
    const ntca::TimerOptions&                  options,
    const bsl::shared_ptr<ntci::TimerSession>& session,
    bslma::Allocator*                          basicAllocator)
{
    return bsl::shared_ptr<ntci::Timer>();
}

bsl::shared_ptr<ntci::Timer> Resolver::createTimer(
    const ntca::TimerOptions&  options,
    const ntci::TimerCallback& callback,
    bslma::Allocator*          basicAllocator)
{
    return bsl::shared_ptr<ntci::Timer>();
}

const bsl::shared_ptr<ntci::Strand>& Resolver::strand() const
{
    return d_strand_sp;
}

bsls::TimeInterval Resolver::currentTime() const
{
    return bdlt::CurrentTime::now();
}

/// Provide utilities for processing resolver events in this
/// test driver.
struct ResolverUtil {
    /// Process the resolution of a domain name to the specified
    /// 'ipAddressList' by the specified 'resolver' according to the
    /// specified 'event'. Post to the specified 'semaphore'.
    static void processGetIpAddressResult(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const bsl::vector<ntsa::IpAddress>&    ipAddressList,
        const ntca::GetIpAddressEvent&         event,
        bslmt::Semaphore*                      semaphore);

    /// Process the resolution of an IP address to the specified
    /// 'domainName' by the specified 'resolver' according to the specified
    /// 'event'. Post to the specified 'semaphore'.
    static void processGetDomainNameResult(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const bsl::string&                     domainName,
        const ntca::GetDomainNameEvent&        event,
        bslmt::Semaphore*                      semaphore);

    /// Process the resolution of a service name to the specified 'portList'
    /// by the specified 'resolver' according to the specified 'event'. Post
    /// to the specified 'semaphore'.
    static void processGetPortResult(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const bsl::vector<ntsa::Port>&         portList,
        const ntca::GetPortEvent&              event,
        bslmt::Semaphore*                      semaphore);

    /// Process the resolution of a port to the specified 'serviceName' by
    /// the specified 'resolver' according to the specified 'event'. Post to
    /// the specified 'semaphore'.
    static void processGetServiceNameResult(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const bsl::string&                     serviceName,
        const ntca::GetServiceNameEvent&       event,
        bslmt::Semaphore*                      semaphore);

    /// Process the resolution of an authority (i.e., host and port) to the
    /// specified 'endpoint' by the specified 'resolver' according to the
    /// specified 'event'. Post to the specified 'semaphore'.
    static void processGetEndpointResult(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const ntsa::Endpoint&                  endpoint,
        const ntca::GetEndpointEvent&          event,
        bslmt::Semaphore*                      semaphore);

    /// Process the resolution of a domain name to the specified
    /// 'ipAddressList' by the specified 'resolver' according to the
    /// specified 'event'. Load 'ipAddressList' into the specified
    /// 'outputIpAddressList', load 'event' into the specified
    /// 'outputEvent', and post to the specified 'semaphore'.
    static void processGetIpAddressEvent(
        bsl::vector<ntsa::IpAddress>*          outputIpAddressList,
        ntca::GetIpAddressEvent*               outputEvent,
        bslmt::Semaphore*                      semaphore,
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const bsl::vector<ntsa::IpAddress>&    ipAddressList,
        const ntca::GetIpAddressEvent&         event);
};

void ResolverUtil::processGetIpAddressResult(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const bsl::vector<ntsa::IpAddress>&    ipAddressList,
    const ntca::GetIpAddressEvent&         event,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get IP address event " << event
                          << NTCI_LOG_STREAM_END;

    if (event.type() == ntca::GetIpAddressEventType::e_COMPLETE) {
        if (ipAddressList.size() > 0) {
            for (bsl::size_t i = 0; i < ipAddressList.size(); ++i) {
                const ntsa::IpAddress& ipAddress = ipAddressList[i];
                NTCI_LOG_STREAM_INFO << "The domain name '"
                                     << event.context().domainName()
                                     << "' has resolved to " << ipAddress
                                     << NTCI_LOG_STREAM_END;
            }
        }
        else {
            NTCI_LOG_STREAM_INFO
                << "The domain name '" << event.context().domainName()
                << "' has no IP addresses assigned" << NTCI_LOG_STREAM_END;
        }
    }

    semaphore->post();
}

void ResolverUtil::processGetDomainNameResult(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const bsl::string&                     domainName,
    const ntca::GetDomainNameEvent&        event,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get domain name event " << event
                          << NTCI_LOG_STREAM_END;

    if (event.type() == ntca::GetDomainNameEventType::e_COMPLETE) {
        if (!domainName.empty()) {
            NTCI_LOG_STREAM_INFO
                << "The IP address " << event.context().ipAddress()
                << " has resolved to the domain name '" << domainName << "'"
                << NTCI_LOG_STREAM_END;
        }
        else {
            NTCI_LOG_STREAM_INFO << "The IP address '"
                                 << event.context().ipAddress()
                                 << "' is not assigned to any domain name"
                                 << NTCI_LOG_STREAM_END;
        }
    }

    semaphore->post();
}

void ResolverUtil::processGetPortResult(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const bsl::vector<ntsa::Port>&         portList,
    const ntca::GetPortEvent&              event,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get port event " << event
                          << NTCI_LOG_STREAM_END;

    if (event.type() == ntca::GetPortEventType::e_COMPLETE) {
        if (portList.size() > 0) {
            for (bsl::size_t i = 0; i < portList.size(); ++i) {
                ntsa::Port port = portList[i];
                NTCI_LOG_STREAM_INFO << "The service name '"
                                     << event.context().serviceName()
                                     << "' has resolved to port " << port
                                     << NTCI_LOG_STREAM_END;
            }
        }
        else {
            NTCI_LOG_STREAM_INFO
                << "The service name '" << event.context().serviceName()
                << "' has no ports assigned" << NTCI_LOG_STREAM_END;
        }
    }

    semaphore->post();
}

void ResolverUtil::processGetServiceNameResult(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const bsl::string&                     serviceName,
    const ntca::GetServiceNameEvent&       event,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get service name event " << event
                          << NTCI_LOG_STREAM_END;

    if (event.type() == ntca::GetServiceNameEventType::e_COMPLETE) {
        if (!serviceName.empty()) {
            NTCI_LOG_STREAM_INFO << "The port " << event.context().port()
                                 << " has resolved to the service name '"
                                 << serviceName << "'" << NTCI_LOG_STREAM_END;
        }
        else {
            NTCI_LOG_STREAM_INFO << "The port '" << event.context().port()
                                 << "' is not assigned to any service name"
                                 << NTCI_LOG_STREAM_END;
        }
    }

    semaphore->post();
}

void ResolverUtil::processGetEndpointResult(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const ntsa::Endpoint&                  endpoint,
    const ntca::GetEndpointEvent&          event,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get endpoint event " << event
                          << NTCI_LOG_STREAM_END;

    if (event.type() == ntca::GetEndpointEventType::e_COMPLETE) {
        NTCI_LOG_STREAM_INFO
            << "The authority '" << event.context().authority()
            << "' has resolved to " << endpoint << NTCI_LOG_STREAM_END;
    }

    semaphore->post();
}

void ResolverUtil::processGetIpAddressEvent(
    bsl::vector<ntsa::IpAddress>*          outputIpAddressList,
    ntca::GetIpAddressEvent*               outputEvent,
    bslmt::Semaphore*                      semaphore,
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const bsl::vector<ntsa::IpAddress>&    ipAddressList,
    const ntca::GetIpAddressEvent&         event)
{
    NTCCFG_WARNING_UNUSED(resolver);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get IP address event " << event
                          << NTCI_LOG_STREAM_END;

    *outputIpAddressList = ipAddressList;
    *outputEvent         = event;

    semaphore->post();
}

/// Provide callbacks used in examples. This struct is thread
/// safe.
struct ExampleUtil {
    /// Assert the specified 'event' indicates an asynchronous 'connect'
    /// operation on the specified 'streamSocket' completed successfully
    /// then post to the specified 'semaphore'.
    static void processConnect(
        const bsl::shared_ptr<ntci::Connector>& connector,
        const ntca::ConnectEvent&               event,
        bslmt::Semaphore*                       semaphore);

    /// Assert the specified 'event' indicates an asynchronous 'accept'
    /// operation on the specified 'listenerSocket' completed successfully
    /// then assign the specified 'streamSocket' (which is the accepted
    /// socket) to the specified 'result' then post to the specified
    /// 'semaphore'.
    static void processAccept(
        const bsl::shared_ptr<ntci::Acceptor>&     acceptor,
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::AcceptEvent&                   event,
        bslmt::Semaphore*                          semaphore,
        bsl::shared_ptr<ntci::StreamSocket>*       result);

    /// Assert the specified 'event' indicates an asynchronous 'send'
    /// operation on the specified 'streamSocket' completed successfully
    /// then post to the specified 'semaphore'.
    static void processSend(const bsl::shared_ptr<ntci::Sender>& sender,
                            const ntca::SendEvent&               event,
                            bslmt::Semaphore*                    semaphore);

    /// Assert the specified 'event' indicates an asynchronous 'send'
    /// operation on the specified 'streamSocket' completed successfully
    /// then assign the specified 'data' (which is the data received) to the
    /// specified 'result' then post to the specified 'semaphore'.
    static void processReceive(const bsl::shared_ptr<ntci::Receiver>& receiver,
                               const bsl::shared_ptr<bdlbb::Blob>&    data,
                               const ntca::ReceiveEvent&              event,
                               bslmt::Semaphore* semaphore,
                               bdlbb::Blob*      result);

    /// Post to the specified 'semaphore'.
    static void processClose(bslmt::Semaphore* semaphore);
};

void ExampleUtil::processConnect(
    const bsl::shared_ptr<ntci::Connector>& connector,
    const ntca::ConnectEvent&               event,
    bslmt::Semaphore*                       semaphore)
{
    BSLS_ASSERT(event.type() == ntca::ConnectEventType::e_COMPLETE);
    semaphore->post();
}

void ExampleUtil::processAccept(
    const bsl::shared_ptr<ntci::Acceptor>&     acceptor,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::AcceptEvent&                   event,
    bslmt::Semaphore*                          semaphore,
    bsl::shared_ptr<ntci::StreamSocket>*       result)
{
    BSLS_ASSERT(event.type() == ntca::AcceptEventType::e_COMPLETE);
    *result = streamSocket;
    semaphore->post();
}

void ExampleUtil::processSend(const bsl::shared_ptr<ntci::Sender>& sender,
                              const ntca::SendEvent&               event,
                              bslmt::Semaphore*                    semaphore)
{
    BSLS_ASSERT(event.type() == ntca::SendEventType::e_COMPLETE);
    semaphore->post();
}

void ExampleUtil::processReceive(
    const bsl::shared_ptr<ntci::Receiver>& receiver,
    const bsl::shared_ptr<bdlbb::Blob>&    data,
    const ntca::ReceiveEvent&              event,
    bslmt::Semaphore*                      semaphore,
    bdlbb::Blob*                           result)
{
    BSLS_ASSERT(event.type() == ntca::ReceiveEventType::e_COMPLETE);
    *result = *data;
    semaphore->post();
}

void ExampleUtil::processClose(bslmt::Semaphore* semaphore)
{
    semaphore->post();
}

/// Provide a callback for processing connection events for
/// sockets that are closed while the connection is in progress. This struct
/// is thread safe.
struct CloseUtil {
    /// Assert the specified 'event' indicates an asynchronous 'connect'
    /// operation on the specified 'streamSocket' completed successfully
    /// then post to the specified 'semaphore'.
    static void processConnect(
        const bsl::shared_ptr<ntci::Connector>& connector,
        const ntca::ConnectEvent&               event,
        bslmt::Semaphore*                       semaphore);
};

void CloseUtil::processConnect(
    const bsl::shared_ptr<ntci::Connector>& connector,
    const ntca::ConnectEvent&               event,
    bslmt::Semaphore*                       semaphore)
{
    // Assert the specified 'event' indicates an asynchronous 'connect'
    // operation on the specified 'streamSocket' completed successfully then
    // post to the specified 'semaphore'.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_INFO << "Processing connect event = " << event
                         << NTCI_LOG_STREAM_END;

    // BSLS_ASSERT(event.type() == ntca::ConnectEventType::e_COMPLETE);
    semaphore->post();
}

/// Provide callbacks for datagram socket tests.
struct DatagramSocketUtil {
    static void processReceiveFailed(
        const bsl::shared_ptr<ntci::Receiver>& receiver,
        const bsl::shared_ptr<bdlbb::Blob>&    data,
        const ntca::ReceiveEvent&              event,
        bslmt::Semaphore*                      semaphore);

    static void processReceiveTimeout(
        const bsl::shared_ptr<ntci::Receiver>& receiver,
        const bsl::shared_ptr<bdlbb::Blob>&    data,
        const ntca::ReceiveEvent&              event,
        ntsa::Error::Code                      error,
        bslmt::Semaphore*                      semaphore);

    static void processReceiveCancelled(
        const bsl::shared_ptr<ntci::Receiver>& receiver,
        const bsl::shared_ptr<bdlbb::Blob>&    data,
        const ntca::ReceiveEvent&              event,
        bslmt::Semaphore*                      semaphore);

    static void cancelReceive(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ReceiveToken                     token);
};

void DatagramSocketUtil::processReceiveFailed(
    const bsl::shared_ptr<ntci::Receiver>& receiver,
    const bsl::shared_ptr<bdlbb::Blob>&    data,
    const ntca::ReceiveEvent&              event,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing receive from event type %s: %s",
                   ntca::ReceiveEventType::toString(event.type()),
                   event.context().error().text().c_str());

    NTCCFG_TEST_EQ(event.type(), ntca::ReceiveEventType::e_ERROR);
    NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_EOF);

    semaphore->post();
}

void DatagramSocketUtil::processReceiveTimeout(
    const bsl::shared_ptr<ntci::Receiver>& receiver,
    const bsl::shared_ptr<bdlbb::Blob>&    data,
    const ntca::ReceiveEvent&              event,
    ntsa::Error::Code                      error,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing receive from event type %s: %s",
                   ntca::ReceiveEventType::toString(event.type()),
                   event.context().error().text().c_str());

    NTCCFG_TEST_EQ(event.type(), ntca::ReceiveEventType::e_ERROR);
    NTCCFG_TEST_EQ(event.context().error(), error);

    semaphore->post();
}

void DatagramSocketUtil::processReceiveCancelled(
    const bsl::shared_ptr<ntci::Receiver>& receiver,
    const bsl::shared_ptr<bdlbb::Blob>&    data,
    const ntca::ReceiveEvent&              event,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing receive from event type %s: %s",
                   ntca::ReceiveEventType::toString(event.type()),
                   event.context().error().text().c_str());

    NTCCFG_TEST_EQ(event.type(), ntca::ReceiveEventType::e_ERROR);
    NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_CANCELLED);

    semaphore->post();
}

void DatagramSocketUtil::cancelReceive(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReceiveToken                     token)
{
    ntsa::Error error = datagramSocket->cancel(token);
    NTCCFG_TEST_FALSE(error);
}

/// Provide callbacks for listener socket tests.
struct ListenerSocketUtil {
    static void processAcceptTimeout(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const bsl::shared_ptr<ntci::Acceptor>&       acceptor,
        const bsl::shared_ptr<ntci::StreamSocket>&   streamSocket,
        const ntca::AcceptEvent&                     event,
        bslmt::Semaphore*                            semaphore);

    static void processAcceptCancelled(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const bsl::shared_ptr<ntci::Acceptor>&       acceptor,
        const bsl::shared_ptr<ntci::StreamSocket>&   streamSocket,
        const ntca::AcceptEvent&                     event,
        const ntsa::Error::Code                      error,
        bslmt::Semaphore*                            semaphore);

    static void cancelAccept(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptToken                      token);
};

void ListenerSocketUtil::processAcceptTimeout(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const bsl::shared_ptr<ntci::Acceptor>&       acceptor,
    const bsl::shared_ptr<ntci::StreamSocket>&   streamSocket,
    const ntca::AcceptEvent&                     event,
    bslmt::Semaphore*                            semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing accept event type %s: %s",
                   ntca::AcceptEventType::toString(event.type()),
                   event.context().error().text().c_str());

    NTCCFG_TEST_EQ(event.type(), ntca::AcceptEventType::e_ERROR);
    NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_WOULD_BLOCK);

    semaphore->post();
}

void ListenerSocketUtil::processAcceptCancelled(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const bsl::shared_ptr<ntci::Acceptor>&       acceptor,
    const bsl::shared_ptr<ntci::StreamSocket>&   streamSocket,
    const ntca::AcceptEvent&                     event,
    const ntsa::Error::Code                      error,
    bslmt::Semaphore*                            semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing accept event type %s: %s",
                   ntca::AcceptEventType::toString(event.type()),
                   event.context().error().text().c_str());

    NTCCFG_TEST_EQ(event.type(), ntca::AcceptEventType::e_ERROR);
    NTCCFG_TEST_EQ(event.context().error(), error);

    semaphore->post();
}

void ListenerSocketUtil::cancelAccept(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptToken                      token)
{
    ntsa::Error error = listenerSocket->cancel(token);
    NTCCFG_TEST_FALSE(error);
}

/// Provide callbacks for stream socket tests.
struct StreamSocketUtil {
    static void processReceiveTimeout(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Receiver>&     receiver,
        const bsl::shared_ptr<bdlbb::Blob>&        data,
        const ntca::ReceiveEvent&                  event,
        ntsa::Error::Code                          error,
        bslmt::Semaphore*                          semaphore);

    static void processReceiveCancelled(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Receiver>&     receiver,
        const bsl::shared_ptr<bdlbb::Blob>&        data,
        const ntca::ReceiveEvent&                  event,
        bslmt::Semaphore*                          semaphore);

    static void processSendAborted(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Sender>&       sender,
        const ntca::SendEvent&                     event,
        const bsl::string&                         name,
        bslmt::Semaphore*                          semaphore);

    static void processSendSuccessOrTimeout(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Sender>&       sender,
        const ntca::SendEvent&                     event,
        const bsl::string&                         name,
        const ntsa::Error&                         error,
        bslmt::Semaphore*                          semaphore);

    static void processSendSuccessOrCancelled(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Sender>&       sender,
        const ntca::SendEvent&                     event,
        const bsl::string&                         name,
        const ntsa::Error&                         error,
        bslmt::Semaphore*                          semaphore);

    static void processReceiveSuccessOrTimeout(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Receiver>&     receiver,
        const bsl::shared_ptr<bdlbb::Blob>&        data,
        const ntca::ReceiveEvent&                  event,
        const bsl::string&                         name,
        const ntsa::Error&                         error,
        bslmt::Semaphore*                          semaphore);

    static void processReceiveSuccessOrCancelled(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Receiver>&     receiver,
        const bsl::shared_ptr<bdlbb::Blob>&        data,
        const ntca::ReceiveEvent&                  event,
        const bsl::string&                         name,
        const ntsa::Error&                         error,
        bslmt::Semaphore*                          semaphore);

    static void cancelSend(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::SendToken                      token);

    static void cancelReceive(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReceiveToken                   token);
};

void StreamSocketUtil::processReceiveTimeout(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Receiver>&     receiver,
    const bsl::shared_ptr<bdlbb::Blob>&        data,
    const ntca::ReceiveEvent&                  event,
    ntsa::Error::Code                          error,
    bslmt::Semaphore*                          semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing receive event type %s: %s",
                   ntca::ReceiveEventType::toString(event.type()),
                   event.context().error().text().c_str());

    NTCCFG_TEST_EQ(event.type(), ntca::ReceiveEventType::e_ERROR);
    NTCCFG_TEST_EQ(event.context().error(), error);

    semaphore->post();
}

void StreamSocketUtil::processReceiveCancelled(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Receiver>&     receiver,
    const bsl::shared_ptr<bdlbb::Blob>&        data,
    const ntca::ReceiveEvent&                  event,
    bslmt::Semaphore*                          semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing receive event type %s: %s",
                   ntca::ReceiveEventType::toString(event.type()),
                   event.context().error().text().c_str());

    NTCCFG_TEST_EQ(event.type(), ntca::ReceiveEventType::e_ERROR);
    NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_CANCELLED);

    semaphore->post();
}

void StreamSocketUtil::processSendAborted(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Sender>&       sender,
    const ntca::SendEvent&                     event,
    const bsl::string&                         name,
    bslmt::Semaphore*                          semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing send event type %s: %s",
                   ntca::SendEventType::toString(event.type()),
                   event.context().error().text().c_str());

    NTCI_LOG_INFO("Message %s send was aborted", name.c_str());
    NTCCFG_TEST_EQ(event.type(), ntca::SendEventType::e_ERROR);
    NTCCFG_TEST_TRUE(
        (event.context().error() ==
         ntsa::Error::e_CONNECTION_DEAD) ||                      // Reactors
        (event.context().error() == ntsa::Error::e_CANCELLED));  // Proactors

    semaphore->post();
}

void StreamSocketUtil::processSendSuccessOrTimeout(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Sender>&       sender,
    const ntca::SendEvent&                     event,
    const bsl::string&                         name,
    const ntsa::Error&                         error,
    bslmt::Semaphore*                          semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing send event type %s: %s",
                   ntca::SendEventType::toString(event.type()),
                   event.context().error().text().c_str());

    if (error) {
        NTCI_LOG_INFO("Message %s has timed out", name.c_str());
        NTCCFG_TEST_EQ(event.type(), ntca::SendEventType::e_ERROR);
        NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_WOULD_BLOCK);
    }
    else {
        NTCI_LOG_INFO("Message %s has been sent", name.c_str());
    }

    semaphore->post();
}

void StreamSocketUtil::processSendSuccessOrCancelled(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Sender>&       sender,
    const ntca::SendEvent&                     event,
    const bsl::string&                         name,
    const ntsa::Error&                         error,
    bslmt::Semaphore*                          semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing send event type %s: %s",
                   ntca::SendEventType::toString(event.type()),
                   event.context().error().text().c_str());

    if (error) {
        NTCI_LOG_INFO("Message %s has been canceled", name.c_str());
        NTCCFG_TEST_EQ(event.type(), ntca::SendEventType::e_ERROR);
        NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_CANCELLED);
    }
    else {
        NTCI_LOG_INFO("Message %s has been sent", name.c_str());
    }

    semaphore->post();
}

void StreamSocketUtil::processReceiveSuccessOrTimeout(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Receiver>&     receiver,
    const bsl::shared_ptr<bdlbb::Blob>&        data,
    const ntca::ReceiveEvent&                  event,
    const bsl::string&                         name,
    const ntsa::Error&                         error,
    bslmt::Semaphore*                          semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing receive event type %s: %s",
                   ntca::ReceiveEventType::toString(event.type()),
                   event.context().error().text().c_str());

    if (error) {
        NTCI_LOG_INFO("Message %s has timed out", name.c_str());
        NTCCFG_TEST_EQ(event.type(), ntca::ReceiveEventType::e_ERROR);
        NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_WOULD_BLOCK);
    }
    else {
        NTCI_LOG_INFO("Message %s has been received", name.c_str());

        NTCI_LOG_DEBUG("Comparing message %s", name.c_str());

        bsl::size_t position = 0;
        for (int dataBufferIndex = 0; dataBufferIndex < data->numDataBuffers();
             ++dataBufferIndex)
        {
            const bdlbb::BlobBuffer& dataBuffer =
                data->buffer(dataBufferIndex);

            const char* dataPtr = dataBuffer.data();
            int dataSize        = dataBufferIndex == data->numDataBuffers() - 1
                                      ? data->lastDataBufferLength()
                                      : dataBuffer.size();

            for (int dataByteIndex = 0; dataByteIndex < dataSize;
                 ++dataByteIndex)
            {
                char e = ntcd::DataUtil::generateByte(position, 0);
                char f = dataPtr[dataByteIndex];

                if (e != f) {
                    // bsl::cout << "Message:\n"
                    //           << bdlbb::BlobUtilHexDumper(
                    //                                data.get(), 0, 256)
                    //           << bsl::endl;

                    NTCI_LOG_ERROR(
                        "Unexpected byte found at position %zu relative "
                        "offset %d: expected '%c', found '%c'",
                        position,
                        dataByteIndex,
                        e,
                        f);
                }

                NTCCFG_TEST_EQ(f, e);
                ++position;
            }
        }

        NTCI_LOG_DEBUG("Comparing message %s: OK", name.c_str());
    }

    semaphore->post();
}

void StreamSocketUtil::processReceiveSuccessOrCancelled(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Receiver>&     receiver,
    const bsl::shared_ptr<bdlbb::Blob>&        data,
    const ntca::ReceiveEvent&                  event,
    const bsl::string&                         name,
    const ntsa::Error&                         error,
    bslmt::Semaphore*                          semaphore)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Processing receive event type %s: %s",
                   ntca::ReceiveEventType::toString(event.type()),
                   event.context().error().text().c_str());

    if (error) {
        NTCI_LOG_INFO("Message %s has been canceled", name.c_str());
        NTCCFG_TEST_EQ(event.type(), ntca::ReceiveEventType::e_ERROR);
        NTCCFG_TEST_EQ(event.context().error(), ntsa::Error::e_CANCELLED);
    }
    else {
        NTCI_LOG_INFO("Message %s has been received", name.c_str());

        NTCI_LOG_DEBUG("Comparing message %s", name.c_str());

        bsl::size_t position = 0;
        for (int dataBufferIndex = 0; dataBufferIndex < data->numDataBuffers();
             ++dataBufferIndex)
        {
            const bdlbb::BlobBuffer& dataBuffer =
                data->buffer(dataBufferIndex);

            const char* dataPtr = dataBuffer.data();
            int dataSize        = dataBufferIndex == data->numDataBuffers() - 1
                                      ? data->lastDataBufferLength()
                                      : dataBuffer.size();

            for (int dataByteIndex = 0; dataByteIndex < dataSize;
                 ++dataByteIndex)
            {
                char e = ntcd::DataUtil::generateByte(position, 0);
                char f = dataPtr[dataByteIndex];

                if (e != f) {
                    // bsl::cout << "Message:\n"
                    //           << bdlbb::BlobUtilHexDumper(
                    //                                data.get(), 0, 256)
                    //           << bsl::endl;

                    NTCI_LOG_ERROR(
                        "Unexpected byte found at position %zu relative "
                        "offset %d: expected '%c', found '%c'",
                        position,
                        dataByteIndex,
                        e,
                        f);
                }

                NTCCFG_TEST_EQ(f, e);
                ++position;
            }
        }

        NTCI_LOG_DEBUG("Comparing message %s: OK", name.c_str());
    }

    semaphore->post();
}

void StreamSocketUtil::cancelSend(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::SendToken                      token)
{
    ntsa::Error error = streamSocket->cancel(token);
    NTCCFG_TEST_FALSE(error);
}

void StreamSocketUtil::cancelReceive(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReceiveToken                   token)
{
    ntsa::Error error = streamSocket->cancel(token);
    NTCCFG_TEST_FALSE(error);
}

//                              CONCERNS

typedef NTCCFG_FUNCTION(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                        bslma::Allocator* allocator) ConcernCallback;

void concern(const ConcernCallback& concernCallback,
             bslma::Allocator*      allocator)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    const bsl::size_t MIN_THREADS = 4;
    const bsl::size_t MAX_THREADS = 4;
    const bsl::size_t LOAD_FACTOR = 10000;

#else

    const bsl::size_t MIN_THREADS = 2;
    const bsl::size_t MAX_THREADS = 2;
    const bsl::size_t LOAD_FACTOR = 10000;

#endif

    const bool FORCE_ZERO_COPY[] = {false, true};

    NTCCFG_FOREACH(bsl::size_t, forceZeroCopyIndex, 0, 2)
    {
        const bool forceZeroCopy = FORCE_ZERO_COPY[forceZeroCopyIndex];

#if !defined(BSLS_PLATFORM_OS_LINUX)
        if (forceZeroCopy == true) {
            continue;
        }
#endif

        const bool DYNAMIC_LOAD_BALANCING[] = {false, true};

        NTCCFG_FOREACH(bsl::size_t, dynamicLoadBalancingIndex, 0, 2)
        {
            const bool dynamicLoadBalancing =
                DYNAMIC_LOAD_BALANCING[dynamicLoadBalancingIndex];

#if defined(NTCF_SYSTEM_TEST_DYNAMIC_LOAD_BALANCING)
            if (dynamicLoadBalancing !=
                NTCF_SYSTEM_TEST_DYNAMIC_LOAD_BALANCING)
            {
                continue;
            }
#endif

#if NTC_BUILD_WITH_DYNAMIC_LOAD_BALANCING == 0
            if (dynamicLoadBalancing) {
                continue;
            }
#endif

            bsl::vector<bsl::string> driverTypes;
            ntcf::System::loadDriverSupport(&driverTypes,
                                            dynamicLoadBalancing);

            NTCCFG_FOREACH(bsl::size_t, driverTypeIndex, 0, driverTypes.size())
            {
                bsl::string driverType = driverTypes[driverTypeIndex];

#if defined(NTCF_SYSTEM_TEST_DRIVER_TYPE)
                if (driverType != NTCF_SYSTEM_TEST_DRIVER_TYPE) {
                    continue;
                }
#endif

                BSLS_LOG_WARN("Testing driver %s (%s), zero-copy: %s",
                              driverType.c_str(),
                              (dynamicLoadBalancing ? "dynamic" : "static"),
                              (forceZeroCopy ? "enabled" : "disabled"));

                ntca::SchedulerConfig schedulerConfig;
                schedulerConfig.setDriverName(driverType);
                schedulerConfig.setThreadName("network");
                schedulerConfig.setMinThreads(MIN_THREADS);
                schedulerConfig.setMaxThreads(MAX_THREADS);
                schedulerConfig.setThreadLoadFactor(LOAD_FACTOR);
                schedulerConfig.setDynamicLoadBalancing(dynamicLoadBalancing);
                if (forceZeroCopy) {
                    schedulerConfig.setZeroCopyThreshold(0);
                }

                bsl::shared_ptr<ntci::Scheduler> scheduler =
                    ntcf::System::createScheduler(schedulerConfig, allocator);

                scheduler->start();

                concernCallback(scheduler, allocator);

                scheduler->shutdown();
                scheduler->linger();
            }
        }
    }
}

void concernExample1(bslma::Allocator* allocator)
{
    // Initialize the library.

    ntcf::System::initialize();
    ntcf::System::ignore(ntscfg::Signal::e_PIPE);

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    // Create and start an I/O thread.

    ntca::ThreadConfig threadConfig;
    threadConfig.setThreadName("example");

    bsl::shared_ptr<ntci::Thread> thread =
        ntcf::System::createThread(threadConfig);

    error = thread->start();
    BSLS_ASSERT(!error);

    // Create a listener socket and begin listening.

    ntca::ListenerSocketOptions listenerSocketOptions;
    listenerSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
    listenerSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0));

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        thread->createListenerSocket(listenerSocketOptions);

    error = listenerSocket->open();
    BSLS_ASSERT(!error);

    error = listenerSocket->listen();
    NTCCFG_TEST_OK(error);

    // Connect a socket to the listener.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> clientSocket =
        thread->createStreamSocket(streamSocketOptions);

    ntci::ConnectCallback connectCallback =
        clientSocket->createConnectCallback(
            NTCCFG_BIND(&test::ExampleUtil::processConnect,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        &semaphore));

    error = clientSocket->connect(listenerSocket->sourceEndpoint(),
                                  ntca::ConnectOptions(),
                                  connectCallback);
    BSLS_ASSERT(!error);

    semaphore.wait();

    // Accept a connection from the listener socket's backlog.

    bsl::shared_ptr<ntci::StreamSocket> serverSocket;

    ntci::AcceptCallback acceptCallback = listenerSocket->createAcceptCallback(
        NTCCFG_BIND(&test::ExampleUtil::processAccept,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    &semaphore,
                    &serverSocket));

    error = listenerSocket->accept(ntca::AcceptOptions(), acceptCallback);
    NTCCFG_TEST_OK(error);

    semaphore.wait();

    // Send some data from the client to the server.

    bdlbb::Blob clientData(clientSocket->outgoingBlobBufferFactory().get());
    bdlbb::BlobUtil::append(&clientData, "Hello, world!", 13);

    ntci::SendCallback sendCallback = clientSocket->createSendCallback(
        NTCCFG_BIND(&test::ExampleUtil::processSend,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    &semaphore));

    error = clientSocket->send(clientData, ntca::SendOptions(), sendCallback);
    BSLS_ASSERT(!error);

    semaphore.wait();

    // Receive the expected amount of data from the client.

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setSize(13);

    bdlbb::Blob serverData;

    ntci::ReceiveCallback receiveCallback =
        serverSocket->createReceiveCallback(
            NTCCFG_BIND(&test::ExampleUtil::processReceive,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        &semaphore,
                        &serverData));

    error = serverSocket->receive(receiveOptions, receiveCallback);
    NTCCFG_TEST_OK(error);

    semaphore.wait();

    // Ensure the data received matches the data sent.

    BSLS_ASSERT(bdlbb::BlobUtil::compare(clientData, serverData) == 0);

    // Close the client socket.

    {
        ntci::CloseCallback closeCallback = clientSocket->createCloseCallback(
            NTCCFG_BIND(&test::ExampleUtil::processClose, &semaphore));

        clientSocket->close(closeCallback);
        semaphore.wait();
    }

    // Close the server socket.

    {
        ntci::CloseCallback closeCallback = serverSocket->createCloseCallback(
            NTCCFG_BIND(&test::ExampleUtil::processClose, &semaphore));

        serverSocket->close(closeCallback);
        semaphore.wait();
    }

    // Close the listener socket.

    {
        ntci::CloseCallback closeCallback =
            listenerSocket->createCloseCallback(
                NTCCFG_BIND(&test::ExampleUtil::processClose, &semaphore));

        listenerSocket->close(closeCallback);
        semaphore.wait();
    }

    // Stop the thread.

    thread->shutdown();
    thread->linger();
}

void concernExample2(bslma::Allocator* allocator)
{
    // Initialize the library.

    ntcf::System::initialize();
    ntcf::System::ignore(ntscfg::Signal::e_PIPE);

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    // Create and start pool of I/O threads.

    ntca::SchedulerConfig schedulerConfig;
    schedulerConfig.setThreadName("example");
    schedulerConfig.setMinThreads(3);
    schedulerConfig.setMaxThreads(3);

    bsl::shared_ptr<ntci::Scheduler> scheduler =
        ntcf::System::createScheduler(schedulerConfig);

    scheduler->start();

    // Create a listener socket and begin listening.

    ntca::ListenerSocketOptions listenerSocketOptions;
    listenerSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
    listenerSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0));

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        scheduler->createListenerSocket(listenerSocketOptions);

    error = listenerSocket->open();
    BSLS_ASSERT(!error);

    error = listenerSocket->listen();
    NTCCFG_TEST_OK(error);

    // Connect a socket to the listener.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> clientSocket =
        scheduler->createStreamSocket(streamSocketOptions);

    ntci::ConnectCallback connectCallback =
        clientSocket->createConnectCallback(
            NTCCFG_BIND(&test::ExampleUtil::processConnect,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        &semaphore));

    error = clientSocket->connect(listenerSocket->sourceEndpoint(),
                                  ntca::ConnectOptions(),
                                  connectCallback);
    BSLS_ASSERT(!error);

    semaphore.wait();

    // Accept a connection from the listener socket's backlog.

    bsl::shared_ptr<ntci::StreamSocket> serverSocket;

    ntci::AcceptCallback acceptCallback = listenerSocket->createAcceptCallback(
        NTCCFG_BIND(&test::ExampleUtil::processAccept,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    &semaphore,
                    &serverSocket));

    error = listenerSocket->accept(ntca::AcceptOptions(), acceptCallback);
    NTCCFG_TEST_OK(error);

    semaphore.wait();

    // Send some data from the client to the server.

    bdlbb::Blob clientData(clientSocket->outgoingBlobBufferFactory().get());
    bdlbb::BlobUtil::append(&clientData, "Hello, world!", 13);

    ntci::SendCallback sendCallback = clientSocket->createSendCallback(
        NTCCFG_BIND(&test::ExampleUtil::processSend,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    &semaphore));

    error = clientSocket->send(clientData, ntca::SendOptions(), sendCallback);
    BSLS_ASSERT(!error);

    semaphore.wait();

    // Receive the expected amount of data from the client.

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setSize(13);

    bdlbb::Blob serverData;

    ntci::ReceiveCallback receiveCallback =
        serverSocket->createReceiveCallback(
            NTCCFG_BIND(&test::ExampleUtil::processReceive,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        &semaphore,
                        &serverData));

    error = serverSocket->receive(receiveOptions, receiveCallback);
    NTCCFG_TEST_OK(error);

    semaphore.wait();

    // Ensure the data received matches the data sent.

    BSLS_ASSERT(bdlbb::BlobUtil::compare(clientData, serverData) == 0);

    // Close the client socket.

    {
        ntci::CloseCallback closeCallback = clientSocket->createCloseCallback(
            NTCCFG_BIND(&test::ExampleUtil::processClose, &semaphore));

        clientSocket->close(closeCallback);
        semaphore.wait();
    }

    // Close the server socket.

    {
        ntci::CloseCallback closeCallback = serverSocket->createCloseCallback(
            NTCCFG_BIND(&test::ExampleUtil::processClose, &semaphore));

        serverSocket->close(closeCallback);
        semaphore.wait();
    }

    // Close the listener socket.

    {
        ntci::CloseCallback closeCallback =
            listenerSocket->createCloseCallback(
                NTCCFG_BIND(&test::ExampleUtil::processClose, &semaphore));

        listenerSocket->close(closeCallback);
        semaphore.wait();
    }

    // Stop the thread pool.

    scheduler->shutdown();
    scheduler->linger();
}

void concernExample3(bslma::Allocator* allocator)
{
    // Initialize the library.

    ntcf::System::initialize();
    ntcf::System::ignore(ntscfg::Signal::e_PIPE);

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    // Enable the monitorable registry.

    ntca::MonitorableRegistryConfig monitorableRegistryConfig;

    ntcf::System::enableMonitorableRegistry(monitorableRegistryConfig);

    // Enable the monitorable object of the entire process.

    ntcf::System::enableProcessMetrics();

    // Enable the periodic collection of monitorable objects.

    ntca::MonitorableCollectorConfig monitorableCollectorConfig;
    monitorableCollectorConfig.setThreadName("metrics");
    monitorableCollectorConfig.setPeriod(1);

    ntcf::System::enableMonitorableCollector(monitorableCollectorConfig);

    // Register a publisher of monitorable object statistics to the BSLS log at
    // the INFO severity level.

    ntcf::System::registerMonitorablePublisher(bsls::LogSeverity::e_INFO);

    // Wait one second for the collector to publish statistics for each
    // currently active monitorable object.

    bslmt::ThreadUtil::sleep(bsls::TimeInterval(2));

    // Create and start an I/O thread.

    ntca::ThreadConfig threadConfig;
    threadConfig.setThreadName("incoming");

    bsl::shared_ptr<ntci::Thread> thread =
        ntcf::System::createThread(threadConfig);

    error = thread->start();
    BSLS_ASSERT(!error);

    // Create a listener socket and begin listening.

    ntca::ListenerSocketOptions listenerSocketOptions;
    listenerSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0));

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        thread->createListenerSocket(listenerSocketOptions);

    error = listenerSocket->open();
    BSLS_ASSERT(!error);

    error = listenerSocket->listen();
    NTCCFG_TEST_OK(error);

    // Connect a socket to the listener.

    ntca::StreamSocketOptions streamSocketOptions;

    bsl::shared_ptr<ntci::StreamSocket> clientSocket =
        thread->createStreamSocket(streamSocketOptions);

    ntca::ConnectOptions connectOptions;

    ntci::ConnectCallback connectCallback =
        clientSocket->createConnectCallback(
            NTCCFG_BIND(&test::ExampleUtil::processConnect,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        &semaphore));

    error = clientSocket->connect(listenerSocket->sourceEndpoint(),
                                  connectOptions,
                                  connectCallback);
    BSLS_ASSERT(!error);

    semaphore.wait();

    // Accept a connection from the listener socket's backlog.

    bsl::shared_ptr<ntci::StreamSocket> serverSocket;

    ntci::AcceptCallback acceptCallback = listenerSocket->createAcceptCallback(
        NTCCFG_BIND(&test::ExampleUtil::processAccept,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    &semaphore,
                    &serverSocket));

    error = listenerSocket->accept(ntca::AcceptOptions(), acceptCallback);
    NTCCFG_TEST_OK(error);

    semaphore.wait();

    // Send some data from the client to the server.

    bdlbb::Blob clientData(clientSocket->outgoingBlobBufferFactory().get());
    bdlbb::BlobUtil::append(&clientData, "Hello, world!", 13);

    ntca::SendOptions sendOptions;

    ntci::SendCallback sendCallback = clientSocket->createSendCallback(
        NTCCFG_BIND(&test::ExampleUtil::processSend,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    &semaphore));

    error = clientSocket->send(clientData, sendOptions, sendCallback);
    BSLS_ASSERT(!error);

    semaphore.wait();

    // Receive the expected amount of data from the client.

    bdlbb::Blob serverData;

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setSize(13);

    ntci::ReceiveCallback receiveCallback =
        serverSocket->createReceiveCallback(
            NTCCFG_BIND(&test::ExampleUtil::processReceive,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        &semaphore,
                        &serverData));

    error = serverSocket->receive(receiveOptions, receiveCallback);
    NTCCFG_TEST_OK(error);

    semaphore.wait();

    // Ensure the data received matches the data sent.

    BSLS_ASSERT(bdlbb::BlobUtil::compare(clientData, serverData) == 0);

    // Wait one second for the collector to publish statistics for each
    // currently active monitorable object.

    bslmt::ThreadUtil::sleep(bsls::TimeInterval(2));

    // Register a publisher of monitorable object statistics to the BSLS log at
    // the INFO severity level.

    ntcf::System::deregisterMonitorablePublisher(bsls::LogSeverity::e_INFO);

    // Disable the monitorable collector and registry.

    ntcf::System::disableMonitorableCollector();
    ntcf::System::disableMonitorableRegistry();

    // Close the client socket.

    {
        ntci::CloseCallback closeCallback = clientSocket->createCloseCallback(
            NTCCFG_BIND(&test::ExampleUtil::processClose, &semaphore));

        clientSocket->close(closeCallback);
        semaphore.wait();
    }

    // Close the server socket.

    {
        ntci::CloseCallback closeCallback = serverSocket->createCloseCallback(
            NTCCFG_BIND(&test::ExampleUtil::processClose, &semaphore));

        serverSocket->close(closeCallback);
        semaphore.wait();
    }

    // Close the listener socket.

    {
        ntci::CloseCallback closeCallback =
            listenerSocket->createCloseCallback(
                NTCCFG_BIND(&test::ExampleUtil::processClose, &semaphore));

        listenerSocket->close(closeCallback);
        semaphore.wait();
    }

    // Stop the thread.

    thread->shutdown();
    thread->linger();
}

void concernExample4(bslma::Allocator* allocator)
{
    // Initialize the library.

    ntcf::System::initialize();
    ntcf::System::ignore(ntscfg::Signal::e_PIPE);

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    // Enable the monitorable registry.

    ntca::MonitorableRegistryConfig monitorableRegistryConfig;

    ntcf::System::enableMonitorableRegistry(monitorableRegistryConfig);

    // Enable the monitorable object of the entire process.

    ntcf::System::enableProcessMetrics();

    // Enable the periodic collection of monitorable objects.

    ntca::MonitorableCollectorConfig monitorableCollectorConfig;
    monitorableCollectorConfig.setThreadName("metrics");
    monitorableCollectorConfig.setPeriod(1);

    ntcf::System::enableMonitorableCollector(monitorableCollectorConfig);

    // Register a publisher of monitorable object statistics to the BSLS log at
    // the INFO severity level.

    ntcf::System::registerMonitorablePublisher(bsls::LogSeverity::e_INFO);

    // Wait one second for the collector to publish statistics for each
    // currently active monitorable object.

    bslmt::ThreadUtil::sleep(bsls::TimeInterval(2));

    // Create and start pool of I/O threads.

    ntca::SchedulerConfig schedulerConfig;
    schedulerConfig.setThreadName("incoming");
    schedulerConfig.setMinThreads(3);
    schedulerConfig.setMaxThreads(3);
    schedulerConfig.setDriverMetrics(true);
    schedulerConfig.setDriverMetricsPerWaiter(true);
    schedulerConfig.setSocketMetrics(true);
    schedulerConfig.setSocketMetricsPerHandle(false);

    bsl::shared_ptr<ntci::DataPool> dataPool =
        ntcf::System::createDataPool(4096, 4096);

    bsl::shared_ptr<ntci::Scheduler> scheduler =
        ntcf::System::createScheduler(schedulerConfig, dataPool);

    scheduler->start();

    // Create a listener socket and begin listening.

    ntca::ListenerSocketOptions listenerSocketOptions;
    listenerSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0));

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        scheduler->createListenerSocket(listenerSocketOptions);

    error = listenerSocket->open();
    BSLS_ASSERT(!error);

    error = listenerSocket->listen();
    NTCCFG_TEST_OK(error);

    // Connect a socket to the listener.

    ntca::StreamSocketOptions streamSocketOptions;

    bsl::shared_ptr<ntci::StreamSocket> clientSocket =
        scheduler->createStreamSocket(streamSocketOptions);

    ntca::ConnectOptions connectOptions;

    ntci::ConnectCallback connectCallback =
        clientSocket->createConnectCallback(
            NTCCFG_BIND(&test::ExampleUtil::processConnect,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        &semaphore));

    error = clientSocket->connect(listenerSocket->sourceEndpoint(),
                                  connectOptions,
                                  connectCallback);
    BSLS_ASSERT(!error);

    semaphore.wait();

    // Accept a connection from the listener socket's backlog.

    bsl::shared_ptr<ntci::StreamSocket> serverSocket;

    ntci::AcceptCallback acceptCallback = listenerSocket->createAcceptCallback(
        NTCCFG_BIND(&test::ExampleUtil::processAccept,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    &semaphore,
                    &serverSocket));

    error = listenerSocket->accept(ntca::AcceptOptions(), acceptCallback);
    NTCCFG_TEST_OK(error);

    semaphore.wait();

    // Send some data from the client to the server.

    bdlbb::Blob clientData(clientSocket->outgoingBlobBufferFactory().get());
    bdlbb::BlobUtil::append(&clientData, "Hello, world!", 13);

    ntca::SendOptions sendOptions;

    ntci::SendCallback sendCallback = clientSocket->createSendCallback(
        NTCCFG_BIND(&test::ExampleUtil::processSend,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    &semaphore));

    error = clientSocket->send(clientData, sendOptions, sendCallback);
    BSLS_ASSERT(!error);

    semaphore.wait();

    // Receive the expected amount of data from the client.

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setSize(13);

    bdlbb::Blob serverData;

    ntci::ReceiveCallback receiveCallback =
        serverSocket->createReceiveCallback(
            NTCCFG_BIND(&test::ExampleUtil::processReceive,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        &semaphore,
                        &serverData));

    error = serverSocket->receive(receiveOptions, receiveCallback);
    NTCCFG_TEST_OK(error);

    semaphore.wait();

    // Ensure the data received matches the data sent.

    BSLS_ASSERT(bdlbb::BlobUtil::compare(clientData, serverData) == 0);

    // Wait one second for the collector to publish statistics for each
    // currently active monitorable object.

    bslmt::ThreadUtil::sleep(bsls::TimeInterval(2));

    // Register a publisher of monitorable object statistics to the BSLS log at
    // the INFO severity level.

    ntcf::System::deregisterMonitorablePublisher(bsls::LogSeverity::e_INFO);

    // Disable the monitorable collector and registry.

    ntcf::System::disableMonitorableCollector();
    ntcf::System::disableMonitorableRegistry();

    // Close the client socket.

    {
        ntci::CloseCallback closeCallback = clientSocket->createCloseCallback(
            NTCCFG_BIND(&test::ExampleUtil::processClose, &semaphore));

        clientSocket->close(closeCallback);
        semaphore.wait();
    }

    // Close the server socket.

    {
        ntci::CloseCallback closeCallback = serverSocket->createCloseCallback(
            NTCCFG_BIND(&test::ExampleUtil::processClose, &semaphore));

        serverSocket->close(closeCallback);
        semaphore.wait();
    }

    // Close the listener socket.

    {
        ntci::CloseCallback closeCallback =
            listenerSocket->createCloseCallback(
                NTCCFG_BIND(&test::ExampleUtil::processClose, &semaphore));

        listenerSocket->close(closeCallback);
        semaphore.wait();
    }

    // Stop the thread pool.

    scheduler->shutdown();
    scheduler->linger();
}

void concernResolverGetIpAddress(bslma::Allocator* allocator)
{
    // Concern: Test 'Resolver::getIpAddress'
    // Plan:

    NTCI_LOG_CONTEXT();

    const bsl::size_t k_NUM_ITERATIONS = 2;

    ntsa::Error error;

    // Define a resolver configuration with the DNS client enabled and the
    // operating system name service functions disabled.

    ntca::ResolverConfig resolverConfig;
    resolverConfig.setClientEnabled(true);
    resolverConfig.setSystemEnabled(false);

    // Create a start a resolver.

    bsl::shared_ptr<ntci::Resolver> resolver =
        ntcf::System::createResolver(resolverConfig, allocator);

    error = resolver->start();
    NTCCFG_TEST_OK(error);

    for (bsl::size_t i = 0; i < k_NUM_ITERATIONS; ++i) {
        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetIpAddressCallback callback =
            resolver->createGetIpAddressCallback(
                bdlf::BindUtil::bind(
                    &test::ResolverUtil::processGetIpAddressResult,
                    bdlf::PlaceHolders::_1,
                    bdlf::PlaceHolders::_2,
                    bdlf::PlaceHolders::_3,
                    &semaphore),
                allocator);

        // Define the options.

        ntca::GetIpAddressOptions options;
        options.setIpAddressType(ntsa::IpAddressType::e_V4);

        // Get the IP addresses assigned to "google.com".

        error = resolver->getIpAddress("google.com", options, callback);
        NTCCFG_TEST_OK(error);

        semaphore.wait();
    }

    // Stop the resolver.

    resolver->shutdown();
    resolver->linger();
}

void concernResolverGetDomainName(bslma::Allocator* allocator)
{
    // Concern: Test 'Resolver::getDomainName'
    // Plan:

    NTCI_LOG_CONTEXT();

    const bsl::size_t k_NUM_ITERATIONS = 2;

    ntsa::Error error;

    // Define a resolver configuration with the DNS client enabled and the
    // operating system name service functions disabled.

    ntca::ResolverConfig resolverConfig;
    resolverConfig.setClientEnabled(true);
    resolverConfig.setSystemEnabled(false);

    // Create a start a resolver.

    bsl::shared_ptr<ntci::Resolver> resolver =
        ntcf::System::createResolver(resolverConfig, allocator);

    error = resolver->start();
    NTCCFG_TEST_OK(error);

    for (bsl::size_t i = 0; i < k_NUM_ITERATIONS; ++i) {
        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetDomainNameCallback callback =
            resolver->createGetDomainNameCallback(
                bdlf::BindUtil::bind(
                    &test::ResolverUtil::processGetDomainNameResult,
                    bdlf::PlaceHolders::_1,
                    bdlf::PlaceHolders::_2,
                    bdlf::PlaceHolders::_3,
                    &semaphore),
                allocator);

        // Define the options.

        ntca::GetDomainNameOptions options;

        // Get the domain name to which "8.8.8.8" is assigned.

        error = resolver->getDomainName(ntsa::IpAddress("8.8.8.8"),
                                        options,
                                        callback);
        NTCCFG_TEST_OK(error);

        semaphore.wait();
    }

    // Stop the resolver.

    resolver->shutdown();
    resolver->linger();
}

void concernResolverGetPort(bslma::Allocator* allocator)
{
    // Concern: Test 'Resolver::getPort'
    // Plan:

    if (!ntscfg::Platform::hasPortDatabase()) {
        return;
    }

    NTCI_LOG_CONTEXT();

    const bsl::size_t k_NUM_ITERATIONS = 2;

    ntsa::Error error;

    // Define a resolver configuration with the DNS client enabled and the
    // operating system name service functions disabled.

    ntca::ResolverConfig resolverConfig;
    resolverConfig.setClientEnabled(true);
    resolverConfig.setSystemEnabled(false);

    // Create a start a resolver.

    bsl::shared_ptr<ntci::Resolver> resolver =
        ntcf::System::createResolver(resolverConfig, allocator);

    error = resolver->start();
    NTCCFG_TEST_OK(error);

    for (bsl::size_t i = 0; i < k_NUM_ITERATIONS; ++i) {
        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetPortCallback callback = resolver->createGetPortCallback(
            bdlf::BindUtil::bind(&test::ResolverUtil::processGetPortResult,
                                 bdlf::PlaceHolders::_1,
                                 bdlf::PlaceHolders::_2,
                                 bdlf::PlaceHolders::_3,
                                 &semaphore),
            allocator);

        // Define the options.

        ntca::GetPortOptions options;
        options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

        // Get the ports assigned to the "echo" service.

        error = resolver->getPort("echo", options, callback);
        NTCCFG_TEST_OK(error);

        semaphore.wait();
    }

    // Stop the resolver.

    resolver->shutdown();
    resolver->linger();
}

void concernResolverGetServiceName(bslma::Allocator* allocator)
{
    // Concern: Test 'Resolver::getServiceName'
    // Plan:

    if (!ntscfg::Platform::hasPortDatabase()) {
        return;
    }

    NTCI_LOG_CONTEXT();

    const bsl::size_t k_NUM_ITERATIONS = 2;

    ntsa::Error error;

    // Define a resolver configuration with the DNS client enabled and the
    // operating system name service functions disabled.

    ntca::ResolverConfig resolverConfig;
    resolverConfig.setClientEnabled(true);
    resolverConfig.setSystemEnabled(false);

    // Create a start a resolver.

    bsl::shared_ptr<ntci::Resolver> resolver =
        ntcf::System::createResolver(resolverConfig, allocator);

    error = resolver->start();
    NTCCFG_TEST_OK(error);

    for (bsl::size_t i = 0; i < k_NUM_ITERATIONS; ++i) {
        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetServiceNameCallback callback =
            resolver->createGetServiceNameCallback(
                bdlf::BindUtil::bind(
                    &test::ResolverUtil::processGetServiceNameResult,
                    bdlf::PlaceHolders::_1,
                    bdlf::PlaceHolders::_2,
                    bdlf::PlaceHolders::_3,
                    &semaphore),
                allocator);

        // Define the options.

        ntca::GetServiceNameOptions options;
        options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

        // Get the service name to which TCP port 7 is assigned.

        error = resolver->getServiceName(7, options, callback);
        NTCCFG_TEST_OK(error);

        semaphore.wait();
    }

    // Stop the resolver.

    resolver->shutdown();
    resolver->linger();
}

void concernResolverGetEndpoint(bslma::Allocator* allocator)
{
    // Concern: Test 'getEndpoint'
    // Plan:

    NTCI_LOG_CONTEXT();

    const bsl::size_t k_NUM_ITERATIONS = 2;

    ntsa::Error error;

    // Define a resolver configuration with the DNS client enabled and the
    // operating system name service functions disabled.

    ntca::ResolverConfig resolverConfig;
    resolverConfig.setClientEnabled(true);
    resolverConfig.setSystemEnabled(false);

    // Create a start a resolver.

    bsl::shared_ptr<ntci::Resolver> resolver =
        ntcf::System::createResolver(resolverConfig, allocator);

    error = resolver->start();
    NTCCFG_TEST_OK(error);

    for (bsl::size_t i = 0; i < k_NUM_ITERATIONS; ++i) {
        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetEndpointCallback callback =
            resolver->createGetEndpointCallback(
                bdlf::BindUtil::bind(
                    &test::ResolverUtil::processGetEndpointResult,
                    bdlf::PlaceHolders::_1,
                    bdlf::PlaceHolders::_2,
                    bdlf::PlaceHolders::_3,
                    &semaphore),
                allocator);

        // Define the options.

        ntca::GetEndpointOptions options;
        options.setIpAddressType(ntsa::IpAddressType::e_V4);

        // Get the endpoint assigned to "dns.google.com:http".

        error =
            resolver->getEndpoint("dns.google.com:http", options, callback);
        NTCCFG_TEST_OK(error);

        semaphore.wait();
    }

    // Stop the resolver.

    resolver->shutdown();
    resolver->linger();
}

void concernInterfaceResolverGetIpAddressFromOverride(
    bslma::Allocator* allocator)
{
    // Concern: Test 'Resolver::getIpAddress' using an override
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    // Define a resolver configuration with the DNS client enabled and the
    // operating system name service functions disabled.

    ntca::ResolverConfig resolverConfig;

    resolverConfig.setHostDatabaseEnabled(false);
    resolverConfig.setPortDatabaseEnabled(false);
    resolverConfig.setPositiveCacheEnabled(false);
    resolverConfig.setNegativeCacheEnabled(false);
    resolverConfig.setClientEnabled(false);
    resolverConfig.setSystemEnabled(false);

    // Define an scheduler configuration that uses the resolver configuration.

    ntca::SchedulerConfig schedulerConfig;

    schedulerConfig.setThreadName("test");
    schedulerConfig.setMinThreads(1);
    schedulerConfig.setMaxThreads(1);
    schedulerConfig.setResolverConfig(resolverConfig);

    // Create and start an scheduler.

    bsl::shared_ptr<ntci::Scheduler> scheduler =
        ntcf::System::createScheduler(schedulerConfig, allocator);

    error = scheduler->start();
    NTCCFG_TEST_OK(error);

    {
        // Get the resolver used by the scheduler.

        bsl::shared_ptr<ntci::Resolver> resolver = scheduler->resolver();

        // Set overrides.

        error = resolver->addIpAddress("test.example.net",
                                       ntsa::IpAddress("192.168.0.100"));
        NTCCFG_TEST_FALSE(error);

        error = resolver->addIpAddress("test.example.net",
                                       ntsa::IpAddress("192.168.0.101"));
        NTCCFG_TEST_FALSE(error);

        error = resolver->addIpAddress("test.example.net",
                                       ntsa::IpAddress("192.168.0.102"));
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bsl::vector<ntsa::IpAddress> ipAddressList;
        ntca::GetIpAddressEvent      event;
        bslmt::Semaphore             semaphore;

        ntci::GetIpAddressCallback callback =
            resolver->createGetIpAddressCallback(
                NTCCFG_BIND(&test::ResolverUtil::processGetIpAddressEvent,
                            &ipAddressList,
                            &event,
                            &semaphore,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3),
                allocator);

        // Define the options.

        ntca::GetIpAddressOptions options;
        options.setIpAddressType(ntsa::IpAddressType::e_V4);

        // Get the IP addresses assigned to "test.example.net".

        error = resolver->getIpAddress("test.example.net", options, callback);
        NTCCFG_TEST_OK(error);

        semaphore.wait();

        // Verify the result.

        NTCCFG_TEST_OK(event.context().error());

        NTCCFG_TEST_EQ(event.context().source(),
                       ntca::ResolverSource::e_OVERRIDE);

        NTCCFG_TEST_EQ(ipAddressList.size(), 3);

        NTCCFG_TEST_EQ(ipAddressList[0], ntsa::IpAddress("192.168.0.100"));
        NTCCFG_TEST_EQ(ipAddressList[1], ntsa::IpAddress("192.168.0.101"));
        NTCCFG_TEST_EQ(ipAddressList[2], ntsa::IpAddress("192.168.0.102"));
    }

    // Stop the scheduler.

    scheduler->shutdown();
    scheduler->linger();
}

void concernInterfaceResolverGetIpAddressFromSystem(
    bslma::Allocator* allocator)
{
    // TODO
}

void concernInterfaceResolverGetIpAddressFromServer(
    bslma::Allocator* allocator)
{
    // TODO
}

void concernDataExchange(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                         bslma::Allocator*                       allocator)
{
    // Concern: Listen, connect, accept, send and receive
    // Plan:

    NTCI_LOG_CONTEXT();

#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    const bsl::size_t WRITE_QUEUE_HIGH_WATERMARK = 1024 * 512;

    const bsl::size_t NUM_LISTENERS                = 2;
    const bsl::size_t NUM_CONNECTIONS_PER_LISTENER = 5;
    const bsl::size_t MESSAGE_SIZE                 = 1024 * 32;
    const bsl::size_t NUM_MESSAGES                 = 100;

#else

    const bsl::size_t WRITE_QUEUE_HIGH_WATERMARK = 1024 * 512;

    const bsl::size_t NUM_LISTENERS                = 2;
    const bsl::size_t NUM_CONNECTIONS_PER_LISTENER = 2;
    const bsl::size_t MESSAGE_SIZE                 = 1024;
    const bsl::size_t NUM_MESSAGES                 = 10;

#endif

    const bool ENCRYPTION[] = {
        false
#if NTCF_SYSTEM_TEST_BUILD_WITH_TLS
        ,
        true
#endif
    };

    NTCCFG_FOREACH(bsl::size_t,
                   encryptionIndex,
                   0,
                   sizeof(ENCRYPTION) / sizeof(ENCRYPTION[0]))
    {
        const bool encryption = ENCRYPTION[encryptionIndex];

#if defined(NTCF_SYSTEM_TEST_ENCRYPTION)
        if (encryption != NTCF_SYSTEM_TEST_ENCRYPTION) {
            continue;
        }
#endif

        bsl::vector<test::TransferParameters::AddressFamily> addressFamilyList;

#if defined(NTCF_SYSTEM_TEST_ADDRESS_FAMILY)
        addressFamilyList.push_back(NTCF_SYSTEM_TEST_ADDRESS_FAMILY);
#else
        addressFamilyList.push_back(test::TransferParameters::e_IPV4);
        {
            if (ntsu::AdapterUtil::supportsIpv6()) {
                addressFamilyList.push_back(test::TransferParameters::e_IPV6);
            }
        }

        {
            if (ntsu::AdapterUtil::supportsLocalStream()) {
                addressFamilyList.push_back(test::TransferParameters::e_LOCAL);
            }
        }
#endif

        NTCCFG_FOREACH(bsl::size_t,
                       addressFamilyIndex,
                       0,
                       addressFamilyList.size())
        {
            BSLS_LOG_WARN("Testing encryption %d address family %d",
                          (int)(encryption),
                          (int)(addressFamilyList[addressFamilyIndex]));

            test::TransferParameters parameters;

            parameters.d_addressFamily = addressFamilyList[addressFamilyIndex];
            parameters.d_numListeners  = NUM_LISTENERS;
            parameters.d_numConnectionsPerListener =
                NUM_CONNECTIONS_PER_LISTENER;
            parameters.d_readQueueHighWatermark  = (bsl::size_t)(-1);
            parameters.d_writeQueueHighWatermark = WRITE_QUEUE_HIGH_WATERMARK;
            parameters.d_messageSize             = MESSAGE_SIZE;
            parameters.d_numMessages             = NUM_MESSAGES;
            parameters.d_messageVerification     = true;
            parameters.d_encrypted               = encryption;

            if (encryption) {
                ntsa::Error error;

                ntsa::DistinguishedName authorityIdentity;
                authorityIdentity["CN"] = "Certificate Authority";
                authorityIdentity["O"]  = "Bloomberg LP";

                NTCI_LOG_INFO("Generating authority key");
                error = scheduler->generateKey(
                    &parameters.d_authorityPrivateKey_sp,
                    ntca::EncryptionKeyOptions(),
                    allocator);
                NTCCFG_TEST_OK(error);

                ntca::EncryptionCertificateOptions authorityCertificateOptions;

                authorityCertificateOptions.setAuthority(true);

                NTCI_LOG_INFO("Generating authority certificate");
                error = scheduler->generateCertificate(
                    &parameters.d_authorityCertificate_sp,
                    authorityIdentity,
                    parameters.d_authorityPrivateKey_sp,
                    authorityCertificateOptions,
                    allocator);
                NTCCFG_TEST_OK(error);

                ntsa::DistinguishedName clientIdentity;
                clientIdentity["CN"] = "TransferClient User";
                clientIdentity["O"]  = "Bloomberg LP";

                NTCI_LOG_INFO("Generating client key");
                error =
                    scheduler->generateKey(&parameters.d_clientPrivateKey_sp,
                                           ntca::EncryptionKeyOptions(),
                                           allocator);
                NTCCFG_TEST_OK(error);

                NTCI_LOG_INFO("Generating client certificate");
                error = scheduler->generateCertificate(
                    &parameters.d_clientCertificate_sp,
                    clientIdentity,
                    parameters.d_clientPrivateKey_sp,
                    parameters.d_authorityCertificate_sp,
                    parameters.d_authorityPrivateKey_sp,
                    ntca::EncryptionCertificateOptions(),
                    allocator);
                NTCCFG_TEST_OK(error);

                ntsa::DistinguishedName serverIdentity;
                serverIdentity["CN"] = "TransferServer User";
                serverIdentity["O"]  = "Bloomberg LP";

                NTCI_LOG_INFO("Generating server key");
                error =
                    scheduler->generateKey(&parameters.d_serverPrivateKey_sp,
                                           ntca::EncryptionKeyOptions(),
                                           allocator);
                NTCCFG_TEST_OK(error);

                NTCI_LOG_INFO("Generating server certificate");
                error = scheduler->generateCertificate(
                    &parameters.d_serverCertificate_sp,
                    serverIdentity,
                    parameters.d_serverPrivateKey_sp,
                    parameters.d_authorityCertificate_sp,
                    parameters.d_authorityPrivateKey_sp,
                    ntca::EncryptionCertificateOptions(),
                    allocator);
                NTCCFG_TEST_OK(error);
            }

            // TODO: The full test is flaky for local sockets.
            if (addressFamilyList[addressFamilyIndex] ==
                test::TransferParameters::e_LOCAL)
            {
                parameters.d_numListeners              = 1;
                parameters.d_numConnectionsPerListener = 1;
                parameters.d_messageSize               = 32;
                parameters.d_numMessages               = 10;
            }

            parameters.d_message_sp = scheduler->createOutgoingBlob();

            ntcd::DataUtil::generateData(parameters.d_message_sp.get(),
                                         parameters.d_messageSize);

            test::TransferServer server(scheduler, parameters, allocator);

            server.listen();

            bsl::vector<ntsa::Endpoint> endpoints;
            server.getListenerEndpoints(&endpoints);

            test::TransferClient client(scheduler, parameters, allocator);
            NTCCFG_FOREACH_ITERATOR(
                bsl::vector<ntsa::Endpoint>::const_iterator,
                it,
                endpoints)
            {
                const ntsa::Endpoint& remoteEndpoint = *it;
                client.connect(remoteEndpoint);
            }

            client.wait();
            server.wait();
        }
    }
}

void concernClose(bslma::Allocator* allocator)
{
    // Concern: Closing sockets under various conditions.
    // Plan: The
    //

    NTCI_LOG_CONTEXT();

    // Initialize the library.

    ntcf::System::initialize();
    ntcf::System::ignore(ntscfg::Signal::e_PIPE);

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    // Create and start an scheduler.

    ntca::SchedulerConfig schedulerConfig;
    schedulerConfig.setThreadName("example");
    schedulerConfig.setMinThreads(3);
    schedulerConfig.setMaxThreads(3);

    bsl::shared_ptr<ntci::Scheduler> scheduler =
        ntcf::System::createScheduler(schedulerConfig, allocator);

    scheduler->start();

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    // Test closing a stream socket immediately after creating it, without
    // either opening or connecting it.

    NTCI_LOG_INFO("Testing close immediately after creation");

    {
        bsl::shared_ptr<ntci::StreamSocket> clientSocket =
            scheduler->createStreamSocket(streamSocketOptions, allocator);

        ntci::StreamSocketCloseGuard closeGuard(clientSocket);
    }

    // Test closing a stream socket after opening it, without connecting
    // it.

    NTCI_LOG_INFO("Testing close without connect");

    {
        bsl::shared_ptr<ntci::StreamSocket> clientSocket =
            scheduler->createStreamSocket(streamSocketOptions, allocator);

        error = clientSocket->open();
        NTCCFG_TEST_OK(error);

        ntci::StreamSocketCloseGuard closeGuard(clientSocket);
    }

    // Test closing a stream socket during the connection in progress.

    NTCI_LOG_INFO("Testing close during connect");

    {
        bsl::shared_ptr<ntci::StreamSocket> clientSocket =
            scheduler->createStreamSocket(streamSocketOptions, allocator);

        ntca::ConnectOptions connectOptions;

        bslmt::Semaphore      semaphore;
        ntci::ConnectCallback connectCallback =
            clientSocket->createConnectCallback(
                NTCCFG_BIND(&test::CloseUtil::processConnect,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            &semaphore));

        error = clientSocket->connect(ntsa::Endpoint("169.254.177.13:1024"),
                                      connectOptions,
                                      connectCallback);
        NTCCFG_TEST_OK(error);

        {
            clientSocket->close();
            // ntci::StreamSocketCloseGuard closeGuard(clientSocket);
        }

        semaphore.wait();
    }

    // Stop the scheduler.

    scheduler->shutdown();
    scheduler->linger();
}

void concernConnectAndShutdown(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: test that shutdown works without assertions firing when it is
    // called during socket detachment process. As it is impossible to ensure
    // that socket is being detached at the moment timeout value in this test
    // case was selected empirically. There is an assumption that port number
    // for the remote enpoint is never listend by any service (port 51
    // historically is used for Interface Message Processor logical address
    // management).
    // This test should not be unstable on other systems and it should work
    // even if connection is not refused or shutdown is called not during
    // socket detachment process

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 100;
    const int         k_TIMEOUT_MICROSEC        = 100;
    const char*       address                   = "127.0.0.1:51";

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    const ntci::StreamSocketCloseGuard closeGuard(streamSocket);

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);

    const ntsa::Endpoint endpoint(address);

    const ntci::ConnectCallback emptyCb;

    error = streamSocket->connect(endpoint, connectOptions, emptyCb);
    NTCCFG_TEST_OK(error);
    bslmt::ThreadUtil::microSleep(k_TIMEOUT_MICROSEC, 0);

    error = streamSocket->shutdown(ntsa::ShutdownType::e_BOTH,
                                   ntsa::ShutdownMode::e_GRACEFUL);
    NTCCFG_TEST_OK(error);
}

void concernConnectEndpoint1(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                             bslma::Allocator*                       allocator)
{
    // Concern: Connect to endpoint periodically fails but eventually succeeds
    // Testing: ECONNREFUSED, connection established

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 100;

    const double k_RETRY_INTERVAL_SECONDS = 0.1;

    const bsls::TimeInterval k_RETRY_INTERVAL(k_RETRY_INTERVAL_SECONDS);
    const bsls::TimeInterval k_LINGER_INTERVAL(k_RETRY_INTERVAL_SECONDS * 3);

    NTCI_LOG_CONTEXT();

    BSLS_LOG_WARN("ECONNREFUSED/ETIMEDOUT (x4), connection up");

    ntsa::Error error;

    // Create the stream socket.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    ntci::StreamSocketCloseGuard closeGuard(streamSocket);

    // Create the listener socket.

    bsl::shared_ptr<ntsi::ListenerSocket> listenerSocket =
        ntsf::System::createListenerSocket(allocator);

    error = listenerSocket->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTCCFG_TEST_OK(error);

    // Bind the listener socket.

    error =
        listenerSocket->bind(ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                             true);
    NTCCFG_TEST_OK(error);

    ntsa::Endpoint endpoint;
    error = listenerSocket->sourceEndpoint(&endpoint);
    NTCCFG_TEST_OK(error);

    // Connect the stream socket to the listener socket.

    bsl::size_t numErrors = 0;
    NTCCFG_TEST_GT(k_MAX_CONNECTION_ATTEMPTS, 0);

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);
    connectOptions.setRetryInterval(k_RETRY_INTERVAL);

    ntci::ConnectFuture connectFuture(allocator);
    error = streamSocket->connect(endpoint, connectOptions, connectFuture);
    NTCCFG_TEST_OK(error);

    // Since the listening socket is bound, but not listening, to the endpoint
    // to which the stream socket is attempting to connect, ensure there are
    // several failures indicating ECONNREFUSED.

    for (bsl::size_t i = 0; i < 4; ++i) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        if (connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD))
        {
            ++numErrors;
            NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
            bsl::size_t attemptsRemaining =
                k_MAX_CONNECTION_ATTEMPTS - numErrors;

            NTCCFG_TEST_EQ(connectResult.event().context().endpoint(),
                           endpoint);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           attemptsRemaining);
        }
        else {
            NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                  << connectResult.event()
                                  << NTCCFG_TEST_LOG_END;
            NTCCFG_TEST_TRUE(false);
        }
    }

    // Begin listening.

    error = listenerSocket->listen(100);
    NTCCFG_TEST_OK(error);

    // Now that the listener socket is listening, the stream socket should
    // eventually connect.

    while (true) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        if (connectResult.event().type() == ntca::ConnectEventType::e_ERROR) {
            if (connectResult.event().context().error() ==
                    ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
                connectResult.event().context().error() ==
                    ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
                connectResult.event().context().error() ==
                    ntsa::Error(ntsa::Error::e_CONNECTION_DEAD))
            {
                ++numErrors;
                NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
                bsl::size_t attemptsRemaining =
                    k_MAX_CONNECTION_ATTEMPTS - numErrors;

                NTCCFG_TEST_EQ(connectResult.event().context().endpoint(),
                               endpoint);
                NTCCFG_TEST_EQ(
                    connectResult.event().context().attemptsRemaining(),
                    attemptsRemaining);
            }
            else {
                NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                      << connectResult.event()
                                      << NTCCFG_TEST_LOG_END;
                NTCCFG_TEST_TRUE(false);
            }
        }
        else {
            NTCCFG_TEST_EQ(connectResult.event().type(),
                           ntca::ConnectEventType::e_COMPLETE);

            NTCCFG_TEST_OK(connectResult.event().context().error());

            NTCCFG_TEST_EQ(connectResult.event().context().endpoint(),
                           endpoint);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           0);

            NTCCFG_TEST_FALSE(
                connectResult.event().context().latency().isNull());
            NTCCFG_TEST_GT(connectResult.event().context().latency().value(),
                           bsls::TimeInterval());

            break;
        }
    }

    // Since the socket is connected, the stream socket should never reconnect.

    {
        NTCCFG_TEST_LOG_INFO << "Waiting for 3 seconds to ensure no retries "
                                "are attempted"
                             << NTCCFG_TEST_LOG_END;

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(
            &connectResult,
            streamSocket->currentTime() + k_LINGER_INTERVAL);
        NTCCFG_TEST_TRUE(error);
    }
}

void concernConnectEndpoint2(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                             bslma::Allocator*                       allocator)
{
    // Concern: Connect to endpoint periodically refused until all attempts are
    // exhausted
    //
    // Testing: ECONNREFUSED, connection attempt exhaustion

    NTCI_LOG_CONTEXT();

    BSLS_LOG_WARN("ECONNREFUSED/ETIMEDOUT (x5)");

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 5;

    const double k_RETRY_INTERVAL_SECONDS = 0.1;

    const bsls::TimeInterval k_RETRY_INTERVAL(k_RETRY_INTERVAL_SECONDS);
    const bsls::TimeInterval k_LINGER_INTERVAL(k_RETRY_INTERVAL_SECONDS * 3);

    ntsa::Error error;

    // Create the stream socket.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    ntci::StreamSocketCloseGuard closeGuard(streamSocket);

    // Create the listener socket.

    bsl::shared_ptr<ntsi::ListenerSocket> listenerSocket =
        ntsf::System::createListenerSocket(allocator);

    error = listenerSocket->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTCCFG_TEST_OK(error);

    // Bind the listener socket.

    error =
        listenerSocket->bind(ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                             true);
    NTCCFG_TEST_OK(error);

    ntsa::Endpoint endpoint;
    error = listenerSocket->sourceEndpoint(&endpoint);
    NTCCFG_TEST_OK(error);

    // Connect the stream socket to the listener socket.

    bsl::size_t numErrors = 0;
    NTCCFG_TEST_GT(k_MAX_CONNECTION_ATTEMPTS, 0);

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);
    connectOptions.setRetryInterval(k_RETRY_INTERVAL);

    ntci::ConnectFuture connectFuture(allocator);
    error = streamSocket->connect(endpoint, connectOptions, connectFuture);
    NTCCFG_TEST_OK(error);

    // Since the listening socket is bound, but not listening, to the endpoint
    // to which the stream socket is attempting to connect, ensure there are
    // exactly k_MAX_CONNECTION_ATTEMPTS number of failures, each indicating
    // ECONNREFUSED.

    for (bsl::size_t i = 0; i < k_MAX_CONNECTION_ATTEMPTS; ++i) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        if (connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD))
        {
            ++numErrors;
            NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
            bsl::size_t attemptsRemaining =
                k_MAX_CONNECTION_ATTEMPTS - numErrors;

            NTCCFG_TEST_EQ(connectResult.event().context().endpoint(),
                           endpoint);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           attemptsRemaining);
        }
        else {
            NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                  << connectResult.event()
                                  << NTCCFG_TEST_LOG_END;
            NTCCFG_TEST_TRUE(false);
        }
    }

    // Since the listener socket never begins listening, and all retry
    // attempts have been exhausted, the stream socket should never reconnect.

    {
        NTCCFG_TEST_LOG_INFO << "Waiting for 3 seconds to ensure no retries "
                                "are attempted"
                             << NTCCFG_TEST_LOG_END;

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(
            &connectResult,
            streamSocket->currentTime() + k_LINGER_INTERVAL);
        NTCCFG_TEST_TRUE(error);
    }
}

void concernConnectEndpoint3(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                             bslma::Allocator*                       allocator)
{
    // Concern: Connect to endpoint periodically failes due to either timeouts
    // or ICMP messages leading to e_UNREACHABLE error until all attempts are
    // exhausted.
    // Testing: e_CONNECTION_TIMEOUT, e_UNREACHABLE, connection attempt
    // exhaustion

    NTCI_LOG_CONTEXT();

    BSLS_LOG_WARN("e_CONNECTION_TIMEOUT || e_UNREACHABLE (x5)");

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 5;

    const double k_RETRY_INTERVAL_SECONDS = 0.1;

    const bsls::TimeInterval k_RETRY_INTERVAL(k_RETRY_INTERVAL_SECONDS);
    const bsls::TimeInterval k_LINGER_INTERVAL(k_RETRY_INTERVAL_SECONDS * 3);

    ntsa::Error error;

    // Create the stream socket.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    ntci::StreamSocketCloseGuard closeGuard(streamSocket);

    // Connect the stream socket to the listener socket.

    // 240.0.0.1 belongs to class E ipv4 address space and it is assumed that
    // it will never be assigned.

    ntsa::Endpoint endpoint(
        ntsa::IpEndpoint(ntsa::Ipv4Address("240.0.0.1"), 1024));

    bsl::size_t numErrors = 0;
    NTCCFG_TEST_GT(k_MAX_CONNECTION_ATTEMPTS, 0);

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);
    connectOptions.setRetryInterval(k_RETRY_INTERVAL);

    ntci::ConnectFuture connectFuture(allocator);
    error = streamSocket->connect(endpoint, connectOptions, connectFuture);
    NTCCFG_TEST_OK(error);

    // Since we assume there is no machine assigned to 240.0.0.1, the endpoint
    // to which the stream socket is attempting to connect, ensure there are
    // exactly k_MAX_CONNECTION_ATTEMPTS number of failures, each
    // indicating either e_CONNECTION_TIMEOUT or e_UNREACHABLE.

    for (bsl::size_t i = 0; i < k_MAX_CONNECTION_ATTEMPTS; ++i) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        const bool errorIsExpected =
            (connectResult.event().context().error() ==
                 ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
             connectResult.event().context().error() ==
                 ntsa::Error(ntsa::Error::e_UNREACHABLE) ||
             connectResult.event().context().error() ==
                 ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED));
        NTCCFG_TEST_TRUE(errorIsExpected);

        ++numErrors;
        NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
        bsl::size_t attemptsRemaining = k_MAX_CONNECTION_ATTEMPTS - numErrors;

        NTCCFG_TEST_EQ(connectResult.event().context().endpoint(), endpoint);
        NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                       attemptsRemaining);
    }

    // Since the listener socket never begins listening, and all retry
    // attempts have been exhausted, the stream socket should never reconnect.

    {
        NTCCFG_TEST_LOG_INFO << "Waiting for 3 seconds to ensure no retries "
                                "are attempted"
                             << NTCCFG_TEST_LOG_END;

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(
            &connectResult,
            streamSocket->currentTime() + k_LINGER_INTERVAL);
        NTCCFG_TEST_TRUE(error);
    }
}

void concernConnectEndpoint4(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                             bslma::Allocator*                       allocator)
{
    // Concern: Connections to endpoints may be cancelled while waiting to
    // begin connecting.
    //
    // Testing: ECONNREFUSED, ECANCELED.

    NTCI_LOG_CONTEXT();

    BSLS_LOG_WARN("ECONNREFUSED (x4), ECANCELED");

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 100;

    const double k_RETRY_INTERVAL_SECONDS = 0.1;

    const bsls::TimeInterval k_RETRY_INTERVAL(k_RETRY_INTERVAL_SECONDS);
    const bsls::TimeInterval k_LINGER_INTERVAL(k_RETRY_INTERVAL_SECONDS * 3);

    ntsa::Error error;

    // Create the stream socket.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    ntci::StreamSocketCloseGuard closeGuard(streamSocket);

    // Connect the stream socket to the listener socket.

    ntsa::Endpoint endpoint(
        ntsa::IpEndpoint(ntsa::Ipv4Address("127.0.0.1"), 1024));

    bsl::size_t numErrors = 0;
    NTCCFG_TEST_GT(k_MAX_CONNECTION_ATTEMPTS, 0);

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);
    connectOptions.setRetryInterval(k_RETRY_INTERVAL);

    ntci::ConnectFuture connectFuture(allocator);
    error = streamSocket->connect(endpoint, connectOptions, connectFuture);
    NTCCFG_TEST_OK(error);

    // Ideally, we'd like to bind to an ephemeral port on the loopback
    // scheduler but not begin listening, then have connection attempts
    // made to that address fail with ECONNREFUSED. However, only
    // Linux has this behavior. All other platforms just time out the
    // connection attempt.

    for (bsl::size_t i = 0; i < 4; ++i) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        if (connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD))
        {
            ++numErrors;
            NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
            bsl::size_t attemptsRemaining =
                k_MAX_CONNECTION_ATTEMPTS - numErrors;

            NTCCFG_TEST_EQ(connectResult.event().context().endpoint(),
                           endpoint);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           attemptsRemaining);
        }
        else {
            NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                  << connectResult.event()
                                  << NTCCFG_TEST_LOG_END;
            NTCCFG_TEST_TRUE(false);
        }
    }

    // Close the connection.

    streamSocket->close();

    // The connection should eventually be cancelled.

    while (true) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        if (connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD))
        {
            ++numErrors;
            NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
            bsl::size_t attemptsRemaining =
                k_MAX_CONNECTION_ATTEMPTS - numErrors;

            NTCCFG_TEST_EQ(connectResult.event().context().endpoint(),
                           endpoint);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           attemptsRemaining);
        }
        else if (connectResult.event().context().error() ==
                 ntsa::Error(ntsa::Error::e_CANCELLED))
        {
            NTCCFG_TEST_EQ(connectResult.event().context().endpoint(),
                           endpoint);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           0);

            break;
        }
        else {
            NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                  << connectResult.event()
                                  << NTCCFG_TEST_LOG_END;
            NTCCFG_TEST_TRUE(false);
        }
    }

    // Since the stream socket has been closed during a connection attempt,
    // the stream socket should never reconnect.

    {
        NTCCFG_TEST_LOG_INFO << "Waiting for 3 seconds to ensure no retries "
                                "are attempted"
                             << NTCCFG_TEST_LOG_END;

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(
            &connectResult,
            streamSocket->currentTime() + k_LINGER_INTERVAL);
        NTCCFG_TEST_TRUE(error);
    }
}

void concernConnectEndpoint5(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                             bslma::Allocator*                       allocator)
{
    // Concern: Connections to endpoints may be cancelled while connections
    // have been initiated but not yet complete.
    //
    // Testing: ETIMEDOUT, ECANCELED

    NTCI_LOG_CONTEXT();

    BSLS_LOG_WARN("ETIMEDOUT (x4), ECANCELED");

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 100;

    const double k_RETRY_INTERVAL_SECONDS = 0.1;

    const bsls::TimeInterval k_RETRY_INTERVAL(k_RETRY_INTERVAL_SECONDS);
    const bsls::TimeInterval k_LINGER_INTERVAL(k_RETRY_INTERVAL_SECONDS * 3);

    ntsa::Error error;

    // Create the stream socket.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    ntci::StreamSocketCloseGuard closeGuard(streamSocket);

    // Connect the stream socket to the listener socket.

    ntsa::Endpoint endpoint(
        ntsa::IpEndpoint(ntsa::Ipv4Address("240.0.0.1"), 1024));

    bsl::size_t numErrors = 0;
    NTCCFG_TEST_GT(k_MAX_CONNECTION_ATTEMPTS, 0);

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);
    connectOptions.setRetryInterval(k_RETRY_INTERVAL);

    ntci::ConnectFuture connectFuture(allocator);
    error = streamSocket->connect(endpoint, connectOptions, connectFuture);
    NTCCFG_TEST_OK(error);

    // Since we assume there is no machine assigned to 240.0.0.1, the endpoint
    // to which the stream socket is attempting to connect, ensure there are
    // exactly k_MAX_CONNECTION_ATTEMPTS number of failures, each indicating
    // ETIMEDOUT.

    for (bsl::size_t i = 0; i < 4; ++i) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        if (connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_UNREACHABLE))
        {
            ++numErrors;
            NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
            bsl::size_t attemptsRemaining =
                k_MAX_CONNECTION_ATTEMPTS - numErrors;

            NTCCFG_TEST_EQ(connectResult.event().context().endpoint(),
                           endpoint);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           attemptsRemaining);
        }
        else {
            NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                  << connectResult.event()
                                  << NTCCFG_TEST_LOG_END;
            NTCCFG_TEST_TRUE(false);
        }
    }

    // Close the connection.

    streamSocket->close();

    // The connection should eventually be cancelled.

    while (true) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        if (connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_UNREACHABLE))
        {
            ++numErrors;
            NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
            bsl::size_t attemptsRemaining =
                k_MAX_CONNECTION_ATTEMPTS - numErrors;

            NTCCFG_TEST_EQ(connectResult.event().context().endpoint(),
                           endpoint);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           attemptsRemaining);
        }
        else if (connectResult.event().context().error() ==
                 ntsa::Error(ntsa::Error::e_CANCELLED))
        {
            NTCCFG_TEST_EQ(connectResult.event().context().endpoint(),
                           endpoint);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           0);

            break;
        }
        else {
            NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                  << connectResult.event()
                                  << NTCCFG_TEST_LOG_END;
            NTCCFG_TEST_TRUE(false);
        }
    }

    // Since the stream socket has been closed during a connection attempt,
    // the stream socket should never reconnect.

    {
        NTCCFG_TEST_LOG_INFO << "Waiting for 3 seconds to ensure no retries "
                                "are attempted"
                             << NTCCFG_TEST_LOG_END;

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(
            &connectResult,
            streamSocket->currentTime() + k_LINGER_INTERVAL);
        NTCCFG_TEST_TRUE(error);
    }
}

void concernConnectEndpoint6(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                             bslma::Allocator*                       allocator)
{
    // Concern: Connections to endpoints may reach their deadline while waiting
    // to begin connecting.
    //
    // Testing: ECONNREFUSED, ETIMEDOUT/ECANCELED.

    NTCI_LOG_CONTEXT();

    BSLS_LOG_WARN("ECONNREFUSED (x4), ETIMEDOUT/ECANCELED");

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 100;

    const double k_RETRY_INTERVAL_SECONDS = 0.1;

    const bsls::TimeInterval k_RETRY_INTERVAL(k_RETRY_INTERVAL_SECONDS);
    const bsls::TimeInterval k_LINGER_INTERVAL(k_RETRY_INTERVAL_SECONDS * 3);
    const bsls::TimeInterval k_DEADLINE_INTERVAL(k_RETRY_INTERVAL_SECONDS * 5);

    ntsa::Error error;

    // Create the stream socket.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    ntci::StreamSocketCloseGuard closeGuard(streamSocket);

    // Connect the stream socket to the listener socket.

    ntsa::Endpoint endpoint(
        ntsa::IpEndpoint(ntsa::Ipv4Address("127.0.0.1"), 1024));

    bsl::size_t numErrors = 0;
    NTCCFG_TEST_GT(k_MAX_CONNECTION_ATTEMPTS, 0);

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);
    connectOptions.setRetryInterval(k_RETRY_INTERVAL);
    connectOptions.setDeadline(streamSocket->currentTime() +
                               k_DEADLINE_INTERVAL);

    ntci::ConnectFuture connectFuture(allocator);
    error = streamSocket->connect(endpoint, connectOptions, connectFuture);
    NTCCFG_TEST_OK(error);

    // Ideally, we'd like to bind to an ephemeral port on the loopback
    // scheduler but not begin listening, then have connection attempts
    // made to that address fail with ECONNREFUSED. However, only
    // Linux has this behavior. All other platforms just time out the
    // connection attempt.

    while (true) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        if (connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD))
        {
            ++numErrors;

            NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
            bsl::size_t attemptsRemaining =
                k_MAX_CONNECTION_ATTEMPTS - numErrors;

            NTCCFG_TEST_EQ(connectResult.event().context().endpoint(),
                           endpoint);

            if (connectResult.event().context().attemptsRemaining() == 0) {
                break;
            }
            else {
                NTCCFG_TEST_EQ(
                    connectResult.event().context().attemptsRemaining(),
                    attemptsRemaining);
            }
        }
        else if (connectResult.event().context().error() ==
                 ntsa::Error(ntsa::Error::e_CANCELLED))
        {
            NTCCFG_TEST_EQ(connectResult.event().context().endpoint(),
                           endpoint);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           0);

            break;
        }
        else {
            NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                  << connectResult.event()
                                  << NTCCFG_TEST_LOG_END;
            NTCCFG_TEST_TRUE(false);
        }
    }

    // Since the stream socket has been closed during a connection attempt,
    // the stream socket should never reconnect.

    {
        NTCCFG_TEST_LOG_INFO << "Waiting for 3 seconds to ensure no retries "
                                "are attempted"
                             << NTCCFG_TEST_LOG_END;

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(
            &connectResult,
            streamSocket->currentTime() + k_LINGER_INTERVAL);
        NTCCFG_TEST_TRUE(error);
    }
}

void concernConnectEndpoint7(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                             bslma::Allocator*                       allocator)
{
    // Concern: Connections to endpoints may be reach their deadline while
    // connections have been initiated but not yet complete.
    //
    // Testing: ETIMEDOUT, ETIMEDOUT/ECANCELED

    NTCI_LOG_CONTEXT();

    BSLS_LOG_WARN("ETIMEDOUT (x4), ETIMEDOUT/ECANCELED");

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 100;

    const double k_RETRY_INTERVAL_SECONDS = 0.1;

    const bsls::TimeInterval k_RETRY_INTERVAL(k_RETRY_INTERVAL_SECONDS);
    const bsls::TimeInterval k_LINGER_INTERVAL(k_RETRY_INTERVAL_SECONDS * 3);
    const bsls::TimeInterval k_DEADLINE_INTERVAL(k_RETRY_INTERVAL_SECONDS * 5);

    ntsa::Error error;

    // Create the stream socket.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    ntci::StreamSocketCloseGuard closeGuard(streamSocket);

    // Connect the stream socket to the listener socket.

    ntsa::Endpoint endpoint(
        ntsa::IpEndpoint(ntsa::Ipv4Address("240.0.0.1"), 1024));

    bsl::size_t numErrors = 0;
    NTCCFG_TEST_GT(k_MAX_CONNECTION_ATTEMPTS, 0);

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);
    connectOptions.setRetryInterval(k_RETRY_INTERVAL);
    connectOptions.setDeadline(streamSocket->currentTime() +
                               k_DEADLINE_INTERVAL);

    ntci::ConnectFuture connectFuture(allocator);
    error = streamSocket->connect(endpoint, connectOptions, connectFuture);
    NTCCFG_TEST_OK(error);

    // Since we assume there is no machine assigned to 240.0.0.1, the endpoint
    // to which the stream socket is attempting to connect, ensure there are
    // exactly k_MAX_CONNECTION_ATTEMPTS number of failures, each indicating
    // ETIMEDOUT.

    while (true) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        if (connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_UNREACHABLE))
        {
            ++numErrors;
            NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
            bsl::size_t attemptsRemaining =
                k_MAX_CONNECTION_ATTEMPTS - numErrors;

            NTCCFG_TEST_EQ(connectResult.event().context().endpoint(),
                           endpoint);

            if (connectResult.event().context().attemptsRemaining() == 0) {
                break;
            }
            else {
                NTCCFG_TEST_EQ(
                    connectResult.event().context().attemptsRemaining(),
                    attemptsRemaining);
            }
        }
        else if (connectResult.event().context().error() ==
                 ntsa::Error(ntsa::Error::e_CANCELLED))
        {
            NTCCFG_TEST_EQ(connectResult.event().context().endpoint(),
                           endpoint);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           0);

            break;
        }
        else {
            NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                  << connectResult.event()
                                  << NTCCFG_TEST_LOG_END;
            NTCCFG_TEST_TRUE(false);
        }
    }

    // Since the stream socket has been closed during a connection attempt,
    // the stream socket should never reconnect.

    {
        NTCCFG_TEST_LOG_INFO << "Waiting for 3 seconds to ensure no retries "
                                "are attempted"
                             << NTCCFG_TEST_LOG_END;

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(
            &connectResult,
            streamSocket->currentTime() + k_LINGER_INTERVAL);
        NTCCFG_TEST_TRUE(error);
    }
}

void concernConnectEndpoint8(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                             bslma::Allocator*                       allocator)
{
    // Concern: Connect to endpoint periodically times out nearly
    // instantaneously or the timer races with the system refusing or aborting
    // a connection to an endpoint at which there is no listener, until all
    // attempts are exhausted (see also 'concernConnectEndpoint3'.)
    //
    // Testing: ETIMEDOUT/ECONNREFUSED/ECONNABORTED,
    //          connection attempt exhaustion

    NTCI_LOG_CONTEXT();

    BSLS_LOG_WARN(
        "ETIMEDOUT/ECONNREFUSED/ECONNABORTED (x100) (instantaneous)");

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 100;

    const double k_RETRY_INTERVAL_SECONDS = 0.1;

    const bsls::TimeInterval k_RETRY_INTERVAL(0, 1);
    const bsls::TimeInterval k_LINGER_INTERVAL(k_RETRY_INTERVAL_SECONDS * 3);

    ntsa::Error error;

    // Create the stream socket.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    ntci::StreamSocketCloseGuard closeGuard(streamSocket);

    // Connect the stream socket to the listener socket.

    ntsa::Endpoint endpoint(
        ntsa::IpEndpoint(ntsa::Ipv4Address("127.0.0.1"), 1024));

    bsl::size_t numErrors = 0;
    NTCCFG_TEST_GT(k_MAX_CONNECTION_ATTEMPTS, 0);

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);
    connectOptions.setRetryInterval(k_RETRY_INTERVAL);

    ntci::ConnectFuture connectFuture(allocator);
    error = streamSocket->connect(endpoint, connectOptions, connectFuture);
    NTCCFG_TEST_OK(error);

    // Since we assume there is no listener assigned to 127.0.0.1:1024, the
    // endpoint to which the stream socket is attempting to connect, is
    // refused by the operating system or instantaneously times out, ensure
    // there are exactly k_MAX_CONNECTION_ATTEMPTS number of failures, each
    // indicating ECONNREFUSED or ETIMEDOUT.

    for (bsl::size_t i = 0; i < k_MAX_CONNECTION_ATTEMPTS; ++i) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        if (connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD))
        {
            ++numErrors;
            NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
            bsl::size_t attemptsRemaining =
                k_MAX_CONNECTION_ATTEMPTS - numErrors;

            NTCCFG_TEST_EQ(connectResult.event().context().endpoint(),
                           endpoint);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           attemptsRemaining);
        }
        else {
            NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                  << connectResult.event()
                                  << NTCCFG_TEST_LOG_END;
            NTCCFG_TEST_TRUE(false);
        }
    }

    // Since the listener socket never begins listening, and all retry
    // attempts have been exhausted, the stream socket should never reconnect.

    {
        NTCCFG_TEST_LOG_INFO << "Waiting for 3 seconds to ensure no retries "
                                "are attempted"
                             << NTCCFG_TEST_LOG_END;

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(
            &connectResult,
            streamSocket->currentTime() + k_LINGER_INTERVAL);
        NTCCFG_TEST_TRUE(error);
    }
}

void concernConnectName1(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                         bslma::Allocator*                       allocator)
{
    // Concern: Connect to name periodically fails but eventually succeeds
    // Testing: ECONNREFUSED, connection established

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 100;

    const double k_RETRY_INTERVAL_SECONDS = 0.1;

    const bsls::TimeInterval k_RETRY_INTERVAL(k_RETRY_INTERVAL_SECONDS);
    const bsls::TimeInterval k_LINGER_INTERVAL(k_RETRY_INTERVAL_SECONDS * 3);

    NTCI_LOG_CONTEXT();

    BSLS_LOG_WARN("ECONNREFUSED/ETIMEDOUT (x4), connection up");

    ntsa::Error error;

    // Create the stream socket.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    ntci::StreamSocketCloseGuard closeGuard(streamSocket);

    // Create the listener socket.

    bsl::shared_ptr<ntsi::ListenerSocket> listenerSocket =
        ntsf::System::createListenerSocket(allocator);

    error = listenerSocket->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTCCFG_TEST_OK(error);

    // Bind the listener socket.

    error =
        listenerSocket->bind(ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                             true);
    NTCCFG_TEST_OK(error);

    ntsa::Endpoint endpoint;
    error = listenerSocket->sourceEndpoint(&endpoint);
    NTCCFG_TEST_OK(error);

    // Connect the stream socket to the listener socket.

    bsl::size_t numErrors = 0;
    NTCCFG_TEST_GT(k_MAX_CONNECTION_ATTEMPTS, 0);

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);
    connectOptions.setRetryInterval(k_RETRY_INTERVAL);

    bsl::string connectName =
        "localhost:" + bsl::to_string(endpoint.ip().port());

    ntci::ConnectFuture connectFuture(allocator);
    error = streamSocket->connect(connectName, connectOptions, connectFuture);
    NTCCFG_TEST_OK(error);

    // Since the listening socket is bound, but not listening, to the endpoint
    // to which the stream socket is attempting to connect, ensure there are
    // several failures indicating ECONNREFUSED.

    for (bsl::size_t i = 0; i < 4; ++i) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        if (connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD))
        {
            ++numErrors;
            NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
            bsl::size_t attemptsRemaining =
                k_MAX_CONNECTION_ATTEMPTS - numErrors;

            NTCCFG_TEST_EQ(connectResult.event().context().name(),
                           connectName);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           attemptsRemaining);
        }
        else {
            NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                  << connectResult.event()
                                  << NTCCFG_TEST_LOG_END;
            NTCCFG_TEST_TRUE(false);
        }
    }

    // Begin listening.

    error = listenerSocket->listen(100);
    NTCCFG_TEST_OK(error);

    // Now that the listener socket is listening, the stream socket should
    // eventually connect.

    while (true) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        if (connectResult.event().type() == ntca::ConnectEventType::e_ERROR) {
            if (connectResult.event().context().error() ==
                    ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
                connectResult.event().context().error() ==
                    ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
                connectResult.event().context().error() ==
                    ntsa::Error(ntsa::Error::e_CONNECTION_DEAD))
            {
                ++numErrors;
                NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
                bsl::size_t attemptsRemaining =
                    k_MAX_CONNECTION_ATTEMPTS - numErrors;

                NTCCFG_TEST_EQ(connectResult.event().context().name(),
                               connectName);
                NTCCFG_TEST_EQ(
                    connectResult.event().context().attemptsRemaining(),
                    attemptsRemaining);
            }
            else {
                NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                      << connectResult.event()
                                      << NTCCFG_TEST_LOG_END;
                NTCCFG_TEST_TRUE(false);
            }
        }
        else {
            NTCCFG_TEST_EQ(connectResult.event().type(),
                           ntca::ConnectEventType::e_COMPLETE);

            NTCCFG_TEST_OK(connectResult.event().context().error());

            NTCCFG_TEST_EQ(connectResult.event().context().name(),
                           connectName);
            NTCCFG_TEST_EQ(connectResult.event().context().endpoint(),
                           endpoint);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           0);

            NTCCFG_TEST_FALSE(
                connectResult.event().context().latency().isNull());
            NTCCFG_TEST_GT(connectResult.event().context().latency().value(),
                           bsls::TimeInterval());

            break;
        }
    }

    // Since the socket is connected, the stream socket should never reconnect.

    {
        NTCCFG_TEST_LOG_INFO << "Waiting for 3 seconds to ensure no retries "
                                "are attempted"
                             << NTCCFG_TEST_LOG_END;

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(
            &connectResult,
            streamSocket->currentTime() + k_LINGER_INTERVAL);
        NTCCFG_TEST_TRUE(error);
    }
}

void concernConnectName2(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                         bslma::Allocator*                       allocator)
{
    // Concern: Connect to name periodically refused until all attempts are
    // exhausted
    //
    // Testing: ECONNREFUSED, connection attempt exhaustion

    NTCI_LOG_CONTEXT();

    BSLS_LOG_WARN("ECONNREFUSED/ETIMEDOUT (x5)");

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 5;

    const double k_RETRY_INTERVAL_SECONDS = 0.1;

    const bsls::TimeInterval k_RETRY_INTERVAL(k_RETRY_INTERVAL_SECONDS);
    const bsls::TimeInterval k_LINGER_INTERVAL(k_RETRY_INTERVAL_SECONDS * 3);

    ntsa::Error error;

    // Create the stream socket.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    ntci::StreamSocketCloseGuard closeGuard(streamSocket);

    // Create the listener socket.

    bsl::shared_ptr<ntsi::ListenerSocket> listenerSocket =
        ntsf::System::createListenerSocket(allocator);

    error = listenerSocket->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTCCFG_TEST_OK(error);

    // Bind the listener socket.

    error =
        listenerSocket->bind(ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                             true);
    NTCCFG_TEST_OK(error);

    ntsa::Endpoint endpoint;
    error = listenerSocket->sourceEndpoint(&endpoint);
    NTCCFG_TEST_OK(error);

    // Connect the stream socket to the listener socket.

    bsl::size_t numErrors = 0;
    NTCCFG_TEST_GT(k_MAX_CONNECTION_ATTEMPTS, 0);

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);
    connectOptions.setRetryInterval(k_RETRY_INTERVAL);

    bsl::string connectName =
        "localhost:" + bsl::to_string(endpoint.ip().port());

    ntci::ConnectFuture connectFuture(allocator);
    error = streamSocket->connect(connectName, connectOptions, connectFuture);
    NTCCFG_TEST_OK(error);

    // Since the listening socket is bound, but not listening, to the endpoint
    // to which the stream socket is attempting to connect, ensure there are
    // exactly k_MAX_CONNECTION_ATTEMPTS number of failures, each indicating
    // ECONNREFUSED.

    for (bsl::size_t i = 0; i < k_MAX_CONNECTION_ATTEMPTS; ++i) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        if (connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD))
        {
            ++numErrors;
            NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
            bsl::size_t attemptsRemaining =
                k_MAX_CONNECTION_ATTEMPTS - numErrors;

            NTCCFG_TEST_EQ(connectResult.event().context().name(),
                           connectName);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           attemptsRemaining);
        }
        else {
            NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                  << connectResult.event()
                                  << NTCCFG_TEST_LOG_END;
            NTCCFG_TEST_TRUE(false);
        }
    }

    // Since the listener socket never begins listening, and all retry
    // attempts have been exhausted, the stream socket should never reconnect.

    {
        NTCCFG_TEST_LOG_INFO << "Waiting for 3 seconds to ensure no retries "
                                "are attempted"
                             << NTCCFG_TEST_LOG_END;

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(
            &connectResult,
            streamSocket->currentTime() + k_LINGER_INTERVAL);
        NTCCFG_TEST_TRUE(error);
    }
}

void concernConnectName3(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                         bslma::Allocator*                       allocator)
{
    // Concern: Connect to name periodically times out until all attempts
    // are exhausted
    //
    // Testing: ETIMEDOUT, connection attempt exhaustion

    NTCI_LOG_CONTEXT();

    BSLS_LOG_WARN("ETIMEDOUT (x5)");

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 5;

    const double k_RETRY_INTERVAL_SECONDS = 0.1;

    const bsls::TimeInterval k_RETRY_INTERVAL(k_RETRY_INTERVAL_SECONDS);
    const bsls::TimeInterval k_LINGER_INTERVAL(k_RETRY_INTERVAL_SECONDS * 3);

    ntsa::Error error;

    // Create a test resolver.

    bsl::shared_ptr<test::Resolver> resolver;
    resolver.createInplace(allocator, allocator);

    resolver->setDelay(bsls::TimeInterval(2.0));

    error = resolver->start();
    NTCCFG_TEST_OK(error);

    // Create the stream socket.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    streamSocket->registerResolver(resolver);

    ntci::StreamSocketCloseGuard closeGuard(streamSocket);

    // Connect the stream socket to the listener socket.

    bsl::size_t numErrors = 0;
    NTCCFG_TEST_GT(k_MAX_CONNECTION_ATTEMPTS, 0);

    bsl::string connectName = "foo.bar.baz:1024";

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);
    connectOptions.setRetryInterval(k_RETRY_INTERVAL);

    ntci::ConnectFuture connectFuture(allocator);
    error = streamSocket->connect(connectName, connectOptions, connectFuture);
    NTCCFG_TEST_OK(error);

    // Since we assume "foo.bar.baz:1024", the name to which the stream socket
    // is attempting to connect, never resolves, ensure there are exactly
    // k_MAX_CONNECTION_ATTEMPTS number of failures, each indicating ETIMEDOUT.

    for (bsl::size_t i = 0; i < k_MAX_CONNECTION_ATTEMPTS; ++i) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        NTCCFG_TEST_EQ(connectResult.event().context().error(),
                       ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT));

        ++numErrors;
        NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
        bsl::size_t attemptsRemaining = k_MAX_CONNECTION_ATTEMPTS - numErrors;

        NTCCFG_TEST_EQ(connectResult.event().context().name(), connectName);
        NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                       attemptsRemaining);
    }

    // Since the listener socket never begins listening, and all retry
    // attempts have been exhausted, the stream socket should never reconnect.

    {
        NTCCFG_TEST_LOG_INFO << "Waiting for 3 seconds to ensure no retries "
                                "are attempted"
                             << NTCCFG_TEST_LOG_END;

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(
            &connectResult,
            streamSocket->currentTime() + k_LINGER_INTERVAL);
        NTCCFG_TEST_TRUE(error);
    }

    // Stop the test resolver.

    resolver->shutdown();
    resolver->linger();
}

void concernConnectName4(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                         bslma::Allocator*                       allocator)
{
    // Concern: Connections to names may be cancelled while waiting to
    // begin connecting.
    //
    // Testing: ECONNREFUSED, ECANCELED.

    NTCI_LOG_CONTEXT();

    BSLS_LOG_WARN("ECONNREFUSED (x4), ECANCELED");

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 100;

    const double k_RETRY_INTERVAL_SECONDS = 0.1;

    const bsls::TimeInterval k_RETRY_INTERVAL(k_RETRY_INTERVAL_SECONDS);
    const bsls::TimeInterval k_LINGER_INTERVAL(k_RETRY_INTERVAL_SECONDS * 3);

    ntsa::Error error;

    // Create the stream socket.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    ntci::StreamSocketCloseGuard closeGuard(streamSocket);

    // Connect the stream socket to the listener socket.

    bsl::size_t numErrors = 0;
    NTCCFG_TEST_GT(k_MAX_CONNECTION_ATTEMPTS, 0);

    const bsl::string connectName = "localhost:1024";
    ntsa::Endpoint    endpoint("127.0.0.1:1024");

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);
    connectOptions.setRetryInterval(k_RETRY_INTERVAL);

    ntci::ConnectFuture connectFuture(allocator);
    error = streamSocket->connect(connectName, connectOptions, connectFuture);
    NTCCFG_TEST_OK(error);

    // Ideally, we'd like to bind to an ephemeral port on the loopback
    // scheduler but not begin listening, then have connection attempts
    // made to that address fail with ECONNREFUSED. However, only
    // Linux has this behavior. All other platforms just time out the
    // connection attempt.

    for (bsl::size_t i = 0; i < 4; ++i) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        if (connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD))
        {
            ++numErrors;
            NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
            bsl::size_t attemptsRemaining =
                k_MAX_CONNECTION_ATTEMPTS - numErrors;

            NTCCFG_TEST_EQ(connectResult.event().context().name(),
                           connectName);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           attemptsRemaining);
        }
        else {
            NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                  << connectResult.event()
                                  << NTCCFG_TEST_LOG_END;
            NTCCFG_TEST_TRUE(false);
        }
    }

    // Close the connection.

    streamSocket->close();

    // The connection should eventually be cancelled.

    while (true) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        if (connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD))
        {
            ++numErrors;
            NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
            bsl::size_t attemptsRemaining =
                k_MAX_CONNECTION_ATTEMPTS - numErrors;

            NTCCFG_TEST_EQ(connectResult.event().context().name(),
                           connectName);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           attemptsRemaining);
        }
        else if (connectResult.event().context().error() ==
                 ntsa::Error(ntsa::Error::e_CANCELLED))
        {
            NTCCFG_TEST_EQ(connectResult.event().context().name(),
                           connectName);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           0);

            break;
        }
        else {
            NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                  << connectResult.event()
                                  << NTCCFG_TEST_LOG_END;
            NTCCFG_TEST_TRUE(false);
        }
    }

    // Since the stream socket has been closed during a connection attempt,
    // the stream socket should never reconnect.

    {
        NTCCFG_TEST_LOG_INFO << "Waiting for 3 seconds to ensure no retries "
                                "are attempted"
                             << NTCCFG_TEST_LOG_END;

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(
            &connectResult,
            streamSocket->currentTime() + k_LINGER_INTERVAL);
        NTCCFG_TEST_TRUE(error);
    }
}

void concernConnectName5(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                         bslma::Allocator*                       allocator)
{
    // Concern: Connections to names may be cancelled while connections have
    // been initiated but not yet complete.
    //
    // Testing: ETIMEDOUT, ECANCELED

    NTCI_LOG_CONTEXT();

    BSLS_LOG_WARN("ETIMEDOUT (x4), ECANCELED");

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 100;

    const double k_RETRY_INTERVAL_SECONDS = 0.1;

    const bsls::TimeInterval k_RETRY_INTERVAL(k_RETRY_INTERVAL_SECONDS);
    const bsls::TimeInterval k_LINGER_INTERVAL(k_RETRY_INTERVAL_SECONDS * 3);

    ntsa::Error error;

    // Create a test resolver.

    bsl::shared_ptr<test::Resolver> resolver;
    resolver.createInplace(allocator, allocator);

    resolver->setDelay(bsls::TimeInterval(2.0));

    error = resolver->start();
    NTCCFG_TEST_OK(error);

    // Create the stream socket.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    streamSocket->registerResolver(resolver);

    ntci::StreamSocketCloseGuard closeGuard(streamSocket);

    // Connect the stream socket to the listener socket.

    bsl::string connectName = "foo.bar.baz:1024";

    bsl::size_t numErrors = 0;
    NTCCFG_TEST_GT(k_MAX_CONNECTION_ATTEMPTS, 0);

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);
    connectOptions.setRetryInterval(k_RETRY_INTERVAL);

    ntci::ConnectFuture connectFuture(allocator);
    error = streamSocket->connect(connectName, connectOptions, connectFuture);
    NTCCFG_TEST_OK(error);

    // Since we assume that foo.bar.baz:1024, the endpoint to which the stream
    // socket is attempting to connect, never resolves, ensure there are
    // exactly k_MAX_CONNECTION_ATTEMPTS number of failures, each indicating
    // ETIMEDOUT.

    for (bsl::size_t i = 0; i < 4; ++i) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        if (connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD))
        {
            ++numErrors;
            NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
            bsl::size_t attemptsRemaining =
                k_MAX_CONNECTION_ATTEMPTS - numErrors;

            NTCCFG_TEST_EQ(connectResult.event().context().name(),
                           connectName);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           attemptsRemaining);
        }
        else {
            NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                  << connectResult.event()
                                  << NTCCFG_TEST_LOG_END;
            NTCCFG_TEST_TRUE(false);
        }
    }

    // Close the connection.

    streamSocket->close();

    // The connection should eventually be cancelled.

    while (true) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        if (connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD))
        {
            ++numErrors;
            NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
            bsl::size_t attemptsRemaining =
                k_MAX_CONNECTION_ATTEMPTS - numErrors;

            NTCCFG_TEST_EQ(connectResult.event().context().name(),
                           connectName);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           attemptsRemaining);
        }
        else if (connectResult.event().context().error() ==
                 ntsa::Error(ntsa::Error::e_CANCELLED))
        {
            NTCCFG_TEST_EQ(connectResult.event().context().name(),
                           connectName);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           0);

            break;
        }
        else {
            NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                  << connectResult.event()
                                  << NTCCFG_TEST_LOG_END;
            NTCCFG_TEST_TRUE(false);
        }
    }

    // Since the stream socket has been closed during a connection attempt,
    // the stream socket should never reconnect.

    {
        NTCCFG_TEST_LOG_INFO << "Waiting for 3 seconds to ensure no retries "
                                "are attempted"
                             << NTCCFG_TEST_LOG_END;

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(
            &connectResult,
            streamSocket->currentTime() + k_LINGER_INTERVAL);
        NTCCFG_TEST_TRUE(error);
    }

    // Stop the test resolver.

    resolver->shutdown();
    resolver->linger();
}

void concernConnectName6(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                         bslma::Allocator*                       allocator)
{
    // Concern: Connections to names may be reach their deadline while waiting
    // to begin connecting.
    //
    // Testing: ECONNREFUSED, ETIMEDOUT/ECANCELED.

    NTCI_LOG_CONTEXT();

    BSLS_LOG_WARN("ECONNREFUSED (x4), ETIMEDOUT/ECANCELED");

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 100;

    const double k_RETRY_INTERVAL_SECONDS = 0.1;

    const bsls::TimeInterval k_RETRY_INTERVAL(k_RETRY_INTERVAL_SECONDS);
    const bsls::TimeInterval k_LINGER_INTERVAL(k_RETRY_INTERVAL_SECONDS * 3);
    const bsls::TimeInterval k_DEADLINE_INTERVAL(k_RETRY_INTERVAL_SECONDS * 5);

    ntsa::Error error;

    // Create the stream socket.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    ntci::StreamSocketCloseGuard closeGuard(streamSocket);

    // Connect the stream socket to the listener socket.

    bsl::size_t numErrors = 0;
    NTCCFG_TEST_GT(k_MAX_CONNECTION_ATTEMPTS, 0);

    const bsl::string connectName = "localhost:1024";
    ntsa::Endpoint    endpoint("127.0.0.1:1024");

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);
    connectOptions.setRetryInterval(k_RETRY_INTERVAL);
    connectOptions.setDeadline(streamSocket->currentTime() +
                               k_DEADLINE_INTERVAL);

    ntci::ConnectFuture connectFuture(allocator);
    error = streamSocket->connect(connectName, connectOptions, connectFuture);
    NTCCFG_TEST_OK(error);

    // Ideally, we'd like to bind to an ephemeral port on the loopback
    // scheduler but not begin listening, then have connection attempts
    // made to that address fail with ECONNREFUSED. However, only
    // Linux has this behavior. All other platforms just time out the
    // connection attempt.

    while (true) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        if (connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD))
        {
            ++numErrors;
            NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
            bsl::size_t attemptsRemaining =
                k_MAX_CONNECTION_ATTEMPTS - numErrors;

            NTCCFG_TEST_EQ(connectResult.event().context().name(),
                           connectName);

            if (connectResult.event().context().attemptsRemaining() == 0) {
                break;
            }
            else {
                NTCCFG_TEST_EQ(
                    connectResult.event().context().attemptsRemaining(),
                    attemptsRemaining);
            }
        }
        else if (connectResult.event().context().error() ==
                 ntsa::Error(ntsa::Error::e_CANCELLED))
        {
            NTCCFG_TEST_EQ(connectResult.event().context().name(),
                           connectName);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           0);

            break;
        }
        else {
            NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                  << connectResult.event()
                                  << NTCCFG_TEST_LOG_END;
            NTCCFG_TEST_TRUE(false);
        }
    }

    // Since the stream socket has been closed during a connection attempt,
    // the stream socket should never reconnect.

    {
        NTCCFG_TEST_LOG_INFO << "Waiting for 3 seconds to ensure no retries "
                                "are attempted"
                             << NTCCFG_TEST_LOG_END;

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(
            &connectResult,
            streamSocket->currentTime() + k_LINGER_INTERVAL);
        NTCCFG_TEST_TRUE(error);
    }
}

void concernConnectName7(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                         bslma::Allocator*                       allocator)
{
    // Concern: Connections to names may reach their deadline while connections
    // have been initiated but not yet complete.
    //
    // Testing: ETIMEDOUT, ETIMEDOUT/ECANCELED

    NTCI_LOG_CONTEXT();

    BSLS_LOG_WARN("ETIMEDOUT (x4), ETIMEDOUT/ECANCELED");

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 100;

    const double k_RETRY_INTERVAL_SECONDS = 0.1;

    const bsls::TimeInterval k_RETRY_INTERVAL(k_RETRY_INTERVAL_SECONDS);
    const bsls::TimeInterval k_LINGER_INTERVAL(k_RETRY_INTERVAL_SECONDS * 3);
    const bsls::TimeInterval k_DEADLINE_INTERVAL(k_RETRY_INTERVAL_SECONDS * 5);

    ntsa::Error error;

    // Create a test resolver.

    bsl::shared_ptr<test::Resolver> resolver;
    resolver.createInplace(allocator, allocator);

    resolver->setDelay(bsls::TimeInterval(2.0));

    error = resolver->start();
    NTCCFG_TEST_OK(error);

    // Create the stream socket.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    streamSocket->registerResolver(resolver);

    ntci::StreamSocketCloseGuard closeGuard(streamSocket);

    // Connect the stream socket to the listener socket.

    bsl::string connectName = "foo.bar.baz:1024";

    bsl::size_t numErrors = 0;
    NTCCFG_TEST_GT(k_MAX_CONNECTION_ATTEMPTS, 0);

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);
    connectOptions.setRetryInterval(k_RETRY_INTERVAL);
    connectOptions.setDeadline(streamSocket->currentTime() +
                               k_DEADLINE_INTERVAL);

    ntci::ConnectFuture connectFuture(allocator);
    error = streamSocket->connect(connectName, connectOptions, connectFuture);
    NTCCFG_TEST_OK(error);

    // Since we assume that foo.bar.baz:1024, the endpoint to which the stream
    // socket is attempting to connect, never resolves, ensure there are
    // exactly k_MAX_CONNECTION_ATTEMPTS number of failures, each indicating
    // ETIMEDOUT.

    while (true) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        if (connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
            connectResult.event().context().error() ==
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD))
        {
            ++numErrors;
            NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
            bsl::size_t attemptsRemaining =
                k_MAX_CONNECTION_ATTEMPTS - numErrors;

            NTCCFG_TEST_EQ(connectResult.event().context().name(),
                           connectName);

            if (connectResult.event().context().attemptsRemaining() == 0) {
                break;
            }
            else {
                NTCCFG_TEST_EQ(
                    connectResult.event().context().attemptsRemaining(),
                    attemptsRemaining);
            }
        }
        else if (connectResult.event().context().error() ==
                 ntsa::Error(ntsa::Error::e_CANCELLED))
        {
            NTCCFG_TEST_EQ(connectResult.event().context().name(),
                           connectName);
            NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                           0);

            break;
        }
        else {
            NTCCFG_TEST_LOG_FATAL << "Unexpected connect event "
                                  << connectResult.event()
                                  << NTCCFG_TEST_LOG_END;
            NTCCFG_TEST_TRUE(false);
        }
    }

    // Since the stream socket has been closed during a connection attempt,
    // the stream socket should never reconnect.

    {
        NTCCFG_TEST_LOG_INFO << "Waiting for 3 seconds to ensure no retries "
                                "are attempted"
                             << NTCCFG_TEST_LOG_END;

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(
            &connectResult,
            streamSocket->currentTime() + k_LINGER_INTERVAL);
        NTCCFG_TEST_TRUE(error);
    }

    // Stop the test resolver.

    resolver->shutdown();
    resolver->linger();
}

void concernConnectName8(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                         bslma::Allocator*                       allocator)
{
    // Concern: Connect to name periodically times out with a nearly
    // instantaneous timeout or the timer races with the resolution of a name
    // to which the connection is refused or detected to have been aborted,
    // until all attempts are exhausted (see also 'concernConnectName3'.)
    //
    // Testing: ETIMEDOUT/ECONNREFUSED/ECONNABORTED,
    //          connection attempt exhaustion

    NTCI_LOG_CONTEXT();

    BSLS_LOG_WARN(
        "ETIMEDOUT/ECONNREFUSED/ECONNABORTED (x100) (instantaneous)");

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 100;

    const double k_RETRY_INTERVAL_SECONDS = 0.1;

    const bsls::TimeInterval k_RETRY_INTERVAL(0, 1);
    const bsls::TimeInterval k_LINGER_INTERVAL(k_RETRY_INTERVAL_SECONDS * 3);

    ntsa::Error error;

    // Create the stream socket.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    ntci::StreamSocketCloseGuard closeGuard(streamSocket);

    // Connect the stream socket to the listener socket.

    bsl::size_t numErrors = 0;
    NTCCFG_TEST_GT(k_MAX_CONNECTION_ATTEMPTS, 0);

    bsl::string connectName = "localhost:1024";

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);
    connectOptions.setRetryInterval(k_RETRY_INTERVAL);

    ntci::ConnectFuture connectFuture(allocator);
    error = streamSocket->connect(connectName, connectOptions, connectFuture);
    NTCCFG_TEST_OK(error);

    // Since we assume "localhost:1024", the name to which the stream socket is
    // attempting to connect, resolves and the connection is refused, or
    // instantaneously times out, ensure there are exactly
    // k_MAX_CONNECTION_ATTEMPTS number of failures, each indicating either
    // ECONNREFUSED or ETIMEDOUT.

    for (bsl::size_t i = 0; i < k_MAX_CONNECTION_ATTEMPTS; ++i) {
        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResult.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        // TODO: The implementation has quirk where sometimes the socket is
        // closed before some internal state transition causes an operation
        // on the socket. This is a technically a bug that needs to be
        // investigated and fixed. For now, log a warning for what we expect,
        // but allow ntsa::Error::e_INVALID in what we assert.

        if (connectResult.event().context().error() !=
                ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) &&
            connectResult.event().context().error() !=
                ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) &&
            connectResult.event().context().error() !=
                ntsa::Error(ntsa::Error::e_CONNECTION_DEAD))
        {
            BSLS_LOG_WARN(
                "Expected CONNECTION_TIMEOUT, CONNECTION_REFUSED, "
                "or CONNECTION_DEAD, but found: %s (%d)",
                connectResult.event().context().error().text().c_str(),
                connectResult.event().context().error().number());
        }

        NTCCFG_TEST_TRUE(connectResult.event().context().error() ==
                             ntsa::Error(ntsa::Error::e_CONNECTION_TIMEOUT) ||
                         connectResult.event().context().error() ==
                             ntsa::Error(ntsa::Error::e_CONNECTION_REFUSED) ||
                         connectResult.event().context().error() ==
                             ntsa::Error(ntsa::Error::e_CONNECTION_DEAD) ||
                         connectResult.event().context().error() ==
                             ntsa::Error(ntsa::Error::e_INVALID));

        ++numErrors;
        NTCCFG_TEST_LE(numErrors, k_MAX_CONNECTION_ATTEMPTS);
        bsl::size_t attemptsRemaining = k_MAX_CONNECTION_ATTEMPTS - numErrors;

        NTCCFG_TEST_EQ(connectResult.event().context().name(), connectName);
        NTCCFG_TEST_EQ(connectResult.event().context().attemptsRemaining(),
                       attemptsRemaining);
    }

    // Since the listener socket never begins listening, and all retry
    // attempts have been exhausted, the stream socket should never reconnect.

    {
        NTCCFG_TEST_LOG_INFO << "Waiting for 3 seconds to ensure no retries "
                                "are attempted"
                             << NTCCFG_TEST_LOG_END;

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(
            &connectResult,
            streamSocket->currentTime() + k_LINGER_INTERVAL);
        NTCCFG_TEST_TRUE(error);
    }
}

void concernConnectLimitActive(bslma::Allocator* allocator)
{
    // Concern: Connection limit reached on active side.
    // Plan:

    ntsa::Error error;

    // Create an scheduler that only allows one connection at a time.

    ntca::SchedulerConfig schedulerConfig;
    schedulerConfig.setThreadName("test");
    schedulerConfig.setMinThreads(2);
    schedulerConfig.setMaxThreads(2);
    schedulerConfig.setMaxConnections(1);

    bsl::shared_ptr<ntci::Scheduler> scheduler =
        ntcf::System::createScheduler(schedulerConfig, allocator);

    error = scheduler->start();
    NTCCFG_TEST_OK(error);

    // Create two stream sockets.

    ntca::StreamSocketOptions streamSocketOptionsA;
    streamSocketOptionsA.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocketA =
        scheduler->createStreamSocket(streamSocketOptionsA, allocator);

    ntca::StreamSocketOptions streamSocketOptionsB;
    streamSocketOptionsB.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocketB =
        scheduler->createStreamSocket(streamSocketOptionsB, allocator);

    // Create the listener socket.

    bsl::shared_ptr<ntsi::ListenerSocket> listenerSocket =
        ntsf::System::createListenerSocket(allocator);

    error = listenerSocket->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTCCFG_TEST_OK(error);

    // Bind the listener socket.

    error =
        listenerSocket->bind(ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0),
                             true);
    NTCCFG_TEST_OK(error);

    ntsa::Endpoint endpoint;
    error = listenerSocket->sourceEndpoint(&endpoint);
    NTCCFG_TEST_OK(error);

    // Begin listening.

    error = listenerSocket->listen(100);
    NTCCFG_TEST_OK(error);

    // Connect stream socket A to the listener socket and ensure the
    // connection completes successfully.

    ntca::ConnectOptions connectOptionsA;
    connectOptionsA.setRetryCount(100);
    connectOptionsA.setRetryInterval(bsls::TimeInterval(1));

    ntci::ConnectFuture connectFutureA(allocator);
    error = streamSocketA->connect(endpoint, connectOptionsA, connectFutureA);
    NTCCFG_TEST_OK(error);

    ntci::ConnectResult connectResultA;
    error = connectFutureA.wait(&connectResultA);
    NTCCFG_TEST_OK(error);

    NTCCFG_TEST_EQ(connectResultA.event().type(),
                   ntca::ConnectEventType::e_COMPLETE);

    // Connect stream socket B to the listener socket, and ensure that
    // the first several connection attempts fail because the connection
    // limit has already been reached.

    ntca::ConnectOptions connectOptionsB;
    connectOptionsB.setRetryCount(100);
    connectOptionsB.setRetryInterval(bsls::TimeInterval(1));

    ntci::ConnectFuture connectFutureB(allocator);
    error = streamSocketB->connect(endpoint, connectOptionsB, connectFutureB);
    NTCCFG_TEST_OK(error);

    for (bsl::size_t i = 0; i < 4; ++i) {
        ntci::ConnectResult connectResultB;
        error = connectFutureB.wait(&connectResultB);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResultB.event() << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_EQ(connectResultB.event().type(),
                       ntca::ConnectEventType::e_ERROR);

        NTCCFG_TEST_EQ(connectResultB.event().context().error(),
                       ntsa::Error(ntsa::Error::e_LIMIT));
    }

    // Close the first stream socket.

    {
        ntci::StreamSocketCloseGuard guard(streamSocketA);
    }

    // Now that the first stream socket is closed, the second stream socket
    // should eventually connect.

    while (true) {
        ntci::ConnectResult connectResultB;
        error = connectFutureB.wait(&connectResultB);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_INFO << "Processing connect event "
                             << connectResultB.event() << NTCCFG_TEST_LOG_END;

        if (connectResultB.event().type() == ntca::ConnectEventType::e_ERROR) {
            NTCCFG_TEST_EQ(connectResultB.event().context().error(),
                           ntsa::Error(ntsa::Error::e_LIMIT));
        }
        else {
            NTCCFG_TEST_EQ(connectResultB.event().type(),
                           ntca::ConnectEventType::e_COMPLETE);
            break;
        }
    }

    // Close the second stream socket.

    {
        ntci::StreamSocketCloseGuard guard(streamSocketB);
    }

    // Stop the scheduler.

    scheduler->shutdown();
    scheduler->linger();
}

void concernConnectLimitPassive(bslma::Allocator* allocator)
{
    // Concern: Connection limit reached on passive side.
    // Plan:

    ntsa::Error error;

    // Create an scheduler that only allows one connection at a time.

    ntca::SchedulerConfig schedulerConfig;
    schedulerConfig.setThreadName("test");
    schedulerConfig.setMinThreads(2);
    schedulerConfig.setMaxThreads(2);
    schedulerConfig.setMaxConnections(1);

    bsl::shared_ptr<ntci::Scheduler> scheduler =
        ntcf::System::createScheduler(schedulerConfig, allocator);

    error = scheduler->start();
    NTCCFG_TEST_OK(error);

    // Create a listener socket and begin listening.

    ntca::ListenerSocketOptions listenerSocketOptions;
    listenerSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
    listenerSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)));

    listenerSocketOptions.setReuseAddress(true);
    listenerSocketOptions.setBacklog(100);
    listenerSocketOptions.setAcceptQueueLowWatermark(0);
    listenerSocketOptions.setAcceptQueueHighWatermark(100);

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        scheduler->createListenerSocket(listenerSocketOptions, allocator);

    error = listenerSocket->open();
    NTCCFG_TEST_OK(error);

    error = listenerSocket->listen();
    NTCCFG_TEST_OK(error);

    error = listenerSocket->relaxFlowControl(ntca::FlowControlType::e_RECEIVE);
    NTCCFG_TEST_OK(error);

    ntsa::Endpoint endpoint = listenerSocket->sourceEndpoint();

    // Create two stream sockets.

    bsl::shared_ptr<ntsi::StreamSocket> streamSocketA =
        ntsf::System::createStreamSocket(allocator);

    error = streamSocketA->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTCCFG_TEST_OK(error);

    bsl::shared_ptr<ntsi::StreamSocket> streamSocketB =
        ntsf::System::createStreamSocket(allocator);

    error = streamSocketB->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTCCFG_TEST_OK(error);

    // Connect the first stream socket to the listener and ensure the
    // connection succeeds.

    error = streamSocketA->connect(endpoint);
    BSLS_LOG_INFO("Stream socket A connect: %s", error.text().c_str());
    NTCCFG_TEST_OK(error);

    // Connect the second stream socket to the listener and ensure either
    // the connection fails (because it was detected to have been
    // immediately closed by the peer before it was accepted by the
    // operating system) or that writes to the socket eventually fail,
    // because the connection limit should force the listener socket to
    // immediately close the connection after accepting it from the
    // operating system.

    error = streamSocketB->connect(endpoint);
    BSLS_LOG_INFO("Stream socket B connect: %s", error.text().c_str());

    if (error) {
        if (error == ntsa::Error::e_CONNECTION_RESET ||
            error == ntsa::Error::e_CONNECTION_DEAD ||
            error == ntsa::Error::e_EOF)
        {
            NTCCFG_TEST_TRUE(true);
        }
        else {
            BSLS_LOG_FATAL("Unexpected error: %s", error.text().c_str());
            NTCCFG_TEST_TRUE(false);
        }
    }
    else {
        while (true) {
            char buffer = 'X';

            ntsa::Data sendData(ntsa::ConstBuffer(&buffer, sizeof buffer));

            ntsa::SendContext sendContext;
            ntsa::SendOptions sendOptions;

            error = streamSocketB->send(&sendContext, sendData, sendOptions);

            BSLS_LOG_INFO("Stream socket B send: %s", error.text().c_str());

            if (error) {
                if (error == ntsa::Error::e_CONNECTION_RESET ||
                    error == ntsa::Error::e_CONNECTION_DEAD ||
                    error == ntsa::Error::e_EOF)
                {
                    NTCCFG_TEST_TRUE(true);
                    break;
                }
                else {
                    BSLS_LOG_FATAL("Unexpected error: %s",
                                   error.text().c_str());
                    NTCCFG_TEST_TRUE(false);
                }
            }
        }
    }

    // Close both stream sockets.

    streamSocketA->close();
    streamSocketA.reset();

    streamSocketB->close();
    streamSocketB.reset();

    // Close the listener socket.

    {
        ntci::ListenerSocketCloseGuard guard(listenerSocket);
    }

    listenerSocket.reset();

    // Stop the scheduler.

    scheduler->shutdown();
    scheduler->linger();
}

void concernAcceptClosure(bslma::Allocator* allocator)
{
    // Concern: Connections that have been accepted by a listening socket
    //          but not used are automatically closed if the ntci::Scheduler
    //          is instructed to close all sockets.
    // Plan:

    ntsa::Error error;

    // Create an scheduler.

    ntca::SchedulerConfig schedulerConfig;
    schedulerConfig.setThreadName("test");
    schedulerConfig.setMinThreads(1);
    schedulerConfig.setMaxThreads(1);

    bsl::shared_ptr<ntci::Scheduler> scheduler =
        ntcf::System::createScheduler(schedulerConfig, allocator);

    error = scheduler->start();
    NTCCFG_TEST_OK(error);

    // Create a listener socket and begin listening.

    ntca::ListenerSocketOptions listenerSocketOptions;
    listenerSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
    listenerSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)));

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        scheduler->createListenerSocket(listenerSocketOptions, allocator);

    error = listenerSocket->open();
    NTCCFG_TEST_OK(error);

    error = listenerSocket->listen();
    NTCCFG_TEST_OK(error);

    // Create a stream socket.

    ntca::StreamSocketOptions streamSocketOptions;
    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions, allocator);

    // Connect the stream socket to the listening socket.

    ntca::ConnectOptions connectOptions;
    connectOptions.setRetryCount(9999);
    connectOptions.setRetryInterval(bsls::TimeInterval(0.01));
    connectOptions.setDeadline(streamSocket->currentTime() +
                               bsls::TimeInterval(600));

    ntci::ConnectFuture connectFuture;
    error = streamSocket->connect(listenerSocket->sourceEndpoint(),
                                  connectOptions,
                                  connectFuture);
    NTCCFG_TEST_OK(error);

    // Wait for an arrival of the connect future.

    ntci::ConnectResult connectResult;
    error = connectFuture.wait(&connectResult);
    NTCCFG_TEST_OK(error);

    // Ensure the connect future's result indicates the operation was
    // successful.

    NTCCFG_TEST_TRUE(connectResult.event().isComplete());

    // Accept a stream socket to act as the server.

    ntci::AcceptFuture acceptFuture;
    error = listenerSocket->accept(ntca::AcceptOptions(), acceptFuture);
    NTCCFG_TEST_OK(error);

    // Wait for an arrival of the accept future.

    ntci::AcceptResult acceptResult;
    error = acceptFuture.wait(&acceptResult);
    NTCCFG_TEST_OK(error);

    // Ensure the accept future's result indicates the operation was
    // successful.

    NTCCFG_TEST_TRUE(acceptResult.event().isComplete());

    // Close all sockets managed by the scheduler.

    scheduler->closeAll();

    // Stop the scheduler.

    scheduler->shutdown();
    scheduler->linger();
}

void concernDatagramSocket(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                           const test::DatagramSocketParameters&   parameters,
                           bslma::Allocator*                       allocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Datagram socket test starting");

    BSLS_ASSERT_OPT(parameters.d_transport != ntsa::Transport::e_UNDEFINED);

    ntsa::TransportMode::Value transportMode =
        ntsa::Transport::getMode(parameters.d_transport);

    BSLS_ASSERT_OPT(transportMode == ntsa::TransportMode::e_DATAGRAM);

    bsl::shared_ptr<test::DatagramSocketManager> datagramSocketManager;
    datagramSocketManager.createInplace(allocator,
                                        scheduler,
                                        parameters,
                                        allocator);

    datagramSocketManager->run();
    datagramSocketManager.reset();

    NTCI_LOG_DEBUG("Datagram socket test complete");
}

void concernDatagramSocketBasicReactive(bslma::Allocator* allocator)
{
    // Concern: Breathing test.

    test::DatagramSocketParameters parameters;

    parameters.d_numTimers         = 0;
    parameters.d_numSocketPairs    = 1;
    parameters.d_numMessages       = 1;
    parameters.d_messageSize       = 32;
    parameters.d_useAsyncCallbacks = false;

    test::concern(NTCCFG_BIND(&test::concernDatagramSocket,
                              NTCCFG_BIND_PLACEHOLDER_1,
                              parameters,
                              NTCCFG_BIND_PLACEHOLDER_2),
                  allocator);
}

void concernDatagramSocketBasicProactive(bslma::Allocator* allocator)
{
    // Concern: Breathing test using asynchronous callbacks.

    test::DatagramSocketParameters parameters;

    parameters.d_numTimers         = 0;
    parameters.d_numSocketPairs    = 1;
    parameters.d_numMessages       = 1;
    parameters.d_messageSize       = 32;
    parameters.d_useAsyncCallbacks = true;

    test::concern(NTCCFG_BIND(&test::concernDatagramSocket,
                              NTCCFG_BIND_PLACEHOLDER_1,
                              parameters,
                              NTCCFG_BIND_PLACEHOLDER_2),
                  allocator);
}

void concernDatagramSocketStressReactive(bslma::Allocator* allocator)
{
    // Concern: Stress test.

    // The test currently fails sporadically on Linux on CI build machines
    // with "Assertion failed: !d_chronology_sp->hasAnyDeferred()".
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0
    test::DatagramSocketParameters parameters;

    parameters.d_numTimers         = 100;
    parameters.d_numSocketPairs    = 100;
    parameters.d_numMessages       = 32;
    parameters.d_messageSize       = 1024;
    parameters.d_useAsyncCallbacks = false;

    parameters.d_receiveBufferSize = 500 * 1000;

    test::concern(NTCCFG_BIND(&test::concernDatagramSocket,
                              NTCCFG_BIND_PLACEHOLDER_1,
                              parameters,
                              NTCCFG_BIND_PLACEHOLDER_2),
                  allocator);
#endif
}

void concernDatagramSocketStressProactive(bslma::Allocator* allocator)
{
    // Concern: Stress test using asynchronous callbacks.

    // The test currently fails sporadically on Linux on CI build machines
    // with "Assertion failed: !d_chronology_sp->hasAnyDeferred()".
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0
    test::DatagramSocketParameters parameters;

    parameters.d_numTimers         = 100;
    parameters.d_numSocketPairs    = 100;
    parameters.d_numMessages       = 32;
    parameters.d_messageSize       = 1024;
    parameters.d_useAsyncCallbacks = true;

    parameters.d_receiveBufferSize = 500 * 1000;

    test::concern(NTCCFG_BIND(&test::concernDatagramSocket,
                              NTCCFG_BIND_PLACEHOLDER_1,
                              parameters,
                              NTCCFG_BIND_PLACEHOLDER_2),
                  allocator);
#endif
}

void concernDatagramSocketReceiveDeadline(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: Receive deadlines.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Datagram socket receive deadline test starting");

    const int k_RECEIVE_TIMEOUT_IN_MILLISECONDS = 200;

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_UDP_IPV4_DATAGRAM;

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    ntca::DatagramSocketOptions options;
    options.setTransport(transport);
    options.setSourceEndpoint(test::EndpointUtil::any(transport));

    bsl::shared_ptr<ntci::Resolver> resolver;

    bsl::shared_ptr<ntci::DatagramSocket> datagramSocket =
        scheduler->createDatagramSocket(options, allocator);

    error = datagramSocket->open();
    NTCCFG_TEST_FALSE(error);

    bsls::TimeInterval receiveTimeout;
    receiveTimeout.setTotalMilliseconds(k_RECEIVE_TIMEOUT_IN_MILLISECONDS);

    bsls::TimeInterval receiveDeadline =
        datagramSocket->currentTime() + receiveTimeout;

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setDeadline(receiveDeadline);

    ntci::ReceiveCallback receiveCallback =
        datagramSocket->createReceiveCallback(
            NTCCFG_BIND(&test::DatagramSocketUtil::processReceiveTimeout,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        ntsa::Error::e_WOULD_BLOCK,
                        &semaphore),
            allocator);

    error = datagramSocket->receive(receiveOptions, receiveCallback);
    NTCCFG_TEST_OK(error);

    semaphore.wait();

    {
        ntci::DatagramSocketCloseGuard datagramSocketCloseGuard(
            datagramSocket);
    }

    NTCI_LOG_DEBUG("Datagram socket receive deadline test complete");
}

void concernDatagramSocketReceiveDeadlineClose(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: validate that receive deadline timer is automatically closed
    // when the socket is closed and then destroyed

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Datagram socket receive deadline test starting");

    const int k_RECEIVE_TIMEOUT_IN_HOURS = 1;

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_UDP_IPV4_DATAGRAM;

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    ntca::DatagramSocketOptions options;
    options.setTransport(transport);
    options.setSourceEndpoint(test::EndpointUtil::any(transport));

    bsl::shared_ptr<ntci::Resolver> resolver;

    bsl::shared_ptr<ntci::DatagramSocket> datagramSocket =
        scheduler->createDatagramSocket(options, allocator);

    error = datagramSocket->open();
    NTCCFG_TEST_FALSE(error);

    bsls::TimeInterval receiveTimeout;
    receiveTimeout.setTotalHours(k_RECEIVE_TIMEOUT_IN_HOURS);

    bsls::TimeInterval receiveDeadline =
        datagramSocket->currentTime() + receiveTimeout;

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setDeadline(receiveDeadline);

    ntci::ReceiveCallback receiveCallback =
        datagramSocket->createReceiveCallback(
            NTCCFG_BIND(&test::DatagramSocketUtil::processReceiveTimeout,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        ntsa::Error::e_EOF,
                        &semaphore),
            allocator);

    error = datagramSocket->receive(receiveOptions, receiveCallback);
    NTCCFG_TEST_OK(error);

    {
        ntci::DatagramSocketCloseGuard datagramSocketCloseGuard(
            datagramSocket);
    }

    semaphore.wait();

    NTCI_LOG_DEBUG("Datagram socket receive deadline test complete");
}

void concernDatagramSocketReceiveCancellation(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: Receive cancellation.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Datagram socket receive cancellation test starting");

    const int k_RECEIVE_TIMEOUT_IN_MILLISECONDS = 200;

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_UDP_IPV4_DATAGRAM;

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    ntca::DatagramSocketOptions options;
    options.setTransport(transport);
    options.setSourceEndpoint(test::EndpointUtil::any(transport));

    bsl::shared_ptr<ntci::Resolver> resolver;

    bsl::shared_ptr<ntci::DatagramSocket> datagramSocket =
        scheduler->createDatagramSocket(options, allocator);

    error = datagramSocket->open();
    NTCCFG_TEST_FALSE(error);

    bsls::TimeInterval receiveTimeout;
    receiveTimeout.setTotalMilliseconds(k_RECEIVE_TIMEOUT_IN_MILLISECONDS);

    bsls::TimeInterval receiveDeadline =
        datagramSocket->currentTime() + receiveTimeout;

    ntca::ReceiveToken receiveToken;
    receiveToken.setValue(1);

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setToken(receiveToken);

    ntci::ReceiveCallback receiveCallback =
        datagramSocket->createReceiveCallback(
            NTCCFG_BIND(&test::DatagramSocketUtil::processReceiveCancelled,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        &semaphore),
            allocator);

    error = datagramSocket->receive(receiveOptions, receiveCallback);
    NTCCFG_TEST_OK(error);

    ntca::TimerOptions timerOptions;

    timerOptions.setOneShot(true);
    timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    ntci::TimerCallback timerCallback = datagramSocket->createTimerCallback(
        NTCCFG_BIND(&test::DatagramSocketUtil::cancelReceive,
                    datagramSocket,
                    receiveToken),
        allocator);

    bsl::shared_ptr<ntci::Timer> timer =
        datagramSocket->createTimer(timerOptions, timerCallback, allocator);

    error = timer->schedule(receiveDeadline);
    NTCCFG_TEST_FALSE(error);

    semaphore.wait();

    {
        ntci::DatagramSocketCloseGuard datagramSocketCloseGuard(
            datagramSocket);
    }

    NTCI_LOG_DEBUG("Datagram socket receive cancellation test complete");
}

void concernListenerSocketAcceptDeadline(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: Accept deadlines.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Listener socket accept deadline test starting");

    const int k_ACCEPT_TIMEOUT_IN_MILLISECONDS = 200;

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_TCP_IPV4_STREAM;

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    ntca::ListenerSocketOptions options;
    options.setTransport(transport);
    options.setSourceEndpoint(test::EndpointUtil::any(transport));

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        scheduler->createListenerSocket(options, allocator);

    error = listenerSocket->open();
    NTCCFG_TEST_FALSE(error);

    error = listenerSocket->listen();
    NTCCFG_TEST_FALSE(error);

    bsls::TimeInterval acceptTimeout;
    acceptTimeout.setTotalMilliseconds(k_ACCEPT_TIMEOUT_IN_MILLISECONDS);

    bsls::TimeInterval acceptDeadline =
        listenerSocket->currentTime() + acceptTimeout;

    ntca::AcceptOptions acceptOptions;
    acceptOptions.setDeadline(acceptDeadline);

    ntci::AcceptCallback acceptCallback = listenerSocket->createAcceptCallback(
        NTCCFG_BIND(&test::ListenerSocketUtil::processAcceptTimeout,
                    listenerSocket,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    &semaphore),
        allocator);

    error = listenerSocket->accept(acceptOptions, acceptCallback);
    NTCCFG_TEST_OK(error);

    semaphore.wait();

    {
        ntci::ListenerSocketCloseGuard listenerSocketCloseGuard(
            listenerSocket);
    }

    NTCI_LOG_DEBUG("Listener socket accept deadline test complete");
}

void concernListenerSocketAcceptCancellation(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: Accept cancellation.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Listener socket accept cancellation test starting");

    const int k_ACCEPT_TIMEOUT_IN_MILLISECONDS = 200;

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_TCP_IPV4_STREAM;

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    ntca::ListenerSocketOptions options;
    options.setTransport(transport);
    options.setSourceEndpoint(test::EndpointUtil::any(transport));

    bsl::shared_ptr<ntci::Resolver> resolver;

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        scheduler->createListenerSocket(options, allocator);

    error = listenerSocket->open();
    NTCCFG_TEST_FALSE(error);

    error = listenerSocket->listen();
    NTCCFG_TEST_FALSE(error);

    bsls::TimeInterval acceptTimeout;
    acceptTimeout.setTotalMilliseconds(k_ACCEPT_TIMEOUT_IN_MILLISECONDS);

    bsls::TimeInterval acceptDeadline =
        listenerSocket->currentTime() + acceptTimeout;

    ntca::AcceptToken acceptToken;
    acceptToken.setValue(1);

    ntca::AcceptOptions acceptOptions;
    acceptOptions.setToken(acceptToken);

    ntci::AcceptCallback acceptCallback = listenerSocket->createAcceptCallback(
        NTCCFG_BIND(&test::ListenerSocketUtil::processAcceptCancelled,
                    listenerSocket,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    ntsa::Error::e_CANCELLED,
                    &semaphore),
        allocator);

    error = listenerSocket->accept(acceptOptions, acceptCallback);
    NTCCFG_TEST_OK(error);

    ntca::TimerOptions timerOptions;

    timerOptions.setOneShot(true);
    timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    ntci::TimerCallback timerCallback = listenerSocket->createTimerCallback(
        NTCCFG_BIND(&test::ListenerSocketUtil::cancelAccept,
                    listenerSocket,
                    acceptToken),
        allocator);

    bsl::shared_ptr<ntci::Timer> timer =
        listenerSocket->createTimer(timerOptions, timerCallback, allocator);

    error = timer->schedule(acceptDeadline);
    NTCCFG_TEST_FALSE(error);

    semaphore.wait();

    {
        ntci::ListenerSocketCloseGuard listenerSocketCloseGuard(
            listenerSocket);
    }

    NTCI_LOG_DEBUG("Listener socket accept cancellation test complete");
}

void concernStreamSocket(const bsl::shared_ptr<ntci::Scheduler>& scheduler,
                         const test::StreamSocketParameters&     parameters,
                         bslma::Allocator*                       allocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket test starting");

    BSLS_ASSERT_OPT(parameters.d_transport != ntsa::Transport::e_UNDEFINED);

    ntsa::TransportMode::Value transportMode =
        ntsa::Transport::getMode(parameters.d_transport);

    BSLS_ASSERT_OPT(transportMode == ntsa::TransportMode::e_STREAM);

    bsl::shared_ptr<test::StreamSocketManager> streamSocketManager;
    streamSocketManager.createInplace(allocator,
                                      scheduler,
                                      parameters,
                                      allocator);

    streamSocketManager->run();
    streamSocketManager.reset();

    NTCI_LOG_DEBUG("Stream socket test complete");
}

void concernStreamSocketBasicReactive(bslma::Allocator* allocator)
{
    // Concern: Breathing test.

    test::StreamSocketParameters parameters;

    parameters.d_numTimers                 = 0;
    parameters.d_numListeners              = 1;
    parameters.d_numConnectionsPerListener = 1;
    parameters.d_numMessages               = 1;
    parameters.d_messageSize               = 32;
    parameters.d_useAsyncCallbacks         = false;

    test::concern(NTCCFG_BIND(&test::concernStreamSocket,
                              NTCCFG_BIND_PLACEHOLDER_1,
                              parameters,
                              NTCCFG_BIND_PLACEHOLDER_2),
                  allocator);
}

void concernStreamSocketBasicProactive(bslma::Allocator* allocator)
{
    // Concern: Breathing test using asynchronous callbacks.

    test::StreamSocketParameters parameters;

    parameters.d_numTimers                 = 0;
    parameters.d_numListeners              = 1;
    parameters.d_numConnectionsPerListener = 1;
    parameters.d_numMessages               = 1;
    parameters.d_messageSize               = 32;
    parameters.d_useAsyncCallbacks         = true;

    test::concern(NTCCFG_BIND(&test::concernStreamSocket,
                              NTCCFG_BIND_PLACEHOLDER_1,
                              parameters,
                              NTCCFG_BIND_PLACEHOLDER_2),
                  allocator);
}

void concernStreamSocketMinimalReadQueueHighWatermarkReactive(
    bslma::Allocator* allocator)
{
    // Concern: Minimal read queue high watermark.

    test::StreamSocketParameters parameters;

    parameters.d_numTimers                 = 0;
    parameters.d_numListeners              = 1;
    parameters.d_numConnectionsPerListener = 1;
    parameters.d_numMessages               = 100;
    parameters.d_messageSize               = 1024 * 32;
    parameters.d_useAsyncCallbacks         = false;
    parameters.d_readQueueHighWatermark    = 1;

    test::concern(NTCCFG_BIND(&test::concernStreamSocket,
                              NTCCFG_BIND_PLACEHOLDER_1,
                              parameters,
                              NTCCFG_BIND_PLACEHOLDER_2),
                  allocator);
}

void concernStreamSocketMinimalReadQueueHighWatermarkProactive(
    bslma::Allocator* allocator)
{
    // Concern: Minimal read queue high watermark using asynchronous callbacks.

    test::StreamSocketParameters parameters;

    parameters.d_numTimers                 = 0;
    parameters.d_numListeners              = 1;
    parameters.d_numConnectionsPerListener = 1;
    parameters.d_numMessages               = 100;
    parameters.d_messageSize               = 1024 * 32;
    parameters.d_useAsyncCallbacks         = true;
    parameters.d_readQueueHighWatermark    = 1;

    test::concern(NTCCFG_BIND(&test::concernStreamSocket,
                              NTCCFG_BIND_PLACEHOLDER_1,
                              parameters,
                              NTCCFG_BIND_PLACEHOLDER_2),
                  allocator);
}

void concernStreamSocketMinimalWriteQueueHighWatermarkReactive(
    bslma::Allocator* allocator)
{
    // Concern: Minimal write queue high watermark.

    test::StreamSocketParameters parameters;

    parameters.d_numTimers                 = 0;
    parameters.d_numListeners              = 1;
    parameters.d_numConnectionsPerListener = 1;
    parameters.d_numMessages               = 100;
    parameters.d_messageSize               = 1024 * 32;
    parameters.d_useAsyncCallbacks         = false;
    parameters.d_writeQueueHighWatermark   = 1;
    parameters.d_sendBufferSize            = 32 * 1024;

    test::concern(NTCCFG_BIND(&test::concernStreamSocket,
                              NTCCFG_BIND_PLACEHOLDER_1,
                              parameters,
                              NTCCFG_BIND_PLACEHOLDER_2),
                  allocator);
}

void concernStreamSocketMinimalWriteQueueHighWatermarkProactive(
    bslma::Allocator* allocator)
{
    // Concern: Minimal write queue high watermark using asynchronous
    // callbacks.

    test::StreamSocketParameters parameters;

    parameters.d_numTimers                 = 0;
    parameters.d_numListeners              = 1;
    parameters.d_numConnectionsPerListener = 1;
    parameters.d_numMessages               = 100;
    parameters.d_messageSize               = 1024 * 32;
    parameters.d_useAsyncCallbacks         = true;
    parameters.d_writeQueueHighWatermark   = 1;
    parameters.d_sendBufferSize            = 32 * 1024;

    test::concern(NTCCFG_BIND(&test::concernStreamSocket,
                              NTCCFG_BIND_PLACEHOLDER_1,
                              parameters,
                              NTCCFG_BIND_PLACEHOLDER_2),
                  allocator);
}

void concernStreamSocketRateLimitReceiveBufferReactive(
    bslma::Allocator* allocator)
{
    // Concern: Rate limit copying from the receive buffer.

#if !defined(BSLS_PLATFORM_OS_AIX)
    test::StreamSocketParameters parameters;

    parameters.d_numTimers                 = 0;
    parameters.d_numListeners              = 1;
    parameters.d_numConnectionsPerListener = 1;
    parameters.d_numMessages               = 1;
    parameters.d_messageSize               = 32 * 1024 * 4;
    parameters.d_useAsyncCallbacks         = false;
    parameters.d_readRate                  = 32 * 1024;
    parameters.d_receiveBufferSize         = 32 * 1024;

    test::concern(NTCCFG_BIND(&test::concernStreamSocket,
                              NTCCFG_BIND_PLACEHOLDER_1,
                              parameters,
                              NTCCFG_BIND_PLACEHOLDER_2),
                  allocator);
#endif
}

void concernStreamSocketRateLimitReceiveBufferProactive(
    bslma::Allocator* allocator)
{
    // Concern: Rate limit copying from the receive buffer using
    // asynchronous callbacks.

#if !defined(BSLS_PLATFORM_OS_AIX)
    test::StreamSocketParameters parameters;

    parameters.d_numTimers                 = 0;
    parameters.d_numListeners              = 1;
    parameters.d_numConnectionsPerListener = 1;
    parameters.d_numMessages               = 1;
    parameters.d_messageSize               = 32 * 1024 * 4;
    parameters.d_useAsyncCallbacks         = true;
    parameters.d_readRate                  = 32 * 1024;
    parameters.d_receiveBufferSize         = 32 * 1024;

    test::concern(NTCCFG_BIND(&test::concernStreamSocket,
                              NTCCFG_BIND_PLACEHOLDER_1,
                              parameters,
                              NTCCFG_BIND_PLACEHOLDER_2),
                  allocator);
#endif
}

void concernStreamSocketRateLimitSendBufferReactive(
    bslma::Allocator* allocator)
{
    // Concern: Rate limit copying to the send buffer.

    test::StreamSocketParameters parameters;

    parameters.d_numTimers                 = 0;
    parameters.d_numListeners              = 1;
    parameters.d_numConnectionsPerListener = 1;
    parameters.d_numMessages               = 1;
    parameters.d_messageSize               = 32 * 1024 * 4;
    parameters.d_useAsyncCallbacks         = false;
    parameters.d_writeRate                 = 32 * 1024;
    parameters.d_sendBufferSize            = 32 * 1024;

    test::concern(NTCCFG_BIND(&test::concernStreamSocket,
                              NTCCFG_BIND_PLACEHOLDER_1,
                              parameters,
                              NTCCFG_BIND_PLACEHOLDER_2),
                  allocator);
}

void concernStreamSocketRateLimitSendBufferProactive(
    bslma::Allocator* allocator)
{
    // Concern: Rate limit copying to the send buffer using asynchronous
    // callbacks.

    test::StreamSocketParameters parameters;

    parameters.d_numTimers                 = 0;
    parameters.d_numListeners              = 1;
    parameters.d_numConnectionsPerListener = 1;
    parameters.d_numMessages               = 1;
    parameters.d_messageSize               = 32 * 1024 * 4;
    parameters.d_useAsyncCallbacks         = true;
    parameters.d_writeRate                 = 32 * 1024;
    parameters.d_sendBufferSize            = 32 * 1024;

    test::concern(NTCCFG_BIND(&test::concernStreamSocket,
                              NTCCFG_BIND_PLACEHOLDER_1,
                              parameters,
                              NTCCFG_BIND_PLACEHOLDER_2),
                  allocator);
}

void concernStreamSocketStressReactive(bslma::Allocator* allocator)
{
    // Concern: Stress test.

    test::StreamSocketParameters parameters;

    parameters.d_numTimers                 = 0;
    parameters.d_numListeners              = 10;
    parameters.d_numConnectionsPerListener = 10;
    parameters.d_numMessages               = 100;
    parameters.d_messageSize               = 32;
    parameters.d_useAsyncCallbacks         = false;

    test::concern(NTCCFG_BIND(&test::concernStreamSocket,
                              NTCCFG_BIND_PLACEHOLDER_1,
                              parameters,
                              NTCCFG_BIND_PLACEHOLDER_2),
                  allocator);
}

void concernStreamSocketStressProactive(bslma::Allocator* allocator)
{
    // Concern: Stress test using asynchronous callbacks.

    test::StreamSocketParameters parameters;

    parameters.d_numTimers                 = 0;
    parameters.d_numListeners              = 10;
    parameters.d_numConnectionsPerListener = 10;
    parameters.d_numMessages               = 100;
    parameters.d_messageSize               = 32;
    parameters.d_useAsyncCallbacks         = true;

    test::concern(NTCCFG_BIND(&test::concernStreamSocket,
                              NTCCFG_BIND_PLACEHOLDER_1,
                              parameters,
                              NTCCFG_BIND_PLACEHOLDER_2),
                  allocator);
}

void concernStreamSocketReceiveDeadline(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: Receive deadlines.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket receive deadline test starting");

    const int k_RECEIVE_TIMEOUT_IN_MILLISECONDS = 200;

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_TCP_IPV4_STREAM;

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntci::StreamSocket> serverStreamSocket;
    {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntsi::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntsi::StreamSocket> basicServerSocket;

        error = ntsf::System::createStreamSocketPair(&basicClientSocket,
                                                     &basicServerSocket,
                                                     transport);
        NTCCFG_TEST_FALSE(error);

        clientStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
    }

    bsls::TimeInterval receiveTimeout;
    receiveTimeout.setTotalMilliseconds(k_RECEIVE_TIMEOUT_IN_MILLISECONDS);

    bsls::TimeInterval receiveDeadline =
        serverStreamSocket->currentTime() + receiveTimeout;

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setDeadline(receiveDeadline);

    ntci::ReceiveCallback receiveCallback =
        serverStreamSocket->createReceiveCallback(
            NTCCFG_BIND(&test::StreamSocketUtil::processReceiveTimeout,
                        serverStreamSocket,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        ntsa::Error::e_WOULD_BLOCK,
                        &semaphore),
            allocator);

    error = serverStreamSocket->receive(receiveOptions, receiveCallback);
    NTCCFG_TEST_OK(error);

    semaphore.wait();

    {
        ntci::StreamSocketCloseGuard clientStreamSocketCloseGuard(
            clientStreamSocket);

        ntci::StreamSocketCloseGuard serverStreamSocketCloseGuard(
            serverStreamSocket);
    }

    NTCI_LOG_DEBUG("Stream socket receive deadline test complete");
}

void concernStreamSocketReceiveCancellation(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: Receive cancellation.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket receive cancellation test starting");

    const int k_RECEIVE_TIMEOUT_IN_MILLISECONDS = 200;

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_TCP_IPV4_STREAM;

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntci::StreamSocket> serverStreamSocket;
    {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntsi::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntsi::StreamSocket> basicServerSocket;

        error = ntsf::System::createStreamSocketPair(&basicClientSocket,
                                                     &basicServerSocket,
                                                     transport);
        NTCCFG_TEST_FALSE(error);

        clientStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
    }

    bsls::TimeInterval receiveTimeout;
    receiveTimeout.setTotalMilliseconds(k_RECEIVE_TIMEOUT_IN_MILLISECONDS);

    bsls::TimeInterval receiveDeadline =
        serverStreamSocket->currentTime() + receiveTimeout;

    ntca::ReceiveToken receiveToken;
    receiveToken.setValue(1);

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setToken(receiveToken);

    ntci::ReceiveCallback receiveCallback =
        serverStreamSocket->createReceiveCallback(
            NTCCFG_BIND(&test::StreamSocketUtil::processReceiveCancelled,
                        serverStreamSocket,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        &semaphore),
            allocator);

    error = serverStreamSocket->receive(receiveOptions, receiveCallback);
    NTCCFG_TEST_OK(error);

    ntca::TimerOptions timerOptions;

    timerOptions.setOneShot(true);
    timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    ntci::TimerCallback timerCallback =
        serverStreamSocket->createTimerCallback(
            NTCCFG_BIND(&test::StreamSocketUtil::cancelReceive,
                        serverStreamSocket,
                        receiveToken),
            allocator);

    bsl::shared_ptr<ntci::Timer> timer =
        serverStreamSocket->createTimer(timerOptions,
                                        timerCallback,
                                        allocator);

    error = timer->schedule(receiveDeadline);
    NTCCFG_TEST_FALSE(error);

    semaphore.wait();

    {
        ntci::StreamSocketCloseGuard clientStreamSocketCloseGuard(
            clientStreamSocket);

        ntci::StreamSocketCloseGuard serverStreamSocketCloseGuard(
            serverStreamSocket);
    }

    NTCI_LOG_DEBUG("Stream socket receive cancellation test complete");
}

void concernStreamSocketReceiveDeadlineClose(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: validate that receive deadline timer is automatically closed
    // when the socket is closed and then destroyed

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket receive deadline test starting");

    const int k_RECEIVE_TIMEOUT_IN_HOURS = 1;

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_TCP_IPV4_STREAM;

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntci::StreamSocket> serverStreamSocket;
    {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntsi::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntsi::StreamSocket> basicServerSocket;

        error = ntsf::System::createStreamSocketPair(&basicClientSocket,
                                                     &basicServerSocket,
                                                     transport);
        NTCCFG_TEST_FALSE(error);

        clientStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
    }

    bsls::TimeInterval receiveTimeout;
    receiveTimeout.setTotalHours(k_RECEIVE_TIMEOUT_IN_HOURS);

    bsls::TimeInterval receiveDeadline =
        serverStreamSocket->currentTime() + receiveTimeout;

    ntca::ReceiveOptions receiveOptions;
    receiveOptions.setDeadline(receiveDeadline);

    ntci::ReceiveCallback receiveCallback =
        serverStreamSocket->createReceiveCallback(
            NTCCFG_BIND(&test::StreamSocketUtil::processReceiveTimeout,
                        serverStreamSocket,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        ntsa::Error::e_EOF,
                        &semaphore),
            allocator);

    error = serverStreamSocket->receive(receiveOptions, receiveCallback);
    NTCCFG_TEST_OK(error);

    {
        ntci::StreamSocketCloseGuard clientStreamSocketCloseGuard(
            clientStreamSocket);

        ntci::StreamSocketCloseGuard serverStreamSocketCloseGuard(
            serverStreamSocket);
    }

    semaphore.wait();

    NTCI_LOG_DEBUG("Stream socket receive deadline test complete");
}

void concernStreamSocketSendDeadline(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: Send deadlines.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket send deadline test starting");

    const int k_SEND_TIMEOUT_IN_MILLISECONDS = 1000;
    const int k_MESSAGE_A_SIZE               = 1024 * 1024 * 16;
    const int k_MESSAGE_B_SIZE               = 1024;
    const int k_MESSAGE_C_SIZE               = 1024 * 32;

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_TCP_IPV4_STREAM;

    ntsa::Error      error;
    bslmt::Semaphore sendSemaphore;
    bslmt::Semaphore receiveSemaphore;

    bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntci::StreamSocket> serverStreamSocket;
    {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);
        options.setWriteQueueHighWatermark(
            k_MESSAGE_A_SIZE + k_MESSAGE_B_SIZE + k_MESSAGE_C_SIZE);
        options.setReadQueueHighWatermark(k_MESSAGE_A_SIZE + k_MESSAGE_B_SIZE +
                                          k_MESSAGE_C_SIZE);

        options.setSendBufferSize(1024 * 32);
        options.setReceiveBufferSize(1024 * 32);

        bsl::shared_ptr<ntsi::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntsi::StreamSocket> basicServerSocket;

        error = ntsf::System::createStreamSocketPair(&basicClientSocket,
                                                     &basicServerSocket,
                                                     transport);
        NTCCFG_TEST_FALSE(error);

        clientStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
    }

    NTCI_LOG_DEBUG("Generating message A");

    bsl::shared_ptr<bdlbb::Blob> dataA =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(dataA.get(), k_MESSAGE_A_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Generating message A: OK");

    NTCI_LOG_DEBUG("Generating message B");

    bsl::shared_ptr<bdlbb::Blob> dataB =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(dataB.get(), k_MESSAGE_B_SIZE, 0, 1);

    NTCI_LOG_DEBUG("Generating message B: OK");

    NTCI_LOG_DEBUG("Generating message C");

    bsl::shared_ptr<bdlbb::Blob> dataC =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(dataC.get(), k_MESSAGE_C_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Generating message C: OK");

    NTCI_LOG_DEBUG("Sending message A");
    {
        ntca::SendOptions sendOptions;

        error = clientStreamSocket->send(*dataA, sendOptions);
        NTCCFG_TEST_TRUE(!error);
    }

    NTCI_LOG_DEBUG("Sending message B");
    {
        bsls::TimeInterval sendTimeout;
        sendTimeout.setTotalMilliseconds(k_SEND_TIMEOUT_IN_MILLISECONDS);

        bsls::TimeInterval sendDeadline =
            clientStreamSocket->currentTime() + sendTimeout;

        ntca::SendOptions sendOptions;
        sendOptions.setDeadline(sendDeadline);

        ntci::SendCallback sendCallback =
            clientStreamSocket->createSendCallback(
                NTCCFG_BIND(
                    &test::StreamSocketUtil::processSendSuccessOrTimeout,
                    clientStreamSocket,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    bsl::string("B"),
                    ntsa::Error(ntsa::Error::e_CANCELLED),
                    &sendSemaphore),
                allocator);

        error = clientStreamSocket->send(*dataB, sendOptions, sendCallback);
        NTCCFG_TEST_TRUE(!error);
    }

    NTCI_LOG_DEBUG("Sending message C");
    {
        ntca::SendOptions sendOptions;

        error = clientStreamSocket->send(*dataC, sendOptions);
        NTCCFG_TEST_TRUE(!error);
    }

    NTCI_LOG_INFO("Waiting for message B to time out");

    sendSemaphore.wait();

    NTCI_LOG_INFO("Message B has timed out");
    NTCI_LOG_INFO("Receiving message A and C");

    {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setSize(k_MESSAGE_A_SIZE);

        ntci::ReceiveCallback receiveCallback =
            serverStreamSocket->createReceiveCallback(
                NTCCFG_BIND(
                    &test::StreamSocketUtil::processReceiveSuccessOrTimeout,
                    serverStreamSocket,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    bsl::string("A"),
                    ntsa::Error(ntsa::Error::e_OK),
                    &receiveSemaphore),
                allocator);

        error = serverStreamSocket->receive(receiveOptions, receiveCallback);
        NTCCFG_TEST_OK(error);
    }

    {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setSize(k_MESSAGE_C_SIZE);

        ntci::ReceiveCallback receiveCallback =
            serverStreamSocket->createReceiveCallback(
                NTCCFG_BIND(
                    &test::StreamSocketUtil::processReceiveSuccessOrTimeout,
                    serverStreamSocket,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    bsl::string("C"),
                    ntsa::Error(ntsa::Error::e_OK),
                    &receiveSemaphore),
                allocator);

        error = serverStreamSocket->receive(receiveOptions, receiveCallback);
        NTCCFG_TEST_OK(error);
    }

    receiveSemaphore.wait();
    receiveSemaphore.wait();

    {
        ntci::StreamSocketCloseGuard clientStreamSocketCloseGuard(
            clientStreamSocket);

        ntci::StreamSocketCloseGuard serverStreamSocketCloseGuard(
            serverStreamSocket);
    }

    NTCI_LOG_DEBUG("Stream socket send deadline test complete");
}

void concernStreamSocketSendCancellation(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: Send cancellation.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket send cancellation test starting");

    const int k_SEND_TIMEOUT_IN_MILLISECONDS = 1000;
    const int k_MESSAGE_A_SIZE               = 1024 * 1024 * 16;
    const int k_MESSAGE_B_SIZE               = 1024;
    const int k_MESSAGE_C_SIZE               = 1024 * 32;

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_TCP_IPV4_STREAM;

    ntsa::Error      error;
    bslmt::Semaphore sendSemaphore;
    bslmt::Semaphore receiveSemaphore;

    bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntci::StreamSocket> serverStreamSocket;
    {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);
        options.setWriteQueueHighWatermark(
            k_MESSAGE_A_SIZE + k_MESSAGE_B_SIZE + k_MESSAGE_C_SIZE);
        options.setReadQueueHighWatermark(k_MESSAGE_A_SIZE + k_MESSAGE_B_SIZE +
                                          k_MESSAGE_C_SIZE);

        options.setSendBufferSize(1024 * 32);
        options.setReceiveBufferSize(1024 * 32);

        bsl::shared_ptr<ntsi::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntsi::StreamSocket> basicServerSocket;

        error = ntsf::System::createStreamSocketPair(&basicClientSocket,
                                                     &basicServerSocket,
                                                     transport);
        NTCCFG_TEST_FALSE(error);

        clientStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
    }

    NTCI_LOG_DEBUG("Generating message A");

    bsl::shared_ptr<bdlbb::Blob> dataA =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(dataA.get(), k_MESSAGE_A_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Generating message A: OK");

    NTCI_LOG_DEBUG("Generating message B");

    bsl::shared_ptr<bdlbb::Blob> dataB =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(dataB.get(), k_MESSAGE_B_SIZE, 0, 1);

    NTCI_LOG_DEBUG("Generating message B: OK");

    NTCI_LOG_DEBUG("Generating message C");

    bsl::shared_ptr<bdlbb::Blob> dataC =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(dataC.get(), k_MESSAGE_C_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Generating message C: OK");

    NTCI_LOG_DEBUG("Sending message A");
    {
        ntca::SendOptions sendOptions;

        error = clientStreamSocket->send(*dataA, sendOptions);
        NTCCFG_TEST_TRUE(!error);
    }

    NTCI_LOG_DEBUG("Sending message B");
    {
        bsls::TimeInterval sendTimeout;
        sendTimeout.setTotalMilliseconds(k_SEND_TIMEOUT_IN_MILLISECONDS);

        bsls::TimeInterval sendDeadline =
            clientStreamSocket->currentTime() + sendTimeout;

        ntca::SendToken sendToken;
        sendToken.setValue(1);

        ntca::SendOptions sendOptions;
        sendOptions.setToken(sendToken);

        ntci::SendCallback sendCallback =
            clientStreamSocket->createSendCallback(
                NTCCFG_BIND(
                    &test::StreamSocketUtil::processSendSuccessOrCancelled,
                    clientStreamSocket,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    bsl::string("B"),
                    ntsa::Error(ntsa::Error::e_CANCELLED),
                    &sendSemaphore),
                allocator);

        error = clientStreamSocket->send(*dataB, sendOptions, sendCallback);
        NTCCFG_TEST_TRUE(!error);

        ntca::TimerOptions timerOptions;

        timerOptions.setOneShot(true);
        timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

        ntci::TimerCallback timerCallback =
            clientStreamSocket->createTimerCallback(
                NTCCFG_BIND(&test::StreamSocketUtil::cancelSend,
                            clientStreamSocket,
                            sendToken),
                allocator);

        bsl::shared_ptr<ntci::Timer> timer =
            clientStreamSocket->createTimer(timerOptions,
                                            timerCallback,
                                            allocator);

        error = timer->schedule(sendDeadline);
        NTCCFG_TEST_FALSE(error);
    }

    NTCI_LOG_DEBUG("Sending message C");
    {
        ntca::SendOptions sendOptions;

        error = clientStreamSocket->send(*dataC, sendOptions);
        NTCCFG_TEST_TRUE(!error);
    }

    NTCI_LOG_INFO("Waiting for message B to be canceled");

    sendSemaphore.wait();

    NTCI_LOG_INFO("Message B has been canceled");
    NTCI_LOG_INFO("Receiving message A and C");

    {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setSize(k_MESSAGE_A_SIZE);

        ntci::ReceiveCallback receiveCallback =
            serverStreamSocket->createReceiveCallback(
                NTCCFG_BIND(
                    &test::StreamSocketUtil::processReceiveSuccessOrCancelled,
                    serverStreamSocket,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    bsl::string("A"),
                    ntsa::Error(ntsa::Error::e_OK),
                    &receiveSemaphore),
                allocator);

        error = serverStreamSocket->receive(receiveOptions, receiveCallback);
        NTCCFG_TEST_OK(error);
    }

    {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setSize(k_MESSAGE_C_SIZE);

        ntci::ReceiveCallback receiveCallback =
            serverStreamSocket->createReceiveCallback(
                NTCCFG_BIND(
                    &test::StreamSocketUtil::processReceiveSuccessOrCancelled,
                    serverStreamSocket,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    bsl::string("C"),
                    ntsa::Error(ntsa::Error::e_OK),
                    &receiveSemaphore),
                allocator);

        error = serverStreamSocket->receive(receiveOptions, receiveCallback);
        NTCCFG_TEST_OK(error);
    }

    receiveSemaphore.wait();
    receiveSemaphore.wait();

    {
        ntci::StreamSocketCloseGuard clientStreamSocketCloseGuard(
            clientStreamSocket);

        ntci::StreamSocketCloseGuard serverStreamSocketCloseGuard(
            serverStreamSocket);
    }

    NTCI_LOG_DEBUG("Stream socket send cancellation test complete");
}

void concernStreamSocketSendDeadlineClose(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: validate that send deadline timer is automatically closed
    // when the socket is closed and then destroyed

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Stream socket send deadline test starting");

    const int k_SEND_TIMEOUT_IN_HOURS = 1;
    const int k_MESSAGE_A_SIZE        = 1024 * 1024 * 16;
    const int k_MESSAGE_B_SIZE        = 1024;

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_TCP_IPV4_STREAM;

    ntsa::Error      error;
    bslmt::Semaphore sendSemaphore;
    bslmt::Semaphore receiveSemaphore;

    bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntci::StreamSocket> serverStreamSocket;
    {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);
        options.setWriteQueueHighWatermark(k_MESSAGE_A_SIZE +
                                           k_MESSAGE_B_SIZE);
        options.setReadQueueHighWatermark(k_MESSAGE_B_SIZE * 2);
        options.setReadQueueLowWatermark(k_MESSAGE_B_SIZE);

        options.setSendBufferSize(1024 * 32);
        options.setReceiveBufferSize(1024 * 32);

        bsl::shared_ptr<ntsi::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntsi::StreamSocket> basicServerSocket;

        error = ntsf::System::createStreamSocketPair(&basicClientSocket,
                                                     &basicServerSocket,
                                                     transport);
        NTCCFG_TEST_FALSE(error);

        clientStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
    }

    NTCI_LOG_DEBUG("Generating message A");

    bsl::shared_ptr<bdlbb::Blob> dataA =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(dataA.get(), k_MESSAGE_A_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Generating message A: OK");

    NTCI_LOG_DEBUG("Generating message B");

    bsl::shared_ptr<bdlbb::Blob> dataB =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(dataB.get(), k_MESSAGE_B_SIZE, 0, 1);

    NTCI_LOG_DEBUG("Generating message B: OK");

    NTCI_LOG_DEBUG("Sending message A");
    {
        ntca::SendOptions sendOptions;

        error = clientStreamSocket->send(*dataA, sendOptions);
        NTCCFG_TEST_TRUE(!error);
    }

    NTCI_LOG_DEBUG("Sending message B");
    {
        bsls::TimeInterval sendTimeout;
        sendTimeout.setTotalHours(k_SEND_TIMEOUT_IN_HOURS);

        bsls::TimeInterval sendDeadline =
            clientStreamSocket->currentTime() + sendTimeout;

        ntca::SendOptions sendOptions;
        sendOptions.setDeadline(sendDeadline);

        ntci::SendCallback sendCallback =
            clientStreamSocket->createSendCallback(
                NTCCFG_BIND(&test::StreamSocketUtil::processSendAborted,
                            clientStreamSocket,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            bsl::string("B"),
                            &sendSemaphore),
                allocator);

        error = clientStreamSocket->send(*dataB, sendOptions, sendCallback);
        NTCCFG_TEST_TRUE(!error);
    }

    {
        ntci::StreamSocketCloseGuard clientStreamSocketCloseGuard(
            clientStreamSocket);

        ntci::StreamSocketCloseGuard serverStreamSocketCloseGuard(
            serverStreamSocket);
    }

    sendSemaphore.wait();

    NTCI_LOG_DEBUG("Stream socket send deadline test complete");
}

void concernListenerSocketAcceptClose(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: validate that accept deadline timer is automatically closed
    // when the socket is closed and then destroyed

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Listener socket accept cancellation test starting");

    const int k_ACCEPT_TIMEOUT_IN_HOURS = 1;

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_TCP_IPV4_STREAM;

    ntsa::Error      error;
    bslmt::Semaphore semaphore;

    ntca::ListenerSocketOptions options;
    options.setTransport(transport);
    options.setSourceEndpoint(test::EndpointUtil::any(transport));

    bsl::shared_ptr<ntci::Resolver> resolver;

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        scheduler->createListenerSocket(options, allocator);

    error = listenerSocket->open();
    NTCCFG_TEST_FALSE(error);

    error = listenerSocket->listen();
    NTCCFG_TEST_FALSE(error);

    bsls::TimeInterval acceptTimeout;
    acceptTimeout.setTotalHours(k_ACCEPT_TIMEOUT_IN_HOURS);

    const bsls::TimeInterval acceptDeadline =
        listenerSocket->currentTime() + acceptTimeout;

    ntca::AcceptOptions acceptOptions;
    acceptOptions.setDeadline(acceptDeadline);

    ntci::AcceptCallback acceptCallback = listenerSocket->createAcceptCallback(
        NTCCFG_BIND(&test::ListenerSocketUtil::processAcceptCancelled,
                    listenerSocket,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    ntsa::Error::e_EOF,
                    &semaphore),
        allocator);

    error = listenerSocket->accept(acceptOptions, acceptCallback);
    NTCCFG_TEST_OK(error);

    {
        ntci::ListenerSocketCloseGuard listenerSocketCloseGuard(
            listenerSocket);
    }

    semaphore.wait();

    NTCI_LOG_DEBUG("Listener socket accept cancellation test complete");
}

namespace rateLimit {

class DatagramSocketSession : public ntci::DatagramSocketSession
{
    bslmt::Semaphore* d_semaphore_p;
    bsls::AtomicInt   d_readQueueRateLimitApplied;
    bsls::AtomicInt   d_readQueueRateLimitRelaxed;
    bsls::AtomicInt   d_writeQueueRateLimitApplied;
    bsls::AtomicInt   d_writeQueueRateLimitRelaxed;

  private:
    DatagramSocketSession(const DatagramSocketSession&) BSLS_KEYWORD_DELETED;
    DatagramSocketSession& operator=(const DatagramSocketSession&)
        BSLS_KEYWORD_DELETED;

  public:
    explicit DatagramSocketSession(bslmt::Semaphore* semaphore);

    ~DatagramSocketSession() BSLS_KEYWORD_OVERRIDE;

    void processReadQueueRateLimitApplied(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    void processReadQueueRateLimitRelaxed(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    void processWriteQueueRateLimitApplied(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    void processWriteQueueRateLimitRelaxed(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    BSLS_ANNOTATION_NODISCARD int readQueueRateLimitApplied() const;
    BSLS_ANNOTATION_NODISCARD int readQueueRateLimitRelaxed() const;

    BSLS_ANNOTATION_NODISCARD int writeQueueRateLimitApplied() const;
    BSLS_ANNOTATION_NODISCARD int writeQueueRateLimitRelaxed() const;
};

DatagramSocketSession::DatagramSocketSession(bslmt::Semaphore* semaphore)
: d_semaphore_p(semaphore)
, d_readQueueRateLimitApplied(0)
, d_readQueueRateLimitRelaxed(0)
, d_writeQueueRateLimitApplied(0)
, d_writeQueueRateLimitRelaxed(0)
{
}

DatagramSocketSession::~DatagramSocketSession()
{
}

void DatagramSocketSession::processReadQueueRateLimitApplied(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCI_LOG_CONTEXT();
    NTCCFG_TEST_EQ(event.type(),
                   ntca::ReadQueueEventType::e_RATE_LIMIT_APPLIED);
    d_readQueueRateLimitApplied.addAcqRel(1);
    d_semaphore_p->post();
}

void DatagramSocketSession::processReadQueueRateLimitRelaxed(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCI_LOG_CONTEXT();
    NTCCFG_TEST_EQ(event.type(),
                   ntca::ReadQueueEventType::e_RATE_LIMIT_RELAXED);
    d_readQueueRateLimitRelaxed.addAcqRel(1);
    d_semaphore_p->post();
}

void DatagramSocketSession::processWriteQueueRateLimitApplied(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCI_LOG_CONTEXT();
    NTCCFG_TEST_EQ(event.type(),
                   ntca::WriteQueueEventType::e_RATE_LIMIT_APPLIED);
    d_writeQueueRateLimitApplied.addAcqRel(1);
    d_semaphore_p->post();
}

void DatagramSocketSession::processWriteQueueRateLimitRelaxed(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCI_LOG_CONTEXT();
    NTCCFG_TEST_EQ(event.type(),
                   ntca::WriteQueueEventType::e_RATE_LIMIT_RELAXED);
    d_writeQueueRateLimitRelaxed.addAcqRel(1);
    d_semaphore_p->post();
}

int DatagramSocketSession::readQueueRateLimitApplied() const
{
    return d_readQueueRateLimitApplied.loadAcquire();
}

int DatagramSocketSession::readQueueRateLimitRelaxed() const
{
    return d_readQueueRateLimitRelaxed.loadAcquire();
}

int DatagramSocketSession::writeQueueRateLimitApplied() const
{
    return d_writeQueueRateLimitApplied.loadAcquire();
}

int DatagramSocketSession::writeQueueRateLimitRelaxed() const
{
    return d_writeQueueRateLimitRelaxed.loadAcquire();
}

class StreamSocketSession : public ntci::StreamSocketSession
{
    bslmt::Semaphore* d_semaphore_p;
    bsls::AtomicInt   d_readQueueRateLimitApplied;
    bsls::AtomicInt   d_readQueueRateLimitRelaxed;
    bsls::AtomicInt   d_writeQueueRateLimitApplied;
    bsls::AtomicInt   d_writeQueueRateLimitRelaxed;

  private:
    StreamSocketSession(const StreamSocketSession&) BSLS_KEYWORD_DELETED;
    StreamSocketSession& operator=(const StreamSocketSession&)
        BSLS_KEYWORD_DELETED;

  public:
    explicit StreamSocketSession(bslmt::Semaphore* semaphore);

    ~StreamSocketSession() BSLS_KEYWORD_OVERRIDE;

    void processReadQueueRateLimitApplied(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    void processReadQueueRateLimitRelaxed(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    void processWriteQueueRateLimitApplied(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    void processWriteQueueRateLimitRelaxed(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    BSLS_ANNOTATION_NODISCARD int readQueueRateLimitApplied() const;
    BSLS_ANNOTATION_NODISCARD int readQueueRateLimitRelaxed() const;
    BSLS_ANNOTATION_NODISCARD int writeQueueRateLimitApplied() const;
    BSLS_ANNOTATION_NODISCARD int writeQueueRateLimitRelaxed() const;
};

StreamSocketSession::StreamSocketSession(bslmt::Semaphore* semaphore)
: d_semaphore_p(semaphore)
, d_readQueueRateLimitApplied(0)
, d_readQueueRateLimitRelaxed(0)
, d_writeQueueRateLimitApplied(0)
, d_writeQueueRateLimitRelaxed(0)
{
}

StreamSocketSession::~StreamSocketSession()
{
}

void StreamSocketSession::processReadQueueRateLimitApplied(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCI_LOG_CONTEXT();
    NTCCFG_TEST_EQ(event.type(),
                   ntca::ReadQueueEventType::e_RATE_LIMIT_APPLIED);
    d_readQueueRateLimitApplied.addAcqRel(1);
    d_semaphore_p->post();
}

void StreamSocketSession::processWriteQueueRateLimitApplied(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCI_LOG_CONTEXT();
    NTCCFG_TEST_EQ(event.type(),
                   ntca::WriteQueueEventType::e_RATE_LIMIT_APPLIED);
    d_writeQueueRateLimitApplied.addAcqRel(1);
    d_semaphore_p->post();
}

void StreamSocketSession::processReadQueueRateLimitRelaxed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCI_LOG_CONTEXT();
    NTCCFG_TEST_EQ(event.type(),
                   ntca::ReadQueueEventType::e_RATE_LIMIT_RELAXED);
    d_readQueueRateLimitRelaxed.addAcqRel(1);
    d_semaphore_p->post();
}

void StreamSocketSession::processWriteQueueRateLimitRelaxed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCI_LOG_CONTEXT();
    NTCCFG_TEST_EQ(event.type(),
                   ntca::WriteQueueEventType::e_RATE_LIMIT_RELAXED);
    d_writeQueueRateLimitRelaxed.addAcqRel(1);
    d_semaphore_p->post();
}

int StreamSocketSession::readQueueRateLimitApplied() const
{
    return d_readQueueRateLimitApplied.loadAcquire();
}

int StreamSocketSession::writeQueueRateLimitApplied() const
{
    return d_writeQueueRateLimitApplied.loadAcquire();
}

int StreamSocketSession::readQueueRateLimitRelaxed() const
{
    return d_readQueueRateLimitRelaxed.loadAcquire();
}

int StreamSocketSession::writeQueueRateLimitRelaxed() const
{
    return d_writeQueueRateLimitRelaxed.loadAcquire();
}

class ListenerSocketSession : public ntci::ListenerSocketSession
{
    bslmt::Semaphore* d_semaphore_p;
    bsls::AtomicInt   d_rateLimitApplied;
    bsls::AtomicInt   d_rateLimitRelaxed;

  private:
    ListenerSocketSession(const ListenerSocketSession&) BSLS_KEYWORD_DELETED;
    ListenerSocketSession& operator=(const ListenerSocketSession&)
        BSLS_KEYWORD_DELETED;

  public:
    explicit ListenerSocketSession(bslmt::Semaphore* semaphore);

    ~ListenerSocketSession() BSLS_KEYWORD_OVERRIDE;

    void processAcceptQueueRateLimitApplied(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    void processAcceptQueueRateLimitRelaxed(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    BSLS_ANNOTATION_NODISCARD int rateLimitApplied() const;
    BSLS_ANNOTATION_NODISCARD int rateLimitRelaxed() const;
};

ListenerSocketSession::ListenerSocketSession(bslmt::Semaphore* semaphore)
: d_semaphore_p(semaphore)
{
}

ListenerSocketSession::~ListenerSocketSession()
{
}

void ListenerSocketSession::processAcceptQueueRateLimitApplied(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCI_LOG_CONTEXT();
    NTCCFG_TEST_EQ(event.type(),
                   ntca::AcceptQueueEventType::e_RATE_LIMIT_APPLIED);
    d_rateLimitApplied.addAcqRel(1);
    d_semaphore_p->post();
}

void ListenerSocketSession::processAcceptQueueRateLimitRelaxed(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCI_LOG_CONTEXT();
    NTCCFG_TEST_EQ(event.type(),
                   ntca::AcceptQueueEventType::e_RATE_LIMIT_RELAXED);
    d_rateLimitRelaxed.addAcqRel(1);
    d_semaphore_p->post();
}

int ListenerSocketSession::rateLimitApplied() const
{
    return d_rateLimitApplied.loadAcquire();
}

int ListenerSocketSession::rateLimitRelaxed() const
{
    return d_rateLimitApplied.loadAcquire();
}

}  // rateLimit

void concernDatagramSocketReceiveRateLimitTimerClose(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: validate that receive rate limit timer is automatically closed
    // when the socket is closed and then destroyed

    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Test started");

    const bsl::size_t k_MESSAGE_SIZE = 65507;
    const bsl::size_t k_NUM_MESSAGES = 1024;

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_UDP_IPV4_DATAGRAM;

    ntsa::Error      error;
    bslmt::Semaphore receiveSemaphore;
    bslmt::Semaphore rateLimitSemaphore;

    bsl::shared_ptr<ntci::DatagramSocket> clientDatagramSocket;
    bsl::shared_ptr<ntci::DatagramSocket> serverDatagramSocket;
    {
        ntca::DatagramSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntsi::DatagramSocket> basicClientSocket;
        bsl::shared_ptr<ntsi::DatagramSocket> basicServerSocket;

        error = ntsf::System::createDatagramSocketPair(&basicClientSocket,
                                                       &basicServerSocket,
                                                       transport);
        NTCCFG_TEST_FALSE(error);

        {
            ntsa::SocketOption socketOption;
            socketOption.makeSendBufferSize(k_MESSAGE_SIZE);

            error = ntsf::System::setOption(basicClientSocket->handle(),
                                            socketOption);
            NTCCFG_TEST_FALSE(error);
        }

        {
            ntsa::SocketOption socketOption;
            error = ntsf::System::getOption(
                &socketOption,
                basicClientSocket->handle(),
                ntsa::SocketOptionType::e_SEND_BUFFER_SIZE);
            NTCCFG_TEST_FALSE(error);

            NTCCFG_TEST_TRUE(socketOption.isSendBufferSize());

            const bsl::size_t sendBufferSize = socketOption.sendBufferSize();
            NTCCFG_TEST_GE(sendBufferSize, k_MESSAGE_SIZE);
        }

        clientDatagramSocket =
            scheduler->createDatagramSocket(options, allocator);

        error = clientDatagramSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverDatagramSocket =
            scheduler->createDatagramSocket(options, allocator);

        error = serverDatagramSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
    }

    bsl::shared_ptr<rateLimit::DatagramSocketSession> session;
    session.createInplace(allocator, &rateLimitSemaphore);
    serverDatagramSocket->registerSession(session);

    NTCI_LOG_DEBUG("Generating message");
    bsl::shared_ptr<bdlbb::Blob> message =
        clientDatagramSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(message.get(), k_MESSAGE_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Generating message: OK");

    {
        bsl::shared_ptr<ntcs::RateLimiter> limiter;

        //configure sustained rate limit in a way that it will be reached
        // immediately. e.g. 2 bytes per second
        const bsl::uint64_t sustainedRateLimit = 2;
        bsls::TimeInterval  sustainedRateWindow;
        sustainedRateWindow.setTotalHours(1);

        // do not care about peak limit
        const bsl::uint64_t peakRateLimit =
            bsl::numeric_limits<bsl::uint32_t>::max();
        bsls::TimeInterval peakRateWindow;
        peakRateWindow.setTotalSeconds(1);

        limiter.createInplace(allocator,
                              sustainedRateLimit,
                              sustainedRateWindow,
                              peakRateLimit,
                              peakRateWindow,
                              serverDatagramSocket->currentTime());
        serverDatagramSocket->setReadRateLimiter(limiter);
    }

    NTCI_LOG_DEBUG("Sending messages");
    {
        for (bsl::size_t i = 0; i < k_NUM_MESSAGES; ++i) {
            ntca::SendOptions sendOptions;

            error = clientDatagramSocket->send(*message, sendOptions);
            NTCCFG_TEST_TRUE(!error);
        }
    }

    NTCI_LOG_DEBUG("Receiving messages");
    {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setSize(k_MESSAGE_SIZE * k_NUM_MESSAGES);

        ntci::ReceiveCallback receiveCallback =
            serverDatagramSocket->createReceiveCallback(
                NTCCFG_BIND(&test::DatagramSocketUtil::processReceiveFailed,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            &receiveSemaphore),
                allocator);

        error = serverDatagramSocket->receive(receiveOptions, receiveCallback);
        NTCCFG_TEST_OK(error);
    }

    rateLimitSemaphore.wait();

    {
        ntci::DatagramSocketCloseGuard closeGuardClient(clientDatagramSocket);
        ntci::DatagramSocketCloseGuard closeGuardServer(serverDatagramSocket);
    }

    receiveSemaphore.wait();

    NTCCFG_TEST_GE(session->readQueueRateLimitApplied(), 1);
}

void concernStreamSocketReceiveRateLimitTimerClose(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: validate that receive rate limit timer is automatically closed
    // when the socket is closed and then destroyed

    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Test started");

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_TCP_IPV4_STREAM;

    ntsa::Error      error;
    bslmt::Semaphore receiveSemaphore;
    bslmt::Semaphore rateLimitSemaphore;

    bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntci::StreamSocket> serverStreamSocket;
    {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntsi::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntsi::StreamSocket> basicServerSocket;

        error = ntsf::System::createStreamSocketPair(&basicClientSocket,
                                                     &basicServerSocket,
                                                     transport);
        NTCCFG_TEST_FALSE(error);

        clientStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
    }

    bsl::shared_ptr<rateLimit::StreamSocketSession> session;
    session.createInplace(allocator, &rateLimitSemaphore);
    serverStreamSocket->registerSession(session);

    const int k_MESSAGE_SIZE = 65000;
    const int k_NUM_MESSAGES = 1024;

    NTCI_LOG_DEBUG("Generating message");
    bsl::shared_ptr<bdlbb::Blob> message =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(message.get(), k_MESSAGE_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Generating message: OK");

    {
        bsl::shared_ptr<ntcs::RateLimiter> limiter;

        //configure sustained rate limit in a way that it will be reached
        // immediately. e.g. 2 bytes per hour
        const bsl::uint64_t sustainedRateLimit = 2;
        bsls::TimeInterval  sustainedRateWindow;
        sustainedRateWindow.setTotalHours(1);

        // do not care about peak limit
        const bsl::uint64_t peakRateLimit =
            bsl::numeric_limits<bsl::uint32_t>::max();
        bsls::TimeInterval peakRateWindow;
        peakRateWindow.setTotalSeconds(1);

        limiter.createInplace(allocator,
                              sustainedRateLimit,
                              sustainedRateWindow,
                              peakRateLimit,
                              peakRateWindow,
                              serverStreamSocket->currentTime());
        serverStreamSocket->setReadRateLimiter(limiter);
    }

    NTCI_LOG_DEBUG("Sending messages");
    {
        for (int i = 0; i < k_NUM_MESSAGES; ++i) {
            ntca::SendOptions sendOptions;

            error = clientStreamSocket->send(*message, sendOptions);
            NTCCFG_TEST_TRUE(!error);
        }
    }

    NTCI_LOG_DEBUG("Receiving messages");
    {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setSize(k_MESSAGE_SIZE * k_NUM_MESSAGES);

        ntci::ReceiveCallback receiveCallback =
            serverStreamSocket->createReceiveCallback(
                NTCCFG_BIND(&test::DatagramSocketUtil::processReceiveFailed,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            &receiveSemaphore),
                allocator);

        error = serverStreamSocket->receive(receiveOptions, receiveCallback);
        NTCCFG_TEST_OK(error);
    }

    rateLimitSemaphore.wait();

    {
        ntci::StreamSocketCloseGuard closeGuardClient(clientStreamSocket);
        ntci::StreamSocketCloseGuard closeGuardServer(serverStreamSocket);
    }

    receiveSemaphore.wait();

    NTCCFG_TEST_GE(session->readQueueRateLimitApplied(), 1);
}

void concernDatagramSocketSendRateLimitTimerClose(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: validate that send rate limit timer is automatically closed
    // when the socket is closed and then destroyed

    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Test started");

    const bsl::size_t k_MESSAGE_SIZE = 65507;
    const bsl::size_t k_NUM_MESSAGES = 1024;

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_UDP_IPV4_DATAGRAM;

    ntsa::Error      error;
    bslmt::Semaphore rateLimitSemaphore;

    bsl::shared_ptr<ntci::DatagramSocket> clientDatagramSocket;
    bsl::shared_ptr<ntci::DatagramSocket> serverDatagramSocket;
    {
        ntca::DatagramSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntsi::DatagramSocket> basicClientSocket;
        bsl::shared_ptr<ntsi::DatagramSocket> basicServerSocket;

        error = ntsf::System::createDatagramSocketPair(&basicClientSocket,
                                                       &basicServerSocket,
                                                       transport);
        NTCCFG_TEST_FALSE(error);

        {
            ntsa::SocketOption socketOption;
            socketOption.makeSendBufferSize(k_MESSAGE_SIZE);

            error = ntsf::System::setOption(basicClientSocket->handle(),
                                            socketOption);
            NTCCFG_TEST_FALSE(error);
        }

        {
            ntsa::SocketOption socketOption;
            error = ntsf::System::getOption(
                &socketOption,
                basicClientSocket->handle(),
                ntsa::SocketOptionType::e_SEND_BUFFER_SIZE);
            NTCCFG_TEST_FALSE(error);

            NTCCFG_TEST_TRUE(socketOption.isSendBufferSize());

            const bsl::size_t sendBufferSize = socketOption.sendBufferSize();
            NTCCFG_TEST_GE(sendBufferSize, k_MESSAGE_SIZE);
        }

        clientDatagramSocket =
            scheduler->createDatagramSocket(options, allocator);

        error = clientDatagramSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverDatagramSocket =
            scheduler->createDatagramSocket(options, allocator);

        error = serverDatagramSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
    }

    bsl::shared_ptr<rateLimit::DatagramSocketSession> session;
    session.createInplace(allocator, &rateLimitSemaphore);
    clientDatagramSocket->registerSession(session);

    NTCI_LOG_DEBUG("Generating message");
    bsl::shared_ptr<bdlbb::Blob> message =
        clientDatagramSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(message.get(), k_MESSAGE_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Generating message: OK");

    {
        bsl::shared_ptr<ntcs::RateLimiter> limiter;

        //configure sustained rate limit in a way that it will be reached
        // immediately. e.g. 2 bytes per hour
        const bsl::uint64_t sustainedRateLimit = 2;
        bsls::TimeInterval  sustainedRateWindow;
        sustainedRateWindow.setTotalHours(1);

        // do not care about peak limit
        const bsl::uint64_t peakRateLimit =
            bsl::numeric_limits<bsl::uint32_t>::max();
        bsls::TimeInterval peakRateWindow;
        peakRateWindow.setTotalSeconds(1);

        limiter.createInplace(allocator,
                              sustainedRateLimit,
                              sustainedRateWindow,
                              peakRateLimit,
                              peakRateWindow,
                              serverDatagramSocket->currentTime());
        clientDatagramSocket->setWriteRateLimiter(limiter);
    }

    NTCI_LOG_DEBUG("Sending messages");
    {
        for (bsl::size_t i = 0; i < k_NUM_MESSAGES; ++i) {
            ntca::SendOptions sendOptions;

            error = clientDatagramSocket->send(*message, sendOptions);
            NTCCFG_TEST_TRUE(!error);
        }
    }
    NTCI_LOG_DEBUG("Sending done");

    rateLimitSemaphore.wait();

    NTCCFG_TEST_GE(session->writeQueueRateLimitApplied(), 1);

    {
        ntci::DatagramSocketCloseGuard closeGuardClient(clientDatagramSocket);
        ntci::DatagramSocketCloseGuard closeGuardServer(serverDatagramSocket);
    }
}

void concernStreamSocketSendRateLimitTimerClose(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: validate that send rate limit timer is automatically closed
    // when the socket is closed and then destroyed

    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Test started");

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_TCP_IPV4_STREAM;

    ntsa::Error      error;
    bslmt::Semaphore receiveSemaphore;
    bslmt::Semaphore rateLimitSemaphore;

    bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntci::StreamSocket> serverStreamSocket;
    {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntsi::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntsi::StreamSocket> basicServerSocket;

        error = ntsf::System::createStreamSocketPair(&basicClientSocket,
                                                     &basicServerSocket,
                                                     transport);
        NTCCFG_TEST_FALSE(error);

        clientStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
    }

    bsl::shared_ptr<rateLimit::StreamSocketSession> session;
    session.createInplace(allocator, &rateLimitSemaphore);
    clientStreamSocket->registerSession(session);

    const int k_MESSAGE_SIZE = 65000;
    const int k_NUM_MESSAGES = 1024;

    NTCI_LOG_DEBUG("Generating message");
    bsl::shared_ptr<bdlbb::Blob> message =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(message.get(), k_MESSAGE_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Generating message: OK");

    {
        bsl::shared_ptr<ntcs::RateLimiter> limiter;

        //configure sustained rate limit in a way that it will be reached
        // immediately. e.g. 2 bytes per hour
        const bsl::uint64_t sustainedRateLimit = 2;
        bsls::TimeInterval  sustainedRateWindow;
        sustainedRateWindow.setTotalHours(1);

        // do not care about peak limit
        const bsl::uint64_t peakRateLimit =
            bsl::numeric_limits<bsl::uint32_t>::max();
        bsls::TimeInterval peakRateWindow;
        peakRateWindow.setTotalSeconds(1);

        limiter.createInplace(allocator,
                              sustainedRateLimit,
                              sustainedRateWindow,
                              peakRateLimit,
                              peakRateWindow,
                              serverStreamSocket->currentTime());
        clientStreamSocket->setWriteRateLimiter(limiter);
    }

    NTCI_LOG_DEBUG("Sending messages");
    {
        for (int i = 0; i < k_NUM_MESSAGES; ++i) {
            ntca::SendOptions sendOptions;

            error = clientStreamSocket->send(*message, sendOptions);
            NTCCFG_TEST_TRUE(!error);
        }
    }

    rateLimitSemaphore.wait();

    {
        ntci::StreamSocketCloseGuard closeGuardClient(clientStreamSocket);
        ntci::StreamSocketCloseGuard closeGuardServer(serverStreamSocket);
    }

    NTCCFG_TEST_GE(session->writeQueueRateLimitApplied(), 1);
}

void concernListenerSocketAcceptRateLimitTimerClose(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: validate that accept rate limit timer is automatically closed
    // when the socket is closed and then destroyed

    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Test started");

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_TCP_IPV4_STREAM;

    ntsa::Error      error;
    bslmt::Semaphore acceptLimitSemaphore;

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket;
    {
        ntca::ListenerSocketOptions options;
        options.setTransport(transport);
        options.setSourceEndpoint(test::EndpointUtil::any(transport));

        listenerSocket = scheduler->createListenerSocket(options, allocator);
    }

    bsl::shared_ptr<rateLimit::ListenerSocketSession> session;
    session.createInplace(allocator, &acceptLimitSemaphore);
    listenerSocket->registerSession(session);

    {
        bsl::shared_ptr<ntcs::RateLimiter> limiter;

        // configure sustained rate limit in a way that it will be reached
        // immediately. e.g. 1 acceptance per second
        const bsl::uint64_t sustainedRateLimit = 1;
        bsls::TimeInterval  sustainedRateWindow;
        sustainedRateWindow.setTotalHours(1);

        // do not care about peak limit
        const bsl::uint64_t peakRateLimit = 1;
        bsls::TimeInterval  peakRateWindow;
        peakRateWindow.setTotalSeconds(1);

        limiter.createInplace(allocator,
                              sustainedRateLimit,
                              sustainedRateWindow,
                              peakRateLimit,
                              peakRateWindow,
                              listenerSocket->currentTime());
        listenerSocket->setAcceptRateLimiter(limiter);
    }

    error = listenerSocket->open();
    NTCCFG_TEST_FALSE(error);

    error = listenerSocket->listen();
    NTCCFG_TEST_FALSE(error);

    NTCI_LOG_DEBUG("Listener socket starts listening");

    const int numSockets = 10;

    bsl::vector<bsl::shared_ptr<ntci::StreamSocket> > clientSockets(allocator);
    bsl::vector<bsl::shared_ptr<ntci::ConnectFuture> > connectFutures(
        allocator);
    bsl::vector<bsl::shared_ptr<ntci::AcceptFuture> > acceptFutures(allocator);

    NTCI_LOG_DEBUG("Creating client sockets");

    for (int i = 0; i < numSockets; ++i) {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket =
            scheduler->createStreamSocket(options, allocator);

        error = clientStreamSocket->open();
        NTCCFG_TEST_OK(error);
        clientSockets.push_back(clientStreamSocket);
    }

    NTCI_LOG_DEBUG("Preparing futures");
    for (int i = 0; i < numSockets; ++i) {
        bsl::shared_ptr<ntci::ConnectFuture> connectFuture;
        connectFuture.createInplace(allocator);
        connectFutures.push_back(connectFuture);

        bsl::shared_ptr<ntci::AcceptFuture> acceptFuture;
        acceptFuture.createInplace(allocator);
        acceptFutures.push_back(acceptFuture);
    }

    NTCI_LOG_DEBUG("Accepting connections");
    for (int i = 0; i < numSockets; ++i) {
        error =
            listenerSocket->accept(ntca::AcceptOptions(), *(acceptFutures[i]));
        NTCCFG_TEST_OK(error);
    }

    NTCI_LOG_DEBUG("Initiating connections");
    for (int i = 0; i < numSockets; ++i) {
        error = clientSockets[i]->connect(listenerSocket->sourceEndpoint(),
                                          ntca::ConnectOptions(),
                                          *(connectFutures[i]));
        NTCCFG_TEST_OK(error);
    }

    acceptLimitSemaphore.wait();
    acceptLimitSemaphore.wait();

    NTCCFG_TEST_GE(session->rateLimitApplied(), 1);
    NTCCFG_TEST_GE(session->rateLimitRelaxed(), 1);

    {
        {
            ntci::ListenerSocketCloseGuard listenerSocketCloseGuard(
                listenerSocket);
        }
        listenerSocket.reset();
    }

    NTCI_LOG_DEBUG("Cleaning accepted connections");
    for (size_t i = 0; i < acceptFutures.size(); ++i) {
        const bsls::TimeInterval zero;
        ntci::AcceptResult       acceptResult;
        error = acceptFutures[i]->wait(&acceptResult, zero);
        if (!error) {
            ntci::StreamSocketCloseGuard closeGuard(
                acceptResult.streamSocket());
        }
    }

    for (size_t i = 0; i < clientSockets.size(); ++i) {
        ntci::StreamSocketCloseGuard closeGuard(clientSockets[i]);
    }
}

void concernStreamSocketConnectRetryTimerClose(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: validate that connect retry timer is automatically closed
    // when the socket is closed and then destroyed

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 2;
    const double      k_RETRY_INTERVAL_HOURS    = 1;

    NTCI_LOG_CONTEXT();

    ntsa::Error                  error;
    const ntsa::Transport::Value transport =
        ntsa::Transport::e_TCP_IPV4_STREAM;

    NTCI_LOG_DEBUG("Creating stream socket");
    bsl::shared_ptr<ntci::StreamSocket> streamSocket;
    {
        ntca::StreamSocketOptions streamSocketOptions;
        streamSocketOptions.setTransport(transport);

        streamSocket =
            scheduler->createStreamSocket(streamSocketOptions, allocator);
    }

    NTCI_LOG_DEBUG("Creating and binding listener socket");
    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket;
    {
        ntca::ListenerSocketOptions options;
        options.setTransport(transport);
        options.setSourceEndpoint(test::EndpointUtil::any(transport));

        listenerSocket = scheduler->createListenerSocket(options, allocator);
        listenerSocket->open();
    }
    ntci::ListenerSocketCloseGuard listenerCloseGuard(listenerSocket);

    NTCI_LOG_DEBUG("Trying to connect");
    ntci::ConnectFuture connectFuture(allocator);
    {
        bsls::TimeInterval retryInterval;
        retryInterval.setTotalHours(k_RETRY_INTERVAL_HOURS);
        ntca::ConnectOptions connectOptions;
        connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS - 1);
        connectOptions.setRetryInterval(retryInterval);

        const ntsa::Endpoint endpoint = listenerSocket->sourceEndpoint();

        error = streamSocket->connect(endpoint, connectOptions, connectFuture);
        NTCCFG_TEST_OK(error);
    }

    NTCI_LOG_DEBUG("closing the socket");
    {
        {
            ntci::StreamSocketCloseGuard closeGuard(streamSocket);
        }
        streamSocket.reset();
    }
}

void concernStreamSocketConnectDeadlineTimerClose(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: validate that connect deadline timer is automatically closed
    // when the socket is closed and then destroyed

    const bsl::size_t k_MAX_CONNECTION_ATTEMPTS = 60;
    const int         k_DEADLINE_INTERVAL_HOURS = 1;
    const int         k_RETRY_INTERVAL_MINUTES  = 1;

    NTCI_LOG_CONTEXT();

    ntsa::Error                  error;
    const ntsa::Transport::Value transport =
        ntsa::Transport::e_TCP_IPV4_STREAM;

    NTCI_LOG_DEBUG("Creating stream socket");
    bsl::shared_ptr<ntci::StreamSocket> streamSocket;
    {
        ntca::StreamSocketOptions streamSocketOptions;
        streamSocketOptions.setTransport(transport);

        streamSocket =
            scheduler->createStreamSocket(streamSocketOptions, allocator);
    }

    NTCI_LOG_DEBUG("Creating and binding listener socket");
    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket;
    {
        ntca::ListenerSocketOptions options;
        options.setTransport(transport);
        options.setSourceEndpoint(test::EndpointUtil::any(transport));

        listenerSocket = scheduler->createListenerSocket(options, allocator);
        listenerSocket->open();
    }
    ntci::ListenerSocketCloseGuard listenerCloseGuard(listenerSocket);

    NTCI_LOG_DEBUG("Trying to connect");
    ntci::ConnectFuture connectFuture(allocator);
    {
        bsls::TimeInterval deadlineInterval;
        deadlineInterval.setTotalHours(k_DEADLINE_INTERVAL_HOURS);

        bsls::TimeInterval retryInterval;
        retryInterval.setTotalMinutes(k_RETRY_INTERVAL_MINUTES);

        ntca::ConnectOptions connectOptions;
        connectOptions.setRetryCount(k_MAX_CONNECTION_ATTEMPTS);
        connectOptions.setRetryInterval(retryInterval);
        connectOptions.setDeadline(streamSocket->currentTime() +
                                   deadlineInterval);

        const ntsa::Endpoint endpoint = listenerSocket->sourceEndpoint();

        error = streamSocket->connect(endpoint, connectOptions, connectFuture);
        NTCCFG_TEST_OK(error);
    }

    NTCI_LOG_DEBUG("closing the socket");
    {
        {
            ntci::StreamSocketCloseGuard closeGuard(streamSocket);
        }
        streamSocket.reset();
    }
}

void concernDatagramSocketReceiveRateLimitTimerEventNotifications(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: validate that e_RATE_LIMIT_APPLIED and e_RATE_LIMIT_RELAXED
    // events are fired

    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Test started");

    const bsl::size_t k_MESSAGE_SIZE = 65507;
    const bsl::size_t k_NUM_MESSAGES = 100;

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_UDP_IPV4_DATAGRAM;

    ntsa::Error      error;
    bslmt::Semaphore receiveSemaphore;
    bslmt::Semaphore rateLimitSemaphore;

    bsl::shared_ptr<ntci::DatagramSocket> clientDatagramSocket;
    bsl::shared_ptr<ntci::DatagramSocket> serverDatagramSocket;
    {
        ntca::DatagramSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntsi::DatagramSocket> basicClientSocket;
        bsl::shared_ptr<ntsi::DatagramSocket> basicServerSocket;

        error = ntsf::System::createDatagramSocketPair(&basicClientSocket,
                                                       &basicServerSocket,
                                                       transport);
        NTCCFG_TEST_FALSE(error);

        {
            ntsa::SocketOption socketOption;
            socketOption.makeSendBufferSize(k_MESSAGE_SIZE);

            error = ntsf::System::setOption(basicClientSocket->handle(),
                                            socketOption);
            NTCCFG_TEST_FALSE(error);
        }

        {
            ntsa::SocketOption socketOption;
            error = ntsf::System::getOption(
                &socketOption,
                basicClientSocket->handle(),
                ntsa::SocketOptionType::e_SEND_BUFFER_SIZE);
            NTCCFG_TEST_FALSE(error);

            NTCCFG_TEST_TRUE(socketOption.isSendBufferSize());

            const bsl::size_t sendBufferSize = socketOption.sendBufferSize();
            NTCCFG_TEST_GE(sendBufferSize, k_MESSAGE_SIZE);
        }

        clientDatagramSocket =
            scheduler->createDatagramSocket(options, allocator);

        error = clientDatagramSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverDatagramSocket =
            scheduler->createDatagramSocket(options, allocator);

        error = serverDatagramSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
    }

    bsl::shared_ptr<rateLimit::DatagramSocketSession> session;
    session.createInplace(allocator, &rateLimitSemaphore);
    serverDatagramSocket->registerSession(session);

    NTCI_LOG_DEBUG("Generating message");
    bsl::shared_ptr<bdlbb::Blob> message =
        clientDatagramSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(message.get(), k_MESSAGE_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Generating message: OK");

    {
        bsl::shared_ptr<ntcs::RateLimiter> limiter;

        // configure sustained rate limit in a way that it will be reached
        // immediately

        const bsl::uint64_t      sustainedRateLimit = k_MESSAGE_SIZE;
        const bsls::TimeInterval sustainedRateWindow(0.01);

        // do not care about peak limit
        const bsl::uint64_t peakRateLimit =
            bsl::numeric_limits<bsl::uint32_t>::max();
        bsls::TimeInterval peakRateWindow;
        peakRateWindow.setTotalSeconds(10);

        limiter.createInplace(allocator,
                              sustainedRateLimit,
                              sustainedRateWindow,
                              peakRateLimit,
                              peakRateWindow,
                              serverDatagramSocket->currentTime());
        serverDatagramSocket->setReadRateLimiter(limiter);
    }

    NTCI_LOG_DEBUG("Sending messages");
    {
        for (bsl::size_t i = 0; i < k_NUM_MESSAGES; ++i) {
            ntca::SendOptions sendOptions;

            NTCI_LOG_DEBUG("Sending message %d", i);

            error = clientDatagramSocket->send(*message, sendOptions);
            NTCCFG_TEST_TRUE(!error);

            NTCI_LOG_DEBUG("Sent message %d: OK", i);
        }
    }

    NTCI_LOG_DEBUG("Receiving messages");
    {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setSize(k_MESSAGE_SIZE * k_NUM_MESSAGES);

        ntci::ReceiveCallback receiveCallback =
            serverDatagramSocket->createReceiveCallback(
                NTCCFG_BIND(&test::DatagramSocketUtil::processReceiveFailed,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            &receiveSemaphore),
                allocator);

        error = serverDatagramSocket->receive(receiveOptions, receiveCallback);
        NTCCFG_TEST_OK(error);
    }

    rateLimitSemaphore.wait();
    rateLimitSemaphore.wait();

    NTCCFG_TEST_GE(session->readQueueRateLimitApplied(), 1);
    NTCCFG_TEST_GE(session->readQueueRateLimitRelaxed(), 1);

    {
        ntci::DatagramSocketCloseGuard closeGuardClient(clientDatagramSocket);
        ntci::DatagramSocketCloseGuard closeGuardServer(serverDatagramSocket);
    }

    receiveSemaphore.wait();
}

void concernDatagramSocketSendRateLimitTimerEventNotifications(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: validate that e_RATE_LIMIT_APPLIED and e_RATE_LIMIT_RELAXED
    // events are fired

    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Test started");

    const bsl::size_t k_MESSAGE_SIZE = 65507;
    const bsl::size_t k_NUM_MESSAGES = 100;

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_UDP_IPV4_DATAGRAM;

    ntsa::Error      error;
    bslmt::Semaphore receiveSemaphore;
    bslmt::Semaphore rateLimitSemaphore;

    bsl::shared_ptr<ntci::DatagramSocket> clientDatagramSocket;
    bsl::shared_ptr<ntci::DatagramSocket> serverDatagramSocket;
    {
        ntca::DatagramSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntsi::DatagramSocket> basicClientSocket;
        bsl::shared_ptr<ntsi::DatagramSocket> basicServerSocket;

        error = ntsf::System::createDatagramSocketPair(&basicClientSocket,
                                                       &basicServerSocket,
                                                       transport);
        NTCCFG_TEST_FALSE(error);

        {
            ntsa::SocketOption socketOption;
            socketOption.makeSendBufferSize(k_MESSAGE_SIZE);

            error = ntsf::System::setOption(basicClientSocket->handle(),
                                            socketOption);
            NTCCFG_TEST_FALSE(error);
        }

        {
            ntsa::SocketOption socketOption;
            error = ntsf::System::getOption(
                &socketOption,
                basicClientSocket->handle(),
                ntsa::SocketOptionType::e_SEND_BUFFER_SIZE);
            NTCCFG_TEST_FALSE(error);

            NTCCFG_TEST_TRUE(socketOption.isSendBufferSize());

            const bsl::size_t sendBufferSize = socketOption.sendBufferSize();
            NTCCFG_TEST_GE(sendBufferSize, k_MESSAGE_SIZE);
        }

        clientDatagramSocket =
            scheduler->createDatagramSocket(options, allocator);

        error = clientDatagramSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverDatagramSocket =
            scheduler->createDatagramSocket(options, allocator);

        error = serverDatagramSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
    }

    bsl::shared_ptr<rateLimit::DatagramSocketSession> session;
    session.createInplace(allocator, &rateLimitSemaphore);
    clientDatagramSocket->registerSession(session);

    NTCI_LOG_DEBUG("Generating message");
    bsl::shared_ptr<bdlbb::Blob> message =
        clientDatagramSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(message.get(), k_MESSAGE_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Generating message: OK");

    {
        bsl::shared_ptr<ntcs::RateLimiter> limiter;

        // configure sustained rate limit in a way that it will be reached
        // immediately

        const bsl::uint64_t      sustainedRateLimit = k_MESSAGE_SIZE / 2;
        const bsls::TimeInterval sustainedRateWindow(1.0);

        // do not care about peak limit
        const bsl::uint64_t peakRateLimit =
            bsl::numeric_limits<bsl::uint32_t>::max();
        bsls::TimeInterval peakRateWindow;
        peakRateWindow.setTotalSeconds(10);

        limiter.createInplace(allocator,
                              sustainedRateLimit,
                              sustainedRateWindow,
                              peakRateLimit,
                              peakRateWindow,
                              serverDatagramSocket->currentTime());
        clientDatagramSocket->setWriteRateLimiter(limiter);
    }

    NTCI_LOG_DEBUG("Sending messages");
    {
        for (bsl::size_t i = 0; i < k_NUM_MESSAGES; ++i) {
            ntca::SendOptions sendOptions;

            error = clientDatagramSocket->send(*message, sendOptions);
            NTCCFG_TEST_TRUE(!error);
        }
    }

    rateLimitSemaphore.wait();
    rateLimitSemaphore.wait();

    NTCCFG_TEST_GE(session->writeQueueRateLimitApplied(), 1);
    NTCCFG_TEST_GE(session->writeQueueRateLimitRelaxed(), 1);

    ntci::DatagramSocketCloseGuard closeGuardClient(clientDatagramSocket);
    ntci::DatagramSocketCloseGuard closeGuardServer(serverDatagramSocket);
}

void concernStreamSocketReceiveRateLimitTimerEventNotifications(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: validate that e_RATE_LIMIT_APPLIED and e_RATE_LIMIT_RELAXED
    // events are fired

    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Test started");

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_TCP_IPV4_STREAM;

    ntsa::Error      error;
    bslmt::Semaphore receiveSemaphore;
    bslmt::Semaphore rateLimitSemaphore;

    bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntci::StreamSocket> serverStreamSocket;
    {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntsi::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntsi::StreamSocket> basicServerSocket;

        error = ntsf::System::createStreamSocketPair(&basicClientSocket,
                                                     &basicServerSocket,
                                                     transport);
        NTCCFG_TEST_FALSE(error);

        clientStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
    }

    bsl::shared_ptr<rateLimit::StreamSocketSession> session;
    session.createInplace(allocator, &rateLimitSemaphore);
    serverStreamSocket->registerSession(session);

    const int k_MESSAGE_SIZE = 65000;
    const int k_NUM_MESSAGES = 10;

    NTCI_LOG_DEBUG("Generating message");
    bsl::shared_ptr<bdlbb::Blob> message =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(message.get(), k_MESSAGE_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Generating message: OK");

    {
        bsl::shared_ptr<ntcs::RateLimiter> limiter;

        //configure sustained rate limit in a way that it will be reached
        // immediately
        const bsl::uint64_t      sustainedRateLimit = k_MESSAGE_SIZE;
        const bsls::TimeInterval sustainedRateWindow(0.01);

        // do not care about peak limit
        const bsl::uint64_t peakRateLimit =
            bsl::numeric_limits<bsl::uint32_t>::max();
        bsls::TimeInterval peakRateWindow;
        peakRateWindow.setTotalSeconds(1);

        limiter.createInplace(allocator,
                              sustainedRateLimit,
                              sustainedRateWindow,
                              peakRateLimit,
                              peakRateWindow,
                              serverStreamSocket->currentTime());
        serverStreamSocket->setReadRateLimiter(limiter);
    }

    NTCI_LOG_DEBUG("Sending messages");
    {
        for (int i = 0; i < k_NUM_MESSAGES; ++i) {
            ntca::SendOptions sendOptions;

            error = clientStreamSocket->send(*message, sendOptions);
            NTCCFG_TEST_TRUE(!error);
        }
    }

    NTCI_LOG_DEBUG("Receiving messages");
    {
        ntca::ReceiveOptions receiveOptions;
        receiveOptions.setSize(k_MESSAGE_SIZE * k_NUM_MESSAGES);

        ntci::ReceiveCallback receiveCallback =
            serverStreamSocket->createReceiveCallback(
                NTCCFG_BIND(&test::DatagramSocketUtil::processReceiveFailed,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            &receiveSemaphore),
                allocator);

        error = serverStreamSocket->receive(receiveOptions, receiveCallback);
        NTCCFG_TEST_OK(error);
    }

    rateLimitSemaphore.wait();
    rateLimitSemaphore.wait();

    NTCCFG_TEST_GE(session->readQueueRateLimitApplied(), 1);
    NTCCFG_TEST_GE(session->readQueueRateLimitRelaxed(), 1);

    {
        ntci::StreamSocketCloseGuard closeGuardClient(clientStreamSocket);
        ntci::StreamSocketCloseGuard closeGuardServer(serverStreamSocket);
    }

    receiveSemaphore.wait();
}

void concernStreamSocketSendRateLimitTimerEventNotifications(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: validate that e_RATE_LIMIT_APPLIED and e_RATE_LIMIT_RELAXED
    // events are fired

    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Test started");

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_TCP_IPV4_STREAM;

    ntsa::Error      error;
    bslmt::Semaphore rateLimitSemaphore;

    bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket;
    bsl::shared_ptr<ntci::StreamSocket> serverStreamSocket;
    {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntsi::StreamSocket> basicClientSocket;
        bsl::shared_ptr<ntsi::StreamSocket> basicServerSocket;

        error = ntsf::System::createStreamSocketPair(&basicClientSocket,
                                                     &basicServerSocket,
                                                     transport);
        NTCCFG_TEST_FALSE(error);

        clientStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = clientStreamSocket->open(transport, basicClientSocket);
        NTCCFG_TEST_FALSE(error);

        serverStreamSocket = scheduler->createStreamSocket(options, allocator);

        error = serverStreamSocket->open(transport, basicServerSocket);
        NTCCFG_TEST_FALSE(error);
    }

    bsl::shared_ptr<rateLimit::StreamSocketSession> session;
    session.createInplace(allocator, &rateLimitSemaphore);
    clientStreamSocket->registerSession(session);

    const int k_MESSAGE_SIZE = 65000;
    const int k_NUM_MESSAGES = 10;

    NTCI_LOG_DEBUG("Generating message");
    bsl::shared_ptr<bdlbb::Blob> message =
        clientStreamSocket->createOutgoingBlob();
    ntcd::DataUtil::generateData(message.get(), k_MESSAGE_SIZE, 0, 0);

    NTCI_LOG_DEBUG("Generating message: OK");

    {
        bsl::shared_ptr<ntcs::RateLimiter> limiter;

        //configure sustained rate limit in a way that it will be reached
        // immediately
        const bsl::uint64_t      sustainedRateLimit = k_MESSAGE_SIZE / 2;
        const bsls::TimeInterval sustainedRateWindow(1.0);

        // do not care about peak limit
        const bsl::uint64_t peakRateLimit =
            bsl::numeric_limits<bsl::uint32_t>::max();
        bsls::TimeInterval peakRateWindow;
        peakRateWindow.setTotalSeconds(1);

        limiter.createInplace(allocator,
                              sustainedRateLimit,
                              sustainedRateWindow,
                              peakRateLimit,
                              peakRateWindow,
                              serverStreamSocket->currentTime());
        clientStreamSocket->setWriteRateLimiter(limiter);
    }

    NTCI_LOG_DEBUG("Sending messages");
    {
        for (int i = 0; i < k_NUM_MESSAGES; ++i) {
            ntca::SendOptions sendOptions;

            error = clientStreamSocket->send(*message, sendOptions);
            NTCCFG_TEST_TRUE(!error);
        }
    }

    rateLimitSemaphore.wait();
    rateLimitSemaphore.wait();

    NTCCFG_TEST_GE(session->writeQueueRateLimitApplied(), 1);
    NTCCFG_TEST_GE(session->writeQueueRateLimitRelaxed(), 1);

    {
        ntci::StreamSocketCloseGuard closeGuardClient(clientStreamSocket);
        ntci::StreamSocketCloseGuard closeGuardServer(serverStreamSocket);
    }
}

void concernListenerSocketAcceptRateLimitTimerEventNotifications(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: validate that e_RATE_LIMIT_APPLIED and e_RATE_LIMIT_RELAXED
    // events are fired

    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Test started");

    const ntsa::Transport::Value transport =
        ntsa::Transport::e_TCP_IPV4_STREAM;

    ntsa::Error      error;
    bslmt::Semaphore acceptLimitSemaphore;

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket;
    {
        ntca::ListenerSocketOptions options;
        options.setTransport(transport);
        options.setSourceEndpoint(test::EndpointUtil::any(transport));

        listenerSocket = scheduler->createListenerSocket(options, allocator);
    }

    bsl::shared_ptr<rateLimit::ListenerSocketSession> session;
    session.createInplace(allocator, &acceptLimitSemaphore);
    listenerSocket->registerSession(session);

    {
        bsl::shared_ptr<ntcs::RateLimiter> limiter;

        //configure sustained rate limit in a way that it will be reached
        // immediately. e.g. 1 acceptance per second
        const bsl::uint64_t sustainedRateLimit = 1;
        bsls::TimeInterval  sustainedRateWindow;
        sustainedRateWindow.setTotalHours(1);

        // do not care about peak limit
        const bsl::uint64_t peakRateLimit = 1;
        bsls::TimeInterval  peakRateWindow;
        peakRateWindow.setTotalSeconds(1);

        limiter.createInplace(allocator,
                              sustainedRateLimit,
                              sustainedRateWindow,
                              peakRateLimit,
                              peakRateWindow,
                              listenerSocket->currentTime());
        listenerSocket->setAcceptRateLimiter(limiter);
    }

    error = listenerSocket->open();
    NTCCFG_TEST_FALSE(error);

    error = listenerSocket->listen();
    NTCCFG_TEST_FALSE(error);

    NTCI_LOG_DEBUG("Listener socket starts listening");

    const int numSockets = 10;

    bsl::vector<bsl::shared_ptr<ntci::StreamSocket> > clientSockets(allocator);
    bsl::vector<bsl::shared_ptr<ntci::ConnectFuture> > connectFutures(
        allocator);
    bsl::vector<bsl::shared_ptr<ntci::AcceptFuture> > acceptFutures(allocator);

    NTCI_LOG_DEBUG("Creating client sockets");

    for (int i = 0; i < numSockets; ++i) {
        ntca::StreamSocketOptions options;
        options.setTransport(transport);

        bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket =
            scheduler->createStreamSocket(options, allocator);

        error = clientStreamSocket->open();
        NTCCFG_TEST_OK(error);
        clientSockets.push_back(clientStreamSocket);
    }

    NTCI_LOG_DEBUG("Preparing futures");
    for (int i = 0; i < numSockets; ++i) {
        bsl::shared_ptr<ntci::ConnectFuture> connectFuture;
        connectFuture.createInplace(allocator);
        connectFutures.push_back(connectFuture);

        bsl::shared_ptr<ntci::AcceptFuture> acceptFuture;
        acceptFuture.createInplace(allocator);
        acceptFutures.push_back(acceptFuture);
    }

    NTCI_LOG_DEBUG("Accepting connections");
    for (int i = 0; i < numSockets; ++i) {
        error =
            listenerSocket->accept(ntca::AcceptOptions(), *(acceptFutures[i]));
        NTCCFG_TEST_OK(error);
    }

    NTCI_LOG_DEBUG("Initiating connections");
    for (int i = 0; i < numSockets; ++i) {
        error = clientSockets[i]->connect(listenerSocket->sourceEndpoint(),
                                          ntca::ConnectOptions(),
                                          *(connectFutures[i]));
        NTCCFG_TEST_OK(error);
    }

    acceptLimitSemaphore.wait();
    {
        {
            ntci::ListenerSocketCloseGuard listenerSocketCloseGuard(
                listenerSocket);
        }
        listenerSocket.reset();
    }

    NTCI_LOG_DEBUG("Cleaning accepted connections");
    for (size_t i = 0; i < acceptFutures.size(); ++i) {
        const bsls::TimeInterval zero;
        ntci::AcceptResult       acceptResult;
        error = acceptFutures[i]->wait(&acceptResult, zero);
        if (!error) {
            ntci::StreamSocketCloseGuard closeGuard(
                acceptResult.streamSocket());
        }
    }

    for (size_t i = 0; i < clientSockets.size(); ++i) {
        ntci::StreamSocketCloseGuard closeGuard(clientSockets[i]);
    }
}

void distributedFunction(bslmt::Barrier* suspendBarrier,
                         bslmt::Barrier* releaseBarrier,
                         bsl::size_t     threadIndex)
{
    NTCCFG_WARNING_UNUSED(suspendBarrier);
    NTCCFG_WARNING_UNUSED(releaseBarrier);
    NTCCFG_WARNING_UNUSED(threadIndex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_INFO("Thread %zu enter", threadIndex);

    suspendBarrier->wait();

    NTCI_LOG_INFO("Thread %zu leave", threadIndex);

    releaseBarrier->wait();
}

void distributedTimer(bslmt::Barrier*                     suspendBarrier,
                      bslmt::Barrier*                     releaseBarrier,
                      const bsl::shared_ptr<ntci::Timer>& timer,
                      const ntca::TimerEvent&             event,
                      bsl::size_t                         threadIndex)
{
    NTCCFG_WARNING_UNUSED(suspendBarrier);
    NTCCFG_WARNING_UNUSED(releaseBarrier);
    NTCCFG_WARNING_UNUSED(timer);
    NTCCFG_WARNING_UNUSED(event);
    NTCCFG_WARNING_UNUSED(threadIndex);
}

void concernInterfaceFunctionAndTimerDistribution(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    bslma::Allocator*                       allocator)
{
    // Concern: Functions and timers deferred/scheduled at the scheduler/level
    // are distributed amongst threads, so one thread blocked on processing
    // a function/timer does not prevent another thread from processing another
    // function/timer that is due, regardless of whether static or dynamic
    // load balancing is configured.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("Test started");

    // Get the scheduler configuration.

    const ntca::SchedulerConfig& schedulerConfig = scheduler->configuration();

    const bsl::size_t minThreads = schedulerConfig.minThreads();
    const bsl::size_t maxThreads = schedulerConfig.maxThreads();

    // This test assumes that there are a fixed number of threads run by 
    // the scheduler.

    NTCCFG_TEST_EQ(minThreads, maxThreads);

    const bsl::size_t numThreads = maxThreads;
    if (numThreads > 1) {
        bslmt::Barrier suspendBarrier(numThreads + 1);
        bslmt::Barrier releaseBarrier(numThreads + 1);

        for (bsl::size_t threadIndex = 0; 
                         threadIndex < numThreads; 
                       ++threadIndex) 
        {
            scheduler->execute(
                NTCCFG_BIND(&distributedFunction,
                            &suspendBarrier,
                            &releaseBarrier,
                            threadIndex));
        }

        suspendBarrier.wait();
        releaseBarrier.wait();
    }

    NTCI_LOG_DEBUG("Test complete");
}

}  // close namespace 'test'

NTCCFG_TEST_CASE(1)
{
    bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_INFO);

    ntccfg::TestAllocator ta;
    {
        test::concernExample1(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    ntccfg::TestAllocator ta;
    {
        test::concernExample2(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    ntccfg::TestAllocator ta;
    {
        test::concernExample3(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    ntccfg::TestAllocator ta;
    {
        test::concernExample4(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(5)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    ntccfg::TestAllocator ta;
    {
        test::concernResolverGetIpAddress(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(6)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    ntccfg::TestAllocator ta;
    {
        test::concernResolverGetDomainName(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(7)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    ntccfg::TestAllocator ta;
    {
        test::concernResolverGetPort(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(8)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    ntccfg::TestAllocator ta;
    {
        test::concernResolverGetServiceName(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(9)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    ntccfg::TestAllocator ta;
    {
        test::concernResolverGetEndpoint(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(10)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernDataExchange, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(11)
{
    ntccfg::TestAllocator ta;
    {
        test::concernClose(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(12)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernConnectAndShutdown, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(13)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernConnectEndpoint1, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(14)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernConnectEndpoint2, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(15)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernConnectEndpoint3, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(16)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernConnectEndpoint4, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(17)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernConnectEndpoint5, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(18)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernConnectEndpoint6, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(19)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernConnectEndpoint7, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(20)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernConnectEndpoint8, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(21)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernConnectName1, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(22)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernConnectName2, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(23)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernConnectName3, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(24)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernConnectName4, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(25)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernConnectName5, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(26)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernConnectName6, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(27)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernConnectName7, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(28)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernConnectName8, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(29)
{
    ntccfg::TestAllocator ta;
    {
        test::concernConnectLimitActive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(30)
{
    ntccfg::TestAllocator ta;
    {
        test::concernConnectLimitPassive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(31)
{
    ntccfg::TestAllocator ta;
    {
        test::concernAcceptClosure(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(32)
{
    // Empty
}

NTCCFG_TEST_CASE(33)
{
    ntccfg::TestAllocator ta;
    {
        test::concernDatagramSocketBasicReactive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(34)
{
    ntccfg::TestAllocator ta;
    {
        test::concernDatagramSocketBasicProactive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(35)
{
    ntccfg::TestAllocator ta;
    {
        test::concernDatagramSocketStressReactive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(36)
{
    ntccfg::TestAllocator ta;
    {
        test::concernDatagramSocketStressProactive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(37)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernDatagramSocketReceiveDeadline, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(38)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernDatagramSocketReceiveCancellation, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(39)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernListenerSocketAcceptDeadline, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(40)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernListenerSocketAcceptCancellation, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(41)
{
    // Empty
}

NTCCFG_TEST_CASE(42)
{
    // Empty
}

NTCCFG_TEST_CASE(43)
{
    ntccfg::TestAllocator ta;
    {
        test::concernStreamSocketBasicReactive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(44)
{
    ntccfg::TestAllocator ta;
    {
        test::concernStreamSocketBasicProactive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(45)
{
    ntccfg::TestAllocator ta;
    {
        test::concernStreamSocketMinimalReadQueueHighWatermarkReactive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(46)
{
    ntccfg::TestAllocator ta;
    {
        test::concernStreamSocketMinimalReadQueueHighWatermarkProactive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(47)
{
    ntccfg::TestAllocator ta;
    {
        test::concernStreamSocketMinimalWriteQueueHighWatermarkReactive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(48)
{
    ntccfg::TestAllocator ta;
    {
        test::concernStreamSocketMinimalWriteQueueHighWatermarkProactive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(49)
{
    ntccfg::TestAllocator ta;
    {
        test::concernStreamSocketRateLimitReceiveBufferReactive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(50)
{
    ntccfg::TestAllocator ta;
    {
        test::concernStreamSocketRateLimitReceiveBufferProactive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(51)
{
    ntccfg::TestAllocator ta;
    {
        test::concernStreamSocketRateLimitSendBufferReactive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(52)
{
    ntccfg::TestAllocator ta;
    {
        test::concernStreamSocketRateLimitSendBufferProactive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(53)
{
    ntccfg::TestAllocator ta;
    {
        test::concernStreamSocketStressReactive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(54)
{
    ntccfg::TestAllocator ta;
    {
        test::concernStreamSocketStressProactive(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(55)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernStreamSocketReceiveDeadline, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(56)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernStreamSocketReceiveCancellation, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(57)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernStreamSocketSendDeadline, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(58)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernStreamSocketSendCancellation, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(59)
{
    ntccfg::TestAllocator ta;
    {
        test::concernInterfaceResolverGetIpAddressFromOverride(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(60)
{
    ntccfg::TestAllocator ta;
    {
        test::concernInterfaceResolverGetIpAddressFromServer(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(61)
{
    ntccfg::TestAllocator ta;
    {
        test::concernInterfaceResolverGetIpAddressFromSystem(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(62)
{
    // Concern: Default executors.

    ntccfg::TestAllocator ta;
    {
        bsl::shared_ptr<ntci::Executor> executor;
        ntcf::System::getDefault(&executor);

        bsl::size_t numReferences = executor.use_count();
        NTCCFG_TEST_EQ(numReferences, 2);

        bslmt::Latch latch(1);
        executor->execute(NTCCFG_BIND(&bslmt::Latch::arrive, &latch));
        latch.wait();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(63)
{
    // Concern: Default interfaces.

    ntccfg::TestAllocator ta;
    {
        bsl::shared_ptr<ntci::Scheduler> scheduler;
        ntcf::System::getDefault(&scheduler);

        bslmt::Latch latch(1);
        scheduler->execute(NTCCFG_BIND(&bslmt::Latch::arrive, &latch));
        latch.wait();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

namespace case64 {

class StreamSocketSession : public ntci::StreamSocketSession
{
    bsl::string         d_name;
    ntci::CloseCallback d_closeCallback;
    bslma::Allocator*   d_allocator_p;

  private:
    StreamSocketSession(const StreamSocketSession&);
    StreamSocketSession& operator=(const StreamSocketSession&);

  public:
    explicit StreamSocketSession(const bsl::string&         name,
                                 const ntci::CloseCallback& closeCallback,
                                 bslma::Allocator* basicAllocator = 0);

    ~StreamSocketSession() BSLS_KEYWORD_OVERRIDE;

    void processShutdownInitiated(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    void processShutdownComplete(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;
};

StreamSocketSession::StreamSocketSession(
    const bsl::string&         name,
    const ntci::CloseCallback& closeCallback,
    bslma::Allocator*          basicAllocator)
: d_name(name, basicAllocator)
, d_closeCallback(closeCallback, basicAllocator)
, d_allocator_p(basicAllocator)
{
}

StreamSocketSession::~StreamSocketSession()
{
}

void StreamSocketSession::processShutdownInitiated(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_INFO("%s shutdown initiated", d_name.c_str());
}

void StreamSocketSession::processShutdownComplete(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_INFO("%s shutdown complete", d_name.c_str());

    NTCCFG_TEST_EQ(event.type(), ntca::ShutdownEventType::e_COMPLETE);

    streamSocket->close(d_closeCallback);
    streamSocket->close(d_closeCallback);
}

class ListenerSocketSession : public ntci::ListenerSocketSession
{
    bsl::string         d_name;
    ntci::CloseCallback d_closeCallback;
    bslma::Allocator*   d_allocator_p;

  private:
    ListenerSocketSession(const ListenerSocketSession&);
    ListenerSocketSession& operator=(const ListenerSocketSession&);

  public:
    explicit ListenerSocketSession(const bsl::string&         name,
                                   const ntci::CloseCallback& closeCallback,
                                   bslma::Allocator* basicAllocator = 0);

    ~ListenerSocketSession() BSLS_KEYWORD_OVERRIDE;

    void processShutdownInitiated(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    void processShutdownComplete(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;
};

ListenerSocketSession::ListenerSocketSession(
    const bsl::string&         name,
    const ntci::CloseCallback& closeCallback,
    bslma::Allocator*          basicAllocator)
: d_name(name, basicAllocator)
, d_closeCallback(closeCallback, basicAllocator)
, d_allocator_p(basicAllocator)
{
}

ListenerSocketSession::~ListenerSocketSession()
{
}

void ListenerSocketSession::processShutdownInitiated(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_INFO("%s shutdown initiated", d_name.c_str());
}

void ListenerSocketSession::processShutdownComplete(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_INFO("%s shutdown complete", d_name.c_str());

    NTCCFG_TEST_EQ(event.type(), ntca::ShutdownEventType::e_COMPLETE);

    listenerSocket->close(d_closeCallback);
    listenerSocket->close(d_closeCallback);
}

void processClosed(bslmt::Semaphore* semaphore, const char* msg)
{
    NTCI_LOG_CONTEXT();
    if (msg) {
        NTCI_LOG_INFO("%s", msg);
    }
    semaphore->post();
}

void processBarrier(bslmt::Barrier* barrier)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_INFO("I/O thread suspended");

    barrier->wait();

    NTCI_LOG_INFO("I/O thread released");
}

}  // close namespace case64

NTCCFG_TEST_CASE(64)
{
    // Concern: Any asynchronous close operation results in its callback
    // invoked, regardless of whether the close operation is initiated from
    // the main thread or from within the I/O thread during the shutdown
    // sequence, even when the socket is closed multiple times (i.e. it is
    // safe for the user to close the socket multiple times; the first close
    // operation begins the shutdown sequence, and all close callbacks are
    // invoked after the shutdown sequence completes.)

    ntccfg::TestAllocator ta;
    {
        NTCI_LOG_CONTEXT();

        ntsa::Error error;

        ntca::SchedulerConfig schedulerConfig;
        schedulerConfig.setThreadName("test");
        schedulerConfig.setMinThreads(1);
        schedulerConfig.setMaxThreads(1);

        bsl::shared_ptr<ntci::Scheduler> scheduler =
            ntcf::System::createScheduler(schedulerConfig, &ta);

        ntci::SchedulerStopGuard schedulerGuard(scheduler);

        error = scheduler->start();
        NTCCFG_TEST_OK(error);

        ntca::ListenerSocketOptions listenerSocketOptions;

        listenerSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
        listenerSocketOptions.setBacklog(1);
        listenerSocketOptions.setReuseAddress(true);

        listenerSocketOptions.setSourceEndpoint(ntsa::Endpoint(
            ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)));

        bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
            scheduler->createListenerSocket(listenerSocketOptions, &ta);

        ntci::ListenerSocketCloseGuard listenerGuard(listenerSocket);

        error = listenerSocket->open();
        NTCCFG_TEST_OK(error);

        error = listenerSocket->listen();
        NTCCFG_TEST_OK(error);

        error =
            listenerSocket->relaxFlowControl(ntca::FlowControlType::e_RECEIVE);
        NTCCFG_TEST_OK(error);

        ntca::StreamSocketOptions clientStreamSocketOptions;
        clientStreamSocketOptions.setTransport(
            ntsa::Transport::e_TCP_IPV4_STREAM);

        bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket =
            scheduler->createStreamSocket(clientStreamSocketOptions, &ta);

        ntci::ConnectFuture connectFuture;
        error = clientStreamSocket->connect(listenerSocket->sourceEndpoint(),
                                            ntca::ConnectOptions(),
                                            connectFuture);
        NTCCFG_TEST_OK(error);

        ntci::ConnectResult connectResult;
        error = connectFuture.wait(&connectResult);
        NTCCFG_TEST_OK(error);
        NTCCFG_TEST_EQ(connectResult.event().type(),
                       ntca::ConnectEventType::e_COMPLETE);

        ntci::AcceptFuture acceptFuture;
        error = listenerSocket->accept(ntca::AcceptOptions(), acceptFuture);
        NTCCFG_TEST_OK(error);

        ntci::AcceptResult acceptResult;
        error = acceptFuture.wait(&acceptResult);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_EQ(acceptResult.event().type(),
                       ntca::AcceptEventType::e_COMPLETE);

        bsl::shared_ptr<ntci::StreamSocket> serverStreamSocket =
            acceptResult.streamSocket();

        ntci::StreamSocketCloseGuard clientStreamSocketGuard(
            clientStreamSocket);

        ntci::StreamSocketCloseGuard serverStreamSocketGuard(
            serverStreamSocket);

        bslmt::Semaphore clientInternalCloseSemaphore;
        bslmt::Semaphore serverInternalCloseSemaphore;
        bslmt::Semaphore listenerInternalCloseSemaphore;

        bslmt::Semaphore clientExternalCloseSemaphore;
        bslmt::Semaphore serverExternalCloseSemaphore;
        bslmt::Semaphore listenerExternalCloseSemaphore;

        ntci::CloseCallback clientInternalCloseCallback =
            clientStreamSocket->createCloseCallback(
                NTCCFG_BIND(&case64::processClosed,
                            &clientInternalCloseSemaphore,
                            "Client closed internally from I/O thread"),
                &ta);

        ntci::CloseCallback serverInternalCloseCallback =
            serverStreamSocket->createCloseCallback(
                NTCCFG_BIND(&case64::processClosed,
                            &serverInternalCloseSemaphore,
                            "Server closed internally from I/O thread"),
                &ta);

        ntci::CloseCallback listenerInternalCloseCallback =
            listenerSocket->createCloseCallback(
                NTCCFG_BIND(
                    &case64::processClosed,
                    &listenerInternalCloseSemaphore,
                    "Server (listener) closed internally from I/O thread"),
                &ta);

        ntci::CloseCallback clientExternalCloseCallback =
            clientStreamSocket->createCloseCallback(
                NTCCFG_BIND(&case64::processClosed,
                            &clientExternalCloseSemaphore,
                            "Client closed externally from main thread"),
                &ta);

        ntci::CloseCallback serverExternalCloseCallback =
            serverStreamSocket->createCloseCallback(
                NTCCFG_BIND(&case64::processClosed,
                            &serverExternalCloseSemaphore,
                            "Server closed externally from main thread"),
                &ta);

        ntci::CloseCallback listenerExternalCloseCallback =
            listenerSocket->createCloseCallback(
                NTCCFG_BIND(
                    &case64::processClosed,
                    &listenerExternalCloseSemaphore,
                    "Listener socket closed externally from main thread"),
                &ta);

        bsl::shared_ptr<case64::StreamSocketSession> clientStreamSocketSession;
        clientStreamSocketSession.createInplace(&ta,
                                                "Client",
                                                clientInternalCloseCallback,
                                                &ta);

        bsl::shared_ptr<case64::StreamSocketSession> serverStreamSocketSession;
        serverStreamSocketSession.createInplace(&ta,
                                                "ServerStream",
                                                serverInternalCloseCallback,
                                                &ta);

        bsl::shared_ptr<case64::ListenerSocketSession> listenerSocketSession;
        listenerSocketSession.createInplace(&ta,
                                            "ServerListener",
                                            listenerInternalCloseCallback,
                                            &ta);

        error = clientStreamSocket->registerSession(clientStreamSocketSession);
        NTCCFG_TEST_OK(error);

        error = serverStreamSocket->registerSession(serverStreamSocketSession);
        NTCCFG_TEST_OK(error);

        error = listenerSocket->registerSession(listenerSocketSession);
        NTCCFG_TEST_OK(error);

        error = clientStreamSocket->relaxFlowControl(
            ntca::FlowControlType::e_RECEIVE);
        NTCCFG_TEST_OK(error);

        error = serverStreamSocket->relaxFlowControl(
            ntca::FlowControlType::e_RECEIVE);
        NTCCFG_TEST_OK(error);

        bslmt::Barrier barrier(2);
        scheduler->execute(NTCCFG_BIND(&case64::processBarrier, &barrier));

        NTCI_LOG_INFO("Closing client");

        clientStreamSocket->close(clientExternalCloseCallback);
        clientStreamSocket->close(clientExternalCloseCallback);

        NTCI_LOG_INFO("Closing server");

        serverStreamSocket->close(serverExternalCloseCallback);
        serverStreamSocket->close(serverExternalCloseCallback);

        NTCI_LOG_INFO("Closing server (listener)");

        listenerSocket->close(listenerExternalCloseCallback);
        listenerSocket->close(listenerExternalCloseCallback);

        barrier.wait();

        clientInternalCloseSemaphore.wait();
        clientInternalCloseSemaphore.wait();

        clientExternalCloseSemaphore.wait();
        clientExternalCloseSemaphore.wait();

        serverInternalCloseSemaphore.wait();
        serverInternalCloseSemaphore.wait();

        serverExternalCloseSemaphore.wait();
        serverExternalCloseSemaphore.wait();

        listenerInternalCloseSemaphore.wait();
        listenerInternalCloseSemaphore.wait();

        listenerExternalCloseSemaphore.wait();
        listenerExternalCloseSemaphore.wait();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

namespace case65 {

class DatagramSocketSession : public ntci::DatagramSocketSession
{
    bsl::string         d_name;
    ntci::CloseCallback d_closeCallback;
    bslma::Allocator*   d_allocator_p;

  private:
    DatagramSocketSession(const DatagramSocketSession&);
    DatagramSocketSession& operator=(const DatagramSocketSession&);

  public:
    explicit DatagramSocketSession(const bsl::string&         name,
                                   const ntci::CloseCallback& closeCallback,
                                   bslma::Allocator* basicAllocator = 0);

    ~DatagramSocketSession() BSLS_KEYWORD_OVERRIDE;

    void processShutdownInitiated(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    void processShutdownComplete(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;
};

DatagramSocketSession::DatagramSocketSession(
    const bsl::string&         name,
    const ntci::CloseCallback& closeCallback,
    bslma::Allocator*          basicAllocator)
: d_name(name, basicAllocator)
, d_closeCallback(closeCallback, basicAllocator)
, d_allocator_p(basicAllocator)
{
}

DatagramSocketSession::~DatagramSocketSession()
{
}

void DatagramSocketSession::processShutdownInitiated(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_INFO("%s shutdown initiated", d_name.c_str());
}

void DatagramSocketSession::processShutdownComplete(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_INFO("%s shutdown complete", d_name.c_str());

    NTCCFG_TEST_EQ(event.type(), ntca::ShutdownEventType::e_COMPLETE);

    datagramSocket->close(d_closeCallback);
    datagramSocket->close(d_closeCallback);
}

void processCloseCb(bslmt::Semaphore* semaphore)
{
    semaphore->post();
}

}  // close namespace case65

NTCCFG_TEST_CASE(65)
{
    ntccfg::TestAllocator ta;
    {
        NTCI_LOG_CONTEXT();

        ntsa::Error error;

        ntca::SchedulerConfig schedulerConfig;
        schedulerConfig.setThreadName("test");
        schedulerConfig.setMinThreads(1);
        schedulerConfig.setMaxThreads(1);

        bsl::shared_ptr<ntci::Scheduler> scheduler =
            ntcf::System::createScheduler(schedulerConfig, &ta);

        ntci::SchedulerStopGuard schedulerGuard(scheduler);

        error = scheduler->start();
        NTCCFG_TEST_OK(error);

        ntca::DatagramSocketOptions datagramSocketOptions;
        datagramSocketOptions.setTransport(
            ntsa::Transport::e_UDP_IPV4_DATAGRAM);

        bsl::shared_ptr<ntci::DatagramSocket> datagramSocket =
            scheduler->createDatagramSocket(datagramSocketOptions, &ta);

        ntci::DatagramSocketCloseGuard closeGuard(datagramSocket);

        error = datagramSocket->open();
        NTCCFG_TEST_OK(error);

        bslmt::Semaphore externalCloseSemaphore;
        bslmt::Semaphore internalCloseSemaphore;

        ntci::CloseCallback externalCloseCallback =
            datagramSocket->createCloseCallback(
                NTCCFG_BIND(case65::processCloseCb, &externalCloseSemaphore),
                &ta);

        ntci::CloseCallback internalCloseCallback =
            datagramSocket->createCloseCallback(
                NTCCFG_BIND(case65::processCloseCb, &internalCloseSemaphore),
                &ta);

        bsl::shared_ptr<case65::DatagramSocketSession> datagramSocketSession;
        datagramSocketSession.createInplace(&ta,
                                            "Session",
                                            internalCloseCallback,
                                            &ta);

        datagramSocket->registerSession(datagramSocketSession);

        bslmt::Barrier barrier(2);
        scheduler->execute(NTCCFG_BIND(&case64::processBarrier, &barrier));

        datagramSocket->close(externalCloseCallback);
        datagramSocket->close(externalCloseCallback);

        barrier.wait();

        externalCloseSemaphore.wait();
        externalCloseSemaphore.wait();

        internalCloseSemaphore.wait();
        internalCloseSemaphore.wait();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(66)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernDatagramSocketReceiveDeadlineClose, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(67)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernStreamSocketReceiveDeadlineClose, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(68)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernStreamSocketSendDeadlineClose, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(69)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernListenerSocketAcceptClose, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(70)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(
            &test::
                concernDatagramSocketReceiveRateLimitTimerEventNotifications,
            &ta);
        NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
    }
}

NTCCFG_TEST_CASE(71)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(
            &test::concernStreamSocketReceiveRateLimitTimerEventNotifications,
            &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(72)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(
            &test::concernDatagramSocketSendRateLimitTimerEventNotifications,
            &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(73)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(
            &test::concernStreamSocketSendRateLimitTimerEventNotifications,
            &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(74)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(
            &test::concernListenerSocketAcceptRateLimitTimerEventNotifications,
            &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(75)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernStreamSocketConnectRetryTimerClose, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(76)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernStreamSocketConnectDeadlineTimerClose,
                      &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(77)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernDatagramSocketReceiveRateLimitTimerClose,
                      &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(78)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernDatagramSocketSendRateLimitTimerClose,
                      &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(79)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernStreamSocketReceiveRateLimitTimerClose,
                      &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(80)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernStreamSocketSendRateLimitTimerClose, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(81)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernListenerSocketAcceptRateLimitTimerClose,
                      &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(82)
{
    ntccfg::TestAllocator ta;
    {
        test::concern(&test::concernInterfaceFunctionAndTimerDistribution,
                      &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
    NTCCFG_TEST_REGISTER(5);
    NTCCFG_TEST_REGISTER(6);
    NTCCFG_TEST_REGISTER(7);
    NTCCFG_TEST_REGISTER(8);
    NTCCFG_TEST_REGISTER(9);
    NTCCFG_TEST_REGISTER(10);
    NTCCFG_TEST_REGISTER(11);
    NTCCFG_TEST_REGISTER(12);
    NTCCFG_TEST_REGISTER(13);
    NTCCFG_TEST_REGISTER(14);
    NTCCFG_TEST_REGISTER(15);
    NTCCFG_TEST_REGISTER(16);
    NTCCFG_TEST_REGISTER(17);
    NTCCFG_TEST_REGISTER(18);
    NTCCFG_TEST_REGISTER(19);
    NTCCFG_TEST_REGISTER(20);
    NTCCFG_TEST_REGISTER(21);
    NTCCFG_TEST_REGISTER(22);
    NTCCFG_TEST_REGISTER(23);
    NTCCFG_TEST_REGISTER(24);
    NTCCFG_TEST_REGISTER(25);
    NTCCFG_TEST_REGISTER(26);
    NTCCFG_TEST_REGISTER(27);
    NTCCFG_TEST_REGISTER(28);
    NTCCFG_TEST_REGISTER(29);
    NTCCFG_TEST_REGISTER(30);
    NTCCFG_TEST_REGISTER(31);
    NTCCFG_TEST_REGISTER(32);
    NTCCFG_TEST_REGISTER(33);
    NTCCFG_TEST_REGISTER(34);
    NTCCFG_TEST_REGISTER(35);
    NTCCFG_TEST_REGISTER(36);
    NTCCFG_TEST_REGISTER(37);
    NTCCFG_TEST_REGISTER(38);
    NTCCFG_TEST_REGISTER(39);
    NTCCFG_TEST_REGISTER(40);
    NTCCFG_TEST_REGISTER(41);
    NTCCFG_TEST_REGISTER(42);
    NTCCFG_TEST_REGISTER(43);
    NTCCFG_TEST_REGISTER(44);
    NTCCFG_TEST_REGISTER(45);
    NTCCFG_TEST_REGISTER(46);
    NTCCFG_TEST_REGISTER(47);
    NTCCFG_TEST_REGISTER(48);
    NTCCFG_TEST_REGISTER(49);
    NTCCFG_TEST_REGISTER(50);
    NTCCFG_TEST_REGISTER(51);
    NTCCFG_TEST_REGISTER(52);
    NTCCFG_TEST_REGISTER(53);
    NTCCFG_TEST_REGISTER(54);
    NTCCFG_TEST_REGISTER(55);
    NTCCFG_TEST_REGISTER(56);
    NTCCFG_TEST_REGISTER(57);
    NTCCFG_TEST_REGISTER(58);
    NTCCFG_TEST_REGISTER(59);
    NTCCFG_TEST_REGISTER(60);
    NTCCFG_TEST_REGISTER(61);
    NTCCFG_TEST_REGISTER(62);
    NTCCFG_TEST_REGISTER(63);
    NTCCFG_TEST_REGISTER(64);
    NTCCFG_TEST_REGISTER(65);
    NTCCFG_TEST_REGISTER(66);
    NTCCFG_TEST_REGISTER(67);
    NTCCFG_TEST_REGISTER(68);
    NTCCFG_TEST_REGISTER(69);
    NTCCFG_TEST_REGISTER(70);
    NTCCFG_TEST_REGISTER(71);
    NTCCFG_TEST_REGISTER(72);
    NTCCFG_TEST_REGISTER(73);
    NTCCFG_TEST_REGISTER(74);
    NTCCFG_TEST_REGISTER(75);
    NTCCFG_TEST_REGISTER(76);
    NTCCFG_TEST_REGISTER(77);
    NTCCFG_TEST_REGISTER(78);
    NTCCFG_TEST_REGISTER(79);
    NTCCFG_TEST_REGISTER(80);
    NTCCFG_TEST_REGISTER(81);
    NTCCFG_TEST_REGISTER(82);
}
NTCCFG_TEST_DRIVER_END;
