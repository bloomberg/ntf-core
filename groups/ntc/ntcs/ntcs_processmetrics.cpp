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

#include <ntcs_processmetrics.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_processmetrics_cpp, "$Id$ $CSID$")

#include <ntcs_processutil.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcs {

const ntci::MetricMetadata ProcessMetrics::STATISTICS[] = {
    NTCI_METRIC_METADATA_TOTAL(cpuTimeUser),
    NTCI_METRIC_METADATA_TOTAL(cpuTimeSystem),
    NTCI_METRIC_METADATA_GAUGE(memoryResident),
    NTCI_METRIC_METADATA_GAUGE(memoryAddressSpace),
    NTCI_METRIC_METADATA_TOTAL(contextSwitchesUser),
    NTCI_METRIC_METADATA_TOTAL(contextSwitchesSystem),
    NTCI_METRIC_METADATA_TOTAL(pageFaultsMajor),
    NTCI_METRIC_METADATA_TOTAL(pageFaultsMinor),
};

void ProcessMetrics::collect()
{
    ntcs::ProcessStatistics current;
    ntcs::ProcessUtil::getResourceUsage(&current);

    if (!current.cpuTimeUser().isNull()) {
        d_cpuTimeUser.update(
            current.cpuTimeUser().value().totalSecondsAsDouble());
    }

    if (!current.cpuTimeSystem().isNull()) {
        d_cpuTimeSystem.update(
            current.cpuTimeSystem().value().totalSecondsAsDouble());
    }

    if (!current.memoryResident().isNull()) {
        d_memoryResident.update(
            static_cast<double>(current.memoryResident().value()));
    }

    if (!current.memoryAddressSpace().isNull()) {
        d_memoryAddressSpace.update(
            static_cast<double>(current.memoryAddressSpace().value()));
    }

    if (!current.contextSwitchesUser().isNull()) {
        d_contextSwitchesUser.update(
            static_cast<double>(current.contextSwitchesUser().value()));
    }

    if (!current.contextSwitchesSystem().isNull()) {
        d_contextSwitchesSystem.update(
            static_cast<double>(current.contextSwitchesSystem().value()));
    }

    if (!current.pageFaultsMajor().isNull()) {
        d_pageFaultsMajor.update(
            static_cast<double>(current.pageFaultsMajor().value()));
    }

    if (!current.pageFaultsMinor().isNull()) {
        d_pageFaultsMinor.update(
            static_cast<double>(current.pageFaultsMinor().value()));
    }
}

ProcessMetrics::ProcessMetrics(const bslstl::StringRef& prefix,
                               const bslstl::StringRef& objectName,
                               bslma::Allocator*        basicAllocator)
: d_mutex()
, d_cpuTimeUser()
, d_cpuTimeSystem()
, d_memoryResident()
, d_memoryAddressSpace()
, d_contextSwitchesUser()
, d_contextSwitchesSystem()
, d_pageFaultsMajor()
, d_pageFaultsMinor()
, d_prefix(prefix, basicAllocator)
, d_objectName(objectName, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->collect();
}

ProcessMetrics::~ProcessMetrics()
{
}

void ProcessMetrics::getStats(bdld::ManagedDatum* result)
{
    LockGuard guard(&d_mutex);

    this->collect();

    bdld::DatumMutableArrayRef array;
    bdld::Datum::createUninitializedArray(&array,
                                          numOrdinals(),
                                          result->allocator());

    bsl::size_t index = 0;

    d_cpuTimeUser.collectTotal(&array, &index);
    d_cpuTimeSystem.collectTotal(&array, &index);
    d_memoryResident.collectLast(&array, &index);
    d_memoryAddressSpace.collectLast(&array, &index);
    d_contextSwitchesUser.collectTotal(&array, &index);
    d_contextSwitchesSystem.collectTotal(&array, &index);
    d_pageFaultsMajor.collectTotal(&array, &index);
    d_pageFaultsMinor.collectTotal(&array, &index);

    *array.length() = numOrdinals();

    result->adopt(bdld::Datum::adoptArray(array));
}

const char* ProcessMetrics::getFieldPrefix(int ordinal) const
{
    NTCCFG_WARNING_UNUSED(ordinal);

    return d_prefix.c_str();
}

const char* ProcessMetrics::getFieldName(int ordinal) const
{
    if (ordinal < numOrdinals()) {
        return ProcessMetrics::STATISTICS[ordinal].d_name;
    }
    else {
        return 0;
    }
}

const char* ProcessMetrics::getFieldDescription(int ordinal) const
{
    NTCCFG_WARNING_UNUSED(ordinal);

    return "";
}

ntci::Monitorable::StatisticType ProcessMetrics::getFieldType(
    int ordinal) const
{
    if (ordinal < numOrdinals()) {
        return ProcessMetrics::STATISTICS[ordinal].d_type;
    }
    else {
        return ntci::Monitorable::e_AVERAGE;
    }
}

int ProcessMetrics::getFieldTags(int ordinal) const
{
    NTCCFG_WARNING_UNUSED(ordinal);

    return ntci::Monitorable::e_ANONYMOUS;
}

int ProcessMetrics::getFieldOrdinal(const char* fieldName) const
{
    int result = 0;

    for (int ordinal = 0; ordinal < numOrdinals(); ++ordinal) {
        if (bsl::strcmp(ProcessMetrics::STATISTICS[ordinal].d_name,
                        fieldName) == 0)
        {
            result = ordinal;
        }
    }

    return result;
}

int ProcessMetrics::numOrdinals() const
{
    return sizeof ProcessMetrics::STATISTICS /
           sizeof ProcessMetrics::STATISTICS[0];
}

const char* ProcessMetrics::objectName() const
{
    return d_objectName.c_str();
}

}  // close package namespace
}  // close enterprise namespace
