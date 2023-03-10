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

#include <ntsa_host.h>
#include <ntscfg_test.h>
#include <bslma_testallocator.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_unordered_set.h>

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

    ntscfg::TestAllocator ta;
    {
        {
            ntsa::Host host;
            bool       valid = host.parse("192.168.1.250");
            NTSCFG_TEST_ASSERT(valid);
            NTSCFG_TEST_ASSERT(host.isIp());
            NTSCFG_TEST_ASSERT(host.ip().isV4());

            {
                bsl::stringstream ss;
                ss << host;

                BSLS_LOG_DEBUG("Host = %s", ss.str().c_str());
            }
        }

        {
            ntsa::Host host;
            bool valid = host.parse("2001:41c0::645:a65e:60ff:feda:589d");
            NTSCFG_TEST_ASSERT(valid);
            NTSCFG_TEST_ASSERT(host.isIp());
            NTSCFG_TEST_ASSERT(host.ip().isV6());

            {
                bsl::stringstream ss;
                ss << host;

                BSLS_LOG_DEBUG("Host = %s", ss.str().c_str());
            }
        }

        {
            ntsa::Host host;
            bool       valid = host.parse("localhost");
            NTSCFG_TEST_ASSERT(valid);
            NTSCFG_TEST_ASSERT(host.isDomainName());

            {
                bsl::stringstream ss;
                ss << host;

                BSLS_LOG_DEBUG("Host = %s", ss.str().c_str());
            }
        }

        {
            ntsa::Host host;
            bool       valid = host.parse("localhost.localdomain");
            NTSCFG_TEST_ASSERT(valid);
            NTSCFG_TEST_ASSERT(host.isDomainName());

            {
                bsl::stringstream ss;
                ss << host;

                BSLS_LOG_DEBUG("Host = %s", ss.str().c_str());
            }
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(2)
{
    // Concern:
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::Host host1("192.168.1.250");
        ntsa::Host host2("2001:41c0::645:a65e:60ff:feda:589d");
        ntsa::Host host3("localhost.localdomain");

        bsl::unordered_set<ntsa::Host> hostSet;
        hostSet.insert(host1);
        hostSet.insert(host2);
        hostSet.insert(host3);

        NTSCFG_TEST_ASSERT(hostSet.size() == 3);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
}
NTSCFG_TEST_DRIVER_END;
