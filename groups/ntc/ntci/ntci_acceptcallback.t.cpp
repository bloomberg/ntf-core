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

#include <ntci_acceptcallback.h>

#include <ntccfg_test.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_semaphore.h>
#include <bsls_assert.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------

// [ 1]
//-----------------------------------------------------------------------------
// [ 1]
//-----------------------------------------------------------------------------

namespace test {

/// Provide utilities for processing callbacks.
struct AcceptUtil {
    /// Execute the specified 'callback'.
    static void executeAccept(const ntci::AcceptFunction& callback);

    /// Execute the specified 'callback'.
    static void executeAccept(const ntci::AcceptCallback& callback);

    /// Process an accept of the specified 'streamSocket' from the specfied
    /// 'acceptor' according to the specified 'event'.
    static void processAccept(
        const bsl::shared_ptr<ntci::Acceptor>&     acceptor,
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::AcceptEvent&                   event);

    /// Process an accept of the specified 'streamSocket' from the specfied
    /// 'acceptor' according to the specified 'event'.
    static void processAcceptSemaphore(
        const bsl::shared_ptr<ntci::Acceptor>&     acceptor,
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::AcceptEvent&                   event,
        bslmt::Semaphore*                          semaphore);
};

void AcceptUtil::executeAccept(const ntci::AcceptFunction& callback)
{
    bsl::shared_ptr<ntci::Acceptor>     acceptor;
    bsl::shared_ptr<ntci::StreamSocket> streamSocket;
    ntca::AcceptEvent                   event;

    callback(acceptor, streamSocket, event);
}

void AcceptUtil::executeAccept(const ntci::AcceptCallback& callback)
{
    bsl::shared_ptr<ntci::Acceptor>     acceptor;
    bsl::shared_ptr<ntci::StreamSocket> streamSocket;
    ntca::AcceptEvent                   event;

    callback.execute(acceptor, streamSocket, event, ntci::Strand::unknown());
}

void AcceptUtil::processAccept(
    const bsl::shared_ptr<ntci::Acceptor>&     acceptor,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::AcceptEvent&                   event)
{
}

void AcceptUtil::processAcceptSemaphore(
    const bsl::shared_ptr<ntci::Acceptor>&     acceptor,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::AcceptEvent&                   event,
    bslmt::Semaphore*                          semaphore)
{
    semaphore->post();
}

}  // close namespace test

NTCCFG_TEST_CASE(1)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntci::AcceptFunction acceptFunction = &test::AcceptUtil::processAccept;

        test::AcceptUtil::executeAccept(acceptFunction);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        bslmt::Semaphore semaphore;

        ntci::AcceptFunction acceptFunction =
            NTCCFG_BIND(&test::AcceptUtil::processAcceptSemaphore,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        &semaphore);

        test::AcceptUtil::executeAccept(acceptFunction);
        semaphore.wait();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntci::AcceptCallback acceptCallack =
            ntci::AcceptCallback(&test::AcceptUtil::processAccept);

        test::AcceptUtil::executeAccept(acceptCallack);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        bslmt::Semaphore semaphore;

        ntci::AcceptCallback acceptCallback = ntci::AcceptCallback(
            NTCCFG_BIND(&test::AcceptUtil::processAcceptSemaphore,
                        NTCCFG_BIND_PLACEHOLDER_1,
                        NTCCFG_BIND_PLACEHOLDER_2,
                        NTCCFG_BIND_PLACEHOLDER_3,
                        &semaphore));

        test::AcceptUtil::executeAccept(acceptCallback);
        semaphore.wait();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(5)
{
    // Concern:
    // Plan:

#if NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMDAS
    ntccfg::TestAllocator ta;
    {
        ntci::AcceptFunction acceptFunction = &test::AcceptUtil::processAccept;

        test::AcceptUtil::executeAccept(
            [&](auto acceptor, auto streamSocket, auto event) {});
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
#endif
}

NTCCFG_TEST_CASE(6)
{
    // Concern:
    // Plan:

#if NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMDAS
    ntccfg::TestAllocator ta;
    {
        bslmt::Semaphore semaphore;

        test::AcceptUtil::executeAccept(
            [&](auto acceptor, auto streamSocket, auto event) {
                semaphore.post();
            });

        semaphore.wait();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
#endif
}

NTCCFG_TEST_CASE(7)
{
    // Concern:
    // Plan:

#if NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMDAS
    ntccfg::TestAllocator ta;
    {
        ntci::AcceptCallback acceptCallack = ntci::AcceptCallback(
            [&](auto acceptor, auto streamSocket, auto event) {});

        test::AcceptUtil::executeAccept(acceptCallack);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
#endif
}

NTCCFG_TEST_CASE(8)
{
    // Concern:
    // Plan:

#if NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMDAS
    ntccfg::TestAllocator ta;
    {
        bslmt::Semaphore semaphore;

        ntci::AcceptCallback acceptCallback = ntci::AcceptCallback(
            [&](auto acceptor, auto streamSocket, auto event) {
                semaphore.post();
            });

        test::AcceptUtil::executeAccept(acceptCallback);
        semaphore.wait();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
#endif
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
    NTCCFG_TEST_REGISTER(8);
}
NTCCFG_TEST_DRIVER_END;
