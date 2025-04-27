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
BSLS_IDENT_RCSID(ntccfg_platform_t_cpp, "$Id$ $CSID$")

#include <ntccfg_platform.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntccfg {

// Provide tests for 'ntccfg::Platform'.
class PlatformTest
{
  public:
    // TODO
    static void verifySafeInt();

    // TODO
    static void verifySafeString();
};

NTSCFG_TEST_FUNCTION(ntccfg::PlatformTest::verifySafeInt)
{
    // Default constructor.

    {
        ntccfg::Safe<int> s1(NTSCFG_TEST_ALLOCATOR);

        int v1 = 1;
        s1.load(&v1);
        NTSCFG_TEST_EQ(v1, 0);
    }

    // Value constructor.

    {
        ntccfg::Safe<int> s1(1, NTSCFG_TEST_ALLOCATOR);
        ntccfg::Safe<int> s2(2, NTSCFG_TEST_ALLOCATOR);

        {
            int v1 = 0;
            s1.load(&v1);
            NTSCFG_TEST_EQ(v1, 1);
        }

        {
            int v2 = 0;
            s2.load(&v2);
            NTSCFG_TEST_EQ(v2, 2);
        }
    }

    // Copy constructor.

    {
        ntccfg::Safe<int> s1(1, NTSCFG_TEST_ALLOCATOR);
        ntccfg::Safe<int> s2(s1, NTSCFG_TEST_ALLOCATOR);

        {
            int v1 = 0;
            s1.load(&v1);
            NTSCFG_TEST_EQ(v1, 1);
        }

        {
            int v2 = 0;
            s2.load(&v2);
            NTSCFG_TEST_EQ(v2, 1);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntccfg::PlatformTest::verifySafeString)
{
}

}  // close namespace ntccfg
}  // close namespace BloombergLP
