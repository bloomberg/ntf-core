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
BSLS_IDENT_RCSID(ntcdns_protocol_t_cpp, "$Id$ $CSID$")

#include <ntcdns_protocol.h>

#include <ntci_log.h>
#include <ntsa_ipv4address.h>
#include <ntsa_ipv6address.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcdns {

// Provide tests for 'ntcdns::Protocol'.
//
// The payloads defined in this test driver were gathered by running a UDP
// proxy between a 'dig' DNS client and the Google public DNS server available
// at 8.8.8.8. First, run a UDP proxy "at" a particular address that proxies
// traffic "from" an address "to" another address (and the reverse).
//
//     $ ntf proxy udp --task-name my-dns-proxy
//                     --at 192.168.42.131:8053
//                     --from 127.0.0.1:8054
//                     --to 8.8.8.8:53
//
// In this example, the IP address of the host is 192.168.42.131 (note a
// loopback address cannot be chosen), and we've arbitrarily chosen port 8053
// as the port used by the proxy, and port 8054 as the port used by the
// DNS client. The proxy forwards the traffic it receives to Google's public
// DNS server at the standard DNS port. Next, run 'dig' to send a DNS request
// to the proxy, remembering to bind the UDP socket used by 'dig' to the
// expected port 8054 that we've chosen.
//
//     $ dig google.com @192.168.42.131 -4 -p 8053 -b127.0.0.1#8054 +noadflag
class ProtocolTest
{
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
    static void verifyWks();

    // Verify character strings are decoded within a limit.
    static void verifyCharacterStringLimit();

    // Verify TXT records.
    static void verifyTxt();

    // Verify malformed TXT records are rejected.
    static void verifyTxtMalformed();

    // Verify HINFO records.
    static void verifyHinfo();

    // Verify malformed HINFO records are rejected.
    static void verifyHinfoMalformed();

    // Verify malformed WKS records are rejected.
    static void verifyWksMalformed();

  private:
    // Load into the specified 'result' a response to a question for the
    // specified 'type' of "example.com" having the specified 'ancount'
    // answers, the first of which has the specified 'type', the specified
    // 'rdlength', and is followed by the specified 'payload' having the
    // specified 'payloadSize'. Note that 'payloadSize' may differ from
    // 'rdlength' and 'payload' may contain subsequent answers.
    static void buildResponse(bsl::vector<bsl::uint8_t>* result,
                              bsl::uint16_t              type,
                              bsl::uint16_t              ancount,
                              bsl::uint16_t              rdlength,
                              const bsl::uint8_t*        payload,
                              bsl::size_t                payloadSize);

    // Verify the specified 'message' survives an encode/decode round trip.
    static void verifyRoundTrip(const ntcdns::Message& message);
};

void ProtocolTest::buildResponse(bsl::vector<bsl::uint8_t>* result,
                                 bsl::uint16_t              type,
                                 bsl::uint16_t              ancount,
                                 bsl::uint16_t              rdlength,
                                 const bsl::uint8_t*        payload,
                                 bsl::size_t                payloadSize)
{
    const bsl::uint8_t typeHi = static_cast<bsl::uint8_t>(type >> 8);
    const bsl::uint8_t typeLo = static_cast<bsl::uint8_t>(type & 0xFF);

    // clang-format off
    const bsl::uint8_t PREFIX[] = {
        0x12, 0x34, 0x84, 0x00, 0x00, 0x01,
        static_cast<bsl::uint8_t>(ancount >> 8),
        static_cast<bsl::uint8_t>(ancount & 0xFF),
        0x00, 0x00, 0x00, 0x00,
        0x07, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65,
        0x03, 0x63, 0x6f, 0x6d, 0x00,
        typeHi, typeLo, 0x00, 0x01,
        0xc0, 0x0c, typeHi, typeLo, 0x00, 0x01,
        0x00, 0x00, 0x0e, 0x10,
        static_cast<bsl::uint8_t>(rdlength >> 8),
        static_cast<bsl::uint8_t>(rdlength & 0xFF)
    };
    // clang-format on

    result->assign(PREFIX, PREFIX + sizeof PREFIX);
    result->insert(result->end(), payload, payload + payloadSize);
}

void ProtocolTest::verifyRoundTrip(const ntcdns::Message& message)
{
    ntsa::Error error;

    bsl::vector<bsl::uint8_t> buffer(1024 * 64);

    ntcdns::MemoryEncoder encoder(&buffer[0], buffer.size());

    error = message.encode(&encoder);
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));

    ntcdns::MemoryDecoder decoder(&buffer[0], encoder.position());

    ntcdns::Message other(NTSCFG_TEST_ALLOCATOR);
    error = other.decode(&decoder);
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));

    NTSCFG_TEST_EQ(message, other);
}

NTSCFG_TEST_FUNCTION(ntcdns::ProtocolTest::verifyCase1)
{
    // Concern: Real request data.
    // Plan:
    // Command:
    // dig google.com @192.168.42.131 -4 -p 8053 -b127.0.0.1#8054
    //     +noadflag +noedns +nodnssec +additional

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    // clang-format off
    const bsl::uint8_t REQUEST[] = {
        0x33, 0x7b, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x06, 0x67, 0x6f, 0x6f,
        0x67, 0x6c, 0x65, 0x03, 0x63, 0x6f, 0x6d, 0x00,
        0x00, 0x01, 0x00, 0x01
    };
    // clang-format on

    ntcdns::Message message(NTSCFG_TEST_ALLOCATOR);

    {
        ntcdns::MemoryDecoder decoder(REQUEST, sizeof REQUEST);
        error = message.decode(&decoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));
    }

    NTCI_LOG_STREAM_DEBUG << "Message = " << message << NTCI_LOG_STREAM_END;

    NTSCFG_TEST_EQ(message.id(), 13179);

    NTSCFG_TEST_EQ(message.direction(), ntcdns::Direction::e_REQUEST);
    NTSCFG_TEST_EQ(message.operation(), ntcdns::Operation::e_STANDARD);
    NTSCFG_TEST_EQ(message.error(), ntcdns::Error::e_OK);

    NTSCFG_TEST_EQ(message.aa(), false);
    NTSCFG_TEST_EQ(message.tc(), false);
    NTSCFG_TEST_EQ(message.rd(), true);
    NTSCFG_TEST_EQ(message.ra(), false);
    NTSCFG_TEST_EQ(message.ad(), false);
    NTSCFG_TEST_EQ(message.cd(), false);

    NTSCFG_TEST_EQ(message.qdcount(), 1);
    NTSCFG_TEST_EQ(message.ancount(), 0);
    NTSCFG_TEST_EQ(message.nscount(), 0);
    NTSCFG_TEST_EQ(message.arcount(), 0);

    const ntcdns::Question& question = message.qd(0);

    NTSCFG_TEST_EQ(question.name(), "google.com");
    NTSCFG_TEST_EQ(question.type(), ntcdns::Type::e_A);
    NTSCFG_TEST_EQ(question.classification(),
                   ntcdns::Classification::e_INTERNET);

    {
        bsl::vector<bsl::uint8_t> buffer(1024 * 64);

        ntcdns::MemoryEncoder encoder(&buffer[0], buffer.size());

        error = message.encode(&encoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));

        bsl::size_t bufferSize = encoder.position();

        ntcdns::MemoryDecoder decoder(&buffer[0], bufferSize);

        ntcdns::Message other(NTSCFG_TEST_ALLOCATOR);
        error = other.decode(&decoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));

        NTSCFG_TEST_EQ(message, other);
    }
}

NTSCFG_TEST_FUNCTION(ntcdns::ProtocolTest::verifyCase2)
{
    // Concern: Real response data.
    // Plan:
    // Command:
    // dig google.com @192.168.42.131 -4 -p 8053 -b127.0.0.1#8054
    //     +noadflag +noedns +nodnssec +additional

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    // clang-format off
    const bsl::uint8_t RESPONSE[] = {
        0x33, 0x7b, 0x81, 0x80, 0x00, 0x01, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x06, 0x67, 0x6f, 0x6f,
        0x67, 0x6c, 0x65, 0x03, 0x63, 0x6f, 0x6d, 0x00,
        0x00, 0x01, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x01,
        0x00, 0x01, 0x00, 0x00, 0x00, 0x77, 0x00, 0x04,
        0xac, 0xd9, 0x06, 0xee
    };
    // clang-format on

    ntcdns::Message message(NTSCFG_TEST_ALLOCATOR);

    {
        ntcdns::MemoryDecoder decoder(RESPONSE, sizeof RESPONSE);
        error = message.decode(&decoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));
    }

    NTCI_LOG_STREAM_DEBUG << "Message = " << message << NTCI_LOG_STREAM_END;

    NTSCFG_TEST_EQ(message.id(), 13179);

    NTSCFG_TEST_EQ(message.direction(), ntcdns::Direction::e_RESPONSE);
    NTSCFG_TEST_EQ(message.operation(), ntcdns::Operation::e_STANDARD);
    NTSCFG_TEST_EQ(message.error(), ntcdns::Error::e_OK);

    NTSCFG_TEST_EQ(message.aa(), false);
    NTSCFG_TEST_EQ(message.tc(), false);
    NTSCFG_TEST_EQ(message.rd(), true);
    NTSCFG_TEST_EQ(message.ra(), true);
    NTSCFG_TEST_EQ(message.ad(), false);
    NTSCFG_TEST_EQ(message.cd(), false);

    NTSCFG_TEST_EQ(message.qdcount(), 1);
    NTSCFG_TEST_EQ(message.ancount(), 1);
    NTSCFG_TEST_EQ(message.nscount(), 0);
    NTSCFG_TEST_EQ(message.arcount(), 0);

    const ntcdns::Question& question = message.qd(0);

    NTSCFG_TEST_EQ(question.name(), "google.com");
    NTSCFG_TEST_EQ(question.type(), ntcdns::Type::e_A);
    NTSCFG_TEST_EQ(question.classification(),
                   ntcdns::Classification::e_INTERNET);

    const ntcdns::ResourceRecord& answer = message.an(0);

    NTSCFG_TEST_EQ(answer.name(), "google.com");
    NTSCFG_TEST_EQ(answer.type(), ntcdns::Type::e_A);
    NTSCFG_TEST_EQ(answer.classification(),
                   ntcdns::Classification::e_INTERNET);
    NTSCFG_TEST_EQ(answer.ttl(), 119);
    NTSCFG_TEST_EQ(answer.rdata().isIpv4Value(), true);
    NTSCFG_TEST_EQ(ntsa::Ipv4Address(answer.rdata().ipv4().address()),
                   ntsa::Ipv4Address("172.217.6.238"));

    {
        bsl::vector<bsl::uint8_t> buffer(1024 * 64);

        ntcdns::MemoryEncoder encoder(&buffer[0], buffer.size());

        error = message.encode(&encoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));

        bsl::size_t bufferSize = encoder.position();

        ntcdns::MemoryDecoder decoder(&buffer[0], bufferSize);

        ntcdns::Message other(NTSCFG_TEST_ALLOCATOR);
        error = other.decode(&decoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));

        NTSCFG_TEST_EQ(message, other);
    }
}

NTSCFG_TEST_FUNCTION(ntcdns::ProtocolTest::verifyCase3)
{
    // Concern: Real request data (trace) for nameservers with EDNS resource
    // records.
    // Plan:
    // Command:
    // dig google.com @192.168.42.131 -4 -p 8053 -b127.0.0.1#8054
    //     +noadflag +noedns +nodnssec +additional +trace

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    // clang-format off
    const bsl::uint8_t REQUEST[] = {
        0x5d, 0xeb, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0x00,
        0x01, 0x00, 0x00, 0x29, 0x10, 0x00, 0x00, 0x00,
        0x80, 0x00, 0x00, 0x0c, 0x00, 0x0a, 0x00, 0x08,
        0x48, 0x1c, 0x38, 0xa7, 0xee, 0x08, 0xfe, 0x69
    };
    // clang-format on

    ntcdns::Message message(NTSCFG_TEST_ALLOCATOR);

    {
        ntcdns::MemoryDecoder decoder(REQUEST, sizeof REQUEST);
        error = message.decode(&decoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));
    }

    NTCI_LOG_STREAM_DEBUG << "Message = " << message << NTCI_LOG_STREAM_END;

    NTSCFG_TEST_EQ(message.id(), 24043);

    NTSCFG_TEST_EQ(message.direction(), ntcdns::Direction::e_REQUEST);
    NTSCFG_TEST_EQ(message.operation(), ntcdns::Operation::e_STANDARD);
    NTSCFG_TEST_EQ(message.error(), ntcdns::Error::e_OK);

    NTSCFG_TEST_EQ(message.aa(), false);
    NTSCFG_TEST_EQ(message.tc(), false);
    NTSCFG_TEST_EQ(message.rd(), true);
    NTSCFG_TEST_EQ(message.ra(), false);
    NTSCFG_TEST_EQ(message.ad(), false);
    NTSCFG_TEST_EQ(message.cd(), false);

    NTSCFG_TEST_EQ(message.qdcount(), 1);
    NTSCFG_TEST_EQ(message.ancount(), 0);
    NTSCFG_TEST_EQ(message.nscount(), 0);
    NTSCFG_TEST_EQ(message.arcount(), 1);

    const ntcdns::Question& question = message.qd(0);

    NTSCFG_TEST_EQ(question.name(), "");
    NTSCFG_TEST_EQ(question.type(), ntcdns::Type::e_NS);
    NTSCFG_TEST_EQ(question.classification(),
                   ntcdns::Classification::e_INTERNET);

    const ntcdns::ResourceRecord& additional = message.ar(0);

    NTSCFG_TEST_EQ(additional.name(), "");
    NTSCFG_TEST_EQ(additional.type(), ntcdns::Type::e_OPT);
    NTSCFG_TEST_EQ(additional.payloadSize(), 4096);
    NTSCFG_TEST_EQ(additional.flags(), 32768);
    NTSCFG_TEST_EQ(additional.rdata().isRawValue(), true);
    // TODO: Compare expected EDNS OPT pseudo-record type content with the
    // value found.

    {
        bsl::vector<bsl::uint8_t> buffer(1024 * 64);

        ntcdns::MemoryEncoder encoder(&buffer[0], buffer.size());

        error = message.encode(&encoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));

        bsl::size_t bufferSize = encoder.position();

        ntcdns::MemoryDecoder decoder(&buffer[0], bufferSize);

        ntcdns::Message other(NTSCFG_TEST_ALLOCATOR);
        error = other.decode(&decoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));

        NTSCFG_TEST_EQ(message, other);
    }
}

NTSCFG_TEST_FUNCTION(ntcdns::ProtocolTest::verifyCase4)
{
    // Concern: Real response data (trace) for nameservers with EDNS resource
    // records.
    // Plan:
    // Command:
    // dig google.com @192.168.42.131 -4 -p 8053 -b127.0.0.1#8054
    //     +noadflag +noedns +nodnssec +additional +trace

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    // clang-format off
    const bsl::uint8_t RESPONSE[] = {
        0x5d, 0xeb, 0x81, 0xa0, 0x00, 0x01, 0x00, 0x0e,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0x00,
        0x01, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00,
        0x33, 0xa1, 0x00, 0x14, 0x01, 0x61, 0x0c, 0x72,
        0x6f, 0x6f, 0x74, 0x2d, 0x73, 0x65, 0x72, 0x76,
        0x65, 0x72, 0x73, 0x03, 0x6e, 0x65, 0x74, 0x00,
        0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x33,
        0xa1, 0x00, 0x04, 0x01, 0x62, 0xc0, 0x1e, 0x00,
        0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x33, 0xa1,
        0x00, 0x04, 0x01, 0x63, 0xc0, 0x1e, 0x00, 0x00,
        0x02, 0x00, 0x01, 0x00, 0x00, 0x33, 0xa1, 0x00,
        0x04, 0x01, 0x64, 0xc0, 0x1e, 0x00, 0x00, 0x02,
        0x00, 0x01, 0x00, 0x00, 0x33, 0xa1, 0x00, 0x04,
        0x01, 0x65, 0xc0, 0x1e, 0x00, 0x00, 0x02, 0x00,
        0x01, 0x00, 0x00, 0x33, 0xa1, 0x00, 0x04, 0x01,
        0x66, 0xc0, 0x1e, 0x00, 0x00, 0x02, 0x00, 0x01,
        0x00, 0x00, 0x33, 0xa1, 0x00, 0x04, 0x01, 0x67,
        0xc0, 0x1e, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00,
        0x00, 0x33, 0xa1, 0x00, 0x04, 0x01, 0x68, 0xc0,
        0x1e, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00,
        0x33, 0xa1, 0x00, 0x04, 0x01, 0x69, 0xc0, 0x1e,
        0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x33,
        0xa1, 0x00, 0x04, 0x01, 0x6a, 0xc0, 0x1e, 0x00,
        0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x33, 0xa1,
        0x00, 0x04, 0x01, 0x6b, 0xc0, 0x1e, 0x00, 0x00,
        0x02, 0x00, 0x01, 0x00, 0x00, 0x33, 0xa1, 0x00,
        0x04, 0x01, 0x6c, 0xc0, 0x1e, 0x00, 0x00, 0x02,
        0x00, 0x01, 0x00, 0x00, 0x33, 0xa1, 0x00, 0x04,
        0x01, 0x6d, 0xc0, 0x1e, 0x00, 0x00, 0x2e, 0x00,
        0x01, 0x00, 0x00, 0x33, 0xa1, 0x01, 0x13, 0x00,
        0x02, 0x08, 0x00, 0x00, 0x07, 0xe9, 0x00, 0x60,
        0xcf, 0xac, 0x50, 0x60, 0xbe, 0x7a, 0xc0, 0x39,
        0x27, 0x00, 0x11, 0x58, 0x22, 0xab, 0x56, 0xf7,
        0x05, 0xd7, 0x8e, 0xcf, 0xd6, 0x25, 0x52, 0x25,
        0x72, 0xd4, 0xa5, 0xe1, 0x3a, 0xd9, 0xdd, 0x84,
        0x3f, 0x5b, 0xf2, 0x9b, 0xc2, 0xeb, 0x10, 0x6b,
        0x3c, 0x8d, 0x3e, 0x79, 0x6d, 0x6a, 0x75, 0x49,
        0x62, 0x8b, 0xac, 0x0b, 0x19, 0xb6, 0x78, 0x64,
        0xae, 0x68, 0x7c, 0xbb, 0x80, 0x44, 0x6d, 0x65,
        0x19, 0xaf, 0x42, 0x00, 0x4d, 0xe1, 0xcf, 0xf2,
        0xd7, 0x9b, 0xcd, 0xde, 0x20, 0x46, 0x42, 0x7a,
        0x6f, 0xc4, 0x74, 0x7f, 0x3c, 0x4b, 0x30, 0xc7,
        0x4b, 0xee, 0x3c, 0xf8, 0xfa, 0x81, 0x9a, 0x35,
        0xa5, 0xf4, 0x63, 0x50, 0x21, 0x93, 0x81, 0x64,
        0xfe, 0x55, 0xc4, 0x5d, 0x11, 0xf6, 0x15, 0xd9,
        0x8f, 0x06, 0x4b, 0xe8, 0x3e, 0x98, 0x54, 0xec,
        0x5f, 0x82, 0x37, 0x2a, 0x30, 0xb1, 0x82, 0x8b,
        0x96, 0x26, 0x25, 0xc0, 0x13, 0x6f, 0xf6, 0xa2,
        0xe3, 0x04, 0x22, 0x18, 0x0d, 0xb9, 0x90, 0x16,
        0xbe, 0xa5, 0x43, 0xb5, 0xea, 0x8c, 0xa4, 0x90,
        0x1b, 0x2d, 0x96, 0x68, 0xd3, 0x7a, 0xf8, 0x40,
        0x45, 0xff, 0xc2, 0x95, 0x90, 0x47, 0x5a, 0xb7,
        0x07, 0xf4, 0xef, 0x24, 0x5e, 0x9e, 0x08, 0x2a,
        0x3a, 0x57, 0x05, 0xc3, 0xcf, 0x55, 0x41, 0xa5,
        0x19, 0x83, 0x5a, 0xe6, 0x48, 0xc5, 0x6e, 0xa0,
        0x14, 0xb7, 0x31, 0x0f, 0x6e, 0xe1, 0x6f, 0x59,
        0x6a, 0x88, 0xec, 0x04, 0x57, 0xbb, 0x0c, 0x04,
        0x81, 0x7b, 0x4a, 0x04, 0x7a, 0xb0, 0xc3, 0xac,
        0x18, 0xbb, 0x80, 0x3a, 0x4f, 0xda, 0x3d, 0x41,
        0xd6, 0x38, 0xab, 0xd6, 0x99, 0xd1, 0x65, 0xc1,
        0xc3, 0x1f, 0xf6, 0x1b, 0xb9, 0xbc, 0x85, 0x3c,
        0xb0, 0xfe, 0xdd, 0x3b, 0xc7, 0xd7, 0x81, 0xb3,
        0x70, 0x53, 0x49, 0x8b, 0xb5, 0xe2, 0x48, 0xb8,
        0x60, 0xb4, 0xe4, 0xcf, 0x8a, 0x14, 0x2d, 0x4f,
        0xb8, 0x24, 0x00, 0x00, 0x29, 0x02, 0x00, 0x00,
        0x00, 0x80, 0x00, 0x00, 0x00
    };
    // clang-format on

    ntcdns::Message message(NTSCFG_TEST_ALLOCATOR);

    {
        ntcdns::MemoryDecoder decoder(RESPONSE, sizeof RESPONSE);
        error = message.decode(&decoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));
    }

    NTCI_LOG_STREAM_DEBUG << "Message = " << message << NTCI_LOG_STREAM_END;

    NTSCFG_TEST_EQ(message.id(), 24043);

    NTSCFG_TEST_EQ(message.direction(), ntcdns::Direction::e_RESPONSE);
    NTSCFG_TEST_EQ(message.operation(), ntcdns::Operation::e_STANDARD);
    NTSCFG_TEST_EQ(message.error(), ntcdns::Error::e_OK);

    NTSCFG_TEST_EQ(message.aa(), false);
    NTSCFG_TEST_EQ(message.tc(), false);
    NTSCFG_TEST_EQ(message.rd(), true);
    NTSCFG_TEST_EQ(message.ra(), true);
    NTSCFG_TEST_EQ(message.ad(), false);
    NTSCFG_TEST_EQ(message.cd(), false);

    NTSCFG_TEST_EQ(message.qdcount(), 1);
    NTSCFG_TEST_EQ(message.ancount(), 14);
    NTSCFG_TEST_EQ(message.nscount(), 0);
    NTSCFG_TEST_EQ(message.arcount(), 1);

    const ntcdns::Question& question = message.qd(0);

    NTSCFG_TEST_EQ(question.name(), "");
    NTSCFG_TEST_EQ(question.type(), ntcdns::Type::e_NS);
    NTSCFG_TEST_EQ(question.classification(),
                   ntcdns::Classification::e_INTERNET);

    for (bsl::size_t i = 0; i < 13; ++i) {
        const ntcdns::ResourceRecord& answer = message.an(i);

        bsl::string nsdname;
        {
            bsl::stringstream ss;
            ss << static_cast<char>('a' + i) << ".root-servers.net";
            nsdname = ss.str();
        }

        NTSCFG_TEST_EQ(answer.name(), "");
        NTSCFG_TEST_EQ(answer.type(), ntcdns::Type::e_NS);
        NTSCFG_TEST_EQ(answer.classification(),
                       ntcdns::Classification::e_INTERNET);
        NTSCFG_TEST_EQ(answer.ttl(), 13217);
        NTSCFG_TEST_EQ(answer.rdata().isNameServerValue(), true);
        NTSCFG_TEST_EQ(answer.rdata().nameServer().nsdname(), nsdname);
    }

    {
        const ntcdns::ResourceRecord& answer = message.an(13);

        NTSCFG_TEST_EQ(answer.name(), "");
        NTSCFG_TEST_EQ(answer.type(), ntcdns::Type::e_RRSIG);
        NTSCFG_TEST_EQ(answer.classification(),
                       ntcdns::Classification::e_INTERNET);
        NTSCFG_TEST_EQ(answer.ttl(), 13217);
        NTSCFG_TEST_EQ(answer.rdata().isRawValue(), true);
        // TODO: Compare expected RRSIG resource record type content with
        // the value found.
    }

    const ntcdns::ResourceRecord& additional = message.ar(0);

    NTSCFG_TEST_EQ(additional.name(), "");
    NTSCFG_TEST_EQ(additional.type(), ntcdns::Type::e_OPT);
    NTSCFG_TEST_EQ(additional.payloadSize(), 512);
    NTSCFG_TEST_EQ(additional.flags(), 32768);
    NTSCFG_TEST_EQ(additional.rdata().isUndefinedValue(), true);

    {
        bsl::vector<bsl::uint8_t> buffer(1024 * 64);

        ntcdns::MemoryEncoder encoder(&buffer[0], buffer.size());

        error = message.encode(&encoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));

        bsl::size_t bufferSize = encoder.position();

        ntcdns::MemoryDecoder decoder(&buffer[0], bufferSize);

        ntcdns::Message other(NTSCFG_TEST_ALLOCATOR);
        error = other.decode(&decoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));

        NTSCFG_TEST_EQ(message, other);
    }
}

NTSCFG_TEST_FUNCTION(ntcdns::ProtocolTest::verifyWks)
{
    // Concern: Well-known Services (WKS) records.

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    // clang-format off
    const bsl::uint8_t RESPONSE[57] = {
        0x12, 0x34, 0x84, 0x00, 0x00, 0x01, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x07, 0x65, 0x78, 0x61,
        0x6d, 0x70, 0x6c, 0x65, 0x03, 0x63, 0x6f, 0x6d,
        0x00, 0x00, 0x0b, 0x00, 0x01, 0xc0, 0x0c, 0x00,
        0x0b, 0x00, 0x01, 0x00, 0x00, 0x0e, 0x10, 0x00,
        0x10, 0xc0, 0xa8, 0x01, 0x01, 0x06, 0x00, 0x00,
        0x02, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x80
    };
    // clang-format on

    NTCI_LOG_STREAM_DEBUG << "I:\n"
                          << bdlb::PrintStringHexDumper(
                                reinterpret_cast<const char*>(RESPONSE),
                                static_cast<int>(sizeof RESPONSE))
                          << NTCI_LOG_STREAM_END;

    ntcdns::Message message(NTSCFG_TEST_ALLOCATOR);

    {
        ntcdns::MemoryDecoder decoder(RESPONSE, sizeof RESPONSE);
        error = message.decode(&decoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));
    }

    NTCI_LOG_STREAM_DEBUG << "Message = " << message << NTCI_LOG_STREAM_END;

    NTSCFG_TEST_EQ(message.id(), 4660);

    NTSCFG_TEST_EQ(message.direction(), ntcdns::Direction::e_RESPONSE);
    NTSCFG_TEST_EQ(message.operation(), ntcdns::Operation::e_STANDARD);
    NTSCFG_TEST_EQ(message.error(), ntcdns::Error::e_OK);

    NTSCFG_TEST_EQ(message.aa(), true);
    NTSCFG_TEST_EQ(message.tc(), false);
    NTSCFG_TEST_EQ(message.rd(), false);
    NTSCFG_TEST_EQ(message.ra(), false);
    NTSCFG_TEST_EQ(message.ad(), false);
    NTSCFG_TEST_EQ(message.cd(), false);

    NTSCFG_TEST_EQ(message.qdcount(), 1);
    NTSCFG_TEST_EQ(message.ancount(), 1);
    NTSCFG_TEST_EQ(message.nscount(), 0);
    NTSCFG_TEST_EQ(message.arcount(), 0);

    const ntcdns::Question& question = message.qd(0);

    NTSCFG_TEST_EQ(question.name(), "example.com");
    NTSCFG_TEST_EQ(question.type(), ntcdns::Type::e_WKS);
    NTSCFG_TEST_EQ(question.classification(),
                   ntcdns::Classification::e_INTERNET);

    const ntcdns::ResourceRecord& answer = message.an(0);

    NTSCFG_TEST_EQ(answer.name(), "example.com");
    NTSCFG_TEST_EQ(answer.type(), ntcdns::Type::e_WKS);
    NTSCFG_TEST_EQ(answer.classification(),
                   ntcdns::Classification::e_INTERNET);
    NTSCFG_TEST_EQ(answer.ttl(), 3600);
    NTSCFG_TEST_EQ(answer.rdata().isWellKnownServiceValue(), true);

    const ntcdns::ResourceRecordDataWks& wks =
        answer.rdata().wellKnownService();

    NTSCFG_TEST_EQ(wks.port().size(), 3);
    NTSCFG_TEST_EQ(wks.port()[0], 22);
    NTSCFG_TEST_EQ(wks.port()[1], 25);
    NTSCFG_TEST_EQ(wks.port()[2], 80);

    {
        bsl::vector<bsl::uint8_t> buffer(1024 * 64);

        ntcdns::MemoryEncoder encoder(&buffer[0], buffer.size());

        error = message.encode(&encoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));

        bsl::size_t bufferSize = encoder.position();

        NTCI_LOG_STREAM_DEBUG << "O:\n"
                              << bdlb::PrintStringHexDumper(
                                    reinterpret_cast<const char*>(&buffer[0]),
                                    static_cast<int>(bufferSize))
                              << NTCI_LOG_STREAM_END;

        ntcdns::MemoryDecoder decoder(&buffer[0], bufferSize);

        ntcdns::Message other(NTSCFG_TEST_ALLOCATOR);
        error = other.decode(&decoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));

        NTSCFG_TEST_EQ(message, other);
    }
}

NTSCFG_TEST_FUNCTION(ntcdns::ProtocolTest::verifyCharacterStringLimit)
{
    // Concern: A character string whose declared length exceeds the limit
    // is rejected without advancing the decoder.

    ntsa::Error error;

    const bsl::uint8_t DATA[] = {0x05, 0x68, 0x65, 0x6c, 0x6c, 0x6f};

    {
        ntcdns::MemoryDecoder decoder(DATA, sizeof DATA);

        bsl::string value(NTSCFG_TEST_ALLOCATOR);
        error = decoder.decodeCharacterString(&value, 3);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));
        NTSCFG_TEST_EQ(decoder.position(), 0);
        NTSCFG_TEST_TRUE(value.empty());

        error = decoder.decodeCharacterString(&value, 0);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));
        NTSCFG_TEST_EQ(decoder.position(), 0);

        error = decoder.decodeCharacterString(&value, sizeof DATA);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));
        NTSCFG_TEST_EQ(decoder.position(), sizeof DATA);
        NTSCFG_TEST_EQ(value, "hello");
    }

    {
        // A limit greater than the buffer is clamped to the buffer.

        ntcdns::MemoryDecoder decoder(DATA, 2);

        bsl::string value(NTSCFG_TEST_ALLOCATOR);
        error = decoder.decodeCharacterString(&value, 100);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));
        NTSCFG_TEST_EQ(decoder.position(), 0);
    }
}

NTSCFG_TEST_FUNCTION(ntcdns::ProtocolTest::verifyTxt)
{
    // Concern: TXT records containing multiple character strings, including
    // an empty character string.

    ntsa::Error error;

    // clang-format off
    const bsl::uint8_t RDATA[] = {
        0x05, 0x68, 0x65, 0x6c, 0x6c, 0x6f,
        0x00,
        0x05, 0x77, 0x6f, 0x72, 0x6c, 0x64
    };
    // clang-format on

    bsl::vector<bsl::uint8_t> response(NTSCFG_TEST_ALLOCATOR);
    ProtocolTest::buildResponse(&response,
                                ntcdns::Type::e_TXT,
                                1,
                                sizeof RDATA,
                                RDATA,
                                sizeof RDATA);

    ntcdns::Message message(NTSCFG_TEST_ALLOCATOR);

    {
        ntcdns::MemoryDecoder decoder(&response[0], response.size());
        error = message.decode(&decoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));
    }

    NTSCFG_TEST_EQ(message.ancount(), 1);

    const ntcdns::ResourceRecord& answer = message.an(0);

    NTSCFG_TEST_EQ(answer.type(), ntcdns::Type::e_TXT);
    NTSCFG_TEST_TRUE(answer.rdata().isTextValue());

    const ntcdns::ResourceRecordDataTxt& txt = answer.rdata().text();

    NTSCFG_TEST_EQ(txt.text().size(), 3);
    NTSCFG_TEST_EQ(txt.text()[0], "hello");
    NTSCFG_TEST_EQ(txt.text()[1], "");
    NTSCFG_TEST_EQ(txt.text()[2], "world");

    ProtocolTest::verifyRoundTrip(message);
}

NTSCFG_TEST_FUNCTION(ntcdns::ProtocolTest::verifyTxtMalformed)
{
    // Concern: TXT records whose character strings are inconsistent with
    // the declared RDATA length are rejected.

    ntsa::Error error;

    {
        // A character string that extends past the RDATA into a subsequent,
        // well-formed answer.

        // clang-format off
        const bsl::uint8_t PAYLOAD[] = {
            0x05, 0x61, 0x62, 0x63,
            0xc0, 0x0c, 0x00, 0x10, 0x00, 0x01,
            0x00, 0x00, 0x0e, 0x10, 0x00, 0x02,
            0x01, 0x78
        };
        // clang-format on

        bsl::vector<bsl::uint8_t> response(NTSCFG_TEST_ALLOCATOR);
        ProtocolTest::buildResponse(&response,
                                    ntcdns::Type::e_TXT,
                                    2,
                                    4,
                                    PAYLOAD,
                                    sizeof PAYLOAD);

        {
            ntcdns::MemoryDecoder decoder(&response[0], response.size());
            ntcdns::Message       message(NTSCFG_TEST_ALLOCATOR);
            error = message.decode(&decoder);
            NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));
        }

        {
            // The decoder must not advance past the RDATA of the first
            // answer, which begins immediately after the response prefix.

            const bsl::size_t answerOffset = 29;
            const bsl::size_t rdataOffset  = response.size() - sizeof PAYLOAD;

            ntcdns::MemoryDecoder decoder(&response[0], response.size());
            error = decoder.seek(answerOffset);
            NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));

            ntcdns::ResourceRecord answer(NTSCFG_TEST_ALLOCATOR);
            error = answer.decode(&decoder);
            NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));
            NTSCFG_TEST_EQ(decoder.position(), rdataOffset);
        }
    }

    {
        // The declared RDATA length extends past the end of the message.

        const bsl::uint8_t PAYLOAD[] = {0x05, 0x68, 0x65, 0x6c, 0x6c, 0x6f};

        bsl::vector<bsl::uint8_t> response(NTSCFG_TEST_ALLOCATOR);
        ProtocolTest::buildResponse(&response,
                                    ntcdns::Type::e_TXT,
                                    1,
                                    32,
                                    PAYLOAD,
                                    sizeof PAYLOAD);

        ntcdns::MemoryDecoder decoder(&response[0], response.size());
        ntcdns::Message       message(NTSCFG_TEST_ALLOCATOR);
        error = message.decode(&decoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));
    }

    {
        // The declared RDATA length ends within a character string.

        const bsl::uint8_t PAYLOAD[] = {0x05, 0x68, 0x65, 0x6c, 0x6c, 0x6f};

        bsl::vector<bsl::uint8_t> response(NTSCFG_TEST_ALLOCATOR);
        ProtocolTest::buildResponse(&response,
                                    ntcdns::Type::e_TXT,
                                    1,
                                    2,
                                    PAYLOAD,
                                    sizeof PAYLOAD);

        ntcdns::MemoryDecoder decoder(&response[0], response.size());
        ntcdns::Message       message(NTSCFG_TEST_ALLOCATOR);
        error = message.decode(&decoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));
    }
}

NTSCFG_TEST_FUNCTION(ntcdns::ProtocolTest::verifyHinfo)
{
    // Concern: HINFO records.

    ntsa::Error error;

    // clang-format off
    const bsl::uint8_t RDATA[] = {
        0x03, 0x78, 0x38, 0x36,
        0x05, 0x4c, 0x69, 0x6e, 0x75, 0x78
    };
    // clang-format on

    bsl::vector<bsl::uint8_t> response(NTSCFG_TEST_ALLOCATOR);
    ProtocolTest::buildResponse(&response,
                                ntcdns::Type::e_HINFO,
                                1,
                                sizeof RDATA,
                                RDATA,
                                sizeof RDATA);

    ntcdns::Message message(NTSCFG_TEST_ALLOCATOR);

    {
        ntcdns::MemoryDecoder decoder(&response[0], response.size());
        error = message.decode(&decoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_OK));
    }

    const ntcdns::ResourceRecord& answer = message.an(0);

    NTSCFG_TEST_EQ(answer.type(), ntcdns::Type::e_HINFO);
    NTSCFG_TEST_TRUE(answer.rdata().isHostInfoValue());
    NTSCFG_TEST_EQ(answer.rdata().hostInfo().cpu(), "x86");
    NTSCFG_TEST_EQ(answer.rdata().hostInfo().os(), "Linux");

    ProtocolTest::verifyRoundTrip(message);
}

NTSCFG_TEST_FUNCTION(ntcdns::ProtocolTest::verifyHinfoMalformed)
{
    // Concern: HINFO records whose character strings are inconsistent with
    // the declared RDATA length are rejected.

    ntsa::Error error;

    // clang-format off
    const bsl::uint8_t PAYLOAD[] = {
        0x03, 0x78, 0x38, 0x36,
        0x05, 0x4c, 0x69, 0x6e, 0x75, 0x78
    };
    // clang-format on

    // The declared RDATA length ends within the CPU, after the CPU, and
    // within the OS, respectively.

    const bsl::uint16_t RDLENGTH[] = {2, 4, 7};

    for (bsl::size_t i = 0; i < sizeof RDLENGTH / sizeof RDLENGTH[0]; ++i) {
        bsl::vector<bsl::uint8_t> response(NTSCFG_TEST_ALLOCATOR);
        ProtocolTest::buildResponse(&response,
                                    ntcdns::Type::e_HINFO,
                                    1,
                                    RDLENGTH[i],
                                    PAYLOAD,
                                    sizeof PAYLOAD);

        ntcdns::MemoryDecoder decoder(&response[0], response.size());
        ntcdns::Message       message(NTSCFG_TEST_ALLOCATOR);
        error = message.decode(&decoder);
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));
    }
}

NTSCFG_TEST_FUNCTION(ntcdns::ProtocolTest::verifyWksMalformed)
{
    // Concern: WKS records whose declared RDATA length is less than the size
    // of the address and protocol are rejected.

    ntsa::Error error;

    const bsl::uint8_t PAYLOAD[] = {0xc0, 0xa8, 0x01, 0x01, 0x06, 0x00, 0x00};

    bsl::vector<bsl::uint8_t> response(NTSCFG_TEST_ALLOCATOR);
    ProtocolTest::buildResponse(&response,
                                ntcdns::Type::e_WKS,
                                1,
                                3,
                                PAYLOAD,
                                sizeof PAYLOAD);

    ntcdns::MemoryDecoder decoder(&response[0], response.size());
    ntcdns::Message       message(NTSCFG_TEST_ALLOCATOR);
    error = message.decode(&decoder);
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));
}

}  // close namespace ntcdns
}  // close namespace BloombergLP
