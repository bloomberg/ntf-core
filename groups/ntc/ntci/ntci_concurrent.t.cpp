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
BSLS_IDENT_RCSID(ntci_concurrent_t_cpp, "$Id$ $CSID$")

#include <ntci_concurrent.h>

using namespace BloombergLP;

#if NTC_BUILD_WITH_COROUTINES
namespace BloombergLP {
namespace ntci {

// Provide tests for 'ntci::Concurrent'.
class ConcurrentTest
{
    BALL_LOG_SET_CLASS_CATEGORY("NTCI.CONCURRENT");

    // TODO
    static ntsa::CoroutineTask<void> coVerifySandbox(
        ntsa::Allocator allocator);

  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifySandbox();
};

ntsa::CoroutineTask<void> ConcurrentTest::coVerifySandbox(
    ntsa::Allocator allocator)
{
    ntsa::AwaitableValue<int> awaitable(static_cast<int>(123));

    int value = co_await awaitable;

    BALL_LOG_DEBUG << "Value = " << value << BALL_LOG_END;

    co_return;
}

NTSCFG_TEST_FUNCTION(ntci::ConcurrentTest::verifyCase1)
{
}

NTSCFG_TEST_FUNCTION(ntci::ConcurrentTest::verifyCase2)
{
}

NTSCFG_TEST_FUNCTION(ntci::ConcurrentTest::verifySandbox)
{
    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    coVerifySandbox(allocator);

    BloombergLP::ntscfg::TestAllocator* testAllocator =
        dynamic_cast<BloombergLP::ntscfg::TestAllocator*>(
            NTSCFG_TEST_ALLOCATOR);

    if (testAllocator != 0) {
        BALL_LOG_DEBUG << "Allocator:"
                       << "\n    Blocks in use: "
                       << testAllocator->numBlocksInUse() << BALL_LOG_END;
    }
}

}  // close namespace ntci
}  // close namespace BloombergLP
#endif
