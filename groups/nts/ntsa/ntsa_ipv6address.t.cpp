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

#include <ntsa_ipv6address.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv6address_t_cpp, "$Id$ $CSID$")

#include <ntscfg_test.h>
#include <bslma_testallocator.h>
#include <bsls_platform.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_unordered_set.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Ipv6Address'.
class Ipv6AddressTest
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

    // Test formatting.
    static void verifyFormatting();

    // Test parsing.
    static void verifyParsing();

    // Test parsing scope identifiers.
    static void verifyParsingScopeId();

    // Test parsing abbreviations.
    static void verifyParsingAbbreviation();

    // Test hashing.
    static void verifyHashing();

    // Test comparison. In particular, comparison must yield consistent results
    // across different CPUs with different endianness.
    static void verifyComparison();
};

void Ipv6AddressTest::verifyTypeTraits()
{
    const bool isBitwiseInitializable =
        NTSCFG_TYPE_CHECK_BITWISE_INITIALIZABLE(ntsa::Ipv6Address);

    NTSCFG_TEST_TRUE(isBitwiseInitializable);

    const bool isBitwiseMovable =
        NTSCFG_TYPE_CHECK_BITWISE_MOVABLE(ntsa::Ipv6Address);

    NTSCFG_TEST_TRUE(isBitwiseMovable);

    const bool isBitwiseCopyable =
        NTSCFG_TYPE_CHECK_BITWISE_COPYABLE(ntsa::Ipv6Address);

    NTSCFG_TEST_TRUE(isBitwiseCopyable);

    const bool isBitwiseComparable =
        NTSCFG_TYPE_CHECK_BITWISE_COMPARABLE(ntsa::Ipv6Address);

    NTSCFG_TEST_TRUE(isBitwiseComparable);
}

void Ipv6AddressTest::verifyDefaultConstructor()
{
    ntsa::Ipv6Address u;

    NTSCFG_TEST_EQ(u.byQword(0), 0);
    NTSCFG_TEST_EQ(u.byQword(1), 0);
    NTSCFG_TEST_EQ(u.scopeId(), 0);
}

void Ipv6AddressTest::verifyMoveConstructor()
{
    ntsa::Ipv6Address u("2606:2800:220:1:248:1893:25c8:1946%123");

    NTSCFG_TEST_EQ(u[0x00], 0x26);
    NTSCFG_TEST_EQ(u[0x01], 0x06);
    NTSCFG_TEST_EQ(u[0x02], 0x28);
    NTSCFG_TEST_EQ(u[0x03], 0x00);
    NTSCFG_TEST_EQ(u[0x04], 0x02);
    NTSCFG_TEST_EQ(u[0x05], 0x20);
    NTSCFG_TEST_EQ(u[0x06], 0x00);
    NTSCFG_TEST_EQ(u[0x07], 0x01);
    NTSCFG_TEST_EQ(u[0x08], 0x02);
    NTSCFG_TEST_EQ(u[0x09], 0x48);
    NTSCFG_TEST_EQ(u[0x0A], 0x18);
    NTSCFG_TEST_EQ(u[0x0B], 0x93);
    NTSCFG_TEST_EQ(u[0x0C], 0x25);
    NTSCFG_TEST_EQ(u[0x0D], 0xc8);
    NTSCFG_TEST_EQ(u[0x0E], 0x19);
    NTSCFG_TEST_EQ(u[0x0F], 0x46);

    NTSCFG_TEST_EQ(u.scopeId(), 123);

    ntsa::Ipv6Address v(NTSCFG_MOVE(u));

    NTSCFG_TEST_EQ(v[0x00], 0x26);
    NTSCFG_TEST_EQ(v[0x01], 0x06);
    NTSCFG_TEST_EQ(v[0x02], 0x28);
    NTSCFG_TEST_EQ(v[0x03], 0x00);
    NTSCFG_TEST_EQ(v[0x04], 0x02);
    NTSCFG_TEST_EQ(v[0x05], 0x20);
    NTSCFG_TEST_EQ(v[0x06], 0x00);
    NTSCFG_TEST_EQ(v[0x07], 0x01);
    NTSCFG_TEST_EQ(v[0x08], 0x02);
    NTSCFG_TEST_EQ(v[0x09], 0x48);
    NTSCFG_TEST_EQ(v[0x0A], 0x18);
    NTSCFG_TEST_EQ(v[0x0B], 0x93);
    NTSCFG_TEST_EQ(v[0x0C], 0x25);
    NTSCFG_TEST_EQ(v[0x0D], 0xc8);
    NTSCFG_TEST_EQ(v[0x0E], 0x19);
    NTSCFG_TEST_EQ(v[0x0F], 0x46);

#if NTSCFG_MOVE_RESET_ENABLED
    NTSCFG_TEST_EQ(u.byQword(0), 0);
    NTSCFG_TEST_EQ(u.byQword(1), 0);
    NTSCFG_TEST_EQ(u.scopeId(), 0);
#endif
}

void Ipv6AddressTest::verifyCopyConstructor()
{
    ntsa::Ipv6Address u("2606:2800:220:1:248:1893:25c8:1946%123");

    NTSCFG_TEST_EQ(u[0x00], 0x26);
    NTSCFG_TEST_EQ(u[0x01], 0x06);
    NTSCFG_TEST_EQ(u[0x02], 0x28);
    NTSCFG_TEST_EQ(u[0x03], 0x00);
    NTSCFG_TEST_EQ(u[0x04], 0x02);
    NTSCFG_TEST_EQ(u[0x05], 0x20);
    NTSCFG_TEST_EQ(u[0x06], 0x00);
    NTSCFG_TEST_EQ(u[0x07], 0x01);
    NTSCFG_TEST_EQ(u[0x08], 0x02);
    NTSCFG_TEST_EQ(u[0x09], 0x48);
    NTSCFG_TEST_EQ(u[0x0A], 0x18);
    NTSCFG_TEST_EQ(u[0x0B], 0x93);
    NTSCFG_TEST_EQ(u[0x0C], 0x25);
    NTSCFG_TEST_EQ(u[0x0D], 0xc8);
    NTSCFG_TEST_EQ(u[0x0E], 0x19);
    NTSCFG_TEST_EQ(u[0x0F], 0x46);

    NTSCFG_TEST_EQ(u.scopeId(), 123);

    ntsa::Ipv6Address v(u);

    NTSCFG_TEST_EQ(v[0x00], 0x26);
    NTSCFG_TEST_EQ(v[0x01], 0x06);
    NTSCFG_TEST_EQ(v[0x02], 0x28);
    NTSCFG_TEST_EQ(v[0x03], 0x00);
    NTSCFG_TEST_EQ(v[0x04], 0x02);
    NTSCFG_TEST_EQ(v[0x05], 0x20);
    NTSCFG_TEST_EQ(v[0x06], 0x00);
    NTSCFG_TEST_EQ(v[0x07], 0x01);
    NTSCFG_TEST_EQ(v[0x08], 0x02);
    NTSCFG_TEST_EQ(v[0x09], 0x48);
    NTSCFG_TEST_EQ(v[0x0A], 0x18);
    NTSCFG_TEST_EQ(v[0x0B], 0x93);
    NTSCFG_TEST_EQ(v[0x0C], 0x25);
    NTSCFG_TEST_EQ(v[0x0D], 0xc8);
    NTSCFG_TEST_EQ(v[0x0E], 0x19);
    NTSCFG_TEST_EQ(v[0x0F], 0x46);
}

void Ipv6AddressTest::verifyOverloadConstructor()
{
}

void Ipv6AddressTest::verifyCopyAssignment()
{
    ntsa::Ipv6Address u("2606:2800:220:1:248:1893:25c8:1946%123");

    NTSCFG_TEST_EQ(u[0x00], 0x26);
    NTSCFG_TEST_EQ(u[0x01], 0x06);
    NTSCFG_TEST_EQ(u[0x02], 0x28);
    NTSCFG_TEST_EQ(u[0x03], 0x00);
    NTSCFG_TEST_EQ(u[0x04], 0x02);
    NTSCFG_TEST_EQ(u[0x05], 0x20);
    NTSCFG_TEST_EQ(u[0x06], 0x00);
    NTSCFG_TEST_EQ(u[0x07], 0x01);
    NTSCFG_TEST_EQ(u[0x08], 0x02);
    NTSCFG_TEST_EQ(u[0x09], 0x48);
    NTSCFG_TEST_EQ(u[0x0A], 0x18);
    NTSCFG_TEST_EQ(u[0x0B], 0x93);
    NTSCFG_TEST_EQ(u[0x0C], 0x25);
    NTSCFG_TEST_EQ(u[0x0D], 0xc8);
    NTSCFG_TEST_EQ(u[0x0E], 0x19);
    NTSCFG_TEST_EQ(u[0x0F], 0x46);

    NTSCFG_TEST_EQ(u.scopeId(), 123);

    ntsa::Ipv6Address v;
    v = u;

    NTSCFG_TEST_EQ(v[0x00], 0x26);
    NTSCFG_TEST_EQ(v[0x01], 0x06);
    NTSCFG_TEST_EQ(v[0x02], 0x28);
    NTSCFG_TEST_EQ(v[0x03], 0x00);
    NTSCFG_TEST_EQ(v[0x04], 0x02);
    NTSCFG_TEST_EQ(v[0x05], 0x20);
    NTSCFG_TEST_EQ(v[0x06], 0x00);
    NTSCFG_TEST_EQ(v[0x07], 0x01);
    NTSCFG_TEST_EQ(v[0x08], 0x02);
    NTSCFG_TEST_EQ(v[0x09], 0x48);
    NTSCFG_TEST_EQ(v[0x0A], 0x18);
    NTSCFG_TEST_EQ(v[0x0B], 0x93);
    NTSCFG_TEST_EQ(v[0x0C], 0x25);
    NTSCFG_TEST_EQ(v[0x0D], 0xc8);
    NTSCFG_TEST_EQ(v[0x0E], 0x19);
    NTSCFG_TEST_EQ(v[0x0F], 0x46);
}

void Ipv6AddressTest::verifyMoveAssignment()
{
    ntsa::Ipv6Address u("2606:2800:220:1:248:1893:25c8:1946%123");

    NTSCFG_TEST_EQ(u[0x00], 0x26);
    NTSCFG_TEST_EQ(u[0x01], 0x06);
    NTSCFG_TEST_EQ(u[0x02], 0x28);
    NTSCFG_TEST_EQ(u[0x03], 0x00);
    NTSCFG_TEST_EQ(u[0x04], 0x02);
    NTSCFG_TEST_EQ(u[0x05], 0x20);
    NTSCFG_TEST_EQ(u[0x06], 0x00);
    NTSCFG_TEST_EQ(u[0x07], 0x01);
    NTSCFG_TEST_EQ(u[0x08], 0x02);
    NTSCFG_TEST_EQ(u[0x09], 0x48);
    NTSCFG_TEST_EQ(u[0x0A], 0x18);
    NTSCFG_TEST_EQ(u[0x0B], 0x93);
    NTSCFG_TEST_EQ(u[0x0C], 0x25);
    NTSCFG_TEST_EQ(u[0x0D], 0xc8);
    NTSCFG_TEST_EQ(u[0x0E], 0x19);
    NTSCFG_TEST_EQ(u[0x0F], 0x46);

    NTSCFG_TEST_EQ(u.scopeId(), 123);

    ntsa::Ipv6Address v;
    v = NTSCFG_MOVE(u);

    NTSCFG_TEST_EQ(v[0x00], 0x26);
    NTSCFG_TEST_EQ(v[0x01], 0x06);
    NTSCFG_TEST_EQ(v[0x02], 0x28);
    NTSCFG_TEST_EQ(v[0x03], 0x00);
    NTSCFG_TEST_EQ(v[0x04], 0x02);
    NTSCFG_TEST_EQ(v[0x05], 0x20);
    NTSCFG_TEST_EQ(v[0x06], 0x00);
    NTSCFG_TEST_EQ(v[0x07], 0x01);
    NTSCFG_TEST_EQ(v[0x08], 0x02);
    NTSCFG_TEST_EQ(v[0x09], 0x48);
    NTSCFG_TEST_EQ(v[0x0A], 0x18);
    NTSCFG_TEST_EQ(v[0x0B], 0x93);
    NTSCFG_TEST_EQ(v[0x0C], 0x25);
    NTSCFG_TEST_EQ(v[0x0D], 0xc8);
    NTSCFG_TEST_EQ(v[0x0E], 0x19);
    NTSCFG_TEST_EQ(v[0x0F], 0x46);

#if NTSCFG_MOVE_RESET_ENABLED
    NTSCFG_TEST_EQ(u.byQword(0), 0);
    NTSCFG_TEST_EQ(u.byQword(1), 0);
    NTSCFG_TEST_EQ(u.scopeId(), 0);
#endif
}

void Ipv6AddressTest::verifyOverloadAssignment()
{
}

void Ipv6AddressTest::verifyReset()
{
    ntsa::Ipv6Address u("2606:2800:220:1:248:1893:25c8:1946%123");

    NTSCFG_TEST_EQ(u[0x00], 0x26);
    NTSCFG_TEST_EQ(u[0x01], 0x06);
    NTSCFG_TEST_EQ(u[0x02], 0x28);
    NTSCFG_TEST_EQ(u[0x03], 0x00);
    NTSCFG_TEST_EQ(u[0x04], 0x02);
    NTSCFG_TEST_EQ(u[0x05], 0x20);
    NTSCFG_TEST_EQ(u[0x06], 0x00);
    NTSCFG_TEST_EQ(u[0x07], 0x01);
    NTSCFG_TEST_EQ(u[0x08], 0x02);
    NTSCFG_TEST_EQ(u[0x09], 0x48);
    NTSCFG_TEST_EQ(u[0x0A], 0x18);
    NTSCFG_TEST_EQ(u[0x0B], 0x93);
    NTSCFG_TEST_EQ(u[0x0C], 0x25);
    NTSCFG_TEST_EQ(u[0x0D], 0xc8);
    NTSCFG_TEST_EQ(u[0x0E], 0x19);
    NTSCFG_TEST_EQ(u[0x0F], 0x46);

    NTSCFG_TEST_EQ(u.scopeId(), 123);

    u.reset();

    NTSCFG_TEST_EQ(u.byQword(0), 0);
    NTSCFG_TEST_EQ(u.byQword(1), 0);
    NTSCFG_TEST_EQ(u.scopeId(), 0);
}

void Ipv6AddressTest::verifyFormatting()
{
    // a

    {
        const char OUTPUT[] = "0:0:0:0:0:0:0:0";

        ntsa::Ipv6Address address;

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, false);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "::";

        ntsa::Ipv6Address address;

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, true);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "0:0:0:0:0:0:0:0%123";

        ntsa::Ipv6Address address;
        address.setScopeId(123);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, false);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "::%123";

        ntsa::Ipv6Address address;
        address.setScopeId(123);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, true);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    // b

    {
        const char OUTPUT[] = "0:0:0:0:0:0:0:1";

        const bsl::uint8_t INPUT[16] = {0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, false);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "::1";

        const bsl::uint8_t INPUT[16] = {0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, true);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "0:0:0:0:0:0:0:1%123";

        const bsl::uint8_t INPUT[16] = {0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);
        address.setScopeId(123);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, false);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "::1%123";

        const bsl::uint8_t INPUT[16] = {0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);
        address.setScopeId(123);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, true);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    // c

    {
        const char OUTPUT[] = "2606:2800:220:1:248:1893:25c8:1946";

        const bsl::uint8_t INPUT[16] = {0x26,
                                        0x06,
                                        0x28,
                                        0x00,
                                        0x02,
                                        0x20,
                                        0x00,
                                        0x01,
                                        0x02,
                                        0x48,
                                        0x18,
                                        0x93,
                                        0x25,
                                        0xc8,
                                        0x19,
                                        0x46};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, false);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "2606:2800:220:1:248:1893:25c8:1946";

        const bsl::uint8_t INPUT[16] = {0x26,
                                        0x06,
                                        0x28,
                                        0x00,
                                        0x02,
                                        0x20,
                                        0x00,
                                        0x01,
                                        0x02,
                                        0x48,
                                        0x18,
                                        0x93,
                                        0x25,
                                        0xc8,
                                        0x19,
                                        0x46};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, true);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "2606:2800:220:1:248:1893:25c8:1946%123";

        const bsl::uint8_t INPUT[16] = {0x26,
                                        0x06,
                                        0x28,
                                        0x00,
                                        0x02,
                                        0x20,
                                        0x00,
                                        0x01,
                                        0x02,
                                        0x48,
                                        0x18,
                                        0x93,
                                        0x25,
                                        0xc8,
                                        0x19,
                                        0x46};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);
        address.setScopeId(123);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, false);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "2606:2800:220:1:248:1893:25c8:1946%123";

        const bsl::uint8_t INPUT[16] = {0x26,
                                        0x06,
                                        0x28,
                                        0x00,
                                        0x02,
                                        0x20,
                                        0x00,
                                        0x01,
                                        0x02,
                                        0x48,
                                        0x18,
                                        0x93,
                                        0x25,
                                        0xc8,
                                        0x19,
                                        0x46};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);
        address.setScopeId(123);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, true);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    // d

    {
        const char OUTPUT[] = "2001:41c0:0:645:a65e:60ff:feda:589d";

        const bsl::uint8_t INPUT[16] = {0x20,
                                        0x01,
                                        0x41,
                                        0xc0,
                                        0x00,
                                        0x00,
                                        0x06,
                                        0x45,
                                        0xa6,
                                        0x5e,
                                        0x60,
                                        0xff,
                                        0xfe,
                                        0xda,
                                        0x58,
                                        0x9d};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, false);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "2001:41c0::645:a65e:60ff:feda:589d";

        const bsl::uint8_t INPUT[16] = {0x20,
                                        0x01,
                                        0x41,
                                        0xc0,
                                        0x00,
                                        0x00,
                                        0x06,
                                        0x45,
                                        0xa6,
                                        0x5e,
                                        0x60,
                                        0xff,
                                        0xfe,
                                        0xda,
                                        0x58,
                                        0x9d};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, true);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "2001:41c0:0:645:a65e:60ff:feda:589d%123";

        const bsl::uint8_t INPUT[16] = {0x20,
                                        0x01,
                                        0x41,
                                        0xc0,
                                        0x00,
                                        0x00,
                                        0x06,
                                        0x45,
                                        0xa6,
                                        0x5e,
                                        0x60,
                                        0xff,
                                        0xfe,
                                        0xda,
                                        0x58,
                                        0x9d};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);
        address.setScopeId(123);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, false);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "2001:41c0::645:a65e:60ff:feda:589d%123";

        const bsl::uint8_t INPUT[16] = {0x20,
                                        0x01,
                                        0x41,
                                        0xc0,
                                        0x00,
                                        0x00,
                                        0x06,
                                        0x45,
                                        0xa6,
                                        0x5e,
                                        0x60,
                                        0xff,
                                        0xfe,
                                        0xda,
                                        0x58,
                                        0x9d};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);
        address.setScopeId(123);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, true);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    // e

    {
        const char OUTPUT[] = "2001:db8:0:0:1:0:0:1";

        const bsl::uint8_t INPUT[16] = {0x20,
                                        0x01,
                                        0x0d,
                                        0xb8,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, false);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "2001:db8::1:0:0:1";

        const bsl::uint8_t INPUT[16] = {0x20,
                                        0x01,
                                        0x0d,
                                        0xb8,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, true);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "2001:db8:0:0:1:0:0:1%123";

        const bsl::uint8_t INPUT[16] = {0x20,
                                        0x01,
                                        0x0d,
                                        0xb8,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);
        address.setScopeId(123);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, false);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "2001:db8::1:0:0:1%123";

        const bsl::uint8_t INPUT[16] = {0x20,
                                        0x01,
                                        0x0d,
                                        0xb8,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);
        address.setScopeId(123);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, true);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    // f

    {
        const char OUTPUT[] = "2001:41c0:0:0:0:0:0:1";

        const bsl::uint8_t INPUT[16] = {0x20,
                                        0x01,
                                        0x41,
                                        0xc0,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, false);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "2001:41c0::1";

        const bsl::uint8_t INPUT[16] = {0x20,
                                        0x01,
                                        0x41,
                                        0xc0,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, true);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "2001:41c0:0:0:0:0:0:1%123";

        const bsl::uint8_t INPUT[16] = {0x20,
                                        0x01,
                                        0x41,
                                        0xc0,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);
        address.setScopeId(123);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, false);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "2001:41c0::1%123";

        const bsl::uint8_t INPUT[16] = {0x20,
                                        0x01,
                                        0x41,
                                        0xc0,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);
        address.setScopeId(123);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, true);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    // g

    {
        const char OUTPUT[] = "2606:0:0:0:0:0:0:1";

        const bsl::uint8_t INPUT[16] = {0x26,
                                        0x06,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, false);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "2606::1";

        const bsl::uint8_t INPUT[16] = {0x26,
                                        0x06,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, true);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "2606:0:0:0:0:0:0:1%123";

        const bsl::uint8_t INPUT[16] = {0x26,
                                        0x06,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);
        address.setScopeId(123);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, false);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }

    {
        const char OUTPUT[] = "2606::1%123";

        const bsl::uint8_t INPUT[16] = {0x26,
                                        0x06,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x00,
                                        0x01};

        ntsa::Ipv6Address address;
        address.copyFrom(INPUT, sizeof INPUT);
        address.setScopeId(123);

        char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        const bsl::size_t n = address.format(buffer, sizeof buffer, true);

        NTSCFG_TEST_EQ(n, sizeof OUTPUT - 1);
        NTSCFG_TEST_EQ(bsl::strcmp(buffer, OUTPUT), 0);
    }
}

void Ipv6AddressTest::verifyParsing()
{
    // Microsoft Visual Studio 2013 does not compile the array literal
    // initialization.
#if !defined(BSLS_PLATFORM_OS_WINDOWS)

    struct data {
        const char*        text;
        const bsl::uint8_t data[16];
        bool               success;
    } DATA[] = {
        {                               "abc",
         {0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00},
         false},
        {"2606:2800:220:1:248:1893:25c8:1946",
         {0x26,
         0x06,
         0x28,
         0x00,
         0x02,
         0x20,
         0x00,
         0x01,
         0x02,
         0x48,
         0x18,
         0x93,
         0x25,
         0xc8,
         0x19,
         0x46},
         true },
        {"2001:41c0::645:a65e:60ff:feda:589d",
         {0x20,
         0x01,
         0x41,
         0xc0,
         0x00,
         0x00,
         0x06,
         0x45,
         0xa6,
         0x5e,
         0x60,
         0xff,
         0xfe,
         0xda,
         0x58,
         0x9d},
         true },
        {                 "2001:db8::1:0:0:1",
         {0x20,
         0x01,
         0x0d,
         0xb8,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x01,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x01},
         true },
        {                      "2001:41c0::1",
         {0x20,
         0x01,
         0x41,
         0xc0,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x01},
         true },
        {                           "2606::1",
         {0x26,
         0x06,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x01},
         true },
        {                           "1000::1",
         {0x10,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x01},
         true },
        {                               "::1",
         {0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x01},
         true },
        {                                "::",
         {0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00},
         true }
    };

    enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

    for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
        BSLS_LOG_DEBUG("-------");
        BSLS_LOG_DEBUG("Parsing  = %s", DATA[i].text);

        bsl::string text = DATA[i].text;

        ntsa::Ipv6Address address1;
        bool              success = address1.parse(text);

        if (success) {
            bsl::stringstream ss;
            ss << address1;

            BSLS_LOG_DEBUG("Address1 = %s", ss.str().c_str());
        }

        ntsa::Ipv6Address address2;
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

void Ipv6AddressTest::verifyParsingScopeId()
{
    {
        ntsa::Ipv6Address address;

        bool valid = address.parse("::1%1");
        NTSCFG_TEST_ASSERT(valid);

        {
            bsl::stringstream ss;
            ss << address;

            BSLS_LOG_DEBUG("Address = %s", ss.str().c_str());
        }

        NTSCFG_TEST_ASSERT(address.scopeId() == 1);
    }

    {
        ntsa::Ipv6Address address;

        bool valid = address.parse("2001:41c0::645:a65e:60ff:feda:589d%4");
        NTSCFG_TEST_ASSERT(valid);

        {
            bsl::stringstream ss;
            ss << address;

            BSLS_LOG_DEBUG("Address = %s", ss.str().c_str());
        }

        NTSCFG_TEST_ASSERT(address.scopeId() == 4);
    }
}

void Ipv6AddressTest::verifyParsingAbbreviation()
{
    {
        bslstl::StringRef INPUT("1:2:3:4:5:6:7:8");

        ntsa::Ipv6Address address;
        bool              result = address.parse(INPUT);
        NTSCFG_TEST_TRUE(result);

        char buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        address.format(buffer, sizeof buffer, false);

        BSLS_LOG_DEBUG("Output = %s", buffer);
    }

    // Collapse 1

    {
        bslstl::StringRef INPUT("1::3:4:5:6:7:8");

        ntsa::Ipv6Address address;
        bool              result = address.parse(INPUT);
        NTSCFG_TEST_TRUE(result);

        char buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        address.format(buffer, sizeof buffer, false);

        BSLS_LOG_DEBUG("Output = %s", buffer);
    }

    {
        bslstl::StringRef INPUT("1:2::4:5:6:7:8");

        ntsa::Ipv6Address address;
        bool              result = address.parse(INPUT);
        NTSCFG_TEST_TRUE(result);

        char buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        address.format(buffer, sizeof buffer, false);

        BSLS_LOG_DEBUG("Output = %s", buffer);
    }

    // Collapse 2

    {
        bslstl::StringRef INPUT("1::4:5:6:7:8");

        ntsa::Ipv6Address address;
        bool              result = address.parse(INPUT);
        NTSCFG_TEST_TRUE(result);

        char buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        address.format(buffer, sizeof buffer, false);

        BSLS_LOG_DEBUG("Output = %s", buffer);
    }

    {
        bslstl::StringRef INPUT("1:2::5:6:7:8");

        ntsa::Ipv6Address address;
        bool              result = address.parse(INPUT);
        NTSCFG_TEST_TRUE(result);

        char buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        address.format(buffer, sizeof buffer, false);

        BSLS_LOG_DEBUG("Output = %s", buffer);
    }

    {
        bslstl::StringRef INPUT("::");

        ntsa::Ipv6Address address;
        bool              result = address.parse(INPUT);
        NTSCFG_TEST_TRUE(result);

        char buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        address.format(buffer, sizeof buffer, false);

        BSLS_LOG_DEBUG("Output = %s", buffer);
    }

    {
        bslstl::StringRef INPUT("::1");

        ntsa::Ipv6Address address;
        bool              result = address.parse(INPUT);
        NTSCFG_TEST_TRUE(result);

        char buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
        address.format(buffer, sizeof buffer, false);

        BSLS_LOG_DEBUG("Output = %s", buffer);
    }
}

void Ipv6AddressTest::verifyHashing()
{
    ntsa::Ipv6Address address1("2606:2800:220:1:248:1893:25c8:1946");
    ntsa::Ipv6Address address2("2001:41c0::1");

    bsl::unordered_set<ntsa::Ipv6Address> addressSet;
    addressSet.insert(address1);
    addressSet.insert(address2);

    NTSCFG_TEST_ASSERT(addressSet.size() == 2);
}

void Ipv6AddressTest::verifyComparison()
{
}

}  // close namespace ntsa
}  // close namespace BloombergLP

NTSCFG_TEST_SUITE
{
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv6AddressTest::verifyTypeTraits);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv6AddressTest::verifyDefaultConstructor);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv6AddressTest::verifyMoveConstructor);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv6AddressTest::verifyCopyConstructor);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv6AddressTest::verifyOverloadConstructor);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv6AddressTest::verifyCopyAssignment);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv6AddressTest::verifyMoveAssignment);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv6AddressTest::verifyOverloadAssignment);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv6AddressTest::verifyReset);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv6AddressTest::verifyFormatting);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv6AddressTest::verifyParsing);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv6AddressTest::verifyParsingScopeId);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv6AddressTest::verifyParsingAbbreviation);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv6AddressTest::verifyHashing);
    NTSCFG_TEST_FUNCTION(&ntsa::Ipv6AddressTest::verifyComparison);
}
NTSCFG_TEST_SUITE_END;
