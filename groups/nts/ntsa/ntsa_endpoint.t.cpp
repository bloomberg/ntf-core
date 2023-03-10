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

#include <ntsa_endpoint.h>
#include <ntscfg_test.h>
#include <bslma_testallocator.h>
#include <bsl_sstream.h>

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

    const bsl::string e = "127.0.0.1:12345";

    ntsa::Endpoint endpoint(e);

    NTSCFG_TEST_TRUE(endpoint.isIp());
    NTSCFG_TEST_TRUE(endpoint.ip().host().isV4());
    NTSCFG_TEST_EQ(endpoint.ip().host().v4(), ntsa::Ipv4Address::loopback());
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

NTSCFG_TEST_CASE(2)
{
    // Concern:
    // Plan:

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
}

NTSCFG_TEST_CASE(3)
{
    // Concern:
    // Plan:

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

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
}
NTSCFG_TEST_DRIVER_END;
