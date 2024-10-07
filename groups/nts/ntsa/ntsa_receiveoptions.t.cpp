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
BSLS_IDENT_RCSID(ntsa_receiveoptions_t_cpp, "$Id$ $CSID$")

#include <ntsa_receiveoptions.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::ReceiveOptions'.
class ReceiveOptionsTest
{
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

NTSCFG_TEST_FUNCTION(ntsa::ReceiveOptionsTest::verifyCase1)
{
    // Concern: Test want/show/hide endpoint, timestamp, and foreign handles.

    ntsa::ReceiveOptions opt;
    NTSCFG_TEST_FALSE(opt.wantTimestamp());
    NTSCFG_TEST_TRUE(opt.wantEndpoint());

    opt.hideEndpoint();
    opt.showTimestamp();
    NTSCFG_TEST_TRUE(opt.wantTimestamp());
    NTSCFG_TEST_FALSE(opt.wantEndpoint());

    opt.showEndpoint();
    NTSCFG_TEST_TRUE(opt.wantTimestamp());
    NTSCFG_TEST_TRUE(opt.wantEndpoint());

    opt.hideEndpoint();
    NTSCFG_TEST_TRUE(opt.wantTimestamp());
    NTSCFG_TEST_FALSE(opt.wantEndpoint());

    opt.hideTimestamp();
    NTSCFG_TEST_FALSE(opt.wantTimestamp());
    NTSCFG_TEST_FALSE(opt.wantEndpoint());
}

NTSCFG_TEST_FUNCTION(ntsa::ReceiveOptionsTest::verifyCase2)
{
    // Concern: test equals() method (boolean options considered only).

    ntsa::ReceiveOptions opt1;
    ntsa::ReceiveOptions opt2;
    NTSCFG_TEST_TRUE(opt1.equals(opt2));

    opt1.showTimestamp();
    NTSCFG_TEST_FALSE(opt1.equals(opt2));

    opt2.showTimestamp();
    NTSCFG_TEST_TRUE(opt1.equals(opt2));

    opt2.hideEndpoint();
    opt1.hideEndpoint();
    NTSCFG_TEST_TRUE(opt1.equals(opt2));
}

NTSCFG_TEST_FUNCTION(ntsa::ReceiveOptionsTest::verifyCase3)
{
    // Concern: test less() method (boolean options considered only).

    ntsa::ReceiveOptions opt1;
    ntsa::ReceiveOptions opt2;
    NTSCFG_TEST_FALSE(opt1.less(opt2));

    opt1.showTimestamp();
    NTSCFG_TEST_FALSE(opt1.less(opt2));

    opt2.hideEndpoint();
    NTSCFG_TEST_TRUE(opt2.less(opt1));
}

NTSCFG_TEST_FUNCTION(ntsa::ReceiveOptionsTest::verifyCase4)
{
    // Concern: wantMetaData

    ntsa::ReceiveOptions options;

    NTSCFG_TEST_FALSE(options.wantTimestamp());
    NTSCFG_TEST_FALSE(options.wantForeignHandles());
    NTSCFG_TEST_FALSE(options.wantMetaData());

    options.showTimestamp();

    NTSCFG_TEST_TRUE(options.wantTimestamp());
    NTSCFG_TEST_FALSE(options.wantForeignHandles());
    NTSCFG_TEST_TRUE(options.wantMetaData());

    options.reset();

    NTSCFG_TEST_FALSE(options.wantTimestamp());
    NTSCFG_TEST_FALSE(options.wantForeignHandles());
    NTSCFG_TEST_FALSE(options.wantMetaData());

    options.showForeignHandles();

    NTSCFG_TEST_FALSE(options.wantTimestamp());
    NTSCFG_TEST_TRUE(options.wantForeignHandles());
    NTSCFG_TEST_TRUE(options.wantMetaData());

    options.reset();

    NTSCFG_TEST_FALSE(options.wantTimestamp());
    NTSCFG_TEST_FALSE(options.wantForeignHandles());
    NTSCFG_TEST_FALSE(options.wantMetaData());

    options.showTimestamp();
    options.showForeignHandles();

    NTSCFG_TEST_TRUE(options.wantTimestamp());
    NTSCFG_TEST_TRUE(options.wantForeignHandles());
    NTSCFG_TEST_TRUE(options.wantMetaData());
}

}  // close namespace ntsa
}  // close namespace BloombergLP
