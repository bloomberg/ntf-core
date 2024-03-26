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

#include <ntsa_event.h>
#include <ntscfg_test.h>

using namespace BloombergLP;

NTSCFG_TEST_CASE(1)
{
    // Concern: ntsa::Event stores the event types and correctly reports
    // whether the event type has occurred.

    const ntsa::Handle k_SOCKET         = 10;
    const ntsa::Handle k_SOCKET_INVALID = ntsa::k_INVALID_HANDLE;

    NTSCFG_TEST_NE(k_SOCKET, k_SOCKET_INVALID);

    ntsa::Event event;

    NTSCFG_TEST_EQ(event.handle(), k_SOCKET_INVALID);
    NTSCFG_TEST_FALSE(event.isReadable());
    NTSCFG_TEST_FALSE(event.isWritable());
    NTSCFG_TEST_FALSE(event.isExceptional());
    NTSCFG_TEST_FALSE(event.isError());
    NTSCFG_TEST_FALSE(event.isShutdown());
    NTSCFG_TEST_FALSE(event.isHangup());

    NTSCFG_TEST_OK(event.error());

    event.setHandle(k_SOCKET);

    NTSCFG_TEST_EQ(event.handle(), k_SOCKET);
    NTSCFG_TEST_FALSE(event.isReadable());
    NTSCFG_TEST_FALSE(event.isWritable());
    NTSCFG_TEST_FALSE(event.isExceptional());
    NTSCFG_TEST_FALSE(event.isError());
    NTSCFG_TEST_FALSE(event.isShutdown());
    NTSCFG_TEST_FALSE(event.isHangup());

    NTSCFG_TEST_OK(event.error());

    event.setReadable();

    NTSCFG_TEST_EQ(event.handle(), k_SOCKET);
    NTSCFG_TEST_TRUE(event.isReadable());
    NTSCFG_TEST_FALSE(event.isWritable());
    NTSCFG_TEST_FALSE(event.isExceptional());
    NTSCFG_TEST_FALSE(event.isError());
    NTSCFG_TEST_FALSE(event.isShutdown());
    NTSCFG_TEST_FALSE(event.isHangup());

    NTSCFG_TEST_OK(event.error());

    event.setWritable();

    NTSCFG_TEST_EQ(event.handle(), k_SOCKET);
    NTSCFG_TEST_TRUE(event.isReadable());
    NTSCFG_TEST_TRUE(event.isWritable());
    NTSCFG_TEST_FALSE(event.isExceptional());
    NTSCFG_TEST_FALSE(event.isError());
    NTSCFG_TEST_FALSE(event.isShutdown());
    NTSCFG_TEST_FALSE(event.isHangup());

    NTSCFG_TEST_OK(event.error());

    event.setExceptional();

    NTSCFG_TEST_EQ(event.handle(), k_SOCKET);
    NTSCFG_TEST_TRUE(event.isReadable());
    NTSCFG_TEST_TRUE(event.isWritable());
    NTSCFG_TEST_TRUE(event.isExceptional());
    NTSCFG_TEST_FALSE(event.isError());
    NTSCFG_TEST_FALSE(event.isShutdown());
    NTSCFG_TEST_FALSE(event.isHangup());

    NTSCFG_TEST_OK(event.error());

    event.setError(ntsa::Error(ntsa::Error::e_INVALID));

    NTSCFG_TEST_EQ(event.handle(), k_SOCKET);
    NTSCFG_TEST_TRUE(event.isReadable());
    NTSCFG_TEST_TRUE(event.isWritable());
    NTSCFG_TEST_TRUE(event.isExceptional());
    NTSCFG_TEST_TRUE(event.isError());
    NTSCFG_TEST_FALSE(event.isShutdown());
    NTSCFG_TEST_FALSE(event.isHangup());

    NTSCFG_TEST_EQ(event.error(), ntsa::Error(ntsa::Error::e_INVALID));

    event.setShutdown();

    NTSCFG_TEST_EQ(event.handle(), k_SOCKET);
    NTSCFG_TEST_TRUE(event.isReadable());
    NTSCFG_TEST_TRUE(event.isWritable());
    NTSCFG_TEST_TRUE(event.isExceptional());
    NTSCFG_TEST_TRUE(event.isError());
    NTSCFG_TEST_TRUE(event.isShutdown());
    NTSCFG_TEST_FALSE(event.isHangup());

    NTSCFG_TEST_EQ(event.error(), ntsa::Error(ntsa::Error::e_INVALID));

    event.setHangup();

    NTSCFG_TEST_EQ(event.handle(), k_SOCKET);
    NTSCFG_TEST_TRUE(event.isReadable());
    NTSCFG_TEST_TRUE(event.isWritable());
    NTSCFG_TEST_TRUE(event.isExceptional());
    NTSCFG_TEST_TRUE(event.isError());
    NTSCFG_TEST_TRUE(event.isShutdown());
    NTSCFG_TEST_TRUE(event.isHangup());

    NTSCFG_TEST_EQ(event.error(), ntsa::Error(ntsa::Error::e_INVALID));
}

NTSCFG_TEST_CASE(2)
{
    // Concern: Events for the same socket may be merged together.

    ntsa::Error error;

    const ntsa::Handle k_SOCKET = 10;

    ntsa::Event eventA;
    eventA.setHandle(k_SOCKET);
    eventA.setReadable();

    NTSCFG_TEST_EQ(eventA.handle(), k_SOCKET);
    NTSCFG_TEST_TRUE(eventA.isReadable());
    NTSCFG_TEST_FALSE(eventA.isWritable());

    ntsa::Event eventB;
    eventB.setHandle(k_SOCKET);
    eventB.setWritable();

    NTSCFG_TEST_EQ(eventB.handle(), k_SOCKET);
    NTSCFG_TEST_FALSE(eventB.isReadable());
    NTSCFG_TEST_TRUE(eventB.isWritable());

    error = eventA.merge(eventB);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_EQ(eventA.handle(), k_SOCKET);
    NTSCFG_TEST_TRUE(eventA.isReadable());
    NTSCFG_TEST_TRUE(eventA.isWritable());
}

NTSCFG_TEST_CASE(3)
{
    // Concern: Events for different sockets may not be merged together.

    ntsa::Error error;

    const ntsa::Handle k_SOCKET_A = 10;
    const ntsa::Handle k_SOCKET_B = 20;

    ntsa::Event eventA;
    eventA.setHandle(k_SOCKET_A);
    eventA.setReadable();

    NTSCFG_TEST_EQ(eventA.handle(), k_SOCKET_A);
    NTSCFG_TEST_TRUE(eventA.isReadable());
    NTSCFG_TEST_FALSE(eventA.isWritable());

    ntsa::Event eventB;
    eventB.setHandle(k_SOCKET_B);
    eventB.setWritable();

    NTSCFG_TEST_EQ(eventB.handle(), k_SOCKET_B);
    NTSCFG_TEST_FALSE(eventB.isReadable());
    NTSCFG_TEST_TRUE(eventB.isWritable());

    error = eventA.merge(eventB);
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));

    NTSCFG_TEST_EQ(eventA.handle(), k_SOCKET_A);
    NTSCFG_TEST_TRUE(eventA.isReadable());
    NTSCFG_TEST_FALSE(eventA.isWritable());
}

NTSCFG_TEST_CASE(4)
{
    // Concern: The event set stores and merges socket events.

    ntscfg::TestAllocator ta;
    {
        const ntsa::Handle k_SOCKET_A = 10;
        const ntsa::Handle k_SOCKET_B = 100;
        const ntsa::Handle k_SOCKET_C = 1000;

        ntsa::EventSet eventSet(&ta);

        NTSCFG_TEST_EQ(eventSet.size(), 0);

        {
            ntsa::Event event;
            event.setHandle(k_SOCKET_C);
            event.setReadable();

            eventSet.merge(event);
        }

        NTSCFG_TEST_EQ(eventSet.size(), 1);

        {
            ntsa::Event event;
            event.setHandle(k_SOCKET_C);
            event.setWritable();

            eventSet.merge(event);
        }

        NTSCFG_TEST_EQ(eventSet.size(), 1);

        {
            ntsa::Event event;
            event.setHandle(k_SOCKET_B);
            event.setWritable();

            eventSet.merge(event);
        }

        NTSCFG_TEST_EQ(eventSet.size(), 2);

        {
            ntsa::Event event;
            event.setHandle(k_SOCKET_A);
            event.setReadable();

            eventSet.merge(event);
        }

        NTSCFG_TEST_EQ(eventSet.size(), 3);

        NTSCFG_TEST_TRUE(eventSet.isReadable(k_SOCKET_A));
        NTSCFG_TEST_FALSE(eventSet.isWritable(k_SOCKET_A));

        NTSCFG_TEST_FALSE(eventSet.isReadable(k_SOCKET_B));
        NTSCFG_TEST_TRUE(eventSet.isWritable(k_SOCKET_B));

        NTSCFG_TEST_TRUE(eventSet.isReadable(k_SOCKET_C));
        NTSCFG_TEST_TRUE(eventSet.isWritable(k_SOCKET_C));
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(5)
{
    // Concern: The event set is iteratable.

    ntscfg::TestAllocator ta;
    {
        const ntsa::Handle k_SOCKET_A = 10;
        const ntsa::Handle k_SOCKET_B = 100;
        const ntsa::Handle k_SOCKET_C = 1000;

        ntsa::EventSet eventSet(&ta);

        eventSet.setReadable(k_SOCKET_C);
        eventSet.setWritable(k_SOCKET_C);
        eventSet.setWritable(k_SOCKET_B);
        eventSet.setReadable(k_SOCKET_A);

        NTSCFG_TEST_LOG_DEBUG << "Event set = " << eventSet
                              << NTSCFG_TEST_LOG_END;

        typedef bsl::vector<ntsa::Event> EventVector;
        EventVector                      eventVector(&ta);

        for (ntsa::EventSet::const_iterator it = eventSet.cbegin();
             it != eventSet.cend();
             ++it)
        {
            const ntsa::Event& event = *it;

            NTSCFG_TEST_LOG_DEBUG << "Event = " << event
                                  << NTSCFG_TEST_LOG_END;

            eventVector.push_back(event);
        }

        NTSCFG_TEST_EQ(eventVector.size(), 3);

        NTSCFG_TEST_EQ(eventVector[0].handle(), k_SOCKET_A);
        NTSCFG_TEST_TRUE(eventVector[0].isReadable());

        NTSCFG_TEST_EQ(eventVector[1].handle(), k_SOCKET_B);
        NTSCFG_TEST_TRUE(eventVector[1].isWritable());

        NTSCFG_TEST_EQ(eventVector[2].handle(), k_SOCKET_C);
        NTSCFG_TEST_TRUE(eventVector[2].isReadable());
        NTSCFG_TEST_TRUE(eventVector[2].isWritable());
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
    NTSCFG_TEST_REGISTER(4);
    NTSCFG_TEST_REGISTER(5);
}
NTSCFG_TEST_DRIVER_END;
