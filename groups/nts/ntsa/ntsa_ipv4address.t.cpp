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

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv4address_t_cpp, "$Id$ $CSID$")

#include <ntscfg_test.h>
#include <bslma_testallocator.h>
#include <bsls_platform.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_unordered_set.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Ipv4Address'.
class Ipv4AddressTest
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

    // Test parsing.
    static void verifyParsing();

    // Test generation.
    static void verifyGeneration();

    // Test hashing.
    static void verifyHashing();

    // Test comparison. In particular, comparison must yield consistent results
    // across different CPUs with different endianness.
    static void verifyComparison();
};

void Ipv4AddressTest::verifyTypeTraits()
{
    const bool isBitwiseInitializable =
        NTSCFG_TYPE_CHECK_BITWISE_INITIALIZABLE(ntsa::Ipv4Address);

    NTSCFG_TEST_TRUE(isBitwiseInitializable);

    const bool isBitwiseMovable =
        NTSCFG_TYPE_CHECK_BITWISE_MOVABLE(ntsa::Ipv4Address);

    NTSCFG_TEST_TRUE(isBitwiseMovable);

    const bool isBitwiseCopyable =
        NTSCFG_TYPE_CHECK_BITWISE_COPYABLE(ntsa::Ipv4Address);

    NTSCFG_TEST_TRUE(isBitwiseCopyable);

    const bool isBitwiseComparable =
        NTSCFG_TYPE_CHECK_BITWISE_COMPARABLE(ntsa::Ipv4Address);

    NTSCFG_TEST_TRUE(isBitwiseComparable);
}

void Ipv4AddressTest::verifyDefaultConstructor()
{
    ntsa::Ipv4Address u;

    NTSCFG_TEST_EQ(u[0], 0);
    NTSCFG_TEST_EQ(u[1], 0);
    NTSCFG_TEST_EQ(u[2], 0);
    NTSCFG_TEST_EQ(u[3], 0);
}

void Ipv4AddressTest::verifyMoveConstructor()
{
    ntsa::Ipv4Address u("1.2.3.4");

    NTSCFG_TEST_EQ(u[0], 1);
    NTSCFG_TEST_EQ(u[1], 2);
    NTSCFG_TEST_EQ(u[2], 3);
    NTSCFG_TEST_EQ(u[3], 4);

    ntsa::Ipv4Address v(NTSCFG_MOVE(u));

    NTSCFG_TEST_EQ(v[0], 1);
    NTSCFG_TEST_EQ(v[1], 2);
    NTSCFG_TEST_EQ(v[2], 3);
    NTSCFG_TEST_EQ(v[3], 4);

#if NTSCFG_MOVE_RESET_ENABLED
    NTSCFG_TEST_EQ(u[0], 0);
    NTSCFG_TEST_EQ(u[1], 0);
    NTSCFG_TEST_EQ(u[2], 0);
    NTSCFG_TEST_EQ(u[3], 0);
#endif
}

void Ipv4AddressTest::verifyCopyConstructor()
{
    ntsa::Ipv4Address u("1.2.3.4");

    NTSCFG_TEST_EQ(u[0], 1);
    NTSCFG_TEST_EQ(u[1], 2);
    NTSCFG_TEST_EQ(u[2], 3);
    NTSCFG_TEST_EQ(u[3], 4);

    ntsa::Ipv4Address v(u);

    NTSCFG_TEST_EQ(v[0], 1);
    NTSCFG_TEST_EQ(v[1], 2);
    NTSCFG_TEST_EQ(v[2], 3);
    NTSCFG_TEST_EQ(v[3], 4);
}

void Ipv4AddressTest::verifyOverloadConstructor()
{
    {
        bsl::uint32_t value = 0;
        {
            bsl::uint8_t buffer[4] = {0x01, 0x02, 0x03, 0x04};
            NTSCFG_TEST_EQ(sizeof buffer, sizeof value);
            bsl::memcpy(&value, buffer, sizeof buffer);
        }

        ntsa::Ipv4Address u(value);

        NTSCFG_TEST_EQ(u[0], 1);
        NTSCFG_TEST_EQ(u[1], 2);
        NTSCFG_TEST_EQ(u[2], 3);
        NTSCFG_TEST_EQ(u[3], 4);
    }

    {
        ntsa::Ipv4Address u("1.2.3.4");

        NTSCFG_TEST_EQ(u[0], 1);
        NTSCFG_TEST_EQ(u[1], 2);
        NTSCFG_TEST_EQ(u[2], 3);
        NTSCFG_TEST_EQ(u[3], 4);
    }
}

void Ipv4AddressTest::verifyCopyAssignment()
{
    ntsa::Ipv4Address u("1.2.3.4");

    NTSCFG_TEST_EQ(u[0], 1);
    NTSCFG_TEST_EQ(u[1], 2);
    NTSCFG_TEST_EQ(u[2], 3);
    NTSCFG_TEST_EQ(u[3], 4);

    ntsa::Ipv4Address v;

    NTSCFG_TEST_EQ(v[0], 0);
    NTSCFG_TEST_EQ(v[1], 0);
    NTSCFG_TEST_EQ(v[2], 0);
    NTSCFG_TEST_EQ(v[3], 0);

    v = u;

    NTSCFG_TEST_EQ(v[0], 1);
    NTSCFG_TEST_EQ(v[1], 2);
    NTSCFG_TEST_EQ(v[2], 3);
    NTSCFG_TEST_EQ(v[3], 4);
}

void Ipv4AddressTest::verifyMoveAssignment()
{
    ntsa::Ipv4Address u("1.2.3.4");

    NTSCFG_TEST_EQ(u[0], 1);
    NTSCFG_TEST_EQ(u[1], 2);
    NTSCFG_TEST_EQ(u[2], 3);
    NTSCFG_TEST_EQ(u[3], 4);

    ntsa::Ipv4Address v;

    NTSCFG_TEST_EQ(v[0], 0);
    NTSCFG_TEST_EQ(v[1], 0);
    NTSCFG_TEST_EQ(v[2], 0);
    NTSCFG_TEST_EQ(v[3], 0);

    v = NTSCFG_MOVE(u);

    NTSCFG_TEST_EQ(v[0], 1);
    NTSCFG_TEST_EQ(v[1], 2);
    NTSCFG_TEST_EQ(v[2], 3);
    NTSCFG_TEST_EQ(v[3], 4);

#if NTSCFG_MOVE_RESET_ENABLED
    NTSCFG_TEST_EQ(u[0], 0);
    NTSCFG_TEST_EQ(u[1], 0);
    NTSCFG_TEST_EQ(u[2], 0);
    NTSCFG_TEST_EQ(u[3], 0);
#endif
}

void Ipv4AddressTest::verifyOverloadAssignment()
{
    ntsa::Ipv4Address u;

    NTSCFG_TEST_EQ(u[0], 0);
    NTSCFG_TEST_EQ(u[1], 0);
    NTSCFG_TEST_EQ(u[2], 0);
    NTSCFG_TEST_EQ(u[3], 0);

    u = "1.2.3.4";

    NTSCFG_TEST_EQ(u[0], 1);
    NTSCFG_TEST_EQ(u[1], 2);
    NTSCFG_TEST_EQ(u[2], 3);
    NTSCFG_TEST_EQ(u[3], 4);
}

void Ipv4AddressTest::verifyReset()
{
    ntsa::Ipv4Address u("1.2.3.4");

    NTSCFG_TEST_EQ(u[0], 1);
    NTSCFG_TEST_EQ(u[1], 2);
    NTSCFG_TEST_EQ(u[2], 3);
    NTSCFG_TEST_EQ(u[3], 4);

    u.reset();

    NTSCFG_TEST_EQ(u[0], 0);
    NTSCFG_TEST_EQ(u[1], 0);
    NTSCFG_TEST_EQ(u[2], 0);
    NTSCFG_TEST_EQ(u[3], 0);
}

void Ipv4AddressTest::verifyParsing()
{
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

void Ipv4AddressTest::verifyGeneration()
{
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

void Ipv4AddressTest::verifyHashing()
{
    ntsa::Ipv4Address address1("127.0.0.1");
    ntsa::Ipv4Address address2("196.168.0.1");

    bsl::unordered_set<ntsa::Ipv4Address> addressSet;
    addressSet.insert(address1);
    addressSet.insert(address2);

    NTSCFG_TEST_EQ(addressSet.size(), 2);
}

void Ipv4AddressTest::verifyComparison()
{
    ntsa::Ipv4Address address1("10.0.0.11");
    ntsa::Ipv4Address address2("11.0.0.10");

    NTSCFG_TEST_LT(address1, address2);
}

}  // close namespace ntsa
}  // close namespace BloombergLP

NTSCFG_TEST_SUITE
{
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv4AddressTest::verifyTypeTraits);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv4AddressTest::verifyDefaultConstructor);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv4AddressTest::verifyMoveConstructor);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv4AddressTest::verifyCopyConstructor);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv4AddressTest::verifyOverloadConstructor);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv4AddressTest::verifyCopyAssignment);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv4AddressTest::verifyMoveAssignment);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv4AddressTest::verifyOverloadAssignment);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv4AddressTest::verifyReset);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv4AddressTest::verifyParsing);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv4AddressTest::verifyGeneration);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv4AddressTest::verifyHashing);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv4AddressTest::verifyComparison);
}
NTSCFG_TEST_SUITE_END;
