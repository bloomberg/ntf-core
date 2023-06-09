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

#include <ntcs_dispatch.h>

#include <ntccfg_test.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

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

    /// Validate that d_notifications does not exist.
    void validateNoNotifications() const;

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

/// This class mocks ntci::Strand interface and is used to check how execute
/// method is executed.
class StrandMock : public ntci::Strand
{
  public:
    /// D-tor, validates that d_functor is not assigned.
    ~StrandMock();

    /// Assumed to be never called.
    void drain() BSLS_KEYWORD_OVERRIDE;

    /// Assumed to be never called.
    void clear() BSLS_KEYWORD_OVERRIDE;

    /// Assumed to be never called.
    bool isRunningInCurrentThread() const BSLS_KEYWORD_OVERRIDE;

    /// This method saves the specified 'functor' as d_functor.
    void execute(const ntci::Executor::Functor& functor) BSLS_KEYWORD_OVERRIDE;

    /// Assumed to be never called.
    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE;

    /// Check that d_functor is assigned and then execute it.
    void checkAndExecuteFunctor();

  private:
    ntci::Executor::Functor d_functor;
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

void ReactorSocketMock::validateNoNotifications() const
{
    NTCCFG_TEST_FALSE(d_notifications.has_value());
}

StrandMock::~StrandMock()
{
    NTCCFG_TEST_FALSE(d_functor);
}

void StrandMock::drain()
{
    NTCCFG_TEST_ASSERT(false);
}

void StrandMock::clear()
{
    NTCCFG_TEST_ASSERT(false);
}

bool StrandMock::isRunningInCurrentThread() const
{
    NTCCFG_TEST_ASSERT(false);
    return true;
}

void StrandMock::execute(const ntci::Executor::Functor& functor)
{
    d_functor = functor;
}

void StrandMock::moveAndExecute(FunctorSequence* functorSequence,
                                const Functor&   functor)
{
    NTCCFG_TEST_ASSERT(false);
}

void StrandMock::checkAndExecuteFunctor()
{
    NTCCFG_TEST_TRUE(d_functor);
    d_functor();
    d_functor = ntci::Executor::Functor();
}

}

NTCCFG_TEST_CASE(1)
{
    // Concern: test announceNotifications directly

    const ntsa::Handle handle = 5;
    const int          tsId   = 2;

    ntccfg::TestAllocator ta;
    {
        bsl::shared_ptr<Test::ReactorSocketMock> socket;
        socket.createInplace(&ta, handle);

        ntsa::NotificationQueue queue(&ta);
        ntsa::Notification      n;
        n.makeTimestamp().setId(tsId);

        ntcs::Dispatch::announceNotifications(socket, queue, NULL);
        socket->validateNotifications(queue);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern: test announceNotifications via strand

    const ntsa::Handle handle = 5;
    const int          tsId   = 2;

    ntccfg::TestAllocator ta;
    {
        bsl::shared_ptr<Test::ReactorSocketMock> socket;
        socket.createInplace(&ta, handle);

        bsl::shared_ptr<Test::StrandMock> strand;
        strand.createInplace(&ta);

        ntsa::NotificationQueue queue(&ta);
        ntsa::Notification      n;
        n.makeTimestamp().setId(tsId);

        ntcs::Dispatch::announceNotifications(socket, queue, strand);
        socket->validateNoNotifications();

        strand->checkAndExecuteFunctor();
        socket->validateNotifications(queue);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
}
NTCCFG_TEST_DRIVER_END;
