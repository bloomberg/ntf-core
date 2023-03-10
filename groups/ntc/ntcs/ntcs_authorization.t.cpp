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

#include <ntcs_authorization.h>

#include <ntccfg_test.h>
#include <bslma_testallocator.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------

// [ 1]
//-----------------------------------------------------------------------------
// [ 1]
//-----------------------------------------------------------------------------

NTCCFG_TEST_CASE(1)
{
    // Concern: Basic authorization
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntsa::Error                    error;
        ntcs::Authorization::CountType count;

        ntcs::Authorization authorization;

        // The initial authorization count is zero.

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 0);

        // Releasing an authorization fails when there are no previously
        // acquired authorizations.

        error = authorization.release();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_INVALID);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 0);

        // Acquire an authorization.

        error = authorization.acquire();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 1);

        // Release an authorization.

        error = authorization.release();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 0);

        // Releasing an authorization fails when there are no previously
        // acquired authorizations.

        error = authorization.release();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_INVALID);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 0);

        // Acquire two authorizations.

        error = authorization.acquire();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 1);

        error = authorization.acquire();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 2);

        // Release two authorizations.

        error = authorization.release();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 1);

        error = authorization.release();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 0);

        // Releasing an authorization fails when there are no previously
        // acquired authorizations.

        error = authorization.release();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_INVALID);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 0);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern: Cancellation: acquire/cancel(fails)/acquire/release/release
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntsa::Error                    error;
        ntcs::Authorization::CountType count;

        ntcs::Authorization authorization;

        // Acquire an authorization.

        error = authorization.acquire();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 1);

        // Cancel authorization.

        bool canceled = authorization.abort();
        NTCCFG_TEST_FALSE(canceled);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 1);

        // Acquire an authorization.

        error = authorization.acquire();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 2);

        // Release an authorization.

        error = authorization.release();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 1);

        // Release an authorization.

        error = authorization.release();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 0);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Concern: Cancellation: acquire/cancel(fails)/release/acquire/release
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntsa::Error                    error;
        ntcs::Authorization::CountType count;

        ntcs::Authorization authorization;

        // Acquire an authorization.

        error = authorization.acquire();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 1);

        // Cancel authorization.

        bool canceled = authorization.abort();
        NTCCFG_TEST_FALSE(canceled);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 1);

        // Release an authorization.

        error = authorization.release();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 0);

        // Acquire an authorization.

        error = authorization.acquire();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 1);

        // Release an authorization.

        error = authorization.release();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 0);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    // Concern: Cancellation: cancel/acquire(fails)
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntsa::Error                    error;
        ntcs::Authorization::CountType count;

        ntcs::Authorization authorization;

        // Cancel authorization.

        bool canceled = authorization.abort();
        NTCCFG_TEST_TRUE(canceled);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, -1);

        // Acquire an authorization.

        error = authorization.acquire();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_CANCELLED);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, -1);

        // Release an authorization.

        error = authorization.release();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_CANCELLED);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, -1);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(5)
{
    // Concern: Basic authorization
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntsa::Error                    error;
        ntcs::Authorization::CountType count;

        ntcs::Authorization authorization(1);

        // Acquire an authorization.

        error = authorization.acquire();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 1);

        // Acquire an authorization beyond the limit and ensure authorization
        // fails.

        error = authorization.acquire();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_LIMIT);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 1);

        // Release an authorization.

        error = authorization.release();
        NTCCFG_TEST_EQ(error, ntsa::Error::e_OK);

        count = authorization.count();
        NTCCFG_TEST_EQ(count, 0);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
    NTCCFG_TEST_REGISTER(5);
}
NTCCFG_TEST_DRIVER_END;
