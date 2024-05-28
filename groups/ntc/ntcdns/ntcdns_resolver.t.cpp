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

#include <ntcdns_resolver.h>

#include <ntccfg_test.h>
#include <ntci_log.h>
#include <bslmt_semaphore.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

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

void processGetIpAddressResult(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const bsl::vector<ntsa::IpAddress>&    ipAddressList,
    const ntca::GetIpAddressEvent&         event,
    ntca::ResolverSource::Value            source,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get IP address event " << event
                          << NTCI_LOG_STREAM_END;

    if (event.type() == ntca::GetIpAddressEventType::e_COMPLETE) {
        if (ipAddressList.size() > 0) {
            NTCCFG_TEST_EQ(event.context().source(), source);
            for (bsl::size_t i = 0; i < ipAddressList.size(); ++i) {
                const ntsa::IpAddress& ipAddress = ipAddressList[i];
                NTCI_LOG_STREAM_INFO << "The domain name '"
                                     << event.context().domainName()
                                     << "' has resolved to " << ipAddress
                                     << NTCI_LOG_STREAM_END;
            }
        }
        else {
            NTCI_LOG_STREAM_INFO
                << "The domain name '" << event.context().domainName()
                << "' has no IP addresses assigned" << NTCI_LOG_STREAM_END;
        }
    }

    semaphore->post();
}

void processGetDomainNameResult(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const bsl::string&                     domainName,
    const ntca::GetDomainNameEvent&        event,
    ntca::ResolverSource::Value            source,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get domain name event " << event
                          << NTCI_LOG_STREAM_END;

    if (event.type() == ntca::GetDomainNameEventType::e_COMPLETE) {
        if (!domainName.empty()) {
            NTCCFG_TEST_EQ(event.context().source(), source);
            NTCI_LOG_STREAM_INFO
                << "The IP address " << event.context().ipAddress()
                << " has resolved to the domain name '" << domainName << "'"
                << NTCI_LOG_STREAM_END;
        }
        else {
            NTCI_LOG_STREAM_INFO << "The IP address '"
                                 << event.context().ipAddress()
                                 << "' is not assigned to any domain name"
                                 << NTCI_LOG_STREAM_END;
        }
    }

    semaphore->post();
}

void processGetPortResult(const bsl::shared_ptr<ntci::Resolver>& resolver,
                          const bsl::vector<ntsa::Port>&         portList,
                          const ntca::GetPortEvent&              event,
                          ntca::ResolverSource::Value            source,
                          bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get port event " << event
                          << NTCI_LOG_STREAM_END;

    if (event.type() == ntca::GetPortEventType::e_COMPLETE) {
        if (portList.size() > 0) {
            NTCCFG_TEST_EQ(event.context().source(), source);
            for (bsl::size_t i = 0; i < portList.size(); ++i) {
                ntsa::Port port = portList[i];
                NTCI_LOG_STREAM_INFO << "The service name '"
                                     << event.context().serviceName()
                                     << "' has resolved to port " << port
                                     << NTCI_LOG_STREAM_END;
            }
        }
        else {
            NTCI_LOG_STREAM_INFO
                << "The service name '" << event.context().serviceName()
                << "' has no ports assigned" << NTCI_LOG_STREAM_END;
        }
    }

    semaphore->post();
}

void processGetServiceNameResult(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const bsl::string&                     serviceName,
    const ntca::GetServiceNameEvent&       event,
    ntca::ResolverSource::Value            source,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get service name event " << event
                          << NTCI_LOG_STREAM_END;

    if (event.type() == ntca::GetServiceNameEventType::e_COMPLETE) {
        if (!serviceName.empty()) {
            NTCCFG_TEST_EQ(event.context().source(), source);
            NTCI_LOG_STREAM_INFO << "The port " << event.context().port()
                                 << " has resolved to the service name '"
                                 << serviceName << "'" << NTCI_LOG_STREAM_END;
        }
        else {
            NTCI_LOG_STREAM_INFO << "The port '" << event.context().port()
                                 << "' is not assigned to any service name"
                                 << NTCI_LOG_STREAM_END;
        }
    }

    semaphore->post();
}

void processGetEndpointResult(const bsl::shared_ptr<ntci::Resolver>& resolver,
                              const ntsa::Endpoint&                  endpoint,
                              const ntca::GetEndpointEvent&          event,
                              ntca::ResolverSource::Value            source,
                              bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get endpoint event " << event
                          << NTCI_LOG_STREAM_END;

    if (event.type() == ntca::GetEndpointEventType::e_COMPLETE) {
        NTCCFG_TEST_EQ(event.context().source(), source);
        NTCI_LOG_STREAM_INFO
            << "The authority '" << event.context().authority()
            << "' has resolved to " << endpoint << NTCI_LOG_STREAM_END;
    }

    semaphore->post();
}

}  // close namespace test

//
// Test overrides.
//

NTCCFG_TEST_CASE(1)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getIpAddress' from overrides.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(false);
        resolverConfig.setPortDatabaseEnabled(false);
        resolverConfig.setPositiveCacheEnabled(false);
        resolverConfig.setNegativeCacheEnabled(false);
        resolverConfig.setSystemEnabled(false);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Set overrides.

        error = resolver->addIpAddress("test.example.net",
                                       ntsa::IpAddress("192.168.0.100"));
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetIpAddressCallback callback =
            resolver->createGetIpAddressCallback(
                bdlf::BindUtil::bind(&test::processGetIpAddressResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_OVERRIDE,
                                     &semaphore),
                &ta);

        // Define the options.

        ntca::GetIpAddressOptions options;
        options.setIpAddressType(ntsa::IpAddressType::e_V4);

        // Get the IP addresses assigned to "test.example.net".

        error = resolver->getIpAddress("test.example.net", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the IP addresses assigned to "test.example.net".

        error = resolver->getIpAddress("test.example.net", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(2)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getDomainName' from overrides.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(false);
        resolverConfig.setPortDatabaseEnabled(false);
        resolverConfig.setPositiveCacheEnabled(false);
        resolverConfig.setNegativeCacheEnabled(false);
        resolverConfig.setSystemEnabled(false);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Set overrides.

        error = resolver->addIpAddress("test.example.net",
                                       ntsa::IpAddress("192.168.0.100"));
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetDomainNameCallback callback =
            resolver->createGetDomainNameCallback(
                bdlf::BindUtil::bind(&test::processGetDomainNameResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_OVERRIDE,
                                     &semaphore),
                &ta);

        // Define the options.

        ntca::GetDomainNameOptions options;

        // Get the domain name to which "192.168.0.100" is assigned.

        error = resolver->getDomainName(ntsa::IpAddress("192.168.0.100"),
                                        options,
                                        callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the domain name to which "192.168.0.100" is assigned.

        error = resolver->getDomainName(ntsa::IpAddress("192.168.0.100"),
                                        options,
                                        callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(3)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getPort' from overrides.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(false);
        resolverConfig.setPortDatabaseEnabled(false);
        resolverConfig.setPositiveCacheEnabled(false);
        resolverConfig.setNegativeCacheEnabled(false);
        resolverConfig.setSystemEnabled(false);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Set overrides.

        error = resolver->addPort("ntsp",
                                  6245,
                                  ntsa::Transport::e_TCP_IPV4_STREAM);
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetPortCallback callback = resolver->createGetPortCallback(
            bdlf::BindUtil::bind(&test::processGetPortResult,
                                 bdlf::PlaceHolders::_1,
                                 bdlf::PlaceHolders::_2,
                                 bdlf::PlaceHolders::_3,
                                 ntca::ResolverSource::e_OVERRIDE,
                                 &semaphore),
            &ta);

        // Define the options.

        ntca::GetPortOptions options;
        options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

        // Get the ports assigned to the "ntsp" service.

        error = resolver->getPort("ntsp", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the ports assigned to the "ntsp" service.

        error = resolver->getPort("ntsp", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(4)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getServiceName' from overrides.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(false);
        resolverConfig.setPortDatabaseEnabled(false);
        resolverConfig.setPositiveCacheEnabled(false);
        resolverConfig.setNegativeCacheEnabled(false);
        resolverConfig.setSystemEnabled(false);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Set overrides.

        error = resolver->addPort("ntsp",
                                  6245,
                                  ntsa::Transport::e_TCP_IPV4_STREAM);
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetServiceNameCallback callback =
            resolver->createGetServiceNameCallback(
                bdlf::BindUtil::bind(&test::processGetServiceNameResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_OVERRIDE,
                                     &semaphore),
                &ta);

        // Define the options.

        ntca::GetServiceNameOptions options;
        options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

        // Get the service name to which TCP port 6245 is assigned.

        error = resolver->getServiceName(6245, options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the service name to which TCP port 6245 is assigned.

        error = resolver->getServiceName(6245, options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(5)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getEndpoint' from overrides.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(false);
        resolverConfig.setPortDatabaseEnabled(false);
        resolverConfig.setPositiveCacheEnabled(false);
        resolverConfig.setNegativeCacheEnabled(false);
        resolverConfig.setSystemEnabled(false);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Set overrides.

        error = resolver->addIpAddress("test.example.net",
                                       ntsa::IpAddress("192.168.0.100"));
        NTCCFG_TEST_FALSE(error);

        error = resolver->addPort("ntsp",
                                  6245,
                                  ntsa::Transport::e_TCP_IPV4_STREAM);
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetEndpointCallback callback =
            resolver->createGetEndpointCallback(
                bdlf::BindUtil::bind(&test::processGetEndpointResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_OVERRIDE,
                                     &semaphore),
                &ta);

        // Define the options.

        ntca::GetEndpointOptions options;
        options.setIpAddressType(ntsa::IpAddressType::e_V4);

        // Get the endpoint assigned to "test.example.net:ntsp".

        error =
            resolver->getEndpoint("test.example.net:ntsp", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the endpoint assigned to "test.example.net:ntsp".

        error =
            resolver->getEndpoint("test.example.net:ntsp", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

//
// Test database.
//

NTCCFG_TEST_CASE(6)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getIpAddress' from database.
    // Plan:

    if (!ntscfg::Platform::hasHostDatabase()) {
        return;
    }

    if (!ntscfg::Platform::hasPortDatabase()) {
        return;
    }

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(true);
        resolverConfig.setPortDatabaseEnabled(true);
        resolverConfig.setPositiveCacheEnabled(false);
        resolverConfig.setNegativeCacheEnabled(false);
        resolverConfig.setSystemEnabled(false);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Set database.

        // clang-format off
        const char HOST_DATABASE[] = ""
        "192.168.0.100 test.example.net\n"
        "\n";

        const char PORT_DATABASE[] = ""
        "ntsp 6245/tcp\n"
        "ntsp 6245/udp\n"
        "\n";
        // clang-format on

        error = resolver->loadHostDatabaseText(HOST_DATABASE,
                                               sizeof HOST_DATABASE - 1);
        NTCCFG_TEST_FALSE(error);

        error = resolver->loadPortDatabaseText(PORT_DATABASE,
                                               sizeof PORT_DATABASE - 1);
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetIpAddressCallback callback =
            resolver->createGetIpAddressCallback(
                bdlf::BindUtil::bind(&test::processGetIpAddressResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_DATABASE,
                                     &semaphore),
                &ta);

        // Define the options.

        ntca::GetIpAddressOptions options;
        options.setIpAddressType(ntsa::IpAddressType::e_V4);

        // Get the IP addresses assigned to "test.example.net".

        error = resolver->getIpAddress("test.example.net", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the IP addresses assigned to "test.example.net".

        error = resolver->getIpAddress("test.example.net", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(7)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getDomainName' from database.
    // Plan:

    if (!ntscfg::Platform::hasHostDatabase()) {
        return;
    }

    if (!ntscfg::Platform::hasPortDatabase()) {
        return;
    }

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(true);
        resolverConfig.setPortDatabaseEnabled(true);
        resolverConfig.setPositiveCacheEnabled(false);
        resolverConfig.setNegativeCacheEnabled(false);
        resolverConfig.setSystemEnabled(false);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Set database.

        // clang-format off
        const char HOST_DATABASE[] = ""
        "192.168.0.100 test.example.net\n"
        "\n";

        const char PORT_DATABASE[] = ""
        "ntsp 6245/tcp\n"
        "ntsp 6245/udp\n"
        "\n";
        // clang-format on

        error = resolver->loadHostDatabaseText(HOST_DATABASE,
                                               sizeof HOST_DATABASE - 1);
        NTCCFG_TEST_FALSE(error);

        error = resolver->loadPortDatabaseText(PORT_DATABASE,
                                               sizeof PORT_DATABASE - 1);
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetDomainNameCallback callback =
            resolver->createGetDomainNameCallback(
                bdlf::BindUtil::bind(&test::processGetDomainNameResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_DATABASE,
                                     &semaphore),
                &ta);

        // Define the options.

        ntca::GetDomainNameOptions options;

        // Get the domain name to which "192.168.0.100" is assigned.

        error = resolver->getDomainName(ntsa::IpAddress("192.168.0.100"),
                                        options,
                                        callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the domain name to which "192.168.0.100" is assigned.

        error = resolver->getDomainName(ntsa::IpAddress("192.168.0.100"),
                                        options,
                                        callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(8)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getPort' from database.
    // Plan:

    if (!ntscfg::Platform::hasHostDatabase()) {
        return;
    }

    if (!ntscfg::Platform::hasPortDatabase()) {
        return;
    }

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(true);
        resolverConfig.setPortDatabaseEnabled(true);
        resolverConfig.setPositiveCacheEnabled(false);
        resolverConfig.setNegativeCacheEnabled(false);
        resolverConfig.setSystemEnabled(false);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Set database.

        // clang-format off
        const char HOST_DATABASE[] = ""
        "192.168.0.100 test.example.net\n"
        "\n";

        const char PORT_DATABASE[] = ""
        "ntsp 6245/tcp\n"
        "ntsp 6245/udp\n"
        "\n";
        // clang-format on

        error = resolver->loadHostDatabaseText(HOST_DATABASE,
                                               sizeof HOST_DATABASE - 1);
        NTCCFG_TEST_FALSE(error);

        error = resolver->loadPortDatabaseText(PORT_DATABASE,
                                               sizeof PORT_DATABASE - 1);
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetPortCallback callback = resolver->createGetPortCallback(
            bdlf::BindUtil::bind(&test::processGetPortResult,
                                 bdlf::PlaceHolders::_1,
                                 bdlf::PlaceHolders::_2,
                                 bdlf::PlaceHolders::_3,
                                 ntca::ResolverSource::e_DATABASE,
                                 &semaphore),
            &ta);

        // Define the options.

        ntca::GetPortOptions options;
        options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

        // Get the ports assigned to the "ntsp" service.

        error = resolver->getPort("ntsp", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the ports assigned to the "ntsp" service.

        error = resolver->getPort("ntsp", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(9)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getServiceName' from database.
    // Plan:

    if (!ntscfg::Platform::hasHostDatabase()) {
        return;
    }

    if (!ntscfg::Platform::hasPortDatabase()) {
        return;
    }

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(true);
        resolverConfig.setPortDatabaseEnabled(true);
        resolverConfig.setPositiveCacheEnabled(false);
        resolverConfig.setNegativeCacheEnabled(false);
        resolverConfig.setSystemEnabled(false);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Set database.

        // clang-format off
        const char HOST_DATABASE[] = ""
        "192.168.0.100 test.example.net\n"
        "\n";

        const char PORT_DATABASE[] = ""
        "ntsp 6245/tcp\n"
        "ntsp 6245/udp\n"
        "\n";
        // clang-format on

        error = resolver->loadHostDatabaseText(HOST_DATABASE,
                                               sizeof HOST_DATABASE - 1);
        NTCCFG_TEST_FALSE(error);

        error = resolver->loadPortDatabaseText(PORT_DATABASE,
                                               sizeof PORT_DATABASE - 1);
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetServiceNameCallback callback =
            resolver->createGetServiceNameCallback(
                bdlf::BindUtil::bind(&test::processGetServiceNameResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_DATABASE,
                                     &semaphore),
                &ta);

        // Define the options.

        ntca::GetServiceNameOptions options;
        options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

        // Get the service name to which TCP port 6245 is assigned.

        error = resolver->getServiceName(6245, options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the service name to which TCP port 6245 is assigned.

        error = resolver->getServiceName(6245, options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(10)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getEndpoint' from database.
    // Plan:

    if (!ntscfg::Platform::hasHostDatabase()) {
        return;
    }

    if (!ntscfg::Platform::hasPortDatabase()) {
        return;
    }

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(true);
        resolverConfig.setPortDatabaseEnabled(true);
        resolverConfig.setPositiveCacheEnabled(false);
        resolverConfig.setNegativeCacheEnabled(false);
        resolverConfig.setSystemEnabled(false);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Set database.

        // clang-format off
        const char HOST_DATABASE[] = ""
        "192.168.0.100 test.example.net\n"
        "\n";

        const char PORT_DATABASE[] = ""
        "ntsp 6245/tcp\n"
        "ntsp 6245/udp\n"
        "\n";
        // clang-format on

        error = resolver->loadHostDatabaseText(HOST_DATABASE,
                                               sizeof HOST_DATABASE - 1);
        NTCCFG_TEST_FALSE(error);

        error = resolver->loadPortDatabaseText(PORT_DATABASE,
                                               sizeof PORT_DATABASE - 1);
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetEndpointCallback callback =
            resolver->createGetEndpointCallback(
                bdlf::BindUtil::bind(&test::processGetEndpointResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_DATABASE,
                                     &semaphore),
                &ta);

        // Define the options.

        ntca::GetEndpointOptions options;
        options.setIpAddressType(ntsa::IpAddressType::e_V4);

        // Get the endpoint assigned to "test.example.net:ntsp".

        error =
            resolver->getEndpoint("test.example.net:ntsp", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the endpoint assigned to "test.example.net:ntsp".

        error =
            resolver->getEndpoint("test.example.net:ntsp", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

//
// Test cache.
//

NTCCFG_TEST_CASE(11)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getIpAddress' from cache.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(false);
        resolverConfig.setPortDatabaseEnabled(false);
        resolverConfig.setPositiveCacheEnabled(true);
        resolverConfig.setNegativeCacheEnabled(true);
        resolverConfig.setSystemEnabled(false);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Set the cache.

        error = resolver->cacheHost("test.example.net",
                                    ntsa::IpAddress("192.168.0.100"),
                                    ntsa::Endpoint("192.168.1.1"),
                                    3600,
                                    bdlt::CurrentTime::now());
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetIpAddressCallback callback =
            resolver->createGetIpAddressCallback(
                bdlf::BindUtil::bind(&test::processGetIpAddressResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_CACHE,
                                     &semaphore),
                &ta);

        // Define the options.

        ntca::GetIpAddressOptions options;
        options.setIpAddressType(ntsa::IpAddressType::e_V4);

        // Get the IP addresses assigned to "test.example.net".

        error = resolver->getIpAddress("test.example.net", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the IP addresses assigned to "test.example.net".

        error = resolver->getIpAddress("test.example.net", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(12)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getDomainName' from cache.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(false);
        resolverConfig.setPortDatabaseEnabled(false);
        resolverConfig.setPositiveCacheEnabled(true);
        resolverConfig.setNegativeCacheEnabled(true);
        resolverConfig.setSystemEnabled(false);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Set the cache.

        error = resolver->cacheHost("test.example.net",
                                    ntsa::IpAddress("192.168.0.100"),
                                    ntsa::Endpoint("192.168.1.1"),
                                    3600,
                                    bdlt::CurrentTime::now());
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetDomainNameCallback callback =
            resolver->createGetDomainNameCallback(
                bdlf::BindUtil::bind(&test::processGetDomainNameResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_CACHE,
                                     &semaphore),
                &ta);

        // Define the options.

        ntca::GetDomainNameOptions options;

        // Get the domain name to which "192.168.0.100" is assigned.

        error = resolver->getDomainName(ntsa::IpAddress("192.168.0.100"),
                                        options,
                                        callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the domain name to which "192.168.0.100" is assigned.

        error = resolver->getDomainName(ntsa::IpAddress("192.168.0.100"),
                                        options,
                                        callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(13)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

// Service names and ports are not currently cached.
#if 0
    // Concern: Test 'getPort' from cache.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(false);
        resolverConfig.setPortDatabaseEnabled(false);
        resolverConfig.setPositiveCacheEnabled(true);
        resolverConfig.setNegativeCacheEnabled(true);
        resolverConfig.setSystemEnabled(false);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Set the cache.

        error = resolver->cacheHost(
            "test.example.net",
            ntsa::IpAddress("192.168.0.100"),
            ntsa::Endpoint("192.168.1.1"),
            3600,
            bdlt::CurrentTime::now());
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetPortCallback callback = resolver->createGetPortCallback(
            bdlf::BindUtil::bind(&test::processGetPortResult,
                                 bdlf::PlaceHolders::_1,
                                 bdlf::PlaceHolders::_2,
                                 bdlf::PlaceHolders::_3,
                                 ntca::ResolverSource::e_CACHE,
                                 &semaphore),
            &ta);

        // Define the options.

        ntca::GetPortOptions options;
        options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

        // Get the ports assigned to the "ntsp" service.

        error = resolver->getPort("ntsp", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the ports assigned to the "ntsp" service.

        error = resolver->getPort("ntsp", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
#endif

#endif
}

NTCCFG_TEST_CASE(14)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

// Concern: Test 'getServiceName' from cache.
// Plan:

// Service names and ports are not currently cached.
#if 0
    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(false);
        resolverConfig.setPortDatabaseEnabled(false);
        resolverConfig.setPositiveCacheEnabled(true);
        resolverConfig.setNegativeCacheEnabled(true);
        resolverConfig.setSystemEnabled(false);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Set the cache.

        error = resolver->cacheHost(
            "test.example.net",
            ntsa::IpAddress("192.168.0.100"),
            ntsa::Endpoint("192.168.1.1"),
            3600,
            bdlt::CurrentTime::now());
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetServiceNameCallback callback =
            resolver->createGetServiceNameCallback(
                bdlf::BindUtil::bind(&test::processGetServiceNameResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_CACHE,
                                     &semaphore),
                &ta);

        // Define the options.

        ntca::GetServiceNameOptions options;
        options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

        // Get the service name to which TCP port 6245 is assigned.

        error = resolver->getServiceName(6245, options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the service name to which TCP port 6245 is assigned.

        error = resolver->getServiceName(6245, options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
#endif

#endif
}

NTCCFG_TEST_CASE(15)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getEndpoint' from cache.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(false);
        resolverConfig.setPortDatabaseEnabled(false);
        resolverConfig.setPositiveCacheEnabled(true);
        resolverConfig.setNegativeCacheEnabled(true);
        resolverConfig.setSystemEnabled(false);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Set the cache.

        error = resolver->cacheHost("test.example.net",
                                    ntsa::IpAddress("192.168.0.100"),
                                    ntsa::Endpoint("192.168.1.1"),
                                    3600,
                                    bdlt::CurrentTime::now());
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetEndpointCallback callback =
            resolver->createGetEndpointCallback(
                bdlf::BindUtil::bind(&test::processGetEndpointResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_CACHE,
                                     &semaphore),
                &ta);

        // Define the options.

        ntca::GetEndpointOptions options;
        options.setIpAddressType(ntsa::IpAddressType::e_V4);

        // Get the endpoint assigned to "test.example.net:6245".

        error =
            resolver->getEndpoint("test.example.net:6245", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the endpoint assigned to "test.example.net:6245".

        error =
            resolver->getEndpoint("test.example.net:6245", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

//
// Test system.
//

NTCCFG_TEST_CASE(16)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getIpAddress' from system.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(false);
        resolverConfig.setPortDatabaseEnabled(false);
        resolverConfig.setPositiveCacheEnabled(false);
        resolverConfig.setNegativeCacheEnabled(false);
        resolverConfig.setSystemEnabled(true);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetIpAddressCallback callback =
            resolver->createGetIpAddressCallback(
                bdlf::BindUtil::bind(&test::processGetIpAddressResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_SYSTEM,
                                     &semaphore),
                &ta);

        // Define the options.

        ntca::GetIpAddressOptions options;
        options.setIpAddressType(ntsa::IpAddressType::e_V4);

        // Get the IP addresses assigned to "google.com".

        error = resolver->getIpAddress("google.com", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the IP addresses assigned to "google.com".

        error = resolver->getIpAddress("google.com", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(17)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getDomainName' from system.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(false);
        resolverConfig.setPortDatabaseEnabled(false);
        resolverConfig.setPositiveCacheEnabled(false);
        resolverConfig.setNegativeCacheEnabled(false);
        resolverConfig.setSystemEnabled(true);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetDomainNameCallback callback =
            resolver->createGetDomainNameCallback(
                bdlf::BindUtil::bind(&test::processGetDomainNameResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_SYSTEM,
                                     &semaphore),
                &ta);

        // Define the options.

        ntca::GetDomainNameOptions options;

        // Get the domain name to which "8.8.8.8" is assigned.

        error = resolver->getDomainName(ntsa::IpAddress("8.8.8.8"),
                                        options,
                                        callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the domain name to which "8.8.8.8" is assigned.

        error = resolver->getDomainName(ntsa::IpAddress("8.8.8.8"),
                                        options,
                                        callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(18)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getPort' from system.
    // Plan:

    if (!ntscfg::Platform::hasPortDatabase()) {
        return;
    }

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(false);
        resolverConfig.setPortDatabaseEnabled(false);
        resolverConfig.setPositiveCacheEnabled(false);
        resolverConfig.setNegativeCacheEnabled(false);
        resolverConfig.setSystemEnabled(true);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetPortCallback callback = resolver->createGetPortCallback(
            bdlf::BindUtil::bind(&test::processGetPortResult,
                                 bdlf::PlaceHolders::_1,
                                 bdlf::PlaceHolders::_2,
                                 bdlf::PlaceHolders::_3,
                                 ntca::ResolverSource::e_SYSTEM,
                                 &semaphore),
            &ta);

        // Define the options.

        ntca::GetPortOptions options;
        options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

        // Get the ports assigned to the "echo" service.

        error = resolver->getPort("echo", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the ports assigned to the "echo" service.

        error = resolver->getPort("echo", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(19)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getServiceName' from system.
    // Plan:

    if (!ntscfg::Platform::hasPortDatabase()) {
        return;
    }

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(false);
        resolverConfig.setPortDatabaseEnabled(false);
        resolverConfig.setPositiveCacheEnabled(false);
        resolverConfig.setNegativeCacheEnabled(false);
        resolverConfig.setSystemEnabled(true);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetServiceNameCallback callback =
            resolver->createGetServiceNameCallback(
                bdlf::BindUtil::bind(&test::processGetServiceNameResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_SYSTEM,
                                     &semaphore),
                &ta);

        // Define the options.

        ntca::GetServiceNameOptions options;
        options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

        // Get the service name to which TCP port 7 is assigned.

        error = resolver->getServiceName(7, options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the service name to which TCP port 7 is assigned.

        error = resolver->getServiceName(7, options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(20)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getEndpoint' from system.
    // Plan:

    if (!ntscfg::Platform::hasHostDatabase()) {
        return;
    }

    if (!ntscfg::Platform::hasPortDatabase()) {
        return;
    }

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(false);
        resolverConfig.setPortDatabaseEnabled(false);
        resolverConfig.setPositiveCacheEnabled(false);
        resolverConfig.setNegativeCacheEnabled(false);
        resolverConfig.setSystemEnabled(true);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetEndpointCallback callback =
            resolver->createGetEndpointCallback(
                bdlf::BindUtil::bind(&test::processGetEndpointResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_SYSTEM,
                                     &semaphore),
                &ta);

        // Define the options.

        ntca::GetEndpointOptions options;
        options.setIpAddressType(ntsa::IpAddressType::e_V4);

        // Get the endpoint assigned to "dns.google.com:http".

        error =
            resolver->getEndpoint("dns.google.com:http", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the endpoint assigned to "dns.google.com:http".

        error =
            resolver->getEndpoint("dns.google.com:http", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_CASE(21)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getEndpoint' where the input is empty but both an
    // IP address fallback and a port fallback are specified in the options.
    //
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Define a resolver configuration with the DNS client disabled.

        ntca::ResolverConfig resolverConfig;
        resolverConfig.setClientEnabled(false);
        resolverConfig.setHostDatabaseEnabled(false);
        resolverConfig.setPortDatabaseEnabled(false);
        resolverConfig.setPositiveCacheEnabled(false);
        resolverConfig.setNegativeCacheEnabled(false);
        resolverConfig.setSystemEnabled(false);

        // Create a start a resolver.

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(&ta, resolverConfig, &ta);

        error = resolver->start();
        NTCCFG_TEST_FALSE(error);

        {
            // Create the callback.

            bslmt::Semaphore semaphore;

            ntci::GetEndpointCallback callback =
                resolver->createGetEndpointCallback(
                    bdlf::BindUtil::bind(&test::processGetEndpointResult,
                                         bdlf::PlaceHolders::_1,
                                         bdlf::PlaceHolders::_2,
                                         bdlf::PlaceHolders::_3,
                                         ntca::ResolverSource::e_UNKNOWN,
                                         &semaphore),
                    &ta);

            // Define the options.

            ntca::GetEndpointOptions options;

            // Get the endpoint assigned to the empty string, which should be
            // interpreted as the IP address fallback and port fallback,
            // neither of which are defined, which should result in an error.

            error = resolver->getEndpoint("", options, callback);
            NTCCFG_TEST_TRUE(error);
        }

        {
            // Create the callback.

            bslmt::Semaphore semaphore;

            ntci::GetEndpointCallback callback =
                resolver->createGetEndpointCallback(
                    bdlf::BindUtil::bind(&test::processGetEndpointResult,
                                         bdlf::PlaceHolders::_1,
                                         bdlf::PlaceHolders::_2,
                                         bdlf::PlaceHolders::_3,
                                         ntca::ResolverSource::e_UNKNOWN,
                                         &semaphore),
                    &ta);

            // Define the options.

            ntca::GetEndpointOptions options;
            options.setIpAddressFallback(ntsa::IpAddress("192.168.0.100"));

            // Get the endpoint assigned to the empty string, which should be
            // interpreted as the IP address fallback and port fallback,
            // one of which is not defined, which should result in an error.

            error = resolver->getEndpoint("", options, callback);
            NTCCFG_TEST_TRUE(error);
        }

        {
            // Create the callback.

            bslmt::Semaphore semaphore;

            ntci::GetEndpointCallback callback =
                resolver->createGetEndpointCallback(
                    bdlf::BindUtil::bind(&test::processGetEndpointResult,
                                         bdlf::PlaceHolders::_1,
                                         bdlf::PlaceHolders::_2,
                                         bdlf::PlaceHolders::_3,
                                         ntca::ResolverSource::e_UNKNOWN,
                                         &semaphore),
                    &ta);

            // Define the options.

            ntca::GetEndpointOptions options;
            options.setPortFallback(12345);

            // Get the endpoint assigned to the empty string, which should be
            // interpreted as the IP address fallback and port fallback,
            // one of which is not defined, which should result in an error.

            error = resolver->getEndpoint("", options, callback);
            NTCCFG_TEST_TRUE(error);
        }

        {
            // Create the callback.

            bslmt::Semaphore semaphore;

            ntci::GetEndpointCallback callback =
                resolver->createGetEndpointCallback(
                    bdlf::BindUtil::bind(&test::processGetEndpointResult,
                                         bdlf::PlaceHolders::_1,
                                         bdlf::PlaceHolders::_2,
                                         bdlf::PlaceHolders::_3,
                                         ntca::ResolverSource::e_UNKNOWN,
                                         &semaphore),
                    &ta);

            // Define the options.

            ntca::GetEndpointOptions options;
            options.setIpAddressFallback(ntsa::IpAddress("192.168.0.100"));
            options.setPortFallback(12345);

            // Get the endpoint assigned to the empty string, which should be
            // interpreted as the IP address fallback and port fallback.

            error = resolver->getEndpoint("", options, callback);
            NTCCFG_TEST_FALSE(error);

            semaphore.wait();
        }

        // Stop the resolver.

        resolver->shutdown();
        resolver->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_DRIVER
{
    // Override

    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
    NTCCFG_TEST_REGISTER(5);

    // Database

    NTCCFG_TEST_REGISTER(6);
    NTCCFG_TEST_REGISTER(7);
    NTCCFG_TEST_REGISTER(8);
    NTCCFG_TEST_REGISTER(9);
    NTCCFG_TEST_REGISTER(10);

    // Cache

    NTCCFG_TEST_REGISTER(11);
    NTCCFG_TEST_REGISTER(12);
    NTCCFG_TEST_REGISTER(13);
    NTCCFG_TEST_REGISTER(14);
    NTCCFG_TEST_REGISTER(15);

    // System

    NTCCFG_TEST_REGISTER(16);
    NTCCFG_TEST_REGISTER(17);
    NTCCFG_TEST_REGISTER(18);
    NTCCFG_TEST_REGISTER(19);
    NTCCFG_TEST_REGISTER(20);

    // Empty

    NTCCFG_TEST_REGISTER(21);
}
NTCCFG_TEST_DRIVER_END;
