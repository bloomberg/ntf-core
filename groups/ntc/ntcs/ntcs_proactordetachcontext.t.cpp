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
BSLS_IDENT_RCSID(ntcs_proactordetachcontext_t_cpp, "$Id$ $CSID$")

#include <ntcs_proactordetachcontext.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::ProactorDetachContext'.
class ProactorDetachContextTest
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
};

NTSCFG_TEST_FUNCTION(ntcs::ProactorDetachContextTest::verifyCase1)
{
    ntsa::Error error;
    bool        result;

    // Create the context.

    ntcs::ProactorDetachContext context;

    NTSCFG_TEST_EQ(context.numProcessors(), 0);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Detach (complete).

    error = context.detach();
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_EQ(context.numProcessors(), 0);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHED);
}

NTSCFG_TEST_FUNCTION(ntcs::ProactorDetachContextTest::verifyCase2)
{
    ntsa::Error error;
    bool        result;

    // Create the context.

    ntcs::ProactorDetachContext context;

    NTSCFG_TEST_EQ(context.numProcessors(), 0);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Acquire lease (n = 1).

    result = context.incrementReference();

    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_EQ(context.numProcessors(), 1);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Release lease (n = 0).

    result = context.decrementReference();

    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_EQ(context.numProcessors(), 0);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Detach (complete).

    error = context.detach();
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_EQ(context.numProcessors(), 0);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHED);
}

NTSCFG_TEST_FUNCTION(ntcs::ProactorDetachContextTest::verifyCase3)
{
    ntsa::Error error;
    bool        result;

    // Create the context.

    ntcs::ProactorDetachContext context;

    NTSCFG_TEST_EQ(context.numProcessors(), 0);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Acquire lease (n = 1).

    result = context.incrementReference();

    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_EQ(context.numProcessors(), 1);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Detach (pending).

    error = context.detach();
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_WOULD_BLOCK));

    NTSCFG_TEST_EQ(context.numProcessors(), 1);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHING);

    // Release lease (n = 0, complete).

    result = context.decrementReference();

    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_EQ(context.numProcessors(), 0);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHED);
}

NTSCFG_TEST_FUNCTION(ntcs::ProactorDetachContextTest::verifyCase4)
{
    ntsa::Error error;
    bool        result;

    // Create the context.

    ntcs::ProactorDetachContext context;

    NTSCFG_TEST_EQ(context.numProcessors(), 0);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Acquire lease (n = 1).

    result = context.incrementReference();

    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_EQ(context.numProcessors(), 1);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Release lease (n = 0).

    result = context.decrementReference();

    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_EQ(context.numProcessors(), 0);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Acquire lease (n = 1).

    result = context.incrementReference();

    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_EQ(context.numProcessors(), 1);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Acquire lease (n = 2).

    result = context.incrementReference();

    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_EQ(context.numProcessors(), 2);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Release lease (n = 1).

    result = context.decrementReference();

    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_EQ(context.numProcessors(), 1);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Release lease (n = 0).

    result = context.decrementReference();

    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_EQ(context.numProcessors(), 0);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Acquire lease (n = 1).

    result = context.incrementReference();

    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_EQ(context.numProcessors(), 1);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Acquire lease (n = 2).

    result = context.incrementReference();

    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_EQ(context.numProcessors(), 2);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Detach (pending).

    error = context.detach();
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_WOULD_BLOCK));

    NTSCFG_TEST_EQ(context.numProcessors(), 2);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHING);

    // Detach (pending, failed).

    error = context.detach();
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));

    NTSCFG_TEST_EQ(context.numProcessors(), 2);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHING);

    // Acquire lease (n = 2, failed).

    result = context.incrementReference();

    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_EQ(context.numProcessors(), 2);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHING);

    // Release lease (n = 1).

    result = context.decrementReference();

    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_EQ(context.numProcessors(), 1);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHING);

    // Release lease (n = 0, complete).

    result = context.decrementReference();

    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_EQ(context.numProcessors(), 0);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHED);

    // Acquire lease (n = 0, failed).

    result = context.incrementReference();

    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_EQ(context.numProcessors(), 0);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHED);

    // Detach (complete, failed).

    error = context.detach();
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));

    NTSCFG_TEST_EQ(context.numProcessors(), 0);
    NTSCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHED);
}

}  // close namespace ntcs
}  // close namespace BloombergLP
