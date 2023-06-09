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

#include <ntsa_timestamptype.h>

#include <ntscfg_test.h>

using namespace BloombergLP;
using namespace ntsa;

NTSCFG_TEST_CASE(1)
{
    NTSCFG_TEST_EQ(
        bsl::strcmp(TimestampType::toString(TimestampType::e_UNDEFINED),
                    "UNDEFINED"),
        0);
    NTSCFG_TEST_EQ(
        bsl::strcmp(TimestampType::toString(TimestampType::e_SCHEDULED),
                    "SCHEDULED"),
        0);
    NTSCFG_TEST_EQ(
        bsl::strcmp(TimestampType::toString(TimestampType::e_SENT), "SENT"),
        0);
    NTSCFG_TEST_EQ(
        bsl::strcmp(TimestampType::toString(TimestampType::e_ACKNOWLEDGED),
                    "ACKNOWLEDGED"),
        0);
}

NTSCFG_TEST_CASE(2)
{
    TimestampType::Value v = TimestampType::e_SENT;

    NTSCFG_TEST_EQ(TimestampType::fromInt(&v, -1), -1);
    NTSCFG_TEST_EQ(v, TimestampType::e_SENT);

    NTSCFG_TEST_EQ(TimestampType::fromInt(&v, 0), 0);
    NTSCFG_TEST_EQ(v, TimestampType::e_UNDEFINED);

    NTSCFG_TEST_EQ(TimestampType::fromInt(&v, 1), 0);
    NTSCFG_TEST_EQ(v, TimestampType::e_SCHEDULED);

    NTSCFG_TEST_EQ(TimestampType::fromInt(&v, 2), 0);
    NTSCFG_TEST_EQ(v, TimestampType::e_SENT);

    NTSCFG_TEST_EQ(TimestampType::fromInt(&v, 3), 0);
    NTSCFG_TEST_EQ(v, TimestampType::e_ACKNOWLEDGED);

    NTSCFG_TEST_EQ(TimestampType::fromInt(&v, 4), -1);
    NTSCFG_TEST_EQ(v, TimestampType::e_ACKNOWLEDGED);
}

NTSCFG_TEST_CASE(3)
{
    const bsl::string undefined    = "undefined";
    const bsl::string scheduled    = "scheduled";
    const bsl::string sent         = "sent";
    const bsl::string acknowledged = "acknowledged";
    const bsl::string random       = "random_string";

    TimestampType::Value v = TimestampType::e_SENT;

    NTSCFG_TEST_EQ(TimestampType::fromString(&v, random), -1);
    NTSCFG_TEST_EQ(v, TimestampType::e_SENT);

    NTSCFG_TEST_EQ(TimestampType::fromString(&v, undefined), 0);
    NTSCFG_TEST_EQ(v, TimestampType::e_UNDEFINED);

    NTSCFG_TEST_EQ(TimestampType::fromString(&v, scheduled), 0);
    NTSCFG_TEST_EQ(v, TimestampType::e_SCHEDULED);

    NTSCFG_TEST_EQ(TimestampType::fromString(&v, sent), 0);
    NTSCFG_TEST_EQ(v, TimestampType::e_SENT);

    NTSCFG_TEST_EQ(TimestampType::fromString(&v, acknowledged), 0);
    NTSCFG_TEST_EQ(v, TimestampType::e_ACKNOWLEDGED);
}

NTSCFG_TEST_CASE(4)
{
    bsl::stringstream ss;

    ss << TimestampType::e_SCHEDULED << ", " << TimestampType::e_SENT << ", "
       << TimestampType::e_ACKNOWLEDGED << ", " << TimestampType::e_UNDEFINED;

    NTSCFG_TEST_EQ(ss.str(), "SCHEDULED, SENT, ACKNOWLEDGED, UNDEFINED");
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
    NTSCFG_TEST_REGISTER(4);
}
NTSCFG_TEST_DRIVER_END;
