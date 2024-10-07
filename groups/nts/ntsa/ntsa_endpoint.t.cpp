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

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyCase1)
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

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyCase2)
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

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyCase3)
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

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyCase4)
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

NTSCFG_TEST_FUNCTION(ntsa::EndpointTest::verifyCase5)
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
