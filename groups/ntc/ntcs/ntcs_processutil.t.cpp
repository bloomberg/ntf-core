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

#include <ntcs_processutil.h>

#include <ntccfg_test.h>
#include <ntci_log.h>
#include <ntcs_processstatistics.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

using namespace BloombergLP;

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

bsl::string format(double value)
{
    bsl::stringstream ss;
    ss << bsl::setprecision(2);
    ss << bsl::fixed;

    if (value < 1024) {
        ss << value;
        return ss.str();
    }

    value /= 1024;

    if (value < 1024) {
        ss << value << "K";
        return ss.str();
    }

    value /= 1024;

    if (value < 1024) {
        ss << value << "M";
        return ss.str();
    }

    value /= 1024;

    if (value < 1024) {
        ss << value << "G";
        return ss.str();
    }

    ss << value << "T";
    return ss.str();
}

void log(const ntcs::ProcessStatistics& statistics)
{
    NTCI_LOG_CONTEXT();

    if (!statistics.memoryResident().isNull()) {
        NTCI_LOG_STREAM_DEBUG << "Real: "
                              << test::format(static_cast<double>(
                                     statistics.memoryResident().value()))
                              << NTCI_LOG_STREAM_END;
    }

    if (!statistics.memoryResident().isNull()) {
        NTCI_LOG_STREAM_DEBUG << "Virtual: "
                              << test::format(static_cast<double>(
                                     statistics.memoryResident().value()))
                              << NTCI_LOG_STREAM_END;
    }
}

}  // close namespace test

NTCCFG_TEST_CASE(1)
{
    // Concern:
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    ntccfg::TestAllocator ta;
    {
        NTCI_LOG_STREAM_DEBUG << "Initial" << NTCI_LOG_STREAM_END;

        {
            ntcs::ProcessStatistics statistics;
            ntcs::ProcessUtil::getResourceUsage(&statistics);
            test::log(statistics);
        }

        bsl::vector<bsl::uint8_t> v1(&ta);
        v1.resize(256 * 1024);

        NTCI_LOG_STREAM_DEBUG << "Allocated 256K" << NTCI_LOG_STREAM_END;

        {
            ntcs::ProcessStatistics statistics;
            ntcs::ProcessUtil::getResourceUsage(&statistics);
            test::log(statistics);
        }

        bsl::vector<bsl::uint8_t> v2(&ta);
        v1.resize(100 * 1024 * 1024);

        NTCI_LOG_STREAM_DEBUG << "Allocated 100MB" << NTCI_LOG_STREAM_END;

        {
            ntcs::ProcessStatistics statistics;
            ntcs::ProcessUtil::getResourceUsage(&statistics);
            test::log(statistics);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
}
NTCCFG_TEST_DRIVER_END;

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
