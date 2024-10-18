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
BSLS_IDENT_RCSID(ntcdns_system_t_cpp, "$Id$ $CSID$")

#include <ntcdns_system.h>

#include <ntci_log.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcdns {

// Provide tests for 'ntcdns::System'.
class SystemTest
{
    static void processGetIpAddressResult(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const bsl::vector<ntsa::IpAddress>&    ipAddressList,
        const ntca::GetIpAddressEvent&         event,
        bslmt::Semaphore*                      semaphore);

    static void processGetDomainNameResult(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const bsl::string&                     domainName,
        const ntca::GetDomainNameEvent&        event,
        bslmt::Semaphore*                      semaphore);

    static void processGetPortResult(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const bsl::vector<ntsa::Port>&         portList,
        const ntca::GetPortEvent&              event,
        bslmt::Semaphore*                      semaphore);

    static void processGetServiceNameResult(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const bsl::string&                     serviceName,
        const ntca::GetServiceNameEvent&       event,
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
};

void SystemTest::processGetIpAddressResult(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const bsl::vector<ntsa::IpAddress>&    ipAddressList,
    const ntca::GetIpAddressEvent&         event,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get IP address event " << event
                          << NTCI_LOG_STREAM_END;

    if (event.type() == ntca::GetIpAddressEventType::e_COMPLETE) {
        if (ipAddressList.size() > 0) {
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

void SystemTest::processGetDomainNameResult(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const bsl::string&                     domainName,
    const ntca::GetDomainNameEvent&        event,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get domain name event " << event
                          << NTCI_LOG_STREAM_END;

    if (event.type() == ntca::GetDomainNameEventType::e_COMPLETE) {
        if (!domainName.empty()) {
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

void SystemTest::processGetPortResult(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const bsl::vector<ntsa::Port>&         portList,
    const ntca::GetPortEvent&              event,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get port event " << event
                          << NTCI_LOG_STREAM_END;

    if (event.type() == ntca::GetPortEventType::e_COMPLETE) {
        if (portList.size() > 0) {
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

void SystemTest::processGetServiceNameResult(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const bsl::string&                     serviceName,
    const ntca::GetServiceNameEvent&       event,
    bslmt::Semaphore*                      semaphore)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get service name event " << event
                          << NTCI_LOG_STREAM_END;

    if (event.type() == ntca::GetServiceNameEventType::e_COMPLETE) {
        if (!serviceName.empty()) {
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

NTSCFG_TEST_FUNCTION(ntcdns::SystemTest::verifyCase1)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getIpAddress'
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    // Create a start a multithreaded system resolver.

    bsl::shared_ptr<ntcdns::System> resolver;
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetIpAddressCallback callback(
        bdlf::BindUtil::bind(&SystemTest::processGetIpAddressResult,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2,
                             bdlf::PlaceHolders::_3,
                             &semaphore),
        NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetIpAddressOptions options;
    options.setIpAddressType(ntsa::IpAddressType::e_V4);

    // Get the IP addresses assigned to "google.com".

    error = resolver->getIpAddress(bsl::shared_ptr<ntci::Resolver>(),
                                   "google.com",
                                   bdlt::CurrentTime::now(),
                                   options,
                                   callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the IP addresses assigned to "google.com".

    error = resolver->getIpAddress(bsl::shared_ptr<ntci::Resolver>(),
                                   "google.com",
                                   bdlt::CurrentTime::now(),
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

NTSCFG_TEST_FUNCTION(ntcdns::SystemTest::verifyCase2)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getDomainName'
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    // Create a start a multithreaded system resolver.

    bsl::shared_ptr<ntcdns::System> resolver;
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetDomainNameCallback callback(
        bdlf::BindUtil::bind(&SystemTest::processGetDomainNameResult,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2,
                             bdlf::PlaceHolders::_3,
                             &semaphore),
        NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetDomainNameOptions options;

    // Get the domain name to which "8.8.8.8" is assigned.

    error = resolver->getDomainName(bsl::shared_ptr<ntci::Resolver>(),
                                    ntsa::IpAddress("8.8.8.8"),
                                    bdlt::CurrentTime::now(),
                                    options,
                                    callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the domain name to which "8.8.8.8" is assigned.

    error = resolver->getDomainName(bsl::shared_ptr<ntci::Resolver>(),
                                    ntsa::IpAddress("8.8.8.8"),
                                    bdlt::CurrentTime::now(),
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

NTSCFG_TEST_FUNCTION(ntcdns::SystemTest::verifyCase3)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getPort'
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    // Create a start a multithreaded system resolver.

    bsl::shared_ptr<ntcdns::System> resolver;
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetPortCallback callback(
        bdlf::BindUtil::bind(&SystemTest::processGetPortResult,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2,
                             bdlf::PlaceHolders::_3,
                             &semaphore),
        NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetPortOptions options;
    options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    // Get the ports assigned to the "echo" service.

    error = resolver->getPort(bsl::shared_ptr<ntci::Resolver>(),
                              "echo",
                              bdlt::CurrentTime::now(),
                              options,
                              callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the ports assigned to the "echo" service.

    error = resolver->getPort(bsl::shared_ptr<ntci::Resolver>(),
                              "echo",
                              bdlt::CurrentTime::now(),
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

NTSCFG_TEST_FUNCTION(ntcdns::SystemTest::verifyCase4)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern: Test 'getServiceName'
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    // Create a start a multithreaded system resolver.

    bsl::shared_ptr<ntcdns::System> resolver;
    resolver.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    error = resolver->start();
    NTSCFG_TEST_FALSE(error);

    // Create the callback.

    bslmt::Semaphore semaphore;

    ntci::GetServiceNameCallback callback(
        bdlf::BindUtil::bind(&SystemTest::processGetServiceNameResult,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2,
                             bdlf::PlaceHolders::_3,
                             &semaphore),
        NTSCFG_TEST_ALLOCATOR);

    // Define the options.

    ntca::GetServiceNameOptions options;
    options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    // Get the service name to which TCP port 7 is assigned.

    error = resolver->getServiceName(bsl::shared_ptr<ntci::Resolver>(),
                                     7,
                                     bdlt::CurrentTime::now(),
                                     options,
                                     callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Get the service name to which TCP port 7 is assigned.

    error = resolver->getServiceName(bsl::shared_ptr<ntci::Resolver>(),
                                     7,
                                     bdlt::CurrentTime::now(),
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

}  // close namespace ntcdns
}  // close namespace BloombergLP
