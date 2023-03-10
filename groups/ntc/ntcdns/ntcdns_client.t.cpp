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

#include <ntcdns_client.h>

#include <ntccfg_test.h>
#include <ntcdns_cache.h>
#include <ntcdns_database.h>
#include <ntcdns_utility.h>
#include <ntci_log.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_semaphore.h>
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

// MRM: This test implementation is disable because of the difficulty
// implementing the 'ntcdns::Client' dependencies in a test context.
#if 0

namespace test {

void processGetIpAddressResult(
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

}  // close namespace test

NTCCFG_TEST_CASE(1)
{
#if NTC_BUILD_FROM_CONTINUOUS_INTEGRATION == 0

    // Concern:
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        // Load the default client configuration from /etc/resolv.conf.

        ntcdns::ClientConfig clientConfig(&ta);
        error = ntcdns::Utility::loadClientConfig(&clientConfig);
        NTCCFG_TEST_FALSE(error);

        // Create the cache.

        bsl::shared_ptr<ntcdns::Cache> cache;
        cache.createInplace(&ta, &ta);

        // Create a start a client.

        bsl::shared_ptr<ntcdns::Client> client;
        client.createInplace(&ta, clientConfig, cache, &ta);

        error = client->start();
        NTCCFG_TEST_FALSE(error);

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
            &ta);

        error =
            client->getIpAddress(resolver, "google.com", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        error =
            client->getIpAddress(resolver, "google.com", options, callback);
        NTCCFG_TEST_FALSE(error);

        semaphore.wait();

        // Stop the client.

        client->shutdown();
        client->linger();
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);

#endif
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
}
NTCCFG_TEST_DRIVER_END;

#else

NTCCFG_TEST_CASE(1)
{
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
}
NTCCFG_TEST_DRIVER_END;

#endif
