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
BSLS_IDENT_RCSID(ntca_encryptioncertificate_t_cpp, "$Id$ $CSID$")

#include <ntca_encryptioncertificate.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntca {

// Provide tests for 'ntca::EncryptionCertificate'.
class EncryptionCertificateTest
{
  public:
    // Verify the traversal of the DER-encoding of a certificate.
    static void verifyPrerequisites();

    // Verify encoding/decoding end-user certificates.
    static void verifyUser();

    // Verify encoding/decoding trusted certificate authorities.
    static void verifyAuthority();

    // Verify matching domain names.
    static void verifyMatchName();

    // Verify matching endpoints, IP addresses, local names, and URIs.
    static void verifyMatchAddress();

    // Verify the examination of various fields of the certificate.
    static void verifyFields();

  private:
    // The encoded end-user certificate.
    static const bsl::uint8_t k_USER_CERTIFICATE_ASN1[614];

    // The encoded trusted certificate authority.
    static const bsl::uint8_t k_CA_CERTIFICATE_ASN1[479];

    // Provide a I/O stream manipulator to indent output.
    class Indentation;

    // Decode a certificate from the specified 'buffer'.
    static void decodeCertificate(bsl::streambuf* buffer);

    // Decode an ASN.1 constructed element from the specified 'decoder'.
    static void decodeConstructed(ntsa::AbstractSyntaxDecoder* decoder);

    // Decode an ASN.1 sequence from the specified 'decoder'.
    static void decodeSequence(ntsa::AbstractSyntaxDecoder* decoder);

    // Decode an ASN.1 primitive from the specified 'decoder'.
    static void decodePrimitive(ntsa::AbstractSyntaxDecoder* decoder);
};

class EncryptionCertificateTest::Indentation
{
  public:
    bsl::size_t d_numSpaces;

    explicit Indentation(bsl::size_t numSpaces)
    : d_numSpaces(numSpaces)
    {
    }

    friend bsl::ostream& operator<<(bsl::ostream&      stream,
                                    const Indentation& object)
    {
        for (bsl::size_t i = 0; i < object.d_numSpaces; ++i) {
            stream << "    ";
        }

        return stream;
    }
};

NTSCFG_TEST_FUNCTION(ntca::EncryptionCertificateTest::verifyPrerequisites)
{
    ntsa::Error error;

    NTSCFG_TEST_LOG_DEBUG << "Decoding: "
                          << bdlb::PrintStringSingleLineHexDumper(
                                 (const char*)k_USER_CERTIFICATE_ASN1,
                                 sizeof k_USER_CERTIFICATE_ASN1)
                          << NTSCFG_TEST_LOG_END;

    ntca::EncryptionCertificate certificate(NTSCFG_TEST_ALLOCATOR);

    bdlsb::FixedMemInStreamBuf isb(
        reinterpret_cast<const char*>(k_USER_CERTIFICATE_ASN1),
        sizeof k_USER_CERTIFICATE_ASN1);

    EncryptionCertificateTest::decodeCertificate(&isb);
}

NTSCFG_TEST_FUNCTION(ntca::EncryptionCertificateTest::verifyUser)
{
    ntsa::Error error;
    int         rc;

    NTSCFG_TEST_LOG_DEBUG << "Decoding: "
                          << bdlb::PrintStringSingleLineHexDumper(
                                 (const char*)k_USER_CERTIFICATE_ASN1,
                                 sizeof k_USER_CERTIFICATE_ASN1)
                          << NTSCFG_TEST_LOG_END;

    bdlsb::FixedMemInStreamBuf isb(
        reinterpret_cast<const char*>(k_USER_CERTIFICATE_ASN1),
        sizeof k_USER_CERTIFICATE_ASN1);

    ntsa::AbstractSyntaxDecoder decoder(&isb, NTSCFG_TEST_ALLOCATOR);

    ntca::EncryptionCertificate certificate(NTSCFG_TEST_ALLOCATOR);
    error = certificate.decode(&decoder);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_LOG_DEBUG << "Certificate = " << certificate
                          << NTSCFG_TEST_LOG_END;

    bdlsb::MemOutStreamBuf osb(NTSCFG_TEST_ALLOCATOR);

    ntsa::AbstractSyntaxEncoder encoder(&osb, NTSCFG_TEST_ALLOCATOR);

    error = certificate.encode(&encoder);
    NTSCFG_TEST_OK(error);

    rc = osb.pubsync();
    NTSCFG_TEST_EQ(rc, 0);

    NTSCFG_TEST_EQ(osb.length(), sizeof k_USER_CERTIFICATE_ASN1);

    rc = bsl::memcmp(osb.data(),
                     k_USER_CERTIFICATE_ASN1,
                     sizeof k_USER_CERTIFICATE_ASN1);
    NTSCFG_TEST_EQ(rc, 0);

    bool isCa = certificate.isAuthority();
    NTSCFG_TEST_FALSE(isCa);
}

NTSCFG_TEST_FUNCTION(ntca::EncryptionCertificateTest::verifyAuthority)
{
    ntsa::Error error;
    int         rc;

    NTSCFG_TEST_LOG_DEBUG << "Decoding: "
                          << bdlb::PrintStringSingleLineHexDumper(
                                 (const char*)k_CA_CERTIFICATE_ASN1,
                                 sizeof k_CA_CERTIFICATE_ASN1)
                          << NTSCFG_TEST_LOG_END;

    bdlsb::FixedMemInStreamBuf isb(
        reinterpret_cast<const char*>(k_CA_CERTIFICATE_ASN1),
        sizeof k_CA_CERTIFICATE_ASN1);

    ntsa::AbstractSyntaxDecoder decoder(&isb, NTSCFG_TEST_ALLOCATOR);

    ntca::EncryptionCertificate certificate(NTSCFG_TEST_ALLOCATOR);
    error = certificate.decode(&decoder);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_LOG_DEBUG << "Certificate = " << certificate
                          << NTSCFG_TEST_LOG_END;

    bdlsb::MemOutStreamBuf osb(NTSCFG_TEST_ALLOCATOR);

    ntsa::AbstractSyntaxEncoder encoder(&osb, NTSCFG_TEST_ALLOCATOR);

    error = certificate.encode(&encoder);
    NTSCFG_TEST_OK(error);

    rc = osb.pubsync();
    NTSCFG_TEST_EQ(rc, 0);

    NTSCFG_TEST_EQ(osb.length(), sizeof k_CA_CERTIFICATE_ASN1);

    rc = bsl::memcmp(osb.data(),
                     k_CA_CERTIFICATE_ASN1,
                     sizeof k_CA_CERTIFICATE_ASN1);
    NTSCFG_TEST_EQ(rc, 0);

    bool isCa = certificate.isAuthority();
    NTSCFG_TEST_TRUE(isCa);
}

NTSCFG_TEST_FUNCTION(ntca::EncryptionCertificateTest::verifyMatchName)
{
    // clang-format off

    bool match;

    match = ntca::EncryptionCertificateUtil::matches(
        "c.b.a", 
        "c.b.a");
    NTSCFG_TEST_TRUE(match);

    match = ntca::EncryptionCertificateUtil::matches(
        "c.b.x", 
        "c.b.a");
    NTSCFG_TEST_FALSE(match);

    match = ntca::EncryptionCertificateUtil::matches(
        "c.x.a", 
        "c.b.a");
    NTSCFG_TEST_FALSE(match);

    match = ntca::EncryptionCertificateUtil::matches(
        "x.b.a", 
        "c.b.a");
    NTSCFG_TEST_FALSE(match);

    match = ntca::EncryptionCertificateUtil::matches(
        "d.c.b.a", 
            "c.b.a");
    NTSCFG_TEST_FALSE(match);

    match = ntca::EncryptionCertificateUtil::matches(
            "c.b.a", 
        "d.c.b.a");
    NTSCFG_TEST_FALSE(match);

    match = ntca::EncryptionCertificateUtil::matches(
        "d.c.b.a", 
        "*.c.b.a");
    NTSCFG_TEST_TRUE(match);

    match = ntca::EncryptionCertificateUtil::matches(
        "e.d.c.b.a", 
            "*.c.b.a");
    NTSCFG_TEST_TRUE(match);

    // clang-format on
}

NTSCFG_TEST_FUNCTION(ntca::EncryptionCertificateTest::verifyMatchAddress)
{
    bool match;

    {
        ntsa::IpAddress requestedIpAddress("10.0.92.14");
        bsl::string     certifiedString = "10.0.92.14";

        match = ntca::EncryptionCertificateUtil::matches(requestedIpAddress,
                                                         certifiedString);
        NTSCFG_TEST_TRUE(match);
    }

    {
        ntsa::IpAddress requestedIpAddress("10.0.92.254");
        bsl::string     certifiedString = "10.0.92.14";

        match = ntca::EncryptionCertificateUtil::matches(requestedIpAddress,
                                                         certifiedString);
        NTSCFG_TEST_FALSE(match);
    }

    {
        ntsa::LocalName requestedLocalName;
        requestedLocalName.setValue("/path/to/resource");

        bsl::string certifiedString = "/path/to/resource";

        match = ntca::EncryptionCertificateUtil::matches(requestedLocalName,
                                                         certifiedString);
        NTSCFG_TEST_TRUE(match);
    }

    {
        ntsa::LocalName requestedLocalName;
        requestedLocalName.setValue("/path/to/missing");

        bsl::string certifiedString = "/path/to/resource";

        match = ntca::EncryptionCertificateUtil::matches(requestedLocalName,
                                                         certifiedString);
        NTSCFG_TEST_FALSE(match);
    }

    {
        ntsa::Uri requestedUri;
        requestedUri.setScheme("http");
        requestedUri.setHost("found");
        requestedUri.setPort(80);

        bsl::string certifiedString = "found";

        match = ntca::EncryptionCertificateUtil::matches(requestedUri,
                                                         certifiedString);
        NTSCFG_TEST_TRUE(match);
    }

    {
        ntsa::Uri requestedUri;
        requestedUri.setScheme("http");
        requestedUri.setHost("missing");
        requestedUri.setPort(80);

        bsl::string certifiedString = "found";

        match = ntca::EncryptionCertificateUtil::matches(requestedUri,
                                                         certifiedString);
        NTSCFG_TEST_FALSE(match);
    }

    {
        ntsa::IpAddress requestedIpAddress("10.0.92.14");
        ntsa::IpAddress certifiedIpAddress("10.0.92.14");

        match = ntca::EncryptionCertificateUtil::matches(requestedIpAddress,
                                                         certifiedIpAddress);
        NTSCFG_TEST_TRUE(match);
    }

    {
        ntsa::IpAddress requestedIpAddress("10.0.92.254");
        ntsa::IpAddress certifiedIpAddress("10.0.92.14");

        match = ntca::EncryptionCertificateUtil::matches(requestedIpAddress,
                                                         certifiedIpAddress);
        NTSCFG_TEST_FALSE(match);
    }

    {
        ntsa::Uri requestedUri;
        requestedUri.setScheme("http");
        requestedUri.setHost("found");
        requestedUri.setPort(80);

        ntsa::Uri certifiedUri;
        certifiedUri.setScheme("https");  // intentional
        certifiedUri.setHost("found");
        certifiedUri.setPort(8080);  // intentional

        match = ntca::EncryptionCertificateUtil::matches(requestedUri,
                                                         certifiedUri);
        NTSCFG_TEST_TRUE(match);
    }

    {
        ntsa::Uri requestedUri;
        requestedUri.setScheme("http");
        requestedUri.setHost("missing");
        requestedUri.setPort(80);

        ntsa::Uri certifiedUri;
        certifiedUri.setScheme("https");  // intentional
        certifiedUri.setHost("found");
        certifiedUri.setPort(8080);  // intentional

        match = ntca::EncryptionCertificateUtil::matches(requestedUri,
                                                         certifiedUri);
        NTSCFG_TEST_FALSE(match);
    }
}

NTSCFG_TEST_FUNCTION(ntca::EncryptionCertificateTest::verifyFields)
{
    ntsa::Error error;
    int         rc;

    NTSCFG_TEST_LOG_DEBUG << "Decoding: "
                          << bdlb::PrintStringSingleLineHexDumper(
                                 (const char*)k_USER_CERTIFICATE_ASN1,
                                 sizeof k_USER_CERTIFICATE_ASN1)
                          << NTSCFG_TEST_LOG_END;

    bdlsb::FixedMemInStreamBuf isb(
        reinterpret_cast<const char*>(k_USER_CERTIFICATE_ASN1),
        sizeof k_USER_CERTIFICATE_ASN1);

    ntsa::AbstractSyntaxDecoder decoder(&isb, NTSCFG_TEST_ALLOCATOR);

    ntca::EncryptionCertificate certificate(NTSCFG_TEST_ALLOCATOR);
    error = certificate.decode(&decoder);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_LOG_DEBUG << "Certificate = " << certificate
                          << NTSCFG_TEST_LOG_END;

    bool matchesSubjectDomainName =
        certificate.matchesSubjectDomainName("ntf.bloomberg.com");
    NTSCFG_TEST_TRUE(matchesSubjectDomainName);

    bool isEllipticCurve = certificate.usesSubjectPublicKeyAlgorithm(
        ntca::EncryptionKeyAlgorithmIdentifierType::e_ELLIPTIC_CURVE);
    NTSCFG_TEST_TRUE(isEllipticCurve);

    bool isEllipticCurveP256 =
        certificate.usesSubjectPublicKeyAlgorithmParameters(
            ntca::EncryptionKeyEllipticCurveParametersIdentifierType ::
                e_SEC_P256_R1);
    NTSCFG_TEST_TRUE(isEllipticCurveP256);

    bool allowsDigitalSignatures = certificate.allowsKeyUsage(
        ntca::EncryptionCertificateSubjectKeyUsageType ::e_DIGITAL_SIGNATURE);
    NTSCFG_TEST_FALSE(allowsDigitalSignatures);

    bool allowsTlsClients = certificate.allowsKeyUsage(
        ntca::EncryptionCertificateSubjectKeyUsageExtendedType ::e_TLS_CLIENT);
    NTSCFG_TEST_FALSE(allowsTlsClients);

    bool isCa = certificate.isAuthority();
    NTSCFG_TEST_FALSE(isCa);
}

void EncryptionCertificateTest::decodeCertificate(bsl::streambuf* buffer)
{
    ntsa::Error error;

    ntsa::AbstractSyntaxDecoder decoder(buffer, NTSCFG_TEST_ALLOCATOR);

    error = decoder.decodeTag();
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_LOG_DEBUG << Indentation(decoder.depth())
                          << "Context = " << decoder.current()
                          << NTSCFG_TEST_LOG_END;

    NTSCFG_TEST_EQ(decoder.current().tagClass(),
                   ntsa::AbstractSyntaxTagClass::e_UNIVERSAL);

    NTSCFG_TEST_EQ(decoder.current().tagType(),
                   ntsa::AbstractSyntaxTagType::e_CONSTRUCTED);

    NTSCFG_TEST_EQ(decoder.current().tagNumber(),
                   ntsa::AbstractSyntaxTagNumber::e_SEQUENCE);

    EncryptionCertificateTest::decodeConstructed(&decoder);

    error = decoder.decodeTagComplete();
    NTSCFG_TEST_OK(error);
}

void EncryptionCertificateTest::decodeConstructed(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    NTSCFG_TEST_EQ(decoder->current().tagType(),
                   ntsa::AbstractSyntaxTagType::e_CONSTRUCTED);

    if (decoder->current().tagClass() ==
            ntsa::AbstractSyntaxTagClass::e_UNIVERSAL &&
        decoder->current().tagNumber() ==
            ntsa::AbstractSyntaxTagNumber::e_SEQUENCE)
    {
        EncryptionCertificateTest::decodeSequence(decoder);
    }
    else {
        error = decoder->skip();
        NTSCFG_TEST_OK(error);
    }
}

void EncryptionCertificateTest::decodeSequence(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    NTSCFG_TEST_EQ(decoder->current().tagClass(),
                   ntsa::AbstractSyntaxTagClass::e_UNIVERSAL);

    NTSCFG_TEST_EQ(decoder->current().tagType(),
                   ntsa::AbstractSyntaxTagType::e_CONSTRUCTED);

    NTSCFG_TEST_EQ(decoder->current().tagNumber(),
                   ntsa::AbstractSyntaxTagNumber::e_SEQUENCE);

    const bsl::uint64_t endPosition =
        decoder->current().contentPosition() +
        decoder->current().contentLength().value();

    while (true) {
        const bsl::uint64_t currentPosition = decoder->position();

        if (currentPosition >= endPosition) {
            break;
        }

        error = decoder->decodeTag();
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_LOG_DEBUG << Indentation(decoder->depth())
                              << "Context = " << decoder->current()
                              << NTSCFG_TEST_LOG_END;

        if (decoder->current().tagType() ==
            ntsa::AbstractSyntaxTagType::e_CONSTRUCTED)
        {
            EncryptionCertificateTest::decodeConstructed(decoder);
        }
        else {
            EncryptionCertificateTest::decodePrimitive(decoder);
        }

        error = decoder->decodeTagComplete();
        NTSCFG_TEST_OK(error);
    }
}

void EncryptionCertificateTest::decodePrimitive(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    if (decoder->current().tagClass() ==
            ntsa::AbstractSyntaxTagClass::e_UNIVERSAL &&
        decoder->current().tagType() ==
            ntsa::AbstractSyntaxTagType::e_PRIMITIVE)
    {
    }

    decoder->skip();
}

// clang-format off
const bsl::uint8_t EncryptionCertificateTest::k_USER_CERTIFICATE_ASN1[614] = {
    0x30, 0x82, 0x02, 0x62, 0x30, 0x82, 0x02, 0x08, 0xA0, 0x03,
    0x02, 0x01, 0x02, 0x02, 0x01, 0x02, 0x30, 0x0A, 0x06, 0x08,
    0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02, 0x30, 0x48,
    0x31, 0x10, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C,
    0x07, 0x54, 0x45, 0x53, 0x54, 0x2E, 0x43, 0x41, 0x31, 0x15,
    0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x0C, 0x42,
    0x6C, 0x6F, 0x6F, 0x6D, 0x62, 0x65, 0x72, 0x67, 0x20, 0x4C,
    0x50, 0x31, 0x1D, 0x30, 0x1B, 0x06, 0x03, 0x55, 0x04, 0x0B,
    0x0C, 0x14, 0x4D, 0x69, 0x64, 0x64, 0x6C, 0x65, 0x77, 0x61,
    0x72, 0x65, 0x20, 0x54, 0x72, 0x61, 0x6E, 0x73, 0x70, 0x6F,
    0x72, 0x74, 0x30, 0x1E, 0x17, 0x0D, 0x32, 0x34, 0x30, 0x34,
    0x32, 0x33, 0x31, 0x34, 0x33, 0x34, 0x32, 0x31, 0x5A, 0x17,
    0x0D, 0x32, 0x35, 0x30, 0x34, 0x32, 0x33, 0x31, 0x34, 0x33,
    0x34, 0x32, 0x31, 0x5A, 0x30, 0x7C, 0x31, 0x12, 0x30, 0x10,
    0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x09, 0x54, 0x45, 0x53,
    0x54, 0x2E, 0x55, 0x53, 0x45, 0x52, 0x31, 0x15, 0x30, 0x13,
    0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x0C, 0x42, 0x6C, 0x6F,
    0x6F, 0x6D, 0x62, 0x65, 0x72, 0x67, 0x20, 0x4C, 0x50, 0x31,
    0x14, 0x30, 0x12, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x0B,
    0x45, 0x6E, 0x67, 0x69, 0x6E, 0x65, 0x65, 0x72, 0x69, 0x6E,
    0x67, 0x31, 0x1A, 0x30, 0x18, 0x06, 0x03, 0x55, 0x04, 0x0B,
    0x0C, 0x11, 0x50, 0x6C, 0x61, 0x74, 0x66, 0x6F, 0x72, 0x6D,
    0x20, 0x53, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x73, 0x31,
    0x1D, 0x30, 0x1B, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x14,
    0x4D, 0x69, 0x64, 0x64, 0x6C, 0x65, 0x77, 0x61, 0x72, 0x65,
    0x20, 0x54, 0x72, 0x61, 0x6E, 0x73, 0x70, 0x6F, 0x72, 0x74,
    0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE,
    0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D,
    0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0xDF, 0x0D, 0x91,
    0xDD, 0x48, 0x1B, 0xF5, 0xE0, 0x5E, 0x93, 0x1C, 0x54, 0x20,
    0x9E, 0xAD, 0xB6, 0xA2, 0x86, 0x10, 0x3F, 0x8F, 0x29, 0x9A,
    0xB0, 0x05, 0xF9, 0x84, 0x34, 0x34, 0x7F, 0x08, 0x30, 0xCB,
    0x65, 0x62, 0x84, 0x17, 0x87, 0x72, 0x59, 0x99, 0xF0, 0x76,
    0x82, 0xD2, 0x4B, 0x56, 0xBF, 0x7E, 0x9A, 0x2D, 0xF6, 0x89,
    0xE0, 0x06, 0x05, 0x45, 0xCF, 0xD7, 0x69, 0xFF, 0x1B, 0x84,
    0x6F, 0xA3, 0x81, 0xAE, 0x30, 0x81, 0xAB, 0x30, 0x09, 0x06,
    0x03, 0x55, 0x1D, 0x13, 0x04, 0x02, 0x30, 0x00, 0x30, 0x1D,
    0x06, 0x03, 0x55, 0x1D, 0x0E, 0x04, 0x16, 0x04, 0x14, 0xB0,
    0xF6, 0xEA, 0x19, 0x1E, 0xFE, 0xA6, 0xC2, 0x36, 0xF5, 0x8C,
    0x9C, 0x38, 0xD2, 0xD9, 0x04, 0x62, 0x55, 0x02, 0x6F, 0x30,
    0x1F, 0x06, 0x03, 0x55, 0x1D, 0x23, 0x04, 0x18, 0x30, 0x16,
    0x80, 0x14, 0x2C, 0x27, 0xDB, 0x5A, 0x05, 0xC8, 0xA1, 0x49,
    0xFB, 0xC8, 0x1C, 0xFB, 0x40, 0x68, 0xC7, 0xA2, 0xBA, 0x42,
    0xD8, 0xDA, 0x30, 0x1C, 0x06, 0x03, 0x55, 0x1D, 0x11, 0x04,
    0x15, 0x30, 0x13, 0x82, 0x11, 0x6E, 0x74, 0x66, 0x2E, 0x62,
    0x6C, 0x6F, 0x6F, 0x6D, 0x62, 0x65, 0x72, 0x67, 0x2E, 0x63,
    0x6F, 0x6D, 0x30, 0x1E, 0x06, 0x03, 0x55, 0x1D, 0x11, 0x04,
    0x17, 0x30, 0x15, 0x82, 0x13, 0x2A, 0x2E, 0x64, 0x65, 0x76,
    0x2E, 0x62, 0x6C, 0x6F, 0x6F, 0x6D, 0x62, 0x65, 0x72, 0x67,
    0x2E, 0x63, 0x6F, 0x6D, 0x30, 0x0F, 0x06, 0x03, 0x55, 0x1D,
    0x11, 0x04, 0x08, 0x30, 0x06, 0x87, 0x04, 0x0A, 0x22, 0x06,
    0x17, 0x30, 0x0F, 0x06, 0x03, 0x55, 0x1D, 0x11, 0x04, 0x08,
    0x30, 0x06, 0x87, 0x04, 0x0A, 0x22, 0x07, 0x57, 0x30, 0x0A,
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02,
    0x03, 0x48, 0x00, 0x30, 0x45, 0x02, 0x20, 0x37, 0x2E, 0xA7,
    0x2E, 0xC0, 0x63, 0x26, 0x60, 0x46, 0xB7, 0x37, 0xD9, 0x19,
    0xD6, 0x60, 0x89, 0x19, 0xBA, 0x43, 0xB8, 0x61, 0x90, 0x77,
    0x5B, 0x7F, 0x6D, 0xB5, 0xD8, 0xAE, 0x7B, 0x93, 0xC2, 0x02,
    0x21, 0x00, 0xD5, 0xB2, 0x8C, 0x90, 0xA1, 0x68, 0x5D, 0xA8,
    0xB5, 0xAF, 0xA2, 0xAF, 0x30, 0x08, 0x11, 0xC4, 0xCC, 0x73,
    0x9D, 0x60, 0xB2, 0xEF, 0x9D, 0xC5, 0xA9, 0x63, 0x71, 0xB3,
    0xF4, 0x5A, 0x89, 0xF4
};

const bsl::uint8_t EncryptionCertificateTest::k_CA_CERTIFICATE_ASN1[479] = {
    0x30, 0x82, 0x01, 0xDB, 0x30, 0x82, 0x01, 0x82, 0xA0, 0x03,
    0x02, 0x01, 0x02, 0x02, 0x01, 0x01, 0x30, 0x0A, 0x06, 0x08,
    0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02, 0x30, 0x48,
    0x31, 0x10, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C,
    0x07, 0x54, 0x45, 0x53, 0x54, 0x2E, 0x43, 0x41, 0x31, 0x15,
    0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x0C, 0x42,
    0x6C, 0x6F, 0x6F, 0x6D, 0x62, 0x65, 0x72, 0x67, 0x20, 0x4C,
    0x50, 0x31, 0x1D, 0x30, 0x1B, 0x06, 0x03, 0x55, 0x04, 0x0B,
    0x0C, 0x14, 0x4D, 0x69, 0x64, 0x64, 0x6C, 0x65, 0x77, 0x61,
    0x72, 0x65, 0x20, 0x54, 0x72, 0x61, 0x6E, 0x73, 0x70, 0x6F,
    0x72, 0x74, 0x30, 0x1E, 0x17, 0x0D, 0x32, 0x34, 0x30, 0x34,
    0x32, 0x32, 0x31, 0x32, 0x30, 0x33, 0x34, 0x34, 0x5A, 0x17,
    0x0D, 0x32, 0x35, 0x30, 0x34, 0x32, 0x32, 0x31, 0x32, 0x30,
    0x33, 0x34, 0x34, 0x5A, 0x30, 0x48, 0x31, 0x10, 0x30, 0x0E,
    0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x07, 0x54, 0x45, 0x53,
    0x54, 0x2E, 0x43, 0x41, 0x31, 0x15, 0x30, 0x13, 0x06, 0x03,
    0x55, 0x04, 0x0A, 0x0C, 0x0C, 0x42, 0x6C, 0x6F, 0x6F, 0x6D,
    0x62, 0x65, 0x72, 0x67, 0x20, 0x4C, 0x50, 0x31, 0x1D, 0x30,
    0x1B, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x14, 0x4D, 0x69,
    0x64, 0x64, 0x6C, 0x65, 0x77, 0x61, 0x72, 0x65, 0x20, 0x54,
    0x72, 0x61, 0x6E, 0x73, 0x70, 0x6F, 0x72, 0x74, 0x30, 0x59,
    0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02,
    0x01, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01,
    0x07, 0x03, 0x42, 0x00, 0x04, 0xD7, 0x4E, 0x0D, 0x3B, 0xD1,
    0xA3, 0x45, 0x01, 0x79, 0xE7, 0x88, 0x57, 0x2C, 0xB1, 0x89,
    0xE6, 0xEF, 0x49, 0xE3, 0x64, 0x10, 0xA6, 0xCD, 0x3F, 0x77,
    0x98, 0x2F, 0x7B, 0xED, 0xC2, 0x18, 0xFF, 0xEE, 0xAA, 0x62,
    0xCA, 0x05, 0x66, 0x24, 0x6D, 0xA0, 0x25, 0x10, 0x81, 0x3E,
    0x8D, 0x19, 0x2D, 0x6F, 0xD5, 0x7A, 0x40, 0x18, 0xC7, 0x5B,
    0xE3, 0xB6, 0x4C, 0x34, 0xF6, 0x9D, 0x65, 0x04, 0xAA, 0xA3,
    0x5D, 0x30, 0x5B, 0x30, 0x0C, 0x06, 0x03, 0x55, 0x1D, 0x13,
    0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xFF, 0x30, 0x0B, 0x06,
    0x03, 0x55, 0x1D, 0x0F, 0x04, 0x04, 0x03, 0x02, 0x01, 0x06,
    0x30, 0x1D, 0x06, 0x03, 0x55, 0x1D, 0x0E, 0x04, 0x16, 0x04,
    0x14, 0x2C, 0x27, 0xDB, 0x5A, 0x05, 0xC8, 0xA1, 0x49, 0xFB,
    0xC8, 0x1C, 0xFB, 0x40, 0x68, 0xC7, 0xA2, 0xBA, 0x42, 0xD8,
    0xDA, 0x30, 0x1F, 0x06, 0x03, 0x55, 0x1D, 0x23, 0x04, 0x18,
    0x30, 0x16, 0x80, 0x14, 0x2C, 0x27, 0xDB, 0x5A, 0x05, 0xC8,
    0xA1, 0x49, 0xFB, 0xC8, 0x1C, 0xFB, 0x40, 0x68, 0xC7, 0xA2,
    0xBA, 0x42, 0xD8, 0xDA, 0x30, 0x0A, 0x06, 0x08, 0x2A, 0x86,
    0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02, 0x03, 0x47, 0x00, 0x30,
    0x44, 0x02, 0x20, 0x73, 0xF0, 0xCC, 0xEB, 0x8C, 0xF2, 0xF0,
    0x81, 0x00, 0x1B, 0xBA, 0xFC, 0x66, 0x64, 0x6C, 0xBC, 0x2E,
    0xF5, 0x42, 0x26, 0xFB, 0x81, 0x54, 0xB2, 0x80, 0x87, 0xD5,
    0x8C, 0x66, 0x23, 0x8A, 0x53, 0x02, 0x20, 0x7F, 0xD0, 0x87,
    0x65, 0x65, 0x9B, 0xBA, 0x39, 0x93, 0x81, 0xAA, 0x05, 0x58,
    0x7B, 0x55, 0x13, 0xDA, 0x9E, 0x78, 0x82, 0xEB, 0x89, 0xD1,
    0xD1, 0x4D, 0xB1, 0x57, 0x82, 0x35, 0x4B, 0xAF, 0x9B
};
// clang-format on

}  // close namespace ntca
}  // close namespace BloombergLP
