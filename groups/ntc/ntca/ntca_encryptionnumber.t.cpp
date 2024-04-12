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
#include <bsl_climits.h>

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

#define NTSA_ABSTRACT_INTEGER_LOG_INTEGRAL_OPERATION( \
    operation, data, expected, found) \
    do { \
        NTCCFG_TEST_LOG_DEBUG << "Testing " << (operation) << " operation " << (data).variationIndex << "/" << (data).variationCount << ":" \
                              << "\nLHS:  " << (data).lhs \
                              << "\nRHS:  " << (data).rhs \
                              << "\nE:    " << (expected) \
                              << "\nF:    " << (found) \
                              << NTCCFG_TEST_LOG_END; \
    } while (false)

namespace test {

struct Data {
    bsl::size_t   variationIndex;
    bsl::size_t   variationCount;
    bsl::uint64_t lhs;
    bsl::uint64_t rhs;
};

typedef bsl::vector<Data> DataVector;

// Provide implementations of test cases.
class Case
{
  private:
    static void loadData(DataVector* result);

    static void verifyBase(bslma::Allocator* allocator);
    static void verifyRepresentation(bslma::Allocator* allocator);
    static void verifyQuantity(bslma::Allocator* allocator);
    static void verifyQuantityAdd(bslma::Allocator* allocator);
    static void verifyQuantitySub(bslma::Allocator* allocator);
    static void verifyQuantityMul(bslma::Allocator* allocator);
    static void verifyQuantityDiv(bslma::Allocator* allocator);
    static void verifyQuantityMod(bslma::Allocator* allocator);

  public:
    // Verify the fundamental mechanisms used to build abstract integers.
    static void verifyPrerequisites(bslma::Allocator* allocator);
};

void Case::loadData(DataVector* result)
{
    result->clear();

    typedef bsl::vector<bsl::int64_t> DiscontinuityVector;
    typedef bsl::vector<bsl::int64_t> AdjustmentVector;

    DiscontinuityVector discontinuity;
    AdjustmentVector adjustment;

    discontinuity.push_back(0);
    discontinuity.push_back(255);
    discontinuity.push_back(65535);
    discontinuity.push_back(4294967295);

    adjustment.push_back(-3);
    adjustment.push_back(-2);
    adjustment.push_back(-1);
    adjustment.push_back(0);
    adjustment.push_back(1);
    adjustment.push_back(2);
    adjustment.push_back(3);

    for (bsl::size_t i = 0; i < discontinuity.size(); ++i) {
        for (bsl::size_t j = 0; j < adjustment.size(); ++j) {
            for (bsl::size_t k = 0; k < discontinuity.size(); ++k) {
                for (bsl::size_t l = 0; l < adjustment.size(); ++l) {
                    const bsl::int64_t lhsValue = 
                        discontinuity[i] + adjustment[j];

                    const bsl::int64_t rhsValue = 
                        discontinuity[k] + adjustment[l];

                    if (lhsValue > 0 && rhsValue > 0) {
                        Data data;
                        data.lhs = lhsValue;
                        data.rhs = rhsValue;



                        result->push_back(data);
                    }                    
                }
            }
        }
    }

    for (bsl::size_t i = 0; i < result->size(); ++i) {
        (*result)[i].variationIndex = i;
        (*result)[i].variationCount = result->size();
    }
}

void Case::verifyBase(bslma::Allocator* allocator)
{
    {
        bsl::size_t radix = ntca::AbstractIntegerBase::radix(
            ntca::AbstractIntegerBase::e_NATIVE);
        NTCCFG_TEST_EQ(radix, 65536);
    }

    {
        bsl::size_t radix = ntca::AbstractIntegerBase::radix(
            ntca::AbstractIntegerBase::e_BINARY);
        NTCCFG_TEST_EQ(radix, 2);
    }

    {
        bsl::size_t radix = ntca::AbstractIntegerBase::radix(
            ntca::AbstractIntegerBase::e_OCTAL);
        NTCCFG_TEST_EQ(radix, 8);
    }

    {
        bsl::size_t radix = ntca::AbstractIntegerBase::radix(
            ntca::AbstractIntegerBase::e_DECIMAL);
        NTCCFG_TEST_EQ(radix, 10);
    }

    {
        bsl::size_t radix = ntca::AbstractIntegerBase::radix(
            ntca::AbstractIntegerBase::e_HEXADECIMAL);
        NTCCFG_TEST_EQ(radix, 16);
    }
}

void Case::verifyRepresentation(bslma::Allocator* allocator)
{
    typedef bsl::vector<ntca::AbstractIntegerBase::Value> BaseVector;
    BaseVector baseVector;

    baseVector.push_back(ntca::AbstractIntegerBase::e_NATIVE);
    baseVector.push_back(ntca::AbstractIntegerBase::e_BINARY);
    baseVector.push_back(ntca::AbstractIntegerBase::e_OCTAL);
    baseVector.push_back(ntca::AbstractIntegerBase::e_DECIMAL);
    baseVector.push_back(ntca::AbstractIntegerBase::e_HEXADECIMAL);

    for (bsl::size_t i = 0; i < baseVector.size(); ++i)
    {
        const ntca::AbstractIntegerBase::Value base = baseVector[i];

        ntca::AbstractIntegerRepresentation rep(base, allocator);
        NTCCFG_TEST_EQ(rep.allocator(), allocator);

        NTCCFG_TEST_EQ(rep.size(), 0);
        NTCCFG_TEST_EQ(rep.base(), base);

        NTCCFG_TEST_LOG_DEBUG << "Rep = " << rep << NTCCFG_TEST_LOG_END;
        rep.assign(123);
        NTCCFG_TEST_LOG_DEBUG << "Rep = " << rep << NTCCFG_TEST_LOG_END;
    }
}

void Case::verifyQuantity(bslma::Allocator* allocator)
{
    verifyQuantityAdd(allocator);
    verifyQuantitySub(allocator);
    verifyQuantityMul(allocator);
    //verifyQuantityDiv(allocator);
    //verifyQuantityMod(allocator);
}

void Case::verifyQuantityAdd(bslma::Allocator* allocator)
{
    test::DataVector dataVector(allocator);
    test::Case::loadData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        test::Data data = dataVector[i];

        ntca::AbstractIntegerQuantity lhs(data.lhs, allocator);
        ntca::AbstractIntegerQuantity rhs(data.rhs, allocator);

        ntca::AbstractIntegerQuantity sum(allocator);

        ntca::AbstractIntegerQuantityUtil::add(&sum, lhs, rhs);

        bsl::string expected = bsl::to_string(data.lhs + data.rhs);

        bsl::string found;
        sum.generate(&found, 
                     ntca::AbstractIntegerSign::e_POSITIVE, 
                     ntca::AbstractIntegerBase::e_DECIMAL);

        NTSA_ABSTRACT_INTEGER_LOG_INTEGRAL_OPERATION(
            "add", data, expected, found);

        NTCCFG_TEST_EQ(found, expected);
    }
}

void Case::verifyQuantitySub(bslma::Allocator* allocator)
{
    test::DataVector dataVector(allocator);
    test::Case::loadData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        test::Data data = dataVector[i];

        ntca::AbstractIntegerQuantity lhs(data.lhs, allocator);
        ntca::AbstractIntegerQuantity rhs(data.rhs, allocator);

        ntca::AbstractIntegerQuantity difference(allocator);

        ntca::AbstractIntegerQuantityUtil::subtract(&difference, lhs, rhs);

        bsl::string expected; 
        if (data.lhs < data.rhs) {
            expected = "0";
        }
        else {
            expected = bsl::to_string(data.lhs - data.rhs);
        }

        bsl::string found;
        difference.generate(&found, 
                     ntca::AbstractIntegerSign::e_POSITIVE, 
                     ntca::AbstractIntegerBase::e_DECIMAL);

        NTSA_ABSTRACT_INTEGER_LOG_INTEGRAL_OPERATION(
            "subtract", data, expected, found);

        NTCCFG_TEST_EQ(found, expected);
    }
}

void Case::verifyQuantityMul(bslma::Allocator* allocator)
{
    test::DataVector dataVector(allocator);
    test::Case::loadData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        test::Data data = dataVector[i];

        // The multiplication of values >= 2^32 overflows an unsigned 64-bit
        // integer, so skip those variations.

        if (data.lhs >= bsl::numeric_limits<bsl::uint32_t>::max() &&
            data.rhs >= bsl::numeric_limits<bsl::uint32_t>::max())
        {
            continue;
        }

        ntca::AbstractIntegerQuantity lhs(data.lhs, allocator);
        ntca::AbstractIntegerQuantity rhs(data.rhs, allocator);

        ntca::AbstractIntegerQuantity product(allocator);

        ntca::AbstractIntegerQuantityUtil::multiply(&product, lhs, rhs);

        bsl::string expected = bsl::to_string(data.lhs * data.rhs);

        bsl::string found;
        product.generate(&found, 
                     ntca::AbstractIntegerSign::e_POSITIVE, 
                     ntca::AbstractIntegerBase::e_DECIMAL);

        NTSA_ABSTRACT_INTEGER_LOG_INTEGRAL_OPERATION(
            "multiply", data, expected, found);

        NTCCFG_TEST_EQ(found, expected);
    }
}

void Case::verifyQuantityDiv(bslma::Allocator* allocator)
{

}

void Case::verifyQuantityMod(bslma::Allocator* allocator)
{

}

void Case::verifyPrerequisites(bslma::Allocator* allocator)
{
    verifyBase(allocator);
    verifyRepresentation(allocator);
    verifyQuantity(allocator);
}

}  // close namespace test

NTCCFG_TEST_CASE(1)
{
    // Test prerequisites.

    ntccfg::TestAllocator ta;
    {
        test::Case::verifyPrerequisites(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Test default constructor.

    ntccfg::TestAllocator ta;
    {
        ntca::AbstractInteger number(&ta);

        NTCCFG_TEST_EQ(number.allocator(), &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Test copy constructor.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    // Test move constructor.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(5)
{
    // Test assignment operator.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(6)
{
    // Test move-assignment operator.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(7)
{
    // Test 'reset'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(8)
{
    // Test 'move'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(9)
{
    // Test 'parse/generate'.

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;

        {
            ntca::AbstractInteger number(&ta);
            error = number.parse("");
            NTCCFG_TEST_TRUE(error);
        }

        {
            ntca::AbstractInteger number(&ta);
            error = number.parse("+");
            NTCCFG_TEST_TRUE(error);
        }

        {
            ntca::AbstractInteger number(&ta);
            error = number.parse("-");
            NTCCFG_TEST_TRUE(error);
        }

        {
            ntca::AbstractInteger number(&ta);
            error = number.parse("0x");
            NTCCFG_TEST_TRUE(error);
        }

        {
            ntca::AbstractInteger number(&ta);
            error = number.parse("+0x");
            NTCCFG_TEST_TRUE(error);
        }

        {
            ntca::AbstractInteger number(&ta);
            error = number.parse("-0x");
            NTCCFG_TEST_TRUE(error);
        }

        {
            bsl::string input = "0";

            ntca::AbstractInteger number(&ta);
            error = number.parse(input);
            NTCCFG_TEST_OK(error);

            bsl::string output;
            number.generate(&output, 10);

            NTCCFG_TEST_EQ(output, input);
        }

        {
            bsl::string input = "123";

            ntca::AbstractInteger number(&ta);
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

NTCCFG_TEST_CASE(10)
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
#endif

            {"65535", "2", "65537"},

 // { "4294901761", "65535", "4294967296" }
        };

        enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

        for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
            NTCCFG_TEST_LOG_DEBUG << "Parse starting" << NTCCFG_TEST_LOG_END;

            ntca::AbstractInteger lhs(&ta);
            error = lhs.parse(DATA[i].lhs);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_LOG_DEBUG << "Parse complete" << NTCCFG_TEST_LOG_END;

            ntca::AbstractInteger rhs(&ta);
            error = rhs.parse(DATA[i].rhs);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_LOG_DEBUG << "Generate starting"
                                  << NTCCFG_TEST_LOG_END;

            bsl::string lhsString;
            lhs.generate(&lhsString, 10);

            bsl::string rhsString;
            rhs.generate(&rhsString, 10);

            NTCCFG_TEST_LOG_DEBUG << "Generate complete"
                                  << NTCCFG_TEST_LOG_END;

            NTCCFG_TEST_LOG_DEBUG << "Add starting" << NTCCFG_TEST_LOG_END;

            ntca::AbstractInteger result = lhs + rhs;

            NTCCFG_TEST_LOG_DEBUG << "Add complete" << NTCCFG_TEST_LOG_END;

            bsl::string resultString;
            result.generate(&resultString, 10);

            NTCCFG_TEST_LOG_DEBUG << "Testing:"
                                  << "\nLhsInput:  " << DATA[i].lhs
                                  << "\nLhsValue:  " << lhsString
                                  << "\nRhsInput:  " << DATA[i].rhs
                                  << "\nRhsValue:  " << rhsString
                                  << "\nE:         " << DATA[i].result
                                  << "\nF:         " << resultString
                                  << NTCCFG_TEST_LOG_END;
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(11)
{
    // Test 'subtract'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(12)
{
    // Test 'multiply'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(13)
{
    // Test 'divide'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(14)
{
    // Test TODO

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(15)
{
    // Test 'equals'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(16)
{
    // Test 'less'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(17)
{
    // Test 'hash'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(18)
{
    // Test 'print'.

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);  // Test prerequisites

    NTCCFG_TEST_REGISTER(2);  // Test default constructor
    NTCCFG_TEST_REGISTER(3);  // Test copy constructor
    NTCCFG_TEST_REGISTER(4);  // Test cove constructor

    NTCCFG_TEST_REGISTER(5);  // Test assignment operator
    NTCCFG_TEST_REGISTER(6);  // Test move-assignment operator

    NTCCFG_TEST_REGISTER(7);  // Test 'reset'
    NTCCFG_TEST_REGISTER(8);  // Test 'move'

    NTCCFG_TEST_REGISTER(9);  // Test 'parse/generate'

    NTCCFG_TEST_REGISTER(10);  // Test 'add'
    NTCCFG_TEST_REGISTER(11);  // Test 'subtract'
    NTCCFG_TEST_REGISTER(12);  // Test 'multiply'
    NTCCFG_TEST_REGISTER(13);  // Test 'divide'

    NTCCFG_TEST_REGISTER(14);  // Test TODO

    NTCCFG_TEST_REGISTER(15);  // Test 'equals'
    NTCCFG_TEST_REGISTER(16);  // Test 'less'
    NTCCFG_TEST_REGISTER(17);  // Test 'hash'
    NTCCFG_TEST_REGISTER(18);  // Test 'print'
}
NTCCFG_TEST_DRIVER_END;
