// Copyright 2020-2024 Bloomberg Finance L.P.
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
BSLS_IDENT_RCSID(ntctls_plugin_t_cpp, "$Id$ $CSID$")

#include <ntctls_plugin.h>

#include <ntca_encryptionauthentication.h>
#include <ntca_encryptioncertificate.h>
#include <ntca_encryptioncertificateoptions.h>
#include <ntca_encryptionclientoptions.h>
#include <ntca_encryptionkey.h>
#include <ntca_encryptionkeyoptions.h>
#include <ntca_encryptionkeytype.h>
#include <ntca_encryptionmethod.h>
#include <ntca_encryptionoptions.h>
#include <ntca_encryptionresource.h>
#include <ntca_encryptionresourcedescriptor.h>
#include <ntca_encryptionresourceoptions.h>
#include <ntca_encryptionresourcetype.h>
#include <ntca_encryptionrole.h>
#include <ntca_encryptionsecret.h>
#include <ntca_encryptionserveroptions.h>
#include <ntca_encryptionvalidation.h>
#include <ntci_encryption.h>
#include <ntci_encryptioncertificate.h>
#include <ntci_encryptioncertificategenerator.h>
#include <ntci_encryptioncertificatestorage.h>
#include <ntci_encryptionclient.h>
#include <ntci_encryptionclientfactory.h>
#include <ntci_encryptiondriver.h>
#include <ntci_encryptionkey.h>
#include <ntci_encryptionkeygenerator.h>
#include <ntci_encryptionkeystorage.h>
#include <ntci_encryptionserver.h>
#include <ntci_encryptionserverfactory.h>
#include <ntci_log.h>
#include <ntci_mutex.h>
#include <ntcs_datapool.h>
#include <ntsf_system.h>
#include <bslim_printer.h>

using namespace BloombergLP;

#if NTC_BUILD_WITH_OPENSSL

#define NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_MAIN()                           \
    NTCI_LOG_CONTEXT_GUARD_OWNER("main")

#define NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_CLIENT()                         \
    NTCI_LOG_CONTEXT_GUARD_OWNER("client")

#define NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_SERVER()                         \
    NTCI_LOG_CONTEXT_GUARD_OWNER("server")

namespace BloombergLP {
namespace ntctls {

// Provide utilties for key tests.
class KeyTestUtil
{
  public:
    // Load into the specified 'result' each supported key type.
    static void loadKeyTypes(
        bsl::vector<ntca::EncryptionKeyType::Value>* result);

    // Load into the specified 'result' each supported variation of key
    // storage options.
    static void loadKeyStorageOptions(
        bsl::vector<ntca::EncryptionResourceOptions>* result);

    // Log the hex dump of the specified 'osb' stream buffer.
    static void logHexDump(const bdlsb::MemOutStreamBuf& osb);

    // Log the hex dump of the specified 'ob' blob.
    static void logHexDump(const bdlbb::Blob& ob);

    // Generate a key.
    static void verifyKeyGeneration(
        bsl::shared_ptr<ntci::EncryptionKey>* result,
        const ntca::EncryptionKeyOptions&     keyConfig,
        bslma::Allocator*                     allocator);

    // Encode then decode the specified 'key' to verify it is encoded and
    // decoded losslessly.
    static void verifyKeyEncoding(
        const bsl::shared_ptr<ntci::EncryptionKey>& key,
        const ntca::EncryptionKeyOptions&           keyConfig,
        const ntca::EncryptionResourceOptions&      keyStorageOptions,
        bslma::Allocator*                           allocator);

    // Generate a key and test that the key can be encoded and decoded
    // losslessly.
    static void verifyKeyConfig(
        const ntca::EncryptionKeyOptions&      keyConfig,
        const ntca::EncryptionResourceOptions& keyStorageOptions,
        bslma::Allocator*                      allocator);
};

// Provide keys tests.
class KeyTest
{
  public:
    // Generate a key and verify it can be encoded and decoded losslessly.
    static void verifyUsage();
};

void KeyTestUtil::loadKeyTypes(
    bsl::vector<ntca::EncryptionKeyType::Value>* result)
{
    // result->push_back(ntca::EncryptionKeyType::e_DSA);
    result->push_back(ntca::EncryptionKeyType::e_RSA);
    result->push_back(ntca::EncryptionKeyType::e_NIST_P256);
    result->push_back(ntca::EncryptionKeyType::e_NIST_P384);
    result->push_back(ntca::EncryptionKeyType::e_NIST_P521);
    result->push_back(ntca::EncryptionKeyType::e_ED25519);
    result->push_back(ntca::EncryptionKeyType::e_ED448);
}

void KeyTestUtil::loadKeyStorageOptions(
    bsl::vector<ntca::EncryptionResourceOptions>* result)
{
    {
        ntca::EncryptionResourceOptions options;
        options.setType(ntca::EncryptionResourceType::e_ASN1);
        result->push_back(options);
    }

    {
        ntca::EncryptionResourceOptions options;
        options.setType(ntca::EncryptionResourceType::e_ASN1_PEM);
        result->push_back(options);
    }

    {
        ntca::EncryptionResourceOptions options;
        options.setType(ntca::EncryptionResourceType::e_PKCS8);
        result->push_back(options);
    }

    {
        ntca::EncryptionResourceOptions options;
        options.setType(ntca::EncryptionResourceType::e_PKCS8_PEM);
        result->push_back(options);
    }

    {
        ntca::EncryptionResourceOptions options;
        options.setType(ntca::EncryptionResourceType::e_PKCS12);
        result->push_back(options);
    }
}

void KeyTestUtil::logHexDump(const bdlsb::MemOutStreamBuf& osb)
{
    bsl::stringstream ss;
    ss << bdlb::PrintStringHexDumper(osb.data(),
                                     static_cast<int>(osb.length()));

    BSLS_LOG_DEBUG("Key:\n%s", ss.str().c_str());
}

void KeyTestUtil::logHexDump(const bdlbb::Blob& ob)
{
    bsl::stringstream ss;
    ss << bdlbb::BlobUtilHexDumper(&ob);

    BSLS_LOG_DEBUG("Key:\n%s", ss.str().c_str());
}

void KeyTestUtil::verifyKeyGeneration(
    bsl::shared_ptr<ntci::EncryptionKey>* result,
    const ntca::EncryptionKeyOptions&     keyConfig,
    bslma::Allocator*                     allocator)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> driver;
    ntctls::Plugin::load(&driver);

    error = driver->generateKey(result, keyConfig, allocator);
    NTSCFG_TEST_OK(error);
}

void KeyTestUtil::verifyKeyEncoding(
    const bsl::shared_ptr<ntci::EncryptionKey>& key,
    const ntca::EncryptionKeyOptions&           keyConfig,
    const ntca::EncryptionResourceOptions&      keyStorageOptions,
    bslma::Allocator*                           allocator)
{
    ntsa::Error error;
    int         rc;

    bsl::shared_ptr<ntci::EncryptionDriver> driver;
    ntctls::Plugin::load(&driver);

    // Write the key to an output stream buffer.

    bdlsb::MemOutStreamBuf osb(allocator);

    error = key->encode(&osb, keyStorageOptions);
    NTSCFG_TEST_OK(error);

    rc = osb.pubsync();
    NTSCFG_TEST_EQ(rc, 0);

    ntctls::KeyTestUtil::logHexDump(osb);

    NTSCFG_TEST_NE(osb.data(), 0);
    NTSCFG_TEST_GT(osb.length(), 0);

    // Read the key from an input stream buffer.

    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());

    bsl::shared_ptr<ntci::EncryptionKey> key1;
    error = driver->decodeKey(&key1, &isb, keyStorageOptions, allocator);
    NTSCFG_TEST_OK(error);

    // Compare the two keys.

    bool equal = key->equals(*key1);
    NTSCFG_TEST_TRUE(equal);

    // Write the key just read back to another output stream buffer.

    bdlsb::MemOutStreamBuf osb1(allocator);

    error = key1->encode(&osb1, keyStorageOptions);
    NTSCFG_TEST_OK(error);

    rc = osb1.pubsync();
    NTSCFG_TEST_EQ(rc, 0);

    ntctls::KeyTestUtil::logHexDump(osb1);

    // Compare the contents of the two streams. Note that recreating
    // a PKCS12 container can result in a different encoding, even if
    // the keys are the same.

    if (keyStorageOptions.type().isNull() ||
        keyStorageOptions.type().value() !=
            ntca::EncryptionResourceType::e_PKCS12)
    {
        NTSCFG_TEST_EQ(osb.length(), osb1.length());

        rc = bsl::memcmp(osb.data(), osb1.data(), osb1.length());
        NTSCFG_TEST_EQ(rc, 0);
    }
}

void KeyTestUtil::verifyKeyConfig(
    const ntca::EncryptionKeyOptions&      keyConfig,
    const ntca::EncryptionResourceOptions& keyStorageOptions,
    bslma::Allocator*                      allocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_INFO << "Testing " << keyConfig << " storage "
                         << keyStorageOptions << NTCI_LOG_STREAM_END;

    // Generate a key.

    bsl::shared_ptr<ntci::EncryptionKey> key;
    ntctls::KeyTestUtil::verifyKeyGeneration(&key, keyConfig, allocator);

    // Test that the key can be encoded and decoded losslessly.

    ntctls::KeyTestUtil::verifyKeyEncoding(key,
                                           keyConfig,
                                           keyStorageOptions,
                                           allocator);
}

NTSCFG_TEST_FUNCTION(ntctls::KeyTest::verifyUsage)
{
    NTCI_LOG_CONTEXT();

    NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_MAIN();

    bslma::Allocator* allocator = NTSCFG_TEST_ALLOCATOR;

    bsl::vector<ntca::EncryptionKeyType::Value> keyTypeVector(allocator);
    ntctls::KeyTestUtil::loadKeyTypes(&keyTypeVector);

    bsl::vector<ntca::EncryptionResourceOptions> keyStorageOptionsVector;
    ntctls::KeyTestUtil::loadKeyStorageOptions(&keyStorageOptionsVector);

    for (bsl::size_t i = 0; i < keyTypeVector.size(); ++i) {
        for (bsl::size_t j = 0; j < keyStorageOptionsVector.size(); ++j) {
            ntca::EncryptionKeyOptions keyConfig;
            keyConfig.setType(keyTypeVector[i]);

            ntctls::KeyTestUtil::verifyKeyConfig(keyConfig,
                                                 keyStorageOptionsVector[j],
                                                 allocator);
        }
    }
}

// Provide utilties for certificate tests.
class CertificateTestUtil
{
  public:
    // Load into the specified 'result' each supported key type.
    static void loadKeyTypes(
        bsl::vector<ntca::EncryptionKeyType::Value>* result);

    // Load into the specified 'result' each supported variation of key
    // storage options.
    static void loadKeyStorageOptions(
        bsl::vector<ntca::EncryptionResourceOptions>* result);

    // Load into the specified 'result' each supported variation of certificate
    // storage options.
    static void loadCertificateStorageOptions(
        bsl::vector<ntca::EncryptionResourceOptions>* result);

    // Log the hex dump of the specified 'osb' stream buffer.
    static void logHexDump(const char*                   type,
                           const bdlsb::MemOutStreamBuf& osb);

    // Log the hex dump of the specified 'ob' blob.
    static void logHexDump(const char* type, const bdlbb::Blob& ob);

    // Log the specified 'certificate'.
    static void logCertificate(
        const char*                                         label,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate);

    // Generate a key.
    static void verifyKeyGeneration(
        bsl::shared_ptr<ntci::EncryptionKey>* result,
        const ntca::EncryptionKeyOptions&     keyConfig,
        bslma::Allocator*                     allocator);

    // Generate a self-signed certificate.
    static void verifyCertificateGeneration(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const ntsa::DistinguishedName&                userIdentity,
        const bsl::shared_ptr<ntci::EncryptionKey>&   userPrivateKey,
        const ntca::EncryptionCertificateOptions&     certificateConfig,
        bslma::Allocator*                             allocator);

    // Generate a certificate signed by another certificate.
    static void verifyCertificateGeneration(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const ntsa::DistinguishedName&                userIdentity,
        const bsl::shared_ptr<ntci::EncryptionKey>&   userPrivateKey,
        const bsl::shared_ptr<ntci::EncryptionCertificate>&
                                                    authorityCertificate,
        const bsl::shared_ptr<ntci::EncryptionKey>& authorityPrivateKey,
        const ntca::EncryptionCertificateOptions&   certificateConfig,
        bslma::Allocator*                           allocator);

    // Encode then decode the specified 'key' to verify it is encoded and
    // decoded losslessly.
    static void verifyKeyEncoding(
        const bsl::shared_ptr<ntci::EncryptionKey>& key,
        const ntca::EncryptionKeyOptions&           keyConfig,
        const ntca::EncryptionResourceOptions&      keyStorageOptions,
        bslma::Allocator*                           allocator);

    // Encode then decode the specified 'certificate' to verify it is encoded
    // and decoded losslessly.
    static void verifyCertificateEncoding(
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
        const ntca::EncryptionCertificateOptions&           certificateConfig,
        const ntca::EncryptionResourceOptions& certificateStorageOptions,
        bslma::Allocator*                      allocator);
};

void CertificateTestUtil::loadKeyTypes(
    bsl::vector<ntca::EncryptionKeyType::Value>* result)
{
    // result->push_back(ntca::EncryptionKeyType::e_DSA);
    // result->push_back(ntca::EncryptionKeyType::e_RSA);
    result->push_back(ntca::EncryptionKeyType::e_NIST_P256);
    // result->push_back(ntca::EncryptionKeyType::e_NIST_P384);
    // result->push_back(ntca::EncryptionKeyType::e_NIST_P521);
    result->push_back(ntca::EncryptionKeyType::e_ED25519);
    // result->push_back(ntca::EncryptionKeyType::e_ED448);
}

void CertificateTestUtil::loadKeyStorageOptions(
    bsl::vector<ntca::EncryptionResourceOptions>* result)
{
    {
        ntca::EncryptionResourceOptions options;
        options.setType(ntca::EncryptionResourceType::e_ASN1);
        result->push_back(options);
    }

    {
        ntca::EncryptionResourceOptions options;
        options.setType(ntca::EncryptionResourceType::e_ASN1_PEM);
        result->push_back(options);
    }

    {
        ntca::EncryptionResourceOptions options;
        options.setType(ntca::EncryptionResourceType::e_PKCS8);
        result->push_back(options);
    }

    {
        ntca::EncryptionResourceOptions options;
        options.setType(ntca::EncryptionResourceType::e_PKCS8_PEM);
        result->push_back(options);
    }

    {
        ntca::EncryptionResourceOptions options;
        options.setType(ntca::EncryptionResourceType::e_PKCS12);
        result->push_back(options);
    }
}

void CertificateTestUtil::loadCertificateStorageOptions(
    bsl::vector<ntca::EncryptionResourceOptions>* result)
{
    {
        ntca::EncryptionResourceOptions options;
        options.setType(ntca::EncryptionResourceType::e_ASN1);
        result->push_back(options);
    }

    {
        ntca::EncryptionResourceOptions options;
        options.setType(ntca::EncryptionResourceType::e_ASN1_PEM);
        result->push_back(options);
    }

    {
        ntca::EncryptionResourceOptions options;
        options.setType(ntca::EncryptionResourceType::e_PKCS12);
        result->push_back(options);
    }
}

void CertificateTestUtil::logHexDump(const char*                   type,
                                     const bdlsb::MemOutStreamBuf& osb)
{
    bsl::stringstream ss;
    ss << bdlb::PrintStringHexDumper(osb.data(),
                                     static_cast<int>(osb.length()));

    BSLS_LOG_TRACE("%s:\n%s", type, ss.str().c_str());
}

void CertificateTestUtil::logHexDump(const char* type, const bdlbb::Blob& ob)
{
    bsl::stringstream ss;
    ss << bdlbb::BlobUtilHexDumper(&ob);

    BSLS_LOG_TRACE("%s:\n%s", type, ss.str().c_str());
}

void CertificateTestUtil::logCertificate(
    const char*                                         label,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate)
{
    bsl::stringstream ss;
    certificate->print(ss);

    BSLS_LOG_DEBUG("%s = \n%s", label, ss.str().c_str());
}

void CertificateTestUtil::verifyKeyGeneration(
    bsl::shared_ptr<ntci::EncryptionKey>* result,
    const ntca::EncryptionKeyOptions&     keyConfig,
    bslma::Allocator*                     allocator)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> driver;
    ntctls::Plugin::load(&driver);

    error = driver->generateKey(result, keyConfig, allocator);
    NTSCFG_TEST_OK(error);
}

void CertificateTestUtil::verifyCertificateGeneration(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const ntsa::DistinguishedName&                userIdentity,
    const bsl::shared_ptr<ntci::EncryptionKey>&   userPrivateKey,
    const ntca::EncryptionCertificateOptions&     certificateConfig,
    bslma::Allocator*                             allocator)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> driver;
    ntctls::Plugin::load(&driver);

    error = driver->generateCertificate(result,
                                        userIdentity,
                                        userPrivateKey,
                                        certificateConfig,
                                        allocator);
    NTSCFG_TEST_OK(error);
}

void CertificateTestUtil::verifyCertificateGeneration(
    bsl::shared_ptr<ntci::EncryptionCertificate>*       result,
    const ntsa::DistinguishedName&                      userIdentity,
    const bsl::shared_ptr<ntci::EncryptionKey>&         userPrivateKey,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& authorityCertificate,
    const bsl::shared_ptr<ntci::EncryptionKey>&         authorityPrivateKey,
    const ntca::EncryptionCertificateOptions&           certificateConfig,
    bslma::Allocator*                                   allocator)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> driver;
    ntctls::Plugin::load(&driver);

    error = driver->generateCertificate(result,
                                        userIdentity,
                                        userPrivateKey,
                                        authorityCertificate,
                                        authorityPrivateKey,
                                        certificateConfig,
                                        allocator);
    NTSCFG_TEST_OK(error);
}

void CertificateTestUtil::verifyKeyEncoding(
    const bsl::shared_ptr<ntci::EncryptionKey>& key,
    const ntca::EncryptionKeyOptions&           keyConfig,
    const ntca::EncryptionResourceOptions&      keyStorageOptions,
    bslma::Allocator*                           allocator)
{
    ntsa::Error error;
    int         rc;

    bsl::shared_ptr<ntci::EncryptionDriver> driver;
    ntctls::Plugin::load(&driver);

    // Write the key to an output stream buffer.

    bdlsb::MemOutStreamBuf osb(allocator);

    error = key->encode(&osb, keyStorageOptions);
    NTSCFG_TEST_OK(error);

    rc = osb.pubsync();
    NTSCFG_TEST_EQ(rc, 0);

    CertificateTestUtil::logHexDump("Key", osb);

    NTSCFG_TEST_NE(osb.data(), 0);
    NTSCFG_TEST_GT(osb.length(), 0);

    // Read the key from an input stream buffer.

    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());

    bsl::shared_ptr<ntci::EncryptionKey> key1;
    error = driver->decodeKey(&key1, &isb, keyStorageOptions, allocator);
    NTSCFG_TEST_OK(error);

    // Compare the two keys.

    bool equal = key->equals(*key1);
    NTSCFG_TEST_TRUE(equal);

    // Write the key just read back to another output stream buffer.

    bdlsb::MemOutStreamBuf osb1(allocator);

    error = key1->encode(&osb1, keyStorageOptions);
    NTSCFG_TEST_OK(error);

    rc = osb1.pubsync();
    NTSCFG_TEST_EQ(rc, 0);

    CertificateTestUtil::logHexDump("Key", osb1);

    // Compare the contents of the two streams. Note that recreating
    // a PKCS12 container can result in a different encoding, even if
    // the keys are the same.

    if (keyStorageOptions.type().isNull() ||
        keyStorageOptions.type().value() !=
            ntca::EncryptionResourceType::e_PKCS12)
    {
        NTSCFG_TEST_EQ(osb.length(), osb1.length());

        rc = bsl::memcmp(osb.data(), osb1.data(), osb1.length());
        NTSCFG_TEST_EQ(rc, 0);
    }
}

void CertificateTestUtil::verifyCertificateEncoding(
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const ntca::EncryptionCertificateOptions&           certificateConfig,
    const ntca::EncryptionResourceOptions& certificateStorageOptions,
    bslma::Allocator*                      allocator)
{
    ntsa::Error error;
    int         rc;

    bsl::shared_ptr<ntci::EncryptionDriver> driver;
    ntctls::Plugin::load(&driver);

    // Write the certificate to an output stream buffer.

    bdlsb::MemOutStreamBuf osb(allocator);

    error = certificate->encode(&osb, certificateStorageOptions);
    NTSCFG_TEST_OK(error);

    rc = osb.pubsync();
    NTSCFG_TEST_EQ(rc, 0);

    CertificateTestUtil::logHexDump("Certificate", osb);

    NTSCFG_TEST_NE(osb.data(), 0);
    NTSCFG_TEST_GT(osb.length(), 0);

    // Read the certificate from an input stream buffer.

    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());

    bsl::shared_ptr<ntci::EncryptionCertificate> certificate1;
    error = driver->decodeCertificate(&certificate1,
                                      &isb,
                                      certificateStorageOptions,
                                      allocator);
    NTSCFG_TEST_OK(error);

    // Compare the two certificates.

    bool equal = certificate->equals(*certificate1);
    NTSCFG_TEST_TRUE(equal);

    // Write the certificate just read back to another output stream
    // buffer.

    bdlsb::MemOutStreamBuf osb1(allocator);

    error = certificate1->encode(&osb1, certificateStorageOptions);
    NTSCFG_TEST_OK(error);

    rc = osb1.pubsync();
    NTSCFG_TEST_EQ(rc, 0);

    CertificateTestUtil::logHexDump("Certificate", osb1);

    // Compare the contents of the two streams .Note that recreating
    // a PKCS12 container can result in a different encoding, even if
    // the keys are the same.

    if (certificateStorageOptions.type().isNull() ||
        certificateStorageOptions.type().value() !=
            ntca::EncryptionResourceType::e_PKCS12)
    {
        NTSCFG_TEST_EQ(osb.length(), osb1.length());

        rc = bsl::memcmp(osb.data(), osb1.data(), osb1.length());
        NTSCFG_TEST_EQ(rc, 0);
    }
}

// Provide certificate tests.
class CertificateTest
{
  private:
    // Generate certificate authorities.
    static void verifyAuthorityOptions(
        ntca::EncryptionKeyType::Value         authorityKeyType,
        const ntca::EncryptionResourceOptions& keyStorageOptions,
        const ntca::EncryptionResourceOptions& certificateStorageOptions,
        bslma::Allocator*                      allocator);

    // Generate self-signed certificates.
    static void verifyUserSignedBySelfOptions(
        ntca::EncryptionKeyType::Value         userKeyType,
        const ntca::EncryptionResourceOptions& keyStorageOptions,
        const ntca::EncryptionResourceOptions& certificateStorageOptions,
        bslma::Allocator*                      allocator);

    // Generate a certificate signed by a certificate authority.
    static void verifyUserSignedByAuthorityOptions(
        ntca::EncryptionKeyType::Value         authorityKeyType,
        ntca::EncryptionKeyType::Value         userKeyType,
        const ntca::EncryptionResourceOptions& keyStorageOptions,
        const ntca::EncryptionResourceOptions& certificateStorageOptions,
        bslma::Allocator*                      allocator);

  public:
    // Generate certificate authorities.
    static void verifyAuthority();

    // Generate self-signed certificates.
    static void verifyUserSignedBySelf();

    // Generate a certificate signed by a certificate authority.
    static void verifyUserSignedByAuthority();
};

void CertificateTest::verifyAuthorityOptions(
    ntca::EncryptionKeyType::Value         authorityKeyType,
    const ntca::EncryptionResourceOptions& keyStorageOptions,
    const ntca::EncryptionResourceOptions& certificateStorageOptions,
    bslma::Allocator*                      allocator)
{
    // TESTING 'generate' (CA)
    //
    // Concerns: The mechanism under test can successfully generate certificate
    // authorities.
    //
    // Plan: Initialize the OpenSSL framework. Create a generator mechanism.
    // Configure the identity of the certificate authority.  Ensure the
    // generator can successfully generate a key used by the certificate
    // authority.  Ensure the generator can successfully generate a certificate
    // authority using the configured identity and generated key.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_INFO << "Testing authority key type " << authorityKeyType
                         << " key storage " << keyStorageOptions
                         << " certificate storage "
                         << certificateStorageOptions << NTCI_LOG_STREAM_END;

    // Define the authority subject.

    ntsa::DistinguishedName authorityIdentity;
    authorityIdentity["CN"] = "TEST.AUTHORITY";
    authorityIdentity["O"]  = "Bloomberg LP";

    // Define the authority private key generation configuration.

    ntca::EncryptionKeyOptions authorityKeyConfig;
    authorityKeyConfig.setType(authorityKeyType);

    // Define the authority certificate generation configuration.

    ntca::EncryptionCertificateOptions authorityCertificateConfig;
    authorityCertificateConfig.setAuthority(true);

    // Generate the authority private key.

    bsl::shared_ptr<ntci::EncryptionKey> authorityPrivateKey;
    CertificateTestUtil::verifyKeyGeneration(&authorityPrivateKey,
                                             authorityKeyConfig,
                                             allocator);

    // Test that the authority private key can be encoded and decoded
    // losslessly.

    CertificateTestUtil::verifyKeyEncoding(authorityPrivateKey,
                                           authorityKeyConfig,
                                           keyStorageOptions,
                                           allocator);

    // Generate the authority certificated.

    bsl::shared_ptr<ntci::EncryptionCertificate> authorityCertificate;
    CertificateTestUtil::verifyCertificateGeneration(
        &authorityCertificate,
        authorityIdentity,
        authorityPrivateKey,
        authorityCertificateConfig,
        allocator);

    CertificateTestUtil::logCertificate("Trusted certificate",
                                        authorityCertificate);

    // Test that the authority certificate can be encoded and decoded
    // losslessly.

    CertificateTestUtil::verifyCertificateEncoding(authorityCertificate,
                                                   authorityCertificateConfig,
                                                   certificateStorageOptions,
                                                   allocator);
}

void CertificateTest::verifyUserSignedBySelfOptions(
    ntca::EncryptionKeyType::Value         userKeyType,
    const ntca::EncryptionResourceOptions& keyStorageOptions,
    const ntca::EncryptionResourceOptions& certificateStorageOptions,
    bslma::Allocator*                      allocator)
{
    // TESTING 'generate' (self-signed)
    //
    // Concerns: The mechanism under test can successfully generate a
    // certificate signed by a certificate authority.
    //
    // Plan: Initialize the OpenSSL framework. Create a generator mechanism.
    // Configure the identity of a user certificate.  Ensure the generator can
    // successfully generate a key used by the user certificate.  Ensure the
    // generator can successfully generate a self-signed certificate using the
    // configured user identity and user key.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_INFO << "Testing user key type " << userKeyType
                         << " key storage " << keyStorageOptions
                         << " certificate storage "
                         << certificateStorageOptions << NTCI_LOG_STREAM_END;

    // Define the user subject.

    ntsa::DistinguishedName userIdentity;
    userIdentity["CN"] = "TEST.USER";
    userIdentity["O"]  = "Bloomberg LP";

    // Define the user private key generation configuration.

    ntca::EncryptionKeyOptions userKeyConfig;
    userKeyConfig.setType(userKeyType);

    // Define the user certificate generation configuration.

    ntca::EncryptionCertificateOptions userCertificateConfig;
    userCertificateConfig.setAuthority(false);

    // Generate the user private key.

    bsl::shared_ptr<ntci::EncryptionKey> userPrivateKey;
    CertificateTestUtil::verifyKeyGeneration(&userPrivateKey,
                                             userKeyConfig,
                                             allocator);

    // Test that the user private key can be encoded and decoded
    // losslessly.

    CertificateTestUtil::verifyKeyEncoding(userPrivateKey,
                                           userKeyConfig,
                                           keyStorageOptions,
                                           allocator);

    // Generate the certificate of the user, signed by itself.

    bsl::shared_ptr<ntci::EncryptionCertificate> userCertificate;
    CertificateTestUtil::verifyCertificateGeneration(&userCertificate,
                                                     userIdentity,
                                                     userPrivateKey,
                                                     userCertificateConfig,
                                                     allocator);

    CertificateTestUtil::logCertificate("Self-signed user certificate",
                                        userCertificate);

    // Test that the user certificate can be encoded and decoded
    // losslessly.

    CertificateTestUtil::verifyCertificateEncoding(userCertificate,
                                                   userCertificateConfig,
                                                   certificateStorageOptions,
                                                   allocator);
}

void CertificateTest::verifyUserSignedByAuthorityOptions(
    ntca::EncryptionKeyType::Value         authorityKeyType,
    ntca::EncryptionKeyType::Value         userKeyType,
    const ntca::EncryptionResourceOptions& keyStorageOptions,
    const ntca::EncryptionResourceOptions& certificateStorageOptions,
    bslma::Allocator*                      allocator)
{
    // TESTING 'generate' (CA-signed)
    //
    // Concerns: The mechanism under test can successfully generate a
    // certificate signed by a certificate authority.
    //
    // Plan: Initialize the OpenSSL framework. Create a generator mechanism.
    // Configure the identity of the certificate authority.  Ensure the
    // generator can successfully generate a key used by the certificate
    // authority.  Ensure the generator can successfully generate a certificate
    // authority using the configured identity and generated key.  Configure
    // the identity of a user certificate.  Ensure the generator can
    // successfully generate a key used by the user certificate.  Ensure the
    // generator can successfully generate a certificate using the configured
    // user identity and user key signed by the certificate authority.

    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_INFO << "Testing user key type " << userKeyType
                         << " authority key type " << authorityKeyType
                         << " key storage " << keyStorageOptions
                         << " certificate storage "
                         << certificateStorageOptions << NTCI_LOG_STREAM_END;

    // Define the authority subject.

    ntsa::DistinguishedName authorityIdentity;
    authorityIdentity["CN"] = "TEST.AUTHORITY";
    authorityIdentity["O"]  = "Bloomberg LP";

    // Define the authority private key generation configuration.

    ntca::EncryptionKeyOptions authorityKeyConfig;
    authorityKeyConfig.setType(authorityKeyType);

    // Define the authority certificate generation configuration.

    ntca::EncryptionCertificateOptions authorityCertificateConfig;
    authorityCertificateConfig.setAuthority(true);

    // Define the user subject.

    ntsa::DistinguishedName userIdentity;
    userIdentity["CN"] = "TEST.USER";
    userIdentity["O"]  = "Bloomberg LP";

    // Define the user private key generation configuration.

    ntca::EncryptionKeyOptions userKeyConfig;
    userKeyConfig.setType(userKeyType);

    // Define the user certificate generation configuration.

    ntca::EncryptionCertificateOptions userCertificateConfig;
    userCertificateConfig.setAuthority(false);

    // Generate the authority private key.

    bsl::shared_ptr<ntci::EncryptionKey> authorityPrivateKey;
    CertificateTestUtil::verifyKeyGeneration(&authorityPrivateKey,
                                             authorityKeyConfig,
                                             allocator);

    // Test that the authority private key can be encoded and decoded
    // losslessly.

    CertificateTestUtil::verifyKeyEncoding(authorityPrivateKey,
                                           authorityKeyConfig,
                                           keyStorageOptions,
                                           allocator);

    // Generate the authority certificated.

    bsl::shared_ptr<ntci::EncryptionCertificate> authorityCertificate;
    CertificateTestUtil::verifyCertificateGeneration(
        &authorityCertificate,
        authorityIdentity,
        authorityPrivateKey,
        authorityCertificateConfig,
        allocator);

    CertificateTestUtil::logCertificate("Trusted certificate",
                                        authorityCertificate);

    // Test that the authority certificate can be encoded and decoded
    // losslessly.

    CertificateTestUtil::verifyCertificateEncoding(authorityCertificate,
                                                   authorityCertificateConfig,
                                                   certificateStorageOptions,
                                                   allocator);

    // Generate the user private key.

    bsl::shared_ptr<ntci::EncryptionKey> userPrivateKey;
    CertificateTestUtil::verifyKeyGeneration(&userPrivateKey,
                                             userKeyConfig,
                                             allocator);

    // Test that the user private key can be encoded and decoded
    // losslessly.

    CertificateTestUtil::verifyKeyEncoding(userPrivateKey,
                                           userKeyConfig,
                                           keyStorageOptions,
                                           allocator);

    // Generate the certificate of the user, signed by the authority.

    bsl::shared_ptr<ntci::EncryptionCertificate> userCertificate;
    CertificateTestUtil::verifyCertificateGeneration(&userCertificate,
                                                     userIdentity,
                                                     userPrivateKey,
                                                     authorityCertificate,
                                                     authorityPrivateKey,
                                                     userCertificateConfig,
                                                     allocator);

    CertificateTestUtil::logCertificate("CA-signed user certificate",
                                        userCertificate);

    // Test that the user certificate can be encoded and decoded
    // losslessly.

    CertificateTestUtil::verifyCertificateEncoding(userCertificate,
                                                   userCertificateConfig,
                                                   certificateStorageOptions,
                                                   allocator);
}

NTSCFG_TEST_FUNCTION(ntctls::CertificateTest::verifyAuthority)
{
    NTCI_LOG_CONTEXT();

    NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_MAIN();

    bslma::Allocator* allocator = NTSCFG_TEST_ALLOCATOR;

    bsl::vector<ntca::EncryptionKeyType::Value> keyTypeVector(allocator);
    CertificateTestUtil::loadKeyTypes(&keyTypeVector);

    bsl::vector<ntca::EncryptionResourceOptions> keyStorageOptionsVector;
    CertificateTestUtil::loadKeyStorageOptions(&keyStorageOptionsVector);

    bsl::vector<ntca::EncryptionResourceOptions>
        certificateStorageOptionsVector;
    CertificateTestUtil::loadCertificateStorageOptions(
        &certificateStorageOptionsVector);

    for (bsl::size_t i = 0; i < keyTypeVector.size(); ++i) {
        for (bsl::size_t j = 0; j < keyStorageOptionsVector.size(); ++j) {
            for (bsl::size_t k = 0; k < certificateStorageOptionsVector.size();
                 ++k)
            {
                CertificateTest::verifyAuthorityOptions(
                    keyTypeVector[i],
                    keyStorageOptionsVector[j],
                    certificateStorageOptionsVector[k],
                    allocator);
            }
        }
    }
}

NTSCFG_TEST_FUNCTION(ntctls::CertificateTest::verifyUserSignedBySelf)
{
    NTCI_LOG_CONTEXT();

    NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_MAIN();

    bslma::Allocator* allocator = NTSCFG_TEST_ALLOCATOR;

    bsl::vector<ntca::EncryptionKeyType::Value> keyTypeVector(allocator);
    CertificateTestUtil::loadKeyTypes(&keyTypeVector);

    bsl::vector<ntca::EncryptionResourceOptions> keyStorageOptionsVector;
    CertificateTestUtil::loadKeyStorageOptions(&keyStorageOptionsVector);

    bsl::vector<ntca::EncryptionResourceOptions>
        certificateStorageOptionsVector;
    CertificateTestUtil::loadCertificateStorageOptions(
        &certificateStorageOptionsVector);

    for (bsl::size_t i = 0; i < keyTypeVector.size(); ++i) {
        for (bsl::size_t j = 0; j < keyStorageOptionsVector.size(); ++j) {
            for (bsl::size_t k = 0; k < certificateStorageOptionsVector.size();
                 ++k)
            {
                CertificateTest::verifyUserSignedBySelfOptions(
                    keyTypeVector[i],
                    keyStorageOptionsVector[j],
                    certificateStorageOptionsVector[k],
                    allocator);
            }
        }
    }
}

NTSCFG_TEST_FUNCTION(ntctls::CertificateTest::verifyUserSignedByAuthority)
{
    NTCI_LOG_CONTEXT();

    NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_MAIN();

    bslma::Allocator* allocator = NTSCFG_TEST_ALLOCATOR;

    bsl::vector<ntca::EncryptionKeyType::Value> keyTypeVector(allocator);
    CertificateTestUtil::loadKeyTypes(&keyTypeVector);

    bsl::vector<ntca::EncryptionResourceOptions> keyStorageOptionsVector;
    CertificateTestUtil::loadKeyStorageOptions(&keyStorageOptionsVector);

    bsl::vector<ntca::EncryptionResourceOptions>
        certificateStorageOptionsVector;
    CertificateTestUtil::loadCertificateStorageOptions(
        &certificateStorageOptionsVector);

    for (bsl::size_t i = 0; i < keyTypeVector.size(); ++i) {
        for (bsl::size_t j = 0; j < keyTypeVector.size(); ++j) {
            for (bsl::size_t k = 0; k < keyStorageOptionsVector.size(); ++k) {
                for (bsl::size_t l = 0;
                     l < certificateStorageOptionsVector.size();
                     ++l)
                {
                    CertificateTest::verifyUserSignedByAuthorityOptions(
                        keyTypeVector[i],
                        keyTypeVector[j],
                        keyStorageOptionsVector[k],
                        certificateStorageOptionsVector[l],
                        allocator);
                }
            }
        }
    }
}

// Uncomment to test a specific test case parameter variation.
// #define NTCTLS_RESOURCE_TEST_CASE_ID 21505

const ntca::EncryptionKeyType::Value k_DEFAULT_KEY_TYPE =
    ntca::EncryptionKeyType::e_NIST_P256;

const bsl::size_t k_DEFAULT_INCLUDE_PRIVATE_KEY = 1;
const bsl::size_t k_DEFAULT_INCLUDE_CERTIFICATE = 1;

const bsl::size_t k_DEFAULT_TRUST_CHAIN_COUNT = 1;
const bsl::size_t k_MIN_TRUST_CHAIN_COUNT     = 0;
const bsl::size_t k_MAX_TRUST_CHAIN_COUNT     = 1;

const bsl::size_t k_DEFAULT_TRUST_CHAIN_DEPTH = 1;
const bsl::size_t k_MIN_TRUST_CHAIN_DEPTH     = 0;
const bsl::size_t k_MAX_TRUST_CHAIN_DEPTH     = 1;

const char k_DEFAULT_SECRET[] = "abcdefghikjlkmopqrstuvwxyz";

// Describe test parameters.
class ResourceTestParameters
{
    bsl::size_t                     d_variationIndex;
    bsl::size_t                     d_variationCount;
    ntca::EncryptionKeyType::Value  d_keyType;
    ntca::EncryptionResourceOptions d_resourceEncoderOptions;
    ntca::EncryptionResourceOptions d_resourceDecoderOptions;
    bool                            d_includePrivateKey;
    bool                            d_includeCertificate;
    bsl::size_t                     d_trustChainCount;
    bsl::size_t                     d_trustChainDepth;

  public:
    // Create new test parameters having the default value. Optionally specify
    // a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    // currently installed default allocator is used.
    explicit ResourceTestParameters(bslma::Allocator* basicAllocator = 0);

    // Create new test parameters having the same value as the specified
    // 'original' object. Optionally specify a 'basicAllocator' used to supply
    // memory. If 'basicAllocator' is 0, the currently installed default
    // allocator is used.
    ResourceTestParameters(const ResourceTestParameters& original,
                           bslma::Allocator*             basicAllocator = 0);

    // Destroy this object.
    ~ResourceTestParameters();

    // Assign the value of the specified 'other' object to this object. Return
    // a reference to this object.
    ResourceTestParameters& operator=(const ResourceTestParameters& other);

    // Reset the value of this object to its value upon default construction.
    void reset();

    // Set the index of these parameters in the overall parameters set to the
    // specified 'value'.
    void setVariationIndex(bsl::size_t value);

    // Set the total number of variations of parameters in the set of
    // parameters being tested to the specified 'value'.
    void setVariationCount(bsl::size_t value);

    // Set the key type to the specified 'value'.
    void setKeyType(ntca::EncryptionKeyType::Value value);

    // Set the resource encoder options to the specified 'value'.
    void setResourceEncoderOptions(
        const ntca::EncryptionResourceOptions& value);

    // Set the resource decoder options to the specified 'value'.
    void setResourceDecoderOptions(
        const ntca::EncryptionResourceOptions& value);

    // Set the flag to include the user's private key in the resource to the to
    // the specified 'value'.
    void setIncludePrivateKey(bool value);

    // Set the flag to include the user's certificate in the resource to the to
    // the specified 'value'.
    void setIncludeCertificate(bool value);

    // Set the number of certificate authority chains to the specified 'value'.
    void setTrustChainCount(bsl::size_t value);

    // Set the depth of each certificate authority chain to the specified
    // 'value'.
    void setTrustChainDepth(bsl::size_t value);

    // Return the index of these parameters in the overall parameters set.
    bsl::size_t variationIndex() const;

    // Return the total number of variations of parameters in the set of
    // parameters being tested.
    bsl::size_t variationCount() const;

    // Return the key type.
    ntca::EncryptionKeyType::Value keyType() const;

    // Return the resource encoder options.
    const ntca::EncryptionResourceOptions& resourceEncoderOptions() const;

    // Return the resource decoder options.
    const ntca::EncryptionResourceOptions& resourceDecoderOptions() const;

    // Return the flag to include the user's certificate in the resource.
    bool includePrivateKey() const;

    // Return the flag to include the user's certificate in the resource.
    bool includeCertificate() const;

    // Return the number of certificate authority chains.
    bsl::size_t trustChainCount() const;

    // Return the depth of each certificate authority chain.
    bsl::size_t trustChainDepth() const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(ResourceTestParameters);
};

// Write a formatted, human-readable description of the specified 'object' to
// the specified 'stream'. Return a reference to the 'stream'.
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const ResourceTestParameters& object);

// Define a type alias for a vector of test parameters.
typedef bsl::vector<ResourceTestParameters> ResourceTestParametersVector;

// Provide utilities for loading test parameters.
class ResourceTestParametersUtil
{
  public:
    // Load into the specified 'result' each supported key type.
    static void loadKeyTypes(
        bsl::vector<ntca::EncryptionKeyType::Value>* result);

    // Load into the specified 'result' each supported resource type.
    static void loadResourceTypes(
        bsl::vector<ntca::EncryptionResourceType::Value>* result);

    // Load into the specified 'result' each supported variation of resource
    // storage options.
    static void loadResourceOptions(
        bsl::vector<ntca::EncryptionResourceOptions>* result);

    // Load into the specified 'result' the test parameters.
    static void loadParameters(
        bsl::vector<ntctls::ResourceTestParameters>* result);

    // Load into the specified 'result' the passphrase used when symmetrically
    // encrypting a resource. Return the error.
    static ntsa::Error loadSecret(ntca::EncryptionSecret* result);

    // Load into the specified 'result' a resource containing private keys,
    // certificates, and certificate authority chains according to the
    // specified 'parameters'. Optionally specify a 'basicAllocator' used to
    // supply memory. If 'basicAllocator' is 0, the currently installed default
    // allocator is used.
    static ntsa::Error loadResource(
        bsl::shared_ptr<ntci::EncryptionResource>* result,
        const ntctls::ResourceTestParameters&      parameters,
        bslma::Allocator*                          basicAllocator = 0);

    // Log the hex dump of the specified 'osb' stream buffer.
    static void logHexDump(ntca::EncryptionResourceType::Value type,
                           const bdlsb::MemOutStreamBuf&       osb);

    // Log the speciifed 'key' with the specified 'label'.
    static void logKey(const char*                                 label,
                       const bsl::shared_ptr<ntci::EncryptionKey>& key);

    // Log the speciifed 'key' with the specified 'label'.
    static void logCertificate(
        const char*                                         label,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate);

    // Verify the specified 'error' found when loading a resource according to
    // the specified 'parameters'.
    static void verifyLoadResult(
        ntsa::Error                           error,
        const ntctls::ResourceTestParameters& parameters);

    // Verify the specified 'error' found when encoding a resource according to
    // the specified 'parameters'.
    static void verifyEncodeResult(
        ntsa::Error                           error,
        const ntctls::ResourceTestParameters& parameters);

    // Verify the specified 'error' found when decoding a resource according to
    // the specified 'parameters'.
    static void verifyDecodeResult(
        ntsa::Error                           error,
        const ntctls::ResourceTestParameters& parameters);

    // Verify the specified 'foundResource' has the private keys and
    // certificates that are possible to decode from an encoding of the
    // specified 'expectedResource' according to the specified 'paramters'.
    static void verifyEquals(
        const bsl::shared_ptr<ntci::EncryptionResource>& foundResource,
        const bsl::shared_ptr<ntci::EncryptionResource>& expectedResource,
        const ntctls::ResourceTestParameters&            parameters);

    /// Return true if the specified 'lhsResource' contains the keys of the
    /// specified 'other' 'rhsResource' (if the specified 'includePrivateKeys'
    /// flag is true) and this resource contains the certificate and
    /// certificate authorities of the 'other' resource (if the specified
    /// 'includeCertificates' flag is true.
    static bool verifyContains(
        const bsl::shared_ptr<ntci::EncryptionResource>& lhsResource,
        const bsl::shared_ptr<ntci::EncryptionResource>& rhsResource,
        bool                                             includePrivateKeys,
        bool                                             includeCertificates);
};

// Provide implementations of test cases.
class ResourceTest
{
  private:
    // Generate a resource, encode it, decode it, and verify the decoded
    // resource matches the encoded resource. Perform the test according to the
    // specified 'parameters'. Allocate memory using the specified 'allocator'.
    static void verifyUsageParameters(
        const ntctls::ResourceTestParameters& parameters,
        bslma::Allocator*                     allocator);

  public:
    // Generate a resource, encode it, decode it, and verify the decoded
    // resource matches the encoded resource. Repeat the test for a varitey of
    // parameters.
    static void verifyUsage();
};

ResourceTestParameters::ResourceTestParameters(
    bslma::Allocator* basicAllocator)
: d_variationIndex(0)
, d_variationCount(0)
, d_keyType(k_DEFAULT_KEY_TYPE)
, d_resourceEncoderOptions(basicAllocator)
, d_resourceDecoderOptions(basicAllocator)
, d_includePrivateKey(k_DEFAULT_INCLUDE_PRIVATE_KEY)
, d_includeCertificate(k_DEFAULT_INCLUDE_CERTIFICATE)
, d_trustChainCount(k_DEFAULT_TRUST_CHAIN_COUNT)
, d_trustChainDepth(k_DEFAULT_TRUST_CHAIN_DEPTH)
{
}

ResourceTestParameters::ResourceTestParameters(
    const ResourceTestParameters& original,
    bslma::Allocator*             basicAllocator)
: d_variationIndex(original.d_variationIndex)
, d_variationCount(original.d_variationCount)
, d_keyType(original.d_keyType)
, d_resourceEncoderOptions(original.d_resourceEncoderOptions, basicAllocator)
, d_resourceDecoderOptions(original.d_resourceDecoderOptions, basicAllocator)
, d_includePrivateKey(original.d_includePrivateKey)
, d_includeCertificate(original.d_includeCertificate)
, d_trustChainCount(original.d_trustChainCount)
, d_trustChainDepth(original.d_trustChainDepth)
{
}

ResourceTestParameters::~ResourceTestParameters()
{
}

ResourceTestParameters& ResourceTestParameters::operator=(
    const ResourceTestParameters& other)
{
    if (this != &other) {
        d_variationIndex         = other.d_variationIndex;
        d_variationCount         = other.d_variationCount;
        d_keyType                = other.d_keyType;
        d_resourceEncoderOptions = other.d_resourceEncoderOptions;
        d_resourceDecoderOptions = other.d_resourceDecoderOptions;
        d_includePrivateKey      = other.d_includePrivateKey;
        d_includeCertificate     = other.d_includeCertificate;
        d_trustChainCount        = other.d_trustChainCount;
        d_trustChainDepth        = other.d_trustChainDepth;
    }

    return *this;
}

void ResourceTestParameters::reset()
{
    d_variationIndex = 0;
    d_variationCount = 0;
    d_keyType        = k_DEFAULT_KEY_TYPE;
    d_resourceEncoderOptions.reset();
    d_resourceDecoderOptions.reset();
    d_includePrivateKey  = k_DEFAULT_INCLUDE_PRIVATE_KEY;
    d_includeCertificate = k_DEFAULT_INCLUDE_CERTIFICATE;
    d_trustChainCount    = k_DEFAULT_TRUST_CHAIN_COUNT;
    d_trustChainDepth    = k_DEFAULT_TRUST_CHAIN_DEPTH;
}

void ResourceTestParameters::setVariationIndex(bsl::size_t value)
{
    d_variationIndex = value;
}

void ResourceTestParameters::setVariationCount(bsl::size_t value)
{
    d_variationCount = value;
}

void ResourceTestParameters::setKeyType(ntca::EncryptionKeyType::Value value)
{
    d_keyType = value;
}

void ResourceTestParameters::setResourceEncoderOptions(
    const ntca::EncryptionResourceOptions& value)
{
    d_resourceEncoderOptions = value;
}

void ResourceTestParameters::setResourceDecoderOptions(
    const ntca::EncryptionResourceOptions& value)
{
    d_resourceDecoderOptions = value;
}

void ResourceTestParameters::setIncludePrivateKey(bool value)
{
    d_includePrivateKey = value;
}

void ResourceTestParameters::setIncludeCertificate(bool value)
{
    d_includeCertificate = value;
}

void ResourceTestParameters::setTrustChainCount(bsl::size_t value)
{
    d_trustChainCount = value;
}

void ResourceTestParameters::setTrustChainDepth(bsl::size_t value)
{
    d_trustChainDepth = value;
}

bsl::size_t ResourceTestParameters::variationIndex() const
{
    return d_variationIndex;
}

bsl::size_t ResourceTestParameters::variationCount() const
{
    return d_variationCount;
}

ntca::EncryptionKeyType::Value ResourceTestParameters::keyType() const
{
    return d_keyType;
}

const ntca::EncryptionResourceOptions& ResourceTestParameters::
    resourceEncoderOptions() const
{
    return d_resourceEncoderOptions;
}

const ntca::EncryptionResourceOptions& ResourceTestParameters::
    resourceDecoderOptions() const
{
    return d_resourceDecoderOptions;
}

bool ResourceTestParameters::includePrivateKey() const
{
    return d_includePrivateKey;
}

bool ResourceTestParameters::includeCertificate() const
{
    return d_includeCertificate;
}

bsl::size_t ResourceTestParameters::trustChainCount() const
{
    return d_trustChainCount;
}

bsl::size_t ResourceTestParameters::trustChainDepth() const
{
    return d_trustChainDepth;
}

bsl::ostream& ResourceTestParameters::print(bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    bsl::stringstream ss;
    ss << d_variationIndex << '/' << d_variationCount;

    printer.printAttribute("id", ss.str());
    printer.printAttribute("keyType", d_keyType);
    printer.printAttribute("resourceEncoderOptions", d_resourceEncoderOptions);
    printer.printAttribute("resourceDecoderOptions", d_resourceDecoderOptions);
    printer.printAttribute("includePrivateKey", d_includePrivateKey);
    printer.printAttribute("includeCertificate", d_includeCertificate);
    printer.printAttribute("trustChainCount", d_trustChainCount);
    printer.printAttribute("trustChainDepth", d_trustChainDepth);

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const ResourceTestParameters& object)
{
    return object.print(stream, 0, -1);
}

void ResourceTestParametersUtil::loadKeyTypes(
    bsl::vector<ntca::EncryptionKeyType::Value>* result)
{
    result->clear();

    // result->push_back(ntca::EncryptionKeyType::e_DSA);
    // result->push_back(ntca::EncryptionKeyType::e_RSA);
    result->push_back(ntca::EncryptionKeyType::e_NIST_P256);
    // result->push_back(ntca::EncryptionKeyType::e_NIST_P384);
    // result->push_back(ntca::EncryptionKeyType::e_NIST_P521);
    // result->push_back(ntca::EncryptionKeyType::e_ED25519);
    // result->push_back(ntca::EncryptionKeyType::e_ED448);
}

void ResourceTestParametersUtil::loadResourceTypes(
    bsl::vector<ntca::EncryptionResourceType::Value>* result)
{
    result->clear();

    result->push_back(ntca::EncryptionResourceType::e_ASN1);
    result->push_back(ntca::EncryptionResourceType::e_ASN1_PEM);
    result->push_back(ntca::EncryptionResourceType::e_PKCS8);
    result->push_back(ntca::EncryptionResourceType::e_PKCS8_PEM);
    result->push_back(ntca::EncryptionResourceType::e_PKCS12);
}

void ResourceTestParametersUtil::loadResourceOptions(
    bsl::vector<ntca::EncryptionResourceOptions>* result)
{
    bsl::vector<ntca::EncryptionResourceType::Value> resourceTypes;
    ResourceTestParametersUtil::loadResourceTypes(&resourceTypes);

    ntca::EncryptionSecret secret;
    secret.append(k_DEFAULT_SECRET, sizeof k_DEFAULT_SECRET - 1);

    {
        ntca::EncryptionResourceOptions options;
        result->push_back(options);
    }

    {
        ntca::EncryptionResourceOptions options;
        options.setEncrypted(true);
        result->push_back(options);
    }

    {
        ntca::EncryptionResourceOptions options;
        options.setEncrypted(true);
        options.setSecret(secret);
        result->push_back(options);
    }

    {
        ntca::EncryptionResourceOptions options;
        options.setEncrypted(true);
        options.setSecretCallback(
            &ntctls::ResourceTestParametersUtil::loadSecret);
        result->push_back(options);
    }

    for (bsl::size_t i = 0; i < resourceTypes.size(); ++i) {
        ntca::EncryptionResourceType::Value resourceType = resourceTypes[i];

        {
            ntca::EncryptionResourceOptions options;
            options.setType(resourceType);
            result->push_back(options);
        }

#if 0
        {
            ntca::EncryptionResourceOptions options;
            options.setType(resourceType);
            options.setSecret(secret);
            result->push_back(options);
        }

        {
            ntca::EncryptionResourceOptions options;
            options.setType(resourceType);
            options.setSecretCallback(
                &ntctls::ResourceTestParametersUtil::loadSecret);
            result->push_back(options);
        }

        {
            ntca::EncryptionResourceOptions options;
            options.setType(resourceType);
            options.setEncrypted(false);
            result->push_back(options);
        }

        {
            ntca::EncryptionResourceOptions options;
            options.setType(resourceType);
            options.setEncrypted(false);
            options.setSecret(secret);
            result->push_back(options);
        }

        {
            ntca::EncryptionResourceOptions options;
            options.setType(resourceType);
            options.setEncrypted(false);
            options.setSecretCallback(
                &ntctls::ResourceTestParametersUtil::loadSecret);
            result->push_back(options);
        }
#endif

        {
            ntca::EncryptionResourceOptions options;
            options.setType(resourceType);
            options.setEncrypted(true);
            result->push_back(options);
        }

        {
            ntca::EncryptionResourceOptions options;
            options.setType(resourceType);
            options.setEncrypted(true);
            options.setSecret(secret);
            result->push_back(options);
        }

        {
            ntca::EncryptionResourceOptions options;
            options.setType(resourceType);
            options.setEncrypted(true);
            options.setSecretCallback(
                &ntctls::ResourceTestParametersUtil::loadSecret);
            result->push_back(options);
        }
    }
}

void ResourceTestParametersUtil::loadParameters(
    bsl::vector<ntctls::ResourceTestParameters>* result)
{
    bsl::vector<ntca::EncryptionKeyType::Value> keyTypeVector;
    ntctls::ResourceTestParametersUtil::loadKeyTypes(&keyTypeVector);

    bsl::vector<ntca::EncryptionResourceOptions> resourceOptionsVector;
    ntctls::ResourceTestParametersUtil::loadResourceOptions(
        &resourceOptionsVector);

    for (bsl::size_t i = 0; i < keyTypeVector.size(); ++i) {
        for (bsl::size_t j = 0; j < resourceOptionsVector.size(); ++j) {
            for (bsl::size_t k = 0; k < resourceOptionsVector.size(); ++k) {
                for (bsl::size_t chainCount = k_MIN_TRUST_CHAIN_COUNT;
                     chainCount <= k_MAX_TRUST_CHAIN_COUNT;
                     ++chainCount)
                {
                    for (bsl::size_t chainDepth = k_MIN_TRUST_CHAIN_DEPTH;
                         chainDepth <= k_MAX_TRUST_CHAIN_DEPTH;
                         ++chainDepth)
                    {
                        ntctls::ResourceTestParameters parameters;

                        parameters.setKeyType(keyTypeVector[i]);
                        parameters.setResourceEncoderOptions(
                            resourceOptionsVector[j]);
                        parameters.setResourceDecoderOptions(
                            resourceOptionsVector[k]);

                        parameters.setTrustChainCount(chainCount);
                        parameters.setTrustChainDepth(chainDepth);

                        parameters.setIncludePrivateKey(false);
                        parameters.setIncludeCertificate(false);

                        result->push_back(parameters);

                        parameters.setIncludePrivateKey(true);
                        parameters.setIncludeCertificate(false);

                        result->push_back(parameters);

                        parameters.setIncludePrivateKey(false);
                        parameters.setIncludeCertificate(true);

                        result->push_back(parameters);

                        parameters.setIncludePrivateKey(true);
                        parameters.setIncludeCertificate(true);

                        result->push_back(parameters);
                    }
                }
            }
        }
    }

    BSLS_ASSERT_OPT(result->size() > 0);

    for (bsl::size_t i = 0; i < result->size(); ++i) {
        (*result)[i].setVariationIndex(i);
        (*result)[i].setVariationCount(result->size());
    }
}

ntsa::Error ResourceTestParametersUtil::loadSecret(
    ntca::EncryptionSecret* result)
{
    result->reset();
    result->append(k_DEFAULT_SECRET, sizeof k_DEFAULT_SECRET - 1);

    return ntsa::Error();
}

ntsa::Error ResourceTestParametersUtil::loadResource(
    bsl::shared_ptr<ntci::EncryptionResource>* result,
    const ntctls::ResourceTestParameters&      parameters,
    bslma::Allocator*                          basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntci::EncryptionDriver> driver;
    ntctls::Plugin::load(&driver);

    bsl::shared_ptr<ntci::EncryptionResource> resource;
    error = driver->createEncryptionResource(&resource, allocator);
    NTSCFG_TEST_OK(error);

    bsl::shared_ptr<ntci::EncryptionKey>         authorityKeyDefault;
    bsl::shared_ptr<ntci::EncryptionCertificate> authorityCertificateDefault;

    const bsl::size_t chainCount = parameters.trustChainCount();
    const bsl::size_t chainDepth = parameters.trustChainDepth();

    BSLS_ASSERT_OPT(chainCount <= 9);
    BSLS_ASSERT_OPT(chainDepth <= 26);

    for (bsl::size_t i = 0; i < chainCount; ++i) {
        typedef bsl::vector<bsl::shared_ptr<ntci::EncryptionKey> > KeyVector;
        typedef bsl::vector<bsl::shared_ptr<ntci::EncryptionCertificate> >
            CertificateVector;

        KeyVector         authorityKeyVector;
        CertificateVector authorityCertificateVector;

        for (bsl::size_t j = 0; j < chainDepth; ++j) {
            bsl::string authoritySubjectCommonName;
            {
                bsl::stringstream ss;
                ss << "TEST.AUTHORITY." << i << ".";

                if (j == 0) {
                    ss << "ROOT";
                }
                else {
                    ss << static_cast<char>('A' + j);
                }
                authoritySubjectCommonName = ss.str();
            }

            ntca::EncryptionKeyOptions authorityKeyOptions;
            authorityKeyOptions.setType(parameters.keyType());

            bsl::shared_ptr<ntci::EncryptionKey> authorityKey;
            error = driver->generateKey(&authorityKey,
                                        authorityKeyOptions,
                                        allocator);
            NTSCFG_TEST_OK(error);

            ntsa::DistinguishedName authoritySubject;
            authoritySubject["CN"] = authoritySubjectCommonName;
            authoritySubject["O"]  = "Bloomberg LP";

            ntca::EncryptionCertificateOptions authorityCertificateOptions;
            authorityCertificateOptions.setAuthority(true);

            bsl::shared_ptr<ntci::EncryptionCertificate> authorityCertificate;

            if (j > 0) {
                error = driver->generateCertificate(
                    &authorityCertificate,
                    authoritySubject,
                    authorityKey,
                    authorityCertificateVector[j - 1],
                    authorityKeyVector[j - 1],
                    authorityCertificateOptions,
                    allocator);
                NTSCFG_TEST_OK(error);
            }
            else {
                error =
                    driver->generateCertificate(&authorityCertificate,
                                                authoritySubject,
                                                authorityKey,
                                                authorityCertificateOptions);
                NTSCFG_TEST_OK(error);
            }

            if (!authorityKeyDefault) {
                authorityKeyDefault = authorityKey;
            }

            if (!authorityCertificateDefault) {
                authorityCertificate = authorityCertificate;
            }

            authorityKeyVector.push_back(authorityKey);
            authorityCertificateVector.push_back(authorityCertificate);

            error = resource->addCertificateAuthority(authorityCertificate);
            NTSCFG_TEST_OK(error);
        }
    }

    bsl::string userSubjectCommonName = "TEST.USER";

    ntca::EncryptionKeyOptions userKeyOptions;
    userKeyOptions.setType(parameters.keyType());

    bsl::shared_ptr<ntci::EncryptionKey> userKey;
    error = driver->generateKey(&userKey, userKeyOptions, allocator);
    NTSCFG_TEST_OK(error);

    ntsa::DistinguishedName userSubject;
    userSubject["CN"] = userSubjectCommonName;
    userSubject["O"]  = "Bloomberg LP";

    ntca::EncryptionCertificateOptions userCertificateOptions;
    userCertificateOptions.setAuthority(false);

    bsl::shared_ptr<ntci::EncryptionCertificate> userCertificate;
    if (authorityCertificateDefault && authorityKeyDefault) {
        error = driver->generateCertificate(&userCertificate,
                                            userSubject,
                                            userKey,
                                            authorityCertificateDefault,
                                            authorityKeyDefault,
                                            userCertificateOptions);
        NTSCFG_TEST_OK(error);
    }
    else {
        error = driver->generateCertificate(&userCertificate,
                                            userSubject,
                                            userKey,
                                            userCertificateOptions);
        NTSCFG_TEST_OK(error);
    }

    if (parameters.includePrivateKey()) {
        error = resource->setPrivateKey(userKey);
        NTSCFG_TEST_OK(error);
    }

    if (parameters.includeCertificate()) {
        error = resource->setCertificate(userCertificate);
        NTSCFG_TEST_OK(error);
    }

    *result = resource;
    return ntsa::Error();
}

void ResourceTestParametersUtil::logHexDump(
    ntca::EncryptionResourceType::Value type,
    const bdlsb::MemOutStreamBuf&       osb)
{
    bsl::stringstream ss;
    if (osb.length() > 0) {
        ss << bdlb::PrintStringHexDumper(osb.data(),
                                         static_cast<int>(osb.length()));
    }

    BSLS_LOG_TRACE("Encoded %s:\n%s",
                   ntca::EncryptionResourceType::toString(type),
                   ss.str().c_str());
}

void ResourceTestParametersUtil::logKey(
    const char*                                 label,
    const bsl::shared_ptr<ntci::EncryptionKey>& key)
{
    bsl::stringstream ss;
    key->print(ss);

    BSLS_LOG_DEBUG("%s = \n%s", label, ss.str().c_str());
}

void ResourceTestParametersUtil::logCertificate(
    const char*                                         label,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate)
{
    bsl::stringstream ss;
    certificate->print(ss);

    BSLS_LOG_DEBUG("%s = \n%s", label, ss.str().c_str());
}

void ResourceTestParametersUtil::verifyLoadResult(
    ntsa::Error                           error,
    const ntctls::ResourceTestParameters& parameters)
{
    ntsa::Error expectedError;

    if (expectedError) {
        NTSCFG_TEST_TRUE(error);
    }
    else {
        NTSCFG_TEST_OK(error);
    }
}

void ResourceTestParametersUtil::verifyEncodeResult(
    ntsa::Error                           error,
    const ntctls::ResourceTestParameters& parameters)
{
    if (!error) {
        return;
    }

    ntsa::Error expectedError;

    const ntca::EncryptionResourceType::Value type =
        parameters.resourceEncoderOptions().type().value_or(
            ntca::EncryptionResourceType::e_ASN1_PEM);

    bsl::size_t numPrivateKeys = 0;
    if (parameters.includePrivateKey()) {
        ++numPrivateKeys;
    }

    bsl::size_t numCertificates = 0;
    if (parameters.includeCertificate()) {
        ++numCertificates;
    }

    numCertificates +=
        parameters.trustChainCount() * parameters.trustChainDepth();

    if (numPrivateKeys == 0 && numCertificates == 0) {
        expectedError = ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (type == ntca::EncryptionResourceType::e_PKCS8 ||
        type == ntca::EncryptionResourceType::e_PKCS8_PEM)
    {
        if (numPrivateKeys == 0) {
            expectedError = ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (numCertificates > 0) {
            expectedError = ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (type == ntca::EncryptionResourceType::e_PKCS7 ||
        type == ntca::EncryptionResourceType::e_PKCS7_PEM)
    {
        if (numCertificates == 0) {
            expectedError = ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (numPrivateKeys > 0) {
            expectedError = ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    const bool encrypted =
        parameters.resourceEncoderOptions().encrypted().value_or(false);

    if (encrypted) {
        if (parameters.resourceEncoderOptions().secret().isNull() &&
            parameters.resourceEncoderOptions().secretCallback().isNull())
        {
            expectedError = ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (expectedError) {
        NTSCFG_TEST_TRUE(error);
    }
    else {
        NTSCFG_TEST_OK(error);
    }
}

void ResourceTestParametersUtil::verifyDecodeResult(
    ntsa::Error                           error,
    const ntctls::ResourceTestParameters& parameters)
{
    if (!error) {
        return;
    }

    ntsa::Error expectedError;

    if (!parameters.resourceDecoderOptions().type().isNull()) {
        ntca::EncryptionResourceType::Value decoderType =
            parameters.resourceDecoderOptions().type().value_or(
                ntca::EncryptionResourceType::e_ASN1_PEM);

        ntca::EncryptionResourceType::Value encoderType =
            parameters.resourceEncoderOptions().type().value_or(
                ntca::EncryptionResourceType::e_ASN1_PEM);

        if (decoderType != encoderType) {
            expectedError = ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    const bool encrypted =
        parameters.resourceEncoderOptions().encrypted().value_or(false);

    if (encrypted) {
        if (parameters.resourceDecoderOptions().secret().isNull() &&
            parameters.resourceDecoderOptions().secretCallback().isNull())
        {
            expectedError = ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (expectedError) {
        NTSCFG_TEST_TRUE(error);
    }
    else {
        NTSCFG_TEST_OK(error);
    }
}

void ResourceTestParametersUtil::verifyEquals(
    const bsl::shared_ptr<ntci::EncryptionResource>& foundResource,
    const bsl::shared_ptr<ntci::EncryptionResource>& expectedResource,
    const ntctls::ResourceTestParameters&            parameters)
{
    bool includePrivateKeys  = true;
    bool includeCertificates = true;

    if (!parameters.resourceDecoderOptions().type().isNull()) {
        const ntca::EncryptionResourceType::Value type =
            parameters.resourceDecoderOptions().type().value();

        if (type == ntca::EncryptionResourceType::e_PKCS7 ||
            type == ntca::EncryptionResourceType::e_PKCS7_PEM)
        {
            includePrivateKeys = false;
        }
        else if (type == ntca::EncryptionResourceType::e_PKCS8 ||
                 type == ntca::EncryptionResourceType::e_PKCS8_PEM)
        {
            includeCertificates = false;
        }
    }
    else {
        includePrivateKeys  = true;
        includeCertificates = true;
    }

    bool result =
        ResourceTestParametersUtil::verifyContains(foundResource,
                                                   expectedResource,
                                                   includePrivateKeys,
                                                   includeCertificates);

    NTSCFG_TEST_TRUE(result);
}

bool ResourceTestParametersUtil::verifyContains(
    const bsl::shared_ptr<ntci::EncryptionResource>& lhsResource,
    const bsl::shared_ptr<ntci::EncryptionResource>& rhsResource,
    bool                                             includePrivateKeys,
    bool                                             includeCertificates)
{
    if (includePrivateKeys) {
        bsl::shared_ptr<ntci::EncryptionKey> lhsKey;
        lhsResource->getPrivateKey(&lhsKey);

        bsl::shared_ptr<ntci::EncryptionKey> rhsKey;
        rhsResource->getPrivateKey(&rhsKey);

        if (lhsKey) {
            if (!rhsKey) {
                return false;
            }
        }

        if (rhsKey) {
            if (!lhsKey) {
                return false;
            }
        }

        if (lhsKey && rhsKey) {
            if (!lhsKey->equals(*rhsKey)) {
                return false;
            }
        }
    }

    if (includeCertificates) {
        bsl::shared_ptr<ntci::EncryptionCertificate> lhsCertificate;
        lhsResource->getCertificate(&lhsCertificate);

        bsl::shared_ptr<ntci::EncryptionCertificate> rhsCertificate;
        rhsResource->getCertificate(&rhsCertificate);

        if (lhsCertificate) {
            if (!rhsCertificate) {
                return false;
            }
        }

        if (rhsCertificate) {
            if (!lhsCertificate) {
                return false;
            }
        }

        if (lhsCertificate && rhsCertificate) {
            if (!lhsCertificate->equals(*rhsCertificate)) {
                return false;
            }
        }

        typedef bsl::vector<bsl::shared_ptr<ntci::EncryptionCertificate> >
            CertificateVector;

        CertificateVector lhsCaList;
        lhsResource->getCertificateAuthoritySet(&lhsCaList);

        CertificateVector rhsCaList;
        rhsResource->getCertificateAuthoritySet(&rhsCaList);

        if (lhsCaList.size() != rhsCaList.size()) {
            return false;
        }

        for (bsl::size_t i = 0; i < lhsCaList.size(); ++i) {
            if (!lhsCaList[i]->equals(*rhsCaList[i])) {
                return false;
            }
        }
    }

    return true;
}

void ResourceTest::verifyUsageParameters(
    const ntctls::ResourceTestParameters& parameters,
    bslma::Allocator*                     allocator)
{
    ntsa::Error error;
    int         rc;

    NTSCFG_TEST_LOG_INFO << "Testing parameters " << parameters
                         << NTSCFG_TEST_LOG_END;

    bsl::shared_ptr<ntci::EncryptionDriver> driver;
    ntctls::Plugin::load(&driver);

    // Load the resource.

    bsl::shared_ptr<ntci::EncryptionResource> resource1;

    error = ntctls::ResourceTestParametersUtil::loadResource(&resource1,
                                                             parameters,
                                                             allocator);

    ntctls::ResourceTestParametersUtil::verifyLoadResult(error, parameters);
    if (error) {
        return;
    }

    // Encode the resource.

    bdlsb::MemOutStreamBuf osb(allocator);

    error = resource1->encode(&osb, parameters.resourceEncoderOptions());
    ntctls::ResourceTestParametersUtil::verifyEncodeResult(error, parameters);
    if (error) {
        return;
    }

    rc = osb.pubsync();
    NTSCFG_TEST_EQ(rc, 0);

    ntctls::ResourceTestParametersUtil::logHexDump(
        parameters.resourceEncoderOptions().type().value_or(
            ntca::EncryptionResourceType::e_ASN1_PEM),
        osb);

    NTSCFG_TEST_NE(osb.data(), 0);
    NTSCFG_TEST_GT(osb.length(), 0);

    // Decode the resource.

    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());

    bsl::shared_ptr<ntci::EncryptionResource> resource2;
    error = driver->createEncryptionResource(&resource2, allocator);
    NTSCFG_TEST_OK(error);

    error = resource2->decode(&isb, parameters.resourceDecoderOptions());
    ntctls::ResourceTestParametersUtil::verifyDecodeResult(error, parameters);
    if (error) {
        return;
    }

    // Ensure the decoded resource matches the encoded resource.

    ntctls::ResourceTestParametersUtil::verifyEquals(resource2,
                                                     resource1,
                                                     parameters);
}

NTSCFG_TEST_FUNCTION(ntctls::ResourceTest::verifyUsage)
{
    NTCI_LOG_CONTEXT();

    NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_MAIN();

    bslma::Allocator* allocator = NTSCFG_TEST_ALLOCATOR;

    bsl::vector<ntctls::ResourceTestParameters> parametersVector;
    ntctls::ResourceTestParametersUtil::loadParameters(&parametersVector);

    for (bsl::size_t i = 0; i < parametersVector.size(); ++i) {
        const ntctls::ResourceTestParameters& parameters = parametersVector[i];

#if defined(NTCTLS_RESOURCE_TEST_CASE_ID)
        if (parameters.variationIndex() != NTCTLS_RESOURCE_TEST_CASE_ID) {
            continue;
        }
#endif

        ntctls::ResourceTest::verifyUsageParameters(parameters, allocator);
    }
}

/// Describes the certificates and keys used in this test driver.
class EncryptionTestEnvironment
{
    bsl::string                        d_domainName;
    ntsa::IpAddress                    d_ipAddress;
    ntca::EncryptionKey                d_roguePrivateKey;
    ntca::EncryptionKeyOptions         d_roguePrivateKeyOptions;
    ntca::EncryptionCertificate        d_rogueCertificate;
    ntca::EncryptionCertificateOptions d_rogueCertificateOptions;
    ntca::EncryptionKey                d_authorityPrivateKey;
    ntca::EncryptionKeyOptions         d_authorityPrivateKeyOptions;
    ntca::EncryptionCertificate        d_authorityCertificate;
    ntca::EncryptionCertificateOptions d_authorityCertificateOptions;
    ntca::EncryptionKey                d_clientPrivateKey;
    ntca::EncryptionKeyOptions         d_clientPrivateKeyOptions;
    ntca::EncryptionCertificate        d_clientCertificate;
    ntca::EncryptionCertificateOptions d_clientCertificateOptions;
    ntca::EncryptionKey                d_serverPrivateKey;
    ntca::EncryptionKeyOptions         d_serverPrivateKeyOptions;
    ntca::EncryptionCertificate        d_serverCertificate;
    ntca::EncryptionCertificateOptions d_serverCertificateOptions;
    bsl::string                        d_serverOneName;
    ntca::EncryptionKey                d_serverOnePrivateKey;
    ntca::EncryptionKeyOptions         d_serverOnePrivateKeyOptions;
    ntca::EncryptionCertificate        d_serverOneCertificate;
    ntca::EncryptionCertificateOptions d_serverOneCertificateOptions;
    bsl::string                        d_serverTwoName;
    ntca::EncryptionKey                d_serverTwoPrivateKey;
    ntca::EncryptionKeyOptions         d_serverTwoPrivateKeyOptions;
    ntca::EncryptionCertificate        d_serverTwoCertificate;
    ntca::EncryptionCertificateOptions d_serverTwoCertificateOptions;
    bslma::Allocator*                  d_allocator_p;

  public:
    // Create a new test environment. Optionally specify a 'basicAllocator'
    // used to supply memory. If 'basicAllocator' is 0, the currently installed
    // default allocator is used.
    explicit EncryptionTestEnvironment(bslma::Allocator* basicAllocator);

    // Destroy this object.
    ~EncryptionTestEnvironment();

    // Return the domain name of the machine on which the test is running.
    const bsl::string& domainName() const;

    // Return the IP address of the machine on which the test is running.
    const ntsa::IpAddress& ipAddress() const;

    // Return the certificate used by an untrusted certificate authority.
    const ntca::EncryptionCertificate& rogueCertificate() const;

    // Return the private key used by an untrusted certificate authority.
    const ntca::EncryptionKey& roguePrivateKey() const;

    // Return the certificate used by an trusted certificate authority.
    const ntca::EncryptionCertificate& authorityCertificate() const;

    // Return the private key used by an trusted certificate authority.
    const ntca::EncryptionKey& authorityPrivateKey() const;

    // Return the certificate used by an end-user acting as a client.
    const ntca::EncryptionCertificate& clientCertificate() const;

    // Return the private key used by an end-user acting as a client.
    const ntca::EncryptionKey& clientPrivateKey() const;

    // Return the certificate used by an end-user acting as a server.
    const ntca::EncryptionCertificate& serverCertificate() const;

    // Return the private key used by an end-user acting as a server.
    const ntca::EncryptionKey& serverPrivateKey() const;

    // Return the server name indication of the first name.
    const bsl::string& serverOneName() const;

    // Return the certificate used by an end-user acting as a server for the
    // indication for the first name.
    const ntca::EncryptionCertificate& serverOneCertificate() const;

    // Return the private key used by an end-user acting as a server for the
    // indidication of the first name.
    const ntca::EncryptionKey& serverOnePrivateKey() const;

    // Return the server name indication of the second name.
    const bsl::string& serverTwoName() const;

    // Return the certificate used by an end-user acting as a server for the
    // indication for the second name.
    const ntca::EncryptionCertificate& serverTwoCertificate() const;

    // Return the private key used by an end-user acting as a server for the
    // indidication of the second name.
    const ntca::EncryptionKey& serverTwoPrivateKey() const;
};

EncryptionTestEnvironment::EncryptionTestEnvironment(
    bslma::Allocator* basicAllocator)
: d_domainName(basicAllocator)
, d_ipAddress()
, d_roguePrivateKey(basicAllocator)
, d_roguePrivateKeyOptions(basicAllocator)
, d_rogueCertificate(basicAllocator)
, d_rogueCertificateOptions(basicAllocator)
, d_authorityPrivateKey(basicAllocator)
, d_authorityPrivateKeyOptions(basicAllocator)
, d_authorityCertificate(basicAllocator)
, d_authorityCertificateOptions(basicAllocator)
, d_clientPrivateKey(basicAllocator)
, d_clientPrivateKeyOptions(basicAllocator)
, d_clientCertificate(basicAllocator)
, d_clientCertificateOptions(basicAllocator)
, d_serverPrivateKey(basicAllocator)
, d_serverPrivateKeyOptions(basicAllocator)
, d_serverCertificate(basicAllocator)
, d_serverCertificateOptions(basicAllocator)
, d_serverOneName(basicAllocator)
, d_serverOnePrivateKey(basicAllocator)
, d_serverOnePrivateKeyOptions(basicAllocator)
, d_serverOneCertificate(basicAllocator)
, d_serverOneCertificateOptions(basicAllocator)
, d_serverTwoName(basicAllocator)
, d_serverTwoPrivateKey(basicAllocator)
, d_serverTwoPrivateKeyOptions(basicAllocator)
, d_serverTwoCertificate(basicAllocator)
, d_serverTwoCertificateOptions(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> driver;
    ntctls::Plugin::load(&driver);

    error = ntsf::System::getHostnameFullyQualified(&d_domainName);
    NTSCFG_TEST_OK(error);

    ntsa::IpAddressOptions ipAddressOptions;
    ipAddressOptions.setIpAddressType(ntsa::IpAddressType::e_V4);

    bsl::vector<ntsa::IpAddress> ipAddressList;
    error = ntsf::System::getIpAddress(&ipAddressList,
                                       d_domainName,
                                       ipAddressOptions);
    NTSCFG_TEST_OK(error);
    NTSCFG_TEST_FALSE(ipAddressList.empty());

    d_ipAddress = ipAddressList.front();

    // Generate a certificate and private key for a certificate rogue.

    d_roguePrivateKeyOptions.setType(ntca::EncryptionKeyType::e_NIST_P256);

    error = driver->generateKey(&d_roguePrivateKey,
                                d_roguePrivateKeyOptions,
                                d_allocator_p);
    NTSCFG_TEST_OK(error);

    ntsa::DistinguishedName rogueIdentity;
    rogueIdentity["CN"] = "Rogue";

    d_rogueCertificateOptions.setSerialNumber(1);
    d_rogueCertificateOptions.setAuthority(true);

    error = driver->generateCertificate(&d_rogueCertificate,
                                        rogueIdentity,
                                        d_roguePrivateKey,
                                        d_rogueCertificateOptions,
                                        d_allocator_p);
    NTSCFG_TEST_OK(error);

    // Generate a certificate and private key for a certificate authority.

    d_authorityPrivateKeyOptions.setType(ntca::EncryptionKeyType::e_NIST_P256);

    error = driver->generateKey(&d_authorityPrivateKey,
                                d_authorityPrivateKeyOptions,
                                d_allocator_p);
    NTSCFG_TEST_OK(error);

    ntsa::DistinguishedName authorityIdentity;
    authorityIdentity["CN"] = "Authority";

    d_authorityCertificateOptions.setSerialNumber(2);
    d_authorityCertificateOptions.setAuthority(true);

    error = driver->generateCertificate(&d_authorityCertificate,
                                        authorityIdentity,
                                        d_authorityPrivateKey,
                                        d_authorityCertificateOptions,
                                        d_allocator_p);
    NTSCFG_TEST_OK(error);

    // Generate a certificate and private key for the client, signed by the
    // certificate authority.

    d_clientPrivateKeyOptions.setType(ntca::EncryptionKeyType::e_NIST_P256);

    error = driver->generateKey(&d_clientPrivateKey,
                                d_clientPrivateKeyOptions,
                                d_allocator_p);
    NTSCFG_TEST_OK(error);

    ntsa::DistinguishedName clientIdentity;
    clientIdentity["CN"] = "Client";

    d_clientCertificateOptions.setSerialNumber(3);
    d_clientCertificateOptions.addHost(d_domainName);
    d_clientCertificateOptions.addHost(d_ipAddress);

    error = driver->generateCertificate(&d_clientCertificate,
                                        clientIdentity,
                                        d_clientPrivateKey,
                                        d_authorityCertificate,
                                        d_authorityPrivateKey,
                                        d_clientCertificateOptions,
                                        d_allocator_p);
    NTSCFG_TEST_OK(error);

    // Generate a certificate and private key for the server, signed by the
    // certificate authority.

    d_serverPrivateKeyOptions.setType(ntca::EncryptionKeyType::e_NIST_P256);

    error = driver->generateKey(&d_serverPrivateKey,
                                d_serverPrivateKeyOptions,
                                d_allocator_p);
    NTSCFG_TEST_OK(error);

    ntsa::DistinguishedName serverIdentity;
    serverIdentity["CN"] = "Server";

    d_serverCertificateOptions.setSerialNumber(4);
    d_serverCertificateOptions.addHost(d_domainName);
    d_serverCertificateOptions.addHost(d_ipAddress);

    error = driver->generateCertificate(&d_serverCertificate,
                                        serverIdentity,
                                        d_serverPrivateKey,
                                        d_authorityCertificate,
                                        d_authorityPrivateKey,
                                        d_serverCertificateOptions,
                                        d_allocator_p);
    NTSCFG_TEST_OK(error);

    // Generate a certificate and private key for the server indication "one",
    // signed by the certificate authority.

    d_serverOneName = "one";

    d_serverOnePrivateKeyOptions.setType(ntca::EncryptionKeyType::e_NIST_P256);

    error = driver->generateKey(&d_serverOnePrivateKey,
                                d_serverOnePrivateKeyOptions,
                                d_allocator_p);
    NTSCFG_TEST_OK(error);

    ntsa::DistinguishedName serverOneIdentity;
    serverOneIdentity["CN"] = "ServerOne";

    d_serverOneCertificateOptions.setSerialNumber(5);
    d_serverOneCertificateOptions.addHost(d_domainName);
    d_serverOneCertificateOptions.addHost(d_ipAddress);

    error = driver->generateCertificate(&d_serverOneCertificate,
                                        serverOneIdentity,
                                        d_serverOnePrivateKey,
                                        d_authorityCertificate,
                                        d_authorityPrivateKey,
                                        d_serverOneCertificateOptions,
                                        d_allocator_p);
    NTSCFG_TEST_OK(error);

    // Generate a certificate and private key for the server indication "two",
    // signed by the certificate authority.

    d_serverTwoName = "two";

    d_serverTwoPrivateKeyOptions.setType(ntca::EncryptionKeyType::e_NIST_P256);

    error = driver->generateKey(&d_serverTwoPrivateKey,
                                d_serverTwoPrivateKeyOptions,
                                d_allocator_p);
    NTSCFG_TEST_OK(error);

    ntsa::DistinguishedName serverTwoIdentity;
    serverTwoIdentity["CN"] = "ServerTwo";

    d_serverTwoCertificateOptions.setSerialNumber(6);
    d_serverTwoCertificateOptions.addHost(d_domainName);
    d_serverTwoCertificateOptions.addHost(d_ipAddress);

    error = driver->generateCertificate(&d_serverTwoCertificate,
                                        serverTwoIdentity,
                                        d_serverTwoPrivateKey,
                                        d_authorityCertificate,
                                        d_authorityPrivateKey,
                                        d_serverTwoCertificateOptions,
                                        d_allocator_p);
    NTSCFG_TEST_OK(error);
}

EncryptionTestEnvironment::~EncryptionTestEnvironment()
{
}

const bsl::string& EncryptionTestEnvironment::domainName() const
{
    return d_domainName;
}

const ntsa::IpAddress& EncryptionTestEnvironment::ipAddress() const
{
    return d_ipAddress;
}

const ntca::EncryptionCertificate& EncryptionTestEnvironment::
    rogueCertificate() const
{
    return d_rogueCertificate;
}

const ntca::EncryptionKey& EncryptionTestEnvironment::roguePrivateKey() const
{
    return d_roguePrivateKey;
}

const ntca::EncryptionCertificate& EncryptionTestEnvironment::
    authorityCertificate() const
{
    return d_authorityCertificate;
}

const ntca::EncryptionKey& EncryptionTestEnvironment::authorityPrivateKey()
    const
{
    return d_authorityPrivateKey;
}

const ntca::EncryptionCertificate& EncryptionTestEnvironment::
    clientCertificate() const
{
    return d_clientCertificate;
}

const ntca::EncryptionKey& EncryptionTestEnvironment::clientPrivateKey() const
{
    return d_clientPrivateKey;
}

const ntca::EncryptionCertificate& EncryptionTestEnvironment::
    serverCertificate() const
{
    return d_serverCertificate;
}

const ntca::EncryptionKey& EncryptionTestEnvironment::serverPrivateKey() const
{
    return d_serverPrivateKey;
}

const bsl::string& EncryptionTestEnvironment::serverOneName() const
{
    return d_serverOneName;
}

const ntca::EncryptionCertificate& EncryptionTestEnvironment::
    serverOneCertificate() const
{
    return d_serverOneCertificate;
}

const ntca::EncryptionKey& EncryptionTestEnvironment::serverOnePrivateKey()
    const
{
    return d_serverOnePrivateKey;
}

const bsl::string& EncryptionTestEnvironment::serverTwoName() const
{
    return d_serverTwoName;
}

const ntca::EncryptionCertificate& EncryptionTestEnvironment::
    serverTwoCertificate() const
{
    return d_serverTwoCertificate;
}

const ntca::EncryptionKey& EncryptionTestEnvironment::serverTwoPrivateKey()
    const
{
    return d_serverTwoPrivateKey;
}

// Describes the configurable parameters of the test driver.
class EncryptionTestParameters
{
    bsl::size_t                           d_variationIndex;
    bsl::size_t                           d_variationCount;
    int                                   d_bufferSize;
    ntca::EncryptionAuthentication::Value d_clientAuthentication;
    ntca::EncryptionMethod::Value         d_clientMinMethod;
    ntca::EncryptionMethod::Value         d_clientMaxMethod;
    ntca::EncryptionAuthentication::Value d_serverAuthentication;
    ntca::EncryptionMethod::Value         d_serverMinMethod;
    ntca::EncryptionMethod::Value         d_serverMaxMethod;
    bsl::string                           d_serverNameIndication;
    bsl::size_t                           d_numReuses;
    bool                                  d_success;

  public:
    // Create new test parameters. Optionally specify a 'basicAllocator' used
    // to supply memory. If 'basicAllocator' is 0, the currently installed
    // default allocator is used.
    explicit EncryptionTestParameters(bslma::Allocator* basicAllocator = 0);

    // Create new test parameters having the same value as the specified
    // 'original' object. Optionally specify a 'basicAllocator' used to supply
    // memory. If 'basicAllocator' is 0, the currently installed default
    // allocator is used.
    EncryptionTestParameters(const EncryptionTestParameters& original,
                             bslma::Allocator* basicAllocator = 0);

    // Destroy this object.
    ~EncryptionTestParameters();

    // Assign the value of the specified 'other' object to this object. Return
    // a reference to this object.
    EncryptionTestParameters& operator=(const EncryptionTestParameters& other);

    // Reset the value of this object to its value upon default construction.
    void reset();

    // Set the variation index to the specified 'value'.
    void setVariationIndex(bsl::size_t value);

    // Set the variation count to the specified 'value'.
    void setVariationCount(bsl::size_t value);

    // Set the buffer size to the specified 'value'.
    void setBufferSize(bsl::size_t value);

    // Set the client authentication to the specified 'value'.
    void setClientAuthentication(ntca::EncryptionAuthentication::Value value);

    // Set the minimium supported client method to the specified 'value'.
    void setClientMinMethod(ntca::EncryptionMethod::Value value);

    // Set the maximum supported client method to the specified 'value'.
    void setClientMaxMethod(ntca::EncryptionMethod::Value value);

    // Set the server authentication to the specified 'value'.
    void setServerAuthentication(ntca::EncryptionAuthentication::Value value);

    // Set the minimium supported client method to the specified 'value'.
    void setServerMinMethod(ntca::EncryptionMethod::Value value);

    // Set the maximum supported client method to the specified 'value'.
    void setServerMaxMethod(ntca::EncryptionMethod::Value value);

    // Set the server name indication to the specified 'value'.
    void setServerNameIndication(const bsl::string& value);

    // Set the number of reuses to the specified 'value'.
    void setReuseCount(bsl::size_t value);

    // Set the flag that indicates the parameters should result in successful
    // authentication and encryption to the specified 'value'.
    void setSuccess(bool value);

    // Return the variation index.
    bsl::size_t variationIndex() const;

    // Return the variation count.
    bsl::size_t variationCount() const;

    // Return the buffer size.
    bsl::size_t bufferSize() const;

    // Return the client authentication.
    ntca::EncryptionAuthentication::Value clientAuthentication() const;

    // Return the minimum supported client method.
    ntca::EncryptionMethod::Value clientMinMethod() const;

    // Return the maximum supported client method.
    ntca::EncryptionMethod::Value clientMaxMethod() const;

    // Return the server authentication.
    ntca::EncryptionAuthentication::Value serverAuthentication() const;

    // Return the minimum supported server method.
    ntca::EncryptionMethod::Value serverMinMethod() const;

    // Return the maximum supported server method.
    ntca::EncryptionMethod::Value serverMaxMethod() const;

    // Return the server name indication.
    const bsl::string& serverNameIndication() const;

    // Return the number of reuses.
    bsl::size_t reuseCount() const;

    // Return the flag that indicates the parameters should result in
    // successful authentication and encryption.
    bool success() const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(EncryptionTestParameters);
};

// Write a formatted, human-readable description of the specified 'object' to
// the specified 'stream'. Return a reference to the 'stream'.
bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const EncryptionTestParameters& object);

EncryptionTestParameters::EncryptionTestParameters(
    bslma::Allocator* basicAllocator)
: d_variationIndex(0)
, d_variationCount(0)
, d_bufferSize(4096)
, d_clientAuthentication(ntca::EncryptionAuthentication::e_DEFAULT)
, d_clientMinMethod(ntca::EncryptionMethod::e_DEFAULT)
, d_clientMaxMethod(ntca::EncryptionMethod::e_DEFAULT)
, d_serverAuthentication(ntca::EncryptionAuthentication::e_DEFAULT)
, d_serverMinMethod(ntca::EncryptionMethod::e_DEFAULT)
, d_serverMaxMethod(ntca::EncryptionMethod::e_DEFAULT)
, d_serverNameIndication(basicAllocator)
, d_numReuses(0)
, d_success(true)
{
}

EncryptionTestParameters::EncryptionTestParameters(
    const EncryptionTestParameters& original,
    bslma::Allocator*               basicAllocator)
: d_variationIndex(original.d_variationIndex)
, d_variationCount(original.d_variationCount)
, d_bufferSize(original.d_bufferSize)
, d_clientAuthentication(original.d_clientAuthentication)
, d_clientMinMethod(original.d_clientMinMethod)
, d_clientMaxMethod(original.d_clientMaxMethod)
, d_serverAuthentication(original.d_serverAuthentication)
, d_serverMinMethod(original.d_serverMinMethod)
, d_serverMaxMethod(original.d_serverMaxMethod)
, d_serverNameIndication(original.d_serverNameIndication, basicAllocator)
, d_numReuses(original.d_numReuses)
, d_success(original.d_success)
{
}

EncryptionTestParameters::~EncryptionTestParameters()
{
}

EncryptionTestParameters& EncryptionTestParameters::operator=(
    const EncryptionTestParameters& other)
{
    if (this != &other) {
        d_variationIndex       = other.d_variationIndex;
        d_variationCount       = other.d_variationCount;
        d_bufferSize           = other.d_bufferSize;
        d_clientAuthentication = other.d_clientAuthentication;
        d_clientMinMethod      = other.d_clientMinMethod;
        d_clientMaxMethod      = other.d_clientMaxMethod;
        d_serverAuthentication = other.d_serverAuthentication;
        d_serverMinMethod      = other.d_serverMinMethod;
        d_serverMaxMethod      = other.d_serverMaxMethod;
        d_serverNameIndication = other.d_serverNameIndication;
        d_numReuses            = other.d_numReuses;
        d_success              = other.d_success;
    }

    return *this;
}

void EncryptionTestParameters::reset()
{
    d_variationIndex       = 0;
    d_variationCount       = 0;
    d_bufferSize           = 4096;
    d_clientAuthentication = ntca::EncryptionAuthentication::e_DEFAULT;
    d_clientMinMethod      = ntca::EncryptionMethod::e_DEFAULT;
    d_clientMaxMethod      = ntca::EncryptionMethod::e_DEFAULT;
    d_serverAuthentication = ntca::EncryptionAuthentication::e_DEFAULT;
    d_serverMinMethod      = ntca::EncryptionMethod::e_DEFAULT;
    d_serverMaxMethod      = ntca::EncryptionMethod::e_DEFAULT;
    d_serverNameIndication.clear();
    d_numReuses = 0;
    d_success   = true;
}

void EncryptionTestParameters::setVariationIndex(bsl::size_t value)
{
    d_variationIndex = value;
}

void EncryptionTestParameters::setVariationCount(bsl::size_t value)
{
    d_variationCount = value;
}

void EncryptionTestParameters::setBufferSize(bsl::size_t value)
{
    d_bufferSize = value;
}

void EncryptionTestParameters::setClientAuthentication(
    ntca::EncryptionAuthentication::Value value)
{
    d_clientAuthentication = value;
}

void EncryptionTestParameters::setClientMinMethod(
    ntca::EncryptionMethod::Value value)
{
    d_clientMinMethod = value;
}

void EncryptionTestParameters::setClientMaxMethod(
    ntca::EncryptionMethod::Value value)
{
    d_clientMaxMethod = value;
}

void EncryptionTestParameters::setServerAuthentication(
    ntca::EncryptionAuthentication::Value value)
{
    d_serverAuthentication = value;
}

void EncryptionTestParameters::setServerMinMethod(
    ntca::EncryptionMethod::Value value)
{
    d_serverMinMethod = value;
}

void EncryptionTestParameters::setServerMaxMethod(
    ntca::EncryptionMethod::Value value)
{
    d_serverMaxMethod = value;
}

void EncryptionTestParameters::setServerNameIndication(
    const bsl::string& value)
{
    d_serverNameIndication = value;
}

void EncryptionTestParameters::setReuseCount(bsl::size_t value)
{
    d_numReuses = value;
}

void EncryptionTestParameters::setSuccess(bool value)
{
    d_success = value;
}

bsl::size_t EncryptionTestParameters::variationIndex() const
{
    return d_variationIndex;
}

bsl::size_t EncryptionTestParameters::variationCount() const
{
    return d_variationCount;
}

bsl::size_t EncryptionTestParameters::bufferSize() const
{
    return d_bufferSize;
}

ntca::EncryptionAuthentication::Value EncryptionTestParameters::
    clientAuthentication() const
{
    return d_clientAuthentication;
}

ntca::EncryptionMethod::Value EncryptionTestParameters::clientMinMethod() const
{
    return d_clientMinMethod;
}

ntca::EncryptionMethod::Value EncryptionTestParameters::clientMaxMethod() const
{
    return d_clientMaxMethod;
}

ntca::EncryptionAuthentication::Value EncryptionTestParameters::
    serverAuthentication() const
{
    return d_serverAuthentication;
}

ntca::EncryptionMethod::Value EncryptionTestParameters::serverMinMethod() const
{
    return d_serverMinMethod;
}

ntca::EncryptionMethod::Value EncryptionTestParameters::serverMaxMethod() const
{
    return d_serverMaxMethod;
}

const bsl::string& EncryptionTestParameters::serverNameIndication() const
{
    return d_serverNameIndication;
}

bsl::size_t EncryptionTestParameters::reuseCount() const
{
    return d_numReuses;
}

bool EncryptionTestParameters::success() const
{
    return d_success;
}

bsl::ostream& EncryptionTestParameters::print(bsl::ostream& stream,
                                              int           level,
                                              int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    bsl::stringstream ss;
    ss << d_variationIndex << '/' << d_variationCount;

    printer.printAttribute("id", ss.str());
    printer.printAttribute("bufferSize", d_bufferSize);
    printer.printAttribute("clientAuthentication", d_clientAuthentication);
    printer.printAttribute("clientMinMethod", d_clientMinMethod);
    printer.printAttribute("clientMaxMethod", d_clientMaxMethod);
    printer.printAttribute("serverAuthentication", d_serverAuthentication);
    printer.printAttribute("serverMinMethod", d_serverMinMethod);
    printer.printAttribute("serverMaxMethod", d_serverMaxMethod);

    if (!d_serverNameIndication.empty()) {
        printer.printAttribute("serverNameIndication", d_serverNameIndication);
    }

    printer.printAttribute("reuseCount", d_numReuses);
    printer.printAttribute("success", d_success);

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const EncryptionTestParameters& object)
{
    return object.print(stream, 0, -1);
}

// Define a type alias for a vector of test parameters.
typedef bsl::vector<EncryptionTestParameters> EncryptionTestParametersVector;

// Provide utiltities for generating test parameters.
class EncryptionTestParametersUtil
{
  public:
    static void generateForEach(
        ntctls::EncryptionTestParametersVector* result);

    static void generateForEachBufferSize(
        ntctls::EncryptionTestParametersVector* result,
        const ntctls::EncryptionTestParameters& prototype);

    static void generateForEachClientAuthentication(
        ntctls::EncryptionTestParametersVector* result,
        const ntctls::EncryptionTestParameters& prototype);

    static void generateForEachClientMinMethod(
        ntctls::EncryptionTestParametersVector* result,
        const ntctls::EncryptionTestParameters& prototype);

    static void generateForEachClientMaxMethod(
        ntctls::EncryptionTestParametersVector* result,
        const ntctls::EncryptionTestParameters& prototype);

    static void generateForEachServerAuthentication(
        ntctls::EncryptionTestParametersVector* result,
        const ntctls::EncryptionTestParameters& prototype);

    static void generateForEachServerMinMethod(
        ntctls::EncryptionTestParametersVector* result,
        const ntctls::EncryptionTestParameters& prototype);

    static void generateForEachServerMaxMethod(
        ntctls::EncryptionTestParametersVector* result,
        const ntctls::EncryptionTestParameters& prototype);

    static void generateForEachServerNameIndication(
        ntctls::EncryptionTestParametersVector* result,
        const ntctls::EncryptionTestParameters& prototype);

    static void generateForEachReuse(
        ntctls::EncryptionTestParametersVector* result,
        const ntctls::EncryptionTestParameters& prototype);

    static void generateForEachEntry(
        ntctls::EncryptionTestParametersVector* result,
        const ntctls::EncryptionTestParameters& prototype);
};

#define NTCTLS_ENCRYPTION_TEST_SIMPLE 0

void EncryptionTestParametersUtil::generateForEach(
    ntctls::EncryptionTestParametersVector* result)
{
    result->clear();

    ntctls::EncryptionTestParameters prototype;
    EncryptionTestParametersUtil::generateForEachBufferSize(result, prototype);

    for (bsl::size_t i = 0; i < result->size(); ++i) {
        (*result)[i].setVariationIndex(i);
        (*result)[i].setVariationCount(result->size());
    }
}

void EncryptionTestParametersUtil::generateForEachBufferSize(
    ntctls::EncryptionTestParametersVector* result,
    const ntctls::EncryptionTestParameters& prototype)
{
    bsl::vector<int> bufferSizeVector;

#if NTCTLS_ENCRYPTION_TEST_SIMPLE
    bufferSizeVector.push_back(4096);
#else
    bufferSizeVector.push_back(1);
    bufferSizeVector.push_back(2);
    bufferSizeVector.push_back(4);
    bufferSizeVector.push_back(8);
    bufferSizeVector.push_back(32);
    bufferSizeVector.push_back(1024);
    bufferSizeVector.push_back(4096);
#endif

    for (bsl::size_t i = 0; i < bufferSizeVector.size(); ++i) {
        ntctls::EncryptionTestParameters entry = prototype;
        entry.setBufferSize(bufferSizeVector[i]);
        EncryptionTestParametersUtil::generateForEachClientAuthentication(
            result,
            entry);
    }
}

void EncryptionTestParametersUtil::generateForEachClientAuthentication(
    ntctls::EncryptionTestParametersVector* result,
    const ntctls::EncryptionTestParameters& prototype)
{
    bsl::vector<ntca::EncryptionAuthentication::Value> authenticationVector;

#if NTCTLS_ENCRYPTION_TEST_SIMPLE
    authenticationVector.push_back(ntca::EncryptionAuthentication::e_VERIFY);
#else
    authenticationVector.push_back(ntca::EncryptionAuthentication::e_NONE);
    authenticationVector.push_back(ntca::EncryptionAuthentication::e_VERIFY);
#endif

    for (bsl::size_t i = 0; i < authenticationVector.size(); ++i) {
        ntctls::EncryptionTestParameters entry = prototype;
        entry.setClientAuthentication(authenticationVector[i]);
        EncryptionTestParametersUtil::generateForEachClientMinMethod(result,
                                                                     entry);
    }
}

void EncryptionTestParametersUtil::generateForEachClientMinMethod(
    ntctls::EncryptionTestParametersVector* result,
    const ntctls::EncryptionTestParameters& prototype)
{
    bsl::vector<ntca::EncryptionMethod::Value> methodVector;

#if NTCTLS_ENCRYPTION_TEST_SIMPLE
    methodVector.push_back(ntca::EncryptionMethod::e_TLS_V1_3);
#else
    methodVector.push_back(ntca::EncryptionMethod::e_TLS_V1_2);
    methodVector.push_back(ntca::EncryptionMethod::e_TLS_V1_3);
    methodVector.push_back(ntca::EncryptionMethod::e_TLS_V1_X);
#endif

    for (bsl::size_t i = 0; i < methodVector.size(); ++i) {
        ntctls::EncryptionTestParameters entry = prototype;
        entry.setClientMinMethod(methodVector[i]);
        EncryptionTestParametersUtil::generateForEachClientMaxMethod(result,
                                                                     entry);
    }
}

void EncryptionTestParametersUtil::generateForEachClientMaxMethod(
    ntctls::EncryptionTestParametersVector* result,
    const ntctls::EncryptionTestParameters& prototype)
{
    bsl::vector<ntca::EncryptionMethod::Value> methodVector;

#if NTCTLS_ENCRYPTION_TEST_SIMPLE
    methodVector.push_back(ntca::EncryptionMethod::e_TLS_V1_3);
#else
    methodVector.push_back(ntca::EncryptionMethod::e_TLS_V1_2);
    methodVector.push_back(ntca::EncryptionMethod::e_TLS_V1_3);
    methodVector.push_back(ntca::EncryptionMethod::e_TLS_V1_X);
#endif

    for (bsl::size_t i = 0; i < methodVector.size(); ++i) {
        ntctls::EncryptionTestParameters entry = prototype;
        entry.setClientMaxMethod(methodVector[i]);
        EncryptionTestParametersUtil::generateForEachServerAuthentication(
            result,
            entry);
    }
}

void EncryptionTestParametersUtil::generateForEachServerAuthentication(
    ntctls::EncryptionTestParametersVector* result,
    const ntctls::EncryptionTestParameters& prototype)
{
    bsl::vector<ntca::EncryptionAuthentication::Value> authenticationVector;

#if NTCTLS_ENCRYPTION_TEST_SIMPLE
    authenticationVector.push_back(ntca::EncryptionAuthentication::e_NONE);
#else
    authenticationVector.push_back(ntca::EncryptionAuthentication::e_NONE);
    authenticationVector.push_back(ntca::EncryptionAuthentication::e_VERIFY);
#endif

    for (bsl::size_t i = 0; i < authenticationVector.size(); ++i) {
        ntctls::EncryptionTestParameters entry = prototype;
        entry.setServerAuthentication(authenticationVector[i]);
        EncryptionTestParametersUtil::generateForEachServerMinMethod(result,
                                                                     entry);
    }
}

void EncryptionTestParametersUtil::generateForEachServerMinMethod(
    ntctls::EncryptionTestParametersVector* result,
    const ntctls::EncryptionTestParameters& prototype)
{
    bsl::vector<ntca::EncryptionMethod::Value> methodVector;

#if NTCTLS_ENCRYPTION_TEST_SIMPLE
    methodVector.push_back(ntca::EncryptionMethod::e_TLS_V1_3);
#else
    methodVector.push_back(ntca::EncryptionMethod::e_TLS_V1_2);
    methodVector.push_back(ntca::EncryptionMethod::e_TLS_V1_3);
    methodVector.push_back(ntca::EncryptionMethod::e_TLS_V1_X);
#endif

    for (bsl::size_t i = 0; i < methodVector.size(); ++i) {
        ntctls::EncryptionTestParameters entry = prototype;
        entry.setServerMinMethod(methodVector[i]);
        EncryptionTestParametersUtil::generateForEachServerMaxMethod(result,
                                                                     entry);
    }
}

void EncryptionTestParametersUtil::generateForEachServerMaxMethod(
    ntctls::EncryptionTestParametersVector* result,
    const ntctls::EncryptionTestParameters& prototype)
{
    bsl::vector<ntca::EncryptionMethod::Value> methodVector;

#if NTCTLS_ENCRYPTION_TEST_SIMPLE
    methodVector.push_back(ntca::EncryptionMethod::e_TLS_V1_3);
#else
    methodVector.push_back(ntca::EncryptionMethod::e_TLS_V1_2);
    methodVector.push_back(ntca::EncryptionMethod::e_TLS_V1_3);
    methodVector.push_back(ntca::EncryptionMethod::e_TLS_V1_X);
#endif

    for (bsl::size_t i = 0; i < methodVector.size(); ++i) {
        ntctls::EncryptionTestParameters entry = prototype;
        entry.setServerMaxMethod(methodVector[i]);
        EncryptionTestParametersUtil::generateForEachServerNameIndication(
            result,
            entry);
    }
}

void EncryptionTestParametersUtil::generateForEachServerNameIndication(
    ntctls::EncryptionTestParametersVector* result,
    const ntctls::EncryptionTestParameters& prototype)
{
    bsl::vector<bsl::string> serverNameIndicationVector;

#if NTCTLS_ENCRYPTION_TEST_SIMPLE
    serverNameIndicationVector.push_back("");
#else
    serverNameIndicationVector.push_back("");
    serverNameIndicationVector.push_back("one");
    serverNameIndicationVector.push_back("two");
#endif

    for (bsl::size_t i = 0; i < serverNameIndicationVector.size(); ++i) {
        ntctls::EncryptionTestParameters entry = prototype;
        entry.setServerNameIndication(serverNameIndicationVector[i]);
        EncryptionTestParametersUtil::generateForEachReuse(result, entry);
    }
}

void EncryptionTestParametersUtil::generateForEachReuse(
    ntctls::EncryptionTestParametersVector* result,
    const ntctls::EncryptionTestParameters& prototype)
{
    bsl::vector<bsl::size_t> reuseVector;

#if NTCTLS_ENCRYPTION_TEST_SIMPLE
    reuseVector.push_back(0);
#else
    reuseVector.push_back(0);
    reuseVector.push_back(1);
    reuseVector.push_back(2);
#endif

    for (bsl::size_t i = 0; i < reuseVector.size(); ++i) {
        ntctls::EncryptionTestParameters entry = prototype;
        entry.setReuseCount(reuseVector[i]);
        EncryptionTestParametersUtil::generateForEachEntry(result, entry);
    }
}

void EncryptionTestParametersUtil::generateForEachEntry(
    ntctls::EncryptionTestParametersVector* result,
    const ntctls::EncryptionTestParameters& prototype)
{
    ntctls::EncryptionTestParameters parameters = prototype;

    parameters.setSuccess(true);

    // Verification by neither fails because no anonymous key exchange
    // algorithm is supported.

    if (parameters.clientAuthentication() ==
            ntca::EncryptionAuthentication::e_NONE &&
        parameters.serverAuthentication() ==
            ntca::EncryptionAuthentication::e_NONE)
    {
        parameters.setSuccess(false);
    }

    // Server authentication of the client fails unless the client is also
    // authenticating the server (or at least, it fails unless both the client
    // and the server provide certificates during the handshake. This test
    // driver implementation only loads a certificate if the other side is
    // configured to verify it. Determine how to configure OpenSSL such that
    // the client can verify the server without having to provide its own
    // certificate.
    //
    // TODO: It appears there is no configuration available in OpenSSL that
    // supports the server requesting and verifying the client's certificate
    // without also providing its own certificate. See the OpenSSL
    // documentation for SSL_VERIFY_NONE for the client mode.

    if (parameters.clientAuthentication() ==
            ntca::EncryptionAuthentication::e_NONE &&
        parameters.serverAuthentication() ==
            ntca::EncryptionAuthentication::e_VERIFY)
    {
        parameters.setSuccess(false);
    }

    // OpenSSL has an open issue where a resumed session by the client causes
    // the server to fail the handshake with "session id context
    // uninitialized". This error only occurs after the session is used once
    // then attempt to be reused after it has been cleanly shut down. The error
    // does not occur when only the client verifies the server, and the server
    // does not verify the client.
    //
    // See https://github.com/openssl/openssl/issues/10168

    if (parameters.clientAuthentication() ==
            ntca::EncryptionAuthentication::e_VERIFY &&
        parameters.serverAuthentication() ==
            ntca::EncryptionAuthentication::e_VERIFY &&
        parameters.reuseCount() > 0)
    {
        parameters.setSuccess(false);
    }

    if (parameters.clientMinMethod() > parameters.clientMaxMethod()) {
        return;  // NON-SENSICAL
    }

    if (parameters.serverMinMethod() > parameters.serverMaxMethod()) {
        return;  // NON-SENSICAL
    }

    if (parameters.clientMaxMethod() < parameters.serverMinMethod()) {
        parameters.setSuccess(false);
    }

    if (parameters.serverMaxMethod() < parameters.clientMinMethod()) {
        parameters.setSuccess(false);
    }

    result->push_back(parameters);
}

// Provide functions for implementing encryption tests.
class EncryptionTestUtil
{
  public:
    // Load the specified 'parameter's.
    static void logParameters(
        const char*                             label,
        const ntctls::EncryptionTestParameters& parameters);

    // Log the hex dump of the specified 'blob' prefixed by the specified
    // 'label'.
    static void logHexDump(const char* label, const bdlbb::Blob& blob);

    // Authenticate the specified 'client' certificate'. Return the error.
    static bool processClientAuthenticationByServer(
        const ntctls::EncryptionTestEnvironment* environment,
        const ntctls::EncryptionTestParameters*  parameters,
        const ntca::EncryptionCertificate&       client);

    // Authenticate the specified 'client' certificate'. Return the error.
    static bool processServerAuthenticationByClient(
        const ntctls::EncryptionTestEnvironment* environment,
        const ntctls::EncryptionTestParameters*  parameters,
        const ntca::EncryptionCertificate&       server);

    // Process the completion or failure of the handshake of the specified
    // 'clientSession' according to the specified 'error'. The session is
    // established with server identified by the specified 'serverCertificate',
    // if any. Set the specified 'clientCompleteFlag' to true.
    static void processClientHandshakeComplete(
        const ntsa::Error&                                  error,
        const bsl::shared_ptr<ntci::Encryption>&            clientSession,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& serverCertificate,
        const bsl::string&                                  details,
        bool*                                               clientCompleteFlag,
        const ntctls::EncryptionTestParameters&             parameters);

    // Process the completion or failure of the handshake of the specified
    // 'serverSession' according to the specified 'error'. The session is
    // established with server identified by the specified 'serverCertificate',
    // if any. Set the specified 'serverCompleteFlag' to true.
    static void processServerHandshakeComplete(
        const ntsa::Error&                                  error,
        const bsl::shared_ptr<ntci::Encryption>&            serverSession,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& clientCertificate,
        const bsl::string&                                  details,
        bool*                                               serverCompleteFlag,
        const ntctls::EncryptionTestParameters&             parameters);

    // Repeatedly read and write from the specified 'clientSession' and
    // 'serverSession' until both the specified 'clientCompleteFlag' and
    // 'serverCompleteFlag' is set.
    static ntsa::Error cycleHandshake(
        const bsl::shared_ptr<ntci::Encryption>& clientSession,
        const bsl::shared_ptr<ntci::Encryption>& serverSession,
        const bsl::shared_ptr<ntci::DataPool>&   dataPool,
        bdlbb::Blob*                             clientPlaintextRead,
        bdlbb::Blob*                             serverPlaintextRead,
        const ntctls::EncryptionTestParameters&  parameters);

    // Repeatedly read and write from the specified 'clientSession' and
    // 'serverSession' until both the specified 'clientCompleteFlag' and
    // 'serverCompleteFlag' is set.
    static ntsa::Error cycleShutdown(
        const bsl::shared_ptr<ntci::Encryption>& clientSession,
        const bsl::shared_ptr<ntci::Encryption>& serverSession,
        const bsl::shared_ptr<ntci::DataPool>&   dataPool,
        bdlbb::Blob*                             clientPlaintextRead,
        bdlbb::Blob*                             serverPlaintextRead,
        const ntctls::EncryptionTestParameters&  parameters);

    // Execute the test described by the specified 'parameters' using the
    // specified 'authorityCertificate' and 'authorityPrivateKey' for the
    // trusted certificate authority, the specified 'clientCertificate' and
    // 'clientPrivateKey' for the client, and the specified 'serverCertificate'
    // and 'serverPrivateKey'. Optionally specify a 'basicAllocator' used to
    // supply memory. If 'basicAllocator' is 0, the currently installed default
    // allocator is used.
    static void execute(const ntctls::EncryptionTestEnvironment& environment,
                        const ntctls::EncryptionTestParameters&  parameters,
                        bslma::Allocator* basicAllocator = 0);
};

void EncryptionTestUtil::logParameters(
    const char*                             label,
    const ntctls::EncryptionTestParameters& parameters)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_INFO << label << " = " << parameters
                         << NTCI_LOG_STREAM_END;
}

void EncryptionTestUtil::logHexDump(const char* label, const bdlbb::Blob& blob)
{
    NTCI_LOG_CONTEXT();

    bsl::stringstream ss;
    ss << bdlbb::BlobUtilHexDumper(&blob);

    NTCI_LOG_DEBUG("%s %d bytes", label, (int)(blob.length()));

    NTCI_LOG_TRACE("%s:\n%s", label, ss.str().c_str());
}

bool EncryptionTestUtil::processClientAuthenticationByServer(
    const ntctls::EncryptionTestEnvironment* environment,
    const ntctls::EncryptionTestParameters*  parameters,
    const ntca::EncryptionCertificate&       client)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Server authenticated client" << client
                          << NTCI_LOG_STREAM_END;

    return true;
}

bool EncryptionTestUtil::processServerAuthenticationByClient(
    const ntctls::EncryptionTestEnvironment* environment,
    const ntctls::EncryptionTestParameters*  parameters,
    const ntca::EncryptionCertificate&       server)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Client authenticated server" << server
                          << NTCI_LOG_STREAM_END;

    if (parameters->serverNameIndication().empty()) {
        NTSCFG_TEST_EQ(server, environment->serverCertificate());
    }
    else {
        if (parameters->serverNameIndication() == environment->serverOneName())
        {
            NTSCFG_TEST_EQ(server, environment->serverOneCertificate());
        }
        else if (parameters->serverNameIndication() ==
                 environment->serverTwoName())
        {
            NTSCFG_TEST_EQ(server, environment->serverTwoCertificate());
        }
        else {
            NTSCFG_TEST_TRUE(false);
        }
    }

    return true;
}

void EncryptionTestUtil::processClientHandshakeComplete(
    const ntsa::Error&                                  error,
    const bsl::shared_ptr<ntci::Encryption>&            clientSession,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& serverCertificate,
    const bsl::string&                                  details,
    bool*                                               clientCompleteFlag,
    const ntctls::EncryptionTestParameters&             parameters)
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

void EncryptionTestUtil::processServerHandshakeComplete(
    const ntsa::Error&                                  error,
    const bsl::shared_ptr<ntci::Encryption>&            serverSession,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& clientCertificate,
    const bsl::string&                                  details,
    bool*                                               serverCompleteFlag,
    const ntctls::EncryptionTestParameters&             parameters)
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

ntsa::Error EncryptionTestUtil::cycleHandshake(
    const bsl::shared_ptr<ntci::Encryption>& clientSession,
    const bsl::shared_ptr<ntci::Encryption>& serverSession,
    const bsl::shared_ptr<ntci::DataPool>&   dataPool,
    bdlbb::Blob*                             clientPlaintextRead,
    bdlbb::Blob*                             serverPlaintextRead,
    const ntctls::EncryptionTestParameters&  parameters)
{
    NTCI_LOG_CONTEXT();

    NTCCFG_WARNING_UNUSED(parameters);

    ntsa::Error error;

    while (!clientSession->isHandshakeFinished() ||
           !serverSession->isHandshakeFinished())
    {
        {
            NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_CLIENT();

            if (clientSession->hasOutgoingCipherText()) {
                bdlbb::Blob data(dataPool->outgoingBlobBufferFactory().get());
                clientSession->popOutgoingCipherText(&data);

                EncryptionTestUtil::logHexDump("Client sending ciphertext",
                                               data);

                {
                    NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_SERVER();

                    error = serverSession->pushIncomingCipherText(data);
                    if (parameters.success()) {
                        NTSCFG_TEST_OK(error);
                    }
                    else {
                        NTSCFG_TEST_NE(error, ntsa::Error());
                        return error;
                    }
                }
            }
        }

        {
            NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_SERVER();

            if (serverSession->hasOutgoingCipherText()) {
                bdlbb::Blob data(dataPool->outgoingBlobBufferFactory().get());
                serverSession->popOutgoingCipherText(&data);

                EncryptionTestUtil::logHexDump("Server sending ciphertext",
                                               data);

                {
                    NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_CLIENT();

                    error = clientSession->pushIncomingCipherText(data);
                    if (parameters.success()) {
                        NTSCFG_TEST_OK(error);
                    }
                    else {
                        NTSCFG_TEST_NE(error, ntsa::Error());
                        return error;
                    }
                }
            }
        }

        {
            NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_CLIENT();

            if (clientSession->hasIncomingPlainText()) {
                bdlbb::Blob data(dataPool->incomingBlobBufferFactory().get());
                error = clientSession->popIncomingPlainText(&data);
                if (parameters.success()) {
                    NTSCFG_TEST_OK(error);
                }
                else {
                    NTSCFG_TEST_NE(error, ntsa::Error());
                    return error;
                }

                EncryptionTestUtil::logHexDump("Client received plaintext",
                                               data);

                bdlbb::BlobUtil::append(clientPlaintextRead, data);
            }
        }

        {
            NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_SERVER();

            if (serverSession->hasIncomingPlainText()) {
                bdlbb::Blob data(dataPool->incomingBlobBufferFactory().get());
                error = serverSession->popIncomingPlainText(&data);
                if (parameters.success()) {
                    NTSCFG_TEST_OK(error);
                }
                else {
                    NTSCFG_TEST_NE(error, ntsa::Error());
                    return error;
                }

                EncryptionTestUtil::logHexDump("Server received plaintext",
                                               data);

                bdlbb::BlobUtil::append(serverPlaintextRead, data);
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error EncryptionTestUtil::cycleShutdown(
    const bsl::shared_ptr<ntci::Encryption>& clientSession,
    const bsl::shared_ptr<ntci::Encryption>& serverSession,
    const bsl::shared_ptr<ntci::DataPool>&   dataPool,
    bdlbb::Blob*                             clientPlaintextRead,
    bdlbb::Blob*                             serverPlaintextRead,
    const ntctls::EncryptionTestParameters&  parameters)
{
    NTCI_LOG_CONTEXT();

    NTCCFG_WARNING_UNUSED(parameters);

    ntsa::Error error;

    while (!clientSession->isShutdownFinished() ||
           !serverSession->isShutdownFinished())
    {
        {
            NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_CLIENT();

            if (clientSession->hasOutgoingCipherText()) {
                bdlbb::Blob data(dataPool->outgoingBlobBufferFactory().get());
                clientSession->popOutgoingCipherText(&data);

                EncryptionTestUtil::logHexDump("Client sending ciphertext",
                                               data);

                {
                    NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_SERVER();

                    error = serverSession->pushIncomingCipherText(data);
                    if (parameters.success()) {
                        NTSCFG_TEST_OK(error);
                    }
                    else {
                        NTSCFG_TEST_NE(error, ntsa::Error());
                        return error;
                    }
                }
            }
        }

        {
            NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_SERVER();

            if (serverSession->hasOutgoingCipherText()) {
                bdlbb::Blob data(dataPool->outgoingBlobBufferFactory().get());
                serverSession->popOutgoingCipherText(&data);

                EncryptionTestUtil::logHexDump("Server sending ciphertext",
                                               data);

                {
                    NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_CLIENT();

                    error = clientSession->pushIncomingCipherText(data);
                    if (parameters.success()) {
                        NTSCFG_TEST_OK(error);
                    }
                    else {
                        NTSCFG_TEST_NE(error, ntsa::Error());
                        return error;
                    }
                }
            }
        }

        {
            NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_CLIENT();

            if (clientSession->hasIncomingPlainText()) {
                bdlbb::Blob data(dataPool->incomingBlobBufferFactory().get());
                error = clientSession->popIncomingPlainText(&data);
                if (parameters.success()) {
                    NTSCFG_TEST_OK(error);
                }
                else {
                    NTSCFG_TEST_NE(error, ntsa::Error());
                    return error;
                }

                EncryptionTestUtil::logHexDump("Client received plaintext",
                                               data);

                bdlbb::BlobUtil::append(clientPlaintextRead, data);
            }
        }

        {
            NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_SERVER();

            if (serverSession->hasIncomingPlainText()) {
                bdlbb::Blob data(dataPool->incomingBlobBufferFactory().get());
                error = serverSession->popIncomingPlainText(&data);
                if (parameters.success()) {
                    NTSCFG_TEST_OK(error);
                }
                else {
                    NTSCFG_TEST_NE(error, ntsa::Error());
                    return error;
                }

                EncryptionTestUtil::logHexDump("Server received plaintext",
                                               data);

                bdlbb::BlobUtil::append(serverPlaintextRead, data);
            }
        }
    }

    return ntsa::Error();
}

void EncryptionTestUtil::execute(
    const ntctls::EncryptionTestEnvironment& environment,
    const ntctls::EncryptionTestParameters&  parameters,
    bslma::Allocator*                        basicAllocator)
{
    NTCI_LOG_CONTEXT();

    EncryptionTestUtil::logParameters("Testing parameters", parameters);

    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntci::EncryptionDriver> driver;
    ntctls::Plugin::load(&driver);

    // Create a data pool.

    bsl::shared_ptr<ntci::DataPool> dataPool;
    {
        bsl::shared_ptr<ntcs::DataPool> concreteDataPool;
        concreteDataPool.createInplace(allocator,
                                       parameters.bufferSize(),
                                       parameters.bufferSize(),
                                       allocator);

        dataPool = concreteDataPool;
    }

    // Create the client.

    ntca::EncryptionClientOptions clientOptions;

    clientOptions.setAuthentication(parameters.clientAuthentication());
    clientOptions.setMinMethod(parameters.clientMinMethod());
    clientOptions.setMaxMethod(parameters.clientMaxMethod());

    if (parameters.clientAuthentication() ==
        ntca::EncryptionAuthentication::e_VERIFY)
    {
        clientOptions.addAuthority(environment.authorityCertificate());
    }

    if (parameters.serverAuthentication() ==
        ntca::EncryptionAuthentication::e_VERIFY)
    {
        clientOptions.setIdentity(environment.clientCertificate());
        clientOptions.setPrivateKey(environment.clientPrivateKey());
    }

    ntca::EncryptionValidation clientValidation;
    clientValidation.setCallback(
        NTCCFG_BIND(&EncryptionTestUtil::processServerAuthenticationByClient,
                    &environment,
                    &parameters,
                    NTCCFG_BIND_PLACEHOLDER_1));

    clientOptions.setValidation(clientValidation);

    bsl::shared_ptr<ntci::EncryptionClient> client;
    error = driver->createEncryptionClient(&client,
                                           clientOptions,
                                           dataPool,
                                           allocator);
    NTSCFG_TEST_OK(error);

    // Create the server.

    ntca::EncryptionServerOptions serverOptions;

    serverOptions.setAuthentication(parameters.serverAuthentication());
    serverOptions.setMinMethod(parameters.serverMinMethod());
    serverOptions.setMaxMethod(parameters.serverMaxMethod());

    if (parameters.serverAuthentication() ==
        ntca::EncryptionAuthentication::e_VERIFY)
    {
        serverOptions.addAuthority(environment.authorityCertificate());
    }

    if (parameters.clientAuthentication() ==
        ntca::EncryptionAuthentication::e_VERIFY)
    {
        serverOptions.setIdentity(environment.serverCertificate());
        serverOptions.setPrivateKey(environment.serverPrivateKey());
    }

    ntca::EncryptionValidation serverValidation;

    serverValidation.setCallback(
        NTCCFG_BIND(&EncryptionTestUtil::processClientAuthenticationByServer,
                    &environment,
                    &parameters,
                    NTCCFG_BIND_PLACEHOLDER_1));

    serverOptions.setValidation(serverValidation);

    ntca::EncryptionOptions serverOneOptions;

    serverOneOptions.setAuthentication(parameters.serverAuthentication());
    serverOneOptions.setMinMethod(parameters.serverMinMethod());
    serverOneOptions.setMaxMethod(parameters.serverMaxMethod());

    if (parameters.serverAuthentication() ==
        ntca::EncryptionAuthentication::e_VERIFY)
    {
        serverOneOptions.addAuthority(environment.authorityCertificate());
    }

    if (parameters.clientAuthentication() ==
        ntca::EncryptionAuthentication::e_VERIFY)
    {
        serverOneOptions.setIdentity(environment.serverOneCertificate());
        serverOneOptions.setPrivateKey(environment.serverOnePrivateKey());
    }

    serverOneOptions.setValidation(serverValidation);

    serverOptions.addOverrides(environment.serverOneName(), serverOneOptions);

    ntca::EncryptionOptions serverTwoOptions;

    serverTwoOptions.setAuthentication(parameters.serverAuthentication());
    serverTwoOptions.setMinMethod(parameters.serverMinMethod());
    serverTwoOptions.setMaxMethod(parameters.serverMaxMethod());

    if (parameters.serverAuthentication() ==
        ntca::EncryptionAuthentication::e_VERIFY)
    {
        serverTwoOptions.addAuthority(environment.authorityCertificate());
    }

    if (parameters.clientAuthentication() ==
        ntca::EncryptionAuthentication::e_VERIFY)
    {
        serverTwoOptions.setIdentity(environment.serverTwoCertificate());
        serverTwoOptions.setPrivateKey(environment.serverTwoPrivateKey());
    }

    serverTwoOptions.setValidation(serverValidation);

    serverOptions.addOverrides(environment.serverTwoName(), serverTwoOptions);

    bsl::shared_ptr<ntci::EncryptionServer> server;
    error = driver->createEncryptionServer(&server,
                                           serverOptions,
                                           dataPool,
                                           allocator);
    NTSCFG_TEST_OK(error);

    // Create the client session.

    bsl::shared_ptr<ntci::Encryption> clientSession;
    error = client->createEncryption(&clientSession, dataPool, allocator);
    NTSCFG_TEST_OK(error);

    // Create the server session.

    bsl::shared_ptr<ntci::Encryption> serverSession;
    error = server->createEncryption(&serverSession, dataPool, allocator);
    NTSCFG_TEST_OK(error);

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
         usageIteration < parameters.reuseCount() + 1;
         ++usageIteration)
    {
        NTCI_LOG_DEBUG("Iteration %d/%d starting",
                       usageIteration + 1,
                       parameters.reuseCount() + 1);

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
            NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_CLIENT();

            NTCI_LOG_DEBUG("Client handshake initiating");

            ntca::UpgradeOptions clientUpgradeOptions;
            if (!parameters.serverNameIndication().empty()) {
                clientUpgradeOptions.setServerName(
                    parameters.serverNameIndication());
            }

            ntci::Encryption::HandshakeCallback clientUpgradeCallback =
                NTCCFG_BIND(
                    &EncryptionTestUtil::processClientHandshakeComplete,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    clientSession,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    &clientHandshakeComplete,
                    parameters);

            error = clientSession->initiateHandshake(clientUpgradeOptions,
                                                     clientUpgradeCallback);
            if (parameters.success()) {
                NTSCFG_TEST_OK(error);
            }
            else {
                NTSCFG_TEST_NE(error, ntsa::Error());
                return;
            }
        }

        // Initiate the handshake from the server.

        {
            NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_SERVER();

            NTCI_LOG_DEBUG("Server handshake initiating");

            ntca::UpgradeOptions serverUpgradeOptions;

            ntci::Encryption::HandshakeCallback serverUpgradeCallback =
                NTCCFG_BIND(
                    &EncryptionTestUtil::processServerHandshakeComplete,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    serverSession,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    &serverHandshakeComplete,
                    parameters);

            error = serverSession->initiateHandshake(serverUpgradeOptions,
                                                     serverUpgradeCallback);
            if (parameters.success()) {
                NTSCFG_TEST_OK(error);
            }
            else {
                NTSCFG_TEST_NE(error, ntsa::Error());
                return;
            }
        }

        // Send data immediately after the handshake is initiated.

        {
            NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_CLIENT();

            error = clientSession->pushOutgoingPlainText(helloServer);
            if (parameters.success()) {
                NTSCFG_TEST_OK(error);
            }
            else {
                NTSCFG_TEST_NE(error, ntsa::Error());
                return;
            }
        }

        {
            NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_SERVER();

            error = serverSession->pushOutgoingPlainText(helloClient);
            if (parameters.success()) {
                NTSCFG_TEST_OK(error);
            }
            else {
                NTSCFG_TEST_NE(error, ntsa::Error());
                return;
            }
        }

        // Process the TLS state machine until the handshake is complete.

        error = EncryptionTestUtil::cycleHandshake(clientSession,
                                                   serverSession,
                                                   dataPool,
                                                   &clientPlaintextRead,
                                                   &serverPlaintextRead,
                                                   parameters);

        if (parameters.success()) {
            NTSCFG_TEST_OK(error);
        }
        else {
            NTSCFG_TEST_NE(error, ntsa::Error());
            return;
        }

        NTSCFG_TEST_TRUE(clientHandshakeComplete);
        NTSCFG_TEST_TRUE(serverHandshakeComplete);

        // Send data immediately before the shutdown is initiated.

        {
            NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_CLIENT();

            error = clientSession->pushOutgoingPlainText(goodbyeServer);
            if (parameters.success()) {
                NTSCFG_TEST_OK(error);
            }
            else {
                NTSCFG_TEST_NE(error, ntsa::Error());
                return;
            }
        }

        {
            NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_SERVER();

            error = serverSession->pushOutgoingPlainText(goodbyeClient);
            if (parameters.success()) {
                NTSCFG_TEST_OK(error);
            }
            else {
                NTSCFG_TEST_NE(error, ntsa::Error());
                return;
            }
        }

        // Initiate the shutdown from the client.

        {
            NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_CLIENT();

            NTCI_LOG_DEBUG("Client shutdown initiating");

            error = clientSession->shutdown();
            NTSCFG_TEST_OK(error);
        }

        // Initiate the shutdown from the server.

        {
            NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_SERVER();

            NTCI_LOG_DEBUG("Server shutdown initiating");

            error = serverSession->shutdown();
            NTSCFG_TEST_OK(error);
        }

        // Process the TLS state machine until the shutdown is complete.

        error = EncryptionTestUtil::cycleShutdown(clientSession,
                                                  serverSession,
                                                  dataPool,
                                                  &clientPlaintextRead,
                                                  &serverPlaintextRead,
                                                  parameters);
        if (parameters.success()) {
            NTSCFG_TEST_OK(error);
        }
        else {
            NTSCFG_TEST_NE(error, ntsa::Error());
            return;
        }

        // Ensure the plaintext received by the client matches the expected data.

        NTSCFG_TEST_EQ(bdlbb::BlobUtil::compare(clientPlaintextRead,
                                                expectedClientPlaintextRead),
                       0);

        // Ensure the plaintext received by the server matches the expected data.

        NTSCFG_TEST_EQ(bdlbb::BlobUtil::compare(serverPlaintextRead,
                                                expectedServerPlaintextRead),
                       0);

        NTCI_LOG_DEBUG("Iteration %d/%d complete",
                      usageIteration + 1,
                      parameters.reuseCount() + 1);
    }

    NTCI_LOG_DEBUG("Test complete");
}

// Provide utilities for executing test cases.
class EncryptionTest
{
  public:
    // Verify the behavior of encryption sessions with various configuration
    // parameters.
    static void verifyUsage();
};

NTSCFG_TEST_FUNCTION(ntctls::EncryptionTest::verifyUsage)
{
    // Concern: Verify the behavior of encryption sessions with various
    // configuration parameters.

    NTCI_LOG_CONTEXT();

    NTCTLS_PLUGIN_TEST_LOG_CONTEXT_GUARD_MAIN();

    bslma::Allocator* allocator = NTSCFG_TEST_ALLOCATOR;

    ntctls::EncryptionTestEnvironment environment(allocator);

    ntctls::EncryptionTestParametersVector parametersVector(allocator);
    ntctls::EncryptionTestParametersUtil::generateForEach(&parametersVector);

    for (bsl::size_t i = 0; i < parametersVector.size(); ++i) {
        const ntctls::EncryptionTestParameters& parameters =
            parametersVector[i];

        if (!parameters.success()) {
            continue;
        }

        ntctls::EncryptionTestUtil::execute(environment,
                                            parameters,
                                            allocator);
    }
}

}  // close namespace ntctls
}  // close namespace BloombergLP

#endif
