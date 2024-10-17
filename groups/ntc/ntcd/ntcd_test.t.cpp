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
BSLS_IDENT_RCSID(ntcd_timer_t_cpp, "$Id$ $CSID$")

#include <ntcd_test.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcd {

// Provide tests for 'ntcd::Test'.
class SampleTest
{
    static void verifyReactorConfig(
        const bsl::shared_ptr<ntci::Reactor>& reactor);

    static void verifyProactorConfig(
        const bsl::shared_ptr<ntci::Proactor>& proactor);

  public:
    static void verifyReactor();

    static void verifyProactor();
};

void SampleTest::verifyReactorConfig(
    const bsl::shared_ptr<ntci::Reactor>& reactor)
{
}

void SampleTest::verifyProactorConfig(
    const bsl::shared_ptr<ntci::Proactor>& proactor)
{
}

NTSCFG_TEST_FUNCTION(ntcd::SampleTest::verifyReactor)
{
    ntcd::TestFramework::verifyReactor(&ntcd::SampleTest::verifyReactorConfig);
}

NTSCFG_TEST_FUNCTION(ntcd::SampleTest::verifyProactor)
{
    ntcd::TestFramework::verifyProactor(
        &ntcd::SampleTest::verifyProactorConfig);
}

}  // close namespace ntcd
}  // close namespace BloombergLP
