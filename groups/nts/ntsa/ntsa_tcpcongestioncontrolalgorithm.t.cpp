// Copyright 2023 Bloomberg Finance L.P.
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

#include <ntsa_tcpcongestioncontrolalgorithm.h>

#include <ntscfg_test.h>

using namespace BloombergLP;
using namespace ntsa;

NTSCFG_TEST_CASE(1)
{
    NTSCFG_TEST_EQ(bsl::strcmp(TcpCongestionControlAlgorithm::toString(
                                   TcpCongestionControlAlgorithm::e_RENO),
                               "RENO"),
                   0);
    NTSCFG_TEST_EQ(bsl::strcmp(TcpCongestionControlAlgorithm::toString(
                                   TcpCongestionControlAlgorithm::e_BBR),
                               "BBR"),
                   0);
    NTSCFG_TEST_EQ(bsl::strcmp(TcpCongestionControlAlgorithm::toString(
                                   TcpCongestionControlAlgorithm::e_YEAH),
                               "YEAH"),
                   0);
}

NTSCFG_TEST_CASE(2)
{
    TcpCongestionControlAlgorithm::Value v =
        TcpCongestionControlAlgorithm::e_YEAH;

    NTSCFG_TEST_EQ(TcpCongestionControlAlgorithm::fromInt(&v, -1), -1);
    NTSCFG_TEST_EQ(v, TcpCongestionControlAlgorithm::e_YEAH);

    NTSCFG_TEST_EQ(TcpCongestionControlAlgorithm::fromInt(&v, 0), 0);
    NTSCFG_TEST_EQ(v, TcpCongestionControlAlgorithm::e_RENO);

    NTSCFG_TEST_EQ(TcpCongestionControlAlgorithm::fromInt(&v, 1), 0);
    NTSCFG_TEST_EQ(v, TcpCongestionControlAlgorithm::e_CUBIC);

    NTSCFG_TEST_EQ(TcpCongestionControlAlgorithm::fromInt(&v, 2), 0);
    NTSCFG_TEST_EQ(v, TcpCongestionControlAlgorithm::e_BBR);

    NTSCFG_TEST_EQ(TcpCongestionControlAlgorithm::fromInt(&v, 100), -1);
    NTSCFG_TEST_EQ(v, TcpCongestionControlAlgorithm::e_BBR);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
}
NTSCFG_TEST_DRIVER_END;
