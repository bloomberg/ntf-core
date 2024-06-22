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

#include <ntcm_logpublisher.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcm_logpublisher_cpp, "$Id$ $CSID$")

#include <ntci_log.h>

#include <bdld_datum.h>
#include <bdlsb_memoutstreambuf.h>
#include <bdlt_epochutil.h>

#include <bslmt_lockguard.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_log.h>

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_iomanip.h>
#include <bsl_sstream.h>

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
    bool operator()(const LogPublisherRecord& lhs,
                    const LogPublisherRecord& rhs) const
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

LogPublisherRecord::LogPublisherRecord(bslma::Allocator* basicAllocator)
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

LogPublisherRecord::LogPublisherRecord(const LogPublisherRecord& original,
                                       bslma::Allocator* basicAllocator)
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

LogPublisherRecord::~LogPublisherRecord()
{
}

LogPublisherRecord& LogPublisherRecord::operator=(
    const LogPublisherRecord& other)
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

void LogPublisherRecord::setGuid(const bsl::string& guid)
{
    d_guid = guid;
}

void LogPublisherRecord::setObjectId(const bsl::string& objectId)
{
    d_objectId = objectId;
}

void LogPublisherRecord::setObjectName(const bsl::string& objectName)
{
    d_objectName = objectName;
}

void LogPublisherRecord::setPrefix(const bsl::string& prefix)
{
    d_prefix = prefix;
}

void LogPublisherRecord::setName(const bsl::string& name)
{
    d_name = name;
}

void LogPublisherRecord::setDescription(const bsl::string& description)
{
    d_description = description;
}

void LogPublisherRecord::setValue(double value)
{
    d_value = value;
}

void LogPublisherRecord::setType(ntci::Monitorable::StatisticType type)
{
    d_type = type;
}

const bsl::string& LogPublisherRecord::guid() const
{
    return d_guid;
}

const bsl::string& LogPublisherRecord::objectId() const
{
    return d_objectId;
}

const bsl::string& LogPublisherRecord::objectName() const
{
    return d_objectName;
}

const bsl::string& LogPublisherRecord::prefix() const
{
    return d_prefix;
}

const bsl::string& LogPublisherRecord::name() const
{
    return d_name;
}

const bsl::string& LogPublisherRecord::description() const
{
    return d_description;
}

double LogPublisherRecord::value() const
{
    return d_value;
}

ntci::Monitorable::StatisticType LogPublisherRecord::type() const
{
    return d_type;
}

bool operator==(const LogPublisherRecord& lhs, const LogPublisherRecord& rhs)
{
    return lhs.guid() == rhs.guid() && lhs.objectId() == rhs.objectId() &&
           lhs.objectName() == rhs.objectName() &&
           lhs.prefix() == rhs.prefix() && lhs.name() == rhs.name() &&
           lhs.description() == rhs.description() &&
           lhs.value() == rhs.value() && lhs.type() == rhs.type();
}

bool operator!=(const LogPublisherRecord& lhs, const LogPublisherRecord& rhs)
{
    return !operator==(lhs, rhs);
}

LogPublisher::LogPublisher(bslma::Allocator* basicAllocator)
: d_mutex()
, d_records(basicAllocator)
, d_severityLevel(static_cast<int>(bsls::LogSeverity::e_DEBUG))
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

LogPublisher::LogPublisher(bsls::LogSeverity::Enum severityLevel,
                           bslma::Allocator*       basicAllocator)
: d_mutex()
, d_records(basicAllocator)
, d_severityLevel(static_cast<int>(severityLevel))
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

LogPublisher::~LogPublisher()
{
}

void LogPublisher::publish(
    const bsl::shared_ptr<ntci::Monitorable>& monitorable,
    const bdld::Datum&                        statistics,
    const bsls::TimeInterval&                 time,
    bool                                      final)
{
    NTCCFG_WARNING_UNUSED(time);

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

        LogPublisherRecord record;
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
            const LogPublisherRecord& record = *it;

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
                    const LogPublisherRecord& record = *it;

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

void LogPublisher::setSeverityLevel(bsls::LogSeverity::Enum severityLevel)
{
    d_severityLevel = static_cast<int>(severityLevel);
}

}  // close package namespace
}  // close enterprise namespace
