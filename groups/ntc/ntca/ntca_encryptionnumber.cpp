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

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptionnumber_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_cmath.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntca {



void EncryptionNumber::privateAssign(BlockVector*       result,
                                     const BlockVector& other)
{
    BSLS_ASSERT_OPT(&other != result);

    *result = other;
}

void EncryptionNumber::privateAssign(BlockVector* result, bsl::size_t value)
{
    BSLS_ASSERT_OPT(value <= bsl::numeric_limits<Block>::max());

    result->clear();
    result->resize(1);
    result->front() = static_cast<Block>(value);
}

void EncryptionNumber::privateKeep(Sign* result, Sign other)
{
    BSLS_ASSERT_OPT(&other != result);

    *result = other;
}

void EncryptionNumber::privateFlip(Sign* result, Sign other)
{
    BSLS_ASSERT_OPT(&other != result);

    if (other == e_POSITIVE) {
        *result = e_NEGATIVE;
    }
    else if (other == e_NEGATIVE) {
        *result = e_POSITIVE;
    }
    else {
        *result = e_ZERO;
    }
}

void EncryptionNumber::privateAdd(BlockVector*       sum,
                                  const BlockVector& addend1,
                                  const BlockVector& addend2)
{
    BSLS_ASSERT_OPT(sum);

    BlockVector result(sum->get_allocator());

    const BlockVector& lhs = addend1;
    const BlockVector& rhs = addend2;

    const bsl::size_t lhsSize = lhs.size();
    const bsl::size_t rhsSize = rhs.size();

	if (lhsSize == 0) {
		result = rhs;
	} 
    else if (rhsSize == 0) {
		result = lhs;
	}
    else {
        bsl::size_t maxSize = lhsSize;
        if (maxSize < rhsSize) {
            maxSize = rhsSize;
        }

        bool carryPrev = false;
        bool carryNext = false;

        for (bsl::size_t i = 0; i < maxSize; ++i) {
            Block lhsValue = i < lhsSize ? lhs[i] : 0;
            Block rhsValue = i < rhsSize ? rhs[i] : 0;

            Block temp = lhsValue + rhsValue;

            carryNext = (temp < lhsValue) || (temp < rhsValue);

            if (carryPrev) {
                ++temp;
                if (temp == 0) {
                    carryNext = true;
                }
            }

            result.push_back(temp);
            carryPrev = carryNext;
        }

        if (carryPrev) {
            result.push_back(1);
        }
    }

    EncryptionNumber::privateTrim(&result);
    sum->swap(result);
}

void EncryptionNumber::privateSubtract(BlockVector*       difference,
                                       const BlockVector& minuend,
                                       const BlockVector& subtrahend)
{
    BSLS_ASSERT_OPT(difference);

    BlockVector result(difference->get_allocator());

    const BlockVector& lhs = minuend;
    const BlockVector& rhs = subtrahend;

    const bsl::size_t lhsSize = lhs.size();
    const bsl::size_t rhsSize = rhs.size();

    BSLS_ASSERT_OPT(lhsSize >= rhsSize);

    if (rhsSize == 0) {
        result = lhs;
    }
    else {
        bsl::size_t maxSize = lhsSize;
        if (maxSize < rhsSize) {
            maxSize = rhsSize;
        }

        bool borrowPrev = false;
        bool borrowNext = false;

        for (bsl::size_t i = 0; i < maxSize; ++i) {
            Block lhsValue = i < lhsSize ? lhs[i] : 0;
            Block rhsValue = i < rhsSize ? rhs[i] : 0;

            Block temp = lhsValue - rhsValue;

            borrowNext = (temp > lhsValue);

            if (borrowPrev) {
                if (temp == 0) {
                    borrowNext = true;
                }
                --temp;
            }

            result.push_back(temp);
            borrowPrev = borrowNext;
        }
    }

    EncryptionNumber::privateTrim(&result);
    difference->swap(result);
}

void EncryptionNumber::privateMultiply(BlockVector*       product,
                                       const BlockVector& multiplicand,
                                       const BlockVector& multiplier)
{
    BSLS_ASSERT_OPT(product);

    BlockVector result(product->get_allocator());

    const bsl::size_t multiplicandSize = multiplicand.size();
    const bsl::size_t multiplierSize = multiplier.size();

	if (multiplicandSize != 0 && multiplierSize != 0) {
        const BlockVector& u = multiplicand;
        const BlockVector& v = multiplier;
        BlockVector&       w = result;

        const bsl::size_t m = multiplicandSize;
        const bsl::size_t n = multiplierSize;

        const bsl::size_t b = bsl::numeric_limits<Block>::max(); // radix

        // M1

        w.resize(m + n);
        bsl::size_t j = 0;

        while (true) {
            // M2

            if (v[j] == 0) {
                w[j + m] = 0;
                // goto M6
            }
            else {
                // M3

                bsl::size_t i = 0;
                bsl::size_t k = 0;

                while (true) {
                    // M4

                    bsl::size_t t = u[i] * v[j] + w[i + j] + k;

                    w[i + j] = t % b;
                    k = t / b;

                    BSLS_ASSERT_OPT(k >= 0);
                    BSLS_ASSERT_OPT(k < b);

                    // M5

                    ++i;
                    if (i < m) {
                        // goto M4
                        continue;
                    }
                    else {
                        w[j + m] = k;
                        break;
                    }
                }
            }

            // M6

            ++j;
            if (j < n) {
                // goto M2
                continue;
            }
            else {
                // done
                break;
            }
        }
    }

    EncryptionNumber::privateTrim(&result);
    product->swap(result);
}


void EncryptionNumber::privateMultiplyByAddition(
        BlockVector*       product,
        const BlockVector& multiplicand,
        const BlockVector& multiplier)
{
    BSLS_ASSERT_OPT(product);

    BlockVector result(product->get_allocator());

    const bsl::size_t multiplicandSize = multiplicand.size();
    const bsl::size_t multiplierSize = multiplier.size();

	if (multiplicandSize != 0 && multiplierSize != 0) {
        BSLS_ASSERT(!"Not implemented");
    }

    EncryptionNumber::privateTrim(&result);
    product->swap(result);
}

void EncryptionNumber::privateMultiplyByAddition(
        BlockVector*       product,
        const BlockVector& multiplicand,
        bsl::size_t        multiplier)
{
    BSLS_ASSERT_OPT(product);

    BlockVector result(product->get_allocator());

    const bsl::size_t multiplicandSize = multiplicand.size();

	if (multiplicandSize != 0 && multiplier != 0) {
        for (bsl::size_t i = 0; i < multiplier; ++i) {
            BlockVector temp(result.get_allocator());
            EncryptionNumber::privateAdd(&temp, result, multiplicand);
            result.swap(temp);
        }
    }

    EncryptionNumber::privateTrim(&result);
    product->swap(result);
}

void EncryptionNumber::privateDivideBySubtraction(
        BlockVector*       quotient,
        BlockVector*       remainder,
        const BlockVector& dividend,
        const BlockVector& divisor)
{
    BSLS_ASSERT_OPT(quotient);
    BSLS_ASSERT_OPT(remainder);

    NTCCFG_WARNING_UNUSED(dividend);
    NTCCFG_WARNING_UNUSED(divisor);
    
    BSLS_ASSERT_OPT(!"Not implemented");
}

void EncryptionNumber::privateDivideBySubtraction(
        BlockVector*       quotient,
        BlockVector*       remainder,
        const BlockVector& dividend,
        bsl::size_t        divisor)
{
    BSLS_ASSERT_OPT(quotient);
    BSLS_ASSERT_OPT(remainder);

    const BlockVectorAllocator& allocator = quotient->get_allocator();

    BlockVector iterations(allocator);

    BlockVector minuend(allocator);
    EncryptionNumber::privateAssign(&minuend, dividend);

    BlockVector subtrahend(allocator);
    EncryptionNumber::privateAssign(&subtrahend, divisor);

    BlockVector counter(allocator);
    EncryptionNumber::privateAssign(&counter, 1);

    const bsl::size_t dividendSize = dividend.size();

	if (dividendSize != 0 && divisor != 0) {

        while (true) {
            const int comparison = 
                EncryptionNumber::privateCompare(minuend, subtrahend);

            if (comparison < 0) {
                break;
            }

            BlockVector difference(allocator);
            EncryptionNumber::privateSubtract(&difference, minuend, subtrahend);
            difference.swap(minuend);

            EncryptionNumber::privateAdd(&iterations, iterations, counter);
        }

        // MRM
        // bigint result = 0, temp = *this;
	    // while (temp >= that) {
		//     temp -= that;
		//     ++result;
	    // }
	    // return result;
    }

    EncryptionNumber::privateTrim(&iterations);
    quotient->swap(iterations);

    EncryptionNumber::privateTrim(&minuend);
    remainder->swap(minuend);
}

int EncryptionNumber::privateCompare(const BlockVector& lhs, 
                                     const BlockVector& rhs)
{
    int result = 0;

    const bsl::size_t lhsSize = lhs.size();
    const bsl::size_t rhsSize = rhs.size();

    if (lhsSize < rhsSize) {
        result = -1;
    }
    else if (rhsSize < lhsSize) {
        result = +1;
    }
    else {
        bsl::size_t i = lhsSize;
        while (i > 0) {
            --i;
            if (lhs[i] < rhs[i]) {
                result = -1;
                break;
            }
            else if (rhs[i] < lhs[i]) {
                result = +1;
                break;
            }
            else {
                continue;
            }
        }
    }

    return result;
}

void EncryptionNumber::privateTrim(BlockVector* result)
{
    while (!result->empty()) {
        if (result->back() == 0) {
            result->pop_back();
        }
        else {
            break;
        }
    }
}


bool EncryptionNumber::privateIsZero(const BlockVector& value)
{
    return value.empty() || (value.size() == 1 && value.front() == 0);
}

bool EncryptionNumber::privateIsOne(const BlockVector& value)
{
    return value.size() == 1 && value.front() == 1;
}

EncryptionNumber::EncryptionNumber(bslma::Allocator* basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionNumber::EncryptionNumber(short             value,
                                   bslma::Allocator* basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->assign(value);
}

EncryptionNumber::EncryptionNumber(unsigned short    value,
                                   bslma::Allocator* basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->assign(value);
}

EncryptionNumber::EncryptionNumber(int value, bslma::Allocator* basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->assign(value);
}

EncryptionNumber::EncryptionNumber(unsigned int      value,
                                   bslma::Allocator* basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->assign(value);
}

EncryptionNumber::EncryptionNumber(long              value,
                                   bslma::Allocator* basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->assign(value);
}

EncryptionNumber::EncryptionNumber(unsigned long     value,
                                   bslma::Allocator* basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->assign(value);
}

EncryptionNumber::EncryptionNumber(long long         value,
                                   bslma::Allocator* basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->assign(value);
}

EncryptionNumber::EncryptionNumber(unsigned long long value,
                                   bslma::Allocator*  basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->assign(value);
}

EncryptionNumber::EncryptionNumber(const EncryptionNumber& original,
                                   bslma::Allocator*       basicAllocator)
: d_sign(original.d_sign)
, d_data(original.d_data, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionNumber::~EncryptionNumber()
{
}

EncryptionNumber& EncryptionNumber::operator=(const EncryptionNumber& other)
{
    if (this != &other) {
        d_sign = other.d_sign;
        d_data = other.d_data;
    }

    return *this;
}

void EncryptionNumber::reset()
{
    d_sign = e_ZERO;
    d_data.clear();
}

void EncryptionNumber::swap(EncryptionNumber& other)
{
    if (this != &other) {
        bsl::swap(d_sign, other.d_sign);
        d_data.swap(other.d_data);
    }
}

ntsa::Error EncryptionNumber::parse(const bsl::string_view& text)
{
    if (text.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    int  base     = 10;
    bool negative = false;

    bsl::string_view temp;
    {
        bsl::string_view::const_iterator it = text.begin();
        bsl::string_view::const_iterator et = text.end();

        if (it == et) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (*it == '0') {
            ++it;
            if (it == et) {
                this->reset();
                return ntsa::Error();
            }
            else if (*it == 'x' || *it == 'X') {
                base = 16;
                ++it;
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }

        if (*it == '+') {
            if (base != 10) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            ++it;
        }
        else if (*it == '-') {
            if (base != 10) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            negative = true;
            ++it;
        }

        if (it == et) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        temp = bsl::string_view(it, et);
    }

    // Vector of digits in the base, least-significant first.
    BlockVector digits;
    {
        bsl::string_view::const_reverse_iterator it = temp.rbegin();
        bsl::string_view::const_reverse_iterator et = temp.rend();

        for (; it != et; ++it) {
            const char ch = *it;

            if (base == 10) {
                if (ch >= '0' && ch <= '9') {
                    Block digit = ch - '0';
                    digits.push_back(digit);
                }
                else {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }
            }
            else if (base == 16) {
                if (ch >= '0' && ch <= '9') {
                    Block digit = ch - '0';
                    digits.push_back(digit);
                }
                else if (ch >= 'a' && ch <= 'f') {
                    Block digit = ch - 'a';
                    digits.push_back(digit);
                }
                else if (ch >= 'A' && ch <= 'F') {
                    Block digit = ch - 'A';
                    digits.push_back(digit);
                }
                else {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }

        EncryptionNumber::privateTrim(&digits);
    }

    BSLS_ASSERT_OPT(!digits.empty());

    EncryptionNumber result(this->allocator());
    {
        BlockVector::const_reverse_iterator it = digits.rbegin();
        BlockVector::const_reverse_iterator et = digits.rend();

        for (; it != et; ++it) {
            Block value = *it;
            // result.multiply(base);
            {
                EncryptionNumber addend(result);
                for (int b = 0; b < base - 1; ++b) {
                    result.add(addend);
                }
            }

            result.add(value);
        }
    }

    if (result.d_data.size() == 1 && result.d_data.front() == 0) {
        result.d_sign = e_ZERO;
        result.d_data.clear();
    }
    else if (negative) {
        result.d_sign = e_NEGATIVE;
    }

    this->swap(result);
    return ntsa::Error();
}

EncryptionNumber& EncryptionNumber::assign(short value)
{
    if (value == 0) {
        d_sign = e_ZERO;
        d_data.clear();
    }
    else {
        if (value > 0) {
            d_sign = e_POSITIVE;
        }
        else if (value < 0) {
            d_sign = e_NEGATIVE;
        }
        d_data.clear();
        d_data.resize(1, static_cast<Block>(value));
    }

    return *this;
}

EncryptionNumber& EncryptionNumber::assign(unsigned short value)
{
    if (value == 0) {
        d_sign = e_ZERO;
        d_data.clear();
    }
    else {
        d_sign = e_POSITIVE;
        d_data.clear();
        d_data.resize(1, static_cast<Block>(value));
    }

    return *this;
}

EncryptionNumber& EncryptionNumber::assign(int value)
{
    if (value == 0) {
        d_sign = e_ZERO;
        d_data.clear();
    }
    else {
        if (value > 0) {
            d_sign = e_POSITIVE;
        }
        else if (value < 0) {
            d_sign = e_NEGATIVE;
        }
        d_data.clear();
        d_data.resize(1, static_cast<Block>(value));
    }

    return *this;
}

EncryptionNumber& EncryptionNumber::assign(unsigned int value)
{
    if (value == 0) {
        d_sign = e_ZERO;
        d_data.clear();
    }
    else {
        d_sign = e_POSITIVE;
        d_data.clear();
        d_data.resize(1, static_cast<Block>(value));
    }

    return *this;
}

EncryptionNumber& EncryptionNumber::assign(long value)
{
    if (value == 0) {
        d_sign = e_ZERO;
        d_data.clear();
    }
    else {
        if (value > 0) {
            d_sign = e_POSITIVE;
        }
        else if (value < 0) {
            d_sign = e_NEGATIVE;
        }
        d_data.clear();
        d_data.resize(1, static_cast<Block>(value));
    }

    return *this;
}

EncryptionNumber& EncryptionNumber::assign(unsigned long value)
{
    if (value == 0) {
        d_sign = e_ZERO;
        d_data.clear();
    }
    else {
        d_sign = e_POSITIVE;
        d_data.clear();
        d_data.resize(1, static_cast<Block>(value));
    }

    return *this;
}

EncryptionNumber& EncryptionNumber::assign(long long value)
{
    if (value == 0) {
        d_sign = e_ZERO;
        d_data.clear();
    }
    else {
        if (value > 0) {
            d_sign = e_POSITIVE;
        }
        else if (value < 0) {
            d_sign = e_NEGATIVE;
        }
        d_data.clear();
        d_data.resize(1, static_cast<Block>(value));
    }

    return *this;
}

EncryptionNumber& EncryptionNumber::assign(unsigned long long value)
{
    if (value == 0) {
        d_sign = e_ZERO;
        d_data.clear();
    }
    else {
        d_sign = e_POSITIVE;
        d_data.clear();
        d_data.resize(1, static_cast<Block>(value));
    }

    return *this;
}

EncryptionNumber& EncryptionNumber::assign(const EncryptionNumber& value)
{
    if (this != &value) {
        d_sign = value.d_sign;
        d_data = value.d_data;
    }

    return *this;
}

EncryptionNumber& EncryptionNumber::negate()
{
    if (d_sign == e_POSITIVE) {
        d_sign = e_NEGATIVE;
    }
    else if (d_sign == e_NEGATIVE) {
        d_sign = e_POSITIVE;
    }

    return *this;
}

EncryptionNumber& EncryptionNumber::increment()
{
    return this->add(1);
}

EncryptionNumber& EncryptionNumber::decrement()
{
    return this->subtract(1);
}

EncryptionNumber& EncryptionNumber::add(short value)
{
    return this->add(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::add(unsigned short value)
{
    return this->add(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::add(int value)
{
    return this->add(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::add(unsigned int value)
{
    return this->add(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::add(long value)
{
    return this->add(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::add(unsigned long value)
{
    return this->add(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::add(long long value)
{
    return this->add(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::add(unsigned long long value)
{
    return this->add(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::add(const EncryptionNumber& other)
{
    const EncryptionNumber& lhs = *this;
    const EncryptionNumber& rhs = other;

    if (lhs.d_sign == e_ZERO) {
        EncryptionNumber result(d_allocator_p);
        EncryptionNumber::privateAssign(&result.d_data, rhs.d_data);
        EncryptionNumber::privateKeep(&result.d_sign, rhs.d_sign);
        this->swap(result);
        return *this;
    }
    else if (rhs.d_sign == e_ZERO) {
        return *this;
    }
    else if (lhs.d_sign == rhs.d_sign) {
        EncryptionNumber result(d_allocator_p);
        EncryptionNumber::privateAdd(&result.d_data, lhs.d_data, rhs.d_data);
        EncryptionNumber::privateKeep(&result.d_sign, lhs.d_sign);
        this->swap(result);
        return *this;
    }
    else {
        const int comparison = lhs.compare(rhs);
        if (comparison == 0) {
            this->reset();
            return *this;
        }
        else if (comparison > 0) {
            EncryptionNumber result(d_allocator_p);
            EncryptionNumber::privateSubtract(&result.d_data,
                                              lhs.d_data,
                                              rhs.d_data);
            EncryptionNumber::privateKeep(&result.d_sign, lhs.d_sign);
            this->swap(result);
            return *this;
        }
        else {
            EncryptionNumber result(d_allocator_p);
            EncryptionNumber::privateSubtract(&result.d_data,
                                              rhs.d_data,
                                              lhs.d_data);
            EncryptionNumber::privateKeep(&result.d_sign, rhs.d_sign);
            this->swap(result);
            return *this;
        }
    }
}

EncryptionNumber& EncryptionNumber::subtract(short value)
{
    return this->subtract(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::subtract(unsigned short value)
{
    return this->subtract(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::subtract(int value)
{
    return this->subtract(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::subtract(unsigned int value)
{
    return this->subtract(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::subtract(long value)
{
    return this->subtract(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::subtract(unsigned long value)
{
    return this->subtract(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::subtract(long long value)
{
    return this->subtract(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::subtract(unsigned long long value)
{
    return this->subtract(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::subtract(const EncryptionNumber& other)
{
    const EncryptionNumber& lhs = *this;
    const EncryptionNumber& rhs = other;

    if (lhs.d_sign == e_ZERO) {
        EncryptionNumber result(d_allocator_p);
        EncryptionNumber::privateAssign(&result.d_data, rhs.d_data);
        EncryptionNumber::privateFlip(&result.d_sign, rhs.d_sign);
        this->swap(result);
        return *this;
    }
    else if (rhs.d_sign == e_ZERO) {
        return *this;
    }
    else if (lhs.d_sign != rhs.d_sign) {
        EncryptionNumber result(d_allocator_p);
        EncryptionNumber::privateAdd(&result.d_data, lhs.d_data, rhs.d_data);
        EncryptionNumber::privateKeep(&result.d_sign, lhs.d_sign);
        this->swap(result);
        return *this;
    }
    else {
        const int comparison = lhs.compare(rhs);
        if (comparison == 0) {
            this->reset();
            return *this;
        }
        else if (comparison > 0) {
            EncryptionNumber result(d_allocator_p);
            EncryptionNumber::privateSubtract(&result.d_data,
                                              lhs.d_data,
                                              rhs.d_data);
            EncryptionNumber::privateKeep(&result.d_sign, lhs.d_sign);
            this->swap(result);
            return *this;
        }
        else {
            EncryptionNumber result(rhs, d_allocator_p);
            EncryptionNumber::privateSubtract(&result.d_data,
                                              rhs.d_data,
                                              lhs.d_data);
            EncryptionNumber::privateFlip(&result.d_sign, rhs.d_sign);
            this->swap(result);
            return *this;
        }
    }
}

EncryptionNumber& EncryptionNumber::multiply(short value)
{
    return this->multiply(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::multiply(unsigned short value)
{
    return this->multiply(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::multiply(int value)
{
    return this->multiply(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::multiply(unsigned int value)
{
    return this->multiply(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::multiply(long value)
{
    return this->multiply(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::multiply(unsigned long value)
{
    return this->multiply(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::multiply(long long value)
{
    return this->multiply(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::multiply(unsigned long long value)
{
    return this->multiply(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::multiply(const EncryptionNumber& other)
{
    const EncryptionNumber& lhs = *this;
    const EncryptionNumber& rhs = other;

    if (lhs.d_sign == e_ZERO || rhs.d_sign == e_ZERO) {
        this->reset();
        return *this;
    }

    EncryptionNumber result(d_allocator_p);
        
    EncryptionNumber::privateMultiply(&result.d_data, lhs.d_data, rhs.d_data);
    result.d_sign = lhs.d_sign == rhs.d_sign ? e_POSITIVE : e_NEGATIVE;

    this->swap(result);
    return *this;
}

EncryptionNumber& EncryptionNumber::divide(short value)
{
    return this->divide(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::divide(unsigned short value)
{
    return this->divide(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::divide(int value)
{
    return this->divide(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::divide(unsigned int value)
{
    return this->divide(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::divide(long value)
{
    return this->divide(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::divide(unsigned long value)
{
    return this->divide(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::divide(long long value)
{
    return this->divide(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::divide(unsigned long long value)
{
    return this->divide(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::divide(const EncryptionNumber& other)
{
    NTCCFG_WARNING_UNUSED(other);
    BSLS_ASSERT_OPT(!"Not implemented");
    return *this;
}

EncryptionNumber& EncryptionNumber::divide(const EncryptionNumber& other, 
                                           EncryptionNumber*       remainder)
{
    NTCCFG_WARNING_UNUSED(other);
    NTCCFG_WARNING_UNUSED(remainder);
    BSLS_ASSERT_OPT(!"Not implemented");
    return *this;
}

EncryptionNumber& EncryptionNumber::modulus(short value)
{
    return this->modulus(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::modulus(unsigned short value)
{
    return this->modulus(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::modulus(int value)
{
    return this->modulus(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::modulus(unsigned int value)
{
    return this->modulus(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::modulus(long value)
{
    return this->modulus(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::modulus(unsigned long value)
{
    return this->modulus(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::modulus(long long value)
{
    return this->modulus(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::modulus(unsigned long long value)
{
    return this->modulus(EncryptionNumber(value));
}

EncryptionNumber& EncryptionNumber::modulus(const EncryptionNumber& other)
{
    NTCCFG_WARNING_UNUSED(other);
    BSLS_ASSERT_OPT(!"Not implemented");
    return *this;
}

bool EncryptionNumber::equals(short value) const
{
    return this->equals(EncryptionNumber(value));
}

bool EncryptionNumber::equals(unsigned short value) const
{
    return this->equals(EncryptionNumber(value));
}

bool EncryptionNumber::equals(int value) const
{
    return this->equals(EncryptionNumber(value));
}

bool EncryptionNumber::equals(unsigned int value) const
{
    return this->equals(EncryptionNumber(value));
}

bool EncryptionNumber::equals(long value) const
{
    return this->equals(EncryptionNumber(value));
}

bool EncryptionNumber::equals(unsigned long value) const
{
    return this->equals(EncryptionNumber(value));
}

bool EncryptionNumber::equals(long long value) const
{
    return this->equals(EncryptionNumber(value));
}

bool EncryptionNumber::equals(unsigned long long value) const
{
    return this->equals(EncryptionNumber(value));
}

bool EncryptionNumber::equals(const EncryptionNumber& other) const
{
    return d_sign == other.d_sign && d_data == other.d_data;
}

int EncryptionNumber::compare(short value) const
{
    return this->compare(EncryptionNumber(value));
}

int EncryptionNumber::compare(unsigned short value) const
{
    return this->compare(EncryptionNumber(value));
}

int EncryptionNumber::compare(int value) const
{
    return this->compare(EncryptionNumber(value));
}

int EncryptionNumber::compare(unsigned int value) const
{
    return this->compare(EncryptionNumber(value));
}

int EncryptionNumber::compare(long value) const
{
    return this->compare(EncryptionNumber(value));
}

int EncryptionNumber::compare(unsigned long value) const
{
    return this->compare(EncryptionNumber(value));
}

int EncryptionNumber::compare(long long value) const
{
    return this->compare(EncryptionNumber(value));
}

int EncryptionNumber::compare(unsigned long long value) const
{
    return this->compare(EncryptionNumber(value));
}

int EncryptionNumber::compare(const EncryptionNumber& other) const
{
    const Sign lhsSign = d_sign;
    const Sign rhsSign = other.d_sign;

    int result = 0;

    if (lhsSign < rhsSign) {
        result = -1;
    }
    else if (rhsSign < lhsSign) {
        result = +1;
    }
    else {
        result = EncryptionNumber::privateCompare(d_data, other.d_data) * 
                 static_cast<int>(lhsSign);
    }

    return result;
}

ntsa::Error EncryptionNumber::convert(short* result) const
{
    const bsl::size_t size = d_data.size();

    if (size == 0) {
        *result = 0;
        return ntsa::Error();
    }

    if (size > 1) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    const Block value = d_data.front();

    if (value > static_cast<Block>(bsl::numeric_limits<short>::max())) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    *result = static_cast<short>(value);
    return ntsa::Error();
}

ntsa::Error EncryptionNumber::convert(unsigned short* result) const
{
    const bsl::size_t size = d_data.size();

    if (size == 0) {
        *result = 0;
        return ntsa::Error();
    }

    if (size > 1) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    if (d_sign == e_NEGATIVE) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    const Block value = d_data.front();

    if (value > static_cast<Block>(bsl::numeric_limits<short>::max())) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    *result = static_cast<short>(value);
    return ntsa::Error();
}

ntsa::Error EncryptionNumber::convert(int* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionNumber::convert(unsigned int* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionNumber::convert(long* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionNumber::convert(unsigned long* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionNumber::convert(long long* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionNumber::convert(unsigned long long* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

void EncryptionNumber::generate(bsl::string* result, int base) const
{
    ntsa::Error error;

    result->clear();

    const bsl::size_t size = d_data.size();

    if (d_sign == e_ZERO || size == 0) {
        result->push_back('0');
        return;
    }

    // Vector of digits in the base, least-significant first.
    BlockVector digits;
    {
        BlockVector dividend(d_allocator_p);
        EncryptionNumber::privateAssign(&dividend, d_data);

        const bsl::size_t divisor = static_cast<bsl::size_t>(base);

        while (!EncryptionNumber::privateIsZero(dividend)) {
            BlockVector quotient(d_allocator_p);
            BlockVector remainder(d_allocator_p);

            EncryptionNumber::privateDivideBySubtraction(
                &quotient, &remainder, dividend, divisor);

            BSLS_ASSERT_OPT(remainder.size() == 1);
            Block digit = static_cast<Block>(remainder.front());
            
            digits.push_back(digit);
            dividend.swap(quotient);
        }
    }

    BlockVector::const_iterator it = digits.begin();
    BlockVector::const_iterator et = digits.end();

    for (; it != et; ++it) {
        Block value = *it;

        if (value < 10) {
            result->push_back(static_cast<char>('0' + value));
        }
        else {
            result->push_back(static_cast<char>('a' + (value - 10)));
        }

        #if 0
        while (true) {
            const bsl::uint64_t digit = value % base;

            if (digit < 10) {
                result->push_back(static_cast<char>('0' + digit));
            }
            else {
                result->push_back(static_cast<char>('a' + (digit - 10)));
            }

            value /= base;

            if (value == 0) {
                break;
            }
        }
        #endif
    }

    if (d_sign == e_NEGATIVE) {
        result->push_back('-');
    }

    bsl::reverse(result->begin(), result->end());
}

bool EncryptionNumber::isZero() const
{
    return d_sign == e_ZERO;
}

bool EncryptionNumber::isPositive() const
{
    return d_sign == e_POSITIVE;
}

bool EncryptionNumber::isNegative() const
{
    return d_sign == e_NEGATIVE;
}

bslma::Allocator* EncryptionNumber::allocator() const
{
    return d_allocator_p;
}

bsl::ostream& EncryptionNumber::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    NTCCFG_WARNING_UNUSED(level);
    NTCCFG_WARNING_UNUSED(spacesPerLevel);

    bsl::string result;
    int         base = 10;

    const bsl::ostream::fmtflags flags = stream.flags();

    if ((flags & std::ios_base::hex) != 0) {
        base = 16;
    }
    else if ((flags & std::ios_base::oct) != 0) {
        base = 8;
    }

    this->generate(&result, base);
    stream << result;
    return stream;
}

EncryptionNumber EncryptionNumber::operator+(
    const EncryptionNumber& other) const
{
    EncryptionNumber result(*this);
    result.add(other);
    return result;
}

EncryptionNumber EncryptionNumber::operator-(
    const EncryptionNumber& other) const
{
    EncryptionNumber result(*this);
    result.subtract(other);
    return result;
}

EncryptionNumber EncryptionNumber::operator*(
    const EncryptionNumber& other) const
{
    EncryptionNumber result(*this);
    result.multiply(other);
    return result;
}

EncryptionNumber EncryptionNumber::operator/(
    const EncryptionNumber& other) const
{
    EncryptionNumber result(*this);
    result.divide(other);
    return result;
}

EncryptionNumber EncryptionNumber::operator%(
    const EncryptionNumber& other) const
{
    EncryptionNumber result(*this);
    result.modulus(other);
    return result;
}

EncryptionNumber EncryptionNumber::operator-() const
{
    EncryptionNumber result(*this);
    result.negate();
    return result;
}

EncryptionNumber& EncryptionNumber::operator+=(const EncryptionNumber& other)
{
    return this->add(other);
}

EncryptionNumber& EncryptionNumber::operator-=(const EncryptionNumber& other)
{
    return this->subtract(other);
}

EncryptionNumber& EncryptionNumber::operator*=(const EncryptionNumber& other)
{
    return this->multiply(other);
}

EncryptionNumber& EncryptionNumber::operator/=(const EncryptionNumber& other)
{
    return this->divide(other);
}

EncryptionNumber& EncryptionNumber::operator%=(const EncryptionNumber& other)
{
    return this->modulus(other);
}

EncryptionNumber& EncryptionNumber::operator++()
{
    return this->increment();
}

EncryptionNumber EncryptionNumber::operator++(int)
{
    EncryptionNumber result(*this);
    result.increment();
    return result;
}

EncryptionNumber& EncryptionNumber::operator--()
{
    return this->decrement();
}

EncryptionNumber EncryptionNumber::operator--(int)
{
    EncryptionNumber result(*this);
    result.decrement();
    return result;
}

bsl::ostream& operator<<(bsl::ostream& stream, const EncryptionNumber& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionNumber& lhs, const EncryptionNumber& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionNumber& lhs, const EncryptionNumber& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionNumber& lhs, const EncryptionNumber& rhs)
{
    return lhs.compare(rhs) < 0;
}

bool operator<=(const EncryptionNumber& lhs, const EncryptionNumber& rhs)
{
    return lhs.compare(rhs) <= 0;
}

bool operator>(const EncryptionNumber& lhs, const EncryptionNumber& rhs)
{
    return lhs.compare(rhs) > 0;
}

bool operator>=(const EncryptionNumber& lhs, const EncryptionNumber& rhs)
{
    return lhs.compare(rhs) >= 0;
}

}  // close package namespace
}  // close enterprise namespace
