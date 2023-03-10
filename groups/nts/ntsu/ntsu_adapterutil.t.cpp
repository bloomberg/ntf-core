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
#include <ntsu_adapterutil.h>
#include <bslma_testallocator.h>

#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace ntsu;

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

namespace test {

void logAdapterList(const bsl::vector<ntsa::Adapter>& adapterList)
{
    const bsl::size_t WN = 40;
    const bsl::size_t WI = 6;
    const bsl::size_t WE = 21;
    const bsl::size_t W4 = ntsa::Ipv4Address::MAX_TEXT_LENGTH + 4;
    const bsl::size_t W6 = ntsa::Ipv6Address::MAX_TEXT_LENGTH + 4;
    const bsl::size_t MC = 15;

    bsl::stringstream ss;
    ss << bsl::left << bsl::setw(WN) << "Name";
    ss << bsl::left << bsl::setw(WI) << "Index";
    ss << bsl::left << bsl::setw(WE) << "Ethernet";
    ss << bsl::left << bsl::setw(W4) << "IPv4";
    ss << bsl::left << bsl::setw(W6) << "IPv6";
    ss << bsl::left << bsl::setw(MC) << "Multicast";
    ss << bsl::endl;

    for (bsl::size_t i = 0; i < adapterList.size(); ++i) {
        const ntsa::Adapter& adapter = adapterList[i];

        bsl::string name            = adapter.name();
        bsl::size_t index           = adapter.index();
        bsl::string ethernetAddress = adapter.ethernetAddress();
        bsl::string ipv4Address;
        bsl::string ipv6Address;
        bsl::string multicast;

        if (!adapter.ipv4Address().isNull()) {
            ipv4Address = adapter.ipv4Address().value().text();
        }
        else {
            ipv4Address = "-";
        }

        if (!adapter.ipv6Address().isNull()) {
            ipv6Address = adapter.ipv6Address().value().text();
        }
        else {
            ipv6Address = "-";
        }

        if (adapter.multicast()) {
            multicast = "UCAST/MCAST";
        }
        else {
            multicast = "UCAST";
        }

        ss << bsl::left << bsl::setw(WN) << name;
        ss << bsl::left << bsl::setw(WI) << index;
        ss << bsl::left << bsl::setw(WE) << ethernetAddress;
        ss << bsl::left << bsl::setw(W4) << ipv4Address;
        ss << bsl::left << bsl::setw(W6) << ipv6Address;
        ss << bsl::left << bsl::setw(MC) << multicast;
        ss << bsl::endl;
    }

    bsl::string adapterListReport = ss.str();

    BSLS_LOG_DEBUG("\nAdapter list:\n%s", adapterListReport.c_str());
}

}  // close namespace test

NTSCFG_TEST_CASE(1)
{
    // Concern:
    // Plan:

    ntscfg::TestAllocator ta;
    {
        bsl::vector<ntsa::Adapter> adapterList;
        ntsu::AdapterUtil::discoverAdapterList(&adapterList);
        test::logAdapterList(adapterList);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(2)
{
    const bool supportsIpv4 = ntsu::AdapterUtil::supportsIpv4();
    NTSCFG_TEST_TRUE(supportsIpv4);

#if defined(BSLS_PLATFORM_OS_UNIX)

    const bool supportsLocalStream = ntsu::AdapterUtil::supportsLocalStream();

#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
    NTSCFG_TEST_TRUE(supportsLocalStream);
#else
    NTSCFG_TEST_FALSE(supportsLocalStream);
#endif

    const bool supportsLocalDatagram =
        ntsu::AdapterUtil::supportsLocalDatagram();

#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
    NTSCFG_TEST_TRUE(supportsLocalDatagram);
#else
    NTSCFG_TEST_FALSE(supportsLocalDatagram);
#endif

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    const bool supportsLocalStream = ntsu::AdapterUtil::supportsLocalStream();

#if NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
    NTSCFG_TEST_TRUE(supportsLocalStream);
#else
    NTSCFG_TEST_FALSE(supportsLocalStream);
#endif

    const bool supportsLocalDatagram =
        ntsu::AdapterUtil::supportsLocalDatagram();
    NTSCFG_TEST_FALSE(supportsLocalDatagram);

#else
#error Unsupported platform
#endif
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
}
NTSCFG_TEST_DRIVER_END;
