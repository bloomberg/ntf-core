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

#ifndef INCLUDED_NTCI_METRIC
#define INCLUDED_NTCI_METRIC

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_monitorable.h>
#include <ntcscm_version.h>
#include <bsls_spinlock.h>
#include <bsl_algorithm.h>
#include <bsl_limits.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntci {

/// @internal @brief
/// Describe the metadata for a metric.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_metrics
struct MetricMetadata {
    const char*                      d_name;
    ntci::Monitorable::StatisticType d_type;
};

/// @internal @brief
/// Describe a snapshot of the value measured for a metric.
///
/// @par Thread Safety
/// This class is not thread safe.
class MetricValue
{
    bsl::uint64_t d_count;    // Number of collections
    double        d_total;    // Total of the metric values
    double        d_minimum;  // The minumum metric value
    double        d_maximum;  // The maximum metric value
    double        d_last;     // The last update

  public:
    /// Create a new metric snapshot having default values.
    MetricValue();

    /// Reset the values to their defaults.
    void reset();

    /// Update the snapshot with the specified measured 'value'.
    void update(double value);

    /// Number of times the metric has been collected.
    bsl::uint64_t count() const;

    /// Total of the metric values.
    double total() const;

    /// The minumum metric value.
    double minimum() const;

    /// The average metric value.
    double average() const;

    /// The maximum metric value.
    double maximum() const;

    /// The last update.
    double last() const;
};

/// Provide a measurement defined by the total, minimum, average, and
/// maximum of the recorded values.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_metrics
class Metric
{
    bsls::SpinLock    d_lock;
    ntci::MetricValue d_value;

  public:
    /// Create a new metric having default values.
    Metric();

    /// Update the snapshot with the specified measured 'value'.
    void update(double value);

    /// Load the value of the metric into the specified 'result'.
    void load(ntci::MetricValue* result);

    /// Load the count of measurements of the metric into the specified
    /// 'array', starting at '*index' and modifying the indexes used.
    void collectCount(bdld::DatumMutableArrayRef* array, bsl::size_t* index);

    /// Load the total value of the metric into the specified 'array',
    /// starting at '*index' and modifying the indexes used.
    void collectTotal(bdld::DatumMutableArrayRef* array, bsl::size_t* index);

    /// Load the minimum value of the metric into the specified 'array',
    /// starting at '*index' and modifying the indexes used.
    void collectMin(bdld::DatumMutableArrayRef* array, bsl::size_t* index);

    /// Load the average value of the metric into the specified 'array',
    /// starting at '*index' and modifying the indexes used.
    void collectAvg(bdld::DatumMutableArrayRef* array, bsl::size_t* index);

    /// Load the maximum value of the metric into the specified 'array',
    /// starting at '*index' and modifying the indexes used.
    void collectMax(bdld::DatumMutableArrayRef* array, bsl::size_t* index);

    /// Load the last value of the metric into the specified 'array',
    /// starting at '*index' and modifying the indexes used.
    void collectLast(bdld::DatumMutableArrayRef* array, bsl::size_t* index);

    /// Load the entire value of the metric into the specified 'array',
    /// starting at '*index' and modifying the indexes used.
    void collectSummary(bdld::DatumMutableArrayRef* array, bsl::size_t* index);
};

/// Provide a measurement defined by the last recorded value.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_metrics
class MetricGauge
{
    bsls::SpinLock d_lock;
    double         d_value;

  public:
    /// Create a new metric having default values.
    MetricGauge();

    /// Update the snapshot with the specified measured 'value'. Return
    /// true if the update is valid, and false otherwise.
    bool update(double value);

    /// Load the value of the metric into the specified 'result'.
    void load(double* result);

    /// Load the last value of the metric into the specified 'array',
    /// starting at '*index' and modifying the indexes used.
    void collectLast(bdld::DatumMutableArrayRef* array, bsl::size_t* index);
};

/// Provide a measurement defined by the sum of all recorded values.
///
/// @details
/// This class represents the successive measurement's of a total measurement
/// of a quantity. For example, this measurement can be used to store the
/// system's measurement of the number of context switches for a process, which
/// is given by the system as a monotonically increasing number. This class
/// stores the value the total had been incremented at each sample. A total's
/// value is never reset.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_metrics
class MetricTotal
{
    bsls::SpinLock d_lock;
    double         d_total;
    double         d_delta;

  public:
    /// Create a new metric having default values.
    MetricTotal();

    /// Update the snapshot with the difference between the specified
    /// measured 'value' and the last measured value. Return true if the
    /// update is valid, and false otherwise.
    bool update(double value);

    /// Load the value of the metric into the specified 'result'.
    void load(double* result);

    /// Load the last value of the metric into the specified 'array',
    /// starting at '*index' and modifying the indexes used.
    void collectTotal(bdld::DatumMutableArrayRef* array, bsl::size_t* index);
};

#define NTCI_METRIC_METADATA_COUNT(name)                                      \
    {                                                                         \
        #name ".count", ntci::Monitorable::e_SUM                              \
    }

#define NTCI_METRIC_METADATA_TOTAL(name)                                      \
    {                                                                         \
        #name ".total", ntci::Monitorable::e_SUM                              \
    }

#define NTCI_METRIC_METADATA_MIN(name)                                        \
    {                                                                         \
        #name ".min", ntci::Monitorable::e_MINIMUM                            \
    }

#define NTCI_METRIC_METADATA_AVG(name)                                        \
    {                                                                         \
        #name ".avg", ntci::Monitorable::e_AVERAGE                            \
    }

#define NTCI_METRIC_METADATA_MAX(name)                                        \
    {                                                                         \
        #name ".max", ntci::Monitorable::e_MAXIMUM                            \
    }

#define NTCI_METRIC_METADATA_GAUGE(name)                                      \
    {                                                                         \
        #name ".current", ntci::Monitorable::e_GAUGE                          \
    }

#define NTCI_METRIC_METADATA_SUMMARY(name)                                    \
    NTCI_METRIC_METADATA_COUNT(name), NTCI_METRIC_METADATA_TOTAL(name),       \
        NTCI_METRIC_METADATA_MIN(name), NTCI_METRIC_METADATA_AVG(name),       \
        NTCI_METRIC_METADATA_MAX(name)

NTCCFG_INLINE
MetricValue::MetricValue()
: d_count(0)
, d_total(0)
, d_minimum(bsl::numeric_limits<double>::max())
, d_maximum(bsl::numeric_limits<double>::min())
, d_last(0)
{
}

NTCCFG_INLINE
void MetricValue::reset()
{
    d_count   = 0;
    d_total   = 0;
    d_minimum = bsl::numeric_limits<double>::max();
    d_maximum = bsl::numeric_limits<double>::min();
    // d_last is intentionally not reset
}

NTCCFG_INLINE
void MetricValue::update(double value)
{
    d_count   += 1;
    d_total   += value;
    d_minimum  = bsl::min(d_minimum, value);
    d_maximum  = bsl::max(d_maximum, value);
    d_last     = value;
}

NTCCFG_INLINE
bsl::uint64_t MetricValue::count() const
{
    return d_count;
}

NTCCFG_INLINE
double MetricValue::total() const
{
    return d_total;
}

NTCCFG_INLINE
double MetricValue::minimum() const
{
    return d_minimum;
}

NTCCFG_INLINE
double MetricValue::average() const
{
    return d_total / static_cast<double>(d_count);
}

NTCCFG_INLINE
double MetricValue::maximum() const
{
    return d_maximum;
}

NTCCFG_INLINE
double MetricValue::last() const
{
    return d_last;
}

NTCCFG_INLINE
Metric::Metric()
: d_lock(bsls::SpinLock::s_unlocked)
{
}

NTCCFG_INLINE
void Metric::update(double value)
{
    bsls::SpinLockGuard guard(&d_lock);

    d_value.update(value);
}

NTCCFG_INLINE
MetricTotal::MetricTotal()
: d_lock(bsls::SpinLock::s_unlocked)
, d_total(0.0)
, d_delta(0.0)
{
}

NTCCFG_INLINE
bool MetricTotal::update(double value)
{
    bsls::SpinLockGuard guard(&d_lock);

    if (value >= d_total) {
        d_delta = value - d_total;
        d_total = value;
        return true;
    }
    else {
        return false;
    }
}

NTCCFG_INLINE
MetricGauge::MetricGauge()
: d_lock(bsls::SpinLock::s_unlocked)
, d_value(0.0)
{
}

NTCCFG_INLINE
bool MetricGauge::update(double value)
{
    bsls::SpinLockGuard guard(&d_lock);
    d_value = value;
    return true;
}

}  // close package namespace
}  // close enterprise namespace

#endif
