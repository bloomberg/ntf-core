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

#include <ntco_iocp.h>

#if NTC_BUILD_WITH_IOCP

#include <ntccfg_test.h>

#include <ntccfg_bind.h>
#include <ntci_log.h>
#include <ntci_proactor.h>
#include <ntci_proactorsocket.h>
#include <ntsf_system.h>
#include <bdlb_guid.h>
#include <bdlb_guidutil.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobutil.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bslmt_barrier.h>
#include <bslmt_latch.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>
#include <bsls_stopwatch.h>
#include <bsls_timeinterval.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_list.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>

// Uncomment to enable testing of shutting down writes.
#define NTCD_PROACTOR_TEST_SHUTDOWN_WRITE 1

// Uncomment to enable testing of shutting down reads.
//
// IMPLEMENTATION NOTE: Some read operating systems or third-party
// implementations of the proactor model never fail nor complete a read
// initiated after the socket has been announced to have been shutdown, so this
// test cannot pass without some sort of emulation of the desired behavior.
#define NTCD_PROACTOR_TEST_SHUTDOWN_READ 0

using namespace BloombergLP;

namespace test {
namespace case1 {

class ProactorStreamSocket;
class ProactorListenerSocket;

class ProactorStreamSocket : public ntci::ProactorSocket,
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

    ntsa::Transport::Value transport() const BSLS_KEYWORD_OVERRIDE;
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

void ProactorStreamSocket::processSocketAccepted(
    const ntsa::Error&                         asyncError,
    const bsl::shared_ptr<ntsi::StreamSocket>& streamSocket)
{
    NTCCFG_TEST_TRUE(false);
}

void ProactorStreamSocket::processSocketConnected(
    const ntsa::Error& asyncError)
{
    if (asyncError) {
        if (asyncError == ntsa::Error::e_CANCELLED) {
            return;
        }

        NTCCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor "
                              << d_handle << " at " << d_sourceEndpoint
                              << " failed to connect: " << asyncError
                              << NTCCFG_TEST_LOG_END;

        if (d_abortOnErrorFlag) {
            NTCCFG_TEST_EQ(asyncError, ntsa::Error::e_OK);
        }

        d_lastError = asyncError;

        d_errorSemaphore.post();

        if (d_errorCallback) {
            d_errorCallback(asyncError);
        }

        return;
    }

    NTCCFG_TEST_FALSE(asyncError);

    ntsa::Error error;

    ntsa::Endpoint sourceEndpoint;
    error = d_streamSocket_sp->sourceEndpoint(&sourceEndpoint);
    NTCCFG_TEST_OK(error);

    ntsa::Endpoint remoteEndpoint;
    error = d_streamSocket_sp->remoteEndpoint(&remoteEndpoint);
    NTCCFG_TEST_OK(error);

    NTCCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << sourceEndpoint << " to "
                          << remoteEndpoint << " connected"
                          << NTCCFG_TEST_LOG_END;

    d_sourceEndpoint = sourceEndpoint;
    d_remoteEndpoint = remoteEndpoint;

    d_connectSemaphore.post();

    if (d_connectCallback) {
        d_connectCallback();
    }
}

void ProactorStreamSocket::processSocketReceived(
    const ntsa::Error&          asyncError,
    const ntsa::ReceiveContext& context)
{
    if (asyncError) {
        if (asyncError == ntsa::Error::e_CANCELLED) {
            return;
        }

        if (asyncError == ntsa::Error(ntsa::Error::e_EOF)) {
            NTCCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor "
                                  << d_handle << " at " << d_sourceEndpoint
                                  << " to " << d_remoteEndpoint << " shutdown"
                                  << NTCCFG_TEST_LOG_END;

            d_shutdownSemaphore.post();

            if (d_shutdownCallback) {
                d_shutdownCallback();
            }
        }
        else {
            NTCCFG_TEST_LOG_DEBUG
                << "Proactor stream socket descriptor " << d_handle << " at "
                << d_sourceEndpoint << " to " << d_remoteEndpoint
                << " failed to receive: " << asyncError << NTCCFG_TEST_LOG_END;

            if (d_abortOnErrorFlag) {
                NTCCFG_TEST_EQ(asyncError, ntsa::Error::e_OK);
            }

            d_lastError = asyncError;

            d_errorSemaphore.post();

            if (d_errorCallback) {
                d_errorCallback(asyncError);
            }
        }

        return;
    }

    NTCCFG_TEST_FALSE(asyncError);

    NTCCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " received "
                          << context.bytesReceived() << "/"
                          << context.bytesReceivable()
                          << (context.bytesReceivable() == 1 ? " byte"
                                                             : " bytes")
                          << NTCCFG_TEST_LOG_END;

    NTCCFG_TEST_TRUE(d_receiveData_sp);
    bsl::shared_ptr<bdlbb::Blob> data = d_receiveData_sp;
    d_receiveData_sp.reset();

    if (context.bytesReceived() == 0) {
        NTCCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor "
                              << d_handle << " at " << d_sourceEndpoint
                              << " to " << d_remoteEndpoint << " shutdown"
                              << NTCCFG_TEST_LOG_END;

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

void ProactorStreamSocket::processSocketSent(const ntsa::Error& asyncError,
                                             const ntsa::SendContext& context)
{
    if (asyncError) {
        if (asyncError == ntsa::Error::e_CANCELLED) {
            return;
        }

        NTCCFG_TEST_LOG_DEBUG
            << "Proactor stream socket descriptor " << d_handle << " at "
            << d_sourceEndpoint << " to " << d_remoteEndpoint
            << " failed to send: " << asyncError << NTCCFG_TEST_LOG_END;

        if (d_abortOnErrorFlag) {
            NTCCFG_TEST_EQ(asyncError, ntsa::Error::e_OK);
        }

        d_lastError = asyncError;

        d_errorSemaphore.post();

        if (d_errorCallback) {
            d_errorCallback(asyncError);
        }

        return;
    }

    NTCCFG_TEST_FALSE(asyncError);

    NTCCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " sent "
                          << context.bytesSent() << "/"
                          << context.bytesSendable()
                          << (context.bytesSendable() == 1 ? " byte"
                                                           : " bytes")
                          << NTCCFG_TEST_LOG_END;

    NTCCFG_TEST_TRUE(d_sendData_sp);
    bsl::shared_ptr<bdlbb::Blob> data = d_sendData_sp;
    d_sendData_sp.reset();

    d_sendSemaphore.post();

    if (d_sendCallback) {
        d_sendCallback(data, context.bytesSent());
    }
}

void ProactorStreamSocket::processSocketError(const ntsa::Error& error)
{
    NTCCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " error: " << error
                          << NTCCFG_TEST_LOG_END;

    if (d_abortOnErrorFlag) {
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
    }

    d_lastError = error;

    d_errorSemaphore.post();

    if (d_errorCallback) {
        d_errorCallback(error);
    }
}

void ProactorStreamSocket::processSocketDetached()
{
    NTCCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " is detached"
                          << NTCCFG_TEST_LOG_END;

    d_detachSemaphore.post();

    if (d_detachCallback) {
        d_detachCallback();
    }
}

void ProactorStreamSocket::close()
{
    d_streamSocket_sp->close();
}

bool ProactorStreamSocket::isStream() const
{
    return true;
}

bool ProactorStreamSocket::isDatagram() const
{
    return false;
}

bool ProactorStreamSocket::isListener() const
{
    return false;
}

ntsa::Transport::Value ProactorStreamSocket::transport() const
{
    return ntsa::Transport::e_TCP_IPV4_STREAM;
}

const bsl::shared_ptr<ntci::Strand>& ProactorStreamSocket::strand() const
{
    return d_strand_sp;
}

ntsa::Handle ProactorStreamSocket::handle() const
{
    return d_handle;
}

ProactorStreamSocket::ProactorStreamSocket(
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
    NTCCFG_TEST_OK(error);

    d_handle = d_streamSocket_sp->handle();

    error = d_streamSocket_sp->setBlocking(false);
    NTCCFG_TEST_OK(error);

    error =
        d_streamSocket_sp->bind(ntsa::Endpoint(ntsa::IpEndpoint(
                                    ntsa::IpEndpoint::loopbackIpv4Address(),
                                    ntsa::IpEndpoint::anyPort())),
                                true);
    NTCCFG_TEST_OK(error);

    error = d_streamSocket_sp->sourceEndpoint(&d_sourceEndpoint);
    NTCCFG_TEST_OK(error);

    if (d_proactor_sp->maxThreads() > 1) {
        d_strand_sp = d_proactor_sp->createStrand();
    }

    NTCCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " created"
                          << NTCCFG_TEST_LOG_END;
}

ProactorStreamSocket::ProactorStreamSocket(
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
    NTCCFG_TEST_OK(error);

    error = d_streamSocket_sp->sourceEndpoint(&d_sourceEndpoint);
    NTCCFG_TEST_OK(error);

    error = d_streamSocket_sp->remoteEndpoint(&d_remoteEndpoint);
    NTCCFG_TEST_OK(error);

    if (d_proactor_sp->maxThreads() > 1) {
        d_strand_sp = d_proactor_sp->createStrand();
    }

    NTCCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " created"
                          << NTCCFG_TEST_LOG_END;
}

ProactorStreamSocket::~ProactorStreamSocket()
{
    d_streamSocket_sp.reset();
    d_proactor_sp.reset();

    NTCCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " destroyed"
                          << NTCCFG_TEST_LOG_END;
}

void ProactorStreamSocket::setConnectCallback(const ConnectCallback& callback)
{
    d_connectCallback = callback;
}

void ProactorStreamSocket::setSendCallback(const SendCallback& callback)
{
    d_sendCallback = callback;
}

void ProactorStreamSocket::setReceiveCallback(const ReceiveCallback& callback)
{
    d_receiveCallback = callback;
}

void ProactorStreamSocket::setShutdownCallback(
    const ShutdownCallback& callback)
{
    d_shutdownCallback = callback;
}

void ProactorStreamSocket::setErrorCallback(const ErrorCallback& callback)
{
    d_errorCallback = callback;
}

void ProactorStreamSocket::setDetachCallback(const DetachCallback& callback)
{
    d_detachCallback = callback;
}

ntsa::Error ProactorStreamSocket::bind(const ntsa::Endpoint& sourceEndpoint)
{
    ntsa::Error error;

    error = d_streamSocket_sp->bind(sourceEndpoint, false);
    if (error) {
        return error;
    }

    error = d_streamSocket_sp->sourceEndpoint(&d_sourceEndpoint);
    NTCCFG_TEST_OK(error);

    NTCCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " is bound"
                          << NTCCFG_TEST_LOG_END;

    return ntsa::Error();
}

ntsa::Error ProactorStreamSocket::connect(const ntsa::Endpoint& remoteEndpoint)
{
    d_remoteEndpoint = remoteEndpoint;

    NTCCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " is connecting"
                          << NTCCFG_TEST_LOG_END;

    bsl::shared_ptr<ProactorStreamSocket> self = this->getSelf(this);

    return d_proactor_sp->connect(self, remoteEndpoint);
}

ntsa::Error ProactorStreamSocket::send(
    const bsl::shared_ptr<bdlbb::Blob>& data)
{
    NTCCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " sending up to "
                          << data->length()
                          << (data->length() == 1 ? " byte" : " bytes")
                          << NTCCFG_TEST_LOG_END;

    bsl::shared_ptr<ProactorStreamSocket> self = this->getSelf(this);

    NTCCFG_TEST_FALSE(d_sendData_sp);
    d_sendData_sp = data;

    return d_proactor_sp->send(self, *data, ntsa::SendOptions());
}

ntsa::Error ProactorStreamSocket::receive(
    const bsl::shared_ptr<bdlbb::Blob>& data)
{
    NTCCFG_TEST_LOG_DEBUG << "Proactor stream socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " to "
                          << d_remoteEndpoint << " receiving up to "
                          << data->totalSize() - data->length()
                          << (data->totalSize() - data->length() == 1
                                  ? " byte"
                                  : " bytes")
                          << NTCCFG_TEST_LOG_END;

    bsl::shared_ptr<ProactorStreamSocket> self = this->getSelf(this);

    NTCCFG_TEST_FALSE(d_receiveData_sp);
    d_receiveData_sp = data;

    return d_proactor_sp->receive(self, data.get(), ntsa::ReceiveOptions());
}

ntsa::Error ProactorStreamSocket::shutdown(ntsa::ShutdownType::Value direction)
{
    bsl::shared_ptr<ProactorStreamSocket> self = this->getSelf(this);

    return d_proactor_sp->shutdown(self, direction);
}

void ProactorStreamSocket::abortOnError(bool value)
{
    d_abortOnErrorFlag = value;
}

void ProactorStreamSocket::waitForConnected()
{
    d_connectSemaphore.wait();
}

void ProactorStreamSocket::waitForSent()
{
    d_sendSemaphore.wait();
}

void ProactorStreamSocket::waitForReceived()
{
    d_receiveSemaphore.wait();
}

void ProactorStreamSocket::waitForShutdown()
{
    d_shutdownSemaphore.wait();
}

void ProactorStreamSocket::waitForError()
{
    d_errorSemaphore.wait();
}

void ProactorStreamSocket::waitForDetached()
{
    d_detachSemaphore.wait();
}

bool ProactorStreamSocket::pollForConnected()
{
    return d_connectSemaphore.tryWait() == 0;
}

bool ProactorStreamSocket::pollForSent()
{
    return d_sendSemaphore.tryWait() == 0;
}

bool ProactorStreamSocket::pollForReceived()
{
    return d_receiveSemaphore.tryWait() == 0;
}

bool ProactorStreamSocket::pollForShutdown()
{
    return d_shutdownSemaphore.tryWait() == 0;
}

bool ProactorStreamSocket::pollForError()
{
    return d_errorSemaphore.tryWait() == 0;
}

bool ProactorStreamSocket::pollForDetached()
{
    return d_detachSemaphore.tryWait() == 0;
}

ntsa::Endpoint ProactorStreamSocket::sourceEndpoint() const
{
    return d_sourceEndpoint;
}

ntsa::Endpoint ProactorStreamSocket::remoteEndpoint() const
{
    return d_remoteEndpoint;
}

ntsa::Error ProactorStreamSocket::lastError() const
{
    return d_lastError;
}

class ProactorListenerSocket : public ntci::ProactorSocket,
                               public ntccfg::Shared<ProactorListenerSocket>
{
    // Provide an implementation of a proactor listener socket for
    // use by this test driver. This class is thread safe.

  public:
    typedef NTCCFG_FUNCTION(
        const bsl::shared_ptr<test::case1::ProactorStreamSocket>&
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
    typedef bsl::list<bsl::shared_ptr<test::case1::ProactorStreamSocket> >
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

    ntsa::Transport::Value transport() const BSLS_KEYWORD_OVERRIDE;
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

    ~ProactorListenerSocket() BSLS_KEYWORD_OVERRIDE;
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

    bsl::shared_ptr<test::case1::ProactorStreamSocket> accepted();
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

void ProactorListenerSocket::processSocketAccepted(
    const ntsa::Error&                         asyncError,
    const bsl::shared_ptr<ntsi::StreamSocket>& streamSocket)
{
    if (asyncError) {
        if (asyncError == ntsa::Error::e_CANCELLED) {
            return;
        }

        NTCCFG_TEST_LOG_DEBUG << "Proactor socket descriptor " << d_handle
                              << " at " << d_sourceEndpoint
                              << " failed to accept: " << asyncError
                              << NTCCFG_TEST_LOG_END;

        if (d_abortOnErrorFlag) {
            NTCCFG_TEST_EQ(asyncError, ntsa::Error::e_OK);
        }

        d_lastError = asyncError;

        d_errorSemaphore.post();

        if (d_errorCallback) {
            d_errorCallback(asyncError);
        }

        return;
    }

    NTCCFG_TEST_FALSE(asyncError);

    ntsa::Error error;

    error = streamSocket->setBlocking(false);
    NTCCFG_TEST_OK(error);

    ntsa::Endpoint sourceEndpoint;
    error = streamSocket->sourceEndpoint(&sourceEndpoint);
    NTCCFG_TEST_OK(error);

    ntsa::Endpoint remoteEndpoint;
    error = streamSocket->remoteEndpoint(&remoteEndpoint);
    NTCCFG_TEST_OK(error);

    NTCCFG_TEST_LOG_DEBUG << "Proactor listener socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint
                          << " accepted proactor stream socket descriptor "
                          << streamSocket->handle() << " at " << sourceEndpoint
                          << " to " << remoteEndpoint << NTCCFG_TEST_LOG_END;

    bsl::shared_ptr<test::case1::ProactorStreamSocket> proactorSocket;
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

void ProactorListenerSocket::processSocketConnected(
    const ntsa::Error& asyncError)
{
    NTCCFG_TEST_TRUE(false);
}

void ProactorListenerSocket::processSocketReceived(
    const ntsa::Error&          asyncError,
    const ntsa::ReceiveContext& context)
{
    NTCCFG_TEST_TRUE(false);
}

void ProactorListenerSocket::processSocketSent(
    const ntsa::Error&       asyncError,
    const ntsa::SendContext& context)
{
    NTCCFG_TEST_TRUE(false);
}

void ProactorListenerSocket::processSocketError(const ntsa::Error& error)
{
    NTCCFG_TEST_LOG_DEBUG << "Proactor listener socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " error: " << error
                          << NTCCFG_TEST_LOG_END;

    if (d_abortOnErrorFlag) {
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);
    }

    d_lastError = error;

    d_errorSemaphore.post();

    if (d_errorCallback) {
        d_errorCallback(error);
    }
}

void ProactorListenerSocket::processSocketDetached()
{
    NTCCFG_TEST_LOG_DEBUG << "Proactor listener socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " is detached"
                          << NTCCFG_TEST_LOG_END;

    d_detachSemaphore.post();

    if (d_detachCallback) {
        d_detachCallback();
    }
}

void ProactorListenerSocket::close()
{
    d_listenerSocket_sp->close();
}

bool ProactorListenerSocket::isStream() const
{
    return true;
}

bool ProactorListenerSocket::isDatagram() const
{
    return false;
}

bool ProactorListenerSocket::isListener() const
{
    return true;
}

ntsa::Transport::Value ProactorListenerSocket::transport() const
{
    return ntsa::Transport::e_TCP_IPV4_STREAM;
}

const bsl::shared_ptr<ntci::Strand>& ProactorListenerSocket::strand() const
{
    return d_strand_sp;
}

ntsa::Handle ProactorListenerSocket::handle() const
{
    return d_handle;
}

ProactorListenerSocket::ProactorListenerSocket(
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
    NTCCFG_TEST_OK(error);

    d_handle = d_listenerSocket_sp->handle();

    error = d_listenerSocket_sp->setBlocking(false);
    NTCCFG_TEST_OK(error);

    error =
        d_listenerSocket_sp->bind(ntsa::Endpoint(ntsa::IpEndpoint(
                                      ntsa::IpEndpoint::loopbackIpv4Address(),
                                      ntsa::IpEndpoint::anyPort())),
                                  false);
    NTCCFG_TEST_OK(error);

    error = d_listenerSocket_sp->sourceEndpoint(&d_sourceEndpoint);
    NTCCFG_TEST_OK(error);

    if (d_proactor_sp->maxThreads() > 1) {
        d_strand_sp = d_proactor_sp->createStrand();
    }

    NTCCFG_TEST_LOG_DEBUG << "Proactor listener socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " created"
                          << NTCCFG_TEST_LOG_END;
}

ProactorListenerSocket::~ProactorListenerSocket()
{
    d_acceptQueue.clear();

    d_listenerSocket_sp.reset();
    d_proactor_sp.reset();

    NTCCFG_TEST_LOG_DEBUG << "Proactor listener socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " destroyed"
                          << NTCCFG_TEST_LOG_END;
}

void ProactorListenerSocket::setAcceptCallback(const AcceptCallback& callback)
{
    d_acceptCallback = callback;
}

void ProactorListenerSocket::setErrorCallback(const ErrorCallback& callback)
{
    d_errorCallback = callback;
}

void ProactorListenerSocket::setDetachCallback(const DetachCallback& callback)
{
    d_detachCallback = callback;
}

ntsa::Error ProactorListenerSocket::bind(const ntsa::Endpoint& sourceEndpoint)
{
    ntsa::Error error;

    error = d_listenerSocket_sp->bind(sourceEndpoint, false);
    if (error) {
        return error;
    }

    error = d_listenerSocket_sp->sourceEndpoint(&d_sourceEndpoint);
    NTCCFG_TEST_OK(error);

    NTCCFG_TEST_LOG_DEBUG << "Proactor listener socket descriptor " << d_handle
                          << "at " << d_sourceEndpoint << " is bound"
                          << NTCCFG_TEST_LOG_END;

    return ntsa::Error();
}

ntsa::Error ProactorListenerSocket::listen()
{
    NTCCFG_TEST_LOG_DEBUG << "Proactor listener socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " is listening"
                          << NTCCFG_TEST_LOG_END;

    return d_listenerSocket_sp->listen(1);
}

ntsa::Error ProactorListenerSocket::accept()
{
    NTCCFG_TEST_LOG_DEBUG << "Proactor listener socket descriptor " << d_handle
                          << " at " << d_sourceEndpoint << " initiating accept"
                          << NTCCFG_TEST_LOG_END;

    bsl::shared_ptr<ProactorSocket> self = this->getSelf(this);

    return d_proactor_sp->accept(self);
}

bsl::shared_ptr<test::case1::ProactorStreamSocket> ProactorListenerSocket::
    accepted()
{
    NTCCFG_TEST_FALSE(d_acceptQueue.empty());

    bsl::shared_ptr<test::case1::ProactorStreamSocket> result =
        d_acceptQueue.front();
    d_acceptQueue.pop_front();

    return result;
}

void ProactorListenerSocket::abortOnError(bool value)
{
    d_abortOnErrorFlag = value;
}

void ProactorListenerSocket::waitForAccepted()
{
    d_acceptSemaphore.wait();
}

void ProactorListenerSocket::waitForError()
{
    d_errorSemaphore.wait();
}

void ProactorListenerSocket::waitForDetached()
{
    d_detachSemaphore.wait();
}

bool ProactorListenerSocket::pollForAccepted()
{
    return d_acceptSemaphore.tryWait() == 0;
}

bool ProactorListenerSocket::pollForError()
{
    return d_errorSemaphore.tryWait() == 0;
}

bool ProactorListenerSocket::pollForDetached()
{
    return d_detachSemaphore.tryWait() == 0;
}

ntsa::Endpoint ProactorListenerSocket::sourceEndpoint() const
{
    return d_sourceEndpoint;
}

ntsa::Error ProactorListenerSocket::lastError() const
{
    return d_lastError;
}

}  // close namespace case1
}  // close test namespace

NTCCFG_TEST_CASE(1)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;

        // Create the blob buffer factory.

        bdlbb::PooledBlobBufferFactory blobBufferFactory(32, &ta);

        // Define the user.

        bsl::shared_ptr<ntci::User> user;

        // Create the proactor.

        ntca::ProactorConfig proactorConfig;
        proactorConfig.setMetricName("test");
        proactorConfig.setMinThreads(1);
        proactorConfig.setMaxThreads(1);

        bsl::shared_ptr<ntco::IocpFactory> proactorFactory;
        proactorFactory.createInplace(&ta, &ta);

        bsl::shared_ptr<ntci::Proactor> proactor =
            proactorFactory->createProactor(proactorConfig, user, &ta);

        // Register this thread as the thread that will wait on the
        // proactor.

        ntci::Waiter waiter = proactor->registerWaiter(ntca::WaiterOptions());

        // Create a TCP/IPv4 non-blocking socket bound to any ephemeral
        // port on the IPv4 loopback address. This socket will act as the
        // listener.

        bsl::shared_ptr<test::case1::ProactorListenerSocket> listener;
        listener.createInplace(&ta, proactor, &ta);

        listener->abortOnError(true);

        // Begin listening for connections.

        error = listener->listen();
        NTCCFG_TEST_OK(error);

        // Attach the listener socket to the proactor.

        error = proactor->attachSocket(listener);
        NTCCFG_TEST_OK(error);

        // Create a TCP/IPv4 non-blocking socket. This socket will act as
        // the client.

        bsl::shared_ptr<test::case1::ProactorStreamSocket> client;
        client.createInplace(&ta, proactor, &ta);

        client->abortOnError(true);

        // Attach the client socket to the proactor.

        error = proactor->attachSocket(client);
        NTCCFG_TEST_OK(error);

        // Asynchronously accept the next connection.

        error = listener->accept();
        NTCCFG_TEST_OK(error);

        // Asynchronously connect the client socket to the listener's local
        // endpoint.

        ntsa::Endpoint serverEndpoint = listener->sourceEndpoint();
        serverEndpoint.ip().setHost(ntsa::Ipv4Address::loopback());

        error = client->connect(listener->sourceEndpoint());
        NTCCFG_TEST_OK(error);

        // Wait for the listener to accept the connection from the client
        // and pop that socket off the accept queue. This socket will
        // act as the server socket.

        while (!listener->pollForAccepted()) {
            proactor->poll(waiter);
        }

        bsl::shared_ptr<test::case1::ProactorStreamSocket> server =
            listener->accepted();

        server->abortOnError(true);

        // Attach the server socket to the proactor.

        error = proactor->attachSocket(server);
        NTCCFG_TEST_OK(error);

        // Wait for the client to become connected.

        while (!client->pollForConnected()) {
            proactor->poll(waiter);
        }

        // Send a single byte to the server.

        {
            bsl::shared_ptr<bdlbb::Blob> data;
            data.createInplace(&ta, &blobBufferFactory, &ta);

            bdlbb::BlobUtil::append(data.get(), "X", 1);

            error = client->send(data);
            NTCCFG_TEST_OK(error);
        }

        // Wait for the data to be copied to the client's socket send
        // buffer.

        while (!client->pollForSent()) {
            proactor->poll(waiter);
        }

        // Receive a single byte from the client.

        {
            bsl::shared_ptr<bdlbb::Blob> data;
            data.createInplace(&ta, &blobBufferFactory, &ta);

            data->setLength(1);
            data->setLength(0);

            error = server->receive(data);
            NTCCFG_TEST_OK(error);
        }

        // Wait for the data to be copied from the server's socket receive
        // buffer.

        while (!server->pollForReceived()) {
            proactor->poll(waiter);
        }

#if NTCD_PROACTOR_TEST_SHUTDOWN_WRITE

        client->abortOnError(false);
        server->abortOnError(false);

        // Shutdown writing from the server.

        error = server->shutdown(ntsa::ShutdownType::e_SEND);
        NTCCFG_TEST_OK(error);

        // Try to send a single byte to the client, but observe the send
        // fails because the server has shutdown writing from its side of
        // the connection.

        {
            bsl::shared_ptr<bdlbb::Blob> data;
            data.createInplace(&ta, &blobBufferFactory, &ta);

            bdlbb::BlobUtil::append(data.get(), "X", 1);

            error = server->send(data);
            if (error) {
                NTCCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
            }
            else {
                // Wait until the send attempt results in an error.

                while (!server->pollForError()) {
                    proactor->poll(waiter);
                }

                error = server->lastError();
                NTCCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
            }
        }

        // Receive a single byte from the server, but observe that zero
        // bytes have been successfully read, indicating the server has
        // shutdown writing from its side of the connection.

        {
            bsl::shared_ptr<bdlbb::Blob> data;
            data.createInplace(&ta, &blobBufferFactory, &ta);

            data->setLength(1);
            data->setLength(0);

            error = client->receive(data);
            NTCCFG_TEST_OK(error);
        }

        // Wait for the client detects the socket has been shutdown.

        while (!client->pollForShutdown()) {
            proactor->poll(waiter);
        }

        // Shutdown writing from the client.

        error = client->shutdown(ntsa::ShutdownType::e_SEND);
        NTCCFG_TEST_OK(error);

        // Try to send a single byte to the server, but observe the send
        // fails because the client has shutdown writing from its side of
        // the connection.

        {
            bsl::shared_ptr<bdlbb::Blob> data;
            data.createInplace(&ta, &blobBufferFactory, &ta);

            bdlbb::BlobUtil::append(data.get(), "X", 1);

            error = client->send(data);

            if (error) {
                NTCCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
            }
            else {
                // Wait until the send attempt results in an error.

                while (!client->pollForError()) {
                    proactor->poll(waiter);
                }

                error = client->lastError();
                NTCCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
            }
        }

        // Receive a single byte from the client, but observe that zero
        // bytes have been successfully read, indicating the client has
        // shutdown writing from its side of the connection.

        {
            bsl::shared_ptr<bdlbb::Blob> data;
            data.createInplace(&ta, &blobBufferFactory, &ta);

            data->setLength(1);
            data->setLength(0);

            error = server->receive(data);
            NTCCFG_TEST_OK(error);
        }

        // Wait for the server detects the socket has been shutdown.

        while (!server->pollForShutdown()) {
            proactor->poll(waiter);
        }

#endif

#if NTCD_PROACTOR_TEST_SHUTDOWN_READ

        // Shutdown reading from the server.

        error = server->shutdown(ntsa::ShutdownType::e_RECEIVE);
        NTCCFG_TEST_OK(error);

        // Try to receive a single byte from the client, and observe the
        // receive fails indicating the connection is dead.

        {
            bsl::shared_ptr<bdlbb::Blob> data;
            data.createInplace(&ta, &blobBufferFactory, &ta);

            data->setLength(1);
            data->setLength(0);

            error = server->receive(data);
            if (error) {
                NTCCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
            }
            else {
                while (!server->pollForError()) {
                    proactor->poll(waiter);
                }

                error = server->lastError();
                NTCCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
            }
        }

        // Shutdown reading from the client.

        error = client->shutdown(ntsa::ShutdownType::e_RECEIVE);
        NTCCFG_TEST_OK(error);

        // Try to receive a single byte from the server, and observe the
        // receive fails indicating the connection is dead.

        {
            bsl::shared_ptr<bdlbb::Blob> data;
            data.createInplace(&ta, &blobBufferFactory, &ta);

            data->setLength(1);
            data->setLength(0);

            error = client->receive(data);
            if (error) {
                NTCCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
            }
            else {
                while (!client->pollForError()) {
                    proactor->poll(waiter);
                }

                error = client->lastError();
                NTCCFG_TEST_EQ(error, ntsa::Error::e_CONNECTION_DEAD);
            }
        }

#endif

        // Detach the server from the proactor.

        error = proactor->detachSocketAsync(server);
        NTCCFG_TEST_OK(error);

        // Wait for the server to become detached from the proactor.

        while (!server->pollForDetached()) {
            proactor->poll(waiter);
        }

        // Detach the client from the proactor.

        error = proactor->detachSocketAsync(client);
        NTCCFG_TEST_OK(error);

        // Wait for the client to become detached from the proactor.

        while (!client->pollForDetached()) {
            proactor->poll(waiter);
        }

        // Detach the listener from the proactor.

        proactor->detachSocketAsync(listener);
        NTCCFG_TEST_OK(error);

        // Wait for the listener to become detached from the proactor.

        while (!listener->pollForDetached()) {
            proactor->poll(waiter);
        }

        // Deregister the waiter.

        proactor->deregisterWaiter(waiter);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

namespace test {
namespace case2 {

class TimerSession : public ntci::TimerSession
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

void TimerSession::processTimerDeadline(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_TEST_EQ(event.type(), ntca::TimerEventType::e_DEADLINE);

    NTCCFG_TEST_LOG_DEBUG << "Timer '" << d_name << "' deadline"
                          << NTCCFG_TEST_LOG_END;

    d_deadline.arrive();
}

void TimerSession::processTimerCancelled(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_TEST_EQ(event.type(), ntca::TimerEventType::e_CANCELED);

    NTCCFG_TEST_LOG_DEBUG << "Timer '" << d_name << "' cancelled"
                          << NTCCFG_TEST_LOG_END;

    d_cancelled.arrive();
}

void TimerSession::processTimerClosed(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntca::TimerEvent&             event)
{
    NTCCFG_TEST_EQ(event.type(), ntca::TimerEventType::e_CLOSED);

    NTCCFG_TEST_LOG_DEBUG << "Timer '" << d_name << "' closed"
                          << NTCCFG_TEST_LOG_END;

    d_closed.arrive();
}

const bsl::shared_ptr<ntci::Strand>& TimerSession::strand() const
{
    return ntci::Strand::unspecified();
}

TimerSession::TimerSession(const bsl::string& name,
                           bslma::Allocator*  basicAllocator)
: d_name(name, basicAllocator)
, d_deadline(1)
, d_cancelled(1)
, d_closed(1)
{
}

TimerSession::~TimerSession()
{
}

void TimerSession::wait(ntca::TimerEventType::Value timerEventType)
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
        NTCCFG_TEST_TRUE(false);
    }
}

bool TimerSession::tryWait(ntca::TimerEventType::Value timerEventType)
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
        NTCCFG_TEST_TRUE(false);
        return false;
    }
}

bool TimerSession::has(ntca::TimerEventType::Value timerEventType)
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
        NTCCFG_TEST_TRUE(false);
        return 0;
    }
}

bsl::size_t TimerSession::count(ntca::TimerEventType::Value timerEventType)
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
        NTCCFG_TEST_TRUE(false);
        return 0;
    }
}

}  // close namespace case2
}  // close namespace test

NTCCFG_TEST_CASE(2)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

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

            NTCCFG_TEST_LOG_INFO << "Testing timer options = " << timerOptions
                                 << NTCCFG_TEST_LOG_END;

            ntccfg::TestAllocator ta;
            {
                ntsa::Error error;

                // Define the user.

                bsl::shared_ptr<ntci::User> user;

                // Create the proactor.

                ntca::ProactorConfig proactorConfig;
                proactorConfig.setMetricName("test");
                proactorConfig.setMinThreads(1);
                proactorConfig.setMaxThreads(1);

                bsl::shared_ptr<ntco::IocpFactory> proactorFactory;
                proactorFactory.createInplace(&ta, &ta);

                bsl::shared_ptr<ntci::Proactor> proactor =
                    proactorFactory->createProactor(proactorConfig, user, &ta);

                // Register this thread as a thread that will wait on the
                // proactor.

                ntci::Waiter waiter =
                    proactor->registerWaiter(ntca::WaiterOptions());

                // Register three timers to fire at t1, t2, and t3. Implement the
                // first timer so that when it fires at t1, it cancels the timer
                // at t2.

                bslma::Allocator* allocator =
                    bslma::Default::defaultAllocator();

                bsl::shared_ptr<test::case2::TimerSession> timerSession1;
                timerSession1.createInplace(&ta, "timer1", &ta);

                bsl::shared_ptr<test::case2::TimerSession> timerSession2;
                timerSession2.createInplace(&ta, "timer2", &ta);

                bsl::shared_ptr<test::case2::TimerSession> timerSession3;
                timerSession3.createInplace(&ta, "timer3", &ta);

                bsl::shared_ptr<ntci::Timer> timer1 = proactor->createTimer(
                    timerOptions,
                    static_cast<bsl::shared_ptr<ntci::TimerSession> >(
                        timerSession1),
                    &ta);

                bsl::shared_ptr<ntci::Timer> timer2 = proactor->createTimer(
                    timerOptions,
                    static_cast<bsl::shared_ptr<ntci::TimerSession> >(
                        timerSession2),
                    &ta);

                bsl::shared_ptr<ntci::Timer> timer3 = proactor->createTimer(
                    timerOptions,
                    static_cast<bsl::shared_ptr<ntci::TimerSession> >(
                        timerSession3),
                    &ta);

                bsls::TimeInterval now = bdlt::CurrentTime::now();

                timer1->schedule(now + bsls::TimeInterval(1));
                timer2->schedule(now + bsls::TimeInterval(2));
                timer3->schedule(now + bsls::TimeInterval(3));

                // Wait for the first timer at t1 to fire.

                NTCCFG_TEST_TRUE(
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

                NTCCFG_TEST_TRUE(
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

                NTCCFG_TEST_EQ(
                    timerSession1->count(ntca::TimerEventType::e_DEADLINE),
                    1);
                if (timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED)) {
                    NTCCFG_TEST_EQ(
                        timerSession1->count(ntca::TimerEventType::e_CANCELED),
                        0);
                }

                // Ensure the timer at t2 did not fire, because it was cancelled
                // by the timer at t1.

                NTCCFG_TEST_EQ(
                    timerSession2->count(ntca::TimerEventType::e_DEADLINE),
                    0);
                if (timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED)) {
                    NTCCFG_TEST_EQ(
                        timerSession2->count(ntca::TimerEventType::e_CANCELED),
                        1);
                }

                // Ensure the timer at t3 fired and was not canceled.

                NTCCFG_TEST_EQ(
                    timerSession3->count(ntca::TimerEventType::e_DEADLINE),
                    1);
                if (timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED)) {
                    NTCCFG_TEST_EQ(
                        timerSession3->count(ntca::TimerEventType::e_CANCELED),
                        0);
                }

                // Ensure all three timers are closed.

                if (timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED)) {
                    NTCCFG_TEST_EQ(
                        timerSession1->count(ntca::TimerEventType::e_CLOSED),
                        1);
                    NTCCFG_TEST_EQ(
                        timerSession2->count(ntca::TimerEventType::e_CLOSED),
                        1);
                    NTCCFG_TEST_EQ(
                        timerSession3->count(ntca::TimerEventType::e_CLOSED),
                        1);
                }

                // Deregister the waiter.

                proactor->deregisterWaiter(waiter);
            }
            NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
        }
    }
}

namespace test {
namespace case3 {

void processFunction(bslmt::Latch* latch)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_DEBUG("Executed function");

    latch->arrive();
}

void execute(bslma::Allocator* allocator)
{
    ntsa::Error error;

    // Create the user.

    bsl::shared_ptr<ntci::User> user;

    // Create the proactor.

    ntca::ProactorConfig proactorConfig;

    proactorConfig.setMetricName("test");
    proactorConfig.setMinThreads(1);
    proactorConfig.setMaxThreads(1);

    bsl::shared_ptr<ntco::IocpFactory> proactorFactory;
    proactorFactory.createInplace(allocator, allocator);

    bsl::shared_ptr<ntci::Proactor> proactor =
        proactorFactory->createProactor(proactorConfig, user, allocator);

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

}  // close namespace case3
}  // close namespace test

NTCCFG_TEST_CASE(3)
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    ntccfg::TestAllocator ta;
    {
        test::case3::execute(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
}
NTCCFG_TEST_DRIVER_END;

#else

int main(int argc, char** argv)
{
    NTCCFG_WARNING_UNUSED(argc);
    NTCCFG_WARNING_UNUSED(argv);

    if (argc == 1) {
        return 0;
    }

    return -1;
}

#endif
