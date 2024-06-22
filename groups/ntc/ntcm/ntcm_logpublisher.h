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

#ifndef INCLUDED_NTCM_LOGPUBLISHER
#define INCLUDED_NTCM_LOGPUBLISHER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntci_monitorable.h>
#include <ntcscm_version.h>
#include <bslmt_mutex.h>
#include <bsls_atomic.h>
#include <bsls_keyword.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcm {

/// @internal @brief
/// Describe a collected metric from a monitorable object that will be
/// published to the log.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcm
class LogPublisherRecord
{
    bsl::string                      d_guid;
    bsl::string                      d_objectId;
    bsl::string                      d_objectName;
    bsl::string                      d_prefix;
    bsl::string                      d_name;
    bsl::string                      d_description;
    double                           d_value;
    ntci::Monitorable::StatisticType d_type;

  public:
    /// Create a new log publisher record. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit LogPublisherRecord(bslma::Allocator* basicAllocator = 0);

    /// Create a new log publisher record having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    LogPublisherRecord(const LogPublisherRecord& original,
                       bslma::Allocator*         basicAllocator = 0);

    /// Destroy this object.
    ~LogPublisherRecord();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    LogPublisherRecord& operator=(const LogPublisherRecord& other);

    /// Set the GUID of the monitorable object that published the statistic
    /// to the specified 'guid'.
    void setGuid(const bsl::string& guid);

    /// Set the object ID of the monitorable object that published the
    /// statistic to the specified 'objectId'.
    void setObjectId(const bsl::string& objectId);

    /// Set the object name of the monitorable object that published the
    /// statistic to the specified 'objectName'.
    void setObjectName(const bsl::string& objectName);

    /// Set the prefix of the statistic to the specified 'prefix'.
    void setPrefix(const bsl::string& prefix);

    /// Set the name of the statistic to the specified 'name'.
    void setName(const bsl::string& name);

    /// Set the description of the statistic to the specified 'description'.
    void setDescription(const bsl::string& description);

    /// Set the value of the statistic to the specified 'value'.
    void setValue(double value);

    /// Set the value of the statistic to the specified 'type'.
    void setType(ntci::Monitorable::StatisticType type);

    /// Return the GUID of the monitorable object that published the
    /// statistic.
    const bsl::string& guid() const;

    /// Return the objectId of the monitorable object that published the
    /// statistic.
    const bsl::string& objectId() const;

    /// Return the object name of the monitorable object that published the
    /// statistic.
    const bsl::string& objectName() const;

    /// Return the prefix of the statistic.
    const bsl::string& prefix() const;

    /// Return the name of the statistic.
    const bsl::string& name() const;

    /// Return the description of the statistic.
    const bsl::string& description() const;

    /// Return the value of the statistic.
    double value() const;

    /// Return the type of the statistic.
    ntci::Monitorable::StatisticType type() const;
};

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntcm::LogPublisherRecord
bool operator==(const LogPublisherRecord& lhs, const LogPublisherRecord& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntcm::LogPublisherRecord
bool operator!=(const LogPublisherRecord& lhs, const LogPublisherRecord& rhs);

/// @internal @brief
/// Provide a metrics publisher to the application log.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcm
class LogPublisher : public ntci::MonitorablePublisher
{
    /// Define a type alias for a vector of published
    /// statistics.
    typedef bsl::vector<LogPublisherRecord> RecordVector;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    Mutex             d_mutex;
    RecordVector      d_records;
    bsls::AtomicInt   d_severityLevel;
    bslma::Allocator* d_allocator_p;

  private:
    LogPublisher(const LogPublisher&) BSLS_KEYWORD_DELETED;
    LogPublisher& operator=(const LogPublisher&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new log publisher to the BSLS log at the DEBUG severity
    /// level. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit LogPublisher(bslma::Allocator* basicAllocator = 0);

    /// Create a new log publisher to the BSLS log at the specified
    /// 'severityLevel'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit LogPublisher(bsls::LogSeverity::Enum severityLevel,
                          bslma::Allocator*       basicAllocator = 0);

    /// Destroy this object.
    virtual ~LogPublisher();

    /// Publish the specified 'statistics' collected from the specified
    /// 'monitorable' object at the specified 'time'. If the specified
    /// 'final' flag is true, these 'statistics' are the final statistics
    /// collected during the same sample at the 'time'.
    virtual void publish(const bsl::shared_ptr<ntci::Monitorable>& monitorable,
                         const bdld::Datum&                        statistics,
                         const bsls::TimeInterval&                 time,
                         bool                                      final);

    /// Set the log severity level to the specified 'severityLevel'.
    void setSeverityLevel(bsls::LogSeverity::Enum severityLevel);
};

}  // close package namespace
}  // close enterprise namespace
#endif
