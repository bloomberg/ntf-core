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
: d_progression(bsls::TimeInterval())
, d_minJitter()
, d_maxJitter()
, d_limit()
{
}

Backoff::Backoff(const Backoff& original)
: d_progression(original.d_progression)
, d_minJitter(original.d_minJitter)
, d_maxJitter(original.d_maxJitter)
, d_limit(original.d_limit)
{
}

Backoff::~Backoff()
{
}

Backoff& Backoff::operator=(const Backoff& other)
{
    if (this != &other) {
        d_progression = other.d_progression;
        d_minJitter   = other.d_minJitter;
        d_maxJitter   = other.d_maxJitter;
        d_limit       = other.d_limit;
    }

    return *this;
}

void Backoff::reset()
{
    d_progression = bsls::TimeInterval();
    d_minJitter   = bsls::TimeInterval();
    d_maxJitter   = bsls::TimeInterval();
    d_limit.reset();
}

void Backoff::makeArithmetic(const bsls::TimeInterval& value)
{
    d_progression = value;
}

void Backoff::makeGeometric(double value)
{
    d_progression = value;
}

void Backoff::setMinJitter(const bsls::TimeInterval& value)
{
    d_minJitter = value;
}

void Backoff::setMaxJitter(const bsls::TimeInterval& value)
{
    d_maxJitter = value;
}

void Backoff::setLimit(const bsls::TimeInterval& value)
{
    d_limit = value;
}

bdlb::NullableValue<bsls::TimeInterval> Backoff::arithmetic() const
{
    bdlb::NullableValue<bsls::TimeInterval> result;
    if (bsl::holds_alternative<bsls::TimeInterval>(d_progression)) {
        result.makeValue(bsl::get<bsls::TimeInterval>(d_progression));
    }
    return result;
}

bdlb::NullableValue<double> Backoff::geometric() const
{
    bdlb::NullableValue<double> result;
    if (bsl::holds_alternative<double>(d_progression)) {
        result.makeValue(bsl::get<double>(d_progression));
    }
    return result;
}

const bsls::TimeInterval& Backoff::minJitter() const
{
    return d_minJitter;
}

const bsls::TimeInterval& Backoff::maxJitter() const
{
    return d_maxJitter;
}

const bdlb::NullableValue<bsls::TimeInterval>& Backoff::limit() const
{
    return d_limit;
}

bsls::TimeInterval Backoff::apply(const bsls::TimeInterval& value) const
{
    using namespace bsl;

    bsls::TimeInterval result;

    const bsls::Types::Int64 initial = value.totalMicroseconds();

    bsls::Types::Int64 next = 0;

    if (bsl::holds_alternative<bsls::TimeInterval>(d_progression)) {
        const bsls::TimeInterval arithmetic =
            bsl::get<bsls::TimeInterval>(d_progression);

        next = initial + arithmetic.totalMicroseconds();
    }
    else if (bsl::holds_alternative<double>(d_progression)) {
        const double geometric = bsl::get<double>(d_progression);

        next = initial * geometric;
    }
    else {
        BSLA_UNREACHABLE;
    }

    const bsls::Types::Int64 jitterMin = d_minJitter.totalMicroseconds();
    const bsls::Types::Int64 jitterMax = d_maxJitter.totalMicroseconds();

    const bsls::Types::Int64 jitterRange =
        jitterMax > jitterMin ? jitterMax - jitterMin : 0;

    const bsls::Types::Int64 jitter =
        jitterMin +
        (static_cast<double>(static_cast<bsl::uint32_t>(RAND_MAX) -
                             static_cast<bsl::uint32_t>(bsl::rand())) *
         jitterRange);

    result.addMicroseconds(next + jitter);

    if (d_limit.has_value()) {
        return bsl::min(result, d_limit.value());
    }
    else {
        return result;
    }
}

bool Backoff::equals(const Backoff& other) const
{
    return d_progression == other.d_progression &&
           d_minJitter == other.d_minJitter &&
           d_maxJitter == other.d_maxJitter && d_limit == other.d_limit;
}

bool Backoff::less(const Backoff& other) const
{
    if (d_progression < other.d_progression) {
        return true;
    }

    if (other.d_progression < d_progression) {
        return false;
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

    return d_limit < other.d_limit;
}

bsl::ostream& Backoff::print(bsl::ostream& stream,
                             int           level,
                             int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (bsl::holds_alternative<bsls::TimeInterval>(d_progression)) {
        const bsls::TimeInterval arithmetic =
            bsl::get<bsls::TimeInterval>(d_progression);
        printer.printAttribute("arithmetic", arithmetic);
    }
    else if (bsl::holds_alternative<double>(d_progression)) {
        const double geometric = bsl::get<double>(d_progression);
        printer.printAttribute("geometric", geometric);
    }

    printer.printAttribute("minJitter", d_minJitter);
    printer.printAttribute("maxJitter", d_maxJitter);

    if (d_limit.has_value()) {
        printer.printAttribute("limit", d_limit.value());
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
