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
BSLS_IDENT_RCSID(ntcd_encryption_t_cpp, "$Id$ $CSID$")

#include <ntcd_encryption.h>

#include <ntccfg_bind.h>
#include <ntci_log.h>
#include <ntcs_datapool.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcd {

// Provide tests for 'ntcd::Encryption'.
class EncryptionTest
{
    /// Describes the configurable parameters of the test driver.
    struct Parameters {
        Parameters()
        : d_bufferSize(32)
        , d_numReuses(0)
        , d_clientRejectsServer(false)
        , d_serverRejectsClient(false)
        , d_success(true)
        {
        }

        bsl::size_t d_bufferSize;
        bsl::size_t d_numReuses;
        bool        d_clientRejectsServer;
        bool        d_serverRejectsClient;
        bool        d_success;
    };

    // Log the specified 'parameters' prefixed by the specified 'label'.
    static void logParameters(const char* label, const Parameters& parameters);

    // Log the hex dump of the specified 'blob' prefixed by the specified
    // 'label'.
    static void logHexDump(const char* label, const bdlbb::Blob& blob);

    // Log the specified 'certificate' prefixed by the specified 'label'.
    static void logCertificate(
        const char*                                         label,
        const bsl::shared_ptr<ntcd::EncryptionCertificate>& certificate);

    // Authenticate the specified 'client' certificate'. Return the error.
    static ntsa::Error processClientAuthenticationByServer(
        const bsl::shared_ptr<ntcd::EncryptionCertificate>& client);

    // Authenticate the specified 'client' certificate'. Return the error.
    static ntsa::Error processServerAuthenticationByClient(
        const bsl::shared_ptr<ntcd::EncryptionCertificate>& server);

    // Process the completion or failure of the handshake of the specified
    // 'clientSession' according to the specified 'error'. The session is
    // established with server identified by the specified 'serverCertificate',
    // if any. Set the specified 'clientCompleteFlag' to true.
    static void processClientHandshakeComplete(
        const ntsa::Error&                                  error,
        const bsl::shared_ptr<ntcd::Encryption>&            clientSession,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& serverCertificate,
        const bsl::string&                                  details,
        bool*                                               clientCompleteFlag,
        const Parameters&                                   parameters);

    // Process the completion or failure of the handshake of the specified
    // 'serverSession' according to the specified 'error'. The session is
    // established with server identified by the specified 'serverCertificate',
    // if any. Set the specified 'serverCompleteFlag' to true.
    static void processServerHandshakeComplete(
        const ntsa::Error&                                  error,
        const bsl::shared_ptr<ntcd::Encryption>&            serverSession,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& clientCertificate,
        const bsl::string&                                  details,
        bool*                                               serverCompleteFlag,
        const Parameters&                                   parameters);

    // Repeatedly read and write from the specified 'clientSession' and
    // 'serverSession' until both the specified 'clientCompleteFlag' and
    // 'serverCompleteFlag' is set.
    static void cycleHandshake(
        const bsl::shared_ptr<ntcd::Encryption>& clientSession,
        const bsl::shared_ptr<ntcd::Encryption>& serverSession,
        const bsl::shared_ptr<ntci::DataPool>&   dataPool,
        bdlbb::Blob*                             clientPlaintextRead,
        bdlbb::Blob*                             serverPlaintextRead,
        const Parameters&                        parameters);

    // Repeatedly read and write from the specified 'clientSession' and
    // 'serverSession' until both the specified 'clientCompleteFlag' and
    // 'serverCompleteFlag' is set.
    static void cycleShutdown(
        const bsl::shared_ptr<ntcd::Encryption>& clientSession,
        const bsl::shared_ptr<ntcd::Encryption>& serverSession,
        const bsl::shared_ptr<ntci::DataPool>&   dataPool,
        bdlbb::Blob*                             clientPlaintextRead,
        bdlbb::Blob*                             serverPlaintextRead,
        const Parameters&                        parameters);

    // Execute the test described by the specified 'parameters' using the
    // specified 'authorityCertificate' and 'authorityPrivateKey' for the
    // trusted certificate authority, the specified 'clientCertificate' and
    // 'clientPrivateKey' for the client, and the specified 'serverCertificate'
    // and 'serverPrivateKey'.
    static void execute(const Parameters& parameters,
                        const bsl::shared_ptr<ntcd::EncryptionDriver>& driver);

  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifyCase3();

    // TODO
    static void verifyCase4();

    // TODO
    static void verifyCase5();
};

void EncryptionTest::logParameters(const char*       label,
                                   const Parameters& parameters)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_DEBUG("%s = [ "
                  "BS = %d "
                  "numReuses = %d "
                  "rc = %d ]",
                  label,
                  (int)(parameters.d_bufferSize),
                  (int)(parameters.d_numReuses),
                  (int)(parameters.d_success));
}

void EncryptionTest::logHexDump(const char* label, const bdlbb::Blob& blob)
{
    NTCI_LOG_CONTEXT();

    bsl::stringstream ss;
    ss << bdlbb::BlobUtilHexDumper(&blob);

    NTCI_LOG_DEBUG("%s %d bytes", label, (int)(blob.length()));

    NTCI_LOG_TRACE("%s:\n%s", label, ss.str().c_str());
}

void EncryptionTest::logCertificate(
    const char*                                         label,
    const bsl::shared_ptr<ntcd::EncryptionCertificate>& certificate)
{
    NTCI_LOG_CONTEXT();

    bsl::stringstream ss;
    certificate->print(ss);

    NTCI_LOG_DEBUG("%s:\n%s", label, ss.str().c_str());
}

ntsa::Error EncryptionTest::processClientAuthenticationByServer(
    const bsl::shared_ptr<ntcd::EncryptionCertificate>& client)
{
    logCertificate("Server authenticated client", client);
    return ntsa::Error();
}

ntsa::Error EncryptionTest::processServerAuthenticationByClient(
    const bsl::shared_ptr<ntcd::EncryptionCertificate>& server)
{
    logCertificate("Client authenticated server", server);
    return ntsa::Error();
}

void EncryptionTest::processClientHandshakeComplete(
    const ntsa::Error&                                  error,
    const bsl::shared_ptr<ntcd::Encryption>&            clientSession,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& serverCertificate,
    const bsl::string&                                  details,
    bool*                                               clientCompleteFlag,
    const Parameters&                                   parameters)
{
    NTCI_LOG_CONTEXT();

    NTCCFG_WARNING_UNUSED(serverCertificate);
    NTCCFG_WARNING_UNUSED(parameters);

    if (!error) {
        bsl::string cipher;
        bool        found = clientSession->getCipher(&cipher);
        NTSCFG_TEST_TRUE(found);

        NTCI_LOG_DEBUG("Client handshake complete: %s", cipher.c_str());
    }
    else {
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED));
        NTCI_LOG_DEBUG("Client handshake failed: %s", details.c_str());
    }

    *clientCompleteFlag = true;
}

void EncryptionTest::processServerHandshakeComplete(
    const ntsa::Error&                                  error,
    const bsl::shared_ptr<ntcd::Encryption>&            serverSession,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& clientCertificate,
    const bsl::string&                                  details,
    bool*                                               serverCompleteFlag,
    const Parameters&                                   parameters)
{
    NTCI_LOG_CONTEXT();

    NTCCFG_WARNING_UNUSED(clientCertificate);
    NTCCFG_WARNING_UNUSED(parameters);

    if (!error) {
        bsl::string cipher;
        bool        found = serverSession->getCipher(&cipher);
        NTSCFG_TEST_TRUE(found);

        NTCI_LOG_DEBUG("Server handshake complete: %s", cipher.c_str());
    }
    else {
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED));
        NTCI_LOG_DEBUG("Server handshake failed: %s", details.c_str());
    }

    *serverCompleteFlag = true;
}

void EncryptionTest::cycleHandshake(
    const bsl::shared_ptr<ntcd::Encryption>& clientSession,
    const bsl::shared_ptr<ntcd::Encryption>& serverSession,
    const bsl::shared_ptr<ntci::DataPool>&   dataPool,
    bdlbb::Blob*                             clientPlaintextRead,
    bdlbb::Blob*                             serverPlaintextRead,
    const Parameters&                        parameters)
{
    NTCI_LOG_CONTEXT();

    NTCCFG_WARNING_UNUSED(parameters);

    ntsa::Error error;

    while (!clientSession->isHandshakeFinished() ||
           !serverSession->isHandshakeFinished())
    {
        {
            NTCI_LOG_CONTEXT_GUARD_OWNER("client");

            if (clientSession->hasOutgoingCipherText()) {
                bdlbb::Blob data(dataPool->outgoingBlobBufferFactory().get());
                clientSession->popOutgoingCipherText(&data);

                logHexDump("Client sending ciphertext", data);

                {
                    NTCI_LOG_CONTEXT_GUARD_OWNER("server");

                    error = serverSession->pushIncomingCipherText(data);
                    NTSCFG_TEST_FALSE(error);
                }
            }
        }

        {
            NTCI_LOG_CONTEXT_GUARD_OWNER("server");

            if (serverSession->hasOutgoingCipherText()) {
                bdlbb::Blob data(dataPool->outgoingBlobBufferFactory().get());
                serverSession->popOutgoingCipherText(&data);

                logHexDump("Server sending ciphertext", data);

                {
                    NTCI_LOG_CONTEXT_GUARD_OWNER("client");

                    error = clientSession->pushIncomingCipherText(data);
                    NTSCFG_TEST_FALSE(error);
                }
            }
        }

        {
            NTCI_LOG_CONTEXT_GUARD_OWNER("client");

            if (clientSession->hasIncomingPlainText()) {
                bdlbb::Blob data(dataPool->incomingBlobBufferFactory().get());
                error = clientSession->popIncomingPlainText(&data);
                NTSCFG_TEST_FALSE(error);

                logHexDump("Client received plaintext", data);

                bdlbb::BlobUtil::append(clientPlaintextRead, data);
            }
        }

        {
            NTCI_LOG_CONTEXT_GUARD_OWNER("server");

            if (serverSession->hasIncomingPlainText()) {
                bdlbb::Blob data(dataPool->incomingBlobBufferFactory().get());
                error = serverSession->popIncomingPlainText(&data);
                NTSCFG_TEST_FALSE(error);

                logHexDump("Server received plaintext", data);

                bdlbb::BlobUtil::append(serverPlaintextRead, data);
            }
        }
    }
}

void EncryptionTest::cycleShutdown(
    const bsl::shared_ptr<ntcd::Encryption>& clientSession,
    const bsl::shared_ptr<ntcd::Encryption>& serverSession,
    const bsl::shared_ptr<ntci::DataPool>&   dataPool,
    bdlbb::Blob*                             clientPlaintextRead,
    bdlbb::Blob*                             serverPlaintextRead,
    const Parameters&                        parameters)
{
    NTCI_LOG_CONTEXT();

    NTCCFG_WARNING_UNUSED(parameters);

    ntsa::Error error;

    while (!clientSession->isShutdownFinished() ||
           !serverSession->isShutdownFinished())
    {
        {
            NTCI_LOG_CONTEXT_GUARD_OWNER("client");

            if (clientSession->hasOutgoingCipherText()) {
                bdlbb::Blob data(dataPool->outgoingBlobBufferFactory().get());
                clientSession->popOutgoingCipherText(&data);

                logHexDump("Client sending ciphertext", data);

                {
                    NTCI_LOG_CONTEXT_GUARD_OWNER("server");

                    error = serverSession->pushIncomingCipherText(data);
                    NTSCFG_TEST_FALSE(error);
                }
            }
        }

        {
            NTCI_LOG_CONTEXT_GUARD_OWNER("server");

            if (serverSession->hasOutgoingCipherText()) {
                bdlbb::Blob data(dataPool->outgoingBlobBufferFactory().get());
                serverSession->popOutgoingCipherText(&data);

                logHexDump("Server sending ciphertext", data);

                {
                    NTCI_LOG_CONTEXT_GUARD_OWNER("client");

                    error = clientSession->pushIncomingCipherText(data);
                    NTSCFG_TEST_FALSE(error);
                }
            }
        }

        {
            NTCI_LOG_CONTEXT_GUARD_OWNER("client");

            if (clientSession->hasIncomingPlainText()) {
                bdlbb::Blob data(dataPool->incomingBlobBufferFactory().get());
                error = clientSession->popIncomingPlainText(&data);
                NTSCFG_TEST_FALSE(error);

                logHexDump("Client received plaintext", data);

                bdlbb::BlobUtil::append(clientPlaintextRead, data);
            }
        }

        {
            NTCI_LOG_CONTEXT_GUARD_OWNER("server");

            if (serverSession->hasIncomingPlainText()) {
                bdlbb::Blob data(dataPool->incomingBlobBufferFactory().get());
                error = serverSession->popIncomingPlainText(&data);
                NTSCFG_TEST_FALSE(error);

                logHexDump("Server received plaintext", data);

                bdlbb::BlobUtil::append(serverPlaintextRead, data);
            }
        }
    }
}

void EncryptionTest::execute(
    const Parameters&                              parameters,
    const bsl::shared_ptr<ntcd::EncryptionDriver>& driver)
{
    NTCI_LOG_CONTEXT();

    logParameters("Testing parameters", parameters);

    ntsa::Error error;

    // Create the data pool used by each session.

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(NTSCFG_TEST_ALLOCATOR,
                           parameters.d_bufferSize,
                           parameters.d_bufferSize,
                           NTSCFG_TEST_ALLOCATOR);

    bdlt::DatetimeTz startTime(bdlt::Datetime(2000, 1, 1), 0);
    bdlt::DatetimeTz expirationTime(bdlt::Datetime(2100, 1, 1), 0);

    typedef ntsa::DistinguishedName DN;

    // Create the authority certificate and key.

    ntsa::DistinguishedName authorityIdentity;
    authorityIdentity[DN::e_COMMON_NAME].addAttribute("authority");

    ntca::EncryptionKeyOptions authorityKeyOptions;

    bsl::shared_ptr<ntcd::EncryptionKey> authorityKey;
    authorityKey.createInplace(NTSCFG_TEST_ALLOCATOR);

    error = authorityKey->generate(authorityKeyOptions);
    NTSCFG_TEST_OK(error);

    ntca::EncryptionCertificateOptions authorityCertificateOptions;
    authorityCertificateOptions.setAuthority(true);
    authorityCertificateOptions.setSerialNumber(1);
    authorityCertificateOptions.setStartTime(startTime);
    authorityCertificateOptions.setExpirationTime(expirationTime);

    bsl::shared_ptr<ntcd::EncryptionCertificate> authorityCertificate;
    authorityCertificate.createInplace(NTSCFG_TEST_ALLOCATOR,
                                       NTSCFG_TEST_ALLOCATOR);

    error = authorityCertificate->generate(authorityIdentity,
                                           authorityKey,
                                           authorityCertificateOptions);
    NTSCFG_TEST_OK(error);

    // Create the client certificate and key.

    ntsa::DistinguishedName clientIdentity;
    clientIdentity[DN::e_COMMON_NAME].addAttribute("client");

    ntca::EncryptionKeyOptions clientKeyOptions;

    bsl::shared_ptr<ntcd::EncryptionKey> clientKey;
    clientKey.createInplace(NTSCFG_TEST_ALLOCATOR);

    error = clientKey->generate(clientKeyOptions);
    NTSCFG_TEST_OK(error);

    ntca::EncryptionCertificateOptions clientCertificateOptions;
    clientCertificateOptions.setAuthority(false);
    clientCertificateOptions.setSerialNumber(2);
    clientCertificateOptions.setStartTime(startTime);
    clientCertificateOptions.setExpirationTime(expirationTime);

    bsl::shared_ptr<ntcd::EncryptionCertificate> clientCertificate;
    clientCertificate.createInplace(NTSCFG_TEST_ALLOCATOR,
                                    NTSCFG_TEST_ALLOCATOR);

    error = clientCertificate->generate(clientIdentity,
                                        clientKey,
                                        authorityCertificate,
                                        authorityKey,
                                        clientCertificateOptions);
    NTSCFG_TEST_OK(error);

    // Create the server certificate and key.

    ntsa::DistinguishedName serverIdentity;
    serverIdentity[DN::e_COMMON_NAME].addAttribute("server");

    ntca::EncryptionKeyOptions serverKeyOptions;

    bsl::shared_ptr<ntcd::EncryptionKey> serverKey;
    serverKey.createInplace(NTSCFG_TEST_ALLOCATOR);

    error = serverKey->generate(serverKeyOptions);
    NTSCFG_TEST_OK(error);

    ntca::EncryptionCertificateOptions serverCertificateOptions;
    serverCertificateOptions.setAuthority(false);
    serverCertificateOptions.setSerialNumber(2);
    serverCertificateOptions.setStartTime(startTime);
    serverCertificateOptions.setExpirationTime(expirationTime);

    bsl::shared_ptr<ntcd::EncryptionCertificate> serverCertificate;
    serverCertificate.createInplace(NTSCFG_TEST_ALLOCATOR,
                                    NTSCFG_TEST_ALLOCATOR);

    error = serverCertificate->generate(serverIdentity,
                                        serverKey,
                                        authorityCertificate,
                                        authorityKey,
                                        serverCertificateOptions);
    NTSCFG_TEST_OK(error);

    // Create the client session.

    bsl::shared_ptr<ntcd::Encryption> clientSession;
    clientSession.createInplace(NTSCFG_TEST_ALLOCATOR,
                                ntca::EncryptionRole::e_CLIENT,
                                clientCertificate,
                                clientKey,
                                dataPool,
                                NTSCFG_TEST_ALLOCATOR);

    if (!parameters.d_clientRejectsServer) {
        clientSession->authorizePeer("server");
    }

    // Create the server session.

    bsl::shared_ptr<ntcd::Encryption> serverSession;
    serverSession.createInplace(NTSCFG_TEST_ALLOCATOR,
                                ntca::EncryptionRole::e_SERVER,
                                serverCertificate,
                                serverKey,
                                dataPool,
                                NTSCFG_TEST_ALLOCATOR);

    if (!parameters.d_serverRejectsClient) {
        serverSession->authorizePeer("client");
    }

    // Create the test state variables.

    bdlbb::Blob helloServer(dataPool->outgoingBlobBufferFactory().get());
    bdlbb::BlobUtil::append(&helloServer, "Hello, server!", 0, 14);

    bdlbb::Blob helloClient(dataPool->outgoingBlobBufferFactory().get());
    bdlbb::BlobUtil::append(&helloClient, "Hello, client!", 0, 14);

    bdlbb::Blob goodbyeServer(dataPool->outgoingBlobBufferFactory().get());
    bdlbb::BlobUtil::append(&goodbyeServer, "Goodbye, server!", 0, 16);

    bdlbb::Blob goodbyeClient(dataPool->outgoingBlobBufferFactory().get());
    bdlbb::BlobUtil::append(&goodbyeClient, "Goodbye, client!", 0, 16);

    bdlbb::Blob expectedClientPlaintextRead(
        dataPool->incomingBlobBufferFactory().get(),
        NTSCFG_TEST_ALLOCATOR);
    bdlbb::BlobUtil::append(&expectedClientPlaintextRead, helloClient);
    bdlbb::BlobUtil::append(&expectedClientPlaintextRead, goodbyeClient);

    bdlbb::Blob expectedServerPlaintextRead(
        dataPool->incomingBlobBufferFactory().get(),
        NTSCFG_TEST_ALLOCATOR);
    bdlbb::BlobUtil::append(&expectedServerPlaintextRead, helloServer);
    bdlbb::BlobUtil::append(&expectedServerPlaintextRead, goodbyeServer);

    for (bsl::size_t usageIteration = 0;
         usageIteration < parameters.d_numReuses + 1;
         ++usageIteration)
    {
        NTCI_LOG_DEBUG("Iteration %d/%d starting",
                      usageIteration + 1,
                      parameters.d_numReuses + 1);

        bool clientHandshakeComplete = false;
        bool serverHandshakeComplete = false;

        bdlbb::Blob clientPlaintextRead(
            dataPool->incomingBlobBufferFactory().get(),
            NTSCFG_TEST_ALLOCATOR);
        bdlbb::Blob serverPlaintextRead(
            dataPool->incomingBlobBufferFactory().get(),
            NTSCFG_TEST_ALLOCATOR);

        // Initiate the handshake from the client.

        {
            NTCI_LOG_CONTEXT_GUARD_OWNER("client");

            NTCI_LOG_DEBUG("Client handshake initiating");

            error = clientSession->initiateHandshake(
                NTCCFG_BIND(&processClientHandshakeComplete,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            clientSession,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            &clientHandshakeComplete,
                            parameters));
            NTSCFG_TEST_FALSE(error);
        }

        // Initiate the handshake from the server.

        {
            NTCI_LOG_CONTEXT_GUARD_OWNER("server");

            NTCI_LOG_DEBUG("Server handshake initiating");

            error = serverSession->initiateHandshake(
                NTCCFG_BIND(&processServerHandshakeComplete,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            serverSession,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            &serverHandshakeComplete,
                            parameters));
            NTSCFG_TEST_FALSE(error);
        }

        // Send data immediately after the handshake is initiated.

        {
            NTCI_LOG_CONTEXT_GUARD_OWNER("client");

            error = clientSession->pushOutgoingPlainText(helloServer);
            NTSCFG_TEST_FALSE(error);
        }

        {
            NTCI_LOG_CONTEXT_GUARD_OWNER("server");

            error = serverSession->pushOutgoingPlainText(helloClient);
            NTSCFG_TEST_FALSE(error);
        }

        // Process the TLS state machine until the handshake is complete.

        cycleHandshake(clientSession,
                       serverSession,
                       dataPool,
                       &clientPlaintextRead,
                       &serverPlaintextRead,
                       parameters);

        if (!parameters.d_success) {
            return;
        }

        NTSCFG_TEST_TRUE(clientHandshakeComplete);
        NTSCFG_TEST_TRUE(serverHandshakeComplete);

        // Send data immediately before the shutdown is initiated.

        {
            NTCI_LOG_CONTEXT_GUARD_OWNER("client");

            error = clientSession->pushOutgoingPlainText(goodbyeServer);
            NTSCFG_TEST_FALSE(error);
        }

        {
            NTCI_LOG_CONTEXT_GUARD_OWNER("server");

            error = serverSession->pushOutgoingPlainText(goodbyeClient);
            NTSCFG_TEST_FALSE(error);
        }

        // Initiate the shutdown from the client.

        {
            NTCI_LOG_CONTEXT_GUARD_OWNER("client");

            NTCI_LOG_DEBUG("Client shutdown initiating");

            error = clientSession->shutdown();
            NTSCFG_TEST_FALSE(error);
        }

        // Initiate the shutdown from the server.

        {
            NTCI_LOG_CONTEXT_GUARD_OWNER("server");

            NTCI_LOG_DEBUG("Server shutdown initiating");

            error = serverSession->shutdown();
            NTSCFG_TEST_FALSE(error);
        }

        // Process the TLS state machine until the shutdown is complete.

        cycleShutdown(clientSession,
                      serverSession,
                      dataPool,
                      &clientPlaintextRead,
                      &serverPlaintextRead,
                      parameters);

        // Ensure the plaintext received by the client matches the expected
        // data.

        NTSCFG_TEST_EQ(bdlbb::BlobUtil::compare(clientPlaintextRead,
                                                expectedClientPlaintextRead),
                       0);

        // Ensure the plaintext received by the server matches the expected
        // data.

        NTSCFG_TEST_EQ(bdlbb::BlobUtil::compare(serverPlaintextRead,
                                                expectedServerPlaintextRead),
                       0);

        NTCI_LOG_DEBUG("Iteration %d/%d complete",
                      usageIteration + 1,
                      parameters.d_numReuses + 1);
    }

    NTCI_LOG_DEBUG("Test complete");
}

NTSCFG_TEST_FUNCTION(ntcd::EncryptionTest::verifyCase1)
{
    // Concern: Encryption key.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error        error;
    ntsa::StreamBuffer buffer(NTSCFG_TEST_ALLOCATOR);

    ntca::EncryptionKeyOptions      keyOptions;
    ntca::EncryptionResourceOptions keyStorageOptions;

    bsl::shared_ptr<ntcd::EncryptionKey> key1;
    key1.createInplace(NTSCFG_TEST_ALLOCATOR);

    error = key1->generate(keyOptions);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Key1 = " << *key1 << NTCI_LOG_STREAM_END;

    error = key1->encode(&buffer, keyStorageOptions);
    NTSCFG_TEST_OK(error);

    bsl::shared_ptr<ntcd::EncryptionKey> key2;
    key2.createInplace(NTSCFG_TEST_ALLOCATOR);

    error = key2->decode(&buffer, keyStorageOptions);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Key2 = " << *key2 << NTCI_LOG_STREAM_END;

    NTSCFG_TEST_EQ(key1->value(), key2->value());
}

NTSCFG_TEST_FUNCTION(ntcd::EncryptionTest::verifyCase2)
{
    // Concern: Encryption certificate.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntsa::StreamBuffer buffer(NTSCFG_TEST_ALLOCATOR);

    ntca::EncryptionKeyOptions      keyOptions;
    ntca::EncryptionResourceOptions keyStorageOptions;

    ntca::EncryptionCertificateOptions certificateOptions;
    ntca::EncryptionResourceOptions    certificateStorageOptions;

    bsl::shared_ptr<ntcd::EncryptionKey> key1;
    key1.createInplace(NTSCFG_TEST_ALLOCATOR);

    error = key1->generate(keyOptions);
    NTSCFG_TEST_OK(error);

    ntsa::DistinguishedName identity1;
    identity1[ntsa::DistinguishedName::e_COMMON_NAME].addAttribute("John Doe");
    identity1[ntsa::DistinguishedName::e_ORGANIZATION_NAME].addAttribute(
        "Bloomberg");

    bsl::shared_ptr<ntcd::EncryptionCertificate> certificate1;
    certificate1.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    error = certificate1->generate(identity1, key1, certificateOptions);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Certificate1 = " << *certificate1
                          << NTCI_LOG_STREAM_END;

    error = certificate1->encode(&buffer, certificateStorageOptions);
    NTSCFG_TEST_OK(error);

    bsl::shared_ptr<ntcd::EncryptionCertificate> certificate2;
    certificate2.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    error = certificate2->decode(&buffer, certificateStorageOptions);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_DEBUG << "Certificate2 = " << *certificate2
                          << NTCI_LOG_STREAM_END;

    ntsa::DistinguishedName identity2 = certificate2->subject();

    bsl::string identityText1;
    rc = identity1.generate(&identityText1);
    NTSCFG_TEST_EQ(rc, 0);

    bsl::string identityText2;
    rc = identity2.generate(&identityText2);
    NTSCFG_TEST_EQ(rc, 0);

    NTSCFG_TEST_EQ(identityText1, identityText2);
}

NTSCFG_TEST_FUNCTION(ntcd::EncryptionTest::verifyCase3)
{
    // Concern: Successful encryption.

    bsl::shared_ptr<ntcd::EncryptionDriver> driver;
    driver.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    Parameters parameters;
    parameters.d_bufferSize          = 32;
    parameters.d_clientRejectsServer = false;
    parameters.d_serverRejectsClient = false;
    parameters.d_success             = true;

    execute(parameters, driver);
}

NTSCFG_TEST_FUNCTION(ntcd::EncryptionTest::verifyCase4)
{
    // Concern: Client reject server.

    bsl::shared_ptr<ntcd::EncryptionDriver> driver;
    driver.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    Parameters parameters;
    parameters.d_bufferSize          = 32;
    parameters.d_clientRejectsServer = true;
    parameters.d_serverRejectsClient = false;
    parameters.d_success             = false;

    execute(parameters, driver);
}

NTSCFG_TEST_FUNCTION(ntcd::EncryptionTest::verifyCase5)
{
    // Concern: Server rejects client.

    bsl::shared_ptr<ntcd::EncryptionDriver> driver;
    driver.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    Parameters parameters;
    parameters.d_bufferSize          = 32;
    parameters.d_clientRejectsServer = false;
    parameters.d_serverRejectsClient = true;
    parameters.d_success             = false;

    execute(parameters, driver);
}

}  // close namespace ntcd
}  // close namespace BloombergLP
