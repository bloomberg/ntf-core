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

#ifndef INCLUDED_NTCS_PROACTORMETRICS
#define INCLUDED_NTCS_PROACTORMETRICS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_metric.h>
#include <ntci_monitorable.h>
#include <ntci_proactormetrics.h>
#include <ntcscm_version.h>

#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>

#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide statistics for the runtime behavior of proactors.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class ProactorMetrics : public ntci::ProactorMetrics,
                        public ntccfg::Shared<ProactorMetrics>
{
    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex                          d_mutex;
    ntci::Metric                           d_numInterrupts;
    ntci::Metric                           d_numReadablePerPoll;
    ntci::Metric                           d_numWritablePerPoll;
    ntci::Metric                           d_numErrorsPerPoll;
    ntci::Metric                           d_numSocketsDeferred;
    ntci::Metric                           d_numWakeupsSpurious;
    ntci::Metric                           d_readProcessingTime;
    ntci::Metric                           d_writeProcessingTime;
    ntci::Metric                           d_errorProcessingTime;
    bsl::string                            d_prefix;
    bsl::string                            d_objectName;
    bsl::shared_ptr<ntci::ProactorMetrics> d_parent_sp;
    bslma::Allocator*                      d_allocator_p;

    static const struct ntci::MetricMetadata STATISTICS[];

  private:
    ProactorMetrics(const ProactorMetrics&) BSLS_KEYWORD_DELETED;
    ProactorMetrics& operator=(const ProactorMetrics&) BSLS_KEYWORD_DELETED;

  public:
    /// Create new metrics for the specified 'objectName whose field names
    /// have the specified 'prefix'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    ProactorMetrics(const bslstl::StringRef& prefix,
                    const bslstl::StringRef& objectName,
                    bslma::Allocator*        basicAllocator = 0);

    /// Create new metrics for the specified 'objectName whose field names
    /// have the specified 'prefix'. Aggregate updates into the specified
    /// 'parent'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    ProactorMetrics(const bslstl::StringRef&                      prefix,
                    const bslstl::StringRef&                      objectName,
                    const bsl::shared_ptr<ntci::ProactorMetrics>& parent,
                    bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ProactorMetrics() BSLS_KEYWORD_OVERRIDE;

    /// Log the writing of the specified 'numSignals' to the controller.
    void logInterrupt(bsl::size_t numSignals) BSLS_KEYWORD_OVERRIDE;

    /// Log the polling of the specified 'numReadable', 'numWritable', and
    /// 'numErrors' sockets.
    void logPoll(bsl::size_t numReadable,
                 bsl::size_t numWritable,
                 bsl::size_t numErrors) BSLS_KEYWORD_OVERRIDE;

    /// Log the encounter of a socket that has been polled as readable,
    /// writable, or failed but cannot be processed because it is being
    /// processed by another thread (i.e., contributing to a thunderous
    /// herd problem.)
    void logDeferredSocket() BSLS_KEYWORD_OVERRIDE;

    /// Log the encounter of a wakeup soley from a controller who has
    /// signals pending for some other thread to acknowledge than the
    /// current thread, i.e. a spurious wakeup solely from deficiencies in
    /// the controller interrupt system.
    void logSpuriousWakeup() BSLS_KEYWORD_OVERRIDE;

    /// Log the specified 'duration' in the function to process a readable
    /// socket.
    void logReadCallback(const bsls::TimeInterval& duration)
        BSLS_KEYWORD_OVERRIDE;

    /// Log the specified 'duration' in the function to process a writable
    /// socket.
    void logWriteCallback(const bsls::TimeInterval& duration)
        BSLS_KEYWORD_OVERRIDE;

    /// Log the specified 'duration' in the function to process a readable
    /// socket.
    void logErrorCallback(const bsls::TimeInterval& duration)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the array of statistics from the
    /// specified 'snapshot' for this object based on the specified
    /// 'operation': if 'operation' is e_CUMULATIVE then the statistics are
    /// for the entire life of this object;  otherwise the statistics are
    /// for the period since the last call to this function. If 'operation'
    /// is e_INTERVAL_WITH_RESET then reset all internal measurements.  Note
    /// that 'result->theArray().length()' is expected to have the same
    /// value each time this function returns.
    void getStats(bdld::ManagedDatum* result) BSLS_KEYWORD_OVERRIDE;

    /// Return the prefix corresponding to the field at the specified
    /// 'ordinal' position, or 0 if no field at the 'ordinal' position
    /// exists.
    const char* getFieldPrefix(int ordinal) const BSLS_KEYWORD_OVERRIDE;

    /// Return the field name corresponding to the field at the specified
    /// 'ordinal' position, or 0 if no field at the 'ordinal' position
    /// exists.
    const char* getFieldName(int ordinal) const BSLS_KEYWORD_OVERRIDE;

    /// Return the field description corresponding to the field at the
    /// specified 'ordinal' position, or 0 if no field at the 'ordinal'
    /// position exists.
    const char* getFieldDescription(int ordinal) const BSLS_KEYWORD_OVERRIDE;

    /// Return the type of the statistic at the specified 'ordinal'
    /// position, or e_AVERAGE if no field at the 'ordinal' position exists
    /// or the type is unknown.
    ntci::Monitorable::StatisticType getFieldType(int ordinal) const
        BSLS_KEYWORD_OVERRIDE;

    /// Return the flags that indicate which indexes to apply to the
    /// statistics measured by this monitorable object.
    int getFieldTags(int ordinal) const BSLS_KEYWORD_OVERRIDE;

    /// Return the ordinal of the specified 'fieldName', or a negative value
    /// if no field identified by 'fieldName' exists.
    int getFieldOrdinal(const char* fieldName) const BSLS_KEYWORD_OVERRIDE;

    /// Return the maximum number of elements in a datum resulting from
    /// a call to 'getStats()'.
    int numOrdinals() const BSLS_KEYWORD_OVERRIDE;

    /// Return the human-readable name of the monitorable object, or 0 or
    /// the empty string if no such human-readable name has been assigned to
    /// the monitorable object.
    const char* objectName() const BSLS_KEYWORD_OVERRIDE;

    /// Return the parent metrics object into which these metrics are
    /// aggregated, or null if no such parent object is defined.
    const bsl::shared_ptr<ntci::ProactorMetrics>& parent() const;

    /// Set the specified 'metrics' as the metrics to use by this thread.
    /// Return the previous metrics used by this thread, if any.
    static ntcs::ProactorMetrics* setThreadLocal(
        ntcs::ProactorMetrics* metrics);

    /// Return the metrics to use by the current thread, if any.
    static ntcs::ProactorMetrics* getThreadLocal();
};

/// @internal @brief
/// Provide a guard to install and uninstall proactor metrics into thread-local
/// storage.
///
/// @details
/// Provide a guard to automatically install a proactor metrics
/// object into thread local storage when the object is constructed and
/// un-install it from thread local storage when the object is destroyed.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class ProactorMetricsGuard
{
    ntcs::ProactorMetrics* d_current_p;
    ntcs::ProactorMetrics* d_previous_p;

  private:
    ProactorMetricsGuard(const ProactorMetricsGuard&) BSLS_KEYWORD_DELETED;
    ProactorMetricsGuard& operator=(const ProactorMetricsGuard&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new metrics guard that installs the specified 'metrics'
    /// object into thread local storage and uninstalls it when this object
    /// is destroyed.
    explicit ProactorMetricsGuard(ntcs::ProactorMetrics* metrics);

    /// Uninstall the underlying metrics object from thread local storage
    /// then destroy this object.
    ~ProactorMetricsGuard();
};

NTCCFG_INLINE
ProactorMetricsGuard::ProactorMetricsGuard(ntcs::ProactorMetrics* metrics)
: d_current_p(metrics)
, d_previous_p(0)
{
    if (d_current_p) {
        d_previous_p = ntcs::ProactorMetrics::setThreadLocal(d_current_p);
    }
}

NTCCFG_INLINE
ProactorMetricsGuard::~ProactorMetricsGuard()
{
    if (d_current_p) {
        ntcs::ProactorMetrics::setThreadLocal(d_previous_p);
    }
}

#if NTC_BUILD_WITH_METRICS

#define NTCS_PROACTORMETRICS_GET()                                            \
    ntci::ProactorMetrics* metrics = result->d_metrics_sp.get()

#define NTCS_PROACTORMETRICS_UPDATE_INTERRUPTS(numSignals)                    \
    if (metrics) {                                                            \
        metrics->logInterrupt(numSignals);                                    \
    }

#define NTCS_PROACTORMETRICS_UPDATE_POLL(numReadable, numWritable, numErrors) \
    if (metrics) {                                                            \
        metrics->logPoll(numReadable, numWritable, numErrors);                \
    }

#define NTCS_PROACTORMETRICS_UPDATE_DEFERRED_SOCKET()                         \
    if (metrics) {                                                            \
        metrics->logDeferredSocket();                                         \
    }

#define NTCS_PROACTORMETRICS_UPDATE_SPURIOUS_WAKEUP()                         \
    if (metrics) {                                                            \
        metrics->logSpuriousWakeup();                                         \
    }

#define NTCS_PROACTORMETRICS_UPDATE_ERROR_CALLBACK_TIME_BEGIN()               \
    bsl::int64_t errorProcessingStartTime;                                    \
    if (metrics) {                                                            \
        errorProcessingStartTime = bsls::TimeUtil::getTimer();                \
    }

#define NTCS_PROACTORMETRICS_UPDATE_ERROR_CALLBACK_TIME_END()                 \
    if (metrics) {                                                            \
        bsl::int64_t errorProcessingStopTime = bsls::TimeUtil::getTimer();    \
        bsl::int64_t errorProcessingTime =                                    \
            errorProcessingStopTime - errorProcessingStartTime;               \
        if (errorProcessingTime < 0) {                                        \
            errorProcessingTime = 0;                                          \
        }                                                                     \
        bsls::TimeInterval duration;                                          \
        duration.setTotalNanoseconds(errorProcessingTime);                    \
        metrics->logErrorCallback(duration);                                  \
    }

#define NTCS_PROACTORMETRICS_UPDATE_WRITE_CALLBACK_TIME_BEGIN()               \
    bsl::int64_t writeProcessingStartTime;                                    \
    if (metrics) {                                                            \
        writeProcessingStartTime = bsls::TimeUtil::getTimer();                \
    }

#define NTCS_PROACTORMETRICS_UPDATE_WRITE_CALLBACK_TIME_END()                 \
    if (metrics) {                                                            \
        bsl::int64_t writeProcessingStopTime = bsls::TimeUtil::getTimer();    \
        bsl::int64_t writeProcessingTime =                                    \
            writeProcessingStopTime - writeProcessingStartTime;               \
        if (writeProcessingTime < 0) {                                        \
            writeProcessingTime = 0;                                          \
        }                                                                     \
        bsls::TimeInterval duration;                                          \
        duration.setTotalNanoseconds(writeProcessingTime);                    \
        metrics->logWriteCallback(duration);                                  \
    }

#define NTCS_PROACTORMETRICS_UPDATE_READ_CALLBACK_TIME_BEGIN()                \
    bsl::int64_t readProcessingStartTime;                                     \
    if (metrics) {                                                            \
        readProcessingStartTime = bsls::TimeUtil::getTimer();                 \
    }

#define NTCS_PROACTORMETRICS_UPDATE_READ_CALLBACK_TIME_END()                  \
    if (metrics) {                                                            \
        bsl::int64_t readProcessingStopTime = bsls::TimeUtil::getTimer();     \
        bsl::int64_t readProcessingTime =                                     \
            readProcessingStopTime - readProcessingStartTime;                 \
        if (readProcessingTime < 0) {                                         \
            readProcessingTime = 0;                                           \
        }                                                                     \
        bsls::TimeInterval duration;                                          \
        duration.setTotalNanoseconds(readProcessingTime);                     \
        metrics->logReadCallback(duration);                                   \
    }

#else

#define NTCS_PROACTORMETRICS_GET()
#define NTCS_PROACTORMETRICS_UPDATE_INTERRUPTS(numSignals)
#define NTCS_PROACTORMETRICS_UPDATE_POLL(numReadable, numWritable, numErrors)
#define NTCS_PROACTORMETRICS_UPDATE_DEFERRED_SOCKET()
#define NTCS_PROACTORMETRICS_UPDATE_SPURIOUS_WAKEUP()
#define NTCS_PROACTORMETRICS_UPDATE_ERROR_CALLBACK_TIME_BEGIN()
#define NTCS_PROACTORMETRICS_UPDATE_ERROR_CALLBACK_TIME_END()
#define NTCS_PROACTORMETRICS_UPDATE_WRITE_CALLBACK_TIME_BEGIN()
#define NTCS_PROACTORMETRICS_UPDATE_WRITE_CALLBACK_TIME_END()
#define NTCS_PROACTORMETRICS_UPDATE_READ_CALLBACK_TIME_BEGIN()
#define NTCS_PROACTORMETRICS_UPDATE_READ_CALLBACK_TIME_END()

#endif

}  // close package namespace
}  // close enterprise namespace
#endif
