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
    // Concern:
    // Plan:

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

NTSCFG_TEST_CASE(2)
{
    // Concern:
    // Plan:

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

NTSCFG_TEST_CASE(3)
{
    // Concern:
    // Plan:

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

NTSCFG_TEST_CASE(4)
{
    // Concern:
    // Plan:

    ntsa::Ipv6Address address1("2606:2800:220:1:248:1893:25c8:1946");
    ntsa::Ipv6Address address2("2001:41c0::1");

    bsl::unordered_set<ntsa::Ipv6Address> addressSet;
    addressSet.insert(address1);
    addressSet.insert(address2);

    NTSCFG_TEST_ASSERT(addressSet.size() == 2);
}

NTSCFG_TEST_CASE(5)
{
    // Concern:
    // Plan:

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

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
    NTSCFG_TEST_REGISTER(4);
    NTSCFG_TEST_REGISTER(5);
}
NTSCFG_TEST_DRIVER_END;
