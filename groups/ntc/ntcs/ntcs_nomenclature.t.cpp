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
BSLS_IDENT_RCSID(ntcs_nomenclature_t_cpp, "$Id$ $CSID$")

#include <ntcs_nomenclature.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::Nomenclature'.
class NomenclatureTest
{
  public:
    // TODO
    static void verifyReactorName();

    // TODO
    static void verifyProactorName();

    // TODO
    static void verifyThreadName();

    // TODO
    static void verifyInterfaceName();
};

NTSCFG_TEST_FUNCTION(ntcs::NomenclatureTest::verifyReactorName)
{
    bsl::string n0 = ntcs::Nomenclature::createReactorName();
    bsl::string n1 = ntcs::Nomenclature::createReactorName();
    bsl::string n2 = ntcs::Nomenclature::createReactorName();

    NTSCFG_TEST_EQ(n0, "reactor-0");
    NTSCFG_TEST_EQ(n1, "reactor-1");
    NTSCFG_TEST_EQ(n2, "reactor-2");
}

NTSCFG_TEST_FUNCTION(ntcs::NomenclatureTest::verifyProactorName)
{
    bsl::string n0 = ntcs::Nomenclature::createProactorName();
    bsl::string n1 = ntcs::Nomenclature::createProactorName();
    bsl::string n2 = ntcs::Nomenclature::createProactorName();

    NTSCFG_TEST_EQ(n0, "proactor-0");
    NTSCFG_TEST_EQ(n1, "proactor-1");
    NTSCFG_TEST_EQ(n2, "proactor-2");
}

NTSCFG_TEST_FUNCTION(ntcs::NomenclatureTest::verifyThreadName)
{
    bsl::string n0 = ntcs::Nomenclature::createThreadName();
    bsl::string n1 = ntcs::Nomenclature::createThreadName();
    bsl::string n2 = ntcs::Nomenclature::createThreadName();

    NTSCFG_TEST_EQ(n0, "thread-0");
    NTSCFG_TEST_EQ(n1, "thread-1");
    NTSCFG_TEST_EQ(n2, "thread-2");
}

NTSCFG_TEST_FUNCTION(ntcs::NomenclatureTest::verifyInterfaceName)
{
    bsl::string n0 = ntcs::Nomenclature::createInterfaceName();
    bsl::string n1 = ntcs::Nomenclature::createInterfaceName();
    bsl::string n2 = ntcs::Nomenclature::createInterfaceName();

    NTSCFG_TEST_EQ(n0, "interface-0");
    NTSCFG_TEST_EQ(n1, "interface-1");
    NTSCFG_TEST_EQ(n2, "interface-2");
}

}  // close namespace ntcs
}  // close namespace BloombergLP
