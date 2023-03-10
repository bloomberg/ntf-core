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

#ifndef INCLUDED_NTCA_RATELIMITERCONFIG
#define INCLUDED_NTCA_RATELIMITERCONFIG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>
#include <bsl_cstddef.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe the configuration of a rate limiter.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b sustainedRateLimit:
/// The sustained rate limit, in unspecified units.
///
/// @li @b sustainedRateWindow:
/// The sustained rate window, in seconds.
///
/// @li @b peakRateLimit:
/// The sustained rate limit, in unspecified units.
///
/// @li @b peakRateWindow:
/// The sustained rate window, in seconds.
///
/// @li @b currentTime:
/// The current time, since the Unix epoch.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_utility
class RateLimiterConfig
{
    bdlb::NullableValue<bsl::uint64_t>      d_sustainedRateLimit;
    bdlb::NullableValue<bsls::TimeInterval> d_sustainedRateWindow;
    bdlb::NullableValue<bsl::uint64_t>      d_peakRateLimit;
    bdlb::NullableValue<bsls::TimeInterval> d_peakRateWindow;
    bdlb::NullableValue<bsls::TimeInterval> d_currentTime;

  public:
    /// Create a new rate limiter configuration.
    RateLimiterConfig();

    /// Create a new rate limiter configuration having the same value as the
    /// specified 'original' driver configuration.
    RateLimiterConfig(const RateLimiterConfig& original);

    /// Destroy this object.
    ~RateLimiterConfig();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    RateLimiterConfig& operator=(const RateLimiterConfig& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the sustained rate limit to the specified 'value'.
    void setSustainedRateLimit(bsl::uint64_t value);

    /// Set the sustained rate window to the specified 'value'.
    void setSustainedRateWindow(const bsls::TimeInterval& value);

    /// Set the peak rate limit to the specified 'value'.
    void setPeakRateLimit(bsl::uint64_t value);

    /// Set the peak rate window to the specified 'value'.
    void setPeakRateWindow(const bsls::TimeInterval& value);

    /// Set the current time to the specified 'value'.
    void setCurrentTime(const bsls::TimeInterval& value);

    /// Return the sustained rate limit.
    const bdlb::NullableValue<bsl::uint64_t>& sustainedRateLimit() const;

    /// Return the sustained rate window.
    const bdlb::NullableValue<bsls::TimeInterval>& sustainedRateWindow() const;

    /// Return the peak rate limit.
    const bdlb::NullableValue<bsl::uint64_t>& peakRateLimit() const;

    /// Return the peak rate window.
    const bdlb::NullableValue<bsls::TimeInterval>& peakRateWindow() const;

    /// Return the current time.
    const bdlb::NullableValue<bsls::TimeInterval>& currentTime() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const RateLimiterConfig& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const RateLimiterConfig& other) const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(RateLimiterConfig);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::RateLimiterConfig
bsl::ostream& operator<<(bsl::ostream&            stream,
                         const RateLimiterConfig& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::RateLimiterConfig
bool operator==(const RateLimiterConfig& lhs, const RateLimiterConfig& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::RateLimiterConfig
bool operator!=(const RateLimiterConfig& lhs, const RateLimiterConfig& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::RateLimiterConfig
bool operator<(const RateLimiterConfig& lhs, const RateLimiterConfig& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::RateLimiterConfig
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const RateLimiterConfig& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const RateLimiterConfig& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.sustainedRateLimit());
    hashAppend(algorithm, value.sustainedRateWindow());
    hashAppend(algorithm, value.peakRateLimit());
    hashAppend(algorithm, value.peakRateWindow());
    hashAppend(algorithm, value.currentTime());
}

}  // close package namespace
}  // close enterprise namespace
#endif
