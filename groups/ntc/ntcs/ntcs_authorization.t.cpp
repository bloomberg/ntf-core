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
BSLS_IDENT_RCSID(ntcs_authorization_t_cpp, "$Id$ $CSID$")

#include <ntcs_authorization.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::Authorization'.
class AuthorizationTest
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

    // TODO
    static void verifyCase5();
};

NTSCFG_TEST_FUNCTION(ntcs::AuthorizationTest::verifyCase1)
{
    // Concern: Basic authorization
    // Plan:

    ntsa::Error                    error;
    ntcs::Authorization::CountType count;

    ntcs::Authorization authorization;

    // The initial authorization count is zero.

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 0);

    // Releasing an authorization fails when there are no previously
    // acquired authorizations.

    error = authorization.release();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_INVALID);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 0);

    // Acquire an authorization.

    error = authorization.acquire();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 1);

    // Release an authorization.

    error = authorization.release();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 0);

    // Releasing an authorization fails when there are no previously
    // acquired authorizations.

    error = authorization.release();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_INVALID);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 0);

    // Acquire two authorizations.

    error = authorization.acquire();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 1);

    error = authorization.acquire();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 2);

    // Release two authorizations.

    error = authorization.release();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 1);

    error = authorization.release();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 0);

    // Releasing an authorization fails when there are no previously
    // acquired authorizations.

    error = authorization.release();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_INVALID);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 0);
}

NTSCFG_TEST_FUNCTION(ntcs::AuthorizationTest::verifyCase2)
{
    // Concern: Cancellation: acquire/cancel(fails)/acquire/release/release
    // Plan:

    ntsa::Error                    error;
    ntcs::Authorization::CountType count;

    ntcs::Authorization authorization;

    // Acquire an authorization.

    error = authorization.acquire();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 1);

    // Cancel authorization.

    bool canceled = authorization.abort();
    NTSCFG_TEST_FALSE(canceled);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 1);

    // Acquire an authorization.

    error = authorization.acquire();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 2);

    // Release an authorization.

    error = authorization.release();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 1);

    // Release an authorization.

    error = authorization.release();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 0);
}

NTSCFG_TEST_FUNCTION(ntcs::AuthorizationTest::verifyCase3)
{
    // Concern: Cancellation: acquire/cancel(fails)/release/acquire/release
    // Plan:

    ntsa::Error                    error;
    ntcs::Authorization::CountType count;

    ntcs::Authorization authorization;

    // Acquire an authorization.

    error = authorization.acquire();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 1);

    // Cancel authorization.

    bool canceled = authorization.abort();
    NTSCFG_TEST_FALSE(canceled);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 1);

    // Release an authorization.

    error = authorization.release();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 0);

    // Acquire an authorization.

    error = authorization.acquire();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 1);

    // Release an authorization.

    error = authorization.release();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 0);
}

NTSCFG_TEST_FUNCTION(ntcs::AuthorizationTest::verifyCase4)
{
    // Concern: Cancellation: cancel/acquire(fails)
    // Plan:

    ntsa::Error                    error;
    ntcs::Authorization::CountType count;

    ntcs::Authorization authorization;

    // Cancel authorization.

    bool canceled = authorization.abort();
    NTSCFG_TEST_TRUE(canceled);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, -1);

    // Acquire an authorization.

    error = authorization.acquire();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_CANCELLED);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, -1);

    // Release an authorization.

    error = authorization.release();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_CANCELLED);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, -1);
}

NTSCFG_TEST_FUNCTION(ntcs::AuthorizationTest::verifyCase5)
{
    // Concern: Basic authorization
    // Plan:

    ntsa::Error                    error;
    ntcs::Authorization::CountType count;

    ntcs::Authorization authorization(1);

    // Acquire an authorization.

    error = authorization.acquire();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 1);

    // Acquire an authorization beyond the limit and ensure authorization
    // fails.

    error = authorization.acquire();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_LIMIT);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 1);

    // Release an authorization.

    error = authorization.release();
    NTSCFG_TEST_EQ(error, ntsa::Error::e_OK);

    count = authorization.count();
    NTSCFG_TEST_EQ(count, 0);
}

}  // close namespace ntcs
}  // close namespace BloombergLP
