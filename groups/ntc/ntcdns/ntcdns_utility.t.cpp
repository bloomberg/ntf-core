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

#include <ntcdns_utility.h>

#include <ntccfg_test.h>
#include <ntci_log.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_iostream.h>
#include <bsl_set.h>
#include <bsl_string.h>
#include <bsl_vector.h>

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

NTCCFG_TEST_CASE(1)
{
    // Concern: Client configurations.

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        ntcdns::ClientConfig clientConfig(&ta);
        error = ntcdns::Utility::loadClientConfig(&clientConfig);
        NTCCFG_TEST_OK(error);

        NTCI_LOG_STREAM_TRACE << "Client configuration = " << clientConfig
                              << NTCI_LOG_STREAM_END;
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern: Host database configurations.

    if (!ntscfg::Platform::hasHostDatabase()) {
        return;
    }

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        ntcdns::HostDatabaseConfig hostDatabaseConfig(&ta);
        error = ntcdns::Utility::loadHostDatabaseConfig(&hostDatabaseConfig);
        NTCCFG_TEST_OK(error);

        NTCI_LOG_STREAM_TRACE << "Host database = " << hostDatabaseConfig
                              << NTCI_LOG_STREAM_END;
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Concern: Port database configurations.

    if (!ntscfg::Platform::hasPortDatabase()) {
        return;
    }

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        ntcdns::PortDatabaseConfig portDatabaseConfig(&ta);
        error = ntcdns::Utility::loadPortDatabaseConfig(&portDatabaseConfig);
        NTCCFG_TEST_OK(error);

        NTCI_LOG_STREAM_TRACE << "Port database = " << portDatabaseConfig
                              << NTCI_LOG_STREAM_END;
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    // Concern: Resolver configurations.

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        ntcdns::ResolverConfig resolverConfig(&ta);
        error = ntcdns::Utility::loadResolverConfig(&resolverConfig);
        NTCCFG_TEST_OK(error);

        NTCI_LOG_STREAM_TRACE << "Resolver configuration = " << resolverConfig
                              << NTCI_LOG_STREAM_END;
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(5)
{
    NTCI_LOG_CONTEXT();

    ntccfg::TestAllocator ta;
    {
        typedef ntcdns::Map<int, int> MapType;

        MapType map(&ta);

        bool result = false;
        int  value  = 0;

        NTCCFG_TEST_TRUE(map.empty());
        NTCCFG_TEST_EQ(map.size(), 0);

        result = map.find(&value, 1);
        NTCCFG_TEST_FALSE(result);

        result = map.add(1, 100);
        NTCCFG_TEST_TRUE(result);

        NTCCFG_TEST_FALSE(map.empty());
        NTCCFG_TEST_EQ(map.size(), 1);

        result = map.find(&value, 1);
        NTCCFG_TEST_TRUE(result);
        NTCCFG_TEST_EQ(value, 100);

        result = map.add(1, 10000);
        NTCCFG_TEST_FALSE(result);

        NTCCFG_TEST_FALSE(map.empty());
        NTCCFG_TEST_EQ(map.size(), 1);

        result = map.find(&value, 1);
        NTCCFG_TEST_TRUE(result);
        NTCCFG_TEST_EQ(value, 100);

        result = map.find(&value, 2);
        NTCCFG_TEST_FALSE(result);

        result = map.add(2, 200);
        NTCCFG_TEST_TRUE(result);

        NTCCFG_TEST_FALSE(map.empty());
        NTCCFG_TEST_EQ(map.size(), 2);

        result = map.find(&value, 2);
        NTCCFG_TEST_TRUE(result);
        NTCCFG_TEST_EQ(value, 200);

        result = map.add(2, 20000);
        NTCCFG_TEST_FALSE(result);

        NTCCFG_TEST_FALSE(map.empty());
        NTCCFG_TEST_EQ(map.size(), 2);

        result = map.find(&value, 2);
        NTCCFG_TEST_TRUE(result);
        NTCCFG_TEST_EQ(value, 200);

        result = map.find(&value, 1);
        NTCCFG_TEST_TRUE(result);
        NTCCFG_TEST_EQ(value, 100);

        {
            bsl::vector<int> keys;
            map.keys(&keys);
            NTCCFG_TEST_EQ(keys.size(), 2);
            {
                bsl::set<int> oracle(&ta);
                oracle.insert(1);
                oracle.insert(2);

                for (bsl::size_t i = 0; i < keys.size(); ++i) {
                    result = oracle.count(keys[i]) == 1;
                    NTCCFG_TEST_TRUE(result);
                    oracle.erase(keys[i]);
                }
            }
        }

        {
            bsl::vector<int> values;
            map.values(&values);
            NTCCFG_TEST_EQ(values.size(), 2);
            {
                bsl::set<int> oracle(&ta);
                oracle.insert(100);
                oracle.insert(200);

                for (bsl::size_t i = 0; i < values.size(); ++i) {
                    result = oracle.count(values[i]) == 1;
                    NTCCFG_TEST_TRUE(result);
                    oracle.erase(values[i]);
                }
            }
        }

        result = map.remove(3);
        NTCCFG_TEST_FALSE(result);

        NTCCFG_TEST_FALSE(map.empty());
        NTCCFG_TEST_EQ(map.size(), 2);

        result = map.remove(&value, 1);
        NTCCFG_TEST_TRUE(result);
        NTCCFG_TEST_EQ(value, 100);

        NTCCFG_TEST_FALSE(map.empty());
        NTCCFG_TEST_EQ(map.size(), 1);

        result = map.find(&value, 1);
        NTCCFG_TEST_FALSE(result);

        result = map.remove(&value, 1);
        NTCCFG_TEST_FALSE(result);

        result = map.find(&value, 2);
        NTCCFG_TEST_TRUE(result);
        NTCCFG_TEST_EQ(value, 200);

        result = map.replace(3, 300);
        NTCCFG_TEST_FALSE(result);

        result = map.replace(2, 20000);
        NTCCFG_TEST_TRUE(result);

        result = map.find(&value, 2);
        NTCCFG_TEST_TRUE(result);
        NTCCFG_TEST_EQ(value, 20000);

        map.clear();

        NTCCFG_TEST_TRUE(map.empty());
        NTCCFG_TEST_EQ(map.size(), 0);

        result = map.find(&value, 1);
        NTCCFG_TEST_FALSE(result);

        result = map.find(&value, 2);
        NTCCFG_TEST_FALSE(result);

        {
            MapType m1(&ta);
            MapType m2(&ta);

            m1.add(1, 100);
            m1.add(2, 200);

            NTCCFG_TEST_EQ(m1.size(), 2);

            result = m1.find(&value, 1);
            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_EQ(value, 100);

            result = m1.find(&value, 2);
            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_EQ(value, 200);

            m1.swap(&m2);

            NTCCFG_TEST_EQ(m1.size(), 0);

            result = m1.find(&value, 1);
            NTCCFG_TEST_FALSE(result);

            result = m1.find(&value, 2);
            NTCCFG_TEST_FALSE(result);

            NTCCFG_TEST_EQ(m2.size(), 2);

            result = m2.find(&value, 1);
            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_EQ(value, 100);

            result = m2.find(&value, 2);
            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_EQ(value, 200);
        }

        {
            MapType m1(&ta);
            MapType m2(&ta);

            m1.add(1, 100);
            m1.add(2, 200);

            NTCCFG_TEST_EQ(m1.size(), 2);

            result = m1.find(&value, 1);
            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_EQ(value, 100);

            result = m1.find(&value, 2);
            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_EQ(value, 200);

            result = m1.find(&value, 3);
            NTCCFG_TEST_FALSE(result);

            m2.add(3, 300);

            NTCCFG_TEST_EQ(m2.size(), 1);

            result = m2.find(&value, 1);
            NTCCFG_TEST_FALSE(result);

            result = m2.find(&value, 2);
            NTCCFG_TEST_FALSE(result);

            result = m2.find(&value, 3);
            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_EQ(value, 300);

            m1.swap(&m2);

            NTCCFG_TEST_EQ(m1.size(), 1);

            result = m1.find(&value, 1);
            NTCCFG_TEST_FALSE(result);

            result = m1.find(&value, 2);
            NTCCFG_TEST_FALSE(result);

            result = m1.find(&value, 3);
            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_EQ(value, 300);

            NTCCFG_TEST_EQ(m2.size(), 2);

            result = m2.find(&value, 1);
            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_EQ(value, 100);

            result = m2.find(&value, 2);
            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_EQ(value, 200);

            result = m2.find(&value, 3);
            NTCCFG_TEST_FALSE(result);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(6)
{
    NTCI_LOG_CONTEXT();

    ntccfg::TestAllocator ta;
    {
        typedef ntcdns::Queue<int> QueueType;

        QueueType queue(&ta);

        bool result = false;
        int  value  = 0;

        NTCCFG_TEST_TRUE(queue.empty());
        NTCCFG_TEST_EQ(queue.size(), 0);

        result = queue.pop(&value);
        NTCCFG_TEST_FALSE(result);

        queue.push(1);

        NTCCFG_TEST_FALSE(queue.empty());
        NTCCFG_TEST_EQ(queue.size(), 1);

        result = queue.pop(&value);
        NTCCFG_TEST_TRUE(result);
        NTCCFG_TEST_EQ(value, 1);

        NTCCFG_TEST_TRUE(queue.empty());
        NTCCFG_TEST_EQ(queue.size(), 0);

        result = queue.pop(&value);
        NTCCFG_TEST_FALSE(result);

        queue.push(1);

        NTCCFG_TEST_FALSE(queue.empty());
        NTCCFG_TEST_EQ(queue.size(), 1);

        queue.push(2);

        NTCCFG_TEST_FALSE(queue.empty());
        NTCCFG_TEST_EQ(queue.size(), 2);

        result = queue.pop(&value);
        NTCCFG_TEST_TRUE(result);
        NTCCFG_TEST_EQ(value, 1);

        NTCCFG_TEST_FALSE(queue.empty());
        NTCCFG_TEST_EQ(queue.size(), 1);

        result = queue.pop(&value);
        NTCCFG_TEST_TRUE(result);
        NTCCFG_TEST_EQ(value, 2);

        NTCCFG_TEST_TRUE(queue.empty());
        NTCCFG_TEST_EQ(queue.size(), 0);

        result = queue.pop(&value);
        NTCCFG_TEST_FALSE(result);

        queue.push(1);

        NTCCFG_TEST_FALSE(queue.empty());
        NTCCFG_TEST_EQ(queue.size(), 1);

        queue.clear();

        NTCCFG_TEST_TRUE(queue.empty());
        NTCCFG_TEST_EQ(queue.size(), 0);

        result = queue.pop(&value);
        NTCCFG_TEST_FALSE(result);

        {
            QueueType q1(&ta);
            QueueType q2(&ta);

            q1.push(1);
            q1.push(2);

            NTCCFG_TEST_EQ(q1.size(), 2);
            NTCCFG_TEST_EQ(q2.size(), 0);

            q1.swap(&q2);

            NTCCFG_TEST_EQ(q1.size(), 0);
            NTCCFG_TEST_EQ(q2.size(), 2);

            result = q1.pop(&value);
            NTCCFG_TEST_FALSE(result);

            result = q2.pop(&value);
            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_EQ(value, 1);

            result = q2.pop(&value);
            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_EQ(value, 2);

            result = q2.pop(&value);
            NTCCFG_TEST_FALSE(result);
        }

        {
            QueueType q1(&ta);
            QueueType q2(&ta);

            q1.push(1);

            NTCCFG_TEST_EQ(q1.size(), 1);

            q2.push(2);
            q2.push(3);

            NTCCFG_TEST_EQ(q2.size(), 2);

            q1.push(q2);

            NTCCFG_TEST_EQ(q1.size(), 3);
            NTCCFG_TEST_EQ(q2.size(), 2);

            result = q1.pop(&value);
            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_EQ(value, 1);

            result = q1.pop(&value);
            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_EQ(value, 2);

            result = q1.pop(&value);
            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_EQ(value, 3);

            result = q1.pop(&value);
            NTCCFG_TEST_FALSE(result);

            result = q2.pop(&value);
            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_EQ(value, 2);

            result = q2.pop(&value);
            NTCCFG_TEST_TRUE(result);
            NTCCFG_TEST_EQ(value, 3);

            result = q2.pop(&value);
            NTCCFG_TEST_FALSE(result);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
    NTCCFG_TEST_REGISTER(5);
    NTCCFG_TEST_REGISTER(6);
}
NTCCFG_TEST_DRIVER_END;
