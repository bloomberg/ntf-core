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

#ifndef INCLUDED_NTCA_THREADCONFIG
#define INCLUDED_NTCA_THREADCONFIG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_drivermechanism.h>
#include <ntca_resolverconfig.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntca {

/// Provide the configuration of a thread.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b metricName:
/// The name of the metrics collected by the thread. If no metric name is
/// explicitly set, the metric name is derived from the thread name.
///
/// @li @b threadName:
/// The name of the thread. If no thread name is explicitly set, the thread
/// name is derived from the metric name.
///
/// @li @b driverName:
/// The name of the implementation of the driver.  Valid values are "select",
/// "poll", "epoll", "devpoll", "eventport", "pollset", "kqueue", "iocp",
/// "iouring", "asio", and the empty string to represent the default driver
/// implementation for the current platform.
///
/// @li @b maxEventsPerWait:
/// The maximum number of events to discover each time the polling mechanism is
/// polled. The default value is null, indicating the driver should select an
/// implementation-defined default value.
///
/// @li @b maxTimersPerWait:
/// The maximum number of timers to discover that are due after each time the
/// polling mechanism is polled. The default value is null, indicating the
/// maximum number of timers is unlimited.
///
/// @li @b maxCyclesPerWait:
/// The maximum number of cycles to perform to both discover if any functions
/// have had their execution deferred to be invoked on the I/O thread and to
/// discover any timers that are due. A higher value mitigates the cost of
/// instantanously polling for socket events each time a batch of functions are
/// deferred or timers scheduled to be executed, if it is likely that no socket
/// events have occured, at the possible expense of starving the I/O thread
/// from being able to process socket events that actually have occurred. The
/// default value is null, indicating that only one cycle is performed.
///
/// @li @b metricCollection:
/// The flag that indicates the collection of metrics is enabled or disabled.
///
/// @li @b metricCollectionPerWaiter:
/// The flag that indicates the collection of metrics per waiter is enabled or
/// disabled.
///
/// @li @b metricCollectionPerSocket:
/// The flag that indicates the collection of metrics per socket is enabled or
/// disabled.
///
/// @li @b resolverEnabled:
/// The flag that indicates this interface should run an asynchronous resolver.
/// The default value is null, indicating that a default resolver is *not* run.
///
/// @li @b resolverConfig:
/// The asynchronous resolver configuration. The default value is null,
/// indicating that when an asynchronous resolver is enabled it is configured
/// with the default configuration.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_runtime
class ThreadConfig
{
    bdlb::NullableValue<bsl::string>          d_metricName;
    bdlb::NullableValue<bsl::string>          d_threadName;
    bdlb::NullableValue<bsl::string>          d_driverName;
    bdlb::NullableValue<bsl::size_t>          d_maxEventsPerWait;
    bdlb::NullableValue<bsl::size_t>          d_maxTimersPerWait;
    bdlb::NullableValue<bsl::size_t>          d_maxCyclesPerWait;
    bdlb::NullableValue<bool>                 d_metricCollection;
    bdlb::NullableValue<bool>                 d_metricCollectionPerWaiter;
    bdlb::NullableValue<bool>                 d_metricCollectionPerSocket;
    bdlb::NullableValue<bool>                 d_resolverEnabled;
    bdlb::NullableValue<ntca::ResolverConfig> d_resolverConfig;

  public:
    /// Create a new driver configuration. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit ThreadConfig(bslma::Allocator* basicAllocator = 0);

    /// Create  anew driver configuration having the same value as the
    /// specified 'original' driver configuration. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    ThreadConfig(const ThreadConfig& original,
                 bslma::Allocator*   basicAllocator = 0);

    /// Destroy this object.
    ~ThreadConfig();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ThreadConfig& operator=(const ThreadConfig& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the name of metrics collected by the thread to the specified
    /// 'value'. If no metric name is explicitly set, the metric name is
    /// derived from the thread name.
    void setMetricName(const bsl::string& value);

    /// Set the name of the thread to the specified 'value'. If no thread
    /// name is explicitly set, the thread name is derived from the metric
    /// name.
    void setThreadName(const bsl::string& value);

    /// Set the name of the driver implementation to the specified 'value'.
    /// Valid values are "select", "poll", "epoll", "devpoll", "eventport",
    /// "pollset", "kqueue", "iocp", "iouring", "asio", and the empty string
    /// to represent the default driver implementation for the current
    /// platform. Note that not all driver implementations are available on
    /// all platforms; consult the component documentation more details.
    void setDriverName(const bsl::string& value);

    /// Set the maximum number of events to discover each time the polling
    /// mechanism is polled.
    void setMaxEventsPerWait(bsl::size_t value);

    /// Set the maximum number of timers to discover that are due after each
    /// time the polling mechanism is polled to the specified 'value'.
    void setMaxTimersPerWait(bsl::size_t value);

    /// Set the maximum number of cycles to perform to both discover if any
    /// functions have had their execution deferred to be invoked on the
    /// I/O thread and to discover any timers that are due to the specified
    /// 'value'.
    void setMaxCyclesPerWait(bsl::size_t value);

    /// Set the collection of metrics to be enabled or disabled according
    /// to the specified 'value'.
    void setMetricCollection(bool value);

    /// Set the collection of metrics per waiter to be enabled or disabled
    /// according to the specified 'value'.
    void setMetricCollectionPerWaiter(bool value);

    /// Set the collection of metrics per socket to be enabled or disabled
    /// according to the specified 'value'.
    void setMetricCollectionPerSocket(bool value);

    /// Set the flag that indicates this interface should run an
    /// asynchronous resolver to the specified 'value'. The default value is
    /// null, indicating that a default resolver is *not* run.
    void setResolverEnabled(bool value);

    /// Set the asynchronous resolver configuration to the specified
    /// 'value'. The default value is null, indicating that when an
    /// asynchronous resolver is enabled it is configured with the default
    /// configuration.
    void setResolverConfig(const ntca::ResolverConfig& value);

    /// Return the name of metrics collected by the thread. If no metric
    /// name is explicitly set, the metric name is derived from the thread
    /// name.
    const bdlb::NullableValue<bsl::string>& metricName() const;

    /// Return the name of the thread. If no thread name is explicitly set,
    /// the thread name is derived from the metric name.
    const bdlb::NullableValue<bsl::string>& threadName() const;

    /// Return the name of the driver implementation.
    const bdlb::NullableValue<bsl::string>& driverName() const;

    /// Return the maximum number of events to discover each time
    /// the polling mechanism is polled. If the value is null,
    /// the driver should select an implementation-defined default value.
    const bdlb::NullableValue<bsl::size_t>& maxEventsPerWait() const;

    /// Return the maximum number of timers to discover that are due after
    /// each time the polling mechanism is polled. If the value is null, the
    /// maximum number of timers is unlimited.
    const bdlb::NullableValue<bsl::size_t>& maxTimersPerWait() const;

    /// Return the maximum number of cycles to perform to both discover if
    /// any functions have had their execution deferred to be invoked on the
    /// I/O thread and to discover any timers that are due. If the value is
    /// null, only one cycle is performed.
    const bdlb::NullableValue<bsl::size_t>& maxCyclesPerWait() const;

    /// Return the flag that indicates the collection of metrics is enabled
    /// or disabled.
    const bdlb::NullableValue<bool>& metricCollection() const;

    /// Return the flag that indicates the collection of metrics per waiter
    /// is enabled or disabled.
    const bdlb::NullableValue<bool>& metricCollectionPerWaiter() const;

    /// Return the flag that indicates the collection of metrics per waiter
    /// is enabled or disabled.
    const bdlb::NullableValue<bool>& metricCollectionPerSocket() const;

    /// Return the flag that indicates this interface should run an
    /// asynchronous resolver. The default value is null, indicating that a
    /// default resolver is *not* run.
    const bdlb::NullableValue<bool>& resolverEnabled() const;

    /// Return the asynchronous resolver configuration. The default value is
    /// null, indicating that when an asynchronous resolver is enabled it is
    /// configured with the default configuration.
    const bdlb::NullableValue<ntca::ResolverConfig>& resolverConfig() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ThreadConfig& other) const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(ThreadConfig);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::ThreadConfig
bsl::ostream& operator<<(bsl::ostream& stream, const ThreadConfig& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ThreadConfig
bool operator==(const ThreadConfig& lhs, const ThreadConfig& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ThreadConfig
bool operator!=(const ThreadConfig& lhs, const ThreadConfig& rhs);

}  // close package namespace
}  // close enterprise namespace
#endif
