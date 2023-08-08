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
#include <bslmt_latch.h>
#include <bslmt_threadutil.h>

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

/// This class mocks ntci::Executor interface
struct ExecutorMock : public ntci::Executor {
    /// Construct the object
    ExecutorMock();

    /// Destroy this object.
    ~ExecutorMock() BSLS_KEYWORD_NOEXCEPT;

    /// Defer the execution of the specified 'functor'.
    void execute(const Functor& functor) BSLS_KEYWORD_OVERRIDE;

    /// Atomically defer the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE;

    void setExecuteExpected();

  private:
    bool d_executeExpected;
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

ExecutorMock::ExecutorMock()
: d_executeExpected(false)
{
}

ExecutorMock::~ExecutorMock() BSLS_KEYWORD_NOEXCEPT
{
}

void ExecutorMock::execute(const BloombergLP::ntci::Executor::Functor& functor)
{
    NTCCFG_TEST_ASSERT(d_executeExpected && "not expected to be called");
    d_executeExpected = false;
}

void ExecutorMock::moveAndExecute(
    BloombergLP::ntci::Executor::FunctorSequence* functorSequence,
    const BloombergLP::ntci::Executor::Functor&   functor)
{
    NTCCFG_TEST_ASSERT(false && "not expected to be called");
}

void ExecutorMock::setExecuteExpected()
{
    d_executeExpected = true;
}

void testCase5Helper(bslmt::Latch&                latch,
                     int                          iterations,
                     const bsl::function<void()>& functor)
{
    latch.arrive();
    for (int i = 0; i < iterations; ++i) {
        functor();
    }
}

void noop()
{
}

void testCase6Helper(bslmt::Latch&        latch,
                     ntcs::RegistryEntry& entry,
                     int&                 detachPermitted)
{
    latch.arrive();
    if (entry.askForDetachmentAnnouncementPermission()) {
        ++detachPermitted;
    }
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

NTCCFG_TEST_CASE(5)
{
    const ntsa::Handle handle = 22;

    ntccfg::TestAllocator ta;
    {
        ntcs::RegistryEntry entry(handle,
                                  ntca::ReactorEventTrigger::e_LEVEL,
                                  false,
                                  &ta);
        NTCCFG_TEST_EQ(entry.processCounter(), 0);

        entry.addOngoingProcess();
        NTCCFG_TEST_EQ(entry.processCounter(), 1);
        entry.addOngoingProcess();
        NTCCFG_TEST_EQ(entry.processCounter(), 2);

        NTCCFG_TEST_EQ(entry.decrementProcessCounter(), 1);
        NTCCFG_TEST_EQ(entry.processCounter(), 1);

        NTCCFG_TEST_EQ(entry.decrementProcessCounter(), 0);
        NTCCFG_TEST_EQ(entry.processCounter(), 0);

        const int startingCounter = 1000;
        for (int i = 0; i < startingCounter; i++) {
            entry.addOngoingProcess();
        }
        NTCCFG_TEST_EQ(entry.processCounter(), startingCounter);

        const bsl::function<void()> addProcessFunctor =
            NTCCFG_BIND(&ntcs::RegistryEntry::addOngoingProcess, &entry);
        const bsl::function<void()> decrementProcessFunctor =
            NTCCFG_BIND(&ntcs::RegistryEntry::decrementProcessCounter, &entry);

        bslmt::Latch latch(3);

        const int finalCounter = startingCounter + 200;

        bslmt::ThreadUtil::Handle inc = bslmt::ThreadUtil::invalidHandle();
        bslmt::ThreadUtil::create(&inc,
                                  NTCCFG_BIND(Test::testCase5Helper,
                                              bsl::ref<bslmt::Latch>(latch),
                                              finalCounter,
                                              addProcessFunctor));
        NTCCFG_TEST_ASSERT(inc != bslmt::ThreadUtil::invalidHandle());

        bslmt::ThreadUtil::Handle dec1 = bslmt::ThreadUtil::invalidHandle();
        bslmt::ThreadUtil::create(&dec1,
                                  NTCCFG_BIND(Test::testCase5Helper,
                                              bsl::ref<bslmt::Latch>(latch),
                                              startingCounter / 2,
                                              decrementProcessFunctor));
        NTCCFG_TEST_ASSERT(dec1 != bslmt::ThreadUtil::invalidHandle());

        bslmt::ThreadUtil::Handle dec2 = bslmt::ThreadUtil::invalidHandle();
        bslmt::ThreadUtil::create(&dec2,
                                  NTCCFG_BIND(Test::testCase5Helper,
                                              bsl::ref<bslmt::Latch>(latch),
                                              startingCounter / 2,
                                              decrementProcessFunctor));
        NTCCFG_TEST_ASSERT(dec2 != bslmt::ThreadUtil::invalidHandle());

        bslmt::ThreadUtil::join(inc);
        bslmt::ThreadUtil::join(dec1);
        bslmt::ThreadUtil::join(dec2);
        NTCCFG_TEST_EQ(entry.processCounter(), finalCounter);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(6)
{
    const ntsa::Handle handle = 22;

    ntccfg::TestAllocator ta;
    {
        for (int i = 0; i < 100; ++i) {
            ntcs::RegistryEntry entry(handle,
                                      ntca::ReactorEventTrigger::e_LEVEL,
                                      false,
                                      &ta);

            const ntci::SocketDetachedCallback callback;

            entry.setDetachmentRequired(callback);

            int detachPermitted = 0;

            bslmt::Latch latch(3);

            bslmt::ThreadUtil::Handle t1 = bslmt::ThreadUtil::invalidHandle();
            bslmt::ThreadUtil::create(
                &t1,
                NTCCFG_BIND(Test::testCase6Helper,
                            bsl::ref<bslmt::Latch>(latch),
                            bsl::ref<ntcs::RegistryEntry>(entry),
                            bsl::ref<int>(detachPermitted)));
            NTCCFG_TEST_ASSERT(t1 != bslmt::ThreadUtil::invalidHandle());

            bslmt::ThreadUtil::Handle t2 = bslmt::ThreadUtil::invalidHandle();
            bslmt::ThreadUtil::create(
                &t2,
                NTCCFG_BIND(Test::testCase6Helper,
                            bsl::ref<bslmt::Latch>(latch),
                            bsl::ref<ntcs::RegistryEntry>(entry),
                            bsl::ref<int>(detachPermitted)));
            NTCCFG_TEST_ASSERT(t2 != bslmt::ThreadUtil::invalidHandle());

            bslmt::ThreadUtil::Handle t3 = bslmt::ThreadUtil::invalidHandle();
            bslmt::ThreadUtil::create(
                &t3,
                NTCCFG_BIND(Test::testCase6Helper,
                            bsl::ref<bslmt::Latch>(latch),
                            bsl::ref<ntcs::RegistryEntry>(entry),
                            bsl::ref<int>(detachPermitted)));
            NTCCFG_TEST_ASSERT(t3 != bslmt::ThreadUtil::invalidHandle());

            bslmt::ThreadUtil::join(t1);
            bslmt::ThreadUtil::join(t2);
            bslmt::ThreadUtil::join(t3);
            NTCCFG_TEST_EQ(detachPermitted, 1);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(7)
{
    const ntsa::Handle handle = 22;

    ntccfg::TestAllocator ta;
    {
        ntcs::RegistryEntry entry(handle,
                                  ntca::ReactorEventTrigger::e_LEVEL,
                                  false,
                                  &ta);

        bsl::shared_ptr<Test::ExecutorMock> executor =
            bsl::make_shared<Test::ExecutorMock>();

        const ntci::SocketDetachedCallback callback(Test::noop);

        entry.setDetachmentRequired(callback);
        NTCCFG_TEST_TRUE(entry.askForDetachmentAnnouncementPermission());
        executor->setExecuteExpected();
        entry.announceDetached(executor);
        entry.announceDetached(executor);

        entry.clear();
        entry.setDetachmentRequired(callback);
        NTCCFG_TEST_TRUE(entry.askForDetachmentAnnouncementPermission());
        executor->setExecuteExpected();
        entry.announceDetached(executor);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
    NTCCFG_TEST_REGISTER(5);
    NTCCFG_TEST_REGISTER(6);
    NTCCFG_TEST_REGISTER(7);
}
NTCCFG_TEST_DRIVER_END;
