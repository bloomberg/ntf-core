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
BSLS_IDENT_RCSID(ntsa_id_t_cpp, "$Id$ $CSID$")

#include <ntsa_id.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Id'.
class IdTest
{
  public:
    // TODO
    static void verify();
};

NTSCFG_TEST_FUNCTION(ntsa::IdTest::verify)
{
    // TESTING OBJECT ID CREATION
    //
    // Concerns:
    //   Objects that automatically generated identifiers unique within
    //   the current process. Copy construction does not generate new
    //   values. Object are properly formatted when written to streams.
    //
    // Plan:
    //   This test plan assumes that no other objects in this process
    //   have previously called 'ntsa::Id::generate()'. First,
    //   create two 'ntsa::Id's. Ensure they have been assigned
    //   integer identifiers monitonically increasing starting at 1.
    //   Ensure the objects are properly formatted when written to streams.
    //   Ensure that copy construction does not generate new identifiers;
    //   a copy-constructed 'ntsa::Id' conceptually identifies the
    //   same object as the value from which it was constructed.

    // Generate the first locally-unique identifier to this process and
    // ensure it is assigned the integer value 1.

    ntsa::Id id1(ntsa::Id::generate());
    NTSCFG_TEST_EQ(static_cast<int>(id1), 1);
    
    {
        bsl::ostringstream ss;
        ss << id1;
        NTSCFG_TEST_EQ(ss.str(), "id:000001");
    }

    // Generate the second locally-unique identifier to this process and
    // ensure it is assigned the integer value 1.

    ntsa::Id id2(ntsa::Id::generate());
    NTSCFG_TEST_EQ(static_cast<int>(id2), 2);
    {
        bsl::ostringstream ss;
        ss << id2;
        NTSCFG_TEST_EQ(ss.str(), "id:000002");
    }

    // Ensure that the two identifiers do not have the same value.

    NTSCFG_TEST_NE(id1, id2);

    // Copy construct the first identifier and ensure it has the same
    // value as the original value: copy-construction does not generate
    // an new unique value.

    ntsa::Id id1copy(id1);
    NTSCFG_TEST_EQ(id1, id1copy);

    // Copy construct the second identifier and ensure it has the same
    // value as the original value: copy-construction does not generate
    // an new unique value.

    ntsa::Id id2copy(id2);
    NTSCFG_TEST_EQ(id2, id2copy);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
