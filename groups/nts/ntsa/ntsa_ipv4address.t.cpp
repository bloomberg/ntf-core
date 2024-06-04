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

#include <ntsa_ipv4address.h>
#include <ntscfg_test.h>
#include <bslma_testallocator.h>
#include <bsls_platform.h>
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
    // Concern: Parsing
    // Plan:

    // Microsoft Visual Studio 2013 does not compile the array literal
    // initialization.
#if !defined(BSLS_PLATFORM_OS_WINDOWS)

    struct Data {
        const char*        text;
        const bsl::uint8_t data[4];
        bool               success;
    } DATA[] = {
        {            "0.0.0.0", {0x00, 0x00, 0x00, 0x00},  true},
        {            "1.2.3.4", {0x01, 0x02, 0x03, 0x04},  true},
        {         "0.1.12.123", {0x00, 0x01, 0x0C, 0x7B},  true},
        {    "255.255.255.255", {0xFF, 0xFF, 0xFF, 0xFF},  true},

        {            "x.y.z.w", {0x00, 0x00, 0x00, 0x00}, false},
        {            "x.2.3.4", {0x00, 0x00, 0x00, 0x00}, false},
        {            "1.2.3.w", {0x00, 0x00, 0x00, 0x00}, false},
        {          "x.2.3.4.5", {0x00, 0x00, 0x00, 0x00}, false},
        {    "256.256.256.256", {0x00, 0x00, 0x00, 0x00}, false},
        {"9999.9999.9999.9999", {0x00, 0x00, 0x00, 0x00}, false},
    };

    enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

    for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
        BSLS_LOG_DEBUG("-------");
        BSLS_LOG_DEBUG("Parsing  = %s", DATA[i].text);

        bsl::string text = DATA[i].text;

        ntsa::Ipv4Address address1;
        bool              success = address1.parse(text);

        if (success) {
            bsl::stringstream ss;
            ss << address1;

            BSLS_LOG_DEBUG("Address1 = %s", ss.str().c_str());
        }

        ntsa::Ipv4Address address2;
        address2.copyFrom(DATA[i].data, sizeof DATA[i].data);

        if (address1 != address2) {
            bsl::stringstream ss;
            ss << address2;

            BSLS_LOG_DEBUG("Address2 = %s", ss.str().c_str());
        }

        NTSCFG_TEST_ASSERT(success == DATA[i].success);
        if (success) {
            NTSCFG_TEST_ASSERT(address1 == address2);
        }
    }

#endif
}

NTSCFG_TEST_CASE(2)
{
    // Concern: Generating
    // Plan:

    // Microsoft Visual Studio 2013 does not compile the array literal
    // initialization.
#if !defined(BSLS_PLATFORM_OS_WINDOWS)

    struct Data {
        const char*        text;
        const bsl::uint8_t data[4];
        bool               success;
    } DATA[] = {
        {        "0.0.0.0", {0x00, 0x00, 0x00, 0x00}, true},
        {        "1.2.3.4", {0x01, 0x02, 0x03, 0x04}, true},
        {     "0.1.12.123", {0x00, 0x01, 0x0C, 0x7B}, true},
        {"255.255.255.255", {0xFF, 0xFF, 0xFF, 0xFF}, true}
    };

    enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

    for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
        ntsa::Ipv4Address address1;
        address1.copyFrom(DATA[i].data, sizeof DATA[i].data);

        BSLS_LOG_DEBUG("-------");
        BSLS_LOG_DEBUG("Generating  = %s", DATA[i].text);

        char buffer[ntsa::Ipv4Address::MAX_TEXT_LENGTH + 1];
        bsl::memset(buffer, static_cast<int>(0xFF), sizeof buffer);

        const bsl::size_t expectedLength = bsl::strlen(DATA[i].text);

        bsl::size_t foundLength = address1.format(buffer, sizeof buffer);

        NTSCFG_TEST_EQ(foundLength, expectedLength);

        NTSCFG_TEST_EQ(bsl::strcmp(buffer, DATA[i].text), 0);
    }

#endif
}

NTSCFG_TEST_CASE(3)
{
    // Concern:
    // Plan:

    ntsa::Ipv4Address address1("127.0.0.1");
    ntsa::Ipv4Address address2("196.168.0.1");

    bsl::unordered_set<ntsa::Ipv4Address> addressSet;
    addressSet.insert(address1);
    addressSet.insert(address2);

    NTSCFG_TEST_ASSERT(addressSet.size() == 2);
}

NTSCFG_TEST_CASE(4)
{
    // Concern: IPv4 address comparison must yield consistent result across
    //          CPUs with varying endianness
    // Plan:

    ntsa::Ipv4Address address1("10.0.0.11");
    ntsa::Ipv4Address address2("11.0.0.10");

    NTSCFG_TEST_ASSERT(address1 < address2);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
    NTSCFG_TEST_REGISTER(4);
}
NTSCFG_TEST_DRIVER_END;
