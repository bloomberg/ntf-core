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

#ifndef INCLUDED_NTCI_PROACTORMETRICS
#define INCLUDED_NTCI_PROACTORMETRICS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_monitorable.h>
#include <ntcscm_version.h>
#include <bsls_timeinterval.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to record the runtime behavior of proactors.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_proactor
class ProactorMetrics : public ntci::Monitorable
{
  public:
    /// Destroy this object.
    virtual ~ProactorMetrics();

    /// Log the writing of the specified 'numSignals' to the controller.
    virtual void logInterrupt(bsl::size_t numSignals) = 0;

    /// Log the polling of the specified 'numReadable', 'numWritable', and
    /// 'numErrors' sockets.
    virtual void logPoll(bsl::size_t numReadable,
                         bsl::size_t numWritable,
                         bsl::size_t numErrors) = 0;

    /// Log the encounter of a socket that has been polled as readable,
    /// writable, or failed but cannot be processed because it is being
    /// processed by another thread (i.e., contributing to a thunderous
    /// herd problem.)
    virtual void logDeferredSocket() = 0;

    /// Log the encounter of a wakeup soley from a controller who has
    /// signals pending for some other thread to acknowledge than the
    /// current thread, i.e. a spurious wakeup solely from deficiencies in
    /// the controller interrupt system.
    virtual void logSpuriousWakeup() = 0;

    /// Log the specified 'duration' in the function to process a readable
    /// socket.
    virtual void logReadCallback(const bsls::TimeInterval& duration) = 0;

    /// Log the specified 'duration' in the function to process a writable
    /// socket.
    virtual void logWriteCallback(const bsls::TimeInterval& duration) = 0;

    /// Log the specified 'duration' in the function to process a readable
    /// socket.
    virtual void logErrorCallback(const bsls::TimeInterval& duration) = 0;
};

#if NTC_BUILD_WITH_METRICS

#define NTCI_PROACTORMETRICS_GET()                                            \
    ntci::ProactorMetrics* metrics = result->d_metrics_sp.get()

#define NTCI_PROACTORMETRICS_UPDATE_INTERRUPTS(numSignals)                    \
    if (metrics) {                                                            \
        metrics->logInterrupt(numSignals);                                    \
    }

#define NTCI_PROACTORMETRICS_UPDATE_POLL(numReadable, numWritable, numErrors) \
    if (metrics) {                                                            \
        metrics->logPoll(numReadable, numWritable, numErrors);                \
    }

#define NTCI_PROACTORMETRICS_UPDATE_DEFERRED_SOCKET()                         \
    if (metrics) {                                                            \
        metrics->logDeferredSocket();                                         \
    }

#define NTCI_PROACTORMETRICS_UPDATE_SPURIOUS_WAKEUP()                         \
    if (metrics) {                                                            \
        metrics->logSpuriousWakeup();                                         \
    }

#define NTCI_PROACTORMETRICS_UPDATE_ERROR_CALLBACK_TIME_BEGIN()               \
    bsl::int64_t errorProcessingStartTime;                                    \
    if (metrics) {                                                            \
        errorProcessingStartTime = bsls::TimeUtil::getTimer();                \
    }

#define NTCI_PROACTORMETRICS_UPDATE_ERROR_CALLBACK_TIME_END()                 \
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

#define NTCI_PROACTORMETRICS_UPDATE_WRITE_CALLBACK_TIME_BEGIN()               \
    bsl::int64_t writeProcessingStartTime;                                    \
    if (metrics) {                                                            \
        writeProcessingStartTime = bsls::TimeUtil::getTimer();                \
    }

#define NTCI_PROACTORMETRICS_UPDATE_WRITE_CALLBACK_TIME_END()                 \
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

#define NTCI_PROACTORMETRICS_UPDATE_READ_CALLBACK_TIME_BEGIN()                \
    bsl::int64_t readProcessingStartTime;                                     \
    if (metrics) {                                                            \
        readProcessingStartTime = bsls::TimeUtil::getTimer();                 \
    }

#define NTCI_PROACTORMETRICS_UPDATE_READ_CALLBACK_TIME_END()                  \
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

#define NTCI_PROACTORMETRICS_GET()
#define NTCI_PROACTORMETRICS_UPDATE_INTERRUPTS(numSignals)
#define NTCI_PROACTORMETRICS_UPDATE_POLL(numReadable, numWritable, numErrors)
#define NTCI_PROACTORMETRICS_UPDATE_DEFERRED_SOCKET()
#define NTCI_PROACTORMETRICS_UPDATE_SPURIOUS_WAKEUP()
#define NTCI_PROACTORMETRICS_UPDATE_ERROR_CALLBACK_TIME_BEGIN()
#define NTCI_PROACTORMETRICS_UPDATE_ERROR_CALLBACK_TIME_END()
#define NTCI_PROACTORMETRICS_UPDATE_WRITE_CALLBACK_TIME_BEGIN()
#define NTCI_PROACTORMETRICS_UPDATE_WRITE_CALLBACK_TIME_END()
#define NTCI_PROACTORMETRICS_UPDATE_READ_CALLBACK_TIME_BEGIN()
#define NTCI_PROACTORMETRICS_UPDATE_READ_CALLBACK_TIME_END()

#endif

}  // close package namespace
}  // close enterprise namespace
#endif
