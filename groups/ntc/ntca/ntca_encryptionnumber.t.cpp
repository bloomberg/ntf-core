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

// #define NTCA_ENCRYPTION_NUMBER_CASE 1

#define NTSA_ABSTRACT_INTEGER_LOG_INTEGRAL_OPERATION( \
    operation, data, expected, found) \
    do { \
        NTCCFG_TEST_LOG_DEBUG << "Testing " << (operation) << " operation " \
                              << (data).variationIndex \
                              << "/" \
                              << (data).variationCount \
                              << ":" \
                              << "\nLHS:  " << (data).lhs \
                              << "\nRHS:  " << (data).rhs \
                              << "\nE:    " << (expected) \
                              << "\nF:    " << (found) \
                              << NTCCFG_TEST_LOG_END; \
    } while (false)

#define NTSA_ABSTRACT_INTEGER_LOG_ADD(data, expected, found) \
    NTSA_ABSTRACT_INTEGER_LOG_INTEGRAL_OPERATION( \
        "add", data, expected, found)

#define NTSA_ABSTRACT_INTEGER_LOG_SUBTRACT(data, expected, found) \
    NTSA_ABSTRACT_INTEGER_LOG_INTEGRAL_OPERATION( \
        "subtract", data, expected, found)

#define NTSA_ABSTRACT_INTEGER_LOG_MULTIPLY(data, expected, found) \
    NTSA_ABSTRACT_INTEGER_LOG_INTEGRAL_OPERATION( \
        "multiply", data, expected, found)

#define NTSA_ABSTRACT_INTEGER_LOG_DIVIDE( \
    data, eq, fq, er, fr) \
    do { \
        NTCCFG_TEST_LOG_DEBUG << "Testing divide operation " \
                              << (data).variationIndex \
                              << "/" \
                              << (data).variationCount \
                              << ":" \
                              << "\nLHS:  " << (data).lhs \
                              << "\nRHS:  " << (data).rhs \
                              << "\nEQ:   " << (eq) \
                              << "\nFQ:   " << (fq) \
                              << "\nER:   " << (er) \
                              << "\nFR:   " << (fr) \
                              << NTCCFG_TEST_LOG_END; \
    } while (false)


namespace test {

struct UnsignedIntegerData {
    bsl::size_t   variationIndex;
    bsl::size_t   variationCount;
    bsl::uint64_t lhs;
    bsl::uint64_t rhs;

    bool multiplicationWouldOverflow() const
    {
        if (rhs != 0 && lhs > UINT64_MAX / rhs) {
            return true;
        }

        return false;
    }
};

typedef bsl::vector<UnsignedIntegerData> UnsignedIntegerDataVector;

struct SignedIntegerData {
    bsl::size_t   variationIndex;
    bsl::size_t   variationCount;
    bsl::int64_t lhs;
    bsl::int64_t rhs;

    bool multiplicationWouldOverflow() const
    {
        if (lhs > 0 && rhs > 0 && lhs > INT64_MAX / rhs) {
            return true;
        }

        if (lhs < 0 && rhs > 0 && lhs < INT64_MIN / rhs) {
            return true;
        }

        if (lhs > 0 && rhs < 0 && rhs < INT64_MIN / lhs) {
            return true;
        }

        if (lhs < 0 && rhs < 0 && lhs < INT64_MAX / rhs) {
            return true;
        }

        return false;
    }
};

typedef bsl::vector<SignedIntegerData> SignedIntegerDataVector;

// Provide implementations of test cases.
class Case
{
  private:
    static void loadUnsignedIntegerData(UnsignedIntegerDataVector* result);
    static void loadSignedIntegerData(SignedIntegerDataVector* result);

    static void verifyNativeMath(bslma::Allocator* allocator);
    static void verifyBase(bslma::Allocator* allocator);
    static void verifyRepresentation(bslma::Allocator* allocator);

  public:
    // Verify the fundamental mechanisms used to build abstract integers.
    static void verifyPrerequisites(bslma::Allocator* allocator);

    static void verifyUnsignedIntegerAdd(bslma::Allocator* allocator);
    static void verifyUnsignedIntegerSubtract(bslma::Allocator* allocator);
    static void verifyUnsignedIntegerMultiply(bslma::Allocator* allocator);
    static void verifyUnsignedIntegerDivide(bslma::Allocator* allocator);


    static void verifySignedIntegerAdd(bslma::Allocator* allocator);
    static void verifySignedIntegerSubtract(bslma::Allocator* allocator);
    static void verifySignedIntegerMultiply(bslma::Allocator* allocator);
    static void verifySignedIntegerDivide(bslma::Allocator* allocator);
};

void Case::loadUnsignedIntegerData(UnsignedIntegerDataVector* result)
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

    const int N = 13;

    for (int i = 0; i < N; ++i) {
        adjustment.push_back(-i);
    }

    adjustment.push_back(0);

    for (int i = 0; i < N; ++i) {
        adjustment.push_back(+i);
    }

    for (bsl::size_t i = 0; i < discontinuity.size(); ++i) {
        for (bsl::size_t j = 0; j < adjustment.size(); ++j) {
            for (bsl::size_t k = 0; k < discontinuity.size(); ++k) {
                for (bsl::size_t l = 0; l < adjustment.size(); ++l) {
                    const bsl::int64_t lhsValue = 
                        discontinuity[i] + adjustment[j];

                    const bsl::int64_t rhsValue = 
                        discontinuity[k] + adjustment[l];

                    if (lhsValue > 0 && rhsValue > 0) {
                        UnsignedIntegerData data;
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

void Case::loadSignedIntegerData(SignedIntegerDataVector* result)
{
    result->clear();

    typedef bsl::vector<bsl::int64_t> DiscontinuityVector;
    typedef bsl::vector<bsl::int64_t> AdjustmentVector;

    DiscontinuityVector discontinuity;
    AdjustmentVector adjustment;

    discontinuity.push_back(0);
    discontinuity.push_back(255);
    discontinuity.push_back(-255);
    discontinuity.push_back(65535);
    discontinuity.push_back(-65535);
    discontinuity.push_back(4294967295);
    discontinuity.push_back(-4294967295);

    const int N = 13;

    for (int i = 0; i < N; ++i) {
        adjustment.push_back(-i);
    }

    adjustment.push_back(0);

    for (int i = 0; i < N; ++i) {
        adjustment.push_back(+i);
    }

    for (bsl::size_t i = 0; i < discontinuity.size(); ++i) {
        for (bsl::size_t j = 0; j < adjustment.size(); ++j) {
            for (bsl::size_t k = 0; k < discontinuity.size(); ++k) {
                for (bsl::size_t l = 0; l < adjustment.size(); ++l) {
                    const bsl::int64_t lhsValue = 
                        discontinuity[i] + adjustment[j];

                    const bsl::int64_t rhsValue = 
                        discontinuity[k] + adjustment[l];

                    SignedIntegerData data;
                    data.lhs = lhsValue;
                    data.rhs = rhsValue;

                    result->push_back(data);                   
                }
            }
        }
    }

    for (bsl::size_t i = 0; i < result->size(); ++i) {
        (*result)[i].variationIndex = i;
        (*result)[i].variationCount = result->size();
    }
}



void Case::verifyNativeMath(bslma::Allocator* allocator)
{
    {
        bsl::uint64_t a = 1;
        bsl::uint64_t b = 10;

        bsl::uint64_t q = a / b;
        bsl::uint64_t r = a % b;

        NTCCFG_TEST_EQ(q, 0);
        NTCCFG_TEST_EQ(r, a);
    }

    {
        bsl::int64_t a = 0;
        bsl::int64_t b = -1;

        bsl::int64_t q = a / b;
        bsl::int64_t r = a % b;

        NTCCFG_TEST_EQ(q, 0);
        NTCCFG_TEST_EQ(r, 0);
    }
}

void Case::verifyBase(bslma::Allocator* allocator)
{
    {
        bsl::uint64_t radix = ntca::AbstractIntegerBase::radix(
            ntca::AbstractIntegerBase::e_NATIVE);

        bsl::uint64_t expected = 
            1 << (sizeof(ntca::AbstractIntegerRepresentation::Block) * 8);

        NTCCFG_TEST_EQ(radix, expected);
    }

    {
        bsl::uint64_t radix = ntca::AbstractIntegerBase::radix(
            ntca::AbstractIntegerBase::e_BINARY);
        NTCCFG_TEST_EQ(radix, 2);
    }

    {
        bsl::uint64_t radix = ntca::AbstractIntegerBase::radix(
            ntca::AbstractIntegerBase::e_OCTAL);
        NTCCFG_TEST_EQ(radix, 8);
    }

    {
        bsl::uint64_t radix = ntca::AbstractIntegerBase::radix(
            ntca::AbstractIntegerBase::e_DECIMAL);
        NTCCFG_TEST_EQ(radix, 10);
    }

    {
        bsl::uint64_t radix = ntca::AbstractIntegerBase::radix(
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

void Case::verifyPrerequisites(bslma::Allocator* allocator)
{
    verifyNativeMath(allocator);
    verifyBase(allocator);
    verifyRepresentation(allocator);
}

void Case::verifyUnsignedIntegerAdd(bslma::Allocator* allocator)
{
    test::UnsignedIntegerDataVector dataVector(allocator);
    test::Case::loadUnsignedIntegerData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        test::UnsignedIntegerData data = dataVector[i];

#if defined(NTCA_ENCRYPTION_NUMBER_CASE)
        if (data.variationIndex != NTCA_ENCRYPTION_NUMBER_CASE) {
            continue;
        }
#endif

        ntca::AbstractIntegerQuantity lhs(data.lhs, allocator);
        ntca::AbstractIntegerQuantity rhs(data.rhs, allocator);

        ntca::AbstractIntegerQuantity sum(allocator);

        ntca::AbstractIntegerQuantityUtil::add(&sum, lhs, rhs);

        bsl::string expected = bsl::to_string(data.lhs + data.rhs);

        bsl::string found;
        sum.generate(&found, 
                     ntca::AbstractIntegerSign::e_POSITIVE, 
                     ntca::AbstractIntegerBase::e_DECIMAL);

        NTSA_ABSTRACT_INTEGER_LOG_ADD(data, expected, found);

        NTCCFG_TEST_EQ(found, expected);
    }
}

void Case::verifyUnsignedIntegerSubtract(bslma::Allocator* allocator)
{
    test::UnsignedIntegerDataVector dataVector(allocator);
    test::Case::loadUnsignedIntegerData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        test::UnsignedIntegerData data = dataVector[i];

#if defined(NTCA_ENCRYPTION_NUMBER_CASE)
        if (data.variationIndex != NTCA_ENCRYPTION_NUMBER_CASE) {
            continue;
        }
#endif

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

        NTSA_ABSTRACT_INTEGER_LOG_SUBTRACT(data, expected, found);

        NTCCFG_TEST_EQ(found, expected);
    }
}

void Case::verifyUnsignedIntegerMultiply(bslma::Allocator* allocator)
{
    test::UnsignedIntegerDataVector dataVector(allocator);
    test::Case::loadUnsignedIntegerData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        test::UnsignedIntegerData data = dataVector[i];

#if defined(NTCA_ENCRYPTION_NUMBER_CASE)
        if (data.variationIndex != NTCA_ENCRYPTION_NUMBER_CASE) {
            continue;
        }
#endif

        // Skip variations whose products overflow 64-bit unsigned integers. 

        if (data.multiplicationWouldOverflow()) {
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

        NTSA_ABSTRACT_INTEGER_LOG_MULTIPLY(data, expected, found);

        NTCCFG_TEST_EQ(found, expected);
    }
}

void Case::verifyUnsignedIntegerDivide(bslma::Allocator* allocator)
{
    test::UnsignedIntegerDataVector dataVector(allocator);
    test::Case::loadUnsignedIntegerData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        test::UnsignedIntegerData data = dataVector[i];

#if defined(NTCA_ENCRYPTION_NUMBER_CASE)
        if (data.variationIndex != NTCA_ENCRYPTION_NUMBER_CASE) {
            continue;
        }
#endif

        if (data.rhs == 0) {
            continue;
        }

        ntca::AbstractIntegerQuantity lhs(data.lhs, allocator);
        ntca::AbstractIntegerQuantity rhs(data.rhs, allocator);

        ntca::AbstractIntegerQuantity q(allocator);
        ntca::AbstractIntegerQuantity r(allocator);

        ntca::AbstractIntegerQuantityUtil::divide(&q, &r, lhs, rhs);

        bsl::string eq = bsl::to_string(data.lhs / data.rhs);
        bsl::string er = bsl::to_string(data.lhs % data.rhs);

        bsl::string fq;
        q.generate(&fq, ntca::AbstractIntegerSign::e_POSITIVE, 
                        ntca::AbstractIntegerBase::e_DECIMAL);

        bsl::string fr;
        r.generate(&fr, ntca::AbstractIntegerSign::e_POSITIVE, 
                        ntca::AbstractIntegerBase::e_DECIMAL);

        NTSA_ABSTRACT_INTEGER_LOG_DIVIDE(data, eq, fq, er, fr);

        NTCCFG_TEST_EQ(fq, eq);
        NTCCFG_TEST_EQ(fr, er);
    }
}



































void Case::verifySignedIntegerAdd(bslma::Allocator* allocator)
{
    test::SignedIntegerDataVector dataVector(allocator);
    test::Case::loadSignedIntegerData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        test::SignedIntegerData data = dataVector[i];

#if defined(NTCA_ENCRYPTION_NUMBER_CASE)
        if (data.variationIndex != NTCA_ENCRYPTION_NUMBER_CASE) {
            continue;
        }
#endif

        ntca::AbstractInteger lhs(data.lhs, allocator);
        ntca::AbstractInteger rhs(data.rhs, allocator);

        ntca::AbstractInteger sum(allocator);

        ntca::AbstractIntegerUtil::add(&sum, lhs, rhs);

        bsl::string expected = bsl::to_string(data.lhs + data.rhs);

        bsl::string found;
        sum.generate(&found, 
                     ntca::AbstractIntegerBase::e_DECIMAL);

        NTSA_ABSTRACT_INTEGER_LOG_ADD(data, expected, found);

        NTCCFG_TEST_EQ(found, expected);
    }
}

void Case::verifySignedIntegerSubtract(bslma::Allocator* allocator)
{
    test::SignedIntegerDataVector dataVector(allocator);
    test::Case::loadSignedIntegerData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        test::SignedIntegerData data = dataVector[i];

#if defined(NTCA_ENCRYPTION_NUMBER_CASE)
        if (data.variationIndex != NTCA_ENCRYPTION_NUMBER_CASE) {
            continue;
        }
#endif

        ntca::AbstractInteger lhs(data.lhs, allocator);
        ntca::AbstractInteger rhs(data.rhs, allocator);

        ntca::AbstractInteger difference(allocator);

        ntca::AbstractIntegerUtil::subtract(&difference, lhs, rhs);

        bsl::string expected = bsl::to_string(data.lhs - data.rhs);

        bsl::string found;
        difference.generate(&found, 
                     ntca::AbstractIntegerBase::e_DECIMAL);

        NTSA_ABSTRACT_INTEGER_LOG_SUBTRACT(data, expected, found);

        NTCCFG_TEST_EQ(found, expected);
    }
}

void Case::verifySignedIntegerMultiply(bslma::Allocator* allocator)
{
    test::SignedIntegerDataVector dataVector(allocator);
    test::Case::loadSignedIntegerData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        test::SignedIntegerData data = dataVector[i];

#if defined(NTCA_ENCRYPTION_NUMBER_CASE)
        if (data.variationIndex != NTCA_ENCRYPTION_NUMBER_CASE) {
            continue;
        }
#endif

        // Skip variations whose products overflow 64-bit signed integers. 

        if (data.multiplicationWouldOverflow()) {
            continue;
        }
        
        ntca::AbstractInteger lhs(data.lhs, allocator);
        ntca::AbstractInteger rhs(data.rhs, allocator);

        ntca::AbstractInteger product(allocator);

        ntca::AbstractIntegerUtil::multiply(&product, lhs, rhs);

        bsl::string expected = bsl::to_string(data.lhs * data.rhs);

        bsl::string found;
        product.generate(&found, 
                     ntca::AbstractIntegerBase::e_DECIMAL);

        NTSA_ABSTRACT_INTEGER_LOG_MULTIPLY(data, expected, found);

        NTCCFG_TEST_EQ(found, expected);
    }
}

void Case::verifySignedIntegerDivide(bslma::Allocator* allocator)
{
    test::SignedIntegerDataVector dataVector(allocator);
    test::Case::loadSignedIntegerData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        test::SignedIntegerData data = dataVector[i];

#if defined(NTCA_ENCRYPTION_NUMBER_CASE)
        if (data.variationIndex != NTCA_ENCRYPTION_NUMBER_CASE) {
            continue;
        }
#endif

        if (data.rhs == 0) {
            continue;
        }

        ntca::AbstractInteger lhs(data.lhs, allocator);
        ntca::AbstractInteger rhs(data.rhs, allocator);

        ntca::AbstractInteger q(allocator);
        ntca::AbstractInteger r(allocator);

        ntca::AbstractIntegerUtil::divide(&q, &r, lhs, rhs);

        bsl::string eq = bsl::to_string(data.lhs / data.rhs);
        bsl::string er = bsl::to_string(data.lhs % data.rhs);

        bsl::string fq;
        q.generate(&fq, ntca::AbstractIntegerBase::e_DECIMAL);

        bsl::string fr;
        r.generate(&fr, ntca::AbstractIntegerBase::e_DECIMAL);

        NTSA_ABSTRACT_INTEGER_LOG_DIVIDE(data, eq, fq, er, fr);

        NTCCFG_TEST_EQ(fq, eq);
        NTCCFG_TEST_EQ(fr, er);
    }
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
    // Test abstract unsigned integer addition.

    ntccfg::TestAllocator ta;
    {
        test::Case::verifyUnsignedIntegerAdd(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Test abstract unsigned integer subtraction.

    ntccfg::TestAllocator ta;
    {
        test::Case::verifyUnsignedIntegerSubtract(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    // Test abstract unsigned integer multiplication.

    ntccfg::TestAllocator ta;
    {
        test::Case::verifyUnsignedIntegerMultiply(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(5)
{
    // Test abstract unsigned integer division.

    ntccfg::TestAllocator ta;
    {
        test::Case::verifyUnsignedIntegerDivide(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(6)
{
    // Test abstract unsigned integer facilities.

    // Test default constructor
    // Test copy constructor
    // Test move constructor
    // Test assignment operator
    // Test move-assignment operator
    // Test move-assignment operator.
    // Test 'reset'
    // Test 'move'

    // Test 'parse/generate'
    // Test 'equals'
    // Test 'less'
    // Test 'hash'
    // Test 'print'

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(7)
{
    // Test abstract signed integer addition.

    ntccfg::TestAllocator ta;
    {
        test::Case::verifySignedIntegerAdd(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(8)
{
    // Test abstract signed integer subtraction.

    ntccfg::TestAllocator ta;
    {
        test::Case::verifySignedIntegerSubtract(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(9)
{
    // Test abstract signed integer multiplication.

    ntccfg::TestAllocator ta;
    {
        test::Case::verifySignedIntegerMultiply(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(10)
{
    // Test abstract signed integer division.

    ntccfg::TestAllocator ta;
    {
        test::Case::verifySignedIntegerDivide(&ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(11)
{
    // Test abstract signed integer facilities.
    
    // Test default constructor
    // Test copy constructor
    // Test move constructor
    // Test assignment operator
    // Test move-assignment operator
    // Test move-assignment operator.
    // Test 'reset'
    // Test 'move'

    // Test 'parse/generate'
    // Test 'equals'
    // Test 'less'
    // Test 'hash'
    // Test 'print'

    ntccfg::TestAllocator ta;
    {
        // Test parse/generate.

        // MRM
        #if 0
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
        #endif
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);   // Test prerequisites

    NTCCFG_TEST_REGISTER(2);   // Test abstract unsigned integer addition
    NTCCFG_TEST_REGISTER(3);   // Test abstract unsigned integer subtraction
    NTCCFG_TEST_REGISTER(4);   // Test abstract unsigned integer multiplication
    NTCCFG_TEST_REGISTER(5);   // Test abstract unsigned integer division
    NTCCFG_TEST_REGISTER(6);   // Test abstract unsigned integer facilities

    NTCCFG_TEST_REGISTER(7);   // Test abstract signed integer addition
    NTCCFG_TEST_REGISTER(8);   // Test abstract signed integer subtraction
    NTCCFG_TEST_REGISTER(9);   // Test abstract signed integer multiplication
    NTCCFG_TEST_REGISTER(10);  // Test abstract signed integer division
    NTCCFG_TEST_REGISTER(11);  // Test abstract signed integer facilities
}
NTCCFG_TEST_DRIVER_END;
