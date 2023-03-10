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

#include <ntsa_ipv6scopeid.h>
#include <ntscfg_test.h>
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

    ntscfg::TestAllocator ta;
    {
        struct Data {
            const char*   d_text;
            bsl::uint16_t d_value;
            bool          d_success;
        } DATA[] = {
            {    "0",     0,  true},
            {    "1",     1,  true},
            {   "12",    12,  true},
            {  "123",   123,  true},
            {"28588", 28588,  true},
            {"65535", 65535,  true},
            {   "-1",     0, false},
            {"65536",     0, false}
        };

        enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

        for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
            ntsa::Ipv6ScopeId scopeId;
            bool              result =
                ntsa::Ipv6ScopeIdUtil::parse(&scopeId,
                                             DATA[i].d_text,
                                             bsl::strlen(DATA[i].d_text));
            if (DATA[i].d_success) {
                NTSCFG_TEST_TRUE(result);
                NTSCFG_TEST_EQ(scopeId, DATA[i].d_value);
            }
            else {
                NTSCFG_TEST_FALSE(result);
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
        struct Data {
            bsl::uint16_t d_value;
            const char*   d_text;
        } DATA[] = {
            {    0,     "0"},
            {    1,     "1"},
            {   12,    "12"},
            {  123,   "123"},
            {28588, "28588"},
            {65535, "65535"}
        };

        enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

        for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
            char        buffer[ntsa::Ipv6ScopeIdUtil::MAX_LENGTH + 1];
            bsl::size_t size = ntsa::Ipv6ScopeIdUtil::format(buffer,
                                                             sizeof buffer,
                                                             DATA[i].d_value);
            NTSCFG_TEST_EQ(size, bsl::strlen(buffer));
            NTSCFG_TEST_EQ(0, bsl::strcmp(buffer, DATA[i].d_text));
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
}
NTSCFG_TEST_DRIVER_END;
