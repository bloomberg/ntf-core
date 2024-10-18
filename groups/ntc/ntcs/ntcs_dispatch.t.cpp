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
BSLS_IDENT_RCSID(ntcs_dispatch_t_cpp, "$Id$ $CSID$")

#include <ntcs_dispatch.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::Dispatch'.
class DispatchTest
{
    /// This class mocks ntci::ReactorSocket interface and is used to validate how
    /// processNotifications method is called.
    class ReactorSocketMock;

    /// This class mocks ntci::Strand interface and is used to check how execute
    /// method is executed.
    class StrandMock;

    /// This class mocks the ntci::Proactor interface.
    class ProactorSocketMock;

  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifyCase3();

    // TODO
    static void verifyCase4();
};

/// This class mocks ntci::ReactorSocket interface and is used to validate how
/// processNotifications method is called.
class DispatchTest::ReactorSocketMock : public ntci::ReactorSocket
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
class DispatchTest::StrandMock : public ntci::Strand
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

/// This class mocks the ntci::Proactor interface.
class DispatchTest::ProactorSocketMock : public ntci::ProactorSocket
{
  public:
    /// Construct the object using the specified 'handle'
    ProactorSocketMock(const ntsa::Handle handle);

    /// Destroy this object.q
    ~ProactorSocketMock() BSLS_KEYWORD_NOEXCEPT;

    /// Return the handle.
    ntsa::Handle handle() const BSLS_KEYWORD_OVERRIDE;

    /// Process the completion of socket detachment
    void processSocketDetached() BSLS_KEYWORD_OVERRIDE;

    /// Close the stream socket.
    void close() BSLS_KEYWORD_OVERRIDE;

    void setProcessSocketDetachedExpected();

  private:
    const ntsa::Handle d_handle;
    bool               d_processSocketDetachedExpected;
};

DispatchTest::ReactorSocketMock::ReactorSocketMock(ntsa::Handle handle)
: d_handle(handle)
{
}

DispatchTest::ReactorSocketMock::~ReactorSocketMock()
{
    NTSCFG_TEST_FALSE(d_notifications.has_value());
}

ntsa::Handle DispatchTest::ReactorSocketMock::handle() const
{
    return d_handle;
}

void DispatchTest::ReactorSocketMock::close()
{
    NTSCFG_TEST_ASSERT(false);
}

void DispatchTest::ReactorSocketMock::processNotifications(
    const ntsa::NotificationQueue& notifications)
{
    NTSCFG_TEST_FALSE(d_notifications.has_value());
    d_notifications = notifications;
}

void DispatchTest::ReactorSocketMock::validateNotifications(
    const ntsa::NotificationQueue& notifications)
{
    NTSCFG_TEST_EQ(d_notifications, notifications);
    d_notifications.reset();
}

void DispatchTest::ReactorSocketMock::validateNoNotifications() const
{
    NTSCFG_TEST_FALSE(d_notifications.has_value());
}

DispatchTest::StrandMock::~StrandMock()
{
    NTSCFG_TEST_FALSE(d_functor);
}

void DispatchTest::StrandMock::drain()
{
    NTSCFG_TEST_ASSERT(false);
}

void DispatchTest::StrandMock::clear()
{
    NTSCFG_TEST_ASSERT(false);
}

bool DispatchTest::StrandMock::isRunningInCurrentThread() const
{
    NTSCFG_TEST_ASSERT(false);
    return true;
}

void DispatchTest::StrandMock::execute(const ntci::Executor::Functor& functor)
{
    d_functor = functor;
}

void DispatchTest::StrandMock::moveAndExecute(FunctorSequence* functorSequence,
                                              const Functor&   functor)
{
    NTSCFG_TEST_ASSERT(false);
}

void DispatchTest::StrandMock::checkAndExecuteFunctor()
{
    NTSCFG_TEST_TRUE(d_functor);
    d_functor();
    d_functor = ntci::Executor::Functor();
}

DispatchTest::ProactorSocketMock::ProactorSocketMock(const ntsa::Handle handle)
: d_handle(handle)
, d_processSocketDetachedExpected(false)
{
}

DispatchTest::ProactorSocketMock::~ProactorSocketMock() BSLS_KEYWORD_NOEXCEPT
{
}

ntsa::Handle DispatchTest::ProactorSocketMock::handle() const
{
    return d_handle;
}

void DispatchTest::ProactorSocketMock::processSocketDetached()
{
    NTSCFG_TEST_ASSERT(d_processSocketDetachedExpected && "unexpected call");
    d_processSocketDetachedExpected = false;
}

void DispatchTest::ProactorSocketMock::close()
{
    NTSCFG_TEST_ASSERT(false && "unexpected call");
}

void DispatchTest::ProactorSocketMock::setProcessSocketDetachedExpected()
{
    d_processSocketDetachedExpected = true;
}

NTSCFG_TEST_FUNCTION(ntcs::DispatchTest::verifyCase1)
{
    // Concern: test announceNotifications directly

    const ntsa::Handle handle = 5;
    const int          tsId   = 2;

    bsl::shared_ptr<DispatchTest::ReactorSocketMock> socket;
    socket.createInplace(NTSCFG_TEST_ALLOCATOR, handle);

    ntsa::NotificationQueue queue(NTSCFG_TEST_ALLOCATOR);
    ntsa::Notification      n;
    n.makeTimestamp().setId(tsId);

    ntcs::Dispatch::announceNotifications(socket, queue, NULL);
    socket->validateNotifications(queue);
}

NTSCFG_TEST_FUNCTION(ntcs::DispatchTest::verifyCase2)
{
    // Concern: test announceNotifications via strand

    const ntsa::Handle handle = 5;
    const int          tsId   = 2;

    bsl::shared_ptr<DispatchTest::ReactorSocketMock> socket;
    socket.createInplace(NTSCFG_TEST_ALLOCATOR, handle);

    bsl::shared_ptr<DispatchTest::StrandMock> strand;
    strand.createInplace(NTSCFG_TEST_ALLOCATOR);

    ntsa::NotificationQueue queue(NTSCFG_TEST_ALLOCATOR);
    ntsa::Notification      n;
    n.makeTimestamp().setId(tsId);

    ntcs::Dispatch::announceNotifications(socket, queue, strand);
    socket->validateNoNotifications();

    strand->checkAndExecuteFunctor();
    socket->validateNotifications(queue);
}

NTSCFG_TEST_FUNCTION(ntcs::DispatchTest::verifyCase3)
{
    // Concern: test announceDetached directly

    const ntsa::Handle h = 22;

    bsl::shared_ptr<DispatchTest::ProactorSocketMock> socket;
    socket.createInplace(NTSCFG_TEST_ALLOCATOR, h);

    bsl::shared_ptr<DispatchTest::StrandMock> strand;

    socket->setProcessSocketDetachedExpected();
    ntcs::Dispatch::announceDetached(socket, strand);
}

NTSCFG_TEST_FUNCTION(ntcs::DispatchTest::verifyCase4)
{
    // Concern: test announceDetached via strand

    const ntsa::Handle h = 22;

    bsl::shared_ptr<DispatchTest::ProactorSocketMock> socket;
    socket.createInplace(NTSCFG_TEST_ALLOCATOR, h);

    bsl::shared_ptr<DispatchTest::StrandMock> strand;
    strand.createInplace(NTSCFG_TEST_ALLOCATOR);

    ntcs::Dispatch::announceDetached(socket, strand);

    socket->setProcessSocketDetachedExpected();
    strand->checkAndExecuteFunctor();
}

}  // close namespace ntcs
}  // close namespace BloombergLP
