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

#include <ntsa_ipendpoint.h>
#include <ntscfg_test.h>
#include <balber_berdecoder.h>
#include <balber_berencoder.h>
#include <baljsn_decoder.h>
#include <baljsn_encoder.h>
#include <bdlsb_memoutstreambuf.h>
#include <bdlsb_fixedmeminstreambuf.h>
#include <bslma_testallocator.h>

using namespace BloombergLP;
using namespace ntsa;

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

NTSCFG_TEST_CASE(1)
{
    // Concern:
    // Plan:

    ntsa::IpEndpoint ipEndpoint("127.0.0.1:12345");

    NTSCFG_TEST_TRUE(ipEndpoint.host().isV4());
    NTSCFG_TEST_EQ(ipEndpoint.host().v4(), ntsa::Ipv4Address::loopback());
    NTSCFG_TEST_EQ(ipEndpoint.port(), 12345);
}

NTSCFG_TEST_CASE(2)
{
    // Concern:
    // Plan:

    ntsa::IpEndpoint ipEndpoint("[::1]:12345");

    NTSCFG_TEST_TRUE(ipEndpoint.host().isV6());
    NTSCFG_TEST_EQ(ipEndpoint.host().v6(), ntsa::Ipv6Address::loopback());
    NTSCFG_TEST_EQ(ipEndpoint.port(), 12345);
}

NTSCFG_TEST_CASE(3)
{
    int rc;

    ntscfg::TestAllocator ta;
    {
        ntsa::IpEndpoint e1("10.26.55.100:12345");
        ntsa::IpEndpoint e2;

        bdlsb::MemOutStreamBuf osb(&ta);

        balber::BerEncoder encoder(0, &ta);
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

        balber::BerDecoder decoder(0, &ta);
        rc = decoder.decode(&isb, &e2);
        if (rc != 0) {
            NTSCFG_TEST_LOG_DEBUG << encoder.loggedMessages() 
                                << NTSCFG_TEST_LOG_END;

            NTSCFG_TEST_EQ(rc, 0);
        }

        NTSCFG_TEST_EQ(e2, e1);
    }
    NTSCFG_TEST_EQ(ta.numBlocksInUse(), 0);
}

NTSCFG_TEST_CASE(4)
{
    int rc;

    ntscfg::TestAllocator ta;
    {
        bsl::vector<ntsa::IpEndpoint> e1(&ta);
        bsl::vector<ntsa::IpEndpoint> e2(&ta);

        e1.push_back(ntsa::IpEndpoint("10.26.55.100:12345"));

        bdlsb::MemOutStreamBuf osb(&ta);

        baljsn::Encoder encoder(&ta);
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

        baljsn::Decoder decoder(&ta);
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
    NTSCFG_TEST_EQ(ta.numBlocksInUse(), 0);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
    NTSCFG_TEST_REGISTER(4);
}
NTSCFG_TEST_DRIVER_END;
