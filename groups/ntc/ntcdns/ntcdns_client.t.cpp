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
BSLS_IDENT_RCSID(ntcdns_client_t_cpp, "$Id$ $CSID$")

#include <ntcdns_client.h>

#include <ntcdns_cache.h>
#include <ntcdns_database.h>
#include <ntcdns_utility.h>
#include <ntci_log.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcdns {

// Provide tests for 'ntcdns::Client'.
class ClientTest
{
    static void processGetIpAddressResult(
                         const bsl::shared_ptr<ntci::Resolver>&  resolver,
                         const bsl::vector<ntsa::IpAddress>&     ipAddressList,
                         const ntca::GetIpAddressEvent&          event,
                         bslmt::Semaphore                       *semaphore);
  public:
    // TODO
    static void verify();
};

void ClientTest::processGetIpAddressResult(
                         const bsl::shared_ptr<ntci::Resolver>&  resolver,
                         const bsl::vector<ntsa::IpAddress>&     ipAddressList,
                         const ntca::GetIpAddressEvent&          event,
                         bslmt::Semaphore                       *semaphore)
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

NTSCFG_TEST_FUNCTION(ntcdns::ClientTest::verify)
{
// MRM: This test implementation is disable because of the difficulty
// implementing the 'ntcdns::Client' dependencies in a test context.
#if 0

    // Concern:
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    // Load the default client configuration from /etc/resolv.conf.

    ntcdns::ClientConfig clientConfig(NTSCFG_TEST_ALLOCATOR);
    error = ntcdns::Utility::loadClientConfig(&clientConfig);
    NTSCFG_TEST_FALSE(error);

    // Create the cache.

    bsl::shared_ptr<ntcdns::Cache> cache;
    cache.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    // Create a start a client.

    bsl::shared_ptr<ntcdns::Client> client;
    client.createInplace(
        NTSCFG_TEST_ALLOCATOR, clientConfig, cache, NTSCFG_TEST_ALLOCATOR);

    error = client->start();
    NTSCFG_TEST_FALSE(error);

    bsl::shared_ptr<ntci::Resolver> resolver;

    ntca::GetIpAddressOptions options;
    options.setIpAddressType(ntsa::IpAddressType::e_V4);

    bslmt::Semaphore semaphore;

    ntci::GetIpAddressCallback callback(
        bdlf::BindUtil::bind(&test::processGetIpAddressResult,
                                bdlf::PlaceHolders::_1,
                                bdlf::PlaceHolders::_2,
                                bdlf::PlaceHolders::_3,
                                &semaphore),
        NTSCFG_TEST_ALLOCATOR);

    error =
        client->getIpAddress(resolver, "google.com", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    error =
        client->getIpAddress(resolver, "google.com", options, callback);
    NTSCFG_TEST_FALSE(error);

    semaphore.wait();

    // Stop the client.

    client->shutdown();
    client->linger();

#endif
}

}  // close namespace ntcdns
}  // close namespace BloombergLP
