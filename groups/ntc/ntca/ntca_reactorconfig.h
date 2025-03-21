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

#ifndef INCLUDED_NTCA_REACTORCONFIG
#define INCLUDED_NTCA_REACTORCONFIG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_drivermechanism.h>
#include <ntca_reactoreventtrigger.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntca {

/// Describe the configuration of a reactor.
///
/// @par Implementations
/// Underneath a reactor is a driver, which may be implemented by one of a
/// number of possible operating system APIs or third-party libraries. The
/// following table describes which driver names are supported on each
/// platform.
///
/// +-----------+-----------------------------------------------------------+
/// | AIX       | "select", "poll", "pollset" (default)                     |
/// +-----------+-----------------------------------------------------------+
/// | Darwin    | "select", "poll", "kqueue" (default)                      |
/// +-----------+-----------------------------------------------------------+
/// | FreeBSD   | "select", "poll", "kqueue" (default)                      |
/// +-----------+-----------------------------------------------------------+
/// | Linux     | "select", "poll", "epoll" (default)                       |
/// +-----------+-----------------------------------------------------------+
/// | Solaris   | "select", "poll", "devpoll", "eventport" (default)        |
/// +-----------+-----------------------------------------------------------+
/// | Windows   | "select", "poll" (default)                                |
/// +-----------+-----------------------------------------------------------+
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b driverName:
/// The name of the implementation of the driver.  Valid values are "select",
/// "poll", "epoll", "devpoll", "eventport", "pollset", "kqueue", and the empty
/// string to represent the default reactor implementation for the current
/// platform.
///
/// @li @b driverMechanism:
/// An externally-created and owned mechanism, which is to be injected into
/// this framework. The default value indicates that the required mechanisms
/// for each driver are created and owned internally. It is unusual to set this
/// value; typically it is only set to share the same underlying machinery
/// between multiple distinct asynchronous frameworks.
///
/// @li @b metricName:
/// The name of the metrics collected by the reactor.
///
/// @li @b minThreads:
/// The minimum number of threads expected to run the reactor.
///
/// @li @b maxThreads:
/// The maximum number of threads allowed to run the reactor.
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
/// instantaneously polling for socket events each time a batch of functions
/// are deferred or timers scheduled to be executed, if it is likely that no
/// socket events have occurred, at the possible expense of starving the I/O
/// thread from being able to process socket events that actually have
/// occurred. The default value is null, indicating that only one cycle is
/// performed.
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
/// @li @b autoAttach:
/// Automatically attach the socket to the reactor when interest in any event
/// for a socket is gained and the socket has not been explicitly attached. If
/// this flag is unset or false, sockets must be explicitly attached to the
/// reactor before being able to register interest in their events. The default
/// value is unset, or effectively false.
///
/// @li @b autoDetach:
/// Automatically detach the socket from the reactor when interest in all
/// events is lost. If this flag is unset or false, sockets must be explicitly
/// detached from the reactor. The default value is unset, or effectively
/// false.
///
/// @li @b oneShot:
/// Process events detected by the reactor in one-shot mode: once an event is
/// detected, it must be explicitly re-armed before being detected again. When
/// running the same reactor simultaneously by multiple threads, configuring
/// the reactor in one-shot mode is practically required. The default value is
/// unset, or effectively false when the reactor is driven by only one thread,
/// and effectively true when the reactor is driven by more than one thread.
///
/// @li @b trigger:
/// Specify the conditions that trigger events. When events are
/// level-triggered, the event will occur as long as the conditions for the
/// event continue to be satisfied. When events are edge-triggered, the event
/// is raised when conditions for the event change are first satisfied, but the
/// event is not subsequently raised until the conditions are "reset". The
/// default value is unset, or effectively for events to be level-triggered.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_reactor
class ReactorConfig
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
    bdlb::NullableValue<bool>                  d_autoAttach;
    bdlb::NullableValue<bool>                  d_autoDetach;
    bdlb::NullableValue<ntca::ReactorEventTrigger::Value> d_trigger;
    bdlb::NullableValue<bool>                             d_oneShot;

  public:
    /// Create a new driver configuration. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit ReactorConfig(bslma::Allocator* basicAllocator = 0);

    /// Create  anew driver configuration having the same value as the
    /// specified 'original' driver configuration. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    ReactorConfig(const ReactorConfig& original,
                  bslma::Allocator*    basicAllocator = 0);

    /// Destroy this object.
    ~ReactorConfig();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ReactorConfig& operator=(const ReactorConfig& other);

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

    /// Set the name of metrics collected by the reactor to the specified
    /// 'value'.
    void setMetricName(const bsl::string& value);

    /// Set the minimum number of threads expected to run the reactor to the
    /// specified 'value'.
    void setMinThreads(bsl::size_t value);

    /// Set the maximum number of threads allowed to run the reactor to the
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

    /// Set the flag that indicates a socket should be automatically
    /// attached to the reactor when interest in any event for a socket is
    /// gained to the specified 'value'.
    void setAutoAttach(bool value);

    /// Set the flag that indicates a socket should be automatically
    /// detached from the reactor when interest in all events for the socket
    /// is lost to the specified 'value'.
    void setAutoDetach(bool value);

    /// Set the trigger of events to the specified 'value'. When events are
    /// level-triggered, the event will occur as long as the conditions for
    /// the event continue to be satisfied. When events are edge-triggered,
    /// the event is raised when conditions for the event change are first
    /// satisfied, but the event is not subsequently raised until the
    /// conditions are "reset".
    void setTrigger(ntca::ReactorEventTrigger::Value value);

    /// Set the one-shot mode to the specified 'value'. When 'oneShot' mode
    /// is enabled, after a reactor detects the socket is readable or
    /// writable, interest in readability or writability must be explicitly
    /// re-registered before the reactor will again detect the socket is
    /// readable or writable.
    void setOneShot(bool value);

    /// Return the mechanism of the driver. The returned value identifies
    /// an externally-created and owned mechanism, injected into this
    /// framework. If the value is null, the required mechanisms for each
    /// driver are created and owned internally.
    const bdlb::NullableValue<ntca::DriverMechanism>& driverMechanism() const;

    /// Return the name of the driver implementation.
    const bdlb::NullableValue<bsl::string>& driverName() const;

    /// Return the name of metrics collected by the reactor.
    const bdlb::NullableValue<bsl::string>& metricName() const;

    /// Return the minimum number of threads expected to run the reactor.
    const bdlb::NullableValue<bsl::size_t>& minThreads() const;

    /// Return the maximum number of threads allowed to run the reactor.
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

    /// Return the flag that indicates a socket should be automatically
    /// attached to the reactor when interest in any event for a socket is
    /// gained.
    const bdlb::NullableValue<bool>& autoAttach() const;

    /// Return the flag that indicates a socket should be automatically
    /// detached from the reactor when interest in all events for the socket
    /// is lost.
    const bdlb::NullableValue<bool>& autoDetach() const;

    /// Return the trigger mode. When events are level-triggered, the event
    /// will occur as long as the conditions for the event continue to be
    /// satisfied. When events are edge-triggered, the event is raised when
    /// conditions for the event change are first satisfied, but the event
    /// is not subsequently raised until the conditions are "reset".
    const bdlb::NullableValue<ntca::ReactorEventTrigger::Value>& trigger()
        const;

    /// Return the one-shot mode. When 'oneShot' mode is enabled, after a
    /// reactor detects the socket is readable or writable, interest in
    /// readability or writability must be explicitly re-registered before
    /// the reactor will again detect the socket is readable or writable.
    const bdlb::NullableValue<bool>& oneShot() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ReactorConfig& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ReactorConfig& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(ReactorConfig);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::ReactorConfig
bsl::ostream& operator<<(bsl::ostream& stream, const ReactorConfig& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ReactorConfig
bool operator==(const ReactorConfig& lhs, const ReactorConfig& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ReactorConfig
bool operator!=(const ReactorConfig& lhs, const ReactorConfig& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ReactorConfig
bool operator<(const ReactorConfig& lhs, const ReactorConfig& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ReactorConfig
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReactorConfig& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReactorConfig& value)
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
    hashAppend(algorithm, value.autoAttach());
    hashAppend(algorithm, value.autoDetach());
    hashAppend(algorithm, value.trigger());
    hashAppend(algorithm, value.oneShot());
}

}  // close package namespace
}  // close enterprise namespace
#endif
