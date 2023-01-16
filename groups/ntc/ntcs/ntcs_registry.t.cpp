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

#include <ntcs_registry.h>

#include <ntccfg_test.h>
#include <bdlb_random.h>
#include <bslma_testallocator.h>

using namespace BloombergLP;

namespace Test {
/// This class mocks ntci::ReactorSocket interface and is used to validate how
/// processNotifications method is called.
class ReactorSocketMock : public ntci::ReactorSocket
{
  public:
    /// Construct object using the specified 'handle'.
    ReactorSocketMock(ntsa::Handle handle);

    /// D-tor, validates that d_notifications does not exist.
    ~ReactorSocketMock();

    /// Return the handle.
    ntsa::Handle handle() const BSLS_KEYWORD_OVERRIDE;

    /// Assumed to be never called.
    void close() BSLS_KEYWORD_OVERRIDE;

    /// Validate that stored d_notifications equals the specified
    /// 'notifications'.
    void validateNotifications(const ntsa::NotificationQueue& notifications);

  private:
    ReactorSocketMock(const ReactorSocketMock&) BSLS_KEYWORD_DELETED;
    ReactorSocketMock& operator=(const ReactorSocketMock&)
        BSLS_KEYWORD_DELETED;

    /// This overrided method just saves the specified 'notifications'.
    void processNotifications(const ntsa::NotificationQueue& notifications)
        BSLS_KEYWORD_OVERRIDE;

    ntsa::Handle d_handle;

    bdlb::NullableValue<ntsa::NotificationQueue> d_notifications;
};

/// This is a functor which is used to test how ReactorNotificationCallback is
/// called.
struct NotificationCallbackMock {
    /// D-tor, checks that d_notifications does not exist.
    ~NotificationCallbackMock();

    /// Operator saves the specified 'notifications' as d_notifications.
    void operator()(const ntsa::NotificationQueue& notifications);

    /// Validate that the specified 'notifications' equals to d_notifications.
    void validateNotifications(const ntsa::NotificationQueue& notifications);

    bdlb::NullableValue<ntsa::NotificationQueue> d_notifications;
};

/// Generator of randomly filled NotificationQueues.
struct RandomNotificationsGenerator {
    /// Returns random NotificationQueue.
    static ntsa::NotificationQueue generate();
};

ReactorSocketMock::ReactorSocketMock(ntsa::Handle handle)
: d_handle(handle)
{
}

ReactorSocketMock::~ReactorSocketMock()
{
    NTCCFG_TEST_FALSE(d_notifications.has_value());
}

ntsa::Handle ReactorSocketMock::handle() const
{
    return d_handle;
}

void ReactorSocketMock::close()
{
    NTCCFG_TEST_ASSERT(false);
}

void ReactorSocketMock::processNotifications(
    const ntsa::NotificationQueue& notifications)
{
    NTCCFG_TEST_FALSE(d_notifications.has_value());
    d_notifications = notifications;
}

void ReactorSocketMock::validateNotifications(
    const ntsa::NotificationQueue& notifications)
{
    NTCCFG_TEST_EQ(d_notifications, notifications);
    d_notifications.reset();
}

NotificationCallbackMock::~NotificationCallbackMock()
{
    NTCCFG_TEST_FALSE(d_notifications.has_value());
}

void NotificationCallbackMock::operator()(
    const ntsa::NotificationQueue& notifications)
{
    NTCCFG_TEST_FALSE(d_notifications.has_value());
    d_notifications = notifications;
}

void NotificationCallbackMock::validateNotifications(
    const ntsa::NotificationQueue& notifications)
{
    NTCCFG_TEST_EQ(d_notifications, notifications);
    d_notifications.reset();
}

ntsa::NotificationQueue RandomNotificationsGenerator::generate()
{
    static int              seed = 10;
    ntsa::NotificationQueue queue;
    int                     size = (bdlb::Random::generate15(&seed) & 10) + 1;
    for (int i = 0; i < size; ++i) {
        ntsa::Notification n;
        if (bdlb::Random::generate15(&seed) & 1) {
            ntsa::Timestamp& ts = n.makeTimestamp();
            ts.setId(bdlb::Random::generate15(&seed));
        }
        queue.addNotification(n);
    }
    return queue;
}

}

NTCCFG_TEST_CASE(1)
{
    // Concern: test hideNotifications, wantNotifications and showNotifications
    // methods
    const ntsa::Handle    handle = 5;
    ntccfg::TestAllocator ta;
    {
        bsl::shared_ptr<Test::ReactorSocketMock> socket;
        socket.createInplace(&ta, handle);

        ntcs::RegistryEntry re(socket,
                               ntca::ReactorEventTrigger::e_LEVEL,
                               false,
                               &ta);
        NTCCFG_TEST_TRUE(re.wantNotifications());

        const ntcs::Interest interest = re.hideNotifications();
        ntcs::Interest       expected;
        expected.hideNotifications();
        NTCCFG_TEST_EQ(interest, expected);
        NTCCFG_TEST_FALSE(re.wantNotifications());
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern: test announceNotifications
    const ntsa::Handle handle = 5;

    ntccfg::TestAllocator ta;
    {
        bsl::shared_ptr<Test::ReactorSocketMock> socket;
        socket.createInplace(&ta, handle);

        ntcs::RegistryEntry re(socket,
                               ntca::ReactorEventTrigger::e_LEVEL,
                               false,
                               &ta);
        NTCCFG_TEST_TRUE(re.wantNotifications());

        const ntsa::NotificationQueue notifications =
            Test::RandomNotificationsGenerator::generate();

        re.announceNotifications(notifications);
        socket->validateNotifications(notifications);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Concern: test hidden notifications are not announced
    const ntsa::Handle    handle = 5;
    ntccfg::TestAllocator ta;
    {
        bsl::shared_ptr<Test::ReactorSocketMock> socket;
        socket.createInplace(&ta, handle);

        ntcs::RegistryEntry re(socket,
                               ntca::ReactorEventTrigger::e_LEVEL,
                               false,
                               &ta);
        NTCCFG_TEST_TRUE(re.wantNotifications());

        re.hideNotifications();

        const ntsa::NotificationQueue notifications =
            Test::RandomNotificationsGenerator::generate();
        re.announceNotifications(notifications);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    // Concern: test notificationCallback is used
    const ntsa::Handle    handle = 5;
    ntccfg::TestAllocator ta;
    {
        ntcs::RegistryEntry re(handle,
                               ntca::ReactorEventTrigger::e_LEVEL,
                               false,
                               &ta);
        NTCCFG_TEST_TRUE(re.wantNotifications());

        {
            // Nothing should break, but no callback is installed
            const ntsa::NotificationQueue notifications =
                Test::RandomNotificationsGenerator::generate();
            re.announceNotifications(notifications);
        }

        {
            Test::NotificationCallbackMock    callbackMock;
            ntci::ReactorNotificationCallback callback(
                NTCCFG_BIND(&Test::NotificationCallbackMock::operator(),
                            &callbackMock,
                            NTCCFG_BIND_PLACEHOLDER_1));

            re.showNotificationsCallback(callback);
            const ntsa::NotificationQueue notifications =
                Test::RandomNotificationsGenerator::generate();
            re.announceNotifications(notifications);
            callbackMock.validateNotifications(notifications);
        }

        {
            re.hideNotificationsCallback();
            NTCCFG_TEST_FALSE(re.wantNotifications());

            const ntsa::NotificationQueue notifications =
                Test::RandomNotificationsGenerator::generate();
            ;
            re.announceNotifications(notifications);
        }

        {
            Test::NotificationCallbackMock    callbackMock;
            ntci::ReactorNotificationCallback callback(
                NTCCFG_BIND(&Test::NotificationCallbackMock::operator(),
                            &callbackMock,
                            NTCCFG_BIND_PLACEHOLDER_1));

            re.showNotificationsCallback(callback);
            re.hideNotifications();  //callback is still installed
            NTCCFG_TEST_FALSE(re.wantNotifications());
            const ntsa::NotificationQueue notifications =
                Test::RandomNotificationsGenerator::generate();
            re.announceNotifications(notifications);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
}
NTCCFG_TEST_DRIVER_END;
