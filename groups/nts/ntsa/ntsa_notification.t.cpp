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

#include <ntsa_notification.h>

#include <ntscfg_test.h>

using namespace BloombergLP;
using namespace ntsa;

NTSCFG_TEST_CASE(1)
{
    const bsl::uint32_t        id     = 12;
    const TimestampType::Value tsType = TimestampType::e_ACKNOWLEDGED;

    Notification n;
    NTSCFG_TEST_TRUE(n.isUndefined());
    NTSCFG_TEST_FALSE(n.isTimestamp());
    NTSCFG_TEST_FALSE(n.isZeroCopy());
    NTSCFG_TEST_EQ(n.type(), NotificationType::e_UNDEFINED);

    Timestamp& ts = n.makeTimestamp();
    NTSCFG_TEST_TRUE(n.isTimestamp());
    NTSCFG_TEST_FALSE(n.isUndefined());
    NTSCFG_TEST_FALSE(n.isZeroCopy());
    NTSCFG_TEST_EQ(n.type(), NotificationType::e_TIMESTAMP);

    ts.setId(id);
    ts.setType(tsType);
    NTSCFG_TEST_EQ(n.timestamp(), ts);
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

    Notification     n;
    const Timestamp& ts = n.makeTimestamp(t);
    NTSCFG_TEST_TRUE(n.isTimestamp());
    NTSCFG_TEST_EQ(ts, t);
    NTSCFG_TEST_EQ(n.timestamp(), t);
    NTSCFG_TEST_EQ(n.type(), NotificationType::e_TIMESTAMP);
}

NTSCFG_TEST_CASE(3)
{
    const bsl::uint32_t        id   = 456;
    const TimestampType::Value type = TimestampType::e_SENT;
    const bsls::TimeInterval   time(15, 20);

    Timestamp t;
    t.setId(id);
    t.setType(type);
    t.setTime(time);

    Notification n;
    n.makeTimestamp(t);

    NTSCFG_TEST_TRUE(n.isTimestamp());

    n.reset();
    NTSCFG_TEST_TRUE(n.isUndefined());
    NTSCFG_TEST_FALSE(n.isTimestamp());
    NTSCFG_TEST_EQ(n.type(), NotificationType::e_UNDEFINED);
}

NTSCFG_TEST_CASE(4)
{
    Notification n1;
    Notification n2;

    NTSCFG_TEST_EQ(n1, n2);
    n1.makeTimestamp();

    NTSCFG_TEST_NE(n1, n2);

    n2.makeZeroCopy();
    NTSCFG_TEST_NE(n1, n2);
}

NTSCFG_TEST_CASE(5)
{
    const bsl::uint32_t        id   = 456;
    const TimestampType::Value type = TimestampType::e_SENT;
    const bsls::TimeInterval   time(15, 20);

    Timestamp t;
    t.setId(id);
    t.setType(type);
    t.setTime(time);

    Notification n;
    n.makeTimestamp(t);

    Notification n2(n);

    NTSCFG_TEST_EQ(n, n2);
    NTSCFG_TEST_EQ(n2.timestamp(), t);
}

NTSCFG_TEST_CASE(6)
{
    Notification n;
    NTSCFG_TEST_TRUE(n.isUndefined());
    NTSCFG_TEST_FALSE(n.isZeroCopy());
    NTSCFG_TEST_EQ(n.type(), NotificationType::e_UNDEFINED);

    ZeroCopy& zc = n.makeZeroCopy();
    NTSCFG_TEST_TRUE(n.isZeroCopy());
    NTSCFG_TEST_FALSE(n.isUndefined());
    NTSCFG_TEST_FALSE(n.isTimestamp());
    NTSCFG_TEST_EQ(n.type(), NotificationType::e_ZERO_COPY);

    zc.setFrom(1);
    zc.setThru(22);
    zc.setType(ntsa::ZeroCopyType::e_DEFERRED);
    NTSCFG_TEST_EQ(n.zeroCopy(), zc);
}

NTSCFG_TEST_CASE(7)
{
    ZeroCopy zc;
    zc.setFrom(1);
    zc.setThru(22);
    zc.setType(ntsa::ZeroCopyType::e_DEFERRED);

    Notification n;
    n.makeZeroCopy(zc);

    NTSCFG_TEST_TRUE(n.isZeroCopy());

    n.reset();
    NTSCFG_TEST_TRUE(n.isUndefined());
    NTSCFG_TEST_FALSE(n.isZeroCopy());
    NTSCFG_TEST_EQ(n.type(), NotificationType::e_UNDEFINED);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
    NTSCFG_TEST_REGISTER(4);
    NTSCFG_TEST_REGISTER(5);
    NTSCFG_TEST_REGISTER(6);
    NTSCFG_TEST_REGISTER(7);
}
NTSCFG_TEST_DRIVER_END;
