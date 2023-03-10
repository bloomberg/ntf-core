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

#include <ntcs_shutdownstate.h>

#include <ntccfg_test.h>
#include <ntci_log.h>
#include <bsl_iostream.h>

using namespace BloombergLP;

NTCCFG_TEST_CASE(1)
{
    // Test shutdown sequence with half-open sockets.
    //
    // Test case 1: shutdown(SEND)
    //              shutdown(RECEIVE, PEER)
    //
    // Test case 2: shutdown(SEND)
    //              shutdown(RECEIVE, LOCAL)
    //
    // Test case 3: shutdown(RECEIVE, PEER)
    //              shutdown(SEND)
    //
    // Test case 4: shutdown(RECEIVE, LOCAL)
    //              shutdown(SEND)

    // Verify the initial state.

    {
        ntcs::ShutdownState state;

        NTCCFG_TEST_FALSE(state.initiated());
        NTCCFG_TEST_TRUE(state.canSend());
        NTCCFG_TEST_TRUE(state.canReceive());
        NTCCFG_TEST_FALSE(state.completed());
    }

    // Test case 1:
    //
    // shutdown(SEND)
    // shutdown(RECEIVE, PEER)

    {
        ntcs::ShutdownState   state;
        ntcs::ShutdownContext context;

        // shutdown(SEND)

        {
            bool result = state.tryShutdownSend(&context, true);

            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_TRUE(context.shutdownInitiated());
            NTCCFG_TEST_TRUE(context.shutdownSend());
            NTCCFG_TEST_FALSE(context.shutdownReceive());
            NTCCFG_TEST_FALSE(context.shutdownCompleted());
        }

        // shutdown(RECEIVE, PEER)

        {
            bool result =
                state.tryShutdownReceive(&context,
                                         true,
                                         ntsa::ShutdownOrigin::e_REMOTE);

            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_FALSE(context.shutdownInitiated());
            NTCCFG_TEST_FALSE(context.shutdownSend());
            NTCCFG_TEST_TRUE(context.shutdownReceive());
            NTCCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTCCFG_TEST_TRUE(state.initiated());
        NTCCFG_TEST_FALSE(state.canSend());
        NTCCFG_TEST_FALSE(state.canReceive());
        NTCCFG_TEST_TRUE(state.completed());
    }

    // Test case 2:
    //
    // shutdown(SEND)
    // shutdown(RECEIVE, LOCAL)

    {
        ntcs::ShutdownState   state;
        ntcs::ShutdownContext context;

        // shutdown(SEND)

        {
            bool result = state.tryShutdownSend(&context, true);

            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_TRUE(context.shutdownInitiated());
            NTCCFG_TEST_TRUE(context.shutdownSend());
            NTCCFG_TEST_FALSE(context.shutdownReceive());
            NTCCFG_TEST_FALSE(context.shutdownCompleted());
        }

        // shutdown(RECEIVE, LOCAL)

        {
            bool result =
                state.tryShutdownReceive(&context,
                                         true,
                                         ntsa::ShutdownOrigin::e_SOURCE);

            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_FALSE(context.shutdownInitiated());
            NTCCFG_TEST_FALSE(context.shutdownSend());
            NTCCFG_TEST_TRUE(context.shutdownReceive());
            NTCCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTCCFG_TEST_TRUE(state.initiated());
        NTCCFG_TEST_FALSE(state.canSend());
        NTCCFG_TEST_FALSE(state.canReceive());
        NTCCFG_TEST_TRUE(state.completed());
    }

    // Test case 3:
    //
    // shutdown(RECEIVE, PEER)
    // shutdown(SEND)

    {
        ntcs::ShutdownState   state;
        ntcs::ShutdownContext context;

        // shutdown(RECEIVE, PEER)

        {
            bool result =
                state.tryShutdownReceive(&context,
                                         true,
                                         ntsa::ShutdownOrigin::e_REMOTE);

            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_TRUE(context.shutdownInitiated());
            NTCCFG_TEST_FALSE(context.shutdownSend());
            NTCCFG_TEST_TRUE(context.shutdownReceive());
            NTCCFG_TEST_FALSE(context.shutdownCompleted());
        }

        // shutdown(SEND)

        {
            bool result = state.tryShutdownSend(&context, true);

            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_FALSE(context.shutdownInitiated());
            NTCCFG_TEST_TRUE(context.shutdownSend());
            NTCCFG_TEST_FALSE(context.shutdownReceive());
            NTCCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTCCFG_TEST_TRUE(state.initiated());
        NTCCFG_TEST_FALSE(state.canSend());
        NTCCFG_TEST_FALSE(state.canReceive());
        NTCCFG_TEST_TRUE(state.completed());
    }

    // Test case 4:
    //
    // shutdown(RECEIVE, LOCAL)
    // shutdown(SEND)

    {
        ntcs::ShutdownState   state;
        ntcs::ShutdownContext context;

        // shutdown(RECEIVE, LOCAL)

        {
            bool result =
                state.tryShutdownReceive(&context,
                                         true,
                                         ntsa::ShutdownOrigin::e_SOURCE);

            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_FALSE(context.shutdownInitiated());
            NTCCFG_TEST_FALSE(context.shutdownSend());
            NTCCFG_TEST_TRUE(context.shutdownReceive());
            NTCCFG_TEST_FALSE(context.shutdownCompleted());
        }

        // shutdown(SEND)

        {
            bool result = state.tryShutdownSend(&context, true);

            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_TRUE(context.shutdownInitiated());
            NTCCFG_TEST_TRUE(context.shutdownSend());
            NTCCFG_TEST_FALSE(context.shutdownReceive());
            NTCCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTCCFG_TEST_TRUE(state.initiated());
        NTCCFG_TEST_FALSE(state.canSend());
        NTCCFG_TEST_FALSE(state.canReceive());
        NTCCFG_TEST_TRUE(state.completed());
    }
}

NTCCFG_TEST_CASE(2)
{
    // Test shutdown sequence with automatically-closed (i.e., non-half-open)
    // sockets.
    //
    // Test case 1: shutdown(SEND)
    //
    // Test case 2: shutdown(RECEIVE, LOCAL)
    //
    // Test case 3: shutdown(RECEIVE, PEER)

    // Verify the initial state.

    {
        ntcs::ShutdownState state;

        NTCCFG_TEST_FALSE(state.initiated());
        NTCCFG_TEST_TRUE(state.canSend());
        NTCCFG_TEST_TRUE(state.canReceive());
        NTCCFG_TEST_FALSE(state.completed());
    }

    // Test case 1:
    //
    // shutdown(SEND)

    {
        ntcs::ShutdownState   state;
        ntcs::ShutdownContext context;

        // shutdown(SEND)

        {
            bool result = state.tryShutdownSend(&context, false);

            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_TRUE(context.shutdownInitiated());
            NTCCFG_TEST_TRUE(context.shutdownSend());
            NTCCFG_TEST_TRUE(context.shutdownReceive());
            NTCCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTCCFG_TEST_TRUE(state.initiated());
        NTCCFG_TEST_FALSE(state.canSend());
        NTCCFG_TEST_FALSE(state.canReceive());
        NTCCFG_TEST_TRUE(state.completed());
    }

    // Test case 2:
    //
    // shutdown(RECEIVE, LOCAL)

    {
        ntcs::ShutdownState   state;
        ntcs::ShutdownContext context;

        // shutdown(RECEIVE, LOCAL)

        {
            bool result =
                state.tryShutdownReceive(&context,
                                         false,
                                         ntsa::ShutdownOrigin::e_SOURCE);

            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_TRUE(context.shutdownInitiated());
            NTCCFG_TEST_TRUE(context.shutdownSend());
            NTCCFG_TEST_TRUE(context.shutdownReceive());
            NTCCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTCCFG_TEST_TRUE(state.initiated());
        NTCCFG_TEST_FALSE(state.canSend());
        NTCCFG_TEST_FALSE(state.canReceive());
        NTCCFG_TEST_TRUE(state.completed());
    }

    // Test case 3:
    //
    // shutdown(RECEIVE, PEER)

    {
        ntcs::ShutdownState   state;
        ntcs::ShutdownContext context;

        // shutdown(RECEIVE, PEER)

        {
            bool result =
                state.tryShutdownReceive(&context,
                                         false,
                                         ntsa::ShutdownOrigin::e_REMOTE);

            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_TRUE(context.shutdownInitiated());
            NTCCFG_TEST_TRUE(context.shutdownSend());
            NTCCFG_TEST_TRUE(context.shutdownReceive());
            NTCCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTCCFG_TEST_TRUE(state.initiated());
        NTCCFG_TEST_FALSE(state.canSend());
        NTCCFG_TEST_FALSE(state.canReceive());
        NTCCFG_TEST_TRUE(state.completed());
    }
}

NTCCFG_TEST_CASE(3)
{
    // Test shutdown sequence on already closed socket.
    //
    // Test case 1: shutdown(SEND)
    //
    // Test case 2: shutdown(RECEIVE, LOCAL)
    //
    // Test case 3: shutdown(RECEIVE, PEER)

    // Verify the initial state.

    {
        ntcs::ShutdownState state;

        NTCCFG_TEST_FALSE(state.initiated());
        NTCCFG_TEST_TRUE(state.canSend());
        NTCCFG_TEST_TRUE(state.canReceive());
        NTCCFG_TEST_FALSE(state.completed());
    }

    // Test case 1:
    //
    // shutdown(SEND)

    {
        ntcs::ShutdownState   state;
        ntcs::ShutdownContext context;

        // shutdown(SEND)

        {
            bool result = state.tryShutdownSend(&context, false);

            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_TRUE(context.shutdownInitiated());
            NTCCFG_TEST_TRUE(context.shutdownSend());
            NTCCFG_TEST_TRUE(context.shutdownReceive());
            NTCCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTCCFG_TEST_TRUE(state.initiated());
        NTCCFG_TEST_FALSE(state.canSend());
        NTCCFG_TEST_FALSE(state.canReceive());
        NTCCFG_TEST_TRUE(state.completed());

        {
            bool result = state.tryShutdownSend(&context, false);
            NTCCFG_TEST_FALSE(result);
        }

        NTCCFG_TEST_TRUE(state.initiated());
        NTCCFG_TEST_FALSE(state.canSend());
        NTCCFG_TEST_FALSE(state.canReceive());
        NTCCFG_TEST_TRUE(state.completed());

        {
            bool result =
                state.tryShutdownReceive(&context,
                                         false,
                                         ntsa::ShutdownOrigin::e_SOURCE);
            NTCCFG_TEST_FALSE(result);
        }

        NTCCFG_TEST_TRUE(state.initiated());
        NTCCFG_TEST_FALSE(state.canSend());
        NTCCFG_TEST_FALSE(state.canReceive());
        NTCCFG_TEST_TRUE(state.completed());

        {
            bool result =
                state.tryShutdownReceive(&context,
                                         false,
                                         ntsa::ShutdownOrigin::e_REMOTE);
            NTCCFG_TEST_FALSE(result);
        }

        NTCCFG_TEST_TRUE(state.initiated());
        NTCCFG_TEST_FALSE(state.canSend());
        NTCCFG_TEST_FALSE(state.canReceive());
        NTCCFG_TEST_TRUE(state.completed());
    }

    // Test case 2:
    //
    // shutdown(RECEIVE, LOCAL)

    {
        ntcs::ShutdownState   state;
        ntcs::ShutdownContext context;

        // shutdown(RECEIVE, LOCAL)

        {
            bool result =
                state.tryShutdownReceive(&context,
                                         false,
                                         ntsa::ShutdownOrigin::e_SOURCE);

            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_TRUE(context.shutdownInitiated());
            NTCCFG_TEST_TRUE(context.shutdownSend());
            NTCCFG_TEST_TRUE(context.shutdownReceive());
            NTCCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTCCFG_TEST_TRUE(state.initiated());
        NTCCFG_TEST_FALSE(state.canSend());
        NTCCFG_TEST_FALSE(state.canReceive());
        NTCCFG_TEST_TRUE(state.completed());

        {
            bool result = state.tryShutdownSend(&context, false);

            NTCCFG_TEST_FALSE(result);
        }

        NTCCFG_TEST_TRUE(state.initiated());
        NTCCFG_TEST_FALSE(state.canSend());
        NTCCFG_TEST_FALSE(state.canReceive());
        NTCCFG_TEST_TRUE(state.completed());
    }

    // Test case 3:
    //
    // shutdown(RECEIVE, PEER)

    {
        ntcs::ShutdownState   state;
        ntcs::ShutdownContext context;

        // shutdown(RECEIVE, PEER)

        {
            bool result =
                state.tryShutdownReceive(&context,
                                         false,
                                         ntsa::ShutdownOrigin::e_REMOTE);

            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_TRUE(context.shutdownInitiated());
            NTCCFG_TEST_TRUE(context.shutdownSend());
            NTCCFG_TEST_TRUE(context.shutdownReceive());
            NTCCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTCCFG_TEST_TRUE(state.initiated());
        NTCCFG_TEST_FALSE(state.canSend());
        NTCCFG_TEST_FALSE(state.canReceive());
        NTCCFG_TEST_TRUE(state.completed());

        {
            bool result = state.tryShutdownSend(&context, false);

            NTCCFG_TEST_FALSE(result);
        }

        NTCCFG_TEST_TRUE(state.initiated());
        NTCCFG_TEST_FALSE(state.canSend());
        NTCCFG_TEST_FALSE(state.canReceive());
        NTCCFG_TEST_TRUE(state.completed());
    }
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
}
NTCCFG_TEST_DRIVER_END;
