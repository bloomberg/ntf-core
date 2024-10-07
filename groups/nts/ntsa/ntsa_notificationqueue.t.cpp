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
BSLS_IDENT_RCSID(ntsa_notificationqueue_t_cpp, "$Id$ $CSID$")

#include <ntsa_notificationqueue.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::NotificationQueue'.
class NotificationQueueTest
{
  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifyCase3();
};

NTSCFG_TEST_FUNCTION(ntsa::NotificationQueueTest::verifyCase1)
{
    NotificationQueue q(NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(q.handle(), ntsa::k_INVALID_HANDLE);
    NTSCFG_TEST_TRUE(q.notifications().empty());
}

NTSCFG_TEST_FUNCTION(ntsa::NotificationQueueTest::verifyCase2)
{
    const Handle h = 5;
    Notification n1;
    Notification n2;
    Notification n3;

    n1.makeTimestamp().setType(TimestampType::e_SCHEDULED);
    n2.makeTimestamp().setType(TimestampType::e_SENT);
    n3.makeTimestamp().setType(TimestampType::e_ACKNOWLEDGED);

    NotificationQueue q(NTSCFG_TEST_ALLOCATOR);
    q.setHandle(h);
    q.addNotification(n1);
    q.addNotification(n2);
    q.addNotification(n3);

    NTSCFG_TEST_EQ(q.handle(), h);
    NTSCFG_TEST_EQ(q.notifications().size(), 3);
    NTSCFG_TEST_EQ(q.notifications().at(0), n1);
    NTSCFG_TEST_EQ(q.notifications().at(1), n2);
    NTSCFG_TEST_EQ(q.notifications().at(2), n3);
}

NTSCFG_TEST_FUNCTION(ntsa::NotificationQueueTest::verifyCase3)
{
    const Handle h = 6;
    Notification n1;
    Notification n2;
    n1.makeTimestamp().setType(TimestampType::e_SCHEDULED);
    n2.makeTimestamp().setType(TimestampType::e_ACKNOWLEDGED);

    NotificationQueue q(NTSCFG_TEST_ALLOCATOR);
    q.setHandle(h);
    q.addNotification(n1);

    NotificationQueue q2(q, NTSCFG_TEST_ALLOCATOR);
    NTSCFG_TEST_EQ(q2, q);

    q2.addNotification(n2);
    NTSCFG_TEST_NE(q2, q);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
