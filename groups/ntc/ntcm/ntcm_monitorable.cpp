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

#include <ntcm_monitorable.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcm_monitorable_cpp, "$Id$ $CSID$")

#include <ntci_log.h>
#include <bdld_datum.h>
#include <bdld_manageddatum.h>
#include <bdlf_memfn.h>
#include <bdlma_bufferedsequentialallocator.h>
#include <bdlsb_memoutstreambuf.h>
#include <bdlt_currenttime.h>
#include <bdlt_epochutil.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsls_timeinterval.h>
#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_iomanip.h>
#include <bsl_sstream.h>
#include <bsl_unordered_map.h>
#include <bsl_vector.h>

#define NTCM_LOGPUBLISHER_FULL 0
#define NTCM_LOGPUBLISHER_SORTED 1

namespace BloombergLP {
namespace ntcm {

namespace {

#if NTCM_LOGPUBLISHER_FULL
const char* describeStatisticType(ntci::Monitorable::StatisticType type)
{
    switch (type) {
    case ntci::Monitorable::e_GAUGE:
        return "GAUGE";
        break;
    case ntci::Monitorable::e_SUM:
        return "SUM";
        break;
    case ntci::Monitorable::e_MINIMUM:
        return "MINIMUM";
        break;
    case ntci::Monitorable::e_MAXIMUM:
        return "MAXIMUM";
        break;
    case ntci::Monitorable::e_AVERAGE:
        return "AVERAGE";
        break;
    }

    return "UNKNOWN";
}
#endif

bsl::string formatMetricName(const bsl::string& objectName,
                             const bsl::string& prefix,
                             const bsl::string& fieldName)
{
    NTCCFG_WARNING_UNUSED(objectName);

    bsl::ostringstream ss;

    if (!prefix.empty()) {
        ss << prefix << '.';
    }

    ss << fieldName;

    return ss.str();
}

/// Provide a functor to sort two records.
struct RecordSorter {
    bool operator()(const MonitorableLogRecord& lhs,
                    const MonitorableLogRecord& rhs) const
    {
        if (lhs.prefix() < rhs.prefix()) {
            return true;
        }

        if (rhs.prefix() < lhs.prefix()) {
            return false;
        }

        if (lhs.objectName() < rhs.objectName()) {
            return true;
        }

        if (rhs.objectName() < lhs.objectName()) {
            return false;
        }

        return lhs.name() < rhs.name();
    }
};

bsl::string format(double value)
{
    bsl::stringstream ss;
    ss << bsl::setprecision(2);
    ss << bsl::fixed;

    if (value < 1024) {
        ss << value;
        return ss.str();
    }

    value /= 1024;

    if (value < 1024) {
        ss << value << "K";
        return ss.str();
    }

    value /= 1024;

    if (value < 1024) {
        ss << value << "M";
        return ss.str();
    }

    value /= 1024;

    if (value < 1024) {
        ss << value << "G";
        return ss.str();
    }

    ss << value << "T";
    return ss.str();
}

}  // close unnamed namespace

MonitorableLogRecord::MonitorableLogRecord(bslma::Allocator* basicAllocator)
: d_guid(basicAllocator)
, d_objectId(basicAllocator)
, d_objectName(basicAllocator)
, d_prefix(basicAllocator)
, d_name(basicAllocator)
, d_description(basicAllocator)
, d_value(0.0)
, d_type(ntci::Monitorable::e_AVERAGE)
{
}

MonitorableLogRecord::MonitorableLogRecord(
    const MonitorableLogRecord& original,
    bslma::Allocator*           basicAllocator)
: d_guid(original.d_guid, basicAllocator)
, d_objectId(original.d_objectId, basicAllocator)
, d_objectName(original.d_objectName, basicAllocator)
, d_prefix(original.d_prefix, basicAllocator)
, d_name(original.d_name, basicAllocator)
, d_description(original.d_description, basicAllocator)
, d_value(original.d_value)
, d_type(original.d_type)
{
}

MonitorableLogRecord::~MonitorableLogRecord()
{
}

MonitorableLogRecord& MonitorableLogRecord::operator=(
    const MonitorableLogRecord& other)
{
    if (this != &other) {
        d_guid        = other.d_guid;
        d_objectId    = other.d_objectId;
        d_objectName  = other.d_objectName;
        d_prefix      = other.d_prefix;
        d_name        = other.d_name;
        d_description = other.d_description;
        d_value       = other.d_value;
        d_type        = other.d_type;
    }

    return *this;
}

void MonitorableLogRecord::setGuid(const bsl::string& guid)
{
    d_guid = guid;
}

void MonitorableLogRecord::setObjectId(const bsl::string& objectId)
{
    d_objectId = objectId;
}

void MonitorableLogRecord::setObjectName(const bsl::string& objectName)
{
    d_objectName = objectName;
}

void MonitorableLogRecord::setPrefix(const bsl::string& prefix)
{
    d_prefix = prefix;
}

void MonitorableLogRecord::setName(const bsl::string& name)
{
    d_name = name;
}

void MonitorableLogRecord::setDescription(const bsl::string& description)
{
    d_description = description;
}

void MonitorableLogRecord::setValue(double value)
{
    d_value = value;
}

void MonitorableLogRecord::setType(ntci::Monitorable::StatisticType type)
{
    d_type = type;
}

const bsl::string& MonitorableLogRecord::guid() const
{
    return d_guid;
}

const bsl::string& MonitorableLogRecord::objectId() const
{
    return d_objectId;
}

const bsl::string& MonitorableLogRecord::objectName() const
{
    return d_objectName;
}

const bsl::string& MonitorableLogRecord::prefix() const
{
    return d_prefix;
}

const bsl::string& MonitorableLogRecord::name() const
{
    return d_name;
}

const bsl::string& MonitorableLogRecord::description() const
{
    return d_description;
}

double MonitorableLogRecord::value() const
{
    return d_value;
}

ntci::Monitorable::StatisticType MonitorableLogRecord::type() const
{
    return d_type;
}

bool operator==(const MonitorableLogRecord& lhs,
                const MonitorableLogRecord& rhs)
{
    return lhs.guid() == rhs.guid() && lhs.objectId() == rhs.objectId() &&
           lhs.objectName() == rhs.objectName() &&
           lhs.prefix() == rhs.prefix() && lhs.name() == rhs.name() &&
           lhs.description() == rhs.description() &&
           lhs.value() == rhs.value() && lhs.type() == rhs.type();
}

bool operator!=(const MonitorableLogRecord& lhs,
                const MonitorableLogRecord& rhs)
{
    return !operator==(lhs, rhs);
}

MonitorableLog::MonitorableLog(bslma::Allocator* basicAllocator)
: d_mutex()
, d_records(basicAllocator)
, d_severityLevel(static_cast<int>(bsls::LogSeverity::e_DEBUG))
, d_numPublications(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

MonitorableLog::MonitorableLog(bsls::LogSeverity::Enum severityLevel,
                               bslma::Allocator*       basicAllocator)
: d_mutex()
, d_records(basicAllocator)
, d_severityLevel(static_cast<int>(severityLevel))
, d_numPublications(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

MonitorableLog::~MonitorableLog()
{
}

void MonitorableLog::publish(
    const bsl::shared_ptr<ntci::Monitorable>& monitorable,
    const bdld::Datum&                        statistics,
    const bsls::TimeInterval&                 time,
    bool                                      final)
{
    NTCCFG_WARNING_UNUSED(time);

    ++d_numPublications;

    // Ensure the monitorable object reports its statistics as an array.

    if (!statistics.isArray()) {
        return;
    }

    bsls::LogSeverity::Enum severityLevel =
        static_cast<bsls::LogSeverity::Enum>(
            static_cast<int>(d_severityLevel));

    if (static_cast<int>(severityLevel) >
        static_cast<int>(bsls::Log::severityThreshold()))
    {
        return;
    }

    LockGuard guard(&d_mutex);

    // For each statistic retrieved from the monitorable object...

    for (int fieldOrdinal = 0;
         fieldOrdinal < static_cast<int>(statistics.theArray().length());
         ++fieldOrdinal)
    {
        // Determine the datapoint value for this statistic, skipping
        // nulls, which represent a statistic with no measured value during
        // this interval.

        double value = 0.0;

        if (statistics.theArray().data()[fieldOrdinal].isNull()) {
            continue;
        }
        else if (statistics.theArray().data()[fieldOrdinal].isDouble()) {
            value = statistics.theArray().data()[fieldOrdinal].theDouble();
        }
        else if (statistics.theArray().data()[fieldOrdinal].isInteger64()) {
            value = static_cast<double>(
                statistics.theArray().data()[fieldOrdinal].theInteger64());
        }
        else {
            continue;
        }

        // Resolve the statistic name.

        const char* fieldName = monitorable->getFieldName(fieldOrdinal);
        if (!fieldName) {
            continue;
        }

        // Resolve the statistic aggregation type.

        ntci::Monitorable::StatisticType statisticType =
            monitorable->getFieldType(fieldOrdinal);

        // Record the datapoint using the approximate GUTZ_METRICS macros
        // that best match the desired aggregation properties.

        bsl::string guid;
        {
            char guidText[ntsa::Guid::SIZE_TEXT];
            monitorable->guid().writeText(guidText);
            guid.assign(guidText, guidText + sizeof guidText);
        }

        bsl::string objectId;
        {
            bsl::ostringstream ss;
            ss << monitorable->objectId();
            objectId = ss.str();
        }

        MonitorableLogRecord record;
        record.setGuid(guid);
        record.setObjectId(objectId);
        record.setName(fieldName);
        record.setValue(value);
        record.setType(statisticType);

        const char* objectName = monitorable->objectName();
        if (objectName) {
            record.setObjectName(objectName);
        }

        const char* fieldPrefix = monitorable->getFieldPrefix(fieldOrdinal);
        if (fieldPrefix) {
            record.setPrefix(fieldPrefix);
        }

        const char* fieldDescription =
            monitorable->getFieldDescription(fieldOrdinal);
        if (fieldDescription) {
            record.setDescription(fieldDescription);
        }

        d_records.push_back(record);
    }

    if (final) {
#if NTCM_LOGPUBLISHER_SORTED
        bsl::sort(d_records.begin(), d_records.end(), RecordSorter());
#endif

        bsl::size_t maxObjectNameLength = 0;
        bsl::size_t maxPrefixLength     = 0;
        bsl::size_t maxNameLength       = 0;
        bsl::size_t maxMetricLength     = 0;
        for (RecordVector::const_iterator it = d_records.begin();
             it != d_records.end();
             ++it)
        {
            const MonitorableLogRecord& record = *it;

            if (record.objectName().size() > maxObjectNameLength) {
                maxObjectNameLength = record.objectName().size();
            }

            if (record.prefix().size() > maxPrefixLength) {
                maxPrefixLength = record.prefix().size();
            }

            if (record.name().size() > maxNameLength) {
                maxNameLength = record.name().size();
            }

            bsl::size_t metricLength = 0;
            // if (record.objectName().size() > 0) {
            //    metricLength += record.objectName().size() + 1;
            // }
            if (record.prefix().size() > 0) {
                metricLength += record.prefix().size() + 1;
            }
            metricLength += record.name().size();

            if (metricLength > maxMetricLength) {
                maxMetricLength = metricLength;
            }
        }

        if (maxObjectNameLength < 6) {
            maxObjectNameLength = 6;
        }

        if (maxPrefixLength < 6) {
            maxPrefixLength = 6;
        }

        if (maxNameLength < 4) {
            maxNameLength = 4;
        }

        if (maxMetricLength < 6) {
            maxMetricLength = 6;
        }

        if (d_records.size() > 0) {
            bdlsb::MemOutStreamBuf osb(d_allocator_p);
            {
                bsl::ostream ss(&osb);

                ss
#if NTCM_LOGPUBLISHER_FULL
                    << bsl::setw(32) << bsl::left << "GUID"
                    << "  " << bsl::setw(10) << bsl::left << "Instance"
                    << "  "
#endif
                    << bsl::setw(static_cast<int>(maxObjectNameLength))
                    << bsl::left << "Object"
                    << "  "
#if NTCM_LOGPUBLISHER_FULL
                    << bsl::setw(static_cast<int>(maxPrefixLength))
                    << bsl::left << "Prefix"
                    << "  " << bsl::setw(static_cast<int>(maxNameLength))
                    << bsl::left << "Name"
                    << "  "
#endif
                    << bsl::setw(static_cast<int>(maxMetricLength))
                    << bsl::left << "Metric"
                    << "  " << bsl::setw(10) << bsl::right << "Display"
#if NTCM_LOGPUBLISHER_FULL
                    << "  " << bsl::setw(20) << bsl::right << "Value"
                    << "  " << bsl::setw(10) << bsl::left << "Type"
                    << "  " << bsl::left << "Description"
#endif
                    << '\n';

                for (RecordVector::const_iterator it = d_records.begin();
                     it != d_records.end();
                     ++it)
                {
                    const MonitorableLogRecord& record = *it;

#if NTCM_LOGPUBLISHER_FULL
                    ss << bsl::setw(32) << bsl::left << record.guid() << "  ";

                    ss << bsl::setw(10) << bsl::left << record.objectId()
                       << "  ";
#endif

                    ss << bsl::setw(static_cast<int>(maxObjectNameLength))
                       << bsl::left;

                    if (record.objectName().empty()) {
                        ss << "--";
                    }
                    else {
                        ss << record.objectName();
                    }
                    ss << "  ";

#if NTCM_LOGPUBLISHER_FULL
                    ss << bsl::setw(static_cast<int>(maxPrefixLength))
                       << bsl::left;

                    if (record.prefix().empty()) {
                        ss << "--";
                    }
                    else {
                        ss << record.prefix();
                    }
                    ss << "  ";

                    ss << bsl::setw(static_cast<int>(maxNameLength))
                       << bsl::left << record.name() << "  ";
#endif

                    ss << bsl::setw(static_cast<int>(maxMetricLength))
                       << bsl::left
                       << formatMetricName(record.objectName(),
                                           record.prefix(),
                                           record.name())
                       << "  ";

                    {
                        bsl::string display = format(record.value());

                        ss << bsl::setw(10) << bsl::right << display << "  ";
                    }

#if NTCM_LOGPUBLISHER_FULL
                    ss << bsl::setw(20) << bsl::right << bsl::fixed
                       << bsl::setprecision(2) << record.value() << "  ";

                    ss << bsl::setw(10) << bsl::left
                       << describeStatisticType(record.type()) << "  "
                       << bsl::left << record.description();
#endif

                    ss << '\n';
                }

                ss << '\0';
            }

            osb.pubsync();

            d_records.clear();

            if (osb.length() > 0) {
                BSLS_ASSERT_OPT(osb.data()[osb.length() - 1] == 0);
                BSLS_LOG(severityLevel, "Metrics:\n%s", osb.data());
            }
        }
        else {
            // BSLS_LOG(severityLevel, "No metrics");
        }
    }
}

void MonitorableLog::setSeverityLevel(bsls::LogSeverity::Enum severityLevel)
{
    d_severityLevel = static_cast<int>(severityLevel);
}

bsl::size_t MonitorableLog::numPublications() const
{
    return d_numPublications;
}

Collector::Collector(const LoadCallback& loadCallback,
                     bslma::Allocator*   basicAllocator)
: d_mutex()
, d_publishers(basicAllocator)
, d_memoryPools(k_POOLS_UP_TO_1K, basicAllocator)
, d_loader(bsl::allocator_arg, basicAllocator, loadCallback)
, d_config(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Collector::Collector(const ntca::MonitorableCollectorConfig& configuration,
                     const LoadCallback&                     loadCallback,
                     bslma::Allocator*                       basicAllocator)
: d_mutex()
, d_publishers(basicAllocator)
, d_memoryPools(k_POOLS_UP_TO_1K, basicAllocator)
, d_loader(bsl::allocator_arg, basicAllocator, loadCallback)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Collector::~Collector()
{
}

void Collector::registerPublisher(
    const bsl::shared_ptr<ntci::MonitorablePublisher>& publisher)
{
    LockGuard guard(&d_mutex);
    d_publishers.insert(publisher);
}

void Collector::deregisterPublisher(
    const bsl::shared_ptr<ntci::MonitorablePublisher>& publisher)
{
    LockGuard guard(&d_mutex);
    d_publishers.erase(publisher);
}

void Collector::start()
{
}

void Collector::stop()
{
}

void Collector::collect()
{
    char                               arena[1024];
    bdlma::BufferedSequentialAllocator sequentialAllocator(arena,
                                                           sizeof arena,
                                                           &d_memoryPools);

    typedef bsl::vector<bsl::shared_ptr<ntci::MonitorablePublisher> >
                    PublisherVector;
    PublisherVector publishers(&sequentialAllocator);
    {
        LockGuard guard(&d_mutex);

        publishers.reserve(d_publishers.size());
        publishers.assign(d_publishers.begin(), d_publishers.end());
    }

    typedef bsl::vector<bsl::shared_ptr<ntci::Monitorable> > MonitorableVector;
    MonitorableVector monitorables(&sequentialAllocator);

    d_loader(&monitorables);

    bsls::TimeInterval now = bdlt::CurrentTime::now();

    for (MonitorableVector::const_iterator it = monitorables.begin();
         it != monitorables.end();
         ++it)
    {
        const bsl::shared_ptr<ntci::Monitorable>& monitorable = *it;

        const bool final = it == monitorables.end() - 1;

        bdld::ManagedDatum statistics(&sequentialAllocator);
        monitorable->getStats(&statistics);

        if (!statistics.datum().isArray()) {
            continue;
        }

        for (PublisherVector::const_iterator jt = publishers.begin();
             jt != publishers.end();
             ++jt)
        {
            const bsl::shared_ptr<ntci::MonitorablePublisher>& publisher = *jt;
            publisher->publish(monitorable, statistics.datum(), now, final);
        }
    }
}

const ntca::MonitorableCollectorConfig& Collector::configuration() const
{
    return d_config;
}

PeriodicCollector::PeriodicCollector(const bsls::TimeInterval& interval,
                                     const LoadCallback&       loadCallback,
                                     bslma::Allocator*         basicAllocator)
: d_scheduler(basicAllocator)
, d_event()
, d_interval(interval)
, d_collector(loadCallback, basicAllocator)
{
}

PeriodicCollector::PeriodicCollector(
    const ntca::MonitorableCollectorConfig& configuration,
    const LoadCallback&                     loadCallback,
    bslma::Allocator*                       basicAllocator)
: d_scheduler(basicAllocator)
, d_event()
, d_interval(0)
, d_collector(configuration, loadCallback, basicAllocator)
{
    if (!configuration.period().isNull()) {
        d_interval = bsls::TimeInterval(configuration.period().value(), 0);
    }
    else {
        d_interval = bsls::TimeInterval(k_DEFAULT_INTERVAL, 0);
    }
}

PeriodicCollector::~PeriodicCollector()
{
}

void PeriodicCollector::registerPublisher(
    const bsl::shared_ptr<ntci::MonitorablePublisher>& publisher)
{
    d_collector.registerPublisher(publisher);
}

void PeriodicCollector::deregisterPublisher(
    const bsl::shared_ptr<ntci::MonitorablePublisher>& publisher)
{
    d_collector.deregisterPublisher(publisher);
}

void PeriodicCollector::start()
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_TRACE("Starting metrics collector");

    d_collector.start();

    if (d_interval > bsls::TimeInterval(0, 0)) {
        d_scheduler.scheduleRecurringEvent(
            &d_event,
            d_interval,
            bdlf::MemFnUtil::memFn(&ntcm::Collector::collect, &d_collector));

        bslmt::ThreadAttributes threadAttributes;
        if (!d_collector.configuration().threadName().isNull()) {
            threadAttributes.setThreadName(
                d_collector.configuration().threadName().value());
        }

        int rc = d_scheduler.start(threadAttributes);
        if (rc != 0) {
            NTCI_LOG_ERROR(
                "Failed to start metrics collector scheduler, rc = %d",
                rc);
            return;
        }
    }
}

void PeriodicCollector::stop()
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_TRACE("Stopping metrics collector");

    if (d_interval > bsls::TimeInterval(0, 0)) {
        d_scheduler.cancelEventAndWait(&d_event);
        d_scheduler.stop();
    }

    d_collector.stop();
}

void PeriodicCollector::collect()
{
    d_collector.collect();
}

MonitorableRegistry::MonitorableRegistry(bslma::Allocator* basicAllocator)
: d_mutex()
, d_objects(basicAllocator)
, d_config(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

MonitorableRegistry::MonitorableRegistry(
    const ntca::MonitorableRegistryConfig& configuration,
    bslma::Allocator*                      basicAllocator)
: d_mutex()
, d_objects(basicAllocator)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

MonitorableRegistry::~MonitorableRegistry()
{
}

void MonitorableRegistry::registerMonitorable(
    const bsl::shared_ptr<ntci::Monitorable>& object)
{
    LockGuard guard(&d_mutex);

    if (!d_config.maxSize().isNull()) {
        if (d_objects.size() >= d_config.maxSize().value()) {
            return;
        }
    }

    d_objects[static_cast<int>(object->objectId())] = object;
}

void MonitorableRegistry::deregisterMonitorable(
    const bsl::shared_ptr<ntci::Monitorable>& object)
{
    LockGuard guard(&d_mutex);

    d_objects.erase(static_cast<int>(object->objectId()));
}

void MonitorableRegistry::loadRegisteredObjects(
    bsl::vector<bsl::shared_ptr<ntci::Monitorable> >* result) const
{
    LockGuard guard(&d_mutex);

    result->reserve(result->size() + d_objects.size());

    for (ObjectMap::const_iterator it = d_objects.begin();
         it != d_objects.end();
         ++it)
    {
        result->push_back(it->second);
    }
}

namespace {

bsls::SpinLock s_monitorableRegistryLock = BSLS_SPINLOCK_UNLOCKED;
bsl::shared_ptr<ntci::MonitorableRegistry> s_monitorableRegistry_sp;

bsls::SpinLock s_monitorableCollectorLock = BSLS_SPINLOCK_UNLOCKED;
bsl::shared_ptr<ntci::MonitorableCollector> s_monitorableCollector_sp;

bsls::SpinLock s_monitorableSystemLock = BSLS_SPINLOCK_UNLOCKED;
bsl::shared_ptr<ntci::Monitorable> s_monitorableSystem_sp;

bsls::SpinLock s_logPublisherLock = BSLS_SPINLOCK_UNLOCKED;
bsl::shared_ptr<ntcm::MonitorableLog> s_logPublisher_sp;

}  // close unnamed namespace

void MonitorableUtil::initialize()
{
}

void MonitorableUtil::enableMonitorableRegistry(
    const ntca::MonitorableRegistryConfig& configuration,
    bslma::Allocator*                      basicAllocator)
{
    bslma::Allocator* allocator =
        basicAllocator ? basicAllocator : bslma::Default::globalAllocator();

    bsl::shared_ptr<ntcm::MonitorableRegistry> monitorableRegistry;
    monitorableRegistry.createInplace(allocator, configuration, allocator);

    bsls::SpinLockGuard guard(&s_monitorableRegistryLock);
    s_monitorableRegistry_sp = monitorableRegistry;
}

void MonitorableUtil::enableMonitorableRegistry(
    const bsl::shared_ptr<ntci::MonitorableRegistry>& monitorableRegistry)
{
    bsls::SpinLockGuard guard(&s_monitorableRegistryLock);
    s_monitorableRegistry_sp = monitorableRegistry;
}

void MonitorableUtil::disableMonitorableRegistry()
{
    {
        bsls::SpinLockGuard guard(&s_monitorableSystemLock);
        s_monitorableSystem_sp.reset();
    }

    {
        bsls::SpinLockGuard guard(&s_monitorableRegistryLock);
        s_monitorableRegistry_sp.reset();
    }
}

void MonitorableUtil::enableMonitorableCollector(
    const ntca::MonitorableCollectorConfig& configuration,
    bslma::Allocator*                       basicAllocator)
{
    bslma::Allocator* allocator =
        basicAllocator ? basicAllocator : bslma::Default::globalAllocator();

    bsl::shared_ptr<ntci::MonitorableCollector> monitorableCollector;

    if (!configuration.period().isNull() && configuration.period().value() > 0)
    {
        bsl::shared_ptr<ntcm::PeriodicCollector> concreteMonitorableCollector;
        concreteMonitorableCollector.createInplace(
            allocator,
            configuration,
            ntcm::MonitorableUtil::loadCallback(),
            allocator);
        monitorableCollector = concreteMonitorableCollector;
    }
    else {
        bsl::shared_ptr<ntcm::Collector> concreteMonitorableCollector;
        concreteMonitorableCollector.createInplace(
            allocator,
            configuration,
            ntcm::MonitorableUtil::loadCallback(),
            allocator);
        monitorableCollector = concreteMonitorableCollector;
    }

    monitorableCollector->start();

    bsl::shared_ptr<ntci::MonitorableCollector> previousMonitorableCollector;
    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        previousMonitorableCollector = s_monitorableCollector_sp;
        s_monitorableCollector_sp    = monitorableCollector;
    }

    if (previousMonitorableCollector) {
        previousMonitorableCollector->stop();
    }
}

void MonitorableUtil::enableMonitorableCollector(
    const bsl::shared_ptr<ntci::MonitorableCollector>& monitorableCollector)
{
    bsl::shared_ptr<ntci::MonitorableCollector> previousMonitorableCollector;
    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        previousMonitorableCollector = s_monitorableCollector_sp;
        s_monitorableCollector_sp    = monitorableCollector;
    }

    if (previousMonitorableCollector &&
        previousMonitorableCollector != monitorableCollector)
    {
        previousMonitorableCollector->stop();
    }
}

void MonitorableUtil::disableMonitorableCollector()
{
    bsl::shared_ptr<ntci::MonitorableCollector> previousMonitorableCollector;
    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        previousMonitorableCollector = s_monitorableCollector_sp;
        s_monitorableCollector_sp.reset();
    }

    if (previousMonitorableCollector) {
        previousMonitorableCollector->stop();
    }
}

void MonitorableUtil::registerMonitorable(
    const bsl::shared_ptr<ntci::Monitorable>& monitorable)
{
    bsl::shared_ptr<ntci::MonitorableRegistry> monitorableRegistry;
    {
        bsls::SpinLockGuard guard(&s_monitorableRegistryLock);
        monitorableRegistry = s_monitorableRegistry_sp;
    }

    if (monitorableRegistry) {
        monitorableRegistry->registerMonitorable(monitorable);
    }
}

void MonitorableUtil::deregisterMonitorable(
    const bsl::shared_ptr<ntci::Monitorable>& monitorable)
{
    bsl::shared_ptr<ntci::MonitorableRegistry> monitorableRegistry;
    {
        bsls::SpinLockGuard guard(&s_monitorableRegistryLock);
        monitorableRegistry = s_monitorableRegistry_sp;
    }

    if (monitorableRegistry) {
        monitorableRegistry->deregisterMonitorable(monitorable);
    }
}

void MonitorableUtil::registerMonitorableProcess(
    const bsl::shared_ptr<ntci::Monitorable>& monitorable)
{
    {
        bsls::SpinLockGuard guard(&s_monitorableSystemLock);
        if (s_monitorableSystem_sp) {
            return;
        }

        s_monitorableSystem_sp = monitorable;
    }

    MonitorableUtil::registerMonitorable(monitorable);
}

void MonitorableUtil::deregisterMonitorableProcess()
{
    bsl::shared_ptr<ntci::Monitorable> monitorable;

    {
        bsls::SpinLockGuard guard(&s_monitorableSystemLock);
        if (!s_monitorableSystem_sp) {
            return;
        }

        s_monitorableSystem_sp.swap(monitorable);
    }

    MonitorableUtil::deregisterMonitorable(monitorable);
}

void MonitorableUtil::registerMonitorablePublisher(
    const bsl::shared_ptr<ntci::MonitorablePublisher>& monitorablePublisher)
{
    bsl::shared_ptr<ntci::MonitorableCollector> monitorableCollector;
    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        monitorableCollector = s_monitorableCollector_sp;
    }

    if (monitorableCollector) {
        monitorableCollector->registerPublisher(monitorablePublisher);
    }
}

void MonitorableUtil::deregisterMonitorablePublisher(
    const bsl::shared_ptr<ntci::MonitorablePublisher>& monitorablePublisher)
{
    bsl::shared_ptr<ntci::MonitorableCollector> monitorableCollector;
    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        monitorableCollector = s_monitorableCollector_sp;
    }

    if (monitorableCollector) {
        monitorableCollector->deregisterPublisher(monitorablePublisher);
    }
}

void MonitorableUtil::registerMonitorablePublisher(
    bsls::LogSeverity::Enum severityLevel)
{
    bsl::shared_ptr<ntci::MonitorableCollector> monitorableCollector;
    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        monitorableCollector = s_monitorableCollector_sp;
    }

    bsl::shared_ptr<ntcm::MonitorableLog> logPublisher;
    {
        bsls::SpinLockGuard guard(&s_logPublisherLock);

        if (s_logPublisher_sp) {
            s_logPublisher_sp->setSeverityLevel(severityLevel);
            return;
        }
        else {
            s_logPublisher_sp.createInplace(bslma::Default::globalAllocator(),
                                            severityLevel,
                                            bslma::Default::globalAllocator());
            logPublisher = s_logPublisher_sp;
        }
    }

    if (monitorableCollector) {
        monitorableCollector->registerPublisher(logPublisher);
    }
}

void MonitorableUtil::deregisterMonitorablePublisher(
    bsls::LogSeverity::Enum severityLevel)
{
    NTCCFG_WARNING_UNUSED(severityLevel);

    bsl::shared_ptr<ntci::MonitorableCollector> monitorableCollector;
    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        monitorableCollector = s_monitorableCollector_sp;
    }

    bsl::shared_ptr<ntcm::MonitorableLog> logPublisher;
    {
        bsls::SpinLockGuard guard(&s_logPublisherLock);
        logPublisher.swap(s_logPublisher_sp);
    }

    if (monitorableCollector) {
        monitorableCollector->deregisterPublisher(logPublisher);
    }
}

void MonitorableUtil::collectMetrics()
{
    bsl::shared_ptr<ntci::MonitorableCollector> monitorableCollector;
    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        monitorableCollector = s_monitorableCollector_sp;
    }

    if (monitorableCollector) {
        monitorableCollector->collect();
    }
}

void MonitorableUtil::loadRegisteredObjects(
    bsl::vector<bsl::shared_ptr<ntci::Monitorable> >* result)
{
    bsl::shared_ptr<ntci::MonitorableRegistry> monitorableRegistry;
    {
        bsls::SpinLockGuard guard(&s_monitorableRegistryLock);
        monitorableRegistry = s_monitorableRegistry_sp;
    }

    if (monitorableRegistry) {
        monitorableRegistry->loadRegisteredObjects(result);
    }
}

MonitorableUtil::LoadCallback MonitorableUtil::loadCallback()
{
    return &MonitorableUtil::loadRegisteredObjects;
}

void MonitorableUtil::exit()
{
    {
        bsls::SpinLockGuard guard(&s_logPublisherLock);
        s_logPublisher_sp.reset();
    }

    {
        bsls::SpinLockGuard guard(&s_monitorableSystemLock);
        s_monitorableSystem_sp.reset();
    }

    {
        bsls::SpinLockGuard guard(&s_monitorableCollectorLock);
        s_monitorableCollector_sp.reset();
    }

    {
        bsls::SpinLockGuard guard(&s_monitorableRegistryLock);
        s_monitorableRegistry_sp.reset();
    }
}

}  // close namespace ntcm
}  // close namespace BloombergLP
