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
BSLS_IDENT_RCSID(ntci_acceptcallback_t_cpp, "$Id$ $CSID$")

#include <ntci_acceptcallback.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntci {

// Provide tests for 'ntci::AcceptCallback'.
class AcceptCallbackTest
{
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

    // TODO
    static void verifyCase8();

  private:
    /// Execute the specified 'callback'.
    static void invoke(const ntci::AcceptFunction& callback);

    /// Execute the specified 'callback'.
    static void invoke(const ntci::AcceptCallback& callback);

    /// Process an accept of the specified 'streamSocket' from the specfied
    /// 'acceptor' according to the specified 'event'.
    static void process(
        const bsl::shared_ptr<ntci::Acceptor>&     acceptor,
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::AcceptEvent&                   event);

    /// Process an accept of the specified 'streamSocket' from the specfied
    /// 'acceptor' according to the specified 'event'.
    static void processSemaphore(
        const bsl::shared_ptr<ntci::Acceptor>&     acceptor,
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::AcceptEvent&                   event,
        bslmt::Semaphore*                          semaphore);
};

NTSCFG_TEST_FUNCTION(ntci::AcceptCallbackTest::verifyCase1)
{
    ntci::AcceptFunction acceptFunction = &AcceptCallbackTest::process;

    AcceptCallbackTest::invoke(acceptFunction);
}

NTSCFG_TEST_FUNCTION(ntci::AcceptCallbackTest::verifyCase2)
{
    bslmt::Semaphore semaphore;

    ntci::AcceptFunction acceptFunction =
        NTCCFG_BIND(&AcceptCallbackTest::processSemaphore,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    &semaphore);

    AcceptCallbackTest::invoke(acceptFunction);
    semaphore.wait();
}

NTSCFG_TEST_FUNCTION(ntci::AcceptCallbackTest::verifyCase3)
{
    ntci::AcceptCallback acceptCallack =
        ntci::AcceptCallback(&AcceptCallbackTest::process);

    AcceptCallbackTest::invoke(acceptCallack);
}

NTSCFG_TEST_FUNCTION(ntci::AcceptCallbackTest::verifyCase4)
{
    bslmt::Semaphore semaphore;

    ntci::AcceptCallback acceptCallback = ntci::AcceptCallback(
        NTCCFG_BIND(&AcceptCallbackTest::processSemaphore,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    NTCCFG_BIND_PLACEHOLDER_3,
                    &semaphore));

    AcceptCallbackTest::invoke(acceptCallback);
    semaphore.wait();
}

NTSCFG_TEST_FUNCTION(ntci::AcceptCallbackTest::verifyCase5)
{
#if NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMDAS

    ntci::AcceptFunction acceptFunction = &AcceptCallbackTest::process;

    AcceptCallbackTest::invoke(
        [&](auto acceptor, auto streamSocket, auto event) {});

#endif
}

NTSCFG_TEST_FUNCTION(ntci::AcceptCallbackTest::verifyCase6)
{
#if NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMDAS

    bslmt::Semaphore semaphore;

    AcceptCallbackTest::invoke(
        [&](auto acceptor, auto streamSocket, auto event) {
            semaphore.post();
        });

    semaphore.wait();

#endif
}

NTSCFG_TEST_FUNCTION(ntci::AcceptCallbackTest::verifyCase7)
{
#if NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMDAS

    ntci::AcceptCallback acceptCallack = ntci::AcceptCallback(
        [&](auto acceptor, auto streamSocket, auto event) {});

    AcceptCallbackTest::invoke(acceptCallack);

#endif
}

NTSCFG_TEST_FUNCTION(ntci::AcceptCallbackTest::verifyCase8)
{
#if NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMDAS

    bslmt::Semaphore semaphore;

    ntci::AcceptCallback acceptCallback = ntci::AcceptCallback(
        [&](auto acceptor, auto streamSocket, auto event) {
            semaphore.post();
        });

    AcceptCallbackTest::invoke(acceptCallback);
    semaphore.wait();

#endif
}

void AcceptCallbackTest::invoke(const ntci::AcceptFunction& callback)
{
    bsl::shared_ptr<ntci::Acceptor>     acceptor;
    bsl::shared_ptr<ntci::StreamSocket> streamSocket;
    ntca::AcceptEvent                   event;

    callback(acceptor, streamSocket, event);
}

void AcceptCallbackTest::invoke(const ntci::AcceptCallback& callback)
{
    bsl::shared_ptr<ntci::Acceptor>     acceptor;
    bsl::shared_ptr<ntci::StreamSocket> streamSocket;
    ntca::AcceptEvent                   event;

    callback.execute(acceptor, streamSocket, event, ntci::Strand::unknown());
}

void AcceptCallbackTest::process(
    const bsl::shared_ptr<ntci::Acceptor>&     acceptor,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::AcceptEvent&                   event)
{
}

void AcceptCallbackTest::processSemaphore(
    const bsl::shared_ptr<ntci::Acceptor>&     acceptor,
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::AcceptEvent&                   event,
    bslmt::Semaphore*                          semaphore)
{
    semaphore->post();
}

}  // close namespace ntci
}  // close namespace BloombergLP
