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
BSLS_IDENT_RCSID(ntsa_ipv4endpoint_t_cpp, "$Id$ $CSID$")

#include <ntsa_ipv4endpoint.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Ipv4Endpoint'.
class Ipv4EndpointTest
{
  public:
    // TODO
    static void verify();
};

NTSCFG_TEST_FUNCTION(ntsa::Ipv4EndpointTest::verify)
{
    ntsa::Ipv4Endpoint ipv4Endpoint("127.0.0.1:12345");

    NTSCFG_TEST_EQ(ipv4Endpoint.host(), ntsa::Ipv4Address::loopback());
    NTSCFG_TEST_EQ(ipv4Endpoint.port(), 12345);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
