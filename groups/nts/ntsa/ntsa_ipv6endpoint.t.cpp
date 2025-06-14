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
BSLS_IDENT_RCSID(ntsa_ipv6endpoint_t_cpp, "$Id$ $CSID$")

#include <ntsa_ipv6endpoint.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Ipv6Endpoint'.
class Ipv6EndpointTest
{
  public:
    // TODO
    static void verify();
};

NTSCFG_TEST_FUNCTION(ntsa::Ipv6EndpointTest::verify)
{
    {
        ntsa::Ipv6Endpoint ipv6Endpoint;
        
        bool valid = ipv6Endpoint.parse("[::1]:12345");
        NTSCFG_TEST_TRUE(valid);

        NTSCFG_TEST_EQ(ipv6Endpoint.host(), ntsa::Ipv6Address::loopback());
        NTSCFG_TEST_EQ(ipv6Endpoint.port(), 12345);
    }

    {
        ntsa::Ipv6Endpoint ipv6Endpoint;
    
        bool valid = ipv6Endpoint.parse("::1:12345");
        NTSCFG_TEST_FALSE(valid);
    }

    {
        ntsa::Ipv6Endpoint ipv6Endpoint;

        bool valid = ipv6Endpoint.parse("[::1:12345");
        NTSCFG_TEST_FALSE(valid);
    }

    {
        ntsa::Ipv6Endpoint ipv6Endpoint;

        bool valid = ipv6Endpoint.parse("::1]:12345");
        NTSCFG_TEST_FALSE(valid);
    }
}

}  // close namespace ntsa
}  // close namespace BloombergLP
