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
BSLS_IDENT_RCSID(ntsa_endpoint_t_cpp, "$Id$ $CSID$")

#include <ntsa_endpoint.h>

#include <balber_berdecoder.h>
#include <balber_berencoder.h>
#include <baljsn_decoder.h>
#include <baljsn_encoder.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Endpoint'.
class EndpointTest
{
  public:
    // Test value semantics: type traits.
    static void verifyTypeTraits();

    // Test value semantics:default constructor.
    static void verifyDefaultConstructor();

    // Test value semantics: move constructor.
    static void verifyMoveConstructor();

    // Test value semantics: copy constructor.
    static void verifyCopyConstructor();

    // Test value semantics: overload constructor.
    static void verifyOverloadConstructor();

    // Test value semantics: copy assignment.
    static void verifyCopyAssignment();

    // Test value semantics: move assignment.
    static void verifyMoveAssignment();

    // Test value semantics: overload assignment.
    static void verifyOverloadAssignment();

    // Test value semantics: resetting.
    static void verifyReset();

    // Test parsing: IPv4 address.
    static void verifyParsingIpv4();

    // Test parsing: IPv6 address.
    static void verifyParsingIpv6();

    // Test parsing: local name.
    static void verifyParsingLocalName();

    // Test generation.
    static void verifyGeneration();

    // Test hashing.
    static void verifyHashing();

    // Test comparison. In particular, comparison must yield consistent results
    // across different CPUs with different endianness.
    static void verifyComparison();

    // Verify encoding/decoding BER.
    static void verifyCodecBer();

    // Verify encoding/decoding JSON.
    static void verifyCodecJson();
};

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyTypeTraits)
{
    const bool isBitwiseInitializable =
        NTSCFG_TYPE_CHECK_BITWISE_INITIALIZABLE(ntsa::Endpoint);

    NTSCFG_TEST_FALSE(isBitwiseInitializable);

    const bool isBitwiseMovable =
        NTSCFG_TYPE_CHECK_BITWISE_MOVABLE(ntsa::Endpoint);

    NTSCFG_TEST_TRUE(isBitwiseMovable);

    const bool isBitwiseCopyable =
        NTSCFG_TYPE_CHECK_BITWISE_COPYABLE(ntsa::Endpoint);

    NTSCFG_TEST_TRUE(isBitwiseCopyable);

    const bool isBitwiseComparable =
        NTSCFG_TYPE_CHECK_BITWISE_COMPARABLE(ntsa::Endpoint);

    NTSCFG_TEST_FALSE(isBitwiseComparable);

    const bool isAllocatorAware =
        NTSCFG_TYPE_CHECK_ALLOCATOR_AWARE(ntsa::Endpoint);

    NTSCFG_TEST_FALSE(isAllocatorAware);
}

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyDefaultConstructor)
{
    ntsa::Endpoint u;

    NTSCFG_TEST_TRUE(u.isUndefined());
}

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyMoveConstructor)
{
    ntsa::Endpoint u(ntsa::Ipv4Address::loopback(), 12345);

    NTSCFG_TEST_TRUE(u.isIp());
    NTSCFG_TEST_TRUE(u.ip().host().isV4());
    NTSCFG_TEST_EQ(u.ip().host().v4(), ntsa::Ipv4Address::loopback());
    NTSCFG_TEST_EQ(u.ip().port(), 12345);

    ntsa::Endpoint v(NTSCFG_MOVE(u));

    NTSCFG_TEST_TRUE(v.isIp());
    NTSCFG_TEST_TRUE(v.ip().host().isV4());
    NTSCFG_TEST_EQ(v.ip().host().v4(), ntsa::Ipv4Address::loopback());
    NTSCFG_TEST_EQ(v.ip().port(), 12345);

#if NTSCFG_MOVE_RESET_ENABLED
    NTSCFG_TEST_TRUE(u.isUndefined());
#endif
}

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyCopyConstructor)
{
    ntsa::Endpoint u(ntsa::Ipv4Address::loopback(), 12345);

    NTSCFG_TEST_TRUE(u.isIp());
    NTSCFG_TEST_TRUE(u.ip().host().isV4());
    NTSCFG_TEST_EQ(u.ip().host().v4(), ntsa::Ipv4Address::loopback());
    NTSCFG_TEST_EQ(u.ip().port(), 12345);

    ntsa::Endpoint v(u);

    NTSCFG_TEST_TRUE(v.isIp());
    NTSCFG_TEST_TRUE(v.ip().host().isV4());
    NTSCFG_TEST_EQ(v.ip().host().v4(), ntsa::Ipv4Address::loopback());
    NTSCFG_TEST_EQ(v.ip().port(), 12345);
}

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyOverloadConstructor)
{
    {
        ntsa::Endpoint u(ntsa::Ipv4Address::loopback(), 12345);

        NTSCFG_TEST_TRUE(u.isIp());
        NTSCFG_TEST_TRUE(u.ip().host().isV4());
        NTSCFG_TEST_EQ(u.ip().host().v4(), ntsa::Ipv4Address::loopback());
        NTSCFG_TEST_EQ(u.ip().port(), 12345);
    }

    {
        ntsa::Endpoint u(ntsa::Ipv6Address::loopback(), 12345);

        NTSCFG_TEST_TRUE(u.isIp());
        NTSCFG_TEST_TRUE(u.ip().host().isV6());
        NTSCFG_TEST_EQ(u.ip().host().v6(), ntsa::Ipv6Address::loopback());
        NTSCFG_TEST_EQ(u.ip().port(), 12345);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyCopyAssignment)
{
    ntsa::Endpoint u(ntsa::Ipv4Address::loopback(), 12345);

    NTSCFG_TEST_TRUE(u.isIp());
    NTSCFG_TEST_TRUE(u.ip().host().isV4());
    NTSCFG_TEST_EQ(u.ip().host().v4(), ntsa::Ipv4Address::loopback());
    NTSCFG_TEST_EQ(u.ip().port(), 12345);

    ntsa::Endpoint v;

    NTSCFG_TEST_TRUE(v.isUndefined());

    v = u;

    NTSCFG_TEST_TRUE(v.isIp());
    NTSCFG_TEST_TRUE(v.ip().host().isV4());
    NTSCFG_TEST_EQ(v.ip().host().v4(), ntsa::Ipv4Address::loopback());
    NTSCFG_TEST_EQ(v.ip().port(), 12345);
}

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyMoveAssignment)
{
    ntsa::Endpoint u(ntsa::Ipv4Address::loopback(), 12345);

    NTSCFG_TEST_TRUE(u.isIp());
    NTSCFG_TEST_TRUE(u.ip().host().isV4());
    NTSCFG_TEST_EQ(u.ip().host().v4(), ntsa::Ipv4Address::loopback());
    NTSCFG_TEST_EQ(u.ip().port(), 12345);

    ntsa::Endpoint v;

    NTSCFG_TEST_TRUE(v.isUndefined());

    v = NTSCFG_MOVE(u);

    NTSCFG_TEST_TRUE(v.isIp());
    NTSCFG_TEST_TRUE(v.ip().host().isV4());
    NTSCFG_TEST_EQ(v.ip().host().v4(), ntsa::Ipv4Address::loopback());
    NTSCFG_TEST_EQ(v.ip().port(), 12345);

#if NTSCFG_MOVE_RESET_ENABLED
    NTSCFG_TEST_TRUE(u.isUndefined());
#endif
}

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyOverloadAssignment)
{
    ntsa::Endpoint u;

    NTSCFG_TEST_TRUE(u.isUndefined());

    u = ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 12345);

    NTSCFG_TEST_TRUE(u.isIp());
    NTSCFG_TEST_TRUE(u.ip().host().isV4());
    NTSCFG_TEST_EQ(u.ip().host().v4(), ntsa::Ipv4Address::loopback());
    NTSCFG_TEST_EQ(u.ip().port(), 12345);

    ntsa::LocalName localName;
    localName.setValue("/tmp/test/ntf.uds");

    u = localName;

    NTSCFG_TEST_TRUE(u.isLocal());
    NTSCFG_TEST_EQ(u.local().value(), "/tmp/test/ntf.uds");
}

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyReset)
{
    ntsa::Endpoint u(ntsa::Ipv4Address::loopback(), 12345);

    NTSCFG_TEST_TRUE(u.isIp());
    NTSCFG_TEST_TRUE(u.ip().host().isV4());
    NTSCFG_TEST_EQ(u.ip().host().v4(), ntsa::Ipv4Address::loopback());
    NTSCFG_TEST_EQ(u.ip().port(), 12345);

    u.reset();

    NTSCFG_TEST_TRUE(u.isUndefined());
}

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyParsingIpv4)
{
    const bsl::string e = "127.0.0.1:12345";

    {
        ntsa::Endpoint endpoint(e);

        NTSCFG_TEST_TRUE(endpoint.isIp());
        NTSCFG_TEST_TRUE(endpoint.ip().host().isV4());
        NTSCFG_TEST_EQ(endpoint.ip().host().v4(),
                       ntsa::Ipv4Address::loopback());
        NTSCFG_TEST_EQ(endpoint.ip().port(), 12345);

        {
            bsl::stringstream ss;
            ss << endpoint;

            bsl::string f = ss.str();

            NTSCFG_TEST_LOG_DEBUG << "E: " << e << NTSCFG_TEST_LOG_END;
            NTSCFG_TEST_LOG_DEBUG << "F: " << f << NTSCFG_TEST_LOG_END;

            NTSCFG_TEST_EQ(e, f);
        }
    }

    {
        ntsa::Ipv4Endpoint ipv4Endpoint(e);
        ntsa::Endpoint     endpoint(ipv4Endpoint);

        NTSCFG_TEST_TRUE(endpoint.isIp());
        NTSCFG_TEST_TRUE(endpoint.ip().host().isV4());
        NTSCFG_TEST_EQ(endpoint.ip().host().v4(),
                       ntsa::Ipv4Address::loopback());
        NTSCFG_TEST_EQ(endpoint.ip().port(), 12345);

        {
            bsl::stringstream ss;
            ss << endpoint;

            bsl::string f = ss.str();

            NTSCFG_TEST_LOG_DEBUG << "E: " << e << NTSCFG_TEST_LOG_END;
            NTSCFG_TEST_LOG_DEBUG << "F: " << f << NTSCFG_TEST_LOG_END;

            NTSCFG_TEST_EQ(e, f);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyParsingIpv6)
{
    {
        const bsl::string e = "[::1]:12345";

        ntsa::Endpoint endpoint(e);

        ntsa::Ipv6Address loopbackAddress = ntsa::Ipv6Address::loopback();

        NTSCFG_TEST_TRUE(endpoint.isIp());
        NTSCFG_TEST_TRUE(endpoint.ip().host().isV6());
        NTSCFG_TEST_EQ(endpoint.ip().host().v6(), loopbackAddress);
        NTSCFG_TEST_EQ(endpoint.ip().port(), 12345);

        {
            bsl::stringstream ss;
            ss << endpoint;

            bsl::string f = ss.str();

            NTSCFG_TEST_LOG_DEBUG << "E: " << e << NTSCFG_TEST_LOG_END;
            NTSCFG_TEST_LOG_DEBUG << "F: " << f << NTSCFG_TEST_LOG_END;

            NTSCFG_TEST_EQ(e, f);
        }
    }

    {
        const bsl::string       e = "[::1%1]:12345";
        const ntsa::Ipv6ScopeId s = 1;

        ntsa::Endpoint endpoint(e);

        ntsa::Ipv6Address loopbackAddress = ntsa::Ipv6Address::loopback();
        loopbackAddress.setScopeId(s);

        NTSCFG_TEST_TRUE(endpoint.isIp());
        NTSCFG_TEST_TRUE(endpoint.ip().host().isV6());
        NTSCFG_TEST_EQ(endpoint.ip().host().v6(), loopbackAddress);
        NTSCFG_TEST_EQ(endpoint.ip().host().v6().scopeId(), s);
        NTSCFG_TEST_EQ(endpoint.ip().port(), 12345);

        {
            bsl::stringstream ss;
            ss << endpoint;

            bsl::string f = ss.str();

            NTSCFG_TEST_LOG_DEBUG << "E: " << e << NTSCFG_TEST_LOG_END;
            NTSCFG_TEST_LOG_DEBUG << "F: " << f << NTSCFG_TEST_LOG_END;

            NTSCFG_TEST_EQ(e, f);
        }
    }

    {
        const bsl::string       e = "[::1%1]:12345";
        const ntsa::Ipv6ScopeId s = 1;

        ntsa::Ipv6Endpoint ipv6Endpoint(e);
        ntsa::Endpoint     endpoint(ipv6Endpoint);

        ntsa::Ipv6Address loopbackAddress = ntsa::Ipv6Address::loopback();
        loopbackAddress.setScopeId(s);

        NTSCFG_TEST_TRUE(endpoint.isIp());
        NTSCFG_TEST_TRUE(endpoint.ip().host().isV6());
        NTSCFG_TEST_EQ(endpoint.ip().host().v6(), loopbackAddress);
        NTSCFG_TEST_EQ(endpoint.ip().host().v6().scopeId(), s);
        NTSCFG_TEST_EQ(endpoint.ip().port(), 12345);

        {
            bsl::stringstream ss;
            ss << endpoint;

            bsl::string f = ss.str();

            NTSCFG_TEST_LOG_DEBUG << "E: " << e << NTSCFG_TEST_LOG_END;
            NTSCFG_TEST_LOG_DEBUG << "F: " << f << NTSCFG_TEST_LOG_END;

            NTSCFG_TEST_EQ(e, f);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyParsingLocalName)
{
    const bsl::string e = "/tmp/server";

    ntsa::Endpoint endpoint(e);

    NTSCFG_TEST_TRUE(endpoint.isLocal());
    NTSCFG_TEST_TRUE(endpoint.local().value() == bsl::string("/tmp/server"));

    {
        bsl::stringstream ss;
        ss << endpoint;

        bsl::string f = ss.str();

        NTSCFG_TEST_LOG_DEBUG << "E: " << e << NTSCFG_TEST_LOG_END;
        NTSCFG_TEST_LOG_DEBUG << "F: " << f << NTSCFG_TEST_LOG_END;

        NTSCFG_TEST_EQ(e, f);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyGeneration)
{
    {
        ntsa::Endpoint endpoint(ntsa::Ipv4Address::loopback(), 12345);
        bsl::string text = endpoint.text();
        NTSCFG_TEST_EQ(text, "127.0.0.1:12345");
    }

    {
        ntsa::Endpoint endpoint(ntsa::Ipv6Address::loopback(), 12345);
        bsl::string text = endpoint.text();
        NTSCFG_TEST_EQ(text, "[::1]:12345");
    }

    {
        ntsa::LocalName localName;
        localName.setValue("/tmp/test/ntf.uds");

        ntsa::Endpoint endpoint(localName);
        bsl::string text = endpoint.text();
        NTSCFG_TEST_EQ(text, localName.value());
    }
}

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyHashing)
{
    ntsa::Endpoint endpoint1("127.0.0.1:12345");
    ntsa::Endpoint endpoint2("196.168.0.1:12345");

    bsl::unordered_set<ntsa::Endpoint> endpointSet;
    endpointSet.insert(endpoint1);
    endpointSet.insert(endpoint2);

    NTSCFG_TEST_EQ(endpointSet.size(), 2);
}

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyComparison)
{
    ntsa::Endpoint endpoint1("10.0.0.11:12345");
    ntsa::Endpoint endpoint2("11.0.0.10:12345");

    NTSCFG_TEST_LT(endpoint1, endpoint2);
}

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyCodecBer)
{
    int rc;

    ntsa::Endpoint e1("10.26.55.100:12345");
    ntsa::Endpoint e2;

    bdlsb::MemOutStreamBuf osb(NTSCFG_TEST_ALLOCATOR);

    balber::BerEncoder encoder(0, NTSCFG_TEST_ALLOCATOR);
    rc = encoder.encode(&osb, e1);
    if (rc != 0) {
        NTSCFG_TEST_LOG_DEBUG << encoder.loggedMessages()
                              << NTSCFG_TEST_LOG_END;

        NTSCFG_TEST_EQ(rc, 0);
    }

    rc = osb.pubsync();
    NTSCFG_TEST_EQ(rc, 0);

    NTSCFG_TEST_GT(osb.length(), 0);
    NTSCFG_TEST_NE(osb.data(), 0);

    NTSCFG_TEST_LOG_DEBUG << "Encoded:\n"
                          << bdlb::PrintStringHexDumper(
                                 osb.data(),
                                 static_cast<bsl::size_t>(osb.length()))
                          << NTSCFG_TEST_LOG_END;

    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());

    balber::BerDecoder decoder(0, NTSCFG_TEST_ALLOCATOR);
    rc = decoder.decode(&isb, &e2);
    if (rc != 0) {
        NTSCFG_TEST_LOG_DEBUG << encoder.loggedMessages()
                              << NTSCFG_TEST_LOG_END;

        NTSCFG_TEST_EQ(rc, 0);
    }

    NTSCFG_TEST_EQ(e2, e1);
}

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyCodecJson)
{
    int rc;

    bsl::vector<ntsa::Endpoint> e1(NTSCFG_TEST_ALLOCATOR);
    bsl::vector<ntsa::Endpoint> e2(NTSCFG_TEST_ALLOCATOR);

    e1.push_back(ntsa::Endpoint("10.26.55.100:12345"));

    bdlsb::MemOutStreamBuf osb;

    baljsn::Encoder encoder(NTSCFG_TEST_ALLOCATOR);
    rc = encoder.encode(&osb, e1);
    if (rc != 0) {
        NTSCFG_TEST_LOG_DEBUG << encoder.loggedMessages()
                              << NTSCFG_TEST_LOG_END;

        NTSCFG_TEST_EQ(rc, 0);
    }

    rc = osb.pubsync();
    NTSCFG_TEST_EQ(rc, 0);

    NTSCFG_TEST_GT(osb.length(), 0);
    NTSCFG_TEST_NE(osb.data(), 0);

    NTSCFG_TEST_LOG_DEBUG << "Encoded: "
                          << bsl::string_view(
                                 osb.data(),
                                 static_cast<bsl::size_t>(osb.length()))
                          << NTSCFG_TEST_LOG_END;

    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());

    baljsn::Decoder decoder(NTSCFG_TEST_ALLOCATOR);
    rc = decoder.decode(&isb, &e2);
    if (rc != 0) {
        NTSCFG_TEST_LOG_DEBUG << encoder.loggedMessages()
                              << NTSCFG_TEST_LOG_END;

        NTSCFG_TEST_EQ(rc, 0);
    }

    NTSCFG_TEST_EQ(e2.size(), e1.size());

    for (bsl::size_t i = 0; i < e1.size(); ++i) {
        NTSCFG_TEST_EQ(e2[i], e1[i]);
    }
}

}  // close namespace ntsa
}  // close namespace BloombergLP
