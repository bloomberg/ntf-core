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

#include <ntsa_ethernetheader.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ethernetheader_t_cpp, "$Id$ $CSID$")

#include <ntscfg_test.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::EthernetHeader'.
class EthernetHeaderTest
{
  public:
    // Test value semantics: type traits.
    static void verifyTypeTraits();

    // Test usage example.
    static void verifyUsage();
};

void EthernetHeaderTest::verifyTypeTraits()
{
    const bool isBitwiseInitializable =
        NTSCFG_TYPE_CHECK_BITWISE_INITIALIZABLE(ntsa::EthernetHeader);

    NTSCFG_TEST_TRUE(isBitwiseInitializable);

    const bool isBitwiseMovable =
        NTSCFG_TYPE_CHECK_BITWISE_MOVABLE(ntsa::EthernetHeader);

    NTSCFG_TEST_TRUE(isBitwiseMovable);

    const bool isBitwiseCopyable =
        NTSCFG_TYPE_CHECK_BITWISE_COPYABLE(ntsa::EthernetHeader);

    NTSCFG_TEST_TRUE(isBitwiseCopyable);
}

void EthernetHeaderTest::verifyUsage()
{
    ntsa::EthernetHeader header;

    header.setSource(ntsa::EthernetAddress("36:2c:a1:55:0c:c0"));
    header.setDestination(ntsa::EthernetAddress("3e:07:f4:b5:bf:48"));
    header.setProtocol(ntsa::EthernetProtocol::e_IPV4);

    NTSCFG_TEST_LOG_DEBUG << "Ethernet = " << header << NTSCFG_TEST_LOG_END;
}

}  // close namespace ntsa
}  // close namespace BloombergLP

NTSCFG_TEST_SUITE
{
    NTSCFG_TEST_FUNCTION(&ntsa::EthernetHeaderTest::verifyTypeTraits);
    NTSCFG_TEST_FUNCTION(&ntsa::EthernetHeaderTest::verifyUsage);
}
NTSCFG_TEST_SUITE_END;
