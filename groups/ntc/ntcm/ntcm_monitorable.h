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

#ifndef INCLUDED_NTCM_MONITORABLE
#define INCLUDED_NTCM_MONITORABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_monitorablecollectorconfig.h>
#include <ntca_monitorableregistryconfig.h>
#include <ntccfg_platform.h>
#include <ntci_identifiable.h>
#include <ntci_monitorable.h>
#include <ntcscm_version.h>
#include <bdld_manageddatum.h>
#include <bdlma_concurrentmultipoolallocator.h>
#include <bdlmt_eventscheduler.h>
#include <bslmt_mutex.h>
#include <bsls_atomic.h>
#include <bsls_keyword.h>
#include <bsls_spinlock.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
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
class MonitorableLogRecord
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
    explicit MonitorableLogRecord(bslma::Allocator* basicAllocator = 0);

    /// Create a new log publisher record having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    MonitorableLogRecord(const MonitorableLogRecord& original,
                         bslma::Allocator*           basicAllocator = 0);

    /// Destroy this object.
    ~MonitorableLogRecord();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    MonitorableLogRecord& operator=(const MonitorableLogRecord& other);

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
/// @related ntcm::MonitorableLogRecord
bool operator==(const MonitorableLogRecord& lhs,
                const MonitorableLogRecord& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntcm::MonitorableLogRecord
bool operator!=(const MonitorableLogRecord& lhs,
                const MonitorableLogRecord& rhs);

/// @internal @brief
/// Provide a metrics publisher to the application log.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcm
class MonitorableLog : public ntci::MonitorablePublisher
{
    /// Define a type alias for a vector of published
    /// statistics.
    typedef bsl::vector<MonitorableLogRecord> RecordVector;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    Mutex             d_mutex;
    RecordVector      d_records;
    bsls::AtomicInt   d_severityLevel;
    bsls::AtomicInt   d_numPublications;
    bslma::Allocator* d_allocator_p;

  private:
    MonitorableLog(const MonitorableLog&) BSLS_KEYWORD_DELETED;
    MonitorableLog& operator=(const MonitorableLog&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new log publisher to the BSLS log at the DEBUG severity
    /// level. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    explicit MonitorableLog(bslma::Allocator* basicAllocator = 0);

    /// Create a new log publisher to the BSLS log at the specified
    /// 'severityLevel'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit MonitorableLog(bsls::LogSeverity::Enum severityLevel,
                            bslma::Allocator*       basicAllocator = 0);

    /// Destroy this object.
    virtual ~MonitorableLog();

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

    /// Return the number of publications.
    bsl::size_t numPublications() const;
};

/// @internal @brief
/// Provide a collector of statistics from monitorable objects.
///
/// @details
/// Provide a mechanism that provides a collector of statistics
/// from monitorable objects. Each set of collected statistics are published,
/// along with the monitorable object that measured them, through various
/// registered publishers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcm
class Collector : public ntci::MonitorableCollector
{
  public:
    /// This typedef defines a function to load the set of currently active
    /// monitorable objects.
    typedef bsl::function<void(
        bsl::vector<bsl::shared_ptr<ntci::Monitorable> >* result)>
        LoadCallback;

  private:
    /// The constants used by this implementation.
    enum Constants {
        /// The number of pools with which to configure a multipool allocator
        /// such that the smallest pool allocates 8 bytes and the largest pool
        /// allocates 1K.
        k_POOLS_UP_TO_1K = 8
    };

    /// Define a type alias for a set of registered
    /// publishers.
    typedef bsl::unordered_set<bsl::shared_ptr<ntci::MonitorablePublisher> >
        PublisherSet;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    Mutex                               d_mutex;
    PublisherSet                        d_publishers;
    bdlma::ConcurrentMultipoolAllocator d_memoryPools;
    LoadCallback                        d_loader;
    ntca::MonitorableCollectorConfig    d_config;
    bslma::Allocator*                   d_allocator_p;

  private:
    Collector(const Collector&) BSLS_KEYWORD_DELETED;
    Collector& operator=(const Collector&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new collector having a default configuration that collects
    /// statistics on-demand from all monitorable objects loaded from the
    /// specified 'loadCallback'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Collector(const LoadCallback& loadCallback,
                       bslma::Allocator*   basicAllocator = 0);

    /// Create a new collector having the specified 'configuration' that
    /// collects statistics on-demand from all monitorable objects loaded
    /// from the specified 'loadCallback'.  Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Collector(const ntca::MonitorableCollectorConfig& configuration,
                       const LoadCallback&                     loadCallback,
                       bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Collector() BSLS_KEYWORD_OVERRIDE;

    /// Register the specified 'publisher' to publish statistics measured
    /// by each monitorable object.
    void registerPublisher(const bsl::shared_ptr<ntci::MonitorablePublisher>&
                               publisher) BSLS_KEYWORD_OVERRIDE;

    /// Deregister the specified 'publisher' to no longer publish statistics
    /// measured by each monitorable object.
    void deregisterPublisher(const bsl::shared_ptr<ntci::MonitorablePublisher>&
                                 publisher) BSLS_KEYWORD_OVERRIDE;

    /// Start the background thread periodically collecting statistics.
    void start() BSLS_KEYWORD_OVERRIDE;

    /// Stop the background thread periodically collecting statistics.
    void stop() BSLS_KEYWORD_OVERRIDE;

    /// Collect statistics from each monitorable object registered with
    /// the default monitorable object registry and publish their statistics
    /// through each registered publisher.
    void collect() BSLS_KEYWORD_OVERRIDE;

    /// Return the configuration of this object.
    const ntca::MonitorableCollectorConfig& configuration() const;
};

/// @internal @brief
/// Provide a periodic collector of statistics in a separate thread.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcm
class PeriodicCollector : public ntci::MonitorableCollector
{
    enum Constants { k_DEFAULT_INTERVAL = 30 };

    bdlmt::EventScheduler                       d_scheduler;
    bdlmt::EventScheduler::RecurringEventHandle d_event;
    bsls::TimeInterval                          d_interval;
    ntcm::Collector                             d_collector;

  private:
    PeriodicCollector(const PeriodicCollector&) BSLS_KEYWORD_DELETED;
    PeriodicCollector& operator=(const PeriodicCollector&)
        BSLS_KEYWORD_DELETED;

  public:
    /// This typedef defines a function to load the set of currently active
    /// monitorable objects.
    typedef ntcm::Collector::LoadCallback LoadCallback;

    /// Create a new collector that periodically collects statistcs from all
    /// monitorable objects loaded from the specified 'loadCallback' at the
    /// specified 'interval'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit PeriodicCollector(const bsls::TimeInterval& interval,
                               const LoadCallback&       loadCallback,
                               bslma::Allocator*         basicAllocator = 0);

    /// Create a new collector having the specified 'configuration' that
    /// periodically collects statistics from all monitorable objects loaded
    /// from the specified 'loadCallback'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit PeriodicCollector(
        const ntca::MonitorableCollectorConfig& configuration,
        const LoadCallback&                     loadCallback,
        bslma::Allocator*                       basicAllocator = 0);

    /// Destroy this object.
    ~PeriodicCollector() BSLS_KEYWORD_OVERRIDE;

    /// Register the specified 'publisher' to publish statistics measured
    /// by each monitorable object.
    void registerPublisher(const bsl::shared_ptr<ntci::MonitorablePublisher>&
                               publisher) BSLS_KEYWORD_OVERRIDE;

    /// Deregister the specified 'publisher' to no longer publish statistics
    /// measured by each monitorable object.
    void deregisterPublisher(const bsl::shared_ptr<ntci::MonitorablePublisher>&
                                 publisher) BSLS_KEYWORD_OVERRIDE;

    /// Start the background thread periodically collecting statistics.
    void start() BSLS_KEYWORD_OVERRIDE;

    /// Stop the background thread periodically collecting statistics.
    void stop() BSLS_KEYWORD_OVERRIDE;

    /// Force the collection of statistics from each monitorable object
    /// registered with the default monitorable object registry and publish
    /// their statistics through each registered publisher.
    void collect() BSLS_KEYWORD_OVERRIDE;
};

/// @internal @brief
/// Provide a concrete registry of monitorable objects.
///
/// @details
/// Provide a mechanism that implements the
/// 'ntci::MonitorableRegistry' protocol to register monitorable objects by
/// their locally-unique object IDs.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcm
class MonitorableRegistry : public ntci::MonitorableRegistry
{
    /// Define a type alias for a map of locally-unique object
    /// identifiers to the shared pointers to registered monitorable objects
    /// so identified.
    typedef bsl::unordered_map<int, bsl::shared_ptr<ntci::Monitorable> >
        ObjectMap;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex                   d_mutex;
    ObjectMap                       d_objects;
    ntca::MonitorableRegistryConfig d_config;
    bslma::Allocator*               d_allocator_p;

  private:
    MonitorableRegistry(const MonitorableRegistry&) BSLS_KEYWORD_DELETED;
    MonitorableRegistry& operator=(const MonitorableRegistry&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new monitorable registry having a default configuration.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit MonitorableRegistry(bslma::Allocator* basicAllocator = 0);

    /// Create a new monitorable registry having the specified
    /// 'configuration'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit MonitorableRegistry(
        const ntca::MonitorableRegistryConfig& configuration,
        bslma::Allocator*                      basicAllocator = 0);

    /// Destroy this object.
    ~MonitorableRegistry() BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'object' to this monitorable object registry.
    void registerMonitorable(const bsl::shared_ptr<ntci::Monitorable>& object)
        BSLS_KEYWORD_OVERRIDE;

    /// Remove the specified 'object' from this monitorable object
    /// registry.
    void deregisterMonitorable(const bsl::shared_ptr<ntci::Monitorable>&
                                   object) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' each currently registered
    /// monitorable object.
    void loadRegisteredObjects(
        bsl::vector<bsl::shared_ptr<ntci::Monitorable> >* result) const
        BSLS_KEYWORD_OVERRIDE;
};

/// @internal @brief
/// Provide utilities to manage a default monitorable registry.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcm
struct MonitorableUtil {
    /// This typedef defines a function to load the set of currently active
    /// monitorable objects.
    typedef bsl::function<void(
        bsl::vector<bsl::shared_ptr<ntci::Monitorable> >* result)>
        LoadCallback;

    /// Initialize this component.
    static void initialize();

    /// Set the default monitorable object registry to an object with the
    /// specified 'configuration' and enable the registration of monitorable
    /// objects with that default registry. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator is 0, the
    /// global allocator is used.
    static void enableMonitorableRegistry(
        const ntca::MonitorableRegistryConfig& configuration,
        bslma::Allocator*                      basicAllocator = 0);

    /// Set a default monitorable object registry to the specified
    /// 'monitorableRegistry' and enable the registration of monitorable
    /// object with that default registry. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator is 0, the
    /// global allocator is used.
    static void enableMonitorableRegistry(
        const bsl::shared_ptr<ntci::MonitorableRegistry>& monitorableRegistry);

    /// Disable the registration of monitorable objects with the default
    /// registry and unset the default monitorable object registry, if any.
    static void disableMonitorableRegistry();

    /// Set the default monitorable object collector to an object with the
    /// specified 'configuration' and enable the periodic collection of
    /// monitorable objects by that default collector. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator is 0, the
    /// global allocator is used.
    static void enableMonitorableCollector(
        const ntca::MonitorableCollectorConfig& configuration,
        bslma::Allocator*                       basicAllocator = 0);

    /// Set the default monitorable object collector to the specified
    /// 'monitorableCollector' and enable the periodic collection of
    /// monitorable objects by that default collector. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator is 0, the
    /// global allocator is used.
    static void enableMonitorableCollector(
        const bsl::shared_ptr<ntci::MonitorableCollector>&
            monitorableCollector);

    /// Disable the periodic collection of monitorable objects by the
    /// default monitorable object collector and unset the default
    /// monitorable object collector, if any.
    static void disableMonitorableCollector();

    /// Add the specified 'monitorable' to the default monitorable object
    /// registry, if a default monitorable object registry has been enabled.
    static void registerMonitorable(
        const bsl::shared_ptr<ntci::Monitorable>& monitorable);

    /// Remove the specified 'monitorable' from the default monitorable
    /// object registry, if a default monitorable object registry has been
    /// enabled.
    static void deregisterMonitorable(
        const bsl::shared_ptr<ntci::Monitorable>& monitorable);

    /// Add the specified 'monitorable' of the entire process to the default
    /// monitorable object registry, if a default monitorable object
    /// registry has been enabled, and no other 'monitorable' for the entire
    /// process has already been registered.
    static void registerMonitorableProcess(
        const bsl::shared_ptr<ntci::Monitorable>& monitorable);

    /// Remove any monitorable of the entire process from the default
    /// monitorable object registry, if a default monitorable object
    /// registry has been enabled and any monitorable of the entire process
    /// had been previously registered.
    static void deregisterMonitorableProcess();

    /// Add the specified 'monitorablePublisher' to the default monitorable
    /// object collector, if a default monitorable object collector has been
    /// set.
    static void registerMonitorablePublisher(
        const bsl::shared_ptr<ntci::MonitorablePublisher>&
            monitorablePublisher);

    /// Remove the specified 'monitorablePublisher' from the default
    /// monitorable object collector, if a default monitorable object
    /// collector has been set.
    static void deregisterMonitorablePublisher(
        const bsl::shared_ptr<ntci::MonitorablePublisher>&
            monitorablePublisher);

    /// Add a monitorable publisher to the BSLS log at the specified
    /// 'severityLevel'.
    static void registerMonitorablePublisher(
        bsls::LogSeverity::Enum severityLevel);

    /// Remove a monitorable publisher to the BSLS log at the specified
    /// 'severityLevel'.
    static void deregisterMonitorablePublisher(
        bsls::LogSeverity::Enum severityLevel);

    /// Force the collection of statistics from each monitorable object
    /// registered with the default monitorable object registry and publish
    /// their statistics through each registered publisher.
    static void collectMetrics();

    /// Append to the specified 'result' each currently registered
    /// monitorable object with the default monitorable object registry, if
    /// a default monitorable object registry has been set.
    static void loadRegisteredObjects(
        bsl::vector<bsl::shared_ptr<ntci::Monitorable> >* result);

    /// Return the function to load the set of currently active monitorable
    /// objects.
    static LoadCallback loadCallback();

    /// Cleanup the resources used by this component.
    static void exit();
};

}  // close namespace ntcm
}  // close namespace BloombergLP
#endif
