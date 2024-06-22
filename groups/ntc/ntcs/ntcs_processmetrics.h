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

#ifndef INCLUDED_NTCS_PROCESSMETRICS
#define INCLUDED_NTCS_PROCESSMETRICS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_metric.h>
#include <ntci_monitorable.h>
#include <ntcs_processstatistics.h>
#include <ntcscm_version.h>
#include <bslmt_mutex.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide metrics for the runtime behavior of the process.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class ProcessMetrics : public ntci::Monitorable,
                       public ntccfg::Shared<ProcessMetrics>
{
    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex        d_mutex;
    ntci::MetricTotal    d_cpuTimeUser;
    ntci::MetricTotal    d_cpuTimeSystem;
    ntci::MetricGauge    d_memoryResident;
    ntci::MetricGauge    d_memoryAddressSpace;
    ntci::MetricTotal    d_contextSwitchesUser;
    ntci::MetricTotal    d_contextSwitchesSystem;
    ntci::MetricTotal    d_pageFaultsMajor;
    ntci::MetricTotal    d_pageFaultsMinor;
    bsl::string          d_prefix;
    bsl::string          d_objectName;
    bslma::Allocator*    d_allocator_p;

    static const struct ntci::MetricMetadata STATISTICS[];

  private:
    ProcessMetrics(const ProcessMetrics&) BSLS_KEYWORD_DELETED;
    ProcessMetrics& operator=(const ProcessMetrics&) BSLS_KEYWORD_DELETED;

  private:
    /// Collect process metrics.
    void collect();

  public:
    /// Create new metrics for the specified 'objectName' whose field names
    /// have the specified 'prefix'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    ProcessMetrics(const bslstl::StringRef& prefix,
                   const bslstl::StringRef& objectName,
                   bslma::Allocator*        basicAllocator = 0);

    /// Destroy this object.
    ~ProcessMetrics() BSLS_KEYWORD_OVERRIDE;

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
};

}  // close package namespace
}  // close enterprise namespace
#endif
