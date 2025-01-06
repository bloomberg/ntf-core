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

#ifndef INCLUDED_NTCI_MONITORABLE
#define INCLUDED_NTCI_MONITORABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_identifiable.h>
#include <ntcscm_version.h>
#include <bdld_manageddatum.h>
#include <bsls_spinlock.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface for an object that evaluates the quality and nature of
/// its execution.
///
/// @details
/// The interface for the querying of statistics is independent of the
/// implementation details of how those statistics are measured and stored.
/// Statistics are represented in terms of a 'bdld::Datum' that must in term
/// represent an array of values (themselves potentially datums as well.) The
/// only requirement is that the length of the datum array that is the result
/// of querying statistics is stable over the lifetime of the object.
/// Monitorable objects are responsible for resolving the name of a statistic
/// from the datum array index ordinal.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @par Usage Example
/// Let's assume the existence of a class 'example::Object', that implements
/// the 'ntci::Monitorable' interface, and an 'object' instantiated of that class
/// type. Let's query its statistics and print the field names and values to
/// standard output in a table.
///
///     bslma::Allocator* allocator = bslma::Default::getDefaultAllocator();
///
///     bdld::Datum datum;
///     object->getStats(&datum,
///                      ntci::Monitorable::e_INTERVAL_NO_RESET,
///                      ntci::Monitorable::e_INTERNAL,
///                      allocator);
///
///     ASSERT(datum.isArray());
///
///     for (int ordinal = 0;
///              ordinal < datum.theArray().length();
///            ++ordinal)
///     {
///         ASSERT(datum.theArray().data()[ordinal].isInteger64());
///
///         bsl::cout << bsl::setw(10)
///                   << bsl::left
///                   << object->getFieldName(ordinal)
///                   << bsl::setw(30)
///                   << bsl::right
///                   << datum.theArray().data()[ordinal]
///                   << bsl::endl;
///     }
///
///     bdld::Datum::destroy(datum, allocator);
///
/// Note the requirement to the destroy the resulting datum after the caller
/// is done with it.
///
/// @ingroup module_ntci_metrics
class Monitorable : public virtual Identifiable
{
  public:
    enum StatisticType {
        // This enumeration describes the semantics of the measurements and
        // the function of their aggregation.

        e_GAUGE,
        // The statistic represents the measurement of a continuous level
        // of resource usage.  This type is most suitable for measurements
        // of such resources as the current total number of socket
        // connections, or the virtual memory size of a process. When
        // statistics of this type are aggregated the result is the last
        // contribution.

        e_SUM,
        // The statistic represents the occurrence of an event. This type
        // is most suitable for measurements such as the number of accepted
        // sockets, or the number of bytes read from a socket.  When
        // statistics of this type are aggregated the result is the sum of
        // all contributions.

        e_MINIMUM,
        // The statistic represents the minimum of a number of measurements
        // over an interval. When statistics of this type are aggregated
        // the result is the minimum of the minimums over the aggregated
        // interval.

        e_MAXIMUM,
        // The statistic represents the maximum of a number of measurements
        // over an interval. When statistics of this type are aggregated
        // the result is the maximum of the maximums over the aggregated
        // interval.

        e_AVERAGE
        // The statistic represents the average of a number of measurements
        // over an interval. When statistics of this type are aggregated
        // the result is the sum of the averages divided by the number
        // of aggregated averages; i.e., the average of averages.
    };

    enum StatisticTag {
        // This enumeration defines the bit flags that indicate how a
        // statistic measured by a monitorable object should be tagged (i.e.
        // indexed.)

        e_ANONYMOUS = 0x00,
        // The statistic should not be indexed by the identity of the
        // monitorable object.

        e_GUID = 0x01,
        // The statistic should be indexed by the GUID of the monitorable
        // object.

        e_NAME = 0x02
        // The statistic should be indexed by the human readable name
        // assigned to the monitorable object, if any.
    };

    /// Destroy this object.
    virtual ~Monitorable();

    /// Load into the specified 'result' the array of statistics from the
    /// specified 'snapshot' for this object based on the specified
    /// 'operation': if 'operation' is e_CUMULATIVE then the statistics are
    /// for the entire life of this object;  otherwise the statistics are
    /// for the period since the last call to this function. If 'operation'
    /// is e_INTERVAL_WITH_RESET then reset all internal measurements.  Note
    /// that 'result->theArray().length()' is expected to have the same
    /// value each time this function returns.
    virtual void getStats(bdld::ManagedDatum* result) = 0;

    /// Return the prefix corresponding to the field at the specified
    /// 'ordinal' position, or 0 if no field at the 'ordinal' position
    /// exists.
    virtual const char* getFieldPrefix(int ordinal) const = 0;

    /// Return the field name corresponding to the field at the specified
    /// 'ordinal' position, or 0 if no field at the 'ordinal' position
    /// exists.
    virtual const char* getFieldName(int ordinal) const = 0;

    /// Return the field description corresponding to the field at the
    /// specified 'ordinal' position, or 0 if no field at the 'ordinal'
    /// position exists.
    virtual const char* getFieldDescription(int ordinal) const = 0;

    /// Return the type of the statistic at the specified 'ordinal'
    /// position, or e_AVERAGE if no field at the 'ordinal' position exists
    /// or the type is unknown.
    virtual StatisticType getFieldType(int ordinal) const = 0;

    /// Return the flags that indicate which indexes to apply to the
    /// statistics measured by this monitorable object.
    virtual int getFieldTags(int ordinal) const = 0;

    /// Return the ordinal of the specified 'fieldName', or a negative value
    /// if no field identified by 'fieldName' exists.
    virtual int getFieldOrdinal(const char* fieldName) const = 0;

    /// Return the maximum number of elements in a datum resulting from
    /// a call to 'getStats()'.
    virtual int numOrdinals() const = 0;

    /// Return the human-readable name of the monitorable object, or 0 or
    /// the empty string if no such human-readable name has been assigned to
    /// the monitorable object.
    virtual const char* objectName() const = 0;
};

/// Provide an interface to publish statistics measured by monitorable objects.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_metrics
class MonitorablePublisher
{
  public:
    /// Destroy this object.
    virtual ~MonitorablePublisher();

    /// Publish the specified 'statistics' collected from the specified
    /// 'monitorable' object at the specified 'time'. If the specified
    /// 'final' flag is true, these 'statistics' are the final statistics
    /// collected during the same sample at the 'time'.
    virtual void publish(const bsl::shared_ptr<ntci::Monitorable>& monitorable,
                         const bdld::Datum&                        statistics,
                         const bsls::TimeInterval&                 time,
                         bool                                      final) = 0;
};

/// Provide an interface to periodically collect metrics.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_metrics
class MonitorableCollector
{
  public:
    /// Destroy this object.
    virtual ~MonitorableCollector();

    /// Register the specified 'publisher' to publish statistics measured
    /// by each monitorable object.
    virtual void registerPublisher(
        const bsl::shared_ptr<ntci::MonitorablePublisher>& publisher) = 0;

    /// Deregister the specified 'publisher' to no longer publish statistics
    /// measured by each monitorable object.
    virtual void deregisterPublisher(
        const bsl::shared_ptr<ntci::MonitorablePublisher>& publisher) = 0;

    /// Start the background thread periodically collecting statistics.
    virtual void start() = 0;

    /// Stop the background thread periodically collecting statistics.
    virtual void stop() = 0;

    /// Force the collection of statistics from each monitorable object
    /// registered with the default monitorable object registry and publish
    /// their statistics through each registered publisher.
    virtual void collect() = 0;
};

/// Provide an interface for a registry of monitorable objects.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_metrics
class MonitorableRegistry
{
  public:
    /// Destroy this object.
    virtual ~MonitorableRegistry();

    /// Add the specified 'object' to this monitorable object registry.
    virtual void registerMonitorable(
        const bsl::shared_ptr<ntci::Monitorable>& object) = 0;

    /// Remove the specified 'object' from this monitorable object
    /// registry.
    virtual void deregisterMonitorable(
        const bsl::shared_ptr<ntci::Monitorable>& object) = 0;

    /// Append to the specified 'result' each currently registered
    /// monitorable object.
    virtual void loadRegisteredObjects(
        bsl::vector<bsl::shared_ptr<ntci::Monitorable> >* result) const = 0;
};

}  // close package namespace
}  // close enterprise namespace

#endif
