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

#include <ntcs_event.h>

#include <ntccfg_test.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

using namespace BloombergLP;

namespace test {

// Provide an implementation of the 'ntci::ProactorSocket' interface for use
// by this test driver.
class ProactorSocket : public ntci::ProactorSocket
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

// Provide a proactor context for an implementation of the 'ntci::Proactor'
// interface implemented using the I/O completion port API.
class ProactorSocketContext : public ntcs::ProactorDetachContext
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

ProactorSocket::ProactorSocket(ntsa::Handle       handle, 
                               const bsl::string& name,
                               bslma::Allocator*  basicAllocator)
: d_handle(handle)
, d_name(name, basicAllocator)
, d_detached(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ProactorSocket::~ProactorSocket()
{
}

void ProactorSocket::processSocketDetached()
{
    NTCCFG_TEST_FALSE(d_detached);
    d_detached = true;
}

void ProactorSocket::close()
{
}

ntsa::Handle ProactorSocket::handle() const
{
    return d_handle;
}

const bsl::string& ProactorSocket::name() const
{
    return d_name;
}

bool ProactorSocket::isDetached() const
{
    return d_detached;
}

ProactorSocketContext::ProactorSocketContext()
: ntcs::ProactorDetachContext()
{
}

ProactorSocketContext::~ProactorSocketContext()
{
}

const ntsa::Handle k_SOCKET_HANDLE = 100;
const bsl::string  k_SOCKET_NAME   = "default";

typedef ntcs::ProactorDetachState State;

} // close namespace test

NTCCFG_TEST_CASE(1)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;
        bool        result;

        // Create the context.

        ntcs::EventPool eventPool(&ta);

        bsl::shared_ptr<test::ProactorSocketContext> context;
        context.createInplace(&ta);

        bsl::shared_ptr<test::ProactorSocket> socket;
        socket.createInplace(
            &ta, test::k_SOCKET_HANDLE, test::k_SOCKET_NAME, &ta);

        socket->setProactorContext(context);

        NTCCFG_TEST_EQ(context->numProcessors(), 0);
        NTCCFG_TEST_EQ(context->state(), test::State::e_ATTACHED);

        NTCCFG_TEST_FALSE(socket->isDetached());

        // Detach (complete).

        error = context->detach();
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_EQ(context->numProcessors(), 0);
        NTCCFG_TEST_EQ(context->state(), test::State::e_DETACHED);

        socket->processSocketDetached();

        NTCCFG_TEST_TRUE(socket->isDetached());
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;
        bool        result;

        // Create the context.

        ntcs::EventPool eventPool(&ta);

        bsl::shared_ptr<test::ProactorSocketContext> context;
        context.createInplace(&ta);

        bsl::shared_ptr<test::ProactorSocket> socket;
        socket.createInplace(
            &ta, test::k_SOCKET_HANDLE, test::k_SOCKET_NAME, &ta);

        socket->setProactorContext(context);

        NTCCFG_TEST_EQ(context->numProcessors(), 0);
        NTCCFG_TEST_EQ(context->state(), test::State::e_ATTACHED);

        NTCCFG_TEST_FALSE(socket->isDetached());

        // Acquire lease (n = 1).

        bslma::ManagedPtr<ntcs::Event> event = 
            eventPool.getManagedObject(socket, context);

        NTCCFG_TEST_TRUE(event);

        NTCCFG_TEST_EQ(event->d_socket, socket);
        NTCCFG_TEST_EQ(event->d_context, context);

        NTCCFG_TEST_EQ(context->numProcessors(), 1);
        NTCCFG_TEST_EQ(context->state(), test::State::e_ATTACHED);

        // Release lease (n = 0).

        event.reset();

        NTCCFG_TEST_EQ(context->numProcessors(), 0);
        NTCCFG_TEST_EQ(context->state(), test::State::e_ATTACHED);

        NTCCFG_TEST_FALSE(socket->isDetached());

        // Detach (complete).

        error = context->detach();
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_EQ(context->numProcessors(), 0);
        NTCCFG_TEST_EQ(context->state(), test::State::e_DETACHED);

        socket->processSocketDetached();

        NTCCFG_TEST_TRUE(socket->isDetached());
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;
        bool        result;

        // Create the context.

        ntcs::EventPool eventPool(&ta);

        bsl::shared_ptr<test::ProactorSocketContext> context;
        context.createInplace(&ta);

        bsl::shared_ptr<test::ProactorSocket> socket;
        socket.createInplace(
            &ta, test::k_SOCKET_HANDLE, test::k_SOCKET_NAME, &ta);

        socket->setProactorContext(context);

        NTCCFG_TEST_EQ(context->numProcessors(), 0);
        NTCCFG_TEST_EQ(context->state(), test::State::e_ATTACHED);

        NTCCFG_TEST_FALSE(socket->isDetached());

        // Acquire lease (n = 1).

        bslma::ManagedPtr<ntcs::Event> event = 
            eventPool.getManagedObject(socket, context);

        NTCCFG_TEST_TRUE(event);

        NTCCFG_TEST_EQ(event->d_socket, socket);
        NTCCFG_TEST_EQ(event->d_context, context);

        NTCCFG_TEST_EQ(context->numProcessors(), 1);
        NTCCFG_TEST_EQ(context->state(), test::State::e_ATTACHED);

        // Detach (pending).

        error = context->detach();
        NTCCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_WOULD_BLOCK));

        NTCCFG_TEST_EQ(context->numProcessors(), 1);
        NTCCFG_TEST_EQ(context->state(), test::State::e_DETACHING);

        NTCCFG_TEST_FALSE(socket->isDetached());

        // Release lease (n = 0, complete).

        event.reset();

        NTCCFG_TEST_EQ(context->numProcessors(), 0);
        NTCCFG_TEST_EQ(context->state(), test::State::e_DETACHED);

        NTCCFG_TEST_TRUE(socket->isDetached());
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;
        bool        result;

        // Create the context.

        ntcs::EventPool eventPool(&ta);

        bslma::ManagedPtr<ntcs::Event> event1;
        bslma::ManagedPtr<ntcs::Event> event2;
        bslma::ManagedPtr<ntcs::Event> event3;

        bsl::shared_ptr<test::ProactorSocketContext> context;
        context.createInplace(&ta);

        bsl::shared_ptr<test::ProactorSocket> socket;
        socket.createInplace(
            &ta, test::k_SOCKET_HANDLE, test::k_SOCKET_NAME, &ta);

        socket->setProactorContext(context);

        NTCCFG_TEST_EQ(context->numProcessors(), 0);
        NTCCFG_TEST_EQ(context->state(), test::State::e_ATTACHED);

        NTCCFG_TEST_FALSE(socket->isDetached());

        // Acquire lease (n = 1).

        event1 = eventPool.getManagedObject(socket, context);

        NTCCFG_TEST_TRUE(event1);

        NTCCFG_TEST_EQ(event1->d_socket, socket);
        NTCCFG_TEST_EQ(event1->d_context, context);

        NTCCFG_TEST_EQ(context->numProcessors(), 1);
        NTCCFG_TEST_EQ(context->state(), test::State::e_ATTACHED);

        // Release lease (n = 0).

        event1.reset();

        NTCCFG_TEST_EQ(context->numProcessors(), 0);
        NTCCFG_TEST_EQ(context->state(), test::State::e_ATTACHED);

        NTCCFG_TEST_FALSE(socket->isDetached());

        // Acquire lease (n = 1).

        event1 = eventPool.getManagedObject(socket, context);

        NTCCFG_TEST_TRUE(event1);

        NTCCFG_TEST_EQ(context->numProcessors(), 1);
        NTCCFG_TEST_EQ(context->state(), test::State::e_ATTACHED);

        // Acquire lease (n = 2).

        event2 = eventPool.getManagedObject(socket, context);

        NTCCFG_TEST_TRUE(event2);

        NTCCFG_TEST_EQ(event2->d_socket, socket);
        NTCCFG_TEST_EQ(event2->d_context, context);

        NTCCFG_TEST_EQ(context->numProcessors(), 2);
        NTCCFG_TEST_EQ(context->state(), test::State::e_ATTACHED);

        // Release lease (n = 1).

        event1.reset();

        NTCCFG_TEST_EQ(context->numProcessors(), 1);
        NTCCFG_TEST_EQ(context->state(), test::State::e_ATTACHED);

        NTCCFG_TEST_FALSE(socket->isDetached());

        // Release lease (n = 0).

        event2.reset();

        NTCCFG_TEST_EQ(context->numProcessors(), 0);
        NTCCFG_TEST_EQ(context->state(), test::State::e_ATTACHED);

        NTCCFG_TEST_FALSE(socket->isDetached());

        // Acquire lease (n = 1).

        event1 = eventPool.getManagedObject(socket, context);

        NTCCFG_TEST_TRUE(event1);

        NTCCFG_TEST_EQ(event1->d_socket, socket);
        NTCCFG_TEST_EQ(event1->d_context, context);

        NTCCFG_TEST_EQ(context->numProcessors(), 1);
        NTCCFG_TEST_EQ(context->state(), test::State::e_ATTACHED);

        // Acquire lease (n = 2).

        event2 = eventPool.getManagedObject(socket, context);

        NTCCFG_TEST_TRUE(event2);

        NTCCFG_TEST_EQ(event2->d_socket, socket);
        NTCCFG_TEST_EQ(event2->d_context, context);

        NTCCFG_TEST_EQ(context->numProcessors(), 2);
        NTCCFG_TEST_EQ(context->state(), test::State::e_ATTACHED);

        // Detach (pending).

        error = context->detach();
        NTCCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_WOULD_BLOCK));

        NTCCFG_TEST_EQ(context->numProcessors(), 2);
        NTCCFG_TEST_EQ(context->state(), test::State::e_DETACHING);

        NTCCFG_TEST_FALSE(socket->isDetached());

        // Detach (pending, failed).

        error = context->detach();
        NTCCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));

        NTCCFG_TEST_EQ(context->numProcessors(), 2);
        NTCCFG_TEST_EQ(context->state(), test::State::e_DETACHING);

        NTCCFG_TEST_FALSE(socket->isDetached());

        // Acquire lease (n = 2, failed).

        event3 = eventPool.getManagedObject(socket, context);

        NTCCFG_TEST_FALSE(event3);

        NTCCFG_TEST_EQ(context->numProcessors(), 2);
        NTCCFG_TEST_EQ(context->state(), test::State::e_DETACHING);

        NTCCFG_TEST_FALSE(socket->isDetached());

        // Release lease (n = 1).

        event1.reset();

        NTCCFG_TEST_EQ(context->numProcessors(), 1);
        NTCCFG_TEST_EQ(context->state(), test::State::e_DETACHING);

        NTCCFG_TEST_FALSE(socket->isDetached());

        // Release lease (n = 0, complete).

        event2.reset();

        NTCCFG_TEST_EQ(context->numProcessors(), 0);
        NTCCFG_TEST_EQ(context->state(), test::State::e_DETACHED);

        NTCCFG_TEST_TRUE(socket->isDetached());

        // Acquire lease (n = 0, failed).

        event3 = eventPool.getManagedObject(socket, context);

        NTCCFG_TEST_FALSE(event3);

        NTCCFG_TEST_EQ(context->numProcessors(), 0);
        NTCCFG_TEST_EQ(context->state(), test::State::e_DETACHED);

        // Detach (complete, failed).

        error = context->detach();
        NTCCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));

        NTCCFG_TEST_EQ(context->numProcessors(), 0);
        NTCCFG_TEST_EQ(context->state(), test::State::e_DETACHED);
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
