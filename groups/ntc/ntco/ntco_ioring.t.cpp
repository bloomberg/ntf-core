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
BSLS_IDENT_RCSID(ntco_ioring_t_cpp, "$Id$ $CSID$")

#include <ntco_ioring.h>

#include <ntccfg_bind.h>
#include <ntci_log.h>
#include <ntci_proactor.h>
#include <ntci_proactorsocket.h>
#include <ntsf_system.h>
#include <ntco_test.h>

using namespace BloombergLP;

#if NTC_BUILD_WITH_IORING

// Uncomment to enable testing of shutting down reads.
//
// IMPLEMENTATION NOTE: Some read operating systems or third-party
// implementations of the proactor model never fail nor complete a read
// initiated after the socket has been announced to have been shutdown, so this
// test cannot pass without some sort of emulation of the desired behavior.
#define NTCO_IORING_TEST_SHUTDOWN_READ 0

// Uncomment to enable testing of shutting down writes.
#define NTCO_IORING_TEST_SHUTDOWN_WRITE 1

#define NTCO_IORING_TEST_LOG_OPERATION(test, operationDescription, id)        \
    do {                                                                      \
        if (NTSCFG_TEST_VERBOSITY >= 3) {                                     \
            BSLS_LOG_INFO("%s: ID %zu"                                        \
                          "\n    Submission queue head: %zu"                  \
                          "\n    Submission queue tail: %zu"                  \
                          "\n    Completion queue head: %zu"                  \
                          "\n    Completion queue tail: %zu",                 \
                          operationDescription,                               \
                          (bsl::size_t)(id),                                  \
                          (bsl::size_t)((test)->submissionQueueHead()),       \
                          (bsl::size_t)((test)->submissionQueueTail()),       \
                          (bsl::size_t)((test)->completionQueueHead()),       \
                          (bsl::size_t)((test)->completionQueueTail()));      \
        }                                                                     \
    } while (false)

#define NTCO_IORING_TEST_LOG_PUSH_STARTING(test, id)                          \
    NTCO_IORING_TEST_LOG_OPERATION(test, "Push starting", id)

#define NTCO_IORING_TEST_LOG_PUSH_COMPLETE(test, id)                          \
    NTCO_IORING_TEST_LOG_OPERATION(test, "Push complete", id)

#define NTCO_IORING_TEST_LOG_POPPED(test, id)                                 \
    NTCO_IORING_TEST_LOG_OPERATION(test, "Popped", id)

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntco::IoRing'.
class IoRingTest
{
    class ProactorStreamSocket;
    class ProactorListenerSocket;
    class TimerSession;

    static void processFunction(bslmt::Latch* latch);

  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifyCase3();

    // TODO
    static void verifyCase4();
};

class IoRingTest::ProactorStreamSocket : public ntci::ProactorSocket,
                             public ntccfg::Shared<ProactorStreamSocket>
{
    // Provide an implementation of a proactor socket for use by
    // this test driver.

  public:
    typedef NTCCFG_FUNCTION() ConnectCallback;
    // Define a type alias for a function invoked when the
    // socket has established a connection to its peer.

    typedef NTCCFG_FUNCTION(const bsl::shared_ptr<bdlbb::Blob>& data,
                            bsl::size_t numBytesSent) SendCallback;
    // Define a type alias for a function invoked when the
    // socket has copied data to the socket send buffer.

    typedef NTCCFG_FUNCTION(const bsl::shared_ptr<bdlbb::Blob>& data,
                            bsl::size_t numBytesReceived) ReceiveCallback;
    // Define a type alias for a function invoked when the
    // socket has copied data from the socket send buffer.

    typedef NTCCFG_FUNCTION() ShutdownCallback;
    // Define a type alias for a function invoked when the
    // socket has been shutdown.

    typedef NTCCFG_FUNCTION(const ntsa::Error& error) ErrorCallback;
    // Define a type alias for a function invoked when the
    // socket encounters an error.

    typedef NTCCFG_FUNCTION() DetachCallback;
    // Define a type alias for a function invoked when the
    // socket its detached from its proactor.

  private:
    bsl::shared_ptr<ntci::Proactor>     d_proactor_sp;
    bsl::shared_ptr<ntsi::StreamSocket> d_streamSocket_sp;
    ntsa::Handle                        d_handle;
    ntsa::Endpoint                      d_sourceEndpoint;
    ntsa::Endpoint                      d_remoteEndpoint;
    bsl::shared_ptr<bdlbb::Blob>        d_sendData_sp;
    bsl::shared_ptr<bdlbb::Blob>        d_receiveData_sp;
    bsl::shared_ptr<ntci::Strand>       d_strand_sp;
    ConnectCallback                     d_connectCallback;
    bslmt::Semaphore                    d_connectSemaphore;
    SendCallback                        d_sendCallback;
    bslmt::Semaphore                    d_sendSemaphore;
    ReceiveCallback                     d_receiveCallback;
    bslmt::Semaphore                    d_receiveSemaphore;
    ShutdownCallback                    d_shutdownCallback;
    bslmt::Semaphore                    d_shutdownSemaphore;
    ErrorCallback                       d_errorCallback;
    bslmt::Semaphore                    d_errorSemaphore;
    DetachCallback                      d_detachCallback;
    bslmt::Semaphore                    d_detachSemaphore;
    bool                                d_abortOnErrorFlag;
    ntsa::Error                         d_lastError;
    bslma::Allocator*                   d_allocator_p;

  private:
    ProactorStreamSocket(const ProactorStreamSocket&) BSLS_KEYWORD_DELETED;
    ProactorStreamSocket& operator=(const ProactorStreamSocket&)
        BSLS_KEYWORD_DELETED;

  private:
    void processSocketAccepted(const ntsa::Error& error,
                               const bsl::shared_ptr<ntsi::StreamSocket>&
                                   streamSocket) BSLS_KEYWORD_OVERRIDE;
    // Process the completion of the acceptance of the specified
    // 'streamSocket' or the specified 'error'.

    void processSocketConnected(const ntsa::Error& error)
        BSLS_KEYWORD_OVERRIDE;
    // Process the completion of the establishment of the connection or the
    // specified 'error'.

    void processSocketReceived(const ntsa::Error&          error,
                               const ntsa::ReceiveContext& context)
        BSLS_KEYWORD_OVERRIDE;
    // Process the completion of the reception of data described by the
    // specified 'context' or the specified 'error'.

    void processSocketSent(const ntsa::Error&       error,
                           const ntsa::SendContext& context)
        BSLS_KEYWORD_OVERRIDE;
    // Process the completion of the transmission of data described by the
    // specified 'context' or the specified 'error'.

    void processSocketError(const ntsa::Error& error) BSLS_KEYWORD_OVERRIDE;
    // Process the specified 'error' that has occurred on the socket.

    void processSocketDetached() BSLS_KEYWORD_OVERRIDE;
    // Process the completion of socket detachment.

    void close() BSLS_KEYWORD_OVERRIDE;
    // Close the socket.

    bool isStream() const BSLS_KEYWORD_OVERRIDE;
    // Return true if the proactor socket has stream semantics, otherwise
    // return false.

    bool isDatagram() const BSLS_KEYWORD_OVERRIDE;
    // Return true if the proactor socket has datagram semantics, otherwise
    // return false.

    bool isListener() const BSLS_KEYWORD_OVERRIDE;
    // Return true if the proactor socket is a listener for incoming
    // connection requests, otherwise return false.

    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;
    // Return the strand on which this object's functions should be called.

    ntsa::Handle handle() const BSLS_KEYWORD_OVERRIDE;
    // Return the handle to the descriptor.

  public:
    ProactorStreamSocket(const bsl::shared_ptr<ntci::Proactor>& proactor,
                         bslma::Allocator* basicAllocator = 0);
    // Create a new proactor stream socket run with the specified
    // 'proactor'. Optionally specify a 'basicAllocator' used to supply
    // memory. If 'basicAllocator' is 0, the currently installed default
    // allocator is used.

    ProactorStreamSocket(
        const bsl::shared_ptr<ntci::Proactor>&     proactor,
        const bsl::shared_ptr<ntsi::StreamSocket>& streamSocket,
        bslma::Allocator*                          basicAllocator = 0);
    // Create a new proactor stream socket implemented by the specified
    // 'streamSocket' run with the specified 'proactor'. Optionally specify
    // a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    // the currently installed default allocator is used.

    virtual ~ProactorStreamSocket();
    // Destroy this object.

    void setConnectCallback(const ConnectCallback& callback);
    // Set the callback to be invoked when the socket establishes a
    // connection to its peer to the specified 'callback'.

    void setSendCallback(const SendCallback& callback);
    // Set the callback to be invoked when the socket copies data to the
    // socket send buffer to the specified 'callback'.

    void setReceiveCallback(const ReceiveCallback& callback);
    // Set the callback to be invoked when the socket copies data from the
    // socket receive buffer to the specified 'callback'.

    void setShutdownCallback(const ShutdownCallback& callback);
    // Set the callback to be invoked when the socket has been shutdown to
    // the specified 'callback'.

    void setErrorCallback(const ErrorCallback& callback);
    // Set the callback to be invoked when the socket encounters an error
    // to the specified 'callback'.

    void setDetachCallback(const DetachCallback& callback);
    // Set the callback to be invoked when the socket is detached from its
    // proactor to the specified 'callback'.

    ntsa::Error bind(const ntsa::Endpoint& sourceEndpoint);
    // Bind the socket to the specified 'sourceEndpoint'. Return the
    // error.

    ntsa::Error connect(const ntsa::Endpoint& remoteEndpoint);
    // Connect to the specified 'remoteEndpoint'. Invoke the connection
    // callback when the connection is established or the error callback
    // if the connect fails. Return the error.

    ntsa::Error send(const bsl::shared_ptr<bdlbb::Blob>& data);
    // Send the specified 'data' to the peer endpoint. Invoke the send
    // callback when at least some of the data has been copied to the
    // socket send buffer or the error callback if the send fails. Return
    // the error.

    ntsa::Error receive(const bsl::shared_ptr<bdlbb::Blob>& data);
    // Recieve into the available capacity of the specified 'data' the
    // transmission from the peer endpoint. Invoke the receive callback
    // when at least some of the data has been copied out of the socket
    // receive buffer or the error callback if the receive fails. Return
    // the error.

    ntsa::Error shutdown(ntsa::ShutdownType::Value direction);
    // Shutdown the stream socket in the specified 'direction'. Return the
    // error.

    ntsa::Error cancel();
    // Cancel all pending operations. Return the error.

    void abortOnError(bool value);
    // Fail the test if the socket encounters and error according to the
    // specified 'value'.

    void waitForConnected();
    // Wait until the socket is connected to its peer.

    void waitForSent();
    // Wait until the socket has copied data to the socket send
    // buffer.

    void waitForReceived();
    // Wait until the socket has copied data from the socket receive
    // buffer.

    void waitForShutdown();
    // Wait until the socket has been shutdown.

    void waitForError();
    // Wait until the socket has encountered an error.

    void waitForDetached();
    // Wait until the socket is detached from its proactor.

    bool pollForConnected();
    // Poll for the socket is connected to its peer. Return
    // true if the proactor has connected, and false otherwise.

    bool pollForSent();
    // Poll for the socket has copied data to the socket send
    // buffer. Return true if the socket has copied data to the socket
    // send buffer, and false otherwise.

    bool pollForReceived();
    // Poll for the socket has copied data from the socket receive
    // buffer. Return true if the socket has copied data from the socket
    // receive buffer, and false otherwise.

    bool pollForError();
    // Poll for the socket has encountered an error. Return true
    // if the socket has encountered an error, and false otherwise.

    bool pollForShutdown();
    // Poll for the socket to be shutdown. Return true if the socket has
    // been shutdown, and false otherwise.

    bool pollForDetached();
    // Poll for the socket to be detached from its proactor. Return true if the
    // socket has been detached, and false otherwise.

    ntsa::Endpoint sourceEndpoint() const;
    // Return the source endpoint.

    ntsa::Endpoint remoteEndpoint() const;
    // Return the remote endpoint.

    ntsa::Error lastError() const;
    // Return the last asynchrously notified error.
};

class IoRingTest::ProactorListenerSocket : public ntci::ProactorSocket,
                               public ntccfg::Shared<ProactorListenerSocket>
{
    // Provide an implementation of a proactor listener socket for
    // use by this test driver. This class is thread safe.

  public:
    typedef NTCCFG_FUNCTION(
        const bsl::shared_ptr<IoRingTest::ProactorStreamSocket>&
            acceptedSocket) AcceptCallback;
    // Define a type alias for a function invoked when the
    // socket has accepted a connection from a peer.

    typedef NTCCFG_FUNCTION(const ntsa::Error& error) ErrorCallback;
    // Define a type alias for a function invoked when the
    // socket encounters an error.

    typedef NTCCFG_FUNCTION() DetachCallback;
    // Define a type alias for a function invoked when the
    // socket is detached from its reactor.

  private:
    typedef bsl::list<bsl::shared_ptr<IoRingTest::ProactorStreamSocket> >
        AcceptQueue;
    // Define a type alias for a queue of accepted sockets.

    bsl::shared_ptr<ntci::Proactor>       d_proactor_sp;
    bsl::shared_ptr<ntsi::ListenerSocket> d_listenerSocket_sp;
    ntsa::Handle                          d_handle;
    ntsa::Endpoint                        d_sourceEndpoint;
    bsl::shared_ptr<ntci::Strand>         d_strand_sp;
    AcceptQueue                           d_acceptQueue;
    AcceptCallback                        d_acceptCallback;
    bslmt::Semaphore                      d_acceptSemaphore;
    ErrorCallback                         d_errorCallback;
    bslmt::Semaphore                      d_errorSemaphore;
    DetachCallback                        d_detachCallback;
    bslmt::Semaphore                      d_detachSemaphore;
    bool                                  d_abortOnErrorFlag;
    ntsa::Error                           d_lastError;
    bslma::Allocator*                     d_allocator_p;

  private:
    ProactorListenerSocket(const ProactorListenerSocket&) BSLS_KEYWORD_DELETED;
    ProactorListenerSocket& operator=(const ProactorListenerSocket&)
        BSLS_KEYWORD_DELETED;

  private:
    void processSocketAccepted(const ntsa::Error& error,
                               const bsl::shared_ptr<ntsi::StreamSocket>&
                                   streamSocket) BSLS_KEYWORD_OVERRIDE;
    // Process the completion of the acceptance of the specified
    // 'streamSocket' or the specified 'error'.

    void processSocketConnected(const ntsa::Error& error)
        BSLS_KEYWORD_OVERRIDE;
    // Process the completion of the establishment of the connection or the
    // specified 'error'.

    void processSocketReceived(const ntsa::Error&          error,
                               const ntsa::ReceiveContext& context)
        BSLS_KEYWORD_OVERRIDE;
    // Process the completion of the reception of data described by the
    // specified 'context' or the specified 'error'.

    void processSocketSent(const ntsa::Error&       error,
                           const ntsa::SendContext& context)
        BSLS_KEYWORD_OVERRIDE;
    // Process the completion of the transmission of data described by the
    // specified 'context' or the specified 'error'.

    void processSocketError(const ntsa::Error& error) BSLS_KEYWORD_OVERRIDE;
    // Process the specified 'error' that has occurred on the socket.

    void processSocketDetached() BSLS_KEYWORD_OVERRIDE;
    // Process the completion of socket detachment.

    void close() BSLS_KEYWORD_OVERRIDE;
    // Close the socket.

    bool isStream() const BSLS_KEYWORD_OVERRIDE;
    // Return true if the proactor socket has stream semantics, otherwise
    // return false.

    bool isDatagram() const BSLS_KEYWORD_OVERRIDE;
    // Return true if the proactor socket has datagram semantics, otherwise
    // return false.

    bool isListener() const BSLS_KEYWORD_OVERRIDE;
    // Return true if the proactor socket is a listener for incoming
    // connection requests, otherwise return false.

    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;
    // Return the strand on which this object's functions should be called.

    ntsa::Handle handle() const BSLS_KEYWORD_OVERRIDE;
    // Return the handle to the descriptor.

  public:
    ProactorListenerSocket(const bsl::shared_ptr<ntci::Proactor>& proactor,
                           bslma::Allocator* basicAllocator = 0);
    // Create a new proactor socket of the specified 'transport' run with
    // the specified 'proactor'. Optionally specify a 'basicAllocator'
    // used to supply memory. If 'basicAllocator' is 0, the currently
    // installed default allocator is used.

    virtual ~ProactorListenerSocket();
    // Destroy this object.

    void setAcceptCallback(const AcceptCallback& callback);
    // Set the callback to be invoked when the socket accepts a
    // connection from a peer to the specified 'callback'.

    void setErrorCallback(const ErrorCallback& callback);
    // Set the callback to be invoked when the socket encounters an error
    // to the specified 'callback'.

    void setDetachCallback(const DetachCallback& callback);
    // Set the callback to be invoked when the socket is detached from its
    // proactor to the specified 'callback'.

    ntsa::Error bind(const ntsa::Endpoint& sourceEndpoint);
    // Bind the socket to the specified 'sourceEndpoint'. Return the
    // error.

    ntsa::Error listen();
    // Listen for incoming connections. Return the error.

    ntsa::Error connect(const ntsa::Endpoint& remoteEndpoint);
    // Connect to the specified 'remoteEndpoint'. Invoke the connection
    // callback when the connection is established or the error callback
    // if the connect fails. Return the error.

    ntsa::Error accept();
    // Accept the next connection. Invoke the accept callback when the
    // next connection has been acceped or the error callback if the
    // accept fails. Return the error.

    ntsa::Error cancel();
    // Cancel all pending operations. Return the error.

    bsl::shared_ptr<IoRingTest::ProactorStreamSocket> accepted();
    // Pop and return the next available accepted socket.

    void abortOnError(bool value);
    // Fail the test if the socket encounters and error according to the
    // specified 'value'.

    void waitForAccepted();
    // Wait until the socket has accepted a socket from a peer.

    void waitForError();
    // Wait until the socket has encountered an error.

    void waitForDetached();
    // Wait until the socket is detached from its proactor.

    bool pollForAccepted();
    // Poll for the socket has accepted a socket from a peer.
    // Return true if the proactor has accepted a socket, and false
    // otherwise.

    bool pollForError();
    // Poll for the socket has encountered an error. Return true
    // if the socket has encountered an error, and false otherwise.

    bool pollForDetached();
    // Poll for the socket to be detached from its proactor. Return true if the
    // socket has been detached, and false otherwise.

    ntsa::Endpoint sourceEndpoint() const;
    // Return the source endpoint.

    ntsa::Error lastError() const;
    // Return the last asynchrously notified error.
};

class IoRingTest::TimerSession : public ntci::TimerSession
{
    // Provide an implementation of the 'ntci::TimerSession'
    // protocol for use by this test driver.

    bsl::string  d_name;
    bslmt::Latch d_deadline;
    bslmt::Latch d_cancelled;
    bslmt::Latch d_closed;

  private:
    void processTimerDeadline(const bsl::shared_ptr<ntci::Timer>& timer,
                              const ntca::TimerEvent&             event)
        BSLS_KEYWORD_OVERRIDE;
    // Process the arrival of the timer's deadline.

    void processTimerCancelled(const bsl::shared_ptr<ntci::Timer>& timer,
                               const ntca::TimerEvent&             event)
        BSLS_KEYWORD_OVERRIDE;
    // Process the cancellation of the timer.

    void processTimerClosed(const bsl::shared_ptr<ntci::Timer>& timer,
                            const ntca::TimerEvent&             event)
        BSLS_KEYWORD_OVERRIDE;
    // Process the removal of the timer.

    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;
    // Return the strand on which this object's functions should be called.

  private:
    TimerSession(const TimerSession&) BSLS_KEYWORD_DELETED;
    TimerSession& operator=(const TimerSession&) BSLS_KEYWORD_DELETED;

  public:
    explicit TimerSession(const bsl::string& name,
                          bslma::Allocator*  basicAllocator = 0);
    // Create a new timer session having the specified 'name'. Optionally
    // specify a 'basicAllocator' used to supply memory. If
    // 'basicAllocator' is 0, the currently installed default allocator is
    // used.

    ~TimerSession() BSLS_KEYWORD_OVERRIDE;
    // Destroy this object.

    void wait(ntca::TimerEventType::Value timerEventType);
    // Wait until the occurrence of the specified 'timerEventType'.

    bool tryWait(ntca::TimerEventType::Value timerEventType);
    // Try to wait until the occurrence of the specified 'timerEventType'.
    // Return true if the wait is successful and the 'timerEventType' has
    // occurred, otherwise return false.

    bool has(ntca::TimerEventType::Value timerEventType);
    // Return true if the specified 'timerEventType' has occurred,
    // otherwise return false.

    bsl::size_t count(ntca::TimerEventType::Value timerEventType);
    // Return the number of occurrences of the specified 'timerEventType'.
};

void IoRingTest::ProactorStreamSocket::processSocketAccepted(
    const ntsa::Error&                         asyncError,
    const bsl::shared_ptr<ntsi::StreamSocket>& streamSocket)
{
    NTSCFG_TEST_TRUE(false);
}

void IoRingTest::ProactorStreamSocket::processSocketConnected(
    const ntsa::Error& asyncError)
{
    if (asyncError) {
        if (asyncError == ntsa::Error::e_CANCELLED) {
            return;
        }

        NTSCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor "
                              << d_handle << " at " << d_sourceEndpoint
                              << " failed to connect: " << asyncError
                              << NTSCFG_TEST_LOG_END;

        if (d_abortOnErrorFlag) {
            NTSCFG_TEST_EQ(asyncError, ntsa::Error::e_OK);
        }

        d_lastError = asyncError;

        d_errorSemaphore.post();

        if (d_errorCallback) {
            d_errorCallback(asyncError);
        }

        return;
    }

    NTSCFG_TEST_FALSE(asyncError);

    ntsa::Error error;

    ntsa::Endpoint sourceEndpoint;
    error = d_streamSocket_sp->sourceEndpoint(&sourceEndpoint);
    NTSCFG_TEST_OK(error);

    ntsa::Endpoint remoteEndpoint;
    error = d_streamSocket_sp->remoteEndpoint(&remoteEndpoint);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << sourceEndpoint << " to "
                          << remoteEndpoint << " connected"
                          << NTSCFG_TEST_LOG_END;

    d_sourceEndpoint = sourceEndpoint;
    d_remoteEndpoint = remoteEndpoint;

    d_connectSemaphore.post();

    if (d_connectCallback) {
        d_connectCallback();
    }
}

void IoRingTest::ProactorStreamSocket::processSocketReceived(
    const ntsa::Error&          asyncError,
    const ntsa::ReceiveContext& context)
{
    if (asyncError) {
        if (asyncError == ntsa::Error::e_CANCELLED) {
            return;
        }

        if (asyncError == ntsa::Error(ntsa::Error::e_EOF)) {
            NTSCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor "
                                  << d_handle << " at " << d_sourceEndpoint
                                  << " to " << d_remoteEndpoint << " shutdown"
                                  << NTSCFG_TEST_LOG_END;

            d_shutdownSemaphore.post();

            if (d_shutdownCallback) {
                d_shutdownCallback();
            }
        }
        else {
            NTSCFG_TEST_LOG_DEBUG
                << "Proactor stream socket descriptor " << d_handle << " at "
                << d_sourceEndpoint << " to " << d_remoteEndpoint
                << " failed to receive: " << asyncError << NTSCFG_TEST_LOG_END;

            if (d_abortOnErrorFlag) {
                NTSCFG_TEST_EQ(asyncError, ntsa::Error::e_OK);
            }

            d_lastError = asyncError;

            d_errorSemaphore.post();

            if (d_errorCallback) {
                d_errorCallback(asyncError);
            }
        }

        return;
    }

    NTSCFG_TEST_FALSE(asyncError);

    NTSCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " received "
                          << context.bytesReceived() << "/"
                          << context.bytesReceivable()
                          << (context.bytesReceivable() == 1 ? " byte"
                                                             : " bytes")
                          << NTSCFG_TEST_LOG_END;

    NTSCFG_TEST_TRUE(d_receiveData_sp);
    bsl::shared_ptr<bdlbb::Blob> data = d_receiveData_sp;
    d_receiveData_sp.reset();

    if (context.bytesReceived() == 0) {
        NTSCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor "
                              << d_handle << " at " << d_sourceEndpoint
                              << " to " << d_remoteEndpoint << " shutdown"
                              << NTSCFG_TEST_LOG_END;

        d_shutdownSemaphore.post();

        if (d_shutdownCallback) {
            d_shutdownCallback();
        }
    }
    else {
        d_receiveSemaphore.post();

        if (d_receiveCallback) {
            d_receiveCallback(data, context.bytesReceived());
        }
    }
}

void IoRingTest::ProactorStreamSocket::processSocketSent(const ntsa::Error& asyncError,
                                             const ntsa::SendContext& context)
{
    if (asyncError) {
        if (asyncError == ntsa::Error::e_CANCELLED) {
            return;
        }

        NTSCFG_TEST_LOG_DEBUG
            << "Proactor stream socket descriptor " << d_handle << " at "
            << d_sourceEndpoint << " to " << d_remoteEndpoint
            << " failed to send: " << asyncError << NTSCFG_TEST_LOG_END;

        if (d_abortOnErrorFlag) {
            NTSCFG_TEST_EQ(asyncError, ntsa::Error::e_OK);
        }

        d_lastError = asyncError;

        d_errorSemaphore.post();

        if (d_errorCallback) {
            d_errorCallback(asyncError);
        }

        return;
    }

    NTSCFG_TEST_FALSE(asyncError);

    NTSCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " sent "
                          << context.bytesSent() << "/"
                          << context.bytesSendable()
                          << (context.bytesSendable() == 1 ? " byte"
                                                           : " bytes")
                          << NTSCFG_TEST_LOG_END;

    NTSCFG_TEST_TRUE(d_sendData_sp);
    bsl::shared_ptr<bdlbb::Blob> data = d_sendData_sp;
    d_sendData_sp.reset();

    d_sendSemaphore.post();

    if (d_sendCallback) {
        d_sendCallback(data, context.bytesSent());
    }
}

void IoRingTest::ProactorStreamSocket::processSocketError(const ntsa::Error& error)
{
    NTSCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " error: " << error
                          << NTSCFG_TEST_LOG_END;

    if (d_abortOnErrorFlag) {
        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
    }

    d_lastError = error;

    d_errorSemaphore.post();

    if (d_errorCallback) {
        d_errorCallback(error);
    }
}

void IoRingTest::ProactorStreamSocket::processSocketDetached()
{
    NTSCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " is detached"
                          << NTSCFG_TEST_LOG_END;

    d_detachSemaphore.post();

    if (d_detachCallback) {
        d_detachCallback();
    }
}

void IoRingTest::ProactorStreamSocket::close()
{
    d_streamSocket_sp->close();
}

bool IoRingTest::ProactorStreamSocket::isStream() const
{
    return true;
}

bool IoRingTest::ProactorStreamSocket::isDatagram() const
{
    return false;
}

bool IoRingTest::ProactorStreamSocket::isListener() const
{
    return false;
}

const bsl::shared_ptr<ntci::Strand>& IoRingTest::ProactorStreamSocket::strand() const
{
    return d_strand_sp;
}

ntsa::Handle IoRingTest::ProactorStreamSocket::handle() const
{
    return d_handle;
}

IoRingTest::ProactorStreamSocket::ProactorStreamSocket(
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    bslma::Allocator*                      basicAllocator)
: d_proactor_sp(proactor)
, d_streamSocket_sp()
, d_handle(ntsa::k_INVALID_HANDLE)
, d_sourceEndpoint()
, d_remoteEndpoint()
, d_sendData_sp()
, d_receiveData_sp()
, d_strand_sp()
, d_connectCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_connectSemaphore()
, d_sendCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_sendSemaphore()
, d_receiveCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_receiveSemaphore()
, d_shutdownCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_shutdownSemaphore()
, d_errorCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_errorSemaphore()
, d_detachCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_detachSemaphore()
, d_abortOnErrorFlag(false)
, d_lastError()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntsa::Error error;

    d_streamSocket_sp = ntsf::System::createStreamSocket(d_allocator_p);

    error = d_streamSocket_sp->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    d_handle = d_streamSocket_sp->handle();

    error = d_streamSocket_sp->setBlocking(false);
    NTSCFG_TEST_OK(error);

    error =
        d_streamSocket_sp->bind(ntsa::Endpoint(ntsa::IpEndpoint(
                                    ntsa::IpEndpoint::loopbackIpv4Address(),
                                    ntsa::IpEndpoint::anyPort())),
                                true);
    NTSCFG_TEST_OK(error);

    error = d_streamSocket_sp->sourceEndpoint(&d_sourceEndpoint);
    NTSCFG_TEST_OK(error);

    if (d_proactor_sp->maxThreads() > 1) {
        d_strand_sp = d_proactor_sp->createStrand();
    }

    NTSCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " created"
                          << NTSCFG_TEST_LOG_END;
}

IoRingTest::ProactorStreamSocket::ProactorStreamSocket(
    const bsl::shared_ptr<ntci::Proactor>&     proactor,
    const bsl::shared_ptr<ntsi::StreamSocket>& streamSocket,
    bslma::Allocator*                          basicAllocator)
: d_proactor_sp(proactor)
, d_streamSocket_sp(streamSocket)
, d_handle(ntsa::k_INVALID_HANDLE)
, d_sourceEndpoint()
, d_remoteEndpoint()
, d_sendData_sp()
, d_receiveData_sp()
, d_strand_sp()
, d_connectCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_connectSemaphore()
, d_sendCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_sendSemaphore()
, d_receiveCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_receiveSemaphore()
, d_shutdownCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_shutdownSemaphore()
, d_errorCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_errorSemaphore()
, d_detachCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_detachSemaphore()
, d_abortOnErrorFlag(false)
, d_lastError()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntsa::Error error;

    d_handle = d_streamSocket_sp->handle();

    error = d_streamSocket_sp->setBlocking(false);
    NTSCFG_TEST_OK(error);

    error = d_streamSocket_sp->sourceEndpoint(&d_sourceEndpoint);
    NTSCFG_TEST_OK(error);

    error = d_streamSocket_sp->remoteEndpoint(&d_remoteEndpoint);
    NTSCFG_TEST_OK(error);

    if (d_proactor_sp->maxThreads() > 1) {
        d_strand_sp = d_proactor_sp->createStrand();
    }

    NTSCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " created"
                          << NTSCFG_TEST_LOG_END;
}

IoRingTest::ProactorStreamSocket::~ProactorStreamSocket()
{
    d_streamSocket_sp.reset();
    d_proactor_sp.reset();

    NTSCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " destroyed"
                          << NTSCFG_TEST_LOG_END;
}

void IoRingTest::ProactorStreamSocket::setConnectCallback(const ConnectCallback& callback)
{
    d_connectCallback = callback;
}

void IoRingTest::ProactorStreamSocket::setSendCallback(const SendCallback& callback)
{
    d_sendCallback = callback;
}

void IoRingTest::ProactorStreamSocket::setReceiveCallback(const ReceiveCallback& callback)
{
    d_receiveCallback = callback;
}

void IoRingTest::ProactorStreamSocket::setShutdownCallback(
    const ShutdownCallback& callback)
{
    d_shutdownCallback = callback;
}

void IoRingTest::ProactorStreamSocket::setErrorCallback(const ErrorCallback& callback)
{
    d_errorCallback = callback;
}

void IoRingTest::ProactorStreamSocket::setDetachCallback(const DetachCallback& callback)
{
    d_detachCallback = callback;
}

ntsa::Error IoRingTest::ProactorStreamSocket::bind(const ntsa::Endpoint& sourceEndpoint)
{
    ntsa::Error error;

    error = d_streamSocket_sp->bind(sourceEndpoint, false);
    if (error) {
        return error;
    }

    error = d_streamSocket_sp->sourceEndpoint(&d_sourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " is bound"
                          << NTSCFG_TEST_LOG_END;

    return ntsa::Error();
}

ntsa::Error IoRingTest::ProactorStreamSocket::connect(const ntsa::Endpoint& remoteEndpoint)
{
    d_remoteEndpoint = remoteEndpoint;

    NTSCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " is connecting"
                          << NTSCFG_TEST_LOG_END;

    bsl::shared_ptr<ProactorStreamSocket> self = this->getSelf(this);

    return d_proactor_sp->connect(self, remoteEndpoint);
}

ntsa::Error IoRingTest::ProactorStreamSocket::send(
    const bsl::shared_ptr<bdlbb::Blob>& data)
{
    NTSCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " sending up to "
                          << data->length()
                          << (data->length() == 1 ? " byte" : " bytes")
                          << NTSCFG_TEST_LOG_END;

    bsl::shared_ptr<ProactorStreamSocket> self = this->getSelf(this);

    NTSCFG_TEST_FALSE(d_sendData_sp);
    d_sendData_sp = data;

    return d_proactor_sp->send(self, *data, ntsa::SendOptions());
}

ntsa::Error IoRingTest::ProactorStreamSocket::receive(
    const bsl::shared_ptr<bdlbb::Blob>& data)
{
    NTSCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " receiving up to "
                          << data->totalSize() - data->length()
                          << (data->totalSize() - data->length() == 1
                                  ? " byte"
                                  : " bytes")
                          << NTSCFG_TEST_LOG_END;

    bsl::shared_ptr<ProactorStreamSocket> self = this->getSelf(this);

    NTSCFG_TEST_FALSE(d_receiveData_sp);
    d_receiveData_sp = data;

    return d_proactor_sp->receive(self, data.get(), ntsa::ReceiveOptions());
}

ntsa::Error IoRingTest::ProactorStreamSocket::shutdown(ntsa::ShutdownType::Value direction)
{
    bsl::shared_ptr<ProactorStreamSocket> self = this->getSelf(this);

    return d_proactor_sp->shutdown(self, direction);
}

ntsa::Error IoRingTest::ProactorStreamSocket::cancel()
{
    bsl::shared_ptr<ProactorStreamSocket> self = this->getSelf(this);

    ntsa::Error error = d_proactor_sp->cancel(self);

    d_receiveData_sp.reset();

    return error;
}

void IoRingTest::ProactorStreamSocket::abortOnError(bool value)
{
    d_abortOnErrorFlag = value;
}

void IoRingTest::ProactorStreamSocket::waitForConnected()
{
    d_connectSemaphore.wait();
}

void IoRingTest::ProactorStreamSocket::waitForSent()
{
    d_sendSemaphore.wait();
}

void IoRingTest::ProactorStreamSocket::waitForReceived()
{
    d_receiveSemaphore.wait();
}

void IoRingTest::ProactorStreamSocket::waitForShutdown()
{
    d_shutdownSemaphore.wait();
}

void IoRingTest::ProactorStreamSocket::waitForError()
{
    d_errorSemaphore.wait();
}

void IoRingTest::ProactorStreamSocket::waitForDetached()
{
    d_detachSemaphore.wait();
}

bool IoRingTest::ProactorStreamSocket::pollForConnected()
{
    return d_connectSemaphore.tryWait() == 0;
}

bool IoRingTest::ProactorStreamSocket::pollForSent()
{
    return d_sendSemaphore.tryWait() == 0;
}

bool IoRingTest::ProactorStreamSocket::pollForReceived()
{
    return d_receiveSemaphore.tryWait() == 0;
}

bool IoRingTest::ProactorStreamSocket::pollForShutdown()
{
    return d_shutdownSemaphore.tryWait() == 0;
}

bool IoRingTest::ProactorStreamSocket::pollForError()
{
    return d_errorSemaphore.tryWait() == 0;
}

bool IoRingTest::ProactorStreamSocket::pollForDetached()
{
    return d_detachSemaphore.tryWait() == 0;
}

ntsa::Endpoint IoRingTest::ProactorStreamSocket::sourceEndpoint() const
{
    return d_sourceEndpoint;
}

ntsa::Endpoint IoRingTest::ProactorStreamSocket::remoteEndpoint() const
{
    return d_remoteEndpoint;
}

ntsa::Error IoRingTest::ProactorStreamSocket::lastError() const
{
    return d_lastError;
}

void IoRingTest::ProactorListenerSocket::processSocketAccepted(
    const ntsa::Error&                         asyncError,
    const bsl::shared_ptr<ntsi::StreamSocket>& streamSocket)
{
    if (asyncError) {
        if (asyncError == ntsa::Error::e_CANCELLED) {
            return;
        }

        NTSCFG_TEST_LOG_DEBUG << "Proactor socket descriptor " << d_handle
                              << " at " << d_sourceEndpoint
                              << " failed to accept: " << asyncError
                              << NTSCFG_TEST_LOG_END;

        if (d_abortOnErrorFlag) {
            NTSCFG_TEST_EQ(asyncError, ntsa::Error::e_OK);
        }

        d_lastError = asyncError;

        d_errorSemaphore.post();

        if (d_errorCallback) {
            d_errorCallback(asyncError);
        }

        return;
    }

    NTSCFG_TEST_FALSE(asyncError);

    ntsa::Error error;

    error = streamSocket->setBlocking(false);
    NTSCFG_TEST_OK(error);

    ntsa::Endpoint sourceEndpoint;
    error = streamSocket->sourceEndpoint(&sourceEndpoint);
    NTSCFG_TEST_OK(error);

    ntsa::Endpoint remoteEndpoint;
    error = streamSocket->remoteEndpoint(&remoteEndpoint);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_LOG_DEBUG << "Proactor listener socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint
                          << " accepted proactor stream socket descriptor "
                          << streamSocket->handle() << " at " << sourceEndpoint
                          << " to " << remoteEndpoint << NTSCFG_TEST_LOG_END;

    bsl::shared_ptr<IoRingTest::ProactorStreamSocket> proactorSocket;
    proactorSocket.createInplace(d_allocator_p,
                                 d_proactor_sp,
                                 streamSocket,
                                 d_allocator_p);

    d_acceptQueue.push_back(proactorSocket);

    d_acceptSemaphore.post();

    if (d_acceptCallback) {
        d_acceptCallback(proactorSocket);
    }
}

void IoRingTest::ProactorListenerSocket::processSocketConnected(
    const ntsa::Error& asyncError)
{
    NTSCFG_TEST_TRUE(false);
}

void IoRingTest::ProactorListenerSocket::processSocketReceived(
    const ntsa::Error&          asyncError,
    const ntsa::ReceiveContext& context)
{
    NTSCFG_TEST_TRUE(false);
}

void IoRingTest::ProactorListenerSocket::processSocketSent(
    const ntsa::Error&       asyncError,
    const ntsa::SendContext& context)
{
    NTSCFG_TEST_TRUE(false);
}

void IoRingTest::ProactorListenerSocket::processSocketError(const ntsa::Error& error)
{
    NTSCFG_TEST_LOG_DEBUG << "Proactor listener socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " error: " << error
                          << NTSCFG_TEST_LOG_END;

    if (d_abortOnErrorFlag) {
        NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);
    }

    d_lastError = error;

    d_errorSemaphore.post();

    if (d_errorCallback) {
        d_errorCallback(error);
    }
}

void IoRingTest::ProactorListenerSocket::processSocketDetached()
{
    NTSCFG_TEST_LOG_DEBUG << "Proactor listener socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " is detached"
                          << NTSCFG_TEST_LOG_END;

    d_detachSemaphore.post();

    if (d_detachCallback) {
        d_detachCallback();
    }
}

void IoRingTest::ProactorListenerSocket::close()
{
    d_listenerSocket_sp->close();
}

bool IoRingTest::ProactorListenerSocket::isStream() const
{
    return true;
}

bool IoRingTest::ProactorListenerSocket::isDatagram() const
{
    return false;
}

bool IoRingTest::ProactorListenerSocket::isListener() const
{
    return true;
}

const bsl::shared_ptr<ntci::Strand>& IoRingTest::ProactorListenerSocket::strand() const
{
    return d_strand_sp;
}

ntsa::Handle IoRingTest::ProactorListenerSocket::handle() const
{
    return d_handle;
}

IoRingTest::ProactorListenerSocket::ProactorListenerSocket(
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    bslma::Allocator*                      basicAllocator)
: d_proactor_sp(proactor)
, d_listenerSocket_sp()
, d_handle(ntsa::k_INVALID_HANDLE)
, d_sourceEndpoint()
, d_strand_sp()
, d_acceptQueue(basicAllocator)
, d_acceptCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_acceptSemaphore()
, d_errorCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_errorSemaphore()
, d_detachCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_detachSemaphore()
, d_abortOnErrorFlag(false)
, d_lastError()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntsa::Error error;

    d_listenerSocket_sp = ntsf::System::createListenerSocket(d_allocator_p);

    error = d_listenerSocket_sp->open(ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_OK(error);

    d_handle = d_listenerSocket_sp->handle();

    error = d_listenerSocket_sp->setBlocking(false);
    NTSCFG_TEST_OK(error);

    error =
        d_listenerSocket_sp->bind(ntsa::Endpoint(ntsa::IpEndpoint(
                                      ntsa::IpEndpoint::loopbackIpv4Address(),
                                      ntsa::IpEndpoint::anyPort())),
                                  false);
    NTSCFG_TEST_OK(error);

    error = d_listenerSocket_sp->sourceEndpoint(&d_sourceEndpoint);
    NTSCFG_TEST_OK(error);

    if (d_proactor_sp->maxThreads() > 1) {
        d_strand_sp = d_proactor_sp->createStrand();
    }

    NTSCFG_TEST_LOG_DEBUG << "Proactor listener socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " created"
                          << NTSCFG_TEST_LOG_END;
}

IoRingTest::ProactorListenerSocket::~ProactorListenerSocket()
{
    d_acceptQueue.clear();

    d_listenerSocket_sp.reset();
    d_proactor_sp.reset();

    NTSCFG_TEST_LOG_DEBUG << "Proactor listener socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " destroyed"
                          << NTSCFG_TEST_LOG_END;
}

void IoRingTest::ProactorListenerSocket::setAcceptCallback(const AcceptCallback& callback)
{
    d_acceptCallback = callback;
}

void IoRingTest::ProactorListenerSocket::setErrorCallback(const ErrorCallback& callback)
{
    d_errorCallback = callback;
}

void IoRingTest::ProactorListenerSocket::setDetachCallback(const DetachCallback& callback)
{
    d_detachCallback = callback;
}

ntsa::Error IoRingTest::ProactorListenerSocket::bind(const ntsa::Endpoint& sourceEndpoint)
{
    ntsa::Error error;

    error = d_listenerSocket_sp->bind(sourceEndpoint, false);
    if (error) {
        return error;
    }

    error = d_listenerSocket_sp->sourceEndpoint(&d_sourceEndpoint);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_LOG_DEBUG << "Proactor listener socket descriptor " << d_handle
                          << "at " << d_sourceEndpoint << " is bound"
                          << NTSCFG_TEST_LOG_END;

    return ntsa::Error();
}

ntsa::Error IoRingTest::ProactorListenerSocket::listen()
{
    NTSCFG_TEST_LOG_DEBUG << "Proactor listener socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " is listening"
                          << NTSCFG_TEST_LOG_END;

    return d_listenerSocket_sp->listen(1);
}

ntsa::Error IoRingTest::ProactorListenerSocket::accept()
{
    NTSCFG_TEST_LOG_DEBUG << "Proactor listener socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " initiating accept"
                          << NTSCFG_TEST_LOG_END;

    bsl::shared_ptr<ProactorSocket> self = this->getSelf(this);

    return d_proactor_sp->accept(self);
}

ntsa::Error IoRingTest::ProactorListenerSocket::cancel()
{
    bsl::shared_ptr<ProactorSocket> self = this->getSelf(this);

    return d_proactor_sp->cancel(self);
}

bsl::shared_ptr<IoRingTest::ProactorStreamSocket> IoRingTest::ProactorListenerSocket::
    accepted()
{
    NTSCFG_TEST_FALSE(d_acceptQueue.empty());

    bsl::shared_ptr<IoRingTest::ProactorStreamSocket> result =
        d_acceptQueue.front();
    d_acceptQueue.pop_front();

    return result;
}

void IoRingTest::ProactorListenerSocket::abortOnError(bool value)
{
    d_abortOnErrorFlag = value;
}

void IoRingTest::ProactorListenerSocket::waitForAccepted()
{
    d_acceptSemaphore.wait();
}

void IoRingTest::ProactorListenerSocket::waitForError()
{
    d_errorSemaphore.wait();
}

void IoRingTest::ProactorListenerSocket::waitForDetached()
{
    d_detachSemaphore.wait();
}

bool IoRingTest::ProactorListenerSocket::pollForAccepted()
{
    return d_acceptSemaphore.tryWait() == 0;
}

bool IoRingTest::ProactorListenerSocket::pollForError()
{
    return d_errorSemaphore.tryWait() == 0;
}

bool IoRingTest::ProactorListenerSocket::pollForDetached()
{
    return d_detachSemaphore.tryWait() == 0;
}

ntsa::Endpoint IoRingTest::ProactorListenerSocket::sourceEndpoint() const
{
    return d_sourceEndpoint;
}

ntsa::Error IoRingTest::ProactorListenerSocket::lastError() const
{
    return d_lastError;
}

void IoRingTest::TimerSession::processTimerDeadline(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTSCFG_TEST_EQ(event.type(), ntca::TimerEventType::e_DEADLINE);

    NTSCFG_TEST_LOG_DEBUG << "Timer '" << d_name << "' deadline"
                          << NTSCFG_TEST_LOG_END;

    d_deadline.arrive();
}

void IoRingTest::TimerSession::processTimerCancelled(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTSCFG_TEST_EQ(event.type(), ntca::TimerEventType::e_CANCELED);

    NTSCFG_TEST_LOG_DEBUG << "Timer '" << d_name << "' cancelled"
                          << NTSCFG_TEST_LOG_END;

    d_cancelled.arrive();
}

void IoRingTest::TimerSession::processTimerClosed(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTSCFG_TEST_EQ(event.type(), ntca::TimerEventType::e_CLOSED);

    NTSCFG_TEST_LOG_DEBUG << "Timer '" << d_name << "' closed"
                          << NTSCFG_TEST_LOG_END;

    d_closed.arrive();
}

const bsl::shared_ptr<ntci::Strand>& IoRingTest::TimerSession::strand() const
{
    return ntci::Strand::unspecified();
}

IoRingTest::TimerSession::TimerSession(const bsl::string& name,
                           bslma::Allocator*  basicAllocator)
: d_name(name, basicAllocator)
, d_deadline(1)
, d_cancelled(1)
, d_closed(1)
{
}

IoRingTest::TimerSession::~TimerSession()
{
}

void IoRingTest::TimerSession::wait(ntca::TimerEventType::Value timerEventType)
{
    if (timerEventType == ntca::TimerEventType::e_DEADLINE) {
        d_deadline.wait();
    }
    else if (timerEventType == ntca::TimerEventType::e_CANCELED) {
        d_cancelled.wait();
    }
    else if (timerEventType == ntca::TimerEventType::e_CLOSED) {
        d_closed.wait();
    }
    else {
        NTSCFG_TEST_TRUE(false);
    }
}

bool IoRingTest::TimerSession::tryWait(ntca::TimerEventType::Value timerEventType)
{
    if (timerEventType == ntca::TimerEventType::e_DEADLINE) {
        return d_deadline.tryWait();
    }
    else if (timerEventType == ntca::TimerEventType::e_CANCELED) {
        return d_cancelled.tryWait();
    }
    else if (timerEventType == ntca::TimerEventType::e_CLOSED) {
        return d_closed.tryWait();
    }
    else {
        NTSCFG_TEST_TRUE(false);
        return false;
    }
}

bool IoRingTest::TimerSession::has(ntca::TimerEventType::Value timerEventType)
{
    if (timerEventType == ntca::TimerEventType::e_DEADLINE) {
        return d_deadline.currentCount() != 1;
    }
    else if (timerEventType == ntca::TimerEventType::e_CANCELED) {
        return d_cancelled.currentCount() != 1;
    }
    else if (timerEventType == ntca::TimerEventType::e_CLOSED) {
        return d_closed.currentCount() != 1;
    }
    else {
        NTSCFG_TEST_TRUE(false);
        return 0;
    }
}

bsl::size_t IoRingTest::TimerSession::count(ntca::TimerEventType::Value timerEventType)
{
    if (timerEventType == ntca::TimerEventType::e_DEADLINE) {
        return 1 - d_deadline.currentCount();
    }
    else if (timerEventType == ntca::TimerEventType::e_CANCELED) {
        return 1 - d_cancelled.currentCount();
    }
    else if (timerEventType == ntca::TimerEventType::e_CLOSED) {
        return 1 - d_closed.currentCount();
    }
    else {
        NTSCFG_TEST_TRUE(false);
        return 0;
    }
}

void IoRingTest::processFunction(bslmt::Latch* latch)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Executed function");

    latch->arrive();
}

NTSCFG_TEST_FUNCTION(ntcs::IoRingTest::verifyCase1)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    if (!ntco::IoRingFactory::isSupported()) {
        return;
    }

    const bsl::size_t k_QUEUE_DEPTH = 4;


        NTSCFG_TEST_TRUE(ntco::IoRingFactory::isSupported());

        bsl::shared_ptr<ntco::IoRingFactory> proactorFactory;
        proactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<ntco::IoRingTest> test =
            proactorFactory->createTest(k_QUEUE_DEPTH, NTSCFG_TEST_ALLOCATOR);

        NTSCFG_TEST_EQ(test->submissionQueueCapacity(), k_QUEUE_DEPTH);
        NTSCFG_TEST_EQ(test->completionQueueCapacity(), k_QUEUE_DEPTH * 2);

        const bsl::size_t k_SUBMISSION_COUNT =
            test->completionQueueCapacity() + test->submissionQueueCapacity() -
            1;
        // We shall not overflow completion queue, so at maximum we push
        // k_SUBMISSION_COUNT entries

        for (bsl::size_t id = 0; id < k_SUBMISSION_COUNT; ++id) {
            NTCO_IORING_TEST_LOG_PUSH_STARTING(test, id);
            test->defer(id);
            NTCO_IORING_TEST_LOG_PUSH_COMPLETE(test, id);
        }

        bsl::size_t expectedId = 0;
        {
            bsl::vector<bsl::uint64_t> result;
            test->wait(&result, test->completionQueueCapacity());

            NTSCFG_TEST_EQ(result.size(), test->completionQueueCapacity());

            for (bsl::size_t i = 0; i < result.size(); ++i) {
                NTCO_IORING_TEST_LOG_POPPED(test, result[i]);
                NTSCFG_TEST_EQ(result[i], expectedId);
                ++expectedId;
            }
        }

        {
            // Add one more submission entry, it will automatically cause
            // entering the io_ring and submission of existing entries
            NTCO_IORING_TEST_LOG_PUSH_STARTING(test, k_SUBMISSION_COUNT);
            test->defer(k_SUBMISSION_COUNT);
            NTCO_IORING_TEST_LOG_PUSH_COMPLETE(test, k_SUBMISSION_COUNT);

            const size_t entriesToWait = test->submissionQueueCapacity();
            bsl::vector<bsl::uint64_t> result;
            test->wait(&result, entriesToWait);

            NTSCFG_TEST_EQ(result.size(), entriesToWait);

            for (bsl::size_t i = 0; i < result.size(); ++i) {
                NTCO_IORING_TEST_LOG_POPPED(test, result[i]);
                NTSCFG_TEST_EQ(result[i], expectedId);
                ++expectedId;
            }
        }
}

NTSCFG_TEST_FUNCTION(ntcs::IoRingTest::verifyCase2)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    if (!ntco::IoRingFactory::isSupported()) {
        return;
    }

        ntsa::Error error;

        // Create the blob buffer factory.

        bdlbb::PooledBlobBufferFactory blobBufferFactory(32, NTSCFG_TEST_ALLOCATOR);

        // Define the user.

        bsl::shared_ptr<ntci::User> user;

        // Create the proactor.

        ntca::ProactorConfig proactorConfig;
        proactorConfig.setMetricName("test");
        proactorConfig.setMinThreads(1);
        proactorConfig.setMaxThreads(1);

        bsl::shared_ptr<ntco::IoRingFactory> proactorFactory;
        proactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

        bsl::shared_ptr<ntci::Proactor> proactor =
            proactorFactory->createProactor(proactorConfig, user, NTSCFG_TEST_ALLOCATOR);

        // Register this thread as the thread that will wait on the
        // proactor.

        ntci::Waiter waiter = proactor->registerWaiter(ntca::WaiterOptions());

        // Create a TCP/IPv4 non-blocking socket bound to any ephemeral
        // port on the IPv4 loopback address. This socket will act as the
        // listener.

        bsl::shared_ptr<IoRingTest::ProactorListenerSocket> listener;
        listener.createInplace(NTSCFG_TEST_ALLOCATOR, proactor, NTSCFG_TEST_ALLOCATOR);

        listener->abortOnError(true);

        // Begin listening for connections.

        error = listener->listen();
        NTSCFG_TEST_OK(error);

        // Attach the listener socket to the proactor.

        error = proactor->attachSocket(listener);
        NTSCFG_TEST_OK(error);

        // Create a TCP/IPv4 non-blocking socket. This socket will act as
        // the client.

        bsl::shared_ptr<IoRingTest::ProactorStreamSocket> client;
        client.createInplace(NTSCFG_TEST_ALLOCATOR, proactor, NTSCFG_TEST_ALLOCATOR);

        client->abortOnError(true);

        // Attach the client socket to the proactor.

        error = proactor->attachSocket(client);
        NTSCFG_TEST_OK(error);

        // Asynchronously accept the next connection.

        error = listener->accept();
        NTSCFG_TEST_OK(error);

        // Asynchronously connect the client socket to the listener's local
        // endpoint.

        ntsa::Endpoint serverEndpoint = listener->sourceEndpoint();
        serverEndpoint.ip().setHost(ntsa::Ipv4Address::loopback());

        error = client->connect(listener->sourceEndpoint());
        NTSCFG_TEST_OK(error);

        // Wait for the listener to accept the connection from the client
        // and pop that socket off the accept queue. This socket will
        // act as the server socket.

        while (!listener->pollForAccepted()) {
            proactor->poll(waiter);
        }

        bsl::shared_ptr<IoRingTest::ProactorStreamSocket> server =
            listener->accepted();

        server->abortOnError(true);

        // Attach the server socket to the proactor.

        error = proactor->attachSocket(server);
        NTSCFG_TEST_OK(error);

        // Wait for the client to become connected.

        while (!client->pollForConnected()) {
            proactor->poll(waiter);
        }

        // Asynchronously accept the next connection.

        error = listener->accept();
        NTSCFG_TEST_OK(error);

        // Cancel the accept operation.

        error = listener->cancel();
        NTSCFG_TEST_OK(error);

        // Wait for the accept to be cancelled.

        proactor->poll(waiter);

        // Send a single byte to the server.

        {
            bsl::shared_ptr<bdlbb::Blob> data;
            data.createInplace(NTSCFG_TEST_ALLOCATOR, &blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

            bdlbb::BlobUtil::append(data.get(), "X", 1);

            error = client->send(data);
            NTSCFG_TEST_OK(error);
        }

        // Wait for the data to be copied to the client's socket send
        // buffer.

        while (!client->pollForSent()) {
            proactor->poll(waiter);
        }

        // Receive a single byte from the client.

        {
            bsl::shared_ptr<bdlbb::Blob> data;
            data.createInplace(NTSCFG_TEST_ALLOCATOR, &blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

            data->setLength(1);
            data->setLength(0);

            error = server->receive(data);
            NTSCFG_TEST_OK(error);
        }

        // Wait for the data to be copied from the server's socket receive
        // buffer.

        while (!server->pollForReceived()) {
            proactor->poll(waiter);
        }

#if NTCO_IORING_TEST_SHUTDOWN_WRITE

        client->abortOnError(false);
        server->abortOnError(false);

        // Shutdown writing from the server.

        error = server->shutdown(ntsa::ShutdownType::e_SEND);
        NTSCFG_TEST_OK(error);

        // Try to send a single byte to the client, but observe the send
        // fails because the server has shutdown writing from its side of
        // the connection.

        {
            bsl::shared_ptr<bdlbb::Blob> data;
            data.createInplace(NTSCFG_TEST_ALLOCATOR, &blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

            bdlbb::BlobUtil::append(data.get(), "X", 1);

            error = server->send(data);
            if (error) {
                NTSCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
            }
            else {
                // Wait until the send attempt results in an error.

                while (!server->pollForError()) {
                    proactor->poll(waiter);
                }

                error = server->lastError();
                NTSCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
            }
        }

        // Receive a single byte from the server, but observe that zero
        // bytes have been successfully read, indicating the server has
        // shutdown writing from its side of the connection.

        {
            bsl::shared_ptr<bdlbb::Blob> data;
            data.createInplace(NTSCFG_TEST_ALLOCATOR, &blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

            data->setLength(1);
            data->setLength(0);

            error = client->receive(data);
            NTSCFG_TEST_OK(error);
        }

        // Wait for the client detects the socket has been shutdown.

        while (!client->pollForShutdown()) {
            proactor->poll(waiter);
        }

        // Shutdown writing from the client.

        error = client->shutdown(ntsa::ShutdownType::e_SEND);
        NTSCFG_TEST_OK(error);

        // Try to send a single byte to the server, but observe the send
        // fails because the client has shutdown writing from its side of
        // the connection.

        {
            bsl::shared_ptr<bdlbb::Blob> data;
            data.createInplace(NTSCFG_TEST_ALLOCATOR, &blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

            bdlbb::BlobUtil::append(data.get(), "X", 1);

            error = client->send(data);

            if (error) {
                NTSCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
            }
            else {
                // Wait until the send attempt results in an error.

                while (!client->pollForError()) {
                    proactor->poll(waiter);
                }

                error = client->lastError();
                NTSCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
            }
        }

        // Receive a single byte from the client, but observe that zero
        // bytes have been successfully read, indicating the client has
        // shutdown writing from its side of the connection.

        {
            bsl::shared_ptr<bdlbb::Blob> data;
            data.createInplace(NTSCFG_TEST_ALLOCATOR, &blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

            data->setLength(1);
            data->setLength(0);

            error = server->receive(data);
            NTSCFG_TEST_OK(error);
        }

        // Wait for the server detects the socket has been shutdown.

        while (!server->pollForShutdown()) {
            proactor->poll(waiter);
        }

#endif

#if NTCO_IORING_TEST_SHUTDOWN_READ

        // Shutdown reading from the server.

        error = server->shutdown(ntsa::ShutdownType::e_RECEIVE);
        NTSCFG_TEST_OK(error);

        // Try to receive a single byte from the client, and observe the
        // receive fails indicating the connection is dead.

        {
            bsl::shared_ptr<bdlbb::Blob> data;
            data.createInplace(NTSCFG_TEST_ALLOCATOR, &blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

            data->setLength(1);
            data->setLength(0);

            error = server->receive(data);
            if (error) {
                NTSCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
            }
            else {
                while (!server->pollForError()) {
                    proactor->poll(waiter);
                }

                error = server->lastError();
                NTSCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
            }
        }

        // Shutdown reading from the client.

        error = client->shutdown(ntsa::ShutdownType::e_RECEIVE);
        NTSCFG_TEST_OK(error);

        // Try to receive a single byte from the server, and observe the
        // receive fails indicating the connection is dead.

        {
            bsl::shared_ptr<bdlbb::Blob> data;
            data.createInplace(NTSCFG_TEST_ALLOCATOR, &blobBufferFactory, NTSCFG_TEST_ALLOCATOR);

            data->setLength(1);
            data->setLength(0);

            error = client->receive(data);
            if (error) {
                NTSCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
            }
            else {
                while (!client->pollForError()) {
                    proactor->poll(waiter);
                }

                error = client->lastError();
                NTSCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
            }
        }

#endif

        // Detach the server from the proactor.

        error = proactor->detachSocket(server);
        NTSCFG_TEST_OK(error);

        // Wait for the server to become detached from the proactor.

        while (!server->pollForDetached()) {
            proactor->poll(waiter);
        }

        // Detach the client from the proactor.

        error = proactor->detachSocket(client);
        NTSCFG_TEST_OK(error);

        // Wait for the client to become detached from the proactor.

        while (!client->pollForDetached()) {
            proactor->poll(waiter);
        }

        // Detach the listener from the proactor.

        proactor->detachSocket(listener);
        NTSCFG_TEST_OK(error);

        // Wait for the listener to become detached from the proactor.

        while (!listener->pollForDetached()) {
            proactor->poll(waiter);
        }

        // Deregister the waiter.

        proactor->deregisterWaiter(waiter);
}



NTSCFG_TEST_FUNCTION(ntcs::IoRingTest::verifyCase3)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    if (!ntco::IoRingFactory::isSupported()) {
        return;
    }

    for (bsl::size_t maskInterestCase = 0; maskInterestCase < 4;
         ++maskInterestCase)
    {
        for (bsl::size_t oneShotCase = 0; oneShotCase < 2; ++oneShotCase) {
            ntca::TimerOptions timerOptions;

            switch (maskInterestCase) {
            case 0:
                timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
                timerOptions.showEvent(ntca::TimerEventType::e_CANCELED);
                timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);
                break;
            case 1:
                timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
                timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
                timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);
                break;
            case 2:
                timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
                timerOptions.showEvent(ntca::TimerEventType::e_CANCELED);
                timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
                break;
            case 3:
                timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
                timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
                timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
                break;
            }

            if (oneShotCase == 0) {
                timerOptions.setOneShot(false);
            }
            else {
                timerOptions.setOneShot(true);
            }

            NTSCFG_TEST_LOG_INFO << "Testing timer options = " << timerOptions
                                 << NTSCFG_TEST_LOG_END;

                ntsa::Error error;

                // Define the user.

                bsl::shared_ptr<ntci::User> user;

                // Create the proactor.

                ntca::ProactorConfig proactorConfig;
                proactorConfig.setMetricName("test");
                proactorConfig.setMinThreads(1);
                proactorConfig.setMaxThreads(1);

                bsl::shared_ptr<ntco::IoRingFactory> proactorFactory;
                proactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

                bsl::shared_ptr<ntci::Proactor> proactor =
                    proactorFactory->createProactor(proactorConfig, user, NTSCFG_TEST_ALLOCATOR);

                // Register this thread as a thread that will wait on the
                // proactor.

                ntci::Waiter waiter =
                    proactor->registerWaiter(ntca::WaiterOptions());

                // Register three timers to fire at t1, t2, and t3. Implement the
                // first timer so that when it fires at t1, it cancels the timer
                // at t2.

                bslma::Allocator* allocator =
                    bslma::Default::defaultAllocator();

                bsl::shared_ptr<IoRingTest::TimerSession> timerSession1;
                timerSession1.createInplace(NTSCFG_TEST_ALLOCATOR, "timer1", NTSCFG_TEST_ALLOCATOR);

                bsl::shared_ptr<IoRingTest::TimerSession> timerSession2;
                timerSession2.createInplace(NTSCFG_TEST_ALLOCATOR, "timer2", NTSCFG_TEST_ALLOCATOR);

                bsl::shared_ptr<IoRingTest::TimerSession> timerSession3;
                timerSession3.createInplace(NTSCFG_TEST_ALLOCATOR, "timer3", NTSCFG_TEST_ALLOCATOR);

                bsl::shared_ptr<ntci::Timer> timer1 = proactor->createTimer(
                    timerOptions,
                    static_cast<bsl::shared_ptr<ntci::TimerSession> >(
                        timerSession1),
                    NTSCFG_TEST_ALLOCATOR);

                bsl::shared_ptr<ntci::Timer> timer2 = proactor->createTimer(
                    timerOptions,
                    static_cast<bsl::shared_ptr<ntci::TimerSession> >(
                        timerSession2),
                    NTSCFG_TEST_ALLOCATOR);

                bsl::shared_ptr<ntci::Timer> timer3 = proactor->createTimer(
                    timerOptions,
                    static_cast<bsl::shared_ptr<ntci::TimerSession> >(
                        timerSession3),
                    NTSCFG_TEST_ALLOCATOR);

                bsls::TimeInterval now = bdlt::CurrentTime::now();

                timer1->schedule(now + bsls::TimeInterval(1));
                timer2->schedule(now + bsls::TimeInterval(2));
                timer3->schedule(now + bsls::TimeInterval(3));

                // Wait for the first timer at t1 to fire.

                NTSCFG_TEST_TRUE(
                    timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));

                while (
                    !timerSession1->tryWait(ntca::TimerEventType::e_DEADLINE))
                {
                    proactor->poll(waiter);
                }

                if (!timerOptions.oneShot()) {
                    timer1->close();
                }

                // Cancel the timer at t2.

                timer2->close();

                // Wait for the timer at t1 to be closed.

                if (timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED)) {
                    while (!timerSession1->tryWait(
                        ntca::TimerEventType::e_CLOSED))
                    {
                        proactor->poll(waiter);
                    }
                }

                // Wait for the timer at t2 to be cancelled.

                if (timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED)) {
                    while (!timerSession2->tryWait(
                        ntca::TimerEventType::e_CANCELED))
                    {
                        proactor->poll(waiter);
                    }
                }

                // Wait for the timer at t2 to be closed.

                if (timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED)) {
                    while (!timerSession2->tryWait(
                        ntca::TimerEventType::e_CLOSED))
                    {
                        proactor->poll(waiter);
                    }
                }

                // Wait for the timer at t3 to fire.

                NTSCFG_TEST_TRUE(
                    timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));

                while (
                    !timerSession3->tryWait(ntca::TimerEventType::e_DEADLINE))
                {
                    proactor->poll(waiter);
                }

                if (!timerOptions.oneShot()) {
                    timer3->close();
                }

                // Wait for the timer at t3 to be closed.

                if (timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED)) {
                    while (!timerSession3->tryWait(
                        ntca::TimerEventType::e_CLOSED))
                    {
                        proactor->poll(waiter);
                    }
                }

                // Ensure the timer at t1 fired and was not cancelled.

                NTSCFG_TEST_EQ(
                    timerSession1->count(ntca::TimerEventType::e_DEADLINE),
                    1);
                if (timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED)) {
                    NTSCFG_TEST_EQ(
                        timerSession1->count(ntca::TimerEventType::e_CANCELED),
                        0);
                }

                // Ensure the timer at t2 did not fire, because it was cancelled
                // by the timer at t1.

                NTSCFG_TEST_EQ(
                    timerSession2->count(ntca::TimerEventType::e_DEADLINE),
                    0);
                if (timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED)) {
                    NTSCFG_TEST_EQ(
                        timerSession2->count(ntca::TimerEventType::e_CANCELED),
                        1);
                }

                // Ensure the timer at t3 fired and was not canceled.

                NTSCFG_TEST_EQ(
                    timerSession3->count(ntca::TimerEventType::e_DEADLINE),
                    1);
                if (timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED)) {
                    NTSCFG_TEST_EQ(
                        timerSession3->count(ntca::TimerEventType::e_CANCELED),
                        0);
                }

                // Ensure all three timers are closed.

                if (timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED)) {
                    NTSCFG_TEST_EQ(
                        timerSession1->count(ntca::TimerEventType::e_CLOSED),
                        1);
                    NTSCFG_TEST_EQ(
                        timerSession2->count(ntca::TimerEventType::e_CLOSED),
                        1);
                    NTSCFG_TEST_EQ(
                        timerSession3->count(ntca::TimerEventType::e_CLOSED),
                        1);
                }

                // Deregister the waiter.

                proactor->deregisterWaiter(waiter);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntcs::IoRingTest::verifyCase4)
{
    ntsa::Error error;

    // Create the user.

    bsl::shared_ptr<ntci::User> user;

    // Create the proactor.

    ntca::ProactorConfig proactorConfig;

    proactorConfig.setMetricName("test");
    proactorConfig.setMinThreads(1);
    proactorConfig.setMaxThreads(1);

    bsl::shared_ptr<ntco::IoRingFactory> proactorFactory;
    proactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Proactor> proactor =
        proactorFactory->createProactor(proactorConfig, user, NTSCFG_TEST_ALLOCATOR);

    // Register this thread as a thread that will wait on the proactor.

    ntci::Waiter waiter = proactor->registerWaiter(ntca::WaiterOptions());

    // Defer a function to execute.

    bslmt::Latch latch(1);
    proactor->execute(NTCCFG_BIND(&processFunction, &latch));

    while (!latch.tryWait()) {
        proactor->poll(waiter);
    }

    // Deregister the waiter.

    proactor->deregisterWaiter(waiter);
}

}  // close namespace ntco
}  // close namespace BloombergLP

#endif


