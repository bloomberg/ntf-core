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

namespace {

const ntca::AbstractIntegerBase::Value k_DEFAULT_BASE = 
    AbstractIntegerBase::e_DECIMAL;

struct AbstractIntegerBaseTraits {
    bsl::uint64_t d_radix;
    bsl::uint64_t d_minValue;
    bsl::uint64_t d_maxValue;
};

// clang-format off
static const AbstractIntegerBaseTraits k_TRAITS[5] = {
    { 65536, 0, 65535 },
    {     2, 0,     1 },
    {     8, 0,     7 },
    {    10, 0,     9 },
    {    16, 0,    15 }
};
// clang-format on

}  // close unnamed namespace


int AbstractIntegerSign::multiplier(Value sign)
{
    return static_cast<int>(sign);
}

const char* AbstractIntegerSign::toString(Value value)
{
    switch (value) {
    case e_NEGATIVE: {
        return "NEGATIVE";
    } break;
    case e_ZERO: {
        return "ZERO";
    } break;
    case e_POSITIVE: {
        return "POSITIVE";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& AbstractIntegerSign::print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, AbstractIntegerSign::Value rhs)
{
    return AbstractIntegerSign::print(stream, rhs);
}



bsl::uint64_t AbstractIntegerBase::radix(AbstractIntegerBase::Value base)
{
    return k_TRAITS[static_cast<bsl::size_t>(base)].d_radix;
}

bool AbstractIntegerBase::validate(AbstractIntegerBase::Value base,
                                   bsl::uint64_t              value)
{
    return value <= k_TRAITS[static_cast<bsl::size_t>(base)].d_maxValue;
}

const char* AbstractIntegerBase::toString(Value value)
{
    switch (value) {
    case e_NATIVE: {
        return "NATIVE";
    } break;
    case e_BINARY: {
        return "BINARY";
    } break;
    case e_OCTAL: {
        return "OCTAL";
    } break;
    case e_DECIMAL: {
        return "DECIMAL";
    } break;
    case e_HEXADECIMAL: {
        return "HEXADECIMAL";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& AbstractIntegerBase::print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, AbstractIntegerBase::Value rhs)
{
    return AbstractIntegerBase::print(stream, rhs);
}

void AbstractIntegerRepresentation::privateBlockShiftLeft(
    AbstractIntegerRepresentation* result,
    bsl::size_t              count)
{
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(count);

    NTCCFG_NOT_IMPLEMENTED();
}

void AbstractIntegerRepresentation::privateBlockShiftRight(
    AbstractIntegerRepresentation* result,
    bsl::size_t              count)
{
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(count);

    NTCCFG_NOT_IMPLEMENTED();
}

void AbstractIntegerRepresentation::privateBlockCopyPrefix(
    AbstractIntegerRepresentation*       result,
    const AbstractIntegerRepresentation& value,
    bsl::size_t                    count)
{
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(value);
    NTCCFG_WARNING_UNUSED(count);

    NTCCFG_NOT_IMPLEMENTED();
}

void AbstractIntegerRepresentation::privateBlockCopySuffix(
    AbstractIntegerRepresentation*       result,
    const AbstractIntegerRepresentation& value,
    bsl::size_t                    count)
{
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(value);
    NTCCFG_WARNING_UNUSED(count);

    NTCCFG_NOT_IMPLEMENTED();
}

void AbstractIntegerRepresentation::privateBlockCopyRange(
    AbstractIntegerRepresentation*       result,
    const AbstractIntegerRepresentation& value,
    bsl::size_t                    offset,
    bsl::size_t                    count)
{
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(value);
    NTCCFG_WARNING_UNUSED(offset);
    NTCCFG_WARNING_UNUSED(count);

    NTCCFG_NOT_IMPLEMENTED();
}

AbstractIntegerRepresentation::AbstractIntegerRepresentation(
    bslma::Allocator* basicAllocator)
: d_base(k_DEFAULT_BASE)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

AbstractIntegerRepresentation::AbstractIntegerRepresentation(
    AbstractIntegerBase::Value base,
    bslma::Allocator*          basicAllocator)
: d_base(base)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

AbstractIntegerRepresentation::AbstractIntegerRepresentation(
    const AbstractIntegerRepresentation& original,
    bslma::Allocator*                    basicAllocator)
: d_base(original.d_base)
, d_data(original.d_data, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

AbstractIntegerRepresentation::~AbstractIntegerRepresentation()
{
}

AbstractIntegerRepresentation& AbstractIntegerRepresentation::operator=(
    const AbstractIntegerRepresentation& other)
{
    this->assign(other);
    return *this;
}

void AbstractIntegerRepresentation::reset()
{
    d_base = k_DEFAULT_BASE;

    d_data.clear();
    d_data.shrink_to_fit();
}

void AbstractIntegerRepresentation::resize(bsl::size_t size)
{
    d_data.resize(size);
}

void AbstractIntegerRepresentation::swap(AbstractIntegerRepresentation& other)
{
    bsl::swap(d_base, other.d_base);
    d_data.swap(other.d_data);
}

void AbstractIntegerRepresentation::assign(
    const AbstractIntegerRepresentation& other)
{
    if (this != &other) {
        d_base = other.d_base;
        d_data = other.d_data;
    }
}

void AbstractIntegerRepresentation::assign(bsl::uint64_t value)
{
    d_data.clear();

    const bsl::uint64_t radix = this->radix();

    bsl::uint64_t remaining = value;

    while (remaining != 0) {
        bsl::uint64_t place  = remaining % radix;
        remaining           /= radix;

        this->push(place);
    }
}

void AbstractIntegerRepresentation::normalize()
{
    while (!d_data.empty()) {
        if (d_data.back() == 0) {
            d_data.pop_back();
        }
        else {
            break;
        }
    }
}

void AbstractIntegerRepresentation::set(bsl::size_t index, bsl::uint64_t value)
{
    BSLS_ASSERT_OPT(ntca::AbstractIntegerBase::validate(d_base, value));

    if (index >= d_data.size()) {
        d_data.resize(index + 1);
    }

    BSLS_ASSERT_OPT(index < d_data.size());
    d_data[index] = value;
}

void AbstractIntegerRepresentation::push(bsl::uint64_t value)
{
    BSLS_ASSERT_OPT(ntca::AbstractIntegerBase::validate(d_base, value));
    d_data.push_back(value);
}

void AbstractIntegerRepresentation::pop()
{
    d_data.pop_back();
}

bsl::uint64_t AbstractIntegerRepresentation::get(bsl::size_t index) const
{
    if (index < d_data.size()) {
        return static_cast<bsl::uint64_t>(d_data[index]);
    }
    else {
        return 0;
    }
}

AbstractIntegerBase::Value AbstractIntegerRepresentation::base() const
{
    return d_base;
}

bsl::uint64_t AbstractIntegerRepresentation::radix() const
{
    return AbstractIntegerBase::radix(d_base);
}

bool AbstractIntegerRepresentation::equals(
    const AbstractIntegerRepresentation& other) const
{
    return d_data == other.d_data;
}

int AbstractIntegerRepresentation::compare(
    const AbstractIntegerRepresentation& other) const
{
    int result = 0;

    const bsl::size_t lhsSize = d_data.size();
    const bsl::size_t rhsSize = other.d_data.size();

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
            if (d_data[i] < other.d_data[i]) {
                result = -1;
                break;
            }
            else if (other.d_data[i] < d_data[i]) {
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

bsl::size_t AbstractIntegerRepresentation::size() const
{
    return d_data.size();
}

bool AbstractIntegerRepresentation::isZero() const
{
    return d_data.empty() || (d_data.size() == 1 && d_data.front() == 0);
}

bool AbstractIntegerRepresentation::isOne() const
{
    return d_data.size() == 1 && d_data.front() == 1;
}

bool AbstractIntegerRepresentation::isAlias(
    const AbstractIntegerRepresentation* other) const
{
    return this == other;
}

bool AbstractIntegerRepresentation::isAlias(
    const AbstractIntegerRepresentation& other) const
{
    return this == &other;
}

bslma::Allocator* AbstractIntegerRepresentation::allocator() const
{
    return d_allocator_p;
}

bsl::ostream& AbstractIntegerRepresentation::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("base", d_base);

    if (!d_data.empty()) {
        printer.printAttribute("data", d_data.rbegin(), d_data.rend());
    }
    else {
        printer.printAttribute("data", static_cast<Block>(0));
    }

    printer.end();
    return stream;
}

void AbstractIntegerRepresentation::add(
    AbstractIntegerRepresentation*       sum,
    const AbstractIntegerRepresentation& addend1,
    const AbstractIntegerRepresentation& addend2)
{
    BSLS_ASSERT_OPT(!addend1.isAlias(sum));
    BSLS_ASSERT_OPT(!addend2.isAlias(sum));

    BSLS_ASSERT_OPT(addend1.base() == sum->base());
    BSLS_ASSERT_OPT(addend2.base() == sum->base());

    sum->reset();

    if (addend1.isZero()) {
        *sum = addend2;
        return;
    }

    if (addend2.isZero()) {
        *sum = addend1;
        return;
    }

    const bsl::uint64_t radix = sum->radix();

    const AbstractIntegerRepresentation& lhs = addend1;
    const AbstractIntegerRepresentation& rhs = addend2;

    const bsl::size_t lhsSize = lhs.size();
    const bsl::size_t rhsSize = rhs.size();
    const bsl::size_t maxSize = bsl::max(lhsSize, rhsSize);

    bool carry = false;

    for (bsl::size_t i = 0; i < maxSize; ++i) {
        bsl::uint64_t lhsValue = lhs.get(i);
        bsl::uint64_t rhsValue = rhs.get(i);

        bsl::uint64_t temp = lhsValue + rhsValue;

        if (carry) {
            ++temp;
        }

        if (temp >= radix) {
            temp -= radix;
            carry = true;
        }
        else {
            carry = false;
        }

        sum->push(temp);
    }

    if (carry) {
        sum->push(1);
    }

    sum->normalize();
}

void AbstractIntegerRepresentation::subtract(
    AbstractIntegerRepresentation*       difference,
    const AbstractIntegerRepresentation& minuend,
    const AbstractIntegerRepresentation& subtrahend)
{
    BSLS_ASSERT_OPT(!minuend.isAlias(difference));
    BSLS_ASSERT_OPT(!subtrahend.isAlias(difference));

    BSLS_ASSERT_OPT(minuend.base() == difference->base());
    BSLS_ASSERT_OPT(subtrahend.base() == difference->base());

    difference->reset();

    if (subtrahend.isZero()) {
        *difference = minuend;
        return;
    }

    const bsl::uint64_t radix = difference->radix();

    const AbstractIntegerRepresentation& lhs = minuend;
    const AbstractIntegerRepresentation& rhs = subtrahend;

    const bsl::size_t lhsSize = lhs.size();
    const bsl::size_t rhsSize = rhs.size();
    const bsl::size_t maxSize = bsl::max(lhsSize, rhsSize);

    if (lhsSize < rhsSize) {
        return;
    }

    const int comparison = lhs.compare(rhs);
    if (comparison < 0) {
        return;
    }

    bool borrow = false;

    for (bsl::size_t i = 0; i < maxSize; ++i) {
        bsl::uint64_t lhsValue = lhs.get(i);
        bsl::uint64_t rhsValue = rhs.get(i);

        bsl::uint64_t temp = lhsValue - rhsValue;

        if (borrow) {
            --temp;
        }

        if (temp >= radix) {
            temp += radix;
            borrow = true;
        }
        else {
            borrow = false;
        }

        difference->push(temp);
    }

    difference->normalize();
}


void AbstractIntegerRepresentation::multiply(
    AbstractIntegerRepresentation*       product,
    const AbstractIntegerRepresentation& multiplicand,
    const AbstractIntegerRepresentation& multiplier)
{
    BSLS_ASSERT_OPT(!multiplicand.isAlias(product));
    BSLS_ASSERT_OPT(!multiplier.isAlias(product));

    BSLS_ASSERT_OPT(multiplicand.base() == product->base());
    BSLS_ASSERT_OPT(multiplier.base() == product->base());

    if (multiplicand.isZero()) {
        product->reset();
        return;
    }

    if (multiplicand.isOne()) {
        *product = multiplier;
        return;
    }

    if (multiplier.isZero()) {
        product->reset();
        return;
    }

    if (multiplier.isOne()) {
        *product = multiplicand;
        return;
    }

    const bsl::uint64_t radix = product->radix();
    const bsl::uint64_t b = radix;

    const AbstractIntegerRepresentation& u = multiplicand;
    const AbstractIntegerRepresentation& v = multiplier;
    AbstractIntegerRepresentation*       w = product;

    const bsl::size_t m = u.size();
    const bsl::size_t n = v.size();

    // M1

    bsl::size_t j = 0;

    while (true) {
        // M2

        if (v.get(j) == 0) {
            w->set(j + m, 0);
            // goto M6
        }
        else {
            // M3

            bsl::size_t i = 0;
            bsl::size_t k = 0;

            while (true) {
                // M4

                bsl::size_t t = u.get(i) * v.get(j) + w->get(i + j) + k;

                w->set(i + j, t % b);
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
                    w->set(j + m, k);
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

    product->normalize();
}

void AbstractIntegerRepresentation::divide(
        AbstractIntegerRepresentation*       quotient,
        AbstractIntegerRepresentation*       remainder,
        const AbstractIntegerRepresentation& dividend,
        const AbstractIntegerRepresentation& divisor)
{
    // The following implementation is derived from algorithm D ("division of
    // non-negative integers") in section 4.3.1 of Volume 2 of "The Art of
    // Computer Programming", by Donald Knuth.

    BSLS_ASSERT_OPT(!dividend.isAlias(quotient));
    BSLS_ASSERT_OPT(!dividend.isAlias(remainder));
    BSLS_ASSERT_OPT(!divisor.isAlias(quotient));
    BSLS_ASSERT_OPT(!divisor.isAlias(remainder));

    BSLS_ASSERT_OPT(remainder->base() == quotient->base());

    BSLS_ASSERT_OPT(dividend.base() == quotient->base());
    BSLS_ASSERT_OPT(divisor.base() == quotient->base());

    quotient->reset();
    remainder->reset();

    if (dividend.isZero()) {
        return;
    }

    if (dividend.isOne()) {
        return;
    }

    if (divisor.isZero()) {
        *remainder = dividend;
        return;
    }

    if (divisor.isOne()) {
        return;
    }

    const int comparison = dividend.compare(divisor);
    if (comparison < 0) {
        *remainder = dividend;
        return;
    }

    // const bsl::uint64_t radix = quotient->radix();
    // const bsl::uint64_t b     = radix;

    NTCCFG_NOT_IMPLEMENTED();

#if 0

    BlockVector& q = quotient->d_data;
    BlockVector& r = remainder->d_data;

    // D1

    AbstractIntegerQuantity u = dividend;
    AbstractIntegerQuantity v = divisor;

    const bsl::size_t m = u.size() - v.size();
    const bsl::size_t n = v.size();

    AbstractIntegerQuantity d((b - 1) / v.get(n - 1)); // MRM: floor?

    AbstractIntegerQuantityUtil::multiply(&u, u, d);
    AbstractIntegerQuantityUtil::multiply(&v, v, d);


    // D2

    bsl::size_t j = m;

    // D3

    while (true) {
        Block x = (u[j + n] * b) + u[j + n - 1];
        Block y = v[n - 1];

        Block qc = x / y;
        Block rc = x % y;

        if ((qc == b) || (qc * v[n - 2] > b * rc + u[j + n - 2])) {
            qc += 1;
            rc += v[n - 1];

            if (rc < b) {
                continue; // goto D3
            }
        }

        break;
    }

    // D4


#endif
}


bool AbstractIntegerRepresentation::parse(
    AbstractIntegerRepresentation* result,
    AbstractIntegerSign::Value*    sign,
    const bsl::string_view&  text)
{
    result->reset();

    if (text.empty()) {
        *sign = AbstractIntegerSign::e_ZERO;
        return true;
    }

    *sign = AbstractIntegerSign::e_POSITIVE;

    int base = 10;

    bsl::string_view temp;
    {
        bsl::string_view::const_iterator it = text.begin();
        bsl::string_view::const_iterator et = text.end();

        if (it == et) {
            return false;
        }

        if (*it == '0') {
            ++it;
            if (it == et) {
                *sign = AbstractIntegerSign::e_ZERO;
                return true;
            }
            else if (*it == 'x' || *it == 'X') {
                base = 16;
                ++it;
            }
            else {
                return false;
            }
        }

        if (*it == '+') {
            if (base != 10) {
                return false;
            }

            ++it;
        }
        else if (*it == '-') {
            if (base != 10) {
                return false;
            }

            *sign = AbstractIntegerSign::e_NEGATIVE;
            ++it;
        }

        if (it == et) {
            return false;
        }

        temp = bsl::string_view(it, et);
    }

    // Vector of digits in the base, least-significant first.

    typedef bsl::uint64_t      Digit;
    typedef bsl::vector<Digit> DigitVector;

    DigitVector digits;
    {
        bsl::string_view::const_reverse_iterator it = temp.rbegin();
        bsl::string_view::const_reverse_iterator et = temp.rend();

        for (; it != et; ++it) {
            const char ch = *it;

            if (base == 10) {
                if (ch >= '0' && ch <= '9') {
                    Digit digit = ch - '0';
                    digits.push_back(digit);
                }
                else {
                    return false;
                }
            }
            else if (base == 16) {
                if (ch >= '0' && ch <= '9') {
                    Digit digit = ch - '0';
                    digits.push_back(digit);
                }
                else if (ch >= 'a' && ch <= 'f') {
                    Digit digit = ch - 'a';
                    digits.push_back(digit);
                }
                else if (ch >= 'A' && ch <= 'F') {
                    Digit digit = ch - 'A';
                    digits.push_back(digit);
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }
        }

        while (!digits.empty()) {
            if (digits.back() == 0) {
                digits.pop_back();
            }
            else {
                break;
            }
        }
    }

    BSLS_ASSERT_OPT(!digits.empty());

    {
        DigitVector::const_reverse_iterator it = digits.rbegin();
        DigitVector::const_reverse_iterator et = digits.rend();

        for (; it != et; ++it) {
            Digit value = *it;

            {
                AbstractIntegerRepresentation product(result->allocator());

                AbstractIntegerRepresentation lhs;
                lhs.assign(*result);

                AbstractIntegerRepresentation rhs;
                rhs.assign(base);

                AbstractIntegerRepresentation::multiply(&product, lhs, rhs);

                result->swap(product);
            }

            {
                AbstractIntegerRepresentation sum(result->allocator());

                AbstractIntegerRepresentation lhs;
                lhs.assign(*result);

                AbstractIntegerRepresentation rhs;
                rhs.assign(value);

                AbstractIntegerRepresentation::add(&sum, lhs, rhs);

                result->swap(sum);
            }
        }
    }

    result->normalize();

    return true;
}

void AbstractIntegerRepresentation::generate(
    bsl::string*                         result,
    const AbstractIntegerRepresentation& value,
    AbstractIntegerSign::Value           sign,
    AbstractIntegerBase::Value           base)
{
    result->clear();

    const bsl::size_t size = value.size();

    if (size == 0) {
        result->push_back('0');
    }
    else if (base == value.base()) {
        if (sign == AbstractIntegerSign::e_NEGATIVE) {
            result->push_back('-');
        }

        bsl::size_t i = size - 1;
        while (true) {
            bsl::uint64_t temp = value.get(i);

            if (temp < 10) {
                result->push_back(static_cast<char>('0' + temp));
            }
            else {
                result->push_back(static_cast<char>('a' + (temp - 10)));
            }

            if (i == 0) {
                break;
            }

            --i;
        }
    }
    else {
        typedef bsl::uint64_t      Digit;
        typedef bsl::vector<Digit> DigitVector;

        DigitVector digits;
        {
            AbstractIntegerRepresentation dividend(value);
            AbstractIntegerRepresentation divisor;
            divisor.assign(base);

            while (!dividend.isZero()) {
                AbstractIntegerRepresentation quotient;
                AbstractIntegerRepresentation remainder;

                AbstractIntegerRepresentation::divide(&quotient,
                                                      &remainder,
                                                      dividend,
                                                      divisor);

                Digit digit = static_cast<Digit>(remainder.get(0));

                digits.push_back(digit);
                dividend.swap(quotient);
            }
        }

        DigitVector::const_iterator it = digits.begin();
        DigitVector::const_iterator et = digits.end();

        for (; it != et; ++it) {
            Digit value = *it;

            if (value < 10) {
                result->push_back(static_cast<char>('0' + value));
            }
            else {
                result->push_back(static_cast<char>('a' + (value - 10)));
            }
        }

        if (sign == AbstractIntegerSign::e_NEGATIVE) {
            result->push_back('-');
        }

        bsl::reverse(result->begin(), result->end());
    }
}


bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const AbstractIntegerRepresentation& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const AbstractIntegerRepresentation& lhs,
                const AbstractIntegerRepresentation& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const AbstractIntegerRepresentation& lhs,
                const AbstractIntegerRepresentation& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const AbstractIntegerRepresentation& lhs,
               const AbstractIntegerRepresentation& rhs)
{
    return lhs.compare(rhs) < 0;
}

bool operator<=(const AbstractIntegerRepresentation& lhs,
                const AbstractIntegerRepresentation& rhs)
{
    return lhs.compare(rhs) <= 0;
}

bool operator>(const AbstractIntegerRepresentation& lhs,
               const AbstractIntegerRepresentation& rhs)
{
    return lhs.compare(rhs) > 0;
}

bool operator>=(const AbstractIntegerRepresentation& lhs,
                const AbstractIntegerRepresentation& rhs)
{
    return lhs.compare(rhs) >= 0;
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    bslma::Allocator* basicAllocator)
: d_rep(basicAllocator)
{
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    short             value,
    bslma::Allocator* basicAllocator)
: d_rep(basicAllocator)
{
    this->assign(value);
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    unsigned short    value,
    bslma::Allocator* basicAllocator)
: d_rep(basicAllocator)
{
    this->assign(value);
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    int               value,
    bslma::Allocator* basicAllocator)
: d_rep(basicAllocator)
{
    this->assign(value);
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    unsigned int      value,
    bslma::Allocator* basicAllocator)
: d_rep(basicAllocator)
{
    this->assign(value);
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    long              value,
    bslma::Allocator* basicAllocator)
: d_rep(basicAllocator)
{
    this->assign(value);
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    unsigned long     value,
    bslma::Allocator* basicAllocator)
: d_rep(basicAllocator)
{
    this->assign(value);
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    long long         value,
    bslma::Allocator* basicAllocator)
: d_rep(basicAllocator)
{
    this->assign(value);
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    unsigned long long value,
    bslma::Allocator*  basicAllocator)
: d_rep(basicAllocator)
{
    this->assign(value);
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    const AbstractIntegerQuantity& original,
    bslma::Allocator*              basicAllocator)
: d_rep(original.d_rep, basicAllocator)
{
}

AbstractIntegerQuantity::~AbstractIntegerQuantity()
{
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(
    const AbstractIntegerQuantity& other)
{
    this->assign(other);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(short value)
{
    this->assign(value);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(
    unsigned short value)
{
    this->assign(value);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(int value)
{
    this->assign(value);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(unsigned int value)
{
    this->assign(value);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(long value)
{
    this->assign(value);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(
    unsigned long value)
{
    this->assign(value);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(long long value)
{
    this->assign(value);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(
    unsigned long long value)
{
    this->assign(value);
    return *this;
}

void AbstractIntegerQuantity::reset()
{
    d_rep.reset();
}

void AbstractIntegerQuantity::resize(bsl::size_t size)
{
    d_rep.resize(size);
}

void AbstractIntegerQuantity::swap(AbstractIntegerQuantity& other)
{
    if (this != &other) {
        d_rep.swap(other.d_rep);
    }
}

bool AbstractIntegerQuantity::parse(AbstractIntegerSign::Value*    sign,
                                    const bsl::string_view& text)
{
    return AbstractIntegerRepresentation::parse(&d_rep, sign, text);
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(short value)
{
    d_rep.assign(static_cast<bsl::uint64_t>(value));
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(unsigned short value)
{
    d_rep.assign(static_cast<bsl::uint64_t>(value));
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(int value)
{
    d_rep.assign(static_cast<bsl::uint64_t>(value));
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(unsigned int value)
{
    d_rep.assign(static_cast<bsl::uint64_t>(value));
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(long value)
{
    d_rep.assign(static_cast<bsl::uint64_t>(value));
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(unsigned long value)
{
    d_rep.assign(static_cast<bsl::uint64_t>(value));
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(long long value)
{
    d_rep.assign(static_cast<bsl::uint64_t>(value));
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(
    unsigned long long value)
{
    d_rep.assign(static_cast<bsl::uint64_t>(value));
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(
    const AbstractIntegerQuantity& value)
{
    if (this != &value) {
        d_rep = value.d_rep;
    }

    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::increment()
{
    return this->add(1);
}

AbstractIntegerQuantity& AbstractIntegerQuantity::decrement()
{
    return this->subtract(1);
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(short value)
{
    return this->add(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(unsigned short value)
{
    return this->add(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(int value)
{
    return this->add(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(unsigned int value)
{
    return this->add(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(long value)
{
    return this->add(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(unsigned long value)
{
    return this->add(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(long long value)
{
    return this->add(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(unsigned long long value)
{
    return this->add(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(
    const AbstractIntegerQuantity& other)
{
    AbstractIntegerQuantityUtil::add(this, *this, other);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(short value)
{
    return this->subtract(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(
    unsigned short value)
{
    return this->subtract(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(int value)
{
    return this->subtract(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(unsigned int value)
{
    return this->subtract(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(long value)
{
    return this->subtract(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(unsigned long value)
{
    return this->subtract(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(long long value)
{
    return this->subtract(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(
    unsigned long long value)
{
    return this->subtract(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(
    const AbstractIntegerQuantity& other)
{
    AbstractIntegerQuantityUtil::subtract(this, *this, other);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(short value)
{
    return this->multiply(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(
    unsigned short value)
{
    return this->multiply(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(int value)
{
    return this->multiply(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(unsigned int value)
{
    return this->multiply(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(long value)
{
    return this->multiply(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(unsigned long value)
{
    return this->multiply(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(long long value)
{
    return this->multiply(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(
    unsigned long long value)
{
    return this->multiply(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(
    const AbstractIntegerQuantity& other)
{
    AbstractIntegerQuantityUtil::multiply(this, *this, other);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(short value)
{
    return this->divide(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(unsigned short value)
{
    return this->divide(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(int value)
{
    return this->divide(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(unsigned int value)
{
    return this->divide(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(long value)
{
    return this->divide(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(unsigned long value)
{
    return this->divide(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(long long value)
{
    return this->divide(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(
    unsigned long long value)
{
    return this->divide(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(
    const AbstractIntegerQuantity& other)
{
    ntca::AbstractIntegerQuantity remainder(d_rep.allocator());
    return this->divide(other, &remainder);
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(
    const AbstractIntegerQuantity& other,
    AbstractIntegerQuantity*       remainder)
{
    AbstractIntegerQuantityUtil::divide(this, remainder, *this, other);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(short value)
{
    return this->modulus(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(unsigned short value)
{
    return this->modulus(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(int value)
{
    return this->modulus(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(unsigned int value)
{
    return this->modulus(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(long value)
{
    return this->modulus(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(unsigned long value)
{
    return this->modulus(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(long long value)
{
    return this->modulus(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(
    unsigned long long value)
{
    return this->modulus(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(
    const AbstractIntegerQuantity& other)
{
    AbstractIntegerQuantity quotient;
    AbstractIntegerQuantityUtil::divide(&quotient, this, *this, other);
    return *this;
}

bool AbstractIntegerQuantity::equals(short value) const
{
    return this->equals(AbstractIntegerQuantity(value));
}

bool AbstractIntegerQuantity::equals(unsigned short value) const
{
    return this->equals(AbstractIntegerQuantity(value));
}

bool AbstractIntegerQuantity::equals(int value) const
{
    return this->equals(AbstractIntegerQuantity(value));
}

bool AbstractIntegerQuantity::equals(unsigned int value) const
{
    return this->equals(AbstractIntegerQuantity(value));
}

bool AbstractIntegerQuantity::equals(long value) const
{
    return this->equals(AbstractIntegerQuantity(value));
}

bool AbstractIntegerQuantity::equals(unsigned long value) const
{
    return this->equals(AbstractIntegerQuantity(value));
}

bool AbstractIntegerQuantity::equals(long long value) const
{
    return this->equals(AbstractIntegerQuantity(value));
}

bool AbstractIntegerQuantity::equals(unsigned long long value) const
{
    return this->equals(AbstractIntegerQuantity(value));
}

bool AbstractIntegerQuantity::equals(
    const AbstractIntegerQuantity& other) const
{
    return d_rep.equals(other.d_rep);
}

int AbstractIntegerQuantity::compare(short value) const
{
    return this->compare(AbstractIntegerQuantity(value));
}

int AbstractIntegerQuantity::compare(unsigned short value) const
{
    return this->compare(AbstractIntegerQuantity(value));
}

int AbstractIntegerQuantity::compare(int value) const
{
    return this->compare(AbstractIntegerQuantity(value));
}

int AbstractIntegerQuantity::compare(unsigned int value) const
{
    return this->compare(AbstractIntegerQuantity(value));
}

int AbstractIntegerQuantity::compare(long value) const
{
    return this->compare(AbstractIntegerQuantity(value));
}

int AbstractIntegerQuantity::compare(unsigned long value) const
{
    return this->compare(AbstractIntegerQuantity(value));
}

int AbstractIntegerQuantity::compare(long long value) const
{
    return this->compare(AbstractIntegerQuantity(value));
}

int AbstractIntegerQuantity::compare(unsigned long long value) const
{
    return this->compare(AbstractIntegerQuantity(value));
}

int AbstractIntegerQuantity::compare(
    const AbstractIntegerQuantity& other) const
{
    return d_rep.compare(other.d_rep);
}

ntsa::Error AbstractIntegerQuantity::convert(short* result) const
{
    const bsl::size_t size = d_rep.size();

    if (size == 0) {
        *result = 0;
        return ntsa::Error();
    }

    if (size > 1) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    const bsl::uint64_t value = d_rep.get(0);

    if (value > static_cast<bsl::uint64_t>(bsl::numeric_limits<short>::max())) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    *result = static_cast<short>(value);
    return ntsa::Error();
}

ntsa::Error AbstractIntegerQuantity::convert(unsigned short* result) const
{
    const bsl::size_t size = d_rep.size();

    if (size == 0) {
        *result = 0;
        return ntsa::Error();
    }

    if (size > 1) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    const bsl::uint64_t value = d_rep.get(0);

    if (value > static_cast<bsl::uint64_t>(bsl::numeric_limits<short>::max())) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    *result = static_cast<short>(value);
    return ntsa::Error();
}

ntsa::Error AbstractIntegerQuantity::convert(int* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractIntegerQuantity::convert(unsigned int* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractIntegerQuantity::convert(long* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractIntegerQuantity::convert(unsigned long* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractIntegerQuantity::convert(long long* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractIntegerQuantity::convert(unsigned long long* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

void AbstractIntegerQuantity::generate(bsl::string* result,
                                       AbstractIntegerSign::Value sign,
                                       AbstractIntegerBase::Value base) const
{
    AbstractIntegerRepresentation::generate(result, d_rep, sign, base);
}

bsl::size_t AbstractIntegerQuantity::size() const
{
    return d_rep.size();
}

AbstractIntegerBase::Value AbstractIntegerQuantity::base() const
{
    return d_rep.base();
}

bsl::uint64_t AbstractIntegerQuantity::radix() const
{
    return d_rep.radix();
}

bool AbstractIntegerQuantity::isZero() const
{
    return d_rep.isZero();
}

bool AbstractIntegerQuantity::isOne() const
{
    return d_rep.isOne();
}

bool AbstractIntegerQuantity::isAlias(
    const AbstractIntegerQuantity* other) const
{
    return this == other;
}

bool AbstractIntegerQuantity::isAlias(
    const AbstractIntegerQuantity& other) const
{
    return this == &other;
}

bslma::Allocator* AbstractIntegerQuantity::allocator() const
{
    return d_rep.allocator();
}

bsl::ostream& AbstractIntegerQuantity::print(bsl::ostream& stream,
                                             int           level,
                                             int spacesPerLevel) const
{
    NTCCFG_WARNING_UNUSED(level);
    NTCCFG_WARNING_UNUSED(spacesPerLevel);

    bsl::string result;
    AbstractIntegerBase::Value base = AbstractIntegerBase::e_DECIMAL;

    const bsl::ostream::fmtflags flags = stream.flags();

    if ((flags & std::ios_base::hex) != 0) {
        base = AbstractIntegerBase::e_HEXADECIMAL;
    }
    else if ((flags & std::ios_base::oct) != 0) {
        base = AbstractIntegerBase::e_OCTAL;
    }

    this->generate(&result, AbstractIntegerSign::e_POSITIVE, base);
    stream << result;
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const AbstractIntegerQuantity& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const AbstractIntegerQuantity& lhs,
                const AbstractIntegerQuantity& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const AbstractIntegerQuantity& lhs,
                const AbstractIntegerQuantity& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const AbstractIntegerQuantity& lhs,
               const AbstractIntegerQuantity& rhs)
{
    return lhs.compare(rhs) < 0;
}

bool operator<=(const AbstractIntegerQuantity& lhs,
                const AbstractIntegerQuantity& rhs)
{
    return lhs.compare(rhs) <= 0;
}

bool operator>(const AbstractIntegerQuantity& lhs,
               const AbstractIntegerQuantity& rhs)
{
    return lhs.compare(rhs) > 0;
}

bool operator>=(const AbstractIntegerQuantity& lhs,
                const AbstractIntegerQuantity& rhs)
{
    return lhs.compare(rhs) >= 0;
}

void AbstractIntegerQuantityUtil::add(AbstractIntegerQuantity*       sum,
                                      const AbstractIntegerQuantity& addend1,
                                      const AbstractIntegerQuantity& addend2)
{
    if (!addend1.isAlias(sum) && !addend2.isAlias(sum)) {
        AbstractIntegerRepresentation::add(&sum->d_rep, addend1.d_rep, addend2.d_rep);
    }
    else {
        AbstractIntegerQuantity tempSum(sum->allocator());
        AbstractIntegerRepresentation::add(&tempSum.d_rep, addend1.d_rep, addend2.d_rep);
        sum->swap(tempSum);
    }
}

void AbstractIntegerQuantityUtil::subtract(
    AbstractIntegerQuantity*       difference,
    const AbstractIntegerQuantity& minuend,
    const AbstractIntegerQuantity& subtrahend)
{
    if (!minuend.isAlias(difference) && !subtrahend.isAlias(difference)) {
        AbstractIntegerRepresentation::subtract(&difference->d_rep, minuend.d_rep, subtrahend.d_rep);
    }
    else {
        AbstractIntegerQuantity tempDifference(difference->allocator());
        AbstractIntegerRepresentation::subtract(&tempDifference.d_rep,
                                              minuend.d_rep,
                                              subtrahend.d_rep);
        difference->swap(tempDifference);
    }
}

void AbstractIntegerQuantityUtil::multiply(
    AbstractIntegerQuantity*       product,
    const AbstractIntegerQuantity& multiplicand,
    const AbstractIntegerQuantity& multiplier)
{
    if (!multiplicand.isAlias(product) && !multiplier.isAlias(product)) {
        AbstractIntegerRepresentation::multiply(&product->d_rep,
                                              multiplicand.d_rep,
                                              multiplier.d_rep);
    }
    else {
        AbstractIntegerQuantity tempProduct(product->allocator());
        AbstractIntegerRepresentation::multiply(&tempProduct.d_rep,
                                              multiplicand.d_rep,
                                              multiplier.d_rep);
        product->swap(tempProduct);
    }
}

void AbstractIntegerQuantityUtil::divide(
    AbstractIntegerQuantity*       quotient,
    AbstractIntegerQuantity*       remainder,
    const AbstractIntegerQuantity& dividend,
    const AbstractIntegerQuantity& divisor)
{
    bdlb::NullableValue<AbstractIntegerQuantity> defaultQuotient;
    bdlb::NullableValue<AbstractIntegerQuantity> defaultRemainder;

    if (!quotient) {
        quotient = &defaultQuotient.makeValue();
    }

    if (!remainder) {
        remainder = &defaultRemainder.makeValue();
    }

    if (!dividend.isAlias(quotient) && !dividend.isAlias(remainder) &&
        !divisor.isAlias(quotient) && !divisor.isAlias(remainder))
    {
        AbstractIntegerRepresentation::divide(&quotient->d_rep,
                                            &remainder->d_rep,
                                            dividend.d_rep,
                                            divisor.d_rep);
    }
    else {
        AbstractIntegerQuantity tempQuotient(quotient->allocator());
        AbstractIntegerQuantity tempRemainder(remainder->allocator());
        AbstractIntegerRepresentation::divide(&tempQuotient.d_rep,
                                            &tempRemainder.d_rep,
                                            dividend.d_rep,
                                            divisor.d_rep);
        quotient->swap(tempQuotient);
        remainder->swap(tempRemainder);
    }
}

void AbstractInteger::privateAssign(BlockVector*       result,
                                    const BlockVector& other)
{
    BSLS_ASSERT_OPT(&other != result);

    *result = other;
}

void AbstractInteger::privateAssign(BlockVector* result, bsl::size_t value)
{
    BSLS_ASSERT_OPT(value <= bsl::numeric_limits<Block>::max());

    result->clear();
    result->resize(1);
    result->front() = static_cast<Block>(value);
}

void AbstractInteger::privateKeep(Sign* result, Sign other)
{
    BSLS_ASSERT_OPT(&other != result);

    *result = other;
}

void AbstractInteger::privateFlip(Sign* result, Sign other)
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

void AbstractInteger::privateIncrement(BlockVector*       sum,
                                       const BlockVector& addend)
{
    BlockVector value(addend.get_allocator());
    AbstractInteger::privateAssign(&value, 1);
    AbstractInteger::privateAdd(sum, addend, value);
}

void AbstractInteger::privateDecrement(BlockVector*       difference,
                                       const BlockVector& minuend)
{
    BlockVector value(minuend.get_allocator());
    AbstractInteger::privateAssign(&value, 1);
    AbstractInteger::privateSubtract(difference, minuend, value);
}

void AbstractInteger::privateAdd(BlockVector*       sum,
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

    AbstractInteger::privateTrim(&result);
    sum->swap(result);
}

void AbstractInteger::privateSubtract(BlockVector*       difference,
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

    AbstractInteger::privateTrim(&result);
    difference->swap(result);
}

void AbstractInteger::privateMultiply(BlockVector*       product,
                                      const BlockVector& multiplicand,
                                      const BlockVector& multiplier)
{
    BSLS_ASSERT_OPT(product);

    BlockVector result(product->get_allocator());

    const bsl::size_t multiplicandSize = multiplicand.size();
    const bsl::size_t multiplierSize   = multiplier.size();

    if (multiplicandSize != 0 && multiplierSize != 0) {
        const BlockVector& u = multiplicand;
        const BlockVector& v = multiplier;
        BlockVector&       w = result;

        const bsl::size_t m = multiplicandSize;
        const bsl::size_t n = multiplierSize;

        const bsl::size_t b = bsl::numeric_limits<Block>::max();  // radix

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
                    k        = t / b;

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

    AbstractInteger::privateTrim(&result);
    product->swap(result);
}

void AbstractInteger::privateDivide(BlockVector*       quotient,
                                    BlockVector*       remainder,
                                    const BlockVector& dividend,
                                    const BlockVector& divisor)
{
    NTCCFG_WARNING_UNUSED(quotient);
    NTCCFG_WARNING_UNUSED(remainder);
    NTCCFG_WARNING_UNUSED(dividend);
    NTCCFG_WARNING_UNUSED(divisor);

    BSLS_ASSERT_OPT(!"Not implemented");
}

void AbstractInteger::privateMultiplyByAddition(
    BlockVector*       product,
    const BlockVector& multiplicand,
    const BlockVector& multiplier)
{
    BSLS_ASSERT_OPT(product);

    BlockVector result(product->get_allocator());

    const bsl::size_t multiplicandSize = multiplicand.size();
    const bsl::size_t multiplierSize   = multiplier.size();

    if (multiplicandSize != 0 && multiplierSize != 0) {
        BSLS_ASSERT(!"Not implemented");
    }

    AbstractInteger::privateTrim(&result);
    product->swap(result);
}

void AbstractInteger::privateMultiplyByAddition(
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
            AbstractInteger::privateAdd(&temp, result, multiplicand);
            result.swap(temp);
        }
    }

    AbstractInteger::privateTrim(&result);
    product->swap(result);
}

void AbstractInteger::privateDivideBySubtraction(BlockVector*       quotient,
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

void AbstractInteger::privateDivideBySubtraction(BlockVector*       quotient,
                                                 BlockVector*       remainder,
                                                 const BlockVector& dividend,
                                                 bsl::size_t        divisor)
{
    BSLS_ASSERT_OPT(quotient);
    BSLS_ASSERT_OPT(remainder);

    const BlockVectorAllocator& allocator = quotient->get_allocator();

    BlockVector iterations(allocator);

    BlockVector minuend(allocator);
    AbstractInteger::privateAssign(&minuend, dividend);

    BlockVector subtrahend(allocator);
    AbstractInteger::privateAssign(&subtrahend, divisor);

    BlockVector counter(allocator);
    AbstractInteger::privateAssign(&counter, 1);

    const bsl::size_t dividendSize = dividend.size();

    if (dividendSize != 0 && divisor != 0) {
        while (true) {
            const int comparison =
                AbstractInteger::privateCompare(minuend, subtrahend);

            if (comparison < 0) {
                break;
            }

            BlockVector difference(allocator);
            AbstractInteger::privateSubtract(&difference, minuend, subtrahend);
            difference.swap(minuend);

            {
                bsl::stringstream ss;
                bslim::Printer    printer(&ss, 0, -1);
                printer.start();
                printer.printAttribute("dividend", minuend);
                printer.end();

                BSLS_LOG_INFO("%s", ss.str().c_str());
            }

            AbstractInteger::privateAdd(&iterations, iterations, counter);
        }

        // MRM
        // bigint result = 0, temp = *this;
        // while (temp >= that) {
        //     temp -= that;
        //     ++result;
        // }
        // return result;
    }

    AbstractInteger::privateTrim(&iterations);
    quotient->swap(iterations);

    AbstractInteger::privateTrim(&minuend);
    remainder->swap(minuend);
}

int AbstractInteger::privateCompare(const BlockVector& lhs,
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

void AbstractInteger::privateTrim(BlockVector* result)
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

bool AbstractInteger::privateIsZero(const BlockVector& value)
{
    return value.empty() || (value.size() == 1 && value.front() == 0);
}

bool AbstractInteger::privateIsOne(const BlockVector& value)
{
    return value.size() == 1 && value.front() == 1;
}

AbstractInteger::AbstractInteger(bslma::Allocator* basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

AbstractInteger::AbstractInteger(short value, bslma::Allocator* basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->assign(value);
}

AbstractInteger::AbstractInteger(unsigned short    value,
                                 bslma::Allocator* basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->assign(value);
}

AbstractInteger::AbstractInteger(int value, bslma::Allocator* basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->assign(value);
}

AbstractInteger::AbstractInteger(unsigned int      value,
                                 bslma::Allocator* basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->assign(value);
}

AbstractInteger::AbstractInteger(long value, bslma::Allocator* basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->assign(value);
}

AbstractInteger::AbstractInteger(unsigned long     value,
                                 bslma::Allocator* basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->assign(value);
}

AbstractInteger::AbstractInteger(long long         value,
                                 bslma::Allocator* basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->assign(value);
}

AbstractInteger::AbstractInteger(unsigned long long value,
                                 bslma::Allocator*  basicAllocator)
: d_sign(e_ZERO)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->assign(value);
}

AbstractInteger::AbstractInteger(const AbstractInteger& original,
                                 bslma::Allocator*      basicAllocator)
: d_sign(original.d_sign)
, d_data(original.d_data, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

AbstractInteger::~AbstractInteger()
{
}

AbstractInteger& AbstractInteger::operator=(const AbstractInteger& other)
{
    if (this != &other) {
        d_sign = other.d_sign;
        d_data = other.d_data;
    }

    return *this;
}

void AbstractInteger::reset()
{
    d_sign = e_ZERO;
    d_data.clear();
}

void AbstractInteger::swap(AbstractInteger& other)
{
    if (this != &other) {
        bsl::swap(d_sign, other.d_sign);
        d_data.swap(other.d_data);
    }
}

ntsa::Error AbstractInteger::parse(const bsl::string_view& text)
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

        AbstractInteger::privateTrim(&digits);
    }

    BSLS_ASSERT_OPT(!digits.empty());

    AbstractInteger result(this->allocator());
    {
        BlockVector::const_reverse_iterator it = digits.rbegin();
        BlockVector::const_reverse_iterator et = digits.rend();

        for (; it != et; ++it) {
            Block value = *it;
            // result.multiply(base);
            {
                AbstractInteger addend(result);
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

AbstractInteger& AbstractInteger::assign(short value)
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

AbstractInteger& AbstractInteger::assign(unsigned short value)
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

AbstractInteger& AbstractInteger::assign(int value)
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

AbstractInteger& AbstractInteger::assign(unsigned int value)
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

AbstractInteger& AbstractInteger::assign(long value)
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

AbstractInteger& AbstractInteger::assign(unsigned long value)
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

AbstractInteger& AbstractInteger::assign(long long value)
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

AbstractInteger& AbstractInteger::assign(unsigned long long value)
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

AbstractInteger& AbstractInteger::assign(const AbstractInteger& value)
{
    if (this != &value) {
        d_sign = value.d_sign;
        d_data = value.d_data;
    }

    return *this;
}

AbstractInteger& AbstractInteger::negate()
{
    if (d_sign == e_POSITIVE) {
        d_sign = e_NEGATIVE;
    }
    else if (d_sign == e_NEGATIVE) {
        d_sign = e_POSITIVE;
    }

    return *this;
}

AbstractInteger& AbstractInteger::increment()
{
    return this->add(1);
}

AbstractInteger& AbstractInteger::decrement()
{
    return this->subtract(1);
}

AbstractInteger& AbstractInteger::add(short value)
{
    return this->add(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::add(unsigned short value)
{
    return this->add(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::add(int value)
{
    return this->add(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::add(unsigned int value)
{
    return this->add(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::add(long value)
{
    return this->add(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::add(unsigned long value)
{
    return this->add(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::add(long long value)
{
    return this->add(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::add(unsigned long long value)
{
    return this->add(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::add(const AbstractInteger& other)
{
    const AbstractInteger& lhs = *this;
    const AbstractInteger& rhs = other;

    if (lhs.d_sign == e_ZERO) {
        AbstractInteger result(d_allocator_p);
        AbstractInteger::privateAssign(&result.d_data, rhs.d_data);
        AbstractInteger::privateKeep(&result.d_sign, rhs.d_sign);
        this->swap(result);
        return *this;
    }
    else if (rhs.d_sign == e_ZERO) {
        return *this;
    }
    else if (lhs.d_sign == rhs.d_sign) {
        AbstractInteger result(d_allocator_p);
        AbstractInteger::privateAdd(&result.d_data, lhs.d_data, rhs.d_data);
        AbstractInteger::privateKeep(&result.d_sign, lhs.d_sign);
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
            AbstractInteger result(d_allocator_p);
            AbstractInteger::privateSubtract(&result.d_data,
                                             lhs.d_data,
                                             rhs.d_data);
            AbstractInteger::privateKeep(&result.d_sign, lhs.d_sign);
            this->swap(result);
            return *this;
        }
        else {
            AbstractInteger result(d_allocator_p);
            AbstractInteger::privateSubtract(&result.d_data,
                                             rhs.d_data,
                                             lhs.d_data);
            AbstractInteger::privateKeep(&result.d_sign, rhs.d_sign);
            this->swap(result);
            return *this;
        }
    }
}

AbstractInteger& AbstractInteger::subtract(short value)
{
    return this->subtract(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::subtract(unsigned short value)
{
    return this->subtract(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::subtract(int value)
{
    return this->subtract(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::subtract(unsigned int value)
{
    return this->subtract(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::subtract(long value)
{
    return this->subtract(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::subtract(unsigned long value)
{
    return this->subtract(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::subtract(long long value)
{
    return this->subtract(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::subtract(unsigned long long value)
{
    return this->subtract(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::subtract(const AbstractInteger& other)
{
    const AbstractInteger& lhs = *this;
    const AbstractInteger& rhs = other;

    if (lhs.d_sign == e_ZERO) {
        AbstractInteger result(d_allocator_p);
        AbstractInteger::privateAssign(&result.d_data, rhs.d_data);
        AbstractInteger::privateFlip(&result.d_sign, rhs.d_sign);
        this->swap(result);
        return *this;
    }
    else if (rhs.d_sign == e_ZERO) {
        return *this;
    }
    else if (lhs.d_sign != rhs.d_sign) {
        AbstractInteger result(d_allocator_p);
        AbstractInteger::privateAdd(&result.d_data, lhs.d_data, rhs.d_data);
        AbstractInteger::privateKeep(&result.d_sign, lhs.d_sign);
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
            AbstractInteger result(d_allocator_p);
            AbstractInteger::privateSubtract(&result.d_data,
                                             lhs.d_data,
                                             rhs.d_data);
            AbstractInteger::privateKeep(&result.d_sign, lhs.d_sign);
            this->swap(result);
            return *this;
        }
        else {
            AbstractInteger result(rhs, d_allocator_p);
            AbstractInteger::privateSubtract(&result.d_data,
                                             rhs.d_data,
                                             lhs.d_data);
            AbstractInteger::privateFlip(&result.d_sign, rhs.d_sign);
            this->swap(result);
            return *this;
        }
    }
}

AbstractInteger& AbstractInteger::multiply(short value)
{
    return this->multiply(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::multiply(unsigned short value)
{
    return this->multiply(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::multiply(int value)
{
    return this->multiply(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::multiply(unsigned int value)
{
    return this->multiply(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::multiply(long value)
{
    return this->multiply(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::multiply(unsigned long value)
{
    return this->multiply(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::multiply(long long value)
{
    return this->multiply(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::multiply(unsigned long long value)
{
    return this->multiply(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::multiply(const AbstractInteger& other)
{
    const AbstractInteger& lhs = *this;
    const AbstractInteger& rhs = other;

    if (lhs.d_sign == e_ZERO || rhs.d_sign == e_ZERO) {
        this->reset();
        return *this;
    }

    AbstractInteger result(d_allocator_p);

    AbstractInteger::privateMultiply(&result.d_data, lhs.d_data, rhs.d_data);
    result.d_sign = lhs.d_sign == rhs.d_sign ? e_POSITIVE : e_NEGATIVE;

    this->swap(result);
    return *this;
}

AbstractInteger& AbstractInteger::divide(short value)
{
    return this->divide(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::divide(unsigned short value)
{
    return this->divide(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::divide(int value)
{
    return this->divide(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::divide(unsigned int value)
{
    return this->divide(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::divide(long value)
{
    return this->divide(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::divide(unsigned long value)
{
    return this->divide(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::divide(long long value)
{
    return this->divide(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::divide(unsigned long long value)
{
    return this->divide(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::divide(const AbstractInteger& other)
{
    ntca::AbstractInteger remainder(d_allocator_p);
    return this->divide(other, &remainder);
}

AbstractInteger& AbstractInteger::divide(const AbstractInteger& other,
                                         AbstractInteger*       remainder)
{
    const AbstractInteger& lhs = *this;
    const AbstractInteger& rhs = other;

    remainder->reset();

    if (lhs.isZero()) {
        this->reset();
        remainder->reset();
        return *this;
    }

    if (rhs.isZero()) {
        this->reset();
        remainder->assign(*this);
        return *this;
    }

    AbstractInteger result(d_allocator_p);

    if (d_sign == other.d_sign) {
        AbstractInteger::privateDivide(&result.d_data,
                                       &remainder->d_data,
                                       lhs.d_data,
                                       rhs.d_data);
        result.d_sign = e_POSITIVE;
    }
    else {
        AbstractInteger        quotient(d_allocator_p);
        AbstractInteger        dividend(*this, d_allocator_p);
        const AbstractInteger& divisor = other;

        AbstractInteger::privateDecrement(&dividend.d_data, dividend.d_data);

        AbstractInteger::privateDivide(&quotient.d_data,
                                       &remainder->d_data,
                                       dividend.d_data,
                                       divisor.d_data);

        AbstractInteger::privateIncrement(&quotient.d_data, quotient.d_data);

        AbstractInteger::privateSubtract(&remainder->d_data,
                                         divisor.d_data,
                                         remainder->d_data);

        AbstractInteger::privateDecrement(&remainder->d_data,
                                          remainder->d_data);

        remainder->d_sign = divisor.d_sign;

        quotient.d_sign = e_NEGATIVE;

        AbstractInteger::privateIncrement(&quotient.d_data, quotient.d_data);

        if (AbstractInteger::privateIsZero(remainder->d_data)) {
            remainder->d_sign = e_ZERO;
        }

        if (AbstractInteger::privateIsZero(quotient.d_data)) {
            quotient.d_sign = e_ZERO;
        }
    }

    this->swap(result);
    return *this;
}

AbstractInteger& AbstractInteger::modulus(short value)
{
    return this->modulus(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::modulus(unsigned short value)
{
    return this->modulus(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::modulus(int value)
{
    return this->modulus(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::modulus(unsigned int value)
{
    return this->modulus(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::modulus(long value)
{
    return this->modulus(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::modulus(unsigned long value)
{
    return this->modulus(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::modulus(long long value)
{
    return this->modulus(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::modulus(unsigned long long value)
{
    return this->modulus(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::modulus(const AbstractInteger& other)
{
    NTCCFG_WARNING_UNUSED(other);
    BSLS_ASSERT_OPT(!"Not implemented");
    return *this;
}

bool AbstractInteger::equals(short value) const
{
    return this->equals(AbstractInteger(value));
}

bool AbstractInteger::equals(unsigned short value) const
{
    return this->equals(AbstractInteger(value));
}

bool AbstractInteger::equals(int value) const
{
    return this->equals(AbstractInteger(value));
}

bool AbstractInteger::equals(unsigned int value) const
{
    return this->equals(AbstractInteger(value));
}

bool AbstractInteger::equals(long value) const
{
    return this->equals(AbstractInteger(value));
}

bool AbstractInteger::equals(unsigned long value) const
{
    return this->equals(AbstractInteger(value));
}

bool AbstractInteger::equals(long long value) const
{
    return this->equals(AbstractInteger(value));
}

bool AbstractInteger::equals(unsigned long long value) const
{
    return this->equals(AbstractInteger(value));
}

bool AbstractInteger::equals(const AbstractInteger& other) const
{
    return d_sign == other.d_sign && d_data == other.d_data;
}

int AbstractInteger::compare(short value) const
{
    return this->compare(AbstractInteger(value));
}

int AbstractInteger::compare(unsigned short value) const
{
    return this->compare(AbstractInteger(value));
}

int AbstractInteger::compare(int value) const
{
    return this->compare(AbstractInteger(value));
}

int AbstractInteger::compare(unsigned int value) const
{
    return this->compare(AbstractInteger(value));
}

int AbstractInteger::compare(long value) const
{
    return this->compare(AbstractInteger(value));
}

int AbstractInteger::compare(unsigned long value) const
{
    return this->compare(AbstractInteger(value));
}

int AbstractInteger::compare(long long value) const
{
    return this->compare(AbstractInteger(value));
}

int AbstractInteger::compare(unsigned long long value) const
{
    return this->compare(AbstractInteger(value));
}

int AbstractInteger::compare(const AbstractInteger& other) const
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
        result = AbstractInteger::privateCompare(d_data, other.d_data) *
                 static_cast<int>(lhsSign);
    }

    return result;
}

ntsa::Error AbstractInteger::convert(short* result) const
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

ntsa::Error AbstractInteger::convert(unsigned short* result) const
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

ntsa::Error AbstractInteger::convert(int* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractInteger::convert(unsigned int* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractInteger::convert(long* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractInteger::convert(unsigned long* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractInteger::convert(long long* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractInteger::convert(unsigned long long* result) const
{
    NTCCFG_WARNING_UNUSED(result);
    BSLS_ASSERT_OPT(!"Not implemented");
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

void AbstractInteger::generate(bsl::string* result, int base) const
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
        AbstractInteger::privateAssign(&dividend, d_data);

        const bsl::size_t divisor = static_cast<bsl::size_t>(base);

        while (!AbstractInteger::privateIsZero(dividend)) {
            BlockVector quotient(d_allocator_p);
            BlockVector remainder(d_allocator_p);

            AbstractInteger::privateDivideBySubtraction(&quotient,
                                                        &remainder,
                                                        dividend,
                                                        divisor);

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
    }

    if (d_sign == e_NEGATIVE) {
        result->push_back('-');
    }

    bsl::reverse(result->begin(), result->end());
}

bool AbstractInteger::isZero() const
{
    return d_sign == e_ZERO;
}

bool AbstractInteger::isPositive() const
{
    return d_sign == e_POSITIVE;
}

bool AbstractInteger::isNegative() const
{
    return d_sign == e_NEGATIVE;
}

bslma::Allocator* AbstractInteger::allocator() const
{
    return d_allocator_p;
}

bsl::ostream& AbstractInteger::print(bsl::ostream& stream,
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

AbstractInteger AbstractInteger::operator+(const AbstractInteger& other) const
{
    AbstractInteger result(*this);
    result.add(other);
    return result;
}

AbstractInteger AbstractInteger::operator-(const AbstractInteger& other) const
{
    AbstractInteger result(*this);
    result.subtract(other);
    return result;
}

AbstractInteger AbstractInteger::operator*(const AbstractInteger& other) const
{
    AbstractInteger result(*this);
    result.multiply(other);
    return result;
}

AbstractInteger AbstractInteger::operator/(const AbstractInteger& other) const
{
    AbstractInteger result(*this);
    result.divide(other);
    return result;
}

AbstractInteger AbstractInteger::operator%(const AbstractInteger& other) const
{
    AbstractInteger result(*this);
    result.modulus(other);
    return result;
}

AbstractInteger AbstractInteger::operator-() const
{
    AbstractInteger result(*this);
    result.negate();
    return result;
}

AbstractInteger& AbstractInteger::operator+=(const AbstractInteger& other)
{
    return this->add(other);
}

AbstractInteger& AbstractInteger::operator-=(const AbstractInteger& other)
{
    return this->subtract(other);
}

AbstractInteger& AbstractInteger::operator*=(const AbstractInteger& other)
{
    return this->multiply(other);
}

AbstractInteger& AbstractInteger::operator/=(const AbstractInteger& other)
{
    return this->divide(other);
}

AbstractInteger& AbstractInteger::operator%=(const AbstractInteger& other)
{
    return this->modulus(other);
}

AbstractInteger& AbstractInteger::operator++()
{
    return this->increment();
}

AbstractInteger AbstractInteger::operator++(int)
{
    AbstractInteger result(*this);
    result.increment();
    return result;
}

AbstractInteger& AbstractInteger::operator--()
{
    return this->decrement();
}

AbstractInteger AbstractInteger::operator--(int)
{
    AbstractInteger result(*this);
    result.decrement();
    return result;
}

bsl::ostream& operator<<(bsl::ostream& stream, const AbstractInteger& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const AbstractInteger& lhs, const AbstractInteger& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const AbstractInteger& lhs, const AbstractInteger& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const AbstractInteger& lhs, const AbstractInteger& rhs)
{
    return lhs.compare(rhs) < 0;
}

bool operator<=(const AbstractInteger& lhs, const AbstractInteger& rhs)
{
    return lhs.compare(rhs) <= 0;
}

bool operator>(const AbstractInteger& lhs, const AbstractInteger& rhs)
{
    return lhs.compare(rhs) > 0;
}

bool operator>=(const AbstractInteger& lhs, const AbstractInteger& rhs)
{
    return lhs.compare(rhs) >= 0;
}

}  // close package namespace
}  // close enterprise namespace
