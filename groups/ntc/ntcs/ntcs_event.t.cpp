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
BSLS_IDENT_RCSID(ntcs_event_t_cpp, "$Id$ $CSID$")

#include <ntcs_event.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::Event'.
class EventTest
{
    // Provide an implementation of the 'ntci::ProactorSocket' interface for
    // use by this test driver.
    class ProactorSocket;

    // Provide an implementation of the 'ntcs::ProactorDetachContext' interface
    // for use by this test driver.
    class ProactorSocketContext;

    // Define a type alias the proactor detachment state.
    typedef ntcs::ProactorDetachState State;

    static const ntsa::Handle k_SOCKET_HANDLE;
    static const bsl::string  k_SOCKET_NAME;

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

const ntsa::Handle EventTest::k_SOCKET_HANDLE = 100;
const bsl::string  EventTest::k_SOCKET_NAME   = "default";

// Provide an implementation of the 'ntci::ProactorSocket' interface for use
// by this test driver.
class EventTest::ProactorSocket : public ntci::ProactorSocket
{
    ntsa::Handle      d_handle;
    const bsl::string d_name;
    bool              d_detached;
    bslma::Allocator* d_allocator_p;

  private:
    ProactorSocket(const ProactorSocket&) BSLS_KEYWORD_DELETED;
    ProactorSocket& operator=(const ProactorSocket&) BSLS_KEYWORD_DELETED;

  public:
    // Create a new proactor socket identified by the specified 'handle' and
    // 'name' for use by this test driver. Optionally specify a
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    // currently installed default allocator is used.
    ProactorSocket(ntsa::Handle       handle,
                   const bsl::string& name,
                   bslma::Allocator*  basicAllocator = 0);

    // Destroy this object.
    ~ProactorSocket() BSLS_KEYWORD_OVERRIDE;

    // Process the completion of socket detachment.
    void processSocketDetached() BSLS_KEYWORD_OVERRIDE;

    // Close the socket.
    void close() BSLS_KEYWORD_OVERRIDE;

    // Return the handle.
    ntsa::Handle handle() const BSLS_KEYWORD_OVERRIDE;

    // Return the name.
    const bsl::string& name() const;

    // Return true if asynchronous detachment is complete, otherwise return
    // false.
    bool isDetached() const;
};

// Provide an implementation of the 'ntcs::ProactorDetachContext' interface
// for use by this test driver.
class EventTest::ProactorSocketContext : public ntcs::ProactorDetachContext
{
  private:
    ProactorSocketContext(const ProactorSocketContext&) BSLS_KEYWORD_DELETED;
    ProactorSocketContext& operator=(const ProactorSocketContext&)
        BSLS_KEYWORD_DELETED;

  public:
    // Create a new I/O completion port context.
    ProactorSocketContext();

    // Destroy this object.
    ~ProactorSocketContext();
};

EventTest::ProactorSocket::ProactorSocket(ntsa::Handle       handle,
                                          const bsl::string& name,
                                          bslma::Allocator*  basicAllocator)
: d_handle(handle)
, d_name(name, basicAllocator)
, d_detached(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EventTest::ProactorSocket::~ProactorSocket()
{
}

void EventTest::ProactorSocket::processSocketDetached()
{
    NTSCFG_TEST_FALSE(d_detached);
    d_detached = true;
}

void EventTest::ProactorSocket::close()
{
}

ntsa::Handle EventTest::ProactorSocket::handle() const
{
    return d_handle;
}

const bsl::string& EventTest::ProactorSocket::name() const
{
    return d_name;
}

bool EventTest::ProactorSocket::isDetached() const
{
    return d_detached;
}

EventTest::ProactorSocketContext::ProactorSocketContext()
: ntcs::ProactorDetachContext()
{
}

EventTest::ProactorSocketContext::~ProactorSocketContext()
{
}

NTSCFG_TEST_FUNCTION(ntcs::EventTest::verifyCase1)
{
    ntsa::Error error;
    bool        result;

    // Create the context.

    ntcs::EventPool eventPool(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<EventTest::ProactorSocketContext> context;
    context.createInplace(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<EventTest::ProactorSocket> socket;
    socket.createInplace(NTSCFG_TEST_ALLOCATOR,
                         EventTest::k_SOCKET_HANDLE,
                         EventTest::k_SOCKET_NAME,
                         NTSCFG_TEST_ALLOCATOR);

    socket->setProactorContext(context);

    NTSCFG_TEST_EQ(context->numProcessors(), 0);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_ATTACHED);

    NTSCFG_TEST_FALSE(socket->isDetached());

    // Detach (complete).

    error = context->detach();
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_EQ(context->numProcessors(), 0);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_DETACHED);

    socket->processSocketDetached();

    NTSCFG_TEST_TRUE(socket->isDetached());
}

NTSCFG_TEST_FUNCTION(ntcs::EventTest::verifyCase2)
{
    ntsa::Error error;
    bool        result;

    // Create the context.

    ntcs::EventPool eventPool(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<EventTest::ProactorSocketContext> context;
    context.createInplace(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<EventTest::ProactorSocket> socket;
    socket.createInplace(NTSCFG_TEST_ALLOCATOR,
                         EventTest::k_SOCKET_HANDLE,
                         EventTest::k_SOCKET_NAME,
                         NTSCFG_TEST_ALLOCATOR);

    socket->setProactorContext(context);

    NTSCFG_TEST_EQ(context->numProcessors(), 0);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_ATTACHED);

    NTSCFG_TEST_FALSE(socket->isDetached());

    // Acquire lease (n = 1).

    bslma::ManagedPtr<ntcs::Event> event =
        eventPool.getManagedObject(socket, context);

    NTSCFG_TEST_TRUE(event);

    NTSCFG_TEST_EQ(event->d_socket, socket);
    NTSCFG_TEST_EQ(event->d_context, context);

    NTSCFG_TEST_EQ(context->numProcessors(), 1);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_ATTACHED);

    // Release lease (n = 0).

    event.reset();

    NTSCFG_TEST_EQ(context->numProcessors(), 0);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_ATTACHED);

    NTSCFG_TEST_FALSE(socket->isDetached());

    // Detach (complete).

    error = context->detach();
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_EQ(context->numProcessors(), 0);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_DETACHED);

    socket->processSocketDetached();

    NTSCFG_TEST_TRUE(socket->isDetached());
}

NTSCFG_TEST_FUNCTION(ntcs::EventTest::verifyCase3)
{
    ntsa::Error error;
    bool        result;

    // Create the context.

    ntcs::EventPool eventPool(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<EventTest::ProactorSocketContext> context;
    context.createInplace(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<EventTest::ProactorSocket> socket;
    socket.createInplace(NTSCFG_TEST_ALLOCATOR,
                         EventTest::k_SOCKET_HANDLE,
                         EventTest::k_SOCKET_NAME,
                         NTSCFG_TEST_ALLOCATOR);

    socket->setProactorContext(context);

    NTSCFG_TEST_EQ(context->numProcessors(), 0);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_ATTACHED);

    NTSCFG_TEST_FALSE(socket->isDetached());

    // Acquire lease (n = 1).

    bslma::ManagedPtr<ntcs::Event> event =
        eventPool.getManagedObject(socket, context);

    NTSCFG_TEST_TRUE(event);

    NTSCFG_TEST_EQ(event->d_socket, socket);
    NTSCFG_TEST_EQ(event->d_context, context);

    NTSCFG_TEST_EQ(context->numProcessors(), 1);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_ATTACHED);

    // Detach (pending).

    error = context->detach();
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_WOULD_BLOCK));

    NTSCFG_TEST_EQ(context->numProcessors(), 1);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_DETACHING);

    NTSCFG_TEST_FALSE(socket->isDetached());

    // Release lease (n = 0, complete).

    event.reset();

    NTSCFG_TEST_EQ(context->numProcessors(), 0);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_DETACHED);

    NTSCFG_TEST_TRUE(socket->isDetached());
}

NTSCFG_TEST_FUNCTION(ntcs::EventTest::verifyCase4)
{
    ntsa::Error error;
    bool        result;

    // Create the context.

    ntcs::EventPool eventPool(NTSCFG_TEST_ALLOCATOR);

    bslma::ManagedPtr<ntcs::Event> event1;
    bslma::ManagedPtr<ntcs::Event> event2;
    bslma::ManagedPtr<ntcs::Event> event3;

    bsl::shared_ptr<EventTest::ProactorSocketContext> context;
    context.createInplace(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<EventTest::ProactorSocket> socket;
    socket.createInplace(NTSCFG_TEST_ALLOCATOR,
                         EventTest::k_SOCKET_HANDLE,
                         EventTest::k_SOCKET_NAME,
                         NTSCFG_TEST_ALLOCATOR);

    socket->setProactorContext(context);

    NTSCFG_TEST_EQ(context->numProcessors(), 0);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_ATTACHED);

    NTSCFG_TEST_FALSE(socket->isDetached());

    // Acquire lease (n = 1).

    event1 = eventPool.getManagedObject(socket, context);

    NTSCFG_TEST_TRUE(event1);

    NTSCFG_TEST_EQ(event1->d_socket, socket);
    NTSCFG_TEST_EQ(event1->d_context, context);

    NTSCFG_TEST_EQ(context->numProcessors(), 1);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_ATTACHED);

    // Release lease (n = 0).

    event1.reset();

    NTSCFG_TEST_EQ(context->numProcessors(), 0);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_ATTACHED);

    NTSCFG_TEST_FALSE(socket->isDetached());

    // Acquire lease (n = 1).

    event1 = eventPool.getManagedObject(socket, context);

    NTSCFG_TEST_TRUE(event1);

    NTSCFG_TEST_EQ(context->numProcessors(), 1);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_ATTACHED);

    // Acquire lease (n = 2).

    event2 = eventPool.getManagedObject(socket, context);

    NTSCFG_TEST_TRUE(event2);

    NTSCFG_TEST_EQ(event2->d_socket, socket);
    NTSCFG_TEST_EQ(event2->d_context, context);

    NTSCFG_TEST_EQ(context->numProcessors(), 2);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_ATTACHED);

    // Release lease (n = 1).

    event1.reset();

    NTSCFG_TEST_EQ(context->numProcessors(), 1);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_ATTACHED);

    NTSCFG_TEST_FALSE(socket->isDetached());

    // Release lease (n = 0).

    event2.reset();

    NTSCFG_TEST_EQ(context->numProcessors(), 0);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_ATTACHED);

    NTSCFG_TEST_FALSE(socket->isDetached());

    // Acquire lease (n = 1).

    event1 = eventPool.getManagedObject(socket, context);

    NTSCFG_TEST_TRUE(event1);

    NTSCFG_TEST_EQ(event1->d_socket, socket);
    NTSCFG_TEST_EQ(event1->d_context, context);

    NTSCFG_TEST_EQ(context->numProcessors(), 1);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_ATTACHED);

    // Acquire lease (n = 2).

    event2 = eventPool.getManagedObject(socket, context);

    NTSCFG_TEST_TRUE(event2);

    NTSCFG_TEST_EQ(event2->d_socket, socket);
    NTSCFG_TEST_EQ(event2->d_context, context);

    NTSCFG_TEST_EQ(context->numProcessors(), 2);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_ATTACHED);

    // Detach (pending).

    error = context->detach();
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_WOULD_BLOCK));

    NTSCFG_TEST_EQ(context->numProcessors(), 2);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_DETACHING);

    NTSCFG_TEST_FALSE(socket->isDetached());

    // Detach (pending, failed).

    error = context->detach();
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));

    NTSCFG_TEST_EQ(context->numProcessors(), 2);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_DETACHING);

    NTSCFG_TEST_FALSE(socket->isDetached());

    // Acquire lease (n = 2, failed).

    event3 = eventPool.getManagedObject(socket, context);

    NTSCFG_TEST_FALSE(event3);

    NTSCFG_TEST_EQ(context->numProcessors(), 2);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_DETACHING);

    NTSCFG_TEST_FALSE(socket->isDetached());

    // Release lease (n = 1).

    event1.reset();

    NTSCFG_TEST_EQ(context->numProcessors(), 1);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_DETACHING);

    NTSCFG_TEST_FALSE(socket->isDetached());

    // Release lease (n = 0, complete).

    event2.reset();

    NTSCFG_TEST_EQ(context->numProcessors(), 0);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_DETACHED);

    NTSCFG_TEST_TRUE(socket->isDetached());

    // Acquire lease (n = 0, failed).

    event3 = eventPool.getManagedObject(socket, context);

    NTSCFG_TEST_FALSE(event3);

    NTSCFG_TEST_EQ(context->numProcessors(), 0);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_DETACHED);

    // Detach (complete, failed).

    error = context->detach();
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));

    NTSCFG_TEST_EQ(context->numProcessors(), 0);
    NTSCFG_TEST_EQ(context->state(), EventTest::State::e_DETACHED);
}

}  // close namespace ntcs
}  // close namespace BloombergLP
