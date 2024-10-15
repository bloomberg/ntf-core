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
BSLS_IDENT_RCSID(ntcdns_utility_t_cpp, "$Id$ $CSID$")

#include <ntcdns_utility.h>

#include <ntci_log.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcdns {

// Provide tests for 'ntcdns::Utility'.
class UtilityTest
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
};

NTSCFG_TEST_FUNCTION(ntcdns::UtilityTest::verifyCase1)
{
    // Concern: Client configurations.

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntcdns::ClientConfig clientConfig(NTSCFG_TEST_ALLOCATOR);
    error = ntcdns::Utility::loadClientConfig(&clientConfig);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_TRACE << "Client configuration = " << clientConfig
                          << NTCI_LOG_STREAM_END;
}

NTSCFG_TEST_FUNCTION(ntcdns::UtilityTest::verifyCase2)
{
    // Concern: Host database configurations.

    if (!ntscfg::Platform::hasHostDatabase()) {
        return;
    }

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntcdns::HostDatabaseConfig hostDatabaseConfig(NTSCFG_TEST_ALLOCATOR);
    error = ntcdns::Utility::loadHostDatabaseConfig(&hostDatabaseConfig);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_TRACE << "Host database = " << hostDatabaseConfig
                          << NTCI_LOG_STREAM_END;
}

NTSCFG_TEST_FUNCTION(ntcdns::UtilityTest::verifyCase3)
{
    // Concern: Port database configurations.

    if (!ntscfg::Platform::hasPortDatabase()) {
        return;
    }

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntcdns::PortDatabaseConfig portDatabaseConfig(NTSCFG_TEST_ALLOCATOR);
    error = ntcdns::Utility::loadPortDatabaseConfig(&portDatabaseConfig);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_TRACE << "Port database = " << portDatabaseConfig
                          << NTCI_LOG_STREAM_END;
}

NTSCFG_TEST_FUNCTION(ntcdns::UtilityTest::verifyCase4)
{
    // Concern: Resolver configurations.

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntcdns::ResolverConfig resolverConfig(NTSCFG_TEST_ALLOCATOR);
    error = ntcdns::Utility::loadResolverConfig(&resolverConfig);
    NTSCFG_TEST_OK(error);

    NTCI_LOG_STREAM_TRACE << "Resolver configuration = " << resolverConfig
                          << NTCI_LOG_STREAM_END;
}

NTSCFG_TEST_FUNCTION(ntcdns::UtilityTest::verifyCase5)
{
    NTCI_LOG_CONTEXT();

    typedef ntcdns::Map<int, int> MapType;

    MapType map(NTSCFG_TEST_ALLOCATOR);

    bool result = false;
    int  value  = 0;

    NTSCFG_TEST_TRUE(map.empty());
    NTSCFG_TEST_EQ(map.size(), 0);

    result = map.find(&value, 1);
    NTSCFG_TEST_FALSE(result);

    result = map.add(1, 100);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_FALSE(map.empty());
    NTSCFG_TEST_EQ(map.size(), 1);

    result = map.find(&value, 1);
    NTSCFG_TEST_TRUE(result);
    NTSCFG_TEST_EQ(value, 100);

    result = map.add(1, 10000);
    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_FALSE(map.empty());
    NTSCFG_TEST_EQ(map.size(), 1);

    result = map.find(&value, 1);
    NTSCFG_TEST_TRUE(result);
    NTSCFG_TEST_EQ(value, 100);

    result = map.find(&value, 2);
    NTSCFG_TEST_FALSE(result);

    result = map.add(2, 200);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_FALSE(map.empty());
    NTSCFG_TEST_EQ(map.size(), 2);

    result = map.find(&value, 2);
    NTSCFG_TEST_TRUE(result);
    NTSCFG_TEST_EQ(value, 200);

    result = map.add(2, 20000);
    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_FALSE(map.empty());
    NTSCFG_TEST_EQ(map.size(), 2);

    result = map.find(&value, 2);
    NTSCFG_TEST_TRUE(result);
    NTSCFG_TEST_EQ(value, 200);

    result = map.find(&value, 1);
    NTSCFG_TEST_TRUE(result);
    NTSCFG_TEST_EQ(value, 100);

    {
        bsl::vector<int> keys;
        map.keys(&keys);
        NTSCFG_TEST_EQ(keys.size(), 2);
        {
            bsl::set<int> oracle(NTSCFG_TEST_ALLOCATOR);
            oracle.insert(1);
            oracle.insert(2);

            for (bsl::size_t i = 0; i < keys.size(); ++i) {
                result = oracle.count(keys[i]) == 1;
                NTSCFG_TEST_TRUE(result);
                oracle.erase(keys[i]);
            }
        }
    }

    {
        bsl::vector<int> values;
        map.values(&values);
        NTSCFG_TEST_EQ(values.size(), 2);
        {
            bsl::set<int> oracle(NTSCFG_TEST_ALLOCATOR);
            oracle.insert(100);
            oracle.insert(200);

            for (bsl::size_t i = 0; i < values.size(); ++i) {
                result = oracle.count(values[i]) == 1;
                NTSCFG_TEST_TRUE(result);
                oracle.erase(values[i]);
            }
        }
    }

    result = map.remove(3);
    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_FALSE(map.empty());
    NTSCFG_TEST_EQ(map.size(), 2);

    result = map.remove(&value, 1);
    NTSCFG_TEST_TRUE(result);
    NTSCFG_TEST_EQ(value, 100);

    NTSCFG_TEST_FALSE(map.empty());
    NTSCFG_TEST_EQ(map.size(), 1);

    result = map.find(&value, 1);
    NTSCFG_TEST_FALSE(result);

    result = map.remove(&value, 1);
    NTSCFG_TEST_FALSE(result);

    result = map.find(&value, 2);
    NTSCFG_TEST_TRUE(result);
    NTSCFG_TEST_EQ(value, 200);

    result = map.replace(3, 300);
    NTSCFG_TEST_FALSE(result);

    result = map.replace(2, 20000);
    NTSCFG_TEST_TRUE(result);

    result = map.find(&value, 2);
    NTSCFG_TEST_TRUE(result);
    NTSCFG_TEST_EQ(value, 20000);

    map.clear();

    NTSCFG_TEST_TRUE(map.empty());
    NTSCFG_TEST_EQ(map.size(), 0);

    result = map.find(&value, 1);
    NTSCFG_TEST_FALSE(result);

    result = map.find(&value, 2);
    NTSCFG_TEST_FALSE(result);

    {
        MapType m1(NTSCFG_TEST_ALLOCATOR);
        MapType m2(NTSCFG_TEST_ALLOCATOR);

        m1.add(1, 100);
        m1.add(2, 200);

        NTSCFG_TEST_EQ(m1.size(), 2);

        result = m1.find(&value, 1);
        NTSCFG_TEST_TRUE(result);
        NTSCFG_TEST_EQ(value, 100);

        result = m1.find(&value, 2);
        NTSCFG_TEST_TRUE(result);
        NTSCFG_TEST_EQ(value, 200);

        m1.swap(&m2);

        NTSCFG_TEST_EQ(m1.size(), 0);

        result = m1.find(&value, 1);
        NTSCFG_TEST_FALSE(result);

        result = m1.find(&value, 2);
        NTSCFG_TEST_FALSE(result);

        NTSCFG_TEST_EQ(m2.size(), 2);

        result = m2.find(&value, 1);
        NTSCFG_TEST_TRUE(result);
        NTSCFG_TEST_EQ(value, 100);

        result = m2.find(&value, 2);
        NTSCFG_TEST_TRUE(result);
        NTSCFG_TEST_EQ(value, 200);
    }

    {
        MapType m1(NTSCFG_TEST_ALLOCATOR);
        MapType m2(NTSCFG_TEST_ALLOCATOR);

        m1.add(1, 100);
        m1.add(2, 200);

        NTSCFG_TEST_EQ(m1.size(), 2);

        result = m1.find(&value, 1);
        NTSCFG_TEST_TRUE(result);
        NTSCFG_TEST_EQ(value, 100);

        result = m1.find(&value, 2);
        NTSCFG_TEST_TRUE(result);
        NTSCFG_TEST_EQ(value, 200);

        result = m1.find(&value, 3);
        NTSCFG_TEST_FALSE(result);

        m2.add(3, 300);

        NTSCFG_TEST_EQ(m2.size(), 1);

        result = m2.find(&value, 1);
        NTSCFG_TEST_FALSE(result);

        result = m2.find(&value, 2);
        NTSCFG_TEST_FALSE(result);

        result = m2.find(&value, 3);
        NTSCFG_TEST_TRUE(result);
        NTSCFG_TEST_EQ(value, 300);

        m1.swap(&m2);

        NTSCFG_TEST_EQ(m1.size(), 1);

        result = m1.find(&value, 1);
        NTSCFG_TEST_FALSE(result);

        result = m1.find(&value, 2);
        NTSCFG_TEST_FALSE(result);

        result = m1.find(&value, 3);
        NTSCFG_TEST_TRUE(result);
        NTSCFG_TEST_EQ(value, 300);

        NTSCFG_TEST_EQ(m2.size(), 2);

        result = m2.find(&value, 1);
        NTSCFG_TEST_TRUE(result);
        NTSCFG_TEST_EQ(value, 100);

        result = m2.find(&value, 2);
        NTSCFG_TEST_TRUE(result);
        NTSCFG_TEST_EQ(value, 200);

        result = m2.find(&value, 3);
        NTSCFG_TEST_FALSE(result);
    }
}

NTSCFG_TEST_FUNCTION(ntcdns::UtilityTest::verifyCase6)
{
    NTCI_LOG_CONTEXT();

    typedef ntcdns::Queue<int> QueueType;

    QueueType queue(NTSCFG_TEST_ALLOCATOR);

    bool result = false;
    int  value  = 0;

    NTSCFG_TEST_TRUE(queue.empty());
    NTSCFG_TEST_EQ(queue.size(), 0);

    result = queue.pop(&value);
    NTSCFG_TEST_FALSE(result);

    queue.push(1);

    NTSCFG_TEST_FALSE(queue.empty());
    NTSCFG_TEST_EQ(queue.size(), 1);

    result = queue.pop(&value);
    NTSCFG_TEST_TRUE(result);
    NTSCFG_TEST_EQ(value, 1);

    NTSCFG_TEST_TRUE(queue.empty());
    NTSCFG_TEST_EQ(queue.size(), 0);

    result = queue.pop(&value);
    NTSCFG_TEST_FALSE(result);

    queue.push(1);

    NTSCFG_TEST_FALSE(queue.empty());
    NTSCFG_TEST_EQ(queue.size(), 1);

    queue.push(2);

    NTSCFG_TEST_FALSE(queue.empty());
    NTSCFG_TEST_EQ(queue.size(), 2);

    result = queue.pop(&value);
    NTSCFG_TEST_TRUE(result);
    NTSCFG_TEST_EQ(value, 1);

    NTSCFG_TEST_FALSE(queue.empty());
    NTSCFG_TEST_EQ(queue.size(), 1);

    result = queue.pop(&value);
    NTSCFG_TEST_TRUE(result);
    NTSCFG_TEST_EQ(value, 2);

    NTSCFG_TEST_TRUE(queue.empty());
    NTSCFG_TEST_EQ(queue.size(), 0);

    result = queue.pop(&value);
    NTSCFG_TEST_FALSE(result);

    queue.push(1);

    NTSCFG_TEST_FALSE(queue.empty());
    NTSCFG_TEST_EQ(queue.size(), 1);

    queue.clear();

    NTSCFG_TEST_TRUE(queue.empty());
    NTSCFG_TEST_EQ(queue.size(), 0);

    result = queue.pop(&value);
    NTSCFG_TEST_FALSE(result);

    {
        QueueType q1(NTSCFG_TEST_ALLOCATOR);
        QueueType q2(NTSCFG_TEST_ALLOCATOR);

        q1.push(1);
        q1.push(2);

        NTSCFG_TEST_EQ(q1.size(), 2);
        NTSCFG_TEST_EQ(q2.size(), 0);

        q1.swap(&q2);

        NTSCFG_TEST_EQ(q1.size(), 0);
        NTSCFG_TEST_EQ(q2.size(), 2);

        result = q1.pop(&value);
        NTSCFG_TEST_FALSE(result);

        result = q2.pop(&value);
        NTSCFG_TEST_TRUE(result);
        NTSCFG_TEST_EQ(value, 1);

        result = q2.pop(&value);
        NTSCFG_TEST_TRUE(result);
        NTSCFG_TEST_EQ(value, 2);

        result = q2.pop(&value);
        NTSCFG_TEST_FALSE(result);
    }

    {
        QueueType q1(NTSCFG_TEST_ALLOCATOR);
        QueueType q2(NTSCFG_TEST_ALLOCATOR);

        q1.push(1);

        NTSCFG_TEST_EQ(q1.size(), 1);

        q2.push(2);
        q2.push(3);

        NTSCFG_TEST_EQ(q2.size(), 2);

        q1.push(q2);

        NTSCFG_TEST_EQ(q1.size(), 3);
        NTSCFG_TEST_EQ(q2.size(), 2);

        result = q1.pop(&value);
        NTSCFG_TEST_TRUE(result);
        NTSCFG_TEST_EQ(value, 1);

        result = q1.pop(&value);
        NTSCFG_TEST_TRUE(result);
        NTSCFG_TEST_EQ(value, 2);

        result = q1.pop(&value);
        NTSCFG_TEST_TRUE(result);
        NTSCFG_TEST_EQ(value, 3);

        result = q1.pop(&value);
        NTSCFG_TEST_FALSE(result);

        result = q2.pop(&value);
        NTSCFG_TEST_TRUE(result);
        NTSCFG_TEST_EQ(value, 2);

        result = q2.pop(&value);
        NTSCFG_TEST_TRUE(result);
        NTSCFG_TEST_EQ(value, 3);

        result = q2.pop(&value);
        NTSCFG_TEST_FALSE(result);
    }
}

}  // close namespace ntcdns
}  // close namespace BloombergLP
