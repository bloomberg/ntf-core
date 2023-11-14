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

#include <ntsa_socketoption.h>
#include <ntscfg_test.h>
#include <bslma_testallocator.h>

using namespace BloombergLP;
using namespace ntsa;

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
    // Concern: test timestampIncomingData option

    ntsa::SocketOption so;
    NTSCFG_TEST_FALSE(so.isTimestampIncomingData());

    so.makeTimestampIncomingData(true);
    NTSCFG_TEST_TRUE(so.isTimestampIncomingData());

    bool& val = so.timestampIncomingData();
    NTSCFG_TEST_TRUE(val);
    val = false;
    NTSCFG_TEST_FALSE(so.timestampIncomingData());

    val = true;
    NTSCFG_TEST_TRUE(so.timestampIncomingData());

    so.makeTimestampIncomingData();
    NTSCFG_TEST_FALSE(so.timestampIncomingData());

    so.reset();
    NTSCFG_TEST_FALSE(so.isTimestampIncomingData());
}

NTSCFG_TEST_CASE(2)
{
    // Concern: test timestampOutgoingData option

    ntsa::SocketOption so;
    NTSCFG_TEST_FALSE(so.isTimestampOutgoingData());

    so.makeTimestampOutgoingData(true);
    NTSCFG_TEST_TRUE(so.isTimestampOutgoingData());

    bool& val = so.timestampOutgoingData();
    NTSCFG_TEST_TRUE(val);
    val = false;
    NTSCFG_TEST_FALSE(so.timestampOutgoingData());

    val = true;
    NTSCFG_TEST_TRUE(so.timestampOutgoingData());

    so.makeTimestampOutgoingData();
    NTSCFG_TEST_FALSE(so.timestampOutgoingData());

    so.reset();
    NTSCFG_TEST_FALSE(so.isTimestampOutgoingData());
}

NTSCFG_TEST_CASE(3)
{
    // Concern: test allowMsgZeroCopy option

    ntsa::SocketOption so;
    NTSCFG_TEST_FALSE(so.isZeroCopy());

    so.makeZeroCopy(true);
    NTSCFG_TEST_TRUE(so.isZeroCopy());

    bool& val = so.zeroCopy();
    NTSCFG_TEST_TRUE(val);
    val = false;
    NTSCFG_TEST_FALSE(so.zeroCopy());

    val = true;
    NTSCFG_TEST_TRUE(so.zeroCopy());

    so.makeZeroCopy();
    NTSCFG_TEST_FALSE(so.zeroCopy());

    so.reset();
    NTSCFG_TEST_FALSE(so.isZeroCopy());
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
}
NTSCFG_TEST_DRIVER_END;
