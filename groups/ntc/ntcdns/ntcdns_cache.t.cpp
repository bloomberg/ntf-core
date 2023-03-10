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

#include <ntcdns_cache.h>

#include <ntccfg_test.h>
#include <ntcdns_utility.h>
#include <ntci_log.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_iostream.h>

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

namespace test {

int s_now = 0;

bsls::TimeInterval getNow()
{
    return bsls::TimeInterval(s_now, 0);
}

void advanceNow()
{
    ++s_now;
}

void processGetIpAddressResult(
    const bsl::vector<ntsa::IpAddress>& ipAddressList,
    const ntca::GetIpAddressContext&    context)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get IP address context " << context
                          << NTCI_LOG_STREAM_END;

    if (!context.error()) {
        if (ipAddressList.size() > 0) {
            for (bsl::size_t i = 0; i < ipAddressList.size(); ++i) {
                const ntsa::IpAddress& ipAddress = ipAddressList[i];
                NTCI_LOG_STREAM_INFO << "The domain name '"
                                     << context.domainName()
                                     << "' has resolved to " << ipAddress
                                     << NTCI_LOG_STREAM_END;
            }
        }
        else {
            NTCI_LOG_STREAM_INFO << "The domain name '" << context.domainName()
                                 << "' has no IP addresses assigned"
                                 << NTCI_LOG_STREAM_END;
        }
    }
}

void processGetDomainNameResult(const bsl::string&                domainName,
                                const ntca::GetDomainNameContext& context)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get domain name context " << context
                          << NTCI_LOG_STREAM_END;

    if (!context.error()) {
        if (domainName.size() > 0) {
            NTCI_LOG_STREAM_INFO << "The IP address " << context.ipAddress()
                                 << " has resolved to '" << domainName << "'"
                                 << NTCI_LOG_STREAM_END;
        }
        else {
            NTCI_LOG_STREAM_INFO << "The IP address " << context.ipAddress()
                                 << " is not assigned to any domain name"
                                 << NTCI_LOG_STREAM_END;
        }
    }
}

}  // close namespace 'test'

NTCCFG_TEST_CASE(1)
{
    // Concern: Test 'getIpAddress' insertion, lookup, and expiration.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Create a cache.

        ntcdns::Cache cache(&ta);

        // Define a test domain name assigned to an IP address from a
        // name server with a TTL of 2.

        const bsl::string     DOMAIN_NAME("test.example.com");
        const ntsa::Endpoint  NAME_SERVER("127.0.0.1:53");
        const ntsa::IpAddress IP_ADDRESS("192.168.0.101");
        const bsl::size_t     TTL = 2;

        // Ensure no IP addresses are initially cached.

        NTCCFG_TEST_EQ(cache.numHostEntries(), 0);

        // Get the IP addresses assigned to the domain name. Ensure the
        // operation fails: no IP addresses for the domain name are yet cached.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = cache.getIpAddress(&context,
                                       &ipAddressList,
                                       DOMAIN_NAME,
                                       options,
                                       test::getNow());
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        // Insert IP addresses for a domain name at T 0 with a TTL of 2.

        cache.updateHost(DOMAIN_NAME,
                         IP_ADDRESS,
                         NAME_SERVER,
                         TTL,
                         test::getNow());

        // Ensure the IP address is now cached.

        NTCCFG_TEST_EQ(cache.numHostEntries(), 1);

        // Get the IP addresses assigned to the domain name at T 0.
        // Ensure the operation succeeds because T < 2.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = cache.getIpAddress(&context,
                                       &ipAddressList,
                                       DOMAIN_NAME,
                                       options,
                                       test::getNow());
            NTCCFG_TEST_OK(error);

            test::processGetIpAddressResult(ipAddressList, context);

            NTCCFG_TEST_EQ(ipAddressList.size(), 1);
            NTCCFG_TEST_EQ(ipAddressList[0], IP_ADDRESS);

            NTCCFG_TEST_EQ(context.nameServer(), NAME_SERVER);
            NTCCFG_TEST_EQ(context.timeToLive(), TTL);
        }

        // Advance time to T 1.

        test::advanceNow();

        // Get the IP addresses assigned to the domain name at T 1.
        // Ensure the operation succeeds because T < 2.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = cache.getIpAddress(&context,
                                       &ipAddressList,
                                       DOMAIN_NAME,
                                       options,
                                       test::getNow());
            NTCCFG_TEST_OK(error);

            test::processGetIpAddressResult(ipAddressList, context);

            NTCCFG_TEST_EQ(ipAddressList.size(), 1);
            NTCCFG_TEST_EQ(ipAddressList[0], IP_ADDRESS);

            NTCCFG_TEST_EQ(context.nameServer(), NAME_SERVER);
            NTCCFG_TEST_EQ(context.timeToLive(), TTL - 1);
        }

        // Advance time to T 2.

        test::advanceNow();

        // Get the IP addresses assigned to the domain name at T 1.
        // Ensure the operation fails because T >= 2.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = cache.getIpAddress(&context,
                                       &ipAddressList,
                                       DOMAIN_NAME,
                                       options,
                                       test::getNow());
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        // Ensure the previously lookup has caused the previously cached IP
        // address to be evicted.

        NTCCFG_TEST_EQ(cache.numHostEntries(), 0);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern: Test 'getIpAddress' insertion, lookup, and update.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Create a cache.

        ntcdns::Cache cache(&ta);

        // Define a test domain name assigned to an IP address from a
        // name server with a TTL of 2.

        const bsl::string     DOMAIN_NAME("test.example.com");
        const ntsa::Endpoint  NAME_SERVER_1("127.0.0.1:53");
        const ntsa::Endpoint  NAME_SERVER_2("10.10.0.1:53");
        const ntsa::IpAddress IP_ADDRESS("192.168.0.101");
        const bsl::size_t     TTL = 2;

        // Ensure no IP addresses are initially cached.

        NTCCFG_TEST_EQ(cache.numHostEntries(), 0);

        // Get the IP addresses assigned to the domain name. Ensure the
        // operation fails: no IP addresses for the domain name are yet cached.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = cache.getIpAddress(&context,
                                       &ipAddressList,
                                       DOMAIN_NAME,
                                       options,
                                       test::getNow());
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        // Insert IP addresses for a domain name at T 0 with a TTL of 2.

        cache.updateHost(DOMAIN_NAME,
                         IP_ADDRESS,
                         NAME_SERVER_1,
                         TTL,
                         test::getNow());

        // Ensure the IP address is now cached.

        NTCCFG_TEST_EQ(cache.numHostEntries(), 1);

        // Get the IP addresses assigned to the domain name at T 0.
        // Ensure the operation succeeds because T < 2.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = cache.getIpAddress(&context,
                                       &ipAddressList,
                                       DOMAIN_NAME,
                                       options,
                                       test::getNow());
            NTCCFG_TEST_OK(error);

            test::processGetIpAddressResult(ipAddressList, context);

            NTCCFG_TEST_EQ(ipAddressList.size(), 1);
            NTCCFG_TEST_EQ(ipAddressList[0], IP_ADDRESS);

            NTCCFG_TEST_EQ(context.nameServer(), NAME_SERVER_1);
            NTCCFG_TEST_EQ(context.timeToLive(), TTL);
        }

        // Advance time to T 1.

        test::advanceNow();

        // Get the IP addresses assigned to the domain name at T 1.
        // Ensure the operation succeeds because T < 2.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = cache.getIpAddress(&context,
                                       &ipAddressList,
                                       DOMAIN_NAME,
                                       options,
                                       test::getNow());
            NTCCFG_TEST_OK(error);

            test::processGetIpAddressResult(ipAddressList, context);

            NTCCFG_TEST_EQ(ipAddressList.size(), 1);
            NTCCFG_TEST_EQ(ipAddressList[0], IP_ADDRESS);

            NTCCFG_TEST_EQ(context.nameServer(), NAME_SERVER_1);
            NTCCFG_TEST_EQ(context.timeToLive(), TTL - 1);
        }

        // Advance time to T 2.

        test::advanceNow();

        // Update IP addresses for a domain name at T 0 with a TTL of 4.

        cache.updateHost(DOMAIN_NAME,
                         IP_ADDRESS,
                         NAME_SERVER_2,
                         2 * TTL,
                         test::getNow());

        // Get the IP addresses assigned to the domain name at T 1. If the
        // entry hadn't been previously updated, this operation would fail
        // because T >= 2, the original TTL. However, since it has been updated
        // the TTL is extended. Ensure the operation succeeds, and the context
        // describes the new TTL and the new name server.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = cache.getIpAddress(&context,
                                       &ipAddressList,
                                       DOMAIN_NAME,
                                       options,
                                       test::getNow());

            NTCCFG_TEST_OK(error);

            test::processGetIpAddressResult(ipAddressList, context);

            NTCCFG_TEST_EQ(ipAddressList.size(), 1);
            NTCCFG_TEST_EQ(ipAddressList[0], IP_ADDRESS);

            NTCCFG_TEST_EQ(context.nameServer(), NAME_SERVER_2);
            NTCCFG_TEST_EQ(context.timeToLive(), 2 * TTL);
        }

        // Ensure the previously lookup has caused the previously cached IP
        // address's lifetime to be extended.

        NTCCFG_TEST_EQ(cache.numHostEntries(), 1);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Concern: Test 'getDomainName' insertion, lookup, and expiration.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Create a cache.

        ntcdns::Cache cache(&ta);

        // Define a test domain name assigned to an IP address from a
        // name server with a TTL of 2.

        const bsl::string     DOMAIN_NAME("test.example.com");
        const ntsa::Endpoint  NAME_SERVER("127.0.0.1:53");
        const ntsa::IpAddress IP_ADDRESS("192.168.0.101");
        const bsl::size_t     TTL = 2;

        // Ensure no IP addresses are initially cached.

        NTCCFG_TEST_EQ(cache.numHostEntries(), 0);

        // Get the domain name name to which the IP address is assigned. Ensure
        // the operation fails: no domain name for the IP address is yet
        // cached.

        {
            ntca::GetDomainNameContext context;
            ntca::GetDomainNameOptions options;

            bsl::string domainName;
            error = cache.getDomainName(&context,
                                        &domainName,
                                        IP_ADDRESS,
                                        options,
                                        test::getNow());
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        // Insert IP addresses for a domain name at T 0 with a TTL of 2.

        cache.updateHost(DOMAIN_NAME,
                         IP_ADDRESS,
                         NAME_SERVER,
                         TTL,
                         test::getNow());

        // Ensure the domain name is now cached.

        NTCCFG_TEST_EQ(cache.numHostEntries(), 1);

        // Get the domain name to which the IP address is assigned at T 0.
        // Ensure the operation succeeds because T < 2.

        {
            ntca::GetDomainNameContext context;
            ntca::GetDomainNameOptions options;

            bsl::string domainName;
            error = cache.getDomainName(&context,
                                        &domainName,
                                        IP_ADDRESS,
                                        options,
                                        test::getNow());
            NTCCFG_TEST_OK(error);

            test::processGetDomainNameResult(domainName, context);

            NTCCFG_TEST_EQ(domainName, DOMAIN_NAME);
            NTCCFG_TEST_EQ(context.nameServer(), NAME_SERVER);
            NTCCFG_TEST_EQ(context.timeToLive(), TTL);
        }

        // Advance time to T 1.

        test::advanceNow();

        // Get the domain name to which the IP address is assigned at T 1.
        // Ensure the operation succeeds because T < 2.

        {
            ntca::GetDomainNameContext context;
            ntca::GetDomainNameOptions options;

            bsl::string domainName;
            error = cache.getDomainName(&context,
                                        &domainName,
                                        IP_ADDRESS,
                                        options,
                                        test::getNow());
            NTCCFG_TEST_OK(error);

            test::processGetDomainNameResult(domainName, context);

            NTCCFG_TEST_EQ(domainName, DOMAIN_NAME);
            NTCCFG_TEST_EQ(context.nameServer(), NAME_SERVER);
            NTCCFG_TEST_EQ(context.timeToLive(), TTL - 1);
        }

        // Advance time to T 2.

        test::advanceNow();

        // Get the domain name to which the IP address is assigned at T 1.
        // Ensure the operation fails because T >= 2.

        {
            ntca::GetDomainNameContext context;
            ntca::GetDomainNameOptions options;

            bsl::string domainName;
            error = cache.getDomainName(&context,
                                        &domainName,
                                        IP_ADDRESS,
                                        options,
                                        test::getNow());
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        // Ensure the previously lookup has caused the previously cached domain
        // name to be evicted.

        NTCCFG_TEST_EQ(cache.numHostEntries(), 0);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    // Concern: Test 'getDomainName' insertion, lookup, and update.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Create a cache.

        ntcdns::Cache cache(&ta);

        // Define a test domain name assigned to an IP address from a
        // name server with a TTL of 2.

        const bsl::string     DOMAIN_NAME("test.example.com");
        const ntsa::Endpoint  NAME_SERVER_1("127.0.0.1:53");
        const ntsa::Endpoint  NAME_SERVER_2("10.10.0.1:53");
        const ntsa::IpAddress IP_ADDRESS("192.168.0.101");
        const bsl::size_t     TTL = 2;

        // Ensure no IP addresses are initially cached.

        NTCCFG_TEST_EQ(cache.numHostEntries(), 0);

        // Get the domain name to which the IP address is assigned. Ensure the
        // operation fails: no domain name for the IP address is yet cached.

        {
            ntca::GetDomainNameContext context;
            ntca::GetDomainNameOptions options;

            bsl::string domainName;
            error = cache.getDomainName(&context,
                                        &domainName,
                                        IP_ADDRESS,
                                        options,
                                        test::getNow());
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        // Insert IP addresses for a domain name at T 0 with a TTL of 2.

        cache.updateHost(DOMAIN_NAME,
                         IP_ADDRESS,
                         NAME_SERVER_1,
                         TTL,
                         test::getNow());

        // Ensure the domain name is now cached.

        NTCCFG_TEST_EQ(cache.numHostEntries(), 1);

        // Get the domain name to which the IP address is assigned at T 0.
        // Ensure the operation succeeds because T < 2.

        {
            ntca::GetDomainNameContext context;
            ntca::GetDomainNameOptions options;

            bsl::string domainName;
            error = cache.getDomainName(&context,
                                        &domainName,
                                        IP_ADDRESS,
                                        options,
                                        test::getNow());
            NTCCFG_TEST_OK(error);

            test::processGetDomainNameResult(domainName, context);

            NTCCFG_TEST_EQ(domainName, DOMAIN_NAME);
            NTCCFG_TEST_EQ(context.nameServer(), NAME_SERVER_1);
            NTCCFG_TEST_EQ(context.timeToLive(), TTL);
        }

        // Advance time to T 1.

        test::advanceNow();

        // Get the domain name to which the IP address is assigned at T 1.
        // Ensure the operation succeeds because T < 2.

        {
            ntca::GetDomainNameContext context;
            ntca::GetDomainNameOptions options;

            bsl::string domainName;
            error = cache.getDomainName(&context,
                                        &domainName,
                                        IP_ADDRESS,
                                        options,
                                        test::getNow());
            NTCCFG_TEST_OK(error);

            test::processGetDomainNameResult(domainName, context);

            NTCCFG_TEST_EQ(domainName, DOMAIN_NAME);
            NTCCFG_TEST_EQ(context.nameServer(), NAME_SERVER_1);
            NTCCFG_TEST_EQ(context.timeToLive(), TTL - 1);
        }

        // Advance time to T 2.

        test::advanceNow();

        // Update IP addresses for a domain name at T 0 with a TTL of 4.

        cache.updateHost(DOMAIN_NAME,
                         IP_ADDRESS,
                         NAME_SERVER_2,
                         2 * TTL,
                         test::getNow());

        // Get the domain name to which the IP address is assigned at T 1. If
        // the entry hadn't been previously updated, this operation would fail
        // because T >= 2, the original TTL. However, since it has been updated
        // the TTL is extended. Ensure the operation succeeds, and the context
        // describes the new TTL and the new name server.

        {
            ntca::GetDomainNameContext context;
            ntca::GetDomainNameOptions options;

            bsl::string domainName;
            error = cache.getDomainName(&context,
                                        &domainName,
                                        IP_ADDRESS,
                                        options,
                                        test::getNow());

            NTCCFG_TEST_OK(error);

            test::processGetDomainNameResult(domainName, context);

            NTCCFG_TEST_EQ(domainName, DOMAIN_NAME);
            NTCCFG_TEST_EQ(context.nameServer(), NAME_SERVER_2);
            NTCCFG_TEST_EQ(context.timeToLive(), 2 * TTL);
        }

        // Ensure the previously lookup has caused the previously cached domain
        // names's lifetime to be extended.

        NTCCFG_TEST_EQ(cache.numHostEntries(), 1);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
}
NTCCFG_TEST_DRIVER_END;
