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

#include <ntcs_metrics.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_metrics_cpp, "$Id$ $CSID$")

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

const ntci::MetricMetadata Metrics::STATISTICS[] = {
    NTCI_METRIC_METADATA_SUMMARY(bytesSendable),
    NTCI_METRIC_METADATA_SUMMARY(bytesSent),

    NTCI_METRIC_METADATA_SUMMARY(bytesReceivable),
    NTCI_METRIC_METADATA_SUMMARY(bytesReceived),

    NTCI_METRIC_METADATA_SUMMARY(iterationsAccepting),
    NTCI_METRIC_METADATA_SUMMARY(iterationsSending),
    NTCI_METRIC_METADATA_SUMMARY(iterationsReceiving),

    NTCI_METRIC_METADATA_SUMMARY(connectionsInAcceptQueue),
    NTCI_METRIC_METADATA_SUMMARY(delayInAcceptQueue),

    NTCI_METRIC_METADATA_SUMMARY(bytesInWriteQueue),
    NTCI_METRIC_METADATA_SUMMARY(delayInWriteQueue),

    NTCI_METRIC_METADATA_SUMMARY(bytesInReadQueue),
    NTCI_METRIC_METADATA_SUMMARY(delayInReadQueue),

    NTCI_METRIC_METADATA_SUMMARY(connectionsAccepted),
    NTCI_METRIC_METADATA_SUMMARY(connectionsUnacceptable),

    NTCI_METRIC_METADATA_SUMMARY(connectionsSynchronized),
    NTCI_METRIC_METADATA_SUMMARY(connectionsUnsynchronizable),

    NTCI_METRIC_METADATA_SUMMARY(bytesAllocated),

    NTCI_METRIC_METADATA_SUMMARY(txDelayBeforeScheduling),
    NTCI_METRIC_METADATA_SUMMARY(txDelayInSoftware),
    NTCI_METRIC_METADATA_SUMMARY(txDelay),
    NTCI_METRIC_METADATA_SUMMARY(txDelayBeforeAcknowledgement),

    NTCI_METRIC_METADATA_SUMMARY(rxDelayInHardware),
    NTCI_METRIC_METADATA_SUMMARY(rxDelay)};

Metrics::Metrics(const bslstl::StringRef& prefix,
                 const bslstl::StringRef& objectName,
                 bslma::Allocator*        basicAllocator)
: d_mutex()
, d_numBytesSendable()
, d_numBytesSent()
, d_numBytesReceivable()
, d_numBytesReceived()
, d_numAcceptIterations()
, d_numSendIterations()
, d_numReceiveIterations()
, d_acceptQueueSize()
, d_acceptQueueDelay()
, d_writeQueueSize()
, d_writeQueueDelay()
, d_readQueueSize()
, d_readQueueDelay()
, d_numConnectionsAccepted()
, d_numConnectionsUnacceptable()
, d_numConnectionsSynchronized()
, d_numConnectionsUnsynchronizable()
, d_numBytesAllocated()
, d_txDelayBeforeScheduling()
, d_txDelayInSoftware()
, d_txDelay()
, d_txDelayBeforeAcknowledgement()
, d_rxDelayInHardware()
, d_rxDelay()
, d_prefix(prefix, basicAllocator)
, d_objectName(objectName, basicAllocator)
, d_parent_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Metrics::Metrics(const bslstl::StringRef&              prefix,
                 const bslstl::StringRef&              objectName,
                 const bsl::shared_ptr<ntcs::Metrics>& parent,
                 bslma::Allocator*                     basicAllocator)
: d_mutex()
, d_numBytesSendable()
, d_numBytesSent()
, d_numBytesReceivable()
, d_numBytesReceived()
, d_numAcceptIterations()
, d_numSendIterations()
, d_numReceiveIterations()
, d_acceptQueueSize()
, d_acceptQueueDelay()
, d_writeQueueSize()
, d_writeQueueDelay()
, d_readQueueSize()
, d_readQueueDelay()
, d_numConnectionsAccepted()
, d_numConnectionsUnacceptable()
, d_numConnectionsSynchronized()
, d_numConnectionsUnsynchronizable()
, d_numBytesAllocated()
, d_txDelayBeforeScheduling()
, d_txDelayInSoftware()
, d_txDelay()
, d_txDelayBeforeAcknowledgement()
, d_rxDelayInHardware()
, d_rxDelay()
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

Metrics::~Metrics()
{
}

void Metrics::logConnectCompletion()
{
    d_numConnectionsSynchronized.update(1);

    if (d_parent_sp) {
        d_parent_sp->logConnectCompletion();
    }
}

void Metrics::logConnectFailure()
{
    d_numConnectionsUnsynchronizable.update(1);

    if (d_parent_sp) {
        d_parent_sp->logConnectFailure();
    }
}

void Metrics::logAcceptCompletion()
{
    d_numConnectionsAccepted.update(1);

    if (d_parent_sp) {
        d_parent_sp->logAcceptCompletion();
    }
}

void Metrics::logAcceptFailure()
{
    d_numConnectionsUnacceptable.update(1);

    if (d_parent_sp) {
        d_parent_sp->logAcceptFailure();
    }
}

void Metrics::logAcceptIterations(bsl::size_t numIterations)
{
    if (numIterations > 0) {
        d_numReceiveIterations.update(static_cast<double>(numIterations));
    }

    if (d_parent_sp) {
        d_parent_sp->logAcceptIterations(numIterations);
    }
}

void Metrics::logSendCompletion(bsl::size_t numBytesSendable,
                                bsl::size_t numBytesSent)
{
    d_numBytesSendable.update(static_cast<double>(numBytesSendable));
    d_numBytesSent.update(static_cast<double>(numBytesSent));

    if (d_parent_sp) {
        d_parent_sp->logSendCompletion(numBytesSendable, numBytesSent);
    }
}

void Metrics::logSendIterations(bsl::size_t numIterations)
{
    if (numIterations > 0) {
        d_numSendIterations.update(static_cast<double>(numIterations));
    }

    if (d_parent_sp) {
        d_parent_sp->logSendIterations(numIterations);
    }
}

void Metrics::logReceiveCompletion(bsl::size_t numBytesReceivable,
                                   bsl::size_t numBytesReceived)
{
    d_numBytesReceivable.update(static_cast<double>(numBytesReceivable));
    d_numBytesReceived.update(static_cast<double>(numBytesReceived));

    if (d_parent_sp) {
        d_parent_sp->logReceiveCompletion(numBytesReceivable,
                                          numBytesReceived);
    }
}

void Metrics::logReceiveIterations(bsl::size_t numIterations)
{
    if (numIterations > 0) {
        d_numReceiveIterations.update(static_cast<double>(numIterations));
    }

    if (d_parent_sp) {
        d_parent_sp->logReceiveIterations(numIterations);
    }
}

void Metrics::logAcceptQueueSize(bsl::size_t acceptQueueSize)
{
    d_acceptQueueSize.update(static_cast<double>(acceptQueueSize));

    if (d_parent_sp) {
        d_parent_sp->logAcceptQueueSize(acceptQueueSize);
    }
}

void Metrics::logAcceptQueueDelay(const bsls::TimeInterval& acceptQueueDelay)
{
    d_acceptQueueDelay.update(acceptQueueDelay.totalSecondsAsDouble());

    if (d_parent_sp) {
        d_parent_sp->logAcceptQueueDelay(acceptQueueDelay);
    }
}

void Metrics::logWriteQueueSize(bsl::size_t writeQueueSize)
{
    d_writeQueueSize.update(static_cast<double>(writeQueueSize));

    if (d_parent_sp) {
        d_parent_sp->logWriteQueueSize(writeQueueSize);
    }
}

void Metrics::logWriteQueueDelay(const bsls::TimeInterval& writeQueueDelay)
{
    d_writeQueueDelay.update(writeQueueDelay.totalSecondsAsDouble());

    if (d_parent_sp) {
        d_parent_sp->logWriteQueueDelay(writeQueueDelay);
    }
}

void Metrics::logReadQueueSize(bsl::size_t readQueueSize)
{
    d_readQueueSize.update(static_cast<double>(readQueueSize));

    if (d_parent_sp) {
        d_parent_sp->logReadQueueSize(readQueueSize);
    }
}

void Metrics::logReadQueueDelay(const bsls::TimeInterval& readQueueDelay)
{
    d_readQueueDelay.update(readQueueDelay.totalSecondsAsDouble());

    if (d_parent_sp) {
        d_parent_sp->logReadQueueDelay(readQueueDelay);
    }
}

void Metrics::logBlobBufferAllocation(bsl::size_t blobBufferCapacity)
{
    d_numBytesAllocated.update(static_cast<double>(blobBufferCapacity));

    if (d_parent_sp) {
        d_parent_sp->logBlobBufferAllocation(blobBufferCapacity);
    }
}

void Metrics::logTxDelayBeforeScheduling(
    const bsls::TimeInterval& txDelayBeforeScheduling)
{
    d_txDelayBeforeScheduling.update(
        static_cast<double>(txDelayBeforeScheduling.totalMicroseconds()));

    if (d_parent_sp) {
        d_parent_sp->logTxDelayBeforeScheduling(txDelayBeforeScheduling);
    }
}

void Metrics::logTxDelayInSoftware(const bsls::TimeInterval& txDelayInSoftware)
{
    d_txDelayInSoftware.update(
        static_cast<double>(txDelayInSoftware.totalMicroseconds()));

    if (d_parent_sp) {
        d_parent_sp->logTxDelayInSoftware(txDelayInSoftware);
    }
}

void Metrics::logTxDelay(const bsls::TimeInterval& txDelay)
{
    d_txDelay.update(static_cast<double>(txDelay.totalMicroseconds()));

    if (d_parent_sp) {
        d_parent_sp->logTxDelay(txDelay);
    }
}

void Metrics::logTxDelayBeforeAcknowledgement(
    const bsls::TimeInterval& txDelayBeforeAcknowledgement)
{
    d_txDelayBeforeAcknowledgement.update(
        static_cast<double>(txDelayBeforeAcknowledgement.totalMicroseconds()));

    if (d_parent_sp) {
        d_parent_sp->logTxDelayBeforeAcknowledgement(
            txDelayBeforeAcknowledgement);
    }
}

void Metrics::logRxDelayInHardware(const bsls::TimeInterval& rxDelayInHardware)
{
    d_rxDelayInHardware.update(
        static_cast<double>(rxDelayInHardware.totalMicroseconds()));

    if (d_parent_sp) {
        d_parent_sp->logRxDelay(rxDelayInHardware);
    }
}

void Metrics::logRxDelay(const bsls::TimeInterval& rxDelay)
{
    d_rxDelay.update(static_cast<double>(rxDelay.totalMicroseconds()));

    if (d_parent_sp) {
        d_parent_sp->logRxDelay(rxDelay);
    }
}

void Metrics::getStats(bdld::ManagedDatum* result)
{
    LockGuard guard(&d_mutex);

    bdld::DatumMutableArrayRef array;
    bdld::Datum::createUninitializedArray(&array,
                                          numOrdinals(),
                                          result->allocator());

    bsl::size_t index = 0;

    d_numBytesSendable.collectSummary(&array, &index);
    d_numBytesSent.collectSummary(&array, &index);

    d_numBytesReceivable.collectSummary(&array, &index);
    d_numBytesReceived.collectSummary(&array, &index);

    d_numAcceptIterations.collectSummary(&array, &index);
    d_numSendIterations.collectSummary(&array, &index);
    d_numReceiveIterations.collectSummary(&array, &index);

    d_acceptQueueSize.collectSummary(&array, &index);
    d_acceptQueueDelay.collectSummary(&array, &index);

    d_writeQueueSize.collectSummary(&array, &index);
    d_writeQueueDelay.collectSummary(&array, &index);

    d_readQueueSize.collectSummary(&array, &index);
    d_readQueueDelay.collectSummary(&array, &index);

    d_numConnectionsAccepted.collectSummary(&array, &index);

    d_numConnectionsUnacceptable.collectSummary(&array, &index);

    d_numConnectionsSynchronized.collectSummary(&array, &index);

    d_numConnectionsUnsynchronizable.collectSummary(&array, &index);

    d_numBytesAllocated.collectSummary(&array, &index);

    d_txDelayBeforeScheduling.collectSummary(&array, &index);
    d_txDelayInSoftware.collectSummary(&array, &index);
    d_txDelay.collectSummary(&array, &index);
    d_txDelayBeforeAcknowledgement.collectSummary(&array, &index);
    d_rxDelayInHardware.collectSummary(&array, &index);
    d_rxDelay.collectSummary(&array, &index);

    // TODO: Calculate and publish derivative metrics.
    // double avgBytesSentPerEvent = 0;
    // double avgBytesReceivedPerEvent = 0;

    *array.length() = numOrdinals();

    result->adopt(bdld::Datum::adoptArray(array));
}

const char* Metrics::getFieldPrefix(int ordinal) const
{
    NTCCFG_WARNING_UNUSED(ordinal);

    return d_prefix.c_str();
}

const char* Metrics::getFieldName(int ordinal) const
{
    if (ordinal < numOrdinals()) {
        return Metrics::STATISTICS[ordinal].d_name;
    }
    else {
        return 0;
    }
}

const char* Metrics::getFieldDescription(int ordinal) const
{
    NTCCFG_WARNING_UNUSED(ordinal);

    return "";
}

ntci::Monitorable::StatisticType Metrics::getFieldType(int ordinal) const
{
    if (ordinal < numOrdinals()) {
        return Metrics::STATISTICS[ordinal].d_type;
    }
    else {
        return ntci::Monitorable::e_AVERAGE;
    }
}

int Metrics::getFieldTags(int ordinal) const
{
    NTCCFG_WARNING_UNUSED(ordinal);

    return ntci::Monitorable::e_ANONYMOUS;
}

int Metrics::getFieldOrdinal(const char* fieldName) const
{
    int result = 0;

    for (int ordinal = 0; ordinal < numOrdinals(); ++ordinal) {
        if (bsl::strcmp(Metrics::STATISTICS[ordinal].d_name, fieldName) == 0) {
            result = ordinal;
        }
    }

    return result;
}

int Metrics::numOrdinals() const
{
    return sizeof Metrics::STATISTICS / sizeof Metrics::STATISTICS[0];
}

const char* Metrics::objectName() const
{
    return d_objectName.c_str();
}

const bsl::shared_ptr<ntcs::Metrics>& Metrics::parent() const
{
    return d_parent_sp;
}

ntcs::Metrics* Metrics::setThreadLocal(ntcs::Metrics* metrics)
{
    ntcs::Metrics* previous = reinterpret_cast<ntcs::Metrics*>(
        bslmt::ThreadUtil::getSpecific(s_key));

    int rc = bslmt::ThreadUtil::setSpecific(
        s_key,
        const_cast<const void*>(static_cast<void*>(metrics)));
    BSLS_ASSERT_OPT(rc == 0);

    return previous;
}

ntcs::Metrics* Metrics::getThreadLocal()
{
    ntcs::Metrics* current = reinterpret_cast<ntcs::Metrics*>(
        bslmt::ThreadUtil::getSpecific(s_key));

    return current;
}

}  // close package namespace
}  // close enterprise namespace
