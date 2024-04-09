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

#include <ntca_encryptionnumber.h>

#include <ntccfg_test.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

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

typedef ntca::EncryptionNumber Number;

namespace test {

}  // close namespace test

NTCCFG_TEST_CASE(1)
{
    // Test default constructor.

    ntccfg::TestAllocator ta;
    {
        Number number(&ta);

        NTCCFG_TEST_EQ(number.allocator(), &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Test copy constructor.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Test move constructor.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    // Test assignment operator.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(5)
{
    // Test move-assignment operator.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(6)
{
    // Test 'reset'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(7)
{
    // Test 'move'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(8)
{
    // Test 'parse/generate'.

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;

        {
            Number number(&ta);
            error = number.parse("");
            NTCCFG_TEST_TRUE(error);
        }

        {
            Number number(&ta);
            error = number.parse("+");
            NTCCFG_TEST_TRUE(error);
        }

        {
            Number number(&ta);
            error = number.parse("-");
            NTCCFG_TEST_TRUE(error);
        }

        {
            Number number(&ta);
            error = number.parse("0x");
            NTCCFG_TEST_TRUE(error);
        }

        {
            Number number(&ta);
            error = number.parse("+0x");
            NTCCFG_TEST_TRUE(error);
        }

        {
            Number number(&ta);
            error = number.parse("-0x");
            NTCCFG_TEST_TRUE(error);
        }

        {
            bsl::string input = "0";

            Number number(&ta);
            error = number.parse(input);
            NTCCFG_TEST_OK(error);

            bsl::string output;
            number.generate(&output, 10);

            NTCCFG_TEST_EQ(output, input);
        }

        {
            bsl::string input = "123";

            Number number(&ta);
            error = number.parse(input);
            NTCCFG_TEST_OK(error);

            bsl::string output;
            number.generate(&output, 10);

            NTCCFG_TEST_LOG_DEBUG << "Testing:"
                                     "\nInput:  "
                                  << input << "\nOutput: " << output
                                  << NTCCFG_TEST_LOG_END;

            NTCCFG_TEST_EQ(output, input);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(9)
{
    // Test 'add'.

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;

        const struct Data {
            const char* lhs;
            const char* rhs;
            const char* result;
        } DATA[] = {
            #if 0
            { "0", "0", "0" },
            { "0", "1", "1" },
            { "1", "0", "1" },
            { "1", "1", "2" },
            
            { "10", "20", "30" },
            { "12", "34", "46" },
            #endif

            #if 0
            { "65535", "0", "65535" },
            { "65535", "1", "65536" },
            { "65535", "2", "65537" },
            #endif

            { "65535", "4294901761", "4294967296" }
        };

        enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

        for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
            Number lhs(&ta);
            error = lhs.parse(DATA[i].lhs);
            NTCCFG_TEST_OK(error);

            Number rhs(&ta);
            error = rhs.parse(DATA[i].rhs);
            NTCCFG_TEST_OK(error);

            bsl::string lhsString;
            lhs.generate(&lhsString, 10);

            bsl::string rhsString;
            rhs.generate(&rhsString, 10);

            Number result = lhs + rhs;

            bsl::string resultString;
            result.generate(&resultString, 10);

            NTCCFG_TEST_LOG_DEBUG 
                << "Testing:"
                << "\nLhsInput:  "
                << DATA[i].lhs 
                << "\nLhsValue:  "
                << lhsString 
                << "\nRhsInput:  "
                << DATA[i].rhs 
                << "\nRhsValue:  "
                << rhsString 
                << "\nE:         "
                << DATA[i].result
                << "\nF:         "
                << resultString
                << NTCCFG_TEST_LOG_END;
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(10)
{
    // Test 'subtract'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(11)
{
    // Test 'multiply'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(12)
{
    // Test 'divide'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(13)
{
    // Test TODO

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(14)
{
    // Test 'equals'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(15)
{
    // Test 'less'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(16)
{
    // Test 'hash'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(17)
{
    // Test 'print'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);  // Test Default constructor
    NTCCFG_TEST_REGISTER(2);  // Test copy constructor
    NTCCFG_TEST_REGISTER(3);  // Test cove constructor

    NTCCFG_TEST_REGISTER(4);  // Test assignment operator
    NTCCFG_TEST_REGISTER(5);  // Test move-assignment operator

    NTCCFG_TEST_REGISTER(6);  // Test 'reset'
    NTCCFG_TEST_REGISTER(7);  // Test 'move'

    NTCCFG_TEST_REGISTER(8);  // Test 'parse/generate'

    NTCCFG_TEST_REGISTER(9);   // Test 'add'
    NTCCFG_TEST_REGISTER(10);  // Test 'subtract'
    NTCCFG_TEST_REGISTER(11);  // Test 'multiply'
    NTCCFG_TEST_REGISTER(12);  // Test 'divide'

    NTCCFG_TEST_REGISTER(13);  // Test TODO

    NTCCFG_TEST_REGISTER(14);  // Test 'equals'
    NTCCFG_TEST_REGISTER(15);  // Test 'less'
    NTCCFG_TEST_REGISTER(16);  // Test 'hash'
    NTCCFG_TEST_REGISTER(17);  // Test 'print'
}
NTCCFG_TEST_DRIVER_END;
