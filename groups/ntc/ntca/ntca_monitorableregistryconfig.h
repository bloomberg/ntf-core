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

#ifndef INCLUDED_NTCA_MONITORABLEREGISTRYCONFIG
#define INCLUDED_NTCA_MONITORABLEREGISTRYCONFIG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe the configuration of a monitorable object registry.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b maxSize:
/// The maximum number of registered monitorable objects. The default value is
/// null, indicating that no limit is enforced.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_metrics
class MonitorableRegistryConfig
{
    bdlb::NullableValue<bsl::size_t> d_maxSize;

  public:
    /// Create a new monitorable object registry configuration. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit MonitorableRegistryConfig(bslma::Allocator* basicAllocator = 0);

    /// Create a new monitorable object registry configuration having the
    /// same value as the specified 'original' driver configuration.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    MonitorableRegistryConfig(const MonitorableRegistryConfig& original,
                              bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~MonitorableRegistryConfig();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    MonitorableRegistryConfig& operator=(
        const MonitorableRegistryConfig& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the maximum number of monitored objects to the specified
    /// 'value'.
    void setMaxSize(bsl::size_t value);

    /// Return the maximum number of monitored objects.
    const bdlb::NullableValue<bsl::size_t>& maxSize() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const MonitorableRegistryConfig& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(MonitorableRegistryConfig);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::MonitorableRegistryConfig
bsl::ostream& operator<<(bsl::ostream&                    stream,
                         const MonitorableRegistryConfig& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::MonitorableRegistryConfig
bool operator==(const MonitorableRegistryConfig& lhs,
                const MonitorableRegistryConfig& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::MonitorableRegistryConfig
bool operator!=(const MonitorableRegistryConfig& lhs,
                const MonitorableRegistryConfig& rhs);

}  // close package namespace
}  // close enterprise namespace
#endif
