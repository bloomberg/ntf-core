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
BSLS_IDENT_RCSID(ntci_identifiable_t_cpp, "$Id$ $CSID$")

#include <ntci_identifiable.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntci {

// Provide tests for 'ntci::Identifiable'.
class IdentifiableTest
{
  public:
    // TODO
    static void verify();

  private:
    // Provide an object for use by this test driver.
    class Object : public ntci::Identifiable
    {
    };
};

NTSCFG_TEST_FUNCTION(ntci::IdentifiableTest::verify)
{
    Object object1;
    Object object2;

    NTSCFG_TEST_NE(object1.objectId(), object2.objectId());
    NTSCFG_TEST_NE(object1.guid(), object2.guid());
}

}  // close namespace ntci
}  // close namespace BloombergLP
