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

#include <ntcs_ratelimiter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_ratelimiter_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntcs {

bool RateLimiter::supportsExactly(bsl::uint64_t             limit,
                                  const bsls::TimeInterval& window)
{
    // Aside from checking that the capacity calculated from 'window' and
    // 'limit' can back out the same 'window' value, we also include checks on
    // the parameters that the functions called do as assertions, so that this
    // function will return 'false' for those values, e.g., 'window' is large
    // enough to cause integer overflow.
    return limit > 0 && window > bsls::TimeInterval() &&
           (limit == 1 ||
            window <= ntcs::LeakyBucket::calculateDrainTime(ULLONG_MAX,
                                                            limit,
                                                            true)) &&
           window == ntcs::LeakyBucket::calculateTimeWindow(
                         limit,
                         ntcs::LeakyBucket::calculateCapacity(limit, window));
}

RateLimiter::RateLimiter(bsl::uint64_t             sustainedRateLimit,
                         const bsls::TimeInterval& sustainedRateWindow,
                         bsl::uint64_t             peakRateLimit,
                         const bsls::TimeInterval& peakRateWindow,
                         const bsls::TimeInterval& currentTime)
: d_lock(bsls::SpinLock::s_unlocked)
, d_peakRateBucket(1, 1, currentTime)
, d_sustainedRateBucket(1, 1, currentTime)
{
    setRateLimits(sustainedRateLimit,
                  sustainedRateWindow,
                  peakRateLimit,
                  peakRateWindow);
}

RateLimiter::~RateLimiter()
{
    BSLS_ASSERT_SAFE(sustainedRateLimit() > 0);
    BSLS_ASSERT_SAFE(peakRateLimit() > 0);

    BSLS_ASSERT_SAFE(sustainedRateWindow() > bsls::TimeInterval(0));
    BSLS_ASSERT_SAFE(peakRateWindow() > bsls::TimeInterval(0));

    BSLS_ASSERT_SAFE(peakRateLimit() == 1 ||
                     peakRateWindow() <=
                         ntcs::LeakyBucket::calculateDrainTime(ULLONG_MAX,
                                                               peakRateLimit(),
                                                               true));

    BSLS_ASSERT_SAFE(
        sustainedRateLimit() == 1 ||
        sustainedRateWindow() <=
            ntcs::LeakyBucket::calculateDrainTime(ULLONG_MAX,
                                                  sustainedRateLimit(),
                                                  true));
}

bool RateLimiter::supportsRateLimitsExactly(
    bsl::uint64_t             sustainedRateLimit,
    const bsls::TimeInterval& sustainedRateWindow,
    bsl::uint64_t             peakRateLimit,
    const bsls::TimeInterval& peakRateWindow)
{
    return supportsExactly(sustainedRateLimit, sustainedRateWindow) &&
           supportsExactly(peakRateLimit, peakRateWindow);
}

void RateLimiter::setRateLimits(bsl::uint64_t             sustainedRateLimit,
                                const bsls::TimeInterval& sustainedRateWindow,
                                bsl::uint64_t             peakRateLimit,
                                const bsls::TimeInterval& peakRateWindow)
{
    bsls::SpinLockGuard guard(&d_lock);

    BSLS_ASSERT(sustainedRateLimit > 0);
    BSLS_ASSERT(peakRateLimit > 0);

    BSLS_ASSERT(sustainedRateWindow > bsls::TimeInterval(0));
    BSLS_ASSERT(peakRateWindow > bsls::TimeInterval(0));

    BSLS_ASSERT(peakRateLimit == 1 ||
                peakRateWindow <=
                    ntcs::LeakyBucket::calculateDrainTime(ULLONG_MAX,
                                                          peakRateLimit,
                                                          true));

    BSLS_ASSERT(sustainedRateLimit == 1 ||
                sustainedRateWindow <=
                    ntcs::LeakyBucket::calculateDrainTime(ULLONG_MAX,
                                                          sustainedRateLimit,
                                                          true));

    bsl::uint64_t capacity =
        ntcs::LeakyBucket::calculateCapacity(sustainedRateLimit,
                                             sustainedRateWindow);

    d_sustainedRateBucket.setRateAndCapacity(sustainedRateLimit, capacity);

    capacity =
        ntcs::LeakyBucket::calculateCapacity(peakRateLimit, peakRateWindow);

    d_peakRateBucket.setRateAndCapacity(peakRateLimit, capacity);
}

bsls::TimeInterval RateLimiter::calculateTimeToSubmit(
    const bsls::TimeInterval& currentTime)
{
    bsls::SpinLockGuard guard(&d_lock);

    bsls::TimeInterval timeToSubmitPeak =
        d_peakRateBucket.calculateTimeToSubmit(currentTime);

    bsls::TimeInterval timeToSubmitSustained =
        d_sustainedRateBucket.calculateTimeToSubmit(currentTime);

    return bsl::max(timeToSubmitPeak, timeToSubmitSustained);
}

}  // close package namespace
}  // close enterprise namespace
