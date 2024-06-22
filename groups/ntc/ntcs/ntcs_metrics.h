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

#ifndef INCLUDED_NTCS_METRICS
#define INCLUDED_NTCS_METRICS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_metric.h>
#include <ntci_monitorable.h>
#include <ntcscm_version.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide statistics for the runtime behavior of sockets.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class Metrics : public ntci::Monitorable, public ntccfg::Shared<Metrics>
{
    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex                  d_mutex;
    ntci::Metric                   d_numBytesSendable;
    ntci::Metric                   d_numBytesSent;
    ntci::Metric                   d_numBytesReceivable;
    ntci::Metric                   d_numBytesReceived;
    ntci::Metric                   d_numAcceptIterations;
    ntci::Metric                   d_numSendIterations;
    ntci::Metric                   d_numReceiveIterations;
    ntci::Metric                   d_acceptQueueSize;
    ntci::Metric                   d_acceptQueueDelay;
    ntci::Metric                   d_writeQueueSize;
    ntci::Metric                   d_writeQueueDelay;
    ntci::Metric                   d_readQueueSize;
    ntci::Metric                   d_readQueueDelay;
    ntci::Metric                   d_numConnectionsAccepted;
    ntci::Metric                   d_numConnectionsUnacceptable;
    ntci::Metric                   d_numConnectionsSynchronized;
    ntci::Metric                   d_numConnectionsUnsynchronizable;
    ntci::Metric                   d_numBytesAllocated;
    ntci::Metric                   d_txDelayBeforeScheduling;
    ntci::Metric                   d_txDelayInSoftware;
    ntci::Metric                   d_txDelay;
    ntci::Metric                   d_txDelayBeforeAcknowledgement;
    ntci::Metric                   d_rxDelayInHardware;
    ntci::Metric                   d_rxDelay;
    bsl::string                    d_prefix;
    bsl::string                    d_objectName;
    bsl::shared_ptr<ntcs::Metrics> d_parent_sp;
    bslma::Allocator*              d_allocator_p;

    static const struct ntci::MetricMetadata STATISTICS[];

  private:
    Metrics(const Metrics&) BSLS_KEYWORD_DELETED;
    Metrics& operator=(const Metrics&) BSLS_KEYWORD_DELETED;

  public:
    /// Create new metrics for the specified 'objectName whose field names
    /// have the specified 'prefix'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    Metrics(const bslstl::StringRef& prefix,
            const bslstl::StringRef& objectName,
            bslma::Allocator*        basicAllocator = 0);

    /// Create new metrics for the specified 'objectName whose field names
    /// have the specified 'prefix'. Aggregate updates into the specified
    /// 'parent'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    Metrics(const bslstl::StringRef&              prefix,
            const bslstl::StringRef&              objectName,
            const bsl::shared_ptr<ntcs::Metrics>& parent,
            bslma::Allocator*                     basicAllocator = 0);

    /// Destroy this object.
    ~Metrics() BSLS_KEYWORD_OVERRIDE;

    /// Log the synchronization of a connection.
    void logConnectCompletion();

    /// Log the failure to synchronize a connection.
    void logConnectFailure();

    /// Log the acceptance of a connection.
    void logAcceptCompletion();

    /// Log the failure to accept a connection.
    void logAcceptFailure();

    /// Log the specified 'numIterations' accepting a connection each time
    /// the listening socket has been polled as readable.
    void logAcceptIterations(bsl::size_t numIterations);

    /// Log the specified 'numBytesSendable' out of the specified
    /// 'numBytesSent'.
    void logSendCompletion(bsl::size_t numBytesSendable,
                           bsl::size_t numBytesSent);

    /// Log the specified 'numIterations' copying data out of a socket
    /// send buffer each time the socket has been polled as writable.
    void logSendIterations(bsl::size_t numIterations);

    /// Log the specified 'numBytesReceived' out of the specified
    /// 'numBytesReceivable'.
    void logReceiveCompletion(bsl::size_t numBytesReceivable,
                              bsl::size_t numBytesReceived);

    /// Log the specified 'numIterations' copying data out of a socket
    /// receive buffer each time the socket has been polled as readable.
    void logReceiveIterations(bsl::size_t numIterations);

    /// Log the gauge of the specified 'writeQueueSize'.
    void logAcceptQueueSize(bsl::size_t writeQueueSize);

    /// Log the gauge of the specified 'writeQueueDelay'.
    void logAcceptQueueDelay(const bsls::TimeInterval& writeQueueDelay);

    /// Log the gauge of the specified 'writeQueueSize'.
    void logWriteQueueSize(bsl::size_t writeQueueSize);

    /// Log the gauge of the specified 'writeQueueDelay'.
    void logWriteQueueDelay(const bsls::TimeInterval& writeQueueDelay);

    /// Log the gauge of the specified 'writeQueueSize'.
    void logReadQueueSize(bsl::size_t writeQueueSize);

    /// Log the gauge of the specified 'writeQueueDelay'.
    void logReadQueueDelay(const bsls::TimeInterval& writeQueueDelay);

    /// Log an allocation of a blob buffer with the specified
    /// 'blobBufferCapacity'.
    void logBlobBufferAllocation(bsl::size_t blobBufferCapacity);

    /// Log the gauge of the specified 'txDelayBeforeScheduling'.
    void logTxDelayBeforeScheduling(
        const bsls::TimeInterval& txDelayBeforeScheduling);

    /// Log the gauge of the specified 'txDelayInSoftware'.
    void logTxDelayInSoftware(const bsls::TimeInterval& txDelayInSoftware);

    /// Log the gauge of the specified 'txDelay'.
    void logTxDelay(const bsls::TimeInterval& txDelay);

    /// Log the gauge of the specified 'txDelayBeforeAcknowledgement'.
    void logTxDelayBeforeAcknowledgement(
        const bsls::TimeInterval& txDelayBeforeAcknowledgement);

    /// Log the gauge of the specified 'rxDelayInHardware'.
    void logRxDelayInHardware(const bsls::TimeInterval& rxDelayInHardware);

    /// Log the gauge of the specified 'rxDelay'.
    void logRxDelay(const bsls::TimeInterval& rxDelay);

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
    const bsl::shared_ptr<ntcs::Metrics>& parent() const;

    /// Set the specified 'metrics' as the metrics to use by this thread.
    /// Return the previous metrics used by this thread, if any.
    static ntcs::Metrics* setThreadLocal(ntcs::Metrics* metrics);

    /// Return the metrics to use by the current thread, if any.
    static ntcs::Metrics* getThreadLocal();
};

/// @internal @brief
/// Provide a guard to install and uninstall socket metrics into thread-local
/// storage.
///
/// @details
/// Provide a guard to automatically install a socket metrics
/// object into thread local storage when the object is constructed and
/// un-install it from thread local storage when the object is destroyed.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class MetricsGuard
{
    ntcs::Metrics* d_current_p;
    ntcs::Metrics* d_previous_p;

  private:
    MetricsGuard(const MetricsGuard&) BSLS_KEYWORD_DELETED;
    MetricsGuard& operator=(const MetricsGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new metrics guard that installs the specified 'metrics'
    /// object into thread local storage and uninstalls it when this object
    /// is destroyed.
    explicit MetricsGuard(ntcs::Metrics* metrics);

    /// Uninstall the underlying metrics object from thread local storage
    /// then destroy this object.
    ~MetricsGuard();
};

NTCCFG_INLINE
MetricsGuard::MetricsGuard(ntcs::Metrics* metrics)
: d_current_p(metrics)
, d_previous_p(0)
{
    if (d_current_p) {
        d_previous_p = ntcs::Metrics::setThreadLocal(d_current_p);
    }
}

NTCCFG_INLINE
MetricsGuard::~MetricsGuard()
{
    if (d_current_p) {
        ntcs::Metrics::setThreadLocal(d_previous_p);
    }
}

#if NTC_BUILD_WITH_METRICS

#define NTCS_METRICS_UPDATE_ACCEPT_COMPLETE()                                 \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logAcceptCompletion();                              \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_ACCEPT_FAILURE()                                  \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logAcceptFailure();                                 \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_ACCEPT_ITERATIONS(numIterations)                  \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logAcceptIterations(numIterations);                 \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_ACCEPT_QUEUE_SIZE(acceptQueueSize)                \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logAcceptQueueSize(acceptQueueSize);                \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_ACCEPT_QUEUE_DELAY(acceptQueueDelay)              \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logAcceptQueueDelay(acceptQueueDelay);              \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_CONNECT_COMPLETE()                                \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logConnectCompletion();                             \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_CONNECT_FAILURE()                                 \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logConnectFailure();                                \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_SEND_COMPLETE(context)                            \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logSendCompletion((context).bytesSendable(),        \
                                            (context).bytesSent());           \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_SEND_ITERATIONS(numIterations)                    \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logSendIterations(numIterations);                   \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_RECEIVE_COMPLETE(context)                         \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logReceiveCompletion((context).bytesReceivable(),   \
                                               (context).bytesReceived());    \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_RECEIVE_ITERATIONS(numIterations)                 \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logReceiveIterations(numIterations);                \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_WRITE_QUEUE_SIZE(writeQueueSize)                  \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logWriteQueueSize(writeQueueSize);                  \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_WRITE_QUEUE_DELAY(writeQueueDelay)                \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logWriteQueueDelay(writeQueueDelay);                \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_READ_QUEUE_SIZE(readQueueSize)                    \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logReadQueueSize(readQueueSize);                    \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_READ_QUEUE_DELAY(readQueueDelay)                  \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logReadQueueDelay(readQueueDelay);                  \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_BLOB_BUFFER_ALLOCATIONS(capacity)                 \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logBlobBufferAllocation(capacity);                  \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_TX_DELAY_BEFORE_SCHEDULING(delay)                 \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logTxDelayBeforeScheduling(delay);                  \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_TX_DELAY_IN_SOFTWARE(delay)                       \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logTxDelayInSoftware(delay);                        \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_TX_DELAY(delay)                                   \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logTxDelay(delay);                                  \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_TX_DELAY_BEFORE_ACKNOWLEDGEMENT(delay)            \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logTxDelayBeforeAcknowledgement(delay);             \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_RX_DELAY_IN_HARDWARE(rxDelayInHardware)           \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logRxDelayInHardware(rxDelayInHardware);            \
        }                                                                     \
    } while (false)

#define NTCS_METRICS_UPDATE_RX_DELAY(rxDelay)                                 \
    do {                                                                      \
        if (d_metrics_sp) {                                                   \
            d_metrics_sp->logRxDelay(rxDelay);                                \
        }                                                                     \
    } while (false)

#else

#define NTCS_METRICS_UPDATE_ACCEPT_COMPLETE()
#define NTCS_METRICS_UPDATE_ACCEPT_FAILURE()
#define NTCS_METRICS_UPDATE_ACCEPT_ITERATIONS(numIterations)
#define NTCS_METRICS_UPDATE_ACCEPT_QUEUE_SIZE(acceptQueueSize)
#define NTCS_METRICS_UPDATE_ACCEPT_QUEUE_DELAY(acceptQueueDelay)

#define NTCS_METRICS_UPDATE_CONNECT_COMPLETE()
#define NTCS_METRICS_UPDATE_CONNECT_FAILURE()

#define NTCS_METRICS_UPDATE_SEND_COMPLETE(context)
#define NTCS_METRICS_UPDATE_SEND_ITERATIONS(numIterations)

#define NTCS_METRICS_UPDATE_RECEIVE_COMPLETE(context)
#define NTCS_METRICS_UPDATE_RECEIVE_ITERATIONS(numIterations)

#define NTCS_METRICS_UPDATE_WRITE_QUEUE_SIZE(writeQueueSize)
#define NTCS_METRICS_UPDATE_WRITE_QUEUE_DELAY(writeQueueDelay)

#define NTCS_METRICS_UPDATE_READ_QUEUE_SIZE(readQueueSize)
#define NTCS_METRICS_UPDATE_READ_QUEUE_DELAY(readQueueDelay)

#define NTCS_METRICS_UPDATE_BLOB_BUFFER_ALLOCATIONS(capacity)

#define NTCS_METRICS_UPDATE_TX_DELAY_BEFORE_SCHEDULING(delay)
#define NTCS_METRICS_UPDATE_TX_DELAY_IN_SOFTWARE(delay)
#define NTCS_METRICS_UPDATE_TX_DELAY_BEFORE_ACKNOWLEDGEMENT(delay)
#define NTCS_METRICS_UPDATE_TX_DELAY(delay)

#define NTCS_METRICS_UPDATE_RX_DELAY_IN_HARDWARE(rxDelayInHardware)
#define NTCS_METRICS_UPDATE_RX_DELAY(rxDelay)

#endif

}  // close package namespace
}  // close enterprise namespace
#endif
