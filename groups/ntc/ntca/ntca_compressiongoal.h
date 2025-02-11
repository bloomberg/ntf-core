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

#ifndef INCLUDED_NTCA_COMPRESSIONGOAL
#define INCLUDED_NTCA_COMPRESSIONGOAL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntca {

/// Provide an enumeration of the compression goals.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_identity
class CompressionGoal 
{
  public:
    /// Provide an enumeration of the compression goals.
    enum Value {
        /// The compression goal is undefined.
        e_UNDEFINED = 0,

        /// The compression goal favors size over speed.
        e_BEST_SIZE = 1,

        /// The compression goal favors size, but not to the total detriment
        /// of speed.
        e_BETTER_SIZE = 2,

        /// The compression goal favors neither size nor speed, but tries to 
        /// achieve a good tradeoff for both.
        e_BALANCED = 3,

        /// The compression goal favors speed, but not to the total detriment
        /// of size.
        e_BETTER_SPEED = 4,

        /// The compression goal favors speed over size.
        e_BEST_SPEED = 5
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntvc::CompressionGoal
bsl::ostream& operator<<(bsl::ostream& stream, CompressionGoal::Value rhs);

}  // close package namespace
}  // close enterprise namespace
#endif
