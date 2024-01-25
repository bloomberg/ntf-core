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

#include <ntsa_zerocopy.h>

#include <ntscfg_test.h>

using namespace BloombergLP;
using namespace ntsa;

NTSCFG_TEST_CASE(1)
{
    {
        ZeroCopy zc;
        NTSCFG_TEST_EQ(zc.from(), 0);
        NTSCFG_TEST_EQ(zc.thru(), 0);
        NTSCFG_TEST_EQ(zc.type(), ntsa::ZeroCopyType::e_AVOIDED);
    }
    {
        const bsl::uint32_t             from = 5;
        const bsl::uint32_t             thru = 15;
        const ntsa::ZeroCopyType::Value type = ntsa::ZeroCopyType::e_DEFERRED;

        ZeroCopy zc(from, thru, type);
        NTSCFG_TEST_EQ(zc.from(), from);
        NTSCFG_TEST_EQ(zc.thru(), thru);
        NTSCFG_TEST_EQ(zc.type(), type);
    }
    {
        const bsl::uint32_t             from = 10;
        const bsl::uint32_t             thru = 22;
        const ntsa::ZeroCopyType::Value type = ntsa::ZeroCopyType::e_DEFERRED;
        
        ZeroCopy zc;
        zc.setFrom(from);
        zc.setThru(thru);
        zc.setType(type);
        NTSCFG_TEST_EQ(zc.from(), from);
        NTSCFG_TEST_EQ(zc.thru(), thru);
        NTSCFG_TEST_EQ(zc.type(), type);

        ZeroCopy copy(zc);
        NTSCFG_TEST_EQ(copy, zc);
    }
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
}
NTSCFG_TEST_DRIVER_END;
