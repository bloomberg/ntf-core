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

#include <ntcd_encryption.h>

#include <ntccfg_bind.h>
#include <ntccfg_test.h>
#include <ntci_log.h>
#include <ntcs_datapool.h>

#include <bdlb_print.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobstreambuf.h>
#include <bdlbb_blobutil.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_memoutstreambuf.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_functional.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------

// [ 1]
//-----------------------------------------------------------------------------
// [ 1]
//-----------------------------------------------------------------------------

#define NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_MAIN()                              \
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

#define NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_CLIENT()                            \
    NTCI_LOG_CONTEXT_GUARD_OWNER("client");

#define NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_SERVER()                            \
    NTCI_LOG_CONTEXT_GUARD_OWNER("server");

namespace test {

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

void logParameters(const char* label, const test::Parameters& parameters)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_INFO("%s = [ "
                  "BS = %d "
                  "numReuses = %d "
                  "rc = %d ]",
                  label,
                  (int)(parameters.d_bufferSize),
                  (int)(parameters.d_numReuses),
                  (int)(parameters.d_success));
}

void logHexDump(const char* label, const bdlbb::Blob& blob)
{
    // Log the hex dump of the specified 'blob' prefixed by the specified
    // 'label'.

    NTCI_LOG_CONTEXT();

    bsl::stringstream ss;
    ss << bdlbb::BlobUtilHexDumper(&blob);

    NTCI_LOG_DEBUG("%s %d bytes", label, (int)(blob.length()));

    NTCI_LOG_TRACE("%s:\n%s", label, ss.str().c_str());
}

void logCertificate(
    const char*                                         label,
    const bsl::shared_ptr<ntcd::EncryptionCertificate>& certificate)
{
    // Log the specified 'certificate' prefixed by the specified 'label'.

    NTCI_LOG_CONTEXT();

    bsl::stringstream ss;
    certificate->print(ss);

    NTCI_LOG_DEBUG("%s:\n%s", label, ss.str().c_str());
}

ntsa::Error processClientAuthenticationByServer(
    const bsl::shared_ptr<ntcd::EncryptionCertificate>& client)
{
    // Authenticate the specified 'client' certificate'. Return the error.

    logCertificate("Server authenticated client", client);
    return ntsa::Error();
}

ntsa::Error processServerAuthenticationByClient(
    const bsl::shared_ptr<ntcd::EncryptionCertificate>& server)
{
    // Authenticate the specified 'client' certificate'. Return the error.

    logCertificate("Client authenticated server", server);
    return ntsa::Error();
}

void processClientHandshakeComplete(
    const ntsa::Error&                                  error,
    const bsl::shared_ptr<ntcd::Encryption>&            clientSession,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& serverCertificate,
    const bsl::string&                                  details,
    bool*                                               clientCompleteFlag,
    const test::Parameters&                             parameters)
{
    // Process the completion or failure of the handshake of the specified
    // 'clientSession' according to the specified 'error'. The session is
    // established with server identified by the specified 'serverCertificate',
    // if any. Set the specified 'clientCompleteFlag' to true.

    NTCI_LOG_CONTEXT();

    NTCCFG_WARNING_UNUSED(serverCertificate);
    NTCCFG_WARNING_UNUSED(parameters);

    if (!error) {
        bsl::string cipher;
        bool        found = clientSession->getCipher(&cipher);
        NTCCFG_TEST_TRUE(found);

        NTCI_LOG_INFO("Client handshake complete: %s", cipher.c_str());
    }
    else {
        NTCCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED));
        NTCI_LOG_INFO("Client handshake failed: %s", details.c_str());
    }

    *clientCompleteFlag = true;
}

void processServerHandshakeComplete(
    const ntsa::Error&                                  error,
    const bsl::shared_ptr<ntcd::Encryption>&            serverSession,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& clientCertificate,
    const bsl::string&                                  details,
    bool*                                               serverCompleteFlag,
    const test::Parameters&                             parameters)
{
    // Process the completion or failure of the handshake of the specified
    // 'serverSession' according to the specified 'error'. The session is
    // established with server identified by the specified 'serverCertificate',
    // if any. Set the specified 'serverCompleteFlag' to true.

    NTCI_LOG_CONTEXT();

    NTCCFG_WARNING_UNUSED(clientCertificate);
    NTCCFG_WARNING_UNUSED(parameters);

    if (!error) {
        bsl::string cipher;
        bool        found = serverSession->getCipher(&cipher);
        NTCCFG_TEST_TRUE(found);

        NTCI_LOG_INFO("Server handshake complete: %s", cipher.c_str());
    }
    else {
        NTCCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED));
        NTCI_LOG_INFO("Server handshake failed: %s", details.c_str());
    }

    *serverCompleteFlag = true;
}

void cycleHandshake(const bsl::shared_ptr<ntcd::Encryption>& clientSession,
                    const bsl::shared_ptr<ntcd::Encryption>& serverSession,
                    const bsl::shared_ptr<ntci::DataPool>&   dataPool,
                    bdlbb::Blob*            clientPlaintextRead,
                    bdlbb::Blob*            serverPlaintextRead,
                    const test::Parameters& parameters)
{
    // Repeatedly read and write from the specified 'clientSession' and
    // 'serverSession' until both the specified 'clientCompleteFlag' and
    // 'serverCompleteFlag' is set.

    NTCI_LOG_CONTEXT();

    NTCCFG_WARNING_UNUSED(parameters);

    ntsa::Error error;

    while (!clientSession->isHandshakeFinished() ||
           !serverSession->isHandshakeFinished())
    {
        {
            NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_CLIENT();

            if (clientSession->hasOutgoingCipherText()) {
                bdlbb::Blob data(dataPool->outgoingBlobBufferFactory().get());
                clientSession->popOutgoingCipherText(&data);

                test::logHexDump("Client sending ciphertext", data);

                {
                    NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_SERVER();

                    error = serverSession->pushIncomingCipherText(data);
                    NTCCFG_TEST_FALSE(error);
                }
            }
        }

        {
            NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_SERVER();

            if (serverSession->hasOutgoingCipherText()) {
                bdlbb::Blob data(dataPool->outgoingBlobBufferFactory().get());
                serverSession->popOutgoingCipherText(&data);

                test::logHexDump("Server sending ciphertext", data);

                {
                    NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_CLIENT();

                    error = clientSession->pushIncomingCipherText(data);
                    NTCCFG_TEST_FALSE(error);
                }
            }
        }

        {
            NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_CLIENT();

            if (clientSession->hasIncomingPlainText()) {
                bdlbb::Blob data(dataPool->incomingBlobBufferFactory().get());
                error = clientSession->popIncomingPlainText(&data);
                NTCCFG_TEST_FALSE(error);

                test::logHexDump("Client received plaintext", data);

                bdlbb::BlobUtil::append(clientPlaintextRead, data);
            }
        }

        {
            NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_SERVER();

            if (serverSession->hasIncomingPlainText()) {
                bdlbb::Blob data(dataPool->incomingBlobBufferFactory().get());
                error = serverSession->popIncomingPlainText(&data);
                NTCCFG_TEST_FALSE(error);

                test::logHexDump("Server received plaintext", data);

                bdlbb::BlobUtil::append(serverPlaintextRead, data);
            }
        }
    }
}

void cycleShutdown(const bsl::shared_ptr<ntcd::Encryption>& clientSession,
                   const bsl::shared_ptr<ntcd::Encryption>& serverSession,
                   const bsl::shared_ptr<ntci::DataPool>&   dataPool,
                   bdlbb::Blob*            clientPlaintextRead,
                   bdlbb::Blob*            serverPlaintextRead,
                   const test::Parameters& parameters)
{
    // Repeatedly read and write from the specified 'clientSession' and
    // 'serverSession' until both the specified 'clientCompleteFlag' and
    // 'serverCompleteFlag' is set.

    NTCI_LOG_CONTEXT();

    NTCCFG_WARNING_UNUSED(parameters);

    ntsa::Error error;

    while (!clientSession->isShutdownFinished() ||
           !serverSession->isShutdownFinished())
    {
        {
            NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_CLIENT();

            if (clientSession->hasOutgoingCipherText()) {
                bdlbb::Blob data(dataPool->outgoingBlobBufferFactory().get());
                clientSession->popOutgoingCipherText(&data);

                test::logHexDump("Client sending ciphertext", data);

                {
                    NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_SERVER();

                    error = serverSession->pushIncomingCipherText(data);
                    NTCCFG_TEST_FALSE(error);
                }
            }
        }

        {
            NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_SERVER();

            if (serverSession->hasOutgoingCipherText()) {
                bdlbb::Blob data(dataPool->outgoingBlobBufferFactory().get());
                serverSession->popOutgoingCipherText(&data);

                test::logHexDump("Server sending ciphertext", data);

                {
                    NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_CLIENT();

                    error = clientSession->pushIncomingCipherText(data);
                    NTCCFG_TEST_FALSE(error);
                }
            }
        }

        {
            NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_CLIENT();

            if (clientSession->hasIncomingPlainText()) {
                bdlbb::Blob data(dataPool->incomingBlobBufferFactory().get());
                error = clientSession->popIncomingPlainText(&data);
                NTCCFG_TEST_FALSE(error);

                test::logHexDump("Client received plaintext", data);

                bdlbb::BlobUtil::append(clientPlaintextRead, data);
            }
        }

        {
            NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_SERVER();

            if (serverSession->hasIncomingPlainText()) {
                bdlbb::Blob data(dataPool->incomingBlobBufferFactory().get());
                error = serverSession->popIncomingPlainText(&data);
                NTCCFG_TEST_FALSE(error);

                test::logHexDump("Server received plaintext", data);

                bdlbb::BlobUtil::append(serverPlaintextRead, data);
            }
        }
    }
}

void execute(const test::Parameters& parameters,
             bslma::Allocator*       basicAllocator = 0)
{
    // Execute the test described by the specified 'parameters' using the
    // specified 'authorityCertificate' and 'authorityPrivateKey' for the
    // trusted certificate authority, the specified 'clientCertificate' and
    // 'clientPrivateKey' for the client, and the specified 'serverCertificate'
    // and 'serverPrivateKey'. Optionally specify a 'basicAllocator' used to
    // supply memory. If 'basicAllocator' is 0, the currently installed default
    // allocator is used.

    NTCI_LOG_CONTEXT();

    test::logParameters("Testing parameters", parameters);

    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    // Create the data pool used by each session.

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(allocator,
                           parameters.d_bufferSize,
                           parameters.d_bufferSize,
                           allocator);

    bdlt::DatetimeTz startTime(bdlt::Datetime(2000, 1, 1), 0);
    bdlt::DatetimeTz expirationTime(bdlt::Datetime(2100, 1, 1), 0);

    typedef ntsa::DistinguishedName DN;

    // Create the authority certificate and key.

    ntsa::DistinguishedName authorityIdentity;
    authorityIdentity[DN::e_COMMON_NAME].addAttribute("authority");

    ntca::EncryptionKeyOptions authorityKeyOptions;

    bsl::shared_ptr<ntcd::EncryptionKey> authorityKey =
        ntcd::EncryptionKey::generate(authorityKeyOptions, allocator);

    ntca::EncryptionCertificateOptions authorityCertificateOptions;
    authorityCertificateOptions.setAuthority(true);
    authorityCertificateOptions.setSerialNumber(1);
    authorityCertificateOptions.setStartTime(startTime);
    authorityCertificateOptions.setExpirationTime(expirationTime);

    bsl::shared_ptr<ntcd::EncryptionCertificate> authorityCertificate =
        ntcd::EncryptionCertificate::generate(authorityIdentity,
                                              authorityKey,
                                              authorityCertificateOptions,
                                              allocator);

    // Create the client certificate and key.

    ntsa::DistinguishedName clientIdentity;
    clientIdentity[DN::e_COMMON_NAME].addAttribute("client");

    ntca::EncryptionKeyOptions clientKeyOptions;

    bsl::shared_ptr<ntcd::EncryptionKey> clientKey =
        ntcd::EncryptionKey::generate(clientKeyOptions, allocator);

    ntca::EncryptionCertificateOptions clientCertificateOptions;
    clientCertificateOptions.setAuthority(false);
    clientCertificateOptions.setSerialNumber(2);
    clientCertificateOptions.setStartTime(startTime);
    clientCertificateOptions.setExpirationTime(expirationTime);

    bsl::shared_ptr<ntcd::EncryptionCertificate> clientCertificate =
        ntcd::EncryptionCertificate::generate(clientIdentity,
                                              clientKey,
                                              authorityCertificate,
                                              authorityKey,
                                              clientCertificateOptions,
                                              allocator);

    // Create the server certificate and key.

    ntsa::DistinguishedName serverIdentity;
    serverIdentity[DN::e_COMMON_NAME].addAttribute("server");

    ntca::EncryptionKeyOptions serverKeyOptions;

    bsl::shared_ptr<ntcd::EncryptionKey> serverKey =
        ntcd::EncryptionKey::generate(serverKeyOptions, allocator);

    ntca::EncryptionCertificateOptions serverCertificateOptions;
    serverCertificateOptions.setAuthority(false);
    serverCertificateOptions.setSerialNumber(2);
    serverCertificateOptions.setStartTime(startTime);
    serverCertificateOptions.setExpirationTime(expirationTime);

    bsl::shared_ptr<ntcd::EncryptionCertificate> serverCertificate =
        ntcd::EncryptionCertificate::generate(serverIdentity,
                                              serverKey,
                                              authorityCertificate,
                                              authorityKey,
                                              serverCertificateOptions,
                                              allocator);

    // Create the client session.

    bsl::shared_ptr<ntcd::Encryption> clientSession;
    clientSession.createInplace(allocator,
                                ntca::EncryptionRole::e_CLIENT,
                                clientCertificate,
                                clientKey,
                                dataPool,
                                allocator);

    if (!parameters.d_clientRejectsServer) {
        clientSession->authorizePeer("server");
    }

    // Create the server session.

    bsl::shared_ptr<ntcd::Encryption> serverSession;
    serverSession.createInplace(allocator,
                                ntca::EncryptionRole::e_SERVER,
                                serverCertificate,
                                serverKey,
                                dataPool,
                                allocator);

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
        allocator);
    bdlbb::BlobUtil::append(&expectedClientPlaintextRead, helloClient);
    bdlbb::BlobUtil::append(&expectedClientPlaintextRead, goodbyeClient);

    bdlbb::Blob expectedServerPlaintextRead(
        dataPool->incomingBlobBufferFactory().get(),
        allocator);
    bdlbb::BlobUtil::append(&expectedServerPlaintextRead, helloServer);
    bdlbb::BlobUtil::append(&expectedServerPlaintextRead, goodbyeServer);

    for (bsl::size_t usageIteration = 0;
         usageIteration < parameters.d_numReuses + 1;
         ++usageIteration)
    {
        NTCI_LOG_INFO("Iteration %d/%d starting",
                      usageIteration + 1,
                      parameters.d_numReuses + 1);

        bool clientHandshakeComplete = false;
        bool serverHandshakeComplete = false;

        bdlbb::Blob clientPlaintextRead(
            dataPool->incomingBlobBufferFactory().get(),
            allocator);
        bdlbb::Blob serverPlaintextRead(
            dataPool->incomingBlobBufferFactory().get(),
            allocator);

        // Initiate the handshake from the client.

        {
            NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_CLIENT();

            NTCI_LOG_INFO("Client handshake initiating");

            error = clientSession->initiateHandshake(
                NTCCFG_BIND(&test::processClientHandshakeComplete,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            clientSession,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            &clientHandshakeComplete,
                            parameters));
            NTCCFG_TEST_FALSE(error);
        }

        // Initiate the handshake from the server.

        {
            NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_SERVER();

            NTCI_LOG_INFO("Server handshake initiating");

            error = serverSession->initiateHandshake(
                NTCCFG_BIND(&test::processServerHandshakeComplete,
                            NTCCFG_BIND_PLACEHOLDER_1,
                            serverSession,
                            NTCCFG_BIND_PLACEHOLDER_2,
                            NTCCFG_BIND_PLACEHOLDER_3,
                            &serverHandshakeComplete,
                            parameters));
            NTCCFG_TEST_FALSE(error);
        }

        // Send data immediately after the handshake is initiated.

        {
            NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_CLIENT();

            error = clientSession->pushOutgoingPlainText(helloServer);
            NTCCFG_TEST_FALSE(error);
        }

        {
            NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_SERVER();

            error = serverSession->pushOutgoingPlainText(helloClient);
            NTCCFG_TEST_FALSE(error);
        }

        // Process the TLS state machine until the handshake is complete.

        test::cycleHandshake(clientSession,
                             serverSession,
                             dataPool,
                             &clientPlaintextRead,
                             &serverPlaintextRead,
                             parameters);

        if (!parameters.d_success) {
            return;
        }

        NTCCFG_TEST_TRUE(clientHandshakeComplete);
        NTCCFG_TEST_TRUE(serverHandshakeComplete);

        // Send data immediately before the shutdown is initiated.

        {
            NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_CLIENT();

            error = clientSession->pushOutgoingPlainText(goodbyeServer);
            NTCCFG_TEST_FALSE(error);
        }

        {
            NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_SERVER();

            error = serverSession->pushOutgoingPlainText(goodbyeClient);
            NTCCFG_TEST_FALSE(error);
        }

        // Initiate the shutdown from the client.

        {
            NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_CLIENT();

            NTCI_LOG_INFO("Client shutdown initiating");

            error = clientSession->shutdown();
            NTCCFG_TEST_FALSE(error);
        }

        // Initiate the shutdown from the server.

        {
            NTCD_ENCRYPTION_LOG_CONTEXT_GUARD_SERVER();

            NTCI_LOG_INFO("Server shutdown initiating");

            error = serverSession->shutdown();
            NTCCFG_TEST_FALSE(error);
        }

        // Process the TLS state machine until the shutdown is complete.

        test::cycleShutdown(clientSession,
                            serverSession,
                            dataPool,
                            &clientPlaintextRead,
                            &serverPlaintextRead,
                            parameters);

        // Ensure the plaintext received by the client matches the expected
        // data.

        NTCCFG_TEST_EQ(bdlbb::BlobUtil::compare(clientPlaintextRead,
                                                expectedClientPlaintextRead),
                       0);

        // Ensure the plaintext received by the server matches the expected
        // data.

        NTCCFG_TEST_EQ(bdlbb::BlobUtil::compare(serverPlaintextRead,
                                                expectedServerPlaintextRead),
                       0);

        NTCI_LOG_INFO("Iteration %d/%d complete",
                      usageIteration + 1,
                      parameters.d_numReuses + 1);
    }

    NTCI_LOG_INFO("Test complete");
}

}  // close namespace test

NTCCFG_TEST_CASE(1)
{
    // Concern: Encryption key.
    // Plan:

    ntccfg::TestAllocator ta;
    {
        NTCI_LOG_CONTEXT();

        ntsa::Error       error;
        bsl::vector<char> buffer;

        bsl::shared_ptr<ntcd::EncryptionKey> key1 =
            ntcd::EncryptionKey::generate(ntca::EncryptionKeyOptions(), &ta);

        NTCCFG_TEST_TRUE(key1);

        NTCI_LOG_STREAM_DEBUG << "Key1 = " << *key1 << NTCI_LOG_STREAM_END;

        error = key1->encode(&buffer);
        NTCCFG_TEST_OK(error);

        bsl::shared_ptr<ntcd::EncryptionKey> key2;
        error = ntcd::EncryptionKey::decode(&key2, buffer, &ta);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_TRUE(key2);

        NTCI_LOG_STREAM_DEBUG << "Key2 = " << *key2 << NTCI_LOG_STREAM_END;

        NTCCFG_TEST_EQ(key1->value(), key2->value());
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern: Encryption certificate.
    // Plan:

    ntccfg::TestAllocator ta;
    {
        NTCI_LOG_CONTEXT();

        ntsa::Error error;
        int         rc;

        bsl::vector<char> buffer;

        bsl::shared_ptr<ntcd::EncryptionKey> key1 =
            ntcd::EncryptionKey::generate(ntca::EncryptionKeyOptions(), &ta);

        NTCCFG_TEST_TRUE(key1);

        ntsa::DistinguishedName identity1;
        identity1[ntsa::DistinguishedName::e_COMMON_NAME].addAttribute(
            "John Doe");
        identity1[ntsa::DistinguishedName::e_ORGANIZATION_NAME].addAttribute(
            "Bloomberg");

        bsl::shared_ptr<ntcd::EncryptionCertificate> certificate1 =
            ntcd::EncryptionCertificate::generate(
                identity1,
                key1,
                ntca::EncryptionCertificateOptions(),
                &ta);

        NTCCFG_TEST_TRUE(certificate1);

        NTCI_LOG_STREAM_DEBUG << "Certificate1 = " << *certificate1
                              << NTCI_LOG_STREAM_END;

        error = certificate1->encode(&buffer);
        NTCCFG_TEST_OK(error);

        bsl::shared_ptr<ntcd::EncryptionCertificate> certificate2;
        error =
            ntcd::EncryptionCertificate::decode(&certificate2, buffer, &ta);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_TRUE(certificate2);

        NTCI_LOG_STREAM_DEBUG << "Certificate2 = " << *certificate2
                              << NTCI_LOG_STREAM_END;

        ntsa::DistinguishedName identity2 = certificate2->subject();

        bsl::string identityText1;
        rc = identity1.generate(&identityText1);
        NTCCFG_TEST_EQ(rc, 0);

        bsl::string identityText2;
        rc = identity2.generate(&identityText2);
        NTCCFG_TEST_EQ(rc, 0);

        NTCCFG_TEST_EQ(identityText1, identityText2);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Concern: Successful encryption.
    // Plan:

    ntccfg::TestAllocator ta;
    {
        test::Parameters parameters;
        parameters.d_bufferSize          = 32;
        parameters.d_clientRejectsServer = false;
        parameters.d_serverRejectsClient = false;
        parameters.d_success             = true;

        test::execute(parameters, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    // Concern: Client reject server.
    // Plan:

    ntccfg::TestAllocator ta;
    {
        test::Parameters parameters;
        parameters.d_bufferSize          = 32;
        parameters.d_clientRejectsServer = true;
        parameters.d_serverRejectsClient = false;
        parameters.d_success             = false;

        test::execute(parameters, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(5)
{
    // Concern: Server rejects client.
    // Plan:

    ntccfg::TestAllocator ta;
    {
        test::Parameters parameters;
        parameters.d_bufferSize          = 32;
        parameters.d_clientRejectsServer = false;
        parameters.d_serverRejectsClient = true;
        parameters.d_success             = false;

        test::execute(parameters, &ta);
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
}
NTCCFG_TEST_DRIVER_END;
