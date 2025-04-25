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
BSLS_IDENT_RCSID(ntcdns_resolver_t_cpp, "$Id$ $CSID$")

#include <ntcdns_resolver.h>

#include <ntci_log.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcdns {

// Provide tests for 'ntcdns::Resolver'.
class ResolverTest
{
    static void processGetIpAddressResult(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const bsl::vector<ntsa::IpAddress>&    ipAddressList,
        const ntca::GetIpAddressEvent&         event,
        ntca::ResolverSource::Value            source,
        bslmt::Semaphore*                      semaphore);

    static void processGetDomainNameResult(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const bsl::string&                     domainName,
        const ntca::GetDomainNameEvent&        event,
        ntca::ResolverSource::Value            source,
        bslmt::Semaphore*                      semaphore);

    static void processGetPortResult(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const bsl::vector<ntsa::Port>&         portList,
        const ntca::GetPortEvent&              event,
        ntca::ResolverSource::Value            source,
        bslmt::Semaphore*                      semaphore);

    static void processGetServiceNameResult(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const bsl::string&                     serviceName,
        const ntca::GetServiceNameEvent&       event,
        ntca::ResolverSource::Value            source,
        bslmt::Semaphore*                      semaphore);

    static void processGetEndpointResult(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const ntsa::Endpoint&                  endpoint,
        const ntca::GetEndpointEvent&          event,
        ntca::ResolverSource::Value            source,
        bslmt::Semaphore*                      semaphore);

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

    // TODO
    static void verifyCase7();

    // TODO
    static void verifyCase8();

    // TODO
    static void verifyCase9();

    // TODO
    static void verifyCase10();

    // TODO
    static void verifyCase11();

    // TODO
    static void verifyCase12();

    // TODO
    static void verifyCase13();

    // TODO
    static void verifyCase14();

    // TODO
    static void verifyCase15();

    // TODO
    static void verifyCase16();

    // TODO
    static void verifyCase17();

    // TODO
    static void verifyCase18();

    // TODO
    static void verifyCase19();

    // TODO
    static void verifyCase20();

    // TODO
    static void verifyCase21();
};

void ResolverTest::processGetIpAddressResult(
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
            NTSCFG_TEST_EQ(event.context().source(), source);
            for (bsl::size_t i = 0; i < ipAddressList.size(); ++i) {
                const ntsa::IpAddress& ipAddress = ipAddressList[i];
                NTCI_LOG_STREAM_DEBUG << "The domain name '"
                                     << event.context().domainName()
                                     << "' has resolved to " << ipAddress
                                     << NTCI_LOG_STREAM_END;
            }
        }
        else {
            NTCI_LOG_STREAM_DEBUG
                << "The domain name '" << event.context().domainName()
                << "' has no IP addresses assigned" << NTCI_LOG_STREAM_END;
        }
    }

    semaphore->post();
}

void ResolverTest::processGetDomainNameResult(
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
            NTSCFG_TEST_EQ(event.context().source(), source);
            NTCI_LOG_STREAM_DEBUG
                << "The IP address " << event.context().ipAddress()
                << " has resolved to the domain name '" << domainName << "'"
                << NTCI_LOG_STREAM_END;
        }
        else {
            NTCI_LOG_STREAM_DEBUG << "The IP address '"
                                 << event.context().ipAddress()
                                 << "' is not assigned to any domain name"
                                 << NTCI_LOG_STREAM_END;
        }
    }

    semaphore->post();
}

void ResolverTest::processGetPortResult(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
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
            NTSCFG_TEST_EQ(event.context().source(), source);
            for (bsl::size_t i = 0; i < portList.size(); ++i) {
                ntsa::Port port = portList[i];
                NTCI_LOG_STREAM_DEBUG << "The service name '"
                                     << event.context().serviceName()
                                     << "' has resolved to port " << port
                                     << NTCI_LOG_STREAM_END;
            }
        }
        else {
            NTCI_LOG_STREAM_DEBUG
                << "The service name '" << event.context().serviceName()
                << "' has no ports assigned" << NTCI_LOG_STREAM_END;
        }
    }

    semaphore->post();
}

void ResolverTest::processGetServiceNameResult(
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
            NTSCFG_TEST_EQ(event.context().source(), source);
            NTCI_LOG_STREAM_DEBUG << "The port " << event.context().port()
                                 << " has resolved to the service name '"
                                 << serviceName << "'" << NTCI_LOG_STREAM_END;
        }
        else {
            NTCI_LOG_STREAM_DEBUG << "The port '" << event.context().port()
                                 << "' is not assigned to any service name"
                                 << NTCI_LOG_STREAM_END;
        }
    }

    semaphore->post();
}

void ResolverTest::processGetEndpointResult(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const ntsa::Endpoint&                  endpoint,
    const ntca::GetEndpointEvent&          event,
    ntca::ResolverSource::Value            source,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get endpoint event " << event
                          << NTCI_LOG_STREAM_END;

    if (event.type() == ntca::GetEndpointEventType::e_COMPLETE) {
        NTSCFG_TEST_EQ(event.context().source(), source);
        NTCI_LOG_STREAM_DEBUG
            << "The authority '" << event.context().authority()
            << "' has resolved to " << endpoint << NTCI_LOG_STREAM_END;
    }

    semaphore->post();
}

//
// Test overrides.
//

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase1)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getIpAddress' from overrides.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    // Set overrides.

    error = resolver->addIpAddress("test.example.net",
                                   ntsa::IpAddress("192.168.0.100"));
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetIpAddressCallback callback = resolver->createGetIpAddressCallback(
        bdlf::BindUtil::bind(&ResolverTest::processGetIpAddressResult,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2,
                             bdlf::PlaceHolders::_3,
                             ntca::ResolverSource::e_OVERRIDE,
                             &semaphore),
        NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetIpAddressOptions options;
    options.setIpAddressType(ntsa::IpAddressType::e_V4);

    // Get the IP addresses assigned to "test.example.net".

    error = resolver->getIpAddress("test.example.net", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the IP addresses assigned to "test.example.net".

    error = resolver->getIpAddress("test.example.net", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase2)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getDomainName' from overrides.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    // Set overrides.

    error = resolver->addIpAddress("test.example.net",
                                   ntsa::IpAddress("192.168.0.100"));
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetDomainNameCallback callback =
        resolver->createGetDomainNameCallback(
            bdlf::BindUtil::bind(&ResolverTest::processGetDomainNameResult,
                                 bdlf::PlaceHolders::_1,
                                 bdlf::PlaceHolders::_2,
                                 bdlf::PlaceHolders::_3,
                                 ntca::ResolverSource::e_OVERRIDE,
                                 &semaphore),
            NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetDomainNameOptions options;

    // Get the domain name to which "192.168.0.100" is assigned.

    error = resolver->getDomainName(ntsa::IpAddress("192.168.0.100"),
                                    options,
                                    callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the domain name to which "192.168.0.100" is assigned.

    error = resolver->getDomainName(ntsa::IpAddress("192.168.0.100"),
                                    options,
                                    callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase3)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getPort' from overrides.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    // Set overrides.

    error =
        resolver->addPort("ntsp", 6245, ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetPortCallback callback = resolver->createGetPortCallback(
        bdlf::BindUtil::bind(&ResolverTest::processGetPortResult,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2,
                             bdlf::PlaceHolders::_3,
                             ntca::ResolverSource::e_OVERRIDE,
                             &semaphore),
        NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetPortOptions options;
    options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    // Get the ports assigned to the "ntsp" service.

    error = resolver->getPort("ntsp", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the ports assigned to the "ntsp" service.

    error = resolver->getPort("ntsp", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase4)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getServiceName' from overrides.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    // Set overrides.

    error =
        resolver->addPort("ntsp", 6245, ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetServiceNameCallback callback =
        resolver->createGetServiceNameCallback(
            bdlf::BindUtil::bind(&ResolverTest::processGetServiceNameResult,
                                 bdlf::PlaceHolders::_1,
                                 bdlf::PlaceHolders::_2,
                                 bdlf::PlaceHolders::_3,
                                 ntca::ResolverSource::e_OVERRIDE,
                                 &semaphore),
            NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetServiceNameOptions options;
    options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    // Get the service name to which TCP port 6245 is assigned.

    error = resolver->getServiceName(6245, options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the service name to which TCP port 6245 is assigned.

    error = resolver->getServiceName(6245, options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase5)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getEndpoint' from overrides.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    // Set overrides.

    error = resolver->addIpAddress("test.example.net",
                                   ntsa::IpAddress("192.168.0.100"));
    NTSCFG_TEST_FALSE(error);

    error =
        resolver->addPort("ntsp", 6245, ntsa::Transport::e_TCP_IPV4_STREAM);
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetEndpointCallback callback = resolver->createGetEndpointCallback(
        bdlf::BindUtil::bind(&ResolverTest::processGetEndpointResult,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2,
                             bdlf::PlaceHolders::_3,
                             ntca::ResolverSource::e_OVERRIDE,
                             &semaphore),
        NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetEndpointOptions options;
    options.setIpAddressType(ntsa::IpAddressType::e_V4);

    // Get the endpoint assigned to "test.example.net:ntsp".

    error = resolver->getEndpoint("test.example.net:ntsp", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the endpoint assigned to "test.example.net:ntsp".

    error = resolver->getEndpoint("test.example.net:ntsp", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

//
// Test database.
//

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase6)
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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

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
    NTSCFG_TEST_FALSE(error);

    error = resolver->loadPortDatabaseText(PORT_DATABASE,
                                           sizeof PORT_DATABASE - 1);
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetIpAddressCallback callback = resolver->createGetIpAddressCallback(
        bdlf::BindUtil::bind(&ResolverTest::processGetIpAddressResult,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2,
                             bdlf::PlaceHolders::_3,
                             ntca::ResolverSource::e_DATABASE,
                             &semaphore),
        NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetIpAddressOptions options;
    options.setIpAddressType(ntsa::IpAddressType::e_V4);

    // Get the IP addresses assigned to "test.example.net".

    error = resolver->getIpAddress("test.example.net", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the IP addresses assigned to "test.example.net".

    error = resolver->getIpAddress("test.example.net", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase7)
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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

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
    NTSCFG_TEST_FALSE(error);

    error = resolver->loadPortDatabaseText(PORT_DATABASE,
                                           sizeof PORT_DATABASE - 1);
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetDomainNameCallback callback =
        resolver->createGetDomainNameCallback(
            bdlf::BindUtil::bind(&ResolverTest::processGetDomainNameResult,
                                 bdlf::PlaceHolders::_1,
                                 bdlf::PlaceHolders::_2,
                                 bdlf::PlaceHolders::_3,
                                 ntca::ResolverSource::e_DATABASE,
                                 &semaphore),
            NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetDomainNameOptions options;

    // Get the domain name to which "192.168.0.100" is assigned.

    error = resolver->getDomainName(ntsa::IpAddress("192.168.0.100"),
                                    options,
                                    callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the domain name to which "192.168.0.100" is assigned.

    error = resolver->getDomainName(ntsa::IpAddress("192.168.0.100"),
                                    options,
                                    callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase8)
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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

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
    NTSCFG_TEST_FALSE(error);

    error = resolver->loadPortDatabaseText(PORT_DATABASE,
                                           sizeof PORT_DATABASE - 1);
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetPortCallback callback = resolver->createGetPortCallback(
        bdlf::BindUtil::bind(&ResolverTest::processGetPortResult,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2,
                             bdlf::PlaceHolders::_3,
                             ntca::ResolverSource::e_DATABASE,
                             &semaphore),
        NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetPortOptions options;
    options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    // Get the ports assigned to the "ntsp" service.

    error = resolver->getPort("ntsp", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the ports assigned to the "ntsp" service.

    error = resolver->getPort("ntsp", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase9)
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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

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
    NTSCFG_TEST_FALSE(error);

    error = resolver->loadPortDatabaseText(PORT_DATABASE,
                                           sizeof PORT_DATABASE - 1);
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetServiceNameCallback callback =
        resolver->createGetServiceNameCallback(
            bdlf::BindUtil::bind(&ResolverTest::processGetServiceNameResult,
                                 bdlf::PlaceHolders::_1,
                                 bdlf::PlaceHolders::_2,
                                 bdlf::PlaceHolders::_3,
                                 ntca::ResolverSource::e_DATABASE,
                                 &semaphore),
            NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetServiceNameOptions options;
    options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    // Get the service name to which TCP port 6245 is assigned.

    error = resolver->getServiceName(6245, options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the service name to which TCP port 6245 is assigned.

    error = resolver->getServiceName(6245, options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase10)
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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

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
    NTSCFG_TEST_FALSE(error);

    error = resolver->loadPortDatabaseText(PORT_DATABASE,
                                           sizeof PORT_DATABASE - 1);
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetEndpointCallback callback = resolver->createGetEndpointCallback(
        bdlf::BindUtil::bind(&ResolverTest::processGetEndpointResult,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2,
                             bdlf::PlaceHolders::_3,
                             ntca::ResolverSource::e_DATABASE,
                             &semaphore),
        NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetEndpointOptions options;
    options.setIpAddressType(ntsa::IpAddressType::e_V4);

    // Get the endpoint assigned to "test.example.net:ntsp".

    error = resolver->getEndpoint("test.example.net:ntsp", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the endpoint assigned to "test.example.net:ntsp".

    error = resolver->getEndpoint("test.example.net:ntsp", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

//
// Test cache.
//

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase11)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getIpAddress' from cache.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    // Set the cache.

    error = resolver->cacheHost("test.example.net",
                                ntsa::IpAddress("192.168.0.100"),
                                ntsa::Endpoint("192.168.1.1"),
                                3600,
                                bdlt::CurrentTime::now());
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetIpAddressCallback callback = resolver->createGetIpAddressCallback(
        bdlf::BindUtil::bind(&ResolverTest::processGetIpAddressResult,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2,
                             bdlf::PlaceHolders::_3,
                             ntca::ResolverSource::e_CACHE,
                             &semaphore),
        NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetIpAddressOptions options;
    options.setIpAddressType(ntsa::IpAddressType::e_V4);

    // Get the IP addresses assigned to "test.example.net".

    error = resolver->getIpAddress("test.example.net", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the IP addresses assigned to "test.example.net".

    error = resolver->getIpAddress("test.example.net", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase12)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getDomainName' from cache.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    // Set the cache.

    error = resolver->cacheHost("test.example.net",
                                ntsa::IpAddress("192.168.0.100"),
                                ntsa::Endpoint("192.168.1.1"),
                                3600,
                                bdlt::CurrentTime::now());
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetDomainNameCallback callback =
        resolver->createGetDomainNameCallback(
            bdlf::BindUtil::bind(&ResolverTest::processGetDomainNameResult,
                                 bdlf::PlaceHolders::_1,
                                 bdlf::PlaceHolders::_2,
                                 bdlf::PlaceHolders::_3,
                                 ntca::ResolverSource::e_CACHE,
                                 &semaphore),
            NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetDomainNameOptions options;

    // Get the domain name to which "192.168.0.100" is assigned.

    error = resolver->getDomainName(ntsa::IpAddress("192.168.0.100"),
                                    options,
                                    callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the domain name to which "192.168.0.100" is assigned.

    error = resolver->getDomainName(ntsa::IpAddress("192.168.0.100"),
                                    options,
                                    callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase13)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

// Service names and ports are not currently cached.
#if 0
    // Concern: Test 'getPort' from cache.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

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
        resolver.createInplace(NTSCFG_TEST_ALLOCATOR, resolverConfig, NTSCFG_TEST_ALLOCATOR);

        error = resolver->start();
        NTSCFG_TEST_FALSE(error);

        // Set the cache.

        error = resolver->cacheHost(
            "test.example.net",
            ntsa::IpAddress("192.168.0.100"),
            ntsa::Endpoint("192.168.1.1"),
            3600,
            bdlt::CurrentTime::now());
        NTSCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetPortCallback callback = resolver->createGetPortCallback(
            bdlf::BindUtil::bind(&ResolverTest::processGetPortResult,
                                 bdlf::PlaceHolders::_1,
                                 bdlf::PlaceHolders::_2,
                                 bdlf::PlaceHolders::_3,
                                 ntca::ResolverSource::e_CACHE,
                                 &semaphore),
            NTSCFG_TEST_ALLOCATOR);

        // Define the options.

        ntca::GetPortOptions options;
        options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

        // Get the ports assigned to the "ntsp" service.

        error = resolver->getPort("ntsp", options, callback);
        NTSCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the ports assigned to the "ntsp" service.

        error = resolver->getPort("ntsp", options, callback);
        NTSCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();

#endif

#endif
}

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase14)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

// Concern: Test 'getServiceName' from cache.
// Plan:

// Service names and ports are not currently cached.
#if 0
    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

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
        resolver.createInplace(NTSCFG_TEST_ALLOCATOR, resolverConfig, NTSCFG_TEST_ALLOCATOR);

        error = resolver->start();
        NTSCFG_TEST_FALSE(error);

        // Set the cache.

        error = resolver->cacheHost(
            "test.example.net",
            ntsa::IpAddress("192.168.0.100"),
            ntsa::Endpoint("192.168.1.1"),
            3600,
            bdlt::CurrentTime::now());
        NTSCFG_TEST_FALSE(error);

        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetServiceNameCallback callback =
            resolver->createGetServiceNameCallback(
                bdlf::BindUtil::bind(&ResolverTest::processGetServiceNameResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_CACHE,
                                     &semaphore),
                NTSCFG_TEST_ALLOCATOR);

        // Define the options.

        ntca::GetServiceNameOptions options;
        options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

        // Get the service name to which TCP port 6245 is assigned.

        error = resolver->getServiceName(6245, options, callback);
        NTSCFG_TEST_FALSE(error);

        semaphore.wait();

        // Get the service name to which TCP port 6245 is assigned.

        error = resolver->getServiceName(6245, options, callback);
        NTSCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the resolver.

        callback.reset();

        resolver->shutdown();
        resolver->linger();
#endif

#endif
}

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase15)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getEndpoint' from cache.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    // Set the cache.

    error = resolver->cacheHost("test.example.net",
                                ntsa::IpAddress("192.168.0.100"),
                                ntsa::Endpoint("192.168.1.1"),
                                3600,
                                bdlt::CurrentTime::now());
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetEndpointCallback callback = resolver->createGetEndpointCallback(
        bdlf::BindUtil::bind(&ResolverTest::processGetEndpointResult,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2,
                             bdlf::PlaceHolders::_3,
                             ntca::ResolverSource::e_CACHE,
                             &semaphore),
        NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetEndpointOptions options;
    options.setIpAddressType(ntsa::IpAddressType::e_V4);

    // Get the endpoint assigned to "test.example.net:6245".

    error = resolver->getEndpoint("test.example.net:6245", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the endpoint assigned to "test.example.net:6245".

    error = resolver->getEndpoint("test.example.net:6245", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

//
// Test system.
//

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase16)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getIpAddress' from system.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetIpAddressCallback callback = resolver->createGetIpAddressCallback(
        bdlf::BindUtil::bind(&ResolverTest::processGetIpAddressResult,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2,
                             bdlf::PlaceHolders::_3,
                             ntca::ResolverSource::e_SYSTEM,
                             &semaphore),
        NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetIpAddressOptions options;
    options.setIpAddressType(ntsa::IpAddressType::e_V4);

    // Get the IP addresses assigned to "google.com".

    error = resolver->getIpAddress("google.com", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the IP addresses assigned to "google.com".

    error = resolver->getIpAddress("google.com", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase17)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getDomainName' from system.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetDomainNameCallback callback =
        resolver->createGetDomainNameCallback(
            bdlf::BindUtil::bind(&ResolverTest::processGetDomainNameResult,
                                 bdlf::PlaceHolders::_1,
                                 bdlf::PlaceHolders::_2,
                                 bdlf::PlaceHolders::_3,
                                 ntca::ResolverSource::e_SYSTEM,
                                 &semaphore),
            NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetDomainNameOptions options;

    // Get the domain name to which "8.8.8.8" is assigned.

    error =
        resolver->getDomainName(ntsa::IpAddress("8.8.8.8"), options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the domain name to which "8.8.8.8" is assigned.

    error =
        resolver->getDomainName(ntsa::IpAddress("8.8.8.8"), options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase18)
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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetPortCallback callback = resolver->createGetPortCallback(
        bdlf::BindUtil::bind(&ResolverTest::processGetPortResult,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2,
                             bdlf::PlaceHolders::_3,
                             ntca::ResolverSource::e_SYSTEM,
                             &semaphore),
        NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetPortOptions options;
    options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    // Get the ports assigned to the "echo" service.

    error = resolver->getPort("echo", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the ports assigned to the "echo" service.

    error = resolver->getPort("echo", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase19)
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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetServiceNameCallback callback =
        resolver->createGetServiceNameCallback(
            bdlf::BindUtil::bind(&ResolverTest::processGetServiceNameResult,
                                 bdlf::PlaceHolders::_1,
                                 bdlf::PlaceHolders::_2,
                                 bdlf::PlaceHolders::_3,
                                 ntca::ResolverSource::e_SYSTEM,
                                 &semaphore),
            NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetServiceNameOptions options;
    options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    // Get the service name to which TCP port 7 is assigned.

    error = resolver->getServiceName(7, options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the service name to which TCP port 7 is assigned.

    error = resolver->getServiceName(7, options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase20)
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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetEndpointCallback callback = resolver->createGetEndpointCallback(
        bdlf::BindUtil::bind(&ResolverTest::processGetEndpointResult,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2,
                             bdlf::PlaceHolders::_3,
                             ntca::ResolverSource::e_SYSTEM,
                             &semaphore),
        NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetEndpointOptions options;
    options.setIpAddressType(ntsa::IpAddressType::e_V4);

    // Get the endpoint assigned to "dns.google.com:http".

    error = resolver->getEndpoint("dns.google.com:http", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the endpoint assigned to "dns.google.com:http".

    error = resolver->getEndpoint("dns.google.com:http", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the resolver.

    callback.reset();

    resolver->shutdown();
    resolver->linger();

#endif
}

NTSCFG_TEST_FUNCTION(ntcdns::ResolverTest::verifyCase21)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getEndpoint' where the input is empty but both an
    // IP address fallback and a port fallback are specified in the options.
    //
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

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
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR,
                           resolverConfig,
                           NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    {
        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetEndpointCallback callback =
            resolver->createGetEndpointCallback(
                bdlf::BindUtil::bind(&ResolverTest::processGetEndpointResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_UNKNOWN,
                                     &semaphore),
                NTSCFG_TEST_ALLOCATOR);

        // Define the options.

        ntca::GetEndpointOptions options;

        // Get the endpoint assigned to the empty string, which should be
        // interpreted as the IP address fallback and port fallback,
        // neither of which are defined, which should result in an error.

        error = resolver->getEndpoint("", options, callback);
        NTSCFG_TEST_TRUE(error);
    }

    {
        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetEndpointCallback callback =
            resolver->createGetEndpointCallback(
                bdlf::BindUtil::bind(&ResolverTest::processGetEndpointResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_UNKNOWN,
                                     &semaphore),
                NTSCFG_TEST_ALLOCATOR);

        // Define the options.

        ntca::GetEndpointOptions options;
        options.setIpAddressFallback(ntsa::IpAddress("192.168.0.100"));

        // Get the endpoint assigned to the empty string, which should be
        // interpreted as the IP address fallback and port fallback,
        // one of which is not defined, which should result in an error.

        error = resolver->getEndpoint("", options, callback);
        NTSCFG_TEST_TRUE(error);
    }

    {
        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetEndpointCallback callback =
            resolver->createGetEndpointCallback(
                bdlf::BindUtil::bind(&ResolverTest::processGetEndpointResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_UNKNOWN,
                                     &semaphore),
                NTSCFG_TEST_ALLOCATOR);

        // Define the options.

        ntca::GetEndpointOptions options;
        options.setPortFallback(12345);

        // Get the endpoint assigned to the empty string, which should be
        // interpreted as the IP address fallback and port fallback,
        // one of which is not defined, which should result in an error.

        error = resolver->getEndpoint("", options, callback);
        NTSCFG_TEST_TRUE(error);
    }

    {
        // Create the callback.

        bslmt::Semaphore semaphore;

        ntci::GetEndpointCallback callback =
            resolver->createGetEndpointCallback(
                bdlf::BindUtil::bind(&ResolverTest::processGetEndpointResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     ntca::ResolverSource::e_UNKNOWN,
                                     &semaphore),
                NTSCFG_TEST_ALLOCATOR);

        // Define the options.

        ntca::GetEndpointOptions options;
        options.setIpAddressFallback(ntsa::IpAddress("192.168.0.100"));
        options.setPortFallback(12345);

        // Get the endpoint assigned to the empty string, which should be
        // interpreted as the IP address fallback and port fallback.

        error = resolver->getEndpoint("", options, callback);
        NTSCFG_TEST_FALSE(error);

        semaphore.wait();
    }

    // Stop the resolver.

    resolver->shutdown();
    resolver->linger();

#endif
}

}  // close namespace ntcdns
}  // close namespace BloombergLP
