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

#include <ntsa_timestamp.h>

#include <ntscfg_test.h>

using namespace BloombergLP;
using namespace ntsa;

NTSCFG_TEST_CASE(1)
{
    Timestamp t;
    NTSCFG_TEST_EQ(t.type(), TimestampType::e_UNDEFINED);
    NTSCFG_TEST_EQ(t.id(), 0);
    NTSCFG_TEST_EQ(t.time(), bsls::TimeInterval());
}

NTSCFG_TEST_CASE(2)
{
    const bsl::uint32_t        id   = 123;
    const TimestampType::Value type = TimestampType::e_ACKNOWLEDGED;
    const bsls::TimeInterval   time(20, 22);

    Timestamp t;
    t.setId(id);
    t.setType(type);
    t.setTime(time);

    NTSCFG_TEST_EQ(t.type(), type);
    NTSCFG_TEST_EQ(t.id(), id);
    NTSCFG_TEST_EQ(t.time(), time);
}

NTSCFG_TEST_CASE(3)
{
    const bsl::uint32_t        id   = 456;
    const TimestampType::Value type = TimestampType::e_SENT;
    const bsls::TimeInterval   time(15, 20);

    Timestamp t1;
    t1.setId(id);
    t1.setType(type);
    t1.setTime(time);

    Timestamp t2(t1);
    Timestamp t3;

    NTSCFG_TEST_EQ(t2.type(), type);
    NTSCFG_TEST_EQ(t2.id(), id);
    NTSCFG_TEST_EQ(t2.time(), time);

    t3 = t1;
    NTSCFG_TEST_EQ(t2.type(), t3.type());
    NTSCFG_TEST_EQ(t2.id(), t3.id());
    NTSCFG_TEST_EQ(t2.time(), t3.time());
}

NTSCFG_TEST_CASE(4)
{
    const bsl::uint32_t        id   = 456;
    const TimestampType::Value type = TimestampType::e_SENT;
    const bsls::TimeInterval   time(15, 20);

    const bsl::uint32_t        id2   = 789;
    const TimestampType::Value type2 = TimestampType::e_SCHEDULED;
    const bsls::TimeInterval   time2(11, 3);

    Timestamp t1;
    t1.setId(id);
    t1.setType(type);
    t1.setTime(time);

    Timestamp t2;
    t2.setId(id2);
    t2.setType(type2);
    t2.setTime(time2);

    NTSCFG_TEST_NE(t1, t2);

    t2.setId(id);
    NTSCFG_TEST_NE(t1, t2);

    t2.setType(type);
    NTSCFG_TEST_NE(t1, t2);

    t2.setTime(time);
    NTSCFG_TEST_EQ(t1, t2);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
    NTSCFG_TEST_REGISTER(4);
}
NTSCFG_TEST_DRIVER_END;
