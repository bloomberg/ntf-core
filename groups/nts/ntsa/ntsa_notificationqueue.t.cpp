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

#include <ntsa_notificationqueue.h>

#include <bslma_testallocator.h>
#include <bsls_assert.h>

#include <ntscfg_test.h>

using namespace BloombergLP;
using namespace ntsa;

NTSCFG_TEST_CASE(1)
{
    bslma::TestAllocator ta;
    {
        NotificationQueue q(&ta);

        NTSCFG_TEST_EQ(q.handle(), ntsa::k_INVALID_HANDLE);
        NTSCFG_TEST_TRUE(q.notifications().empty());
    }
    BSLS_ASSERT_OPT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(2)
{
    const Handle h = 5;
    Notification n1;
    Notification n2;
    Notification n3;

    n1.makeTimestamp().setType(TimestampType::e_SCHEDULED);
    n2.makeTimestamp().setType(TimestampType::e_SENT);
    n3.makeTimestamp().setType(TimestampType::e_ACKNOWLEDGED);

    bslma::TestAllocator ta;
    {
        NotificationQueue q(&ta);
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
    BSLS_ASSERT_OPT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(3)
{
    const Handle h = 6;
    Notification n1;
    Notification n2;
    n1.makeTimestamp().setType(TimestampType::e_SCHEDULED);
    n2.makeTimestamp().setType(TimestampType::e_ACKNOWLEDGED);

    bslma::TestAllocator ta;
    {
        NotificationQueue q(&ta);
        q.setHandle(h);
        q.addNotification(n1);

        NotificationQueue q2(q, &ta);
        NTSCFG_TEST_EQ(q2, q);

        q2.addNotification(n2);
        NTSCFG_TEST_NE(q2, q);
    }
    BSLS_ASSERT_OPT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(4)
{
    /// NotificationQueue is allocated on stack inside reactor drivers with
    /// help of bdlma::LocalSequentialAllocator. Currently
    /// NotificationQueue::k_NUM_BYTES_TO_ALLOCATE is used as a constant to
    /// allocate enough memory on the stack for the queue. If it happens
    /// that size of ntsa::Notification (and thus size of
    /// ntsa::NotificationQueue) is changed then eiher fix this comment and
    /// comment above NotificationQueue::k_NUM_BYTES_TO_ALLOCATE (number of
    /// notifications should be decreased) or make the constant bigger.
    const int            numNotifications = 32;
    bslma::TestAllocator ta;
    {
        NotificationQueue q(&ta);
        for (int i = 0; i < numNotifications; ++i) {
            Notification n;
            q.addNotification(n);
        }
        NTSCFG_TEST_LE(ta.numBytesInUse(),
                       NotificationQueue::k_NUM_BYTES_TO_ALLOCATE);
    }
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
    NTSCFG_TEST_REGISTER(4);
}
NTSCFG_TEST_DRIVER_END;
