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

#ifndef INCLUDED_NTCA_MONITORABLECOLLECTORCONFIG
#define INCLUDED_NTCA_MONITORABLECOLLECTORCONFIG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntca {

/// Describe the configuration of a monitorable object collector.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b period:
/// The interval over which monitorable objects are automatically and
/// periodically collected by a separate thread and their statistics directed
/// to each registered publisher, in seconds. The default value is null,
/// indicating that monitorable objects are never automatically and
/// periodically collected; collection must be performed explicitly.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_metrics
class MonitorableCollectorConfig
{
    bdlb::NullableValue<bsl::string> d_threadName;
    bdlb::NullableValue<bsl::size_t> d_period;

  public:
    /// Create a new monitorable object collector configuration. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit MonitorableCollectorConfig(bslma::Allocator* basicAllocator = 0);

    /// Create a new monitorable object collector configuration having the
    /// same value as the specified 'original' driver configuration.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    MonitorableCollectorConfig(const MonitorableCollectorConfig& original,
                               bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~MonitorableCollectorConfig();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    MonitorableCollectorConfig& operator=(
        const MonitorableCollectorConfig& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the name of the thread that automatically and periodically
    /// collects monitorable objects to the specified 'value'.
    void setThreadName(const bsl::string& value);

    /// Set the interval over which monitorable objects are automatically
    /// and periodically collected to the specified 'value', in seconds.
    void setPeriod(bsl::size_t value);

    /// Return the name of the thread that automatically and periodically
    /// collects monitorable objects.
    const bdlb::NullableValue<bsl::string>& threadName() const;

    /// Return the interval over which monitorable objects are automatically
    /// and periodically collected, in seconds.
    const bdlb::NullableValue<bsl::size_t>& period() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const MonitorableCollectorConfig& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(MonitorableCollectorConfig);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::MonitorableCollectorConfig
bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const MonitorableCollectorConfig& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::MonitorableCollectorConfig
bool operator==(const MonitorableCollectorConfig& lhs,
                const MonitorableCollectorConfig& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::MonitorableCollectorConfig
bool operator!=(const MonitorableCollectorConfig& lhs,
                const MonitorableCollectorConfig& rhs);

}  // close package namespace
}  // close enterprise namespace
#endif
