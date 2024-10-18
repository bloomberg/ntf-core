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
BSLS_IDENT_RCSID(ntcs_registry_t_cpp, "$Id$ $CSID$")

#include <ntcs_registry.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::Registry'.
class RegistryTest
{
    /// This class mocks ntci::ReactorSocket interface and is used to validate
    /// how processNotifications method is called.
    class ReactorSocketMock;

    /// This is a functor which is used to test how ReactorNotificationCallback is
    /// called.
    class NotificationCallbackMock;

    /// This class mocks ntci::Executor interface
    class ExecutorMock;

    /// Returns random NotificationQueue.
    static ntsa::NotificationQueue generate();

    static void noop();

    static void testCase5Helper(bslmt::Latch&                latch,
                                int                          iterations,
                                const bsl::function<void()>& functor);

    static void testCase6Helper(
        bslmt::Latch&                                      latch,
        ntcs::RegistryEntry&                               entry,
        const bsl::shared_ptr<RegistryTest::ExecutorMock>& executor);

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

/// This class mocks ntci::ReactorSocket interface and is used to validate how
/// processNotifications method is called.
class RegistryTest::ReactorSocketMock : public ntci::ReactorSocket
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
class RegistryTest::NotificationCallbackMock
{
  public:
    /// D-tor, checks that d_notifications does not exist.
    ~NotificationCallbackMock();

    /// Operator saves the specified 'notifications' as d_notifications.
    void operator()(const ntsa::NotificationQueue& notifications);

    /// Validate that the specified 'notifications' equals to d_notifications.
    void validateNotifications(const ntsa::NotificationQueue& notifications);

    bdlb::NullableValue<ntsa::NotificationQueue> d_notifications;
};

/// This class mocks ntci::Executor interface
class RegistryTest::ExecutorMock : public ntci::Executor
{
  public:
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

RegistryTest::ReactorSocketMock::ReactorSocketMock(ntsa::Handle handle)
: d_handle(handle)
{
}

RegistryTest::ReactorSocketMock::~ReactorSocketMock()
{
    NTSCFG_TEST_FALSE(d_notifications.has_value());
}

ntsa::Handle RegistryTest::ReactorSocketMock::handle() const
{
    return d_handle;
}

void RegistryTest::ReactorSocketMock::close()
{
    NTSCFG_TEST_ASSERT(false);
}

void RegistryTest::ReactorSocketMock::processNotifications(
    const ntsa::NotificationQueue& notifications)
{
    NTSCFG_TEST_FALSE(d_notifications.has_value());
    d_notifications = notifications;
}

void RegistryTest::ReactorSocketMock::validateNotifications(
    const ntsa::NotificationQueue& notifications)
{
    NTSCFG_TEST_EQ(d_notifications, notifications);
    d_notifications.reset();
}

RegistryTest::NotificationCallbackMock::~NotificationCallbackMock()
{
    NTSCFG_TEST_FALSE(d_notifications.has_value());
}

void RegistryTest::NotificationCallbackMock::operator()(
    const ntsa::NotificationQueue& notifications)
{
    NTSCFG_TEST_FALSE(d_notifications.has_value());
    d_notifications = notifications;
}

void RegistryTest::NotificationCallbackMock::validateNotifications(
    const ntsa::NotificationQueue& notifications)
{
    NTSCFG_TEST_EQ(d_notifications, notifications);
    d_notifications.reset();
}

RegistryTest::ExecutorMock::ExecutorMock()
: d_executeExpected(false)
{
}

RegistryTest::ExecutorMock::~ExecutorMock() BSLS_KEYWORD_NOEXCEPT
{
}

void RegistryTest::ExecutorMock::execute(
    const BloombergLP::ntci::Executor::Functor& functor)
{
    NTSCFG_TEST_ASSERT(d_executeExpected && "not expected to be called");
    d_executeExpected = false;
}

void RegistryTest::ExecutorMock::moveAndExecute(
    BloombergLP::ntci::Executor::FunctorSequence* functorSequence,
    const BloombergLP::ntci::Executor::Functor&   functor)
{
    NTSCFG_TEST_ASSERT(false && "not expected to be called");
}

void RegistryTest::ExecutorMock::setExecuteExpected()
{
    d_executeExpected = true;
}

ntsa::NotificationQueue RegistryTest::generate()
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

void RegistryTest::noop()
{
}

void RegistryTest::testCase5Helper(bslmt::Latch&                latch,
                                   int                          iterations,
                                   const bsl::function<void()>& functor)
{
    latch.arriveAndWait();
    for (int i = 0; i < iterations; ++i) {
        functor();
    }
}

void RegistryTest::testCase6Helper(
    bslmt::Latch&                                      latch,
    ntcs::RegistryEntry&                               entry,
    const bsl::shared_ptr<RegistryTest::ExecutorMock>& executor)
{
    latch.arriveAndWait();
    entry.announceDetached(executor);
}

NTSCFG_TEST_FUNCTION(ntcs::RegistryTest::verifyCase1)
{
    // Concern: test hideNotifications, wantNotifications and showNotifications
    // methods.

    const ntsa::Handle handle = 5;

    bsl::shared_ptr<RegistryTest::ReactorSocketMock> socket;
    socket.createInplace(NTSCFG_TEST_ALLOCATOR, handle);

    ntcs::RegistryEntry re(socket,
                           ntca::ReactorEventTrigger::e_LEVEL,
                           false,
                           NTSCFG_TEST_ALLOCATOR);
    NTSCFG_TEST_TRUE(re.wantNotifications());

    const ntcs::Interest interest = re.hideNotifications();
    ntcs::Interest       expected;
    expected.hideNotifications();
    NTSCFG_TEST_EQ(interest, expected);
    NTSCFG_TEST_FALSE(re.wantNotifications());
}

NTSCFG_TEST_FUNCTION(ntcs::RegistryTest::verifyCase2)
{
    // Concern: test announceNotifications
    const ntsa::Handle handle = 5;

    bsl::shared_ptr<RegistryTest::ReactorSocketMock> socket;
    socket.createInplace(NTSCFG_TEST_ALLOCATOR, handle);

    ntcs::RegistryEntry re(socket,
                           ntca::ReactorEventTrigger::e_LEVEL,
                           false,
                           NTSCFG_TEST_ALLOCATOR);
    NTSCFG_TEST_TRUE(re.wantNotifications());

    const ntsa::NotificationQueue notifications = RegistryTest::generate();

    re.announceNotifications(notifications);
    socket->validateNotifications(notifications);
}

NTSCFG_TEST_FUNCTION(ntcs::RegistryTest::verifyCase3)
{
    // Concern: test hidden notifications are not announced.

    const ntsa::Handle handle = 5;

    bsl::shared_ptr<RegistryTest::ReactorSocketMock> socket;
    socket.createInplace(NTSCFG_TEST_ALLOCATOR, handle);

    ntcs::RegistryEntry re(socket,
                           ntca::ReactorEventTrigger::e_LEVEL,
                           false,
                           NTSCFG_TEST_ALLOCATOR);
    NTSCFG_TEST_TRUE(re.wantNotifications());

    re.hideNotifications();

    const ntsa::NotificationQueue notifications = RegistryTest::generate();
    re.announceNotifications(notifications);
}

NTSCFG_TEST_FUNCTION(ntcs::RegistryTest::verifyCase4)
{
    // Concern: test notificationCallback is used.

    const ntsa::Handle handle = 5;

    ntcs::RegistryEntry re(handle,
                           ntca::ReactorEventTrigger::e_LEVEL,
                           false,
                           NTSCFG_TEST_ALLOCATOR);
    NTSCFG_TEST_TRUE(re.wantNotifications());

    {
        // Nothing should break, but no callback is installed
        const ntsa::NotificationQueue notifications = RegistryTest::generate();
        re.announceNotifications(notifications);
    }

    {
        RegistryTest::NotificationCallbackMock callbackMock;
        ntci::ReactorNotificationCallback      callback(
            NTCCFG_BIND(&RegistryTest::NotificationCallbackMock::operator(),
                        &callbackMock,
                        NTCCFG_BIND_PLACEHOLDER_1));

        re.showNotificationsCallback(callback);
        const ntsa::NotificationQueue notifications = RegistryTest::generate();
        re.announceNotifications(notifications);
        callbackMock.validateNotifications(notifications);
    }

    {
        re.hideNotificationsCallback();
        NTSCFG_TEST_FALSE(re.wantNotifications());

        const ntsa::NotificationQueue notifications = RegistryTest::generate();
        ;
        re.announceNotifications(notifications);
    }

    {
        RegistryTest::NotificationCallbackMock callbackMock;
        ntci::ReactorNotificationCallback      callback(
            NTCCFG_BIND(&RegistryTest::NotificationCallbackMock::operator(),
                        &callbackMock,
                        NTCCFG_BIND_PLACEHOLDER_1));

        re.showNotificationsCallback(callback);
        re.hideNotifications();  //callback is still installed
        NTSCFG_TEST_FALSE(re.wantNotifications());
        const ntsa::NotificationQueue notifications = RegistryTest::generate();
        re.announceNotifications(notifications);
    }
}

NTSCFG_TEST_FUNCTION(ntcs::RegistryTest::verifyCase5)
{
    const ntsa::Handle handle = 22;

    ntcs::RegistryEntry entry(handle,
                              ntca::ReactorEventTrigger::e_LEVEL,
                              false,
                              NTSCFG_TEST_ALLOCATOR);
    NTSCFG_TEST_EQ(entry.processCounter(), 0);
    NTSCFG_TEST_FALSE(entry.isProcessing());

    entry.incrementProcessCounter();
    NTSCFG_TEST_EQ(entry.processCounter(), 1);
    NTSCFG_TEST_TRUE(entry.isProcessing());
    entry.incrementProcessCounter();
    NTSCFG_TEST_EQ(entry.processCounter(), 2);
    NTSCFG_TEST_TRUE(entry.isProcessing());

    NTSCFG_TEST_EQ(entry.decrementProcessCounter(), 1);
    NTSCFG_TEST_EQ(entry.processCounter(), 1);
    NTSCFG_TEST_TRUE(entry.isProcessing());

    NTSCFG_TEST_EQ(entry.decrementProcessCounter(), 0);
    NTSCFG_TEST_EQ(entry.processCounter(), 0);
    NTSCFG_TEST_FALSE(entry.isProcessing());

    const int startingCounter = 1000;
    for (int i = 0; i < startingCounter; i++) {
        entry.incrementProcessCounter();
    }
    NTSCFG_TEST_EQ(entry.processCounter(), startingCounter);

    const bsl::function<void()> addProcessFunctor =
        NTCCFG_BIND(&ntcs::RegistryEntry::incrementProcessCounter, &entry);
    const bsl::function<void()> decrementProcessFunctor =
        NTCCFG_BIND(&ntcs::RegistryEntry::decrementProcessCounter, &entry);

    bslmt::Latch latch(3);

    const int finalCounter = startingCounter + 200;

    bslmt::ThreadUtil::Handle inc = bslmt::ThreadUtil::invalidHandle();
    bslmt::ThreadUtil::create(&inc,
                              NTCCFG_BIND(RegistryTest::testCase5Helper,
                                          bsl::ref<bslmt::Latch>(latch),
                                          finalCounter,
                                          addProcessFunctor));
    NTSCFG_TEST_ASSERT(inc != bslmt::ThreadUtil::invalidHandle());

    bslmt::ThreadUtil::Handle dec1 = bslmt::ThreadUtil::invalidHandle();
    bslmt::ThreadUtil::create(&dec1,
                              NTCCFG_BIND(RegistryTest::testCase5Helper,
                                          bsl::ref<bslmt::Latch>(latch),
                                          startingCounter / 2,
                                          decrementProcessFunctor));
    NTSCFG_TEST_ASSERT(dec1 != bslmt::ThreadUtil::invalidHandle());

    bslmt::ThreadUtil::Handle dec2 = bslmt::ThreadUtil::invalidHandle();
    bslmt::ThreadUtil::create(&dec2,
                              NTCCFG_BIND(RegistryTest::testCase5Helper,
                                          bsl::ref<bslmt::Latch>(latch),
                                          startingCounter / 2,
                                          decrementProcessFunctor));
    NTSCFG_TEST_ASSERT(dec2 != bslmt::ThreadUtil::invalidHandle());

    bslmt::ThreadUtil::join(inc);
    bslmt::ThreadUtil::join(dec1);
    bslmt::ThreadUtil::join(dec2);
    NTSCFG_TEST_EQ(entry.processCounter(), finalCounter);
}

NTSCFG_TEST_FUNCTION(ntcs::RegistryTest::verifyCase6)
{
    const ntsa::Handle handle = 22;

    for (int i = 0; i < 100; ++i) {
        ntcs::RegistryEntry entry(handle,
                                  ntca::ReactorEventTrigger::e_LEVEL,
                                  false,
                                  NTSCFG_TEST_ALLOCATOR);

        const ntci::SocketDetachedCallback callback;

        entry.setDetachmentRequired(callback);

        bslmt::Latch latch(3);

        bsl::shared_ptr<RegistryTest::ExecutorMock> executor =
            bsl::make_shared<RegistryTest::ExecutorMock>();
        executor->setExecuteExpected();  //only once

        bslmt::ThreadUtil::Handle t1 = bslmt::ThreadUtil::invalidHandle();
        bslmt::ThreadUtil::create(
            &t1,
            NTCCFG_BIND(RegistryTest::testCase6Helper,
                        bsl::ref<bslmt::Latch>(latch),
                        bsl::ref<ntcs::RegistryEntry>(entry),
                        executor));
        NTSCFG_TEST_ASSERT(t1 != bslmt::ThreadUtil::invalidHandle());

        bslmt::ThreadUtil::Handle t2 = bslmt::ThreadUtil::invalidHandle();
        bslmt::ThreadUtil::create(
            &t2,
            NTCCFG_BIND(RegistryTest::testCase6Helper,
                        bsl::ref<bslmt::Latch>(latch),
                        bsl::ref<ntcs::RegistryEntry>(entry),
                        executor));
        NTSCFG_TEST_ASSERT(t2 != bslmt::ThreadUtil::invalidHandle());

        bslmt::ThreadUtil::Handle t3 = bslmt::ThreadUtil::invalidHandle();
        bslmt::ThreadUtil::create(
            &t3,
            NTCCFG_BIND(RegistryTest::testCase6Helper,
                        bsl::ref<bslmt::Latch>(latch),
                        bsl::ref<ntcs::RegistryEntry>(entry),
                        executor));
        NTSCFG_TEST_ASSERT(t3 != bslmt::ThreadUtil::invalidHandle());

        bslmt::ThreadUtil::join(t1);
        bslmt::ThreadUtil::join(t2);
        bslmt::ThreadUtil::join(t3);
    }
}

NTSCFG_TEST_FUNCTION(ntcs::RegistryTest::verifyCase7)
{
    const ntsa::Handle handle = 22;

    ntcs::RegistryEntry entry(handle,
                              ntca::ReactorEventTrigger::e_LEVEL,
                              false,
                              NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<RegistryTest::ExecutorMock> executor =
        bsl::make_shared<RegistryTest::ExecutorMock>();

    const ntci::SocketDetachedCallback callback(RegistryTest::noop);

    entry.setDetachmentRequired(callback);
    executor->setExecuteExpected();
    NTSCFG_TEST_TRUE(entry.announceDetached(executor));
    NTSCFG_TEST_FALSE(entry.announceDetached(executor));

    entry.clear();
    entry.setDetachmentRequired(callback);
    executor->setExecuteExpected();
    NTSCFG_TEST_TRUE(entry.announceDetached(executor));
    NTSCFG_TEST_FALSE(entry.announceDetached(executor));
}

}  // close namespace ntcs
}  // close namespace BloombergLP
