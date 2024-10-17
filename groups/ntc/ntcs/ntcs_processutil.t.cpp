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
BSLS_IDENT_RCSID(ntcs_processutil_t_cpp, "$Id$ $CSID$")

#include <ntcs_processutil.h>

#include <ntci_log.h>
#include <ntcs_processstatistics.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::ProcessUtil'.
class ProcessUtilTest
{
    static bsl::string format(double value);

    static void log(const ntcs::ProcessStatistics& statistics);

  public:
    // TODO
    static void verify();
};

bsl::string ProcessUtilTest::format(double value)
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

void ProcessUtilTest::log(const ntcs::ProcessStatistics& statistics)
{
    NTCI_LOG_CONTEXT();

    if (!statistics.memoryResident().isNull()) {
        NTCI_LOG_STREAM_DEBUG << "Real: "
                              << ProcessUtilTest::format(static_cast<double>(
                                     statistics.memoryResident().value()))
                              << NTCI_LOG_STREAM_END;
    }

    if (!statistics.memoryResident().isNull()) {
        NTCI_LOG_STREAM_DEBUG << "Virtual: "
                              << ProcessUtilTest::format(static_cast<double>(
                                     statistics.memoryResident().value()))
                              << NTCI_LOG_STREAM_END;
    }
}

NTSCFG_TEST_FUNCTION(ntcs::ProcessUtilTest::verify)
{
    // Concern:
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("main");

    NTCI_LOG_STREAM_DEBUG << "Initial" << NTCI_LOG_STREAM_END;

    {
        ntcs::ProcessStatistics statistics;
        ntcs::ProcessUtil::getResourceUsage(&statistics);
        ProcessUtilTest::log(statistics);
    }

    bsl::vector<bsl::uint8_t> v1(NTSCFG_TEST_ALLOCATOR);
    v1.resize(256 * 1024);

    NTCI_LOG_STREAM_DEBUG << "Allocated 256K" << NTCI_LOG_STREAM_END;

    {
        ntcs::ProcessStatistics statistics;
        ntcs::ProcessUtil::getResourceUsage(&statistics);
        ProcessUtilTest::log(statistics);
    }

    bsl::vector<bsl::uint8_t> v2(NTSCFG_TEST_ALLOCATOR);
    v1.resize(100 * 1024 * 1024);

    NTCI_LOG_STREAM_DEBUG << "Allocated 100MB" << NTCI_LOG_STREAM_END;

    {
        ntcs::ProcessStatistics statistics;
        ntcs::ProcessUtil::getResourceUsage(&statistics);
        ProcessUtilTest::log(statistics);
    }
}

}  // close namespace ntcs
}  // close namespace BloombergLP
