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

#ifndef INCLUDED_NTCA_LOADBALANCINGOPTIONS
#define INCLUDED_NTCA_LOADBALANCINGOPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslmt_threadutil.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntca {

/// Describe the configuration of I/O load balancing.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b threadHandle:
/// The specific thread whose reactor or proactor should drive the I/O for the
/// socket or timer.
///
/// @li @b threadIndex:
/// The thread index, applied modulo the current number of I/O threads, of the
/// thread whose reactor or proactor should drive the I/O for the socket or
/// timer.
///
/// @li @b weight:
/// The estimated amount of load induced by the socket or timer.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_runtime
class LoadBalancingOptions
{
    bdlb::NullableValue<bslmt::ThreadUtil::Handle> d_threadHandle;
    bdlb::NullableValue<bsl::size_t>               d_threadIndex;
    bdlb::NullableValue<bsl::size_t>               d_weight;

  public:
    /// Create a new load balancing configuration having the default
    /// value.
    LoadBalancingOptions();

    /// Set the specific thread whose reactor or proactor should drive the
    /// I/O for the socket or timer to the specified 'threadHandle'.
    void setThreadHandle(bslmt::ThreadUtil::Handle threadHandle);

    /// Set the the thread index, applied modulo the current number of
    /// I/O threads, of the thread whose reactor or proactor should
    /// drive the I/O for the socket or timer to the specified
    /// 'threadIndex'.
    void setThreadIndex(bsl::size_t threadIndex);

    /// Set the estimated load induced by the socket or timer to the
    /// specified 'weight'.
    void setWeight(bsl::size_t weight);

    /// Return the specific thread whose reactor or proactor should drive
    /// the I/O for the socket or timer.
    const bdlb::NullableValue<bslmt::ThreadUtil::Handle>& threadHandle() const;

    /// Return the the thread index, applied modulo the current number of
    /// I/O threads, of the thread whose reactor or proactor should
    /// drive the I/O for the socket or timer.
    const bdlb::NullableValue<bsl::size_t>& threadIndex() const;

    /// Return the estimated load induced by the socket or timer to the
    /// specified 'weight'.
    const bdlb::NullableValue<bsl::size_t>& weight() const;

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
};

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::LoadBalancingOptions
bool operator==(const LoadBalancingOptions& lhs,
                const LoadBalancingOptions& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::LoadBalancingOptions
bool operator!=(const LoadBalancingOptions& lhs,
                const LoadBalancingOptions& rhs);

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::LoadBalancingOptions
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const LoadBalancingOptions& object);

}  // close package namespace
}  // close enterprise namespace
#endif
