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

#include <ntccfg_bind.h>
#include <ntcf_system.h>
#include <bdlb_string.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobstreambuf.h>
#include <bdlbb_blobutil.h>
#include <bdlcc_objectcatalog.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bslmt_latch.h>
#include <bslmt_semaphore.h>
#include <bslx_genericinstream.h>
#include <bslx_genericoutstream.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_memory.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>

#define NTCU12_BUILD_WITH_TLS 0

using namespace BloombergLP;

namespace example {

//
// Implementing an Application Protocol Client and Server
//
// This example illustrates an implementation of a hypothetical application
// protocol to allow a client to request the uppercase version of a string
// from a server. The patterns in this implementation can form the basis of
// other production implementation of real world application protocols.
//
// Due to the nature of asynchronous APIs, this usage example is rather
// lengthy. For brevity, all error checking has been omitted. Production
// implementation should take care to honor all results from functions in this
// library and handle them as appropriate.
//
// All code common to all usage examples is placed into an 'example' namespace.
// Code that is particular to just this example is placed into a nested
// 'usage1' namespace.
//
// First, let's design an application protocol atop any of the
// connection-oriented, streaming protocols TCP/IPv4, TCP/IPv6, or Unix domain
// sockets. Our application protocol supports clients requesting the uppercase
// version of a string, and allows multiple messages to be simultanously
// multiplexed over the same connection. The simplest design of such
// an application protocol contains one field to uniquely identify the request
// in the connection, and one field to indicate the length of this field.
// Our application protocol message then has two parts: a fixed-length header
// part, and a variable-length payload part. The application protocol can be
// represented by the following diagram.
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |        Transaction ID         |         Payload Legnth        |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Network protocols historically encode integers in big endian byte-order,
// although either endianness can be chosen so long as one is explicitly
// specified. The 32-bit transaction ID and payload length integers in our
// application protocol will be transmitted in big-endian byte order. The
// fixed length application protocol header is immediately followed by the
// variable length string payload, representing the string the client is
// requesting the server to uppercase.
//
// Next, let's implement our application protocol in code. First, define the
// fixed length message header.
//

// This struct describes the header of a message in the example wire
// protocol.
struct MessageHeader {
    enum { e_LENGTH = 8 };

    int d_transactionId;  // The transaction identifier
    int d_payloadLength;  // The payload length
};

//
// Second, let's define the message itself, which consists of the fixed length
// header followed by the variable length payload string.
//

// This struct describes a message in the example wire protocol. A
// message consists of a fixed length header followed by a variable-length
// string.
struct Message {
    example::MessageHeader d_header;   // The fixed length header
    bsl::string            d_payload;  // The variable length payload
};

//
// Third, let's declare a utility to help encode and decode the parts of our
// application protocol.
//

// Provide functions for encoding the message header and
// payload of the example wire protocol.
struct MessageUtil {
    // Encode the specified 'header' to the specified 'result'.
    static void encodeHeader(bdlbb::Blob*                  result,
                             const example::MessageHeader& header);

    // Encode the specified 'payload' to the specified 'result'.
    static void encodePayload(bdlbb::Blob* result, const bsl::string& payload);

    // Decode the specified specified 'result' from the specified 'blob'.
    static void decodeHeader(example::MessageHeader* result,
                             const bdlbb::Blob&      blob);

    // Decode the specified specified 'result' from the specified 'blob'.
    static void decodePayload(bsl::string*       result,
                              const bdlbb::Blob& blob,
                              int                length);
};

//
// Now, let's implement the encoding and decoding functions in our utility.
//

void MessageUtil::encodeHeader(bdlbb::Blob*                  result,
                               const example::MessageHeader& header)
{
    bdlbb::OutBlobStreamBuf osb(result);
    {
        bslx::GenericOutStream<bdlbb::OutBlobStreamBuf> os(&osb, 1);
        os.putInt32(header.d_transactionId);
        os.putInt32(header.d_payloadLength);
    }

    osb.pubsync();
}

void MessageUtil::encodePayload(bdlbb::Blob*       result,
                                const bsl::string& payload)
{
    bdlbb::OutBlobStreamBuf osb(result);
    osb.sputn(payload.c_str(), payload.size());
    osb.pubsync();
}

void MessageUtil::decodeHeader(example::MessageHeader* result,
                               const bdlbb::Blob&      blob)
{
    bdlbb::InBlobStreamBuf isb(&blob);
    {
        bslx::GenericInStream<bdlbb::InBlobStreamBuf> is(&isb);
        is.getInt32(result->d_transactionId);
        is.getInt32(result->d_payloadLength);
    }
}

void MessageUtil::decodePayload(bsl::string*       result,
                                const bdlbb::Blob& blob,
                                int                length)
{
    bdlbb::InBlobStreamBuf isb(&blob);
    result->resize(length);
    isb.sgetn(&((*result)[0]), length);
}

//
// Next, let's declare a mechanism to parse a byte stream into the variable
// length messages framed according to our application protocol. Observe
// that the 'parse' function in our parsing mechanism has a particular
// signature that is friendly to the design of the consumption of data read
// from a connection, which will become relevant later.
//

// Provide a mechanism to parse messages in the wire protocol
// used in this example from a stream of bytes.
class MessageParser
{
    enum State { e_STATE_WANT_HEADER, e_STATE_WANT_PAYLOAD };

    State            d_state;
    example::Message d_message;

  private:
    MessageParser(const MessageParser&) BSLS_KEYWORD_DELETED;
    MessageParser& operator=(const MessageParser&) BSLS_KEYWORD_DELETED;

  public:
    // This typedef defines a callback function invoked when a message is
    // parsed.
    typedef bsl::function<void(const example::Message& message)>
        MessageCallback;

    // Create a new message parser.
    MessageParser();

    // Destroy this object.
    ~MessageParser();

    // Receive zero or more messages from the read queue of the specified
    // 'streamSocket'. If the read queue of the 'streamSocket' does not
    // contain sufficient bytes to parse the next portion of the message
    // protocol, set the read queue low watermark of the 'streamSocket' to
    // the minimum number of bytes required to be available in the read
    // queue before this function should be called again. Return the error.
    ntsa::Error parse(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                      const MessageCallback&                     callback);
};

//
// Finally, let's implement our parser. The parser contains a simple state
// machine that indicates whether the fixed-length header or the
// variable-length payload of the next message is being parsed. This state
// allows the implementation of this mechanism to communicate back to its
// callers how much more data is needed to transition to the next state.
//

MessageParser::MessageParser()
: d_state(e_STATE_WANT_HEADER)
, d_message()
{
}

MessageParser::~MessageParser()
{
}

ntsa::Error MessageParser::parse(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const MessageCallback&                     callback)
{
    ntsa::Error error;

    while (true) {
        if (d_state == e_STATE_WANT_HEADER) {
            ntca::ReceiveOptions options;
            options.setMinSize(example::MessageHeader::e_LENGTH);
            options.setMaxSize(example::MessageHeader::e_LENGTH);

            ntca::ReceiveContext receiveContext;
            bdlbb::Blob          data;
            error = streamSocket->receive(&receiveContext, &data, options);
            if (error) {
                if (error == ntsa::Error::e_WOULD_BLOCK) {
                    streamSocket->setReadQueueLowWatermark(
                        example::MessageHeader::e_LENGTH);
                    return ntsa::Error();
                }
                else {
                    return error;
                }
            }

            example::MessageUtil::decodeHeader(&d_message.d_header, data);

            if (d_message.d_header.d_payloadLength > 0) {
                d_state = e_STATE_WANT_PAYLOAD;

                streamSocket->setReadQueueLowWatermark(
                    d_message.d_header.d_payloadLength);
            }
            else {
                callback(d_message);

                d_state = e_STATE_WANT_HEADER;

                streamSocket->setReadQueueLowWatermark(
                    example::MessageHeader::e_LENGTH);
            }
        }

        if (d_state == e_STATE_WANT_PAYLOAD) {
            BSLS_ASSERT_OPT(d_message.d_header.d_payloadLength > 0);

            ntca::ReceiveOptions options;
            options.setMinSize(d_message.d_header.d_payloadLength);
            options.setMaxSize(d_message.d_header.d_payloadLength);

            ntca::ReceiveContext receiveContext;
            bdlbb::Blob          data;
            error = streamSocket->receive(&receiveContext, &data, options);
            if (error) {
                if (error == ntsa::Error::e_WOULD_BLOCK) {
                    streamSocket->setReadQueueLowWatermark(
                        d_message.d_header.d_payloadLength);
                    return ntsa::Error();
                }
                else {
                    return error;
                }
            }

            example::MessageUtil::decodePayload(
                &d_message.d_payload,
                data,
                d_message.d_header.d_payloadLength);

            callback(d_message);

            d_message.d_header.d_transactionId = 0;
            d_message.d_header.d_payloadLength = 0;
            d_message.d_payload.clear();

            d_state = e_STATE_WANT_HEADER;

            streamSocket->setReadQueueLowWatermark(
                example::MessageHeader::e_LENGTH);
        }
    }
}

//
// Now, we have the basic building blocks for our application protocol. Next,
// let's implement the client application protocol over a proactive socket. Our
// client application protocol is implemented in terms of the socket that
// represents the connection, a blob buffer factory to allocate buffers to be
// sent over the connection, our application protocol message parser,
// previously implemented, to decode our variable-length messages from the byte
// stream read from the connection, and a data structure to track requests
// which have been sent but whose responses have not yet been received, which
// allows multiple messages to be simultaneously multiplexed over the same
// connection. This class derives from and implements
// 'ntci::StreamSocketSession', the pure interface for a application
// protocol over a stream socket.
//

// This class implements the 'ntci::StreamSocketSession' protocol to
// provide client communication of the example wire protocol over an
// asynchronous stream socket. This class is thread safe.
class ClientSocket : public ntci::StreamSocketSession,
                     public ntccfg::Shared<ClientSocket>
{
  public:
    // Defines a type alias for a generic function callback.
    typedef bsl::function<void()> Callback;

    // Define a type alias for a function invoked when the
    // response to request is received.
    typedef bsl::function<void(const bsl::string& responsePayload)>
        ResponseCallback;

  private:
    // Define a type alias for an associative data
    // structure mapping a transaction ID to the callback function
    // to be invoked when the response for that transaction ID is received.
    typedef bdlcc::ObjectCatalog<ResponseCallback> PendingCatalog;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    Mutex                               d_mutex;
    bsl::shared_ptr<ntci::StreamSocket> d_streamSocket_sp;
    PendingCatalog                      d_pendingRequests;
    example::MessageParser              d_parser;
    example::ClientSocket::Callback     d_upgradeCallback;
    example::ClientSocket::Callback     d_downgradeCallback;
    bslma::Allocator*                   d_allocator_p;

  private:
    ClientSocket(const ClientSocket&) BSLS_KEYWORD_DELETED;
    ClientSocket& operator=(const ClientSocket&) BSLS_KEYWORD_DELETED;

  private:
    // Process the condition that the size of the read queue is greater
    // than or equal to the read queue low watermark.
    void processReadQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    // Process the condition that the peer has indicated that it will
    // not send any more encrypted data.
    void processDowngradeComplete(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::DowngradeEvent& event) BSLS_KEYWORD_OVERRIDE;

    // Process the upgrade of the specified 'upgradable' that is the
    // specified 'streamSocket' according to the specified 'upgradeEvent'.
    void processUpgrade(const bsl::shared_ptr<ntci::Upgradable>& upgradable,
                        const ntca::UpgradeEvent&                upgradeEvent);

    // Process the specified 'response' received from the server.
    void processResponse(const example::Message& response);

  public:
    // Create a new application client socket over the specified
    // 'streamSocket'. Optionally specify a 'basicAllocator' used to
    // supply memory. If 'basicAllocator' is 0, the currently installed
    // default allocator is used.
    explicit ClientSocket(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        bslma::Allocator*                          basicAllocator = 0);

    // Destroy this object.
    ~ClientSocket();

    // Assume the TLS client role and begin upgrading the socket from
    // being unencrypted to being encrypted with TLS. Invoke the specified
    // 'upgradeCallback' when the socket has completed upgrading to TLS,
    // and invoke the specified 'downgradeCallback' when the socket has
    // completed downgrading from TLS back to clear text.
    void upgrade(
        const bsl::shared_ptr<ntci::EncryptionClient>& encryptionClient,
        const example::ClientSocket::Callback&         upgradeCallback,
        const example::ClientSocket::Callback&         downgradeCallback);

    // Send a message requesting the transformation of the specified
    // 'requestPayload' according to the wire protocol. On success, invoke
    // the specified 'responseCallback' when the response to the request is
    // received. Return the error.
    ntsa::Error send(
        const bsl::string&                             requestPayload,
        const example::ClientSocket::ResponseCallback& responseCallback);

    // Start or stop receiving data according to the specified 'enabled'
    // flag. When receiving data is enabled, the
    // 'processReadQueueLowWatermark' virtual member function will be
    // invoked when incoming data has been buffered to to the read queue
    // low watermark and the 'processShutdownReceive' virtual member
    // function will be invoked when the peer has shut down writing from
    // its side of the connection. When receiving data is disabled, back
    // pressure will start being applied to transmissions by the peer once
    // the operating system receive buffer for this socket becomes full.
    void receive(bool enabled);

    // Begin downgrading the socket from being encrypted with TLS to
    // being unencrypted.
    void downgrade();

    // Shutdown and close the socket.
    void shutdown();

    // Return the certificate of the peer of the socket.
    bsl::shared_ptr<ntci::EncryptionCertificate> remoteCertificate() const;
};

//
// Next, let's define the implementation of the client socket. Note the
// private virtual member function 'processReadQueueLowWatermark', which is
// invoked when enough data has been read and buffered to satisfy the criteria
// of the 'numNeeded', where 'numNeeded' corresponds to the total amount of
// data that must be read for the message parser to transition to the next
// state (i.e. parsing either the fixed-length header or the variable
// length-payload.) Also note the two major responsibilities of the virtual
// member function 'processReadQueueLowWatermark' override. First, it must
// parse and consume 'readQueue' according to the application protocol then
// "pop" (or erase) all the data consumed from the front of 'readQueue'.
// Second, it must set 'numNeeded' to the number of total bytes needed to
// transition to the next parsing state. In this example, both responsibilities
// are fulfilled by our parsing mechanism. Also note the private virtual member
// function 'processShutdown', which is invoked when the peer has shut down
// writing from its side of the connection, or the implementation has shut
// down reading from the local side of the connection. The responsibility of
// the 'processShutdown' virtual member function override is to shut down
// writing from the local side of the connection, when appropriate, to continue
//  the graceful shutdown sequence of the connection.
//

void ClientSocket::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    ntsa::Error error = d_parser.parse(
        d_streamSocket_sp,
        bdlf::MemFnUtil::memFn(&example::ClientSocket::processResponse, this));
}

void ClientSocket::processDowngradeComplete(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::DowngradeEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    d_streamSocket_sp->downgrade();

    example::ClientSocket::Callback downgradeCallback(bsl::allocator_arg,
                                                      d_allocator_p);
    {
        LockGuard lockGuard(&d_mutex);
        d_downgradeCallback.swap(downgradeCallback);
    }

    if (downgradeCallback) {
        downgradeCallback();
    }
}

void ClientSocket::processUpgrade(
    const bsl::shared_ptr<ntci::Upgradable>& upgradable,
    const ntca::UpgradeEvent&                upgradeEvent)
{
    NTCCFG_WARNING_UNUSED(upgradable);

    if (upgradeEvent.type() == ntca::UpgradeEventType::e_COMPLETE) {
        example::ClientSocket::Callback upgradeCallback(bsl::allocator_arg,
                                                        d_allocator_p);
        {
            LockGuard lockGuard(&d_mutex);
            d_upgradeCallback.swap(upgradeCallback);
        }

        if (upgradeCallback) {
            upgradeCallback();
        }
    }
}

void ClientSocket::processResponse(const example::Message& response)
{
    example::ClientSocket::ResponseCallback responseCallback;
    if (0 != d_pendingRequests.remove(response.d_header.d_transactionId,
                                      &responseCallback))
    {
        return;
    }

    responseCallback(response.d_payload);
}

ClientSocket::ClientSocket(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    bslma::Allocator*                          basicAllocator)
: d_mutex()
, d_streamSocket_sp(streamSocket)
, d_pendingRequests(basicAllocator)
, d_parser()
, d_upgradeCallback(bsl::allocator_arg, basicAllocator)
, d_downgradeCallback(bsl::allocator_arg, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ClientSocket::~ClientSocket()
{
}

void ClientSocket::upgrade(
    const bsl::shared_ptr<ntci::EncryptionClient>& encryptionClient,
    const example::ClientSocket::Callback&         upgradeCallback,
    const example::ClientSocket::Callback&         downgradeCallback)
{
    {
        LockGuard lockGuard(&d_mutex);
        d_upgradeCallback   = upgradeCallback;
        d_downgradeCallback = downgradeCallback;
    }

    d_streamSocket_sp->upgrade(
        encryptionClient,
        ntca::UpgradeOptions(),
        d_streamSocket_sp->createUpgradeCallback(
            bdlf::MemFnUtil::memFn(&ClientSocket::processUpgrade, this),
            d_allocator_p));
}

ntsa::Error ClientSocket::send(
    const bsl::string&                             requestPayload,
    const example::ClientSocket::ResponseCallback& responseCallback)
{
    int transactionId = d_pendingRequests.add(responseCallback);

    example::Message request;

    request.d_header.d_transactionId = transactionId;
    request.d_header.d_payloadLength = static_cast<int>(requestPayload.size());
    request.d_payload                = requestPayload;

    bdlbb::Blob requestBlob(
        d_streamSocket_sp->outgoingBlobBufferFactory().get());

    example::MessageUtil::encodeHeader(&requestBlob, request.d_header);
    example::MessageUtil::encodePayload(&requestBlob, request.d_payload);

    return d_streamSocket_sp->send(requestBlob, ntca::SendOptions());
}

void ClientSocket::receive(bool enabled)
{
    if (enabled) {
        d_streamSocket_sp->relaxFlowControl(ntca::FlowControlType::e_RECEIVE);
    }
    else {
        d_streamSocket_sp->applyFlowControl(
            ntca::FlowControlType::e_RECEIVE,
            ntca::FlowControlMode::e_IMMEDIATE);
    }
}

void ClientSocket::downgrade()
{
    d_streamSocket_sp->downgrade();
}

void ClientSocket::shutdown()
{
    d_streamSocket_sp->shutdown(ntsa::ShutdownType::e_BOTH,
                                ntsa::ShutdownMode::e_GRACEFUL);

    d_streamSocket_sp->close();
}

bsl::shared_ptr<ntci::EncryptionCertificate> ClientSocket::remoteCertificate()
    const
{
    return d_streamSocket_sp->remoteCertificate();
}

//
// Now that the client socket is implemented, let's declare the server
// socket. The server socket is implemented similar to the client socket
// with the exception that there is no pending work to keep track of, all
// requests are synchronously processed and the resposes immeditiately written
// back to the socket. Otherwise, all notes about the requirements of the
// client socket pertain to the server socket.
//

// This class implements the 'ntci::StreamSocketSession' protocol to
// provide client communication of the example wire protocol over an
// asynchronous stream socket. This class is thread safe.
class ServerSocket : public ntci::StreamSocketSession,
                     public ntccfg::Shared<ServerSocket>
{
  public:
    // Defines a type alias for a generic function callback.
    typedef bsl::function<void()> Callback;

  private:
    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    Mutex                               d_mutex;
    bsl::shared_ptr<ntci::StreamSocket> d_streamSocket_sp;
    example::MessageParser              d_parser;
    example::ServerSocket::Callback     d_upgradeCallback;
    example::ServerSocket::Callback     d_downgradeCallback;
    bslma::Allocator*                   d_allocator_p;

  private:
    ServerSocket(const ServerSocket&) BSLS_KEYWORD_DELETED;
    ServerSocket& operator=(const ServerSocket&) BSLS_KEYWORD_DELETED;

  private:
    // Process the condition that the size of the read queue is greater
    // than or equal to the read queue low watermark.
    void processReadQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    // Process the condition that the peer has indicated that it will
    // not send any more encrypted data.
    void processDowngradeComplete(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::DowngradeEvent& event) BSLS_KEYWORD_OVERRIDE;

    // Process the upgrade of the specified 'upgradable' that is the
    // specified 'streamSocket' according to the specified 'upgradeEvent'.
    void processUpgrade(const bsl::shared_ptr<ntci::Upgradable>& upgradable,
                        const ntca::UpgradeEvent&                upgradeEvent);

    // Process the specified 'request' received from the client.
    void processRequest(const example::Message& request);

  public:
    // Create a new application server socket over the specified
    // 'streamSocket'. Optionally specify a 'basicAllocator' used to
    // supply memory. If 'basicAllocator' is 0, the currently installed
    // default allocator is used.
    explicit ServerSocket(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        bslma::Allocator*                          basicAllocator = 0);

    // Destroy this object.
    ~ServerSocket() BSLS_KEYWORD_OVERRIDE;

    // Assume the TLS server role and begin upgrading the socket from
    // being unencrypted to being encrypted with TLS. Invoke the specified
    // 'upgradeCallback' when the socket has completed upgrading to TLS,
    // and invoke the specified 'downgradeCallback' when the socket has
    // completed downgrading from TLS back to clear text.
    void upgrade(
        const bsl::shared_ptr<ntci::EncryptionServer>& encryptionServer,
        const example::ServerSocket::Callback&         upgradeCallback,
        const example::ServerSocket::Callback&         downgradeCallback);

    // Start or stop receiving data according to the specified 'enabled'
    // flag. When receiving data is enabled, the
    // 'processReadQueueLowWatermark' virtual member function will be
    // invoked when incoming data has been buffered to to the read queue
    // low watermark and the 'processShutdownReceive' virtual member
    // function will be invoked when the peer has shut down writing from
    // its side of the connection. When receiving data is disabled, back
    // pressure will start being applied to transmissions by the peer once
    // the operating system receive buffer for this socket becomes full.
    void receive(bool enabled);

    // Begin downgrading the socket from being encrypted with TLS to
    // being unencrypted.
    void downgrade();

    // Shutdown and close the socket.
    void shutdown();

    // Return the certificate of the peer of the socket.
    bsl::shared_ptr<ntci::EncryptionCertificate> remoteCertificate() const;
};

//
// Next, let's define the implementation of the server socket.
//

void ServerSocket::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    ntsa::Error error = d_parser.parse(
        d_streamSocket_sp,
        bdlf::MemFnUtil::memFn(&example::ServerSocket::processRequest, this));
}

void ServerSocket::processDowngradeComplete(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::DowngradeEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    d_streamSocket_sp->downgrade();

    example::ServerSocket::Callback downgradeCallback(bsl::allocator_arg,
                                                      d_allocator_p);
    {
        LockGuard lockGuard(&d_mutex);
        d_downgradeCallback.swap(downgradeCallback);
    }

    if (downgradeCallback) {
        downgradeCallback();
    }
}

void ServerSocket::processUpgrade(
    const bsl::shared_ptr<ntci::Upgradable>& upgradable,
    const ntca::UpgradeEvent&                upgradeEvent)
{
    NTCCFG_WARNING_UNUSED(upgradable);

    if (upgradeEvent.type() == ntca::UpgradeEventType::e_COMPLETE) {
        example::ServerSocket::Callback upgradeCallback(bsl::allocator_arg,
                                                        d_allocator_p);
        {
            LockGuard lockGuard(&d_mutex);
            d_upgradeCallback.swap(upgradeCallback);
        }

        if (upgradeCallback) {
            upgradeCallback();
        }
    }
}

void ServerSocket::processRequest(const example::Message& request)
{
    example::Message response;

    response.d_header.d_transactionId = request.d_header.d_transactionId;
    response.d_header.d_payloadLength = request.d_header.d_payloadLength;

    response.d_payload = request.d_payload;
    bdlb::String::toUpper(&response.d_payload);

    bdlbb::Blob responseBlob(
        d_streamSocket_sp->outgoingBlobBufferFactory().get());

    example::MessageUtil::encodeHeader(&responseBlob, response.d_header);
    example::MessageUtil::encodePayload(&responseBlob, response.d_payload);

    d_streamSocket_sp->send(responseBlob, ntca::SendOptions());
}

ServerSocket::ServerSocket(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    bslma::Allocator*                          basicAllocator)
: d_mutex()
, d_streamSocket_sp(streamSocket)
, d_parser()
, d_upgradeCallback(bsl::allocator_arg, basicAllocator)
, d_downgradeCallback(bsl::allocator_arg, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ServerSocket::~ServerSocket()
{
}

void ServerSocket::upgrade(
    const bsl::shared_ptr<ntci::EncryptionServer>& encryptionServer,
    const example::ServerSocket::Callback&         upgradeCallback,
    const example::ServerSocket::Callback&         downgradeCallback)
{
    {
        LockGuard lockGuard(&d_mutex);
        d_upgradeCallback   = upgradeCallback;
        d_downgradeCallback = downgradeCallback;
    }

    d_streamSocket_sp->upgrade(
        encryptionServer,
        ntca::UpgradeOptions(),
        d_streamSocket_sp->createUpgradeCallback(
            bdlf::MemFnUtil::memFn(&ServerSocket::processUpgrade, this),
            d_allocator_p));
}

void ServerSocket::receive(bool enabled)
{
    if (enabled) {
        d_streamSocket_sp->relaxFlowControl(ntca::FlowControlType::e_RECEIVE);
    }
    else {
        d_streamSocket_sp->applyFlowControl(
            ntca::FlowControlType::e_RECEIVE,
            ntca::FlowControlMode::e_IMMEDIATE);
    }
}

void ServerSocket::downgrade()
{
    d_streamSocket_sp->downgrade();
}

void ServerSocket::shutdown()
{
    d_streamSocket_sp->shutdown(ntsa::ShutdownType::e_BOTH,
                                ntsa::ShutdownMode::e_GRACEFUL);
    d_streamSocket_sp->close();
}

bsl::shared_ptr<ntci::EncryptionCertificate> ServerSocket::remoteCertificate()
    const
{
    return d_streamSocket_sp->remoteCertificate();
}

//
// Now that the server socket is implemented, let's declare the listener
// socket. The listener socket simply accepts stream sockets from the backlog
// once they have been connected.
//

// This class implements the 'ntci::ListenerSocketSession' protocol to
// accept stream sockets from the backlog of a listening socket. This class
// is thread safe.
class ListenerSocket : public ntci::ListenerSocketSession,
                       public ntccfg::Shared<ListenerSocket>
{
  public:
    // Defines a type alias for a generic function callback.
    typedef bsl::function<void()> Callback;

  private:
    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    Mutex                                 d_mutex;
    bsl::shared_ptr<ntci::ListenerSocket> d_listenerSocket_sp;
    bslma::Allocator*                     d_allocator_p;

  private:
    ListenerSocket(const ListenerSocket&) BSLS_KEYWORD_DELETED;
    ListenerSocket& operator=(const ListenerSocket&) BSLS_KEYWORD_DELETED;

  private:
    // Process the condition that the size of the accept queue is greater
    // than or equal to the accept queue low watermark.
    void processAcceptQueueLowWatermark(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        const ntca::AcceptQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

  public:
    // Create a new application listener socket over the specified
    // 'listenerSocket'. Optionally specify a 'basicAllocator' used to
    // supply memory. If 'basicAllocator' is 0, the currently installed
    // default allocator is used.
    explicit ListenerSocket(
        const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
        bslma::Allocator*                            basicAllocator = 0);

    // Destroy this object.
    ~ListenerSocket() BSLS_KEYWORD_OVERRIDE;

    // Start or stop accepting connections according to the specified
    // 'enabled' flag. When accepting connections is enabled, the
    // 'processAcceptQueueLowWatermark' virtual member function will be
    // invoked when backlog has been buffered to to the accept queue
    // low watermark. When accepting connections is disabled, back pressure
    // will start being applied to connection attempts by the peer once
    // the operating system backlog for this socket becomes full.
    void accept(bool enabled);

    // Shutdown and close the socket.
    void shutdown();
};

//
// Next, let's define the implementation of the listener socket.
//

void ListenerSocket::processAcceptQueueLowWatermark(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    const ntca::AcceptQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
    NTCCFG_WARNING_UNUSED(event);

    ntsa::Error error;

    while (true) {
        ntca::AcceptContext                 acceptContext;
        bsl::shared_ptr<ntci::StreamSocket> streamSocket;
        error = d_listenerSocket_sp->accept(&acceptContext,
                                            &streamSocket,
                                            ntca::AcceptOptions());
        if (error) {
            break;
        }

        streamSocket->open();
    }
}

ListenerSocket::ListenerSocket(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket,
    bslma::Allocator*                            basicAllocator)
: d_mutex()
, d_listenerSocket_sp(listenerSocket)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ListenerSocket::~ListenerSocket()
{
}

void ListenerSocket::accept(bool enabled)
{
    if (enabled) {
        d_listenerSocket_sp->relaxFlowControl(
            ntca::FlowControlType::e_RECEIVE);
    }
    else {
        d_listenerSocket_sp->applyFlowControl(
            ntca::FlowControlType::e_RECEIVE,
            ntca::FlowControlMode::e_IMMEDIATE);
    }
}

void ListenerSocket::shutdown()
{
    d_listenerSocket_sp->shutdown();
    d_listenerSocket_sp->close();
}

//
// Now that we've implemented the client and server sockets that speak our
// application protocol, we can implement the clients and servers themselves.
// The clients are responsible for initiating connections to the server,
// tracking established connections, and translating from the semantics of the
// stream socket to the semantics of the client role of the application
// protocol. Additionally, for thread safety and correctness to ensure that no
// callbacks are invoked on objects have they are destroyed, the client
// provides member functions to shutdown all connections and linger until they
// are all completely closed.  Let's start by declaring the client.
//

// This class implements the 'ntci::StreamSocketManager' protocol to
// provide client communication of the example wire protocol. This class is
// thread safe.
class Client : public ntci::StreamSocketManager, public ntccfg::Shared<Client>
{
  public:
    // Defines a type alias for a deferred function.
    typedef bsl::function<void(
        const bsl::shared_ptr<example::ClientSocket>& clientSocket)>
        ClientSocketCallback;

  private:
    // This typedef defines a map of stream sockets pending the completion
    // of their connection to the callbacks to notify the user of the
    // completion of the connection.
    typedef bsl::unordered_map<bsl::shared_ptr<ntci::StreamSocket>,
                               ClientSocketCallback>
        PendingConnectionMap;

    // This typedef defines a map of stream sockets to applications of
    // those stream sockets.
    typedef bsl::unordered_map<bsl::shared_ptr<ntci::StreamSocket>,
                               bsl::shared_ptr<example::ClientSocket> >
        StreamSocketMap;

    ntccfg::ConditionMutex           d_mutex;
    bsl::shared_ptr<ntci::Interface> d_interface_sp;
    PendingConnectionMap             d_pendingConnections;
    StreamSocketMap                  d_streamSockets;
    ntccfg::Condition                d_linger;
    bslma::Allocator*                d_allocator_p;

  private:
    Client(const Client&) BSLS_KEYWORD_DELETED;
    Client& operator=(const Client&) BSLS_KEYWORD_DELETED;

  private:
    void processStreamSocketEstablished(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
        BSLS_KEYWORD_OVERRIDE;
    // Process the establishment of the specified 'streamSocket'.

    // Process the closure of the specified 'streamSocket'.
    void processStreamSocketClosed(const bsl::shared_ptr<ntci::StreamSocket>&
                                       streamSocket) BSLS_KEYWORD_OVERRIDE;

    // Process the specified 'connectEvent' for the specified 'connector'
    // that is the specified 'streamSocket'.
    void processConnect(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Connector>&    connector,
        const ntca::ConnectEvent&                  connectEvent);

  public:
    // Create a new client that creates stream sockets using the specified
    // 'interface'. Optionally specify a 'basicAllocator' used to supply
    // memory. If 'basicAllocator' is 0, the currently installed default
    // allocator is used.
    explicit Client(const bsl::shared_ptr<ntci::Interface>& interface,
                    bslma::Allocator* basicAllocator = 0);

    // Destroy this object.
    ~Client() BSLS_KEYWORD_OVERRIDE;

    // Establish a new connection to the specified 'remoteEndpoint' and
    // invoke the specified 'callback' when the connection has been
    // established.
    void connect(const ntsa::Endpoint&                        remoteEndpoint,
                 const example::Client::ClientSocketCallback& callback);

    // Shutdown and close all sockets.
    void shutdown();

    // Wait until all sockets are closed.
    void linger();
};

//
// Next, let's define the implementation of the client.
//

void Client::processStreamSocketEstablished(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    bsl::shared_ptr<example::ClientSocket> clientSocket;
    example::Client::ClientSocketCallback  callback;

    {
        ntccfg::ConditionMutexGuard lockGuard(&d_mutex);

        PendingConnectionMap::iterator it =
            d_pendingConnections.find(streamSocket);
        BSLS_ASSERT_OPT(it != d_pendingConnections.end());

        callback = it->second;
        d_pendingConnections.erase(it);

        clientSocket.createInplace(d_allocator_p, streamSocket, d_allocator_p);

        streamSocket->registerSession(clientSocket);

        d_streamSockets[streamSocket] = clientSocket;
    }

    callback(clientSocket);
}

void Client::processStreamSocketClosed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    ntccfg::ConditionMutexGuard lockGuard(&d_mutex);

    d_streamSockets.erase(streamSocket);

    if (d_streamSockets.empty()) {
        d_linger.signal();
    }
}

void Client::processConnect(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Connector>&    connector,
    const ntca::ConnectEvent&                  connectEvent)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(connector);

    if (connectEvent.context().error()) {
        BSLS_LOG_ERROR("Failed to connect: %s",
                       connectEvent.context().error().text().c_str());
    }
}

Client::Client(const bsl::shared_ptr<ntci::Interface>& interface,
               bslma::Allocator*                       basicAllocator)
: d_mutex()
, d_interface_sp(interface)
, d_pendingConnections(basicAllocator)
, d_streamSockets(basicAllocator)
, d_linger()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Client::~Client()
{
}

void Client::connect(const ntsa::Endpoint& remoteEndpoint,
                     const example::Client::ClientSocketCallback& callback)
{
    ntsa::Error error;

    ntca::StreamSocketOptions streamSocketOptions;

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        d_interface_sp->createStreamSocket(streamSocketOptions, d_allocator_p);

    streamSocket->registerManager(this->getSelf(this));

    {
        ntccfg::ConditionMutexGuard lockGuard(&d_mutex);
        d_pendingConnections[streamSocket] = callback;
    }

    ntci::ConnectCallback connectCallback =
        streamSocket->createConnectCallback(
            NTCCFG_BIND(&Client::processConnect,
                        this,
                        streamSocket,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2));

    streamSocket->connect(remoteEndpoint,
                          ntca::ConnectOptions(),
                          connectCallback);
}

void Client::shutdown()
{
    StreamSocketMap streamSockets;
    {
        ntccfg::ConditionMutexGuard lockGuard(&d_mutex);
        streamSockets = d_streamSockets;
    }

    for (StreamSocketMap::iterator it = streamSockets.begin();
         it != streamSockets.end();
         ++it)
    {
        it->first->close();
    }
}

void Client::linger()
{
    ntccfg::ConditionMutexGuard lockGuard(&d_mutex);

    while (!d_streamSockets.empty()) {
        d_linger.wait(&d_mutex);
    }
}

//
// Next, let's declare the server. The server is nearly identical to the
// client, except that it transforms the semantics of the stream socket to the
// semantics of the server role of the application protocol, rather than the
// client role.
//

// This class implements the 'ntci::ListenerSocketManager' and
// protocol to 'ntci::ListenerSocketSession' protocols to provide server
// communication of the example wire protocol. This class is thread safe.
class Server : public ntci::ListenerSocketManager,
               public ntci::ListenerSocketSession,
               public ntccfg::Shared<Server>
{
  public:
    // Defines a type alias for a deferred function.
    typedef bsl::function<void(
        const bsl::shared_ptr<example::ServerSocket>& serverSocket)>
        ServerSocketCallback;

    // This typedef defines a map of listener sockets listening for
    // connections to the callbacks to notify the user of the
    // completion of an accepted connection.
    typedef bsl::unordered_map<bsl::shared_ptr<ntci::ListenerSocket>,
                               ServerSocketCallback>
        PendingConnectionMap;

    // This typedef defines a map of listener sockets to applications of
    // those listener sockets.
    typedef bsl::unordered_map<bsl::shared_ptr<ntci::ListenerSocket>,
                               bsl::shared_ptr<example::ListenerSocket> >
        ListenerSocketMap;

    // This typedef defines a map of stream sockets to applications of
    // those stream sockets.
    typedef bsl::unordered_map<bsl::shared_ptr<ntci::StreamSocket>,
                               bsl::shared_ptr<example::ServerSocket> >
        StreamSocketMap;

    mutable ntccfg::ConditionMutex   d_mutex;
    bsl::shared_ptr<ntci::Interface> d_interface_sp;
    PendingConnectionMap             d_pendingConnections;
    ListenerSocketMap                d_listenerSockets;
    StreamSocketMap                  d_streamSockets;
    ntccfg::Condition                d_linger;
    bslma::Allocator*                d_allocator_p;

  private:
    Server(const Server&) BSLS_KEYWORD_DELETED;
    Server& operator=(const Server&) BSLS_KEYWORD_DELETED;

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
    // Process the establishment of the specified 'streamSocket'. Return
    // the application protocol of the 'streamSocket'.

    // Process the closure of the specified 'streamSocket'.
    void processStreamSocketClosed(const bsl::shared_ptr<ntci::StreamSocket>&
                                       streamSocket) BSLS_KEYWORD_OVERRIDE;

  public:
    // Create a new server that creates listener and stream sockets using
    // the specified 'interface'. Optionally specify a 'basicAllocator'
    // used to supply memory. If 'basicAllocator' is 0, the currently
    // installed default allocator is used.
    explicit Server(const bsl::shared_ptr<ntci::Interface>& interface,
                    bslma::Allocator* basicAllocator = 0);

    // Destroy this object.
    ~Server() BSLS_KEYWORD_OVERRIDE;

    // Begin listening for connections to the specified 'sourceEndpoint'
    // and invoke the specified 'callback' when the listener has
    // begun listening. Return the source endpoint to which the listener
    // is bound.
    ntsa::Endpoint listen(const ntsa::Endpoint&       sourceEndpoint,
                          const ServerSocketCallback& callback);

    // Shutdown and close all listeners and sessions.
    void shutdown();

    // Wait until all listeners and sessions are closed.
    void linger();
};

//
// Now, let's define the implementation of the server.
//

void Server::processListenerSocketEstablished(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
{
    bsl::shared_ptr<example::ListenerSocket> listenerSocketSession;
    {
        ntccfg::ConditionMutexGuard lockGuard(&d_mutex);

        listenerSocketSession.createInplace(d_allocator_p,
                                            listenerSocket,
                                            d_allocator_p);

        listenerSocket->registerSession(listenerSocketSession);

        d_listenerSockets[listenerSocket] = listenerSocketSession;
    }

    listenerSocketSession->accept(true);
}

void Server::processListenerSocketClosed(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
{
    ntccfg::ConditionMutexGuard lockGuard(&d_mutex);

    {
        std::size_t n = d_pendingConnections.erase(listenerSocket);
        BSLS_ASSERT_OPT(n == 1);
    }

    {
        std::size_t n = d_listenerSockets.erase(listenerSocket);
        BSLS_ASSERT_OPT(n == 1);
    }

    if (d_pendingConnections.empty() && d_listenerSockets.empty() &&
        d_streamSockets.empty())
    {
        d_linger.signal();
    }
}

void Server::processStreamSocketEstablished(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    bsl::shared_ptr<example::ServerSocket> serverSocket;
    serverSocket.createInplace(d_allocator_p, streamSocket, d_allocator_p);

    streamSocket->registerSession(serverSocket);

    ServerSocketCallback callback;
    {
        ntccfg::ConditionMutexGuard lockGuard(&d_mutex);

        callback = d_pendingConnections[streamSocket->acceptor()];
        d_streamSockets[streamSocket] = serverSocket;
    }

    callback(serverSocket);
}

void Server::processStreamSocketClosed(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
{
    ntccfg::ConditionMutexGuard lockGuard(&d_mutex);

    std::size_t n = d_streamSockets.erase(streamSocket);
    BSLS_ASSERT_OPT(n == 1);

    if (d_pendingConnections.empty() && d_listenerSockets.empty() &&
        d_streamSockets.empty())
    {
        d_linger.signal();
    }
}

Server::Server(const bsl::shared_ptr<ntci::Interface>& interface,
               bslma::Allocator*                       basicAllocator)
: d_mutex()
, d_interface_sp(interface)
, d_pendingConnections(basicAllocator)
, d_listenerSockets(basicAllocator)
, d_streamSockets(basicAllocator)
, d_linger()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Server::~Server()
{
}

ntsa::Endpoint Server::listen(const ntsa::Endpoint&       sourceEndpoint,
                              const ServerSocketCallback& callback)
{
    ntsa::Error error;

    ntca::ListenerSocketOptions listenerSocketOptions;
    listenerSocketOptions.setSourceEndpoint(sourceEndpoint);

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        d_interface_sp->createListenerSocket(listenerSocketOptions,
                                             d_allocator_p);

    listenerSocket->registerManager(this->getSelf(this));
    listenerSocket->registerSession(this->getSelf(this));

    {
        ntccfg::ConditionMutexGuard lockGuard(&d_mutex);
        d_pendingConnections[listenerSocket] = callback;
    }

    error = listenerSocket->open();
    BSLS_ASSERT_OPT(!error);

    error = listenerSocket->listen();
    BSLS_ASSERT_OPT(!error);

    return listenerSocket->sourceEndpoint();
}

void Server::shutdown()
{
    ListenerSocketMap listenerSockets;
    StreamSocketMap   streamSockets;
    {
        ntccfg::ConditionMutexGuard lockGuard(&d_mutex);
        listenerSockets = d_listenerSockets;
        streamSockets   = d_streamSockets;
    }

    for (ListenerSocketMap::iterator it = listenerSockets.begin();
         it != listenerSockets.end();
         ++it)
    {
        it->first->close();
    }

    for (StreamSocketMap::iterator it = streamSockets.begin();
         it != streamSockets.end();
         ++it)
    {
        it->first->close();
    }
}

void Server::linger()
{
    ntccfg::ConditionMutexGuard lockGuard(&d_mutex);
    while (!d_pendingConnections.empty() || !d_listenerSockets.empty() ||
           !d_streamSockets.empty())
    {
        d_linger.wait(&d_mutex);
    }
}

//
// Finally, let's define callbacks to be installed into our client and server.
// We'll use semaphores to block the calling thread until desired events occur
// on the I/O threads managed by the asynchronous socket interface, allowing us
// to write code having a linear flow of control despite its asynchronous
// behavior.
//

void processServerSocketEstablished(
    const bsl::shared_ptr<example::Server>&       server,
    const bsl::shared_ptr<example::ServerSocket>& serverSocket,
    bslmt::Semaphore*                             semaphore,
    bsl::shared_ptr<example::ServerSocket>*       result)
{
    NTCCFG_WARNING_UNUSED(server);

    *result = serverSocket;
    semaphore->post();
}

void processServerSocketEvent(
    const bsl::shared_ptr<example::Server>&       server,
    const bsl::shared_ptr<example::ServerSocket>& serverSocket,
    bslmt::Semaphore*                             semaphore)
{
    NTCCFG_WARNING_UNUSED(server);
    NTCCFG_WARNING_UNUSED(serverSocket);

    semaphore->post();
}

void processClientSocketEstablished(
    const bsl::shared_ptr<example::Client>&       client,
    const bsl::shared_ptr<example::ClientSocket>& clientSocket,
    bslmt::Semaphore*                             semaphore,
    bsl::shared_ptr<example::ClientSocket>*       result)
{
    NTCCFG_WARNING_UNUSED(client);

    *result = clientSocket;
    semaphore->post();
}

void processClientSocketEvent(
    const bsl::shared_ptr<example::Client>&       client,
    const bsl::shared_ptr<example::ClientSocket>& clientSocket,
    bslmt::Semaphore*                             semaphore)
{
    NTCCFG_WARNING_UNUSED(client);
    NTCCFG_WARNING_UNUSED(clientSocket);

    semaphore->post();
}

void processClientResponseReceived(bsl::string*       result,
                                   bslmt::Latch*      resultLatch,
                                   const bsl::string& response)
{
    // Store the response received and indicate a response was received.

    *result = response;
    resultLatch->arrive();
}

//
// At this point, all the custom infrastructure for our application protocol
// has been implemented. Now, let's move on to the example of how to use the
// application protocol classes that are implemented in terms of asynchronous
// sockets.
//
// Let's write a function which creates and starts an asynchronous socket
// interface, creates an application protocol server and client, connects the
// client to the server, sends a request from the client to the server,
// receives a response from the server to the client, shuts down the
// connection, waits until the connection is closed, then stops the application
// protocol client and server then finally stops the asynchronous socket
// interface.
//

namespace usage1 {

void execute()
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    bslma::Allocator* allocator = bslma::Default::defaultAllocator();

    // Create an asynchronous socket interface running two I/O threads.

    ntca::InterfaceConfig interfaceConfig;
    interfaceConfig.setMetricName("example");
    interfaceConfig.setMinThreads(2);
    interfaceConfig.setMaxThreads(2);

    bsl::shared_ptr<ntci::Interface> interface =
        ntcf::System::createInterface(interfaceConfig, allocator);

    interface->start();

    // Create a server of the application wire protocol and begin listening
    // for incoming connections on a port chosen for that application wire
    // protocol.

    bsl::shared_ptr<example::Server> server;
    server.createInplace(allocator, interface, allocator);

    bsl::shared_ptr<example::ServerSocket> serverSocket;
    bslmt::Semaphore                       serverEstablished;

    ntsa::LocalName localName;
    const ntsa::Error error = ntsa::LocalName::generateUnique(&localName);
    BSLS_ASSERT_OPT(!error);

    ntsa::Endpoint listenerEndpoint = ntsa::Endpoint(localName);

    listenerEndpoint =
        server->listen(listenerEndpoint,
                       NTCCFG_BIND(&example::processServerSocketEstablished,
                                   server,
                                   NTCCFG_BIND_PLACEHOLDER_1,
                                   &serverEstablished,
                                   &serverSocket));

    // Create a client of the application wire protocol and begin connecting
    // to the server.

    bsl::shared_ptr<example::Client> client;
    client.createInplace(allocator, interface, allocator);

    bsl::shared_ptr<example::ClientSocket> clientSocket;
    bslmt::Semaphore                       clientEstablished;

    client->connect(listenerEndpoint,
                    NTCCFG_BIND(&example::processClientSocketEstablished,
                                client,
                                NTCCFG_BIND_PLACEHOLDER_1,
                                &clientEstablished,
                                &clientSocket));

    // Wait for the server socket to become established.

    serverEstablished.wait();

    // Begin reading data from the server socket.

    serverSocket->receive(true);

    // Wait for the client session to become established.

    clientEstablished.wait();

    // Begin reading data from the client socket.

    clientSocket->receive(true);

    // Send a request through the client session.

    bsl::string  response;
    bslmt::Latch responseLatch(1);

    clientSocket->send("Hello, world!",
                       NTCCFG_BIND(&example::processClientResponseReceived,
                                   &response,
                                   &responseLatch,
                                   NTCCFG_BIND_PLACEHOLDER_1));

    // Wait until a response is received and ensure the response matches
    // the request transformed into uppercase, as defined by the application
    // wire protocol.

    responseLatch.wait();
    BSLS_ASSERT_OPT(response == "HELLO, WORLD!");

    BSLS_LOG_INFO("Received '%s'", response.c_str());

    // Shutdown and wait for the client to stop.

    client->shutdown();
    client->linger();

    // Shutdown and wait for the server to stop.

    server->shutdown();
    server->linger();

    // Shutdown and wait for the asynchronous socket interface to stop.

    interface->shutdown();
    interface->linger();
#endif
}

}  // close namespace usage1

//
// Running the 'example::usage1::execute' function results in something like
// the following printed to standard output:
//
//     Received 'HELLO, WORLD!'
//

// @par Usage Example 2: Upgrade connections using Transport Layer Security (TLS)
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example illustrates how to establish a connection from a client to a
// server, exchange a request/response over cleartext, then upgrade to an
// authenticated, secure connection using TLS, exchange an encrypted
// request/response, then downgrade back to cleartext, and finally exchange
// a request/response in cleartext again.
//
// This example extends the first example, using the infrastructure described
// in that example. Refer to the first usage example for a description
// of the classes in the 'example' namespaces.
//
// Note that, for the purposes of a reproducible, executing example, this
// usage shows how to configure the TLS client and server using a locally
// generated certificate authority (CA) to sign a locally generated certificate
// for the server. In real production scenearios, all certificates and private
// keys are typically loaded from a secure location on disk.
//

namespace usage2 {

void execute()
{
    // Only run this example when built with OpenSSL support.

#if defined(BSLS_PLATFORM_OS_UNIX)
#if NTCU12_BUILD_WITH_TLS

    bslma::Allocator* allocator = bslma::Default::defaultAllocator();

    ntsa::Error error;

    // Create an asynchronous socket interface running two I/O threads.

    ntca::InterfaceConfig interfaceConfig;
    interfaceConfig.setMetricName("example");
    interfaceConfig.setMinThreads(2);
    interfaceConfig.setMaxThreads(2);

    bsl::shared_ptr<ntci::Interface> interface =
        ntcf::System::createInterface(interfaceConfig, allocator);

    interface->start();

    // Generate the certificates and private keys of the server and the
    // certificate authority (CA) that issues the server's certificate. In
    // a production scenario, the CA certificate, server certificate, and
    // server private key are typically loaded from a secure location on disk.

    bsl::shared_ptr<ntci::EncryptionCertificate> authorityCertificate;
    bsl::shared_ptr<ntci::EncryptionKey>         authorityPrivateKey;

    bsl::shared_ptr<ntci::EncryptionCertificate> serverCertificate;
    bsl::shared_ptr<ntci::EncryptionKey>         serverPrivateKey;

    {
        // Generate the certificate and private key of a trusted authority.

        ntsa::DistinguishedName authorityIdentity;
        authorityIdentity["CN"] = "Authority";
        authorityIdentity["O"]  = "Bloomberg LP";

        interface->generateKey(&authorityPrivateKey,
                               ntca::EncryptionKeyOptions());

        ntca::EncryptionCertificateOptions authorityCertificateOptions;
        authorityCertificateOptions.setAuthority(true);

        interface->generateCertificate(&authorityCertificate,
                                       authorityIdentity,
                                       authorityPrivateKey,
                                       authorityCertificateOptions);

        // Generate the certificate and private key of the server, signed
        // by the trusted authority.

        ntsa::DistinguishedName serverIdentity;
        serverIdentity["CN"] = "Server";
        serverIdentity["O"]  = "Bloomberg LP";

        interface->generateKey(&serverPrivateKey,
                               ntca::EncryptionKeyOptions());

        interface->generateCertificate(&serverCertificate,
                                       serverIdentity,
                                       serverPrivateKey,
                                       authorityCertificate,
                                       authorityPrivateKey,
                                       ntca::EncryptionCertificateOptions());
    }

    // Create an encryption server and configure the encryption server to
    // accept upgrades to TLS 1.1 and higher, to cryptographically identify
    // itself using the server certificate previously generated, to encrypt
    // data using the server private key previously generated, and to not
    // require identification from the client.

    ntca::EncryptionServerOptions encryptionServerOptions;
    encryptionServerOptions.setMinMethod(ntca::EncryptionMethod::e_TLS_V1_X);
    encryptionServerOptions.setMaxMethod(ntca::EncryptionMethod::e_TLS_V1_X);
    encryptionServerOptions.setAuthentication(
        ntca::EncryptionAuthentication::e_NONE);

    {
        bsl::vector<char> identityData;
        serverCertificate->encode(&identityData);
        encryptionServerOptions.setIdentityData(identityData);
    }

    {
        bsl::vector<char> privateKeyData;
        serverPrivateKey->encode(&privateKeyData);
        encryptionServerOptions.setPrivateKeyData(privateKeyData);
    }

    bsl::shared_ptr<ntci::EncryptionServer> encryptionServer;
    error = interface->createEncryptionServer(&encryptionServer,
                                              encryptionServerOptions);
    BSLS_ASSERT_OPT(!error);

    // Create an encryption client and configure the encryption client to
    // request upgrades using TLS 1.2 require identification from the server,
    // and to trust the certificate authority previously generated to verify
    // the authenticity of the server.

    ntca::EncryptionClientOptions encryptionClientOptions;
    encryptionClientOptions.setMinMethod(ntca::EncryptionMethod::e_TLS_V1_2);
    encryptionClientOptions.setMaxMethod(ntca::EncryptionMethod::e_TLS_V1_2);
    encryptionClientOptions.setAuthentication(
        ntca::EncryptionAuthentication::e_VERIFY);

    {
        bsl::vector<char> authorityData;
        authorityCertificate->encode(&authorityData);
        encryptionClientOptions.addAuthorityData(authorityData);
    }

    bsl::shared_ptr<ntci::EncryptionClient> encryptionClient;
    error = interface->createEncryptionClient(&encryptionClient,
                                              encryptionClientOptions);
    BSLS_ASSERT_OPT(!error);

    // Create a server of the application wire protocol and begin listening
    // for incoming connections on a port chosen for that application wire
    // protocol.

    bsl::shared_ptr<example::Server> server;
    server.createInplace(allocator, interface, allocator);

    bsl::shared_ptr<example::ServerSocket> serverSocket;
    bslmt::Semaphore                       serverEstablished;

    ntsa::Endpoint listenerEndpoint =
        ntsa::Endpoint(ntsa::LocalName::generateUnique());

    listenerEndpoint =
        server->listen(listenerEndpoint,
                       NTCCFG_BIND(&example::processServerSocketEstablished,
                                   server,
                                   NTCCFG_BIND_PLACEHOLDER_1,
                                   &serverEstablished,
                                   &serverSocket));

    // Create a client of the application wire protocol and begin connecting
    // to the server.

    bsl::shared_ptr<example::Client> client;
    client.createInplace(allocator, interface, allocator);

    bsl::shared_ptr<example::ClientSocket> clientSocket;
    bslmt::Semaphore                       clientEstablished;

    client->connect(listenerEndpoint,
                    NTCCFG_BIND(&example::processClientSocketEstablished,
                                client,
                                NTCCFG_BIND_PLACEHOLDER_1,
                                &clientEstablished,
                                &clientSocket));

    // Wait for the server socket to become established.

    serverEstablished.wait();

    // Begin reading data from the server socket.

    serverSocket->receive(true);

    // Wait for the client session to become established.

    clientEstablished.wait();

    // Begin reading data from the client socket.

    clientSocket->receive(true);

    // Send a request in cleartext through the client channel.

    {
        bsl::string  response;
        bslmt::Latch responseLatch(1);

        clientSocket->send("Hello, unsecure world!",
                           NTCCFG_BIND(&example::processClientResponseReceived,
                                       &response,
                                       &responseLatch,
                                       NTCCFG_BIND_PLACEHOLDER_1));

        // Wait until a response is received and ensure the response matches
        // the request transformed into uppercase, as defined by the
        // application wire protocol.

        responseLatch.wait();
        BSLS_ASSERT_OPT(response == "HELLO, UNSECURE WORLD!");

        BSLS_LOG_INFO("Received '%s'", response.c_str());
    }

    // Begin waiting for the client to requst and upgrade to a secure channel.

    bslmt::Semaphore serverUpgraded;
    bslmt::Semaphore serverDowngraded;
    serverSocket->upgrade(encryptionServer,
                          NTCCFG_BIND(&example::processServerSocketEvent,
                                      server,
                                      serverSocket,
                                      &serverUpgraded),
                          NTCCFG_BIND(&example::processServerSocketEvent,
                                      server,
                                      serverSocket,
                                      &serverDowngraded));

    // Begin initiating a request to the server to upgrade to a secure channel.

    bslmt::Semaphore clientUpgraded;
    bslmt::Semaphore clientDowngraded;
    clientSocket->upgrade(encryptionClient,
                          NTCCFG_BIND(&example::processClientSocketEvent,
                                      client,
                                      clientSocket,
                                      &clientUpgraded),
                          NTCCFG_BIND(&example::processClientSocketEvent,
                                      client,
                                      clientSocket,
                                      &clientDowngraded));

    // Wait until the client and server have completed the TLS handshake.
    // At this point, the client has verified the authenticity of the server
    // and communication between the two peers is encrypted.

    clientUpgraded.wait();
    serverUpgraded.wait();

    // The client has possession of the server's certificate.

    {
        bsl::shared_ptr<ntci::EncryptionCertificate> peerCertificate =
            clientSocket->remoteCertificate();

        bsl::stringstream ss;
        peerCertificate->print(ss);

        BSLS_LOG_INFO("Secure channel established with:\n%s",
                      ss.str().c_str());
    }

    // Send a request in ciphertext through the client channel.

    {
        bsl::string  response;
        bslmt::Latch responseLatch(1);

        clientSocket->send("Hello, secure world!",
                           NTCCFG_BIND(&example::processClientResponseReceived,
                                       &response,
                                       &responseLatch,
                                       NTCCFG_BIND_PLACEHOLDER_1));

        // Wait until a response is received and ensure the response matches
        // the request transformed into uppercase, as defined by the
        // application wire protocol.

        responseLatch.wait();
        BSLS_ASSERT_OPT(response == "HELLO, SECURE WORLD!");

        BSLS_LOG_INFO("Received '%s'", response.c_str());
    }

    // Begin downgrading the secure channel back to cleartext. In our example
    // implementation, the server will automatically downgrade its side of the
    // connection to gracefully complete the TLS shutdown sequence when it
    // detects the client has begun the shutdown sequence.

    clientSocket->downgrade();

    // Wait until the client and server have completed the TLS shutdown
    // sequence. At this point, the client and server channel are back to
    // operating unsecurely, exchanging data in cleartext.

    serverDowngraded.wait();
    clientDowngraded.wait();

    // Send a request in cleartext through the client channel.

    {
        bsl::string  response;
        bslmt::Latch responseLatch(1);

        clientSocket->send("Hello, unsecure world again!",
                           NTCCFG_BIND(&example::processClientResponseReceived,
                                       &response,
                                       &responseLatch,
                                       NTCCFG_BIND_PLACEHOLDER_1));

        // Wait until a response is received and ensure the response matches
        // the request transformed into uppercase, as defined by the
        // application wire protocol.

        responseLatch.wait();
        BSLS_ASSERT_OPT(response == "HELLO, UNSECURE WORLD AGAIN!");

        BSLS_LOG_INFO("Received '%s'", response.c_str());
    }

    // Shutdown and wait for the client to stop.

    client->shutdown();
    client->linger();

    // Shutdown and wait for the server to stop.

    server->shutdown();
    server->linger();

    // Shutdown and wait for the asynchronous socket interface to stop.

    interface->shutdown();
    interface->linger();

#endif
#endif
}

}  // close namespace usage2

}  // close namespace example

//
// Running the 'example::usage2::execute' function results in something like
// the following printed to standard output:
//
//     Received 'HELLO, UNSECURE WORLD!'
//     Secure session established with:
//     Certificate:
//         Data:
//             Version: 3 (0x2)
//             Serial Number: 1 (0x1)
//             Signature Algorithm: sha256WithRSAEncryption
//             Issuer: CN=Authority,O=Bloomberg LP
//             Validity
//                 Not Before: May 11 14:40:52 2020 GMT
//                 Not After : May 11 14:40:52 2021 GMT
//             Subject: CN=Server,O=Bloomberg LP
//             Subject Public Key Info:
//                 Public Key Algorithm: rsaEncryption
//                     RSA Public-Key: (2048 bit)
//                     Modulus:
//                         00:bd:28:27:80:b2:0d:be:ef:44:c9:6a:9a:5e:a0:
//                         b5:0b:3f:7d:a8:eb:7c:7e:7d:90:48:06:da:79:c3:
//                         37:e7:0c:8a:8c:86:86:be:ac:fd:f4:fa:eb:0a:8c:
//                         3e:36:bd:0d:83:a0:3c:41:35:19:b5:0d:53:b0:03:
//                         64:6a:14:79:b0:36:67:2b:e0:2c:ff:a8:9a:cc:36:
//                         8c:da:fd:f9:26:0f:3a:01:cd:b5:ba:a9:79:88:e6:
//                         87:d7:b2:11:d5:79:c4:cc:ea:73:b2:86:83:b0:e5:
//                         e8:fa:df:60:62:ca:90:4e:db:5e:3c:0d:6e:35:4a:
//                         15:a2:be:1f:ba:03:08:f2:f5:7f:5f:07:d8:ac:39:
//                         a8:c1:73:38:27:3c:9e:83:b9:1e:38:58:a5:88:ab:
//                         7a:19:2e:72:7c:f8:76:8c:13:78:2a:8f:f5:c7:59:
//                         a9:e3:96:32:a8:d8:d6:43:e8:70:f1:0b:c0:6b:12:
//                         62:5d:2b:15:37:f3:e8:77:54:0d:7a:3e:47:2d:ef:
//                         b0:50:3e:d1:84:ea:d8:a7:f8:1e:77:32:c7:77:e6:
//                         8e:88:7f:eb:2a:1f:a1:9e:a4:15:8e:4a:c6:97:29:
//                         6b:3f:a9:39:64:92:31:9f:03:77:b6:de:68:0d:29:
//                         1a:9c:39:fb:37:e9:0d:ad:18:ca:ca:1a:46:f4:c6:
//                         6e:53
//                     Exponent: 65537 (0x10001)
//             X509v3 extensions:
//                 X509v3 Basic Constraints:
//                     CA:FALSE
//
//         Signature Algorithm: sha256WithRSAEncryption
//              b7:ba:1a:34:cf:0f:2f:62:ea:65:93:98:a8:4a:1d:c4:a3:a4:
//              53:90:e8:74:23:90:b3:43:ae:6c:f5:25:a4:a9:1e:cb:70:5d:
//              d9:49:2b:9a:54:4e:24:99:fe:e4:5a:c4:81:40:f9:fe:11:bc:
//              eb:da:f0:21:2c:63:6c:36:54:fc:7f:86:10:e1:fa:24:f0:5b:
//              b7:56:33:a7:a2:f8:82:b5:12:ec:58:25:15:01:94:07:28:40:
//              84:e8:29:1c:99:11:cd:be:24:4d:cb:04:1d:bf:9b:73:a5:62:
//              b3:f7:e1:84:ec:2a:2c:77:06:8e:8d:c0:91:1d:6b:f9:58:af:
//              af:a7:d4:c0:2e:fb:33:6c:04:40:04:a3:82:83:21:ed:dc:72:
//              7b:56:8a:c3:6d:09:ab:68:99:47:1b:41:61:79:ef:c2:41:c8:
//              79:60:0a:2e:ec:b8:d2:33:eb:75:e0:7c:b0:81:9b:80:a5:24:
//              61:7d:7a:89:f1:b1:92:c2:ad:d3:1b:77:d4:12:2b:ac:a9:43:
//              73:94:f3:a7:d2:a3:ef:75:e9:fd:aa:2c:09:c4:38:ec:61:29:
//              82:63:13:a0:c4:e9:19:0a:ea:73:3c:0f:4e:09:f0:24:35:88:
//              f5:27:a3:ef:63:1f:b3:a5:9e:56:34:64:d5:d0:ee:7f:b2:bd:
//              12:43:a9:1e
//
//     Received 'HELLO, SECURE WORLD!'
//     Received 'HELLO, UNSECURE WORLD AGAIN!'
//

void help()
{
    bsl::cout << "usage: <program>> [-v <level>]" << bsl::endl;
}

int main(int argc, char** argv)
{
    ntcf::System::initialize();
    ntcf::System::ignore(ntscfg::Signal::e_PIPE);

    int verbosity = 0;
    {
        int i = 1;
        while (i < argc) {
            if ((0 == std::strcmp(argv[i], "-?")) ||
                (0 == std::strcmp(argv[i], "--help")))
            {
                help();
                return 0;
            }

            if (0 == std::strcmp(argv[i], "-v") ||
                0 == std::strcmp(argv[i], "--verbosity"))
            {
                ++i;
                if (i >= argc) {
                    help();
                    return 1;
                }
                verbosity = std::atoi(argv[i]);
                ++i;
                continue;
            }

            bsl::cerr << "Invalid option: " << argv[i] << bsl::endl;
            return 1;
        }
    }

    switch (verbosity) {
    case 0:
        break;
    case 1:
        bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_ERROR);
        break;
    case 2:
        bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_WARN);
        break;
    case 3:
        bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_INFO);
        break;
    case 4:
        bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_DEBUG);
        break;
    default:
        bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_TRACE);
        break;
    }

    example::usage1::execute();
    example::usage2::execute();

    return 0;
}
