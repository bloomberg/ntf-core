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
BSLS_IDENT_RCSID(ntsa_notification_t_cpp, "$Id$ $CSID$")

#include <ntsa_notification.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Notification'.
class NotificationTest
{
  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifyCase3();

    // TODO
    static void verifyCase4();

    // TODO
    static void verifyCase5();

    // TODO
    static void verifyCase6();

    // TODO
    static void verifyCase7();
};

NTSCFG_TEST_FUNCTION(ntsa::NotificationTest::verifyCase1)
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

NTSCFG_TEST_FUNCTION(ntsa::NotificationTest::verifyCase2)
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

NTSCFG_TEST_FUNCTION(ntsa::NotificationTest::verifyCase3)
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

NTSCFG_TEST_FUNCTION(ntsa::NotificationTest::verifyCase4)
{
    Notification n1;
    Notification n2;

    NTSCFG_TEST_EQ(n1, n2);
    n1.makeTimestamp();

    NTSCFG_TEST_NE(n1, n2);

    n2.makeZeroCopy();
    NTSCFG_TEST_NE(n1, n2);
}

NTSCFG_TEST_FUNCTION(ntsa::NotificationTest::verifyCase5)
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

NTSCFG_TEST_FUNCTION(ntsa::NotificationTest::verifyCase6)
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

NTSCFG_TEST_FUNCTION(ntsa::NotificationTest::verifyCase7)
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

}  // close namespace ntsa
}  // close namespace BloombergLP
