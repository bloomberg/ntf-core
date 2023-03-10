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

#ifndef INCLUDED_NTCA_PROACTORCONFIG
#define INCLUDED_NTCA_PROACTORCONFIG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_drivermechanism.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntca {

/// Describe the configuration of a proactor.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b driverName:
/// The name of the implementation of the driver.
///
/// @li @b metricName:
/// The name of the metrics collected by the proactor.
///
/// @li @b minThreads:
/// The minimum number of threads expected to run the proactor.
///
/// @li @b maxThreads:
/// The maximum number of threads allowed to run the proactor.
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
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_proactor
class ProactorConfig
{
    bdlb::NullableValue<ntca::DriverMechanism> d_driverMechanism;
    bdlb::NullableValue<bsl::string>           d_driverName;
    bdlb::NullableValue<bsl::string>           d_metricName;
    bdlb::NullableValue<bsl::size_t>           d_minThreads;
    bdlb::NullableValue<bsl::size_t>           d_maxThreads;
    bdlb::NullableValue<bsl::size_t>           d_maxEventsPerWait;
    bdlb::NullableValue<bsl::size_t>           d_maxTimersPerWait;
    bdlb::NullableValue<bsl::size_t>           d_maxCyclesPerWait;
    bdlb::NullableValue<bool>                  d_metricCollection;
    bdlb::NullableValue<bool>                  d_metricCollectionPerWaiter;
    bdlb::NullableValue<bool>                  d_metricCollectionPerSocket;

  public:
    /// Create a new driver configuration. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit ProactorConfig(bslma::Allocator* basicAllocator = 0);

    /// Create  anew driver configuration having the same value as the
    /// specified 'original' driver configuration. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    ProactorConfig(const ProactorConfig& original,
                   bslma::Allocator*     basicAllocator = 0);

    /// Destroy this object.
    ~ProactorConfig();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ProactorConfig& operator=(const ProactorConfig& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the mechanism of the driver to the specified 'value'. The
    /// 'value' identifies an externally-created and owned mechanism,
    /// injected into this framework. The default value indicates that
    /// the required mechanisms for each driver are created and owned
    /// internally. It is unusual to set this value; typically it is only
    /// set to share the same underlying machinery between multiple
    /// distinct asynchronous frameworks.
    void setDriverMechanism(const ntca::DriverMechanism& value);

    /// Set the name of the driver implementation to the specified 'value'.
    /// Valid values are "select", "poll", "epoll", "devpoll", "eventport",
    /// "pollset", "kqueue", "iocp", "iouring", "asio", and the empty string
    /// to represent the default driver implementation for the current
    /// platform. Note that not all driver implementations are available on
    /// all platforms; consult the component documentation more details.
    void setDriverName(const bsl::string& value);

    /// Set the name of metrics collected by the proactor to the specified
    /// 'value'.
    void setMetricName(const bsl::string& value);

    /// Set the minimum number of threads expected to run the proactor to
    /// the specified 'value'.
    void setMinThreads(bsl::size_t value);

    /// Set the maximum number of threads allowed to run the proactor to the
    /// specified 'value'.
    void setMaxThreads(bsl::size_t value);

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

    /// Return the mechanism of the driver. The returned value identifies
    /// an externally-created and owned mechanism, injected into this
    /// framework. If the value is null, the required mechanisms for each
    /// driver are created and owned internally.
    const bdlb::NullableValue<ntca::DriverMechanism>& driverMechanism() const;

    /// Return the name of the driver implementation.
    const bdlb::NullableValue<bsl::string>& driverName() const;

    /// Return the name of metrics collected by the proactor.
    const bdlb::NullableValue<bsl::string>& metricName() const;

    /// Return the minimum number of threads expected to run the proactor.
    const bdlb::NullableValue<bsl::size_t>& minThreads() const;

    /// Return the maximum number of threads allowed to run the proactor.
    const bdlb::NullableValue<bsl::size_t>& maxThreads() const;

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

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ProactorConfig& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ProactorConfig& other) const;

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

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(ProactorConfig);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::ProactorConfig
bsl::ostream& operator<<(bsl::ostream& stream, const ProactorConfig& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ProactorConfig
bool operator==(const ProactorConfig& lhs, const ProactorConfig& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ProactorConfig
bool operator!=(const ProactorConfig& lhs, const ProactorConfig& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ProactorConfig
bool operator<(const ProactorConfig& lhs, const ProactorConfig& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ProactorConfig
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ProactorConfig& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ProactorConfig& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.driverName());
    hashAppend(algorithm, value.metricName());
    hashAppend(algorithm, value.minThreads());
    hashAppend(algorithm, value.maxThreads());
    hashAppend(algorithm, value.maxEventsPerWait());
    hashAppend(algorithm, value.maxTimersPerWait());
    hashAppend(algorithm, value.maxCyclesPerWait());
    hashAppend(algorithm, value.metricCollection());
    hashAppend(algorithm, value.metricCollectionPerWaiter());
    hashAppend(algorithm, value.metricCollectionPerSocket());
}

}  // close package namespace
}  // close enterprise namespace
#endif
