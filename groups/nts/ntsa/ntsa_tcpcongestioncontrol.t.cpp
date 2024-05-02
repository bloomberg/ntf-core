// Copyright 2024 Bloomberg Finance L.P.
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

#include <ntsa_tcpcongestioncontrol.h>

#include <ntscfg_test.h>

using namespace BloombergLP;
using namespace ntsa;

NTSCFG_TEST_CASE(1)
{
    ntscfg::TestAllocator ta;
    {
        TcpCongestionControl cc(&ta);
        NTSCFG_TEST_EQ(cc.algorithm(), "");

        {
            const char* name = "some_long_name_here";
            NTSCFG_TEST_OK(cc.setAlgorithmName(name));
            NTSCFG_TEST_EQ(cc.algorithm(), name);
        }
        {
            NTSCFG_TEST_OK(
                cc.setAlgorithm(TcpCongestionControlAlgorithm::e_BBR));
            NTSCFG_TEST_EQ(cc.algorithm(), "bbr");
        }
        {
            cc.reset();
            NTSCFG_TEST_EQ(cc.algorithm(), "");
        }
        {
            TcpCongestionControl cc2(&ta);
            NTSCFG_TEST_OK(
                cc2.setAlgorithm(TcpCongestionControlAlgorithm::e_HYBLA));
            NTSCFG_TEST_NE(cc2, cc);
            NTSCFG_TEST_OK(
                cc.setAlgorithm(TcpCongestionControlAlgorithm::e_HYBLA));
            NTSCFG_TEST_EQ(cc2, cc);
        }
        {
            TcpCongestionControl cc2(&ta);
            NTSCFG_TEST_OK(
                cc2.setAlgorithm(TcpCongestionControlAlgorithm::e_WESTWOOD));
            cc = cc2;
            NTSCFG_TEST_EQ(cc.algorithm(), "westwood");
        }
    }
    NTSCFG_TEST_EQ(ta.numBlocksInUse(), 0);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
}
NTSCFG_TEST_DRIVER_END;
