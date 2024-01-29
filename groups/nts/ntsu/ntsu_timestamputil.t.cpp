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

#include <ntsu_timestamputil.h>

#include <ntscfg_test.h>

using namespace BloombergLP;

NTSCFG_TEST_CASE(1)
{
    
    bsl::string result;

    {
        bsls::TimeInterval delay;
        delay.setTotalNanoseconds(0);
        result = ntsu::TimestampUtil::describeDelay(delay);
        BSLS_LOG_DEBUG("  0ns: %s", result.c_str());
    }

    {
        bsls::TimeInterval delay;
        delay.setTotalNanoseconds(4);
        result = ntsu::TimestampUtil::describeDelay(delay);
        BSLS_LOG_DEBUG("  4ns: %s", result.c_str());
    }

    {
        bsls::TimeInterval delay;
        delay.setTotalNanoseconds(999);
        result = ntsu::TimestampUtil::describeDelay(delay);
        BSLS_LOG_DEBUG("999ns: %s", result.c_str());
    }

    {
        bsls::TimeInterval delay;
        delay.setTotalMicroseconds(1);
        result = ntsu::TimestampUtil::describeDelay(delay);
        BSLS_LOG_DEBUG("  1us: %s", result.c_str());
    }

    {
        bsls::TimeInterval delay;
        delay.setTotalMicroseconds(4);
        result = ntsu::TimestampUtil::describeDelay(delay);
        BSLS_LOG_DEBUG("  4us: %s", result.c_str());
    }

    {
        bsls::TimeInterval delay;
        delay.setTotalMicroseconds(999);
        result = ntsu::TimestampUtil::describeDelay(delay);
        BSLS_LOG_DEBUG("999us: %s", result.c_str());
    }

    {
        bsls::TimeInterval delay;
        delay.setTotalMilliseconds(1);
        result = ntsu::TimestampUtil::describeDelay(delay);
        BSLS_LOG_DEBUG("  1ms: %s", result.c_str());
    }

    {
        bsls::TimeInterval delay;
        delay.setTotalMilliseconds(1);
        result = ntsu::TimestampUtil::describeDelay(delay);
        BSLS_LOG_DEBUG("  4ms: %s", result.c_str());
    }

    {
        bsls::TimeInterval delay;
        delay.setTotalMilliseconds(999);
        result = ntsu::TimestampUtil::describeDelay(delay);
        BSLS_LOG_DEBUG("999ms: %s", result.c_str());
    }

    {
        bsls::TimeInterval delay;
        delay.setTotalMilliseconds(1500);
        result = ntsu::TimestampUtil::describeDelay(delay);
        BSLS_LOG_DEBUG(" 1.5s: %s", result.c_str());
    }




}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
}
NTSCFG_TEST_DRIVER_END;
