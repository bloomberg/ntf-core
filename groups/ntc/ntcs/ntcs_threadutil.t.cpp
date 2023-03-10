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

#include <ntcs_threadutil.h>

#include <ntccfg_test.h>

#include <ntci_log.h>

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

namespace test {

void* execute(void* context)
{
    NTCI_LOG_CONTEXT();

    NTCCFG_TEST_EQ(context, 0);

    bsl::string threadName;
    bslmt::ThreadUtil::getThreadName(&threadName);

    NTCI_LOG_DEBUG("Thread name = '%s'", threadName.c_str());

    return 0;
}

}  // close namespace 'test'

NTCCFG_TEST_CASE(1)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        NTCI_LOG_CONTEXT();

        ntsa::Error error;

        bslmt::ThreadAttributes attributes;
        attributes.setThreadName("test");

        bslmt::ThreadUtil::Handle handle;
        error =
            ntcs::ThreadUtil::create(&handle, attributes, &test::execute, 0);
        NTCCFG_TEST_OK(error);

        ntcs::ThreadUtil::join(handle);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
}
NTCCFG_TEST_DRIVER_END;
