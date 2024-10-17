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
BSLS_IDENT_RCSID(ntcs_shutdownstate_t_cpp, "$Id$ $CSID$")

#include <ntcs_shutdownstate.h>

#include <ntci_log.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::ShutdownState'.
class ShutdownStateTest
{
  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifyCase3();
};

NTSCFG_TEST_FUNCTION(ntcs::ShutdownStateTest::verifyCase1)
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

        NTSCFG_TEST_FALSE(state.initiated());
        NTSCFG_TEST_TRUE(state.canSend());
        NTSCFG_TEST_TRUE(state.canReceive());
        NTSCFG_TEST_FALSE(state.completed());
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

            NTSCFG_TEST_TRUE(result);
            NTSCFG_TEST_TRUE(context.shutdownInitiated());
            NTSCFG_TEST_TRUE(context.shutdownSend());
            NTSCFG_TEST_FALSE(context.shutdownReceive());
            NTSCFG_TEST_FALSE(context.shutdownCompleted());
        }

        // shutdown(RECEIVE, PEER)

        {
            bool result =
                state.tryShutdownReceive(&context,
                                         true,
                                         ntsa::ShutdownOrigin::e_REMOTE);

            NTSCFG_TEST_TRUE(result);
            NTSCFG_TEST_FALSE(context.shutdownInitiated());
            NTSCFG_TEST_FALSE(context.shutdownSend());
            NTSCFG_TEST_TRUE(context.shutdownReceive());
            NTSCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTSCFG_TEST_TRUE(state.initiated());
        NTSCFG_TEST_FALSE(state.canSend());
        NTSCFG_TEST_FALSE(state.canReceive());
        NTSCFG_TEST_TRUE(state.completed());
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

            NTSCFG_TEST_TRUE(result);
            NTSCFG_TEST_TRUE(context.shutdownInitiated());
            NTSCFG_TEST_TRUE(context.shutdownSend());
            NTSCFG_TEST_FALSE(context.shutdownReceive());
            NTSCFG_TEST_FALSE(context.shutdownCompleted());
        }

        // shutdown(RECEIVE, LOCAL)

        {
            bool result =
                state.tryShutdownReceive(&context,
                                         true,
                                         ntsa::ShutdownOrigin::e_SOURCE);

            NTSCFG_TEST_TRUE(result);
            NTSCFG_TEST_FALSE(context.shutdownInitiated());
            NTSCFG_TEST_FALSE(context.shutdownSend());
            NTSCFG_TEST_TRUE(context.shutdownReceive());
            NTSCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTSCFG_TEST_TRUE(state.initiated());
        NTSCFG_TEST_FALSE(state.canSend());
        NTSCFG_TEST_FALSE(state.canReceive());
        NTSCFG_TEST_TRUE(state.completed());
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

            NTSCFG_TEST_TRUE(result);
            NTSCFG_TEST_TRUE(context.shutdownInitiated());
            NTSCFG_TEST_FALSE(context.shutdownSend());
            NTSCFG_TEST_TRUE(context.shutdownReceive());
            NTSCFG_TEST_FALSE(context.shutdownCompleted());
        }

        // shutdown(SEND)

        {
            bool result = state.tryShutdownSend(&context, true);

            NTSCFG_TEST_TRUE(result);
            NTSCFG_TEST_FALSE(context.shutdownInitiated());
            NTSCFG_TEST_TRUE(context.shutdownSend());
            NTSCFG_TEST_FALSE(context.shutdownReceive());
            NTSCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTSCFG_TEST_TRUE(state.initiated());
        NTSCFG_TEST_FALSE(state.canSend());
        NTSCFG_TEST_FALSE(state.canReceive());
        NTSCFG_TEST_TRUE(state.completed());
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

            NTSCFG_TEST_TRUE(result);
            NTSCFG_TEST_FALSE(context.shutdownInitiated());
            NTSCFG_TEST_FALSE(context.shutdownSend());
            NTSCFG_TEST_TRUE(context.shutdownReceive());
            NTSCFG_TEST_FALSE(context.shutdownCompleted());
        }

        // shutdown(SEND)

        {
            bool result = state.tryShutdownSend(&context, true);

            NTSCFG_TEST_TRUE(result);
            NTSCFG_TEST_TRUE(context.shutdownInitiated());
            NTSCFG_TEST_TRUE(context.shutdownSend());
            NTSCFG_TEST_FALSE(context.shutdownReceive());
            NTSCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTSCFG_TEST_TRUE(state.initiated());
        NTSCFG_TEST_FALSE(state.canSend());
        NTSCFG_TEST_FALSE(state.canReceive());
        NTSCFG_TEST_TRUE(state.completed());
    }
}

NTSCFG_TEST_FUNCTION(ntcs::ShutdownStateTest::verifyCase2)
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

        NTSCFG_TEST_FALSE(state.initiated());
        NTSCFG_TEST_TRUE(state.canSend());
        NTSCFG_TEST_TRUE(state.canReceive());
        NTSCFG_TEST_FALSE(state.completed());
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

            NTSCFG_TEST_TRUE(result);
            NTSCFG_TEST_TRUE(context.shutdownInitiated());
            NTSCFG_TEST_TRUE(context.shutdownSend());
            NTSCFG_TEST_TRUE(context.shutdownReceive());
            NTSCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTSCFG_TEST_TRUE(state.initiated());
        NTSCFG_TEST_FALSE(state.canSend());
        NTSCFG_TEST_FALSE(state.canReceive());
        NTSCFG_TEST_TRUE(state.completed());
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

            NTSCFG_TEST_TRUE(result);
            NTSCFG_TEST_TRUE(context.shutdownInitiated());
            NTSCFG_TEST_TRUE(context.shutdownSend());
            NTSCFG_TEST_TRUE(context.shutdownReceive());
            NTSCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTSCFG_TEST_TRUE(state.initiated());
        NTSCFG_TEST_FALSE(state.canSend());
        NTSCFG_TEST_FALSE(state.canReceive());
        NTSCFG_TEST_TRUE(state.completed());
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

            NTSCFG_TEST_TRUE(result);
            NTSCFG_TEST_TRUE(context.shutdownInitiated());
            NTSCFG_TEST_TRUE(context.shutdownSend());
            NTSCFG_TEST_TRUE(context.shutdownReceive());
            NTSCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTSCFG_TEST_TRUE(state.initiated());
        NTSCFG_TEST_FALSE(state.canSend());
        NTSCFG_TEST_FALSE(state.canReceive());
        NTSCFG_TEST_TRUE(state.completed());
    }
}

NTSCFG_TEST_FUNCTION(ntcs::ShutdownStateTest::verifyCase3)
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

        NTSCFG_TEST_FALSE(state.initiated());
        NTSCFG_TEST_TRUE(state.canSend());
        NTSCFG_TEST_TRUE(state.canReceive());
        NTSCFG_TEST_FALSE(state.completed());
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

            NTSCFG_TEST_TRUE(result);
            NTSCFG_TEST_TRUE(context.shutdownInitiated());
            NTSCFG_TEST_TRUE(context.shutdownSend());
            NTSCFG_TEST_TRUE(context.shutdownReceive());
            NTSCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTSCFG_TEST_TRUE(state.initiated());
        NTSCFG_TEST_FALSE(state.canSend());
        NTSCFG_TEST_FALSE(state.canReceive());
        NTSCFG_TEST_TRUE(state.completed());

        {
            bool result = state.tryShutdownSend(&context, false);
            NTSCFG_TEST_FALSE(result);
        }

        NTSCFG_TEST_TRUE(state.initiated());
        NTSCFG_TEST_FALSE(state.canSend());
        NTSCFG_TEST_FALSE(state.canReceive());
        NTSCFG_TEST_TRUE(state.completed());

        {
            bool result =
                state.tryShutdownReceive(&context,
                                         false,
                                         ntsa::ShutdownOrigin::e_SOURCE);
            NTSCFG_TEST_FALSE(result);
        }

        NTSCFG_TEST_TRUE(state.initiated());
        NTSCFG_TEST_FALSE(state.canSend());
        NTSCFG_TEST_FALSE(state.canReceive());
        NTSCFG_TEST_TRUE(state.completed());

        {
            bool result =
                state.tryShutdownReceive(&context,
                                         false,
                                         ntsa::ShutdownOrigin::e_REMOTE);
            NTSCFG_TEST_FALSE(result);
        }

        NTSCFG_TEST_TRUE(state.initiated());
        NTSCFG_TEST_FALSE(state.canSend());
        NTSCFG_TEST_FALSE(state.canReceive());
        NTSCFG_TEST_TRUE(state.completed());
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

            NTSCFG_TEST_TRUE(result);
            NTSCFG_TEST_TRUE(context.shutdownInitiated());
            NTSCFG_TEST_TRUE(context.shutdownSend());
            NTSCFG_TEST_TRUE(context.shutdownReceive());
            NTSCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTSCFG_TEST_TRUE(state.initiated());
        NTSCFG_TEST_FALSE(state.canSend());
        NTSCFG_TEST_FALSE(state.canReceive());
        NTSCFG_TEST_TRUE(state.completed());

        {
            bool result = state.tryShutdownSend(&context, false);

            NTSCFG_TEST_FALSE(result);
        }

        NTSCFG_TEST_TRUE(state.initiated());
        NTSCFG_TEST_FALSE(state.canSend());
        NTSCFG_TEST_FALSE(state.canReceive());
        NTSCFG_TEST_TRUE(state.completed());
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

            NTSCFG_TEST_TRUE(result);
            NTSCFG_TEST_TRUE(context.shutdownInitiated());
            NTSCFG_TEST_TRUE(context.shutdownSend());
            NTSCFG_TEST_TRUE(context.shutdownReceive());
            NTSCFG_TEST_TRUE(context.shutdownCompleted());
        }

        NTSCFG_TEST_TRUE(state.initiated());
        NTSCFG_TEST_FALSE(state.canSend());
        NTSCFG_TEST_FALSE(state.canReceive());
        NTSCFG_TEST_TRUE(state.completed());

        {
            bool result = state.tryShutdownSend(&context, false);

            NTSCFG_TEST_FALSE(result);
        }

        NTSCFG_TEST_TRUE(state.initiated());
        NTSCFG_TEST_FALSE(state.canSend());
        NTSCFG_TEST_FALSE(state.canReceive());
        NTSCFG_TEST_TRUE(state.completed());
    }
}

}  // close namespace ntcs
}  // close namespace BloombergLP
