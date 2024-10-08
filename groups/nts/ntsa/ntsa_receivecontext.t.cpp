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
BSLS_IDENT_RCSID(ntsa_receivecontext_t_cpp, "$Id$ $CSID$")

#include <ntsa_receivecontext.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::ReceiveContext'.
class ReceiveContextTest
{
  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();
};

NTSCFG_TEST_FUNCTION(ntsa::ReceiveContextTest::verifyCase1)
{
    ntsa::ReceiveContext ctx;

    NTSCFG_TEST_TRUE(ctx.softwareTimestamp().isNull());
    NTSCFG_TEST_TRUE(ctx.hardwareTimestamp().isNull());

    const bsls::TimeInterval swTs(100, 200);
    const bsls::TimeInterval hwTs(300, 500);

    ctx.setSoftwareTimestamp(swTs);
    ctx.setHardwareTimestamp(hwTs);

    NTSCFG_TEST_FALSE(ctx.softwareTimestamp().isNull());
    NTSCFG_TEST_FALSE(ctx.hardwareTimestamp().isNull());

    NTSCFG_TEST_EQ(ctx.softwareTimestamp().value(), swTs);
    NTSCFG_TEST_EQ(ctx.hardwareTimestamp().value(), hwTs);
}

NTSCFG_TEST_FUNCTION(ntsa::ReceiveContextTest::verifyCase2)
{
    ntsa::ReceiveContext ctx1;
    ntsa::ReceiveContext ctx2;

    NTSCFG_TEST_TRUE(ctx1.equals(ctx2));

    const bsls::TimeInterval bigTs(200, 0);
    const bsls::TimeInterval smallTs(100, 0);

    ctx1.setHardwareTimestamp(bigTs);
    ctx2.setHardwareTimestamp(smallTs);

    NTSCFG_TEST_TRUE(ctx2.less(ctx1));

    ctx1.setSoftwareTimestamp(smallTs);
    ctx2.setSoftwareTimestamp(bigTs);

    NTSCFG_TEST_TRUE(ctx1.less(ctx2));
}

}  // close namespace ntsa
}  // close namespace BloombergLP
