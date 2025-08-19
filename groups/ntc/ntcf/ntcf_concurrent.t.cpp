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
BSLS_IDENT_RCSID(ntcf_concurrent_t_cpp, "$Id$ $CSID$")

#include <ntcf_concurrent.h>
#include <ntcf_test.h>

using namespace BloombergLP;

#if NTC_BUILD_WITH_COROUTINES
namespace BloombergLP {
namespace ntcf {

// Provide tests for 'ntcf::Concurrent'.
class ConcurrentTest
{
    BALL_LOG_SET_CLASS_CATEGORY("NTCF.CONCURRENT");

    // TODO
    static ntsa::CoroutineTask<void> coVerifySandbox(
        const bsl::shared_ptr<ntci::Scheduler>& scheduler,
        ntsa::Allocator                         memory);

  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifySandbox();
};

ntsa::CoroutineTask<void> ConcurrentTest::coVerifySandbox(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    ntsa::Allocator                         memory)
{
    // Get the memory allocator.

    bslma::Allocator* allocator = memory.mechanism();

    // Create a client datagram socket.

    ntca::DatagramSocketOptions clientDatagramSocketOptions;
    clientDatagramSocketOptions.setTransport(
        ntsa::Transport::e_UDP_IPV4_DATAGRAM);

    bsl::shared_ptr<ntci::DatagramSocket> clientDatagramSocket =
        scheduler->createDatagramSocket(clientDatagramSocketOptions,
                                        allocator);

    // Create a server datagram socket.

    ntca::DatagramSocketOptions serverDatagramSocketOptions;
    serverDatagramSocketOptions.setTransport(
        ntsa::Transport::e_UDP_IPV4_DATAGRAM);

    bsl::shared_ptr<ntci::DatagramSocket> serverDatagramSocket =
        scheduler->createDatagramSocket(serverDatagramSocketOptions,
                                        allocator);

    ntsa::AwaitableValue<int> awaitable(static_cast<int>(123));

    int value = co_await awaitable;

    BALL_LOG_DEBUG << "Value = " << value << BALL_LOG_END;

    co_return;
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifyCase1)
{
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifyCase2)
{
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifySandbox)
{
    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Scheduler> scheduler;
    ntcf::System::getDefault(&scheduler);

    ntsa::CoroutineTask<void> task = coVerifySandbox(scheduler, allocator);
    ntsa::CoroutineTaskUtil::synchronize(bsl::move(task));
}

}  // close namespace ntcf
}  // close namespace BloombergLP
#endif
