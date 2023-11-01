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

#include <ntcs_proactordetachcontext.h>

#include <ntccfg_test.h>

using namespace BloombergLP;

NTCCFG_TEST_CASE(1)
{
    ntsa::Error error;
    bool        result;

    // Create the context.

    ntcs::ProactorDetachContext context;

    NTCCFG_TEST_EQ(context.numProcessors(), 0);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Detach (complete).

    error = context.detach();
    NTCCFG_TEST_OK(error);

    NTCCFG_TEST_EQ(context.numProcessors(), 0);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHED);
}

NTCCFG_TEST_CASE(2)
{
    ntsa::Error error;
    bool        result;

    // Create the context.

    ntcs::ProactorDetachContext context;

    NTCCFG_TEST_EQ(context.numProcessors(), 0);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Acquire lease (n = 1).

    result = context.incrementReference();

    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_EQ(context.numProcessors(), 1);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Release lease (n = 0).

    result = context.decrementReference();

    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_EQ(context.numProcessors(), 0);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Detach (complete).

    error = context.detach();
    NTCCFG_TEST_OK(error);

    NTCCFG_TEST_EQ(context.numProcessors(), 0);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHED);
}

NTCCFG_TEST_CASE(3)
{
    ntsa::Error error;
    bool        result;

    // Create the context.

    ntcs::ProactorDetachContext context;

    NTCCFG_TEST_EQ(context.numProcessors(), 0);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Acquire lease (n = 1).

    result = context.incrementReference();

    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_EQ(context.numProcessors(), 1);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Detach (pending).

    error = context.detach();
    NTCCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_WOULD_BLOCK));

    NTCCFG_TEST_EQ(context.numProcessors(), 1);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHING);

    // Release lease (n = 0, complete).

    result = context.decrementReference();

    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_EQ(context.numProcessors(), 0);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHED);
}

NTCCFG_TEST_CASE(4)
{
    ntsa::Error error;
    bool        result;

    // Create the context.

    ntcs::ProactorDetachContext context;

    NTCCFG_TEST_EQ(context.numProcessors(), 0);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Acquire lease (n = 1).

    result = context.incrementReference();

    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_EQ(context.numProcessors(), 1);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Release lease (n = 0).

    result = context.decrementReference();

    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_EQ(context.numProcessors(), 0);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Acquire lease (n = 1).

    result = context.incrementReference();

    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_EQ(context.numProcessors(), 1);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Acquire lease (n = 2).

    result = context.incrementReference();

    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_EQ(context.numProcessors(), 2);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Release lease (n = 1).

    result = context.decrementReference();

    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_EQ(context.numProcessors(), 1);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Release lease (n = 0).

    result = context.decrementReference();

    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_EQ(context.numProcessors(), 0);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Acquire lease (n = 1).

    result = context.incrementReference();

    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_EQ(context.numProcessors(), 1);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Acquire lease (n = 2).

    result = context.incrementReference();

    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_EQ(context.numProcessors(), 2);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_ATTACHED);

    // Detach (pending).

    error = context.detach();
    NTCCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_WOULD_BLOCK));

    NTCCFG_TEST_EQ(context.numProcessors(), 2);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHING);

    // Detach (pending, failed).

    error = context.detach();
    NTCCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));

    NTCCFG_TEST_EQ(context.numProcessors(), 2);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHING);

    // Acquire lease (n = 2, failed).

    result = context.incrementReference();

    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_EQ(context.numProcessors(), 2);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHING);

    // Release lease (n = 1).

    result = context.decrementReference();

    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_EQ(context.numProcessors(), 1);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHING);

    // Release lease (n = 0, complete).

    result = context.decrementReference();

    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_EQ(context.numProcessors(), 0);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHED);

    // Acquire lease (n = 0, failed).

    result = context.incrementReference();

    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_EQ(context.numProcessors(), 0);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHED);

    // Detach (complete, failed).

    error = context.detach();
    NTCCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));

    NTCCFG_TEST_EQ(context.numProcessors(), 0);
    NTCCFG_TEST_EQ(context.state(), ntcs::ProactorDetachState::e_DETACHED);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
}
NTCCFG_TEST_DRIVER_END;
