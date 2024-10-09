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

#ifndef INCLUDED_NTSA_RESOLVERCONFIG
#define INCLUDED_NTSA_RESOLVERCONFIG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Describe the configuration of a blocking resolver.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b overridesEnabled:
/// The flag that indicates that any defined overrides should be first examined
/// when attempting a resolution. The default value is null, which indicates
/// that overrides are enabled.
///
/// @li @b systemEnabled:
/// The flag indicating that name resolution by blocking system calls is
/// enabled. The default value is null, which indicates a blocking system calls
/// made are enabled.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class ResolverConfig
{
    bdlb::NullableValue<bool> d_overridesEnabled;
    bdlb::NullableValue<bool> d_systemEnabled;

  public:
    /// Create a new resolver configuration.
    ResolverConfig();

    /// Create a new resolver configuration having the same value as the
    /// specified 'original' resolver configuration.
    ResolverConfig(const ResolverConfig& original);

    /// Destroy this object.
    ~ResolverConfig();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ResolverConfig& operator=(const ResolverConfig& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the flag indicating overrides are enabled to the specified 'value'.
    /// This flag indicates that any defined overrides should be first examined
    /// when attempting a resolution. The default value is null, which
    /// indicates that overrides are enabled.
    void setOverridesEnabled(bool value);

    /// Set the flag indicating that name resolution by blocking system calls
    /// is enabled to the specified 'value'. The default value is null, which
    /// indicates a blocking system calls are enabled.
    void setSystemEnabled(bool value);

    /// Return the flag indicating overrides are enabled. This flag indicates
    /// that any defined overrides should be first examined when attempting a
    /// resolution. The default value is null, which indicates that overrides
    /// are enabled.
    const bdlb::NullableValue<bool>& overridesEnabled() const;

    /// Return the flag indicating that name resolution by blocking system
    /// calls is enabled. The default value is null, which indicates a blocking
    /// system calls are enabled.
    const bdlb::NullableValue<bool>& systemEnabled() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ResolverConfig& other) const;

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

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(ResolverConfig);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(ResolverConfig);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::ResolverConfig
bsl::ostream& operator<<(bsl::ostream& stream, const ResolverConfig& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::ResolverConfig
bool operator==(const ResolverConfig& lhs, const ResolverConfig& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::ResolverConfig
bool operator!=(const ResolverConfig& lhs, const ResolverConfig& rhs);

}  // close package namespace
}  // close enterprise namespace
#endif
