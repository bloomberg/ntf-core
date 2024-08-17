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

#include <ntsa_ethernetaddress.h>

#include <ntscfg_test.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

using namespace BloombergLP;
using namespace ntsa;

NTSCFG_TEST_CASE(1)
{
    // Concern:
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::EthernetAddress address1;

        address1[0] = 0xb8;
        address1[1] = 0xe6;
        address1[2] = 0x0c;
        address1[3] = 0x06;
        address1[4] = 0x3c;
        address1[5] = 0x7b;

        NTSCFG_TEST_LOG_DEBUG << "Address1 = " << address1 
                              << NTSCFG_TEST_LOG_END;

        char buffer[ntsa::EthernetAddress::MAX_TEXT_LENGTH + 1];
        bsl::memset(buffer, 0, sizeof buffer);

        bsl::size_t n = address1.format(buffer, sizeof buffer);
        NTSCFG_TEST_EQ(n, ntsa::EthernetAddress::MAX_TEXT_LENGTH);

        ntsa::EthernetAddress address2;

        bool result = address2.parse(
            bslstl::StringRef(buffer, ntsa::EthernetAddress::MAX_TEXT_LENGTH));
        NTSCFG_TEST_TRUE(result);

        NTSCFG_TEST_LOG_DEBUG << "Address2 = " << address2 
                              << NTSCFG_TEST_LOG_END;

        NTSCFG_TEST_EQ(address1, address2);

    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
}
NTSCFG_TEST_DRIVER_END;
