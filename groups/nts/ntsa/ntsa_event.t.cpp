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

#include <ntsa_event.h>
#include <ntscfg_test.h>

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

NTSCFG_TEST_CASE(1)
{
    // Concern: 
    // Plan:

    ntscfg::TestAllocator ta;
    {
        const ntsa::Handle k_SOCKET_A = 10;
        const ntsa::Handle k_SOCKET_B = 100;
        const ntsa::Handle k_SOCKET_C = 1000;

        ntsa::EventSet eventSet(&ta);

        eventSet.setReadable(k_SOCKET_C);
        eventSet.setWritable(k_SOCKET_C);

        eventSet.setWritable(k_SOCKET_B);

        eventSet.setReadable(k_SOCKET_A);

        NTSCFG_TEST_LOG_DEBUG << "Event set = " << eventSet
                              << NTSCFG_TEST_LOG_END;

        for (ntsa::EventSet::const_iterator it  = eventSet.cbegin(); 
                                            it != eventSet.cend(); 
                                          ++it)
        {
            NTSCFG_TEST_LOG_DEBUG << "Event = " << *it 
                                  << NTSCFG_TEST_LOG_END;
        }

    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
}
NTSCFG_TEST_DRIVER_END;
