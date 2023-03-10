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

#include <ntca_ratelimiterconfig.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_ratelimiterconfig_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

RateLimiterConfig::RateLimiterConfig()
: d_sustainedRateLimit()
, d_sustainedRateWindow()
, d_peakRateLimit()
, d_peakRateWindow()
, d_currentTime()
{
}

RateLimiterConfig::RateLimiterConfig(const RateLimiterConfig& original)
: d_sustainedRateLimit(original.d_sustainedRateLimit)
, d_sustainedRateWindow(original.d_sustainedRateWindow)
, d_peakRateLimit(original.d_peakRateLimit)
, d_peakRateWindow(original.d_peakRateWindow)
, d_currentTime(original.d_currentTime)
{
}

RateLimiterConfig::~RateLimiterConfig()
{
}

RateLimiterConfig& RateLimiterConfig::operator=(const RateLimiterConfig& other)
{
    if (this != &other) {
        d_sustainedRateLimit  = other.d_sustainedRateLimit;
        d_sustainedRateWindow = other.d_sustainedRateWindow;
        d_peakRateLimit       = other.d_peakRateLimit;
        d_peakRateWindow      = other.d_peakRateWindow;
        d_currentTime         = other.d_currentTime;
    }

    return *this;
}

void RateLimiterConfig::reset()
{
    d_sustainedRateLimit.reset();
    d_sustainedRateWindow.reset();
    d_peakRateLimit.reset();
    d_peakRateWindow.reset();
    d_currentTime.reset();
}

void RateLimiterConfig::setSustainedRateLimit(bsl::uint64_t value)
{
    d_sustainedRateLimit = value;
}

void RateLimiterConfig::setSustainedRateWindow(const bsls::TimeInterval& value)
{
    d_sustainedRateWindow = value;
}

void RateLimiterConfig::setPeakRateLimit(bsl::uint64_t value)
{
    d_peakRateLimit = value;
}

void RateLimiterConfig::setPeakRateWindow(const bsls::TimeInterval& value)
{
    d_peakRateWindow = value;
}

void RateLimiterConfig::setCurrentTime(const bsls::TimeInterval& value)
{
    d_currentTime = value;
}

const bdlb::NullableValue<bsl::uint64_t>& RateLimiterConfig::
    sustainedRateLimit() const
{
    return d_sustainedRateLimit;
}

const bdlb::NullableValue<bsls::TimeInterval>& RateLimiterConfig::
    sustainedRateWindow() const
{
    return d_sustainedRateWindow;
}

const bdlb::NullableValue<bsl::uint64_t>& RateLimiterConfig::peakRateLimit()
    const
{
    return d_peakRateLimit;
}

const bdlb::NullableValue<bsls::TimeInterval>& RateLimiterConfig::
    peakRateWindow() const
{
    return d_peakRateWindow;
}

const bdlb::NullableValue<bsls::TimeInterval>& RateLimiterConfig::currentTime()
    const
{
    return d_currentTime;
}

bool RateLimiterConfig::equals(const RateLimiterConfig& other) const
{
    return d_sustainedRateLimit == other.d_sustainedRateLimit &&
           d_sustainedRateWindow == other.d_sustainedRateWindow &&
           d_peakRateLimit == other.d_peakRateLimit &&
           d_peakRateWindow == other.d_peakRateWindow &&
           d_currentTime == other.d_currentTime;
}

bool RateLimiterConfig::less(const RateLimiterConfig& other) const
{
    if (d_sustainedRateLimit < other.d_sustainedRateLimit) {
        return true;
    }

    if (d_sustainedRateLimit > other.d_sustainedRateLimit) {
        return false;
    }

    if (d_sustainedRateWindow < other.d_sustainedRateWindow) {
        return true;
    }

    if (d_sustainedRateWindow > other.d_sustainedRateWindow) {
        return false;
    }

    if (d_peakRateLimit < other.d_peakRateLimit) {
        return true;
    }

    if (d_peakRateLimit > other.d_peakRateLimit) {
        return false;
    }

    if (d_peakRateWindow < other.d_peakRateWindow) {
        return true;
    }

    if (d_peakRateWindow > other.d_peakRateWindow) {
        return false;
    }

    return d_currentTime < other.d_currentTime;
}

bsl::ostream& RateLimiterConfig::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("sustainedRateLimit", d_sustainedRateLimit);
    printer.printAttribute("sustainedRateWindow", d_sustainedRateWindow);
    printer.printAttribute("peakRateLimit", d_peakRateLimit);
    printer.printAttribute("peakRateWindow", d_peakRateWindow);
    printer.printAttribute("peakRateWindow", d_currentTime);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const RateLimiterConfig& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const RateLimiterConfig& lhs, const RateLimiterConfig& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const RateLimiterConfig& lhs, const RateLimiterConfig& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const RateLimiterConfig& lhs, const RateLimiterConfig& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
