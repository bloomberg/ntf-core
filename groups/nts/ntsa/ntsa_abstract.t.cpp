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
BSLS_IDENT_RCSID(ntscfg_abstract_t_cpp, "$Id$ $CSID$")

#include <ntsa_abstract.h>

#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_memoutstreambuf.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_climits.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntscfg::Abstract'.
class AbstractTest
{
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
        bsl::size_t  variationIndex;
        bsl::size_t  variationCount;
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

    struct HardwareIntegerEncodingData {
        HardwareIntegerEncodingData(const bsl::uint8_t* data,
                    bsl::size_t         size,
                    bsl::int64_t        value)
        : d_encoding(data, data + size)
        , d_value(value)
        {
        }

        bsl::vector<bsl::uint8_t> d_encoding;
        bsl::int64_t              d_value;
    };


    struct SoftwareIntegerEncodingData {
        SoftwareIntegerEncodingData(const bsl::uint8_t* data,
                    bsl::size_t         size,
                    bsl::string         value)
        : d_encoding(data, data + size)
        , d_value(value)
        {
        }

        bsl::vector<bsl::uint8_t> d_encoding;
        bsl::string               d_value;
    };

  private:
    static void loadUnsignedIntegerData(UnsignedIntegerDataVector* result);
    static void loadSignedIntegerData(SignedIntegerDataVector* result);

    static void verifyNativeMath();
    static void verifyBase();
    static void verifyRepresentation();

    static void verifyDecoderUtilityTag();
    static void verifyDecoderUtilityLength();
    static void verifyDecoderUtilityInteger();
    static void verifyDecoderUtilityDatetime();

    static void verifyEncoderUtilityTag();
    static void verifyEncoderUtilityLength();
    static void verifyEncoderUtilityInteger();
    static void verifyEncoderUtilityDatetime();

  public:
    // Verify the fundamental mechanisms used to build abstract integers.
    static void verifyPrerequisites();

    static void verifyUnsignedIntegerAdd();
    static void verifyUnsignedIntegerSubtract();
    static void verifyUnsignedIntegerMultiply();
    static void verifyUnsignedIntegerDivide();
    static void verifyUnsignedIntegerFacilities();

    static void verifySignedIntegerAdd();
    static void verifySignedIntegerSubtract();
    static void verifySignedIntegerMultiply();
    static void verifySignedIntegerDivide();
    static void verifySignedIntegerFacilities();

    static void verifyDecoderUtility();
    static void verifyEncoderUtility();

    static void verifyHardwareIntegerCodec();
    static void verifySoftwareIntegerCodec();

    static void verifyObjectIdentifier();
    static void verifyBitString();
};

// #define NTSA_ABSTRACT_TEST_VARIATION 1

#define NTSA_ABSTRACT_INTEGER_LOG_INTEGRAL_OPERATION(operation,               \
                                                     data,                    \
                                                     expected,                \
                                                     found)                   \
    do {                                                                      \
        NTSCFG_TEST_LOG_DEBUG                                                 \
            << "Testing " << (operation) << " operation "                     \
            << (data).variationIndex << "/" << (data).variationCount << ":"   \
            << "\nLHS:  " << (data).lhs << "\nRHS:  " << (data).rhs           \
            << "\nE:    " << (expected) << "\nF:    " << (found)              \
            << NTSCFG_TEST_LOG_END;                                           \
    } while (false)

#define NTSA_ABSTRACT_INTEGER_LOG_ADD(data, expected, found)                  \
    NTSA_ABSTRACT_INTEGER_LOG_INTEGRAL_OPERATION("add", data, expected, found)

#define NTSA_ABSTRACT_INTEGER_LOG_SUBTRACT(data, expected, found)             \
    NTSA_ABSTRACT_INTEGER_LOG_INTEGRAL_OPERATION("subtract",                  \
                                                 data,                        \
                                                 expected,                    \
                                                 found)

#define NTSA_ABSTRACT_INTEGER_LOG_MULTIPLY(data, expected, found)             \
    NTSA_ABSTRACT_INTEGER_LOG_INTEGRAL_OPERATION("multiply",                  \
                                                 data,                        \
                                                 expected,                    \
                                                 found)

#define NTSA_ABSTRACT_INTEGER_LOG_DIVIDE(data, eq, fq, er, fr)                \
    do {                                                                      \
        NTSCFG_TEST_LOG_DEBUG                                                 \
            << "Testing divide operation " << (data).variationIndex << "/"    \
            << (data).variationCount << ":"                                   \
            << "\nLHS:  " << (data).lhs << "\nRHS:  " << (data).rhs           \
            << "\nEQ:   " << (eq) << "\nFQ:   " << (fq) << "\nER:   " << (er) \
            << "\nFR:   " << (fr) << NTSCFG_TEST_LOG_END;                     \
    } while (false)

#define NTSA_ABSTRACT_TEST_LOG_ENCODING_MISMATCH(value, expected, found)      \
    do {                                                                      \
        bsl::stringstream ess;                                                \
        ess << bdlb::PrintStringSingleLineHexDumper(                          \
            reinterpret_cast<const char*>(&(expected).front()),               \
            static_cast<int>((expected).size()));                             \
                                                                              \
        bsl::stringstream fss;                                                \
        fss << bdlb::PrintStringSingleLineHexDumper(                          \
            reinterpret_cast<const char*>(&(found).front()),                  \
            static_cast<int>((found).size()));                                \
                                                                              \
        NTSCFG_TEST_LOG_DEBUG << "Mismatched encoding for " << (value)        \
                              << "\nE: " << ess.str() << "\nF: " << fss.str() \
                              << NTSCFG_TEST_LOG_END;                         \
    } while (false)




void AbstractTest::loadUnsignedIntegerData(UnsignedIntegerDataVector* result)
{
    result->clear();

    typedef bsl::vector<bsl::int64_t> DiscontinuityVector;
    typedef bsl::vector<bsl::int64_t> AdjustmentVector;

    DiscontinuityVector discontinuity;
    AdjustmentVector    adjustment;

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

void AbstractTest::loadSignedIntegerData(SignedIntegerDataVector* result)
{
    result->clear();

    typedef bsl::vector<bsl::int64_t> DiscontinuityVector;
    typedef bsl::vector<bsl::int64_t> AdjustmentVector;

    DiscontinuityVector discontinuity;
    AdjustmentVector    adjustment;

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

void AbstractTest::verifyNativeMath()
{
    {
        bsl::uint64_t a = 1;
        bsl::uint64_t b = 10;

        bsl::uint64_t q = a / b;
        bsl::uint64_t r = a % b;

        NTSCFG_TEST_EQ(q, 0);
        NTSCFG_TEST_EQ(r, a);
    }

    {
        bsl::int64_t a = 0;
        bsl::int64_t b = -1;

        bsl::int64_t q = a / b;
        bsl::int64_t r = a % b;

        NTSCFG_TEST_EQ(q, 0);
        NTSCFG_TEST_EQ(r, 0);
    }
}

void AbstractTest::verifyBase()
{
    {
        bsl::uint64_t radix = ntsa::AbstractIntegerBase::radix(
            ntsa::AbstractIntegerBase::e_NATIVE);

        bsl::uint64_t expected =
            1ULL << (sizeof(ntsa::AbstractIntegerRepresentation::Block) * 8);

        NTSCFG_TEST_EQ(radix, expected);
    }

    {
        bsl::uint64_t radix = ntsa::AbstractIntegerBase::radix(
            ntsa::AbstractIntegerBase::e_BINARY);
        NTSCFG_TEST_EQ(radix, 2);
    }

    {
        bsl::uint64_t radix = ntsa::AbstractIntegerBase::radix(
            ntsa::AbstractIntegerBase::e_OCTAL);
        NTSCFG_TEST_EQ(radix, 8);
    }

    {
        bsl::uint64_t radix = ntsa::AbstractIntegerBase::radix(
            ntsa::AbstractIntegerBase::e_DECIMAL);
        NTSCFG_TEST_EQ(radix, 10);
    }

    {
        bsl::uint64_t radix = ntsa::AbstractIntegerBase::radix(
            ntsa::AbstractIntegerBase::e_HEXADECIMAL);
        NTSCFG_TEST_EQ(radix, 16);
    }
}

void AbstractTest::verifyRepresentation()
{
    typedef bsl::vector<ntsa::AbstractIntegerBase::Value> BaseVector;
    BaseVector                                            baseVector;

    baseVector.push_back(ntsa::AbstractIntegerBase::e_NATIVE);
    baseVector.push_back(ntsa::AbstractIntegerBase::e_BINARY);
    baseVector.push_back(ntsa::AbstractIntegerBase::e_OCTAL);
    baseVector.push_back(ntsa::AbstractIntegerBase::e_DECIMAL);
    baseVector.push_back(ntsa::AbstractIntegerBase::e_HEXADECIMAL);

    for (bsl::size_t i = 0; i < baseVector.size(); ++i) {
        const ntsa::AbstractIntegerBase::Value base = baseVector[i];

        ntsa::AbstractIntegerRepresentation rep(base, NTSCFG_TEST_ALLOCATOR);
        NTSCFG_TEST_EQ(rep.allocator(), NTSCFG_TEST_ALLOCATOR);

        NTSCFG_TEST_EQ(rep.size(), 0);
        NTSCFG_TEST_EQ(rep.base(), base);

        NTSCFG_TEST_LOG_DEBUG << "Rep = " << rep << NTSCFG_TEST_LOG_END;
        rep.assign(123);
        NTSCFG_TEST_LOG_DEBUG << "Rep = " << rep << NTSCFG_TEST_LOG_END;
    }
}

void AbstractTest::verifyDecoderUtilityTag()
{

}

void AbstractTest::verifyDecoderUtilityLength()
{

}

void AbstractTest::verifyDecoderUtilityInteger()
{

}

void AbstractTest::verifyDecoderUtilityDatetime()
{
    ntsa::Error error;

    // UTC TIME

    // 2404152136Z
    // 240415213642Z
    // 2404152136-0430
    // 2404152136+0430
    // 240415213642-0430
    // 240415213642+0430

    {
        const char k_DATA[] = "2404152136Z";

        bdlt::DatetimeTz result;
        error = ntsa::AbstractSyntaxDecoderUtil::decodeDatetimeTz(
            &result,
            ntsa::AbstractSyntaxTagClass::e_UNIVERSAL,
            ntsa::AbstractSyntaxTagType::e_PRIMITIVE,
            ntsa::AbstractSyntaxTagNumber::e_UTC_TIME,
            k_DATA,
            sizeof k_DATA - 1);

        NTSCFG_TEST_OK(error);

        bdlt::Datetime local  = result.localDatetime();
        int            offset = result.offset();

        NTSCFG_TEST_EQ(local.year(), 2024);
        NTSCFG_TEST_EQ(local.month(), 4);
        NTSCFG_TEST_EQ(local.day(), 15);

        NTSCFG_TEST_EQ(local.hour(), 21);
        NTSCFG_TEST_EQ(local.minute(), 36);
        NTSCFG_TEST_EQ(local.second(), 0);

        NTSCFG_TEST_EQ(offset, 0);
    }

    {
        const char k_DATA[] = "240415213642Z";

        bdlt::DatetimeTz result;
        error = ntsa::AbstractSyntaxDecoderUtil::decodeDatetimeTz(
            &result,
            ntsa::AbstractSyntaxTagClass::e_UNIVERSAL,
            ntsa::AbstractSyntaxTagType::e_PRIMITIVE,
            ntsa::AbstractSyntaxTagNumber::e_UTC_TIME,
            k_DATA,
            sizeof k_DATA - 1);

        NTSCFG_TEST_OK(error);

        bdlt::Datetime local  = result.localDatetime();
        int            offset = result.offset();

        NTSCFG_TEST_EQ(local.year(), 2024);
        NTSCFG_TEST_EQ(local.month(), 4);
        NTSCFG_TEST_EQ(local.day(), 15);

        NTSCFG_TEST_EQ(local.hour(), 21);
        NTSCFG_TEST_EQ(local.minute(), 36);
        NTSCFG_TEST_EQ(local.second(), 42);

        NTSCFG_TEST_EQ(offset, 0);
    }

    {
        const char k_DATA[] = "2404152136-0430";

        bdlt::DatetimeTz result;
        error = ntsa::AbstractSyntaxDecoderUtil::decodeDatetimeTz(
            &result,
            ntsa::AbstractSyntaxTagClass::e_UNIVERSAL,
            ntsa::AbstractSyntaxTagType::e_PRIMITIVE,
            ntsa::AbstractSyntaxTagNumber::e_UTC_TIME,
            k_DATA,
            sizeof k_DATA - 1);

        NTSCFG_TEST_OK(error);

        bdlt::Datetime local  = result.localDatetime();
        int            offset = result.offset();

        NTSCFG_TEST_EQ(local.year(), 2024);
        NTSCFG_TEST_EQ(local.month(), 4);
        NTSCFG_TEST_EQ(local.day(), 15);

        NTSCFG_TEST_EQ(local.hour(), 21);
        NTSCFG_TEST_EQ(local.minute(), 36);
        NTSCFG_TEST_EQ(local.second(), 0);

        NTSCFG_TEST_EQ(offset, -270);
    }

    {
        const char k_DATA[] = "2404152136+0430";

        bdlt::DatetimeTz result;
        error = ntsa::AbstractSyntaxDecoderUtil::decodeDatetimeTz(
            &result,
            ntsa::AbstractSyntaxTagClass::e_UNIVERSAL,
            ntsa::AbstractSyntaxTagType::e_PRIMITIVE,
            ntsa::AbstractSyntaxTagNumber::e_UTC_TIME,
            k_DATA,
            sizeof k_DATA - 1);

        NTSCFG_TEST_OK(error);

        bdlt::Datetime local  = result.localDatetime();
        int            offset = result.offset();

        NTSCFG_TEST_EQ(local.year(), 2024);
        NTSCFG_TEST_EQ(local.month(), 4);
        NTSCFG_TEST_EQ(local.day(), 15);

        NTSCFG_TEST_EQ(local.hour(), 21);
        NTSCFG_TEST_EQ(local.minute(), 36);
        NTSCFG_TEST_EQ(local.second(), 0);

        NTSCFG_TEST_EQ(offset, 270);
    }

    {
        const char k_DATA[] = "240415213642-0430";

        bdlt::DatetimeTz result;
        error = ntsa::AbstractSyntaxDecoderUtil::decodeDatetimeTz(
            &result,
            ntsa::AbstractSyntaxTagClass::e_UNIVERSAL,
            ntsa::AbstractSyntaxTagType::e_PRIMITIVE,
            ntsa::AbstractSyntaxTagNumber::e_UTC_TIME,
            k_DATA,
            sizeof k_DATA - 1);

        NTSCFG_TEST_OK(error);

        bdlt::Datetime local  = result.localDatetime();
        int            offset = result.offset();

        NTSCFG_TEST_EQ(local.year(), 2024);
        NTSCFG_TEST_EQ(local.month(), 4);
        NTSCFG_TEST_EQ(local.day(), 15);

        NTSCFG_TEST_EQ(local.hour(), 21);
        NTSCFG_TEST_EQ(local.minute(), 36);
        NTSCFG_TEST_EQ(local.second(), 42);

        NTSCFG_TEST_EQ(offset, -270);
    }

    {
        const char k_DATA[] = "240415213642+0430";

        bdlt::DatetimeTz result;
        error = ntsa::AbstractSyntaxDecoderUtil::decodeDatetimeTz(
            &result,
            ntsa::AbstractSyntaxTagClass::e_UNIVERSAL,
            ntsa::AbstractSyntaxTagType::e_PRIMITIVE,
            ntsa::AbstractSyntaxTagNumber::e_UTC_TIME,
            k_DATA,
            sizeof k_DATA - 1);

        NTSCFG_TEST_OK(error);

        bdlt::Datetime local  = result.localDatetime();
        int            offset = result.offset();

        NTSCFG_TEST_EQ(local.year(), 2024);
        NTSCFG_TEST_EQ(local.month(), 4);
        NTSCFG_TEST_EQ(local.day(), 15);

        NTSCFG_TEST_EQ(local.hour(), 21);
        NTSCFG_TEST_EQ(local.minute(), 36);
        NTSCFG_TEST_EQ(local.second(), 42);

        NTSCFG_TEST_EQ(offset, 270);
    }

    // GENERALIZED TIME:
    //     1. YYYYMMDDHH[MM[SS[.fff]]]
    //     2. YYYYMMDDHH[MM[SS[.fff]]]Z
    //     3. YYYYMMDDHH[MM[SS[.fff]]]{+,-}hhmm

    // 19960415203000.0-0600

    {
        const char k_DATA[] = "2024"
                              "04"
                              "15"
                              "21"
                              "36"
                              "42"
                              ".0"
                              "-0430";

        bdlt::DatetimeTz result;
        error = ntsa::AbstractSyntaxDecoderUtil::decodeDatetimeTz(
            &result,
            ntsa::AbstractSyntaxTagClass::e_UNIVERSAL,
            ntsa::AbstractSyntaxTagType::e_PRIMITIVE,
            ntsa::AbstractSyntaxTagNumber::e_GENERALIZED_TIME,
            k_DATA,
            sizeof k_DATA - 1);

        NTSCFG_TEST_OK(error);

        bdlt::Datetime local  = result.localDatetime();
        int            offset = result.offset();

        NTSCFG_TEST_EQ(local.year(), 2024);
        NTSCFG_TEST_EQ(local.month(), 4);
        NTSCFG_TEST_EQ(local.day(), 15);

        NTSCFG_TEST_EQ(local.hour(), 21);
        NTSCFG_TEST_EQ(local.minute(), 36);
        NTSCFG_TEST_EQ(local.second(), 42);
        NTSCFG_TEST_EQ(local.millisecond(), 0);

        NTSCFG_TEST_EQ(offset, -270);
    }

    {
        const char k_DATA[] = "2024"
                              "04"
                              "15"
                              "21"
                              "36"
                              "42"
                              ".1"
                              "-0430";

        bdlt::DatetimeTz result;
        error = ntsa::AbstractSyntaxDecoderUtil::decodeDatetimeTz(
            &result,
            ntsa::AbstractSyntaxTagClass::e_UNIVERSAL,
            ntsa::AbstractSyntaxTagType::e_PRIMITIVE,
            ntsa::AbstractSyntaxTagNumber::e_GENERALIZED_TIME,
            k_DATA,
            sizeof k_DATA - 1);

        NTSCFG_TEST_OK(error);

        bdlt::Datetime local  = result.localDatetime();
        int            offset = result.offset();

        NTSCFG_TEST_EQ(local.year(), 2024);
        NTSCFG_TEST_EQ(local.month(), 4);
        NTSCFG_TEST_EQ(local.day(), 15);

        NTSCFG_TEST_EQ(local.hour(), 21);
        NTSCFG_TEST_EQ(local.minute(), 36);
        NTSCFG_TEST_EQ(local.second(), 42);
        NTSCFG_TEST_EQ(local.millisecond(), 1);

        NTSCFG_TEST_EQ(offset, -270);
    }

    {
        const char k_DATA[] = "2024"
                              "04"
                              "15"
                              "21"
                              "36"
                              "42"
                              ".12"
                              "-0430";

        bdlt::DatetimeTz result;
        error = ntsa::AbstractSyntaxDecoderUtil::decodeDatetimeTz(
            &result,
            ntsa::AbstractSyntaxTagClass::e_UNIVERSAL,
            ntsa::AbstractSyntaxTagType::e_PRIMITIVE,
            ntsa::AbstractSyntaxTagNumber::e_GENERALIZED_TIME,
            k_DATA,
            sizeof k_DATA - 1);

        NTSCFG_TEST_OK(error);

        bdlt::Datetime local  = result.localDatetime();
        int            offset = result.offset();

        NTSCFG_TEST_EQ(local.year(), 2024);
        NTSCFG_TEST_EQ(local.month(), 4);
        NTSCFG_TEST_EQ(local.day(), 15);

        NTSCFG_TEST_EQ(local.hour(), 21);
        NTSCFG_TEST_EQ(local.minute(), 36);
        NTSCFG_TEST_EQ(local.second(), 42);
        NTSCFG_TEST_EQ(local.millisecond(), 12);

        NTSCFG_TEST_EQ(offset, -270);
    }

    {
        const char k_DATA[] = "2024"
                              "04"
                              "15"
                              "21"
                              "36"
                              "42"
                              ".123"
                              "-0430";

        bdlt::DatetimeTz result;
        error = ntsa::AbstractSyntaxDecoderUtil::decodeDatetimeTz(
            &result,
            ntsa::AbstractSyntaxTagClass::e_UNIVERSAL,
            ntsa::AbstractSyntaxTagType::e_PRIMITIVE,
            ntsa::AbstractSyntaxTagNumber::e_GENERALIZED_TIME,
            k_DATA,
            sizeof k_DATA - 1);

        NTSCFG_TEST_OK(error);

        bdlt::Datetime local  = result.localDatetime();
        int            offset = result.offset();

        NTSCFG_TEST_EQ(local.year(), 2024);
        NTSCFG_TEST_EQ(local.month(), 4);
        NTSCFG_TEST_EQ(local.day(), 15);

        NTSCFG_TEST_EQ(local.hour(), 21);
        NTSCFG_TEST_EQ(local.minute(), 36);
        NTSCFG_TEST_EQ(local.second(), 42);
        NTSCFG_TEST_EQ(local.millisecond(), 123);

        NTSCFG_TEST_EQ(offset, -270);
    }
}

void AbstractTest::verifyEncoderUtilityTag()
{
}

void AbstractTest::verifyEncoderUtilityLength()
{
}

void AbstractTest::verifyEncoderUtilityInteger()
{
}

void AbstractTest::verifyEncoderUtilityDatetime()
{
}

NTSCFG_TEST_FUNCTION(ntsa::AbstractTest::verifyPrerequisites)
{
    verifyNativeMath();
    verifyBase();
    verifyRepresentation();
}

NTSCFG_TEST_FUNCTION(ntsa::AbstractTest::verifyUnsignedIntegerAdd)
{
    UnsignedIntegerDataVector dataVector(NTSCFG_TEST_ALLOCATOR);
    loadUnsignedIntegerData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        UnsignedIntegerData data = dataVector[i];

#if defined(NTSA_ABSTRACT_TEST_VARIATION)
        if (data.variationIndex != NTSA_ABSTRACT_TEST_VARIATION) {
            continue;
        }
#endif

        ntsa::AbstractIntegerQuantity lhs(data.lhs, NTSCFG_TEST_ALLOCATOR);
        ntsa::AbstractIntegerQuantity rhs(data.rhs, NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractIntegerQuantity sum(NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractIntegerQuantityUtil::add(&sum, lhs, rhs);

        bsl::string expected = bsl::to_string(data.lhs + data.rhs);

        bsl::string found;
        sum.generate(&found,
                     ntsa::AbstractIntegerSign::e_POSITIVE,
                     ntsa::AbstractIntegerBase::e_DECIMAL);

        NTSA_ABSTRACT_INTEGER_LOG_ADD(data, expected, found);

        NTSCFG_TEST_EQ(found, expected);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::AbstractTest::verifyUnsignedIntegerSubtract)
{
    UnsignedIntegerDataVector dataVector(NTSCFG_TEST_ALLOCATOR);
    loadUnsignedIntegerData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        UnsignedIntegerData data = dataVector[i];

#if defined(NTSA_ABSTRACT_TEST_VARIATION)
        if (data.variationIndex != NTSA_ABSTRACT_TEST_VARIATION) {
            continue;
        }
#endif

        ntsa::AbstractIntegerQuantity lhs(data.lhs, NTSCFG_TEST_ALLOCATOR);
        ntsa::AbstractIntegerQuantity rhs(data.rhs, NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractIntegerQuantity difference(NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractIntegerQuantityUtil::subtract(&difference, lhs, rhs);

        bsl::string expected;
        if (data.lhs < data.rhs) {
            expected = "0";
        }
        else {
            expected = bsl::to_string(data.lhs - data.rhs);
        }

        bsl::string found;
        difference.generate(&found,
                            ntsa::AbstractIntegerSign::e_POSITIVE,
                            ntsa::AbstractIntegerBase::e_DECIMAL);

        NTSA_ABSTRACT_INTEGER_LOG_SUBTRACT(data, expected, found);

        NTSCFG_TEST_EQ(found, expected);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::AbstractTest::verifyUnsignedIntegerMultiply)
{
    UnsignedIntegerDataVector dataVector(NTSCFG_TEST_ALLOCATOR);
    loadUnsignedIntegerData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        UnsignedIntegerData data = dataVector[i];

#if defined(NTSA_ABSTRACT_TEST_VARIATION)
        if (data.variationIndex != NTSA_ABSTRACT_TEST_VARIATION) {
            continue;
        }
#endif

        // Skip variations whose products overflow 64-bit unsigned integers.

        if (data.multiplicationWouldOverflow()) {
            continue;
        }

        ntsa::AbstractIntegerQuantity lhs(data.lhs, NTSCFG_TEST_ALLOCATOR);
        ntsa::AbstractIntegerQuantity rhs(data.rhs, NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractIntegerQuantity product(NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractIntegerQuantityUtil::multiply(&product, lhs, rhs);

        bsl::string expected = bsl::to_string(data.lhs * data.rhs);

        bsl::string found;
        product.generate(&found,
                         ntsa::AbstractIntegerSign::e_POSITIVE,
                         ntsa::AbstractIntegerBase::e_DECIMAL);

        NTSA_ABSTRACT_INTEGER_LOG_MULTIPLY(data, expected, found);

        NTSCFG_TEST_EQ(found, expected);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::AbstractTest::verifyUnsignedIntegerDivide)
{
    UnsignedIntegerDataVector dataVector(NTSCFG_TEST_ALLOCATOR);
    loadUnsignedIntegerData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        UnsignedIntegerData data = dataVector[i];

#if defined(NTSA_ABSTRACT_TEST_VARIATION)
        if (data.variationIndex != NTSA_ABSTRACT_TEST_VARIATION) {
            continue;
        }
#endif

        if (data.rhs == 0) {
            continue;
        }

        ntsa::AbstractIntegerQuantity lhs(data.lhs, NTSCFG_TEST_ALLOCATOR);
        ntsa::AbstractIntegerQuantity rhs(data.rhs, NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractIntegerQuantity q(NTSCFG_TEST_ALLOCATOR);
        ntsa::AbstractIntegerQuantity r(NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractIntegerQuantityUtil::divide(&q, &r, lhs, rhs);

        bsl::string eq = bsl::to_string(data.lhs / data.rhs);
        bsl::string er = bsl::to_string(data.lhs % data.rhs);

        bsl::string fq;
        q.generate(&fq,
                   ntsa::AbstractIntegerSign::e_POSITIVE,
                   ntsa::AbstractIntegerBase::e_DECIMAL);

        bsl::string fr;
        r.generate(&fr,
                   ntsa::AbstractIntegerSign::e_POSITIVE,
                   ntsa::AbstractIntegerBase::e_DECIMAL);

        NTSA_ABSTRACT_INTEGER_LOG_DIVIDE(data, eq, fq, er, fr);

        NTSCFG_TEST_EQ(fq, eq);
        NTSCFG_TEST_EQ(fr, er);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::AbstractTest::verifyUnsignedIntegerFacilities)
{

}

NTSCFG_TEST_FUNCTION(ntsa::AbstractTest::verifySignedIntegerAdd)
{
    SignedIntegerDataVector dataVector(NTSCFG_TEST_ALLOCATOR);
    loadSignedIntegerData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        SignedIntegerData data = dataVector[i];

#if defined(NTSA_ABSTRACT_TEST_VARIATION)
        if (data.variationIndex != NTSA_ABSTRACT_TEST_VARIATION) {
            continue;
        }
#endif

        ntsa::AbstractInteger lhs(data.lhs, NTSCFG_TEST_ALLOCATOR);
        ntsa::AbstractInteger rhs(data.rhs, NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractInteger sum(NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractIntegerUtil::add(&sum, lhs, rhs);

        bsl::string expected = bsl::to_string(data.lhs + data.rhs);

        bsl::string found;
        sum.generate(&found, ntsa::AbstractIntegerBase::e_DECIMAL);

        NTSA_ABSTRACT_INTEGER_LOG_ADD(data, expected, found);

        NTSCFG_TEST_EQ(found, expected);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::AbstractTest::verifySignedIntegerSubtract)
{
    SignedIntegerDataVector dataVector(NTSCFG_TEST_ALLOCATOR);
    loadSignedIntegerData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        SignedIntegerData data = dataVector[i];

#if defined(NTSA_ABSTRACT_TEST_VARIATION)
        if (data.variationIndex != NTSA_ABSTRACT_TEST_VARIATION) {
            continue;
        }
#endif

        ntsa::AbstractInteger lhs(data.lhs, NTSCFG_TEST_ALLOCATOR);
        ntsa::AbstractInteger rhs(data.rhs, NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractInteger difference(NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractIntegerUtil::subtract(&difference, lhs, rhs);

        bsl::string expected = bsl::to_string(data.lhs - data.rhs);

        bsl::string found;
        difference.generate(&found, ntsa::AbstractIntegerBase::e_DECIMAL);

        NTSA_ABSTRACT_INTEGER_LOG_SUBTRACT(data, expected, found);

        NTSCFG_TEST_EQ(found, expected);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::AbstractTest::verifySignedIntegerMultiply)
{
    SignedIntegerDataVector dataVector(NTSCFG_TEST_ALLOCATOR);
    loadSignedIntegerData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        SignedIntegerData data = dataVector[i];

#if defined(NTSA_ABSTRACT_TEST_VARIATION)
        if (data.variationIndex != NTSA_ABSTRACT_TEST_VARIATION) {
            continue;
        }
#endif

        // Skip variations whose products overflow 64-bit signed integers.

        if (data.multiplicationWouldOverflow()) {
            continue;
        }

        ntsa::AbstractInteger lhs(data.lhs, NTSCFG_TEST_ALLOCATOR);
        ntsa::AbstractInteger rhs(data.rhs, NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractInteger product(NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractIntegerUtil::multiply(&product, lhs, rhs);

        bsl::string expected = bsl::to_string(data.lhs * data.rhs);

        bsl::string found;
        product.generate(&found, ntsa::AbstractIntegerBase::e_DECIMAL);

        NTSA_ABSTRACT_INTEGER_LOG_MULTIPLY(data, expected, found);

        NTSCFG_TEST_EQ(found, expected);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::AbstractTest::verifySignedIntegerDivide)
{
    SignedIntegerDataVector dataVector(NTSCFG_TEST_ALLOCATOR);
    loadSignedIntegerData(&dataVector);

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        SignedIntegerData data = dataVector[i];

#if defined(NTSA_ABSTRACT_TEST_VARIATION)
        if (data.variationIndex != NTSA_ABSTRACT_TEST_VARIATION) {
            continue;
        }
#endif

        if (data.rhs == 0) {
            continue;
        }

        ntsa::AbstractInteger lhs(data.lhs, NTSCFG_TEST_ALLOCATOR);
        ntsa::AbstractInteger rhs(data.rhs, NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractInteger q(NTSCFG_TEST_ALLOCATOR);
        ntsa::AbstractInteger r(NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractIntegerUtil::divide(&q, &r, lhs, rhs);

        bsl::string eq = bsl::to_string(data.lhs / data.rhs);
        bsl::string er = bsl::to_string(data.lhs % data.rhs);

        bsl::string fq;
        q.generate(&fq, ntsa::AbstractIntegerBase::e_DECIMAL);

        bsl::string fr;
        r.generate(&fr, ntsa::AbstractIntegerBase::e_DECIMAL);

        NTSA_ABSTRACT_INTEGER_LOG_DIVIDE(data, eq, fq, er, fr);

        NTSCFG_TEST_EQ(fq, eq);
        NTSCFG_TEST_EQ(fr, er);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::AbstractTest::verifySignedIntegerFacilities)
{
    // Test parse/generate.

    {
        ntsa::AbstractInteger number(NTSCFG_TEST_ALLOCATOR);
        bool                  valid = number.parse("");
        NTSCFG_TEST_FALSE(valid);
    }

    {
        ntsa::AbstractInteger number(NTSCFG_TEST_ALLOCATOR);
        bool                  valid = number.parse("+");
        NTSCFG_TEST_FALSE(valid);
    }

    {
        ntsa::AbstractInteger number(NTSCFG_TEST_ALLOCATOR);
        bool                  valid = number.parse("-");
        NTSCFG_TEST_FALSE(valid);
    }

    {
        ntsa::AbstractInteger number(NTSCFG_TEST_ALLOCATOR);
        bool                  valid = number.parse("0x");
        NTSCFG_TEST_FALSE(valid);
    }

    {
        ntsa::AbstractInteger number(NTSCFG_TEST_ALLOCATOR);
        bool                  valid = number.parse("+0x");
        NTSCFG_TEST_FALSE(valid);
    }

    {
        ntsa::AbstractInteger number(NTSCFG_TEST_ALLOCATOR);
        bool                  valid = number.parse("-0x");
        NTSCFG_TEST_FALSE(valid);
    }

    {
        bsl::string input = "0";

        ntsa::AbstractInteger number(NTSCFG_TEST_ALLOCATOR);
        bool                  valid = number.parse(input);
        NTSCFG_TEST_TRUE(valid);

        bsl::string output;
        number.generate(&output, ntsa::AbstractIntegerBase::e_DECIMAL);

        NTSCFG_TEST_EQ(output, input);
    }

    {
        bsl::string input = "123";

        ntsa::AbstractInteger number(NTSCFG_TEST_ALLOCATOR);
        bool                  valid = number.parse(input);
        NTSCFG_TEST_TRUE(valid);

        bsl::string output;
        number.generate(&output, ntsa::AbstractIntegerBase::e_DECIMAL);

        NTSCFG_TEST_EQ(output, input);
    }

    {
        bsl::string input = "-123";

        ntsa::AbstractInteger number(NTSCFG_TEST_ALLOCATOR);
        bool                  valid = number.parse(input);
        NTSCFG_TEST_TRUE(valid);

        bsl::string output;
        number.generate(&output, ntsa::AbstractIntegerBase::e_DECIMAL);

        NTSCFG_TEST_EQ(output, input);
    }
}


NTSCFG_TEST_FUNCTION(ntsa::AbstractTest::verifyDecoderUtility)
{
    verifyDecoderUtilityTag();
    verifyDecoderUtilityLength();
    verifyDecoderUtilityInteger();
    verifyDecoderUtilityDatetime();
}

NTSCFG_TEST_FUNCTION(ntsa::AbstractTest::verifyEncoderUtility)
{
    verifyEncoderUtilityTag();
    verifyEncoderUtilityLength();
    verifyEncoderUtilityInteger();
    verifyEncoderUtilityDatetime();
}

NTSCFG_TEST_FUNCTION(ntsa::AbstractTest::verifyHardwareIntegerCodec)
{
    ntsa::Error error;

    typedef HardwareIntegerEncodingData EncodingData;
    bsl::vector<EncodingData> dataVector;

    {
        // CONSTRUCTED { INTEGER(0) }
        const bsl::uint8_t k_DER[] = {0x30, 0x03, 0x02, 0x01, 0x00};

        dataVector.push_back(EncodingData(k_DER, sizeof k_DER, 0));
    }

    {
        // CONSTRUCTED { INTEGER(1) }
        const bsl::uint8_t k_DER[] = {0x30, 0x03, 0x02, 0x01, 0x01};

        dataVector.push_back(EncodingData(k_DER, sizeof k_DER, 1));
    }

    {
        // CONSTRUCTED { INTEGER(-1) }
        const bsl::uint8_t k_DER[] = {0x30, 0x03, 0x02, 0x01, 0xff};

        dataVector.push_back(EncodingData(k_DER, sizeof k_DER, -1));
    }

    {
        // CONSTRUCTED { INTEGER(2) }
        const bsl::uint8_t k_DER[] = {0x30, 0x03, 0x02, 0x01, 0x02};

        dataVector.push_back(EncodingData(k_DER, sizeof k_DER, 2));
    }

    {
        // CONSTRUCTED { INTEGER(-2) }
        const bsl::uint8_t k_DER[] = {0x30, 0x03, 0x02, 0x01, 0xfe};

        dataVector.push_back(EncodingData(k_DER, sizeof k_DER, -2));
    }

    {
        // CONSTRUCTED { INTEGER(3) }
        const bsl::uint8_t k_DER[] = {0x30, 0x03, 0x02, 0x01, 0x03};

        dataVector.push_back(EncodingData(k_DER, sizeof k_DER, 3));
    }

    {
        // CONSTRUCTED { INTEGER(-3) }
        const bsl::uint8_t k_DER[] = {0x30, 0x03, 0x02, 0x01, 0xfd};

        dataVector.push_back(EncodingData(k_DER, sizeof k_DER, -3));
    }

    {
        // CONSTRUCTED { INTEGER(11927552) }
        const bsl::uint8_t k_DER[] =
            {0x30, 0x06, 0x02, 0x04, 0x00, 0xb6, 0x00, 0x00};

        dataVector.push_back(EncodingData(k_DER, sizeof k_DER, 11927552));
    }

    {
        // CONSTRUCTED { INTEGER(-11927552) }
        const bsl::uint8_t k_DER[] =
            {0x30, 0x06, 0x02, 0x04, 0xff, 0x4a, 0x00, 0x00};

        dataVector.push_back(EncodingData(k_DER, sizeof k_DER, -11927552));
    }

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        const EncodingData& data = dataVector[i];

        bdlsb::FixedMemInStreamBuf isb(
            reinterpret_cast<const char*>(&data.d_encoding.front()),
            data.d_encoding.size());

        ntsa::AbstractSyntaxDecoder decoder(&isb, NTSCFG_TEST_ALLOCATOR);

        {
            ntsa::AbstractSyntaxDecoderFrame contextOuter;
            error = decoder.decodeTag(&contextOuter);
            NTSCFG_TEST_OK(error);

            NTSCFG_TEST_LOG_DEBUG << "Context = " << contextOuter
                                    << NTSCFG_TEST_LOG_END;

            NTSCFG_TEST_EQ(contextOuter.tagClass(),
                            ntsa::AbstractSyntaxTagClass::e_UNIVERSAL);

            NTSCFG_TEST_EQ(contextOuter.tagType(),
                            ntsa::AbstractSyntaxTagType::e_CONSTRUCTED);

            NTSCFG_TEST_EQ(contextOuter.contentLength().has_value(), true);

            {
                ntsa::AbstractSyntaxDecoderFrame contextInner;
                error = decoder.decodeTag(&contextInner);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_LOG_DEBUG << "Context = " << contextInner
                                        << NTSCFG_TEST_LOG_END;

                NTSCFG_TEST_EQ(contextInner.tagClass(),
                                ntsa::AbstractSyntaxTagClass::e_UNIVERSAL);

                NTSCFG_TEST_EQ(contextInner.tagType(),
                                ntsa::AbstractSyntaxTagType::e_PRIMITIVE);

                NTSCFG_TEST_EQ(contextInner.contentLength().has_value(),
                                true);

                bsl::int32_t value =
                    bsl::numeric_limits<bsl::int32_t>::max();
                error = decoder.decodeValue(&value);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_LOG_DEBUG << "Value = " << value
                                        << NTSCFG_TEST_LOG_END;

                NTSCFG_TEST_EQ(value, data.d_value);

                error = decoder.decodeTagComplete();
                NTSCFG_TEST_OK(error);
            }

            error = decoder.decodeTagComplete();
            NTSCFG_TEST_OK(error);
        }

        bdlsb::MemOutStreamBuf osb(NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractSyntaxEncoder encoder(&osb, NTSCFG_TEST_ALLOCATOR);

        error =
            encoder.encodeTag(ntsa::AbstractSyntaxTagClass::e_UNIVERSAL,
                                ntsa::AbstractSyntaxTagType::e_CONSTRUCTED,
                                ntsa::AbstractSyntaxTagNumber::e_SEQUENCE);
        NTSCFG_TEST_OK(error);

        error =
            encoder.encodeTag(ntsa::AbstractSyntaxTagClass::e_UNIVERSAL,
                                ntsa::AbstractSyntaxTagType::e_PRIMITIVE,
                                ntsa::AbstractSyntaxTagNumber::e_INTEGER);
        NTSCFG_TEST_OK(error);

        error = encoder.encodeValue(data.d_value);
        NTSCFG_TEST_OK(error);

        error = encoder.encodeTagComplete();
        NTSCFG_TEST_OK(error);

        error = encoder.encodeTagComplete();
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_GT(osb.length(), 0);
        NTSCFG_TEST_NE(osb.data(), 0);

        bsl::vector<bsl::uint8_t> encoding(
            reinterpret_cast<const bsl::uint8_t*>(osb.data()),
            reinterpret_cast<const bsl::uint8_t*>(osb.data()) +
                osb.length());

        bool sameEncoding = encoding == data.d_encoding;
        if (!sameEncoding) {
            NTSA_ABSTRACT_TEST_LOG_ENCODING_MISMATCH(data.d_value,
                                                        data.d_encoding,
                                                        encoding);
        }

        NTSCFG_TEST_TRUE(sameEncoding);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::AbstractTest::verifySoftwareIntegerCodec)
{
    ntsa::Error error;

    typedef SoftwareIntegerEncodingData EncodingData;
    bsl::vector<EncodingData> dataVector;

    {
        // CONSTRUCTED { INTEGER(0) }
        const bsl::uint8_t k_DER[] = {0x30, 0x03, 0x02, 0x01, 0x00};

        dataVector.push_back(EncodingData(k_DER, sizeof k_DER, "0"));
    }

    {
        // CONSTRUCTED { INTEGER(1) }
        const bsl::uint8_t k_DER[] = {0x30, 0x03, 0x02, 0x01, 0x01};

        dataVector.push_back(EncodingData(k_DER, sizeof k_DER, "1"));
    }

    {
        // CONSTRUCTED { k_DER(-1) }
        const bsl::uint8_t k_DER[] = {0x30, 0x03, 0x02, 0x01, 0xff};

        dataVector.push_back(EncodingData(k_DER, sizeof k_DER, "-1"));
    }

    {
        // CONSTRUCTED { INTEGER(2) }
        const bsl::uint8_t k_DER[] = {0x30, 0x03, 0x02, 0x01, 0x02};

        dataVector.push_back(EncodingData(k_DER, sizeof k_DER, "2"));
    }

    {
        // CONSTRUCTED { INTEGER(-2) }
        const bsl::uint8_t k_DER[] = {0x30, 0x03, 0x02, 0x01, 0xfe};

        dataVector.push_back(EncodingData(k_DER, sizeof k_DER, "-2"));
    }

    {
        // CONSTRUCTED { INTEGER(3) }
        const bsl::uint8_t k_DER[] = {0x30, 0x03, 0x02, 0x01, 0x03};

        dataVector.push_back(EncodingData(k_DER, sizeof k_DER, "3"));
    }

    {
        // CONSTRUCTED { INTEGER(-3) }
        const bsl::uint8_t k_DER[] = {0x30, 0x03, 0x02, 0x01, 0xfd};

        dataVector.push_back(EncodingData(k_DER, sizeof k_DER, "-3"));
    }

    {
        // CONSTRUCTED { INTEGER(11927552) }
        const bsl::uint8_t k_DER[] =
            {0x30, 0x06, 0x02, 0x04, 0x00, 0xb6, 0x00, 0x00};

        dataVector.push_back(
            EncodingData(k_DER, sizeof k_DER, "11927552"));
    }

    {
        // CONSTRUCTED { INTEGER(-11927552) }
        const bsl::uint8_t k_DER[] =
            {0x30, 0x06, 0x02, 0x04, 0xff, 0x4a, 0x00, 0x00};

        dataVector.push_back(
            EncodingData(k_DER, sizeof k_DER, "-11927552"));
    }

    for (bsl::size_t i = 0; i < dataVector.size(); ++i) {
        const EncodingData& data = dataVector[i];

        bdlsb::FixedMemInStreamBuf isb(
            reinterpret_cast<const char*>(&data.d_encoding.front()),
            data.d_encoding.size());

        ntsa::AbstractSyntaxDecoder decoder(&isb, NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractInteger value(NTSCFG_TEST_ALLOCATOR);

        {
            ntsa::AbstractSyntaxDecoderFrame contextOuter;
            error = decoder.decodeTag(&contextOuter);
            NTSCFG_TEST_OK(error);

            NTSCFG_TEST_LOG_DEBUG << "Context = " << contextOuter
                                    << NTSCFG_TEST_LOG_END;

            NTSCFG_TEST_EQ(contextOuter.tagClass(),
                            ntsa::AbstractSyntaxTagClass::e_UNIVERSAL);

            NTSCFG_TEST_EQ(contextOuter.tagType(),
                            ntsa::AbstractSyntaxTagType::e_CONSTRUCTED);

            NTSCFG_TEST_EQ(contextOuter.contentLength().has_value(), true);

            {
                ntsa::AbstractSyntaxDecoderFrame contextInner;
                error = decoder.decodeTag(&contextInner);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_LOG_DEBUG << "Context = " << contextInner
                                        << NTSCFG_TEST_LOG_END;

                NTSCFG_TEST_EQ(contextInner.tagClass(),
                                ntsa::AbstractSyntaxTagClass::e_UNIVERSAL);

                NTSCFG_TEST_EQ(contextInner.tagType(),
                                ntsa::AbstractSyntaxTagType::e_PRIMITIVE);

                NTSCFG_TEST_EQ(contextInner.contentLength().has_value(),
                                true);

                error = decoder.decodeValue(&value);
                NTSCFG_TEST_OK(error);

                bsl::string valueString;
                value.generate(&valueString,
                                ntsa::AbstractIntegerBase::e_DECIMAL);

                NTSCFG_TEST_LOG_DEBUG << "Value = " << valueString
                                        << NTSCFG_TEST_LOG_END;

                NTSCFG_TEST_EQ(valueString, data.d_value);

                error = decoder.decodeTagComplete();
                NTSCFG_TEST_OK(error);
            }

            error = decoder.decodeTagComplete();
            NTSCFG_TEST_OK(error);
        }

        bdlsb::MemOutStreamBuf osb(NTSCFG_TEST_ALLOCATOR);

        ntsa::AbstractSyntaxEncoder encoder(&osb, NTSCFG_TEST_ALLOCATOR);

        error =
            encoder.encodeTag(ntsa::AbstractSyntaxTagClass::e_UNIVERSAL,
                                ntsa::AbstractSyntaxTagType::e_CONSTRUCTED,
                                ntsa::AbstractSyntaxTagNumber::e_SEQUENCE);
        NTSCFG_TEST_OK(error);

        error =
            encoder.encodeTag(ntsa::AbstractSyntaxTagClass::e_UNIVERSAL,
                                ntsa::AbstractSyntaxTagType::e_PRIMITIVE,
                                ntsa::AbstractSyntaxTagNumber::e_INTEGER);
        NTSCFG_TEST_OK(error);

        error = encoder.encodeValue(value);
        NTSCFG_TEST_OK(error);

        error = encoder.encodeTagComplete();
        NTSCFG_TEST_OK(error);

        error = encoder.encodeTagComplete();
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_GT(osb.length(), 0);
        NTSCFG_TEST_NE(osb.data(), 0);

        bsl::vector<bsl::uint8_t> encoding(
            reinterpret_cast<const bsl::uint8_t*>(osb.data()),
            reinterpret_cast<const bsl::uint8_t*>(osb.data()) +
                osb.length());

        bool sameEncoding = encoding == data.d_encoding;
        if (!sameEncoding) {
            NTSA_ABSTRACT_TEST_LOG_ENCODING_MISMATCH(value,
                                                        data.d_encoding,
                                                        encoding);
        }

        NTSCFG_TEST_TRUE(sameEncoding);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::AbstractTest::verifyObjectIdentifier)
{
    ntsa::Error error;

    const bsl::uint8_t k_ENCODING[] = {
        0x06, 0x21, 0x2a, 0x03, 0x04, 0x81, 0x7e, 0x81, 0x7f,
        0x82, 0x00, 0x83, 0xff, 0x7e, 0x83, 0xff, 0x7f, 0x84,
        0x80, 0x00, 0x8f, 0xff, 0xff, 0xff, 0x7e, 0x8f, 0xff,
        0xff, 0xff, 0x7f, 0x90, 0x80, 0x80, 0x80, 0x00};

    bdlsb::FixedMemInStreamBuf isb(
        reinterpret_cast<const char*>(k_ENCODING),
        sizeof k_ENCODING);

    ntsa::AbstractSyntaxDecoder decoder(&isb, NTSCFG_TEST_ALLOCATOR);

    ntsa::AbstractObjectIdentifier value(NTSCFG_TEST_ALLOCATOR);

    ntsa::AbstractSyntaxDecoderFrame decoderFrame;
    error = decoder.decodeTag(&decoderFrame);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_LOG_DEBUG << "Context = " << decoderFrame
                            << NTSCFG_TEST_LOG_END;

    NTSCFG_TEST_EQ(decoderFrame.tagClass(),
                    ntsa::AbstractSyntaxTagClass::e_UNIVERSAL);

    NTSCFG_TEST_EQ(decoderFrame.tagType(),
                    ntsa::AbstractSyntaxTagType::e_PRIMITIVE);

    NTSCFG_TEST_EQ(decoderFrame.contentLength().has_value(), true);

    error = decoder.decodeValue(&value);
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_LOG_DEBUG << "Value = " << value << NTSCFG_TEST_LOG_END;

    NTSCFG_TEST_EQ(value.size(), 13);

    NTSCFG_TEST_EQ(value.get(0), 1);
    NTSCFG_TEST_EQ(value.get(1), 2);
    NTSCFG_TEST_EQ(value.get(2), 3);
    NTSCFG_TEST_EQ(value.get(3), 4);
    NTSCFG_TEST_EQ(value.get(4), 254);
    NTSCFG_TEST_EQ(value.get(5), 255);
    NTSCFG_TEST_EQ(value.get(6), 256);
    NTSCFG_TEST_EQ(value.get(7), 65534);
    NTSCFG_TEST_EQ(value.get(8), 65535);
    NTSCFG_TEST_EQ(value.get(9), 65536);
    NTSCFG_TEST_EQ(value.get(10), 4294967294);
    NTSCFG_TEST_EQ(value.get(11), 4294967295);
    NTSCFG_TEST_EQ(value.get(12), 4294967296ULL);

    error = decoder.decodeTagComplete();
    NTSCFG_TEST_OK(error);

    bdlsb::MemOutStreamBuf osb(NTSCFG_TEST_ALLOCATOR);

    ntsa::AbstractSyntaxEncoder encoder(&osb, NTSCFG_TEST_ALLOCATOR);

    error = encoder.encodeTag(
        ntsa::AbstractSyntaxTagClass::e_UNIVERSAL,
        ntsa::AbstractSyntaxTagType::e_PRIMITIVE,
        ntsa::AbstractSyntaxTagNumber::e_OBJECT_IDENTIFIER);
    NTSCFG_TEST_OK(error);

    error = encoder.encodeValue(value);
    NTSCFG_TEST_OK(error);

    error = encoder.encodeTagComplete();
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_GT(osb.length(), 0);
    NTSCFG_TEST_NE(osb.data(), 0);

    bsl::vector<bsl::uint8_t> encoding(
        reinterpret_cast<const bsl::uint8_t*>(osb.data()),
        reinterpret_cast<const bsl::uint8_t*>(osb.data()) + osb.length());

    bsl::vector<bsl::uint8_t> expectedEncoding(
        reinterpret_cast<const bsl::uint8_t*>(k_ENCODING),
        reinterpret_cast<const bsl::uint8_t*>(k_ENCODING) +
            sizeof k_ENCODING);

    bool sameEncoding = encoding == expectedEncoding;
    if (!sameEncoding) {
        NTSA_ABSTRACT_TEST_LOG_ENCODING_MISMATCH(value,
                                                    expectedEncoding,
                                                    encoding);
    }

    NTSCFG_TEST_TRUE(sameEncoding);
}

NTSCFG_TEST_FUNCTION(ntsa::AbstractTest::verifyBitString)
{
    ntsa::Error error;

    {
        ntsa::AbstractBitString v(NTSCFG_TEST_ALLOCATOR);

        NTSCFG_TEST_ASSERT(v.numBytes() == 0);
        NTSCFG_TEST_ASSERT(v.numBits() == 0);

        NTSCFG_TEST_LOG_DEBUG << "V = " << v << NTSCFG_TEST_LOG_END;
    }

    {
        ntsa::AbstractBitString v(NTSCFG_TEST_ALLOCATOR);

        NTSCFG_TEST_ASSERT(v.numBytes() == 0);
        NTSCFG_TEST_ASSERT(v.numBits() == 0);

        v.setBit(0, 1);

        NTSCFG_TEST_EQ(v.getBit(0), 1);
        NTSCFG_TEST_EQ(v.getBit(1), 0);

        NTSCFG_TEST_LOG_DEBUG << "V = " << v << NTSCFG_TEST_LOG_END;
    }

    {
        ntsa::AbstractBitString v(NTSCFG_TEST_ALLOCATOR);

        NTSCFG_TEST_ASSERT(v.numBytes() == 0);
        NTSCFG_TEST_ASSERT(v.numBits() == 0);

        v.setBit(1, 1);

        NTSCFG_TEST_EQ(v.getBit(0), 0);
        NTSCFG_TEST_EQ(v.getBit(1), 1);
        NTSCFG_TEST_EQ(v.getBit(2), 0);

        NTSCFG_TEST_LOG_DEBUG << "V = " << v << NTSCFG_TEST_LOG_END;
    }

    {
        ntsa::AbstractBitString v(NTSCFG_TEST_ALLOCATOR);

        NTSCFG_TEST_ASSERT(v.numBytes() == 0);
        NTSCFG_TEST_ASSERT(v.numBits() == 0);

        v.setBit(2, 1);

        NTSCFG_TEST_EQ(v.getBit(0), 0);
        NTSCFG_TEST_EQ(v.getBit(1), 0);
        NTSCFG_TEST_EQ(v.getBit(2), 1);
        NTSCFG_TEST_EQ(v.getBit(3), 0);

        NTSCFG_TEST_LOG_DEBUG << "V = " << v << NTSCFG_TEST_LOG_END;
    }

    {
        ntsa::AbstractBitString v(NTSCFG_TEST_ALLOCATOR);

        NTSCFG_TEST_ASSERT(v.numBytes() == 0);
        NTSCFG_TEST_ASSERT(v.numBits() == 0);

        v.setBit(8, 1);

        NTSCFG_TEST_EQ(v.getBit(0), 0);
        NTSCFG_TEST_EQ(v.getBit(1), 0);
        NTSCFG_TEST_EQ(v.getBit(2), 0);
        NTSCFG_TEST_EQ(v.getBit(3), 0);
        NTSCFG_TEST_EQ(v.getBit(4), 0);
        NTSCFG_TEST_EQ(v.getBit(5), 0);
        NTSCFG_TEST_EQ(v.getBit(6), 0);
        NTSCFG_TEST_EQ(v.getBit(7), 0);

        NTSCFG_TEST_EQ(v.getBit(8), 1);
        NTSCFG_TEST_EQ(v.getBit(9), 0);

        NTSCFG_TEST_LOG_DEBUG << "V = " << v << NTSCFG_TEST_LOG_END;
    }

    {
        ntsa::AbstractBitString v(NTSCFG_TEST_ALLOCATOR);

        NTSCFG_TEST_ASSERT(v.numBytes() == 0);
        NTSCFG_TEST_ASSERT(v.numBits() == 0);

        v.setBit(9, 1);

        NTSCFG_TEST_EQ(v.getBit(0), 0);
        NTSCFG_TEST_EQ(v.getBit(1), 0);
        NTSCFG_TEST_EQ(v.getBit(2), 0);
        NTSCFG_TEST_EQ(v.getBit(3), 0);
        NTSCFG_TEST_EQ(v.getBit(4), 0);
        NTSCFG_TEST_EQ(v.getBit(5), 0);
        NTSCFG_TEST_EQ(v.getBit(6), 0);
        NTSCFG_TEST_EQ(v.getBit(7), 0);

        NTSCFG_TEST_EQ(v.getBit(8), 0);
        NTSCFG_TEST_EQ(v.getBit(9), 1);
        NTSCFG_TEST_EQ(v.getBit(10), 0);

        NTSCFG_TEST_LOG_DEBUG << "V = " << v << NTSCFG_TEST_LOG_END;
    }

    {
        ntsa::AbstractBitString v(NTSCFG_TEST_ALLOCATOR);

        NTSCFG_TEST_ASSERT(v.numBytes() == 0);
        NTSCFG_TEST_ASSERT(v.numBits() == 0);

        v.setBit(10, 1);

        NTSCFG_TEST_EQ(v.getBit(0), 0);
        NTSCFG_TEST_EQ(v.getBit(1), 0);
        NTSCFG_TEST_EQ(v.getBit(2), 0);
        NTSCFG_TEST_EQ(v.getBit(3), 0);
        NTSCFG_TEST_EQ(v.getBit(4), 0);
        NTSCFG_TEST_EQ(v.getBit(5), 0);
        NTSCFG_TEST_EQ(v.getBit(6), 0);
        NTSCFG_TEST_EQ(v.getBit(7), 0);

        NTSCFG_TEST_EQ(v.getBit(8), 0);
        NTSCFG_TEST_EQ(v.getBit(9), 0);
        NTSCFG_TEST_EQ(v.getBit(10), 1);
        NTSCFG_TEST_EQ(v.getBit(11), 0);

        NTSCFG_TEST_LOG_DEBUG << "V = " << v << NTSCFG_TEST_LOG_END;
    }

    {
        ntsa::AbstractBitString v(NTSCFG_TEST_ALLOCATOR);

        NTSCFG_TEST_ASSERT(v.numBytes() == 0);
        NTSCFG_TEST_ASSERT(v.numBits() == 0);

        v.setBit(0, 0);
        v.setBit(1, 1);
        v.setBit(2, 0);
        v.setBit(3, 1);
        v.setBit(4, 1);
        v.setBit(5, 0);
        v.setBit(6, 1);
        v.setBit(7, 1);

        v.setBit(8, 1);
        v.setBit(9, 0);
        v.setBit(10, 1);

        NTSCFG_TEST_EQ(v.getBit(0), 0);
        NTSCFG_TEST_EQ(v.getBit(1), 1);
        NTSCFG_TEST_EQ(v.getBit(2), 0);
        NTSCFG_TEST_EQ(v.getBit(3), 1);
        NTSCFG_TEST_EQ(v.getBit(4), 1);
        NTSCFG_TEST_EQ(v.getBit(5), 0);
        NTSCFG_TEST_EQ(v.getBit(6), 1);
        NTSCFG_TEST_EQ(v.getBit(7), 1);

        NTSCFG_TEST_EQ(v.getBit(8), 1);
        NTSCFG_TEST_EQ(v.getBit(9), 0);
        NTSCFG_TEST_EQ(v.getBit(10), 1);
        NTSCFG_TEST_EQ(v.getBit(11), 0);

        NTSCFG_TEST_LOG_DEBUG << "V = " << v << NTSCFG_TEST_LOG_END;
    }
}

#undef NTSA_ABSTRACT_INTEGER_LOG_INTEGRAL_OPERATION
#undef NTSA_ABSTRACT_INTEGER_LOG_ADD
#undef NTSA_ABSTRACT_INTEGER_LOG_SUBTRACT
#undef NTSA_ABSTRACT_INTEGER_LOG_MULTIPLY
#undef NTSA_ABSTRACT_INTEGER_LOG_DIVIDE
#undef NTSA_ABSTRACT_TEST_LOG_ENCODING_MISMATCH

}  // close namespace ntsa
}  // close namespace BloombergLP
