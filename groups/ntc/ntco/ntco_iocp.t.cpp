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
BSLS_IDENT_RCSID(ntco_iocp_t_cpp, "$Id$ $CSID$")

#include <ntco_iocp.h>

#include <ntccfg_bind.h>
#include <ntci_log.h>
#include <ntci_proactor.h>
#include <ntci_proactorsocket.h>
#include <ntco_test.h>
#include <ntsf_system.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntco {

// Provide tests for 'ntco::Iocp'.
class IocpTest
{
  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifyCase3();
};


NTSCFG_TEST_FUNCTION(ntco::IocpTest::verifyCase1)
{
#if NTC_BUILD_WITH_IOCP

    bsl::shared_ptr<ntco::IocpFactory> proactorFactory;
    proactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR,
                                  NTSCFG_TEST_ALLOCATOR);

    Test::verifyProactorSockets(proactorFactory);

#endif
}

NTSCFG_TEST_FUNCTION(ntco::IocpTest::verifyCase2)
{
#if NTC_BUILD_WITH_IOCP

    bsl::shared_ptr<ntco::IocpFactory> proactorFactory;
    proactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR,
                                  NTSCFG_TEST_ALLOCATOR);

    Test::verifyProactorTimers(proactorFactory);

#endif
}

NTSCFG_TEST_FUNCTION(ntco::IocpTest::verifyCase3)
{
#if NTC_BUILD_WITH_IOCP

    bsl::shared_ptr<ntco::IocpFactory> proactorFactory;
    proactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR,
                                  NTSCFG_TEST_ALLOCATOR);

    Test::verifyProactorFunctions(proactorFactory);

#endif
}

}  // close namespace ntco
}  // close namespace BloombergLP
