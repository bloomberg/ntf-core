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

#include <ntcs_leakybucket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_leakybucket_cpp, "$Id$ $CSID$")

#include <bsl_c_math.h>

namespace BloombergLP {
namespace ntcs {

namespace {

/// Return the number of units that would be drained from a leaky bucket
/// over the specified 'timeInterval' at the specified 'drainRate', plus the
/// specified 'fractionalUnitDrainedInNanoUnits', representing a fractional
/// remainder from a previous call to 'calculateNumberOfUnitsToDrain'.  Load
/// into 'fractionalUnitDrainedInNanoUnits' the fractional remainder
/// (between 0.0 and 1.0, represented in nano-units) from this caculation.
/// The behavior is undefined unless
/// '0 <= *fractionalUnitDrainedInNanoUnits < 1000000000' (i.e., it
/// represents a value between 0 and 1 unit) and
/// 'timeInterval.seconds() * drainRate <= ULLONG_MAX'.  Note that
/// 'fractionalUnitDrainedInNanoUnits' is represented in nano-units to avoid
/// using a floating point representation.
bsl::uint64_t calculateNumberOfUnitsToDrain(
    bsl::uint64_t*            fractionalUnitDrainedInNanoUnits,
    bsl::uint64_t             drainRate,
    const bsls::TimeInterval& timeInterval)

{
    const bsl::uint64_t NANOUNITS_PER_UNIT = 1000000000;

    BSLS_ASSERT(static_cast<bsl::uint64_t>(timeInterval.seconds()) <=
                ULLONG_MAX / drainRate);
    BSLS_ASSERT(0 != fractionalUnitDrainedInNanoUnits);
    BSLS_ASSERT(*fractionalUnitDrainedInNanoUnits < NANOUNITS_PER_UNIT);

    bsl::uint64_t units = drainRate * timeInterval.seconds();
    units += (drainRate / NANOUNITS_PER_UNIT) * timeInterval.nanoseconds();

    bsl::uint64_t nanounits = 0;

    // As long as rate is represented by a whole number, the fractional part
    // of number of units to drain comes from fractional part of seconds of
    // the time interval

    nanounits = *fractionalUnitDrainedInNanoUnits +
                (drainRate % NANOUNITS_PER_UNIT) * timeInterval.nanoseconds();

    *fractionalUnitDrainedInNanoUnits = nanounits % NANOUNITS_PER_UNIT;

    units += nanounits / NANOUNITS_PER_UNIT;

    return units;
}

}  // close unnamed namespace

bsls::TimeInterval LeakyBucket::calculateDrainTime(bsl::uint64_t numUnits,
                                                   bsl::uint64_t drainRate,
                                                   bool          ceilFlag)
{
    BSLS_ASSERT(drainRate > 1 || numUnits <= LLONG_MAX);

    bsls::TimeInterval interval(0, 0);

    interval.addSeconds(numUnits / drainRate);
    bsl::uint64_t remUnits = numUnits % drainRate;

    const double nanoSecs =
        static_cast<double>(remUnits) * 1e9 / static_cast<double>(drainRate);
    interval.addNanoseconds(static_cast<bsl::int64_t>(
        ceilFlag ? ceil(nanoSecs) : floor(nanoSecs)));

    return interval;
}

bsls::TimeInterval LeakyBucket::calculateTimeWindow(bsl::uint64_t drainRate,
                                                    bsl::uint64_t capacity)
{
    BSLS_ASSERT(drainRate > 0);
    BSLS_ASSERT(drainRate > 1 || capacity <= LLONG_MAX);

    bsls::TimeInterval window =
        LeakyBucket::calculateDrainTime(capacity, drainRate, true);

    if (0 == window) {
        window.addNanoseconds(1);
    }

    return window;
}

bsl::uint64_t LeakyBucket::calculateCapacity(
    bsl::uint64_t             drainRate,
    const bsls::TimeInterval& timeWindow)
{
    BSLS_ASSERT(1 == drainRate ||
                timeWindow <= LeakyBucket::calculateDrainTime(ULLONG_MAX,
                                                              drainRate,
                                                              false));

    bsl::uint64_t fractionalUnitsInNanoUnits = 0;

    bsl::uint64_t capacity =
        calculateNumberOfUnitsToDrain(&fractionalUnitsInNanoUnits,
                                      drainRate,
                                      timeWindow);

    // Round the returned capacity to 1, which is okay, because it does not
    // affect the drain rate.

    return (0 != capacity) ? capacity : 1;
}

LeakyBucket::LeakyBucket(bsl::uint64_t             drainRate,
                         bsl::uint64_t             capacity,
                         const bsls::TimeInterval& currentTime)
: d_drainRate(drainRate)
, d_capacity(capacity)
, d_unitsReserved(0)
, d_unitsInBucket(0)
, d_fractionalUnitDrainedInNanoUnits(0)
, d_lastUpdateTime(currentTime)
, d_statSubmittedUnits(0)
, d_statSubmittedUnitsAtLastUpdate(0)
, d_statisticsCollectionStartTime(currentTime)
{
    BSLS_ASSERT_OPT(0 < d_drainRate);
    BSLS_ASSERT_OPT(0 < d_capacity);
    BSLS_ASSERT(LLONG_MIN != currentTime.seconds());

    // Calculate the maximum interval between updates that would not cause the
    // number of units drained to overflow an unsigned 64-bit integral type.

    if (drainRate == 1) {
        // 'd_maxUpdateInterval' is a signed 64-bit integral type that can't
        // represent 'ULLONG_MAX' number of seconds, so we set
        // 'd_maxUpdateInterval' to the maximum representable value when
        // 'drainRate' is 1.

        d_maxUpdateInterval = bsls::TimeInterval(LLONG_MAX, 999999999);
    }
    else {
        d_maxUpdateInterval =
            LeakyBucket::calculateDrainTime(ULLONG_MAX, drainRate, false);
    }
}

bsls::TimeInterval LeakyBucket::calculateTimeToSubmit(
    const bsls::TimeInterval& currentTime)
{
    bsl::uint64_t usedUnits = d_unitsInBucket + d_unitsReserved;

    // Return 0-length time interval if units can be submitted right now.

    if (usedUnits < d_capacity) {
        return bsls::TimeInterval(0, 0);  // RETURN
    }

    updateState(currentTime);

    // Return 0-length time interval if units can be submitted after the state
    // has been updated.

    if (d_unitsInBucket + d_unitsReserved < d_capacity) {
        return bsls::TimeInterval(0, 0);  // RETURN
    }

    bsls::TimeInterval timeToSubmit(0, 0);
    bsl::uint64_t      backlogUnits;

    // From here, 'd_unitsInBucket + d_unitsReserved' is always greater than
    // 'd_capacity'

    backlogUnits = d_unitsInBucket + d_unitsReserved - d_capacity + 1;

    timeToSubmit =
        LeakyBucket::calculateDrainTime(backlogUnits, d_drainRate, true);

    // Return 1 nanosecond if the time interval was rounded to zero (in cases
    // of high drain rates).

    if (timeToSubmit == 0) {
        timeToSubmit.addNanoseconds(1);
    }

    return timeToSubmit;
}

void LeakyBucket::setRateAndCapacity(bsl::uint64_t newRate,
                                     bsl::uint64_t newCapacity)
{
    BSLS_ASSERT_SAFE(0 < newRate);
    BSLS_ASSERT_SAFE(0 < newCapacity);

    d_drainRate = newRate;
    d_capacity  = newCapacity;

    // Calculate the maximum interval between updates that would not cause the
    // number of units drained to overflow an unsigned 64-bit integral type.

    if (newRate == 1) {
        d_maxUpdateInterval = bsls::TimeInterval(LLONG_MAX, 999999999);
    }
    else {
        d_maxUpdateInterval =
            LeakyBucket::calculateDrainTime(ULLONG_MAX, newRate, false);
    }
}

void LeakyBucket::updateState(const bsls::TimeInterval& currentTime)
{
    BSLS_ASSERT(LLONG_MIN != currentTime.seconds());

    bsls::TimeInterval delta         = currentTime - d_lastUpdateTime;
    d_statSubmittedUnitsAtLastUpdate = d_statSubmittedUnits;

    // If delta is greater than the time it takes to drain the maximum number
    // of units representable by 64 bit integral type, then reset
    // 'unitsInBucket'.

    if (delta > d_maxUpdateInterval) {
        d_lastUpdateTime                   = currentTime;
        d_unitsInBucket                    = 0;
        d_fractionalUnitDrainedInNanoUnits = 0;
        return;  // RETURN
    }

    if (delta >= bsls::TimeInterval(0, 0)) {
        bsl::uint64_t units;
        units =
            calculateNumberOfUnitsToDrain(&d_fractionalUnitDrainedInNanoUnits,
                                          d_drainRate,
                                          delta);

        if (units < d_unitsInBucket) {
            d_unitsInBucket -= units;
        }
        else {
            d_unitsInBucket = 0;
        }
    }
    else {
        // The delta maybe negative when the system clocks are updated.  If the
        // specified 'currentTime' precedes 'statisticsCollectionStartTime',
        // adjust it to prevent the statistics collection interval from going
        // negative.

        if (currentTime < d_statisticsCollectionStartTime) {
            d_statisticsCollectionStartTime = currentTime;
        }
    }

    d_lastUpdateTime = currentTime;
}

bool LeakyBucket::wouldOverflow(const bsls::TimeInterval& currentTime)
{
    updateState(currentTime);

    if (1 > ULLONG_MAX - d_unitsInBucket - d_unitsReserved ||
        d_unitsInBucket + d_unitsReserved + 1 > d_capacity)
    {
        return true;  // RETURN
    }
    return false;
}

void LeakyBucket::getStatistics(bsl::uint64_t* submittedUnits,
                                bsl::uint64_t* unusedUnits) const
{
    BSLS_ASSERT(0 != submittedUnits);
    BSLS_ASSERT(0 != unusedUnits);

    *submittedUnits               = d_statSubmittedUnitsAtLastUpdate;
    bsl::uint64_t fractionalUnits = 0;

    // 'monitoredInterval' can not be negative, as the 'updateState' method
    // ensures that 'd_lastUpdateTime' always precedes
    // 'statisticsCollectionStartTime'.

    bsls::TimeInterval monitoredInterval =
        d_lastUpdateTime - d_statisticsCollectionStartTime;

    bsl::uint64_t drainedUnits =
        calculateNumberOfUnitsToDrain(&fractionalUnits,
                                      d_drainRate,
                                      monitoredInterval);

    if (drainedUnits < d_statSubmittedUnitsAtLastUpdate) {
        *unusedUnits = 0;
    }
    else {
        *unusedUnits = drainedUnits - d_statSubmittedUnitsAtLastUpdate;
    }
}

}  // close package namespace
}  // close enterprise namespace
