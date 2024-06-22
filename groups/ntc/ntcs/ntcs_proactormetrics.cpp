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

#include <ntcs_proactormetrics.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_proactormetrics_cpp, "$Id$ $CSID$")

#include <bslmt_lockguard.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcs {

namespace {

bslmt::ThreadUtil::Key s_key;

struct Initializer {
    Initializer()
    {
        int rc = bslmt::ThreadUtil::createKey(&s_key, 0);
        BSLS_ASSERT_OPT(rc == 0);
    }

    ~Initializer()
    {
        // MRM: int rc = bslmt::ThreadUtil::deleteKey(s_key);
        // MRM: BSLS_ASSERT_OPT(rc == 0);
    }
} s_initializer;

}  // close unnamed namespace

const ntci::MetricMetadata ProactorMetrics::STATISTICS[] = {
    NTCI_METRIC_METADATA_SUMMARY(interrupts),
    NTCI_METRIC_METADATA_SUMMARY(socketsReadable),
    NTCI_METRIC_METADATA_SUMMARY(socketsWritable),
    NTCI_METRIC_METADATA_SUMMARY(socketsFailed),
    NTCI_METRIC_METADATA_SUMMARY(socketsDeferred),
    NTCI_METRIC_METADATA_SUMMARY(wakeupsSpurious),
    NTCI_METRIC_METADATA_SUMMARY(timeProcessingRead),
    NTCI_METRIC_METADATA_SUMMARY(timeProcessingWrite),
    NTCI_METRIC_METADATA_SUMMARY(timeProcessingError)};

ProactorMetrics::ProactorMetrics(const bslstl::StringRef& prefix,
                                 const bslstl::StringRef& objectName,
                                 bslma::Allocator*        basicAllocator)
: d_mutex()
, d_numInterrupts()
, d_numReadablePerPoll()
, d_numWritablePerPoll()
, d_numErrorsPerPoll()
, d_numSocketsDeferred()
, d_numWakeupsSpurious()
, d_readProcessingTime()
, d_writeProcessingTime()
, d_errorProcessingTime()
, d_prefix(prefix, basicAllocator)
, d_objectName(objectName, basicAllocator)
, d_parent_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ProactorMetrics::ProactorMetrics(
    const bslstl::StringRef&                      prefix,
    const bslstl::StringRef&                      objectName,
    const bsl::shared_ptr<ntci::ProactorMetrics>& parent,
    bslma::Allocator*                             basicAllocator)
: d_mutex()
, d_numInterrupts()
, d_numReadablePerPoll()
, d_numWritablePerPoll()
, d_numErrorsPerPoll()
, d_numSocketsDeferred()
, d_numWakeupsSpurious()
, d_readProcessingTime()
, d_writeProcessingTime()
, d_errorProcessingTime()
, d_prefix(basicAllocator)
, d_objectName(basicAllocator)
, d_parent_sp(parent)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_parent_sp) {
        d_prefix.append(d_parent_sp->getFieldPrefix(0));
        d_prefix.append(1, '.');

        d_objectName.append(d_parent_sp->objectName());
        d_objectName.append(1, '-');
    }

    d_prefix.append(prefix);
    d_objectName.append(objectName);
}

ProactorMetrics::~ProactorMetrics()
{
}

// MANNIPULATORS
void ProactorMetrics::logInterrupt(bsl::size_t numSignals)
{
    d_numInterrupts.update(static_cast<double>(numSignals));

    if (d_parent_sp) {
        d_parent_sp->logInterrupt(numSignals);
    }
}

void ProactorMetrics::logPoll(bsl::size_t numReadable,
                              bsl::size_t numWritable,
                              bsl::size_t numErrors)
{
    d_numReadablePerPoll.update(static_cast<double>(numReadable));
    d_numWritablePerPoll.update(static_cast<double>(numWritable));
    d_numErrorsPerPoll.update(static_cast<double>(numErrors));

    if (d_parent_sp) {
        d_parent_sp->logPoll(numReadable, numWritable, numErrors);
    }
}

void ProactorMetrics::logDeferredSocket()
{
    d_numSocketsDeferred.update(1);

    if (d_parent_sp) {
        d_parent_sp->logDeferredSocket();
    }
}

void ProactorMetrics::logSpuriousWakeup()
{
    d_numWakeupsSpurious.update(1);

    if (d_parent_sp) {
        d_parent_sp->logSpuriousWakeup();
    }
}

void ProactorMetrics::logReadCallback(const bsls::TimeInterval& duration)
{
    d_readProcessingTime.update(duration.totalSecondsAsDouble());

    if (d_parent_sp) {
        d_parent_sp->logReadCallback(duration);
    }
}

void ProactorMetrics::logWriteCallback(const bsls::TimeInterval& duration)
{
    d_writeProcessingTime.update(duration.totalSecondsAsDouble());

    if (d_parent_sp) {
        d_parent_sp->logWriteCallback(duration);
    }
}

void ProactorMetrics::logErrorCallback(const bsls::TimeInterval& duration)
{
    d_errorProcessingTime.update(duration.totalSecondsAsDouble());

    if (d_parent_sp) {
        d_parent_sp->logErrorCallback(duration);
    }
}

void ProactorMetrics::getStats(bdld::ManagedDatum* result)
{
    LockGuard guard(&d_mutex);

    bdld::DatumMutableArrayRef array;
    bdld::Datum::createUninitializedArray(&array,
                                          numOrdinals(),
                                          result->allocator());

    bsl::size_t index = 0;

    d_numInterrupts.collectSummary(&array, &index);

    d_numReadablePerPoll.collectSummary(&array, &index);

    d_numWritablePerPoll.collectSummary(&array, &index);

    d_numErrorsPerPoll.collectSummary(&array, &index);

    d_numSocketsDeferred.collectSummary(&array, &index);

    d_numWakeupsSpurious.collectSummary(&array, &index);

    d_readProcessingTime.collectSummary(&array, &index);

    d_writeProcessingTime.collectSummary(&array, &index);

    d_errorProcessingTime.collectSummary(&array, &index);

    *array.length() = numOrdinals();

    result->adopt(bdld::Datum::adoptArray(array));
}

const char* ProactorMetrics::getFieldPrefix(int ordinal) const
{
    NTCCFG_WARNING_UNUSED(ordinal);

    return d_prefix.c_str();
}

const char* ProactorMetrics::getFieldName(int ordinal) const
{
    if (ordinal < numOrdinals()) {
        return ProactorMetrics::STATISTICS[ordinal].d_name;
    }
    else {
        return 0;
    }
}

const char* ProactorMetrics::getFieldDescription(int ordinal) const
{
    NTCCFG_WARNING_UNUSED(ordinal);

    return "";
}

ntci::Monitorable::StatisticType ProactorMetrics::getFieldType(
    int ordinal) const
{
    if (ordinal < numOrdinals()) {
        return ProactorMetrics::STATISTICS[ordinal].d_type;
    }
    else {
        return ntci::Monitorable::e_AVERAGE;
    }
}

int ProactorMetrics::getFieldTags(int ordinal) const
{
    NTCCFG_WARNING_UNUSED(ordinal);

    return ntci::Monitorable::e_ANONYMOUS;
}

int ProactorMetrics::getFieldOrdinal(const char* fieldName) const
{
    int result = 0;

    for (int ordinal = 0; ordinal < numOrdinals(); ++ordinal) {
        if (bsl::strcmp(ProactorMetrics::STATISTICS[ordinal].d_name,
                        fieldName) == 0)
        {
            result = ordinal;
        }
    }

    return result;
}

int ProactorMetrics::numOrdinals() const
{
    return sizeof ProactorMetrics::STATISTICS /
           sizeof ProactorMetrics::STATISTICS[0];
}

const char* ProactorMetrics::objectName() const
{
    return d_objectName.c_str();
}

const bsl::shared_ptr<ntci::ProactorMetrics>& ProactorMetrics::parent() const
{
    return d_parent_sp;
}

ntcs::ProactorMetrics* ProactorMetrics::setThreadLocal(
    ntcs::ProactorMetrics* metrics)
{
    ntcs::ProactorMetrics* previous = reinterpret_cast<ntcs::ProactorMetrics*>(
        bslmt::ThreadUtil::getSpecific(s_key));

    int rc = bslmt::ThreadUtil::setSpecific(
        s_key,
        const_cast<const void*>(static_cast<void*>(metrics)));
    BSLS_ASSERT_OPT(rc == 0);

    return previous;
}

ntcs::ProactorMetrics* ProactorMetrics::getThreadLocal()
{
    ntcs::ProactorMetrics* current = reinterpret_cast<ntcs::ProactorMetrics*>(
        bslmt::ThreadUtil::getSpecific(s_key));

    return current;
}

}  // close package namespace
}  // close enterprise namespace
