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
BSLS_IDENT_RCSID(ntsa_ipaddress_t_cpp, "$Id$ $CSID$")

#include <ntsa_ipaddress.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::IpAddress'.
class IpAddressTest
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

NTSCFG_TEST_FUNCTION(ntsa::IpAddressTest::verifyCase1)
{
    ntsa::IpAddress ipAddress("127.0.0.1");
    NTSCFG_TEST_TRUE(ipAddress.isV4());
    NTSCFG_TEST_EQ(ipAddress.v4(), ntsa::Ipv4Address::loopback());
}

NTSCFG_TEST_FUNCTION(ntsa::IpAddressTest::verifyCase2)
{
    ntsa::IpAddress ipAddress("::1");
    NTSCFG_TEST_TRUE(ipAddress.isV6());
    NTSCFG_TEST_EQ(ipAddress.v6(), ntsa::Ipv6Address::loopback());
}

NTSCFG_TEST_FUNCTION(ntsa::IpAddressTest::verifyCase3)
{
    ntsa::IpAddress ipAddress("123.45.67.89");

    const bslstl::StringRef empty;
    NTSCFG_TEST_FALSE(ipAddress.parse(empty));
    NTSCFG_TEST_TRUE(ipAddress.isUndefined());
}

NTSCFG_TEST_FUNCTION(ntsa::IpAddressTest::verifyCase4)
{
    ntsa::IpAddress ipAddress;

    const bslstl::StringRef ipv4 = "22.44.66.88";
    NTSCFG_TEST_TRUE(ipAddress.parse(ipv4));
    NTSCFG_TEST_FALSE(ipAddress.isUndefined());
    NTSCFG_TEST_TRUE(ipAddress.isV4());
    NTSCFG_TEST_EQ(ipAddress.v4(), ntsa::Ipv4Address(ipv4));
}

NTSCFG_TEST_FUNCTION(ntsa::IpAddressTest::verifyCase5)
{
    ntsa::IpAddress ipAddress;

    const bslstl::StringRef ipv6 = "2001:db8:3333:4444:5555:6666:7777:8888";
    NTSCFG_TEST_TRUE(ipAddress.parse(ipv6));
    NTSCFG_TEST_FALSE(ipAddress.isUndefined());
    NTSCFG_TEST_TRUE(ipAddress.isV6());
    NTSCFG_TEST_EQ(ipAddress.v6(), ntsa::Ipv6Address(ipv6));
}

}  // close namespace ntsa
}  // close namespace BloombergLP
