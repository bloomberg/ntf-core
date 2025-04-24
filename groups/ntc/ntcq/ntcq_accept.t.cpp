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
BSLS_IDENT_RCSID(ntcq_accept_t_cpp, "$Id$ $CSID$")

#include <ntcq_accept.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcq {

// Provide tests for 'ntcq::AcceptQueue'.
class AcceptQueueTest
{
  public:
    // Verify the queue with level-triggered semantics.
    static void verifyTriggerByLevel();

    // Verify the queue with edge-triggered semantics.
    static void verifyTriggerAtEdge();
};

NTSCFG_TEST_FUNCTION(ntcq::AcceptQueueTest::verifyTriggerByLevel)
{
    const bsl::size_t k_LOW_WATERMARK = 1;
    const bsl::size_t k_HIGH_WATERMARK = 3;

    ntcq::AcceptQueue acceptQueue(NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(acceptQueue.lowWatermark(), 
                   NTCCFG_DEFAULT_LISTENER_SOCKET_ACCEPT_QUEUE_LOW_WATERMARK);

    NTSCFG_TEST_EQ(acceptQueue.highWatermark(), 
                   NTCCFG_DEFAULT_LISTENER_SOCKET_ACCEPT_QUEUE_HIGH_WATERMARK);

    NTSCFG_TEST_EQ(acceptQueue.size(), 0);
    NTSCFG_TEST_FALSE(acceptQueue.isLowWatermarkSatisfied());
    NTSCFG_TEST_FALSE(acceptQueue.isHighWatermarkViolated());

    acceptQueue.setLowWatermark(k_LOW_WATERMARK);
    acceptQueue.setHighWatermark(k_HIGH_WATERMARK);

    NTSCFG_TEST_EQ(acceptQueue.lowWatermark(), k_LOW_WATERMARK);
    NTSCFG_TEST_EQ(acceptQueue.highWatermark(), k_HIGH_WATERMARK);

    for (bsl::size_t iteration = 0; iteration < 3; ++iteration) {
        NTSCFG_TEST_EQ(acceptQueue.size(), 0);
        NTSCFG_TEST_FALSE(acceptQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_FALSE(acceptQueue.isHighWatermarkViolated());

        {
            ntcq::AcceptQueueEntry entry;
            entry.setTimestamp(1);

            bool becameNonEmpty = acceptQueue.pushEntry(entry);
            NTSCFG_TEST_TRUE(becameNonEmpty);
        }

        NTSCFG_TEST_EQ(acceptQueue.size(), 1);
        NTSCFG_TEST_TRUE(acceptQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_FALSE(acceptQueue.isHighWatermarkViolated());

        NTSCFG_TEST_TRUE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());

        NTSCFG_TEST_TRUE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());

        {
            ntcq::AcceptQueueEntry entry;
            entry.setTimestamp(2);

            bool becameNonEmpty = acceptQueue.pushEntry(entry);
            NTSCFG_TEST_FALSE(becameNonEmpty);
        }

        NTSCFG_TEST_EQ(acceptQueue.size(), 2);
        NTSCFG_TEST_TRUE(acceptQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_FALSE(acceptQueue.isHighWatermarkViolated());

        NTSCFG_TEST_TRUE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());
        
        NTSCFG_TEST_TRUE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());

        {
            ntcq::AcceptQueueEntry entry;
            entry.setTimestamp(3);

            bool becameNonEmpty = acceptQueue.pushEntry(entry);
            NTSCFG_TEST_FALSE(becameNonEmpty);
        }

        NTSCFG_TEST_EQ(acceptQueue.size(), 3);
        NTSCFG_TEST_TRUE(acceptQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_TRUE(acceptQueue.isHighWatermarkViolated());

        NTSCFG_TEST_TRUE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_TRUE(acceptQueue.authorizeHighWatermarkEvent());
        
        NTSCFG_TEST_TRUE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_TRUE(acceptQueue.authorizeHighWatermarkEvent());

        {
            bool becameEmpty = acceptQueue.popEntry();
            NTSCFG_TEST_FALSE(becameEmpty);
        }

        NTSCFG_TEST_EQ(acceptQueue.size(), 2);
        NTSCFG_TEST_TRUE(acceptQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_FALSE(acceptQueue.isHighWatermarkViolated());

        NTSCFG_TEST_TRUE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());
        
        NTSCFG_TEST_TRUE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());

        {
            bool becameEmpty = acceptQueue.popEntry();
            NTSCFG_TEST_FALSE(becameEmpty);
        }

        NTSCFG_TEST_EQ(acceptQueue.size(), 1);
        NTSCFG_TEST_TRUE(acceptQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_FALSE(acceptQueue.isHighWatermarkViolated());

        NTSCFG_TEST_TRUE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());
        
        NTSCFG_TEST_TRUE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());

        {
            bool becameEmpty = acceptQueue.popEntry();
            NTSCFG_TEST_TRUE(becameEmpty);
        }

        NTSCFG_TEST_EQ(acceptQueue.size(), 0);
        NTSCFG_TEST_FALSE(acceptQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_FALSE(acceptQueue.isHighWatermarkViolated());

        NTSCFG_TEST_FALSE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());
        
        NTSCFG_TEST_FALSE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());
    }
}

NTSCFG_TEST_FUNCTION(ntcq::AcceptQueueTest::verifyTriggerAtEdge)
{
    const bsl::size_t k_LOW_WATERMARK = 1;
    const bsl::size_t k_HIGH_WATERMARK = 3;

    ntcq::AcceptQueue acceptQueue(NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(acceptQueue.lowWatermark(), 
                   NTCCFG_DEFAULT_LISTENER_SOCKET_ACCEPT_QUEUE_LOW_WATERMARK);

    NTSCFG_TEST_EQ(acceptQueue.highWatermark(), 
                   NTCCFG_DEFAULT_LISTENER_SOCKET_ACCEPT_QUEUE_HIGH_WATERMARK);

    NTSCFG_TEST_EQ(acceptQueue.size(), 0);
    NTSCFG_TEST_FALSE(acceptQueue.isLowWatermarkSatisfied());
    NTSCFG_TEST_FALSE(acceptQueue.isHighWatermarkViolated());

    acceptQueue.setTrigger(ntca::ReactorEventTrigger::e_EDGE);
    acceptQueue.setLowWatermark(k_LOW_WATERMARK);
    acceptQueue.setHighWatermark(k_HIGH_WATERMARK);

    NTSCFG_TEST_EQ(acceptQueue.lowWatermark(), k_LOW_WATERMARK);
    NTSCFG_TEST_EQ(acceptQueue.highWatermark(), k_HIGH_WATERMARK);

    for (bsl::size_t iteration = 0; iteration < 3; ++iteration) {
        NTSCFG_TEST_EQ(acceptQueue.size(), 0);
        NTSCFG_TEST_FALSE(acceptQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_FALSE(acceptQueue.isHighWatermarkViolated());

        {
            ntcq::AcceptQueueEntry entry;
            entry.setTimestamp(1);

            bool becameNonEmpty = acceptQueue.pushEntry(entry);
            NTSCFG_TEST_TRUE(becameNonEmpty);
        }

        NTSCFG_TEST_EQ(acceptQueue.size(), 1);
        NTSCFG_TEST_TRUE(acceptQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_FALSE(acceptQueue.isHighWatermarkViolated());

        NTSCFG_TEST_TRUE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());

        NTSCFG_TEST_FALSE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());

        {
            ntcq::AcceptQueueEntry entry;
            entry.setTimestamp(2);

            bool becameNonEmpty = acceptQueue.pushEntry(entry);
            NTSCFG_TEST_FALSE(becameNonEmpty);
        }

        NTSCFG_TEST_EQ(acceptQueue.size(), 2);
        NTSCFG_TEST_TRUE(acceptQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_FALSE(acceptQueue.isHighWatermarkViolated());

        NTSCFG_TEST_FALSE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());
        
        NTSCFG_TEST_FALSE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());

        {
            ntcq::AcceptQueueEntry entry;
            entry.setTimestamp(3);

            bool becameNonEmpty = acceptQueue.pushEntry(entry);
            NTSCFG_TEST_FALSE(becameNonEmpty);
        }

        NTSCFG_TEST_EQ(acceptQueue.size(), 3);
        NTSCFG_TEST_TRUE(acceptQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_TRUE(acceptQueue.isHighWatermarkViolated());

        NTSCFG_TEST_FALSE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_TRUE(acceptQueue.authorizeHighWatermarkEvent());
        
        NTSCFG_TEST_FALSE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());

        {
            bool becameEmpty = acceptQueue.popEntry();
            NTSCFG_TEST_FALSE(becameEmpty);
        }

        NTSCFG_TEST_EQ(acceptQueue.size(), 2);
        NTSCFG_TEST_TRUE(acceptQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_FALSE(acceptQueue.isHighWatermarkViolated());

        NTSCFG_TEST_FALSE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());
        
        NTSCFG_TEST_FALSE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());

        {
            bool becameEmpty = acceptQueue.popEntry();
            NTSCFG_TEST_FALSE(becameEmpty);
        }

        NTSCFG_TEST_EQ(acceptQueue.size(), 1);
        NTSCFG_TEST_TRUE(acceptQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_FALSE(acceptQueue.isHighWatermarkViolated());

        NTSCFG_TEST_FALSE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());
        
        NTSCFG_TEST_FALSE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());

        {
            bool becameEmpty = acceptQueue.popEntry();
            NTSCFG_TEST_TRUE(becameEmpty);
        }

        NTSCFG_TEST_EQ(acceptQueue.size(), 0);
        NTSCFG_TEST_FALSE(acceptQueue.isLowWatermarkSatisfied());
        NTSCFG_TEST_FALSE(acceptQueue.isHighWatermarkViolated());

        NTSCFG_TEST_FALSE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());
        
        NTSCFG_TEST_FALSE(acceptQueue.authorizeLowWatermarkEvent());
        NTSCFG_TEST_FALSE(acceptQueue.authorizeHighWatermarkEvent());
    }
}

}  // close namespace ntcq
}  // close namespace BloombergLP
