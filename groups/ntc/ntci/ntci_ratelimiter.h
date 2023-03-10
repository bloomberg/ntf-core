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

#ifndef INCLUDED_NTCI_RATELIMITER
#define INCLUDED_NTCI_RATELIMITER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

namespace BloombergLP {
namespace ntci {

/// Provide a mechanism to limit peak and sustained consumption rates.
///
/// @details
/// Provide a mechanism to enable clients to monitor and control the
/// use of a resource such that the peak consumption rate and the sustained
/// consumption rate do not exceed their respective configured limits.
///
/// The limits on resource consumption rates of a rate limiter object are
/// configured using a specified peak rate (measured in 'units/s') along with
/// its time-window, and a specified sustained rate (measured in 'units/s')
/// along with its time-window.  The peak-rate time-window indicates a sliding
/// time period over which the average consumption rate shall not exceed the
/// peak-rate; similarly, the sustained-rate time-window indicates a sliding
/// time period over which the average consumption rate shall not exceed the
/// sustained rate.  'unit' is a generic unit of measurement (e.g., bytes,
/// megabytes, number of messages, packets, liters, clock cycles, etc.).
///
/// @par Internal Model
/// Internally, a rate limiter effectively models resource usage using two
/// corresponding "leaky bucket" mechanisms, one for limiting peak resource
/// usage and one for limiting sustained resource usage.  Each leaky bucket
/// provides an approximation for a moving total, where the configured time
/// window corresponds to the period of the moving total, and that time window
/// multiplied by the corresponding rate indicates the sum that the moving
/// total may not exceed (i.e., the capacity of the leaky bucket).  As the
/// units are submitted to a rate limiter, they are added to both the peak and
/// sustained rate moving-totals, and then removed over time at the
/// corresponding configured rate.
///
/// Figure 1 illustrates the behavior of a rate limiter during a typical usage
/// scenario using moving-totals:
///
/// Fig. 1:
///
///     Rp (peak rate)                  = 2 units/s
///     Wp (peak-rate time-window)      = 2 s
///     Rs (sustained rate)             = 1 units/s
///     Ws (sustained-rate time-window) = 7 s
///
///          Submit 5                                      Submit 7
///
///          |   |     |   |        |   |      |   |       |   |     7|~~~|
///        12|   |    6|   |      12|   |     6|   |     12|   |     6|~~~|
///        11|   |    5|~~~|      11|   |     5|   |     11|   |     5|~~~|
///        10|   | Lp-4|~~~|      10|   |  Lp-4|---|     10|   |  Lp-4|~~~|
///         9|   |    3|~~~|       9|   |     3|   |      9|~~~|     3|~~~|
///         8|   |    2|~~~|       8|   |     2|   |      8|~~~|     2|~~~|
///      Ls-7|---|    1|~~~|    Ls-7|---|     1|~~~|   Ls-7|~~~|     1|~~~|
///         6|   |     +- -+       6|   |      +- -+      6|~~~|      +- -+
///         5|~~~|                 5|   |                 5|~~~|
///         4|~~~|                 4|   |                 4|~~~|
///         3|~~~|                 3|~~~|                 3|~~~|
///         2|~~~|                 2|~~~|                 2|~~~|
///         1|~~~|                 1|~~~|                 1|~~~|
///          +- -+                  +- -+                  +- -+
///
///     Time:   t0                t0 + 2s                t0 + 2s
///
///                                                        Submit 2
///
///          |   |     7|   |       |   |     7|   |       |   |     7|   |
///        12|   |     6|   |     12|   |     6|   |     12|   |     6|   |
///        11|   |     5|   |     11|   |     5|   |     11|   |     5|   |
///        10|   |  Lp-4|---|     10|   |  Lp-4|---|     10|   |  Lp-4|---|
///         9|   |     3|~~~|      9|   |     3|   |      9|   |     3|~~~|
///         8|   |     2|~~~|      8|   |     2|   |      8|   |     2|~~~|
///      Ls-7|~~~|     1|~~~|   Ls-7|---|     1|~~~|   Ls-7|~~~|     1|~~~|
///         6|~~~|      +- -+      6|---|      +- -+      6|~~~|      +- -+
///         5|~~~|                 5|~~~|                 5|~~~|
///         4|~~~|                 4|~~~|                 4|~~~|
///         3|~~~|                 3|~~~|                 3|~~~|
///         2|~~~|                 2|~~~|                 2|~~~|
///         1|~~~|                 1|~~~|                 1|~~~|
///          +- -+                  +- -+                  +- -+
///
///     Time: t0 + 4s             t0 + 6s                t0 + 6s
///
/// Suppose we have a rate limiter with a peak rate of 'Rp = 2 units/s', a
/// peak-rate time-window of 'Wp = 2 s', a sustained rate of 'Rs = 1 units/s',
/// and a sustained-rate time-window of 'Ws = 7 s'.
///
/// This rate limiter maintains a moving-total having a capacity
/// 'Lp = Rp * Wp = 4 units' that controls the peak rate and another
/// moving-total having a capacity 'Ls = Rs * Ws = 7 units' that controls the
/// sustained rate.
///
/// Figure 1 shows the following sequence of events:
///
/// 1. At time 't0s', we submit 5 units.  The submitted units are added to the
///    both moving-totals, and as a result the 'Lp' is exceeded, which means
///    that the average consumption rate over the peak-rate time-window has
///    exceeded the peak rate.  Note that we can not submit any more units at
///    this time even though 'Ls' is not exceeded (the average consumption rate
///    over the sustained-rate time-windows has not exceeded the sustained
///    rate).
///
/// 2. At time 't0 + 2s' the number of units contained moving-totals are
///    recalculated.  As a result, 4 units ('Rp * 2 s') are subtracted from the
///    peak rate moving-total, and 2 units ('Rs * 2 s') are subtracted from the
///    sustained rate moving-total.  Now, capacities of both moving-totals are
///    no longer exceeded, so we are free to submit more units.  We submit 7
///    units, causing both 'Lp' and 'Ls' to be exceeded.
///
/// 3. At time 't0 + 4s', the moving-totals are again updated.  The 'Lp' limit
///    is no longer exceeded.  The number of units held by the moving-total
///    tracking sustained rate matches the moving-total's capacity, and this
///    boundary condition imply and no units can be submitted, because
///    submitting any amount of units would cause 'Ls' to be exceeded.
///
/// 4. At time 't0 + 6s', the moving-totals are again updated.  Both 'Lp' and
///    'Ls' are no longer exceeded.  We submit 2 units.  The 'Lp' limit is not
///    exceeded, but 'Ls' limit is exceeded.
///
/// @par Monitoring Resource Usage
/// A leaky bucket mechanism provides methods to both submit units and reserve
/// units for future submission.  Submitting a unit indicates that it has been
/// consumed by the entity being modeled, and it is added to the moving-totals
/// tracking both peak and sustained resource usage.
///
/// Reserving a unit guarantees that available capacity will be reserved so
/// that unit can be submitted in the future without exceeding the configured
/// limits. Reserved units may be later submitted using the 'submitReserved'
/// method or canceled using the 'cancelReserved' method.  Reserved units
/// permanently reside in the two moving-totals of consumed units, resulting in
/// the reduction in the effective capacities of the moving-totals, until the
/// reserved units are canceled or submitted.  Reserving units effectively
/// shortens the time-window during which the average sustained and peak rate
/// are enforced.  Therefore, the time interval between reserving units and
/// submitting or canceling them should be kept as short as possible.
///
/// The recommended usage of a rate limiter is to first check whether 1 unit
/// can be added without exceeding the rate limiter's configured limits, and if
/// so, consume the desired amount of the resource.  Afterwards, submit the
/// amount of consumed resource to the rate limiter.
///
/// Whether submitting more units would exceed the configured limits can be
/// determined using the 'wouldExceedBandwidth' method.  The estimated amount
/// of time to wait before 1 more unit will be allowed to be submitted can be
/// determined using the 'calculateTimeToSubmit' method.
///
/// @par Time Synchronization
/// A rate limiter does not utilize an internal timer, so timing must be
/// handled manually.  Clients can specify an initial time interval for a rate
/// limiter object at construction or using the 'reset' method.  Whenever the
/// state of a rate limiter object needs to be updated, clients must invoke the
/// 'updateState' method specifying the current time interval.  Since rate
/// limiter cares only about the elapsed time (not absolute time), the
/// specified time intervals may be relative to any arbitrary time origin,
/// though all of them must refer to the same origin.  For the sake of
/// consistency, clients are encouraged to use the unix epoch time (such as the
/// values returned by 'bdlt::CurrentTime::now').
///
/// @par Usage Example 1: Controlling Network Traffic Generation
/// Suppose that we want to send data over a network interface with the load
/// spike limitations explained below:
///
/// - The long term average rate of resource usage (i.e., the sustained rate)
///   should not exceed 1024 bytes/s ('Rs').
///
/// - The period over which to monitor the long term average rate (i.e., the
///   sustained-rate time-window) should be 0.5s ('Wp').
///
/// - The peak resource usage (i.e., the peak rate) should not exceed 2048
///   bytes/s ('Rp').
///
/// - The period over which to monitor the peak resource usage should be
///   0.0625s (Wp).
///
/// This is shown in Figure 2 below.
///
///     Fig. 2:
///
///         ^ Rate (Units per second)
///         |                             _____         .
///         |                            /  B  \        .
///     2048|---------------------------/-------\--------Rp (Maximum peak rate)
///         |           __             /         \      .
///         |          /  \           /    A2     \     .
///         |         / A1 \         /             \    .
///     1024|--------/------\ ------/---------------\----Rs (Maximum sust rate)
///         |   __  /        \     /                 \__.
///         |__/  \/          \___/                     .
///         |                                           .
///          --------------------------------------------->
///                                             T (seconds)
///
/// Notice that we can understand the limitations imposed by the rate-limiter
/// graphically as the maximum area above the respective lines, 'Rp' and 'Rs',
/// that the usage curve to allowed to achieve.  In the example above:
///
///  - The area above the sustained rate 'Rs' (e.g., 'A1' or 'A2+B') should
///    contain no more than 512 bytes (Rs * Ws).
///
///  - The area above the peak rate 'Rp' should contain no more than 128 bytes
///    (Rp * Wp).
///
/// Further suppose that we have a function, 'sendData', that transmits a
/// specified amount of data over that network:
///
///     bool sendData(size_t dataSize);
///         // Send a specified 'dataSize' amount of data over the network.
///         // Return 'true' if data was sent successfully and 'false'
///         // otherwise.
///
/// First, we create an 'ntci::RateLimiter' object having a sustained rate of
/// 1024 bytes/s, a sustained-rate time-window of 0.5s
/// (512 bytes / 1024 bytes/s), a peak-rate of 2048 bytes/s, and a peak-rate
/// time-window of 0.0625s (128 bytes / 2048 bytes/s):
///
///     ntca::RateLimiterConfig rateLimiterConfig;
///     rateLimiterConfig.setSustainedRateLimit(1024);
///     rateLimiterConfig.setSustainedRateWindow(bsls::TimeInterval(0.5));
///     rateLimiterConfig.setPeakRateLimit(2048);
///     rateLimiterConfig.setPeakRateWindow(bsls::TimeInterval(0.0625));
///     rateLimiterConfig.setCurrentTime(bdlt::CurrentTime::now());
///
///     bsl::shared_ptr<ntci::RateLimiter> rateLimiter =
///         ntcf::System::createRateLimiter(rateLimiterConfig);
///
/// Note that the rate limiter does not prevent the rate at any instant from
/// exceeding either the peak-rate or the sustained rate; instead, it prevents
/// the average rate over the peak-rate time-window from exceeding maximum
/// peak-rate and the average rate over the sustained-rate time-window from
/// exceeding the maximum sustained-rate.
///
/// Then, we define the size of data to be send, the size of each data chunk,
/// and a counter of data actually sent:
///
///     bsl::uint64_t sizeOfData = 10 * 1024; // in bytes
///     bsl::uint64_t chunkSize  = 64;        // in bytes
///     bsl::uint64_t bytesSent  = 0;
///
/// Now, we send the chunks of data using a loop.  For each iteration, we check
/// whether submitting another byte would exceed the rate limiter's bandwidth
/// limits.  If not, we send an additional chunk of data and submit the number
/// of bytes sent to the leaky bucket.  Note that 'submit' is invoked only
/// after the data has been sent.
///
///     while (bytesSent < sizeOfData) {
///         now = bdlt::CurrentTime::now();
///         if (!rateLimiter->wouldExceedBandwidth(now)) {
///             if (true == sendData(chunkSize)) {
///                 rateLimiter->submit(chunkSize);
///                 bytesSent += chunkSize;
///             }
///         }
///
/// Finally, if submitting another byte will cause the rate limiter to exceed
/// its bandwidth limits, then we wait until the submission will be allowed by
/// waiting for an amount time returned by the 'calculateTimeToSubmit' method:
///
///         else {
///             bsls::TimeInterval timeToSubmit =
///                                rateLimiter->calculateTimeToSubmit(now);
///
///             bsl::uint64_t uS = timeToSubmit.totalMicroseconds() +
///                                (timeToSubmit.nanoseconds() % 1000 ? 1 : 0);
///
///             bslmt::ThreadUtil::microSleep(uS);
///         }
///     }
///
/// Notice that we wait by putting the thread into a sleep state instead of
/// using busy-waiting to better optimize for multi-threaded applications.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_utility
class RateLimiter
{
  public:
    /// Destroy this object.
    virtual ~RateLimiter();

    /// Set the 'lastUpdateTime' of this rate limiter to the specified
    /// 'currentTime'.  If the specified 'currentTime' is after
    /// 'lastUpdateTime', then recalculate number of units available for
    /// consumption based on the 'peakRate', 'sustainedRate' and the time
    /// interval between 'lastUpdateTime' and 'currentTime'.  If
    /// 'currentTime' is before 'statisticsCollectionStartTime', set it' to
    /// 'currentTime'.
    virtual void updateState(const bsls::TimeInterval& currentTime) = 0;

    /// Reset the statics collected for this rate limiter by setting the
    /// number of units used and the number of units submitted to 0, and set
    /// the 'statisticsCollectionStartTime' to the 'lastUpdateTime' of this
    /// leaky bucket.
    virtual void resetStatistics() = 0;

    /// Set the sustained rate of this rate limiter to the specified
    /// 'sustainedRateLimit', the sustained-rate time-window to the
    /// specified 'sustainedRateWindow', the peak rate to the specified
    /// 'peakRateLimit' and the peak-rate time-window to the specified
    /// 'peakRateWindow'.  The behavior is undefined unless
    /// '0 < sustainedRateLimit', '0 < sustainedRateWindow',
    /// '0 < peakRateLimit', '0 < peakRateWindow', the product of
    /// 'sustainedRateLimit' and 'sustainedRateWindow' can be represented by
    /// 64-bit unsigned integral type, and the product of 'peakRateLimit'
    /// and 'peakRateWindow' can be represented by 64-bit unsigned integral
    /// type.
    virtual void setRateLimits(bsl::uint64_t             sustainedRateLimit,
                               const bsls::TimeInterval& sustainedRateWindow,
                               bsl::uint64_t             peakRateLimit,
                               const bsls::TimeInterval& peakRateWindow) = 0;

    /// Submit the specified 'numUnits' to this rate limiter.  The behavior
    /// is undefined unless the sum of 'numUnits', unused units previously
    /// submitted to this rate limiter, and 'unitsReserved' can be
    /// represented by a 64-bit unsigned integral type.
    virtual void submit(bsl::uint64_t numUnits) = 0;

    /// Reserve the specified 'numUnits' for future use by this rate
    /// limiter.  The behavior is undefined unless the sum of 'numUnits',
    /// unused units previously submitted to this rate limiter, and
    /// 'unitsReserved' can be represented by a 64-bit unsigned integral
    /// type.
    virtual void reserve(bsl::uint64_t numUnits) = 0;

    /// Cancel the specified 'numUnits' that were previously reserved.  The
    /// behavior is undefined unless 'numUnits <= unitsReserved()'.
    virtual void cancelReserved(bsl::uint64_t numUnits) = 0;

    /// Submit the specified 'numUnits' that were previously reserved.  The
    /// behavior is undefined unless 'numUnits <= unitsReserved()'.
    virtual void submitReserved(bsl::uint64_t numUnits) = 0;

    /// Update the state of this rate limiter to the specified
    /// 'currentTime'.  Return 'true' if submitting 1 unit at the
    /// 'currentTime' would exceed the configured limits, and false
    /// otherwise.
    virtual bool wouldExceedBandwidth(
        const bsls::TimeInterval& currentTime) = 0;

    /// Update the state of this rate limiter to the specified
    /// 'currentTime'.  Return the estimated time interval that should pass
    /// from 'currentTime' until 1 more unit can be submitted to this rate
    /// limiter without exceeding its configured limits.  The number of
    /// nanoseconds in the returned time interval is rounded up.  Note that
    /// a time interval of 0 is returned if 1 or more units can be submitted
    /// at 'currentTime'.  Also note that after waiting for the returned
    /// time interval, clients should typically check again using this
    /// method, because additional units may have been submitted in the
    /// interim.
    virtual bsls::TimeInterval calculateTimeToSubmit(
        const bsls::TimeInterval& currentTime) = 0;

    /// Reset the statistics counter for this rate limiter to 0, and set the
    /// 'lastUpdateTime' of this rate limiter to the specified
    /// 'currentTime'.
    virtual void reset(const bsls::TimeInterval& currentTime) = 0;

    /// Return the peak rate of this rate limiter.
    virtual bsl::uint64_t peakRateLimit() const = 0;

    /// Return the peak-rate time-period of this rate limiter.  Note that
    /// this period is generally significantly shorter than
    /// 'sustainedRateWindow'.
    virtual bsls::TimeInterval peakRateWindow() const = 0;

    /// Return the sustained rate of this rate limiter.
    virtual bsl::uint64_t sustainedRateLimit() const = 0;

    /// Return the sustained-rate time-period of this rate limiter.  Note
    /// that this period is generally significantly longer than the
    /// 'peakRateWindow'.
    virtual bsls::TimeInterval sustainedRateWindow() const = 0;

    /// Return the number of reserved units for this rate limiter.
    virtual bsl::uint64_t unitsReserved() const = 0;

    /// Return the time when this rate limiter was last updated.
    virtual bsls::TimeInterval lastUpdateTime() const = 0;

    /// Load, into the specified 'submittedUnits' and the specified
    /// 'unusedUnits' respectively, the numbers of submitted units and the
    /// number of unused units for this rate limiter from the
    /// 'statisticsCollectionStartTime' to the 'lastUpdateTime.  The number
    /// of unused units is the difference between the number of units that
    /// could have been consumed at the sustained rate and the number of
    /// units actually submitted for the time period.
    virtual void getStatistics(bsl::uint64_t* submittedUnits,
                               bsl::uint64_t* unusedUnits) const = 0;

    /// Return the time interval when the collection of the statistics (as
    /// returned by 'getStatistics') started.
    virtual bsls::TimeInterval statisticsCollectionStartTime() const = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
