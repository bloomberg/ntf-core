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

#include <ntsa_notificationtype.h>

#include <ntscfg_test.h>

using namespace BloombergLP;
using namespace ntsa;

NTSCFG_TEST_CASE(1)
{
    NTSCFG_TEST_EQ(
        bsl::strcmp(NotificationType::toString(NotificationType::e_UNDEFINED),
                    "UNDEFINED"),
        0);
    NTSCFG_TEST_EQ(
        bsl::strcmp(NotificationType::toString(NotificationType::e_TIMESTAMP),
                    "TIMESTAMP"),
        0);
    NTSCFG_TEST_EQ(bsl::strcmp(NotificationType::toString(
                                   NotificationType::e_ZERO_COPY),
                               "ZERO_COPY"),
                   0);
}

NTSCFG_TEST_CASE(2)
{
    NotificationType::Value v = NotificationType::e_TIMESTAMP;

    NTSCFG_TEST_EQ(NotificationType::fromInt(&v, -1), -1);
    NTSCFG_TEST_EQ(v, NotificationType::e_TIMESTAMP);

    NTSCFG_TEST_EQ(NotificationType::fromInt(&v, 0), 0);
    NTSCFG_TEST_EQ(v, NotificationType::e_UNDEFINED);

    NTSCFG_TEST_EQ(NotificationType::fromInt(&v, 1), 0);
    NTSCFG_TEST_EQ(v, NotificationType::e_TIMESTAMP);

    NTSCFG_TEST_EQ(NotificationType::fromInt(&v, 2), 0);
    NTSCFG_TEST_EQ(v, NotificationType::e_ZERO_COPY);

    NTSCFG_TEST_EQ(NotificationType::fromInt(&v, 3), -1);
    NTSCFG_TEST_EQ(v, NotificationType::e_ZERO_COPY);
}

NTSCFG_TEST_CASE(3)
{
    const bsl::string undefined = "undefined";
    const bsl::string timestamp = "timestamp";
    const bsl::string zerocopy  = "zero_copy";
    const bsl::string random    = "random_string";

    NotificationType::Value v = NotificationType::e_TIMESTAMP;

    NTSCFG_TEST_EQ(NotificationType::fromString(&v, random), -1);
    NTSCFG_TEST_EQ(v, NotificationType::e_TIMESTAMP);

    NTSCFG_TEST_EQ(NotificationType::fromString(&v, undefined), 0);
    NTSCFG_TEST_EQ(v, NotificationType::e_UNDEFINED);

    NTSCFG_TEST_EQ(NotificationType::fromString(&v, timestamp), 0);
    NTSCFG_TEST_EQ(v, NotificationType::e_TIMESTAMP);

    NTSCFG_TEST_EQ(NotificationType::fromString(&v, zerocopy), 0);
    NTSCFG_TEST_EQ(v, NotificationType::e_ZERO_COPY);
}

NTSCFG_TEST_CASE(4)
{
    bsl::stringstream ss;

    ss << NotificationType::e_TIMESTAMP << ", "
       << NotificationType::e_UNDEFINED << ", "
       << NotificationType::e_ZERO_COPY;

    NTSCFG_TEST_EQ(ss.str(), "TIMESTAMP, UNDEFINED, ZERO_COPY");
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
    NTSCFG_TEST_REGISTER(4);
}
NTSCFG_TEST_DRIVER_END;
