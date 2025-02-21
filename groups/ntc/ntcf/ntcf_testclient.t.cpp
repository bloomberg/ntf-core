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

#ifndef INCLUDED_NTCF_TESTCLIENT_T_CPP
#define INCLUDED_NTCF_TESTCLIENT_T_CPP

#include <ntscfg_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcf_testclient_t_cpp, "$Id$ $CSID$")

#include <ntcf_testclient.h>

namespace BloombergLP {
namespace ntcf {

// Provide tests for 'ntcf::TestClient'.
class TestClientTest
{
  public:
    // TODO
    static void verify();
};

NTSCFG_TEST_FUNCTION(ntcf::TestClientTest::verify)
{
    ntsa::Error error;

    ntcf::TestServerConfig exchangeConfig;
    exchangeConfig.name = "exchange";
    exchangeConfig.numNetworkingThreads = 1;
    exchangeConfig.keepHalfOpen = false;

    ntcf::TestServer exchange(exchangeConfig, NTSCFG_TEST_ALLOCATOR);

    ntcf::TestClientConfig sellerConfig;
    sellerConfig.name = "seller";
    sellerConfig.numNetworkingThreads = 1;
    sellerConfig.keepHalfOpen = false;

    ntcf::TestClient seller(sellerConfig, 
                            exchange.tcpEndpoint(), 
                            exchange.udpEndpoint(), 
                            NTSCFG_TEST_ALLOCATOR);

    ntcf::TestClientConfig buyerConfig;
    buyerConfig.name = "buyer";
    buyerConfig.numNetworkingThreads = 1;
    buyerConfig.keepHalfOpen = false;

    ntcf::TestClient buyer(buyerConfig, 
                           exchange.tcpEndpoint(), 
                           exchange.udpEndpoint(), 
                           NTSCFG_TEST_ALLOCATOR);

    {
        ntcf::TestEchoResult result;
        ntcf::TestSignal     signal;
        ntcf::TestOptions    options;

        signal.id = 1;
        signal.reflect = 64;
        ntscfg::TestDataUtil::generateData(
            &signal.value, 
            32, 
            0, 
            ntscfg::TestDataUtil::k_DATASET_CLIENT_COMPRESSABLE);

        error = buyer.signal(&result, signal, options);
        NTSCFG_TEST_OK(error);
    }
    

    



    

    
}

}  // close namespace ntcf
}  // close namespace BloombergLP
#endif
