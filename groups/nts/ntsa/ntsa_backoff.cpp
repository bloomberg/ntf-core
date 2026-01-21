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

#include <ntsa_backoff.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_backoff_cpp, "$Id$ $CSID$")

#include <bdlb_doublecompareutil.h>
#include <bslim_printer.h>
#include <bsl_c_math.h>

namespace BloombergLP {
namespace ntsa {

Backoff::Backoff()
: d_type(e_UNDEFINED)
, d_minJitter()
, d_maxJitter()
, d_minLimit()
, d_maxLimit()
{
}

Backoff::Backoff(const Backoff& original)
: d_type(original.d_type)
, d_minJitter(original.d_minJitter)
, d_maxJitter(original.d_maxJitter)
, d_minLimit(original.d_minLimit)
, d_maxLimit(original.d_maxLimit)
{
    if (d_type == e_ARITHMETIC) {
        new (d_rep.d_arithmetic.buffer())
            ArithmeticType(original.d_rep.d_arithmetic.object());
    }
    else if (d_type == e_GEOMETRIC) {
        new (d_rep.d_geometric.buffer())
            GeometricType(original.d_rep.d_geometric.object());
    }
}

Backoff::Backoff(bslmf::MovableRef<Backoff> original)
: d_type(NTSCFG_MOVE_FROM(original, d_type))
, d_minJitter(NTSCFG_MOVE_FROM(original, d_minJitter))
, d_maxJitter(NTSCFG_MOVE_FROM(original, d_maxJitter))
, d_minLimit(NTSCFG_MOVE_FROM(original, d_minLimit))
, d_maxLimit(NTSCFG_MOVE_FROM(original, d_maxLimit))
{
    if (d_type == e_ARITHMETIC) {
        new (d_rep.d_arithmetic.buffer()) ArithmeticType(
            NTSCFG_MOVE_FROM(original, d_rep.d_arithmetic.object()));
    }
    else if (d_type == e_GEOMETRIC) {
        new (d_rep.d_geometric.buffer()) GeometricType(
            NTSCFG_MOVE_FROM(original, d_rep.d_geometric.object()));
    }

    NTSCFG_MOVE_RESET(original);
}

Backoff::~Backoff()
{
}

Backoff& Backoff::operator=(const Backoff& other)
{
    if (this != &other) {
        this->reset();

        d_type = other.d_type;

        if (d_type == e_ARITHMETIC) {
            new (d_rep.d_arithmetic.buffer())
                ArithmeticType(other.d_rep.d_arithmetic.object());
        }
        else if (d_type == e_GEOMETRIC) {
            new (d_rep.d_geometric.buffer())
                GeometricType(other.d_rep.d_geometric.object());
        }

        d_minJitter = other.d_minJitter;
        d_maxJitter = other.d_maxJitter;
        d_minLimit  = other.d_minLimit;
        d_maxLimit  = other.d_maxLimit;
    }

    return *this;
}

Backoff& Backoff::operator=(bslmf::MovableRef<Backoff> other)
{
    this->reset();

    d_type = NTSCFG_MOVE_FROM(other, d_type);

    if (d_type == e_ARITHMETIC) {
        new (d_rep.d_arithmetic.buffer()) ArithmeticType(
            NTSCFG_MOVE_FROM(other, d_rep.d_arithmetic.object()));
    }
    else if (d_type == e_GEOMETRIC) {
        new (d_rep.d_geometric.buffer())
            GeometricType(NTSCFG_MOVE_FROM(other, d_rep.d_geometric.object()));
    }

    d_minJitter = NTSCFG_MOVE_FROM(other, d_minJitter);
    d_maxJitter = NTSCFG_MOVE_FROM(other, d_maxJitter);
    d_minLimit  = NTSCFG_MOVE_FROM(other, d_minLimit);
    d_maxLimit  = NTSCFG_MOVE_FROM(other, d_maxLimit);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

void Backoff::reset()
{
    if (d_type == e_ARITHMETIC) {
        d_rep.d_arithmetic.object().~ArithmeticType();
    }
    else if (d_type == e_GEOMETRIC) {
        d_rep.d_geometric.object().~GeometricType();
    }

    d_type      = e_UNDEFINED;
    d_minJitter = bsls::TimeInterval();
    d_maxJitter = bsls::TimeInterval();
    d_minLimit.reset();
    d_maxLimit.reset();
}

void Backoff::makeArithmetic(const bsls::TimeInterval& value)
{
    if (d_type == e_ARITHMETIC) {
        d_rep.d_arithmetic.object() = value;
    }
    else {
        if (d_type == e_GEOMETRIC) {
            d_rep.d_geometric.object().~GeometricType();
        }

        new (d_rep.d_arithmetic.buffer()) ArithmeticType(value);
        d_type = e_ARITHMETIC;
    }
}

void Backoff::makeGeometric(double value)
{
    if (d_type == e_GEOMETRIC) {
        d_rep.d_geometric.object() = value;
    }
    else {
        if (d_type == e_ARITHMETIC) {
            d_rep.d_arithmetic.object().~ArithmeticType();
        }

        new (d_rep.d_geometric.buffer()) GeometricType(value);
        d_type = e_GEOMETRIC;
    }
}

void Backoff::setMinJitter(const bsls::TimeInterval& value)
{
    d_minJitter = value;
}

void Backoff::setMaxJitter(const bsls::TimeInterval& value)
{
    d_maxJitter = value;
}

void Backoff::setMinLimit(const bsls::TimeInterval& value)
{
    d_minLimit = value;
}

void Backoff::setMaxLimit(const bsls::TimeInterval& value)
{
    d_maxLimit = value;
}

const bsls::TimeInterval& Backoff::arithmetic() const
{
    BSLS_ASSERT(d_type == e_ARITHMETIC);
    return d_rep.d_arithmetic.object();
}

double Backoff::geometric() const
{
    BSLS_ASSERT(d_type == e_GEOMETRIC);
    return d_rep.d_geometric.object();
}

const bsls::TimeInterval& Backoff::minJitter() const
{
    return d_minJitter;
}

const bsls::TimeInterval& Backoff::maxJitter() const
{
    return d_maxJitter;
}

const bdlb::NullableValue<bsls::TimeInterval>& Backoff::minLimit() const
{
    return d_minLimit;
}

const bdlb::NullableValue<bsls::TimeInterval>& Backoff::maxLimit() const
{
    return d_maxLimit;
}

bsls::TimeInterval Backoff::apply(const bsls::TimeInterval& value) const
{
    using namespace bsl;

    bsls::TimeInterval result;

    const bsls::Types::Int64 initial = value.totalMicroseconds();

    bsls::Types::Int64 next = 0;

    if (d_type == e_UNDEFINED) {
        next = initial;
    }
    else if (d_type == e_ARITHMETIC) {
        const bsls::TimeInterval arithmetic = d_rep.d_arithmetic.object();
        next = initial + arithmetic.totalMicroseconds();
    }
    else if (d_type == e_GEOMETRIC) {
        const double geometric = d_rep.d_geometric.object();
        next = static_cast<bsls::Types::Int64>(initial * geometric);
    }
    else {
        BSLA_UNREACHABLE;
    }

    const bsls::Types::Int64 jitterMin = d_minJitter.totalMicroseconds();
    const bsls::Types::Int64 jitterMax = d_maxJitter.totalMicroseconds();

    const bsls::Types::Int64 jitterRange =
        jitterMax > jitterMin ? jitterMax - jitterMin : 0;

    const double jitterFactor = 
        static_cast<double>(bsl::rand()) / static_cast<double>(RAND_MAX);

    const bsls::Types::Int64 jitterAmount = 
        static_cast<bsls::Types::Int64>(jitterFactor * jitterRange);

    const bsls::Types::Int64 jitter = jitterMin + jitterAmount;

    result.addMicroseconds(next + jitter);

    if (d_minLimit.has_value()) {
        if (result < d_minLimit.value()) {
            result = d_minLimit.value();
        }
    }

    if (d_maxLimit.has_value()) {
        if (result > d_maxLimit.value()) {
            result = d_maxLimit.value();
        }
    }

    return result;
}

bool Backoff::equals(const Backoff& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == e_ARITHMETIC) {
        if (d_rep.d_arithmetic.object() != other.d_rep.d_arithmetic.object()) {
            return false;
        }
    }
    else if (d_type == e_GEOMETRIC) {
        if (d_rep.d_geometric.object() != other.d_rep.d_geometric.object()) {
            return false;
        }
    }

    return d_minJitter == other.d_minJitter &&
           d_maxJitter == other.d_maxJitter &&
           d_minLimit == other.d_minLimit && d_maxLimit == other.d_maxLimit;
}

bool Backoff::less(const Backoff& other) const
{
    if (static_cast<int>(d_type) < static_cast<int>(other.d_type)) {
        return true;
    }

    if (static_cast<int>(other.d_type) < static_cast<int>(d_type)) {
        return false;
    }

    if (d_type == e_ARITHMETIC) {
        if (d_rep.d_arithmetic.object() < other.d_rep.d_arithmetic.object()) {
            return true;
        }

        if (other.d_rep.d_arithmetic.object() < d_rep.d_arithmetic.object()) {
            return false;
        }
    }
    else if (d_type == e_GEOMETRIC) {
        if (d_rep.d_geometric.object() < other.d_rep.d_geometric.object()) {
            return true;
        }

        if (other.d_rep.d_geometric.object() < d_rep.d_geometric.object()) {
            return false;
        }
    }

    if (d_minJitter < other.d_minJitter) {
        return true;
    }

    if (other.d_minJitter < d_minJitter) {
        return false;
    }

    if (d_maxJitter < other.d_maxJitter) {
        return true;
    }

    if (other.d_maxJitter < d_maxJitter) {
        return false;
    }

    if (d_minLimit < other.d_minLimit) {
        return true;
    }

    if (other.d_minLimit < d_minLimit) {
        return false;
    }

    return d_maxLimit < other.d_maxLimit;
}

bsl::ostream& Backoff::print(bsl::ostream& stream,
                             int           level,
                             int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (d_type == e_ARITHMETIC) {
        const bsls::TimeInterval arithmetic = d_rep.d_arithmetic.object();
        printer.printAttribute("arithmetic", arithmetic);
    }
    else if (d_type == e_GEOMETRIC) {
        const double geometric = d_rep.d_geometric.object();
        printer.printAttribute("geometric", geometric);
    }

    printer.printAttribute("minJitter", d_minJitter);
    printer.printAttribute("maxJitter", d_maxJitter);

    if (d_minLimit.has_value()) {
        printer.printAttribute("minLimit", d_minLimit.value());
    }

    if (d_maxLimit.has_value()) {
        printer.printAttribute("maxLimit", d_maxLimit.value());
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const Backoff& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const Backoff& lhs, const Backoff& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const Backoff& lhs, const Backoff& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const Backoff& lhs, const Backoff& rhs)
{
    return lhs.less(rhs);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
