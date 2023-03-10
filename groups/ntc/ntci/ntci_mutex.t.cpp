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

#include <ntci_mutex.h>

#include <ntccfg_test.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

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
    // Concern:
    // Plan:

#if 0
    {
        int value = 1;

        int previous = ntci::Mutex::compareAndSwap(&value, 1, 2);
        NTCCFG_TEST_EQ(previous, 1);
        NTCCFG_TEST_EQ(value, 2);
    }

    {
        int value = 1;

        int previous = ntci::Mutex::compareAndSwap(&value, 3, 2);
        NTCCFG_TEST_EQ(previous, 1);
        NTCCFG_TEST_EQ(value, 1);
    }
#endif

    ntci::Mutex mutex;

    mutex.lock();
    mutex.unlock();

    mutex.lock();
    mutex.unlock();
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
}
NTCCFG_TEST_DRIVER_END;
